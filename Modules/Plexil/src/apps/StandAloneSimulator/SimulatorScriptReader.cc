/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "SimulatorScriptReader.hh"
#include "Simulator.hh"
#include "timeval-utils.hh"
#include "ResponseMessageManager.hh"
#include "ResponseBase.hh"
#include "ResponseFactory.hh"
#include "TelemetryResponseManager.hh"

#include "Debug.hh"

#include <fstream>
#include <iostream>
#include <sstream>

SimulatorScriptReader::SimulatorScriptReader(ResponseManagerMap& map,
					     ResponseFactory& factory)
  : m_map(map),
    m_factory(factory)
{
}

SimulatorScriptReader::~SimulatorScriptReader()
{
}

bool SimulatorScriptReader::readCommandScript(const std::string& fName)
{
  return readScript(fName);
}

bool SimulatorScriptReader::readTelemetryScript(const std::string& fName)
{
  return readScript(fName, true);
}

bool SimulatorScriptReader::readScript(const std::string& fName,
                                       bool telemetry)
{
  std::ifstream inputFile(fName.c_str());
  
  if (!inputFile) {
	std::cerr << "Error: cannot open script file \"" << fName << "\"" << std::endl;
	return false;
  }

  unsigned int lineCount = 0;

  const int MAX_INPUT_LINE_LENGTH = 1024;
  
  while (!inputFile.eof()) {
	char inLine[MAX_INPUT_LINE_LENGTH];
      
	// Get first line of response
	// Skip lines with non-alpha leading characters.
	do {
	  inputFile.getline(inLine, MAX_INPUT_LINE_LENGTH);
	  lineCount++;
	}	
	while (!inputFile.eof() && !isalpha(inLine[0]));

	if (inputFile.eof()) {
	  return true;
	} 
      
	std::istringstream inputStringStream(inLine);
      
	std::string commandName;
	inputStringStream >> commandName;

	// Check for mode lines
	if (commandName == "BEGIN_TELEMETRY") {
	  telemetry = true;
	  continue;
	}
	else if (commandName == "BEGIN_COMMANDS") {
	  telemetry = false;
	  continue;
	}

	unsigned long commandIndex;
	unsigned int numOfResponses;
	double delay;

	ResponseMessageManager* responseMessageManager =
	  ensureResponseMessageManager(commandName, telemetry);

	if (telemetry) {
		// Telemetry index is strictly sequential starting from 0
		commandIndex = responseMessageManager->getCounter();
		numOfResponses = 1;
		inputStringStream >> delay;
	  }
	else {
		inputStringStream >> commandIndex;
		inputStringStream >> numOfResponses;
		inputStringStream >> delay;
	  }

	debugMsg("SimulatorScriptReader:readScript", " Read a new line for \"" << commandName
			 << "\", delay = " << delay);

	// Get successive responses
	inputFile.getline(inLine, MAX_INPUT_LINE_LENGTH);
	lineCount++;

	if (inputFile.eof()) {
		std::cerr << "Error: response line missing in script-file " << fName 
				  << " at line " << lineCount << std::endl;
		return false;
	  }
      
	ResponseBase* response = 
	  m_factory.parseResponseValues(commandName, inLine, lineCount);
      
	debugMsg("SimulatorScriptReader:readScript",
			 " Command Index: " << commandIndex);
	if (response != NULL) {
		timeval timeDelay = doubleToTimeval(delay);
		response->setDelay(timeDelay);
		response->setNumberOfResponses(numOfResponses);
		responseMessageManager->addResponse(response, commandIndex);
	  }
	else {
		std::cerr << "ERROR: Unable to parse response for \""
				  << commandName
				  << "\" at line "
				  << lineCount
				  << "\nResponse line was: "
				  << inLine
				  << std::endl;
		return false;
	  }
  }
  
  inputFile.close();

  return true;
}

ResponseMessageManager* 
SimulatorScriptReader::ensureResponseMessageManager(const std::string& name,
						    bool telemetry)
{
  debugMsg("SimulatorScriptReader:ensureResponseMessageManager",
	   " " << name << ", " << (telemetry ? "true" : "false"));
  ResponseManagerMap::const_iterator it = m_map.find(name);
  if (it != m_map.end())
    {
      debugMsg("SimulatorScriptReader:ensureResponseMessageManager",
	       " " << name << " exists");
      // TODO: check whether it's a telemetry manager
      return it->second;
    }
  // TODO: get appropriate class!
  ResponseMessageManager* result =
    constructResponseMessageManager(name, telemetry);
  m_map[name] = result;
  return result;
}

ResponseMessageManager* 
SimulatorScriptReader::constructResponseMessageManager(const std::string& name,
						       bool telemetry)
{
   if (telemetry)
    return new TelemetryResponseManager(name);
  else
    return new ResponseMessageManager(name);
}

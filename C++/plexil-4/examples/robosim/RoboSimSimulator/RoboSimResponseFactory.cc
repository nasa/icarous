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

#include "RoboSimResponseFactory.hh"
#include "GenericResponse.hh"
#include "simdefs.hh"

#include "Debug.hh"

#include <sstream>
#include <iostream>

RoboSimResponseFactory::RoboSimResponseFactory()
{
}

RoboSimResponseFactory::~RoboSimResponseFactory()
{
}

ResponseBase* RoboSimResponseFactory::parseResponseValues(const std::string& cmdName,
							  const std::string& line,
							  unsigned int lineCount)
{
  std::istringstream inStr(line);
  std::vector<double> values;
  if (cmdName == "RobotState")
    {
      const size_t NUMBER_OF_STATE_READINGS=3;
      for (size_t i = 0; i < NUMBER_OF_STATE_READINGS; ++i)
        {
          double eLevel;
          if (parseType<double>(inStr, eLevel))
            values.push_back(eLevel);
          else
            break;
        }
      if (values.size() != NUMBER_OF_STATE_READINGS)
	{
	  std::cerr << "Line " << lineCount << ": unable to parse return value for \""
		    << cmdName << "\"" << std::endl;
	  return NULL;
	}
    }
  else if (cmdName == "QueryEnergySensor")
    {
      const size_t NUMBER_OF_ENERGY_LEVEL_READINGS=5;
      for (size_t i = 0; i < NUMBER_OF_ENERGY_LEVEL_READINGS; ++i)
        {
          double eLevel;
          if (parseType<double>(inStr, eLevel))
            values.push_back(eLevel);
          else
            break;
        }
      if (values.size() != NUMBER_OF_ENERGY_LEVEL_READINGS)
	{
	  std::cerr << "Line " << lineCount << ": unable to parse return value for \""
		    << cmdName << "\"" << std::endl;
	  return NULL;
	}
    }
  else if ((cmdName == "MoveUp")
	   || (cmdName == "MoveRight")
	   || (cmdName == "MoveDown")
	   || (cmdName == "MoveLeft"))
    {
      int returnValue;
      if (parseType<int>(inStr, returnValue))
	{
	  values.push_back((double) returnValue);
	}
      else
	{
	  std::cerr << "Line " << lineCount << ": unable to parse return value for \""
		    << cmdName << "\"" << std::endl;
	  return NULL;
	}
    }
  else
    {
      // Default case
      std::vector<double> values;
      while (!inStr.eof())
	{
	  double retVal;
	  if (parseType<double>(inStr, retVal))
	    {
	      values.push_back(retVal);
	    }
	  else if (!inStr.eof())
	    {
	      std::cerr << "Line " << lineCount << ": unrecognized return value format for \""
			<< cmdName << "\"" << std::endl;
	      return NULL;
	    }
	}
    }
  debugMsg("RoboSimResponseFactory:parse", 
	   " Returning new GenericResponse with " << values.size() << " values");
  std::vector<PLEXIL::Value> vals;
  for (std::vector<double>::const_iterator i = values.begin(); i != values.end(); i++) {
	vals.push_back(*i);
  }
  return new GenericResponse(vals);
}

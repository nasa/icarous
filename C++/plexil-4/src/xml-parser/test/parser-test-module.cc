/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

#include "Debug.hh"
#include "Expressions.hh"
#include "lifecycle-utils.h"
#include "SymbolTable.hh"
#include "TestSupport.hh"

#include <cstring>
#include <fstream>
#include <iostream>

extern bool arrayReferenceXmlParserTest();
extern bool constantXmlParserTest();
extern bool variableXmlParserTest();
extern bool functionXmlParserTest();
extern bool commandXmlParserTest();
extern bool lookupXmlParserTest();
extern bool updateXmlParserTest();
extern bool nodeXmlParserTest();

using PLEXIL::g_symbolTable;

void runTests()
{
  // Initialize factories
  PLEXIL::initializeExpressions();

  // Construct symbol table
  g_symbolTable = PLEXIL::makeSymbolTable();

  // Initialize infrastructure
  PLEXIL::PlanError::doThrowExceptions();

  // Expressions
  runTestSuite(constantXmlParserTest);
  runTestSuite(variableXmlParserTest);
  runTestSuite(arrayReferenceXmlParserTest);
  runTestSuite(functionXmlParserTest);

  // External interface
  runTestSuite(commandXmlParserTest);
  runTestSuite(lookupXmlParserTest);
  runTestSuite(updateXmlParserTest);

  // Nodes
  runTestSuite(nodeXmlParserTest);

  // Clean up
  delete g_symbolTable;
  g_symbolTable = NULL;

  plexilRunFinalizers();

  std::cout << "Finished" << std::endl;
}

int main(int argc, char *argv[]) 
{
  {
    std::string debugConfig("Debug.cfg");
  
    for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-d") == 0)
	debugConfig = std::string(argv[++i]);
    }
  
    std::ifstream config(debugConfig.c_str());
    if (config.good()) {
      PLEXIL::readDebugConfigStream(config);
      std::cout << "Reading configuration file " << debugConfig.c_str() << "\n";
    }
    else
      std::cout << "Warning: unable to read configuration file " << debugConfig.c_str() << "\n";
  }
  runTests();

  return 0;
}

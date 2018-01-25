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

#include "ExpressionFactory.hh"
#include "Command.hh"
#include "commandXmlParser.hh"
#include "test/FactoryTestNodeConnector.hh"
#include "TestSupport.hh"
#include "test/TrivialNodeConnector.hh"
#include "UserVariable.hh"

#include "pugixml.hpp"

using namespace PLEXIL;

using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

static bool testCommandParserBasics()
{
  FactoryTestNodeConnector conn;
  Expression *flagVar = new BooleanVariable(&conn, "flag");
  conn.storeVariable("flag", flagVar);

  xml_document doc;

  // Minimum case
  {
    xml_node simpleXml = doc.append_child("Command");
    simpleXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("foo");
  
    Command *simple = constructCommand(&conn, simpleXml);
    assertTrue_1(simple);

    finalizeCommand(simple, &conn, simpleXml);
    assertTrue_1(!simple->getDest());
    simple->activate();
    simple->fixValues();
    assertTrue_1(simple->getName() == "foo");
    assertTrue_1(simple->getArgValues().empty());
    assertTrue_1(simple->getCommand() == State("foo"));
    simple->fixResourceValues();
    assertTrue_1(simple->getResourceValues().empty());
    delete simple;
  }

  // Empty arglist
  {
    xml_node emptyXml = doc.append_child("Command");
    emptyXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("empty");
    emptyXml.append_child("Arguments");
    Command *empty = constructCommand(&conn, emptyXml);
    assertTrue_1(empty);
    finalizeCommand(empty, &conn, emptyXml);
    assertTrue_1(!empty->getDest());
    empty->activate();
    empty->fixValues();
    assertTrue_1(empty->getName() == "empty");
    assertTrue_1(empty->getArgValues().empty());
    assertTrue_1(empty->getCommand() == State("empty"));
    empty->fixResourceValues();
    assertTrue_1(empty->getResourceValues().empty());
    delete empty;
  }

  // Arguments
  {
    xml_node arghXml = doc.append_child("Command");
    arghXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("argh");
    arghXml.append_child("Arguments").append_child("IntegerValue").append_child(node_pcdata).set_value("0");
    Command *argh = constructCommand(&conn, arghXml);
    assertTrue_1(argh);
    finalizeCommand(argh, &conn, arghXml);
    assertTrue_1(!argh->getDest());
    argh->activate();
    argh->fixValues();
    assertTrue_1(argh->getName() == "argh");
    assertTrue_1(argh->getArgValues().size() == 1);
    State arghState = State("argh", 1);
    arghState.setParameter(0, Value((int32_t) 0));
    assertTrue_1(argh->getCommand() == arghState);
    argh->fixResourceValues();
    assertTrue_1(argh->getResourceValues().empty());
    delete argh;
  }

  // Return value
  {
    xml_node resultantXml = doc.append_child("Command");
    resultantXml.append_child("BooleanVariable").append_child(node_pcdata).set_value("flag");
    resultantXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("resultant");
    Command *resultant = constructCommand(&conn, resultantXml);
    assertTrue_1(resultant);
    finalizeCommand(resultant, &conn, resultantXml);
    assertTrue_1(resultant->getDest() == flagVar);
    resultant->activate();
    resultant->fixValues();
    assertTrue_1(resultant->getName() == "resultant");
    assertTrue_1(resultant->getArgValues().empty());
    assertTrue_1(resultant->getCommand() == State("resultant"));
    resultant->fixResourceValues();
    assertTrue_1(resultant->getResourceValues().empty());
    delete resultant;
  }

  // Empty resource
  {
    xml_node resourcelessXml = doc.append_child("Command");
    resourcelessXml.append_child("ResourceList");
    resourcelessXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("resourceless");
    Command *resourceless = constructCommand(&conn, resourcelessXml);
    assertTrue_1(resourceless);
    finalizeCommand(resourceless, &conn, resourcelessXml);
    assertTrue_1(!resourceless->getDest());
    resourceless->activate();
    resourceless->fixValues();
    assertTrue_1(resourceless->getName() == "resourceless");
    assertTrue_1(resourceless->getArgValues().empty());
    assertTrue_1(resourceless->getCommand() == State("resourceless"));
    resourceless->fixResourceValues();
    assertTrue_1(resourceless->getResourceValues().empty());
    delete resourceless;
  }

  // Non-empty resource
  {
    xml_node resourcefulXml = doc.append_child("Command");
    xml_node resourcefulList = resourcefulXml.append_child("ResourceList");
    xml_node resource = resourcefulList.append_child("Resource");
    resource.append_child("ResourceName").append_child("StringValue").append_child(node_pcdata).set_value("a");
    resource.append_child("ResourcePriority").append_child("IntegerValue").append_child(node_pcdata).set_value("0");
    resourcefulXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("resourceful");
    Command *resourceful = constructCommand(&conn, resourcefulXml);
    assertTrue_1(resourceful);
    finalizeCommand(resourceful, &conn, resourcefulXml);
    assertTrue_1(!resourceful->getDest());
    resourceful->activate();
    resourceful->fixValues();
    assertTrue_1(resourceful->getName() == "resourceful");
    assertTrue_1(resourceful->getArgValues().empty());
    assertTrue_1(resourceful->getCommand() == State("resourceful"));
    resourceful->fixResourceValues();
    ResourceValueList const &rlist = resourceful->getResourceValues();
    assertTrue_1(rlist.size() == 1);
    ResourceValue const &rmap = rlist.front();
    assertTrue_1(rmap.name == "a");
    assertTrue_1(rmap.priority == 0);
    delete resourceful;
  }

  // Non-empty resource with return value
  {
    xml_node remorsefulXml = doc.append_child("Command");
    xml_node remorsefulList = remorsefulXml.append_child("ResourceList");
    xml_node remorse = remorsefulList.append_child("Resource");
    remorse.append_child("ResourceName").append_child("StringValue").append_child(node_pcdata).set_value("a");
    remorse.append_child("ResourcePriority").append_child("IntegerValue").append_child(node_pcdata).set_value("1");
    remorsefulXml.append_child("BooleanVariable").append_child(node_pcdata).set_value("flag");
    remorsefulXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("remorseful");

    Command *remorseful = constructCommand(&conn, remorsefulXml);
    assertTrue_1(remorseful);
    finalizeCommand(remorseful, &conn, remorsefulXml);
    assertTrue_1(remorseful->getDest() == flagVar);
    remorseful->activate();
    remorseful->fixValues();
    assertTrue_1(remorseful->getName() == "remorseful");
    assertTrue_1(remorseful->getArgValues().empty());
    assertTrue_1(remorseful->getCommand() == State("remorseful"));
    remorseful->fixResourceValues();
    ResourceValueList const &slist = remorseful->getResourceValues();
    assertTrue_1(slist.size() == 1);
    ResourceValue const &smap = slist.front();
    assertTrue_1(smap.name == "a");
    assertTrue_1(smap.priority == 1);
    delete remorseful;
  }

  // Kitchen sink
  {
    xml_node regretfulXml = doc.append_child("Command");
    xml_node regretfulList = regretfulXml.append_child("ResourceList");
    xml_node regret = regretfulList.append_child("Resource");
    regret.append_child("ResourceName").append_child("StringValue").append_child(node_pcdata).set_value("b");
    regret.append_child("ResourcePriority").append_child("IntegerValue").append_child(node_pcdata).set_value("2");
    regretfulXml.append_child("BooleanVariable").append_child(node_pcdata).set_value("flag");
    regretfulXml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("regretful");
    regretfulXml.append_child("Arguments").append_child("BooleanValue").append_child(node_pcdata).set_value("true");
    Command *regretful = constructCommand(&conn, regretfulXml);
    assertTrue_1(regretful);
    finalizeCommand(regretful, &conn, regretfulXml);
    assertTrue_1(regretful->getDest() == flagVar);
    regretful->activate();
    regretful->fixValues();
    assertTrue_1(regretful->getName() == "regretful");
    assertTrue_1(regretful->getArgValues().size() == 1);
    assertTrue_1(regretful->getArgValues().front() == Value(true));
    State regretCmd = State("regretful", 1);
    regretCmd.setParameter(0, Value(true));
    assertTrue_1(regretful->getCommand() == regretCmd);
    regretful->fixResourceValues();
    ResourceValueList const &tlist = regretful->getResourceValues();
    assertTrue_1(tlist.size() == 1);
    ResourceValue const &tmap = tlist.front();
    assertTrue_1(tmap.name == "b");
    assertTrue_1(tmap.priority == 2);
    delete regretful;
  }

  return true;
}

static bool testCommandParserErrorHandling()
{
  TrivialNodeConnector conn;

  xml_document doc;

  xml_node mtCmd = doc.append_child("Command");
  try {
    Command *mtCmdCmd = constructCommand(&conn, mtCmd);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect empty Command element");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Empty name
  xml_node mtName = doc.append_child("Command");
  mtName.append_child("Name");
  try {
    Command *mtNameCmd = constructCommand(&conn, mtName);
    assertTrue_2(ALWAYS_FAIL, "Failed to detect empty Name element");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Name is not a string
  xml_node wrongTypeName = doc.append_child("Command");
  wrongTypeName.append_child("Name").append_child("RealValue").append_child(node_pcdata).set_value("3.14");
  {
    Command *wrongTypeNameCmd = constructCommand(&conn, wrongTypeName);
    try {
      finalizeCommand(wrongTypeNameCmd, &conn, wrongTypeName);
      assertTrue_2(ALWAYS_FAIL, "Failed to detect non-string Name value");
    }
    catch (ParserException const & /* exc */) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete wrongTypeNameCmd;
  }

  // Invalid return expression
  xml_node invalidReturn = doc.append_child("Command");
  invalidReturn.append_child("StringValue").append_child(node_pcdata).set_value("illegal");
  invalidReturn.append_child("Name").append_child(node_pcdata).set_value("legal");
  // Do it this way because we may not detect error in first pass
  {
    Command *invalidReturnCmd = NULL;
    try {
      invalidReturnCmd = constructCommand(&conn, invalidReturn);
      finalizeCommand(invalidReturnCmd, &conn, invalidReturn);
      assertTrue_2(ALWAYS_FAIL, "Failed to detect invalid return expression");
    }
    catch (ParserException const & /* exc */) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete invalidReturnCmd;
  }

  return true;
}

bool commandXmlParserTest()
{
  runTest(testCommandParserBasics);
  runTest(testCommandParserErrorHandling);

  return true;
}

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
#include "test/FactoryTestNodeConnector.hh"
#include "Lookup.hh"
#include "SymbolTable.hh"
#include "TestSupport.hh"

#include "pugixml.hpp"

#include <cstring>

using namespace PLEXIL;

using pugi::xml_attribute;
using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

static bool testBasics()
{
  FactoryTestNodeConnector conn;
  bool wasCreated = false;

  xml_document doc;

  // Basics
  {
    xml_node test1Xml = doc.append_child("LookupNow");
    test1Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("foo");

    Expression *lookup1 = createExpression(test1Xml, &conn, wasCreated);
    assertTrue_1(lookup1);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup1->exprName(), "LookupNow"));
    delete lookup1;
  }

  {
    xml_node test2Xml = doc.append_child("LookupOnChange");
    test2Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("bar");
    test2Xml.append_child("Tolerance").append_child("RealValue").append_child(node_pcdata).set_value("0.5");
    test2Xml.append_child("Arguments").append_child("IntegerValue").append_child(node_pcdata).set_value("0");

    wasCreated = false;
    Expression *lookup2 = createExpression(test2Xml, &conn, wasCreated);
    assertTrue_1(lookup2);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup2->exprName(), "LookupOnChange"));
    delete lookup2;
  }

  {
    xml_node test3Xml = doc.append_child("LookupOnChange");
    test3Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("baz");
    xml_node argsXml = test3Xml.append_child("Arguments");
    argsXml.append_child("IntegerValue").append_child(node_pcdata).set_value("1");
    argsXml.append_child("RealValue").append_child(node_pcdata).set_value("1.5");
    argsXml.append_child("StringValue").append_child(node_pcdata).set_value("too");

    wasCreated = false;
    Expression *lookup3 = createExpression(test3Xml, &conn, wasCreated);
    assertTrue_1(lookup3);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup3->exprName(), "LookupNow"));
    delete lookup3;
  }

  return true;
}

static bool testDeclaredReturnTypes()
{
  // Set up symbol table
  {
    Symbol *returnsBoolean = g_symbolTable->addLookup("returnsBoolean");
    returnsBoolean->setReturnType(BOOLEAN_TYPE);

    Symbol *returnsInteger = g_symbolTable->addLookup("returnsInteger");
    returnsInteger->setReturnType(INTEGER_TYPE);

    Symbol *returnsReal = g_symbolTable->addLookup("returnsReal");
    returnsReal->setReturnType(REAL_TYPE);

    Symbol *returnsString = g_symbolTable->addLookup("returnsString");
    returnsString->setReturnType(STRING_TYPE);

    Symbol *returnsBooleanArray = g_symbolTable->addLookup("returnsBooleanArray");
    returnsBooleanArray->setReturnType(BOOLEAN_ARRAY_TYPE);

    Symbol *returnsIntegerArray = g_symbolTable->addLookup("returnsIntegerArray");
    returnsIntegerArray->setReturnType(INTEGER_ARRAY_TYPE);

    Symbol *returnsRealArray = g_symbolTable->addLookup("returnsRealArray");
    returnsRealArray->setReturnType(REAL_ARRAY_TYPE);

    Symbol *returnsStringArray = g_symbolTable->addLookup("returnsStringArray");
    returnsStringArray->setReturnType(STRING_ARRAY_TYPE);
  }

  // Other setup
  FactoryTestNodeConnector conn;
  bool wasCreated = false;

  xml_document doc;

  // Test basics
  {
    xml_node test0Xml = doc.append_child("LookupNow");
    test0Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("unknownLookup");

    Expression *lookup0 = createExpression(test0Xml, &conn, wasCreated);
    assertTrue_1(lookup0);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup0->exprName(), "LookupNow"));
    assertTrue_1(lookup0->valueType() == UNKNOWN_TYPE);

    delete lookup0;
  }

  {
    xml_node test1Xml = doc.append_child("LookupNow");
    test1Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsBoolean");

    Expression *lookup1 = createExpression(test1Xml, &conn, wasCreated);
    assertTrue_1(lookup1);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup1->exprName(), "LookupNow"));
    assertTrue_1(lookup1->valueType() == BOOLEAN_TYPE);

    delete lookup1;
  }

  {
    xml_node test2Xml = doc.append_child("LookupNow");
    test2Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsInteger");

    Expression *lookup2 = createExpression(test2Xml, &conn, wasCreated);
    assertTrue_1(lookup2);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup2->exprName(), "LookupNow"));
    assertTrue_1(lookup2->valueType() == INTEGER_TYPE);

    delete lookup2;
  }

  {
    xml_node test3Xml = doc.append_child("LookupNow");
    test3Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsReal");

    Expression *lookup3 = createExpression(test3Xml, &conn, wasCreated);
    assertTrue_1(lookup3);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup3->exprName(), "LookupNow"));
    assertTrue_1(lookup3->valueType() == REAL_TYPE);

    delete lookup3;
  }

  {
    xml_node test4Xml = doc.append_child("LookupNow");
    test4Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsString");

    Expression *lookup4 = createExpression(test4Xml, &conn, wasCreated);
    assertTrue_1(lookup4);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup4->exprName(), "LookupNow"));
    assertTrue_1(lookup4->valueType() == STRING_TYPE);

    delete lookup4;
  }

  {
    xml_node test5Xml = doc.append_child("LookupNow");
    test5Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsBooleanArray");

    Expression *lookup5 = createExpression(test5Xml, &conn, wasCreated);
    assertTrue_1(lookup5);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup5->exprName(), "LookupNow"));
    assertTrue_1(lookup5->valueType() == BOOLEAN_ARRAY_TYPE);

    delete lookup5;
  }

  {
    xml_node test6Xml = doc.append_child("LookupNow");
    test6Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsIntegerArray");

    Expression *lookup6 = createExpression(test6Xml, &conn, wasCreated);
    assertTrue_1(lookup6);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup6->exprName(), "LookupNow"));
    assertTrue_1(lookup6->valueType() == INTEGER_ARRAY_TYPE);

    delete lookup6;
  }

  {
    xml_node test7Xml = doc.append_child("LookupNow");
    test7Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsRealArray");

    Expression *lookup7 = createExpression(test7Xml, &conn, wasCreated);
    assertTrue_1(lookup7);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup7->exprName(), "LookupNow"));
    assertTrue_1(lookup7->valueType() == REAL_ARRAY_TYPE);

    delete lookup7;
  }

  {
    xml_node test8Xml = doc.append_child("LookupNow");
    test8Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("returnsStringArray");

    Expression *lookup8 = createExpression(test8Xml, &conn, wasCreated);
    assertTrue_1(lookup8);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup8->exprName(), "LookupNow"));
    assertTrue_1(lookup8->valueType() == STRING_ARRAY_TYPE);

    delete lookup8;
  }

  return true;
}

static bool testDeclaredParameters()
{
  // Set up symbol table
  {
    Symbol *anyTest = g_symbolTable->addLookup("anyTest");
    anyTest->setReturnType(BOOLEAN_TYPE);
    anyTest->addParameterType(UNKNOWN_TYPE);

    Symbol *scalarTest = g_symbolTable->addLookup("scalarTest");
    scalarTest->setReturnType(BOOLEAN_TYPE);
    scalarTest->addParameterType(BOOLEAN_TYPE);
    scalarTest->addParameterType(INTEGER_TYPE);
    scalarTest->addParameterType(REAL_TYPE);
    scalarTest->addParameterType(STRING_TYPE);

    Symbol *anyArgsNoReqdTest = g_symbolTable->addLookup("anyArgsNoReqdTest");
    anyArgsNoReqdTest->setReturnType(BOOLEAN_TYPE);
    anyArgsNoReqdTest->setAnyParameters();

    Symbol *anyArgsOneReqdTest = g_symbolTable->addLookup("anyArgsOneReqdTest");
    anyArgsOneReqdTest->setReturnType(BOOLEAN_TYPE);
    anyArgsOneReqdTest->addParameterType(STRING_TYPE);
    anyArgsOneReqdTest->setAnyParameters();
  }

  // Other setup
  FactoryTestNodeConnector conn;
  bool wasCreated = false;

  xml_document doc;

  // Test basics

  {
    xml_node test0Xml = doc.append_child("LookupNow");
    test0Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("anyTest");
    xml_node args = test0Xml.append_child("Arguments");
    args.append_child("BooleanValue").append_child(node_pcdata).set_value("true");

    Expression *lookup0 = createExpression(test0Xml, &conn, wasCreated);
    assertTrue_1(lookup0);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup0->exprName(), "LookupNow"));
    assertTrue_1(lookup0->valueType() == BOOLEAN_TYPE);

    delete lookup0;
  }

  {
    xml_node test1Xml = doc.append_child("LookupNow");
    test1Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("scalarTest");
    xml_node args = test1Xml.append_child("Arguments");
    args.append_child("BooleanValue").append_child(node_pcdata).set_value("true");
    args.append_child("IntegerValue").append_child(node_pcdata).set_value("42");
    args.append_child("RealValue").append_child(node_pcdata).set_value("2.718");
    args.append_child("StringValue").append_child(node_pcdata).set_value("foo");

    Expression *lookup1 = createExpression(test1Xml, &conn, wasCreated);
    assertTrue_1(lookup1);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup1->exprName(), "LookupNow"));
    assertTrue_1(lookup1->valueType() == BOOLEAN_TYPE);

    delete lookup1;
  }

  {
    xml_node test2Xml = doc.append_child("LookupNow");
    test2Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("anyArgsNoReqdTest");

    Expression *lookup2 = createExpression(test2Xml, &conn, wasCreated);
    assertTrue_1(lookup2);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup2->exprName(), "LookupNow"));
    assertTrue_1(lookup2->valueType() == BOOLEAN_TYPE);

    delete lookup2;
  }

  {
    xml_node test3Xml = doc.append_child("LookupNow");
    test3Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("anyArgsNoReqdTest");
    xml_node args = test3Xml.append_child("Arguments");
    args.append_child("BooleanValue").append_child(node_pcdata).set_value("true");
    args.append_child("IntegerValue").append_child(node_pcdata).set_value("42");
    args.append_child("RealValue").append_child(node_pcdata).set_value("2.718");
    args.append_child("StringValue").append_child(node_pcdata).set_value("foo");

    Expression *lookup3 = createExpression(test3Xml, &conn, wasCreated);
    assertTrue_1(lookup3);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup3->exprName(), "LookupNow"));
    assertTrue_1(lookup3->valueType() == BOOLEAN_TYPE);

    delete lookup3;
  }

  {
    xml_node test4Xml = doc.append_child("LookupNow");
    test4Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("anyArgsOneReqdTest");
    xml_node args = test4Xml.append_child("Arguments");
    args.append_child("StringValue").append_child(node_pcdata).set_value("foo");

    Expression *lookup4 = createExpression(test4Xml, &conn, wasCreated);
    assertTrue_1(lookup4);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup4->exprName(), "LookupNow"));
    assertTrue_1(lookup4->valueType() == BOOLEAN_TYPE);

    delete lookup4;
  }

  {
    xml_node test5Xml = doc.append_child("LookupNow");
    test5Xml.append_child("Name").append_child("StringValue").append_child(node_pcdata).set_value("anyArgsOneReqdTest");
    xml_node args = test5Xml.append_child("Arguments");
    args.append_child("StringValue").append_child(node_pcdata).set_value("foo");
    args.append_child("BooleanValue").append_child(node_pcdata).set_value("true");
    args.append_child("IntegerValue").append_child(node_pcdata).set_value("42");
    args.append_child("RealValue").append_child(node_pcdata).set_value("2.718");

    Expression *lookup5 = createExpression(test5Xml, &conn, wasCreated);
    assertTrue_1(lookup5);
    assertTrue_1(wasCreated);
    assertTrue_1(0 == strcmp(lookup5->exprName(), "LookupNow"));
    assertTrue_1(lookup5->valueType() == BOOLEAN_TYPE);

    delete lookup5;
  }

  return true;
}

bool lookupXmlParserTest()
{
  runTest(testBasics);
  runTest(testDeclaredReturnTypes);
  runTest(testDeclaredParameters);
  // TODO test error checking

  return true;
}

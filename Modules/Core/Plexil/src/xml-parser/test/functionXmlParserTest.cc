/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#include "ArrayImpl.hh"
#include "ExpressionFactory.hh"
#include "NodeConnector.hh"
#include "ParserException.hh"
#include "TestSupport.hh"
#include "test/TrivialNodeConnector.hh"

#include "pugixml.hpp"

using namespace PLEXIL;

using pugi::xml_attribute;
using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

// Global for convenience

static NodeConnector *nc = NULL;

static bool isKnownXmlParserTest()
{
  bool wasCreated, temp;

  xml_document doc;

  xml_node knownXml = doc.append_child("IsKnown");

  // Check no-arg error detection
  try {
    Expression *zeroArgExp = createExpression(knownXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  knownXml.append_child("BooleanValue").append_child(node_pcdata).set_value("UNKNOWN");
  {
    Expression *oneArgExp = createExpression(knownXml, nc, wasCreated);
    assertTrue_1(oneArgExp);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgExp->valueType() == BOOLEAN_TYPE);
    oneArgExp->activate();
    assertTrue_1(oneArgExp->isKnown());
    assertTrue_1(oneArgExp->getValue(temp));
    assertTrue_1(!temp);
    delete oneArgExp;
  }

  // Check two-arg form
  knownXml.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  try {
    Expression *twoArgExp = createExpression(knownXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too many args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  return true;
}

static bool stringFunctionXmlParserTest()
{
  bool wasCreated;
  int32_t itemp;

  xml_document doc;

  // StringLength
  xml_node lenXml = doc.append_child("STRLEN");

  // Check no-arg error detection
  try {
    Expression *zeroArgExp = createExpression(lenXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  lenXml.append_child("StringValue").append_child(node_pcdata).set_value("Foo");
  {
    Expression *oneArgExp = createExpression(lenXml, nc, wasCreated);
    assertTrue_1(oneArgExp);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgExp->valueType() == INTEGER_TYPE);
    oneArgExp->activate();
    assertTrue_1(oneArgExp->isKnown());
    assertTrue_1(oneArgExp->getValue(itemp));
    assertTrue_1(itemp == 3);
    delete oneArgExp;
  }

  // Check two-arg form
  lenXml.append_child("StringValue").append_child(node_pcdata).set_value("1");
  try {
    Expression *twoArgExp = createExpression(lenXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too many args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // String concatenation
  std::string const *stemp = NULL;

  xml_node catXml = doc.append_child("Concat");

  // No-arg form
  {
    Expression *noArgCat = createExpression(catXml, nc, wasCreated);
    assertTrue_1(noArgCat);
    assertTrue_1(wasCreated);
    assertTrue_1(noArgCat->valueType() == STRING_TYPE);
    noArgCat->activate();
    assertTrue_1(noArgCat->isKnown());
    assertTrue_1(noArgCat->getValuePointer(stemp));
    assertTrue_1(stemp->empty());
    stemp = NULL;
    delete noArgCat;
  }

  // One-arg form
  catXml.append_child("StringValue").append_child(node_pcdata).set_value("Foo");
  {
    Expression *oneArgCat = createExpression(catXml, nc, wasCreated);
    assertTrue_1(oneArgCat);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgCat->valueType() == STRING_TYPE);
    oneArgCat->activate();
    assertTrue_1(oneArgCat->isKnown());
    assertTrue_1(oneArgCat->getValuePointer(stemp));
    assertTrue_1(*stemp == "Foo");
    stemp = NULL;
    delete oneArgCat;
  }

  // Two-arg form
  catXml.append_child("StringValue").append_child(node_pcdata).set_value("Bar");
  {
    Expression *twoArgCat = createExpression(catXml, nc, wasCreated);
    assertTrue_1(twoArgCat);
    assertTrue_1(wasCreated);
    assertTrue_1(twoArgCat->valueType() == STRING_TYPE);
    twoArgCat->activate();
    assertTrue_1(twoArgCat->isKnown());
    assertTrue_1(twoArgCat->getValuePointer(stemp));
    assertTrue_1(*stemp == "FooBar");
    stemp = NULL;
    delete twoArgCat;
  }

  // Three-arg form
  catXml.append_child("StringValue").append_child(node_pcdata).set_value("Baz");
  {
    Expression *threeArgCat = createExpression(catXml, nc, wasCreated);
    assertTrue_1(threeArgCat);
    assertTrue_1(wasCreated);
    assertTrue_1(threeArgCat->valueType() == STRING_TYPE);
    threeArgCat->activate();
    assertTrue_1(threeArgCat->isKnown());
    assertTrue_1(threeArgCat->getValuePointer(stemp));
    assertTrue_1(*stemp == "FooBarBaz");
    stemp = NULL;
    delete threeArgCat;
  }

  return true;
}

static bool booleanFunctionXmlParserTest()
{
  bool wasCreated, temp;

  xml_document doc;

  xml_node notOpXml = doc.append_child("NOT");

  // Check no-arg error detection
  try {
    Expression *zeroArgNot = createExpression(notOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  notOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  {
    Expression *oneArgNot = createExpression(notOpXml, nc, wasCreated);
    assertTrue_1(oneArgNot);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgNot->valueType() == BOOLEAN_TYPE);
    oneArgNot->activate();
    assertTrue_1(oneArgNot->isKnown());
    assertTrue_1(oneArgNot->getValue(temp));
    assertTrue_1(!temp);
    delete oneArgNot;
  }

  // Check two-arg form
  notOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  try {
    Expression *twoArgNot = createExpression(notOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too many args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node orOpXml = doc.append_child("OR");

  // Check no-arg error detection
  try {
    Expression *zeroArgOr = createExpression(orOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  orOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("0");
  {
    Expression *oneArgOr = createExpression(orOpXml, nc, wasCreated);
    assertTrue_1(oneArgOr);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgOr->valueType() == BOOLEAN_TYPE);
    oneArgOr->activate();
    assertTrue_1(oneArgOr->isKnown());
    assertTrue_1(oneArgOr->getValue(temp));
    assertTrue_1(!temp);
    delete oneArgOr;
  }

  // Check two-arg form
  orOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  {
    Expression *twoArgOr = createExpression(orOpXml, nc, wasCreated);
    assertTrue_1(twoArgOr);
    assertTrue_1(wasCreated);
    assertTrue_1(twoArgOr->valueType() == BOOLEAN_TYPE);
    twoArgOr->activate();
    assertTrue_1(twoArgOr->isKnown());
    assertTrue_1(twoArgOr->getValue(temp));
    assertTrue_1(temp);
    delete twoArgOr;
  }

  // check 3-arg
  orOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("UNKNOWN");
  {
    Expression *threeArgOr = createExpression(orOpXml, nc, wasCreated);
    assertTrue_1(threeArgOr);
    assertTrue_1(wasCreated);
    assertTrue_1(threeArgOr->valueType() == BOOLEAN_TYPE);
    threeArgOr->activate();
    assertTrue_1(threeArgOr->isKnown());
    assertTrue_1(threeArgOr->getValue(temp));
    assertTrue_1(temp);
    delete threeArgOr;
  }

  xml_node andOpXml = doc.append_child("AND");

  // Check no-arg error detection
  try {
    Expression *zeroArgAnd = createExpression(andOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  andOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("0");
  {
    Expression *oneArgAnd = createExpression(andOpXml, nc, wasCreated);
    assertTrue_1(oneArgAnd);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgAnd->valueType() == BOOLEAN_TYPE);
    oneArgAnd->activate();
    assertTrue_1(oneArgAnd->isKnown());
    assertTrue_1(oneArgAnd->getValue(temp));
    assertTrue_1(!temp);
    delete oneArgAnd;
  }

  // Check two-arg form
  andOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  {
    Expression *twoArgAnd = createExpression(andOpXml, nc, wasCreated);
    assertTrue_1(twoArgAnd);
    assertTrue_1(wasCreated);
    assertTrue_1(twoArgAnd->valueType() == BOOLEAN_TYPE);
    twoArgAnd->activate();
    assertTrue_1(twoArgAnd->isKnown());
    assertTrue_1(twoArgAnd->getValue(temp));
    assertTrue_1(!temp);
    delete twoArgAnd;
  }

  // check 3-arg
  andOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("UNKNOWN");
  {
    Expression *threeArgAnd = createExpression(andOpXml, nc, wasCreated);
    assertTrue_1(threeArgAnd);
    assertTrue_1(wasCreated);
    assertTrue_1(threeArgAnd->valueType() == BOOLEAN_TYPE);
    threeArgAnd->activate();
    assertTrue_1(threeArgAnd->isKnown());
    assertTrue_1(threeArgAnd->getValue(temp));
    assertTrue_1(!temp);
    delete threeArgAnd;
  }

  xml_node xorOpXml = doc.append_child("XOR");

  // Check no-arg error detection
  try {
    Expression *zeroArgXor = createExpression(xorOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  xorOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("0");
  {
    Expression *oneArgXor = createExpression(xorOpXml, nc, wasCreated);
    assertTrue_1(oneArgXor);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgXor->valueType() == BOOLEAN_TYPE);
    oneArgXor->activate();
    assertTrue_1(oneArgXor->isKnown());
    assertTrue_1(oneArgXor->getValue(temp));
    assertTrue_1(!temp);
    delete oneArgXor;
  }

  // Check two-arg form
  xorOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  {
    Expression *twoArgXor = createExpression(xorOpXml, nc, wasCreated);
    assertTrue_1(twoArgXor);
    assertTrue_1(wasCreated);
    assertTrue_1(twoArgXor->valueType() == BOOLEAN_TYPE);
    twoArgXor->activate();
    assertTrue_1(twoArgXor->isKnown());
    assertTrue_1(twoArgXor->getValue(temp));
    assertTrue_1(temp);
    delete twoArgXor;
  }

  // check 3-arg
  xorOpXml.append_child("BooleanValue").append_child(node_pcdata).set_value("UNKNOWN");
  {
    Expression *threeArgXor = createExpression(xorOpXml, nc, wasCreated);
    assertTrue_1(threeArgXor);
    assertTrue_1(wasCreated);
    assertTrue_1(threeArgXor->valueType() == BOOLEAN_TYPE);
    threeArgXor->activate();
    assertTrue_1(!threeArgXor->isKnown());
    assertTrue_1(!threeArgXor->getValue(temp));
    delete threeArgXor;
  }

  return true;
}

static bool arithmeticFunctionXmlParserTest()
{
  bool wasCreated;
  int32_t itemp;
  double rtemp;

  xml_document doc;

  xml_node sqrtOpXml = doc.append_child("SQRT");

  // Check no-arg error detection
  try {
    Expression *zeroArgSqrt = createExpression(sqrtOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  sqrtOpXml.append_child("RealValue").append_child(node_pcdata).set_value("4");
  {
    Expression *oneArgSqrt = createExpression(sqrtOpXml, nc, wasCreated);
    assertTrue_1(oneArgSqrt);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgSqrt->valueType() == REAL_TYPE);
    oneArgSqrt->activate();
    assertTrue_1(oneArgSqrt->isKnown());
    assertTrue_1(oneArgSqrt->getValue(rtemp));
    assertTrue_1(rtemp == 2);
    delete oneArgSqrt;
  }

  // check 2-arg
  sqrtOpXml.append_child("RealValue").append_child(node_pcdata).set_value("0");
  try {
    Expression *twoArgSqrt = createExpression(sqrtOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too many args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node realToIntegerOpXml = doc.append_child("REAL_TO_INT");

  // Check no-arg error detection
  try {
    Expression *zeroArgRealToInteger = createExpression(realToIntegerOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  realToIntegerOpXml.append_child("RealValue").append_child(node_pcdata).set_value("4");
  {
    Expression *oneArgRealToInteger = createExpression(realToIntegerOpXml, nc, wasCreated);
    assertTrue_1(oneArgRealToInteger);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgRealToInteger->valueType() == INTEGER_TYPE);
    oneArgRealToInteger->activate();
    assertTrue_1(oneArgRealToInteger->isKnown());
    assertTrue_1(oneArgRealToInteger->getValue(itemp));
    assertTrue_1(itemp == 4);
    delete oneArgRealToInteger;
  }

  // check 2-arg
  realToIntegerOpXml.append_child("RealValue").append_child(node_pcdata).set_value("0");
  try {
    Expression *twoArgRealToInteger = createExpression(realToIntegerOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too many args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node absOpXml = doc.append_child("ABS");

  // Check no-arg error detection
  try {
    Expression *zeroArgAbs = createExpression(absOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  absOpXml.append_child("RealValue").append_child(node_pcdata).set_value("-2");
  {
    Expression *oneArgAbs = createExpression(absOpXml, nc, wasCreated);
    assertTrue_1(oneArgAbs);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgAbs->valueType() == REAL_TYPE);
    oneArgAbs->activate();
    assertTrue_1(oneArgAbs->isKnown());
    assertTrue_1(oneArgAbs->getValue(rtemp));
    assertTrue_1(rtemp == 2);
    delete oneArgAbs;
  }

  // check 2-arg
  absOpXml.append_child("RealValue").append_child(node_pcdata).set_value("0");
  try {
    Expression *twoArgAbs = createExpression(absOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too many args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node subOpXml = doc.append_child("SUB");

  // Check no-arg error detection
  try {
    Expression *zeroArgSub = createExpression(subOpXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect too few args");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  // Check one-arg form
  subOpXml.append_child("IntegerValue").append_child(node_pcdata).set_value("-2");
  {
    Expression *oneArgSub = createExpression(subOpXml, nc, wasCreated);
    assertTrue_1(oneArgSub);
    assertTrue_1(wasCreated);
    assertTrue_1(oneArgSub->valueType() == INTEGER_TYPE);
    oneArgSub->activate();
    assertTrue_1(oneArgSub->isKnown());
    assertTrue_1(oneArgSub->getValue(itemp));
    assertTrue_1(itemp == 2);
    delete oneArgSub;
  }

  // Check two-arg form
  subOpXml.append_child("RealValue").append_child(node_pcdata).set_value("-2.5");
  {
    Expression *twoArgSub = createExpression(subOpXml, nc, wasCreated);
    assertTrue_1(twoArgSub);
    assertTrue_1(wasCreated);
    assertTrue_1(twoArgSub->valueType() == REAL_TYPE);
    twoArgSub->activate();
    assertTrue_1(twoArgSub->isKnown());
    assertTrue_1(twoArgSub->getValue(rtemp));
    assertTrue_1(rtemp == 0.5);
    delete twoArgSub;
  }

  // Check three-arg form
  subOpXml.append_child("IntegerValue").append_child(node_pcdata).set_value("3");
  {
    Expression *threeArgSub = createExpression(subOpXml, nc, wasCreated);
    assertTrue_1(threeArgSub);
    assertTrue_1(wasCreated);
    assertTrue_1(threeArgSub->valueType() == REAL_TYPE);
    threeArgSub->activate();
    assertTrue_1(threeArgSub->isKnown());
    assertTrue_1(threeArgSub->getValue(rtemp));
    assertTrue_1(rtemp == -2.5);
    delete threeArgSub;
  }

  return true;
}

bool functionXmlParserTest()
{
  // Initialize infrastructure
  nc = new TrivialNodeConnector();

  runTest(isKnownXmlParserTest);
  runTest(stringFunctionXmlParserTest);
  runTest(booleanFunctionXmlParserTest);
  runTest(arithmeticFunctionXmlParserTest);

  delete nc;
  nc = NULL;
  return true;
}

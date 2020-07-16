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
#include "Assignable.hh"
#include "ExpressionFactory.hh"
#include "test/FactoryTestNodeConnector.hh"
#include "TestSupport.hh"

#include "pugixml.hpp"

using namespace PLEXIL;

using pugi::xml_attribute;
using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

static bool booleanVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  bool temp;

  xml_document doc;
  doc.set_name("booleanVariableXmlParserTest");

  // uninitialized
  xml_node bXml = doc.append_child("DeclareVariable");
  bXml.append_child("Name").append_child(node_pcdata).set_value("b");
  bXml.append_child("Type").append_child(node_pcdata).set_value("Boolean");

  Expression *bExp = createExpression(bXml, nc, wasCreated);
  assertTrue_1(bExp);
  assertTrue_1(wasCreated);
  assertTrue_1(bExp->isAssignable());
  assertTrue_1(bExp->valueType() == BOOLEAN_TYPE);
  bExp->activate();
  assertTrue_1(!bExp->isKnown());
  assertTrue_1(!bExp->getValue(temp));
  realNc.storeVariable("b", bExp);

  // initialized
  xml_node fXml = doc.append_copy(bXml);
  fXml.first_child().first_child().set_value("f");
  xml_node tempText = fXml.append_child("InitialValue").append_child("BooleanValue").append_child(node_pcdata);
  tempText.set_value("0");

  Expression *fExp = createExpression(fXml, nc, wasCreated);
  assertTrue_1(fExp);
  assertTrue_1(wasCreated);
  assertTrue_1(fExp->isAssignable());
  assertTrue_1(fExp->valueType() == BOOLEAN_TYPE);
  Expression *fInit = createExpression(fXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(fInit);
  fExp->asAssignable()->setInitializer(fInit, wasCreated);
  fExp->activate();
  assertTrue_1(fExp->isKnown());
  assertTrue_1(fExp->getValue(temp));
  assertTrue_1(!temp);
  realNc.storeVariable("f", fExp);

  xml_node tXml = doc.append_copy(bXml);
  tXml.first_child().first_child().set_value("t");
  tempText = tXml.append_child("InitialValue").append_child("BooleanValue").append_child(node_pcdata);
  tempText.set_value("1");

  Expression *tExp = createExpression(tXml, nc, wasCreated);
  assertTrue_1(tExp);
  assertTrue_1(wasCreated);
  assertTrue_1(tExp->isAssignable());
  assertTrue_1(tExp->valueType() == BOOLEAN_TYPE);
  Expression *tInit = createExpression(tXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(tInit);
  tExp->asAssignable()->setInitializer(tInit, wasCreated);
  tExp->activate();
  assertTrue_1(tExp->isKnown());
  assertTrue_1(tExp->getValue(temp));
  assertTrue_1(temp);
  realNc.storeVariable("t", tExp);

  xml_node uXml = doc.append_copy(bXml);
  uXml.first_child().first_child().set_value("u");
  tempText = uXml.append_child("InitialValue").append_child("BooleanValue").append_child(node_pcdata);
  tempText.set_value("UNKNOWN");

  {
    Expression *uExp = createExpression(uXml, nc, wasCreated);
    assertTrue_1(uExp);
    assertTrue_1(wasCreated);
    assertTrue_1(uExp->isAssignable());
    assertTrue_1(uExp->valueType() == BOOLEAN_TYPE);
    Expression *uInit = createExpression(uXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(uInit);
    uExp->asAssignable()->setInitializer(uInit, wasCreated);
    uExp->activate();
    assertTrue_1(!uExp->isKnown());
    assertTrue_1(!uExp->getValue(temp));
    delete uExp;
  }

  xml_node bogusTypeXml = doc.append_copy(bXml);
  bogusTypeXml.first_child().first_child().set_value("bogusType");
  tempText = bogusTypeXml.append_child("InitialValue").append_child("StringValue").append_child(node_pcdata);
  tempText.set_value("12345");
  {
    Expression *bogusTypeExp = createExpression(bogusTypeXml, nc, wasCreated);
    assertTrue_1(bogusTypeExp);
    assertTrue_1(wasCreated);
    assertTrue_1(bogusTypeExp->isAssignable());
    assertTrue_1(bogusTypeExp->valueType() == BOOLEAN_TYPE);
    Expression *bogusTypeInit = createExpression(bogusTypeXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(bogusTypeInit);
    try {
      bogusTypeExp->asAssignable()->setInitializer(bogusTypeInit, wasCreated);
      assertTrue_2(false, "Failed to detect initial value of wrong type");
    }
    catch (Error const & /*exc*/) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete bogusTypeInit;
    delete bogusTypeExp;
  }

  // Variable references

  xml_node bRefXml = doc.append_child("BooleanVariable");
  bRefXml.append_child(node_pcdata).set_value("b");

  Expression *bExpRef = createExpression(bRefXml, nc, wasCreated);
  assertTrue_1(!wasCreated);
  assertTrue_1(bExpRef == bExp);

  xml_node qRefXml = doc.append_child("BooleanVariable");
  qRefXml.append_child(node_pcdata).set_value("q");

  try {
    Expression *qExpRef = createExpression(qRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect nonexistent variable");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node tBadRefXml = doc.append_child("IntegerVariable");
  tBadRefXml.append_child(node_pcdata).set_value("t");

  try {
    Expression *tBadExpRef = createExpression(tBadRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type conflict");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }
    
  return true;
}

static bool integerVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  int32_t temp;

  xml_document doc;
  doc.set_name("integerVariableXmlParserTest");

  // uninitialized
  xml_node iXml = doc.append_child("DeclareVariable");
  iXml.append_child("Name").append_child(node_pcdata).set_value("i");
  iXml.append_child("Type").append_child(node_pcdata).set_value("Integer");

  Expression *iExp = createExpression(iXml, nc, wasCreated);
  assertTrue_1(iExp);
  assertTrue_1(wasCreated);
  assertTrue_1(iExp->isAssignable());
  assertTrue_1(iExp->valueType() == INTEGER_TYPE);
  iExp->activate();
  assertTrue_1(!iExp->isKnown());
  assertTrue_1(!iExp->getValue(temp));
  realNc.storeVariable("i", iExp);

  // initialized
  xml_node zeroXml = doc.append_copy(iXml);
  zeroXml.first_child().first_child().set_value("z");
  xml_node tempText = zeroXml.append_child("InitialValue").append_child("IntegerValue").append_child(node_pcdata);
  tempText.set_value("0");

  Expression *zeroExp = createExpression(zeroXml, nc, wasCreated);
  assertTrue_1(zeroExp);
  assertTrue_1(wasCreated);
  assertTrue_1(zeroExp->isAssignable());
  assertTrue_1(zeroExp->valueType() == INTEGER_TYPE);
  Expression *zeroInit = createExpression(zeroXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(zeroInit);
  zeroExp->asAssignable()->setInitializer(zeroInit, wasCreated);
  zeroExp->activate();
  assertTrue_1(zeroExp->isKnown());
  assertTrue_1(zeroExp->getValue(temp));
  assertTrue_1(temp == 0);
  realNc.storeVariable("z", zeroExp);

  xml_node tXml = doc.append_copy(iXml);
  tXml.first_child().first_child().set_value("t");
  tempText = tXml.append_child("InitialValue").append_child("IntegerValue").append_child(node_pcdata);
  tempText.set_value("-2000000000");

  Expression *tExp = createExpression(tXml, nc, wasCreated);
  assertTrue_1(tExp);
  assertTrue_1(wasCreated);
  assertTrue_1(tExp->isAssignable());
  assertTrue_1(tExp->valueType() == INTEGER_TYPE);
  Expression *tInit = createExpression(tXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(tInit);
  tExp->asAssignable()->setInitializer(tInit, wasCreated);
  tExp->activate();
  assertTrue_1(tExp->isKnown());
  assertTrue_1(tExp->getValue(temp));
  assertTrue_1(temp == (int32_t) -2000000000);
  realNc.storeVariable("t", tExp);

  xml_node hXml = doc.append_copy(iXml);
  hXml.first_child().first_child().set_value("h");
  tempText = hXml.append_child("InitialValue").append_child("IntegerValue").append_child(node_pcdata);
  tempText.set_value("0xBADF00D");

  Expression *hExp = createExpression(hXml, nc, wasCreated);
  assertTrue_1(hExp);
  assertTrue_1(wasCreated);
  assertTrue_1(hExp->isAssignable());
  assertTrue_1(hExp->valueType() == INTEGER_TYPE);
  Expression *hInit = createExpression(hXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(hInit);
  hExp->asAssignable()->setInitializer(hInit, wasCreated);
  hExp->activate();
  assertTrue_1(hExp->isKnown());
  assertTrue_1(hExp->getValue(temp));
  assertTrue_1(temp == 0xBADF00D);
  realNc.storeVariable("h", hExp);

  xml_node uXml = doc.append_copy(iXml);
  uXml.first_child().first_child().set_value("u");
  tempText = uXml.append_child("InitialValue").append_child("IntegerValue").append_child(node_pcdata);
  tempText.set_value("UNKNOWN");

  {
    Expression *uExp = createExpression(uXml, nc, wasCreated);
    assertTrue_1(uExp);
    assertTrue_1(wasCreated);
    assertTrue_1(uExp->isAssignable());
    assertTrue_1(uExp->valueType() == INTEGER_TYPE);
    Expression *uInit = createExpression(uXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(uInit);
    uExp->asAssignable()->setInitializer(uInit, wasCreated);
    uExp->activate();
    assertTrue_1(!uExp->isKnown());
    assertTrue_1(!uExp->getValue(temp));
    delete uExp;
  }

  xml_node bogusTypeXml = doc.append_copy(iXml);
  bogusTypeXml.first_child().first_child().set_value("bogusType");
  tempText = bogusTypeXml.append_child("InitialValue").append_child("StringValue").append_child(node_pcdata);
  tempText.set_value("12345");
  {
    Expression *bogusTypeExp = createExpression(bogusTypeXml, nc, wasCreated);
    assertTrue_1(bogusTypeExp);
    assertTrue_1(wasCreated);
    Expression *bogusTypeInit = createExpression(bogusTypeXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(bogusTypeInit);
    try {
      bogusTypeExp->asAssignable()->setInitializer(bogusTypeInit, wasCreated);
      assertTrue_2(false, "Failed to detect initial value of wrong type");
    }
    catch (Error const & /*exc*/) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete bogusTypeInit;
    delete bogusTypeExp;
  }

  // Variable references

  xml_node iRefXml = doc.append_child("IntegerVariable");
  iRefXml.append_child(node_pcdata).set_value("i");

  Expression *iExpRef = createExpression(iRefXml, nc, wasCreated);
  assertTrue_1(!wasCreated);
  assertTrue_1(iExpRef == iExp);

  xml_node qRefXml = doc.append_child("IntegerVariable");
  qRefXml.append_child(node_pcdata).set_value("q");
  try {
    Expression *qExpRef = createExpression(qRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect nonexistent variable");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node zBadRefXml = doc.append_child("BooleanVariable");
  zBadRefXml.append_child(node_pcdata).set_value("z");
  try {
    Expression *tBadExpRef = createExpression(zBadRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type conflict");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }
    
  return true;
}

static bool realVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  double temp;

  xml_document doc;
  doc.set_name("realVariableXmlParserTest");

  // uninitialized
  xml_node iXml = doc.append_child("DeclareVariable");
  iXml.append_child("Name").append_child(node_pcdata).set_value("i");
  iXml.append_child("Type").append_child(node_pcdata).set_value("Real");

  Expression *iExp = createExpression(iXml, nc, wasCreated);
  assertTrue_1(iExp);
  assertTrue_1(wasCreated);
  assertTrue_1(iExp->isAssignable());
  assertTrue_1(iExp->valueType() == REAL_TYPE);
  iExp->activate();
  assertTrue_1(!iExp->isKnown());
  assertTrue_1(!iExp->getValue(temp));
  realNc.storeVariable("i", iExp);

  // initialized
  xml_node zeroXml = doc.append_copy(iXml);
  zeroXml.first_child().first_child().set_value("z");
  xml_node tempText = zeroXml.append_child("InitialValue").append_child("RealValue").append_child(node_pcdata);
  tempText.set_value("0");

  Expression *zeroExp = createExpression(zeroXml, nc, wasCreated);
  assertTrue_1(zeroExp);
  assertTrue_1(wasCreated);
  assertTrue_1(zeroExp->isAssignable());
  assertTrue_1(zeroExp->valueType() == REAL_TYPE);
  Expression *zeroInit = createExpression(zeroXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(zeroInit);
  zeroExp->asAssignable()->setInitializer(zeroInit, wasCreated);
  zeroExp->activate();
  assertTrue_1(zeroExp->isKnown());
  assertTrue_1(zeroExp->getValue(temp));
  assertTrue_1(temp == 0);
  realNc.storeVariable("z", zeroExp);

  xml_node tXml = doc.append_copy(iXml);
  tXml.first_child().first_child().set_value("t");
  tempText = tXml.append_child("InitialValue").append_child("RealValue").append_child(node_pcdata);
  tempText.set_value("-2e10");

  Expression *tExp = createExpression(tXml, nc, wasCreated);
  assertTrue_1(tExp);
  assertTrue_1(wasCreated);
  assertTrue_1(tExp->isAssignable());
  assertTrue_1(tExp->valueType() == REAL_TYPE);
  Expression *tInit = createExpression(tXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(tInit);
  tExp->asAssignable()->setInitializer(tInit, wasCreated);
  tExp->activate();
  assertTrue_1(tExp->isKnown());
  assertTrue_1(tExp->getValue(temp));
  assertTrue_1(temp == -20000000000.0);
  realNc.storeVariable("t", tExp);

  xml_node piXml = doc.append_copy(iXml);
  piXml.first_child().first_child().set_value("pi");
  tempText = piXml.append_child("InitialValue").append_child("RealValue").append_child(node_pcdata);
  tempText.set_value("3.1415");

  Expression *piExp = createExpression(piXml, nc, wasCreated);
  assertTrue_1(piExp);
  assertTrue_1(wasCreated);
  assertTrue_1(piExp->isAssignable());
  assertTrue_1(piExp->valueType() == REAL_TYPE);
  Expression *piInit = createExpression(piXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(piInit);
  piExp->asAssignable()->setInitializer(piInit, wasCreated);
  piExp->activate();
  assertTrue_1(piExp->isKnown());
  assertTrue_1(piExp->getValue(temp));
  assertTrue_1(temp == 3.1415);
  realNc.storeVariable("pi", piExp);

  xml_node uXml = doc.append_copy(iXml);
  uXml.first_child().first_child().set_value("u");
  tempText = uXml.append_child("InitialValue").append_child("RealValue").append_child(node_pcdata);
  tempText.set_value("UNKNOWN");

  {
    Expression *uExp = createExpression(uXml, nc, wasCreated);
    assertTrue_1(uExp);
    assertTrue_1(wasCreated);
    assertTrue_1(uExp->isAssignable());
    assertTrue_1(uExp->valueType() == REAL_TYPE);
    Expression *uInit = createExpression(uXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(uInit);
    uExp->asAssignable()->setInitializer(uInit, wasCreated);
    uExp->activate();
    assertTrue_1(!uExp->isKnown());
    assertTrue_1(!uExp->getValue(temp));
    delete uExp;
  }

  xml_node bogusTypeXml = doc.append_copy(iXml);
  bogusTypeXml.first_child().first_child().set_value("bogusType");
  tempText = bogusTypeXml.append_child("InitialValue").append_child("StringValue").append_child(node_pcdata);
  tempText.set_value("12345");
  {
    Expression *bogusTypeExp = createExpression(bogusTypeXml, nc, wasCreated);
    assertTrue_1(bogusTypeExp);
    assertTrue_1(wasCreated);
    assertTrue_1(bogusTypeExp->isAssignable());
    assertTrue_1(bogusTypeExp->valueType() == REAL_TYPE);
    Expression *bogusTypeInit = createExpression(bogusTypeXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(bogusTypeInit);
    try {
      bogusTypeExp->asAssignable()->setInitializer(bogusTypeInit, wasCreated);
      assertTrue_2(false, "Failed to detect initial value of wrong type");
    }
    catch (Error const & /*exc*/) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete bogusTypeInit;
    delete bogusTypeExp;
  }

  // Variable references

  xml_node iRefXml = doc.append_child("RealVariable");
  iRefXml.append_child(node_pcdata).set_value("i");

  Expression *iExpRef = createExpression(iRefXml, nc, wasCreated);
  assertTrue_1(!wasCreated);
  assertTrue_1(iExpRef);
  assertTrue_1(iExpRef == iExp);

  xml_node qRefXml = doc.append_child("RealVariable");
  qRefXml.append_child(node_pcdata).set_value("q");
  try {
    Expression *qExpRef = createExpression(qRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect nonexistent variable");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node zBadRefXml = doc.append_child("BooleanVariable");
  zBadRefXml.append_child(node_pcdata).set_value("z");
  try {
    Expression *zBadExpRef = createExpression(zBadRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type conflict");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }
    
  return true;
}

static bool stringVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  std::string const *temp = NULL;

  xml_document doc;
  doc.set_name("stringVariableXmlParserTest");

  // uninitialized
  xml_node unkXml = doc.append_child("DeclareVariable");
  unkXml.append_child("Name").append_child(node_pcdata).set_value("unk");
  unkXml.append_child("Type").append_child(node_pcdata).set_value("String");

  Expression *unkExp = createExpression(unkXml, nc, wasCreated);
  assertTrue_1(unkExp);
  assertTrue_1(wasCreated);
  assertTrue_1(unkExp->isAssignable());
  assertTrue_1(unkExp->valueType() == STRING_TYPE);
  unkExp->activate();
  assertTrue_1(!unkExp->isKnown());
  assertTrue_1(!unkExp->getValuePointer(temp));
  assertTrue_1(temp == NULL);
  realNc.storeVariable("unk", unkExp);

  // initialized empty
  xml_node mtXml = doc.append_copy(unkXml);
  mtXml.first_child().first_child().set_value("mt");
  mtXml.append_child("InitialValue").append_child("StringValue").append_child(node_pcdata);

  Expression *mtExp = createExpression(mtXml, nc, wasCreated);
  assertTrue_1(mtExp);
  assertTrue_1(wasCreated);
  assertTrue_1(mtExp->isAssignable());
  assertTrue_1(mtExp->valueType() == STRING_TYPE);
  Expression *mtInit = createExpression(mtXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(mtInit);
  mtExp->asAssignable()->setInitializer(mtInit, wasCreated);
  mtExp->activate();
  assertTrue_1(mtExp->isKnown());
  assertTrue_1(mtExp->getValuePointer(temp));
  assertTrue_1(temp);
  assertTrue_1(temp->empty());
  realNc.storeVariable("mt", mtExp);

  // initialized to literal
  xml_node fooXml = doc.append_copy(unkXml);
  fooXml.first_child().first_child().set_value("foo");
  xml_node tempText = fooXml.append_child("InitialValue").append_child("StringValue").append_child(node_pcdata);
  tempText.set_value("Foo!");

  Expression *fooExp = createExpression(fooXml, nc, wasCreated);
  assertTrue_1(fooExp);
  assertTrue_1(wasCreated);
  assertTrue_1(fooExp->isAssignable());
  assertTrue_1(fooExp->valueType() == STRING_TYPE);
  Expression *fooInit = createExpression(fooXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(fooInit);
  fooExp->asAssignable()->setInitializer(fooInit, wasCreated);
  fooExp->activate();
  assertTrue_1(fooExp->isKnown());
  assertTrue_1(fooExp->getValuePointer(temp));
  assertTrue_1(temp);
  assertTrue_1(!temp->empty());
  assertTrue_1(*temp == "Foo!");
  realNc.storeVariable("foo", fooExp);

  // initialized to var ref
  xml_node barXml = doc.append_copy(unkXml);
  barXml.first_child().first_child().set_value("bar");
  barXml.append_child("InitialValue").append_child("StringVariable").append_child(node_pcdata).set_value("foo");

  Expression *barExp = createExpression(barXml, nc, wasCreated);
  assertTrue_1(barExp);
  assertTrue_1(wasCreated);
  assertTrue_1(barExp->isAssignable());
  assertTrue_1(barExp->valueType() == STRING_TYPE);
  Expression *barInit = createExpression(barXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(barInit);
  barExp->asAssignable()->setInitializer(barInit, wasCreated);
  barExp->activate();
  assertTrue_1(barExp->isKnown());
  assertTrue_1(barExp->getValuePointer(temp));
  assertTrue_1(temp);
  assertTrue_1(!temp->empty());
  assertTrue_1(*temp == "Foo!");
  realNc.storeVariable("bar", barExp);

  xml_node bogusTypeXml = doc.append_copy(unkXml);
  bogusTypeXml.first_child().first_child().set_value("bogusType");
  bogusTypeXml.append_child("InitialValue").append_child("IntegerValue").append_child(node_pcdata).set_value("12345");
  {
    Expression *bogusTypeExp = createExpression(bogusTypeXml, nc, wasCreated);
    assertTrue_1(bogusTypeExp);
    assertTrue_1(wasCreated);
    assertTrue_1(bogusTypeExp->isAssignable());
    assertTrue_1(bogusTypeExp->valueType() == STRING_TYPE);
    Expression *bogusTypeInit = createExpression(bogusTypeXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(bogusTypeInit);
    try {
      bogusTypeExp->asAssignable()->setInitializer(bogusTypeInit, wasCreated);
      assertTrue_2(false, "Failed to detect invalid initial value");
    }
    catch (Error const & /*exc*/) {
      std::cout << "Caught expected exception" << std::endl;
    }
    delete bogusTypeInit;
    delete bogusTypeExp;
  }

  // Variable references

  xml_node unkRefXml = doc.append_child("StringVariable");
  unkRefXml.append_child(node_pcdata).set_value("unk");

  Expression *unkRefExp = createExpression(unkRefXml, nc, wasCreated);
  assertTrue_1(!wasCreated);
  assertTrue_1(unkRefExp);
  assertTrue_1(unkRefExp = unkExp);

  xml_node badRefXml = doc.append_child("StringVariable");
  badRefXml.append_child(node_pcdata).set_value("bad");

  try {
    Expression *badRefExp = createExpression(badRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect nonexistent variable");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node badTypeRefXml = doc.append_child("BooleanVariable");
  badTypeRefXml.append_child(node_pcdata).set_value("mt");

  try {
    Expression *badTypeRefExp = createExpression(badTypeRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type conflict");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  return true;
}

static bool booleanArrayVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated, temp;
  BooleanArray const *aryTemp = NULL;

  xml_document doc;
  doc.set_name("booleanArrayVariableXmlParserTest");

  xml_node ba1Xml = doc.append_child("DeclareArray");
  ba1Xml.append_child("Name").append_child(node_pcdata).set_value("ba1");
  ba1Xml.append_child("Type").append_child(node_pcdata).set_value("Boolean");
  ba1Xml.append_child("MaxSize").append_child(node_pcdata).set_value("2");

  Expression *ba1Exp = createExpression(ba1Xml, nc, wasCreated);
  assertTrue_1(wasCreated);
  assertTrue_1(ba1Exp);
  assertTrue_1(ba1Exp->valueType() == BOOLEAN_ARRAY_TYPE);
  ba1Exp->activate();
  assertTrue_1(ba1Exp->isKnown());
  assertTrue_1(ba1Exp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp);
  assertTrue_1(aryTemp->size() == 2);
  assertTrue_1(!aryTemp->anyElementsKnown());
  realNc.storeVariable("ba1", ba1Exp);

  xml_node ba2Xml = doc.append_child("DeclareArray");
  ba2Xml.append_child("Name").append_child(node_pcdata).set_value("ba2");
  ba2Xml.append_child("Type").append_child(node_pcdata).set_value("Boolean");
  ba2Xml.append_child("MaxSize").append_child(node_pcdata).set_value("7");
  xml_node initvals = ba2Xml.append_child("InitialValue").append_child("ArrayValue");
  initvals.append_attribute("Type").set_value("Boolean");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("0");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("1");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("UNKNOWN");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("true");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("false");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("FALSE");
  initvals.append_child("BooleanValue").append_child(node_pcdata).set_value("TRUE");

  Expression *ba2Exp = createExpression(ba2Xml, nc, wasCreated);
  assertTrue_1(wasCreated);
  assertTrue_1(ba2Exp);
  assertTrue_1(ba2Exp->valueType() == BOOLEAN_ARRAY_TYPE);
  Expression *ba2Init = createExpression(initvals, nc, wasCreated);
  assertTrue_1(ba2Init);
  assertTrue_1(ba2Init->isKnown());
  ba2Exp->asAssignable()->setInitializer(ba2Init, wasCreated);
  ba2Exp->activate();
  assertTrue_1(ba2Exp->isKnown());
  assertTrue_1(ba2Exp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp);
  assertTrue_1(aryTemp->size() == 7);
  assertTrue_1(aryTemp->anyElementsKnown());
  assertTrue_1(!aryTemp->allElementsKnown());
  assertTrue_1(aryTemp->getElement(0, temp));
  assertTrue_1(!temp);
  assertTrue_1(aryTemp->getElement(1, temp));
  assertTrue_1(temp);
  assertTrue_1(!aryTemp->getElement(2, temp));
  assertTrue_1(aryTemp->getElement(3, temp));
  assertTrue_1(temp);
  assertTrue_1(aryTemp->getElement(4, temp));
  assertTrue_1(!temp);
  assertTrue_1(aryTemp->getElement(5, temp));
  assertTrue_1(!temp);
  assertTrue_1(aryTemp->getElement(6, temp));
  assertTrue_1(temp);
  realNc.storeVariable("ba2", ba2Exp);

  // Variable refs
  
  xml_node ba1RefXml = doc.append_child("ArrayVariable");
  ba1RefXml.append_child(node_pcdata).set_value("ba1");

  Expression *ba1RefExp = createExpression(ba1RefXml, nc, wasCreated);
  assertTrue_1(ba1RefExp);
  assertTrue_1(!wasCreated);
  assertTrue_1(ba1RefExp == ba1Exp);

  xml_node ba2RefXml = doc.append_child("BooleanVariable");
  ba2RefXml.append_child(node_pcdata).set_value("ba2");

  try {
    Expression *ba2RefExp = createExpression(ba2RefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type mismatch");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected error" << std::endl;
  }

  xml_node badRefXml = doc.append_child("ArrayVariable");
  badRefXml.append_child(node_pcdata).set_value("bad");

  try {
    Expression *badRefExp = createExpression(badRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect invalid variable reference");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected error" << std::endl;
  }

  return true;
}

static bool integerArrayVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  IntegerArray const *aryTemp = NULL;

  xml_document doc;
  doc.set_name("integerArrayVariableXmlParserTest");

  xml_node emptyXml = doc.append_child("DeclareArray");
  emptyXml.append_child("Name").append_child(node_pcdata).set_value("empty");
  emptyXml.append_child("Type").append_child(node_pcdata).set_value("Integer");
  emptyXml.append_child("MaxSize").append_child(node_pcdata).set_value("2");
  xml_node emptyInitXml = emptyXml.append_child("InitialValue").append_child("ArrayValue");
  emptyInitXml.append_attribute("Type").set_value("Integer");

  Expression *emptyExp = createExpression(emptyXml, nc, wasCreated);
  assertTrue_1(emptyExp);
  assertTrue_1(wasCreated);
  assertTrue_1(emptyExp->valueType() == INTEGER_ARRAY_TYPE);
  Expression *emptyInit = createExpression(emptyInitXml, nc, wasCreated);
  assertTrue_1(emptyInit);
  emptyExp->asAssignable()->setInitializer(emptyInit, wasCreated);
  emptyExp->activate();
  assertTrue_1(emptyExp->isKnown());
  assertTrue_1(emptyExp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp != NULL);
  assertTrue_1(aryTemp->size() == 2);
  realNc.storeVariable("empty", emptyExp);

  int32_t temp;

  xml_node validXml = doc.append_child("DeclareArray");
  validXml.append_child("Name").append_child(node_pcdata).set_value("valid");
  validXml.append_child("Type").append_child(node_pcdata).set_value("Integer");
  validXml.append_child("MaxSize").append_child(node_pcdata).set_value("6");
  xml_node validContents = validXml.append_child("InitialValue").append_child("ArrayValue");
  validContents.append_attribute("Type").set_value("Integer");
  validContents.append_child("IntegerValue").append_child(node_pcdata).set_value("0");
  validContents.append_child("IntegerValue").append_child(node_pcdata).set_value("1");
  validContents.append_child("IntegerValue").append_child(node_pcdata).set_value("UNKNOWN");
  validContents.append_child("IntegerValue").append_child(node_pcdata).set_value("-123456789");
  validContents.append_child("IntegerValue").append_child(node_pcdata).set_value("987654321");
  validContents.append_child("IntegerValue").append_child(node_pcdata).set_value("0x69");

  Expression *validValExp = createExpression(validXml, nc, wasCreated);
  assertTrue_1(validValExp);
  assertTrue_1(wasCreated);
  assertTrue_1(validValExp->valueType() == INTEGER_ARRAY_TYPE);
  Expression *validValInit = createExpression(validContents, nc, wasCreated);
  assertTrue_1(validValInit);
  validValExp->asAssignable()->setInitializer(validValInit, wasCreated);
  validValExp->activate();
  assertTrue_1(validValExp->isKnown());
  assertTrue_1(validValExp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp != NULL);
  assertTrue_1(aryTemp->size() == 6);
  assertTrue_1(aryTemp->getElement(0, temp));
  assertTrue_1(temp == 0);
  assertTrue_1(aryTemp->getElement(1, temp));
  assertTrue_1(temp == 1);
  assertTrue_1(!aryTemp->getElement(2, temp));
  assertTrue_1(aryTemp->getElement(3, temp));
  assertTrue_1(temp == -123456789);
  assertTrue_1(aryTemp->getElement(4, temp));
  assertTrue_1(temp == 987654321);
  assertTrue_1(aryTemp->getElement(5, temp));
  assertTrue_1(temp == 0x69);
  realNc.storeVariable("valid", validValExp);

  xml_node tooLongValueXml = doc.append_child("DeclareArray");
  tooLongValueXml.append_child("Name").append_child(node_pcdata).set_value("tooLongValue");
  tooLongValueXml.append_child("Type").append_child(node_pcdata).set_value("Integer");
  tooLongValueXml.append_child("MaxSize").append_child(node_pcdata).set_value("1");
  xml_node tooLongContents = tooLongValueXml.append_child("InitialValue").append_child("ArrayValue");
  tooLongContents.append_attribute("Type").set_value("Integer");
  tooLongContents.append_child("IntegerValue").append_child(node_pcdata).set_value("1");
  tooLongContents.append_child("IntegerValue").append_child(node_pcdata).set_value("2");

  {
    Expression *tooLongValueExp = createExpression(tooLongValueXml, nc, wasCreated);
    assertTrue_1(tooLongValueExp);
    assertTrue_1(wasCreated);
    assertTrue_1(tooLongValueExp->valueType() == INTEGER_ARRAY_TYPE);
    assertTrue_1(tooLongValueExp->isAssignable());
    Expression *tooLongValueInit = createExpression(tooLongValueXml.child("InitialValue").first_child(), nc, wasCreated);
    assertTrue_1(tooLongValueInit);
    try {
      tooLongValueExp->asAssignable()->setInitializer(tooLongValueInit, wasCreated);
      assertTrue_2(ALWAYS_FAIL, "Failed to detect oversized initial value");
    }
    catch (Error const & /* exc */) {
      std::cout << "Caught expected error" << std::endl;
    }
    delete tooLongValueInit;
    delete tooLongValueExp;
  }

  // Variable reference tests

  xml_node validRefXml = doc.append_child("ArrayVariable");
  validRefXml.append_child(node_pcdata).set_value("valid");

  Expression *validRefExp = createExpression(validRefXml, nc, wasCreated);
  assertTrue_1(validRefExp);
  assertTrue_1(!wasCreated);
  assertTrue_1(validRefExp == validValExp);

  xml_node badNameRefXml = doc.append_child("ArrayVariable");
  badNameRefXml.append_child(node_pcdata).set_value("bad");

  try {
    Expression *badNameExp = createExpression(badNameRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect invalid variable reference");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected error" << std::endl;
  }

  xml_node wrongTypeRefXml = doc.append_child("IntegerVariable");
  wrongTypeRefXml.append_child(node_pcdata).set_value("empty");

  try {
    Expression *wrongTypeExp = createExpression(wrongTypeRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type mismatch");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected error" << std::endl;
  }

  return true;
}

static bool realArrayVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  RealArray const *aryTemp = NULL;
  double temp;

  xml_document doc;
  doc.set_name("realArrayVariableXmlParserTest");

  xml_node emptyXml = doc.append_child("DeclareArray");
  emptyXml.append_child("Name").append_child(node_pcdata).set_value("empty");
  emptyXml.append_child("Type").append_child(node_pcdata).set_value("Real");
  emptyXml.append_child("MaxSize").append_child(node_pcdata).set_value("0");
  xml_node emptyInitXml = emptyXml.append_child("InitialValue").append_child("ArrayValue");
  emptyInitXml.append_attribute("Type").set_value("Real");

  Expression *emptyExp = createExpression(emptyXml, nc, wasCreated);
  assertTrue_1(emptyExp);
  assertTrue_1(wasCreated);
  assertTrue_1(emptyExp->valueType() == REAL_ARRAY_TYPE);
  Expression *emptyInit = createExpression(emptyInitXml, nc, wasCreated);
  assertTrue_1(emptyInit);
  emptyExp->asAssignable()->setInitializer(emptyInit, wasCreated);
  emptyExp->activate();
  assertTrue_1(emptyExp->isKnown());
  assertTrue_1(emptyExp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp != NULL);
  assertTrue_1(aryTemp->size() == 0);
  realNc.storeVariable("emmty", emptyExp);

  xml_node validXml = doc.append_child("DeclareArray");
  validXml.append_child("Name").append_child(node_pcdata).set_value("vallid");
  validXml.append_child("Type").append_child(node_pcdata).set_value("Real");
  validXml.append_child("MaxSize").append_child(node_pcdata).set_value("6");
  xml_node contentsElt = validXml.append_child("InitialValue").append_child("ArrayValue");
  contentsElt.append_attribute("Type").set_value("Real");
  contentsElt.append_child("RealValue").append_child(node_pcdata).set_value("0");
  contentsElt.append_child("RealValue").append_child(node_pcdata).set_value("1");
  contentsElt.append_child("RealValue").append_child(node_pcdata).set_value("UNKNOWN");
  contentsElt.append_child("RealValue").append_child(node_pcdata).set_value("3.14");
  contentsElt.append_child("RealValue").append_child(node_pcdata).set_value("1e-100");
  contentsElt.append_child("RealValue").append_child(node_pcdata).set_value("6.0221413e+23");

  Expression *validVarExp = createExpression(validXml, nc, wasCreated);
  assertTrue_1(validVarExp);
  assertTrue_1(wasCreated);
  assertTrue_1(validVarExp->valueType() == REAL_ARRAY_TYPE);
  Expression *validVarInit = createExpression(validXml.child("InitialValue").first_child(), nc, wasCreated);
  assertTrue_1(validVarInit);
  validVarExp->asAssignable()->setInitializer(validVarInit, wasCreated);
  validVarExp->activate();
  assertTrue_1(validVarExp->isKnown());
  assertTrue_1(validVarExp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp != NULL);
  assertTrue_1(aryTemp->size() == 6);
  assertTrue_1(aryTemp->getElement(0, temp));
  assertTrue_1(temp == 0);
  assertTrue_1(aryTemp->getElement(1, temp));
  assertTrue_1(temp == 1);
  assertTrue_1(!aryTemp->getElement(2, temp));
  assertTrue_1(aryTemp->getElement(3, temp));
  assertTrue_1(temp == 3.14);
  assertTrue_1(aryTemp->getElement(4, temp));
  assertTrue_1(temp == 1e-100);
  assertTrue_1(aryTemp->getElement(5, temp));
  assertTrue_1(temp == 6.0221413e+23);
  realNc.storeVariable("vallid", validVarExp);

  // Variable reference tests

  xml_node validRefXml = doc.append_child("ArrayVariable");
  validRefXml.append_child(node_pcdata).set_value("vallid");

  Expression *validRefExp = createExpression(validRefXml, nc, wasCreated);
  assertTrue_1(validRefExp);
  assertTrue_1(!wasCreated);
  assertTrue_1(validRefExp == validVarExp);

  xml_node badNameRefXml = doc.append_child("ArrayVariable");
  badNameRefXml.append_child(node_pcdata).set_value("bad");
  try {
    Expression *badNameExp = createExpression(badNameRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect invalid variable reference");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected error" << std::endl;
  }

  xml_node wrongTypeRefXml = doc.append_child("RealVariable");
  wrongTypeRefXml.append_child(node_pcdata).set_value("empty");

  try {
    Expression *wrongTypeExp = createExpression(wrongTypeRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type mismatch");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected error" << std::endl;
  }

  return true;
}

static bool stringArrayVariableXmlParserTest()
{
  FactoryTestNodeConnector realNc;
  NodeConnector *nc = &realNc;

  bool wasCreated;
  StringArray const *aryTemp = NULL;
  std::string const *temp;

  xml_document doc;
  doc.set_name("realArrayVariableXmlParserTest");

  xml_node emptyXml = doc.append_child("DeclareArray");
  emptyXml.append_child("Name").append_child(node_pcdata).set_value("mty");
  emptyXml.append_child("Type").append_child(node_pcdata).set_value("String");
  emptyXml.append_child("MaxSize").append_child(node_pcdata).set_value("0");
  xml_node emptyInitXml = emptyXml.append_child("InitialValue").append_child("ArrayValue");
  emptyInitXml.append_attribute("Type").set_value("String");

  Expression *emptyExp = createExpression(emptyXml, nc, wasCreated);
  assertTrue_1(emptyExp);
  assertTrue_1(wasCreated);
  assertTrue_1(emptyExp->valueType() == STRING_ARRAY_TYPE);
  Expression *emptyInit = createExpression(emptyInitXml, nc, wasCreated);
  assertTrue_1(emptyInit);
  emptyExp->asAssignable()->setInitializer(emptyInit, wasCreated);
  emptyExp->activate();
  assertTrue_1(emptyExp->isKnown());
  assertTrue_1(emptyExp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp != NULL);
  assertTrue_1(aryTemp->size() == 0);
  realNc.storeVariable("mty", emptyExp);

  xml_node validValXml = doc.append_child("DeclareArray");
  validValXml.append_child("Name").append_child(node_pcdata).set_value("vlaid");
  validValXml.append_child("Type").append_child(node_pcdata).set_value("String");
  validValXml.append_child("MaxSize").append_child(node_pcdata).set_value("6");
  xml_node contentsElt = validValXml.append_child("InitialValue").append_child("ArrayValue");
  contentsElt.append_attribute("Type").set_value("String");
  contentsElt.append_child("StringValue").append_child(node_pcdata).set_value("0");
  contentsElt.append_child("StringValue").append_child(node_pcdata).set_value("1");
  contentsElt.append_child("StringValue").append_child(node_pcdata).set_value("UNKNOWN");
  contentsElt.append_child("StringValue").append_child(node_pcdata).set_value("3.14");
  contentsElt.append_child("StringValue").append_child(node_pcdata).set_value("1e-100");
  contentsElt.append_child("StringValue").append_child(node_pcdata).set_value("6.0221413e+23");

  Expression *validValExp = createExpression(validValXml, nc, wasCreated);
  assertTrue_1(validValExp);
  assertTrue_1(wasCreated);
  assertTrue_1(validValExp->valueType() == STRING_ARRAY_TYPE);
  Expression *validValInit = createExpression(contentsElt, nc, wasCreated);
  assertTrue_1(validValInit);
  validValExp->asAssignable()->setInitializer(validValInit, wasCreated);
  validValExp->activate();
  assertTrue_1(validValExp->isKnown());
  assertTrue_1(validValExp->getValuePointer(aryTemp));
  assertTrue_1(aryTemp != NULL);
  assertTrue_1(aryTemp->size() == 6);
  assertTrue_1(aryTemp->getElementPointer(0, temp));
  assertTrue_1(*temp == "0");
  assertTrue_1(aryTemp->getElementPointer(1, temp));
  assertTrue_1(*temp == "1");
  assertTrue_1(aryTemp->getElementPointer(2, temp));
  assertTrue_1(*temp == "UNKNOWN");
  assertTrue_1(aryTemp->getElementPointer(3, temp));
  assertTrue_1(*temp == "3.14");
  assertTrue_1(aryTemp->getElementPointer(4, temp));
  assertTrue_1(*temp == "1e-100");
  assertTrue_1(aryTemp->getElementPointer(5, temp));
  assertTrue_1(*temp == "6.0221413e+23");
  realNc.storeVariable("vlaid", validValExp);

  // Variable reference tests

  xml_node emptyRefXml = doc.append_child("ArrayVariable");
  emptyRefXml.append_child(node_pcdata).set_value("mty");

  Expression *emptyRefExp = createExpression(emptyRefXml, nc, wasCreated);
  assertTrue_1(emptyRefExp);
  assertTrue_1(!wasCreated);
  assertTrue_1(emptyRefExp == emptyExp);

  xml_node wrongTypeRefXml = doc.append_child("StringVariable");
  wrongTypeRefXml.append_child(node_pcdata).set_value("vlaid");

  try {
    Expression *wrongTypeExp = createExpression(wrongTypeRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect variable type mismatch");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  xml_node badRefXml = doc.append_child("ArrayVariable");
  badRefXml.append_child(node_pcdata).set_value("bad");

  try {
    Expression *badExp = createExpression(badRefXml, nc, wasCreated);
    assertTrue_2(false, "Failed to detect invalid variable reference");
  }
  catch (ParserException const & /* exc */) {
    std::cout << "Caught expected exception" << std::endl;
  }

  return true;
}

bool variableXmlParserTest()
{
  runTest(booleanVariableXmlParserTest);
  runTest(integerVariableXmlParserTest);
  runTest(realVariableXmlParserTest);
  runTest(stringVariableXmlParserTest);

  runTest(booleanArrayVariableXmlParserTest);
  runTest(integerArrayVariableXmlParserTest);
  runTest(realArrayVariableXmlParserTest);
  runTest(stringArrayVariableXmlParserTest);

  return true;
}

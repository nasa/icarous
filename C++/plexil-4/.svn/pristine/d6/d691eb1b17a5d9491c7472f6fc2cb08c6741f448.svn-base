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

#include "Assignable.hh"
#include "Assignment.hh"
#include "AssignmentNode.hh"
#include "Command.hh"
#include "CommandNode.hh"
#include "ExpressionFactory.hh"
#include "Node.hh"
#include "parseNode.hh"
#include "planLibrary.hh"
#include "TestSupport.hh"
#include "Update.hh"
#include "UpdateNode.hh"
#include "test/FactoryTestNodeConnector.hh"
#include "test/TransitionExternalInterface.hh"

#include "pugixml.hpp"

using namespace PLEXIL;

using pugi::xml_attribute;
using pugi::xml_document;
using pugi::xml_node;
using pugi::node_pcdata;

//
// Local utility functions
//

static void makePcdataElement(xml_node parent, char const *name, char const *value)
{
  xml_node result = parent.append_child(name);
  assertTrue_1(result);
  assertTrue_1(result.append_child(node_pcdata).set_value(value));
}

static xml_node makeNode(xml_node parent,
                         char const *nodeId,
                         char const *nodeType)
{
  xml_node result = parent.append_child("Node");
  assertTrue_1(result);
  assertTrue_1(result.append_attribute("NodeType").set_value(nodeType));
  makePcdataElement(result, "NodeId", nodeId);
  return result;
}

static xml_node makeDeclareVariable(xml_node varDeclsNode,
                                    char const *varName,
                                    char const *varType)
{
  xml_node decl = varDeclsNode.append_child("DeclareVariable");
  assertTrue_1(decl);
  makePcdataElement(decl, "Name", varName);
  makePcdataElement(decl, "Type", varType);
  return decl;
}

static xml_node makeDeclareArray(xml_node varDeclsNode,
                                 char const *varName,
                                 char const *eltType, 
                                 char const *maxSize)
{
  xml_node decl = varDeclsNode.append_child("DeclareArray");
  assertTrue_1(decl);
  makePcdataElement(decl, "Name", varName);
  makePcdataElement(decl, "Type", eltType);
  makePcdataElement(decl, "MaxSize", maxSize);
  return decl;
}

static xml_document *doc = NULL;

static bool emptyNodeXmlParserTest()
{
  assertTrue_1(doc);

  {
    xml_node minimal = makeNode(*doc, "minimal", "Empty");
    Node *minimalNode = parseNode(minimal, NULL);
    assertTrue_1(minimalNode);
    assertTrue_1(minimalNode->getType() == NodeType_Empty);
    assertTrue_1(minimalNode->getChildren().empty());
    finalizeNode(minimalNode, minimal);
    assertTrue_1(!minimalNode->getLocalVariables());
    delete minimalNode;
  }

  {
    xml_node comment = makeNode(*doc, "comment", "Empty");
    makePcdataElement(comment, "Comment", "This is a comment");
    Node *commentNode = parseNode(comment, NULL);
    assertTrue_1(commentNode);
    assertTrue_1(commentNode->getType() == NodeType_Empty);
    assertTrue_1(commentNode->getChildren().empty());
    finalizeNode(commentNode, comment);
    assertTrue_1(!commentNode->getLocalVariables());
    delete commentNode;
  }

  //
  // Test conditions
  //

  {
    xml_node preCond = makeNode(*doc, "preCond", "Empty");
    makePcdataElement(preCond.append_child("PreCondition"), "BooleanValue", "true");

    Node *preCondNode = parseNode(preCond, NULL);
    assertTrue_1(preCondNode);
    assertTrue_1(preCondNode->getType() == NodeType_Empty);
    assertTrue_1(preCondNode->getChildren().empty());
    finalizeNode(preCondNode, preCond);
    assertTrue_1(!preCondNode->getLocalVariables());
    Expression *sc = preCondNode->getPreCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete preCondNode;
  }

  {
    xml_node endCond = makeNode(*doc, "endCond", "Empty");
    makePcdataElement(endCond.append_child("EndCondition"), "BooleanValue", "true");

    Node *endCondNode = parseNode(endCond, NULL);
    assertTrue_1(endCondNode);
    assertTrue_1(endCondNode->getType() == NodeType_Empty);
    assertTrue_1(endCondNode->getChildren().empty());
    finalizeNode(endCondNode, endCond);
    assertTrue_1(!endCondNode->getLocalVariables());
    Expression *sc = endCondNode->getEndCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete endCondNode;
  }

  {
    xml_node exitCond = makeNode(*doc, "exitCond", "Empty");
    makePcdataElement(exitCond.append_child("ExitCondition"), "BooleanValue", "true");

    Node *exitCondNode = parseNode(exitCond, NULL);
    assertTrue_1(exitCondNode);
    assertTrue_1(exitCondNode->getType() == NodeType_Empty);
    assertTrue_1(exitCondNode->getChildren().empty());
    finalizeNode(exitCondNode, exitCond);
    assertTrue_1(!exitCondNode->getLocalVariables());
    Expression *sc = exitCondNode->getExitCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete exitCondNode;
  }

  {
    xml_node postCond = makeNode(*doc, "postCond", "Empty");
    makePcdataElement(postCond.append_child("PostCondition"), "BooleanValue", "true");

    Node *postCondNode = parseNode(postCond, NULL);
    assertTrue_1(postCondNode);
    assertTrue_1(postCondNode->getType() == NodeType_Empty);
    assertTrue_1(postCondNode->getChildren().empty());
    finalizeNode(postCondNode, postCond);
    assertTrue_1(!postCondNode->getLocalVariables());
    Expression *sc = postCondNode->getPostCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete postCondNode;
  }

  {
    xml_node skipCond = makeNode(*doc, "skipCond", "Empty");
    makePcdataElement(skipCond.append_child("SkipCondition"), "BooleanValue", "true");

    Node *skipCondNode = parseNode(skipCond, NULL);
    assertTrue_1(skipCondNode);
    assertTrue_1(skipCondNode->getType() == NodeType_Empty);
    assertTrue_1(skipCondNode->getChildren().empty());
    finalizeNode(skipCondNode, skipCond);
    assertTrue_1(!skipCondNode->getLocalVariables());
    Expression *sc = skipCondNode->getSkipCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete skipCondNode;
  }

  {
    xml_node startCond = makeNode(*doc, "startCond", "Empty");
    makePcdataElement(startCond.append_child("StartCondition"), "BooleanValue", "true");

    Node *startCondNode = parseNode(startCond, NULL);
    assertTrue_1(startCondNode);
    assertTrue_1(startCondNode->getType() == NodeType_Empty);
    assertTrue_1(startCondNode->getChildren().empty());
    finalizeNode(startCondNode, startCond);
    assertTrue_1(!startCondNode->getLocalVariables());
    Expression *sc = startCondNode->getStartCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete startCondNode;
  }

  {
    xml_node repeatCond = makeNode(*doc, "repeatCond", "Empty");
    makePcdataElement(repeatCond.append_child("RepeatCondition"), "BooleanValue", "true");

    Node *repeatCondNode = parseNode(repeatCond, NULL);
    assertTrue_1(repeatCondNode);
    assertTrue_1(repeatCondNode->getType() == NodeType_Empty);
    assertTrue_1(repeatCondNode->getChildren().empty());
    finalizeNode(repeatCondNode, repeatCond);
    assertTrue_1(!repeatCondNode->getLocalVariables());
    Expression *sc = repeatCondNode->getRepeatCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete repeatCondNode;
  }

  {
    xml_node invariantCond = makeNode(*doc, "invariantCond", "Empty");
    makePcdataElement(invariantCond.append_child("InvariantCondition"), "BooleanValue", "true");

    Node *invariantCondNode = parseNode(invariantCond, NULL);
    assertTrue_1(invariantCondNode);
    assertTrue_1(invariantCondNode->getType() == NodeType_Empty);
    assertTrue_1(invariantCondNode->getChildren().empty());
    finalizeNode(invariantCondNode, invariantCond);
    assertTrue_1(!invariantCondNode->getLocalVariables());
    Expression *sc = invariantCondNode->getInvariantCondition();
    assertTrue_1(sc);
    assertTrue_1(sc->isConstant());
    assertTrue_1(sc->valueType() == BOOLEAN_TYPE);
    bool scValue;
    assertTrue_1(sc->getValue(scValue));
    assertTrue_1(scValue);
    delete invariantCondNode;
  }

  {
    xml_node simpleVarDecl = makeNode(*doc, "simpleVarDecl", "Empty");
    xml_node decls = simpleVarDecl.append_child("VariableDeclarations");
    makeDeclareVariable(decls, "b", "Boolean");

    Node *simpleVarDeclNode = parseNode(simpleVarDecl, NULL);
    assertTrue_1(simpleVarDeclNode);
    assertTrue_1(simpleVarDeclNode->getType() == NodeType_Empty);
    assertTrue_1(simpleVarDeclNode->getChildren().empty());
    finalizeNode(simpleVarDeclNode, simpleVarDecl);
    assertTrue_1(simpleVarDeclNode->getLocalVariables()
                 && !simpleVarDeclNode->getLocalVariables()->empty());
    Expression *simpleVar = simpleVarDeclNode->findLocalVariable("b");
    assertTrue_1(simpleVar);
    assertTrue_1(simpleVar->valueType() == BOOLEAN_TYPE);
    assertTrue_1(!simpleVar->isActive());
    assertTrue_1(simpleVar->isAssignable());
    Assignable *simpleVarA = simpleVar->asAssignable();
    assertTrue_1(simpleVarA);
    assertTrue_1(simpleVarA->getBaseVariable() == simpleVar);
    assertTrue_1(simpleVarA->getNode() == simpleVarDeclNode);
    delete simpleVarDeclNode;
  }

  {
    xml_node initedVarDecl = makeNode(*doc, "initedVarDecl", "Empty");
    xml_node decls = initedVarDecl.append_child("VariableDeclarations");
    xml_node initedDecl = makeDeclareVariable(decls, "b", "Boolean");
    makePcdataElement(initedDecl.append_child("InitialValue"), "BooleanValue", "false");

    Node *initedVarDeclNode = parseNode(initedVarDecl, NULL);
    assertTrue_1(initedVarDeclNode);
    assertTrue_1(initedVarDeclNode->getType() == NodeType_Empty);
    assertTrue_1(initedVarDeclNode->getChildren().empty());
    finalizeNode(initedVarDeclNode, initedVarDecl);
    assertTrue_1(initedVarDeclNode->getLocalVariables()
                 && !initedVarDeclNode->getLocalVariables()->empty());
    Expression *initedVar = initedVarDeclNode->findLocalVariable("b");
    assertTrue_1(initedVar);
    assertTrue_1(initedVar->valueType() == BOOLEAN_TYPE);
    assertTrue_1(!initedVar->isActive());
    assertTrue_1(initedVar->isAssignable());
    initedVar->activate();
    bool initedVal = true;
    assertTrue_1(initedVar->getValue(initedVal));
    assertTrue_1(!initedVal);
    Assignable *initedVarA = initedVar->asAssignable();
    assertTrue_1(initedVarA);
    assertTrue_1(initedVarA->getBaseVariable() == initedVar);
    assertTrue_1(initedVarA->getNode() == initedVarDeclNode);
    delete initedVarDeclNode;
  }

  {
    xml_node simpleArrayVarDecl = makeNode(*doc, "simpleArrayVarDecl", "Empty");
    xml_node decls = simpleArrayVarDecl.append_child("VariableDeclarations");
    xml_node simpleArrayDecl = makeDeclareArray(decls, "ba", "Boolean", "1");

    Node *simpleArrayVarDeclNode = parseNode(simpleArrayVarDecl, NULL);
    assertTrue_1(simpleArrayVarDeclNode);
    assertTrue_1(simpleArrayVarDeclNode->getType() == NodeType_Empty);
    assertTrue_1(simpleArrayVarDeclNode->getChildren().empty());
    finalizeNode(simpleArrayVarDeclNode, simpleArrayVarDecl);
    assertTrue_1(simpleArrayVarDeclNode->getLocalVariables()
                 && !simpleArrayVarDeclNode->getLocalVariables()->empty());
    Expression *simpleArrayVar = simpleArrayVarDeclNode->findLocalVariable("ba");
    assertTrue_1(simpleArrayVar);
    assertTrue_1(simpleArrayVar->valueType() == BOOLEAN_ARRAY_TYPE);
    assertTrue_1(!simpleArrayVar->isActive());
    assertTrue_1(simpleArrayVar->isAssignable());
    Assignable *simpleArrayVarA = simpleArrayVar->asAssignable();
    assertTrue_1(simpleArrayVarA);
    assertTrue_1(simpleArrayVarA->getBaseVariable() == simpleArrayVar);
    assertTrue_1(simpleArrayVarA->getNode() == simpleArrayVarDeclNode);
    delete simpleArrayVarDeclNode;
  }

  {
    xml_node initedArrayVarDecl = makeNode(*doc, "initedArrayVarDecl", "Empty");
    xml_node decls = initedArrayVarDecl.append_child("VariableDeclarations");
    xml_node initedArrayDecl = makeDeclareArray(decls, "iba", "Boolean", "1");
    xml_node initXml = initedArrayDecl.append_child("InitialValue").append_child("ArrayValue");
    initXml.append_attribute("Type").set_value("Boolean");
    makePcdataElement(initXml, "BooleanValue", "false");

    Node *initedArrayVarDeclNode = parseNode(initedArrayVarDecl, NULL);
    assertTrue_1(initedArrayVarDeclNode);
    assertTrue_1(initedArrayVarDeclNode->getType() == NodeType_Empty);
    assertTrue_1(initedArrayVarDeclNode->getChildren().empty());
    finalizeNode(initedArrayVarDeclNode, initedArrayVarDecl);
    assertTrue_1(initedArrayVarDeclNode->getLocalVariables()
                 && !initedArrayVarDeclNode->getLocalVariables()->empty());
    Expression *initedArrayVar = initedArrayVarDeclNode->findLocalVariable("iba");
    assertTrue_1(initedArrayVar);
    assertTrue_1(initedArrayVar->valueType() == BOOLEAN_ARRAY_TYPE);
    assertTrue_1(!initedArrayVar->isActive());
    assertTrue_1(initedArrayVar->isAssignable());
    initedArrayVar->activate();
    BooleanArray const *initedArrayVal = NULL;
    assertTrue_1(initedArrayVar->getValuePointer(initedArrayVal));
    assertTrue_1(initedArrayVal);
    assertTrue_1(initedArrayVal->size() == 1);
    assertTrue_1(initedArrayVal->allElementsKnown());
    bool eltVal = true;
    assertTrue_1(initedArrayVal->getElement(0, eltVal));
    assertTrue_1(!eltVal);

    Assignable *initedArrayVarA = initedArrayVar->asAssignable();
    assertTrue_1(initedArrayVarA);
    assertTrue_1(initedArrayVarA->getBaseVariable() == initedArrayVar);
    assertTrue_1(initedArrayVarA->getNode() == initedArrayVarDeclNode);
    delete initedArrayVarDeclNode;
  }

  return true;
}

static bool listNodeXmlParserTest()
{
  assertTrue_1(doc);
  xml_node basicListXml = makeNode(*doc, "basicList", "NodeList");
  xml_node list = basicListXml.append_child("NodeBody").append_child("NodeList");
  assertTrue_1(list);

  {
    Node *emptyList = parseNode(basicListXml, NULL);
    assertTrue_1(emptyList);
    assertTrue_1(emptyList->getType() == NodeType_NodeList);
    assertTrue_1(emptyList->getNodeId() == "basicList");
    finalizeNode(emptyList, basicListXml);
    assertTrue_1(emptyList->getChildren().empty());
    assertTrue_1(!emptyList->getLocalVariables());
    delete emptyList;
  }

  {
    xml_node oneListXml = doc->append_copy(basicListXml);
    assertTrue_1(oneListXml);
    assertTrue_1(oneListXml.child("NodeId").first_child().set_value("oneList"));
    xml_node oneListKid = makeNode(oneListXml.child("NodeBody").child("NodeList"),
                                   "oneListKid", "Empty");

    Node *oneList = parseNode(oneListXml, NULL);
    assertTrue_1(oneList);
    assertTrue_1(oneList->getType() == NodeType_NodeList);
    assertTrue_1(oneList->getNodeId() == "oneList");
    assertTrue_1(!oneList->getChildren().empty());
    assertTrue_1(oneList->getChildren().size() == 1);
    Node *kid = oneList->getChildren().front();
    assertTrue_1(kid->getType() == NodeType_Empty); 
    assertTrue_1(kid->getNodeId() == "oneListKid");
    assertTrue_1(kid->getChildren().empty());
    finalizeNode(oneList, oneListXml);
    assertTrue_1(!oneList->getLocalVariables());
    assertTrue_1(!kid->getLocalVariables());
    delete oneList;
  }

  {
    xml_node anotherListXml = doc->append_copy(basicListXml);
    assertTrue_1(anotherListXml);
    assertTrue_1(anotherListXml.child("NodeId").first_child().set_value("anotherList"));
    xml_node theList = anotherListXml.child("NodeBody").child("NodeList");
    assertTrue_1(theList);
    xml_node anotherListKid = makeNode(theList, "anotherListKid0", "Empty");
    xml_node yaListKid = makeNode(theList, "anotherListKid1", "Empty");

    Node *anotherList = parseNode(anotherListXml, NULL);
    assertTrue_1(anotherList);
    assertTrue_1(anotherList->getType() == NodeType_NodeList);
    assertTrue_1(anotherList->getNodeId() == "anotherList");
    assertTrue_1(!anotherList->getChildren().empty());
    assertTrue_1(anotherList->getChildren().size() == 2);
    Node *kid0 = anotherList->getChildren().at(0);
    assertTrue_1(kid0->getType() == NodeType_Empty);
    assertTrue_1(kid0->getNodeId() == "anotherListKid0");
    assertTrue_1(kid0->getChildren().empty());
    Node *kid1 = anotherList->getChildren().at(1);
    assertTrue_1(kid1->getType() == NodeType_Empty);
    assertTrue_1(kid1->getNodeId() == "anotherListKid1");
    assertTrue_1(kid1->getChildren().empty());
    finalizeNode(anotherList, anotherListXml);
    assertTrue_1(!anotherList->getLocalVariables());
    assertTrue_1(!kid0->getLocalVariables());
    assertTrue_1(!kid1->getLocalVariables());
    delete anotherList;
  }

  {
    xml_node varAccessListXml = doc->append_copy(basicListXml);
    assertTrue_1(varAccessListXml);
    assertTrue_1(varAccessListXml.child("NodeId").first_child().set_value("varAccessList"));
    xml_node rootDecls = varAccessListXml.append_child("VariableDeclarations");
    assertTrue_1(rootDecls);
    xml_node decl0 = makeDeclareVariable(rootDecls, "foo", "Integer");
    makePcdataElement(decl0.append_child("InitialValue"), "IntegerValue", "42");
    xml_node varAccessListKid = makeNode(varAccessListXml.child("NodeBody").child("NodeList"),
                                         "varAccessListKid", "Empty");
    assertTrue_1(varAccessListKid);
    xml_node kidDecls = varAccessListKid.append_child("VariableDeclarations");
    assertTrue_1(kidDecls);
    xml_node decl1 = makeDeclareVariable(kidDecls, "bar", "Integer");
    assertTrue_1(decl1);
    makePcdataElement(decl1.append_child("InitialValue"), "IntegerVariable", "foo");

    Node *varAccessList = parseNode(varAccessListXml, NULL);
    assertTrue_1(varAccessList);
    assertTrue_1(varAccessList->getType() == NodeType_NodeList);
    assertTrue_1(varAccessList->getNodeId() == "varAccessList");
    assertTrue_1(!varAccessList->getChildren().empty());
    assertTrue_1(varAccessList->getChildren().size() == 1);
    Node *kid = varAccessList->getChildren().front();
    assertTrue_1(kid->getType() == NodeType_Empty); 
    assertTrue_1(kid->getNodeId() == "varAccessListKid");
    assertTrue_1(kid->getChildren().empty());
    finalizeNode(varAccessList, varAccessListXml);
    assertTrue_1(varAccessList->getLocalVariables()
                 && !varAccessList->getLocalVariables()->empty());
    assertTrue_1(varAccessList->getLocalVariables()->size() == 1);
    Expression *rootVar = varAccessList->getLocalVariables()->front();
    assertTrue_1(rootVar);
    assertTrue_1(rootVar->valueType() == INTEGER_TYPE);
    assertTrue_1(rootVar->asAssignable()->getNode() == varAccessList);
    assertTrue_1(rootVar == varAccessList->findVariable("foo"));
    assertTrue_1(!varAccessList->findVariable("bar"));

    assertTrue_1(kid->getLocalVariables()
                 && !kid->getLocalVariables()->empty());
    assertTrue_1(kid->getLocalVariables()->size() == 1);
    Expression *kidVar = kid->getLocalVariables()->front();
    assertTrue_1(kidVar);
    assertTrue_1(kidVar->valueType() == INTEGER_TYPE);
    assertTrue_1(kidVar->asAssignable()->getNode() == kid);
    assertTrue_1(rootVar == kid->findVariable("foo"));
    assertTrue_1(kidVar == kid->findVariable("bar"));

    delete varAccessList;
  }

  {
    xml_node nodeRefTestXml = doc->append_copy(basicListXml);
    assertTrue_1(nodeRefTestXml);
    assertTrue_1(nodeRefTestXml.child("NodeId").first_child().set_value("nodeRefTest"));
    xml_node eq = nodeRefTestXml.append_child("ExitCondition").append_child("EQInternal");
    assertTrue_1(eq);
    makePcdataElement(eq.append_child("NodeOutcomeVariable"), "NodeId", "nodeRefTestKid");
    makePcdataElement(eq, "NodeOutcomeValue", "FAILURE");
    xml_node nodeRefTestKid = makeNode(nodeRefTestXml.child("NodeBody").child("NodeList"),
                                       "nodeRefTestKid",
                                       "Empty");
    xml_node neq = nodeRefTestKid.append_child("InvariantCondition").append_child("NEInternal");
    assertTrue_1(neq);
    makePcdataElement(neq.append_child("NodeStateVariable"), "NodeId", "nodeRefTest");
    makePcdataElement(neq, "NodeStateValue", "EXECUTING");
    Node *nodeRefTest = parseNode(nodeRefTestXml, NULL);
    assertTrue_1(nodeRefTest);
    assertTrue_1(nodeRefTest->getType() == NodeType_NodeList);
    assertTrue_1(nodeRefTest->getNodeId() == "nodeRefTest");
    assertTrue_1(!nodeRefTest->getChildren().empty());
    assertTrue_1(nodeRefTest->getChildren().size() == 1);
    Node *kid = nodeRefTest->getChildren().front();
    assertTrue_1(kid->getType() == NodeType_Empty); 
    assertTrue_1(kid->getNodeId() == "nodeRefTestKid");
    assertTrue_1(kid->getChildren().empty());
    finalizeNode(nodeRefTest, nodeRefTestXml);
    assertTrue_1(!nodeRefTest->getLocalVariables());
    assertTrue_1(!kid->getLocalVariables());
    assertTrue_1(nodeRefTest->getExitCondition());
    assertTrue_1(nodeRefTest->getExitCondition()->valueType() == BOOLEAN_TYPE);
    assertTrue_1(kid->getInvariantCondition());
    assertTrue_1(kid->getInvariantCondition()->valueType() == BOOLEAN_TYPE);
    delete nodeRefTest;
  }

  return true;
}

static bool assignmentNodeXmlParserTest()
{
  assertTrue_1(doc);

  // Scalar assignment
  {
    xml_node listNodeXml = makeNode(*doc, "listNode", "NodeList");
    xml_node listNodeDecls = listNodeXml.append_child("VariableDeclarations");
    xml_node decl0 = makeDeclareVariable(listNodeDecls, "foo", "Integer");
    xml_node listNodeList = listNodeXml.append_child("NodeBody").append_child("NodeList");

    xml_node basicAssnXml = makeNode(listNodeList, "basicAssn", "Assignment");
    xml_node assnXml = basicAssnXml.append_child("NodeBody").append_child("Assignment");
    makePcdataElement(assnXml, "IntegerVariable", "foo");
    makePcdataElement(assnXml.append_child("NumericRHS"), "IntegerValue", "2");

    Node *listNode = parseNode(listNodeXml, NULL);
    assertTrue_1(listNode);
    assertTrue_1(listNode->getType() == NodeType_NodeList);
    assertTrue_1(!listNode->getChildren().empty());
    assertTrue_1(listNode->getChildren().size() == 1);
    assertTrue_1(listNode->getLocalVariables()
                 && !listNode->getLocalVariables()->empty());
    assertTrue_1(listNode->getLocalVariables()->size() == 1);

    Node *basicAssn = listNode->getChildren().front();
    assertTrue_1(basicAssn);
    assertTrue_1(basicAssn->getType() == NodeType_Assignment);
    assertTrue_1(basicAssn->getChildren().empty());
    assertTrue_1(!basicAssn->getLocalVariables());

    finalizeNode(listNode, listNodeXml);
    AssignmentNode *anode = dynamic_cast<AssignmentNode *>(basicAssn);
    assertTrue_1(anode);
    Assignment *assn = anode->getAssignment();
    assertTrue_1(assn);

    Expression *fooVar = listNode->findLocalVariable("foo");
    assertTrue_1(fooVar);
    assertTrue_1(fooVar->valueType() == INTEGER_TYPE);
    assertTrue_1(assn->getDest() == fooVar);

    delete listNode;
  }

  // Scalar assignment w/ priority
  {
    xml_node listNode1Xml = makeNode(*doc, "listNode1", "NodeList");
    xml_node listNode1Decls = listNode1Xml.append_child("VariableDeclarations");
    xml_node decl0 = makeDeclareVariable(listNode1Decls, "foo", "Integer");
    xml_node listNode1List = listNode1Xml.append_child("NodeBody").append_child("NodeList");

    xml_node basicAssnXml = makeNode(listNode1List, "basicAssn", "Assignment");
    makePcdataElement(basicAssnXml, "Priority", "69");
    xml_node assnXml = basicAssnXml.append_child("NodeBody").append_child("Assignment");
    makePcdataElement(assnXml, "IntegerVariable", "foo");
    makePcdataElement(assnXml.append_child("NumericRHS"), "IntegerValue", "2");

    Node *listNode1 = parseNode(listNode1Xml, NULL);
    assertTrue_1(listNode1);
    assertTrue_1(listNode1->getType() == NodeType_NodeList);
    assertTrue_1(!listNode1->getChildren().empty());
    assertTrue_1(listNode1->getChildren().size() == 1);
    assertTrue_1(listNode1->getLocalVariables()
                 && !listNode1->getLocalVariables()->empty());
    assertTrue_1(listNode1->getLocalVariables()->size() == 1);

    Node *basicAssn = listNode1->getChildren().front();
    assertTrue_1(basicAssn);
    assertTrue_1(basicAssn->getType() == NodeType_Assignment);
    assertTrue_1(basicAssn->getChildren().empty());
    assertTrue_1(!basicAssn->getLocalVariables());

    finalizeNode(listNode1, listNode1Xml);
    AssignmentNode *anode = dynamic_cast<AssignmentNode *>(basicAssn);
    assertTrue_1(anode);
    assertTrue_1(anode->getPriority() == 69);
    Assignment *assn = anode->getAssignment();
    assertTrue_1(assn);

    Expression *fooVar = listNode1->findLocalVariable("foo");
    assertTrue_1(fooVar);
    assertTrue_1(fooVar->valueType() == INTEGER_TYPE);
    assertTrue_1(assn->getDest() == fooVar);

    delete listNode1;
  }

  // Array assignment
  {
    xml_node listNode2Xml = makeNode(*doc, "listNode2", "NodeList");
    xml_node listNode2Decls = listNode2Xml.append_child("VariableDeclarations");
    xml_node decl1 = makeDeclareArray(listNode2Decls, "bar", "Integer", "2");
    xml_node listNode2List = listNode2Xml.append_child("NodeBody").append_child("NodeList");

    xml_node arrayAssnXml = makeNode(listNode2List, "arrayAssn", "Assignment");
    xml_node assnXml = arrayAssnXml.append_child("NodeBody").append_child("Assignment");
    makePcdataElement(assnXml, "ArrayVariable", "bar");
    xml_node arrayXml = assnXml.append_child("ArrayRHS").append_child("ArrayValue");
    arrayXml.append_attribute("Type").set_value("Integer");
    makePcdataElement(arrayXml, "IntegerValue", "2");
    makePcdataElement(arrayXml, "IntegerValue", "3");

    Node *listNode2 = parseNode(listNode2Xml, NULL);
    assertTrue_1(listNode2);
    assertTrue_1(listNode2->getType() == NodeType_NodeList);
    assertTrue_1(!listNode2->getChildren().empty());
    assertTrue_1(listNode2->getChildren().size() == 1);
    assertTrue_1(listNode2->getLocalVariables()
                 && !listNode2->getLocalVariables()->empty());
    assertTrue_1(listNode2->getLocalVariables()->size() == 1);

    Node *arrayAssn = listNode2->getChildren().front();
    assertTrue_1(arrayAssn);
    assertTrue_1(arrayAssn->getType() == NodeType_Assignment);
    assertTrue_1(arrayAssn->getChildren().empty());
    assertTrue_1(!arrayAssn->getLocalVariables());

    finalizeNode(listNode2, listNode2Xml);
    AssignmentNode *aanode = dynamic_cast<AssignmentNode *>(arrayAssn);
    assertTrue_1(aanode);
    Assignment *aassn = aanode->getAssignment();
    assertTrue_1(aassn);

    Expression *barVar = listNode2->findLocalVariable("bar");
    assertTrue_1(barVar);
    assertTrue_1(barVar->valueType() == INTEGER_ARRAY_TYPE);
    assertTrue_1(aassn->getDest() == barVar);

    delete listNode2;
  }

  // Array element assignment
  {
    xml_node listNode3Xml = makeNode(*doc, "listNode3", "NodeList");
    xml_node listNode3Decls = listNode3Xml.append_child("VariableDeclarations");
    xml_node decl1 = makeDeclareArray(listNode3Decls, "baz", "Integer", "2");
    xml_node listNode3List = listNode3Xml.append_child("NodeBody").append_child("NodeList");

    xml_node arrayAssnXml = makeNode(listNode3List, "arrayAssn", "Assignment");
    xml_node assnXml = arrayAssnXml.append_child("NodeBody").append_child("Assignment");
    xml_node elemXml = assnXml.append_child("ArrayElement");
    makePcdataElement(elemXml, "Name", "baz");
    makePcdataElement(elemXml.append_child("Index"), "IntegerValue", "0");
    makePcdataElement(assnXml.append_child("NumericRHS"), "IntegerValue", "3");

    Node *listNode3 = parseNode(listNode3Xml, NULL);
    assertTrue_1(listNode3);
    assertTrue_1(listNode3->getType() == NodeType_NodeList);
    assertTrue_1(!listNode3->getChildren().empty());
    assertTrue_1(listNode3->getChildren().size() == 1);
    assertTrue_1(listNode3->getLocalVariables()
                 && !listNode3->getLocalVariables()->empty());
    assertTrue_1(listNode3->getLocalVariables()->size() == 1);

    Node *arrayAssn = listNode3->getChildren().front();
    assertTrue_1(arrayAssn);
    assertTrue_1(arrayAssn->getType() == NodeType_Assignment);
    assertTrue_1(arrayAssn->getChildren().empty());
    assertTrue_1(!arrayAssn->getLocalVariables());

    finalizeNode(listNode3, listNode3Xml);
    AssignmentNode *aanode = dynamic_cast<AssignmentNode *>(arrayAssn);
    assertTrue_1(aanode);
    Assignment *aassn = aanode->getAssignment();
    assertTrue_1(aassn);

    Expression *bazVar = listNode3->findLocalVariable("baz");
    assertTrue_1(bazVar);
    assertTrue_1(bazVar->valueType() == INTEGER_ARRAY_TYPE);
    assertTrue_1(aassn->getDest()->valueType() == INTEGER_TYPE);
    assertTrue_1(aassn->getDest() != bazVar);
    assertTrue_1(aassn->getDest()->asAssignable()->getBaseVariable() == bazVar);

    delete listNode3;
  }

  return true;
}

// Local utility for Command node tests

static xml_node makeLiteralResource(xml_node resList, char const *name, char const *priority)
{
  xml_node result = resList.append_child("Resource");
  assertTrue_1(result);
  makePcdataElement(result.append_child("ResourceName"), "StringValue", name);
  makePcdataElement(result.append_child("ResourcePriority"), "IntegerValue", priority);
  return result;
}

static bool commandNodeXmlParserTest()
{
  assertTrue_1(doc);

  // Simple
  {
    xml_node basicCmdXml = makeNode(*doc, "basicCmd", "Command");
    xml_node cmdXml = basicCmdXml.append_child("NodeBody").append_child("Command");
    makePcdataElement(cmdXml.append_child("Name"),"StringValue", "foo");

    Node *basicCmd = parseNode(basicCmdXml, NULL);
    assertTrue_1(basicCmd);
    assertTrue_1(basicCmd->getType() == NodeType_Command);
    assertTrue_1(basicCmd->getNodeId() == "basicCmd");
    assertTrue_1(basicCmd->getChildren().empty());
    assertTrue_1(!basicCmd->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(basicCmd);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(basicCmd, basicCmdXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "foo");
    assertTrue_1(cmd->getArgValues().empty());
    assertTrue_1(!cmd->getDest());
    cmd->fixResourceValues();
    assertTrue_1(cmd->getResourceValues().empty());

    delete basicCmd;
  }

  // With resources
  {
    xml_node cmdWithResourcesXml = makeNode(*doc, "cmdWithResources", "Command");
    xml_node cmdXml = cmdWithResourcesXml.append_child("NodeBody").append_child("Command");

    xml_node resListXml = cmdXml.append_child("ResourceList");
    xml_node res1Xml = makeLiteralResource(resListXml, "Resource1", "1024");
    xml_node res2Xml = makeLiteralResource(resListXml, "Resource2", "127");
    makePcdataElement(res2Xml.append_child("ResourceLowerBound"), "IntegerValue", "1");
    makePcdataElement(res2Xml.append_child("ResourceUpperBound"), "IntegerValue", "10");
    makePcdataElement(res2Xml.append_child("ResourceReleaseAtTermination"), "BooleanValue", "false");

    makePcdataElement(cmdXml.append_child("Name"), "StringValue", "goo");

    Node *cmdWithResources = parseNode(cmdWithResourcesXml, NULL);
    assertTrue_1(cmdWithResources);
    assertTrue_1(cmdWithResources->getType() == NodeType_Command);
    assertTrue_1(cmdWithResources->getNodeId() == "cmdWithResources");
    assertTrue_1(cmdWithResources->getChildren().empty());
    assertTrue_1(!cmdWithResources->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(cmdWithResources);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(cmdWithResources, cmdWithResourcesXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "goo");
    assertTrue_1(cmd->getArgValues().empty());
    assertTrue_1(!cmd->getDest());

    cmd->fixResourceValues();
    ResourceValueList res = cmd->getResourceValues();
    assertTrue_1(!res.empty());
    ResourceValue &res1 = res[0];
    assertTrue_1(res1.lowerBound == 1.0);
    assertTrue_1(res1.upperBound == 1.0);
    assertTrue_1(res1.releaseAtTermination);
    assertTrue_1(res1.name == "Resource1");
    assertTrue_1(res1.priority == 1024);
    ResourceValue &res2 = res[1];
    assertTrue_1(res2.name == Value("Resource2"));
    assertTrue_1(res2.priority == 127);
    assertTrue_1(res2.lowerBound == 1.0);
    assertTrue_1(res2.upperBound == 10.0);
    assertTrue_1(!res2.releaseAtTermination);

    delete cmdWithResources;
  }

  // With parameters
  {
    xml_node cmdWithArgsXml = makeNode(*doc, "cmdWithArgs", "Command");
    xml_node cmdXml = cmdWithArgsXml.append_child("NodeBody").append_child("Command");
    makePcdataElement(cmdXml.append_child("Name"), "StringValue", "hoo");
    xml_node argsXml = cmdXml.append_child("Arguments");
    makePcdataElement(argsXml, "BooleanValue", "true");
    makePcdataElement(argsXml, "IntegerValue", "5");
    makePcdataElement(argsXml, "RealValue", "3.14");
    makePcdataElement(argsXml, "StringValue", "hi there");

    Node *cmdWithArgs = parseNode(cmdWithArgsXml, NULL);
    assertTrue_1(cmdWithArgs);
    assertTrue_1(cmdWithArgs->getType() == NodeType_Command);
    assertTrue_1(cmdWithArgs->getNodeId() == "cmdWithArgs");
    assertTrue_1(cmdWithArgs->getChildren().empty());
    assertTrue_1(!cmdWithArgs->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(cmdWithArgs);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(cmdWithArgs, cmdWithArgsXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "hoo");
    assertTrue_1(!cmd->getDest());

    std::vector<Value> const &args = cmd->getArgValues();
    assertTrue_1(!args.empty());
    assertTrue_1(args.size() == 4);
    assertTrue_1(args[0] == Value(true));
    assertTrue_1(args[1] == Value((int32_t) 5));
    assertTrue_1(args[2] == Value(3.14));
    assertTrue_1(args[3] == Value("hi there"));

    cmd->fixResourceValues();
    assertTrue_1(cmd->getResourceValues().empty());

    delete cmdWithArgs;
  }

  // With return value
  {
    xml_node listNodeXml = makeNode(*doc, "listNode", "NodeList");
    xml_node listNodeDecls = listNodeXml.append_child("VariableDeclarations");
    xml_node decl0 = makeDeclareVariable(listNodeDecls, "foo", "Integer");
    xml_node listNodeList = listNodeXml.append_child("NodeBody").append_child("NodeList");

    xml_node cmdWithReturnXml = makeNode(listNodeList, "cmdWithReturn", "Command");
    xml_node cmdXml = cmdWithReturnXml.append_child("NodeBody").append_child("Command");
    makePcdataElement(cmdXml, "IntegerVariable", "foo");
    makePcdataElement(cmdXml.append_child("Name"), "StringValue", "bar");
    Node *listNode = parseNode(listNodeXml, NULL);
    assertTrue_1(listNode);
    assertTrue_1(listNode->getType() == NodeType_NodeList);
    assertTrue_1(!listNode->getChildren().empty());
    assertTrue_1(listNode->getChildren().size() == 1);
    assertTrue_1(listNode->getLocalVariables()
                 && !listNode->getLocalVariables()->empty());
    assertTrue_1(listNode->getLocalVariables()->size() == 1);

    Node *cmdWithReturn = listNode->getChildren().front();
    assertTrue_1(cmdWithReturn);
    assertTrue_1(cmdWithReturn->getType() == NodeType_Command);
    assertTrue_1(cmdWithReturn->getNodeId() == "cmdWithReturn");
    assertTrue_1(cmdWithReturn->getChildren().empty());
    assertTrue_1(!cmdWithReturn->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(cmdWithReturn);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(listNode, listNodeXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "bar");
    assertTrue_1(cmd->getArgValues().empty());
    Expression *dest = cmd->getDest();
    assertTrue_1(dest);
    assertTrue_1(dest->valueType() == INTEGER_TYPE);
    assertTrue_1(dest->isAssignable());
    cmd->fixResourceValues();
    assertTrue_1(cmd->getResourceValues().empty());

    delete listNode;
  }

  // With return value and parameters
  {
    xml_node listNodeXml = makeNode(*doc, "listNode", "NodeList");
    xml_node listNodeDecls = listNodeXml.append_child("VariableDeclarations");
    xml_node decl0 = makeDeclareVariable(listNodeDecls, "foo", "Integer");
    xml_node listNodeList = listNodeXml.append_child("NodeBody").append_child("NodeList");

    xml_node cmdWithReturnXml = makeNode(listNodeList, "cmdWithReturn", "Command");
    xml_node cmdXml = cmdWithReturnXml.append_child("NodeBody").append_child("Command");
    makePcdataElement(cmdXml, "IntegerVariable", "foo");
    makePcdataElement(cmdXml.append_child("Name"), "StringValue", "bar");
    xml_node argsXml = cmdXml.append_child("Arguments");
    makePcdataElement(argsXml, "BooleanValue", "true");
    makePcdataElement(argsXml, "IntegerValue", "5");
    makePcdataElement(argsXml, "RealValue", "3.14");
    makePcdataElement(argsXml, "StringValue", "hi there");

    Node *listNode = parseNode(listNodeXml, NULL);
    assertTrue_1(listNode);
    assertTrue_1(listNode->getType() == NodeType_NodeList);
    assertTrue_1(!listNode->getChildren().empty());
    assertTrue_1(listNode->getChildren().size() == 1);
    assertTrue_1(listNode->getLocalVariables()
                 && !listNode->getLocalVariables()->empty());
    assertTrue_1(listNode->getLocalVariables()->size() == 1);

    Node *cmdWithReturn = listNode->getChildren().front();
    assertTrue_1(cmdWithReturn);
    assertTrue_1(cmdWithReturn->getType() == NodeType_Command);
    assertTrue_1(cmdWithReturn->getNodeId() == "cmdWithReturn");
    assertTrue_1(cmdWithReturn->getChildren().empty());
    assertTrue_1(!cmdWithReturn->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(cmdWithReturn);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(listNode, listNodeXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "bar");
    Expression *dest = cmd->getDest();
    assertTrue_1(dest);
    assertTrue_1(dest->valueType() == INTEGER_TYPE);
    assertTrue_1(dest->isAssignable());

    std::vector<Value> const &args = cmd->getArgValues();
    assertTrue_1(!args.empty());
    assertTrue_1(args.size() == 4);
    assertTrue_1(args[0] == Value(true));
    assertTrue_1(args[1] == Value((int32_t) 5));
    assertTrue_1(args[2] == Value(3.14));
    assertTrue_1(args[3] == Value("hi there"));

    cmd->fixResourceValues();
    assertTrue_1(cmd->getResourceValues().empty());

    delete listNode;
  }

  // With resource and return value
  {
    xml_node listNodeXml = makeNode(*doc, "listNode", "NodeList");
    xml_node listNodeDecls = listNodeXml.append_child("VariableDeclarations");
    xml_node decl0 = makeDeclareVariable(listNodeDecls, "foo", "Integer");
    xml_node listNodeList = listNodeXml.append_child("NodeBody").append_child("NodeList");

    xml_node cmdRetResXml = makeNode(listNodeList, "cmdRetRes", "Command");
    xml_node cmdXml = cmdRetResXml.append_child("NodeBody").append_child("Command");

    xml_node resListXml = cmdXml.append_child("ResourceList");
    xml_node res1Xml = makeLiteralResource(resListXml, "Resource1", "1024");
    xml_node res2Xml = makeLiteralResource(resListXml, "Resource2", "127");
    makePcdataElement(res2Xml.append_child("ResourceLowerBound"), "IntegerValue", "1");
    makePcdataElement(res2Xml.append_child("ResourceUpperBound"), "IntegerValue", "10");
    makePcdataElement(res2Xml.append_child("ResourceReleaseAtTermination"), "BooleanValue", "false");

    makePcdataElement(cmdXml, "IntegerVariable", "foo");
    makePcdataElement(cmdXml.append_child("Name"), "StringValue", "har");
    Node *listNode = parseNode(listNodeXml, NULL);
    assertTrue_1(listNode);
    assertTrue_1(listNode->getType() == NodeType_NodeList);
    assertTrue_1(!listNode->getChildren().empty());
    assertTrue_1(listNode->getChildren().size() == 1);
    assertTrue_1(listNode->getLocalVariables()
                 && !listNode->getLocalVariables()->empty());
    assertTrue_1(listNode->getLocalVariables()->size() == 1);

    Node *cmdRetRes = listNode->getChildren().front();
    assertTrue_1(cmdRetRes);
    assertTrue_1(cmdRetRes->getType() == NodeType_Command);
    assertTrue_1(cmdRetRes->getNodeId() == "cmdRetRes");
    assertTrue_1(cmdRetRes->getChildren().empty());
    assertTrue_1(!cmdRetRes->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(cmdRetRes);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(listNode, listNodeXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "har");
    assertTrue_1(cmd->getArgValues().empty());
    Expression *dest = cmd->getDest();
    assertTrue_1(dest);
    assertTrue_1(dest->valueType() == INTEGER_TYPE);
    assertTrue_1(dest->isAssignable());

    cmd->fixResourceValues();
    ResourceValueList res = cmd->getResourceValues();
    assertTrue_1(!res.empty());
    ResourceValue &res1 = res[0];
    assertTrue_1(res1.lowerBound == 1.0);
    assertTrue_1(res1.upperBound == 1.0);
    assertTrue_1(res1.releaseAtTermination);
    assertTrue_1(res1.name == "Resource1");
    assertTrue_1(res1.priority == 1024);
    ResourceValue &res2 = res[1];
    assertTrue_1(res2.name == "Resource2");
    assertTrue_1(res2.priority == 127);
    assertTrue_1(res2.lowerBound == 1.0);
    assertTrue_1(res2.upperBound == 10.0);
    assertTrue_1(!res2.releaseAtTermination);

    delete listNode;
  }

  // With kitchen sink
  {
    xml_node listNodeXml = makeNode(*doc, "listNode", "NodeList");
    xml_node listNodeDecls = listNodeXml.append_child("VariableDeclarations");
    xml_node decl0 = makeDeclareVariable(listNodeDecls, "foo", "Integer");
    xml_node listNodeList = listNodeXml.append_child("NodeBody").append_child("NodeList");

    xml_node kitchenSinkXml = makeNode(listNodeList, "kitchenSink", "Command");
    xml_node cmdXml = kitchenSinkXml.append_child("NodeBody").append_child("Command");

    xml_node resListXml = cmdXml.append_child("ResourceList");
    xml_node res1Xml = makeLiteralResource(resListXml, "Resource1", "1024");
    xml_node res2Xml = makeLiteralResource(resListXml, "Resource2", "127");
    makePcdataElement(res2Xml.append_child("ResourceLowerBound"), "IntegerValue", "1");
    makePcdataElement(res2Xml.append_child("ResourceUpperBound"), "IntegerValue", "10");
    makePcdataElement(res2Xml.append_child("ResourceReleaseAtTermination"), "BooleanValue", "false");

    makePcdataElement(cmdXml, "IntegerVariable", "foo");

    makePcdataElement(cmdXml.append_child("Name"), "StringValue", "har");

    xml_node argsXml = cmdXml.append_child("Arguments");
    makePcdataElement(argsXml, "BooleanValue", "true");
    makePcdataElement(argsXml, "IntegerVariable", "foo");
    makePcdataElement(argsXml, "RealValue", "3.14");
    makePcdataElement(argsXml, "StringValue", "hi there");

    Node *listNode = parseNode(listNodeXml, NULL);
    assertTrue_1(listNode);
    assertTrue_1(listNode->getType() == NodeType_NodeList);
    assertTrue_1(!listNode->getChildren().empty());
    assertTrue_1(listNode->getChildren().size() == 1);
    assertTrue_1(listNode->getLocalVariables()
                 && !listNode->getLocalVariables()->empty());
    assertTrue_1(listNode->getLocalVariables()->size() == 1);

    Node *kitchenSink = listNode->getChildren().front();
    assertTrue_1(kitchenSink);
    assertTrue_1(kitchenSink->getType() == NodeType_Command);
    assertTrue_1(kitchenSink->getNodeId() == "kitchenSink");
    assertTrue_1(kitchenSink->getChildren().empty());
    assertTrue_1(!kitchenSink->getLocalVariables());
    CommandNode *cnode = dynamic_cast<CommandNode *>(kitchenSink);
    assertTrue_1(cnode);
    Command *cmd = cnode->getCommand();
    assertTrue_1(cmd);
    
    finalizeNode(listNode, listNodeXml);
    assertTrue_1(!cmd->isActive());
    cmd->activate();
    cmd->fixValues();
    assertTrue_1(cmd->getName() == "har");

    Expression *dest = cmd->getDest();
    assertTrue_1(dest);
    assertTrue_1(dest->valueType() == INTEGER_TYPE);
    assertTrue_1(dest->isAssignable());

    std::vector<Value> const &args = cmd->getArgValues();
    assertTrue_1(!args.empty());
    assertTrue_1(args.size() == 4);
    assertTrue_1(args[0] == Value(true));
    assertTrue_1(!args[1].isKnown());
    assertTrue_1(args[2] == Value(3.14));
    assertTrue_1(args[3] == Value("hi there"));

    cmd->fixResourceValues();
    ResourceValueList res = cmd->getResourceValues();
    assertTrue_1(!res.empty());
    ResourceValue &res1 = res[0];
    assertTrue_1(res1.lowerBound == 1.0);
    assertTrue_1(res1.upperBound == 1.0);
    assertTrue_1(res1.releaseAtTermination);
    assertTrue_1(res1.name == "Resource1");
    assertTrue_1(res1.priority == 1024);
    ResourceValue &res2 = res[1];
    assertTrue_1(res2.name == "Resource2");
    assertTrue_1(res2.priority == 127);
    assertTrue_1(res2.lowerBound == 1.0);
    assertTrue_1(res2.upperBound == 10.0);
    assertTrue_1(!res2.releaseAtTermination);

    delete listNode;
  }

  return true;
}

// Local fn for Update node test
// Must append value representation to its result
static xml_node makePair(xml_node parent, char const *name)
{
  xml_node result = parent.append_child("Pair");
  assertTrue_1(result);
  makePcdataElement(result, "Name", name);
  return result;
}

static bool updateNodeXmlParserTest()
{
  assertTrue_1(doc);

  // Empty update
  {
    xml_node emptyUpdXml = makeNode(*doc, "emptyUpd", "Update");
    emptyUpdXml.append_child("NodeBody").append_child("Update");

    Node *emptyUpd = parseNode(emptyUpdXml, NULL);
    assertTrue_1(emptyUpd);
    assertTrue_1(emptyUpd->getNodeId() == "emptyUpd");
    assertTrue_1(emptyUpd->getType() == NodeType_Update);
    UpdateNode *unode = dynamic_cast<UpdateNode *>(emptyUpd);
    assertTrue_1(unode);

    finalizeNode(emptyUpd, emptyUpdXml);
    Update *upd = unode->getUpdate();
    assertTrue_1(upd);
    upd->activate();
    upd->fixValues();
    assertTrue_1(upd->getPairs().empty()); 

    delete emptyUpd;
  }

  // Update with literal values
  {
    xml_node literalUpdXml = makeNode(*doc, "literalUpd", "Update");
    xml_node updXml = literalUpdXml.append_child("NodeBody").append_child("Update");
    makePcdataElement(makePair(updXml, "foo"), "BooleanValue", "true");
    makePcdataElement(makePair(updXml, "bar"), "IntegerValue", "216");
    makePcdataElement(makePair(updXml, "baz"), "RealValue", "2.718");
    makePcdataElement(makePair(updXml, "bletch"), "StringValue", "bletch");

    Node *literalUpd = parseNode(literalUpdXml, NULL);
    assertTrue_1(literalUpd);
    assertTrue_1(literalUpd->getNodeId() == "literalUpd");
    assertTrue_1(literalUpd->getType() == NodeType_Update);
    UpdateNode *unode = dynamic_cast<UpdateNode *>(literalUpd);
    assertTrue_1(unode);

    finalizeNode(literalUpd, literalUpdXml);
    Update *upd = unode->getUpdate();
    assertTrue_1(upd);
    upd->activate();
    upd->fixValues();
    Update::PairValueMap const &pairs = upd->getPairs();
    assertTrue_1(!pairs.empty());
    assertTrue_1(pairs["foo"] == Value(true));
    assertTrue_1(pairs["bar"] == Value((int32_t) 216));
    assertTrue_1(pairs["baz"] == Value(2.718));
    assertTrue_1(pairs["bletch"] == Value("bletch"));

    delete literalUpd;
  }

  // Update with expression values
  {
    xml_node listNodeXml = makeNode(*doc, "listNode", "NodeList");
    xml_node decls = listNodeXml.append_child("VariableDeclarations");
    makePcdataElement(makeDeclareVariable(decls, "i", "Integer").append_child("InitialValue"),
                      "IntegerValue",
                      "42");
    xml_node ainit = 
      makeDeclareArray(decls, "a", "Integer", "2").append_child("InitialValue").append_child("ArrayValue");
    ainit.append_attribute("Type").set_value("Integer");
    makePcdataElement(ainit, "IntegerValue", "3");
    makePcdataElement(ainit, "IntegerValue", "6");
    xml_node listBodyXml = listNodeXml.append_child("NodeBody").append_child("NodeList");

    xml_node exprUpdXml = makeNode(listBodyXml, "exprUpd", "Update");
    xml_node updXml = exprUpdXml.append_child("NodeBody").append_child("Update");
    makePcdataElement(makePair(updXml, "bar"), "IntegerVariable", "i");
    xml_node aeXml = makePair(updXml, "baz").append_child("ArrayElement");
    makePcdataElement(aeXml, "Name", "a");
    makePcdataElement(aeXml.append_child("Index"), "IntegerValue", "1");
    makePcdataElement(makePair(updXml, "bletch"), "StringValue", "bletch");

    Node *listNode = parseNode(listNodeXml, NULL);
    assertTrue_1(listNode);
    assertTrue_1(listNode->getNodeId() == "listNode");
    assertTrue_1(listNode->getType() == NodeType_NodeList);
    std::vector<Node *> const &nodeList = listNode->getChildren();
    assertTrue_1(!nodeList.empty());
    assertTrue_1(nodeList.size() == 1);
    std::vector<Expression *> const *vars = listNode->getLocalVariables();
    assertTrue_1(vars && !vars->empty());
    assertTrue_1(vars->size() == 2);
    Expression *ivar = listNode->findLocalVariable("i");
    assertTrue_1(ivar);
    assertTrue_1(ivar->valueType() == INTEGER_TYPE);
    Expression *avar = listNode->findLocalVariable("a");
    assertTrue_1(avar);
    assertTrue_1(avar->valueType() == INTEGER_ARRAY_TYPE);

    Node *exprUpd = nodeList.front();
    assertTrue_1(exprUpd);
    assertTrue_1(exprUpd->getNodeId() == "exprUpd");
    assertTrue_1(exprUpd->getType() == NodeType_Update);
    UpdateNode *unode = dynamic_cast<UpdateNode *>(exprUpd);
    assertTrue_1(unode);

    finalizeNode(listNode, listNodeXml);
    Update *upd = unode->getUpdate();
    assertTrue_1(upd);

    ivar->activate();
    avar->activate();
    upd->activate();
    upd->fixValues();
    Update::PairValueMap const &pairs = upd->getPairs();
    assertTrue_1(!pairs.empty());
    assertTrue_1(pairs["bar"].valueType() == INTEGER_TYPE);
    assertTrue_1(pairs["bar"] == Value((int32_t) 42));
    assertTrue_1(pairs["baz"].valueType() == INTEGER_TYPE);
    assertTrue_1(pairs["baz"] == Value((int32_t) 6));
    assertTrue_1(pairs["bletch"] == Value("bletch"));

    delete listNode;
  }

  return true;
}

static bool libraryCallNodeXmlParserTest()
{
  assertTrue_1(doc);

  //
  // Construct some nodes to call
  //
  
  // Simple
  xml_document *dummyDoc = new xml_document;
  xml_node dummyXml = makeNode(dummyDoc->append_child("PlexilPlan"), "dummy", "Empty");
  addLibraryNode("dummy", dummyDoc);

  // With In variable, no default
  xml_document *withInDoc = new xml_document;
  xml_node withInVarXml = makeNode(withInDoc->append_child("PlexilPlan"), "withInVar", "Empty");
  makeDeclareVariable(withInVarXml.append_child("Interface").append_child("In"),
                      "inInt",
                      "Integer");
  addLibraryNode("withInVar", withInDoc);
  
  // In variable with default
  xml_document *defaultedInDoc = new xml_document;
  xml_node defaultedInVarXml = makeNode(defaultedInDoc->append_child("PlexilPlan"), "defaultedInVar", "Empty");
  xml_node defaultedInDecl = makeDeclareVariable(defaultedInVarXml.append_child("Interface").append_child("In"), 
                                                 "defInInt",
                                                 "Integer");
  makePcdataElement(defaultedInDecl.append_child("InitialValue"), "IntegerValue", "5");
  addLibraryNode("defaultedInVar", defaultedInDoc);
  
  // With InOut variable
  xml_document *inOutDoc = new xml_document;
  xml_node inOutVarXml = makeNode(inOutDoc->append_child("PlexilPlan"), "inOutVar", "Assignment");
  makeDeclareVariable(inOutVarXml.append_child("Interface").append_child("InOut"),
                      "inOutInt",
                      "Integer");
  xml_node inOutBody = inOutVarXml.append_child("NodeBody").append_child("Assignment");
  makePcdataElement(inOutBody, "IntegerVariable", "inOutInt");
  makePcdataElement(inOutBody.append_child("NumericRHS"), "IntegerValue", "-2");
  addLibraryNode("inOutVar", inOutDoc);

  // With defaulted InOut variable
  xml_document *defInOutDoc = new xml_document;
  xml_node defInOutVarXml = makeNode(defInOutDoc->append_child("PlexilPlan"), "defInOutVar", "Assignment");
  xml_node defInOutDecl = 
    makeDeclareVariable(defInOutVarXml.append_child("Interface").append_child("InOut"),
                        "defInOutInt",
                        "Integer");
  makePcdataElement(defInOutDecl.append_child("InitialValue"), "IntegerValue", "33");
  xml_node defInOutBody = defInOutVarXml.append_child("NodeBody").append_child("Assignment");
  makePcdataElement(defInOutBody, "IntegerVariable", "defInOutInt");
  makePcdataElement(defInOutBody.append_child("NumericRHS"), "IntegerValue", "-99");
  addLibraryNode("defInOutVar", defInOutDoc);

  //
  // Call tests
  //

  // Call with no aliases or interface variables
  {
    xml_node basicLibCallXml = makeNode(*doc, "basicLibCall", "LibraryNodeCall");
    xml_node libCall = basicLibCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "dummy");

    Node *basicLibCall = parseNode(basicLibCallXml, NULL);
    assertTrue_1(basicLibCall);
    assertTrue_1(basicLibCall->getNodeId() == "basicLibCall");
    assertTrue_1(basicLibCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(basicLibCall, basicLibCallXml);
    assertTrue_1(!basicLibCall->getChildren().empty());
    assertTrue_1(basicLibCall->getChildren().size() == 1);
    Node *dummy = basicLibCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "dummy");
    assertTrue_1(dummy->getType() == NodeType_Empty);
    assertTrue_1(dummy->getChildren().empty());

    delete basicLibCall;
  }

  // Call with defaulted In variable
  {
    xml_node defaultedInCallXml = makeNode(*doc, "defaultedInCall", "LibraryNodeCall");
    xml_node libCall = defaultedInCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "defaultedInVar");

    Node *defaultedInCall = parseNode(defaultedInCallXml, NULL);
    assertTrue_1(defaultedInCall);
    assertTrue_1(defaultedInCall->getNodeId() == "defaultedInCall");
    assertTrue_1(defaultedInCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(defaultedInCall, defaultedInCallXml);
    assertTrue_1(!defaultedInCall->getChildren().empty());
    assertTrue_1(defaultedInCall->getChildren().size() == 1);
    Node *dummy = defaultedInCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "defaultedInVar");
    assertTrue_1(dummy->getType() == NodeType_Empty);
    assertTrue_1(dummy->getChildren().empty());
    assertTrue_1(dummy->getLocalVariables()
                 && !dummy->getLocalVariables()->empty());
    Expression *divar = dummy->findVariable("defInInt");
    assertTrue_1(divar);
    assertTrue_1(divar->valueType() == INTEGER_TYPE);
    divar->activate();
    int32_t dival;
    assertTrue_1(divar->getValue(dival));
    assertTrue_1(dival == 5);

    delete defaultedInCall;
  }

  // Call with supplied In variable
  {
    xml_node inCallXml = makeNode(*doc, "inCall", "LibraryNodeCall");
    xml_node libCall = inCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "withInVar");
    xml_node alias0 = libCall.append_child("Alias");
    makePcdataElement(alias0, "NodeParameter", "inInt");
    makePcdataElement(alias0, "IntegerValue", "3");

    Node *inCall = parseNode(inCallXml, NULL);
    assertTrue_1(inCall);
    assertTrue_1(inCall->getNodeId() == "inCall");
    assertTrue_1(inCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(inCall, inCallXml);
    assertTrue_1(!inCall->getChildren().empty());
    assertTrue_1(inCall->getChildren().size() == 1);
    Node *dummy = inCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "withInVar");
    assertTrue_1(dummy->getType() == NodeType_Empty);
    assertTrue_1(dummy->getChildren().empty());
    assertTrue_1(!dummy->getLocalVariables()
                 || dummy->getLocalVariables()->empty());
    Expression *ivar = dummy->findVariable("inInt");
    assertTrue_1(ivar);
    assertTrue_1(ivar->valueType() == INTEGER_TYPE);
    ivar->activate();
    int32_t ival;
    assertTrue_1(ivar->getValue(ival));
    assertTrue_1(ival == 3);

    delete inCall;
  }

  // Call with alias supplied to In variable with default
  {
    xml_node nondefaultedInCallXml = makeNode(*doc, "nondefaultedInCall", "LibraryNodeCall");
    xml_node libCall = nondefaultedInCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "defaultedInVar");
    xml_node alias0 = libCall.append_child("Alias");
    makePcdataElement(alias0, "NodeParameter", "defInInt");
    makePcdataElement(alias0, "IntegerValue", "19");

    Node *nondefaultedInCall = parseNode(nondefaultedInCallXml, NULL);
    assertTrue_1(nondefaultedInCall);
    assertTrue_1(nondefaultedInCall->getNodeId() == "nondefaultedInCall");
    assertTrue_1(nondefaultedInCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(nondefaultedInCall, nondefaultedInCallXml);
    assertTrue_1(!nondefaultedInCall->getChildren().empty());
    assertTrue_1(nondefaultedInCall->getChildren().size() == 1);
    Node *dummy = nondefaultedInCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "defaultedInVar");
    assertTrue_1(dummy->getType() == NodeType_Empty);
    assertTrue_1(dummy->getChildren().empty());
    assertTrue_1(!dummy->getLocalVariables()
                 || dummy->getLocalVariables()->empty());
    Expression *divar = dummy->findVariable("defInInt");
    assertTrue_1(divar);
    assertTrue_1(divar->valueType() == INTEGER_TYPE);
    divar->activate();
    int32_t dival;
    assertTrue_1(divar->getValue(dival));
    assertTrue_1(dival == 19);

    delete nondefaultedInCall;
  }

  // Call with supplied InOut variable
  {
    xml_node inOutCallXml = makeNode(*doc, "inOutCall", "LibraryNodeCall");
    xml_node libCall = inOutCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "inOutVar");
    xml_node aliasedVarDecl = 
      makeDeclareVariable(inOutCallXml.append_child("VariableDeclarations"), "aliasedInOut", "Integer");
    makePcdataElement(aliasedVarDecl.append_child("InitialValue"), "IntegerValue", "42");
    xml_node alias0 = libCall.append_child("Alias");
    makePcdataElement(alias0, "NodeParameter", "inOutInt");
    makePcdataElement(alias0, "IntegerVariable", "aliasedInOut");

    Node *inOutCall = parseNode(inOutCallXml, NULL);
    assertTrue_1(inOutCall);
    assertTrue_1(inOutCall->getNodeId() == "inOutCall");
    assertTrue_1(inOutCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(inOutCall, inOutCallXml);
    assertTrue_1(!inOutCall->getChildren().empty());
    assertTrue_1(inOutCall->getChildren().size() == 1);
    Expression *avar = inOutCall->findLocalVariable("aliasedInOut");
    assertTrue_1(avar);
    assertTrue_1(avar->valueType() == INTEGER_TYPE);

    Node *dummy = inOutCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "inOutVar");
    assertTrue_1(dummy->getType() == NodeType_Assignment);
    assertTrue_1(dummy->getChildren().empty());
    assertTrue_1(!dummy->getLocalVariables()
                 || dummy->getLocalVariables()->empty());
    Expression *ivar = dummy->findVariable("inOutInt");
    assertTrue_1(ivar);
    assertTrue_1(ivar->valueType() == INTEGER_TYPE);
    avar->activate();
    int32_t ival = 0;
    assertTrue_1(avar->getValue(ival));
    assertTrue_1(ival == 42);

    ival = 0;
    ivar->activate();
    assertTrue_1(ivar->getValue(ival));
    assertTrue_1(ival == 42);

    delete inOutCall;
  }

  // Call with defaulted InOut variable
  {
    xml_node defInOutCallXml = makeNode(*doc, "defInOutCall", "LibraryNodeCall");
    xml_node libCall = defInOutCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "defInOutVar");

    Node *defInOutCall = parseNode(defInOutCallXml, NULL);
    assertTrue_1(defInOutCall);
    assertTrue_1(defInOutCall->getNodeId() == "defInOutCall");
    assertTrue_1(defInOutCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(defInOutCall, defInOutCallXml);
    assertTrue_1(!defInOutCall->getChildren().empty());
    assertTrue_1(defInOutCall->getChildren().size() == 1);
    assertTrue_1(!defInOutCall->getLocalVariables());

    Node *dummy = defInOutCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "defInOutVar");
    assertTrue_1(dummy->getType() == NodeType_Assignment);
    assertTrue_1(dummy->getChildren().empty());
    assertTrue_1(dummy->getLocalVariables()
                 && !dummy->getLocalVariables()->empty());
    Expression *ivar = dummy->findVariable("defInOutInt");
    assertTrue_1(ivar);
    assertTrue_1(ivar->valueType() == INTEGER_TYPE);
    int32_t ival = 0;
    ivar->activate();
    assertTrue_1(ivar->getValue(ival));
    assertTrue_1(ival == 33);

    delete defInOutCall;
  }

  // Call with alias for defaulted InOut variable
  {
    xml_node nonDefInOutCallXml = makeNode(*doc, "nonDefInOutCall", "LibraryNodeCall");
    xml_node aliasedVarDecl = 
      makeDeclareVariable(nonDefInOutCallXml.append_child("VariableDeclarations"), "aliasedInOut", "Integer");
    makePcdataElement(aliasedVarDecl.append_child("InitialValue"), "IntegerValue", "42");
    xml_node libCall = nonDefInOutCallXml.append_child("NodeBody").append_child("LibraryNodeCall");
    makePcdataElement(libCall, "NodeId", "defInOutVar");
    xml_node alias0 = libCall.append_child("Alias");
    makePcdataElement(alias0, "NodeParameter", "defInOutInt");
    makePcdataElement(alias0, "IntegerVariable", "aliasedInOut");

    Node *nonDefInOutCall = parseNode(nonDefInOutCallXml, NULL);
    assertTrue_1(nonDefInOutCall);
    assertTrue_1(nonDefInOutCall->getNodeId() == "nonDefInOutCall");
    assertTrue_1(nonDefInOutCall->getType() == NodeType_LibraryNodeCall);

    finalizeNode(nonDefInOutCall, nonDefInOutCallXml);
    assertTrue_1(!nonDefInOutCall->getChildren().empty());
    assertTrue_1(nonDefInOutCall->getChildren().size() == 1);
    assertTrue_1(nonDefInOutCall->getLocalVariables()
                 && !nonDefInOutCall->getLocalVariables()->empty());
    Expression *avar = nonDefInOutCall->findLocalVariable("aliasedInOut");
    assertTrue_1(avar);
    assertTrue_1(avar->valueType() == INTEGER_TYPE);

    Node *dummy = nonDefInOutCall->getChildren().front();
    assertTrue_1(dummy->getNodeId() == "defInOutVar");
    assertTrue_1(dummy->getType() == NodeType_Assignment);
    assertTrue_1(dummy->getChildren().empty());
    assertTrue_1(!dummy->getLocalVariables()
                 || dummy->getLocalVariables()->empty());
    Expression *ivar = dummy->findVariable("defInOutInt");
    assertTrue_1(ivar);
    assertTrue_1(ivar->valueType() == INTEGER_TYPE);

    int32_t ival = 0;
    avar->activate();
    assertTrue_1(avar->getValue(ival));
    assertTrue_1(ival == 42);

    ival = 0;
    ivar->activate();
    assertTrue_1(ivar->getValue(ival));
    assertTrue_1(ival == 42);

    delete nonDefInOutCall;
  }

  return true;
}

bool nodeXmlParserTest()
{
  doc = new xml_document();
  g_interface = new TransitionExternalInterface;

  runTest(emptyNodeXmlParserTest);
  runTest(listNodeXmlParserTest);
  runTest(assignmentNodeXmlParserTest);
  runTest(commandNodeXmlParserTest);
  runTest(updateNodeXmlParserTest);
  runTest(libraryCallNodeXmlParserTest);

  delete g_interface;
  g_interface = NULL;
  delete doc;
  doc = NULL;
  return true;
}

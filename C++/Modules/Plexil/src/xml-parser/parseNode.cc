/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

#include "parseNode.hh"

#include "Alias.hh"
#include "ArrayLiteralFactory.hh"
#include "AssignmentNode.hh"
#include "CommandNode.hh"
#include "commandXmlParser.hh"
#include "Debug.hh"
#include "Error.hh"
#include "LibraryCallNode.hh"
#include "ListNode.hh"
#include "NodeFactory.hh"
#include "parseAssignment.hh"
#include "parseLibraryCall.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"
#include "UpdateNode.hh"
#include "updateXmlParser.hh"

#include "pugixml.hpp"

#include <cstdlib> // for strtoul()
#include <cstring>

using pugi::xml_attribute;
using pugi::xml_node;
using pugi::node_element;

namespace PLEXIL
{
  //
  // First pass
  //

  // Early first pass
  // Preallocate symbol table space for variables
  // Don't do any parsing, just count
  static size_t estimateVariableSpace(xml_node const decls)
  {
    return std::distance(decls.begin(), decls.end());
  }

  static char const *getVarDeclName(xml_node const decl)
    throw (ParserException)
  {
    xml_node nameXml = decl.first_child();
    checkTag(NAME_TAG, nameXml);
    char const *name = nameXml.child_value();
    checkParserExceptionWithLocation(*name,
                                     nameXml,
                                     "Empty " << NAME_TAG << " element in " << decl.name());
    return name;
  }

  static void parseVariableDeclarations(Node *node, xml_node const decls)
    throw (ParserException)
  {
    for (xml_node decl = decls.first_child(); decl; decl = decl.next_sibling()) {
      // Check for duplicate names before allocating
      char const *name = getVarDeclName(decl);
      for (xml_node sib = decl.previous_sibling(); sib; sib = sib.previous_sibling()) {
        checkParserExceptionWithLocation(strcmp(name, getVarDeclName(sib)),
                                         decl,
                                         "Multiple variables named \""
                                         << name
                                         << "\" in node "
                                         << node->getNodeId());
      }

      // Variables are always created here, no need for "garbage" flag.
      node->addLocalVariable(name, createExpression(decl, node));
    }
  }

  // Early first pass
  // Estimate symbol table space for interface variables
  // Don't do any parsing, just count - overestimating is better than under
  static size_t estimateInterfaceSpace(xml_node const iface)
  {
    size_t n = 0;
    for (xml_node elt = iface.first_child(); elt; elt = elt.next_sibling())
      n += std::distance(elt.begin(), elt.end());
    return n;
  }

  // For Interface specs; may have other uses.
  static ValueType getVarDeclType(xml_node const decl)
    throw (ParserException)
  {
    xml_node typeElt = decl.child(TYPE_TAG);
    checkParserExceptionWithLocation(typeElt,
                                     decl,
                                     "Variable declaration lacks " << TYPE_TAG << " element");
    checkNotEmpty(typeElt);
    ValueType typ = parseValueType(typeElt.child_value());
    checkParserExceptionWithLocation(typ != UNKNOWN_TYPE,
                                     typeElt,
                                     "Unknown variable type name " << typeElt.child_value());
    checkParserExceptionWithLocation(isScalarType(typ),
                                     typeElt,
                                     "Invalid type name " << typeElt.child_value() << " for " << decl.name());
    if (testTag(DECL_ARRAY_TAG, decl))
      typ = arrayType(typ);
    else 
      checkParserExceptionWithLocation(testTag(DECL_VAR_TAG, decl),
                                       decl,
                                       "Only " << DECL_VAR_TAG << " and " << DECL_ARRAY_TAG << " are valid in this context");
    return typ;
  }

  //
  // N.B. There is a limited amount of checking we can do on interface variables in the first pass.
  // LibraryNodeCall aliases can't be expanded because some of the variables they can reference
  // (e.g. child node internal vars) may not exist yet. Same with default values.

  // First pass checking of one In interface variable
  static void checkInDecl(Node *node, xml_node const inXml, bool isCall)
    throw (ParserException)
  {
    char const *name = getVarDeclName(inXml);
    checkParserExceptionWithLocation(!node->findLocalVariable(name),
                                     inXml,
                                     "In interface variable " << name
                                     << " shadows another variable of same name in this node");
    getVarDeclType(inXml); // for effect
  }

  // First pass checking of one InOut interface
  static void checkInOutDecl(Node *node, xml_node const inOutXml, bool isCall)
    throw (ParserException)
  {
    char const *name = getVarDeclName(inOutXml);
    checkParserExceptionWithLocation(!node->findLocalVariable(name),
                                     inOutXml,
                                     "InOut interface variable " << name
                                     << " shadows another variable of same name in this node");
    getVarDeclType(inOutXml); // for effect
  }

  // First pass
  static void parseInterface(Node *node, xml_node const iface)
    throw (ParserException)
  {
    // Figure out if this is a library node expansion
    Node *parent = node->getParent();
    bool isCall = (parent && parent->getType() == NodeType_LibraryNodeCall);
    for (xml_node elt = iface.first_child(); elt; elt = elt.next_sibling()) {
      char const *name = elt.name();
      if (0 == strcmp(IN_TAG, name)) {
        for (xml_node decl = elt.first_child(); decl; decl = decl.next_sibling())
          checkInDecl(node, decl, isCall);
      }
      else if (0 == strcmp(INOUT_TAG, name)) {
        for (xml_node decl = elt.first_child(); decl; decl = decl.next_sibling())
          checkInOutDecl(node, decl, isCall);
      }
      else
        reportParserExceptionWithLocation(elt,
                                          "Node " << node->getNodeId()
                                          << ": Illegal " << name << " element inside " << INTERFACE_TAG);
    }
  }

  // Check child nodes for duplicated names. Returns # of child nodes.

  static size_t checkNodeIds(std::string const &parentId, xml_node const kidsXml)
    throw (ParserException)
  {
    size_t nKids = 0;
    for (xml_node kidXml = kidsXml.first_child();
         kidXml;
         kidXml = kidXml.next_sibling()) {
      ++nKids;

      // Check parent and child don't have same name
      char const *kidId = kidXml.child(NODEID_TAG).child_value();
      checkParserExceptionWithLocation(parentId != kidId,
                                       kidXml,
                                       "List Node " << parentId
                                       << " has a child node with the same NodeId");

      // Check that none of siblings has same name
      for (xml_node nextKid = kidXml.previous_sibling();
           nextKid;
           nextKid = nextKid.previous_sibling()) {
        checkParserExceptionWithLocation(strcmp(kidId,
                                                nextKid.child(NODEID_TAG).child_value()),
                                         kidXml,
                                         "List Node " << parentId
                                         << " has multiple child nodes with the same NodeId "
                                         << kidId);
      }
    }
    return nKids;
  }

  static void constructChildNodes(ListNode *node, xml_node const kidsXml)
    throw (ParserException)
  {
    assertTrue_1(node);

    node->reserveChildren(checkNodeIds(node->getNodeId(), kidsXml));

    // Construct the children.
    for (xml_node kidXml = kidsXml.first_child();
         kidXml;
         kidXml = kidXml.next_sibling()) {
      // Parse and add it
      node->addChild(parseNode(kidXml, node));
    }
  }

  PlexilNodeType checkNodeTypeAttr(xml_node const xml)
    throw (ParserException)
  {
    xml_attribute const typeAttr = xml.attribute(NODETYPE_ATTR);
    checkParserExceptionWithLocation(typeAttr,
                                     xml,
                                     "Node has no " << NODETYPE_ATTR << " attribute");

    PlexilNodeType nodeType = parseNodeType(typeAttr.value());
    checkParserExceptionWithLocation(nodeType < NodeType_error,
                                     xml, // should be attribute
                                     "Invalid node type \"" << typeAttr.value() << "\"");
    return nodeType;
  }

  static PlexilNodeType checkNode(xml_node const xml)
    throw (ParserException)
  {
    checkTag(NODE_TAG, xml);

    PlexilNodeType nodeType = checkNodeTypeAttr(xml);
    bool hasId = false;
    bool hasPrio = false;
    bool hasIface = false;
    bool hasVarDecls = false;
    bool hasBody = false;

    // Scan all children in order
    for (xml_node temp = xml.first_child(); temp; temp = temp.next_sibling()) {
      char const *tag = temp.name();
      checkParserExceptionWithLocation(*tag,
                                       xml,
                                       "Non-element found at top level of node");
      debugMsg("parseNode", " parsing element " << tag);
      switch (*tag) {

      case 'A': // Assume - annotation for analysis, ignored
        checkParserExceptionWithLocation(!strcmp(ASSUME_TAG, tag),
                                         temp, 
                                         "Illegal element \"" << tag << "\" in Node");
        break;
        
      case 'C': // Comment
        checkParserExceptionWithLocation(!strcmp(COMMENT_TAG, tag),
                                         temp, 
                                         "Illegal element \"" << tag << "\" in Node");
        break;

      case 'D': // Desire - annotation for analysis, ignored
        checkParserExceptionWithLocation(!strcmp(DESIRE_TAG, tag),
                                         temp, 
                                         "Illegal element \"" << tag << "\" in Node");
        break;
        
      case 'E': // EndCondition, ExitCondition, Expect
        if (!strcmp(END_CONDITION_TAG, tag)
            || !strcmp(EXIT_CONDITION_TAG, tag)
            || !strcmp(EXPECT_TAG, tag)) // Annotation for analysis, ignored
          break;
        reportParserExceptionWithLocation(temp,
                                          "Illegal element \"" << tag << "\" in Node");
        break;

      case 'I': // Interface, InvariantCondition
        if (!strcmp(INVARIANT_CONDITION_TAG, tag))
          break;
        if (!strcmp(INTERFACE_TAG, tag)) {
          checkParserExceptionWithLocation(!hasIface,
                                           temp, 
                                           "Duplicate " << tag << " element in Node");
          hasIface = true;
          break;
        }
        reportParserExceptionWithLocation(temp, 
                                          "Illegal element \"" << tag << "\" in Node");
        break;


      case 'N': // NodeId, NodeBody
        if (!strcmp(NODEID_TAG, tag)) {
          checkParserExceptionWithLocation(!hasId,
                                           temp, 
                                           "Duplicate " << tag << " element in Node");
          hasId = true;
        }
        else if (!strcmp(BODY_TAG, tag)) {
          checkParserExceptionWithLocation(!hasBody,
                                           temp, 
                                           "Duplicate " << tag << " element in Node");
          hasBody = true;
        }
        else {
          reportParserExceptionWithLocation(temp,
                                            "Illegal element \"" << tag << "\" in Node");
        }
        break;

      case 'P': // PostCondition, Priority, PreCondition
        if (!strcmp(POST_CONDITION_TAG, tag)
            || !strcmp(PRE_CONDITION_TAG, tag))
          break;
        if (!strcmp(PRIORITY_TAG, tag)) {
          checkParserExceptionWithLocation(nodeType == NodeType_Assignment,
                                           temp,
                                           "Only Assignment nodes may have a Priority element");
          checkParserExceptionWithLocation(!hasPrio,
                                           temp, 
                                           "Duplicate " << tag << " element in Node");
          hasPrio = true;
          break;
        }
        reportParserExceptionWithLocation(temp,
                                          "Illegal element \"" << tag << "\" in Node");
        break;

      case 'R': // RepeatCondition
        checkParserExceptionWithLocation(!strcmp(REPEAT_CONDITION_TAG, tag),
                                         temp, 
                                         "Illegal element \"" << tag << "\" in Node");
        break;

      case 'S': // SkipCondition, StartCondition
        if (!strcmp(START_CONDITION_TAG, tag)
            || !strcmp(SKIP_CONDITION_TAG, tag))
          break;
        reportParserExceptionWithLocation(temp,
                                          "Illegal element \"" << tag << "\" in Node");
        break;

      case 'V': // VariableDeclarations
        if (!strcmp(VAR_DECLS_TAG, tag)) {
          checkParserExceptionWithLocation(!hasVarDecls,
                                           temp, 
                                           "Duplicate " << tag << " element in Node");
          hasVarDecls = true;
          break;
        }
        // else fall thru to parser error

      default:
        reportParserExceptionWithLocation(temp,
                                          "Illegal element \"" << tag << "\" in Node");
        break;
      }
    }

    checkParserExceptionWithLocation(hasId,
                                     xml,
                                     "Node missing " << NODEID_TAG << " element");

    // Empty NodeId check
    checkParserExceptionWithLocation(*(xml.child(NODEID_TAG).child_value()),
                                     xml.child(NODEID_TAG), 
                                     "Empty " << NODEID_TAG << " element in Node");

    // Should it have a node body? Does it?
    if (hasBody) {
      checkParserExceptionWithLocation(nodeType != NodeType_Empty,
                                       xml.child(BODY_TAG),
                                       "Empty Node \""
                                       << xml.child(NODEID_TAG).child_value()
                                       << "\" may not have a NodeBody element");
    }
    else
      checkParserExceptionWithLocation(nodeType == NodeType_Empty,
                                       xml,
                                       "Node \""
                                       << xml.child(NODEID_TAG).child_value()
                                       << "\" has no " << BODY_TAG << " element");
    return nodeType;
  }

  static void initializeNodeVariables(Node *node, xml_node const xml)
  {
    xml_node const varDecls = xml.child(VAR_DECLS_TAG);
    xml_node const iface = xml.child(INTERFACE_TAG);

    // By now we have an upper bound on how many entries are required.
    // Reserve space for them. 
    // This saves us from reallocating and copying the whole table as it grows.
    if (varDecls || iface) {
      size_t nVariables = 0;
      if (varDecls)
        nVariables = estimateVariableSpace(varDecls);
      if (iface)
        nVariables += estimateInterfaceSpace(xml.child(INTERFACE_TAG));
      node->allocateVariables(nVariables);
    }

    // Check interface variables
    if (iface) {
      debugMsg("parseNode", " parsing interface declarations");
      parseInterface(node, iface);
    }

    // Populate local variables
    if (varDecls) {
      debugMsg("parseNode", " parsing variable declarations");
      parseVariableDeclarations(node, varDecls);
    }
  }

  Node *parseNode(xml_node const xml, Node *parent)
    throw (ParserException)
  {
    // Perform sanity checks
    PlexilNodeType nodeType = checkNode(xml);

    debugMsg("parseNode", " constructing node");
    Node *node =
      NodeFactory::createNode(xml.child(NODEID_TAG).child_value(),
                              nodeType,
                              parent);
    debugMsg("parseNode", " Node " << node->getNodeId()  << " created");

    try {
      // Populate interface and local variables.
      initializeNodeVariables(node, xml);

      // Construct body
      debugMsg("parseNode", " constructing body");
      switch (nodeType) {
      case NodeType_Assignment:
        constructAssignment(dynamic_cast<AssignmentNode *>(node), xml);
        break;

      case NodeType_Command:
        constructAndSetCommand(dynamic_cast<CommandNode *>(node),
                               xml.child(BODY_TAG).first_child());
        break;

      case NodeType_LibraryNodeCall:
        constructLibraryCall(dynamic_cast<LibraryCallNode *>(node),
                             xml.child(BODY_TAG).first_child());
        break;

      case NodeType_NodeList:
        constructChildNodes(dynamic_cast<ListNode *>(node),
                            xml.child(BODY_TAG).first_child());
        break;

      case NodeType_Update:
        constructAndSetUpdate(dynamic_cast<UpdateNode *>(node),
                              xml.child(BODY_TAG).first_child());
        break;

      case NodeType_Empty:
        // no-op
        break;

      default:
        assertTrue_2(ALWAYS_FAIL, "Internal error: bad node type");
        break;
      }
    }
    catch (std::exception const & exc) {
      debugMsg("parseNode", " recovering from parse error, deleting node "
               << node->getNodeId());
      delete node;
      throw;
    }
    debugMsg("parseNode", " first pass done.");
    return node;
  }

  //
  // Second pass
  //
  // The node is partially built and most XML checking has been done.
  // All nodes and their declared and internal variables have been constructed,
  // but aliases and interface variables may not have been.
  // Expressions (including LHS variable references) have NOT been constructed.
  // Finish populating the node and its children.
  // 

  static void parseVariableInitializer(Node *node, xml_node const decl)
    throw (ParserException)
  {
    xml_node initXml = decl.child(INITIALVAL_TAG);
    if (initXml) {
      char const *varName = decl.child_value(NAME_TAG);
      Expression *var = node->findLocalVariable(varName);
      assertTrueMsg(var,
                    "finalizeNode: Internal error: variable " << varName
                    << " not found in node " << node->getNodeId());
      // FIXME: is this needed/possible?
      checkParserExceptionWithLocation(var->isAssignable(),
                                       initXml,
                                       "This variable may not take an initializer");
      checkHasChildElement(initXml);
      Expression *init;
      ValueType varType = var->valueType();
      bool garbage;
      if (isArrayType(varType)
          && testTag(typeNameAsValue(arrayElementType(varType)).c_str(), initXml.first_child())) {
        // Handle old style initializer
        garbage = true; // always constructed
        switch (varType) {
        case BOOLEAN_ARRAY_TYPE:
          init = createArrayLiteral<bool>("Boolean", initXml);
          break;

        case INTEGER_ARRAY_TYPE:
          init = createArrayLiteral<int32_t>("Integer", initXml);
          break;

        case REAL_ARRAY_TYPE:
          init = createArrayLiteral<double>("Real", initXml);
          break;

        case STRING_ARRAY_TYPE:
          init = createArrayLiteral<std::string>("String", initXml);
          break;

        default:
          reportParserExceptionWithLocation(initXml,
                                            "Can't parse initial value for unimplemented or illegal type "
                                            << valueTypeName(varType));
          return;
        }
      }
      else {
        // Simply parse whatever's inside the <InitialValue>
        initXml = initXml.first_child();
        init = createExpression(initXml, node, garbage);
      }
      ValueType initType = init->valueType();
      if (!areTypesCompatible(varType, initType)) {
        if (garbage)
          delete init;
        reportParserExceptionWithLocation(initXml,
                                          "Node " << node->getNodeId()
                                          << ": Initialization type mismatch for variable "
                                          << varName << ", variable is " << valueTypeName(varType)
                                          << ", initializer is " << valueTypeName(initType));
        return; // make cppcheck happy
      }
      if (isArrayType(varType)) {
        // Check whether initial value is larger than declared size
        int sizeSpec = decl.child(MAX_SIZE_TAG).text().as_int(-1);
        if (sizeSpec >= 0) {
          Array const *initArray = NULL;
          assertTrueMsg(init->getValuePointer(initArray),
                        "Internal error: array initial value is unknown");
          if (initArray->size() > (size_t) sizeSpec) {
            if (garbage)
              delete init;
            reportParserExceptionWithLocation(decl,
                                              "Node " << node->getNodeId()
                                              << ": initial value for array variable "
                                              << varName << " exceeds declared array size");
            return; // make cppcheck happy
          }
        }
      }
      var->asAssignable()->setInitializer(init, garbage);
    }
  }

  // Process variable declarations, if any
  static void constructVariableInitializers(Node *node, xml_node const xml)
    throw (ParserException)
  {
    xml_node temp = xml.child(VAR_DECLS_TAG);
    if (!temp)
      return;

    debugMsg("finalizeNode",
             " constructing variable initializers for " << node->getNodeId());
    for (xml_node decl = temp.first_child();
         decl;
         decl = decl.next_sibling()) 
      parseVariableInitializer(node, decl);
  }

  static void linkInVar(Node *node, xml_node const inXml, bool isCall)
    throw (ParserException)
  {
    char const *name = getVarDeclName(inXml);

    debugMsg("linkInVar",
             " node " << node->getNodeId() << ", In variable " << name);
    // Find the variable, if it exists
    // If a library call, should be in caller's alias list.
    // If not, should have been declared by an ancestor.
    Expression *exp = node->findVariable(name);
    ValueType typ = getVarDeclType(inXml);
    if (exp) {
      debugMsg("linkInVar", " found ancestor variable");
      checkParserExceptionWithLocation(areTypesCompatible(typ, exp->valueType()),
                                       inXml,
                                       "In interface variable " << name
                                       << ": Type " << valueTypeName(typ)
                                       << " expected, but expression of type "
                                       << valueTypeName(exp->valueType()) << " was provided");
      if (exp->isAssignable()) {
        // Construct read-only alias.
        debugMsg("linkInVar",
                 " constructing read-only alias for ancestor variable " << name);

        // Ancestor owns the aliased expression, so we can't delete it.
        Expression *alias = new Alias(node, name, exp, false);
        if (!node->addLocalVariable(name, alias)) {
          delete alias;
          reportParserExceptionWithLocation(inXml,
                                            "In interface variable " << name
                                            << " shadows existing local variable of same name");
        }
        
        // else nothing to do - "variable" already accessible and read-only
      }
    }
    else {
      debugMsg("linkInVar", " no ancestor variable found");

      // No such variable/alias - use default initial value
      xml_node initXml = inXml.child(INITIALVAL_TAG);
      checkParserExceptionWithLocation(initXml,
                                       inXml,
                                       "In variable " << name << " not found and no default InitialValue provided");

      debugMsg("linkInVar", " constructing default value");
      bool garbage;
      exp = createExpression(initXml.first_child(), node, garbage);
      if (!areTypesCompatible(typ, exp->valueType())) {
        ValueType expType = exp->valueType();
        if (garbage)
          delete exp;
        reportParserExceptionWithLocation(initXml,
                                          "In interface variable " << name
                                          << " has type " << valueTypeName(typ)
                                          << " but default InitialValue is of incompatible type "
                                          << valueTypeName(expType));
        return; // make cppcheck happy
      }
      // If exp is writable or is not something we can delete, 
      // wrap it in an Alias
      if (exp->isAssignable() || !garbage) {
        debugMsg("linkInVar", " constructing read-only alias for default value");
        exp = new Alias(node, name, exp, garbage);
      }
      if (!node->addLocalVariable(name, exp)) {
        delete exp;
        reportParserExceptionWithLocation(inXml,
                                          "In interface variable " << name
                                          << " shadows local variable of same name");
        return; // make cppcheck happy
      }
    }
  }

  static void linkInOutVar(Node *node, xml_node const inOutXml, bool isCall)
    throw (ParserException)
  {
    char const *name = getVarDeclName(inOutXml);
    ValueType typ = getVarDeclType(inOutXml);
    // Find the variable, if it exists
    // If a library call, should be in caller's alias list.
    // If not, should have been declared by an ancestor.
    Expression *exp = node->findVariable(name);
    if (exp) {
      checkParserExceptionWithLocation(areTypesCompatible(typ, exp->valueType()),
                                       inOutXml,
                                       "InOut interface variable " << name
                                       << ": Type " << valueTypeName(typ)
                                       << " expected, but expression of type "
                                       << valueTypeName(exp->valueType()) << " was provided");
      checkParserExceptionWithLocation(exp->isAssignable(),
                                       inOutXml,
                                       "InOut interface variable " << name
                                       << " is read-only");
    }
    else {
      // No such variable/alias - use default initial value
      xml_node initXml = inOutXml.child(INITIALVAL_TAG);
      checkParserExceptionWithLocation(initXml,
                                       inOutXml,
                                       "InOut variable " << name << " not found and no default InitialValue provided");
      bool garbage, initGarbage;
      Expression *initExp =
        createExpression(initXml.first_child(), node, initGarbage);
      ValueType initExpType = initExp->valueType(); 
      if (!areTypesCompatible(typ, initExpType)) {
        if (initGarbage)
          delete initExp;
        reportParserExceptionWithLocation(initXml,
                                          "InOut variable " << name
                                          << " has type " << valueTypeName(typ)
                                          << " but default InitialValue is of incompatible type "
                                          << valueTypeName(initExpType));
      }
      Expression *var = createAssignable(inOutXml, node, garbage);
      assertTrue_1(garbage); // better be something we can delete!
      if (!node->addLocalVariable(name, var)) {
        delete var;
        reportParserExceptionWithLocation(inOutXml,
                                          "InOut interface variable " << name
                                          << " shadows local variable of same name");
      }
      var->asAssignable()->setInitializer(initExp, initGarbage);
    }
  }

  static void linkAndInitializeInterfaceVars(Node *node, xml_node const nodeXml)
    throw (ParserException)
  {
    xml_node const iface = nodeXml.child(INTERFACE_TAG);
    if (!iface)
      return;
    
    debugMsg("linkAndInitializeInterface", " node " << node->getNodeId());
    Node *parent = node->getParent();
    bool isCall = (parent && parent->getType() == NodeType_LibraryNodeCall);
    for (xml_node temp = iface.first_child(); temp; temp = temp.next_sibling()) {
      checkHasChildElement(temp);
      if (0 == strcmp(IN_TAG, temp.name())) {
        for (xml_node decl = temp.first_child(); decl; decl = decl.next_sibling())
          linkInVar(node, decl, isCall);
      }
      else if (0 == strcmp(INOUT_TAG, temp.name())) {
        for (xml_node decl = temp.first_child(); decl; decl = decl.next_sibling())
          linkInOutVar(node, decl, isCall);
      }
      else
        assertTrueMsg(ALWAYS_FAIL,
                      "Internal error: Found " << temp.name()
                      << " element in Interface during second pass");
    }
  }

  static void createConditions(Node *node, xml_node const xml)
    throw (ParserException)
  {
    for (xml_node elt = xml.first_child(); elt; elt = elt.next_sibling()) {
      char const *tag = elt.name();
      if (testSuffix(CONDITION_SUFFIX, tag)) {
        debugMsg("finalizeNode", " processing condition " << tag);
        bool garbage;
        Expression *cond = createExpression(elt.first_child(), node, garbage);
        ValueType condType = cond->valueType();
        if (condType != BOOLEAN_TYPE && condType != UNKNOWN_TYPE) {
          if (garbage)
            delete cond;
          reportParserExceptionWithLocation(elt.first_child(),
                                            "Node " << node->getNodeId() << ": "
                                            << tag << " expression is not Boolean");
        }
        node->addUserCondition(tag, cond, garbage);
      }
    }

    node->finalizeConditions();
  }

  static void finalizeListNode(ListNode *node, xml_node const listXml)
    throw (ParserException)
  {
    assertTrue_1(node);
    std::vector<Node *> &kids = node->getChildren();
    std::vector<Node *>::iterator kid = kids.begin();
    xml_node kidXml = listXml.first_child();
    while (kid != kids.end() && kidXml) {
      finalizeNode(*kid, kidXml);
      ++kid;
      kidXml = kidXml.next_sibling();
    }
  }

  void finalizeNode(Node *node, xml_node const xml)
    throw (ParserException)
  {
    debugMsg("finalizeNode", " node " << node->getNodeId());

    linkAndInitializeInterfaceVars(node, xml);
    constructVariableInitializers(node, xml);
    createConditions(node, xml);

    // Process body
    switch (node->getType()) {
    case NodeType_Assignment:
      finalizeAssignment(dynamic_cast<AssignmentNode *>(node),
                         xml.child(BODY_TAG).first_child());
      break;
      
    case NodeType_Command:
      finalizeCommandNode(dynamic_cast<CommandNode *>(node),
                          xml.child(BODY_TAG).first_child());
      break;

    case NodeType_LibraryNodeCall:
      finalizeLibraryCall(dynamic_cast<LibraryCallNode *>(node),
                          xml.child(BODY_TAG).first_child());
      break;

    case NodeType_NodeList:
      finalizeListNode(dynamic_cast<ListNode *>(node),
                       xml.child(BODY_TAG).first_child());
      break;

    case NodeType_Update:
      finalizeUpdateNode(dynamic_cast<UpdateNode *>(node),
                         xml.child(BODY_TAG).first_child());
      break;

      // No-op for empty.
      // Invalid type should have been caught in first pass.
    default:
      break;
    }
  }

} // namespace PLEXIL

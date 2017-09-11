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

#include "InternalExpressionFactories.hh"

#include "Command.hh"
#include "CommandNode.hh"
#include "Error.hh"
#include "Node.hh"
#include "NodeConstantExpressions.hh"
#include "ParserException.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"

#include "pugixml.hpp"

#include <cstring>

namespace PLEXIL
{
  // Utility routines
  static Node *parseNodeRef(pugi::xml_node nodeRef, NodeConnector *node)
  {
    // parse directional reference
    checkAttr(DIR_ATTR, nodeRef);
    const char* dirValue = nodeRef.attribute(DIR_ATTR).value();

    if (!strcmp(dirValue, SELF_VAL))
      return dynamic_cast<Node *>(node);

    Node *result = NULL;
    if (0 == strcmp(dirValue, PARENT_VAL)) {
      result = node->getParent();
      checkParserExceptionWithLocation(result,
                                       nodeRef,
                                       "createExpression: Parent node reference in root node "
                                       << node->getNodeId());
      return result;
    }

    const char *name = nodeRef.child_value();
    checkParserExceptionWithLocation(*name,
                                     nodeRef,
                                     "createExpression: Empty node name");
    if (!strcmp(dirValue, CHILD_VAL)) {
      result = node->findChild(name);
      checkParserExceptionWithLocation(result,
                                       nodeRef,
                                       "createExpression: No child node named " << name 
                                       << " in node " << node->getNodeId());
      return result;
    }
    if (!strcmp(dirValue, SIBLING_VAL)) {
      Node *parent = node->getParent();
      checkParserExceptionWithLocation(parent,
                                       nodeRef,
                                       "createExpression: Sibling node reference from root node "
                                       << node->getNodeId());
      result = parent->findChild(name);
      checkParserExceptionWithLocation(result,
                                       nodeRef,
                                       "createExpression: No sibling node named " << name 
                                       << " for node " << node->getNodeId());
      return result;
    }
    else {
      reportParserExceptionWithLocation(nodeRef,
                                        "XML parsing error: Invalid value for " << DIR_ATTR << " attibute \""
                                        << dirValue << "\"");
      return NULL;
    }
  }

  static Node *findLocalNodeId(char const *name, NodeConnector *node)
  {
    // search for node ID
    if (node->getNodeId() == name)
      return dynamic_cast<Node *>(node);
    // Check children, if any
    Node *result = node->findChild(name);
    if (result)
      return result;
    return NULL;
  }

  static Node *parseNodeId(pugi::xml_node nodeRef, NodeConnector *node)
  {
    // search for node ID
    char const *name = nodeRef.child_value();
    checkParserExceptionWithLocation(*name,
                                     nodeRef,
                                     "Empty or invalid " << nodeRef.name() << " element");
    Node *result = findLocalNodeId(name, node);
    if (result)
      return result;

    Node *parent = node->getParent();
    while (parent) {
      result = findLocalNodeId(name, parent);
      if (result)
        return result;
      parent = parent->getParent();
    }
    reportParserExceptionWithLocation(nodeRef.first_child(),
                                      "createExpression: No node named "
                                      << name
                                      << " reachable from node " << node->getNodeId());
    return NULL;
  }

  static Node *parseNodeReference(pugi::xml_node nodeRef, NodeConnector *node)
  {
    const char* tag = nodeRef.name();
    checkParserExceptionWithLocation(*tag,
                                     nodeRef.parent(),
                                     "createExpression: Node reference is not an element");
    if (0 == strcmp(tag, NODEREF_TAG))
      return parseNodeRef(nodeRef, node);
    else if (0 == strcmp(tag, NODEID_TAG))
      return parseNodeId(nodeRef, node);
    reportParserExceptionWithLocation(nodeRef,
                                      "createExpression: Invalid node reference");
    return NULL;
  }
  
  //
  // Specializations for internal variables
  //

  Expression *ConcreteExpressionFactory<StateVariable>::allocate(pugi::xml_node const expr,
                                                                 NodeConnector *node,
                                                                 bool &wasCreated,
                                                                 ValueType /* returnType */) const
  {
    checkHasChildElement(expr);
    Node *refNode = parseNodeReference(expr.first_child(), node); // can throw ParserException
    wasCreated = false;
    return refNode->getStateVariable();
  }

  Expression *ConcreteExpressionFactory<OutcomeVariable>::allocate(pugi::xml_node const expr,
                                                                   NodeConnector *node,
                                                                   bool &wasCreated,
                                                                   ValueType /* returnType */) const
  {
    checkHasChildElement(expr);
    Node *refNode = parseNodeReference(expr.first_child(), node); // can throw ParserException
    wasCreated = false;
    return refNode->getOutcomeVariable();
  }

  Expression *ConcreteExpressionFactory<FailureVariable>::allocate(pugi::xml_node const expr,
                                                                   NodeConnector *node,
                                                                   bool &wasCreated,
                                                                   ValueType /* returnType */) const
  {
    checkHasChildElement(expr);
    Node *refNode = parseNodeReference(expr.first_child(), node); // can throw ParserException
    wasCreated = false;
    return refNode->getFailureTypeVariable();
  }

  Expression *ConcreteExpressionFactory<CommandHandleVariable>::allocate(pugi::xml_node const expr,
                                                                         NodeConnector *node,
                                                                         bool &wasCreated,
                                                                         ValueType /* returnType */) const
  {
    checkHasChildElement(expr);
    pugi::xml_node nodeRef = expr.first_child();
    Node *refNode = parseNodeReference(nodeRef, node); // can throw ParserException
    checkParserExceptionWithLocation(refNode->getType() == NodeType_Command,
                                     expr.first_child(),
                                     "createExpression: Node " << refNode->getNodeId()
                                     << " is not a Command node");
    CommandNode *cnode = dynamic_cast<CommandNode *>(refNode);
    assertTrue_1(cnode);
    wasCreated = false;
    return cnode->getCommand()->getAck();
  }
  
  // Specialization for node timepoint references

  Expression *ConcreteExpressionFactory<NodeTimepointValue>::allocate(pugi::xml_node const expr,
                                                                      NodeConnector *node,
                                                                      bool &wasCreated,
                                                                      ValueType /* returnType */) const
  {
    checkHasChildElement(expr);
    pugi::xml_node nodeRef = expr.first_child();
    Node *refNode = parseNodeReference(nodeRef, node); // can throw ParserException
    pugi::xml_node stateName = nodeRef.next_sibling();
    checkParserExceptionWithLocation(stateName && testTag(STATEVAL_TAG, stateName),
                                     expr,
                                     "createExpression: NodeTimepointValue has no NodeStateValue element");
    checkNotEmpty(stateName);
    NodeState state = parseNodeState(stateName.child_value());
    checkParserExceptionWithLocation(state != NO_NODE_STATE,
                                     stateName,
                                     "createExpression: Invalid NodeStateValue \""
                                     << stateName.child_value()
                                     << "\"");
    pugi::xml_node which = stateName.next_sibling();
    checkParserExceptionWithLocation(which && testTag(TIMEPOINT_TAG, which),
                                     expr,
                                     "createExpression: NodeTimepointValue has no Timepoint element");
    checkNotEmpty(which);
    char const *whichStr = which.child_value();
    bool isEnd;
    if (0 == strcmp(START_VAL, whichStr))
      isEnd = false;
    else if (0 == strcmp(END_VAL, whichStr))
      isEnd = true;
    else {
      reportParserExceptionWithLocation(which,
                                        "createExpression: Invalid Timepoint value \""
                                        << whichStr << "\"");
      return NULL;
    }
    wasCreated = false;
    return refNode->ensureTimepoint(state, isEnd);
  }

  //
  // Named constant methods
  //

  template <>
  Expression *NamedConstantExpressionFactory<NodeStateConstant>::allocate(pugi::xml_node const expr,
                                                                          NodeConnector *node,
                                                                          bool &wasCreated,
                                                                          ValueType /* returnType */) const
  {
    checkNotEmpty(expr);
    wasCreated = false;
    switch (parseNodeState(expr.child_value())) {
    case INACTIVE_STATE:
      return INACTIVE_CONSTANT();

    case WAITING_STATE:
      return WAITING_CONSTANT();

    case EXECUTING_STATE:
      return EXECUTING_CONSTANT();

    case ITERATION_ENDED_STATE:
      return ITERATION_ENDED_CONSTANT();

    case FINISHED_STATE:
      return FINISHED_CONSTANT();

    case FAILING_STATE:
      return FAILING_CONSTANT();

    case FINISHING_STATE:
      return FINISHING_CONSTANT();

    default:
      reportParserExceptionWithLocation(expr.first_child(),
                                        "createExpression: Invalid NodeStateValue \"" << expr.child_value() << "\"");
      return NULL;
    }
  }

  template <>
  Expression *NamedConstantExpressionFactory<NodeOutcomeConstant>::allocate(pugi::xml_node const expr,
                                                                            NodeConnector *node,
                                                                            bool &wasCreated,
                                                                            ValueType /* returnType */) const
  {
    checkNotEmpty(expr);
    wasCreated = false;
    switch (parseNodeOutcome(expr.child_value())) {
    case SUCCESS_OUTCOME:
      return SUCCESS_CONSTANT();

    case FAILURE_OUTCOME:
      return FAILURE_CONSTANT();

    case SKIPPED_OUTCOME:
      return SKIPPED_CONSTANT();

    case INTERRUPTED_OUTCOME:
      return INTERRUPTED_CONSTANT();

    default:
      reportParserExceptionWithLocation(expr.first_child(),
                                        "createExpression: Invalid NodeOutcomeValue \"" << expr.child_value() << "\"");
      return NULL;
    }
  }

  template <>
  Expression *NamedConstantExpressionFactory<FailureTypeConstant>::allocate(pugi::xml_node const expr,
                                                                            NodeConnector *node,
                                                                            bool &wasCreated,
                                                                            ValueType /* returnType */) const
  {
    checkNotEmpty(expr);
    wasCreated = false;
    switch (parseFailureType(expr.child_value())) {
    case PRE_CONDITION_FAILED:
      return PRE_CONDITION_FAILED_CONSTANT();

    case POST_CONDITION_FAILED:
      return POST_CONDITION_FAILED_CONSTANT();

    case INVARIANT_CONDITION_FAILED:
      return INVARIANT_CONDITION_FAILED_CONSTANT();

    case PARENT_FAILED:
      return PARENT_FAILED_CONSTANT();

    case EXITED:
      return EXITED_CONSTANT();

    case PARENT_EXITED:
      return PARENT_EXITED_CONSTANT();

    default:
      reportParserExceptionWithLocation(expr.first_child(),
                                        "createExpression: Invalid FailureTypeValue \"" << expr.child_value() << "\"");
      return NULL;
    }
  }

  template <>
  Expression *NamedConstantExpressionFactory<CommandHandleConstant>::allocate(pugi::xml_node const expr,
                                                                              NodeConnector *node,
                                                                              bool &wasCreated,
                                                                              ValueType /* returnType */) const
  {
    checkNotEmpty(expr);
    wasCreated = false;
    switch (parseCommandHandleValue(expr.child_value())) {
    case COMMAND_SENT_TO_SYSTEM:
      return COMMAND_SENT_TO_SYSTEM_CONSTANT();

    case COMMAND_ACCEPTED:
      return COMMAND_ACCEPTED_CONSTANT();

    case COMMAND_RCVD_BY_SYSTEM:
      return COMMAND_RCVD_BY_SYSTEM_CONSTANT();

    case COMMAND_FAILED:
      return COMMAND_FAILED_CONSTANT();

    case COMMAND_DENIED:
      return COMMAND_DENIED_CONSTANT();

    case COMMAND_SUCCESS:
      return COMMAND_SUCCESS_CONSTANT();

    default:
      reportParserExceptionWithLocation(expr.first_child(),
                                        "createExpression: Invalid CommandHandleValue \"" << expr.child_value() << "\"");
      return NULL;
    }
  }

// Convenience macros
#define ENSURE_NAMED_CONSTANT_FACTORY(CLASS) template class PLEXIL::NamedConstantExpressionFactory<CLASS >;

  // Named constants
  ENSURE_NAMED_CONSTANT_FACTORY(NodeStateConstant);
  ENSURE_NAMED_CONSTANT_FACTORY(NodeOutcomeConstant);
  ENSURE_NAMED_CONSTANT_FACTORY(FailureTypeConstant);
  ENSURE_NAMED_CONSTANT_FACTORY(CommandHandleConstant);

} // namespace PLEXIL

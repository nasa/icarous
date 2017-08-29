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

#include "Assignable.hh"
#include "Command.hh"
#include "CommandNode.hh"
#include "Error.hh"
#include "ExpressionFactory.hh"
#include "ExprVec.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"
#include "SymbolTable.hh"
#include "resource-tags.hh"

#include "pugixml.hpp"

#include <cstring>

using pugi::xml_node;

namespace PLEXIL
{
  // First pass
  
  // Entry point for unit test; see wrapper below for parser
  
  Command *constructCommand(NodeConnector *node, xml_node const cmdXml)
    throw (ParserException)
  {
    checkHasChildElement(cmdXml);
    xml_node temp = cmdXml.first_child();

    // Optional ResourceList
    if (testTag(RESOURCE_LIST_TAG, temp)) {
      // Process resource list
      for (xml_node resourceElt = temp.first_child(); 
           resourceElt;
           resourceElt = resourceElt.next_sibling()) {
        checkTag(RESOURCE_TAG, resourceElt);
        // check that the resource has a name and a priority
        checkParserExceptionWithLocation(resourceElt.child(RESOURCE_NAME_TAG),
                                         resourceElt,
                                         "No " << RESOURCE_NAME_TAG << " element for resource");
        checkParserExceptionWithLocation(resourceElt.child(RESOURCE_PRIORITY_TAG),
                                         resourceElt,
                                         "No " << RESOURCE_PRIORITY_TAG << " element for resource");
        // save rest for 2nd pass
      }

      temp = temp.next_sibling();
    }

    // Optional destination expression
    // Ensure it's a user variable reference or ArrayElement
    if (testTagSuffix(VAR_SUFFIX, temp) || testTag(ARRAYELEMENT_TAG, temp)) {
      temp = temp.next_sibling();
    }

    // Required command name expression
    checkTag(NAME_TAG, temp);
    checkHasChildElement(temp);

    // Optional arguments
    temp = temp.next_sibling();
    if (temp)
      checkTag(ARGS_TAG, temp);

    Command *result = new Command(node->getNodeId());
    return result;
  }

  // Entry point from parser

  void constructAndSetCommand(CommandNode *node, xml_node const cmdXml)
    throw (ParserException)
  {
    assertTrue_1(node);
    node->setCommand(constructCommand(node, cmdXml));
  }

  static void finalizeResourceList(NodeConnector *node,
                                   Command *cmd,
                                   xml_node const rlist)
    throw (ParserException)
  {
    ResourceList *resources =
      new ResourceList(std::distance(rlist.begin(), rlist.end()));
    size_t n = 0;
    try {
      for (xml_node resourceElt = rlist.first_child(); 
           resourceElt;
           resourceElt = resourceElt.next_sibling()) {
        checkTag(RESOURCE_TAG, resourceElt);
        // Update ResourceSpec in place
        ResourceSpec &rspec = (*resources)[n++];
        for (xml_node rtemp = resourceElt.first_child();
             rtemp;
             rtemp = rtemp.next_sibling()) {
          char const* tag = rtemp.name();
          size_t taglen = strlen(tag);
          bool isGarbage = false;
          Expression *exp = NULL;
          switch (taglen) {
          case 12: // ResourceName
            checkParserExceptionWithLocation(0 == strcmp(RESOURCE_NAME_TAG, tag),
                                             rtemp,
                                             "Invalid " << tag << " element in Command Resource");
            checkParserExceptionWithLocation(rspec.nameExp == NULL,
                                             rtemp,
                                             "Duplicate " << RESOURCE_NAME_TAG << " element in Command Resource");
            exp = createExpression(rtemp.first_child(), node, isGarbage);
            checkParserExceptionWithLocation(exp->valueType() == STRING_TYPE || exp->valueType() == UNKNOWN_TYPE,
                                             rtemp.first_child(),
                                             RESOURCE_NAME_TAG << " expression is not String valued in Command Resource");
            rspec.setNameExpression(exp, isGarbage);
            break;

          case 16: // ResourcePriority
            checkParserExceptionWithLocation(0 == strcmp(RESOURCE_PRIORITY_TAG, tag),
                                             rtemp,
                                             "Invalid " << tag << " element in Command Resource");
            checkParserExceptionWithLocation(rspec.priorityExp == NULL,
                                             rtemp,
                                             "Duplicate " << RESOURCE_PRIORITY_TAG << " element in Command Resource");
            exp = createExpression(rtemp.first_child(), node, isGarbage);
            checkParserExceptionWithLocation(exp->valueType() == INTEGER_TYPE || exp->valueType() == UNKNOWN_TYPE,
                                             rtemp.first_child(),
                                             RESOURCE_PRIORITY_TAG << " expression is not Integer valued in Command Resource");
            rspec.setPriorityExpression(exp, isGarbage);
            break;

          case 18: // ResourceLowerBound, ResourceUpperBound
            if (0 == strcmp(RESOURCE_LOWER_BOUND_TAG, tag)) {
              checkParserExceptionWithLocation(rspec.lowerBoundExp == NULL,
                                               rtemp,
                                               "Duplicate " << RESOURCE_LOWER_BOUND_TAG << " element in Command Resource");
              exp = createExpression(rtemp.first_child(), node, isGarbage);
              checkParserExceptionWithLocation(isNumericType(exp->valueType()) || exp->valueType() == UNKNOWN_TYPE,
                                               rtemp.first_child(),
                                               RESOURCE_LOWER_BOUND_TAG << " expression is not a numeric expression in Command Resource");
              rspec.setLowerBoundExpression(exp, isGarbage);
            }
            else {
              checkParserExceptionWithLocation(0 == strcmp(RESOURCE_UPPER_BOUND_TAG, tag),
                                               rtemp,
                                               "Invalid " << tag << " element in Command Resource");
              checkParserExceptionWithLocation(rspec.upperBoundExp == NULL,
                                               rtemp,
                                               "Duplicate " << RESOURCE_UPPER_BOUND_TAG << " element in Command Resource");
              exp = createExpression(rtemp.first_child(), node, isGarbage);
              checkParserExceptionWithLocation(isNumericType(exp->valueType()) || exp->valueType() == UNKNOWN_TYPE,
                                               rtemp.first_child(),
                                               RESOURCE_UPPER_BOUND_TAG << " expression is not a numeric expression in Command Resource");
              rspec.setUpperBoundExpression(exp, isGarbage);
            }
            break;

          default:
            checkParserExceptionWithLocation(0 == strcmp(RESOURCE_RELEASE_AT_TERMINATION_TAG, tag),
                                             rtemp,
                                             "Invalid " << tag << " element in Command Resource");
            checkParserExceptionWithLocation(rspec.releaseAtTermExp == NULL,
                                             rtemp,
                                             "Duplicate " << RESOURCE_RELEASE_AT_TERMINATION_TAG << " element in Command Resource");
            exp = createExpression(rtemp.first_child(), node, isGarbage);
            checkParserExceptionWithLocation(exp->valueType() == BOOLEAN_TYPE || exp->valueType() == UNKNOWN_TYPE,
                                             rtemp.first_child(),
                                             RESOURCE_RELEASE_AT_TERMINATION_TAG << " expression is not a Boolean expression in Command");
            rspec.setReleaseAtTerminationExpression(exp, isGarbage);
            break;
          }
        }
      }
    }
    catch (ParserException const &e) {
      delete resources;
      throw;
    }

    cmd->setResourceList(resources);
  }

  // Entry point from unit test and wrapper below

  void finalizeCommand(Command *cmd, NodeConnector *node, xml_node const cmdXml)
    throw (ParserException)
  {
    xml_node temp = cmdXml.first_child();

    // Optional ResourceList needs expressions parsed
    if (testTag(RESOURCE_LIST_TAG, temp)) {
      finalizeResourceList(node, cmd, temp);
      temp = temp.next_sibling();
    }

    // Optional destination expression
    xml_node destXml;
    Expression *dest = NULL;
    if (!testTag(NAME_TAG, temp)) {
      destXml = temp;
      bool destIsGarbage = false;
      dest = createAssignable(temp, node, destIsGarbage);
      cmd->setDestination(dest, destIsGarbage);
      temp = temp.next_sibling();
    }

    // Required command name expression
    checkTag(NAME_TAG, temp); // belt-and-suspenders check
    xml_node nameElt = temp.first_child();
    bool nameIsGarbage = false;
    Expression *nameExpr = createExpression(nameElt, node, nameIsGarbage);
    ValueType nameType = nameExpr->valueType();
    if (nameType != STRING_TYPE && nameType != UNKNOWN_TYPE) {
      if (nameIsGarbage)
        delete nameExpr;
      reportParserExceptionWithLocation(temp,
                                        "Command Name must be a String expression");
    }

    cmd->setNameExpr(nameExpr, nameIsGarbage);

    // Get symbol table entry, if name is constant and declared as command
    Symbol const *cmdSym = NULL;
    if (nameExpr->isConstant() && nameType == STRING_TYPE) {
      std::string cmdName = nameExpr->valueString();
      cmdSym = g_symbolTable->getCommand(cmdName.c_str());
    }

    if (cmdSym && dest) {
      // Check destination consistency with command declaration
      checkParserExceptionWithLocation(areTypesCompatible(dest->valueType(),
                                                          cmdSym->returnType()),
                                       destXml,
                                       "Command " << cmdSym->name() << " returns type "
                                       << cmdSym->returnType() << ", but result variable has type "
                                       << dest->valueType());
    }

    // Optional arguments
    temp = temp.next_sibling();
    if (temp) {
      size_t n = 0;
      xml_node arg;
      for (arg = temp.first_child(); arg; arg = arg.next_sibling())
        ++n;
      if (cmdSym) {
        // Check argument count against command declaration
        checkParserExceptionWithLocation(n == cmdSym->parameterCount()
                                         || (cmdSym->anyParameters() && n > cmdSym->parameterCount()),
                                         temp,
                                         "Command " << cmdSym->name() << " expects "
                                         << (cmdSym->anyParameters() ? "at least " : "")
                                         << cmdSym->parameterCount() << " arguments, but was supplied "
                                         << n);
      }
      if (n) {
        ExprVec *argVec = makeExprVec(n);
        cmd->setArgumentVector(argVec);

        size_t i = 0;
        for (arg = temp.first_child(); arg; arg = arg.next_sibling(), ++i) {
          bool wasCreated = false;
          Expression *thisArg = createExpression(arg, node, wasCreated);
          argVec->setArgument(i, thisArg, wasCreated);

          if (cmdSym && i < cmdSym->parameterCount()) {
            // Check argument type against declaration
            ValueType actual = thisArg->valueType();
            ValueType expected = cmdSym->parameterType(i);
            checkParserExceptionWithLocation(areTypesCompatible(expected, actual),
                                             arg,
                                             "Parameter " << i << " to command "
                                             << cmdSym->name() << " should be of type "
                                             << valueTypeName(expected)
                                             << ", but has type "
                                             << valueTypeName(actual));
          }
        }
      }
    }
  }

  // Entry point from parser

  void finalizeCommandNode(CommandNode *node, xml_node const cmdXml)
    throw (ParserException)
  {
    assertTrue_1(node);
    finalizeCommand(node->getCommand(), node, cmdXml);
  }

} // namespace PLEXIL

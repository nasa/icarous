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

#include "Assignment.hh"
#include "AssignmentNode.hh"
#include "Error.hh"
#include "ExpressionFactory.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"

#include "pugixml.hpp"

#include <cerrno>
#include <cstdlib> // for strtoul()
#include <limits>

using pugi::xml_node;

namespace PLEXIL
{

  // First pass
  static void parsePriority(AssignmentNode *anode, xml_node const nodeXml)
    throw (ParserException)
  {
    xml_node const prio = nodeXml.child(PRIORITY_TAG);
    if (!prio)
      return; // nothing to do

    char const *prioString = prio.child_value();
    checkParserExceptionWithLocation(*prioString,
                                     prio,
                                     "Priority element is empty");
    char *endptr = NULL;
    errno = 0;
    unsigned long prioValue = strtoul(prioString, &endptr, 10);
    checkParserExceptionWithLocation(endptr != prioString && !*endptr,
                                     prio,
                                     "Priority element does not contain a non-negative integer");
    checkParserExceptionWithLocation(!errno,
                                     prio,
                                     "Priority element contains negative or out-of-range integer");
    checkParserExceptionWithLocation(prioValue < (unsigned long) std::numeric_limits<int32_t>::max(),
                                     prio,
                                     "Priority element contains out-of-range integer");
    anode->setPriority((int32_t) prioValue);
  }

  static void checkAssignment(std::string const &nodeId, xml_node const nodeXml)
    throw (ParserException)
  {
    xml_node const assn = nodeXml.child(BODY_TAG).first_child();
    checkTag(ASSN_TAG, assn);

    xml_node const varXml = assn.first_child();
    xml_node const rhsXml = varXml.next_sibling();
    checkParserExceptionWithLocation(rhsXml,
                                     assn,
                                     "Assignment Node " << nodeId
                                     << ": Malformed Assignment element");
    checkTagSuffix(RHS_TAG, rhsXml);
  }

  // First pass
  void constructAssignment(AssignmentNode *anode, xml_node const xml)
    throw (ParserException)
  {
    assertTrue_1(anode);

    // Can throw ParserException
    checkAssignment(anode->getNodeId(), xml);
    parsePriority(anode, xml);
    
    // Just construct it, will be populated in second pass
    anode->setAssignment(new Assignment(anode->getNodeId()));
  }

  // Second pass
  void finalizeAssignment(AssignmentNode *anode, xml_node const assn)
    throw (ParserException)
  {
    assertTrue_1(anode);
    Assignment *assign = anode->getAssignment();
    assertTrue_2(anode, "finalizeAssignment: AssignmentNode without an Assignment");
    xml_node temp = assn.first_child();
    bool varGarbage = false;
    Expression *var = createAssignable(temp, anode, varGarbage);
    assertTrue_2(var, "finalizeAssignment: Internal error: null LHS expression");
    ValueType varType = var->valueType();
    temp = temp.next_sibling().first_child();
    bool rhsGarbage = false;
    Expression *rhs = NULL;
    try {
      rhs = createExpression(temp, anode, rhsGarbage, varType);
    }
    catch (ParserException &e) {
      if (varGarbage)
        delete var;
      throw;
    }
    assertTrue_2(rhs, "finalizeAssignment: Internal error: null RHS expression");
    ValueType rhsType = rhs->valueType();
    if (!areTypesCompatible(varType, rhsType)) {
      if (varGarbage)
        delete var;
      if (rhsGarbage)
        delete rhs;
      reportParserExceptionWithLocation(assn,
                                        "Assignment Node " << anode->getNodeId()
                                        << ": RHS expression type "
                                        << valueTypeName(rhsType)
                                        << " incompatible with variable of type "
                                        << valueTypeName(varType));
    }
    assign->setVariable(var, varGarbage);
    assign->setExpression(rhs, rhsGarbage);
  }

} // namespace PLEXIL

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

#include "ConcreteExpressionFactory.hh"

#include "ArrayReference.hh"
#include "ArrayVariable.hh"
#include "Constant.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExpressionConstants.hh"
#include "NodeConnector.hh"
#include "ParserException.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"
#include "UserVariable.hh"

#include "pugixml.hpp"

#include <sstream>

#include <cstring>

// Local convenience macros
#define ENSURE_EXPRESSION_FACTORY(CLASS) template class PLEXIL::ConcreteExpressionFactory<CLASS >;

namespace PLEXIL
{

  //
  // Factories for scalar constants
  //

  // General case. For all but string types, the value string may not be empty.
  template <typename T>
  Expression *ConcreteExpressionFactory<Constant<T> >::allocate(pugi::xml_node const expr,
                                                                NodeConnector * /* node */,
                                                                bool &wasCreated,
                                                                ValueType /* returnType */) const
  {
    checkParserExceptionWithLocation(expr.first_child() && *(expr.child_value()),
                                     expr,
                                     "Empty value is not valid for \"" << expr.name() << "\"");

    wasCreated = true;
    T value;
    if (parseValue(expr.child_value(), value))
      return new Constant<T>(value);
    else 
      return new Constant<T>();
  }

  // Since there are exactly 3 possible Boolean constants, return references to them.
  template <>
  Expression *ConcreteExpressionFactory<Constant<bool> >::allocate(pugi::xml_node const expr,
                                                                   NodeConnector * /* node */,
                                                                   bool &wasCreated,
                                                                   ValueType /* returnType */) const
  {
    checkParserExceptionWithLocation(expr.first_child() && *(expr.child_value()),
                                     expr,
                                     "Empty value is not valid for \"" << expr.name() << "\"");

    bool value;
    bool known = parseValue(expr.child_value(), value);
    // if we got here, there was no parsing exception
    wasCreated = false;
    if (!known)
      return UNKNOWN_BOOLEAN_EXP();
    else if (value)
      return TRUE_EXP();
    else
      return FALSE_EXP();
  }

  // Look for common Integer values, e.g. 1, 0, -1
  template <>
  Expression *ConcreteExpressionFactory<Constant<int32_t> >::allocate(pugi::xml_node const expr,
                                                                      NodeConnector * /* node */,
                                                                      bool &wasCreated,
                                                                      ValueType /* returnType */) const
  {
    // check for empty value
    checkParserExceptionWithLocation(expr.first_child() && *(expr.child_value()),
                                     expr,
                                     "Empty value is not valid for \"" << expr.name() << "\"");

    int32_t value;
    if (!parseValue<int32_t>(expr.child_value(), value)) {
      // Unknown
      wasCreated = true;
      return new Constant<int32_t>();
    }

    // Known
    switch (value) {
    case 1:
      wasCreated = false;
      return INT_ONE_EXP();

    case 0:
      wasCreated = false;
      return INT_ZERO_EXP();

    case -1:
      wasCreated = false;
      return INT_MINUS_ONE_EXP();

    default:
      wasCreated = true;
      return new Constant<int32_t>(value);
    }
  }

  // String can be empty, don't care about contents
  template <>
  Expression *ConcreteExpressionFactory<Constant<std::string> >::allocate(pugi::xml_node const expr,
                                                                          NodeConnector * /* node */,
                                                                          bool &wasCreated,
                                                                          ValueType /* returnType */) const
  {
    wasCreated = true;
    return new Constant<std::string>(expr.child_value());
  }

  //
  // Array references
  //

  // Common subroutine
  static void parseArrayElement(pugi::xml_node const expr,
                                NodeConnector *node,
                                Expression *&arrayExpr,
                                Expression *&indexExpr,
                                bool &arrayCreated,
                                bool &indexCreated)
  {
    // Syntax checks
    checkHasChildElement(expr);
    pugi::xml_node arrayXml = expr.first_child();
    checkParserExceptionWithLocation(arrayXml && arrayXml.type() == pugi::node_element,
                                     expr,
                                     "Ill-formed ArrayElement expression");
    pugi::xml_node indexXml = arrayXml.next_sibling();
    checkParserExceptionWithLocation(indexXml && testTag(INDEX_TAG, indexXml),
                                     expr,
                                     "ArrayElement has no Index element");
    checkHasChildElement(indexXml);
    indexXml = indexXml.first_child();
    checkParserExceptionWithLocation(indexXml.type() == pugi::node_element,
                                     indexXml,
                                     "ArrayElement Index is not an element");

    // Checks on array
    if (testTag(NAME_TAG, arrayXml)) {
      checkNotEmpty(arrayXml);
      const char *arrayName = arrayXml.child_value();
      arrayExpr = node->findVariable(arrayName);
      checkParserExceptionWithLocation(arrayExpr,
                                       arrayXml,
                                       "No array variable named \""
                                       << arrayName << "\" accessible from node "
                                       << node->getNodeId());
      checkParserExceptionWithLocation(isArrayType(arrayExpr->valueType()),
                                       arrayXml,
                                       "Variable \"" << arrayName
                                       << "\" is not an array variable");
    }
    else {
      arrayExpr = createExpression(arrayXml, node, arrayCreated);
      // Have to allow for UNKNOWN (e.g. lookups)
      ValueType exprType = arrayExpr->valueType();
      checkParserExceptionWithLocation(isArrayType(exprType) || exprType == UNKNOWN_TYPE,
                                       arrayXml,
                                       "Array expression is not an array");
    }

    // Checks on index
    indexExpr = createExpression(indexXml, node, indexCreated);
    assertTrue_1(indexExpr);
    ValueType indexType = indexExpr->valueType();
    checkParserExceptionWithLocation(indexType == INTEGER_TYPE || indexType == UNKNOWN_TYPE,
                                     indexXml,
                                     "Array index expression is not numeric");
  }

  Expression *
  ConcreteExpressionFactory<ArrayReference>::allocate(pugi::xml_node const expr,
                                                      NodeConnector *node,
                                                      bool & wasCreated,
                                                      ValueType /* returnType */) const
  {
    Expression *arrayExpr = NULL;
    Expression *indexExpr = NULL;
    bool arrayCreated = false;
    bool indexCreated = false;
    
    parseArrayElement(expr, node, arrayExpr, indexExpr, arrayCreated, indexCreated);

    wasCreated = true;
    return new ArrayReference(arrayExpr, indexExpr, arrayCreated, indexCreated);
  }

  Expression *createMutableArrayReference(pugi::xml_node const expr,
                                          NodeConnector *node,
                                          bool & wasCreated)
  {
    Expression *arrayExpr = NULL;
    Expression *indexExpr = NULL;
    bool arrayCreated = false;
    bool indexCreated = false;
    
    parseArrayElement(expr, node, arrayExpr, indexExpr, arrayCreated, indexCreated);
    try {
      checkParserExceptionWithLocation(arrayExpr->isAssignable(),
                                       expr,
                                       "Can't create a writeable array reference on a read-only array expression");
    }
    catch (ParserException & e) {
      if (arrayCreated)
        delete arrayExpr;
      if (indexCreated)
        delete indexExpr;
    }

    wasCreated = true;
    return new MutableArrayReference(arrayExpr, indexExpr, arrayCreated, indexCreated);
  }

  // Generic variable references
  Expression *
  VariableReferenceFactory::allocate(pugi::xml_node const expr,
                                     NodeConnector *node,
                                     bool & wasCreated,
                                     ValueType /* returnType */) const
  {
    assertTrue_1(node); // internal error
    checkNotEmpty(expr);
    char const *varName = expr.child_value();
    checkParserExceptionWithLocation(*varName,
                                     expr,
                                     "Empty or malformed " << expr.name() << " element");
    Expression *result = node->findVariable(varName);
    checkParserExceptionWithLocation(result,
                                     expr,
                                     "No variable named " << varName << " accessible in this context");
    bool match = (m_type == result->valueType());
    // *** FIXME? ***
    // Shouldn't be parsing reference to Integer variables as Real
    if (!match
        && m_type == REAL_TYPE
        && result->valueType() == INTEGER_TYPE)
      match = true; // expecting Real, but naming an Integer variable
    checkParserExceptionWithLocation(match,
				     expr,
				     "Variable " << varName
				     << " has invalid type " << valueTypeName(result->valueType())
				     << " for a " << expr.name());
    wasCreated = false;
    return result;
  }

  // Explicit instantiations
  ENSURE_EXPRESSION_FACTORY(BooleanConstant);
  ENSURE_EXPRESSION_FACTORY(IntegerConstant);
  ENSURE_EXPRESSION_FACTORY(RealConstant);
  ENSURE_EXPRESSION_FACTORY(StringConstant);

  ENSURE_EXPRESSION_FACTORY(BooleanVariable);
  ENSURE_EXPRESSION_FACTORY(IntegerVariable);
  ENSURE_EXPRESSION_FACTORY(RealVariable);
  ENSURE_EXPRESSION_FACTORY(StringVariable);
  ENSURE_EXPRESSION_FACTORY(BooleanArrayVariable);
  ENSURE_EXPRESSION_FACTORY(IntegerArrayVariable);
  ENSURE_EXPRESSION_FACTORY(RealArrayVariable);
  ENSURE_EXPRESSION_FACTORY(StringArrayVariable);

} // namespace PLEXIL


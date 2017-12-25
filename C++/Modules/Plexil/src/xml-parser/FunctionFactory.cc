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

#include "FunctionFactory.hh"

#include "ArithmeticOperators.hh"
#include "BooleanOperators.hh"
#include "Comparisons.hh"
#include "parser-utils.hh"

#include "pugixml.hpp"

namespace PLEXIL
{
  FunctionFactory::FunctionFactory(std::string const &name)
    : ExpressionFactory(name)
  {
  }

  FunctionFactory::~FunctionFactory()
  {
  }

  Expression *FunctionFactory::allocate(pugi::xml_node const expr,
                                        NodeConnector *node,
                                        bool &wasCreated,
                                        ValueType returnType) const
  {
    size_t n = std::distance(expr.begin(), expr.end());
    Operator const *oper = this->getOperator();
    checkParserExceptionWithLocation(oper->checkArgCount(n),
                                     expr,
                                     "Wrong number of operands for operator "
                                     << oper->getName());

    Function *result = makeFunction(oper, n);
    try {
      size_t i = 0;
      for (pugi::xml_node subexp = expr.first_child();
           subexp && i < n;
           subexp = subexp.next_sibling(), ++i) {
        bool created;
        Expression *arg = createExpression(subexp, node, created, returnType);
        result->setArgument(i, arg, created);
      }
    }
    catch (ParserException & /* e */) {
      delete result;
      throw;
    }

    if (!oper->checkArgTypes(result)) {
      delete result;
      reportParserExceptionWithLocation(expr,
                                        "Operand type mismatch or unimplemented type for "
                                        << oper->getName());
    }

    wasCreated = true;
    return result;
  }

  //
  // Concrete instantiations of class templates
  //

  // Convenience macros
#define ENSURE_FUNCTION_FACTORY(CLASS) template class PLEXIL::FunctionFactoryImpl<CLASS>;

  // Comparisons
  ENSURE_FUNCTION_FACTORY(Equal);
  ENSURE_FUNCTION_FACTORY(NotEqual);

  // Not currently in the schema
  // ENSURE_FUNCTION_FACTORY(GreaterThan<std::string>);
  // ENSURE_FUNCTION_FACTORY(GreaterEqual<std::string>);
  // ENSURE_FUNCTION_FACTORY(LessThan<std::string>);
  // ENSURE_FUNCTION_FACTORY(LessEqual<std::string>);

  // Boolean operators
  ENSURE_FUNCTION_FACTORY(BooleanNot);
  ENSURE_FUNCTION_FACTORY(BooleanOr);
  ENSURE_FUNCTION_FACTORY(BooleanAnd);
  ENSURE_FUNCTION_FACTORY(BooleanXor);
  ENSURE_FUNCTION_FACTORY(SquareRoot<double>);
  ENSURE_FUNCTION_FACTORY(RealToInteger);


} // namespace PLEXIL

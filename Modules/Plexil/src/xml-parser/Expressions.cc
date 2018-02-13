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

#include "ArithmeticFunctionFactory.hh"
#include "ArithmeticOperators.hh"
#include "ArrayLiteralFactory.hh"
#include "ArrayOperators.hh"
#include "ArrayVariable.hh"
#include "ArrayVariableFactory.hh"
#include "ArrayVariableReferenceFactory.hh"
#include "BooleanOperators.hh"
#include "Comparisons.hh"
#include "ConcreteExpressionFactory.hh"
#include "Constant.hh"
#include "FunctionFactory.hh"
#include "InternalExpressionFactories.hh"
#include "LookupFactory.hh"
#include "NodeConstantExpressions.hh"
#include "NodeVariables.hh"
#include "StringOperators.hh"
#include "UserVariable.hh"
#include "UserVariableFactory.hh"
#include "lifecycle-utils.h"

// Local convenience macros
#define REGISTER_EXPRESSION(CLASS,NAME) {new PLEXIL::ConcreteExpressionFactory<CLASS >(#NAME);}
#define REGISTER_NAMED_CONSTANT_FACTORY(CLASS,NAME) {new PLEXIL::NamedConstantExpressionFactory<CLASS >(#NAME);}

namespace PLEXIL
{

  void initializeExpressions()
  {
    static bool sl_inited = false;
    if (!sl_inited) {
      plexilAddFinalizer(&purgeExpressionFactories);

      //
      // IMPORTANT! Please sort these in alpha order by expression name,
      // as it affects the speed of factory map initialization.
      // Remember, upper case precedes lower case in ASCII order.
      //

      REGISTER_ARITHMETIC_FUNCTION(AbsoluteValue, ABS);
      REGISTER_ARITHMETIC_FUNCTION(Addition, ADD);

      REGISTER_FUNCTION(AllElementsKnown, ALL_KNOWN);
      REGISTER_FUNCTION(BooleanAnd, AND);
      REGISTER_FUNCTION(AnyElementsKnown, ANY_KNOWN);

      REGISTER_FUNCTION(ArraySize, ArraySize);
      REGISTER_FUNCTION(ArrayMaxSize, ArrayMaxSize);

      REGISTER_EXPRESSION(ArrayReference, ArrayElement);
      new ArrayLiteralFactory("ArrayValue"); // for effect
      new ArrayVariableReferenceFactory("ArrayVariable"); // for effect

      REGISTER_EXPRESSION(BooleanConstant, BooleanValue);
      new VariableReferenceFactory("BooleanVariable", BOOLEAN_TYPE); // for effect

      REGISTER_ARITHMETIC_FUNCTION(Ceiling, CEIL);
      REGISTER_FUNCTION(StringConcat, Concat);

      REGISTER_ARITHMETIC_FUNCTION(Division, DIV);

      new ArrayVariableFactory("DeclareArray"); // for effect
      new UserVariableFactory("DeclareVariable"); // for effect

      REGISTER_FUNCTION(Equal, EQBoolean);
      REGISTER_FUNCTION(Equal, EQInternal);
      REGISTER_FUNCTION(Equal, EQNumeric);
      REGISTER_FUNCTION(Equal, EQString);
      REGISTER_FUNCTION(Equal, EQArray);

      REGISTER_ARITHMETIC_FUNCTION(Floor, FLOOR);

      REGISTER_ARITHMETIC_FUNCTION(GreaterEqual, GE);
      REGISTER_ARITHMETIC_FUNCTION(GreaterThan, GT);

      REGISTER_EXPRESSION(IntegerConstant, IntegerValue);
      new VariableReferenceFactory("IntegerVariable", INTEGER_TYPE); // for effect

      REGISTER_FUNCTION(IsKnown, IsKnown);

      REGISTER_ARITHMETIC_FUNCTION(LessEqual, LE);
      REGISTER_ARITHMETIC_FUNCTION(LessThan, LT);

      new LookupFactory("LookupNow"); // for effect
      new LookupFactory("LookupOnChange");  // for effect

      REGISTER_ARITHMETIC_FUNCTION(Maximum, MAX);
      REGISTER_ARITHMETIC_FUNCTION(Minimum, MIN);
      REGISTER_ARITHMETIC_FUNCTION(Modulo, MOD);
      REGISTER_ARITHMETIC_FUNCTION(Multiplication, MUL);

      REGISTER_FUNCTION(NotEqual, NEBoolean);
      REGISTER_FUNCTION(NotEqual, NEInternal);
      REGISTER_FUNCTION(NotEqual, NENumeric);
      REGISTER_FUNCTION(NotEqual, NEString);
      REGISTER_FUNCTION(NotEqual, NEArray);

      REGISTER_FUNCTION(BooleanNot, NOT);

      REGISTER_NAMED_CONSTANT_FACTORY(CommandHandleConstant, NodeCommandHandleValue);
      REGISTER_EXPRESSION(CommandHandleVariable, NodeCommandHandleVariable);

      REGISTER_NAMED_CONSTANT_FACTORY(FailureTypeConstant, NodeFailureValue);
      REGISTER_EXPRESSION(FailureVariable, NodeFailureVariable);

      REGISTER_NAMED_CONSTANT_FACTORY(NodeOutcomeConstant, NodeOutcomeValue);
      REGISTER_EXPRESSION(OutcomeVariable, NodeOutcomeVariable);

      REGISTER_NAMED_CONSTANT_FACTORY(NodeStateConstant, NodeStateValue);
      REGISTER_EXPRESSION(StateVariable, NodeStateVariable);

      REGISTER_EXPRESSION(NodeTimepointValue, NodeTimepointValue);

      REGISTER_FUNCTION(BooleanOr, OR);

      REGISTER_FUNCTION(RealToInteger, REAL_TO_INT);

  // Believe it or not, VxWorks 6.8 for PowerPC doesn't have round()
#if !defined(__VXWORKS__)
      REGISTER_ARITHMETIC_FUNCTION(Round, ROUND);
#endif // !defined(__VXWORKS__)

      REGISTER_EXPRESSION(RealConstant, RealValue);
      new VariableReferenceFactory("RealVariable", REAL_TYPE); // for effect

      REGISTER_FUNCTION(SquareRoot<Real>, SQRT);

      REGISTER_FUNCTION(StringLength, STRLEN);

      REGISTER_ARITHMETIC_FUNCTION(Subtraction, SUB);

      REGISTER_EXPRESSION(StringConstant, StringValue);
      new VariableReferenceFactory("StringVariable", STRING_TYPE); // for effect

  // Believe it or not, VxWorks 6.8 for PowerPC doesn't have trunc()
#if !defined(__VXWORKS__)
      REGISTER_ARITHMETIC_FUNCTION(Truncate, TRUNC);
#endif // !defined(__VXWORKS__)

      REGISTER_FUNCTION(BooleanXor, XOR);

      sl_inited = true;
    }
  }

} // namespace PLEXIL

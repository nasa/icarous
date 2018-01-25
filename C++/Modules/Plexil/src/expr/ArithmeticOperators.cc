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

#include "ArithmeticOperators.hh"
#include "Function.hh"
// #include "PlanError.hh" // included by OperatorImpl.hh

#include <cmath>
#include <limits>

namespace PLEXIL
{

  // TODO:
  // - Overflow/underflow checks

  //
  // Addition
  //

  template <typename NUM>
  Addition<NUM>::Addition()
    : OperatorImpl<NUM>("ADD")
  {
  }

  template <typename NUM>
  Addition<NUM>::~Addition()
  {
  }

  template <typename NUM>
  bool Addition<NUM>::checkArgCount(size_t count) const
  {
    return true;
  }

  template <typename NUM>
  bool Addition<NUM>::calc(NUM &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  // TODO: overflow checks
  template <typename NUM>
  bool Addition<NUM>::calc(NUM &result, Expression const *arg0, Expression const *arg1) const
  {
    NUM temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1)))
      return false;
    result = temp0 + temp1;
    return true;
  }

  // TODO: overflow checks
  template <typename NUM>
  bool Addition<NUM>::calc(NUM &result,
                           Function const &args) const
  {
    NUM workingResult = 0;
    for (size_t i = 0; i < args.size(); ++i) {
      NUM temp;
      if (!args[i]->getValue(temp))
        return false; // unknown if any arg unknown
      workingResult += temp;
    }
    result = workingResult;
    return true;
  }

  //
  // Subtraction
  //

  template <typename NUM>
  Subtraction<NUM>::Subtraction()
    : OperatorImpl<NUM>("SUB")
  {
  }

  template <typename NUM>
  Subtraction<NUM>::~Subtraction()
  {
  }

  template <typename NUM>
  bool Subtraction<NUM>::checkArgCount(size_t count) const
  {
    return count >= 1;
  }

  // TODO:
  // - Overflow checks
  // - If we extend to unsigned numeric types, add an error message for these methods 

  template <typename NUM>
  bool Subtraction<NUM>::calc(NUM &result, Expression const *arg) const
  {
    NUM temp;
    if (!arg->getValue(temp))
      return false;
    result = -temp;
    return true;
  }

  template <typename NUM>
  bool Subtraction<NUM>::calc(NUM &result, Expression const *arg0, Expression const *arg1) const
  {
    NUM temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1)))
      return false;
    result = temp0 - temp1;
    return true;
  }

  template <typename NUM>
  bool Subtraction<NUM>::calc(NUM &result,
                              Function const &args) const
  {
    checkPlanError(args.size() > 0,
                   this->getName() << " requires at least one operand");
    NUM temp;
    if (!args[0]->getValue(temp))
      return false;
    if (args.size() == 1) {
      // Unary
      result = -temp;
      return true;
    }
    // 2 or more args
    NUM workingResult = temp;
    for (size_t i = 1; i < args.size(); ++i) {
      if (!args[i]->getValue(temp))
        return false;
      workingResult -= temp;
    }
    result = workingResult;
    return true;
  }

  //
  // Multiplication
  //

  template <typename NUM>
  Multiplication<NUM>::Multiplication()
    : OperatorImpl<NUM>("MUL")
  {
  }

  template <typename NUM>
  Multiplication<NUM>::~Multiplication()
  {
  }

  template <typename NUM>
  bool Multiplication<NUM>::checkArgCount(size_t count) const
  {
    return count > 0;
  }

  template <typename NUM>
  bool Multiplication<NUM>::calc(NUM &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  // TODO: overflow checks
  template <typename NUM>
  bool Multiplication<NUM>::calc(NUM &result, Expression const *arg0, Expression const *arg1) const
  {
    NUM temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1)))
      return false;
    result = temp0 * temp1;
    return true;
  }

  // TODO: overflow checks
  template <typename NUM>
  bool Multiplication<NUM>::calc(NUM &result,
                                 Function const &args) const
  {
    NUM workingResult, temp;
    if (!args[0]->getValue(workingResult))
      return false;
    for (size_t i = 1; i < args.size(); ++i) {
      if (!args[i]->getValue(temp))
        return false;
      workingResult *= temp;
    }
    result = workingResult;
    return true;
  }

  //
  // Division
  //

  template <typename NUM>
  Division<NUM>::Division()
    : OperatorImpl<NUM>("DIV")
  {
  }

  template <typename NUM>
  Division<NUM>::~Division()
  {
  }

  template <typename NUM>
  bool Division<NUM>::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  // TODO: warn on zero divisor?
  template <typename NUM>
  bool Division<NUM>::calc(NUM &result, Expression const *arg0, Expression const *arg1) const
  {
    NUM temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1))
        || temp1 == 0)
      return false;
    result = temp0 / temp1;
    return true;
  }

  //
  // Modulo
  //

  template <typename NUM>
  Modulo<NUM>::Modulo()
    : OperatorImpl<NUM>("MOD")
  {
  }

  template <typename NUM>
  Modulo<NUM>::~Modulo()
  {
  }

  template <typename NUM>
  bool Modulo<NUM>::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  // Integer implementation
  template <>
  bool Modulo<Integer>::calc(Integer &result, Expression const *arg0, Expression const *arg1) const
  {
    Integer temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1))
        || temp1 == 0)
      return false;
    result = temp0 % temp1;
    return true;
  }

  // Real implementation
  template <>
  bool Modulo<Real>::calc(Real &result, Expression const *arg0, Expression const *arg1) const
  {
    Real temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1))
        || temp1 == 0)
      return false;
    result = fmod(temp0, temp1);
    return true;
  }

  //
  // Minimum
  //

  template <typename NUM>
  Minimum<NUM>::Minimum()
    : OperatorImpl<NUM>("MIN")
  {
  }

  template <typename NUM>
  Minimum<NUM>::~Minimum()
  {
  }

  template <typename NUM>
  bool Minimum<NUM>::checkArgCount(size_t count) const
  {
    return count >= 1;
  }

  template <typename NUM>
  bool Minimum<NUM>::calc(NUM &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  template <typename NUM>
  bool Minimum<NUM>::calc(NUM &result, Expression const *arg0, Expression const *arg1) const
  {
    NUM temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1)))
      return false;
    result = (temp0 < temp1 ? temp0 : temp1);
    return true;
  }

  template <typename NUM>
  bool Minimum<NUM>::calc(NUM &result,
                          Function const &args) const
  {
    NUM workingResult;
    if (!args[0]->getValue(workingResult))
      return false;
    for (size_t i = 1; i < args.size(); ++i) {
      NUM temp;
      if (!args[i]->getValue(temp))
        return false; // unknown if any arg unknown
      if (temp < workingResult)
        workingResult = temp;
    }
    result = workingResult;
    return true;
  }

  //
  // Maximum
  //

  template <typename NUM>
  Maximum<NUM>::Maximum()
    : OperatorImpl<NUM>("MAX")
  {
  }

  template <typename NUM>
  Maximum<NUM>::~Maximum()
  {
  }

  template <typename NUM>
  bool Maximum<NUM>::checkArgCount(size_t count) const
  {
    return count >= 1;
  }

  template <typename NUM>
  bool Maximum<NUM>::calc(NUM &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  template <typename NUM>
  bool Maximum<NUM>::calc(NUM &result, Expression const *arg0, Expression const *arg1) const
  {
    NUM temp0, temp1;
    if (!(arg0->getValue(temp0) && arg1->getValue(temp1)))
      return false;
    result = (temp0 > temp1 ? temp0 : temp1);
    return true;
  }

  template <typename NUM>
  bool Maximum<NUM>::calc(NUM &result,
                          Function const &args) const
  {
    NUM workingResult;
    if (!args[0]->getValue(workingResult))
      return false;
    for (size_t i = 1; i < args.size(); ++i) {
      NUM temp;
      if (!args[i]->getValue(temp))
        return false; // unknown if any arg unknown
      if (temp > workingResult)
        workingResult = temp;
    }
    result = workingResult;
    return true;
  }

  //
  // AbsoluteValue
  //

  template <typename NUM>
  AbsoluteValue<NUM>::AbsoluteValue()
    : OperatorImpl<NUM>("ABS")
  {
  }

  template <typename NUM>
  bool AbsoluteValue<NUM>::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  template <typename NUM>
  AbsoluteValue<NUM>::~AbsoluteValue()
  {
  }

  // TODO: Unsigned numeric types need a simple passthrough method

  template <typename NUM>
  bool AbsoluteValue<NUM>::calc(NUM &result,
                                Expression const *arg) const
  {
    NUM temp;
    if (!arg->getValue(temp))
      return false;
    result = (temp < 0) ? -temp : temp;
    return true;
  }

  //
  // SquareRoot
  //

  template <typename NUM>
  SquareRoot<NUM>::SquareRoot()
    : OperatorImpl<NUM>("SQRT")
  {
  }

  template <typename NUM>
  SquareRoot<NUM>::~SquareRoot()
  {
  }

  template <typename NUM>
  bool SquareRoot<NUM>::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  template <typename NUM>
  bool SquareRoot<NUM>::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return isNumericType(ty) || ty == UNKNOWN_TYPE;
  }

  template <>
  bool SquareRoot<Real>::calc(Real &result,
                                Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp)
        || temp < 0) // imaginary result
      return false;
    result = sqrt(temp);
    return true;
  }

  //
  // Helper function for Real -> int conversions
  // Returns true if conversion successful,
  // false if x is out of range or not an integer.
  //
  static bool RealToInt(Real x, Integer &result)
  {
    Real tempInt;
    x = modf(x, &tempInt);
    // TODO: allow fraction to be +/- epsilon
    if (x != 0)
      return false; // not an integer
    if (tempInt < std::numeric_limits<Integer>::min()
        || tempInt > std::numeric_limits<Integer>::max())
      return false; // out of range
    result = (Integer) tempInt;
    return true;
  }

  //
  // Ceiling, Floor, Round, Truncate
  //

  template <typename NUM>
  Ceiling<NUM>::Ceiling()
    : OperatorImpl<NUM>("CEIL")
  {
  }

  template <typename NUM>
  Ceiling<NUM>::~Ceiling()
  {
  }

  template <typename NUM>
  bool Ceiling<NUM>::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  template <>
  bool Ceiling<Real>::calc(Real &result,
                             Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    result = ceil(temp);
    return true;
  }

  template <>
  bool Ceiling<Integer>::calc(Integer &result,
                              Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    return RealToInt(ceil(temp), result);
  }

  template <typename NUM>
  Floor<NUM>::Floor()
    : OperatorImpl<NUM>("FLOOR")
  {
  }

  template <typename NUM>
  Floor<NUM>::~Floor()
  {
  }

  template <typename NUM>
  bool Floor<NUM>::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  template <>
  bool Floor<Real>::calc(Real &result,
                           Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    result = floor(temp);
    return true;
  }

  template <>
  bool Floor<Integer>::calc(Integer &result,
                            Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    return RealToInt(floor(temp), result);
  }

  // Believe it or not, VxWorks 6.8 for PowerPC doesn't have round() or trunc()
#if !defined(__VXWORKS__)

  template <typename NUM>
  Round<NUM>::Round()
    : OperatorImpl<NUM>("ROUND")
  {
  }

  template <typename NUM>
  Round<NUM>::~Round()
  {
  }

  template <typename NUM>
  bool Round<NUM>::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  template <>
  bool Round<Real>::calc(Real &result,
                           Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    result = round(temp);
    return true;
  }

  template <>
  bool Round<Integer>::calc(Integer &result,
                            Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    return RealToInt(round(temp), result);
  }

  template <typename NUM>
  Truncate<NUM>::Truncate()
    : OperatorImpl<NUM>("TRUNC")
  {
  }

  template <typename NUM>
  Truncate<NUM>::~Truncate()
  {
  }

  template <typename NUM>
  bool Truncate<NUM>::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  template <>
  bool Truncate<Real>::calc(Real &result,
                              Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    result = trunc(temp);
    return true;
  }

  template <>
  bool Truncate<Integer>::calc(Integer &result,
                               Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false;
    return RealToInt(trunc(temp), result);
  }
#endif // !defined(__VXWORKS__)

  //
  // RealToInteger
  //

  RealToInteger::RealToInteger()
    : OperatorImpl<Integer>("REAL_TO_INT")
  {
  }

  RealToInteger::~RealToInteger()
  {
  }

  bool RealToInteger::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool RealToInteger::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return isNumericType(ty) || ty == UNKNOWN_TYPE;
  }

  bool RealToInteger::calc(Integer & result,
                           Expression const *arg) const
  {
    Real temp;
    if (!arg->getValue(temp))
      return false; // unknown/invalid
    return RealToInt(temp, result);
  }


  //
  // Explicit instantiations
  //
  template class Addition<Real>;
  template class Addition<Integer>;
  template class Subtraction<Real>;
  template class Subtraction<Integer>;
  template class Multiplication<Real>;
  template class Multiplication<Integer>;
  template class Division<Real>;
  template class Division<Integer>;
  template class Modulo<Integer>;
  template class Modulo<Real>;
  template class Minimum<Real>;
  template class Minimum<Integer>;
  template class Maximum<Real>;
  template class Maximum<Integer>;
  template class AbsoluteValue<Real>;
  template class AbsoluteValue<Integer>;
  // Only implemented for floating point types
  template class SquareRoot<Real>;
  template class Ceiling<Real>;
  template class Ceiling<Integer>;
  template class Floor<Real>;
  template class Floor<Integer>;
  // Believe it or not, VxWorks 6.8 for PowerPC doesn't have round() or trunc()
#if !defined(__VXWORKS__)
  template class Round<Real>;
  template class Round<Integer>;
  template class Truncate<Real>;
  template class Truncate<Integer>;
#endif // !defined(__VXWORKS__)

} // namespace PLEXIL

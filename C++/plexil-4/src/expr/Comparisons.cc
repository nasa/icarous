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

#include "Comparisons.hh"

#include "Array.hh"
#include "Function.hh"
#include "PlexilTypeTraits.hh"

namespace PLEXIL
{

  //
  // IsKnown
  //
  IsKnown::IsKnown()
    : OperatorImpl<Boolean>("IsKnown")
  {
  }

  IsKnown::~IsKnown()
  {
  }

  bool IsKnown::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool IsKnown::operator()(bool &result, Expression const *arg) const
  {
    result = arg->isKnown();
    return true; // result of active IsKnown is always known
  }

  //
  // Equal
  //

  // General (scalar) case
  template <typename T>
  static bool compareEqual(Boolean &result, Expression const *argA, Expression const *argB)
  {
    T tempA, tempB;
    if (!argA->getValue(tempA) || !argB->getValue(tempB))
      return false; // some value unknown
    result = (tempA == tempB);
    return true;
  }

  //
  // Special cases
  //

  template <>
  bool compareEqual<String>(Boolean &result, Expression const *argA, Expression const *argB)
  {
    String const *tempA, *tempB;
    if (!argA->getValuePointer(tempA) || !argB->getValuePointer(tempB))
      return false; // some value unknown
    result = (*tempA == *tempB);
    return true;
  }

  template <>
  bool compareEqual<Array>(Boolean &result, Expression const *argA, Expression const *argB)
  {
    Array const *tempA, *tempB;
    if (!argA->getValuePointer(tempA) || !argB->getValuePointer(tempB))
      return false; // some value unknown
    result = (*tempA == *tempB);
    return true;
  }

  static bool isEqual(Boolean &result, Expression const *argA, Expression const *argB)
  {
    switch (argA->valueType()) {
    case UNKNOWN_TYPE:
      return false; // unknown == any -> unknown

    case BOOLEAN_TYPE:
      return compareEqual<Boolean>(result, argA, argB);

    case INTEGER_TYPE:
      if (argB->valueType() == INTEGER_TYPE)
        return compareEqual<Integer>(result, argA, argB);
      // else fall through to Real case

    case REAL_TYPE:
      return compareEqual<Real>(result, argA, argB);

    case STRING_TYPE:
      return compareEqual<String>(result, argA, argB);

    case BOOLEAN_ARRAY_TYPE:
    case INTEGER_ARRAY_TYPE:
    case REAL_ARRAY_TYPE:
    case STRING_ARRAY_TYPE:
      return compareEqual<Array>(result, argA, argB);

    case NODE_STATE_TYPE:
    case OUTCOME_TYPE:
    case FAILURE_TYPE:
    case COMMAND_HANDLE_TYPE:
      if (argA->valueType() != argB->valueType()) {
        result = false;
        return true; // type mismatch
      }
      return compareEqual<uint16_t>(result, argA, argB);

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "isEqual: Invalid or unimplemented expression type " << argA->valueType());
      return false;
    }
  }

  Equal::Equal()
    : OperatorImpl<Boolean>("EQ")
  {
  }

  Equal::~Equal()
  {
  }

  bool Equal::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  // Called at plan load time, so some expressions (e.g. Lookups) may not know their own types
  bool Equal::checkArgTypes(Function const *ev) const
  {
    ValueType t0 = (*ev)[0]->valueType();
    if (t0 == UNKNOWN_TYPE)
      return true;
    ValueType t1 = (*ev)[1]->valueType();
    return (t1 == UNKNOWN_TYPE
            || t0 == t1
            || (isNumericType(t0) && isNumericType(t1)));
  }

  bool Equal::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    return isEqual(result, argA, argB);
  }

  //
  // NotEqual
  //

  NotEqual::NotEqual()
    : OperatorImpl<Boolean>("NEQ")
  {
  }

  NotEqual::~NotEqual()
  {
  }

  bool NotEqual::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  bool NotEqual::checkArgTypes(Function const *ev) const
  {
    ValueType t0 = (*ev)[0]->valueType();
    if (t0 == UNKNOWN_TYPE)
      return true;
    ValueType t1 = (*ev)[1]->valueType();
    return (t1 == UNKNOWN_TYPE
            || t0 == t1
            || (isNumericType(t0) && isNumericType(t1)));
  }

  bool NotEqual::operator()(Boolean &result, Expression const *argA, Expression const *argB) const
  {
    Boolean tempResult;
    bool returnVal = isEqual(tempResult, argA, argB);
    if (returnVal)
      result = !tempResult;
    return returnVal;
  }

  //
  // GreaterThan
  //

  template <typename T>
  GreaterThan<T>::GreaterThan()
    : OperatorImpl<Boolean>("GT")
  {
  }

  template <typename T>
  GreaterThan<T>::~GreaterThan()
  {
  }

  template <typename T>
  bool GreaterThan<T>::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  template <typename T>
  bool GreaterThan<T>::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(PlexilValueType<T>::value);
  }

  template <typename T>
  bool GreaterThan<T>::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    T tempA, tempB;
    if (!argA->getValue(tempA) || !argB->getValue(tempB))
      return false; // some value unknown
    result = (tempA > tempB);
    return true;
  }

  //
  // GreaterEqual
  //

  template <typename T>
  GreaterEqual<T>::GreaterEqual()
    : OperatorImpl<Boolean>("GEQ")
  {
  }

  template <typename T>
  GreaterEqual<T>::~GreaterEqual()
  {
  }

  template <typename T>
  bool GreaterEqual<T>::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  template <typename T>
  bool GreaterEqual<T>::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(PlexilValueType<T>::value);
  }

  template <typename T>
  bool GreaterEqual<T>::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    T tempA, tempB;
    if (!argA->getValue(tempA) || !argB->getValue(tempB))
      return false; // some value unknown
    result = (tempA >= tempB);
    return true;
  }

  //
  // LessThan
  //

  template <typename T>
  LessThan<T>::LessThan()
    : OperatorImpl<Boolean>("LT")
  {
  }

  template <typename T>
  LessThan<T>::~LessThan()
  {
  }

  template <typename T>
  bool LessThan<T>::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  template <typename T>
  bool LessThan<T>::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(PlexilValueType<T>::value);
  }

  template <typename T>
  bool LessThan<T>::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    T tempA, tempB;
    if (!argA->getValue(tempA) || !argB->getValue(tempB))
      return false; // some value unknown
    result = (tempA < tempB);
    return true;
  }

  //
  // LessEqual
  //

  template <typename T>
  LessEqual<T>::LessEqual()
    : OperatorImpl<Boolean>("LEQ")
  {
  }

  template <typename T>
  LessEqual<T>::~LessEqual()
  {
  }

  template <typename T>
  bool LessEqual<T>::checkArgCount(size_t count) const
  {
    return count == 2;
  }

  template <typename T>
  bool LessEqual<T>::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(PlexilValueType<T>::value);
  }

  template <typename T>
  bool LessEqual<T>::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    T tempA, tempB;
    if (!argA->getValue(tempA) || !argB->getValue(tempB))
      return false; // some value unknown
    result = (tempA <= tempB);
    return true;
  }

  //
  // Explicit instantiations of template classes
  //

  // Comparisons below don't make sense for Booleans

  template class GreaterThan<Integer>;
  template class GreaterThan<Real>;
  template class GreaterThan<String>;

  template class GreaterEqual<Integer>;
  template class GreaterEqual<Real>;
  template class GreaterEqual<String>;

  template class LessThan<Integer>;
  template class LessThan<Real>;
  template class LessThan<String>;

  template class LessEqual<Integer>;
  template class LessEqual<Real>;
  template class LessEqual<String>;

}

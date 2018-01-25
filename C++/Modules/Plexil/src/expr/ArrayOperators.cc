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

#include "ArrayOperators.hh"

#include "Array.hh"
#include "Function.hh"

namespace PLEXIL
{
  //
  // ArraySize
  //

  ArraySize::ArraySize()
    : OperatorImpl<int32_t>("ArraySize")
  {
  }
  
  ArraySize::~ArraySize()
  {
  }

  bool ArraySize::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool ArraySize::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return isArrayType(ty) || ty == UNKNOWN_TYPE;
  }

  bool ArraySize::operator()(int32_t &result, Expression const *arg) const
  {
    Array const *ary;
    if (!arg->getValuePointer(ary))
      return false;
    result = ary->size();
    return true;
  }

  //
  // ArrayMaxSize
  //

  ArrayMaxSize::ArrayMaxSize()
    : OperatorImpl<int32_t>("ArrayMaxSize")
  {
  }
  
  ArrayMaxSize::~ArrayMaxSize()
  {
  }

  bool ArrayMaxSize::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool ArrayMaxSize::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return isArrayType(ty) || ty == UNKNOWN_TYPE;
  }

  bool ArrayMaxSize::operator()(int32_t &result, Expression const *arg) const
  {
    // *** TODO: Reimplement as Expression metadata query, not expression value query ***
    Array const *ary;
    if (!arg->getValuePointer(ary))
      return false;
    result = ary->size();
    return true;
  }

  //
  // AllElementsKnown
  //

  AllElementsKnown::AllElementsKnown()
    : OperatorImpl<bool>("ALL_KNOWN")
  {
  }

  AllElementsKnown::~AllElementsKnown()
  {
  }

  bool AllElementsKnown::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool AllElementsKnown::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return isArrayType(ty) || ty == UNKNOWN_TYPE;
  }

  bool AllElementsKnown::operator()(bool &result, Expression const *arg) const
  {
    Array const *ary;
    if (!arg->getValuePointer(ary))
      return false;
    result = ary->allElementsKnown();
    return true;
  }

  //
  // AnyElementsKnown
  //

  AnyElementsKnown::AnyElementsKnown()
    : OperatorImpl<bool>("ANY_KNOWN")
  {
  }

  AnyElementsKnown::~AnyElementsKnown()
  {
  }

  bool AnyElementsKnown::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool AnyElementsKnown::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return isArrayType(ty) || ty == UNKNOWN_TYPE;
  }

  bool AnyElementsKnown::operator()(bool &result, Expression const *arg) const
  {
    Array const *ary;
    if (!arg->getValuePointer(ary))
      return false;
    result = ary->anyElementsKnown();
    return true;
  }

} // namespace PLEXIL

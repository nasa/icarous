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

#include "BooleanOperators.hh"

#include "Function.hh"

namespace PLEXIL
{
  BooleanNot::BooleanNot()
    : OperatorImpl<bool>("NOT")
  {
  }

  BooleanNot::~BooleanNot()
  {
  }

  bool BooleanNot::checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool BooleanNot::checkArgTypes(Function const *ev) const
  {
    ValueType ty = (*ev)[0]->valueType();
    return ty == BOOLEAN_TYPE || ty == UNKNOWN_TYPE;
  }

  bool BooleanNot::operator()(bool &result, Expression const *arg) const
  {
    bool temp;
    if (!arg->getValue(temp))
      return false;
    result = !temp;
    return true;
  }

  BooleanOr::BooleanOr()
    : OperatorImpl<bool>("OR")
  {
  }

  BooleanOr::~BooleanOr()
  {
  }

  bool BooleanOr::checkArgCount(size_t count) const
  {
    return count > 0;
  }

  bool BooleanOr::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(BOOLEAN_TYPE);
  }

  bool BooleanOr::operator()(bool &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  bool BooleanOr::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    bool temp;
    if (argA->getValue(temp)) {
      if (temp) {
        result = true;
        return true;
      }
      bool known = argB->getValue(temp);
      // A known but false
      if (known)
        result = temp;
      return known;
    }
    // A unknown
    if (argB->getValue(temp) && temp) {
      result = temp;
      return true;
    }
    return false;
  }

  bool BooleanOr::operator()(bool &result, Function const &args) const
  {
    size_t const n = args.size();
    bool anyKnown = false;
    for (size_t i = 0; i < n; ++i) {
      bool temp;
      if (args[i]->getValue(temp)) {
        // Return if any arg is known and true
        if (temp) {
          result = true;
          return true;
        }
        anyKnown = true; // but no true value yet
      }
    }
    if (anyKnown)
      result = false; // or we would have returned above
    return anyKnown;
  }

  //
  // AND
  //

  BooleanAnd::BooleanAnd()
    : OperatorImpl<bool>("AND")
  {
  }

  BooleanAnd::~BooleanAnd()
  {
  }

  bool BooleanAnd::checkArgCount(size_t count) const
  {
    return count > 0;
  }

  bool BooleanAnd::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(BOOLEAN_TYPE);
  }

  bool BooleanAnd::operator()(bool &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  bool BooleanAnd::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    bool temp;
    if (argA->getValue(temp)) {
      // A known
      if (!temp) {
        result = false; // cannot be true
        return true;
      }
      // A known and true
      bool known = argB->getValue(temp);
      if (known)
        result = temp;
      return known;
    }
    // A unknown
    if (argB->getValue(temp) && !temp) {
      result = false; // cannot be true
      return true;
    }
    return false; // cannot be known
  }

  bool BooleanAnd::operator()(bool &result, Function const &args) const
  {
    size_t const n = args.size();
    bool allKnown = true;
    for (size_t i = 0; i < n; ++i) {
      bool temp;
      if (args[i]->getValue(temp)) {
        if (!temp) {
          // Any known and false -> result known and false
          result = false;
          return true;
        }
      }
      else
        allKnown = false;
    }
    if (allKnown)
      result = true;
    return allKnown;
  }

  BooleanXor::BooleanXor()
    : OperatorImpl<bool>("XOR")
  {
  }

  BooleanXor::~BooleanXor()
  {
  }

  bool BooleanXor::checkArgCount(size_t count) const
  {
    return count > 0;
  }

  bool BooleanXor::checkArgTypes(Function const *ev) const
  {
    return ev->allSameTypeOrUnknown(BOOLEAN_TYPE);
  }

  bool BooleanXor::operator()(bool &result, Expression const *arg) const
  {
    return arg->getValue(result);
  }

  bool BooleanXor::operator()(bool &result, Expression const *argA, Expression const *argB) const
  {
    bool temp1, temp2;
    if (!argA->getValue(temp1))
      return false;
    if (!argB->getValue(temp2))
      return false;
    result = (temp1 != temp2);
    return true;
  }

  bool BooleanXor::operator()(bool &result, Function const &args) const
  {
    size_t const n = args.size();
    bool temp1 = false;
    for (size_t i = 0; i < n; ++i) {
      bool temp2;
      // Return unknown if any arg is unknown
      if (args[i]->getValue(temp2))
        return false;
      temp1 = (temp1 != temp2);
    }
    // Can only get here if all known
    result = temp1;
    return true;
  }

} // namespace PLEXIL


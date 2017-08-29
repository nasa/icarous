/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#include "ExpressionConstants.hh"
#include "Constant.hh"

namespace PLEXIL
{
  Expression *FALSE_EXP()
  {
    static BooleanConstant sl_false(false);
    return &sl_false;
  }

  Expression *TRUE_EXP()
  {
    static BooleanConstant sl_true(true);
    return &sl_true;
  }

  Expression *UNKNOWN_BOOLEAN_EXP()
  {
    static BooleanConstant sl_unknown;
    return &sl_unknown;
  }

  Expression *INT_ONE_EXP()
  {
    static IntegerConstant sl_int_one(1);
    return &sl_int_one;
  }

  Expression *INT_ZERO_EXP()
  {
    static IntegerConstant sl_int_zero((int32_t) 0);
    return &sl_int_zero;
  }

  Expression *INT_MINUS_ONE_EXP()
  {
    static IntegerConstant sl_int_minus_one(-1);
    return &sl_int_minus_one;
  }

} // namespace PLEXIL


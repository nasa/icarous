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

//
// Define default operator() methods
//

#include "NodeOperator.hh"

#include "Error.hh"

namespace PLEXIL
{

#define DEFINE_NODE_OPERATOR_DEFAULT_METHOD(_rtype_) \
  bool NodeOperator::operator()(_rtype_ & /* result */, Node const * /* node */) const \
  { assertTrueMsg(ALWAYS_FAIL, "No method defined for _rtype_"); return false; }

  DEFINE_NODE_OPERATOR_DEFAULT_METHOD(Boolean)
  DEFINE_NODE_OPERATOR_DEFAULT_METHOD(Integer)
  DEFINE_NODE_OPERATOR_DEFAULT_METHOD(Real)
  DEFINE_NODE_OPERATOR_DEFAULT_METHOD(String)

  // Not needed yet
  // DEFINE_NODE_OPERATOR_DEFAULT_METHOD(Array)
  // DEFINE_NODE_OPERATOR_DEFAULT_METHOD(BooleanArray)
  // DEFINE_NODE_OPERATOR_DEFAULT_METHOD(IntegerArray)
  // DEFINE_NODE_OPERATOR_DEFAULT_METHOD(RealArray)
  // DEFINE_NODE_OPERATOR_DEFAULT_METHOD(StringArray)

  DEFINE_NODE_OPERATOR_DEFAULT_METHOD(uint16_t)
  
#undef DEFINE_NODE_OPERATOR_DEFAULT_METHOD

}

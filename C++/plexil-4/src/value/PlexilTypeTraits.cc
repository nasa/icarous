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

#include "PlexilTypeTraits.hh"

namespace PLEXIL
{

  char const * const PlexilValueType<Boolean>::typeName = BOOLEAN_STR;
  char const * const PlexilValueType<Integer>::typeName = INTEGER_STR;
  char const * const PlexilValueType<Real>::typeName = REAL_STR;
  char const * const PlexilValueType<String>::typeName = STRING_STR;
  char const * const PlexilValueType<NodeState>::typeName = NODE_STATE_STR;
  char const * const PlexilValueType<NodeOutcome>::typeName = NODE_OUTCOME_STR;
  char const * const PlexilValueType<FailureType>::typeName = NODE_FAILURE_STR;
  char const * const PlexilValueType<CommandHandleValue>::typeName = NODE_COMMAND_HANDLE_STR;

  char const * const PlexilValueType<Array>::typeName = ARRAY_STR;
  char const * const PlexilValueType<BooleanArray>::typeName = BOOLEAN_ARRAY_STR;
  char const * const PlexilValueType<IntegerArray>::typeName = INTEGER_ARRAY_STR;
  char const * const PlexilValueType<RealArray>::typeName = REAL_ARRAY_STR;
  char const * const PlexilValueType<StringArray>::typeName = STRING_ARRAY_STR;

}

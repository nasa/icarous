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

#ifndef PLEXIL_TYPE_TRAITS_HH
#define PLEXIL_TYPE_TRAITS_HH

#include "CommandHandle.hh"
#include "NodeConstants.hh"
#include "ValueType.hh"


namespace PLEXIL
{

  template <typename T>
  struct PlexilValueType
  {
    static const ValueType value = UNKNOWN_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = UNKNOWN_STR;
    static const bool isPlexilType = false;
  };

  template <typename T>
  char const * const PlexilValueType<T>::typeName = "UNKNOWN";

  template <>
  struct PlexilValueType<Boolean>
  {
    static const ValueType value = BOOLEAN_TYPE;
    static const ValueType arrayValue = BOOLEAN_ARRAY_TYPE;
    static char const * const typeName; // = BOOLEAN_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<NodeState>
  {
    static const ValueType value = NODE_STATE_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = NODE_STATE_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<NodeOutcome>
  {
    static const ValueType value = OUTCOME_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = NODE_OUTCOME_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<FailureType>
  {
    static const ValueType value = FAILURE_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = NODE_FAILURE_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<CommandHandleValue>
  {
    static const ValueType value = COMMAND_HANDLE_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = NODE_COMMAND_HANDLE_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<Integer>
  {
    static const ValueType value = INTEGER_TYPE;
    static const ValueType arrayValue = INTEGER_ARRAY_TYPE;
    static char const * const typeName; // = INTEGER_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<Real>
  {
    static const ValueType value = REAL_TYPE;
    static const ValueType arrayValue = REAL_ARRAY_TYPE;
    static char const * const typeName; // = REAL_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<String>
  {
    static const ValueType value = STRING_TYPE;
    static const ValueType arrayValue = STRING_ARRAY_TYPE;
    static char const * const typeName; // = STRING_STR;
    static const bool isPlexilType = true;
  };

  // *** Not sure about this ***
  template <>
  struct PlexilValueType<Array>
  {
    static const ValueType value = ARRAY_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = ARRAY_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<BooleanArray>
  {
    static const ValueType value = BOOLEAN_ARRAY_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = BOOLEAN_ARRAY_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<IntegerArray>
  {
    static const ValueType value = INTEGER_ARRAY_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = INTEGER_ARRAY_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<RealArray>
  {
    static const ValueType value = REAL_ARRAY_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = REAL_ARRAY_STR;
    static const bool isPlexilType = true;
  };

  template <>
  struct PlexilValueType<StringArray>
  {
    static const ValueType value = STRING_ARRAY_TYPE;
    static const ValueType arrayValue = UNKNOWN_TYPE;
    static char const * const typeName; // = STRING_ARRAY_STR;
    static const bool isPlexilType = true;
  };

} // namespace PLEXIL

#endif // PLEXIL_TYPE_TRAITS_HH

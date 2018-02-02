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

#include "NodeConstants.hh"

namespace PLEXIL
{

  /**
   * @brief Table of state names.
   * @note Must be in same order as NodeState enum.
   */
  std::string const ALL_STATE_NAMES[] =
    {"NO_STATE",
     "INACTIVE",
     "WAITING",
     "EXECUTING",
     "ITERATION_ENDED",
     "FINISHED",
     "FAILING",
     "FINISHING"
    };

  /**
   * @brief Table of outcome names.
   * @note Must be in same order as NodeOutcome enum.
   */
  std::string const ALL_OUTCOME_NAMES[] =
    {"NO_OUTCOME",
     "SUCCESS",
     "FAILURE",
     "SKIPPED",
     "INTERRUPTED"
    };

  /**
   * @brief Table of failure type names.
   * @note Must be in same order as FailureType enum.
   */
  std::string const ALL_FAILURE_NAMES[] =
    {"NO_FAILURE",
     "PRE_CONDITION_FAILED",
     "POST_CONDITION_FAILED",
     "INVARIANT_CONDITION_FAILED",
     "PARENT_FAILED",
     "EXITED",
     "PARENT_EXITED"
    };

  // Simple linear search
  NodeState parseNodeState(std::string const &name)
  {
    return parseNodeState(name.c_str());
  }

  NodeState parseNodeState(char const *name)
  {
    for (size_t i = INACTIVE_STATE; i < NODE_STATE_MAX; ++i)
      if (ALL_STATE_NAMES[i] == name)
        return (NodeState) i;
    return NO_NODE_STATE;
  }

  std::string const &nodeStateName(unsigned int s)
  {
    if (s >= NODE_STATE_MAX)
      return ALL_STATE_NAMES[NO_NODE_STATE];
    return ALL_STATE_NAMES[s];
  }

  bool isNodeStateValid(unsigned int val)
  {
    return val >= INACTIVE_STATE && val < NODE_STATE_MAX;
  }
  
  NodeOutcome parseNodeOutcome(std::string const &name)
  {
    return parseNodeOutcome(name.c_str());
  }

  NodeOutcome parseNodeOutcome(char const *name)
  {
    for (size_t i = SUCCESS_OUTCOME; i < OUTCOME_MAX; ++i)
      if (ALL_OUTCOME_NAMES[i - NO_OUTCOME] == name)
        return (NodeOutcome) i;
    return NO_OUTCOME;
  }

  std::string const &outcomeName(unsigned int o)
  {
    if (o <= NO_OUTCOME || o >= OUTCOME_MAX)
      return ALL_OUTCOME_NAMES[0];
    return ALL_OUTCOME_NAMES[o - NO_OUTCOME];
  }

  bool isNodeOutcomeValid(unsigned int val)
  {
    return val > NO_OUTCOME && val < OUTCOME_MAX;
  }

  FailureType parseFailureType(std::string const &name)
  {
    return parseFailureType(name.c_str());
  }

  FailureType parseFailureType(char const *name)
  {
    for (size_t i = PRE_CONDITION_FAILED; i < FAILURE_TYPE_MAX; ++i)
      if (ALL_FAILURE_NAMES[i - NO_FAILURE] == name)
        return (FailureType) i;
    return NO_FAILURE;
  }

  std::string const &failureTypeName(unsigned int f)
  {
    if (f <= NO_FAILURE || f >= FAILURE_TYPE_MAX)
      return ALL_FAILURE_NAMES[0];
    return ALL_FAILURE_NAMES[f - NO_FAILURE];
  }

  bool isFailureTypeValid(unsigned int val)
  {
    return val > NO_FAILURE && val < FAILURE_TYPE_MAX;
  }

} // namespace PLEXIL

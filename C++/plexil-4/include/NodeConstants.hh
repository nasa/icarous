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

#ifndef PLEXIL_NODE_CONSTANTS_HH
#define PLEXIL_NODE_CONSTANTS_HH

//
// Place to define all constants used in nodes
//

#include <string>

namespace PLEXIL {

  //
  // Node state
  //

  /**
   * @brief Node state enumeration.
   */
  enum NodeState {
    NO_NODE_STATE = 0,
    INACTIVE_STATE,
    WAITING_STATE,
    EXECUTING_STATE,
    ITERATION_ENDED_STATE,
    FINISHED_STATE,
    FAILING_STATE,           // All but empty nodes
    FINISHING_STATE,         // Command, List/LibraryCall only
    NODE_STATE_MAX
  };

  /**
   * @brief Table of state names.
   * @note Must be in same order as NodeState enum above.
   */
  extern std::string const ALL_STATE_NAMES[];

  /**
   * @brief Parse the given string as a node state name.
   * @param The name.
   * @return The state, or NO_NODE_STATE if not found.
   */
  extern NodeState parseNodeState(std::string const &name);
  extern NodeState parseNodeState(char const *name);

  /**
   * @brief Get the name of this state.
   * @return Const reference to one of the state names.
   */
  extern std::string const &nodeStateName(unsigned int s);

  /**
   * @brief Test whether the value is a valid NodeState value.
   * @param val The value to check.
   * @return True if valid, false otherwise.
   */
  extern bool isNodeStateValid(unsigned int val);

  /**
   * @brief Outcome enumeration.
   */
  enum NodeOutcome {
    NO_OUTCOME = 16,
    SUCCESS_OUTCOME,
    FAILURE_OUTCOME,
    SKIPPED_OUTCOME,
    INTERRUPTED_OUTCOME,
    OUTCOME_MAX
  };

  /**
   * @brief Table of outcome names.
   * @note Must be in same order as NodeOutcome enum above.
   */
  extern std::string const ALL_OUTCOME_NAMES[];

  /**
   * @brief Parse the given string as a node outcome name.
   * @param The name.
   * @return The outcome value, or NO_OUTCOME if not found.
   */
  extern NodeOutcome parseNodeOutcome(std::string const &name);
  extern NodeOutcome parseNodeOutcome(char const *name);

  /**
   * @brief Get the name of this outcome.
   * @return Const reference to one of the outcome names.
   */
  extern std::string const &outcomeName(unsigned int o);

  /**
   * @brief Test whether the value is a valid NodeOutcome value.
   * @param val The value to check.
   * @return True if valid, false otherwise.
   */
  extern bool isNodeOutcomeValid(unsigned int val);

  /**
   * @brief Node failure type enumeration.
   */
  enum FailureType {
    NO_FAILURE = 32,
    PRE_CONDITION_FAILED,
    POST_CONDITION_FAILED,
    INVARIANT_CONDITION_FAILED,
    PARENT_FAILED,
    EXITED,
    PARENT_EXITED,
    FAILURE_TYPE_MAX
  };

  /**
   * @brief Table of failure type names.
   * @note Must be in same order as FailureType enum above.
   */
  extern std::string const ALL_FAILURE_NAMES[];

  /**
   * @brief Parse the given string as a failure type name.
   * @param The name.
   * @return The FailureType value, or NO_FAILURE if not found.
   */
  extern FailureType parseFailureType(std::string const &name);
  extern FailureType parseFailureType(char const *name);

  /**
   * @brief Get the name of this failure type.
   * @return Pointer to one of the names.
   */
  extern std::string const &failureTypeName(unsigned int f);

  /**
   * @brief Test whether the value is a valid FailureType value.
   * @param val The value to check.
   * @return True if valid, false otherwise.
   */
  extern bool isFailureTypeValid(unsigned int val);

} // namespace PLEXIL

#endif // PLEXIL_NODE_CONSTANTS_HH

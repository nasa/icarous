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

#include "ExecListenerFilter.hh"

#include "Error.hh" // warn() macro
#include "ExecListenerFilterFactory.hh"
#include "InterfaceSchema.hh"
#include "Node.hh"
#include "NodeConstants.hh"

#define STATES_TAG "States"
#define IGNORED_STATES_TAG "IgnoredStates"

//
// Library of standard ExecListenerFilter classes
//

namespace PLEXIL
{

  /**
   * @class NodeStateFilter
   * @brief Determines whether to publish a node transition event based on previous or next state.
   */

  class NodeStateFilter : public ExecListenerFilter
  {
  public:

    NodeStateFilter(pugi::xml_node const xml)
      : ExecListenerFilter(xml)
    {
    }
  
    ~NodeStateFilter()
    {
    }

    bool initialize()
    {
      bool hasStates = false;
      const char* states = this->getXml().child_value(STATES_TAG);
      if (*states) {
        hasStates = true;
        // Default is don't report
        for (size_t i = 0; i < NODE_STATE_MAX; ++i)
          m_stateEnabled[i] = false;
        std::vector<std::string>* stateNames = InterfaceSchema::parseCommaSeparatedArgs(states);
        for (std::vector<std::string>::const_iterator it = stateNames->begin();
             it != stateNames->end();
             ++it)
          m_stateEnabled[parseNodeState(*it)] = true;
        delete stateNames;
      }
      states = this->getXml().child_value(IGNORED_STATES_TAG);
      if (!*states) {
        if (!hasStates) {
          warn("NodeStateFilter: neither <States> nor <IgnoredStates> provided; all transitions will be reported");
          for (size_t i = 0; i < NODE_STATE_MAX; ++i)
            m_stateEnabled[i] = true;
        }
      }
      else {
        if (hasStates) {
          warn("NodeStateFilter configuration error: both <States> and <IgnoredStates> provided");
          return false;
        }
        // Default is report
        for (size_t i = 0; i < NODE_STATE_MAX; ++i)
          m_stateEnabled[i] = true;
        std::vector<std::string>* stateNames = InterfaceSchema::parseCommaSeparatedArgs(states);
        for (std::vector<std::string>::const_iterator it = stateNames->begin();
             it != stateNames->end();
             ++it)
          m_stateEnabled[parseNodeState(*it)] = false;
        delete stateNames;
      }
      return true;
    }

    // Return true if either the previous or new state is in the filter.
    bool reportNodeTransition(NodeState prevState, Node *node)
    {
      return m_stateEnabled[prevState] || m_stateEnabled[node->getState()];
    }

  private:

    //
    // Deliberately unimplemented
    //
    NodeStateFilter();
    NodeStateFilter(const NodeStateFilter&);
    NodeStateFilter& operator=(const NodeStateFilter&);

    bool m_stateEnabled[NODE_STATE_MAX];
  };

  //
  // More later
  //

  /**
   * @brief Register the standard exec listener filters.
   */
  void registerExecListenerFilters()
  {
    REGISTER_EXEC_LISTENER_FILTER(NodeStateFilter, "NodeState")
  }

}

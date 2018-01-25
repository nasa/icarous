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

#include "InterfaceAdapter.hh"
#include "AdapterConfiguration.hh"
#include "AdapterExecInterface.hh"
#include "Command.hh"
#include "Update.hh"
#include "Debug.hh"
#include "StateCacheEntry.hh"

namespace PLEXIL
{

  //
  // Constructors
  //

  InterfaceAdapter::InterfaceAdapter(AdapterExecInterface& execInterface)
    : m_execInterface(execInterface),
      m_xml()
  {
  }

  InterfaceAdapter::InterfaceAdapter(AdapterExecInterface& execInterface, 
                                     pugi::xml_node const xml)
    : m_execInterface(execInterface),
      m_xml(xml)
  {
  }

  //
  // Destructor
  //

  InterfaceAdapter::~InterfaceAdapter()
  {
  }


  //
  // Default methods for InterfaceManager API
  // All are no-ops
  //

  void InterfaceAdapter::lookupNow(State const & state, StateCacheEntry &cacheEntry)
  {
    debugMsg("InterfaceAdapter:lookupNow", " default method called for state " << state);
  }

  void InterfaceAdapter::subscribe(const State& state)
  {
    debugMsg("InterfaceAdapter:subscribe", " default method called for state " << state);
  }

  void InterfaceAdapter::unsubscribe(const State& state)
  {
    debugMsg("InterfaceAdapter:unsubscribe", " default method called for state " << state);
  }

  void InterfaceAdapter::setThresholds(const State& state, double /* hi */, double /* lo */)
  {
    debugMsg("InterfaceAdapter:setThresholds", " default method called for state " << state);
  }

  void InterfaceAdapter::setThresholds(const State& state, int32_t /* hi */, int32_t /* lo */)
  {
    debugMsg("InterfaceAdapter:setThresholds", " default method called");
  }

  void InterfaceAdapter::sendPlannerUpdate(Update * /* upd */)
  {
    debugMsg("InterfaceAdapter:sendPlannerUpdate", " default method called");
  }

  void InterfaceAdapter::executeCommand(Command * /* cmd */)
  {
    debugMsg("InterfaceAdapter:executeCommand", " default method called");
  }

  void InterfaceAdapter::invokeAbort(Command * /* cmd */)
  {
    debugMsg("InterfaceAdapter:invokeAbort", " default method called");
  }

  /**
   * @brief Register this adapter based on its XML configuration data.
   * @note The adapter is presumed to be fully initialized and working at the time of this call.
   * @note This is a default method; adapters are free to override it.
   */
  void InterfaceAdapter::registerAdapter()
  {
    g_configuration->defaultRegisterAdapter(this);
  }

}

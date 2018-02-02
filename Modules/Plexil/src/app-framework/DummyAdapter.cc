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

#include "DummyAdapter.hh"

#include "AdapterExecInterface.hh"
#include "Command.hh"
#include "Debug.hh"
#include "StateCacheEntry.hh"
#include "Update.hh"

namespace PLEXIL
{
  /**
   * @brief Base constructor.
   */
  DummyAdapter::DummyAdapter(AdapterExecInterface& execInterface)
    : InterfaceAdapter(execInterface)
  {
  }

  /**
   * @brief Constructor w/ configuration XML.
   */
  DummyAdapter::DummyAdapter(AdapterExecInterface& execInterface,
                             pugi::xml_node const xml)
    : InterfaceAdapter(execInterface, xml)
  {
  }

  /**
   * @brief Destructor.
   */
  DummyAdapter::~DummyAdapter()
  {
  }

  /**
   * @brief Initialize and register the dummy adapter.
   */
  bool DummyAdapter::initialize()
  {
    this->registerAdapter();
    return true;
  }

  /**
   * @brief Starts the adaptor, possibly using its configuration data.  
   * @return true if successful, false otherwise.
   */
  bool DummyAdapter::start()
  {
    return true;
  }

  /**
   * @brief Stops the adaptor.  
   * @return true if successful, false otherwise.
   */
  bool DummyAdapter::stop()
  {
    return true;
  }

  /**
   * @brief Resets the adaptor.  
   * @return true if successful, false otherwise.
   */
  bool DummyAdapter::reset()
  {
    return true;
  }

  /**
   * @brief Shuts down the adaptor, releasing any of its resources.
   * @return true if successful, false otherwise.
   */
  bool DummyAdapter::shutdown()
  {
    return true;
  }


  void DummyAdapter::lookupNow(State const &state, StateCacheEntry &cacheEntry)
  {
    debugMsg("ExternalInterface:dummy", 
             " LookupNow of " << state << " returning UNKNOWN");
    cacheEntry.setUnknown();
  }

  void DummyAdapter::subscribe(const State& state)
  {
    debugMsg("ExternalInterface:dummy",
             " subscribe of " << state << " called, ignoring");
  }

  void DummyAdapter::unsubscribe(const State& state)
  {
    debugMsg("ExternalInterface:dummy",
             " unsubscribe of " << state << " called, ignoring");
  }

  void DummyAdapter::setThresholds(const State& state, double /* hi */, double /* lo */)
  {
    debugMsg("ExternalInterface:dummy",
             " setThresholds of " << state << " called, ignoring");
  }

  void DummyAdapter::setThresholds(const State& state, int32_t /* hi */, int32_t /* lo */)
  {
    debugMsg("ExternalInterface:dummy",
             " setThresholds of " << state << " called, ignoring");
  }

  void DummyAdapter::sendPlannerUpdate(Update *upd)
  {
    debugMsg("ExternalInterface:dummy", " sendPlannerUpdate called");

    // acknowledge updates
    debugMsg("ExternalInterface:dummy",
             " faking acknowledgment of update");
    m_execInterface.handleUpdateAck(upd, true);
    m_execInterface.notifyOfExternalEvent();
  }

  void DummyAdapter::executeCommand(Command *cmd)
  {
    debugMsg("ExternalInterface:dummy", " executeCommand for " << cmd->getName());
    m_execInterface.handleCommandAck(cmd, COMMAND_SENT_TO_SYSTEM);
    m_execInterface.notifyOfExternalEvent();
  }

  //abort the given command
  void DummyAdapter::invokeAbort(Command *cmd)
  {
    debugMsg("ExternalInterface:dummy", " invokeAbort for " << cmd->getName());
    m_execInterface.handleCommandAbortAck(cmd, true);
    m_execInterface.notifyOfExternalEvent();
  }

}

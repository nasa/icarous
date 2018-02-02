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

#include "UtilityAdapter.hh"

#include "AdapterConfiguration.hh"
#include "AdapterFactory.hh"
#include "AdapterExecInterface.hh"
#include "Command.hh"
#include "Debug.hh"
#include "plan-utils.hh"

#include <iostream>

namespace PLEXIL {

UtilityAdapter::UtilityAdapter(AdapterExecInterface& execInterface,
                               pugi::xml_node const configXml) :
    InterfaceAdapter(execInterface, configXml)
{
  debugMsg("UtilityAdapter", " created.");
}

bool UtilityAdapter::initialize()
{
  g_configuration->registerCommandInterface("print", this);
  g_configuration->registerCommandInterface("pprint", this);
  debugMsg("UtilityAdapter", " initialized.");
  return true;
}

bool UtilityAdapter::start()
{
  debugMsg("UtilityAdapter", " started.");
  return true;
}

bool UtilityAdapter::stop()
{
  debugMsg("UtilityAdapter", " stopped.");
  return true;
}

bool UtilityAdapter::reset()
{
  debugMsg("UtilityAdapter", " reset.");
  return true;
}

bool UtilityAdapter::shutdown()
{
  debugMsg("UtilityAdapter", " shut down.");
  return true;
}

void UtilityAdapter::executeCommand(Command * cmd) 
{
  const std::string& name = cmd->getName();
  debugMsg("UtilityAdapter", "Received executeCommand for " << name);  

  if (name == "print") 
    print(cmd->getArgValues());
  else if (name == "pprint") 
    pprint(cmd->getArgValues());
  else
    std::cerr <<
      "Error in Utility Adapter: invalid command (should never happen!): "
              << name << std::endl;

  m_execInterface.handleCommandAck(cmd, COMMAND_SUCCESS);
  m_execInterface.notifyOfExternalEvent();
}

void UtilityAdapter::invokeAbort(Command *cmd)
{
  const std::string& name = cmd->getName();
  if (name != "print" && name != "pprint") {
    std::cerr << "Error in Utility Adapter: aborting invalid command \"" 
              << name << "\" (should never happen!)" << std::endl;
  }
  m_execInterface.handleCommandAbortAck(cmd, true);
  m_execInterface.notifyOfExternalEvent();
}

extern "C" {
  void initUtilityAdapter() {
    REGISTER_ADAPTER(UtilityAdapter, "UtilityAdapter");
  }
}

} // namespace PLEXIL

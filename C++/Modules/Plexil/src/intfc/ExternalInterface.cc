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

#include "ExternalInterface.hh"

#include "Command.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ResourceArbiterInterface.hh"
#include "StateCacheEntry.hh"
#include "StateCacheMap.hh"
#include "Update.hh"

namespace PLEXIL
{
  // Define global variable
  ExternalInterface *g_interface = NULL;

  ExternalInterface::ExternalInterface()
    : m_updatesToExecute(),
      m_commandsToExecute(),
      m_raInterface(makeResourceArbiter()),
      m_cycleCount(1)
  {
  }

  ExternalInterface::~ExternalInterface()
  {
    delete m_raInterface;
  }
    
  /**
   * @brief Read command resource hierarchy from the named file.
   * @param fname File name.
   * @return True if successful, false otherwise.
   */

  bool ExternalInterface::readResourceFile(std::string const &fname)
  {
    return m_raInterface->readResourceHierarchyFile(fname);
  }

  /**
   * @brief Return the number of "macro steps" since this instance was constructed.
   * @return The macro step count.
   */
  unsigned int ExternalInterface::getCycleCount() const 
  {
    return m_cycleCount;
  }

  /**
   * @brief Increment the macro step count and return the new value.
   * @return The updated macro step count.
   */
  unsigned int ExternalInterface::incrementCycleCount()
  {
    return ++m_cycleCount;
  }

  /**
   * @brief Schedule this command for execution.
   */
  void ExternalInterface::enqueueCommand(Command *cmd)
  {
    m_commandsToExecute.push(cmd);
  }

  /**
   * @brief Abort the pending command.
   */
  void ExternalInterface::abortCommand(Command *cmd)
  {
    this->invokeAbort(cmd);
  }

  /**
   * @brief Release resources in use by the command.
   */

  void ExternalInterface::releaseResourcesForCommand(Command *cmd)
  {
    m_raInterface->releaseResourcesForCommand(cmd);
  }

  /**
   * @brief Schedule this update for execution.
   */
  void ExternalInterface::enqueueUpdate(Update *update)
  {
    m_updatesToExecute.push(update);
  }

  /**
   * @brief Send all pending commands and updates to the external system(s).
   */
  void ExternalInterface::executeOutboundQueue()
  {
    if (!m_commandsToExecute.empty()) {
      LinkedQueue<Command> resourceCmds;
      while (Command *cmd = m_commandsToExecute.front()) {
        m_commandsToExecute.pop();
        if (cmd->getResourceValues().empty()) {
          // Execute it now
          debugMsg("ResourceArbiterInterface:partitionCommands",
                   " accepting " << cmd->getName() << " with no resource requests"); // legacy msg
          this->executeCommand(cmd);
        }
        else {
          // Queue it for arbitration
          resourceCmds.push(cmd);
        }
      }

      if (!resourceCmds.empty()) {
        LinkedQueue<Command> acceptCmds, rejectCmds;
        m_raInterface->arbitrateCommands(resourceCmds, acceptCmds, rejectCmds);
        while (Command *cmd = acceptCmds.front()) {
          acceptCmds.pop();
          this->executeCommand(cmd);
        }
        while (Command *cmd = rejectCmds.front()) {
          rejectCmds.pop();
          debugMsg("Test:testOutput", 
                   "Permission to execute " << cmd->getName()
                   << " has been denied by the resource arbiter.");
          reportCommandArbitrationFailure(cmd);
        }
      }
    }

    while (!m_updatesToExecute.empty()) {
      this->executeUpdate(m_updatesToExecute.front());
      m_updatesToExecute.pop();
    }
  }

  bool ExternalInterface::outboundQueueEmpty() const
  {
    return m_commandsToExecute.empty() && m_updatesToExecute.empty();
  }

  void ExternalInterface::lookupReturn(State const &state, Value const &value)
  {
    debugMsg("ExternalInterface:lookupReturn", '(' << state << ", " << value << ')');
    StateCacheMap::instance().ensureStateCacheEntry(state)->update(value);
  }

  void ExternalInterface::commandReturn(Command *cmd, Value const &value)
  {
    cmd->returnValue(value);
  }

  void ExternalInterface::commandHandleReturn(Command *cmd, CommandHandleValue val)
  {
    cmd->setCommandHandle(val);
  }

  void ExternalInterface::commandAbortAcknowledge(Command *cmd, bool ack)
  {
    cmd->acknowledgeAbort(ack);
  }

  void ExternalInterface::acknowledgeUpdate(Update *upd, bool val)
  {
    upd->acknowledge(val);
  }

}


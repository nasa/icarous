/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

#ifndef IPC_ROBOT_ADAPTER_HH
#define IPC_ROBOT_ADAPTER_HH

#include "ipc.h"
#include "ipc-data-formats.h"
#include "IpcFacade.hh"

#include <map>
#include <string>
#include <vector>

#include <pthread.h>

class RobotBase;

class IpcRobotAdapter
{
public:

  /**
   * @brief Constructor. Opens the connection and spawns a listener thread.
   */
  IpcRobotAdapter(const std::string& centralhost = "localhost:1381");

  /**
   * @brief Destructor. Shuts down the listener thread and closes the connection.
   */
  ~IpcRobotAdapter();

  /**
   * @brief Make the adapter aware of the robot.
   */
  void registerRobot(const std::string& name, RobotBase* robot);

private:

  //
  // Deliberately unimplemented
  //
  IpcRobotAdapter();
  IpcRobotAdapter(const IpcRobotAdapter&);
  IpcRobotAdapter& operator=(const IpcRobotAdapter&);

  //
  // Private data types
  //

  //* @brief Map robot names to instances
  typedef std::map<const std::string, RobotBase*> NameToRobotMap;

  //* @brief Unique identifier of a message sequence
  typedef std::pair<std::string, uint32_t> IpcMessageId;

  //* brief Cache of not-yet-complete incoming message sequences
  typedef std::map<IpcMessageId, std::vector<const PlexilMsgBase*> > IncompleteMessageMap;

  //* @brief State name to unique ID map
  typedef std::map<std::string, IpcMessageId> NameUniqueIDMap;

  //
  // Implementation methods
  //

  /**
   * @brief Handler function as seen by IPC.
   */

  static void messageHandler(MSG_INSTANCE rawMsg,
			     void * unmarshalledMsg,
			     void * this_as_void_ptr);

  //* brief Class to receive messages from Ipc
  class MessageListener : public PLEXIL::IpcMessageListener {
  public:
    MessageListener(IpcRobotAdapter&);
    ~MessageListener();
    void ReceiveMessage(const std::vector<const PlexilMsgBase*>& msgs);
  private:
    IpcRobotAdapter& m_adapter;
  };

  /**
   * @brief Send a command to the simulator
   */
  void processCommand(const std::vector<const PlexilMsgBase*>& msgs);

  /**
   * @brief Deal with a LookupNow request
   */
  void processLookupNow(const std::vector<const PlexilMsgBase*>& msgs);


  //
  // Member variables
  //

  //* @brief Map recording robot names and instances.
  NameToRobotMap m_robots;
  
  //* @brief Map from state name to unique ID of LookupOnChange request
  NameUniqueIDMap m_stateUIDMap;

  //* @brief Handler for the IPC connection
  PLEXIL::IpcFacade m_ipcFacade;

  //* @brief Message handler for IPC
  MessageListener m_listener;

};

#endif // IPC_ROBOT_ADAPTER_HH

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

#include "IpcAdapter.hh"

#include "ipc-data-formats.h"

#include "AdapterExecInterface.hh"
#include "AdapterConfiguration.hh"
#include "ArrayImpl.hh"
#include "Command.hh"
#include "Debug.hh"
#include "Error.hh"
#include "InterfaceSchema.hh"
#include "parsePlan.hh"
#include "State.hh"
#include "StateCacheEntry.hh"
#include "ThreadSpawn.hh"
#include "Update.hh"
#include "NodeConnector.hh"

#include "pugixml.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>

namespace PLEXIL 
{

  /**
   * @brief Constructor.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   */
  IpcAdapter::IpcAdapter(AdapterExecInterface& execInterface) :
    InterfaceAdapter(execInterface),
    m_ipcFacade(),
    m_messageQueues(execInterface),
    m_externalLookupNames(),
    m_externalLookups(),
    m_listener(*this),
    m_lookupSem(),
    m_cmdMutex(),
    m_pendingLookupResult(),
    m_pendingLookupState(),
    m_pendingLookupSerial(0)
  {
    debugMsg("IpcAdapter:IpcAdapter", " configuration XML not provided");
  }

  /**
   * @brief Constructor from configuration XML.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   * @param xml A const reference to the XML element describing this adapter
   */
  IpcAdapter::IpcAdapter(AdapterExecInterface& execInterface, pugi::xml_node const xml) :
    InterfaceAdapter(execInterface, xml),
    m_ipcFacade(),
    m_messageQueues(execInterface),
    m_externalLookupNames(),
    m_externalLookups(),
    m_listener(*this),
    m_lookupSem(),
    m_cmdMutex(),
    m_pendingLookupResult(),
    m_pendingLookupState(),
    m_pendingLookupSerial(0)
  {
    condDebugMsg(xml == NULL, "IpcAdapter:IpcAdapter", " configuration XML not provided");
    condDebugMsg(xml != NULL, "IpcAdapter:IpcAdapter", " configuration XML = " << xml);
  }

  /**
   * @brief Destructor.
   */
  IpcAdapter::~IpcAdapter() {
  }

  //
  // API to ExecApplication
  //

  /**
   * @brief Initializes the adapter, possibly using its configuration data.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::initialize() {
    debugMsg("IpcAdapter:initialize", " called");

    // Get taskName, serverName from XML, if supplied
    const char* taskName = "";
    const char* serverName = "";
    pugi::xml_attribute acceptDuplicates;

    pugi::xml_node const xml = this->getXml();
    if (!xml.empty()) {
      taskName = xml.attribute("TaskName").value();
      serverName = xml.attribute("Server").value();
      acceptDuplicates = xml.attribute("AllowDuplicateMessages");
      parseExternalLookups(xml.child("ExternalLookups"));
    }

    // Use defaults if necessary
    if (*serverName == '\0') {
      serverName = "localhost";
    }
    if (*taskName == '\0') {
      taskName = m_ipcFacade.getUID().c_str();
    }
    if (!acceptDuplicates.empty()) {
      m_messageQueues.setAllowDuplicateMessages(acceptDuplicates.as_bool());
    } 
    else {
      //debugging only. set to true for release
      m_messageQueues.setAllowDuplicateMessages(false);
    }

    debugMsg("IpcAdapter:initialize",
             " Connecting module " << taskName <<
             " to central server at " << serverName);

    // init IPC
    assertTrueMsg(m_ipcFacade.initialize(taskName, serverName) == IPC_OK,
                  "IpcAdapter: Unable to connect to the central server at " << serverName);

    // Register with AdapterExecInterface
    g_configuration->defaultRegisterAdapter(this);

    // Auto-register for standard IPC commands, if not already done
    if ((g_configuration->getDefaultInterface() != this)
        && (g_configuration->getDefaultCommandInterface() != this)) {
      static const std::string sl_internalCommands[] =
        {SEND_MESSAGE_COMMAND(),
         RECEIVE_MESSAGE_COMMAND(),
         RECEIVE_COMMAND_COMMAND(),
         GET_PARAMETER_COMMAND(),
         SEND_RETURN_VALUE_COMMAND(),
         UPDATE_LOOKUP_COMMAND(),
         ""
        };
      for (size_t i = 0; !sl_internalCommands[i].empty(); ++i) {
        std::string const &cmd = sl_internalCommands[i];
        InterfaceAdapter const *adapter = g_configuration->getCommandInterface(cmd);
        if (adapter == NULL || adapter != this)
          g_configuration->registerCommandInterface(cmd, this);
      }
    }

    debugMsg("IpcAdapter:initialize", " succeeded");
    return true;
  }

  /**
   * @brief Starts the adapter, possibly using its configuration data.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::start() {
    // Spawn listener thread
    assertTrueMsg(m_ipcFacade.start() == IPC_OK,
                  "IpcAdapter: Unable to spawn IPC dispatch thread");
    debugMsg("IpcAdapter:start", " spawned IPC dispatch thread");
    m_ipcFacade.subscribeAll(&m_listener);
    debugMsg("IpcAdapter:start", " succeeded");
    return true;
  }

  /**
   * @brief Stops the adapter.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::stop() {
    m_ipcFacade.stop();

    debugMsg("IpcAdapter:stop", " succeeded");
    return true;
  }

  /**
   * @brief Resets the adapter.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::reset() {
    // No-op (?)
    return true;
  }

  /**
   * @brief Shuts down the adapter, releasing any of its resources.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::shutdown() {
    m_ipcFacade.shutdown();
    debugMsg("IpcAdapter:shutdown", " succeeded");
    return true;
  }

  /**
   * @brief Perform an immediate lookup on an existing state.
   * @param state The state.
   * @param entry The state cache entry in which to store the result.
   */

  void IpcAdapter::lookupNow(const State& state, StateCacheEntry &entry) 
  {
    ExternalLookupMap::iterator it = m_externalLookups.find(state);
    if (it != m_externalLookups.end()) {
      debugMsg("IpcAdapter:lookupNow",
               " returning external lookup " << state
               << " with internal value " << it->second);
      entry.update(it->second);
    }
    else {
      const std::string& stateName = state.name();
      const std::vector<Value>& params = state.parameters();
      size_t nParams = params.size();
      debugMsg("IpcAdapter:lookupNow",
               " for state " << stateName
               << " with " << nParams << " parameters");

      //send lookup message
      m_pendingLookupResult.setUnknown();
      size_t sep_pos = stateName.find_first_of(TRANSACTION_ID_SEPARATOR_CHAR);
      //decide to direct or publish lookup
      {
        ThreadMutexGuard g(m_cmdMutex);
        if (sep_pos != std::string::npos) {
          std::string const dest(stateName.substr(0, sep_pos));
          std::string const sentStateName = stateName.substr(sep_pos + 1);
          m_pendingLookupState = state;
          m_pendingLookupState.setName(sentStateName);
          m_pendingLookupSerial = m_ipcFacade.sendLookupNow(sentStateName,
                                                            dest,
                                                            params);
        }
        else {
          m_pendingLookupState = state;
          m_pendingLookupSerial = m_ipcFacade.publishLookupNow(stateName, params);
        }
      }

      // Wait for results
      // N.B. shouldn't have to worry about signals causing wait to be interrupted -
      // ExecApplication blocks most of the common ones
      int errnum = m_lookupSem.wait();
      assertTrueMsg(errnum == 0,
                    "IpcAdapter::lookupNow: semaphore wait failed, result = " << errnum);

      entry.update(m_pendingLookupResult);

      // Clean up
      {
        ThreadMutexGuard g(m_cmdMutex);
        m_pendingLookupSerial = 0;
        m_pendingLookupState = State();
      }
      m_pendingLookupResult.setUnknown();
    }
  }

  /**
   * @brief Inform the interface that it should report changes in value of this state.
   * @param state The state.
   * @note Since all telemetry received is sent to the Exec, this is a no-op.
   */

  void IpcAdapter::subscribe(const State& state)
  {
  }

  /**
   * @brief Inform the interface that a lookup should no longer receive updates.
   * @param state The state.
   * @note Since all telemetry received is sent to the Exec, this is a no-op.
   */

  void IpcAdapter::unsubscribe(const State& state)
  {
  }

  /**
   * @brief Advise the interface of the current thresholds to use when reporting this state.
   * @param state The state.
   * @param hi The upper threshold, at or above which to report changes.
   * @param lo The lower threshold, at or below which to report changes.
   * @note This is a no-op for IpcAdapter.
   */

  void IpcAdapter::setThresholds(const State& /* state */, double /* hi */, double /* lo */)
  {
  }

  void IpcAdapter::setThresholds(const State& /* state */, int32_t /* hi */, int32_t /* lo */)
  {
  }

  /**
   * @brief Send the name of the supplied node, and the supplied value pairs, to the planner.
   * @param update The Update object.
   */

  void IpcAdapter::sendPlannerUpdate(Update* update)
  {
    std::string const& name = update->getSource()->getNodeId();
    debugMsg("IpcAdapter:sendPlannerUpdate", " for \"" << name << "\"");
    std::vector<std::pair<std::string, Value> > args(update->getPairs().begin(),
                                                     update->getPairs().end());
    if(args.empty()) {
      debugMsg("IpcAdapter:sendPlannerUpdate", "Emtpy update");
      return;
    }

    ThreadMutexGuard guard(m_cmdMutex);
    uint32_t serial = m_ipcFacade.publishUpdate(name, args);
    assertTrueMsg(serial != IpcFacade::ERROR_SERIAL(),
                  "IpcAdapter::sendPlannerUpdate: IPC Error, IPC_errno = " <<
                  m_ipcFacade.getError());
    //is this the right thing to do?
    update->acknowledge(true);
    m_execInterface.handleUpdateAck(update, true);
    m_execInterface.notifyOfExternalEvent();
  }

  /**
   * @brief Execute a command with the requested arguments.
   * @param command The Command object.
   */

  void IpcAdapter::executeCommand(Command *command) 
  {
    std::string const &name = command->getName();
    // Check for SendMessage command
    if (name == SEND_MESSAGE_COMMAND())
      executeSendMessageCommand(command);
    // Check for SendReturnValue command
    else if (name == SEND_RETURN_VALUE_COMMAND())
      executeSendReturnValueCommand(command);
    // Check for ReceiveMessage command
    else if (name == RECEIVE_MESSAGE_COMMAND())
      executeReceiveMessageCommand(command);
    // Check for ReceiveCommand command
    else if (name == RECEIVE_COMMAND_COMMAND())
      executeReceiveCommandCommand(command);
    else if (name == GET_PARAMETER_COMMAND())
      executeGetParameterCommand(command);
    // Check for UpdateLookup command
    else if (name == UPDATE_LOOKUP_COMMAND())
      executeUpdateLookupCommand(command);
    else
      // general case
      executeDefaultCommand(command);
  }

  /**
   * @brief Abort the pending command.
   * @param command The Command object.
   */

  void IpcAdapter::invokeAbort(Command *command) {
    //TODO: implement unique command IDs for referencing command instances
    std::string const &cmdName = command->getName();
    assertTrueMsg(cmdName == RECEIVE_MESSAGE_COMMAND() || cmdName == RECEIVE_COMMAND_COMMAND(),
                  "IpcAdapter: Attempt to abort \"" << cmdName
                  << "\" command.\n Only ReceiveMessage and ReceiveCommand commands can be aborted.");
    std::vector<Value> const &cmdArgs = command->getArgValues();
    assertTrueMsg(cmdArgs.size() == 1,
                  "IpcAdapter: Aborting ReceiveMessage requires exactly one argument");
    assertTrueMsg(cmdArgs.front().isKnown() && cmdArgs.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The argument to the ReceiveMessage abort, "
                  << cmdArgs.front()
                  << ", is not a valid String value");

    std::string const *theMessage;
    cmdArgs.front().getValuePointer(theMessage);

    debugMsg("IpcAdapter:invokeAbort", "Aborting command listener " << *theMessage << std::endl);
    m_messageQueues.removeRecipient(*theMessage, command);
    m_execInterface.handleCommandAbortAck(command, true);
    m_execInterface.notifyOfExternalEvent();
  }

  //
  // Implementation methods
  //


  /**
   * @brief Helper function for converting message names into the proper format given the command type and a user-defined id.
   */
  std::string IpcAdapter::formatMessageName(const std::string& name, const std::string& command, int id) 
  {
    std::ostringstream ss;
    if (command == RECEIVE_COMMAND_COMMAND()) {
      ss << COMMAND_PREFIX() << name;
    }
    else if (command == GET_PARAMETER_COMMAND()) {
      ss << PARAM_PREFIX() << name;
    }
    else {
      ss << name;
    }
    ss << '_' << id;
    return ss.str();
  }

  /**
   * @brief Helper function for converting message names into the proper format given the command type.
   */
  std::string IpcAdapter::formatMessageName(const std::string& name, const std::string& command) {
    return formatMessageName(name, command, 0);
  }

  /**
   * @brief Helper function for converting message names into the proper format given the command type.
   */
  std::string IpcAdapter::formatMessageName(const char* name, const std::string& command) {
    return formatMessageName(std::string(name), command, 0);
  }

  /**
   * @brief handles SEND_MESSAGE_COMMAND commands from the exec
   */
  void IpcAdapter::executeSendMessageCommand(Command *command)
  {
    std::vector<Value> const &args = command->getArgValues();
    // Check for one argument, the message
    assertTrueMsg(args.size() == 1,
                  "IpcAdapter: The SendMessage command requires exactly one argument");
    assertTrueMsg(args.front().isKnown() && args.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The argument to the SendMessage command, "
                  << args.front() << ", is not a valid String value");
    std::string const *theMessage;
    args.front().getValuePointer(theMessage);
    debugMsg("IpcAdapter:executeCommand",
             " SendMessage(\"" << *theMessage << "\")");
    assertTrue_2(m_ipcFacade.publishMessage(*theMessage) != IpcFacade::ERROR_SERIAL(), "Message publish failed");
    // store ack
    m_execInterface.handleCommandAck(command, COMMAND_SUCCESS);
    m_execInterface.notifyOfExternalEvent();
    debugMsg("IpcAdapter:executeCommand", " message \"" << *theMessage << "\" sent.");
  }

  /**
   * @brief handles SEND_RETURN_VALUE_COMMAND commands from the exec
   */
  void IpcAdapter::executeSendReturnValueCommand(Command *command)
  {
    std::vector<Value> const &args = command->getArgValues();
    // Check for two arguments, the message and the value
    assertTrueMsg(args.size() == 2,
                  "IpcAdapter: The SendReturnValue command requires exactly two arguments.");
    assertTrueMsg(args.front().isKnown() && args.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The message name argument to the SendReturnValue command, "
                  << args.front() << ", is not a valid String value");
    const std::string *front;
    args.front().getValuePointer(front);
    uint32_t serial;
    //grab serial from parameter
    std::string::size_type sep_pos = front->find(SERIAL_UID_SEPERATOR(), 1);
    assertTrueMsg(sep_pos != std::string::npos, "Could not find UID seperator in first parameter of return value");
    std::string const serial_string = front->substr(0, sep_pos);
    serial = atoi(serial_string.c_str());
    std::string front_string = front->substr(sep_pos + 1, front->size());
    debugMsg("IpcAdapter:executeCommand",
             " SendReturnValue(sender_serial:\"" << serial << "\" \"" << front_string << "\")");
    //publish
    serial = m_ipcFacade.publishReturnValues(serial, front_string, args[1]);
    assertTrue_2(serial != IpcFacade::ERROR_SERIAL(), "Return values failed to be sent");

    // store ack
    m_execInterface.handleCommandAck(command, COMMAND_SUCCESS);
    m_execInterface.notifyOfExternalEvent();
    debugMsg("IpcAdapter:executeCommand", " return value sent.");
  }

  /**
   * @brief handles SEND_RETURN_VALUE_COMMAND commands from the exec
   */
  void IpcAdapter::executeReceiveMessageCommand(Command *command) 
  {
    std::vector<Value> const &args = command->getArgValues();
    // Check for one argument, the message
    assertTrueMsg(args.size() == 1,
                  "IpcAdapter: The ReceiveMessage command requires exactly one argument");
    assertTrueMsg(args.front().isKnown() && args.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The argument to the SendMessage command, "
                  << args.front()
                  << ", is not a valid String value");
    std::string const *theMessage;
    args.front().getValuePointer(theMessage);
    m_messageQueues.addRecipient(*theMessage, command);
    m_execInterface.handleCommandAck(command, COMMAND_SENT_TO_SYSTEM);
    m_execInterface.notifyOfExternalEvent();
    debugMsg("IpcAdapter:executeCommand", " message handler for \"" << *theMessage << "\" registered.");
  }

  /**
   * @brief handles RECEIVE_COMMAND_COMMAND commands from the exec
   */
  void IpcAdapter::executeReceiveCommandCommand(Command *command)
  {
    std::vector<Value> const &args = command->getArgValues();
    // Check for one argument, the message
    assertTrueMsg(args.size() == 1,
                  "IpcAdapter: The " << RECEIVE_COMMAND_COMMAND().c_str() << " command requires exactly one argument");
    assertTrueMsg(args.front().isKnown() && args.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The argument to the " << RECEIVE_COMMAND_COMMAND()
                  << " command, " << args.front()
                  << ", is not a string");
    std::string const *cmdName = NULL;
    args.front().getValuePointer(cmdName);
    std::string msgName(formatMessageName(*cmdName, RECEIVE_COMMAND_COMMAND()));
    m_messageQueues.addRecipient(msgName, command);
    m_execInterface.handleCommandAck(command, COMMAND_SENT_TO_SYSTEM);
    m_execInterface.notifyOfExternalEvent();
    debugMsg("IpcAdapter:executeCommand", " message handler for \"" << msgName << "\" registered.");
  }

  /**
   * @brief handles GET_PARAMETER_COMMAND commands from the exec
   */
  void IpcAdapter::executeGetParameterCommand(Command *command) 
  {
    std::vector<Value> const &args = command->getArgValues();
    // Check for one argument, the message
    assertTrueMsg(args.size() == 1 || args.size() == 2,
                  "IpcAdapter: The " << GET_PARAMETER_COMMAND().c_str() << " command requires either one or two arguments");
    assertTrueMsg(args.front().isKnown() && args.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The first argument to the " << GET_PARAMETER_COMMAND() << " command, "
                  << args.front() << ", is not a string");
    int32_t id;
    if (args.size() == 1)
      id = 0;
    else {
      assertTrueMsg(args[1].isKnown() && args[1].valueType() == INTEGER_TYPE,
                    "IpcAdapter: The second argument to the " << GET_PARAMETER_COMMAND() << " command, " << args[1]
                    << ", is not an Integer");
      args[1].getValue(id);
      assertTrueMsg(id >= 0,
                    "IpcAdapter: The second argument to the " << GET_PARAMETER_COMMAND() << " command, " << args[1]
                    << ", is not a valid index");
    }
    std::string const *cmdName = NULL;
    args.front().getValuePointer(cmdName);
    std::string msgName(formatMessageName(*cmdName, GET_PARAMETER_COMMAND(), id));
    m_messageQueues.addRecipient(msgName, command);
    m_execInterface.handleCommandAck(command, COMMAND_SENT_TO_SYSTEM);
    m_execInterface.notifyOfExternalEvent();
    debugMsg("IpcAdapter:executeCommand", " message handler for \"" << msgName << "\" registered.");
  }

  void IpcAdapter::executeUpdateLookupCommand(Command *command) 
  {
    std::vector<Value> const &args = command->getArgValues();
    size_t nargs = args.size();
    assertTrueMsg(nargs >= 2,
                  "IpcAdapter: The " << UPDATE_LOOKUP_COMMAND() << " command requires at least two arguments");
    assertTrueMsg(args.front().isKnown() && args.front().valueType() == STRING_TYPE,
                  "IpcAdapter: The argument to the " << UPDATE_LOOKUP_COMMAND().c_str()
                  << " command, " << args.front() << ", is not a string");
    ThreadMutexGuard guard(m_cmdMutex);

    // Extract state from command parameters
    std::string const *lookupName;
    args.front().getValuePointer(lookupName);
    // Warn, but do not crash, if not declared
    if (std::find(m_externalLookupNames.begin(),
                  m_externalLookupNames.end(),
                  *lookupName)
        == m_externalLookupNames.end()) {
      warn("IpcAdapter: The external lookup " << *lookupName << " is not declared. Ignoring UpdateLookup command.");
      m_execInterface.handleCommandAck(command, COMMAND_FAILED);
      m_execInterface.notifyOfExternalEvent();
      return;
    }
      
    State state(*lookupName, nargs - 2);
    for (size_t i = 2; i < nargs; ++i)
      state.setParameter(i - 2, args[i]);

    //Set value internally
    m_externalLookups[state] = args[1];
    // Notify internal users, if any
    m_execInterface.handleValueChange(state, args[1]);

    //send telemetry
    std::vector<Value> result_and_args(nargs - 1);
    for (size_t i = 1; i < nargs; ++i)
      result_and_args[i - 1] = args[i];
    if (m_ipcFacade.publishTelemetry(*lookupName, result_and_args)
        == IpcFacade::ERROR_SERIAL()) {
      warn("IpcAdapter: publishTelemetry returned status \"" << m_ipcFacade.getError() << "\"");
      m_execInterface.handleCommandAck(command, COMMAND_FAILED);
    }
    else {
      // Notify of success
      m_execInterface.handleCommandAck(command, COMMAND_SUCCESS);
    }
    
    m_execInterface.notifyOfExternalEvent();
  }

  /**
   * @brief handles all other commands from the exec
   */
  void IpcAdapter::executeDefaultCommand(Command *command) 
  {
    std::string const &name = command->getName();
    debugMsg("IpcAdapter:executeCommand", " for \"" << name << "\"");
    std::vector<Value> const &args = command->getArgValues();
    if (!args.empty())
      debugMsg("IpcAdapter:executeCommand", " first parameter is \""
               << args.front()
               << "\"");

    uint32_t serial;
    size_t sep_pos = name.find_first_of(TRANSACTION_ID_SEPARATOR_CHAR);
    //lock mutex to ensure no return values are processed while the command is being
    //sent and logged
    ThreadMutexGuard guard(m_cmdMutex);
    //decide to direct or publish command
    if (sep_pos != std::string::npos) {
      serial = m_ipcFacade.sendCommand(name.substr(sep_pos + 1), name.substr(0, sep_pos), args);
    }
    else {
      serial = m_ipcFacade.publishCommand(name, args);
    }
    // log ack and return variables in case we get values for them
    assertTrueMsg(serial != IpcFacade::ERROR_SERIAL(),
                  "IpcAdapter::executeCommand: IPC Error, IPC_errno = " << m_ipcFacade.getError());
    m_pendingCommands[serial] = command;
    // store ack
    m_execInterface.handleCommandAck(command, COMMAND_SENT_TO_SYSTEM);
    m_execInterface.notifyOfExternalEvent();
    debugMsg("IpcAdapter:executeCommand", " command \"" << name << "\" sent.");
  }

  // Helper function used in ParseExternalLookups.
  static Value parseTypedValue(char const *type, pugi::xml_attribute const value)
  {
    ValueType t = parseValueType(type);

    switch (t) {
    case BOOLEAN_TYPE:
      return Value(value.as_bool());

    case INTEGER_TYPE:
      // FIXME: pugixml attribute routines don't have out-of-band error signaling
      // FIXME: pugixml relies on compiler definition of int type
      return Value((int32_t) value.as_int());

    case REAL_TYPE:
      return Value(value.as_double());
      
    case STRING_TYPE:
      return Value(value.value()); // empty string is valid

    case BOOLEAN_ARRAY_TYPE: {
      std::vector<std::string> * args =
        InterfaceSchema::parseCommaSeparatedArgs(value.value());
      size_t n = args->size();
      BooleanArray ba(n);
      for (size_t i = 0; i < n; ++i) {
        Boolean b = false;
        if (!parseValue<Boolean>((*args)[i], b)) {
          assertTrueMsg(ALWAYS_FAIL,
                        "IpcAdapter: \"" << (*args)[i] << "\" is not a valid Boolean");
          return Value();
        }
        ba.setElement(i, b);
      }
      delete args;
      return Value(ba);
    }

    case INTEGER_ARRAY_TYPE: {
      std::vector<std::string> * args =
        InterfaceSchema::parseCommaSeparatedArgs(value.value());
      size_t n = args->size();
      IntegerArray ia(n);
      for (size_t i = 0; i < n; ++i) {
        Integer in = 0;
        if (!parseValue<Integer>((*args)[i], in)) {
          assertTrueMsg(ALWAYS_FAIL,
                        "IpcAdapter: \"" << (*args)[i] << "\" is not a valid Integer");
          return Value();
        }
        ia.setElement(i, in);
      }
      delete args;
      return Value(ia);
    }

    case REAL_ARRAY_TYPE: {
      std::vector<std::string> * args =
        InterfaceSchema::parseCommaSeparatedArgs(value.value());
      size_t n = args->size();
      RealArray ra(n);
      for (size_t i = 0; i < n; ++i) {
        Real d = 0.0;
        if (!parseValue<Real>((*args)[i], d)) {
          assertTrueMsg(ALWAYS_FAIL,
                        "IpcAdapter: \"" << (*args)[i] << "\" is not a valid Real number");
          return Value();
        }
        ra.setElement(i, d);
      }
      delete args;
      return Value(ra);
    }
      
    case STRING_ARRAY_TYPE: {
      std::vector<std::string> * args =
        InterfaceSchema::parseCommaSeparatedArgs(value.value());
      size_t n = args->size();
      StringArray sa(n);
      for (size_t i = 0; i < n; ++i) {
        String s = (*args)[i];
        // TODO: handle escapes?
        if (s[0] != '"' || s[s.size() - 1] != '"') {
          assertTrueMsg(ALWAYS_FAIL,
                        "IpcAdapter: String \"" << (*args)[i] << "\" lacks leading or trailing double-quote character(s)");
          return Value();
        }
        s = s.substr(1, s.size() - 2);
        sa.setElement(i, s);
      }
      delete args;
      return Value(sa);
    }
      
    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "IpcAdapter: invalid or unimplemented lookup value type " << type);
      return Value();
    }
  }
  
  void IpcAdapter::parseExternalLookups(pugi::xml_node const external) 
  {
    if (external) {

      //process external lookups
      pugi::xml_node lookup = external.child("Lookup");
      while (lookup != 0) {
        const pugi::xml_attribute nameAttr = lookup.attribute("name");
        assertTrueMsg(!nameAttr.empty() && nameAttr.value(),
                      "IpcAdapter:parseExternalLookups: Lookup element attribute 'name' missing or empty");
        const pugi::xml_attribute typeAttr = lookup.attribute("type");
        assertTrueMsg(!typeAttr.empty() && typeAttr.value(),
                      "IpcAdapter:parseExternalLookups: Lookup element attribute 'type' missing or empty");
        const pugi::xml_attribute defAttr = lookup.attribute("value");
        assertTrueMsg(!defAttr.empty() && defAttr.value(),
                      "IpcAdapter:parseExternalLookups: Lookup element attribute 'value' missing or empty");

        std::string const nameString(nameAttr.value());

        debugMsg("IpcAdapter:parseExternalLookups",
                 "External Lookup: state=\"" << nameString
                 << "\" type=\"" << typeAttr.value()
                 << "\" default=\"" << defAttr.value() << "\"");
        const char *type = typeAttr.value();
        if (std::find(m_externalLookupNames.begin(),
                      m_externalLookupNames.end(),
                      nameString)
            != m_externalLookupNames.end()) {
          warn("IpcAdapter: Lookup name \"" << nameString << "\" multiply declared");
        }
        else {
          m_externalLookupNames.push_back(nameString);
          g_configuration->registerLookupInterface(nameString, this);
          m_externalLookups[State(nameString)] = parseTypedValue(type, defAttr);
        }
        lookup = lookup.next_sibling("Lookup");
      }
    }
  }

  /**
   * @brief Send a single message to the Exec's queue and free the message
   */
  void IpcAdapter::enqueueMessage(const PlexilMsgBase* msgData) {
    assertTrue_2(msgData, "IpcAdapter::enqueueMessage: msgData is null");

    switch (msgData->msgType) {
    case PlexilMsgType_NotifyExec:
      m_execInterface.notifyOfExternalEvent();
      break;

      // AddPlan is a PlexilStringValueMsg
    case PlexilMsgType_AddPlan: {
      const PlexilStringValueMsg* stringMsg = reinterpret_cast<const PlexilStringValueMsg*>(msgData);
      assertTrueMsg(stringMsg->stringValue != NULL,
                    "IpcAdapter::enqueueMessage: AddPlan message contains null plan string");

      // parse into XML document
      try {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load(stringMsg->stringValue);
        if (result.status == pugi::status_ok) {
          m_execInterface.handleAddPlan(doc.document_element());
          // Always notify immediately when adding a plan
          m_execInterface.notifyOfExternalEvent();
        }
        else {
          std::cerr << "Error parsing plan XML:\n" << result.description() << std::endl;
        }
      } catch (const ParserException& e) {
        std::cerr << "Error parsing plan: \n" << e.what() << std::endl;
      }
    }
      break;

      // AddPlanFile is a PlexilStringValueMsg
    case PlexilMsgType_AddPlanFile: {
      const PlexilStringValueMsg* stringMsg = reinterpret_cast<const PlexilStringValueMsg*>(msgData);
      assertTrueMsg(stringMsg->stringValue != NULL,
                    "IpcAdapter::enqueueMessage: AddPlanFile message contains null file name");

      // parse into XML document
      try {
        pugi::xml_document *doc = loadXmlFile(stringMsg->stringValue);
        if (doc) {
          m_execInterface.handleAddPlan(doc->document_element());
          // Always notify immediately when adding a plan
          m_execInterface.notifyOfExternalEvent();
        }
        else {
          std::cerr << "Error parsing plan from file: file " << stringMsg->stringValue << " not found" << std::endl;
        }
      } catch (const ParserException& e) {
        std::cerr << "Error parsing plan from file: \n" << e.what() << std::endl;
      }
    }
      break;

      // AddLibrary is a PlexilStringValueMsg
    case PlexilMsgType_AddLibrary: {
      const PlexilStringValueMsg* stringMsg = reinterpret_cast<const PlexilStringValueMsg*>(msgData);
      assertTrueMsg(stringMsg->stringValue != NULL,
                    "IpcAdapter::enqueueMessage: AddLibrary message contains null library node string");

      // parse XML into node structure
      try {
        pugi::xml_document *doc = new pugi::xml_document;
        pugi::xml_parse_result result = doc->load(stringMsg->stringValue);
        if (result.status == pugi::status_ok)
          m_execInterface.handleAddLibrary(doc);
        else {
          delete doc;
          std::cerr << "Error parsing plan XML:\n" << result.description() << std::endl;
        }
      } catch (const ParserException& e) {
        std::cerr << "Error parsing library node: \n" << e.what() << std::endl;
      }
    }
      break;

      // AddLibraryFile is a PlexilStringValueMsg
    case PlexilMsgType_AddLibraryFile: {
      const PlexilStringValueMsg* stringMsg = reinterpret_cast<const PlexilStringValueMsg*>(msgData);
      assertTrueMsg(stringMsg->stringValue != NULL,
                    "IpcAdapter::enqueueMessage: AddLibraryFile message contains null file name");

      // parse XML into node structure
      try {
        pugi::xml_document *doc = loadXmlFile(stringMsg->stringValue);
        if (doc)
          m_execInterface.handleAddLibrary(doc);
        else {
          std::cerr << "Error parsing plan from file: file " << stringMsg->stringValue << " not found" << std::endl;
        }
      } catch (const ParserException& e) {
        std::cerr << "Error parsing library file: \n" << e.what() << std::endl;
      }
    }
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "IpcAdapter::enqueueMessage: invalid message type " << msgData->msgType)
        ;
    }
  }

  /**
   * @brief Process a PlexilMsgType_Message packet and free the message
   */
  void IpcAdapter::handleMessageMessage(const PlexilStringValueMsg* msgData) {
    assertTrueMsg(msgData != NULL,
                  "IpcAdapter::handleMessageMessage: msgData is null")
      assertTrueMsg(msgData->stringValue != NULL,
                    "IpcAdapter::handleMessageMessage: stringValue is null")
      m_messageQueues.addMessage(msgData->stringValue);
  }

  /**
   * @brief Queues the command in the message queue
   */
  void IpcAdapter::handleCommandSequence(const std::vector<const PlexilMsgBase*>& msgs) {
    //only support one parameter, the id
    //TODO: support more parameters
    const PlexilStringValueMsg* header = reinterpret_cast<const PlexilStringValueMsg*>(msgs[0]);
    std::ostringstream uid;
    uid << ((int) header->header.serial) << SERIAL_UID_SEPERATOR() << header->header.senderUID;
    std::string uid_lbl(uid.str());
    debugMsg("IpcAdapter:handleCommandSequence",
             " adding \"" << header->stringValue << "\" to the command queue");
    const std::string& msg(formatMessageName(header->stringValue, RECEIVE_COMMAND_COMMAND()));
    m_messageQueues.addMessage(msg, uid_lbl);
    int i = 0;
    for (std::vector<const PlexilMsgBase*>::const_iterator it = ++msgs.begin(); it != msgs.end(); ++it, ++i) {
      std::string const paramLbl(formatMessageName(uid_lbl, GET_PARAMETER_COMMAND(), i));
      m_messageQueues.addMessage(paramLbl, getPlexilMsgValue(*it));
    }
  }

  /**
   * @brief Process a TelemetryValues message sequence
   */

  void IpcAdapter::handleTelemetryValuesSequence(const std::vector<const PlexilMsgBase*>& msgs) 
  {
    const PlexilStringValueMsg* tv = reinterpret_cast<const PlexilStringValueMsg*>(msgs[0]);
    char const *stateName = tv->stringValue;
    if (!stateName || !*stateName) {
      debugMsg("IpcAdapter:handleTelemetryValuesSequence",
               " state name missing or empty, ignoring");
      return;
    }

    size_t nValues = msgs[0]->count;
    checkError(nValues > 0,
               "Telemetry values message requires at least 1 value");
    checkError(nValues + 1 == msgs.size(),
               "handleTelemetryValuesSequence: Expecting " << nValues + 1 << " messages, got " << msgs.size());

    // Result is next in sequence
    Value result = getPlexilMsgValue(msgs[1]);

    // Extract state parameters from trailing messages
    State state(stateName, nValues - 1);
    for (size_t i = 2; i <= nValues; ++i)
      state.setParameter(i - 2, getPlexilMsgValue(msgs[i]));
    
    debugMsg("IpcAdapter:handleTelemetryValuesSequence",
             " state \"" << stateName << "\" found, processing");

    // Check to see if a LookupNow is waiting on this value
    {
      ThreadMutexGuard g(m_cmdMutex);
      if (m_pendingLookupState == state) {
        m_pendingLookupResult = result;
        m_lookupSem.post();
        return;
      }
    }
    // Otherwise process normally
    m_execInterface.handleValueChange(state, result);
    m_execInterface.notifyOfExternalEvent();
  }

  /**
   * @brief Process a ReturnValues message sequence
   */

  void IpcAdapter::handleReturnValuesSequence(const std::vector<const PlexilMsgBase*>& msgs) 
  {
    const PlexilReturnValuesMsg* rv = (const PlexilReturnValuesMsg*) msgs[0];
    //lock mutex to ensure all sending procedures are complete.
    ThreadMutexGuard guard(m_cmdMutex);
    if (rv->requestSerial == m_pendingLookupSerial) {
      // LookupNow for which we are awaiting data
      debugMsg("IpcAdapter:handleReturnValuesSequence",
               " processing value(s) for a pending LookupNow");
      m_pendingLookupResult = parseReturnValue(msgs);
      // *** TODO: check for error
      m_lookupSem.post();
      return;
    }

    PendingCommandsMap::iterator cit = m_pendingCommands.find(rv->requestSerial);
    if (cit != m_pendingCommands.end()) {
      Command *cmd = cit->second;
      assertTrue_1(cmd);
      size_t nValues = msgs[0]->count;
      if (msgs[1]->count == MSG_COUNT_CMD_ACK) {
        assertTrueMsg(nValues == 1,
                      "IpcAdapter::handleReturnValuesSequence: command ack requires 1 value, received "
                      << nValues);
        if (!cmd->isActive()) {
          debugMsg("IpcAdapter:handleReturnValuesSequence",
                   " ignoring command handle value for inactive command");
          m_pendingCommands.erase(rv->requestSerial);
          return;
        }
        debugMsg("IpcAdapter:handleReturnValuesSequence",
                 " processing command acknowledgment for command " << cmd->getName());
        Value ack = parseReturnValue(msgs);
        assertTrue_2(ack.isKnown() && ack.valueType() == COMMAND_HANDLE_TYPE,
                     "IpcAdapter:handleReturnValuesSequence received a command acknowledgment which is not a CommandHandle value");
        uint16_t handle;
        ack.getValue(handle);
        m_execInterface.handleCommandAck(cmd, (CommandHandleValue) handle);
        m_execInterface.notifyOfExternalEvent();
      }
      else {
        if (!cmd->isActive()) {
          debugMsg("IpcAdapter:handleReturnValuesSequence",
                   " ignoring return value for inactive command");
          m_pendingCommands.erase(rv->requestSerial);
          return;
        }
        debugMsg("IpcAdapter:handleReturnValuesSequence",
                 " processing command return value for command " << cmd->getName());
        m_execInterface.handleCommandReturn(cmd, parseReturnValue(msgs));
        m_execInterface.notifyOfExternalEvent();
      }
    }
    else {
      debugMsg("IpcAdapter:handleReturnValuesSequence",
               " no lookup or command found for sequence");
    }
  }

  /**
   * @brief Process a LookupNow. Ignores any lookups that are not defined in config or published.
   */
  void IpcAdapter::handleLookupNow(const std::vector<const PlexilMsgBase*>& msgs) 
  {
    debugMsg("IpcAdapter:handleLookupNow", " received LookupNow");
    const PlexilStringValueMsg* msg = reinterpret_cast<const PlexilStringValueMsg*> (msgs.front());
    std::string name(msg->stringValue);
    size_t nParms = msgs.front()->count;
    State lookup(name, nParms);
    for (size_t i = 1 ; i <= nParms ; ++i)
      lookup.setParameter(i - 1, getPlexilMsgValue(msgs[i]));

    ThreadMutexGuard guard(m_cmdMutex);
      
    ExternalLookupMap::iterator it = m_externalLookups.find(lookup);
    if (it != m_externalLookups.end()) {
      debugMsg("IpcAdapter:handleLookupNow", " Publishing value of external lookup \"" << lookup
               << "\" with internal value '" << (*it).second << "'");
      m_ipcFacade.publishReturnValues(msg->header.serial, msg->header.senderUID, (*it).second);
      return;
    }

    if (nParms > 0) {
      // See if name is declared, and return default value
      lookup.setParameterCount(0);
      it = m_externalLookups.find(lookup);
      if (it != m_externalLookups.end()) {
        debugMsg("IpcAdapter:handleLookupNow", " Publishing default value of external lookup \"" << name
                 << "\" = '" << (*it).second << "'");
        m_ipcFacade.publishReturnValues(msg->header.serial, msg->header.senderUID, (*it).second);
        return;
      }      
    }

    // Ignore silently if we're not declared to handle it
    // Tracing here for debugging purposes
    debugMsg("IpcAdapter:handleLookupNow",
             " undeclared external lookup \"" << name << "\", ignoring");
  }

  /**
   * @brief Given a sequence of messages, turn the trailers into a double value for the Exec.
   */
  Value IpcAdapter::parseReturnValue(const std::vector<const PlexilMsgBase*>& msgs) 
  {
    size_t nValues = msgs[0]->count;
    checkError(nValues == 1, "PlexilMsgType_ReturnValue may only have one parameter");
    return getPlexilMsgValue(msgs[1]);
  }

  IpcAdapter::MessageListener::MessageListener(IpcAdapter& adapter)
    : m_adapter(adapter)
  {
  }

  IpcAdapter::MessageListener::~MessageListener()
  {
  }

  void IpcAdapter::MessageListener::ReceiveMessage(const std::vector<const PlexilMsgBase*>& msgs)
  {
    PlexilMsgType msgType = (PlexilMsgType) msgs.front()->msgType;
    debugMsg("IpcAdapter:handleIpcMessage", " received message type = " << msgType);
    switch (msgType) {
      // NotifyExec is a PlexilMsgBase
    case PlexilMsgType_NotifyExec:

      // AddPlan/AddPlanFile is a PlexilStringValueMsg
    case PlexilMsgType_AddPlan:
    case PlexilMsgType_AddPlanFile:

      // AddLibrary/AddLibraryFile is a PlexilStringValueMsg
    case PlexilMsgType_AddLibrary:
    case PlexilMsgType_AddLibraryFile:

      // In each case, simply send it to Exec and free the message
      m_adapter.enqueueMessage(msgs.front());
      break;

      // Command is a PlexilStringValueMsg
      // Optionally followed by parameters

    case PlexilMsgType_Command:
      // Stash this and wait for the rest
      debugMsg("IpcAdapter:handleIpcMessage", " processing as command")
        ;
      m_adapter.handleCommandSequence(msgs);
      break;

      // Message is a PlexilStringValueMsg
      // No parameters

    case PlexilMsgType_Message:
      debugMsg("IpcAdapter:handleIpcMessage", " processing as message")
        ;
      m_adapter.handleMessageMessage(reinterpret_cast<const PlexilStringValueMsg*>(msgs.front()));
      break;

      // Not implemented yet
    case PlexilMsgType_PlannerUpdate:
      break;

      // TelemetryValues is a PlexilStringValueMsg
      // Followed by 0 (?) or more values
    case PlexilMsgType_TelemetryValues:
      m_adapter.handleTelemetryValuesSequence(msgs);
      break;

      // ReturnValues is a PlexilReturnValuesMsg
      // Followed by 0 (?) or more values
    case PlexilMsgType_ReturnValues:
      // Only pay attention to our return values
      debugMsg("IpcAdapter:handleIpcMessage", " processing as return value")
        ;
      m_adapter.handleReturnValuesSequence(msgs);
      break;

    case PlexilMsgType_LookupNow:
      m_adapter.handleLookupNow(msgs);
      break;

      //unhandled so far
    default:
      debugMsg("IpcAdapter::enqueueMessageSequence",
               "Unhandled leader message type " << msgs.front()->msgType << ". Disregarding")
        ;
    }
  }

  //
  // Static member functions
  //

  /**
   * @brief Returns true if the string starts with the prefix, false otherwise.
   */
  bool IpcAdapter::hasPrefix(const std::string& s, const std::string& prefix) {
    if (s.size() < prefix.size())
      return false;
    return (0 == s.compare(0, prefix.size(), prefix));
  }

}

/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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
#include "IpcFacade.hh"
#include "MessageQueueMap.hh"
#include "State.hh"
#include "ThreadSemaphore.hh"

#include <ipc.h>

#include <list>

// Forward declarations outside of namespace
struct PlexilMsgBase;
struct PlexilStringValueMsg;

#define TRANSACTION_ID_SEPARATOR_CHAR ':'

namespace PLEXIL 
{

  class IpcAdapter: public InterfaceAdapter
  {
  public:

    //
    // Static class constants
    //

    DECLARE_STATIC_CLASS_CONST(std::string, COMMAND_PREFIX, "__COMMAND__")
    DECLARE_STATIC_CLASS_CONST(std::string, MESSAGE_PREFIX, "__MESSAGE__")
    DECLARE_STATIC_CLASS_CONST(std::string, LOOKUP_PREFIX, "__LOOKUP__")
    DECLARE_STATIC_CLASS_CONST(std::string, LOOKUP_ON_CHANGE_PREFIX, "__LOOKUP_ON_CHANGE__")
    DECLARE_STATIC_CLASS_CONST(std::string, PARAM_PREFIX, "__PARAMETER__")
    DECLARE_STATIC_CLASS_CONST(std::string, SERIAL_UID_SEPERATOR, ":")

    DECLARE_STATIC_CLASS_CONST(std::string, SEND_MESSAGE_COMMAND, "SendMessage")
    DECLARE_STATIC_CLASS_CONST(std::string, RECEIVE_MESSAGE_COMMAND, "ReceiveMessage")
    DECLARE_STATIC_CLASS_CONST(std::string, RECEIVE_COMMAND_COMMAND, "ReceiveCommand")
    DECLARE_STATIC_CLASS_CONST(std::string, GET_PARAMETER_COMMAND, "GetParameter")
    DECLARE_STATIC_CLASS_CONST(std::string, SEND_RETURN_VALUE_COMMAND, "SendReturnValue")
    DECLARE_STATIC_CLASS_CONST(std::string, UPDATE_LOOKUP_COMMAND, "UpdateLookup")


    /**
     * @brief Constructor.
     * @param execInterface Reference to the parent AdapterExecInterface object.
     */
    IpcAdapter(AdapterExecInterface& execInterface);

    /**
     * @brief Constructor from configuration XML.
     * @param execInterface Reference to the parent AdapterExecInterface object.
     * @param xml A const reference to the XML element describing this adapter
     * @note The instance maintains a shared pointer to the XML.
     */
    IpcAdapter(AdapterExecInterface& execInterface, pugi::xml_node const xml);

    /**
     * @brief Destructor.
     */
    virtual ~IpcAdapter();

    //
    // API to ExecApplication
    //

    /**
     * @brief Initializes the adapter, possibly using its configuration data.
     * @return true if successful, false otherwise.
     */
    virtual bool initialize();

    /**
     * @brief Starts the adapter, possibly using its configuration data.
     * @return true if successful, false otherwise.
     */
    virtual bool start();

    /**
     * @brief Stops the adapter.
     * @return true if successful, false otherwise.
     */
    virtual bool stop();

    /**
     * @brief Resets the adapter.
     * @return true if successful, false otherwise.
     * @note Adapters should provide their own methods.  The default method simply returns true.
     */
    virtual bool reset();

    /**
     * @brief Shuts down the adapter, releasing any of its resources.
     * @return true if successful, false otherwise.
     * @note Adapters should provide their own methods.  The default method simply returns true.
     */
    virtual bool shutdown();

    /**
     * @brief Perform an immediate lookup on an existing state.
     * @param state The state.
     * @param entry The state cache entry in which to store the result.
     */
    virtual void lookupNow(const State& state, StateCacheEntry &entry);

    /**
     * @brief Inform the interface that it should report changes in value of this state.
     * @param state The state.
     */
    virtual void subscribe(const State& state);

    /**
     * @brief Inform the interface that a lookup should no longer receive updates.
     * @param state The state.
     */
    virtual void unsubscribe(const State& state);

    /**
     * @brief Advise the interface of the current thresholds to use when reporting this state.
     * @param state The state.
     * @param hi The upper threshold, at or above which to report changes.
     * @param lo The lower threshold, at or below which to report changes.
     */
    virtual void setThresholds(const State& state, double hi, double lo);

    virtual void setThresholds(const State& state, int32_t hi, int32_t lo);

    /**
     * @brief Send the name of the supplied node, and the supplied value pairs, to the planner.
     * @param update The Update object.
     */

    virtual void sendPlannerUpdate(Update *update);

    /**
     * @brief Execute a command with the requested arguments.
     * @param command The Command object.
     */

    virtual void executeCommand(Command *command);

    /**
     * @brief Abort the pending command with the supplied name and arguments.
     * @param command The Command object.
     */

    virtual void invokeAbort(Command *command);

  private:

    // Deliberately unimplemented
    IpcAdapter();
    IpcAdapter(const IpcAdapter &);
    IpcAdapter & operator=(const IpcAdapter &);

    //
    // Implementation methods
    //

    /**
     * @brief handles SEND_MESSAGE_COMMAND commands from the exec
     */
    void executeSendMessageCommand(Command *command);

    /**
     * @brief handles SEND_RETURN_VALUE_COMMAND commands from the exec
     */
    void executeSendReturnValueCommand(Command *command);

    /**
     * @brief handles SEND_RETURN_VALUE_COMMAND commands from the exec
     */
    void executeReceiveMessageCommand(Command *command);

    /**
     * @brief handles SEND_RETURN_VALUE_COMMAND commands from the exec
     */
    void executeReceiveCommandCommand(Command *command);

    /**
     * @brief handles GET_PARAMETER_COMMAND commands from the exec
     */
    void executeGetParameterCommand(Command *command);

    /**
     * @brief handles UPDATE_LOOKUP_COMMAND commands from the exec
     */
    void executeUpdateLookupCommand(Command *command);

    /**
     * @brief handles all other commands from the exec
     */
    void executeDefaultCommand(Command *command);

    /**
     * @brief Parses external lookups from xml and puts them in the lookup map.
     * If external is NULL, does nothing.
     */
    void parseExternalLookups(pugi::xml_node const external);

    /**
     * @brief Handler function as seen by adapter.
     */

    void handleIpcMessage(const std::vector<PlexilMsgBase *> msgData);

    //
    // Helper methods
    //
    /**
     * @brief Send a single message to the Exec's queue and free the message
     */
    void enqueueMessage(const PlexilMsgBase* msgData);

    /**
     * @brief Process a PlexilMsgType_Message packet and free the message
     */
    void handleMessageMessage(const PlexilStringValueMsg* msgData);

    /**
     * @brief Queues the command in the message queue
     */
    void handleCommandSequence(const std::vector<const PlexilMsgBase*>& msgs);

    /**
     * @brief Process a TelemetryValues message sequence
     */
    void handleTelemetryValuesSequence(const std::vector<const PlexilMsgBase*>& msgs);

    /**
     * @brief Process a ReturnValues message sequence
     */
    void handleReturnValuesSequence(const std::vector<const PlexilMsgBase*>& msgs);

    /**
     * @brief Process a LookupNow. Ignores any lookups that are not defined in config
     */
    void handleLookupNow(const std::vector<const PlexilMsgBase*>& msgs);

    /**
     * @brief Helper function for converting message names into the proper format given the command type and a user-defined id.
     */
    std::string formatMessageName(const std::string& name, const std::string& command, int id);

    /**
     * @brief Helper function for converting message names into the propper format given the command type.
     */
    std::string formatMessageName(const std::string& name, const std::string& command);

    /**
     * @brief Helper function for converting message names into the propper format given the command type.
     */
    std::string formatMessageName(const char* name, const std::string& command);

    //
    // Static member functions
    //

    /**
     * @brief Returns true if the string starts with the prefix, false otherwise.
     */
    static bool hasPrefix(const std::string& s, const std::string& prefix);

    /**
     * @brief Given a sequence of messages, turn the trailers into a value for the Exec.
     */
    static Value parseReturnValue(const std::vector<const PlexilMsgBase*>& msgs);

    //
    // Private data types
    //

    //* brief Cache of message/command/lookup names we're actively listening for
    typedef std::map<std::string, State> ActiveListenerMap;

    //* brief Cache of command serials and their corresponding ack and return value variables
    typedef std::map<uint32_t, Command *> PendingCommandsMap;

    typedef std::map<State, Value> ExternalLookupMap;

    //* brief Class to receive messages from Ipc
    class MessageListener : public IpcMessageListener {
    public:
      MessageListener(IpcAdapter&);
      ~MessageListener();
      void ReceiveMessage(const std::vector<const PlexilMsgBase*>& msgs);
    private:
      IpcAdapter& m_adapter;
    };

    //
    // Member variables
    //

    //* @brief Interface with IPC
    IpcFacade m_ipcFacade;

    //* @brief Map of queues for holding complete messages and message handlers while they wait to be paired
    MessageQueueMap m_messageQueues;

    //* @brief Cache of ack and return value variables for commands we sent
    PendingCommandsMap m_pendingCommands;

    //* @brief Stores names of declared external lookups
    std::vector<std::string> m_externalLookupNames;

    //* @brief Map of external lookup values.
    ExternalLookupMap m_externalLookups;

    //* @brief Listener instance to receive messages.
    MessageListener m_listener;

    //* @brief Semaphore for return values from LookupNow
    ThreadSemaphore m_lookupSem;

    /**
     * @brief Mutex used to hold the processing of incoming return values while commands
     * are being sent and recorded.
     */
    ThreadMutex m_cmdMutex;

    //* @brief Place to store result of current pending LookupNow request
    Value m_pendingLookupResult;

    //* @brief Place to store state of pending LookupNow
    State m_pendingLookupState;

    //* @brief Serial # of current pending LookupNow request, or 0
    uint32_t m_pendingLookupSerial;
  };
}

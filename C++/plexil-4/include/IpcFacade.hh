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

#ifndef DEFINE_IPC_FACADE_H
#define DEFINE_IPC_FACADE_H

#include "ipc.h"
#include "ipc-data-formats.h"

#include "ConstantMacros.hh"
#include "RecursiveThreadMutex.hh"
#include "Value.hh"

#include <limits>
#include <list>
#include <map>
#include <string>
#include <vector>


namespace PLEXIL {

  class IpcMessageListener;

  /**
   * @brief Manages connection with IPC. This class is not thread-safe.
   */
  //TODO: Integrate all plexil type converting into this class.
  class IpcFacade {
  public:

    //
    // Class constants
    //

    DECLARE_STATIC_CLASS_CONST(uint32_t, ERROR_SERIAL, std::numeric_limits<uint32_t>::max())

    IpcFacade();
    ~IpcFacade();

    /**
     * @brief Returns the unique id of the IPC connection. This method will always return
     * a valid string, and it will never change, regardless of the state changes of the
     * connection.
     */
    const std::string& getUID();

    /**
     * @brief Connects to the Ipc server. This should be called before calling start().
     * If it is not, this method is called by start. If already initialized, this method
     * does nothing and returns IPC_OK.
     */
    IPC_RETURN_TYPE initialize(const char* taskName, const char* serverName);

    /**
     * @brief Starts the Ipc message handling thread. If not initialized, initialization occurs.
     * If Ipc is already started, this method does nothing and returns IPC_OK.
     */
    IPC_RETURN_TYPE start();
    /**
     * @brief Stops the Ipc message handling thread and removes all subscriptions. If
     * Ipc is not running, this method does nothing and returns IPC_OK.
     */
    void stop();
    /**
     * @brief Disconnects from the Ipc server. This puts Ipc back in its initial state before
     * being initialized. If not initialized, this method does nothing.
     */
    void shutdown();

    /**
     * @brief Adds the IpcMessageListener as a subscriber to all plexil
     * message types
     * @param handler The handler function to call for each message
     * @param clientData The object to associate as a subscriber
     */
    void subscribeAll(IpcMessageListener* listener);

    /**
     * @brief Adds the given IpcMessageListener as a subscriber to the given
     * PLEXIL message type
     * @param handler The handler function to call for each message
     * @param clientData The object to associate as a subscriber
     */
    void subscribe(IpcMessageListener* listener, PlexilMsgType type);

    /**
     * @brief Subscribes to all PLEXIL messages.
     * @return IPC_OK if all succeeded.
     */
    IPC_RETURN_TYPE subscribeToMsgs();

    /**
     * @brief Unsubscribes from all PLEXIL messages.
     * @return IPC_OK if all succeeded.
     */
    IPC_RETURN_TYPE unsubscribeFromMsgs();

    /**
     * @brief Removes all registered IpcMessageListeners to all plexil message subscriptions
     */
    void unsubscribeAll();

    /**
     * @brief Removes the given IpcMessageListener to all plexil message subscriptions
     * @param handler The handler function to be unsubscribed
     */
    void unsubscribeAll(IpcMessageListener* handler);

    /**
     * @brief publishes the given message via IPC
     * @param command The command string to send
     * @return IPC status
     */
    uint32_t publishMessage(std::string const &command);

    /**
     * @brief publishes the given command via IPC. This is equivalent to calling
     * sendCommand with an empty destination string.
     * Note: The response to this command may be received before
     * this method returns.
     * @param command The command name.
     * @param argsToDeliver The parameters for the command.
     * @return IPC status
     */
    uint32_t publishCommand(std::string const &command, std::vector<Value> const &argsToDeliver);

    /**
     * @brief Sends the given command to the given client ID via IPC. If the client ID is
     * an empty string, the command is published to all clients.
     * Note: The response to this command may be received before
     * this method returns.
     * @param command The command name.
     * @param dest The destination ID for this command.
     * @param argsToDeliver The parameters for the command.
     * @return IPC status
     */
    uint32_t sendCommand(std::string const &command,
                         std::string const &dest, 
                         std::vector<Value> const &argsToDeliver);

    /**
     * @brief publishes the given LookupNow call via IPC
     * @param lookup The state name
     * @param argsToDeliver Vector of state parameters.
     * @return IPC status
     */
    uint32_t publishLookupNow(std::string const &lookup, 
                              std::vector<Value> const &argsToDeliver);

    /**
     * @brief Sends the given LookupNow to the given client ID via IPC. If the client ID is
     * an empty string, the LookupNow is published to all clients.
     * @param lookup The state name.
     * @param dest The destination ID for this LookupNow
     * @param argsToDeliver Vector of state parameters.
     * @return IPC status
     */
    uint32_t sendLookupNow(std::string const &lookup,
                           std::string const &dest,
                           std::vector<Value> const &argsToDeliver);

    /**
     * @brief publishes the given return values via IPC
     * @param request_serial The serial of the request to which this is a response.
     * @param command The command name being responded to.
     * @param arg The value being returned.
     * @return IPC status
     */
    uint32_t publishReturnValues(uint32_t request_serial,
                                 std::string const &command,
                                 Value const &arg);

    /**
     * @brief publishes the given telemetry values via IPC
     * @param destName The destination ID for this message.
     * @param values Vector of PLEXIL Values to be published.
     * @return IPC status
     */
    uint32_t publishTelemetry(std::string const &destName, std::vector<Value> const &values);

    /**
     * @brief publishes the given telemetry value via IPC
     * @param nodeName The name of the node publishing the update.
     * @param update Vector of name, value pairs to publish.
     * @return IPC status
     */
    uint32_t publishUpdate(const std::string& nodeName,
                           std::vector<std::pair<std::string, Value> > const& update);
    /**
     * @brief Get next serial number
     */
    uint32_t getSerialNumber();

    /**
     * @brief Returns the error code of the last publish method call. If the last publish call returned
     * -1, this will return the appropriate error. Otherwise, it will return IPC_OK.
     */
    IPC_RETURN_TYPE getError();

  private:
    //
    // Private types
    //

    //* brief Structure for holding references to listeners registered through local instances
    typedef std::pair<uint16_t, IpcMessageListener*> LocalListenerRef;

    //* brief List of listeners registered through local instances
    typedef std::list<LocalListenerRef> LocalListenerList;

    //* brief List of listeners registered globally - for ListenerMap
    typedef std::list<IpcMessageListener*> ListenerList;

    //* brief Map of message types to lists of registered listeners
    typedef std::map<uint16_t, ListenerList> ListenerMap;

    //* brief Unique identifier of a message sequence
    typedef std::pair<std::string, uint32_t> IpcMessageId;

    //* brief Cache of not-yet-complete message sequences
    typedef std::map<IpcMessageId, std::vector<const PlexilMsgBase*> > IncompleteMessageMap;

    //* brief basic types of items to send
    enum BasicType { UNKNOWN, STRING, NUMERIC };

    //
    // Class constants
    //

    DECLARE_STATIC_CLASS_CONST(uint16_t, ALL_MSG_TYPE, std::numeric_limits<uint16_t>::max())

    /**
     * @brief IPC listener thread top level function to replace IPC_dispatch().
     */
    static void myIpcDispatch(void * this_as_void_ptr);

    /**
     * @brief Handler function as seen by IPC.
     */
    static void messageHandler(MSG_INSTANCE rawMsg, void * unmarshalledMsg, void * this_as_void_ptr);

    //
    // Implementation methods
    //

    /**
     * @brief Cache start message of a multi-message sequence
     */

    void cacheMessageLeader(const PlexilMsgBase* msgData);

    /**
     * @brief Cache following message of a multi-message sequence
     */

    void cacheMessageTrailer(const PlexilMsgBase* msgData);

    /**
     * @brief Deliver the given message to all listeners registered for it
     */
    void deliverMessage(const std::vector<const PlexilMsgBase*>& msgs);

    /**
     * @brief Helper function for sending a vector of parameters via IPC.
     * @param args The arguments to convert into messages and send
     * @param serial The serial to send along with each parameter. This should be the same serial as the header
     */
    IPC_RETURN_TYPE sendParameters(std::vector<Value> const &args, uint32_t serial);

    /**
     * @brief Helper function for sending a vector of parameters via IPC to a specific executive.
     * @param args The arguments to convert into messages and send
     * @param serial The serial to send along with each parameter. This should be the same serial as the header
     * @param dest The destination executive name
     */
    IPC_RETURN_TYPE sendParameters(std::vector<Value> const &args, uint32_t serial, std::string const &dest);

    /** 
     * @brief Helper function for sending a vector of pairs via IPC.
     * @param pairs The pairs to convert into messages and send
     * @param serial The serial to send along with each parameter.  This should be the same serial s the header.
     * 
     * @return The IPC error status.
     */
    IPC_RETURN_TYPE sendPairs(std::vector<std::pair<std::string, Value> > const& pairs,
                              uint32_t serial);
    /**
     * @brief Define all PLEXIL message types with Central. Also defines each PLEXIL message type with
     * the UID as a prefix for directed communication. Has no effect for any previously defined message types.
     * @return true if successful, false otherwise
     * @note Caller should ensure IPC_initialize() has been called first
     */
    bool definePlexilIPCMessageTypes();

    /**
     * @brief Set the error code of the last called IPC method.
     * @param error The error code of the last called IPC method.
     */
    void setError(IPC_RETURN_TYPE error);

    /**
     * @brief Registers the subscription of the given msg type/listener
     */
    void subscribeGlobal(const LocalListenerRef& listener);

    /**
     * @brief Unsubscribe the given listener from the listener map.
     * @return True if found and unsubscribed. False if not found.
     */
    bool unsubscribeGlobal(const LocalListenerRef& listener);

    //
    // Static utility methods
    //

    /**
     * @brief Initialize unique ID string
     */
    static std::string generateUID();

    /**
     * Unsubscribes from the given message on central. Wrapper for IPC_unsubscribe
     * @param msgName the name of the message to unsubscribe from
     * @param handler The handler to unsubscribe.
     */
    IPC_RETURN_TYPE unsubscribeCentral(const char *msgName, HANDLER_TYPE handler);

    /**
     * Subscribes to the given message on central. Wrapper for IPC_subscribeData
     * @param msgName the name of the message to unsubscribe from
     * @param handler The handler to unsubscribe.
     */
    IPC_RETURN_TYPE subscribeDataCentral(const char *msgName,
                                         HANDLER_DATA_TYPE handler);

    //* @brief Is the facade initialized?
    bool m_isInitialized;
    //* @brief Is the facade started?
    bool m_isStarted;
    //* @brief True if the dispatch thread should stop.
    bool m_stopDispatchThread;
    //* @brief Count of # of outgoing commands and requests, starting with 1
    //  @note Should only ever be 0 at initialization
    uint32_t m_nextSerial;
    //* @brief Unique ID of this adapter instance
    std::string m_myUID;
    //* @brief The error code of the last called IPC method.
    IPC_RETURN_TYPE m_error;
    //* @brief The listeners registered under this facade
    LocalListenerList m_localRegisteredHandlers;
    //* @brief The handle for the message thread
    pthread_t m_threadHandle;
    //* @brief The mutex used for synchronizing initialization/shutdown methods
    RecursiveThreadMutex m_mutex;
    //* @brief Cache of incomplete received message data
    IncompleteMessageMap m_incompletes;
    //* @brief Map of message type to list of listeners for that type
    ListenerMap m_registeredListeners;

  };

  /**
   * @brief Base class for receiving messages through Ipc. To use, create an instance of
   * IpcFacade, initialize and start it, and register an instance of the listener as a
   * recipient for the plexil message type you wish to handle.
   */
  class IpcMessageListener {
  public:
    virtual ~IpcMessageListener() {};
    virtual void ReceiveMessage(const std::vector<const PlexilMsgBase*>& msgs) = 0;
  };

  /**
   * @brief Utility function to create a value message from a PLEXIL Value.
   * @param val The Value to encode in the message.
   * @return Pointer to newly allocated IPC message.
   * @note Returns NULL for unimplemented/invalid Values.
   */
  extern struct PlexilMsgBase *constructPlexilValueMsg(Value const &val);

  /**
   * @brief Utility function to create a pair message from a string and a PLEXIL Value.
   * @param name The name of the pair.
   * @param val The Value to encode in the message.
   * @return Pointer to newly allocated IPC message.
   * @note Returns NULL for unimplemented/invalid Values.
   */
  extern struct PlexilMsgBase* constructPlexilPairMsg(std::string const& name,
                                                      Value const val);
  /**
   * @brief Utility function to extract the value from a value message.
   * @param msg Pointer to const IPC message.
   * @return The Value represented by the message.
   * @note The returned value will be unknown if the message is not a value message.
   */
  extern Value getPlexilMsgValue(struct PlexilMsgBase const *msg);

}

#endif /* DEFINE_IPC_FACADE_H */

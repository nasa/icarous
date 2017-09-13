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

#include "ConstantMacros.hh"
#include "InterfaceAdapter.hh"
#include "MessageQueueMap.hh"
#include "udp-utils.hh"
#include "ThreadSpawn.hh"

namespace PLEXIL
{

  class Parameter
  {
  public:
    std::string desc;           // optional parameter description
    std::string type;           // int|float|bool|string|int-array|float-array|string-array|bool-array
    unsigned int len;           // number of bytes for type (or array element)
    unsigned int elements;      // number of elements in the array (non-array types are 0 or 1?)
  };

  class UdpMessage
  {
  public:
    std::string name;                // the Plexil Command name
    std::string peer;                // peer to which to send
    std::vector<Parameter> parameters; // message value parameters
    void *self;                      // reference to the UdpAdapter for use in message decoding
    unsigned int len;                         // the length of the message in bytes
    unsigned int local_port;                  // local port on which to receive
    unsigned int peer_port;                   // port to which to send
    int sock;                        // socket to use -- only meaningful in call to waitForUdpMessage

    UdpMessage()
      : name(),
	peer(),
	parameters(),
	self(NULL),
	len(0),
	local_port(0),
	peer_port(0),
	sock(0)
    {
    }
  };

  typedef std::map<std::string, UdpMessage> MessageMap;
  typedef std::map<std::string, pthread_t> ThreadMap;
  typedef std::map<std::string, int> SocketMap;

  class UdpAdapter : public InterfaceAdapter
  {
  public:
    // Static Class Constants
    DECLARE_STATIC_CLASS_CONST(std::string, COMMAND_PREFIX, "__COMMAND__")
    DECLARE_STATIC_CLASS_CONST(std::string, PARAM_PREFIX, "__PARAMETER__")
    DECLARE_STATIC_CLASS_CONST(std::string, SEND_MESSAGE_COMMAND, "SendMessage")
    DECLARE_STATIC_CLASS_CONST(std::string, RECEIVE_COMMAND_COMMAND, "ReceiveCommand")
    DECLARE_STATIC_CLASS_CONST(std::string, GET_PARAMETER_COMMAND, "GetParameter")
    DECLARE_STATIC_CLASS_CONST(std::string, SEND_RETURN_VALUE_COMMAND, "SendReturnValue")
    //DECLARE_STATIC_CLASS_CONST(std::string, MESSAGE_PREFIX, "__MESSAGE__")
    //DECLARE_STATIC_CLASS_CONST(std::string, LOOKUP_PREFIX, "__LOOKUP__")
    //DECLARE_STATIC_CLASS_CONST(std::string, LOOKUP_ON_CHANGE_PREFIX, "__LOOKUP_ON_CHANGE__")
    //DECLARE_STATIC_CLASS_CONST(std::string, SERIAL_UID_SEPARATOR, ":")
    //DECLARE_STATIC_CLASS_CONST(std::string, SEND_UDP_MESSAGE_COMMAND, "SendUdpMessage")
    //DECLARE_STATIC_CLASS_CONST(std::string, RECEIVE_UDP_MESSAGE_COMMAND, "ReceiveUdpMessage")
    //DECLARE_STATIC_CLASS_CONST(std::string, RECEIVE_MESSAGE_COMMAND, "ReceiveMessage")
    //DECLARE_STATIC_CLASS_CONST(std::string, UPDATE_LOOKUP_COMMAND, "UpdateLookup")

    // Constructor/Destructor
    UdpAdapter(AdapterExecInterface& execInterface);
    UdpAdapter(AdapterExecInterface& execInterface, pugi::xml_node const xml);
    virtual ~UdpAdapter();

    // InterfaceAdapter API
    bool initialize();
    bool start();
    bool stop();
    bool reset();
    bool shutdown();
    void lookupNow(State const &state, StateCacheEntry &cacheEntry);
    void subscribe(const State& state);
    void unsubscribe(const State& state);
    void sendPlannerUpdate(Update *upd);
    // Executes the command.
    void executeCommand(Command *cmd);
    // Abort the given command.  Store the abort-complete into ack
    void invokeAbort(Command *cmd);

    ThreadMutex m_cmdMutex;

    // Somewhere to hang the messages, default ports and peers, threads and sockets
    unsigned int m_default_local_port;
    unsigned int m_default_peer_port;
    std::string m_default_peer;
    MessageMap m_messages;
    MessageQueueMap m_messageQueues;
    ThreadMap m_activeThreads;
    SocketMap m_activeSockets;
    bool m_debug; // Show debugging output

  private:
    // Deliberately unimplemented
    UdpAdapter();
    UdpAdapter(const UdpAdapter &);
    UdpAdapter& operator=(const UdpAdapter &);

    //
    // Implementation methods
    //
    void executeSendUdpMessageCommand(Command *cmd);
    void executeReceiveUdpCommand(Command *cmd);
    void executeSendMessageCommand(Command *cmd);
    void executeReceiveCommandCommand(Command *cmd);
    void executeGetParameterCommand(Command *cmd);
    void executeSendReturnValueCommand(Command *cmd);
    void executeDefaultCommand(Command *cmd);

    //
    // XML Support
    //
    void parseXmlMessageDefinitions(pugi::xml_node const xml);
    void printMessageDefinitions();
    int buildUdpBuffer(unsigned char* buffer,
                       const UdpMessage& msg,
                       const std::vector<Value>& args,
                       bool skip_arg=false,
                       bool debug=false);
    void printMessageContent(const std::string& name, const std::vector<Value>& args);
    int sendUdpMessage(const unsigned char* buffer, const UdpMessage& msg, bool debug=false);
    int startUdpMessageReceiver(const std::string& name, Command *cmd);
    static void* waitForUdpMessage(UdpMessage* msg);
    int handleUdpMessage(const UdpMessage* msg, const unsigned char* buffer, bool debug=false);
    std::string formatMessageName(const std::string& name, const std::string& command, int id);
    std::string formatMessageName(const std::string& name, const std::string& command);
    std::string formatMessageName(const char* name, const std::string& command, int id);
  };
}

extern "C"
{
  void initUdpAdapter();
}

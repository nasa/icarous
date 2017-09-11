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

#include "IpcFacade.hh"

#include "ArrayImpl.hh"
#include "CommandHandle.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ThreadSpawn.hh"

#include <map>

#include <cstring>

// ooid classes
#include "uuid_gen.h"
#include "system/devrand.h"

namespace PLEXIL 
{

  // Message type name cache
  // Not clear whether string concatenation or map lookup is more expensive.

  typedef std::pair<std::string, std::string> MessageFormatKey;
  typedef std::map<MessageFormatKey, std::string> MessageFormatMap;
  static MessageFormatMap messageFormatMap;

  /**
   * Returns a constant character string pointer for the formatted message type,
   *  given the basic message type and destination ID.
   * @param msgName The name of the message type
   * @param destId The destination ID for the message
   * @return Character string pointer.
   */
  static char const *formatMsgName(const std::string& msgName, const std::string& destId) {
    MessageFormatKey const key(msgName, destId);
    MessageFormatMap::iterator it = messageFormatMap.find(key);
    if (it != messageFormatMap.end())
      return it->second.c_str();
    it = messageFormatMap.insert(it,
                                 std::pair<MessageFormatKey, std::string>(key, destId + msgName));
    return it->second.c_str();
  }

  /**
   * @brief Return the message format string corresponding to the message type.
   * @param typ The message type.
   * @return Const char pointer to the message format name.
   */
  static inline const char* msgFormatForType(const PlexilMsgType typ)
  {
    switch (typ) {
    case PlexilMsgType_NotifyExec:
    case PlexilMsgType_UnknownValue:

      return MSG_BASE;
      break;

    case PlexilMsgType_AddPlan:
    case PlexilMsgType_AddPlanFile:
    case PlexilMsgType_AddLibrary:
    case PlexilMsgType_AddLibraryFile:
    case PlexilMsgType_Command:
    case PlexilMsgType_Message:
    case PlexilMsgType_LookupNow:
    case PlexilMsgType_PlannerUpdate:
    case PlexilMsgType_StringValue:
    case PlexilMsgType_TelemetryValues:
      return STRING_VALUE_MSG;
      break;

    case PlexilMsgType_ReturnValues:
      return RETURN_VALUE_MSG;
      break;

    case PlexilMsgType_BooleanValue:
      return BOOLEAN_VALUE_MSG;
      break;

    case PlexilMsgType_IntegerValue:
      return INTEGER_VALUE_MSG;
      break;

    case PlexilMsgType_RealValue:
      return REAL_VALUE_MSG;
      break;
              
    case PlexilMsgType_BooleanArray:
      return BOOLEAN_ARRAY_MSG;
      break;
              
    case PlexilMsgType_IntegerArray:
      return INTEGER_ARRAY_MSG;
      break;
              
    case PlexilMsgType_RealArray:
      return REAL_ARRAY_MSG;
      break;

    case PlexilMsgType_StringArray:
      return STRING_ARRAY_MSG;
      break;

    case PlexilMsgType_PairBoolean:
      return BOOLEAN_PAIR_MSG;
      break;

    case PlexilMsgType_PairInteger:
      return INTEGER_PAIR_MSG;
      break;

    case PlexilMsgType_PairReal:
      return REAL_PAIR_MSG;
      break;

    case PlexilMsgType_PairString:
      return STRING_PAIR_MSG;
      break;

    default:
      return NULL;
      break;
    }
  }

  /**
   * @brief Utility function to create a value message from a PLEXIL Value.
   * @param val The Value to encode in the message.
   * @return Pointer to newly allocated IPC message.
   * @note Returns NULL for unimplemented/invalid Values.
   */
  struct PlexilMsgBase *constructPlexilValueMsg(Value const &val)
  {
    if (val.isKnown())
      switch (val.valueType()) {
      case BOOLEAN_TYPE: {
        bool b;
        val.getValue(b);
        struct PlexilBooleanValueMsg *boolMsg = new struct PlexilBooleanValueMsg;
        boolMsg->header.msgType = PlexilMsgType_BooleanValue;
        boolMsg->boolValue = (unsigned char) b;
        debugMsg("constructPlexilValueMsg", "Boolean value is " << b);
        return (struct PlexilMsgBase *) boolMsg;
      }

      case INTEGER_TYPE: {
        struct PlexilIntegerValueMsg *intMsg = new struct PlexilIntegerValueMsg;
        intMsg->header.msgType = PlexilMsgType_IntegerValue;
        val.getValue(intMsg->intValue);
        debugMsg("constructPlexilValueMsg", "Integer value is " << intMsg->intValue);
        return (struct PlexilMsgBase *) intMsg;
      }

      case REAL_TYPE: {
        struct PlexilRealValueMsg *realMsg = new struct PlexilRealValueMsg;
        realMsg->header.msgType = PlexilMsgType_RealValue;
        val.getValue(realMsg->doubleValue);
        debugMsg("constructPlexilValueMsg", "Real value is " << realMsg->doubleValue);
        return (struct PlexilMsgBase *) realMsg;
      }

      case STRING_TYPE: {
        std::string const *sp;
        val.getValuePointer(sp);
        struct PlexilStringValueMsg *stringMsg = new struct PlexilStringValueMsg;
        stringMsg->header.msgType = PlexilMsgType_StringValue;
        stringMsg->stringValue = sp->c_str();
        debugMsg("constructPlexilValueMsg", "String value is \"" << stringMsg->stringValue << "\"");
        return (struct PlexilMsgBase *) stringMsg;
      }

      case COMMAND_HANDLE_TYPE: {
        uint16_t handle;
        val.getValue(handle);
        struct PlexilCommandHandleValueMsg *handleMsg = new struct PlexilCommandHandleValueMsg;
        handleMsg->header.msgType = PlexilMsgType_CommandHandleValue;
        handleMsg->commandHandleValue = handle;
        return (struct PlexilMsgBase *) handleMsg;
      }

      case BOOLEAN_ARRAY_TYPE: {
        BooleanArray const *ba = NULL;
        val.getValuePointer(ba);
        assertTrue_1(ba);
        size_t size = ba->size();
        unsigned char *bools = new unsigned char[size];
        for (size_t i = 0; i < size; i++) {
          bool b;
          assertTrue_2(ba->getElement(i, b), "Boolean array element is UNKNOWN");
          bools[i] = (unsigned char) b;
        }
        struct PlexilBooleanArrayMsg* boolArrayMsg = new PlexilBooleanArrayMsg();
        boolArrayMsg->header.msgType = PlexilMsgType_BooleanArray;
        boolArrayMsg->arraySize = size;
        boolArrayMsg->boolArray = bools;
        debugMsg("constructPlexilValueMsg",
                 "Boolean array size " << boolArrayMsg->arraySize);
        debugStmt("constructPlexilValueMsg",
                  for (size_t i = 0; i < size; ++i) 
                    std::cout << "elt " << i << ": " << (bool) boolArrayMsg->boolArray[i] << '\n');
        debugMsg("constructPlexilValueMsg", "First parameter of Boolean array is " << (bool) boolArrayMsg->boolArray[0]);
        return (struct PlexilMsgBase *) boolArrayMsg;
      }

      case INTEGER_ARRAY_TYPE: {
        IntegerArray const *ia = NULL;
        val.getValuePointer(ia);
        assertTrue_1(ia);
        size_t size = ia->size();
        int32_t *nums = new int32_t[size];
        for (size_t i = 0; i < size; i++) 
          assertTrue_2(ia->getElement(i, nums[i]), "Integer array element is UNKNOWN");
        struct PlexilIntegerArrayMsg* intArrayMsg = new PlexilIntegerArrayMsg();
        intArrayMsg->header.msgType = PlexilMsgType_IntegerArray;
        intArrayMsg->arraySize = size;
        intArrayMsg->intArray = nums;
        debugMsg("constructPlexilValueMsg",
                 "Integer array size " << intArrayMsg->arraySize);
        debugStmt("constructPlexilValueMsg",
                  for (size_t i = 0; i < size; ++i) 
                    std::cout << "elt " << i << ": " << intArrayMsg->intArray[i] << '\n');
        return (struct PlexilMsgBase*) intArrayMsg;
      }

      case REAL_ARRAY_TYPE: {
        RealArray const *ra = NULL;
        val.getValuePointer(ra);
        assertTrue_1(ra);
        size_t size = ra->size();
        double *nums = new double[size];
        for (size_t i = 0; i < size; i++) 
          assertTrue_1(ra->getElement(i, nums[i]));
        struct PlexilRealArrayMsg* realArrayMsg = new PlexilRealArrayMsg();
        realArrayMsg->header.msgType = PlexilMsgType_RealArray;
        realArrayMsg->arraySize = size;
        realArrayMsg->doubleArray = nums;
        debugMsg("constructPlexilValueMsg",
                 "Real array size " << realArrayMsg->arraySize);
        debugStmt("constructPlexilValueMsg",
                  for (size_t i = 0; i < size; ++i) 
                    std::cout << "elt " << i << ": " << realArrayMsg->doubleArray[i] << '\n');
        return (struct PlexilMsgBase *) realArrayMsg;
      }

      case STRING_ARRAY_TYPE: {
        StringArray const *sa = NULL;
        val.getValuePointer(sa);
        assertTrue_1(sa);
        size_t size = sa->size();
        const char **strings = new const char*[size];
        for (size_t i = 0; i < size; i++) {
          std::string const *temp = NULL;
          assertTrue_1(sa->getElementPointer(i, temp));
          strings[i] = temp->c_str();
        }
        struct PlexilStringArrayMsg* strArrayMsg = new PlexilStringArrayMsg();
        strArrayMsg->header.msgType = PlexilMsgType_StringArray;
        strArrayMsg->arraySize = size;
        strArrayMsg->stringArray = strings;
        debugMsg("constructPlexilValueMsg",
                 "String array size " << strArrayMsg->arraySize);
        debugStmt("constructPlexilValueMsg",
                  for (size_t i = 0; i < size; ++i) 
                    std::cout << "elt " << i << ": " << strArrayMsg->stringArray[i] << '\n');
        return (struct PlexilMsgBase *) strArrayMsg;
      }

      default:
        assertTrue_2(ALWAYS_FAIL, "Invalid or unimplemented PLEXIL data type");
        return NULL;
      }
    else {
      // Unknown
      struct PlexilUnknownValueMsg *unkMsg = new struct PlexilUnknownValueMsg;
      unkMsg->header.msgType = PlexilMsgType_UnknownValue;
      debugMsg("constructPlexilValueMsg", " Unknown value");
      return (struct PlexilMsgBase *) unkMsg;
    }
  }

  struct PlexilMsgBase* constructPlexilPairMsg(std::string const& name,
                                               Value const val) {
  PlexilMsgBase* result = NULL;
  if(val.isKnown()) {
    switch(val.valueType()) {
      case BOOLEAN_TYPE: {
        bool b;
        val.getValue(b);
        struct BooleanPair* boolMsg = new BooleanPair;
        boolMsg->pairBoolValue = b;
        debugMsg("constructPlexilPairMsg",
                 "(" << name << ", " << boolMsg->pairBoolValue << ")");
        result = reinterpret_cast<PlexilMsgBase*>(boolMsg);
        result->msgType = PlexilMsgType_PairBoolean;
        break;
      }
      case INTEGER_TYPE: {
        struct IntegerPair* intMsg = new IntegerPair;
        val.getValue(intMsg->pairIntValue);
        debugMsg("constructPlexilPairMsg",
                 "(" << name << ", " << intMsg->pairIntValue << ")");
        result = reinterpret_cast<PlexilMsgBase*>(intMsg);
        result->msgType = PlexilMsgType_PairInteger;
        break;
      }
      case REAL_TYPE: {
        struct RealPair* realMsg = new RealPair;
        val.getValue(realMsg->pairDoubleValue);
        debugMsg("constructPlexilPairMsg",
                 "(" << name << ", " << realMsg->pairDoubleValue << ")");
        result = reinterpret_cast<PlexilMsgBase*>(realMsg);
        result->msgType = PlexilMsgType_PairReal;
        break;
      }
      case STRING_TYPE: {
        std::string const* sp;
        val.getValuePointer(sp);
        struct StringPair* strMsg = new StringPair;
        strMsg->pairStringValue = sp->c_str();
        debugMsg("constructPlexilPairMsg",
                 "(" << name << ", " << strMsg->pairStringValue << ")");
        result = reinterpret_cast<PlexilMsgBase*>(strMsg);
        result->msgType = PlexilMsgType_PairString;
        break;
      }
      default:        
        break;
    }
    if(result != NULL)
      reinterpret_cast<PairHeader*>(result)->pairName = name.c_str();
  }
  else {
    debugMsg("constructPlexilPairMsg", " Unknown value.");
  }
  return result;
}

  /**
   * @brief Utility function to extract the value from a value message.
   * @param msg Pointer to const IPC message.
   * @return The Value represented by the message.
   * @note The returned value will be unknown if the message is not a value message.
   */
  Value getPlexilMsgValue(struct PlexilMsgBase const *msg)
  {
    switch ((PlexilMsgType) msg->msgType) {
    case PlexilMsgType_CommandHandleValue: {
      PlexilCommandHandleValueMsg const *param = reinterpret_cast<const struct PlexilCommandHandleValueMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received CommandHandle " << commandHandleValueName(param->commandHandleValue));
      return Value(param->commandHandleValue, COMMAND_HANDLE_TYPE);
    }

    case PlexilMsgType_BooleanValue: {
      const struct PlexilBooleanValueMsg* param = reinterpret_cast<const struct PlexilBooleanValueMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received Boolean " << param->boolValue);
      return Value((bool) param->boolValue);
    }

    case PlexilMsgType_IntegerValue: {
      const struct PlexilIntegerValueMsg* param = reinterpret_cast<const struct PlexilIntegerValueMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received Integer " << param->intValue);
      return Value(param->intValue);
    }

    case PlexilMsgType_RealValue: {
      const struct PlexilRealValueMsg* param = reinterpret_cast<const struct PlexilRealValueMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received Real " << param->doubleValue);
      return Value(param->doubleValue);
    }

    case PlexilMsgType_StringValue: {
      const struct PlexilStringValueMsg* param = reinterpret_cast<const struct PlexilStringValueMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received String " << param->stringValue);
      return Value(param->stringValue);
    }

    case PlexilMsgType_BooleanArray: {
      const struct PlexilBooleanArrayMsg* param = reinterpret_cast<const struct PlexilBooleanArrayMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received Boolean array of size " << param->arraySize);
      debugStmt("getPlexilMsgValue",
                for (size_t j = 0; j < param->arraySize; j++)
                  std::cout << " elt " << j << ": " << (bool) param->boolArray[j] << '\n');
      BooleanArray array(param->arraySize);
      for (size_t j = 0; j < param->arraySize; j++)
        array.setElement(j, (bool) param->boolArray[j]);
      return Value(array);
    }

    case PlexilMsgType_IntegerArray: {
      const struct PlexilIntegerArrayMsg* param = reinterpret_cast<const struct PlexilIntegerArrayMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received Integer array of size " << param->arraySize);
      debugStmt("getPlexilMsgValue",
                for (size_t j = 0; j < param->arraySize; j++)
                  std::cout << " elt " << j << ": " << param->intArray[j] << '\n');
      IntegerArray array(param->arraySize);
      for (size_t j = 0; j < param->arraySize; j++)
        array.setElement(j, param->intArray[j]);
      return Value(array);
    }

    case PlexilMsgType_RealArray: {
      const struct PlexilRealArrayMsg* param = reinterpret_cast<const struct PlexilRealArrayMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received Real array of size " << param->arraySize);
      debugStmt("getPlexilMsgValue",
                for (size_t j = 0; j < param->arraySize; j++)
                  std::cout << " elt " << j << ": " << param->doubleArray[j] << '\n');
      RealArray array(param->arraySize);
      for (size_t j = 0; j < param->arraySize; j++)
        array.setElement(j, param->doubleArray[j]);
      return Value(array);
    }

    case PlexilMsgType_StringArray: {
      const struct PlexilStringArrayMsg* param = reinterpret_cast<const struct PlexilStringArrayMsg*> (msg);
      debugMsg("getPlexilMsgValue",
               " received String array of size " << param->arraySize);
      debugStmt("getPlexilMsgValue",
                for (size_t j = 0; j < param->arraySize; j++)
                  std::cout << " elt " << j << ": " << param->stringArray[j] << '\n');
      StringArray array(param->arraySize);
      for (size_t j = 0; j < param->arraySize; j++)
        array.setElement(j, std::string(param->stringArray[j]));
      return Value(array);
    }

    default:
      // TODO: handle error more gracefully
      assertTrue_2(ALWAYS_FAIL, "getPlexilMsgValue: invalid or unimplemented message type");
      // fall thru...

    case PlexilMsgType_UnknownValue:
      return Value();
    }
  }

  IpcFacade::IpcFacade() :
    m_isInitialized(false),
    m_isStarted(false),
    m_stopDispatchThread(false),
    m_nextSerial(1),
    m_myUID(generateUID())
  {
    debugMsg("IpcFacade", " constructor");
  }

  IpcFacade::~IpcFacade() {
    debugMsg("IpcFacade", " destructor");
    if (m_isStarted) {
      stop();
    }
    if (m_isInitialized) {
      shutdown();
    }
    // *** why is this necessary?? ***
    if (m_mutex.isLockedByCurrentThread()) {
      debugMsg("IpcFacade", " destructor: unlocking mutex");
      m_mutex.unlock();
    }
  }

  const std::string& IpcFacade::getUID() {
    return m_myUID;
  }
  /**
   * @brief Connects to the Ipc server. This should be called before calling start().
   * If it is not, this method is called by start. If already initialized, this method
   * does nothing and returns IPC_OK.
   * @param taskName If null, the current UID of the IpcFacade is used as the task name.
   */
  IPC_RETURN_TYPE IpcFacade::initialize(const char* taskName, const char* serverName) {
    if (m_isInitialized) {
      debugMsg("IpcFacade:initialize", " already done, returning");
      return IPC_OK;
    }

    if (taskName != NULL && taskName != m_myUID)
      m_myUID = taskName;

    IPC_RETURN_TYPE result = IPC_OK;
    debugMsg("IpcFacade:initialize", " locking mutex");
    RTMutexGuard guard(m_mutex);

    // perform global initialization
    // Initialize IPC
    // possibly redundant, but always safe
    debugMsg("IpcFacade:initialize", " calling IPC_initialize()");
    result = IPC_initialize();
    if (result != IPC_OK) {
      debugMsg("IpcFacade:initialize", " IPC_initialize() failed, IPC_errno = " << IPC_errno);
      return result;
    }

    // Connect to central
    debugMsg("IpcFacade:initialize", " calling IPC_connectModule()");
    result = IPC_connectModule(m_myUID.c_str(), serverName);
    if (result != IPC_OK)
      {
        debugMsg("IpcFacade:initialize", " IPC_connectModule() failed, IPC_errno = " << IPC_errno);
        return result;
      }

    // Define messages
    debugMsg("IpcFacade:initialize", " defining message types");
    if (definePlexilIPCMessageTypes())
      {
        result = IPC_OK;
      }
    else
      {
        condDebugMsg(result == IPC_OK, "IpcFacade:initialize", " defining message types failed");
        result = IPC_Error;
      }
    if (result == IPC_OK) {
      m_isInitialized = true;
      debugMsg("IpcFacade:initialize", " succeeded");
    }
    return result;
  }

  /**
   * @brief Initializes and starts the Ipc message handling thread. If Ipc is already
   * started, this method does nothing and returns IPC_OK.
   * @return IPC_Error if the dispatch thread is not started correctly, IPC_OK otherwise
   */
  IPC_RETURN_TYPE IpcFacade::start() {
    IPC_RETURN_TYPE result = IPC_OK;
    if (!m_isInitialized || !IPC_isConnected())
      result = IPC_Error;
    debugMsg("IpcFacade:start", " locking mutex");
    RTMutexGuard guard(m_mutex);
    //perform only when this instance is the only started instance of the class
    if (result == IPC_OK && !m_isStarted) {
      // Subscribe to messages
      debugMsg("IpcFacade:start", " subscribing to messages");
      subscribeToMsgs();

      // Spawn message thread AFTER all subscribes complete
      // Running thread in parallel with subscriptions resulted in deadlocks
      debugMsg("IpcFacade:start", " spawning IPC dispatch thread");
      if (threadSpawn((THREAD_FUNC_PTR) myIpcDispatch, this, m_threadHandle)) {
        debugMsg("IpcFacade:start", " succeeded");
        m_isStarted = true;
      }
    }
    return result;
  }

  /**
   * @brief Removes all subscriptions registered by this IpcFacade. If
   * this is the only running instance of IpcFacade, stops the Ipc message
   * handling thread. If Ipc is not running, this method does nothing and returns IPC_OK.
   */
  void IpcFacade::stop() {
    if (!m_isStarted) {
      return;
    }
    debugMsg("IpcFacade:stop", " locking mutex");
    RTMutexGuard guard(m_mutex);

    // Cancel IPC dispatch thread first to prevent deadlocks
    debugMsg("IpcFacade:stop", " cancelling dispatch thread");
    m_stopDispatchThread = true;
    int myErrno = pthread_join(m_threadHandle, NULL);
    if (myErrno != 0) {
      debugMsg("IpcUtil:stop", "Error in pthread_join; errno = " << myErrno);
    }

    debugMsg("IpcFacade:stop", " unsubscribing from messages");
    unsubscribeFromMsgs();

    m_isStarted = false;
    unsubscribeAll();

  }

  /**
   * @brief Disconnects from the Ipc server. This puts Ipc back in its initial state before
   * being initialized.
   */
  void IpcFacade::shutdown() {
    debugMsg("IpcFacade::shutdown", "locking mutex");
    RTMutexGuard guard(m_mutex);
    if (m_isInitialized) {
      if (m_isStarted) {
        stop();
      }

      // Disconnect from central
      IPC_disconnect();
    }
    m_isInitialized = false;
  }

  void IpcFacade::subscribeAll(IpcMessageListener* listener) {
    m_localRegisteredHandlers.push_back(LocalListenerRef(ALL_MSG_TYPE(), listener));
    subscribeGlobal(LocalListenerRef(ALL_MSG_TYPE(), listener));
  }

  void IpcFacade::subscribe(IpcMessageListener* listener, PlexilMsgType type) {
    m_localRegisteredHandlers.push_back(LocalListenerRef((uint16_t) type, listener));
    subscribeGlobal(LocalListenerRef((uint16_t) type, listener));
  }

  IPC_RETURN_TYPE IpcFacade::subscribeToMsgs()
  {
    IPC_RETURN_TYPE status;
    status = subscribeDataCentral(MSG_BASE, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << MSG_BASE << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(RETURN_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << RETURN_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(BOOLEAN_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << BOOLEAN_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(INTEGER_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << INTEGER_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(REAL_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << REAL_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(STRING_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << STRING_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(BOOLEAN_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << BOOLEAN_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(INTEGER_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << INTEGER_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(REAL_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << REAL_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(STRING_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << STRING_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(BOOLEAN_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << BOOLEAN_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(INTEGER_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << INTEGER_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(REAL_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << REAL_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = subscribeDataCentral(STRING_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK, "IpcFacade::start: Error subscribing to " << STRING_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    return status;
  }

  void IpcFacade::unsubscribeAll() {
    debugMsg("IpcFacade:unsubscribeAll", " entered");
    while (!m_localRegisteredHandlers.empty())
      unsubscribeAll(m_localRegisteredHandlers.front().second);
    debugMsg("IpcFacade:unsubscribeAll", " succeeded");
  }

  void IpcFacade::unsubscribeAll(IpcMessageListener* listener) {
    //prevent modification and access while removing
    RTMutexGuard guard(m_mutex);
    bool removed = false;
    for (LocalListenerList::iterator it = m_localRegisteredHandlers.begin();
         !removed && it != m_localRegisteredHandlers.end();
         it++) {
      if ((*it).second == listener) {
        unsubscribeGlobal(*it);
        m_localRegisteredHandlers.erase(it);
        removed = true;
      }
    }
  }

  IPC_RETURN_TYPE IpcFacade::unsubscribeFromMsgs()
  {
    IPC_RETURN_TYPE status;
    status = IPC_unsubscribe(MSG_BASE, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << MSG_BASE << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(RETURN_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << RETURN_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(BOOLEAN_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << BOOLEAN_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(INTEGER_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << INTEGER_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(REAL_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << REAL_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(STRING_VALUE_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << STRING_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(BOOLEAN_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << BOOLEAN_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(INTEGER_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << INTEGER_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(REAL_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << REAL_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(STRING_ARRAY_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << STRING_ARRAY_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(BOOLEAN_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << BOOLEAN_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(INTEGER_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << INTEGER_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(REAL_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << REAL_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(STRING_PAIR_MSG, messageHandler);
    assertTrueMsg(status == IPC_OK,
                  "IpcFacade: Error unsubscribing from " << STRING_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    return status;
  }

  /**
   * @brief publishes the given message via IPC
   * @param command The command string to send
   */
  uint32_t IpcFacade::publishMessage(std::string const &command) {
    assertTrue_2(m_isStarted, "publishMessage called before started");
    struct PlexilStringValueMsg packet = { { PlexilMsgType_Message, 0, getSerialNumber(), m_myUID.c_str() }, command.c_str() };
    return IPC_publishData(STRING_VALUE_MSG, (void *) &packet);
  }

  uint32_t IpcFacade::publishCommand(std::string const &command, std::vector<Value> const &argsToDeliver) {
    return sendCommand(command, "", argsToDeliver);
  }

  uint32_t IpcFacade::sendCommand(std::string const &command, std::string const &dest, std::vector<Value> const &argsToDeliver) {
    assertTrue_2(m_isStarted, "publishCommand called before started");
    uint32_t serial = getSerialNumber();
    struct PlexilStringValueMsg cmdPacket = { { PlexilMsgType_Command, (uint16_t) argsToDeliver.size(), serial, m_myUID.c_str() }, command.c_str() };
    IPC_RETURN_TYPE result = IPC_publishData(formatMsgName(STRING_VALUE_MSG, dest), (void *) &cmdPacket);
    if (result == IPC_OK) {
      result = sendParameters(argsToDeliver, serial);
      debugMsg("IpcFacade:publishCommand", "Command " << command << " published with serial " << serial);
    }
    setError(result);
    return result == IPC_OK ? serial : ERROR_SERIAL();
  }

  uint32_t IpcFacade::publishLookupNow(std::string const &lookup, std::vector<Value> const &argsToDeliver) {
    return sendLookupNow(lookup, "", argsToDeliver);
  }

  uint32_t IpcFacade::sendLookupNow(std::string const &lookup, std::string const &dest, std::vector<Value> const &argsToDeliver) {
    // Construct the messages
    // Leader
    uint32_t serial = getSerialNumber();
    struct PlexilStringValueMsg leader = { { PlexilMsgType_LookupNow, (uint16_t) argsToDeliver.size(), serial, m_myUID.c_str() }, lookup.c_str() };

    IPC_RETURN_TYPE result =
      IPC_publishData(formatMsgName(STRING_VALUE_MSG, dest), (void *) &leader);
    if (result == IPC_OK) {
      result = sendParameters(argsToDeliver, serial);
    }
    setError(result);
    return result == IPC_OK ? serial : ERROR_SERIAL();
  }

  uint32_t IpcFacade::publishReturnValues(uint32_t request_serial,
                                          std::string const &request_uid,
                                          Value const &arg)
  {
    assertTrue_2(m_isStarted, "publishReturnValues called before started");
    uint32_t serial = getSerialNumber();
    struct PlexilReturnValuesMsg packet = { { PlexilMsgType_ReturnValues, 1, serial, m_myUID.c_str() }, request_serial, request_uid.c_str() };
    IPC_RETURN_TYPE result =
      IPC_publishData(formatMsgName(RETURN_VALUE_MSG, request_uid), (void *) &packet);
    if (result == IPC_OK) {
      result = sendParameters(std::vector<Value>(1, arg), serial, request_uid);
    }
    setError(result);
    return result == IPC_OK ? serial : ERROR_SERIAL();
  }

  IPC_RETURN_TYPE IpcFacade::getError() {
    return m_error;
  }

  void IpcFacade::setError(IPC_RETURN_TYPE error) {
    m_error = error;
  }

  uint32_t IpcFacade::publishTelemetry(const std::string& destName, std::vector<Value> const &values) {
    // Telemetry values message
    debugMsg("IpcFacade:publishTelemetry",
             " sending telemetry message for \"" << destName << "\"");
    // *** FIXME: Memory leak? ***
    PlexilStringValueMsg* tvMsg = new PlexilStringValueMsg();
    tvMsg->header.msgType = (uint16_t) PlexilMsgType_TelemetryValues;
    tvMsg->stringValue = destName.c_str();

    tvMsg->header.count = values.size();
    uint32_t leaderSerial = getSerialNumber();
    tvMsg->header.serial = leaderSerial;
    tvMsg->header.senderUID = m_myUID.c_str();
    IPC_RETURN_TYPE status = IPC_publishData(STRING_VALUE_MSG, (void *) tvMsg);
    if (status == IPC_OK) {
      status = sendParameters(values, leaderSerial);
    }
    setError(status);
    return status == IPC_OK ? leaderSerial : ERROR_SERIAL();
  }
  
uint32_t IpcFacade::publishUpdate(const std::string& nodeName, std::vector<std::pair<std::string, Value> > const& update) {
  debugMsg("IpcFacade:publishUpdate",
           " sending planner update for \"" << nodeName << "\"");
  uint32_t serial = getSerialNumber();
  struct PlexilStringValueMsg updatePacket =
    {{PlexilMsgType_PlannerUpdate, (uint16_t) update.size(), serial, m_myUID.c_str()},
     nodeName.c_str()};
  IPC_RETURN_TYPE status = IPC_publishData(STRING_VALUE_MSG, (void*) &updatePacket);
  if(status == IPC_OK) {
    status = sendPairs(update, serial);
  }
  setError(status);
  return status == IPC_OK ? serial : ERROR_SERIAL();
}

  /**
   * @brief Helper function for sending a vector of parameters via IPC.
   * @param args The arguments to convert into messages and send
   * @param serial The serial to send along with each parameter. This should be the same serial as the header
   */
  IPC_RETURN_TYPE IpcFacade::sendParameters(std::vector<Value> const &args, uint32_t serial) {
    return sendParameters(args, serial, "");
  }

  /**
   * @brief Helper function for sending a vector of parameters via IPC to a specific executive.
   * @param args The arguments to convert into messages and send
   * @param serial The serial to send along with each parameter. This should be the same serial as the header
   * @param dest The destination executive name. If dest is an empty string, parameters are broadcast to
   * all executives
   */
  IPC_RETURN_TYPE IpcFacade::sendParameters(std::vector<Value> const &args, uint32_t serial, std::string const &dest) {
    size_t nParams = args.size();
    // Construct parameter messages
    PlexilMsgBase* paramMsgs[nParams];
    for (size_t i = 0; i < nParams; ++i) {
      PlexilMsgBase* paramMsg = constructPlexilValueMsg(args[i]);
      // Fill in common fields
      paramMsg->count = i;
      paramMsg->serial = serial;
      paramMsg->senderUID = m_myUID.c_str();
      paramMsgs[i] = paramMsg;
    }
    
    // Send the messages
    IPC_RETURN_TYPE result = IPC_OK;
    for (size_t i = 0; i < nParams && result == IPC_OK; i++) {
      char const *msgFormat = msgFormatForType((PlexilMsgType) paramMsgs[i]->msgType);
      if (dest.empty()) {
        debugMsg("IpcFacade:sendParameters", " using format " << msgFormat << " for parameter " << i);
        result = IPC_publishData(msgFormat, paramMsgs[i]);
      }
      else {
        char const *msgName = formatMsgName(msgFormat, dest);
        debugMsg("IpcFacade:sendParameters", " using format " << msgName << " for parameter " << i);
        result = IPC_publishData(msgName, paramMsgs[i]);
      }
    }

    // free the parameter packets
    for (size_t i = 0; i < nParams; i++) {
      PlexilMsgBase* m = paramMsgs[i];
      paramMsgs[i] = NULL;
      switch (m->msgType) {
      case PlexilMsgType_UnknownValue:
        delete (PlexilUnknownValueMsg*) m;
        break;

      case PlexilMsgType_CommandHandleValue:
        delete (PlexilCommandHandleValueMsg*) m;
        break;

      case PlexilMsgType_BooleanValue:
        delete (PlexilBooleanValueMsg*) m;
        break;

      case PlexilMsgType_IntegerValue:
        delete (PlexilIntegerValueMsg*) m;
        break;

      case PlexilMsgType_RealValue:
        delete (PlexilRealValueMsg*) m;
        break;

      case PlexilMsgType_StringValue:
        delete (PlexilStringValueMsg*) m;
        break;

        // *** DON'T FREE ARRAY DATA! IPC does this. ***
      case PlexilMsgType_BooleanArray: {
        PlexilBooleanArrayMsg *bam = (PlexilBooleanArrayMsg*) m;
        delete bam;
        break;
      }

      case PlexilMsgType_IntegerArray: {
        PlexilIntegerArrayMsg *iam = (PlexilIntegerArrayMsg*) m;
        delete iam;
        break;
      }

      case PlexilMsgType_RealArray: {
        PlexilRealArrayMsg *ram = (PlexilRealArrayMsg*) m;
        delete ram;
        break;
      }

      case PlexilMsgType_StringArray: {
        PlexilStringArrayMsg *sam = (PlexilStringArrayMsg*) m;
        delete sam;
        break;
      }

      default:
        delete m;
        break;
      }
    }

    return result;
  }

/** 
 * @brief Helper function for sending a vector of pairs via IPC.
 * @param pairs The pairs to convert into messages and send
 * @param serial The serial to send along with each parameter.  This should be the same serial s the header.
 * 
 * @return The IPC error status.
 */
IPC_RETURN_TYPE IpcFacade::sendPairs(std::vector<std::pair<std::string, Value> > const& pairs,
                                     uint32_t serial) {
  IPC_RETURN_TYPE result = IPC_OK;
  for(std::vector<std::pair<std::string, Value> >::const_iterator it = pairs.begin();
      it != pairs.end() && result == IPC_OK; ++it) {
    PlexilMsgBase* pairMsg = constructPlexilPairMsg(it->first, it->second);
    pairMsg->count = std::distance(pairs.begin(), it);
    pairMsg->serial = serial;
    pairMsg->senderUID = m_myUID.c_str();
    std::string msgName =
        formatMsgName(std::string(msgFormatForType((PlexilMsgType) pairMsg->msgType)),
                      "");
    result = IPC_publishData(msgName.c_str(), pairMsg);

    switch(pairMsg->msgType) {
      case PlexilMsgType_PairBoolean:
        delete reinterpret_cast<BooleanPair*>(pairMsg);
        break;
      case PlexilMsgType_PairInteger:
        delete reinterpret_cast<IntegerPair*>(pairMsg);
        break;
      case PlexilMsgType_PairReal:
        delete reinterpret_cast<RealPair*>(pairMsg);
        break;
      case PlexilMsgType_PairString:
        delete reinterpret_cast<StringPair*>(pairMsg);
        break;
      default:
        delete pairMsg;
        break;
    }
  }
  return result;
}

  /**
   * @brief Get next serial number
   */
  uint32_t IpcFacade::getSerialNumber() {
    return m_nextSerial++;
  }

  bool IpcFacade::definePlexilIPCMessageTypes() {
    debugMsg("IpcFacade:definePlexilIPCMessageTypes", " entered");
    IPC_RETURN_TYPE status;
    status = IPC_defineMsg(MSG_BASE, IPC_VARIABLE_LENGTH, MSG_BASE_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(MSG_BASE, m_myUID), IPC_VARIABLE_LENGTH, MSG_BASE_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(RETURN_VALUE_MSG, IPC_VARIABLE_LENGTH, RETURN_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(RETURN_VALUE_MSG, m_myUID), IPC_VARIABLE_LENGTH, RETURN_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(BOOLEAN_VALUE_MSG, IPC_VARIABLE_LENGTH, BOOLEAN_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(BOOLEAN_VALUE_MSG, m_myUID), IPC_VARIABLE_LENGTH, BOOLEAN_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(INTEGER_VALUE_MSG, IPC_VARIABLE_LENGTH, INTEGER_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(INTEGER_VALUE_MSG, m_myUID), IPC_VARIABLE_LENGTH, INTEGER_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(REAL_VALUE_MSG, IPC_VARIABLE_LENGTH, REAL_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(REAL_VALUE_MSG, m_myUID), IPC_VARIABLE_LENGTH, REAL_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(STRING_VALUE_MSG, IPC_VARIABLE_LENGTH, STRING_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(STRING_VALUE_MSG, m_myUID), IPC_VARIABLE_LENGTH, STRING_VALUE_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(BOOLEAN_ARRAY_MSG, IPC_VARIABLE_LENGTH, BOOLEAN_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(BOOLEAN_ARRAY_MSG, m_myUID), IPC_VARIABLE_LENGTH, BOOLEAN_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(INTEGER_ARRAY_MSG, IPC_VARIABLE_LENGTH, INTEGER_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(INTEGER_ARRAY_MSG, m_myUID), IPC_VARIABLE_LENGTH, INTEGER_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(REAL_ARRAY_MSG, IPC_VARIABLE_LENGTH, REAL_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(REAL_ARRAY_MSG, m_myUID), IPC_VARIABLE_LENGTH, REAL_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(STRING_ARRAY_MSG, IPC_VARIABLE_LENGTH, STRING_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(STRING_ARRAY_MSG, m_myUID), IPC_VARIABLE_LENGTH, STRING_ARRAY_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(BOOLEAN_PAIR_MSG, IPC_VARIABLE_LENGTH, BOOLEAN_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(BOOLEAN_PAIR_MSG, m_myUID), IPC_VARIABLE_LENGTH, BOOLEAN_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(INTEGER_PAIR_MSG, IPC_VARIABLE_LENGTH, INTEGER_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(INTEGER_PAIR_MSG, m_myUID), IPC_VARIABLE_LENGTH, INTEGER_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(REAL_PAIR_MSG, IPC_VARIABLE_LENGTH, REAL_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(REAL_PAIR_MSG, m_myUID), IPC_VARIABLE_LENGTH, REAL_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(STRING_PAIR_MSG, IPC_VARIABLE_LENGTH, STRING_PAIR_MSG_FORMAT);
    if (status != IPC_OK)
      return false;
    status = IPC_defineMsg(formatMsgName(STRING_PAIR_MSG, m_myUID), IPC_VARIABLE_LENGTH, STRING_PAIR_MSG_FORMAT);
    condDebugMsg(status == IPC_OK, "IpcFacade:definePlexilIPCMessageTypes", " succeeded");
    return status == IPC_OK;
  }

  /**
   * @brief IPC listener thread top level function to replace IPC_dispatch().
   */
  void IpcFacade::myIpcDispatch(void * this_as_void_ptr)
  {
    IpcFacade* facade = reinterpret_cast<IpcFacade*>(this_as_void_ptr);
    assertTrueMsg(facade != NULL,
                  "IpcFacade::messageHandler: pointer to IpcFacade instance is null!");
    debugMsg("IpcFacade:myIpcDispatch", " started");
    IPC_RETURN_TYPE ipcStatus = IPC_OK;
    while (!facade->m_stopDispatchThread
           && ipcStatus != IPC_Error) {
      ipcStatus = IPC_listenClear(1000); // 
    }
    assertTrueMsg(ipcStatus != IPC_Error, "IpcFacade::myIpcDispatch: IPC error, IPC_errno = " << IPC_errno);
      
    facade->m_stopDispatchThread = false;
    debugMsg("IpcFacade:myIpcDispatch", " terminated");
  }

  /**
   * @brief Handler function as seen by IPC.
   */
  void IpcFacade::messageHandler(MSG_INSTANCE /* rawMsg */,
                                 void * unmarshalledMsg,
                                 void * this_as_void_ptr) {
    const PlexilMsgBase* msgData = reinterpret_cast<const PlexilMsgBase*> (unmarshalledMsg);
    assertTrueMsg(msgData != NULL,
                  "IpcFacade::messageHandler: pointer to message data is null!");
    IpcFacade* facade = reinterpret_cast<IpcFacade*>(this_as_void_ptr);
    assertTrueMsg(facade != NULL,
                  "IpcFacade::messageHandler: pointer to IpcFacade instance is null!");

    PlexilMsgType msgType = (PlexilMsgType) msgData->msgType;
    debugMsg("IpcFacade:messageHandler", " received message type = " << msgType);
    switch (msgType) {
      // TODO: filter out commands/msgs we aren't prepared to handle

    case PlexilMsgType_Command:
    case PlexilMsgType_LookupNow:
    case PlexilMsgType_PlannerUpdate:
    case PlexilMsgType_TelemetryValues:
      debugMsg("IpcFacade:messageHandler", "processing as multi-part message");
      facade->cacheMessageLeader(msgData);
      break;

      // ReturnValues is a PlexilReturnValuesMsg
      // Followed by 0 (?) or more values
      // Only pay attention to return values directed at us
    case PlexilMsgType_ReturnValues: {
      debugMsg("IpcFacade:messageHandler", " processing as return value");
      const PlexilReturnValuesMsg* returnLeader = (const PlexilReturnValuesMsg*) msgData;
      if (strcmp(returnLeader->requesterUID, facade->getUID().c_str()) == 0)
        facade->cacheMessageLeader(msgData);
      break;
    }
      // Values - could be parameters or return values
    case PlexilMsgType_UnknownValue:
    case PlexilMsgType_BooleanValue:
    case PlexilMsgType_IntegerValue:
    case PlexilMsgType_RealValue:
    case PlexilMsgType_StringValue:
      // Arrays
    case PlexilMsgType_BooleanArray:
    case PlexilMsgType_IntegerArray:
    case PlexilMsgType_RealArray:
    case PlexilMsgType_StringArray:

      // PlannerUpdate pairs
    case PlexilMsgType_PairBoolean:
    case PlexilMsgType_PairInteger:
    case PlexilMsgType_PairReal:
    case PlexilMsgType_PairString:

      // Log with corresponding leader message
      facade->cacheMessageTrailer(msgData);
      break;

    default:
      debugMsg("IpcFacade:messageHandler", "Received single-message type, delivering to listeners");
      facade->deliverMessage(std::vector<const PlexilMsgBase*>(1, msgData));
      break;
    }
  }
  /**
   * @brief Cache start message of a multi-message sequence
   */

  // N.B. Presumes that messages are received in order.
  // Also presumes that any required filtering (e.g. on command name) has been done by the caller

  void IpcFacade::cacheMessageLeader(const PlexilMsgBase* msgData) {
    IpcMessageId msgId(msgData->senderUID, msgData->serial);

    // Check that this isn't a duplicate header
    IncompleteMessageMap::iterator it = m_incompletes.find(msgId);
    assertTrueMsg(it == m_incompletes.end(),
                  "IpcFacade::cacheMessageLeader: internal error: found existing sequence for sender "
                  << msgData->senderUID << ", serial " << msgData->serial);

    if (msgData->count == 0) {
      debugMsg("IpcFacade:cacheMessageLeader", " count == 0, processing immediately");
      std::vector<const PlexilMsgBase*> msgVec(1, msgData);
      deliverMessage(msgVec);
    } else {
      debugMsg("IpcFacade:cacheMessageLeader",
               " storing leader with sender " << msgData->senderUID << ", serial " << msgData->serial
               << ",\n expecting " << msgData->count << " values");
      m_incompletes[msgId] = std::vector<const PlexilMsgBase*>(1, msgData);
    }
  }

  /**
   * @brief Cache following message of a multi-message sequence
   */

  // N.B. Presumes that messages are received in order.

  void IpcFacade::cacheMessageTrailer(const PlexilMsgBase* msgData) {
    IpcMessageId msgId(msgData->senderUID, msgData->serial);
    IncompleteMessageMap::iterator it = m_incompletes.find(msgId);
    if (it == m_incompletes.end()) {
      debugMsg("IpcFacade::cacheMessageTrailer",
               " no existing sequence for sender "
               << msgData->senderUID << ", serial " << msgData->serial << ", ignoring");
      return;
    }
    std::vector<const PlexilMsgBase*>& msgs = it->second;
    msgs.push_back(msgData);
    // Have we got them all?
    if (msgs.size() > msgs[0]->count) {
      debugMsg("IpcFacade:cacheMessageTrailer",
               " delivering " << msgs.size() << " messages");
      deliverMessage(msgs);
      m_incompletes.erase(it);
    }
  }

  /**
   * @brief Deliver the given message to all listeners registered for it
   */
  void IpcFacade::deliverMessage(const std::vector<const PlexilMsgBase*>& msgs) {
    if (!msgs.empty()) {
      //send to listeners for all
      ListenerMap::iterator map_it = m_registeredListeners.find(ALL_MSG_TYPE());
      if (map_it != m_registeredListeners.end()) {
        for (ListenerList::iterator it = (*map_it).second.begin(); it != (*map_it).second.end(); it++) {
          (*it)->ReceiveMessage(msgs);
        }
      }
      //send to listeners for msg type
      map_it = m_registeredListeners.find(msgs.front()->msgType);
      if (map_it != m_registeredListeners.end()) {
        for (ListenerList::iterator it = (*map_it).second.begin(); it != (*map_it).second.end(); it++) {
          (*it)->ReceiveMessage(msgs);
        }
      }

      // clean up
      for (size_t i = 0; i < msgs.size(); i++) {
        const PlexilMsgBase* msg = msgs[i];
        IPC_freeData(IPC_msgFormatter(msgFormatForType((PlexilMsgType) msg->msgType)), (void *) msg);
      }
    }
  }

  void IpcFacade::subscribeGlobal(const LocalListenerRef& listener) {
    //creates a new entry if one does not already exist
    m_registeredListeners[listener.first].push_back(listener.second);
  }

  /**
   * @brief Unsubscribe the given listener from the listener map.
   * @return True if found and unsubscribed. False if not found.
   */
  bool IpcFacade::unsubscribeGlobal(const LocalListenerRef& listener) {
    ListenerMap::iterator map_it = m_registeredListeners.find(listener.first);
    if (map_it != m_registeredListeners.end()) {
      for (ListenerList::iterator it = (*map_it).second.begin(); it != (*map_it).second.end(); it++) {
        if (listener.second == (*it)) {
          it = (*map_it).second.erase(it);
          return true;
        }
      }
    }
    return false;
  }

  /**
   * @brief Initialize unique ID string
   */
  std::string IpcFacade::generateUID() {
    kashmir::system::DevRand randomStream;
    kashmir::uuid_t uuid;
    randomStream >> uuid;
    std::ostringstream s;
    s << uuid;
    debugMsg("IpcFacade:generateUID", " generated UUID " << s.str());
    return s.str();
  }

  /**
   * Unsubscribes from the given message and the UID-specific version of the given message on central. Wrapper for IPC_unsubscribe.
   * If an error occurs in unsubscribing from the given message, the UID-specific version is not processed
   * @param msgName the name of the message to unsubscribe from
   * @param handler The handler to unsubscribe.
   */
  IPC_RETURN_TYPE IpcFacade::unsubscribeCentral (const char *msgName, HANDLER_TYPE handler) {
    IPC_RETURN_TYPE result = IPC_unsubscribe(msgName, handler);
    if (result != IPC_OK)
      return result;
    return IPC_unsubscribe(formatMsgName(std::string(msgName), m_myUID), handler);
  }

  /**
   * Subscribes from the given message and the UID-specific version of the given message on central. Wrapper for IPC_unsubscribe.
   * If an error occurs in subscribing from the given message, the UID-specific version is not processed
   * @param msgName the name of the message to subscribe from
   * @param handler The handler to subscribe.
   * @param clientData Pointer to data that will be passed to handler upon message receipt.
   */
  IPC_RETURN_TYPE IpcFacade::subscribeDataCentral (const char *msgName,
                                                   HANDLER_DATA_TYPE handler) {
    void* clientData = reinterpret_cast<void*>(this);
    debugMsg("IpcFacade:subscribeDataCentral", " for message name \"" << msgName << "\"");
    checkError(IPC_isMsgDefined(msgName),
               "IpcFacade::subscribeDataCentral: fatal error: message \"" << msgName << "\" not defined");
    IPC_RETURN_TYPE result = IPC_subscribeData(msgName, handler, clientData);
    if (result == IPC_OK)
      result = IPC_subscribeData(formatMsgName(msgName, m_myUID), handler, clientData);
    condDebugMsg(result != IPC_OK,
                 "IpcFacade:subscribeDataCentral", " for message name \"" << msgName << "\" failed, IPC_errno = " << IPC_errno);
    return result;
  }

}

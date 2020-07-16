/*****************************************************************************
 *       $Id: IPC.java,v 1.8 2009/09/04 19:11:20 reids Exp $
 * $Revision: 1.8 $
 *     $Date: 2009/09/04 19:11:20 $
 *   $Author: reids $
 *    $State: Exp $
 *   $Locker:  $
 *
 * PROJECT:	NM-DS1
 *
 * FILE:		IPC.java
 *
 * DESCRIPTION: JAVA class for IPC.
 *
 * IPC.java contains the JNI connection to IPC to enable publishing and
 * subscribing to IPC messages.  An instance of IPC establishes a
 * JNI object for communicating over IPC.  Each JAVA program should have
 * only one IPC object -- if you need to connect to multiple servers,
 * you can call IPC_connect multiple times (don't forget to do IPC_getContext
 * after each connection, so that you can switch back and forth between 
 * servers).
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * HISTORY: Based on version by Carroll Thronesbery, Metrica (Sept 2000)
 *
 * $Log: IPC.java,v $
 * Revision 1.8  2009/09/04 19:11:20  reids
 * IPC Java is now in its own package
 *
 * Revision 1.7  2009/07/28 16:21:42  reids
 * Handle messages with null data
 *
 * Revision 1.6  2009/03/16 19:06:26  reids
 * Fixed bug in cases where the formatter is null
 *
 * Revision 1.5  2009/02/07 18:54:45  reids
 * Updates for use on 64 bit machines
 *
 * Revision 1.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2002/01/11 22:35:23  reids
 * Improved error handling for Java version
 *
 * Revision 1.2  2002/01/02 21:10:59  reids
 * Made functions stand alone (no longer throw exceptions -- but catch
 *  the exceptions, print out the problem, and return appropriate error code).
 *
 * Revision 1.1  2002/01/02 17:40:16  reids
 * Initial (and more, or less, complete) release of Java version of IPC.
 *
 *
 *****************************************************************************/

package ipc.java;

import java.lang.*;
import java.util.*;
import java.io.*;

public class IPC {

  /*****************************************************************
   *
   *    CLASSES AND INTERFACES NEEDED TO INTERFACE WITH IPC
   *
   ****************************************************************/

  public static class MSG_INSTANCE {
    MSG_INSTANCE(long theCptr) { cptr = theCptr; }
    long cptr;
  };

  public static class IPC_CONTEXT {
    IPC_CONTEXT(long theCptr) { cptr = theCptr; }
    long cptr;
  };

  public interface HANDLER_TYPE {
    void handle (MSG_INSTANCE msgInstance, Object callData);
  }

  public interface FD_HANDLER_TYPE {
    void handle (int fd);
  }

  public interface CONNECT_HANDLE_TYPE {
    void handle (String moduleName);
  }

  public interface CHANGE_HANDLE_TYPE {
    void handle (String msgName, int numHandlers);
  }

  public interface TIMER_HANDLER_TYPE {
    void handle (long currentTime, long scheduledTime);
  }

  public static class TIMER_REF {
    public TIMER_REF() {}
    int cptr;
  };

  /*****************************************************************
   *
   *    IPC CONSTANTS
   *
   ****************************************************************/

  public static final long IPC_WAIT_FOREVER = 0xFFFFFFFF;

  public static final long TRIGGER_FOREVER = -1;

  /* IPC_RETURN_TYPE */
  public static final int IPC_Error   = 0;
  public static final int IPC_OK      = 1;
  public static final int IPC_Timeout = 2;

  /* IPC_ERROR_TYPE */
  public static final int IPC_No_Error               = 0;
  public static final int IPC_Not_Connected          = 1;
  public static final int IPC_Not_Initialized        = 2;
  public static final int IPC_Message_Not_Defined    = 3;
  public static final int IPC_Not_Fixed_Length       = 4;
  public static final int IPC_Message_Lengths_Differ = 5;
  public static final int IPC_Argument_Out_Of_Range  = 6;
  public static final int IPC_Null_Argument          = 7;
  public static final int IPC_Illegal_Formatter      = 8;
  public static final int IPC_Mismatched_Formatter   = 9;
  public static final int IPC_Wrong_Buffer_Length    = 10;
  public static final int IPC_Communication_Error    = 11;

  /* IPC_VERBOSITY_TYPE */
  public static final int IPC_Silent         = 0;
  public static final int IPC_Print_Warnings = 1;
  public static final int IPC_Print_Errors   = 2;
  public static final int IPC_Exit_On_Errors = 3;
  
  /*****************************************************************
   *
   *    IPC API FUNCTIONS FOR JAVA
   *
   ****************************************************************/

  public static int initialize () { return IPC_initialize(); }

  public static int connectModule (String moduleName, String serverName) {
    return IPC_connectModule(moduleName, serverName);
  }

  public static int connect (String moduleName) {
      return IPC_connect(moduleName);
  }

  public static int disconnect () { return IPC_disconnect(); }

  public static boolean isConnected () { return IPC_isConnected(); }

  public static boolean isModuleConnected (String moduleName) {
    return IPC_isModuleConnected(moduleName);
  }

  public static int defineMsg (String msgName, String formatString) {
    return IPC_defineMsg(msgName, formatString);
  }

  public static boolean isMsgDefined (String msgName) {
    return IPC_isMsgDefined(msgName);
  }

  public static String msgInstanceName (MSG_INSTANCE msgInstance) {
    return IPC_msgInstanceName(msgInstance.cptr);
  }

  public static int subscribeData (String msgName, HANDLER_TYPE msgHandler,
				   Class dataClass) { 
    /* Do it this way because multiple handlers can be subscribed 
       for the same message */
    String handlerName = msgName + msgHandler.getClass().getName();
    int handlerNum = ++handlerNumber;
    String hashKey = Integer.toString(handlerNum);

    msgHashTable.put(handlerName, hashKey);
    msgHashTable.put(hashKey, new handlerHashData(msgHandler, dataClass));

    return IPC_subscribe(msgName, handlerName, handlerNum);
  }

  public static int unsubscribe (String msgName, Class msgHandlerClass) { 
    /* Do it this way because multiple handlers can be subscribed for the
       same message */
    String handlerName = msgName + msgHandlerClass.getName();
    String hashKey = (String)msgHashTable.get(handlerName);

    msgHashTable.remove(handlerName);
    msgHashTable.remove(hashKey);

    return IPC_unsubscribe(msgName, handlerName);
  }

  public static int unsubscribe (String msgName, HANDLER_TYPE msgHandler) { 
    return unsubscribe(msgName, msgHandler.getClass());
  }

  public static int subscribeFD (int fd, FD_HANDLER_TYPE fdHandler) { 
    /* Note: This is simplified since at most one handler can be subscribed 
       for each file descriptor */
    fdHashTable.put(Integer.toString(fd), fdHandler);
    return IPC_subscribeFD(fd);
  }

  public static int unsubscribeFD (int fd) { 
    fdHashTable.remove(Integer.toString(fd));
    return IPC_unsubscribeFD(fd);
  }

  public static int listen (long timeoutMSecs) {
    return IPC_listen(timeoutMSecs);
  }

  public static int listenClear (long timeoutMSecs) {
    return IPC_listenClear(timeoutMSecs);
  }

  public static int listenWait (long timeoutMSecs) {
    return IPC_listenWait(timeoutMSecs);
  }

  public static int handleMessage (long timeoutMSecs) {
    return IPC_handleMessage(timeoutMSecs);
  }

  public static int dispatch () { return IPC_dispatch(); }

  public static int dataLength (MSG_INSTANCE msgInstance) {
    return IPC_dataLength(msgInstance.cptr);
  }

  public static void perror (String msg) { IPC_perror(msg); }

  public static int setCapacity (int capacity) { 
      return IPC_setCapacity(capacity);
  }

  public static int setMsgQueueLength (String msgName, int queueLength) {
    return IPC_setMsgQueueLength(msgName, queueLength);
  }

  public static int setMsgPriority (String msgName, int priority) {
    return IPC_setMsgPriority(msgName, priority);
  }

  public static int setVerbosity (int verbosity) {
    return IPC_setVerbosity(verbosity);
  }

  public static int subscribeConnect (CONNECT_HANDLE_TYPE connectHandler) {
    /* Do it this way because multiple handlers can be subscribed  */
    connectHandlers.add(0, connectHandler);

    if (connectHandlers.size() == 1) {
	return IPC_subscribeConnect();
    } else {
	return IPC_OK;
    }
  }

  public static int subscribeDisconnect (CONNECT_HANDLE_TYPE disconnectHandler){
    /* Do it this way because multiple handlers can be subscribed  */
    disconnectHandlers.add(0, disconnectHandler);

    if (disconnectHandlers.size() == 1) {
	return IPC_subscribeDisconnect();
    } else {
	return IPC_OK;
    }
  }

  private static boolean removeFromHandlerList (List list, Class handlerClass) {
    boolean found = false;
    Iterator iter = list.iterator();

    /* Do it this way because multiple handlers can be subscribed  */
    while (iter.hasNext() && !found) {
      found = (iter.next().getClass() == handlerClass);
      if (found) iter.remove();
    }
    return found;
  }


  public static int unsubscribeConnect (Class connectHandlerClass) {
    if (!removeFromHandlerList(connectHandlers, connectHandlerClass)) {
	System.out.println("No connection handler found of class "+
			   connectHandlerClass.getName());
	return IPC_Error;
    } else if (connectHandlers.size() == 0) {
	return IPC_unsubscribeConnect();
    } else {
	return IPC_OK;
    }
  }

  public static int unsubscribeConnect (CONNECT_HANDLE_TYPE connectHandler) {
    return unsubscribeConnect(connectHandler.getClass());
  }

  public static int unsubscribeDisconnect (Class disconnectHandlerClass) {
    if (!removeFromHandlerList(disconnectHandlers, disconnectHandlerClass)) {
	System.out.println("No disconnection handler found of class "+
			   disconnectHandlerClass.getName());
	return IPC_Error;
    } else if (disconnectHandlers.size() == 0) {
	return IPC_unsubscribeDisconnect();
    } else {
	return IPC_OK;
    }
  }

  public static int unsubscribeDisconnect (CONNECT_HANDLE_TYPE 
					   disconnectHandler) {
    return unsubscribeDisconnect(disconnectHandler.getClass());
  }

  public static int subscribeHandlerChange (String msgName, 
					    CHANGE_HANDLE_TYPE
					    handlerChangeHandler) {
    /* Do it this way because multiple handlers can be subscribed 
       for same message */
    List list = (List)handlerChangeHashTable.get(msgName);
    if (list == null) {
      list = new LinkedList();
      handlerChangeHashTable.put(msgName, list);
    }
    list.add(0, handlerChangeHandler);

    if (list.size() == 1) {
	return IPC_subscribeHandlerChange(msgName);
    } else {
	return IPC_OK;
    }
  }

  public static int unsubscribeHandlerChange (String msgName,
					      Class handlerChangeHandlerClass) {
    List handlerList = (List)handlerChangeHashTable.get(msgName);
    if (handlerList == null) {
      return IPC_Error;
    } else if (!removeFromHandlerList(handlerList, handlerChangeHandlerClass)) {
      System.out.println("No change handler found for message "+ msgName
			   +" of class "+ handlerChangeHandlerClass.getName());
      return IPC_Error;
    } else if (handlerList.size() == 0) {
      return IPC_unsubscribeHandlerChange(msgName);
    } else {
      return IPC_OK;
    }
  }

  public static int unsubscribeHandlerChange (String msgName,
					      CHANGE_HANDLE_TYPE
					      handlerChangeHandler){
    return unsubscribeHandlerChange(msgName, handlerChangeHandler.getClass());
  }

  public static int numHandlers (String msgName) {
    return IPC_numHandlers(msgName);
  }

  public static int defineFormat (String formatName, String formatString) {
    return IPC_defineFormat(formatName, formatString);
  }

  public static int checkMsgFormats (String formatName, String formatString) {
    return IPC_checkMsgFormats(formatName, formatString);
  }

  public static int publishData (String msgName, Object data)  {
    formatters.VARCONTENT varcontent = new formatters.VARCONTENT();
    int retVal, marshallRet = IPC_OK;

    if (data == null) {
      return IPC_publish(msgName, 0, 0);
    } else {
      try { marshallRet = formatters.marshall(IPC_msgFormatter(msgName), 
					      coerceDataObject(data), 
					      varcontent);
      } catch ( Exception e) { 
	handleException("publishData", msgName, e);
      }
      if (marshallRet == IPC_Error) {
	return IPC_Error;
      } else {
	retVal = IPC_publish(msgName, varcontent.length, varcontent.byteArray);
	if (varcontent.byteArray != 0) IPC_freeByteArray(varcontent.byteArray);
	
	return retVal;
      } 
    }
  }

  /* Specialized versions of "publish" to handle primitive data types */
  public static int publishData (String msgName, char data) {
    return publishData(msgName, new formatters.IPCChar(data)); }
  public static int publishData (String msgName, boolean data)  {
    return publishData(msgName, new formatters.IPCBoolean(data)); }
  public static int publishData (String msgName, byte data) {
    return publishData(msgName, new formatters.IPCByte(data)); }
  public static int publishData (String msgName, short data) {
    return publishData(msgName, new formatters.IPCShort(data)); }
  public static int publishData (String msgName, int data) {
    return publishData(msgName, new formatters.IPCInteger(data)); }
  public static int publishData (String msgName, long data) {
    return publishData(msgName, new formatters.IPCLong(data)); }
  public static int publishData (String msgName, float data) {
    return publishData(msgName, new formatters.IPCFloat(data)); }
  public static int publishData (String msgName, double data) {
    return publishData(msgName, new formatters.IPCDouble(data)); }

  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 Object data) {
    formatters.VARCONTENT varcontent = new formatters.VARCONTENT();
    int retVal, marshallRet = IPC_OK;

    try {
      marshallRet = formatters.marshall(IPC_msgFormatter(msgName), 
					coerceDataObject(data), varcontent);
    } catch ( Exception e) {
      handleException("respondData", msgName, e);
    }
    if (marshallRet == IPC_Error) {
      return IPC_Error;
    } else {
      retVal = IPC_respond(msgInstance.cptr, msgName,
			   varcontent.length, varcontent.byteArray);
      if (varcontent.byteArray != 0) IPC_freeByteArray(varcontent.byteArray);
      return retVal;
    }
  }

  /* Specialized versions of "respond" to handle primitive data types */
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 char data) {
    return respondData(msgInstance, msgName, new formatters.IPCChar(data)); }
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 boolean data) {
    return respondData(msgInstance, msgName, new formatters.IPCBoolean(data));}
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 byte data) {
    return respondData(msgInstance, msgName, new formatters.IPCByte(data)); }
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 short data) {
    return respondData(msgInstance, msgName, new formatters.IPCShort(data)); }
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 int data) {
    return respondData(msgInstance, msgName, new formatters.IPCInteger(data));}
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 long data) {
    return respondData(msgInstance, msgName, new formatters.IPCLong(data)); }
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 float data) {
    return respondData(msgInstance, msgName, new formatters.IPCFloat(data)); }
  public static int respondData (MSG_INSTANCE msgInstance, String msgName,
				 double data) {
    return respondData(msgInstance, msgName, new formatters.IPCDouble(data)); }

  public static int queryNotifyData (String msgName, Object data,
				     HANDLER_TYPE handler, Class responseClass)
    {
    formatters.VARCONTENT varcontent = new formatters.VARCONTENT();
    int retVal, marshallRet = IPC_OK;

    try {
      marshallRet = formatters.marshall(IPC_msgFormatter(msgName), 
					coerceDataObject(data), varcontent);
    } catch ( Exception e) {
      handleException("queryNotifyData", msgName, e);
    }
    if (marshallRet == IPC_Error) {
      return IPC_Error;
    } else {
      int handlerNum = ++handlerNumber;
      String hashKey = Integer.toString(handlerNum);
    
      msgHashTable.put(hashKey, new handlerHashData(handler, responseClass));

      retVal = IPC_queryNotify(msgName, varcontent.length, 
			       varcontent.byteArray, handlerNum);
      if (varcontent.byteArray != 0) IPC_freeByteArray(varcontent.byteArray);
      return retVal;
    }
  }

  /* Specialized versions of "queryNotify" to handle primitive data types */
  public static int queryNotifyData (String msgName, char data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCChar(data),
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, boolean data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCBoolean(data), 
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, byte data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCByte(data),
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, short data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCShort(data),
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, int data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCInteger(data),
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, long data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCLong(data),
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, float data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCFloat(data),
			   handler, responseClass); }
  public static int queryNotifyData (String msgName, double data,
				     HANDLER_TYPE handler,
				     Class responseClass) {
    return queryNotifyData(msgName, new formatters.IPCDouble(data),
			   handler, responseClass); }

  public static Object queryResponseData (String msgName, Object data,
					  Class responseClass,
					  long timeoutMSecs) {
    formatters.VARCONTENT varcontent = new formatters.VARCONTENT();
    Object responseObject = null;
    int retVal;

    try {
      if (formatters.marshall(IPC_msgFormatter(msgName),coerceDataObject(data),
			      varcontent) != IPC_Error) {
	queryResponse response = new queryResponse();

	retVal = IPC_queryResponse(msgName, varcontent.length,
				   varcontent.byteArray, response,
				   timeoutMSecs);
	if (varcontent.byteArray != 0) IPC_freeByteArray(varcontent.byteArray);
	if (retVal == IPC_OK && varcontent.byteArray != 0) {
	  responseObject = unmarshallMsgData(response.formatter,
					     response.byteArray,
					     responseClass);
	}

      }
    } catch ( Exception e) {
      handleException("queryResponseData", msgName, e);
    }
    return responseObject;
  }

  /* Specialized versions of "queryResponse" to handle primitive data types */
  public static Object queryResponseData (String msgName, char data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCChar(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, boolean data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCBoolean(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, byte data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCByte(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, short data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCShort(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, int data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCInteger(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, long data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCLong(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, float data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCFloat(data),
			     responseClass, timeOutMSecs); }
  public static Object queryResponseData (String msgName, double data,
					  Class responseClass,
					  long timeOutMSecs) {
    return queryResponseData(msgName, new formatters.IPCDouble(data),
			     responseClass, timeOutMSecs); }

  public static IPC_CONTEXT getContext () { 
      return new IPC_CONTEXT(IPC_getContext());
  }

  public static int setContext (IPC_CONTEXT context) {
    return IPC_setContext(context.cptr);
  }

  public static int addTimer (long tdelay, long count, 
			      TIMER_HANDLER_TYPE handler) {
    int handlerNum = ++handlerNumber;
    String hashKey = Integer.toString(handlerNum);

    timerHashTable.put(handler.getClass().getName(), hashKey);
    timerHashTable.put(hashKey, handler);

    return IPC_addTimer(tdelay, count, handlerNum);
  }

  public static int addTimerGetRef (long tdelay, long count, 
				    TIMER_HANDLER_TYPE handler,
				    TIMER_REF timerRef) {
    int refVal, handlerNum = ++handlerNumber;
    String hashKey = Integer.toString(handlerNum);

    timerHashTable.put(hashKey, handler);

    refVal = IPC_addTimerGetRef(tdelay, count, handlerNum);
    if (refVal == 0) {
      return IPC_Error;
    } else {
	timerHashTable.put(Integer.toString(refVal), hashKey);
      timerRef.cptr = refVal;
      return IPC_OK;
    }
  }

  public static int addOneShotTimer (long tdelay, TIMER_HANDLER_TYPE handler) {
    return IPC.addTimer(tdelay, 1, handler);
  }

  public static int addPeriodicTimer (long tdelay,
				      TIMER_HANDLER_TYPE handler) {
    return IPC.addTimer(tdelay, TRIGGER_FOREVER, handler);
  }

  public static int removeTimer (Class handlerClass) {
    String hashKey = handlerClass.getName();
    String handlerNum = (String)timerHashTable.get(hashKey);

    timerHashTable.remove(hashKey);
    timerHashTable.remove(handlerNum);

    return IPC_removeTimer(Integer.parseInt(handlerNum));
  }

  public static int removeTimer (TIMER_HANDLER_TYPE handler) {
    return removeTimer(handler.getClass());
  }

  public static int removeTimerByRef (TIMER_REF timerRef) {
    String hashKey = Integer.toString(timerRef.cptr);
    String handlerNum = (String)timerHashTable.get(hashKey);

    timerHashTable.remove(hashKey);
    timerHashTable.remove(handlerNum);

    return IPC_removeTimerByRef(timerRef.cptr);
  }

  public static long timeInMillis () {
    return IPC_timeInMillis();
  }

  /*****************************************************************
   *
   *    INTERNAL FUNCTIONS
   *
   ****************************************************************/

  private native static int IPC_initialize();
  private native static int IPC_connectModule(String moduleName,
					      String serverName);
  private native static int IPC_connect(String moduleName);
  private native static int IPC_disconnect();
  private native static boolean IPC_isConnected();
  private native static boolean IPC_isModuleConnected(String moduleName);
  private native static int IPC_defineMsg(String msgName, String formatString);
  private native static boolean IPC_isMsgDefined(String msgName);
  private native static int IPC_dataLength (long msgInstance);
  private native static String IPC_msgInstanceName (long msgInstance);
  private native static long IPC_msgInstanceFormatter (long msgInstance);
  private native static int IPC_subscribe(String msgName, String handlerName,
					  int handlerNum);
  private native static int IPC_unsubscribe(String msgName, String handlerName);
  private native static int IPC_subscribeFD(int fd);
  private native static int IPC_unsubscribeFD(int fd);
  private native static int IPC_publish (String msgName, 
					 int length, long byteArray);
  private native static int IPC_listen(long timeoutMSecs);
  private native static int IPC_listenClear(long timeoutMSecs);
  private native static int IPC_listenWait(long timeoutMSecs);
  private native static int IPC_handleMessage(long timeoutMSecs);
  private native static int IPC_dispatch();
  private native static void IPC_perror(String msg);
  private native static int IPC_setCapacity (int capacity);
  private native static int IPC_setMsgQueueLength (String msgName, 
						   int queueLength);
  private native static int IPC_setMsgPriority (String msgName, int priority);
  private native static int IPC_setVerbosity (int verbosity);
  private native static int IPC_subscribeConnect ();
  private native static int IPC_subscribeDisconnect ();
  private native static int IPC_unsubscribeConnect ();
  private native static int IPC_unsubscribeDisconnect ();
  private native static int IPC_subscribeHandlerChange (String msgName);
  private native static int IPC_unsubscribeHandlerChange (String msgName);
  private native static int IPC_numHandlers (String msgName); 
  private native static int IPC_respond (long msgInstance, String msgName,
					 int length, long byteArray);
  private native static int IPC_queryNotify (String msgName,
					     int length, long byteArray,
					     int handlerNum);
  private native static int IPC_queryResponse (String msgName,
					       int length, long byteArray,
					       queryResponse response,
					       long timeoutMSecs);
  private native static int IPC_defineFormat (String formatName,
					      String formatString);
  private native static int IPC_checkMsgFormats (String formatName,
						 String formatString);
  private native static long IPC_getContext (); 
  private native static int IPC_setContext (long context);
  private native static void IPC_freeByteArray(long byteArray);
  private native static long IPC_msgFormatter(String msgName);
  private native static int IPC_addTimer(long tdelay, long count, 
					 int handlerNum);
  private native static int IPC_addTimerGetRef(long tdelay, long count, 
					       int handlerNum);
  private native static int IPC_removeTimer(int handlerNum);
  private native static int IPC_removeTimerByRef(long timerRef);
  private native static long IPC_timeInMillis();

  private static Map msgHashTable = Collections.synchronizedMap(new HashMap());
  private static List connectHandlers =
      Collections.synchronizedList(new ArrayList());
  private static List disconnectHandlers = 
      Collections.synchronizedList(new ArrayList());
  private static Map handlerChangeHashTable =
      Collections.synchronizedMap(new HashMap());
  private static Map fdHashTable = Collections.synchronizedMap(new HashMap());
  private static Map timerHashTable =
      Collections.synchronizedMap(new HashMap());
  private static int handlerNumber = 0;

  private static class queryResponse {
    long byteArray;
    long formatter;
  }

  private static class handlerHashData {
    handlerHashData (HANDLER_TYPE msgHandler, Class theClass) {
      handler = msgHandler; dataClass = theClass; }
    HANDLER_TYPE handler;
    Class dataClass;
  }

  // Change primitive types into types that IPC can handle (same with
  //  the JAVA object versions of those types).
  private static Class coerceDataClass (Class dataClass) {
    if (dataClass == int.class || dataClass == Integer.class) {
      return formatters.IPCInteger.class;
    } else if (dataClass == double.class || dataClass == Double.class) {
      return formatters.IPCDouble.class;
    } else if (dataClass == String.class) {
      return formatters.IPCString.class;
    } else if (dataClass == float.class || dataClass == Float.class) {
      return formatters.IPCFloat.class;
    } else if (dataClass == short.class || dataClass == Short.class) {
      return formatters.IPCShort.class;
    } else if (dataClass == long.class || dataClass == Long.class) {
      return formatters.IPCLong.class;
    } else if (dataClass == char.class) {
      return formatters.IPCChar.class;
    } else if (dataClass == boolean.class || dataClass == Boolean.class) {
      return formatters.IPCBoolean.class;
    } else if (dataClass == byte.class || dataClass == Byte.class) {
      return formatters.IPCByte.class;
    } else {
      return dataClass;
    }
  }

  // Change Java types that "objectize" primitives (plus String)
  //   to types that IPC can handle.
  private static Object coerceDataObject (Object dataObject) {
    Class dataClass = dataObject.getClass();

    if (formatters.IPCPrim.class.isAssignableFrom(dataClass)) {
      return dataObject;
    } else if (dataClass == Integer.class) {
      return new formatters.IPCInteger(((Integer)dataObject).intValue());
    } else if (dataClass == Double.class) {
      return new formatters.IPCDouble(((Double)dataObject).intValue());
    } else if (dataClass == String.class) {
      return new formatters.IPCString((String)dataObject);
    } else if (dataClass == Float.class) {
      return new formatters.IPCFloat(((Float)dataObject).floatValue());
    } else if (dataClass == Short.class) {
      return new formatters.IPCShort(((Short)dataObject).shortValue());
    } else if (dataClass == Long.class) {
      return new formatters.IPCLong(((Long)dataObject).longValue());
    } else if (dataClass == Boolean.class) {
      return new formatters.IPCBoolean(((Boolean)dataObject).booleanValue());
    } else if (dataClass == Byte.class) {
      return new formatters.IPCByte(((Byte)dataObject).byteValue());
    } else {
      return dataObject;
    }
  }

  private static Object unmarshallMsgData (long formatter, long byteArray,
					   Class msgDataClass) 
      throws Exception {
    Object object = null;

    // No data or formatter
    if (byteArray == 0 || formatter == 0) return object;

    // Throw an exception if class does not match formatter
    formatters.checkDataClass(formatter, msgDataClass);

    if (msgDataClass.isArray()) {
      // Create a top-level fixed-length array, based on the formatter.
      object = formatters.createFixedArray(msgDataClass.getComponentType(),
					   formatter);
    } else {
      // Create an object type that Java IPC can handle
      object = coerceDataClass(msgDataClass).newInstance();
    }
    formatters.unmarshall(formatter, byteArray, object);
    IPC_freeByteArray(byteArray);
    if (formatters.IPCPrim.class.isAssignableFrom(object.getClass())) {
	object = ((formatters.IPCPrim)object).coerce();
    }

    return object;
  }

  private static void msgCallbackHandler (int handlerNum, long msgInstance,
					  long byteArray) {
    handlerHashData handlerData = 
	(handlerHashData)msgHashTable.get(Integer.toString(handlerNum));

    if (handlerData != null) {
      Object object;
      try {
	  long formatter = IPC_msgInstanceFormatter(msgInstance);
	  object = unmarshallMsgData(formatter, byteArray,
				     handlerData.dataClass);
	  handlerData.handler.handle(new MSG_INSTANCE(msgInstance), object);
      } catch ( Exception e) {
	  handleException("msgCallbackHandler", 
			  IPC_msgInstanceName(msgInstance), e);
      }
    } else
      System.out.println("Ooops -- no handler for "+
			 IPC_msgInstanceName(msgInstance));
  }

  private static void queryNotifyCallbackHandler (int handlerNum,
						  long msgInstance,
						  long byteArray) {
    String hashKey = Integer.toString(handlerNum);
    handlerHashData handlerData = (handlerHashData)msgHashTable.get(hashKey);

    msgHashTable.remove(hashKey);
    if (handlerData != null) {
      Object object;
      try {
	object = unmarshallMsgData(IPC_msgInstanceFormatter(msgInstance),
				   byteArray, handlerData.dataClass);
	handlerData.handler.handle(new MSG_INSTANCE(msgInstance), object);
      } catch ( Exception e) { 
	handleException("queryNotifyCallbackHandler", 
			IPC_msgInstanceName(msgInstance), e);
      }
    } else
      System.out.println("Ooops -- no query notification handler for "+
			 IPC_msgInstanceName(msgInstance));
  }

  private static void fdCallbackHandler (int fd) {
    FD_HANDLER_TYPE handler =
      (FD_HANDLER_TYPE)fdHashTable.get(Integer.toString(fd));

    if (handler != null) {
      handler.handle(fd);
    } else
      System.out.println("Ooops -- no handler for fd");
  }

  private static void timerCallbackHandler (int handlerNum, long currentTime,
					    long scheduledTime) {
    TIMER_HANDLER_TYPE handler =
      (TIMER_HANDLER_TYPE)timerHashTable.get(Integer.toString(handlerNum));

    if (handler != null) {
      handler.handle(currentTime, scheduledTime);
    } else
      System.out.println("Ooops -- no handler for timer");
  }

  private static void connectCallbackHandler (String moduleName,
					      boolean isConnect) {
    Iterator iter = (isConnect ? connectHandlers.iterator()
		     : disconnectHandlers.iterator());

    while (iter.hasNext()) {
      CONNECT_HANDLE_TYPE handler = (CONNECT_HANDLE_TYPE)iter.next();
      handler.handle(moduleName);
    }
  }

  private static void changeCallbackHandler (String msgName, int numHandlers) {
    List handlerList = (List)handlerChangeHashTable.get(msgName);
    if (handlerList == null) {
      System.out.println("Ooops -- no change handlers for message "+msgName);
      return;
    }
    Iterator iter = handlerList.iterator();

    while (iter.hasNext()) {
      ((CHANGE_HANDLE_TYPE)iter.next()).handle(msgName, numHandlers);
    }
  }

  private static void handleException(String where, String msgName, 
				      Exception e) {
    System.err.println("ERROR: "+ where +": For message \""+msgName +"\": "+ 
		       e.getMessage());
    e.printStackTrace();
  }

  static {
    System.loadLibrary("ipcjava");
  }
}

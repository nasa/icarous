/*****************************************************************************
 *       $Id: ipcjava.c,v 1.7 2011/08/17 01:10:27 reids Exp $
 * $Revision: 1.7 $
 *     $Date: 2011/08/17 01:10:27 $
 *   $Author: reids $
 *    $State: Exp $
 *   $Locker:  $
 *
 * PROJECT:	NM-DS1
 *
 * FILE:		ipcjava.c
 *
 * DESCRIPTION: JNI functions for the JAVA version of IPC.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * HISTORY: Based on version by Carroll Thronesbery, Metrica (Sept 2000)
 *
 * $Log: ipcjava.c,v $
 * Revision 1.7  2011/08/17 01:10:27  reids
 * Now uses ipcFFI
 *
 * Revision 1.6  2011/08/16 16:04:05  reids
 * Updated Java version to support 64-bit machines
 *
 * Revision 1.5  2009/09/04 19:11:20  reids
 * IPC Java is now in its own package
 *
 * Revision 1.4  2009/02/07 18:54:45  reids
 * Updates for use on 64 bit machines
 *
 * Revision 1.3  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2002/01/02 21:09:03  reids
 * Added another debugging function (printByteArray).
 *
 * Revision 1.1  2002/01/02 17:40:17  reids
 * Initial (and more, or less, complete) release of Java version of IPC.
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <jni.h>
#include "globalM.h"
#include "IPC.h"
#include "formatters.h"
#include "primFmttrs.h"

#define JAVA
#include "ipcFFI.c"

#define NOT_YET_SET 0xFFFFFFFF

#define MSG_CALLBACK_SIGNATURE     "(IJJ)V"
#define TIMER_CALLBACK_SIGNATURE   "(IJJ)V"
#define FD_CALLBACK_SIGNATURE      "(I)V"
#define CONNECT_CALLBACK_SIGNATURE "(Ljava/lang/String;Z)V"
#define CHANGE_CALLBACK_SIGNATURE  "(Ljava/lang/String;I)V"

JavaVM* pJavaVM = NULL;
jclass ipcClass                = (jclass)NOT_YET_SET;
jmethodID msgHandlerID         = (jmethodID)NOT_YET_SET;
jmethodID queryNotifyHandlerID = (jmethodID)NOT_YET_SET;
jmethodID timerHandlerID       = (jmethodID)NOT_YET_SET;
jmethodID fdHandlerID          = (jmethodID)NOT_YET_SET;
jmethodID connectHandlerID     = (jmethodID)NOT_YET_SET;
jmethodID disconnectHandlerID  = (jmethodID)NOT_YET_SET;
jmethodID changeHandlerID      = (jmethodID)NOT_YET_SET;

static void ipcJavaMsgHandler (MSG_INSTANCE msgInstance, BYTE_ARRAY callData,
			       void *handlerNum)
{
  JNIEnv* env;

  (*pJavaVM)->AttachCurrentThread(pJavaVM, (void **)(void *)&env, NULL);

  (*env)->CallStaticVoidMethod(env, ipcClass, msgHandlerID, 
			       (jint)(size_t)handlerNum,
			       (jlong)(size_t)msgInstance,
			       (jlong)(size_t)callData);
  /* Does not seem to work under my version of Linux (or else I am doing
     something really wrong here */
  //(*pJavaVM)->DetachCurrentThread(pJavaVM);
}

static void ipcJavaQueryNotifyHandler (MSG_INSTANCE msgInstance, 
				       BYTE_ARRAY callData, void *handlerNum)
{
  JNIEnv* env;

  (*pJavaVM)->AttachCurrentThread(pJavaVM, (void **)(void *)&env, NULL);
  (*env)->CallStaticVoidMethod(env, ipcClass, queryNotifyHandlerID, 
			       (jint)(size_t)handlerNum,
			       (jlong)(size_t)msgInstance, 
			       (jlong)(size_t)callData);
  /* Does not seem to work under my version of Linux (or else I am doing
     something really wrong here */
  //(*pJavaVM)->DetachCurrentThread(pJavaVM);
}

static void ipcJavaTimerHandler (void *handlerNum, unsigned long currentTime, 
				 unsigned long scheduledTime)
{
  JNIEnv* env;

  (*pJavaVM)->AttachCurrentThread(pJavaVM, (void **)(void *)&env, NULL);
  (*env)->CallStaticVoidMethod(env, ipcClass, timerHandlerID,
			       (jint)(size_t)handlerNum,
			       (jlong)(size_t)currentTime,
			       (jlong)(size_t)scheduledTime);
  /* Does not seem to work under my version of Linux (or else I am doing
     something really wrong here */
  //(*pJavaVM)->DetachCurrentThread(pJavaVM);
}

static void ipcJavaFdHandler (int fd, void *handlerNum)
{
  JNIEnv* env;

  (*pJavaVM)->AttachCurrentThread(pJavaVM, (void **)(void *)&env, NULL);
  (*env)->CallStaticVoidMethod(env, ipcClass, fdHandlerID, (jint)fd);
  /* Does not seem to work under my version of Linux (or else I am doing
     something really wrong here */
  //(*pJavaVM)->DetachCurrentThread(pJavaVM);
}

static void ipcJavaConnectHandler (const char *moduleName, void *clientData)
{
  JNIEnv* env;

  (*pJavaVM)->AttachCurrentThread(pJavaVM, (void **)(void *)&env, NULL);
  (*env)->CallStaticVoidMethod(env, ipcClass, connectHandlerID,
			       (*env)->NewStringUTF(env, moduleName),
			       ((BOOLEAN)(size_t)clientData ? JNI_TRUE : JNI_FALSE));

  /* Does not seem to work under my version of Linux (or else I am doing
     something really wrong here */
  //(*pJavaVM)->DetachCurrentThread(pJavaVM);
}

static void ipcJavaChangeHandler (const char *msgName, int numHandlers,
				  void *clientData)
{
  JNIEnv* env;

  (*pJavaVM)->AttachCurrentThread(pJavaVM, (void **)(void *)&env, NULL);
  (*env)->CallStaticVoidMethod(env, ipcClass, changeHandlerID,
			       (*env)->NewStringUTF(env, msgName),
			       (jint)numHandlers);

  /* Does not seem to work under my version of Linux (or else I am doing
     something really wrong here */
  //(*pJavaVM)->DetachCurrentThread(pJavaVM);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1initialize (JNIEnv *env, jclass theClass)
{
  return (jint)IPC_initialize();
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1connectModule (JNIEnv *env, jclass theClass,
			     jstring moduleName, jstring serverName)
{
  const char *cmoduleName, *cserverName;
  int retVal;

  cmoduleName = (*env)->GetStringUTFChars(env, moduleName, 0);
  cserverName = (*env)->GetStringUTFChars(env, serverName, 0);
  retVal = IPC_connectModule(cmoduleName, cserverName);
  (*env)->ReleaseStringUTFChars(env, moduleName, cmoduleName);
  (*env)->ReleaseStringUTFChars(env, serverName, cserverName);

  return (jint)retVal;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1connect (JNIEnv *env, jclass theClass,
					      jstring moduleName) 
{
  const char *cmoduleName;
  int retVal;

  cmoduleName = (*env)->GetStringUTFChars(env, moduleName, 0);
  retVal = IPC_connect(cmoduleName);
  (*env)->ReleaseStringUTFChars(env, moduleName, cmoduleName);

  return (jint)retVal;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1disconnect (JNIEnv *env, jclass theClass)
{
  return (jint)IPC_disconnect();
}

JNIEXPORT jboolean JNICALL Java_ipc_java_IPC_IPC_1isConnected (JNIEnv *env,
						      jclass theClass)
{
  return (IPC_isConnected() == TRUE ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jboolean JNICALL
Java_ipc_java_IPC_IPC_1isModuleConnected (JNIEnv *env, jclass theClass, 
				 jstring moduleName)
{
  const char *cmoduleName;
  int retVal;

  cmoduleName = (*env)->GetStringUTFChars(env, moduleName, 0);
  retVal = IPC_isModuleConnected(cmoduleName);
  (*env)->ReleaseStringUTFChars(env, moduleName, cmoduleName);

  return (retVal == TRUE ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1defineMsg (JNIEnv *env, jclass clas,
						jstring msgName,
						jstring formatString)
{
  const char *cmsgName, *cformatString;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  cformatString = (*env)->GetStringUTFChars(env, formatString, 0);
  retVal = IPC_defineMsg(cmsgName, IPC_VARIABLE_LENGTH, cformatString);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  (*env)->ReleaseStringUTFChars(env, formatString, cformatString);

  return (jint)retVal;
}

JNIEXPORT jboolean JNICALL 
Java_ipc_java_IPC_IPC_1isMsgDefined (JNIEnv *env, jclass theClass, jstring msgName)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_isMsgDefined(cmsgName);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);

  return (retVal == TRUE ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1dataLength (JNIEnv *env, jclass theClass,
						 jlong msgInstance)
{
  return (jint)IPC_dataLength((MSG_INSTANCE)(size_t)msgInstance);
}

JNIEXPORT jstring JNICALL
Java_ipc_java_IPC_IPC_1msgInstanceName (JNIEnv *env, jclass theClass,
			       jlong msgInstance)
{
  const char *cmsgName;

  cmsgName = IPC_msgInstanceName((MSG_INSTANCE)(size_t)msgInstance);
  return (*env)->NewStringUTF(env, cmsgName);
}

JNIEXPORT jlong JNICALL
Java_ipc_java_IPC_IPC_1msgInstanceFormatter (JNIEnv *env, jclass theClass,
				    jlong msgInstance)
{
  return (jlong)(size_t)IPC_msgInstanceFormatter((MSG_INSTANCE)(size_t)msgInstance);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1subscribe (JNIEnv *env, jclass theClass,
						jstring msgName,
						jstring handlerName,
						jint handlerNum)
{
  const char *cmsgName, *chandlerName;
  int retVal;

  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (msgHandlerID == (jmethodID)NOT_YET_SET)
    msgHandlerID = (*env)->GetStaticMethodID(env, theClass,
					     "msgCallbackHandler",
					     MSG_CALLBACK_SIGNATURE);

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  chandlerName = (*env)->GetStringUTFChars(env, handlerName, 0);
  retVal = _IPC_subscribe(cmsgName, (char *)chandlerName, ipcJavaMsgHandler,
			  (void *)(size_t)handlerNum, 0);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  return retVal;
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1unsubscribe (JNIEnv *env, jclass theClass,
			   jstring msgName, jstring handlerName) 
{
  const char *cmsgName, *chandlerName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  chandlerName = (*env)->GetStringUTFChars(env, handlerName, 0);
  retVal = _IPC_unsubscribe(cmsgName, chandlerName);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  (*env)->ReleaseStringUTFChars(env, handlerName, chandlerName);
  return retVal;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1subscribeFD (JNIEnv *env, jclass theClass,
						  jint fd)
{
  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (fdHandlerID == (jmethodID)NOT_YET_SET)
    fdHandlerID = (*env)->GetStaticMethodID(env, theClass,
					    "fdCallbackHandler",
					     FD_CALLBACK_SIGNATURE);

  return (jint)IPC_subscribeFD((int)fd, ipcJavaFdHandler, NULL);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1unsubscribeFD (JNIEnv *env,
						    jclass theClass, jint fd)
{
  return (jint)IPC_unsubscribeFD((int)fd, ipcJavaFdHandler);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1publish (JNIEnv *env, jclass theClass,
					      jstring msgName, jint length,
					      jlong byteArray)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_publish(cmsgName, length, (void *)(size_t)byteArray);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  return retVal;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1listen (JNIEnv *env, jclass theClass,
					     jlong timeoutMSecs)
{
  return (jint)IPC_listen(timeoutMSecs);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1listenClear (JNIEnv *env, jclass theClass, jlong timeoutMSecs)
{
  return (jint)IPC_listenClear(timeoutMSecs);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1listenWait (JNIEnv *env, jclass theClass,
						 jlong timeoutMSecs)
{
  return (jint)IPC_listenWait(timeoutMSecs);
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1handleMessage (JNIEnv *env, jclass theClass, jlong timeoutMSecs)
{
  return (jint)IPC_handleMessage(timeoutMSecs);
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1dispatch (JNIEnv *env, jclass theClass)
{
  return (jint)IPC_dispatch();
}

JNIEXPORT void JNICALL Java_ipc_java_IPC_IPC_1perror (JNIEnv *env, jclass theClass,
					     jstring msg)
{
  const char *cmsg = (*env)->GetStringUTFChars(env, msg, 0);
  IPC_perror(cmsg);
  (*env)->ReleaseStringUTFChars(env, msg, cmsg);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1setCapacity (JNIEnv *env, jclass theClass, jint capacity)
{
  return IPC_setCapacity(capacity);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1setMsgQueueLength (JNIEnv *env, jclass theClass, 
				 jstring msgName, jint queueLength)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_setMsgQueueLength((char *)cmsgName, queueLength);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  return retVal;
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1setMsgPriority (JNIEnv *env, jclass theClass,
			      jstring msgName, jint priority)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_setMsgPriority((char *)cmsgName, priority);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  return retVal;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1setVerbosity (JNIEnv *env, jclass theClass,
						   jint verbosity)
{
  return (jint)IPC_setVerbosity((IPC_VERBOSITY_TYPE)verbosity);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1subscribeConnect (JNIEnv *env, jclass theClass)
{
  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (connectHandlerID == (jmethodID)NOT_YET_SET)
    connectHandlerID = (*env)->GetStaticMethodID(env, theClass,
						 "connectCallbackHandler",
						 CONNECT_CALLBACK_SIGNATURE);
  return (jint)IPC_subscribeConnect(ipcJavaConnectHandler, (void *)TRUE);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1subscribeDisconnect (JNIEnv *env, jclass theClass)
{
  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (connectHandlerID == (jmethodID)NOT_YET_SET)
    connectHandlerID = (*env)->GetStaticMethodID(env, theClass,
						 "connectCallbackHandler",
						 CONNECT_CALLBACK_SIGNATURE);
  return (jint)IPC_subscribeDisconnect(ipcJavaConnectHandler, (void *)FALSE);
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1unsubscribeConnect (JNIEnv *env, jclass theClass)
{
  return IPC_unsubscribeConnect(ipcJavaConnectHandler);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1unsubscribeDisconnect (JNIEnv *env, jclass theClass)
{
  return IPC_unsubscribeDisconnect(ipcJavaConnectHandler);
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1subscribeHandlerChange (JNIEnv *env, jclass theClass,
				      jstring msgName)
{
  const char *cmsgName;
  int retVal;

  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (changeHandlerID == (jmethodID)NOT_YET_SET)
    changeHandlerID = (*env)->GetStaticMethodID(env, theClass,
						"changeCallbackHandler",
						CHANGE_CALLBACK_SIGNATURE);

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_subscribeHandlerChange(cmsgName, ipcJavaChangeHandler, NULL);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  return retVal;
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1unsubscribeHandlerChange (JNIEnv *env, jclass theClass,
					jstring msgName)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_unsubscribeHandlerChange(cmsgName, ipcJavaChangeHandler);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);
  return retVal; 
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1numHandlers (JNIEnv *env, jclass theClass, jstring msgName)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_numHandlers(cmsgName);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);

  return retVal;
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1respond (JNIEnv *env, jclass theClass, jlong msgInstance,
		       jstring msgName, jint length, jlong byteArray)
{
  const char *cmsgName;
  int retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_respond((MSG_INSTANCE)(size_t)msgInstance, cmsgName,
		       (int)length, (BYTE_ARRAY)(size_t)byteArray);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);

  return retVal;
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1queryNotify (JNIEnv *env, jclass theClass,
			   jstring msgName, jint length, jlong byteArray,
			   jint handlerNum)
{
  const char *cmsgName;
  int retVal;

  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (queryNotifyHandlerID == (jmethodID)NOT_YET_SET)
    queryNotifyHandlerID =
      (*env)->GetStaticMethodID(env, theClass, "queryNotifyCallbackHandler",
				MSG_CALLBACK_SIGNATURE);

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_queryNotify(cmsgName, (int)length,
			   (BYTE_ARRAY)(size_t)byteArray,
			   ipcJavaQueryNotifyHandler,
			   (void *)(size_t)handlerNum);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);

  return retVal;
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1queryResponse (JNIEnv *env, jclass theClass, jstring msgName,
			     jint length, jlong byteArray, jobject response,
			     jlong timeoutMSecs)
{
  const char *cmsgName;
  int retVal;
  BYTE_ARRAY replyHandle;
  FORMATTER_PTR replyFormat;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = _IPC_queryResponse(cmsgName, (int)length,
			      (BYTE_ARRAY)(size_t)byteArray,
			      &replyHandle, &replyFormat,
			      (unsigned int)timeoutMSecs);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);

  if (retVal == IPC_OK) {
    jclass queryResponseClass = (*env)->GetObjectClass(env, response);
    jfieldID byteArrayFieldID = (*env)->GetFieldID(env, queryResponseClass,
						   "byteArray", "J");
    jfieldID formatterFieldID = (*env)->GetFieldID(env, queryResponseClass,
						   "formatter", "J");
    (*env)->SetLongField(env, response, byteArrayFieldID,
			 (jlong)(size_t)replyHandle);
    (*env)->SetLongField(env, response, formatterFieldID,
			 (jlong)(size_t)replyFormat);
  }

  return (jint)retVal;
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1defineFormat (JNIEnv *env, jclass theClass, 
			    jstring formatName, jstring formatString)
{
  const char *cformatName, *cformatString;
  int retVal;

  cformatName = (*env)->GetStringUTFChars(env, formatName, 0);
  cformatString = (*env)->GetStringUTFChars(env, formatString, 0);
  retVal = IPC_defineFormat(cformatName, cformatString);
  (*env)->ReleaseStringUTFChars(env, formatName, cformatName);
  (*env)->ReleaseStringUTFChars(env, formatString, cformatString);

  return (jint)retVal;
}

JNIEXPORT jint JNICALL
Java_ipc_java_IPC_IPC_1checkMsgFormats (JNIEnv *env, jclass theClass,
			       jstring formatName, jstring formatString)
{
  const char *cformatName, *cformatString;
  int retVal;

  cformatName = (*env)->GetStringUTFChars(env, formatName, 0);
  cformatString = (*env)->GetStringUTFChars(env, formatString, 0);
  retVal = IPC_checkMsgFormats(cformatName, cformatString);
  (*env)->ReleaseStringUTFChars(env, formatName, cformatName);
  (*env)->ReleaseStringUTFChars(env, formatString, cformatString);

  return (jint)retVal;
}

JNIEXPORT jlong JNICALL Java_ipc_java_IPC_IPC_1getContext (JNIEnv *env,
						    jclass theClass)
{
  return (jlong)(size_t)IPC_getContext();
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1setContext (JNIEnv *env, jclass theClass,
						 jlong context)
{
  return IPC_setContext((IPC_CONTEXT_PTR)(size_t)context);
}

JNIEXPORT void JNICALL
Java_ipc_java_IPC_IPC_1freeByteArray (JNIEnv *env, jclass theClass, jlong byteArray)
{
  IPC_freeByteArray((BYTE_ARRAY)(size_t)byteArray);
}

JNIEXPORT jlong JNICALL Java_ipc_java_IPC_IPC_1msgFormatter (JNIEnv *env,
						   jclass theClass,
						   jstring msgName)
{
  const char *cmsgName;
  FORMATTER_PTR retVal;

  cmsgName = (*env)->GetStringUTFChars(env, msgName, 0);
  retVal = IPC_msgFormatter(cmsgName);
  (*env)->ReleaseStringUTFChars(env, msgName, cmsgName);

  return (jlong)(size_t)retVal;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1addTimer (JNIEnv *env, jclass theClass,
					       jlong tdelay, jlong count,
					       jint handlerNum)
{
  /* Set up information needed for the handler callbacks */
  if (pJavaVM == NULL) (*env)->GetJavaVM(env, &pJavaVM);
  if (ipcClass == (jclass)NOT_YET_SET)
    ipcClass = (jclass)(*env)->NewGlobalRef(env, theClass);
  if (timerHandlerID == (jmethodID)NOT_YET_SET)
    timerHandlerID = (*env)->GetStaticMethodID(env, theClass,
					       "timerCallbackHandler",
					       TIMER_CALLBACK_SIGNATURE);

  return (jint)IPC_addTimer((unsigned long)tdelay, (unsigned long)count, 
			    ipcJavaTimerHandler, (void *)(long)handlerNum);
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1addTimerGetRef (JNIEnv *env, jclass theClass,
			      jlong tdelay, jlong count,
			      jint handlerNum)
{
  return 0;
}

JNIEXPORT jint JNICALL Java_ipc_java_IPC_IPC_1removeTimer (JNIEnv *env, jclass theClass,
						  jint handlerNum)
{
  return 0;
}

JNIEXPORT jint JNICALL 
Java_ipc_java_IPC_IPC_1removeTimerByRef (JNIEnv *env, jclass theClass, jlong timerRef)
{
  return 0;
}

JNIEXPORT jlong JNICALL Java_ipc_java_IPC_IPC_1timeInMillis (JNIEnv *env,
						    jclass theClass)
{
  return (jlong)x_ipc_timeInMsecs();
}

/*****************************************************************
 *
 * For formatters.java
 * 
 ****************************************************************/

JNIEXPORT jint JNICALL
Java_ipc_java_formatters_formatType (JNIEnv *env, jclass theClass, jlong formatter)
{
  return (jint)(size_t)formatType((FORMAT_PTR)(size_t)formatter);
}

JNIEXPORT jint JNICALL
Java_ipc_java_formatters_formatPrimitiveProc (JNIEnv *env, jclass theClass,
				     jlong formatter)
{
  return (jint)(size_t)formatPrimitiveProc((FORMAT_PTR)(size_t)formatter);
}


JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_formatChoosePtrFormat (JNIEnv *env, jclass theClass,
				       jlong formatter, jlong parentFormat)
{
  return (jlong)(size_t)formatChoosePtrFormat((CONST_FORMAT_PTR)(size_t)formatter,
					      (FORMAT_PTR)(size_t)parentFormat);
}

JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_formatFormatArray (JNIEnv *env, jclass theClass,
				     jlong formatter)
{
  return (jlong)(size_t)formatFormatArray((FORMAT_PTR)(size_t)formatter);
}

JNIEXPORT jint JNICALL
Java_ipc_java_formatters_formatFormatArrayMax (JNIEnv *env, jclass theClass,
				      jlong formatArray)
{
  return (jint)formatFormatArrayMax((FORMAT_ARRAY_PTR)(size_t)formatArray);
}

JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_formatFormatArrayItemPtr (JNIEnv *env, jclass theClass,
					  jlong formatArray, jint n)
{
  return (jlong)(size_t)formatFormatArrayItem((FORMAT_ARRAY_PTR)(size_t)formatArray,
					      (int)n);
}

JNIEXPORT jint JNICALL
Java_ipc_java_formatters_formatFormatArrayItemInt (JNIEnv *env, jclass theClass,
					  jlong formatArray, jint n)
{
  return (jint)(size_t)formatFormatArrayItem((FORMAT_ARRAY_PTR)(size_t)formatArray,
					     (int)n);
}

JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_findNamedFormat (JNIEnv *env, jclass theClass, jlong format)
{
  return (jlong)(size_t)findNamedFormat((FORMAT_PTR)(size_t)format);
}

JNIEXPORT jboolean JNICALL
Java_ipc_java_formatters_checkMarshallStatus (JNIEnv *env, jclass theClass,
				     jlong formatter)
{
  return (checkMarshallStatus((FORMATTER_PTR)(size_t)formatter) == TRUE
	  ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_createBuffer(JNIEnv *env, jclass theClass, jlong byteArray)
{
  BUFFER_PTR buffer = NEW(BUFFER_TYPE);

  buffer->bstart = 0;
  buffer->buffer = (char *)(size_t)byteArray;

  return (jlong)(size_t)buffer;
}

JNIEXPORT void JNICALL
Java_ipc_java_formatters_freeBuffer (JNIEnv *env, jclass theClass, jlong buffer)
{
  free((void *)(size_t)buffer);
}

JNIEXPORT jint JNICALL
Java_ipc_java_formatters_bufferLength (JNIEnv *env, jclass theClass, jlong buffer)
{
  return (jint)((BUFFER_PTR)(size_t)buffer)->bstart;
}

JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_createByteArray (JNIEnv *env, jclass theClass, jint length)
{
  return (jlong)(size_t)x_ipcMalloc(length);
}

#ifdef NEED_DEBUGGING
/* The following functions are for debugging purposes, only */

JNIEXPORT void JNICALL
Java_ipc_java_formatters_rewindBuffer (JNIEnv *env, jclass theClass, jlong buffer)
{
  ((BUFFER_PTR)buffer)->bstart = 0;
}

JNIEXPORT void JNICALL
Java_ipc_java_formatters_printBuffer (JNIEnv *env, jclass theClass, jlong buf)
{
  BUFFER_PTR buffer = (BUFFER_PTR)buf;
  int i;
  fprintf(stderr, "BUFFER: (%d) ", buffer->bstart);
  for (i = 0; i<buffer->bstart; i++) {
    fprintf(stderr, "%2X ", buffer->buffer[i]);
  }
  fprintf(stderr, "\n");
}

JNIEXPORT void JNICALL
Java_ipc_java_formatters_printByteArray (JNIEnv *env, jclass theClass, 
				jlong byteArray, jint length)
{
  int i;
  fprintf(stderr, "BYTE ARRAY: (%d) ", (int)length);
  for (i = 0; i<length; i++) {
    fprintf(stderr, "%2X ", ((char *)byteArray)[i]);
  }
  fprintf(stderr, "\n");
}

JNIEXPORT jlong JNICALL
Java_ipc_java_formatters_parseFormat (JNIEnv *env, jclass theClass, jstring format)
{
  const char *cformat;
  int retVal;

  cformat = (*env)->GetStringUTFChars(env, format, 0);
  retVal = (int)IPC_parseFormat(cformat);
  (*env)->ReleaseStringUTFChars(env, format, cformat);

  return (jlong)retVal;
}
#endif /* NEED_DEBUGGING */

/*****************************************************************
 *
 * For primFmttrs.java
 * 
 ****************************************************************/

JNIEXPORT jchar JNICALL Java_ipc_java_primFmttrs_formatGetChar (JNIEnv *env, jclass c,
						       jlong buffer)
{
  return (jchar)formatGetChar((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutChar (JNIEnv *env, jclass c,
						      jlong buffer, 
						      jchar theChar)
{
  formatPutChar((BUFFER_PTR)(size_t)buffer, (char)theChar);
}

JNIEXPORT jboolean JNICALL Java_ipc_java_primFmttrs_formatGetBoolean (JNIEnv *env,
							     jclass c,
							     jlong buffer)
{
  return (formatGetInt((BUFFER_PTR)(size_t)buffer) == FALSE
	  ? JNI_FALSE : JNI_TRUE);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutBoolean (JNIEnv *env, jclass c,
							 jlong buffer,
							 jboolean theBool)
{
  formatPutInt((BUFFER_PTR)(size_t)buffer,
	       (theBool == JNI_TRUE ? TRUE : FALSE));
}

JNIEXPORT jbyte JNICALL Java_ipc_java_primFmttrs_formatGetByte (JNIEnv *env, jclass c,
						       jlong buffer)
{
  return (jbyte)formatGetByte((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutByte (JNIEnv *env, jclass c,
						      jlong buffer, jbyte byte)
{
  formatPutByte((BUFFER_PTR)(size_t)buffer, (int32)byte);
}

JNIEXPORT jbyte JNICALL Java_ipc_java_primFmttrs_formatGetUByte (JNIEnv *env, jclass c,
						       jlong buffer)
{
  return (jbyte)formatGetUByte((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutUByte (JNIEnv *env, jclass c,
						       jlong buffer, jbyte byte)
{
  formatPutUByte((BUFFER_PTR)(size_t)buffer, (int32)byte);
}

JNIEXPORT jint JNICALL Java_ipc_java_primFmttrs_formatGetInt (JNIEnv *env, jclass c,
						     jlong buffer)
{
  return (jint)formatGetInt((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutInt (JNIEnv *env, jclass c,
						     jlong buffer, jint theInt)
{
  formatPutInt((BUFFER_PTR)(size_t)buffer, (int32)theInt);
}

JNIEXPORT jlong JNICALL Java_ipc_java_primFmttrs_formatGetLong (JNIEnv *env, jclass c,
						       jlong buffer)
{
  return (jlong)formatGetInt((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutLong (JNIEnv *env, jclass c,
						      jlong buffer,
						      jlong theLong)
{
  formatPutInt((BUFFER_PTR)(size_t)buffer, (int32)theLong);
}

JNIEXPORT jfloat JNICALL Java_ipc_java_primFmttrs_formatGetFloat (JNIEnv *env, jclass c,
							 jlong buffer)
{
  return (jfloat)formatGetFloat((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutFloat (JNIEnv *env, jclass c,
						       jlong buffer,
						       jfloat theFloat)
{
  formatPutFloat((BUFFER_PTR)(size_t)buffer, (float)theFloat);
}

JNIEXPORT jdouble JNICALL Java_ipc_java_primFmttrs_formatGetDouble (JNIEnv *env,
							   jclass c,
							   jlong buffer)
{
  return (jdouble)formatGetDouble((BUFFER_PTR)(size_t)buffer);
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutDouble (JNIEnv *env, jclass c,
							jlong buffer,
							jdouble theDouble)
{
  formatPutDouble((BUFFER_PTR)(size_t)buffer, (double)theDouble);
}

JNIEXPORT jstring JNICALL Java_ipc_java_primFmttrs_formatGetString (JNIEnv *env,
							   jclass c, jlong buf)
{
  BUFFER_PTR buffer = (BUFFER_PTR)(size_t)buf;
  int length = formatGetInt(buffer);
#define CBUF_LEN 100
  char charBuffer[CBUF_LEN], *tmp;
  jstring theString;

  tmp = charBuffer;
  if (length == 0) {
    charBuffer[0] = '\0';
    formatGetChar(buffer);
  } else {
    if (length >= CBUF_LEN) {
      /* Need to allocate this array because NewStringUTF needs a 
	 null-terminated string.  Sigh... */
      tmp = (char *)malloc(length+1);
    }
    BCOPY(buffer->buffer+buffer->bstart, tmp, length);
    buffer->bstart += length;
    tmp[length] = '\0';
  }
  theString = (*env)->NewStringUTF(env, tmp);
  if (length >= CBUF_LEN) free(tmp);
  return theString;
}

JNIEXPORT void JNICALL Java_ipc_java_primFmttrs_formatPutString (JNIEnv *env, jclass c,
							jlong buf,
							jstring theString)
{
  BUFFER_PTR buffer = (BUFFER_PTR)(size_t)buf;
  const char *cstring = (*env)->GetStringUTFChars(env, theString, 0);
  int length = strlen(cstring);

  formatPutInt(buffer, length);
  if (length == 0) {
    formatPutChar(buffer, 'Z');
  } else {
    BCOPY(cstring, buffer->buffer+buffer->bstart, length);
    buffer->bstart += length;
  }
  (*env)->ReleaseStringUTFChars(env, theString, cstring);
}

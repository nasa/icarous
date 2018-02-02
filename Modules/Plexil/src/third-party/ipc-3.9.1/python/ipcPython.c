/******************************************************************************
 * PROJECT: IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2011 Reid Simmons.  All rights reserved.
 *
 * FILE: ipcPython.c
 *
 * ABSTRACT: C-code for interfacing specifically with Python
 *           Used by SWIG (see ffi/IPC.i)
 *
 *       $Id: ipcPython.c,v 1.1 2011/08/16 16:00:36 reids Exp $
 * $Revision: 1.1 $
 *     $Date: 2011/08/16 16:00:36 $
 *   $Author: reids $
 *    $State: Exp $
 *   $Locker:  $
 *
 * Copyright (c) 2011, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 * $Log: ipcPython.c,v $
 * Revision 1.1  2011/08/16 16:00:36  reids
 * Adding Python interface to IPC
 *
 ****************************************************************/	

#include "ipcFFI.h"
#include "ipcPython.h"

extern const char *ipcErrorStrings[];

static void ipcPythonMsgHandler (MSG_INSTANCE msg, void *data, void *key)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module, "msgCallbackHandler");
  PyObject *pymsg = SWIG_NewPointerObj(msg, SWIGTYPE_p__X_IPC_REF, 0);
  PyObject *pydata = SWIG_NewPointerObj(data, SWIGTYPE_p_void, 0);
  PyObject *arglist = Py_BuildValue("(OOl)", pymsg, pydata, (long)key);
  PyEval_CallObject(hnd, arglist);
}

IPC_RETURN_TYPE subscribe (const char *msgName, const char *hndName, long key)
{
  return _IPC_subscribe(msgName, hndName, ipcPythonMsgHandler,
                        (void *)(size_t)key, 0);
}

static void ipcPythonFdHandler (int fd, void *key)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module, "fdCallbackHandler");
  PyObject *arglist = Py_BuildValue("(il)", fd, (long)key);
  PyEval_CallObject(hnd, arglist);
}

IPC_RETURN_TYPE subscribeFD (int fd, long key)
{
  return IPC_subscribeFD(fd, ipcPythonFdHandler, (void *)(size_t)key);
}

IPC_RETURN_TYPE unsubscribeFD (int fd)
{
  return IPC_unsubscribeFD(fd, ipcPythonFdHandler);
}

static void ipcPythonConnectHandler (const char *moduleName, void *clientData)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module, "connectCallbackHandler");
  PyObject *arglist = Py_BuildValue("(s)", moduleName);
  PyEval_CallObject(hnd, arglist);
}

static void ipcPythonDisconnectHandler (const char *moduleName,
                                        void *clientData)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module,
                                         "disconnectCallbackHandler");
  PyObject *arglist = Py_BuildValue("(s)", moduleName);
  PyEval_CallObject(hnd, arglist);
}

IPC_RETURN_TYPE subscribeConnect (void)
{
  return IPC_subscribeConnect(ipcPythonConnectHandler, NULL);
}

IPC_RETURN_TYPE subscribeDisconnect (void)
{
  return IPC_subscribeDisconnect(ipcPythonDisconnectHandler, NULL);
}

IPC_RETURN_TYPE unsubscribeConnect (void)
{
  return IPC_unsubscribeConnect(ipcPythonConnectHandler);
}

IPC_RETURN_TYPE unsubscribeDisconnect (void)
{
  return IPC_unsubscribeDisconnect(ipcPythonDisconnectHandler);
}

static void ipcPythonChangeHandler (const char *msgName, int numHandlers,
                                    void *clientData)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module, "changeCallbackHandler");
  PyObject *arglist = Py_BuildValue("(si)", msgName, numHandlers);
  PyEval_CallObject(hnd, arglist);
}

static void ipcPythonQueryHandler (MSG_INSTANCE msg, void *data, void *qhndKey)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module, "queryCallbackHandler");
  PyObject *pymsg = SWIG_NewPointerObj(msg, SWIGTYPE_p__X_IPC_REF, 0);
  PyObject *pydata = SWIG_NewPointerObj(data, SWIGTYPE_p_void, 0);
  PyObject *arglist = Py_BuildValue("(OOl)", pymsg, pydata, (long)qhndKey);
  PyEval_CallObject(hnd, arglist);
}

IPC_RETURN_TYPE subscribeHandlerChange (const char *msgName)
{
  return IPC_subscribeHandlerChange(msgName, ipcPythonChangeHandler, NULL);
}

IPC_RETURN_TYPE unsubscribeHandlerChange (const char *msgName)
{
  return IPC_unsubscribeHandlerChange(msgName, ipcPythonChangeHandler);
}

IPC_RETURN_TYPE queryResponse (const char *msgName, unsigned int length,
 		               BYTE_ARRAY content, IPC_VARCONTENT_PTR vc,
		               FORMATTER_CONTAINER_TYPE *replyFormat,
			       unsigned int timeoutMsecs)
{
  return _IPC_queryResponse(msgName, length, content, &(vc->content),
                            &replyFormat->formatter, timeoutMsecs);
}

IPC_RETURN_TYPE queryNotify (const char *msgName, unsigned int length,
 		             BYTE_ARRAY content, unsigned int handlerIndex)
{
  return IPC_queryNotify(msgName, length, content, ipcPythonQueryHandler,
                         (void *)(size_t)handlerIndex);
}

IPC_RETURN_TYPE printData (FORMATTER_PTR formatter, int fd, 
                           IPC_VARCONTENT_PTR vc)
{
  FILE *file = fdopen(fd, "w");
  void *dataptr;
  IPC_unmarshall(formatter, vc->content, &dataptr);
  IPC_RETURN_TYPE retval = IPC_printData(formatter, file, dataptr);
  IPC_freeData(formatter, dataptr);
  IPC_freeByteArray(vc->content);
  return retval;
}

static void ipcPythonTimerHandler (void *hndIndex, unsigned long currentTime, 
				   unsigned long scheduledTime)
{
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *hnd = PyObject_GetAttrString(IPC_module, "timerCallbackHandler");
  PyObject *arglist = Py_BuildValue("(lll)", (long)hndIndex, 
                                    currentTime, scheduledTime);
  PyEval_CallObject(hnd, arglist);
}

IPC_RETURN_TYPE addTimerGetRef(unsigned long tdelay, long count,
			       unsigned long handlerIndex,
			       TIMER_REF_CONTAINER_TYPE *timerRef)
{
  return IPC_addTimerGetRef(tdelay, count, ipcPythonTimerHandler,
	                    (void *)handlerIndex, &timerRef->timerRef);
}

// For Python, do not exit interpreter on error - raise exception, instead
static void ipcErrorProc (void)
{
  IPC_perror("IPC error detected");
  // Get the specific IPCError exception defined within Python (see below)
  PyObject *IPC_module = PyImport_AddModule("IPC");
  PyObject *IPCError = PyObject_GetAttrString(IPC_module, "IPCError");
  PyErr_SetString(IPCError, ipcErrorStrings[IPC_errno]);
}

void x_ipcRegisterExitProc(void (*proc)(void));

void setExitProc (void)
{
  x_ipcRegisterExitProc(ipcErrorProc);
}


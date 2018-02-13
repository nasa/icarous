/******************************************************************************
 * PROJECT: IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2011 Reid Simmons.  All rights reserved.
 *
 * FILE: ipcPython.h
 *
 * ABSTRACT: C-headers for interfacing specifically with Python
 *           Used by SWIG (see ffi/IPC.i)
 *
 *       $Id: ipcPython.h,v 1.2 2011/08/17 00:47:04 reids Exp $
 * $Revision: 1.2 $
 *     $Date: 2011/08/17 00:47:04 $
 *   $Author: reids $
 *    $State: Exp $
 *   $Locker:  $
 *
 * Copyright (c) 2011, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 * $Log: ipcPython.h,v $
 * Revision 1.2  2011/08/17 00:47:04  reids
 * Removed mention of IPC_freeData and IPC_freeDataElements.
 * Changed the signature of IPC_unmarshall.
 *
 * Revision 1.1  2011/08/16 16:00:36  reids
 * Adding Python interface to IPC
 *
 ****************************************************************/	

#ifndef IPC_PYTHON_H
#define IPC_PYTHON_H

// Needed for queryResponse, to pass back replyFormat
typedef struct { FORMATTER_PTR formatter; } FORMATTER_CONTAINER_TYPE;
// Needed for addTimerGetRef, to pass back timerRef
typedef struct { TIMER_REF timerRef; } TIMER_REF_CONTAINER_TYPE;

IPC_RETURN_TYPE subscribe (const char *msgName, const char *hndName, long key);

IPC_RETURN_TYPE _IPC_subscribe (const char *msgName, const char *hndName,
				HANDLER_TYPE handler, void *clientData,
				int autoUnmarshall);
IPC_RETURN_TYPE _IPC_unsubscribe (const char *msgName, const char *hndName);
IPC_RETURN_TYPE subscribe (const char *msgName, const char *hndName, long key);
IPC_RETURN_TYPE subscribeFD (int fd, long key);
IPC_RETURN_TYPE unsubscribeFD (int fd);
IPC_RETURN_TYPE subscribeConnect (void);
IPC_RETURN_TYPE subscribeDisconnect (void);
IPC_RETURN_TYPE unsubscribeConnect (void);
IPC_RETURN_TYPE unsubscribeDisconnect (void);
IPC_RETURN_TYPE subscribeHandlerChange (const char *msgName);
IPC_RETURN_TYPE unsubscribeHandlerChange (const char *msgName);
IPC_RETURN_TYPE queryResponse (const char *msgName, unsigned int length,
 		               BYTE_ARRAY content, IPC_VARCONTENT_PTR vc,
		               FORMATTER_CONTAINER_TYPE *replyFormat,
			       unsigned int timeoutMsecs);
IPC_RETURN_TYPE queryNotify (const char *msgName, unsigned int length,
 		               BYTE_ARRAY content, unsigned int handlerIndex);
IPC_RETURN_TYPE printData (FORMATTER_PTR formatter, int fd, 
                           IPC_VARCONTENT_PTR vc);
IPC_RETURN_TYPE addTimerGetRef(unsigned long tdelay, long count,
			       unsigned long handlerIndex,
			       TIMER_REF_CONTAINER_TYPE *timerRef);

void setExitProc (void);

#endif // IPC_PYTHON_H

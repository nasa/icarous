/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: comModule.c
 *
 * ABSTRACT:
 * 
 * Module Communications
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/comModule.h,v $ 
 * $Revision: 2.7 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: comModule.h,v $
 * Revision 2.7  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2002/01/03 20:52:10  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/01/31 17:54:11  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.4  2000/07/27 16:59:09  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.3  2000/07/19 20:55:49  reids
 * Added handling for SIGPIPE; fixed handling of direct messages when pipe
 *   is broken.
 * Also, need to reset handlers (maybe??) since, according to the man page for
 *  "signal", the Linux version resets the handler to its default value each
 *  time the signal is raised.
 *
 * Revision 2.2  2000/07/03 17:03:22  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.1  1996/10/22 18:49:31  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.1  1996/05/09 01:01:17  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/03 03:11:08  reids
 * The data logging (-ld) option now works, for both IPC and regular X_IPC msgs.
 * Fixed the interaction between signals and listening for messages.
 * IPC version will not exit if connection to central server is not available.
 * IPC version will not print out stats if module exits (x_ipcModError).
 *
 * Revision 1.2  1996/03/12 03:19:33  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:08  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.17  1996/03/05  05:04:21  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.16  1996/03/02  03:21:30  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.15  1995/10/25  22:48:03  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.14  1995/05/31  19:35:09  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.13  1995/04/21  03:53:15  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.12  1995/04/08  02:06:21  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.11  1995/04/07  05:02:58  rich
 * Fixed GNUmakefiles to find the release directory.
 * Cleaned up libc.h file for sgi and vxworks.  Moved all system includes
 * into libc.h
 * Got direct queries to work.
 * Fixed problem in allocating/initializing generic mats.
 * The direct flag (-c) now mostly works.  Connect message has been extended to
 * indicate when direct connections are the default.
 * Problem with failures on sunOS machines.
 * Fixed problem where x_ipcError would not print out its message if logging had
 * not been initialized.
 * Fixed another memory problem in modVar.c.
 * Fixed problems found in by sgi cc compiler.  Many type problems.
 *
 * Revision 1.10  1995/04/04  19:41:54  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.9  1995/03/30  15:42:36  rich
 * DBMALLOC works.  To use "gmake -k -w DBMALLOC=DBMALLOC install"
 * Added simple list of strings data structure that can be passed via x_ipc
 * messages.
 * Use the string list to maintain a global variable of messages with taps.
 * Tapped messages are not sent via direct connections.
 * Implemented code to vectorize data to be sent so that it does not have
 * to be copied.  Currently, only flat, packed data structures are
 * vectored.  This can now be easily extended.
 * Changed Boolean -> BOOLEAN for consistency and to avoid conflicts with x11.
 * Fixed bug were central would try and free the "***New Module***" and
 * "*** Unkown Host***" strings when a module crashed on startup.
 * Fixed a bug reported by Jay Gowdy where the code to find the size of a
 * variable lenght array would access already freed data when called from
 * x_ipcFreeData.
 *
 * Revision 1.8  1995/01/18  22:39:56  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/17  23:15:24  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1994/04/28  16:15:32  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.5  1993/12/01  18:57:47  rich
 * Changed TCP to  IPPROTO_TCP, a system defined constant.
 *
 * Revision 1.4  1993/12/01  18:02:56  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.3  1993/11/21  20:17:23  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.2  1993/10/21  16:13:39  rich
 * Fixed compiler warnings.
 *
 * Revision 1.1  1993/08/27  07:14:22  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/06/22  13:58:44  rich
 * Added makefile.depend.  Dependencies automatically generated using gcc.
 * Fixed some warnings.
 * Updated the -D<arch> flags to correspond to those generated
 * automatically by the makefile.
 * Changed system includes to the proper format "stdio.h" -> <stdio.h>.
 * This was needed so that the automatic dependency generation can
 * distinguish between "local" and system headers.  The location of the
 * system headers changes from architecture to architecture and should not
 * be included in the dependency list.
 *
 * Revision 1.2  1993/05/27  22:16:27  rich
 * Added automatic logging.
 *
 ****************************************************************/

#ifndef INCcomModule
#define INCcomModule

#define GET_MESSAGE(name) \
(MSG_PTR)x_ipc_hashTableFind(name, GET_C_GLOBAL(messageTable))
#define GET_HANDLER(hndKeyPtr) \
(HND_PTR)x_ipc_hashTableFind((void *)hndKeyPtr, GET_C_GLOBAL(handlerTable))
#define GET_CLASS_FORMAT(classPtr) \
(CLASS_FORM_PTR)x_ipc_hashTableFind((const char *)classPtr, \
			            GET_M_GLOBAL(classFormatTable))
#define GET_FD_HANDLER(fd) \
(FD_HND_PTR)x_ipc_hashTableFind((void *)&fd, GET_M_GLOBAL(externalFdTable))

#define ADD_MESSAGE(name, msg) \
(void)x_ipc_hashTableInsert(name, strlen(name)+1, (const char *)msg, \
		            GET_C_GLOBAL(messageTable))
#define ADD_HANDLER(hndKeyPtr, hnd) \
(void)x_ipc_hashTableInsert((char *)hndKeyPtr, sizeof(HND_KEY_TYPE), \
 		            (char *)hnd, GET_C_GLOBAL(handlerTable))
#define ADD_CLASS_FORMAT(classPtr, classForm) \
(void)x_ipc_hashTableInsert((char *)classPtr, sizeof(X_IPC_MSG_CLASS_TYPE), \
		            (char *)classForm, GET_M_GLOBAL(classFormatTable))

typedef struct {
  X_IPC_VERSION_TYPE version;
  BOOLEAN direct;
} MOD_START_TYPE;

typedef struct {
  X_IPC_REF_PTR ref;
  REPLY_HANDLER_FN handler;
  HND_LANGUAGE_ENUM language;
  void *clientData;
} QUERY_NOTIFICATION_TYPE, *QUERY_NOTIFICATION_PTR;

typedef struct {
  CONNECTION_PTR connection;
  DATA_MSG_PTR   dataMsg;
} QUEUED_MSG_TYPE, *QUEUED_MSG_PTR;

#define MSG_QUEUE_INCR 20

typedef struct {
  QUEUED_MSG_PTR messages;
  int numMessages;
  int queueSize;
} MSG_QUEUE_TYPE, *MSG_QUEUE_PTR;

void initMsgQueue (MSG_QUEUE_PTR msgQueue);
void enqueueMsg (MSG_QUEUE_PTR msgQueue,
		 CONNECTION_PTR connection, DATA_MSG_PTR dataMsg);
QUEUED_MSG_PTR dequeueMsg (MSG_QUEUE_PTR msgQueue);

void x_ipcModuleInitialize(void);

void *x_ipc_decodeDataInLanguage (DATA_MSG_PTR dataMsg, CONST_FORMAT_PTR decodeFormat,
			    HND_LANGUAGE_ENUM language);

X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReplyFrom(X_IPC_REF_PTR ref, void *reply, 
				       BOOLEAN sel, long timeout, int fd);
X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReply(X_IPC_REF_PTR ref, void *reply);
void freeDirectList (MSG_PTR msg);
void x_ipc_establishDirect(MSG_PTR msg);

void x_ipcHandleClosedConnection(int sd, CONNECTION_PTR connection);

#ifdef NMP_IPC
extern MSG_PTR getResponseMsg (DATA_MSG_PTR dataMsg);
#endif

void insertConnect (LIST_PTR *listPtr,
		    char *msgName, X_IPC_HND_FN connectHandler,
		    CONNECT_HANDLE_TYPE handler, void *clientData);
void removeConnect (LIST_PTR list, char *msgName, CONNECT_HANDLE_TYPE handler);

void insertChange (const char *msgName,
		   CHANGE_HANDLE_TYPE handler, void *clientData);
void removeChange (const char *msgName, CHANGE_HANDLE_TYPE handler);

#endif /* INCcomModule */

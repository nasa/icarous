/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: Receive Message
 *
 * FILE: recvMsg.h
 *
 * ABSTRACT:
 * 
 * All messages enter through receive message. 
 * Receive message provides the main thread of flow and control in x_ipc.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/recvMsg.h,v $ 
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: recvMsg.h,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:28  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:54  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:24  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.9  1995/07/12  04:55:23  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.8  1995/05/31  19:36:25  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.7  1995/03/30  15:43:59  rich
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
 * Revision 1.6  1995/01/18  22:42:22  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.5  1994/05/25  04:57:56  rich
 * Defined macros for registering simple messages and handlers at once.
 * Added function to ignore logging for all messages associated with a
 * global variable.
 * Moved module global variable routines to a new file so they are not
 * included in the .sa library file.  Gets better code sharing and lets you
 * debug these routines.
 * Added code to force the module variables to be re-initialized after the
 * server goes down.
 * x_ipcClose now will not crash if the server is down and frees some module
 * memory.
 * The command line flag "-u" turns off the simple user interface.
 * Added routines to free hash tables and id tables.
 *
 * Revision 1.4  1994/05/17  23:17:17  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.3  1994/01/31  18:28:50  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.2  1993/12/01  18:04:21  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.1  1993/08/27  07:16:32  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  17:40:11  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:20:06  rich
 * Added automatic logging.
 *
 *
 ****************************************************************/

#ifndef INCrecvMsg
#define INCrecvMsg

void msgInfoMsgSend(int sd);
void processResAttendDeliver(DISPATCH_PTR dispatch);
void processActiveMessage(DISPATCH_PTR dispatch);
void processHandledMessage(DISPATCH_PTR dispatch);
void processInactiveTests(DISPATCH_PTR dispatch);
void multiQueryUpdate(DISPATCH_PTR dispatch, int32 logStatus);
void recvMessageBuild(MODULE_PTR module, DATA_MSG_PTR dataMsg);
void recvMessage(DISPATCH_PTR dispatch, X_IPC_MSG_CLASS_TYPE msg_class,
		 void *classData);
extern void blockingCommandReply(DISPATCH_PTR dispatch,
				 X_IPC_MSG_CLASS_TYPE msg_class);
extern DISPATCH_PTR buildDispatchInternal(MSG_PTR msg, DATA_MSG_PTR dataMsg,
					  void *classData,
					  DISPATCH_PTR parentDispatch,
					  RESP_PROC_FN resProc, void *resData);
void sendDataMsgWithReply(MSG_PTR msg, DATA_MSG_PTR dataMsg,
			  DISPATCH_PTR parentDispatch,
			  RESP_PROC_FN respProc, void *respData);
void sendDataMsg(MSG_PTR msg, DATA_MSG_PTR dataMsg, void *classData,
		 DISPATCH_PTR parentDispatch);
X_IPC_RETURN_VALUE_TYPE centralSendMessage(X_IPC_REF_PTR ref, MSG_PTR msg,
					 void *msgData, void *classData);
X_IPC_RETURN_VALUE_TYPE centralSendResponse(DISPATCH_PTR dispatch, MSG_PTR msg,
					  void *resData,
					  X_IPC_MSG_CLASS_TYPE resClass,
					  void *resClassData,
					  int sd);
void classInitialize(void);
void recvSuccessFailureMessage (DISPATCH_PTR dispatch,
				X_IPC_MSG_CLASS_TYPE msg_class);

#endif /* INCrecvMsg */

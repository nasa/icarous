/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: x_ipc
 *
 * FILE: msgTap.h
 *
 * ABSTRACT: Type definitions needed for the "wire tapping" mechanism
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: msgTap.h,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:27  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:45  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:04  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.12  1995/12/17  20:21:47  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.11  1995/10/07  19:07:36  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.10  1995/05/31  19:36:06  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.9  1995/03/30  15:43:41  rich
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
 * Revision 1.8  1995/01/18  22:41:31  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/17  23:16:53  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1994/01/31  18:28:27  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.5  1993/12/01  18:04:04  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:18:42  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:15:50  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:23  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:46  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:31:18  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:49  fedor
 * Added Logging.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCmsgTap
#define INCmsgTap

typedef struct {
  TAP_CONDITION_TYPE condition;
  MSG_PTR listeningMsg;
} TAP_TYPE, *TAP_PTR;

typedef struct _TAP_INFO {
  DISPATCH_PTR tappedDispatch;
  struct _LIST *extraTaps;
} TAP_INFO_TYPE, *TAP_INFO_PTR;

typedef struct {
  TAP_CONDITION_TYPE condition;
  const char *tappedMsg;
  const char *listeningMsg;
} TAP_MSG_TYPE, *TAP_MSG_PTR;

typedef struct {
  TAP_CONDITION_TYPE condition;
  X_IPC_REF_PTR tappedRef;
  const char *listeningMsg;
} TAP_REF_TYPE, *TAP_REF_PTR;

#define DISPATCH_TAPS(dispatch) \
(((dispatch)->msg) ? (dispatch)->msg->tapList : NULL)

#define DISPATCH_EXTRA_TAPS(dispatch) \
(((dispatch)->tapInfo) ? (dispatch)->tapInfo->extraTaps : NULL)

#define IS_LISTENING(dispatch) \
(((dispatch)->tapInfo) ? (dispatch)->tapInfo->tappedDispatch != NULL : FALSE)

#define X_IPC_TAPPED_MSG_VAR "X_IPC_TAPPED_MSG_VAR"

#define X_IPC_BROADCAST_MSG_VAR "X_IPC_BROADCAST_MSG_VAR"

int32 checkTaps(MSG_PTR msg, TAP_PTR tap);
void freeTapInfo(TAP_INFO_PTR *tapInfo);

void tapWhenSent(DISPATCH_PTR dispatch);
void tapBeforeHandling(DISPATCH_PTR dispatch);
void tapWhileHandling(DISPATCH_PTR dispatch);
void tapAfterHandled(DISPATCH_PTR dispatch);
void tapAfterSuccess(DISPATCH_PTR dispatch);
void tapWhenSuccess(DISPATCH_PTR dispatch);
void tapAfterFailure(DISPATCH_PTR dispatch);
void tapWhenFailure(DISPATCH_PTR dispatch);
void tapAfterReplied(DISPATCH_PTR dispatch);

void setUpBeforeTaps(DISPATCH_PTR dispatch);
void setUpDuringAndAfterTaps(DISPATCH_PTR dispatch);

void tapInitialize(void);

#endif /* INCmsgTap */

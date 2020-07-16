/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: Receive Message
 *
 * FILE: recvMsg.c
 *
 * ABSTRACT:
 * 
 * All messages enter through receive message. 
 * Receive message provides the main thread of flow and control in x_ipc.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/recvMsg.c,v $ 
 * $Revision: 2.7 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: recvMsg.c,v $
 * Revision 2.7  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2002/01/03 20:52:16  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/01/12 15:55:03  reids
 * Improved handling of queries, especially when sender does not expect a reply
 *
 * Revision 2.4  2000/07/27 16:59:11  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.3  2000/07/03 17:03:28  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/01/27 20:51:48  reids
 * Changes for RedHat 6 (and also to remove compiler warnings).
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.3  1996/12/18 15:13:05  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.2  1996/10/24 15:19:24  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.2.2.1  1996/10/18 18:14:42  reids
 * Distinguish when ref and parent id's should be logged.
 *
 * Revision 1.2  1996/05/24 16:46:01  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:53  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:22  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.38  1996/07/25  22:24:30  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.37  1996/07/19  18:14:24  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.36  1996/06/25  20:51:21  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.35  1996/02/06  19:05:03  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.34  1996/01/27  21:54:08  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.33  1995/10/29  18:26:58  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.32  1995/10/25  22:48:43  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.31  1995/08/14  21:32:01  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.30  1995/08/06  16:44:07  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.29  1995/08/05  21:11:53  reids
 * The "sharedBuffers" flag was not always being set.
 *
 * Revision 1.28  1995/08/05  18:13:21  rich
 * Fixed problem with x_ipc_writeNBuffers on partial writes.
 * Added "sharedBuffers" flag to the dataMsg structure, rather than
 * checking to see if the dataStruct pointer and the message data pointer
 * are the same.  This allows central to clear the dataStruc pointer so
 * that messages don't try to access old data structures that might have
 * changed since the  message was created.
 *
 * Revision 1.27  1995/07/19  14:26:23  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.26  1995/07/12  04:55:18  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.25  1995/07/10  16:18:29  rich
 * Interm save.
 *
 * Revision 1.24  1995/06/14  03:22:10  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.23  1995/05/31  19:36:21  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.22  1995/04/19  14:28:47  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.21  1995/03/30  15:43:57  rich
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
 * Revision 1.20  1995/03/28  01:14:55  rich
 * - Added ability to log data with direct connections.  Also fixed some
 * problems with global variables. It now uses broadcasts for watching variables.
 * - Added preliminary memory recovery routines to handle out of memory
 * conditions.  It currently purges items from resource queues.  Needs to
 * be tested.
 * - If the CENTRALHOST environment variable is not set, try the current
 * host.
 * - Fixed a problem with central registered messages that caused the parsed
 * formatters to be lost.
 * - Added const declarations where needed to the prototypes in x_ipc.h.
 * - x_ipcGetConnections: Get the fd_set.  Needed for direct connections.
 * - Added x_ipcExecute and x_ipcExecuteWithConstraints.  Can "execute" a goal
 *   or command.
 * - x_ipcPreloadMessage: Preload the definition of a message from the
 *   central server.
 *
 * Revision 1.19  1995/01/30  16:18:26  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.18  1995/01/18  22:42:18  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.17  1994/10/25  17:10:44  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.16  1994/05/17  23:17:14  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.15  1994/04/28  16:17:02  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.14  1994/04/16  19:43:03  rich
 * First release of X_IPC for the DEC alpha.
 * Changes were needed because longs are 64 bits.
 * Fixed alignment assumption in the data message format.
 * Fixed the way offsets are calculated for variable length arrays.  This
 * was a problem even without 64 bit longs and pointers.
 *
 * Added the commit date to the version information printed out with the -v
 * option.
 *
 * Now uses standard defines for byte order
 * (BYTE_ORDER = BIG_ENDIAN, LITTLE_ENDIAN or PDP_ENDIAN)
 *
 * Defined alignment types: ALIGN_INT ALINE_LONGEST and ALIGN_WORD.
 *
 * *** WARNING ***
 * sending longs between alphas and non-alpha machines will probably not work.
 * *** WARNING ***
 *
 * Revision 1.13  1994/04/04  16:01:29  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.12  1994/03/28  02:23:10  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.11  1994/03/27  22:50:38  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.10  1994/01/31  18:28:42  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.9  1993/12/14  17:34:54  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.8  1993/11/21  20:19:17  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.7  1993/08/31  05:47:14  fedor
 * Added back quick temp fix to displaying X_IPC_INTERVAL_TYPE correctly
 *
 * Revision 1.6  1993/08/27  08:38:53  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.5  1993/08/27  07:16:30  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.6  1993/08/23  17:40:08  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.5  1993/07/08  05:39:08  rich
 * Added function prototypes
 *
 * Revision 1.4  1993/07/08  03:02:39  reids
 * Fixed a bug that Broadcast messages that did not
 * have any handlers registered were not being freed (at all) in central.
 *
 * Revision 1.3  1993/06/22  14:00:16  rich
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
 * Revision 1.2  1993/05/27  22:20:04  rich
 * Added automatic logging.
 *
 * 13-Oct-92  Reid Simmons, School of Computer Science, CMU
 * Updated InformClass messages (used to be ConstraintClass) to issue a
 * response.
 * Added BroadcastClass and MultiQueryClass messages.
 * Broke "recvMessage" into more manageable chunks.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 14-Aug-91 Christopher Fedor, School of Computer Science, CMU
 * Added Richard Goodwin's change to set tplConstr = NO_TPLCONSTR
 * for PollingMonitorClass and DemonMonitorClass in recvMessage.
 *
 * 26-Jul-91  Reid Simmons, School of Computer Science, CMU
 * Messages added to "toBeKilled" nodes were not being handled correctly.
 *
 * 02-Jul-91  Reid Simmons, School of Computer Science, CMU
 * Fixed up some memory management problems, including freeing class data.
 *
 * 14-May-91 Christopher Fedor, School of Computer Science, CMU
 * Moved dispatchUpdateAndDisplay to dispatch routine collection.
 *
 * 12-Mar-91  Reid Simmons, School of Computer Science, CMU
 * Added calls to for tapping messages.
 *
 *  1-Nov-90 Christopher Fedor, School of Computer Science, CMU
 * Rewritten for version x_ipc 5.x
 *
 * 29-May-90 Christopher Fedor, School of Computer Science, CMU
 * Replaced DReply with centralReply which performs a x_ipcReply for
 * centrally handled queries.
 *
 * 23-Apr-90 Reid Simmons, School of Computer Science, CMU
 * Added initial hacks for choosing between multiple handlers.
 *
 * 28-Nov-89 Long-Ji Lin, School of Computer Science, CMU
 * Added code to dispatchServer() to choose exception handler 
 * and to add task tree node for new message.
 *
 * 11-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Changed "goaltree" to "tasktree".
 *
 *  3-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Added code to (selectively) ignore logging 
 * certain messages (registration msgs)
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Changed "printf"s to use logging facility.
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Added call to "Update_Last_Child".
 *
 * 10-July-89 Reid Simmons, School of Computer Science, CMU
 * Split off from dispatch.c all the server (central) dependent routines.
 *
 *  9-May-89 Christopher Fedor, School of Computer Science, CMU
 * Added "FakeReplyHnd" to DReply. DReply should go!
 *
 *  9-May-89 Reid Simmons, School of Computer Science, CMU
 * Moved call to AddTplConstraints.
 *
 *  6-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * Divided to partition module/server calls.
 *
 * 24-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Implemented ReplyFreeList as an idtable.
 *
 * 23-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Replaced ReadDataMsg and WriteDataMsg.
 *
 *    Dec-88 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 *****************************************************************************/

#include "globalS.h"
#include "recvMsg.h"

/*************************************************/

static DISPATCH_HND_PTR chooseMsgHandler(MSG_PTR msg)
{
  int32 size;
  LIST_PTR hndList;
  DISPATCH_HND_PTR hnd, nextHnd, retHnd;
  
  hndList = msg->hndList;
  size = x_ipc_listLength(hndList);
  
  hnd = (DISPATCH_HND_PTR)x_ipc_listFirst(hndList);
  
  if (!size) {
    size = 1;
    hnd = NEW(DISPATCH_HND_TYPE);
    hnd->sd = 0;
    hnd->localId =0;
    hnd->msg = NULL;
    hnd->hndProc = NULL;
    hnd->hndOrg = GET_S_GLOBAL(x_ipcServerModGlobal);
    hnd->hndData = NULL;
    hnd->msgList = NULL;
    hnd->resource = resourceCreate(0, 0, msg->msgData->name, 1);
    hnd->resource->capacity = 0;
    hnd->hndLanguage = C_LANGUAGE;
#ifdef NMP_IPC
    hnd->clientData = NO_CLIENT_DATA;
#endif
    
    x_ipc_listInsertItem((char *)hnd, msg->hndList);
  }
  
  if (size > 1) {
    retHnd = hnd;
    nextHnd = (DISPATCH_HND_PTR)x_ipc_listNext(hndList);
    while (nextHnd && nextHnd->hndData &&
	   STREQ(hnd->hndData->hndName, nextHnd->hndData->hndName)) {
      if (resourceAvailableRes(nextHnd->resource))
	retHnd = nextHnd;
      nextHnd = (DISPATCH_HND_PTR)x_ipc_listNext(hndList);
    }
    hnd = !nextHnd ? retHnd : NULL;
  }
  
  return hnd;
}

/*************************************************/

static void hndSelect(DISPATCH_PTR dispatch)
{
  DISPATCH_HND_PTR hnd;
  
#ifndef NMP_IPC
  if (dispatch->msg->msgData->msg_class == ExceptionClass) {
    if (dispatch->hnd)
      return; /* assumes set by byPassExcepHnd or retryHnd */
    hnd = chooseExceptionHandler(dispatch);
  }
  else
#endif
    hnd = chooseMsgHandler(dispatch->msg);
  
  dispatch->hnd = hnd;
  
  if (!hnd)
    return;
  
  dispatch->des = hnd->hndOrg;
  dispatch->desId = hnd->hndOrg->writeSd;
}

/*************************************************/

void processResAttendDeliver(DISPATCH_PTR dispatch)
{
  resourceAttending(dispatch);
#ifndef NMP_IPC
  AttendingConstraints(dispatch);
#endif
  deliverDispatch(dispatch);
  tapWhileHandling(dispatch);
  /* 02-Jul-91: Reid: 
   * Just handled a central query or inform: 
   * It's OK to free the dispatch here 
   */
  if (dispatch->msg_class == ReplyClass ||
      (dispatch->msg_class == InformClass && 
       dispatch->des == GET_S_GLOBAL(x_ipcServerModGlobal)))
    dispatchFree(dispatch);
}

/*************************************************/

static void processResourceAndDeliver(DISPATCH_PTR dispatch)
{
#ifndef NMP_IPC
  if (!dispatch->treeNode) {
    tapBeforeHandling(dispatch);
  }
#endif
  if (resourceAvailable(dispatch)) 
    processResAttendDeliver(dispatch);
  else {
    resourcePending(dispatch);
#ifndef NMP_IPC
    PendingConstraints(dispatch);
#endif
    dispatchUpdateAndDisplay(PendingDispatch, dispatch);
  }
}

/*************************************************/

void processActiveMessage(DISPATCH_PTR dispatch)
{
  hndSelect(dispatch);
  /* Moved here from "recvMessage" because need to wait until handler
     selected, (reids 10-92) */ 
  if (dispatch->msg_class == QueryClass && !resourceSelfQuery(dispatch))
    pendingReplyAdd(dispatch);
  
  if (DISPATCH_HANDLER_CHOOSEN(dispatch)) {
    processResourceAndDeliver(dispatch);
  } else {
    X_IPC_ERROR1("ERROR: processActiveMessage: No Handler Choosen for %s.\n",
	     dispatch->msg->msgData->name);
  }
}

/*************************************************/

void processHandledMessage(DISPATCH_PTR dispatch)
{
  dispatchUpdateAndDisplay(HandledDispatch, dispatch);
  resourceRemoveAttending(dispatch);
  
  tapAfterHandled(dispatch);
  if (dispatch->msg && !ONE_WAY_MSG(dispatch->msg)) {
    switch (dispatch->msg_class) {
    case SuccessClass: 
#ifndef NMP_IPC
      tapWhenSuccess(dispatch); 
      tapAfterSuccess(dispatch); 
#endif
      break;
#ifndef NMP_IPC
    case FailureClass: 
      tapWhenFailure(dispatch);
      tapAfterFailure(dispatch);
      break;
#endif
    default:
      /* X_IPC_MOD_ERROR("Unhandled default"); */
      X_IPC_MOD_WARNING1("unhandled default in  processHandledMessage %d\n",
		    dispatch->msg_class);
      break;
    }
  }
  
  resourceProcessPending(dispatch);
#ifndef NMP_IPC
  CompletionConstraints(dispatch);
  HandleKillAfterAttendingNodes(dispatch);
#endif
  if (!dispatch->msg || ONE_WAY_MSG(dispatch->msg))
    dispatchFree(dispatch);
  
  /* 14-May-91: fedor: exception class messages that are byPassed or retry
     should not be dispatch freed arbitrarly - may need to declare the first
     message handled and then resend the same dispatch through recvMessage */
}

/*************************************************/

#ifndef NMP_IPC
/* 22-May-91:fedor: another level of flow and control for exception handling */
void processInactiveTests(DISPATCH_PTR dispatch)
{
  if (NeedToKillMessage(dispatch)) {
    dispatch->status = InactiveDispatch;
    dispatch->treeNode->status = ((dispatch->refCount <= 0)
				  ? KilledNode : ToBeKilledNode);
    /* RGS: 12/1/93: If this is a blocking command, need to issue a reply
     * in order to prevent deadlock
     */
    if (dispatch->blockCom && dispatch->blockCom->waitFlag &&
	dispatch->msg && dispatch->msg->msgData->msg_class == CommandClass) {
      blockingCommandReply(dispatch, FailureDispatch);
    }
  } else if (ATTENDABLE(dispatch->treeNode)) 
    processActiveMessage(dispatch);
  else {
    InactiveConstraints(dispatch);
    dispatchUpdateAndDisplay(InactiveDispatch, dispatch);
  }
}
#endif

/*************************************************/

void recvMessageBuild(MODULE_PTR module, DATA_MSG_PTR dataMsg)
{
  char *classData;
  MSG_PTR msg = NULL;
  
  DISPATCH_PTR dispatch;
  X_IPC_MSG_CLASS_TYPE msg_class;
  CLASS_FORM_PTR classForm;
  
  CONST_FORMAT_PTR classFormat = NULL;
  
  if (dataMsg->intent != NO_REF)
    msg = (MSG_PTR)idTableItem(ABS(dataMsg->intent), GET_C_GLOBAL(msgIdTable));
  
  msg_class = (X_IPC_MSG_CLASS_TYPE)dataMsg->classId;
  classForm = GET_CLASS_FORMAT(&msg_class);
  
  if (classForm)
    classFormat = classForm->format;
  
  classData = (char *)x_ipc_dataMsgDecodeClass(classFormat, dataMsg);
  
  if (dataMsg->dispatchRef != NO_REF) {
    dispatch = DISPATCH_FROM_ID(dataMsg->dispatchRef);
  } else {
    dispatch = dispatchAllocate();
    dispatch->msg = msg;
    
    dispatch->org = module;
    dispatch->orgId = module->readSd;
    
    dispatch->refId = dataMsg->msgRef;
    dispatch->pRef = dataMsg->parentRef;
  }
  
  if (dispatch == NULL)
    /* Probably a reply that was not needed */
    return;
  
  if (dataMsg->msgRef == NO_REF)
    dispatchSetResData(dataMsg, dispatch);
  else 
    dispatchSetMsgData(dataMsg, dispatch);
  
  
  /* 22-May-91: fedor: NOTE: class and classData will change if this
     is a previously created dispatch. This may be undesired for
     behavior for a x_ipc_sendResponse call that refers to a previously
     created dispatch. */
  /*  3-Jul-91: reid: Clean up previous class data, 
      except for command class data, which is stored in blockCom */
  if (dispatch->classData && dispatch->msg_class != CommandClass) 
    x_ipc_classDataFree(dispatch->msg_class, dispatch->classData);

  /* RGS 1/11/01: This prevents a reply from being sent when it shouldn't be.
     The problem is that with IPC, one can have a message sent to a query
     handler without expecting a reply (e.g, using a broadcast rather than
     a QueryClass). */
  if (msg_class == ReplyClass && 
      (dispatch->msg_class != QueryClass &&
       dispatch->msg_class != MultiQueryClass)) {
    msg_class = SuccessClass;
  }

  dispatch->msg_class = msg_class;
  dispatch->classData = classData;
  
  recvMessage(dispatch, msg_class, classData);
}

/*************************************************/

#ifndef NMP_IPC
static void recvTaskTreeMessage (DISPATCH_PTR dispatch,
				 X_IPC_MSG_CLASS_TYPE msg_class, void *classData)
{
  int32 tplConstr;
  
  if (msg_class == CommandClass) {
    dispatch->blockCom = (BLOCK_COM_PTR)classData;
    tplConstr = dispatch->blockCom->tplConstr;
    
    /* 28-Oct-91: fedor: Blah! Need some way to see if this blocking
       command is a self query style - or if that should be allowed? */
    if (dispatch->blockCom->waitFlag)
      pendingReplyAdd(dispatch);
  } else if (msg_class == PollingMonitorClass || msg_class == DemonMonitorClass) {
    tplConstr = NO_TPLCONSTR;
  } else
    tplConstr = *(int32 *)classData;
  
  if (!dispatch->treeNode) {
    Initialize_TaskTreeNode(dispatch, dispatch->pRef);
  } else if (DEAD_NODE(dispatch->treeNode)) {
    /* 29-Jul-91: Reid: Stop if this message has already been killed. */
    /* RGS: 12/1/93: If this is a blocking command, need to issue a reply
     * in order to prevent deadlock
     */
    if (dispatch->blockCom && dispatch->blockCom->waitFlag &&
	dispatch->msg && dispatch->msg->msgData->msg_class == CommandClass) {
      blockingCommandReply(dispatch, FailureDispatch);
    }
    return;
  }
  
  addTplConstraints(dispatch, tplConstr);
  
  if ((msg_class == PollingMonitorClass) || (msg_class == DemonMonitorClass)) {
    setIntervalMonitorStartTime(dispatch, (INTERVAL_MON_CLASS_PTR)classData);
  }
  
  (void)Update_Last_Child(dispatch->treeNode);
  
  tapWhenSent(dispatch);
  setUpBeforeTaps(dispatch);
  
  processInactiveTests(dispatch);
  
  setUpDuringAndAfterTaps(dispatch);
}

void blockingCommandReply(DISPATCH_PTR dispatch, X_IPC_MSG_CLASS_TYPE msg_class)
{
  DISPATCH_PTR replyDispatch;
  
  replyDispatch = dispatchAllocate();
  replyDispatch->msg = dispatch->msg;
  
  replyDispatch->org = dispatch->org;
  replyDispatch->orgId = dispatch->orgId;
  
  replyDispatch->refId = dispatch->refId;
  
  DISPATCH_SET_STATUS(AttendingDispatch, replyDispatch);
  
  centralReply(replyDispatch, (char *)&msg_class);
}
#endif

/* 9-Oct-90: fedor - check durablFn in mon.c when implementing failure */
void recvSuccessFailureMessage (DISPATCH_PTR dispatch,
				X_IPC_MSG_CLASS_TYPE msg_class)
{
#ifdef UNUSED_PRAGMA
#pragma unused(msg_class)
#endif
#ifndef NMP_IPC
  if (dispatch->blockCom && 
      dispatch->msg && dispatch->msg->msgData->msg_class == CommandClass) {
    if (dispatch->blockCom->waitFlag) {
      blockingCommandReply(dispatch, msg_class);
    }
    x_ipcFree((char *)dispatch->blockCom);
    dispatch->blockCom = NULL;
  }
#endif
  
  processHandledMessage(dispatch);
}

static int32 processBroadcast(DISPATCH_PTR dispatch, DISPATCH_HND_PTR hnd)
{
  if (hnd != (DISPATCH_HND_PTR)x_ipc_listFirst(dispatch->msg->hndList)) {
    dispatch = dispatchCopy(dispatch);
  }
  dispatch->hnd = hnd;
  dispatch->des = hnd->hndOrg;
  dispatch->desId = hnd->hndOrg->writeSd;
  processResourceAndDeliver(dispatch);
  return TRUE;
}

static void recvBroadcastMessage (DISPATCH_PTR dispatch)
{
  tapWhenSent(dispatch);
  if (x_ipc_listLength(dispatch->msg->hndList) == 0) {
    dispatchFree(dispatch);
  } else {
    (void)x_ipc_listIterateFromLast((LIST_ITER_FN)processBroadcast, 
			      (char *)dispatch, 
			      dispatch->msg->hndList);
  }
}

#ifndef NMP_IPC
/*
   The straightforward way to do this would be using "centralReply", 
   but that calls "recvMessage" recursively, which breaks things.
   */
static void sendMultiQueryTermination (DISPATCH_PTR dispatch)
{
  static DATA_MSG_PTR nullReplyData = NULL;
  
  if (!nullReplyData) {
    nullReplyData = x_ipc_dataMsgCreate(0, QUERY_REPLY_INTENT, 0, 0, 0, 
				  (FORMAT_PTR)NULL,
				  (char *)NULL, 
				  (FORMAT_PTR)NULL, (char *)NULL);
  }
  
  nullReplyData->msgRef = dispatch->refId;
  
  if (!dispatch->org)
    X_IPC_ERROR("Error: sendMultiQueryTermination: No query origin.");
  else {
    if (dispatch->org->alive) {
      (void)x_ipc_dataMsgSend(dispatch->org->writeSd, nullReplyData);
    } else if (!dispatch->org->repliesPending) {
      moduleFree(dispatch->org);
    }
  }
}

static void recvMultiQueryMessage (DISPATCH_PTR dispatch)
{
  tapWhenSent(dispatch);
  /* 
   * Need to "park" the class data somewhere, so that it will not be freed
   * when a reply comes in.  Instead of adding another slot to "dispatch",
   * just reuse the "blockCom" slot
   */
  dispatch->blockCom = (BLOCK_COM_PTR)dispatch->classData;
  dispatch->classData = NULL;
  
  MULTI_QUERY_NUM(dispatch) = x_ipc_listLength(dispatch->msg->hndList);
  if (MULTI_QUERY_MAX(dispatch) > MULTI_QUERY_NUM(dispatch)) {
    MULTI_QUERY_MAX(dispatch) = MULTI_QUERY_NUM(dispatch);
  }
  
  if (MULTI_QUERY_MAX(dispatch) == 0) {
    sendMultiQueryTermination(dispatch);
    /* Free the class data, saved in "blockCom" slot */
    x_ipcFree((char *)dispatch->blockCom);
    dispatchFree(dispatch);
  } else {
    (void)x_ipc_listIterateFromLast((LIST_ITER_FN)processBroadcast, 
			      (char *)dispatch, 
			      dispatch->msg->hndList);
  }
}

void multiQueryUpdate(DISPATCH_PTR dispatch, int32 logStatus)
{
  --MULTI_QUERY_NUM(dispatch);
  --MULTI_QUERY_MAX(dispatch);
  if (MULTI_QUERY_MAX(dispatch) == 0) {
    sendMultiQueryTermination(dispatch);
  } else if (logStatus && MULTI_QUERY_MAX(dispatch) < 0) {
    LOG_STATUS("    MultiQuery: Max replies already received; this reply not actually sent\n");
  }
  if (MULTI_QUERY_NUM(dispatch) == 0) {
    /* Free the class data, saved in "blockCom" slot */
    x_ipcFree((char *)dispatch->blockCom);
  }
}
#endif

static void recvReplyMessage (DISPATCH_PTR dispatch)
{
  dispatchUpdateAndDisplay(HandledDispatch, dispatch);
  resourceRemoveAttending(dispatch);
  /* Added for safety, the dispatch message could have been 
   * freed if there was an error.  */
  if ((dispatch->msg == NULL) || (dispatch->msg->msgData == NULL))
    return;
  if (dispatch->msg->msgData->msg_class != MultiQueryClass ||
      MULTI_QUERY_MAX(dispatch) > 0) {
    deliverResponse(dispatch);
    tapAfterReplied(dispatch);
    tapAfterHandled(dispatch);
  }

#ifndef NMP_IPC
  if (dispatch->msg->msgData->msg_class == MultiQueryClass) {
    multiQueryUpdate(dispatch, TRUE);
  }
#endif

  /* 31-Oct-90: fedor: problem with replies that are not simply
     replies to standard query messages ...etc. sooo we need to
     keep the dispatch around but perhaps not the datamsg.
     This just postpones the eventual memory lossage */
  
  x_ipc_dataMsgFree(dispatch->msgData);
  x_ipc_dataMsgFree(dispatch->resData);
  dispatch->msgData = NULL;
  dispatch->resData = NULL;
  
  /* 31-Oct-90: fedor: also need to do free before processing further
     otherwise we may loop and never free anything */
  
  resourceProcessPending(dispatch);
  
  /* 02-Jul-91: reid: Centrally handled dispatch queries are
   * freed in "processResAttendDeliver" 
   */
  if (dispatch->des != GET_S_GLOBAL(x_ipcServerModGlobal))
    dispatchFree(dispatch);
}

void recvMessage(DISPATCH_PTR dispatch, X_IPC_MSG_CLASS_TYPE msg_class, 
		 void *classData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(classData)
#endif
  if (GET_S_GLOBAL(x_ipcDebugGlobal)) {
    x_ipc_idTablePrintInfo(GET_S_GLOBAL(dispatchTable));
    X_IPC_MOD_WARNING("\n");
    x_ipcStats(stderr);
  }
  
  switch(msg_class) {
  case QueryClass:
  case InformClass:
    tapWhenSent(dispatch);
    processActiveMessage(dispatch);
    break;
  case ReplyClass:
    recvReplyMessage(dispatch);
    break;
    
#ifndef NMP_IPC
  case GoalClass:
  case PointMonitorClass:
  case DemonMonitorClass:
  case PollingMonitorClass:
  case ExceptionClass:
  case CommandClass:
    recvTaskTreeMessage(dispatch, msg_class, classData);
    break;
#endif
    
  case BroadcastClass:
    recvBroadcastMessage(dispatch);
    break;
    
#ifndef NMP_IPC
  case MultiQueryClass:
    recvMultiQueryMessage(dispatch);
    break;
#endif
    
  case SuccessClass:
  case FailureClass:
    recvSuccessFailureMessage(dispatch, msg_class);
    break;
    
#ifndef NMP_IPC
  case FireDemonClass:
    LOG_MESSAGE2("%-9s %15s [%d]:", "FireDemon",
		dispatch->msg->msgData->name, *(int32 *)classData);
    Log_RefId(dispatch, LOGGING_MESSAGE);
    LOG_MESSAGE1("%10s", dispatch->org->modData->modName);
    LOG_MESSAGE1(" --> %-15s (Sent)",
		GET_S_GLOBAL(x_ipcServerModGlobal)->modData->modName);
    Log_Time(1); 
    LOG_MESSAGE("\n");
    fireDemonHnd(dispatch, (int32 *)classData);
    break;
#endif
  case HandlerRegClass:
  case UNKNOWN:
  case ExecHndClass:
#ifndef TEST_CASE_COVERAGE
  default:
#endif
    X_IPC_ERROR1("ERROR: recvMessage: class not implemented: %d\n", msg_class);
  }
}

/*************************************************/

DISPATCH_PTR buildDispatchInternal(MSG_PTR msg, DATA_MSG_PTR dataMsg,
				   void *classData, 
				   DISPATCH_PTR parentDispatch,
				   RESP_PROC_FN resProc, void *resData)
{
  int32 savedRef;
  DISPATCH_PTR dispatch;
  
  if (!msg) {
    X_IPC_ERROR("ERROR: sendDataMsg: NULL msg.\n");
  }
  
  /* save the message ref to use in case x_ipc_msgFind triggers 
     a recursive call to x_ipc_sendMessage */
  savedRef = x_ipc_nextSendMessageRef();
  
  dispatch = dispatchAllocate();
  dispatch->msg = msg;
  
  dispatch->org = GET_S_GLOBAL(x_ipcServerModGlobal);
  dispatch->orgId = GET_C_GLOBAL(serverWrite);
  
  dispatch->refId = savedRef;
  
  dispatchSetMsgData(dataMsg, dispatch);
  
  if (resProc) {
    dispatch->respProc = resProc;
    dispatch->respData = (char *)resData;
  }
  
  if (parentDispatch)
    dispatch->pRef = parentDispatch->locId;
  
  dispatch->msg_class = msg->msgData->msg_class;
  dispatch->classData = (char *)classData;
  
  return dispatch;
}


/*******************************************************/

static void sendDataMsgInternal(MSG_PTR msg, DATA_MSG_PTR dataMsg, 
				void *classData,
				DISPATCH_PTR parentDispatch, 
				RESP_PROC_FN resProc,
				void *resData)
{
  DISPATCH_PTR dispatch;
  
  dispatch = buildDispatchInternal(msg, dataMsg, classData, parentDispatch, 
				   resProc, resData);
  recvMessage(dispatch, dispatch->msg_class, classData);
}


/*******************************************************/

void sendDataMsgWithReply(MSG_PTR msg, DATA_MSG_PTR dataMsg,
			  DISPATCH_PTR parentDispatch,
			  RESP_PROC_FN respProc, void *respData)
{
  sendDataMsgInternal(msg, dataMsg, (char *)NULL, parentDispatch, 
		      respProc, respData);
}

/*******************************************************/

void sendDataMsg(MSG_PTR msg, DATA_MSG_PTR dataMsg, void *classData,
		 DISPATCH_PTR parentDispatch)
{
  sendDataMsgInternal(msg, dataMsg, classData, parentDispatch, NULL,
		      (char *)NULL);
}

/*******************************************************/

static void formatterSend(int sd, CONST_FORMAT_PTR form)
{
  DATA_MSG_PTR dataMsg;
  NAMED_FORMAT_PTR formatFormat;
  
  formatFormat = 
    (NAMED_FORMAT_PTR)x_ipc_hashTableFind("format", 
				    GET_M_GLOBAL(formatNamesTable));
  
  if (!formatFormat) {
    X_IPC_ERROR("ERROR: formatterSend: no formatFormat");
  }
  dataMsg = x_ipc_dataMsgCreate(0, 0, 0, 0, 0, formatFormat->format, (void *)&form, 
			  (FORMAT_PTR)NULL, (void *)NULL);
  
  (void)x_ipc_dataMsgSend(sd, dataMsg);
  
  x_ipc_dataMsgFree(dataMsg);
}

/*******************************************************/

void msgInfoMsgSend(int sd)
{
  int32 refId;
  MSG_PTR msg;
  
  msg = GET_MESSAGE(X_IPC_MSG_INFO_QUERY);
  
  refId = msg->msgData->refId;
  
  INT_TO_NET_INT(refId);
  (void)x_ipc_writeNBytes(sd, (char *)&refId, sizeof(int32));
  
  formatterSend(sd, msg->msgData->msgFormat);
  formatterSend(sd, msg->msgData->resFormat);
}

/*******************************************************/

static void registerClass(X_IPC_MSG_CLASS_TYPE className, char *format)
{
  CONST_FORMAT_PTR form;
  CLASS_FORM_PTR classForm;
  
  form = ParseFormatString(format);
  classForm = GET_CLASS_FORMAT(&className);
  
  if (classForm)
    classForm->format = form;
  else {
    classForm = NEW(CLASS_FORM_TYPE);
    classForm->className = className;
    classForm->format = form;
    
    ADD_CLASS_FORMAT(&className, classForm);
  }
}

/*******************************************************/

X_IPC_RETURN_VALUE_TYPE centralSendMessage(X_IPC_REF_PTR ref, MSG_PTR msg,
					 void *msgData, void *classData)
{
  int32 refId, savedRef;
  DATA_MSG_PTR msgDataMsg;
  X_IPC_MSG_CLASS_TYPE msg_class;
  
  CLASS_FORM_PTR classForm;
  
  CONST_FORMAT_PTR classFormat;
  
  classFormat = NULL;
  
  if (!msg) {
    X_IPC_ERROR("ERROR: x_ipc_sendMessage: NULL msg.\n");
  }
  
  /* save the message ref to use in case x_ipc_msgFind triggers 
     a recursive call to x_ipc_sendMessage */
  
  savedRef = x_ipc_nextSendMessageRef();
  
  msg_class = msg->msgData->msg_class;
  
  classForm = GET_CLASS_FORMAT(&msg_class);
  
  if (classForm)
    classFormat = classForm->format;
  
  if (ref) 
    refId = ref->refId;
  else
    refId = NO_REF;
  
  /* 8-Oct-90: fedor:
     parentRefGlobal problem!!! ** it is initialized to -1 in behaviors.c 
     so maybe this will not be a problem */
  
  msgDataMsg = x_ipc_dataMsgCreate(GET_C_GLOBAL(parentRefGlobal),
				   msg->msgData->refId, 
				   (int32)msg_class, refId, savedRef, 
				   msg->msgData->msgFormat,
				   msgData, classFormat, classData);
  
  if (msgDataMsg == NULL) return Failure;
  
  /* RTG - must set datastruct to be null so message does not get decoded 
   * into the same memory location.  We also have to copy the message
   * buffer if it is the same as the original data structure because
   * the original data structure can change, if the message is queued.
   */
  if (msgDataMsg->msgData == msgData) {
    /* Need to copy the data, in case the messages gets queued.  */
    char *newCopy = NULL;
    newCopy = (char *)x_ipcMalloc(msgDataMsg->msgTotal);
    BCOPY(msgDataMsg->msgData, newCopy,msgDataMsg->msgTotal);
    msgDataMsg->msgData = newCopy;
    msgDataMsg->dataStruct = NULL;
  } else 
    msgDataMsg->dataStruct = NULL;

  recvMessageBuild(GET_S_GLOBAL(x_ipcServerModGlobal), msgDataMsg);
  return Success;
}

/*******************************************************/

X_IPC_RETURN_VALUE_TYPE centralSendResponse(DISPATCH_PTR dispatch, MSG_PTR msg,
					  void *resData,
					  X_IPC_MSG_CLASS_TYPE resClass,
					  void *resClassData,
					  int sd)
{
#ifdef UNUSED_PRAGMA
#pragma unused(resClassData, sd)
#endif
  int32 intent;
  DATA_MSG_PTR dataMsg;
  
  if (msg) {
    intent = msg->msgData->refId;
  } else {
    intent = -1;
    if (dispatch) {
      if (!dispatch->msg) 
	dispatch->msg = x_ipc_msgFind(dispatch->msg->msgData->name);
      msg = dispatch->msg;
    }
  }
  
  dataMsg = x_ipc_dataMsgCreate(GET_C_GLOBAL(parentRefGlobal), intent, 
			  (int32)resClass, 0, 0,
			  dispatch->msg->msgData->resFormat, resData,
			  (FORMAT_PTR)NULL, (char *)NULL);
  
  if (dataMsg == NULL) return Failure;

  /* RTG - must set datastruct to be null so message does not get decoded 
   * into the same memory location.  We also have to copy the message
   * buffer if it is the same as the original data structure because
   * the original data structure can change, if the message is queued.
   */
  if (dataMsg->msgData == resData) {
    /* Need to copy the data, in case the messages gets queued.  */
    char *newCopy = NULL;
    newCopy = (char *)x_ipcMalloc(dataMsg->msgTotal);
    BCOPY(dataMsg->msgData, newCopy,dataMsg->msgTotal);
    dataMsg->msgData = newCopy;
    dataMsg->dataStruct = NULL;
  } else 
    dataMsg->dataStruct = NULL;
  
  recvMessageBuild(GET_S_GLOBAL(x_ipcServerModGlobal), dataMsg);
  
  x_ipc_dataMsgFree(dataMsg);
  
  return Success;
}

/*******************************************************/

void classInitialize(void)
{
  registerClass(GoalClass, "int");
  registerClass(CommandClass, "{int, int}");
  registerClass(ExceptionClass, "int");
  registerClass(PointMonitorClass, "int");
  registerClass(ExecHndClass, "string");
  registerClass(FailureClass, "string");
  
  if (sizeof(int32) != sizeof(X_IPC_POINT_CLASS_TYPE)) {
    X_IPC_ERROR("INTERNAL ERROR: X_IPC_POINT_CLASS_TYPE is not of size `int'\n");
  } else {
    
    centralRegisterNamedFormatter("X_IPC_POINT_CLASS_TYPE", "int");
    /* 19-Aug-93: fedor:
       see search_hash_table_for_format for error in printData.c */
    /* RTG - this causes problems with byte order between machines.
     * since these are just integers, use "int" for now.  The method
     * used to do printing should be fixed.  It should not have to scan
     * the hash table looking for structurally similar formats.
     */
    /* centralRegisterLengthFormatter("X_IPC_POINT_CLASS_TYPE", 
     *			   sizeof(int32));
     */
  }
  
  if (sizeof(int32) != sizeof(X_IPC_INTERVAL_CLASS_TYPE)) {
    X_IPC_ERROR("INTERNAL ERROR: X_IPC_INTERVAL_CLASS_TYPE is not of size `int'\n");
  } else {
    centralRegisterNamedFormatter("X_IPC_INTERVAL_CLASS_TYPE", "int");
    /* 19-Aug-93: fedor:
       see search_hash_table_for_format for error in printData.c */
    /* RTG - this causes problems with byte order between machines.
     * since these are just integers, use "int" for now.  The method
     * used to do printing should be fixed.  It should not have to scan
     * the hash table looking for structurally similar formats.
     */
    /* centralRegisterLengthFormatter("X_IPC_INTERVAL_CLASS_TYPE",
     *			   sizeof(int32));
     */
  }
  
   
  centralRegisterNamedFormatter("X_IPC_INTERVAL_TYPE",
				"{X_IPC_INTERVAL_CLASS_TYPE, int}");
  
  
  /*  centralRegisterNamedFormatter("X_IPC_INTERVAL_TYPE",*/
  /*				"{int, int}");*/
  
  centralRegisterNamedFormatter("X_IPC_TIME_POINT_TYPE",
  		"{X_IPC_POINT_CLASS_TYPE, X_IPC_INTERVAL_TYPE}");
  
  
  /*  centralRegisterNamedFormatter("X_IPC_TIME_POINT_TYPE",*/
  /*				"{int, X_IPC_INTERVAL_TYPE}");*/
  
  centralRegisterNamedFormatter("INTERVAL_MONITOR_OPTIONS_PTR",
				"*{int, int, int, int, int}");
  
  centralRegisterNamedFormatter("INT_MON_CLASS_TYPE", 
				"{X_IPC_TIME_POINT_TYPE, X_IPC_TIME_POINT_TYPE, INTERVAL_MONITOR_OPTIONS_PTR}");
  
  registerClass(PollingMonitorClass, "INT_MON_CLASS_TYPE");
  registerClass(DemonMonitorClass, "INT_MON_CLASS_TYPE");
  registerClass(FireDemonClass, "int");
  
  registerClass(MultiQueryClass, "{int, int}");
}

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: dispatch
 *
 * FILE: dispatch.c
 *
 * ABSTRACT:
 * 
 * A dispatch represents a single message and the message's progress.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: dispatch.c,v $
 * Revision 2.5  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2002/01/03 19:43:33  reids
 * Fixed central logging of the name of the module that sent a reply.
 *
 * Revision 2.3  2001/01/12 15:52:29  reids
 * Made logging more efficient for ignored messages
 *
 * Revision 2.2  2000/07/03 17:03:23  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.7  1997/02/26 04:17:36  reids
 * Once again refixed the logic behind data logging.
 *
 * Revision 1.2.2.6  1997/01/27 20:09:18  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.4  1997/01/21 17:20:25  reids
 * Re-re-fixed the logic of "x_ipc_LogIgnoreP" (needed an extra
 *   function: LogIgnoreAllP).
 *
 * Revision 1.2.2.3  1997/01/16 22:16:50  reids
 * Increased DISPATCH_FREE_INC to match DISPATCH_TABLE_SIZE
 *
 * Revision 1.2.2.2  1996/12/18 15:12:45  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.1  1996/10/18 18:01:49  reids
 * Better freeing of memory.
 *
 * Revision 1.2  1996/05/24 16:45:50  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:20  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/03 03:11:09  reids
 * The data logging (-ld) option now works, for both IPC and regular X_IPC msgs.
 * Fixed the interaction between signals and listening for messages.
 * IPC version will not exit if connection to central server is not available.
 * IPC version will not print out stats if module exits (x_ipcModError).
 *
 * Revision 1.2  1996/03/19 03:38:39  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.1  1996/03/03 04:31:15  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.42  1996/07/19  18:13:57  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.41  1996/07/18  02:08:39  rich
 * Last fix for logging replies was not quite right.
 *
 * Revision 1.40  1996/07/13  19:07:15  rich
 * Problem with getting data pointer for messages originating at central.
 *
 * Revision 1.39  1996/07/03  19:52:07  rich
 * Don't free data that originated in central.
 *
 * Revision 1.38  1996/06/25  20:50:29  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.37  1996/05/09  18:30:49  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.36  1996/03/19  02:29:13  reids
 * Plugged some more memory leaks; Added test code for enum formatters.
 * Added code to free formatter data structures.
 *
 * Revision 1.35  1996/02/10  16:49:48  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.34  1996/02/06  19:04:35  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.33  1996/01/27  21:53:17  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.32  1996/01/10  03:16:23  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.31  1995/12/17  20:21:24  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.30  1995/11/03  03:04:24  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.29  1995/10/29  18:26:38  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.28  1995/10/25  22:48:10  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.27  1995/07/25  20:08:53  rich
 * Changes for NeXT machine (3.0).
 *
 * Revision 1.26  1995/07/19  14:26:11  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.25  1995/07/12  04:54:43  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.24  1995/07/08  17:51:04  rich
 * Linux Changes.  Also added GNUmakefile.defs.
 *
 * Revision 1.23  1995/07/06  21:16:02  rich
 * Solaris and Linux changes.
 *
 * Revision 1.22  1995/06/14  03:21:34  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.21  1995/05/31  19:35:20  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.20  1995/04/19  14:28:03  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.19  1995/03/30  15:42:46  rich
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
 * Revision 1.18  1995/03/28  01:14:29  rich
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
 * Revision 1.17  1995/03/16  18:05:19  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.16.2.1  1995/03/14  03:57:33  rich
 * Added a data reference count to the dataMsg type.  It is used to decide
 * when to free the data associated with a message.  Messages can share
 * data buffers.
 * Fixed bug in the vector read routine (x_ipc_read2Buffers).  It would not
 * correctly update the buffer pointers if multiple reads were needed.
 *
 * Revision 1.16  1995/01/18  22:40:15  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.15  1994/10/25  17:07:32  reids
 * Changed the logging functions to accept variable number of arguments.
 * Fixed the way the "ignore" (i) logging option worked.
 * Fixed the way the "parent id" (p) logging option worked, when the parent
 *   was at top-level.
 *
 * Revision 1.14  1994/05/17  23:15:40  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.13  1994/04/28  16:15:52  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.12  1994/04/16  19:42:00  rich
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
 * Revision 1.11  1994/03/28  02:22:50  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.10  1994/03/27  22:50:23  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.9  1993/12/14  17:33:23  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.8  1993/11/21  20:17:36  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.7  1993/10/21  16:13:48  rich
 * Fixed compiler warnings.
 *
 * Revision 1.6  1993/08/30  23:13:48  fedor
 * Added SUN4 as well as sun4 compile flag.
 * Corrected Top level failure message name display with a define in dispatch.c
 *
 * Revision 1.5  1993/08/30  21:53:19  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.4  1993/08/27  07:14:33  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/20  23:06:50  fedor
 * Minor changes for merge. Mostly added htons and removed cfree calls.
 *
 * Revision 1.2  1993/05/26  23:17:14  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:19  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:33  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:32  fedor
 * Added Logging.
 *
 * 5-Jan-93 Domingo Gallardo, School of Computer Science, CMU
 * Added code to log the parent's ID of the processed dispatches
 *
 * 26-Jul-91 Reid Simmons, School of Computer Science, CMU
 * Added "refCount" field -- the problem is that a dispatch could be freed
 * when another module still has a refence to it.  For example, one module
 * can kill a task tree node that another wants to add temporal constraints to.
 * This can cause nasty things to happen.  As a result, free a dispatch (and
 * task tree node) only if the refCount is zero.
 *
 * 14-May-91 Christopher Fedor, School of Computer Science, CMU
 * Moved dispatch update and display code to here from main recv message loop.
 *
 * 14-Mar-91  Reid Simmons, School of Computer Science, CMU
 * Fixed constraint messages from printing out a blank line when they
 * completed.
 *
 * 16-Sep-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 *  6-Jul-89 Reid Simmons, School of Computer Science, CMU
 * Split out server (central) related code into "dispatch_server.c"
 *
 * 24-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Changed dispatch to cache selected Msg and Hnd.
 * Removed arguments from AllocDispatch.
 *
 *  5-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Changed to use IdTable.
 *
 * 20-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * created
 *
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"

/******************************************************************************
 *
 * FUNCTION: void dispatchIncrementFreeList()
 *
 * DESCRIPTION:
 * Add DISPATCH_FREE_INC number of dispatches to the free list.
 * Insert each newly created dispatch into the dispatchTable.
 *
 * INPUTS: none.
 *
 * OUTPUTS: none.
 *
 *****************************************************************************/

static void dispatchIncrementFreeList(void)
{
  int32 i;
  DISPATCH_PTR newPtr, current;	  /* JGraham, 8/19/97, changed variable
				     name, new, to newPtr for g++ */
  
  current = NULL;
  for(i=0;i<DISPATCH_FREE_INC;i++) {
    newPtr = NEW(DISPATCH_TYPE);
    
    newPtr->refId = NO_REF;
    newPtr->orgId = 0;
    newPtr->desId = 0;
    newPtr->pRef = NO_REF; /* 9-Jul-90: fedor: change to ROOT_NODE */
    newPtr->org = NULL;
    newPtr->des = NULL;
    
    newPtr->msg = NULL;
    newPtr->hnd = NULL;
    newPtr->resource = NULL;
    
    newPtr->msgData = NULL;
    newPtr->resData = NULL;
    
#ifndef NMP_IPC
    newPtr->treeNode = NULL;
#endif
    newPtr->blockCom = NULL;
    newPtr->status = UnallocatedDispatch;
    
    newPtr->classData = NULL;
    newPtr->msg_class = UNKNOWN;
    
    newPtr->respProc = NULL;
    newPtr->respData = NULL;
    
    newPtr->tapInfo = NULL;
    
    newPtr->refCount = 0;
    
    newPtr->locId = x_ipc_idTableInsert((char *)newPtr, GET_S_GLOBAL(dispatchTable));
    
    newPtr->next = current;
    current = newPtr;
  }
  GET_S_GLOBAL(dispatchFreeListGlobal) = current;
}


/******************************************************************************
 *
 * FUNCTION: DISPATCH_PTR dispatchAllocate()
 *
 * DESCRIPTION:
 * Return a newly initialized dispatch from the free list.
 *
 * INPUTS: none.
 *
 * OUTPUTS: DISPATCH_PTR
 *
 * NOTES: 
 * 5-13-93: reids: 
 * Function now automatically sets dispatch status to "allocated"
 *
 *****************************************************************************/

DISPATCH_PTR dispatchAllocate(void)
{
  DISPATCH_PTR tmp;
  
  if (!GET_S_GLOBAL(dispatchFreeListGlobal))
    dispatchIncrementFreeList();
  
  tmp = GET_S_GLOBAL(dispatchFreeListGlobal);
  
  GET_S_GLOBAL(dispatchFreeListGlobal) =
    GET_S_GLOBAL(dispatchFreeListGlobal)->next;
  
  tmp->next = NULL;
  
  DISPATCH_SET_STATUS(AllocatedDispatch, tmp);
  
  return tmp;
}


/******************************************************************************
 *
 * FUNCTION: void dispatchFree(dispatch)
 *
 * DESCRIPTION:
 * Initialize the dispatch and return it to the free list.
 * Call x_ipc_dataMsgFree on the msgData and resData slots.
 *
 * INPUTS: DISPATCH_PTR dispatch
 *
 * OUTPUTS: none.
 *
 * NOTES: 
 * 5-13-93: reids: the check that dispatch had not already been freed ("next"
 * field is NULL) is not correct: If the dispatchFreeListGlobal is NULL, then
 * a freed dispatch will have a NULL next pointer.  The "correct" check is to
 * see if it is unallocated.
 *
 * 17-Oct-91: fedor: added check that dispatch has not already been freed.
 * The next field should be NULL. If it gets freed more than once the 
 * dispatchFreeListGlobal will become a circular list of one dispatch.
 *
 * 26-Jul-91: Reid: Do not free if refCount > 0.
 *
 *****************************************************************************/

void dispatchFree(DISPATCH_PTR dispatch)
{
  if (dispatch->refCount <= 0 && 
      dispatch->status != UnallocatedDispatch) { /* 5-13-93 reids: Changed */
    dispatch->refId = NO_REF;
    dispatch->orgId = 0;
    dispatch->desId = 0;
    dispatch->pRef = NO_REF;	/* 9-Jul-90: fedor: change to ROOT_NODE */
    dispatch->org = NULL;
    dispatch->des = NULL;
    
    dispatch->msg = NULL;
    dispatch->hnd = NULL;
    
    dispatch->resource = NULL;
    
#ifndef NMP_IPC
    dispatch->treeNode = NULL;
#endif
    dispatch->blockCom = NULL;
    dispatch->status = UnallocatedDispatch;
    
    x_ipc_dataMsgFree(dispatch->msgData);
    x_ipc_dataMsgFree(dispatch->resData);
    
    dispatch->msgData = NULL;
    dispatch->resData = NULL;
    
    /* Command class data is stored in "blockCom"; It has already been freed */
    if (dispatch->msg_class != CommandClass) 
      x_ipc_classDataFree(dispatch->msg_class, dispatch->classData);
    dispatch->classData = NULL;
    dispatch->msg_class = UNKNOWN;
    
    dispatch->respProc = NULL;
    dispatch->respData = NULL;
    
    freeTapInfo(&(dispatch->tapInfo));
    dispatch->tapInfo = NULL;
    
    dispatch->refCount = 0;
    
    dispatch->next = GET_S_GLOBAL(dispatchFreeListGlobal);
    GET_S_GLOBAL(dispatchFreeListGlobal) = dispatch;
  }
}


/******************************************************************************
 *
 * FUNCTION: void freeDispatchList(DISPATCH_PTR dispatch)
 *
 * DESCRIPTION:
 * Really free a dispatch list.
 *
 * INPUTS: DISPATCH_PTR dispatch
 *
 * OUTPUTS: none.
 *
 * NOTES: 
 *
 *****************************************************************************/

void freeDispatchList(DISPATCH_PTR dispatch)
{
  if (dispatch == NULL) return;
  freeDispatchList(dispatch->next);
  x_ipcFree((char *)(dispatch));
}


/******************************************************************************
 *
 * FUNCTION: void dispatchSetMsgData(msgData, dispatch)
 *
 * DESCRIPTION:
 * Set the dispatch msgData to the new msgData calling x_ipc_dataMsgFree on 
 * any existing msgData. Increment the new msgData reference count.
 *
 * INPUTS: 
 * DATA_MSG_PTR msgData;
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: none.
 *
 *****************************************************************************/

void dispatchSetMsgData(DATA_MSG_PTR msgData, DISPATCH_PTR dispatch)
{
  x_ipc_dataMsgFree(dispatch->msgData);
  dispatch->msgData = msgData;
  if (msgData)
    (msgData->refCount)++;
}


/******************************************************************************
 *
 * FUNCTION: void dispatchSetResData(resData, dispatch)
 *
 * DESCRIPTION:
 * Set the dispatch resData to the new resData calling x_ipc_dataMsgFree on 
 * any existing resData. Increment the new resData reference count.
 *
 * INPUTS: 
 * DATA_MSG_PTR resData;
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: none.
 *
 *****************************************************************************/

void dispatchSetResData(DATA_MSG_PTR resData, DISPATCH_PTR dispatch)
{
  x_ipc_dataMsgFree(dispatch->resData);
  dispatch->resData = resData;
  if (resData)
    (resData->refCount)++;
}


/******************************************************************************
 *
 * FUNCTION: DISPATCH_PTR dispatchCopy(dispatch)
 *
 * DESCRIPTION:
 * Create a copy of the dispatch, including all its slots (except for locId)
 *
 * INPUTS: DISPATCH_PTR dispatch
 *
 * OUTPUTS: DISPATCH_PTR: Returns the duplicated dispatch.
 *
 * NOTES: 
 *****************************************************************************/

DISPATCH_PTR dispatchCopy(DISPATCH_PTR dispatch)
{
  DISPATCH_PTR newDispatch;
  int32 locId;
  
  newDispatch = dispatchAllocate();
  locId = newDispatch->locId;
  
  BCOPY((char *)dispatch, (char *)newDispatch, sizeof(DISPATCH_TYPE));
  
  DISPATCH_MSG_DATA(newDispatch) = NULL;
  dispatchSetMsgData(DISPATCH_MSG_DATA(dispatch), newDispatch);
  newDispatch->locId = locId;
  
  return newDispatch;
}


/******************************************************************************
 *
 * FUNCTION: char *dispatchStatusName(status)
 *
 * DESCRIPTION: Return a printable string version of the dispatch status.
 *
 * INPUTS: DISPATCH_STATUS_TYPE status;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

static char *dispatchStatusName(DISPATCH_STATUS_TYPE status)
{
  switch(status) {
  case UnallocatedDispatch:
    return("Unallocated");
  case AllocatedDispatch:
    return("Allocated");
  case InactiveDispatch:
    return("Inactive");
  case PendingDispatch:
    return("Pending");
  case AttendingDispatch:
    return("Attending");
  case HandledDispatch:
    return("Handled");
  case KilledDispatch:
    return("Killed");
  case ReplyDispatch:
    return("Reply");
  case SuccessDispatch:
    return("Success");
  case FailureDispatch:
    return("Failure");
  case CentralPendingDispatch:
    return("Central Pending");
#ifndef TEST_CASE_COVERAGE
  default:
    return("UNKNOWN");
#endif
  }
}


/******************************************************************************
 *
 * FUNCTION: void dispatchUpdateAndDisplay1(newStatus, dispatch)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static int32 monitorClass(X_IPC_MSG_CLASS_TYPE msg_class)
{
  return(msg_class == PointMonitorClass || msg_class == PollingMonitorClass ||
	 msg_class == DemonMonitorClass);
}

static void dispatchUpdateAndDisplay1(DISPATCH_STATUS_TYPE newStatus,
				      DISPATCH_PTR dispatch)
{
  int32 error = FALSE, printed = TRUE;
  int32 parentId;
  DISPATCH_PTR parent_dispatch;
  X_IPC_MSG_CLASS_TYPE msg_class, mclass;
  
#ifndef NMP_IPC
  parent_dispatch = findParentDispatch(dispatch);
#else
  parent_dispatch = NULL;
#endif
  parentId = (parent_dispatch ? parent_dispatch->locId : dispatch->pRef);
  
  msg_class = dispatch->msg_class;
  
  /* 30Aug93: fedor: Replaced msgName = dispatch->msg->msgData->name
     with DISPATCH_MSG_NAME because the message name may not
     always be defined for a dispatch. See HandledDispatch - FailureClass */
  
  switch(dispatch->status) {
  case AllocatedDispatch:
    switch(newStatus) {
    case AttendingDispatch:
      LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		  DISPATCH_MSG_NAME(dispatch));
      Log_RefId(dispatch, LOGGING_MESSAGE);
      LOG_MESSAGE1(": %10s", dispatch->org->modData->modName);
      Log_ParentId(parentId, LOGGING_MESSAGE);
      LOG_MESSAGE1(" --> %-15s (Sent)", dispatch->des->modData->modName);
      break;
    case PendingDispatch:
      LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		  DISPATCH_MSG_NAME(dispatch));
      Log_RefId(dispatch, LOGGING_MESSAGE);
      LOG_MESSAGE1(": %10s", dispatch->org->modData->modName);
      Log_ParentId(parentId, LOGGING_MESSAGE);
      LOG_MESSAGE1(" --> Resource %-6s (Pending)", 
		  dispatch->hnd->resource->name); 
      break;
    case InactiveDispatch:
      LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		  DISPATCH_MSG_NAME(dispatch));
      Log_RefId(dispatch, LOGGING_MESSAGE);
      LOG_MESSAGE1(": %10s", dispatch->org->modData->modName);
      Log_ParentId(parentId, LOGGING_MESSAGE);
      LOG_MESSAGE(" --> ON HOLD         (Inactive)");
      break;
    case HandledDispatch:
      if (msg_class == FailureClass) {
	LOG_MESSAGE2("Failure  %15s: %s",
		    "*Top Level*", *(char **)dispatch->classData);
      }
      break;
    default:
      error = TRUE;
      break;
    }
    break;
  case PendingDispatch:
    switch(newStatus) {
    case AttendingDispatch:
      LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class),
		  DISPATCH_MSG_NAME(dispatch));
      Log_RefId(dispatch, LOGGING_MESSAGE);
      LOG_MESSAGE2(": Resource %s --> %-15s (Sent)",
		  dispatch->hnd->resource->name, 
		  dispatch->des->modData->modName);
      break;
    default:
      error = TRUE;
      break;
    }
    break;
  case CentralPendingDispatch:
    switch(newStatus) {
    case AttendingDispatch:
      LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		  DISPATCH_MSG_NAME(dispatch));
      Log_RefId(dispatch, LOGGING_MESSAGE);
      LOG_MESSAGE1(": %10s", dispatch->org->modData->modName);
      Log_ParentId(parentId, LOGGING_MESSAGE);
      LOG_MESSAGE1(" --> %-15s (Sent)", dispatch->des->modData->modName);
      break;
    default:
      error = TRUE;
      break;
    }
    break;
  case AttendingDispatch:
    switch(newStatus) {
    case HandledDispatch:
      if (TWO_WAY_MSG(dispatch->msg)) {
	LOG_MESSAGE1("  Reply  %15s", DISPATCH_MSG_NAME(dispatch));
	Log_RefId(dispatch, LOGGING_MESSAGE);
	LOG_MESSAGE2(": %10s --> %-15s", dispatch->des->modData->modName,
		    dispatch->org->modData->modName);
	Log_ParentId(parentId, LOGGING_MESSAGE);
	if (!dispatch->org->alive) {
	  LOG_MESSAGE1("\nWARNING: Reply Ignored for dead module: %s",
		      dispatch->org->modData->modName);
	}
      } else if (monitorClass(msg_class)) {
	LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		    DISPATCH_MSG_NAME(dispatch));
	Log_RefId(dispatch, LOGGING_MESSAGE);
	LOG_MESSAGE(":                              (Completed)");
      } else if (msg_class == SuccessClass ||
		 (msg_class == InformClass && 
		  dispatch->des == GET_S_GLOBAL(x_ipcServerModGlobal))) {
	mclass = dispatch->msg->msgData->msg_class;
	if (mclass == InformClass || mclass == BroadcastClass) {
 	  LOG_MESSAGE("  Done    ");
 	} else {
	  LOG_MESSAGE("  Success ");
	}
	LOG_MESSAGE1("%15s", DISPATCH_MSG_NAME(dispatch));
	Log_RefId(dispatch, LOGGING_MESSAGE);
	LOG_MESSAGE(":");
      } else if (msg_class == FailureClass) {
	LOG_MESSAGE1("  Failure %15s", DISPATCH_MSG_NAME(dispatch));
	Log_RefId(dispatch, LOGGING_MESSAGE);
	LOG_MESSAGE1(": %s", *(char **)dispatch->classData);
      } else if (msg_class == ReplyClass) {
	LOG_MESSAGE1("  Reply  %15s", DISPATCH_MSG_NAME(dispatch));
	Log_RefId(dispatch, LOGGING_MESSAGE);
#ifdef NMP_IPC
	LOG_MESSAGE2(": %10s --> %-15s", 
		     dispatch->des->modData->modName,
		     dispatch->org->modData->modName);
#else
	LOG_MESSAGE2(": %10s --> %-15s", 
		     GET_S_GLOBAL(x_ipcServerModGlobal)->modData->modName,
		     dispatch->org->modData->modName);
#endif
	Log_ParentId(parentId, LOGGING_MESSAGE);
      } else {
	printed = FALSE;
      }
      break;
    case CentralPendingDispatch:
      LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		  DISPATCH_MSG_NAME(dispatch));
      Log_RefId(dispatch, LOGGING_MESSAGE);
      LOG_MESSAGE1(": %10s", dispatch->org->modData->modName);
      Log_ParentId(parentId, LOGGING_MESSAGE);
      LOG_MESSAGE1(" --> %-15s (Pending)", dispatch->des->modData->modName);
      break;
    default:
      error = TRUE;
      break;
    }
    break;
  case InactiveDispatch:
    LOG_MESSAGE2("%-9s %15s", x_ipc_messageClassName(msg_class), 
		DISPATCH_MSG_NAME(dispatch));
    Log_RefId(dispatch, LOGGING_MESSAGE);
    LOG_MESSAGE(":  ON HOLD ");
    switch(newStatus) {
    case AttendingDispatch:
      LOG_MESSAGE1(" --> %-15s (Sent)", dispatch->des->modData->modName);
      break;
    case PendingDispatch:
      LOG_MESSAGE1(" --> Resource %-6s (Pending)", 
		  dispatch->hnd->resource->name); 
      break;
    default:
      error = TRUE;
      break;
    }
    break;
  default:
    error = TRUE;
    break;
  }
  
  if (error) {
    LOG_MESSAGE1("\nWARNING: dispatchUpdateAndDisplay: %s", 
		DISPATCH_MSG_NAME(dispatch));
    Log_RefId(dispatch, LOGGING_MESSAGE);
    LOG_MESSAGE2(": %s --> %s", dispatchStatusName(dispatch->status),
		dispatchStatusName(newStatus));
  }
  
  if (printed) {
    Log_Time(1); 
    LOG_MESSAGE("\n");
  }
}


#ifdef NMP_IPC
static void log_IPC_Data (MSG_PTR msg, const char *Data, DATA_MSG_PTR msgData,
			  BOOLEAN freeStruct)
{
  const char *Data1;
  DATA_MSG_TYPE tmpMsgData = *msgData;
  CONST_FORMAT_PTR Format = msg->msgData->msgFormat;
  CONST_FORMAT_PTR Format1 = msg->msgData->resFormat;

  if (Format->type == FixedArrayFMT) {
    tmpMsgData.msgData = (char *)Data;
  } else {
    tmpMsgData.msgTotal = ((INT_STR_KEY_PTR)Data)->num;
    tmpMsgData.msgData = (char *)((INT_STR_KEY_PTR)Data)->str;
  }
  Data1 = (const char *)x_ipc_dataMsgDecodeMsg(Format1, &tmpMsgData, TRUE);
  Log_Data(Format1, Data1, 4);
  if (freeStruct && (Data != msgData->msgData)) {
    x_ipc_freeDataStructure(Format, (void *)Data);
  }
  Data = NULL;
  if (Data1 != tmpMsgData.msgData) {
    x_ipc_freeDataStructure(Format1, (void *)Data1);
  }
}
#endif

static const char *dispatchDecodeData (CONST_FORMAT_PTR Format, 
				       DATA_MSG_PTR msgData,
				       BOOLEAN *freeStruct)
{
  if (msgData->dataStruct == NULL) {
    *freeStruct = TRUE;
    return (const char *)x_ipc_dataMsgDecodeMsg(Format, msgData, TRUE);
  } else {
    return msgData->dataStruct;
  }
}

/******************************************************************************
 *
 * FUNCTION: void dispatchUpdateAndDisplay(newStatus, dispatch)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

void dispatchUpdateAndDisplay(DISPATCH_STATUS_TYPE newStatus,
			      DISPATCH_PTR dispatch)
{
  const char *Data;
  CONST_FORMAT_PTR Format;
  BOOLEAN ignored;
  BOOLEAN freeStruct = FALSE, keepStruct = FALSE;
  
  if (x_ipc_LogMessagesP()) {
    ignored = x_ipc_LogIgnoreP() && Ignore_Logging_Message(dispatch->msg);
    if (!ignored || !LogIgnoreAllP()) {
      if (ignored) Start_Ignore_Logging();
    
      dispatchUpdateAndDisplay1(newStatus, dispatch);
    
      if ((newStatus == AttendingDispatch || dispatch->msg_class == ReplyClass)
	  && x_ipc_LogDataP()) {
      
	if (dispatch->msg) {
	  Data = NULL;
	  if (newStatus == HandledDispatch) {
	    MSG_PTR replyMsg;
#ifdef NMP_IPC
	    if (ONE_WAY_MSG(dispatch->msg) && dispatch->resData->classData) {
	      replyMsg = getResponseMsg(dispatch->resData);
	      Format = replyMsg->msgData->msgFormat;
	    } else
#endif
	      {
		replyMsg = dispatch->msg;
		Format = dispatch->msg->msgData->resFormat;
	      }
	    parseMsgFormats(replyMsg); /* In case not yet parsed */
	    if (Format != NULL) {
	      if ( (dispatch->resData) && (dispatch->resData->dataStruct)) {
		/* This is a central message, since the data is already here.  */
		freeStruct = FALSE;
		keepStruct = TRUE;
		Data = dispatch->resData->dataStruct;
	      } else {
		Data = dispatchDecodeData(Format, dispatch->resData, 
					  &freeStruct);
	      }
#ifdef NMP_IPC
	      if (Data && ONE_WAY_MSG(replyMsg) &&
		  replyMsg->msgData->resFormat) {
		log_IPC_Data(replyMsg, Data, dispatch->resData, freeStruct);
	      } else
#endif
		{ 
		  Log_Data(Format, Data, 4);
		  if (Data && freeStruct && 
		      (Data != dispatch->resData->msgData)) {
		    /* free decoded data, including all its subelements */
		    x_ipc_freeDataStructure(Format, (void *)Data);
		    Data = NULL;
		  } else if ((dispatch->resData) && !keepStruct) {
		    dispatch->resData->dataStruct = NULL;
		  }
		}
	    }
	  } else {
	    Format = dispatch->msg->msgData->msgFormat;
	    if (Format != NULL) {
	      Data = dispatchDecodeData(Format, dispatch->msgData, &freeStruct);
#ifdef NMP_IPC
	      if (Data && ONE_WAY_MSG(dispatch->msg) &&
		  dispatch->msg->msgData->resFormat) {
		log_IPC_Data(dispatch->msg, Data, dispatch->msgData, freeStruct);
	      } else
#endif
		{ 
		  Log_Data(Format, Data, 4);
		  if (Data && freeStruct && (Data != dispatch->msgData->msgData)){
		    /* free decoded data, including all its subelements */
		    x_ipc_freeDataStructure(Format, (void *)Data);
		  } else if (dispatch->msgData) {
		    dispatch->msgData->dataStruct = NULL;
		  }
		  Data = NULL;
		}
	    }
	  }
	}
      }
      if (ignored) End_Ignore_Logging();
    }
  }
  DISPATCH_SET_STATUS(newStatus, dispatch);
}



/******************************************************************************
 *
 * FUNCTION: void deliverDispatch(dispatch)
 *
 * DESCRIPTION: 
 * Delivers a dispatch to the correct module to select a handler.
 * Triggers a x_ipc_msgFind on the module side if the format has changed for this
 * handler request. This allows multiple formats (multiple messages with 
 * different formats) for a single handler.  The handler code can simply cast 
 * based on a tag value - or request the message name as a tag from the ref.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void.
 *
 * NOTES:
 * 
 *
 *****************************************************************************/

void deliverDispatch(DISPATCH_PTR dispatch)
{
  char *data;
  DATA_MSG_PTR newDataMsg;
  X_IPC_MSG_CLASS_TYPE msg_class;
  CLASS_FORM_PTR classForm;
  
  data = NULL;
  
  dispatchUpdateAndDisplay(AttendingDispatch, dispatch);
  
  if (dispatch->des == GET_S_GLOBAL(x_ipcServerModGlobal)) {
    if (monitorClass(dispatch->msg->msgData->msg_class)) {
      data = dispatch->classData;
    } else {
      data = (char *)x_ipc_dataMsgDecodeMsg(dispatch->msg->msgData->msgFormat, 
				      DISPATCH_MSG_DATA(dispatch),TRUE);
    }
    (*dispatch->hnd->hndProc)(dispatch, data);
    if (dispatch->msg_class == InformClass) {
      dispatchUpdateAndDisplay(HandledDispatch, dispatch);
    }
  } else {
    if (dispatch->hnd->msg != dispatch->msg) {
      /* handler to recieve new message */
      msg_class = ExecHndClass;
      classForm = GET_CLASS_FORMAT(&msg_class);
      if (!classForm)
	X_IPC_ERROR("ERROR: deliverDispatch: missing ExecHndClass class format.");
      
      newDataMsg = 
	x_ipc_dataMsgReplaceClassData(classForm->format,
				(char*)&dispatch->msg->msgData->name,
				dispatch->msgData,
				dispatch->msg->msgData->msgFormat
				);
      
      /* 31-Oct-90: fedor: the x_ipc_dataMsgReplaceClassData routine calls
	 x_ipc_dataMsgFree - this avoids calling it twice and gets the 
	 refs correct. blah! */
      dispatch->msgData = NULL;
      
      dispatchSetMsgData(newDataMsg, dispatch);
    }
    else {
      /* The datamags may already have old class data.
	 Since class data follows message data, this
	 will prevent old class data from being sent. */
      dispatch->msgData->classTotal = 0;
    }
    
    dispatch->msgData->intent = dispatch->hnd->hndData->refId;
    dispatch->msgData->msgRef = dispatch->locId;
    
    (void)x_ipc_dataMsgSend(dispatch->desId, dispatch->msgData);
  }
}


/******************************************************************************
 *
 * FUNCTION: void deliverResponse(dispatch)
 *
 * DESCRIPTION: 
 * Delivers a response dispatch to an internal procedure or to a query reply.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void deliverResponse(DISPATCH_PTR dispatch)
{
  if (dispatch->respProc) {
    (*dispatch->respProc)(dispatch, dispatch->respData);
    return;
  }
  
  if (!resourceSelfQuery(dispatch))
    pendingReplyRemove(dispatch);
  
  if (dispatch->org->alive) {
    dispatch->resData->intent = QUERY_REPLY_INTENT;
    dispatch->resData->msgRef = dispatch->refId;
    
    (void)x_ipc_dataMsgSend(dispatch->org->writeSd, DISPATCH_RES_DATA(dispatch));
  } else if (!dispatch->org->repliesPending) {
    moduleFree(dispatch->org);
  }
}


/******************************************************************************
 *
 * FUNCTION: void centralReply(dispatch, data)
 *
 * DESCRIPTION: allows central to reply like a module.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * void *data;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE centralReply(DISPATCH_PTR dispatch, const void *data)
{
  CONST_FORMAT_PTR format=NULL;

  if (data != NULL) 
    format = dispatch->msg->msgData->resFormat;

  dispatch->resData = x_ipc_dataMsgCreate(0, 0, 0, 0, 0,
				    format, data,
				    (FORMAT_PTR)NULL, (char *)NULL);
  
  if (dispatch->resData == NULL) return Failure;
  /* 20-Nov-90: fedor: to be consistent with recvMesgBuild */
  dispatch->msg_class = ReplyClass;
  recvMessage(dispatch, ReplyClass, dispatch->classData);

  return Success;
}

/******************************************************************************
 *
 * FUNCTION: void centralNullReply(dispatch)
 *
 * DESCRIPTION: allows central to reply like a module.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE centralNullReply(DISPATCH_PTR dispatch)
{
  return centralReply(dispatch, (void *) NULL);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE centralInform(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE centralInform(const char *name, const void *data)
{ 
  MSG_PTR msg;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, InformClass);
  
  return centralSendMessage((X_IPC_REF_PTR)NULL, msg,
			    (char *)data, (char *)NULL);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE centralBroadcast(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE centralBroadcast(const char *name, const void *data)
{ 
  MSG_PTR msg;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, BroadcastClass);
  
  if ((GET_C_GLOBAL(broadcastMsgs) == NULL) ||
      (x_ipc_strListMemberItem(name, GET_C_GLOBAL(broadcastMsgs))))
    return centralSendMessage((X_IPC_REF_PTR)NULL, msg,
			      (char *)data, (char *)NULL);
  else
    return Success;
}


/******************************************************************************
 *
 * FUNCTION: void centralSuccess(dispatch)
 *
 * DESCRIPTION: allows central indicate a command or inform was successful.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void centralSuccess(DISPATCH_PTR dispatch)
{
  dispatch->msg_class = SuccessClass;
  recvMessage(dispatch, SuccessClass, dispatch->classData);
}


/******************************************************************************
 *
 * FUNCTION: void centralFailure(dispatch)
 *
 * DESCRIPTION: allows central indicate a command or inform failed.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void centralFailure(DISPATCH_PTR dispatch)
{
  dispatch->msg_class = FailureClass;
  recvMessage(dispatch, FailureClass, dispatch->classData);
}


/******************************************************************************
 *
 * FUNCTION: void *dispatchDecodeResponse(dispatch)
 *
 * DESCRIPTION: Make use of the msg resFormat to decode the res datamsg.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

void *dispatchDecodeResponse(DISPATCH_PTR dispatch)
{
  return(x_ipc_dataMsgDecodeMsg(dispatch->msg->msgData->resFormat,
			  DISPATCH_RES_DATA(dispatch),TRUE));
}

/******************************************************************************
 *
 * FUNCTION: void reserveDispatch(dispatch)
 *           void releaseDispatch(dispatch)
 *
 * DESCRIPTION: Some module has an outstanding reference to this dispatch.
 *              "reserveDispatch" records that fact, "releaseDispatch" removes
 *              that assertion.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: none
 *
 * NOTES: With "releaseDispatch", need to check if the associated task tree
 *        node is waiting to be killed.
 *
 *****************************************************************************/

void reserveDispatch(DISPATCH_PTR dispatch)
{
  if (dispatch) {
    dispatch->refCount++;
  }
}

void releaseDispatch(DISPATCH_PTR dispatch)
{
  if (dispatch) {
    
    if (dispatch->refCount == 0) {
      LOG2("\nWARNING: Releasing an unreserved dispatch %s (%d)\n",
	  dispatch->msg->msgData->name, dispatch->locId);
    } else {
      dispatch->refCount--;
    }
    
    if (dispatch->refCount <= 0) {
#ifndef NMP_IPC
      HandleKillAfterAttendingNodes(dispatch);
      if ((dispatch->status == HandledDispatch) && !dispatch->treeNode) {
	dispatchFree(dispatch);
#else
      if (dispatch->status == HandledDispatch) {
	dispatchFree(dispatch);
#endif
      }
    }
  }
}

/******************************************************************************
 *
 * FUNCTION: void dispatchStats
 *
 * DESCRIPTION: Print stats about the utilization of dispatches in central
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

#if 0
/* No longer used */
void dispatchStats ()
{
  int32 i, num=0;
  DISPATCH_PTR dispatch;
  
  for (i=0; i<GET_S_GLOBAL(dispatchTable)->currentSize; i++) {
    dispatch = (DISPATCH_PTR)idTableItem(i, GET_S_GLOBAL(dispatchTable));
    if (dispatch->status != UnallocatedDispatch) {
      printf("%d. %s\n", ++num, dispatch->msg->msgData->name);
    }
  }
}
#endif

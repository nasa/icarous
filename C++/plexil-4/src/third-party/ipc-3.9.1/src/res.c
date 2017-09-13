/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: resources
 *
 * FILE: res.c
 *
 * ABSTRACT:
 * 
 * Resources
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/res.c,v $ 
 * $Revision: 2.6 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: res.c,v $
 * Revision 2.6  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.5  2005/12/30 17:01:44  reids
 * Support for Mac OSX
 *
 * Revision 2.4  2000/12/11 16:09:56  reids
 * Renamed "modNameEq" to avoid confusion with another function of same name
 *   in comServer.c
 *
 * Revision 2.3  2000/07/03 17:03:28  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/01/27 20:51:49  reids
 * Changes for RedHat 6 (and also to remove compiler warnings).
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.4  1996/12/18 15:11:35  reids
 * Changed logging code to remove VxWorks dependence on varargs
 * Don't add resource to resourceList if just changing its capacity
 *
 * Revision 1.2.2.3  1996/10/22 18:33:04  reids
 * Change a "Log" to "Log_Status"
 *
 * Revision 1.2.2.2  1996/10/18 18:17:08  reids
 * Better freeing of memory.
 * Transfer any pending messages to the new resource under "addHndToResource"
 *
 * Revision 1.2.2.1  1996/10/14 03:54:47  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.2  1996/05/24 16:46:02  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:55  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:19:59  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:28  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.41  1996/07/25  22:24:34  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.40  1996/07/24  13:46:08  reids
 * Support for Windows95 (Thanks to Tam Ngo, JSC)
 * When module goes down, do not remove dispatch if still have task tree node.
 * Handle NULL format in x_ipc_dataStructureSize.
 * Add short, byte and ubyte formats for Lisp.
 * Ignore stdin when checking sockets for input.
 *
 * Revision 1.39  1996/07/19  18:14:28  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.38  1996/03/02  03:21:55  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.37  1996/02/12  00:54:01  rich
 * Get VX works compile to work with GNUmakefiles.
 *
 * Revision 1.36  1996/02/06  19:05:07  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.35  1996/01/30  15:04:53  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.34  1996/01/27  21:54:14  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.33  1996/01/23  00:06:36  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.32  1996/01/10  03:16:26  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.31  1995/12/17  20:22:05  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.30  1995/11/03  03:04:42  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.29  1995/10/29  18:27:03  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.28  1995/10/25  22:48:48  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.27  1995/10/18  19:28:17  reids
 * resourceProcessPendingRes now checks for available resources before sending
 *   off pending messages.  Had been a problem under certain race conditions.
 *   Also cleaned up a few of the functions that return values.
 *
 * Revision 1.26  1995/10/10  00:43:06  rich
 * Added more system messages to ignore.
 *
 * Revision 1.25  1995/10/07  19:07:44  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.24  1995/07/19  14:26:28  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.23  1995/07/12  04:55:28  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.22  1995/06/14  03:22:16  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.21  1995/05/31  19:36:33  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.20  1995/04/21  03:53:25  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.19  1995/03/30  15:44:01  rich
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
 * Revision 1.18  1995/03/28  01:14:57  rich
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
 * Revision 1.17  1995/01/18  22:42:30  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.16  1994/10/25  17:06:19  reids
 * Changed the logging functions to accept variable number of arguments.
 * Fixed the way the "ignore" logging option worked.
 *
 * Revision 1.15  1994/05/31  13:40:38  reids
 * Deadlock and Fairness: fixed a bug that would cause deadlock if a module
 * tried to lock/reserve itself from within a message handler; fixed another
 * bug that could cause messages to be delayed indefinitely if "limit pending"
 * was used.
 *
 * Revision 1.14  1994/05/25  17:32:33  reids
 * Added utilities to limit the number of pending messages
 *
 * Revision 1.13  1994/05/25  04:58:00  rich
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
 * Revision 1.12  1994/05/24  13:48:46  reids
 * Fixed so that messages are not sent until a x_ipcWaitUntilReady is received
 * (and the expected number of modules have all connected)
 *
 * Revision 1.11  1994/05/17  23:17:24  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.10  1994/04/28  16:17:09  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.9  1994/04/16  19:43:06  rich
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
 * Revision 1.8  1993/12/14  17:35:06  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.7  1993/11/21  20:19:22  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.6  1993/10/21  16:14:22  rich
 * Fixed compiler warnings.
 *
 * Revision 1.5  1993/08/30  21:54:23  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.4  1993/08/27  08:38:55  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.3  1993/08/27  07:16:40  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/08/23  17:40:18  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.3  1993/06/22  14:00:23  rich
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
 * Revision 1.2  1993/05/27  22:20:15  rich
 * Added automatic logging.
 *
 * 13-Sep-92, Reid Simmons, School of Computer Science, CMU
 * When deleting a resource (when a module goes down), clear the handlers
 * of the dispatches that are already active or queued.
 *
 * 28-Oct-91 Christopher Fedor, School of Computer Science, CMU
 * Changed logic of tryResendingMessage to key on 'y' instead of != 'n'
 * Fixed tryResendingMessage to try all active/pending resends. 
 * Added readChar routine to improve interaction with getchar and user input.
 *
 * 13-Sep-91 Christopher Fedor, School of Computer Science, CMU
 * Changed resourcePending to add new items to the last in the list for FIFO.
 *
 * 25-Jul-91, Reid Simmons, School of Computer Science, CMU
 * Built on Chris' earlier work for resending messages after a module crash.
 *
 *  3-Jul-91, Reid Simmons, School of Computer Science, CMU
 * Freed data used the centrally registered handlers. 
 *
 *  3-Dec-90 Christopher Fedor, School of Computer Science, CMU
 * Moved self query test from recvMsg.c to resourceAvailable so that a 
 * locked resource would not handle a self query style message.
 *
 *  3-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * Added resourceRemovePending routine to provide a resource interface
 * to taskTree.c for kiling of taskTree nodes.
 *
 *  3-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * Removed repetitve code by creating resourceReserveLock and
 * resourceModReserveLock. Changed resourceCreate to update capacity
 * on existing resources and to force capacity to a minimum of 1.
 *
 *  1-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * Finished reservation and lock - lots of repetitve code to stamp out still.
 *
 * 25-Sep-90 Christopher Fedor, School of Computer Science, CMU
 * Added x_ipc 5.0 versions of reservation and lock.
 *
 * 24-Sep-90 Christopher Fedor, School of Computer Science, CMU
 * Rewritten to support a list of equal name resources to implement
 * locking and reservation.
 *
 * 26-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * New implmentation of resources.
 *
 * NOTES:
 *  3-Oct-90: fedor
 * Most (if not all) of the resource routines check for a NULL resource
 * and simply return. This is desired because the central server does
 * not have a resource (currently) and this avoids many special case
 * tests elsewhere.
 *
 *****************************************************************************/

#include "globalS.h"
#ifdef DOS_FILE_NAMES
#include "comServe.h"
#else
#include "comServer.h"
#endif


/******************************************************************************
 *
 * FUNCTION: RESOURCE_PTR resourceCreate(sd, name, capacity)
 *
 * DESCRIPTION:
 * Resources are unique by module. 
 * If the same resource is requested to be created the capacity is simply
 * updated. NULL is returned if the resource is found.
 *
 * A capacity of less than 1 is simply set to 1 and a message displayed.
 *
 * INPUTS: 
 * int sd;
 * const char *name;
 * int32 capacity;
 *
 * OUTPUTS: RESOURCE_PTR (NULL value if resource is updated.)
 *
 *****************************************************************************/

static int32 resEqFunc(int *sd, RESOURCE_PTR resource)
{
  return((resource->readSd == *sd) || (resource->writeSd == *sd));
}

static int32 resEqNameFunc(char *name, RESOURCE_PTR resource)
{
  return (strcmp(name,resource->name) == 0);
}

RESOURCE_PTR resourceCreate(int readSd, int writeSd, 
			    const char *name, int32 capacity)
{
  LIST_PTR resList;
  RESOURCE_PTR resource;
  
  if (capacity < 1) {
    LOG_STATUS2("\nWARNING: Capacity of %d:%s set to 1\n", readSd, name);
    capacity = 1;
  }
  
  resList = (LIST_PTR)x_ipc_hashTableFind(name, GET_C_GLOBAL(resourceTable));
  
  if (resList) {
    resource = (RESOURCE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) resEqNameFunc, 
					       (char *)name, resList);
    if (resource) {
      if (resource->capacity != capacity) {
	LOG_STATUS3("\nWARNING: Capacity of %d:%s changed to %d\n",
		   readSd, name, capacity);
	resource->capacity = capacity;
      }
      RESOURCE_SET_STATUS(ActiveResource, resource);
      return resource;
    }
  }
  
  resource = NEW(RESOURCE_TYPE);
  
  resource->readSd = readSd;
  resource->writeSd = writeSd;
  
  resource->name = strdup(name);
  
  resource->capacity = capacity;
  resource->module = NULL;
  resource->pendingList = x_ipc_listCreate();
  resource->attendingList = x_ipc_listCreate();
  resource->pendingLimit = NO_PENDING_LIMIT;
  resource->msgLimitList = NULL;
  
  RESOURCE_SET_STATUS(ActiveResource, resource);
  
  if (!resList) {
    resList = x_ipc_listCreate();
    (void)x_ipc_hashTableInsert(resource->name, strlen(resource->name)+1,
			  (char *)resList, GET_C_GLOBAL(resourceTable));
  }

  x_ipc_listInsertItem((char *)resource, resList);
  
  return resource;
}


static int32 readChar(void)
{
  int32 i, c=0;
  
  while ((i = getchar()) != '\n' && i != EOF)
    c = i;
  
  return c;
}



/******************************************************************************
 *
 * FUNCTION: int32 tryResendingMessage
 *
 * DESCRIPTION: 
 * Ask to resend the message, and do so if the response is positive.
 *
 * INPUTS: char *param; (Ignored)
 *         DISPATCH_PTR dispatch;
 *
 * OUTPUTS: Returns 1 to continue iterating through the list
 *****************************************************************************/

/*ARGSUSED*/
static int32 tryResendingMessage(void *param, DISPATCH_PTR dispatch)
{
#ifdef UNUSED_PRAGMA
#pragma unused(param)
#endif
  LOG1("Resend %s message?", dispatch->msg->msgData->name);
  if (readChar() == 'y') {
    switch (dispatch->msg->msgData->msg_class) {
    case QueryClass:
    case MultiQueryClass:
    case InformClass:
    case BroadcastClass:
      DISPATCH_SET_STATUS(AllocatedDispatch, dispatch);
      processActiveMessage(dispatch);
      break;
#ifndef NMP_IPC
    case GoalClass:
    case CommandClass:
      dispatch->hnd = NULL;
      reExecMsg(dispatch, (DISPATCH_HND_PTR)NULL);
      break;
#endif
    default: LOG1("Cannot resend messages of %s class\n", 
		 x_ipc_messageClassName(dispatch->msg_class));
    }
  } else {
    LOG1("Not resending message: %s\n", dispatch->msg->msgData->name);
  }
  
  return 1; /* 28-Oct-91: fedor: test all */
}


/******************************************************************************
 *
 * FUNCTION: int32 clearHandler
 *
 * DESCRIPTION: Clear the handler slot of the dispatch (when the associated
 *              resource is deleted)
 *
 * INPUTS: char *param; (Ignored)
 *         DISPATCH_PTR dispatch;
 *
 * OUTPUTS: Returns 1 to continue iterating through the list
 *****************************************************************************/

/*ARGSUSED*/
static int32 clearHandler(char *param, DISPATCH_PTR dispatch)
{
#ifdef UNUSED_PRAGMA
#pragma unused(param)
#endif
  if (dispatch->msg && dispatch->msg->msgData) {
    LOG1("Clearing handler for message %s", dispatch->msg->msgData->name);
    Log_RefId(dispatch, LOGGING_ALWAYS);
    LOG("\n");
  }
  dispatch->hnd = NULL;
#ifndef NMP_IPC
  if (dispatch->treeNode) {
    LOG("  But keeping dispatch, since it has a task tree node\n");
  } else 
#endif
    {
      dispatchFree(dispatch);
    }

  return TRUE; /* test all items */
}

/******************************************************************************
 *
 * FUNCTION: int32 clearLimits
 *
 * DESCRIPTION: Clear the message limits when a resouce is deleted.
 *
 * INPUTS: char *param; (Ignored)
 *         DISPATCH_PTR dispatch;
 *
 * OUTPUTS: Returns 1 to continue iterating through the list
 *****************************************************************************/

/*ARGSUSED*/
static int32 clearLimits(char *param, LIMIT_PENDING_PTR limitPtr)
{
#ifdef UNUSED_PRAGMA
#pragma unused(param)
#endif
  if (limitPtr->msgName != NULL)
    x_ipcFree((void *)limitPtr->msgName);
  if (limitPtr->resName != NULL)
    x_ipcFree((void *)limitPtr->resName);
  x_ipcFree((void *)limitPtr);

  return TRUE; /* test all items */
}

/******************************************************************************
 *
 * FUNCTION: int32 multiQueryUpdateIterate
 *
 * DESCRIPTION: If the message being handled is a "MultiQuery", need to
 *              update the data structure that indicates how many responses are
 *              still outstanding.
 *
 * INPUTS: char *param; (Ignored)
 *         DISPATCH_PTR dispatch;
 *
 * OUTPUTS: Returns 1 to continue iterating through the list
 *****************************************************************************/

/*ARGSUSED*/
static int32 multiQueryUpdateIterate(char *param, DISPATCH_PTR dispatch)
{
#ifdef UNUSED_PRAGMA
#pragma unused(param, dispatch)
#endif
#ifndef NMP_IPC
  if (dispatch->msg && dispatch->msg->msgData->msg_class == MultiQueryClass) {
    multiQueryUpdate(dispatch, FALSE);
  }
#endif
  
  return TRUE; /* test all items */
}

/******************************************************************************
 *
 * FUNCTION: void resourceDelete(resource)
 *
 * DESCRIPTION:
 * Remove the named resource from the resource table.
 * Display a warning message if the pending or attending lists are not empty.
 *
 * INPUTS: RESOURCE_PTR resource;
 *
 * OUTPUTS: void.
 *
 * NOTES: If "resendAfterCrashGlobal" is true (the "-r" option to central),
 *        then will ask to resend each message in the active and pending sets
 *        of the resource. (25-Jul-91: Reid)
 *****************************************************************************/
static int clearHndRes(HND_KEY_PTR key, DISPATCH_HND_PTR hnd, 
		       RESOURCE_PTR resource)
{
#ifdef UNUSED_PRAGMA
#pragma unused(key)
#endif
  if (hnd && (hnd->resource == resource)) {
    hnd->resource = NULL;
  }
  
  return 1;
}

void resourceDelete(RESOURCE_PTR resource)
{
  LIST_PTR resList;
  LIST_ELEM_PTR listTmp;
  MODULE_PTR module;
  
  if (!resource)
    return;
  
  resList = (LIST_PTR)x_ipc_hashTableFind(resource->name,
				    GET_C_GLOBAL(resourceTable));
  x_ipc_listDeleteItem((char *)resource, resList);
  
  if (resList && !x_ipc_listLength(resList)) {
    (void)x_ipc_hashTableRemove(resource->name, GET_C_GLOBAL(resourceTable));
    x_ipc_listFree(&resList);
  }
  /* Need to remove from the module list.   */
  for (listTmp = GET_M_GLOBAL(moduleList)->first; 
       (listTmp != NULL);
       listTmp = listTmp->next
       ) {
    module = (MODULE_PTR)(listTmp->item);
    x_ipc_listDeleteItemAll(resource,module->resourceList);
  }
  if (x_ipc_listLength(resource->attendingList)) {
    if (GET_S_GLOBAL(resendAfterCrashGlobal)) {
      LOG1("Resend active and/or pending messages for the %s resource?",
	  resource->name);
      if (readChar() == 'y') {
	(void)x_ipc_listIterate((LIST_ITER_FN)tryResendingMessage, (char *)NULL, 
			  resource->attendingList);
	(void)x_ipc_listIterate((LIST_ITER_FN)tryResendingMessage, 
			  (char *)NULL, resource->pendingList);
      } else {
	LOG("Not resending messages.\n");
      }
    } else {
      (void)x_ipc_listIterate((LIST_ITER_FN)clearHandler, 
			(char *)NULL, resource->attendingList);
      (void)x_ipc_listIterate((LIST_ITER_FN)clearHandler, 
			(char *)NULL, resource->pendingList);
      (void)x_ipc_listIterate((LIST_ITER_FN)multiQueryUpdateIterate, (char *)NULL, 
			resource->attendingList);
      (void)x_ipc_listIterate((LIST_ITER_FN)multiQueryUpdateIterate, (char *)NULL, 
			resource->pendingList);
    }
    x_ipc_listFree(&(resource->attendingList));
    x_ipc_listFree(&(resource->pendingList));
  }
  
  (void)x_ipc_listIterate((LIST_ITER_FN)clearLimits, 
		    (char *)NULL, resource->msgLimitList);
  x_ipc_listFree(&(resource->pendingList));
  x_ipc_listFree(&(resource->attendingList));
  x_ipc_listFree(&(resource->msgLimitList));
  /* Need to remove references from the handlers to the resouce. */
  x_ipc_hashTableIterate((HASH_ITER_FN)clearHndRes, GET_C_GLOBAL(handlerTable), 
		   resource);
  x_ipcFree(resource->name);
  x_ipcFree((char *)resource);
}


/******************************************************************************
 *
 * FUNCTION: int32 resourceSelfQuery(dispatch)
 *
 * DESCRIPTION: Test if the message is a self query.
 *
 * INPUTS: DISPATCH_PTR dispatch
 *
 * OUTPUTS: int32 (TRUE or FALSE)
 *
 *****************************************************************************/

int32 resourceSelfQuery(DISPATCH_PTR dispatch)
{
  return (dispatch->msg->msgData->msg_class == QueryClass &&
	  dispatch->des->readSd == dispatch->org->readSd);
}


/******************************************************************************
 *
 * FUNCTION: int32 resourceAvailableRes(resource)
 *
 * DESCRIPTION: Test if the resource has capacity for a handler.
 *
 * INPUTS: RESOURCE_PTR resource;
 *
 * OUTPUTS: int32 (TRUE or FALSE)
 *
 * NOTES:
 * 1-Oct-90: fedor: resourceAvailableRes
 * Used to select handlers from duplicate identical name handlers from
 * duplicate modules.
 * Sooo this may return true when the correct answer is false because the
 * resource may be reserved or locked. This is ok because 
 * processResourceAndDeliver will call resourceAvailable with the complete
 * dispatch before sending - and that test will trap resource reserved and
 * locked conditions.
 *
 * 3-Dec-90: fedor: 
 * Now this is also used in the lock/reserve code to make sure the
 * resource is not busy before setting the lock/reserve.
 *
 *****************************************************************************/

int32 resourceAvailableRes(RESOURCE_PTR resource)
{
  return (resource
	  ? (x_ipc_listLength(resource->attendingList) < resource->capacity) : TRUE);
}


/******************************************************************************
 *
 * FUNCTION: int32 resourceAvailable(dispatch)
 *
 * DESCRIPTION: Test if the dispatch's handlers's resource is available.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: int32 (TRUE or FALSE)
 *
 *****************************************************************************/

int32 resourceAvailable(DISPATCH_PTR dispatch)
{
  RESOURCE_PTR resource;
  
  resource = dispatch->hnd->resource;
  
  if (!resource) {
    return TRUE;
  } else if (dispatch->des->wait == TRUE) {
    return FALSE; /* Don't send until x_ipcWaitUntilReady */
  } else if (resource->status == LockedResource) {
    return FALSE;
  } else if (resource->status != ReservedResource ||
	     resource->module == dispatch->org) {
    return(resourceAvailableRes(resource) || resourceSelfQuery(dispatch));
  } else {
    return FALSE;
  }
}


/******************************************************************************
 *
 * FUNCTION: void resourceAttending(dispatch)
 *
 * DESCRIPTION: Add the dispatch to its handler's resource attending list.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void resourceAttending(DISPATCH_PTR dispatch)
{
  if (dispatch->hnd->resource)
    x_ipc_listInsertItem((char *)dispatch, dispatch->hnd->resource->attendingList);
}


/******************************************************************************
 *
 * FUNCTION: void resourceRemoveAttending(dispatch)
 *
 * DESCRIPTION: Remove the dispatch fom its handler's resource attending list.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void resourceRemoveAttending(DISPATCH_PTR dispatch)
{
  if (dispatch->hnd && dispatch->hnd->resource)
    x_ipc_listDeleteItem((char *)dispatch, dispatch->hnd->resource->attendingList);
}


/******************************************************************************
 *
 * FUNCTION: void resourceRemovePending(dispatch)
 *
 * DESCRIPTION: 
 * Removes the dispatch from the dispatch's hnd resource pending set.
 * Interface routine for taskTree killing of tree nodes.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void.
 *
 * NOTES:
 * 24-Oct-90: fedor: little concerned over use of pending set to control
 * availability of resource and other pending items.
 *
 *****************************************************************************/

void resourceRemovePending(DISPATCH_PTR dispatch)
{
  if (dispatch->hnd && dispatch->hnd->resource)
    x_ipc_listDeleteItem((char *)dispatch, dispatch->hnd->resource->pendingList);
}


/******************************************************************************
 *
 * FUNCTION: void resourcePending(dispatch)
 *
 * DESCRIPTION: Add the dispatch to its handler's resource pending list.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static int32 limitEqFunc(char *msgName, LIMIT_PENDING_PTR limitPtr)
{
  return(x_ipc_strKeyEqFunc(msgName, limitPtr->msgName));
}

/* Returns the number of pending messages of the given name, and sets
   the first one of the list that matches */
static int32 numPending (const char *msgName, LIST_PTR pendingList,
		       DISPATCH_PTR *oldest)
{
  DISPATCH_PTR dispatch;
  int32 num = 0;
  
  dispatch = (DISPATCH_PTR)x_ipc_listFirst(pendingList);
  while (dispatch) {
    if (STREQ(dispatch->msg->msgData->name, msgName)) {
      if (num == 0) *oldest = dispatch;
      num++;
    }
    dispatch = (DISPATCH_PTR)x_ipc_listNext(pendingList); 
  }
  return num;
}

static void deletePendingDispatch(DISPATCH_PTR dispatch)
{
  int32 ignored;
  
  ignored = Ignore_Logging_Message(dispatch->msg);
  if (ignored) Start_Ignore_Logging();
  
  LOG_STATUS("PENDING LIMIT: ");
  
  resourceRemovePending(dispatch);
  
#ifndef NMP_IPC
  if (dispatch->treeNode) {
    RetractHoldingConstraint(dispatch->treeNode);
    
    /* If this is a blocking command, issue a reply to prevent deadlock */
    if (dispatch->blockCom && dispatch->blockCom->waitFlag &&
	dispatch->msg && dispatch->msg->msgData->msg_class == CommandClass) {
      blockingCommandReply(dispatch, FailureDispatch);
    }
    dispatch->treeNode->status = ToBeKilledNode;
    /* Will force message to be freed when all references are released */
    DISPATCH_SET_STATUS(HandledDispatch, dispatch);
    HandleKillAfterAttendingNodes(dispatch);
    (void)runRules(-1);
  } else 
#endif
  {
    if (dispatch->refCount <= 0) {
      LOG_STATUS1("Deleted message %s", dispatch->msg->msgData->name);
      Log_RefId(dispatch, LOGGING_STATUS);
      Log_Time(1); LOG_STATUS("\n");
      dispatchFree(dispatch);
    } else {
      LOG_STATUS1("Will free %s", dispatch->msg->msgData->name);
      Log_RefId(dispatch, LOGGING_STATUS);
      LOG_STATUS(" when all references to it are released\n");
      /* Will force message to be freed when all references are released */
      DISPATCH_SET_STATUS(HandledDispatch, dispatch);
    }
  }
  if (ignored) End_Ignore_Logging();
}

/* Delete the dispatch, but move all dispatches with the same message name
   up, to ensure fairness */
static void removePendingWithFairness (DISPATCH_PTR oldDispatch, 
				       DISPATCH_PTR newDispatch)
{
  DISPATCH_PTR moveUp;
  LIST_PTR pendingList;
  LIST_ELEM_PTR element, lastDispatchElement=NULL;
  const char *msgName;
  
  msgName = newDispatch->msg->msgData->name;
  pendingList = newDispatch->hnd->resource->pendingList;
  
  /* This is *really* crufty, but I need to get it up and working.
   * Figure out the neat way to do it later (Reid, 5/30/94) */
  
  element = pendingList->first;
  while (element) {
    moveUp = (DISPATCH_PTR)element->item;
    if (STREQ(moveUp->msg->msgData->name, msgName)) {
      if (lastDispatchElement) {
	lastDispatchElement->item = element->item;
      }
      lastDispatchElement = element;
    }
    element = element->next;
  }
  if (lastDispatchElement != NULL)
    lastDispatchElement->item = (const char *)newDispatch;
  
  deletePendingDispatch(oldDispatch);
}

#ifdef NMP_IPC
static void addPendingInPriorityOrder(DISPATCH_PTR dispatch, int priority,
				      RESOURCE_PTR resource)
{
  DISPATCH_PTR lastPending, nextPending;

  lastPending = NULL;
  nextPending = (DISPATCH_PTR)x_ipc_listFirst(resource->pendingList);
  while (nextPending && priority <= nextPending->msg->priority) {
    lastPending = nextPending;
    nextPending = (DISPATCH_PTR)x_ipc_listNext(resource->pendingList);
  }
  x_ipc_listInsertItemAfter((const void *)dispatch, (void *)lastPending,
		      resource->pendingList);
}
#endif

void resourcePending(DISPATCH_PTR dispatch)
{
  RESOURCE_PTR resource;
  LIMIT_PENDING_PTR msgLimit;
  DISPATCH_PTR oldest=NULL;
  
  resource = dispatch->hnd->resource;
  if (resource->msgLimitList) {
    msgLimit = 
      (LIMIT_PENDING_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN)limitEqFunc,
					   dispatch->hnd->hndData->msgName,
					   resource->msgLimitList);
    if (msgLimit &&
	msgLimit->limit <= numPending(msgLimit->msgName,
				      resource->pendingList, &oldest)) {
      removePendingWithFairness(oldest, dispatch);
      return;
    }
  }
  if (resource->pendingLimit != NO_PENDING_LIMIT &&
      resource->pendingLimit <= x_ipc_listLength(resource->pendingList)) {
    oldest = (DISPATCH_PTR)x_ipc_listFirst(resource->pendingList);
    removePendingWithFairness(oldest, dispatch);
    return;
  }
  
#ifdef NMP_IPC
  if (dispatch->msg->priority != DEFAULT_PRIORITY && 
      x_ipc_listLength(resource->pendingList) > 0) {
    addPendingInPriorityOrder(dispatch, dispatch->msg->priority, resource);
  } else
#endif
  x_ipc_listInsertItemLast((char *)dispatch, resource->pendingList);
}


/******************************************************************************
 *
 * FUNCTION: void resourceProcessPendingRes(resource)
 *
 * DESCRIPTION:
 * Process a pending dispatch for this resource.
 * If the resource is locked no dispatch is processed.
 * If the resource is reserved then only a dispatch of the owner of the
 * reservation is processed.
 *
 * Called by resourceProcessPending from flow and control and called by
 * cancelation of a reservation or a lock to allow the resource pending
 * dispatches to continue.
 *
 * INPUTS: RESOURCE_PTR resource;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static int32 sameModTest(MODULE_PTR module, DISPATCH_PTR dispatch)
{
  return(dispatch->org == module);
}

void resourceProcessPendingRes(RESOURCE_PTR resource)
{
  DISPATCH_PTR pendingDispatch;
  
  if (resource && (resource->status != LockedResource) &&
      resourceAvailableRes(resource)) {
    if (resource->status == ReservedResource) {
      pendingDispatch = 
	(DISPATCH_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) sameModTest, 
					(char *)resource->module,
					resource->pendingList);
      x_ipc_listDeleteItem((char *)pendingDispatch, resource->pendingList);
    } else {
      pendingDispatch = (DISPATCH_PTR)x_ipc_listPopItem(resource->pendingList);
    }

    if (pendingDispatch) 
      processResAttendDeliver(pendingDispatch);
  }
}


/******************************************************************************
 *
 * FUNCTION: void resourceProcessPending(dispatch)
 *
 * DESCRIPTION: 
 * Calls resourceProcessPendingRes. Provides an interface to resources for
 * flow and control that deals in terms of dispatches.
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void resourceProcessPending(DISPATCH_PTR dispatch)
{
  if (dispatch->hnd)
    resourceProcessPendingRes(dispatch->hnd->resource);
}


/******************************************************************************
 *
 * FUNCTION: void registerResourceHnd(dispatch, addResForm)
 *
 * DESCRIPTION: Handler for x_ipcRegisterResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * ADD_RES_FORM_PTR addResForm;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void registerResourceHnd(DISPATCH_PTR dispatch,
				ADD_RES_FORM_PTR addResForm)
{
  RESOURCE_PTR resource;
  
  resource = resourceCreate(dispatch->orgId,dispatch->orgId,
			    addResForm->resName, 
			    addResForm->capacity);

  if (resource &&
      /* Not just redefining an existing resource */
      !x_ipc_listMemberItem(resource, dispatch->org->resourceList)) {
    x_ipc_listInsertItem((char *)resource, dispatch->org->resourceList);
    
    x_ipc_strListPushUnique(strdup(resource->name),dispatch->org->providesList);
  }

  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)addResForm);
}


/******************************************************************************
 *
 * FUNCTION: void addHndToResourceHnd(dispatch, addHndForm)
 *
 * DESCRIPTION: The handler for x_ipcAddHndToResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * ADD_HND_FORM_PTR addHndForm;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static int32 equalHndName(char *hndName, HND_PTR hnd)
{
  return(x_ipc_strKeyEqFunc(hndName, hnd->hndData->hndName));
}

static int32 equalResName(char *resName, RESOURCE_PTR res)
{
  return(x_ipc_strKeyEqFunc(resName, res->name));
}

typedef struct { HND_PTR handler;
		 RESOURCE_PTR newResource;
		 RESOURCE_PTR oldResource;
	       } TRANSFER_HANDLER_TYPE, *TRANSFER_HANDLER_PTR;

static int32 transferPendingToNewResource(TRANSFER_HANDLER_PTR transferData,
					  DISPATCH_PTR dispatch)
{
  if ((HND_PTR)dispatch->hnd == transferData->handler) {
  
    LOG_STATUS1("   Transferring %s", DISPATCH_MSG_NAME(dispatch));
    Log_RefId(dispatch, LOGGING_STATUS);
    LOG_STATUS2(" from Resource %s to %s\n", transferData->oldResource->name,
	       transferData->newResource->name);

    x_ipc_listDeleteItem(dispatch, transferData->oldResource->pendingList);
    dispatch->resource = transferData->newResource;
    resourcePending(dispatch);
  }
  return TRUE;
}

static void addHndToResourceHnd(DISPATCH_PTR dispatch,
				ADD_HND_FORM_PTR addHndForm)
{
  HND_PTR hnd;
  RESOURCE_PTR resource, oldResource;
  TRANSFER_HANDLER_TYPE transferData;
  
  hnd = (HND_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) equalHndName,
				   addHndForm->hndName,
				   dispatch->org->hndList);
  
  if (!hnd)
    X_IPC_ERROR1("ERROR: x_ipcAddHndToResource: no handler %s\n",
	     addHndForm->hndName);
  
  resource = (RESOURCE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) equalResName,
					     addHndForm->resName,
					     dispatch->org->resourceList);  
  
  if (!resource)
    X_IPC_ERROR1("ERROR: x_ipcAddHndToResource: no resource %s\n",
	     addHndForm->resName);
  
  if (hnd->resource != dispatch->org->impliedResource)
    LOG_STATUS3("\nWARNING: Resource for %s is being changed from %s to %s.\n",
	       hnd->hndData->hndName, hnd->resource->name, 
	       resource->name);

  oldResource = hnd->resource;
  hnd->resource = resource;

  /* Need to transfer any pending messages to the new resource (Reid 7/96) */
  transferData.oldResource = oldResource;
  transferData.newResource = resource;
  transferData.handler = hnd;
  x_ipc_listIterateFromFirst((LIST_ITER_FN)transferPendingToNewResource,
		       (void *)&transferData, oldResource->pendingList);

  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)addHndForm);
}

/******************************************************************************
 *
 * FUNCTION: void limitPendingHnd(dispatch, limitPendingData)
 *
 * DESCRIPTION: The handler for both x_ipcLimitPendingMessages and
 *                x_ipcLimitPendingResource.
 *              If the msgName field is NULL, the limit applies to all
 *                messages in the resource, o/w just to the named message.
 *              The resource must have been registered already.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * LIMIT_PENDING_PTR limitPendingData;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void limitPendingHnd(DISPATCH_PTR dispatch,
			    LIMIT_PENDING_PTR limitPendingData)
{
  LIST_PTR resList;
  RESOURCE_PTR resource;
  LIMIT_PENDING_PTR msgLimit;
  int32 freeData = TRUE;
  
  resList = (LIST_PTR)x_ipc_hashTableFind(limitPendingData->resName,
				    GET_C_GLOBAL(resourceTable));
  if (!resList) {
    LOG_STATUS1("ERROR: Non-existent resource %s; Pending limit not applied\n",
	       limitPendingData->resName);
  } else {
    if (x_ipc_listLength(resList) == 1) {
      resource = (RESOURCE_PTR)x_ipc_listFirst(resList);
    } else {
      /* If there is more than one resource with the same name, try to choose
	 the one that is associated with the module that sent the message */
      resource = (RESOURCE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) resEqFunc, 
						 (char *)&dispatch->org->readSd,
						 resList);
      if (!resource) {
	LOG_STATUS1("ERROR: Multiple resources named %s; Pending limit not applied\n",
		   limitPendingData->resName);
	return;
      }
    }
    if (!limitPendingData->msgName) {
      /* Limit applies to the whole resource */
      if (resource->pendingLimit != NO_PENDING_LIMIT &&
	  resource->pendingLimit != limitPendingData->limit) {
	LOG_STATUS3("WARNING: Changing pending resource limit of %s from %d to %d",
		   limitPendingData->resName, resource->pendingLimit,
		   limitPendingData->limit);
      }
      resource->pendingLimit = limitPendingData->limit;
    } else {
      if (!resource->msgLimitList) resource->msgLimitList = x_ipc_listCreate();
      msgLimit = 
	(LIMIT_PENDING_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN)limitEqFunc,
					     limitPendingData->msgName,
					     resource->msgLimitList);
      if (!msgLimit) {
	x_ipc_listInsertItem(limitPendingData, resource->msgLimitList);
	freeData = FALSE;
      } else if (msgLimit->limit != limitPendingData->limit) {
	LOG_STATUS3("WARNING: Changing pending message limit of %s from %d to %d",
		   msgLimit->msgName, msgLimit->limit,
		   limitPendingData->limit);
	msgLimit->limit = limitPendingData->limit;
      }
    }
  }
  if (freeData) {
    x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, limitPendingData);
  }
}

/*****************************************************************
 * Return TRUE if the resource lock/reservation request came from
 * the same resource that is being locked/reserved (to prevent 
 * deadlock, since the lock/reservation request is a blocking query)
 ****************************************************************/

static int32 selfRequestingResource (RESOURCE_PTR resource, 
				   DISPATCH_PTR dispatch)
{
  return (RESOURCE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN)equalResName,
					 resource->name,
					 dispatch->org->resourceList) != NULL;
}

/******************************************************************************
 *
 * FUNCTION: void resourceSetStatus(dispatch, resource, status)
 *
 * DESCRIPTION:
 * Sets the reserved or locked status of a resource.
 * Checks for multiple reservations of the same module
 *
 * INPUTS:
 * DISPATCH_PTR dispatch;
 * RESOURCE_PTR resource;
 * RESOURCE_STATUS_TYPE status;
 *
 * OUTPUTS: void.
 *
 * NOTES: This handles the central reply for reservations and locks.
 *
 *****************************************************************************/

static void resourceSetStatus(DISPATCH_PTR dispatch, RESOURCE_PTR resource,
			      RESOURCE_STATUS_TYPE status)
{
  int32 reject;
  
  if (resource->status == ActiveResource &&
      (resourceAvailableRes(resource) ||
       selfRequestingResource(resource, dispatch))) {
    reserveDispatch(dispatch);
    resource->module = dispatch->org;
    dispatch->resource = resource;
    RESOURCE_SET_STATUS(status, resource);
    centralReply(dispatch, (char *)&(dispatch->locId));
  } else if (status == ReservedResource && resource->module == dispatch->org) {
    /* multiple request for reservation from same module */
    reject = -1;
    centralReply(dispatch, (char *)&reject);
  } else {
    x_ipc_listInsertItem((char *)dispatch, resource->pendingList);
    dispatchUpdateAndDisplay(CentralPendingDispatch, dispatch);
  }
}


/******************************************************************************
 *
 * FUNCTION: void resourceReserveLock(dispatch, resName, status)
 *
 * DESCRIPTION:
 * Called by the handlers for x_ipcReserveResource or x_ipcLockResource.
 * The local id of the dispatch for this message becomes the reference for
 * the reservation or lock. This dispatch can not be freed until the 
 * reservation or lock is removed.
 *
 * If a reservation or lock is already in progress then this message is
 * queued in the pending set of the resource to be sent again when
 * the resource becomes available.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * char *resName;
 * RESOURCE_STATUS_TYPE status;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void resourceReserveLock(DISPATCH_PTR dispatch, char *resName,
				RESOURCE_STATUS_TYPE status)
{
  LIST_PTR resList;
  RESOURCE_PTR resource;
  
  resList = (LIST_PTR)x_ipc_hashTableFind(resName, GET_C_GLOBAL(resourceTable));
  
  if (!resList)
    X_IPC_ERROR2("ERROR: %s: No Resource: %s\n",
	     dispatch->msg->msgData->name, resName);
  
  if (x_ipc_listLength(resList) > 1)
    X_IPC_ERROR2("ERROR: %s: Resource %s is not unique. Specify module.\n", 
	     dispatch->msg->msgData->name, resName);
  
  resource = (RESOURCE_PTR)x_ipc_listFirst(resList);
  
  resourceSetStatus(dispatch, resource, status);
}


/******************************************************************************
 *
 * FUNCTION: void resourceModReserveLock(dispatch, addForm, status)
 *
 * DESCRIPTION: 
 * Called by the handlers for x_ipcReserveModResource and x_ipcLockModResource.
 * The local id of the dispatch for this message becomes the reference for
 * the reservation or lock. This dispatch can not be freed until the 
 * reservation or lock is removed.
 *
 * If a reservation or lock is already in progress then this message is
 * queued in the pending set of the resource to be sent again when
 * the resource becomes available.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * ADD_HND_FORM_PTR addForm;
 * RESOURCE_STATUS_TYPE status;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static int32 moduleNameEq(char *modName, MODULE_PTR module)
{
  return(x_ipc_strKeyEqFunc(modName, module->modData->modName));
}

static void resourceModReserveLock(DISPATCH_PTR dispatch,
				   ADD_HND_FORM_PTR addForm,
				   RESOURCE_STATUS_TYPE status)
{
  LIST_PTR resList;
  MODULE_PTR module;
  RESOURCE_PTR resource;
  
  resList = (LIST_PTR)x_ipc_hashTableFind(addForm->resName, 
				    GET_C_GLOBAL(resourceTable));
  
  if (!resList)
    X_IPC_ERROR2("ERROR: %s: No Resource: %s\n", 
	     dispatch->msg->msgData->name, addForm->resName);
  
  module = (MODULE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) moduleNameEq,
					 addForm->hndName, 
					 GET_M_GLOBAL(moduleList));
  
  resource = (RESOURCE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) resEqFunc,
					     (char *)&(module->readSd),
					     resList);
  
  if (!resource)
    X_IPC_ERROR3("ERROR: %s: No Resource: %s: Registered by: %s\n", 
	     dispatch->msg->msgData->name, addForm->resName, addForm->hndName);
  
  resourceSetStatus(dispatch, resource, status);
}


/******************************************************************************
 *
 * FUNCTION: void reserveResourceHnd(dispatch, resName)
 *
 * DESCRIPTION: Handler for x_ipcReserveResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * char **resName;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void reserveResourceHnd(DISPATCH_PTR dispatch, char **resName)
{
  resourceReserveLock(dispatch, *resName, ReservedResource);
  
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)resName);
}


/******************************************************************************
 *
 * FUNCTION: void reserveModResourceHnd(dispatch, addForm)
 *
 * DESCRIPTION: Handler for x_ipcReserveModResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * ADD_HND_FORM_PTR addForm;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void reserveModResourceHnd(DISPATCH_PTR dispatch,
				  ADD_HND_FORM_PTR addForm)
{
  resourceModReserveLock(dispatch, addForm, ReservedResource);
  
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)addForm);
}


/******************************************************************************
 *
 * FUNCTION: void cancelReservationHnd(dispatch, ref)
 *
 * DESCRIPTION: 
 * Handler for x_ipcCancelReservation.
 * Also called by the handler for x_ipcUnlockResource.
 *
 * INPUTS:
 * DISPATCH_PTR dispatch;
 * int32 *ref; 
 *
 * OUTPUTS: void.
 *
 * NOTES: ref should be changed to a X_IPC_REF_PTR
 *
 *****************************************************************************/

/*ARGSUSED*/
static void cancelReservationHnd(DISPATCH_PTR dispatch, int32 *ref)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dispatch)
#endif
  DISPATCH_PTR reservation;
  
  reservation = DISPATCH_FROM_ID(*ref);
  
  reservation->resource->module = NULL;
  RESOURCE_SET_STATUS(ActiveResource, reservation->resource);
  
  resourceProcessPendingRes(reservation->resource);
  
  releaseDispatch(reservation);
  
  x_ipcFree((char *)ref); /* Use simple free: only 1 int32 */
}


/******************************************************************************
 *
 * FUNCTION: void lockResourceHnd(dispatch, resName)
 *
 * DESCRIPTION: Handler for x_ipcLockResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * char **resName;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void lockResourceHnd(DISPATCH_PTR dispatch, char **resName)
{
  resourceReserveLock(dispatch, *resName, LockedResource);
  
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)resName);
}


/******************************************************************************
 *
 * FUNCTION: void lockModResourceHnd(dispatch, addForm)
 *
 * DESCRIPTION: Handler for x_ipcLockModResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * ADD_HND_FORM_PTR addForm;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void lockModResourceHnd(DISPATCH_PTR dispatch, ADD_HND_FORM_PTR addForm)
{
  resourceModReserveLock(dispatch, addForm, LockedResource);
  
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)addForm);
}


/******************************************************************************
 *
 * FUNCTION: void unlockResourceHnd(dispatch, ref)
 *
 * DESCRIPTION: Handler for x_ipcUnlockResource.
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * int32 *ref;
 *
 * OUTPUTS: void.
 *
 * NOTES: ref should be changed to a X_IPC_REF_PTR
 *
 *****************************************************************************/

static void unlockResourceHnd(DISPATCH_PTR dispatch, int32 *ref)
{
  cancelReservationHnd(dispatch, ref);
  
  x_ipcFree((char *)ref); /* Use simple free: only 1 int */
}


/******************************************************************************
 *
 * FUNCTION: void resourceInitialize()
 *
 * DESCRIPTION: Initialize resources.
 *
 * INPUTS: none.
 *
 * OUTPUTS: none.
 *
 *****************************************************************************/

void resourceInitialize(void)
{
  centralRegisterInform(X_IPC_REGISTER_RESOURCE_INFORM, 
			X_IPC_REGISTER_RESOURCE_INFORM_FORMAT,
			registerResourceHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_RESOURCE_INFORM);

  centralRegisterInform(X_IPC_REGISTER_RESOURCE_INFORM_OLD, 
			X_IPC_REGISTER_RESOURCE_INFORM_FORMAT,
			registerResourceHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_RESOURCE_INFORM_OLD);
  
  centralRegisterInform(X_IPC_HANDLER_TO_RESOURCE_INFORM,
			X_IPC_HANDLER_TO_RESOURCE_INFORM_FORMAT,
			addHndToResourceHnd);
  Add_Message_To_Ignore(X_IPC_HANDLER_TO_RESOURCE_INFORM);
  
  centralRegisterInform(X_IPC_HANDLER_TO_RESOURCE_INFORM_OLD,
			X_IPC_HANDLER_TO_RESOURCE_INFORM_FORMAT,
			addHndToResourceHnd);
  Add_Message_To_Ignore(X_IPC_HANDLER_TO_RESOURCE_INFORM_OLD);
  
  centralRegisterInform(X_IPC_LIMIT_PENDING_INFORM,
			X_IPC_LIMIT_PENDING_INFORM_FORMAT,
			limitPendingHnd);
  Add_Message_To_Ignore(X_IPC_LIMIT_PENDING_INFORM);
  
  centralRegisterInform(X_IPC_LIMIT_PENDING_INFORM_OLD,
			X_IPC_LIMIT_PENDING_INFORM_FORMAT,
			limitPendingHnd);
  Add_Message_To_Ignore(X_IPC_LIMIT_PENDING_INFORM_OLD);
  
  centralRegisterQuery(X_IPC_RESERVE_RESOURCE_QUERY,
		       X_IPC_RESERVE_RESOURCE_QUERY_FORMAT,
		       X_IPC_RESERVE_RESOURCE_QUERY_REPLY,
		       reserveResourceHnd);
  
  centralRegisterQuery(X_IPC_RESERVE_MOD_RESOURCE_QUERY,
		       X_IPC_RESERVE_MOD_RESOURCE_QUERY_FORMAT,
		       X_IPC_RESERVE_MOD_RESOURCE_QUERY_REPLY,
		       reserveModResourceHnd);
  
  centralRegisterQuery(X_IPC_RESERVE_MOD_RESOURCE_QUERY_OLD,
		       X_IPC_RESERVE_MOD_RESOURCE_QUERY_FORMAT,
		       X_IPC_RESERVE_MOD_RESOURCE_QUERY_REPLY,
		       reserveModResourceHnd);
  
  centralRegisterInform(X_IPC_CANCEL_RESOURCE_INFORM,
			X_IPC_CANCEL_RESOURCE_INFORM_FORMAT,
			cancelReservationHnd);
  
  centralRegisterInform(X_IPC_CANCEL_RESOURCE_INFORM_OLD,
			X_IPC_CANCEL_RESOURCE_INFORM_FORMAT,
			cancelReservationHnd);
  
  centralRegisterQuery(X_IPC_LOCK_RESOURCE_QUERY,
		       X_IPC_LOCK_RESOURCE_QUERY_FORMAT,
		       X_IPC_LOCK_RESOURCE_QUERY_REPLY,
		       lockResourceHnd);
  
  centralRegisterQuery(X_IPC_LOCK_RESOURCE_QUERY_OLD,
		       X_IPC_LOCK_RESOURCE_QUERY_FORMAT,
		       X_IPC_LOCK_RESOURCE_QUERY_REPLY,
		       lockResourceHnd);
  
  centralRegisterQuery(X_IPC_LOCK_MOD_RESOURCE_QUERY,
		       X_IPC_LOCK_MOD_RESOURCE_QUERY_FORMAT,
		       X_IPC_LOCK_MOD_RESOURCE_QUERY_REPLY,
		       lockModResourceHnd);
  
  centralRegisterQuery(X_IPC_LOCK_MOD_RESOURCE_QUERY_OLD,
		       X_IPC_LOCK_MOD_RESOURCE_QUERY_FORMAT,
		       X_IPC_LOCK_MOD_RESOURCE_QUERY_REPLY,
		       lockModResourceHnd);
  
  centralRegisterInform(X_IPC_UNLOCK_RESOURCE_INFORM,
			X_IPC_UNLOCK_RESOURCE_INFORM_FORMAT,
			unlockResourceHnd);
  
  centralRegisterInform(X_IPC_UNLOCK_RESOURCE_INFORM_OLD,
			X_IPC_UNLOCK_RESOURCE_INFORM_FORMAT,
			unlockResourceHnd);
  
  GET_C_GLOBAL(resourceTable) = x_ipc_hashTableCreate(11, (HASH_FN)x_ipc_strHashFunc, 
						(EQ_HASH_FN)x_ipc_strKeyEqFunc);
}


/******************************************************************************
 *
 * FUNCTION: BOOLEAN purgeResoucePending(void *key, LIST_PTR resList)
 *
 * DESCRIPTION: Purge all pending queues.
 *
 * INPUTS: void pointer and a pointer to a list of resources.
 *
 * OUTPUTS: void
 *
 * NOTES: 
 *
 *****************************************************************************/

static BOOLEAN purgeResoucePendingItr(void *key, LIST_PTR resList)
{
#ifdef UNUSED_PRAGMA
#pragma unused(key)
#endif
  const LIST_ELEM_TYPE *tmp, *nextTmp;
  RESOURCE_PTR resource;
  DISPATCH_PTR oldest, newest;
  
  if (!resList)
    return FALSE;
  else {
    tmp = resList->first;
    while (tmp) {
      nextTmp = tmp->next;
      resource = (RESOURCE_PTR) tmp->item;
      oldest = (DISPATCH_PTR)x_ipc_listFirst(resource->pendingList);
      newest = (DISPATCH_PTR)x_ipc_listLast(resource->pendingList);
      if ((oldest) && (newest) && (oldest != newest))
	removePendingWithFairness(oldest, newest);
      tmp = nextTmp;
    }
  }
  return TRUE;
}

void purgeResoucePending(void)
{  
  x_ipc_hashTableIterate((HASH_ITER_FN)purgeResoucePendingItr,
		   GET_C_GLOBAL(resourceTable), NULL);
}


/******************************************************************************
 *
 * FUNCTION: void showResourceStatus(void)
 *
 * DESCRIPTION: Display status of all resources.
 *
 * INPUTS: void
 *
 * OUTPUTS: void
 *
 * NOTES: 
 *
 *****************************************************************************/

static BOOLEAN showResouceItr(void *key, LIST_PTR resList)
{
#ifdef UNUSED_PRAGMA
#pragma unused(key)
#endif
  const LIST_ELEM_TYPE *tmp;
  RESOURCE_PTR resource;
  
  if (!resList)
    return FALSE;
  else {
    tmp = resList->first;
    while (tmp) {
      resource = (RESOURCE_PTR) tmp->item;
      LOG3(" Resource %s  Attending: %d  Pending: %d ", resource->name,
	  x_ipc_listLength(resource->attendingList),
	  x_ipc_listLength(resource->pendingList));
      switch (resource->status) {
      case ActiveResource:
	LOG("Resource Status: Active\n");
	break;
      case ReservedResource:
	LOG("Resource Status: Reserved\n");
	break;
      case LockedResource:
	LOG("Resource Status: Locked\n");
	break;
      default:
	LOG("Resource Status: ???\n");
	break;
      }
      tmp = tmp->next;
    }
  }
  return TRUE;
}

void showResourceStatus(void)
{
  LOG("Resource status\n");
  x_ipc_hashTableIterate((HASH_ITER_FN)showResouceItr,
		   GET_C_GLOBAL(resourceTable), NULL);
}

void unlockResource(char *name)
{
  int32 i;
  DISPATCH_PTR dispatch;
  
  /* Need to find the locking dispatch. */
  for (i=0; i<GET_S_GLOBAL(dispatchTable)->currentSize; i++) {
    dispatch = (DISPATCH_PTR)idTableItem(i, GET_S_GLOBAL(dispatchTable));
    if ((dispatch != NULL) &&
	(dispatch->resource != NULL) &&
	(strcmp(dispatch->resource->module->modData->modName,name) == 0)) {
      cancelReservationHnd(NULL, &(dispatch->locId));
    }
  }
}

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: resources
 *
 * FILE: resMod.c
 *
 * ABSTRACT:
 * 
 * Resources - Module Routines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: resMod.c,v $
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:17  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.2  1996/12/18 15:13:08  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.1  1996/10/18 18:10:23  reids
 * Better error checking and handling.
 *
 * Revision 1.1  1996/05/09 01:01:57  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:30  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.17  1996/08/22  16:36:01  rich
 * Check the return code on x_ipcQueryCentral calls.
 *
 * Revision 1.16  1996/06/25  20:51:33  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.15  1995/11/03  03:04:46  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.14  1995/10/25  22:48:51  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.13  1995/06/14  03:22:18  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.12  1995/04/19  14:28:52  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.11  1995/04/07  05:03:29  rich
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
 * Revision 1.10  1995/01/18  22:42:35  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.9  1994/10/25  17:10:49  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.8  1994/05/25  17:32:37  reids
 * Added utilities to limit the number of pending messages
 *
 * Revision 1.7  1994/05/17  23:17:28  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1994/04/16  19:43:10  rich
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
 * Revision 1.5  1993/11/21  20:19:25  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/30  21:54:26  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.3  1993/08/27  07:16:47  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:19:03  rich
 * Fixed up the comments at the top of the file.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 30-Jan-91 Christopher Fedor, School of Computer Science, CMU
 * Added fflush(stdout) to printf for module code calls from lisp
 *
 * 24-Sep-90 Christopher Fedor, School of Computer Science, CMU
 * Module resource routines - x_ipc version 5.0
 *
 * 10-Jul-90 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

#include "resMod.h"


/******************************************************************************
 *
 * FUNCTION: void x_ipcRegisterResource(resName, capacity)
 *
 * DESCRIPTION: Define a new resource for a module.
 *
 * INPUTS: 
 * char *resName;
 * int32 capacity;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcRegisterResource(const char *resName, int32 capacity)
{
  ADD_RES_FORM_TYPE addResForm;
  
  addResForm.capacity = capacity;
  addResForm.resName = resName;
  
  (void)x_ipcInform(X_IPC_REGISTER_RESOURCE_INFORM, (void *)&addResForm);
  
  LOCK_CM_MUTEX;
  if(GET_C_GLOBAL(directDefault)) {
    /* Make the default module direct. */
    x_ipcDirectResource(resName);
  }
  UNLOCK_CM_MUTEX;
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcAddHndToResource(hndName, resName)
 *
 * DESCRIPTION: Assign a resource to a handler.
 *
 * INPUTS: char *hndName, *resName;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcAddHndToResource(const char *hndName, const char *resName)
{
  ADD_HND_FORM_TYPE addHndForm;
  
  addHndForm.hndName = hndName;
  addHndForm.resName = resName;
  
  (void)x_ipcInform(X_IPC_HANDLER_TO_RESOURCE_INFORM, (void *)&addHndForm);
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcLimitPendingResource(resName, int32 limit)
 *
 * DESCRIPTION: Allow only a maximum of "limit" messages of *any type* to
 *              be kept on the pending queue of the resource "resName".
 *              Deletes messages in a FIFO manner to maintain the limit
 *
 * INPUTS: char *resName; Name of the resource whose pending queue is affected
 *                        (if no special resource has been defined, just use
 *                         the module name)
 *         int32 limit;     Maximum number of pending messages to maintain
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcLimitPendingResource(const char *resName, int32 limit)
{
  LIMIT_PENDING_TYPE limitPendingData;
  
  limitPendingData.msgName = NULL;
  limitPendingData.resName = resName;
  limitPendingData.limit = limit;
  
  if (limit < 0) {
    X_IPC_MOD_ERROR2("Illegal pending limit %d for %s ignored.", limit, resName);
    return;
  }
  
  (void)x_ipcInform(X_IPC_LIMIT_PENDING_INFORM, (void *)&limitPendingData);
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcLimitPendingMessages(msgName, resName, int32 limit)
 *
 * DESCRIPTION: Allow only a maximum of "limit" messages of "msgName" to
 *              be kept on the pending queue of the resource "resName".
 *              Deletes messages in a FIFO manner to maintain the limit
 *
 * INPUTS: char *msgName; Name of the message to limit
 *         char *resName; Name of the resource whose pending queue is affected
 *                        (if no special resource has been defined, just use
 *                         the module name)
 *         int32 limit;     Maximum number of pending messages to maintain
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcLimitPendingMessages(const char *msgName, const char *resName, 
			     int32 limit)
{
  LIMIT_PENDING_TYPE limitPendingData;
  
  limitPendingData.msgName = msgName;
  limitPendingData.resName = resName;
  limitPendingData.limit = limit;
  
  if (limit < 0) {
    X_IPC_MOD_ERROR2("Illegal pending limit %d for %s ignored.", limit, msgName);
    return;
  }
  
  (void)x_ipcInform(X_IPC_LIMIT_PENDING_INFORM, (void *)&limitPendingData);
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_REF_PTR x_ipcReserveResource(resName)
 *
 * DESCRIPTION: Reserve a resource for use only by the reserving module.
 *
 * INPUTS: char *resName;
 *
 * OUTPUTS: X_IPC_REF_PTR
 *
 *****************************************************************************/

X_IPC_REF_PTR x_ipcReserveResource(const char *resName)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR ref;
  
  refId = 0;
  
  if (x_ipcQuery(X_IPC_RESERVE_RESOURCE_QUERY, (void *)&resName, (void *)&refId)
      != Success)
    return NULL;
  
  if (refId < 0) {
    X_IPC_MOD_WARNING( "\nWARNING: Ignoring reservation request.\n");
    X_IPC_MOD_WARNING1( "%s is already reserved by this module.\n", resName);
    ref = CREATE_NULL_REF();
  }
  else {
    msg = x_ipc_msgFind(X_IPC_RESERVE_RESOURCE_QUERY);
    if (msg == NULL) return NULL;
    ref = x_ipcRefCreate(msg, X_IPC_RESERVE_RESOURCE_QUERY, refId);
  }
  
  return ref;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_REF_PTR x_ipcReserveModResource(modName, resName)
 *
 * DESCRIPTION: Reserve a resource by further specifying the module name.
 *
 * INPUTS:
 * char *modName;
 * char *resName;
 *
 * OUTPUTS: X_IPC_REF_PTR
 *
 *****************************************************************************/

X_IPC_REF_PTR x_ipcReserveModResource(const char *modName, const char *resName)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR ref;
  ADD_HND_FORM_TYPE addForm;
  
  addForm.hndName = modName;
  addForm.resName = resName;
  
  refId = 0;
  
  if (x_ipcQuery(X_IPC_RESERVE_MOD_RESOURCE_QUERY, (void *)&addForm,
	       (void *)&refId) != Success)
    return NULL;
  
  if (refId < 0) {
    X_IPC_MOD_WARNING( "\nWARNING: Ignoring reservation request.\n");
    X_IPC_MOD_WARNING2( "%s of module %s is already reserved by this module.\n", 
		  resName, modName);
    ref = CREATE_NULL_REF();
  }
  else {
    msg = x_ipc_msgFind(X_IPC_RESERVE_MOD_RESOURCE_QUERY);
    if (msg == NULL) return NULL;
    ref = x_ipcRefCreate(msg, X_IPC_RESERVE_MOD_RESOURCE_QUERY, refId);
  }
  
  return ref;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcCancelReservation(ref)
 *
 * DESCRIPTION: Cancel a reservation.
 *
 * INPUTS: X_IPC_REF_PTR ref
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcCancelReservation(X_IPC_REF_PTR ref)
{
  if (STREQ(ref->name, X_IPC_RESERVE_RESOURCE_QUERY) ||
      STREQ(ref->name, X_IPC_RESERVE_MOD_RESOURCE_QUERY)) {
    (void)x_ipcInform(X_IPC_CANCEL_RESOURCE_INFORM, (void *)&(ref->refId));
    x_ipcRefFree(ref);
  }
  else {
    X_IPC_MOD_ERROR("ERROR: x_ipcCancelReservation: X_IPC_REF_PTR not a reservation.\n");
  }
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_REF_PTR x_ipcLockResource(resName)
 *
 * DESCRIPTION: Lock a resource to prevent any module from using it.
 *
 * INPUTS: char *resName;
 *
 * OUTPUTS: X_IPC_REF_PTR
 *
 *****************************************************************************/

X_IPC_REF_PTR x_ipcLockResource(const char *resName)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR ref;
  
  if (x_ipcQuery(X_IPC_LOCK_RESOURCE_QUERY, (void *)&resName, (void *)&refId) 
      != Success)
    return NULL;
  
  msg = x_ipc_msgFind(X_IPC_LOCK_RESOURCE_QUERY);
  if (msg == NULL) return NULL;
  
  ref = x_ipcRefCreate(msg, X_IPC_LOCK_RESOURCE_QUERY, refId);
  
  return ref;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_REF_PTR x_ipcLockModResource(modName, resName)
 *
 * DESCRIPTION: Lock a resource - specified with module name.
 *
 * INPUTS: 
 * char *modName;
 * char *resName;
 *
 * OUTPUTS: X_IPC_REF_PTR
 *
 *****************************************************************************/

X_IPC_REF_PTR x_ipcLockModResource(const char *modName, const char *resName)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR ref;
  ADD_HND_FORM_TYPE addForm;
  
  addForm.hndName = modName;
  addForm.resName = resName;
  
  if (x_ipcQuery(X_IPC_LOCK_MOD_RESOURCE_QUERY, 
	       (void *)&addForm, (void *)&refId) != Success)
    return NULL;
  
  msg = x_ipc_msgFind(X_IPC_LOCK_MOD_RESOURCE_QUERY);
  if (msg == NULL) return NULL;
  
  ref = x_ipcRefCreate(msg, X_IPC_LOCK_MOD_RESOURCE_QUERY, refId);
  
  return ref;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcUnlockResource(ref)
 *
 * DESCRIPTION: Cancel a resource lock.
 *
 * INPUTS: X_IPC_REF_PTR ref
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcUnlockResource(X_IPC_REF_PTR ref)
{
  if (STREQ(ref->name, X_IPC_LOCK_RESOURCE_QUERY) ||
      STREQ(ref->name, X_IPC_LOCK_MOD_RESOURCE_QUERY)) {
    (void)x_ipcInform(X_IPC_UNLOCK_RESOURCE_INFORM, (void *)&(ref->refId));
    x_ipcRefFree(ref);
  }
  else {
    X_IPC_MOD_ERROR("ERROR: x_ipcUnlockResource: X_IPC_REF_PTR not a lock.\n");
  }
}

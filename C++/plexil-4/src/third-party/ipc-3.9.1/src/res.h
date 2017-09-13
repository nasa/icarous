/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: resources
 *
 * FILE: res.h
 *
 * ABSTRACT:
 * 
 * Resources
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: res.h,v $
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
 * Revision 1.1  1996/05/09 01:01:56  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:29  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.13  1996/02/06  19:05:10  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.12  1996/01/30  15:04:56  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.11  1996/01/27  21:54:18  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.10  1996/01/10  03:16:28  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.9  1995/05/31  19:36:35  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.8  1995/03/28  01:14:59  rich
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
 * Revision 1.7  1995/01/18  22:42:33  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.6  1994/05/25  17:32:35  reids
 * Added utilities to limit the number of pending messages
 *
 * Revision 1.5  1994/05/17  23:17:26  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.4  1993/12/01  18:04:24  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.3  1993/08/27  07:16:43  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:59  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:47  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:06  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:25:36  fedor
 * Added Logging.
 *
 * 26-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * New implmentation of resources - x_ipc version 5.0.
 *
 * 13-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCres
#define INCres

#define NO_PENDING_LIMIT -1

typedef enum {
  ActiveResource=0, ReservedResource=1, LockedResource=2
#ifdef FORCE_32BIT_ENUM
    , dummyResourceStatus = 0x7FFFFFFF
#endif
} RESOURCE_STATUS_TYPE;

typedef struct _RESOURCE {
  int readSd;
  int writeSd;
  char *name;
  int32 capacity;
  MODULE_PTR module;
  RESOURCE_STATUS_TYPE status;
  struct _LIST *pendingList, *attendingList;
  int32 pendingLimit;
  LIST_PTR msgLimitList;
} RESOURCE_TYPE, *RESOURCE_PTR;

#define RESOURCE_SET_STATUS(stat, resource) (resource->status = stat) 

RESOURCE_PTR resourceCreate(int readSd, int writeSd, 
			    const char *name, int32 capacity);

void resourceDelete(RESOURCE_PTR resource);
int32 resourceSelfQuery(DISPATCH_PTR);
int32 resourceAvailableRes(RESOURCE_PTR resource);
int32 resourceAvailable(DISPATCH_PTR dispatch);
void resourceAttending(DISPATCH_PTR dispatch);
void resourceRemoveAttending(DISPATCH_PTR dispatch);
void resourceRemovePending(DISPATCH_PTR dispatch);
void resourcePending(DISPATCH_PTR dispatch);
void resourceProcessPendingRes(RESOURCE_PTR resource);
void resourceProcessPending(DISPATCH_PTR dispatch);

void resourceInitialize(void);
void purgeResoucePending(void);
void showResourceStatus(void);
void unlockResource(char *name);

#endif /* INCres */

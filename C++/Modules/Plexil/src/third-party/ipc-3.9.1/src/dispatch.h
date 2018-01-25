/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: dispatch
 *
 * FILE: dispatch.h
 *
 * ABSTRACT:
 * 
 * A dispatch include file - needs work.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: dispatch.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:23  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.4  1997/01/27 20:09:20  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.2  1997/01/16 22:16:48  reids
 * Increased DISPATCH_FREE_INC to match DISPATCH_TABLE_SIZE
 *
 * Revision 1.2.2.1  1996/10/18 18:01:31  reids
 * DISPATCH_HND_TYPE fields must match HND_TYPE.
 *
 * Revision 1.2  1996/05/24 16:45:50  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:21  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:17  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.19  1996/02/10  16:49:52  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.18  1996/01/30  15:04:01  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.17  1996/01/27  21:53:19  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.16  1995/07/06  21:16:07  rich
 * Solaris and Linux changes.
 *
 * Revision 1.15  1995/05/31  19:35:23  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.14  1995/04/19  14:28:06  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.13  1995/03/30  15:42:48  rich
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
 * Revision 1.12  1995/03/28  01:14:31  rich
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
 * Revision 1.11  1995/01/18  22:40:17  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.10  1994/05/17  23:15:43  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.9  1994/01/31  18:27:39  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of 
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.8  1993/12/14  17:33:26  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.7  1993/12/01  18:03:20  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.6  1993/11/21  20:17:38  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.5  1993/08/30  23:13:52  fedor
 * Added SUN4 as well as sun4 compile flag.
 * Corrected Top level failure message name display with a define in dispatch.c
 *
 * Revision 1.4  1993/08/27  08:38:29  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.3  1993/08/27  07:14:35  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:17  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:42  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:36  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:34  fedor
 * Added Logging.
 *
 * 26-Jul-91 Reid Simmons, School of Computer Science, CMU
 * Added "refCount" field -- the problem is that a dispatch could be freed
 * when another module still has a refence to it.
 *
 * 29-May-91 Christopher Fedor, School of Computer Science, CMU
 * Dispatch is getting too messy with block com and class data sillyness.
 *
 * 16-Apr-91 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 * 15-Apr-91 Reid Simmons, School of Computer Science, CMU
 * Added tap field.
 *
 *  2-Oct-89 Reid Simmons, School of Computer Science, CMU
 * Moved "Kill" status to task tree nodes.
 *
 * 14-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Added Status slot; added statuses for killing trees
 *
 * 28-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Added TreeNode.
 *
 * 27-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Removed desId - since info is from Hnd. Added TC_ModInfoPtr Org.
 *
 * 24-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Added Local Id, LocId.
 * Changed int MsgId to a Msg and int HndId to Hnd.
 *
 *  5-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Removed MsgRef. Added RefId. ReImplement using IdTable.
 *
 * 20-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCdispatch
#define INCdispatch

/* 16-Apr-91: fedor: the number of dispatch status types used in taskTree.c */

#define DISPATCH_STATUS_TOTAL 11 

#define DISPATCH_FREE_INC DISPATCH_TABLE_SIZE

#define DISPATCH_MSG(dispatch) (dispatch->msg)
#define DISPATCH_MSG_NAME(dispatch) (dispatch->msg->msgData->name)
#define DISPATCH_MSG_DATA(dispatch) (dispatch->msgData)
#define DISPATCH_RES_DATA(dispatch) (dispatch->resData)
#define DISPATCH_PARENT_REF(dispatch) (dispatch->pRef)
#define DISPATCH_HANDLER_CHOOSEN(dispatch) (dispatch->hnd)
#define DISPATCH_SET_STATUS(newStatus, dispatch) (dispatch->status = newStatus)

#define DISPATCH_FROM_ID(id) \
(DISPATCH_PTR)idTableItem(id, GET_S_GLOBAL(dispatchTable))

typedef enum {
  UnallocatedDispatch=0, AllocatedDispatch=1, InactiveDispatch=2, 
  PendingDispatch=3, AttendingDispatch=4, HandledDispatch=5,
  KilledDispatch=6, ReplyDispatch=7, SuccessDispatch=8,
  FailureDispatch=9, CentralPendingDispatch=10
#ifdef FORCE_32BIT_ENUM
    , dummyDispatchStatus = 0x7FFFFFFF
#endif
} DISPATCH_STATUS_TYPE;

typedef struct _DISPATCH *DISPATCH_PTR;

typedef void 
(* RESP_PROC_FN)(DISPATCH_PTR, char *);

typedef void 
(* DISPATCH_HND_FN)(DISPATCH_PTR, const char *);

/* NOTE: DISPATCH_HND_TYPE fields must match HND_TYPE. */
typedef struct _DISPATCH_HND {
  int sd;
  int32 localId;
  MSG_PTR msg;
  DISPATCH_HND_FN hndProc;
  MODULE_PTR hndOrg;
  HND_DATA_PTR hndData;
  struct _LIST *msgList;
  struct _RESOURCE *resource;
  HND_LANGUAGE_ENUM hndLanguage;
#ifdef NMP_IPC
  void *clientData;
#endif
} DISPATCH_HND_TYPE, *DISPATCH_HND_PTR;

typedef struct _DISPATCH {
  int32 locId;
  int32 refId;
  int32 orgId;
  int32 desId;
  int32 pRef;
  
  MSG_PTR msg;
  DISPATCH_HND_PTR hnd;
  struct _RESOURCE *resource;
  
  char *classData;
  X_IPC_MSG_CLASS_TYPE msg_class;
  
  char *respData;
  RESP_PROC_FN respProc;
  
  MODULE_PTR org, des;
  DATA_MSG_PTR msgData, resData;
  
#ifndef NMP_IPC
  struct _TREE_NODE *treeNode;
#endif
  struct _DISPATCH *next;
  
  struct _BLOCK_COM_TYPE *blockCom;
  
  DISPATCH_STATUS_TYPE status;
  struct _TAP_INFO *tapInfo;
  int32 refCount;
} DISPATCH_TYPE;


DISPATCH_PTR dispatchAllocate(void);
void dispatchFree(DISPATCH_PTR dispatch);
void dispatchSetMsgData(DATA_MSG_PTR msgData, DISPATCH_PTR dispatch);
void dispatchSetResData(DATA_MSG_PTR resData, DISPATCH_PTR dispatch);
DISPATCH_PTR dispatchCopy(DISPATCH_PTR dispatch);
void dispatchUpdateAndDisplay(DISPATCH_STATUS_TYPE newStatus,
			      DISPATCH_PTR dispatch);
void deliverDispatch(DISPATCH_PTR dispatch);
void deliverResponse(DISPATCH_PTR dispatch);
X_IPC_RETURN_VALUE_TYPE centralReply(DISPATCH_PTR dispatch, const void *data);
X_IPC_RETURN_VALUE_TYPE centralNullReply(DISPATCH_PTR dispatch);
void centralSuccess(DISPATCH_PTR dispatch);
void centralFailure(DISPATCH_PTR dispatch);
X_IPC_RETURN_VALUE_TYPE centralInform(const char *name, const void *data);
X_IPC_RETURN_VALUE_TYPE centralBroadcast(const char *name, const void *data);
void *dispatchDecodeResponse(DISPATCH_PTR dispatch);
void reserveDispatch(DISPATCH_PTR dispatch);
void releaseDispatch(DISPATCH_PTR dispatch);
void freeDispatchList(DISPATCH_PTR dispatch);

#endif /* INCdispatch */

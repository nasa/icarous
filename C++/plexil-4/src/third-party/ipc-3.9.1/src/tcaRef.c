/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: x_ipcRef
 *
 * FILE: x_ipcRef.c
 *
 * ABSTRACT:
 * 
 * Implements X_IPC_REF_PTR
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: tcaRef.c,v $
 * Revision 2.5  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2002/01/03 20:52:18  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.3  2000/07/27 16:59:12  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.2  2000/07/03 17:03:30  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.7  1997/01/27 20:10:06  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.5  1997/01/11 01:21:28  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.2.4.4.1  1996/12/24 14:41:50  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.2.2.4  1996/12/18 15:13:12  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.3  1996/10/24 15:19:30  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.2.2.2  1996/10/22 18:49:54  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.2.2.1  1996/10/18 18:10:24  reids
 * Better error checking and handling.
 *
 * Revision 1.2  1996/05/26 04:11:49  reids
 * Added function IPC_dataLength -- length of byte array assd with msgInstance
 *
 * Revision 1.1  1996/05/09 01:02:09  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/24 19:07:17  reids
 * Remove compiler warnings for return type.
 *
 * Revision 1.2  1996/03/12 03:20:07  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:55  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.23  1996/03/05  05:05:02  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.22  1996/03/02  03:22:06  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.21  1996/02/10  16:50:47  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.20  1996/02/06  19:05:18  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.19  1995/11/03  03:04:54  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.18  1995/10/29  18:27:20  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.17  1995/10/25  22:49:09  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.16  1995/06/14  03:22:54  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.15  1995/04/19  14:29:12  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.14  1995/04/08  02:06:41  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.13  1995/01/18  22:43:28  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.12  1994/10/25  17:11:06  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.11  1994/05/17  23:18:20  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.10  1994/05/11  01:57:42  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.9  1994/05/05  00:46:32  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.8  1994/04/28  16:17:47  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.7  1994/04/16  19:43:33  rich
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
 * Revision 1.6  1993/12/14  17:35:35  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.5  1993/11/21  20:19:58  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/30  21:55:01  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.3  1993/08/27  07:17:27  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:19:48  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:40  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:50  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:26:24  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 14-Dec-91 Christopher Fedor, School of Computer Science, CMU
 * Addded Rich Goodwin's x_ipcReferenceStatus.
 *
 * 14-Mar-91 Christopher Fedor, School of Computer Science, CMU
 * Added x_ipcReferenceId for lisp interface to tpl constraints.
 *
 * 13-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

void x_ipcRefInitialize(void)
{
  LOCK_CM_MUTEX;
  GET_C_GLOBAL(x_ipcRefFreeList) = x_ipc_listCreate();
  UNLOCK_CM_MUTEX;
}

/* 26-Jul-91: Reid: There was a memory leak here -- sometimes the name is
   newly malloc'd (see primFmtters) and sometimes an old copy is passed in.
   In either case, the name is not freed.  Solution is to always copy the
   string name in x_ipcRefCreate and free it in x_ipcRefFree
   */

X_IPC_REF_PTR x_ipcRefCreate(MSG_PTR msg, const char *name, int32 refId)
{
  X_IPC_REF_PTR x_ipcRef;
  int32 length;
  
  LOCK_CM_MUTEX;
  if (!x_ipc_listLength(GET_C_GLOBAL(x_ipcRefFreeList)))
    x_ipcRef = NEW(X_IPC_REF_TYPE);
  else 
    x_ipcRef = (X_IPC_REF_PTR)x_ipc_listPopItem(GET_C_GLOBAL(x_ipcRefFreeList));
  UNLOCK_CM_MUTEX;

  x_ipcRef->refId = refId;    
  x_ipcRef->msg = msg;
  
  if (!name) {
    x_ipcRef->name = NULL;
  } else {
    char *namePtr;
    length = strlen(name);
    namePtr = (char *)x_ipcMalloc((unsigned)(length+1));
    BCOPY(name, namePtr, length);
    namePtr[length] = '\0';
    x_ipcRef->name = namePtr;
  }
  x_ipcRef->responseSd = NO_FD;
#ifdef NMP_IPC
  x_ipcRef->responded = FALSE;
  x_ipcRef->dataLength = 0;
#endif
  
  return x_ipcRef;
}

void x_ipcRefFree(X_IPC_REF_PTR x_ipcRef)
{
  if (x_ipcRef) {
    x_ipcRef->refId = 0;
    x_ipcRef->msg = NULL;
    
    x_ipcFree((char *)x_ipcRef->name);
    x_ipcRef->name = NULL;
    
    LOCK_CM_MUTEX;
    x_ipc_listInsertItemFirst((char *)x_ipcRef, GET_C_GLOBAL(x_ipcRefFreeList));
    UNLOCK_CM_MUTEX;
  }
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcReferenceRelease(ref)
 *
 * DESCRIPTION: 
 * Frees the reference and 
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/
void x_ipcReferenceRelease(X_IPC_REF_PTR ref)
{
  if (ref) {
    (void)x_ipcInform(X_IPC_REF_RELEASE_INFORM, (void *)&(ref->refId));
    
    x_ipcRefFree(ref);
  }
}

/******************************************************************************
 *
 * FUNCTION: char *x_ipcReferenceName(ref)
 *
 * DESCRIPTION: 
 * Returns the name of the message associated with the reference.
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

const char *x_ipcReferenceName(X_IPC_REF_PTR ref)
{
  if (ref->name == NULL) {
    X_IPC_MOD_ERROR("ERROR: x_ipcReferenceName: invalid x_ipc reference pointer.\n");
    return NULL;
  } else {
    return ref->name;
  }
}


/******************************************************************************
 *
 * FUNCTION: int32 x_ipcReferenceId(ref)
 *
 * DESCRIPTION: 
 * Done primarily for lisp.
 * Returns the msg id of the message associated with the reference.
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

int x_ipcReferenceId(X_IPC_REF_PTR ref)
{
  if (ref)
    return ref->refId;
  else
    return NO_REF;
}


/******************************************************************************
 *
 * FUNCTION: void *x_ipcReferenceData(ref)
 *
 * DESCRIPTION: 
 * Returns a pointer to the data that was sent when the message
 * that is associated with the reference was sent.
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: Pointer to data.
 *
 *****************************************************************************/

void *x_ipcReferenceData(X_IPC_REF_PTR ref)
{
  int32 refId, sd;
  MSG_PTR msg;
  char *msgData;
  X_IPC_REF_PTR waitRef;
  X_IPC_RETURN_VALUE_TYPE returnValue;
  
  msg = x_ipc_msgFind(X_IPC_REF_DATA_QUERY);
  if (msg == NULL) return NULL;
  
  if (!ref->msg) {
    if (!ref->name) {
      /* 17-Jun-91: fedor: start enforcing correct refs */
      X_IPC_MOD_ERROR1("ERROR: x_ipcReferenceData: Badly Formed Reference: %d\n",
		  ref->refId);
      return NULL;
    }
    ref->msg = x_ipc_msgFind(ref->name);
    if (ref->msg == NULL) return NULL;
  }
  
  /* 17-Jun-91: fedor: check if any message form */
  if (!ref->msg->msgData->msgFormat)
    return NULL;
  
  refId = x_ipc_nextSendMessageRef();
  returnValue = x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, 
				  (char *)&ref->refId, (char *)NULL, refId);
  
  if (returnValue != Success) {
    X_IPC_MOD_ERROR("ERROR: x_ipcReferenceData: x_ipc_sendMessage Failed.\n");
    return NULL;
  }
  
  waitRef = x_ipcRefCreate(ref->msg, ref->name, refId);
  
  msgData = (char *)x_ipcMalloc((unsigned)x_ipc_dataStructureSize(ref->msg->msgData->msgFormat));
  
  LOCK_CM_MUTEX;
  sd = GET_C_GLOBAL(serverRead);
  UNLOCK_CM_MUTEX;
  returnValue = x_ipc_waitForReplyFrom(waitRef, msgData, TRUE, WAITFOREVER, sd);
  
  x_ipcRefFree(waitRef);
  
  if (returnValue == NullReply) {
    /* 17-Jun-91: fedor: if NullReply then nothing else was malloced. */
    x_ipcFree(msgData);
    return NULL;
  }
  else
    return msgData;
}

/******************************************************************************
 *
 * FUNCTION:  X_IPC_REF_STATUS_TYPE x_ipcReferenceStatus(ref)
 *
 * DESCRIPTION: 
 * Returns the msg status of the message associated with the reference.
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: X_IPC_REF_STATUS_TYPE
 *
 *****************************************************************************/
#if 0
/* New version: install after Rich finishes testing with the Hero simulator */
X_IPC_REF_STATUS_TYPE x_ipcReferenceStatus(X_IPC_REF_PTR ref)
{
  X_IPC_REF_STATUS_TYPE status;
  
  (void)x_ipcQueryCentral(X_IPC_REF_STATUS_QUERY, (void *)&(ref->refId),
			(void *)&status);
  
  return status;
}
#endif

int x_ipcReferenceStatus(X_IPC_REF_PTR ref)
{
  int status = -1;
  (void)x_ipcQueryCentral(X_IPC_REF_STATUS_QUERY, &(ref->refId), &status);
  return status;
}

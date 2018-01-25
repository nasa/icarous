/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: registration
 *
 * FILE: reg.c
 *
 * ABSTRACT:
 * 
 * Register messages and handlers
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/reg.c,v $ 
 * $Revision: 2.7 $
 * $Date: 2009/05/04 19:03:41 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: reg.c,v $
 * Revision 2.7  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.6  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.5  2002/01/03 20:52:17  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.4  2001/01/31 17:53:46  reids
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.3  2000/07/27 16:59:11  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.2  2000/07/03 17:03:28  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.8  1997/03/07 17:49:51  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.1.2.7  1997/01/27 20:09:57  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.5  1997/01/11 01:21:20  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.4.4.1  1996/12/24 14:41:46  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.1.2.4  1996/12/18 15:13:06  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.3  1996/10/22 18:49:48  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.1.2.2  1996/10/18 18:16:50  reids
 * Fixed freeing of memory for messages and handlers.
 * Better error handling; Support for CLISP.
 *
 * Revision 1.1.2.1  1996/10/14 03:54:45  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.1  1996/05/09 01:01:54  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:19:58  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:25  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.37  1996/08/22  16:35:59  rich
 * Check the return code on x_ipcQueryCentral calls.
 *
 * Revision 1.36  1996/07/27  21:18:49  rich
 * Close log file on a "quit" typed into central.
 * Fixed problem with deleting handlers.  The hash iteration routine works
 * if the hander causes the hash item to be deleted.
 *
 * Revision 1.35  1996/07/26  18:21:26  rich
 * Check to see if moduleList is non-NULL before dereferencing.
 *
 * Revision 1.34  1996/07/25  22:24:32  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.33  1996/06/25  21:04:27  rich
 * Removed extra function.
 *
 * Revision 1.32  1996/06/25  20:51:26  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.31  1996/05/07  16:49:42  rich
 * Changes for clisp.
 *
 * Revision 1.30  1996/03/09  06:13:18  rich
 * Fixed problem where lisp could use the wrong byte order if it had to
 * query for a message format.  Also fixed some memory leaks.
 *
 * Revision 1.29  1996/03/05  05:04:49  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.28  1996/03/02  03:21:53  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.27  1996/02/10  16:50:30  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.26  1996/01/27  21:54:11  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.25  1996/01/05  16:31:36  rich
 * Added windows NT port.
 *
 * Revision 1.24  1995/12/21  19:17:38  reids
 * For safety's sake, copy strings when registering messages, handlers and
 *    when connecting.
 *
 * Revision 1.23  1995/12/17  20:22:01  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.22  1995/10/29  18:27:00  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.21  1995/10/25  22:48:45  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.20  1995/10/17  17:36:44  reids
 * Added a "language" slot to the HND_TYPE data structure, so that the LISP
 *   X_IPC version will know what language to decode the data structure into
 *   (extensible for other languages, as well).
 *
 * Revision 1.19  1995/10/07  19:07:40  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.18  1995/08/06  16:44:11  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.17  1995/07/12  04:55:25  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.16  1995/07/06  21:17:10  rich
 * Solaris and Linux changes.
 *
 * Revision 1.15  1995/06/14  03:22:12  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.14  1995/05/31  19:36:27  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.13  1995/04/19  14:28:50  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.12  1995/01/18  22:42:25  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.11  1994/10/25  17:10:47  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.10  1994/05/29  16:20:35  reids
 * Changes needed to successfully make a VxWorks version
 *
 * Revision 1.9  1994/05/25  04:57:58  rich
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
 * Revision 1.8  1994/05/17  23:17:19  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.7  1994/05/05  00:46:30  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.6  1994/04/28  16:17:07  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.5  1993/12/14  17:34:59  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.4  1993/11/21  20:19:19  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/30  21:54:19  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.2  1993/08/27  07:16:35  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.6  1993/08/23  17:40:13  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.5  1993/07/08  05:39:10  rich
 * Added function prototypes
 *
 * Revision 1.4  1993/07/05  18:11:24  reids
 * Added x_ipcDeregisterHandler function
 *
 * Revision 1.3  1993/06/22  14:00:18  rich
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
 * Revision 1.2  1993/05/27  22:20:10  rich
 * Added automatic logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 30-Jan-91 Christopher Fedor, School of Computer Science, CMU
 * Added fflush(stdout) to printf for module code calls from lisp
 *
 * 28-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Rewritten. Revised to Software Standards.
 *
 *  9-Nov-89 Reid Simmons, School of Computer Science, CMU
 * Broke out the code to pre-register messages as a separate function
 * ("x_ipc_findOrRegisterMessage").
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Changed "printf"s to use logging facilities.
 *
 *  1-Dec-88 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 *****************************************************************************/

#include "globalM.h"

#ifdef   DOS_FILE_NAMES
#include "globalVa.h"
#include "centralM.h"
#include "comServe.h"
#include "primFmtt.h"
#else
#include "globalVar.h"
#include "centralMsg.h"
#include "comServer.h"
#include "primFmttrs.h"
#endif

#include "msgTap.h"

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

#undef x_ipcRegisterHandler
void _x_ipcRegisterHandler(const char *msgName, const char *hndName, 
			 X_IPC_HND_FN hndProc);

#if !(defined(LISP) && defined(CLISP))
void x_ipcRegisterHandler(const char *msgName, const char *hndName,
			X_IPC_HND_FN hndProc);
#else /* LISP && CLISP */
extern lispdispatch(char *hndName, X_IPC_REF_PTR ref, char *data);

void x_ipcRegisterHandler(const char *msgName, const char *hndName);
#endif /* LISP && CLISP */


/******************************************************************************
 *
 * FUNCTION: MSG_PTR x_ipc_msgCreate(msgData)
 *
 * DESCRIPTION: 
 * Ceates a new message using msgData. The newly created message is
 * stored in messageTable hash table stored by message name.  The message
 * is also inserted into the idtable msgIdTable.
 *
 * INPUTS: MSG_DATA_PTR msgData;
 *
 * OUTPUTS: MSG_PTR
 *
 *****************************************************************************/

MSG_PTR x_ipc_msgCreate(MSG_DATA_PTR msgData)
{
  MSG_PTR msg;
  
  msg = NEW(MSG_TYPE);
  msg->direct = FALSE;
  msg->parsedFormats = FALSE;
  msg->msgData = msgData;
  msg->hndList = x_ipc_listCreate();
  msg->tapList = NULL;  
  msg->excepList = NULL;  
  msg->directList = NULL;  
#ifdef NMP_IPC
  msg->priority = DEFAULT_PRIORITY;
  msg->limit    = MAX_INT;
  msg->notifyHandlerChange = FALSE;
#endif
  
  /* 11-Jun-91: fedor: Blah! storing the parse string should 
     not be done here - but didnt want to mess with msgData for now ! */
  msg->msgFormatStr = NULL;
  msg->resFormatStr = NULL;
  
  LOCK_CM_MUTEX;
  ADD_MESSAGE(msgData->name, msg);
  UNLOCK_CM_MUTEX;
  return msg;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_msgFree(char *name, MSG_PTR msg)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static void x_ipc_msgFreeMsg(MSG_PTR msg)
{
  LIST_PTR hndList;

  if (msg) {
    if (msg->direct == -1) return;
    msg->direct = -1; /* Flag that msg has already been freed */

    if ((msg)->hndList != NULL) {
      hndList = (msg)->hndList;
      (msg)->hndList = NULL;
      x_ipc_listFree(&hndList);
    }
    freeDirectList(msg);
    if (msg->msgData) {
      x_ipcFree((char *)msg->msgData->name);
      x_ipc_freeFormatter(&(msg->msgData->msgFormat));
      x_ipc_freeFormatter(&(msg->msgData->resFormat));
      x_ipcFree((char *)msg->msgData);
    }
    x_ipcFree((char *)(msg));
    msg = NULL;
  }
}

void x_ipc_msgFree(char *name, MSG_PTR msg)
{
#ifdef UNUSED_PRAGMA
#pragma unused(name)
#endif
  x_ipc_msgFreeMsg(msg);
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_hndFree(char *name, HND_PTR msg)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS:
 *
 *****************************************************************************/

void x_ipc_hndFree(HND_KEY_PTR key, HND_PTR hnd)
{
#ifdef UNUSED_PRAGMA
#pragma unused(key)
#endif
  x_ipc_hndDelete(hnd);
}


/******************************************************************************
 *
 * FUNCTION: MSG_PTR x_ipc_findOrRegisterMessage(name)
 *
 * DESCRIPTION: 
 * Find the message, if registered.  If not, create a message 
 * record, setting the "class" to a special value to indicate that
 * it is really not yet registered.
 *
 * INPUTS: const char *name;
 *
 * OUTPUTS: MSG_PTR
 *
 *****************************************************************************/

MSG_PTR x_ipc_findOrRegisterMessage(const char *name)
{
  MSG_PTR msg;
  MSG_DATA_PTR msgData;
  
  LOCK_CM_MUTEX;
  msg = GET_MESSAGE(name);
  UNLOCK_CM_MUTEX;
  if (!msg) {
    msgData = NEW(MSG_DATA_TYPE);
    msgData->refId = 0;
    msgData->name = strdup(name);
    msgData->msgFormat = NULL;
    msgData->resFormat = NULL;
    msgData->msg_class = HandlerRegClass;
    
    msg = x_ipc_msgCreate(msgData);
  }
  return msg;
}


/******************************************************************************
 *
 * FUNCTION: HND_PTR x_ipc_selfRegisterHnd(hndData, hndProc)
 *
 * DESCRIPTION: 
 * x_ipc_selfRegisterHnd creates a new handler from hndData and updates the message
 * associated with this handler to include this handler. If no message
 * has been found a new message of class HandlerRegClass is created.
 *
 * The newly created Handler is inserted into the handler idtable hndIdTable
 * and the handler hash table handlerTable. If a handler by the same name
 * is found a warning is issued and the handler information is updated.
 *
 * If hndProc is NOT NULL then refId of hndData is updated to the localId
 * returned by the idtable hndIdTable.
 *
 * The newly created or found handler is returned.
 *
 * INPUTS:
 * HND_DATA_PTR hndData;
 * X_IPC_HND_FN hndProc;
 *
 * OUTPUTS: HND_PTR
 *
 *****************************************************************************/

HND_PTR x_ipc_selfRegisterHnd(int sd, MODULE_PTR hndOrg, 
			HND_DATA_PTR hndData, X_IPC_HND_FN hndProc)
{
  int32 localId;
  MSG_PTR msg;
  HND_PTR hnd;
  HND_KEY_TYPE hndKey;
  
  msg = x_ipc_findOrRegisterMessage(hndData->msgName);
  
  hndKey.num = sd;
  hndKey.str = hndData->hndName;
  
  LOCK_CM_MUTEX;
  hnd = GET_HANDLER(&hndKey);
  UNLOCK_CM_MUTEX;
  if (!hnd) {
    hnd = NEW(HND_TYPE);
    hnd->sd = sd;
    hnd->localId = 0;
    hnd->msg = NULL;
    hnd->hndProc = hndProc;
    hnd->hndOrg = hndOrg;
    hnd->hndData = hndData;
    hnd->msgList = x_ipc_listCreate();
    hnd->resource = NULL;
    hnd->hndLanguage = C_LANGUAGE; /* The default */
#ifdef NMP_IPC
    hnd->clientData = NO_CLIENT_DATA;
    hnd->isRegistered = TRUE;
#endif
    
    LOCK_CM_MUTEX;
    ADD_HANDLER(&hndKey, hnd);
    
    localId = x_ipc_idTableInsert((char *)hnd, GET_C_GLOBAL(hndIdTable));
    UNLOCK_CM_MUTEX;
    
    hnd->localId = localId;
    
    if (hndProc)
      hnd->hndData->refId = localId;
  } else {
    LOCK_M_MUTEX;
    if (!IS_LISP_MODULE() && hndProc != hnd->hndProc) {
      /* 24-Jun-91: fedor: the warning is not meaningful for lisp because each
	 re-register will cause a pointer change - lisp functions are not at
	 static locations like c */
      X_IPC_MOD_WARNING1("\nWARNING: Procedure change ignored for existing handler %s.\n",
			 hnd->hndData->hndName);
    }
    UNLOCK_M_MUTEX;
  }

  /* 3-Sep-90: fedor: NULL forces initial module cache of a message. */
  hnd->msg = NULL;
  
  if (!x_ipc_listMemberItem((char *)hnd, msg->hndList))
    x_ipc_listInsertItem((char *)hnd, msg->hndList); 
  
  if (!x_ipc_listMemberItem((char *)msg, hnd->msgList))
    x_ipc_listInsertItem((char *)msg, hnd->msgList);
  
  return hnd;
}


/******************************************************************************
 *
 * FUNCTION: int32 modHashFunc(modKey)
 *
 * DESCRIPTION: Module data hash function.
 *
 * INPUTS: MODULE_KEY_PTR modKey;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

static int32 x_ipc_removeHndFromMsg(HND_PTR hnd, MSG_PTR msg)
{
  x_ipc_listDeleteItem((char *)hnd, msg->hndList);

  return TRUE;
}

/******************************************************************************
 *
 * FUNCTION: void x_ipc_hndDelete(modKey)
 *
 * DESCRIPTION: Delete a handler from the handler table, and all messages
 *              that use it.
 *
 * INPUTS: HND_PTR hnd;  A pointer to the handler
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void x_ipc_hndDelete(HND_PTR hnd)
{
  HND_KEY_TYPE hndKey;
  LIST_PTR msgList;
  LIST_ELEM_PTR listTmp;
  MODULE_PTR module;
  MSG_PTR msg;
  
  if (hnd) {
    if (hnd->hndLanguage == UNKNOWN_LANGUAGE) return;
    
    hnd->hndLanguage = UNKNOWN_LANGUAGE;
    
    hnd->msg = NULL;
    msgList = hnd->msgList;
    hnd->msgList = NULL; /* to revent recursion of free's */
    x_ipc_listTestDeleteItemAll((LIST_ITER_FN) x_ipc_removeHndFromMsg, 
			  (char *)hnd, msgList);
    
    x_ipc_listFree(&msgList);
    
    LOCK_CM_MUTEX;
    (void)x_ipc_idTableRemove(hnd->localId, GET_C_GLOBAL(hndIdTable));
    /* Need to remove from the module list.   */
    if (GET_M_GLOBAL(moduleList) != NULL) {
      for (listTmp = GET_M_GLOBAL(moduleList)->first; 
	   (listTmp != NULL);
	   listTmp = listTmp->next
	   ) {
	module = (MODULE_PTR)(listTmp->item);
	x_ipc_listDeleteItemAll(hnd,module->hndList);
      }
    }
    if (hnd->hndData) {
      hndKey.num = hnd->sd;
      hndKey.str = hnd->hndData->hndName;
      x_ipc_hashTableRemove((char *)&hndKey, GET_C_GLOBAL(handlerTable));
      msg = GET_MESSAGE(hnd->hndData->msgName);
      if (msg) x_ipc_removeHndFromMsg(hnd, msg);
      x_ipcFree((char *)hnd->hndData->msgName);
      hnd->hndData->msgName = NULL;
      x_ipcFree((char *)hnd->hndData->hndName);
      hnd->hndData->hndName = NULL;
      x_ipcFree((char *)hnd->hndData);
      hnd->hndData = NULL;
    }
    UNLOCK_CM_MUTEX;
    hnd->hndData = NULL;
    
    x_ipcFree((char *)hnd);
  }
}

/******************************************************************************
 *
 * FUNCTION: int32 x_ipc_deregisterHnd(sd, hndData)
 *
 * DESCRIPTION: Deregister the given handler for the given message, for
 *              the given module (as indicated by the socket descriptor sd).
 *
 * INPUTS: int32 sd; Socket descriptor of the module that registere the handler 
 *         HND_DATA_PTR hndData; Name of message and handler pair.
 *
 * OUTPUTS: int32: returns FALSE if the handler is not currently registered.
 *
 * NOTES: Just remove the handler from the message's hndList.  Do not actually
 *        delete it, since other things depend on having the handler around.
 *
 *****************************************************************************/
int32 x_ipc_deregisterHnd(int sd, HND_DATA_PTR hndData)
{
  HND_PTR hnd;
  MSG_PTR msg;
  HND_KEY_TYPE hndKey;
  
  msg = x_ipc_findOrRegisterMessage(hndData->msgName);
  
  hndKey.num = sd;
  hndKey.str = hndData->hndName;
  
  LOCK_CM_MUTEX;
  hnd = (HND_PTR)x_ipc_hashTableFind((char *)&hndKey, GET_C_GLOBAL(handlerTable));
  UNLOCK_CM_MUTEX;
  if (hnd) {
    x_ipc_removeHndFromMsg(hnd, msg);
    x_ipc_listDeleteItem((char *)msg, hnd->msgList);
    hnd->isRegistered = FALSE;
  } 
  return (hnd != NULL);
}

/***************************************************************************/

void x_ipcRegisterMessage(const char *name, X_IPC_MSG_CLASS_TYPE msg_class,
			const char *msgFormat, const char *resFormat)
{
  MSG_REG_DATA_TYPE msgRegData;
  
  /* Copy these -- they need to be saved */
  /* No they don't.  RTG.*/
  msgRegData.name = name;
  msgRegData.msg_class = msg_class;
  msgRegData.msgFormat = msgFormat;
  msgRegData.resFormat = resFormat;
  
  if (msg_class == CommandClass)
    msgRegData.resFormat = "int";
  
  (void)x_ipcInform(X_IPC_REGISTER_MSG_INFORM, (void *)&msgRegData);
}

/*************************************************************************/

void _x_ipcRegisterHandlerL(const char *msgName, const char *hndName, 
			  X_IPC_HND_FN hndProc, HND_LANGUAGE_ENUM hndLanguage)
{
  HND_DATA_PTR hndData;
  HND_PTR hnd;
  
  if (!hndProc) {
    X_IPC_MOD_ERROR1("ERROR: x_ipcRegisterHandler: hndProc for %s is NULL.\n",
		hndName);
    return;
  }
  
  hndData = NEW(HND_DATA_TYPE);
  
  hndData->refId = 0; /* will be updated by selfRegisterHandler */
  /* Copy these -- they need to be saved */
  hndData->msgName = strdup(msgName);
  hndData->hndName = strdup(hndName);
  
  hnd = x_ipc_selfRegisterHnd(0, (MODULE_PTR)NULL, hndData, hndProc);
  hnd->hndLanguage = hndLanguage;
  
  if (x_ipcInform(X_IPC_REGISTER_HND_INFORM, (void *)hndData) == Success) {
    if (hnd->hndData != hndData) {
      x_ipcFree((void *)hndData->msgName);
      x_ipcFree((void *)hndData->hndName);
      x_ipcFree((void *)hndData);
    }
  }
}

/* Also maintain the original function without the language parameter 
 * for compatibility. 
 */

#if !(defined(LISP) && defined(CLISP))
void _x_ipcRegisterHandler(const char *msgName, const char *hndName, 
			 X_IPC_HND_FN hndProc)
{
  x_ipcRegisterHandler(msgName, hndName, hndProc);
}

/* put in a function with the original name so that old code will still 
 * link with the new library.
 * This should be removed in the next release. RTG.
 */
#undef x_ipcRegisterHandler
void x_ipcRegisterHandler(const char *msgName, const char *hndName,
			X_IPC_HND_FN hndProc)
{
  _x_ipcRegisterHandler(msgName, hndName, hndProc);
}

#else /* LISP && CLISP */

/* Clisp needs to have the handler dispatched in the lisp code since it can
 * not register lisp function to call from c at run time.
 */
static void clispDispatch(X_IPC_REF_PTR ref, char *data)
{
  lispdispatch((char *)ref->msg->msgData->name, ref, data);
}

void x_ipcRegisterHandler(const char *msgName, const char *hndName)
{
  _x_ipcRegisterHandlerL(msgName, hndName, clispDispatch,LISP_LANGUAGE);
}
#endif /* LISP && CLISP */

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcDeregisterHandler(msgName, hndName)
 *
 * DESCRIPTION: Deregister the given handler for the given message.
 *
 * INPUTS: char *msgName; The name of the message associated with the handler
 *         char *hndName; The name of the handler
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE: Returns "Failure" if the handler is not
 *          currently registered for the message; otherwise, returns "Success"
 *
 *****************************************************************************/

void x_ipcDeregisterHandler(const char *msgName, const char *hndName)
{
  HND_DATA_TYPE hndData;
  
  hndData.refId = 0; /* Not needed for deregistering handlers */
  /* No need to copy these -- they are not saved */
  hndData.msgName = (char *) msgName;
  hndData.hndName = (char *) hndName;
  
  (void)x_ipc_deregisterHnd(0, &hndData);
  
  (void)x_ipcInform(X_IPC_DEREGISTER_HND_INFORM, (void *)&hndData);
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipcRegisterNamedFormatter(formatterName, formatString)
 *
 * DESCRIPTION:
 *
 * INPUTS: char *formatterName, *formatString;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcRegisterNamedFormatter(const char *formatterName, 
			       const char *formatString)
{ 
  REG_DATA_TYPE regData;
  
  /* No need to copy these -- they are not saved */
  regData.name = formatterName;
  regData.format = (char *) formatString;
  
  (void)x_ipcInform(X_IPC_NAMED_FORM_INFORM, (void *)&regData);
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipcRegisterLengthFormatter(formatter, length)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * char *formatter;
 * int32 length;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcRegisterLengthFormatter(const char *formatter, int32 length)
{
  char s[11];
  
  bzero(s, sizeof(s));
  
  (void)snprintf(s, sizeof(s), "%d", length);
  
  x_ipcRegisterNamedFormatter(formatter, s);
}

/*****************************************************************************
 *
 * FUNCTION: int x_ipcMessageRegistered(msgName)
 *
 * DESCRIPTION: Is the given message registered?  Caches the answer, if
 *              positive (sufficient, since messages cannot be deregistered).
 *
 * INPUTS: const char char *msgName;
 *
 * OUTPUTS: int: 1 if the message has been registered, 0 o/w
 *
 *****************************************************************************/

int x_ipcMessageRegistered(const char *msgName)
{
  return (x_ipc_msgFind2(msgName, (const char *)NULL) != NULL);
}

/*****************************************************************************
 *
 * FUNCTION: int x_ipcMessageHandlerRegistered(msgName)
 *
 * DESCRIPTION: Is a handler for the message registered?  Checks with the 
 *              central server each time, since handlers can be registered
 *              and deregistered at will.
 *
 * INPUTS: const char char *msgName;
 *
 * OUTPUTS: int: 1 if a handler for the message is currently registered, 0 o/w
 *
 *****************************************************************************/

int x_ipcMessageHandlerRegistered(const char *msgName)
{
  MSG_ASK_TYPE ask, receive;
  int retVal=0;

  ask.msgName = msgName;
  ask.hndName = (const char *)NULL;
  receive.msgName = NULL;
  receive.hndName = (const char *)NULL;
  if (x_ipcQuery(X_IPC_HND_INFO_QUERY, (void *)&ask, (void *)&receive) == Success){
    retVal = (receive.hndName != NULL);
    x_ipcFreeReply(X_IPC_HND_INFO_QUERY, (void *)&receive);
  }
  return retVal;
}


/******************************************************************************
 *
 * FUNCTION: const char **x_ipcGetRegisteredMsgs(void)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

char **x_ipcGetRegisteredMsgs(void)
{
  STR_LIST_PTR messages;
  int length,i;
  char **msgs;
  
  messages = x_ipc_strListCreate();
  x_ipcQuery(X_IPC_MESSAGES_QUERY, NULL, messages);
  length = x_ipc_strListLength(messages);
  msgs = (char **)x_ipcMalloc((length + 1) * sizeof(char *));
  i = 0;
  STR_LIST_ITERATE(messages, string, { msgs[i] = (char *)string; i++; });
  x_ipc_strListFree(&messages,FALSE);
  msgs[length] = NULL;
  return msgs;
}


/******************************************************************************
 *
 * FUNCTION: int x_ipcGetMsgInfo(MSG_INFO_PTR info)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcGetMsgInfo(MSG_INFO_PTR info)
{
  if (info == NULL)
    return Failure;

  return x_ipcQuery(X_IPC_MESSAGE_INFO_QUERY, &(info->name), info);
}

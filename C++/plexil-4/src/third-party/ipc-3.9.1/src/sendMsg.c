/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: send message
 *
 * FILE: sendMsg.c
 *
 * ABSTRACT:
 * 
 * Module routines for initializing and using sendMsg/recvMsg pair.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: sendMsg.c,v $
 * Revision 2.9  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.8  2002/01/03 20:52:17  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.7  2001/08/13 13:39:02  reids
 * Added support for access control (using /etc/hosts.allow, /etc/hosts.deny)
 *
 * Revision 2.6  2001/01/12 15:53:46  reids
 * Added IPC_delayResponse to enable responding to a query outside of the
 *   message handler.
 *
 * Revision 2.5  2000/08/14 21:28:34  reids
 * Added support for making under Windows.
 *
 * Revision 2.4  2000/07/27 16:59:11  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.3  2000/07/19 20:56:33  reids
 * Handle broken pipes
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
 * Revision 1.1.2.4  1996/12/18 15:13:09  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.3  1996/10/22 18:49:49  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.1.2.2  1996/10/18 18:17:36  reids
 * Better error checking.
 * Need to duplicate X_IPC_MSG_INFO_QUERY, to avoid freeing static memory.
 *
 * Revision 1.1.2.1  1996/10/14 00:19:36  reids
 * Consolidate and clean up the x_ipc_sendMessage function
 *
 * Revision 1.1  1996/05/09 01:01:59  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:20:01  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:36  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.37  1996/06/25  20:51:35  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.36  1996/03/05  05:04:51  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.35  1996/03/02  03:21:59  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.34  1996/02/06  19:05:12  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.33  1996/01/27  21:54:22  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.32  1995/11/03  03:04:48  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.31  1995/10/29  18:27:05  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.30  1995/10/25  22:48:55  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.29  1995/07/10  16:18:36  rich
 * Interm save.
 *
 * Revision 1.28  1995/06/14  03:22:24  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.27  1995/06/05  23:59:08  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.26  1995/05/31  19:36:46  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.25  1995/04/19  14:28:54  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.24  1995/04/08  02:06:31  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.23  1995/04/07  05:03:31  rich
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
 * Revision 1.22  1995/04/04  19:42:59  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.21  1995/03/30  15:44:04  rich
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
 * Revision 1.20  1995/03/28  01:15:02  rich
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
 * Revision 1.19  1995/03/19  19:39:42  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.18  1995/01/30  16:18:29  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.17  1995/01/25  00:01:41  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.16  1995/01/18  22:42:51  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.15  1994/05/25  04:58:04  rich
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
 * Revision 1.14  1994/05/17  23:17:43  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.13  1994/05/11  01:57:32  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.12  1994/04/28  16:17:21  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.11  1994/04/16  19:43:12  rich
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
 * Revision 1.10  1994/04/04  16:01:34  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.9  1994/01/31  18:28:55  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.8  1993/12/14  17:35:16  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.7  1993/11/21  20:19:34  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.6  1993/10/21  16:14:27  rich
 * Fixed compiler warnings.
 *
 * Revision 1.5  1993/08/30  21:54:28  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.4  1993/08/27  07:16:57  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/06/13  23:28:30  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.2  1993/05/26  23:19:14  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:49  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:20  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:25:47  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 *  7-May-90 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.9 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

/*******************************************************/

int32 x_ipc_nextSendMessageRef(void)
{
  int32 result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(sendMessageRefGlobal);
  GET_C_GLOBAL(sendMessageRefGlobal)++;
  UNLOCK_CM_MUTEX;

  return result;
}

/*******************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipc_sendMessage(X_IPC_REF_PTR ref, MSG_PTR msg,
					  void *msgData, void *classData,
					  int32 preallocatedRefId)
{
  int32 refId, savedRef, i, sd;
  DATA_MSG_PTR msgDataMsg;
  X_IPC_MSG_CLASS_TYPE msg_class;
  CLASS_FORM_PTR classForm;
  CONST_FORMAT_PTR classFormat;
  X_IPC_RETURN_STATUS_TYPE result;
  DIRECT_MSG_HANDLER_PTR direct;
  CONNECTION_PTR connection;
  
  classFormat = NULL;
  
  if (!msg) {
    X_IPC_MOD_ERROR("ERROR: x_ipc_sendMessage: NULL msg.\n");
  }
  
  if (!x_ipc_isValidServerConnection()) {
    X_IPC_MOD_ERROR("ERROR: Must be connected to the server to send a message");
    return Failure;
  }
  
  /* save the message ref to use in case x_ipc_msgFind triggers 
     a recursive call to x_ipc_sendMessage */
  
  LOCK_CM_MUTEX;
  savedRef = (preallocatedRefId != NO_REF ? preallocatedRefId
	      : x_ipc_nextSendMessageRef());
  
  msg_class = msg->msgData->msg_class;
  classForm = GET_CLASS_FORMAT(&msg_class);
  
  if (classForm)
    classFormat = classForm->format;
  
  refId = (ref ? ref->refId : NO_REF);
  
  /* RTG For now, only do direct if no logging. */
  msgDataMsg = x_ipc_dataMsgCreate(GET_C_GLOBAL(parentRefGlobal), -1,
				   (int32)msg_class, refId, savedRef,
				   msg->msgData->msgFormat, msgData,
				   (FORMAT_PTR)classFormat, classData);
  UNLOCK_CM_MUTEX;
  if (msgDataMsg == NULL) {
    X_IPC_MOD_ERROR1("Unable to send message %s, probably a bad format\n",
		msg->msgData->name);
    return Failure;
  }
  if (!msg->direct || !msg->directList || msg->directList->numHandlers == 0) {
    msgDataMsg->intent = msg->msgData->refId;
    LOCK_CM_MUTEX;
    sd = GET_C_GLOBAL(serverWrite);
    UNLOCK_CM_MUTEX;
    result = x_ipc_dataMsgSend(sd, msgDataMsg);
  } else {
    /* Send in reverse order (to match what happens within central) */
    for (i=msg->directList->numHandlers-1, result=StatOK;
	 i>=0 && result == StatOK; i--) {
      direct = &(msg->directList->handlers[i]);
      if (direct->readSd >= 0) {
	msgDataMsg->intent = direct->intent;
	result = x_ipc_dataMsgSend(direct->writeSd, msgDataMsg);
	if (result == StatError && errno == EPIPE) {
	  fprintf(stderr, "Need to close %d\n", direct->writeSd);
	  LOCK_CM_MUTEX;
	  connection = (CONNECTION_PTR)x_ipc_hashTableFind((void *)&direct->writeSd,
							   GET_C_GLOBAL(moduleConnectionTable));
	  UNLOCK_CM_MUTEX;
	  if (connection) {
	    x_ipcHandleClosedConnection(direct->writeSd, connection);
	    result = StatOK;
	  }
	}
      }
    }
  }

  x_ipc_dataMsgFree(msgDataMsg);
  
  if (result != StatOK) 
    return Failure;
  return Success;
}

/*******************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipc_sendResponse(X_IPC_REF_PTR ref, MSG_PTR msg,
				   void *resData,
				   X_IPC_MSG_CLASS_TYPE resClass,
				   void *resClassData,
				   int sd)
{
  int32 intent=-1, refId,msgId=-1;
  CONST_FORMAT_PTR format;
  DATA_MSG_PTR dataMsg;
  CLASS_FORM_PTR classForm;
  CONST_FORMAT_PTR classFormat;
  X_IPC_RETURN_STATUS_TYPE result;
  
#ifdef NMP_IPC
  if (ref->responded == TRUE) {
    /* Already responded -- something is wrong! */
    X_IPC_MOD_ERROR2("x_ipc_sendResponse: Already responed to instance %d (%s)\n",
		ref->refId, ref->msg->msgData->name);
  }
#endif

  classFormat = NULL;
  format = NULL;
  
  if (msg) {
#ifndef NMP_IPC
    intent = msg->msgData->refId;
    format = msg->msgData->resFormat;
#else
    /* The only place X_IPC calls this function with "msg" set is in
       fireDemon.  Since IPC doesn't use that, we should be safe using
       "msg" for another reason */
    format = msg->msgData->msgFormat;
    if (!ref->msg) {
      ref->msg = x_ipc_msgFind(ref->name);
      if (ref->msg == NULL) return MsgUndefined;
    }
    if (ref->msg->direct) {
      intent = QUERY_REPLY_INTENT;
      msgId = ref->refId;
    } else {
      intent = -1;
    }

    /* We have to "fool" the receiver of the response into using this
       message for encoding/decoding, rather than the query message.
       Not pretty, but it works.  Depends on the fact that ReplyClass has
       no class data.  Later, rework central so that it knows how to handle
       this case explicitly. */ 
    { int resClass1 = ExecHndClass;
      /* Format for ExecHndClass is "string" -- used to send the message name */
      LOCK_M_MUTEX;
      classForm = GET_CLASS_FORMAT(&resClass1);
      UNLOCK_M_MUTEX;
      classFormat = classForm->format;
      resClassData = (void *)&(msg->msgData->name);
    }
#endif
  } else {
    if (ref) {
      if (!ref->msg) {
	ref->msg = x_ipc_msgFind(ref->name);
	if (ref->msg == NULL) return MsgUndefined;
      }
      if (resData != NULL)
	format = ref->msg->msgData->resFormat;
      msg = ref->msg;
      if (msg->direct){
	intent = QUERY_REPLY_INTENT;
	msgId = ref->refId;
      } else {
	intent = -1;
      }
    }
  }
  
  LOCK_M_MUTEX;
  classForm = GET_CLASS_FORMAT(&resClass);
  UNLOCK_M_MUTEX;

  if (classForm)
    classFormat = classForm->format;
  
  refId = (ref ? ref->refId : NO_REF);
  LOCK_CM_MUTEX;
  dataMsg = x_ipc_dataMsgCreate(GET_C_GLOBAL(parentRefGlobal), intent, 
			  (int32)resClass,
			  refId, msgId, (FORMAT_PTR)format, resData,
			  (FORMAT_PTR)classFormat, resClassData);
  UNLOCK_CM_MUTEX;

  if (dataMsg == NULL) return Failure;
  result = x_ipc_dataMsgSend(sd, dataMsg);

#ifdef NMP_IPC
  if (result == StatOK) {
    if (ref->responded == -1) {
      /* Happening outside of the handler -- nuke the reference */
      x_ipcRefFree(ref);
    } else
      ref->responded = TRUE;
  }
#endif
  
  x_ipc_dataMsgFree(dataMsg);
  
  return (result != StatOK ? Failure : Success);
}

/***************/

static CONST_FORMAT_PTR x_ipc_formatterRecv(int sd)
{
  DATA_MSG_PTR dataMsg = NULL;
  CONST_FORMAT_PTR *pointerToFormat;
  CONST_FORMAT_PTR format;
  NAMED_FORMAT_PTR formatFormat;
  
  LOCK_M_MUTEX;
  formatFormat = (NAMED_FORMAT_PTR)
    x_ipc_hashTableFind("format", GET_M_GLOBAL(formatNamesTable));
  UNLOCK_M_MUTEX;
  if (!formatFormat) {
    X_IPC_MOD_ERROR("ERROR: x_ipc_formatterRecv: no formatFormat");
  }
  
  switch(x_ipc_dataMsgRecv(sd, &dataMsg, 0, NULL, 0)){
  case StatOK:
    pointerToFormat = (CONST_FORMAT_PTR *)
      x_ipc_dataMsgDecodeMsg(formatFormat->format, dataMsg, FALSE);
    x_ipc_dataMsgFree(dataMsg);
    format = *pointerToFormat;
    x_ipcFree((void *)pointerToFormat);
    return(format);
  case StatError:
    X_IPC_MOD_ERROR("ERROR: foramatterRecv: x_ipc_dataMsgRecv: StatError:");
    /*NOTREACHED*/
    break;
  case StatEOF:
    X_IPC_MOD_ERROR("ERROR: foramatterRecv: x_ipc_dataMsgRecv: StatEOF:");
    /*NOTREACHED*/
    break;
  case StatSendEOF:    
  case StatSendError:    
  case StatRecvEOF:    
  case StatRecvError:    
    X_IPC_MOD_ERROR("ERROR: foramatterRecv: x_ipc_dataMsgRecv: Unexpected error \n");
    break;
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR("ERROR: foramatterRecv: x_ipc_dataMsgRecv: UNKNOWN:");
    /*NOTREACHED*/
#endif
  }
  return ( CONST_FORMAT_PTR) NULL;
}

/*******************************************************/

void x_ipc_msgInfoMsgInitialize(void)
{
  int32 refId;
  MSG_DATA_PTR msgData;
  
  LOCK_CM_MUTEX;
  LOCK_IO_MUTEX;
  if (x_ipc_readNBytes(GET_C_GLOBAL(serverRead),
		       (char *)&refId, sizeof(int32)) != StatOK) {
    GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
    GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
    X_IPC_MOD_ERROR("ERROR: Socket connection broken or access denied to central server\n");
    UNLOCK_IO_MUTEX;
    UNLOCK_CM_MUTEX;
    return;
  }

  NET_INT_TO_INT(refId);
  
  msgData = NEW(MSG_DATA_TYPE);
  msgData->refId = refId;
  msgData->name = strdup(X_IPC_MSG_INFO_QUERY);
  msgData->msg_class = QueryClass;
  
  msgData->msgFormat = (FORMAT_PTR)x_ipc_formatterRecv(GET_C_GLOBAL(serverRead));
  msgData->resFormat = (FORMAT_PTR)x_ipc_formatterRecv(GET_C_GLOBAL(serverRead));
  UNLOCK_IO_MUTEX;
  UNLOCK_CM_MUTEX;
  
  (void)x_ipc_msgCreate(msgData);
}

/*******************************************************/

void x_ipc_classModInitialize(void)
{
  X_IPC_MSG_CLASS_TYPE msg_class;
  CLASS_FORM_PTR classForm=NULL;
  
  /* 31-Aug-90: fedor: more and more difficult to do 
     query msg ref with a QueryClass format of "int" */
  
  msg_class = GoalClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
			(void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = CommandClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = ExceptionClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = PointMonitorClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = PollingMonitorClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = DemonMonitorClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = ExecHndClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = FailureClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = FireDemonClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
  
  msg_class = MultiQueryClass;
  if (x_ipcQueryCentral(X_IPC_CLASS_INFO_QUERY, (void *)&msg_class,
		      (void *)&classForm) == Success) {
    LOCK_M_MUTEX;
    ADD_CLASS_FORMAT(&msg_class, classForm);
    UNLOCK_M_MUTEX;
  }
}

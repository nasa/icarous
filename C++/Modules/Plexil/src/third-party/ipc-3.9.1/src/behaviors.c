/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: behaviors
 *
 * FILE: behaviors.c
 *
 * ABSTRACT:
 * 
 * x_ipc behavior level.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: behaviors.c,v $
 * Revision 2.12  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.11  2003/07/23 20:25:03  reids
 * Fixed bug in handling message that is received but already unsubscribed.
 * Removed compiler warning.
 *
 * Revision 2.10  2003/04/14 15:30:43  reids
 * Fixed bug in use of IPC_delayResponse
 *
 * Revision 2.9  2002/06/25 16:44:58  reids
 * Fixed the way memory is freed when responses are handled;
 *   Allowed me to remove "responseIssuedGlobal".
 *
 * Revision 2.8  2002/01/03 20:52:10  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.7  2001/02/28 03:13:19  trey
 * added explicit cast to avoid warning
 *
 * Revision 2.6  2001/01/31 17:53:47  reids
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.5  2001/01/12 15:55:03  reids
 * Improved handling of queries, especially when sender does not expect a reply
 *
 * Revision 2.4  2001/01/10 15:32:49  reids
 * Added the function IPC_subscribeData that automatically unmarshalls
 *   the data before invoking the handler.
 *
 * Revision 2.3  2000/07/27 16:59:08  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.2  2000/07/03 17:03:21  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.11  1997/03/07 17:49:24  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.3.2.10  1997/01/27 20:09:04  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.3.2.8  1997/01/11 01:20:41  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.3.2.7.4.1  1996/12/24 14:41:27  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.3.2.7  1996/12/18 15:08:50  reids
 * Changed logging code to remove VxWorks dependence on varargs
 * Fixed bug in Lispworks/VxWorks handling of messages, where a message
 *   could be "stranded" if it arrived during a blocking query
 *
 * Revision 1.3.2.6  1996/10/29 14:50:45  reids
 * Cache formatter attributes when receiving new message info
 * Make "byteOrder" and "alignment" vars available to C, not just LISP.
 *
 * Revision 1.3.2.5  1996/10/22 18:49:22  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.3.2.4  1996/10/18 17:57:50  reids
 * Better error checking.
 *
 * Revision 1.3.2.3  1996/10/14 15:01:12  reids
 * Changes to fix freeing of formatters, and to handle errors more cleanly.
 *
 * Revision 1.3.2.2  1996/10/08 14:23:31  reids
 * Changes for IPC operating under Lispworks on the PPC.  Mainly changes
 * (LISPWORKS_FFI_HACK) due to the fact that Lispworks on the PPC is currently
 * missing the foreign-callable function.
 *
 * Revision 1.3.2.1  1996/10/02 20:56:14  reids
 * Changes to support LISPWORKS.
 * Fixed the procedure for dealing with named formatters.
 *
 * Revision 1.3  1996/05/26 04:11:46  reids
 * Added function IPC_dataLength -- length of byte array assd with msgInstance
 *
 * Revision 1.2  1996/05/09 16:45:37  reids
 * Still another fix for sending NULL data via IPC
 *
 * Revision 1.1  1996/05/09 01:01:09  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/04/01 02:36:52  reids
 * Needed to make x_ipc_queryNotifySend globally accessible for IPC
 *
 * Revision 1.3  1996/03/19 03:38:32  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:27  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:30:52  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.52  1996/06/25  20:50:00  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.51  1996/05/09  18:30:16  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.50  1996/03/15  21:13:07  reids
 * Added x_ipcQueryNotify and plugged a memory leak -- ref was not being
 *   freed when inform/broadcast handler completed.
 *
 * Revision 1.49  1996/03/09  06:12:57  rich
 * Fixed problem where lisp could use the wrong byte order if it had to
 * query for a message format.  Also fixed some memory leaks.
 *
 * Revision 1.48  1996/03/05  05:04:11  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.47  1996/03/02  03:21:15  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.46  1996/02/21  18:29:58  rich
 * Created single event loop.
 *
 * Revision 1.45  1996/02/12  17:41:49  rich
 * Handle direct connection disconnect/reconnect.
 *
 * Revision 1.44  1996/02/10  16:49:25  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.43  1996/02/06  19:04:05  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.42  1996/01/27  21:52:49  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.41  1995/11/13  21:23:01  rich
 * Fixed x_ipcWaitForExecutionWithConstraints.
 *
 * Revision 1.40  1995/11/03  03:04:11  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.39  1995/10/29  18:26:23  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.38  1995/10/25  22:47:50  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.37  1995/10/17  17:36:41  reids
 * Added a "language" slot to the HND_TYPE data structure, so that the LISP
 *   X_IPC version will know what language to decode the data structure into
 *   (extensible for other languages, as well).
 *
 * Revision 1.36  1995/10/07  19:06:56  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.35  1995/08/05  18:13:08  rich
 * Fixed problem with x_ipc_writeNBuffers on partial writes.
 * Added "sharedBuffers" flag to the dataMsg structure, rather than
 * checking to see if the dataStruct pointer and the message data pointer
 * are the same.  This allows central to clear the dataStruc pointer so
 * that messages don't try to access old data structures that might have
 * changed since the  message was created.
 *
 * Revision 1.34  1995/08/05  17:16:11  reids
 * Several important bug fixes:
 *   a) Found a memory leak in the tms (when nodes are unasserted)
 *   b) Fixed a problem with direct connections that would cause X_IPC to crash
 *      when a command or goal message was sent from an inform or a query.
 *      As part of that fix, all command and goal messages that are sent from
 *      informs or queries are now added to the root node of the task tree.
 *
 * Revision 1.33  1995/07/24  15:39:17  reids
 * Small change to x_ipc_msgFind2 to take out extraneous warning.
 *
 * Revision 1.32  1995/07/19  14:25:45  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.31  1995/07/12  04:53:51  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.30  1995/07/10  16:16:42  rich
 * Interm save.
 *
 * Revision 1.29  1995/06/14  03:21:22  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.28  1995/06/05  23:58:30  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.27  1995/05/31  19:34:56  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.26  1995/04/19  14:27:43  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.25  1995/04/08  02:06:14  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.24  1995/04/07  05:02:42  rich
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
 * Revision 1.23  1995/04/04  19:41:45  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.22  1995/03/28  01:14:13  rich
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
 * Revision 1.21  1995/03/19  19:39:19  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.20  1995/01/25  00:00:48  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.19  1995/01/18  22:39:35  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.18  1994/11/02  21:33:55  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.17  1994/10/25  17:09:34  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.16  1994/07/05  16:05:34  reids
 * Fixed a bug in which exception messages were incorrectly related to task
 * tree nodes if distributed responses was enabled.
 *
 * Revision 1.15  1994/05/17  23:15:06  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.14  1994/05/05  00:45:54  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.13  1994/04/28  16:15:15  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.12  1994/04/16  19:41:32  rich
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
 * Revision 1.11  1993/12/14  17:32:42  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.10  1993/11/21  20:17:06  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.9  1993/10/21  16:13:27  rich
 * Fixed compiler warnings.
 *
 * Revision 1.8  1993/10/20  20:28:49  rich
 * Fixed a bug in findMsg2 where it was losing a self registered message
 * when it did the query to find the handler.
 *
 * Revision 1.7  1993/10/20  19:00:21  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.6  1993/10/06  18:07:16  reids
 * Fixed two bugs: x_ipcTplConstrain was not externed correctly and
 * -D option was not being incorporated correctly in compiling
 *
 * Revision 1.5  1993/08/30  21:52:59  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.4  1993/08/27  07:14:02  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/06/13  23:27:58  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.2  1993/05/26  23:16:43  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:52  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:07  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.3  1993/05/19  17:23:06  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 *  2-Jul-91 Reid Simmons, School of Computer Science, CMU
 * Hacked memory management -- closed some holes.
 *
 * 27-May-91 Christopher Fedor, School of Computer Science, CMU
 * Changed x_ipc_msgFind to pass back hndName to cache msgInfo from the msgInfoHnd
 * instead of from deliverDispatch to avoid timing errors.
 *
 * 30-Jan-91 Christopher Fedor, School of Computer Science, CMU
 * Added fflush(stdout) to printf for module code calls from lisp
 *
 *  3-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * Removed logging and changed call from x_ipcError to x_ipcModError.
 *
 *  3-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * Moved module monitor code from behaviors.c to monModule.c
 *
 *  3-Sep-90 Christopher Fedor, School of Computer Science, CMU
 * Replaced msgQuery and FindMessage with x_ipc_msgFind.
 * Moved msg.c comments to behaviors and removed msg.c
 * Moved msgQeury and FindMessage to behaviors.c. Updated message
 * lookup to handle HandlerRegisteredClass and multiple messages for
 * a single handler.
 *
 * 16-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * moved Message_Class_Name from com.c to behaviors.c
 *
 *  7-Apr-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 *  1-Dec-89 Long-Ji Lin, School of Computer Science, CMU
 * Made variable "responseIssuedGlobal" global.
 *
 * 27-Nov-89 Reid Simmons, School of Computer Science, CMU
 * Added non-blocking query pairs -- x_ipcQuerySend and x_ipcQueryReceive.
 *
 * 17-aug-89 Reid Simmons, School of Computer Science, CMU
 * Moved error message from FindMessage to MsgQuery, so that MsgQuery can
 * be called to check whether a message has been registered.
 * Added exception handlers.
 *
 * 14-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Moved "CreateReference" to tasktree.c
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Added checks to ensure that routines are called with the 
 * correct message class.
 *
 * 28-Jun-89 Reid Simmons, School of Computer Science, CMU
 * Added check to see if response (Reply, Success, Failure) is sent by
 * handlers.
 *
 *  9-May-89 Christopher Fedor, School of Computer Science, CMU
 * Added change to SelfRegisterMsg Calls. Probably broke modularization.
 *
 *  3-May-89 Christopher Fedor, School of Computer Science, CMU
 * Fixed Failure to send the descriptive string.
 * Changed Failure to be called at top level with NULL as a message ref.
 * Changed route_module.c to behaviors.c
 *
 *  1-May-89 Reid Simmons, School of Computer Science, CMU
 * Added code for sending temporal constraints with goal messages.  
 * Re-modularized.
 *
 * 28-Apr-89 Reid Simmons, School of Computer Science, CMU
 * Added code for sending the parent reference (dispatch) with messages.
 *
 *  7-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * Moved user level routines to route_module.c Created ClassQuery, MsgQuery.
 *
 *  6-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * created behaviors.
 * 
 *  1-Dec-88 Christopher Fedor, School of Computer Science, CMU
 * created message cache.
 *
 * $Revision: 2.12 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"
#ifdef NMP_IPC
#ifdef DOS_FILE_NAMES
#include "parseFmt.h"
#else
#include "parseFmttrs.h"
#endif
#endif


/******************************************************************************
 *
 * FUNCTION: MSG_PTR x_ipc_msgHndFind(name, hndName)
 *           MSG_PTR x_ipc_msgFind(name)
 *
 * DESCRIPTION: 
 * Performs a message lookup for a message of name. If the message is not
 * found the central server is queried and the information is cached.
 * If the message is still not found or the information returned from the
 * central server is not complete an error is issued. 
 *
 * INPUTS: 
 * char *name, *hndName;
 *
 * OUTPUTS: MSG_PTR
 *
 *****************************************************************************/

static void x_ipc_relinkMessagesAndHandlers(MSG_PTR msg, MSG_PTR oldMsg)
{
  const HND_TYPE *hnd;
  
  /* Update priority and limits, if needed */
  if (oldMsg->priority > msg->priority) msg->priority = oldMsg->priority;
  if (oldMsg->limit > msg->limit)       msg->limit = oldMsg->limit;

  hnd = (const HND_TYPE *)x_ipc_listFirst(oldMsg->hndList);
  while (hnd) {
    x_ipc_listDeleteItem(oldMsg, hnd->msgList);
    x_ipc_listInsertItem(msg, hnd->msgList);
    x_ipc_listInsertItem(hnd, msg->hndList);
    hnd = (const HND_TYPE *)x_ipc_listNext(oldMsg->hndList);
  }

  /* Need to free oldMsg. */
  if (oldMsg->msgData) {
    if (oldMsg->msgData->name)
      x_ipcFree((void *)(oldMsg->msgData->name));
    if (oldMsg->msgData->msgFormat) {
      x_ipcFree((void *)(oldMsg->msgData->msgFormat));
      oldMsg->msgData->msgFormat = NULL;
    }
    if (oldMsg->msgData->resFormat) {
      x_ipcFree((void *)(oldMsg->msgData->resFormat));
      oldMsg->msgData->resFormat = NULL;
    }
    x_ipcFree((void *)oldMsg->msgData);
  }
  x_ipc_listFree(&(oldMsg->hndList));
  x_ipc_listFree(&(oldMsg->tapList));
  freeDirectList(oldMsg);
  if (oldMsg->msgFormatStr) {
    x_ipcFree((void *)oldMsg->msgFormatStr);
    oldMsg->msgFormatStr = NULL;
  }
  if(oldMsg->resFormatStr) {
    x_ipcFree((void *)oldMsg->resFormatStr);
    oldMsg->resFormatStr = NULL;
  }
  x_ipc_listFree(&(oldMsg->excepList));
  x_ipcFree((void *)(oldMsg));
}

static MSG_PTR x_ipc_msgHndFind(const char *name, const char *hndName)
{
  MSG_PTR msg=NULL, oldMsg=NULL, currentMsg;
  MSG_DATA_PTR msgData=NULL;
  MSG_ASK_TYPE msgAsk;
  int32 byteOrder, sd;
  ALIGNMENT_TYPE alignment;
  
  LOCK_M_MUTEX;
  msg = GET_MESSAGE(name);
  UNLOCK_M_MUTEX;
  
  if (!msg || msg->msgData->msg_class == HandlerRegClass) {
    LOCK_CM_MUTEX;
    sd = GET_C_GLOBAL(serverRead);
    UNLOCK_CM_MUTEX;
    if (sd != CENTRAL_SERVER_ID) {
      oldMsg = msg;
      msgAsk.msgName = name;
      msgAsk.hndName = hndName;
      
      LOCK_M_MUTEX;
      byteOrder = GET_M_GLOBAL(byteOrder);
      alignment = GET_M_GLOBAL(alignment);
      UNLOCK_M_MUTEX;

      if (x_ipcQueryCentral(X_IPC_MSG_INFO_QUERY, (void *)&msgAsk,
			    (void *)&msgData) != Success) {
	return NULL;
      }

      /* Found a screw case -- it is possible if messages come in fast enough
	 (either direct connections or capacity > 1) that the same message will
	 be looked up at the "same time" -- need to detect this (Reid 7/2000) */
      LOCK_M_MUTEX;
      currentMsg = GET_MESSAGE(name);
      UNLOCK_M_MUTEX;
      if (oldMsg && oldMsg != currentMsg) {
	/* This message has already been found and replaced */
	x_ipcFree((char *)msgData->name);	
	x_ipcFree((char *)msgData);
	LOCK_M_MUTEX;
	currentMsg = GET_MESSAGE(name);
	UNLOCK_M_MUTEX;
	return currentMsg;
      }

      LOCK_M_MUTEX;
      GET_M_GLOBAL(byteOrder) = byteOrder;
      GET_M_GLOBAL(alignment) = alignment;
      UNLOCK_M_MUTEX;

      if (!msgData) {
	X_IPC_MOD_ERROR1("ERROR: x_ipc_msgFind: Query for message %s returned NULL.\n",
		    name);
	return NULL;
      }
      
      /* Cache formatter attribtues */
      if (msgData->msgFormat != NULL)
	cacheFormatterAttributes((FORMAT_PTR)msgData->msgFormat);
      if (msgData->resFormat != NULL)
	cacheFormatterAttributes((FORMAT_PTR)msgData->resFormat);

      if (msgData->msg_class != HandlerRegClass) {
	msg = x_ipc_msgCreate(msgData);
	if (msgData->refId <= -1) {
	  msg->direct = TRUE;
	  if (!hndName)
	    x_ipc_establishDirect(msg);
	}
	if (oldMsg) x_ipc_relinkMessagesAndHandlers(msg, oldMsg);
	return msg;
      }
    }
    X_IPC_MOD_ERROR1("ERROR: x_ipc_msgFind: Message %s used before registered.\n", name);
    return NULL;
  } else if (!hndName && msg->direct && !msg->directList) {
    msg->direct = TRUE;
    x_ipc_establishDirect(msg);
  }
  
  return msg;
}

MSG_PTR x_ipc_msgFind(const char *name)
{
  return x_ipc_msgHndFind(name, (const char *)NULL);
}

void x_ipcPreloadMessage(const char *name)
{
  x_ipc_msgHndFind(name, (const char *)NULL);
}

MSG_PTR x_ipc_msgFind2(const char *name, const char *hndName)
{
  MSG_PTR msg, oldMsg, currentMsg;
  MSG_DATA_PTR msgData;
  MSG_ASK_TYPE msgAsk;
  
  LOCK_M_MUTEX;
  msg = GET_MESSAGE(name);
  UNLOCK_M_MUTEX;

  if (!msg || msg->msgData->msg_class == HandlerRegClass) {
    oldMsg = msg;
    msgAsk.msgName = name;
    msgAsk.hndName = hndName;
    
    if (x_ipcQueryCentral(X_IPC_MSG_INFO_QUERY, (void *)&msgAsk,
			(void *)&msgData) != Success) {
      return NULL;
    }
    if (!msgData) {
      return NULL;
    }

    /* Found a screw case -- it is possible if messages come in fast enough
       (either direct connections or capacity > 1) that the same message will be
       looked up at the "same time" -- need to detect this (Reid July 2000) */
    LOCK_M_MUTEX;
    currentMsg = GET_MESSAGE(name);
    UNLOCK_M_MUTEX;
    if (oldMsg && oldMsg != currentMsg) {
      /* This message has already been found and replaced */
      x_ipcFree((char *)msgData->name);	
      x_ipcFree((char *)msgData);
      LOCK_M_MUTEX;
      currentMsg = GET_MESSAGE(name);
      UNLOCK_M_MUTEX;
      return currentMsg;
    }

    if (msgData->msg_class != HandlerRegClass) {
      msg = x_ipc_msgCreate(msgData);
      if (!hndName && msgData->refId <= -1) {
	msg->direct = TRUE;
	x_ipc_establishDirect(msg);
      }
      if (oldMsg) x_ipc_relinkMessagesAndHandlers(msg, oldMsg);
      return msg;
    }
    return NULL;
  }
  return msg;
}


/******************************************************************************
 *
 * FUNCTION: CONST_FORMAT_PTR x_ipc_fmtFind(const char *name)
 *
 * DESCRIPTION:
 * Return the named formatter format.
 *
 * INPUTS: Named formatter name.
 *
 * OUTPUTS: The formatter.
 *
 *****************************************************************************/

CONST_FORMAT_PTR x_ipc_fmtFind(const char *name)
{
  NAMED_FORMAT_PTR format = NULL;
  HASH_TABLE_PTR formatNamesTable;
  int sd;

  LOCK_M_MUTEX;
  formatNamesTable = GET_M_GLOBAL(formatNamesTable);
  UNLOCK_M_MUTEX;
  format = (NAMED_FORMAT_PTR) x_ipc_hashTableFind(name, formatNamesTable);
  if (!format) {
    LOCK_CM_MUTEX;
    sd = GET_C_GLOBAL(serverRead);
    UNLOCK_CM_MUTEX;
    if (sd != CENTRAL_SERVER_ID) {
      format = (NAMED_FORMAT_PTR) x_ipcMalloc(sizeof(NAMED_FORMAT_TYPE));
      bzero((void *)format,sizeof(NAMED_FORMAT_TYPE));
      if (x_ipcQueryCentral(X_IPC_NAMED_FORM_QUERY, (void *)&name,
			  (void *)&(format->format)) != Success){
	x_ipcFree((char *)format);
	return NULL;
      }
      if (!(format->format)) {
	X_IPC_MOD_WARNING1("Unknown named format %s used.\n", name);
	format->format = NEW_FORMATTER();
	format->format->type = BadFormatFMT;
	format->format->formatter.f = NULL;
      }
    } else {
      X_IPC_MOD_WARNING1("Unknown named format %s used.\n", name);
      format->format = NEW_FORMATTER();
      format->format->type = BadFormatFMT;
      format->format->formatter.f = NULL;
    }
    LOCK_M_MUTEX;
    x_ipc_hashTableInsert((void *)name, 1+strlen(name), 
			  (void *)format, GET_M_GLOBAL(formatNamesTable));
    UNLOCK_M_MUTEX;
  }
#ifdef NMP_IPC
  else if (!format->parsed) {
    /* Need to use the named formatter -- parse it now */
    format->format = (FORMAT_PTR)ParseFormatString(format->definition);
    format->parsed = TRUE;
  }
#endif

  return (CONST_FORMAT_PTR) format->format;
}


/******************************************************************************
 *
 * FUNCTION: const char *x_ipc_messageClassName(msg_class)
 *
 * DESCRIPTION:
 * Return the name of the class (a string). Used for display purposes
 *
 * INPUTS: X_IPC_MSG_CLASS_TYPE msg_class;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

const char *x_ipc_messageClassName(X_IPC_MSG_CLASS_TYPE msg_class)
{ 
  switch(msg_class) {
  case QueryClass: 
    return("Query");
  case GoalClass: 
    return("Goal");
  case CommandClass: 
    return("Command");
  case PointMonitorClass: 
    return("PMonitor");
  case DemonMonitorClass: 
  case PollingMonitorClass: 
    return("IMonitor");
  case InformClass: 
    return("Inform");
  case ExceptionClass: 
    return("Exception");
  case BroadcastClass:
    return("Broadcast");
  case MultiQueryClass:
    return("MQuery");

  case UNKNOWN:
  case HandlerRegClass:
  case ExecHndClass:
  case ReplyClass:
  case SuccessClass:
  case FailureClass:
  case FireDemonClass:
    return ("UNKNOWN CLASS");

#ifndef TEST_CASE_COVERAGE
  default: 
    return ("UNKNOWN CLASS");
#endif
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_checkMessageClass(msg, class)
 *
 * DESCRIPTION:
 * Check to make sure that functions, such as Query, are called
 * only with messages of that class.
 *
 * INPUTS:
 * MSG_PTR msg;
 * X_IPC_MSG_CLASS_TYPE msg_class;
 *
 * OUTPUTS: void. Message is logged.
 *
 *****************************************************************************/

void x_ipc_checkMessageClass(MSG_PTR msg, X_IPC_MSG_CLASS_TYPE msg_class)
{ 
  const char *name, *use=NULL;
  
  if (msg->msgData->msg_class != msg_class) {
    name = x_ipc_messageClassName(msg_class);
    
    switch (msg->msgData->msg_class) {
    case QueryClass: 
      use = "x_ipcQuery"; 
      break;
    case GoalClass:
      use = "x_ipcExpandGoal"; 
      break;
    case CommandClass:
      use = "x_ipcExecuteCommand";
      break;
    case PointMonitorClass:
      use = "x_ipcPointMonitor";
      break;
    case InformClass: 
      use = "x_ipcInform"; 
      break;
    case BroadcastClass: 
      use = "x_ipcBroadcast"; 
      break;
    case MultiQueryClass: 
      use = "x_ipcMultiQuery"; 
      break;

    case UNKNOWN:
    case HandlerRegClass:
    case ExecHndClass: 
    case ExceptionClass:
    case PollingMonitorClass:
    case DemonMonitorClass:
    case ReplyClass:
    case SuccessClass:
    case FailureClass:
    case FireDemonClass:
#ifndef TEST_CASE_COVERAGE
    default:
#endif
      X_IPC_MOD_WARNING1("Internal Error:Unhandled default in x_ipc_checkMessageClass %d\n",
		    msg->msgData->msg_class);
      break;
    }
    
    X_IPC_MOD_ERROR3("ERROR: %s is not a %s class message.  Use function '%s' instead\n",
		msg->msgData->name, name, use);
  }
}
#ifdef NMP_IPC

/* Determine whether format is "[type: n]", "{int, <type: n>}", 
   or something else */
FORMAT_CLASS_TYPE ipcFormatClassType (CONST_FORMAT_PTR format)
{
  if (!format) {
    return BadFormatFMT;
  } else if (format->type == FixedArrayFMT) { 
    return FixedArrayFMT;
  } else if (format->type == StructFMT && format->formatter.a[0].i == 3 &&
	     format->formatter.a[1].f->type == PrimitiveFMT &&
	     format->formatter.a[1].f->formatter.i == INT_FMT &&
	     format->formatter.a[2].f->type == VarArrayFMT) {
    return VarArrayFMT;
  } else {
    return BadFormatFMT;
  }
}

char *ipcData (CONST_FORMAT_PTR formatter, char *data)
{
  char *data1;

  if (!formatter) {
    return NULL;
  } else {
    switch (ipcFormatClassType(formatter)) {
    case FixedArrayFMT: return data;
    case VarArrayFMT: 
      data1 = data;
      data = (char *)((IPC_VAR_DATA_PTR)data1)->content;
      x_ipcFree(data1);
      return data; 
    default: 
      X_IPC_MOD_ERROR("ipcData: Passed a general formatter\n"); return data;
      return NULL;
    }
  }
}
#endif /* NMP_IPC */

#ifdef LISPWORKS_FFI_HACK
static void setExecHndState (X_IPC_REF_PTR x_ipcRef, const char* hndName,
			     void *data, void *clientData,
			     CONNECTION_PTR connection, MSG_PTR msg,
			     int tmpParentRef)
{
  LOCK_CM_MUTEX;
  if (GET_C_GLOBAL(execHndState).state != ExecHnd_Idle) {
    X_IPC_MOD_ERROR("IPC for Lispworks on VxWorks cannot handle recursive calls to x_ipc_execHnd\n");
  } else {
    GET_C_GLOBAL(execHndState).state = ExecHnd_Pending;
    GET_C_GLOBAL(execHndState).x_ipcRef = x_ipcRef;
    GET_C_GLOBAL(execHndState).hndName = hndName;
    GET_C_GLOBAL(execHndState).data = data;
    GET_C_GLOBAL(execHndState).clientData = clientData;
    GET_C_GLOBAL(execHndState).connection = connection;
    GET_C_GLOBAL(execHndState).msg = msg;
    GET_C_GLOBAL(execHndState).tmpParentRef = tmpParentRef;
    GET_C_GLOBAL(execHndState).tmpResponse = tmpResponse;
  }
  UNLOCK_CM_MUTEX;
}
#endif /* LISPWORKS_FFI_HACK */

/******************************************************************************
 *
 * FUNCTION: void x_ipc_execHnd(connection, dataMsg)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * CONNECTION_PTR connection;
 * DATA_MSG_PTR dataMsg;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipc_execHnd(CONNECTION_PTR connection, DATA_MSG_PTR dataMsg)
{ 
  MSG_PTR msg;
  HND_PTR hnd;
  char *data=NULL, *name, **pointerToName;
  X_IPC_REF_PTR x_ipcRef;
  X_IPC_MSG_CLASS_TYPE msg_class;
  CLASS_FORM_PTR classForm;
  int tmpParentRef, byteOrder;
  X_IPC_CONTEXT_PTR currentContext;
#ifdef LISPWORKS_FFI_HACK
  BOOLEAN isLisp;
#endif
  
  /* RTG Don't really know what intent is, but trying to use it. */
  if (dataMsg->intent == QUERY_REPLY_INTENT) {
    X_IPC_MOD_WARNING("Warning: Unknown query reply\n");
    return;
  } else {
    LOCK_CM_MUTEX;
    hnd = (HND_PTR)idTableItem(dataMsg->intent, GET_C_GLOBAL(hndIdTable));
    UNLOCK_CM_MUTEX;
  }

  if (dataMsg->classTotal) {
    msg_class = ExecHndClass;
    LOCK_M_MUTEX;
    classForm = GET_CLASS_FORMAT(&msg_class);
    UNLOCK_M_MUTEX;
    if (!classForm) {
      X_IPC_MOD_ERROR("Internal Error: x_ipc_execHnd, missing ExecHndClass class format.");
      return;
    }
    
    pointerToName = (char **)x_ipc_dataMsgDecodeClass(classForm->format, dataMsg);
    name = (char *)(*pointerToName);
    
    msg = x_ipc_msgHndFind(name, hnd->hndData->hndName);
    x_ipc_classDataFree(msg_class, (char *)pointerToName);
    hnd->msg = msg;
  }
  else
    msg = hnd->msg;
  
  if (!msg) {
    msg = x_ipc_msgHndFind(hnd->hndData->msgName, hnd->hndData->hndName);
    hnd->msg = msg;
    if (msg->msgData->refId > -1) {
      X_IPC_MOD_ERROR("Internal Error: x_ipc_execHnd: Sanity Check Failed!");
      return;
    }
  }
  
  x_ipcRef = x_ipcRefCreate(msg, msg->msgData->name, dataMsg->msgRef);
  x_ipcRef->responseSd = connection->writeSd;
#ifdef NMP_IPC
  x_ipcRef->dataLength = dataMsg->msgTotal;
#endif
  
  /* If the handler is *not* in the message list, it was probably deregistered 
     after central sent it the message */
  if (x_ipc_listMemberItem(hnd, msg->hndList)) {
    data = (char *)x_ipc_decodeDataInLanguage(dataMsg, msg->msgData->msgFormat, 
					hnd->hndLanguage);
    x_ipc_dataMsgFree(dataMsg);
    
    LOCK_CM_MUTEX;
    tmpParentRef = GET_C_GLOBAL(parentRefGlobal);
    GET_C_GLOBAL(parentRefGlobal) = (DIRECT_CONNECTION(connection)
				     ? NO_REF : x_ipcRef->refId);
    UNLOCK_CM_MUTEX;

    currentContext = x_ipcGetContext();
#ifdef NMP_IPC
    if (hnd->clientData == NO_CLIENT_DATA) {
      (*hnd->hndProc)(x_ipcRef, data);
      endExecHandler(x_ipcRef, connection, msg, tmpParentRef);
    } else {
      if (ipcFormatClassType(msg->msgData->msgFormat) == VarArrayFMT) {
	x_ipcRef->dataLength = ((IPC_VAR_DATA_PTR)data)->length;
      }
      data = ipcData(msg->msgData->msgFormat, data);
#ifdef LISPWORKS_FFI_HACK
      LOCK_M_MUTEX;
      isLisp = IS_LISP_MODULE();
      UNLOCK_M_MUTEX;
      if (isLisp) {
	/* LISPWORKS on VxWorks cannot call Lisp functions from C,
	   so need this hack to handle incoming messages */
	setExecHndState(x_ipcRef, hnd->hndData->hndName, data, hnd->clientData,
			connection, msg, tmpParentRef);
      } else
#endif /* LISPWORKS_FFI_HACK */
	{
	  if (hnd->autoUnmarshall && msg->msgData->resFormat) {
	    LOCK_M_MUTEX;
	    byteOrder = GET_M_GLOBAL(byteOrder);
	    UNLOCK_M_MUTEX;
	      /* Skip if the data is simple -- already fully decoded */
	    if (!(byteOrder == BYTE_ORDER && 
		  x_ipc_sameFixedSizeDataBuffer(msg->msgData->resFormat))) {
	      void *data1;
	      IPC_unmarshall(msg->msgData->resFormat, data, &data1);
	      IPC_freeByteArray(data);
	      data = (char *)data1;
	    }
	  }
	  (*((X_IPC_HND_DATA_FN)hnd->hndProc))(x_ipcRef, data, hnd->clientData);
	  x_ipcSetContext(currentContext);
	  endExecHandler(x_ipcRef, connection, msg, tmpParentRef);
	}
    }
#else
    (*hnd->hndProc)(x_ipcRef, data);
    x_ipcSetContext(currentContext);
    endExecHandler(x_ipcRef, connection, msg, tmpParentRef);
#endif
  } else {
    X_IPC_MOD_WARNING2("WARNING: Message '%s' received but not processed: Handler '%s' not registered\n", 
		  msg->msgData->name, hnd->hndData->hndName);
    switch (msg->msgData->msg_class) {
    case QueryClass:
    case MultiQueryClass:
      x_ipcNullReply(x_ipcRef); break;
      
    case InformClass:
    case BroadcastClass: {
      BOOLEAN direct;
      LOCK_CM_MUTEX;
      direct = DIRECT_CONNECTION(connection);
      UNLOCK_CM_MUTEX;
      if (!direct) x_ipcSuccess(x_ipcRef); break;
    }

    case UNKNOWN:
    case HandlerRegClass:
    case ExecHndClass:
    case GoalClass:
    case CommandClass:
    case ExceptionClass:
    case PollingMonitorClass:
    case PointMonitorClass:
    case DemonMonitorClass:
    case ReplyClass:
    case SuccessClass:
    case FailureClass:
    case FireDemonClass:
#ifndef TEST_CASE_COVERAGE
    default:
#endif
      x_ipcFailure(x_ipcRef, "Deregistered Handler", NULL); break;      
    }
    LOCK_M_MUTEX;
    if (!GET_M_GLOBAL(enableDistributedResponses)) {
      x_ipcRefFree(x_ipcRef);
    }
    UNLOCK_M_MUTEX;
  }
}
    
void endExecHandler (X_IPC_REF_PTR x_ipcRef, CONNECTION_PTR connection, 
		     MSG_PTR msg, int tmpParentRef)
{
  int sd = connection->readSd;
  BOOLEAN enableDistributed;

#ifdef LISPWORKS_FFI_HACK
  LOCK_CM_MUTEX;
  if (IS_LISP_MODULE()) {
    if (GET_C_GLOBAL(execHndState).state != ExecHnd_Handling) {
      X_IPC_MOD_ERROR("endExecHandler: Not currently handling a message\n");
      return;
    } else {
      GET_C_GLOBAL(execHndState).state = ExecHnd_Idle;
    }
  }
  UNLOCK_CM_MUTEX;
#endif /* LISPWORKS_FFI_HACK */

  /* If we have disconnected, then just return.  */
  LOCK_CM_MUTEX;
  if (!x_ipc_hashTableFind((void *)&sd, GET_C_GLOBAL(moduleConnectionTable))){
    UNLOCK_CM_MUTEX;
    return;
  }
  /* Send a "success" reponse to inform messages -- new for version 7.
     Gets rid of race conditions that existed in old version (reids) */
  /* Only do if the connection is not direct.   */
  if (ONE_WAY_MSG(msg) && !DIRECT_CONNECTION(connection)
#ifdef NMP_IPC
      /* In the NMP IPC, you can "respond" to a broadcast -- need
	 to ensure that it doesn't respond twice to the same message */
      && (x_ipcRef->responded == FALSE)
#endif
      ) {
    sd = GET_C_GLOBAL(serverWrite);
    UNLOCK_CM_MUTEX;
    (void)x_ipc_sendResponse(x_ipcRef, (MSG_PTR)NULL, NULL, SuccessClass,
			     NULL, sd);
    x_ipcRefFree(x_ipcRef);

  } else {
    enableDistributed = GET_M_GLOBAL(enableDistributedResponses);
    UNLOCK_CM_MUTEX;
    if (!enableDistributed) {
      if (x_ipcRef->responded != TRUE && RESPONSE_EXPECTED(msg)) {
	X_IPC_MOD_WARNING("\nWARNING: No ");
	if (TWO_WAY_MSG(msg)) {
	  X_IPC_MOD_WARNING("Reply");
	} else {
	  X_IPC_MOD_WARNING("Success/Failure");
	}
	X_IPC_MOD_WARNING1(" sent for message %s;\nResource will probably remain locked\n",
			   msg->msgData->name);
      }
    }
    if (x_ipcRef->responded == TRUE) x_ipcRefFree(x_ipcRef);
  }
    
  LOCK_CM_MUTEX;
  GET_C_GLOBAL(parentRefGlobal) = tmpParentRef;
  UNLOCK_CM_MUTEX;
}

/******************************************************************************
 *
 * FUNCTION: BOOLEAN x_ipc_execFdHnd(fd)
 *
 * DESCRIPTION: Find the handler (if any) associated with that file descriptor,
 *              and execute the handler (possibly passing along client data).
 *
 * INPUTS: int fd
 *
 * OUTPUTS: BOOLEAN -- TRUE if a handler was executed for that fd
 *
 *****************************************************************************/

BOOLEAN x_ipc_execFdHnd(int fd)
{
  FD_HND_PTR fdHndData;

  LOCK_M_MUTEX;
  fdHndData = (FD_HND_PTR)x_ipc_hashTableFind((void *)&fd,
					      GET_M_GLOBAL(externalFdTable));
  UNLOCK_M_MUTEX;
  if (fdHndData != NULL && fdHndData->hndProc != NULL) {
    (fdHndData->hndProc)(fd, fdHndData->clientData);
  }
  return (fdHndData != NULL);
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcReply(dispatch, data)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * DISPATCH_PTR dispatch;
 * void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcReply(X_IPC_REF_PTR ref, const void *data)
{
  X_IPC_RETURN_VALUE_TYPE status;
  
  ref->responded = TRUE;

  status = x_ipc_sendResponse(ref, (MSG_PTR)NULL, (char *)data, ReplyClass,
			      (char *)NULL, ref->responseSd);

  LOCK_M_MUTEX;
  if (GET_M_GLOBAL(enableDistributedResponses)) {
    x_ipcRefFree(ref);
  }
  UNLOCK_M_MUTEX;
  
  return status;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcNullReply(ref)
 *
 * DESCRIPTION:
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcNullReply(X_IPC_REF_PTR ref)
{
  return x_ipcReply(ref, (void *)NULL);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcSuccess(ref)
 *
 * DESCRIPTION:
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcSuccess(X_IPC_REF_PTR ref)
{
  X_IPC_RETURN_VALUE_TYPE status;
  int sd;

  LOCK_CM_MUTEX;
  sd = GET_C_GLOBAL(serverWrite);
  UNLOCK_CM_MUTEX;
#ifndef NMP_IPC
  ref->responded = TRUE;
#endif

  status = x_ipc_sendResponse(ref, (MSG_PTR)NULL, (char *)NULL, 
			      SuccessClass, (char *)NULL, sd);
  
  LOCK_M_MUTEX;
  if (GET_M_GLOBAL(enableDistributedResponses)) {
    x_ipcRefFree(ref);
  }
  UNLOCK_M_MUTEX;
  
  return status;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcFailure(ref, description, data)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref;
 * char *description;
 * void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcFailure(X_IPC_REF_PTR ref, const char *description, 
				 const void *data)
{
  MSG_PTR msg;
  int tplConstr, tmpParentRef=0, sd;
  X_IPC_RETURN_VALUE_TYPE status;
  
  LOCK_CM_MUTEX;
  ref->responded = TRUE;

  /* If the response is to a message, but not from within the handler,
     need to force the parent reference to be correct */
  if (GET_M_GLOBAL(enableDistributedResponses) && ref) {
    tmpParentRef = GET_C_GLOBAL(parentRefGlobal);    
    GET_C_GLOBAL(parentRefGlobal) = ref->refId;
  }
  UNLOCK_CM_MUTEX;

  /* 12-May-91: fedor: this double sending of messages is silly */
  msg = x_ipc_msgFind2(description, (const char *)NULL);
  if (msg) {
    if (msg->msgData->msg_class != ExceptionClass) {
      X_IPC_MOD_ERROR1("ERROR: %s is not an exception class message for x_ipcFailure.\n",
		  msg->msgData->name);
      return WrongMsgClass;
    }
    
    tplConstr = NO_TPLCONSTR;
    (void)x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)data, 
			    (char *)&tplConstr, NO_REF);
  }
  
  LOCK_CM_MUTEX;
  sd =  GET_C_GLOBAL(serverWrite);
  UNLOCK_CM_MUTEX;
  status = x_ipc_sendResponse(ref, (MSG_PTR)NULL, (char *)NULL, FailureClass,
			      (char *)&description, sd);
  
  LOCK_CM_MUTEX;
  if (GET_M_GLOBAL(enableDistributedResponses) && ref) {
    GET_C_GLOBAL(parentRefGlobal) = tmpParentRef;
    x_ipcRefFree(ref);
  }
  UNLOCK_CM_MUTEX;
  
  return status;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcQuerySend(name, query, ref)
 *
 * DESCRIPTION:
 * Send a non-blocking query, which doesn't wait for a reply.
 * Must be paired with a corresponding x_ipcQueryReceive, with the same ref.
 *
 * INPUTS:
 * const char *name;
 * void *query;
 * X_IPC_REF_PTR *ref;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/
#ifndef NMP_IPC
X_IPC_RETURN_VALUE_TYPE x_ipcQuerySend(const char *name, 
				   void *query, 
				   X_IPC_REF_PTR *ref)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_RETURN_VALUE_TYPE returnValue;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, QueryClass);
  
  refId = x_ipc_nextSendMessageRef();
  returnValue = x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, 
				  (char *)query, (char *)NULL, refId);
  
  *ref = x_ipcRefCreate(msg, name, refId);
#ifdef LISP
  LOCK_M_MUTEX;
  GET_M_GLOBAL(lispRefSaveGlobal) = *ref;
  UNLOCK_M_MUTEX;
#endif /* LISP */
  
  return returnValue;
}
#endif

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcQueryReceive(ref, reply)
 *
 * DESCRIPTION:
 * ref is a X_IPC_REF_PTR created by x_ipcQuerySend.
 * reply is a pointer to (already allocated) data.
 *
 * Waits for a reply to a x_ipcQuerySend.  The ref passed must be the same as the
 * one gotten from the x_ipcQuerySend function, but the send's and receive's do
 * not have to be in the same order. That is, you can send query1, send query2,
 * receive query2, receive query1.  X_IPC makes sure the right replies get paired
 * with the right receives.
 *
 * INPUTS:
 * X_IPC_REF_PTR ref;
 * void *reply;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcQueryReceive(X_IPC_REF_PTR ref, void *reply)
{
  X_IPC_RETURN_VALUE_TYPE returnValue;
  
  returnValue = x_ipc_waitForReply(ref, (char *)reply);
  
  x_ipcRefFree(ref);
  
  return returnValue;
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcQueryNotify(name, query, 
 *                                                replyHandler, clientData)
 *
 * DESCRIPTION: Send the query and invoke the replyHandler function when
 *              the query is replied to.  The handler function is invoked
 *              with the reply data and the client data.
 *
 * INPUTS:
 * const char *name; The message name
 * void *query;      The query data
 * REPLY_HANDLER_FN replyHandler; The function to invoke on the reply data.
 * void *clientData; Data passed to the handler along with the reply data.
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipc_queryNotifySend (MSG_PTR msg, const char *name,
					       void *query,
					       REPLY_HANDLER_FN replyHandler, 
					       HND_LANGUAGE_ENUM language,
					       void *clientData)
{
  int32 refId;
  X_IPC_RETURN_VALUE_TYPE returnValue;
  QUERY_NOTIFICATION_PTR queryNotif;
  X_IPC_MSG_CLASS_TYPE real_msg_class;

  refId = x_ipc_nextSendMessageRef();
  /* Make system think that this is actually a query (expecting a reply) */
  real_msg_class = msg->msgData->msg_class;
  msg->msgData->msg_class = QueryClass;
  /* For Threaded IPC, need to insert the notification item *before* sending
     out the message, since with threading the response can come back before
     the thread that is running this function gets a chance to run again */
  queryNotif = NEW(QUERY_NOTIFICATION_TYPE);
  queryNotif->ref = x_ipcRefCreate(msg, name, refId);
  queryNotif->handler = replyHandler;
  queryNotif->language = language;
  queryNotif->clientData = clientData;
  LOCK_CM_MUTEX;
  x_ipc_listInsertItem((void *)queryNotif, GET_C_GLOBAL(queryNotificationList));
  UNLOCK_CM_MUTEX;

  returnValue = x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)query,
				  NULL, refId);
  msg->msgData->msg_class = real_msg_class;
  if (returnValue != Success) {
    LOCK_CM_MUTEX;
    x_ipc_listDeleteItem((void *)queryNotif,
			 GET_C_GLOBAL(queryNotificationList));
    UNLOCK_CM_MUTEX;
    x_ipcFree((char *)queryNotif);
  }
  return returnValue;
}

#ifndef NMP_IPC
X_IPC_RETURN_VALUE_TYPE _x_ipcQueryNotify(const char *name, void *query, 
				      REPLY_HANDLER_FN replyHandler, 
				      HND_LANGUAGE_ENUM language,
				      void *clientData)
{
  MSG_PTR msg;
 
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, QueryClass);
  return x_ipc_queryNotifySend(msg, name, query, replyHandler, language, clientData);
}

X_IPC_RETURN_VALUE_TYPE x_ipcQueryNotify(const char *name, void *query, 
				     REPLY_HANDLER_FN replyHandler, 
				     void *clientData)
{
  return _x_ipcQueryNotify(name, query, replyHandler, C_LANGUAGE, clientData);
}
#endif

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcQuery(name, query, reply)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *query, *reply;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES
 * 3-May-89: fedor:
 * ReturnValue always returns Success from x_ipc_sendMessage - otherwise
 * x_ipc_sendMessage will exit. A hint of continued error handling problems.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcQueryFd(const char *name,
				 void *queryData,
				 void *replyData,
				 int fd)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR ref;
  X_IPC_RETURN_VALUE_TYPE returnValue;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, QueryClass);
  
  refId = x_ipc_nextSendMessageRef();
  returnValue = x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, 
				  (char *)queryData, (char *)NULL, refId);
  
  ref = x_ipcRefCreate(msg, name, refId);
  
  if (returnValue == Success) {
    returnValue = x_ipc_waitForReplyFrom(ref, (char *)replyData,
				   FALSE, WAITFOREVER, fd);
  }
  x_ipcRefFree(ref);
  
  return returnValue;
}

X_IPC_RETURN_VALUE_TYPE x_ipcQueryCentral(const char *name,
				      void *queryData,
				      void *replyData)
{
  int sd;
  if (!x_ipc_isValidServerConnection()) return Failure;
  LOCK_CM_MUTEX;
  sd = GET_C_GLOBAL(serverRead);
  UNLOCK_CM_MUTEX;
  return x_ipcQueryFd(name, queryData, replyData, sd);
}

X_IPC_RETURN_VALUE_TYPE x_ipcQuery(const char *name,
			       void *queryData,
			       void *replyData)
{
  return x_ipcQueryFd(name,queryData,replyData,NO_FD);
}


#ifndef NMP_IPC
/******************************************************************************
 *
 * FUNCTION: int testInconsistent(tplConstraints)
 *
 * DESCRIPTION: Check if all bits of the inconsistent constraint are set.
 *
 * INPUTS: int tplConstraints;
 *
 * OUTPUTS: int
 *
 * NOTES:
 * Should this be moved to tplConstr?
 *
 *****************************************************************************/

int32 testInconsistent(int32 tplConstraints)
{
  int index;
  
  LOCK_M_MUTEX;
  for (index=0; GET_M_GLOBAL(inconsistentConstraintsGlobal)[index] != 0;
       index++)
    if ((GET_M_GLOBAL(inconsistentConstraintsGlobal)[index] & 
	 tplConstraints) == 
	GET_M_GLOBAL(inconsistentConstraintsGlobal)[index]) {
      UNLOCK_M_MUTEX;
      return TRUE;
    }
  UNLOCK_M_MUTEX;
  return FALSE;
}



/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcExpandGoalWithConstraints(ref, name, 
 *                              data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref;
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcExpandGoalWithConstraints(X_IPC_REF_PTR ref, 
						   const char *name,
						   const void *data,
						   int32 tplConstr)
{ 
  MSG_PTR msg;
  
  if (testInconsistent(tplConstr)) {
    X_IPC_MOD_ERROR1("ERROR: Inconsistent temporal constraints for goal %s\n", name);
    return Failure;
  }
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, GoalClass);
  return x_ipc_sendMessage(ref, msg, (char *)data, (char *)&tplConstr, NO_REF);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcExpandGoal(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcExpandGoal(const char *name, const void *data)
{ 
  MSG_PTR msg;
  int tplConstr;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, GoalClass);
  
  tplConstr = SEQ_ACH;
  
  return x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)data,
			   (char *)&tplConstr, NO_REF);
}
#endif /* NMP_IPC */

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcInform(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcInform(const char *name, const void *data)
{ 
  MSG_PTR msg;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return Failure;
  x_ipc_checkMessageClass(msg, InformClass);
  
  return x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)data,
			   (char *)NULL, NO_REF);
}

#ifndef NMP_IPC
/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcAddConstraint(name, data)
 *
 * DESCRIPTION: THIS FUNCTION SHOULD EVENTUALLY BE REPLACED, AND "x_ipcInform"
 *              USED EXCLUSIVELY INSTEAD.
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcAddConstraint(const char *name, const void *data)
{ 
  return x_ipcInform(name, data);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcExecuteCommand(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcExecuteCommand(const char *name, const void *data)
{ 
  MSG_PTR msg;
  BLOCK_COM_TYPE blockCom;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL)
    return MsgUndefined;
  
  x_ipc_checkMessageClass(msg, CommandClass);
  
  blockCom.waitFlag = 0;
  blockCom.tplConstr = SEQ_ACH;
  
  return x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)data,
			   (char *)&blockCom, NO_REF);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcExecuteCommandWithConstraints(ref, name, 
 *                                  data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref; 
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcExecuteCommandWithConstraints(X_IPC_REF_PTR ref, 
						       const char *name,
						       const void *data, 
						       int32 tplConstr)
{ 
  MSG_PTR msg;
  BLOCK_COM_TYPE blockCom;
  
  if (testInconsistent(tplConstr)) {
    X_IPC_MOD_ERROR1("ERROR: Inconsistent temporal constraints for command %s\n", name);
    return Failure;
  }
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, CommandClass);
  
  blockCom.waitFlag = 0;
  blockCom.tplConstr = tplConstr;
  return x_ipc_sendMessage(ref, msg, (char *)data, (char *)&blockCom, NO_REF);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcWaitForCommand(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES:
 * 14-Aug-91: fedor: should check return valuse from 
 * x_ipc_sendMessage and x_ipc_waitForReply.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcWaitForCommand(const char *name, const void *data)
{

  return x_ipcWaitForCommandWithConstraints((X_IPC_REF_PTR)NULL,name,data,SEQ_ACH);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcWaitForCommandWithConstraints(ref, name, 
 *                                  data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref; 
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES:
 * 14-Aug-91: fedor: should check return valuse from 
 * x_ipc_sendMessage and x_ipc_waitForReply.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcWaitForCommandWithConstraints(X_IPC_REF_PTR ref, 
						       const char *name,
						       const void *data, 
						       int32 tplConstr)
{ 
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR x_ipcRef;
  BLOCK_COM_TYPE blockCom;
  X_IPC_MSG_CLASS_TYPE replyClass;
  
  if (testInconsistent(tplConstr)) {
    X_IPC_MOD_ERROR1("ERROR: Inconsistent temporal constraints for command %s\n", name);
    return Failure;
  }
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, CommandClass);
  
  blockCom.waitFlag = 1;
  blockCom.tplConstr = tplConstr;
  
  refId = x_ipc_nextSendMessageRef();
  if (x_ipc_sendMessage(ref, msg, (char *)data, (char *)&blockCom, refId)
      != Success){
    return Failure;
  }
  
  x_ipcRef = x_ipcRefCreate(msg, name, refId);
  
  (void)x_ipc_waitForReply(x_ipcRef, (char *)&replyClass);
  
  x_ipcRefFree(x_ipcRef);
  
  if (replyClass == SuccessClass)
    return Success;
  else
    return Failure;
}
#endif /* NMP_IPC */

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcBroadcast(name, data)
 *
 * DESCRIPTION: Broadcast a one-way ("inform"-type) message to *all* modules
 *              that registered a handler for that particular message.
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcBroadcast(const char *name, const void *data)
{ 
  MSG_PTR msg;
  STR_LIST_PTR broadcasts;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, BroadcastClass);
  
  LOCK_CM_MUTEX;
  broadcasts = GET_C_GLOBAL(broadcastMsgs);
  UNLOCK_CM_MUTEX;
  if ((broadcasts == NULL) || (x_ipc_strListMemberItem(name, broadcasts)))
    return x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)data, 
			     (char *)NULL, NO_REF);
  else
    return Success;
}


#ifndef NMP_IPC
/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcMultiQuery(name, query, max, refPtr)
 *
 * DESCRIPTION:
 * Send a "MultiQuery" to all handlers that can handle the message.
 * "max" is the maximum number of responses requested.
 * "refPtr" is set to an identifier to be used with a corresponding
 *  x_ipcMultiReceive.
 *
 * INPUTS: const char *name;
 *         void *query;
 *         int max;
 *         X_IPC_REF_PTR *refPtr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTE: "refPtr" is a *pointer* to a X_IPC_REF_PTR.
 *       Although one can set the maximum number of responses to be received,
 *       it is not guaranteed that that number will be received by
 *       x_ipcMultiReceive:
 *       the number of responses is MIN("max", number of registered handlers).
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcMultiQuery(const char *name, void *query, int32 max,
				    X_IPC_REF_PTR *refPtr)
{
  int32 refId;
  MSG_PTR msg;
  MULTI_QUERY_CLASS_TYPE multiQueryClassData;
  X_IPC_RETURN_VALUE_TYPE returnValue;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, MultiQueryClass);
  
  refId = x_ipc_nextSendMessageRef();
  multiQueryClassData.num = 0;
  multiQueryClassData.max = max;
  returnValue = x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, (char *)query,
				  (char *)&multiQueryClassData, refId);
  
  *refPtr = x_ipcRefCreate(msg, name, refId);
#ifdef LISP
  LOCK_CM_MUTEX;
  GET_M_GLOBAL(lispRefSaveGlobal) = *refPtr;
  UNLOCK_CM_MUTEX;
#endif /* LISP */
  
  return returnValue;
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcMultiReceive(ref, reply, timeout)
 *
 * DESCRIPTION: ref is a X_IPC_REF_PTR created by x_ipcMultiQuery.
 *              reply is a pointer to (already allocated) data.
 *
 * Waits for replies to a x_ipcMultiQuery.  The ref passed must be the same as
 *    the one gotten from the x_ipcMultiQuery function.
 * "timeout" is number of seconds to wait for a reply before returning.  If no
 *    message is received in that time, the return value is "TimeOut".
 *  The defined
 *    value WAITFOREVER indicates not to return until the reply is received .
 * To receive all the responses to a multi-query, loop doing a x_ipcMultiReceive.
 *    When no more responses will be forthcoming, the return value of 
 *    x_ipcMultiReceive is "NullReply".
 * For example:  
 * while (x_ipcMultiReceive(mqRef, WAITFOREVER, &reply) != NullReply) {
 *                 -- Process and/or save reply data here --
 *                 }
 *
 * INPUTS: X_IPC_REF_PTR ref;
 *         void *reply;
 *         long timeout;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *          Can have the values: 
 *          "Success" -- a reply was received
 *          "NullReply" -- indicates that no more replies will be forthcoming.
 *		          This can come about either because the "max" number
 *                         of replies were received (see x_ipcMultiSend), or all 
 *                         the handlers registered for that message replied.
 *          "TimeOut" -- "timeout" seconds elapsed without a message being 
 *                        received.
 *
 * NOTE: The responses are received in the order that the modules handle 
 *       the query.
 *       If "max" < number registered handlers, then the "max" fastest replies 
 *       will be reported.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcMultiReceive(X_IPC_REF_PTR ref, void *reply,
				      long timeout)
{
  X_IPC_RETURN_VALUE_TYPE returnValue;
  
  returnValue = x_ipc_waitForReplyFrom(ref, (char *)reply, FALSE, timeout, NO_FD);
  
  if (returnValue == NullReply) 
    x_ipcRefFree(ref);
  
  return returnValue;
}
#endif /* NMP_IPC */

/******************************************************************************
 *
 * FUNCTION: void x_ipcEnableDistributedResponses()
 *           void x_ipcDisableDistributedResponses()
 *
 * DESCRIPTION: Enable messages to be replied to (via x_ipcReply, x_ipcSuccess,
 *              x_ipcFailure) by a procedure other than the handler.  In other
 *              words, a handler can return without issuing a response, and
 *              (later) another procedure can asynchronously respond to the 
 *              message (using the X_IPC_REF_PTR that has been saved somehow by
 *              the original handler).
 *
 *              This capability is useful for modules that must interact with
 *              an asynchronous device: a command or query is passed to the 
 *              device, and the response is handled by a different call-back
 *              procedure.
 *
 *              The "disable" function turns off this capability.
 * INPUTS: none
 *
 * OUTPUTS: none
 *
 * NOTES: If distributed responses are enabled, x_ipcReply, x_ipcSuccess and 
 *        x_ipcFailure all free the X_IPC_REF_PTR.  One should not attempt to
 *        access the pointer after one of those calls have been made 
 *        (in normal situations, the reference is not freed until after 
 *        the handler returns).
 *
 *****************************************************************************/

void x_ipcEnableDistributedResponses(void)
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(enableDistributedResponses) = TRUE;
  UNLOCK_M_MUTEX;
}

void x_ipcDisableDistributedResponses(void)
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(enableDistributedResponses) = FALSE;
  UNLOCK_M_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcWillListen(int listen)
 *
 * DESCRIPTION: Sets flag so X_IPC does not have to quess if the program will
 * listen for messages.
 *
 * INPUTS:
 * 
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcWillListen(int listen)
{
  LOCK_CM_MUTEX;
  GET_C_GLOBAL(willListen) = listen;
  UNLOCK_CM_MUTEX;
}


#ifndef NMP_IPC
/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcExecute(ref, name, data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref; 
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcExecute(const char *name,
				 const void *data)
{
  MSG_PTR msg;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;

  switch (msg->msgData->msg_class) {
  case GoalClass:
    return x_ipcExpandGoal(name, data);
    break;
  case CommandClass:
    return x_ipcExecuteCommand(name, data);
    break;
  default:
    x_ipc_checkMessageClass(msg, CommandClass);
    break;
  }
  return WrongMsgClass;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcExecuteWithConstraints(ref, name, 
 *                                  data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref; 
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcExecuteWithConstraints(X_IPC_REF_PTR ref, 
						const char *name,
						const void *data, 
						int tplConstr)
{
  MSG_PTR msg;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;

  switch (msg->msgData->msg_class) {
  case GoalClass:
    return x_ipcExpandGoalWithConstraints(ref, name, data, tplConstr);
    break;
  case CommandClass:
    return x_ipcExecuteCommandWithConstraints(ref, name, data, tplConstr);
    break;
  default:
    x_ipc_checkMessageClass(msg, CommandClass);
    break;
  }
  return WrongMsgClass;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcWaitForGoalWithConstraints(ref, name, data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref; 
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES:
 * 14-Aug-91: fedor: should check return valuse from 
 * x_ipc_sendMessage and x_ipc_waitForReply.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcWaitForGoalWithConstraints(X_IPC_REF_PTR ref, 
						    const char *name,
						    const void *data, 
						    int tplConstr)
{
  int32 refId;
  MSG_PTR msg;
  X_IPC_REF_PTR x_ipcRef;
  BLOCK_COM_TYPE blockCom;
  X_IPC_MSG_CLASS_TYPE replyClass;
  
  if (testInconsistent(tplConstr)) {
    X_IPC_MOD_ERROR1("ERROR: Inconsistent temporal constraints for command %s\n", name);
    return Failure;
  }
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;
  x_ipc_checkMessageClass(msg, GoalClass);
  
  blockCom.waitFlag = 1;
  blockCom.tplConstr = tplConstr;
  
  refId = x_ipc_nextSendMessageRef();
  (void)x_ipc_sendMessage(ref, msg, (char *)data, (char *)&blockCom, refId);
  
  x_ipcRef = x_ipcRefCreate(msg, name, refId);
  
  (void)x_ipc_waitForReply(x_ipcRef, (char *)&replyClass);
  
  x_ipcRefFree(x_ipcRef);
  
  if (replyClass == SuccessClass)
    return Success;
  else
    return Failure;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE 
 * x_ipcWaitForExecutionWithConstraints(ref, name, data, tplConstr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * X_IPC_REF_PTR ref; 
 * const char *name;
 * void *data; 
 * int tplConstr;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES:
 * 14-Aug-91: fedor: should check return valuse from 
 * x_ipc_sendMessage and x_ipc_waitForReply.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcWaitForExecutionWithConstraints(X_IPC_REF_PTR ref, 
							 const char *name,
							 const void *data, 
							 int tplConstr)
{ 
  MSG_PTR msg;
  
  msg = x_ipc_msgFind(name);
  if (msg == NULL) return MsgUndefined;

  switch (msg->msgData->msg_class) {
  case GoalClass:
    return x_ipcWaitForGoalWithConstraints(ref, name, data, tplConstr);
    break;
  case CommandClass:
    return x_ipcWaitForCommandWithConstraints(ref, name, data, tplConstr);
    break;
  default:
    x_ipc_checkMessageClass(msg, CommandClass);
    break;
  }
  return WrongMsgClass;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcWaitForExecution(name, data)
 *
 * DESCRIPTION:
 *
 * INPUTS: const char *name;
 *         void *data;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES:
 * 14-Aug-91: fedor: should check return valuse from 
 * x_ipc_sendMessage and x_ipc_waitForReply.
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcWaitForExecution(const char *name,
					  const void *data)
{
  
  return x_ipcWaitForExecutionWithConstraints((X_IPC_REF_PTR)NULL,
					    name,
					    data,
					    SEQ_ACH);
}
#endif /* NMP_IPC */

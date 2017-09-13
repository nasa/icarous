/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: burying globals under a rock!
 *
 * FILE: global.c
 *
 * ABSTRACT: define module global support routines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: globalMUtil.c,v $
 * Revision 2.11  2011/08/16 16:01:52  reids
 * Adding Python interface to IPC, plus some minor bug fixes
 *
 * Revision 2.10  2011/04/21 18:17:48  reids
 * IPC 3.9.0:
 * Added NoListen options to IPC_connect, to indicate that module will not
 *   periodically listen for messages.
 * Bug where having a message id of 0 or 1 interfaces with direct message
 *   functionality.
 * Extended functionality of "ping" to handle race condition with concurrent
 *   listens.
 * Fixed bug in how IPC_listenWait was implemented (did not necessarily
 *   respect the timeout).
 * Fixed conditions under which module listens for handler updates.
 *
 * Revision 2.9  2010/12/17 19:20:23  reids
 * Split IO mutex into separate read and write mutexes, to help minimize
 *   probability of deadlock when reading/writing very big messages.
 * Fixed a bug in multi-threaded version where a timeout is not reported
 *   correctly (which could cause IPC_listenClear into a very long loop).
 *
 * Revision 2.8  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.7  2002/06/25 16:44:59  reids
 * Fixed the way memory is freed when responses are handled;
 *   Allowed me to remove "responseIssuedGlobal".
 *
 * Revision 2.6  2002/01/03 20:52:12  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/07/24 12:12:50  reids
 * Enable distributed responses by default.
 *
 * Revision 2.4  2001/01/31 17:54:11  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.3  2000/07/27 16:59:10  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.2  2000/07/03 17:03:24  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.11  1997/01/27 20:09:29  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.3.2.9  1997/01/16 22:17:27  reids
 * Added "totalMemBytes" to improve the way usage stats are reported.
 *
 * Revision 1.3.2.8  1997/01/11 01:21:00  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.3.2.7.4.2  1996/12/27 19:26:02  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.3.2.7.4.1  1996/12/24 14:41:37  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.3.2.7  1996/12/18 15:10:15  reids
 * Fixed bug to enable multiple connects and disconnects (by not calling
 *   taskVarAdd more than once per task)
 * Fixed bug in Lispworks/VxWorks handling of messages, where a message
 *   could be "stranded" if it arrived during a blocking query
 * Defined common macros to clean up code
 *
 * Revision 1.3.2.6  1996/10/29 14:52:07  reids
 * Make "byteOrder" and "alignment" vars available to C, not just LISP.
 *
 * Revision 1.3.2.5  1996/10/24 15:19:17  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.3.2.4  1996/10/22 18:49:38  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.3.2.3  1996/10/18 18:04:36  reids
 * Added a few state vars, and moved one from server to module.
 *
 * Revision 1.3.2.2  1996/10/08 14:23:36  reids
 * Changes for IPC operating under Lispworks on the PPC.  Mainly changes
 * (LISPWORKS_FFI_HACK) due to the fact that Lispworks on the PPC is currently
 * missing the foreign-callable function.
 *
 * Revision 1.3.2.1  1996/10/02 20:58:31  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.4  1996/09/06 22:30:25  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.3  1996/05/09 18:19:29  reids
 * Changes to support CLISP.
 *
 * Revision 1.2  1996/05/09 16:53:41  reids
 * Remove (conditionally) references to matrix format.
 *
 * Revision 1.1  1996/05/09 01:01:26  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/03/12 03:19:45  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.2  1996/03/05 04:10:51  reids
 * Fixed a bug in the LISP version where it would go into an infinite
 *   segv loop if trying to connect without a central (lispExit was not set)
 *
 * Revision 1.1  1996/03/03 04:31:29  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.30  1996/07/25  22:24:24  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.29  1996/07/03  21:43:35  reids
 * Have "x_ipcWaitUntilReady" print out any required resources it is waiting for
 *
 * Revision 1.28  1996/06/27  15:40:12  rich
 * Added x_ipcGetAcceptFds.
 *
 * Revision 1.27  1996/06/25  20:50:40  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.26  1996/05/09  18:30:58  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.25  1996/05/07  16:49:33  rich
 * Changes for clisp.
 *
 * Revision 1.24  1996/03/05  05:04:31  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.23  1996/02/21  18:30:23  rich
 * Created single event loop.
 *
 * Revision 1.22  1996/02/06  19:04:45  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.21  1996/01/30  15:04:14  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.20  1996/01/27  21:53:31  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.19  1996/01/05  16:31:22  rich
 * Added windows NT port.
 *
 * Revision 1.18  1995/12/17  20:21:26  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.17  1995/12/15  01:23:12  rich
 * Moved Makefile to Makefile.generic to encourage people to use
 * GNUmakefile.
 * Fixed a memory leak when a module is closed and some other small fixes.
 *
 * Revision 1.16  1995/10/25  22:48:24  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.15  1995/10/07  19:07:20  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.14  1995/07/19  14:26:15  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.13  1995/07/06  21:16:27  rich
 * Solaris and Linux changes.
 *
 * Revision 1.12  1995/05/31  19:35:38  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.11  1995/04/07  05:03:11  rich
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
 * Revision 1.10  1995/03/30  15:43:03  rich
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
 * Revision 1.9  1995/03/28  01:14:34  rich
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
 * Revision 1.8  1995/03/19  19:39:31  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.7  1995/02/06  14:46:08  reids
 * Removed the "Global" suffix from x_ipcRootNode, x_ipcServer and x_ipcDefaultTime
 *
 * Revision 1.6  1995/01/30  16:18:03  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.5  1995/01/18  22:40:37  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.4  1994/11/02  21:34:17  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.3  1994/07/26  15:03:25  reids
 * Changed the order of an initialization test to make x_ipc_globalMInit work with
 * the Xavier navigation code.
 *
 * Revision 1.2  1994/05/29  16:20:29  reids
 * Changes needed to successfully make a VxWorks version
 *
 * Revision 1.1  1994/05/25  05:10:44  rich
 * Moved module global routines to a new file so they don't get included in
 * the .sa library file.
 *
 * Revision 1.14  1994/05/24  13:48:44  reids
 * Fixed so that messages are not sent until a x_ipcWaitUntilReady is received
 * (and the expected number of modules have all connected)
 *
 * Revision 1.13  1994/05/11  01:57:28  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.12  1994/04/28  16:16:07  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.11  1994/04/26  16:23:28  rich
 * Now you can register an exit handler before anything else and it will
 * get called if connecting to central fails.
 * Also added code to handle pipe breaks during writes.
 *
 * Revision 1.10  1994/04/16  19:42:14  rich
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
 * Revision 1.9  1994/04/15  17:09:53  reids
 * Changes to support vxWorks version of X_IPC 7.5
 *
 * Revision 1.8  1993/12/14  17:33:40  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.7  1993/11/21  20:17:54  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.6  1993/10/20  19:00:33  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.5  1993/08/30  21:53:31  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.4  1993/08/27  08:38:37  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.3  1993/08/27  07:14:56  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/06/13  23:28:09  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:23  rich
 * Importing x_ipc version 8
 *
 * Revision 8.1  1993/05/20  00:29:57  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:50  fedor
 * Added Logging.
 *
 * $Revision: 2.11 $
 * $Date: 2011/08/16 16:01:52 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifdef __sgi
#include "malloc.h"
#endif

#include "globalM.h"
#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif

#ifdef THREADED
#include "multiThread.h"
extern MUTEX_TYPE listMutex; /* Defined in list.c */
#endif

#if defined(CLISP)
extern int (lispbuffersize) (u_int32 g695, u_int32 g696, void* g697);
extern int (lispdecode) (u_int32 g667, u_int32 g668);
extern int (lispencode) (u_int32 g681, void* g682, u_int32 g683);
extern int (lispexit) (void);
#endif

void x_ipc_globalMInit(void)
{
#if defined(VXWORKS)
  if (x_ipc_gM == NULL) {
    if (taskVarAdd(0, (int *)&x_ipc_gM) != OK) {
      printErr("taskVarAdd failed\n");
    }
  }
#endif
  
  if (mGlobalp() && GET_C_GLOBAL(valid)) {
    /* Already initialized, nothing to do. */
    return;
  } else if (x_ipc_isValidServerConnection()) {
    /* Already running, shut down and reinitialize. */
    LOCK_CM_MUTEX;
    SHUTDOWN_SOCKET(GET_C_GLOBAL(serverRead));
    if (GET_C_GLOBAL(serverRead) != GET_C_GLOBAL(serverWrite))
      SHUTDOWN_SOCKET(GET_C_GLOBAL(serverWrite));
    UNLOCK_CM_MUTEX;
    x_ipc_globalMInvalidate();
    x_ipc_globalMFree();
  } else if (mGlobalp()){
    /* Was initialized, but the current values are not valid. */
    /* Free some memory and reinitialize. */
    x_ipc_globalMFree();
  } 
  /* Never created or has been freed, set the global pointer. */
#if defined(VXWORKS)
  x_ipc_gM = (GM_TYPE *)x_ipcMalloc(sizeof(GM_TYPE));
  bzero((void *)x_ipc_gM,sizeof(GM_TYPE));
#else
  x_ipc_gM_ptr = &x_ipc_gM;
#endif

#ifdef THREADED
  initMutex(&GET_M_GLOBAL(mutex));
#endif

  GET_M_GLOBAL(currentContext) = 
    (X_IPC_CONTEXT_PTR)x_ipcMalloc(sizeof(X_IPC_CONTEXT_TYPE));

#ifdef THREADED
  initMutex(&GET_C_GLOBAL(mutex));
  LOCK_CM_MUTEX;
  initMutex(&GET_M_GLOBAL(selectMutex));
  initPing(&GET_M_GLOBAL(ping));
  initMutex(&GET_C_GLOBAL(ioMutex));
  initMutex(&GET_C_GLOBAL(readMutex));
  initMutex(&listMutex);
#endif
  
#if defined(VXWORKS) || defined(NMP_IPC)
  GET_M_GLOBAL(enableDistributedResponses) = TRUE;
#else
  GET_M_GLOBAL(enableDistributedResponses) = FALSE;
#endif
  
  GET_C_GLOBAL(willListen) = TRUE;
  GET_C_GLOBAL(valid) = FALSE;
  
  GET_M_GLOBAL(byteOrder) = BYTE_ORDER;
  GET_M_GLOBAL(alignment) = (ALIGNMENT_TYPE)IPC_ALIGN;
  
  GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
  GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
  GET_C_GLOBAL(directDefault) = FALSE;
  
  GET_M_GLOBAL(pipeBroken) = FALSE;
  
  GET_M_GLOBAL(bufferToAlloc) = NULL;
  
  /* not done */
  GET_M_GLOBAL(byteFormat) = NULL;
  GET_M_GLOBAL(charFormat) = NULL;
  GET_M_GLOBAL(doubleFormat) = NULL;
  GET_M_GLOBAL(floatFormat) = NULL;
  GET_M_GLOBAL(intFormat) = NULL;
  GET_M_GLOBAL(longFormat) = NULL;
  GET_M_GLOBAL(shortFormat) = NULL;
  
  GET_M_GLOBAL(classFormatTable) = NULL;
  GET_M_GLOBAL(formatNamesTable) = NULL;
  GET_C_GLOBAL(handlerTable) = NULL;
  GET_C_GLOBAL(messageTable) = NULL;
  GET_C_GLOBAL(resourceTable) = NULL;
  GET_C_GLOBAL(hndIdTable) = NULL;
  GET_C_GLOBAL(msgIdTable) = NULL;
  GET_M_GLOBAL(listCellFreeListGlobal) = NULL;
  GET_M_GLOBAL(dataMsgBufferList) = NULL;
  GET_M_GLOBAL(listFreeListGlobal) = NULL;
  GET_C_GLOBAL(moduleConnectionTable) = NULL;
  GET_C_GLOBAL(pendingReplies) = NULL;
  initMsgQueue(&GET_C_GLOBAL(msgQueue));
  GET_C_GLOBAL(queryNotificationList) = NULL;
  GET_C_GLOBAL(connectNotifyList) = NULL;
  GET_C_GLOBAL(disconnectNotifyList) = NULL;
  GET_C_GLOBAL(changeNotifyList) = NULL;
  GET_C_GLOBAL(x_ipcRefFreeList) = NULL;
  /* GET_M_GLOBAL(x_ipcDefaultTimeGlobal) = {NoTime, {NoInterval, 0}}; */
  /* 16-Jan-93 */
  /* GET_M_GLOBAL(versionGlobal);*/
#if defined(DBMALLOC)
  GET_M_GLOBAL(x_ipcMallocMemHnd) = debug_malloc;
#elif defined(__TURBOC__)
  GET_M_GLOBAL(x_ipcMallocMemHnd) = farmalloc;
#else
  GET_M_GLOBAL(x_ipcMallocMemHnd) = malloc;
#endif
  GET_M_GLOBAL(modNameGlobal) = NULL;
  GET_C_GLOBAL(servHostGlobal) = NULL;
  
  FD_ZERO(&(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
  FD_ZERO(&(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
  
#ifdef LISP
  GET_M_GLOBAL(lispFlagGlobal) = '\0';
  GET_M_GLOBAL(lispRefSaveGlobal) = NULL;
#ifdef CLISP
  GET_M_GLOBAL(lispBufferSizeGlobal) = lispbuffersize;
  GET_M_GLOBAL(lispDecodeMsgGlobal) = lispdecode;
  GET_M_GLOBAL(lispEncodeMsgGlobal) = lispencode;
  GET_M_GLOBAL(lispExitGlobal) = lispexit;
#else /* !CLISP */
  GET_M_GLOBAL(lispBufferSizeGlobal) = NULL;
  GET_M_GLOBAL(lispDecodeMsgGlobal) = NULL;
  GET_M_GLOBAL(lispEncodeMsgGlobal) = NULL;
  GET_M_GLOBAL(lispExitGlobal) = NULL;
#endif /* !CLISP */
#ifdef NMP_IPC
  GET_M_GLOBAL(lispQueryResponseGlobal) = NULL;
#endif
#endif /* LISP */

  GET_M_GLOBAL(DMFree) = 0;
  GET_M_GLOBAL(DMTotal) =0;
  GET_M_GLOBAL(DMmin) = 0;
  GET_M_GLOBAL(DMmax) = 0;;
  GET_M_GLOBAL(directFlagGlobal) = 0;
  GET_M_GLOBAL(expectedWaitGlobal) = FALSE;
  GET_M_GLOBAL(freeMemRetryAmount) = 0;
  
  /* not done - only used in behaviors.c does not change */
  GET_M_GLOBAL(inconsistentConstraintsGlobal)[0] = (PLAN_FIRST+DELAY_PLANNING);
  GET_M_GLOBAL(inconsistentConstraintsGlobal)[1] = 0;
  
  GET_M_GLOBAL(mallocMemRetryAmount) = 1;
  GET_C_GLOBAL(parentRefGlobal) = -1; 
  GET_C_GLOBAL(sendMessageRefGlobal) = 1;
  GET_C_GLOBAL(listenPortNum) = 0;
  GET_C_GLOBAL(listenPort) = NO_FD;
  GET_C_GLOBAL(listenSocket) = NO_FD;
  GET_M_GLOBAL(totalMemRequest) = 0;
  GET_M_GLOBAL(totalMemBytes) = 0;
  
  GET_M_GLOBAL(indentGlobal) = 0;
  GET_M_GLOBAL(dPrintBYTE_FN) = NULL;
  GET_M_GLOBAL(dPrintUBYTE_FN) = NULL;
  GET_M_GLOBAL(dPrintCHAR_FN) = NULL;
  GET_M_GLOBAL(dPrintDOUBLE_FN) = NULL;
  GET_M_GLOBAL(dPrintFLOAT_FN) = NULL;
  GET_M_GLOBAL(dPrintFORMAT_FN) = NULL;
  GET_M_GLOBAL(dPrintINT_FN) = NULL;
  GET_M_GLOBAL(dPrintBOOLEAN_FN) = NULL;
  GET_M_GLOBAL(dPrintLONG_FN) = NULL;
#ifndef NMP_IPC
  GET_M_GLOBAL(dPrintMAP_FN) = NULL;
#endif
  GET_M_GLOBAL(dPrintSHORT_FN) = NULL;
  GET_M_GLOBAL(dPrintSTR_FN) = NULL;
  GET_M_GLOBAL(dPrintX_IPC_FN) = NULL;
  GET_M_GLOBAL(dPrintTWOBYTE_FN) = NULL;
  GET_M_GLOBAL(dPrintUINT_FN) = NULL;
  GET_M_GLOBAL(dPrintUSHORT_FN) = NULL;
  GET_M_GLOBAL(dPrintULONG_FN) = NULL;

  GET_M_GLOBAL(Message_Ignore_Set) = NULL;
  
  GET_M_GLOBAL(x_ipcExitHnd) = NULL;
  GET_M_GLOBAL(x_ipcFreeMemoryHnd) = NULL;
  
  GET_M_GLOBAL(logList)[0] = NULL;
  GET_M_GLOBAL(logList)[1] = NULL;
  GET_M_GLOBAL(logList)[2] = NULL;
  
  GET_M_GLOBAL(Found_Key) = NULL;/***/
  
  GET_C_GLOBAL(tappedMsgs) = NULL;
  GET_C_GLOBAL(broadcastMsgs) = NULL;
  GET_C_GLOBAL(maxConnection) = 0;

  GET_M_GLOBAL(requiredResources) = x_ipc_strListCreate();
  GET_M_GLOBAL(moduleList) = NULL;

#ifdef NMP_IPC
  GET_M_GLOBAL(timerList) = x_ipc_listCreate();
#endif

#ifdef LISPWORKS_FFI_HACK
  GET_C_GLOBAL(execHndState).state = ExecHnd_Idle;
#endif
  UNLOCK_CM_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_freeContext(X_IPC_CONTEXT_PTR *context)
 *
 * DESCRIPTION: 
 * Free memory that x_ipc allocates for a context.
 *
 *****************************************************************************/
static void x_ipc_freeContextList (LIST_PTR *listPtr)
{
  x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipcFree, *listPtr);
  x_ipc_listFree(listPtr);
  *listPtr = NULL;
}

static void localMsgFree (char *name, MSG_PTR msg)
{
  /* Don't free the DIRECT_MSG_QUERY yet, since it is needed 
     within freeDirectList, which is called by msgFreeMsg */
  if (msg && !STREQ(msg->msgData->name, X_IPC_DIRECT_MSG_QUERY)) {
    x_ipc_msgFree(name, msg);
  }
}

static void x_ipc_freeContext(X_IPC_CONTEXT_PTR *context)
{
  if (!*context) return;

  x_ipcFree((char *)(*context)->servHostGlobal);
  
  x_ipc_freeContextList(&((*context)->queryNotificationList));
  x_ipc_freeContextList(&((*context)->connectNotifyList));
  x_ipc_freeContextList(&((*context)->disconnectNotifyList));
  x_ipc_freeContextList(&((*context)->changeNotifyList));
  LOCK_M_MUTEX;
  x_ipcRefFree(GET_M_GLOBAL(x_ipcRootNodeGlobal));
  GET_M_GLOBAL(x_ipcRootNodeGlobal) = NULL;
  UNLOCK_M_MUTEX;
  x_ipc_freeContextList(&((*context)->x_ipcRefFreeList));
  
  x_ipc_hashTableFree(&((*context)->moduleConnectionTable),
		      x_ipc_hashItemsFree, NULL);
  x_ipc_hashTableFree(&((*context)->handlerTable),
		      (HASH_ITER_FN) x_ipc_hndFree, NULL);

  /* Need to treat DIRECT_MSG specially, freeing it separately at the end,
     since it is needed within x_ipc_msgFree */
  MSG_PTR directMsg = GET_MESSAGE(X_IPC_DIRECT_MSG_QUERY);
  x_ipc_hashTableFree(&((*context)->messageTable),
		      (HASH_ITER_FN) localMsgFree, NULL);
  if (directMsg) x_ipc_msgFree(X_IPC_DIRECT_MSG_QUERY, directMsg);

  x_ipc_hashTableFree(&((*context)->resourceTable),x_ipc_hashItemsFree, NULL);
  x_ipc_idTableFree(&(*context)->hndIdTable);
  x_ipc_idTableFree(&(*context)->msgIdTable);
  x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipc_dataMsgFree,
			 (*context)->pendingReplies);
  x_ipc_listFree(&((*context)->pendingReplies));
  (*context)->pendingReplies = NULL;
  x_ipcFree((char *)(*context)->msgQueue.messages);
  initMsgQueue(&(*context)->msgQueue);
  x_ipc_strListFree(&((*context)->tappedMsgs),TRUE);
  x_ipc_strListFree(&((*context)->broadcastMsgs),TRUE);
  
  x_ipc_listFree(&((*context)->queryNotificationList));
  
  x_ipcFree((char *)*context);
  *context = NULL;
}

/******************************************************************************
 *
 * FUNCTION: void x_ipc_globalMFree(void)
 *
 * DESCRIPTION: 
 * Free memory that x_ipc allocates in the module.
 *
 *****************************************************************************/
void x_ipc_globalMFree(void)
{
  if (mGlobalp()) {
    LOCK_M_MUTEX;
    x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipcFree, 
		     GET_M_GLOBAL(dataMsgBufferList));
    x_ipc_listFree(&(GET_M_GLOBAL(dataMsgBufferList)));
    GET_M_GLOBAL(dataMsgBufferList) = NULL;
    x_ipc_listFree(&(GET_M_GLOBAL(Message_Ignore_Set)));
    GET_M_GLOBAL(Message_Ignore_Set) = NULL;
    
    x_ipc_hashTableFree(&GET_M_GLOBAL(classFormatTable),
		  (HASH_ITER_FN)x_ipc_classEntryFree, NULL);
    x_ipc_hashTableFree(&GET_M_GLOBAL(formatNamesTable),
		  (HASH_ITER_FN)x_ipc_formatFreeEntry, NULL);
    x_ipc_hashTableFree(&GET_M_GLOBAL(externalFdTable),x_ipc_hashItemsFree, NULL);
    
    if (GET_M_GLOBAL(logList)[0]) {
      x_ipcFree((char *)(GET_M_GLOBAL(logList)[0])->theFile);
      x_ipcFree((char *)GET_M_GLOBAL(logList)[0]);
    }
    GET_M_GLOBAL(logList)[0] = NULL;
    if (GET_M_GLOBAL(logList)[1]) {
      x_ipcFree((char *)(GET_M_GLOBAL(logList)[1])->theFile);
      x_ipcFree((char *)GET_M_GLOBAL(logList)[1]);
    }
    GET_M_GLOBAL(logList)[1] = NULL;

    x_ipc_freeContext(&GET_M_GLOBAL(currentContext));

#ifdef THREADED
    freePing(&GET_M_GLOBAL(ping));
#endif

    x_ipc_listCleanup();
    
    x_ipc_strListFree(&GET_M_GLOBAL(requiredResources), TRUE);

    x_ipcFree((char *)GET_M_GLOBAL(modNameGlobal));

    if (GET_M_GLOBAL(byteFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(byteFormat));
    if (GET_M_GLOBAL(charFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(charFormat));
    if (GET_M_GLOBAL(shortFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(shortFormat));
    if (GET_M_GLOBAL(intFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(intFormat));
    if (GET_M_GLOBAL(longFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(longFormat));
    if (GET_M_GLOBAL(floatFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(floatFormat));
    if (GET_M_GLOBAL(doubleFormat)) x_ipc_freeFormatter(&GET_M_GLOBAL(doubleFormat));

    x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipcFree, GET_M_GLOBAL(timerList));
    x_ipc_listFree(&(GET_M_GLOBAL(timerList)));

#if defined(VXWORKS)
    x_ipcFree((char *)x_ipc_gM);
    x_ipc_gM = NULL;
#else
    x_ipc_globalMInit();
    x_ipcFree((char *)GET_M_GLOBAL(currentContext));
    bzero((char *)&x_ipc_gM,sizeof(x_ipc_gM));
    x_ipc_gM_ptr = NULL;
#endif
    UNLOCK_M_MUTEX;
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_globalMInvalidate(void)
 *
 * DESCRIPTION: 
 * Indicate that the module data structures are invalid and need to be 
 * reinitialized.  This is used when the server goes down.
 *
 *****************************************************************************/
void x_ipc_globalMInvalidate(void)
{
  if (mGlobalp()) {
    LOCK_CM_MUTEX;
    GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
    GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
    GET_C_GLOBAL(valid) = FALSE;
    UNLOCK_CM_MUTEX;
  }
}

/******************************************************************************
 *
 * FUNCTION: BOOLEAN x_ipc_isValidServerConnection(void)
 *
 * DESCRIPTION: 
 * Returns TRUE if the connection to the server is valid.
 *
 *****************************************************************************/
BOOLEAN x_ipc_isValidServerConnection(void)
{
  if (!mGlobalp()) {
    return FALSE;
  } else {
    BOOLEAN result;
    LOCK_CM_MUTEX;
    result = GET_C_GLOBAL(serverWrite) != NO_SERVER_GLOBAL;
    UNLOCK_CM_MUTEX;
    return result;
  }
}

/******************************************************************************
 *
 * FUNCTION: int32  x_ipcGetServer(void)
 *
 * DESCRIPTION: 
 * Returns the connection number for connection to central, or -1.
 *
 *****************************************************************************/
int  x_ipcGetServer(void)
{
  if (mGlobalp()) {
    int result;
    LOCK_CM_MUTEX;
    result = GET_C_GLOBAL(serverRead);
    UNLOCK_CM_MUTEX;
    return result;
  } else
    return -1;
}


/******************************************************************************
 *
 * FUNCTION: int  x_ipcGetConnections(void)
 *
 * DESCRIPTION: 
 * Returns set of open file descriptors.
 *
 *****************************************************************************/
fd_set  *x_ipcGetConnections(void)
{
  fd_set * result;

  LOCK_CM_MUTEX;
  result = &GET_C_GLOBAL(x_ipcConnectionListGlobal);
  UNLOCK_CM_MUTEX;

  return result;
}


/******************************************************************************
 *
 * FUNCTION: int  x_ipcGetConnections(void)
 *
 * DESCRIPTION: 
 * Returns set file descriptors open for accepting new connections.
 *
 *****************************************************************************/
fd_set  *x_ipcGetAcceptFds(void)
{
  fd_set * result;

  LOCK_CM_MUTEX;
  result = &GET_C_GLOBAL(x_ipcListenMaskGlobal);
  UNLOCK_CM_MUTEX;

  return result;
}


/******************************************************************************
 *
 * FUNCTION: int  x_ipcGetMaxConnection(void)
 *
 * DESCRIPTION: 
 * Returns the largest connection number ever used.
 *
 *****************************************************************************/
int x_ipcGetMaxConnection(void)
{
  int result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(maxConnection);
  UNLOCK_CM_MUTEX;

  return result;
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_REF_PTR x_ipcRootNode(void)
 *
 * DESCRIPTION: Return a reference to the root node of the task tree
 *
 *****************************************************************************/
X_IPC_REF_PTR x_ipcRootNode(void)
{
  if (mGlobalp()) {
    X_IPC_REF_PTR result;

    LOCK_M_MUTEX;
    result = GET_M_GLOBAL(x_ipcRootNodeGlobal);
    UNLOCK_M_MUTEX;

    return result;
  } else
    return NULL;
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_TIME_POINT_TYPE x_ipcDefaultTime(void)
 *
 * DESCRIPTION: Default time used for specifying interval monitors
 *
 *****************************************************************************/
X_IPC_TIME_POINT_TYPE x_ipcDefaultTime(void)
{
  X_IPC_TIME_POINT_TYPE result;

  LOCK_M_MUTEX;
  result = GET_M_GLOBAL(x_ipcDefaultTimeGlobal);
  UNLOCK_M_MUTEX;

  return result;
}

#ifdef LISP
/****************************************************************
 * Indicate that the module is LISP, not C
 ****************************************************************/
void set_Is_Lisp_Module(void)
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(lispFlagGlobal) = 'L';
  UNLOCK_M_MUTEX;
}
#endif /* LISP */

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: comServer.c
 *
 * ABSTRACT:
 * 
 * Communication Routines - Central Server
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: comServer.c,v $
 * Revision 2.12  2011/04/21 18:17:48  reids
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
 * Revision 2.11  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.10  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.9  2003/02/13 20:41:10  reids
 * Fixed compiler warnings.
 *
 * Revision 2.8  2002/06/25 16:45:39  reids
 * Removed memory leak when handler is deregistered.
 * Added casts to satisfy compiler.
 *
 * Revision 2.7  2001/08/13 13:39:02  reids
 * Added support for access control (using /etc/hosts.allow, /etc/hosts.deny)
 *
 * Revision 2.6  2001/01/31 17:54:11  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.5  2000/07/27 16:59:09  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.4  2000/07/19 20:56:06  reids
 * Took out extraneous logging messages
 *
 * Revision 2.3  2000/07/03 17:03:22  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/02/17 22:40:53  reids
 * Got rid of a stray  that was causing problems for an IRIX compiler.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.5.4.12  1997/03/07 17:49:33  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.5.4.11  1997/01/27 20:09:14  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.5.4.9  1997/01/16 22:16:10  reids
 * Added "memory" option, put "display" option back in.
 *
 * Revision 1.5.4.8  1997/01/11 01:20:49  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.5.4.7.4.1  1996/12/27 19:25:56  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.5.4.7  1996/12/18 15:10:01  reids
 * Changed logging code to remove VxWorks dependence on varargs
 * Defined common macros to clean up code
 *
 * Revision 1.5.4.6  1996/10/24 16:22:09  reids
 * Got rid of debugging output.
 *
 * Revision 1.5.4.5  1996/10/24 16:20:01  reids
 * Made defines for the sizes of some of the hash tables and id tables.
 *
 * Revision 1.5.4.4  1996/10/22 18:49:34  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.5.4.3  1996/10/18 18:00:24  reids
 * Better shutdown procedures.
 *
 * Revision 1.5.4.2  1996/10/16 13:58:50  reids
 * Fixed how formatters are freed.
 * Updated how named formatters are accessed.
 * Display the port number on which the central server is listening.
 *
 * Revision 1.5.4.1  1996/10/14 03:54:38  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.5  1996/07/19 21:18:46  reids
 * Record broadcast messages if handler is registered before message.
 *
 * Revision 1.4  1996/06/14 20:50:17  reids
 * Fixed two bugs related to freeing memory.
 *
 * Revision 1.3  1996/05/24 16:45:48  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.2  1996/05/09 16:53:39  reids
 * Remove (conditionally) references to matrix format.
 *
 * Revision 1.1  1996/05/09 01:01:17  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/04/24 19:11:02  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.3  1996/03/19 03:38:37  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:35  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:09  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.75  1996/08/05  16:06:47  rich
 * Added comments to endifs.
 *
 * Revision 1.74  1996/07/27  21:18:39  rich
 * Close log file on a "quit" typed into central.
 * Fixed problem with deleting handlers.  The hash iteration routine works
 * if the hander causes the hash item to be deleted.
 *
 * Revision 1.73  1996/07/25  22:24:16  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.72  1996/07/24  13:45:55  reids
 * Support for Windows95 (Thanks to Tam Ngo, JSC)
 * When module goes down, do not remove dispatch if still have task tree node.
 * Handle NULL format in x_ipc_dataStructureSize.
 * Add short, byte and ubyte formats for Lisp.
 * Ignore stdin when checking sockets for input.
 *
 * Revision 1.71  1996/07/19  18:13:47  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.70  1996/06/30  20:17:36  reids
 * Handling of polling monitors was severely broken.
 *
 * Revision 1.69  1996/06/25  20:50:22  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.68  1996/06/13  14:24:19  reids
 * Memory error -- message formats were being freed but not cleared.
 * Conditionally compile out task tree stuff for NMP IPC.
 *
 * Revision 1.67  1996/05/09  18:30:39  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.66  1996/03/19  02:29:09  reids
 * Plugged some more memory leaks; Added test code for enum formatters.
 * Added code to free formatter data structures.
 *
 * Revision 1.65  1996/03/15  21:22:54  reids
 * Fixed bug relating to re-registering messages -- now does not crash central.
 *
 * Revision 1.64  1996/03/09  06:13:09  rich
 * Fixed problem where lisp could use the wrong byte order if it had to
 * query for a message format.  Also fixed some memory leaks.
 *
 * Revision 1.63  1996/03/02  03:21:36  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.62  1996/02/21  18:30:17  rich
 * Created single event loop.
 *
 * Revision 1.61  1996/02/13  02:49:50  rich
 * Don't free the newModDataGlobal.
 *
 * Revision 1.60  1996/02/12  00:53:56  rich
 * Get VX works compile to work with GNUmakefiles.
 *
 * Revision 1.59  1996/02/11  21:34:59  rich
 * Updated GNUmakefiles for faster complilation.  Use FAST_COMPILE=1 for
 * routine recompiles.
 *
 * Revision 1.58  1996/02/10  16:49:41  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.57  1996/02/06  19:04:26  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.56  1996/01/30  15:03:56  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.55  1996/01/27  21:53:07  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.54  1996/01/23  00:06:29  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.53  1996/01/13  16:32:04  rich
 * Fixed looping for ever problem in providesHnd.
 *
 * Revision 1.52  1996/01/10  03:16:14  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.51  1996/01/05  16:31:10  rich
 * Added windows NT port.
 *
 * Revision 1.50  1995/12/17  20:21:20  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.49  1995/10/29  18:26:33  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.48.2.1  1995/10/27  15:16:46  rich
 * Fixed problems with connecting to multiple central servers.
 *
 * Revision 1.48  1995/10/26  20:22:30  reids
 * Fixed x_ipcMessageHandlerRegistered to not free the handler name.
 *
 * Revision 1.47  1995/10/25  22:48:06  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.46  1995/10/10  00:42:50  rich
 * Added more system messages to ignore.
 *
 * Revision 1.45  1995/10/07  19:07:07  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.44  1995/08/06  16:43:49  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.43  1995/07/26  20:42:31  rich
 * Recognize dump when not compiled with DBMALLOC, remove proc from prototypes.
 *
 * Revision 1.42  1995/07/19  14:25:58  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.41  1995/07/12  04:54:30  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.40  1995/07/10  16:17:04  rich
 * Interm save.
 *
 * Revision 1.39  1995/07/08  18:24:41  rich
 * Change all /afs/cs to /afs/cs.cmu.edu to get ride of conflict problems.
 *
 * Revision 1.38  1995/07/08  17:50:59  rich
 * Linux Changes.  Also added GNUmakefile.defs.
 *
 * Revision 1.37  1995/07/06  21:15:52  rich
 * Solaris and Linux changes.
 *
 * Revision 1.36  1995/06/14  03:21:30  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.35  1995/06/05  23:58:54  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.34  1995/05/31  19:35:12  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.33  1995/04/21  03:53:18  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.32  1995/04/19  14:27:54  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.31  1995/04/07  05:03:00  rich
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
 * Revision 1.30  1995/04/04  19:41:56  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.29  1995/03/30  15:42:39  rich
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
 * Revision 1.28  1995/03/28  01:14:25  rich
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
 * Revision 1.27  1995/03/19  19:39:26  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.26  1995/03/16  18:05:09  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.25  1995/01/25  00:01:04  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.24  1995/01/18  22:39:59  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.23  1994/11/02  21:34:08  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.22  1994/10/25  17:09:45  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.21  1994/05/25  04:57:15  rich
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
 * Revision 1.20  1994/05/24  13:48:37  reids
 * Fixed so that messages are not sent until a x_ipcWaitUntilReady is received
 * (and the expected number of modules have all connected)
 *
 * Revision 1.19  1994/05/17  23:15:27  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.18  1994/04/28  22:16:46  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 * Revision 1.17  1994/04/28  16:15:37  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.16  1994/04/16  19:41:52  rich
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
 * Revision 1.15  1994/03/28  02:22:44  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.14  1994/03/27  22:50:20  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.13  1994/01/31  18:27:35  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.12  1993/12/14  17:33:11  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.11  1993/12/01  18:57:51  rich
 * Changed TCP to  IPPROTO_TCP, a system defined constant.
 *
 * Revision 1.10  1993/12/01  18:03:02  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.9  1993/11/21  20:17:26  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.8  1993/10/21  16:13:41  rich
 * Fixed compiler warnings.
 *
 * Revision 1.7  1993/08/27  08:38:23  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.6  1993/08/27  07:14:24  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.5  1993/08/20  23:06:41  fedor
 * Minor changes for merge. Mostly added htons and removed cfree calls.
 *
 * Revision 1.4  1993/08/20  06:24:13  fedor
 * Changed centralRegisterNamedFormat back to centralRegisterLengthFormat
 * because of a lookup error in search_hash_table_for_format in parseFmttrs.c
 * This is only a temporary solution and does not really solve the problem.
 *
 * Revision 1.3  1993/06/13  23:28:03  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.2  1993/05/26  23:17:03  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:18  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:23  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:22  fedor
 * Added Logging.
 *
 * 6-May-92 Christopher Fedor, School of Computer Science, CMU
 * Added while loop to select if select dies on an EINTR then retry select.
 *
 * 31-Oct-91 Christopher Fedor, School of Computer Science, CMU
 * Moved resourceProcessPending of the implied resource from 
 * the place where temp resources were removed to x_ipc_waitForReply release
 * so that a reconnecting module has a chance to initialize before handling
 * incoming messages.
 *
 * 28-Oct-91 Christopher Fedor, School of Computer Science, CMU
 * Added repliesPending and alive field to MODULE_PTR to 
 * track replies to modules that have exited and clean up pending replies.
 * Added he calls moduleFree, pendingReplyAdd, pendingReplyRemove.
 *
 * 24-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Update refId field for HandlerRegClass in selfRegisterMsg since the
 * msg was never added to the msgIdTable. This is very old code and the
 * whole registration of messages should be rethought because msgData is
 * not needed by a module until a x_ipc_msgFind succeeds. selfRegisterMsg got
 * moved to server only code and still has alot of old dependencies.
 *
 * 20-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Moved parsing to msgInfoHnd. Added a parsedFormats toggle to msg struct.
 * User messages will get parsed when they are first asked for.
 * This corrects an error which wouldn't allow monitor messages to get parsed
 * unless a x_ipcWaitUntilReady was issued. the wait until ready idea also did
 * not work for a single module - which would need not use that call.
 *
 * 11-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Changed parsing of message formatters to occur before releasing the
 * wait until ready holds. This should allow x_ipcRegisterNamedFormatters time
 * to complete before using them.
 *
 * 13-Mar-91 Reid Simmons, School of Computer Science, CMU
 *  Added initialization routine for message tapping.
 *
 * 11-Dec-90 Christopher Fedor, School of Computer Science, CMU
 * Added timing information. Also added function to remove
 * messages waiting for handlers.
 *
 * 25-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Comments from no longer needed general com routines moved to comServer.c
 * Revised comServer.c to software standards.
 *
 * 18-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Simplified common routines. Service lookup routines were removed
 * they should be put back when the dust settles - for now the port
 * number is fixed and defined in x_ipcInternal.h
 *
 * 16-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Moved Message_Class_Name to behaviors.c
 * Removed initReplyHnd which is no longer used.
 * Revised to Software Standards.
 *  
 *  3-Dec-89 Long-Ji Lin, School of Computer Science, CMU
 * Commented out x_ipc_dataMsgFree. Added call to Exception_Layer_Initialize.
 *
 * 14-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Move "CreateRefHnd" to tasktree.c
 *
 * 11-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Changed "goaltree" to "tasktree".
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Changed "printf"s to use logging facility.
 *
 * 10-May-89 Christopher Fedor, School of Computer Science, CMU
 * InitReplyHnd - reserve a slot for reply.
 *
 *  9-May-89 Reid Simmons, School of Computer Science, CMU
 * Added call to implement polling monitors.
 *
 * 28-Apr-89 Reid Simmons, School of Computer Science, CMU
 * Added code to initialize temporal constraints.
 * Initialize "RootNode" field in TC_ModInfo.
 *
 * 11-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * Rewrote InitInternalFormatters to use x_ipcRegisterLengthFormatter.
 * Removed com_server.c routines found in Reid Simmons communcations.c
 *
 *  9-Mar-89 Reid Simmons, School of Computer Science, CMU
 * Seperated Module/Server Common communciation routines.
 *
 *  6-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * tried to seperate module/server communications
 *
 * 23-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * why yet again of course!
 *
 *  1-Dec-88 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.12 $
 * $Date: 2011/04/21 18:17:48 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"
#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

static void recordBroadcast(const char *name, BOOLEAN active);
static void cleanBroadcast(void);
static void availableHnd(DISPATCH_PTR dispatch, char *ignore);
static void updateDirectHandlers(const MSG_PTR msg);
static void directHandlersList (const MSG_PTR msg, int *num,
				DIRECT_MSG_HANDLER_TYPE handlers[]);

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

static int32 modHashFunc(MODULE_KEY_PTR modKey)
{
  const char *s;
  int32 sum, i;
  
  sum = modKey->sd;
  s = modKey->name;
  for (i=0;s[i] != '\0';i++)
    if (isupper((int)s[i]))
      sum += tolower(s[i]);
    else
      sum += s[i];
  
  return sum;
}


/******************************************************************************
 *
 * FUNCTION: int32 modKeyEqFunc(modKeyA, modKeyB)
 *
 * DESCRIPTION: 
 * Module key equal function. A module is equal if its name and socket
 * origin number are equal. Case insensitive.
 *
 * INPUTS: MODULE_KEY_PTR modKeyA, modKeyB;
 *
 * OUTPUTS: int32
 *
 ****************************************************************************/

static int32 modKeyEqFunc(MODULE_KEY_PTR modKeyA, MODULE_KEY_PTR modKeyB)
{
  int32 i;
  char a1, b1;
  const char *a, *b;
  
  if (modKeyA->sd != modKeyB->sd)
    return FALSE;
  
  a = modKeyA->name;
  b = modKeyB->name;
  
  i = 0;
  while (a[i] != '\0' && b[i] != '\0') {
    if (isupper((int)a[i]))
      a1 = tolower((int)a[i]);
    else 
      a1 = a[i];
    if (isupper((int)b[i]))
      b1 = tolower((int)b[i]);
    else 
      b1 = b[i];
    
    if (a1 != b1)
      return FALSE;
    
    i++;
  }
  
  if (a[i] == b[i])
    return TRUE;
  else
    return FALSE;
}


/******************************************************************************
 *
 * FUNCTION: MODULE_PTR x_ipcModuleCreate(sd, modData)
 *
 * DESCRIPTION:
 * Create a new module. Initialize module parameters sd and modData.
 *
 * INPUTS:
 * int sd;
 * MOD_DATA_PTR modData;
 *
 * OUTPUTS: MODULE_PTR
 *
 *****************************************************************************/

static MODULE_PTR x_ipcModuleCreate(int readSd, int writeSd,
				  MOD_DATA_PTR modData)
{
  MODULE_PTR module;
  
  module = NEW(MODULE_TYPE);
  module->readSd = readSd;
  module->writeSd = writeSd;
  module->wait = TRUE;
  module->port = -1;
  module->alive = TRUE;
  module->repliesPending = 0;
  module->modData = modData;
  module->hndList = x_ipc_listCreate();
  module->impliedResource = NULL;
  module->resourceList = x_ipc_listCreate();
  module->providesList = x_ipc_strListCreate();
  module->requiresList = x_ipc_strListCreate();
  return module;
}


/******************************************************************************
 *
 * FUNCTION: void removeConnection(module)
 *
 * DESCRIPTION: 
 * Removes a module in response to a close detected on a socket or
 * an explicit call to close a module - x_ipcClose.
 *
 * INPUTS: 
 * MODULE_PTR module;
 *
 * OUTPUTS: void. 
 *
 *****************************************************************************/

static int32 removeDispatchFromWait(int *sd, DISPATCH_PTR dispatch)
{
  if (dispatch->orgId == *sd) {
    GET_S_GLOBAL(waitTotalGlobal)--;
    dispatchFree(dispatch);
    return TRUE;
  }
  
  return FALSE;
}

void moduleFree(MODULE_PTR module)
{
  if (module->modData) {
    if ((module->modData->modName != NULL) &&
	(module->modData->modName != GET_S_GLOBAL(newModDataGlobal).modName))
      x_ipcFree((void *)module->modData->modName);
    
    if ((module->modData->hostName != NULL) &&
	(module->modData->hostName != GET_S_GLOBAL(newModDataGlobal).hostName))
      x_ipcFree((void *)module->modData->hostName);
    
    if ((module->modData != &GET_S_GLOBAL(newModDataGlobal)))
      x_ipcFree((void *)module->modData);
  }
  
  x_ipc_listFree(&(module->resourceList));
  x_ipc_strListFree(&(module->providesList), TRUE);
  x_ipc_strListFree(&(module->requiresList), TRUE);
  
  x_ipcFree((void *)module);
  
  module = NULL; /* possibly force some memory errors */
}

void pendingReplyAdd(DISPATCH_PTR dispatch)
{
  if (dispatch->org)
    dispatch->org->repliesPending++;
}

void pendingReplyRemove(DISPATCH_PTR dispatch)
{
  if (dispatch->org)
    dispatch->org->repliesPending--;
}

/******************************************************************************
 *
 * FUNCTION: void moduleClean(MODULE_PTR module)
 *
 * DESCRIPTION: Cleans up a module that has gone down.
 *
 * INPUTS: MODULE_PTR;  A pointer to the module
 *
 * OUTPUTS: none
 *
 *****************************************************************************/
static int32 waitDeleteFunc(MODULE_PTR module, DISPATCH_PTR dispatch)
{
  if (dispatch->org == module) {
    x_ipc_listDeleteItem(dispatch,GET_S_GLOBAL(waitList));
    dispatchFree(dispatch);
    return TRUE;
  } else {
    return FALSE;
  }
}  

void moduleClean(MODULE_PTR module)
{
  LIST_PTR msgList;
  HND_PTR hnd;
  MSG_PTR msg;

  x_ipc_listTestDeleteItem((LIST_ITER_FN) removeDispatchFromWait, 
		     (void *)&module->readSd, GET_S_GLOBAL(waitList));
  /* Gak.  This is ugly -- cannot put the "updateDirectHandlers" within
     x_ipc_hndDelete, because it is in the module library, but updateDirectHandlers
     calls server-only functions.  Thus, I have to save a list of all affected 
     messages *before* calling x_ipc_hndDelete, then updateDirectHandlers */
  msgList = x_ipc_listCreate();
  hnd = (HND_PTR)x_ipc_listFirst(module->hndList);
  while (hnd) {
    msg = (MSG_PTR)x_ipc_listFirst(hnd->msgList);
    while (msg) {
      if ((msg->direct || msg->notifyHandlerChange) &&
	  !x_ipc_listMemberItem((void *)msg, msgList)) 
	x_ipc_listInsertItem((void *)msg, msgList);
      msg = (MSG_PTR)x_ipc_listNext(hnd->msgList);
    }
    hnd = (HND_PTR)x_ipc_listNext(module->hndList);
  }
  x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipc_hndDelete, module->hndList);
  x_ipc_listFreeAllItems((LIST_FREE_FN)resourceDelete, module->resourceList);
  x_ipc_listFree(&(module->hndList));
  x_ipc_listFree(&(module->resourceList));
  
  x_ipc_listFreeAllItems((LIST_FREE_FN)updateDirectHandlers, msgList);
  x_ipc_listFree(&msgList);

#ifndef NMP_IPC
  /*
   * Needed so that restarting the module doesn't interfere with
   * the old temporal constraints
   */
  removeModLastChild(GET_S_GLOBAL(taskTreeRootGlobal), module);
#endif
  
  module->alive = FALSE;
  
  if (module->wait) {
    x_ipc_listTestDeleteItemAll((LIST_ITER_FN) waitDeleteFunc,
			  (void *)module, GET_S_GLOBAL(waitList));
    moduleFree(module);
  } else if (!module->repliesPending) {
    moduleFree(module);
  }
}

void removeConnection(MODULE_PTR module)
{
  x_ipc_listDeleteItem((void *)module, GET_M_GLOBAL(moduleList));
  moduleClean(module);
  cleanBroadcast();
}


/******************************************************************************
 *
 * FUNCTION: MODULE_PTR addConnection(sd, modData)
 *
 * DESCRIPTION:
 * Create a new module.
 * Add the sd to x_ipcConnectionListGlobal.
 * Add the module to the list of active modules.
 * Send the new module intial message set.
 *
 * INPUTS:
 * int sd;
 * MOD_DATA_PTR modData;
 *
 * OUTPUTS: MODULE_PTR
 *
 *****************************************************************************/

static MODULE_PTR addConnection(int readSd, int writeSd, MOD_DATA_PTR modData)
{
  MODULE_PTR module;
  
  module = x_ipcModuleCreate(readSd, writeSd, modData);
  
  FD_SET(module->readSd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
  x_ipc_listInsertItem((void *)module, GET_M_GLOBAL(moduleList));
  
  msgInfoMsgSend(module->writeSd);
  
  /* Need to keep track of the maximum.  */
  GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection), readSd);
  GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection), writeSd);
  return module;
}


/******************************************************************************
 *
 * FUNCTION: MODULE_PTR serverModCreate(sd)
 *
 * DESCRIPTION: 
 * Create a new module for the central server.
 * Add the sd to x_ipcConnectionListGlobal.
 *
 * INPUTS:
 * int sd;
 * MOD_DATA_PTR modData;
 *
 * OUTPUTS: MODULE_PTR
 *
 *****************************************************************************/

static MODULE_PTR serverModCreate(void)
{ 
  MODULE_PTR module = NULL;
  MOD_DATA_PTR modData;
  MODULE_KEY_TYPE modKey;
  char *modNamePtr;
  
  modData = NEW(MOD_DATA_TYPE);
  modNamePtr = (char *)x_ipcMalloc(sizeof(SERVER_MOD_NAME));
  BCOPY(SERVER_MOD_NAME, modNamePtr, sizeof(SERVER_MOD_NAME));
  modData->modName = (const char *) modNamePtr;
  modData->hostName = (char *)x_ipcMalloc(sizeof(char)*HOST_NAME_SIZE+1);
  bzero((void *)modData->hostName,HOST_NAME_SIZE+1);
  gethostname((char *)modData->hostName, HOST_NAME_SIZE);
  
  module = x_ipcModuleCreate(CENTRAL_SERVER_ID,CENTRAL_SERVER_ID, modData);
  
  modKey.sd = CENTRAL_SERVER_ID;
  modKey.name = modData->modName;
  
  (void)x_ipc_hashTableInsert((char *)&modKey, sizeof(MODULE_KEY_TYPE), 
			(char *)module, GET_S_GLOBAL(moduleTable));
  
  x_ipc_listInsertItem((void *)module, GET_M_GLOBAL(moduleList));
  
  return module;
}


/******************************************************************************
 *
 * FUNCTION: MODULE_PTR serverModListen(sd)
 *
 * DESCRIPTION:
 * Create a new module for the central server.
 * Add the sd to x_ipcConnectionListGlobal.
 *
 * INPUTS:
 * int sd;
 * MOD_DATA_PTR modData;
 *
 * OUTPUTS: MODULE_PTR
 *
 *****************************************************************************/

static void serverModListen(int sd)
{
  
  FD_SET(sd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
  FD_SET(sd, &(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
  
  /* Need to keep track of the maximum.  */
  GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection), sd);
}


/******************************************************************************
 *
 * FUNCTION: void providesHnd(DISPATCH_PTR dispatch, STR_LIST_PTR resources)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static void providesHnd(DISPATCH_PTR dispatch, STR_LIST_PTR resources)
{
  const char *res;
  res = x_ipc_strListPopItem(resources);
  while (res != NULL) {
    x_ipc_strListPushUnique(res,dispatch->org->providesList);
    res = x_ipc_strListPopItem(resources);
  }
  x_ipc_strListFree(&resources,FALSE);
}


/******************************************************************************
 *
 * FUNCTION: void requiresHnd(DISPATCH_PTR dispatch, STR_LIST_PTR resources)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static void requiresHnd(DISPATCH_PTR dispatch, STR_LIST_PTR resources)
{
  const char *res;
  res = x_ipc_strListPopItem(resources);
  while (res != NULL) {
    x_ipc_strListPushUnique(res,dispatch->org->requiresList);
    res = x_ipc_strListPopItem(resources);
  }
  x_ipc_strListFree(&resources,FALSE);
}


/******************************************************************************
 *
 * FUNCTION: void availableHnd(DISPATCH_PTR dispatch, STR_LIST_PTR resources)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static void availableHnd(DISPATCH_PTR dispatch, char *ignore)
{
#ifdef UNUSED_PRAGMA
#pragma unused(ignore)
#endif
  STR_LIST_PTR resources;
  LIST_ELEM_PTR listTmp;
  
  resources = x_ipc_strListCreate();
  
  for (listTmp = GET_M_GLOBAL(moduleList)->first; 
       (listTmp != NULL);
       listTmp = listTmp->next
       ) {
    STR_LIST_ITERATE(((MODULE_PTR)(listTmp->item))->providesList, string,
		     { if (string != NULL)
		         x_ipc_strListPushUnique(string, resources);
		     });
  }
  centralReply(dispatch, (void *)resources);
  x_ipc_strListFree(&resources,FALSE);
}


/******************************************************************************
 *
 * FUNCTION: NewModuleConnectHnd(DISPATCH_PTR dispatch, MOD_DATA_PTR modData)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static void NewModuleConnectHnd(DISPATCH_PTR dispatch, MOD_DATA_PTR modData)
{
  RESOURCE_PTR resource;
  MOD_START_TYPE versionData;
  
  LOG1(" Received a new connection: %d\n", dispatch->orgId);
  LOG1("   modName : %s\n", modData->modName);
  LOG1("   hostName: %s\n", modData->hostName);

  /* Inform anyone who is interested about this connection */
  centralBroadcast(IPC_CONNECT_NOTIFY_MSG, &(modData->modName));

  resource = resourceCreate(dispatch->orgId,dispatch->orgId,
			    modData->modName, 1);
  
  dispatch->org->modData = modData;
  dispatch->org->impliedResource = resource;
  
  x_ipc_listInsertItem((void *)resource, dispatch->org->resourceList);
  
  versionData.version.x_ipcMajor = X_IPC_VERSION_MAJOR;
  versionData.version.x_ipcMinor = X_IPC_VERSION_MINOR;
  versionData.direct = GET_S_GLOBAL(directDefault);
  
  x_ipc_strListPushUnique(strdup(resource->name),dispatch->org->providesList);
  centralReply(dispatch, (char *)&versionData);
  
  /* Don't free the modData: It is stored with the module */
  /*  x_ipcFreeData(X_IPC_CONNECT_QUERY,modData);*/
}

/***********************************************************************/

void parseMsg(MSG_PTR msg)
{
  /* 20-Jun-91: fedor: parse formats added to allow messages
     to be registered before they are used and allow register named
     formatter to propagate across modules. */
  if (msg->msgFormatStr)
    msg->msgData->msgFormat = ParseFormatString(msg->msgFormatStr);
  
  if (msg->resFormatStr)
    msg->msgData->resFormat = ParseFormatString(msg->resFormatStr);      
  
  msg->parsedFormats = TRUE;
}


/******************************************************************************
 *
 * FUNCTION: MSG_PTR selfRegisterMsg(msgData)
 *
 * DESCRIPTION: 
 *
 * If a message with the same message name is found to exist in the hash table
 * that message data is replaced with the new msgData and a warning message
 * is issued. If this happens refId of the new msgData is set equal to the
 * refId being replaced.
 *
 * If a message created by x_ipc_selfRegisterHnd is found it is simply replaced
 * without warning.
 *
 * The newly created or updated message is returned.
 *
 * INPUTS:
 * MSG_DATA_PTR msgData;
 *
 * OUTPUTS: MSG_PTR
 *
 * NOTES:
 *
 * 23-Jun-91: fedor: Always a good idea to trigger parse when info changes
 * - this allows us to use HandlerRegClass for taps with missing msgs.
 *
 * 20-Jun-91: fedor: Toggle to reparse formatters - needed when modules
 * come and go - really needed for selfquery msgs of modules that crash.
 *
 *****************************************************************************/

// 11-Apr-2011: reids: msg id's cannot be 0 or 1 -- interferes with other
// aspects of IPC (specifically, direct messages)
static int32 insertMsgInTable (MSG_PTR msg)
{
  int32 msgId = x_ipc_idTableInsert((void *)msg, GET_C_GLOBAL(msgIdTable));
  if (msgId < 2) {
    idTableItem(msgId, GET_C_GLOBAL(msgIdTable)) = NULL; // Ignore this item
    int32 newMsgId = insertMsgInTable(msg);
    msgId = newMsgId;
  }

  return msgId;
}

static MSG_PTR selfRegisterMsg(const char *name, X_IPC_MSG_CLASS_TYPE msg_class)
{
  int32 localId;
  MSG_PTR msg;
  MSG_DATA_PTR msgData;
  CONST_FORMAT_PTR *format;

  msg = GET_MESSAGE(name);
  if (msg) {
    msgData = msg->msgData;
    localId = msgData->refId;
    if (msgData->msg_class != HandlerRegClass) {
      Start_Ignore_Logging();
      LOG_STATUS1("Registration: Message %s Found. Updating.\n", name);
      End_Ignore_Logging();
    } else {
      /* 24-Jun-91: fedor: update refId because a HandlerRegClass
	 did not get added to the msgIdTable - mostly to avoid the
	 msgIdTable in modules. This needs more work and a rewrite! */
      localId = insertMsgInTable(msg);
      // If put this message on the broadcast list (due to a subscribed
      //  handler), but turns out not really a broadcast, remove it from
      //  the broadcast list (13-Apr-2011, reids)
      if (msg_class != BroadcastClass) {
	recordBroadcast(name, FALSE);
      }
    }
    
    x_ipcFree((void *)msgData->name);
    msgData->name = name;
    msgData->msg_class = msg_class;
    if (msgData->msgFormat) {
      /* Monitor formats just point to another message's formats 
       * - don't free */
      if (msgData->msg_class != PollingMonitorClass &&
	  msgData->msg_class != DemonMonitorClass) {
	format = (CONST_FORMAT_PTR *)(&(msgData->msgFormat));
	x_ipc_freeFormatter(format);
      }
      msgData->msgFormat = NULL;
    }
    if (msgData->resFormat) {
      /* Monitor formats just point to another message's formats 
       * - don't free */
      if (msgData->msg_class != PollingMonitorClass &&
	  msgData->msg_class != DemonMonitorClass) {
	format = (CONST_FORMAT_PTR *)(&(msgData->resFormat));
	x_ipc_freeFormatter(format);
      }
      msgData->resFormat = NULL;
    }
    
    /* 24-Jun-91: fedor: not sure if this will force a parse everywhere
       - there may be some strong assumptions about message registration
       before wait until ready - 2nd registration elsewhere may not
       pick up correct parsed format info. */
    msg->parsedFormats = FALSE;
    
    if (msg->tapList && x_ipc_listLength(msg->tapList)) {
      (void)x_ipc_listIterate((LIST_ITER_FN)checkTaps, (void *)msg, msg->tapList);
    }

    if (msg->msgData->msg_class == BroadcastClass &&
	msg->hndList && x_ipc_listLength(msg->hndList)) {
      recordBroadcast(msg->msgData->name, TRUE);
    }

  } else {
    msgData = NEW(MSG_DATA_TYPE);
    msgData->name = name;
    msgData->msg_class = msg_class;
    msgData->msgFormat = NULL;
    msgData->resFormat = NULL;

    msg = x_ipc_msgCreate(msgData);
    localId = insertMsgInTable(msg);
  }
  
  msgData->refId = localId;
  
  return msg;
}

/***********************************************************************/

static void registerMessageHnd(DISPATCH_PTR dispatch,
			       MSG_REG_DATA_PTR msgRegData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dispatch)
#endif
  MSG_PTR msg;
  
  msg = selfRegisterMsg(msgRegData->name, msgRegData->msg_class);
  
  /* Free old format strings, if there are any. */
  if (msg->msgFormatStr) {
    x_ipcFree((void *)msg->msgFormatStr);
    msg->msgFormatStr = NULL;
  }
  if (msg->resFormatStr) {
    x_ipcFree((void *)msg->resFormatStr);
    msg->resFormatStr = NULL;
  }
  /* 3-Jul-91: Reid: Don't bother copying the format strings -- 
     just be careful not to delete msgRegData recursively! */
  if (msgRegData->msgFormat) {
    msg->msgFormatStr = msgRegData->msgFormat;
  }
  if (msgRegData->resFormat) {
    msg->resFormatStr = msgRegData->resFormat;
  }
  
  /* If the message class is inform and query and direct connections are
   * enabled,  
   */
  if (DIRECT_MSG(msg)) {
    msg->direct = GET_S_GLOBAL(directDefault);
    if (msg->direct || msg->notifyHandlerChange) {
      centralRegisterVar(msg->msgData->name, DIRECT_MSG_FORMAT);
      centralIgnoreVarLogging(msg->msgData->name);
      updateDirectHandlers(msg);
    }
  }

  /* Just frees the top-level structure because the strings msgRegData
     points to (i.e., name, msgFormat, resFormat) are all stored. */
  x_ipcFree((void *)msgRegData);
}

/***********************************************************************/

void centralRegisterMessage(const char *name, X_IPC_MSG_CLASS_TYPE msg_class,
			    const char *msgFormat, const char *resFormat)
{
  MSG_PTR msg;
  
  msg = selfRegisterMsg(strdup(name), msg_class);
  
  msg->msgData->msgFormat = (msgFormat ? ParseFormatString(msgFormat) : NULL);
  msg->msgData->resFormat = (resFormat ? ParseFormatString(resFormat) : NULL);

  msg->msgFormatStr = msgFormat;
  msg->resFormatStr = resFormat;
  msg->parsedFormats = TRUE;
}

/***********************************************************************/

void centralRegisterNamedFormatter(const char *formatterName,
				   const char *formatString)
{ 
  x_ipc_addFormatToTable(formatterName, ParseFormatString(formatString));
}

/***********************************************************************/

/* No longer used: Reids 2/93 */
/* back to deal with display problem with printdata - tmp solution 8/93 fedor*/

void centralRegisterLengthFormatter(char *formatterName, int32 length)
{ 
  char s[11];
  
  bzero(s, sizeof(s));
  
  snprintf(s, sizeof(s)-1, "%d", length);
  
  centralRegisterNamedFormatter(formatterName, s);
}

/***********************************************************************/

void _centralRegisterHandler(const char *msgName, 
			     const char *hndName,
			     X_IPC_HND_FN hndProc)
{
  HND_DATA_PTR hndData;
  HND_PTR hnd;
  
  if (!hndProc)
    X_IPC_ERROR1("ERROR: centralRegisterHandler: hndProc for %s is NULL.\n", 
	     hndName);
  
  hndData = NEW(HND_DATA_TYPE);
  
  hndData->refId = 0; /* will be updated by selfRegisterHandler */
  hndData->msgName = strdup(msgName);
  hndData->hndName = strdup(hndName);
  
  hnd = x_ipc_selfRegisterHnd(0, GET_S_GLOBAL(x_ipcServerModGlobal), hndData, hndProc);
  if (hnd->hndData != hndData) {
    x_ipcFree((char *)hndData->msgName);
    x_ipcFree((char *)hndData->hndName);
    x_ipcFree((char *)hndData);
  }
}

/***********************************************************************/

static int32 addHndItem(DISPATCH_HND_PTR hnd, DISPATCH_PTR dispatch)
{
  dispatch->hnd = hnd;
  dispatch->des = hnd->hndOrg;
  dispatch->desId = hnd->hndOrg->writeSd;
  
  return 1;
}

static int32 addPendItem(RESOURCE_PTR newResource, DISPATCH_PTR dispatch)
{
  LOG_STATUS1("   Transferring %s", DISPATCH_MSG_NAME(dispatch));
  Log_RefId(dispatch, LOGGING_STATUS);
  LOG_STATUS2(" from Resource %s to %s", dispatch->hnd->resource->name,
	     newResource->name);
  
  x_ipc_listInsertItem((void *)dispatch, newResource->pendingList);

  return 1;
}

static void registerHandlerHnd(DISPATCH_PTR dispatch, HND_DATA_PTR hndData)
{
  MSG_PTR msg;
  HND_PTR hnd;
  RESOURCE_PTR res;
  
  res = NULL;
  msg = GET_MESSAGE(hndData->msgName);
  if (msg) {
    hnd = (HND_PTR)x_ipc_listFirst(msg->hndList);
    if (hnd) {
      res = hnd->resource;
      if (!res || (!res->capacity)) {
	/* ok this handler is the created one */
	x_ipc_hndDelete(hnd);
	x_ipc_listDeleteItem((char *)hnd, msg->hndList);
      }
    }
    /* Record that the broadcast message is active. */
    if (msg->msgData->msg_class == BroadcastClass) {
      recordBroadcast(msg->msgData->name, TRUE);
    }
  }
  
  hnd = x_ipc_selfRegisterHnd(dispatch->orgId, dispatch->org, hndData, NULL);
  
  hnd->resource = dispatch->org->impliedResource;
  
  // There can a race condition with updating the list of broadcast messages
  //  if the handler is subscribed before the message is defined.  Be 
  //  conservative and record here, but may have to backtrack if the message
  //  turns out to not be a broadcast (12-Apr-2011, reids)
  if (!msg) {
    recordBroadcast(hndData->msgName, TRUE);
  }

#if 0
  if ((dispatch->org->port != -1) && (msg != NULL) &&
      /* Not a "watch var" message */
      strncmp(VAR_WATCH_PREFIX, msg->msgData->name, 
	      strlen(VAR_WATCH_PREFIX)) != 0) {
    /*msg->direct = TRUE;*/
    if (msg->direct || msg->notifyHandlerChange)
      updateDirectHandlers(msg);
  }
#else
  if (msg && (msg->direct || msg->notifyHandlerChange)) {
    updateDirectHandlers(msg);
  }
#endif
  
  if (!x_ipc_listMemberItem((char *)hnd, dispatch->org->hndList))
    x_ipc_listInsertItem((char *)hnd, dispatch->org->hndList);
  
  if (res && !res->capacity) {
    /* 12-May-91: fedor: make sure that this was the tmp resource and
       not simply the resource of the first hnd in the msg->hndList 
       - this can cause problems when registering multiple handlers
       for the same message. Should clean these ideas up. */
    x_ipc_listFree(&(res->attendingList));
    (void)x_ipc_listIterateFromFirst((LIST_ITER_FN)addHndItem, (void *)hnd, 
			       res->pendingList);
    (void)x_ipc_listIterateFromFirst((LIST_ITER_FN)addPendItem, 
			       (void *)dispatch->org->impliedResource,
			       res->pendingList);
    x_ipc_listFree(&(res->pendingList));
    resourceDelete(res);
    
    /* 31-Oct-91: fedor: moved to waitFor release
       resourceProcessPendingRes(dispatch->org->impliedResource);*/
  }

  /* Free the hndData, unless it is stored with the handler */
  if (hnd->hndData != hndData) {
    /* A bit more efficient than using x_ipcFreeData */
    x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)hndData);
  }
}

static void deregisterHandlerHnd(DISPATCH_PTR dispatch, HND_DATA_PTR hndData)
{
  MSG_PTR msg;
  
  (void)x_ipc_deregisterHnd(dispatch->orgId, hndData);
  
  msg = GET_MESSAGE(hndData->msgName);
  if (msg) {
    if ((msg->msgData->msg_class == BroadcastClass) &&
	(x_ipc_listLength(msg->hndList) == 0)){
      recordBroadcast(msg->msgData->name, FALSE);
    }
    if (msg->direct || msg->notifyHandlerChange)
      updateDirectHandlers(msg);
  }
  
  if (hndData->msgName) x_ipcFree((void *)hndData->msgName);
  if (hndData->hndName) x_ipcFree((void *)hndData->hndName);
  x_ipcFree((void *)hndData);
}

/***********************************************************************/

static void registerNamedFormHnd(DISPATCH_PTR dispatch, REG_DATA_PTR regData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dispatch)
#endif
  x_ipc_addFormatStringToTable((char *)regData->name, (char *)regData->format);
  
  /* Free the top level structure: Can the "format" string be freed as well? */
  x_ipcFree((void *)regData->name);
  x_ipcFree((void *)regData);
}

static void getNamedFormHnd(DISPATCH_PTR dispatch, char **namePtr)
{
  NAMED_FORMAT_PTR namedFormat;
  FORMAT_PTR format;

  namedFormat = (NAMED_FORMAT_PTR) x_ipc_hashTableFind(*namePtr,
						GET_M_GLOBAL(formatNamesTable));
  if (!namedFormat) {
    X_IPC_MOD_WARNING1("Unknown named format %s used.\n", *namePtr);
    format = NEW_FORMATTER();
    format->type = BadFormatFMT;
    format->formatter.f = NULL;
  } else {
    if (!namedFormat->parsed) {
      namedFormat->format =
	(FORMAT_PTR)ParseFormatString(namedFormat->definition);
      namedFormat->parsed = TRUE;
    }
    format = namedFormat->format;
  }

  centralReply(dispatch, (void *)&format);
  
  /* "name" can be freed since it is just used for lookup */
  /*  x_ipcFreeData(X_IPC_NAMED_FORM_QUERY,name);*/
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)namePtr);
}

/***********************************************************************/

void parseMsgFormats (MSG_PTR msg)
{
  if (!msg->parsedFormats) {
    switch(msg->msgData->msg_class) {
#ifndef NMP_IPC
    case PollingMonitorClass:
    case PointMonitorClass:
    case DemonMonitorClass:
      parseMonitorMsg(msg);
      break;
#endif
    default:
      parseMsg(msg);
    }
  }
}

static void msgInfoHnd(DISPATCH_PTR dispatch, MSG_ASK_PTR msgAsk)
{
  HND_PTR hnd;
  HND_KEY_TYPE hndKey;
  
  MSG_PTR msg;
  MSG_DATA_PTR msgData;
  
  msgData = NULL;
  msg = GET_MESSAGE(msgAsk->msgName);
  
  if (msg) {
    parseMsgFormats(msg);
    
    msgData = msg->msgData;
    
    if (msg->direct && DIRECT_MSG(msg)) {
      /* trigger establish direct connection */
      msgData->refId = -ABS(msgData->refId);
    }
    
    if (msgAsk->hndName) {
      /* 27-May-91: fedor: flag hnd cached msg info */
      hndKey.num = dispatch->orgId;
      hndKey.str = msgAsk->hndName;
      
      hnd = GET_HANDLER((char *)&hndKey);
      
      if (hnd) {
	hnd->msg = msg;
      } else {
	LOG_MESSAGE1("\nWARNING: msgInfoHnd: Missing handler: %s\n",
		    msgAsk->hndName);
      }
    }
  }
  
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)msgAsk);
  
  centralReply(dispatch, (void *)&msgData);
}

static int getMsg(const char *msgName, MSG_PTR msg, STR_LIST_PTR messages)
{
  if (!x_ipc_listMemberItem((void *)msg, GET_M_GLOBAL(Message_Ignore_Set)))
    x_ipc_strListPush(msgName, messages);
  return 1;
}

static void getMsgsHnd(DISPATCH_PTR dispatch, void *empty)
{
#ifdef UNUSED_PRAGMA
#pragma unused(empty)
#endif
  STR_LIST_PTR messages;
  
  messages = x_ipc_strListCreate();
  
  x_ipc_hashTableIterate((HASH_ITER_FN)getMsg, GET_C_GLOBAL(messageTable), messages);
  centralReply(dispatch, (void *)messages);
  x_ipc_strListFree(&messages,FALSE);
}

static void getMsgInfoHnd(DISPATCH_PTR dispatch, char *name)
{
  MSG_INFO_TYPE msgInfo;
  MSG_PTR msg;
  
  msgInfo.name = name;
  msg = GET_MESSAGE(name);
  if (msg != NULL) {
    msgInfo.msgFormat = (char *)msg->msgFormatStr;
    msgInfo.resFormat = (char *)msg->resFormatStr;
    msgInfo.msg_class = 
      msgInfo.msg_class = msg->msgData->msg_class;
    msgInfo.numberOfHandlers = x_ipc_listLength(msg->hndList);
    if (msgInfo.numberOfHandlers > 0) {
      msgInfo.resourceName = 
	((HND_PTR)(x_ipc_listFirst(msg->hndList)))->resource->name;
    } else {
      msgInfo.resourceName = NULL;
    }
  }
  centralReply(dispatch, (void *)&msgInfo);
  /*  x_ipcFreeData(X_IPC_MESSAGE_INFO_QUERY,name);*/
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)name);
}

/* If a handler for the message is registered, 
   returns the name of the first handler (along with the message name) */ 
static void hndInfoHnd(DISPATCH_PTR dispatch, MSG_ASK_PTR msgAsk)
{
  MSG_PTR msg;
  HND_PTR hnd;
  
  msg = GET_MESSAGE(msgAsk->msgName);
  msgAsk->hndName = (const char *)NULL;
  
  if (msg) {
    hnd = (HND_PTR)x_ipc_listFirst(msg->hndList);
    if (hnd) {
      msgAsk->hndName = hnd->hndData->hndName;
    }
  }
  
  centralReply(dispatch, (void *)msgAsk);
  
  /* Reset to NULL so that the free will not free the handler name */
  msgAsk->hndName = (const char *)NULL;
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)msgAsk);
}

/***********************************************************************/

static int32 waitModuleReady(DISPATCH_PTR dispatch)
{
  MODULE_PTR mod;
  BOOLEAN missing=FALSE;
  BOOLEAN found=FALSE;

  STR_LIST_ITERATE(dispatch->org->requiresList, resource,
		   { /* Scan throught the modules to find it. */
		     found = FALSE;
		     mod = (MODULE_PTR)x_ipc_listFirst(GET_M_GLOBAL(moduleList));
		     while (mod != NULL) {
		       if ((resource == NULL) || 
			   (x_ipc_strListMemberItem(resource, mod->providesList))) {
			 /* Found it. */
			 found = TRUE;
			 break; 
		       }
		       mod = (MODULE_PTR)x_ipc_listNext(GET_M_GLOBAL(moduleList));
		     }
		     if (!found) {
		       missing = TRUE;
		       break;
		     }
		   });
  return !missing;
}  

static int32 waitReleaseFunc(int32 *go, DISPATCH_PTR dispatch)
{
  if (waitModuleReady(dispatch)) {
    dispatch->org->wait = FALSE;
    centralReply(dispatch, (void *)go);
    resourceProcessPendingRes(dispatch->org->impliedResource);
    
    /* 9-Jul-91: Reid:
       The last received wait message dispatch is freed normally,
       since it is being handled.  The rest have to be freed here
       because they have been put on hold */
    dispatchFree(dispatch);
    return TRUE;
  } else {
    return FALSE;
  }
}  

static void WaitHnd(DISPATCH_PTR dispatch, void *data)
{
#ifdef UNUSED_PRAGMA
#pragma unused(data)
#endif
  int32 go = TRUE;
  
  GET_S_GLOBAL(waitTotalGlobal)++;
  if (GET_S_GLOBAL(waitTotalGlobal) < GET_S_GLOBAL(waitExpectedGlobal)) {
    x_ipc_listInsertItem((void *)dispatch, GET_S_GLOBAL(waitList));
  } else {
    /* Parse named formatters and message formats here, to save time when
       messages are called and to catch typos early */
    /* parseFormattersAndMessages(); */
    if (waitModuleReady(dispatch)) {
      dispatch->org->wait = FALSE;
      centralReply(dispatch, (void *)&go);
      resourceProcessPendingRes(dispatch->org->impliedResource);
    } else {
      x_ipc_listInsertItem((void *)dispatch, GET_S_GLOBAL(waitList));
    }
    x_ipc_listTestDeleteItemAll((LIST_ITER_FN) waitReleaseFunc,
			  (void *)&go, GET_S_GLOBAL(waitList));
  }
}

/**************************************************************************/

static void x_ipcCloseHnd(DISPATCH_PTR dispatch, void *dummy)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dummy)
#endif
  LOG1("x_ipcClose: Closed Connection Detected from: sd: %d:\n",
      dispatch->org->readSd);
  if (dispatch->org->modData) {
    LOG2("x_ipcClose: Closing %s on %s\n", dispatch->org->modData->modName,
	dispatch->org->modData->hostName);
  } else
    LOG("\n");
  
  x_ipcCloseMod((char *)(dispatch->org->modData->modName));
  /* x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, dummy);*/
}

static int modNameEq(char *modName, MODULE_PTR module)
{
  return(x_ipc_strKeyEqFunc(modName, module->modData->modName));
}

void x_ipcCloseMod(char *name)
{
  MODULE_PTR module=NULL;
  char moduleName[100], *modName;
  
  module = (MODULE_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN) modNameEq,
					 name, 
					 GET_M_GLOBAL(moduleList));
  if (module) {
    LOG1("close Module: Closing %s\n", name);
    
    FD_CLR((unsigned)module->readSd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    SHUTDOWN_SOCKET(module->readSd);
    if (module->readSd != module->writeSd) {
      SHUTDOWN_SOCKET(module->writeSd);
    }
    /* Save the module name -- it may get deleted in this function */
    strcpy(moduleName, module->modData->modName);
    removeConnection(module);

    /* Inform anyone who is interested about this disconnect */
    /* Needed because a pointer to an array is not the same as a pointer 
       to a string */
    modName = &moduleName[0];
    centralBroadcast(IPC_DISCONNECT_NOTIFY_MSG, &modName);
  } else {
    LOG1("Can not find %s to close.\n", name);
  }
}

/**************************************************************************/

static void classInfoHnd(DISPATCH_PTR dispatch, X_IPC_MSG_CLASS_TYPE *msg_class)
{
  CLASS_FORM_PTR classForm;
  
  classForm = GET_CLASS_FORMAT(msg_class);
  
  centralReply(dispatch, (void *)&classForm);
  
  /* Use simple free: "X_IPC_MSG_CLASS_TYPE" is an integer */
  x_ipcFree((void *)msg_class); 
}

/***********************************************************************/
#ifdef NMP_IPC
static void setMsgPriorityHnd(DISPATCH_PTR dispatch,
			      SET_MSG_PRIORITY_PTR setPriorityData)
{
  MSG_PTR msg;
  
  msg = x_ipc_findOrRegisterMessage(setPriorityData->msgName);
  msg->priority = setPriorityData->priority;

  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, setPriorityData);
}
#endif

/**************************************************************************/

static void directResHnd(DISPATCH_PTR dispatch, DIRECT_PTR direct)
{
  if (x_ipc_strKeyEqFunc(dispatch->org->modData->modName, direct->name)) {
    dispatch->org->port = direct->port;
    LOG_MESSAGE2("Direct Connection Established For: %s at %d\n",
		direct->name, direct->port);
  }
  else {
    LOG_MESSAGE1("Direct Connection: Not Made: %s did not originate call.\n",
		direct->name);
  }
  /* A bit more efficient than using x_ipcFreeData */
  /*  x_ipcFreeData(X_IPC_DIRECT_RES_INFORM,direct);*/
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)direct);
}

/**************************************************************************/

static void directInfoHnd(DISPATCH_PTR dispatch, char **name)
{
  MSG_PTR msg;
  HND_PTR hnd;
  DIRECT_INFO_TYPE directInfo;
  
  msg = GET_MESSAGE(*name);
  
  if (msg != NULL) {
    hnd = (HND_PTR)x_ipc_listFirst(msg->hndList);
    
    if (hnd != NULL) {
      directInfo.readSd = hnd->hndOrg->readSd;
      directInfo.writeSd = hnd->hndOrg->writeSd;
      directInfo.port = hnd->hndOrg->port;
      directInfo.intent = hnd->hndData->refId;
      directInfo.host = hnd->hndOrg->modData->hostName;
      directInfo.module = hnd->hndOrg->modData->modName;
      
      centralReply(dispatch, (void *)&directInfo);
    } else {
      centralNullReply(dispatch);
    } 
  } else {
    centralNullReply(dispatch);
  }
  
  /* "name" can be freed since it is just used for lookup */
  /* A bit more efficient than using x_ipcFreeData */
  /*  x_ipcFreeData(X_IPC_DIRECT_INFO_QUERY,name); */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, name);
}

/**************************************************************************/

static void directMsgHnd(DISPATCH_PTR dispatch, char **name)
{
  MSG_PTR msg;
  DIRECT_MSG_HANDLER_TYPE handlers[MAX_DIRECT_HANDLERS];
  DIRECT_MSG_TYPE directMsg;
  int num;
  
  msg = GET_MESSAGE(*name);

  directHandlersList(msg, &num, handlers);
  if (num > 0) {
    directMsg.numHandlers = num;
    directMsg.handlers = handlers;
    centralReply(dispatch, (void *)&directMsg);
  } else {
    centralNullReply(dispatch);
  }
  
  /* "name" can be freed since it is just used for lookup */
  /* A bit more efficient than using x_ipcFreeData */
  /*  x_ipcFreeData(X_IPC_DIRECT_MSG_QUERY,name); */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, name);
}

static void moduleConnectedHnd (DISPATCH_PTR dispatch, char **moduleName)
{
  int found = 0;
  MODULE_PTR module;

  for (module = (MODULE_PTR)x_ipc_listFirst(GET_M_GLOBAL(moduleList)); 
       module && !found;
       module = (MODULE_PTR)x_ipc_listNext(GET_M_GLOBAL(moduleList))) {
    found = x_ipc_strKeyEqFunc(module->modData->modName, *moduleName);
  }
  centralReply(dispatch, (void *)&found);
  
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, moduleName);
}

static void notifyHandlerChangeHnd (DISPATCH_PTR dispatch, char **msgName)
{
  MSG_PTR msg;

  msg = GET_MESSAGE(*msgName);
  if (!msg) {
    X_IPC_ERROR1("Cannot do handler notification before message defined (%s)",
		 *msgName);
  } else {
    if (!msg->direct && !msg->notifyHandlerChange) {
      centralRegisterVar(msg->msgData->name, DIRECT_MSG_FORMAT);
      centralIgnoreVarLogging(msg->msgData->name);
      updateDirectHandlers(msg);
    }
    msg->notifyHandlerChange = TRUE;
  }

  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, msgName);
}


/******************************************************************************
 *
 * FUNCTION: void serverMessagesInitialize()
 *
 * DESCRIPTION: 
 * Initializes the message/handler system for the server and modules.
 * Defines internal messages.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void serverMessagesInitialize(void)
{
  if (sizeof(int32) != sizeof(X_IPC_POINT_CLASS_TYPE)) {
    X_IPC_ERROR("INTERNAL ERROR: X_IPC_MSG_CLASS_TYPE is not of size `int'\n");
  } else {
    /* centralRegisterNamedFormatter("X_IPC_MSG_CLASS_TYPE", "int");*/
    /* 19-Aug-93: fedor:
       see search_hash_table_for_format for error in parseFmttrs.c */
    /* RTG - this causes problems with byte order between machines.
     * since these are just integers, use "int" for now.  The method
     * used to do printing should be fixed.  It should not have to scan
     * the hash table looking for structurally similar formats.
     */
    /* centralRegisterLengthFormatter("X_IPC_MSG_CLASS_TYPE", 
     *  sizeof(X_IPC_MSG_CLASS_TYPE));
     */
  }
  
#ifndef NMP_IPC
  centralRegisterNamedFormatter(X_IPC_MAP_NAMED, X_IPC_MAP_NAMED_FORMAT);
#endif

  centralRegisterQuery(X_IPC_MSG_INFO_QUERY, 
		       X_IPC_MSG_INFO_QUERY_FORMAT, 
		       X_IPC_MSG_INFO_QUERY_REPLY,
		       msgInfoHnd);
  Add_Message_To_Ignore(X_IPC_MSG_INFO_QUERY);
  
  centralRegisterQuery(X_IPC_MSG_INFO_QUERY_OLD,
		       X_IPC_MSG_INFO_QUERY_FORMAT,
		       X_IPC_MSG_INFO_QUERY_REPLY,
		       msgInfoHnd);
  Add_Message_To_Ignore(X_IPC_MSG_INFO_QUERY_OLD);
  
  centralRegisterQuery(X_IPC_MESSAGES_QUERY,
		       X_IPC_MESSAGES_QUERY_FORMAT,
		       X_IPC_MESSAGES_QUERY_REPLY,
		       getMsgsHnd);
  Add_Message_To_Ignore(X_IPC_MESSAGES_QUERY);
  
  centralRegisterQuery(X_IPC_MESSAGE_INFO_QUERY,
		       X_IPC_MESSAGE_INFO_QUERY_FORMAT,
		       X_IPC_MESSAGE_INFO_QUERY_REPLY,
		       getMsgInfoHnd);
  Add_Message_To_Ignore(X_IPC_MESSAGES_QUERY);

  centralRegisterQuery(X_IPC_HND_INFO_QUERY, 
		       X_IPC_HND_INFO_QUERY_FORMAT, 
		       X_IPC_HND_INFO_QUERY_REPLY, 
		       hndInfoHnd);
  Add_Message_To_Ignore(X_IPC_HND_INFO_QUERY);
  
  centralRegisterQuery(X_IPC_HND_INFO_QUERY_OLD, 
		       X_IPC_HND_INFO_QUERY_FORMAT, 
		       X_IPC_HND_INFO_QUERY_REPLY, 
		       hndInfoHnd);
  Add_Message_To_Ignore(X_IPC_HND_INFO_QUERY_OLD);
  
  centralRegisterInform(X_IPC_PROVIDES_INFORM,
			X_IPC_PROVIDES_INFORM_FORMAT,
			providesHnd);
  Add_Message_To_Ignore(X_IPC_PROVIDES_INFORM);
  
  centralRegisterInform(X_IPC_REQUIRES_INFORM,
			X_IPC_REQUIRES_INFORM_FORMAT,
			requiresHnd);
  Add_Message_To_Ignore(X_IPC_REQUIRES_INFORM);
  
  centralRegisterQuery(X_IPC_AVAILABLE_QUERY,
		       X_IPC_AVAILABLE_QUERY_FORMAT,
		       X_IPC_AVAILABLE_QUERY_REPLY,
		       availableHnd);
  Add_Message_To_Ignore(X_IPC_AVAILABLE_QUERY);
  
  centralRegisterQuery(X_IPC_CONNECT_QUERY,
		       X_IPC_CONNECT_QUERY_FORMAT,
		       X_IPC_CONNECT_QUERY_REPLY, 
		       NewModuleConnectHnd);
  Add_Message_To_Ignore(X_IPC_CONNECT_QUERY);
  
  centralRegisterQuery(X_IPC_CONNECT_QUERY_OLD,
		       X_IPC_CONNECT_QUERY_FORMAT,
		       X_IPC_CONNECT_QUERY_REPLY,
		       NewModuleConnectHnd);
  Add_Message_To_Ignore(X_IPC_CONNECT_QUERY_OLD);
  
  centralRegisterQuery(X_IPC_CLASS_INFO_QUERY,
		       X_IPC_CLASS_INFO_QUERY_FORMAT,
		       X_IPC_CLASS_INFO_QUERY_REPLY, 
		       classInfoHnd);
  Add_Message_To_Ignore(X_IPC_CLASS_INFO_QUERY);
  
  centralRegisterQuery(X_IPC_CLASS_INFO_QUERY_OLD,
		       X_IPC_CLASS_INFO_QUERY_FORMAT,
		       X_IPC_CLASS_INFO_QUERY_REPLY, 
		       classInfoHnd);
  Add_Message_To_Ignore(X_IPC_CLASS_INFO_QUERY_OLD);
  
#ifdef NMP_IPC
  centralRegisterInform(IPC_SET_MSG_PRIORITY_INFORM,
			IPC_SET_MSG_PRIORITY_INFORM_FORMAT,
			setMsgPriorityHnd);
#endif
  
  centralRegisterInform(X_IPC_REGISTER_MSG_INFORM,
			X_IPC_REGISTER_MSG_INFORM_FORMAT,
			registerMessageHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_MSG_INFORM);
  
  centralRegisterInform(X_IPC_REGISTER_MSG_INFORM_OLD,
			X_IPC_REGISTER_MSG_INFORM_FORMAT,
			registerMessageHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_MSG_INFORM_OLD);
  
  centralRegisterInform(X_IPC_REGISTER_HND_INFORM,
			X_IPC_REGISTER_HND_INFORM_FORMAT,
			registerHandlerHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_HND_INFORM);
  
  centralRegisterInform(X_IPC_REGISTER_HND_INFORM_OLD,
			X_IPC_REGISTER_HND_INFORM_FORMAT,
			registerHandlerHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_HND_INFORM_OLD);
  
  centralRegisterInform(X_IPC_DEREGISTER_HND_INFORM,
			X_IPC_DEREGISTER_HND_INFORM_FORMAT,
			deregisterHandlerHnd);
  Add_Message_To_Ignore(X_IPC_DEREGISTER_HND_INFORM_OLD);
  
  centralRegisterInform(X_IPC_DEREGISTER_HND_INFORM_OLD,
			X_IPC_DEREGISTER_HND_INFORM_FORMAT,
			deregisterHandlerHnd);
  Add_Message_To_Ignore(X_IPC_DEREGISTER_HND_INFORM);
  
  centralRegisterInform(X_IPC_NAMED_FORM_INFORM,
			X_IPC_NAMED_FORM_INFORM_FORMAT,
			registerNamedFormHnd);
  Add_Message_To_Ignore(X_IPC_NAMED_FORM_INFORM);
  
  centralRegisterInform(X_IPC_NAMED_FORM_INFORM_OLD,
			X_IPC_NAMED_FORM_INFORM_FORMAT,
			registerNamedFormHnd);
  Add_Message_To_Ignore(X_IPC_NAMED_FORM_INFORM_OLD);
  
  centralRegisterQuery(X_IPC_NAMED_FORM_QUERY,
		       X_IPC_NAMED_FORM_QUERY_FORMAT,
		       X_IPC_NAMED_FORM_QUERY_REPLY,
		       getNamedFormHnd);
  Add_Message_To_Ignore(X_IPC_NAMED_FORM_QUERY);
  
  centralRegisterQuery(X_IPC_WAIT_QUERY, 
		       X_IPC_WAIT_QUERY_FORMAT,
		       X_IPC_WAIT_QUERY_REPLY,
		       WaitHnd);
  Add_Message_To_Ignore(X_IPC_WAIT_QUERY);
  
  centralRegisterQuery(X_IPC_WAIT_QUERY_OLD, 
		       X_IPC_WAIT_QUERY_FORMAT,
		       X_IPC_WAIT_QUERY_REPLY,
		       WaitHnd);
  Add_Message_To_Ignore(X_IPC_WAIT_QUERY_OLD);
  
  centralRegisterInform(X_IPC_IGNORE_LOGGING_INFORM,
			X_IPC_IGNORE_LOGGING_INFORM_FORMAT,
			ignoreLoggingHnd);
  Add_Message_To_Ignore(X_IPC_IGNORE_LOGGING_INFORM);
  
  centralRegisterInform(X_IPC_IGNORE_LOGGING_INFORM_OLD,
			X_IPC_IGNORE_LOGGING_INFORM_FORMAT,
			ignoreLoggingHnd);
  Add_Message_To_Ignore(X_IPC_IGNORE_LOGGING_INFORM_OLD);
  
  centralRegisterInform(X_IPC_RESUME_LOGGING_INFORM,
			X_IPC_RESUME_LOGGING_INFORM_FORMAT,
			resumeLoggingHnd);
  Add_Message_To_Ignore(X_IPC_RESUME_LOGGING_INFORM);
  
  centralRegisterInform(X_IPC_RESUME_LOGGING_INFORM_OLD,
			X_IPC_RESUME_LOGGING_INFORM_FORMAT,
			resumeLoggingHnd);
  Add_Message_To_Ignore(X_IPC_RESUME_LOGGING_INFORM_OLD);
  
  centralRegisterInform(X_IPC_CLOSE_INFORM,
			X_IPC_CLOSE_INFORM_FORMAT,
			x_ipcCloseHnd);
  Add_Message_To_Ignore(X_IPC_CLOSE_INFORM);
  
  centralRegisterInform(X_IPC_CLOSE_INFORM_OLD, 
			X_IPC_CLOSE_INFORM_FORMAT,
			x_ipcCloseHnd);
  Add_Message_To_Ignore(X_IPC_CLOSE_INFORM_OLD);
  
  centralRegisterInform(X_IPC_DIRECT_RES_INFORM,
			X_IPC_DIRECT_RES_INFORM_FORMAT,
			directResHnd);
  Add_Message_To_Ignore(X_IPC_DIRECT_RES_INFORM);
  
  centralRegisterInform(X_IPC_DIRECT_RES_INFORM_OLD,
			X_IPC_DIRECT_RES_INFORM_FORMAT,
			directResHnd);
  Add_Message_To_Ignore(X_IPC_DIRECT_RES_INFORM_OLD);
  
  centralRegisterQuery(X_IPC_DIRECT_INFO_QUERY,
		       X_IPC_DIRECT_INFO_QUERY_FORMAT,
		       X_IPC_DIRECT_INFO_QUERY_REPLY,
		       directInfoHnd);
  Add_Message_To_Ignore(X_IPC_DIRECT_INFO_QUERY);
  
  centralRegisterQuery(X_IPC_DIRECT_INFO_QUERY_OLD,
		       X_IPC_DIRECT_INFO_QUERY_FORMAT,
		       X_IPC_DIRECT_INFO_QUERY_REPLY,
		       directInfoHnd);
  Add_Message_To_Ignore(X_IPC_DIRECT_INFO_QUERY_OLD);

  centralRegisterNamedFormatter(DIRECT_MSG_HANDLER_FORMAT_NAME,
				DIRECT_MSG_HANDLER_FORMAT);

  centralRegisterQuery(X_IPC_DIRECT_MSG_QUERY,
		       X_IPC_DIRECT_MSG_QUERY_FORMAT,
		       X_IPC_DIRECT_MSG_QUERY_REPLY,
		       directMsgHnd);
  Add_Message_To_Ignore(X_IPC_DIRECT_MSG_QUERY);

  centralRegisterBroadcastMessage(IPC_CONNECT_NOTIFY_MSG,
				  IPC_CONNECT_NOTIFY_FORMAT);
  Add_Message_To_Ignore(IPC_CONNECT_NOTIFY_MSG);

  centralRegisterBroadcastMessage(IPC_DISCONNECT_NOTIFY_MSG,
				  IPC_DISCONNECT_NOTIFY_FORMAT);
  Add_Message_To_Ignore(IPC_DISCONNECT_NOTIFY_MSG);

  centralRegisterQuery(IPC_MODULE_CONNECTED_QUERY,
		       IPC_MODULE_CONNECTED_QUERY_FORMAT,
		       IPC_MODULE_CONNECTED_QUERY_REPLY,
		       moduleConnectedHnd);
  Add_Message_To_Ignore(IPC_MODULE_CONNECTED_QUERY);

  centralRegisterInform(IPC_HANDLER_CHANGE_NOTIFY_MSG,
			IPC_HANDLER_CHANGE_NOTIFY_FORMAT,
			notifyHandlerChangeHnd);
  Add_Message_To_Ignore(IPC_HANDLER_CHANGE_NOTIFY_MSG);
}


/******************************************************************************
 *
 * FUNCTION: void printDataServerInitialize();
 *
 * DESCRIPTION: 
 * Sets print data routines for the central server.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void printDataServerInitialize(void)
{
  GET_M_GLOBAL(byteFormat) = ParseFormatString("byte");
  GET_M_GLOBAL(charFormat) = ParseFormatString("char");
  GET_M_GLOBAL(shortFormat) = ParseFormatString("short");
  GET_M_GLOBAL(intFormat) = ParseFormatString("int");
  GET_M_GLOBAL(longFormat) = ParseFormatString("long");
  GET_M_GLOBAL(floatFormat) = ParseFormatString("float");
  GET_M_GLOBAL(doubleFormat) = ParseFormatString("double");
  
  GET_M_GLOBAL(dPrintSTR_FN) = dPrintSTR;
  GET_M_GLOBAL(dPrintFORMAT_FN) = dPrintFORMAT;
#ifndef NMP_IPC
  GET_M_GLOBAL(dPrintMAP_FN) = mapPrint;
#endif
  GET_M_GLOBAL(dPrintX_IPC_FN) = dPrintX_IPC;
  GET_M_GLOBAL(dPrintCHAR_FN) = printChar;
  GET_M_GLOBAL(dPrintSHORT_FN) = printShort;
  GET_M_GLOBAL(dPrintLONG_FN) = printLong;
  GET_M_GLOBAL(dPrintINT_FN) = printInt;
  GET_M_GLOBAL(dPrintBOOLEAN_FN) = printBoolean;
  GET_M_GLOBAL(dPrintFLOAT_FN) = printFloat;
  GET_M_GLOBAL(dPrintDOUBLE_FN) = printDouble;
  GET_M_GLOBAL(dPrintBYTE_FN) = printByte;
  GET_M_GLOBAL(dPrintUBYTE_FN) = printUByte;
  GET_M_GLOBAL(dPrintTWOBYTE_FN) = printTwoByte;

  GET_M_GLOBAL(dPrintUSHORT_FN) = printUShort;
  GET_M_GLOBAL(dPrintUINT_FN) = printUInt;
  GET_M_GLOBAL(dPrintULONG_FN) = printULong;
}


/******************************************************************************
 *
 * FUNCTION: int32 serverInitialize(expectedMods)
 *
 * DESCRIPTION:
 * Initialize the server. Creates a socket, looks up the server port
 * number and binds the socket to that port number, and listens to the
 * socket. The machine name for the central server is assumed to be the
 * machine on which it is running and so it needs not be specified.
 * x_ipcServerModGlobal is initialized to the central server module.
 *
 * INPUTS:
 * int32 expectedMods;
 *
 * OUTPUTS:
 * Returns FALSE if there was an error detected, TRUE otherwise.
 * If no error was detected serverMod is initialized to the server module.
 *
 *****************************************************************************/

int serverInitialize(int expectedMods)
{
  int sd;
  int32 port;
  
#ifndef SERVER_PORT
  struct servent *sp;
#endif
  
#ifdef macintosh
    initGUSI();
#endif

#ifdef SERVER_PORT
  port = GET_S_GLOBAL(serverPortGlobal);
#else
  if ((sp = getservbyname(SERVER_NAME, PROTOCOL_NAME)) == (char *)NULL)
    return FALSE;
  
  GET_S_GLOBAL(serverPortGlobal) = port = sp->s_port;
#endif
  
  GET_M_GLOBAL(moduleList) = x_ipc_listCreate();
  GET_S_GLOBAL(moduleTable) = x_ipc_hashTableCreate(11, (HASH_FN) modHashFunc,
					      (EQ_HASH_FN) modKeyEqFunc);
  
  GET_S_GLOBAL(waitExpectedGlobal) = expectedMods;
  
  if (GET_S_GLOBAL(x_ipcServerModGlobal) != NULL)
    x_ipcFree((char *) GET_S_GLOBAL(x_ipcServerModGlobal));
  
  GET_S_GLOBAL(x_ipcServerModGlobal) = serverModCreate();
  
  /* Start up the TCP/IP socket for accepting connections */
  if (!x_ipc_listenAtPort(&port, &sd)) {
    X_IPC_MOD_WARNING("Error: unable to bind TCP/IP socket for listening\n");
    return FALSE;
  } else {
    serverModListen(sd);
    GET_C_GLOBAL(listenPort) = sd;
  }
  
#if !defined(NO_UNIX_SOCKETS) || defined(VX_PIPES)
  /* Start up the unix socket for accepting connections */
  if (!x_ipc_listenAtSocket(port, &sd)) {
    X_IPC_MOD_WARNING("Error: unable to open socket/pipe for listening\n");
    return FALSE;
  } else {
    serverModListen(sd);
    GET_C_GLOBAL(listenSocket) = sd;
  }
#endif
  
  /* 29-Aug-90: fedor: blah! */
  GET_C_GLOBAL(serverRead) = CENTRAL_SERVER_ID;
  GET_C_GLOBAL(serverWrite) = CENTRAL_SERVER_ID;
  
  printDataServerInitialize();
  
  classInitialize();
  
  resourceInitialize();
  
  serverMessagesInitialize();
  
  GET_S_GLOBAL(dispatchTable) = x_ipc_idTableCreate("Dispatch Table", 
					      DISPATCH_TABLE_SIZE);
  
#ifndef NMP_IPC
  taskTreeInitialize();
  
  monitorInitialize();
#endif
  
  globalVarInitialize();
  
  tapInitialize();
  
#ifndef NMP_IPC
  exceptionIntialize();
#endif
  
  GET_S_GLOBAL(waitList) = x_ipc_listCreate();
  
  GET_M_GLOBAL(logList)[0] = &GET_S_GLOBAL(terminalLog);
  GET_M_GLOBAL(logList)[1] = &GET_S_GLOBAL(fileLog);
  GET_M_GLOBAL(logList)[2] = NULL;
  
  Start_File_Logging();
  Start_Terminal_Logging();
  
  LOG2("Expecting %d on port %d\n",
      expectedMods, GET_S_GLOBAL(serverPortGlobal));
  
  return TRUE;
}


/******************************************************************************
 *
 * FUNCTION: void serverShutdown()
 *
 * DESCRIPTION: 
 * Closes the sockets and removes the unix socket link.
 *
 * INPUTS:
 *
 * OUTPUTS: 
 * Returns FALSE if there was an error detected, TRUE otherwise.
 * If no error was detected serverMod is initialized to the server module.
 *
 *****************************************************************************/

void serverShutdown(void)
{
  int32 i;
  int32 maxDevFd = GET_C_GLOBAL(maxConnection);
  for(i=0; i<=maxDevFd; i++) {
    if (FD_ISSET(i,&GET_C_GLOBAL(x_ipcListenMaskGlobal))) {
      CLOSE_SOCKET(i);
    }
  }
#ifndef NO_UNIX_SOCKETS
  x_ipc_closeSocket(GET_S_GLOBAL(serverPortGlobal));
#endif
#if defined(DBMALLOC)
  clearTaskTree();
  globalSFree();
  x_ipc_globalMFree();
  malloc_dump(1);
#endif
}


/******************************************************************************
 *
 * FUNCTION: BOOLEAN handleDataMsgRecv(sd, module)
 *
 * DESCRIPTION: 
 * This defines an iterate function to call dataRecvMsg 
 * on each module that has input pending.
 *
 * INPUTS:
 * fd_set readMask;
 * MODULE_PTR module;
 *
 * OUTPUTS: TRUE - continue to next module.
 *
 *****************************************************************************/

static BOOLEAN handleDataMsgRecv(int sd, MODULE_PTR module)
{
  DATA_MSG_PTR dataMsg = NULL;
  
  if (module == NULL) {
    LOG1("ERROR: handleDataMsgRecv: No module for %d\n", sd);
    return FALSE;
  }

  GET_S_GLOBAL(startTime) = x_ipc_timeInMsecs();
  
  switch(x_ipc_dataMsgRecv(module->readSd, &dataMsg, 0, NULL, 0)) {
  case StatOK:
    
    GET_S_GLOBAL(byteSize) = dataMsg->classTotal + dataMsg->msgTotal;
    GET_S_GLOBAL(byteTotal) = GET_S_GLOBAL(byteTotal) + GET_S_GLOBAL(byteSize);
    
    recvMessageBuild(module, dataMsg);
    break;
  case StatError:
    perror("ERROR: handleDataMsgRecv ");
    /* fall through to close module */
  case StatEOF:
    /* 28-Aug-90: fedor: recycle dataMsg */
    LOG_STATUS1("Closed Connection Detected from: sd: %d: \n", module->readSd);
    /* 28-Aug-90: fedor: shouldn't need this test. */
    if (module->modData) {
      LOG2(" Closing %s on %s\n", module->modData->modName,
	  module->modData->hostName);
    } else
      LOG("\n");
    x_ipcCloseMod((char *)(module->modData->modName));
    break;
  case StatSendEOF:
  case StatSendError:
  case StatRecvEOF:
  case StatRecvError:
#ifndef TEST_CASE_COVERAGE
  default:
#endif
    X_IPC_ERROR("ERROR: handleDataMsgRecv: Unknown Status:");
  }
  
  GET_S_GLOBAL(endTime) = x_ipc_timeInMsecs();
  
  GET_S_GLOBAL(diffTime) = GET_S_GLOBAL(endTime) - GET_S_GLOBAL(startTime);
  
  GET_S_GLOBAL(totalMsg) = GET_S_GLOBAL(totalMsg) + GET_S_GLOBAL(diffTime);
  
  GET_S_GLOBAL(mTotal)++;
  
  GET_S_GLOBAL(avgTime) = (long)(GET_S_GLOBAL(totalMsg)/GET_S_GLOBAL(mTotal));
  
  GET_S_GLOBAL(avgSize) = GET_S_GLOBAL(byteTotal)/GET_S_GLOBAL(mTotal);
  LogHandleSummary( GET_S_GLOBAL(mTotal), GET_S_GLOBAL(msgPer),
		   GET_S_GLOBAL(monPer), GET_S_GLOBAL(waitPer),
		   GET_S_GLOBAL(byteSize), GET_S_GLOBAL(diffTime),
		   GET_S_GLOBAL(avgSize), GET_S_GLOBAL(avgTime));
  return TRUE;
}


/******************************************************************************
 *
 * FUNCTION: int32 acceptConnection(sd, module)
 *
 * DESCRIPTION: Accept a new connection.
 *
 * INPUTS:
 * fd_set readMask;
 * MODULE_PTR module;
 *
 * OUTPUTS: TRUE - continue to next module.
 *
 *****************************************************************************/

#ifdef ACCESS_CONTROL
#include <tcpd.h>
extern int hosts_ctl(char *, char *, char *, char *);

int allow_severity = 5;
int deny_severity  = 5;

/* Check the /etc/hosts.deny and /etc/hosts.allow files to see if the
   module is allowed to connect.  Return NULL if OK, o/w return the hostname
   of the offending module */
static const char *accessibleConnection (int moduleSd)
{
  struct sockaddr_in peer;
  int addrlen=sizeof(peer);
  struct hostent *client;
  char *client_machine, *client_hostnum;

  getpeername(moduleSd, &peer, &addrlen);
  /* Patch by Nick Roy, 11/02 */
  if (peer.sin_family != AF_INET) {
    return NULL;
  }
  client = gethostbyaddr((char *)&peer.sin_addr, 
                         sizeof(peer.sin_addr), AF_INET);
  client_hostnum = inet_ntoa(peer.sin_addr);
  if (!strcmp(client_hostnum, "0.0.0.0"))
    client_machine = "localhost";
  else if (client == 0)
    /* We couldn't resolve the IP number into a machine name, so just
       use its IP number as its name. */
    client_machine = client_hostnum;
  else
    client_machine = client->h_name;

  /* Do the lookup in /etc/hosts.deny /etc/hosts.allow
   * The server_name is a global string that is associated with this daemon,
   * and matches some entry in the hosts.allow/deny files.   
   */
  return (hosts_ctl("central", client_machine, client_hostnum, 
		    STRING_UNKNOWN) == 0 ? client_machine : NULL);
}
#endif

#ifndef VX_PIPES
static BOOLEAN acceptConnection(int sd)
{
  int moduleSd;
  int32 value=1;
  
  /* server open a new connection */
#ifndef VXWORKS
  moduleSd = accept(sd, (struct sockaddr *)NULL, (socklen_t *)NULL);
#else
  {
    struct sockaddr addr;
    int len = sizeof(struct sockaddr);
    moduleSd = accept(sd,  &addr, &len);
  }
#endif
  if (moduleSd <= 0) {
    X_IPC_MOD_WARNING("Accept Failed\n");
    return FALSE;
  }

#ifdef ACCESS_CONTROL
  {
    const char *client_machine = accessibleConnection(moduleSd);

    if (client_machine != NULL) {
      LOG1("WARNING: connection rejected from %s\n", client_machine);
      close(moduleSd);
      return FALSE;
    }
  }
#endif

  addConnection(moduleSd, moduleSd, &(GET_S_GLOBAL(newModDataGlobal)));
  /* This next line will fail if the connection is a unix socket, but 
   * it does not cause any problems
   */
#ifndef __TURBOC__
  if ( setsockopt(moduleSd, IPPROTO_TCP, TCP_NODELAY, 
		  (char *)&value, sizeof(int32))
       == 0) { /* it is a tcp/ip socket. */
    value = TCP_SOCKET_BUFFER;
  } else { /* it is a unix socket. */
    value = UNIX_SOCKET_BUFFER;
  }
  setsockopt(moduleSd, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(int));
  setsockopt(moduleSd, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof(int));
#endif
  return TRUE;
}

#else

static BOOLEAN acceptConnection(int sd)
{
  int moduleReadSd, moduleWriteSd;
  int32 value=1;
  MODULE_PTR module;
  char socketName[80];
  char portNum[80];
  char modName[80];
  
  if (sd == GET_C_GLOBAL(x_ipcListenSocket)) {
    /* This is the vx pipe, just read the names of the pipes to open.  */
    x_ipc_readNBytes(GET_C_GLOBAL(x_ipcListenSocket),modName, 80);

    bzero(portNum, sizeof(portNum));
    bzero(socketName, sizeof(socketName));
    snprintf(portNum, sizeof(portNum)-1, "%d", GET_S_GLOBAL(serverPortGlobal));
    snprintf(socketName, sizeof(socketName)-1, VX_PIPE_NAME, portNum, modName);
    moduleReadSd = open(socketName, O_RDONLY, 0644);
    if (moduleReadSd < 0) {
      X_IPC_MOD_ERROR("Open pipe Failed\n");
      return FALSE;
    }
    
    snprintf(socketName, sizeof(socketName)-1, VX_PIPE_NAME, modName, portNum);
    moduleWriteSd = open(socketName, O_WRONLY, 0644);
    if (moduleWriteSd < 0) {
      X_IPC_MOD_ERROR("Open pipe Failed\n");
      return FALSE;
    }
    
    x_ipc_writeNBytes(moduleWriteSd,portNum, 80);
    module = addConnection(moduleReadSd, moduleWriteSd, 
			   &(GET_S_GLOBAL(newModDataGlobal)));
    
  } else if (sd == GET_C_GLOBAL(x_ipcListenPort)) {
    /* server open a new connection */
#ifndef VXWORKS
    moduleReadSd = accept(sd, (struct sockaddr *)NULL, (int *)NULL);
#else
    {
      struct sockaddr addr;
      int len = sizeof(struct sockaddr);
      moduleReadSd = accept(sd,  &addr, &len);
    }
#endif
    if (moduleReadSd <= 0) {
      X_IPC_MOD_WARNING("Accept Failed\n");
      return FALSE;
    }
    module = addConnection(moduleReadSd, moduleReadSd,
			   &(GET_S_GLOBAL(newModDataGlobal)));
    /* This next line will fail if the connection is a unix socket, but 
     * it does not cause any problems
     */
    if ( setsockopt(moduleReadSd, IPPROTO_TCP, TCP_NODELAY, 
		    (char *)&value, sizeof(int32))
	 == 0) { /* it is a tcp/ip socket. */
      value = TCP_SOCKET_BUFFER;
    } else { /* it is a unix socket. */
      value = UNIX_SOCKET_BUFFER;
    }
    setsockopt(moduleReadSd, SOL_SOCKET, SO_SNDBUF,
	       (char *)&value, sizeof(int));
    setsockopt(moduleReadSd, SOL_SOCKET, SO_RCVBUF,
	       (char *)&value, sizeof(int));
  }
  return TRUE;
}
#endif /* VXWORKS */


#ifdef NMP_IPC
static void displayMessages(void)
{
  int32 i;
  DISPATCH_PTR dispatch;
  
  Log("Active and Pending Messages:\n");
  for (i=0; i<GET_S_GLOBAL(dispatchTable)->currentSize; i++) {
    dispatch = (DISPATCH_PTR)idTableItem(i, GET_S_GLOBAL(dispatchTable));
    if (dispatch && dispatch->status != UnallocatedDispatch) {
      Log("  %s {%d}: %s\n", dispatch->msg->msgData->name, dispatch->locId,
	  GET_S_GLOBAL(nodeStatusNames)[(int32)dispatch->status]);
    }
  }
}
#endif

/******************************************************************************
 *
 * FUNCTION: void stdinHnd(void)
 *
 * DESCRIPTION: 
 * Handle input from standard in.
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

static void stdinHnd(void)
{
  char inputLine[81];
  
  bzero(inputLine,sizeof(inputLine));
  fgets(inputLine,80,stdin);
  
  /* Kill the \n at the end of the line */
  inputLine[strlen(inputLine)-1] = '\0';
  
  if (strstr(inputLine, "quit")) {
    x_ipcStats(stderr);
    /*x_ipc_idTablePrintInfo(GET_S_GLOBAL(dispatchTable));*/
    X_IPC_MOD_WARNING("Central Quit\n");
    End_File_Logging();
    serverShutdown();
#ifdef _WINSOCK_
    WSACleanup();
    printf("Socket cleaned up.");
#endif /* Unload Winsock DLL */
    exit(0);
  } else if (strstr(inputLine, "dump")) {
#if defined(DBMALLOC)
    malloc_dump(1);
#else
    X_IPC_MOD_WARNING(" dump not available, recompile with DBMALLOC\n");
#endif
  } else if (strstr(inputLine, "display")) {
#ifdef NMP_IPC
    displayMessages();
#else
    showTaskTree();
  } else if (strstr(inputLine, "kill")) {
    clearTaskTree();
#endif
  } else if (strstr(inputLine, "status")) {
    showResourceStatus();
  } else if (strstr(inputLine, "close")) {
    char *modName = inputLine;
    for(;*modName!= '\0';modName++)
      if(*modName == '\n') *modName = '\0';
    modName = (char *)(strstr(inputLine, "close") + strlen("close"));
    /* Really need to parse the line, but just skip spaces for now.   */
    for(;isspace((int)*modName); modName++) {};
    x_ipcCloseMod(modName);
  } else if (strstr(inputLine, "unlock")) {
    char *modName = inputLine;
    for(;*modName!= '\0';modName++)
      if(*modName == '\n') *modName = '\0';
    modName = (char *)(strstr(inputLine, "unlock") + strlen("unlock"));
    /* Really need to parse the line, but just skip spaces for now.   */
    for(;isspace((int)*modName); modName++) {};
    unlockResource(modName);
  } else if (strstr(inputLine, "help") ||
	     checkOccurrence(inputLine, "?")) {	
    displayHelp();
  } else if (strstr(inputLine, "memory")) {	
    x_ipcStats(stdout);
  } else if (strstr(inputLine, "-")){
    parseOpsFromStr(inputLine, NULL, TRUE);
    Start_File_Logging();
    Start_Terminal_Logging();
  } else if (strlen(inputLine) >0) {
    printf("Unrecognized Command :%s\n",inputLine);
    displayHelp();
  }
  if (GET_S_GLOBAL(listenToStdin))
    printPrompt();
}


/******************************************************************************
 *
 * FUNCTION: void listenLoop()
 *
 * DESCRIPTION: 
 * Central Clearing House for Incoming Messages.
 * Grabs an available message and acts on it.
 * Loops until quit. Need to define quit.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 * NOTES:
 * 20-Apr-89: fedor: x_ipc_dataMsgRecv should not return StatOK with a NULL dataMsg.
 * need to insure dataMsg values 
 *
 * 27-Aug-90: fedor: need to put timing info back.
 *
 *****************************************************************************/

static int32 modSdEq(int sd, MODULE_PTR module)
{
  return((sd == module->readSd) ||(sd == module->writeSd));
}

void listenLoop(void)
{
  int32 stat;
  fd_set readMask;
  
  /******************/
  
  GET_S_GLOBAL(totalMsgRun) = 0;
  
  GET_S_GLOBAL(startTime) = GET_S_GLOBAL(endTime) = GET_S_GLOBAL(mTotal) = 
	GET_S_GLOBAL(diffTime) = GET_S_GLOBAL(avgTime) = 0;
  
  GET_S_GLOBAL(startLoop) = GET_S_GLOBAL(endLoop) = 
    GET_S_GLOBAL(startMon) = GET_S_GLOBAL(endMon) = 0;
  GET_S_GLOBAL(totalMsg) = GET_S_GLOBAL(totalLoop) = GET_S_GLOBAL(totalMon) = 0.0;

  GET_S_GLOBAL(totalWait) = 0;
  
  GET_S_GLOBAL(avgSize) = GET_S_GLOBAL(byteTotal) = 0;
  
  GET_S_GLOBAL(msgPer) = GET_S_GLOBAL(monPer) = GET_S_GLOBAL(waitPer) = 0;
  
  /**************/
  
  /* x_ipc_dataMsgDisplayStats();*/
  
  for(;;) {
    
    readMask = (GET_C_GLOBAL(x_ipcConnectionListGlobal));
    
    /* Also listen for commands on standard input */
    if (GET_S_GLOBAL(listenToStdin))
      FD_SET(fileno(stdin), &readMask);
    
    do {
      stat = select(FD_SETSIZE, &readMask, (fd_set *)NULL, (fd_set *)NULL,
		    NULL);
    }
#ifdef _WINSOCK_
    while (stat == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);
#else
    while (stat < 0 && errno == EINTR);
#endif
    
#ifdef _WINSOCK_
    if (stat == SOCKET_ERROR)
#else
      if (stat < 0)
#endif
	{
#if defined(VXWORKS)
/*	  printErrno(errno); */
#elif defined(_WINSOCK_)
	  printf("%d\n",readMask.fd_count);
	  X_IPC_ERROR1("Internal Error: select winsock error in listenLoop %d",WSAGetLastError());
#endif
	  X_IPC_ERROR1("Internal Error: select error in listenLoop %d",errno);
	}
    
    if (FD_ISSET(0,&readMask)) {
      /* Handle input on stdin */
      stdinHnd();
    }
    
    GET_S_GLOBAL(byteSize) = 0;
    
    GET_S_GLOBAL(startLoop) = x_ipc_timeInMsecs();
    
    GET_S_GLOBAL(startMon) = GET_S_GLOBAL(startLoop);
    
#ifndef NMP_IPC
    monitorPoll();
#endif
    
    GET_S_GLOBAL(endMon) = x_ipc_timeInMsecs();
    
    GET_S_GLOBAL(totalMon) = (GET_S_GLOBAL(totalMon) +
			      (GET_S_GLOBAL(endMon) - GET_S_GLOBAL(startMon)));
    
    /* Should really use devUtils, but for now, just loop over
     * the open devices and call the correct routines.
     * The call to find the member of the modules list with the correct
     * file id is wasteful.  The modules should be stored in an array rather
     * than a list.
     */
    if (stat > 0)
      {
	int32 i;
	int32 maxDevFd = GET_C_GLOBAL(maxConnection);
	for(i=0; i<=maxDevFd; i++) {
	  if (i != fileno(stdin) && FD_ISSET(i,&readMask)) {
	    if (FD_ISSET(i,&GET_C_GLOBAL(x_ipcListenMaskGlobal))) {
	      /*	It is a new connection requrest. */
	      acceptConnection(i);
	    } else {
	      /* It is a message to be handled.  */
	      handleDataMsgRecv(i,
				(MODULE_PTR)
				x_ipc_listMemReturnItem((LIST_ITER_FN) modSdEq,
						  (void *)(long)i,
						  GET_M_GLOBAL(moduleList)));
	    }
	  }
	}
      }
    /*  (void)x_ipc_listIterateFromFirst((LIST_ITER_FN) iterateDataMsgRecv,*/
    /*	      (void *)&readMask, GET_M_GLOBAL(moduleList));*/
    GET_S_GLOBAL(endLoop) = x_ipc_timeInMsecs();
    
    GET_S_GLOBAL(totalLoop) = GET_S_GLOBAL(totalLoop) +
      (GET_S_GLOBAL(endLoop) - GET_S_GLOBAL(startLoop));
    
    GET_S_GLOBAL(totalWait) = GET_S_GLOBAL(totalLoop) - GET_S_GLOBAL(totalMsg)
      - GET_S_GLOBAL(totalMon);
    
    
    /* Conditional added by Reid 1/28/93;
       bug found while testing 486 version */
    
    if (GET_S_GLOBAL(totalLoop) > 0) {
      GET_S_GLOBAL(msgPer) = GET_S_GLOBAL(totalMsg) / GET_S_GLOBAL(totalLoop)
	* 100;
      GET_S_GLOBAL(monPer) = GET_S_GLOBAL(totalMon) / GET_S_GLOBAL(totalLoop)
	* 100;
      GET_S_GLOBAL(waitPer) = GET_S_GLOBAL(totalWait) /GET_S_GLOBAL(totalLoop)
	* 100;
    }
  }
}


/*****************************************************************************
 *
 * FUNCTION: void recordBroadcast(const char *name)
 *
 * DESCRIPTION:
 *
 * INPUTS: message name.
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void recordBroadcast(const char *name, BOOLEAN active)
{
  if (active) {
    if (!x_ipc_strListMemberItem(name, GET_C_GLOBAL(broadcastMsgs))) {
      x_ipc_strListPushUnique(strdup(name), GET_C_GLOBAL(broadcastMsgs));
      centralSetVar(X_IPC_BROADCAST_MSG_VAR, 
		    (char *)GET_C_GLOBAL(broadcastMsgs));
    }
  } else if (GET_C_GLOBAL(broadcastMsgs) &&
	     x_ipc_strListMemberItem(name, GET_C_GLOBAL(broadcastMsgs))) {
    x_ipc_strListDeleteItem(name, GET_C_GLOBAL(broadcastMsgs), TRUE);
    centralSetVar(X_IPC_BROADCAST_MSG_VAR, 
		  (char *)GET_C_GLOBAL(broadcastMsgs));
  }
}


/*****************************************************************************
 * 
 * FUNCTION: void cleanBroadcast()
 *
 * DESCRIPTION: 
 *
 * INPUTS: message name.
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void cleanBroadcast(void)
{
  MSG_PTR msg;
  
  STR_LIST_ITERATE(GET_C_GLOBAL(broadcastMsgs), msgName,
		   { if (msgName != NULL) {
		       msg = GET_MESSAGE(msgName);
		       if ((msg != NULL) && (x_ipc_listLength(msg->hndList) == 0)) {
			 recordBroadcast(msgName, FALSE);
			 STR_LIST_REDO(GET_C_GLOBAL(broadcastMsgs));
		       }}});
}

/*****************************************************************************
 * 
 * FUNCTION: void updateDirectHandlers(msg)
 *
 * DESCRIPTION: If direct messages, set the global var to update
 *              the list of direct handlers.
 *
 * INPUTS: msg
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void updateDirectHandlers(const MSG_PTR msg)
{
  DIRECT_MSG_TYPE directMsg;
  DIRECT_MSG_HANDLER_TYPE handlers[MAX_DIRECT_HANDLERS];
  int num;

  directHandlersList(msg, &num, handlers);
  directMsg.numHandlers = num;
  directMsg.handlers = handlers;
    
  if (!x_ipc_hashTableFind(msg->msgData->name, GET_S_GLOBAL(varTable)))
    centralRegisterVar(msg->msgData->name, DIRECT_MSG_FORMAT);

  centralSetVar(msg->msgData->name, (char *)&directMsg);
}

static void directHandlersList (const MSG_PTR msg, int *num,
				DIRECT_MSG_HANDLER_TYPE handlers[])
{
  HND_PTR hnd;
  DIRECT_MSG_HANDLER_PTR directHandler;
  
  *num = 0;
  if (msg != NULL) {
    if (x_ipc_listLength(msg->hndList) > MAX_DIRECT_HANDLERS) {
      LOG1("Too many handlers for %s; Must send through central", 
	  msg->msgData->name);
    } else {
      hnd = (HND_PTR)x_ipc_listFirst(msg->hndList);
      while (hnd) {
	directHandler = &(handlers[*num]);
	directHandler->intent = hnd->hndData->refId;
	directHandler->readSd = directHandler->writeSd = 0;
	directHandler->port = hnd->hndOrg->port;
	directHandler->host = hnd->hndOrg->modData->hostName;
	directHandler->module = hnd->hndOrg->modData->modName;
	(*num)++;
	hnd = (msg->msgData->msg_class != BroadcastClass ? NULL :
	       (HND_PTR)x_ipc_listNext(msg->hndList));
      }
    }
  }
}

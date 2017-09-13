/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: comModule.c
 *
 * ABSTRACT:
 * 
 * Module Communications
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY 
 *
 * $Log: comModule.c,v $
 * Revision 2.23  2011/08/16 16:01:52  reids
 * Adding Python interface to IPC, plus some minor bug fixes
 *
 * Revision 2.22  2011/04/21 18:17:48  reids
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
 * Revision 2.21  2010/12/17 19:20:23  reids
 * Split IO mutex into separate read and write mutexes, to help minimize
 *   probability of deadlock when reading/writing very big messages.
 * Fixed a bug in multi-threaded version where a timeout is not reported
 *   correctly (which could cause IPC_listenClear into a very long loop).
 *
 * Revision 2.20  2009/11/09 17:51:52  reids
 * Fixed invocation of timers -- previously, there were occasions where the
 *   "wait time" provided by IPC_listen was not being honored when there were
 *   timers being fired within the call.
 *
 * Revision 2.19  2009/09/04 19:13:32  reids
 * Remove memory leak
 *
 * Revision 2.18  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.17  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.16  2004/06/09 18:24:16  reids
 * Fixed bug related to starting multiple centrals in the threaded version.
 *
 * Revision 2.15  2003/03/12 05:13:53  trey
 * fixed bug with IPC_connect() clobbering previous IPC_subscribeFD() calls
 *
 * Revision 2.14  2003/03/11 23:39:00  trey
 * Event handling while not connected to central:
 *   This is an experimental feature, disabled by default.  Enable it
 *   by compiling with 'gmake IPC_ALLOW_DISCONNECTED_EVENT_HANDLING=1 install'.
 *   When this feature is enabled, you can call event handling functions
 *   such as IPC_listen() and IPC_dispatch() while not connected to the
 *   central server (you must call IPC_initialize() first, though).  Of
 *   course, no IPC messages can be received, but events registered using
 *   IPC_addTimer() and IPC_subscribeFD() will be handled.
 *
 * Silently close connection to central in child process:
 *   If you fork() and exec() in a process after connecting to IPC, you
 *   should call the new function x_ipcCloseInternal(0) in the child before
 *   the exec() call.  This silently closes the child's copy of the pipe to
 *   central.  Then when the parent exits, central will correctly be
 *   notified that the module has disconnected.
 *
 *   This avoids a rare failure mode where the child outlives the parent,
 *   the parent shuts down using exit(), and the child's still-open copy of
 *   the pipe to central keeps central from finding out that the module is
 *   disconnected.
 *
 * Revision 2.13  2003/03/07 19:45:45  trey
 * added ability to silently close child copies of the connection to the central server
 *
 * Revision 2.12  2003/02/13 20:39:45  reids
 * Fixed bug relating to initializing context data structure.
 *
 * Revision 2.11  2002/06/25 16:45:25  reids
 * Added casts to satisfy compiler.
 *
 * Revision 2.10  2002/01/03 20:52:10  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.9  2001/05/30 19:37:59  reids
 * Fixed a bug in the way timers interacted with query/reply messages.
 *   In particular, timers can no longer run recursively.
 *
 * Revision 2.8  2001/01/31 17:54:10  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.7  2000/12/11 16:09:02  reids
 * Fixed bug in triggering timers.
 *
 * Revision 2.6  2000/08/14 21:28:33  reids
 * Added support for making under Windows.
 *
 * Revision 2.5  2000/08/07 21:17:35  reids
 * Fixed for compiling under IRIX using CC.
 *
 * Revision 2.4  2000/07/27 16:59:09  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.3  2000/07/19 20:55:49  reids
 * Added handling for SIGPIPE; fixed handling of direct messages when pipe
 *   is broken.
 * Also, need to reset handlers (maybe??) since, according to the man page for
 *  "signal", the Linux version resets the handler to its default value each
 *  time the signal is raised.
 *
 * Revision 2.2  2000/07/03 17:03:22  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.14  1997/03/07 17:49:30  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.2.2.13  1997/01/27 20:09:11  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.11  1997/01/16 22:15:42  reids
 * Check that IPC initialized before using global var.
 *
 * Revision 1.2.2.10  1997/01/11 01:20:47  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.2.9.4.3  1997/01/08 18:41:22  reids
 * Added function "killModule" to cleanly shut down vxworks tasks running IPC.
 *
 * Revision 1.2.2.9.4.2  1996/12/27 19:25:54  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.2.2.9.4.1  1996/12/24 14:41:30  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.2.2.9  1996/12/18 15:09:43  reids
 * Changed logging code, and commented out unneeded functions, to remove
 *   VxWorks dependence on varargs
 * Defined common macros to clean up code
 * Shut down modules more gracefully
 *
 * Revision 1.2.2.8  1996/10/29 14:52:02  reids
 * Make "byteOrder" and "alignment" vars available to C, not just LISP.
 *
 * Revision 1.2.2.7  1996/10/24 16:19:59  reids
 * Made defines for the sizes of some of the hash tables and id tables.
 *
 * Revision 1.2.2.6  1996/10/24 15:19:15  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.2.2.5  1996/10/22 18:49:29  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.2.2.4  1996/10/18 17:59:55  reids
 * Better shutdown procedures.
 *
 * Revision 1.2.2.3  1996/10/16 13:17:29  reids
 * Print out the port number that is being connected to.
 * Fix for handling arrays of Lisp data that have different byte order
 *   than sender's.
 *
 * Revision 1.2.2.2  1996/10/08 14:19:19  reids
 * Comment out things not needed for NMP_IPC
 *
 * Revision 1.2.2.1  1996/10/02 20:58:22  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.2  1996/05/09 16:53:37  reids
 * Remove (conditionally) references to matrix format.
 *
 * Revision 1.1  1996/05/09 01:01:16  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.6  1996/04/24 19:11:01  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.5  1996/04/03 03:11:06  reids
 * The data logging (-ld) option now works, for both IPC and regular X_IPC msgs.
 * Fixed the interaction between signals and listening for messages.
 * IPC version will not exit if connection to central server is not available.
 * IPC version will not print out stats if module exits (x_ipcModError).
 *
 * Revision 1.4  1996/03/19 03:38:35  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.3  1996/03/12 03:19:31  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.2  1996/03/05 04:10:48  reids
 * Fixed a bug in the LISP version where it would go into an infinite
 *   segv loop if trying to connect without a central (lispExit was not set)
 *
 * Revision 1.1  1996/03/03 04:31:06  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.78  1996/08/22  16:35:51  rich
 * Check the return code on x_ipcQueryCentral calls.
 *
 * Revision 1.77  1996/07/25  22:24:09  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.76  1996/07/03  21:43:28  reids
 * Have "x_ipcWaitUntilReady" print out any required resources it is waiting for
 *
 * Revision 1.75  1996/06/27  15:40:01  rich
 * Added x_ipcGetAcceptFds.
 *
 * Revision 1.74  1996/06/25  20:50:16  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.73  1996/05/14  22:40:03  rich
 * Need the check for valid context before dereferencing.
 *
 * Revision 1.72  1996/05/09  18:30:31  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.71  1996/03/29  21:28:48  reids
 * Fixed a problem with having a NULL format as the reply format of a query.
 *
 * Revision 1.70  1996/03/15  21:27:52  reids
 * Added x_ipcQueryNotify, x_ipcAddEventHandler, x_ipcRemoveEventHandler.
 *   Fixed re-registration bug; Plugged memory leaks; Fixed way task trees
 *   are killed; Added support for "enum" format type.
 *
 * Revision 1.69  1996/03/05  05:04:18  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.68  1996/03/04  18:32:08  rich
 * Fixed problem with recursive queries.
 *
 * Revision 1.67  1996/03/02  03:21:24  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.66  1996/02/21  18:30:10  rich
 * Created single event loop.
 *
 * Revision 1.65  1996/02/12  17:42:02  rich
 * Handle direct connection disconnect/reconnect.
 *
 * Revision 1.64  1996/02/11  21:34:54  rich
 * Updated GNUmakefiles for faster complilation.  Use FAST_COMPILE=1 for
 * routine recompiles.
 *
 * Revision 1.63  1996/02/10  16:49:37  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.62  1996/02/06  19:04:20  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.61  1996/01/27  21:53:01  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.60  1996/01/23  00:06:24  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.59  1996/01/05  16:31:05  rich
 * Added windows NT port.
 *
 * Revision 1.58  1995/12/21  19:17:35  reids
 * For safety's sake, copy strings when registering messages, handlers and
 *    when connecting.
 *
 * Revision 1.57  1995/12/17  20:21:13  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.56  1995/12/15  01:23:06  rich
 * Moved Makefile to Makefile.generic to encourage people to use
 * GNUmakefile.
 * Fixed a memory leak when a module is closed and some other small fixes.
 *
 * Revision 1.55  1995/11/03  03:04:18  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.54  1995/10/29  18:26:29  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 * 
 * Revision 1.53.2.3  1995/12/19  04:26:37  reids
 * For safety's sake, copy strings when registering messages, handlers and
 *   when connecting.
 *
 * Revision 1.53.2.2  1995/11/02  22:42:13  reids
 * Fixed a problem with incorrectly setting modData.hostName
 *
 * Revision 1.53.2.1  1995/10/27  15:16:40  rich
 * Fixed problems with connecting to multiple central servers.
 *
 * Revision 1.53  1995/10/25  22:48:00  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.52  1995/10/07  19:07:02  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.51  1995/08/14  21:31:19  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.50  1995/08/06  16:43:43  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.49  1995/08/05  17:16:16  reids
 * Several important bug fixes:
 *   a) Found a memory leak in the tms (when nodes are unasserted)
 *   b) Fixed a problem with direct connections that would cause X_IPC to crash
 *      when a command or goal message was sent from an inform or a query.
 *      As part of that fix, all command and goal messages that are sent from
 *      informs or queries are now added to the root node of the task tree.
 *
 * Revision 1.48  1995/07/19  14:25:52  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.47  1995/07/12  04:54:24  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.46  1995/07/10  16:16:57  rich
 * Interm save.
 *
 * Revision 1.45  1995/07/06  21:15:47  rich
 * Solaris and Linux changes.
 *
 * Revision 1.44  1995/06/14  03:21:26  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.43  1995/06/05  23:58:46  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.42  1995/04/21  03:53:12  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.41  1995/04/19  14:27:50  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.40  1995/04/09  20:30:07  rich
 * Added /usr/local/include and /usr/local/lib to the paths for compiling
 * for sunOS machines. (Support for new vendor OS).
 * Create a x_ipc directory in /tmp and put the socket in there so others can
 * delete dead sockets.  The /tmp directory has the sticky bit set so you
 * can't delete files even if you have write permission on the directory.
 * Fixes to libc.h to use the new declarations in the gcc header files and
 * avoid problems with dbmalloc.
 *
 * Revision 1.39  1995/04/08  02:06:19  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.38  1995/04/07  05:02:55  rich
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
 * Revision 1.37  1995/04/05  19:10:36  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.36  1995/04/04  19:41:50  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.35  1995/03/30  15:42:32  rich
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
 * Revision 1.34  1995/03/28  01:14:22  rich
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
 * Revision 1.33  1995/03/19  19:39:22  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.32  1995/03/18  15:11:00  rich
 * Fixed updateVersion script so it can be run from any directory.
 *
 * Revision 1.31  1995/02/06  14:46:16  reids
 * Removed the "Global" suffix from x_ipcRootNode, x_ipcServer and x_ipcDefaultTime
 *
 * Revision 1.30  1995/01/30  16:17:40  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.29  1995/01/25  00:00:59  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.28  1995/01/18  22:39:52  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.27  1994/11/02  21:34:00  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.26  1994/11/02  16:14:26  reids
 * Fixed x_ipc_connectSocket to handle server hosts specified as <host>:<port>
 *
 * Revision 1.25  1994/10/25  17:09:41  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.24  1994/06/07  02:30:22  rich
 * Include the top level README and other files in the tarfile.
 * Include extra header infomation to get rid of warnings.
 *
 * Revision 1.23  1994/05/31  03:23:44  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.22  1994/05/25  04:57:06  rich
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
 * Revision 1.21  1994/05/24  13:48:31  reids
 * Fixed so that messages are not sent until a x_ipcWaitUntilReady is received
 * (and the expected number of modules have all connected)
 *
 * Revision 1.20  1994/05/17  23:15:20  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.19  1994/05/11  01:57:12  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.18  1994/05/05  00:46:03  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.17  1994/04/28  16:15:25  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.16  1994/04/26  16:23:15  rich
 * Now you can register an exit handler before anything else and it will
 * get called if connecting to central fails.
 * Also added code to handle pipe breaks during writes.
 *
 * Revision 1.15  1994/04/16  19:41:45  rich
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
 * Revision 1.14  1994/04/15  17:09:48  reids
 * Changes to support vxWorks version of X_IPC 7.5
 *
 * Revision 1.13  1994/04/04  16:42:48  reids
 * Fixed a bug found by Rich in waitForReply2
 *
 * Revision 1.12  1994/03/27  22:50:14  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.11  1993/12/14  17:33:02  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.10  1993/12/01  18:57:44  rich
 * Changed TCP to  IPPROTO_TCP, a system defined constant.
 *
 * Revision 1.9  1993/12/01  18:02:53  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.8  1993/11/21  20:17:21  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.7  1993/10/21  16:13:36  rich
 * Fixed compiler warnings.
 *
 * Revision 1.6  1993/10/20  19:00:25  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.5  1993/08/30  21:53:12  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.4  1993/08/27  07:14:19  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/20  23:06:37  fedor
 * Minor changes for merge. Mostly added htons and removed cfree calls.
 *
 * Revision 1.2  1993/05/26  23:16:59  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:16  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:18  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:19  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 6-May-92 Christopher Fedor, School of Computer Science, CMU
 * Added while loop to select if select dies on an EINTR then retry select.
 *
 * 17-Feb-92 Christopher Fedor, School of Computer Science, CMU
 * Generalized call to a registered exit handler x_ipcExitHnd for all modules
 * except lisp. VxWorks no longer has a special x_ipcVxExit routine.
 *
 * 28-Oct-91 Christopher Fedor, School of Computer Science, CMU
 * Moved call for x_ipcVxExit so that it will be called when a close is detected
 * by both a direct connection and central disconnect. Currently the VxWorks 
 * part is not set up for direct connections and the exit call was only 
 * happening for direct connections.
 *
 * 14-Aug-91 Christopher Fedor, School of Computer Science, CMU
 * Changed VxWorks include files to vx5.0
 *
 * 25-Mar-91 Christopher Fedor, School of Computer Science, CMU
 * Added lispExitGlobal as the VxWorks way of exiting lisp.
 * Should eventually add a general abort handler and exit handler.
 *
 * 30-Jan-91 Christopher Fedor, School of Computer Science, CMU
 * Added fflush(stdout) to printf for module code calls from lisp
 *
 *  6-Dec-90 Christopher Fedor, School of Computer Science, CMU
 * Removed signal.h because the signal to call x_ipcClose on various signals
 * was commeted out and signal.h for sunos 4.1 eventually includes
 * sys/stdtypes which defines size_t as an int - this conflicts with the
 * vxworks includes which eventually include types.h which defines
 * size_t as a long.
 * Move sys/time.h to non VxWorks and added vxworks utime.h because the
 * timeval struct is needed and the size_t conflict exits.
 *
 *  7-Apr-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 *  6-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * Created.  
 * fedor: 6-Mar-89 problem with many defined globals from com_server and lots
 * of repetitive code needs to be removed!  perhaps a com_com file for
 * common communications between the two - actually i want to avoid 
 * differences - module and server should be alike as much as possible.
 *
 * $Revision: 2.23 $
 * $Date: 2011/08/16 16:01:52 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

#ifdef NMP_IPC
#ifdef DOS_FILE_NAMES
#include "parseFmt.h"
#include "printDat.h"
#else
#include "parseFmttrs.h"
#include "printData.h"
#endif
#endif

#include <stdarg.h>
#ifdef WIN32
#include <signal.h>
#elif !defined(__sgi)
#include <sys/ioctl.h>
#endif

static X_IPC_RETURN_VALUE_TYPE x_ipc_processQueryReply(DATA_MSG_PTR dataMsg, 
					       MSG_PTR msg, BOOLEAN sel,
					       void *reply);
X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReplyFromTime(X_IPC_REF_PTR ref, void *reply, 
					   BOOLEAN sel, struct timeval *time,
					   int fd);
static void invokeChangeHandlers (MSG_PTR msg);

/******************************************************************************
 *
 * FUNCTION: void x_ipc_printDataModuleInitialize();
 *
 * DESCRIPTION: 
 * Sets print data routines for modules.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

#ifndef NMP_IPC
/* ARGSUSED */
static void sampleModulePrintData(FILE *stream, const void *item, int32 next)
{
#ifdef macintosh
#pragma unused(stream, item, next)
#endif
  X_IPC_MOD_WARNING( "\nWARNING: DPrint called!\n");
}

/* ARGSUSED */
static void sampleModuleMapPrintData(FILE *stream, void *map, 
				     CONST_FORMAT_PTR format,
				     int32 next, void *mapElements)
{
#ifdef macintosh
#pragma unused(stream, map, format, next, mapElements)
#endif
  X_IPC_MOD_WARNING( "\nWARNING: DPrint on maps called!\n");
}

static void x_ipc_printDataModuleInitialize(void)
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(dPrintSTR_FN) = (void (*)(FILE *, const char *, int32)) 
    sampleModulePrintData;
  GET_M_GLOBAL(dPrintFORMAT_FN) = (void (*)(FILE *, CONST_FORMAT_PTR, int32))
    sampleModulePrintData;
#ifndef NMP_IPC
  GET_M_GLOBAL(dPrintMAP_FN) = (void (*)(FILE *, const genericMatrix *,
					 CONST_FORMAT_PTR,
					 int, const void *))
    sampleModuleMapPrintData;
#endif
  GET_M_GLOBAL(dPrintX_IPC_FN) = 
    (void (*)(FILE *, const X_IPC_REF_TYPE *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintCHAR_FN) = 
    (void (*)(FILE *, const char *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintSHORT_FN) = 
    (void (*)(FILE *, const int16 *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintLONG_FN) = 
    (void (*)(FILE *, const long *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintINT_FN) = 
    (void (*)(FILE *, const int32 *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintBOOLEAN_FN) = 
    (void (*)(FILE *, const int32 *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintFLOAT_FN) = 
    (void (*)(FILE *, const float *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintDOUBLE_FN) = 
    (void (*)(FILE *, const double *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintBYTE_FN) = 
    (void (*)(FILE *, const char *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintUBYTE_FN) = 
    (void (*)(FILE *, const char *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintTWOBYTE_FN) = 
    (void (*)(FILE *, const char *, int))sampleModulePrintData;

  GET_M_GLOBAL(dPrintUSHORT_FN) = 
    (void (*)(FILE *, const unsigned short *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintUINT_FN) = 
    (void (*)(FILE *, const unsigned int *, int))sampleModulePrintData;
  GET_M_GLOBAL(dPrintULONG_FN) = 
    (void (*)(FILE *, const unsigned long *, int))sampleModulePrintData;
  UNLOCK_M_MUTEX;
}
#else
static void x_ipc_printDataModuleInitialize(void)
{
  LOCK_M_MUTEX;
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
  UNLOCK_M_MUTEX;
}
#endif

#ifdef VXWORKS
/******************************************************************************
 *
 * FUNCTION: void killModule(char *taskName)
 *
 * DESCRIPTION: Enable a clean shutdown of a module under VxWorks
 *
 * NOTE: Meant to be invoked directly from the shell
 *
 *****************************************************************************/

void killModule(char *taskName);

void killModule(char *taskName)
{
  int moduleTID;

  moduleTID = taskNameToId(taskName);
  if (moduleTID == ERROR) {
    X_IPC_MOD_WARNING1("No task named %s was found\n", taskName);
  } else {
    kill(moduleTID, SIGTERM);
  }
}

static void abortModule (int s)
{
  LOCK_M_MUTEX;
  X_IPC_MOD_ERROR2("%s received an abort signal %d\n",
		   GET_M_GLOBAL(modNameGlobal), s);
  UNLOCK_M_MUTEX;
  x_ipcClose();
  exit(-1);
}
#endif

/* Catch SIGPIPE signals */
static void pipeClosedHnd (int s)
{
  fprintf(stderr, "Pipe Broke\n");
#ifdef linux
  /* According to the man page for "signal", the Linux version resets the handler
     to its default value each time the signal is raised. */
  signal(SIGPIPE, pipeClosedHnd);
#endif
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcModuleInitialize()
 *
 * DESCRIPTION: 
 * Sets globals and calls main initialize routines for initializing a module.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcModuleInitialize(void)
{
  if (mGlobalp() && GET_C_GLOBAL(valid))
    /* already have initialized the data structures */
    return;

#ifndef WIN32
  signal(SIGPIPE, pipeClosedHnd);
#endif
#ifdef OS2
  sock_init();
#endif

  x_ipc_globalMInit();

  LOCK_CM_MUTEX;
  GET_M_GLOBAL(x_ipcExitHnd) = NULL;
  
  GET_M_GLOBAL(sizeDM) = 0;
  GET_M_GLOBAL(numAllocatedDM) = 0;
  
  GET_M_GLOBAL(directFlagGlobal) = FALSE;
  GET_C_GLOBAL(moduleConnectionTable) = 
    x_ipc_hashTableCreate(11, (HASH_FN)x_ipc_intHashFunc, (EQ_HASH_FN)x_ipc_intKeyEqFunc);
  FD_ZERO(&GET_M_GLOBAL(externalMask));
  GET_M_GLOBAL(externalFdTable) = 
    x_ipc_hashTableCreate(11, (HASH_FN)x_ipc_intHashFunc, (EQ_HASH_FN)x_ipc_intKeyEqFunc);
  
  GET_M_GLOBAL(versionGlobal).x_ipcMajor = X_IPC_VERSION_MAJOR;
  GET_M_GLOBAL(versionGlobal).x_ipcMinor = X_IPC_VERSION_MINOR;
  
  GET_M_GLOBAL(byteOrder) = BYTE_ORDER;
  GET_M_GLOBAL(alignment) = (ALIGNMENT_TYPE)IPC_ALIGN;
  
  x_ipc_dataMsgInitialize();
  
  x_ipcRefInitialize();
  x_ipc_formatInitialize();

  GET_C_GLOBAL(msgIdTable) = x_ipc_idTableCreate("Message Table", MSG_TABLE_SIZE);
  GET_C_GLOBAL(hndIdTable) = x_ipc_idTableCreate("Handler Table", MSG_TABLE_SIZE);
  
  GET_C_GLOBAL(messageTable) = x_ipc_hashTableCreate(MSG_HASH_TABLE_SIZE,
					       (HASH_FN)x_ipc_strHashFunc, 
					       (EQ_HASH_FN)x_ipc_strKeyEqFunc);
  GET_C_GLOBAL(handlerTable) = x_ipc_hashTableCreate(MSG_HASH_TABLE_SIZE,
					       (HASH_FN)x_ipc_intStrHashFunc, 
					       (EQ_HASH_FN)x_ipc_intStrKeyEqFunc);
  
  GET_M_GLOBAL(classFormatTable) = x_ipc_hashTableCreate(11, (HASH_FN)x_ipc_classHashFunc, 
						   (EQ_HASH_FN) x_ipc_classEqFunc);
  
#ifdef IPC_ALLOW_DISCONNECTED_EVENT_HANDLING
  /* zero these here instead of in IPC_connect() so that
     IPC_subscribeFD() calls before IPC_connect() aren't clobbered. */
  FD_ZERO(&GET_C_GLOBAL(x_ipcConnectionListGlobal));
  FD_ZERO(&(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
  GET_C_GLOBAL(maxConnection) = 0;
#endif

  x_ipc_printDataModuleInitialize();
  
  GET_M_GLOBAL(Message_Ignore_Set) = x_ipc_listCreate();
  
  GET_C_GLOBAL(valid) = TRUE;
  UNLOCK_CM_MUTEX;
}

/******************************************************************************
 *
 * FUNCTION: char *x_ipcServerMachine()
 *
 * DESCRIPTION: Simply calls getenv to read the server machine from CENTRALHOST
 *
 * INPUTS: none.
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

const char *x_ipcServerMachine(void)
{
  char *servHost;
  
  servHost = getenv("CENTRALHOST");
  if (!servHost) {
    X_IPC_MOD_WARNING("Warning: CENTRALHOST environment variable not set.\n");
    return NULL;
  };
  
  return servHost;
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcIgnoreLogging()
 *
 * DESCRIPTION: Inform central to avoid logging a particular message from 
 *              now on
 *
 * INPUTS: char *msgName to ignore.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcIgnoreLogging(char *msgName)
{
  (void)x_ipcInform(X_IPC_IGNORE_LOGGING_INFORM, (void *)&msgName);
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcResumeLogging()
 *
 * DESCRIPTION: Inform central to resume logging a particular message 
 *              from now on
 *
 * INPUTS: char *msgName to resume logging.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcResumeLogging(char *msgName)
{
  (void)x_ipcInform(X_IPC_RESUME_LOGGING_INFORM, (void *)&msgName);
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcClose()
 *
 * DESCRIPTION: 
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcClose(void)
{
  x_ipcCloseInternal(/* informServer = */ 1);
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcCloseInternal()
 *
 * DESCRIPTION: 
 *
 * INPUTS: informServer is a boolean value.  If it is true, we inform
 *         the central server that we are disconnecting.  This is the
 *         usual case.  However, the following is a case when we want a process
 *         to silently disconnect in a way that the server doesn't
 *         notice:
 *
 *         (1) We have a module that connects to central
 *         (2) It wishes to execute a child process via fork().
 *         (3) The forked child inherits a copy of the open fds
 *             for the connection to central.  These open fds
 *             survive even if the child makes an exec() call,
 *             although the child presumably doesn't use them
 *             or listen to them.
 *         (4) If the parent exits before the child, the connection to
 *             central remains open and central does not receive a
 *             SIGPIPE.
 *
 *         The fix is to make an x_ipcCloseInternal(0) call in the
 *         child before calling exec().  This closes the child's
 *         copy of the server connection, leaving the parent connected
 *         normally.  The central server doesn't notice any change.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcCloseInternal(int informServer)
{
  /* inform central that we are shutting down, if a connection exists */
  if (x_ipc_isValidServerConnection()) {
    LOCK_CM_MUTEX;
    if (GET_C_GLOBAL(serverRead) != CENTRAL_SERVER_ID) {
      if (informServer) {
	x_ipcInform(X_IPC_CLOSE_INFORM, NULL);
      }
      if (informServer) {
	/* SHUTDOWN_SOCKET explicitly shuts down the socket, breaks the pipe */
	SHUTDOWN_SOCKET((GET_C_GLOBAL(serverRead)));
      } else {
	/* CLOSE_SOCKET just closes the fd; central doesn't notice unless
	   this is the last copy */
	CLOSE_SOCKET((GET_C_GLOBAL(serverRead)));
      }
      FD_CLR((unsigned)(GET_C_GLOBAL(serverRead)),
	     &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
      if (GET_C_GLOBAL(serverWrite) != GET_C_GLOBAL(serverRead)) {
	if (informServer) {
	  SHUTDOWN_SOCKET(GET_C_GLOBAL(serverWrite));
	} else {
	  CLOSE_SOCKET(GET_C_GLOBAL(serverWrite));
	}
	FD_CLR((unsigned)(GET_C_GLOBAL(serverWrite)),
	       &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
      }
    }
    UNLOCK_CM_MUTEX;
  }
#ifndef NO_UNIX_SOCKETS
  if (mGlobalp()) {
    LOCK_CM_MUTEX;
    if (GET_C_GLOBAL(listenPortNum) != 0)
      x_ipc_closeSocket(GET_C_GLOBAL(listenPortNum));
    UNLOCK_CM_MUTEX;
  }
#endif
  x_ipc_globalMInvalidate();

  /* Added by TNgo, 5/22/97, to unload winsock.dll */
#ifdef _WINSOCK_
  WSACleanup();
  printf("Socket cleaned up.");
#endif
}

/******************************************************************************
 *
 * FUNCTION: int x_ipc_resetDirect(const char *msgName, MSG_PTR msg, int sd)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/
static int x_ipc_resetDirect(const char *msgName, MSG_PTR msg, int sd)
{
  int i, count;
  DIRECT_MSG_HANDLER_PTR handler;

  if (msg->directList && msgName) {
    for (count=0, i=0; i<msg->directList->numHandlers; i++) {
      handler = &msg->directList->handlers[i];
      if (handler->readSd == sd || handler->writeSd == sd) {
	/* Signal that this direct handler is not in use */
	handler->readSd = handler->writeSd = -1;
      }
      if (handler->readSd == -1) {
	count++; /* Count number of deleted handlers */
      }
    }
    /* Delete if all the direct handlers have been reset */
    if (count == msg->directList->numHandlers) 
      freeDirectList(msg);
  }
  return TRUE;
}

void x_ipcHandleClosedConnection(int sd, CONNECTION_PTR connection)
{
  LOCK_CM_MUTEX;
  FD_CLR((unsigned)connection->readSd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
  FD_CLR((unsigned)connection->readSd, &(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
  UNLOCK_CM_MUTEX;
  SHUTDOWN_SOCKET(connection->readSd);
  if (connection->readSd != connection->writeSd) {
    SHUTDOWN_SOCKET(connection->writeSd);
  }
  LOCK_CM_MUTEX;
  if (!DIRECT_CONNECTION(connection)) {
    X_IPC_MOD_ERROR1("Closed Connection Detected from: Central Server %s\n",
		     GET_C_GLOBAL(servHostGlobal));
    /* May have already cleaned up with the x_ipcModError.   */
    if (x_ipc_hashTableFind((void *)&sd, GET_C_GLOBAL(moduleConnectionTable))) {
      x_ipc_hashTableRemove((char *)&(connection->readSd),
		      (GET_C_GLOBAL(moduleConnectionTable)));
      x_ipc_globalMInvalidate();
      x_ipcFree((char *)connection);
    }
#ifdef LISP
    if (IS_LISP_MODULE()) {
      if (GET_M_GLOBAL(lispExitGlobal) != NULL)
	(*(GET_M_GLOBAL(lispExitGlobal)))();
    } else
#endif /* LISP */
      {
	if ((GET_M_GLOBAL(x_ipcExitHnd))) {
	  (*(GET_M_GLOBAL(x_ipcExitHnd)))();
	} else {
#ifdef _WINSOCK_
	  WSACleanup();
#endif /* Unload Winsock DLL */
	  UNLOCK_CM_MUTEX;
	  exit(-1);
	}
      }
  } else {
    X_IPC_MOD_WARNING1("Closed Connection Detected from: Module: sd: %d:\n",
		       connection->readSd);
    x_ipc_hashTableRemove((char *)&sd,
			  (GET_C_GLOBAL(moduleConnectionTable)));
    /* Need to reset the direct info for messages */
    x_ipc_hashTableIterate((HASH_ITER_FN)x_ipc_resetDirect,
			   GET_C_GLOBAL(messageTable), (void *)(size_t)sd);
    x_ipcFree((char *)connection);
  }
  UNLOCK_CM_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcSetContext(X_IPC_CONTEXT_PTR context)
 *
 * DESCRIPTION: 
 *
 * INPUTS: char *modName, *serverHost;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/
void x_ipcSetContext(X_IPC_CONTEXT_PTR context)
{
  if (context != NULL) {
    LOCK_M_MUTEX;
    GET_M_GLOBAL(currentContext) = context;
    UNLOCK_M_MUTEX;
  }
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_CONTEXT_PTR x_ipcGetContext(void)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

X_IPC_CONTEXT_PTR x_ipcGetContext(void)
{
  X_IPC_CONTEXT_PTR result;

  LOCK_M_MUTEX;
  result = GET_M_GLOBAL(currentContext);
  UNLOCK_M_MUTEX;

  return result;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_CONTEXT_PTR x_ipcResetContext(void)
 *
 * NOTE: ASSUMES THAT C AND M MUTEXES ARE ALREADY LOCKED WHEN INVOKED!!!!
 *
 *****************************************************************************/

static void x_ipcResetContext(void)
{
  // LOCK_M_MUTEX; Assumes already locked
  GET_M_GLOBAL(currentContext) = 
    (X_IPC_CONTEXT_PTR)x_ipcMalloc(sizeof(X_IPC_CONTEXT_TYPE));
  // LOCK_C_MUTEX; Assumes already locked
#ifdef THREADED
  initMutex(&GET_C_GLOBAL(mutex));
  initMutex(&GET_C_GLOBAL(ioMutex));
#endif
  
  FD_ZERO(&GET_C_GLOBAL(x_ipcConnectionListGlobal));
  FD_ZERO(&GET_C_GLOBAL(x_ipcListenMaskGlobal));

  GET_C_GLOBAL(servHostGlobal) = NULL;
  GET_C_GLOBAL(moduleConnectionTable) = 
    x_ipc_hashTableCreate(11, (HASH_FN)x_ipc_intHashFunc, 
			  (EQ_HASH_FN)x_ipc_intKeyEqFunc);
  GET_C_GLOBAL(handlerTable) =
    x_ipc_hashTableCreate(101, (HASH_FN)x_ipc_intStrHashFunc, 
			  (EQ_HASH_FN)x_ipc_intStrKeyEqFunc);
  GET_C_GLOBAL(messageTable) =
    x_ipc_hashTableCreate(101, (HASH_FN)x_ipc_strHashFunc,
			  (EQ_HASH_FN)x_ipc_strKeyEqFunc);
  GET_C_GLOBAL(resourceTable) =
    x_ipc_hashTableCreate(11, (HASH_FN)x_ipc_strHashFunc, 
			  (EQ_HASH_FN)x_ipc_strKeyEqFunc);
  GET_C_GLOBAL(hndIdTable) = x_ipc_idTableCreate("Handler Table", 10);
  GET_C_GLOBAL(msgIdTable) = x_ipc_idTableCreate("Message Table", 10);
  GET_C_GLOBAL(pendingReplies) = x_ipc_listCreate();
  initMsgQueue(&GET_C_GLOBAL(msgQueue));
  GET_C_GLOBAL(x_ipcRefFreeList) = x_ipc_listCreate();
  
  GET_C_GLOBAL(maxConnection) = 0;
  GET_C_GLOBAL(parentRefGlobal) = -1; 
  GET_C_GLOBAL(sendMessageRefGlobal) = 1;
  GET_C_GLOBAL(listenPortNum) = 0;
  GET_C_GLOBAL(listenPort) = NO_FD;
  GET_C_GLOBAL(listenSocket) = NO_FD;
  GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
  GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
  GET_C_GLOBAL(willListen) = TRUE;
  GET_C_GLOBAL(tappedMsgs) = NULL;
  GET_C_GLOBAL(broadcastMsgs) = NULL;
  GET_C_GLOBAL(directDefault) = FALSE;
  GET_C_GLOBAL(valid) = FALSE;

  GET_C_GLOBAL(queryNotificationList) = x_ipc_listCreate();
  GET_C_GLOBAL(connectNotifyList) = NULL;
  GET_C_GLOBAL(disconnectNotifyList) = NULL;
  GET_C_GLOBAL(changeNotifyList) = NULL;
  // UNLOCK_CM_MUTEX; Assumes already locked
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcModuleProvides(resourceName, ...)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/
#ifndef NMP_IPC
void x_ipcModuleProvides(const char* resourceName, ...)
{
  const char *res;
  STR_LIST_PTR resources;
  va_list args;
  
  if (resourceName == NULL) 
    return;
  
  resources = x_ipc_strListCreate();
  res = resourceName;
  va_start(args, resourceName);
  while (res != NULL) {
    x_ipc_strListPushUnique(res, resources);
    res = va_arg(args,const char *);
  }
  va_end(args);
  
  x_ipcInform(X_IPC_PROVIDES_INFORM, resources);
  x_ipc_strListFree(&resources,FALSE);
}
#endif

/******************************************************************************
 *
 * FUNCTION: void x_ipcModuleProvidesArgv(resourceName, ...)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcModuleProvidesArgv(const char** resourceNames)
{
  int i=0;
  STR_LIST_PTR resources;
  
  if (resourceNames == NULL) 
    return;
  
  resources = x_ipc_strListCreate();
  for (i=0; resourceNames[i] != NULL; i++){
    x_ipc_strListPushUnique(resourceNames[i], resources);
  }
  
  x_ipcInform(X_IPC_PROVIDES_INFORM, resources);
  x_ipc_strListFree(&resources,FALSE);
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcModuleRequires(resourceName, ...)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/
#ifndef NMP_IPC
void x_ipcModuleRequires(const char *resourceName, ...)
{
  const char *res;
  STR_LIST_PTR resources;
  va_list args;
  
  if (resourceName == NULL) return;
  
  resources = x_ipc_strListCreate();
  res = resourceName;
  va_start(args, resourceName);
  while (res != NULL) {
    x_ipc_strListPushUnique(res, resources);
    res = va_arg(args,const char *);
  }
  va_end(args);
  
  x_ipcInform(X_IPC_REQUIRES_INFORM, resources);
  x_ipc_strListFree(&resources,FALSE);
}
#endif

/******************************************************************************
 *
 * FUNCTION: void x_ipcModuleRequiresArgv(char **resourceNames)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcModuleRequiresArgv(const char **resourceNames)
{
  int i;
  
  if (resourceNames == NULL) return;
  
  LOCK_M_MUTEX;
  for (i=0; resourceNames[i] != NULL; i++){
    x_ipc_strListPushUnique(strdup(resourceNames[i]),
			    GET_M_GLOBAL(requiredResources));
  }
  
  x_ipcInform(X_IPC_REQUIRES_INFORM, GET_M_GLOBAL(requiredResources));
  UNLOCK_M_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: const char **x_ipcQueryAvailable(void)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

char **x_ipcQueryAvailable(void)
{
  STR_LIST_PTR resources;
  int length,i;
  char **res;
  
  resources = x_ipc_strListCreate();
  x_ipcQuery(X_IPC_AVAILABLE_QUERY, NULL, resources);
  length = x_ipc_strListLength(resources);
  res = (char **)x_ipcMalloc((length + 1) * sizeof(char *));
  i = 0;
  STR_LIST_ITERATE(resources, string, { res[i] = (char *)string; i++; });
  x_ipc_strListFree(&resources,FALSE);
  res[length] = NULL;
  return res;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcConnectModule(modName, serverHost)
 *
 * DESCRIPTION: 
 *
 * INPUTS: char *modName, *serverHost;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcConnectModule(const char *modName, const char *serverHost)
{
  MOD_DATA_TYPE modData;
  MOD_START_TYPE vData;
  CONNECTION_PTR connection;
  X_IPC_RETURN_VALUE_TYPE statusV;
  char thisHost[HOST_NAME_SIZE+1], *colon;
  int serverPort;
  
  /* Allow for connections to multiple central servers on different machines */
  /* Determine if already connected to another machine. */
  if (x_ipc_isValidServerConnection()) {
    LOCK_CM_MUTEX;
    if ((GET_C_GLOBAL(servHostGlobal) != NULL) &&
	strcmp(GET_C_GLOBAL(servHostGlobal), serverHost) == 0) {
      X_IPC_MOD_WARNING1("x_ipcConnectModule: already connected to %s\n", serverHost);
    } else {
      /* Connecting to another central server. */
      (GET_M_GLOBAL(modNameGlobal)) = strdup(modName);
      
      /* Need to reset the context. */
      x_ipcResetContext();
      if (serverHost != NULL)
	GET_C_GLOBAL(servHostGlobal) = strdup(serverHost);
      GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
      GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
      
      if (serverHost) {
	if (!x_ipc_connectSocket(serverHost, &(GET_C_GLOBAL(serverRead)),
				 &(GET_C_GLOBAL(serverWrite)))) {
	  X_IPC_MOD_ERROR1("ERROR: Can not connect to the server on %s.",
		      serverHost);
	  UNLOCK_CM_MUTEX;    
	  return;
	}
      } else {
	GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
	GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
	bzero(&(thisHost[0]),HOST_NAME_SIZE+1);
	gethostname(thisHost,HOST_NAME_SIZE);
	X_IPC_MOD_WARNING1("Warning: trying to connect to local host %s.\n",
		      thisHost);
	if (!x_ipc_connectSocket(thisHost, &(GET_C_GLOBAL(serverRead)),
				 &(GET_C_GLOBAL(serverWrite)))) {
	  X_IPC_MOD_ERROR1("ERROR: Can not connect to the server on %s.",
		      thisHost);
	  GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
	  GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
	  UNLOCK_CM_MUTEX;    
	  return;
	}
      }
      
      FD_SET((GET_C_GLOBAL(serverRead)),
	     &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
      
      /* Need to keep track of the maximum.  */
      GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
					 GET_C_GLOBAL(serverRead));
      GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
					 GET_C_GLOBAL(serverWrite));
      
      connection = NEW(CONNECTION_TYPE);
      bzero((void *)connection,sizeof(CONNECTION_TYPE));
      connection->readSd = (GET_C_GLOBAL(serverRead));
      connection->writeSd = (GET_C_GLOBAL(serverWrite));
      
      x_ipc_hashTableInsert((char *)&(connection->readSd),
		      sizeof(connection->readSd),
		      (char *)connection,
		      (GET_C_GLOBAL(moduleConnectionTable)));
      
      x_ipc_msgInfoMsgInitialize();
      x_ipc_classModInitialize();
      
      modData.modName = modName; /* No need to copy; not saved */
      modData.hostName = (char *)x_ipcMalloc(sizeof(char)*HOST_NAME_SIZE+1);
      bzero((char *)modData.hostName,HOST_NAME_SIZE+1);
      gethostname((char *)modData.hostName,HOST_NAME_SIZE);  
      
      statusV = x_ipcQueryCentral(X_IPC_CONNECT_QUERY, (void *)&modData,
				(void *)&vData);
      x_ipcFree((void *)modData.hostName);
      modData.hostName = NULL;
      
      if (statusV != Success) {
	X_IPC_MOD_ERROR("ERROR: ConnectModule Query failed.");
	UNLOCK_CM_MUTEX;    
	return;
      }
      GET_C_GLOBAL(valid) = TRUE;
    }
    UNLOCK_CM_MUTEX;
  } else {
    /* First connection. */
    x_ipcModuleInitialize();
#ifdef macintosh
    initGUSI();
#endif
    
#ifdef VXWORKS
    (void)signal(SIGTERM, abortModule);
#endif

    LOCK_CM_MUTEX;
    GET_M_GLOBAL(modNameGlobal) = strdup(modName);
    if (serverHost != NULL)
      GET_C_GLOBAL(servHostGlobal) = strdup(serverHost);
    
#ifndef IPC_ALLOW_DISCONNECTED_EVENT_HANDLING
    /* need not to zero these in case IPC_subscribeFD() was called
       before IPC_connect().  it is now explicitly zeroed in
       IPC_initialize() */
    FD_ZERO(&(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    FD_ZERO(&(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
    GET_C_GLOBAL(maxConnection) = 0;
#endif
    
    if (serverHost) {
      if (!x_ipc_connectSocket(serverHost, &(GET_C_GLOBAL(serverRead)),
			       &(GET_C_GLOBAL(serverWrite)))) {
	GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
	GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
#ifndef NMP_IPC
	X_IPC_MOD_ERROR1("ERROR: Can not connect to the server on %s.",
		    serverHost);
#endif
	UNLOCK_CM_MUTEX;    
	return;
      }
    } else {
      GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
      GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
      bzero(&(thisHost[0]),HOST_NAME_SIZE+1);
      gethostname(thisHost,HOST_NAME_SIZE);
      X_IPC_MOD_WARNING1("Warning: trying to connect to local host %s.\n",
		    thisHost);
      if (!x_ipc_connectSocket(thisHost, &(GET_C_GLOBAL(serverRead)),
			       &(GET_C_GLOBAL(serverWrite)))) {
	X_IPC_MOD_ERROR1("ERROR: Can not connect to the server on %s.",
			 thisHost);
	GET_C_GLOBAL(serverRead) = NO_SERVER_GLOBAL;
	GET_C_GLOBAL(serverWrite) = NO_SERVER_GLOBAL;
	UNLOCK_CM_MUTEX;    
	return;
      }
    }
    
    FD_SET((GET_C_GLOBAL(serverRead)), 
	   &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    /* Need to keep track of the maximum.  */
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
				       GET_C_GLOBAL(serverRead));
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
				       GET_C_GLOBAL(serverWrite));
    
    connection = NEW(CONNECTION_TYPE);
    bzero((void *)connection,sizeof(CONNECTION_TYPE));
    connection->readSd = (GET_C_GLOBAL(serverRead));
    connection->writeSd = (GET_C_GLOBAL(serverWrite));
    
    x_ipc_hashTableInsert((char *)&(connection->readSd), sizeof(connection->readSd),
		    (char *)connection,
		    (GET_C_GLOBAL(moduleConnectionTable)));
    x_ipc_msgInfoMsgInitialize();
    x_ipc_classModInitialize();
    
    if (!GET_M_GLOBAL(x_ipcRootNodeGlobal)) {
      GET_M_GLOBAL(x_ipcRootNodeGlobal) = CREATE_NULL_REF();
    }
    
    GET_C_GLOBAL(pendingReplies) = x_ipc_listCreate();
    GET_C_GLOBAL(queryNotificationList) = x_ipc_listCreate();
    //initMsgQueue(&GET_C_GLOBAL(msgQueue)); Already done in globalMInit
    
    modData.modName = modName; /* No need to copy; not saved */
    modData.hostName = (char *)x_ipcMalloc(sizeof(char)*HOST_NAME_SIZE+1);
    bzero((void *)modData.hostName,HOST_NAME_SIZE+1);
    gethostname((char *)modData.hostName, HOST_NAME_SIZE);
    
    statusV = x_ipcQueryCentral(X_IPC_CONNECT_QUERY, (void *)&modData,
			      (void *)&vData);
    x_ipcFree((void *)modData.hostName);
    modData.hostName = NULL;
    if (statusV != Success) {
      X_IPC_MOD_ERROR("ERROR: ConnectModule Query failed.");
      UNLOCK_CM_MUTEX;    
      return;
    }
    
    GET_C_GLOBAL(directDefault) = vData.direct;

    /* Get the port number */
    colon = (serverHost ? (char *)index((char *)serverHost, ':') : NULL);
    serverPort = (colon == NULL ? SERVER_PORT : atoi(colon+1));

    if (vData.version.x_ipcMajor != X_IPC_VERSION_MAJOR) {
      X_IPC_MOD_WARNING( "*** ERROR ***\n");
      X_IPC_MOD_WARNING("Major Version Number is different from Task Control Server.\n");
      X_IPC_MOD_WARNING2( "Module Version     : %d.%d\n", 
		    (GET_M_GLOBAL(versionGlobal)).x_ipcMajor,
		    (GET_M_GLOBAL(versionGlobal)).x_ipcMinor);
      X_IPC_MOD_WARNING3( "Task Control Server: %d.%d (port %d)\n",
		    vData.version.x_ipcMajor, vData.version.x_ipcMinor, serverPort);
      X_IPC_MOD_ERROR0();
      UNLOCK_CM_MUTEX;    
      return;
    }
#ifndef NMP_IPC
    else if (vData.version.x_ipcMinor != X_IPC_VERSION_MINOR) {
      X_IPC_MOD_WARNING( "\n*** WARNING ***\n");
      X_IPC_MOD_WARNING( "Minor Version Number is different from Task Control Server.\n");
      X_IPC_MOD_WARNING2( "Module Version     : %d.%d\n", 
		    (GET_M_GLOBAL(versionGlobal)).x_ipcMajor,
		    (GET_M_GLOBAL(versionGlobal)).x_ipcMinor);
      X_IPC_MOD_WARNING3("Task Control Server: %d.%d (port %d)\n",
		    vData.version.x_ipcMajor, vData.version.x_ipcMinor, serverPort);
    } else {
      X_IPC_MOD_WARNING3("Task Control Connected %d.%d (port %d)\n",
		    vData.version.x_ipcMajor, vData.version.x_ipcMinor, serverPort);
    }
#endif
    
    /* RTG: Moved to x_ipcWaitUntilReady */
    /* x_ipc_modVarInitialize();*/
    
    /* x_ipc_dataMsgDisplayStats(); */
    
    if(GET_C_GLOBAL(directDefault)) {
      /* Make the default module direct. */
      x_ipcDirectResource(modName);
    }
    UNLOCK_CM_MUTEX;    
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcWaitUntilReady()
 *
 * DESCRIPTION: 
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcWaitUntilReady(void)
{
  int go;
  X_IPC_RETURN_VALUE_TYPE statusV;
  
  if (!x_ipc_isValidServerConnection()) {
    X_IPC_MOD_ERROR("ERROR: x_ipcWaitUntilReady: Must connect to server first\n");
    return;
  }
  
  /* Now initialize the tap messages. */
  x_ipc_modVarInitialize();
  
#ifndef NMP_IPC
  LOCK_M_MUTEX;
  if (x_ipc_strListEmpty(GET_M_GLOBAL(requiredResources))) {
    X_IPC_MOD_WARNING( "X_IPC Waiting ...\n");
  } else {
    STR_LIST_PTR element = GET_M_GLOBAL(requiredResources)->next;

    X_IPC_MOD_WARNING( "X_IPC Waiting For:");
    while (element) {
      X_IPC_MOD_WARNING1(" %s", (char *)element->item);
      element = element->next;
    }
    X_IPC_MOD_WARNING(" ...\n");
  }
  UNLOCK_M_MUTEX;
#endif
  statusV = x_ipcQueryCentral(X_IPC_WAIT_QUERY, (void *)NULL, (void *)&go);
  if (statusV != Success) {
    X_IPC_MOD_ERROR("ERROR: x_ipcWaitUntilReady Query failed.\n");
    return;
  }
  
#ifndef NMP_IPC
  X_IPC_MOD_WARNING( "... X_IPC Done.\n");
#endif
  LOCK_M_MUTEX;
  GET_M_GLOBAL(expectedWaitGlobal) = TRUE;
  UNLOCK_M_MUTEX;
}

/******************************************************************************
 * FUNCTION: x_ipc_handleQueryNotification
 *
 * DESCRIPTION: Find an applicable notification handler and handle the reply.
 *
 * INPUTS: DATAMSG_PTR dataMsg; QUERY_REPLY_INTENT data message
 *
 * OUTPUTS: BOOLEAN (whether dataMsg was handled within)
 *
 *****************************************************************************/

#ifdef NMP_IPC
/* We have to "fool" the receiver of the response into using this
   message for encoding/decoding, rather than the query message.
   Not pretty, but it works.  Depends on the fact that ReplyClass has
   no class data.  Later, rework central so that it knows how to handle
   this case explicitly. */ 
MSG_PTR getResponseMsg (DATA_MSG_PTR dataMsg)
{ 
  X_IPC_MSG_CLASS_TYPE resClass1 = ExecHndClass;
  CLASS_FORM_PTR classForm;
  MSG_PTR msg;
  char *name, **pointerToName;

  /* Format for ExecHndClass is "string" -- used to send the message name */
  LOCK_M_MUTEX;
  classForm = GET_CLASS_FORMAT(&resClass1);
  UNLOCK_M_MUTEX;
  /* Get the message name, and find it */
  pointerToName = (char **)x_ipc_dataMsgDecodeClass(classForm->format, dataMsg);
  name = (char *)(*pointerToName);
  msg = x_ipc_msgFind(name);
  x_ipc_classDataFree(resClass1, (char *)pointerToName);

  return msg;
}
#endif

static BOOLEAN x_ipc_testNotificationRef(int *refId, QUERY_NOTIFICATION_PTR element)
{
  return (*refId == element->ref->refId);
}

static BOOLEAN x_ipc_handleQueryNotification (DATA_MSG_PTR dataMsg)
{
  QUERY_NOTIFICATION_PTR queryNotif;
  CONST_FORMAT_PTR decodeFormat;
  void *reply;
 
  LOCK_CM_MUTEX;
  queryNotif = ((QUERY_NOTIFICATION_PTR)
		x_ipc_listMemReturnItem((LIST_ITER_FN)x_ipc_testNotificationRef,
					(char *)&(dataMsg->msgRef),
					GET_C_GLOBAL(queryNotificationList)));
  UNLOCK_CM_MUTEX;
  if (queryNotif) {
#ifdef NMP_IPC
    /* For IPC, need to use the message that was responded to, not the
       original message that was queried */
    queryNotif->ref->msg = getResponseMsg(dataMsg);
    queryNotif->ref->name = strdup(queryNotif->ref->msg->msgData->name);
    decodeFormat = queryNotif->ref->msg->msgData->msgFormat;
#else
    decodeFormat = queryNotif->ref->msg->msgData->resFormat;
#endif
    reply = (!dataMsg->msgTotal ? NO_CLIENT_DATA /* Represents NullReply */
	     : x_ipc_decodeDataInLanguage(dataMsg, decodeFormat, 
				    queryNotif->language));
#ifdef NMP_IPC
    queryNotif->ref->responded = TRUE; /* Prevent it from responding */
    reply = ipcData(queryNotif->ref->msg->msgData->msgFormat, (char *)reply);
    if (queryNotif->handler != NULL)
      (*((X_IPC_HND_DATA_FN)queryNotif->handler))(queryNotif->ref, (char *)reply,
						  queryNotif->clientData);
#else
    if (queryNotif->handler != NULL)
      (*queryNotif->handler)(reply, queryNotif->clientData);
#endif
    LOCK_CM_MUTEX;
    x_ipc_listDeleteItem((char *)queryNotif, GET_C_GLOBAL(queryNotificationList));
    UNLOCK_CM_MUTEX;
    x_ipcRefFree(queryNotif->ref);
    x_ipcFree((void *)queryNotif);
    x_ipc_dataMsgFree(dataMsg);
  }
  return (queryNotif != NULL);
}


/******************************************************************************
 *
 * FUNCTION: int iterateModuleConnection(int readSd,
 *                                       CONNECTION_PTR connection)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS:
 *
 * NOTES:
 *
 *****************************************************************************/
#ifndef NMP_IPC
static void handleModuleInput(int readSd,
			      CONNECTION_PTR connection)
{
  DATA_MSG_PTR dataMsg;
  
  if ((connection->readSd > 0) && ( connection->readSd == readSd)) {
    bzero((void *)&dataMsg,sizeof(dataMsg));
    
    switch(x_ipc_dataMsgRecv(connection->readSd, &dataMsg, 0, NULL, 0)){
    case StatOK:
      if (dataMsg->intent != QUERY_REPLY_INTENT) {
	x_ipc_execHnd(connection, dataMsg);
      } else if (!x_ipc_handleQueryNotification(dataMsg)) {
	LOCK_CM_MUTEX;
	x_ipc_listInsertItem(dataMsg, GET_C_GLOBAL(pendingReplies));
	UNLOCK_CM_MUTEX;
      }
      break;
    case StatError:
      X_IPC_MOD_ERROR("ERROR: Reading Data Message.");
      break;
    case StatEOF:
      x_ipcHandleClosedConnection(connection->readSd, connection);
      break;
    case StatSendEOF:
    case StatSendError:
    case StatRecvEOF:
    case StatRecvError:
#ifndef TEST_CASE_COVERAGE
    default:
#endif
      X_IPC_MOD_ERROR("Internal Error: iterateModuleConnections: UNKNOWN INTENT\n");
    }
  }
  return;
}
#endif

/******************************************************************************
 *
 * FUNCTION: static void x_ipc_acceptConnections(void)
 *
 * DESCRIPTION: 
 *   Accept requests for new connections.
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 * NOTES:
 *
 *****************************************************************************/
static void x_ipc_acceptConnections(fd_set *readMask)
{
  int readSd, writeSd;
  CONNECTION_PTR connection;
  struct sockaddr addr;
  int len = sizeof(struct sockaddr);
#ifdef VXWORKS
  char socketName[80]; 
  char portNum[80];
  char modName[80];
#endif

  LOCK_IO_READ_MUTEX;
  LOCK_CM_MUTEX;
  if ((GET_M_GLOBAL(directFlagGlobal)) &&
      (GET_C_GLOBAL(listenPort) != NO_FD) &&
      FD_ISSET((GET_C_GLOBAL(listenPort)), readMask)) {
    readSd = accept((GET_C_GLOBAL(listenPort)), &addr, (socklen_t *)&len);
    if (readSd <= 0) {
      X_IPC_MOD_ERROR("Accept Failed\n");
      UNLOCK_IO_READ_MUTEX;
      UNLOCK_CM_MUTEX;
      return;
    }
    FD_SET(readSd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    /* Need to keep track of the maximum.  */
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection), readSd);
    
    connection = NEW(CONNECTION_TYPE);
    bzero((void *)connection,sizeof(CONNECTION_TYPE));
    connection->readSd = readSd;
    connection->writeSd = readSd;
    
    X_IPC_MOD_WARNING1("New TCP Connection Established for a Module at sd: %d\n",
		  connection->readSd);
    
    x_ipc_hashTableInsert((char *)&(connection->readSd), sizeof(connection->readSd),
		    (char *)connection,
		    (GET_C_GLOBAL(moduleConnectionTable)));
    /* 25-Jun-91: fedor: set sd in readMask as well? */
  } else if ((GET_M_GLOBAL(directFlagGlobal)) &&
	     (GET_C_GLOBAL(listenSocket) != NO_FD) &&
	     FD_ISSET((GET_C_GLOBAL(listenSocket)), readMask)) {
#ifndef VXWORKS
    writeSd = readSd = accept((GET_C_GLOBAL(listenSocket)), &addr,
			      (socklen_t *)&len);
    if (readSd <= 0) {
      X_IPC_MOD_ERROR("Accept Failed\n");
      UNLOCK_IO_READ_MUTEX;
      UNLOCK_CM_MUTEX;
      return;
    }
    
    X_IPC_MOD_WARNING1("New UNIX Connection Established for a Module at sd: %d\n",
		  readSd);
#else
    /* Using Vx pipes for local communication. */
    x_ipc_readNBytes(GET_C_GLOBAL(listenSocket),modName, 80);
    bzero(portNum, sizeof(portNum));
    bzero(socketName, sizeof(socketName));
    snprintf(portNum,sizeof(portNum)-1, "%d", GET_C_GLOBAL(listenPortNum));
    snprintf(socketName, sizeof(socketName)-1, VX_PIPE_NAME, portNum, modName);
    readSd = open(socketName, O_RDONLY, 0644);
    if (readSd < 0) {
      X_IPC_MOD_ERROR("Open pipe Failed\n");
      UNLOCK_IO_READ_MUTEX;
      UNLOCK_CM_MUTEX;
      return;
    }

    snprintf(socketName, sizeof(socketName)-1, VX_PIPE_NAME, modName, portNum);
    writeSd = open(socketName, O_WRONLY, 0644);
    if (writeSd < 0) {
      X_IPC_MOD_ERROR("Open pipe Failed\n");
      UNLOCK_IO_READ_MUTEX;
      UNLOCK_CM_MUTEX;
      return;
    }
    x_ipc_writeNBytes(writeSd,portNum, 80);
    
    X_IPC_MOD_WARNING1("New VX PIPE Connection Established for a Module at sd: %d\n",
		  readSd);
#endif /* VXWORKS */

    connection = NEW(CONNECTION_TYPE);
    bzero((void *)connection,sizeof(CONNECTION_TYPE));
    connection->readSd = readSd;
    connection->writeSd = writeSd;
    
    x_ipc_hashTableInsert((char *)&(connection->readSd), sizeof(connection->readSd),
		    (char *)connection,
		    (GET_C_GLOBAL(moduleConnectionTable)));
    FD_SET(readSd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    /* Need to keep track of the maximum.  */
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection), readSd);
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection), writeSd);
    /* 25-Jun-91: fedor: set sd in readMask as well? */
  }
  UNLOCK_IO_READ_MUTEX;
  UNLOCK_CM_MUTEX;
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcSingleMsgHnd(time)
 *
 * DESCRIPTION: 
 *
 * INPUTS: struct timeval *time;
 *
 * OUTPUTS: int
 *
 * NOTES:
 *
 * x_ipcConnectionListGlobal was inited from x_ipcServerGlobal
 *
 *****************************************************************************/

static X_IPC_RETURN_VALUE_TYPE x_ipcSingleMsgHnd(struct timeval *time)
{
  struct timeval forever;

  if (time == NULL) {
    forever.tv_usec = 0;
    forever.tv_sec = WAITFOREVER;
    time = &forever;
  }
  return x_ipc_waitForReplyFromTime(NULL, NULL, 0, time, NO_FD);
}

#ifndef NMP_IPC
/******************************************************************************
 *
 * FUNCTION: int x_ipcHandleFdInput(int fd)
 *
 * DESCRIPTION: 
 *
 * INPUTS: struct timeval *time;
 *
 * OUTPUTS: int
 *
 * NOTES:
 * x_ipcConnectionListGlobal was inited from x_ipcServerGlobal
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE x_ipcHandleFdInput(int fd)
{
  fd_set readMask;
  CONNECTION_PTR connection = NULL;
  
  FD_ZERO(&readMask);
  FD_SET(fd, &readMask);
  
  x_ipc_acceptConnections(&readMask);
  
  LOCK_CM_MUTEX;
  connection = (CONNECTION_PTR) 
    x_ipc_hashTableFind((void *)&fd, GET_C_GLOBAL(moduleConnectionTable));
  UNLOCK_CM_MUTEX;
  if (connection) {
    handleModuleInput(fd, connection);
  } else {
    (void)x_ipc_execFdHnd(fd);
  }
  
  return 1; /* Success */
}
#endif

/******************************************************************************
 *
 * FUNCTION: void *x_ipc_decodeDataInLanguage (dataMsg, decodeFormat, language)
 *
 * DESCRIPTION: Decode dataMsg using format information, creating (and
 *              returning) a new data structure.
 *
 * INPUTS:
 * DATA_MSG_PTR dataMsg;
 * CONST_FORMAT_PTR decodeFormat;
 * HND_LANGUAGE_ENUM language (C, LISP, RAW)
 *
 * OUTPUTS: void * (the newly created, decoded data)
 *
 *****************************************************************************/

void *x_ipc_decodeDataInLanguage (DATA_MSG_PTR dataMsg, CONST_FORMAT_PTR decodeFormat,
			    HND_LANGUAGE_ENUM language)
{
  void *data;

  switch (language) {
  case C_LANGUAGE:
    data = x_ipc_dataMsgDecodeMsg(decodeFormat, dataMsg, FALSE); break;

#ifdef LISP
  case LISP_LANGUAGE: 
    { BUFFER_TYPE buffer;
      long (*lispDecodeMsgFn)(CONST_FORMAT_PTR, BUFFER_PTR);
      buffer.bstart = 0;
      buffer.buffer = NULL;
      
      LOCK_M_MUTEX;
      lispDecodeMsgFn = GET_M_GLOBAL(lispDecodeMsgGlobal);
      UNLOCK_M_MUTEX;
      if (decodeFormat && dataMsg->msgTotal) {
	buffer.buffer = dataMsg->msgData;
	data = (void *)(*lispDecodeMsgFn)(decodeFormat, &buffer);
      } else {
	data = (void *)(*lispDecodeMsgFn)(0, NULL);
      }
      break;
    }
#endif /* LISP */
  default: 
    data = NULL;
    X_IPC_MOD_ERROR1("Decode in unknown language (%d)\n", language);
  }
  return data;
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipc_processQueryReply(dataMsg, msg, sel, reply)
 *
 * DESCRIPTION: 
 * Decodes dataMsg using msg format information and storing it in reply. 
 * Reclaims dataMsg storage.
 *
 * INPUTS:
 * DATA_MSG_PTR dataMsg;
 * MSG_PTR msg;
 * int sel; (which format to choose)
 * void *reply;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

static X_IPC_RETURN_VALUE_TYPE x_ipc_processQueryReply(DATA_MSG_PTR dataMsg, 
					       MSG_PTR msg, BOOLEAN sel,
					       void *reply)
{
  char *r2= NULL;
  CONST_FORMAT_PTR decodeFormat;
  int amount;
#ifdef LISP
  char *lispDataFlag;
#endif  

  if (dataMsg->msgTotal == 0) {
    x_ipc_dataMsgFree(dataMsg);
    return NullReply;
  }
  
  decodeFormat = (sel ? msg->msgData->msgFormat : msg->msgData->resFormat);
#ifdef LISP
  LOCK_M_MUTEX;
  lispDataFlag = LISP_DATA_FLAG();
  UNLOCK_M_MUTEX;
  if (reply == lispDataFlag) {
    /* The LISP decode function sets the data to a LISP global variable */
    (void)x_ipc_decodeDataInLanguage(dataMsg, decodeFormat, LISP_LANGUAGE);
  } else
#endif /* LISP */
    {
    r2 = (char *)x_ipc_decodeDataInLanguage(dataMsg, decodeFormat, C_LANGUAGE);
    amount = x_ipc_dataStructureSize(decodeFormat);

    /* Free only if it was allocated. */
    if (r2 != reply) {
      BCOPY(r2, reply, amount);
      x_ipcFree((char *)r2);
    }
  }
  
  x_ipc_dataMsgFree(dataMsg);
  return Success;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReply(ref, reply, timeout)
 *
 * DESCRIPTION: Loops to handle messages while waiting for a query reply.
 *
 * INPUTS:
 * X_IPC_REF_PTR ref;
 * char *reply;
 * long timeout;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 * NOTES: Modified from x_ipcSingleMsgHnd
 *
 *****************************************************************************/

static int x_ipc_testMsgRef(int *refId, DATA_MSG_PTR dataMsg)
{
  return (dataMsg->msgRef == *refId);
}

/* How many bytes are available for reading on this file descriptor */
static int chars_available (int fd)
{
#ifndef WIN32
  int available;

  if (ioctl(fd, FIONREAD, &available) == 0)
    return available;
  else
#endif
    return -1;
}

static BOOLEAN x_ipc_handleModuleReply(int fd, BOOLEAN sel,
				       X_IPC_REF_PTR ref,
				       char *replyData,
				       CONNECTION_PTR connection,
				       X_IPC_RETURN_VALUE_TYPE *status)
{
  int refId;
  int sd = connection->readSd;
  DATA_MSG_PTR dataMsg;
  int replySize;
  
  refId = (ref ? ref->refId : NO_REF);
  replySize = (ref ? x_ipc_dataStructureSize(ref->msg->msgData->resFormat) : 0);

  if (connection->readSd == fd) {
    do {
      switch(x_ipc_dataMsgRecv(connection->readSd, &dataMsg, refId,
			       replyData, replySize)){
      case StatOK:
	if (dataMsg->intent == QUERY_REPLY_INTENT) {
	  if (dataMsg->msgRef == refId) {
	    *status = x_ipc_processQueryReply(dataMsg, ref->msg, sel, replyData);
	    return TRUE;
	  } else if (!x_ipc_handleQueryNotification(dataMsg)) {
	    LOCK_CM_MUTEX;
	    x_ipc_listInsertItem((char *)dataMsg, GET_C_GLOBAL(pendingReplies));
	    UNLOCK_CM_MUTEX;
	    return FALSE;
	  }
	} else {
	  LOCK_CM_MUTEX;
	  enqueueMsg(&GET_C_GLOBAL(msgQueue), connection, dataMsg);
	  UNLOCK_CM_MUTEX;
	}
	break;
      case StatError:
	X_IPC_MOD_ERROR1("ERROR: Reading Data Message in x_ipc_waitForReplyFrom %d.\n",
			 connection->readSd);
      case StatEOF:
	/* If we have disconnected, then just return.  */
	LOCK_CM_MUTEX;
	if (!x_ipc_hashTableFind((void *)&sd,
				 GET_C_GLOBAL(moduleConnectionTable))) {
	  UNLOCK_CM_MUTEX;
	  return FALSE;
	}
	UNLOCK_CM_MUTEX;
	x_ipcHandleClosedConnection(connection->readSd, connection);
	break;
      case StatSendEOF:
      case StatSendError:
      case StatRecvEOF:
      case StatRecvError:
#ifndef TEST_CASE_COVERAGE
      default:
#endif
	X_IPC_MOD_ERROR("Internal Error: x_ipc_waitForReplyFrom: UNKNOWN INTENT\n");
      }
    } while (chars_available(fd) > 0);
  }
  return FALSE;
}

static BOOLEAN QUEUED_MSGS (void)
{
  BOOLEAN queued;

  LOCK_CM_MUTEX;
  queued = (GET_C_GLOBAL(msgQueue).numMessages > 0);
  UNLOCK_CM_MUTEX;
  return queued;
}

/*****************
 * Returns Timeout if "timeout" seconds elapse without receiving a message.
 *  WAITFOREVER always waits until reply received (a very long time, at least).
 ****************/
X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReplyFromTime(X_IPC_REF_PTR ref,
						   void *reply, BOOLEAN sel, 
						   struct timeval *timeout,
						   int fd)
{
  DATA_MSG_PTR dataMsg = NULL;
  X_IPC_RETURN_VALUE_TYPE status = Success;
  fd_set readMask;
  int ret, i, timeoutForTimer, maxConnection;
  QUEUED_MSG_PTR queuedMsg;
  HASH_TABLE_PTR connectionTable;

  CONNECTION_PTR connection=NULL;
  unsigned long nextTrigger, waitTimeout, now;
  long relTimeout;
  struct timeval time;
  BOOLEAN isPrimaryThread = FALSE;

   /* do this using macros so that the compiler can statically detect
      and optimize away if (!TRUE) blocks.  (hopefully the compiler is
      that smart). */
#ifdef IPC_ALLOW_DISCONNECTED_EVENT_HANDLING
   BOOLEAN connectedToCentral = x_ipc_isValidServerConnection();
#  define CONNECTED_TO_CENTRAL (connectedToCentral)
#else
#  define CONNECTED_TO_CENTRAL (TRUE)
#endif

   if (!CONNECTED_TO_CENTRAL && ref != NULL) {
     /* I think that ref is non-NULL when we are waiting for a particular reply.
	Since no messages can come in while we're not connected to central,
	we should signal an error. */
     X_IPC_MOD_ERROR("ERROR: connect to central before waiting for a reply");
   }

  TIME_TO_MSECS(*timeout, waitTimeout);
  if (waitTimeout != WAITFOREVER) waitTimeout += x_ipc_timeInMsecs();

  do {
#ifdef THREADED
    if (!isPrimaryThread) {
      isPrimaryThread = (tryLockMutex(&GET_M_GLOBAL(selectMutex))
			 == Mutex_Success);
    }
#else
    isPrimaryThread = TRUE;
#endif
    if (isPrimaryThread) {
      nextTrigger = ipcNextTime();

      do {
	/* Doing this check every time in the inner loop is somewhat wasteful,
	   but there are rare situations where triggering a timer can result in
	   a message being placed on the pending queue.  If it isn't handled here,
	   IPC can block forever */
	if (CONNECTED_TO_CENTRAL && ref != NULL) {
	  LOCK_CM_MUTEX;
	  dataMsg = (DATA_MSG_PTR)
	    x_ipc_listMemReturnItem((LIST_ITER_FN) x_ipc_testMsgRef,
				    (char *)&ref->refId, 
				    GET_C_GLOBAL(pendingReplies));
	  if (dataMsg) {
	    x_ipc_listDeleteItem((char *)dataMsg, (GET_C_GLOBAL(pendingReplies)));
	    UNLOCK_CM_MUTEX;
	    status = x_ipc_processQueryReply(dataMsg, ref->msg, sel, reply);
	    if (nextTrigger != WAITFOREVER) ipcTriggerTimers();
	    UNLOCK_SELECT_MUTEX;
	    return status;
	  }
	  UNLOCK_CM_MUTEX;
	}
	if (fd == NO_FD) {
	  LOCK_CM_MUTEX;
	  readMask = (GET_C_GLOBAL(x_ipcConnectionListGlobal));
	  UNLOCK_CM_MUTEX;
	} else {
	  FD_ZERO(&readMask);
	  FD_SET(fd, &readMask);
	}

	/* Run any overdue triggers now */
	if (nextTrigger != WAITFOREVER && nextTrigger <= x_ipc_timeInMsecs()) {
	  ipcTriggerTimers();
	  nextTrigger = ipcNextTime();
	}

	/* Determine how long to stay in the select -- 
	   either due to the timeout, or due to the timers */
	if (CONNECTED_TO_CENTRAL && QUEUED_MSGS()) {
	  /* There are messages in the queue. 
	     Don't wait -- but still call select to get any new messages */
	  time.tv_sec = 0; time.tv_usec = 0;
	} else if (nextTrigger == WAITFOREVER && waitTimeout == WAITFOREVER) {
	  time.tv_sec = WAITFOREVER; time.tv_usec = 0;
	} else {
	  now = x_ipc_timeInMsecs();
	  if (waitTimeout == WAITFOREVER || nextTrigger <= waitTimeout) {
	    /* If we get here, nextTrigger cannot be WAITFOREVER */
	    relTimeout = nextTrigger - now;
	    if (relTimeout < 0) relTimeout = 0;
	  } else {
	    /* If we get here, either nextTrigger or waitTimeout is not 
	       infinite, and nextTrigger is not less than waitTimeout, so 
	       waitTimeout must not be infinite */
	    relTimeout = waitTimeout - now;
	    if (relTimeout < 0) {
	      ret = 0;
	      break;
	    }
	  }
	  MSECS_TO_TIME(relTimeout, time);
	}

	ret = select(FD_SETSIZE, &readMask, (fd_set *)NULL, (fd_set *)NULL,
		     ((time.tv_sec == WAITFOREVER) ? (struct timeval *)NULL 
		      : &time));

	timeoutForTimer = (ret == 0 && nextTrigger <= waitTimeout);
	if (timeoutForTimer) {
	  ipcTriggerTimers();
	  nextTrigger = ipcNextTime();
	}
      }
#ifdef _WINSOCK_
      while (!(CONNECTED_TO_CENTRAL && QUEUED_MSGS())
	     && ((ret == SOCKET_ERROR && WSAGetLastError() == WSAEINTR)
		 || timeoutForTimer));
#else
      while (!(CONNECTED_TO_CENTRAL && QUEUED_MSGS())
	     && ((ret < 0 && errno == EINTR) || timeoutForTimer));
#endif

      if (ret == 0) {
	if (!(CONNECTED_TO_CENTRAL && QUEUED_MSGS())) {
	  UNLOCK_SELECT_MUTEX;
	  return TimeOut;
	}
      } else if (ret < 1) {
	X_IPC_MOD_ERROR("ERROR: on select in WaitForReplyFrom.\n");
	UNLOCK_SELECT_MUTEX;
	return Failure;
      }
    
      x_ipc_acceptConnections(&readMask);
      
      LOCK_CM_MUTEX;
      maxConnection = GET_C_GLOBAL(maxConnection);
      connectionTable = GET_C_GLOBAL(moduleConnectionTable);
      UNLOCK_CM_MUTEX;
      for (i=0; i<=maxConnection; i++) {
	if (FD_ISSET(i, &readMask)) {
	  if (CONNECTED_TO_CENTRAL
	      /* using pointer as truth value */
	      && (connection = (CONNECTION_PTR) 
		  x_ipc_hashTableFind((void *)&i, connectionTable)))
	    {
	      FD_CLR((unsigned)i, &readMask);
	      if (x_ipc_handleModuleReply(i, sel, ref, (char *)reply,
					  connection, &status)) {
		UNLOCK_SELECT_MUTEX;
#ifdef THREADED
		/* Ping the other threads */
		pingThreads(&GET_M_GLOBAL(ping));
#endif
		return status;
	      }
	    } else {
	      if (x_ipc_execFdHnd(i)) {
		FD_CLR((unsigned)i, &readMask);
	      }
	    }
	}
      }

#ifdef THREADED
	/* Ping the other threads */
      pingThreads(&GET_M_GLOBAL(ping));
#endif
      if (CONNECTED_TO_CENTRAL) {
	/* Pick the first message off the queue */
	LOCK_CM_MUTEX;
	queuedMsg = dequeueMsg(&GET_C_GLOBAL(msgQueue));
	UNLOCK_CM_MUTEX;
	if (queuedMsg) {
	  x_ipc_execHnd(queuedMsg->connection, queuedMsg->dataMsg);
	}
      }

      if (nextTrigger != WAITFOREVER) ipcTriggerTimers();
    } /* if (isPrimaryThread) */
#ifdef THREADED
    else {
      if (CONNECTED_TO_CENTRAL && ref != NULL) {
	LOCK_CM_MUTEX;
	dataMsg = (DATA_MSG_PTR)
	  x_ipc_listMemReturnItem((LIST_ITER_FN) x_ipc_testMsgRef,
				  (char *)&ref->refId, 
				  GET_C_GLOBAL(pendingReplies));
	if (dataMsg) {
	  x_ipc_listDeleteItem((char *)dataMsg, (GET_C_GLOBAL(pendingReplies)));
	  UNLOCK_CM_MUTEX;
	  status = x_ipc_processQueryReply(dataMsg, ref->msg, sel, reply);
	  return status;
	}
	UNLOCK_CM_MUTEX;
      }
      if (timeout != NULL && timeout->tv_sec == WAITFOREVER) timeout = NULL;
      status = (waitForPing(&GET_M_GLOBAL(ping), timeout) == Ping_Timeout 
		? TimeOut : Success);
    }
#endif
  } while (ref != NULL);
  if (isPrimaryThread) UNLOCK_SELECT_MUTEX;
  return status;
}

X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReplyFrom(X_IPC_REF_PTR ref, void *reply, 
					       BOOLEAN sel, long timeout,
					       int fd)
{
  struct timeval time;
  
  time.tv_usec = 0;
  time.tv_sec = timeout;
  return x_ipc_waitForReplyFromTime(ref, reply, sel, &time, fd);
}

X_IPC_RETURN_VALUE_TYPE x_ipc_waitForReply(X_IPC_REF_PTR ref, void *reply)
{
  return x_ipc_waitForReplyFrom(ref, reply, FALSE, WAITFOREVER, NO_FD);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcHandleMessage(sec)
 *
 * DESCRIPTION: 
 *
 * INPUTS: long sec;
 *
 * OUTPUTS: X_IPC_RETURN_VALUE_TYPE
 *
 *****************************************************************************/

/* For NMP_IPC, the time argument is taken to be msecs, not seconds !! */

X_IPC_RETURN_VALUE_TYPE x_ipcHandleMessage(long sec)
{
  struct timeval time;
  
#ifndef IPC_ALLOW_DISCONNECTED_EVENT_HANDLING
  if (!x_ipc_isValidServerConnection()) {
    X_IPC_MOD_ERROR("ERROR: Must connect to server first.\n");
    return Failure;
  }
#endif
  if (
#ifdef IPC_ALLOW_DISCONNECTED_EVENT_HANDLING
      x_ipc_isValidServerConnection() &&
#endif
      !GET_M_GLOBAL(expectedWaitGlobal)) {
    X_IPC_MOD_WARNING(
		  "WARNING: call x_ipcWaitUntilReady before listening for messages\n");
  }
  
  if (sec == WAITFOREVER) {
    return x_ipcSingleMsgHnd((struct timeval *)NULL);
  } else {
#ifndef NMP_IPC
    time.tv_sec = sec;
    time.tv_usec = 0;
#else /* NMP_IPC treats the argument as msecs */
    MSECS_TO_TIME(sec, time);
#endif
    return x_ipcSingleMsgHnd(&time);
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcModuleListen()
 *
 * DESCRIPTION: 
 *
 * INPUTS: none.
 * 
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcModuleListen(void)
{
#ifndef IPC_ALLOW_DISCONNECTED_EVENT_HANDLING
  if (!x_ipc_isValidServerConnection()) {
    X_IPC_MOD_ERROR("ERROR: Must connect to server first (x_ipcModuleListen).\n");
    return;
  }
#endif
  if (!GET_M_GLOBAL(expectedWaitGlobal)) {
    X_IPC_MOD_WARNING(
		  "WARNING: call x_ipcWaitUntilReady before listening for messages\n");
  }
  
  for (;;) {
    (void)x_ipcSingleMsgHnd((struct timeval *)NULL);
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcRegisterExitHandler(proc)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 * int (*proc)();
 * 
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcRegisterExitProc(void (*proc)(void))
{
  /* make sure the data structures are initialzied. */
  x_ipcModuleInitialize();
  LOCK_M_MUTEX;
  (GET_M_GLOBAL(x_ipcExitHnd)) = proc;
  UNLOCK_M_MUTEX;
}

/****************************************************************************/

void x_ipcDirectResource(const char *name)
{
  int port;
  DIRECT_TYPE direct;
  
  LOCK_CM_MUTEX;
  if (GET_C_GLOBAL(listenPort) == NO_FD) {
    port = LISTEN_PORT;
    while ((port < LISTEN_PORT_MAX) && 
	   !x_ipc_listenAtPort(&port, &(GET_C_GLOBAL(listenPort))))
      port++;
    
    if (port == LISTEN_PORT_MAX) {
      X_IPC_MOD_ERROR( "ERROR: Exceeded available ports for direct connection.\n");
      UNLOCK_CM_MUTEX;
      return;
    }
    (GET_M_GLOBAL(directFlagGlobal)) = TRUE;
    FD_SET((GET_C_GLOBAL(listenPort)), 
	   &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    FD_SET((GET_C_GLOBAL(listenPort)), 
	   &(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
    /* Need to keep track of the maximum.  */
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
				       (GET_C_GLOBAL(listenPort)));
#if !defined(NO_UNIX_SOCKETS) || defined(VX_PIPES)
    x_ipc_listenAtSocket(port, &(GET_C_GLOBAL(listenSocket)));
    FD_SET((GET_C_GLOBAL(listenSocket)), 
	   &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
    FD_SET((GET_C_GLOBAL(listenSocket)), 
	   &(GET_C_GLOBAL(x_ipcListenMaskGlobal)));
    /* Need to keep track of the maximum.  */
    GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
				       GET_C_GLOBAL(listenSocket));
#endif
    
    GET_C_GLOBAL(listenPortNum) = port;
  }
  
  direct.port = GET_C_GLOBAL(listenPortNum);
  direct.name = name;
  UNLOCK_CM_MUTEX;
  
  x_ipcInform(X_IPC_DIRECT_RES_INFORM, &direct);
}

/****************************************************************************/

void freeDirectList (MSG_PTR msg)
{
  if (msg->directList) {
    x_ipcFreeReply(X_IPC_DIRECT_MSG_QUERY, msg->directList);
    msg->directList->numHandlers = 0;
  }
}

static void setDirectInfo (MSG_PTR msg, DIRECT_MSG_PTR directInfo)
{
  DIRECT_MSG_HANDLER_PTR directMsgHandler;
  CONNECTION_PTR connection, currentConnection;
  int i, num, maxConnection;
  HASH_TABLE_PTR connectionTable;

  if (msg->direct) {
    for (num=0; num<directInfo->numHandlers; num++) {
      connection = NULL;
      directMsgHandler = &directInfo->handlers[num];
      LOCK_CM_MUTEX;
      maxConnection = GET_C_GLOBAL(maxConnection);
      connectionTable = GET_C_GLOBAL(moduleConnectionTable);
      UNLOCK_CM_MUTEX;
      for (i=0; i<=maxConnection && !connection; i++) {
	currentConnection = 
	  (CONNECTION_PTR)x_ipc_hashTableFind((void *)&i, connectionTable);
	if (currentConnection &&
	    directMsgHandler->port == currentConnection->port &&
	    x_ipc_strKeyEqFunc(directMsgHandler->host,
			       currentConnection->host)) {
	  connection = currentConnection;
	}
      }
      if (!connection) {
	connection = NEW(CONNECTION_TYPE);
	connection->port = directMsgHandler->port;
	connection->module = directMsgHandler->module;
	connection->host = directMsgHandler->host;
	/* So they won't be freed */
	directMsgHandler->host = directMsgHandler->module = NULL;
	if (!x_ipc_connectAt(connection->host, connection->port,
			     &(connection->readSd), &(connection->writeSd))) {
	  X_IPC_MOD_ERROR2("ERROR: Can not establish direct connection to %s at %ld.\n",
			   connection->host, (long)connection->port);
	  return;
	} else {
	  LOCK_CM_MUTEX;
	  GET_M_GLOBAL(directFlagGlobal) = TRUE;
	  X_IPC_MOD_WARNING3( "Direct Connection Established for %s to %s at sd: %d\n",
			      msg->msgData->name, connection->module, 
			      connection->readSd);
	  FD_SET(connection->readSd, &(GET_C_GLOBAL(x_ipcConnectionListGlobal)));
	  /* Need to keep track of the maximum.  */
	  GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
					     connection->readSd);
	  GET_C_GLOBAL(maxConnection) =  MAX(GET_C_GLOBAL(maxConnection),
					     connection->writeSd);
	  x_ipc_hashTableInsert((void *)&(connection->readSd),  
				sizeof(connection->readSd), (char *)connection,
				(GET_C_GLOBAL(moduleConnectionTable)));
	  UNLOCK_CM_MUTEX;
	}
      }
      directMsgHandler->readSd = connection->readSd;
      directMsgHandler->writeSd = connection->writeSd;
    }
  }
  if (!msg->directList) {
    msg->directList = NEW(DIRECT_MSG_TYPE);
  } else {
    freeDirectList(msg);
  }
  *(msg->directList) = *directInfo;
}

static void directMsgWatchHnd (X_IPC_REF_PTR Ref, DIRECT_MSG_PTR directInfo)
{
  MSG_PTR msg;
  const char *watchMsgName;

  watchMsgName = x_ipcReferenceName(Ref);
  LOCK_M_MUTEX;
  msg = GET_MESSAGE(&(watchMsgName[strlen(VAR_WATCH_PREFIX)]));
  UNLOCK_M_MUTEX;

  setDirectInfo(msg, directInfo);

  invokeChangeHandlers(msg);

  /* Just free the top-level structure -- the list is being used 
     in the directList */
  free(directInfo);
}

void x_ipc_establishDirect(MSG_PTR msg)
{
  DIRECT_MSG_TYPE directInfo;
  X_IPC_RETURN_VALUE_TYPE status;

  status = x_ipcQueryCentral(X_IPC_DIRECT_MSG_QUERY, (void *)&msg->msgData->name,
			   (void *)&directInfo);
  if (status == Success && directInfo.numHandlers >= 0) {
    setDirectInfo(msg, &directInfo);
  } else {
    if (msg->direct) {
      X_IPC_MOD_WARNING1("No handlers for direct message %s\n",
			 msg->msgData->name);
      msg->directList = NEW(DIRECT_MSG_TYPE);
      msg->directList->handlers    = NULL;
      msg->directList->numHandlers = 0;
    }
  }
  x_ipcWatchVar(msg->msgData->name, DIRECT_MSG_FORMAT, directMsgWatchHnd);

  /* Don't free the data -- it is being used in the directList */
}


/******************************************************************************
 *
 * FUNCTION: int x_ipcGetVersionMajor(void)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 * 
 * OUTPUTS: version number.
 *
 *****************************************************************************/

int32 x_ipcGetVersionMajor(void)
{
  return X_IPC_VERSION_MAJOR;
}


/******************************************************************************
 *
 * FUNCTION: int x_ipcGetVersionMinor(void)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 * 
 * OUTPUTS: version number.
 *
 *****************************************************************************/

int32 x_ipcGetVersionMinor(void)
{
  return X_IPC_VERSION_MINOR;
}

/******************************************************************************
 *
 * FUNCTION: void x_ipcAddEventHandr(int fd, X_IPC_FD_HND_FN hnd, void *clientData)
 *
 * DESCRIPTION: Add the handler to handle inputs on the file descriptor fd;
 *              The handler function is called with the file descriptor and
 *              the client data (which can be a pointer to any user-specified 
 *              data).
 *
 * INPUTS:
 * 
 * OUTPUTS:
 *
 *****************************************************************************/

void x_ipcAddEventHandler(int fd, X_IPC_FD_HND_FN hnd, void *clientData)
{
  FD_HND_PTR fdHndData;

  LOCK_M_MUTEX;
  fdHndData = (FD_HND_PTR)GET_FD_HANDLER(fd);
  UNLOCK_M_MUTEX;
  if (fdHndData) {
    if (fdHndData->hndProc != hnd) {
      X_IPC_MOD_WARNING1("WARNING: Handler function replaced for fd %d\n", fd);
      fdHndData->hndProc = hnd;
    }
    if (fdHndData->clientData != clientData) {
      X_IPC_MOD_WARNING1("WARNING: Replacing client data for fd %d\n", fd);
      fdHndData->clientData = clientData;
    }
  } else {
    fdHndData = NEW(FD_HND_TYPE);
    fdHndData->fd = fd;
    fdHndData->hndProc = hnd;
    fdHndData->clientData = clientData;

    LOCK_CM_MUTEX;
    x_ipc_hashTableInsert((const void *)&fd, sizeof(fd), fdHndData,
			  GET_M_GLOBAL(externalFdTable));
    FD_SET(fd,&GET_M_GLOBAL(externalMask));
    FD_SET(fd,&GET_C_GLOBAL(x_ipcConnectionListGlobal));
    GET_C_GLOBAL(maxConnection) =  MAX(fd, GET_C_GLOBAL(maxConnection));
    UNLOCK_CM_MUTEX;
  }
}

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcRemoveEventHandler(int fd)
 *
 * DESCRIPTION: Remove the handler to handle events for the file descriptor fd
 *
 * INPUTS: int fd; 
 * 
 * OUTPUTS: none
 *
 *****************************************************************************/

void x_ipcRemoveEventHandler(int fd)
{
  FD_HND_PTR fdHndData;

  LOCK_CM_MUTEX;
  fdHndData = (FD_HND_PTR)x_ipc_hashTableRemove((const void *)&fd,
						GET_M_GLOBAL(externalFdTable));
  if (fdHndData) x_ipcFree((char *)fdHndData);
  FD_CLR((unsigned)fd,&GET_M_GLOBAL(externalMask));
  FD_CLR((unsigned)fd,&GET_C_GLOBAL(x_ipcConnectionListGlobal));
  UNLOCK_CM_MUTEX;
}

static MSG_PTR msgFromDataMsg (DATA_MSG_PTR dataMsg)
{
  HND_PTR hnd;

  LOCK_CM_MUTEX;
  hnd = (HND_PTR)idTableItem(dataMsg->intent, GET_C_GLOBAL(hndIdTable));
  UNLOCK_CM_MUTEX;
  return (!hnd ? NULL : (hnd->msg ? hnd->msg : (MSG_PTR)x_ipc_listFirst(hnd->msgList)));
}

static int32 numPending (MSG_PTR msg, MSG_QUEUE_PTR msgQueue)
{
  int32 i, num;
  MSG_PTR msg2;

  for (i=0, num=0; i<msgQueue->numMessages; i++ ) {
    msg2 = msgFromDataMsg(msgQueue->messages[i].dataMsg);
    if (msg2 && msg2 == msg) num++;
  }
  return num;
}

/* Remove the oldest message of the same type as "msg".
   Move up all entries of the same message, to ensure fairness.
   Return the original position of the newest message, which is where
   the current message should be placed.
 */
static int removeOldest (MSG_PTR msg, MSG_QUEUE_PTR msgQueue)
{
  int lastMsg, i;
  MSG_PTR msg2;

  X_IPC_MOD_WARNING2("WARNING: Removing a %s message from queue (has %d)\n",
		     msg->msgData->name, numPending(msg, msgQueue));

  for (lastMsg=-1, i=msgQueue->numMessages-1; i>=0; i--) {
    msg2 = msgFromDataMsg(msgQueue->messages[i].dataMsg);
    if (msg2 && msg == msg2) {
      if (lastMsg == -1) {
	/* This is the oldest message -- free it */
	x_ipc_dataMsgFree(msgQueue->messages[i].dataMsg);
      } else {
	msgQueue->messages[lastMsg] = msgQueue->messages[i]; /* Move up */
      }
      lastMsg = i;
    }
  }
  return lastMsg;
}

static void resizeMsgQueue (MSG_QUEUE_PTR msgQueue)
{
  msgQueue->queueSize += MSG_QUEUE_INCR;
  msgQueue->messages  = (QUEUED_MSG_PTR)realloc(msgQueue->messages,
						(sizeof(QUEUED_MSG_TYPE)*
						 msgQueue->queueSize));
}

void initMsgQueue (MSG_QUEUE_PTR msgQueue)
{
  msgQueue->numMessages = 0;
  msgQueue->queueSize = MSG_QUEUE_INCR;
  msgQueue->messages = (QUEUED_MSG_PTR)x_ipcMalloc(sizeof(QUEUED_MSG_TYPE) *
						   MSG_QUEUE_INCR);
}

/* The end of the queue is at the front of the array -- makes things
   slightly easier to code.
*/
void enqueueMsg (MSG_QUEUE_PTR msgQueue,
		 CONNECTION_PTR connection, DATA_MSG_PTR dataMsg)
{
  int i, position, moveUp=TRUE;
  MSG_PTR msg, msg2;

  if (msgQueue->numMessages == 0) {
    msgQueue->messages[0].connection = connection;
    msgQueue->messages[0].dataMsg = dataMsg;
  } else {
    if (msgQueue->numMessages == msgQueue->queueSize) resizeMsgQueue(msgQueue);

    /* Deal with pending limits and priority... */
    position = 0;
    msg = msgFromDataMsg(dataMsg);
    if (msg) {
      if (msg->limit > 0 && 
	  msg->limit <= msgQueue->numMessages && /* Quick check */
	  msg->limit <= numPending(msg, msgQueue)) {
	/* Have too many messages of the give type -- remove the oldest */
	position = removeOldest(msg, msgQueue);
	moveUp = FALSE;
      } else if (msg->priority != DEFAULT_PRIORITY) {
	/* Put messages in priority order */
	for (i=msgQueue->numMessages-1; i>=0; i--) {
	  msg2 = msgFromDataMsg(msgQueue->messages[i].dataMsg);
	  if (!msg2 || msg->priority > msg2->priority) {
	    position = i + 1;
	    break;
	  }
	}
      }
    }

    /* Not all that efficient, but hopefully will not be used often... */
    if (moveUp)
      for (i=msgQueue->numMessages; i>position; i--) {
	msgQueue->messages[i] = msgQueue->messages[i-1];
      }
    msgQueue->messages[position].connection = connection;
    msgQueue->messages[position].dataMsg = dataMsg;
  }
  if (moveUp) msgQueue->numMessages++;
}

/* The front of the queue is at the end of the array -- makes things
   slightly easier to code 
*/
QUEUED_MSG_PTR dequeueMsg (MSG_QUEUE_PTR msgQueue)
{
  if (msgQueue->numMessages > 0) {
    msgQueue->numMessages--;
    return &msgQueue->messages[msgQueue->numMessages];
  } else {
    return NULL;
  }
}

/* For debugging only */
void printMsgQueue (void);
void printMsgQueue (void)
{
  int i;
  MSG_PTR msg;

  LOCK_CM_MUTEX;
  for (i=GET_C_GLOBAL(msgQueue).numMessages-1; i>=0; i--) {
    msg = msgFromDataMsg(GET_C_GLOBAL(msgQueue).messages[i].dataMsg);
    fprintf(stderr, "%d: %s [%d] ", i, msg->msgData->name, 
	    GET_C_GLOBAL(msgQueue).messages[i].dataMsg->msgRef);
  }
  UNLOCK_CM_MUTEX;
  fprintf(stderr, "\n");
}

void insertConnect(LIST_PTR *listPtr, 
		   char *msgName, X_IPC_HND_FN connectHandler,
		   CONNECT_HANDLE_TYPE handler, void *clientData)
{
  CONNECT_DATA_PTR data;

  if (!*listPtr) *listPtr = x_ipc_listCreate();
  for (data = (CONNECT_DATA_PTR)x_ipc_listFirst(*listPtr); data;
       data = (CONNECT_DATA_PTR)x_ipc_listNext(*listPtr)) {
    /* Found an existing subscription -- change the data */
    if (data->handler == handler) {
      data->clientData = clientData;
      return;
    }
  }
  /* A new subscription */
  data = NEW(CONNECT_DATA_TYPE);
  data->handler = handler; data->clientData = clientData;
  x_ipc_listInsertItem(data, *listPtr);
  if (x_ipc_listLength(*listPtr) == 1)
    x_ipcRegisterHandler(msgName, msgName, connectHandler);
}

void removeConnect(LIST_PTR list, char *msgName, CONNECT_HANDLE_TYPE handler)
{
  CONNECT_DATA_PTR data;

  if (list && x_ipc_listLength(list) > 0) {
    for (data = (CONNECT_DATA_PTR)x_ipc_listFirst(list); data;
	 data = (CONNECT_DATA_PTR)x_ipc_listNext(list)) {
      if (data->handler == handler) {
	x_ipc_listDeleteItem(data, list);
	x_ipcFree((char *)data);
	break;
      }
    }
  }
  if (x_ipc_listLength(list) == 0)
    x_ipcDeregisterHandler(msgName, msgName);

}

static void invokeChangeHandlers (MSG_PTR msg)
{
  CHANGE_DATA_PTR changeData;
  LIST_PTR list;

  LOCK_CM_MUTEX;
  list = GET_C_GLOBAL(changeNotifyList);
  UNLOCK_CM_MUTEX;
  for (changeData = (CHANGE_DATA_PTR)x_ipc_listFirst(list); changeData;
       changeData = (CHANGE_DATA_PTR)x_ipc_listNext(list)) {
    if (x_ipc_strKeyEqFunc(msg->msgData->name, changeData->msgName)) {
      (changeData->handler)(msg->msgData->name, msg->directList->numHandlers,
			    changeData->clientData);
    }
  }
}

/* How many change handlers currently subscribed for this message */
static unsigned int numChangeHandlers (const char *msgName)
{
  CHANGE_DATA_PTR data;
  int num = 0;
  LIST_PTR list;

  LOCK_CM_MUTEX;
  list = GET_C_GLOBAL(changeNotifyList);
  UNLOCK_CM_MUTEX;
  if (list && x_ipc_listLength(list) > 0) {
    for (data = (CHANGE_DATA_PTR)x_ipc_listFirst(list); data;
	 data = (CHANGE_DATA_PTR)x_ipc_listNext(list)) {
      if (x_ipc_strKeyEqFunc(msgName, data->msgName)) num++;
    }
  }
  return num;
}

void insertChange (const char *msgName,
		   CHANGE_HANDLE_TYPE handler, void *clientData)
{
  CHANGE_DATA_PTR data;
  LIST_PTR *listPtr;
  MSG_PTR msg;

  LOCK_CM_MUTEX;
  listPtr = &GET_C_GLOBAL(changeNotifyList);
  UNLOCK_CM_MUTEX;
  if (!*listPtr) *listPtr = x_ipc_listCreate();
  for (data = (CHANGE_DATA_PTR)x_ipc_listFirst(*listPtr); data;
       data = (CHANGE_DATA_PTR)x_ipc_listNext(*listPtr)) {
    /* Found an existing subscription -- just change the data */
    if (data->handler == handler && x_ipc_strKeyEqFunc(msgName, data->msgName)){
      data->clientData = clientData;
      return;
    }
  }

  /* A new subscription */
  data = NEW(CHANGE_DATA_TYPE);
  data->msgName = strdup(msgName);
  data->handler = handler;
  data->clientData = clientData;
  x_ipc_listInsertItem(data, *listPtr);
  if (numChangeHandlers(msgName) == 1) {
    x_ipcInform(IPC_HANDLER_CHANGE_NOTIFY_MSG, &msgName);
    msg = x_ipc_msgFind2(msgName, NULL);
    if (msg && !msg->direct) x_ipc_establishDirect(msg); 
  }
}

void removeChange (const char *msgName, CHANGE_HANDLE_TYPE handler)
{
  CHANGE_DATA_PTR data;
  LIST_PTR list;
  MSG_PTR msg;

  LOCK_CM_MUTEX;
  list = GET_C_GLOBAL(changeNotifyList);
  UNLOCK_CM_MUTEX;
  if (list && x_ipc_listLength(list) > 0) {
    for (data = (CHANGE_DATA_PTR)x_ipc_listFirst(list); data;
	 data = (CHANGE_DATA_PTR)x_ipc_listNext(list)) {
      if (data->handler == handler && 
	  x_ipc_strKeyEqFunc(msgName, data->msgName)) {
	x_ipc_listDeleteItem(data, list);
	x_ipcFree((char *)data->msgName);
	x_ipcFree((char *)data);
	break;
      }
    }
  }

  if (numChangeHandlers(msgName) == 0) {
    msg = x_ipc_msgFind2(msgName, NULL);
    if (!msg || !msg->direct)
      x_ipcUnwatchVar(msgName);
  }
}

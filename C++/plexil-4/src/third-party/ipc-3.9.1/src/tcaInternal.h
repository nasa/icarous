/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: x_ipc includes
 *
 * FILE: x_ipcInternal.h
 *
 * ABSTRACT:
 * 
 * Common include items and defines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: tcaInternal.h,v $
 * Revision 2.9  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.8  2008/07/16 00:09:03  reids
 * Updates for newer (pickier) compiler gcc 4.x
 *
 * Revision 2.7  2002/06/25 16:45:26  reids
 * Added casts to satisfy compiler.
 *
 * Revision 2.6  2002/01/03 20:52:18  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/01/31 17:53:47  reids
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.4  2001/01/10 15:32:51  reids
 * Added the function IPC_subscribeData that automatically unmarshalls
 *   the data before invoking the handler.
 *
 * Revision 2.3  2000/07/27 16:59:12  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.9  1997/03/07 17:49:52  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.3.2.8  1997/01/27 20:38:50  reids
 * On VxWorks, spawn a task that spawns central with the "right" task
 *   parameters, including "centralPriority" and "centralStacksize".
 *
 * Revision 1.3.2.7  1997/01/27 20:10:03  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.3.2.5  1997/01/11 01:21:26  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.3.2.4.6.1  1996/12/24 14:41:49  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.3.2.4  1996/10/24 16:20:04  reids
 * Made defines for the sizes of some of the hash tables and id tables.
 *
 * Revision 1.3.2.3  1996/10/22 18:49:52  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.3.2.2  1996/10/18 18:18:14  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 *
 * Revision 1.3.2.1  1996/10/14 03:54:48  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.3  1996/05/26 04:11:49  reids
 * Added function IPC_dataLength -- length of byte array assd with msgInstance
 *
 * Revision 1.2  1996/05/09 16:44:25  reids
 * Minor cleanups
 *
 * Revision 1.1  1996/05/09 01:02:06  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.5  1996/04/24 19:11:09  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.4  1996/04/03 03:11:11  reids
 * The data logging (-ld) option now works, for both IPC and regular X_IPC msgs.
 * Fixed the interaction between signals and listening for messages.
 * IPC version will not exit if connection to central server is not available.
 * IPC version will not print out stats if module exits (x_ipcModError).
 *
 * Revision 1.3  1996/03/19 03:38:55  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:20:06  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:48  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.106  1996/08/22  16:36:05  rich
 * Check the return code on x_ipcQueryCentral calls.
 *
 * Revision 1.105  1996/08/05  16:07:01  rich
 * Added comments to endifs.
 *
 * Revision 1.104  1996/07/27  21:18:51  rich
 * Close log file on a "quit" typed into central.
 * Fixed problem with deleting handlers.  The hash iteration routine works
 * if the hander causes the hash item to be deleted.
 *
 * Revision 1.103  1996/07/26  18:21:30  rich
 * Check to see if moduleList is non-NULL before dereferencing.
 *
 * Revision 1.102  1996/07/25  22:24:38  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.101  1996/07/24  13:46:12  reids
 * Support for Windows95 (Thanks to Tam Ngo, JSC)
 * When module goes down, do not remove dispatch if still have task tree node.
 * Handle NULL format in x_ipc_dataStructureSize.
 * Add short, byte and ubyte formats for Lisp.
 * Ignore stdin when checking sockets for input.
 *
 * Revision 1.100  1996/07/19  18:14:36  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.99  1996/07/12  13:56:45  reids
 * Fixed how "last child" is reset -- don't add monitors as the last child!
 *
 * Revision 1.98  1996/07/11  17:09:09  reids
 * Need to make the host and message byte order available to LISP
 *
 * Revision 1.97  1996/07/03  21:43:38  reids
 * Have "x_ipcWaitUntilReady" print out any required resources it is waiting for
 *
 * Revision 1.96  1996/06/30  20:17:53  reids
 * Handling of polling monitors was severely broken.
 *
 * Revision 1.95  1996/06/25  20:51:44  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.94  1996/06/13  14:24:25  reids
 * Memory error -- message formats were being freed but not cleared.
 * Conditionally compile out task tree stuff for NMP IPC.
 *
 * Revision 1.93  1996/06/05  19:48:03  reids
 * Registering global vars freed memory that was later being used.
 *
 * Revision 1.92  1996/05/14  22:40:15  rich
 * Need the check for valid context before dereferencing.
 *
 * Revision 1.91  1996/05/09  18:31:40  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.90  1996/05/07  16:49:48  rich
 * Changes for clisp.
 *
 * Revision 1.89  1996/04/04  18:18:16  rich
 * Fixed problems found by the SGI compiler.
 *
 * Revision 1.88  1996/03/19  02:29:36  reids
 * Plugged some more memory leaks; Added test code for enum formatters.
 * Added code to free formatter data structures.
 *
 * Revision 1.87  1996/03/15  21:28:04  reids
 * Added x_ipcQueryNotify, x_ipcAddEventHandler, x_ipcRemoveEventHandler.
 *   Fixed re-registration bug; Plugged memory leaks; Fixed way task trees
 *   are killed; Added support for "enum" format type.
 *
 * Revision 1.86  1996/03/09  06:13:21  rich
 * Fixed problem where lisp could use the wrong byte order if it had to
 * query for a message format.  Also fixed some memory leaks.
 *
 * Revision 1.85  1996/03/05  05:05:00  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.84  1996/03/02  03:22:01  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.83  1996/02/21  18:30:33  rich
 * Created single event loop.
 *
 * Revision 1.82  1996/02/15  18:16:12  rich
 * Update to new version number.
 *
 * Revision 1.81  1996/02/14  22:12:44  rich
 * Eliminate extra variable logging on startup.
 *
 * Revision 1.80  1996/02/14  03:43:27  rich
 * Added setpgrp for sunos.
 *
 * Revision 1.79  1996/02/10  16:50:43  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.78  1996/02/07  00:27:40  rich
 * Add prefix to VERSION_DATE and COMMIT_DATE.
 *
 * Revision 1.77  1996/02/06  19:05:15  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.76  1996/02/01  04:03:43  rich
 * Generalized updateVersion and added recursion.
 *
 * Revision 1.75  1996/01/30  15:05:06  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.74  1996/01/27  21:54:36  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.73  1996/01/23  00:06:44  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.72  1996/01/12  00:53:18  rich
 * Simplified GNUmakefiles.  Fixed some dbmalloc problems.
 *
 * Revision 1.71  1996/01/10  03:16:35  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.70  1996/01/08  18:16:56  rich
 * Changes for vxWorks 5.2.
 *
 * Revision 1.69  1996/01/05  16:31:47  rich
 * Added windows NT port.
 *
 * Revision 1.68  1995/12/17  20:22:25  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.67  1995/12/15  01:23:26  rich
 * Moved Makefile to Makefile.generic to encourage people to use
 * GNUmakefile.
 * Fixed a memory leak when a module is closed and some other small fixes.
 * 
 * Revision 1.63.2.7  1995/12/19  21:27:43  reids
 * Ooops.  Serves me right for cutting and pasting late at night!
 *
 * Revision 1.63.2.6  1995/12/19  04:26:47  reids
 * For safety's sake, copy strings when registering messages, handlers and
 *   when connecting.
 *
 * Revision 1.66  1995/11/13  21:23:10  rich
 * Fixed x_ipcWaitForExecutionWithConstraints.
 *
 * Revision 1.65  1995/11/03  03:04:51  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.64  1995/10/29  18:27:16  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.63  1995/10/26  20:22:38  reids
 * Fixed x_ipcMessageHandlerRegistered to not free the handler name.
 *
 * Revision 1.62  1995/10/25  22:49:07  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.61  1995/10/18  19:28:21  reids
 * resourceProcessPendingRes now checks for available resources before sending
 *   off pending messages.  Had been a problem under certain race conditions.
 *   Also cleaned up a few of the functions that return values.
 *
 * Revision 1.60  1995/10/17  17:36:54  reids
 * Added a "language" slot to the HND_TYPE data structure, so that the LISP
 *   X_IPC version will know what language to decode the data structure into
 *   (extensible for other languages, as well).
 *
 * Revision 1.59  1995/10/10  00:43:17  rich
 * Added more system messages to ignore.
 *
 * Revision 1.58  1995/10/07  19:07:59  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.57  1995/10/03  22:18:47  rich
 * Fixed error where array size was calculated incorrectly when transfering
 * data for byte arrays.
 *
 * Revision 1.56  1995/09/20  01:25:26  rich
 * Fixed the FireHandler routine to fix a bug where a handler could be
 * missed if the same event was fired from within a handler.
 *
 * Revision 1.55  1995/08/14  21:32:15  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.54  1995/08/08  05:43:36  rich
 * Removed junk from x_ipc.h file.
 *
 * Revision 1.53  1995/08/06  16:44:16  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.52  1995/08/05  23:22:12  rich
 * Added functional interface to devUtils.
 *
 * Revision 1.51  1995/08/05  21:11:57  reids
 * The "sharedBuffers" flag was not always being set.
 *
 * Revision 1.50  1995/08/05  17:16:23  reids
 * Several important bug fixes:
 *   a) Found a memory leak in the tms (when nodes are unasserted)
 *   b) Fixed a problem with direct connections that would cause X_IPC to crash
 *      when a command or goal message was sent from an inform or a query.
 *      As part of that fix, all command and goal messages that are sent from
 *      informs or queries are now added to the root node of the task tree.
 *
 * Revision 1.49  1995/07/30  17:09:48  rich
 * More DOS compatibility. Fixes to devUtils.
 *
 * Revision 1.48  1995/07/25  20:09:11  rich
 * Changes for NeXT machine (3.0).
 *
 * Revision 1.47  1995/07/24  15:57:07  reids
 * Somehow x_ipc.h got all screwed up (confused with x_ipc.lisp)!
 *
 * Revision 1.46  1995/07/24  15:40:41  reids
 * Fixed the "maybe execute" macro definitions
 *
 * Revision 1.45  1995/07/19  14:26:56  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.44  1995/07/12  04:55:37  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.43  1995/07/08  18:24:49  rich
 * Change all /afs/cs to /afs/cs.cmu.edu to get ride of conflict problems.
 *
 * Revision 1.42  1995/07/06  21:17:33  rich
 * Solaris and Linux changes.
 *
 * Revision 1.41  1995/06/14  17:51:04  rich
 * Fixes for Linux 1.2.
 *
 * Revision 1.40  1995/06/14  03:22:41  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.39  1995/06/05  23:59:16  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.38  1995/05/31  19:37:03  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.37  1995/04/21  03:53:37  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.36  1995/04/19  14:29:05  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.35  1995/04/17  16:33:39  rich
 * Adding lisp as a subdirectory so it gets included in the tar file.
 *
 * Revision 1.34  1995/04/09  20:30:15  rich
 * Added /usr/local/include and /usr/local/lib to the paths for compiling
 * for sunOS machines. (Support for new vendor OS).
 * Create a x_ipc directory in /tmp and put the socket in there so others can
 * delete dead sockets.  The /tmp directory has the sticky bit set so you
 * can't delete files even if you have write permission on the directory.
 * Fixes to libc.h to use the new declarations in the gcc header files and
 * avoid problems with dbmalloc.
 *
 * Revision 1.33  1995/04/08  02:06:37  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.32  1995/04/07  05:03:33  rich
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
 * Revision 1.31  1995/04/05  19:11:13  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.30  1995/04/04  19:43:12  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.29  1995/03/30  15:44:19  rich
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
 * Revision 1.28  1995/03/28  01:22:36  rich
 * Moved some logging functions to a seperate file so they can be used by
 * modules to determine if logging is active.
 *
 * Revision 1.27  1995/03/28  01:15:09  rich
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
 * Revision 1.26  1995/03/18  15:11:08  rich
 * Fixed updateVersion script so it can be run from any directory.
 *
 * Revision 1.25  1995/01/25  21:44:13  rich
 * Changing version to 8.0 to start developement.
 *
 * Revision 1.24.2.2  1995/03/17  19:59:05  reids
 * Added support for micro version numbers, automatically incrementing micro
 * version numbers, and getting the version numbers in the GNUmakefile's from
 * x_ipcInternal.h.  Fixed x_ipc.h to be compatible with C++
 *
 * Revision 1.24.2.1  1995/02/26  22:45:28  rich
 * I thought the class data came after the message data in transmission
 * like it does in the message data structure, but it does not.
 * The data transmission order is header->msgData->classData.
 * This has been changed in versin 8.0.
 *
 * Revision 1.24  1995/01/25  00:01:50  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.23  1995/01/18  22:43:17  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.22  1994/10/25  17:10:59  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.21  1994/05/20  23:36:15  rich
 * Fixed release date.  Removed centralSuccess from the delay handler.
 * Added DEPEND_PREFIX for creating dependencies for object files in subdirs.
 *
 * Revision 1.20  1994/05/17  23:18:04  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.19  1994/05/11  01:57:40  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.18  1994/04/28  16:17:41  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.17  1994/04/16  19:43:27  rich
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
 * Revision 1.16  1994/04/15  17:10:04  reids
 * Changes to support vxWorks version of X_IPC 7.5
 *
 * Revision 1.15  1994/04/04  16:01:36  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.14  1994/03/28  02:23:13  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.13  1994/01/31  18:29:07  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.12  1993/12/01  18:04:37  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.11  1993/11/21  20:19:51  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.10  1993/10/20  19:00:56  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.9  1993/08/31  18:51:38  fedor
 * Changed version id for incompatibale chage in version 7.x
 *
 * Revision 1.8  1993/08/31  05:49:15  fedor
 * Update version id
 *
 * Revision 1.7  1993/08/30  21:54:52  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.6  1993/08/27  08:39:00  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.5  1993/08/27  07:17:15  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/08/20  20:25:01  fedor
 * Included correct a,b test case and uped version number for alpha/omega merge
 *
 * Revision 1.3  1993/05/31  23:22:16  fedor
 * Include new ansi-c/c++ x_ipc.h in merge. Update Merge version info.
 *
 * Revision 1.2  1993/05/26  23:19:39  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:50  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:42  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:26:14  fedor
 * Added Logging.
 *
 *
 * 31-Dec-91 Reid Simmons, School of Computer Science, CMU
 * Added slot to MSG_TYPE to enable adding exception handlers to class 
 *  of messages, rather than just a single task tree node.
 *
 * 27-May-91 Christopher Fedor, School of Computer Science, CMU
 * Changed MSG_ASK_TYPE to allow for a different msg info cache scheme.
 *
 * 20-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to software standards.
 *
 * 18-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Removed service define SERVER_NAME and explicitly specified the
 * SERVER_PORT number to avoid calls to service lookup routines. 
 * The number needed to be explicit for VxWorks and this simplified
 * initiating a connection. 
 *
 * Lookup routines for a service should be added for an external release.
 *
 * 10-Oct-89 Christopher Fedor, School of Computer Science, CMU
 * 4.2 fix added info to timing display try gerry rostons fix to socket opts.
 *
 * 19-Sep-89 Reid Simmons, School of Computer Science, CMU
 * Deleted "RootNode" field from X_IPC_MOD_INFO, and made one
 * global root node (x_ipcTaskTreeRootGlobal).
 *
 * 15-Sep-89 Christopher Fedor, School of Computer Science, CMU
 * First release in a series to convert over to softwware standards
 * and begin housecleaning.
 *
 *  1-May-89 Reid Simmons, School of Computer Science, CMU
 * Added "RootNode" field to TC_ModInfo (for goal trees)
 *
 * 27-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Copied Reids new tc3.h (find out what changed!)
 * Added Resource field to TC_HndPtr
 *
 * 24-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Added WAITFOREVER
 *
 *    Feb-89 Reid Simmons, School of Computer Science, CMU
 * New formatters added.
 *
 *    Dec-89 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.9 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCx_ipcInternal
#define INCx_ipcInternal

#ifndef NMP_IPC
/* Integer Version Values for Handshaking */
#define X_IPC_VERSION_MAJOR      8
#define X_IPC_VERSION_MINOR      5
#define X_IPC_VERSION_MICRO  8
#define X_IPC_VERSION_DATE "Apr-24-96"
#define X_IPC_COMMIT_DATE "$Date: 2009/01/12 15:54:57 $"
#else
#ifdef DOS_FILE_NAMES
#include "formatte.h"
#else
#include "formatters.h"
#endif
#include "ipc.h"
#include "ipcPriv.h"
#define X_IPC_VERSION_MAJOR      IPC_VERSION_MAJOR
#define X_IPC_VERSION_MINOR      IPC_VERSION_MINOR
#define X_IPC_VERSION_MICRO      IPC_VERSION_MICRO
#define X_IPC_VERSION_DATE       IPC_VERSION_DATE
#define X_IPC_COMMIT_DATE        IPC_COMMIT_DATE
#endif

/* 1621 is debug. Correct reserved number is 1381 */
#define SERVER_PORT 1381
#define SERVER_NAME "task_control"
#define SERVER_MOD_NAME "X_IPC Server"

#define MSG_TABLE_SIZE      (200)
#define MSG_HASH_TABLE_SIZE (203)
#define DISPATCH_TABLE_SIZE (100)

/* 
 * For direct port connections start at LISTEN_PORT
 * and if unavailable on current host increment by 1 until
 * LISTEN_PORT_MAX and report error.
 */
#define LISTEN_PORT 8000
#define LISTEN_PORT_MAX 9001

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* 13-Apr-94: Mike Blackwell: VxWorks defines a macro called "max"... */
#ifdef VXWORKS
#ifdef max
#undef max
#endif
#endif

/* the new sunos 4.1 for sun3 leaves out tolower/toupper in ctypes.h 
   this is still needed for the 4.0.3 systems */

#ifndef VXWORKS

#ifndef toupper
#define toupper(c) ((c)-'a'+'A')
#endif
#ifndef tolower
#define tolower(c) ((c)-'A'+'a')
#endif

#endif /* VXWORKS */


/* 
 * define system constants
 */

#define NO_FD -1
#define NO_FDS {{0,0,0,0,0,0,0,0}}

/* value of x_ipcServerGlobal when there is no connection. */
#define NO_SERVER_GLOBAL  -1
/* value of x_ipcServerGlobal when the current program is the server. */
#define CENTRAL_SERVER_ID  -2

/* Value of the fd when it is a loop back. */
#define LOOP_BACK -3

#define QUERY_REPLY_INTENT -4

#define NO_CLIENT_DATA ((void *)-1)

typedef struct {
  int num;
  const char *str;
} INT_STR_KEY_TYPE, *INT_STR_KEY_PTR;

typedef struct {
  int32 x_ipcMajor;
  int32 x_ipcMinor;
} X_IPC_VERSION_TYPE, *X_IPC_VERSION_PTR;

typedef struct {
  const char *modName;
  const char *hostName;
} MOD_DATA_TYPE, *MOD_DATA_PTR;

typedef struct {
  int readSd;
  int writeSd;
  int32 wait;
  int32 port;
  int32 alive;
  int32 repliesPending;
  MOD_DATA_PTR modData;
  struct _RESOURCE *impliedResource;
  struct _LIST *hndList, *resourceList;
  STR_LIST_PTR providesList;
  STR_LIST_PTR requiresList;

} MODULE_TYPE, *MODULE_PTR;

typedef struct {
  const char *msgName;
  const char *hndName;
} MSG_ASK_TYPE, *MSG_ASK_PTR;

typedef struct {
  int32 refId;
  const char *name;
  X_IPC_MSG_CLASS_TYPE msg_class;
  const struct _FORMAT_TYPE *msgFormat, *resFormat;
} MSG_DATA_TYPE, *MSG_DATA_PTR;

typedef struct _MSG {
  int direct;
  BOOLEAN parsedFormats;
  MSG_DATA_PTR msgData;
  struct _LIST *hndList; /* A list of HND_PTR or DISPATCH_HND_PTR */
  struct _LIST *tapList;
  const char *msgFormatStr, *resFormatStr;
  struct _LIST *excepList;
  /* All the direct connections for this message.
     Either 0 or 1 for query and inform; multiple entries for broadcasts */
  struct _DIRECT_MSG_TYPE *directList;
#ifdef NMP_IPC
  int32 priority;
  int32 limit; /* Queue limit. Used for modules & direct connection messages */
  int notifyHandlerChange;
#endif
} MSG_TYPE, *MSG_PTR;

#ifdef NMP_IPC
#define DEFAULT_PRIORITY -1

typedef struct
{ const char *msgName;
  int32 priority;
} SET_MSG_PRIORITY_TYPE, *SET_MSG_PRIORITY_PTR;
#endif

typedef struct {
  int32 refId;
  const char *msgName, *hndName;
} HND_DATA_TYPE, *HND_DATA_PTR;

/* NOTE: HND_TYPE fields must match DISPATCH_HND_TYPE. */
typedef struct _HND {
  int sd;
  int32 localId;
  MSG_PTR msg;
  X_IPC_HND_FN hndProc;
  MODULE_PTR hndOrg;
  HND_DATA_PTR hndData;
  struct _LIST *msgList;
  struct _RESOURCE *resource;
  HND_LANGUAGE_ENUM hndLanguage;
#ifdef NMP_IPC
  void *clientData;
  BOOLEAN autoUnmarshall;
  BOOLEAN isRegistered;
#endif
} HND_TYPE, *HND_PTR;

typedef struct _FD_HND {
  int fd;
  X_IPC_FD_HND_FN hndProc;
  void *clientData;
} FD_HND_TYPE, *FD_HND_PTR;

typedef INT_STR_KEY_TYPE HND_KEY_TYPE, *HND_KEY_PTR;

typedef struct _X_IPC_REF {
  int32 refId;
  MSG_PTR msg;
  const char *name;
  int responseSd; /* Who to respond to */
#ifdef NMP_IPC
  BOOLEAN responded;
  unsigned int dataLength;
#endif
} X_IPC_REF_TYPE;

typedef struct {
  const char *name;
  const char *format;
} REG_DATA_TYPE, *REG_DATA_PTR;

typedef struct {
  const char *name;
  X_IPC_MSG_CLASS_TYPE msg_class;
  const char *msgFormat;
  const char *resFormat;
} MSG_REG_DATA_TYPE, *MSG_REG_DATA_PTR;

typedef struct _BLOCK_COM_TYPE {
  int32 waitFlag;
  int32 tplConstr;
} BLOCK_COM_TYPE, *BLOCK_COM_PTR;

typedef struct {
  int32 port;
  const char *name;
} DIRECT_TYPE, *DIRECT_PTR;

typedef struct {
  int readSd;
  int writeSd;
  int32 port;
  int32 intent;
  const char *host;
  const char *module;
} DIRECT_INFO_TYPE, *DIRECT_INFO_PTR;

#define DIRECT_INFO_FORMAT "{int, int, int, int, string, string}"

typedef struct {
  int32 intent;
  int readSd;
  int writeSd;
  int32 port;
  const char *host;
  const char *module;
} DIRECT_MSG_HANDLER_TYPE, *DIRECT_MSG_HANDLER_PTR;

#define DIRECT_MSG_HANDLER_FORMAT_NAME "directMsgHandler"
#define DIRECT_MSG_HANDLER_FORMAT      "{int, int, int, int, string, string}"

typedef struct _DIRECT_MSG_TYPE {
  int numHandlers;
  DIRECT_MSG_HANDLER_PTR handlers;
} DIRECT_MSG_TYPE, *DIRECT_MSG_PTR;

#define MAX_DIRECT_HANDLERS (100)
#define MAX_MESSAGE_NAME_LENGTH (100)

#define DIRECT_MSG_FORMAT "{int, <directMsgHandler: 1>}"

typedef struct {
  int readSd;
  int writeSd;
  int32 port;
  const char *host;
  const char *module;
} CONNECTION_TYPE, *CONNECTION_PTR;

#define CONNECTION_FORMAT "{int, int, int, string, string}"

typedef struct {
  BOOLEAN sel;
  fd_set *readMask;
  X_IPC_REF_PTR ref;
  char *replyData;
  X_IPC_RETURN_VALUE_TYPE status;
} WAIT_FOR_REPLY_TYPE, *WAIT_FOR_REPLY_PTR;

typedef struct { int32 num; /* Number of handlers working on the multi-query */
		 int32 max; /* Maximum number of responses desired */
	       } MULTI_QUERY_CLASS_TYPE, *MULTI_QUERY_CLASS_PTR;

#define MULTI_QUERY_NUM(dispatch) \
(((MULTI_QUERY_CLASS_PTR)(dispatch)->blockCom)->num)

#define MULTI_QUERY_MAX(dispatch) \
(((MULTI_QUERY_CLASS_PTR)(dispatch)->blockCom)->max)

#define ONE_WAY_MSG(msg) \
((msg)->msgData->msg_class == InformClass || \
 (msg)->msgData->msg_class == BroadcastClass)

#define RESPONSE_EXPECTED(msg) (!ONE_WAY_MSG(msg))

#define TWO_WAY_MSG(msg) \
((msg)->msgData->msg_class == QueryClass || \
 (msg)->msgData->msg_class == MultiQueryClass)

#ifndef NMP_IPC
#define DIRECT_MSG(msg) \
((msg)->msgData->msg_class == InformClass || \
 (msg)->msgData->msg_class == QueryClass)
#else
#define DIRECT_MSG(msg) \
((msg)->msgData->msg_class == InformClass || \
 (msg)->msgData->msg_class == BroadcastClass || \
 (msg)->msgData->msg_class == QueryClass)
#endif /* NMP_IPC */

#define DIRECT_CONNECTION(directInfo) \
     ((directInfo)->writeSd != GET_C_GLOBAL(serverWrite))

#define NO_REF -1
#define CREATE_NULL_REF() x_ipcRefCreate((MSG_PTR)NULL, \
				       (const char *)NULL, NO_REF)

#ifdef LISP
#define FLUSH_IF_NEEDED(stream) \
{ LOCK_M_MUTEX; if (IS_LISP_MODULE()) fflush(stream); UNLOCK_M_MUTEX; }
#else /* !LISP */
#define FLUSH_IF_NEEDED(stream) 
#endif /* !LISP */

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifdef VXWORKS
#define CENTRAL_VXWORKS_NAME      "central"
#define CENTRAL_VXWORKS_PRIORITY  (100)
#define CENTRAL_VXWORKS_OPTIONS   VX_FP_TASK
#define CENTRAL_VXWORKS_STACKSIZE (50000)

extern int centralPriority;
extern int centralStacksize;
extern void centralMain (char *options);

#endif /* VXWORKS */

#endif /* INCx_ipcInternal */

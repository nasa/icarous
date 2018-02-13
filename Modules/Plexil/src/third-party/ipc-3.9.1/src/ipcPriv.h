/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: ipcPriv.h
 *
 * ABSTRACT: Internal header file for IPC
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: ipcPriv.h,v $
 * Revision 2.42  2011/08/16 16:01:56  reids
 * Adding Python interface to IPC, plus some minor bug fixes
 *
 * Revision 2.41  2011/04/21 18:17:49  reids
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
 * Revision 2.40  2010/12/17 19:20:23  reids
 * Split IO mutex into separate read and write mutexes, to help minimize
 *   probability of deadlock when reading/writing very big messages.
 * Fixed a bug in multi-threaded version where a timeout is not reported
 *   correctly (which could cause IPC_listenClear into a very long loop).
 *
 * Revision 2.39  2009/11/09 17:51:53  reids
 * Fixed invocation of timers -- previously, there were occasions where the
 *   "wait time" provided by IPC_listen was not being honored when there were
 *   timers being fired within the call.
 *
 * Revision 2.38  2009/09/04 19:16:41  reids
 * Ooops -- wrong micro version
 *
 * Revision 2.37  2009/09/04 19:14:14  reids
 * Port to ARM; Remove memory leak in comModule.c.
 * Put IPC Java in separate package.
 *
 * Revision 2.36  2009/05/04 19:03:16  reids
 * Fixed bug in dealing with longs and doubles for 64 bit machines.
 * Changed to using snprintf to avoid corrupting the stack on overflow.
 *
 * Revision 2.35  2009/02/07 18:56:12  reids
 * Updates to java package for use on 64 bit machines;
 * Fixed several compiler warnings.
 *
 * Revision 2.34  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.33  2008/07/16 00:09:03  reids
 * Updates for newer (pickier) compiler gcc 4.x
 *
 * Revision 2.32  2005/12/30 17:01:44  reids
 * Support for Mac OSX
 *
 * Revision 2.31  2004/06/09 18:24:17  reids
 * Fixed bug related to starting multiple centrals in the threaded version.
 *
 * Revision 2.30  2004/04/12 14:40:12  reids
 * Updates to xdrgen to work with newer version of bison.
 *
 * Revision 2.29  2003/07/23 20:25:04  reids
 * Fixed bug in handling message that is received but already unsubscribed.
 * Removed compiler warning.
 *
 * Revision 2.28  2003/04/14 15:32:00  reids
 * Fixed bug in use of IPC_delayResponse
 * Fixed bug in Cygwin version (wrong Endian)
 * Updated for Windows XP
 *
 * Revision 2.27  2003/03/11 23:39:00  trey
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
 * Revision 2.26  2003/02/13 20:40:38  reids
 * Updated to work under Solaris;
 * Store postscrip file gzip'd;
 * Updated for ACL6.0;
 * Fixed compiler warnings;
 * Fixed bug in IPC_listenClear that could hang if central goes down;
 * Fixed bug relating to initializing context data structure.
 *
 * Revision 2.25  2002/06/25 16:46:21  reids
 * Compiled IPC using "insure" ---
 * Removed memory leaks when handler is deregistered and
 *   when global var that already exists is re-registered
 * Fixed the way memory is freed when responses are handled;
 *   Allowed me to remove "responseIssuedGlobal".
 * Added casts to satisfy compiler.
 *
 * Revision 2.24  2002/04/30 10:09:21  reids
 * New release needs new version
 *
 * Revision 2.23  2002/01/11 22:35:11  reids
 * Improved error handling for Java version
 *
 * Revision 2.22  2002/01/03 20:53:02  reids
 * Updated version date.
 *
 * Revision 2.21  2002/01/03 20:52:12  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.20  2001/08/13 13:39:02  reids
 * Added support for access control (using /etc/hosts.allow, /etc/hosts.deny)
 *
 * Revision 2.19  2001/07/24 12:13:44  reids
 * Changes to support compiling under RedHat 7.1 (kernel 2.4)
 *
 * Revision 2.18  2001/05/30 19:37:59  reids
 * Fixed a bug in the way timers interacted with query/reply messages.
 *   In particular, timers can no longer run recursively.
 *
 * Revision 2.17  2001/03/06 00:20:17  trey
 * added IPC_addTimerGetRef() and IPC_removeTimerByRef() functions
 *
 * Revision 2.16  2001/03/01 22:22:43  reids
 * Don't warn about replacing the timer if it is already deleted!
 *
 * Revision 2.15  2001/02/09 16:24:21  reids
 * Added IPC_getConnections to return list of ports that IPC is listening to.
 * Added IPC_freeDataElements to free the substructure (pointers) of a struct.
 *
 * Revision 2.14  2001/01/31 17:54:12  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.13  2001/01/25 17:37:25  reids
 * Changed LISTEN_CLEAR_WAIT from 100 msecs down to 5 msecs.
 *
 * Revision 2.12  2001/01/12 15:55:34  reids
 * Added IPC_delayResponse to enable responding to a query outside of the
 *   message handler.
 * Improved handling of queries, especially when sender does not expect a reply
 * Made logging more efficient for ignored messages
 *
 * Revision 2.11  2001/01/10 15:32:50  reids
 * Added the function IPC_subscribeData that automatically unmarshalls
 *   the data before invoking the handler.
 *
 * Revision 2.10  2001/01/05 22:43:58  reids
 * Minor changes to enable compiling under Visual C++, Cygwin, & Solaris.
 *
 * Revision 2.9  2000/12/11 16:13:40  reids
 * Extended for compilation under CYGWIN and RedHat 6;
 * Added parsing rule to allow compatibility with RTC fixed array formats;
 * Fixed bug in triggering timers.
 *
 * Revision 2.8  2000/08/14 21:28:34  reids
 * Added support for making under Windows.
 *
 * Revision 2.7  2000/08/07 21:17:37  reids
 * Fixed for compiling under IRIX using CC.
 *
 * Revision 2.6  2000/07/27 16:59:11  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.5  2000/07/19 20:57:18  reids
 * Handling broken pipes better;
 * Added IPC_listenWait;
 * Took out several extraneous messages.
 *
 * Revision 2.4  2000/07/03 17:16:40  hersh
 * Updated internal version number to 3.0.0, since the new version won't
 * interoperate with the old version.
 *
 * Revision 2.3  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/01/27 20:51:48  reids
 * Changes for RedHat 6 (and also to remove compiler warnings).
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.9.2.21  1997/03/07 17:49:44  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.9.2.20  1997/02/26 04:18:39  reids
 * Added IPC_isMsgDefined.
 * Once again refixed the logic behind data logging.
 *
 * Revision 1.9.2.19  1997/01/27 20:41:03  reids
 * Implement a function to check whether a given format string matches the
 *   one registered for a given message.
 * On VxWorks, spawn a task that spawns central with the "right" task
 *   parameters, including "centralPriority" and "centralStacksize".
 * For Lisp, force all format enum values to uppercase; Yields more efficient
 *   C <=> Lisp conversion of enumerated types.
 *
 * Revision 1.9.2.18  1997/01/27 20:09:36  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.9.2.16  1997/01/21 17:21:54  reids
 * Re-re-fixed the logic of "x_ipc_LogIgnoreP" (needed an extra
 *   function: LogIgnoreAllP).
 * Fixed stringEqualNoCase for VxWorks version.
 *
 * Revision 1.9.2.15  1997/01/17 21:18:20  reids
 * Fixed the Lispworks version if a handler is aborted (by a throw or error) --
 *   correctly restore internal state and notify central that the handler ended
 *
 * Revision 1.9.2.14  1997/01/16 22:18:55  reids
 * Added "memory" option, put "display" option back in, and made "-s" (silent)
 *   option work.
 * Improved the way usage stats are reported.
 * Check that IPC initialized before using global var.
 * Took out restriction that Lisp publishing and marshalling was non-reentrant
 *   (needed for multi-tasking in Lisp).
 * Improved error checking for Lisp functions.
 *
 * Revision 1.9.2.13  1997/01/11 01:21:04  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.9.2.12.4.4  1997/01/08 18:41:23  reids
 * Added function "killModule" to cleanly shut down vxworks tasks running IPC.
 *
 * Revision 1.9.2.12.4.3  1996/12/28 01:06:32  reids
 * Fixed a bug in x_ipc_alignField having to do with the way ALIGN_WORD handles
 *   fixed length arrays
 *
 * Revision 1.9.2.12.4.2  1996/12/27 19:49:34  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.9.2.12.4.1  1996/12/24 14:41:41  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.9.2.12  1996/12/18 15:11:53  reids
 * Changed logging code to remove VxWorks dependence on varargs
 * Fixed bug in Lispworks/VxWorks handling of messages, where a message
 *   could be "stranded" if it arrived during a blocking query
 * Fixed bug to enable multiple connects and disconnects (by not calling
 *   taskVarAdd more than once per task)
 * Defined common macros to clean up code
 * Shut down modules more gracefully
 * Print name of module within IPC_perror
 * Re-fixed the logic of "x_ipc_LogIgnoreP"
 * Don't add resource to resourceList if just changing its capacity
 *
 * Revision 1.9.2.11  1996/11/22 19:03:43  rouquett
 * get rid of the annoying warnings with c++
 *
 * Revision 1.9.2.10  1996/11/05 15:30:05  reids
 * Added the missing "index" function to Solaris version.
 *
 * Revision 1.9.2.9  1996/10/29 14:56:39  reids
 * Added IPC_unmarshallData.
 * IPC_freeByteArray function available to C, not just LISP.
 * Make "byteOrder" and "alignment" vars available to C, not just LISP.
 * Cache formatter attributes when receiving new message info.
 * Remove some compiler warnings.
 *
 * Revision 1.9.2.8  1996/10/24 15:19:20  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.9.2.7  1996/10/22 18:49:42  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.9.2.6  1996/10/18 18:07:14  reids
 * Main changes: Merge with CMU X_IPC code; Add short, byte, etc for Lisp.
 * Cache formatter attributes; Better error handling and reporting.
 * Better error checking and reporting (IPC_errno, IPC_perror).
 * Added IPC_initialize so that you don't have to connect with central to
 *   do marshalling.
 * Added IPC_setVerbosity to set the level at which messages are reported.
 * Fixed freeing of formatters.
 * Added the "-i" and "-I" command line options, to ignore logging specific
 *   messages.
 *
 * Revision 1.9.2.5  1996/10/16 15:20:26  reids
 * Main changes: Merge with CMU X_IPC code; Add short, byte, etc for Lisp;
 *   Cache formatter attributes; Better error handling and reporting.
 *
 * Revision 1.9.2.4  1996/10/14 03:54:43  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.9.2.3  1996/10/08 14:23:38  reids
 * Changes for IPC operating under Lispworks on the PPC.  Mainly changes
 * (LISPWORKS_FFI_HACK) due to the fact that Lispworks on the PPC is currently
 * missing the foreign-callable function.
 *
 * Revision 1.9.2.2  1996/10/07 20:15:08  reids
 * Added ability to specify, at the command line, messages for central to
 *   ignore logging.
 *
 * Revision 1.9.2.1  1996/10/02 20:58:38  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.9  1996/05/26 04:11:48  reids
 * Added function IPC_dataLength -- length of byte array assd with msgInstance
 *
 * Revision 1.8  1996/05/24 16:45:54  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.7  1996/05/09 01:06:23  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.6  1996/04/03 02:45:49  reids
 * Made ipc.h C++ compatible.
 * Made IPC_listenClear wait a bit before returning, to catch pending msgs.
 * Made IPC_connect try a few times to connect with the central server.
 *
 * Revision 1.5  1996/04/01 02:36:08  reids
 * Needed to change x_ipcAddFn to x_ipcAddEventHandler.
 *
 * Revision 1.4  1996/03/12 03:23:56  reids
 * New release needs new micro version number.
 *
 * Revision 1.3  1996/03/06 20:20:45  reids
 * Version 2.3 adds two new functions: IPC_defineFormat and IPC_isConnected
 *
 * Revision 1.2  1996/03/05 04:29:10  reids
 * Added error checking to IPC_connect.
 *
 * Revision 1.1  1996/03/03 04:36:20  reids
 * First release of IPC files.  Corresponds to IPC Specifiction 2.2, except
 * that IPC_readData is not yet implemented.  Also contains "cover" functions
 * for the xipc interface.
 *
 ****************************************************************/

#ifndef _IPC_PRIV_H
#define _IPC_PRIV_H

/*****************************************************************
 *                DEFINES -- CONSTANTS AND MACROS
 *****************************************************************/

/* Interal version control */
#define IPC_VERSION_MAJOR  3
#define IPC_VERSION_MINOR  9
#define IPC_VERSION_MICRO  1
#define IPC_VERSION_DATE "Aug-16-11"
#define IPC_COMMIT_DATE "$Date: 2011/08/16 16:01:56 $"

#define MAX_RECONNECT_TRIES (5)
#define RECONNECT_WAIT      (1) /* seconds */

// Was 100 msecs, which caused IPC_listenClear to block indefinitely if
// messages are received faster than 10Hz.  This could cause same thing to
// happen if messages are received at > 200Hz, but IPC is not really optimized
// to handle that volume of messages, anyways.
#define LISTEN_CLEAR_WAIT (5) /* milliseconds to wait while clearning msgs */

#define X_IPC_CONNECTED() (x_ipc_isValidServerConnection())

#define X_IPC_INITIALIZED() (mGlobalp() && GET_C_GLOBAL(valid))

#define RETURN_ERROR(error) \
  { ipcSetError(error); return IPC_Error; }

#define PASS_ON_ERROR() return IPC_Error

/*****************************************************************
 *                INTERNAL TYPE DECLARATIONS
 *****************************************************************/

typedef struct { BOOLEAN handled;
		 void *data;
		 FORMATTER_PTR formatter;
	       } QUERY_REPLY_TYPE, *QUERY_REPLY_PTR;


typedef struct { 
  CONNECT_HANDLE_TYPE handler; 
  void *clientData;
} CONNECT_DATA_TYPE, *CONNECT_DATA_PTR;

typedef struct { 
  const char *msgName;
  CHANGE_HANDLE_TYPE handler; 
  void *clientData;
} CHANGE_DATA_TYPE, *CHANGE_DATA_PTR;

/*****************************************************************
 *                INTERNAL GLOBAL VARIABLES
 *****************************************************************/

extern IPC_VERBOSITY_TYPE ipcVerbosity;

/*****************************************************************
 *                INTERNAL FUNCTION DEFINITIONS
 *****************************************************************/

#ifdef macintosh
#pragma export on
#endif
extern IPC_RETURN_TYPE _IPC_initialize (BOOLEAN isLispModule);

/* Modified by TNgo, 5/22/97 */
extern IPC_RETURN_TYPE _IPC_connect (const char *taskName,
				     const char *serverName,
				     BOOLEAN willListen,
				     BOOLEAN isLispModule);

extern IPC_RETURN_TYPE _IPC_subscribe (const char *msgName, const char *hndName,
				       HANDLER_TYPE handler, void *clientData,
				       int autoUnmarshall);

extern IPC_RETURN_TYPE _IPC_unsubscribe (const char *msgName,
					 const char *hndName);
#ifdef macintosh
#pragma export off
#endif

extern IPC_RETURN_TYPE ipcReturnValue(X_IPC_RETURN_VALUE_TYPE retVal);

extern IPC_RETURN_TYPE ipcDataToSend (CONST_FORMAT_PTR format, 
 				      const char *msgName,
 				      unsigned int length, BYTE_ARRAY content,
 				      void **dataHandle,
				      IPC_VARCONTENT_PTR varcontent);

extern void ipcHandlerName (const char *msgName, HANDLER_TYPE handler, 
			    char *hndName, uint hndNameSize);

#ifdef macintosh
#pragma export on
#endif
extern void ipcSetError (IPC_ERROR_TYPE error);

extern IPC_RETURN_TYPE _IPC_queryResponse (const char *msgName, 
					   unsigned int length,
					   BYTE_ARRAY content,
					   BYTE_ARRAY *replyHandle,
					   FORMATTER_PTR *replyFormatter,
					   unsigned int timeoutMsecs);
#ifdef macintosh
#pragma export off
#endif

unsigned long ipcNextTime (void);

void ipcTriggerTimers (void);

// Return number of times timer can still be triggered
// Needed by some of the FFI's
unsigned int maxTriggers (TIMER_REF timerRef);

/*****************************************************************
 *                     WINSOCK FUNCTIONS
 *****************************************************************/

#ifdef _WINSOCK_
void startWinsock (void);
#endif /* _WINSOCK_ */

#ifdef WIN32
typedef unsigned int uint;
#define snprintf sprintf_s
#endif

#endif /* _IPC_PRIV_H */

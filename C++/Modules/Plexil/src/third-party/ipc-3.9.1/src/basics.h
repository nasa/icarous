/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: 
 *
 * FILE: 
 *
 * ABSTRACT:
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/basics.h,v $ 
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: basics.h,v $
 * Revision 2.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2000/08/14 21:28:32  reids
 * Added support for making under Windows.
 *
 * Revision 2.2  2000/07/03 17:03:21  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.2  1996/12/18 15:08:26  reids
 * Defined common macros to clean up code
 *
 * Revision 1.1.2.1  1996/10/08 14:23:29  reids
 * Changes for IPC operating under Lispworks on the PPC.  Mainly changes
 * (LISPWORKS_FFI_HACK) due to the fact that Lispworks on the PPC is currently
 * missing the foreign-callable function.
 *
 * Revision 1.1  1996/05/09 01:01:09  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:30:51  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.19  1996/01/05  16:30:59  rich
 * Added windows NT port.
 *
 * Revision 1.18  1995/07/25  20:08:45  rich
 * Changes for NeXT machine (3.0).
 *
 * Revision 1.17  1995/07/06  22:08:06  rich
 * Missing directive.
 *
 * Revision 1.16  1995/07/06  21:39:26  rich
 * Fixes ported from 7.9.
 *
 * Revision 1.15  1995/07/06  21:15:40  rich
 * Solaris and Linux changes.
 *
 * Revision 1.14  1995/05/31  19:34:53  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.13  1995/04/21  03:53:00  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.12  1995/04/19  14:27:36  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.11  1995/04/08  02:06:10  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.10  1995/04/07  05:02:39  rich
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
 * Revision 1.9  1995/03/30  15:42:21  rich
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
 * Revision 1.8  1995/03/28  01:14:09  rich
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
 * Revision 1.7  1995/01/18  22:39:30  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.6  1994/10/25  17:09:30  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.5  1994/05/31  03:23:41  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.4  1994/04/28  16:15:10  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.3  1993/11/21  20:17:02  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.2  1993/08/30  21:52:56  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.1  1993/08/27  07:13:59  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/07/08  05:38:00  rich
 * Added function prototypes
 *
 * Revision 1.3  1993/06/22  13:58:26  rich
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
 * Revision 1.2  1993/05/27  22:16:01  rich
 * Added automatic logging.
 *
 *****************************************************************************/

#ifndef INCbasics
#define INCbasics

#define NMP_IPC

/* This is needed because the current version of Lispworks for VxWorks
   is not able to call Lisp function from C */
#if defined(LISPWORKS) && defined(VXWORKS)
#define LISPWORKS_FFI_HACK
#endif

/* GCC supports inlining of functions.*/
#if defined(__GNUC__) && !defined(__NeXT__)
#define INLINE __inline__
#else
#define INLINE
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#if defined(THINK_C) || defined(__TURBOC__) || defined(__svr4__)
#define bzero(buf, len) memset((void *)(buf), 0, (size_t)(len))

#define bcopy(from, to, len) memcpy((void *)(to), (void *)(from), (size_t)len)

#endif /* THINK_C || __TURBOC__ || __svr4__ */

#define BCOPY(source, dest, length) \
  memcpy((void *)(dest), (void *)(source), (size_t)(length))

#ifndef _WINSOCK_  /* windows already has a BOOLEAN type */
typedef int BOOLEAN;
#endif /* !_WINSOCK_ */

/* A generic pointer (prints out better than "data_ptr" does in the debugger */
typedef struct {
  char *dummy;
} *value_ptr;

#ifndef MAX
#define MAX(x,y)  ((x) > (y) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x,y)  ((x) > (y) ? (y) : (x))
#endif

#ifndef NEW
#define NEW(type) (type *)x_ipcMalloc((unsigned int) sizeof(type))
#endif

#ifndef NEW_DB
#define NEW_DB(file,line, type) (type *)x_ipcDBMalloc(file,line, (unsigned int) sizeof(type))
#endif

#ifndef STREQ
#define STREQ(str1, str2) (!(strcmp((str1),(str2))))
#endif

/* the new sunos 4.1 for sun3 leaves out tolower/toupper in ctypes.h 
   this is still needed for the 4.0.3 systems */

#ifndef ABS
#define ABS(x)	  ((x) >= 0 ? (x) : -(x))
#endif


#ifndef PAUSE_MIN_DELAY
#if defined(__sgi)
#define PAUSE_MIN_DELAY() sginap(1);	/* delay 1/100 sec */
#elif defined(__TURBOC__) || defined(__svr4__)
#define PAUSE_MIN_DELAY() sleep(1);	/* delay 1 sec */
#elif defined(OS2)
#define PAUSE_MIN_DELAY() DosSleep(1);	/* delay 0.001 sec - really 0.030 sec */
#elif  defined(WIN32)
#define PAUSE_MIN_DELAY() Sleep(1);	/* delay 0.001 sec - Note Cap "S" */
#else
#define PAUSE_MIN_DELAY() usleep(1);	/* delay 1/1000000 sec */
#endif /* SGI */
#endif

#ifdef _WINSOCK_
#define CLOSE_SOCKET closesocket
#else
#define CLOSE_SOCKET close
#endif

#define SHUTDOWN_SOCKET(sd) { shutdown(sd, 2); CLOSE_SOCKET(sd); }

#endif /* INCbasics */

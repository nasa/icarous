/*
 * x_ipcModError.c
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons. All rights reserved.
 * 
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * $Log: tcModError.c,v $
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:17  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.5  1997/01/27 20:09:58  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.3  1997/01/16 22:19:53  reids
 * Check that IPC initialized before using global var.
 *
 * Revision 1.2.2.2  1997/01/11 01:21:23  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.2.1.6.1  1996/12/24 14:41:48  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.2.2.1  1996/10/18 18:17:53  reids
 * Check the verbosity level at which messages are reported.
 *
 * Revision 1.2  1996/05/09 16:44:24  reids
 * Minor cleanups
 *
 * Revision 1.1  1996/05/09 01:02:04  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/04/03 03:11:10  reids
 * The data logging (-ld) option now works, for both IPC and regular X_IPC msgs.
 * Fixed the interaction between signals and listening for messages.
 * IPC version will not exit if connection to central server is not available.
 * IPC version will not print out stats if module exits (x_ipcModError).
 *
 * Revision 1.3  1996/03/12 03:20:03  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.2  1996/03/05 04:10:52  reids
 * Fixed a bug in the LISP version where it would go into an infinite
 *   segv loop if trying to connect without a central (lispExit was not set)
 *
 * Revision 1.1  1996/03/03 04:32:45  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.14  1996/01/05  16:31:39  rich
 * Added windows NT port.
 *
 * Revision 1.13  1995/06/14  03:22:33  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.12  1995/04/09  20:30:13  rich
 * Added /usr/local/include and /usr/local/lib to the paths for compiling
 * for sunOS machines. (Support for new vendor OS).
 * Create a x_ipc directory in /tmp and put the socket in there so others can
 * delete dead sockets.  The /tmp directory has the sticky bit set so you
 * can't delete files even if you have write permission on the directory.
 * Fixes to libc.h to use the new declarations in the gcc header files and
 * avoid problems with dbmalloc.
 *
 * Revision 1.11  1995/01/18  22:43:11  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.10  1994/11/02  21:34:42  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.9  1994/10/25  17:10:57  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.8  1994/04/28  16:17:37  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.7  1994/04/26  16:23:40  rich
 * Now you can register an exit handler before anything else and it will
 * get called if connecting to central fails.
 * Also added code to handle pipe breaks during writes.
 *
 * Revision 1.6  1994/04/16  19:43:22  rich
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
 * Revision 1.5  1993/12/14  17:35:27  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.4  1993/11/21  20:19:47  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:17:12  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/06/13  23:28:33  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:39  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:38  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:26:09  fedor
 * Added Logging.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 25-Oct-90: fedor: created to avoid logging in modules.
 *****************************************************************/

#include "globalM.h"
#ifdef NMP_IPC
#include "ipcPriv.h"
#endif

void x_ipcModWarning(const char *description, ...)
{
#ifdef NMP_IPC
  if (ipcVerbosity >= IPC_Print_Warnings)
#endif
  if (description) {
    va_list args;
    va_start(args, description);
    vfprintf(stderr, (char *)description, args);
    va_end(args);
    FLUSH_IF_NEEDED(stderr);
  }
}  

void x_ipcModError(const char *description, ...)
{
#ifndef NMP_IPC
  x_ipcStats(stderr);
#endif

#ifdef NMP_IPC
  if (ipcVerbosity >= IPC_Print_Errors)
#endif
  if (description) {
    va_list args;
    va_start(args, description);
    vfprintf(stderr, (char *)description, args);
    fprintf(stderr, "\n");
    va_end(args);
    FLUSH_IF_NEEDED(stderr);
  }
  
#ifdef NMP_IPC
  if (ipcVerbosity >= IPC_Exit_On_Errors)
#endif
  {
    x_ipcClose();
  
    LOCK_M_MUTEX;
    if (mGlobalp()) {
#ifdef LISP
      if (IS_LISP_MODULE()) {
	if (GET_M_GLOBAL(lispExitGlobal) != NULL)
	  (*(GET_M_GLOBAL(lispExitGlobal)))();
      } else
#endif /* LISP */
	{
	  if (GET_M_GLOBAL(x_ipcExitHnd)) {
	    (*(GET_M_GLOBAL(x_ipcExitHnd)))();
	  } else {
	    exit(-1);
	  }
	}
      }
    UNLOCK_M_MUTEX;
  }
}

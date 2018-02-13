/*
 * tcerror.c
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * $Log: tcerror.c,v $
 * Revision 2.4  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:30  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.5  1997/01/27 20:10:08  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.3  1997/01/16 22:19:23  reids
 * Made "-s" (silent) option work.
 *
 * Revision 1.1.2.2  1996/12/18 15:13:13  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.1  1996/10/18 18:18:48  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 *
 * Revision 1.1  1996/05/09 01:02:10  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:58  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.11  1996/06/25  20:51:49  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.10  1996/01/05  16:31:50  rich
 * Added windows NT port.
 *
 * Revision 1.9  1995/04/07  05:03:38  rich
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
 * Revision 1.8  1995/01/18  22:43:32  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/11/02  21:34:48  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.6  1994/10/25  17:11:08  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.5  1994/04/16  19:43:35  rich
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
 * Revision 1.4  1993/08/30  21:55:03  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.3  1993/08/27  07:17:31  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/06/13  23:28:35  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:40  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:52  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:26:26  fedor
 * Added Logging.
 *
 * $Revision: 2.4 $
 * $Date: 2009/05/04 19:03:41 $
 * $Author: reids $
 *
 2-Aug-89 Reid Simmons      Interfaced to the logging facility.
 19-Jul-89 Christopher Fedor created to replace the exit call.
 */

#include "globalS.h"
#include "logging.h"

void x_ipcError(const char *description, ...)
{
  if (!GET_S_GLOBAL(terminalLog).quiet)
    x_ipcStats(stderr);
  
  if (description) {
    va_list args;
    char msg[150];
    
    bzero(msg, sizeof(msg));
    va_start(args, description);
    vsnprintf(msg, sizeof(msg), (char *)description, args);
    va_end(args);
    if (x_ipc_LogMessagesP()) {
      LOG1("%s\n", msg);
    } else {
      fprintf(stderr,"%s\n", msg);
    }
  }
  
  End_File_Logging();
#if 0
  (void)printf("Dump core [y]?\n");
  if (getchar() != 'n')
    abort();
#endif
  serverShutdown();
  exit(1);
}

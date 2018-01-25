/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: memory
 *
 * FILE: x_ipcMem.c
 *
 * ABSTRACT:
 * 
 * Interface to memory management routines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: tcaMem.c,v $
 * Revision 2.6  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.5  2002/06/25 16:46:08  reids
 * "Insure" complained that malloc cannot be paired with cfree...
 *
 * Revision 2.4  2002/01/03 20:52:18  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.3  2000/08/14 21:28:35  reids
 * Added support for making under Windows.
 *
 * Revision 2.2  2000/07/03 17:03:30  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.7  1997/01/27 20:10:04  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.5  1997/01/16 22:17:29  reids
 * Added "totalMemBytes" to improve the way usage stats are reported.
 *
 * Revision 1.1.2.4  1996/12/18 15:13:11  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.3  1996/10/29 14:54:09  reids
 * Use the right prototype for x_ipcMallocHnd
 *
 * Revision 1.1.2.2  1996/10/24 15:19:29  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.1.2.1  1996/10/18 18:18:31  reids
 * Fixed a few compiler warnings.
 *
 * Revision 1.1  1996/05/09 01:02:08  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:52  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.19  1996/02/10  16:50:46  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.18  1995/07/06  21:17:38  rich
 * Solaris and Linux changes.
 *
 * Revision 1.17  1995/06/15  14:58:44  rich
 * Fixed definition of the malloc routine.
 *
 * Revision 1.16  1995/06/14  03:22:48  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.15  1995/06/05  23:59:20  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.14  1995/05/31  19:37:08  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.13  1995/04/08  02:06:39  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.12  1995/01/18  22:43:25  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.11  1994/10/25  17:11:02  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.10  1994/05/31  03:24:03  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.9  1994/05/17  23:18:12  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.8  1994/04/28  16:17:45  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.7  1994/04/16  19:43:29  rich
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
 * Revision 1.6  1993/12/14  17:35:32  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.5  1993/11/21  20:19:53  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/30  21:54:57  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.3  1993/08/27  07:17:23  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:19:46  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:39  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:32:47  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:26:21  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 7-oct-92 Richard Goodwin : Changed free() to cfree().  Try to make it 
 * easier to track down memory problems.  Also added an 
 *
 *  4-Jul-91 Christopher Fedor, School of Computer Science, CMU
 * Added x_ipcRegisterMallocHnd to complete the routines so that modules
 * can handle all memory allocation.
 *
 *  4-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Added x_ipcRegisterFreeMemHnd so that modules can be called to free
 * memory to satisfy a malloc request.
 *
 * 25-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.6 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

#if !defined(DBMALLOC)
#define SIZETYPE int
void *x_ipcDBMalloc(const char* file, int line, unsigned long amount);
void x_ipcDBFree(const char *file, int line, char *item);
#else
#undef x_ipcMalloc
void *x_ipcMalloc(unsigned long amount);
#endif


/******************************************************************************
 *
 * FUNCTION: void x_ipcRegisterFreeMemHnd(void *(*func)(u_int32), int retry)
 *
 * DESCRIPTION: 
 * Sets a function for freeing memory when malloc returns NULL.
 * The function is passed an unsigned int amount of the memory requested.
 * The number of times this routine is called is set by retry.
 * The default is 1.
 *
 * INPUTS: 
 * VOID_FN func;
 * int retry;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcRegisterFreeMemHnd(void (*func)(u_int32), int retry)
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(freeMemRetryAmount) = retry;
  
  if (GET_M_GLOBAL(freeMemRetryAmount) < 1)
    GET_M_GLOBAL(freeMemRetryAmount) = 1;
  
  GET_M_GLOBAL(x_ipcFreeMemoryHnd) = func;
  UNLOCK_M_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcRegisterMallocHnd(func, retry)
 *
 * DESCRIPTION: 
 * Registers a function to call in place of malloc.
 * The routine will be passed an unsigned int of the amount of storage needed.
 * The routine will be called a max of retry times if NULL is returned.
 * The default retry amount is 1.
 *
 * INPUTS: 
 * void *(*func)();
 * int32 retry;
 *
 * OUTPUTS:
 *
 *****************************************************************************/

#if defined(DBMALLOC)
void x_ipcRegisterMallocHnd(void *(*func)(const char*, int, SIZETYPE),
			  int retry)
#elif defined(__sgi)
void x_ipcRegisterMallocHnd(void *(*func)(unsigned int), int retry)
#elif defined(__TURBOC__)
void x_ipcRegisterMallocHnd(void *(*func)(unsigned long), long retry)
#else
void x_ipcRegisterMallocHnd(void *(*func)(size_t size), int retry)
#endif
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(mallocMemRetryAmount) = retry;
  
  if (GET_M_GLOBAL(mallocMemRetryAmount) < 1)
    GET_M_GLOBAL(mallocMemRetryAmount) = 1;
  
  if (func)
    GET_M_GLOBAL(x_ipcMallocMemHnd) = func;
  UNLOCK_M_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcFree(item)
 *
 * DESCRIPTION: 
 * An interface to free - should use x_ipcFreeData or x_ipcFreeReply in most cases.
 *
 * INPUTS: char *item;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

#if defined(DBMALLOC)
#undef x_ipcFree
void x_ipcFree(void *item)
{
  x_ipcDBFree("Unknown", 9999, item);
}
void x_ipcDBFree(const char* file, int line, char *item)
#else
void x_ipcDBFree(const char* file, int line, char *item)
{
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  x_ipcFree(item);
}
void x_ipcFree(void *item)
#endif
{
  if (item == NULL) return;

#if defined(THINK_C) || defined(macintosh) || defined(__sgi)
  free(item);
#elif defined(DBMALLOC)
  debug_free(file,line,item);
#elif defined(__TURBOC__)
  farfree(item);
#else 
  free(item);
#endif /* THINK_C || macintosh */
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipcMalloc(amount)
 *
 * DESCRIPTION: Interface to malloc requests from x_ipc.
 *
 * INPUTS: u_int32 amount;
 *
 * OUTPUTS: char * - generic pointr to the memory
 *
 * NOTES: 
 * Stops everything if we have run out of memory. 
 * Note there may not be enough memory to print that we have run out.
 *
 *****************************************************************************/

#if defined(DBMALLOC)
void *x_ipcMalloc(unsigned long amount)
{
  return x_ipcDBMalloc("Unknown", 9999, amount);
}
void *x_ipcDBMalloc(const char* file, int line, SIZETYPE amount)
#elif defined(__TURBOC__)
void *x_ipcDBMalloc(const char* file, int line, unsigned long amount)
{
  return x_ipcMalloc(amount);
}
void *x_ipcMalloc(unsigned long amount)
#else
void *x_ipcDBMalloc(const char* file, int line, unsigned long amount)
{
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  return x_ipcMalloc(amount);
}
void *x_ipcMalloc(size_t amount)
#endif
{
  int i, j;
  void *mem;
  
  mem = NULL;
  
  if (!mGlobalp() || !GET_M_GLOBAL(x_ipcMallocMemHnd)) {
    /* X_IPC_MOD_WARNING1("calling x_ipcMalloc(%d) before initialized\n", amount);*/
#if defined(DBMALLOC)
    mem = malloc(file, line, amount);
#else
    mem = malloc(amount);
#endif
    if (!mem) 
      X_IPC_MOD_ERROR1("x_ipcMalloc: NULL returned from malloc for request: %d\n", 
		       amount);
    return mem;

  } else {
    LOCK_M_MUTEX;

    if (GET_M_GLOBAL(x_ipcMallocMemHnd)) 
      for(i=0;!mem && i < GET_M_GLOBAL(mallocMemRetryAmount);i++) {
#if defined(DBMALLOC)
	mem = (*GET_M_GLOBAL(x_ipcMallocMemHnd))(file,line,amount);
#else
	mem = (*GET_M_GLOBAL(x_ipcMallocMemHnd))(amount);
#endif
      }

    if (mem) {
      GET_M_GLOBAL(totalMemRequest)++;
      GET_M_GLOBAL(totalMemBytes) += amount;
      UNLOCK_M_MUTEX;
      return(mem);
    }
  
    if (GET_M_GLOBAL(x_ipcFreeMemoryHnd))
      for(j=0;!mem && j < GET_M_GLOBAL(freeMemRetryAmount);j++) {
	(*GET_M_GLOBAL(x_ipcFreeMemoryHnd))(amount);
      
	if (GET_M_GLOBAL(x_ipcMallocMemHnd)) 
	  for(i=0;!mem && i < GET_M_GLOBAL(mallocMemRetryAmount);i++) {
#if defined(DBMALLOC)
	    mem = (*GET_M_GLOBAL(x_ipcMallocMemHnd))(file,line,amount);
#else
	    mem = (*GET_M_GLOBAL(x_ipcMallocMemHnd))(amount);
#endif
	  }
      }
    
    if (mem) {
      GET_M_GLOBAL(totalMemRequest)++;
      GET_M_GLOBAL(totalMemBytes) += amount;
      UNLOCK_M_MUTEX;
      return(mem);
    }
  
    UNLOCK_M_MUTEX;
    X_IPC_MOD_ERROR1("x_ipcMalloc: NULL returned from malloc for request: %d\n",
		     amount);
    return NULL;
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipcStats(stream)
 *
 * DESCRIPTION: Quick hack to display some memory stats.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcStats(FILE *stream)
{
  if (!mGlobalp())
    return;
  
  LOCK_M_MUTEX;
  fprintf(stream,"Cumulative Memory Usage:\n  Requests: %ld (%ld bytes)\n", 
	  GET_M_GLOBAL(totalMemRequest), GET_M_GLOBAL(totalMemBytes));
  UNLOCK_M_MUTEX;
  x_ipc_dataMsgDisplayStats(stream);
  FLUSH_IF_NEEDED(stream);
}

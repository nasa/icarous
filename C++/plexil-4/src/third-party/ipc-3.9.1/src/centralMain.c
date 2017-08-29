/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: central
 *
 * FILE: central.c
 *
 * ABSTRACT:
 *
 * This file contains the "main" of the central x_ipc server.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: centralMain.c,v $
 * Revision 2.7  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2003/02/13 20:41:10  reids
 * Fixed compiler warnings.
 *
 * Revision 2.5  2000/08/14 21:28:32  reids
 * Added support for making under Windows.
 *
 * Revision 2.4  2000/07/19 20:55:49  reids
 * Added handling for SIGPIPE; fixed handling of direct messages when pipe
 *   is broken.
 * Also, need to reset handlers (maybe??) since, according to the man page for
 *  "signal", the Linux version resets the handler to its default value each
 *  time the signal is raised.
 *
 * Revision 2.3  2000/07/03 17:03:21  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/02/25 14:07:25  reids
 * Use of UNUSED_PRAGMA for compilers that do not support "#pragma unused"
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.4.7  1997/03/07 17:49:26  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.1.4.6  1997/01/27 20:38:48  reids
 * On VxWorks, spawn a task that spawns central with the "right" task
 *   parameters, including "centralPriority" and "centralStacksize".
 *
 * Revision 1.1.4.5  1997/01/27 20:09:09  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.4.3  1997/01/11 01:20:45  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.4.2.4.1  1996/12/24 14:35:17  reids
 * Don't need (use) "centralFreeMemory" on VxWorks
 *
 * Revision 1.1.4.2  1996/12/18 15:12:42  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.4.1  1996/10/02 20:57:39  reids
 * Fixed the method for parsing command-line options in VxWorks
 *
 * Revision 1.1  1996/06/29 00:19:34  kamyar
 * used to be called central.c
 *
 * Revision 1.1  1996/05/09 01:01:12  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/04/24 19:10:57  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.1  1996/03/03 04:30:58  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.33  1996/02/11  04:11:58  rich
 * Fixed warning message.
 *
 * Revision 1.32  1996/02/10  16:49:32  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.31  1996/02/06  19:04:09  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.30  1996/01/30  15:03:50  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.29  1996/01/05  16:31:00  rich
 * Added windows NT port.
 *
 * Revision 1.28  1995/12/17  20:21:08  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.27  1995/12/15  01:23:02  rich
 * Moved Makefile to Makefile.generic to encourage people to use
 * GNUmakefile.
 * Fixed a memory leak when a module is closed and some other small fixes.
 *
 * Revision 1.26  1995/07/12  04:54:17  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.25  1995/07/10  16:16:48  rich
 * Interm save.
 *
 * Revision 1.24  1995/07/08  18:24:39  rich
 * Change all /afs/cs to /afs/cs.cmu.edu to get ride of conflict problems.
 *
 * Revision 1.23  1995/07/06  21:15:42  rich
 * Solaris and Linux changes.
 *
 * Revision 1.22  1995/04/21  03:53:07  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.21  1995/04/07  05:02:44  rich
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
 * Revision 1.20  1995/03/28  01:14:16  rich
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
 * Revision 1.19  1995/03/14  22:36:37  rich
 * Fixed problem with multiple read needed when doing a vector read. (Fix
 * to version 7.9)
 * Also fixed the data size problem from 7.9.
 *
 * Revision 1.18  1995/01/25  00:00:54  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.17  1995/01/18  22:39:46  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.16  1994/05/25  04:56:55  rich
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
 * Revision 1.15  1994/05/06  04:46:58  rich
 * Put central io routines in a new file.
 * Fixed GNUmakefile.
 *
 * Revision 1.14  1994/05/05  00:46:00  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.13  1994/04/28  22:16:40  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 * Revision 1.12  1994/04/28  16:15:22  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.11  1994/04/26  16:23:08  rich
 * Now you can register an exit handler before anything else and it will
 * get called if connecting to central fails.
 * Also added code to handle pipe breaks during writes.
 *
 * Revision 1.10  1994/04/16  19:41:42  rich
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
 * Revision 1.9  1994/03/28  02:22:33  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.8  1994/01/31  18:27:30  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.7  1993/12/14  17:32:57  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.6  1993/12/01  18:02:49  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.5  1993/11/21  20:17:18  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/30  21:53:10  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.3  1993/08/27  07:14:16  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:16:57  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:16  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:16  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:16  fedor
 * Added Logging.
 *
 * 25-Jul-91 Reid Simmons at School of Computer Science, CMU
 * Removed the -b option (buffers are no longer managed).
 * Added -r option to try resending messages after module crashes.
 *
 *  8-Jul-91 Reid Simmons at School of Computer Science, CMU
 * Removed the -gc option.
 * Added -b <n> option to pre-allocate a data message buffer.
 *
 *  7-Apr-90 Christopher Fedor at School of Computer Science, CMU
 * Added -v option to simply display current server version info.
 *
 * 30-Mar-90 Christopher Fedor at School of Computer Science, CMU
 * Revised to software standards.
 *
 * $Revision: 2.7 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"


/*****************************************************************************
 *
 * FUNCTION: int parseExpectedMods(argc, argv, expectedMods)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * int argc;
 * char **argv;
 * int *expectedMods;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

#ifndef VXWORKS
static void parseExpectedMods(int argc, char **argv, int *expectedMods)
{ 
  int i;
  
  /* Default to 1 module.  */
  *expectedMods = 1;

  for (i=1; i<argc; i++) {
    if (isdigit((int)argv[i][0])) {
      *expectedMods = atoi(argv[i]);
      return;
    }
  }
}
#endif

/*****************************************************************************
 *
 * FUNCTION: void abortCentral(s, c, sp)
 *
 * DESCRIPTION:
 * Trap fatal errors and attempt a core dump.
 *
 * INPUTS:
 * int s, c;
 * struct sigcontext *sp;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
/*
   sun os 4.0.3 added a parameter
   void abortCentral(s, c, sp, addr)
   int s, c;
   struct sigcontext *sp;
   char *addr;
   */

#if !defined(THINK_C) && !defined(macintosh) && !defined(__TURBOC__) && !defined(OS2) && !defined(_WIN95_MSC_) && !defined(WINNT)
#if (defined(sun4) || defined(sparc)) && !defined(__svr4__)
static void abortCentral(int s, int c, struct sigcontext *sp)
#else
static void abortCentral(int s)
#endif
{
  /*  x_ipc_idTablePrintInfo(GET_S_GLOBAL(dispatchTable));*/
  X_IPC_ERROR1("Central Abort : Signal %d",s);
}

/*****************************************************************************
 *
 * FUNCTION: void pipeClosedHnd(s, c, sp)
 *
 * DESCRIPTION:
 * Trap fatal errors and attempt a core dump.
 *
 * INPUTS:
 * int s, c;
 * struct sigcontext *sp;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
/*
   sun os 4.0.3 added a parameter
   void pipeClosedHnd(s, c, sp, addr)
   int s, c;
   struct sigcontext *sp;
   char *addr;
   */

#if (defined(sun4) || defined(sparc))  && !defined(__svr4__)
static void pipeClosedHnd(int s, int c, struct sigcontext *sp)
#else
     static void pipeClosedHnd(int s)
#endif
{
#ifdef UNUSED_PRAGMA
#pragma unused(s)
#endif
  GET_M_GLOBAL(pipeBroken) = TRUE;
  X_IPC_MOD_WARNING("Pipe Broke \n");
#ifdef linux
  /* According to the man page for "signal", the Linux version resets the handler
     to its default value each time the signal is raised. */
  signal(SIGPIPE, pipeClosedHnd);
#endif
}
#endif

/******************************************************************************
 *
 * FUNCTION: char *centralFreeMalloc(amount)
 *
 * DESCRIPTION: Emergency memory free.
 *
 * INPUTS: unsigned int amount;
 *
 * OUTPUTS: void
 *
 * NOTES: 
 *
 *****************************************************************************/
#ifndef VXWORKS
static void centralFreeMemory(u_int32 amount)
{
#ifdef UNUSED_PRAGMA
#pragma unused(amount)
#endif
  /* Try to free up amount of memory.  Really want to free more. */
  /* Try by deleting stuff from the pending queues. */
  /* The problem is that this can consume memory. */
  X_IPC_MOD_WARNING("Warning MEMORY EXHAUSTED, purging queues !!!!\n");
  
  purgeResoucePending();
  
  return;
}
#endif

/*****************************************************************************
 *
 * FUNCTION: main(argc, argv)
 *
 * DESCRIPTION:
 * start.
 *
 * INPUTS:
 * int argc;
 * char **argv;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
 
#ifdef macintosh
#include <console.h>
#include <Events.h>
#include <SIOUX.h>

static void doSiouxStuff (void)
{
  SIOUXSettings.autocloseonquit = TRUE;
  SIOUXSettings.showstatusline = TRUE;
  SIOUXSettings.asktosaveonclose = FALSE;
}
#endif

#ifdef VXWORKS
/* Used for cleaning stopping central */
static int centralTID = 0;

/* To be called from the shell */
void killCentral (void)
{
  if (centralTID != 0)
    kill(centralTID, SIGTERM);
}

int centralPriority = CENTRAL_VXWORKS_PRIORITY;
int centralStacksize = CENTRAL_VXWORKS_STACKSIZE;

void central (char *options)
{
  taskSpawn(CENTRAL_VXWORKS_NAME, centralPriority, 
	    CENTRAL_VXWORKS_OPTIONS, centralStacksize,
	    (FUNCPTR)centralMain,
	    (int)options, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void centralMain(char *options)
#else
int main(int argc, char **argv)
#endif
{
  int expectedMods = 0;
  
#ifdef macintosh
  doSiouxStuff();
  argc = ccommand(&argv);
#endif
 
  /* Added by Bob Goode/Tam Ngo, 5/21/97, for WINSOCK option. */
#ifdef OS2
  sock_init();
#elif defined(_WINSOCK_)
  startWinsock();
#endif /* Winsock DLL loading */

  x_ipcModuleInitialize();
#ifdef VXWORKS
  /* Do this only after the socket is set up (in case there is an
     old central lying around that needs killed */
  centralTID = taskIdSelf();
#endif
  globalSInit();
  
#if !defined(THINK_C) && !defined(macintosh) && !defined(__TURBOC__) && !defined(OS2) && !defined(_WIN95_MSC_) && !defined(WINNT) && !defined(WIN32)
  (void)signal(SIGINT, abortCentral);
  (void)signal(SIGBUS, abortCentral);
  (void)signal(SIGSEGV, abortCentral);
  (void)signal(SIGPIPE, pipeClosedHnd);
  (void)signal(SIGTERM, abortCentral);
#endif /* !THINK_C && !macintosh */
  
#ifndef VXWORKS
  if ((argc > 1) && (STREQ(argv[1], "-v")))
    displayVersion();
  else if ((argc > 1) && (STREQ(argv[1], "-h"))) {
    displayOptions(argv[0]);
#ifdef macintosh
  SIOUXSettings.autocloseonquit = FALSE;
#endif
  } else {
    parseExpectedMods(argc, argv, &expectedMods);
    parseCommandLineOptions(argc, argv);
#else
  if ((options!= NULL) && (strstr(options, "-v") || strstr(options, "-V"))) {
    displayVersion();
  } else if ((options!= NULL) && 
	     (strstr(options, "-h") || strstr(options, "-H"))) {
    displayOptions("central");
  } else {
    parseOpsFromStr(options, &expectedMods, FALSE);
#endif
      
    if (expectedMods < 1)
      expectedMods = 1;
      
    if (!serverInitialize(expectedMods)) {
      X_IPC_ERROR("ERROR: Unable to start server, Is one already running?\n");
    }
      
#ifndef VXWORKS
    /* Register a method for freeing memory in an emergency. */
    x_ipcRegisterFreeMemHnd(centralFreeMemory,3);
      
    if (GET_S_GLOBAL(listenToStdin))
      printPrompt();
#endif

    listenLoop();
  }
#ifdef _WINSOCK_
  WSACleanup();
  printf("Socket cleaned up.");
#endif /* Unload Winsock DLL */
#ifndef VXWORKS
  return 1;
#endif
}

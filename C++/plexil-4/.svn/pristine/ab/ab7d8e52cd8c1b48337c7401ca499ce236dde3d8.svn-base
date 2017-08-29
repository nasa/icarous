/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: central
 *
 * FILE: centralIO.c
 *
 * ABSTRACT:
 *
 * This file contains i/o routines for central.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: centralIO.c,v $
 * Revision 2.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2003/02/13 20:41:09  reids
 * Fixed compiler warnings.
 *
 * Revision 2.2  2000/07/03 17:03:21  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.6.2.7  1997/01/27 20:38:19  reids
 * Update the "help" displays for command line and terminal IO options.
 *
 * Revision 1.6.2.6  1997/01/27 20:09:07  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.6.2.4  1997/01/16 22:15:20  reids
 * Added "memory" option, put "display" option back in, and made "-s" (silent)
 * option work.
 *
 * Revision 1.6.2.3  1996/12/18 15:12:41  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.6.2.2  1996/10/22 18:10:33  reids
 * Moved MAX_MESSAGE_NAME_LENGTH to x_ipcInternal.h
 *
 * Revision 1.6.2.1  1996/10/07 20:15:11  reids
 * Added ability to specify, at the command line, messages for central to
 *   ignore logging.
 *
 * Revision 1.6  1996/08/30 22:37:14  rouquett
 * Added a field in LOG_PTR to turn off adding comments to the log file
 *
 * Revision 1.5  1996/08/23 04:09:03  rouquett
 * *** empty log message ***
 *
 * Revision 1.4  1996/08/23 03:59:23  rouquett
 * *** empty log message ***
 *
 * Revision 1.3  1996/08/23 03:58:35  rouquett
 * *** empty log message ***
 *
 * Revision 1.2  1996/05/24 16:45:48  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:13  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/04/24 19:10:58  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.1  1996/03/03 04:30:59  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.18  1996/02/07  00:27:34  rich
 * Add prefix to VERSION_DATE and COMMIT_DATE.
 *
 * Revision 1.17  1996/01/30  15:03:52  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.16  1996/01/23  00:06:20  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.15  1996/01/10  03:16:10  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.14  1995/12/17  20:21:10  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.13  1995/10/25  22:47:55  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.12  1995/04/21  03:53:08  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.11  1995/04/07  05:02:46  rich
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
 * Revision 1.10  1995/04/04  19:41:47  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.9  1995/03/30  15:42:26  rich
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
 * Revision 1.8  1995/03/28  01:14:18  rich
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
 * Revision 1.7  1995/03/18  15:10:58  rich
 * Fixed updateVersion script so it can be run from any directory.
 *
 * Revision 1.6  1995/01/18  22:39:49  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.5  1994/10/27  14:43:06  reids
 * Change the default handling of wiretap messages.  Now the default is to
 * place them as children of the tapped message; the "-t" option changes the
 * default to place them all under the task tree root node.
 *
 * Revision 1.4  1994/05/25  04:56:58  rich
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
 * Revision 1.3  1994/05/17  23:15:14  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.2  1994/05/11  01:57:07  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.1  1994/05/06  04:47:01  rich
 * Put central io routines in a new file.
 * Fixed GNUmakefile.
 *
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"
#ifdef NMP_IPC
#include "ipc.h"
#endif

/*****************************************************************************
 *
 * FUNCTION: void setOptions(option, logPtr)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * char *option;
 * LOG_PTR logPtr;
 *
 * OUTPUTS: void
 *
 * HISTORY:
 *
 *  5-Jan-93 Domingo Gallardo at School of Computer Science, CMU
 *  New option: log parent's ID
 *
 *  2-Dec-92 Richard Goodwin at School of Computer Science, CMU
 *  Cleaned it up a little.
 *
 *  6-Oct-89 Christopher Fedor at School of Computer Science, CMU
 *  eeeks -- this setting options is a mess! 
 *
 *****************************************************************************/

BOOLEAN checkOccurrence(char *option, char *item)
{
#ifndef VXWORKS
  return (strpbrk(option, item) != NULL);
#else
  /* use index - vxworks is missing strpbrk */
  
  while(item && item[0] != '\0') {
    if (index(option, item[0])) {
      return TRUE;
    }
    item++;
  }
  
  return FALSE;
#endif
}

void setOptions(char *option, LOG_PTR logPtr)
{
  if ((strlen(option) == 1) || (strlen(option) == 2)) { 
    /* -l or -L -- use all the options, except i */
    logPtr->messages = logPtr->status = logPtr->time = TRUE;
    logPtr->data = logPtr->summary = TRUE;
    logPtr->ignore = logPtr->refId = FALSE;
  }
  else if (checkOccurrence(option, "xX")) {
    /* no options */
    logPtr->messages = logPtr->status = logPtr->time = logPtr->data = FALSE;
    logPtr->data = logPtr->summary = logPtr->ignore = FALSE;
  }
  else {
    logPtr->messages = checkOccurrence(option, "mM");
    logPtr->status = checkOccurrence(option, "sS");
    logPtr->time = checkOccurrence(option, "tT");
    logPtr->data = checkOccurrence(option, "dD");
    logPtr->ignore = checkOccurrence(option, "iI");
    logPtr->summary = checkOccurrence(option, "hH");
    logPtr->refId = checkOccurrence(option, "rR");
    logPtr->parentId = checkOccurrence(option, "pP");
    
    if (checkOccurrence(option, "fF")) /* leave default if not specified. */
      logPtr->flush = FALSE;

    logPtr->addComments = TRUE;
    if (checkOccurrence(option, "nN")) /* leave default if not specified. */
      logPtr->addComments = FALSE;
  }
}

static INLINE void printTorF(int item)
{
  if (item) {
    printf("TRUE\n");
  } else {
    printf("FALSE\n");
  }
}

void printOps(LOG_PTR logPtr)
{
  printf("messages: ");
  printTorF(logPtr->messages);
  
  printf("status: ");
  printTorF(logPtr->status);
  
  printf("time: ");
  printTorF(logPtr->time);
  
  printf("data: ");
  printTorF(logPtr->data);
  
  printf("ignore: ");
  printTorF(logPtr->ignore);
  
  printf("summary: ");
  printTorF(logPtr->summary);
  
  printf("refId: ");
  printTorF(logPtr->refId);
  
  printf("parentId: ");
  printTorF(logPtr->parentId);
  
  printf("flush: ");
  printTorF(logPtr->flush);
}

void printPrompt(void)
{
  if (!GET_S_GLOBAL(terminalLog).quiet) {
    printf("> ");
    fflush(stdout);
  }
}


static void processMessageIgnoreFile (const char *fileName)
{
  FILE *file;
  char messageToIgnore[MAX_MESSAGE_NAME_LENGTH];

  file = fopen(fileName, "r");
  if (!file) {
    X_IPC_MOD_WARNING1("Could not open message ignore file %s\n", fileName);
  } else {
    for (; fgets(messageToIgnore, MAX_MESSAGE_NAME_LENGTH, file) != NULL; ) {
      if (messageToIgnore[strlen(messageToIgnore)-1] == '\n') {
	messageToIgnore[strlen(messageToIgnore)-1] = '\0';
      }
      X_IPC_MOD_WARNING1("Ignore logging of %s\n", messageToIgnore);
      Add_Message_To_Ignore(messageToIgnore);
    }
  }
}


/*****************************************************************************
 *
 * FUNCTION: void parseCommandLineOptions(argc, argv)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * int argc;
 * char **argv;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void parseOption(char *option, BOOLEAN started)
{
  switch(option[1]) {
  case 'l':
    /*    printf("Terminal Logging Status\n");*/
    setOptions(option, &GET_S_GLOBAL(terminalLog));
    if (started) {
      centralSetVar(X_IPC_TERMINAL_LOG_VAR, (char *)&GET_S_GLOBAL(terminalLog));
    }
    /*    printOps(GET_S_GLOBAL(terminalLog));*/
    break;
  case 'L': 
    /*    printf("File Logging Status\n");*/
    setOptions(option, &GET_S_GLOBAL(fileLog));
    if (started) {
      centralSetVar(X_IPC_FILE_LOG_VAR, (char *)&GET_S_GLOBAL(fileLog));
      /*    printOps(GET_S_GLOBAL(fileLog));*/
    }
    break;
  case 'f':
    if (strlen(option) > 2) {
      (void)strncpy(GET_S_GLOBAL(Log_File_Name), option+2, 
		    MAX_LOG_FILE_NAME_LENGTH);
      printf("Logging File Name: %s\n",GET_S_GLOBAL(Log_File_Name));
      fflush(stdout);
    }
    break;
  case 'p':
    if (strlen(option) > 2) 
      (void)sscanf(option+2, "%d", &(GET_S_GLOBAL(serverPortGlobal)));
    break;
  case 'd':
    GET_S_GLOBAL(x_ipcDebugGlobal) = TRUE;
    break;
  case 'r':
    GET_S_GLOBAL(resendAfterCrashGlobal) = TRUE;
    break;
#ifndef NMP_IPC
  case 'q':
    GET_S_GLOBAL(qassertCheckGlobal) = TRUE;
    break;
  case 't':
    GET_S_GLOBAL(tapsUnderRoot) = TRUE;
    break;
#endif
  case 's':
    GET_S_GLOBAL(terminalLog).quiet = TRUE;
#ifdef NMP_IPC
    IPC_setVerbosity(IPC_Silent);
#endif
    break;
  case 'u':
    GET_S_GLOBAL(listenToStdin) = FALSE;
    break;
  case 'c':
    GET_S_GLOBAL(directDefault) = TRUE;
    break;
  case 'i':
    Add_Message_To_Ignore(&option[2]);
    break;
  case 'I':
    processMessageIgnoreFile(&option[2]);
    break;
  default: 
    X_IPC_MOD_WARNING1("Ignoring unknown option '%s'\n", option);
  }
}

void parseCommandLineOptions(int argc, char **argv)
{ 
  int i;
  
  for (i=1; i<argc; i++)
    if (argv[i][0] == '-') {
      parseOption(argv[i],FALSE);
    }
}

void parseOpsFromStr(char *str, int *expectedMods, BOOLEAN started)
{
  char *op;
  int i, j, len;
  
  if (!str) {
    return;
  }
  
  len = strlen(str);
  
  i = 0;
  while (i != len) {
    if (str[i] == ' ' || str[i] == '\0') {
      i++;
    } else {
      op = str+i;
      
      for(j=0;op[j] != ' ' && op[j] != '\0';j++) {};
      op[j] = '\0';
      
      if (op[0] == '-') {
	parseOption(op, started);
      } else if (expectedMods && isdigit((int)op[0])) {
	*expectedMods = atoi(op);
      }

      i += j;
    }
  }
}

/*****************************************************************************
 *
 * FUNCTION: void displayOptions(name)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * char *name;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static void displayOptions1 (BOOLEAN commandLineP)
{
  printf(" -v: display server version info.\n");
  printf(" -l: logging onto terminal.\n");
  printf("     options are: m (message traffic)\n");
  printf("                  s (status of IPC)\n");
  printf("                  t (time messages are handled)\n");
  printf("                  d (data associated with message)\n");
  printf("                  i (ignore logging registration and deregistration messages)\n");
  printf("                  h (incoming message handle time summary)\n");
  printf("                  r (log the reference Id as well as the msg name)\n");
  printf("                  p (log the reference Id of the message's parent)\n");
  printf("                  x (no logging)\n");
  printf("     -l is equivalent to -lmstdh; the default is -lmsi\n");
  printf(" -L: logging into file.\n");
  printf("     options are the same as above with the addition of F.\n");
  printf("                  F (don't flush file after each line)\n");
  printf("                  n (do not prompt the user for comments)\n");
  printf("     the default is -Lx\n");
  printf(" -f: filename to use for logging.\n");
  printf("     If not specified, name is automatically generated.\n");
  if (commandLineP)
    printf(" -p<port>: specify the server port - the default port is: %d\n", 
	   SERVER_PORT);
  printf(" -c: Use direct (not via central) conntections where possible.\n");
  printf(" -i<msgName>: Ignore logging this message (can occur multiple times).\n");
  printf(" -I<fileName>: File with names of messages to ignore logging.\n");
#ifndef NMP_IPC
  printf(" -t: put all wiretap messages under the task tree root node\n");
  printf("     (the default is that they are children of the tapped message\n");
  printf(" -q: test quantity lattice insertions for task tree nodes.\n");
  printf(" -d: flag random debugging.\n");
#endif
  printf(" -s: silent running, don't print anything to stdout.\n");
  printf(" -u: Don't run the user interface (stdin).\n");
  printf(" -r: try resending non-completed messages when modules crash\n");
  fflush(stdout);
}

void displayOptions(char *name)
{
  printf("%s command line options:\n", name);
  displayOptions1(TRUE);
}


/*****************************************************************************
 *
 * FUNCTION: void displayHelp()
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * char *name;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void displayHelp(void)
{
  printf("\nThe following command are available:\n");
  printf("help: print this message.\n");
#ifndef NMP_IPC
  printf("kill: Kill the task tree. Removes all pending and complete messages.\n");
#endif
#ifdef NMP_IPC
  printf("display : Display active and pending messages.\n");
#else
  printf("display : Display the task tree and non-task tree messages.\n");
#endif
  printf("status : Display the known modules and their status.\n");
  printf("memory : Display total memory usage.\n");
  printf("close <module>: Close a connection to a module.\n");
  printf("unlock <resource>: Unlock a locked resouce.\n");
  printf("The following command line options can also be used as commands\n");

  displayOptions1(FALSE);
}


/*****************************************************************************
 *
 * FUNCTION: void displayVersion()
 *
 * DESCRIPTION: displays the central server banner.
 *
 * INPUTS: none.
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void displayVersion(void)
{
  X_IPC_MOD_WARNING3( "Task Control Server %d.%d.%d \n",
		X_IPC_VERSION_MAJOR, X_IPC_VERSION_MINOR, X_IPC_VERSION_MICRO);
  X_IPC_MOD_WARNING1( " Released : %s\n", X_IPC_VERSION_DATE);
  X_IPC_MOD_WARNING1( " Commited : %s\n", X_IPC_COMMIT_DATE);
  X_IPC_MOD_WARNING2( " Compiled : %s %s\n", __DATE__, __TIME__);
}

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: central main function.
 *
 * FILE: central.h
 *
 * ABSTRACT:
 * 
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/centralIO.h,v $ 
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: centralIO.h,v $
 * Revision 2.3  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:21  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
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
 * Revision 1.1  1996/03/03 04:31:01  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.5  1996/01/30  15:03:53  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.4  1995/04/21  03:53:10  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.3  1995/03/30  15:42:27  rich
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
 * Revision 1.2  1994/05/17  23:15:15  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.1  1994/05/06  04:47:07  rich
 * Put central io routines in a new file.
 * Fixed GNUmakefile.
 *
 * Revision 1.1  1994/04/28  22:16:44  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 *
 ****************************************************************/

#ifndef INCcentralIO
#define INCcentralIO

BOOLEAN checkOccurrence(char *option, char *item);
void setOptions(char *option, LOG_PTR logPtr);
void displayHelp(void);
void displayOptions(char *name);
void displayVersion(void);
void parseOpsFromStr(char *str, int *expectedMods, BOOLEAN started);
void printPrompt(void);
void printOps(LOG_PTR logPtr);
void parseOption(char *option, BOOLEAN started);
void parseCommandLineOptions(int argc, char **argv);

#endif /* INCcentralIO */

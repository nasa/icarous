/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: comServer.c
 *
 * ABSTRACT:
 * 
 * Communication Routines - Central Server
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/comServer.h,v $ 
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: comServer.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:22  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:18  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/04/24 19:11:03  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.1  1996/03/03 04:31:11  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.18  1995/10/29  18:26:36  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.17  1995/10/07  19:07:11  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.16  1995/07/06  21:15:55  rich
 * Solaris and Linux changes.
 *
 * Revision 1.15  1995/05/31  19:35:15  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.14  1995/04/21  03:53:20  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.13  1995/03/28  01:14:27  rich
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
 * Revision 1.12  1995/03/19  19:39:28  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.11  1995/03/16  18:05:12  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.10  1995/01/18  22:40:03  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.9  1994/05/25  04:57:18  rich
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
 * Revision 1.8  1994/05/17  23:15:30  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.7  1994/04/28  16:15:40  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.6  1993/12/14  17:33:15  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.5  1993/12/01  18:03:06  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:17:28  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/10/21  16:13:44  rich
 * Fixed compiler warnings.
 *
 * Revision 1.2  1993/08/27  08:38:25  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.1  1993/08/27  07:14:27  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  17:38:02  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:16:33  rich
 * Added automatic logging.
 *
 ****************************************************************/

#ifndef INCcomServer
#define INCcomServer

#define centralRegisterInformMessage(name, format) \
centralRegisterMessage(name, InformClass, format, (const char *)NULL)
     
#define centralRegisterQueryMessage(name, msgFormat, resFormat) \
     centralRegisterMessage(name, QueryClass, msgFormat, resFormat)
     
#define centralRegisterCommandMessage(name, format) \
     centralRegisterMessage(name, CommandClass, format, (const char *)NULL)
     
#define centralRegisterExceptionMessage(name, format) \
     centralRegisterMessage(name, ExceptionClass, format, (const char *)NULL)
     
#define centralRegisterGoalMessage(name, format) \
     (centralRegisterMessage(name, GoalClass, format, (const char *)NULL))
     
#define centralRegisterBroadcastMessage(name, format) \
     (centralRegisterMessage(name, BroadcastClass, format, (const char *)NULL))
     
#define centralRegisterGoal(name, format, hndProc) \
{centralRegisterGoalMessage(name, format); \
   centralRegisterHandler(name, name, proc)}

#define centralRegisterCommand(name, format, hndProc) \
{centralRegisterCommandMessage(name, format); \
   centralRegisterHandler(name, name, hndProc)}

#define centralRegisterInform(name, format, hndProc) \
{centralRegisterInformMessage(name, format); \
   centralRegisterHandler(name, name, hndProc)}

#define centralRegisterQuery(name, format, reply, hndProc) \
{centralRegisterQueryMessage(name, format, reply); \
   centralRegisterHandler(name, name, hndProc)}

#define centralRegisterException(name, format, hndProc) \
{centralRegisterExceptionMessage(name, format); \
   centralRegisterHandler(name, name, hndProc)}

typedef struct {
  int sd;
  const char *name;
} MODULE_KEY_TYPE, *MODULE_KEY_PTR;

void parseMsgFormats (MSG_PTR msg);

int serverInitialize(int expectedMods);
void serverShutdown(void);
void listenLoop(void);
void moduleFree(MODULE_PTR module);
void pendingReplyAdd(DISPATCH_PTR dispatch);
void pendingReplyRemove(DISPATCH_PTR dispatch);

void parseMsg(MSG_PTR msg);
void centralRegisterMessage(const char *name, X_IPC_MSG_CLASS_TYPE msg_class,
			    const char *msgFormat, const char *resFormat);
void centralRegisterNamedFormatter(const char *formatterName, 
				   const char *formatString);

void _centralRegisterHandler(const char *msgName, 
			     const char *hndName,
			     X_IPC_HND_FN hndProc);

void centralRegisterLengthFormatter(char *formatterName, int32 length);

#define centralRegisterHandler(msgName, hndName, hndProc) \
_centralRegisterHandler(msgName, hndName, (X_IPC_HND_FN) hndProc);

void removeConnection(MODULE_PTR module);
void moduleClean(MODULE_PTR module);

void x_ipcCloseMod(char *name);

#endif /* INCcomServer */

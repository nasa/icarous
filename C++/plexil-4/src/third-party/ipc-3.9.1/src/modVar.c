/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1994.
 * 
 * MODULE: communications
 *
 * FILE: modVar.c
 *
 * ABSTRACT:
 * 
 * Provides a system global variables.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: modVar.c,v $
 * Revision 2.5  2011/04/21 18:17:49  reids
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
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:14  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:26  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.1  1996/10/24 15:19:23  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.1  1996/05/09 01:01:41  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:19:50  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:55  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.15  1996/03/09  06:13:15  rich
 * Fixed problem where lisp could use the wrong byte order if it had to
 * query for a message format.  Also fixed some memory leaks.
 *
 * Revision 1.14  1996/02/10  16:50:10  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.13  1995/12/17  20:21:42  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.12  1995/10/25  22:48:35  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.11  1995/10/07  19:07:27  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.10  1995/08/14  21:31:56  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.9  1995/06/05  23:59:05  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.8  1995/04/08  02:06:26  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.7  1995/04/07  05:03:27  rich
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
 * Revision 1.6  1995/04/04  19:42:31  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.5  1995/03/30  15:43:31  rich
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
 * Revision 1.4  1995/03/28  01:14:46  rich
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
 * Revision 1.3  1995/01/18  22:41:15  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.2  1994/05/25  04:57:48  rich
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
 * Revision 1.1  1994/05/17  23:16:33  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 *
 * $Revision: 2.5 $
 * $Date: 2011/04/21 18:17:49 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"
#include "hash.h"
#include "modVar.h"

/* Global Defines */

/* Private Global types */

typedef struct _var {
  const char *varName;
  const char *format;
  void *value;
} GLOBAL_VAR_TYPE, *GLOBAL_VAR_PTR;



/*****************************************************************************
 *
 * FUNCTION: void x_ipcRegisterVar(const char *varName, const char *format)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcRegisterVar(const char *varName, const char *format)
{
  GLOBAL_VAR_TYPE var;
  
  var.varName = varName;
  var.format = format;
  x_ipcInform(X_IPC_REGISTER_VAR_INFORM, &var);
}


/*****************************************************************************
 *
 * FUNCTION: void _x_ipcSetVar(const char *varName, const void *value)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE _x_ipcSetVar(const char *varName, const void *value)
{
  char *setMsgName;
  X_IPC_RETURN_VALUE_TYPE result;
  
  setMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_SET_PREFIX));
  strcpy(setMsgName,VAR_SET_PREFIX);
  strcat(setMsgName,varName);
  
  /*  x_ipcExecuteCommandAsync(setMsgName, value);*/
  result = x_ipcInform(setMsgName, value);

  x_ipcFree(setMsgName);
  return result;
}


/*****************************************************************************
 *
 * FUNCTION: void _x_ipcGetVar(const char *varName, void *value)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE _x_ipcGetVar(const char *varName, void *value)
{
  char *getMsgName;
  X_IPC_RETURN_VALUE_TYPE result;
  
  getMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_GET_PREFIX));
  strcpy(getMsgName,VAR_GET_PREFIX);
  strcat(getMsgName,varName);
  
  result = x_ipcQuery(getMsgName, NULL, value);

  x_ipcFree(getMsgName);
  return result;
}


/*****************************************************************************
 *
 * FUNCTION: void _x_ipcGetSetVar(const char *varName, void *value)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

X_IPC_RETURN_VALUE_TYPE _x_ipcGetSetVar(const char *varName, 
				    void *setValue, void *getValue)
{
  char *getSetMsgName;
  X_IPC_RETURN_VALUE_TYPE result;
  
  getSetMsgName = (char *)x_ipcMalloc(1+strlen(varName)+
				    strlen(VAR_GET_SET_PREFIX));
  strcpy(getSetMsgName,VAR_GET_SET_PREFIX);
  strcat(getSetMsgName,varName);
  
  result = x_ipcQuery(getSetMsgName, setValue, getValue);
  x_ipcFree(getSetMsgName);
  
  return result;
}


/*****************************************************************************
 *
 * FUNCTION: void _x_ipcWatchVar(const char *varName, const char *format, 
 *                             X_IPC_HND_FN watchFn)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void _x_ipcWatchVar(const char *varName, const char *format, X_IPC_HND_FN watchFn)
{
#ifdef UNUSED_PRAGMA
#pragma unused(format)
#endif
  char *watchMsgName;
  
  watchMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_WATCH_PREFIX));
  strcpy(watchMsgName,VAR_WATCH_PREFIX);
  strcat(watchMsgName,varName);
  
  /* Make sure the variable is registered first. */
  /*  x_ipcRegisterVar(varName, format);*/
  
  /* Register the tap messages and handler */
  x_ipcRegisterHandler(watchMsgName,watchMsgName,watchFn);
  x_ipcFree(watchMsgName);
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipcUnwatchVar(const char *varName)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void x_ipcUnwatchVar(const char *varName)
{
  char *watchMsgName;
  
  watchMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_WATCH_PREFIX));
  strcpy(watchMsgName,VAR_WATCH_PREFIX);
  strcat(watchMsgName,varName);
  
  /* Unregister the tap messages and handler */
  x_ipcDeregisterHandler(watchMsgName,watchMsgName);
  x_ipcFree(watchMsgName);
}

void x_ipcIgnoreVarLogging(const char *varName)
{
  char *setMsgName;
  char *getMsgName;
  char *getSetMsgName;
  char *watchMsgName;
  
  setMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_SET_PREFIX));
  strcpy(setMsgName,VAR_SET_PREFIX);
  strcat(setMsgName,varName);
  x_ipcIgnoreLogging(setMsgName);
  
  getMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_GET_PREFIX));
  strcpy(getMsgName,VAR_GET_PREFIX);
  strcat(getMsgName,varName);
  x_ipcIgnoreLogging(getMsgName);
  
  getSetMsgName = (char *)x_ipcMalloc(1+strlen(varName)+ 
				    strlen(VAR_GET_SET_PREFIX));
  strcpy(getSetMsgName,VAR_GET_SET_PREFIX);
  strcat(getSetMsgName,varName);
  x_ipcIgnoreLogging(getSetMsgName);
  
  watchMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_WATCH_PREFIX));
  strcpy(watchMsgName,VAR_WATCH_PREFIX);
  strcat(watchMsgName,varName);
  x_ipcIgnoreLogging(watchMsgName);

  x_ipcFree(setMsgName);
  x_ipcFree(getMsgName);
  x_ipcFree(getSetMsgName);
  x_ipcFree(watchMsgName);
}

void x_ipcFreeVar(const char *varName, void *varData)
{
  char *getMsgName;

  getMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_GET_PREFIX));
  strcpy(getMsgName,VAR_GET_PREFIX);
  strcat(getMsgName,varName);

  x_ipcFreeReply(getMsgName, varData);

  x_ipcFree(getMsgName);

}

static void x_ipc_fileLogVarHnd(X_IPC_REF_PTR Ref, LOG_PTR log)
{
#ifdef UNUSED_PRAGMA
#pragma unused(Ref)
#endif
  LOCK_M_MUTEX;
  if (GET_M_GLOBAL(logList)[0] != NULL)
    x_ipcFree((void *)GET_M_GLOBAL(logList)[0]);
  GET_M_GLOBAL(logList)[0] = log;
  GET_M_GLOBAL(logList)[2] = NULL;
  UNLOCK_M_MUTEX;
}

static void x_ipc_terminalLogVarHnd(X_IPC_REF_PTR Ref, LOG_PTR log)
{
#ifdef UNUSED_PRAGMA
#pragma unused(Ref)
#endif
  LOCK_M_MUTEX;
  if (GET_M_GLOBAL(logList)[1] != NULL)
    x_ipcFree((void *)GET_M_GLOBAL(logList)[1]);
  GET_M_GLOBAL(logList)[1] = log;
  GET_M_GLOBAL(logList)[2] = NULL;
  UNLOCK_M_MUTEX;
}

static void x_ipc_tappedMsgVarHnd(X_IPC_REF_PTR Ref, STR_LIST_PTR tapped)
{
#ifdef UNUSED_PRAGMA
#pragma unused(Ref)
#endif
  MSG_PTR msg;

  LOCK_CM_MUTEX;
  x_ipc_strListFree(&GET_C_GLOBAL(tappedMsgs),TRUE);
  
  GET_C_GLOBAL(tappedMsgs) = tapped;
  STR_LIST_ITERATE(tapped, string, 
                   { LOCK_CM_MUTEX; msg = GET_MESSAGE(string); UNLOCK_CM_MUTEX;
		     if (msg != NULL) {
		       msg->direct = FALSE;
		     }});
  UNLOCK_CM_MUTEX;
}

static void x_ipc_broadcastMsgVarHnd(X_IPC_REF_PTR Ref, STR_LIST_PTR broadcast)
{
#ifdef UNUSED_PRAGMA
#pragma unused(Ref)
#endif
  LOCK_CM_MUTEX;
  x_ipc_strListFree(&GET_C_GLOBAL(broadcastMsgs),TRUE);
  
  GET_C_GLOBAL(broadcastMsgs) = broadcast;
  UNLOCK_CM_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_modVarInitialize()
 *
 * DESCRIPTION: 
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipc_modVarInitialize(void)
{

  LOCK_CM_MUTEX;
  GET_M_GLOBAL(logList)[0] = (LOG_PTR) x_ipcMalloc(sizeof(LOG_TYPE));
  GET_M_GLOBAL(logList)[1] = (LOG_PTR) x_ipcMalloc(sizeof(LOG_TYPE));
  GET_M_GLOBAL(logList)[2] = NULL;

  /* Register Handlers for the logging status. */
  x_ipcGetVar(X_IPC_TERMINAL_LOG_VAR, (GET_M_GLOBAL(logList)[0]));
  x_ipcGetVar(X_IPC_FILE_LOG_VAR, (GET_M_GLOBAL(logList)[1]));
  /* Register Handlers for tapped messages. */
  GET_C_GLOBAL(tappedMsgs) = x_ipc_strListCreate();
  x_ipcGetVar(X_IPC_TAPPED_MSG_VAR, GET_C_GLOBAL(tappedMsgs));
  /* Register Handlers for broadcast messages. */
  GET_C_GLOBAL(broadcastMsgs) = x_ipc_strListCreate();
  x_ipcGetVar(X_IPC_BROADCAST_MSG_VAR, GET_C_GLOBAL(broadcastMsgs));

  /* Only want to tap if the module is also listening for other messages. */
  if (GET_C_GLOBAL(willListen) && GET_C_GLOBAL(directDefault)) {
    x_ipcWatchVar(X_IPC_TERMINAL_LOG_VAR,X_IPC_LOG_VAR_FORMAT, x_ipc_terminalLogVarHnd);
    x_ipcWatchVar(X_IPC_FILE_LOG_VAR, X_IPC_LOG_VAR_FORMAT, x_ipc_fileLogVarHnd);
    x_ipcWatchVar(X_IPC_TAPPED_MSG_VAR,X_IPC_STR_LIST_FORMAT, x_ipc_tappedMsgVarHnd);
    x_ipcLimitPendingMessages(X_IPC_TERMINAL_LOG_VAR, 
			    GET_M_GLOBAL(modNameGlobal), 1);
    x_ipcLimitPendingMessages(X_IPC_FILE_LOG_VAR, 
			    GET_M_GLOBAL(modNameGlobal), 1);
    x_ipcLimitPendingMessages(X_IPC_TAPPED_MSG_VAR, 
			    GET_M_GLOBAL(modNameGlobal), 1);
  }

  /* Only want to tap if the module is also listening for other messages. */
  if (GET_C_GLOBAL(willListen)) {
    x_ipcWatchVar(X_IPC_BROADCAST_MSG_VAR,X_IPC_STR_LIST_FORMAT, x_ipc_broadcastMsgVarHnd);
    x_ipcLimitPendingMessages(X_IPC_BROADCAST_MSG_VAR, 
			    GET_M_GLOBAL(modNameGlobal), 1);
  } else {
    x_ipc_strListFree(&GET_C_GLOBAL(broadcastMsgs), TRUE);
    GET_C_GLOBAL(broadcastMsgs) = NULL;
  }
  UNLOCK_CM_MUTEX;
}


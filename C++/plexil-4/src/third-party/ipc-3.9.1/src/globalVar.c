/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1994.
 * 
 * MODULE: communications
 *
 * FILE: globalVar.c
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
 * $Log: globalVar.c,v $
 * Revision 2.5  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2002/06/25 16:45:56  reids
 * Removed memory leak when global var that already exists is re-registered.
 *
 * Revision 2.3  2000/07/03 17:03:24  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/02/25 14:07:26  reids
 * Use of UNUSED_PRAGMA for compilers that do not support "#pragma unused"
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.3  1996/12/18 15:12:48  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.2  1996/10/22 18:49:40  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.2.2.1  1996/10/16 15:19:07  reids
 * Removed compiler warning.
 *
 * Revision 1.2  1996/06/14 20:50:18  reids
 * Fixed two bugs related to freeing memory.
 *
 * Revision 1.1  1996/05/09 01:01:28  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:19:46  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:33  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.13  1996/06/25  20:50:41  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.12  1996/06/05  19:47:53  reids
 * Registering global vars freed memory that was later being used.
 *
 * Revision 1.11  1996/03/02  03:21:39  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.10  1996/02/14  22:12:27  rich
 * Eliminate extra variable logging on startup.
 *
 * Revision 1.9  1995/10/07  19:07:22  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.8  1995/07/19  14:26:20  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.7  1995/07/06  21:16:33  rich
 * Solaris and Linux changes.
 *
 * Revision 1.6  1995/04/04  19:42:23  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.5  1995/03/30  15:43:08  rich
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
 * Revision 1.4  1995/03/28  01:14:39  rich
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
 * Revision 1.3  1994/10/25  17:09:58  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.2  1994/05/25  04:57:29  rich
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
 * Revision 1.1  1994/05/17  23:16:00  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 *
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"
#include "modVar.h"

/* Private Global types */

typedef struct _var {
  const char *varName;
  const char *format;
  void *value;
} GLOBAL_VAR_TYPE, *GLOBAL_VAR_PTR;

typedef const GLOBAL_VAR_TYPE *CONST_GLOBAL_VAR_PTR;


/*****************************************************************************
 *
 * FUNCTION: static void setVarHnd(DISPATCH_PTR dispatch, void *varValue)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static void setVarHnd(DISPATCH_PTR dispatch, void *varValue)
{
  GLOBAL_VAR_PTR var=NULL;
  const char *msgName = dispatch->msg->msgData->name;
  const char *varName;
  char *watchMsgName;
  
  varName = &(msgName[strlen(VAR_SET_PREFIX)]);
  
  var = (GLOBAL_VAR_PTR) x_ipc_hashTableFind(varName, GET_S_GLOBAL(varTable));
  if (var == NULL) {
    /* handle the error here. */
  } else {
    /* Free the old data if it exists */
    if (var->value != NULL)
      x_ipcFreeData(msgName,var->value);
    /* Store the pointer to the new data */
    var->value = varValue;
  }
  /* Need to do a x_ipcSuccess, if a command. */
  /*  centralSuccess(dispatch);*/
  /* Broadcast the result. */
  
  watchMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_WATCH_PREFIX));
  strcpy(watchMsgName,VAR_WATCH_PREFIX);
  strcat(watchMsgName,varName);

  centralBroadcast(watchMsgName,var->value);
  x_ipcFree(watchMsgName);
}


/*****************************************************************************
 *
 * FUNCTION: static void getVarHnd(DISPATCH_PTR dispatch, void *empty)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static void getVarHnd(DISPATCH_PTR dispatch, void *empty)
{
#ifdef UNUSED_PRAGMA
#pragma unused(empty)
#endif
  
  GLOBAL_VAR_PTR var=NULL;
  const char *msgName;
  const char *varName;
  
  msgName = dispatch->msg->msgData->name;
  varName = &(msgName[strlen(VAR_SET_PREFIX)]);
  var = (GLOBAL_VAR_PTR) x_ipc_hashTableFind(varName, GET_S_GLOBAL(varTable));
  if (var == NULL) {
    /* handle the error here. */
  } else {
    centralReply(dispatch, (char *)var->value);
  }
}


/*****************************************************************************
 *
 * FUNCTION: static void getSetVarHnd(DISPATCH_PTR dispatch, void *varValue)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static void getSetVarHnd(DISPATCH_PTR dispatch, void *varValue)
{
  GLOBAL_VAR_PTR var=NULL;
  const char *msgName;
  const char *varName;
  
  msgName = dispatch->msg->msgData->name;
  varName = &(msgName[strlen(VAR_GET_SET_PREFIX)]);
  var = (GLOBAL_VAR_PTR) x_ipc_hashTableFind(varName, GET_S_GLOBAL(varTable));
  if (var == NULL) {
    /* handle the error here. */
  } else {
    /* Free the old data if it exists */
    if (var->value != NULL) {
      centralReply(dispatch, (char *)var->value);
      x_ipcFreeData(msgName,var->value);
    }
    /* Store the pointer to the new data */
    var->value = varValue;
  }
}


/*****************************************************************************
 *
 * FUNCTION: void registerVarHnd(DISPATCH_PTR dispatch, VAR_REG_PTR varRegData)
 
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static void registerVarHnd(DISPATCH_PTR dispatch, VAR_REG_PTR varRegData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dispatch)
#endif

  GLOBAL_VAR_PTR var;  
  CONST_GLOBAL_VAR_PTR old_var;  
  char *getMsgName, *setMsgName, *getSetMsgName, *watchMsgName;
  
  var = NEW(GLOBAL_VAR_TYPE);
  
  /* Create the data structure to hold information about the variable */
  var->varName = varRegData->varName;
  var->format = varRegData->format;
  var->value = NULL;
  
  /* Create the name strings for the set and get messages.*/
  
  setMsgName = (char *)x_ipcMalloc(1+strlen(var->varName)+
				   strlen(VAR_SET_PREFIX));
  strcpy(setMsgName,VAR_SET_PREFIX);
  strcat(setMsgName,var->varName);
  
  getMsgName = (char *)x_ipcMalloc(1+strlen(var->varName)+
				   strlen(VAR_GET_PREFIX));
  strcpy(getMsgName,VAR_GET_PREFIX);
  strcat(getMsgName,var->varName);
  
  getSetMsgName = (char *)x_ipcMalloc(1+strlen(var->varName)+
				      strlen(VAR_GET_SET_PREFIX));
  strcpy(getSetMsgName,VAR_GET_SET_PREFIX);
  strcat(getSetMsgName,var->varName);
  
  watchMsgName = (char *)x_ipcMalloc(1+strlen(var->varName)+
				     strlen(VAR_WATCH_PREFIX));
  strcpy(watchMsgName,VAR_WATCH_PREFIX);
  strcat(watchMsgName,var->varName);
  
  /* Free up the old var, if any */
  old_var = (CONST_GLOBAL_VAR_PTR)x_ipc_hashTableInsert((void *)var->varName,
						  1+strlen(var->varName),
						  (void *)var,
						  GET_S_GLOBAL(varTable));
  if (old_var != NULL) {
    /* The mesage is already registered, just check the format.   */
    if (strcmp(old_var->format,var->format)) {
      X_IPC_ERROR1("ERROR: centralRegisterVar: variable %s already registered"
		   " with a differnt format.\n", var->varName);
    }
    /* Free the old var */
    if (old_var->value) x_ipcFreeData(setMsgName, old_var->value);
    if (old_var->format) x_ipcFree((void *)old_var->format);
    x_ipcFree((void *)old_var->varName);
    x_ipcFree((void *)old_var);
  }

  /* Register the functions to set and get the variable value.  */
  
  centralRegisterInform(setMsgName, var->format, setVarHnd);
  
  centralRegisterQuery(getMsgName, NULL, var->format, getVarHnd);
  
  centralRegisterQuery(getSetMsgName, var->format, var->format, getSetVarHnd);

  centralRegisterBroadcastMessage(watchMsgName, var->format);

  x_ipcFree(setMsgName);
  x_ipcFree(getMsgName);
  x_ipcFree(getSetMsgName);
  x_ipcFree(watchMsgName);
  /* Just free top level structure -- The strings need to be saved */
  x_ipcFree((char *) varRegData);
}


/*****************************************************************************
 *
 * FUNCTION: void centralRegisterVar
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
void centralRegisterVar(const char *varName, const char *format)
{
  GLOBAL_VAR_TYPE var;
  
  var.varName = varName;
  var.format = format;
  centralInform(X_IPC_REGISTER_VAR_INFORM, &var);
}


/*****************************************************************************
 *
 * FUNCTION: void centralSetVar
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
void centralSetVar(const char *varName, const char *value)
{
  char *setMsgName;

  if (!mGlobalp())
    return;
  
  setMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_SET_PREFIX));
  strcpy(setMsgName,VAR_SET_PREFIX);
  strcat(setMsgName,varName);
  
  centralInform(setMsgName, value);
  x_ipcFree(setMsgName);
}


/*****************************************************************************
 *
 * FUNCTION: void centralIgnoreVarLogging
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
void centralIgnoreVarLogging(const char *varName)
{
  char *setMsgName;
  char *getMsgName;
  char *getSetMsgName;
  char *watchMsgName;
  
  setMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_SET_PREFIX));
  strcpy(setMsgName,VAR_SET_PREFIX);
  strcat(setMsgName,varName);
  Add_Message_To_Ignore(setMsgName);
  x_ipcFree(setMsgName);

  getMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_GET_PREFIX));
  strcpy(getMsgName,VAR_GET_PREFIX);
  strcat(getMsgName,varName);
  Add_Message_To_Ignore(getMsgName);
  x_ipcFree(getMsgName);
  
  getSetMsgName = (char *)x_ipcMalloc(1+strlen(varName)+ 
				    strlen(VAR_GET_SET_PREFIX));
  strcpy(getSetMsgName,VAR_GET_SET_PREFIX);
  strcat(getSetMsgName,varName);
  Add_Message_To_Ignore(getSetMsgName);
  x_ipcFree(getSetMsgName);
  
  watchMsgName = (char *)x_ipcMalloc(1+strlen(varName)+strlen(VAR_WATCH_PREFIX));
  strcpy(watchMsgName,VAR_WATCH_PREFIX);
  strcat(watchMsgName,varName);
  Add_Message_To_Ignore(watchMsgName);
  x_ipcFree(watchMsgName);
}


/*****************************************************************************
 *
 * FUNCTION: void globalVarInitialize(void)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
void globalVarInitialize(void)
{
  /* Create the table to hold the information for each global variable. */
  GET_S_GLOBAL(varTable) = x_ipc_hashTableCreate(11, (HASH_FN) x_ipc_strHashFunc, 
					   (EQ_HASH_FN)x_ipc_strKeyEqFunc);
  
  /* Register the handler for creating the global variable */
  
  centralRegisterInform(X_IPC_REGISTER_VAR_INFORM,
			X_IPC_REGISTER_VAR_INFORM_FORMAT,
			registerVarHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_VAR_INFORM);
  centralRegisterInform(X_IPC_REGISTER_VAR_INFORM_OLD,
			X_IPC_REGISTER_VAR_INFORM_FORMAT,
			registerVarHnd);
  Add_Message_To_Ignore(X_IPC_REGISTER_VAR_INFORM_OLD);

  centralRegisterVar(X_IPC_TERMINAL_LOG_VAR, X_IPC_LOG_VAR_FORMAT);
  centralRegisterVar(X_IPC_FILE_LOG_VAR, X_IPC_LOG_VAR_FORMAT);
  centralIgnoreVarLogging(X_IPC_TERMINAL_LOG_VAR);
  centralIgnoreVarLogging(X_IPC_FILE_LOG_VAR);
  centralSetVar(X_IPC_TERMINAL_LOG_VAR, (char *)&GET_S_GLOBAL(terminalLog));
  centralSetVar(X_IPC_FILE_LOG_VAR, (char *)&GET_S_GLOBAL(fileLog));
}

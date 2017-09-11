/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: x_ipc
 *
 * FILE: msgTap.c
 *
 * ABSTRACT: 
 *
 * Provide the capability to "tap" a message.
 *
 * One can set up a "tap" on a message so that whenever a specified condition
 * holds, a "listening" message is sent the same data as the tapped message.
 * The conditions under which the tap is sent relate mainly to the status of
 * the tapped message (e.g., before it is handled, after a reply is received, 
 * after the message, and its submessages, are achieved, etc.).
 * The file "msgTapMon.c" gives a complete listing of the available conditions.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/msgTap.c,v $ 
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: msgTap.c,v $
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2000/07/03 17:03:26  hersh
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
 * Revision 1.2.2.2  1996/12/18 15:12:58  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.1  1996/10/18 18:14:40  reids
 * Distinguish when ref and parent id's should be logged.
 *
 * Revision 1.2  1996/05/24 16:45:57  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:45  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:19:51  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:03  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.34  1996/07/19  18:14:20  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.33  1996/03/02  03:21:45  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.32  1996/02/14  22:12:37  rich
 * Eliminate extra variable logging on startup.
 *
 * Revision 1.31  1996/02/10  16:50:17  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.30  1996/01/30  15:04:30  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.29  1996/01/27  21:53:37  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.28  1995/12/17  20:21:45  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.27  1995/10/29  18:26:55  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.26  1995/10/25  22:48:39  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.25  1995/10/17  17:34:58  reids
 * Correctly handle case where there are per-task-tree node taps, but no
 * message-wide taps for the task-tree node's message.
 *
 * Revision 1.24  1995/10/10  00:43:02  rich
 * Added more system messages to ignore.
 *
 * Revision 1.23  1995/10/07  19:07:33  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.22  1995/07/30  17:09:41  rich
 * More DOS compatibility. Fixes to devUtils.
 *
 * Revision 1.21  1995/07/12  04:55:01  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.20  1995/07/10  16:18:02  rich
 * Interm save.
 *
 * Revision 1.19  1995/05/31  19:36:04  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.18  1995/04/19  14:28:27  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.17  1995/04/04  19:42:37  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.16  1995/03/30  15:43:34  rich
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
 * Revision 1.15  1995/03/19  19:39:38  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.14  1995/01/18  22:41:27  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.13  1994/10/25  17:10:15  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.12  1994/05/25  04:57:52  rich
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
 * Revision 1.11  1994/05/17  23:16:49  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.10  1994/04/28  16:16:29  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.9  1994/04/16  19:42:47  rich
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
 * Revision 1.8  1994/01/31  18:28:18  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.7  1993/12/14  17:34:22  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.6  1993/11/21  20:18:40  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.5  1993/10/21  16:14:05  rich
 * Fixed compiler warnings.
 *
 * Revision 1.4  1993/08/30  21:53:55  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.5  1993/08/23  17:39:19  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.4  1993/06/28  13:41:49  reids
 * WhenSuccess and WhenFailure were not fully integrated.  Added to switch
 * statements.
 *
 * Revision 1.3  1993/06/22  13:59:47  rich
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
 * Revision 1.2  1993/05/27  22:18:43  rich
 * Added automatic logging.
 *
 * 13-Sep-91 Christopher Fedor, School of Computer Science, CMU
 * Changed conditionName from an array to a function.
 *
 * 18-Aug-91 Christopher Fedor, School of Computer Science, CMU
 * Changed tapClassData for commands to be of type BLOCK_COM_PTR.
 *
 * 17-Aug-91 Christopher Fedor, School of Computer Science, CMU
 * Modified tapClassData to malloc the class data instead of passing an
 * address.
 *
 *  8-Jul-91 Reid Simmons, School of Computer Science, CMU
 * "cleanUpHandler" had to be enhanced considerably -- there were real bad
 * interactions when it was called by a rule, and killed off a task tree before
 * other rules had a chance to fire! 
 *
 * 24-Jun-91 Reid Simmons, School of Computer Science, CMU
 * Streamlined use of rules, and added "cleanUpAfterAchieved" message.
 *
 * 23-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Added x_ipc_findOrRegisterMessage calls to tapMessageHandler call to create 
 * a temp message for missing messages. This call is used by handlers
 * that are registered before their message. Because taps are activated
 * only in response to a message being used, other routines will catch the
 * missing message and update things before the tap call. The update occurs
 * in selfRegisterMsg.
 *
 * 12-Mar-91 Reid Simmons, School of Computer Science, CMU
 * Created.
 *
 *****************************************************************************/

#include "globalS.h"

#ifndef NMP_IPC
/* Defined below; forward reference */
static void tapActivation(const char *ruleName, TMS_NODE_PTR relationshipNode,
			  LIST_ITER_FN activationFn);
#endif

/*****************************************************************************
 * 
 * FUNCTION: char *conditionName(cond)
 *
 * DESCRIPTION: Return a string for display use of the condition name.
 *
 * INPUTS: TAP_CONDITION_TYPE cond
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

static char *conditionName(TAP_CONDITION_TYPE cond)
{
  switch(cond) {
  case WhenSent:
    return("WhenSent");
  case BeforeHandling:
    return("BeforeHandling");
  case WhileHandling:
    return("WhileHandling");
  case AfterHandled:
    return("AfterHandled");
  case AfterReplied:
    return("AfterReplied");
  case AfterSuccess:
    return("AfterSuccess");
  case AfterFailure:
    return("AfterFailure");
  case BeforeAchieving:
    return("BeforeAchieving");
  case WhileAchieving:
    return("WhileAchieving");
  case AfterAchieved:
    return("AfterAchieved");
  case WhenAchieved:
    return("WhenAchieved");
  case BeforePlanning:
    return("BeforePlanning");
  case WhilePlanning:
    return("WhilePlanning");
  case AfterPlanned:
    return("AfterPlanned");
  case WhenPlanned:
    return("WhenPlanned");
  case WhenSuccess:
    return("WhenSuccess");
  case WhenFailure:
    return("WhenFailure");
  case WhenKilled:
    return("WhenKilled");
  case UnknownCondition:
#ifndef TEST_CASE_COVERAGE
  default:
#endif
    return("UNKNOWN CONDITION");
  }
}


/*****************************************************************************
 * 
 * FUNCTION: void recordTap(const char *name)
 *
 * DESCRIPTION: 
 *
 * INPUTS: message name.
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void recordTap(const char *name)
{
  if (!x_ipc_strListMemberItem(name, GET_C_GLOBAL(tappedMsgs))) {
    x_ipc_strListPush(strdup(name), GET_C_GLOBAL(tappedMsgs));
    centralSetVar(X_IPC_TAPPED_MSG_VAR, (char *)GET_C_GLOBAL(tappedMsgs));
  }
}


/*****************************************************************************
 * 
 * FUNCTION: int32 sameTap(tap1, tap2)
 *
 * DESCRIPTION: Do the two taps have the same conditions and messages?
 *
 * INPUTS: TAP_PTR tap1, tap2
 *
 * OUTPUTS: TRUE/FALSE
 *
 *****************************************************************************/

static int32 sameTap(TAP_PTR tap1, TAP_PTR tap2)
{
  return ((tap1->condition == tap2->condition) &&
	  (tap1->listeningMsg == tap2->listeningMsg));
}

/******************************************************************************
 *
 * FUNCTION: addTapInfoToDispatch(tappedDispatch, allocateExtraTaps)
 * 
 * DESCRIPTION: Add a TAP_INFO_TYPE structure to the tapInfo field of the 
 *  tappedDispatch (if one was not there already).  If allocateExtraTaps is
 *  TRUE, start a list of extra taps for this dispatch ("extra taps" apply only
 *  to the dispatch itself, not to all messages of that type; they are added
 *  using "x_ipcTapReference").
 *
 * INPUTS: DISPATCH_PTR tappedDispatch;
 *         int32 allocateExtraTaps; (TRUE/FALSE)
 *
 * OUTPUTS: modifies the tapInfo field of the tappedDispatch
 *
 *****************************************************************************/

static void addTapInfoToDispatch(DISPATCH_PTR tappedDispatch,
				 int32 allocateExtraTaps)
{
  TAP_INFO_PTR tapInfo;
  
  if (!tappedDispatch->tapInfo) {
    tapInfo = NEW(TAP_INFO_TYPE);
    tapInfo->tappedDispatch = NULL;
    tapInfo->extraTaps = NULL;
    tappedDispatch->tapInfo = tapInfo;
  }
  if (allocateExtraTaps && !tappedDispatch->tapInfo->extraTaps) {
    tappedDispatch->tapInfo->extraTaps = x_ipc_listCreate();
  }
}


/******************************************************************************
 *
 * FUNCTION: freeTapInfo(tapInfo)
 * 
 * DESCRIPTION: Free the tap info structure and the "extraTaps" list
 *
 * INPUTS: TAP_INFO_PTR tapInfo
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void freeTapInfo(TAP_INFO_PTR *tapInfo)
{
  if ((*tapInfo)) {
    if ((*tapInfo)->extraTaps) 
      x_ipc_listFree(&((*tapInfo)->extraTaps));
    x_ipcFree((char *)(*tapInfo));
    *tapInfo = NULL;
  }
}


/******************************************************************************
 *
 * FUNCTION: 
 * 
 * DESCRIPTION: Returns TRUE if the tapped and listening messages are the right
 *              types for the given tap condition
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static int32 checkTapMessageClasses(TAP_CONDITION_TYPE tapCondition,
				  MSG_PTR tappedMsg, MSG_PTR listeningMsg)
{
  X_IPC_MSG_CLASS_TYPE tappedClass, listeningClass;
  int32 queryType;
  
  tappedClass = tappedMsg->msgData->msg_class;
  listeningClass = listeningMsg->msgData->msg_class;
  
  if (listeningClass == QueryClass) {
    LOG_MESSAGE1("TAP NOT ADDED: the listening message %s must be a query\n",
		listeningMsg->msgData->name);
    return FALSE;
  }
  
  switch (tapCondition) {
  case WhenSent: 
  case BeforeHandling: 
  case WhileHandling:
  case AfterHandled: 
    return TRUE;
    
  case AfterReplied: 
    queryType = (tappedClass == QueryClass || tappedClass == MultiQueryClass);
    if (!queryType) {
      LOG_MESSAGE2("TAP NOT ADDED: %s must be a query for the '%s' condition\n",
		  tappedMsg->msgData->name, conditionName(tapCondition));
    } 
    return queryType; 
    
  case AfterSuccess:
  case AfterFailure: 
  case WhenSuccess:
  case WhenFailure: 
    if ((tappedClass == GoalClass) || (tappedClass == CommandClass)) {
      return TRUE;
    } else {
      LOG_MESSAGE2("TAP NOT ADDED: %s must be a goal or command for '%s' condition\n",
		  tappedMsg->msgData->name, conditionName(tapCondition));
      return FALSE;
    }
    
  case BeforeAchieving:
  case WhileAchieving:
  case AfterAchieved:
  case WhenAchieved: 
  case WhenKilled: 
    if ((tappedClass == GoalClass) || (tappedClass == CommandClass) ||
	(tappedClass == PointMonitorClass) || 
	(tappedClass == PollingMonitorClass) || 
	(tappedClass == DemonMonitorClass)){
      return TRUE;
    } else {
      LOG_MESSAGE2("TAP NOT ADDED: %s must be a goal, command or monitor for '%s' condition\n",
		  tappedMsg->msgData->name, conditionName(tapCondition));
      return FALSE;
    }
    
  case BeforePlanning:
  case WhilePlanning:
  case AfterPlanned:
  case WhenPlanned: 
    if (tappedClass != GoalClass) {
      LOG_MESSAGE2("TAP NOT ADDED: %s must be a goal for '%s' condition\n",
		  tappedMsg->msgData->name, conditionName(tapCondition));
    }
    return (tappedClass == GoalClass);
    
  case UnknownCondition:
#ifndef TEST_CASE_COVERAGE
  default: 
#endif
    LOG_MESSAGE1("DID NOT FIND %d\n", tapCondition);
    return FALSE;
  }
}


/******************************************************************************
 *
 * FUNCTION: 
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

/* Return the nominal class data associated with messages of the given class */

static char *tapClassData(X_IPC_MSG_CLASS_TYPE msg_class)
{
  int32 *tplConstr;
  BLOCK_COM_PTR comData;
  MULTI_QUERY_CLASS_PTR mqData;
  
  switch (msg_class) {
  case GoalClass:
  case PointMonitorClass: 
    tplConstr = NEW(int32);
    *tplConstr = NO_TPLCONSTR;
    return (char *)tplConstr;
    
  case CommandClass:
    comData = NEW(BLOCK_COM_TYPE);
    comData->waitFlag = FALSE;
    comData->tplConstr = NO_TPLCONSTR;
    return (char *)comData;
    
  case PollingMonitorClass:
  case DemonMonitorClass: 
    X_IPC_ERROR("tapClassData: Monitor class data not implemented");
    
  case QueryClass:
  case InformClass:
  case BroadcastClass:
  case FailureClass: 
    return NULL;
    
  case MultiQueryClass:
    mqData = NEW(MULTI_QUERY_CLASS_TYPE);
    mqData->num = mqData->max = 0;
    return (char *)mqData;
    
  default: 
    X_IPC_ERROR1("tapClassData: Class type  %d not handled\n",msg_class );
    return NULL;
  }
}


/******************************************************************************
 *
 * FUNCTION: int32 checkTaps(msg, tap)
 * 
 * DESCRIPTION: 
 * List iterator for messages with taps that were registered after the tap
 * was registered.
 *
 * INPUTS: 
 * MSG_PTR msg; - the message being tapped.
 * TAP_PTR tap;
 *         
 * OUTPUTS: 1 - force entire list iteration.
 *
 *****************************************************************************/

int32 checkTaps(MSG_PTR msg, TAP_PTR tap)
{
  if (!checkTapMessageClasses(tap->condition, msg, tap->listeningMsg))
    x_ipc_listDeleteItem((char *)tap, msg->tapList);
  
  return 1;
}


/******************************************************************************
 *
 * FUNCTION: tapMessageHandler(dispatch, tapData)
 * 
 * DESCRIPTION: Handle the x_ipcTapMessage call
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *         TAP_MSG_PTR tapData;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/
/*ARGSUSED*/
static void tapMessageHandler(DISPATCH_PTR dispatch, TAP_MSG_PTR tapData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dispatch)
#endif
  TAP_PTR tap;
  MSG_PTR tappedMsg, listeningMsg;
  
  tappedMsg = x_ipc_findOrRegisterMessage(tapData->tappedMsg);
  listeningMsg = x_ipc_findOrRegisterMessage(tapData->listeningMsg);
  
  if (tappedMsg->msgData->msg_class == HandlerRegClass ||
      listeningMsg->msgData->msg_class == HandlerRegClass ||
      checkTapMessageClasses(tapData->condition, tappedMsg, listeningMsg)){
    tap = NEW(TAP_TYPE);
    tap->condition = tapData->condition;
    tap->listeningMsg = listeningMsg;
    if (!tappedMsg->tapList) {
      tappedMsg->tapList = x_ipc_listCreate();
    }
    if (x_ipc_listMemReturnItem((LIST_ITER_FN) sameTap,
			  (char *)tap, tappedMsg->tapList)) {
      LOG_MESSAGE3("Tap %s for %s with condition %s already inserted\n", 
		  tapData->listeningMsg, tapData->tappedMsg,
		  conditionName(tapData->condition));
      x_ipcFree((char *)tap);
    } else {
      x_ipc_listInsertItemLast((char *)tap, tappedMsg->tapList);
      recordTap(tapData->tappedMsg);
    }
  }
  /* Just free the top level structure, because the string names might
     be saved in "x_ipc_findOrRegisterMessage" */
  /* A bit more efficient than using x_ipcFreeData */
  /*  x_ipcFreeData(X_IPC_TAP_MSG_INFORM,tapData);*/
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)tapData);
}


/******************************************************************************
 *
 * FUNCTION: tapReferenceHandler(dispatch, tapData)
 * 
 * DESCRIPTION: Handle the x_ipcTapReference call
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *         TAP_REF_PTR tapData;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/
/*ARGSUSED*/
static void tapReferenceHandler(DISPATCH_PTR dispatch, TAP_REF_PTR tapData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dispatch)
#endif
  TAP_PTR tap;
  DISPATCH_PTR tapDispatch;
  MSG_PTR tappedMsg, listeningMsg;
  
  tappedMsg = tapData->tappedRef->msg;
  if (!tappedMsg) tappedMsg = x_ipc_findOrRegisterMessage(tapData->tappedRef->name);
  listeningMsg = x_ipc_findOrRegisterMessage(tapData->listeningMsg);
  
  if (listeningMsg->msgData->msg_class == HandlerRegClass ||
      checkTapMessageClasses(tapData->condition, tappedMsg, listeningMsg)) {
    tap = NEW(TAP_TYPE);
    tap->condition = tapData->condition;
    tap->listeningMsg = listeningMsg;
    tapDispatch = DISPATCH_FROM_ID(tapData->tappedRef->refId);
    addTapInfoToDispatch(tapDispatch, TRUE);
    x_ipc_listInsertItemLast((char *)tap, tapDispatch->tapInfo->extraTaps);
    recordTap(dispatch->msg->msgData->name);
  }
  /* Don't free the listeningMsg string, since it might be used in
     "x_ipc_findOrRegisterMessage" */
  x_ipcRefFree(tapData->tappedRef);
  /* A bit more efficient than using x_ipcFreeData */
  /*  x_ipcFreeData(X_IPC_TAP_REF_INFORM,tapData);*/
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (void *)tapData);
}


/******************************************************************************
 *
 * FUNCTION: findTappedReferenceHandler(dispatch, tapData)
 * 
 * DESCRIPTION: Handle the x_ipcFindTappedReference call
 *
 * INPUTS: DISPATCH_PTR dispatch;
 *         X_IPC_REF_PTR listeningRefPtr;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

static void findTappedReferenceHandler(DISPATCH_PTR dispatch,
				       int32 *listeningRefIdPtr)
{
  X_IPC_REF_PTR tappedRef; 
  DISPATCH_PTR listeningDispatch, tappedDispatch;
  
  listeningDispatch = DISPATCH_FROM_ID(*listeningRefIdPtr);
  if (!IS_LISTENING(listeningDispatch)) {
    centralNullReply(dispatch);
  } else {
    tappedDispatch = listeningDispatch->tapInfo->tappedDispatch;
    tappedRef = x_ipcRefCreate((MSG_PTR)NULL, tappedDispatch->msg->msgData->name,
			     tappedDispatch->locId);
    reserveDispatch(tappedDispatch);
    centralReply(dispatch, (char *)&tappedRef);
    x_ipcRefFree(tappedRef);
  }
  x_ipcFree((char *)listeningRefIdPtr); /* Use simple free: only 1 int */
}


/******************************************************************************
 *
 * FUNCTION: void removeTapHandler(dispatch, tapData)
 * 
 * DESCRIPTION: Handle the x_ipcRemoveTap call
 *
 * INPUTS: 
 * DISPATCH_PTR dispatch;
 * TAP_MSG_PTR tapData;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

static void removeTapHandler(DISPATCH_PTR dispatch, TAP_MSG_PTR tapData)
{
  TAP_TYPE tap;
  MSG_PTR tappedMsg, listeningMsg;
  
  tappedMsg  = GET_MESSAGE(tapData->tappedMsg);
  if (!tappedMsg) {
    LOG_MESSAGE1("TAP NOT REMOVED: Message %s not registered\n",
		tapData->tappedMsg);
  } else {
    listeningMsg = GET_MESSAGE(tapData->listeningMsg);
    if (!listeningMsg) {
      LOG_MESSAGE1("TAP NOT REMOVED: Message %s not registered\n", 
		  tapData->listeningMsg);
    } else if (!tappedMsg->tapList) {
      LOG_MESSAGE1("%s does not currently have taps", tapData->tappedMsg);
    } else {
      tap.condition = tapData->condition;
      tap.listeningMsg = listeningMsg;
      x_ipc_listTestDeleteItem((LIST_ITER_FN) sameTap, 
			 (char *)&tap, tappedMsg->tapList);
    }
  }
  /* A bit more efficient than using x_ipcFreeData. 
   * It's OK to free the whole structure here, because the code above
   * only uses "x_ipc_hashTableFind" -- the message names are not stored anywhere
   */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, (char *)tapData);
}


/*****************************************************************************
 *
 * FUNCTION: cleanUpLater(ruleName, node, killList)
 * 
 * DESCRIPTION: Kill all the nodes in the killList after the 
 *              other rules have run.  This is a bit hairy to prevent nodes
 *              from being killed multiple times.
 *
 * INPUTS: 
 *
 * OUTPUTS: Returns TRUE (needed by x_ipc_listIterate)
 *
 *****************************************************************************/
#ifndef NMP_IPC
static int32 removeChildrenFromKillList (LIST_PTR killList,
				       TASK_TREE_NODE_PTR treeNode)
{
  x_ipc_listDeleteItem((char *)treeNode->dispatch, killList);
  (void)x_ipc_listIterate((LIST_ITER_FN)removeChildrenFromKillList, 
		    (char *)killList, 
		    treeNode->children);
  return TRUE;
}

/*ARGSUSED*/
static void cleanUpLater (const char *ruleName, TMS_NODE_PTR node, 
			  LIST_PTR killList)
{
#ifdef UNUSED_PRAGMA
#pragma unused(ruleName, node)
#endif
  DISPATCH_PTR dispatchToKill;
  
  dispatchToKill = (DISPATCH_PTR)x_ipc_listFirst(killList);
  while (dispatchToKill) {
    /* Avoid killing nodes multiple times */
    (void)x_ipc_listIterate((LIST_ITER_FN)removeChildrenFromKillList, 
		      (char *)killList,
		      dispatchToKill->treeNode->children);
    
    killTaskTree(dispatchToKill->locId);
    dispatchToKill = (DISPATCH_PTR)x_ipc_listNext(killList);
  }
  x_ipc_listFree(&(killList));
  
  GET_S_GLOBAL(killLaterRuleGlobal) = NULL;
}
#endif

/*****************************************************************************
 *
 * FUNCTION: cleanUpHandler(listeningDispatch, dummy)
 * 
 * DESCRIPTION: Find the associated tapped reference and kill its task tree.
 *
 * INPUTS: 
 * DISPATCH_PTR listeningDispatch;
 * char *dummy;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/
#ifndef NMP_IPC
/*ARGSUSED*/
static void cleanUpHandler(DISPATCH_PTR listeningDispatch, void *dummy)
{
#ifdef UNUSED_PRAGMA
#pragma unused(dummy)
#endif
  DISPATCH_PTR tappedDispatch;
  
  tappedDispatch = listeningDispatch->tapInfo->tappedDispatch;
  
  if (!tappedDispatch->treeNode) {
    LOG_STATUS1("ERROR: Trying to clean up a dispatch (%s",
	       tappedDispatch->msg->msgData->name);
    Log_RefId(tappedDispatch, LOGGING_STATUS);
    LOG_STATUS(") with no task tree node");
  } else if (GET_S_GLOBAL(isDelayedGlobal)) {
    /* Delay the killing of the dispatch until the current set of rules 
       have run */
    if (!GET_S_GLOBAL(killLaterRuleGlobal)) {
      GET_S_GLOBAL(killLaterRuleGlobal) = 
	ruleCreateNode("Delay Kill Rule",
		       (NODE_RULE_FN)  cleanUpLater, 5,
		       GET_S_GLOBAL(alwaysTrueNodeGlobal), 
		       (char *)x_ipc_listCreate());
      addRuleToNode(GET_S_GLOBAL(killLaterRuleGlobal), 
		    GET_S_GLOBAL(alwaysTrueNodeGlobal));
    }
    x_ipc_listInsertItemLast((char *)tappedDispatch,
		       (LIST_PTR)GET_S_GLOBAL(killLaterRuleGlobal)->parameters);
  } else {
    killTaskTree(tappedDispatch->locId);
  }
}
#endif

/******************************************************************************
 *
 * FUNCTION: int32 tapOK(tap)
 * 
 * DESCRIPTION: 
 * Returns true if tap message was registered and has a valid handler.
 *
 * INPUTS: 
 * TAP_PTR tap;
 *
 * OUTPUTS: int32
 *
 * NOTES:
 * A handler will have a resource with capacity of 0 if the server created it.
 *
 *****************************************************************************/

static int32 tapOK(TAP_PTR tap)
{
  HND_PTR hnd;
  MSG_PTR msg;
  int32 flag;
  LIST_PTR hndList;
  
  msg = tap->listeningMsg;
  
  if (msg->msgData->msg_class == HandlerRegClass) {
    LOG_MESSAGE1("\nWARNING: Tap Message Ignored: %s Not Registered.\n",
		msg->msgData->name);
    return 0;
  }
  
  hndList = msg->hndList;
  
  /* 18-Oct-91: fedor: blah! central handlers should
     have a central resource as well - need to merge this idea
     with hndSelect/chooseMsgHanlder in recvMsg */
  
  flag = 0;
  hnd = (HND_PTR)x_ipc_listFirst(hndList);
  while (hnd && !flag) {
    if (hnd->hndOrg == GET_S_GLOBAL(x_ipcServerModGlobal) && hnd->hndProc) 
      flag = 1;
    else if (hnd->resource && hnd->resource->capacity)
      flag = 1;
    hnd = (HND_PTR)x_ipc_listNext(hndList);
  }
  
  if (!flag) {
    LOG_MESSAGE1("\nWARNING: Tap Message Ignored: %s No Handler.\n",
		msg->msgData->name);
  }
  
  return flag;
}


/******************************************************************************
 *
 * FUNCTION: DISPATCH_PTR createListeningDispatch (tappedDispatch, dataMsg, 
 *                                                 tap)
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 * DISPATCH_PTR tappedDispatch; 
 * DATA_MSG_PTR dataMsg; 
 * TAP_PTR tap;
 *
 * OUTPUTS: DISPATCH_PTR
 *
 *****************************************************************************/
static DISPATCH_PTR createListeningDispatch (DISPATCH_PTR tappedDispatch,
					     DATA_MSG_PTR dataMsg, TAP_PTR tap)
{
  DISPATCH_PTR listeningDispatch;
  X_IPC_MSG_CLASS_TYPE listeningClass;
#ifndef NMP_IPC
  TASK_TREE_NODE_PTR listeningNode, tappedNode;
#endif
  
  if (!tapOK(tap))
    return NULL;
  
  listeningClass = tap->listeningMsg->msgData->msg_class;
  listeningDispatch = buildDispatchInternal(tap->listeningMsg, dataMsg, 
					    tapClassData(listeningClass),
					    (DISPATCH_PTR)NULL, NULL, (char *)NULL);
#ifndef NMP_IPC
  if (taskTreeClass(listeningDispatch->msg_class)) {
    Initialize_TaskTreeNode(listeningDispatch, NO_REF);
    if (!GET_S_GLOBAL(tapsUnderRoot) && (tappedDispatch->treeNode != NULL)) {
      if (tappedDispatch->treeNode != NULL) {
	/* Move message from under root node to under the tapped message's node
	   (but don't add temporal constraints or alter the "lastChild" */
	listeningNode = listeningDispatch->treeNode;
	tappedNode = tappedDispatch->treeNode;
	x_ipc_listDeleteItem((void *)listeningNode, listeningNode->parent->children);
	if (!tappedNode->children)
	  tappedNode->children = x_ipc_listCreate();
	x_ipc_listInsertItem((void *)listeningNode, tappedNode->children);
	listeningNode->parent = tappedNode;
	DISPATCH_PARENT_REF(listeningDispatch) = tappedDispatch->refId;
      }
    }
  } else if (!GET_S_GLOBAL(tapsUnderRoot)) {
    DISPATCH_PARENT_REF(listeningDispatch) = tappedDispatch->refId;
  }
#endif

  addTapInfoToDispatch(listeningDispatch, FALSE);
  listeningDispatch->tapInfo->tappedDispatch = tappedDispatch;
  return listeningDispatch;
}

/******************************************************************************
 *
 * FUNCTION: void sendListeningDispatch (tappedDispatch, dataMsg, tap)
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 * DISPATCH_PTR tappedDispatch; 
 * DATA_MSG_PTR dataMsg; 
 * TAP_PTR tap;
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void sendListeningDispatch (DISPATCH_PTR tappedDispatch,
				   DATA_MSG_PTR dataMsg, TAP_PTR tap)
{
  DISPATCH_PTR listeningDispatch;
  MSG_PTR listeningMsg;
  
  listeningDispatch = createListeningDispatch(tappedDispatch, dataMsg, tap);
  
  if (listeningDispatch) {
    listeningMsg = DISPATCH_MSG(listeningDispatch);
    recvMessage(listeningDispatch, listeningMsg->msgData->msg_class,
		listeningDispatch->classData);
  }
}

/******************************************************************************
 *
 * FUNCTION: void sendListeningDispatchAndConstrain(tappedDispatch, tap, 
 *                    tappedTimePoint, relationship, listeningTimePoint)
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 * DISPATCH_PTR tappedDispatch; 
 * TAP_PTR tap;
 * X_IPC_TIME_POINT_PTR tappedTimePoint, listeningTimePoint; 
 * char *relationship;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
#ifndef NMP_IPC
static void sendListeningDispatchAndConstrain 
(DISPATCH_PTR tappedDispatch, TAP_PTR tap,
 X_IPC_TIME_POINT_PTR tappedTimePoint,
 char *relationship, X_IPC_TIME_POINT_PTR listeningTimePoint)
{
  DISPATCH_PTR listeningDispatch;
  MSG_PTR listeningMsg;
  DATA_MSG_PTR tappedMsgData;
  QUANTITY_PTR tappedQuantity, listeningQuantity;
  
  tappedMsgData = DISPATCH_MSG_DATA(tappedDispatch);
  
  listeningDispatch = createListeningDispatch(tappedDispatch, tappedMsgData, 
					      tap);
  if (listeningDispatch) {
    listeningMsg = DISPATCH_MSG(listeningDispatch);
    
    if (taskTreeClass(listeningDispatch->msg_class)) {
      tappedTimePoint->interval.msgRef = tappedDispatch->locId;
      tappedQuantity = Time_Of(tappedTimePoint);
      listeningTimePoint->interval.msgRef = listeningDispatch->locId;
      listeningQuantity = Time_Of(listeningTimePoint);
      QAssert(tappedQuantity, relationship, listeningQuantity,
	      "Tap Constraint");
    }
    recvMessage(listeningDispatch, listeningMsg->msgData->msg_class, 
		listeningDispatch->classData);
  }
}
#endif

/******************************************************************************
 *
 * FUNCTION: int32 sendOnCondition(condition, dispatch, tap)
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 * TAP_CONDITION_TYPE condition; 
 * DISPATCH_PTR dispatch; 
 * TAP_PTR tap; 
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

static int32 sendOnCondition(TAP_CONDITION_TYPE condition, DISPATCH_PTR dispatch,
			   TAP_PTR tap)
{
  if (tap->condition == condition) {
    sendListeningDispatch(dispatch, DISPATCH_MSG_DATA(dispatch), tap);
  }
  return TRUE;
}


/******************************************************************************
 *
 * FUNCTION: void tapOnCondition(sendOnConditionFn, dispatch)
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 * int32 (*sendOnConditionFn)(); 
 * DISPATCH_PTR dispatch;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

static void tapOnCondition(LIST_ITER_FN sendOnConditionFn, 
			   DISPATCH_PTR dispatch)
{
  LIST_PTR tapsList, extraTapsList;
  
  tapsList = DISPATCH_TAPS(dispatch);
  
  if (tapsList) {
    (void)x_ipc_listIterateFromFirst((LIST_ITER_FN)sendOnConditionFn, 
			       (char *)dispatch, tapsList);
  }
  
  extraTapsList = DISPATCH_EXTRA_TAPS(dispatch);
  
  if (extraTapsList) {
    (void)x_ipc_listIterateFromFirst((LIST_ITER_FN)sendOnConditionFn, 
			       (char *)dispatch, extraTapsList);
  }
}


/******************************************************************************
 *
 * FUNCTION: void tapActivation(ruleName, relationshipNode, activationFn)
 * 
 * DESCRIPTION: 
 *
 * INPUTS: 
 * const char *ruleName;
 * TMS_NODE_PTR relationshipNode;
 * int32 (*activationFn)();
 *
 * OUTPUTS: void
 *
 *****************************************************************************/
#ifndef NMP_IPC
static void removeTapRule(NODE_RULE_FN activationFn, CELL_PTR relation)
{
  RULE_PTR rule;
  
  rule = (RULE_PTR)x_ipc_listFirst(relation->outRules);
  while (rule) {
    if ((rule->fn.node_rule == (NODE_RULE_FN) tapActivation) &&
        (rule->parameters == (char *)activationFn)) {
      x_ipc_listDeleteItem((char *)rule, relation->outRules);
      return;
    }
    rule = (RULE_PTR)x_ipc_listNext(relation->outRules);
  }
}

/*ARGSUSED*/
static void tapActivation(const char *ruleName, TMS_NODE_PTR relationshipNode,
			  LIST_ITER_FN activationFn)
{
#ifdef UNUSED_PRAGMA
#pragma unused(ruleName)
#endif
  DISPATCH_PTR dispatch;
  QUANTITY_PTR quantity;
  X_IPC_TIME_POINT_PTR timePoint;
  
  if (Less_Than_Now_Relp(relationshipNode) && 
      Relp_Still_Out(relationshipNode)) {
    /* Remove the tapActivation rule to avoid having it fire again */
    removeTapRule((NODE_RULE_FN)activationFn, relationshipNode->cell);
    
    quantity = relation_other_arg(relationshipNode->cell, GET_S_GLOBAL(Now));
    timePoint = (X_IPC_TIME_POINT_PTR)(quantity->name);
    dispatch = DISPATCH_FROM_ID(timePoint->interval.msgRef);
    tapOnCondition(activationFn, dispatch);
  }
}
#endif

/****************************************************************************/

static int32 sendWhenSentTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(WhenSent, dispatch, tap); 
}

/****************************************************************************/

void tapWhenSent(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendWhenSentTap, dispatch); 
}

/****************************************************************************/

static int32 sendBeforeHandlingTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(BeforeHandling, dispatch, tap); 
}

/****************************************************************************/

void tapBeforeHandling(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendBeforeHandlingTap, dispatch); 
}

/****************************************************************************/

static int32 sendWhileHandlingTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(WhileHandling, dispatch, tap); 
}

/****************************************************************************/

void tapWhileHandling(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendWhileHandlingTap, dispatch); 
}

/****************************************************************************/

static int32 sendAfterHandledTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(AfterHandled, dispatch, tap);
}

/****************************************************************************/

void tapAfterHandled(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendAfterHandledTap, dispatch); 
}

#ifndef NMP_IPC
/****************************************************************************/

static int32 sendAfterSuccessTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(AfterSuccess, dispatch, tap);
}

/****************************************************************************/

void tapAfterSuccess(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendAfterSuccessTap, dispatch); 
}

/****************************************************************************/

static int32 sendWhenSuccess(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  if (tap->condition == WhenSuccess) {
    sendListeningDispatchAndConstrain(dispatch, tap, 
				      &(GET_S_GLOBAL(endAchievement)),
				      ">=",
				      &(GET_S_GLOBAL(endAchievement)));
  }
  return TRUE;
}

/****************************************************************************/

void tapWhenSuccess(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendWhenSuccess, dispatch); 
}
/****************************************************************************/

static int32 sendAfterFailureTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  ;  return sendOnCondition(AfterFailure, dispatch, tap); 
}

/****************************************************************************/

void tapAfterFailure(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendAfterFailureTap, dispatch); 
}
/****************************************************************************/

static int32 sendWhenFailure(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  if (tap->condition == WhenFailure) {
    sendListeningDispatchAndConstrain(dispatch, tap, 
				      &(GET_S_GLOBAL(endAchievement)),
				      ">=", 
				      &(GET_S_GLOBAL(endAchievement)));
  }
  return TRUE;
}

/****************************************************************************/

void tapWhenFailure(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendWhenFailure, dispatch); 
}
#endif
/****************************************************************************/

static int32 sendAfterRepliedTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  if (tap->condition == AfterReplied) {
    sendListeningDispatch(dispatch, DISPATCH_RES_DATA(dispatch), tap);
  }
  return TRUE;
}

/****************************************************************************/

void tapAfterReplied(DISPATCH_PTR dispatch)
{ 
  tapOnCondition((LIST_ITER_FN) sendAfterRepliedTap, dispatch); 
}

/****************************************************************************/
#ifndef NMP_IPC
static int32 sendBeforeHandlingTapAndConstrain(DISPATCH_PTR dispatch, 
					     TAP_PTR tap)
{
  if (tap->condition == BeforeHandling) {
    sendListeningDispatchAndConstrain(dispatch, tap,
				      &(GET_S_GLOBAL(startHandling)), ">=",
				      &(GET_S_GLOBAL(endHandling)));
  }
  return TRUE;
}

/****************************************************************************/

/*****************************************************************
 * Return FALSE if the rule is added (to halt the list iteration)
 ****************************************************************/
static int32 addBeforeHandlingRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  if (tap->condition == BeforeHandling) {
    addOutRule(GET_S_GLOBAL(beforeHandlingRule),
	       relationGetOrMake(GET_S_GLOBAL(Now), 
				 dispatch->treeNode->handlingInterval->start));
    return FALSE;
  } else {
    return TRUE;
  }
}

/****************************************************************************/

static int32 sendBeforeAchievingTapAndConstrain(DISPATCH_PTR dispatch,
					      TAP_PTR tap)
{
  if (tap->condition == BeforeAchieving) {
    sendListeningDispatchAndConstrain(dispatch, tap, 
				      &(GET_S_GLOBAL(startAchievement)), ">=",
				      &(GET_S_GLOBAL(endAchievement)));
  }
  return TRUE;
}

/****************************************************************************/

static int32 sendWhileAchievingTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(WhileAchieving, dispatch, tap); 
}

/****************************************************************************/

static int32 sendWhenAchievedTapAndConstrain(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  if (tap->condition == WhenAchieved) {
    sendListeningDispatchAndConstrain(dispatch, tap, 
				      &(GET_S_GLOBAL(endAchievement)), ">=",
				      &(GET_S_GLOBAL(endAchievement)));
  }
  return TRUE;
}

/****************************************************************************/

static int32 sendAfterAchievedTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(AfterAchieved, dispatch, tap); 
}

/****************************************************************************/

/*****************************************************************
 * Return FALSE if the rule is added (to halt the list iteration)
 ****************************************************************/

static int32 addBeforeAchievingRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR startAchievementQty;
  
  if (tap->condition == BeforeAchieving) {
    startAchievementQty = dispatch->treeNode->achievementInterval->start;
    if (QIs_True(GET_S_GLOBAL(Now), "<", startAchievementQty)) {
      addOutRule(GET_S_GLOBAL(beforeAchievingRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), 
				   startAchievementQty));
      return FALSE;
    } else {
      (void)sendBeforeAchievingTapAndConstrain(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 addWhileAchievingRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR startAchievementQty;
  
  if (tap->condition == WhileAchieving) {
    startAchievementQty = dispatch->treeNode->achievementInterval->start;
    if (QIs_True(GET_S_GLOBAL(Now), "<", startAchievementQty)) {
      addOutRule(GET_S_GLOBAL(whileAchievingRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), 
				   startAchievementQty));
      return FALSE;
    } else {
      (void)sendWhileAchievingTap(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 addAfterAchievedRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR endAchievementQty;
  
  if (tap->condition == AfterAchieved) {
    endAchievementQty = dispatch->treeNode->achievementInterval->end;
    if (QIs_True(GET_S_GLOBAL(Now), "<", endAchievementQty)) {
      addOutRule(GET_S_GLOBAL(afterAchievedRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), endAchievementQty));
      return FALSE;
    } else {
      (void)sendAfterAchievedTap(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 addWhenAchievedRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR endAchievementQty;
  
  if (tap->condition == WhenAchieved) {
    endAchievementQty = dispatch->treeNode->achievementInterval->end;
    if (QIs_True(GET_S_GLOBAL(Now), "<", endAchievementQty)) {
      addOutRule(GET_S_GLOBAL(whenAchievedRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), endAchievementQty));
      return FALSE;
    } else {
      (void)sendWhenAchievedTapAndConstrain(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 sendBeforePlanningTapAndConstrain(DISPATCH_PTR dispatch,
					     TAP_PTR tap)
{
  if (tap->condition == BeforePlanning) {
    sendListeningDispatchAndConstrain(dispatch, tap,
				      &(GET_S_GLOBAL(startPlanning)), ">=", 
				      &(GET_S_GLOBAL(endPlanning)));
  }
  return TRUE;
}

/****************************************************************************/

static int32 sendWhilePlanningTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(WhilePlanning, dispatch, tap); 
}

/****************************************************************************/

static int32 sendWhenPlannedTapAndConstrain(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  if (tap->condition == WhenPlanned) {
    sendListeningDispatchAndConstrain(dispatch, tap, 
				      &(GET_S_GLOBAL(endPlanning)), ">=",
				      &(GET_S_GLOBAL(endPlanning)));
  }
  return TRUE;
}

/****************************************************************************/

static int32 sendAfterPlannedTap(DISPATCH_PTR dispatch, TAP_PTR tap)
{ 
  return sendOnCondition(AfterPlanned, dispatch, tap); 
}

/****************************************************************************/

/*****************************************************************
 * Return FALSE if the rule is added (to halt the list iteration)
 ****************************************************************/

static int32 addBeforePlanningRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR startPlanningQty;
  
  if (tap->condition == BeforePlanning) {
    startPlanningQty = dispatch->treeNode->planningInterval->start;
    if (QIs_True(GET_S_GLOBAL(Now), "<", startPlanningQty)) {
      addOutRule(GET_S_GLOBAL(beforePlanningRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), 
				   startPlanningQty));
      return FALSE;
    } else {
      (void)sendBeforePlanningTapAndConstrain(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 addWhilePlanningRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR startPlanningQty;
  
  if (tap->condition == WhilePlanning) {
    startPlanningQty = dispatch->treeNode->planningInterval->start;
    if (QIs_True(GET_S_GLOBAL(Now), "<", startPlanningQty)) {
      addOutRule(GET_S_GLOBAL(whilePlanningRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), 
				   startPlanningQty));
      return FALSE;
    } else {
      (void)sendWhilePlanningTap(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 addAfterPlannedRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR endPlanningQty;
  
  if (tap->condition == AfterPlanned) {
    endPlanningQty = dispatch->treeNode->planningInterval->end;
    if (QIs_True(GET_S_GLOBAL(Now), "<", endPlanningQty)) {
      addOutRule(GET_S_GLOBAL(afterPlannedRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), endPlanningQty));
      return FALSE;
    } else {
      (void)sendAfterPlannedTap(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

static int32 addWhenPlannedRule(DISPATCH_PTR dispatch, TAP_PTR tap)
{
  QUANTITY_PTR endPlanningQty;
  
  if (tap->condition == WhenPlanned) {
    endPlanningQty = dispatch->treeNode->planningInterval->end;
    if (QIs_True(GET_S_GLOBAL(Now), "<", endPlanningQty)) {
      addOutRule(GET_S_GLOBAL(whenPlannedRule), 
		 relationGetOrMake(GET_S_GLOBAL(Now), endPlanningQty));
      return FALSE;
    } else {
      (void)sendWhenPlannedTapAndConstrain(dispatch, tap);
    }
  }
  return TRUE;
}

/****************************************************************************/

/*****
 * The dispatch should be a goal, command, or monitor 
 *****/

static void setUpTapRule(LIST_ITER_FN addRuleFn, DISPATCH_PTR dispatch)
{
  /* x_ipc_listIterate returns TRUE if all the elements of the list passed the test.
     The "addRule" functions return TRUE if they are not applicable; Thus,
     do the "extra taps" only if first x_ipc_listIterate returns TRUE (or if the
     list length is zero */
  if (!x_ipc_listLength(DISPATCH_TAPS(dispatch)) ||
      TRUE == x_ipc_listIterateFromFirst(addRuleFn, 
				   (char *)dispatch, DISPATCH_TAPS(dispatch))) {
    (void)x_ipc_listIterateFromFirst((LIST_ITER_FN)addRuleFn, (char *)dispatch, 
			       DISPATCH_EXTRA_TAPS(dispatch));
  }
}

/****************************************************************************/

void setUpBeforeTaps(DISPATCH_PTR dispatch)
{
  setUpTapRule((LIST_ITER_FN) addBeforeHandlingRule, dispatch);
  setUpTapRule((LIST_ITER_FN) addBeforeAchievingRule, dispatch);
  
  if (dispatch->msg_class == GoalClass) 
    setUpTapRule((LIST_ITER_FN) addBeforePlanningRule, dispatch);
}

/****************************************************************************/

void setUpDuringAndAfterTaps(DISPATCH_PTR dispatch)
{
  setUpTapRule((LIST_ITER_FN) addWhileAchievingRule, dispatch);
  setUpTapRule((LIST_ITER_FN) addAfterAchievedRule, dispatch);
  setUpTapRule((LIST_ITER_FN) addWhenAchievedRule, dispatch);
  
  if (dispatch->msg_class == GoalClass) {
    setUpTapRule((LIST_ITER_FN) addWhilePlanningRule, dispatch);
    setUpTapRule((LIST_ITER_FN) addAfterPlannedRule, dispatch);
    setUpTapRule((LIST_ITER_FN) addWhenPlannedRule, dispatch);
  }
}
#endif
/****************************************************************************/

#if 0
/* Not yet used */
static void setUpWhenKilledTaps(DISPATCH_PTR dispatch)
{
  if (DISPATCH_TAPS(dispatch) || DISPATCH_EXTRA_TAPS(dispatch)) {
    LOG_MESSAGE("setUpWhenKilledTaps not yet implemented\n");
  }
}
#endif

/****************************************************************************/
/* Currently not used (RGS: 11/11/92)
   void removeWiretapRules (TASK_TREE_NODE_PTR treeNode)
   {
   CELL_PTR relation;
   
   relation = get_relation(GET_S_GLOBAL(Now), 
   treeNode->handlingInterval->start);
   if (relation) {
   removeTapRule(sendBeforeHandlingTapAndConstrain, relation);
   }
   
   relation = get_relation(GET_S_GLOBAL(Now), 
   treeNode->achievementInterval->start);
   if (relation) {
   removeTapRule(sendBeforeAchievingTapAndConstrain, relation);
   removeTapRule(sendWhileAchievingTap, relation);
   }
   
   relation = get_relation(GET_S_GLOBAL(Now),
   treeNode->achievementInterval->end);
   if (relation) {
   removeTapRule(sendAfterAchievedTap, relation);
   removeTapRule(sendWhenAchievedTapAndConstrain, relation);
   }
   
   relation = get_relation(GET_S_GLOBAL(Now), 
   treeNode->planningInterval->start);
   if (relation) {
   removeTapRule(sendBeforePlanningTapAndConstrain, relation);
   removeTapRule(sendWhilePlanningTap, relation);
   }
   
   relation = get_relation(GET_S_GLOBAL(Now), treeNode->planningInterval->end);
   if (relation) {
   removeTapRule(sendAfterPlannedTap, relation);
   removeTapRule(sendWhenPlannedTapAndConstrain, relation);
   }
   }
   */

/******************************************************************************
 *
 * FUNCTION: void tapInitialize()
 * 
 * DESCRIPTION: Register all the user-interface functions for message tapping
 *
 * INPUTS: none
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void tapInitialize(void)
{
  centralRegisterInform(X_IPC_TAP_MSG_INFORM,
			X_IPC_TAP_MSG_INFORM_FORM,
			tapMessageHandler);
  Add_Message_To_Ignore(X_IPC_TAP_MSG_INFORM);
  
  centralRegisterInform(X_IPC_TAP_MSG_INFORM_OLD,
			X_IPC_TAP_MSG_INFORM_FORM,
			tapMessageHandler);
  Add_Message_To_Ignore(X_IPC_TAP_MSG_INFORM_OLD);
  
  centralRegisterInform(X_IPC_TAP_REF_INFORM,
			X_IPC_TAP_REF_INFORM_FORM,
			tapReferenceHandler);
  Add_Message_To_Ignore(X_IPC_TAP_REF_INFORM);
  
  centralRegisterInform(X_IPC_TAP_REF_INFORM_OLD,
			X_IPC_TAP_REF_INFORM_FORM,
			tapReferenceHandler);
  Add_Message_To_Ignore(X_IPC_TAP_REF_INFORM_OLD);
  
  centralRegisterInform(X_IPC_REMOVE_TAP_INFORM,
			X_IPC_REMOVE_TAP_INFORM_FORM,
			removeTapHandler);
  Add_Message_To_Ignore(X_IPC_REMOVE_TAP_INFORM);
  
  centralRegisterInform(X_IPC_REMOVE_TAP_INFORM_OLD,
			X_IPC_REMOVE_TAP_INFORM_FORM,
			removeTapHandler);
  Add_Message_To_Ignore(X_IPC_REMOVE_TAP_INFORM_OLD);
  
  centralRegisterQuery(X_IPC_FIND_TAP_INFORM,
		       X_IPC_FIND_TAP_INFORM_FORM,
		       "X_IPC_REF_PTR",
		       findTappedReferenceHandler);
  Add_Message_To_Ignore(X_IPC_FIND_TAP_INFORM);
  
  centralRegisterQuery(X_IPC_FIND_TAP_INFORM_OLD,
		       X_IPC_FIND_TAP_INFORM_FORM,
		       "X_IPC_REF_PTR",
		       findTappedReferenceHandler);
  Add_Message_To_Ignore(X_IPC_FIND_TAP_INFORM_OLD);
  
#ifndef NMP_IPC
  centralRegisterInform(X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM,
			X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM_FORMAT,
			cleanUpHandler);
  Add_Message_To_Ignore(X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM);
  
  centralRegisterInform(X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM_OLD,
			X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM_FORMAT,
			cleanUpHandler);
  Add_Message_To_Ignore(X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM_OLD);
#endif
  
  GET_C_GLOBAL(tappedMsgs) = x_ipc_strListCreate();
  centralRegisterVar(X_IPC_TAPPED_MSG_VAR, X_IPC_STR_LIST_FORMAT);
  centralIgnoreVarLogging(X_IPC_TAPPED_MSG_VAR);
  centralSetVar(X_IPC_TAPPED_MSG_VAR, (char *)GET_C_GLOBAL(tappedMsgs));

  GET_C_GLOBAL(broadcastMsgs) = x_ipc_strListCreate();
  centralRegisterVar(X_IPC_BROADCAST_MSG_VAR, X_IPC_STR_LIST_FORMAT);
  centralIgnoreVarLogging(X_IPC_BROADCAST_MSG_VAR);
  centralSetVar(X_IPC_BROADCAST_MSG_VAR, (char *)GET_C_GLOBAL(broadcastMsgs));

#ifndef NMP_IPC
  GET_S_GLOBAL(startHandling).point_class = StartPoint; 
  GET_S_GLOBAL(startHandling).interval.interval_class = HandlingInterval;
  GET_S_GLOBAL(endHandling).point_class = EndPoint; 
  GET_S_GLOBAL(endHandling).interval.interval_class = HandlingInterval;
  
  GET_S_GLOBAL(startAchievement).point_class = StartPoint; 
  GET_S_GLOBAL(startAchievement).interval.interval_class = AchievementInterval;
  GET_S_GLOBAL(endAchievement).point_class = EndPoint; 
  GET_S_GLOBAL(endAchievement).interval.interval_class = AchievementInterval;
  
  GET_S_GLOBAL(startPlanning).point_class = StartPoint; 
  GET_S_GLOBAL(startPlanning).interval.interval_class = PlanningInterval;
  GET_S_GLOBAL(endPlanning).point_class = EndPoint; 
  GET_S_GLOBAL(endPlanning).interval.interval_class = PlanningInterval;
  
  GET_S_GLOBAL(beforeHandlingRule) = 
    ruleCreateOut("Before Handling",
		  (NODE_RULE_FN) tapActivation, 2,
		  (char *)sendBeforeHandlingTapAndConstrain);
  GET_S_GLOBAL(beforeHandlingRule)->used = 1000; 
  /* Never free, Reid: 10-Jul-91 */
  
  GET_S_GLOBAL(beforeAchievingRule) = 
    ruleCreateOut("Before Achieving",
		  (NODE_RULE_FN) tapActivation, 2,
		  (char *)sendBeforeAchievingTapAndConstrain);
  GET_S_GLOBAL(beforeAchievingRule)->used = 1000; 
  /* Never free, Reid: 10-Jul-91 */
  
  
  GET_S_GLOBAL(whileAchievingRule) = 
    ruleCreateOut("While Achieving",
		  (NODE_RULE_FN) tapActivation, 4,
		  (char *)sendWhileAchievingTap);
  /* Never free, Reid: 10-Jul-91 */
  GET_S_GLOBAL(whileAchievingRule)->used = 1000; 
  
  GET_S_GLOBAL(afterAchievedRule) = 
    ruleCreateOut("After Achieved",
		  (NODE_RULE_FN) tapActivation, 4,
		  (char *)sendAfterAchievedTap);
  /* Never free, Reid: 10-Jul-91 */
  GET_S_GLOBAL(afterAchievedRule)->used = 1000; 
  
  GET_S_GLOBAL(whenAchievedRule) = 
    ruleCreateOut("When Achieved",
		  (NODE_RULE_FN) tapActivation, 1,
		  (char *)sendWhenAchievedTapAndConstrain);
  /* Never free, Reid: 10-Jul-91 */
  GET_S_GLOBAL(whenAchievedRule)->used = 1000;
  
  GET_S_GLOBAL(beforePlanningRule) = 
    ruleCreateOut("Before Planning",
		  (NODE_RULE_FN) tapActivation, 2,
		  (char *)sendBeforePlanningTapAndConstrain);
  /* Never free, Reid: 10-Jul-91 */
  GET_S_GLOBAL(beforePlanningRule)->used = 1000;
  
  GET_S_GLOBAL(whilePlanningRule) = 
    ruleCreateOut("While Planning",
		  (NODE_RULE_FN) tapActivation, 4,
		  (char *)sendWhilePlanningTap);
  /* Never free, Reid: 10-Jul-91 */
  GET_S_GLOBAL(whilePlanningRule)->used = 1000;
  
  GET_S_GLOBAL(afterPlannedRule) = 
    ruleCreateOut("After Planned",
		  (NODE_RULE_FN) tapActivation, 4,
		  (char *)sendAfterPlannedTap);
  /* Never free, Reid: 10-Jul-91 */
  GET_S_GLOBAL(afterPlannedRule)->used = 1000; 
  
  GET_S_GLOBAL(whenPlannedRule) = 
    ruleCreateOut("When Planned", 
		  (NODE_RULE_FN) tapActivation, 1,
		  (char *)sendWhenPlannedTapAndConstrain);
  GET_S_GLOBAL(whenPlannedRule)->used = 1000; /* Never free, Reid: 10-Jul-91 */
  
  /* Used within "cleanUpHandler" */ 
  GET_S_GLOBAL(alwaysTrueNodeGlobal) = tmsCreateNode("Always True");
  tmsAssertNode(GET_S_GLOBAL(alwaysTrueNodeGlobal), "Always True");
#endif
}

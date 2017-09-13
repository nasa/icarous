/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: centralMsg.h
 *
 * ABSTRACT:
 * 
 * Central message definitions.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/centralMsg.h,v $ 
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: centralMsg.h,v $
 * Revision 2.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2001/01/31 17:54:10  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
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
 * Revision 1.1.2.2  1996/10/22 18:49:27  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.1.2.1  1996/10/14 03:54:36  reids
 * For NMP, added prioritized messages (i.e., prioritized pending queues).
 *
 * Revision 1.1  1996/05/09 01:01:14  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:02  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.10  1996/02/06  19:04:11  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.9  1996/01/27  21:52:56  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.8  1995/10/29  18:26:26  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.7  1995/10/25  22:47:56  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.6  1995/10/07  19:06:58  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.5  1995/07/12  04:54:20  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.4  1995/04/07  05:02:48  rich
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
 * Revision 1.3  1995/03/28  01:14:20  rich
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
 * Revision 1.2  1994/05/25  17:32:29  reids
 * Added utilities to limit the number of pending messages
 *
 * Revision 1.1  1994/05/17  23:15:17  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 *
 ****************************************************************/

#ifndef INCcentralMsg
#define INCcentralMsg

#define X_IPC_MSG_INFO_QUERY        "x_ipc_msgInfoMsg"
#define X_IPC_MSG_INFO_QUERY_OLD     "msgInfoMsg"
#define X_IPC_MSG_INFO_QUERY_FORMAT "{string, string}"
#define X_IPC_MSG_INFO_QUERY_REPLY  "*{int, string, int, format, format}"

#define X_IPC_MESSAGES_QUERY        "x_ipc_getMessages"
#define X_IPC_MESSAGES_QUERY_FORMAT NULL
#define X_IPC_MESSAGES_QUERY_REPLY  X_IPC_STR_LIST_FORMAT

#define X_IPC_MESSAGE_INFO_QUERY        "x_ipc_getMessageInfo"
#define X_IPC_MESSAGE_INFO_QUERY_FORMAT "{string}"
#define X_IPC_MESSAGE_INFO_QUERY_REPLY  "{string,string,string,int,string,int}"

#define X_IPC_HND_INFO_QUERY        "x_ipc_hndInfoMsg"
#define X_IPC_HND_INFO_QUERY_OLD    "hndInfoMsg"
#define X_IPC_HND_INFO_QUERY_FORMAT X_IPC_MSG_INFO_QUERY_FORMAT
#define X_IPC_HND_INFO_QUERY_REPLY  X_IPC_MSG_INFO_QUERY_FORMAT

#define X_IPC_CLASS_INFO_QUERY        "x_ipc_classInfoMsg"
#define X_IPC_CLASS_INFO_QUERY_OLD    "classInfoMsg"
#define X_IPC_CLASS_INFO_QUERY_FORMAT "int"
#define X_IPC_CLASS_INFO_QUERY_REPLY  "*{int, format}"

#ifdef NMP_IPC
#define IPC_SET_MSG_PRIORITY_INFORM        "ipc_setMsgPriority"
#define IPC_SET_MSG_PRIORITY_INFORM_FORMAT "{string, int}"
#endif

#define X_IPC_PROVIDES_INFORM        "x_ipc_moduleProvides"
#define X_IPC_PROVIDES_INFORM_FORMAT X_IPC_STR_LIST_FORMAT

#define X_IPC_REQUIRES_INFORM        "x_ipc_moduleRequires"
#define X_IPC_REQUIRES_INFORM_FORMAT X_IPC_STR_LIST_FORMAT

#define X_IPC_AVAILABLE_QUERY        "x_ipc_available"
#define X_IPC_AVAILABLE_QUERY_FORMAT NULL
#define X_IPC_AVAILABLE_QUERY_REPLY  X_IPC_STR_LIST_FORMAT

#define X_IPC_CONNECT_QUERY        "x_ipc_newModuleConnectMsg"
#define X_IPC_CONNECT_QUERY_OLD    "newModuleConnectMsg"
#define X_IPC_CONNECT_QUERY_FORMAT "{string, string}"
#define X_IPC_CONNECT_QUERY_REPLY  "{{int, int},boolean}"

#define X_IPC_REGISTER_MSG_INFORM        "x_ipc_registerMessageMsg"
#define X_IPC_REGISTER_MSG_INFORM_OLD    "registerMessageMsg"
#define X_IPC_REGISTER_MSG_INFORM_FORMAT "{string, int, string, string}"

#define X_IPC_REGISTER_HND_INFORM        "x_ipc_registerHandlerMsg"
#define X_IPC_REGISTER_HND_INFORM_OLD    "registerHandlerMsg"
#define X_IPC_REGISTER_HND_INFORM_FORMAT "{int, string, string}"

#define X_IPC_DEREGISTER_HND_INFORM        "x_ipc_deregisterHandlerMsg"
#define X_IPC_DEREGISTER_HND_INFORM_OLD    "deregisterHandlerMsg"
#define X_IPC_DEREGISTER_HND_INFORM_FORMAT "{int, string, string}"

#define X_IPC_NAMED_FORM_INFORM        "x_ipc_registerNamedFormMsg"
#define X_IPC_NAMED_FORM_INFORM_OLD    "registerNamedFormMsg"
#define X_IPC_NAMED_FORM_INFORM_FORMAT "{string, string}"

#define X_IPC_NAMED_FORM_QUERY        "x_ipc_getNamedFormMsg"
#define X_IPC_NAMED_FORM_QUERY_FORMAT "string"
#define X_IPC_NAMED_FORM_QUERY_REPLY  "format"

#define X_IPC_WAIT_QUERY        "x_ipc_waitMsg"
#define X_IPC_WAIT_QUERY_OLD    "waitMsg"
#define X_IPC_WAIT_QUERY_FORMAT NULL
#define X_IPC_WAIT_QUERY_REPLY  "int"

#define X_IPC_IGNORE_LOGGING_INFORM        "x_ipc_ignoreLogging"
#define X_IPC_IGNORE_LOGGING_INFORM_OLD    "ignoreLogging"
#define X_IPC_IGNORE_LOGGING_INFORM_FORMAT "string"

#define X_IPC_RESUME_LOGGING_INFORM        "x_ipc_resumeLogging"
#define X_IPC_RESUME_LOGGING_INFORM_OLD    "resumeLogging"
#define X_IPC_RESUME_LOGGING_INFORM_FORMAT "string"

#define X_IPC_CLOSE_INFORM        "x_ipc_CloseMsg"
#define X_IPC_CLOSE_INFORM_OLD    "x_ipcCloseMsg"
#define X_IPC_CLOSE_INFORM_FORMAT NULL

#define X_IPC_DIRECT_RES_INFORM        "x_ipc_directResMsg"
#define X_IPC_DIRECT_RES_INFORM_OLD    "directResMsg"
#define X_IPC_DIRECT_RES_INFORM_FORMAT "{int, string}"

#define X_IPC_DIRECT_INFO_QUERY        "x_ipc_directInfoMsg"
#define X_IPC_DIRECT_INFO_QUERY_OLD    "directInfoMsg"
#define X_IPC_DIRECT_INFO_QUERY_FORMAT "string"
#define X_IPC_DIRECT_INFO_QUERY_REPLY  DIRECT_INFO_FORMAT

#define X_IPC_DIRECT_MSG_QUERY        "x_ipc_directMsgQuery"
#define X_IPC_DIRECT_MSG_QUERY_FORMAT "string"
#define X_IPC_DIRECT_MSG_QUERY_REPLY  DIRECT_MSG_FORMAT

#define X_IPC_MAP_NAMED         "map"
#define X_IPC_MAP_NAMED_FORMAT  "fmat"

#define X_IPC_REGISTER_RESOURCE_INFORM        "x_ipc_registerResourceMsg"
#define X_IPC_REGISTER_RESOURCE_INFORM_OLD    "registerResourceMsg"
#define X_IPC_REGISTER_RESOURCE_INFORM_FORMAT "{int, string}"

#define X_IPC_HANDLER_TO_RESOURCE_INFORM        "x_ipc_addHndToResourceMsg"
#define X_IPC_HANDLER_TO_RESOURCE_INFORM_OLD    "addHndToResourceMsg"
#define X_IPC_HANDLER_TO_RESOURCE_INFORM_FORMAT "{string, string}"

#define X_IPC_LIMIT_PENDING_INFORM        "x_ipc_limitPendingMsg"
#define X_IPC_LIMIT_PENDING_INFORM_OLD    "limitPendingMsg"
#define X_IPC_LIMIT_PENDING_INFORM_FORMAT "{string, string, int}"

#define X_IPC_RESERVE_RESOURCE_QUERY        "x_ipc_reserveResourceMsg"
#define X_IPC_RESERVE_RESOURCE_QUERY_OLD    "reserveResourceMsg"
#define X_IPC_RESERVE_RESOURCE_QUERY_FORMAT "string"
#define X_IPC_RESERVE_RESOURCE_QUERY_REPLY  "int"

#define X_IPC_RESERVE_MOD_RESOURCE_QUERY        "x_ipc_reserveModResourceMsg"
#define X_IPC_RESERVE_MOD_RESOURCE_QUERY_OLD    "reserveModResourceMsg"
#define X_IPC_RESERVE_MOD_RESOURCE_QUERY_FORMAT "{string,string}"
#define X_IPC_RESERVE_MOD_RESOURCE_QUERY_REPLY  "int"

#define X_IPC_CANCEL_RESOURCE_INFORM        "x_ipc_cancelReservationMsg"
#define X_IPC_CANCEL_RESOURCE_INFORM_OLD    "cancelReservationMsg"
#define X_IPC_CANCEL_RESOURCE_INFORM_FORMAT "int"

#define X_IPC_LOCK_RESOURCE_QUERY        "x_ipc_lockResourceMsg"
#define X_IPC_LOCK_RESOURCE_QUERY_OLD    "lockResourceMsg"
#define X_IPC_LOCK_RESOURCE_QUERY_FORMAT "string"
#define X_IPC_LOCK_RESOURCE_QUERY_REPLY  "int"

#define X_IPC_LOCK_MOD_RESOURCE_QUERY        "x_ipc_lockModResourceMsg"
#define X_IPC_LOCK_MOD_RESOURCE_QUERY_OLD    "lockModResourceMsg"
#define X_IPC_LOCK_MOD_RESOURCE_QUERY_FORMAT "{string, string}"
#define X_IPC_LOCK_MOD_RESOURCE_QUERY_REPLY  "int"

#define X_IPC_UNLOCK_RESOURCE_INFORM        "x_ipc_unlockResourceMsg"
#define X_IPC_UNLOCK_RESOURCE_INFORM_OLD    "unlockResourceMsg"
#define X_IPC_UNLOCK_RESOURCE_INFORM_FORMAT "int"

#define X_IPC_REG_MONITOR_INFORM        "x_ipc_regMonitorMsg"
#define X_IPC_REG_MONITOR_INFORM_OLD        "regMonitorMsg"
#define X_IPC_REG_MONITOR_INFORM_FORMAT "{int, string, string, string, string}"

#define X_IPC_DELAY_CMD        "x_ipc_DelayCommand"
#define X_IPC_DELAY_CMD_OLD    "x_ipcDelayCommand"
#define X_IPC_DELAY_CMD_FORMAT "int"

#define X_IPC_TAP_REF_INFORM     "x_ipc_tapReference"
#define X_IPC_TAP_REF_INFORM_OLD "tapReference"
#define X_IPC_TAP_REF_INFORM_FORM "{int, X_IPC_REF_PTR, string}"

#define X_IPC_TAP_MSG_INFORM      "x_ipc_tapMessage"
#define X_IPC_TAP_MSG_INFORM_OLD  "tapMessage"
#define X_IPC_TAP_MSG_INFORM_FORM "{int, string, string}"

#define X_IPC_REMOVE_TAP_INFORM     "x_ipc_removeTap"
#define X_IPC_REMOVE_TAP_INFORM_OLD "removeTap"
#define X_IPC_REMOVE_TAP_INFORM_FORM X_IPC_TAP_MSG_INFORM_FORM

#define X_IPC_FIND_TAP_INFORM      "x_ipc_findTappedReference"
#define X_IPC_FIND_TAP_INFORM_OLD  "findTappedReference"
#define X_IPC_FIND_TAP_INFORM_FORM "int"

#define X_IPC_CONTRAINT_INFORM        "x_ipc_tplConstrain"
#define X_IPC_CONTRAINT_INFORM_OLD    "tplConstrain"
#define X_IPC_CONTRAINT_INFORM_FORMAT "{TimePoint, string, TimePoint}"

#define X_IPC_REGISTER_VAR_INFORM        "x_ipc_registerGlobalVar"
#define X_IPC_REGISTER_VAR_INFORM_OLD    "registerGlobalVar"
#define X_IPC_REGISTER_VAR_INFORM_FORMAT "{string,string}"

/* Task Tree Messages */

#define X_IPC_CREATE_REF_QUERY        "x_ipc_createRefMsg"
#define X_IPC_CREATE_REF_QUERY_OLD    "createRefMsg"
#define X_IPC_CREATE_REF_QUERY_FORMAT "int"
#define X_IPC_CREATE_REF_QUERY_REPLY  "int"

#define X_IPC_ADD_CHILD_QUERY        "x_ipc_addChildRef"
#define X_IPC_ADD_CHILD_QUERY_OLD    "addChildRef"
#define X_IPC_ADD_CHILD_QUERY_FORMAT "{int, int}"
#define X_IPC_ADD_CHILD_QUERY_REPLY  "int"

#define X_IPC_FIND_PARENT_QUERY        "x_ipc_findParentRef"
#define X_IPC_FIND_PARENT_QUERY_OLD    "findParentRef"
#define X_IPC_FIND_PARENT_QUERY_FORMAT "int"
#define X_IPC_FIND_PARENT_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_FIRST_CHILD_QUERY        "x_ipc_findFirstChild"
#define X_IPC_FIRST_CHILD_QUERY_OLD    "findFirstChild"
#define X_IPC_FIRST_CHILD_QUERY_FORMAT "int"
#define X_IPC_FIRST_CHILD_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_LAST_CHILD_QUERY        "x_ipc_findLastChild"
#define X_IPC_LAST_CHILD_QUERY_OLD    "findLastChild"
#define X_IPC_LAST_CHILD_QUERY_FORMAT "int"
#define X_IPC_LAST_CHILD_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_NEXT_CHILD_QUERY        "x_ipc_findNextChild"
#define X_IPC_NEXT_CHILD_QUERY_OLD    "findNextChild"
#define X_IPC_NEXT_CHILD_QUERY_FORMAT "int"
#define X_IPC_NEXT_CHILD_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_PREV_CHILD_QUERY        "x_ipc_findPrevChild"
#define X_IPC_PREV_CHILD_QUERY_OLD    "findPrevChild"
#define X_IPC_PREV_CHILD_QUERY_FORMAT "int"
#define X_IPC_PREV_CHILD_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_FAILED_CHILD_QUERY        "x_ipc_findFailedRef"
#define X_IPC_FAILED_CHILD_QUERY_OLD    "findFailedRef"
#define X_IPC_FAILED_CHILD_QUERY_FORMAT "int"
#define X_IPC_FAILED_CHILD_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_FIND_TOPLEVEL_QUERY        "x_ipc_findTopLevelRef"
#define X_IPC_FIND_TOPLEVEL_QUERY_OLD    "findTopLevelRef"
#define X_IPC_FIND_TOPLEVEL_QUERY_FORMAT "int"
#define X_IPC_FIND_TOPLEVEL_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_ANTE_BY_NAME_QUERY        "x_ipc_findAnteRefByName"
#define X_IPC_ANTE_BY_NAME_QUERY_OLD    "findAnteRefByName"
#define X_IPC_ANTE_BY_NAME_QUERY_FORMAT "{int, string}"
#define X_IPC_ANTE_BY_NAME_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_CHILD_BY_NAME_QUERY        "x_ipc_findChildByName"
#define X_IPC_CHILD_BY_NAME_QUERY_OLD    "findChildByName"
#define X_IPC_CHILD_BY_NAME_QUERY_FORMAT "{int, string}"
#define X_IPC_CHILD_BY_NAME_QUERY_REPLY  "X_IPC_REF_PTR"

#define X_IPC_KILL_TREE_INFORM        "x_ipc_killTaskTree"
#define X_IPC_KILL_TREE_INFORM_OLD    "killTaskTree"
#define X_IPC_KILL_TREE_INFORM_FORMAT "int"

#define X_IPC_KILL_SUBTREE_INFORM        "x_ipc_killSubTaskTree"
#define X_IPC_KILL_SUBTREE_INFORM_OLD    "killSubTaskTree"
#define X_IPC_KILL_SUBTREE_INFORM_FORMAT "int"

#define X_IPC_DISPLAY_TREE_INFORM        "x_ipc_displayTaskTree"
#define X_IPC_DISPLAY_TREE_INFORM_OLD    "displayTaskTree"
#define X_IPC_DISPLAY_TREE_INFORM_FORMAT "int"

#define X_IPC_REF_DATA_QUERY        "x_ipc_referenceData"
#define X_IPC_REF_DATA_QUERY_OLD    "referenceData"
#define X_IPC_REF_DATA_QUERY_FORMAT "int"
#define X_IPC_REF_DATA_QUERY_REPLY  NULL

#define X_IPC_REF_RELEASE_INFORM        "x_ipc_referenceRelease"
#define X_IPC_REF_RELEASE_INFORM_OLD    "referenceRelease"
#define X_IPC_REF_RELEASE_INFORM_FORMAT "int"

#define X_IPC_REF_STATUS_QUERY        "x_ipc_referenceStatusMsg"
#define X_IPC_REF_STATUS_QUERY_OLD    "referenceStatusMsg"
#define X_IPC_REF_STATUS_QUERY_FORMAT "int"
#define X_IPC_REF_STATUS_QUERY_REPLY  "int"

/* Define constants for central varibles. */

#define X_IPC_TERMINAL_LOG_VAR  "CENTRAL_TERMINAL_LOG_VAR"
#define X_IPC_FILE_LOG_VAR  "CENTRAL_FILE_LOG_VAR"
/*#define X_IPC_LOG_VAR_FORMAT "{FILE,int,int,int,int,int,int,int,int,int,int,int}"*/
#define X_IPC_LOG_VAR_FORMAT "{*int,int,int,int,int,int,int,int,int,int,int,int}"

#define X_IPC_CENTRAL_MODULE_DATA_VAR "CENTRAL_MOD_VAR"

#define X_IPC_MOD_DATA_NAME "X_IPC_MOD_DATA"
#define X_IPC_MOD_DATA_FORMAT "{string,string}"

#define X_IPC_LIST_NAME "X_IPC_LIST"
#define X_IPC_LIST_FORMAT "*int"

#define X_IPC_RESOURCE_STATUS_NAME "X_IPC_RESOURCE_STATUS"
#define X_IPC_RESOURCE_STATUS_FORMAT "int"
#define X_IPC_RESOURCE_FORMAT "int,string,int,*int,RESOURCE_STATUS,X_IPC_LIST,X_IPC_LIST,int,LIST_PTR"
#define X_IPC_MODULE_DATA_VAR_FORMAT "int,int,int,int,*MOD_DATA_TYPE,*_RESOURCE,*_LIST,*_LIST"

#define X_IPC_ADD_EXCEP_INFORM        "x_ipc_addExcepMsg"
#define X_IPC_ADD_EXCEP_INFORM_OLD    "addExcepMsg"
#define X_IPC_ADD_EXCEP_INFORM_FORMAT "{X_IPC_REF_PTR, string}"

#define X_IPC_ADD_EXCEPTIONS_INFORM     "x_ipc_addExceptions"
#define X_IPC_ADD_EXCEPTIONS_INFORM_OLD "addExceptions"
#define X_IPC_ADD_EXCEPTIONS_INFORM_FORMAT "{string, string}"

#define X_IPC_RETRY_EXCEP_INFORM     "x_ipc_retryExcepMsg"
#define X_IPC_RETRY_EXCEP_INFORM_OLD "retryExcepMsg"
#define X_IPC_RETRY_EXCEP_INFORM_FORMAT "X_IPC_REF_PTR"

#define X_IPC_BYPASS_EXCEP_INFORM     "x_ipc_byPassExcepMsg"
#define X_IPC_BYPASS_EXCEP_INFORM_OLD "byPassExcepMsg"
#define X_IPC_BYPASS_EXCEP_INFORM_FORMAT "X_IPC_REF_PTR"

#define X_IPC_GET_INFO_EXCEP_QUERY      "x_ipc_getInfoExcepMsg"
#define X_IPC_GET_INFO_EXCEP_QUERY_OLD  "getInfoExcepMsg"
#define X_IPC_GET_INFO_EXCEP_QUERY_FORMAT "{X_IPC_REF_PTR, X_IPC_REF_PTR, int}"

#define X_IPC_KILL_TASK_TREE_EXCEP     "x_ipc_killTaskTreeException"
#define X_IPC_KILL_TASK_TREE_EXCEP_OLD "killTaskTreeException"
#define X_IPC_KILL_TASK_TREE_EXCEP_FORMAT (char *)NULL

#define X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM     "x_ipc_cleanUpAfterAchieved"
#define X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM_OLD "cleanUpAfterAchieved"
#define X_IPC_CLEANUP_AFTER_ACHIEVED_INFORM_FORMAT (char *)NULL

#define IPC_CONNECT_NOTIFY_MSG		"x_ipc_connectNotify"
#define IPC_CONNECT_NOTIFY_FORMAT	"string"

#define IPC_DISCONNECT_NOTIFY_MSG	"x_ipc_disconnectNotify"
#define IPC_DISCONNECT_NOTIFY_FORMAT	"string"

#define IPC_MODULE_CONNECTED_QUERY	  "x_ipc_moduleConnectedQ"
#define IPC_MODULE_CONNECTED_QUERY_FORMAT "string"
#define IPC_MODULE_CONNECTED_QUERY_REPLY  "int"

#define IPC_HANDLER_CHANGE_NOTIFY_MSG	  "x_ipc_notifyHandlerChange"
#define IPC_HANDLER_CHANGE_NOTIFY_FORMAT  "string"

#endif /* INCcentralMsg */

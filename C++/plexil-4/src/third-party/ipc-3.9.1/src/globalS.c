/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: burying globals under a rock!
 *
 * FILE: global.c
 *
 * ABSTRACT: define globals and support routines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: globalS.c,v $
 * Revision 2.5  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.4  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2000/07/27 16:59:10  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
 *
 * Revision 2.2  2000/07/03 17:03:24  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.9  1997/03/07 17:49:40  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.2.2.8  1997/01/27 20:09:30  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.6  1997/01/16 22:17:43  reids
 * Put "display" option back in.
 *
 * Revision 1.2.2.5  1996/12/18 15:10:29  reids
 * Fixed bug to enable multiple connects and disconnects (by not calling
 *   taskVarAdd more than once per task)
 * Defined common macros to clean up code
 *
 * Revision 1.2.2.4  1996/10/29 16:44:24  reids
 * Removed a compiler warning.
 *
 * Revision 1.2.2.3  1996/10/24 15:19:19  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.2.2.2  1996/10/18 18:05:10  reids
 * Moved state var from server to module.
 *
 * Revision 1.2.2.1  1996/10/02 20:58:33  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.3  1996/09/06 22:30:27  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.2  1996/05/24 16:45:53  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:26  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:30  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.32  1996/07/25  22:24:26  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.31  1996/05/09  18:31:00  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.30  1996/02/06  19:04:47  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.29  1996/01/30  15:04:16  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.28  1996/01/23  00:06:32  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.27  1996/01/05  16:31:23  rich
 * Added windows NT port.
 *
 * Revision 1.26  1995/12/17  20:21:28  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.25  1995/10/25  22:48:26  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.24  1995/07/25  20:09:03  rich
 * Changes for NeXT machine (3.0).
 *
 * Revision 1.23  1995/07/10  16:17:29  rich
 * Interm save.
 *
 * Revision 1.22  1995/07/06  21:16:29  rich
 * Solaris and Linux changes.
 *
 * Revision 1.21  1995/04/07  05:03:14  rich
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
 * Revision 1.20  1995/04/05  19:10:51  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.19  1995/04/04  19:42:16  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.18  1995/03/28  01:14:36  rich
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
 * Revision 1.17  1995/01/25  00:01:17  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.16  1995/01/18  22:40:40  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.15  1994/11/02  21:34:22  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.14  1994/10/27  14:43:11  reids
 * Change the default handling of wiretap messages.  Now the default is to
 * place them as children of the tapped message; the "-t" option changes the
 * default to place them all under the task tree root node.
 *
 * Revision 1.13  1994/05/25  04:57:25  rich
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
 * Revision 1.12  1994/05/17  23:15:56  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.11  1994/04/28  22:16:50  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 * Revision 1.10  1994/04/28  16:16:13  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.9  1994/04/16  19:42:19  rich
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
 * Revision 1.8  1994/03/28  02:22:56  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.7  1994/03/27  22:50:30  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.6  1994/01/31  18:27:53  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.5  1993/12/14  17:33:47  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.4  1993/11/21  20:17:58  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/10/21  16:13:55  rich
 * Fixed compiler warnings.
 *
 * Revision 1.2  1993/08/30  21:53:37  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:23  rich
 * Importing x_ipc version 8
 *
 * Revision 8.1  1993/05/20  00:30:01  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:57  fedor
 * Added Logging.
 *
 * $Revision: 2.5 $
 * $Date: 2009/05/04 19:03:41 $
 * $Author: reids $
 *
 *
 *****************************************************************************/

#include "globalS.h"

#ifndef VXWORKS
struct timeval timeBlock;
#endif

/*****************************/

#undef TRUE
#define TRUE  1
#undef FALSE
#define FALSE 0

#if defined(VXWORKS)
/* VX works needs to access the globals through a pointer. */
GS_PTR gS = NULL;

#else

/* In general, the globals can just be statically allocated. */
GS_PTR gS_ptr = NULL;
GS_TYPE gS;

#endif

void globalSInit(void) 
{
  int32 i;
  
#if defined(VXWORKS)
  if (gS == NULL) {
    if (taskVarAdd(0, (int *)&gS) != OK) {
      printErr("taskVarAdd failed\n");
    }
  }
#endif  
  
  if (sGlobalp() && GET_S_GLOBAL(x_ipcServerModGlobal)) {
    SHUTDOWN_SOCKET(GET_S_GLOBAL(x_ipcServerModGlobal)->readSd);
  } else {
#if defined(VXWORKS)
    gS = (GS_TYPE *)x_ipcMalloc(sizeof(GS_TYPE));
    bzero((void *)gS,sizeof(GS_TYPE));
#else
    gS_ptr = &gS;
#endif  
  }
  
  GET_S_GLOBAL(Log_File) = NULL;
  GET_S_GLOBAL(avgSize) = 0;/***/
  GET_S_GLOBAL(avgTime) = 0;/***/
#ifndef NMP_IPC
  GET_S_GLOBAL(Default_Finite_Lower_Qbound) = NULL;/***/
  GET_S_GLOBAL(Default_Finite_Upper_Qbound) = NULL;/***/
  GET_S_GLOBAL(Default_Lower_Qbound) = NULL;/***/
  GET_S_GLOBAL(Default_Upper_Qbound) = NULL;/***/
  GET_S_GLOBAL(FO_Queue) = NULL;
  GET_S_GLOBAL(In_Queue) = NULL;
  GET_S_GLOBAL(Lower_Bound_Propagation_Rule) = NULL;
  GET_S_GLOBAL(MINUS_INFINITY) = -1.0e23;
  GET_S_GLOBAL(Minus_One_Qbound) = NULL;/***/
  GET_S_GLOBAL(Minus_One_Quant) = NULL;/***/
  GET_S_GLOBAL(Node_Counter) = 0;
  GET_S_GLOBAL(Now) = NULL;/***/
  GET_S_GLOBAL(One_Qbound) = NULL;/***/
  GET_S_GLOBAL(One_Quant) = NULL;/***/
  GET_S_GLOBAL(Out_Stack) = NULL;/***/
  GET_S_GLOBAL(PLUS_INFINITY) = 1.0e23;
  GET_S_GLOBAL(Prop_Epsilon) = 0.03;
  GET_S_GLOBAL(Qlat_Epsilon) = 0.001;
  GET_S_GLOBAL(Relation_Hash_Table) = NULL;
  GET_S_GLOBAL(Relation_Propagation_Rule) = NULL;
  GET_S_GLOBAL(Repropagate_Lower_Bound_Rule) = NULL;
  GET_S_GLOBAL(Repropagate_Upper_Bound_Rule) = NULL;
  GET_S_GLOBAL(Search_Memory) = NULL;
  /* GET_S_GLOBAL(THE_CONTEXTS) = NULL;*/
  /* GET_S_GLOBAL(THE_CURRENT_CONTEXT) = NULL; */
  GET_S_GLOBAL(Temp_Out_Memory) = NULL;
  GET_S_GLOBAL(The_Free_List) = NULL;
  GET_S_GLOBAL(Upper_Bound_Propagation_Rule) = NULL;
  GET_S_GLOBAL(Zero_Qbound) = NULL;/***/
  GET_S_GLOBAL(Zero_Quant) = NULL;/***/
  GET_S_GLOBAL(activationRule) = NULL;/***/
  GET_S_GLOBAL(afterAchievedRule) = NULL;/***/
  GET_S_GLOBAL(afterPlannedRule) = NULL;/***/
  GET_S_GLOBAL(alwaysTrueNodeGlobal) = NULL;/***/
  GET_S_GLOBAL(beforeAchievingRule) = NULL;/***/
  GET_S_GLOBAL(beforeHandlingRule) = NULL;/***/
  GET_S_GLOBAL(beforePlanningRule) = NULL;/***/
  GET_S_GLOBAL(byPassHnd) = NULL;
#endif
  GET_S_GLOBAL(byteSize) = 0;/***/
  GET_S_GLOBAL(byteTotal) = 0;/***/
  GET_S_GLOBAL(cursor) = 0;
#ifndef NMP_IPC
  GET_S_GLOBAL(demonMonitorTable) = NULL;
  GET_S_GLOBAL(durableItemList) = NULL;
#endif
  GET_S_GLOBAL(diffTime) = 0;/***/
  GET_S_GLOBAL(dispatchFreeListGlobal) = NULL;
  GET_S_GLOBAL(dispatchTable) = NULL;
  GET_S_GLOBAL(endLoop) = 0;/***/
  GET_S_GLOBAL(endMon) = 0;/***/
  GET_S_GLOBAL(endTime) = 0;/***/
  GET_S_GLOBAL(foundHnd) = NULL;
#ifndef NMP_IPC
  GET_S_GLOBAL(hasAssumptionJust) = 0;/***/
  GET_S_GLOBAL(endOfConstraintRule) = NULL;/***/
  GET_S_GLOBAL(free_flag_location) = 0;/***/
  GET_S_GLOBAL(indentGlobal) = 0;    
  GET_S_GLOBAL(isDelayedGlobal) = FALSE; 
  GET_S_GLOBAL(killLaterRuleGlobal) = NULL;
#endif
  GET_S_GLOBAL(lastTime) = 0;
  GET_S_GLOBAL(mTotal) = 0;/***/
  GET_M_GLOBAL(moduleList) = NULL;
  GET_S_GLOBAL(moduleTable) = NULL;
  GET_S_GLOBAL(monPer) = 0;/***/
  GET_S_GLOBAL(msgPer) = 0;/***/
  GET_S_GLOBAL(newModDataGlobal).modName = "**NEWMOD**";
  GET_S_GLOBAL(newModDataGlobal).hostName = "**UNKNOWN**";
#ifndef NMP_IPC
  GET_S_GLOBAL(monitorTable) = NULL;
  GET_S_GLOBAL(numBoundsGlobal) = 0; 
  GET_S_GLOBAL(numQuantitiesGlobal) = 0;
  GET_S_GLOBAL(numRelationsGlobal) = 0;
  GET_S_GLOBAL(numRuleQueuesGlobal) = 0;
  GET_S_GLOBAL(numRulesRunGlobal) = 0;
  GET_S_GLOBAL(num_consed) = 0;
  GET_S_GLOBAL(num_freed) = 0;
  GET_S_GLOBAL(pollingMonitorList) = NULL;
  GET_S_GLOBAL(qassertCheckGlobal) = FALSE;/***/
  GET_S_GLOBAL(resendAfterCrashGlobal) = FALSE;
  GET_S_GLOBAL(ruleNumberDecimalGlobal) = 1;
  GET_S_GLOBAL(ruleNumberGlobal) = 0;
  GET_S_GLOBAL(ruleNumberPositionsGlobal) = 1;
  GET_S_GLOBAL(ruleTraceGlobal) = FALSE;
#endif
  
#ifdef SERVER_PORT
  GET_S_GLOBAL(serverPortGlobal) = SERVER_PORT;
#else
  GET_S_GLOBAL(serverPortGlobal) = 0;/***/
#endif
  
  GET_S_GLOBAL(startLoop) = 0;/***/
  GET_S_GLOBAL(startMon) = 0;/***/
  GET_S_GLOBAL(startTime) = 0;/***/
  GET_S_GLOBAL(x_ipcAbortTable) = NULL;/***/
  GET_S_GLOBAL(x_ipcDebugGlobal) = FALSE;/***/
#ifndef NMP_IPC
  GET_S_GLOBAL(tmsAssumpJustificationGlobal) = NULL;
  GET_S_GLOBAL(tmsContraNodeGlobal) = NULL;
  GET_S_GLOBAL(tmsTraceGlobal) = FALSE;
#endif
  GET_S_GLOBAL(totalMon) = 0;/***/
  GET_S_GLOBAL(totalMsg) = 0;/***/
  GET_S_GLOBAL(totalMsgRun) = 0;/***/
  GET_S_GLOBAL(totalWait) = 0;/***/
  GET_S_GLOBAL(waitExpectedGlobal) = 0;
  GET_S_GLOBAL(waitList) = NULL;
  GET_S_GLOBAL(waitPer) = 0;/***/
  GET_S_GLOBAL(waitTotalGlobal) = 0;
#ifndef NMP_IPC
  GET_S_GLOBAL(whenAchievedRule) = NULL;/***/
  GET_S_GLOBAL(whenPlannedRule) = NULL;/***/
  GET_S_GLOBAL(whileAchievingRule) = NULL;/***/
  GET_S_GLOBAL(whilePlanningRule) = NULL;/***/
#endif
  
  /* name_buffer[100];*/
  for (i=0;i<100;i++) {
    GET_S_GLOBAL(name_buffer)[i] = '\0';
  }
  
#ifndef NMP_IPC
  GET_S_GLOBAL(freed_flag) = (value_ptr)&GET_S_GLOBAL(free_flag_location);
#endif
  bzero(GET_S_GLOBAL(Log_File_Name), sizeof(GET_S_GLOBAL(Log_File_Name)));
#if defined(VXWORKS)
  snprintf(GET_S_GLOBAL(Log_File_Name), sizeof(GET_S_GLOBAL(Log_File_Name)),
	   "x_ipc.log.%ld", tickGet());
#else
  snprintf(GET_S_GLOBAL(Log_File_Name), sizeof(GET_S_GLOBAL(Log_File_Name)),
	   "x_ipc.log.%ld", (long)time(0));
#endif
  
#ifndef NMP_IPC
  GET_S_GLOBAL(tapsUnderRoot) = FALSE;
#endif
  GET_S_GLOBAL(directDefault) = FALSE;
  
#if defined(VXWORKS) || defined(_WINSOCK_) || defined(OS2)
  GET_S_GLOBAL(listenToStdin) = FALSE;
#else
  GET_S_GLOBAL(listenToStdin) = TRUE;
#endif
  
  GET_S_GLOBAL(terminalLog).theFile = stdout;
  GET_S_GLOBAL(terminalLog).messages = TRUE;
  GET_S_GLOBAL(terminalLog).status = TRUE;
  GET_S_GLOBAL(terminalLog).time = FALSE;
  GET_S_GLOBAL(terminalLog).data = FALSE;
  GET_S_GLOBAL(terminalLog).summary = FALSE;
  GET_S_GLOBAL(terminalLog).refId = FALSE;
  GET_S_GLOBAL(terminalLog).parentId = 0;
  GET_S_GLOBAL(terminalLog).quiet = FALSE;
  GET_S_GLOBAL(terminalLog).flush = TRUE;
  GET_S_GLOBAL(terminalLog).ignore = TRUE;
  GET_S_GLOBAL(terminalLog).ignoreNow = FALSE;
  
  GET_S_GLOBAL(fileLog).theFile = NULL;
  GET_S_GLOBAL(fileLog).messages = FALSE;
  GET_S_GLOBAL(fileLog).status = FALSE;
  GET_S_GLOBAL(fileLog).time = FALSE;
  GET_S_GLOBAL(fileLog).data = FALSE;
  GET_S_GLOBAL(fileLog).summary = FALSE;
  GET_S_GLOBAL(fileLog).refId = FALSE;
  GET_S_GLOBAL(fileLog).parentId = 0;
  GET_S_GLOBAL(fileLog).quiet = FALSE;
  GET_S_GLOBAL(fileLog).flush = TRUE;
  GET_S_GLOBAL(fileLog).ignore = TRUE;
  GET_S_GLOBAL(fileLog).ignoreNow = FALSE;
  
  GET_M_GLOBAL(logList)[0] = &GET_S_GLOBAL(terminalLog);
  GET_M_GLOBAL(logList)[1] = &GET_S_GLOBAL(fileLog);
  GET_M_GLOBAL(logList)[2] = NULL;

  GET_C_GLOBAL(serverRead) = CENTRAL_SERVER_ID;
  GET_C_GLOBAL(serverWrite) = CENTRAL_SERVER_ID;

#ifndef NMP_IPC
  GET_S_GLOBAL(Location) = 0;/***/
  GET_S_GLOBAL(ParseString) = NULL;
  GET_S_GLOBAL(TokenList) = NULL;/***/
  GET_S_GLOBAL(TokenListHead) = NULL;
  GET_S_GLOBAL(cursorPosGlobal) = 0; 
  GET_S_GLOBAL(lineNumGlobal) = 0; 
  GET_S_GLOBAL(truncatedGlobal) = 0; 
  GET_S_GLOBAL(formatStack) = NULL;
#else /* NMP_IPC */
  GET_S_GLOBAL(nodeStatusNames)[(int32)AllocatedDispatch] = "Allocated";
  GET_S_GLOBAL(nodeStatusNames)[(int32)AttendingDispatch] = "Active";
  GET_S_GLOBAL(nodeStatusNames)[(int32)PendingDispatch] = "Pending";
  GET_S_GLOBAL(nodeStatusNames)[(int32)InactiveDispatch] = "Inactive";
  GET_S_GLOBAL(nodeStatusNames)[(int32)HandledDispatch] = "Completed";
#endif /* NMP_IPC */
}

void globalSFree(void) 
{
#ifndef NMP_IPC
  int i;
#endif
  
  if (sGlobalp()) {
    x_ipc_listFreeAllItems((LIST_FREE_FN)moduleClean, 
		     GET_M_GLOBAL(moduleList));
    x_ipc_listFree(&(GET_M_GLOBAL(moduleList)));

    x_ipc_idTableFree(&GET_S_GLOBAL(dispatchTable)); 
#ifndef NMP_IPC
    freeJustification(&GET_S_GLOBAL(tmsAssumpJustificationGlobal));
    x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipcFree, 
		     GET_S_GLOBAL(durableItemList));
    x_ipc_listFree(&(GET_S_GLOBAL(durableItemList)));
    x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipcFree, 
		     GET_S_GLOBAL(pollingMonitorList));
    x_ipc_listFree(&(GET_S_GLOBAL(pollingMonitorList)));
#endif
    x_ipc_listFreeAllItems((LIST_FREE_FN)dispatchFree, 
		     GET_S_GLOBAL(waitList));
    x_ipc_listFree(&(GET_S_GLOBAL(waitList)));
#ifndef NMP_IPC
    x_ipc_listFreeAllItems((LIST_FREE_FN)x_ipcFree, 
		     GET_S_GLOBAL(The_Free_List));
    x_ipc_listFree(&(GET_S_GLOBAL(The_Free_List)));
#endif
    
    freeDispatchList(GET_S_GLOBAL(dispatchFreeListGlobal));

#ifndef NMP_IPC
    ruleFree(&GET_S_GLOBAL(Lower_Bound_Propagation_Rule));
    ruleFree(&GET_S_GLOBAL(Relation_Propagation_Rule));
    ruleFree(&GET_S_GLOBAL(Repropagate_Lower_Bound_Rule));
    ruleFree(&GET_S_GLOBAL(Repropagate_Upper_Bound_Rule));
    ruleFree(&GET_S_GLOBAL(Upper_Bound_Propagation_Rule));
    ruleFree(&GET_S_GLOBAL(activationRule));
    ruleFree(&GET_S_GLOBAL(afterAchievedRule));
    ruleFree(&GET_S_GLOBAL(afterPlannedRule));
    ruleFree(&GET_S_GLOBAL(beforeAchievingRule));
    ruleFree(&GET_S_GLOBAL(beforeHandlingRule));
    ruleFree(&GET_S_GLOBAL(beforePlanningRule));
    ruleFree(&GET_S_GLOBAL(endOfConstraintRule));
    ruleFree(&GET_S_GLOBAL(killLaterRuleGlobal));
    ruleFree(&GET_S_GLOBAL(whenAchievedRule));
    ruleFree(&GET_S_GLOBAL(whenPlannedRule));
    ruleFree(&GET_S_GLOBAL(whileAchievingRule));
    ruleFree(&GET_S_GLOBAL(whilePlanningRule));

    FreeQuantity(&GET_S_GLOBAL(Minus_One_Quant));
/*    FreeQuantity(&GET_S_GLOBAL(Now));*/
    FreeQuantity(&GET_S_GLOBAL(One_Quant));
    FreeQuantity(&GET_S_GLOBAL(Zero_Quant));
#endif
    
    x_ipc_hashTableFree(&GET_S_GLOBAL(Relation_Hash_Table),x_ipc_hashItemsFree, NULL);
#ifndef NMP_IPC
    x_ipc_hashTableFree(&GET_S_GLOBAL(demonMonitorTable),x_ipc_hashItemsFree, NULL);
    x_ipc_hashTableFree(&GET_S_GLOBAL(monitorTable),x_ipc_hashItemsFree, NULL);
#endif
    x_ipc_hashTableFree(&GET_S_GLOBAL(x_ipcAbortTable),x_ipc_hashItemsFree, NULL);
    x_ipc_hashTableFree(&GET_S_GLOBAL(varTable),x_ipc_hashItemsFree, NULL);
    
#ifndef NMP_IPC
    x_ipcFree((char *)GET_S_GLOBAL(Default_Finite_Lower_Qbound));
    x_ipcFree((char *)GET_S_GLOBAL(Default_Finite_Upper_Qbound));
    x_ipcFree((char *)GET_S_GLOBAL(Default_Lower_Qbound));
    x_ipcFree((char *)GET_S_GLOBAL(Default_Upper_Qbound));
    x_ipcFree((char *)GET_S_GLOBAL(Minus_One_Qbound));
    x_ipcFree((char *)GET_S_GLOBAL(One_Qbound));
    x_ipcFree((char *)GET_S_GLOBAL(Zero_Qbound));

    tmsFreeNode(&GET_S_GLOBAL(alwaysTrueNodeGlobal));
    tmsFreeNode(&GET_S_GLOBAL(tmsContraNodeGlobal));
  
    Free_Memory(&GET_S_GLOBAL(Search_Memory));
    Free_Memory(&GET_S_GLOBAL(Temp_Out_Memory));

    Free_Queue(&GET_S_GLOBAL(FO_Queue));
    Free_Queue(&GET_S_GLOBAL(In_Queue));
    for(i=0; i<MAX_POSSIBLE_PRIORITY; i++)
      Free_Queue(&GET_S_GLOBAL(tmsPriorityQueuesGlobal[i]));

    Free_Stack(&GET_S_GLOBAL(Out_Stack));
#endif

#if defined(VXWORKS)
    x_ipcFree((void *)gS);
#endif  
  }
}

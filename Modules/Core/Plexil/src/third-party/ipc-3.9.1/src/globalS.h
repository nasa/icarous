/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: burying globals under a rock!
 *
 * FILE: globalS.h
 *
 * ABSTRACT: externs
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: globalS.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:24  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.4  1997/03/07 17:49:42  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.2.2.3  1996/10/18 18:05:08  reids
 * Moved state var from server to module.
 *
 * Revision 1.2.2.2  1996/10/16 15:18:28  reids
 * Change time value types to "unsigned long"
 *
 * Revision 1.2.2.1  1996/10/02 20:58:35  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.3  1996/09/06 22:30:29  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.2  1996/05/24 16:45:54  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.1  1996/05/09 01:01:27  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:31  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.32  1996/08/05  16:06:56  rich
 * Added comments to endifs.
 *
 * Revision 1.31  1996/07/25  22:24:27  rich
 * Fixed some memory leaks with handlers and removed some problems where a
 * disconnection caused a cleanup, but a variable would be assumed to still
 * be valid.
 *
 * Revision 1.30  1996/06/30  20:17:41  reids
 * Handling of polling monitors was severely broken.
 *
 * Revision 1.29  1996/05/09  18:31:02  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.28  1996/01/23  00:06:34  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.27  1996/01/05  16:31:25  rich
 * Added windows NT port.
 *
 * Revision 1.26  1995/10/25  22:48:28  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.25  1995/07/06  21:16:31  rich
 * Solaris and Linux changes.
 *
 * Revision 1.24  1995/05/31  19:35:40  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.23  1995/04/19  14:28:18  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.22  1995/04/07  05:03:16  rich
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
 * Revision 1.21  1995/04/04  19:42:21  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.20  1995/03/30  15:43:06  rich
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
 * Revision 1.19  1995/03/28  01:14:37  rich
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
 * Revision 1.18  1995/01/25  19:38:35  rich
 * Removed defns.h.  Placed needed definitions in basics.h.
 *
 * Revision 1.17  1995/01/18  22:40:43  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.16  1994/05/25  04:57:27  rich
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
 * Revision 1.15  1994/05/17  23:15:58  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.14  1994/05/06  04:47:09  rich
 * Put central io routines in a new file.
 * Fixed GNUmakefile.
 *
 * Revision 1.13  1994/04/28  22:16:52  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 * Revision 1.12  1994/04/16  19:42:21  rich
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
 * Revision 1.11  1994/01/31  18:27:58  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.10  1993/12/14  17:33:49  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.9  1993/12/01  18:03:37  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.8  1993/11/21  20:17:59  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.7  1993/10/20  19:00:40  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.6  1993/08/30  21:53:39  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.5  1993/08/27  08:38:42  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.4  1993/08/27  07:15:01  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  16:01:31  rich
 * Changed the global include files so that they don't double include
 * system files.  This was causing problems on the mach machines.
 *
 * Revision 1.2  1993/08/20  00:26:09  fedor
 * commiting others changes
 *
 * Revision 1.1.1.1  1993/05/20  05:45:43  rich
 * Importing x_ipc version 8
 *
 * Revision 8.1  1993/05/20  00:30:03  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:59  fedor
 * Added Logging.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *
 *****************************************************************************/

#ifndef INCglobalS
#define INCglobalS

#include "globalM.h"

#include "res.h"

#ifndef NMP_IPC
#include "tms.h"
#include "cells.h"
#include "rules.h"
#include "qlattice.h"
#include "taskTree.h"
#include "tplConstr.h"
#endif
#include "lex.h"

#ifndef NMP_IPC
#include "queue.h"
#endif

#include "logging.h"

#ifdef DOS_FILE_NAMES
#include "printDat.h"
#include "comServe.h"
#include "parseFmt.h"
#ifndef NMP_IPC
#include "exceptio.h"
#endif
#else
#include "printData.h"
#include "comServer.h"
#include "parseFmttrs.h"
#ifndef NMP_IPC
#include "exception.h"
#endif
#endif /* DOS_FILE_NAMES */

#ifndef NMP_IPC
#include "mon.h"
#endif
#include "msgTap.h"
#ifdef DOS_FILE_NAMES
#include "centralI.h"
#include "globalVa.h"
#else
#include "centralIO.h"
#include "globalVar.h"
#endif /* DOS_FILE_NAMES */

/*****************************************/

#if !defined(VXWORKS) && !defined(_WINSOCK_)
extern struct timeval timeBlock;
#endif

/**************************/

typedef struct _GS {
  /** struct timeb tp;****/
  /* CONTEXT_PTR THE_CURRENT_CONTEXT;*/ /* NOT USED: RGS 11/11/92 */
  DISPATCH_PTR dispatchFreeListGlobal;
  FILE *Log_File;

  HASH_TABLE_PTR Relation_Hash_Table;
#ifndef NMP_IPC
  HASH_TABLE_PTR demonMonitorTable;
  HASH_TABLE_PTR monitorTable;
#endif
  HASH_TABLE_PTR moduleTable;
  HASH_TABLE_PTR x_ipcAbortTable;
  HASH_TABLE_PTR varTable;
#ifndef NMP_IPC
  DISPATCH_HND_PTR byPassHnd;
#endif
  DISPATCH_HND_PTR foundHnd;
  ID_TABLE_PTR dispatchTable; 
#ifndef NMP_IPC
  JUSTIFICATION_PTR tmsAssumpJustificationGlobal;
  /* LIST_PTR THE_CONTEXTS;*/ /* NOT USED: RGS 11/11/92 */
  LIST_PTR durableItemList;
  LIST_PTR pollingMonitorList;
#endif
  LIST_PTR waitList;
#ifndef NMP_IPC
  LIST_PTR The_Free_List;
#endif
  MODULE_PTR x_ipcServerModGlobal;
  MOD_DATA_TYPE newModDataGlobal;
#ifndef NMP_IPC
  QBOUND_PTR Default_Finite_Lower_Qbound;
  QBOUND_PTR Default_Finite_Upper_Qbound;
  QBOUND_PTR Default_Lower_Qbound;
  QBOUND_PTR Default_Upper_Qbound;
  QBOUND_PTR Minus_One_Qbound;
  QBOUND_PTR One_Qbound;
  QBOUND_PTR Zero_Qbound;
  QUANTITY_PTR Minus_One_Quant;
  QUANTITY_PTR Now;
  QUANTITY_PTR One_Quant;
  QUANTITY_PTR Zero_Quant;
  RULE_PTR Lower_Bound_Propagation_Rule;
  RULE_PTR Relation_Propagation_Rule;
  RULE_PTR Repropagate_Lower_Bound_Rule;
  RULE_PTR Repropagate_Upper_Bound_Rule;
  RULE_PTR Upper_Bound_Propagation_Rule;
  RULE_PTR activationRule;
  RULE_PTR afterAchievedRule;
  RULE_PTR afterPlannedRule;
  RULE_PTR beforeAchievingRule;
  RULE_PTR beforeHandlingRule;
  RULE_PTR beforePlanningRule;
  RULE_PTR endOfConstraintRule;
  RULE_PTR killLaterRuleGlobal;
  RULE_PTR whenAchievedRule;
  RULE_PTR whenPlannedRule;
  RULE_PTR whileAchievingRule;
  RULE_PTR whilePlanningRule;
  X_IPC_TIME_POINT_TYPE endAchievement;
  X_IPC_TIME_POINT_TYPE endHandling;
  X_IPC_TIME_POINT_TYPE endPlanning;
  X_IPC_TIME_POINT_TYPE startAchievement;
  X_IPC_TIME_POINT_TYPE startHandling;
  X_IPC_TIME_POINT_TYPE startPlanning;
  TMS_NODE_PTR alwaysTrueNodeGlobal;
  TMS_NODE_PTR tmsContraNodeGlobal;
  TokenPtr TokenList;
  TokenPtr TokenListHead;
  const char *ParseString;
  STR_LIST_PTR formatStack;
#endif
  char *nodeStatusNames[DISPATCH_STATUS_TOTAL];
  char Log_File_Name[MAX_LOG_FILE_NAME_LENGTH];
  LOG_TYPE terminalLog, fileLog;
  
  /* 14-Sep-91: fedor: this is actually more space savings than
     many static buffers. sprintf should insure the ending '\0' termination.
     400 characters should be large enough to hold a max_normal value.
     Should only be used if the sprintf is done and then it is printed. */
  /* main scratch buffer for printing */
  
#ifndef NMP_IPC
  char buffer[400]; 
#endif
  char name_buffer[100];
#ifndef NMP_IPC
  float MINUS_INFINITY;
  float PLUS_INFINITY;
  float Prop_Epsilon;
  float Qlat_Epsilon;
#endif
  float monPer;
  float msgPer;
  float totalLoop;
  float totalMon;
  float totalMsg;
  float totalWait;
  float waitPer;
#ifndef NMP_IPC
  int32 Location;
  int32 Node_Counter;
#endif
  long cursor;
#ifndef NMP_IPC
  int32 cursorPosGlobal;
  int32 free_flag_location;
  int32 hasAssumptionJust;
#endif
  int32 indentGlobal;
  int32 isDelayedGlobal;
  unsigned long lastTime;
#ifndef NMP_IPC
  int32 lineNumGlobal;
  int32 numBoundsGlobal;
  int32 numQuantitiesGlobal;
  int32 numRelationsGlobal;
  int32 numRuleQueuesGlobal;
  int32 numRulesRunGlobal;
  int32 num_consed;
  int32 num_freed;
  int32 qassertCheckGlobal;
  int32 ruleNumberDecimalGlobal;
  int32 ruleNumberGlobal;
  int32 ruleNumberPositionsGlobal;
  int32 ruleTraceGlobal;
  int32 tmsTraceGlobal;
  int32 truncatedGlobal;
#endif
  int32 x_ipcDebugGlobal;
  int32 resendAfterCrashGlobal;
  int serverPortGlobal;
  int32 waitExpectedGlobal;
  int32 waitTotalGlobal;
  long avgSize;
  long avgTime;
  long byteSize;
  long byteTotal;
  long diffTime;
  unsigned long endLoop;
  unsigned long endMon;
  unsigned long endTime;
  unsigned long mTotal;
  unsigned long startLoop;
  unsigned long startMon;
  unsigned long startTime;
  unsigned long totalMsgRun;
#ifndef NMP_IPC
  memory_ptr Search_Memory;
  memory_ptr Temp_Out_Memory;
  queue_ptr FO_Queue;
  queue_ptr In_Queue;
  queue_ptr tmsPriorityQueuesGlobal[MAX_POSSIBLE_PRIORITY];
  stack_ptr Out_Stack;
  value_ptr freed_flag;

  int32 tapsUnderRoot;
#endif  
  
  BOOLEAN listenToStdin;
  BOOLEAN directDefault;

#ifndef NMP_IPC
  TASK_TREE_NODE_PTR taskTreeRootGlobal;
#endif

} GS_TYPE, *GS_PTR;

#if defined(VXWORKS)
/* VX works needs to access the globals through a pointer. */

#define GET_S_GLOBAL(var) (gS->var)
#define sGlobalp() (gS != NULL)

extern GS_PTR gS;

#else
/* In general, the globals can just be statically allocated. */

#define GET_S_GLOBAL(var) (gS.var)
#define sGlobalp() (gS_ptr != NULL)

extern GS_TYPE gS;
extern GS_PTR  gS_ptr;

#endif

#ifdef NMP_IPC
#define GET_SM_GLOBAL(var) GET_M_GLOBAL(var)
#else
#define GET_SM_GLOBAL(var) GET_S_GLOBAL(var)
#endif

void globalSInit(void);
void globalSFree(void);

#endif /* INCglobalS */

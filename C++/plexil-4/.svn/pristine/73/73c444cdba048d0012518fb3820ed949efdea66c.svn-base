/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: x_ipc
 *
 * FILE: x_ipc.h
 *
 * ABSTRACT:
 * 
 *  Task Control Architecture
 *
 *  Include File
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/tca.h,v $ 
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: tca.h,v $
 * Revision 2.5  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2003/03/07 19:45:46  trey
 * added ability to silently close child copies of the connection to the central server
 *
 * Revision 2.3  2000/08/14 21:28:35  reids
 * Added support for making under Windows.
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.10  1997/01/27 20:10:01  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.8  1997/01/11 01:21:25  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.7.4.1  1996/12/24 14:41:49  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.1.2.7  1996/12/18 15:27:23  reids
 * Renamed EXTERN_FUNCTION to IPC_EXTERN_FUNCTION to avoid name conflicts
 *
 * Revision 1.1.2.6  1996/11/22 20:18:11  rouquett
 * forgot that semicolon in the c++ EXTERN_FUNCTION
 *
 * Revision 1.1.2.5  1996/11/22 19:03:44  rouquett
 * get rid of the annoying warnings with c++
 *
 * Revision 1.1.2.4  1996/10/29 14:55:00  reids
 * Use the right prototype definition for x_ipcRegisterMallocHnd.
 *
 * Revision 1.1.2.3  1996/10/24 15:19:27  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.1.2.2  1996/10/18 18:18:12  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 *
 * Revision 1.1.2.1  1996/10/14 00:11:49  reids
 * Added a "raw" option to handler languages.
 *
 * Revision 1.1  1996/05/09 01:02:04  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/04/24 19:11:08  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.3  1996/03/19 03:38:54  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:20:05  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:47  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.46  1996/06/27  15:40:18  rich
 * Added x_ipcGetAcceptFds.
 *
 * Revision 1.45  1996/05/09  18:31:37  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.44  1996/03/15  21:28:00  reids
 * Added x_ipcQueryNotify, x_ipcAddEventHandler, x_ipcRemoveEventHandler.
 *   Fixed re-registration bug; Plugged memory leaks; Fixed way task trees
 *   are killed; Added support for "enum" format type.
 *
 * Revision 1.43  1996/03/05  05:04:56  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.42  1996/02/21  18:30:29  rich
 * Created single event loop.
 *
 * Revision 1.41  1996/02/10  16:50:40  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.40  1996/02/01  04:03:40  rich
 * Generalized updateVersion and added recursion.
 *
 * Revision 1.39  1996/01/30  15:05:03  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.38  1996/01/27  21:54:33  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.37  1996/01/05  16:31:42  rich
 * Added windows NT port.
 *
 * Revision 1.36  1995/10/29  18:27:12  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.35.2.1  1995/10/27  01:48:15  rich
 * Removed unimplemented calls.
 *
 * Revision 1.35  1995/10/25  22:49:03  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.34  1995/10/17  17:36:47  reids
 * Added a "language" slot to the HND_TYPE data structure, so that the LISP
 *   X_IPC version will know what language to decode the data structure into
 *   (extensible for other languages, as well).
 *
 * Revision 1.33  1995/10/07  19:07:56  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.32  1995/08/14  21:32:05  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.31  1995/08/08  05:43:32  rich
 * Removed junk from x_ipc.h file.
 *
 * Revision 1.30  1995/07/26  20:42:41  rich
 * Recognize dump when not compiled with DBMALLOC, remove proc from prototypes.
 *
 * Revision 1.29  1995/07/24  15:56:59  reids
 * Somehow x_ipc.h got all screwed up (confused with x_ipc.lisp)!
 *
 * Revision 1.27  1995/07/19  14:26:50  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.26  1995/07/12  04:55:34  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.25  1995/07/10  16:18:53  rich
 * Interm save.
 *
 * Revision 1.24  1995/06/14  03:22:35  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.23  1995/06/05  23:59:12  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.22  1995/05/31  19:37:00  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.21  1995/04/21  03:53:35  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.20  1995/04/04  19:43:08  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.19  1995/03/28  01:15:07  rich
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
 * Revision 1.18  1995/03/19  19:39:47  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.17  1995/03/18  15:11:06  rich
 * Fixed updateVersion script so it can be run from any directory.
 *
 * Revision 1.16  1995/02/06  14:46:13  reids
 * Removed the "Global" suffix from x_ipcRootNode, x_ipcServer and x_ipcDefaultTime
 *
 * Revision 1.15  1995/01/18  22:43:14  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.14  1994/05/25  17:32:41  reids
 * Added utilities to limit the number of pending messages
 *
 * Revision 1.13  1994/05/25  04:58:06  rich
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
 * Revision 1.12  1994/05/17  23:18:00  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.11  1994/05/11  01:57:36  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.10  1994/04/28  16:17:39  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.9  1994/04/25  16:12:07  reids
 * Fixed external declaration of x_ipcByPassException, which was mis-spelled
 *
 * Revision 1.8  1994/04/16  19:43:25  rich
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
 * Revision 1.7  1993/11/21  20:19:49  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/10/20  18:56:11  rich
 * Got rid of VOID_FN an INT_FN.
 *
 * Revision 1.6  1993/10/06  18:07:24  reids
 * Fixed two bugs: x_ipcTplConstrain was not externed correctly and
 * -D option was not being incorporated correctly in compiling
 *
 * Revision 1.5  1993/08/30  21:54:48  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.6  1993/07/08  05:37:45  rich
 * Added function prototypes
 *
 * Revision 1.5  1993/07/06  20:30:05  reids
 * Accidentally deleted prototypes for x_ipcEnableDistributedResponses and
 * Disable...
 *
 * Revision 1.4  1993/07/05  18:11:30  reids
 * Added x_ipcDeregisterHandler function
 *
 * Revision 1.2  1993/05/27  22:15:24  rich
 * Added automatic logging.
 *
 * 13-Oct-92  Reid Simmons, School of Computer Science, CMU
 * Changed ConstraintClass to InformClass (more informative name).
 * Added BroadcastClass and MultiQueryClass messages.
 * Added WhenSuccess and WhenFailure message tap conditions
 *
 ****************************************************************************/

/* This flag ensures that multiple occurrences of the include file 
   don't cause problems*/
#ifndef INCx_ipc
#define INCx_ipc

#include <sys/types.h>

/* Support for HandleMessage */
#define WAITFOREVER (0xFFFFFFFF) /* must be less than 0 */

#ifdef Success
#undef Success
#endif

/* Some parameters need to be of a fixed number of bits. */

#if !defined(__TURBOC__) && !defined(WIN32)
typedef unsigned short u_int16;
typedef short int16;
typedef unsigned int u_int32;
typedef int int32;
#else
typedef unsigned int u_int16;
#define int16 short
typedef unsigned long u_int32;
#define int32 long
#endif

/* Communciation Return Values */
typedef enum {
  Success=0, TimeOut=1, Failure=2, WrongVersion=3, MsgUndefined=4,
  WrongMsgClass=5, NullReply=6
#ifdef FORCE_32BIT_ENUM
    , dummyX_IpcReturnValue = 0x7FFFFFFF
#endif
} X_IPC_RETURN_VALUE_TYPE;

/* Low Level Communication Status Values */
typedef enum {
  StatEOF=0, StatError=1, StatOK=2, StatSendEOF=3, StatSendError=4,
  StatRecvEOF=5, StatRecvError=6
#ifdef FORCE_32BIT_ENUM
    , dummyX_IpcReturnStatus = 0x7FFFFFFF
#endif
} X_IPC_RETURN_STATUS_TYPE;

/* Class Type */
#ifndef __TURBOC__
typedef enum {
  UNKNOWN=0, HandlerRegClass=1, ExecHndClass=2, QueryClass=3,
  GoalClass=4, CommandClass=5, InformClass=6, ExceptionClass=7,
  PollingMonitorClass=8, PointMonitorClass=9,
  DemonMonitorClass=10, ReplyClass=11, SuccessClass=12,
  FailureClass=13, FireDemonClass=14, BroadcastClass=15,
  MultiQueryClass=16
#ifdef FORCE_32BIT_ENUM
    , dummyX_IpcMsgClass = 0x7FFFFFFF
#endif
} X_IPC_MSG_CLASS_TYPE;
#else
typedef int32 X_IPC_MSG_CLASS_TYPE;
#define UNKNOWN              0
#define HandlerRegClass      1
#define ExecHndClass         2
#define QueryClass           3
#define GoalClass            4
#define CommandClass         5
#define InformClass          6
#define ExceptionClass       7
#define PollingMonitorClass  8
#define PointMonitorClass    9
#define DemonMonitorClass   10
#define ReplyClass          11
#define SuccessClass        12
#define FailureClass        13
#define FireDemonClass      14
#define BroadcastClass      15
#define MultiQueryClass     16
#endif

/* For upward compatibility */
#define ConstraintClass InformClass

/* Reference Status Return Values */
typedef 
enum { UnknownRefStatus=0, InactiveRef=1, PendingRef=2, ActiveRef=3,
	 HandledRef=4, PlannedRef=5, AchievedRef=6, KilledRef=7 
     } X_IPC_REF_STATUS_TYPE;

typedef struct _X_IPC_REF *X_IPC_REF_PTR;

typedef struct _X_IPC_CONTEXT *X_IPC_CONTEXT_PTR;

typedef void (* X_IPC_HND_FN)(X_IPC_REF_PTR, char *);

typedef void (* X_IPC_HND_DATA_FN)(X_IPC_REF_PTR, char *, void *clientData);

typedef struct { 
  int maxFire;     /* maximum times to fire action message;
		      -1 indicates infinite (default is -1) */
  int fireEvery;  /* fire action message every n times condition is satisfied.
		     (default is 1) */
  int duration;    /* maximum time, in seconds, that the monitor can run;
		      -1 indicates infinite time (default is -1) */
  int period;      /* time between polls, in seconds */
  int initialWait; /* if TRUE, don't poll immediately (default is FALSE) */
} INTERVAL_MONITOR_OPTIONS_TYPE, *INTERVAL_MONITOR_OPTIONS_PTR;

/*
 * For representing temporal constraints within modules (central uses
 * a different representation).
 */

#ifndef __TURBOC__
typedef enum {
  NoTime=0, StartPoint=1, EndPoint=2
#ifdef FORCE_32BIT_ENUM
    , dummyX_IpcPointClass = 0x7FFFFFFF
#endif
} X_IPC_POINT_CLASS_TYPE;
#else
typedef int32 X_IPC_POINT_CLASS_TYPE;
#define NoTime 0
#define StartPoint 1
#define EndPoint 2
#endif

#ifndef __TURBOC__
typedef enum {
  NoInterval=0, HandlingInterval=1, AchievementInterval=2, 
  PlanningInterval=3
#ifdef FORCE_32BIT_ENUM
    , dummyX_IpcIntervalClass = 0x7FFFFFFF
#endif
} X_IPC_INTERVAL_CLASS_TYPE;
#else
typedef int32 X_IPC_INTERVAL_CLASS_TYPE;
#define  NoInterval          0
#define  HandlingInterval    1
#define  AchievementInterval 2
#define  PlanningInterval    3
#endif

typedef enum {
  UnknownCondition=0, WhenSent=1, BeforeHandling=2,
  WhileHandling=3, AfterHandled=4, AfterReplied=5,
  AfterSuccess=6, AfterFailure=7, BeforeAchieving=8,
  WhileAchieving=9, AfterAchieved=10, WhenAchieved=11,
  BeforePlanning=12, WhilePlanning=13, AfterPlanned=14,
  WhenPlanned=15,	WhenSuccess=16, WhenFailure=17, WhenKilled=18
#ifdef FORCE_32BIT_ENUM
    , dummyTapCondition = 0x7FFFFFFF
#endif
} TAP_CONDITION_TYPE;

typedef struct {
  X_IPC_INTERVAL_CLASS_TYPE interval_class;
  int msgRef;
} X_IPC_INTERVAL_TYPE, *X_IPC_INTERVAL_PTR;

#define X_IPC_INTERVAL_FORMAT "{int,int}"

typedef struct { 
  X_IPC_POINT_CLASS_TYPE point_class;
  X_IPC_INTERVAL_TYPE interval;
} X_IPC_TIME_POINT_TYPE, *X_IPC_TIME_POINT_PTR;

#define X_IPC_TIME_POINT_FORMAT "{int,X_IPC_INTERVAL}"

typedef struct _TIMELIST { 
  X_IPC_TIME_POINT_TYPE timePoint;
  struct _TIMELIST *Next;
} X_IPC_TIME_LIST_TYPE, *X_IPC_TIME_LIST_PTR;

#define X_IPC_TIME_LIST_FORMAT "{X_IPC_TIME_POINT,*!}"

typedef struct { 
  X_IPC_TIME_POINT_TYPE timePoint1;
  const char *relationship;
  X_IPC_TIME_POINT_TYPE timePoint2;
} X_IPC_TPL_CONSTRAINT_TYPE, *X_IPC_TPL_CONSTRAINT_PTR;

#define X_IPC_TPL_CONSTRAINT_FORMAT "{X_IPC_TIME_POINT,*char,X_IPC_TIME_POINT}"

typedef struct {
  X_IPC_REF_PTR ERef, CRef;
  int numOfRetries;		/* number of times of CNode being retried */
} EXC_INFO_TYPE, * EXC_INFO_PTR;

/* The language in which the message handler is written */
typedef enum {
  UNKNOWN_LANGUAGE=0, C_LANGUAGE=1, LISP_LANGUAGE=2,
  RAW_C_LANGUAGE=3, RAW_LISP_LANGUAGE=4
#ifdef FORCE_32BIT_ENUM
    , dummyHndLanguageEnum = 0x7FFFFFFF
#endif
} HND_LANGUAGE_ENUM;

/***************************************************************************/

typedef enum {
  NO_TPLCONSTR = 0,
  
  /* End(Achievement(Last_Node)) <= Start(Achievement(Node)) */
  SEQ_ACH = 01,

  /* End(Planning(Last_Node)) <= Start(Planning(Node)) */
  SEQ_PLANNING = 02,
  
  /* End(Planning(Node)) <= Start(Achievement(Node)) */
  PLAN_FIRST = 04,

  /* Start(Planning(Node)) = Start(Achievement(Node)) */
  DELAY_PLANNING = 010 
#ifdef FORCE_32BIT_ENUM
    , dummyTplconstrEnum = 0x7FFFFFFF
#endif
} TPLCONSTR_ENUM;

#define x_ipcRegisterGoalMessage(name, format) \
x_ipcRegisterMessage(name, GoalClass, format, (const char *)NULL)

#define x_ipcRegisterCommandMessage(name, format) \
x_ipcRegisterMessage(name, CommandClass, format, (const char *)NULL)

#define x_ipcRegisterInformMessage(name, format) \
x_ipcRegisterMessage(name, InformClass, format, (const char *)NULL)

/* Kept for upward compatibility -- use "x_ipcRegisterInformMessage" */
#define x_ipcRegisterConstraintMessage(name, format) \
x_ipcRegisterInformMessage(name, format)

#define x_ipcRegisterQueryMessage(name, msgFormat, resFormat) \
x_ipcRegisterMessage(name, QueryClass, msgFormat, resFormat)

#define x_ipcRegisterExceptionMessage(name, format) \
x_ipcRegisterMessage(name, ExceptionClass, format, (const char *)NULL)

#define x_ipcRegisterBroadcastMessage(name, format) \
x_ipcRegisterMessage(name, BroadcastClass, format, (const char *)NULL)

#define x_ipcRegisterMultiQueryMessage(name, msgFormat, resFormat) \
x_ipcRegisterMessage(name, MultiQueryClass, msgFormat, resFormat)

#define VALID_TIME(timePoint) ((timePoint).point_class != NoTime)

#define TPL_ORDER(interval1, interval2) \
(x_ipcTplConstrain(x_ipcEndOf(interval1), "<=", x_ipcStartOf(interval2)))

#define x_ipcRaiseException(a,b,c)  x_ipcFailure(a,b,c)

/***************************************************************************/

/*  EXTERN_FUNCTION( rtn name, (arg types) );
 *	Macro to make external function declarations portable between 
 *      the major C dialects: C, ANSI C, and C++.
 */

#if defined(__cplusplus) /* C++ */
#define IPC_EXTERN_FUNCTION( rtn, args ) rtn args
#elif defined(__STDC__) || defined(__GNUC__) /* ANSI C */
#define IPC_EXTERN_FUNCTION( rtn, args ) extern rtn args
#elif defined(__TURBOC__) /* ANSI C */
#define IPC_EXTERN_FUNCTION( rtn, args ) extern rtn args
#else /* "Regular" C */
#define IPC_EXTERN_FUNCTION( rtn, args ) extern rtn()
#endif

/*********************
 * Library Functions *
 *********************/

#if defined(__cplusplus) /* C++ */
extern "C" {
#endif

/* 
 * Functions for starting X_IPC and setting parameters 
 */

IPC_EXTERN_FUNCTION( const char *x_ipcServerMachine,
		     (void));

IPC_EXTERN_FUNCTION( void x_ipcConnectModule,
		     (const char *moduleName, const char *serverHost));

IPC_EXTERN_FUNCTION( void x_ipcModuleProvides,
		     (const char *resourceName, ...));

IPC_EXTERN_FUNCTION( void x_ipcModuleProvidesArgv,
		     (const char **resourceNames));

IPC_EXTERN_FUNCTION( void x_ipcModuleRequires,
		     (const char *resourceName, ...));

IPC_EXTERN_FUNCTION( void x_ipcModuleRequiresArgv,
		     (const char **resourceNames));

IPC_EXTERN_FUNCTION( char **x_ipcQueryAvailable,
		     (void));

IPC_EXTERN_FUNCTION( void x_ipcEnableDistributedResponses, 
		     (void));

IPC_EXTERN_FUNCTION( void x_ipcDisableDistributedResponses,
		     (void));

IPC_EXTERN_FUNCTION( void x_ipcRegisterFreeMemHnd,
		     (void (*func)(u_int32), int retry));

#if defined(DBMALLOC)
IPC_EXTERN_FUNCTION( void x_ipcRegisterMallocHnd,
		     (void *(*func)(const char*,int, SIZETYPE), int retry));
#elif defined(__sgi)
IPC_EXTERN_FUNCTION( void x_ipcRegisterMallocHnd,
		     (void *(*func)(unsigned int), int retry));
#elif defined(__TURBOC__)
void x_ipcRegisterMallocHnd(void *(*func)(unsigned long), long retry);
#else
IPC_EXTERN_FUNCTION( void x_ipcRegisterMallocHnd,
		     (void *(*func)(size_t size), int retry));
#endif

IPC_EXTERN_FUNCTION( void x_ipcWillListen,
		     (int listen));

IPC_EXTERN_FUNCTION( void x_ipcWaitUntilReady,
		     (void));

/* 
 * Functions for registering messages, handlers...
 */

IPC_EXTERN_FUNCTION( void x_ipcRegisterNamedFormatter,
		     (const char *formName, const char *formString));

IPC_EXTERN_FUNCTION( void x_ipcRegisterLengthFormatter,
		     (const char *formName, int32 length));

IPC_EXTERN_FUNCTION( void x_ipcRegisterMessage,
		     (const char *name, X_IPC_MSG_CLASS_TYPE msg_class, 
		      const char *msgFormat, const char *resFormat));

IPC_EXTERN_FUNCTION( void x_ipcPreloadMessage,
		     (const char *name));

IPC_EXTERN_FUNCTION( void _x_ipcRegisterHandlerL,
		     (const char *msgName, const char *hndName, 
		      X_IPC_HND_FN hndProc, HND_LANGUAGE_ENUM hndLanguage));

#define x_ipcRegisterHandler(msgName, hndName, hndProc) \
_x_ipcRegisterHandlerL(msgName, hndName, (X_IPC_HND_FN) hndProc, C_LANGUAGE)
     
IPC_EXTERN_FUNCTION( void x_ipcDeregisterHandler,
		     (const char *msgName, const char *hndName));

/* 
 * Functions for getting information about registered messages.
 */

IPC_EXTERN_FUNCTION( int x_ipcMessageRegistered,
		     (const char *msgName));

IPC_EXTERN_FUNCTION( int x_ipcMessageHandlerRegistered,
		     (const char *msgName));

IPC_EXTERN_FUNCTION( char **x_ipcGetRegisteredMsgs,
		     (void));

typedef struct {
  char *name;
  char *msgFormat, *resFormat;
  X_IPC_MSG_CLASS_TYPE msg_class;
  char *resourceName;
  int numberOfHandlers;
} MSG_INFO_TYPE, *MSG_INFO_PTR;

/* Call this function with a pointer to a valid MSG_INFO_TYPE structure with 
 * the name field filled and it will fill in the rest of the fields.
 */

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcGetMsgInfo,
		     (MSG_INFO_PTR info));

/* 
 * define a set of macros that make it easy to register simple 
 * messages and handlers 
 */

#define x_ipcRegisterGoal(name, format, hndProc) \
{x_ipcRegisterGoalMessage(name, format); \
   x_ipcRegisterHandler(name, name, hndProc);}

#define x_ipcRegisterCommand(name, format, hndProc) \
{x_ipcRegisterCommandMessage(name, format); \
   x_ipcRegisterHandler(name, name, hndProc);}

#define x_ipcRegisterInform(name, format, hndProc) \
{x_ipcRegisterInformMessage(name, format); \
   x_ipcRegisterHandler(name, name, hndProc);}

#define x_ipcRegisterQuery(name, format, reply, hndProc) \
{x_ipcRegisterQueryMessage(name, format, reply); \
   x_ipcRegisterHandler(name, name, hndProc);}

#define x_ipcRegisterException(name, format, hndProc) \
{x_ipcRegisterExceptionMessage(name, format); \
   x_ipcRegisterHandler(name, name, hndProc);}

IPC_EXTERN_FUNCTION( void x_ipcRegisterExitProc,
		     (void (*)(void)));

IPC_EXTERN_FUNCTION( void x_ipcLimitPendingMessages,
		     (const char *msgName, const char *resName, int32 limit));

IPC_EXTERN_FUNCTION( void x_ipcIgnoreLogging, (char *msgName));

IPC_EXTERN_FUNCTION( void x_ipcResumeLogging, (char *msgName));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcCleanUpAfterAchieved,
		     (const char *msgName));

/* 
 * Functions for sending/replying to messages 
 */

/* 
 * "x_ipcInform" and "x_ipcAddConstraint" are equivalent -- the former name
 * is just more informative of what the function does
 */
IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcInform,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcAddConstraint,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcQuery,
		     (const char *name, void *queryData, void *replyData));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcReply,
		     (X_IPC_REF_PTR ref, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcNullReply,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcQuerySend,
		     (const char *name, void *query, X_IPC_REF_PTR *ref));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcQueryReceive,
		     (X_IPC_REF_PTR ref, void *reply));

typedef void (*REPLY_HANDLER_FN)(void *replyData, void *clientData);

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcQueryNotify,
		     (const char *name, void *query,
		      REPLY_HANDLER_FN replyHandler,
		      void *clientData));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcExpandGoal,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcExpandGoalWithConstraints,
		     (X_IPC_REF_PTR ref, const char *name,
		      const void *data, int32 tplConstraints));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcExecuteCommand,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcExecuteCommandWithConstraints,
		     (X_IPC_REF_PTR ref, const char *name,
		      const void *data, int32 tplConstraints));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcWaitForCommand,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcWaitForCommandWithConstraints,
		     (X_IPC_REF_PTR ref, const char *name,
		      const void *data, int32 tplConstraints));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcExecute,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcExecuteWithConstraints,
		     (X_IPC_REF_PTR ref, const char *name,
		      const void *data, int tplConstraints));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcWaitForGoalWithConstraints,
		     (X_IPC_REF_PTR ref, 
		      const char *name,
		      const void *data, 
		      int tplConstr));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcWaitForExecutionWithConstraints,
		     (X_IPC_REF_PTR ref, 
		      const char *name,
		      const void *data, 
		      int tplConstr));
		
IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcWaitForExecution,
		     (const char *name, const void *data));

#define x_ipcExecuteCommandAsync(name, data) \
x_ipcExecuteCommandWithConstraints(NULL,name,data,NO_TPLCONSTR)

#define x_ipcExecuteAsync(name, data) \
x_ipcExecuteWithConstraints(NULL,name,data,NO_TPLCONSTR)

#define x_ipcMaybeExecute(name, data) \
{ if (x_ipcMessageHandlerRegistered(name)) x_ipcExecute(name,data); }

#define x_ipcMaybeExecuteWithConstraints(ref, name, data, tplConstraints) \
{ if (x_ipcMessageHandlerRegistered(name))\
    x_ipcExecuteWithConstraints(ref,name,data,tplConstraints); }

#define x_ipcMaybeExecuteAsync(name, data) \
{ if (x_ipcMessageHandlerRegistered(name)) x_ipcExecuteAsync(name,data); }

#ifdef macintosh
/* Needed by MCL version of IPC */
#pragma export on
#endif
IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcSuccess,
		     (X_IPC_REF_PTR ref));
#ifdef macintosh
#pragma export off
#endif

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcFailure,
		     (X_IPC_REF_PTR ref, const char *description,
		      const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcBroadcast,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcMultiQuery,
		     (const char *name, void *query, int32 max,
		      X_IPC_REF_PTR *refPtr));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcMultiReceive,
		     (X_IPC_REF_PTR ref, void *reply, long timeout));

/* 
 * Functions for handling messages 
 */

IPC_EXTERN_FUNCTION( void x_ipcModuleListen,
		     (void));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcHandleMessage,
		     (long seconds));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcHandleFdInput,
		     (int fd));

typedef void (* X_IPC_FD_HND_FN)(int, void *);

IPC_EXTERN_FUNCTION( void x_ipcAddEventHandler,
		     (int fd, X_IPC_FD_HND_FN hnd, void *clientData));

IPC_EXTERN_FUNCTION( void x_ipcRemoveEventHandler,
		     (int fd));

/* 
 * Functions for handling message data.
 */

IPC_EXTERN_FUNCTION( void x_ipcFreeData,
		     (const char *msgName, void *data));

IPC_EXTERN_FUNCTION( void *x_ipcAllocateReply,
		     (const char *msgName));

IPC_EXTERN_FUNCTION( void x_ipcFreeReply,
		     (const char *msgName, void *replyData));

/* 
 * Functions for handling temporal relations.
 */

IPC_EXTERN_FUNCTION( X_IPC_TIME_POINT_TYPE x_ipcStartOf,
		     (X_IPC_INTERVAL_TYPE interval));

IPC_EXTERN_FUNCTION( X_IPC_TIME_POINT_TYPE x_ipcEndOf,
		     (X_IPC_INTERVAL_TYPE interval));

IPC_EXTERN_FUNCTION( X_IPC_INTERVAL_TYPE x_ipcHandlingOf,
		     (X_IPC_REF_PTR msgRef));

IPC_EXTERN_FUNCTION( X_IPC_INTERVAL_TYPE x_ipcAchievementOf,
		     (X_IPC_REF_PTR msgRef));

IPC_EXTERN_FUNCTION( X_IPC_INTERVAL_TYPE x_ipcPlanningOf,
		     (X_IPC_REF_PTR msgRef));

IPC_EXTERN_FUNCTION( void x_ipcTplConstrain,
		     (X_IPC_TIME_POINT_TYPE timePoint1, 
		      const char *relationship,
		      X_IPC_TIME_POINT_TYPE timePoint2));

/* 
 * Functions for handling message taps.
 */

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcTapMessage,
		     (TAP_CONDITION_TYPE condition,
		      const char *tappedMsg, const char *listenMsg));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcTapReference,
		     (TAP_CONDITION_TYPE condition,
		      X_IPC_REF_PTR tappedRef,
		      const char *listenMsg));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindTappedReference,
		     (X_IPC_REF_PTR listeningRef));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcRemoveTap,
		     (TAP_CONDITION_TYPE condition,
		      const char *tappedMsg, const char *listenMsg));

/* 
 * Functions for handling resources.
 */

IPC_EXTERN_FUNCTION( void x_ipcRegisterResource,
		     (const char *resName, int32 capacity));

IPC_EXTERN_FUNCTION( void x_ipcDirectResource,
		     (const char *resName));

IPC_EXTERN_FUNCTION( void x_ipcAddHndToResource,
		     (const char *hndName, const char *resName));

IPC_EXTERN_FUNCTION( void x_ipcLimitPendingResource,
		     (const char *resName, int32 limit));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcReserveResource,
		     (const char *resName));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcReserveModResource,
		     (const char *modName, const char *resName));

IPC_EXTERN_FUNCTION( void x_ipcCancelReservation,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcLockResource,
		     (const char *resName));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcLockModResource,
		     (const char *modName, const char *resName));

IPC_EXTERN_FUNCTION( void x_ipcUnlockResource,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcAddHndToResource,
		     (const char *hndName, const char *resName));

/* 
 * Functions for handling variables.
 */

/* Experimental version of global variables */
IPC_EXTERN_FUNCTION( void x_ipcRegisterVar,
		     (const char*, const char*));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE _x_ipcSetVar,
		     (const char*, const void*));

#define x_ipcSetVar(msgName, value) \
_x_ipcSetVar(msgName, (const void*) value)

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE _x_ipcGetVar,
		     (const char*, void*));

#define x_ipcGetVar(msgName, value) \
_x_ipcGetVar(msgName, (void*) value)

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE _x_ipcGetSetVar,
		     (const char*, void*, void*));

#define x_ipcGetSetVar(msgName, setValue, getValue) \
_x_ipcGetSetVar(msgName, (void*) setValue, (void*) getValue)

IPC_EXTERN_FUNCTION( void _x_ipcWatchVar,
		     (const char *name, const char *format,
		      X_IPC_HND_FN watchFn));

#define x_ipcWatchVar(varName, format, handler) \
_x_ipcWatchVar(varName, format, (X_IPC_HND_FN) handler)

IPC_EXTERN_FUNCTION( void x_ipcUnwatchVar,
		     (const char *name));

IPC_EXTERN_FUNCTION( void x_ipcIgnoreVarLogging, (const char *varName));

IPC_EXTERN_FUNCTION( void x_ipcFreeVar, (const char *varName, void *varData));

/* 
 * Functions for handling the task tree.
 */

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcRootNode,
		     (void));

/* For upward compatibility */
#define x_ipcRootNodeGlobal x_ipcRootNode

IPC_EXTERN_FUNCTION( int x_ipcIsSameReference,
		     (X_IPC_REF_PTR ref1, X_IPC_REF_PTR ref2));

IPC_EXTERN_FUNCTION( int x_ipcReferenceId,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( const char *x_ipcReferenceName,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void *x_ipcReferenceData,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcReferenceRelease,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( int x_ipcReferenceStatus,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcRefFree,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcAddChildReference,
		     (X_IPC_REF_PTR parent, const char *name));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcCreateReference,
		     (const char *name));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindParentReference,
		     (X_IPC_REF_PTR child));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindFirstChild,
		     (X_IPC_REF_PTR parent));

IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindLastChild,
		     (X_IPC_REF_PTR parent));
IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindNextChild,
		     (X_IPC_REF_PTR child));
IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindPreviousChild,
		     (X_IPC_REF_PTR child));
IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindFailedReference,
		     (X_IPC_REF_PTR exception));
IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindTopLevelReference,
		     (X_IPC_REF_PTR child));
IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindAnteReferenceByName,
		     (X_IPC_REF_PTR child, const char *antecedentName));
IPC_EXTERN_FUNCTION( X_IPC_REF_PTR x_ipcFindChildByName,
		     (X_IPC_REF_PTR parent, const char *childName));

IPC_EXTERN_FUNCTION( void x_ipcKillTaskTree,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcKillSubTaskTree,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcDisplayTaskTree,
		     (X_IPC_REF_PTR ref));

/* 
 * Functions for handling monitors.
 */

IPC_EXTERN_FUNCTION( void x_ipcRegisterPointMonitor,
		     (const char *monitor, const char *condition,
		      const char *act));

IPC_EXTERN_FUNCTION( void x_ipcRegisterDemonMonitor,
		     (const char *monitor, const char *setUp, 
		      const char *act, const char *cancel));

IPC_EXTERN_FUNCTION( void x_ipcRegisterPollingMonitor,
		     (const char *monitor, const char *condition,
		      const char *act));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcPointMonitor,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcPointMonitorWithConstraints,
		     (X_IPC_REF_PTR ref, const char *name,
		      const void *data, int32 tplConstraint));

IPC_EXTERN_FUNCTION( INTERVAL_MONITOR_OPTIONS_PTR x_ipcCreateMonitorOptions,
		     (void));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcIntervalMonitor,
		     (const char *name, const void *data));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcIntervalMonitorWithConstraints,
		     (X_IPC_REF_PTR ref, const char *name, const void *data,
		      X_IPC_TIME_POINT_TYPE start,
		      X_IPC_TIME_POINT_TYPE expire,
		      INTERVAL_MONITOR_OPTIONS_PTR ops));

IPC_EXTERN_FUNCTION( X_IPC_RETURN_VALUE_TYPE x_ipcFireDemon,
		     (const char *name, int32 idValue, const void *data));

/* 
 * Functions for getting information about X_IPC.
 */

/* Get the version of X_IPC. */
IPC_EXTERN_FUNCTION(int32 x_ipcGetVersionMajor, (void));

IPC_EXTERN_FUNCTION(int32 x_ipcGetVersionMinor, (void));

IPC_EXTERN_FUNCTION( X_IPC_TIME_POINT_TYPE x_ipcDefaultTime,
		     (void));

/* For upward compatibility */
#define x_ipcDefaultTimeGlobal x_ipcDefaultTime

IPC_EXTERN_FUNCTION( int x_ipcGetServer,
		     (void));

/* For upward compatibility */
#define x_ipcGetServerGlobal x_ipcGetServer

IPC_EXTERN_FUNCTION( fd_set* x_ipcGetConnections,
		     (void));

IPC_EXTERN_FUNCTION( fd_set* x_ipcGetAcceptFds,
		     (void));

IPC_EXTERN_FUNCTION( int x_ipcGetMaxConnection,
		     (void));

/*
 * Functions for handling exceptions.
 */

IPC_EXTERN_FUNCTION( void x_ipcRetry,
		     (X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcByPassException,
		     ( X_IPC_REF_PTR ref));

IPC_EXTERN_FUNCTION( void x_ipcGetExceptionInfo,
		     (X_IPC_REF_PTR ref, EXC_INFO_PTR info));

IPC_EXTERN_FUNCTION( void x_ipcAddExceptionHandler,
		     (X_IPC_REF_PTR ref, const char *hndName));

IPC_EXTERN_FUNCTION( void x_ipcAddExceptionHandlerToMessage,
		     (const char *msgName, const char *hndName));

/* 
 * Functions for handling connections to multiple central servers. 
 */

IPC_EXTERN_FUNCTION(void x_ipcSetContext, (X_IPC_CONTEXT_PTR context));

IPC_EXTERN_FUNCTION(X_IPC_CONTEXT_PTR x_ipcGetContext, (void));

/* 
 * Functions for shutting down.
 */

IPC_EXTERN_FUNCTION( void x_ipcClose,
		     (void));

IPC_EXTERN_FUNCTION( void x_ipcCloseInternal,
		     (int));

#if defined(__cplusplus) /* C++ */
}
#endif

#if defined(VXWORKS)
void central(char *options);
#endif

#endif /* INCx_ipc */

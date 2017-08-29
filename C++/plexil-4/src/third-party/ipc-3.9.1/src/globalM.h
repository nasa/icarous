/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * MODULE: burying globals under a rock!
 *
 * FILE: global.h
 *
 * ABSTRACT: externs
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: globalM.h,v $
 * Revision 2.8  2010/12/17 19:20:23  reids
 * Split IO mutex into separate read and write mutexes, to help minimize
 *   probability of deadlock when reading/writing very big messages.
 * Fixed a bug in multi-threaded version where a timeout is not reported
 *   correctly (which could cause IPC_listenClear into a very long loop).
 *
 * Revision 2.7  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2002/06/25 16:44:59  reids
 * Fixed the way memory is freed when responses are handled;
 *   Allowed me to remove "responseIssuedGlobal".
 *
 * Revision 2.5  2002/01/03 20:52:12  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.4  2001/01/31 17:54:11  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
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
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.11  1997/03/07 17:49:39  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.3.2.10  1997/01/27 20:09:27  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.3.2.8  1997/01/16 22:17:25  reids
 * Added "totalMemBytes" to improve the way usage stats are reported.
 *
 * Revision 1.3.2.7  1997/01/11 01:20:58  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.3.2.6.4.2  1996/12/27 19:26:01  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.3.2.6.4.1  1996/12/24 14:41:36  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.3.2.6  1996/12/18 15:09:28  reids
 * Fixed bug in Lispworks/VxWorks handling of messages, where a message
 *   could be "stranded" if it arrived during a blocking query
 *
 * Revision 1.3.2.5  1996/10/29 14:52:05  reids
 * Make "byteOrder" and "alignment" vars available to C, not just LISP.
 *
 * Revision 1.3.2.4  1996/10/22 18:49:36  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.3.2.3  1996/10/18 18:03:05  reids
 * Added a few state vars, and moved one from server to module.
 *
 * Revision 1.3.2.2  1996/10/08 14:23:35  reids
 * Changes for IPC operating under Lispworks on the PPC.  Mainly changes
 * (LISPWORKS_FFI_HACK) due to the fact that Lispworks on the PPC is currently
 * missing the foreign-callable function.
 *
 * Revision 1.3.2.1  1996/10/02 20:58:29  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.4  1996/09/06 22:30:21  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.3  1996/05/24 16:45:53  reids
 * Removed all (most?) of the task-tree related code from the IPC build.
 *
 * Revision 1.2  1996/05/09 16:53:40  reids
 * Remove (conditionally) references to matrix format.
 *
 * Revision 1.1  1996/05/09 01:01:25  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/24 19:11:04  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.2  1996/03/12 03:19:43  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:27  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.41  1996/03/05  05:04:29  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.40  1996/02/21  18:30:21  rich
 * Created single event loop.
 *
 * Revision 1.39  1996/02/06  19:04:43  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.38  1996/01/05  16:31:20  rich
 * Added windows NT port.
 *
 * Revision 1.37  1995/10/29  18:26:48  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.36  1995/10/25  22:48:22  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.35  1995/10/07  19:07:18  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.34  1995/07/10  16:17:26  rich
 * Interm save.
 *
 * Revision 1.33  1995/07/06  21:16:25  rich
 * Solaris and Linux changes.
 *
 * Revision 1.32  1995/06/14  03:21:36  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.31  1995/05/31  19:35:37  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.30  1995/04/21  03:53:23  rich
 * Added central commands to kill the task tree and close a module.
 * Added x_ipcGetContext and x_ipcSetContext to support connections to multiple
 * central servers.  x_ipcConnectModules can be called multiple times.
 * Fixed a bug in the resource limit pending.
 * Created seperate routines to print help and option messages.
 *
 * Revision 1.29  1995/04/19  14:28:15  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.28  1995/04/07  05:03:08  rich
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
 * Revision 1.27  1995/04/05  19:10:49  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.26  1995/04/04  19:42:13  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.25  1995/03/30  15:43:02  rich
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
 * Revision 1.24  1995/03/28  01:14:32  rich
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
 * Revision 1.23  1995/03/19  19:39:30  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.22  1995/01/30  16:18:01  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.21  1995/01/18  22:40:34  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.20  1994/11/02  21:34:13  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.19  1994/05/31  03:24:01  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.18  1994/05/25  04:57:22  rich
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
 * Revision 1.17  1994/05/17  23:15:53  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.16  1994/05/05  00:49:35  rich
 * Removed duplicate includes.
 *
 * Revision 1.15  1994/05/05  00:46:17  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.14  1994/04/28  16:16:09  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.13  1994/04/26  16:23:30  rich
 * Now you can register an exit handler before anything else and it will
 * get called if connecting to central fails.
 * Also added code to handle pipe breaks during writes.
 *
 * Revision 1.12  1994/04/16  19:42:17  rich
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
 * Revision 1.11  1993/12/14  17:33:44  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.10  1993/11/21  20:17:56  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.9  1993/10/21  16:13:53  rich
 * Fixed compiler warnings.
 *
 * Revision 1.8  1993/10/20  19:00:35  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.7  1993/08/30  23:14:00  fedor
 * Added SUN4 as well as sun4 compile flag.
 * Corrected Top level failure message name display with a define in dispatch.c
 *
 * Revision 1.6  1993/08/30  21:53:32  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.5  1993/08/27  08:38:39  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.4  1993/08/27  07:14:59  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  16:01:28  rich
 * Changed the global include files so that they don't double include
 * system files.  This was causing problems on the mach machines.
 *
 * Revision 1.2  1993/08/20  00:26:06  fedor
 * commiting others changes
 *
 * Revision 1.1.1.1  1993/05/20  05:45:43  rich
 * Importing x_ipc version 8
 *
 * Revision 8.1  1993/05/20  00:29:59  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:55  fedor
 * Added Logging.
 *
 * $Revision: 2.8 $
 * $Date: 2010/12/17 19:20:23 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCglobalM
#define INCglobalM

#include "libc.h"

#include "tca.h"
#include "basics.h"
#include "tcerror.h"
#include "strList.h"
#ifdef DOS_FILE_NAMES
#include "tcaInter.h"
#ifndef NMP_IPC
#include "tcaMatri.h"
#endif
#include "formatte.h"
#else
#include "tcaInternal.h"
#ifndef NMP_IPC
#include "tcaMatrix.h"
#endif
#include "formatters.h"
#endif /* DOS_FILE_NAMES */

#include "datamsg.h"
#include "idtable.h"
#include "hash.h"
#include "list.h"

#include "key.h"

#ifdef DOS_FILE_NAMES
#include "behavior.h"
#else
#include "behaviors.h"
#endif

#include "resMod.h"
#ifndef NMP_IPC
#include "monMod.h"
#endif /* NMP_IPC */

#include "tcaMem.h"

#include "dispatch.h"
#include "com.h"
#ifdef DOS_FILE_NAMES
#include "comModul.h"
#else
#include "comModule.h"
#endif /* DOS_FILE_NAMES */
#include "tcaRef.h"
#include "recvMsg.h"
#include "sendMsg.h"
#include "reg.h"
#ifdef DOS_FILE_NAMES
#include "centralM.h"
#else
#include "centralMsg.h"
#endif /* DOS_FILE_NAMES */
#include "logging.h"
#ifdef DOS_FILE_NAMES
#include "modLoggi.h"
#else
#include "modLogging.h"
#endif /* DOS_FILE_NAMES */
#include "modVar.h"

#ifdef NMP_IPC
#include "lex.h"
#endif

#ifdef THREADED
#include "multiThread.h"

#define   LOCK_M_MUTEX  lockMutex(&GET_M_GLOBAL(mutex))
#define UNLOCK_M_MUTEX  unlockMutex(&GET_M_GLOBAL(mutex))
#define   LOCK_C_MUTEX  lockMutex(&GET_C_GLOBAL(mutex))
#define UNLOCK_C_MUTEX  unlockMutex(&GET_C_GLOBAL(mutex))
#define   LOCK_CM_MUTEX { LOCK_M_MUTEX; LOCK_C_MUTEX; }
#define UNLOCK_CM_MUTEX { UNLOCK_M_MUTEX; UNLOCK_C_MUTEX; }
#define   LOCK_IO_MUTEX { MUTEX_PTR mutex; LOCK_CM_MUTEX; \
 mutex = &GET_C_GLOBAL(ioMutex); UNLOCK_CM_MUTEX; lockMutex(mutex); }
#define UNLOCK_IO_MUTEX \
{ LOCK_CM_MUTEX; unlockMutex(&GET_C_GLOBAL(ioMutex)); UNLOCK_CM_MUTEX; }
#define   LOCK_IO_READ_MUTEX { MUTEX_PTR mutex; LOCK_CM_MUTEX; \
 mutex = &GET_C_GLOBAL(readMutex); UNLOCK_CM_MUTEX; lockMutex(mutex); }
#define UNLOCK_IO_READ_MUTEX \
{ LOCK_CM_MUTEX; unlockMutex(&GET_C_GLOBAL(readMutex)); UNLOCK_CM_MUTEX; }

#define   LOCK_SELECT_MUTEX \
{ LOCK_M_MUTEX; lockMutex(&GET_M_GLOBAL(selectMutex)); UNLOCK_M_MUTEX; }
#define UNLOCK_SELECT_MUTEX \
{ LOCK_M_MUTEX; unlockMutex(&GET_M_GLOBAL(selectMutex)); UNLOCK_M_MUTEX; }

#else
#define   LOCK_M_MUTEX
#define UNLOCK_M_MUTEX
#define   LOCK_C_MUTEX
#define UNLOCK_C_MUTEX
#define   LOCK_CM_MUTEX
#define UNLOCK_CM_MUTEX
#define   LOCK_IO_MUTEX
#define UNLOCK_IO_MUTEX
#define   LOCK_IO_READ_MUTEX
#define UNLOCK_IO_READ_MUTEX
#define   LOCK_SELECT_MUTEX
#define UNLOCK_SELECT_MUTEX
#endif

/***************************************/

#ifdef LISP
#define IS_LISP_MODULE() (GET_M_GLOBAL(lispFlagGlobal) != '\0')
#define LISP_DATA_FLAG() (&GET_M_GLOBAL(lispFlagGlobal))

void set_Is_Lisp_Module(void);

#else /* !LISP */
#define IS_LISP_MODULE() FALSE /* No-op */
#endif /* !LISP */

#ifdef LISPWORKS_FFI_HACK
typedef enum { ExecHnd_Idle=0, ExecHnd_Pending=1,
	       ExecHnd_Handling=2 } EXECHND_STATE_ENUM;

typedef struct {
  EXECHND_STATE_ENUM state;
  X_IPC_REF_PTR x_ipcRef;
  const char *hndName;
  void *data;
  void *clientData;
  CONNECTION_PTR connection;
  MSG_PTR msg;
  int tmpParentRef;
  int tmpResponse;
} LISPWORKS_HACK_TYPE, *LISPWORKS_HACK_PTR;
#endif /* LISPWORKS_FFI_HACK */

typedef struct _X_IPC_CONTEXT {
  fd_set x_ipcConnectionListGlobal;
  fd_set x_ipcListenMaskGlobal;
  const char *servHostGlobal;

  HASH_TABLE_PTR moduleConnectionTable;
  HASH_TABLE_PTR handlerTable;
  HASH_TABLE_PTR messageTable;
  HASH_TABLE_PTR resourceTable;
  ID_TABLE_PTR hndIdTable;
  ID_TABLE_PTR msgIdTable;
  LIST_PTR pendingReplies;
  MSG_QUEUE_TYPE msgQueue;
  LIST_PTR x_ipcRefFreeList;
  int maxConnection;
  int32 parentRefGlobal; 
  int32 sendMessageRefGlobal;
  int32 listenPortNum;
  int listenPort;
  int listenSocket;
  int serverRead;
  int serverWrite;
  int32 willListen;
  STR_LIST_PTR tappedMsgs;
  STR_LIST_PTR broadcastMsgs;
  BOOLEAN directDefault;
  BOOLEAN valid;
  LIST_PTR queryNotificationList;
  LIST_PTR connectNotifyList;
  LIST_PTR disconnectNotifyList;
  LIST_PTR changeNotifyList;
#ifdef THREADED
  MUTEX_TYPE mutex;
  MUTEX_TYPE ioMutex;
  MUTEX_TYPE readMutex;
#endif
#ifdef LISPWORKS_FFI_HACK
  LISPWORKS_HACK_TYPE execHndState;
#endif /* LISPWORKS_FFI_HACK */
} X_IPC_CONTEXT_TYPE;

typedef struct _GM {
  DATA_MSG_BUF_PTR bufferToAlloc;
  CONST_FORMAT_PTR byteFormat;
  CONST_FORMAT_PTR charFormat;
  CONST_FORMAT_PTR doubleFormat;
  CONST_FORMAT_PTR floatFormat;
  CONST_FORMAT_PTR intFormat;
  CONST_FORMAT_PTR longFormat;
  CONST_FORMAT_PTR shortFormat;
  HASH_TABLE_PTR classFormatTable;
  HASH_TABLE_PTR formatNamesTable;
  LIST_ELEM_PTR listCellFreeListGlobal;
  LIST_PTR dataMsgBufferList;
  LIST_PTR listFreeListGlobal;
  X_IPC_TIME_POINT_TYPE x_ipcDefaultTimeGlobal;
  X_IPC_VERSION_TYPE versionGlobal;
#if defined(DBMALLOC)
  void *(*x_ipcMallocMemHnd)(const char*, int, SIZETYPE);
#elif defined(__sgi)
  void *(*x_ipcMallocMemHnd)(unsigned int);
#else
  void *(*x_ipcMallocMemHnd)(size_t size);
#endif
  const char *modNameGlobal, *servHostGlobal;
  TRANSLATE_TYPE TransTable[MAXFORMATTERS+1];

  int32 DMFree, DMTotal, DMmin, DMmax;
  int32 directFlagGlobal;
  int32 expectedWaitGlobal;
  int32 freeMemRetryAmount;
  int32 inconsistentConstraintsGlobal[2];
  int32 mallocMemRetryAmount;
  BOOLEAN pipeBroken;
  unsigned long totalMemRequest;
  unsigned long totalMemBytes;
  int32 indentGlobal;
  void (*dPrintBYTE_FN)(FILE *, Print_Data_Ptr, const char *, int32);
  void (*dPrintUBYTE_FN)(FILE *, Print_Data_Ptr, const char *, int32);
  void (*dPrintCHAR_FN)(FILE *, Print_Data_Ptr, const char *, int32);
  void (*dPrintDOUBLE_FN)(FILE *, Print_Data_Ptr, const double *, int32);
  void (*dPrintFLOAT_FN)(FILE *, Print_Data_Ptr, const float *, int32);
  void (*dPrintFORMAT_FN)(FILE *, Print_Data_Ptr, CONST_FORMAT_PTR, int32);
  void (*dPrintINT_FN)(FILE *, Print_Data_Ptr, const int32 *, int32);
  void (*dPrintBOOLEAN_FN)(FILE *, Print_Data_Ptr, const int32 *, int32);
  void (*dPrintLONG_FN)(FILE *, Print_Data_Ptr, const long *, int32);
#ifndef NMP_IPC
  void (*dPrintMAP_FN)(FILE *, const genericMatrix *, CONST_FORMAT_PTR,
		       int, const void *);
#endif
  void (*dPrintSHORT_FN)(FILE *, Print_Data_Ptr, const int16 *, int32);
  void (*dPrintSTR_FN)(FILE *, Print_Data_Ptr, const char *, int32);
  void (*dPrintX_IPC_FN)(FILE *, Print_Data_Ptr,
			 const X_IPC_REF_TYPE *, int32);
  void (*dPrintTWOBYTE_FN)(FILE *, Print_Data_Ptr, const char *, int32);
  void (*dPrintUSHORT_FN)(FILE *, Print_Data_Ptr,
			  const unsigned short *, int32);
  void (*dPrintUINT_FN)(FILE *, Print_Data_Ptr, const unsigned int *, int32);
  void (*dPrintULONG_FN)(FILE *, Print_Data_Ptr, const unsigned long *, int32);

  void (*x_ipcExitHnd)(void);
  void (*x_ipcFreeMemoryHnd)(u_int32);
  int32 enableDistributedResponses;
  int32 willListen;
  int32 byteOrder;
  ALIGNMENT_TYPE alignment;
  X_IPC_REF_PTR x_ipcRootNodeGlobal;
  
  int32 sizeDM;
  int32 numAllocatedDM;

  LOG_PTR logList[3];
  LIST_PTR Message_Ignore_Set;

  char *Found_Key;

  fd_set externalMask;
  HASH_TABLE_PTR externalFdTable;
  
  X_IPC_CONTEXT_PTR currentContext;

  STR_LIST_PTR requiredResources;

  LIST_PTR moduleList;

#ifdef NMP_IPC
  /* This used to be declared static, but has to be here to prevent
     conflicts between vxworks tasks */
  LIST_PTR timerList;
#endif /* NMP_IPC */
#ifdef THREADED
  MUTEX_TYPE mutex;
  MUTEX_TYPE selectMutex;
  PING_THREAD_TYPE ping;
#endif
#ifdef LISP
  /* LISP-RELATED STUFF */
  char lispFlagGlobal;
  X_IPC_REF_PTR lispRefSaveGlobal;
  int32 (*lispBufferSizeGlobal)(int32 *, CONST_FORMAT_PTR);
  long (*lispDecodeMsgGlobal)(CONST_FORMAT_PTR, BUFFER_PTR);
  long (*lispEncodeMsgGlobal)(CONST_FORMAT_PTR, BUFFER_PTR);
  int32 (*lispExitGlobal)(void);
#ifdef NMP_IPC
  int32 (*lispQueryResponseGlobal)(char *, CONST_FORMAT_PTR);
#endif
#endif /* LISP */
} GM_TYPE, *GM_PTR;

#if defined(VXWORKS)
/* VX works needs to access the globals through a pointer. */

#define GET_M_GLOBAL(var) (x_ipc_gM->var)
#define mGlobalp() ((x_ipc_gM != NULL) && (x_ipc_gM->currentContext != NULL))

extern GM_PTR x_ipc_gM;

#else
/* In general, the globals can just be statically allocated. */

#define GET_M_GLOBAL(var) (x_ipc_gM.var)
#define mGlobalp() ((x_ipc_gM_ptr != NULL) && (x_ipc_gM.currentContext != NULL))

extern GM_TYPE x_ipc_gM;
extern GM_PTR  x_ipc_gM_ptr;
#endif

#define GET_C_GLOBAL(var) (GET_M_GLOBAL(currentContext->var))

void x_ipc_globalMInit(void);
void x_ipc_globalMFree(void);
void x_ipc_globalMInvalidate(void);
BOOLEAN x_ipc_isValidServerConnection(void);

#endif /* INCglobalM */

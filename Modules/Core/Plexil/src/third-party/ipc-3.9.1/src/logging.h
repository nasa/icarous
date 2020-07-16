/*****************************************************************************
 * 
 * PROJECT: Task Control Architecture.
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: logging
 *
 * FILE: logging.h
 *
 * ABSTRACT:
 * External declarations for message and status logging facilities.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: logging.h,v $
 * Revision 2.4  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:26  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.2  1996/12/18 15:12:56  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.3.2.1  1996/10/18 18:09:23  reids
 * Distinguish when ref and parent id's should be logged.
 *
 * Revision 1.3  1996/09/03 17:01:23  rouquett
 * increased MAX_LOG_FILE_NAME_LENGTH
 *
 * Revision 1.2  1996/08/30 22:37:16  rouquett
 * Added a field in LOG_PTR to turn off adding comments to the log file
 *
 * Revision 1.1  1996/05/09 01:01:37  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:51  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.14  1996/07/19  18:14:14  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.13  1995/05/31  19:35:54  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.12  1995/03/30  15:43:27  rich
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
 * Revision 1.11  1995/03/28  01:14:44  rich
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
 * Revision 1.10  1995/01/18  22:41:12  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.9  1994/10/25  17:10:01  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.8  1994/05/17  23:16:20  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.7  1994/04/28  22:16:58  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 * Revision 1.6  1994/04/28  16:16:20  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.5  1993/12/01  18:03:51  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:18:18  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:15:31  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:06  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:45  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:31  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:31  fedor
 * Added Logging.
 *
 *  5-Jan-93 Domingo Gallardo, School of Computer Science, CMU
 * Added field parentId to LOG_TYPE.
 * 
 *  6-Oct-89 Christopher Fedor, School of Computer Science, CMU
 * Added message handle time summary.
 *
 *  3-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Added code for ignoring registration msgs.
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.4 $
 * $Date: 2009/05/04 19:03:41 $
 * $Author: reids $
 *
 *****************************************************************************/

/*********************************************************************
 *                                                                   *
 * logging.h                                                         *
 *                                                                   *
 *                                                                   *
 * Functions:                                                        *
 *   The following three functions take a variable number of         *
 *   arguments, just like printf                                     *
 *                                                                   *
 *   Log (format_string, ...<args>) :                                *
 *     logs unconditionally onto the terminal and into the log file  *
 *     file (if it is open).                                         *
 *                                                                   *
 *   Log_Message (format_string, ...<args>) :                        *
 *     logs onto the terminal if Terminal_Log_Message is not 0;      *
 *     logs into the Log_File if File_Log_Message is not 0;          *
 *                                                                   *
 *   Log_Status (format_string, ...<args>) :                         *
 *     logs onto the terminal if Terminal_Log_Status is not 0;       *
 *     logs into the Log_File if File_Log_Status is not 0;           *
 *                                                                   *
 *   Log_Time (indent) :                                             *
 *     logs the time (hh:mm:sec.milli), indenting "indent" spaces.   *
 *     Logs onto the terminal if Terminal_Log_Time is not 0;         *
 *     Logs into the Log_File if File_Log_Time is not 0.             *
 *                                                                   *
 *   Log_Data (format, data, indent) :                               *
 *     logs the "data" using the given "format", indenting each line *
 *     by "indent".  Assumes a line length of 80 and truncates after *
 *     5 lines of output (these can be changed by setting the        *
 *     variables Line_Length and Print_Length, respectively).        *
 *     Logs onto the terminal if Terminal_Log_Data is not 0;         *
 *     Logs into the Log_File if File_Log_Data is not 0;             *
 *                                                                   *
 *   Start_Ignore_Logging () :                                       *
 *     starting now, don't log messages or status reports            *
 *     on the terminal if Terminal_Log_Ignore is TRUE, and           *
 *     don't log in file if File_Log_Ignore is TRUE.                 *
 *                                                                   *
 *   End_Ignore_Logging () :                                         *
 *     reinstate logging messages and status reports                 *
 *                                                                   *
 *   Start_Terminal_Logging () :                                     *
 *     Prints out the X_IPC header and version number.                 *
 *                                                                   *
 *   Start_File_Logging () :                                         *
 *     If either File_Log_Messages or File_Log_Status are non-zero,  *
 *     opens the file given by Log_File_Name and prints out the X_IPC  *
 *     header and version number.                                    *
 *     Sets signal functions to trap errors and close log file       *
 *     Gives user option to enter initial comments to file.          *
 *                                                                   *
 *   End_File_Logging () :                                           *
 *     Gives user option to enter final comments to file.            *
 *     Close the Log_File (if open).                                 *
 *                                                                   *
 *                                                                   *
 *********************************************************************/

#ifndef INClogging
#define INClogging

/* What type of message is this a part of */
typedef enum { LOGGING_ALWAYS, LOGGING_MESSAGE, LOGGING_STATUS} LOG_STATUS_ENUM;

typedef struct {
  FILE *theFile;
  int32 messages;
  int32 status;
  int32 time;
  int32 data;
  int32 summary;
  int32 refId;
  int32 parentId;
  int32 quiet;
  int32 flush;
  int32 ignore;
  int32 ignoreNow;
  int32 addComments;
} LOG_TYPE, *LOG_PTR; 

#define MAX_LOG_FILE_NAME_LENGTH 255

/*extern char Log_File_Name[MAX_LOG_FILE_NAME_LENGTH];*/

/* It turns out that compiling optimized with Greenhills causes vprintf 
   to crash -- need to define macros for warnings & errors on VxWorks */
#ifndef VXWORKS
#define LOG(description) Log(description)
#define LOG1(description, arg1) Log(description, arg1)
#define LOG2(description, arg1, arg2) Log(description, arg1, arg2)
#define LOG3(description, arg1, arg2, arg3) \
  Log(description, arg1, arg2, arg3)

#define LOG_MESSAGE(description) Log_Message(description)
#define LOG_MESSAGE1(description, arg1) Log_Message(description, arg1)
#define LOG_MESSAGE2(description, arg1, arg2) \
  Log_Message(description, arg1, arg2)
#define LOG_MESSAGE3(description, arg1, arg2, arg3) \
  Log_Message(description, arg1, arg2, arg3)

#define LOG_STATUS(description) Log_Status(description)
#define LOG_STATUS1(description, arg1) Log_Status(description, arg1)
#define LOG_STATUS2(description, arg1, arg2) \
  Log_Status(description, arg1, arg2)
#define LOG_STATUS3(description, arg1, arg2, arg3) \
  Log_Status(description, arg1, arg2, arg3)

#else /* !VXWORKS */
#define MAX_LOG 300
#define LOG(description) Log(description)
#define LOG1(description, arg1) \
  { char str[MAX_LOG]; snprintf(str, MAX_LOG, description, arg1); Log(str); }
#define LOG2(description, arg1, arg2) \
  { char str[MAX_LOG];\
    snprintf(str, MAX_LOG, description, arg1, arg2); Log(str); }
#define LOG3(description, arg1, arg2, arg3) \
  { char str[MAX_LOG];\
    snprintf(str, MAX_LOG, description, arg1, arg2, arg3); Log(str); }

#define LOG_MESSAGE(description) Log_Message(description)
#define LOG_MESSAGE1(description, arg1) \
  { char str[MAX_LOG];\
    snprintf(str, MAX_LOG, description, arg1); Log_Message(str); }
#define LOG_MESSAGE2(description, arg1, arg2) \
  { char str[MAX_LOG]; \
    snprintf(str, MAX_LOG, description, arg1, arg2); Log_Message(str); }
#define LOG_MESSAGE3(description, arg1, arg2, arg3) \
  { char str[MAX_LOG]; \
    snprintf(str, MAX_LOG, description, arg1, arg2, arg3); Log_Message(str); }

#define LOG_STATUS(description) Log_Status(description)
#define LOG_STATUS1(description, arg1) \
  { char str[MAX_LOG];\
    snprintf(str, MAX_LOG, description, arg1); Log_Status(str); }
#define LOG_STATUS2(description, arg1, arg2) \
  { char str[MAX_LOG]; \
    snprintf(str, MAX_LOG, description, arg1, arg2); Log_Status(str); }
#define LOG_STATUS3(description, arg1, arg2, arg3) \
  { char str[MAX_LOG]; \
    snprintf(str, MAX_LOG, description, arg1, arg2, arg3); Log_Status(str); }

#endif /* !VXWORKS */

void Log(const char *format_string, ...);
void Log_Message(const char *format_string, ...);
void Log_Status(const char *format_string, ...);

void LogHandleSummary(long arg1, float arg2, float arg3, float arg4,
		      long arg5, long arg6, long arg7, long arg8);

void Log_Time(int32 indent);
void Log_ParentId (int32 parent_id, LOG_STATUS_ENUM logStatus);
void Log_RefId (DISPATCH_PTR dispatch, LOG_STATUS_ENUM logStatus);
void Log_Data(CONST_FORMAT_PTR Format, const void *Data, int32 indent);

extern int32 Ignore_Logging_Message (MSG_PTR Message);
extern void Add_Message_To_Ignore(const char *name);
extern void ignoreLoggingHnd(DISPATCH_PTR dispatch, char **msgName);
extern void resumeLoggingHnd(DISPATCH_PTR dispatch, char **msgName);

void Start_Ignore_Logging(void);
void End_Ignore_Logging(void);
void Start_Terminal_Logging(void);
void Start_File_Logging(void);
void End_File_Logging(void);

#endif /* INClogging */

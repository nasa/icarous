/*****************************************************************************
 * 
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: logging
 *
 * FILE: logging.c
 *
 * ABSTRACT:
 * Facilities for logging X_IPC data on the terminal or in a log file.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: logging.c,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:26  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.3.2.8  1997/01/27 20:09:41  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.3.2.6  1997/01/21 17:20:23  reids
 * Re-re-fixed the logic of "x_ipc_LogIgnoreP" (needed an extra
 *   function: LogIgnoreAllP).
 *
 * Revision 1.3.2.5  1997/01/16 22:19:21  reids
 * Made "-s" (silent) option work.
 *
 * Revision 1.3.2.4  1996/12/18 15:12:54  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.3.2.3  1996/10/18 18:09:25  reids
 * Distinguish when ref and parent id's should be logged.
 *
 * Revision 1.3.2.2  1996/10/07 20:15:10  reids
 * Added ability to specify, at the command line, messages for central to
 *   ignore logging.
 *
 * Revision 1.3.2.1  1996/10/02 20:57:08  reids
 * Don't buffer log file under VxWorks
 *
 * Revision 1.4  1996/09/06 22:30:33  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.3  1996/08/30 22:44:15  rouquett
 * noComments -> addComments in LOG_PTR typo
 *
 * Revision 1.2  1996/08/30 22:37:15  rouquett
 * Added a field in LOG_PTR to turn off adding comments to the log file
 *
 * Revision 1.1  1996/05/09 01:01:37  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/04/24 19:11:07  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.1  1996/03/03 04:31:49  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.28  1996/07/19  18:14:12  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.27  1996/07/03  21:41:46  reids
 * Somehow was passing variable of wrong type into gettimeofday.
 *
 * Revision 1.26  1996/06/30  20:17:44  reids
 * Handling of polling monitors was severely broken.
 *
 * Revision 1.25  1996/06/25  20:50:55  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.24  1996/05/09  18:31:09  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.23  1996/02/10  16:50:08  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.22  1996/02/07  00:27:37  rich
 * Add prefix to VERSION_DATE and COMMIT_DATE.
 *
 * Revision 1.21  1996/02/06  19:04:56  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.20  1996/01/05  16:31:31  rich
 * Added windows NT port.
 *
 * Revision 1.19  1995/12/17  20:21:38  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.18  1995/10/29  18:26:50  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.17  1995/07/10  16:17:44  rich
 * Interm save.
 *
 * Revision 1.16  1995/04/07  05:03:24  rich
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
 * Revision 1.15  1995/03/30  15:43:21  rich
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
 * Revision 1.14  1995/03/28  01:14:42  rich
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
 * Revision 1.13  1995/03/18  15:11:04  rich
 * Fixed updateVersion script so it can be run from any directory.
 *
 * Revision 1.12  1995/01/18  22:41:09  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.11  1994/11/02  21:34:29  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.10  1994/10/25  17:06:15  reids
 * Changed the logging functions to accept variable number of arguments.
 * Fixed the way the "ignore" logging option worked.
 *
 * Revision 1.9  1994/04/28  22:16:55  rich
 * Added very simple stdin interface to central.  You can type command line
 * argument into standard in while central is running.  One option per
 * line.  In addition, it understands "quit" and "help".
 *
 * Revision 1.8  1994/04/28  16:16:18  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.7  1994/04/16  19:42:28  rich
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
 * Revision 1.6  1994/03/28  02:23:05  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.5  1993/12/14  17:34:02  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.4  1993/11/21  20:18:16  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:15:29  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:04  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:25  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:29  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:29  fedor
 * Added Logging.
 *
 *  5-Jan-93 Domingo Gallardo, School of Computer Science, CMU
 * Added the function Log_parentId
 *
 *  3-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Added code for ignoring registration msgs.
 *
 *  2-Aug-89 Reid Simmons, School of Computer Science, CMU
 * Created.
 *
 *
 * Functions:
 *   The following three functions take a variable number 
 *   arguments, just like printf
 *
 *   Log (format_string, ...<args>) :
 *     logs unconditionally onto the terminal and into the log file
 *     file (if it is open).
 *
 *   Log_Message (format_string, ...<args>) :
 *     logs onto the terminal if Terminal_Log_Message is not 0;
 *     logs into the Log_File if File_Log_Message is not 0;
 *
 *   Log_Status (format_string, ...<args>) :
 *     Logs onto the terminal if Terminal_Log_Status is not 0;
 *     Logs into the Log_File if File_Log_Status is not 0.
 *     Flushes log file.
 *
 *   Log_Time (indent) :
 *     Logs the time (hh:mm:sec.milli), indenting "indent" spaces.
 *     Logs onto the terminal if Terminal_Log_Time is not 0;
 *     Logs into the Log_File if File_Log_Time is not 0.
 *
 *   Log_Data (format, data, indent) :
 *     Logs the "data" using the given "format", indenting each line
 *     by "indent".  Assumes a line length of 80 and truncates after
 *     5 lines of output (these can be changed by setting the
 *     variables Line_Length and Print_Length, respectively).
 *     Logs onto the terminal if Terminal_Log_Data is not 0;
 *     Logs into the Log_File if File_Log_Data is not 0
 *
 *   Start_Ignore_Logging () :
 *     Starting now, don't log messages or status reports
 *     on the terminal if Terminal_Log_Ignore is TRUE, and
 *     don't log in file if File_Log_Ignore is TRUE.
 *
 *   End_Ignore_Logging () :
 *     Reinstate logging messages and status reports
 *
 *   Start_Terminal_Logging () :
 *     Prints out the X_IPC header and version number.
 *
 *   Start_File_Logging () :
 *     If either File_Log_Messages or File_Log_Status are non-zero,
 *     opens the file given by Log_File_Name and prints out the X_IPC
 *     header and version number.
 *     Sets signal functions to trap errors and close log file
 *     Gives user option to enter initial comments to file.
 *
 *   End_File_Logging () :
 *     Gives user option to enter final comments to file.
 *     Close the Log_File (if open).
 *
 *   BOOLEAN x_ipc_LogDataP () :
 *     Returns true if any of the logs have data logging turned on.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"

#include <stdarg.h>

/* fflush does not flush the buffer out to NFS; This should do the trick */
#ifdef VXWORKS
#include "ioLib.h"
#define fflush(file) ioctl(fileno(file), FIOSYNC, 0);
#endif

void Log(const char *format_string, ...)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  va_list args;
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if (!(*log)->quiet) {
      va_start(args, format_string);
      vfprintf((*log)->theFile, (char *)format_string, args);
      va_end(args);
      if ((*log)->flush)
	fflush((*log)->theFile);
    }
  }
}

void Log_Message(const char *format_string, ...)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  va_list args;
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++){
    if ((*log)->messages && !(*log)->ignoreNow && !(*log)->quiet) {
      va_start(args, format_string);
      vfprintf((*log)->theFile, (char *)format_string, args);
      va_end(args);
      if ((*log)->flush)
	fflush((*log)->theFile);
    }
  }
}

void Log_Status(const char *format_string, ...)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  va_list args;
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++){
    if ((*log)->status && !(*log)->ignoreNow && !(*log)->quiet) {
      va_start(args, format_string);
      vfprintf((*log)->theFile, (char *)format_string, args);
      va_end(args);
      if ((*log)->flush)
	fflush((*log)->theFile);
    }
  }
}

void LogHandleSummary(long arg1, float arg2, float arg3, float arg4,
		      long arg5, long arg6, long arg7, long arg8)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->summary && !(*log)->ignoreNow && !(*log)->quiet) {
      fprintf((*log)->theFile,
	      "%5ld: Msg: %2.2f%%, Mon: %2.2f%%, Wait %2.2f%%, Msg: (%ld, %ld) Ave: (%ld, %ld)\n",
	      arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
      if ((*log)->flush)
	fflush((*log)->theFile);
    }
  }
}

#if VXWORKS
void printTimeFromTicks(FILE *fp, int indent)
{
  int  hour, min, sec, hund, tmp;
  unsigned long msec;

  msec = x_ipc_timeInMsecs();
  
  tmp = msec/10;
  hund = (tmp)%100;
  tmp /= 100;
  sec =  (tmp)%60;
  tmp /= 60;
  min =  (tmp)%60;
  tmp /= 60;
  hour = tmp;
  
  fprintf(fp, "%*d:%02d:%02d.%02d", indent+2,hour,min,sec,hund);
}
#endif

#ifdef _WINSOCK_
void printSystemTime(FILE *fp, int indent)
{
  SYSTEMTIME theTime;
  GetSystemTime(&theTime);
  fprintf(fp, "%*d:%02d:%02d.%02d", indent+2,
	  theTime.wHour,
	  theTime.wMinute,
	  theTime.wSecond,
	  theTime.wMilliseconds*10);
}
#endif

void Log_Time(int32 indent)
{
  LOG_PTR *log;
#if !defined(VXWORKS) && !defined(_WINSOCK_)
  int hundredths;
  struct timeval timeBlock;
  struct tm *localTime;
#endif
  
  log = GET_M_GLOBAL(logList);
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->time && !(*log)->ignoreNow && !(*log)->quiet) {
      
#if !defined(VXWORKS) && !defined(_WINSOCK_)
      (void)gettimeofday(&timeBlock, NULL);
#if defined(THINK_C) || defined(macintosh)
      { time_t time = (time_t)timeBlock.tv_sec;
	localTime = localtime(&time);
      }
#else
      localTime = localtime((time_t *)&(timeBlock.tv_sec));
#endif /* THINK_C || macintosh */
      hundredths = timeBlock.tv_usec/10000;
      
      fprintf((*log)->theFile, "%*d:%02d:%02d.%02d", indent+2,
	      localTime->tm_hour, localTime->tm_min,
	      localTime->tm_sec, hundredths);
#elif defined(VXWORKS)
      printTimeFromTicks((*log)->theFile, indent);
#elif defined(_WINSOCK_)
      printSystemTime((*log)->theFile, indent);
#endif
      
      if ((*log)->flush)
	fflush((*log)->theFile);
    }
  }
}


void Log_ParentId(int32 parent_id, LOG_STATUS_ENUM logStatus)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->parentId && !(*log)->ignoreNow && !(*log)->quiet &&
	(logStatus == LOGGING_STATUS ? (*log)->status :
	 logStatus == LOGGING_MESSAGE ? (*log)->messages : TRUE)) {
      fprintf((*log)->theFile," {%d}", parent_id);
    }
    if ((*log)->flush)
      fflush((*log)->theFile);
  }
}

void Log_RefId(DISPATCH_PTR dispatch, LOG_STATUS_ENUM logStatus)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  
  if (dispatch != NULL) {
    for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
      if ((*log)->refId && !(*log)->ignoreNow && !(*log)->quiet &&
	  (logStatus == LOGGING_STATUS ? (*log)->status :
	   logStatus == LOGGING_MESSAGE ? (*log)->messages : TRUE)) {
	fprintf((*log)->theFile," {%d}", dispatch->locId);
      }
      if ((*log)->flush)
	fflush((*log)->theFile);
    }
  }
}

void Log_Data(CONST_FORMAT_PTR Format, const void *Data, int32 indent)
{ 
  int tmp;
  LOG_PTR *log = GET_M_GLOBAL(logList);
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if (Data) {
      tmp = GET_M_GLOBAL(indentGlobal);
      GET_M_GLOBAL(indentGlobal) = indent;
      if ((*log)->data && !(*log)->ignoreNow && !(*log)->quiet){
	Print_Formatted_Data((*log)->theFile, Format, Data);
	if ((*log)->flush)
	  fflush((*log)->theFile); }
      GET_M_GLOBAL(indentGlobal) = tmp;
    }
    else {
      if ((*log)->data && !(*log)->ignoreNow && !(*log)->quiet){
	fprintf((*log)->theFile, "%*s\n", indent+7, "NO DATA");
	if ((*log)->flush)
	  fflush((*log)->theFile);
      }
    }
  }
}

int32 Ignore_Logging_Message (MSG_PTR Message)
{
  return x_ipc_listMemberItem((void *)Message, GET_M_GLOBAL(Message_Ignore_Set));
}

void Add_Message_To_Ignore (const char *name)
{ 
  MSG_PTR Message;
  
  Message = x_ipc_findOrRegisterMessage(name);
  if (Message)
    x_ipc_listInsertItem((void *)Message, GET_M_GLOBAL(Message_Ignore_Set));
}

static void Remove_Message_From_Ignore (const char *name)
{ 
  MSG_PTR Message;
  
  Message = GET_MESSAGE(name);
  if (Message)
    x_ipc_listDeleteItem((void *)Message, GET_M_GLOBAL(Message_Ignore_Set));
}

void ignoreLoggingHnd(DISPATCH_PTR dispatch, char **msgName)
{
  Add_Message_To_Ignore(*msgName);
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, msgName);
}

void resumeLoggingHnd(DISPATCH_PTR dispatch, char **msgName)
{
  Remove_Message_From_Ignore(*msgName);
  /* A bit more efficient than using x_ipcFreeData */
  x_ipc_freeDataStructure(dispatch->msg->msgData->msgFormat, msgName);
}

void Start_Ignore_Logging(void)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->ignore == TRUE)
      (*log)->ignoreNow = TRUE;
  }
}

void End_Ignore_Logging(void)
{
  LOG_PTR *log = GET_M_GLOBAL(logList);
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    (*log)->ignoreNow = FALSE;
  }
}

void Start_Terminal_Logging(void)
{
  if (!GET_S_GLOBAL(terminalLog).quiet)
    printf("Task Control Server %d.%d.%d (%s)\n", X_IPC_VERSION_MAJOR,
	   X_IPC_VERSION_MINOR, X_IPC_VERSION_MICRO, X_IPC_VERSION_DATE);
}

#define LINE_LENGTH 80
#define COMMENT_LENGTH LINE_LENGTH-3

#ifndef VXWORKS
static void add_comments_to_log_file(void)
{ 
  char comment_string[COMMENT_LENGTH+1];
  static char astericks[] = "********************************************************************************";
  static char comments[] =  "*                                   Comments                                   *";
  static char spacer[] =    "*                                                                              *";
  int last_line_p, has_comments_p = FALSE;
  
  printf("Enter any comments in the log file (end with blank line)\n");
  fflush(stdout);
  
  do {
    fgets(comment_string, COMMENT_LENGTH, stdin);
    last_line_p = (comment_string[0] == '\n'); /* the "newline" character */
    if (last_line_p) {
      if (has_comments_p)
	fprintf(GET_S_GLOBAL(fileLog).theFile, "%s\n%s\n\n", spacer, astericks);
    }
    else {
      if (!has_comments_p) {
	fprintf(GET_S_GLOBAL(fileLog).theFile, 
		"\n%s\n%s\n%s\n", astericks, comments, spacer);
	has_comments_p = TRUE;
      }
      /* Replace "newline" character */
      comment_string[strlen(comment_string)-1] = '\0';
      fprintf(GET_S_GLOBAL(fileLog).theFile, "* %-*s*\n", 
	      LINE_LENGTH-3, comment_string);
    }
  }
  while (!last_line_p);
}
#endif

void Start_File_Logging(void)
{ 
#ifndef VXWORKS
  struct timeval current_time;
#endif

  if (GET_S_GLOBAL(fileLog).messages || GET_S_GLOBAL(fileLog).status ||
      GET_S_GLOBAL(fileLog).data || 
      GET_S_GLOBAL(fileLog).time || GET_S_GLOBAL(fileLog).summary) {
    if (GET_S_GLOBAL(fileLog).theFile == NULL) {
      GET_S_GLOBAL(fileLog).theFile = fopen(GET_S_GLOBAL(Log_File_Name), "w");
      if (!GET_S_GLOBAL(fileLog).theFile) {
	X_IPC_MOD_ERROR1("ERROR: Can not open file %s.\n", 
		       GET_S_GLOBAL(Log_File_Name));
	return;
      }
      
#ifndef VXWORKS
      if (!GET_S_GLOBAL(terminalLog).quiet)
	printf("Logging to %s;\n", GET_S_GLOBAL(Log_File_Name));
      
      (void)gettimeofday(&current_time, NULL);
      fprintf(GET_S_GLOBAL(fileLog).theFile, 
	      "Logging Task Control Server %d.%d.%d (%s) on %s",
	      X_IPC_VERSION_MAJOR, X_IPC_VERSION_MINOR, X_IPC_VERSION_MICRO,
	      X_IPC_VERSION_DATE, ctime((time_t *)&current_time.tv_sec));
      
      if (GET_S_GLOBAL(fileLog).addComments == TRUE) { add_comments_to_log_file(); }
      
#else
      /* Need to unbuffer the file to get it to flush immediately on 
	 VxWorks (should I do this for Unix, as well?) */
      setbuf(GET_S_GLOBAL(fileLog).theFile, (char *)NULL);

      if (!GET_S_GLOBAL(terminalLog).quiet)
	printf("Logging to %s\n", GET_S_GLOBAL(Log_File_Name));
      
      fprintf(GET_S_GLOBAL(fileLog).theFile, 
	      "Logging Task Control Server %d.%d.%d (%s)", X_IPC_VERSION_MAJOR,
	      X_IPC_VERSION_MINOR, X_IPC_VERSION_MICRO, X_IPC_VERSION_DATE);
#endif
      
    }
  } else {
    /* logging to the file need not be done. */
    if (GET_S_GLOBAL(fileLog).theFile == NULL) {
      End_File_Logging();
    }
  }
}

void End_File_Logging(void)
{
  if (GET_S_GLOBAL(fileLog).theFile != NULL) {
#ifndef VXWORKS
    if (GET_S_GLOBAL(fileLog).addComments == TRUE) { add_comments_to_log_file(); }
#endif
    fclose(GET_S_GLOBAL(fileLog).theFile);
    GET_S_GLOBAL(fileLog).theFile = NULL;
    GET_S_GLOBAL(fileLog).messages = 
      GET_S_GLOBAL(fileLog).status = GET_S_GLOBAL(fileLog).data = FALSE;
    GET_S_GLOBAL(fileLog).time = FALSE;
  }
}

#if defined(VXWORKS)
void x_ipcCloseLoggingFile()
{
  End_File_Logging();
}
#endif

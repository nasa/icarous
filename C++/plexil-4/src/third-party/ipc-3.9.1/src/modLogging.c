/*****************************************************************************
 * 
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: Logging
 *
 * FILE: modLogging.c
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
 * $Log: modLogging.c,v $
 * Revision 2.4  2009/01/12 15:54:56  reids
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
 * Revision 1.1.2.5  1997/01/27 20:09:44  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.3  1997/01/21 17:20:22  reids
 * Re-re-fixed the logic of "x_ipc_LogIgnoreP" (needed an extra
 *   function: LogIgnoreAllP).
 *
 * Revision 1.1.2.2  1996/12/18 15:11:20  reids
 * Re-fixed the logic of "x_ipc_LogIgnoreP"
 *
 * Revision 1.1.2.1  1996/10/18 18:13:46  reids
 * Fixed the logic of "ignore logging".
 *
 * Revision 1.1  1996/05/09 01:01:39  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:52  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.2  1996/07/12  13:54:49  reids
 * Fixed the logic of "ignore logging".
 *
 * Revision 1.1  1995/03/28  01:22:33  rich
 * Moved some logging functions to a seperate file so they can be used by
 * modules to determine if logging is active.
 *
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

int x_ipc_LogMessagesP(void)
{
  LOG_PTR *log;

  LOCK_M_MUTEX;
  log = GET_M_GLOBAL(logList);
  UNLOCK_M_MUTEX;

  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->messages)
      return(TRUE);
  }
  return(FALSE);
}

/* Return TRUE if data logging is turned on for *at least* one of the files */
int x_ipc_LogDataP(void)
{
  LOG_PTR *log;

  LOCK_M_MUTEX;
  log = GET_M_GLOBAL(logList);
  UNLOCK_M_MUTEX;
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->data)
      return(TRUE);
  }
  return(FALSE);
}

/* Return TRUE if *some* log-file is ignoring messages */
int x_ipc_LogIgnoreP(void)
{
  LOG_PTR *log;

  LOCK_M_MUTEX;
  log = GET_M_GLOBAL(logList);
  UNLOCK_M_MUTEX;
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if ((*log)->ignore)
      return(TRUE);
  }
  return(FALSE);
}

/* Return TRUE if *all* log-files are ignoring messages */
int LogIgnoreAllP(void)
{
  LOG_PTR *log;

  LOCK_M_MUTEX;
  log = GET_M_GLOBAL(logList);
  UNLOCK_M_MUTEX;
  
  for(; (((*log) != NULL) && ((*log)->theFile != NULL)); log++) {
    if (!(*log)->ignore)
      return(FALSE);
  }
  return(TRUE);
}

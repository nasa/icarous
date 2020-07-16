/*****************************************************************************
 * 
 * PROJECT: Task Control Architecture.
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: logging
 *
 * FILE: modLogging.h
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
 * $Log: modLogging.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:26  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.3  1997/01/27 20:09:45  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.1  1997/01/21 17:20:20  reids
 * Re-re-fixed the logic of "x_ipc_LogIgnoreP" (needed an extra
 *   function: LogIgnoreAllP).
 *
 * Revision 1.1  1996/05/09 01:01:40  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:53  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.2  1995/03/30  15:43:29  rich
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
 * Revision 1.1  1995/03/28  01:22:35  rich
 * Moved some logging functions to a seperate file so they can be used by
 * modules to determine if logging is active.
 *
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCmodLogging
#define INCmodLogging

int x_ipc_LogMessagesP(void);
int x_ipc_LogDataP(void);
int x_ipc_LogIgnoreP(void);
int LogIgnoreAllP(void);

#endif /* INCmodLogging */

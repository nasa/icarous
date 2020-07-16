/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: 
 *
 * FILE: tcerror.h
 *
 * ABSTRACT:
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/tcerror.h,v $ 
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: tcerror.h,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:30  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.1  1996/12/18 15:13:14  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1  1996/05/09 01:02:10  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:59  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.5  1995/01/18  22:43:34  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.4  1994/10/25  17:11:09  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.3  1994/05/17  23:18:25  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.2  1993/12/01  18:04:46  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.1  1993/08/31  06:02:25  fedor
 * Forgot to include tcerror.h in repository
 *
 * Revision 1.3  1993/08/23  17:41:08  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:21:26  rich
 * Added automatic logging.
 *
 *****************************************************************************/

#ifndef INCtcerror
#define INCtcerror

/* It turns out that compiling optimized with Greenhills causes vprintf 
   to crash -- need to define macros for warnings & errors on VxWorks */
#ifndef VXWORKS
#define X_IPC_ERROR(description) x_ipcError(description)
#define X_IPC_ERROR1(description, arg1) x_ipcError(description, arg1)
#define X_IPC_ERROR2(description, arg1, arg2) x_ipcError(description, arg1, arg2)
#define X_IPC_ERROR3(description, arg1, arg2, arg3) \
  x_ipcError(description, arg1, arg2, arg3)

#define X_IPC_MOD_ERROR0() x_ipcModError(0)
#define X_IPC_MOD_ERROR(description) x_ipcModError(description)
#define X_IPC_MOD_ERROR1(description, arg1) x_ipcModError(description, arg1)
#define X_IPC_MOD_ERROR2(description, arg1, arg2) \
  x_ipcModError(description, arg1, arg2)
#define X_IPC_MOD_ERROR3(description, arg1, arg2, arg3) \
  x_ipcModError(description, arg1, arg2, arg3)

#define X_IPC_MOD_WARNING(description) x_ipcModWarning(description)
#define X_IPC_MOD_WARNING1(description, arg1) x_ipcModWarning(description, arg1)
#define X_IPC_MOD_WARNING2(description, arg1, arg2) \
  x_ipcModWarning(description, arg1, arg2)
#define X_IPC_MOD_WARNING3(description, arg1, arg2, arg3) \
  x_ipcModWarning(description, arg1, arg2, arg3)

#else /* !VXWORKS */
#define X_IPC_ERROR(description) { LOG(description); x_ipcError(0); }
#define X_IPC_ERROR1(description, arg1) { LOG1(description, arg1); x_ipcError(0); }
#define X_IPC_ERROR2(description, arg1, arg2) \
 { LOG2(description, arg1, arg2); x_ipcError(0); }
#define X_IPC_ERROR3(description, arg1, arg2, arg3) \
 { LOG3(description, arg1, arg2, arg3); x_ipcError(0); }

#define X_IPC_MOD_ERROR0() x_ipcModError(0)
#define X_IPC_MOD_ERROR(description) \
  if (ipcVerbosity >= IPC_Print_Errors) {\
   fprintf(stderr, description); x_ipcModError(0); }
#define X_IPC_MOD_ERROR1(description, arg1) \
  if (ipcVerbosity >= IPC_Print_Errors) {\
   fprintf(stderr, description, arg1); x_ipcModError(0); }
#define X_IPC_MOD_ERROR2(description, arg1, arg2) \
  if (ipcVerbosity >= IPC_Print_Errors) {\
   fprintf(stderr, description, arg1, arg2); x_ipcModError(0); }
#define X_IPC_MOD_ERROR3(description, arg1, arg2, arg3) \
  if (ipcVerbosity >= IPC_Print_Errors) {\
   fprintf(stderr, description, arg1, arg2, arg3); x_ipcModError(0); }

#define X_IPC_MOD_WARNING(description) \
  if (ipcVerbosity >= IPC_Print_Warnings) {\
   fprintf(stderr, description); x_ipcModWarning(0); }
#define X_IPC_MOD_WARNING1(description, arg1) \
  if (ipcVerbosity >= IPC_Print_Warnings) {\
   fprintf(stderr, description, arg1); x_ipcModWarning(0); }
#define X_IPC_MOD_WARNING2(description, arg1, arg2) \
  if (ipcVerbosity >= IPC_Print_Warnings) {\
   fprintf(stderr, description, arg1, arg2); x_ipcModWarning(0); }
#define X_IPC_MOD_WARNING3(description, arg1, arg2, arg3) \
  if (ipcVerbosity >= IPC_Print_Warnings) {\
   fprintf(stderr, description, arg1, arg2, arg3); x_ipcModWarning(0); }

#endif /* !VXWORKS */

/* These functions take a variable number of arguments, just like printf */
void x_ipcError(const char *Description, ...);
void x_ipcModError(const char *description, ...);
void x_ipcModWarning(const char *description, ...);

#endif /* INCtcerror */

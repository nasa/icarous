/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: resources
 *
 * FILE: resMod.h
 *
 * ABSTRACT:
 * 
 * Resources - Module Routines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: resMod.h,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:57  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:32  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.6  1995/05/31  19:36:37  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.5  1994/05/25  17:32:39  reids
 * Added utilities to limit the number of pending messages
 *
 * Revision 1.4  1994/05/17  23:17:31  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.3  1993/11/21  20:19:27  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.2  1993/05/26  23:19:06  rich
 * Fixed up the comments at the top of the file.
 *
 * 26-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * New implmentation of resources - x_ipc version 5.0.
 *
 * 13-Mar-89 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCresMod
#define INCresMod

typedef struct {
  const char *hndName;
  const char *resName;
} ADD_HND_FORM_TYPE, *ADD_HND_FORM_PTR;

typedef struct {
  int32 capacity;
  const char *resName;
} ADD_RES_FORM_TYPE, *ADD_RES_FORM_PTR;

typedef struct {
  const char *msgName;
  const char *resName;
  int32 limit;
} LIMIT_PENDING_TYPE, *LIMIT_PENDING_PTR;

#endif /* INCresMod */

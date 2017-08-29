/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: key
 *
 * FILE: key.h
 *
 * ABSTRACT:
 * 
 * Collection of hash and key functions for table lookups.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: key.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:33  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:41  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.10  1996/02/21  18:36:49  rich
 * Remove null keys.
 *
 * Revision 1.9  1996/02/21  18:30:26  rich
 * Created single event loop.
 *
 * Revision 1.8  1995/08/06  16:44:00  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.7  1995/05/31  19:35:45  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.6  1994/05/17  23:16:11  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.5  1993/12/01  18:03:44  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:18:03  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:15:15  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:46  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:44  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:18  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:12  fedor
 * Added Logging.
 *
 * 16-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCkey
#define INCkey

int32 x_ipc_intHashFunc(int32 *i);
int32 x_ipc_intKeyEqFunc(int32 *a, int32 *b);
int32 x_ipc_strHashFunc(const char *s);
int32 x_ipc_strKeyEqFunc(const char *a, const char *b);
int32 x_ipc_intStrHashFunc(INT_STR_KEY_PTR key);
int32 x_ipc_intStrKeyEqFunc(INT_STR_KEY_PTR keyA, INT_STR_KEY_PTR keyB);
int32 x_ipc_classHashFunc(X_IPC_MSG_CLASS_TYPE *className);
int32 x_ipc_classEqFunc(X_IPC_MSG_CLASS_TYPE *classA, X_IPC_MSG_CLASS_TYPE *classB);

#endif /* INCkey */

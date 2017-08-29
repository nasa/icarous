/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: 
 *
 * FILE: x_ipcRef.c 
 *
 * ABSTRACT:
 * Implements X_IPC_REF_PTR
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/tcaRef.h,v $ 
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
 * $Log: tcaRef.h,v $
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
 * Revision 1.1  1996/05/09 01:02:09  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:56  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.4  1995/04/19  14:29:13  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.3  1994/05/17  23:18:23  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.2  1993/12/01  18:04:43  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.1  1993/08/27  07:17:29  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  17:41:01  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:21:19  rich
 * Added automatic logging.
 *
 *****************************************************************************/

#ifndef INCx_ipcRef
#define INCx_ipcRef

void x_ipcRefInitialize(void);
X_IPC_REF_PTR x_ipcRefCreate(MSG_PTR msg, const char *name, int32 refId);
void x_ipcRefFree(X_IPC_REF_PTR x_ipcRef);

#endif /* INCx_ipcRef */

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: registration
 *
 * FILE: reg.c
 *
 * ABSTRACT:
 * 
 * Register messages and handlers
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/reg.h,v $ 
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
 * $Log: reg.h,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:28  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.1  1996/10/18 18:16:49  reids
 * Fixed freeing of memory for messages and handlers.
 * Better error handling; Support for CLISP.
 *
 * Revision 1.1  1996/05/09 01:01:55  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:26  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.8  1996/06/25  20:51:30  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.7  1995/12/17  20:22:03  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.6  1995/07/06  21:17:16  rich
 * Solaris and Linux changes.
 *
 * Revision 1.5  1995/05/31  19:36:29  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.4  1995/01/18  22:42:27  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.3  1994/05/17  23:17:21  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.2  1993/12/01  18:04:22  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.1  1993/08/27  07:16:38  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/08/23  17:40:16  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.3  1993/07/05  18:11:26  reids
 * Added x_ipcDeregisterHandler function
 *
 * Revision 1.2  1993/05/27  22:20:12  rich
 * Added automatic logging.
 *
 *
 ****************************************************************/

#ifndef INCreg
#define INCreg

MSG_PTR x_ipc_msgCreate(MSG_DATA_PTR msgData);
void x_ipc_msgFree(char *name, MSG_PTR msg);
void x_ipc_hndFree(HND_KEY_PTR key, HND_PTR hnd);

MSG_PTR x_ipc_findOrRegisterMessage(const char *name);
HND_PTR x_ipc_selfRegisterHnd(int sd, MODULE_PTR hndOrg,
			HND_DATA_PTR hndData, X_IPC_HND_FN hndProc);
void x_ipc_hndDelete(HND_PTR hnd);
int32 x_ipc_deregisterHnd(int sd, HND_DATA_PTR hndData);

#endif /* INCreg */

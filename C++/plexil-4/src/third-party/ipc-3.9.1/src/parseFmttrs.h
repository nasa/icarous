/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: Formatters
 *
 * FILE: parseFmttrs.c
 *
 * ABSTRACT:
 * Parser and Printer for Formatter Data Structures
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/parseFmttrs.h,v $ 
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: parseFmttrs.h,v $
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:15  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:27  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:47  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:08  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.7  1996/01/27  21:53:42  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.6  1995/05/31  19:36:08  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.5  1994/05/17  23:16:54  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.4  1993/12/01  18:04:05  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.3  1993/11/21  20:18:48  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.2  1993/08/30  21:54:03  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.1  1993/08/27  07:15:58  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  17:39:33  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:18:58  rich
 * Added automatic logging.
 *
 ****************************************************************/

#ifndef INCparseFmttrs
#define INCparseFmttrs

CONST_FORMAT_PTR ParseFormatString(const char *formString);

#endif /* INCparseFmttrs */

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: idTable
 *
 * FILE: idtable.h
 *
 * ABSTRACT:
 * 
 * Generic dynamic array abstract data type.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: idtable.h,v $
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
 * Revision 1.1  1996/05/09 01:01:31  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:39  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.9  1995/05/31  19:35:43  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.8  1995/04/19  14:28:20  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.7  1995/01/18  22:40:51  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.6  1994/05/25  04:57:38  rich
 * Defined macros for registering simple messages and handlers at once.
 * Added function to ignore logging for all messages associated with a
 * global variable.
 * Moved module global variable routines to a new file so they are not
 * included in the .sa library file.  Gets better code sharing and lets you
 * debug these routines.
 * Added code to force the module variables to be re-initialized after the
 * server goes down.
 * x_ipcClose now will not crash if the server is down and frees some module
 * memory.
 * The command line flag "-u" turns off the simple user interface.
 * Added routines to free hash tables and id tables.
 *
 * Revision 1.5  1994/05/17  23:16:05  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.4  1993/12/01  18:03:41  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.3  1993/08/27  07:15:10  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:41  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:44  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:14  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:07  fedor
 * Added Logging.
 *
 * 25-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 * 20-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCidtable
#define INCidtable


typedef struct _TELEM {
  int32 ref;
  char *data;
  struct _TELEM *next;
} TELEM_TYPE, *TELEM_PTR;

typedef struct {
  char *name;
  int32 incSize;
  int32 freeTotal; 
  int32 currentSize;
  TELEM_PTR table, freeList;
} ID_TABLE_TYPE, *ID_TABLE_PTR;

#define idTableItem(id, idTable) ((idTable->table+id)->data)

ID_TABLE_PTR x_ipc_idTableCreate(char *name, int32 incSize);
void x_ipc_idTableFree(ID_TABLE_PTR *table);
int32 x_ipc_idTableInsert(void *item, ID_TABLE_PTR table);
void *x_ipc_idTableRemove(int32 id, ID_TABLE_PTR table);
void x_ipc_idTablePrintInfo(ID_TABLE_PTR table);

int32 x_ipc_idTableSearch(void *item, ID_TABLE_PTR table);
#endif /* INCidtable */

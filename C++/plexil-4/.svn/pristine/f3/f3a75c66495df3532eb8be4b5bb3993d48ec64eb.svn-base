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
 * $Log: idtable.c,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:24  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.2  1996/12/18 15:12:51  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.1  1996/10/16 15:21:35  reids
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.1  1996/05/09 01:01:30  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:38  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.9  1996/06/25  20:50:45  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.8  1995/08/14  21:31:53  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.7  1995/07/10  16:17:31  rich
 * Interm save.
 *
 * Revision 1.6  1995/07/06  21:16:38  rich
 * Solaris and Linux changes.
 *
 * Revision 1.5  1995/01/18  22:40:50  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.4  1994/05/25  04:57:36  rich
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
 * Revision 1.3  1993/08/27  07:15:07  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:39  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:24  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:09  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:05  fedor
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

#include "globalM.h"
#include "idtable.h"

ID_TABLE_PTR x_ipc_idTableCreate(char *name, int32 incSize)
{
  ID_TABLE_PTR idTable;
  
  idTable = NEW(ID_TABLE_TYPE);
  
  idTable->name = name;
  idTable->incSize = incSize;
  idTable->freeTotal = 0;
  idTable->currentSize = 0;
  idTable->table = NULL;
  idTable->freeList = NULL;
  
  return idTable;
}

void x_ipc_idTableFree(ID_TABLE_PTR *table)
{
  if (table == NULL) return;
  if (*table) {
    if ((*table)->table) {
      
      x_ipcFree((char *)(*table)->table);
    }
    x_ipcFree((char *)*table);
  }
  *table=NULL;
}

static void x_ipc_idTableIncrement(ID_TABLE_PTR table)
{
  int i, newTableSize, currentSize, newFreeIds;
  TELEM_PTR tmp, newTable, oldTable, newFreeList, lastNewFreeBlock;
  
  newFreeIds = table->incSize;
  currentSize = table->currentSize;
  
  newTableSize = currentSize+newFreeIds;
  
  oldTable = table->table;
  newTable = (TELEM_TYPE *)x_ipcMalloc(sizeof(TELEM_TYPE)*
				     (unsigned)newTableSize);
  
  i = 0;
  
  if (oldTable) {
    /* Copy Old Table */
    for (;i<currentSize;i++)
      newTable[i] = oldTable[i];
    x_ipcFree((char *)oldTable);
  }
  
  /* Update Free List and Initialize New Blocks */
  newFreeList = NULL;
  lastNewFreeBlock = newTable + i;
  for (;i<newTableSize;i++) {
    tmp = newTable + i;
    tmp->data = NULL;
    tmp->ref = i;
    tmp->next = newFreeList;
    newFreeList = tmp;
  }
  
  /* Adjust Table Entry */
  lastNewFreeBlock->next = table->freeList;
  table->freeList = newFreeList;
  
  table->freeTotal += newFreeIds;
  table->currentSize = newTableSize;
  
  table->table = newTable;
}

int32 x_ipc_idTableInsert(void *item, ID_TABLE_PTR table)
{
  TELEM_PTR tmp;
  
  if (!table->freeList)
    x_ipc_idTableIncrement(table);
  
  tmp = table->freeList;
  table->freeList = tmp->next;
  tmp->next = NULL;
  table->freeTotal--;
  
  tmp->data = (char *)item;
  
  return(tmp->ref);
}

void *x_ipc_idTableRemove(int32 id, ID_TABLE_PTR table)
{
  char *data;
  TELEM_PTR tmp;
  
  tmp = table->table+id;
  
  data = tmp->data;
  
  if (!tmp->next) {
    /* Not Already on Free List */
    tmp->data = NULL;
    tmp->next = table->freeList;
    table->freeList = tmp;
    table->freeTotal++;
  }
  
  return data;
}


void x_ipc_idTablePrintInfo(ID_TABLE_PTR table)
{
  if (table) {
    X_IPC_MOD_WARNING1( "idTable: %s\n", table->name);
    X_IPC_MOD_WARNING1( "free   : %d\n", table->freeTotal);
    X_IPC_MOD_WARNING1( "size   : %d\n", table->currentSize);
    X_IPC_MOD_WARNING1( "incSize: %d\n", table->incSize);
  }
  else
    X_IPC_MOD_WARNING( "x_ipc_idTablePrintInfo: NULL Table\n");
}

int32 x_ipc_idTableSearch(void *item, ID_TABLE_PTR table)
{
  int i;
  
  for (i=0; i<table->currentSize; i++)
    {
      if (table->table == item)
	return i;
    }
  return -1;
}

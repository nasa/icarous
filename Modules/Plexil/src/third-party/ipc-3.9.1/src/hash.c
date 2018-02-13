/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: hash
 *
 * FILE: hash.c
 *
 * ABSTRACT:
 * 
 * Generic hash table abstract data type.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/hash.c,v $ 
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: hash.c,v $
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
 * Revision 1.1.2.4  1996/12/18 15:12:50  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.3  1996/10/28 16:20:33  reids
 * More informative hash table statistics.
 *
 * Revision 1.1.2.2  1996/10/18 18:05:54  reids
 * Fixed x_ipc_hashTableIterate.
 *
 * Revision 1.1.2.1  1996/10/16 15:21:33  reids
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.1  1996/05/09 01:01:29  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:35  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.15  1996/06/25  20:50:43  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.14  1996/02/10  16:50:01  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.13  1996/02/06  19:04:51  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.12  1996/01/30  15:04:20  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.11  1996/01/12  00:53:13  rich
 * Simplified GNUmakefiles.  Fixed some dbmalloc problems.
 *
 * Revision 1.10  1995/12/17  20:21:30  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.9  1995/10/25  22:48:29  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.8  1995/10/07  19:07:24  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.7  1995/07/06  21:16:35  rich
 * Solaris and Linux changes.
 *
 * Revision 1.6  1995/01/18  22:40:45  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.5  1994/05/25  04:57:31  rich
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
 * Revision 1.4  1993/08/30  21:53:41  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.5  1993/08/23  17:38:29  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.4  1993/07/08  05:38:32  rich
 * Added function prototypes
 *
 * Revision 1.3  1993/06/22  13:59:07  rich
 * Added makefile.depend.  Dependencies automatically generated using gcc.
 * Fixed some warnings.
 * Updated the -D<arch> flags to correspond to those generated
 * automatically by the makefile.
 * Changed system includes to the proper format "stdio.h" -> <stdio.h>.
 * This was needed so that the automatic dependency generation can
 * distinguish between "local" and system headers.  The location of the
 * system headers changes from architecture to architecture and should not
 * be included in the dependency list.
 *
 * Revision 1.2  1993/05/27  22:17:15  rich
 * Added automatic logging.
 *
 *  6-Apr-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 * 17-Nov-89 Reid Simmons, School of Computer Science, CMU
 * Added function for printing hash table stats.
 *
 * 10-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 *****************************************************************************/

#include "globalM.h"

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

#if !defined(DBMALLOC)
HASH_TABLE_PTR x_ipc_hashTableDBCreate(int32 size, HASH_FN hashFunc, 
				 EQ_HASH_FN eqFunc,const char* file, int line);
#else
#undef x_ipc_hashTableCreate
HASH_TABLE_PTR x_ipc_hashTableCreate(int32 size, HASH_FN hashFunc,
			       EQ_HASH_FN eqFunc);
#endif


/******************************************************************************
 *
 * FUNCTION: HASH_TABLE_PTR x_ipc_hashTableCreate(size, hashFunc, eqFunc)
 *
 * DESCRIPTION:
 * Create a hash table.
 *
 * HashFunc(Key)  
 * - returns a hash value of the key.
 * - this value will be modded with table size when (key, item) pair
 *   is added to the table.
 *
 * EqualityFunc(Key1, Key2) 
 * - returns 1 if the key values Key1 and Key2 are equal, 0 otherwise.
 *
 * INPUTS: 
 * int32 size;
 * HASH_FN hashFunc;
 * EQ_HASH_FN eqFunc;
 *
 * OUTPUTS: HASH_TABLE_PTR
 *
 *****************************************************************************/

#if defined(DBMALLOC)
HASH_TABLE_PTR x_ipc_hashTableCreate(int32 size, HASH_FN hashFunc, EQ_HASH_FN eqFunc)
{
  return x_ipc_hashTableDBCreate(size, hashFunc, eqFunc, "Unknown", 9999);
}

HASH_TABLE_PTR x_ipc_hashTableDBCreate(int32 size, HASH_FN hashFunc, 
				 EQ_HASH_FN eqFunc,const char* file, int line)
     
#else
HASH_TABLE_PTR x_ipc_hashTableDBCreate(int32 size, HASH_FN hashFunc, 
					EQ_HASH_FN eqFunc,
					const char* file, int line)
{
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  return x_ipc_hashTableCreate( size, hashFunc, eqFunc);
}     

HASH_TABLE_PTR x_ipc_hashTableCreate(int32 size, HASH_FN hashFunc, EQ_HASH_FN eqFunc)
#endif
{
  int32 i;
  HASH_ELEM_PTR *table;
  HASH_TABLE_PTR hashTable;
  
#if defined(DBMALLOC)
  hashTable = NEW_DB(file,line,HASH_TABLE_TYPE);
  table = (HASH_ELEM_PTR *)x_ipcDBMalloc(file,line,
				       sizeof(HASH_ELEM_PTR)*(unsigned)size);
#else
  hashTable = NEW(HASH_TABLE_TYPE);
  table = (HASH_ELEM_PTR *)x_ipcMalloc(sizeof(HASH_ELEM_PTR)*(unsigned)size);
#endif
  
  for(i=0;i < size;i++)
    table[i] = NULL;
  
  hashTable->size = size;
  hashTable->hashFunc = hashFunc;
  hashTable->eqFunc = eqFunc;
  hashTable->table = table;
  
  return hashTable;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_hashTableFree(HASH_TABLE_PTR table);
 *
 * DESCRIPTION:
 * Release memory used by hash table.  Does not release the memory for the 
 * items in the hash table.
 *
 *****************************************************************************/
static void x_ipc_hashElementFree(HASH_ELEM_PTR element)
{
  
  if (element != NULL) {
    x_ipc_hashElementFree(element->next);
    if (element->key) {
      x_ipcFree((char *) element->key);
      element->key = NULL;
    }
    x_ipcFree((char *) element);
  }
}

int32 x_ipc_hashItemsFree(const void *key, const void *item, void *ignore)
{
#ifdef UNUSED_PRAGMA
#pragma unused(key, ignore)
#endif
  if (item != NULL)
    x_ipcFree((char *)item);
  return 1;
}

void x_ipc_hashTableFree(HASH_TABLE_PTR *table,HASH_ITER_FN iterFunc, void *param)
{
  int32 i;
  
  if (*table == NULL)
    return;
  
  if (iterFunc != NULL) 
    x_ipc_hashTableIterate(iterFunc, *table, param);
  
  for (i=0;i < (*table)->size;i++) {
    x_ipc_hashElementFree((*table)->table[i]);
  }
  x_ipcFree((char *)(*table)->table);
  x_ipcFree((char *)*table);
  *table = NULL;
}


/******************************************************************************
 *
 * FUNCTION: HASH_ELEM_PTR x_ipc_findElement(eq, list, key)
 *
 * DESCRIPTION: 
 * Iterate through the element list returning the element of the list whose
 * key value is "eq" to the given key. NULL is returned if no match is found.
 *
 * INPUTS:
 * EQ_HASH_FN eq;
 * HASH_ELEM_PTR list;
 * char *key;
 *
 * OUTPUTS: HASH_ELEM_PTR
 *
 *****************************************************************************/

static HASH_ELEM_PTR x_ipc_findElement(EQ_HASH_FN eq, 
				 HASH_ELEM_PTR list, 
				 const void *key)
{
  HASH_ELEM_PTR tmp;
  
  tmp = list;
  while (tmp) {
    if ((*eq)(tmp->key, key))
      return tmp;
    tmp = tmp->next;
  }
  
  return NULL;
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipc_hashTableFind(key, table)
 *
 * DESCRIPTION: The item is returned or NULL if not found.
 *
 * INPUTS: 
 * char *key;
 * HASH_TABLE_PTR table;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

const void *x_ipc_hashTableFind(const void *key, HASH_TABLE_PTR table)
{
  HASH_ELEM_PTR tmp;
  int32 hash, location;
  
  if (table == NULL) return NULL;
  hash = (*table->hashFunc)(key);
  location = hash % table->size;
  
  tmp = table->table[location];
  if (tmp) {
    tmp = x_ipc_findElement(table->eqFunc, tmp, key);
    if (tmp)
      return(tmp->data);
    else
      return NULL;
  }
  else
    return NULL;
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipc_hashTableInvFind(item, table)
 *
 * DESCRIPTION: The key for the given item ptr.
 *
 * INPUTS: 
 * char *key;
 * HASH_TABLE_PTR table;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

const void *x_ipc_hashTableInvFind(const void *item, HASH_TABLE_PTR table)
{
  int32 i;
  HASH_ELEM_PTR tmp;
  
  for (i=0;i < table->size;i++) {
    tmp = table->table[i];
    while (tmp){
      if ((const void *)tmp->data == item)
	return tmp->key;
      tmp = tmp->next;
    }
  }
  return NULL;
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipc_hashTableInsert(key, keySize, item, table)
 *
 * DESCRIPTION:
 * The key value is copied for future lookups.
 * The old Item will be returned if the new item replaces 
 * one of the same key value. 
 * - A warning is also issued - the warning may go away soon.
 *
 * INPUTS: 
 * char *key;
 * int32 keySize;
 * char *item;
 * HASH_TABLE_PTR table;
 *
 * OUTPUTS: char * (any item that was already stored under this key or NULL)
 *
 * NOTES:
 * It is common to use a string key be sure that keySize is then
 * equal to strlen(key)+1 so that the NULL terminator is also stored.
 *
 *****************************************************************************/

const void *x_ipc_hashTableInsert(const void *key, int32 keySize, 
			    const void *item, HASH_TABLE_PTR table)
{
  const char *oldData;
  int32 hash, location;
  HASH_ELEM_PTR tmp, element;
  char *keyPtr=NULL;
  
  hash = (*table->hashFunc)(key);
  location = hash % table->size;
  
  tmp = table->table[location];
  if (tmp) {
    tmp = x_ipc_findElement(table->eqFunc, tmp, key);
    if (tmp) {
      /* replace item with new information */
      oldData = tmp->data;
      tmp->data = (const char *)item;
      return oldData;
    }
  }
  
  element = NEW(HASH_ELEM_TYPE);
  keyPtr = (char *)x_ipcMalloc((unsigned)keySize);
  BCOPY(key, keyPtr, keySize);
  element->key = (const char *) keyPtr;
  element->data = (const char *)item;
  element->next = table->table[location];
  table->table[location] = element;
  
  return NULL;
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipc_hashTableRemove(key, table)
 *
 * DESCRIPTION: The item stored with this key value is returned.
 *
 * INPUTS: 
 * char *key;
 * HASH_TABLE_PTR table;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

const void *x_ipc_hashTableRemove(const void *key, HASH_TABLE_PTR table)
{
  EQ_HASH_FN eq;
  const char *oldData;
  int32 hash, location;
  HASH_ELEM_PTR previous, current;
  
  hash = (*table->hashFunc)(key);
  location = hash % table->size;
  
  eq = table->eqFunc;
  
  previous = table->table[location];
  if (!previous)
    return NULL;
  
  if ((*eq)(previous->key, key)) {
    table->table[location] = previous->next;
    oldData = previous->data;
    x_ipcFree((char *)previous->key);
    x_ipcFree((char *)previous);
    return oldData;
  }
  current = previous->next;
  while (current) {
    if ((*eq)(current->key, key)) {
      oldData = current->data;
      previous->next = current->next;
      x_ipcFree((char *)current->key);
      x_ipcFree((char *)current);
      return oldData;
    }
    previous = current;
    current = current->next;
  }
  
  return NULL;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_hashTableIterate(iterFunc, table, param)
 *
 * DESCRIPTION:
 * iterFunc(key, data)
 * char *key, *data;
 *
 * iterFunc
 *  - takes two arguments, a pointer to Key information and a pointer to Data.
 *
 * x_ipc_hashTableIterate will call the function on all of its elements stoping
 * when the list is finished or when iterFunc returns 0 (ie FALSE)
 *
 * INPUTS: 
 * INT_FN iterFunc;
 * HASH_TABLE_PTR table;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipc_hashTableIterate(HASH_ITER_FN iterFunc, HASH_TABLE_PTR table, void *param)
{
  int32 i;
  HASH_ELEM_PTR tmp,next;
  
  for (i=0;i < table->size;i++) {
    tmp = table->table[i];
    while (tmp){
      next = tmp->next;
      if (!(*iterFunc)(tmp->key, tmp->data, param))
	return;
      tmp = next;
    }
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_hashTableStats(hashTable)
 *
 * DESCRIPTION: Calculates and displays hash table stats.
 *
 * INPUTS: HASH_TABLE_PTR hashTable;
 *
 * OUTPUTS: void. Results displayed.
 *
 *****************************************************************************/

void x_ipc_hashTableStats(HASH_TABLE_PTR hashTable)
{
  HASH_ELEM_PTR elem;
  int32 i, num=0, max=0, full=0, length;
  
  if (hashTable) {
    for (i=0;i < hashTable->size; i++) {
      elem = hashTable->table[i];
      if (elem) {
	full++;
	length = 0;
	while (elem) {
	  num++;
	  length++;
	  elem = elem->next;
	}
	if (length > max) 
	  max = length;
      }
    }
    X_IPC_MOD_WARNING2("x_ipc_hashTableStats: Has %d elements in %d slots\n",
		  num, full);
    X_IPC_MOD_WARNING2("   Out of %d slots (%.0f percent full)\n", 
		  hashTable->size, 100*((double)full/hashTable->size));
    X_IPC_MOD_WARNING2("   Average list is %.1f; maximum is %d\n", 
		  (double)num/full, max);
  } else {
    X_IPC_MOD_WARNING("x_ipc_hashTableStats: No Hash Table\n");
  }
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_hashTableCount(hashTable)
 *
 * DESCRIPTION: Calculates number of items in the hash table.
 *
 * INPUTS: HASH_TABLE_PTR hashTable;
 *
 * OUTPUTS: int, the count.
 *
 *****************************************************************************/

int x_ipc_hashTableCount(HASH_TABLE_PTR hashTable)
{
  HASH_ELEM_PTR elem;
  int32 i, num=0;
  
  for (i=0;i < hashTable->size; i++) {
    elem = hashTable->table[i];
    if (elem) {
      while (elem) {
	num++;
	elem = elem->next;
      }
    }
  }
  return num;
}

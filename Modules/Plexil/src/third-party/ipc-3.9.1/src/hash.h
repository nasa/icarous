/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: hash
 *
 * FILE: hash.h
 *
 * ABSTRACT:
 * 
 * Generic hash table abstract data type.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/hash.h,v $ 
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
 * $Log: hash.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:24  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:30  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:37  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.12  1995/10/25  22:48:31  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.11  1995/10/07  19:07:25  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.10  1995/07/06  21:16:37  rich
 * Solaris and Linux changes.
 *
 * Revision 1.9  1995/05/31  19:35:42  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.8  1995/01/18  22:40:47  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/25  04:57:33  rich
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
 * Revision 1.6  1994/05/17  23:16:04  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.5  1993/12/01  18:03:39  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/08/30  21:53:43  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.3  1993/08/23  17:38:31  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:17:20  rich
 * Added automatic logging.
 *
 *  6-Apr-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 * 10-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 *****************************************************************************/

#ifndef INChash
#define INChash

typedef struct _HASH_ELEM {
  const char *key;
  const char *data;
  struct _HASH_ELEM *next;
} HASH_ELEM_TYPE, *HASH_ELEM_PTR;

typedef int32
(* HASH_FN)(const void *);

typedef int32
(* EQ_HASH_FN)(const void *, const void *);

typedef int32
(* HASH_ITER_FN)(const void *, const void *, void *);

typedef struct {
  int32 size;
  HASH_FN hashFunc;
  EQ_HASH_FN eqFunc;
  HASH_ELEM_PTR *table;
} HASH_TABLE_TYPE, *HASH_TABLE_PTR;

#if defined(DBMALLOC)
HASH_TABLE_PTR x_ipc_hashTableDBCreate(int32 size, HASH_FN hashFunc, 
				 EQ_HASH_FN eqFunc,const char* file, int line);
#define x_ipc_hashTableCreate(size,hashFunc,eqFunc) x_ipc_hashTableDBCreate\
(size,hashFunc,eqFunc,__FILE__,__LINE__)
#else
HASH_TABLE_PTR x_ipc_hashTableCreate(int32 size, HASH_FN hashFunc,
			       EQ_HASH_FN eqFunc);
#endif

void x_ipc_hashTableFree(HASH_TABLE_PTR *table,HASH_ITER_FN iterFunc, void *param);
int32 x_ipc_hashItemsFree(const void *key, const void *item, void *ignore);

const void *x_ipc_hashTableFind(const void *key, HASH_TABLE_PTR table);
const void *x_ipc_hashTableInvFind(const void *item, HASH_TABLE_PTR table);
const void *x_ipc_hashTableInsert(const void *key, int32 keySize, 
			    const void *item, HASH_TABLE_PTR table);
const void *x_ipc_hashTableRemove(const void *key, HASH_TABLE_PTR table);
void x_ipc_hashTableIterate(HASH_ITER_FN iterFunc, HASH_TABLE_PTR table,
		      void *param);
void x_ipc_hashTableStats(HASH_TABLE_PTR hashTable);
int x_ipc_hashTableCount(HASH_TABLE_PTR hashTable);

#endif /* INChash */

/**************************************************************************
 * 
 * PROJECT: Task Control Architecture.
 *
 * MODULE: list
 *
 * FILE: list.h
 *
 * ABSTRACT: The list module provides basic list creation and manipulation
 * routines and serves as the base abstract data type for the x_ipc.
 * The include file list.h provides the top level routines for other modules.
 *
 * EXPORTS:
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: list.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:26  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:36  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:48  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.14  1995/12/17  20:21:37  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.13  1995/12/15  01:23:19  rich
 * Moved Makefile to Makefile.generic to encourage people to use
 * GNUmakefile.
 * Fixed a memory leak when a module is closed and some other small fixes.
 *
 * Revision 1.12  1995/10/25  22:48:33  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.11  1995/07/06  21:16:53  rich
 * Solaris and Linux changes.
 *
 * Revision 1.10  1995/05/31  19:35:52  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.9  1995/03/30  15:43:15  rich
 * DBMALLOC works.  To use "gmake -k -w DBMALLOC=DBMALLOC install"
 * Added simple list of strings data structure that can be passed via x_ipc
 * messages.
 * Use the string list to maintain a global variable of messages with taps.
 * Tapped messages are not sent via direct connections.
 * Implemented code to vectorize data to be sent so that it does not have
 * to be copied.  Currently, only flat, packed data structures are
 * vectored.  This can now be easily extended.
 * Changed Boolean -> BOOLEAN for consistency and to avoid conflicts with x11.
 * Fixed bug were central would try and free the "***New Module***" and
 * "*** Unkown Host***" strings when a module crashed on startup.
 * Fixed a bug reported by Jay Gowdy where the code to find the size of a
 * variable lenght array would access already freed data when called from
 * x_ipcFreeData.
 *
 * Revision 1.8  1995/01/18  22:41:06  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/17  23:16:18  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1993/12/01  18:03:49  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.5  1993/11/21  20:18:14  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/27  08:38:46  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.3  1993/08/27  07:15:26  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:01  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:45  rich
 * Importing x_ipc version 8
 *
 * Revision 1.2  1993/05/19  17:24:26  fedor
 * Added Logging.
 *
 * See list.c for history.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 **************************************************************************/

#ifndef INClist
#define INClist

typedef struct _LIST_ELEM {
  const char *item;
  struct _LIST_ELEM *next, *previous;
} LIST_ELEM_TYPE, *LIST_ELEM_PTR;

typedef struct _LIST {
  int32 length;
  LIST_ELEM_PTR first, last, next;
  struct _LIST *freeList;
} LIST_TYPE, *LIST_PTR;

#define LIST_INC_AMOUNT      10
#define LIST_CELL_INC_AMOUNT 10

typedef BOOLEAN 
(* LIST_ITER_FN)(const void *, const char *);

typedef void 
(* LIST_FREE_FN)(void *);

void x_ipc_listFree(LIST_PTR *list);

#if defined(DBMALLOC)
LIST_PTR x_ipc_listDBCreate(const char* file, int line);
#define x_ipc_listCreate() x_ipc_listDBCreate(__FILE__,__LINE__)
#else
LIST_PTR x_ipc_listCreate(void);
#endif

void x_ipc_listInsertItemFirst(const void *item, LIST_PTR list);
void x_ipc_listInsertItemLast(const void *item, LIST_PTR list);
void x_ipc_listInsertItemAfter(const void *item, void *after, LIST_PTR list);

const void *x_ipc_listPopItem(LIST_PTR list);
#define listPushItem(item,list) x_ipc_listInsertItemFirst(item,list)
void x_ipc_listTestDeleteItem(LIST_ITER_FN func, const void *param,
			LIST_PTR list);
void x_ipc_listTestDeleteItemAll(LIST_ITER_FN func,
			   const void *param,
			   LIST_PTR list);
void x_ipc_listFreeAllItems(LIST_FREE_FN func, LIST_PTR list);
int32 x_ipc_listItemEq(void *a, void *b);
void x_ipc_listDeleteItem(const void *item, LIST_PTR list);
void x_ipc_listDeleteItemAll(const void *item, LIST_PTR list);

int32 x_ipc_listMemberItem(const void *item, LIST_PTR list);
const void *x_ipc_listMemReturnItem(LIST_ITER_FN func,
			      const void *param,
			      LIST_PTR list);

int32 x_ipc_listIterateFromFirst(LIST_ITER_FN func, const void *param,
			   LIST_PTR list);
int32 x_ipc_listIterateFromLast(LIST_ITER_FN func, const void *param,
			  LIST_PTR list);

LIST_PTR x_ipc_listCopy(LIST_PTR list);
void x_ipc_listInsertItem(const void *item, LIST_PTR list);

int32 x_ipc_listIterate(LIST_ITER_FN func, const void *param, LIST_PTR list);
int32 x_ipc_listLength(LIST_PTR list);
int32 x_ipc_listEqual(LIST_PTR list1, LIST_PTR list2);

LIST_PTR x_ipc_listMake1(const void *item);
LIST_PTR x_ipc_listMake2(const void *item1, const void *item2);

const void *x_ipc_listFirst(LIST_PTR list);
const void *x_ipc_listLast(LIST_PTR list);
const void *x_ipc_listNext(LIST_PTR list);

void x_ipc_listCleanup(void);

#endif /* INClist */

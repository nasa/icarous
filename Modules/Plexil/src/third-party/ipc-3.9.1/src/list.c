/**************************************************************************
 * 
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: list
 *
 * FILE: list.c
 *
 * ABSTRACT:
 * The list module provides basic list creation and manipulation
 * routines for doubly linked lists.
 *
 * The include file list.h provides the top level routines for other modules.
 *
 * EXPORTS:
 *
 * LIST_PTR x_ipc_listCreate()
 * Creates an empty list.
 *
 * void x_ipc_listFree(list)
 * LIST_PTR list;
 * Frees storage associated with a list.
 *
 * void x_ipc_listInsertItemFirst(item, list)
 * char *item;
 * LIST_PTR list;
 * Adds item as the first item in the list.
 *
 * void x_ipc_listInsertItemLast(item, list)
 * char *item;
 * LIST_PTR list;
 * Adds item as the last item in the list.
 *
 * void x_ipc_listDeleteItem(item, list)
 * char *item;
 * LIST_PTR list;
 * Removes item from list.
 *
 * void x_ipc_listDeleteItemAll(item, list)
 * char *item;
 * LIST_PTR list;
 * Removes all elements containing item from list.
 *
 * void x_ipc_listTestDeleteItem(func, param, list)
 * int (*func)();
 * char *param;
 * LIST_PTR list;
 * Removes the first item in the list found such that func(param, item)
 * returns 1 (TRUE).
 *
 * void x_ipc_listTestDeleteItemAll(func, param, list)
 * int (*func)();
 * char *param;
 * LIST_PTR list;
 * Removes all items in the list found such that func(param, item)
 * returns 1 (TRUE).
 *
 * int x_ipc_listMemberItem(item, list)
 * char *item;
 * LIST_PTR list;
 * Returns 1 (TRUE) if item is in the list, otherwise 0 (FALSE) is returned.
 *
 * char *x_ipc_listMemReturnItem(func, param, list)
 * int (*func)();
 * char *param;
 * LIST_PTR list;
 * x_ipc_listMemReturnItem is a more general form of x_ipc_listMemberItem.
 * x_ipc_listMemReturnItem will return the item (or one of the items) in list
 * for which func(param, item) is non-zero, i.e. is TRUE.
 * The function takes two arguments, the first is the param and the second is 
 * an item of the list and returns an integer value. int func(param, item).
 * If the functions does not satisfy any of the items in the list NULL
 * is returned.
 *
 * int x_ipc_listIterateFromFirst(func, param, list)
 * int (*func)();
 * char *param;
 * LIST_PRT list;
 *
 * int x_ipc_listIterateFromLast(func, param, list)
 * int (*func)();
 * char *param;
 * LIST_PRT list;
 *
 * x_ipc_listIterateFromFirst and x_ipc_listIterateFromLast will call the function
 * func with param on all of its elements stoping when the list is
 * finished or when func returns 0 (ie FALSE). The function func will
 * take two arguments the first is param the second is an item of the
 * set. func(param, item).  x_ipc_listIterate returns 0 (FALSE) if the function
 * func returns 0 (FALSE).  Otherwise x_ipc_listIterate returns 1 (TRUE).
 * x_ipc_listIterate will return 0 (FASLE) if the list is NULL.
 *
 * x_ipc_listIterateFromFirst starts iteration from the first item in the list going
 * forward through the list.
 *
 * x_ipc_listIterateFromLast starts iteration from the last item in the list going
 * backwards through the list.
 *
 * char *x_ipc_listPopItem(list)
 * LIST_PTR list
 * Removes and returns first item from list.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: list.c,v $
 * Revision 2.4  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:13  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.3  1996/12/18 15:12:53  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.2  1996/10/18 18:08:58  reids
 * Better handling of internal errors.
 * More complete freeing of memory.
 *
 * Revision 1.1.2.1  1996/10/14 03:47:41  reids
 * Fixed x_ipc_listInsertItemAfter when adding to the end of the list.
 *
 * Revision 1.1  1996/05/09 01:01:36  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/19 03:38:44  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.1  1996/03/03 04:31:47  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.16  1996/06/25  20:50:49  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.15  1996/03/15  21:19:50  reids
 * Update "next" pointer in list when doing deleting items.
 *
 * Revision 1.14  1996/02/10  16:50:06  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.13  1996/01/12  01:02:37  rich
 * Need to return created list.
 *
 * Revision 1.12  1996/01/12  00:53:15  rich
 * Simplified GNUmakefiles.  Fixed some dbmalloc problems.
 *
 * Revision 1.11  1995/12/17  20:21:35  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.10  1995/12/15  01:23:16  rich
 * Moved Makefile to Makefile.generic to encourage people to use
 * GNUmakefile.
 * Fixed a memory leak when a module is closed and some other small fixes.
 *
 * Revision 1.9  1995/07/06  21:16:47  rich
 * Solaris and Linux changes.
 *
 * Revision 1.8  1995/04/07  05:03:21  rich
 * Fixed GNUmakefiles to find the release directory.
 * Cleaned up libc.h file for sgi and vxworks.  Moved all system includes
 * into libc.h
 * Got direct queries to work.
 * Fixed problem in allocating/initializing generic mats.
 * The direct flag (-c) now mostly works.  Connect message has been extended to
 * indicate when direct connections are the default.
 * Problem with failures on sunOS machines.
 * Fixed problem where x_ipcError would not print out its message if logging had
 * not been initialized.
 * Fixed another memory problem in modVar.c.
 * Fixed problems found in by sgi cc compiler.  Many type problems.
 *
 * Revision 1.7  1995/04/04  19:42:29  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.6  1995/01/18  22:41:03  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.5  1993/12/14  17:33:59  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.4  1993/11/21  20:18:11  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  08:38:44  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.2  1993/08/27  07:15:24  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:58  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:25  rich
 * Importing x_ipc version 8
 *
 * Revision 1.2  1993/05/19  17:24:24  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 8-Dec-91  Christopher Fedor at School of Computer Science, CMU
 * Added nextTmp so that listDelete can be used from within x_ipc_listMemReturnItem.
 *
 * 21-Aug-91  Christopher Fedor at School of Computer Science, CMU
 * Added the routine x_ipc_listInsertItemAfter.
 *
 * 22-Dec-90  Christopher Fedor at School of Computer Science, CMU
 * Added tests so that nill can not be inserted in a list.
 * This avoids possible confusion in list iterate and delete functions.
 *
 * 11-Dec-90  Christopher Fedor at School of Computer Science, CMU
 * Added item tests in list iterate so that holes created by listDelete 
 * do no kill the func call. - this may still need work.
 *
 *  3-Oct-90  Christopher Fedor at School of Computer Science, CMU
 * Added nextTmp and previousTmp to list iterate routines so that
 * they terminate gracefully if there test function calls listDelete.
 * This was needed for the removeConnection routine which is called from
 * within a x_ipc_listIterateFromFirst. Just goes to show that there is
 * still much trouble with interactions among generic data structure 
 * routines.
 *
 *  5-Apr-90  Christopher Fedor at School of Computer Science, CMU
 * Added listFreeListGlobal as a list of free top level list elements.
 * Also added warning code for those routines who insist on freeing
 * already freed lists.
 *
 * 13-Nov-89  Christopher Fedor at School of Computer Science, CMU
 * Added listCellFreeListGlobal as a list element free list.
 *
 * 16-Oct-89  Christopher Fedor at School of Computer Science, CMU
 * Added listPushItem, in list.h,  and x_ipc_listPopItem.
 *
 * 13-Oct-89  Christopher Fedor at School of Computer Science, CMU
 * Added x_ipc_listTestDeleteItem and x_ipc_listTestDeleteItemAll routines.
 *
 * 10-Oct-89  Christopher Fedor at School of Computer Science, CMU
 * Added membership and iteration routines so this list abstract 
 * data type can be used as a simple set abstract data type.
 *
 *  9-Oct-89  Christopher Fedor at School of Computer Science, CMU
 * Rewrote to form basis for sets and queues. Updated to Software Standards.
 * Based routine names on functionality, got rid of lispish names.
 * Removed Long-Ji's test for empty list because it was a redundant test
 * - sounds like the real problem was soleved by a correction elsewhere.
 *
 * 29-Aug-89  Long-Ji Lin at School of Computer Science, CMU
 * Modified "DoList" and "DoList1" to make "body" go 
 * after "list_var = Cdr(list_var)"
 *
 * 24-Aug-89  Long-Ji Lin at School of Computer Science, CMU
 * Added a check to return from x_ipc_listDeleteItem (was SetDelete) if the list
 * exists but is empty.
 *
 * Ancient    Reid Simmons at School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 **************************************************************************/

#include "globalM.h"

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

#if !defined(DBMALLOC)
LIST_PTR x_ipc_listDBCreate(const char* file, int line);
#else
#undef x_ipc_listCreate
LIST_PTR x_ipc_listCreate(void);
#endif

#ifdef THREADED
#include "multiThread.h"
MUTEX_TYPE listMutex;

#define   LOCK_LIST_MUTEX  lockMutex(&listMutex)
#define UNLOCK_LIST_MUTEX  unlockMutex(&listMutex)
#else
#define   LOCK_LIST_MUTEX
#define UNLOCK_LIST_MUTEX
#endif

/**************************************************************************
 *
 * FUNCTION: x_ipc_listIncFreeList()
 *
 * DESCRIPTION: Increment list top level free list.
 *
 * INPUTS: none.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: If malloc returns NULL, call x_ipcError.
 *
 * DESIGN: Malloc LIST_INC_AMOUNT number of list elments.
 *
 * NOTES: listFreeListGlobal points to the newly created elements.
 *
 **************************************************************************/

#if defined(DBMALLOC)
static void listDBIncFreeList(const char* file, int line)
#define x_ipc_listIncFreeList() listDBIncFreeList(__FILE__,__LINE__)
#else
static void x_ipc_listIncFreeList(void)
#endif
{
  int32 i;
  LIST_PTR newList;
  
  LOCK_LIST_MUTEX;
  for(i=1;i < LIST_INC_AMOUNT;i++) {
#if defined(DBMALLOC)
    newList = NEW_DB(file,line,LIST_TYPE);
#else
    newList = NEW(LIST_TYPE);
#endif
    
    if (!newList) {
      X_IPC_MOD_ERROR("Error: Can not increment list top level free list.");
      UNLOCK_LIST_MUTEX;
      return;
    }
    
    newList->length = 0;
    newList->first = NULL;
    newList->last = NULL;
    newList->next = NULL;
    
    /* No need to lock M_MUTEX, since list.c is only place this is accessed */
    newList->freeList = GET_M_GLOBAL(listFreeListGlobal);
    GET_M_GLOBAL(listFreeListGlobal) = newList;
  }
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: x_ipc_listIncCellFreeList()
 *
 * DESCRIPTION: Increment list element free list.
 *
 * INPUTS: none.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: If malloc returns NULL, call x_ipcError.
 *
 * DESIGN: Malloc LIST_CELL_INC_AMOUNT number of list elments.
 *
 * NOTES: listCellFreeListGlobal points to the newly created elements.
 *
 **************************************************************************/

static void x_ipc_listIncCellFreeList(void)
{
  int32 i;
  LIST_ELEM_PTR newCell;
  
  LOCK_LIST_MUTEX;
  for(i=1;i<LIST_CELL_INC_AMOUNT;i++) {
    newCell = NEW(LIST_ELEM_TYPE);
    
    if (!newCell) {
      X_IPC_MOD_ERROR("Error: Can not increment list element free list.");
      UNLOCK_LIST_MUTEX;
      return;
    }
    
    newCell->item = NULL;
    newCell->previous = NULL;
    
    /* No need to lock M_MUTEX, since list.c is only place this is accessed */
    newCell->next = GET_M_GLOBAL(listCellFreeListGlobal);
    GET_M_GLOBAL(listCellFreeListGlobal) = newCell;
  }
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listFreeTop(list)
 *
 * DESCRIPTION: Initializes a list top level and returns it to the free list.
 *
 * INPUTS: LIST_PTR list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: Initializes the cell and inserts it as the first item to 
 * listFreeListGlobal.
 *
 * NOTES:
 *
 **************************************************************************/

static void x_ipc_listFreeTop(LIST_PTR list)
{
  list->length = 0;
  list->first = NULL;
  list->last = NULL;
  list->next = NULL;
  
  LOCK_LIST_MUTEX;
  /* No need to lock M_MUTEX, since list.c is only place this is accessed */
  list->freeList = GET_M_GLOBAL(listFreeListGlobal);
  GET_M_GLOBAL(listFreeListGlobal) = list;
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listFreeCell(listCell)
 *
 * DESCRIPTION: Initializes a list element and returns it to the free list.
 *
 * INPUTS: LIST_ELEM_PTR listCell.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: Initializes the cell and inserts it as the first item to 
 * listCellFreeListGlobal.
 *
 * NOTES:
 *
 **************************************************************************/

static void x_ipc_listFreeCell(LIST_ELEM_PTR listCell)
{
  listCell->item = NULL;
  listCell->previous = NULL;
  
  LOCK_LIST_MUTEX;
  /* No need to lock M_MUTEX, since list.c is only place this is accessed */
  listCell->next = GET_M_GLOBAL(listCellFreeListGlobal);
  GET_M_GLOBAL(listCellFreeListGlobal) = listCell;
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listFree(list)
 *
 * DESCRIPTION: Frees a list.
 *
 * INPUTS: LIST_PTR list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: Call x_ipc_listFreeCell on each list element.
 *
 * NOTES: 
 *
 **************************************************************************/

void x_ipc_listFree(LIST_PTR *list)
{
  LIST_ELEM_PTR tmpA, tmpB;
  
  if (!(*list))
    return;
  
  LOCK_LIST_MUTEX;
  if ((*list)->freeList) {
    X_IPC_MOD_WARNING(
		  "\nlistFree: OP IGNORED WARNING: list already on free list.\n");
    UNLOCK_LIST_MUTEX;
    return;
  }
  
  tmpA = (*list)->first;
  
  while (tmpA) {
    tmpB = tmpA;
    tmpA = tmpA->next;
    
    x_ipc_listFreeCell(tmpB);
  }
  
  x_ipc_listFreeTop((*list));
  *list = NULL;
  UNLOCK_LIST_MUTEX;
}    


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listFreeMaster(list)
 *
 * DESCRIPTION: Frees a master list of lists.
 *
 * INPUTS: LIST_PTR list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: Call x_ipc_listFreeCell on each list element.
 *
 * NOTES: 
 *
 **************************************************************************/

static void x_ipc_listFreeMaster(LIST_PTR *list)
{
  if (!(*list))
    return;
  
  LOCK_LIST_MUTEX;
  x_ipc_listFreeMaster(&(*list)->freeList);
  x_ipcFree((char *)*list);
  *list = NULL;
  UNLOCK_LIST_MUTEX;
}    


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listInsertItemFirst(item, list)
 *
 * DESCRIPTION: Adds item as the first item in the list.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  
 * If the list is NULL, return.
 * If the item is NULL, return.
 *
 * DESIGN: 
 * Create a list element to store item and insert the element as the first item
 * in the list.
 *
 * NOTES: If malloc returns NULL simply return <- this is a major problem.
 *
 **************************************************************************/

void x_ipc_listInsertItemFirst(const void *item, LIST_PTR list)
{
  LIST_ELEM_PTR element;
  
  if (!item || !list)
    return;
  
  LOCK_LIST_MUTEX;
  /* No need to lock M_MUTEX, since list.c is only place this is accessed */
  if (!GET_M_GLOBAL(listCellFreeListGlobal))
    x_ipc_listIncCellFreeList();
  
  element = GET_M_GLOBAL(listCellFreeListGlobal);
  GET_M_GLOBAL(listCellFreeListGlobal) =
    GET_M_GLOBAL(listCellFreeListGlobal)->next;
  
  element->item = (const char *)item;
  element->next = list->first;
  element->previous = NULL;
  
  if (!list->first) {
    list->first = element;
  }
  else {
    list->first->previous = element;
  }
  
  if (!list->last) {
    list->last = element;
  }
  
  list->length++;
  list->first = element;
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listInsertItemLast(item, list)
 *
 * DESCRIPTION: Adds item as the last item in the list.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  
 * If the list is NULL, return.
 * If the item is NULL, return.
 *
 * DESIGN: 
 * Create a list element to store item and insert the element as the first item
 * in the list.
 *
 * NOTES: If malloc returns NULL simply return <- this is a major problem.
 *
 **************************************************************************/

void x_ipc_listInsertItemLast(const void *item, LIST_PTR list)
{
  LIST_ELEM_PTR element;
  
  if (!item || !list)
    return;
  
  LOCK_LIST_MUTEX;
  /* No need to lock M_MUTEX, since list.c is only place this is accessed */
  if (!GET_M_GLOBAL(listCellFreeListGlobal))
    x_ipc_listIncCellFreeList();
  
  element = GET_M_GLOBAL(listCellFreeListGlobal);
  GET_M_GLOBAL(listCellFreeListGlobal) = 
    GET_M_GLOBAL(listCellFreeListGlobal)->next;

  element->item = (const char *)item;
  element->next = NULL;
  element->previous = list->last;
  
  if (!list->first) {
    list->first = element;
  }
  
  if (list->last) {
    list->last->next = element;
  }
  
  list->length++;
  list->last = element;
  UNLOCK_LIST_MUTEX;
}



/**************************************************************************
 *
 * FUNCTION: void x_ipc_listInsertItemAfter(item, after, list)
 *
 * DESCRIPTION: Splices item into the list after, after.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * char *after - the item to be inserted after.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  
 * If the list is NULL, return.
 * If the item is NULL, return.
 * If after is NULL call x_ipc_listInsertItemFirst.
 * If after is not found call x_ipc_listInsertItemLast.
 *
 * NOTES: If malloc returns NULL simply return <- this is a major problem.
 *
 **************************************************************************/

void x_ipc_listInsertItemAfter(const void *item, void *after, LIST_PTR list)
{
  LIST_ELEM_PTR element, tmp;
  
  if (!item || !list)
    return;
  
  LOCK_LIST_MUTEX;
  if (!after) {
    x_ipc_listInsertItemFirst(item, list);
    UNLOCK_LIST_MUTEX;
    return;
  }
  
  tmp = list->first;
  
  while (tmp && tmp->item != after) 
    tmp = tmp->next;
  
  if (!tmp || (tmp == list->last)) {
    x_ipc_listInsertItemLast(item, list);
    UNLOCK_LIST_MUTEX;
    return;
  }
  
  /* No need to lock M_MUTEX, since list.c is only place this is accessed */
  if (!GET_M_GLOBAL(listCellFreeListGlobal))
    x_ipc_listIncCellFreeList();
  
  element = GET_M_GLOBAL(listCellFreeListGlobal);
  GET_M_GLOBAL(listCellFreeListGlobal) = 
    GET_M_GLOBAL(listCellFreeListGlobal)->next;
  
  element->item = (const char *)item;
  element->next = tmp->next;
  element->previous = tmp;
  
  tmp->next = element;
  
  list->length++;
  UNLOCK_LIST_MUTEX;
}

/**************************************************************************
 *
 * FUNCTION: char *x_ipc_listPopItem(list)
 *
 * DESCRIPTION: Removes and returns first item from list.
 *
 * INPUTS: 
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS:
 * The first item or NULL
 *
 * EXCEPTIONS:  If the list is NULL or the list is empty, return NULL.
 *
 * DESIGN: 
 * Remove the first list element, return the item and free the element.
 *
 * NOTES:
 * might want to start a free list of list elements.
 *
 **************************************************************************/

const void *x_ipc_listPopItem(LIST_PTR list)
{
  const char *item;
  LIST_ELEM_PTR oldElement;
  
  item = NULL;
  
  LOCK_LIST_MUTEX;
  if (list && list->first) {
    item = list->first->item;
    oldElement = list->first;
    list->first = list->first->next;
    if (list->first) {
      list->first->previous = NULL;
    }
    if (list->last == oldElement) {
      list->last = NULL;
    }
    list->length--;
    x_ipc_listFreeCell(oldElement);
  }
  
  UNLOCK_LIST_MUTEX;
  return item;
}



void x_ipc_listTestDeleteItem(LIST_ITER_FN func, const void *param, LIST_PTR list)
{
  LIST_ELEM_PTR current, previous;
  
  LOCK_LIST_MUTEX;
  if (!list || !list->first) {
    UNLOCK_LIST_MUTEX;
    return;
  }

  current = previous = list->first;
  
  if ((*func)(param, current->item)) {
    /* item is the first element of the list */
    if (list->last == current) {
      list->last = NULL;
    }
    list->first = current->next;

      /* 15-Mar-96 RGS: Maintain the list->next pointer correctly */
    if (list->next == current) 
      list->next = current->next;

    if (current->next) {
      current->next->previous = NULL;
    }
    list->length--;
    x_ipc_listFreeCell(current);
    UNLOCK_LIST_MUTEX;
    return;
  }
  
  current = current->next;
  
  while (current) {
    if ((*func)(param, current->item)) {
      if (list->last == current) {
	list->last = previous;
      }

      /* 15-Mar-96 RGS: Maintain the list->next pointer correctly */
      if (list->next == current)
	list->next = current->next;

      current->previous = previous;
      previous->next = current->next;
      
      if (current->next) {
	current->next->previous = previous;
      }
      
      list->length--;
      x_ipc_listFreeCell(current);
      UNLOCK_LIST_MUTEX;
      return;
    }
    previous = current;
    current = current->next;
  }
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listTestDeleteItemAll(func, param, list)
 *
 * DESCRIPTION: 
 * Removes all items in the list found such that func(param, item)
 * returns 1 (TRUE).
 *
 * INPUTS: 
 * LIST_ITER_FN func - pointer to a test function of the form func(param, item)
 * char *param - a pointer to a parameter for func.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  If the list is NULL or the list is empty, return;
 *
 * DESIGN: 
 * Linearly search the list for a list element containing item,
 * such that func(param, item) returns 1 (TRUE). If found
 * the list element is removed and freed. All the items of the list
 * are tested. Reset element's previous pointer and list->last if needed.
 *
 * NOTES: item is not freed. Use x_ipc_listDeleteItem to delete a single occurance
 * of item. Modified from x_ipc_listDeleteItem.
 *
 **************************************************************************/

void x_ipc_listTestDeleteItemAll(LIST_ITER_FN func, const void *param, LIST_PTR list)
{
  LIST_ELEM_PTR current, previous;
  
  LOCK_LIST_MUTEX;
  if (!list || !list->first) {
    UNLOCK_LIST_MUTEX;
    return;
  }
  
  while (list->first && (*func)(param, list->first->item)) {
    list->length--;
    current = list->first;
    list->first = current->next;

    /* 15-Mar-96 RGS: Maintain the list->next pointer correctly */    
    if (list->next == current)
      list->next = current->next;

    if (list->first) {
      list->first->previous = NULL;
    }
    if (list->last == current) {
      list->last = NULL;
    }
    x_ipc_listFreeCell(current);
  }
  
  if (!list->first) {
    UNLOCK_LIST_MUTEX;
    return;
  }
  
  previous = list->first;
  current  = list->first->next;
  
  while (current) {
    if ((*func)(param, current->item)) {
      if (list->last == current) {
	list->last = previous;
      }
      
      /* 15-Mar-96 RGS: Maintain the list->next pointer correctly */    
      if (list->next == current)
	list->next = current->next;

      previous->next = current->next;
      
      if (current->next) {
	current->next->previous = previous;
      }
      
      list->length--;
      x_ipc_listFreeCell(current);
      
      current = previous->next;
    }
    else {
      previous = current;
      current = current->next;
    }
  }
  UNLOCK_LIST_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listFreeAllItems(func, list)
 *
 * DESCRIPTION: 
 * Applies the func to each item in the list while removing it.
 *
 * INPUTS: 
 * LIST_ITER_FN func - pointer to a function of the form func(item).
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  If the list is NULL or the list is empty, return;
 *
 * DESIGN: 
 * Based on calling x_ipc_listTestDeleteItemAll.
 *
 * NOTES: 
 * The list itself is not freed.
 *
 **************************************************************************/

static int32 x_ipc_listApplyFunc(void (* func)(void *item), void *item)
{
  (*func)(item);
  
  return 1;
}

void x_ipc_listFreeAllItems(LIST_FREE_FN func, LIST_PTR list)
{
  x_ipc_listTestDeleteItemAll((LIST_ITER_FN)x_ipc_listApplyFunc, (const void *)func, list);
}


/**************************************************************************
 *
 * FUNCTION: int32 x_ipc_listItemEq(a, b)
 *
 * DESCRIPTION: Simple Equal Test for x_ipc_listDeleteItem.
 *
 * INPUTS:
 * char *a, *b;
 *
 * OUTPUTS: Returns 1 TRUE or 0 FALSE.
 *
 * DESIGN: return(a == b);
 *
 * NOTES:
 *
 **************************************************************************/

int32 x_ipc_listItemEq(void *a, void *b)
{
  return(a == b);
}


/**************************************************************************
 *
 * FUNCTION: x_ipc_listDeleteItem(item, list)
 *
 * DESCRIPTION: Removes an item from list.
 *
 * INPUTS:
 * char *item; 
 * LIST_PTR *list;
 *
 * OUTPUTS: none.
 *
 * DESIGN: call x_ipc_listTestDeleteItem with x_ipc_listItemEq test.
 *
 * NOTES: list is modified.
 *
 **************************************************************************/

void x_ipc_listDeleteItem(const void *item, LIST_PTR list)
{
  x_ipc_listTestDeleteItem((LIST_ITER_FN) x_ipc_listItemEq, item, list);
}


/**************************************************************************
 *
 * FUNCTION: x_ipc_listDeleteItemAll(item, list)
 *
 * DESCRIPTION: Removes an all such item from list.
 *
 * INPUTS:
 * char *item; 
 * LIST_PTR *list;
 *
 * OUTPUTS: none.
 *
 * DESIGN: call x_ipc_listTestDeleteItemAll with x_ipc_listItemEq test.
 *
 * NOTES: list is modified.
 *
 **************************************************************************/

void x_ipc_listDeleteItemAll(const void *item, LIST_PTR list)
{
  x_ipc_listTestDeleteItemAll((LIST_ITER_FN) x_ipc_listItemEq, item, list);
}


/**************************************************************************
 *
 * FUNCTION: int32 x_ipc_listMemberItem(item, list)
 *
 * DESCRIPTION: Tests if item is an element of list.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: 
 * 0 - FALSE 
 * 1 - TRUE
 *
 * EXCEPTIONS:  If the list is NULL or the list is empty, return;
 *
 * DESIGN: 
 * Linearly search the list for a list element containing item. 
 * If found the value 1 is returned, else the value 0 is returned.
 *
 * NOTES:
 *
 **************************************************************************/

int32 x_ipc_listMemberItem(const void *item, LIST_PTR list)
{
  LIST_ELEM_PTR tmp;
  
  if (!list)
    return 0; /* False */
  
  LOCK_LIST_MUTEX;
  tmp = list->first;
  
  while (tmp) {
    if ((const void *)(tmp->item) == item) {
      UNLOCK_LIST_MUTEX;
      return 1; /* TRUE */
    }
    tmp = tmp->next;
  }
  
  UNLOCK_LIST_MUTEX;
  return 0; /* False */
}


/**************************************************************************
 *
 * FUNCTION: char *x_ipc_listMemReturnItem(func, param, list)
 *
 * DESCRIPTION:
 * x_ipc_listMemReturnItem is a more general form of x_ipc_listMemberItem.
 * x_ipc_listMemReturnItem will return the item (or one of the items) in list
 * for which func(param, item) is non-zero, i.e. is TRUE.
 * The function takes two arguments, the first is the param and the second is 
 * an item of the list and returns an integer value. int func(param, item).
 * If the functions does not satisfy any of the items in the list NULL
 * is returned.
 *
 * INPUTS: 
 * LIST_ITER_FN func;
 * char *param - a pointer to a parameter that is passed to func.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: 
 * A pointer to an item in the list that satisfies func(param, item) 
 * or NULL if no such item exists. 
 *
 * EXCEPTIONS:  If the list is NULL or the list is empty, NULL is returned.
 *
 * DESIGN: 
 * Linearly search the list for a list element containing item, such that
 * func(param, item) is non-zero. Then return the item.
 *
 * NOTES:
 *
 **************************************************************************/

const void *x_ipc_listMemReturnItem(LIST_ITER_FN func, 
			      const void *param, 
			      LIST_PTR list)
{
  const LIST_ELEM_TYPE *tmp, *nextTmp;
  
  if (!list)
    return NULL;
  else {
    LOCK_LIST_MUTEX;
    tmp = list->first;
    while (tmp) {
      nextTmp = tmp->next;
      if ((*func)(param, tmp->item)) {
	UNLOCK_LIST_MUTEX;
	return tmp->item;
      } else
	tmp = nextTmp;
    }
    UNLOCK_LIST_MUTEX;
    return NULL;
  }
}


/**************************************************************************
 *
 * FUNCTION: int32 x_ipc_listIterateFromFirst(func, param, list)
 *
 * DESCRIPTION:
 * x_ipc_listIterateFromFirst will call the function func with param on all of its
 * elements stoping when the list is finished or when func returns 0 (ie
 * FALSE). The function func will take two arguments the first is 
 * param the second is an item of the set. func(param, item).
 *
 * x_ipc_listIterateFromFirst starts from the first item in the list and iterates
 * forward through the items in the list.
 *
 * INPUTS: 
 * LSIT_ITER_FN func;
 * char *param - a pointer to a parameter that is passed to func.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: 
 * x_ipc_listIterate returns 0 (FALSE) if the function func returns 0 (FALSE).
 * Otherwise x_ipc_listIterate returns 1 (TRUE).
 *
 * EXCEPTIONS: 
 *
 * x_ipc_listIterate will return 0 (FASLE) if the list is NULL.
 *
 * DESIGN: 
 * iterate through the list of elements calling func on each item.
 * return when the list is finished or func has returned 0 (FALSE).
 *
 * NOTES:
 *
 * 11-Dec-90: fedor: 
 * Added item test so holes generated by listDelete do not kill func call
 *
 *  3-Oct-90: fedor: 
 * nextTmp allows the iteration to terminate gracefully if the list element
 * is removed if func calls listDelete.
 *
 **************************************************************************/

int32 x_ipc_listIterateFromFirst(LIST_ITER_FN func, const void *param, LIST_PTR list)
{
  const char *item;
  LIST_ELEM_PTR tmp, nextTmp;
  
  if (!list)
    return 0;
  
  LOCK_LIST_MUTEX;
  tmp = list->first;
  
  while (tmp) {
    item = tmp->item;
    nextTmp = tmp->next;
    if (item && !(*func)(param, item)) {
      UNLOCK_LIST_MUTEX;
      return 0;
    }
    tmp = nextTmp;
  }
  
  UNLOCK_LIST_MUTEX;
  return 1;
}


/**************************************************************************
 *
 * FUNCTION: int x_ipc_listIterateFromLast(func, param, list)
 *
 * DESCRIPTION:
 * x_ipc_listIterateFromLast will call the function func with param on all of its
 * elements stoping when the list is finished or when func returns 0 (ie
 * FALSE). The function func will take two arguments the first is 
 * param the second is an item of the set. func(param, item).
 *
 * x_ipc_listIterateFromLast starts with thelast item in the list and iterates
 * backwards through the list.
 *
 * INPUTS: 
 * LIST_ITER_FN func;
 * char *param - a pointer to a parameter that is passed to func.
 * LIST_PTR list - a pointer to a list.
 *
 * OUTPUTS: 
 * x_ipc_listIterateFromLast returns 0 (FALSE) if the function func returns 0 (FALSE)
 * Otherwise x_ipc_listIterateFromLast returns 1 (TRUE).
 *
 * EXCEPTIONS: 
 *
 * x_ipc_listIterate will return 0 (FASLE) if the list is NULL.
 *
 * DESIGN: 
 * iterate through the list of elements calling func on each item.
 * return when the list is finished or func has returned 0 (FALSE).
 *
 * NOTES:
 *
 * 11-Dec-90: fedor: 
 * Added item test so holes generated by listDelete do not kill func call
 *
 *  3-Oct-90: fedor: 
 * previousTmp allows the iteration to terminate gracefully if the list element
 * is removed if func calls listDelete.
 *
 **************************************************************************/

int32 x_ipc_listIterateFromLast(LIST_ITER_FN func, const void *param, LIST_PTR list)
{
  const char *item;
  LIST_ELEM_PTR tmp, previousTmp;
  
  if (!list)
    return 0;
  
  LOCK_LIST_MUTEX;
  tmp = list->last;
  
  while (tmp) {
    item = tmp->item;
    previousTmp = tmp->previous;
    if (item && !(*func)(param, item)) {
      UNLOCK_LIST_MUTEX;
      return 0;
    }
    tmp = previousTmp;
  }
  
  UNLOCK_LIST_MUTEX;
  return 1;
}


/**************************************************************************
 *
 * FUNCTION: LIST_PTR x_ipc_listCopy(list)
 *
 * DESCRIPTION: Copies the given list.
 *
 * INPUTS: LIST_PTR list
 *
 * OUTPUTS: A pointer to the newly created list of type LIST_PTR.
 * If there is an error NULL is returned.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: Iterate through the original list inserting the items into the
 * new list.
 *
 * NOTES:
 *
 **************************************************************************/

static int32 x_ipc_listCopyInsert(LIST_PTR list, const void *item)
{
  x_ipc_listInsertItemFirst(item , list);
  
  return 1;
}

LIST_PTR x_ipc_listCopy(LIST_PTR list)
{
  LIST_PTR newList;
  
  LOCK_LIST_MUTEX;
  newList = x_ipc_listCreate();
  
  (void)x_ipc_listIterateFromLast((LIST_ITER_FN)x_ipc_listCopyInsert, 
			    (char *)newList, list);
  
  UNLOCK_LIST_MUTEX;
  return newList;
}


/**************************************************************************/

/* List interface routines */

void x_ipc_listInsertItem(const void *item, LIST_PTR list)
{
  x_ipc_listInsertItemFirst(item, list);
}

int32 x_ipc_listIterate(LIST_ITER_FN func, const void *param, LIST_PTR list)
{
  return(x_ipc_listIterateFromFirst(func, param, list));
}

int32 x_ipc_listLength(LIST_PTR list)
{
  if (!list)
    return 0;
  
  return(list->length);
}


int32 x_ipc_listEqual(LIST_PTR list1, LIST_PTR list2)
{
  int32 good;
  LIST_ELEM_PTR a, b;
  
  if (list1 == list2)
    return 1;
  
  LOCK_LIST_MUTEX;
  /* this is the same style test used in tms.c but it is not general ! */
  
  a = list1->first;
  b = list2->first;
  
  good = 1;
  while (good && a && b) {
    good = (a->item == b->item);
    a = a->next;
    b = b->next;
  }
  
  UNLOCK_LIST_MUTEX;
  return(good && (a == NULL) && (b == NULL));
}


LIST_PTR x_ipc_listMake1(const void *item)
{
  LIST_PTR list;
  
  list = x_ipc_listCreate();
  
  x_ipc_listInsertItem(item, list);
  
  return list;
}


LIST_PTR x_ipc_listMake2(const void *item1, const void *item2)
{
  LIST_PTR list;
  
  list = x_ipc_listCreate();
  x_ipc_listInsertItem(item2, list);
  x_ipc_listInsertItem(item1, list);
  
  return list;
}


const void *x_ipc_listFirst(LIST_PTR list)
{
  LOCK_LIST_MUTEX;
  if (list && list->first) {
    list->next = list->first;
    UNLOCK_LIST_MUTEX;
    return(list->first->item);
  } else {
    UNLOCK_LIST_MUTEX;
    return NULL;
  }
}

const void *x_ipc_listLast(LIST_PTR list)
{
  LOCK_LIST_MUTEX;
  if (list && list->last) {
    UNLOCK_LIST_MUTEX;
    return(list->last->item);
  } else {
    UNLOCK_LIST_MUTEX;
    return NULL;
  }
}

const void *x_ipc_listNext(LIST_PTR list)
{
  LOCK_LIST_MUTEX;
  if (list && list->next) {
    list->next = list->next->next;
    
    if (list->next) {
      UNLOCK_LIST_MUTEX;
      return(list->next->item);
    }
  }
  UNLOCK_LIST_MUTEX;
  return NULL;
}


/**************************************************************************
 *
 * FUNCTION: void x_ipc_listCleanup(void)
 *
 * DESCRIPTION: 
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 * NOTES:
 *
 **************************************************************************/

static void x_ipc_listFreeElement(LIST_ELEM_PTR element)
{
  if (element == NULL) return;
  x_ipc_listFreeElement(element->next);
  x_ipcFree((char *)element);
}

void x_ipc_listCleanup(void)
{
  LOCK_M_MUTEX;
  x_ipc_listFreeElement(GET_M_GLOBAL(listCellFreeListGlobal));
  GET_M_GLOBAL(listCellFreeListGlobal) = NULL;
  /* Now need to really free the master list of lists */
  x_ipc_listFreeMaster(&GET_M_GLOBAL(listFreeListGlobal));
  UNLOCK_M_MUTEX;
}


/**************************************************************************
 *
 * FUNCTION: LIST_PTR x_ipc_listCreate()
 *
 * DESCRIPTION: Creates an empty list.
 *
 * INPUTS: none.
 *
 * OUTPUTS: A pointer to the newly created empty list of type LIST_PTR.
 * If there is an error NULL is returned.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: malloc up storage for the list, checking to see if space was
 * successfully allocated, and initialize the list structure.
 *
 * NOTES:
 *
 **************************************************************************/

#if defined(DBMALLOC)
LIST_PTR x_ipc_listCreate(void) 
{return x_ipc_listDBCreate("Unknown",-1);}

LIST_PTR x_ipc_listDBCreate(const char* file, int line)

#else
LIST_PTR x_ipc_listDBCreate(const char* file, int line)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  return x_ipc_listCreate();
}

LIST_PTR x_ipc_listCreate(void)
#endif
{
  LIST_PTR list;
  
  LOCK_LIST_MUTEX;
  /* No need to lock M_MUTEX, since list.c is only place this is accessed */
  if (!GET_M_GLOBAL(listFreeListGlobal)) {
#if defined(DBMALLOC)
    listDBIncFreeList(file,line);
#else
    x_ipc_listIncFreeList();
#endif
  }
  
  list = GET_M_GLOBAL(listFreeListGlobal);
  GET_M_GLOBAL(listFreeListGlobal) = 
    GET_M_GLOBAL(listFreeListGlobal)->freeList;
  
  list->freeList = NULL;
  
  UNLOCK_LIST_MUTEX;
  return list;
}

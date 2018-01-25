/**************************************************************************
 * 
 * PROJECT: 
 *
 * (c) Copyright 1997 Reid Simmons.  All rights reserved.
 * 
 * MODULE: strList
 *
 * FILE: strList.c
 *
 * ABSTRACT: A growable variable length array of strings.
 *
 * EXPORTS:
 *
 * STR_LIST_PTR x_ipc_strListCreate()
 * Creates an empty strList.
 *
 * void x_ipc_strListFree(strList)
 * STR_LIST_PTR strList;
 * Frees storage associated with a strList.
 *
 * void x_ipc_strListPush(item, strList)
 * char *item;
 * STR_LIST_PTR strList;
 * Adds item as the first item in the strList.
 *
 * int x_ipc_strListMemberItem(item, strList)
 * char *item;
 * STR_LIST_PTR strList;
 * Returns 1 (TRUE) if item is in the strList, otherwise 0 (FALSE) is returned.
 *
 * char *x_ipc_strListPopItem(strList)
 * STR_LIST_PTR strList
 * Removes and returns first item from strList.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: strList.c,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.1.8.2  1997/03/05 14:10:24  reids
 * Another checkpoint, before going to single pipe per task.
 *
 * Revision 1.2.2.1.8.1  1997/02/25 19:51:19  reids
 * Removed X_IPC messages not needed by IPC.
 * Added "INDEX_TABLE_TYPE" and changed moduleConnectionTable
 *   and externalFdTable to be that type (rather than hash tables).
 * Changed representation of "STR_LIST_TYPE" so that it is not recursively
 *   defined.
 * Removed much of the malloc'ing done when messages are sent/received.
 * Don't do string duplication when unnecessary, especially when parsing
 *   format strings.
 *
 * Revision 1.2.2.1  1996/10/24 15:19:26  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.2  1996/05/09 18:19:30  reids
 * Changes to support CLISP.
 *
 * Revision 1.1  1996/05/09 01:02:01  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:39  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.10  1996/02/10  16:50:32  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.9  1996/01/23  00:06:39  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.8  1996/01/05  16:31:38  rich
 * Added windows NT port.
 *
 * Revision 1.7  1995/12/17  20:22:13  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.6  1995/10/07  19:07:46  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.5  1995/07/19  14:26:33  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.4  1995/07/12  04:55:30  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.3  1995/07/10  16:18:43  rich
 * Interm save.
 *
 * Revision 1.2  1995/04/04  19:43:03  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.1  1995/03/30  15:44:07  rich
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
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 **************************************************************************/

#include "globalM.h"

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

#if !defined(DBMALLOC)
STR_LIST_PTR x_ipc_strListDBCreate(const char* file, int line);
void x_ipc_strListDBPush(const char* file, int line,
		   const char *item, STR_LIST_PTR strList);
void x_ipc_strListDBPushUnique(const char* file, int line,
			 const char *item, STR_LIST_PTR strList);
#else
#undef x_ipc_strListPush
void x_ipc_strListPush(const char *item, STR_LIST_PTR strList);
#undef x_ipc_strListPushUnique
void x_ipc_strListPushUnique(const char *item, STR_LIST_PTR strList);
#undef x_ipc_strListCreate
STR_LIST_PTR x_ipc_strListCreate(void);
#endif


/**************************************************************************
 *
 * FUNCTION: void x_ipc_strListFree(strList)
 *
 * DESCRIPTION: Frees a strList.
 *
 * INPUTS: STR_LIST_PTR strList.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: Call strListFreeCell on each strList element.
 *
 * NOTES: 
 *
 **************************************************************************/

void x_ipc_strListFree(STR_LIST_PTR *strList, BOOLEAN freeString)
{
  int i;
  const char **strings;
  
  if (!(*strList))
    return;
  
  if (freeString) {
    for (i=0, strings=(*strList)->strings; i<(*strList)->length; 
	 i++, strings++) {
      x_ipcFree((char *)*strings);
    }
  }
  x_ipcFree((void *)(*strList)->strings);
  x_ipcFree((void *)(*strList));
  *strList = NULL;
}    


static void strListIncrement(STR_LIST_PTR strList)
{
  int newListSize, currentSize;
  const char **newList;

  currentSize = strList->size;
  newListSize = currentSize + STR_LIST_INC;
  newList = (const char **)x_ipcMalloc(newListSize*sizeof(char *));
  bzero((char *)newList, newListSize*sizeof(char *));
  if (currentSize > 0) {
    /* Copy Old List */
    BCOPY(strList->strings, newList, currentSize*sizeof(char *));
    x_ipcFree((char *)strList->strings);
  }
  strList->size = newListSize;
  strList->strings = newList;
}

/**************************************************************************
 *
 * FUNCTION: void x_ipc_strListPush(item, strList)
 *
 * DESCRIPTION: Adds item as the first item in the strList.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * STR_LIST_PTR strList - a pointer to a strList.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  
 * If the strList is NULL, return.
 * If the item is NULL, return.
 *
 * DESIGN: 
 * Create a strList element to store item and insert the element as the 
 * first item in the strList.
 *
 * NOTES: If malloc returns NULL simply return <- this is a major problem.
 *
 **************************************************************************/

#if defined(DBMALLOC)
void x_ipc_strListPush(const char *item, STR_LIST_PTR strList)
{x_ipc_strListDBPush("Unknown",-1,item,strList);}

void x_ipc_strListDBPush(const char* file, int line,
		   const char *item, STR_LIST_PTR strList)
{
  STR_LIST_PTR element;
  
  element = x_ipc_strListDBCreate(file,line);
  element->item = (char *)item;
  element->next = strList->next;

  strList->next = element;

}

#else
void x_ipc_strListDBPush(const char* file, int line,
		   const char *item, STR_LIST_PTR strList)
{
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  x_ipc_strListPush(item,strList);
}

void x_ipc_strListPush(const char *item, STR_LIST_PTR strList)
{
  if (strList->length == strList->size)
    strListIncrement(strList);
  
  strList->strings[strList->length] = item;
  strList->length++;
}
#endif


/**************************************************************************
 *
 * FUNCTION: void x_ipc_strListPushUnique(item, strList)
 *
 * DESCRIPTION: Adds item as the first item in the strList.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * STR_LIST_PTR strList - a pointer to a strList.
 *
 * OUTPUTS: none.
 *
 * EXCEPTIONS:  
 * If the strList is NULL, return.
 * If the item is NULL, return.
 *
 * DESIGN: 
 * Create a strList element to store item and insert the element as the 
 * first item in the strList.
 *
 * NOTES: If malloc returns NULL simply return <- this is a major problem.
 *
 **************************************************************************/

#if defined(DBMALLOC)
void x_ipc_strListPushUnique(const char *item, STR_LIST_PTR strList)
{x_ipc_strListDBPushUnique("Unknown",-1,item,strList);}

void x_ipc_strListDBPushUnique(const char* file, int line,
			 const char *item, STR_LIST_PTR strList)
{
  if (!x_ipc_strListMemberItem(item,strList))
    x_ipc_strListDBPush(file, line, item, strList);
}

#else
void x_ipc_strListDBPushUnique(const char* file, int line,
			 const char *item, STR_LIST_PTR strList)
{
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  x_ipc_strListPushUnique(item,strList);
}

void x_ipc_strListPushUnique(const char *item, STR_LIST_PTR strList)
{
  if (!x_ipc_strListMemberItem(item,strList))
    x_ipc_strListPush(item, strList);
}
#endif


/**************************************************************************
 *
 * FUNCTION: char *x_ipc_strListPopItem(strList)
 *
 * DESCRIPTION: Removes and returns first item from strList.
 *
 * INPUTS: 
 * STR_LIST_PTR strList - a pointer to a strList.
 *
 * OUTPUTS:
 * The first item or NULL
 *
 * EXCEPTIONS:  If the strList is NULL or the strList is empty, return NULL.
 *
 * DESIGN: 
 * Remove the first strList element, return the item and free the element.
 *
 **************************************************************************/

const char *x_ipc_strListPopItem(STR_LIST_PTR strList)
{
  const char *item;

  if (strList->length == 0) {
    return NULL;
  } else {
    strList->length--;
    item = strList->strings[strList->length];
    strList->strings[strList->length] = NULL;
    return item;
  }
}


/**************************************************************************
 *
 * FUNCTION: int x_ipc_strListMemberItem(item, strList)
 *
 * DESCRIPTION: Tests if item is an element of strList.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * STR_LIST_PTR strList - a pointer to a strList.
 *
 * OUTPUTS: 
 * 0 - FALSE 
 * 1 - TRUE
 *
 * EXCEPTIONS:  If the strList is NULL or the strList is empty, return;
 *
 * DESIGN: 
 * Linearly search the strList for a strList element containing item. 
 * If found the value 1 is returned, else the value 0 is returned.
 *
 * NOTES:
 *
 **************************************************************************/

BOOLEAN x_ipc_strListMemberItem(const char *item, STR_LIST_PTR strList)
{
  int i;
  const char **strings;

  for (i=0, strings=strList->strings; i<strList->length; i++, strings++) {
    if (STREQ(*strings, item)) {
      return TRUE;
    }
  }
  return FALSE;
}


/**************************************************************************/

/* StrList interface routines */

int x_ipc_strListLength(STR_LIST_PTR strList)
{
  return (strList == NULL ? -1 : strList->length);
}

int x_ipc_strListEmpty(STR_LIST_PTR strList)
{
  return (strList == NULL || strList->length == 0);
}

#if 0
int strListEqual(STR_LIST_PTR strList1, STR_LIST_PTR strList2)
{
  
  if (strList1 == strList2)
    return TRUE;
  
  if ((strList1 == NULL) ||(strList1 == NULL))
    return FALSE;

  if ((((strList1->item == NULL) || (strList2->item == NULL)) && 
       (strList1->item != strList2->item))
      || (strcmp(strList1->item,strList2->item) != 0))
    return FALSE;
  
  return(strListEqual(strList1->next, strList2->next));
}


const char *strListFirst(STR_LIST_PTR strList)
{
  
  if (strList == NULL)
    return NULL;
  
  if (strList->next == NULL)
    return NULL;
  
  return strList->next->item;
}

const char *strListLast(STR_LIST_PTR strList)
{
  STR_LIST_PTR tmp;

  if (strList == NULL)
    return NULL;

  if (strList->next == NULL)
    return NULL;

  tmp = strList->next;
  while (tmp->next != NULL)
    tmp = tmp->next;

  return tmp->item;
}
#endif

/**************************************************************************
 *
 * FUNCTION: int x_ipc_strListDeleteItem(item, strList)
 *
 * DESCRIPTION: Tests if item is an element of strList.
 *
 * INPUTS: 
 * char *item - a pointer to an item of data.
 * STR_LIST_PTR strList - a pointer to a strList.
 *
 * OUTPUTS: 
 * 0 - FALSE 
 * 1 - TRUE
 *
 * EXCEPTIONS:  If the strList is NULL or the strList is empty, return;
 *
 * DESIGN: 
 * Linearly search the strList for a strList element containing item. 
 * If found the value 1 is returned, else the value 0 is returned.
 *
 * NOTES:
 *
 **************************************************************************/

int x_ipc_strListDeleteItem(const char *item,
		      STR_LIST_PTR strList,
		      BOOLEAN freeString)
{
  int i, j;
  const char **strings, **strings1;

  for (i=0, strings=strList->strings; i<strList->length; i++, strings++) {
    if (STREQ(*strings, item)) {
      /* Found it, can now delete it. */
      if (freeString)
	x_ipcFree((char *)*strings);
      for (j=i+1, strings1=&strList->strings[i+1]; j<strList->length;
	   j++, strings1++) {
	*(strings1-1) = *strings1;
      }
      *(strings1-1) = NULL;
      strList->length--;
      return 1;
    }
  }
  return 0;
}


/**************************************************************************
 *
 * FUNCTION: STR_LIST_PTR x_ipc_strListCreate()
 *
 * DESCRIPTION: Creates an empty strList.
 *
 * INPUTS: none.
 *
 * OUTPUTS: A pointer to the newly created empty strList of type STR_LIST_PTR.
 * If there is an error NULL is returned.
 *
 * EXCEPTIONS: none.
 *
 * DESIGN: malloc up storage for the strList, checking to see if space was
 * successfully allocated, and initialize the strList structure.
 *
 * NOTES:
 *
 **************************************************************************/
#if defined(DBMALLOC)
STR_LIST_PTR x_ipc_strListCreate(void) 
{return x_ipc_strListDBCreate("Unknown",-1);}

STR_LIST_PTR x_ipc_strListDBCreate(const char* file, int line)
{
  STR_LIST_PTR newList;
  
  newList = x_ipcDBMalloc(file,line,sizeof(STR_LIST_TYPE));
  newList->length = newList->size = 0;
  newList->strings = NULL;

  return newList;
}
#else
STR_LIST_PTR x_ipc_strListDBCreate(const char* file, int line)
{
#ifdef UNUSED_PRAGMA
#pragma unused(file, line)
#endif
  return x_ipc_strListCreate();
}

STR_LIST_PTR x_ipc_strListCreate(void)
{
  STR_LIST_PTR newList;
  
  newList = NEW(STR_LIST_TYPE);
  newList->length = newList->size = 0;
  newList->strings = NULL;

  return newList;
}
#endif

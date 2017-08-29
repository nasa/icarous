/**************************************************************************
 * 
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *
 * (c) Copyright 1997 Reid Simmons.  All rights reserved.
 *
 * MODULE: strList
 *
 * FILE: strList.h
 *
 * ABSTRACT: A growable variable length array of strings.
 *
 * EXPORTS:
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: strList.h,v $
 * Revision 2.3  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:29  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.18.1  1997/02/25 19:51:20  reids
 * Removed X_IPC messages not needed by IPC.
 * Added "INDEX_TABLE_TYPE" and changed moduleConnectionTable
 *   and externalFdTable to be that type (rather than hash tables).
 * Changed representation of "STR_LIST_TYPE" so that it is not recursively
 *   defined.
 * Removed much of the malloc'ing done when messages are sent/received.
 * Don't do string duplication when unnecessary, especially when parsing
 *   format strings.
 *
 * Revision 1.1  1996/05/09 01:02:02  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:41  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.7  1996/02/10  16:50:33  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.6  1996/01/23  00:06:42  rich
 * Fixed memory leak when a module connects and disconnects.  Also fixed a
 * problem with using the direct connection flag.  This was introduced when
 * we added contexts for keeping track of the central server.
 *
 * Revision 1.5  1995/12/17  20:22:15  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.4  1995/10/07  19:07:48  rich
 * Pre-alpha release of x_ipc-8.2.
 * Added PROJECT_DIR. Added x_ipcWillListen.
 * Only transmit broadcast messages when there is a handler to receive them.
 * All system messages now start with "x_ipc_".  Old messages are also supported.
 *
 * Revision 1.3  1995/07/12  04:55:32  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.2  1995/04/04  19:43:04  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.1  1995/03/30  15:44:09  rich
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
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 **************************************************************************/

#ifndef INCstrList
#define INCstrList

#define STR_LIST_INC (4/*8*/)

typedef struct _STR_LIST {
  int length;
  int size;
  const char **strings;
} STR_LIST_TYPE, *STR_LIST_PTR;

#define X_IPC_STR_LIST_FORMAT "{int, int, <string:2>}"

void x_ipc_strListFree(STR_LIST_PTR *strList, BOOLEAN FreeStrings);

#if defined(DBMALLOC)
STR_LIST_PTR x_ipc_strListDBCreate(const char* file, int line);
#define x_ipc_strListCreate() x_ipc_strListDBCreate(__FILE__,__LINE__)
#else
STR_LIST_PTR x_ipc_strListCreate(void);
#endif

#if defined(DBMALLOC)
void x_ipc_strListDBPush(const char* file, int line,
		   const char *item, STR_LIST_PTR strList);
#define x_ipc_strListPush(item,list) x_ipc_strListDBPush(__FILE__,__LINE__,item,list)
#else
void x_ipc_strListPush(const char *item, STR_LIST_PTR strList);
#endif

#if defined(DBMALLOC)
void x_ipc_strListDBPushUnique(const char* file, int line,
			 const char *item, STR_LIST_PTR strList);
#define x_ipc_strListPushUnique(item,list) \
x_ipc_strListDBPushUnique(__FILE__,__LINE__,item,list)
#else
void x_ipc_strListPushUnique(const char *item, STR_LIST_PTR strList);
#endif

const char *x_ipc_strListPopItem(STR_LIST_PTR strList);

BOOLEAN x_ipc_strListMemberItem(const char *item, STR_LIST_PTR strList);

int x_ipc_strListLength(STR_LIST_PTR strList);
int x_ipc_strListEmpty(STR_LIST_PTR strList);
int strListEqual(STR_LIST_PTR strList1, STR_LIST_PTR strList2);

STR_LIST_PTR strListMake1(const void *item);
STR_LIST_PTR strListMake2(const void *item1, const void *item2);

const char *strListFirst(STR_LIST_PTR strList);
const char *strListLast(STR_LIST_PTR strList);

int x_ipc_strListDeleteItem(const char *item, STR_LIST_PTR strList,
		      BOOLEAN freeString);

#define STR_LIST_ITERATE(strList, data, body) \
{\
  int i_strList;\
  const char *data, **strings_strList;\
  for (i_strList=0, strings_strList=(strList)->strings;\
       i_strList<(strList)->length; i_strList++, strings_strList++) {\
    data = *strings_strList; body }}

#define STR_LIST_REDO(strList) { i_strList--; strings_strList--;}

#endif /* INCstrList */

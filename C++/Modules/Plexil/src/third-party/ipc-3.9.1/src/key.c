/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: key
 *
 * FILE: key.c
 *
 * ABSTRACT:
 * 
 * Collection of hash and key functions for table lookups.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: key.c,v $
 * Revision 2.4  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2003/02/13 20:41:10  reids
 * Fixed compiler warnings.
 *
 * Revision 2.2  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.3  1997/01/27 20:09:37  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.1  1997/01/11 01:21:05  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.16.1  1996/12/24 14:37:12  reids
 * Somewhat more efficient way of computing string hashes.
 *
 * Revision 1.1  1996/05/09 01:01:32  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:40  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.10  1996/02/21  18:36:47  rich
 * Remove null keys.
 *
 * Revision 1.9  1996/02/21  18:30:25  rich
 * Created single event loop.
 *
 * Revision 1.8  1995/08/06  16:43:56  reids
 * A bug existed in that two demon monitors that sent the same ID number
 * would conflict (causing the wrong one to fire).  This has been fixed, and
 * in the process, one of the hash-key functions was made a bit more general.
 *
 * Revision 1.7  1995/07/10  16:17:34  rich
 * Interm save.
 *
 * Revision 1.6  1995/01/18  22:40:53  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.5  1994/05/17  23:16:07  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.4  1993/11/21  20:18:01  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:15:13  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:43  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:24  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:16  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:09  fedor
 * Added Logging.
 *
 * 16-Aug-90 Christopher Fedor, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"
#include "key.h"


/******************************************************************************
 *
 * FUNCTION: int x_ipc_intHashFunc(i)
 *
 * DESCRIPTION: Return int value as hash value.
 *
 * INPUTS: int *i;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_intHashFunc(int32 *i)
{
  return(*i);
}


/******************************************************************************
 *
 * FUNCTION: int x_ipc_intKeyEqFunc(a, b)
 *
 * DESCRIPTION: Return comparison of the value of int pointers a and b
 *
 * INPUTS: 
 * int32 *a, *b;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_intKeyEqFunc(int32 *a, int32 *b)
{
  return(*a == *b);
}


/******************************************************************************
 *
 * FUNCTION: int x_ipc_strHashFunc(s)
 *
 * DESCRIPTION: Calculate a simple hash of a NULL terminated string.
 *
 * INPUTS: char *s
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_strHashFunc(const char *s)
{
  int sum;
  char *c;

  for (sum=0, c=(char *)s; *c != '\0'; c++) {
    sum += (isupper((int)*c) ? tolower((int)*c) : *c);
  }

  return sum;
}


/******************************************************************************
 *
 * FUNCTION: int x_ipc_strKeyEqFunc(a, b)
 *
 * DESCRIPTION: 
 * Return 1 (TRUE) if strings a and b are equal. Returns 0 (FASLE) otherwise.
 * Case insensitive compare.
 *
 * INPUTS: char *a, *b
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_strKeyEqFunc(const char *a, const char *b)
{
  int i;
  char a1, b1;
  
  i = 0;
  while (a[i] != '\0' && b[i] != '\0') {
    a1 = a[i];
    if (isupper((int)a1)) a1 = tolower((int)a1);

    b1 = b[i];
    if (isupper((int)b1)) b1 = tolower((int)b1);

    if (a1 != b1)
      return FALSE;
    
    i++;
  }
  
  return (a[i] == b[i]);
}


/******************************************************************************
 *
 * FUNCTION: int x_ipc_intStrHashFunc(key)
 *
 * DESCRIPTION: Calculate a simple hash of an integer and a
 *              NULL terminated string.
 *
 * INPUTS: INT_STR_KEY_PTR key;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_intStrHashFunc(INT_STR_KEY_PTR key)
{
  return (key->num + x_ipc_strHashFunc(key->str));
}

/******************************************************************************
 *
 * FUNCTION: int x_ipc_intStrKeyEqFunc(keyA, keyB)
 *
 * DESCRIPTION: 
 * Return 1 (TRUE) if keyA and keyB are equal (case insensitive compare).
 * Returns 0 (FASLE) otherwise. 
 *
 * INPUTS: INT_STR_KEY_PTR keyA, keyB;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_intStrKeyEqFunc(INT_STR_KEY_PTR keyA, INT_STR_KEY_PTR keyB)
{
  return ((keyA->num == keyB->num) && x_ipc_strKeyEqFunc(keyA->str, keyB->str));
}

/******************************************************************************
 *
 * FUNCTION: int x_ipc_classHashFunc(className)
 *
 * DESCRIPTION: Class hash function.
 *
 * INPUTS: X_IPC_MSG_CLASS_TYPE *className;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_classHashFunc(X_IPC_MSG_CLASS_TYPE *className)
{
  return ((int32)*className);
}


/******************************************************************************
 *
 * FUNCTION: int x_ipc_classEqFunc(classA, classB)
 *
 * DESCRIPTION: Class key equal function.
 *
 * INPUTS: X_IPC_MSG_CLASS_TYPE *classA, *classB;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_classEqFunc(X_IPC_MSG_CLASS_TYPE *classA, X_IPC_MSG_CLASS_TYPE *classB)
{
  return (*classA == *classB);
}

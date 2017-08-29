/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: throughput2.c
 *
 * ABSTRACT: Test the throughput of IPC for various sized messages.
 *           With a command line argument, indicates how many messages central
 *             can send at any one time.
 *
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:59 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: throughput2.c,v $
 * Revision 2.5  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2007/01/07 20:31:00  reids
 * Removed memory leaks
 *
 * Revision 2.3  2000/08/14 21:32:11  reids
 * Added support for making under Windows.
 *
 * Revision 2.2  2000/07/03 17:03:38  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.1  1997/01/25 23:18:22  udo
 * ipc_2_6 to r3 merge
 *
 * Revision 1.2.14.1  1996/12/24 14:18:46  reids
 * Cleaned up the code a bit, and use "IPC_freeByteArray" instead of "free"
 *
 * Revision 1.2  1996/06/17 19:57:59  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:39  rouquett
 * ipc test files
 *
 * Revision 1.1  1996/05/09 01:07:54  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 *
 * Adapted from X_IPC/TCX test code by Terry Fong (NASA Ames)
 *****************************************************************************/

#include <stdlib.h>
#if !defined(VXWORKS)
#include <string.h>
#if defined(WIN32)
#include <winsock.h>
#else
#include <sys/time.h>
#endif
#ifdef macintosh
char *	strdup(const char *str);
#endif
#else
#include <systime.h>
#endif

#include "ipc.h"
#include "throughput.h"

static void MsgHnd (MSG_INSTANCE msgRef, void *data, void *ClientData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(ClientData)
#endif
  static char *lastMsg = "";
  static int msgCount = 0;

  msgCount++;
  if (strcmp(IPC_msgInstanceName(msgRef), lastMsg)) {
    /* New message */
    if (strlen(lastMsg) > 0) free(lastMsg);
    lastMsg = strdup(IPC_msgInstanceName(msgRef));
    msgCount = 1;
  } else if (msgCount == NSEND) {
    /* Respond if this is the last message */
    IPC_publish(DONE_MSG, IPC_FIXED_LENGTH, NULL);
  }
  if (data) IPC_freeByteArray(data);
}

/******************************************************************************
 * RECEIVER main ()
 *****************************************************************************/
#ifdef VXWORKS
void throughput2(int capacity)
#else
int main (int argc, char *argv[])
#endif
{
  IPC_connect("receiver");

#ifdef VXWORKS
  if (capacity > 1) {
    IPC_setCapacity(capacity);
  }
#else
  if (argc > 1) {
    int capacity;
    capacity = atoi(argv[1]);
    IPC_setCapacity(capacity);
  }
#endif

  /* Define the "done" message */
  IPC_defineMsg(DONE_MSG, 0, NULL);

  /* Subscribe to the messages */
  IPC_subscribe(NULL_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT4_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT16_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT64_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT256_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT1K_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT4K_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT16K_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT64K_MSG, MsgHnd, NULL);
  IPC_subscribe(FLOAT256K_MSG, MsgHnd, NULL);
  
  IPC_dispatch();
#if !defined(VXWORKS)
  return 0;
#endif
}

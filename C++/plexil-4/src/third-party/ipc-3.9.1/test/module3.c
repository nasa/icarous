/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: module3.c
 *
 * ABSTRACT: Test program for IPC.
 *             Subscribes to: MSG1, MSG2
 *             Behavior: Prints out the message data received.
 *                       Exits when 'q' is typed at terminal
 *                       Should be run in conjunction with module1 and module2
 *
 * $Revision: 2.2 $
 * $Date: 2009/01/12 15:54:58 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: module3.c,v $
 * Revision 2.2  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.2  1996/10/28 16:14:20  reids
 * Fixed a stupid syntax error.
 *
 * Revision 1.2.2.1  1996/10/28 16:08:53  reids
 * Tests of the new IPC_unmarshallData function.
 *
 * Revision 1.2  1996/06/17 19:57:58  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:36  rouquett
 * ipc test files
 *
 * Revision 1.4  1996/05/09 01:07:52  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 * Revision 1.3  1996/04/24 19:03:58  reids
 * Put test programs in architecture-specific obj and bin sub-directories.
 * Modify test programs for vxworks version (get rid of "main").
 *
 * Revision 1.2  1996/03/12 03:06:12  reids
 * Test programs now illustrate use of "enum" format;
 * Handlers now free data.
 *
 * Revision 1.1  1996/03/06 20:19:31  reids
 * New test programs for passing data between C and LISP modules
 *
 ****************************************************************/

#include "ipc.h"
#ifdef __sgi
#include <stdlib.h>
#endif
#include "module.h"


/* This is copied from module2.c;  The "right" way to do it is to define
   it once, in a separate file, and link both modules with the same function */
static void msg1Handler_3 (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			   void *clientData)
{
  MSG1_TYPE i1;

  IPC_unmarshallData(IPC_msgInstanceFormatter(msgRef), callData,
		     &i1, sizeof(i1));

  printf("msg1Handler: Receiving %s (%d) [%s]\n", 
	  IPC_msgInstanceName(msgRef), i1, (char *)clientData);

  IPC_freeByteArray(callData);
}

/* This is copied from module1.c;  The "right" way to do it is to define
   it once, in a separate file, and link both modules with the same function */
static void msg2Handler_3 (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			   void *clientData)
{
  MSG2_TYPE str1;

  IPC_unmarshallData(IPC_msgInstanceFormatter(msgRef), callData,
		     &str1, sizeof(str1));

  printf("msg2Handler: Receiving %s (%s) [%s]\n", 
	  IPC_msgInstanceName(msgRef), str1, (char *)clientData);

  IPC_freeByteArray(callData);
}

static void stdinHnd_3 (int fd, void *clientData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(fd)
#endif
  char inputLine[81];

  fgets(inputLine, 80, stdin);

  switch (inputLine[0]) {
  case 'q': case 'Q': 
    IPC_disconnect();
    exit(0);
  default: 
    printf("stdinHnd [%s]: Received %s", (char *)clientData, inputLine);
    fflush(stdout);
  }
}

#if defined(VXWORKS)
void module3(void)
#else
int main (void)
#endif
{
  /* Connect to the central server */
  printf("\nIPC_connect(%s)\n", MODULE3_NAME);
  IPC_connect(MODULE3_NAME);

  /* Subscribe to the messages that this module listens to. */
  printf("\nIPC_subscribe(%s, msg1Handler, %s)\n", MSG1, MODULE3_NAME);
  IPC_subscribe(MSG1, msg1Handler_3, MODULE3_NAME);

  /* Subscribe to the messages that this module listens to. */
  printf("\nIPC_subscribe(%s, msg2Handler, %s)\n", MSG2, MODULE3_NAME);
  IPC_subscribe(MSG2, msg2Handler_3, MODULE3_NAME);

#ifndef VXWORKS /* Since vxworks does not handle stdin from the terminal,
		   this does not make sense. */
  /* Subscribe a handler for tty input. Typing "q" will quit the program. */
  printf("\nIPC_subscribeFD(%d, stdinHnd, %s)\n", fileno(stdin), MODULE3_NAME);
  IPC_subscribeFD(fileno(stdin), stdinHnd_3, MODULE3_NAME);

  printf("\nType 'q' to quit\n");
#endif

  IPC_dispatch();
  IPC_disconnect();
#if !defined(VXWORKS)
  return 0;
#endif
}

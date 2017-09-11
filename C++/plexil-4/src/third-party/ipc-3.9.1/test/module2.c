/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: module2.c
 *
 * ABSTRACT: Test program for IPC.
 *             Publishes: MSG2
 *             Subscribes to: MSG1, QUERY1
 *             Responds with: RESPONSE1
 *             Behavior: Listens for MSG1 and prints out message data.
 *                       When QUERY1 is received, publishes MSG1 and
 *                       responds to the query with RESPONSE1.
 *                       Exits when 'q' is typed at terminal.
 *                       Should be run in conjunction with module1
 *
 * $Revision: 2.4 $
 * $Date: 2009/02/07 18:35:29 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: module2.c,v $
 * Revision 2.4  2009/02/07 18:35:29  reids
 * Fixed compiler warnings
 *
 * Revision 2.3  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2007/01/07 20:31:00  reids
 * Removed memory leaks
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.1  1996/10/28 16:08:52  reids
 * Tests of the new IPC_unmarshallData function.
 *
 * Revision 1.2  1996/06/17 19:57:58  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:35  rouquett
 * ipc test files
 *
 * Revision 1.3  1996/04/24 19:03:57  reids
 * Put test programs in architecture-specific obj and bin sub-directories.
 * Modify test programs for vxworks version (get rid of "main").
 *
 * Revision 1.2  1996/03/12 03:06:09  reids
 * Test programs now illustrate use of "enum" format;
 * Handlers now free data.
 *
 * Revision 1.1  1996/03/06 20:19:28  reids
 * New test programs for passing data between C and LISP modules
 *
 ****************************************************************/

#include <stdio.h>
#ifdef __sgi
#include <stdlib.h>
#endif

#include "ipc.h"
#include "module.h"

static void msg1Handler (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			 void *clientData)
{
  MSG1_TYPE i1;

  IPC_unmarshallData(IPC_msgInstanceFormatter(msgRef), callData,
		     &i1, sizeof(i1));

  printf("msg1Handler: Receiving %s (%d) [%s]\n", 
	  IPC_msgInstanceName(msgRef), i1, (char *)clientData);

  IPC_freeDataElements(IPC_msgInstanceFormatter(msgRef), &i1);
  IPC_freeByteArray(callData);
}

static void queryHandler (MSG_INSTANCE msgRef,
			  BYTE_ARRAY callData, void *clientData)
{
  QUERY1_TYPE t1;
  MSG2_TYPE str1 = "Hello, world";
  RESPONSE1_TYPE t2;

  printf("queryHandler: Receiving %s [%s]\n", 
	  IPC_msgInstanceName(msgRef), (char *)clientData);

  /* NOTE: Have to pass a pointer to t1Ptr! */
  IPC_unmarshallData(IPC_msgInstanceFormatter(msgRef), callData,
		     &t1, sizeof(t1));
  IPC_printData(IPC_msgInstanceFormatter(msgRef), stdout, &t1);

  /* Publish this message -- all subscribers get it */
  /* NOTE: You need to pass a *pointer* to the string, 
     not just the string itself! */
  printf("\n  IPC_publishData(%s, &str1) [%s]\n", MSG2, str1);
  IPC_publishData(MSG2, &str1);

  t2.str1 = str1;
  /* Variable length array of one element */
  t2.t1 = &t1;
  t2.count = 1;
  t2.status = ReceiveVal;

  /* Respond with this message -- only the query handler gets it */
  printf("\n  IPC_respondData(%#lX, %s, &t2)\n", (long)msgRef, RESPONSE1);
  IPC_respondData(msgRef, RESPONSE1, &t2);

  IPC_freeDataElements(IPC_msgInstanceFormatter(msgRef), &t1);
  IPC_freeByteArray(callData);
}

static void stdinHnd (int fd, void *clientData)
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
void module2(void)
#else
int main (void)
#endif
{
  /* Connect to the central server */
  printf("\nIPC_connect(%s)\n", MODULE2_NAME);
  IPC_connect(MODULE2_NAME);

  /* Define the messages that this module publishes */
  printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", MSG2, MSG2_FORMAT);
  IPC_defineMsg(MSG2, IPC_VARIABLE_LENGTH, MSG2_FORMAT);

  printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", 
	 RESPONSE1, RESPONSE1_FORMAT);
  IPC_defineMsg(RESPONSE1, IPC_VARIABLE_LENGTH, RESPONSE1_FORMAT);

  /* Subscribe to the messages that this module listens to. */
  printf("\nIPC_subscribe(%s, msg1Handler, %s)\n", MSG1, MODULE2_NAME);
  IPC_subscribe(MSG1, msg1Handler, MODULE2_NAME);

  printf("\nIPC_subscribe(%s, queryHandler, %s)\n", QUERY1, MODULE2_NAME);
  IPC_subscribe(QUERY1, queryHandler, MODULE2_NAME);

#ifndef VXWORKS /* Since vxworks does not handle stdin from the terminal,
		   this does not make sense. */
  /* Subscribe a handler for tty input. Typing "q" will quit the program. */
  printf("\nIPC_subscribeFD(%d, stdinHnd, %s)\n", fileno(stdin), MODULE2_NAME);
  IPC_subscribeFD(fileno(stdin), stdinHnd, MODULE2_NAME);

  printf("\nType 'q' to quit\n");
#endif

  IPC_dispatch();
  IPC_disconnect();
#if !defined(VXWORKS)
  return 0;
#endif
}


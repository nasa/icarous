/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: module1.c
 *
 * ABSTRACT: Test program for IPC.
 *             Publishes: MSG1, QUERY1
 *             Subscribes to: MSG2
 *             Behavior: Sends MSG1 whenever an "m" is typed at the terminal;
 *                       Sends a QUERY1 whenever an "r" is typed;
 *                       Quits the program when a 'q' is typed.
 *                       Should be run in conjunction with module2.
 *
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:58 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: module1.c,v $
 * Revision 2.5  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2008/07/16 00:08:48  reids
 * Updates for newer (pickier) compiler gcc 4.x
 *
 * Revision 2.3  2007/01/07 20:31:00  reids
 * Removed memory leaks
 *
 * Revision 2.2  2001/01/31 17:53:26  reids
 * Subscribe/unsubscribe to connections/disconnections of modules.
 * Subscribe/unsubscribe to changes in handler registrations for a message.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.1  1996/10/28 16:08:50  reids
 * Tests of the new IPC_unmarshallData function.
 *
 * Revision 1.2  1996/06/17 19:57:57  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:34  rouquett
 * ipc test files
 *
 * Revision 1.4  1996/05/09 01:07:52  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 * Revision 1.3  1996/04/24 19:03:57  reids
 * Put test programs in architecture-specific obj and bin sub-directories.
 * Modify test programs for vxworks version (get rid of "main").
 *
 * Revision 1.2  1996/03/12 03:06:07  reids
 * Test programs now illustrate use of "enum" format;
 * Handlers now free data.
 *
 * Revision 1.1  1996/03/06 20:19:24  reids
 * New test programs for passing data between C and LISP modules
 *
 ****************************************************************/

#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159
#endif

#include "ipc.h"
#include "module.h"

static void msg2Handler (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			 void *clientData)
{
  MSG2_TYPE str1;

  IPC_unmarshallData(IPC_msgInstanceFormatter(msgRef), callData,
		     &str1, sizeof(str1));

  printf("msg2Handler: Receiving %s (%s) [%s]\n", 
	  IPC_msgInstanceName(msgRef), str1, (char *)clientData);

  free(str1);
  IPC_freeByteArray(callData);
}

#ifndef VXWORKS
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
  case 'm': case 'M':
    { MSG1_TYPE i1 = 42;
      printf("\n  IPC_publishData(%s, &i1) [%d]\n", MSG1, i1);
      IPC_publishData(MSG1, &i1); 
      break;
    }
  case 'r': case 'R':
    { QUERY1_TYPE t1 = {666, SendVal, {{0.0, 1.0, 2.0}, {1.0, 2.0, 3.0}}, M_PI};
      RESPONSE1_PTR r1Ptr;
      printf("\n  IPC_queryResponseData(%s, &t1, &r1Ptr, IPC_WAIT_FOREVER)\n", 
	     QUERY1);
      IPC_queryResponseData(QUERY1, &t1, (void **)(void *)&r1Ptr,
			    IPC_WAIT_FOREVER); 
      printf("\n  Received response:\n");
      IPC_printData(IPC_msgFormatter(RESPONSE1), stdout, r1Ptr);
      IPC_freeData(IPC_msgFormatter(RESPONSE1), r1Ptr);
      break;
    }
  default: 
    printf("stdinHnd [%s]: Received %s", (char *)clientData, inputLine);
    fflush(stdout);
  }
}
#endif

static void handlerChangeHnd (const char *msgName, int num, void *clientData)
{
  fprintf(stderr, "HANDLER CHANGE: %s: %d\n", msgName, num);
}

static void handlerChangeHnd2 (const char *msgName, int num, void *clientData)
{
  fprintf(stderr, "HANDLER CHANGE2: %s: %d\n", msgName, num);
}

static void connect1Hnd (const char *moduleName, void *clientData)
{
  fprintf(stderr, "CONNECT1: Connection from %s\n", moduleName);
  fprintf(stderr, "          Confirming connection (%d)\n", 
	  IPC_isModuleConnected(moduleName));
}

static void connect2Hnd (const char *moduleName, void *clientData)
{
  fprintf(stderr, "CONNECT2: Connection from %s\n", moduleName);
  fprintf(stderr, "          Number of handlers: %d\n", 
	  IPC_numHandlers(MSG1));
}

static void disconnect1Hnd (const char *moduleName, void *clientData)
{
  static int first = 1;

  fprintf(stderr, "DISCONNECT: %s\n", moduleName);
  if (first) IPC_unsubscribeConnect(connect1Hnd);
  else IPC_unsubscribeConnect(connect2Hnd);
  if (first) IPC_unsubscribeHandlerChange(MSG1, handlerChangeHnd2);
  else IPC_unsubscribeHandlerChange(MSG1, handlerChangeHnd);
  first = 0;
}

void x_ipcRegisterExitProc(void (*proc)(void));

static void safeExit (void)
{
  printf("Don't really exit\n");
}

#if defined(VXWORKS)
#include <sys/times.h>

void module1(void)
#else
int main (void)
#endif
{
  IPC_initialize();
  x_ipcRegisterExitProc(safeExit);

  /* Connect to the central server */
  printf("\nIPC_connect(%s)\n", MODULE1_NAME);
  IPC_connect(MODULE1_NAME);

  IPC_subscribeConnect(connect1Hnd, NULL);
  IPC_subscribeConnect(connect2Hnd, NULL);
  IPC_subscribeDisconnect(disconnect1Hnd, NULL);

  /* Define the named formats that the modules need */
  printf("\nIPC_defineFormat(%s, %s)\n", T1_NAME, T1_FORMAT);
  IPC_defineFormat(T1_NAME, T1_FORMAT);
  printf("\nIPC_defineFormat(%s, %s)\n", T2_NAME, T2_FORMAT);
  IPC_defineFormat(T2_NAME, T2_FORMAT);
  
  /* Define the messages that this module publishes */
  printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n", MSG1, MSG1_FORMAT);
  IPC_defineMsg(MSG1, IPC_VARIABLE_LENGTH, MSG1_FORMAT);

  IPC_subscribeHandlerChange(MSG1, handlerChangeHnd, NULL);
  IPC_subscribeHandlerChange(MSG1, handlerChangeHnd2, NULL);

  printf("\nIPC_defineMsg(%s, IPC_VARIABLE_LENGTH, %s)\n",
	 QUERY1, QUERY1_FORMAT);
  IPC_defineMsg(QUERY1, IPC_VARIABLE_LENGTH, QUERY1_FORMAT);
  IPC_subscribeHandlerChange(QUERY1, handlerChangeHnd, NULL);

  /* Subscribe to the messages that this module listens to.
   * NOTE: No need to subscribe to the RESPONSE1 message, since it is a
   *       response to a query, not a regular subscription! */
  printf("\nIPC_subscribe(%s, msg2Handler, %s)\n", MSG2, MODULE1_NAME);
  IPC_subscribe(MSG2, msg2Handler, MODULE1_NAME);

#ifndef VXWORKS /* Since vxworks does not handle stdin from the terminal,
		   this does not make sense.  Instead, send off messages
		   periodically */
  /* Subscribe a handler for tty input.
     Typing "q" will quit the program; Typing "m" will send MSG1;
     Typing "r" will send QUERY1 ("r" for response) */
  printf("\nIPC_subscribeFD(%d, stdinHnd, %s)\n", fileno(stdin), MODULE1_NAME);
  IPC_subscribeFD(fileno(stdin), stdinHnd, MODULE1_NAME);

  printf("\nType 'm' to send %s; Type 'r' to send %s; Type 'q' to quit\n",
	 MSG1, QUERY1);

  IPC_dispatch();
#else
#define NUM_MSGS (10)
#define INTERVAL  (5)
  { 
    int i;
    printf("\nWill send a message every %d seconds for %d seconds\n", 
	   INTERVAL, NUM_MSGS);
    for (i=1; i<NUM_MSGS; i++) {
      /* Alternate */
      if (i & 1) {
	MSG1_TYPE i1 = 42;
	printf("\n  IPC_publishData(%s, &i1) [%d]\n", MSG1, i1);
	IPC_publishData(MSG1, &i1); 
      } else {
	QUERY1_TYPE t1 = {666, SendVal, {{0.0, 1.0, 2.0}, {1.0, 2.0, 3.0}}, M_PI};
	RESPONSE1_PTR r1Ptr;
	printf("\n  IPC_queryResponseData(%s, &t1, &r1Ptr, IPC_WAIT_FOREVER)\n", 
	       QUERY1);
	IPC_queryResponseData(QUERY1, &t1, (void **)&r1Ptr, IPC_WAIT_FOREVER); 
	printf("\n  Received response:\n");
	IPC_printData(IPC_msgFormatter(RESPONSE1), stdout, r1Ptr);
	IPC_freeData(IPC_msgFormatter(RESPONSE1), r1Ptr);
      }
      /* This works instead of sleep */
      { struct timeval sleep = {INTERVAL, 0};
        select(FD_SETSIZE, NULL, NULL, NULL, &sleep);
      }
    }
  }
#endif
  IPC_disconnect();
  printf("Made it safely to the end\n");
#if !defined(VXWORKS)
  return 0;
#endif
}

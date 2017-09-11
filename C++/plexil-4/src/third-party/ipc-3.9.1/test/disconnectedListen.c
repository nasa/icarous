/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * AUTHOR: Trey Smith
 * FILE: disconnectedListen.c
 *
 * ABSTRACT: Test the ability of IPC to wait for timers and streams (as in
 *           IPC_subscribeFD) while not connected to the server.  This
 *           program waits for input on stdin and echoes it, and also prints
 *           "timer went off" every 2 seconds.  It should not hang or return an
 *           error.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:58 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: disconnectedListen.c,v $
 * Revision 2.4  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2005/12/30 17:01:48  reids
 * Support for Mac OSX
 *
 * Revision 2.2  2003/03/12 05:14:15  trey
 * added regression test for bug with IPC_connect() clobbering previous IPC_subscribeFD() calls
 *
 * Revision 2.1  2003/02/16 16:42:59  trey
 * initial check-in
 *
 *
 ****************************************************************/

#include "ipc.h"
#include <time.h>

static void timerHandler (void *string, unsigned long currentTime, 
			  unsigned long scheduledTime)
{ 
  printf("timer went off.\n");
}

static void stdinHandler(int fd, void *client_data) {
  char buf[512];
  fgets(buf, sizeof(buf), stdin);
  printf("got stdin input: %s\n", buf);
}

static void disconnectedListen(void)
{
  time_t startTime;

  /*  *** don't *** connect to the central server */
  IPC_initialize();

  /* set up handlers */
  IPC_addTimer(2000, TRIGGER_FOREVER, timerHandler, 0);  /* 2000 is wait time in msecs */
  IPC_subscribeFD(fileno(stdin), stdinHandler, 0);

  printf("dispatching for ~5 seconds\n");
  startTime = time(0);
  while (time(0) - startTime <= 5) {
    IPC_listenClear(100);
  }

  printf("connecting to ipc central server\n");
  IPC_connect("foo");

  IPC_dispatch();
}

#if !defined(VXWORKS)
int main (void)
{
  disconnectedListen();
  return 0;
}
#endif

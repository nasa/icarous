/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: latency2.c
 *
 * ABSTRACT: Test the latency of IPC for various sized messages.
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
 * $Log: latency2.c,v $
 * Revision 2.2  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.4  1997/01/25 23:18:15  udo
 * ipc_2_6 to r3 merge
 *
 * Revision 1.2.2.3.6.1  1996/12/24 14:18:44  reids
 * Cleaned up the code a bit, and use "IPC_freeByteArray" instead of "free"
 *
 * Revision 1.2.2.3  1996/10/24 17:27:26  reids
 * Remove the artificial delay on VxWorks
 *
 * Revision 1.2.2.2  1996/10/24 15:17:46  reids
 * Compile for vxworks; Add more stats.
 *
 * Revision 1.2.2.1  1996/10/22 17:33:34  reids
 * Small mods to make it give more reasonable results.
 *
 * Revision 1.2  1996/06/17 19:57:57  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:33  rouquett
 * ipc test files
 *
 * Revision 1.1  1996/05/09 01:07:51  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 *
 *****************************************************************************/
#include <sys/types.h>
#ifdef VXWORKS
#include <sys/times.h>
#elif defined(macintosh)
extern struct timeval *gettimeofday(struct timeval *, void *);
#include "unistd.h"
#else
#include <sys/time.h>
#endif

#include "ipc.h"
#include "latency.h"

/******************************************************************************
 * SENDER main routine
 *****************************************************************************/

static void latency2(void)
{
  int j;
  struct timeval now;
#ifndef VXWORKS
  struct timeval  sleep;
#endif

#ifdef VXWORKS
  { int currentPriority;
  /* The producer needs to run at a lower priority, else it swamps
     central and the consumer */
  taskPriorityGet(taskIdSelf(), &currentPriority);
  taskPrioritySet(taskIdSelf(), currentPriority+10);
  }
#endif

  IPC_connect("sender");

  /*  TEST_init(); installSwitchHook(); TEST_start();*/
  for (j = 0; j<NSEND; j++) {
    gettimeofday(&now, NULL);
    IPC_publish(MSG_NAME, IPC_FIXED_LENGTH, &now);
#ifndef VXWORKS
    sleep.tv_sec = 0; sleep.tv_usec = 1000;
    select(FD_SETSIZE, NULL, NULL, NULL, &sleep);
#endif
  }
  /* TEST_stop();TEST_transferMeasurementsToFile();*/
  IPC_disconnect();
}

#ifndef VXWORKS
int main (void)
{
  latency2();
  return 0;
}
#endif

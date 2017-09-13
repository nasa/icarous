/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1997 Reid Simmons.  All rights reserved.
 *
 * FILE: timerTest.c
 *
 * ABSTRACT: Test the timer feature of IPC 
 *           (needed for JSC Aercam project).
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:59 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: timerTest.c,v $
 * Revision 2.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
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
 ****************************************************************/

#include "ipc.h"
#include <time.h>
unsigned long x_ipc_timeInMsecs(void);

#define TASK_NAME "timer-test"

#define TIMER1_PERIOD (1000) /* Msecs */
#define TIMER1_COUNT  TRIGGER_FOREVER

#define TIMER2_PERIOD (700) /* Msecs */
#define TIMER2_COUNT  TRIGGER_FOREVER

#define TIMER3_PERIOD (1500) /* Msecs */
#define TIMER3_COUNT  20

#define TIMER4_PERIOD (2000) /* Msecs */
#define TIMER4_COUNT  1

static unsigned long realStart;

static void timerHandler (char *string, unsigned long currentTime, 
			   unsigned long scheduledTime)
{
  printf("Timer %s: Called at %ld (%ld), scheduled at %ld, delta %ld\n",
	 string, currentTime, currentTime - realStart, scheduledTime, 
	 (currentTime - scheduledTime));
}

static void timer1Handler (void *string, unsigned long currentTime, 
			   unsigned long scheduledTime)
{
  timerHandler((char *)string, currentTime, scheduledTime);
}

static void timer2Handler (void *string, unsigned long currentTime, 
			   unsigned long scheduledTime)
{
  static int count = 0;

  timerHandler((char *)string, currentTime, scheduledTime);

  count++;
  if (count == 10)
    IPC_removeTimer(timer2Handler);
}

static void timer4Handler (void *string, unsigned long currentTime, 
			   unsigned long scheduledTime)
{ 
  timerHandler((char *)string, currentTime, scheduledTime);
}

static void timer3Handler (void *string, unsigned long currentTime, 
			   unsigned long scheduledTime)
{
  static int count = 0;

  timerHandler((char *)string, currentTime, scheduledTime);

  count++;
  if (count == 10) {
    IPC_addTimer(TIMER4_PERIOD, TIMER4_COUNT, timer4Handler, "timer4");
    printf("Added timer4 at %ld\n", x_ipc_timeInMsecs() - realStart);
  }
}

/* Prototype to keep compiler happy */
void timerTest(void);

void timerTest(void)
{
  /* Connect to the central server */
  IPC_connect(TASK_NAME);

  /* Add some timers */

  realStart = x_ipc_timeInMsecs();
  IPC_addTimer(TIMER1_PERIOD, TIMER1_COUNT, timer1Handler, "timer1a");
  IPC_addTimer(TIMER1_PERIOD, TIMER1_COUNT, timer1Handler, "timer1b");
  IPC_addTimer(TIMER2_PERIOD, TIMER2_COUNT, timer2Handler, "timer2");
  IPC_addTimer(TIMER3_PERIOD, TIMER3_COUNT, timer3Handler, "timer3");

  IPC_listen(2000);
  printf("  Exit IPC_listen(2000)\n");

  IPC_dispatch();

  /* Should never reach here, but just in case */
  IPC_disconnect();
}

#if !defined(VXWORKS)
int main (void)
{
  timerTest();
  return 0;
}
#endif

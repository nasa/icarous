/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: latency1.c
 *
 * ABSTRACT: Test the latency of IPC for various sized messages.
 *           With a command line argument, indicates how many messages central
 *             can send at any one time.
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
 * $Log: latency1.c,v $
 * Revision 2.2  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.2  1997/01/25 23:18:14  udo
 * ipc_2_6 to r3 merge
 *
 * Revision 1.2.2.1.6.1  1996/12/24 14:18:43  reids
 * Cleaned up the code a bit, and use "IPC_freeByteArray" instead of "free"
 *
 * Revision 1.2.2.1  1996/10/24 15:17:45  reids
 * Compile for vxworks; Add more stats.
 *
 * Revision 1.2  1996/06/17 19:57:56  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:32  rouquett
 * ipc test files
 *
 * Revision 1.1  1996/05/09 01:07:51  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 *
 *****************************************************************************/

#include <stdlib.h>
#include <math.h>

#ifdef VXWORKS
#include <sys/times.h>
#include <math.h>
#elif defined(macintosh)
#include <sys/time.h>
extern struct timeval *gettimeofday(struct timeval *, void *);
#else
#include <sys/time.h>
#endif

#include "ipc.h"
#include "latency.h"

/******************************************************************************
 *
 * sslCalcTimevalDiff - calculate the difference between two timevals in
 * seconds
 *
 * INPUTS:
 *   pT1		pointer to first timeval
 *   pT2		pointer to first timeval
 *
 * RETURNS:
 *   Difference between timevals in seconds (pT1 - pT2)
 *
 *****************************************************************************/
static double sslCalcTimevalDiff (struct timeval *pT1, struct timeval *pT2)
{
  return ((double) (pT1->tv_sec - pT2->tv_sec) +
          ((double) (pT1->tv_usec - pT2->tv_usec)) / 1000000.0);
}

static void MsgHnd (MSG_INSTANCE msgRef, void *data, void *ClientData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(msgRef, ClientData)
#endif
  static double sum = 0.0;
  static double sumSq = 0.0;
  static double max = 0.0;
  static double min = 1.0e10;
  static int msgCount = 0;  
  static struct timeval firstTime;
  struct timeval now, *before;
  double diff, ave;

  gettimeofday(&now, NULL);
  msgCount++;
  before = (struct timeval *)data;
  diff = 1000*sslCalcTimevalDiff(&now, before);
  sum += diff;
  sumSq += diff*diff;
  if (diff > max) max = diff;
  if (diff < min) min = diff;

  if (msgCount == 1) {
    firstTime = now;
  } else if (msgCount == NSEND) {
    ave = sum/msgCount;
    diff = sslCalcTimevalDiff(&now, &firstTime);
    printf("Latency for %d messages: Max: %.1f msecs, Min: %.1f, Mean: %.1f (%.1f), Std Dev: %.1f\n",
	   msgCount, max, min, ave, 1000*(diff/msgCount), 
	   sqrt(sumSq/msgCount - (ave*ave)));
    msgCount = 0;
    max = sum = sumSq = 0.0;
    min = 1.0e10;
  }
  IPC_freeByteArray(data);
}

/******************************************************************************
 * RECEIVER main routine
 *****************************************************************************/

static void latency1(int capacity)
{
  IPC_connect("receiver");

  if (capacity > 1) {
    IPC_setCapacity(capacity);
  }

  /* Define the message -- send raw data, don't confuse the timings
     with the time to marshall/unmarshall the data */
  IPC_defineMsg(MSG_NAME, sizeof(struct timeval), NULL);
  IPC_subscribe(MSG_NAME, MsgHnd, NULL);

  IPC_dispatch();
}

#ifndef VXWORKS
int main (int argc, char *argv[])
{
  int capacity;

  capacity =  (argc > 1 ? atoi(argv[1]) : 1);

  latency1(capacity);
  return 0;
}
#endif

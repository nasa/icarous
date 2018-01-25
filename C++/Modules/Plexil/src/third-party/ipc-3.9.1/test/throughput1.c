/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: throughput1.c
 *
 * ABSTRACT: Test the throughput of IPC for various sized messages.
 *
 * $Revision: 2.7 $
 * $Date: 2009/01/12 15:54:59 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: throughput1.c,v $
 * Revision 2.7  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2008/07/16 00:08:48  reids
 * Updates for newer (pickier) compiler gcc 4.x
 *
 * Revision 2.5  2007/01/07 20:31:00  reids
 * Removed memory leaks
 *
 * Revision 2.4  2005/12/30 17:01:48  reids
 * Support for Mac OSX
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
 * Revision 1.2  1996/06/17 19:57:59  reids
 * Updated test programs to adhere to new Makefiles (mbuild) and directory
 *   structure ("ipc/ipc.h").
 *
 * Revision 1.1  1996/06/17 18:43:39  rouquett
 * ipc test files
 *
 * Revision 1.1  1996/05/09 01:07:53  reids
 * Added latency and throughput tests.
 * Modified to work on vxworks (m68k).
 *
 *
 * Adapted from X_IPC/TCX test code by Terry Fong (NASA Ames)
 *****************************************************************************/
#include <sys/types.h>
#include <unistd.h>
#if defined(VXWORKS)
#include <systime.h>
#elif defined(macintosh)
#include <sys/time.h>
extern struct timeval *gettimeofday (struct timeval *, void *dummy);
extern int select (int, fd_set*, fd_set*, fd_set*, const struct timeval*);
#elif defined(__APPLE__)
#include <sys/time.h>
#include <stdlib.h>
#elif defined(WIN32)
#include <winsock.h>
#else
#include <sys/time.h>
#include <malloc.h>
#endif

#include "ipc.h"
#include "throughput.h"


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
static float sslCalcTimevalDiff (struct timeval *pT1, struct timeval *pT2)
{
  return ((float) (pT1->tv_sec - pT2->tv_sec) +
          ((float) (pT1->tv_usec - pT2->tv_usec)) / 1000000.0);
}

static void doneHandler (MSG_INSTANCE msgRef, BYTE_ARRAY callData,
			 void *clientData)
{
#ifdef UNUSED_PRAGMA
#pragma unused(msgRef, callData, clientData)
#endif
}

static void sendMessages (const char *msgName, int size, void *data)
{
  struct timeval sTime, eTime, sleep;
  int i, nBytes;
  float secs;

  /* Send one message initially to prime the pump.  */
  IPC_publish(msgName, IPC_FIXED_LENGTH, data);
  /* This works instead of sleep */
  sleep.tv_sec = 1; sleep.tv_usec = 0;
  select(FD_SETSIZE, NULL, NULL, NULL, &sleep);

  gettimeofday (&sTime, NULL);
  for (i=0; i<NSEND; i++) {
    IPC_publish(msgName, IPC_FIXED_LENGTH, data);
#ifdef VXWORKS
    /* This works instead of sleep -- need to do this so the producer
       gets swapped out */
    sleep.tv_sec = 0; sleep.tv_usec = size/4096; /* 1 msec for each 4K */
    select(FD_SETSIZE, NULL, NULL, NULL, &sleep);
#endif
  }
  /* Wait for the done message */
  IPC_listen(IPC_WAIT_FOREVER);
  gettimeofday (&eTime, NULL);
  secs = sslCalcTimevalDiff (&eTime, &sTime);
  nBytes = size * NSEND;
    
  printf ("Sent %d bytes in %.2f seconds (%.2f bytes/sec)\n",
	  nBytes, secs, (float) nBytes / secs);
  printf ("%d messages (%d bytes each) at %.0f messages/sec (%.1f msecs/msg)\n",
	  NSEND, size, (float) NSEND / secs, 1.0e3*secs/(float)NSEND);
  fflush(stdout);
}


/******************************************************************************
 * main ()
 *****************************************************************************/
#ifdef VXWORKS
void throughput1(void)
#else
int main (void)
#endif
{
  int j;
  float *data;

#ifdef VXWORKS
  { int currentPriority;
  /* The producer needs to run at a lower priority, else it swamps
     central and the consumer */
  taskPriorityGet(taskIdSelf(), &currentPriority);
  taskPrioritySet(taskIdSelf(), currentPriority+10);
  }
#endif

  data = (float *)malloc(MAX_DATA_SIZE);
  
  for (j = 0; j<MAX_DATA_SIZE/sizeof(float); j++)
    data[j] = (float) j;

  IPC_connect("sender");
  IPC_defineMsg(NULL_MSG, 0, NULL);
  IPC_defineMsg(FLOAT4_MSG, sizeof(float)*4, NULL);
  IPC_defineMsg(FLOAT16_MSG, sizeof(float)*16, NULL);
  IPC_defineMsg(FLOAT64_MSG, sizeof(float)*64, NULL);
  IPC_defineMsg(FLOAT256_MSG, sizeof(float)*256, NULL);
  IPC_defineMsg(FLOAT1K_MSG, sizeof(float)*1024, NULL);
  IPC_defineMsg(FLOAT4K_MSG, sizeof(float)*4*1024, NULL);
  IPC_defineMsg(FLOAT16K_MSG, sizeof(float)*16*1024, NULL);
  IPC_defineMsg(FLOAT64K_MSG, sizeof(float)*64*1024, NULL);
  IPC_defineMsg(FLOAT256K_MSG, sizeof(float)*256*1024, NULL);

  IPC_subscribe(DONE_MSG, doneHandler, NULL);
  
  sendMessages(NULL_MSG, 0, data);
  sendMessages(FLOAT4_MSG, sizeof(float)*4, data);
  sendMessages(FLOAT16_MSG, sizeof(float)*16, data);
  sendMessages(FLOAT64_MSG, sizeof(float)*64, data);
  sendMessages(FLOAT256_MSG, sizeof(float)*256, data);
  sendMessages(FLOAT1K_MSG, sizeof(float)*1024, data);
  sendMessages(FLOAT4K_MSG, sizeof(float)*4*1024, data);
  sendMessages(FLOAT16K_MSG, sizeof(float)*16*1024, data);
  sendMessages(FLOAT64K_MSG, sizeof(float)*64*1024, data);
  sendMessages(FLOAT256K_MSG, sizeof(float)*256*1024, data);

  /* Wait for everything to settle down before shutting down */
  sleep(2);

  /* shut down */
  printf ("\n");
  fflush(stdout);
  IPC_disconnect();
#if !defined(VXWORKS)
  return 0;
#endif
}

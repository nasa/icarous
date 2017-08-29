/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: marshallTest.c
 *
 * ABSTRACT: Test program for IPC marshalling/unmarshalling
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
 * $Log: marshallTest.c,v $
 * Revision 2.4  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2005/12/30 17:01:48  reids
 * Support for Mac OSX
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
 * Revision 1.1.2.7  1997/01/25 23:18:17  udo
 * ipc_2_6 to r3 merge
 *
 * Revision 1.1.2.6.6.1  1996/12/24 14:18:45  reids
 * Cleaned up the code a bit, and use "IPC_freeByteArray" instead of "free"
 *
 * Revision 1.1.2.6  1996/10/24 15:17:01  reids
 * Compile for vxworks
 *
 * Revision 1.1.2.5  1996/10/22 18:04:36  reids
 * Make the program work for IPC 2.4, as well as IPC 2.5
 *
 * Revision 1.1.2.4  1996/10/08 14:25:44  reids
 * Changes to enable the test program to run on VxWorks
 *
 * Revision 1.1.2.3  1996/10/04 17:49:24  reids
 * Bug fix to enable it to handle (empty) strings and (null) pointers.
 *
 * Revision 1.1.2.2  1996/10/03 06:20:57  reids
 * Removed an overflow error by using doubles, rather than longs.
 *
 * Revision 1.1.2.1  1996/10/03 02:24:51  reids
 * Timing tests for the marshall/unmarshall functions.
 *
 ****************************************************************/

#include <stdlib.h>
#include <math.h>
#ifdef VXWORKS
#include <sys/times.h>
#else
#include <string.h>
#ifdef __sgi
#include <strings.h>
#endif
#include <sys/time.h>
#if defined(__APPLE__)
#include <stdlib.h>
#elif !defined(macintosh)
#include <malloc.h>
#else
#include <compat.h>
extern struct timeval *gettimeofday(struct timeval *, void *dummy);
#endif
#endif

#include "ipc.h"
#ifdef _WINSOCK_
#undef BOOLEAN		/* Need int_32 sized booleans instead of char size */
#define BOOLEAN int	/* This gets typedef'd in the MSC includes */
#endif
#include "basics.h"
#include "tca.h"
#include "formatters.h"
#include "tcaMem.h"

#define DEFAULT_TRIALS (1000)
#ifndef macintosh
#define MAX_DATA_SIZE  (100000)
#else
#define MAX_DATA_SIZE  (32000)
#endif
#define ONE_MILLION    (1000000)

#define TIME_TRIAL(code) \
 { struct timeval t1, t2; \
   unsigned long max, dt; \
   double sum, sumSq, ave, sd, var;\
   sum = sumSq = 0.0; max = 0; \
   gettimeofday(&t1, NULL); \
   for (i=0; i<numTrials; i++) {  \
     code; \
     gettimeofday(&t2, NULL); \
     dt = subTime(t2, t1); \
     if (dt > max) max = dt; \
     sum += dt;  sumSq += dt*dt; \
     t1 = t2; \
   } \
   ave = sum/numTrials;  var = (sumSq/numTrials - ave*ave);\
   sd = (var > 0.0 ? sqrt(var) : -1); \
   printf("  %d trials in %.0f msecs (max: %.3f, ave: %.3f, sd: %.3f)\n", \
	  numTrials, sum/1000, (double)max/1000, ave/1000, sd/1000); }

/* Compute t1 - t2 */
static unsigned long subTime (struct timeval t1, struct timeval t2)
{
  if (t1.tv_usec < t2.tv_usec) {
    t1.tv_usec += ONE_MILLION; t1.tv_sec--;
  }
  return (t1.tv_sec-t2.tv_sec)*ONE_MILLION + (t1.tv_usec-t2.tv_usec);
}

#ifdef VXWORKS
void marshallTest (char *formatString, int numTrials)
#else
int main (int argc, char **argv)
#endif
{
#ifndef VXWORKS
  int numTrials;
  char *formatString;
#endif
  int i;
  FORMATTER_PTR format;
  char data[MAX_DATA_SIZE]; /* No error checking if the format is too big */
  void *dataPtr;
  IPC_VARCONTENT_TYPE vc;

  bzero(data, MAX_DATA_SIZE);
#ifndef VXWORKS
  if (argc == 1) {
    fprintf(stderr, "Usage %s: <format_string> [num_trials]\n", argv[0]);
    exit(-1);
  }

  formatString = argv[1];
  numTrials = (argc > 2 ? atoi(argv[2]) : DEFAULT_TRIALS);
#endif
  IPC_initialize();

  format = IPC_parseFormat(formatString);
  if (!format) {
    fprintf(stderr, "Illegal format string: %s\n", formatString);
    exit(-1);
  }

  /* Marshall tests */
  printf("Marshalling trial with %s\n", formatString);
  printf("  Struct Size: %d, Buffer Size: %d (%s size)\n",
	 x_ipc_dataStructureSize(format), 
	 (formatContainsPointers(format) ? -1 : x_ipc_bufferSize(format, NULL)),
	 (x_ipc_sameFixedSizeDataBuffer(format) ? "fixed" : "variable"));

  TIME_TRIAL({ IPC_marshall(format, &data, &vc);
	       IPC_freeByteArray(vc.content);})

  /* Unmarshall tests */
  IPC_marshall(format, &data, &vc);

  printf("Bcopy (%d bytes):\n", vc.length);
  TIME_TRIAL(bcopy(data, vc.content, vc.length););

  printf("Malloc and free (%d bytes):\n", vc.length);
  TIME_TRIAL({ dataPtr = (void *)x_ipcMalloc(vc.length); 
               free(dataPtr);});

  printf("Unmarshalling trial (%d bytes):\n", vc.length);
  TIME_TRIAL({ IPC_unmarshall(format, vc.content, &dataPtr);
	       free(dataPtr);});

  /* How much does the timing itself take? */
  printf("Timing alone:\n");
  TIME_TRIAL(1);  

  exit(0);
#if !defined(VXWORKS)
  return 0;
#endif
}

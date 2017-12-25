/******************************************************************************
 *
 * PROJECT: IPC: Inter-Process Communication Package
 * 
 * (c) Copyright 2001 Reid Simmons.  All rights reserved.
 *
 * FILE: multiThread.c
 *
 * ABSTRACT: Enable IPC to deal with multi-threaded programs.
 *           Mutexes loosely based on David Apfelbaum's THREADS package.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: multiThread.c,v $
 * Revision 2.6  2011/04/21 18:17:49  reids
 * IPC 3.9.0:
 * Added NoListen options to IPC_connect, to indicate that module will not
 *   periodically listen for messages.
 * Bug where having a message id of 0 or 1 interfaces with direct message
 *   functionality.
 * Extended functionality of "ping" to handle race condition with concurrent
 *   listens.
 * Fixed bug in how IPC_listenWait was implemented (did not necessarily
 *   respect the timeout).
 * Fixed conditions under which module listens for handler updates.
 *
 * Revision 2.5  2010/12/17 19:20:23  reids
 * Split IO mutex into separate read and write mutexes, to help minimize
 *   probability of deadlock when reading/writing very big messages.
 * Fixed a bug in multi-threaded version where a timeout is not reported
 *   correctly (which could cause IPC_listenClear into a very long loop).
 *
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2009/01/06 17:42:06  reids
 * Removed a compiler warning
 *
 * Revision 2.2  2003/04/14 15:31:01  reids
 * Updated for Windows XP
 *
 * Revision 2.1  2002/01/03 20:52:14  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 *
 * $Revision: 2.6 $
 * $Date: 2011/04/21 18:17:49 $
 * $Author: reids $
 *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>

#ifdef THREADED
#include <string.h>
#include "multiThread.h"

typedef enum { Data_None=0, Data_Waiting, Data_Ready } DATA_STATUS;

static int32 pthreadHashFunc(pthread_t *i)
{
  int32 val = *i;
  return (val < 0 ? -val : val);
}

static int32 pthreadKeyEqFunc(pthread_t *a, pthread_t *b)
{
  return (*a == *b);
}

MUTEX_STATUS initMutex(MUTEX_PTR mutex)
{
  int result;

  memset(mutex, 0, sizeof(MUTEX_TYPE));

  /* Create the Mutex Attributes */
  pthread_mutexattr_init(&mutex->mutexAttributes);

  result = PTHREAD_MUTEX_SETKIND(&mutex->mutexAttributes,
#if defined(PTHREAD_MUTEX_RECURSIVE) || defined(__FreeBSD__)
				 PTHREAD_MUTEX_RECURSIVE);
#else
				 PTHREAD_MUTEX_RECURSIVE_NP);
#endif
  switch (result) {
  case 0:  /* It worked */
    pthread_mutex_init(&mutex->mutexData,
		       (result == 0 ? &mutex->mutexAttributes : NULL));
    return Mutex_Success;

  case EINVAL:
    fprintf(stderr, "[initMutex]  Programmer Error:  failed with EINVAL.");
    return Mutex_Failure;

  default:  /* What the hell? */
    fprintf(stderr, "[initMutex]  UNKNOWN Error: failed with: %d\n.", result);
    return Mutex_Failure;
  }
}

MUTEX_STATUS destroyMutex(MUTEX_PTR mutex)
{
  return Mutex_Success;
}

MUTEX_STATUS lockMutex(MUTEX_PTR mutex)
{
  int result;
  result = pthread_mutex_lock(&mutex->mutexData);
  switch (result) {
  case 0:  /* It worked */
    return Mutex_Success;

  case EINVAL:
    fprintf(stderr, "[lockMutex]  Error:  [EINVAL]  "
	    "Unable to lock improperly initialized mutex.\n");
    break;

  case EDEADLK:  /* Mutex::ERROR_CHECK only */
    fprintf(stderr, "[lockMutex]  Error:  [EDEADLK]  "
	    "Unable to lock mutex:  Mutex already locked by calling thread.\n");
    break;

  default:
    fprintf(stderr,
	    "[lockMutex]  Error:  pthread_mutex_lock(.) returned: %d.\n",
	    result);
    break;
  }

  return Mutex_Failure;
}

MUTEX_STATUS unlockMutex(MUTEX_PTR mutex)
{
  int result = pthread_mutex_unlock(&mutex->mutexData);
  switch (result) {
  case 0:  /* It worked */
    return Mutex_Success;

  case EINVAL:
    fprintf(stderr, "[unlockMutex]  Error:  [EINVAL]  "
	    "Unable to unlock improperly initialized mutex.\n");
    break;

  case EPERM:  /* Mutex::ERROR_CHECK only */
    fprintf(stderr, "[unlockMutex]  Error:  [EPERM]  "
	    "Unable to unlock Mutex:  Calling thread does not own the mutex.\n");
    break;

  default:
    fprintf(stderr,
	    "[unlockMutex]  Error:  pthread_mutex_unlock(.) returned: %d.\n",
	    result);
    break;
  }

  return Mutex_Failure;
}

MUTEX_STATUS tryLockMutex(MUTEX_PTR mutex)
{
  int result = pthread_mutex_trylock(&mutex->mutexData);
  switch (result) {
  case 0:  /* It worked */
    return Mutex_Success;

  case EINVAL:
    fprintf(stderr, "[tryLockMutex]  Error:  [EINVAL]  "
	    "Unable to lock improperly initialized mutex.\n");
    break;

  case EBUSY:  /* Already locked */
    return Mutex_Already_Locked;

  case EDEADLK:  /* Mutex::ERROR_CHECK only */
    fprintf(stderr, "[tryLockMutex]  Error:  [EDEADLK]  "
	    "Unable to lock mutex:  Mutex already locked by calling thread.\n");
    break;

  default:
    fprintf(stderr,
	    "[tryLockMutex]  Error:  pthread_mutex_trylock(.) returned: %d.\n",
	    result);
    break;
  }

  return Mutex_Failure;
}

PING_STATUS initPing(PING_THREAD_PTR ping)
{
  if (initMutex(&ping->mutex) == Mutex_Success &&
      pthread_cond_init(&ping->pingVar, NULL) == 0) {
    ping->pinged = x_ipc_hashTableCreate(11, (HASH_FN)pthreadHashFunc,
					 (EQ_HASH_FN)pthreadKeyEqFunc);
    return Ping_Success;
  } else
    return Ping_Failure;
}

static int32 setPinged (const void *key, const void *data, void *table)
{
  if ((DATA_STATUS)data != Data_Ready && *(long *)key != pthread_self()) {
    x_ipc_hashTableInsert(key, sizeof(pthread_t),
			  (void *)Data_Ready, table);
  }
  return 1;
}

PING_STATUS pingThreads(PING_THREAD_PTR ping)
{
  lockMutex(&ping->mutex);
  //fprintf(stderr, "PINGING\n");
  x_ipc_hashTableIterate(setPinged, ping->pinged, ping->pinged);
  pthread_cond_broadcast(&ping->pingVar);
  unlockMutex(&ping->mutex);

  return Ping_Success;
}

PING_STATUS waitForPing(PING_THREAD_PTR ping, struct timeval *timeout)
{
  int retcode = 0;
  struct timeval now;
  struct timespec ptimeout;

  lockMutex(&ping->mutex);
  pthread_t thread = pthread_self();
  DATA_STATUS data = (DATA_STATUS)x_ipc_hashTableFind((void *)&thread,
						      ping->pinged);
  if (data == Data_None) 
    x_ipc_hashTableInsert((void *)&thread, sizeof(pthread_t),
			  (void *)Data_Waiting, ping->pinged);

  while (data != Data_Ready && retcode != ETIMEDOUT) {
    if (timeout == NULL) {
      retcode = pthread_cond_wait(&ping->pingVar, &ping->mutex.mutexData);
      //fprintf(stderr, "PINGED (%ld)!\n", pthread_self());
    } else {
      gettimeofday(&now, NULL);
      ptimeout.tv_nsec = (now.tv_usec + timeout->tv_usec) * 1000;
      if (ptimeout.tv_nsec >= 1000000000) {
	now.tv_sec++;
	ptimeout.tv_nsec -= 1000000000;
      }
      ptimeout.tv_sec = now.tv_sec + timeout->tv_sec;
      retcode = pthread_cond_timedwait(&ping->pingVar, &ping->mutex.mutexData,
				       &ptimeout);
      //fprintf(stderr, "PINGED WAIT (%ld)!\n", pthread_self());
    }
    data = (DATA_STATUS)x_ipc_hashTableFind((void *)&thread, ping->pinged);
  }
  x_ipc_hashTableInsert((void *)&thread, sizeof(pthread_t),
			(void *)Data_Waiting, ping->pinged);
  unlockMutex(&ping->mutex);
  return (retcode == ETIMEDOUT ? Ping_Timeout : Ping_Success);
}

PING_STATUS freePing(PING_THREAD_PTR ping)
{
  x_ipc_hashTableFree(&ping->pinged, NULL, NULL);
  return Ping_Success;
}

#endif /* THREADED */

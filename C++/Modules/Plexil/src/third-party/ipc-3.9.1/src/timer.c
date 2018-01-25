/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1997 Reid Simmons.  All rights reserved.
 *
 * FILE: timer.c
 *
 * ABSTRACT: Add and remove timers that invoke handler functions at 
 *           specified intervals.  Needed by JSC Aercam project.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: timer.c,v $
 * Revision 2.9  2011/08/16 16:01:57  reids
 * Adding Python interface to IPC, plus some minor bug fixes
 *
 * Revision 2.8  2009/02/07 18:36:19  reids
 * Fixed compiler warnings
 *
 * Revision 2.7  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2002/01/03 20:52:18  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/05/30 19:37:59  reids
 * Fixed a bug in the way timers interacted with query/reply messages.
 *   In particular, timers can no longer run recursively.
 *
 * Revision 2.4  2001/03/06 00:20:17  trey
 * added IPC_addTimerGetRef() and IPC_removeTimerByRef() functions
 *
 * Revision 2.3  2001/03/01 22:22:44  reids
 * Don't warn about replacing the timer if it is already deleted!
 *
 * Revision 2.2  2000/07/03 17:03:31  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 ****************************************************************/

#include "globalM.h"
#include "ipcPriv.h"
#include "ipc.h"

typedef enum { Timer_Waiting, Timer_In_Use, Timer_Done, 
	       Timer_Deleted } TIMER_STATUS_TYPE;

typedef struct {
  TIMER_HANDLER_TYPE timerFn;
  unsigned long period; /* In msecs */
  void *clientData;
  long maxTrigger;
  unsigned long triggerTime; /* In msecs */
  TIMER_STATUS_TYPE status;
} TIMER_TYPE, *TIMER_PTR;

/****************************************************************
 *
 *   Internal Functions
 *
 ****************************************************************/

static void ipcDeleteTimer (TIMER_PTR timer)
{
  LOCK_M_MUTEX;
  x_ipc_listDeleteItem((const void *)timer, GET_M_GLOBAL(timerList));
  UNLOCK_M_MUTEX;
  x_ipcFree((char *)timer);
}

/* this version tests for the same handler */
static BOOLEAN sameTimerP (TIMER_HANDLER_TYPE handler, TIMER_PTR timer)
{
  return (timer->timerFn == handler);
}

static TIMER_PTR ipcGetTimer (TIMER_HANDLER_TYPE handler)
{
  TIMER_PTR result;

  LOCK_M_MUTEX;
  result = (TIMER_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN)sameTimerP, 
					      (const void *)handler,
					      GET_M_GLOBAL(timerList));
  UNLOCK_M_MUTEX;

  return result;
}

/* this version tests for pointer equality */
static BOOLEAN eqTimerP (TIMER_PTR timer0, TIMER_PTR timer)
{
  return (timer0 == timer);
}

static TIMER_PTR ipcGetTimerEq(TIMER_PTR timerRef)
{
  TIMER_PTR result;

  LOCK_M_MUTEX;
  result = (TIMER_PTR)x_ipc_listMemReturnItem((LIST_ITER_FN)eqTimerP, 
					      (const void *)timerRef,
					      GET_M_GLOBAL(timerList));
  UNLOCK_M_MUTEX;

  return result;
}

unsigned long ipcNextTime (void)
{
  unsigned long nextTime = WAITFOREVER;
  TIMER_PTR timer;

  LOCK_M_MUTEX;
  timer = (TIMER_PTR)x_ipc_listFirst(GET_M_GLOBAL(timerList));
  while (timer) {
    if (timer->triggerTime < nextTime && timer->status == Timer_Waiting) {
      nextTime = timer->triggerTime;
    }
    timer = (TIMER_PTR)x_ipc_listNext(GET_M_GLOBAL(timerList));
  }
  UNLOCK_M_MUTEX;
  return nextTime;
}

void ipcTriggerTimers (void)
{
  unsigned long triggerTime, now;
  TIMER_PTR timer;

  now = x_ipc_timeInMsecs();
  LOCK_M_MUTEX;
  timer = (TIMER_PTR)x_ipc_listFirst(GET_M_GLOBAL(timerList));
  while (timer) {
    triggerTime = timer->triggerTime;
    if (timer->status == Timer_Waiting && triggerTime <= now) {
      timer->status = Timer_In_Use;
      /* Update timer data *before* invoking handler, 
	 just in case triggerTimers is called recursively */
      if (timer->maxTrigger != TRIGGER_FOREVER) {
	timer->maxTrigger--;
	if (timer->maxTrigger == 0)
	  timer->status = Timer_Deleted;
      }
      timer->triggerTime = now + timer->period;
      (timer->timerFn)(timer->clientData, now, triggerTime);
      now = x_ipc_timeInMsecs();

      /* setting status to Timer_Waiting here ensures that the
	 status is only reset after the timer has really finished,
	 and not inside a recursive call. */
      if (timer->status != Timer_Deleted) {
	timer->status = Timer_Done;
      }
    }
    timer = (TIMER_PTR)x_ipc_listNext(GET_M_GLOBAL(timerList));
  }

  /* Clean up any timers that were deleted from within a handler */
  timer = (TIMER_PTR)x_ipc_listFirst(GET_M_GLOBAL(timerList));
  while (timer) {
    if (timer->status == Timer_Deleted) {
      ipcDeleteTimer(timer);
    } else if (timer->status == Timer_Done) {
      timer->status = Timer_Waiting;
    }
    timer = (TIMER_PTR)x_ipc_listNext(GET_M_GLOBAL(timerList));
  }
  UNLOCK_M_MUTEX;
}

/****************************************************************
 *
 *   Public Functions
 *
 ****************************************************************/
IPC_RETURN_TYPE IPC_addTimer(unsigned long tdelay, long count,
			     TIMER_HANDLER_TYPE handler, void *clientData)
{
  return IPC_addTimerGetRef(tdelay,count,handler,clientData,0);
}

IPC_RETURN_TYPE IPC_addTimerGetRef(unsigned long tdelay, long count,
				   TIMER_HANDLER_TYPE handler,
				   void *clientData,
				   TIMER_REF *timerRef)
{
  TIMER_PTR timer;

  if (!handler) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (tdelay == 0) {
    RETURN_ERROR(IPC_Argument_Out_Of_Range);
  } else if (count <= 0 && count != TRIGGER_FOREVER) {
    RETURN_ERROR(IPC_Argument_Out_Of_Range);
  } else {
    if (0 == timerRef) {
      /* if IPC_addTimerGetRef() is being called from IPC_addTimer(),
         we will clobber any old timers with the same handler
         for backwards compatibility */  
      timer = ipcGetTimer(handler);
      if (timer && timer->status != Timer_Deleted) {
	X_IPC_MOD_WARNING1("Replacing existing timer for handler (%#lx)\n",
			   (long)handler);
      } else {
	timer = NEW(TIMER_TYPE);
	LOCK_M_MUTEX;
	x_ipc_listInsertItemLast((const void *)timer, GET_M_GLOBAL(timerList));
	UNLOCK_M_MUTEX;
      }
    } else {
      /* if IPC_addTimerGetRef() is being called directly with a non-zero
	 timerRef argument, we don't clobber */
      timer = NEW(TIMER_TYPE);
      LOCK_M_MUTEX;
      x_ipc_listInsertItemLast((const void *)timer, GET_M_GLOBAL(timerList));
      UNLOCK_M_MUTEX;
    }

    timer->timerFn = handler;
    timer->period = tdelay;
    timer->clientData = clientData;
    timer->maxTrigger = count;
    timer->triggerTime = x_ipc_timeInMsecs() + tdelay;
    timer->status = Timer_Waiting;

    if (0 != timerRef) *timerRef = (TIMER_REF) timer;

    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_addOneShotTimer(long tdelay, TIMER_HANDLER_TYPE handler,
				    void *clientData)
{
  return IPC_addTimer(tdelay, 1, handler, clientData);
}

IPC_RETURN_TYPE IPC_addPeriodicTimer(long tdelay, TIMER_HANDLER_TYPE handler,
				     void *clientData)
{
  return IPC_addTimer(tdelay, TRIGGER_FOREVER, handler, clientData);
}

IPC_RETURN_TYPE IPC_removeTimerByRef(TIMER_REF timerRef) {
  TIMER_PTR timer;

  if (!timerRef) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    timer = ipcGetTimerEq((TIMER_PTR) timerRef);
    if (!timer) {
      X_IPC_MOD_WARNING1("Timer with ref %#x does not exist\n",
			 timerRef);
    } else {
      if (timer->status == Timer_In_Use) {
	/* Need to delay actually deleting the timer in case this function
	   is called from within "ipcTriggerTimers" */
	timer->status = Timer_Deleted;
      } else {
	ipcDeleteTimer(timer);
      }
    }
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_removeTimer(TIMER_HANDLER_TYPE handler)
{
  TIMER_PTR timer;

  if (!handler) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    timer = ipcGetTimer(handler);
    if (!timer) {
      X_IPC_MOD_WARNING1("Timer for handler (%#x) does not exist\n",handler);
    } else if (timer->status == Timer_In_Use) {
      /* Need to delay actually deleting the timer in case this function
	 is called from within "ipcTriggerTimers" */
      timer->status = Timer_Deleted;
    } else {
      ipcDeleteTimer(timer);
    }
    return IPC_OK;
  }
}

// Return number of times timer can still be triggered
// Needed by some of the FFI's
unsigned int maxTriggers (TIMER_REF timerRef)
{
  TIMER_PTR timer = ipcGetTimerEq((TIMER_PTR) timerRef);
  return (!timer ? 0 : timer->maxTrigger);
}

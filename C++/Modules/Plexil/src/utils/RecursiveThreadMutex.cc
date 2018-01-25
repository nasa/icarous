/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "RecursiveThreadMutex.hh"

#include "Debug.hh"
#include "Error.hh"
#include <errno.h>

namespace PLEXIL
{
  RecursiveThreadMutex::RecursiveThreadMutex()
    : m_lockingThread((pthread_t) 0),
      m_lockCount(0)
  {
    pthread_mutexattr_t   mta;

    int rv = pthread_mutexattr_init(&mta);
    assertTrue_2(rv != ENOMEM, "No memory for mutex attribute init.");
    assertTrue_2(rv == 0, "Error initializing mutex attribute structure.");

#if !defined(__VXWORKS__) /* platform lacks function */
    rv = pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_ERRORCHECK);
    assertTrue_2(rv != EINVAL, "PTHREAD_MUTEX_ERRORCHECK is an invalid value");
    assertTrue_2(0 == rv, "Could not set the mutex attribute.");
#endif

    rv = pthread_mutex_init(&m_mutex, &mta);
    assertTrue_2(0 == rv, "Could not initialize the mutex.");
  }

  RecursiveThreadMutex::~RecursiveThreadMutex()
  {
    //release all locks by current thread (protects against assertion failures during exit())
    while (isLockedByCurrentThread())
      unlock();
    int rv = pthread_mutex_destroy(&m_mutex);
    assertTrue_2(rv != EBUSY, "Attempted to destroy mutex while locked or referenced.");
    assertTrue_2(0 == rv, "Could not destroy the mutex.");
  }

  void RecursiveThreadMutex::lock() 
  {
    if (isLockedByCurrentThread()) 
      {
        debugMsg("RecursiveThreadMutex:lock",
                 " Re-locking mutex " << (void *) this << " from thread " << pthread_self() << " with count of " << m_lockCount + 1);
        ++m_lockCount;
        return;
      }
    debugMsg("RecursiveThreadMutex:lock",
             " mutex " << (void *) this << " from thread " << pthread_self());
    int rv = pthread_mutex_lock(&m_mutex);
    assertTrue_2(rv != EINVAL, "The mutex was created with the protocol attribute having the value PTHREAD_PRIO_PROTECT and the calling thread's priority is higher than the mutex's current priority ceiling.");
    assertTrue_2(rv != EAGAIN, "The mutex could not be acquired because the maximum number of recursive locks for mutex has been exceeded.");
    assertTrue_2(rv == 0, "Could not lock the mutex.");
    check_error_2(m_lockCount == 0, "Got a lock without a lock count of 0.");
    m_lockingThread = pthread_self();
    ++m_lockCount;
    debugMsg("RecursiveThreadMutex:lock",
             " thread " << pthread_self() << " acquired mutex " << (void *) this);
  }

  void RecursiveThreadMutex::unlock()
  {
    check_error_2(isLockedByCurrentThread(),
                 "Tried to unlock without owning the mutex.");
    check_error_2(m_lockCount > 0, "Tried to unlock more than locked.");
    debugMsg("RecursiveThreadMutex:unlock",
             " mutex " << (void *) this << " from thread " << pthread_self() << " with count of " << m_lockCount);
    --m_lockCount;
    if (m_lockCount == 0)
      {
        m_lockingThread = (pthread_t) 0;
        int rv = pthread_mutex_unlock(&m_mutex);
        assertTrue_2(0 == rv, "Could not unlock the mutex.");
        debugMsg("RecursiveThreadMutex:unlock",
                 " mutex " << (void *) this << " released by thread " << pthread_self());
      }
  }

}

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

#include "ThreadMutex.hh"
#include "Error.hh"
#include <cerrno>

namespace PLEXIL
{
  ThreadMutex::ThreadMutex()
  {
    pthread_mutexattr_t m_mta;
    int rv = pthread_mutexattr_init(&m_mta);
    if (rv != 0)
      {
        assertTrue_2(rv != ENOMEM, "No memory for mutex attribute init.");
        assertTrueMsg(ALWAYS_FAIL, "pthread_mutexattr_init failed, errno = " << rv);
      }

#if !defined(__VXWORKS__) /* platform lacks function */
    rv = pthread_mutexattr_settype(&m_mta, PTHREAD_MUTEX_NORMAL);
    if (rv != 0)
      {
        assertTrue_2(rv != EINVAL, "PTHREAD_MUTEX_NORMAL is an invalid value");
        assertTrueMsg(ALWAYS_FAIL, "pthread_mutexattr_settype failed, errno = " << rv);
      }
#endif

    // this may not be implemented, skip it if not
    // Android claims it is, but lies
#if defined(_POSIX_THREAD_PRIO_INHERIT) && _POSIX_THREAD_PRIO_INHERIT >= 0 && !defined(PLEXIL_ANDROID)
    rv = pthread_mutexattr_setprotocol(&m_mta, PTHREAD_PRIO_INHERIT);
    if (rv != 0)
      {
        assertTrue_2(rv != ENOTSUP, "PTHREAD_PRIO_INHERIT is not supported");
        assertTrue_2(rv != EINVAL, "Invalid value to pthread_mutexattr_setprotocol");
        assertTrueMsg(ALWAYS_FAIL, "pthread_mutexattr_setprotocol failed, errno = " << rv);
      }
#endif

    rv = pthread_mutex_init(&m_mutex, &m_mta);
    if (rv != 0)
      {
        assertTrue_2(rv != EINVAL, "Mutex pointer or attribute pointer invalid.");
        assertTrue_2(rv != EPERM, "Insufficient permissions for mutex initialization.");
        assertTrue_2(rv != EBUSY, "Attempt to initialize mutex which was already initialized.");
        assertTrue_2(rv != ENOMEM, "No memory for mutex initialization.");
        assertTrue_2(rv != EAGAIN, "Insufficient system resources for mutex initialization.");
        assertTrueMsg(ALWAYS_FAIL, "pthread_mutex_init failed, errno = " << rv);
      }
    
    // Clean up
    rv = pthread_mutexattr_destroy(&m_mta);
    assertTrueMsg(rv == 0, "pthread_mutexattr_destroy failed, errno = " << rv);
  }

  ThreadMutex::~ThreadMutex()
  {
    int rv = pthread_mutex_destroy(&m_mutex);
    if (rv == 0)
      return;
    assertTrue_2(rv != EBUSY, "Attempted to destroy mutex while locked or referenced.");
    assertTrueMsg(ALWAYS_FAIL, "pthread_mutex_destroy failed, errno = " << rv);
  }

  void ThreadMutex::lock()
  {
    int rv = pthread_mutex_lock(&m_mutex);
    if (rv == 0)
      return;

    assertTrue_2(rv != EDEADLK, "Deadlock detected, or attempt to lock mutex that is already locked by this thread.");
    assertTrue_2(rv != EINVAL, "Invalid mutex or insufficient mutex priority ceiling.");
    assertTrueMsg(ALWAYS_FAIL, "pthread_mutex_lock failed, errno = " << rv);
  }

  bool ThreadMutex::trylock()
  {
    int rv = pthread_mutex_trylock(&m_mutex);
    if (rv == 0)
      return true;
    if (rv == EBUSY)
      // mutex already locked
      return false;
    assertTrue_2(rv != EINVAL, "Invalid mutex or insufficient mutex priority ceiling.");
    assertTrueMsg(ALWAYS_FAIL,  "pthread_mutex_trylock failed, errno = " << rv);
    return false; // to make compiler happy
  }

  void ThreadMutex::unlock()
  {
    int rv = pthread_mutex_unlock(&m_mutex);
    if (rv == 0)
      return;
    assertTrue_2(rv != EPERM, "Attempt to unlock mutex that is locked by another thread.");
    assertTrueMsg(ALWAYS_FAIL, "pthread_mutex_unlock failed, errno = " << rv);
  }

  ThreadMutexGuard::ThreadMutexGuard(ThreadMutex& mutex)
    : m_mutex(mutex)
  {
    m_mutex.lock();
  }

  ThreadMutexGuard::~ThreadMutexGuard()
  {
    m_mutex.unlock();
  }

}

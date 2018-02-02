/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
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

// -*- Mode: C++ -*-
//      CVS: $Id: RecursiveThreadMutexs.hh,v 1.2 2006/10/27 22:21:22 miatauro Exp $
//      Tag: $name$
//     Info: 

#ifndef RECURSIVE_THREAD_MUTEX_HEADER_
#define RECURSIVE_THREAD_MUTEX_HEADER_

#include <pthread.h>

namespace PLEXIL
{
  /**
   * @brief Implements a portable mutex which supports multiple symmetrical lock/unlock calls by the same thread.
   */

  class RecursiveThreadMutex
  {
  public:
    /**
     * @brief Default constructor.
     */
    RecursiveThreadMutex();

    /**
     * @brief Destructor.
     */
    ~RecursiveThreadMutex();

    /**
     * @brief Locks the mutex.  
     * If the lock is available, sets lock count to 1.
     * If already locked by this thread, increments the lock count.
     * If locked by another thread, waits until that thread has unlocked.
     */
    void lock();

    /**
     * @brief Unlocks the mutex.
     * Actually decrements the lock count.  When the count reaches 0, the mutex is unlocked.
     * If locked by another thread, signals an error.
     */
    void unlock();

    /**
     * @brief Returns true if the mutex is locked by any thread.
     * @note Slight chance of race condition between check of lock status and locking thread.
     */
    inline bool isLocked()
    {
      return !pthread_equal(m_lockingThread, (pthread_t) 0);
    }

    /**
     * @brief Returns true if the mutex is locked by the current thread.
     */
    inline bool isLockedByCurrentThread()
    {
      return pthread_equal(m_lockingThread, pthread_self());
    }
    
  private:

    // deliberately unimplemented
    RecursiveThreadMutex( const RecursiveThreadMutex& );
    const RecursiveThreadMutex& operator=( const RecursiveThreadMutex& );

    pthread_mutex_t m_mutex;
    pthread_t m_lockingThread;
    int m_lockCount;
  };

  class RTMutexGuard
  {
  public:
    RTMutexGuard( RecursiveThreadMutex& mutex ):
      m_mutex( mutex )
    {
      m_mutex.lock();
    }

    ~RTMutexGuard( )
    {
      m_mutex.unlock();
    }
  private:
    RTMutexGuard(const RTMutexGuard &);
    const RTMutexGuard& operator=(const RTMutexGuard &);

    RecursiveThreadMutex& m_mutex;
  };
}

#endif //RECURSIVE_THREAD_MUTEX_HEADER_

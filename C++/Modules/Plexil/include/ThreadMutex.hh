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

#ifndef THREAD_MUTEX_HEADER_
#define THREAD_MUTEX_HEADER_

#include <pthread.h>

namespace PLEXIL
{
  /**
   * @brief Implements a simple mutex based on POSIX pthread_mutex_t with modest convenience features.
   * @note Users of this class should not depend on ThreadMutex detecting deadlocks,
   * multiple calls to lock() from the same thread, or unlock-while-unlocked.
   */

  class ThreadMutex
  {
  public:
    /**
     * @brief Default constructor.
     */
    ThreadMutex();

    /**
     * @brief Destructor.
     */
    ~ThreadMutex();

    /**
     * @brief Locks the mutex. If locked by another thread, waits until that thread has unlocked.
     */
    void lock();

    /**
     * @brief Tries to lock the mutex.
     * @return True if successful, false if the mutex is already held by any thread.
     */
    bool trylock();

    /**
     * @brief Unlocks the mutex.
     */
    void unlock();
    
  private:

    // deliberately unimplemented
    ThreadMutex( const ThreadMutex& );
    const ThreadMutex& operator=( const ThreadMutex& );

    pthread_mutex_t m_mutex;
  };

  /**
   * @brief Implements a guard using ThreadMutex.
   */

  class ThreadMutexGuard
  {
  public:
    ThreadMutexGuard(ThreadMutex& mutex);
    ~ThreadMutexGuard();

  private:

    // deliberately unimplemented
    ThreadMutexGuard();
    ThreadMutexGuard(const ThreadMutexGuard &);
    const ThreadMutexGuard& operator=(const ThreadMutexGuard &);

    ThreadMutex& m_mutex;
  };
}

#endif //THREAD_MUTEX_HEADER_

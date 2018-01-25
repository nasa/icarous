/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

#ifndef PLEXIL_THREAD_SEMAPHORE_HH
#define PLEXIL_THREAD_SEMAPHORE_HH

//
// Does this platform support POSIX thread semaphores?
//

#include <unistd.h>

#if !defined(_POSIX_SEMAPHORES) || _POSIX_SEMAPHORES == -1
// OS doesn't advertise it provides POSIX semaphores, or they're broken somehow
#ifdef __MACH__
#define PLEXIL_USE_MACH_SEMAPHORES // e.g. Mac OS X
#elif defined(__VXWORKS__)
#define PLEXIL_USE_POSIX_SEMAPHORES
#endif // __VXWORKS __

#else 
// OS provides POSIX semaphores
#define PLEXIL_USE_POSIX_SEMAPHORES
#endif // _POSIX_SEMAPHORES == -1

//
// Include the native semaphore headers
//

#ifdef PLEXIL_USE_POSIX_SEMAPHORES
#include <semaphore.h>
#include <cerrno>

//* @brief Error status showing that the wait() call was interrupted.
#define PLEXIL_SEMAPHORE_STATUS_INTERRUPTED EINTR
#endif // PLEXIL_USE_POSIX_SEMAPHORES

#ifdef PLEXIL_USE_MACH_SEMAPHORES
#include <mach/semaphore.h>
#include <mach/task.h> // for semaphore_create(), semaphore_destroy()

//* @brief Error status showing that the wait() call was interrupted.
#define PLEXIL_SEMAPHORE_STATUS_INTERRUPTED KERN_ABORTED
#endif // PLEXIL_USE_MACH_SEMAPHORES

namespace PLEXIL 
{
  /**
     @class ThreadSemaphore
     @brief Provides a cross-platform API to binary thread semaphores.
     @author Chuck Fry
     @date 11 July 2008
   */

  class ThreadSemaphore
  {
  public:
    /**
       @brief Constructor.
    */
    ThreadSemaphore();

    /**
       @brief Destructor.
    */
    ~ThreadSemaphore();

    /**
       @brief Causes the thread to block until post() is called on the semaphore.
       @return 0 if successful, error number otherwise.
       @note Error number is platform dependent.
	   @note A status of PLEXIL_SEMAPHORE_STATUS_INTERRUPTED means the system call was interrupted,
             e.g. by a signal or pthread_cancel(). This may not be an error depending on the situation.
			 The caller should always check the return value and deal with it appropriately!
    */
    int wait();

    /**
       @brief Causes the semaphore to unblock a thread currently waiting on it.
       @return 0 if successful, error number otherwise.
       @note Error number is platform dependent.
    */
    int post();

  private:

#ifdef PLEXIL_USE_POSIX_SEMAPHORES
    sem_t m_posix_sem;
#endif // PLEXIL_USE_POSIX_SEMAPHORES

#ifdef PLEXIL_USE_MACH_SEMAPHORES
    semaphore_t m_mach_sem;
    task_t m_mach_owning_task;
#endif // PLEXIL_USE_MACH_SEMAPHORES

  };

} // namespace PLEXIL

#endif // PLEXIL_THREAD_SEMAPHORE_HH

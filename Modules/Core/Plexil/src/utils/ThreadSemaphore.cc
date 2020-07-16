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

#include "ThreadSemaphore.hh"
#include "Error.hh"

#ifdef PLEXIL_SEMAPHORE_DEBUG
// debug case
#include "Debug.hh"
#define myDebugMsg(label, msg) debugMsg(label, msg)
#else
// non-debug case
#define myDebugMsg(label, msg)
#endif


#ifdef PLEXIL_USE_MACH_SEMAPHORES
#include <mach/mach_init.h> // for mach_task_self()
#endif

namespace PLEXIL
{

#ifdef PLEXIL_USE_POSIX_SEMAPHORES

  //
  // POSIX implementation
  //

  ThreadSemaphore::ThreadSemaphore()
  {
    int status = sem_init(&m_posix_sem, 0, 0);
    assertTrueMsg(status != -1,
		  "ThreadSemaphore (POSIX) constructor: sem_init failed, errno = "
		  << errno);
	myDebugMsg("ThreadSemaphore:ThreadSemaphore", " @ " << this);
  }

  ThreadSemaphore::~ThreadSemaphore()
  {
    int status = sem_destroy(&m_posix_sem);
    assertTrueMsg(status != -1,
		  "ThreadSemaphore (POSIX) destructor: sem_destroy failed, errno = "
		  << errno);
	myDebugMsg("ThreadSemaphore:~ThreadSemaphore", " @ " << this);
  }

  int ThreadSemaphore::wait()
  {
	myDebugMsg("ThreadSemaphore:wait", " on " << this);
    int status;

    // If the wait fails due to a signal, ignore the error (EINTR).
    // If the error is not EINTR, stop the thread.
    while (((status = sem_wait(&m_posix_sem)) == -1) && (errno == EINTR))
      continue;
    
    if (status == -1) {
	  myDebugMsg("ThreadSemaphore:wait", " failed on " << this << ", errno = " << errno);
      return errno;
	}
	myDebugMsg("ThreadSemaphore:wait", " complete on " << this);
    return 0;
  }

  int ThreadSemaphore::post()
  {
    int status = sem_post(&m_posix_sem);
    if (status == -1)
      return errno;
    else return 0;
  }

#endif // PLEXIL_USE_POSIX_SEMAPHORES

#ifdef PLEXIL_USE_MACH_SEMAPHORES
  //
  // MACH implementation
  //

  ThreadSemaphore::ThreadSemaphore()
	: m_mach_owning_task(mach_task_self())
  {
    kern_return_t status = 
      semaphore_create(m_mach_owning_task,
		       &m_mach_sem,
		       SYNC_POLICY_FIFO,
		       0);
    assertTrueMsg(status == KERN_SUCCESS,
		  "ThreadSemaphore (MACH) constructor: semaphore_create failed, status = "
		  << status);
	myDebugMsg("ThreadSemaphore:ThreadSemaphore", " @ " << this << ", Mach semaphore " << m_mach_sem);
  }

  ThreadSemaphore::~ThreadSemaphore()
  {
	myDebugMsg("ThreadSemaphore:~ThreadSemaphore", " @ " << this << ", Mach semaphore " << m_mach_sem);
    kern_return_t status = 
      semaphore_destroy(m_mach_owning_task,
			m_mach_sem);
    assertTrueMsg(status == KERN_SUCCESS,
		  "ThreadSemaphore (MACH) destructor: semaphore_destroy failed, status = "
		  << status);
  }

  // *** N.B. There's a problem here relative to the POSIX version.
  // POSIX uniquely identifies when the sem_wait() call is interrupted by a signal, 
  // and isn't documented to unblock when (e.g.) pthread_cancel() is called.
  // Mach has a catch-all KERN_ABORTED return value for both cases.
  int ThreadSemaphore::wait()
  {
	myDebugMsg("ThreadSemaphore:wait", " on " << this << ", Mach semaphore " << m_mach_sem);
    kern_return_t status = semaphore_wait(m_mach_sem);
	myDebugMsg("ThreadSemaphore:wait", " complete on " << this << ", Mach semaphore " << m_mach_sem << ", status = " << status);
    return status;
  }

  int ThreadSemaphore::post()
  {
	myDebugMsg("ThreadSemaphore:post", " to " << this << ", Mach semaphore " << m_mach_sem);
    kern_return_t status = semaphore_signal(m_mach_sem);
    return status;
  }

#endif // PLEXIL_USE_MACH_SEMAPHORES

} // namespace PLEXIL

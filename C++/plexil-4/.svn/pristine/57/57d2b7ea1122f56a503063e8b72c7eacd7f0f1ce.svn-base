/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

//
// *** Ignore this file on systems that do not implement POSIX timers
//

#include <unistd.h>
// sigh, Android only defines _POSIX_TIMERS as 1
#if defined(_POSIX_TIMERS) && ((_POSIX_TIMERS - 200112L) >= 0L || defined(PLEXIL_ANDROID))

#include "PosixTimeAdapter.hh"
#include "AdapterExecInterface.hh"
#include "Debug.hh"
#include "InterfaceError.hh"
#include "TimeAdapter.hh"
#include "timespec-utils.hh"

#include <cerrno>
#include <iomanip>

namespace PLEXIL
{
  /**
   * @brief Constructor.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   */
  PosixTimeAdapter::PosixTimeAdapter(AdapterExecInterface& execInterface)
    : TimeAdapterImpl(execInterface)
  {
  }

  /**
   * @brief Constructor from configuration XML.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   * @param xml A const reference to the XML element describing this adapter
   * @note The instance maintains a shared pointer to the XML.
   */
  PosixTimeAdapter::PosixTimeAdapter(AdapterExecInterface& execInterface, 
                                     pugi::xml_node const xml)
    : TimeAdapterImpl(execInterface, xml)
  {
  }

  /**
   * @brief Destructor.
   */
  PosixTimeAdapter::~PosixTimeAdapter()
  {
  }

  /**
   * @brief Stops the adapter.  
   * @return true if successful, false otherwise.
   */
  bool PosixTimeAdapter::stop()
  {
    return TimeAdapterImpl::stop();
  }

  /**
   * @brief Get the current time from the operating system.
   * @return A double representing the current time.
   */
  double PosixTimeAdapter::getCurrentTime()
    throw (InterfaceError)
  {
    timespec ts;
    checkInterfaceError(!clock_gettime(CLOCK_REALTIME, &ts),
                        "getCurrentTime: clock_gettime() failed, errno = " << errno);
    double tym = timespecToDouble(ts);
    debugMsg("TimeAdapter:getCurrentTime", " returning " << std::setprecision(15) << tym);
    return tym;
  }

  /**
   * @brief Initialize signal handling for the process.
   * @return True if successful, false otherwise.
   */
  bool PosixTimeAdapter::configureSignalHandling()
  {
    // Mask SIGUSR1 at the process level
    sigset_t mask;
    if (sigemptyset(&mask)) {
      warn("PosixTimeAdapter: sigemptyset failed!");
      return false;
    }
    
    if (sigaddset(&mask, SIGUSR1)) {
      warn("PosixTimeAdapter: sigaddset failed!");
      return false;
    }
    
    if (sigprocmask(SIG_BLOCK, &mask, NULL)) {
      warn ("PosixTimeAdapter: sigprocmask failed, errno = " << errno);
      return false;
    }
    return true;
  }

  /**
   * @brief Construct and initialize the timer as required.
   * @return True if successful, false otherwise.
   */
  bool PosixTimeAdapter::initializeTimer()
  {
    // Initialize sigevent
    m_sigevent.sigev_notify = SIGEV_SIGNAL;
    m_sigevent.sigev_signo = SIGUSR1; // was SIGALRM
    m_sigevent.sigev_value.sival_int = 0;
    m_sigevent.sigev_notify_function = NULL;
    m_sigevent.sigev_notify_attributes = NULL;

    // Create a timer
    if (timer_create(CLOCK_REALTIME,
                     &m_sigevent,
                     &m_timer)) {
      warn("PosixTimeAdapter: timer_create failed, errno = " << errno);
      return false;
    }
    return true;
  }

  /**
   * @brief Set the timer.
   * @param date The Unix-epoch wakeup time, as a double.
   * @return True if the timer was set, false if clock time had already passed the wakeup time.
   */
  bool PosixTimeAdapter::setTimer(double date)
    throw (InterfaceError)
  {
    // Get the current time
    timespec now;
    if (0 != clock_gettime(CLOCK_REALTIME, &now)) {
      warn("PosixTimeAdapter: clock_gettime() failed, errno = " << errno);
      return false;
    }

    // Set up a timer to go off at the high time
    itimerspec tymrSpec = {{0, 0}, {0, 0}};
    tymrSpec.it_value = doubleToTimespec(date) - now;
    if (tymrSpec.it_value.tv_nsec < 0 || tymrSpec.it_value.tv_sec < 0) {
      // Already past the scheduled time
      debugMsg("TimeAdapter:setTimer",
               " new value " << std::setprecision(15) << date << " is in past, waking up Exec");
      return false;
    }

    tymrSpec.it_interval.tv_sec = tymrSpec.it_interval.tv_nsec = 0; // no repeats
    checkInterfaceError(0 == timer_settime(m_timer,
                                           0, // flags: ~TIMER_ABSTIME
                                           &tymrSpec,
                                           NULL),
                        "TimeAdapter::setTimer: timer_settime failed, errno = " << errno);
    debugMsg("TimeAdapter:setTimer",
             " timer set for " << std::setprecision(15) << date
             << ", tv_nsec = " << tymrSpec.it_value.tv_nsec);
    return true;
  }

  /**
   * @brief Stop the timer.
   */
  bool PosixTimeAdapter::stopTimer()
  {
    static itimerspec sl_tymrDisable = {{0, 0}, {0, 0}};
    int status = timer_settime(m_timer,
                               0,
                               &sl_tymrDisable,
                               NULL);
    if (status) {
      warn("PosixTimeAdapter: timer_settime failed, errno = " << errno);
    }
    return status == 0;
  }

  /**
   * @brief Shut down and delete the timer as required.
   * @return True if successful, false otherwise.
   */
  bool PosixTimeAdapter::deleteTimer()
  {
    int status = timer_delete(m_timer);
    if (status) {
      warn("PosixTimeAdapter: timer_delete failed, errno = " << errno);
    }
    return status == 0;
  }

  /**
   * @brief Initialize the wait thread signal mask.
   * @return True if successful, false otherwise.
   */
  bool PosixTimeAdapter::configureWaitThreadSigmask(sigset_t* mask)
  {
    if (sigemptyset(mask)) {
      warn("PosixTimeAdapter: sigemptyset failed!");
      return false;
    }

    int errnum = sigaddset(mask, SIGALRM);
    errnum = errnum | sigaddset(mask, SIGINT);
    errnum = errnum | sigaddset(mask, SIGHUP);
    errnum = errnum | sigaddset(mask, SIGQUIT);
    errnum = errnum | sigaddset(mask, SIGTERM);
    errnum = errnum | sigaddset(mask, SIGUSR2);
    if (errnum) {
      warn("PosixTimeAdapter: sigaddset failed!");
    }
    return errnum == 0;
  }

  /**
   * @brief Initialize the sigwait mask.
   * @param Pointer to the mask.
   * @return True if successful, false otherwise.
   */
  bool PosixTimeAdapter::initializeSigwaitMask(sigset_t* mask)
  {
    // listen only for SIGUSR1
    if (sigemptyset(mask)) {
      warn("PosixTimeAdapter: sigemptyset failed!");
      return false;
    }
    if (sigaddset(mask, SIGUSR1)) {
      warn("PosixTimeAdapter: sigaddset failed!");
      return false;
    }
    return true;
  }

}

#endif // defined(_POSIX_TIMERS) && (_POSIX_TIMERS - 200112L) >= 0L || defined(PLEXIL_ANDROID))

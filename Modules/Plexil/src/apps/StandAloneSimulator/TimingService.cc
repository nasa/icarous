/* Copyright (c) 2006-2009, Universities Space Research Association (USRA).
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

#include "TimingService.hh"
#include "timeval-utils.hh"

#include "Debug.hh"
#include "Error.hh"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>

TimingService::TimingService() 
  : m_nBlockedSignals(0)
{
  // clear out signal handler storage
  // (not strictly necessary, but better safe than sorry, and it's cheap)
  for (size_t i = 0; i <= TIMING_SERVICE_MAX_N_SIGNALS; i++) {
	m_blockedSignals[i] = 0;
  }
}

TimingService::~TimingService()
{
  stopTimer();
  if (m_nBlockedSignals != 0)
	restoreSignalHandling();
}

/**
 * @brief Dummy signal handler function for signals we process.
 * @note This should never be called!
 */
void dummySignalHandler(int /* signo */) {}

/**
 * @brief Set the process's signal mask and signal actions for timer use.
 * @param signalsToIgnore Zero-terminated array of signals to be handled by wait().
 * @return The status of the sigprocmask call; 0 = success.
 * @note TimingService clients MUST call this or defaultInitializeSignalHandling() prior to arming the timer!
 * @note SIGALRM will never be returned from a wait() call, as TimingService uses it internally.
 */
int TimingService::initializeSignalHandling(const int signumsToIgnore[])
{
  //
  // Generate the mask
  // N.B. the same mask is used in the sigwait() call.
  //
  int errnum = sigemptyset(&m_sigset);
  if (errnum != 0) {
	debugMsg("TimingService:initializeSignalHandling", " sigemptyset returned " << errnum);
	return errnum;
  }
  for (m_nBlockedSignals = 0;
	   m_nBlockedSignals < TIMING_SERVICE_MAX_N_SIGNALS && signumsToIgnore[m_nBlockedSignals] != 0;
	   m_nBlockedSignals++) {
	int sig = signumsToIgnore[m_nBlockedSignals];
	m_blockedSignals[m_nBlockedSignals] = sig; // save to restore it later
	errnum = sigaddset(&m_sigset, sig);
	if (errnum != 0) {
	  debugMsg("TimingService:initializeSignalHandling", " sigaddset returned " << errnum);
	  return errnum;
	}
  }
  // add SIGALRM to mask
  m_blockedSignals[m_nBlockedSignals++] = SIGALRM;
  errnum = sigaddset(&m_sigset, SIGALRM);
  if (errnum != 0) {
	debugMsg("TimingService:initializeSignalHandling", " sigaddset returned " << errnum);
	return errnum;
  }

  // Set the mask
  errnum = sigprocmask(SIG_BLOCK, &m_sigset, &m_restoreSigset);
  if (errnum != 0) {
	debugMsg("TimingService:initializeSignalHandling", " sigprocmask returned " << errnum);
	return errnum;
  }

  //
  // Install signal handlers
  //
  struct sigaction sa;
  sigemptyset(&sa.sa_mask); // *** is this enough?? ***
  sa.sa_flags = 0;
  sa.sa_handler = dummySignalHandler;

  for (size_t i = 0; i < m_nBlockedSignals; i++) {
	errnum = sigaction(m_blockedSignals[i], &sa, &(m_restoreHandlers[i]));
	if (errnum != 0) {
	  debugMsg("TimingService:initializeSignalHandling", " sigaction returned " << errnum);
	  return errnum;
	}
  }
	   
  debugMsg("TimingService:initializeSignalHandling", " complete");
  return 0;
}

/**
 * @brief Set the process's signal mask to a convenient default for timer use.
 * @return True if successful, false otherwise.
 * @note TimingService clients MUST call this or initializeSignalHandling() prior to arming the timer!
 * @note Calls initializeSignalHandling() internally.
 */
bool TimingService::defaultInitializeSignalHandling()
{
  static int defaultSignals[TIMING_SERVICE_MAX_N_SIGNALS] =
	{SIGINT, SIGHUP, SIGQUIT, SIGTERM, 0, 0, 0, 0};
  int errnum = initializeSignalHandling(defaultSignals);
  condDebugMsg(errnum != 0,
			   "TimingService:defaultInitializeSignalHandling",
			   " initializeSignalHandling failed, result = " << errnum);
  return errnum == 0;
}

/**
 * @brief Restore the process's original signal mask to its state at the last initializeSignalHandling() call.
 * @return True if successful, false otherwise.
 */
bool TimingService::restoreSignalHandling()
{
  //
  // Restore old mask
  //
  int errnum = sigprocmask(SIG_SETMASK, &m_restoreSigset, NULL);
  assertTrueMsg(errnum == 0, 
				"TimingService::restoreSignalHandling: Fatal error: sigprocmask returned " << errnum);

  //
  // Restore old signal handlers
  //
  for (size_t i = 0; i < m_nBlockedSignals; i++) {
	errnum = sigaction(m_blockedSignals[i], &m_restoreHandlers[i], NULL);
	if (errnum != 0) {
	  debugMsg("TimingService:restoreSignalHandling", " sigaction returned " << errnum);
	  return errnum;
	}
  }

  // flag as complete
  m_nBlockedSignals = 0;

  debugMsg("TimingService:restoreSignalHandling", " complete");
  return 0;
}

bool TimingService::setTimer(const timeval& time)
{
  assertTrueMsg(m_nBlockedSignals != 0,
				"TimingService::setTimer: Fatal error: signal handling has not been initialized");

  timeval now;
  gettimeofday(&now, NULL);
  itimerval myTimer;
  myTimer.it_interval.tv_sec = myTimer.it_interval.tv_usec = 0;
  myTimer.it_value = time - now;
      
  if (myTimer.it_value.tv_sec < 0 || myTimer.it_value.tv_usec < 0) {
	debugMsg("TimingService:setTimer",
			 " Not setting interval timer, requested time is in the past");
	return false;
  }
      
  int status = setitimer(ITIMER_REAL, &myTimer, NULL);
  assertTrueMsg(status == 0,
				"TimingService::setTimer: Fatal error: setitimer failed, errno = " << errno);
  debugMsg("TimingService:setTimer",
		   " Set interval timer for "
		   << std::setiosflags(std::ios_base::fixed) 
		   << timevalToDouble(myTimer.it_value));
  return true;
}

/**
 * @brief Get the (approximate) absolute time of the next scheduled wakeup.
 * @param result A reference to the variable where the interval should be stored.
 */
void TimingService::getTimer(timeval& result) 
{
  itimerval itime;
  int status = getitimer(ITIMER_REAL, &itime);
  assertTrueMsg(status == 0, 
				"TimingService::getTimer: Fatal error: getitimer failed, status = " << status);
  if (itime.it_value.tv_sec == 0 && itime.it_value.tv_usec == 0) {
	debugMsg("TimingService:getTimer", " timer is not set");
	result.tv_sec = result.tv_usec = 0;
	return;
  }

  // compute result based on current time of day
  timeval now;
  status = gettimeofday(&now, NULL);
  assertTrueMsg(status == 0, 
				"TimingService::getTimer: Fatal error: gettimeofday failed, status = " << status);
  result = now + itime.it_value;
  debugMsg("TimingService:getTimer",
		   " timer set for "
		   << std::setiosflags(std::ios_base::fixed) 
		   << timevalToDouble(result));
}

void TimingService::stopTimer()
{
  debugMsg("TimingService:stopTimer", " disabling timer");
  itimerval myTimer;
  myTimer.it_interval.tv_sec = myTimer.it_interval.tv_usec = 0;
  myTimer.it_value.tv_sec = myTimer.it_value.tv_usec = 0;
  int status = setitimer(ITIMER_REAL, &myTimer, NULL);
  assertTrueMsg(status == 0,
				"TimingService::stopTimer: Fatal error: setitimer failed, errno = " << errno);
}

int TimingService::wait()
{
  assertTrueMsg(m_nBlockedSignals != 0,
				"TimingService::wait: Fatal error: signal handling has not been initialized");

  debugMsg("TimingService:wait", " entered");

  int theSignal;
  int errnum = sigwait(&m_sigset, &theSignal);
  // Check status
  assertTrueMsg(errnum == 0, 
				"TimingService::wait: Fatal error: sigwait returned non-zero status " << errnum);

  // Check the signal number
  if (theSignal == SIGALRM) {
	debugMsg("TimingService:wait", " received timer wakeup");
	return 0;
  }

  debugMsg("TimingService:wait", " received non-timer signal " << theSignal);
  return theSignal;
}

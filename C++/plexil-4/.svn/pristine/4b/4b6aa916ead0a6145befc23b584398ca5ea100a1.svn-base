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

#ifndef TIMING_SERVICE_HH
#define TIMING_SERVICE_HH

#include <sys/time.h>
#include <csignal>

#define TIMING_SERVICE_MAX_N_SIGNALS 8

class TimingService
{
public:
  TimingService();
  ~TimingService();

  /**
   * @brief Set the process's signal mask and signal actions for timer use.
   * @param signalsToIgnore Zero-terminated array of signal numbers to be handled by wait().
   * @return The status of the sigprocmask call; 0 = success.
   * @note TimingService clients MUST call this or defaultInitializeSignalHandling() prior to arming the timer!
   * @note SIGALRM will never be returned from a wait() call, as TimingService uses it internally.
   * @note No more than TIMING_SERVICE_MAX_N_SIGNALS will be processed.
   */
  int initializeSignalHandling(const int signumsToIgnore[]);

  /**
   * @brief Set the process's signal mask to a convenient default for timer use.
   * @return True if successful, false otherwise.
   * @note TimingService clients MUST call this or initializeSignalHandling() prior to arming the timer!
   * @note Calls initializeSignalHandling() internally.
   */
  bool defaultInitializeSignalHandling();

  /**
   * @brief Restore the process's original signal mask to its state at the last initializeSignalHandling() call.
   * @return True if successful, false otherwise.
   */
  bool restoreSignalHandling();

  /**
   * @brief Set the timer for the specified absolute time.
   * @param time Const reference to a timeval with the requested wakeup time.
   * @return True if the time is in the future, false otherwise.
   */
  bool setTimer(const timeval& time);

  /**
   * @brief Get the (estimated) absolute time of the next scheduled wakeup.
   * @param result A reference to the variable where the interval should be stored.
   */
  void getTimer(timeval& result);

  /**
   * @brief Disable the timer.
   */
  void stopTimer();

  /**
   * @brief Wait for a timer wakeup.
   * @return 0 if the wakeup was due to the timer, the signal number otherwise.
   * @note Signals like SIGINT, SIGHUP are caught by this method;
   *       dealing with them properly is the caller's responsibility.
   */
  int wait();

private:  

  // Deliberately not implemented
  TimingService(const TimingService&);
  TimingService& operator=(const TimingService&);

  //
  // Member variables
  //

  // Signal handling
  size_t m_nBlockedSignals;
  int m_blockedSignals[TIMING_SERVICE_MAX_N_SIGNALS + 1];
  struct sigaction m_restoreHandlers[TIMING_SERVICE_MAX_N_SIGNALS + 1];
  sigset_t m_sigset;
  sigset_t m_restoreSigset;
};

#endif

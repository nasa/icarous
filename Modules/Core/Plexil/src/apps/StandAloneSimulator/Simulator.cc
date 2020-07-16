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

#include "Simulator.hh"
#include "timeval-utils.hh"
#include "CommRelayBase.hh"
#include "ResponseBase.hh"
#include "ResponseMessage.hh"
#include "ResponseMessageManager.hh"

#include "Debug.hh"
#include "Error.hh"
#include "ThreadSpawn.hh"

#include <cerrno>
#include <iomanip>

Simulator::Simulator(CommRelayBase* commRelay, ResponseManagerMap& map) : 
  m_CommRelay(commRelay),
  m_TimingService(),
  m_Mutex(),
  m_CmdToRespMgr(map),
  m_SimulatorThread((pthread_t) 0),
  m_Started(false),
  m_Stop(false)
{
  m_CommRelay->registerSimulator(this);
}

Simulator::~Simulator()
{
  // Shut down anything that may be running
  stop();

  // delete all the response managers
  for (std::map<const std::string, ResponseMessageManager*>::iterator iter = m_CmdToRespMgr.begin();
       iter != m_CmdToRespMgr.end();
       ++iter)
    delete iter->second;

  debugMsg("Simulator:~Simulator",
	   " shutting down with " << m_Agenda.size() << " responses pending");
}

void Simulator::start()
{
  threadSpawn(run, this, m_SimulatorThread);
}

void Simulator::stop()
{
  debugMsg("Simulator:stop", " called");
  if (!m_Started)
    return;

  m_TimingService.stopTimer();

  if (m_Stop) {
	// we tried the gentle approach already -
	// take more drastic action
	if (m_SimulatorThread == pthread_self()) {
	  assertTrue_2(ALWAYS_FAIL, "Simulator:stop: Emergency stop!");
	}
	else {
	  int pthread_errno = pthread_cancel(m_SimulatorThread);
	  if (pthread_errno == ESRCH) {
		// no such thread to cancel, i.e. it's already dead
		m_Stop = false;
		m_Started = false;
		return;
	  }
	  else if (pthread_errno != 0) {
		assertTrueMsg(ALWAYS_FAIL, "Simulator::stop: fatal error: pthread_cancel returned " << pthread_errno);
	  }

	  // successfully canceled, wait for it to exit
	  pthread_errno = pthread_join(m_SimulatorThread, NULL);
	  if (pthread_errno == 0 || pthread_errno == ESRCH) {
		// thread joined or died before it could be joined
		// either way we succeeded
		m_Stop = false;
		m_Started = false;
	  }
	  else {
		assertTrueMsg(ALWAYS_FAIL, "Simulator::stop: fatal error: pthread_join returned " << pthread_errno);
	  }
	}
  }
  else {
	// Usual case
	// stop the sim thread
	m_Stop = true;
	if (m_SimulatorThread != pthread_self()) {
	  // Signal the thread to stop
	  int pthread_errno = pthread_kill(m_SimulatorThread, SIGTERM);
	  if (pthread_errno == 0) {
		// wait for thread to terminate
		pthread_errno = pthread_join(m_SimulatorThread, NULL);
		if (pthread_errno == 0 || pthread_errno == ESRCH) {
		  // thread joined or died before it could be joined
		  // either way we succeeded
		  m_Stop = false;
		  m_Started = false;
		}
		else {
		  assertTrueMsg(ALWAYS_FAIL, "Simulator::stop: fatal error: pthread_join returned " << pthread_errno);
		}
	  }
	  else if (pthread_errno != ESRCH) {
		// thread is already dead
		m_Stop = false;
		m_Started = false;
	  }
	  else {
		assertTrueMsg(ALWAYS_FAIL, "Simulator::stop: fatal error: pthread_kill returned " << pthread_errno);
	  }
	}
  }
}

void* Simulator::run(void* this_as_void_ptr)
{
  Simulator* _this = reinterpret_cast<Simulator*>(this_as_void_ptr);
  _this->simulatorTopLevel();
  return NULL;
}

void Simulator::simulatorTopLevel()
{
  // if called directly from the main() thread,
  // record our thread ID
  if (m_SimulatorThread == (pthread_t) 0)
	m_SimulatorThread = pthread_self();
  
  assertTrue_2(m_SimulatorThread == pthread_self(),
               "Internal error: simulatorTopLevel running in thread other than m_SimulatorThread");

  m_Started = true;

  timeval now;
  gettimeofday(&now, NULL);
  debugMsg("Simulator:start", " at "
		   << std::setiosflags(std::ios_base::fixed) 
		   << timevalToDouble(now));

  // Schedule initial telemetry responses
  ResponseManagerMap::const_iterator it = m_CmdToRespMgr.begin();
  while (it != m_CmdToRespMgr.end()) {
	it->second->scheduleInitialEvents(this);
	it++;
  }

  //
  // Initialize signal handling
  //
  if (!m_TimingService.defaultInitializeSignalHandling())
	return;
  
  //
  // Set the timer for the first event, if any
  //

  timeval firstWakeup;
  firstWakeup.tv_sec = firstWakeup.tv_usec = 0;

  // begin critical section
  {
	PLEXIL::ThreadMutexGuard mg(m_Mutex);
	if (!m_Agenda.empty())
	  firstWakeup = m_Agenda.begin()->first;
  }
  // end critical section

  if (firstWakeup.tv_sec != 0 || firstWakeup.tv_usec != 0) {
	debugMsg("Simulator:start",
			 " scheduling first event at "
			 << std::setiosflags(std::ios_base::fixed)
			 << timevalToDouble(firstWakeup));
	scheduleNextResponse(firstWakeup);
  }

  //
  // Handle timer events until interrupted
  //

  while (!m_Stop) {
	// wait for next timer wakeup
	int waitResult = m_TimingService.wait();
	if (waitResult != 0) {
	  // received some other signal
	  debugMsg("Simulator:simulatorTopLevel", " timing service received signal " << waitResult << ", exiting");
	  break;
	}

	// check for exit request
	if (m_Stop) {
	  debugMsg("Simulator:simulatorTopLevel", " stop request received");
	  break;
	}

	handleWakeUp();
  }

  //
  // Clean up
  //
  m_TimingService.stopTimer();
  m_TimingService.restoreSignalHandling();

  m_Stop = false;
  m_Started = false;

  debugMsg("Simulator:simulatorTopLevel", " cleaning up");
  // clean up agenda
  // begin critical section
  {
	PLEXIL::ThreadMutexGuard mg(m_Mutex);
	AgendaMap::iterator it = m_Agenda.begin();
	while (it != m_Agenda.end()) {
	  delete it->second;
	  m_Agenda.erase(it);    // it = m_Agenda.erase(it);
	  it = m_Agenda.begin(); // slightly kludgy, but safe
	}
  }
  // end critical section

  m_SimulatorThread = (pthread_t) 0;
}

ResponseMessageManager* Simulator::getResponseMessageManager(const std::string& cmdName) const
{
  if (m_CmdToRespMgr.find(cmdName) != m_CmdToRespMgr.end())
    return m_CmdToRespMgr.find(cmdName)->second;

  return NULL;
}

void Simulator::scheduleResponseForCommand(const std::string& command,
                                           void* uniqueId)
{
  timeval time;
  gettimeofday(&time, NULL);
  debugMsg("Simulator:scheduleResponseForCommand",
	   " for : " << command);
  bool valid = constructNextResponse(command, uniqueId, time, MSG_COMMAND);
  if (valid)
    scheduleNextResponse(time);
  else
    debugMsg("Simulator:scheduleResponseForCommand",
	     "Not a command that needs a response.");
}

bool Simulator::constructNextResponse(const std::string& command,
                                      void* uniqueId, 
				      timeval& time,
                                      int type)
{
  ResponseManagerMap::const_iterator iter  = m_CmdToRespMgr.find(command);
  if (iter == m_CmdToRespMgr.end())
    {
      debugMsg("Simulator:constructNextResponse",
	       " No response manager for \"" << command << "\", ignoring.");
      return false;
    }
  ResponseMessageManager* msgMgr = iter->second;
  timeval tDelay;
  const ResponseBase* respBase = msgMgr->getResponses(tDelay);
  if (respBase == NULL) 
    {
      debugMsg("Simulator:constructNextResponse",
	       " No more responses for \"" << command << "\"");
      return false;
    }
  
  ResponseMessage* respMsg = new ResponseMessage(respBase, uniqueId, type);
  scheduleMessage(tDelay, respMsg);
  
  return true;
}

/**
 * @brief Get the current value of the named state.
 * @param stateName The state name to which we are responding.
 * @return Pointer to a const ResponseBase object, or NULL.
 */
ResponseMessage* Simulator::getLookupNowResponse(const std::string& stateName, void* uniqueId) const
{
  ResponseManagerMap::const_iterator it = m_CmdToRespMgr.find(stateName);
  if (it == m_CmdToRespMgr.end())
    return NULL; // Name not known
  const ResponseBase* response =  it->second->getLastResponse();
  if (response == NULL)
    return NULL; // Command name (not a state), or no "last" value established
  return new ResponseMessage(response, uniqueId, MSG_LOOKUP);
}

/**
 * @brief Schedules a message to be sent at a future time.
 * @param time The absolute time at which to send the message.
 * @param msg The message to be sent.
 */
void Simulator::scheduleMessage(const timeval& delay, ResponseMessage* msg)
{
  timeval now;
  gettimeofday(&now, NULL);
  timeval eventTime = now + delay;
  scheduleMessageAbsolute(eventTime, msg);
}

void Simulator::scheduleMessageAbsolute(const timeval& eventTime, ResponseMessage* msg)
{
  debugMsg("Simulator:scheduleMessage",
		   " scheduling message at "
		   << std::setiosflags(std::ios_base::fixed) 
		   << timevalToDouble(eventTime));

  // begin critical section
  PLEXIL::ThreadMutexGuard mg(m_Mutex);
  m_Agenda.insert(std::pair<timeval, ResponseMessage*>(eventTime, msg));
  // end critical section
}

void Simulator::scheduleNextResponse(const timeval& time)
{
  timeval nextEvent;
  m_TimingService.getTimer(nextEvent);
  if ((nextEvent.tv_sec == 0 && nextEvent.tv_usec == 0)
	  || time < nextEvent) {
	// Schedule timer
	debugMsg("Simulator:scheduleNextResponse", " Scheduling a timer");
	bool timeIsFuture = m_TimingService.setTimer(time);
	if (timeIsFuture) {
	  debugMsg("Simulator:scheduleNextResponse",
			   " Timer set for "
			   << std::setiosflags(std::ios_base::fixed) 
			   << timevalToDouble(time));
	}
	else {
	  debugMsg("Simulator:scheduleNextResponse", " Immediate response required");
	  handleWakeUp();
	}
  }
  else {
	debugMsg("Simulator:scheduleNextResponse",
			 " A wakeup has already been scheduled for an earlier time.");
  }
}

void Simulator::handleWakeUp()
{
  timeval now;
  gettimeofday(&now, NULL);
  debugMsg("Simulator:handleWakeUp",
           " entered at "
           << std::setiosflags(std::ios_base::fixed) 
           << timevalToDouble(now));

  //
  // Send every message with a scheduled time earlier than now.
  //
  bool moreEvents = false;
  do {
	moreEvents = false;
	ResponseMessage* resp = NULL;

	// begin critical section
	{
	  PLEXIL::ThreadMutexGuard mg(m_Mutex);
	  if (!m_Agenda.empty()) {
		AgendaMap::iterator it = m_Agenda.begin();
		if (it->first < now) {
		  // grab the event and remove it from the map
		  resp = it->second;
		  m_Agenda.erase(it);
		  moreEvents = !m_Agenda.empty();
		}
	  }
	}
	// end critical section
	
	if (resp != NULL) {
	  m_CommRelay->sendResponse(resp);
	  ResponseMessageManager* manager = getResponseMessageManager(resp->getName());
	  manager->notifyMessageSent(resp->getResponseBase());
	  debugMsg("Simulator:handleWakeUp", " Sent response");
	  // delete resp; // handled by comm relay
	}
  }
  while (moreEvents);

  //
  // Schedule next wakeup, if any
  //
  bool scheduleTimer = false;
  timeval nextWakeup;
  // begin critical section
  {
	PLEXIL::ThreadMutexGuard mg(m_Mutex);
	if (!m_Agenda.empty()) {
	  nextWakeup = m_Agenda.begin()->first;
	  scheduleTimer = true;
	}
  }
  // end critical section

  if (scheduleTimer) {
    debugMsg("Simulator:handleWakeUp",
             " Scheduling next wakeup at "
             << std::setiosflags(std::ios_base::fixed) 
             << timevalToDouble(nextWakeup));
    scheduleNextResponse(nextWakeup);
  }
  debugMsg("Simulator:handleWakeUp", " completed");
}

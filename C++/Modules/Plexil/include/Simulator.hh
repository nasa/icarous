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
#ifndef SIMULATOR_HH
#define SIMULATOR_HH

#include <map>
#include "simdefs.hh"
#include "SimulatorScriptReader.hh"
#include "TimingService.hh"
#include "ThreadMutex.hh"

class ResponseMessageManager;
class ResponseMessage;
class CommRelayBase;

class Simulator
{
public:
  Simulator(CommRelayBase* commRelay, ResponseManagerMap& map);
  ~Simulator();

  /**
   * @brief Starts a new background thread with the simulator top level loop and returns immediately.
   * @note Call only after reading all scripts.
   */
  void start();

  /**
   * @brief Stops the simulator's top level thread.  Returns when the thread has rejoined.
   */
  void stop();

  /**
   * @brief The simulator top level loop.  
   * @note Call only after reading all scripts.
   */
  void simulatorTopLevel();

  ResponseMessageManager* getResponseMessageManager(const std::string& cmdName) const;


  /**
   * @brief Schedules a response to the named command.
   * @param command The command name to which we are responding.
   * @param uniqueId Caller-specified identifier, passed through the simulator to the comm relay.
   */
  void scheduleResponseForCommand(const std::string& command, 
                                  void* uniqueId = NULL);

  /**
   * @brief Get the current value of the named state.
   * @param stateName The state name to which we are responding.
   * @return Pointer to a const ResponseBase object, or NULL.
   */
  ResponseMessage* getLookupNowResponse(const std::string& stateName, void* uniqueId) const;
  
  /**
   * @brief Schedules a message to be sent after an interval.
   * @param delay The delay after which to send the message.
   * @param msg The message to be sent.
   */
  void scheduleMessage(const timeval& delay, ResponseMessage* msg);

  /**
   * @brief Schedules a message to be sent at a future time.
   * @param time The absolute time at which to send the message.
   * @param msg The message to be sent.
   */
  void scheduleMessageAbsolute(const timeval& time, ResponseMessage* msg);

private:

  // Deliberately not implemented
  Simulator();
  Simulator(const Simulator&);
  Simulator& operator=(const Simulator&);

  // Thread function for pthread_create
  static void* run(void * this_as_void_ptr);

  void handleWakeUp();

  void scheduleNextResponse(const timeval& time);
  
  /**
   * @brief Constructs a response to the named command.
   * @param command The command name to which we are responding.
   * @param uniqueId Caller-specified identifier, passed through the simulator to the comm relay.
   * @param timeval (Out parameter) The time at which the response will be sent.
   * @param type One of MSG_TELEMETRY or MSG_COMMAND.
   */
  bool constructNextResponse(const std::string& command, 
                             void* uniqueId,
                             timeval& time, 
                             int type);
  

  CommRelayBase* m_CommRelay;
  TimingService m_TimingService;
  PLEXIL::ThreadMutex m_Mutex;

  typedef std::multimap<timeval, ResponseMessage*> AgendaMap;
  AgendaMap m_Agenda;
  ResponseManagerMap& m_CmdToRespMgr;
  pthread_t m_SimulatorThread;
  bool m_Started;
  bool m_Stop;
};

#endif // SIMULATOR_HH

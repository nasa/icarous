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

#include "TelemetryResponseManager.hh"
#include "Simulator.hh"
#include "ResponseBase.hh"
#include "ResponseMessage.hh"
#include "Debug.hh"
#include "Error.hh"

TelemetryResponseManager::TelemetryResponseManager(const std::string& id)
  : ResponseMessageManager(id),
    m_LastResponse(NULL)
{
  debugMsg("TelemetryResponseManager:constructor", " " << id);
  // Counter starts from 0 for telemetry
  m_Counter = 0;
}

TelemetryResponseManager::~TelemetryResponseManager()
{
  m_LastResponse = NULL;
}

MsgType TelemetryResponseManager::getType()
{
  return MSG_TELEMETRY;
}

const ResponseBase* TelemetryResponseManager::getLastResponse() const
{
  return m_LastResponse;
}

void TelemetryResponseManager::addResponse(ResponseBase* resp, int cmdIndex)
{
  ResponseMessageManager::addResponse(resp, cmdIndex);
  m_Counter++;
}

/**
 * @brief Schedule the events dictated by this manager.
 */ 

void TelemetryResponseManager::scheduleInitialEvents(Simulator* sim)
{
  debugMsg("TelemetryResponseManager:scheduleInitialEvents", " entered");
  // Set the default (time zero) response as the current value.
  m_LastResponse = m_DefaultResponse;
  for (IndexResponseMap::const_iterator it = m_CmdIdToResponse.begin();
       it != m_CmdIdToResponse.end();
       ++it) {
    const ResponseBase* base = it->second;
    checkError(base != NULL,
               "TelemetryResponseManager: event list entry is null!");
    const timeval& delay = base->getDelay();
    ResponseMessage* msg = new ResponseMessage(base, NULL, MSG_TELEMETRY);
    debugMsg("TelemetryResponseManager:scheduleInitialEvents",
             " scheduling telemetry message for \"" << m_Identifier << "\" at " << delay.tv_sec);
    sim->scheduleMessage(delay, msg);
  }
}

/**
 * @brief Report that this message has been sent.
 */
void TelemetryResponseManager::notifyMessageSent(const ResponseBase* resp)
{
  checkError(resp->getManager() == this,
	     "TelemetryResponseManager::notifyMessageSent: notified wrong manager!");
  m_LastResponse = resp;
}

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
#include "ResponseMessageManager.hh"
#include "ResponseMessage.hh"
#include "ResponseBase.hh"

#include "Debug.hh"
#include "Error.hh"

#include <sys/time.h>

ResponseMessageManager::ResponseMessageManager(const std::string& id)
  : m_Identifier(id), 
    m_DefaultResponse(NULL),
    m_Counter(1)
{
}

ResponseMessageManager::~ResponseMessageManager()
{
  for (std::map<int, const ResponseBase*>::iterator iter = m_CmdIdToResponse.begin();
       iter != m_CmdIdToResponse.end(); 
       ++iter)
    delete iter->second;
  delete m_DefaultResponse;
}

MsgType ResponseMessageManager::getType()
{
  return MSG_COMMAND;
}

const ResponseBase* ResponseMessageManager::getLastResponse() const
{
  return NULL;
}

const std::string& ResponseMessageManager::getIdentifier() const 
{
  return m_Identifier;
}

int ResponseMessageManager::getCounter() const 
{
  return m_Counter;
}

void ResponseMessageManager::addResponse(ResponseBase* resp, int cmdIndex)
{
  // Make sure the command index had not been specified before.
  if (m_CmdIdToResponse.find(cmdIndex) != m_CmdIdToResponse.end())
    {
      debugMsg("ResponseMessageManager:addResponse",
           "Warning for " << m_Identifier
                << ": Command index " << cmdIndex
           << " has been repeated. Ignoring it.");
      delete resp;
      return;
    }

  resp->setManager(this);
  if (cmdIndex == 0)
    {
      m_DefaultResponse = resp;
    }
  else
    m_CmdIdToResponse[cmdIndex] = resp;
}

const ResponseBase* ResponseMessageManager::getResponses(timeval& tDelay)
{
  IndexResponseMap::iterator iter;
  const ResponseBase* respBase;
  if ((iter = m_CmdIdToResponse.find(m_Counter)) == m_CmdIdToResponse.end())
    {
      debugMsg("ResponseMessageManager:getResponses",
           " for " << m_Identifier << ": Getting default response");
      respBase = m_DefaultResponse;
    }
  else
    {
      debugMsg("ResponseMessageManager:getResponses",
           " for " << m_Identifier << ": Using response for index " << m_Counter);
      respBase = iter->second;
    }
  debugMsg("ResponseMessageManager:getResponses",
       " " << m_Identifier << ", count: " << m_Counter);
  ++m_Counter;

  // This shouldn't happen, but check anyway just in case
  assertTrueMsg(respBase != NULL,
                "ResponseMessageManager::getResponses: Internal error: No response found for \""
                << m_Identifier << "\"");

  if (respBase->getNumberOfResponses() > 0)
    {
      tDelay = respBase->getDelay();
      return respBase;
    }
  return NULL;
}

const ResponseBase* ResponseMessageManager::getDefaultResponse()
{
  return m_DefaultResponse;
}

/**
 * @brief Schedule the events dictated by this manager.
 * @note The default method does nothing.
 */ 
void ResponseMessageManager::scheduleInitialEvents(Simulator* sim)
{
  debugMsg("ResponseMessageManager:scheduleInitialEvents",
       " " << m_Identifier << " is a command manager, ignoring");
}

/**
 * @brief Report that this message has been sent.
 * @note The default method does nothing.
 */
void ResponseMessageManager::notifyMessageSent(const ResponseBase* resp)
{
}

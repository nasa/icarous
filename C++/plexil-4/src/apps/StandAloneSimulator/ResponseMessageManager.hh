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
#ifndef RESPONSE_MESSAGE_MANAGER_HH
#define RESPONSE_MESSAGE_MANAGER_HH

#include "ResponseMessage.hh" // enum MsgType

#include <map>

class Simulator;

struct timeval;

/**
 * @brief Base class which represents the simulation script for the named command or state.
 */
class ResponseMessageManager
{
public:
  ResponseMessageManager(const std::string& id);

  virtual ~ResponseMessageManager();

  virtual MsgType getType();

  // used only for LookupNow (i.e. telemetry managers)
  virtual const ResponseBase* getLastResponse() const;

  const std::string& getIdentifier() const;

  int getCounter() const;

  virtual void addResponse(ResponseBase* resp, int cmdIndex);

  const ResponseBase* getResponses(timeval& tDelay);

  const ResponseBase* getDefaultResponse();

  //
  // Virtual methods for extension
  //

  /**
   * @brief Schedule the events dictated by this manager.
   * @note The default method does nothing.
   */ 
  virtual void scheduleInitialEvents(Simulator* sim);

  /**
   * @brief Report that this message has been sent.
   * @note The default method does nothing.
   */
  virtual void notifyMessageSent(const ResponseBase* resp);

protected:

  //
  // Member variables shared with derived classes
  //

  typedef std::map<int, const ResponseBase*> IndexResponseMap;
  IndexResponseMap m_CmdIdToResponse;
  const std::string m_Identifier;
  const ResponseBase* m_DefaultResponse;
  int m_Counter;

private:

  // Deliberately not implemented
  ResponseMessageManager();
  ResponseMessageManager(const ResponseMessageManager&);
  ResponseMessageManager& operator=(const ResponseMessageManager&);

};
#endif // RESPONSE_MESSAGE_MANAGER_HH

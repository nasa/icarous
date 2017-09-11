/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

#ifndef TELEMETRY_RESPONSE_MANAGER_HH
#define TELEMETRY_RESPONSE_MANAGER_HH

#include "ResponseMessageManager.hh"

class TelemetryResponseManager : public ResponseMessageManager
{
public:
  TelemetryResponseManager(const std::string& id);

  virtual ~TelemetryResponseManager();

  virtual MsgType getType();

  virtual const ResponseBase* getLastResponse() const;

  // Wrapper around the base class method
  virtual void addResponse(ResponseBase* resp, int cmdIndex);

  /**
   * @brief Schedule the events dictated by this manager.
   */ 
  virtual void scheduleInitialEvents(Simulator* sim);

  /**
   * @brief Report that this message has been sent.
   */
  virtual void notifyMessageSent(const ResponseBase* resp);

private:
  // Deliberately not implemented
  TelemetryResponseManager();
  TelemetryResponseManager(const TelemetryResponseManager&);
  TelemetryResponseManager& operator=(const TelemetryResponseManager&);

  const ResponseBase* m_LastResponse;
};


#endif // TELEMETRY_RESPONSE_MANAGER_HH

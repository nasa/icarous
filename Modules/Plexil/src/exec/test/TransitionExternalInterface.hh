/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

#ifndef PLEXIL_TRANSITION_EXTERNAL_INTERFACE_HH
#define PLEXIL_TRANSITION_EXTERNAL_INTERFACE_HH

#include "ExternalInterface.hh"

namespace PLEXIL
{

class TransitionExternalInterface : public ExternalInterface
{
public:
  TransitionExternalInterface()
    : ExternalInterface()
  {
  }

  ~TransitionExternalInterface()
  {
  }

  void lookupNow(State const & /* state */, StateCacheEntry & /* entry */) {}
  void subscribe(State const & /* state */) {}
  void unsubscribe(State const & /* state */) {}
  void setThresholds(State const & /* state */, double /* hi */, double /* lo */) {}
  void setThresholds(State const & /* state */, int32_t /* hi */, int32_t /* lo */) {}
  void enqueueCommand(Command *cmd) {}
  void abortCommand(Command *cmd) {}
  void enqueueUpdate(Update *update) {}
  void executeOutboundQueue() {}
  double currentTime() {return 0.0;}

protected:
  void executeCommand(Command * /* cmd */) {}
  void reportCommandArbitrationFailure(Command * /* cmd */) {}
  void invokeAbort(Command * /* cmd */) {}
  void executeUpdate(Update * /* update */) {}

};

} // namespace PLEXIL

#endif // PLEXIL_TRANSITION_EXTERNAL_INTERFACE_HH

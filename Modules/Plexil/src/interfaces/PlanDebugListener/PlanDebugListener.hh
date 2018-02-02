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

#ifndef PLEXIL_PLAN_DEBUG_LISTENER_HH
#define PLEXIL_PLAN_DEBUG_LISTENER_HH

#include "ExecListener.hh"

namespace PLEXIL
{
  // Provides output from execution useful for debugging a Plexil plan.

  class PlanDebugListener : public ExecListener
  {
  public:

    PlanDebugListener();
    PlanDebugListener(pugi::xml_node const xml);
    virtual ~PlanDebugListener();

    // These methods have no special function.
    virtual bool initialize() { return true; }
    virtual bool start() { return true; }
    virtual bool stop() { return true; }
    virtual bool reset() { return true; }
    virtual bool shutdown() { return true; }

    // Capture and report about useful node state transitions.
    void implementNotifyNodeTransition(NodeState prevState,
                                       Node *node) const;

  private:
    // Disallow copy, and assignment
    PlanDebugListener(const PlanDebugListener&);
    PlanDebugListener& operator= (const PlanDebugListener&);
  };
}

#endif // PLEXIL_PLAN_DEBUG_LISTENER_HH

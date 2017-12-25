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

#include "PlanDebugListener.hh"

#include "Debug.hh"
#include "Node.hh"

#include "pugixml.hpp"

#include <iomanip> // for setprecision

namespace PLEXIL
{
  PlanDebugListener::PlanDebugListener () { }

  PlanDebugListener::PlanDebugListener (pugi::xml_node const xml)
    : ExecListener (xml)
  { }

  PlanDebugListener::~PlanDebugListener () { }

  // For now, use the DebugMsg facilities (really intended for debugging the
  // *executive* and not plans) to display messages of interest.  Later, a more
  // structured approach including listener filters and a different user
  // interface may be in order.

  void PlanDebugListener::
  implementNotifyNodeTransition (NodeState /* prevState */, Node *nodeId) const
  {
    condDebugMsg((nodeId->getState() == FINISHED_STATE),
                 "Node:clock",
                 "Node '" << nodeId->getNodeId() <<
                 "' finished at " << std::setprecision(15) <<
                 nodeId->getCurrentStateStartTime() << " (" <<
                 nodeId->getOutcome() << ")");
    condDebugMsg((nodeId->getState() == EXECUTING_STATE),
                 "Node:clock",
                 "Node '" << nodeId->getNodeId() <<
                 "' started at " << std::setprecision(15) <<
                 nodeId->getCurrentStateStartTime());
  }

}

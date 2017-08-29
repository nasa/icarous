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

#include "NodeTimepointValue.hh"

#include "Node.hh"

#include <iomanip> // for std::setprecision()

namespace PLEXIL
{
  static char const *timepointName[NODE_STATE_MAX][2] =
    {{"INVALID.START", "INVALID.END"},
     {"INACTIVE.START", "INACTIVE.END"},
     {"WAITING.START", "WAITING.END"},
     {"EXECUTING.START", "EXECUTING.END"},
     {"ITERATION_ENDED.START", "ITERATION_ENDED.END"},
     {"FINISHED.START", "FINISHED.END"},
     {"FAILING.START", "FAILING.END"},
     {"FINISHING.START", "FINISHING.END"}};
      

  NodeTimepointValue::NodeTimepointValue(Node *node,
                                         NodeState state,
                                         bool isEnd)
    : NotifierImpl(),
      m_time(0.0),
      m_next(NULL),
      m_node(node),
      m_state(state),
      m_end(isEnd),
      m_known(false)
  {
  }

  NodeTimepointValue::~NodeTimepointValue()
  {
  }


  NodeState NodeTimepointValue::state() const
  {
    return m_state;
  }

  bool NodeTimepointValue::isEnd() const
  {
    return m_end;
  }

  char const *NodeTimepointValue::getName() const
  {
    return timepointName[m_state][m_end ? 1 : 0];
  }
   
  char const *NodeTimepointValue::exprName() const
  {
    return "NodeTimepointValue";
  }

  ValueType NodeTimepointValue::valueType() const
  {
    return DATE_TYPE;
  }

  bool NodeTimepointValue::isKnown() const
  {
    return m_known;
  }

  bool NodeTimepointValue::getValue(Real &result) const // FIXME
  {
    if (m_known)
      result = m_time;
    return m_known;
  }

  void NodeTimepointValue::printValue(std::ostream &s) const
  {
    Real tym;
    if (getValue(tym))
      s << std::setprecision(15) << tym; // FIXME: needs better format
    else
      s << UNKNOWN_STR;
  }

  void NodeTimepointValue::printSpecialized(std::ostream &s) const
  {
    if (m_node)
      s << m_node->getNodeId() << '.';
    s << timepointName[m_state][m_end ? 1 : 0];
  }

  void NodeTimepointValue::setValue(Real newval) // FIXME
  {
    m_time = newval;
    m_known = true;
    publishChange();
  }

  void NodeTimepointValue::reset()
  {
    m_known = false;
    publishChange();
  }

  NodeTimepointValue *NodeTimepointValue::next() const
  {
    return m_next;
  }

  void NodeTimepointValue::setNext(NodeTimepointValue *nxt)
  {
    m_next = nxt;
  }

} // namespace PLEXIL

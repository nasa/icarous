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

#include "NodeVariables.hh"

#include "Node.hh"

namespace PLEXIL
{
  //
  // StateVariable
  //

  StateVariable::StateVariable(Node const &node)
    : NotifierImpl(),
      m_node(node)
  {
  }

  StateVariable::~StateVariable()
  {
  }

  bool StateVariable::getValue(uint16_t &result) const
  {
    result = m_node.getState();
    return true;
  }
      
  void StateVariable::printValue(std::ostream& s) const
  {
    s << nodeStateName(m_node.getState());
  }

  void StateVariable::printSpecialized(std::ostream& s) const
  {
    s << m_node.getNodeId() << ' ';
  }

  void StateVariable::changed()
  {
    this->publishChange();
  }

  //
  // OutcomeVariable
  //

  OutcomeVariable::OutcomeVariable(Node const &node)
    : NotifierImpl(),
      m_node(node)
  {
  }

  OutcomeVariable::~OutcomeVariable()
  {
  }

  bool OutcomeVariable::isKnown() const
  {
    uint16_t outcome = m_node.getOutcome();
    return (outcome == NO_OUTCOME);
  }

  bool OutcomeVariable::getValue(uint16_t &result) const
  {
    uint16_t outcome = m_node.getOutcome();
    if (outcome == NO_OUTCOME)
      return false;
    result = outcome;
    return true;
  }
      
  void OutcomeVariable::printValue(std::ostream& s) const
  {
    uint16_t outcome = m_node.getOutcome();
    if (outcome == NO_OUTCOME)
      s << "UNKNOWN";
    else
      s << outcomeName((NodeOutcome) outcome);
  }

  void OutcomeVariable::printSpecialized(std::ostream& s) const
  {
    s << m_node.getNodeId() << ' ';
  }

  void OutcomeVariable::changed()
  {
    this->publishChange();
  }

  //
  // FailureVariable
  //

  FailureVariable::FailureVariable(Node const &node)
    : NotifierImpl(),
      m_node(node)
  {
  }

  FailureVariable::~FailureVariable()
  {
  }

  bool FailureVariable::isKnown() const
  {
    uint16_t ftype = m_node.getFailureType();
    return (ftype == NO_FAILURE);
  }

  bool FailureVariable::getValue(uint16_t &result) const
  {
    uint16_t ftype = m_node.getFailureType();
    if (ftype == NO_FAILURE)
      return false;
    result = ftype;
    return true;
  }
      
  void FailureVariable::printValue(std::ostream& s) const
  {
    s << failureTypeName(m_node.getFailureType());
  }

  void FailureVariable::printSpecialized(std::ostream& s) const
  {
    s << m_node.getNodeId() << ' ';
  }

  void FailureVariable::changed()
  {
    this->publishChange();
  }

} // namespace PLEXIL

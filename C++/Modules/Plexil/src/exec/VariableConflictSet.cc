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

#include "VariableConflictSet.hh"

#include "Node.hh"
#include "lifecycle-utils.h"

#include <algorithm> // std::find()

namespace PLEXIL
{

  VariableConflictSet::VariableConflictSet()
    : m_next(NULL),
      m_variable(NULL)
  {
    m_nodes.reserve(1);
  }

  VariableConflictSet::~VariableConflictSet()
  {
  }

  Expression const *VariableConflictSet::getVariable() const
  {
    return m_variable;
  }

  void VariableConflictSet::setVariable(Expression *a)
  {
    m_variable = a;
  }

  VariableConflictSet *VariableConflictSet::next() const
  {
    return m_next;
  }

  void VariableConflictSet::setNext(VariableConflictSet *nxt)
  {
    m_next = nxt;
  }

  size_t VariableConflictSet::size() const
  {
    return m_nodes.size();
  }

  bool VariableConflictSet::empty() const
  {
    return m_nodes.empty();
  }

  void VariableConflictSet::push(Node *node)
  {
    // Most common case first
    if (m_nodes.empty()) {
      m_nodes.push_back(node);
      return;
    }
    int32_t prio = node->getPriority();
    for (std::vector<Node *>::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
      if (node == *it)
        return;
      if (prio < (*it)->getPriority()) {
        m_nodes.insert(it, node);
        return;
      }
    }
    // If we got here, has worse priority than everything already in the list
    m_nodes.push_back(node);
  }

  Node *VariableConflictSet::front()
  {
    if (m_nodes.empty())
      return NULL;
    return m_nodes.front();
  }

  void VariableConflictSet::remove(Node *node)
  {
    std::vector<Node *>::iterator it = 
      std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end())
      m_nodes.erase(it);
  }

  size_t VariableConflictSet::front_count() const
  {
    // Expected most common case first
    if (m_nodes.size() == 1)
      return 1;
    if (m_nodes.empty())
      return 0;
    size_t result = 1;
    int32_t prio = m_nodes.front()->getPriority();
    for (size_t i = 1;
         i < m_nodes.size() && m_nodes[i]->getPriority() == prio;
         ++i, ++result)
      {}
    return result;
  }

  VariableConflictSet::const_iterator VariableConflictSet::begin() const
  {
    return m_nodes.begin();
  }

  VariableConflictSet::const_iterator VariableConflictSet::end() const
  {
    return m_nodes.end();
  }

  //
  // Homegrown allocator
  //

  static VariableConflictSet *s_freeList = NULL;

  VariableConflictSet *VariableConflictSet::allocate()
  {
    if (s_freeList) {
      VariableConflictSet *result = s_freeList;
      s_freeList = s_freeList->m_next;
      // Wipe clean
      result->m_next = NULL;
      result->m_variable = NULL;
      result->m_nodes.clear();
      return result;
    }
    else {
      return new VariableConflictSet();
    }
  }

  extern "C"
  void cleanupVariableConflictSets()
  {
    while (s_freeList) {
      VariableConflictSet *temp = s_freeList;
      s_freeList = s_freeList->next();
      delete temp;
    }
  }

  void VariableConflictSet::release(VariableConflictSet *v)
  {
    static bool sl_cleanupInstalled = false;

    v->m_next = s_freeList;
    s_freeList = v;
    if (sl_cleanupInstalled)
      return;

    plexilAddFinalizer(&cleanupVariableConflictSets);
    sl_cleanupInstalled = true;
  }

} // namespace PLEXIL

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

#include "Update.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExternalInterface.hh"
#include "NodeConnector.hh"

namespace PLEXIL
{

  // Simple linked-list entry.
  struct Pair
  {
    Pair(std::string const &nam,
         Expression *exp,
         bool isGarbage)
      : next(NULL),
        name(nam),
        expr(exp),
        garbage(isGarbage)
    {
    }

    ~Pair()
    {
      if (garbage)
        delete expr;
    }

    Pair *next;
    std::string const name;
    Expression *expr;

  private:
    bool garbage;
  };

  Update::Update(NodeConnector *node)
    : m_next(NULL),
      m_source(node),
      m_ack("ack"),
      m_pairs(NULL)
  {
  }

  Update::~Update() 
  {
    cleanUp();
  }

  void Update::cleanUp()
  {
    Pair *next = m_pairs;
    m_pairs = NULL;
    while (next) {
      Pair *tmp = next->next;
      delete next;
      next = tmp;
    }
    m_valuePairs.clear();
  }

  void Update::reservePairs(size_t n)
  {
    // Preallocate space for value pairs
    m_valuePairs.grow(n);
  }

  bool Update::addPair(std::string const &name, Expression *exp, bool garbage)
  {
    check_error_1(exp);

    debugMsg("Update:addPair", " name = \"" << name << "\", exp = " << *exp);
    Pair **tmp = &m_pairs;
    // Search to end of list for duplicate name
    while (*tmp) {
      if ((*tmp)->name == name)
        return false;
      tmp = &((*tmp)->next);
    }

    // tmp should now be pointing at last pointer in list
    // (could be m_pairs)
    *tmp = new Pair(name, exp, garbage);
    return true;
  }

  void Update::fixValues()
  {
    Pair *tmp = m_pairs;
    while (tmp) {
      m_valuePairs[tmp->name] = tmp->expr->toValue();
      debugMsg("Update:fixValues",
               " fixing pair \"" << tmp->name << "\", "
               << tmp->expr << " = " << tmp->expr->toValue());
      tmp = tmp->next;
    }
  }

  void Update::activate() 
  {
    Pair *tmp = m_pairs;
    while (tmp) {
      tmp->expr->activate();
      tmp = tmp->next;
    }
    m_ack.activate(); // resets to false
  }

  void Update::execute()
  {
    assertTrue_1(m_ack.isActive());
    fixValues();
    g_interface->enqueueUpdate(this);
  }

  void Update::acknowledge(bool ack)
  {
    if (!m_ack.isActive())
      return; // ignore if not executing
    m_ack.setValue(ack);
  }

  void Update::deactivate()
  {
    Pair *tmp = m_pairs;
    while (tmp) {
      tmp->expr->deactivate();
      tmp = tmp->next;
    }
    m_ack.deactivate();
  }

}

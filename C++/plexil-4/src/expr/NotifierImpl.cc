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

#include "NotifierImpl.hh"

// This definition should be commented out for production use.
// Uncomment it if you need to trace setup or teardown of, or propagation
// through, the notification graph.
// #define LISTENER_DEBUG 1

#ifdef LISTENER_DEBUG
#include "Debug.hh"
#include <typeinfo>
#endif

#include "Error.hh"

#include <algorithm> // for std::find()

namespace PLEXIL {

  //
  // NotifierImpl
  //

  // Static initialization
#ifdef RECORD_EXPRESSION_STATS
  NotifierImpl *NotifierImpl::s_instanceList = NULL;
#endif

  NotifierImpl::NotifierImpl()
    : m_activeCount(0),
      m_outgoingListeners()
  {
#ifdef RECORD_EXPRESSION_STATS
    m_prev = NULL;
    m_next = s_instanceList;
    s_instanceList = this;
    if (m_next)
      m_next->m_prev = this;
#endif
  }

  NotifierImpl::~NotifierImpl()
  {
#ifdef LISTENER_DEBUG
    if (!m_outgoingListeners.empty()) {
      std::cerr << "*** " << (Expression *) this
                << " HAS " << m_outgoingListeners.size() << " OUTGOING LISTENERS:";
      for (std::vector<ExpressionListener *>::const_iterator it = m_outgoingListeners.begin();
           it != m_outgoingListeners.end();
           ++it)
        std::cerr << ' ' << *it << ' ';
      std::cerr << std::endl;
    }
#endif

    assertTrue_2(m_outgoingListeners.empty(),
                 "Error: Expression still has outgoing listeners.");

#ifdef RECORD_EXPRESSION_STATS
    // Delete this from instance list
    if (m_prev)
      m_prev->m_next = m_next; // may be null
    else
      s_instanceList = m_next; // this was newest - next may be null

    if (m_next)
      m_next->m_prev = m_prev; // may be null
#endif
  }

  bool NotifierImpl::isActive() const
  {
    return m_activeCount > 0;
  }

  void NotifierImpl::activate()
  {
    bool changed = (!m_activeCount);
    ++m_activeCount;
    if (changed)
      this->handleActivate();
    else
      // Check for counter wrap only if active at entry
      assertTrue_2(m_activeCount,
                   "NotifierImpl::activate: Active counter overflowed.");
  }

  // No-op default method.
  void NotifierImpl::handleActivate()
  {
  }

  void NotifierImpl::deactivate()
  {
    assertTrue_2(m_activeCount != 0,
                 "Attempted to deactivate expression too many times.");
    if (--m_activeCount == 0)
      this->handleDeactivate();
  }

  // No-op default method.
  void NotifierImpl::handleDeactivate()
  {
  }

  void NotifierImpl::notifyChanged()
  {
    if (isActive())
      this->handleChange();
  }

  // Default method.
  void NotifierImpl::handleChange()
  {
    publishChange();
  }

  //
  // Expression listener graph construction and teardown
  // 

  //
  // In order to reduce memory usage and graph propagation delays, we try to
  // minimize the number of listeners added to expressions.
  //
  // There are four cases where we want to add a listener to an expression:
  //  1. Root expression, i.e. a node condition. This is the expression on which
  //     addListener() is explicitly called during plan loading.
  //  2. Common subexpression of one or more root expressions (future).
  //     These will be broken out at the node level.
  //  3. Interior subexpression whose value can change independently of its
  //     parameters (e.g. Lookup, random number generator).
  //  4. Leaf expression that can change, i.e. variable.
  //
  // Cases 1-3 are root or interior expressions. In each case, we add the
  // interior expression as a listener to its descendants (cases 2-4), and
  // the original listener is added to the node on which addListener() was
  // originally called.
  //

  //
  // Internal helpers for addListener() method
  //

  // Forward reference
  static void addListenerToSubexprs(Expression *l, Expression *e);
  
  class AddListenerHelper : public ExprUnaryOperator
  {
  public:
    AddListenerHelper(ExpressionListener *listener)
      : l(listener)
    {
    }

    virtual void operator()(Expression *x) const
    {
#ifdef LISTENER_DEBUG
      debugMsg("NotifierImpl:addListenerHelper",
               ' ' << x << " adding "
               << (ExpressionListener *) l);
#endif
      if (x->hasListeners()) {
        // Subexprs already dealt with, just listen to this one
        x->addListenerInternal(l);
      }
      else if (x->isPropagationSource()) {
        // This expression can independently generate notifications,
        // so add requested listener here ...
        x->addListenerInternal(l);
        // ... and make it listen to its descendants.
        addListenerToSubexprs(x, x);
      }
      else {
        // Not a source, recurse on descendants
        x->doSubexprs(*this);
      }
    }

  private:
    ExpressionListener *l;
  };

  static void addListenerToSubexprs(Expression *l, Expression *e)
  {
#ifdef LISTENER_DEBUG
    debugMsg("NotifierImpl:addListenerToSubexprs",
             ' ' << e << " adding " << l << " to subexpressions");
#endif
    e->doSubexprs(AddListenerHelper(l));
  }

  void NotifierImpl::addListener(ExpressionListener *ptr)
  {
#ifdef LISTENER_DEBUG
    debugMsg("NotifierImpl:addListener",
             ' ' << (Expression *) this << ' ' << *this
             << " adding " << ptr << ' ' << typeid(*ptr).name());
#endif
    if (m_outgoingListeners.empty()) {
#ifdef LISTENER_DEBUG
      debugMsg("NotifierImpl:addListener",
               ' ' << (Expression *) this
               << " had no listeners, adding it to its subexpressions");
#endif
      addListenerToSubexprs(this, this);
    }
    addListenerInternal(ptr);
  }

  // Internal member function, not meant to be called from outside this class.
  void NotifierImpl::addListenerInternal(ExpressionListener *ptr)
  {
    // Have to check for duplicates, sigh.
    std::vector<ExpressionListener *>::iterator it =
      std::find(m_outgoingListeners.begin(), m_outgoingListeners.end(), ptr);
    if (it != m_outgoingListeners.end()) {
#ifdef LISTENER_DEBUG
      debugMsg("NotifierImpl:addListener",
               ' ' << (Expression *) this << " listener " << ptr << " already present");
#endif
      return;
    }
    m_outgoingListeners.push_back(ptr);
#ifdef LISTENER_DEBUG
    debugMsg("NotifierImpl:addListener",
             ' ' << (Expression *) this << " added " << ptr);
#endif
  }

  //
  // Internal helpers for removeListener() method
  //

  class RemoveListenerHelper : public ExprUnaryOperator
  {
  public:
    RemoveListenerHelper(ExpressionListener *listener)
      : l(listener)
    {
    }

    virtual void operator()(Expression *e) const
    {
#ifdef LISTENER_DEBUG
      debugMsg("NotifierImpl:removeListener",
               ' ' << e << " removing " << l << " from subexpressions");
#endif
      e->removeListener(l);
    }

  private:
    ExpressionListener *l;
  };

  void NotifierImpl::removeListener(ExpressionListener *ptr)
  {
#ifdef LISTENER_DEBUG
    debugMsg("NotifierImpl:removeListener",
             ' ' << (Expression *) this << ' ' << *this
             << " removing " << ptr << ' ' << typeid(*ptr).name());
#endif
    std::vector<ExpressionListener *>::iterator it =
      std::find(m_outgoingListeners.begin(), m_outgoingListeners.end(), ptr);
    if (it == m_outgoingListeners.end()) {
#ifdef LISTENER_DEBUG
      condDebugMsg(m_outgoingListeners.empty(),
                   "NotifierImpl:removeListener",
                   ' ' << (Expression *) this << " has no listeners");
      condDebugMsg(!m_outgoingListeners.empty(),
                   "NotifierImpl:removeListener",
                   ' ' << (Expression *) this << " listener " << ptr << " not found");
#endif
    }
    else {
      m_outgoingListeners.erase(it);
#ifdef LISTENER_DEBUG
      debugMsg("NotifierImpl:removeListener",
               ' ' << (Expression *) this << " removed " << ptr);
#endif
      // If no one is listening to us, stop listening to our descendants
      if (m_outgoingListeners.empty())
        doSubexprs(RemoveListenerHelper(this));
    }
    // In either case, descendants might have this listener,
    // so walk the whole tree
    doSubexprs(RemoveListenerHelper(ptr));
  }

  bool NotifierImpl::hasListeners() const
  {
    return !m_outgoingListeners.empty();
  }

  void NotifierImpl::publishChange()
  {
    if (isActive())
      for (std::vector<ExpressionListener *>::iterator it = m_outgoingListeners.begin();
           it != m_outgoingListeners.end();
           ++it)
        (*it)->notifyChanged();
  }

#ifdef RECORD_EXPRESSION_STATS
  NotifierImpl const *NotifierImpl::next() const
  {
    return m_next;
  }
  
  NotifierImpl const *NotifierImpl::getInstanceList()
  {
    return s_instanceList;
  }

  size_t NotifierImpl::getListenerCount() const
  {
    return m_outgoingListeners.size();
  }
#endif

} // namespace PLEXIL

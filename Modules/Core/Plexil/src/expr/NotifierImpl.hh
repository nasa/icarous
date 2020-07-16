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

#ifndef PLEXIL_NOTIFIER_IMPL_HH
#define PLEXIL_NOTIFIER_IMPL_HH

#include "Expression.hh"

#include <vector>
#include <cstddef> // size_t

// Uncomment this to enable data gathering.
// Not intended for production use.
// #define RECORD_EXPRESSION_STATS 1

namespace PLEXIL
{

  /**
   * @class NotifierImpl
   * @brief Mixin class for expressions whose value may change. Implements expression graph notification.
   */

  //
  // The expression listener graph (really a forest of trees, there are no cycles)
  // is built during plan loading. Its purpose is to tell a node when one of its
  // conditions may have changed, so that it can be considered for a potential node
  // state transition.
  //

  class NotifierImpl : virtual public Expression
  {
  public:

    /**
     * @brief Destructor.
     */
    virtual ~NotifierImpl();

    //
    // Core NotifierImpl behavior
    //

    //
    // Expression notification graph API
    //

    /**
     * @brief Determine whether this expression is active.
     * @return true if active, false if not.
     */
    virtual bool isActive() const;

    /**
     * @brief Make this expression active.  It will publish value changes and it will accept
     *        incoming change notifications.
     * @note Default method. Calls handleActivate() if previously inactive.
     * @see handleActivate()
     */
    virtual void activate();

    /**
     * @brief Make this Expression inactive.  It will not publish value changes, nor will it
     *        accept incoming change notifications.
     * @note Default method. Calls handleDeactivate() if transitioning from active to inactive.
     * @see handleDeactivate()
     */
    virtual void deactivate();

    /**
     * @brief Add a listener for changes to this Expression's value.
     * @param ptr The pointer to the listener to add.
     * @note May be overridden (or wrapped) by derived classes.
     */
    virtual void addListener(ExpressionListener *ptr);

    /**
     * @brief Remove a listener from this Expression.
     * @param ptr The pointer to the listener to remove.
     */
    virtual void removeListener(ExpressionListener *ptr);

    /**
     * @brief Notify this expression that a subexpression's value has changed.
     * @note This method overrides the one on Expression.
     */
    virtual void notifyChanged();

#ifdef RECORD_EXPRESSION_STATS    
    /**
     * @brief Get head of list of instances.
     */
    static NotifierImpl const *getInstanceList();

    /**
     * @brief Get next in instance list
     */
    NotifierImpl const *next() const;

    /**
     * @brief Get number of listeners for this expression
     */
    size_t getListenerCount() const;
#endif

  protected:

    /**
     * @brief Report whether the expression has listeners.
     * @return True if present, false if not.
     */
    virtual bool hasListeners() const;

    /**
     * @brief Unconditionally add a listener for changes to this Expression's value.
     * @param ptr The pointer to the listener to add.
     */
    virtual void addListenerInternal(ExpressionListener *ptr);

    /**
     * @brief Default constructor.
     * @note Only available to derived classes.
     */
    NotifierImpl();

    /**
     * @brief Make this expression active.  It will publish value changes and it will accept
     *        incoming change notifications.
     * @note Default method does nothing.
     */
    virtual void handleActivate();

    /**
     * @brief Make this expression inactive.  It will not publish value changes, nor will it
     *        accept incoming change notifications.
     * @note Default method does nothing.
     */
    virtual void handleDeactivate();

    /**
     * @brief Called by notifyChanged() when the expression is active.
     * @note Default method calls publishChange().
     */
    virtual void handleChange();

    /**
     * @brief Notify all listeners that this expression's value has changed.
     */
    void publishChange();

  private:
    // Not implemented
    NotifierImpl(const NotifierImpl &);
    NotifierImpl &operator=(const NotifierImpl &);

    // Essential member variables
    size_t m_activeCount; // align to word size
    std::vector<ExpressionListener *> m_outgoingListeners; /*<! For outgoing message notifications (this expression's value has changed) */

#ifdef RECORD_EXPRESSION_STATS
    NotifierImpl *m_prev; // pointer to newer instance
    NotifierImpl *m_next; // pointer to older instance

    static NotifierImpl *s_instanceList;
#endif
    
  };

} // namespace PLEXIL

#endif // PLEXIL_NOTIFIER_IMPL_HH

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

#ifndef PLEXIL_EXEC_LISTENER_HH
#define PLEXIL_EXEC_LISTENER_HH

#include "PlexilListener.hh"
#include "NodeConstants.hh"

#include "pugixml.hpp"

namespace PLEXIL
{
  // Forward references
  class ExecListenerFilter;

  class Node;

  /**
   * @brief A base class for implementing notifications to external agents about exec state changes.
   * @note Provides event filtering hooks.
   */
  class ExecListener : public PlexilListener
  {
  public:

    /**
     * @brief Default constructor.
     */
    ExecListener();

    /**
     * @brief Constructor from configuration XML
     * @param xml Pointer to the (shared) configuration XML describing this listener.
     */
    ExecListener(pugi::xml_node const xml);

    /**
     * @brief Destructor.
     */
    virtual ~ExecListener();

    //
    // API to Exec
    // See ExecListenerBase.hh
    //

    /**
     * @brief Notify that nodes have changed state.
     * @param Vector of node state transition info.
     * @note Current states are accessible via the node.
     */
    void notifyOfTransitions(std::vector<NodeTransition> const &transitions) const;

    /**
     * @brief Notify that a variable assignment has been performed.
     * @param dest The Expression being assigned to.
     * @param destName A string naming the destination.
     * @param value The value (in internal Exec representation) being assigned.
     */
    void notifyOfAssignment(Expression const *dest,
                            std::string const &destName,
                            Value const &value) const;

    //
    // API to application
    //

    /**
     * @brief Notify that a plan has been received by the Exec.
     * @param plan The intermediate representation of the plan.
     */
    void notifyOfAddPlan(pugi::xml_node const plan) const;

    /**
     * @brief Notify that a library node has been received by the Exec.
     * @param libNode The intermediate representation of the plan.
     */
    void notifyOfAddLibrary(pugi::xml_node const libNode) const;

    //
    // API to be implemented by derived classes
    //

    /**
     * @brief Perform listener-specific initialization.
     * @return true if successful, false otherwise.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual bool initialize();

    /**
     * @brief Perform listener-specific startup.
     * @return true if successful, false otherwise.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual bool start();

    /**
     * @brief Perform listener-specific actions to stop.
     * @return true if successful, false otherwise.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual bool stop();

    /**
     * @brief Perform listener-specific actions to reset to initialized state.
     * @return true if successful, false otherwise.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual bool reset();

    /**
     * @brief Perform listener-specific actions to shut down.
     * @return true if successful, false otherwise.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual bool shutdown();

    /**
     * @brief Set the filter of this instance.
     * @param fltr Smart pointer to the filter.
     */
    void setFilter(ExecListenerFilter *fltr);

  protected:

    //
    // API to be implemented by derived classes
    //

    /**
     * @brief Notify that nodes have changed state.
     * @param Vector of node state transition info.
     * @note Current states are accessible via the node.
     * @note ExecListener provides a default method for backward commpatibility.
     *       Derived classes may implement their own method.
     */
    virtual void implementNotifyNodeTransitions(std::vector<NodeTransition> const & /* transitions */) const;

    /**
     * @brief Notify that a node has changed state.
     * @param prevState The old state.
     * @param node The node that has transitioned.
     * @note The current state is accessible via the node.
     * @note The default method does nothing.
     * @note Derived classes may implement methods for this, or for implementNotifyNodeTransitions() for batching purposes.
     */
    virtual void implementNotifyNodeTransition(NodeState /* prevState */,
                                               Node * /* node */) const;

    /**
     * @brief Notify that a plan has been received by the Exec.
     * @param plan The intermediate representation of the plan.
     * @note The default method does nothing.
     */
    virtual void implementNotifyAddPlan(pugi::xml_node const /* plan */) const;

    /**
     * @brief Notify that a library node has been received by the Exec.
     * @param libNode The intermediate representation of the plan.
     * @note The default method does nothing.
     */
    virtual void implementNotifyAddLibrary(pugi::xml_node const /* libNode */) const;

    /**
     * @brief Notify that a variable assignment has been performed.
     * @param dest The Expression being assigned to.
     * @param destName A string naming the destination.
     * @param value The value (in internal Exec representation) being assigned.
     */
    virtual void implementNotifyAssignment(Expression const * /* dest */,
                                           std::string const & /* destName */,
                                           Value const & /* value */) const;


    //
    // Shared API made available to derived classes
    //

    /**
     * @brief Construct the ExecListenerFilter specified by this listener's configuration XML.
     * @return True if successful, false otherwise.
     */
    virtual bool constructFilter();

    pugi::xml_node const getXml() const
    {
      return m_xml;
    }

    /**
     * @brief This instance's filter.
     */
    ExecListenerFilter *m_filter;

  private:

    pugi::xml_node const m_xml;
  };
}

#endif // PLEXIL_EXEC_LISTENER_HH

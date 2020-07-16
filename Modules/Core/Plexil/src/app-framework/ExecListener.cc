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

#include "ExecListener.hh"

#include "Debug.hh"
#include "Error.hh"
#include "ExecListenerFilter.hh"
#include "ExecListenerFilterFactory.hh"
#include "Expression.hh"
#include "InterfaceSchema.hh"
#include "NodeTransition.hh"

namespace PLEXIL
{

  /**
   * @brief Default constructor.
   */
  ExecListener::ExecListener()
    : PlexilListener(),
      m_filter(NULL)
  {
  }

  /**
   * @brief Constructor from configuration XML.
   */
  ExecListener::ExecListener(pugi::xml_node const xml)
    : PlexilListener(),
      m_filter(NULL)
  {
  }

  /**
   * @brief Destructor.
   */
  ExecListener::~ExecListener() 
  { 
    delete m_filter;
  }

  /**
   * @brief Notify that nodes have changed state.
   * @param Vector of node state transition info.
   */
  void ExecListener::notifyOfTransitions(const std::vector<NodeTransition>& transitions) const
  {
    debugMsg("ExecListener:notifyOfTransitions", " reporting " << transitions.size() << " transitions");
    this->implementNotifyNodeTransitions(transitions);
  }

  /**
   * @brief Notify that a plan has been received by the Exec.
   * @param plan The intermediate representation of the plan.
   * @param parent The name of the parent node under which this plan will be inserted.
   */
  void
  ExecListener::notifyOfAddPlan(pugi::xml_node const plan) const
  {
    if (!m_filter
        || m_filter->reportAddPlan(plan))
      this->implementNotifyAddPlan(plan);
  }

  /**
   * @brief Notify that a library node has been received by the Exec.
   * @param libNode The intermediate representation of the plan.
   */
  void 
  ExecListener::notifyOfAddLibrary(pugi::xml_node const libNode) const
  {
    if (!m_filter
        || m_filter->reportAddLibrary(libNode))
      this->implementNotifyAddLibrary(libNode);
  }

  /**
   * @brief Notify that a variable assignment has been performed.
   * @param dest The Expression being assigned to.
   * @param destName A string that names the destination.
   * @param value The value (in internal Exec representation) being assigned.
   */
  void
  ExecListener::notifyOfAssignment(Expression const *dest,
                                   const std::string& destName,
                                   const Value& value) const
  {
    if (!m_filter
        || m_filter->reportAssignment(dest, destName, value))
      this->implementNotifyAssignment(dest, destName, value);
  }

  /**
   * @brief Construct the ExecListenerFilter specified by this listener's configuration XML.
   * @return True if successful, false otherwise.
   */
  bool ExecListener::constructFilter()
  {
    if (m_xml.empty())
      return true; // nothing to do
    if (m_filter)
      return true; // already initialized

    pugi::xml_node filterSpec = m_xml.child(InterfaceSchema::FILTER_TAG());
    if (filterSpec.empty())
      return true;
              
    // Construct specified event filter
    pugi::xml_attribute filterTypeAttr = filterSpec.attribute(InterfaceSchema::FILTER_TYPE_ATTR());
    if (filterTypeAttr.empty()) {
      warn("ExecListener:constructFilter: invalid XML: <"
           << InterfaceSchema::FILTER_TAG()
           << "> element missing a "
           << InterfaceSchema::FILTER_TYPE_ATTR()
           << " attribute");
      return false;
    }

    const char* filterType = filterTypeAttr.value();
    if (!*filterType) {
      warn("ExecListener:constructFilter: invalid XML: <"
           << InterfaceSchema::FILTER_TAG()
           << "> element's "
           << InterfaceSchema::FILTER_TYPE_ATTR()
           << " attribute is empty");
      return false;
    }

    ExecListenerFilter *f = 
      ExecListenerFilterFactory::createInstance(std::string(filterType),
                                                filterSpec);
    if (!f) {
      warn("ExecListener:constructFilter: failed to construct exec listener filter "
           << filterType);
      return false;
    }

    if (!f->initialize()) {
      warn("ExecListener:constructFilter: error initializing listener filter " << filterType);
      delete f;
      return false;
    }
    m_filter = f;
    return true;
  }

  /**
   * @brief Perform listener-specific initialization.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListener::initialize()
  {
    if (!constructFilter())
      return false;
    return true; 
  }

  /**
   * @brief Perform listener-specific startup.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListener::start() 
  { 
    return true; 
  }

  /**
   * @brief Perform listener-specific actions to stop.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListener::stop() 
  {
    return true; 
  }

  /**
   * @brief Perform listener-specific actions to reset to initialized state.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListener::reset() 
  {
    return true; 
  }

  /**
   * @brief Perform listener-specific actions to shut down.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListener::shutdown() 
  { 
    return true; 
  }

  /**
   * @brief Set the filter of this instance.
   * @param fltr Pointer to the filter.
   */
  void ExecListener::setFilter(ExecListenerFilter *fltr)
  {
    m_filter = fltr;
  }

  //
  // Default methods to be overridden by derived classes
  //


  /**
   * @brief Notify that nodes have changed state.
   * @param Vector of node state transition info.
   * @note Current states are accessible via the node.
   * @note This default method is a convenience for backward compatibility.
   */
  void ExecListener::implementNotifyNodeTransitions(const std::vector<NodeTransition>& transitions) const
  {
    debugMsg("ExecListener:implementNotifyNodeTransitions", " default method called");
    if (!m_filter) {
      for (std::vector<NodeTransition>::const_iterator it = transitions.begin();
           it != transitions.end();
           ++it) 
        this->implementNotifyNodeTransition(it->state, it->node);
    }
    else {
      for (std::vector<NodeTransition>::const_iterator it = transitions.begin();
           it != transitions.end();
           ++it)
        if (m_filter->reportNodeTransition(it->state, it->node))
          this->implementNotifyNodeTransition(it->state, it->node);
    }
  }

  /**
   * @brief Notify that a node has changed state.
   * @param prevState The old state.
   * @param node The node that has transitioned.
   * @note The current state is accessible via the node.
   * @note The default method does nothing.
   */
  void ExecListener::implementNotifyNodeTransition(NodeState /* prevState */,
                                                   Node * /* node */) const
  {
    debugMsg("ExecListener:implementNotifyNodeTransition", " default method called");
  }

  /**
   * @brief Notify that a plan has been received by the Exec.
   * @param plan The intermediate representation of the plan.
   * @param parent The name of the parent node under which this plan will be inserted.
   * @note The default method does nothing.
   */
  void ExecListener::implementNotifyAddPlan(pugi::xml_node const /* plan */) const
  {
    debugMsg("ExecListener:implementNotifyAddPlan", " default method called");
  }

  /**
   * @brief Notify that a library node has been received by the Exec.
   * @param libNode The intermediate representation of the plan.
   * @note The default method does nothing.
   */
  void ExecListener::implementNotifyAddLibrary(pugi::xml_node const /* libNode */) const
  {
    debugMsg("ExecListener:implementNotifyAddLibrary", " default method called");
  }

  /**
   * @brief Notify that a variable assignment has been performed.
   * @param dest The Expression being assigned to.
   * @param destName A string naming the destination.
   * @param value The value (in internal Exec representation) being assigned.
   */
  void ExecListener::implementNotifyAssignment(Expression const * /* dest */,
                                               const std::string& /* destName */,
                                               const Value& /* value */) const
  {
    debugMsg("ExecListener:implementNotifyAssignment", " default method called");
  }

}

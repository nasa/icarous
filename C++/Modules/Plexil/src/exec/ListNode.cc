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

#include "ListNode.hh"

#include "BooleanOperators.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExprVec.hh"
#include "Function.hh"
#include "NodeFunction.hh"
#include "NodeOperatorImpl.hh"
#include "UserVariable.hh"

#include <algorithm> // for find_if

namespace PLEXIL
{

  //
  // Condition operators only used by ListNode
  //

  class AllFinished : public NodeOperatorImpl<Boolean>
  {
  public:
    ~AllFinished()
    {
    }

    DECLARE_NODE_OPERATOR_STATIC_INSTANCE(AllFinished);

    bool checkArgCount(size_t count) const
    {
      return true;
    }

    bool operator()(Boolean &result, Node const *node) const
    {
      std::vector<Node *> const &kids = node->getChildren();
      size_t total = kids.size();
      for (size_t i = 0; i < total; ++i) {
        if (kids[i]->getState() != FINISHED_STATE) {
          result = false;
          debugMsg("AllFinished", "result = false");
          return true;
        }
      }
      debugMsg("AllFinished", "result = true");
      result = true;
      return true; // always known
    }

  private:

    AllFinished()
      : NodeOperatorImpl<Boolean>("AllChildrenFinished")
    {
    }

    // Disallow copy, assign
    AllFinished(AllFinished const &);
    AllFinished &operator=(AllFinished const &);
  };

  class AllWaitingOrFinished : public NodeOperatorImpl<Boolean>
  {
  public:
    ~AllWaitingOrFinished()
    {
    }

    DECLARE_NODE_OPERATOR_STATIC_INSTANCE(AllWaitingOrFinished);

    bool checkArgCount(size_t count) const
    {
      return true;
    }

    bool operator()(Boolean &result, Node const *node) const
    {
      std::vector<Node *> const &kids = node->getChildren();
      size_t total = kids.size();
      for (size_t i = 0; i < total; ++i) {
        switch (kids[i]->getState()) {
        case WAITING_STATE:
        case FINISHED_STATE:
          break;

        default:
          result = false;
          debugMsg("AllWaitingOrFinished", " result = false");
          return true;
        }
      }
      result = true;
      debugMsg("AllWaitingOrFinished", " result = true");
      return true; // always known
    }

  private:
    // Should only be called from instance() static member function
    AllWaitingOrFinished()
      : NodeOperatorImpl<Boolean>("AllChildrenWaitingOrFinished")
    {
    }

    // Disallow copy, assign
    AllWaitingOrFinished(AllWaitingOrFinished const &);
    AllWaitingOrFinished &operator=(AllWaitingOrFinished const &);
  };

  //
  // ListNode member functions
  //

  ListNode::ListNode(char const *nodeId, Node *parent)
    : Node(nodeId, parent)
  {
  }

  /**
   * @brief Alternate constructor.
   * Used only by Exec test module, where all conditions are guaranteed to exist.
   */
  ListNode::ListNode(const std::string& type,
                     const std::string& name, 
                     NodeState state,
                     Node *parent)
    : Node(type, name, state, parent)
  {
    checkError(type == LIST || type == LIBRARYNODECALL,
               "Invalid node type \"" << type << "\" for a ListNode");

    switch (m_state) {
    case EXECUTING_STATE:
      deactivatePostCondition();
      m_conditions[ancestorExitIdx]->activate();
      m_conditions[ancestorInvariantIdx]->activate();
      m_conditions[ancestorEndIdx]->activate();
      break;

    case FINISHING_STATE:
      activateAncestorExitInvariantConditions();
      activateActionCompleteCondition();
      activateExitCondition();
      activateInvariantCondition();
      activatePostCondition();
      m_conditions[ancestorExitIdx]->activate();
      m_conditions[ancestorInvariantIdx]->activate();
      m_conditions[ancestorEndIdx]->activate();
      break;

    case FAILING_STATE:
      activateActionCompleteCondition();
      m_conditions[ancestorExitIdx]->activate();
      m_conditions[ancestorInvariantIdx]->activate();
      m_conditions[ancestorEndIdx]->activate();
      break;

    default:
      break;
    }
  }

  NodeVariableMap const *ListNode::getChildVariableMap() const
  {
    if (m_variablesByName)
      return m_variablesByName;

    // Search ancestors for first in chain
    Node *n = m_parent;
    NodeVariableMap const *map = NULL;
    while (n && !map) {
      map = n->getVariableMap();
      if (!map)
        n = n->m_parent;
    }
    return map;
  }

  // Create the ancestor end, ancestor exit, and ancestor invariant conditions required by children
  // This method is called after all user-spec'd conditions have been instantiated
  void ListNode::specializedCreateConditionWrappers()
  {
    // At least one node state function is dependent on child states,
    // so just add this node as a listener to each state variable.
    for (std::vector<Node *>::iterator it = m_children.begin();
         it != m_children.end();
         ++it)
      (*it)->getStateVariable()->addListener(this);

    // Not really a "wrapper", but this is best place to add it.
    Expression *cond =
      new NodeFunction(AllWaitingOrFinished::instance(), this);
    m_conditions[actionCompleteIdx] = cond;
    m_garbageConditions[actionCompleteIdx] = true;

    if (m_parent) {
      if (getExitCondition()) {
        if (getAncestorExitCondition()) {
          m_conditions[ancestorExitIdx] =
            makeFunction(BooleanOr::instance(),
                         getExitCondition(),
                         getAncestorExitCondition(),
                         false,
                         false);
          m_garbageConditions[ancestorExitIdx] = true;
        }
        else 
          m_conditions[ancestorExitIdx] = getExitCondition();
      }
      else 
        m_conditions[ancestorExitIdx] = getAncestorExitCondition(); // could be null

      if (getInvariantCondition()) {
        if (getAncestorInvariantCondition()) {
          m_conditions[ancestorInvariantIdx] =
            makeFunction(BooleanAnd::instance(),
                         getInvariantCondition(),
                         getAncestorInvariantCondition(), // from parent
                         false,
                         false);
          m_garbageConditions[ancestorInvariantIdx] = true;
        }
        else
          m_conditions[ancestorInvariantIdx] = getInvariantCondition();
      }
      else 
        m_conditions[ancestorInvariantIdx] = getAncestorInvariantCondition(); // could be null

      // End is special
      if (getEndCondition()) {
        if (getAncestorEndCondition()) {
          m_conditions[ancestorEndIdx] =
            makeFunction(BooleanOr::instance(),
                         getEndCondition(),
                         getAncestorEndCondition(), // from parent
                         false,
                         false);
          m_garbageConditions[ancestorEndIdx] = true;
        }
        else
          m_conditions[ancestorEndIdx] = getEndCondition();
      }
      else {
        // No user-spec'd end condition - build one
        m_conditions[endIdx] =
          new NodeFunction(AllFinished::instance(), this);
        m_garbageConditions[endIdx] = true;
        // *** N.B. ***
        // Normally ancestor-end is our end condition ORed with parent's ancestor-end.
        // But default all-children-finished end condition will always be false
        // when child evaluates ancestor-end.
        // See node state transition diagrams for proof.
        // Since false OR <anything> == <anything>,
        // just use parent's ancestor-end (which may be empty).
        m_conditions[ancestorEndIdx] = getAncestorEndCondition();
      }
    }
    else {
      // No parent - simply reuse existing conditions, if any
      m_conditions[ancestorExitIdx] = m_conditions[exitIdx]; // could be null
      m_conditions[ancestorInvariantIdx] = m_conditions[invariantIdx]; // could be null
      // End is special
      if (m_conditions[endIdx]) {
        // User-spec'd end condition doubles as ancestor-end
        m_conditions[ancestorEndIdx] = m_conditions[endIdx];
      }
      else {
        // No user-spec'd end condition - build one
        m_conditions[endIdx] =           
          new NodeFunction(AllFinished::instance(), this);
        m_garbageConditions[endIdx] = true;
        // *** N.B. ***
        // Normally for root nodes, ancestor-end is same as end. 
        // But default all-children-finished end condition will always be false
        // when child evaluates ancestor-end.
        // See node state transition diagrams for proof.
        // So if no parent and no user end condition, just leave ancestor-end empty.
        m_conditions[ancestorEndIdx] = NULL;
      }
    }
  }

  /**
   * @brief Destructor.  Cleans up this entire part of the node tree.
   */
  ListNode::~ListNode()
  {
    debugMsg("ListNode:~ListNode", " destructor for " << m_nodeId);

    cleanUpConditions();

    cleanUpNodeBody();
  }

  void ListNode::cleanUpConditions()
  {
    if (m_cleanedConditions)
      return;

    debugMsg("ListNode:cleanUpConditions", " for " << m_nodeId);

    cleanUpChildConditions();

    Node::cleanUpConditions();

    for (std::vector<Node *>::iterator it = m_children.begin();
         it != m_children.end();
         ++it)
      (*it)->getStateVariable()->removeListener(this);
  }

  void ListNode::cleanUpNodeBody()
  {
    if (m_cleanedBody)
      return;

    debugMsg("ListNode:cleanUpNodeBody", " for " << m_nodeId);

    // Delete children
    for (std::vector<Node *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
      delete (Node*) (*it);
    m_children.clear();
    m_cleanedBody = true;
  }

  void ListNode::cleanUpChildConditions()
  {
    debugMsg("ListNode:cleanUpChildConditions", " for " << m_nodeId);
    for (std::vector<Node *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
      (*it)->cleanUpConditions();
    for (std::vector<Node *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
      (*it)->cleanUpNodeBody();
  }

  Node const *ListNode::findChild(char const *childName) const
  {
    for (std::vector<Node *>::const_iterator it = m_children.begin();
         it != m_children.end();
         ++it)
      if ((*it)->getNodeId() == childName)
        return *it;
    return NULL;
  }

  Node *ListNode::findChild(char const *childName)
  {
    for (std::vector<Node *>::const_iterator it = m_children.begin();
         it != m_children.end();
         ++it)
      if ((*it)->getNodeId() == childName)
        return *it;
    return NULL;
  }

  void ListNode::reserveChildren(size_t n)
  {
    m_children.reserve(n);
  }

  /**
   * @brief Sets the state variable to the new state.
   * @param newValue The new node state.
   * @note This method notifies the children of a change in the parent node's state.
   */
  void ListNode::setState(NodeState newValue, double tym)
  {
    Node::setState(newValue, tym);
    // Notify the children if the new state is one that they care about.
    switch (newValue) {
    case WAITING_STATE:
      for (std::vector<Node *>::iterator it = m_children.begin();
           it != m_children.end();
           ++it)
        if ((*it)->getState() == FINISHED_STATE)
          (*it)->notifyChanged();
      break;

    case EXECUTING_STATE:
    case FINISHED_STATE:
      for (std::vector<Node *>::iterator it = m_children.begin();
           it != m_children.end();
           ++it)
        if ((*it)->getState() == INACTIVE_STATE)
          (*it)->notifyChanged();
      break;

    default:
      break;
    }
  }

  //////////////////////////////////////
  //
  // Specialized state transition logic
  //
  //////////////////////////////////////

  //
  // EXECUTING
  //
  // Description and methods here are for NodeList and LibraryNodeCall only
  //
  // Legal predecessor states: WAITING
  // Conditions active: AncestorExit, AncestorInvariant, End, Exit, Invariant
  // Legal successor states: FAILING, FINISHING

  void ListNode::transitionToExecuting()
  {
    // From WAITING, AncestorExit, AncestorInvariant, Exit are active
    activateLocalVariables();

    activateInvariantCondition();
    activateEndCondition();

    // These conditions are for the children.
    if (m_conditions[ancestorExitIdx])
      m_conditions[ancestorExitIdx]->activate();
    if (m_conditions[ancestorInvariantIdx])
      m_conditions[ancestorInvariantIdx]->activate();
    if (m_conditions[ancestorEndIdx])
      m_conditions[ancestorEndIdx]->activate();
  }

  bool ListNode::getDestStateFromExecuting()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Ancestor exit for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and ANCESTOR_EXIT_CONDITION true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Exit condition for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and EXIT_CONDITION true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = EXITED;
        return true;
      }
    }

    if ((cond = getAncestorInvariantCondition())) {
      checkError(cond->isActive(),
                 "Ancestor invariant for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and ANCESTOR_INVARIANT_CONDITION false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = PARENT_FAILED;
        return true;
      }
    }

    if ((cond = getInvariantCondition())) {
      checkError(cond->isActive(),
                 "Invariant for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and INVARIANT_CONDITION false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = INVARIANT_CONDITION_FAILED;
        return true;
      }
    }

    if ((cond = getEndCondition()) && (!cond->getValue(temp) || !temp)) {
      checkError(cond->isActive(),
                 "End for " << getNodeId() << " is inactive.");
      debugMsg("Node:getDestState",
               " '" << m_nodeId << "' destination: no state.");
      return false;
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: FINISHING. List node and END_CONDITION true.");
    m_nextState = FINISHING_STATE;
    return true;
  }

  void ListNode::transitionFromExecuting()
  {
    deactivateEndCondition();
    switch (m_nextState) {
      
    case FAILING_STATE:
      deactivateAncestorExitInvariantConditions(); 
      deactivateExitCondition();
      deactivateInvariantCondition();
      // fall through

    case FINISHING_STATE:
      // Both successor states will need this
      activateActionCompleteCondition();
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition NodeList/LibraryNodeCall from EXECUTING to invalid state "
                    << nodeStateName(m_nextState));
      break;
    }
  }

  //
  // FINISHING
  //
  // State is only valid for NodeList and LibraryNodeCall nodes
  //
  // Legal predecessor states: EXECUTING
  // Conditions active: ActionComplete, AncestorExit, AncestorInvariant, Exit, Invariant, Post
  // Legal successor states: FAILING, ITERATION_ENDED

  void ListNode::transitionToFinishing()
  {
    activatePostCondition();
  }

  bool ListNode::getDestStateFromFinishing()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Ancestor exit for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and ANCESTOR_EXIT_CONDITION true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Exit condition for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and EXIT_CONDITION true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = EXITED;
        return true;
      }
    }

    if ((cond = getAncestorInvariantCondition())) {
      checkError(cond->isActive(),
                 "Ancestor invariant for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and ANCESTOR_INVARIANT_CONDITION false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = PARENT_FAILED;
        return true;
      }
    }

    if ((cond = getInvariantCondition())) {
      checkError(cond->isActive(),
                 "Invariant for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. List node and INVARIANT_CONDITION false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = INVARIANT_CONDITION_FAILED;
        return true;
      }
    }

    cond = getActionCompleteCondition();
    checkError(cond->isActive(),
               "Children waiting or finished for " << getNodeId() <<
               " is inactive.");
    cond->getValue(temp); // cannot be unknown, see above
    if (temp) {
      m_nextState = ITERATION_ENDED_STATE;
      debugMsg("Node:getDestState",
               " '" << m_nodeId << "' destination: ITERATION_ENDED. List node " <<
               "and ALL_CHILDREN_WAITING_OR_FINISHED true.");
      if ((cond = getPostCondition()) && (!cond->getValue(temp) || !temp)) {
        checkError(cond->isActive(),
                   "ListNode::getDestStateFromFinishing: Post for " << m_nodeId << " is inactive.");
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = POST_CONDITION_FAILED;
      }
      else
        m_nextOutcome = SUCCESS_OUTCOME;
      return true;
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: no state. ALL_CHILDREN_WAITING_OR_FINISHED false or unknown.");
    return false;
  }

  void ListNode::transitionFromFinishing()
  {
    deactivateExitCondition();
    deactivateInvariantCondition();
    deactivatePostCondition();

    switch (m_nextState) {

    case ITERATION_ENDED_STATE:
      // N.B. These are conditions for the children.
      if (m_conditions[ancestorExitIdx])
        m_conditions[ancestorExitIdx]->deactivate();
      if (m_conditions[ancestorInvariantIdx])
        m_conditions[ancestorInvariantIdx]->deactivate();
      if (m_conditions[ancestorEndIdx])
        m_conditions[ancestorEndIdx]->deactivate();
      // Local conditions
      deactivateActionCompleteCondition();
      deactivateExecutable();
      activateAncestorEndCondition();
      break;

    case FAILING_STATE:
      deactivateAncestorExitInvariantConditions();
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition List node from FINISHING to invalid state "
                    << nodeStateName(m_nextState));
    }
  }

  //
  // FAILING
  //
  // Description and methods here apply only to NodeList and LibraryNodeCall nodes
  //
  // Legal predecessor states: EXECUTING, FINISHING
  // Conditions active: ActionComplete
  // Legal successor states: FINISHED, ITERATION_ENDED

  void ListNode::transitionToFailing()
  {
    // From EXECUTING: ActionComplete active (see transitionFromExecuting() above)
    // From FINISHING: ActionComplete active
  }

  bool ListNode::getDestStateFromFailing()
  {
    Expression *cond = getActionCompleteCondition();
    checkError(cond->isActive(),
               "Children waiting or finished for " << getNodeId() <<
               " is inactive.");

    bool tempb;
    cond->getValue(tempb); // AllWaitingOrFinished is always known
    if (tempb) {
      if (this->getFailureType() == PARENT_EXITED) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. "
                 << "List node, ALL_CHILDREN_WAITING_OR_FINISHED true and parent exited.");
        m_nextState = FINISHED_STATE;
        return true;
      }
      if (this->getFailureType() == PARENT_FAILED) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FINISHED. List node and ALL_CHILDREN_WAITING_OR_FINISHED" <<
                 " true and parent failed.");
        m_nextState = FINISHED_STATE;
        return true;
      }
      else {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: ITERATION_ENDED. List node and "
                 << (this->getFailureType() == EXITED ? "self-exited" : "self-failure."));
        m_nextState = ITERATION_ENDED_STATE;
        return true;
      }
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: no state.");
    return false;
  }

  void ListNode::transitionFromFailing()
  {
    // N.B. These are conditions for the children.
    if (m_conditions[ancestorExitIdx])
      m_conditions[ancestorExitIdx]->deactivate();
    if (m_conditions[ancestorInvariantIdx])
      m_conditions[ancestorInvariantIdx]->deactivate();
    if (m_conditions[ancestorEndIdx])
      m_conditions[ancestorEndIdx]->deactivate();

    deactivateActionCompleteCondition();
    deactivateExecutable();

    switch (m_nextState) {

    case ITERATION_ENDED_STATE:
      activateAncestorExitInvariantConditions();
      activateAncestorEndCondition();
      break;

    case FINISHED_STATE:
      // nothing to do
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition NodeList/LibraryNodeCall node from FAILING to invalid state "
                    << nodeStateName(m_nextState));
      break;
    }
  }


  // ***
  // *** END NODE STATE LOGIC ***
  // ***

  void ListNode::specializedActivate()
  {
    // Activate all children
    for (std::vector<Node *>::iterator it = m_children.begin(); it != m_children.end(); ++it)
      (*it)->activate();
  }

}

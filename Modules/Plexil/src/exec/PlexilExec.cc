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

#include "PlexilExec.hh"

#include "Assignable.hh"
#include "Assignment.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExecListenerBase.hh"
#include "Expression.hh"
#include "ExternalInterface.hh"
#include "Node.hh"
#include "NodeConstants.hh"
#include "NodeFactory.hh"

#include <algorithm> // for find(), transform
#include <iterator> // for back_insert_iterator
#include <functional>
#include <time.h>

namespace PLEXIL 
{

  // Used internally by PlexilExec class only
  enum QueueStatus {
    QUEUE_NONE = 0,          // not in any queue
    QUEUE_CHECK,             // in check-conditions queue
    QUEUE_TRANSITION,        // in state transition queue
    QUEUE_TRANSITION_CHECK,  // in state transition queue AND check-conditions requested
    QUEUE_DELETE             // no longer eligible to transition
  };

  /**
   * @brief Comparator for ordering nodes that are in conflict.  Higher priority wins, but nodes already EXECUTING dominate.
   */
  struct NodeConflictComparator {
    bool operator() (Node const *x, Node const *y) const
    {
      return (x->getPriority() < y->getPriority());
    }
  };

  PlexilExec::PlexilExec()
    : ExecConnector(),
      m_candidateQueue(),
      m_stateChangeQueue(),
      m_finishedRootNodes(),
      m_assignmentsToExecute(),
      m_assignmentsToRetract(),
      m_plan(),
      m_transitionsToPublish(),
      m_variablesToRetract(),
      m_listener(NULL),
      m_resourceConflicts(NULL),
      m_queuePos(0),
      m_finishedRootNodesDeleted(false)
  {}

  PlexilExec::~PlexilExec() 
  {
    // Every node on this list is also in m_plan
    m_finishedRootNodes.clear();
    for (std::list<Node *>::iterator it = m_plan.begin(); it != m_plan.end(); ++it)
      delete (Node*) (*it);

    while (m_resourceConflicts) {
      VariableConflictSet *temp = m_resourceConflicts;
      m_resourceConflicts = m_resourceConflicts->next();
      delete temp;
    }
  }

  /**
   * @brief Get the list of active plans.
   */
  std::list<Node *> const &PlexilExec::getPlans() const
  {
    return m_plan;
  }

  bool PlexilExec::addPlan(Node *root)
  {
    m_plan.push_back(root);
    root->activate();
    debugMsg("PlexilExec:addPlan",
             "Added plan: " << std::endl << root->toString());
    root->notifyChanged(); // redundant?
    return true;
  }

  /**
   * @brief Queries whether all plans are finished.
   * @return true if all finished, false otherwise.
   */

  bool PlexilExec::allPlansFinished() const
  {
    bool result = m_finishedRootNodesDeleted; // return value in the event no plan is active

    for (std::list<Node *>::const_iterator planit = m_plan.begin();
         planit != m_plan.end();
         ++planit)
      {
        Node *root = *planit;
        if (root->getState() == FINISHED_STATE)
          result = true;
        else
          return false; // some node is not finished
      }
    return result;
  }

  void PlexilExec::markRootNodeFinished(Node *node)
  {
    checkError(node,
               "PlexilExec::markRootNodeFinished: node pointer is invalid");
    addFinishedRootNode(node);
  }

  void PlexilExec::deleteFinishedPlans()
  {
    while (!m_finishedRootNodes.empty()) {
      Node *node = m_finishedRootNodes.front();
      m_finishedRootNodes.pop();
      debugMsg("PlexilExec:deleteFinishedPlans",
               " deleting node \"" << node->getNodeId() << "\"");
      // Remove from active plan
      bool found = false;
      for (std::list<Node *>::iterator pit = m_plan.begin();
           pit != m_plan.end();
           ++pit) {
        if (*pit == node) {
          found = true;
          m_plan.erase(pit);
          break;
        }
      }
      assertTrueMsg(found,
                    "PlexilExec::deleteFinishedPlan: Node \"" << node->getNodeId()
                    << "\" not found on active root node list");
      // Now safe to delete
      delete node;
    }
    m_finishedRootNodesDeleted = true;
  }

  void PlexilExec::notifyNodeConditionChanged(Node *node)
  {
    addCandidateNode(node);
  }

  /**
   * @brief Remove node from consideration for state change.
   * @param node The node which is ineligible for state change.
   */
  void PlexilExec::removeNodeFromConsideration(Node *node)
  {
    removeCandidateNode(node);
  }

  bool PlexilExec::needsStep() const
  {
    return !m_candidateQueue.empty();
  }

  //as a possible optimization, if we spend a lot of time searching through this list,
  //it should be faster to search the list backwards.
  void PlexilExec::handleConditionsChanged(Node *node) 
  {
    debugMsg("PlexilExec:handleConditionsChanged",
             "Node " << node->getNodeId() << " had a relevant condition change.");

    debugMsg("PlexilExec:handleConditionsChanged",
             "Considering node '" << node->getNodeId() << "' for state transition.");

    NodeState destState = node->getNextState();
    if (node->getType() == NodeType_Assignment) {
      // Node can be in contention in either EXECUTING or FAILING 
      switch (destState) {

      case EXECUTING_STATE: {
        // add it to contention consideration
        debugMsg("PlexilExec:handleConditionsChanged",
                 "Node '" << node->getNodeId() <<
                 "' is an assignment node that could be executing.  Adding it to the " <<
                 "resource contention list ");
        addToResourceContention(node);
        return;
      }

      case FAILING_STATE: // Is already in conflict set, and must be enqueued now
        debugMsg("PlexilExec:handleConditionsChanged",
                 "Node '" << node->getNodeId() <<
                 "' is an assignment node that is failing, and is already in the " <<
                 "resource contention list");
        m_variablesToRetract.push_back(node->getAssignmentVariable());
        break;

        // In addition to the obvious paths from EXECUTING,
        // the node could have been in WAITING and eligible for execution
        // but deferred to a higher priority node, then failed/exited/skipped
        // before it could execute.
      case ITERATION_ENDED_STATE:
      case FINISHED_STATE:
        switch (node->getState()) {
        case EXECUTING_STATE:
        case FAILING_STATE:
        case WAITING_STATE:
          debugMsg("PlexilExec:handleConditionsChanged",
                   "Node '" << node->getNodeId() <<
                   "' is an assignment node that is no longer possibly executing.  " <<
                   "Removing it from resource contention.");
          removeFromResourceContention(node);
          // fall thru to...
        default:
          break;
        }
        break;

      default: // Is not in contention now, and not entering it either
        break;
      } // end switch (destState)
    }

    addStateChangeNode(node);
    debugMsg("PlexilExec:handleConditionsChanged",
             "Placing node '" << node->getNodeId() <<
             "' on the state change queue in position " << ++m_queuePos);
  }

  /**
   * @brief Schedule this assignment for execution.
   */
  void PlexilExec::enqueueAssignment(Assignment *assign)
  {
    m_assignmentsToExecute.push(assign);
  }

  /**
   * @brief Schedule this assignment for retraction.
   */
  void PlexilExec::enqueueAssignmentForRetraction(Assignment *assign)
  {
    m_assignmentsToRetract.push(assign);
  }

  //
  // Variable conflict sets
  //

  VariableConflictSet *PlexilExec::getConflictSet(Expression *a)
  {
    VariableConflictSet *result = m_resourceConflicts;
    while (result) {
      if (result->getVariable() == a)
        return result;
      result = result->next();
    }
    return NULL;
  }

  VariableConflictSet *PlexilExec::ensureConflictSet(Expression *a)
  {
    VariableConflictSet *result = m_resourceConflicts;
    while (result) {
      if (result->getVariable() == a)
        return result; // found it
      result = result->next();
    }

    // Not found
    result = VariableConflictSet::allocate();
    result->setNext(m_resourceConflicts);
    result->setVariable(a);
    m_resourceConflicts = result;
    return result;
  }

  // Assumes node is a valid ID and points to an Assignment node
  void PlexilExec::removeFromResourceContention(Node *node) 
  {
    Expression *exp = node->getAssignmentVariable();
    assertTrue_1(exp);
    exp = exp->asAssignable()->getBaseVariable();
    assertTrue_1(exp);

    // Remove node from the variable's conflict set.
    VariableConflictSet *conflictNodes = getConflictSet(exp);
    if (!conflictNodes)
      return; // not found

    conflictNodes->remove(node);

    // If deleted node was only one in conflict set,
    // remove variable from conflicts list.
    if (conflictNodes->empty()) {
      if (m_resourceConflicts == conflictNodes)
        // First on list, just point past it
        m_resourceConflicts = m_resourceConflicts->next();
      else {
        // Delete from middle or end
        VariableConflictSet *prev = m_resourceConflicts;
        VariableConflictSet *curr = prev->next();
        while (curr) {
          if (curr == conflictNodes) {
            prev->setNext(curr->next());
            break;
          }
	  curr = curr->next();
        }
        // didn't find it
        assertTrueMsg(curr, "Internal error: Active conflict set not on active list");
      }
      // give it back
      VariableConflictSet::release(conflictNodes);
    }
  }

  // Assumes node is a valid ID and points to an Assignment node whose next state is EXECUTING
  void PlexilExec::addToResourceContention(Node *node)
  {
    Expression *exp = node->getAssignmentVariable();
    assertTrue_1(exp);
    exp = exp->asAssignable()->getBaseVariable();
    assertTrue_1(exp);

    debugMsg("PlexilExec:addToResourceContention",
             "Adding node '" << node->getNodeId() << "' to resource contention.");
    VariableConflictSet *conflict = ensureConflictSet(exp);
    conflict->push(node);
  }

  void PlexilExec::step(double startTime) 
  {
    //
    // *** BEGIN CRITICAL SECTION ***
    //

    // Queue had better be empty when we get here!
    checkError(m_stateChangeQueue.empty(), "State change queue not empty at entry");

    unsigned int stepCount = 0;
#ifndef NO_DEBUG_MESSAGE_SUPPORT
    // Only used in debugMsg calls
    unsigned int cycleNum = g_interface->getCycleCount();
#endif

    debugMsg("PlexilExec:cycle", "==>Start cycle " << cycleNum);

    // BEGIN QUIESCENCE LOOP
    do {
      debugMsg("PlexilExec:step",
               "[" << cycleNum << ":" << stepCount << "] Check queue: "
               << conditionCheckQueueStr());

      // Size of m_candidateQueue is upper bound on queue size
      // Evaluate conditions of nodes reporting a change
      while (!m_candidateQueue.empty()) {
        Node *candidate = getCandidateNode();
        debugMsg("Node:checkConditions",
                 "Checking condition change for node " << candidate->getNodeId());
        if (candidate->getDestState()) { // sets node's next state
          debugMsg("Node:checkConditions",
                   "Can (possibly) transition to " << nodeStateName(candidate->getNextState()));
          handleConditionsChanged(candidate);
        }
      }

      // Sort Assignment nodes by priority
      resolveResourceConflicts();

      if (m_stateChangeQueue.empty())
        break; // nothing to do, exit quiescence loop

      debugMsg("PlexilExec:step",
               "[" << cycleNum << ":" << stepCount << "] State change queue: "
               << stateChangeQueueStr());

      unsigned int microStepCount = 0;

      // Transition the nodes
      m_transitionsToPublish.reserve(m_stateChangeQueue.size());
      while (!m_stateChangeQueue.empty()) {
        Node *node = getStateChangeNode();
        debugMsg("PlexilExec:step",
                 "[" << cycleNum << ":" << stepCount << ":" << microStepCount <<
                 "] Transitioning node " << node->getNodeId()
                 << " from " << nodeStateName(node->getState())
                 << " to " << nodeStateName(node->getNextState()));
        NodeState oldState = node->getState();
        node->transition(startTime); // may put node on m_candidateQueue or m_finishedRootNodes
        m_transitionsToPublish.push_back(NodeTransition(node, oldState));
        ++microStepCount;
      }

      // TODO: instrument high-water-mark of max nodes transitioned in this step

      // Publish the transitions
      // FIXME: Move call to listener outside of quiescence loop
      if (m_listener)
        m_listener->notifyOfTransitions(m_transitionsToPublish);
      m_transitionsToPublish.clear();

      // done with this batch
      ++stepCount;
    }
    while (m_assignmentsToExecute.empty()
           && m_assignmentsToRetract.empty()
           && g_interface->outboundQueueEmpty()
           && !m_candidateQueue.empty());
    // END QUIESCENCE LOOP
    // Perform side effects
    g_interface->incrementCycleCount();
    performAssignments();
    g_interface->executeOutboundQueue();

    debugMsg("PlexilExec:cycle", "==>End cycle " << cycleNum);
    for (std::list<Node *>::const_iterator it = m_plan.begin(); it != m_plan.end(); ++it) {
      debugMsg("PlexilExec:printPlan", std::endl << **it);
    }
    //
    // *** END CRITICAL SECTION ***
    //
  }

  void PlexilExec::performAssignments() 
  {
    debugMsg("PlexilExec:performAssignments",
             " performing " << m_assignmentsToExecute.size() <<  " assignments and "
             << m_assignmentsToRetract.size() << " retractions");
    while (!m_assignmentsToExecute.empty()) {
      Assignment *assn = m_assignmentsToExecute.front();
      m_assignmentsToExecute.pop();
      assn->execute();
    }
    while (!m_assignmentsToRetract.empty()) {
      Assignment *assn = m_assignmentsToRetract.front();
      m_assignmentsToRetract.pop();
      assn->retract();
    }
    m_variablesToRetract.clear();
  }

  void PlexilExec::resolveResourceConflicts()
  {
    VariableConflictSet *c = m_resourceConflicts;
    while (c) {
      resolveVariableConflicts(c);
      c = c->next();
    }
  }

  /**
   * @brief Resolve conflicts for this variable.
   * @note Subroutine of resolveResourceConflicts() above.
   */
  void PlexilExec::resolveVariableConflicts(VariableConflictSet *conflict)
  {
    Expression const *var = conflict->getVariable();
    assertTrue_1(var);
    checkError(!conflict->empty(),
               "Resource conflict set for " << conflict->getVariable()->toString() << " is empty.");

    // Ignore any variables pending retraction
    for (std::vector<Expression *>::const_iterator vit = m_variablesToRetract.begin();
         vit != m_variablesToRetract.end();
         ++vit) {
      if ((*vit)->asAssignable()->getBaseVariable() == var->asAssignable()->getBaseVariable()) { // compare base variables for (e.g.) aliases, array refs
        debugMsg("PlexilExec:resolveResourceConflicts",
                 " Ignoring Assignments for variable '" << var->getName()
                 << "', which has a retraction pending");
        return;
      }
    }

    // Only look at nodes with the highest priority
    Node *nodeToExecute = NULL;
    NodeState destState = NO_NODE_STATE;
    size_t count = conflict->front_count(); // # of nodes with same priority as top
    if (count == 1) {
      // Usual case (we hope) - make it simple
      nodeToExecute = conflict->front();
      destState = nodeToExecute->getNextState();
    }

    else {
      VariableConflictSet::const_iterator conflictIt = conflict->begin(); 
      // Look at the destination states of all the nodes with equal priority
      for (size_t i = 0, conflictCounter = 0; i < count; ++i, ++conflictIt) {
        Node *node = *conflictIt;
        NodeState dest = node->getNextState();

        // Found one that is scheduled for execution
        if (dest == EXECUTING_STATE || dest == FAILING_STATE)
          ++conflictCounter;
        else 
          // Internal error
          checkError(node->getState() == EXECUTING_STATE
                     || node->getState() == FAILING_STATE,
                     "Error: node '" << node->getNodeId()
                     << " is neither executing nor failing nor eligible for either, yet is in conflict map.");

        // If more than one node is scheduled for execution, we have a resource contention.
        // *** FIXME: This is a plan error. Find a non-fatal way to handle this conflict!! ***
        checkError(conflictCounter < 2,
                   "Error: nodes '" << node->getNodeId() << "' and '"
                   << nodeToExecute->getNodeId() << "' are in contention over variable "
                   << var->toString() << " and have equal priority.");

        nodeToExecute = node;
        destState = dest;
      }
      assertTrue_1(nodeToExecute);
    }

    if (destState == EXECUTING_STATE || destState == FAILING_STATE) {
      addStateChangeNode(nodeToExecute);
      debugMsg("PlexilExec:resolveResourceConflicts",
               "Node '" << nodeToExecute->getNodeId()
               << "' has best priority.  Adding it to be executed in position "
               << ++m_queuePos);
    }
    else {
      condDebugMsg(nodeToExecute->getState() == EXECUTING_STATE
                   || nodeToExecute->getState() == FAILING_STATE,
                   "PlexilExec:resolveResourceConflicts",
                   "Node for " << var->getName() << " already executing.  Nothing to resolve.");
    }
  }

  //
  // Internal queue management
  //

  // N.B. A node can be in only one queue at a time.

  void PlexilExec::addCandidateNode(Node *node)
  {
    switch (node->getQueueStatus()) {
    case QUEUE_NONE:
      debugMsg("PlexilExec:notifyNodeConditionChanged", " for node " << node->getNodeId());
      node->setQueueStatus(QUEUE_CHECK);
      m_candidateQueue.push(node);
      return;

    case QUEUE_CHECK:             // already a candidate
    case QUEUE_TRANSITION_CHECK:  // will be a candidate after pending transition
    case QUEUE_DELETE:            // cannot possibly be a candidate, silently ignore
      return;

    case QUEUE_TRANSITION:        // transition pending, defer adding to queue
      node->setQueueStatus(QUEUE_TRANSITION_CHECK);
      return;
    }
  }

  Node *PlexilExec::getCandidateNode() {
    Node *result = m_candidateQueue.front();
    if (!result)
      return NULL;

    m_candidateQueue.pop();
    result->setQueueStatus(QUEUE_NONE);
    return result;
  }

  void PlexilExec::removeCandidateNode(Node *node) {
    m_candidateQueue.remove(node);
    node->setQueueStatus(QUEUE_NONE);
  }

  Node *PlexilExec::getStateChangeNode() {
    Node *result = m_stateChangeQueue.front();
    if (!result)
      return NULL;
    
    QueueStatus was = (QueueStatus) result->getQueueStatus();
    m_stateChangeQueue.pop();
    result->setQueueStatus(QUEUE_NONE);
    if (was == QUEUE_TRANSITION_CHECK)
      addCandidateNode(result);
    return result;
  }

  void PlexilExec::addStateChangeNode(Node *node) {
    switch (node->getQueueStatus()) {
    case QUEUE_NONE:
      node->setQueueStatus(QUEUE_TRANSITION);
      m_stateChangeQueue.push(node);
      return;

    case QUEUE_CHECK:             // shouldn't happen
      assertTrueMsg(ALWAYS_FAIL,
                    "Cannot add node " << node->getNodeId()
                    << " to transition queue, is still in candidate queue");
      return;

    case QUEUE_TRANSITION:        // already in queue, nothing to do
    case QUEUE_TRANSITION_CHECK:  // already in queue, nothing to do
      return;

    case QUEUE_DELETE:            // cannot possibly transition
      assertTrueMsg(ALWAYS_FAIL,
                    "Cannot add node " << node->getNodeId()
                    << " to transition queue, is finished root node pending deletion");
      return;
    }
  }

  Node *PlexilExec::getFinishedRootNode() {
    Node *result = m_finishedRootNodes.front();
    if (!result)
      return NULL;
    
    m_finishedRootNodes.pop();
    result->setQueueStatus(QUEUE_NONE);
    return result;
  }
  
  void PlexilExec::addFinishedRootNode(Node *node) {
    switch (node->getQueueStatus()) {
      
    case QUEUE_CHECK: // seems plausible
      m_candidateQueue.remove(node);
      // fall thru

    case QUEUE_NONE:
      node->setQueueStatus(QUEUE_DELETE);
      m_finishedRootNodes.push(node);
      return;

    case QUEUE_TRANSITION:
    case QUEUE_TRANSITION_CHECK:
      assertTrueMsg(ALWAYS_FAIL,
                    "Root node " << node->getNodeId()
                    << " is eligible for deletion but is still in state transition queue");
      return;

    case QUEUE_DELETE: // shouldn't happen, but harmless
      return;
    }
  }

  std::string PlexilExec::conditionCheckQueueStr() const {
    std::ostringstream retval;
    Node *node = m_candidateQueue.front();
    while (node) {
      retval << node->getNodeId() << " ";
      node = node->next();
    }
    return retval.str();
  }

  std::string PlexilExec::stateChangeQueueStr() const {
    std::ostringstream retval;
    Node *node = m_stateChangeQueue.front();
    while (node) {
      retval << node->getNodeId() << " ";
      node = node->next();
    }
    return retval.str();
  }

}

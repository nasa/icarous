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

#include "AssignmentNode.hh"
#include "Assignment.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExecConnector.hh"
#include "ExpressionConstants.hh"
#include "UserVariable.hh"

namespace PLEXIL
{

  AssignmentNode::AssignmentNode(char const *nodeId, 
                                 Node *parent)
    : Node(nodeId, parent),
      m_assignment(NULL),
      m_priority(WORST_PRIORITY)
  {
  }

  // Used only by module test
  AssignmentNode::AssignmentNode(const std::string &type,
                                 const std::string &name, 
                                 NodeState state,
                                 Node *parent)
    : Node(type, name, state, parent),
      m_assignment(NULL),
      m_priority(WORST_PRIORITY)
  {
    checkError(type == ASSIGNMENT,
               "Invalid node type \"" << type << "\" for an AssignmentNode");

    // Create Assignment object
    createDummyAssignment();

    switch (state) {
    case EXECUTING_STATE:
      m_assignment->activate();
      activateActionCompleteCondition();
      break;

    case FAILING_STATE:
      m_assignment->activate();
      activateAbortCompleteCondition();
      break;

    case FINISHING_STATE:
      checkError(ALWAYS_FAIL, "Invalid state FINISHING for an AssignmentNode");
      break;

    default:
      break;
    }
  }

  AssignmentNode::~AssignmentNode()
  {
    cleanUpConditions();

    if (m_assignment) {
      debugMsg("AssignmentNode:~AssignmentNode", "<" << m_nodeId << "> Removing assignment.");
      delete m_assignment;
      m_assignment = NULL;
    }
  }

  void AssignmentNode::setAssignment(Assignment *assn)
  {
    assertTrue_1(assn);
    m_assignment = assn;

    // Set action-complete condition
    m_conditions[actionCompleteIdx] = m_assignment->getAck();
    m_garbageConditions[actionCompleteIdx] = false;

    // Set abort-complete condition
    m_conditions[abortCompleteIdx] = m_assignment->getAbortComplete();
    m_garbageConditions[abortCompleteIdx] = false;
  }

  // Unit test variant of above
  void AssignmentNode::createDummyAssignment() 
  {
    m_assignment = new Assignment(m_nodeId);
    m_assignment->setVariable(new BooleanVariable(false),
                              true);
    m_assignment->setExpression(TRUE_EXP(), false);
  }

  Expression *AssignmentNode::getAssignmentVariable() const
  {
    return m_assignment->getDest();
  }

  //
  // Transition handlers
  //

  //
  // EXECUTING 
  // 
  // Description and methods here are for Assignment node only
  //
  // Legal predecessor states: WAITING
  // Conditions active: ActionComplete, AncestorExit, AncestorInvariant, End, Exit, Invariant, Post
  // Legal successor states: FAILING, ITERATION_ENDED

  void AssignmentNode::transitionToExecuting()
  {
    Node::transitionToExecuting();
    activateActionCompleteCondition();
  }

  bool AssignmentNode::getDestStateFromExecuting()
  {
    // Not eligible to transition from EXECUTING until the assignment has been executed.
    Expression *cond = getActionCompleteCondition();
    checkError(cond->isActive(),
               "Node::getDestStateFromExecuting: Assignment-complete for " << m_nodeId << " is inactive.");
    bool temp;
    if (!cond->getValue(temp) || !temp) {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << "' destination: no state. Assignment node and assignment-complete false.");
      return false;
    }

    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Ancestor exit for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. Assignment node and ANCESTOR_EXIT_CONDITION true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Exit condition for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. Assignment node and EXIT_CONDITION true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = EXITED;
        return true;
      }
    }

    if ((cond = getAncestorInvariantCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Ancestor invariant for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. Assignment node and Ancestor invariant false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = PARENT_FAILED;
        return true;
      }
    }

    if ((cond = getInvariantCondition())) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: Invariant for " << m_nodeId << " is inactive.");
      if (cond->getValue(temp) && !temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << "' destination: FAILING. Assignment node and Invariant false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = INVARIANT_CONDITION_FAILED;
        return true;
      }
    }

    if ((cond = getEndCondition()) && (!cond->getValue(temp) || !temp)) {
      checkError(cond->isActive(),
                 "Node::getDestStateFromExecuting: End for " << m_nodeId << " is inactive.");
      return false;
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: ITERATION_ENDED. Assignment node and End condition true.");
    m_nextState = ITERATION_ENDED_STATE;
    if ((cond = getPostCondition()) && (!cond->getValue(temp) || !temp)) { 
      checkError(cond->isActive(),
                 "Node::getDestState: Post for " << m_nodeId << " is inactive.");
      m_nextOutcome = FAILURE_OUTCOME;
      m_nextFailureType = POST_CONDITION_FAILED;
    }
    else 
      m_nextOutcome = SUCCESS_OUTCOME;
    return true;
  }

  void AssignmentNode::specializedHandleExecution()
  {
    // Perform assignment
    assertTrueMsg(m_assignment,
                  "AssignmentNode::execute: Assignment is null");
    m_assignment->activate();
    m_assignment->fixValue();
    g_exec->enqueueAssignment(m_assignment);
  }

  void AssignmentNode::transitionFromExecuting()
  {
    deactivateExitCondition();
    deactivateInvariantCondition();
    deactivateEndCondition();
    deactivatePostCondition();
    deactivateActionCompleteCondition();

    switch (m_nextState) {
    case FAILING_STATE:
      deactivateAncestorExitInvariantConditions();
      break;

    case ITERATION_ENDED_STATE:
      activateAncestorEndCondition();
      deactivateExecutable();
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition AssignmentNode from EXECUTING to invalid state"
                    << nodeStateName(m_nextState));
      break;
    }
  }
    
  //
  // FAILING
  //
  // Description and methods here apply only to Assignment nodes
  //
  // Legal predecessor states: EXECUTING
  // Conditions active: AbortComplete
  // Legal successor states: FINISHED, ITERATION_ENDED

  void AssignmentNode::transitionToFailing()
  {
    activateAbortCompleteCondition();
    abort();
  }

  bool AssignmentNode::getDestStateFromFailing()
  {
    Expression *cond = getAbortCompleteCondition();
    checkError(cond->isActive(),
               "Abort complete for " << getNodeId() << " is inactive.");
    bool temp;
    if (!cond->getValue(temp) || !temp) {
      debugMsg("Node:getDestState",
               " '" << m_nodeId
               << "' destination: no state. Assignment node and abort complete false.");
      return false;
    }

    FailureType failureValue = getFailureType();
    if (failureValue == PARENT_FAILED) {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << 
               "' destination: FINISHED.  Assignment node, abort complete, and parent failed.");
      m_nextState = FINISHED_STATE;
      return true;
    }
    else if (failureValue == PARENT_EXITED) {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << 
               "' destination: FINISHED.  Assignment node, abort complete, and parent exited.");
      m_nextState = FINISHED_STATE;
      return true;
    }
    else {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << 
               "' destination: ITERATION_ENDED.  Assignment node and abort complete.");
      m_nextState = ITERATION_ENDED_STATE;
      return true;
    }
  }

  void AssignmentNode::transitionFromFailing()
  {
    deactivateAbortCompleteCondition();
    deactivateExecutable();

    switch (m_nextState) {
    case ITERATION_ENDED_STATE:
      activateAncestorExitInvariantConditions();
      activateAncestorEndCondition();
      break;

    case FINISHED_STATE:
      // nothing else to do
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition Assignment node from FAILING to invalid state "
                    << nodeStateName(m_nextState));
      break;
    }
  }
    
  void AssignmentNode::abort()
  {
    debugMsg("Node:abort", "Aborting node " << m_nodeId);
    g_exec->enqueueAssignmentForRetraction(m_assignment);
  }

  void AssignmentNode::specializedDeactivateExecutable() 
  {
    if (m_assignment)
      m_assignment->deactivate();
  }

}

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

#include "CommandNode.hh"

#include "BooleanOperators.hh"
#include "Command.hh"
#include "Comparisons.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ExecConnector.hh"
#include "ExpressionConstants.hh"
#include "ExternalInterface.hh"
#include "Function.hh"
#include "NodeFunction.hh"
#include "NodeOperatorImpl.hh"

namespace PLEXIL
{
  /**
   * @class CommandHandleKnown
   * @brief A NodeOperator that returns true if the command handle is known, false otherwise.
   */

  class CommandHandleKnown : public NodeOperatorImpl<Boolean>
  {
  public:
    ~CommandHandleKnown()
    {
    }
    
    DECLARE_NODE_OPERATOR_STATIC_INSTANCE(CommandHandleKnown)

    bool checkArgCount(size_t count) const
    {
      return true;
    }

    bool operator()(Boolean &result, Node const *node) const
    {
      result =
        (NO_COMMAND_HANDLE !=
         ((CommandNode const *) node)->getCommand()->getCommandHandle());
      return true;
    }

  private:

    CommandHandleKnown()
      : NodeOperatorImpl<Boolean>("CommandHandleKnown")
    {
    }

    // Disallow copy, assign
    CommandHandleKnown(CommandHandleKnown const &);
    CommandHandleKnown &operator=(CommandHandleKnown const &);
  };

  /**
   * @class CommandHandleInterruptible
   * @brief A NodeOperator that returns true if the command handle is interruptible, false if not.
   */

  class CommandHandleInterruptible : public NodeOperatorImpl<Boolean>
  {
  public:
    ~CommandHandleInterruptible()
    {
    }

    bool checkArgCount(size_t count) const
    {
      return true;
    }

    bool operator()(Boolean &result, Node const *node) const
    {
      switch (((CommandNode const *) node)->getCommand()->getCommandHandle()) {
        // Cases in which node is terminated early
      case COMMAND_DENIED:          // Insufficient resources
      case COMMAND_FAILED:          // Couldn't be sent/performed
      case COMMAND_INTERFACE_ERROR: // Error in interface code
        result = true;
        break;

      case NO_COMMAND_HANDLE:       // Unknown
      default:
        result = false;
        break;
      }
      return true;
    }

    DECLARE_NODE_OPERATOR_STATIC_INSTANCE(CommandHandleInterruptible)

  private:

    CommandHandleInterruptible()
      : NodeOperatorImpl<Boolean>("Interruptible")
    {
    }

    // Not implemented
    CommandHandleInterruptible(const CommandHandleInterruptible &);
    CommandHandleInterruptible &operator=(const CommandHandleInterruptible &);
  };

  CommandNode::CommandNode(char const *nodeId, Node *parent)
    : Node(nodeId, parent),
      m_command(NULL)
  {
  }

  /**
   * @brief Alternate constructor.  Used only by Exec test module.
   */
  CommandNode::CommandNode(const std::string& type,
                           const std::string& name, 
                           NodeState state,
                           Node *parent)
    : Node(type, name, state, parent),
      m_command(NULL)
  {
    // Create dummy command for unit test
    createDummyCommand();

    switch (m_state) {
    case EXECUTING_STATE:
      deactivatePostCondition();
      m_command->activate();
      break;

    case FINISHING_STATE:
      activateAncestorExitInvariantConditions();
      activateExitCondition();
      activateInvariantCondition();
      activatePostCondition();
      activateActionCompleteCondition();
      m_command->activate();
      break;

    case FAILING_STATE:
      activateAbortCompleteCondition();
      m_command->activate();
      break;

    default:
      break;
    }

  }

  /**
   * @brief Destructor.  Cleans up this entire part of the node tree.
   */
  CommandNode::~CommandNode()
  {
    debugMsg("CommandNode:~CommandNode", " destructor for " << m_nodeId);

    // MUST be called first, here. Yes, it's redundant with base class.
    cleanUpConditions();

    cleanUpNodeBody();

    // Delete command last
    if (m_command) {
      debugMsg("CommandNode:~CommandNode", '<' << m_nodeId << "> Removing command.");
      delete m_command;
      m_command = NULL;
    }
  }

  void CommandNode::cleanUpNodeBody()
  {
    if (m_cleanedBody)
      return;

    debugMsg("CommandNode:cleanUpNodeBody", '<' << m_nodeId << "> entered");
    if (m_command) {
      m_command->getAck()->removeListener(this);
      m_command->cleanUp();
    }
    m_cleanedBody = true;
  }

  void CommandNode::setCommand(Command *cmd)
  {
    assertTrue_1(cmd);
    m_command = cmd;

    // Construct action-complete condition
    m_conditions[actionCompleteIdx] =
      new NodeFunction(CommandHandleKnown::instance(), this);
    m_garbageConditions[actionCompleteIdx] = true;

    // Construct command-aborted condition
    m_conditions[abortCompleteIdx] = m_command->getAbortComplete();
    m_garbageConditions[abortCompleteIdx] = false;
  }

  void CommandNode::specializedCreateConditionWrappers()
  {
    // No need to wrap if end condition is default - (True || anything) == True
    if (m_conditions[endIdx] && m_conditions[endIdx] != TRUE_EXP()) {
      // Construct real end condition by wrapping existing
      m_conditions[endIdx] = 
        makeFunction(BooleanOr::instance(),
                     new NodeFunction(CommandHandleInterruptible::instance(), this),
                     m_conditions[endIdx],
                     true,
                     m_garbageConditions[endIdx]);
      m_garbageConditions[endIdx] = true;
    }

    // Add node as listener on command handle variable
    m_command->getAck()->addListener(this);
  }

  //
  // State transition logic
  //

  //
  // EXECUTING 
  // 
  // Legal predecessor states: WAITING
  // Conditions active: AncestorExit, AncestorInvariant, End, Exit, Invariant
  // Legal successor states: FAILING, FINISHING

  void CommandNode::transitionToExecuting()
  {
    activateLocalVariables();

    activateInvariantCondition();
    activateExitCondition();
    activateEndCondition();
  }

  bool CommandNode::getDestStateFromExecuting()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Ancestor exit for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and ancestor exit true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Exit for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and exit true.");
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
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and ancestor invariant false.");
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
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and invariant false.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = INVARIANT_CONDITION_FAILED;
        return true;
      }
    }

    if ((cond = getEndCondition()) && (!cond->getValue(temp) || !temp )) {
      checkError(cond->isActive(),
                 "End for " << getNodeId() << " is inactive.");
      debugMsg("Node:getDestState",
               " '" << m_nodeId << 
               "' destination from EXECUTING: no state.");
      return false;
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << 
             "' destination: FINISHING.  Command node and end condition true.");
    m_nextState = FINISHING_STATE;
    return true;
  }

  void CommandNode::transitionFromExecuting()
  {
    switch (m_nextState) {

    case FAILING_STATE:
      deactivateAncestorExitInvariantConditions();
      deactivateExitCondition();
      deactivateInvariantCondition();
      // fall through

    case FINISHING_STATE:
      deactivateEndCondition();
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition Command node from EXECUTING to invalid state "
                    << nodeStateName(m_nextState));
      break;
    }
  }

  //
  // FINISHING
  //
  // Legal predecessor states: EXECUTING
  // Conditions active: ActionComplete, AncestorExit, AncestorInvariant, Exit, Invariant, Post
  // Legal successor states: FAILING, ITERATION_ENDED

  void CommandNode::transitionToFinishing()
  {
    activatePostCondition();
    activateActionCompleteCondition();
  }

  bool CommandNode::getDestStateFromFinishing()
  {
    Expression *cond;
    bool temp;
    if ((cond = getAncestorExitCondition())) {
      checkError(cond->isActive(),
                 "Ancestor exit for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and ancestor exit true.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = INTERRUPTED_OUTCOME;
        m_nextFailureType = PARENT_EXITED;
        return true;
      }
    }

    if ((cond = getExitCondition())) {
      checkError(cond->isActive(),
                 "Exit for " << getNodeId() << " is inactive.");
      if (cond->getValue(temp) && temp) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and exit true.");
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
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node and ancestor invariant false.");
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
                 " '" << m_nodeId << 
                 "' destination: FAILING. Command node, invariant false and end false or unknown.");
        m_nextState = FAILING_STATE;
        m_nextOutcome = FAILURE_OUTCOME;
        m_nextFailureType = INVARIANT_CONDITION_FAILED;
        return true;
      }
    }

    cond = getActionCompleteCondition();
    checkError(cond->isActive(),
               "Action complete for " << getNodeId() << " is inactive.");
    if (cond->getValue(temp) && temp) {
      debugMsg("Node:getDestState",
               " '" << m_nodeId << 
               "' destination: ITERATION_ENDED.  Command node and action complete true.");
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
      
    debugMsg("Node:getDestState",
             " '" << m_nodeId << 
             "' destination from FINISHING: no state.");
    return false;
  }

  void CommandNode::transitionFromFinishing()
  {
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
                    "Attempting to transition Command node from FINISHING to invalid state "
                    << nodeStateName(m_nextState));
      break;
    }

    deactivateExitCondition();
    deactivateInvariantCondition();
    deactivatePostCondition();
    deactivateActionCompleteCondition();
  }


  //
  // FAILING
  //
  // Legal predecessor states: EXECUTING, FINISHING
  // Conditions active: AbortComplete
  // Legal successor states: FINISHED, ITERATION_ENDED

  void CommandNode::transitionToFailing()
  {
    activateAbortCompleteCondition();
    abort();
  }

  bool CommandNode::getDestStateFromFailing()
  {
    Expression *cond = getAbortCompleteCondition();
    checkError(cond->isActive(),
               "Abort complete for " << getNodeId() << " is inactive.");
    bool temp;
    if (cond->getValue(temp) && temp) {
      if (getFailureType() == PARENT_FAILED) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: FINISHED.  Command node abort complete, " <<
                 "and parent failed.");
        m_nextState = FINISHED_STATE;
        return true;
      }
      else if (getFailureType() == PARENT_EXITED) {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: FINISHED.  Command node abort complete, " <<
                 "and parent exited.");
        m_nextState = FINISHED_STATE;
        return true;
      }
      else {
        debugMsg("Node:getDestState",
                 " '" << m_nodeId << 
                 "' destination: ITERATION_ENDED.  Command node abort complete.");
        m_nextState = ITERATION_ENDED_STATE;
        return true;
      }
    }

    debugMsg("Node:getDestState",
             " '" << m_nodeId << "' destination: no state.");
    return false;
  }

  void CommandNode::transitionFromFailing()
  {
    deactivateAbortCompleteCondition();
    deactivateExecutable();

    switch (m_nextState) {

    case ITERATION_ENDED_STATE:
      activateAncestorExitInvariantConditions();
      activateAncestorEndCondition();
      break;

    case FINISHED_STATE:
      // all done
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
                    "Attempting to transition Command node from FAILING to invalid state "
                    << nodeStateName(m_nextState));
      break;
    }
  }

  void CommandNode::specializedHandleExecution()
  {
    assertTrue_1(m_command);
    m_command->activate();
    m_command->execute();
  }

  void CommandNode::abort()
  {
    assertTrue_1(m_command);
    m_command->abort();
  }

  void CommandNode::specializedDeactivateExecutable()
  {
    assertTrue_1(m_command);
    m_command->deactivate();
  }

  // Unit test utility
  void CommandNode::createDummyCommand() 
  {
    static StringConstant sl_dummyCmdName("dummy");

    // Empty arglist
    // No destination variable
    // No resource
    ResourceList resourceList;
    m_command = new Command(this->getNodeId());
    m_command->setNameExpr(&sl_dummyCmdName, false);
  }

  void CommandNode::printCommandHandle(std::ostream& stream, const unsigned int indent) const
  {
    std::string indentStr(indent, ' ');
    stream << indentStr << " Command handle: " <<
      m_command->getAck()->toString() << '\n';
  }

}

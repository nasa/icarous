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

#ifndef PLEXIL_EXEC_HH
#define PLEXIL_EXEC_HH

#include "ExecConnector.hh"
#include "LinkedQueue.hh"
#include "NodeTransition.hh"
#include "VariableConflictSet.hh"

#include <list>
#include <map>
#include <queue>

namespace PLEXIL 
{
  // Forward references
  class Expression;
  class ExecListenerBase;

  struct NodeConflictComparator;

  /**
   * @brief The core PLEXIL executive.
   */
  class PlexilExec : public ExecConnector
  {
  public:
    /**
     * @brief Default constructor.
     */
    PlexilExec();

    /**
     * @brief Destructor.
     */
    ~PlexilExec();

    //
    // API to ExternalInterface
    //

    /**
     * @brief Prepare the given plan for execution.
     * @param The plan's root node.
     * @return True if succesful, false otherwise.
     */
    bool addPlan(Node *root);

    /**
     * @brief Begins a single "macro step" i.e. the entire quiescence cycle.
     */
    void step(double startTime); // *** FIXME ***

    /**
     * @brief Returns true if the Exec needs to be stepped.
     */
    bool needsStep() const;

    /**
     * @brief Set the ExecListener instance.
     */
    void setExecListener(ExecListenerBase *l)
    {
      m_listener = l;
    }

    /**
     * @brief Get the ExecListener instance.
     * @return The ExecListener. May be NULL.
     */
    ExecListenerBase *getExecListener()
    {
      return m_listener;
    }

    /**
     * @brief Queries whether all plans are finished.
     * @return true if all finished, false otherwise.
     */
    bool allPlansFinished() const;

    /**
     * @brief Deletes any finished root nodes.
     */
    void deleteFinishedPlans();

    //
    // API to Node classes
    //

    /**
     * @brief Schedule this assignment for execution.
     */
    void enqueueAssignment(Assignment *assign);

    /**
     * @brief Schedule this assignment for retraction.
     */
    void enqueueAssignmentForRetraction(Assignment *assign);

    /**
     * @brief Mark node as finished and no longer eligible for execution.
     */
    void markRootNodeFinished(Node *node);

    /**
     * @brief Handle the fact that a node's conditions may have changed (it is eligible for state change).
     * @param node The node which is eligible for state change.
     */
    void notifyNodeConditionChanged(Node *node);

    /**
     * @brief Remove node from consideration for state change.
     * @param node The node which is ineligible for state change.
     */
    void removeNodeFromConsideration(Node *node);

    /**
     * @brief Handle the fact that a node's relevant conditions have changed (it is eligible for state change).
     * @param node The node which is eligible for state change.
     */
    void handleConditionsChanged(Node *node);

    /**
     * @brief Get the list of active plans.
     */
    std::list<Node *> const &getPlans() const;

  private:
    // Not implemented
    PlexilExec(PlexilExec const &);
    PlexilExec &operator=(PlexilExec const &);

    /**
     * @brief Resolve conflicts among potentially executing assignment variables.
     */
    void resolveResourceConflicts();

    /**
     * @brief Resolve conflicts for this variable.
     */
    void resolveVariableConflicts(VariableConflictSet *conflictNodes);

    /**
     * @brief Adds a node to consideration for resource contention.  The node must be an assignment node and it must be eligible to transition to EXECUTING.
     * @param node The assignment node.
     */
    void addToResourceContention(Node *node);

    /**
     * @brief Removes a node from consideration for resource contention.  This is usually because some condition has changed that makes the node no longer
     * eligible for execution.
     * @param node The assignment node.
     */
    void removeFromResourceContention(Node *node);

    VariableConflictSet *getConflictSet(Expression *);

    VariableConflictSet *ensureConflictSet(Expression *);

    //
    // Internal queue management
    //

    void addCandidateNode(Node *node);
    Node *getCandidateNode();
    void removeCandidateNode(Node *node);

    void addStateChangeNode(Node *node);
    Node *getStateChangeNode();

    void addFinishedRootNode(Node *node);
    Node *getFinishedRootNode();

    /**
     * @brief Gets a stringified version of the current check queue.
     */
    std::string conditionCheckQueueStr() const;

    /**
     * @brief Gets a stringified version of the current state change queue.
     */
    std::string stateChangeQueueStr() const;

    /**
     * @brief Batch-perform internal assignments queued up from a quiescence step.
     */
    void performAssignments();

    LinkedQueue<Node> m_candidateQueue;    /*<! Nodes whose conditions have changed and may be eligible to transition. */
    LinkedQueue<Node> m_stateChangeQueue;  /*<! Nodes awaiting state transition.*/
    LinkedQueue<Node> m_finishedRootNodes; /*<! Root nodes which are no longer eligible to execute. */
    LinkedQueue<Assignment> m_assignmentsToExecute;
    LinkedQueue<Assignment> m_assignmentsToRetract;
    std::list<Node *> m_plan; /*<! The root of the plan.*/
    std::vector<NodeTransition> m_transitionsToPublish;
    std::vector<Expression *> m_variablesToRetract; /*<! Set of variables with assignments to be retracted due to node failures */
    ExecListenerBase *m_listener;
    VariableConflictSet *m_resourceConflicts; /*<! Linked list of variable assignment contention sets. */
    unsigned int m_queuePos;
    bool m_finishedRootNodesDeleted; /*<! True if at least one finished plan has been deleted */
  };

}

#endif

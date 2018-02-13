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

#include "Assignable.hh"
#include "Debug.hh"
#include "ExecConnector.hh"
#include "ExpressionConstants.hh" // FALSE_EXP(), TRUE_EXP()
#include "ExternalInterface.hh"
#include "Node.hh"
#include "NodeFactory.hh"
#include "TestSupport.hh"
#include "test/TransitionExternalInterface.hh"
#include "lifecycle-utils.h"

#include <iostream>
#include <map>
#include <cmath>

using namespace PLEXIL;

// For Boolean variable/condition tests
#define IDX_UNKNOWN 0
#define IDX_FALSE 1
#define IDX_TRUE 2

std::list<Node *> const g_dummyPlanList;

class TransitionExecConnector : public ExecConnector
{
public:
  TransitionExecConnector() : ExecConnector() {}
  void notifyNodeConditionChanged(Node * /* node */) {}
  void removeNodeFromConsideration(Node * /* node */) {}
  void handleConditionsChanged(Node * /* node */) {}
  void enqueueAssignment(Assignment * /* assign */) {}
  void enqueueAssignmentForRetraction(Assignment * /* assign */) {}
  void markRootNodeFinished(Node * /* node */) {}
  bool addPlan(Node * /* root */) { return false; }
  void step(double /* startTime */) {}
  bool needsStep() const {return false;}
  void setExecListener(ExecListenerBase * /* l */) {}
  ExecListenerBase *getExecListener() { return NULL; }
  void deleteFinishedPlans() {}
  bool allPlansFinished() const { return true; }
  std::list<Node *> const &getPlans() const { return g_dummyPlanList; }
};

static bool inactiveDestTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;

  std::string types[5] = {ASSIGNMENT,
                          COMMAND,
                          LIBRARYNODECALL,
                          LIST,
                          UPDATE};
  NodeState states[7] = {INACTIVE_STATE,
                         WAITING_STATE,
                         EXECUTING_STATE,
                         FINISHING_STATE,
                         FINISHED_STATE,
                         FAILING_STATE,
                         ITERATION_ENDED_STATE};
  Value values[3] = {Value(), Value(false), Value(true)};

  for (size_t s = 0; s < 7; ++s) {
    Node *parent =
      NodeFactory::createNode(LIST,
                              std::string("testParent"),
                              states[s],
                              NULL);
    for (int i = 0; i < 4; ++i) {
      Node *node = NodeFactory::createNode(types[i],
                                            std::string("inactiveDestTest"),
                                            INACTIVE_STATE,
                                            parent);

      for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
        node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
          for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
            node->getAncestorEndCondition()->asAssignable()->setValue(values[ancestorEnd]);
            node->getDestState();
            NodeState destState = node->getNextState();
            if (destState != node->getState()) {
              debugMsg("UnitTest:inactiveDestTest",
                       " Node " << node->getNodeId()
                       << " Parent state: " << nodeStateName(states[s])
                       << " Dest: " << nodeStateName(destState));
              if (states[s] == FINISHED_STATE) {
                assertTrue_1(destState == FINISHED_STATE);
              }
              else if (states[s] == EXECUTING_STATE) {
                if (ancestorExit == IDX_TRUE
                    || ancestorInvariant == IDX_FALSE
                    || ancestorEnd == IDX_TRUE) {
                  assertTrue_1(destState == FINISHED_STATE);
                }
                else {
                  assertTrue_1(destState == WAITING_STATE);
                }
              }
            }
            else {
              assertTrue_1(destState == NO_NODE_STATE);
            }
          }
        }
      }
      delete (Node*) node;
    }
    delete (Node*) parent;
  }
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool inactiveTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  std::string types[5] = {ASSIGNMENT,
                          COMMAND,
                          LIBRARYNODECALL,
                          LIST,
                          UPDATE};
  NodeState states[7] = {INACTIVE_STATE,
                         WAITING_STATE,
                         EXECUTING_STATE,
                         FINISHING_STATE,
                         FINISHED_STATE,
                         FAILING_STATE,
                         ITERATION_ENDED_STATE};
  Value values[3] = {Value(), Value(false), Value(true)};
    
  for (size_t s = 0; s < 7; ++s) {
    Node *parent =
      NodeFactory::createNode(LIST, 
                              std::string("testParent"), 
                              states[s],
                              NULL);
    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
          for (int i = 0; i < 5; i++) {
            Node *node = NodeFactory::createNode(types[i],
                                                  std::string("inactiveTransTest"), 
                                                  INACTIVE_STATE, 
                                                  parent);
            node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
            node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
            node->getAncestorEndCondition()->asAssignable()->setValue(values[ancestorEnd]);
      
            if (node->getDestState()) {
              node->transition();
              NodeState state = node->getState();
              debugMsg("UnitTest:inactiveTransTest",
                       " Node type " << types[i]
                       << " Parent state " << nodeStateName(states[s])
                       << " Node state " << nodeStateName(state));
              if (states[s] == FINISHED_STATE) {
                assertTrue_1(state == FINISHED_STATE);
                assertTrue_1(node->getOutcome() == SKIPPED_OUTCOME);
              }
              else if (states[s] == EXECUTING_STATE) {
                if (ancestorExit == IDX_TRUE
                    || ancestorInvariant == IDX_FALSE
                    || ancestorEnd == IDX_TRUE) {
                  assertTrue_1(state == FINISHED_STATE);
                  assertTrue_1(node->getOutcome() == SKIPPED_OUTCOME);
                }
                else {
                  assertTrue_1(state == WAITING_STATE);
                  // These are activated by the parent node on transition to EXECUTING.
                  //assertTrue_1(node->getAncestorEndCondition()->isActive());
                  //assertTrue_1(node->getAncestorExitCondition()->isActive());
                  //assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                  assertTrue_1(node->getExitCondition()->isActive());
                  assertTrue_1(node->getPreCondition()->isActive());
                  assertTrue_1(node->getSkipCondition()->isActive());
                  assertTrue_1(node->getStartCondition()->isActive());
                }
              }
              else {
                assertTrue_1(false);
              }
            }
            delete (Node*) node;
          }
        }
      }
    }
    delete (Node*) parent;
  }
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool waitingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("waitingDestTest"), WAITING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int exit = 0; exit < 3; ++exit) {
      node->getExitCondition()->asAssignable()->setValue(values[exit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
        for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
          node->getAncestorEndCondition()->asAssignable()->setValue(values[ancestorEnd]);
          for (int skip = 0; skip < 3; ++skip) {
            node->getSkipCondition()->asAssignable()->setValue(values[skip]);
            for (int start = 0; start < 3; ++start) {
              node->getStartCondition()->asAssignable()->setValue(values[start]);
              for (int pre = 0; pre < 3; ++pre) {
                node->getPreCondition()->asAssignable()->setValue(values[pre]);
                node->getDestState();
                NodeState destState = node->getNextState();
                debugMsg("UnitTest:waitingDestTest",
                         " ancestor exit = " << values[ancestorExit]
                         << " ancestor invariant = " << values[ancestorInvariant]
                         << " ancestor end = " << values[ancestorEnd]
                         << " skip = " << values[skip]
                         << " start = " << values[start] 
                         << " pre = " << values[pre]
                         << "\n destination state is " << nodeStateName(destState));
                if (ancestorExit == IDX_TRUE
                    || exit == IDX_TRUE
                    || ancestorInvariant == IDX_FALSE
                    || ancestorEnd == IDX_TRUE
                    || skip == IDX_TRUE) {
                  assertTrue_1(destState == FINISHED_STATE);
                }
                else if (start == IDX_TRUE) {
                  if (pre == IDX_TRUE) {
                    assertTrue_1(destState == EXECUTING_STATE);
                  }
                  else {
                    assertTrue_1(destState == ITERATION_ENDED_STATE);
                  }
                }
                else {
                  assertTrue_1(destState == NO_NODE_STATE);
                }
              }
            }
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool waitingTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};

  std::string types[4] = {ASSIGNMENT, COMMAND, LIST, UPDATE};
  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int exit = 0; exit < 3; ++exit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
          for (int skip = 0; skip < 3; ++skip) {
            for (int start = 0; start < 3; ++start) {
              for (int pre = 0; pre < 3; ++pre) {
                for (int i = 0; i < 4; i++) {
                  Node *node = NodeFactory::createNode(types[i], std::string("waitingTransTest"), WAITING_STATE, parent);

                  // Parent's condition
                  node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
                  node->getExitCondition()->asAssignable()->setValue(values[exit]);
                  // Parent's condition
                  node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
                  // Parent's condition
                  node->getAncestorEndCondition()->asAssignable()->setValue(values[ancestorEnd]);
                  node->getSkipCondition()->asAssignable()->setValue(values[skip]);
                  node->getStartCondition()->asAssignable()->setValue(values[start]);
                  node->getPreCondition()->asAssignable()->setValue(values[pre]);

                  debugMsg("UnitTest:waitingTransition",
                           "Testing node type " << types[i]
                           << " with ancestor exit = " << values[ancestorExit]
                           << " ancestor invariant = " << values[ancestorInvariant]
                           << " ancestor end = " << values[ancestorEnd]
                           << " skip = " << values[skip]
                           << " start = " << values[start]
                           << " pre = " << values[pre]
                           );

                  if (node->getDestState()) {
                    node->transition();
                    NodeState state = node->getState();

                    if (ancestorExit == IDX_TRUE
                        || exit == IDX_TRUE
                        || ancestorInvariant == IDX_FALSE
                        || ancestorEnd == IDX_TRUE
                        || skip == IDX_TRUE) {
                      assertTrue_1(state == FINISHED_STATE);
                      assertTrue_1(node->getOutcome() == SKIPPED_OUTCOME);
                    }
                    else if (start == IDX_TRUE) {
                      assertTrue_1(node->getAncestorExitCondition()->isActive());
                      assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                      if (pre == IDX_TRUE) {
                        assertTrue_1(state == EXECUTING_STATE);
                        assertTrue_1(node->getEndCondition()->isActive());
                        assertTrue_1(node->getExitCondition()->isActive());
                        assertTrue_1(node->getInvariantCondition()->isActive());
                        if (types[i] == ASSIGNMENT) {
                          assertTrue_1(node->getActionCompleteCondition()->isActive());
                        }
                      }
                      else {
                        assertTrue_1(state == ITERATION_ENDED_STATE);
                        assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                        assertTrue_1(node->getFailureType() == PRE_CONDITION_FAILED);
                        assertTrue_1(node->getRepeatCondition()->isActive());
                        assertTrue_1(node->getAncestorEndCondition()->isActive());
                      }
                    }
                    else {
                      assertTrue_1(false);
                    }
                  }
                  delete (Node*) node;
                }
              }
            }
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool iterationEndedDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("iterationEndedDestTest"), ITERATION_ENDED_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
      node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
      for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
        node->getAncestorEndCondition()->asAssignable()->setValue(values[ancestorEnd]);
        for (int repeat = 0; repeat < 3; ++repeat) {
          node->getRepeatCondition()->asAssignable()->setValue(values[repeat]);
          node->getDestState();
          NodeState destState = node->getNextState();

          if (ancestorExit == IDX_TRUE
              || ancestorInvariant == IDX_FALSE
              || ancestorEnd == IDX_TRUE
              || repeat == IDX_FALSE) {
            assertTrue_1(destState == FINISHED_STATE);
          }
          else if (repeat == IDX_TRUE) {
            assertTrue_1(destState == WAITING_STATE);
          }
          else {
            assertTrue_1(destState == NO_NODE_STATE);
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool iterationEndedTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);

  Value values[3] = {Value(), Value(false), Value(true)};
  std::string types[4] = {ASSIGNMENT, COMMAND, LIST, UPDATE};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
      for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
        for (int repeat = 0; repeat < 3; ++repeat) {
          for (int i = 0; i < 4; i++) {
            Node *node = NodeFactory::createNode(types[i], std::string("iterationEndedTransTest"), ITERATION_ENDED_STATE, parent);
            node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
            node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
            node->getAncestorEndCondition()->asAssignable()->setValue(values[ancestorEnd]);
            node->getRepeatCondition()->asAssignable()->setValue(values[repeat]);

            debugMsg("UnitTest:iterationEndedTransition",
                     "Testing node type " << types[i] << " with "
                     << " ancestor exit = " << values[ancestorExit]
                     << " ancestor invariant = " << values[ancestorInvariant]
                     << " ancestor end = " << values[ancestorEnd]
                     << " repeat = " << values[repeat]
                     );

            if (node->getDestState()) {
              node->transition();
              NodeState state = node->getState();

              // TODO: check to make sure the reset happened here
              if (ancestorExit == IDX_TRUE) {
                assertTrue_1(state == FINISHED_STATE);
                assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                assertTrue_1(node->getFailureType() == PARENT_EXITED);
              }
              else if (ancestorInvariant == IDX_FALSE || ancestorEnd == IDX_TRUE || repeat == IDX_FALSE) {
                assertTrue_1(state == FINISHED_STATE);
                if (ancestorInvariant == IDX_FALSE) {
                  assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                  assertTrue_1(node->getFailureType() == PARENT_FAILED);
                }
              }
              else if (repeat == IDX_TRUE) {
                assertTrue_1(state == WAITING_STATE);
                assertTrue_1(node->getStartCondition()->isActive());
              }
              else {
                assertTrue_1(false);
              }
            }
            delete (Node*) node;
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool finishedDestTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), INACTIVE_STATE, NULL);
  Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("finishedDestTest"), FINISHED_STATE, parent);
  NodeState states[7] = {INACTIVE_STATE,
                         WAITING_STATE,
                         EXECUTING_STATE,
                         FINISHING_STATE,
                         FINISHED_STATE,
                         FAILING_STATE,
                         ITERATION_ENDED_STATE};

  for (size_t s = 0; s < 7; ++s) {
    parent->setState(states[s], tif.currentTime());
    node->getDestState();
    NodeState destState = node->getNextState();
    if (states[s] == WAITING_STATE) {
      assertTrue_1(destState == INACTIVE_STATE);
    }
    else {
      assertTrue_1(destState == NO_NODE_STATE);
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  return true;
}

static bool finishedTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), INACTIVE_STATE, NULL);

  NodeState states[7] = {INACTIVE_STATE,
                         WAITING_STATE,
                         EXECUTING_STATE,
                         FINISHING_STATE,
                         FINISHED_STATE,
                         FAILING_STATE,
                         ITERATION_ENDED_STATE};
  std::string types[4] = {ASSIGNMENT, COMMAND, LIST, UPDATE};

  for (size_t s = 0; s < 7; ++s) {
    for (int i = 0; i < 4; ++i) {
      Node *node = NodeFactory::createNode(types[i], std::string("finishedTransTest"), FINISHED_STATE, parent);
      parent->setState(states[s], tif.currentTime());

      debugMsg("UnitTest:finishedTransition",
               "Testing node type " << types[i]
               << " with parent state = " << nodeStateName(states[s]));

      if (node->getDestState()) {
        node->transition();
        NodeState state = node->getState();

        if (states[s] == WAITING_STATE) {
          assertTrue_1(state == INACTIVE_STATE);
        }
        else {
          assertTrue_1(false);
        }
      }
      delete (Node*) node;
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool listExecutingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(LIST, std::string("listExecutingDestTest"), EXECUTING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int exit = 0; exit < 3; ++exit) {
      node->getExitCondition()->asAssignable()->setValue(values[exit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
        for (int invariant = 0; invariant < 3; ++invariant) {
          node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
          for (int end = 0; end < 3; ++end) {
            node->getEndCondition()->asAssignable()->setValue(values[end]);
            node->getDestState();
            NodeState destState = node->getNextState();

            if (ancestorExit == IDX_TRUE
                || exit == IDX_TRUE
                || ancestorInvariant == IDX_FALSE 
                || invariant == IDX_FALSE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (end == IDX_TRUE) {
              assertTrue_1(destState == FINISHING_STATE);
            }
            else {
              assertTrue_1(destState == NO_NODE_STATE);
            }
          }
        }
      }
    }
  }

  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool listExecutingTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int exit = 0; exit < 3; ++exit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int invariant = 0; invariant < 3; ++invariant) {
          for (int end = 0; end < 3; ++end) {
            Node *node = NodeFactory::createNode(LIST, std::string("listExecutingTransTest"), EXECUTING_STATE, parent);
            node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
            node->getExitCondition()->asAssignable()->setValue(values[exit]);
            node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
            node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
            node->getEndCondition()->asAssignable()->setValue(values[end]);

            debugMsg("UnitTest:listExecutingTransition",
                     "Testing ListNode with"
                     << " ancestor exit = " << values[ancestorExit]
                     << " exit = " << values[exit]
                     << " ancestor invariant = " << values[ancestorInvariant]
                     << " invariant = " << values[invariant]
                     << " end = " << values[end]);

            if (node->getDestState()) {
              node->transition();
              NodeState state = node->getState();
              assertTrue_1(node->getActionCompleteCondition()->isActive());
              if (ancestorExit == IDX_TRUE || exit == IDX_TRUE) {
                assertTrue_1(state == FAILING_STATE);
                assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                if (ancestorExit == IDX_TRUE) {
                  assertTrue_1(node->getFailureType() == PARENT_EXITED);
                }
                else if (exit == IDX_TRUE) {
                  assertTrue_1(node->getFailureType() == EXITED);
                }
              }
              else if (ancestorInvariant == IDX_FALSE || invariant == IDX_FALSE) {
                assertTrue_1(state == FAILING_STATE);
                assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                if (ancestorInvariant == IDX_FALSE) {
                  assertTrue_1(node->getFailureType() == PARENT_FAILED);
                }
                else if (invariant == IDX_FALSE) {
                  assertTrue_1(node->getFailureType() == INVARIANT_CONDITION_FAILED);
                }
              }
              else if (end == IDX_TRUE) {
                assertTrue_1(state == FINISHING_STATE);
                assertTrue_1(node->getInvariantCondition()->isActive());
                assertTrue_1(node->getAncestorInvariantCondition()->isActive());
              }
              else {
                assertTrue_1(false);
              }
            }
            delete (Node*) node;
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool listFailingDestTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(LIST, std::string("listFailingDestTest"), FAILING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  FailureType failureTypes[2] = {PRE_CONDITION_FAILED, PARENT_FAILED};

  for (int children = IDX_FALSE; children < 3; ++children) {
    node->getActionCompleteCondition()->asAssignable()->setValue(values[children]);
    for (int failure = 0; failure < 2; ++failure) {
      node->setNodeFailureType(failureTypes[failure]);
      node->getDestState();
      NodeState destState = node->getNextState();

      if (children == IDX_TRUE) {
        if (failure == 0) {
          assertTrue_1(destState == ITERATION_ENDED_STATE);
        }
        else if (failure == 1) {
          assertTrue_1(destState == FINISHED_STATE);
        }
      }
      else {
        assertTrue_1(destState == NO_NODE_STATE);
      }
    }
  }

  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool listFailingTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent =
    NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);

  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureType[2] = {INVARIANT_CONDITION_FAILED, PARENT_FAILED};

  for (int children = IDX_FALSE; children < 3; ++children) {
    for (int i = 0; i < 2; ++i) {
      Node *node = NodeFactory::createNode(LIST, std::string("listFailingTransTest"), FAILING_STATE, parent);
      node->getActionCompleteCondition()->asAssignable()->setValue(values[children]);
      node->setNodeFailureType(failureType[i]);

      debugMsg("UnitTest:listFailingTrans",
               "Testing with children waiting or finished = " << values[children]
               << " failure type = " << failureType[i]);

      if (node->getDestState()) {
        node->transition();
        NodeState state = node->getState();

        if (children == IDX_TRUE) {
          if (i == 0) {
            assertTrue_1(state == ITERATION_ENDED_STATE);
            assertTrue_1(node->getAncestorInvariantCondition()->isActive());
            assertTrue_1(node->getAncestorEndCondition()->isActive());
            assertTrue_1(node->getRepeatCondition()->isActive());
          }
          else if (i == 1) {
            assertTrue_1(state == FINISHED_STATE);
          }
          else {
            assertTrue_1(false);
          }
        }
        else {
          assertTrue_1(false);
        }
      }
      delete (Node*) node;
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool listFinishingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(LIST, std::string("listFinishingDestTest"), FINISHING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int exit = 0; exit < 3; ++exit) {
      node->getExitCondition()->asAssignable()->setValue(values[exit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
        for (int invariant = 0; invariant < 3; ++invariant) {
          node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
          for (int children = IDX_FALSE; children < 3; ++children) {
            node->getActionCompleteCondition()->asAssignable()->setValue(values[children]);
            for (int post = 0; post < 3; ++post) {
              node->getPostCondition()->asAssignable()->setValue(values[post]);
              node->getDestState();
              NodeState destState = node->getNextState();

              debugMsg("UnitTest:listFinishingDest",
                       "Testing NodeList with"
                       << " ancestor exit = " << values[ancestorExit]
                       << " exit = " << values[exit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " invariant = " << values[invariant]
                       << " children waiting or finished = " << values[children]
                       << " post = " << values[post]
                       << "\n Got dest " << nodeStateName(destState));

              if (ancestorExit == IDX_TRUE
                  || exit == IDX_TRUE
                  || ancestorInvariant == IDX_FALSE
                  || invariant == IDX_FALSE) {
                assertTrue_1(destState == FAILING_STATE);
              }
              else if (children == IDX_TRUE) {
                assertTrue_1(destState == ITERATION_ENDED_STATE);
              }
              else {
                assertTrue_1(destState == NO_NODE_STATE);
              }
            }
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool listFinishingTransTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);

  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int exit = 0; exit < 3; ++exit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int invariant = 0; invariant < 3; ++invariant) {
          for (int children = IDX_FALSE; children < 3; ++children) {
            for (int post = 0; post < 3; ++post) {
              Node *node = NodeFactory::createNode(LIST, std::string("listFinishingTransTest"), FINISHING_STATE, parent);
              node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
              node->getExitCondition()->asAssignable()->setValue(values[exit]);
              node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
              node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
              node->getActionCompleteCondition()->asAssignable()->setValue(values[children]);
              node->getPostCondition()->asAssignable()->setValue(values[post]);

              debugMsg("UnitTest:listFinishingTrans",
                       "Testing NodeList with"
                       << " ancestor exit = " << values[ancestorExit]
                       << " exit = " << values[exit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " invariant = " << values[invariant]
                       << " children waiting or finished = " << values[children]
                       << " post = " << values[post]);

              if (node->getDestState()) {
                node->transition();
                NodeState state = node->getState();

                if (ancestorExit == IDX_TRUE || exit == IDX_TRUE) {
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                  if (ancestorExit == IDX_TRUE) {
                    assertTrue_1(node->getFailureType() == PARENT_EXITED);
                  }
                  else if (exit == IDX_TRUE) {
                    assertTrue_1(node->getFailureType() == EXITED);
                  }
                  assertTrue_1(node->getActionCompleteCondition()->isActive());
                }
                else if (ancestorInvariant == IDX_FALSE || invariant == IDX_FALSE) {
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                  if (ancestorInvariant == IDX_FALSE) {
                    assertTrue_1(node->getFailureType() == PARENT_FAILED);
                  }
                  else if (invariant == IDX_FALSE) {
                    assertTrue_1(node->getFailureType() == INVARIANT_CONDITION_FAILED);
                  }
                  assertTrue_1(node->getActionCompleteCondition()->isActive());
                }
                else if (children == IDX_TRUE) {
                  assertTrue_1(state == ITERATION_ENDED_STATE);
                  assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                  assertTrue_1(node->getAncestorExitCondition()->isActive());
                  assertTrue_1(node->getAncestorEndCondition()->isActive());
                  assertTrue_1(node->getRepeatCondition()->isActive());
                  if (post == IDX_TRUE) {
                    assertTrue_1(node->getOutcome() == SUCCESS_OUTCOME);
                  }
                  else {
                    assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                    assertTrue_1(node->getFailureType() == POST_CONDITION_FAILED);
                  }
                }
                else {
                  assertTrue_1(false);
                }
              }
              delete (Node*) node;
            }
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool bindingExecutingDestTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("bindingExecutingDestTest"), EXECUTING_STATE, parent);
  Value const values[3] = {Value(), Value(false), Value(true)};
  Assignable *actionCompleteVar = node->getActionCompleteCondition()->asAssignable();
  Assignable *ancestorExitVar = node->getAncestorExitCondition()->asAssignable();
  Assignable *exitVar = node->getExitCondition()->asAssignable();
  Assignable *ancestorInvariantVar = node->getAncestorInvariantCondition()->asAssignable();
  Assignable *invariantVar = node->getInvariantCondition()->asAssignable();
  Assignable *endVar = node->getEndCondition()->asAssignable();
  Assignable *postVar = node->getPostCondition()->asAssignable();

  for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
    actionCompleteVar->setValue(values[actionComplete]);
    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      ancestorExitVar->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        exitVar->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          ancestorInvariantVar->setValue(values[ancestorInvariant]);
          for (int invariant = 0; invariant < 3; ++invariant) {
            invariantVar->setValue(values[invariant]);
            for (int end = 0; end < 3; ++end) {
              endVar->setValue(values[end]);
              for (int post = 0; post < 3; ++post) {
                postVar->setValue(values[post]);

                node->getDestState();
                NodeState destState = node->getNextState();
                if (actionComplete != IDX_TRUE) {
                  assertTrue_1(destState == NO_NODE_STATE);
                }
                else if (ancestorExit == IDX_TRUE) {
                  assertTrue_1(destState == FAILING_STATE);
                }
                else if (exit == IDX_TRUE) {
                  assertTrue_1(destState == FAILING_STATE);
                }
                else if (ancestorInvariant == IDX_FALSE) {
                  assertTrue_1(destState == FAILING_STATE);
                }
                else if (invariant == IDX_FALSE) {
                  assertTrue_1(destState == FAILING_STATE);
                }
                else if (end == IDX_TRUE) {
                  assertTrue_1(destState == ITERATION_ENDED_STATE);
                }
                else {
                  assertTrue_1(destState == NO_NODE_STATE);
                }
              }
            }
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool bindingExecutingTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);

  Value values[3] = {Value(), Value(false), Value(true)};
  std::string type = ASSIGNMENT;

  for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int invariant = 0; invariant < 3; ++invariant) {
            for (int end = 0; end < 3; ++end) {
              for (int post = 0; end < 3; ++end) {
                Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("bindingExecutingTransTest"), EXECUTING_STATE, parent);
                node->getActionCompleteCondition()->asAssignable()->setValue(values[actionComplete]);
                node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
                node->getExitCondition()->asAssignable()->setValue(values[exit]);
                node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
                node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
                node->getEndCondition()->asAssignable()->setValue(values[end]);
                node->getPostCondition()->asAssignable()->setValue(values[post]);

                debugMsg("UnitTest:bindingExecutingTransition",
                         "Testing " << ASSIGNMENT << " with"
                         << " action complete = " << values[actionComplete]
                         << " ancestor exit = " << values[ancestorExit]
                         << " exit = " << values[exit]
                         << " ancestor invariant = " << values[ancestorInvariant]
                         << " invariant = " << values[invariant]
                         << " end = " << values[end]
                         << " post = " << values[post]
                         );

                if (node->getDestState()) {
                  node->transition();
                  NodeState state = node->getState();

                  if (ancestorExit == IDX_TRUE || exit == IDX_TRUE) {
                    assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                    if (ancestorExit == IDX_TRUE) {
                      assertTrue_1(node->getFailureType() == PARENT_EXITED);
                      assertTrue_1(state == FAILING_STATE);
                      assertTrue_1(node->getAbortCompleteCondition()->isActive());
                    }
                    else if (exit == IDX_TRUE) {
                      assertTrue_1(node->getFailureType() == EXITED);
                      assertTrue_1(state == FAILING_STATE);
                      assertTrue_1(node->getAbortCompleteCondition()->isActive());
                    }
                  }
                  else if (ancestorInvariant == IDX_FALSE || invariant == IDX_FALSE) {
                    assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                    if (ancestorInvariant == IDX_FALSE) {
                      assertTrue_1(node->getFailureType() == PARENT_FAILED);
                      assertTrue_1(state == FAILING_STATE);
                      assertTrue_1(node->getAbortCompleteCondition()->isActive());
                    }
                    else if (invariant == IDX_FALSE) {
                      assertTrue_1(node->getFailureType() == INVARIANT_CONDITION_FAILED);
                      assertTrue_1(state == FAILING_STATE);
                      assertTrue_1(node->getAbortCompleteCondition()->isActive());
                    }
                  }
                  else if (end == IDX_TRUE) {
                    assertTrue_1(state == ITERATION_ENDED_STATE);
                    assertTrue_1(node->getAncestorEndCondition()->isActive());
                    assertTrue_1(node->getAncestorExitCondition()->isActive());
                    assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                    assertTrue_1(node->getRepeatCondition()->isActive());
                    if (post == IDX_TRUE) {
                      assertTrue_1(node->getOutcome() == SUCCESS_OUTCOME);
                    }
                    else {
                      assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                      assertTrue_1(node->getFailureType() == POST_CONDITION_FAILED);
                    }
                  }
                  else {
                    assertTrue_1(false);
                  }
                }
                delete (Node*) node;
              }
            }
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool bindingFailingDestTest()
{
  TransitionExecConnector con; 
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("bindingFailingDestTest"), FAILING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureType[4] = {INVARIANT_CONDITION_FAILED,
                                PARENT_FAILED,
                                EXITED,
                                PARENT_EXITED};

  for (int abortComplete = 0; abortComplete < 3; ++abortComplete) {
    node->getAbortCompleteCondition()->asAssignable()->setValue(values[abortComplete]);
    for (int failure = 0; failure < 4; ++ failure) {
      node->setNodeFailureType(failureType[failure]);

      node->getDestState();
      NodeState destState = node->getNextState();
      if (abortComplete == IDX_TRUE) {
        if (failureType[failure] == PARENT_FAILED
            || failureType[failure] == PARENT_EXITED) {
          assertTrue_1(destState == FINISHED_STATE);
        }
        else {
          assertTrue_1(destState == ITERATION_ENDED_STATE);
        }
      }
      else {
        assertTrue_1(destState == NO_NODE_STATE);
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool bindingFailingTransTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureType[4] = {INVARIANT_CONDITION_FAILED,
                                PARENT_FAILED,
                                EXITED,
                                PARENT_EXITED};

  for (int children = 0; children < 3; ++children) {
    for (int failure = 0; failure < 2; ++failure) {
      Node *node = NodeFactory::createNode(ASSIGNMENT, std::string("bindingFailingTransTest"), FAILING_STATE, parent);
      node->getActionCompleteCondition()->asAssignable()->setValue(values[children]);
      node->setNodeFailureType(failureType[failure]);

      debugMsg("UnitTest:listFailingTrans",
               "Testing with children waiting or finished = " << values[children]
               << " failure type = " << failureType[failure]);

      if (node->getDestState()) {
        node->transition();
        NodeState state = node->getState();

        if (children == IDX_TRUE) {
          if (failureType[failure] == PARENT_FAILED
              || failureType[failure] == PARENT_EXITED) {
            assertTrue_1(state == FINISHED_STATE);
          }
          else {
            assertTrue_1(state == ITERATION_ENDED_STATE);
            assertTrue_1(node->getAncestorInvariantCondition()->isActive());
            assertTrue_1(node->getAncestorEndCondition()->isActive());
            assertTrue_1(node->getRepeatCondition()->isActive());
          }
        }
        else {
          assertTrue_1(false);
        }
      }
      delete (Node*) node;
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

  //
  // Command nodes
  // 

static bool commandExecutingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(COMMAND, std::string("commandExecutingDestTest"), EXECUTING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int exit = 0; exit < 3; ++exit) {
      node->getExitCondition()->asAssignable()->setValue(values[exit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
        for (int invariant = 0; invariant < 3; ++invariant) {
          node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
          for (int end = 0; end < 3; ++end) {
            node->getEndCondition()->asAssignable()->setValue(values[end]);

            node->getDestState();
            NodeState destState = node->getNextState();
            if (ancestorExit == IDX_TRUE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (exit == IDX_TRUE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (ancestorInvariant == IDX_FALSE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (invariant == IDX_FALSE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (end == IDX_TRUE) {
              assertTrue_1(destState == FINISHING_STATE);
            }
            else {
              assertTrue_1(destState == NO_NODE_STATE);
            }
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool commandExecutingTransTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);

  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int exit = 0; exit < 3; ++exit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int invariant = 0; invariant < 3; ++invariant) {
          for (int end = 0; end < 3; ++end) {
            Node *node = NodeFactory::createNode(COMMAND, std::string("commandExecutingTransTest"), EXECUTING_STATE, parent);
            node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
            node->getExitCondition()->asAssignable()->setValue(values[exit]);
            node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
            node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
            node->getEndCondition()->asAssignable()->setValue(values[end]);
            debugMsg("UnitTest:actionExecutingTransition",
                     "Testing Command with"
                     << " ancestor exit = " << values[ancestorExit]
                     << " exit = " << values[exit]
                     << " ancestor invariant = " << values[ancestorInvariant]
                     << " invariant = " << values[invariant]
                     << " end = " << values[end]
                     );

            if (node->getDestState()) {
              node->transition();
              NodeState state = node->getState();
              if (ancestorExit == IDX_TRUE) {
                assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                assertTrue_1(node->getFailureType() == PARENT_EXITED);
                assertTrue_1(state == FAILING_STATE);
                assertTrue_1(node->getAbortCompleteCondition()->isActive());
              }
              else if (exit == IDX_TRUE) {
                assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                assertTrue_1(node->getFailureType() == EXITED);
                assertTrue_1(state == FAILING_STATE);
                assertTrue_1(node->getAbortCompleteCondition()->isActive());
              }
              else if (ancestorInvariant == IDX_FALSE) {
                assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                assertTrue_1(node->getFailureType() == PARENT_FAILED);
                assertTrue_1(state == FAILING_STATE);
                assertTrue_1(node->getAbortCompleteCondition()->isActive());
              }
              else if (invariant == IDX_FALSE) {
                assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                assertTrue_1(node->getFailureType() == INVARIANT_CONDITION_FAILED);
                assertTrue_1(state == FAILING_STATE);
                assertTrue_1(node->getAbortCompleteCondition()->isActive());
              }
              else if (end == IDX_TRUE) {
                assertTrue_1(state == FINISHING_STATE);
                assertTrue_1(node->getActionCompleteCondition()->isActive());
                assertTrue_1(node->getAncestorExitCondition()->isActive());
                assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                assertTrue_1(node->getExitCondition()->isActive());
                assertTrue_1(node->getInvariantCondition()->isActive());
                assertTrue_1(node->getPostCondition()->isActive());
              }
              else {
                assertTrue_1(false);
              }
            }
            delete (Node*) node;
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool commandFailingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(COMMAND, std::string("commandFailingDestTest"), FAILING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureTypes[4] = {INVARIANT_CONDITION_FAILED,
                                 PARENT_FAILED,
                                 EXITED,
                                 PARENT_EXITED};
  for (int abortComplete = 0; abortComplete < 3; ++abortComplete) {
    node->getAbortCompleteCondition()->asAssignable()->setValue(values[abortComplete]);
    for (int failure = 0; failure < 4; ++failure) {
      node->setNodeFailureType(failureTypes[failure]);
      node->getDestState();
      NodeState destState = node->getNextState();
      if (abortComplete == IDX_TRUE) {
        if (failureTypes[failure] == PARENT_FAILED
            || failureTypes[failure] == PARENT_EXITED) {
          assertTrue_1(destState == FINISHED_STATE);
        }
        else {
          assertTrue_1(destState == ITERATION_ENDED_STATE);
        }
      }
      else {
        assertTrue_1(destState == NO_NODE_STATE);
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool commandFailingTransTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureTypes[4] = {INVARIANT_CONDITION_FAILED,
                                 PARENT_FAILED,
                                 EXITED,
                                 PARENT_EXITED};

  for (int abort = 0; abort < 3; ++abort) {
    for (int failure = 0; failure < 4; ++failure) {
      Node *node = NodeFactory::createNode(COMMAND, std::string("commandFailingTransTest"), FAILING_STATE, parent);
      node->getAbortCompleteCondition()->asAssignable()->setValue(values[abort]);
      node->setNodeFailureType(failureTypes[failure]);

      debugMsg("UnitTest:finishedTransition",
               "Testing Command with"
               << " abort complete = " << values[abort]
               << " failure type = " << failureTypes[failure]);

      if (node->getDestState()) {
        node->transition();
        NodeState state = node->getState();

        if (abort == IDX_TRUE) {
          if (failureTypes[failure] == PARENT_FAILED
              || failureTypes[failure] == PARENT_EXITED) {
            assertTrue_1(state == FINISHED_STATE);
          }
          else {
            assertTrue_1(state == ITERATION_ENDED_STATE);
            assertTrue_1(node->getRepeatCondition()->isActive());
            assertTrue_1(node->getAncestorEndCondition()->isActive());
            assertTrue_1(node->getAncestorExitCondition()->isActive());
            assertTrue_1(node->getAncestorInvariantCondition()->isActive());
          }
        }
        else {
          assertTrue_1(false);
        }
      }
      delete (Node*) node;
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool commandFinishingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(COMMAND, std::string("commandFinishingDestTest"), FINISHING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int exit = 0; exit < 3; ++exit) {
      node->getExitCondition()->asAssignable()->setValue(values[exit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
        for (int invariant = 0; invariant < 3; ++invariant) {
          node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
          for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
            node->getActionCompleteCondition()->asAssignable()->setValue(values[actionComplete]);

            node->getDestState();
            NodeState destState = node->getNextState();
            if (ancestorExit == IDX_TRUE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (exit == IDX_TRUE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (ancestorInvariant == IDX_FALSE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (invariant == IDX_FALSE) {
              assertTrue_1(destState == FAILING_STATE);
            }
            else if (actionComplete == IDX_TRUE) {
              assertTrue_1(destState == ITERATION_ENDED_STATE);
            }
            else {
              assertTrue_1(destState == NO_NODE_STATE);
            }
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool commandFinishingTransTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int exit = 0; exit < 3; ++exit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int invariant = 0; invariant < 3; ++invariant) {
          for (int actionComplete = IDX_FALSE; actionComplete < 3; ++actionComplete) {
            for (int post = 0; post < 3; ++post) {
              Node *node = NodeFactory::createNode(COMMAND, std::string("commandFinishingTransTest"), FINISHING_STATE, parent);
              node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
              node->getExitCondition()->asAssignable()->setValue(values[exit]);
              node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
              node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
              node->getActionCompleteCondition()->asAssignable()->setValue(values[actionComplete]);
              node->getPostCondition()->asAssignable()->setValue(values[post]);
              debugMsg("UnitTest:actionExecutingTransition",
                       "Testing Command with"
                       << " ancestor exit = " << values[ancestorExit]
                       << " exit = " << values[exit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " invariant = " << values[invariant]
                       << " action complete = " << values[actionComplete]
                       << " post = " << values[post]
                       );

              if (node->getDestState()) {
                node->transition();
                NodeState state = node->getState();
                if (ancestorExit == IDX_TRUE) {
                  assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                  assertTrue_1(node->getFailureType() == PARENT_EXITED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getAbortCompleteCondition()->isActive());
                }
                else if (exit == IDX_TRUE) {
                  assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                  assertTrue_1(node->getFailureType() == EXITED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getAbortCompleteCondition()->isActive());
                }
                else if (ancestorInvariant == IDX_FALSE) {
                  assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                  assertTrue_1(node->getFailureType() == PARENT_FAILED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getAbortCompleteCondition()->isActive());
                }
                else if (invariant == IDX_FALSE) {
                  assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                  assertTrue_1(node->getFailureType() == INVARIANT_CONDITION_FAILED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getAbortCompleteCondition()->isActive());
                }
                else if (actionComplete == IDX_TRUE) {
                  assertTrue_1(state == ITERATION_ENDED_STATE);
                  assertTrue_1(node->getAncestorEndCondition()->isActive());
                  assertTrue_1(node->getAncestorExitCondition()->isActive());
                  assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                  assertTrue_1(node->getRepeatCondition()->isActive());
                  if (post == IDX_TRUE) {
                    assertTrue_1(node->getOutcome() == SUCCESS_OUTCOME);
                  }
                  else {
                    assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                    assertTrue_1(node->getFailureType() == POST_CONDITION_FAILED);
                  }
                }
                else {
                  assertTrue_1(false);
                }
              }
              delete (Node*) node;
            }
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

  //
  // Update nodes
  //

static bool updateExecutingDestTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(UPDATE, std::string("updateExecutingDestTest"), EXECUTING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
    for (int exit = 0; exit < 3; ++exit) {
      node->getExitCondition()->asAssignable()->setValue(values[exit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
        for (int invariant = 0; invariant < 3; ++invariant) {
          node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
          for (int end = 0; end < 3; ++end) {
            node->getEndCondition()->asAssignable()->setValue(values[end]);
            for (int post = 0; post < 3; ++post) {
              node->getPostCondition()->asAssignable()->setValue(values[post]);

              node->getDestState();
              NodeState destState = node->getNextState();
              if (ancestorExit == IDX_TRUE) {
                assertTrue_1(destState == FAILING_STATE);
              }
              else if (exit == IDX_TRUE) {
                assertTrue_1(destState == FAILING_STATE);
              }
              else if (ancestorInvariant == IDX_FALSE) {
                assertTrue_1(destState == FAILING_STATE);
              }
              else if (invariant == IDX_FALSE) {
                assertTrue_1(destState == FAILING_STATE);
              }
              else if (end == IDX_TRUE) {
                assertTrue_1(destState == ITERATION_ENDED_STATE);
              }
              else {
                assertTrue_1(destState == NO_NODE_STATE);
              }
            }
          }
        }
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool updateExecutingTransTest()
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};

  for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
    for (int exit = 0; exit < 3; ++exit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int invariant = 0; invariant < 3; ++invariant) {
          for (int end = 0; end < 3; ++end) {
            for (int post = 0; post < 3; ++post) {
              Node *node = NodeFactory::createNode(UPDATE, std::string("updateExecutingTransTest"), EXECUTING_STATE, parent);
              node->getAncestorExitCondition()->asAssignable()->setValue(values[ancestorExit]);
              node->getExitCondition()->asAssignable()->setValue(values[exit]);
              node->getAncestorInvariantCondition()->asAssignable()->setValue(values[ancestorInvariant]);
              node->getInvariantCondition()->asAssignable()->setValue(values[invariant]);
              node->getEndCondition()->asAssignable()->setValue(values[end]);
              node->getPostCondition()->asAssignable()->setValue(values[post]);
              debugMsg("UnitTest:actionExecutingTransition",
                       "Testing Update with"
                       << " ancestor exit = " << values[ancestorExit]
                       << " exit = " << values[exit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " invariant = " << values[invariant]
                       << " end = " << values[end]
                       << " post = " << values[post]
                       );

              if (node->getDestState()) {
                node->transition();
                NodeState state = node->getState();
                if (ancestorExit == IDX_TRUE) {
                  assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                  assertTrue_1(node->getFailureType() == PARENT_EXITED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getActionCompleteCondition()->isActive());
                }
                else if (exit == IDX_TRUE) {
                  assertTrue_1(node->getOutcome() == INTERRUPTED_OUTCOME);
                  assertTrue_1(node->getFailureType() == EXITED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getActionCompleteCondition()->isActive());
                }
                else if (ancestorInvariant == IDX_FALSE) {
                  assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                  assertTrue_1(node->getFailureType() == PARENT_FAILED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getActionCompleteCondition()->isActive());
                }
                else if (invariant == IDX_FALSE) {
                  assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                  assertTrue_1(node->getFailureType() == INVARIANT_CONDITION_FAILED);
                  assertTrue_1(state == FAILING_STATE);
                  assertTrue_1(node->getActionCompleteCondition()->isActive());
                }
                else if (end == IDX_TRUE) {
                  assertTrue_1(state == ITERATION_ENDED_STATE);
                  assertTrue_1(node->getAncestorEndCondition()->isActive());
                  assertTrue_1(node->getAncestorExitCondition()->isActive());
                  assertTrue_1(node->getAncestorInvariantCondition()->isActive());
                  if (post == IDX_TRUE) {
                    assertTrue_1(node->getOutcome() == SUCCESS_OUTCOME);
                  }
                  else {
                    assertTrue_1(node->getOutcome() == FAILURE_OUTCOME);
                    assertTrue_1(node->getFailureType() == POST_CONDITION_FAILED);
                  }
                }
                else {
                  assertTrue_1(false);
                }
              }
              delete (Node*) node;
            }
          }
        }
      }
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool updateFailingDestTest() 
{
  TransitionExecConnector con; 
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Node *node = NodeFactory::createNode(UPDATE, std::string("updateFailingDestTest"), FAILING_STATE, parent);
  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureTypes[4] = {INVARIANT_CONDITION_FAILED,
                                 PARENT_FAILED,
                                 EXITED,
                                 PARENT_EXITED};
  for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
    node->getActionCompleteCondition()->asAssignable()->setValue(values[actionComplete]);
    for (int failure = 0; failure < 4; ++failure) {
      node->setNodeFailureType(failureTypes[failure]);
      node->getDestState();
      NodeState destState = node->getNextState();
      if (actionComplete == IDX_TRUE) {
        if (failureTypes[failure] == PARENT_FAILED
            || failureTypes[failure] == PARENT_EXITED) {
          assertTrue_1(destState == FINISHED_STATE);
        }
        else {
          assertTrue_1(destState == ITERATION_ENDED_STATE);
        }
      }
      else {
        assertTrue_1(destState == NO_NODE_STATE);
      }
    }
  }
  delete (Node*) node;
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

static bool updateFailingTransTest() 
{
  TransitionExecConnector con;
  g_exec = &con;
  TransitionExternalInterface tif;
  g_interface = &tif;
  Node *parent = NodeFactory::createNode(LIST, std::string("testParent"), EXECUTING_STATE, NULL);
  Value values[3] = {Value(), Value(false), Value(true)};
  FailureType failureTypes[4] = {INVARIANT_CONDITION_FAILED,
                                 PARENT_FAILED,
                                 EXITED,
                                 PARENT_EXITED};

  for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
    for (int failure = 0; failure < 4; ++failure) {
      Node *node = NodeFactory::createNode(UPDATE, std::string("updateFailingTransTest"), FAILING_STATE, parent);
      node->getActionCompleteCondition()->asAssignable()->setValue(values[actionComplete]);
      node->setNodeFailureType(failureTypes[failure]);

      debugMsg("UnitTest:finishedTransition",
               "Testing Update with"
               << " action complete = " << values[actionComplete]
               << " failure type = " << failureTypes[failure]);

      if (node->getDestState()) {
        node->transition();
        NodeState state = node->getState();

        if (actionComplete == IDX_TRUE) {
          if (failureTypes[failure] == PARENT_FAILED
              || failureTypes[failure] == PARENT_EXITED) {
            assertTrue_1(state == FINISHED_STATE);
          }
          else {
            assertTrue_1(state == ITERATION_ENDED_STATE);
            assertTrue_1(node->getRepeatCondition()->isActive());
            assertTrue_1(node->getAncestorEndCondition()->isActive());
            assertTrue_1(node->getAncestorExitCondition()->isActive());
            assertTrue_1(node->getAncestorInvariantCondition()->isActive());
          }
        }
        else {
          assertTrue_1(false);
        }
      }
      delete (Node*) node;
    }
  }
  delete (Node*) parent;
  g_exec = NULL;
  g_interface = NULL;
  return true;
}

bool stateTransitionTests() 
{
  runTest(inactiveDestTest);
  runTest(inactiveTransTest);
  runTest(waitingDestTest);
  runTest(waitingTransTest);
  runTest(iterationEndedDestTest);
  runTest(iterationEndedTransTest);
  runTest(finishedDestTest);
  runTest(finishedTransTest);
  runTest(listExecutingDestTest);
  runTest(listExecutingTransTest);
  runTest(listFailingDestTest);
  runTest(listFailingTransTest);
  runTest(listFinishingDestTest);
  runTest(listFinishingTransTest);
  runTest(bindingExecutingDestTest);
  runTest(bindingExecutingTransTest);
  runTest(bindingFailingDestTest);
  runTest(bindingFailingTransTest);
  runTest(commandExecutingDestTest);
  runTest(commandExecutingTransTest);
  runTest(commandFailingDestTest);
  runTest(commandFailingTransTest);
  runTest(commandFinishingDestTest);
  runTest(commandFinishingTransTest);
  runTest(updateExecutingDestTest);
  runTest(updateExecutingTransTest);
  runTest(updateFailingDestTest);
  runTest(updateFailingTransTest);
  return true;
}

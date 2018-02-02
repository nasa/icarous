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

#include "Launcher.hh"

#include "AdapterConfiguration.hh"
#include "AdapterFactory.hh"
#include "Array.hh"
#include "Command.hh"
#include "Error.hh"
#include "ExecConnector.hh"
#include "ExecListener.hh"
#include "InterfaceAdapter.hh"
#include "InterfaceManager.hh"
#include "Node.hh"
#include "State.hh"
#include "StateCacheEntry.hh"

#include <algorithm>

namespace PLEXIL
{

  /**
   * @class LauncherListener
   * @brief Helper class to allow plans to monitor other plans
   */
  
  class LauncherListener : public ExecListener
  {
  public:
    LauncherListener()
      : ExecListener()
    {
    }

    ~LauncherListener()
    {
    }

    /**
     * @brief Notify that a node has changed state.
     * @param prevState The old state.
     * @param node The node that has transitioned.
     * @note The current state is accessible via the node.
     */
    virtual void implementNotifyNodeTransition(NodeState /* prevState */,
                                               Node * node) const
    {
      if (node->getParent())
        return;
      Value const nodeIdValue(node->getNodeId());
      g_manager->handleValueChange(State(PLAN_STATE_STATE, nodeIdValue),
                                   Value(nodeStateName(node->getState())));
      NodeOutcome o = node->getOutcome();
      if (o != NO_OUTCOME) {
        g_manager->handleValueChange(State(PLAN_OUTCOME_STATE, nodeIdValue),
                                     Value(outcomeName(o)));
        FailureType f = node->getFailureType();
        if (f != NO_FAILURE)
          g_manager->handleValueChange(State(PLAN_FAILURE_TYPE_STATE, nodeIdValue),
                                       Value(failureTypeName(f)));
      }
    }
  };

  static void valueToExprXml(pugi::xml_node parent, Value const &v)
  {
    ValueType vt = v.valueType();
    if (isArrayType(vt)) {
      pugi::xml_node aryxml = parent.append_child("ArrayValue");
      char const *eltType = typeNameAsValue(arrayElementType(vt)).c_str();
      aryxml.append_attribute("Type").set_value(eltType);
      Array const *ary;
      v.getValuePointer(ary); // better succeed!
      for (size_t i = 0; i < ary->size(); ++i) {
        // quick & dirty
        aryxml.append_child(eltType).append_child(pugi::node_pcdata).set_value(ary->getElementValue(i).valueToString().c_str());
      }
    }
    else {
      // Scalar value
      parent.append_child(typeNameAsValue(vt).c_str()).append_child(pugi::node_pcdata).set_value(v.valueToString().c_str());
    }
  }

  static unsigned int nextSerialNumber()
  {
    static unsigned int sl_next = 0;
    return ++sl_next;
  }

  static void executeStartPlanCommand(Command *cmd)
  {
    std::vector<Value> const &args = cmd->getArgValues();
    size_t nargs = args.size();
    std::vector<std::string> formals;
    std::vector<Value> actuals;

    for (size_t i = 1; i < nargs; i += 2) {
      if (i + 1 >= nargs) {
        warn("Arguments to " << cmd->getName() << " command not in name-value pairs");
        g_manager->handleCommandAck(cmd, COMMAND_FAILED);
	g_manager->notifyOfExternalEvent();
        return;
      }

      if (args[i].valueType() != STRING_TYPE) {
        warn("StartPlan command argument " << i << " is not a String");
        g_manager->handleCommandAck(cmd, COMMAND_FAILED);
	g_manager->notifyOfExternalEvent();
        return;
      }
      std::string const *formal = NULL;
      if (!args[i].getValuePointer(formal)) {
        warn("StartPlan command argument " << i << " is UNKNOWN");
        g_manager->handleCommandAck(cmd, COMMAND_FAILED);
	g_manager->notifyOfExternalEvent();
        return;
      }
      formals.push_back(*formal);

      if (!args[i + 1].isKnown()) {
	warn("StartPlan command argument " << i + 1 << " is UNKNOWN");
        g_manager->handleCommandAck(cmd, COMMAND_FAILED);
	g_manager->notifyOfExternalEvent();
        return;
      }
      actuals.push_back(args[i + 1]);
    }
    
    // Construct XML for LibraryNodeCall node
    std::string const *nodeName = NULL;
    args[0].getValuePointer(nodeName);
    std::ostringstream s;
    s << *nodeName << '_' << nextSerialNumber();
    std::string callerId = s.str();
    
    pugi::xml_document doc;
    pugi::xml_node plan = doc.append_child("PlexilPlan");
    pugi::xml_node rootNode = plan.append_child("Node");
    rootNode.append_attribute("NodeType").set_value("LibraryNodeCall");
    rootNode.append_child("NodeId").append_child(pugi::node_pcdata).set_value(callerId.c_str());

    // Construct ExitCondition
    pugi::xml_node exitLookup = rootNode.append_child("ExitCondition").append_child("LookupNow");
    exitLookup.append_child("Name").append_child("StringValue").append_child(pugi::node_pcdata).set_value(EXIT_PLAN_CMD);
    exitLookup.append_child("Arguments").append_child("StringValue").append_child(pugi::node_pcdata).set_value(callerId.c_str());

    pugi::xml_node call = rootNode.append_child("NodeBody").append_child("LibraryNodeCall");
    call.append_child("NodeId").append_child(pugi::node_pcdata).set_value(nodeName->c_str());
    for (size_t i = 0; i < formals.size(); ++i) {
      pugi::xml_node alias = call.append_child("Alias");
      alias.append_child("NodeParameter").append_child(pugi::node_pcdata).set_value(formals[i].c_str());
      valueToExprXml(alias, actuals[i]);
    }
    
    try {
      g_manager->handleAddPlan(plan);
    }
    catch (ParserException &e) {
      warn("Launching plan " << nodeName << " failed:\n"
           << e.what());
      g_manager->handleCommandAck(cmd, COMMAND_FAILED);
      g_manager->notifyOfExternalEvent();
      return;
    }
    g_manager->handleCommandReturn(cmd, Value(callerId));
    g_manager->handleCommandAck(cmd, COMMAND_SUCCESS);
    g_manager->notifyOfExternalEvent();
  }

  // Helper class
  class NodeNameEquals
  {
  public:
    NodeNameEquals(std::string const &s)
      : m_string(s)
    {
    }

    NodeNameEquals(NodeNameEquals const &other)
      : m_string(other.m_string)
    {
    }

    ~NodeNameEquals()
    {
    }

    NodeNameEquals &operator=(NodeNameEquals const &other)
    {
      m_string = other.m_string;
      return *this;
    }

    bool operator()(Node const *n)
    {
      return n->getNodeId() == m_string;
    }

  private:
    std::string m_string;
  };

  static Node *findNode(std::string const &nodeName)
  {
    // Find the named node
    NodeNameEquals pred(nodeName);
    std::list<Node *> const &allNodes = g_exec->getPlans();
    std::list<Node *>::const_iterator it =
      std::find_if(allNodes.begin(), allNodes.end(), pred);
    if (it == allNodes.end()) {
      warn("No such node"); // FIXME
      return NULL;
    }
    Node *result = *it;
    if (allNodes.end() !=
        std::find_if(++it, allNodes.end(), pred)) {
      warn("Multiple nodes named " << nodeName); // FIXME
      return NULL;
    }
    return result;
  }

  class Launcher : public InterfaceAdapter
  {
  public:
    Launcher(AdapterExecInterface& execInterface)
      : InterfaceAdapter(execInterface)
    {
    }
  
    Launcher(AdapterExecInterface& execInterface, 
             pugi::xml_node const xml)
      : InterfaceAdapter(execInterface, xml)
    {
      g_configuration->addExecListener(new LauncherListener());
    }

    ~Launcher()
    {
    }

    bool initialize()
    {
      return true;
    }

    bool start()
    {
      registerAdapter();
      return true;
    }

    bool stop()
    {
      return true;
    }

    bool reset()
    {
      return true;
    }

    bool shutdown()
    {
      return true;
    }
    
    void lookupNow(State const &state, StateCacheEntry &cacheEntry)
    {
    }

    void subscribe(const State& state)
    {
      // TODO
    }

    void unsubscribe(const State& state)
    {
      // TODO
    }

    void executeCommand(Command *cmd)
    {
      // All commands require at least one arg, the first must be a String
      std::vector<Value> const &args = cmd->getArgValues();
      if (args.size() < 1) {
        warn("Not enough parameters to " << cmd->getName() << " command");
        g_manager->handleCommandAck(cmd, COMMAND_FAILED);
      }
      if (args[0].valueType() != STRING_TYPE) {
        warn("First argument to " << cmd->getName() << " command is not a string");
        g_manager->handleCommandAck(cmd, COMMAND_FAILED);
      }
      else {
        std::string const *nodeName = NULL;
        if (!args[0].getValuePointer(nodeName)) {
          warn("Node name parameter value to " << cmd->getName() << " command is UNKNOWN");
          g_manager->handleCommandAck(cmd, COMMAND_FAILED);
        }
        else {
          std::string const &name = cmd->getName();
          if (name == START_PLAN_CMD) {
            executeStartPlanCommand(cmd);
          }
          else if (name == EXIT_PLAN_CMD) {
            if (args.size() > 1) {
              warn("Too many parameters to " << name << " command");
              g_manager->handleCommandAck(cmd, COMMAND_FAILED);
            }
            Node *node = findNode(*nodeName);
            if (!node) {
              // Not found or multiples with same name
              g_manager->handleCommandAck(cmd, COMMAND_FAILED);
            }
            else {
              g_manager->handleValueChange(State(EXIT_PLAN_CMD, args[0]),
                                           Value(true));
              g_manager->handleCommandAck(cmd, COMMAND_SUCCESS);
            }
          }
          else {
            warn("Launcher adapter: Unimplemented command \"" << name << '"');
            g_manager->handleCommandAck(cmd, COMMAND_FAILED);
          }
        }
      }
      g_manager->notifyOfExternalEvent();
    }

    void invokeAbort(Command *cmd)
    {
      // not implemented
      g_manager->handleCommandAbortAck(cmd, false);
      g_manager->notifyOfExternalEvent();
    }

    void registerAdapter()
    {
      g_configuration->registerCommandInterface(START_PLAN_CMD, this);
      g_configuration->registerCommandInterface(EXIT_PLAN_CMD, this);
      g_configuration->registerLookupInterface(EXIT_PLAN_CMD, this, true);
      g_configuration->registerLookupInterface(PLAN_STATE_STATE, this, true);
      g_configuration->registerLookupInterface(PLAN_OUTCOME_STATE, this, true);
      g_configuration->registerLookupInterface(PLAN_FAILURE_TYPE_STATE, this, true);
    }
  };
  
}

extern "C"
void initLauncher()
{
  REGISTER_ADAPTER(PLEXIL::Launcher, "Launcher");
}

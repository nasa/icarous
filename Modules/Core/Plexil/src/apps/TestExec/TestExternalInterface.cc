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

#include "TestExternalInterface.hh"

#include "Command.hh"
#include "Debug.hh"
#include "Error.hh"
#include "Node.hh"
#include "NodeConstants.hh"
#include "PlexilExec.hh"
#include "StateCacheEntry.hh"
#include "Update.hh"
#include "parsePlan.hh"
#include "plan-utils.hh"
#include "pugixml.hpp"
#include "stricmp.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>

namespace PLEXIL
{

  // Forward declarations for local functions
  static std::string getText(const State& c);
  static std::string getText(const State& c, const Value& v);
  static State parseCommand(pugi::xml_node const cmd);
  static Value parseOneValue(const std::string& type,
                             const std::string& valStr);
  static Value parseParam(pugi::xml_node const param);
  static void parseParams(pugi::xml_node const root,
                          std::vector<Value>& dest);
  static Value parseResult(pugi::xml_node const valXml);
  static State parseState(pugi::xml_node const elt);
  static Value parseStateValue(pugi::xml_node const stateXml);


  TestExternalInterface::TestExternalInterface()
    : ExternalInterface()
  {
    // Set a default time of 0
    m_states.insert(std::pair<State, Value>(State::timeState(), Value(0.0)));
  }

  TestExternalInterface::~TestExternalInterface()
  {
  }

  void TestExternalInterface::run(pugi::xml_node const input)
    throw(ParserException)
  {
    checkError(g_exec, "Attempted to run a script without an executive.");

    handleInitialState(input); // steps once
    
    pugi::xml_node script = input.child("Script");
    checkError(!script.empty(), "No Script element in Plexilscript.");
    pugi::xml_node scriptElement = script.first_child();
    while (!scriptElement.empty()) {
      // ignore text element (e.g. from <Script> </Script>)
      if (scriptElement.type() == pugi::node_pcdata) {
        // debugMsg("Test:verboseTestOutput", " Ignoring XML PCDATA");
      }

      // state
      else if (strcmp(scriptElement.name(), "State") == 0) {
        handleState(scriptElement);
      }

      // command
      else if (strcmp(scriptElement.name(), "Command") == 0) {
        handleCommand(scriptElement);
      }

      // command ack
      else if (strcmp(scriptElement.name(), "CommandAck") == 0) {
        handleCommandAck(scriptElement);
      }
         
      // command abort
      else if (strcmp(scriptElement.name(), "CommandAbort") == 0) {
        handleCommandAbort(scriptElement);
      }

      // update ack
      else if (strcmp(scriptElement.name(), "UpdateAck") == 0) {
        handleUpdateAck(scriptElement);
      }

      // send plan
      else if (strcmp(scriptElement.name(), "SendPlan") == 0) {
        handleSendPlan(scriptElement);
      }

      // simultaneous
      else if (strcmp(scriptElement.name(), "Simultaneous") == 0) {
        handleSimultaneous(scriptElement);
      }

      // delay
      else if (strcmp(scriptElement.name(), "Delay") == 0) {
        ; // No-op
      }

      // report unknown script element
      else {
        checkError(ALWAYS_FAIL, "Unknown script element '" << scriptElement.name() << "'");
        return;
      }
         
      // step the exec forward
      if (true /* g_exec->processQueue() */ ) // *** FIXME ***
        g_exec->step(currentTime());

      scriptElement = scriptElement.next_sibling();
    }
    // Script is complete
    // Continue stepping the Exec til quiescent
    while (g_exec->needsStep()) {
      g_exec->step(currentTime());
    }
  }

  void TestExternalInterface::handleInitialState(pugi::xml_node const input)
  {
    pugi::xml_node initialState = input.child("InitialState");
    if (initialState) {
      pugi::xml_node state = initialState.first_child();
      // Deal with <InitialState>  </InitialState>
      if (state.type() != pugi::node_pcdata) {
        while (state) {
          State st = parseState(state);
          Value value = parseStateValue(state);
          debugMsg("Test:testOutput",
                   "Creating initial state " << getText(st, value));
          m_states[st] = value;
          this->lookupReturn(st, value);
          state = state.next_sibling();
        }
      }
    }
    g_exec->step(currentTime());
  }

  void TestExternalInterface::handleState(pugi::xml_node const elt)
  {
    State st = parseState(elt);
    Value value = parseStateValue(elt);
    debugMsg("Test:testOutput",
             "Processing event: " << st << " = " << value);
    m_states[st] = value;
    this->lookupReturn(st, value);
  }

  void TestExternalInterface::handleCommand(pugi::xml_node const elt)
  {
    State command = parseCommand(elt);
    Value value = parseResult(elt);
    debugMsg("Test:testOutput",
             "Sending command result " << getText(command, value));
    StateCommandMap::iterator it = 
      m_executingCommands.find(command);
    checkError(it != m_executingCommands.end(),
               "No currently executing command " << getText(command));
    this->commandReturn(it->second, value);
    m_executingCommands.erase(it);
  }

  void TestExternalInterface::handleCommandAck(pugi::xml_node const elt)
  {
    State command = parseCommand(elt);
    // Ack should be string value
    Value value = parseResult(elt);
    CommandHandleValue handle = NO_COMMAND_HANDLE;
    std::string const *str = NULL;
    if (value.getValuePointer(str))
      handle = parseCommandHandleValue(*str);
    debugMsg("Test:testOutput",
             "Sending command ACK " << getText(command, value));
    StateCommandMap::iterator it = m_commandAcks.find(command);
    assertTrueMsg(it != m_commandAcks.end(), 
                  "No command waiting for acknowledgement " << getText(command));

    this->commandHandleReturn(it->second, handle);
  }

  void TestExternalInterface::handleCommandAbort(pugi::xml_node const elt)
  {
    State command = parseCommand(elt);
    Value value = parseResult(elt);
    debugMsg("Test:testOutput",
             "Sending abort ACK " << getText(command, value));
    StateCommandMap::iterator it = 
      m_abortingCommands.find(command);
    assertTrueMsg(it != m_abortingCommands.end(), 
                  "No abort waiting for acknowledgement " << getText(command));
    debugMsg("Test:testOutput",
             "Acknowledging abort into " << it->second);
    this->commandAbortAcknowledge(it->second, true);
    m_abortingCommands.erase(it);
  }

  void TestExternalInterface::handleUpdateAck(pugi::xml_node const elt)
  {
    std::string name(elt.attribute("name").value());
    debugMsg("Test:testOutput", "Sending update ACK " << name);
    std::map<std::string, Update*>::iterator it = m_waitingUpdates.find(name);
    checkError(it != m_waitingUpdates.end(),
               "No update from node " << name << " waiting for acknowledgement.");
    this->acknowledgeUpdate(it->second, true);
    m_waitingUpdates.erase(it);
  }

  void TestExternalInterface::handleSendPlan(pugi::xml_node const elt)
  {
    const char* filename = elt.attribute("file").value();
    checkError(strlen(filename) > 0,
               "SendPlan element has no file attribute");

    pugi::xml_document* doc = new pugi::xml_document();
    pugi::xml_parse_result parseResult = doc->load_file(filename);
    assertTrueMsg(parseResult.status == pugi::status_ok, 
                  "Error parsing plan file " << elt.attribute("file").value()
                  << ": " << parseResult.description());

    debugMsg("Test:testOutput",
             "Sending plan from file " << elt.attribute("file").value());
    Node *root = NULL;
    try {
      root = parsePlan(doc->document_element().child("PlexilPlan"));
    }
    catch (ParserException const &e) {
      std::cerr << "Error parsing plan XML: \n" << e.what() << std::endl;
    }
    if (root)
      g_exec->addPlan(root);
  }

  void TestExternalInterface::handleSimultaneous(pugi::xml_node const elt)
  {
    debugMsg("Test:testOutput", "Processing simultaneous event(s)");
    pugi::xml_node item = elt.first_child();
    while (!item.empty()) {
      // ignore text element (e.g. from <Script> </Script>)
      if (item.type() == pugi::node_pcdata) {
        //debugMsg("Test:verboseTestOutput", " Ignoring XML PCDATA");
      }
      // state
      else if (strcmp(item.name(), "State") == 0) {
        handleState(item);
      }
      // command
      else if (strcmp(item.name(), "Command") == 0) {
        handleCommand(item);
      }
      // command ack
      else if (strcmp(item.name(), "CommandAck") == 0) {
        handleCommandAck(item);
      }
      // command abort
      else if (strcmp(item.name(), "CommandAbort") == 0) {
        handleCommandAbort(item);
      }
      // update ack
      else if (strcmp(item.name(), "UpdateAck") == 0) {
        handleUpdateAck(item);
      }
      // report unknown script element
      else {
        checkError(ALWAYS_FAIL,
                   "Unknown script element '" << item.name() << "' inside <Simultaneous>");
        return;
      }
      item = item.next_sibling();
    }
    debugMsg("Test:testOutput", "End simultaneous event(s)");
  }

  //
  // Script parsing utilities
  //

  static State parseStateInternal(pugi::xml_node const elt)
  {
    checkError(!elt.attribute("name").empty(),
               "No name attribute in " << elt.name() << " element.");
    State result(elt.attribute("name").value());
    std::vector<Value> parms;
    parseParams(elt, parms);
    size_t n = parms.size();
    if (n) {
      result.setParameterCount(n);
      for (size_t i = 0; i < n; ++i)
        result.setParameter(i, parms[i]);
    }
    return result;
  }

  static State parseState(pugi::xml_node const elt)
  {
    checkError(strcmp(elt.name(), "State") == 0,
               "Expected <State> element. Found '" << elt.name() << "'");
    return parseStateInternal(elt);
  }

  // Parses all command-like elements: Command, CommandAck, CommandAbort.
  static State parseCommand(pugi::xml_node const cmd)
  {
    checkError(strcmp(cmd.name(), "Command") == 0 ||
               strcmp(cmd.name(), "CommandAck") == 0 ||
               strcmp(cmd.name(), "CommandAbort") == 0,
               "Expected <Command> element.  Found '" << cmd.name() << "'");
    return parseStateInternal(cmd);
  }

  static Value parseResult(pugi::xml_node const cmd)
  {
    pugi::xml_node resXml = cmd.child("Result");
    checkError(!resXml.empty(), "No Result child in <" << cmd.name() << "> element.");
    checkError(!resXml.first_child().empty(), "Empty Result child in <" << cmd.name() << "> element.");
    checkError(!cmd.attribute("type").empty(),
               "No type attribute in <" << cmd.name() << "> element.");
    std::string type(cmd.attribute("type").value());

    // read in the initiial values and parameters
    if (type.rfind("array") == std::string::npos) {
      // Not an array
      return parseOneValue(type, resXml.child_value());
    }
    else {
      std::vector<Value> values;
      while (!resXml.empty()) {
        values.push_back(parseOneValue(type, resXml.child_value()));
        resXml = resXml.next_sibling();
      }
      return Value(values);
    }
  }

  static void parseParams(pugi::xml_node const root, 
                          std::vector<Value>& dest)
  {
    size_t n = std::distance(root.begin(), root.end());
    if (!n)
      return; // no parameters

    dest.reserve(n);
    pugi::xml_node param = root.child("Param");
    while (!param.empty()) {
      dest.push_back(parseParam(param));
      param = param.next_sibling("Param");
    }
  }

  static Value parseParam(pugi::xml_node const param)
  {
    checkError(!param.first_child().empty()
               || strcmp(param.attribute("type").value(), "string") == 0,
               "Empty Param child in <" << param.parent().name() << "> element.");
    std::string type(param.attribute("type").value());
    std::string val(param.child_value());
    if (val == "UNKNOWN") {
      // Create a typed unknown
      ValueType t = UNKNOWN_TYPE;
      if (type == "int")
        t = INTEGER_TYPE;
      else if (type == "real")
        t = REAL_TYPE;
      else if (type == "bool")
        t = BOOLEAN_TYPE;
      else if (type == "string")
        t = STRING_TYPE;
      return Value(0, t);
    }
    else if (type == "int") {
      int32_t value;
      std::istringstream str(val);
      str >> value;
      return Value(value);
    }
    else if (type == "real") {
      double value;
      std::istringstream str(val);
      str >> value;
      return Value(value);
    }
    else if (type == "bool") {
      bool value;
      std::istringstream str(val);
      str >> value;
      return Value(value);
    }
    // string case
    else if (param.first_child().empty()) {
      return Value("");
    }
    else {
      return Value(param.child_value());
    }
  }

  static Value parseStateValue(pugi::xml_node const stateXml)
  {
    // read in values
    std::string type(stateXml.attribute("type").value());
    checkError(!type.empty(),
               "No type attribute in <" << stateXml.name() << "> element");

    pugi::xml_node valXml = stateXml.child("Value");
    checkError(valXml,
               "No <Value> element in <"  << stateXml.name() << "> element");
    if (type.rfind("array") == std::string::npos) {
      // Not an array
      return parseOneValue(type, valXml.child_value());
    }
    else {
      std::vector<Value> values;
      while (!valXml.empty()) {
        values.push_back(parseOneValue(type, valXml.child_value()));
        valXml = valXml.next_sibling();
      }
      return Value(values);
    }
  }

  // parse in value
  static Value parseOneValue(const std::string& type, 
                             const std::string& valStr)
  {
    // Unknown
    if (0 == stricmp(valStr.c_str(), "Plexil_Unknown")) return Value();

    // string or string-array
    else if (type.find("string") == 0) {
      return Value(valStr);
    }
    // int, int-array
    else if (type.find("int") == 0) {
      int32_t value;
      std::istringstream ss(valStr);
      ss >> value;
      return Value(value);
    }
    // real, real-array
    else if (type.find("real") == 0) {
      double value;
      std::istringstream ss(valStr);
      ss >> value;
      return Value(value);
    }
    // bool or bool-array
    else if (type.find("bool") == 0) {
      if (0 == stricmp(valStr.c_str(), "true"))
        return Value(true);
      else if (0 == stricmp(valStr.c_str(), "false"))
        return Value(false);
      else {
        bool value;
        std::istringstream ss(valStr);
        ss >> value;
        return Value(value);
      }
    }
    else {
      checkError(ALWAYS_FAIL, "Unknown type attribute \"" << type << "\"");
      return Value();
    }
  }

  void TestExternalInterface::lookupNow(State const &state,
                                        StateCacheEntry &cacheEntry)
  {
    debugMsg("Test:testOutput", "Looking up immediately " << state);
    StateMap::const_iterator it = m_states.find(state);
    if (it == m_states.end()) {
      debugMsg("Test:testOutput", "No state found.  Setting UNKNOWN.");
      it = m_states.insert(std::make_pair(state, Value())).first;
    }
    const Value& value = it->second;
    debugMsg("Test:testOutput", "Returning value " << value);
    cacheEntry.update(value);
  }

  void TestExternalInterface::subscribe(const State& state)
  {
    debugMsg("Test:testOutput",
             "Registering change lookup for " << state);

    //ignore source, because we don't care about bandwidth here
    StateMap::iterator it = m_states.find(state);
    if (it == m_states.end()) {
      std::pair<State, Value> p = 
        std::make_pair(state, Value());
      m_states.insert(p);
    }
  }

  void TestExternalInterface::unsubscribe(const State& /* state */)
  {}

  void TestExternalInterface::setThresholds(const State& /* state */,
                                            double /* highThreshold */,
                                            double /* lowThreshold */)
  {}

  void TestExternalInterface::setThresholds(const State& /* state */,
                                            int32_t /* highThreshold */,
                                            int32_t /* lowThreshold */)
  {}

  void TestExternalInterface::executeCommand(Command *cmd)
  {
    State const& command = cmd->getCommand();
    Expression *dest = cmd->getDest();
    Expression *ack = cmd->getAck();
    debugMsg("Test:testOutput", "Executing " << command <<
             " into " <<
             (dest ? dest->toString() : std::string("noId")) <<
             " with ack " << ack->toString());
    if (dest)
      m_executingCommands[command] = cmd;

    // Special handling of the utility commands (a bit of a hack!):
    std::string const & cmdName = command.name();
    if (cmdName == "print") {
      print(command.parameters());
      this->commandHandleReturn(cmd, COMMAND_SUCCESS);
    }
    else if (cmdName == "pprint") {
      pprint(command.parameters());
      this->commandHandleReturn(cmd, COMMAND_SUCCESS);
    }
    else {
      // Usual case - set up for scripted ack value
      m_commandAcks[command] = cmd;
    }
  }

  /**
   * @brief Report the failure in the appropriate way for the application.
   */
  void TestExternalInterface::reportCommandArbitrationFailure(Command *cmd)
  {
    this->commandHandleReturn(cmd, COMMAND_DENIED);
  }

  /**
   * @brief Abort one command in execution.
   * @param cmd The command.
   */

  void TestExternalInterface::invokeAbort(Command *cmd)
  {
    assertTrue_1(cmd);
    State const &command = cmd->getCommand();
    debugMsg("Test:testOutput", "Aborting " << command);
    m_abortingCommands[command] = cmd;
  }

  void TestExternalInterface::executeUpdate(Update * update)
  {
    debugMsg("Test:testOutput", "Received update: ");
    Update::PairValueMap const &pairs = update->getPairs();
    for (Update::PairValueMap::const_iterator pairIt = pairs.begin(); pairIt != pairs.end(); ++pairIt)
      debugMsg("Test:testOutput", " " << pairIt->first << " => " << pairIt->second);
    m_waitingUpdates.insert(std::make_pair(update->getSource()->getNodeId(), update));
  }

  static std::string getText(const State& c)
  {
    std::ostringstream retval;
    retval << c.name() << "(";
    std::vector<Value>::const_iterator it = c.parameters().begin();
    if (it != c.parameters().end()) {
      retval << *it;
      for (++it; it != c.parameters().end(); ++it)
        retval << ", " << *it;
    }
    retval << ")";
    return retval.str();
  }

  static std::string getText(const State& c, const Value& val)
  {
    std::ostringstream retval;
    retval << getText(c);
    retval << " = ";
    if (val.valueType() == STRING_TYPE)
      retval << "(string)" << val;
    else
      retval << val;
    return retval.str();
  }

  double TestExternalInterface::currentTime()
  {
    double result = 0; // default if unknown
    m_states[State::timeState()].getValue(result);
    return result;
  }
}

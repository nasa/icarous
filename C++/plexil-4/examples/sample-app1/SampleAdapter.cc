/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#include "SampleAdapter.hh"

#include "subscriber.hh"
#include "SampleSystem.hh"

#include "AdapterConfiguration.hh"
#include "AdapterFactory.hh"
#include "AdapterExecInterface.hh"
#include "Debug.hh"
#include "Expression.hh"
#include "StateCacheEntry.hh"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::string;
using std::vector;
using std::copy;


///////////////////////////// Conveniences //////////////////////////////////

// A preamble for error messages.
static string error = "Error in SampleAdaptor: ";

// A prettier name for the "unknown" value.
static Value const Unknown;

// Instantiate the system here.
static SampleSystem System;

// A localized handle on the adapter, which allows a
// decoupling between the sample system and adapter.
static SampleAdapter * Adapter;

// An empty argument vector.
static vector<Value> const EmptyArgs;


///////////////////////////// State support //////////////////////////////////

// Queries the system for the value of a state and its arguments.
//
static Value fetch (const string& state_name, const vector<Value>& args)
{
  debugMsg("SampleAdapter:fetch",
           "Fetch called on " << state_name << " with " << args.size() << " args");
  Value retval;

  // NOTE: A more streamlined approach to dispatching on state name
  // would be nice.

  if (state_name == "Size") retval = System.getSize();
  else if (state_name == "Speed") retval = System.getSpeed();
  else if (state_name == "Color") retval = System.getColor();
  else if (state_name == "at") {
    switch (args.size()) {
    case 0:
      retval = System.at ();
      break;
    case 1: {
      string s;
      args[0].getValue(s);
      retval = System.at(s);
      break;
    }
    case 2: {
      int32_t arg0 = 0, arg1 = 0;
      args[0].getValue(arg0);
      args[1].getValue(arg1);
      retval = System.at (arg0, arg1);
      break;
    }
    default: {
      cerr << error << "invalid lookup of 'at'" << endl;
      retval = Unknown;
    }
    }
  }
  else {
    cerr << error << "invalid state: " << state_name << endl;
    retval = Unknown;
  }

  debugMsg("SampleAdapter:fetch", "Fetch returning " << retval);
  return retval;
}


// The 'receive' functions are the subscribers for system state updates.  They
// receive the name of the state whose value has changed in the system.  Then
// they propagate the state's new value to the executive.

static void propagate (const State& state, const vector<Value>& value)
{
  Adapter->propagateValueChange (state, value);
}

static State createState (const string& state_name, const vector<Value>& value)
{
  State state(state_name, value.size());
  if (value.size() > 0)
  {
    for(size_t i=0; i<value.size();i++)
    {
      state.setParameter(i, value[i]);
    }
  }
  return state;
}

static void receiveInt (const string& state_name, int val)
{
  propagate (createState(state_name, EmptyArgs),
             vector<Value> (1, val));
}

static void receiveFloat (const string& state_name, float val)
{
  propagate (createState(state_name, EmptyArgs),
             vector<Value> (1, val));
}

static void receiveString (const string& state_name, const string& val)
{
  propagate (createState(state_name, EmptyArgs),
             vector<Value> (1, val));
}

static void receiveBoolString (const string& state_name, bool val, const string& arg)
{
  propagate (createState(state_name, vector<Value> (1, arg)),
             vector<Value> (1, val));
}

static void receiveBoolIntInt (const string& state_name, bool val, int arg1, int arg2)
{
  vector<Value> vec;
  vec.push_back (arg1);
  vec.push_back (arg2);
  propagate (createState(state_name, vec), vector<Value> (1, val));
}


///////////////////////////// Member functions //////////////////////////////////


SampleAdapter::SampleAdapter(AdapterExecInterface& execInterface,
                             const pugi::xml_node& configXml) :
    InterfaceAdapter(execInterface, configXml)
{
  debugMsg("SampleAdapter", " created.");
}

bool SampleAdapter::initialize()
{
  g_configuration->defaultRegisterAdapter(this);
  Adapter = this;
  setSubscriber (receiveInt);
  setSubscriber (receiveFloat);
  setSubscriber (receiveString);
  setSubscriber (receiveBoolString);
  setSubscriber (receiveBoolIntInt);

  debugMsg("SampleAdapter", " initialized.");
  return true;
}

bool SampleAdapter::start()
{
  debugMsg("SampleAdapter", " started.");
  return true;
}

bool SampleAdapter::stop()
{
  debugMsg("SampleAdapter", " stopped.");
  return true;
}

bool SampleAdapter::reset()
{
  debugMsg("SampleAdapter", " reset.");
  return true;
}

bool SampleAdapter::shutdown()
{
  debugMsg("SampleAdapter", " shut down.");
  return true;
}


// Sends a command (as invoked in a Plexil command node) to the system and sends
// the status, and return value if applicable, back to the executive.
//
void SampleAdapter::executeCommand(Command *cmd)
{
  string const &name = cmd->getName();
  debugMsg("SampleAdapter", "Received executeCommand for " << name);  

  Value retval = Unknown;
  vector<Value> argv(10);
  const vector<Value>& args = cmd->getArgValues();
  copy (args.begin(), args.end(), argv.begin());

  // NOTE: A more streamlined approach to dispatching on command type
  // would be nice.
  string s;
  int32_t i1 = 0, i2 = 0;
  double d;

  if (name == "SetSize") {
    args[0].getValue(d);
    System.setSize (d);
  }
  else if (name == "SetSpeed") {
    args[0].getValue(i1);
    System.setSpeed (i1);
  }
  else if (name == "SetColor") {
    args[0].getValue(s);
    System.setColor (s);
  }
  else if (name == "Move") {
    args[0].getValue(s);
    args[1].getValue(i1);
    args[2].getValue(i2);
    System.move (s, i1, i2);
  }
  else if (name == "Hello")
    System.hello ();
  else if (name == "Square") {
    args[0].getValue(i1);
    retval = System.square (i1);
  }
  else
    cerr << error << "invalid command: " << name << endl;

  // This sends a command handle back to the executive.
  m_execInterface.handleCommandAck(cmd, COMMAND_SENT_TO_SYSTEM);
  // This sends the command's return value (if expected) to the executive.
  if (retval != Unknown) {
    m_execInterface.handleCommandReturn(cmd, retval);
  }
  m_execInterface.notifyOfExternalEvent();
}


void SampleAdapter::lookupNow (const State& state, StateCacheEntry &entry)
{
  entry.update(fetch(state.name(), state.parameters()));
}


void SampleAdapter::subscribe(const State& state)
{
  debugMsg("SampleAdapter:subscribe", " processing state " << state.name());
  m_subscribedStates.insert(state);
}


void SampleAdapter::unsubscribe (const State& state)
{
  debugMsg("SampleAdapter:subscribe", " from state " << state.name());
  m_subscribedStates.erase(state);
}

// Does nothing.
void SampleAdapter::setThresholds (const State& state, double hi, double lo)
{
}
void SampleAdapter::setThresholds (const State& state, int32_t hi, int32_t lo)
{
}


void SampleAdapter::propagateValueChange (const State& state,
                                          const vector<Value>& vals) const
{
  if (!isStateSubscribed(state))
    return; 
  m_execInterface.handleValueChange (state, vals.front());
  m_execInterface.notifyOfExternalEvent();
}


bool SampleAdapter::isStateSubscribed(const State& state) const
{
  return m_subscribedStates.find(state) != m_subscribedStates.end();
}

// Necessary boilerplate
extern "C" {
  void initSampleAdapter() {
    REGISTER_ADAPTER(SampleAdapter, "SampleAdapter");
  }
}

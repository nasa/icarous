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

#include "State.hh"

#include "Error.hh" // assertTrue_2 macro

#include <ostream>
#include <sstream>

namespace PLEXIL
{

  State::State()
  {
  }

  State::State(State const &other)
    : m_name(other.m_name),
      m_parameters(other.m_parameters)
  {
  }

#if __cplusplus >= 201103L
  State::State(State &&other)
    : m_name(std::move(other.m_name)),
      m_parameters(std::move(other.m_parameters))
  {
  }
#endif

  State::State(char const *name, size_t n)
    : m_name(name),
      m_parameters(n)
  {
  }

  State::State(std::string const &name, size_t n)
    : m_name(name),
      m_parameters(n)
  {
  }

  State::State(std::string const &name, Value const &arg0)
    : m_name(name),
      m_parameters(1, arg0)
  {
  }

  State::~State()
  {
  }

  State &State::operator=(State const &other)
  {
    m_name = other.m_name;
    m_parameters = other.m_parameters;
    return *this;
  }

#if __cplusplus >= 201103L
  State &State::operator=(State &&other)
  {
    m_name = std::move(other.m_name);
    m_parameters = std::move(other.m_parameters);
    return *this;
  }
#endif

  std::string const &State::name() const
  {
    return m_name;
  }

  std::vector<Value> const &State::parameters() const
  {
    return m_parameters;
  }

  size_t State::parameterCount() const
  {
    return m_parameters.size();
  }
   
  bool State::isParameterKnown(size_t n) const
  {
    if (n < m_parameters.size())
      return m_parameters[n].isKnown();
    return false;
  }

  ValueType State::parameterType(size_t n) const
  {
    if (n < m_parameters.size())
      return m_parameters[n].valueType();
    return UNKNOWN_TYPE;
  }

  Value const &State::parameter(size_t n) const
  {
    if (n < m_parameters.size())
      return m_parameters[n];

    // FIXME: This should be a global constant
    static Value const sl_unknown;
    return sl_unknown;
  }

  void State::setName(std::string const &name)
  {
    m_name = name;
  }

  void State::setParameterCount(size_t n)
  {
    m_parameters.resize(n);
  }

  void State::setParameter(size_t i, Value const &val)
  {
    assertTrue_2(i < m_parameters.size(), "State::setParameter: index out of range");
    m_parameters[i] = val;
  }

  void State::print(std::ostream &s) const
  {
    s << m_name << '(';
    size_t i = 0;
    while (i < m_parameters.size()) {
      s << m_parameters[i];
      if (++i < m_parameters.size())
        s << ", ";
    }
    s << ')';
  }

  std::string State::toString() const
  {
    std::ostringstream s;
    print(s);
    return s.str();
  }

  std::ostream &operator<<(std::ostream &s, State const &st)
  {
    st.print(s);
    return s;
  }

  char *State::serialize(char *b) const
  {
    *b++ = STATE_TYPE;
    b = PLEXIL::serialize(m_name, b);
    // Put 3 bytes of parameter count
    size_t s = m_parameters.size();
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);
    for (size_t i = 0; b != NULL && i < s; ++i)
      b = PLEXIL::serialize(m_parameters[i], b);
    return b;
  }

  template <>
  char *serialize<State>(State const &o, char *b)
  {
    return o.serialize(b);
  }

  char const *State::deserialize(char const *b)
  {
    if (STATE_TYPE != (ValueType) *b++)
      return NULL;
    b = PLEXIL::deserialize(m_name, b);
    // Get parameter count
    size_t s = ((size_t) (unsigned char) *b++) << 8;
    s = (s + (size_t) (unsigned char) *b++) << 8;
    s = s + (size_t) (unsigned char) *b++;
    m_parameters.resize(s);
    for (size_t i = 0; b != NULL && i < s; ++i)
      b = PLEXIL::deserialize(m_parameters[i], b);
    return b;
  }

  template <>
  char const *deserialize<State>(State &o, char const *b)
  {
    return o.deserialize(b);
  }

  size_t State::serialSize() const
  {
    size_t result = 4 + PLEXIL::serialSize(m_name);
    for (size_t i = 0; i < m_parameters.size(); ++i)
      result += PLEXIL::serialSize(m_parameters[i]);
    return result;
  }

  template <>
  size_t serialSize<State>(State const &o)
  {
    return o.serialSize();
  }

  bool operator==(State const &a, State const &b)
  {
    return (a.name() == b.name()
            && a.parameters() == b.parameters());
  }


  bool operator<(State const &a, State const &b)
  {
    if (a.m_name < b.m_name)
      return true;
    if (a.m_name > b.m_name)
      return false;
    // Same name
    size_t aSize = a.m_parameters.size();
    if (aSize < b.m_parameters.size())
      return true;
    if (aSize > b.m_parameters.size())
      return false;
    // Same # params
    for (size_t i = 0; i < a.parameterCount(); ++i) {
      Value const &av = a.m_parameters[i];
      Value const &bv = b.m_parameters[i];
      if (av < bv)
        return true;
      if (av > bv)
        return false;
    }
    return false; // states are equal
  }

  // Global "constant"
  State const &State::timeState()
  {
    static State const sl_timeState("time");
    return sl_timeState;
  }

} // namespace PLEXIL

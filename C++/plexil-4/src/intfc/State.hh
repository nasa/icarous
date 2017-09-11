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

#ifndef PLEXIL_STATE_HH
#define PLEXIL_STATE_HH

#include "Value.hh"

namespace PLEXIL
{
  /**
   * @class State
   * @brief Represents the ground values at a particular instant
   *        of the name and arguments of a Lookup or Command. 
   */
  class State
  {
  public:
    State();
    State(State const &orig);
#if __cplusplus >= 201103L
    State(State &&);
#endif
    State(char const *name, size_t n = 0);
    State(std::string const &name, size_t n = 0);

    // Convenience for interface adapters
    State(std::string const &name, Value const &arg0);

    ~State();

    State &operator=(State const &other);
#if __cplusplus >= 201103L
    State &operator=(State &&);
#endif
    std::string const &name() const;
    std::vector<Value> const &parameters() const;

    size_t parameterCount() const;
    bool isParameterKnown(size_t n) const;
    ValueType parameterType(size_t n) const;
    Value const &parameter(size_t n) const;

    void setName(std::string const &name);
    void setParameterCount(size_t n);
    void setParameter(size_t i, Value const &val);

    void print(std::ostream &s) const;
    std::string toString() const;

    static State const &timeState();

    // Serialization support
    char *serialize(char *b) const;
    char const *deserialize(char const *b);
    size_t serialSize() const;

  private:

    friend bool operator<(State const &, State const &);

    std::string m_name;
    std::vector<Value> m_parameters;
  };

  bool operator==(State const &, State const &);
  inline bool operator!=(State const &a, State const &b)
  {
    return !(a == b);
  }

  // Comparisons for use with std::map
  bool operator<(State const &, State const &);
  inline bool operator>(State const &a, State const &b)
  {
    return b < a;
  }
  inline bool operator>=(State const &a, State const &b)
  {
    return !(a < b);
  }
  inline bool operator<=(State const &a, State const &b)
  {
    return !(b < a);
  }

  std::ostream &operator<<(std::ostream &, State const &);

} // namespace PLEXIL

#endif // PLEXIL_STATE_HH

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

#ifndef PLEXIL_UPDATE_HH
#define PLEXIL_UPDATE_HH

#include "SimpleBooleanVariable.hh"
#include "Value.hh"

#include "SimpleMap.hh"

namespace PLEXIL
{
  // Forward declarations in PLEXIL namespace
  class NodeConnector;
  struct Pair;

  class Update 
  {
  public:
    typedef SimpleMap<std::string, Value> PairValueMap;

    Update(NodeConnector *node);

    ~Update();

    //
    // Parser API
    //

    void reservePairs(size_t n);
    // Return true if OK, false if name is a duplicate
    bool addPair(std::string const &name, Expression *exp, bool garbage);

    Expression *getAck() {return &m_ack;}
    const PairValueMap& getPairs() const {return m_valuePairs;}
    NodeConnector *getSource() {return m_source;}
    NodeConnector const *getSource() const {return m_source;}
    void activate();
    void deactivate();

    void execute();
    void acknowledge(bool ack);

    void cleanUp();

    // Made public for parser unit test
    void fixValues();

    // LinkedQueue item API
    Update *next() const
    {
      return m_next;
    }

    Update **nextPtr()
    {
      return &m_next;
    }

  private:
    // Deliberately unimplemented
    Update();
    Update(Update const &);
    Update& operator=(Update const &);

    Update *m_next;
    NodeConnector *m_source;
    SimpleBooleanVariable m_ack;
    Pair *m_pairs;
    PairValueMap m_valuePairs;
  };

}

#endif // PLEXIL_UPDATE_HH

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

#ifndef PLEXIL_VARIABLE_CONFLICT_SET_HH
#define PLEXIL_VARIABLE_CONFLICT_SET_HH

#include <set>
#include <vector>

#include <cstddef> // size_t

namespace PLEXIL
{
  // Forward references
  class Expression;
  class Node;

  /**
   * @class VariableConflictSet
   * @brief A hybrid of std::multiset and std::priority_queue,
   *        intended for maintaining a set of nodes assigning to the same variable
   *        in priority order.
   */
  class VariableConflictSet
  {
  public:
    typedef std::vector<Node *>::const_iterator const_iterator;
    typedef std::vector<Node *>::iterator iterator;

    VariableConflictSet();
    ~VariableConflictSet();

    Expression const *getVariable() const;
    void setVariable(Expression *);

    VariableConflictSet *next() const;
    void setNext(VariableConflictSet *);

    size_t size() const; // self-explanatory
    bool empty() const;  // self-explanatory

    void push(Node *);            // insert unique in (weakly) sorted order
    Node *front();                // access the element with lowest priority which was inserted first
    void remove(Node *);          // delete the indicated element (no error if not there)
    size_t front_count() const;   // how many have same priority as front element

    const_iterator begin() const;
    const_iterator end() const;

    // Managing pool of instances
    static VariableConflictSet *allocate();
    static void release(VariableConflictSet *);

  private:

    // Not implemented
    VariableConflictSet(VariableConflictSet const &);
    VariableConflictSet &operator=(VariableConflictSet const &);

    VariableConflictSet *m_next;
    Expression *m_variable;
    std::vector<Node *> m_nodes;
  };

} // namespace PLEXIL

#endif // PLEXIL_VARIABLE_CONFLICT_SET_HH

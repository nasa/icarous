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

#include "LibraryCallNode.hh"

#include "Debug.hh"
#include "Error.hh"
#include "NodeVariableMap.hh"

namespace PLEXIL
{

  LibraryCallNode::LibraryCallNode(char const *nodeId, Node *parent)
    : ListNode(nodeId, parent),
      m_aliasMap(NULL)
  {
  }

  /**
   * @brief Alternate constructor.  Used only by Exec test module.
   */
  LibraryCallNode::LibraryCallNode(const std::string& type,
                                   const std::string& name, 
                                   NodeState state,
                                   Node *parent)
    : ListNode(type, name, state, parent),
      m_aliasMap(NULL)
  {
    checkError(type == LIBRARYNODECALL,
               "Invalid node type \"" << type << "\" for a LibraryCallNode");
  }

  /**
   * @brief Destructor.  Cleans up this entire part of the node tree.
   */
  LibraryCallNode::~LibraryCallNode()
  {
    debugMsg("LibraryCallNode:~LibraryCallNode", '<' << m_nodeId << '>');

    cleanUpConditions();
    cleanUpNodeBody();
    cleanUpVars();
  }

  void LibraryCallNode::cleanUpNodeBody()
  {
    if (m_cleanedBody)
      return;

    ListNode::cleanUpNodeBody();

    delete m_aliasMap;
  }

  void LibraryCallNode::allocateAliasMap(size_t n)
  {
    m_aliasMap = new NodeVariableMap(NULL);
    m_aliasMap->grow(n);
  }

  // For plan parser.
  bool LibraryCallNode::addAlias(char const *name, Expression *exp, bool isGarbage)
  {
    assertTrue_1(m_aliasMap);
    if (m_aliasMap->find(name) != m_aliasMap->end())
      return false; // duplicate
    (*m_aliasMap)[name] = exp;
    if (isGarbage) {
      if (!m_localVariables)
        m_localVariables = new std::vector<Expression *>();
      m_localVariables->push_back(exp);
    }
    return true;
  }

  // LibraryCall nodes don't allow children to refer to ancestor environment
  NodeVariableMap const *LibraryCallNode::getChildVariableMap() const
  {
    return m_aliasMap;
  }

}

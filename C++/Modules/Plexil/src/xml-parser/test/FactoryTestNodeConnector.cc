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

#include "test/FactoryTestNodeConnector.hh"

#include "Expression.hh"

namespace PLEXIL
{
  FactoryTestNodeConnector::FactoryTestNodeConnector()
    : NodeConnector()
  {
  }

  FactoryTestNodeConnector::~FactoryTestNodeConnector()
  {
    for (TestVariableMap::iterator it = m_variableMap.begin();
         it != m_variableMap.end();
         ++it) {
      Expression *tmp = it->second;
      it->second = NULL;
      delete tmp;
    }
    m_variableMap.clear();
  }

  Expression *FactoryTestNodeConnector::findVariable(char const *name)
  {
    TestVariableMap::const_iterator it = m_variableMap.find(std::string(name));
    if (it != m_variableMap.end())
      return it->second;
    else
      return NULL;
  }

  std::string const &FactoryTestNodeConnector::getNodeId() const
  {
    static std::string sl_empty;
    return sl_empty;
  }

  Node const *FactoryTestNodeConnector::findChild(char const * /* childName */) const
  {
    return NULL;
  }
   
  Node *FactoryTestNodeConnector::findChild(char const * /* childName */)
  {
    return NULL;
  }

  Node *FactoryTestNodeConnector::getParent()
  {
    return NULL;
  }

  Node const *FactoryTestNodeConnector::getParent() const
  {
    return NULL;
  }

  int32_t FactoryTestNodeConnector::getPriority() const
  {
    return WORST_PRIORITY;
  }

  void FactoryTestNodeConnector::storeVariable(const std::string & name, Expression *var)
  {
    TestVariableMap::iterator it = m_variableMap.find(name);
    if (it != m_variableMap.end()) {
      delete it->second;
      it->second = var; // replace existing
    }
    else 
      m_variableMap.insert(name, var);
  }

} // namespace PLEXIL

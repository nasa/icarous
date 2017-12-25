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

#include "NodeFunction.hh"

#include "ArrayImpl.hh"
#include "Error.hh"
#include "NodeOperator.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

namespace PLEXIL
{
  NodeFunction::NodeFunction(NodeOperator const *op, Node *node)
    : NotifierImpl(),
      m_op(op),
      m_node(node),
      m_valueCache(op->allocateCache())
  {
  }

  NodeFunction::~NodeFunction()
  {
    m_op->deleteCache(m_valueCache);
  }

  const char *NodeFunction::exprName() const
  {
    return m_op->getName().c_str();
  }

  ValueType NodeFunction::valueType() const
  {
    return m_op->valueType();
  }

  bool NodeFunction::isKnown() const
  {
    // Delegate to operator
    return m_op->calcNative(m_valueCache, m_node);
  }

  void NodeFunction::printValue(std::ostream &s) const
  {
    m_op->printValue(s, m_valueCache, m_node);
  }

  void NodeFunction::printSubexpressions(std::ostream & /* s */) const
  {
    // TODO
  }

  Value NodeFunction::toValue() const
  {
    return m_op->toValue(m_valueCache, m_node);
  }

#define DEFINE_NODE_FUNC_GET_VALUE_METHOD(_rtype) \
  bool NodeFunction::getValue(_rtype &result) const \
  { \
    return (*m_op)(result, m_node); \
  }

  DEFINE_NODE_FUNC_GET_VALUE_METHOD(Boolean)
  DEFINE_NODE_FUNC_GET_VALUE_METHOD(uint16_t)
  DEFINE_NODE_FUNC_GET_VALUE_METHOD(Integer)
  DEFINE_NODE_FUNC_GET_VALUE_METHOD(Real)
  DEFINE_NODE_FUNC_GET_VALUE_METHOD(String)

#undef DEFINE_NODE_FUNC_GET_VALUE_METHOD

#define DEFINE_NODE_FUNC_GET_VALUE_PTR_METHOD(_rtype) \
  bool NodeFunction::getValuePointer(_rtype const *&ptr) const \
  { \
    bool result = (*m_op)(*static_cast<_rtype *>(m_valueCache), m_node); \
    if (result) \
      ptr = static_cast<_rtype const *>(m_valueCache); /* trust me */ \
    return result; \
  }

  DEFINE_NODE_FUNC_GET_VALUE_PTR_METHOD(String)
  
#undef DEFINE_NODE_FUNC_GET_VALUE_PTR_METHOD

#define DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB(_rtype) \
  bool NodeFunction::getValuePointer(_rtype const *& /* ptr */) const   \
  { \
    checkError(ALWAYS_FAIL, \
               "NodeFunction::getValuePointer not implemented for " \
               << PlexilValueType<_rtype>::typeName); \
    return false; \
  }

  DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB(Array)
  DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB(BooleanArray)
  DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB(IntegerArray)
  DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB(RealArray)
  DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB(StringArray)
  
#undef DEFINE_NODE_FUNC_GET_VALUE_PTR_STUB

} // namespace PLEXIL

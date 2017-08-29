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

#include "Alias.hh"
#include "Error.hh"
#include "Value.hh"

#include <cstdlib> // free()
#include <cstring> // strdup()

namespace PLEXIL
{
  //
  // Alias
  //

  Alias::Alias(NodeConnector *node,
               char const *name,
               Expression *original,
               bool garbage)
    : NotifierImpl(),
      m_exp(original),
      m_node(node),
      m_name(strdup(name)),
      m_garbage(garbage)
  {
  }

  Alias::~Alias()
  {
    m_exp->removeListener(this);
    if (m_garbage)
      delete m_exp;
    free((void *)m_name);
  }

  char const *Alias::getName() const
  {
    return m_name;
  }

  char const *Alias::exprName() const
  {
    return "InAlias";
  }
   
  ValueType Alias::valueType() const
  {
    return m_exp->valueType();
  }
  
  bool Alias::isKnown() const
  {
    if (!isActive())
      return false;
    return m_exp->isKnown();
  }
  
  bool Alias::isAssignable() const
  {
    return false;
  }

  bool Alias::isConstant() const
  {
    return m_exp->isConstant();
  }

  bool Alias::isPropagationSource() const
  {
    return false;
  }

  Expression *Alias::getBaseExpression()
  {
    return m_exp->getBaseExpression();
  }

  Expression const *Alias::getBaseExpression() const
  {
    return m_exp->getBaseExpression();
  }

  void Alias::printValue(std::ostream &s) const
  {
    m_exp->printValue(s);
  }

  bool Alias::getValue(Boolean &var) const
  {
    if (!isActive())
      return false;
    return m_exp->getValue(var);
  }

  bool Alias::getValue(uint16_t &var) const
  {
    if (!isActive())
      return false;
    return m_exp->getValue(var);
  }

  bool Alias::getValue(Integer &var) const
  {
    if (!isActive())
      return false;
    return m_exp->getValue(var);
  }

  bool Alias::getValue(Real &var) const
  {
    if (!isActive())
      return false;
    return m_exp->getValue(var);
  }

  bool Alias::getValue(String &var) const
  {
    if (!isActive())
      return false;
    return m_exp->getValue(var);
  }

  bool Alias::getValuePointer(String const *&ptr) const
  {
    if (!isActive())
      return false;
    return m_exp->getValuePointer(ptr);
  }

  bool Alias::getValuePointer(Array const *&ptr) const
  {
    if (!isActive())
      return false;
    return m_exp->getValuePointer(ptr);
  }

  bool Alias::getValuePointer(BooleanArray const *&ptr) const
  {
    if (!isActive())
      return false;
    return m_exp->getValuePointer(ptr);
  }

  bool Alias::getValuePointer(IntegerArray const *&ptr) const
  {
    if (!isActive())
      return false;
    return m_exp->getValuePointer(ptr);
  }

  bool Alias::getValuePointer(RealArray const *&ptr) const
  {
    if (!isActive())
      return false;
    return m_exp->getValuePointer(ptr);
  }

  bool Alias::getValuePointer(StringArray const *&ptr) const
  {
    if (!isActive())
      return false;
    return m_exp->getValuePointer(ptr);
  }

  Value Alias::toValue() const
  {
    if (!isActive())
      return Value(0, m_exp->valueType());
    return m_exp->toValue();
  }

  void Alias::doSubexprs(ExprUnaryOperator const &f)
  {
    (f)(m_exp);
  }

} // namespace PLEXIL

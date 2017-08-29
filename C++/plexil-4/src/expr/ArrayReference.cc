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

#include "ArrayReference.hh"

#include "Array.hh"
#include "ArrayVariable.hh"
#include "PlanError.hh"
#include "PlexilTypeTraits.hh"

namespace PLEXIL
{
  ArrayReference::ArrayReference(Expression *ary,
                                 Expression *idx,
                                 bool aryIsGarbage,
                                 bool idxIsGarbage)
    : NotifierImpl(),
      m_array(ary),
      m_index(idx),
      m_arrayIsGarbage(aryIsGarbage),
      m_indexIsGarbage(idxIsGarbage),
      m_namePtr(new std::string())
  {
  }

  ArrayReference::~ArrayReference()
  {
    if (m_arrayIsGarbage)
      delete m_array;
    if (m_indexIsGarbage)
      delete m_index;
    delete m_namePtr;
  }

  char const *ArrayReference::getName() const
  {
    Expression const *base = getBaseExpression();
    if (base) {
      std::ostringstream s;
      s << base->getName() << '[' << m_index->valueString() << ']';
      *m_namePtr = s.str();
      return m_namePtr->c_str();
    }
    static char const *sl_dummy = "";
    return sl_dummy;
  }

  char const *ArrayReference::exprName() const
  {
    return "ArrayReference";
  }

  ValueType ArrayReference::valueType() const
  {
    ValueType aryType = m_array->valueType();
    if (!isArrayType(aryType))
      return UNKNOWN_TYPE;
    else
      return arrayElementType(aryType);
  }

  bool ArrayReference::isKnown() const
  {
    Array const *dummyAry;
    size_t dummyIdx;
    return selfCheck(dummyAry, dummyIdx);
  }

  bool ArrayReference::isConstant() const
  {
    return m_array->isConstant() && m_index->isConstant();
  }

  bool ArrayReference::isPropagationSource() const
  {
    return false; // can't change value independently of subexpressions
  }

  bool ArrayReference::isAssignable() const
  {
    return false;
  }

  Expression const *ArrayReference::getBaseExpression() const
  {
    return m_array->getBaseExpression();
  }

  Expression *ArrayReference::getBaseExpression()
  {
    return m_array->getBaseExpression();
  }

  void ArrayReference::printValue(std::ostream &s) const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx)) {
      s << "UNKNOWN";
      return;
    }

    // Punt for now
    s << ary->getElementValue(idx);
  }

  bool ArrayReference::selfCheck(Array const *&valuePtr,
                                 size_t &idx) const
  {
    if (!(this->isActive() && m_array->isActive() && m_index->isActive()))
      return false;
    int32_t idxTemp;
    if (!m_index->getValue(idxTemp))
      return false; // index is unknown
    checkPlanError(idxTemp >= 0, "Array index " << idxTemp << " is negative");
    idx = (size_t) idxTemp;
    if (!m_array->getValuePointer(valuePtr))
      return false; // array unknown or invalid
    std::vector<bool> const &kv = valuePtr->getKnownVector();
    checkPlanError(idx < kv.size(),
                   "Array index " << idx
                   << " equals or exceeds array size " << kv.size());
    return kv[idx];
  }

  // Local macro
#define DEFINE_AREF_GET_VALUE_METHOD(_type_) \
  bool ArrayReference::getValue(_type_ &result) const \
  { \
    Array const *ary; \
    size_t idx; \
    if (!selfCheck(ary, idx)) \
      return false; \
    return ary->getElement(idx, result); \
  }

  // getValue explicit instantiations
  DEFINE_AREF_GET_VALUE_METHOD(Boolean)
  DEFINE_AREF_GET_VALUE_METHOD(Integer)
  DEFINE_AREF_GET_VALUE_METHOD(Real)
  DEFINE_AREF_GET_VALUE_METHOD(String)

#undef DEFINE_AREF_GET_VALUE_METHOD

  bool ArrayReference::getValue(uint16_t &result) const
  {
    checkPlanError(ALWAYS_FAIL,
                   "Array references not implemented for internal values");
    return false;
  }

  bool ArrayReference::getValuePointer(String const *&ptr) const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx))
      return false;
    return ary->getElementPointer(idx, ptr);
  }

  // Error for arrays and any other types we can think of.
#define DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD(_type_) \
  bool ArrayReference::getValuePointer(_type_ const *&ptr) const \
  { \
    assertTrueMsg(ALWAYS_FAIL, \
                  "getValuePointer: trying to get a " << PlexilValueType<_type_>::typeName \
                  << " pointer value from an ArrayReference"); \
    return false; \
  }

  DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD(Array)
  DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD(BooleanArray)
  DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD(IntegerArray)
  DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD(RealArray)
  DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD(StringArray)

#undef DEFINE_AREF_GET_VALUE_PTR_ERROR_METHOD

  Value ArrayReference::toValue() const
  {
    Array const *ary;
    size_t idx;
    if (!selfCheck(ary, idx))
      return Value(); // unknown
    else
      return ary->getElementValue(idx);
  }

  void ArrayReference::doSubexprs(ExprUnaryOperator const &f)
  {
    (f)(m_array);
    (f)(m_index);
  }

  void ArrayReference::handleActivate()
  {
    m_array->activate();
    m_index->activate();
  }
  
  void ArrayReference::handleDeactivate()
  {
    m_array->deactivate();
    m_index->deactivate();
  }

  //
  // MutableArrayReference
  //

  MutableArrayReference::MutableArrayReference(Expression *ary,
                                               Expression *idx,
                                               bool aryIsGarbage,
                                               bool idxIsGarbage)
    : Assignable(),
      ArrayReference(ary, idx, aryIsGarbage, idxIsGarbage),
      m_mutableArray(dynamic_cast<ArrayVariable *>(ary->getBaseExpression())),
      m_saved(false)
  {
  }

  MutableArrayReference::~MutableArrayReference()
  {
  }

  bool MutableArrayReference::isAssignable() const
  {
    return true;
  }

  Assignable const *MutableArrayReference::asAssignable() const
  {
    return dynamic_cast<Assignable const *>(this);
  }

  Assignable *MutableArrayReference::asAssignable()
  {
    return dynamic_cast<Assignable *>(this);
  }

  bool MutableArrayReference::mutableSelfCheck(size_t &idx)
  {
    checkPlanError(m_mutableArray,
                   "Can't assign to an array element not associated with a variable");
    if (!m_mutableArray->isKnown())
      return false;
    Integer idxTemp;
    if (!m_index->getValue(idxTemp))
      return false; // index is unknown
    checkPlanError(idxTemp >= 0,
                   "Array index " << idxTemp << " is negative");
    idx = (size_t) idxTemp;
    return true;
  }

  void MutableArrayReference::setValue(Value const &value)
  {
    size_t idx;
    if (!mutableSelfCheck(idx))
      return;
    m_mutableArray->setElement(idx, value);
  }

  void MutableArrayReference::setUnknown()
  {
    size_t idx;
    if (!mutableSelfCheck(idx))
      return;
    m_mutableArray->setElementUnknown(idx);
  }

  void MutableArrayReference::saveCurrentValue()
  {
    size_t idx;
    if (!mutableSelfCheck(idx)) {
      // unknown or invalid
      m_saved = false;
      return;
    }
    m_savedValue = m_mutableArray->getElementValue(idx);
    m_saved = true;
  }

  void MutableArrayReference::restoreSavedValue()
  {
    size_t idx;
    if (!mutableSelfCheck(idx) || !m_saved) 
      return;
    if (m_savedValue != m_mutableArray->getElementValue(idx)) {
      m_mutableArray->setElement(idx, m_savedValue);
    }
    m_saved = false;
  }

  Value MutableArrayReference::getSavedValue() const
  {
    return Value(m_savedValue);
  }

  NodeConnector const *MutableArrayReference::getNode() const
  {
    return getBaseVariable()->asAssignable()->getNode();
  }

  NodeConnector *MutableArrayReference::getNode()
  {
    return getBaseVariable()->asAssignable()->getNode();
  }

  Expression *MutableArrayReference::getBaseVariable() 
  {
    checkPlanError(m_mutableArray,
                   "Assignable ArrayElement doesn't resolve to an ArrayVariable");
    return m_mutableArray->getBaseVariable();
  }

  Expression const *MutableArrayReference::getBaseVariable() const
  {
    checkPlanError(m_mutableArray,
                   "Assignable ArrayElement doesn't resolve to an ArrayVariable");
    return m_mutableArray->getBaseVariable();
  }

} // namespace PLEXIL


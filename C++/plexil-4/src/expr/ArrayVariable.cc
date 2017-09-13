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

#include "ArrayVariable.hh"

#include "ArrayImpl.hh"
#include "Constant.hh"
#include "PlanError.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

#include <cstdlib> // free()
#include <cstring> // strdup()

namespace PLEXIL
{

  ArrayVariable::ArrayVariable()
    : NotifierImpl(),
      m_value(),
      m_savedValue(),
      m_size(NULL),
      m_initializer(NULL),
      m_name(NULL),
      m_maxSize(0),
      m_node(NULL),
      m_known(false),
      m_savedKnown(false),
      m_sizeIsGarbage(false),
      m_initializerIsGarbage(false),
      m_sizeIsConstant(false),
      m_initializerIsConstant(false)
  {
  }

  ArrayVariable::ArrayVariable(NodeConnector *node,
                               char const *name,
                               Expression *size,
                               bool sizeIsGarbage)
    : NotifierImpl(),
      m_value(),
      m_savedValue(),
      m_size(size),
      m_initializer(NULL),
      m_name(strdup(name)),
      m_maxSize(0),
      m_node(node),
      m_known(false),
      m_savedKnown(false),
      m_sizeIsGarbage(sizeIsGarbage),
      m_initializerIsGarbage(false),
      m_sizeIsConstant(false),
      m_initializerIsConstant(false)
  {
  }

  ArrayVariable::~ArrayVariable()
  {
    delete m_value;
    delete m_savedValue;
    free((void *) m_name);
    if (m_initializerIsGarbage)
      delete m_initializer;
    if (m_sizeIsGarbage)
      delete m_size;
  }

  //
  // Essential Expression API
  //

  bool ArrayVariable::isAssignable() const
  {
    return true;
  }

  Assignable const *ArrayVariable::asAssignable() const
  {
    return static_cast<Assignable const *>(this);
  }

  Assignable *ArrayVariable::asAssignable()
  {
    return static_cast<Assignable *>(this);
  }

  char const *ArrayVariable::getName() const
  {
    if (m_name)
      return m_name;
    static char const *sl_anon = "anonymous";
    return sl_anon;
  }

  const char *ArrayVariable::exprName() const
  {
    return "ArrayVariable";
  }

  bool ArrayVariable::isKnown() const
  {
    return this->isActive() && m_known;
  }

  void ArrayVariable::handleActivate()
  {
    // Ensure maxSize spec is evaluated before initializer.
    if (m_size) {
      m_size->activate();
      if (!m_sizeIsConstant) { // always false on first activation
        Integer specSize;
        if (m_size->getValue(specSize)) {
          checkPlanError(specSize >= 0,
                         "Negative array size " << specSize
                         << " for array " << this->getName());
          m_maxSize = (size_t) specSize;
          if (m_size->isConstant())
            m_sizeIsConstant = true; // won't need to recalculate
        }
      }
    }

    if (m_initializer) {
      m_initializer->activate();
      Array const *valuePtr;
      if (m_initializer->getValuePointer(valuePtr)) {
        // Initial value is known. If there is a max size, enforce it.
        if (m_size) {
          checkPlanError(valuePtr->size() <= m_maxSize,
                         "Initial value for " << this->getName()
                         << " is larger than declared max size " << m_size);
        }
        m_known = false; // to ensure change is published
        this->setValueImpl(valuePtr); // delegate
      }
      else
        // Initial value is unknown, no need to publish change
        // or to muck with m_value either
        m_known = false;
    }
    else {
      // No initializer, preallocate or resize as appropriate
      if (m_size && m_maxSize) {
        if (m_value) {
          m_value->reset(); // to all unknown
          if (m_value->size() < m_maxSize)
            m_value->resize(m_maxSize);
        }
        else
          m_value = this->makeArray(m_maxSize); // delegate to derived class
        m_known = true; // array is known, not its contents
        this->publishChange();
      }
      else
        m_known = false; // no need to publish
    }
  }

  void ArrayVariable::handleDeactivate()
  {
    if (m_initializer)
      m_initializer->deactivate();
    if (m_size)
      m_size->deactivate();
  }

  void ArrayVariable::printSpecialized(std::ostream &s) const
  {
    s << getName() << ' ';
    if (m_size)
      s << "size = " << m_maxSize << ' ';
  }

  void ArrayVariable::setUnknown()
  {
    if (!m_known)
      return; // no change
    m_known = false;
    publishChange();
  }

  void ArrayVariable::saveCurrentValue()
  {
    m_savedKnown = m_known;
    if (m_known) {
      if (m_savedValue)
        *m_savedValue = *m_value;
      else
        m_savedValue = m_value->clone();
    }
  }

  Value ArrayVariable::getSavedValue() const
  {
    if (m_savedValue)
      return Value(*m_savedValue);
    else
      return Value();
  }

  NodeConnector const *ArrayVariable::getNode() const
  {
    return m_node;
  }

  NodeConnector *ArrayVariable::getNode()
  {
    return m_node;
  }

  Expression *ArrayVariable::getBaseVariable()
  {
    return this;
  }

  Expression const *ArrayVariable::getBaseVariable() const
  {
    return this;
  }

  // *** FIXME ***
  void ArrayVariable::setInitializer(Expression *expr, bool garbage)
  {
    checkPlanError(expr->valueType() == this->valueType()
                   || expr->valueType() == UNKNOWN_TYPE,
                   "Type of array variable " << this->getName()
                   << ", " << valueTypeName(this->valueType())
                   << ", differs from initializer's type, "
                   << valueTypeName(expr->valueType()));
    Integer size;
    Array const *temp;
    if (m_size && m_size->getValue(size) && expr->getValuePointer(temp))
      checkPlanError(temp->size() <= (size_t) size,
                     "Array variable " << this->getName()
                     << " initial value is larger than declared array size " << size);
    m_initializer = expr;
    m_initializerIsGarbage = garbage;
  }

  void ArrayVariable::setValue(Value const &val)
  {
    Array const *aryPtr;
    if (val.getValuePointer(aryPtr))
      this->setValueImpl(aryPtr);
    else
      setUnknown();
  }

  void ArrayVariable::setValue(Expression const &val)
  {
    Array const *aryPtr;
    if (val.getValuePointer(aryPtr))
      this->setValueImpl(aryPtr);
    else
      setUnknown();
  }

  bool ArrayVariable::elementIsKnown(size_t idx) const
  {
    if (!this->isActive() || !m_known)
      return false;
    return m_value->elementKnown(idx);
  }

#define DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(_TYPE_) \
  bool ArrayVariable::getElement(size_t /* idx */, _TYPE_ & /* result */) const \
  { \
    checkPlanError(ALWAYS_FAIL, \
                   "Can't get element of type " << PlexilValueType<_TYPE_>::typeName \
                   << " from a " << valueTypeName(arrayElementType(valueType())) << " array"); \
    return false; \
  }

  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(Boolean)
  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(Integer)
  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(Real)
  DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD(String)

#undef DEFINE_GET_ELEMENT_TYPE_ERROR_METHOD

  bool ArrayVariable::getElementPointer(size_t /* idx */, String const *& /* ptr */) const
  {
    checkPlanError(ALWAYS_FAIL,
                   "Can't get String element from a "
                   << valueTypeName(arrayElementType(valueType())) << " array");
    return false;
  }

  Value ArrayVariable::getElementValue(size_t idx) const
  {
    if (!this->isActive() || !m_known)
      return Value();
    return m_value->getElementValue(idx);
  }

  void ArrayVariable::setElementUnknown(size_t idx)
  {
    if (m_known)
      m_value->setElementUnknown(idx);
    // else fail silently
  }

  bool ArrayVariable::getValuePointer(Array const *&ptr) const
  {
    if (!this->isActive())
      return false;
    if (!m_known)
      return false;
    ptr = m_value;
    return true;
  }

  //
  // Implementation classes
  //


  template <typename T>
  ArrayVariableImpl<T>::ArrayVariableImpl()
    : GetValueImpl<ArrayImpl<T> >(),
    ArrayVariable()
  {
  }

  ArrayVariableImpl<Integer>::ArrayVariableImpl()
    : GetValueImpl<ArrayImpl<Integer> >(),
    ArrayVariable()
  {
  }

  ArrayVariableImpl<String>::ArrayVariableImpl()
    : GetValueImpl<ArrayImpl<String> >(),
    ArrayVariable()
  {
  }

  template <typename T>
  ArrayVariableImpl<T>::ArrayVariableImpl(NodeConnector *node,
                                          char const *name,
                                          Expression *size,
                                          bool sizeIsGarbage)
    : GetValueImpl<ArrayImpl<T> >(),
    ArrayVariable(node, name, size, sizeIsGarbage)
  {
  }

  ArrayVariableImpl<Integer>::ArrayVariableImpl(NodeConnector *node,
                                                char const *name,
                                                Expression *size,
                                                bool sizeIsGarbage)
    : GetValueImpl<ArrayImpl<Integer> >(),
    ArrayVariable(node, name, size, sizeIsGarbage)
  {
  }

  ArrayVariableImpl<String>::ArrayVariableImpl(NodeConnector *node,
                                                char const *name,
                                                Expression *size,
                                                bool sizeIsGarbage)
    : GetValueImpl<ArrayImpl<String> >(),
    ArrayVariable(node, name, size, sizeIsGarbage)
  {
  }

  template <typename T>
  ValueType ArrayVariableImpl<T>::valueType() const
  {
    return PlexilValueType<T>::arrayValue;
  }

  ValueType ArrayVariableImpl<Integer>::valueType() const
  {
    return PlexilValueType<Integer>::arrayValue;
  }

  ValueType ArrayVariableImpl<String>::valueType() const
  {
    return PlexilValueType<String>::arrayValue;
  }

  template <typename T>
  ArrayImpl<T> const *ArrayVariableImpl<T>::typedArrayPointer() const
  {
    if (!m_value)
      return NULL;

    ArrayImpl<T> const *typed_value =
      dynamic_cast<ArrayImpl<T> const *>(m_value); // static_cast?
    assertTrue_2(typed_value, "ArrayVariable internal error: Array is wrong type!");

    return typed_value;
  }

  ArrayImpl<Integer> const *ArrayVariableImpl<Integer>::typedArrayPointer() const
  {
    if (!m_value)
      return NULL;

    ArrayImpl<Integer> const *typed_value =
      dynamic_cast<ArrayImpl<Integer> const *>(m_value); // static_cast?
    assertTrue_2(typed_value, "ArrayVariable internal error: Array is wrong type!");

    return typed_value;
  }

  ArrayImpl<String> const *ArrayVariableImpl<String>::typedArrayPointer() const
  {
    if (!m_value)
      return NULL;

    ArrayImpl<String> const *typed_value =
      dynamic_cast<ArrayImpl<String> const *>(m_value); // static_cast?
    assertTrue_2(typed_value, "ArrayVariable internal error: Array is wrong type!");

    return typed_value;
  }

  // Non-const versions
  template <typename T>
  ArrayImpl<T> *ArrayVariableImpl<T>::typedArrayPointer()
  {
    if (!m_value)
      return NULL;

    ArrayImpl<T> *typed_value =
      dynamic_cast<ArrayImpl<T> *>(m_value); // static_cast?
    assertTrue_2(typed_value, "ArrayVariable internal error: Array is wrong type!");

    return typed_value;
  }

  ArrayImpl<Integer> *ArrayVariableImpl<Integer>::typedArrayPointer()
  {
    if (!m_value)
      return NULL;

    ArrayImpl<Integer> *typed_value =
      dynamic_cast<ArrayImpl<Integer> *>(m_value); // static_cast?
    assertTrue_2(typed_value, "ArrayVariable internal error: Array is wrong type!");

    return typed_value;
  }

  ArrayImpl<String> *ArrayVariableImpl<String>::typedArrayPointer()
  {
    if (!m_value)
      return NULL;

    ArrayImpl<String> *typed_value =
      dynamic_cast<ArrayImpl<String> *>(m_value); // static_cast?
    assertTrue_2(typed_value, "ArrayVariable internal error: Array is wrong type!");

    return typed_value;
  }

  template <typename T>
  bool ArrayVariableImpl<T>::equals(Array const *ary) const
  {
    // Trivial cases first
    if (!m_known)
      return !ary;
    else if (!ary)
      return false;

    ArrayImpl<T> const *typed_value = typedArrayPointer();
    ArrayImpl<T> const *typed_ary =
      dynamic_cast<ArrayImpl<T> const *>(ary);
    if (!typed_ary)
      return false; // different type
    if (m_value->getKnownVector() != ary->getKnownVector())
      return false; // different elements known
    std::vector<T> const *my_contents, *ary_contents;
    typed_value->getContentsVector(my_contents);
    typed_ary->getContentsVector(ary_contents);
    return *my_contents == *ary_contents;
  }

  bool ArrayVariableImpl<Integer>::equals(Array const *ary) const
  {
    // Trivial cases first
    if (!m_known)
      return !ary;
    else if (!ary)
      return false;

    ArrayImpl<Integer> const *typed_value = typedArrayPointer();
    ArrayImpl<Integer> const *typed_ary =
      dynamic_cast<ArrayImpl<Integer> const *>(ary);
    if (!typed_ary)
      return false; // different type
    if (m_value->getKnownVector() != ary->getKnownVector())
      return false; // different elements known
    std::vector<Integer> const *my_contents, *ary_contents;
    typed_value->getContentsVector(my_contents);
    typed_ary->getContentsVector(ary_contents);
    return *my_contents == *ary_contents;
  }

  bool ArrayVariableImpl<String>::equals(Array const *ary) const
  {
    // Trivial cases first
    if (!m_known)
      return !ary;
    else if (!ary)
      return false;

    ArrayImpl<String> const *typed_value = typedArrayPointer();
    ArrayImpl<String> const *typed_ary =
      dynamic_cast<ArrayImpl<String> const *>(ary);
    if (!typed_ary)
      return false; // different type
    if (m_value->getKnownVector() != ary->getKnownVector())
      return false; // different elements known
    std::vector<String> const *my_contents, *ary_contents;
    typed_value->getContentsVector(my_contents);
    typed_ary->getContentsVector(ary_contents);
    return *my_contents == *ary_contents;
  }

  template <typename T>
  Array *ArrayVariableImpl<T>::makeArray(size_t n) const
  {
    return new ArrayImpl<T>(n);
  }

  Array *ArrayVariableImpl<Integer>::makeArray(size_t n) const
  {
    return new ArrayImpl<Integer>(n);
  }

  Array *ArrayVariableImpl<String>::makeArray(size_t n) const
  {
    return new ArrayImpl<String>(n);
  }

  template <typename T>
  bool ArrayVariableImpl<T>::getValuePointer(ArrayImpl<T> const *&ptr) const
  {
    if (!this->isActive())
      return false;
    if (m_known)
      ptr = dynamic_cast<ArrayImpl<T> const *>(m_value); // static_cast?
    return m_known;
  }

  bool ArrayVariableImpl<Integer>::getValuePointer(ArrayImpl<Integer> const *&ptr) const
  {
    if (!this->isActive())
      return false;
    if (m_known)
      ptr = dynamic_cast<ArrayImpl<Integer> const *>(m_value); // static_cast?
    return m_known;
  }

  bool ArrayVariableImpl<String>::getValuePointer(ArrayImpl<String> const *&ptr) const
  {
    if (!this->isActive())
      return false;
    if (m_known)
      ptr = dynamic_cast<ArrayImpl<String> const *>(m_value); // static_cast?
    return m_known;
  }

  template <typename T>
  bool ArrayVariableImpl<T>::getElement(size_t idx, T &result) const
  {
    if (!this->isActive() || !m_known)
      return false;
    return m_value->getElement(idx, result);
  }

  bool ArrayVariableImpl<Integer>::getElement(size_t idx, Integer &result) const
  {
    if (!this->isActive() || !m_known)
      return false;
    return m_value->getElement(idx, result);
  }

  // Conversion method
  bool ArrayVariableImpl<Integer>::getElement(size_t idx, Real &result) const
  {
    if (!this->isActive() || !m_known)
      return false;
    Integer temp;
    if (m_value->getElement(idx, temp)) {
      result = (Real) temp;
      return true;
    }
    else
      return false;
  }

  bool ArrayVariableImpl<String>::getElement(size_t idx, String &result) const
  {
    if (!this->isActive() || !m_known)
      return false;
    return m_value->getElement(idx, result);
  }

  bool ArrayVariableImpl<String>::getElementPointer(size_t idx, String const *&result) const
  {
    if (!this->isActive() || !m_known)
      return false;
    return m_value->getElementPointer(idx, result);
  }

  template <typename T>
  void ArrayVariableImpl<T>::setValueImpl(Array const *a)
  {
    ArrayImpl<T> const *ary = dynamic_cast<ArrayImpl<T> const *>(a);
    checkPlanError(ary,
                   "Assigning wrong type array to " << this->getName());

    bool changed = !m_known;
    size_t newSize = ary->size();
    checkPlanError(!m_size || newSize <= m_maxSize,
                   "New value of array variable " << this->getName()
                   << " is bigger than max size " << m_maxSize);

    if (m_value) {
      // FIXME This isn't quite optimal.
      // If there's a max size and the new value is smaller,
      // we wind up recopying and extending the array,
      // even if the (known) contents are identical.
      ArrayImpl<T> *typed_value = typedArrayPointer();
      if (*ary != *typed_value) {
        *typed_value = *ary;
        changed = true;
      }
    }
    else {
      m_value = ary->clone();
      changed = true;
    }
    m_known = true;
    // TODO: find more efficient way to handle arrays smaller than max
    if (newSize < m_maxSize) {
      m_value->resize(m_maxSize);
    }
    if (changed)
      publishChange();
  }

  void ArrayVariableImpl<Integer>::setValueImpl(Array const *a)
  {
    ArrayImpl<Integer> const *ary = dynamic_cast<ArrayImpl<Integer> const *>(a);
    checkPlanError(ary,
                   "Assigning wrong type array to " << this->getName());

    bool changed = !m_known;
    size_t newSize = ary->size();
    checkPlanError(!m_size || newSize <= m_maxSize,
                   "New value of array variable " << this->getName()
                   << " is bigger than max size " << m_maxSize);

    if (m_value) {
      ArrayImpl<Integer> *typed_value = typedArrayPointer();
      if (*ary != *typed_value) {
        *typed_value = *ary;
        changed = true;
      }
    }
    else {
      m_value = ary->clone();
      changed = true;
    }
    m_known = true;
    // TODO: find more efficient way to handle arrays smaller than max
    if (newSize < m_maxSize) {
      m_value->resize(m_maxSize);
    }
    if (changed)
      publishChange();
  }

  void ArrayVariableImpl<String>::setValueImpl(Array const *a)
  {
    ArrayImpl<String> const *ary = dynamic_cast<ArrayImpl<String> const *>(a);
    checkPlanError(ary,
                   "Assigning wrong type array to " << this->getName());

    bool changed = !m_known;
    size_t newSize = ary->size();
    checkPlanError(!m_size || newSize <= m_maxSize,
                   "New value of array variable " << this->getName()
                   << " is bigger than max size " << m_maxSize);

    if (m_value) {
      ArrayImpl<String> *typed_value = typedArrayPointer();
      if (*ary != *typed_value) {
        *typed_value = *ary;
        changed = true;
      }
    }
    else {
      m_value = ary->clone();
      changed = true;
    }
    m_known = true;
    // TODO: find more efficient way to handle arrays smaller than max
    if (newSize < m_maxSize) {
      m_value->resize(m_maxSize);
    }
    if (changed)
      publishChange();
  }

  // Should only be called when active.
  template <typename T>
  void ArrayVariableImpl<T>::restoreSavedValue()
  {
    bool changed = (m_known != m_savedKnown);
    if (m_known && m_savedKnown
        && !equals(m_savedValue)) {
      changed = true;
      ArrayImpl<T> const *saved =
        dynamic_cast<ArrayImpl<T> const *>(m_savedValue);
      assertTrue_2(saved, "ArrayVariable: saved value is null or wrong type!");
      *typedArrayPointer() = *saved;
    }
    m_known = m_savedKnown;
    if (changed)
      publishChange();
  }

  void ArrayVariableImpl<Integer>::restoreSavedValue()
  {
    bool changed = (m_known != m_savedKnown);
    if (m_known && m_savedKnown
        && !equals(m_savedValue)) {
      changed = true;
      ArrayImpl<Integer> const *saved =
        dynamic_cast<ArrayImpl<Integer> const *>(m_savedValue);
      assertTrue_2(saved, "ArrayVariable: saved value is null or wrong type!");
      *typedArrayPointer() = *saved;
    }
    m_known = m_savedKnown;
    if (changed)
      publishChange();
  }

  void ArrayVariableImpl<String>::restoreSavedValue()
  {
    bool changed = (m_known != m_savedKnown);
    if (m_known && m_savedKnown
        && !equals(m_savedValue)) {
      changed = true;
      ArrayImpl<String> const *saved =
        dynamic_cast<ArrayImpl<String> const *>(m_savedValue);
      assertTrue_2(saved, "ArrayVariable: saved value is null or wrong type!");
      *typedArrayPointer() = *saved;
    }
    m_known = m_savedKnown;
    if (changed)
      publishChange();
  }

  template <typename T>
  void ArrayVariableImpl<T>::setElement(size_t idx, Value const &value)
  {
    ArrayImpl<T> *ary = typedArrayPointer();
    T vtemp, atemp;
    bool vknown, aknown;
    vknown = value.getValue(vtemp);
    aknown = ary->getElement(idx, atemp);
    if (vknown) {
      if (!aknown || vtemp != atemp) {
        ary->setElement(idx, vtemp);
        publishChange();
      }
      // else unchanged
    }
    else if (aknown) {
      m_value->setElementUnknown(idx);
      publishChange();
    }
    // else unchanged
  }

  void ArrayVariableImpl<Integer>::setElement(size_t idx, Value const &value)
  {
    ArrayImpl<Integer> *ary = typedArrayPointer();
    Integer vtemp, atemp;
    bool vknown, aknown;
    vknown = value.getValue(vtemp);
    aknown = ary->getElement(idx, atemp);
    if (vknown) {
      if (!aknown || vtemp != atemp) {
        ary->setElement(idx, vtemp);
        publishChange();
      }
      // else unchanged
    }
    else if (aknown) {
      m_value->setElementUnknown(idx);
      publishChange();
    }
    // else unchanged
  }

  void ArrayVariableImpl<String>::setElement(size_t idx, Value const &value)
  {
    ArrayImpl<String> *ary = typedArrayPointer();
    String const *vtemp;
    String const *atemp;
    bool vknown, aknown;
    vknown = value.getValuePointer(vtemp);
    aknown = ary->getElementPointer(idx, atemp);
    if (vknown) {
      if (!aknown || (*vtemp) != (*atemp)) {
        ary->setElement(idx, *vtemp);
        publishChange();
      }
      // else unchanged
    }
    else if (aknown) {
      m_value->setElementUnknown(idx);
      publishChange();
    }
    // else unchanged
  }

  template class ArrayVariableImpl<Boolean>;
  // template class ArrayVariableImpl<Integer>;
  template class ArrayVariableImpl<Real>;
  // template class ArrayVariableImpl<String>;

} // namespace PLEXIL


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

#include "CachedValueImpl.hh"

#include "ArrayImpl.hh"
#include "Debug.hh"
#include "ExternalInterface.hh"
#include "InterfaceError.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

#include <iomanip>

namespace PLEXIL
{
  // Placeholder object
  VoidCachedValue::VoidCachedValue()
    : CachedValue()
  {
  }

  VoidCachedValue::~VoidCachedValue()
  {
  }


  CachedValue &VoidCachedValue::operator=(CachedValue const &other)
  {
    assertTrueMsg(ALWAYS_FAIL,
                  "This method should never be called");
  }

  ValueType VoidCachedValue::valueType() const
  {
    return UNKNOWN_TYPE;
  }

  bool VoidCachedValue::isKnown() const
  {
    return false;
  }

  CachedValue *VoidCachedValue::clone() const
  {
    return new VoidCachedValue();
  }

  bool VoidCachedValue::operator==(CachedValue const &other) const
  {
    return !other.isKnown();
  }

  Value VoidCachedValue::toValue() const
  {
    return Value();
  }

  void VoidCachedValue::printValue(std::ostream &s) const
  {
    s << "UNKNOWN"; 
  }

  /**
   * @brief Update the cache entry with the given new value.
   * @param timestamp Sequence number.
   * @param val The new value.
   * @note The caller is responsible for deleting the object pointed to upon return.
   */

#define DEFINE_UPDATE_METHOD(_type_)                                    \
  bool VoidCachedValue::update(unsigned int /* timestamp */, _type_ const & /* val */) \
  {                                                                     \
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");        \
    return false;                                                       \
  }

  DEFINE_UPDATE_METHOD(Boolean)
  DEFINE_UPDATE_METHOD(Integer)
  DEFINE_UPDATE_METHOD(Real)
  DEFINE_UPDATE_METHOD(String)

#undef DEFINE_UPDATE_METHOD

#define DEFINE_UPDATE_PTR_METHOD(_type_)                                \
  bool VoidCachedValue::updatePtr(unsigned int /* timestamp */, _type_ const * /* valPtr */) \
  {                                                                     \
    assertTrue_2(ALWAYS_FAIL, "Can't update a VoidCachedValue");        \
    return false;                                                       \
  }

  DEFINE_UPDATE_PTR_METHOD(String)
  DEFINE_UPDATE_PTR_METHOD(BooleanArray)
  DEFINE_UPDATE_PTR_METHOD(IntegerArray)
  DEFINE_UPDATE_PTR_METHOD(RealArray)
  DEFINE_UPDATE_PTR_METHOD(StringArray)

#undef DEFINE_UPDATE_PTR_METHOD
  
  bool VoidCachedValue::update(unsigned int timestamp, Value const &val)
  {
    assertTrue_2(!val.isKnown(), "Can't update a VoidCachedValue");
    this->m_timestamp = timestamp;
    return true;
  }

  //
  // Typed implementation
  //

  //
  // Default constructors
  //
  
  template <typename T>
  CachedValueImpl<T>::CachedValueImpl()
    : CachedValue(),
    m_known(false)
  {
  }

  CachedValueImpl<Integer>::CachedValueImpl()
    : CachedValue(),
    m_known(false)
  {
  }

  CachedValueImpl<Real>::CachedValueImpl()
    : CachedValue(),
    m_known(false)
  {
  }

  CachedValueImpl<String>::CachedValueImpl()
    : CachedValue(),
    m_known(false)
  {
  }

  template <typename T>
  CachedValueImpl<ArrayImpl<T> >::CachedValueImpl()
    : CachedValue(),
    m_known(false)
  {
  }

  //
  // Copy constructors
  //  
  
  template <typename T>
  CachedValueImpl<T>::CachedValueImpl(CachedValueImpl<T> const &orig)
    : CachedValue(orig),
      m_value(orig.m_value),
      m_known(orig.m_known)
  {
  }

  CachedValueImpl<Integer>::CachedValueImpl(CachedValueImpl<Integer> const &orig)
    : CachedValue(orig),
      m_value(orig.m_value),
      m_known(orig.m_known)
  {
  }

  CachedValueImpl<Real>::CachedValueImpl(CachedValueImpl<Real> const &orig)
    : CachedValue(orig),
      m_value(orig.m_value),
      m_known(orig.m_known)
  {
  }

  CachedValueImpl<String>::CachedValueImpl(CachedValueImpl<String> const &orig)
    : CachedValue(orig),
      m_value(orig.m_value),
      m_known(orig.m_known)
  {
  }

  template <typename T>
  CachedValueImpl<ArrayImpl<T> >::CachedValueImpl(CachedValueImpl<ArrayImpl<T> > const &orig)
    : CachedValue(orig),
      m_value(orig.m_value),
      m_known(orig.m_known)
  {
  }

  //
  // Assignment operator
  //

  template <typename T>
  CachedValue &CachedValueImpl<T>::operator=(CachedValue const &other)
  {
    CachedValueImpl<T> const *typedOther =
      dynamic_cast<CachedValueImpl<T> const *>(&other);
    assertTrueMsg(typedOther,
                  "Attempt to assign CachedValue of type " << valueTypeName(other.valueType())
                  << " to CachedValue of type " << valueTypeName(valueType()));
    CachedValue::operator=(other);
    m_value = typedOther->m_value;
    m_known = typedOther->m_known;
    return *this;
  }

  CachedValue &CachedValueImpl<Integer>::operator=(CachedValue const &other)
  {
    CachedValueImpl<Integer> const *typedOther =
      dynamic_cast<CachedValueImpl<Integer> const *>(&other);
    assertTrueMsg(typedOther,
                  "Attempt to assign CachedValue of type " << valueTypeName(other.valueType())
                  << " to CachedValue of type " << valueTypeName(valueType()));
    CachedValue::operator=(other);
    m_value = typedOther->m_value;
    m_known = typedOther->m_known;
    return *this;
  }

  CachedValue &CachedValueImpl<Real>::operator=(CachedValue const &other)
  {
    CachedValueImpl<Real> const *typedOther =
      dynamic_cast<CachedValueImpl<Real> const *>(&other);
    assertTrueMsg(typedOther,
                  "Attempt to assign CachedValue of type " << valueTypeName(other.valueType())
                  << " to CachedValue of type " << valueTypeName(valueType()));
    CachedValue::operator=(other);
    m_value = typedOther->m_value;
    m_known = typedOther->m_known;
    return *this;
  }

  CachedValue &CachedValueImpl<String>::operator=(CachedValue const &other)
  {
    CachedValueImpl<String> const *typedOther =
      dynamic_cast<CachedValueImpl<String> const *>(&other);
    assertTrueMsg(typedOther,
                  "Attempt to assign CachedValue of type " << valueTypeName(other.valueType())
                  << " to CachedValue of type " << valueTypeName(valueType()));
    CachedValue::operator=(other);
    m_value = typedOther->m_value;
    m_known = typedOther->m_known;
    return *this;
  }

  template <typename T>
  CachedValue &CachedValueImpl<ArrayImpl<T> >::operator=(CachedValue const &other)
  {
    CachedValueImpl<ArrayImpl<T> > const *typedOther =
      dynamic_cast<CachedValueImpl<ArrayImpl<T> > const *>(&other);
    assertTrueMsg(typedOther,
                  "Attempt to assign CachedValue of type " << valueTypeName(other.valueType())
                  << " to CachedValue of type " << valueTypeName(valueType()));
    CachedValue::operator=(other);
    m_value = typedOther->m_value;
    m_known = typedOther->m_known;
    return *this;
  }

  //
  // GetValue API
  //

  template <typename T>
  ValueType CachedValueImpl<T>::valueType() const
  {
    return PlexilValueType<T>::value;
  }

  ValueType CachedValueImpl<Integer>::valueType() const
  {
    return INTEGER_TYPE;
  }

  ValueType CachedValueImpl<Real>::valueType() const
  {
    return REAL_TYPE;
  }

  ValueType CachedValueImpl<String>::valueType() const
  {
    return STRING_TYPE;
  }

  template <typename T>
  ValueType CachedValueImpl<ArrayImpl<T> >::valueType() const
  {
    return PlexilValueType<ArrayImpl<T> >::value;
  }

  template <typename T>
  bool CachedValueImpl<T>::isKnown() const
  {
    return m_known;
  }

  bool CachedValueImpl<Integer>::isKnown() const
  {
    return m_known;
  }

  bool CachedValueImpl<Real>::isKnown() const
  {
    return m_known;
  }

  bool CachedValueImpl<String>::isKnown() const
  {
    return m_known;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::isKnown() const
  {
    return m_known;
  }

  template <typename T>
  Value CachedValueImpl<T>::toValue() const
  {
    T temp;
    bool known = this->getValue(temp);
    if (known)
      return Value(temp);
    else
      return Value(0, this->valueType());
  }

  Value CachedValueImpl<Integer>::toValue() const
  {
    Integer temp;
    bool known = this->getValue(temp);
    if (known)
      return Value(temp);
    else
      return Value(0, this->valueType());
  }

  Value CachedValueImpl<Real>::toValue() const
  {
    Real temp;
    bool known = this->getValue(temp);
    if (known)
      return Value(temp);
    else
      return Value(0, this->valueType());
  }

  Value CachedValueImpl<String>::toValue() const
  {
    std::string const *ptr;
    bool known = this->getValuePointer(ptr);
    if (known)
      return Value(*ptr);
    else
      return Value(0, this->valueType());
  }

  template <typename T>
  Value CachedValueImpl<ArrayImpl<T> >::toValue() const
  {
    ArrayImpl<T> const *ptr;
    bool known = this->getValuePointer(ptr);
    if (known)
      return Value(*ptr);
    else
      return Value(0, this->valueType());
  }

  template <typename T>
  bool CachedValueImpl<T>::getValue(T &result) const
  {
    if (m_known)
      result = m_value;
    return m_known;
  }

  bool CachedValueImpl<Integer>::getValue(Integer &result) const
  {
    if (m_known)
      result = m_value;
    return m_known;
  }

  // Conversion method
  bool CachedValueImpl<Integer>::getValue(Real &result) const
  {
    if (m_known)
      result = (Real) m_value;
    return m_known;
  }

  bool CachedValueImpl<Real>::getValue(Real &result) const
  {
    if (m_known)
      result = m_value;
    return m_known;
  }

  bool CachedValueImpl<String>::getValue(String &result) const
  {
    if (m_known)
      result = m_value;
    return m_known;
  }

  bool CachedValueImpl<String>::getValuePointer(std::string const *&ptr) const
  {
    if (m_known)
      ptr = &m_value;
    return m_known;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::getValuePointer(ArrayImpl<T> const *&ptr) const
  {
    if (m_known)
      ptr = &m_value;
    return m_known;
  }

  // Conversion method
  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::getValuePointer(Array const *& ptr) const
  {
    if (m_known)
      ptr = dynamic_cast<Array const *>(&m_value);
    return m_known;
  }

  template <typename T>
  void CachedValueImpl<T>::printValue(std::ostream &s) const
  {
    if (m_known)
      PLEXIL::printValue(m_value, s);
    else
      s << "UNKNOWN"; 
  }

  void CachedValueImpl<Integer>::printValue(std::ostream &s) const
  {
    if (m_known)
      PLEXIL::printValue(m_value, s);
    else
      s << "UNKNOWN"; 
  }

  void CachedValueImpl<Real>::printValue(std::ostream &s) const
  {
    if (m_known)
      PLEXIL::printValue(m_value, s);
    else
      s << "UNKNOWN"; 
  }

  void CachedValueImpl<String>::printValue(std::ostream &s) const
  {
    if (m_known)
      PLEXIL::printValue(m_value, s);
    else
      s << "UNKNOWN"; 
  }

  template <typename T>
  void CachedValueImpl<ArrayImpl<T> >::printValue(std::ostream &s) const
  {
    if (m_known)
      PLEXIL::printValue(m_value, s);
    else
      s << "UNKNOWN"; 
  }

  template <typename T>
  bool CachedValueImpl<T>::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    this->m_timestamp = timestamp;
    return wasKnown;
  }

  bool CachedValueImpl<Integer>::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    this->m_timestamp = timestamp;
    return wasKnown;
  }

  bool CachedValueImpl<Real>::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    this->m_timestamp = timestamp;
    return wasKnown;
  }

  bool CachedValueImpl<String>::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    this->m_timestamp = timestamp;
    return wasKnown;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::setUnknown(unsigned int timestamp)
  {
    bool wasKnown = m_known;
    m_known = false;
    this->m_timestamp = timestamp;
    return wasKnown;
  }

  template <typename T>
  bool CachedValueImpl<T>::operator==(CachedValue const &other) const
  {
    if (!m_known && other.isKnown())
      return true;
    CachedValueImpl<T> const *otherPtr = 
      dynamic_cast<CachedValueImpl<T> const *>(&other);
    if (!otherPtr)
      return false; // different type
    if (m_known != otherPtr->m_known
        || m_value != otherPtr->m_value)
      return false; // different value
    return true;
  }

  bool CachedValueImpl<Integer>::operator==(CachedValue const &other) const
  {
    if (!m_known && other.isKnown())
      return true;
    CachedValueImpl<Integer> const *otherPtr = 
      dynamic_cast<CachedValueImpl<Integer> const *>(&other);
    if (!otherPtr)
      return false; // different type
    if (m_known != otherPtr->m_known
        || m_value != otherPtr->m_value)
      return false; // different value
    return true;
  }

  bool CachedValueImpl<Real>::operator==(CachedValue const &other) const
  {
    if (!m_known && other.isKnown())
      return true;
    CachedValueImpl<Real> const *otherPtr = 
      dynamic_cast<CachedValueImpl<Real> const *>(&other);
    if (!otherPtr)
      return false; // different type
    if (m_known != otherPtr->m_known
        || m_value != otherPtr->m_value)
      return false; // different value
    return true;
  }

  bool CachedValueImpl<String>::operator==(CachedValue const &other) const
  {
    if (!m_known && other.isKnown())
      return true;
    CachedValueImpl<String> const *otherPtr = 
      dynamic_cast<CachedValueImpl<String> const *>(&other);
    if (!otherPtr)
      return false; // different type
    if (m_known != otherPtr->m_known
        || m_value != otherPtr->m_value)
      return false; // different value
    return true;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::operator==(CachedValue const &other) const
  {
    if (!m_known && other.isKnown())
      return true;
    CachedValueImpl<ArrayImpl<T> > const *otherPtr = 
      dynamic_cast<CachedValueImpl<ArrayImpl<T> > const *>(&other);
    if (!otherPtr)
      return false; // different type
    if (m_known != otherPtr->m_known
        || m_value != otherPtr->m_value)
      return false; // different value
    return true;
  }

  template <typename T>
  CachedValue *CachedValueImpl<T>::clone() const
  {
    return new CachedValueImpl<T>(*this);
  }

  CachedValue *CachedValueImpl<Integer>::clone() const
  {
    return new CachedValueImpl<Integer>(*this);
  }

  CachedValue *CachedValueImpl<Real>::clone() const
  {
    return new CachedValueImpl<Real>(*this);
  }

  CachedValue *CachedValueImpl<String>::clone() const
  {
    return new CachedValueImpl<String>(*this);
  }

  template <typename T>
  CachedValue *CachedValueImpl<ArrayImpl<T> >::clone() const
  {
    return new CachedValueImpl<ArrayImpl<T> >(*this);
  }

  // Only implemented for scalar types (and string)
  template <typename T>
  bool CachedValueImpl<T>::update(unsigned int timestamp, T const &val)
  {
    if (!m_known || m_value != val) {
      m_value = val;
      m_known = true;
      this->m_timestamp = timestamp;
      debugMsg("CachedValue:update", " updated to " << val);
      return true;
    }
    debugMsg("CachedValue:update", " value is already " << val << ", not updating");
    return false;
  }

  bool CachedValueImpl<Integer>::update(unsigned int timestamp, Integer const &val)
  {
    if (!m_known || m_value != val) {
      m_value = val;
      m_known = true;
      this->m_timestamp = timestamp;
      debugMsg("CachedValue:update", " updated to " << val);
      return true;
    }
    debugMsg("CachedValue:update", " value is already " << val << ", not updating");
    return false;
  }

  // Real is different for debug printing purposes.
  bool CachedValueImpl<Real>::update(unsigned int timestamp, Real const &val)
  {
    if (!m_known || m_value != val) {
      m_value = val;
      m_known = true;
      this->m_timestamp = timestamp;
      debugMsg("CachedValue:update", " updated to " << std::setprecision(15) << val);
      return true;
    }
    debugMsg("CachedValue:update", " value is already " << val << ", not updating");
    return false;
  }

  // Type conversion method.
  bool CachedValueImpl<Real>::update(unsigned int timestamp, Integer const &val)
  {
    return this->update(timestamp, (Real) val);
  }

  bool CachedValueImpl<String>::update(unsigned int timestamp, String const &val)
  {
    if (!m_known || m_value != val) {
      m_value = val;
      m_known = true;
      this->m_timestamp = timestamp;
      return true;
    }
    return false;
  }

  // From Value
  // Default for scalar types
  template <typename T>
  bool CachedValueImpl<T>::update(unsigned int timestamp, Value const &val)
  {
    T nativeVal;
    if (val.getValue(nativeVal))
      return this->update(timestamp, nativeVal);
    else {
      debugMsg("CachedValue:mismatch",
               " value " << val << "is wrong type for "
               << PlexilValueType<T>::typeName << " lookup");
      return setUnknown(timestamp);
    }
  }

  bool CachedValueImpl<Integer>::update(unsigned int timestamp, Value const &val)
  {
    Integer nativeVal;
    if (val.getValue(nativeVal))
      return this->update(timestamp, nativeVal);
    else {
      debugMsg("CachedValue:mismatch",
               " value " << val << "is wrong type for Integer lookup");
      return setUnknown(timestamp);
    }
  }

  bool CachedValueImpl<Real>::update(unsigned int timestamp, Value const &val)
  {
    Real nativeVal;
    if (val.getValue(nativeVal))
      return this->update(timestamp, nativeVal);
    else {
      debugMsg("CachedValue:mismatch",
               " value " << val << "is wrong type for Real lookup");
      return setUnknown(timestamp);
    }
  }

  // Special case for string
  bool CachedValueImpl<String>::update(unsigned int timestamp, Value const &val)
  {
    std::string const *valPtr;
    if (val.getValuePointer(valPtr))
      return this->updatePtr(timestamp, valPtr);
    else {
      debugMsg("CachedValue:mismatch",
               " value " << val << "is wrong type for String lookup");
      return setUnknown(timestamp);
    }
  }

  // Array method
  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::update(unsigned int timestamp, Value const &val)
  {
    ArrayImpl<T> const *valPtr;
    if (val.getValuePointer(valPtr))
      return this->updatePtr(timestamp, valPtr);
    else {
      debugMsg("CachedValue:mismatch",
               " value " << val << "is wrong type for "
               << PlexilValueType<ArrayImpl<T> >::typeName << " lookup");
      return setUnknown(timestamp);
    }
  }

  //
  // updatePtr
  //

  bool CachedValueImpl<String>::updatePtr(unsigned int timestamp, std::string const *ptr)
  {
    if (!m_known || m_value != *ptr) {
      m_value = *ptr;
      m_known = true;
      this->m_timestamp = timestamp;
      return true;
    }
    return false;
  }

  template <typename T>
  bool CachedValueImpl<ArrayImpl<T> >::updatePtr(unsigned int timestamp, ArrayImpl<T> const *ptr)
  {
    if (!m_known || m_value != *ptr) {
      m_value = *ptr;
      m_known = true;
      this->m_timestamp = timestamp;
      return true;
    }
    return false;
  }

  //
  // Factory
  //

  CachedValue *CachedValueFactory(ValueType vtype)
  {
    switch (vtype) {
    case BOOLEAN_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<Boolean>());

    case INTEGER_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<Integer>());

    case REAL_TYPE:
    case DATE_TYPE: // FIXME
    case DURATION_TYPE: // FIXME
      return static_cast<CachedValue *>(new CachedValueImpl<Real>());

    case STRING_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<String>());

    case BOOLEAN_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<BooleanArray>());

    case INTEGER_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<IntegerArray>());

    case REAL_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<RealArray>());

    case STRING_ARRAY_TYPE:
      return static_cast<CachedValue *>(new CachedValueImpl<StringArray>());

    case UNKNOWN_TYPE:
      return static_cast<CachedValue *>(new VoidCachedValue());

    default:
      assertTrue_2(ALWAYS_FAIL, "CachedValueFactory: Invalid or unimplemented value type");
      return NULL;
    }
  }

  //
  // Explicit instantiation (possibly redundant with factory above)
  //

  template class CachedValueImpl<Boolean>;
  // template class CachedValueImpl<Integer>; // redundant
  // template class CachedValueImpl<Real>;    // redundant
  // template class CachedValueImpl<String>;  // redundant

  template class CachedValueImpl<BooleanArray>;
  template class CachedValueImpl<IntegerArray>;
  template class CachedValueImpl<RealArray>;
  template class CachedValueImpl<StringArray>;

} // namespace PLEXIL

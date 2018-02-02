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

#include "GetValueImpl.hh"

#include "ArrayImpl.hh"
#include "Error.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

namespace PLEXIL
{
  template <typename T>
  ValueType GetValueImpl<T>::valueType() const
  {
    return PlexilValueType<T>::value;
  }

  ValueType GetValueImpl<Integer>::valueType() const
  {
    return PlexilValueType<Integer>::value;
  }

  ValueType GetValueImpl<String>::valueType() const
  {
    return PlexilValueType<String>::value;
  }

  template <typename T>
  ValueType GetValueImpl<ArrayImpl<T> >::valueType() const
  {
    return PlexilValueType<ArrayImpl<T> >::value;
  }

  template <typename T>
  bool GetValueImpl<T>::isKnown() const
  {
    T dummy;
    return this->getValue(dummy);
  }

  bool GetValueImpl<Integer>::isKnown() const
  {
    Integer dummy;
    return this->getValue(dummy);
  }

  bool GetValueImpl<String>::isKnown() const
  {
    String const *dummy;
    return this->getValuePointer(dummy);
  }

  template <typename T>
  bool GetValueImpl<ArrayImpl<T> >::isKnown() const
  {
    ArrayImpl<T> const *dummy;
    return this->getValuePointer(dummy);
  }

  template <typename T>
  Value GetValueImpl<T>::toValue() const
  {
    T temp;
    bool known = this->getValue(temp);
    if (known)
      return Value(temp);
    else
      return Value(0, this->valueType());
  }

  Value GetValueImpl<Integer>::toValue() const
  {
    Integer temp;
    bool known = this->getValue(temp);
    if (known)
      return Value(temp);
    else
      return Value(0, this->valueType());
  }
  
  Value GetValueImpl<String>::toValue() const
  {
    std::string const *ptr;
    bool known = this->getValuePointer(ptr);
    if (known)
      return Value(*ptr);
    else
      return Value(0, this->valueType());
  }

  template <typename T>
  Value GetValueImpl<ArrayImpl<T> >::toValue() const
  {
    ArrayImpl<T> const *ptr;
    bool known = this->getValuePointer(ptr);
    if (known)
      return Value(*ptr);
    else
      return Value(0, this->valueType());
  }

  // Type conversion for Integer
  bool GetValueImpl<Integer>::getValue(Real &result) const
  {
    Integer temp;
    if (this->getValue(temp)) {
      result = (Real) temp;
      return true;
    }
    return false;
  }

  // Downcast default for arrays
  template <typename T>
  bool GetValueImpl<ArrayImpl<T> >::getValuePointer(Array const *&ptr) const
  {
    ArrayImpl<T> const *temp;
    if (!this->getValuePointer(temp))
      return false;
    ptr = static_cast<Array const *>(temp);
    return true;
  }

  template <typename T>
  void GetValueImpl<T>::printValue(std::ostream &s) const
  {
    T val;
    if (this->getValue(val))
      PLEXIL::printValue(val, s);
    else
      s << "UNKNOWN"; 
  }

  void GetValueImpl<Integer>::printValue(std::ostream &s) const
  {
    Integer val;
    if (this->getValue(val))
      PLEXIL::printValue(val, s);
    else
      s << "UNKNOWN"; 
  }

  void GetValueImpl<String>::printValue(std::ostream &s) const
  {
    String const *val;
    if (this->getValuePointer(val))
      PLEXIL::printValue(*val, s);
    else
      s << "UNKNOWN"; 
  }

  template <typename T>
  void GetValueImpl<ArrayImpl<T> >::printValue(std::ostream &s) const
  {
    ArrayImpl<T> const *val;
    if (this->getValuePointer(val))
      PLEXIL::printValue(*val, s);
    else
      s << "UNKNOWN"; 
  }

  // Explicit instantiations
  template class GetValueImpl<Boolean>;
  // template class GetValueImpl<Integer>;
  template class GetValueImpl<Real>;
  // template class GetValueImpl<String>;
  template class GetValueImpl<uint16_t>;
  template class GetValueImpl<BooleanArray>;
  template class GetValueImpl<IntegerArray>;
  template class GetValueImpl<RealArray>;
  template class GetValueImpl<StringArray>;

} // namespace PLEXIL

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

#include "ArrayImpl.hh"
#include "PlanError.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

#include <cstring> // memcpy()

namespace PLEXIL
{

  template <typename T>
  ArrayImpl<T>::ArrayImpl()
    : Array()
  {
  }

  ArrayImpl<String>::ArrayImpl()
    : Array()
  {
  }

  template <typename T>
  ArrayImpl<T>::ArrayImpl(ArrayImpl<T> const &orig)
    : Array(orig),
      m_contents(orig.m_contents)
  {
  }

  ArrayImpl<String>::ArrayImpl(ArrayImpl<String> const &orig)
    : Array(orig),
      m_contents(orig.m_contents)
  {
  }

#if __cplusplus >= 201103L
  template <typename T>
  ArrayImpl<T>::ArrayImpl(ArrayImpl<T> &&orig)
    : Array(orig),
      m_contents(std::move(orig.m_contents))
  {
  }

  ArrayImpl<String>::ArrayImpl(ArrayImpl<String> &&orig)
    : Array(orig),
      m_contents(std::move(orig.m_contents))
  {
  }
#endif

  template <typename T>
  ArrayImpl<T>::ArrayImpl(size_t size)
  : Array(size, false),
    m_contents(size)
  {
  }

  ArrayImpl<String>::ArrayImpl(size_t size)
  : Array(size, false),
    m_contents(size)
  {
  }

  template <typename T>
  ArrayImpl<T>::ArrayImpl(size_t size, T const &initval)
  : Array(size, true),
    m_contents(size, initval)
  {
  }

  ArrayImpl<String>::ArrayImpl(size_t size, String const &initval)
  : Array(size, true),
    m_contents(size, initval)
  {
  }

#if __cplusplus >= 201103L
  template <typename T>
  ArrayImpl<T>::ArrayImpl(std::vector<T> &&initval)
    : Array(initval.size(), true),
    m_contents(std::move(initval))
  {
  }

  ArrayImpl<String>::ArrayImpl(std::vector<String> &&initval)
    : Array(initval.size(), true),
    m_contents(std::move(initval))
  {
  }
#endif

  template <typename T>
  ArrayImpl<T>::ArrayImpl(std::vector<T> const &initval)
    : Array(initval.size(), true),
      m_contents(initval)
  {
  }

  ArrayImpl<String>::ArrayImpl(std::vector<String> const &initval)
    : Array(initval.size(), true),
      m_contents(initval)
  {
  }

  template <typename T>
  ArrayImpl<T>::~ArrayImpl()
  {
  }

  ArrayImpl<String>::~ArrayImpl()
  {
  }

  template <typename T>
  Array *ArrayImpl<T>::clone() const
  {
    return new ArrayImpl<T>(*this);
  }

  Array *ArrayImpl<String>::clone() const
  {
    return new ArrayImpl<String>(*this);
  }

  template <typename T>
  Array &ArrayImpl<T>::operator=(Array const &orig)
  {
    ArrayImpl<T> const *typedOrig =
      dynamic_cast<ArrayImpl<T> const *>(&orig);
    checkPlanError(typedOrig,
                   "Can't assign array of element type " << valueTypeName(orig.getElementType())
                   << " to array of element type " << valueTypeName(getElementType()));
    return this->operator=(*typedOrig);
  }

  Array &ArrayImpl<String>::operator=(Array const &orig)
  {
    ArrayImpl<String> const *typedOrig =
      dynamic_cast<ArrayImpl<String> const *>(&orig);
    checkPlanError(typedOrig,
                   "Can't assign array of element type " << valueTypeName(orig.getElementType())
                   << " to array of element type String");
    return this->operator=(*typedOrig);
  }

#if __cplusplus >= 201103L
  template <typename T>
  Array &ArrayImpl<T>::operator=(Array && orig)
  {
    checkPlanError(dynamic_cast<ArrayImpl<T> *>(&orig),
                   "Can't assign array of element type " << valueTypeName(orig.getElementType())
                   << " to array of element type " << valueTypeName(getElementType()));
    return this->operator=(static_cast<ArrayImpl<T> &&>(orig));
  }

  Array &ArrayImpl<String>::operator=(Array &&orig)
  {
    checkPlanError(dynamic_cast<ArrayImpl<String> *>(&orig),
                   "Can't assign array of element type " << valueTypeName(orig.getElementType())
                   << " to array of element type String");
    return this->operator=(static_cast<ArrayImpl<String> &&>(orig));
  }
#endif
  
  template <typename T>
  ArrayImpl<T> &ArrayImpl<T>::operator=(ArrayImpl<T> const &orig)
  {
    Array::operator=(orig);
    m_contents = orig.m_contents;
    return *this;
  }

  ArrayImpl<String> &ArrayImpl<String>::operator=(ArrayImpl<String> const &orig)
  {
    Array::operator=(orig);
    m_contents = orig.m_contents;
    return *this;
  }

#if __cplusplus >= 201103L
  template <typename T>
  ArrayImpl<T> &ArrayImpl<T>::operator=(ArrayImpl<T> &&orig)
  {
    Array::operator=(orig);
    m_contents = std::move(orig.m_contents);
    return *this;
  }

  ArrayImpl<String> &ArrayImpl<String>::operator=(ArrayImpl<String> &&orig)
  {
    Array::operator=(orig);
    m_contents = std::move(orig.m_contents);
    return *this;
  }
#endif

  template <typename T>
  void ArrayImpl<T>::resize(size_t size)
  {
    Array::resize(size);
    m_contents.resize(size);
  }

  void ArrayImpl<String>::resize(size_t size)
  {
    Array::resize(size);
    m_contents.resize(size);
  }

  template <typename T>
  ValueType ArrayImpl<T>::getElementType() const
  {
    return PlexilValueType<T>::value;
  }

  ValueType ArrayImpl<String>::getElementType() const
  {
    return STRING_TYPE;
  }

  template <typename T>
  Value ArrayImpl<T>::getElementValue(size_t index) const
  {
    if (!(this->checkIndex(index) && this->m_known[index]))
      return Value(); // unknown
    else
      return Value(m_contents[index]);
  }

  Value ArrayImpl<String>::getElementValue(size_t index) const
  {
    if (!(this->checkIndex(index) && this->m_known[index]))
      return Value(); // unknown
    else
      return Value(m_contents[index]);
  }

  template <typename T>
  bool ArrayImpl<T>::getElement(size_t index, T &result) const
  {
    if (!this->checkIndex(index))
      return false;
    if (!this->m_known[index])
      return false;
    result = m_contents[index];
    return true;
  }

  bool ArrayImpl<String>::getElement(size_t index, String &result) const
  {
    if (!this->checkIndex(index))
      return false;
    if (!this->m_known[index])
      return false;
    result = m_contents[index];
    return true;
  }

  bool ArrayImpl<String>::getElementPointer(size_t index, String const *&result) const
  {
    if (!this->checkIndex(index))
      return false;
    if (!this->m_known[index])
      return false;
    result = &m_contents[index];
    return true;
  }

  // Generic equality
  template <typename T>
  bool ArrayImpl<T>::operator==(Array const &other) const
  {
    ArrayImpl<T> const *typedOther = 
      dynamic_cast<ArrayImpl<T> const *>(&other);
    if (!typedOther)
      return false;
    return operator==(*typedOther);
  }

  bool ArrayImpl<String>::operator==(Array const &other) const
  {
    ArrayImpl<String> const *typedOther = 
      dynamic_cast<ArrayImpl<String> const *>(&other);
    if (!typedOther)
      return false;
    return operator==(*typedOther);
  }

  // Specific equality
  template <typename T>
  bool ArrayImpl<T>::operator==(ArrayImpl<T> const &other) const
  {
    if (!(this->getKnownVector() == other.getKnownVector()))
      return false;
    return m_contents == other.m_contents;
  }

  bool ArrayImpl<String>::operator==(ArrayImpl<String> const &other) const
  {
    if (!(this->getKnownVector() == other.getKnownVector()))
      return false;
    return m_contents == other.m_contents;
  }

  template <typename T>
  void ArrayImpl<T>::getContentsVector(std::vector<T> const *&result) const
  {
    result = &m_contents;
  }

  void ArrayImpl<String>::getContentsVector(std::vector<String> const *&result) const
  {
    result = &m_contents;
  }

  template <typename T>
  void ArrayImpl<T>::setElement(size_t index, T const &newval)
  {
    if (!this->checkIndex(index))
      return;
    m_contents[index] = newval;
    this->m_known[index] = true;
  }

  void ArrayImpl<String>::setElement(size_t index, String const &newval)
  {
    if (!this->checkIndex(index))
      return;
    m_contents[index] = newval;
    this->m_known[index] = true;
  }

  template <typename T>
  void ArrayImpl<T>::setElementValue(size_t index, Value const &value)
  {
    if (!this->checkIndex(index))
      return;
    T temp;
    bool known = value.getValue(temp);
    if (known)
      m_contents[index] = temp;
    this->m_known[index] = known;
  }

  // Slight optimization for String
  void ArrayImpl<String>::setElementValue(size_t index, Value const &value)
  {
    if (!this->checkIndex(index))
      return;
    String const *temp;
    bool known = value.getValuePointer(temp);
    if (known)
      m_contents[index] = *temp;
    this->m_known[index] = known;
  }

  template <typename T>
  void ArrayImpl<T>::print(std::ostream &s) const
  {
    s << "#(";

    size_t len = this->size();
    size_t i = 0;

    while (i < len) {
      T temp;
      if (getElement(i, temp))
        printValue<T>(temp, s);
      else
        s << "UNKNOWN";
      if (++i < len)
        s << ' ';
    }
    // Print tail
    s << ')';
  }

  // Slight optimization for String
  void ArrayImpl<String>::print(std::ostream &s) const
  {
    s << "#(";

    size_t len = this->size();
    size_t i = 0;

    while (i < len) {
      String const *temp;
      if (getElementPointer(i, temp))
        printValue(*temp, s);
      else
        s << "UNKNOWN";
      if (++i < len)
        s << ' ';
    }
    // Print tail
    s << ')';
  }

  //
  // Non-member functions
  //

  template <typename T>
  bool operator==(ArrayImpl<T> const &a, ArrayImpl<T> const &b)
  {
    if (!(a.getKnownVector() == b.getKnownVector()))
      return false;
    std::vector<T> const *avec, *bvec;
    a.getContentsVector(avec);
    b.getContentsVector(bvec);
    return *avec == *bvec;
  }

  // Generic
  template <typename T>
  bool operator!=(ArrayImpl<T> const &a, Array const &b)
  {
    return !(a == b);
  }

  // Specific
  template <typename T>
  bool operator!=(ArrayImpl<T> const &a, ArrayImpl<T> const &b)
  {
    return !(a == b);
  }

  template <typename T>
  bool operator<(ArrayImpl<T> const &a, ArrayImpl<T> const &b)
  {
    std::vector<bool> const &aKnownVec = a.getKnownVector();
    std::vector<bool> const &bKnownVec = b.getKnownVector();
    // Shorter is less
    size_t aSize = aKnownVec.size();
    size_t bSize = bKnownVec.size();
    if (aSize < bSize)
      return true;
    if (aSize > bSize)
      return false;

    // Same size
    std::vector<T> const *aVec, *bVec;
    a.getContentsVector(aVec);
    b.getContentsVector(bVec);

    for (size_t i = 0; i < aSize; ++i) {
      // Unknown is less than known
      bool aKnown = aKnownVec[i];
      bool bKnown = bKnownVec[i];
      if (!aKnown && bKnown)
        return true; // unknown < known
      if (aKnown && !bKnown)
        return false;
      if (!aKnown)
        continue; // neither known

      // Compare values
      if ((*aVec)[i] < (*bVec)[i])
        return true;
      if ((*aVec)[i] > (*bVec)[i])
        return false;
    }
    return false; // equal
  }

  // Specialization for bool
  template <>
  bool operator< <Boolean>(ArrayImpl<Boolean> const &a, ArrayImpl<Boolean> const &b)
  {
    std::vector<bool> const &aKnownVec = a.getKnownVector();
    std::vector<bool> const &bKnownVec = b.getKnownVector();
    // Shorter is less
    size_t aSize = aKnownVec.size();
    size_t bSize = bKnownVec.size();
    if (aSize < bSize)
      return true;
    if (aSize > bSize)
      return false;

    // Same size
    std::vector<Boolean> const *aVec, *bVec;
    a.getContentsVector(aVec);
    b.getContentsVector(bVec);

    for (size_t i = 0; i < aSize; ++i) {
      // Unknown is less than known
      bool aKnown = aKnownVec[i];
      bool bKnown = bKnownVec[i];
      if (!aKnown && bKnown)
        return true; // unknown < known
      if (aKnown && !bKnown)
        return false;
      if (!aKnown)
        continue; // neither known

      // False less than true
      if (!(*aVec)[i] && (*bVec)[i])
        return true;
      if ((*aVec)[i] && !(*bVec)[i])
        return false;
    }
    return false; // equal
  }

  template <typename T>
  bool operator<=(ArrayImpl<T> const &a, ArrayImpl<T> const &b)
  {
    return !(b < a);
  }


  template <typename T>
  bool operator>(ArrayImpl<T> const &a, ArrayImpl<T> const &b)
  {
    return b < a;
  }

  template <typename T>
  bool operator>=(ArrayImpl<T> const &a, ArrayImpl<T> const &b)
  {
    return !(a < b);
  }


  template <typename T>
  std::ostream &operator<<(std::ostream &s, ArrayImpl<T> const &ary)
  {
    ary.print(s);
    return s;
  }

  //
  // Serialization/deserialization
  //

  // Internal template
  template <typename T>
  size_t elementSerialSize(T const &o)
  {
    return 0;
  }

  // Internal template
  template <typename T>
  char *serializeElement(T const &o, char *b)
  {
    return NULL;
  }

  // Internal template
  template <typename T>
  char const *deserializeElement(T &o, char const *b)
  {
    return NULL;
  }

  //
  // Boolean
  //

  template <>
  char *serializeElement<Boolean>(Boolean const &o, char *b)
  {
    *b++ = (char) o;
    return b;
  }

  template <>
  char const *deserializeElement<Boolean>(Boolean &o, char const *b)
  {
    o = (Boolean) *b++;
    return b;
  }

  template <>
  size_t elementSerialSize<Boolean>(Boolean const & /* o */)
  {
    return 1;
  }

  //
  // Integer
  //

  template <>
  char *serializeElement<Integer>(Integer const &o, char *b)
  {
    // Store in big-endian format
    *b++ = (char) (0xFF & (o >> 24));
    *b++ = (char) (0xFF & (o >> 16));
    *b++ = (char) (0xFF & (o >> 8));
    *b++ = (char) (0xFF & o);
    return b;
  }

  template <>
  char const *deserializeElement<Integer>(Integer &o, char const *b)
  {
    uint32_t n = ((uint32_t) (unsigned char) *b++) << 8;
    n = (n + (uint32_t) (unsigned char) *b++) << 8;
    n = (n + (uint32_t) (unsigned char) *b++) << 8;
    n = (n + (uint32_t) (unsigned char) *b++);
    o = (Integer) n;
    return b;
  }

  template <>
  size_t elementSerialSize<Integer>(Integer const & /* o */)
  {
    return 4;
  }

  //
  // Real
  //

  template <>
  char *serializeElement<Real>(Real const &o, char *b)
  {
    union {
      Real r;
      uint64_t l;
    };
    r = o;
    // Store in big-endian format
    *b++ = (char) (0xFF & (l >> 56));
    *b++ = (char) (0xFF & (l >> 48));
    *b++ = (char) (0xFF & (l >> 40));
    *b++ = (char) (0xFF & (l >> 32));
    *b++ = (char) (0xFF & (l >> 24));
    *b++ = (char) (0xFF & (l >> 16));
    *b++ = (char) (0xFF & (l >> 8));
    *b++ = (char) (0xFF & l);
    return b;
  }

  template <>
  char const *deserializeElement<Real>(Real &o, char const *b)
  {
    union {
      Real r;
      uint64_t l;
    };
    l = (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++; l = l << 8;
    l += (uint64_t) (unsigned char) *b++;
    o = r;
    return b;
  }

  template <>
  size_t elementSerialSize<Real>(Real const & /* o */)
  {
    return 8;
  }

  //
  // String
  //

  template <>
  char *serializeElement<String>(String const &o, char *b)
  {
    size_t s = o.size();
    if (s > 0xFFFFFF)
      return NULL; // too big

    // Put 3 bytes of size first - std::string may contain embedded NUL
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);
    memcpy(b, o.c_str(), s);
    return b + s;
  }

  template <>
  char const *deserializeElement<String>(String &o, char const *b)
  {
    // Get 3 bytes of size
    size_t s = ((size_t) (unsigned char) *b++) << 8;
    s = (s + (size_t) (unsigned char) *b++) << 8;
    s = s + (size_t) (unsigned char) *b++;

    o.replace(o.begin(), o.end(), b, s);
    return b + s;
  }

  template <>
  size_t elementSerialSize<String>(String const &o)
  {
    return 3 + o.size();
  }

  // Internal function
  // Big-endian by bit, little-endian by byte
  static char *serializeBoolVector(std::vector<bool> const &o, char *b)
  {
    int s = o.size();
    size_t i = 0;
    while (s > 0) {
      uint8_t tmp = 0;
      uint8_t mask = 0x80;
      switch (s) {
      default: // s >= 8
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 7:
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 6:
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 5:
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 4:
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 3:
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 2:
	if (o[i++])
	  tmp |= mask;
	mask = mask >> 1;

      case 1:
	if (o[i++])
	  tmp |= mask;
	break;
      }

      *b++ = tmp;
      s -= 8;
    }
    
    return b;
  }

  // Internal function
  // Read from buffer in big-endian form
  // Presumes vector size has already been set.
  static char const *deserializeBoolVector(std::vector<bool> &o, char const *b)
  {
    int s = o.size();
    size_t i = 0;
    while (s > 0) {
      uint8_t tmp = *b++;
      uint8_t mask = 0x80;
      switch (s) {
      default: // s >= 8
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 7:
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 6:
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 5:
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 4:
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 3:
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 2:
	o[i++] = (tmp & mask) ? true : false;
	mask = mask >> 1;

      case 1:
	o[i++] = (tmp & mask) ? true : false;
	break;
      }
      s -= 8;
    }
    return b;
  }

  // Internal function
  static size_t bitVectorSize(size_t n)
  {
    size_t result = n / 8; // integer division, rounds towards 0
    if (n % 8)
      result++;
    return result;
  }

  /**
   * @brief Write a binary version of the object to the given buffer.
   * @param o The object.
   * @param b Pointer to the insertion point in the buffer.
   * @return Pointer to first byte after the object; NULL if failed.
   */

  template <typename T>
  char *ArrayImpl<T>::serialize(char *b) const
  {
    size_t s = this->size();
    if (s > 0xFFFFFF)
      return NULL; // too big to serialize

    // Write type code
    *b++ = (char) PlexilValueType<T>::arrayValue;

    // Write 3 bytes of size
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);

    // Write known vector
    b = serializeBoolVector(this->m_known, b);

    // Write array contents
    for (size_t i = 0; i < s; ++i) {
      b = serializeElement(m_contents[i], b);
      if (!b)
        return NULL; // serializeElement failed
    }
    return b;
  }

  template <>
  char *ArrayImpl<Boolean>::serialize(char *b) const
  {
    size_t s = this->size();
    if (s > 0xFFFFFF)
      return NULL; // too big to serialize

    // Write type code
    *b++ = BOOLEAN_ARRAY_TYPE;

    // Write 3 bytes of size
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);

    // Write known vector
    b = serializeBoolVector(this->m_known, b);

    // Write array contents
    b = serializeBoolVector(m_contents, b);

    return b;
  }

  char *ArrayImpl<String>::serialize(char *b) const
  {
    size_t s = this->size();
    if (s > 0xFFFFFF)
      return NULL; // too big to serialize

    // Write type code
    *b++ = (char) PlexilValueType<String>::arrayValue;

    // Write 3 bytes of size
    *b++ = (char) (0xFF & (s >> 16));
    *b++ = (char) (0xFF & (s >> 8));
    *b++ = (char) (0xFF & s);

    // Write known vector
    b = serializeBoolVector(this->m_known, b);

    // Write array contents
    for (size_t i = 0; i < s; ++i) {
      b = serializeElement(m_contents[i], b);
      if (!b)
        return NULL; // serializeElement failed
    }
    return b;
  }

  /**
   * @brief Read a binary representation from the buffer and store it to the result object.
   * @param o The result object.
   * @param b Pointer to the representation in the buffer.
   * @return Pointer to first byte after the object; NULL if failed.
   */

  // General case
  template <typename T>
  char const *ArrayImpl<T>::deserialize(char const *b)
  {
    // Check type code
    if (PlexilValueType<T>::arrayValue != (ValueType) *b++)
      return NULL; // not an appropriate array

    // Get 3 bytes of size
    size_t s = (size_t) *b++; s = s << 8;
    s += (size_t) *b++; s = s << 8;
    s += (size_t) *b++;
    
    this->resize(s);
    
    b = deserializeBoolVector(this->m_known, b);
    for (size_t i = 0; i < s; ++i)
      b = deserializeElement(m_contents[i], b);

    return b;
  }

  // Special case for Boolean
  template <>
  char const *ArrayImpl<Boolean>::deserialize(char const *b)
  {
    if (BOOLEAN_ARRAY_TYPE != (ValueType) *b++)
      return NULL; // not a Boolean array

    // Get 3 bytes of size
    size_t s = (size_t) *b++; s = s << 8;
    s += (size_t) *b++; s = s << 8;
    s += (size_t) *b++;
    this->resize(s);
    
    b = deserializeBoolVector(this->m_known, b);
    b = deserializeBoolVector(m_contents, b);
    
    return b;
  }

  char const *ArrayImpl<String>::deserialize(char const *b)
  {
    // Check type code
    if (PlexilValueType<String>::arrayValue != (ValueType) *b++)
      return NULL; // not an appropriate array

    // Get 3 bytes of size
    size_t s = (size_t) *b++; s = s << 8;
    s += (size_t) *b++; s = s << 8;
    s += (size_t) *b++;
    
    this->resize(s);
    
    b = deserializeBoolVector(this->m_known, b);
    for (size_t i = 0; i < s; ++i)
      b = deserializeElement(m_contents[i], b);

    return b;
  }

  /**
   * @brief Calculate the size of the serial representation of the object.
   * @param o The object.
   * @return Number of bytes; 0 if the object is not serializable.
   */

  // Numeric case
  template <typename NUM>
  size_t ArrayImpl<NUM>::serialSize() const
  {
    NUM const dummy = 0;
    size_t s = this->size();
    return 4 + bitVectorSize(s) + s * elementSerialSize(dummy);
  }

  template <>
  size_t ArrayImpl<Boolean>::serialSize() const
  {
    return 4 + 2 * bitVectorSize(this->size());
  }

  // Requires traversing entire array
  size_t ArrayImpl<String>::serialSize() const
  {
    size_t s = this->size();
    size_t result = 4 + bitVectorSize(s);
    for (size_t i = 0; i < s; ++i)
      result += 3 + m_contents[i].size();
    return result;
  }

  //
  // Wrappers
  //

  // Seems template functions can't be partially specialized. Fooey.

#define DEF_ARRAY_SERDES_WRAPPERS(typ) \
  template <> char *serialize(ArrayImpl<typ> const &o, char *b)	\
  {return o.serialize(b);} \
\
  template <> char const *deserialize(ArrayImpl<typ> &o, char const *b) \
  {return o.deserialize(b);} \
\
  template <> size_t serialSize(ArrayImpl<typ> const &o) \
  {return o.serialSize();}

  DEF_ARRAY_SERDES_WRAPPERS(Boolean)
  DEF_ARRAY_SERDES_WRAPPERS(Integer)
  DEF_ARRAY_SERDES_WRAPPERS(Real)
  DEF_ARRAY_SERDES_WRAPPERS(String)

#undef DEF_ARRAY_SERDES_WRAPPERS

  //
  // Explicit instantiations
  //
  template class ArrayImpl<Boolean>;
  template class ArrayImpl<Integer>;
  template class ArrayImpl<Real>;
  // template class ArrayImpl<String>;

  template bool operator!=(ArrayImpl<Boolean> const &, ArrayImpl<Boolean> const &);
  template bool operator!=(ArrayImpl<Integer> const &, ArrayImpl<Integer> const &);
  template bool operator!=(ArrayImpl<Real> const &,    ArrayImpl<Real> const &);
  template bool operator!=(ArrayImpl<String> const &,  ArrayImpl<String> const &);

  // Explicitly defined above
  // template bool operator<(ArrayImpl<Boolean> const &, ArrayImpl<Boolean> const &);
  template bool operator<(ArrayImpl<Integer> const &, ArrayImpl<Integer> const &);
  template bool operator<(ArrayImpl<Real> const &,    ArrayImpl<Real> const &);
  template bool operator<(ArrayImpl<String> const &,  ArrayImpl<String> const &);

  template bool operator<=(ArrayImpl<Boolean> const &, ArrayImpl<Boolean> const &);
  template bool operator<=(ArrayImpl<Integer> const &, ArrayImpl<Integer> const &);
  template bool operator<=(ArrayImpl<Real> const &,    ArrayImpl<Real> const &);
  template bool operator<=(ArrayImpl<String> const &,  ArrayImpl<String> const &);

  template bool operator>(ArrayImpl<Boolean> const &, ArrayImpl<Boolean> const &);
  template bool operator>(ArrayImpl<Integer> const &, ArrayImpl<Integer> const &);
  template bool operator>(ArrayImpl<Real> const &,    ArrayImpl<Real> const &);
  template bool operator>(ArrayImpl<String> const &,  ArrayImpl<String> const &);

  template bool operator>=(ArrayImpl<Boolean> const &, ArrayImpl<Boolean> const &);
  template bool operator>=(ArrayImpl<Integer> const &, ArrayImpl<Integer> const &);
  template bool operator>=(ArrayImpl<Real> const &,    ArrayImpl<Real> const &);
  template bool operator>=(ArrayImpl<String> const &,  ArrayImpl<String> const &);

  template std::ostream &operator<<(std::ostream &s, ArrayImpl<Boolean> const &);
  template std::ostream &operator<<(std::ostream &s, ArrayImpl<Integer> const &);
  template std::ostream &operator<<(std::ostream &s, ArrayImpl<Real> const &);
  template std::ostream &operator<<(std::ostream &s, ArrayImpl<String> const &);

} // namespace PLEXIL

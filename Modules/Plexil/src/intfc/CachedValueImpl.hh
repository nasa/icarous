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

#ifndef PLEXIL_CACHED_VALUE_IMPL_HH
#define PLEXIL_CACHED_VALUE_IMPL_HH

#include "CachedValue.hh"

namespace PLEXIL
{

  // Placeholder object
  class VoidCachedValue : public CachedValue
  {
  public:
    VoidCachedValue();
    ~VoidCachedValue();

    CachedValue &operator=(CachedValue const &other);

    ValueType valueType() const;
    bool isKnown() const;
    bool operator==(CachedValue const &) const;
    virtual CachedValue *clone() const;
    void printValue(std::ostream &) const;

    /**
     * @brief Retrieve the cached value.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown or invalid.
     * @note The expression value is not copied if the return value is false.
     * @note Derived classes should implement only the appropriate methods.
     */

    // Local macro
#define DEFINE_VOID_CACHED_VALUE_GET_VALUE_METHOD(_rtype_) \
    virtual bool getValue(_rtype_ &) const \
    { return false; }

    DEFINE_VOID_CACHED_VALUE_GET_VALUE_METHOD(Boolean)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_METHOD(Integer)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_METHOD(Real)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_METHOD(String)
#undef DEFINE_VOID_CACHED_VALUE_GET_VALUE_METHOD

    /**
     * @brief Retrieve a pointer to the (const) cached value.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown or invalid.
     * @note The pointer is not copied if the return value is false.
     * @note Derived classes should implement only the appropriate method.
     * @note Default methods return an error in every case.
     */

    // Local macro
#define DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(_rtype_) \
    virtual bool getValuePointer(_rtype_ const *&) const \
    { return false; }

    DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(String)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(Array)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(BooleanArray)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(IntegerArray)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(RealArray)
    DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD(StringArray)

#undef DEFINE_VOID_CACHED_VALUE_GET_VALUE_POINTER_METHOD

    virtual Value toValue() const;

    /**
     * @brief Set the state to unknown.

     */
    virtual bool setUnknown(unsigned int timestamp)
    { return false; }
      

    /**
     * @brief Update the cache entry with the given new value.
     * @param val The new value.
     * @return True if correct type for lookup, false otherwise.
     * @note Notifies all lookups of the new value.
     * @note The caller is responsible for deleting the object pointed to upon return.
     */
    virtual bool update(unsigned int timestamp, Boolean const &val);
    virtual bool update(unsigned int timestamp, Integer const &val);
    virtual bool update(unsigned int timestamp, Real const &val);
    virtual bool update(unsigned int timestamp, String const &val);
    virtual bool updatePtr(unsigned int timestamp, String const *valPtr);
    virtual bool updatePtr(unsigned int timestamp, BooleanArray const *valPtr);
    virtual bool updatePtr(unsigned int timestamp, IntegerArray const *valPtr);
    virtual bool updatePtr(unsigned int timestamp, RealArray const *valPtr);
    virtual bool updatePtr(unsigned int timestamp, StringArray const *valPtr);

    // For convenience of TestExternalInterface, others
    virtual bool update(unsigned int timestamp, Value const &val);

  private:
    // Not implemented
    VoidCachedValue(VoidCachedValue const &);

  };

  // Scalar types
  template <typename T>
  class CachedValueImpl : public CachedValue
  {
  public:
    CachedValueImpl();
    CachedValueImpl(CachedValueImpl<T> const &);
    ~CachedValueImpl() {}

    CachedValue &operator=(CachedValue const &other);

    virtual CachedValue *clone() const;

    bool operator==(CachedValue const &) const;

    /**
     * @brief Return the value type.
     * @return A constant enumeration.
     * @note May be overridden by derived classes.
     */
    ValueType valueType() const;

    /**
     * @brief Determine whether the value is known or unknown.
     * @return True if known, false otherwise.
     * @note May be overridden by derived classes.
     */
    bool isKnown() const;

    /**
     * @brief Get the value of this expression as a Value instance.
     * @return The Value instance.
     */
    Value toValue() const;

    /**
     * @brief Retrieve the value of this object in its native type.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown.
     */
    virtual bool getValue(T &result) const;

    void printValue(std::ostream &s) const;

    //
    // API to external interface
    //
    
    bool setUnknown(unsigned int timestamp);

    virtual bool update(unsigned int timestamp, T const &val);

    virtual bool update(unsigned int timestamp, Value const &val);

  private:
    CachedValueImpl<T> &operator=(CachedValueImpl<T> const &);

    T m_value;
    bool m_known;
  };

  // Integer specialized to support getValue(Real &) conversion method
  template <>
  class CachedValueImpl<Integer> : public CachedValue
  {
  public:
    CachedValueImpl();
    CachedValueImpl(CachedValueImpl<Integer> const &);
    ~CachedValueImpl() {}

    CachedValue &operator=(CachedValue const &other);

    virtual CachedValue *clone() const;
    bool operator==(CachedValue const &) const;
    ValueType valueType() const;
    bool isKnown() const;
    Value toValue() const;
    virtual bool getValue(Integer &result) const;
    virtual bool getValue(Real &result) const;

    void printValue(std::ostream &s) const;

    bool setUnknown(unsigned int timestamp);
    virtual bool update(unsigned int timestamp, Integer const &val);
    virtual bool update(unsigned int timestamp, Value const &val);

  private:
    CachedValueImpl<Integer> &operator=(CachedValueImpl<Integer> const &);

    Integer m_value;
    bool m_known;
  };

  // Real specialized to support update(unsigned int, Integer const &) conversion method
  template <>
  class CachedValueImpl<Real> : public CachedValue
  {
  public:
    CachedValueImpl();
    CachedValueImpl(CachedValueImpl<Real> const &);
    ~CachedValueImpl() {}

    CachedValue &operator=(CachedValue const &other);

    virtual CachedValue *clone() const;
    bool operator==(CachedValue const &) const;
    ValueType valueType() const;
    bool isKnown() const;
    Value toValue() const;
    virtual bool getValue(Real &result) const;

    void printValue(std::ostream &s) const;

    bool setUnknown(unsigned int timestamp);
    virtual bool update(unsigned int timestamp, Integer const &val);
    virtual bool update(unsigned int timestamp, Real const &val);
    virtual bool update(unsigned int timestamp, Value const &val);

  private:
    CachedValueImpl<Real> &operator=(CachedValueImpl<Real> const &);

    Real m_value;
    bool m_known;
  };

  // String is special
  template <>
  class CachedValueImpl<String> : public CachedValue
  {
  public:
    CachedValueImpl();
    CachedValueImpl(CachedValueImpl<String> const &);
    ~CachedValueImpl() {}

    CachedValue &operator=(CachedValue const &other);

    virtual CachedValue *clone() const;
    bool operator==(CachedValue const &) const;
    ValueType valueType() const;
    bool isKnown() const;
    Value toValue() const;
    virtual bool getValue(String &result) const;
    virtual bool getValuePointer(String const *&) const;

    void printValue(std::ostream &s) const;

    bool setUnknown(unsigned int timestamp);
    virtual bool update(unsigned int timestamp, String const &val);
    virtual bool update(unsigned int timestamp, Value const &val);
    virtual bool updatePtr(unsigned int timestamp, String const *valPtr);

  private:
    CachedValueImpl<String> &operator=(CachedValueImpl<String> const &);

    String m_value;
    bool m_known;
  };

  // Specialized for arrays
  template <typename T>
  class CachedValueImpl<ArrayImpl<T> > : public CachedValue
  {
  public:
    CachedValueImpl();
    CachedValueImpl(CachedValueImpl<ArrayImpl<T> > const &);
    ~CachedValueImpl() {}

    CachedValue &operator=(CachedValue const &other);

    virtual CachedValue *clone() const;
    bool operator==(CachedValue const &) const;
    ValueType valueType() const;
    bool isKnown() const;
    Value toValue() const;

    virtual bool getValuePointer(Array const *&) const;
    virtual bool getValuePointer(ArrayImpl<T> const *&) const;

    void printValue(std::ostream &s) const;
    
    bool setUnknown(unsigned int timestamp);
    bool updatePtr(unsigned int timestamp, ArrayImpl<T> const *valPtr);
    bool update(unsigned int timestamp, Value const &val);

  private:
    CachedValueImpl<ArrayImpl<T> > &operator=(CachedValueImpl<ArrayImpl<T> > const &);

    ArrayImpl<T> m_value;
    bool m_known;
  };

} // namespace PLEXIL

#endif // PLEXIL_CACHED_VALUE_IMPL_HH

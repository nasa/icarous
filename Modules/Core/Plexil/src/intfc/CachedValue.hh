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

#ifndef PLEXIL_CACHED_VALUE_HH
#define PLEXIL_CACHED_VALUE_HH

#include "ArrayFwd.hh"
#include "ValueType.hh"

namespace PLEXIL
{
  class Value;

  /**
   * Abstract base class for storing state cache values
   */
  class CachedValue
  {
  public:
    CachedValue() : m_timestamp(0) {}
    CachedValue(CachedValue const &orig) : m_timestamp(orig.m_timestamp) {}
    virtual ~CachedValue() {}

    virtual CachedValue &operator=(CachedValue const &orig)
    {
      m_timestamp = orig.m_timestamp;
      return *this;
    }

    unsigned int getTimestamp() const
    {
      return m_timestamp;
    }

    // Delegated to derived classes.
    virtual CachedValue *clone() const = 0;
    virtual bool operator==(CachedValue const &) const = 0;

    /**
     * @brief Set the state to unknown.
     * @param timestamp Sequence number.
     * @return True if changed, false otherwise.
     */
    virtual bool setUnknown(unsigned int timestamp) = 0;
    
    /**
     * @brief Return the value type.
     * @return A constant enumeration.
     * @note May be overridden by derived classes.
     */
    virtual ValueType valueType() const = 0;

    /**
     * @brief Determine whether the value is known or unknown.
     * @return True if known, false otherwise.
     * @note May be overridden by derived classes.
     */
    virtual bool isKnown() const = 0;

    /**
     * @brief Get the value of this object as a Value instance.
     * @return The Value instance.
     */
    virtual Value toValue() const = 0;

    /**
     * @brief Print the object's value to the given stream.
     * @param s The output stream.
     */
    virtual void printValue(std::ostream& s) const = 0;

    /**
     * @brief Retrieve the value of this object.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown or invalid.
     * @note The value is not copied if the return value is false.
     * @note Derived classes should implement only the appropriate methods.
     * @note Default methods report a type error.
     */

    virtual bool getValue(Boolean &result) const;
    virtual bool getValue(Integer &result) const;
    virtual bool getValue(Real &result) const;
    virtual bool getValue(String &result) const;

    /**
     * @brief Retrieve a pointer to the (const) value of this object.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown or invalid.
     * @note The pointer is not copied if the return value is false.
     * @note Derived classes should implement only the appropriate methods.
     * @note Default methods report a type error.
     */

    virtual bool getValuePointer(String const *&ptr) const;

    virtual bool getValuePointer(Array const *&ptr) const;

    virtual bool getValuePointer(BooleanArray const *&ptr) const;
    virtual bool getValuePointer(IntegerArray const *&ptr) const;
    virtual bool getValuePointer(RealArray const *&ptr) const;
    virtual bool getValuePointer(StringArray const *&ptr) const;

    /**
     * @brief Update the cache entry with the given new value.
     * @param timestamp Sequence number.
     * @param val The new value.
     * @return True if changed, false otherwise.
     * @note The caller is responsible for deleting the object pointed to upon return.
     * @note Default methods report a type error.
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
    virtual bool update(unsigned int timestamp, Value const &val) = 0;

  protected:
    unsigned int m_timestamp;
  };

  // Factory function
  // See CachedValueImpl.cc
  extern CachedValue *CachedValueFactory(ValueType vtype);

  extern CachedValue *cloneCachedValue(CachedValue const * orig);

}

#endif // PLEXIL_CACHED_VALUE_HH

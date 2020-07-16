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

#ifndef PLEXIL_ARRAY_IMPL_HH
#define PLEXIL_ARRAY_IMPL_HH

#include "Array.hh"

namespace PLEXIL
{

  //
  // ArrayImpl
  //
  template <typename T>
  class ArrayImpl : public Array
  {
  public:
    ArrayImpl();
    ArrayImpl(ArrayImpl const &);
#if __cplusplus >= 201103L
    ArrayImpl(ArrayImpl &&);
#endif
    ArrayImpl(size_t size);
    ArrayImpl(size_t size, T const &initval);
    ArrayImpl(std::vector<T> const &initval);
#if __cplusplus >= 201103L
    ArrayImpl(std::vector<T> &&initval);
#endif
    
    ~ArrayImpl();

    virtual Array *clone() const;

    virtual Array &operator=(Array const &);
#if __cplusplus >= 201103L
    virtual Array &operator=(Array &&);
#endif

    virtual ArrayImpl &operator=(ArrayImpl<T> const &);
#if __cplusplus >= 201103L
    ArrayImpl &operator=(ArrayImpl<T> &&);
#endif

    virtual void resize(size_t size);

    // Generic accessors
    virtual ValueType getElementType() const;
    virtual Value getElementValue(size_t index) const;

    virtual bool operator==(Array const &other) const;
    bool operator==(ArrayImpl<T> const &other) const;

    // Generic setter
    virtual void setElementValue(size_t index, Value const &value);

    // Typed accessors
    virtual bool getElement(size_t index, T &result) const;

    void getContentsVector(std::vector<T> const *&result) const;

    virtual void setElement(size_t index, T const &newVal);

    virtual void print(std::ostream &s) const;

    virtual char *serialize(char *b) const; 
    virtual char const *deserialize(char const *b);
    virtual size_t serialSize() const; 

  private:
    std::vector<T> m_contents;
  };

  //
  // String is special because of getElementPointer()
  // So we have to duplicate every blasted member function.
  //

  template <>
  class ArrayImpl<String> : public Array
  {
  public:
    ArrayImpl();
    ArrayImpl(ArrayImpl const &);
#if __cplusplus >= 201103L
    ArrayImpl(ArrayImpl &&);
#endif
    ArrayImpl(size_t size);
    ArrayImpl(size_t size, String const &initval);
    ArrayImpl(std::vector<String> const &initval);
#if __cplusplus >= 201103L
    ArrayImpl(std::vector<String> &&initval);
#endif

    ~ArrayImpl();

    virtual Array *clone() const;

    virtual Array &operator=(Array const &);
#if __cplusplus >= 201103L
    virtual Array &operator=(Array &&);
#endif

    virtual ArrayImpl &operator=(ArrayImpl<String> const &);
#if __cplusplus >= 201103L
    ArrayImpl &operator=(ArrayImpl<String> &&);
#endif

    virtual void resize(size_t size);

    // Generic accessors
    virtual ValueType getElementType() const;
    virtual Value getElementValue(size_t index) const;

    virtual bool operator==(Array const &other) const;
    bool operator==(ArrayImpl<String> const &other) const;

    // Generic setter
    virtual void setElementValue(size_t index, Value const &value);

    // Typed accessors
    virtual bool getElement(size_t index, String &result) const;

    virtual bool getElementPointer(size_t index, String const *&result) const;

    void getContentsVector(std::vector<String> const *&result) const;

    virtual void setElement(size_t index, String const &newVal);

    virtual void print(std::ostream &s) const;

    virtual char *serialize(char *b) const; 
    virtual char const *deserialize(char const *b);
    virtual size_t serialSize() const; 

  private:
    std::vector<String> m_contents;
  };

  template <typename T>
  bool operator!=(ArrayImpl<T> const &, ArrayImpl<T> const &);

  template <typename T>
  bool operator<(ArrayImpl<T> const &, ArrayImpl<T> const &);

  template <typename T>
  bool operator<=(ArrayImpl<T> const &, ArrayImpl<T> const &);

  template <typename T>
  bool operator>(ArrayImpl<T> const &, ArrayImpl<T> const &);

  template <typename T>
  bool operator>=(ArrayImpl<T> const &, ArrayImpl<T> const &);

  template <typename T>
  std::ostream &operator<<(std::ostream &, ArrayImpl<T> const &);

} // namespace PLEXIL

#endif // PLEXIL_ARRAY_IMPL_HH

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

#ifndef PLEXIL_OPERATOR_IMPL_HH
#define PLEXIL_OPERATOR_IMPL_HH

#include "Operator.hh"

#include "PlanError.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

namespace PLEXIL
{

  template <typename R>
  class OperatorImpl : public Operator
  {
  public:
    virtual ~OperatorImpl() {}

    virtual bool operator()(R &result, Expression const *arg) const;
    virtual bool operator()(R &result, Expression const *arg0, Expression const *arg1) const;
    virtual bool operator()(R &result, Function const &args) const;

    // Default methods, based on R
    ValueType valueType() const;
    void *allocateCache() const;
    void deleteCache(void *ptr) const;

    bool calcNative(void *cache, Function const &exprs) const;
    void printValue(std::ostream &s, void *cache, Function const &exprs) const;
    Value toValue(void *cache, Function const &exprs) const;

    // Delegated to derived classes
    // Default methods issue "wrong argument count" error
    virtual bool calc(R &result, Expression const *arg) const;
    virtual bool calc(R &result, Expression const *arg0, Expression const *arg1) const;
    virtual bool calc(R &result, Function const &args) const;

  protected:
    OperatorImpl(std::string const &name);

  private:
    // Unimplemented
    OperatorImpl();
    OperatorImpl(OperatorImpl const &);
    OperatorImpl &operator=(OperatorImpl const &);
  };

  // Specialized conversions for Integer operator to Real
  template <>
  class OperatorImpl<Integer> : public Operator
  {
  public:
    virtual ~OperatorImpl() {}

    virtual bool operator()(Integer &result, Expression const *arg) const;
    virtual bool operator()(Integer &result, Expression const *arg0, Expression const *arg1) const;
    virtual bool operator()(Integer &result, Function const &args) const;

    // Conversion methods
    virtual bool operator()(Real &result, Expression const *arg) const;
    virtual bool operator()(Real &result, Expression const *arg0, Expression const *arg1) const;
    virtual bool operator()(Real &result, Function const &args) const;

    ValueType valueType() const;
    void *allocateCache() const;
    void deleteCache(void *ptr) const;

    bool calcNative(void *cache, Function const &exprs) const;
    void printValue(std::ostream &s, void *cache, Function const &exprs) const;
    Value toValue(void *cache, Function const &exprs) const;

    virtual bool calc(Integer &result, Expression const *arg) const;
    virtual bool calc(Integer &result, Expression const *arg0, Expression const *arg1) const;
    virtual bool calc(Integer &result, Function const &args) const;

  protected:
    OperatorImpl(std::string const &name);

  private:
    // Unimplemented
    OperatorImpl();
    OperatorImpl(OperatorImpl const &);
    OperatorImpl &operator=(OperatorImpl const &);
  };

  template <typename R>
  class OperatorImpl<ArrayImpl<R> > : public Operator
  {
  public:
    virtual ~OperatorImpl() {}

    virtual bool operator()(ArrayImpl<R> &result, Expression const *arg) const;
    virtual bool operator()(ArrayImpl<R> &result, Expression const *arg0, Expression const *arg1) const;
    virtual bool operator()(ArrayImpl<R> &result, Function const &args) const;

    // Default methods, based on R
    ValueType valueType() const;
    void *allocateCache() const;
    void deleteCache(void *ptr) const;

    bool calcNative(void *cache, Function const &exprs) const;
    void printValue(std::ostream &s, void *cache, Function const &exprs) const;
    Value toValue(void *cache, Function const &exprs) const;

    // Delegated to derived classes
    virtual bool calc(ArrayImpl<R> &result, Expression const *arg) const = 0;
    virtual bool calc(ArrayImpl<R> &result, Expression const *arg0, Expression const *arg1) const = 0;
    virtual bool calc(ArrayImpl<R> &result, Function const &args) const = 0;

  protected:
    // Base class shouldn't be instantiated by itself
    OperatorImpl(std::string const &name);

  private:
    // Unimplemented
    OperatorImpl();
    OperatorImpl(OperatorImpl const &);
    OperatorImpl &operator=(OperatorImpl const &);
  };

} // namespace PLEXIL

/**
 * @brief Helper macro, intended to implement "boilerplate" singleton accessors
 *        for classes derived from OperatorImpl<R>.
 */
#define DECLARE_OPERATOR_STATIC_INSTANCE(CLASS, RETURNS) \
  static PLEXIL::Operator const *instance() \
  { \
    static CLASS const sl_instance; \
    return static_cast<PLEXIL::Operator const *>(&sl_instance); \
  }

#endif // PLEXIL_OPERATOR_IMPL_HH

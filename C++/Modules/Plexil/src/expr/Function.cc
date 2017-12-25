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

#include "Function.hh"

#include "ArrayImpl.hh"
#include "Error.hh"
#include "Operator.hh"
#include "Value.hh"

namespace PLEXIL
{
  Function::Function(Operator const *op)
    : NotifierImpl(),
      m_op(op),
      m_valueCache(op->allocateCache())
  {
  }

  Function::~Function()
  {
    if (m_op && m_valueCache)
      m_op->deleteCache(m_valueCache);
  }

  const char *Function::exprName() const
  {
    return m_op->getName().c_str();
  }

  ValueType Function::valueType() const
  {
    return m_op->valueType();
  }

  bool Function::isKnown() const
  {
    return m_op->calcNative(m_valueCache, *this);
  }

  void Function::printValue(std::ostream &s) const
  {
    m_op->printValue(s, m_valueCache, *this);
  }

  Value Function::toValue() const
  {
    return m_op->toValue(m_valueCache, *this);
  }

  bool Function::isPropagationSource() const
  {
    return m_op->isPropagationSource();
  }

  // Local macro for boilerplate
#define DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD(_type) \
  bool Function::getValue(_type &result) const \
  { \
    return (*m_op)(result, *this); \
  }

  DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD(Boolean)
  DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD(Integer)
  DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD(Real)
  DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD(String)

  DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD(uint16_t)

#undef DEFINE_FUNC_DEFAULT_GET_VALUE_METHOD

#define DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(_type) \
  bool Function::getValuePointer(_type const *&ptr) const \
  { \
    bool result = (*m_op)(*static_cast<_type *>(m_valueCache), *this);    \
    if (result) \
      ptr = static_cast<_type const *>(m_valueCache); /* trust me */ \
    return result; \
  }

  DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(String)
  DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(Array)
  DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(BooleanArray)
  DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(IntegerArray)
  DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(RealArray)
  DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD(StringArray)
  
#undef DEFINE_FUNC_DEFAULT_GET_VALUE_PTR_METHOD
  
  // Default method
  bool Function::apply(Operator const *op, Array &result) const
  {
    return (*op)(result, *this);
  }

  //
  // Implementations
  //

  //
  // NullaryFunction is a function which takes no arguments.
  //

  class NullaryFunction : public Function
  {
  public:
    NullaryFunction(Operator const *op)
      : Function(op)
    {
    }

    virtual ~NullaryFunction()
    {
    }

    virtual size_t size() const
    {
      return 0;
    }
      
    virtual Expression const *operator[](size_t n) const
    {
      assertTrue_2(ALWAYS_FAIL, "operator[]: no arguments in NullaryFunction");
    }

    virtual void setArgument(size_t i, Expression * /* exp */, bool /* garbage */)
    {
      assertTrue_2(ALWAYS_FAIL, "setArgument(): no arguments to set in NullaryFunction");
    }

    virtual bool allSameTypeOrUnknown(ValueType /* vt */) const
    {
      return true;
    }

    virtual void printSubexpressions(std::ostream & /* s */) const
    {
    }

    virtual void handleActivate()
    {
    }

    virtual void handleDeactivate()
    {
    }

    virtual void doSubexprs(ExprUnaryOperator const &f)
    {
    }

  private:

    // Not implemented
    NullaryFunction();
    NullaryFunction(NullaryFunction const &);
    NullaryFunction &operator=(NullaryFunction const &);
  };


  //
  // FixedSizeFunction
  //
  // General cases - optimized cases below
  //

  /**
   * @class FixedSizeFunction
   * @brief Concrete class template for small expression vectors.
   * Allows optimization for common cases (specifically one and two parameter function calls).
   */

  template <unsigned N>
  class FixedSizeFunction : public Function
  {
  public:
    FixedSizeFunction(Operator const *op)
      : Function(op)
    {
      for (size_t i = 0; i < N; ++i)
        exprs[i] = NULL;
      for (size_t i = 0; i < N; ++i)
        garbage[i] = false;
    }

    ~FixedSizeFunction()
    {
      for (size_t i = 0; i < N; ++i) {
        if (exprs[i]) {
          exprs[i]->removeListener(this);
          if (garbage[i])
            delete exprs[i];
        }
      }
    }

    // Not worth optimizing this, it's only used once per function at load time.
    virtual bool allSameTypeOrUnknown(ValueType vt) const
    {
      for (size_t i = 0; i < N; ++i) {
        ValueType vti = exprs[i]->valueType();
        if (vti != vt && vti != UNKNOWN_TYPE)
          return false;
      }
      return true;
    }

    virtual size_t size() const
    {
      return N;
    }

    virtual Expression const *operator[](size_t n) const
    {
      check_error_1(n < N);
      return exprs[n]; 
    }

    void setArgument(size_t i, Expression *exp, bool isGarbage)
    {
      exprs[i] = exp;
      garbage[i] = isGarbage;
    }

    virtual void handleActivate()
    {
      for (size_t i = 0; i < N; ++i)
        exprs[i]->activate();
    }

    virtual void handleDeactivate()
    {
      for (size_t i = 0; i < N; ++i)
        exprs[i]->deactivate();
    }

    virtual void printSubexpressions(std::ostream & s) const
    {
      for (size_t i = 0; i < N; ++i) {
        s << ' ';
        exprs[i]->print(s);
      }
    }

    // Have to define this so specialized template functions can be defined below
#define DEFINE_FIXED_ARG_GET_VALUE_METHOD(_type) \
  virtual bool getValue(_type &result) const \
  { \
    return (*m_op)(result, *this); \
  }

    DEFINE_FIXED_ARG_GET_VALUE_METHOD(Boolean)
    DEFINE_FIXED_ARG_GET_VALUE_METHOD(Integer)
    DEFINE_FIXED_ARG_GET_VALUE_METHOD(Real)
    DEFINE_FIXED_ARG_GET_VALUE_METHOD(String)

    // Use base class method for now
    // DEFINE_FIXED_ARG_GET_VALUE_METHOD(NodeState)
    // DEFINE_FIXED_ARG_GET_VALUE_METHOD(NodeOutcome)
    // DEFINE_FIXED_ARG_GET_VALUE_METHOD(FailureType)
    // DEFINE_FIXED_ARG_GET_VALUE_METHOD(CommandHandleValue)

#undef DEFINE_FIXED_ARG_GET_VALUE_METHOD

// Have to define this so specialized template functions can be defined below
#define DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(_type) \
  virtual bool getValuePointer(_type const *&ptr) const \
  { \
    bool result = (*m_op)(*static_cast<_type *>(m_valueCache), this);    \
    if (result) \
      ptr = static_cast<_type const *>(m_valueCache); /* trust me */ \
    return result; \
  }

    DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(String)
    DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(Array)
    DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(BooleanArray)
    DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(IntegerArray)
    DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(RealArray)
    DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD(StringArray)

#undef DEFINE_FIXED_ARG_GET_VALUE_PTR_METHOD

    // Default method, overridden in specialized variants
    virtual bool apply(Operator const *op, Array &result) const
    {
      return (*op)(result, this);
    }

    // Default method, overridden in specialized variants
    virtual void doSubexprs(ExprUnaryOperator const &f)
    {
      for (size_t i = 0; i < N; ++i)
        (f)(exprs[i]);
    }

  private:

    // Not implemented
    FixedSizeFunction();
    FixedSizeFunction(const FixedSizeFunction &);
    FixedSizeFunction &operator=(const FixedSizeFunction &);

    Expression *exprs[N];
    bool garbage[N];
  };

  // One-arg variants

  template <>
  FixedSizeFunction<1>::FixedSizeFunction(Operator const *op)
    : Function(op)
  {
    exprs[0] = NULL;
    garbage[0] = false;
  }

  template <>
  FixedSizeFunction<1>::~FixedSizeFunction()
  {
    if (exprs[0]) {
      exprs[0]->removeListener(this);
      if (garbage[0])
        delete exprs[0];
    }
  }

  template <>
  void FixedSizeFunction<1>::handleActivate()
  {
    exprs[0]->activate();
  }

  template <>
  void FixedSizeFunction<1>::handleDeactivate()
  {
    exprs[0]->deactivate();
  }

  // Local macro for boilerplate
#define DEFINE_ONE_ARG_GET_VALUE_METHOD(_type) \
  template <> bool FixedSizeFunction<1>::getValue(_type &result) const   \
  { \
    return (*m_op)(result, exprs[0]); \
  }

  DEFINE_ONE_ARG_GET_VALUE_METHOD(Boolean)
  DEFINE_ONE_ARG_GET_VALUE_METHOD(Integer)
  DEFINE_ONE_ARG_GET_VALUE_METHOD(Real)
  DEFINE_ONE_ARG_GET_VALUE_METHOD(String)

  // Use base class method for now
  // DEFINE_ONE_ARG_GET_VALUE_METHOD(NodeState)
  // DEFINE_ONE_ARG_GET_VALUE_METHOD(NodeOutcome)
  // DEFINE_ONE_ARG_GET_VALUE_METHOD(FailureType)
  // DEFINE_ONE_ARG_GET_VALUE_METHOD(Commandhandlevalue)

#undef DEFINE_ONE_ARG_GET_VALUE_METHOD

#define DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(_type) \
  template <> bool FixedSizeFunction<1>::getValuePointer(_type const *&ptr) const   \
  { \
    bool result = (*m_op)(*static_cast<_type *>(m_valueCache), exprs[0]); \
    if (result) \
      ptr = static_cast<_type const *>(m_valueCache); /* trust me */ \
    return result; \
  }

  DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(String)
  DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(Array)
  DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(BooleanArray)
  DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(IntegerArray)
  DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(RealArray)
  DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD(StringArray)
  
#undef DEFINE_ONE_ARG_GET_VALUE_PTR_METHOD

  // Specialized method
  template <>
  bool FixedSizeFunction<1>::apply(Operator const *op, Array &result) const
  {
    return (*op)(result, exprs[0]);
  }

  // Specialized method
  template <>
  void FixedSizeFunction<1>::doSubexprs(ExprUnaryOperator const &f)
  {
    (f)(exprs[0]);
  }

  //
  // Two-arg variants
  //

  template <>
  FixedSizeFunction<2>::FixedSizeFunction(Operator const *op)
    : Function(op)
  {
    exprs[0] = NULL;
    exprs[1] = NULL;
    garbage[0] = false;
    garbage[1] = false;
  }

  template <>
  FixedSizeFunction<2>::~FixedSizeFunction()
  {
    if (exprs[0]) {
      exprs[0]->removeListener(this);
      if (garbage[0])
        delete exprs[0];
    }
    if (exprs[1]) {
      exprs[1]->removeListener(this);
      if (garbage[1])
        delete exprs[1];
    }
  }

  template <>
  void FixedSizeFunction<2>::handleActivate() 
  {
    exprs[0]->activate();
    exprs[1]->activate();
  }

  template <>
  void FixedSizeFunction<2>::handleDeactivate()
  {
    exprs[0]->deactivate();
    exprs[1]->deactivate();
  }

  // Local macro for boilerplate
#define DEFINE_TWO_ARG_GET_VALUE_METHOD(_type) \
  template <> bool FixedSizeFunction<2>::getValue(_type &result) const  \
  { \
    return (*m_op)(result, exprs[0], exprs[1]); \
  }

  DEFINE_TWO_ARG_GET_VALUE_METHOD(Boolean)
  DEFINE_TWO_ARG_GET_VALUE_METHOD(Integer)
  DEFINE_TWO_ARG_GET_VALUE_METHOD(Real)
  DEFINE_TWO_ARG_GET_VALUE_METHOD(String)

  // Use base class method for now 
  // DEFINE_TWO_ARG_GET_VALUE_METHOD(NodeState)
  // DEFINE_TWO_ARG_GET_VALUE_METHOD(NodeOutcome)
  // DEFINE_TWO_ARG_GET_VALUE_METHOD(FailureType)
  // DEFINE_TWO_ARG_GET_VALUE_METHOD(CommandHandleValue)

#undef DEFINE_TWO_ARG_GET_VALUE_METHOD

#define DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(_type) \
  template <> bool FixedSizeFunction<2>::getValuePointer(_type const *&ptr) const \
  { \
    bool result = (*m_op)(*static_cast<_type *>(m_valueCache), exprs[0], exprs[1]); \
    if (result) \
      ptr = static_cast<_type const *>(m_valueCache); /* trust me */ \
    return result; \
  }

  DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(String)
  DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(Array)
  DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(BooleanArray)
  DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(IntegerArray)
  DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(RealArray)
  DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD(StringArray)
  
#undef DEFINE_TWO_ARG_GET_VALUE_PTR_METHOD

  // Specialized method
  template <>
  bool FixedSizeFunction<2>::apply(Operator const *op, Array &result) const
  {
    return (*op)(result, exprs[0], exprs[1]);
  }

  // Specialized method
  template <>
  void FixedSizeFunction<2>::doSubexprs(ExprUnaryOperator const &f)
  {
    (f)(exprs[0]);
    (f)(exprs[1]);
  }

  //
  // NaryFunction
  //

  /**
   * @class NaryFunction
   * @brief Concrete variable-length variant of Function which uses dynamically allocated arrays.
   */
  class NaryFunction : public Function
  {
  public:
    NaryFunction(Operator const *op, size_t n)
      : Function(op),
        m_size(n),
        exprs(new Expression*[n]()),
        garbage(new bool[n]())
    {
    }

    ~NaryFunction()
    {
      for (size_t i = 0; i < m_size; ++i) {
        if (exprs[i]) {
          exprs[i]->removeListener(this);
          if (garbage[i])
            delete exprs[i];
        }
      }
      delete[] garbage;
      delete[] exprs;
    }

    virtual size_t size() const
    {
      return m_size;
    }

    virtual Expression const *operator[](size_t n) const
    {
      check_error_1(n < m_size);
      return exprs[n]; 
    }

    void setArgument(size_t i, Expression *exp, bool isGarbage)
    {
      assertTrue_2(i < m_size, "setArgument(): too many args");
      exprs[i] = exp;
      garbage[i] = isGarbage;
    }

    virtual bool allSameTypeOrUnknown(ValueType vt) const
    {
      for (size_t i = 0; i < m_size; ++i) {
        ValueType vti = exprs[i]->valueType();
        if (vti != vt && vti != UNKNOWN_TYPE)
          return false;
      }
      return true;
    }

    void handleActivate()
    {
      for (size_t i = 0; i < m_size; ++i)
        exprs[i]->activate();
    }
      
    void handleDeactivate()
    {
      for (size_t i = 0; i < m_size; ++i)
        exprs[i]->deactivate();
    }

    void printSubexpressions(std::ostream & s) const
    {
      for (size_t i = 0; i < m_size; ++i) {
        s << ' ';
        exprs[i]->print(s);
      }
    }

    virtual void doSubexprs(ExprUnaryOperator const &f)
    {
      for (size_t i = 0; i < this->size(); ++i)
        (f)(exprs[i]);
    }

  private:
    // Not implemented
    NaryFunction();
    NaryFunction(NaryFunction const &);
    NaryFunction &operator=(NaryFunction const &);

    size_t m_size;
    Expression **exprs;
    bool *garbage;
  };

  //
  // Factory functions
  //
  
  Function *makeFunction(Operator const *op,
                         size_t n)
  {
    assertTrue_2(op, "makeFunction: null operator");

    switch (n) {
    case 0:
      return static_cast<Function *>(new NullaryFunction(op));
    case 1:
      return static_cast<Function *>(new FixedSizeFunction<1>(op));
    case 2:
      return static_cast<Function *>(new FixedSizeFunction<2>(op));
    case 3:
      return static_cast<Function *>(new FixedSizeFunction<3>(op));
    case 4:
      return static_cast<Function *>(new FixedSizeFunction<4>(op));
    default: // anything greater than 4
      return static_cast<Function *>(new NaryFunction(op, n));
    }
  }

  Function *makeFunction(Operator const *op,
                         Expression *expr,
                         bool garbage)
  {
    assertTrue_2(op && expr, "makeFunction: operator or argument is null");
    Function *result = new FixedSizeFunction<1>(op);
    result->setArgument(0, expr, garbage);
    return result;
  }

  Function *makeFunction(Operator const *op, 
                         Expression *expr1,
                         Expression *expr2,
                         bool garbage1,
                         bool garbage2)
  {
    assertTrue_2(op && expr1 && expr2, "makeFunction: operator or argument is null");
    Function *result = new FixedSizeFunction<2>(op);
    result->setArgument(0, expr1, garbage1);
    result->setArgument(1, expr2, garbage2);
    return result;
  }

} // namespace PLEXIL

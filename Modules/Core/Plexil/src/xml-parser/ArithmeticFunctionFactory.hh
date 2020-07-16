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

#ifndef PLEXIL_ARITHMETIC_FUNCTION_FACTORY_HH
#define PLEXIL_ARITHMETIC_FUNCTION_FACTORY_HH

#include "FunctionFactory.hh"
#include "ParserException.hh"

namespace PLEXIL
{

  /**
   * @class ArithmeticFunctionFactory
   * @brief A specialization of ExpressionFactory which selects the appropriate
   * Function and Operator templates, based on the parameter type(s).
   */
  class ArithmeticFunctionFactory : public FunctionFactory
  {
  public:
    ArithmeticFunctionFactory(std::string const &name);
    ~ArithmeticFunctionFactory();

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool & wasCreated,
                         ValueType returnType = UNKNOWN_TYPE) const;

  protected:
    // Override base class virtual method
    Operator const *getOperator() const { return NULL ;}

    // Delegate to derived classes
    virtual Operator const *selectOperator(ValueType type) const = 0;

  private:
    // Not implemented
    ArithmeticFunctionFactory();
    ArithmeticFunctionFactory(ArithmeticFunctionFactory const &);
    ArithmeticFunctionFactory &operator=(ArithmeticFunctionFactory const &);
  };

  template <template <typename NUM> class OP>
  class ArithmeticFunctionFactoryImpl
    : public ArithmeticFunctionFactory
  {
  public:
    ArithmeticFunctionFactoryImpl(std::string const &name)
      : ArithmeticFunctionFactory(name)
    {
    }

    ~ArithmeticFunctionFactoryImpl()
    {
    }

  protected:

    // Default methods, can be overridden as required
    Operator const *selectOperator(ValueType type) const
    {
      switch (type) {
      case INTEGER_TYPE:
        return OP<int32_t>::instance();

      case REAL_TYPE:
        return OP<double>::instance();
      
      default:
        checkParserException(false,
                             "createExpression: invalid or unimplemented type "
                             << valueTypeName(type)
                             << " for operator " << this->m_name);
        return NULL;
      }
    }

  private:

    // Not implemented
    ArithmeticFunctionFactoryImpl();
    ArithmeticFunctionFactoryImpl(ArithmeticFunctionFactoryImpl const &);
    ArithmeticFunctionFactoryImpl &operator=(ArithmeticFunctionFactoryImpl const &);
  };

} // namespace PLEXIL

// Convenience macros
#define REGISTER_ARITHMETIC_FUNCTION(CLASS,NAME) {new PLEXIL::ArithmeticFunctionFactoryImpl<CLASS>(#NAME);}

#endif // PLEXIL_ARITHMETIC_FUNCTION_FACTORY_HH

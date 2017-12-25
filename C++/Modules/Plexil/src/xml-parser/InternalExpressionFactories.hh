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

#ifndef PLEXIL_INTERNAL_EXPRESSION_FACTORIES_HH
#define PLEXIL_INTERNAL_EXPRESSION_FACTORIES_HH

#include "CommandHandleVariable.hh"
#include "ConcreteExpressionFactory.hh"
#include "NodeTimepointValue.hh"
#include "NodeVariables.hh"

namespace PLEXIL
{

  //
  // Declaration of templates for internal variable/reference factories
  //

  template <>
  class ConcreteExpressionFactory<StateVariable> : public ExpressionFactory
  {
  public:
    ConcreteExpressionFactory(const std::string& name)
      : ExpressionFactory(name) 
    {
    }

    ~ConcreteExpressionFactory()
    {
    }

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool &wasCreated,
                         ValueType returnType) const;

  private:
    // Default, copy, assign all prohibited
    ConcreteExpressionFactory();
    ConcreteExpressionFactory(const ConcreteExpressionFactory &);
    ConcreteExpressionFactory &operator=(const ConcreteExpressionFactory &);
  };

  template <>
  class ConcreteExpressionFactory<OutcomeVariable> : public ExpressionFactory
  {
  public:
    ConcreteExpressionFactory(const std::string& name)
      : ExpressionFactory(name) 
    {
    }

    ~ConcreteExpressionFactory()
    {
    }

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool &wasCreated,
                         ValueType returnType) const;

  private:
    // Default, copy, assign all prohibited
    ConcreteExpressionFactory();
    ConcreteExpressionFactory(const ConcreteExpressionFactory &);
    ConcreteExpressionFactory &operator=(const ConcreteExpressionFactory &);
  };

  template <>
  class ConcreteExpressionFactory<FailureVariable> : public ExpressionFactory
  {
  public:
    ConcreteExpressionFactory(const std::string& name)
      : ExpressionFactory(name) 
    {
    }

    ~ConcreteExpressionFactory()
    {
    }

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool &wasCreated,
                         ValueType returnType) const;

  private:
    // Default, copy, assign all prohibited
    ConcreteExpressionFactory();
    ConcreteExpressionFactory(const ConcreteExpressionFactory &);
    ConcreteExpressionFactory &operator=(const ConcreteExpressionFactory &);
  };

  template <>
  class ConcreteExpressionFactory<CommandHandleVariable> : public ExpressionFactory
  {
  public:
    ConcreteExpressionFactory(const std::string& name)
      : ExpressionFactory(name) 
    {
    }

    ~ConcreteExpressionFactory()
    {
    }

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool &wasCreated,
                         ValueType returnType) const;

  private:
    // Default, copy, assign all prohibited
    ConcreteExpressionFactory();
    ConcreteExpressionFactory(const ConcreteExpressionFactory &);
    ConcreteExpressionFactory &operator=(const ConcreteExpressionFactory &);
  };

  template <>
  class ConcreteExpressionFactory<NodeTimepointValue> : public ExpressionFactory
  {
  public:
    ConcreteExpressionFactory(const std::string& name)
      : ExpressionFactory(name) 
    {
    }

    ~ConcreteExpressionFactory()
    {
    }

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool &wasCreated,
                         ValueType returnType) const;

  private:
    // Default, copy, assign all prohibited
    ConcreteExpressionFactory();
    ConcreteExpressionFactory(const ConcreteExpressionFactory &);
    ConcreteExpressionFactory &operator=(const ConcreteExpressionFactory &);
  };

  //
  // Specialization of ExpressionFactory for node internal constants
  //

  template <class C>
  class NamedConstantExpressionFactory : public ExpressionFactory
  {
  public:
    NamedConstantExpressionFactory(const std::string& name)
      : ExpressionFactory(name) 
    {
    }

    ~NamedConstantExpressionFactory()
    {
    }

    Expression *allocate(pugi::xml_node const expr,
                         NodeConnector *node,
                         bool &wasCreated,
                         ValueType returnType) const;

  private:
    // Default, copy, assign all prohibited
    NamedConstantExpressionFactory();
    NamedConstantExpressionFactory(const NamedConstantExpressionFactory &);
    NamedConstantExpressionFactory &operator=(const NamedConstantExpressionFactory &);
  };

} // namespace PLEXIL

#endif // PLEXIL_INTERNAL_EXPRESSION_FACTORIES_HH

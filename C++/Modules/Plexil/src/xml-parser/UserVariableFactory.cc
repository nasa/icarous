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

#include "UserVariableFactory.hh"

#include "Error.hh"
#include "NodeConnector.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"
#include "UserVariable.hh"

#include "pugixml.hpp"

namespace PLEXIL
{
  UserVariableFactory::UserVariableFactory(std::string const &name)
    : ExpressionFactory(name)
  {
  }

  UserVariableFactory::~UserVariableFactory()
  {
  }

  // N.B. Construction of initializer expression happens later.

  Expression *UserVariableFactory::allocate(pugi::xml_node const expr,
                                            NodeConnector *node,
                                            bool &wasCreated,
                                            ValueType /* returnType */) const
    throw(ParserException)
  {
    pugi::xml_node nameElt = expr.first_child();
    checkTag(NAME_TAG, nameElt);
    char const *name = nameElt.child_value();
    checkParserExceptionWithLocation(*name,
                                     nameElt,
                                     "createExpression: DeclareVariable with empty or malformed Name element");

    pugi::xml_node typeElt = nameElt.next_sibling();
    checkParserExceptionWithLocation(typeElt,
                                     expr,
                                     "createExpression: DeclareVariable missing Type element");
    checkTag(TYPE_TAG, typeElt);
    ValueType typ = parseValueType(typeElt.child_value());
    checkParserExceptionWithLocation(isScalarType(typ),
                                     typeElt,
                                     "createExpression: Type " << typeElt.child_value()
                                     << " is invalid for DeclareVariable");

    wasCreated = true;
    switch (typ) {
    case BOOLEAN_TYPE:
      return new BooleanVariable(node, name);

    case INTEGER_TYPE:
      return new IntegerVariable(node, name);

    case DATE_TYPE: // FIXME
    case DURATION_TYPE: // FIXME
    case REAL_TYPE:
      return new RealVariable(node, name);

    case STRING_TYPE:
      return new StringVariable(node, name);

    default:
      assertTrue_2(ALWAYS_FAIL,
                   "UserVariableFactory::allocate: Internal type error");
      return NULL;
    }
  }

} // namespace PLEXIL

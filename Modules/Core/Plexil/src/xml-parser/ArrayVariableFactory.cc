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

#include "ArrayVariableFactory.hh"

#include "ArrayVariable.hh"
#include "Constant.hh"
#include "Error.hh"
#include "NodeConnector.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"

#include "pugixml.hpp"

#include <cstdlib>
#include <limits>

namespace PLEXIL
{
  ArrayVariableFactory::ArrayVariableFactory(std::string const &name)
    : ExpressionFactory(name)
  {
  }

  ArrayVariableFactory::~ArrayVariableFactory()
  {
  }

  // N.B. Construction of initializer expression happens later.

  Expression *ArrayVariableFactory::allocate(pugi::xml_node const expr,
                                             NodeConnector *node,
                                             bool &wasCreated,
                                             ValueType /* returnType */) const
  {
    pugi::xml_node nameElt = expr.first_child();
    checkTag(NAME_TAG, nameElt);
    char const *name = nameElt.child_value();
    checkParserExceptionWithLocation(*name,
                                     expr,
                                     "createExpression: DeclareArray with empty or malformed Name element");

    pugi::xml_node typeElt = nameElt.next_sibling();
    checkParserExceptionWithLocation(typeElt,
                                     expr,
                                     "createExpression: DeclareArray missing Type element");
    checkTag(TYPE_TAG, typeElt);
    ValueType typ = parseValueType(typeElt.child_value());
    checkParserExceptionWithLocation(isScalarType(typ),
                                     typeElt,
                                     "createExpression: Type " << typeElt.child_value()
                                     << " is invalid for DeclareArray");
    pugi::xml_node sizeElt = typeElt.next_sibling();
    Expression *sizeExp = NULL;
    bool sizeIsGarbage = false;
    if (testTag(MAX_SIZE_TAG, sizeElt)) {
      char const *sizeStr = sizeElt.child_value();
      // Syntactic check
      checkParserExceptionWithLocation(isInteger(sizeStr),
                                       sizeElt,
                                       "createExpression: MaxSize value \"" << sizeStr << "\" is not an integer");

      char *end;
      long size = strtol(sizeStr, &end, 10);
      checkParserExceptionWithLocation(!*end,
                                       sizeElt,
                                       "createExpression: MaxSize value \"" << sizeStr << "\" is not an integer");
      checkParserExceptionWithLocation(size >= 0
				       && size < std::numeric_limits<int32_t>::max(),
                                       sizeElt,
                                       "createExpression: MaxSize value " << sizeStr << " is not a non-negative integer");
      sizeExp = new Constant<int32_t>((int32_t) size);
      sizeIsGarbage = true;
    }

    wasCreated = true;
    switch (typ) {
    case BOOLEAN_TYPE:
      return new BooleanArrayVariable(node,
                                      name,
                                      sizeExp,
                                      sizeIsGarbage);

    case INTEGER_TYPE:
      return new IntegerArrayVariable(node,
                                      name,
                                      sizeExp,
                                      sizeIsGarbage);

    case DATE_TYPE: // FIXME
    case DURATION_TYPE: // FIXME
    case REAL_TYPE:
      return new RealArrayVariable(node,
                                   name,
                                   sizeExp,
                                   sizeIsGarbage);

    case STRING_TYPE:
      return new StringArrayVariable(node,
                                     name,
                                     sizeExp,
                                     sizeIsGarbage);

    default:
      assertTrue_2(ALWAYS_FAIL,
                   "ArrayVariableFactory::allocate: Internal type error");
      return NULL;
    }
  }

} // namespace PLEXIL

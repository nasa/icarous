/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

#ifndef PLEXIL_PARSER_UTILS_HH
#define PLEXIL_PARSER_UTILS_HH

#include "Error.hh" // PLEXIL_NORETURN macro
#include "ParserException.hh"

//
// General purpose xml parsing utilities
//

namespace pugi
{
  class xml_node;
}

namespace PLEXIL
{
  extern bool testPrefix(char const *prefix, char const *str);
  extern bool testSuffix(char const *suffix, char const *str);
  extern bool testTag(const char* t, pugi::xml_node const e);
  extern bool testTagPrefix(const char* prefix, pugi::xml_node const e);
  extern bool testTagSuffix(const char* suffix, pugi::xml_node const e);
  extern bool hasChildElement(pugi::xml_node const e);
  extern void checkTag(const char* t, pugi::xml_node const e)
    throw (ParserException);
  extern void checkAttr(const char* t, pugi::xml_node const e)
    throw (ParserException);
  extern void checkTagSuffix(const char* t, pugi::xml_node const e)
    throw (ParserException);
  extern void checkNotEmpty(pugi::xml_node const e)
    throw (ParserException);
  extern void checkHasChildElement(pugi::xml_node const e)
    throw (ParserException);
  extern bool isBoolean(const char* initval);
  extern bool isInteger(const char* initval);
  extern bool isDouble(const char* initval);
  // Helper for checkParserExceptionWithLocation
  extern void reportParserException(std::string const &msg, pugi::xml_node location)
    throw (ParserException) PLEXIL_NORETURN;

} // namespace PLEXIL

/**
 * @def reportParserExceptionWithLocation
 * @brief Throw a ParserException unconditionally
 * @param loc A pugi::xml_node with the location of the exception
 * @param msg An expression which writes the required message to a stream
 */
#define reportParserExceptionWithLocation(loc, msg) { \
  std::ostringstream whatstr; \
  whatstr << msg; \
  reportParserException(whatstr.str().c_str(), loc); \
}

/**
 * @def checkParserExceptionWithLocation
 * @brief If the condition is false, throw a ParserException
 * @param cond The condition to test; if false, throw the exception
 * @param loc A pugi::xml_node with the location of the exception
 * @param msg An expression which writes the required message to a stream
 */
#define checkParserExceptionWithLocation(cond, loc, msg) { \
  if (!(cond)) { \
    reportParserExceptionWithLocation(loc, msg); \
  } \
}

#endif // PLEXIL_PARSER_UTILS_HH

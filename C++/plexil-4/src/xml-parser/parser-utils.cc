/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#include "parser-utils.hh" // for checkParserExceptionWithLocation macro

#include "PlexilSchema.hh"
#include "pugixml.hpp"

#include <cctype>
#include <cstring>

using pugi::node_element;
using pugi::node_pcdata;
using pugi::xml_attribute;
using pugi::xml_node;

namespace PLEXIL
{
  //
  // Internal parsing utilities
  //

  bool testPrefix(char const *prefix, char const *str)
  {
    return str == strstr(str, prefix);
  }

  bool testSuffix(char const* suffix, char const *str)
  {
    char const *tail = strstr(str, suffix);
    if (!tail)
      return false;
    return !strcmp(tail, suffix);
  }

  //
  // The following take advantage of the fact that only xml_nodes of 
  // types node_element, node_pi and node_declaration have a non-empty name()...
  // and by default, node_pi and node_declaration aren't loaded during the parse.
  //

  bool testTag(const char* t, xml_node const e) {
    return 0 == strcmp(t, e.name());
  }

  bool testTagPrefix(const char* prefix, xml_node const e)
  {
    return testPrefix(prefix, e.name());
  }

  bool testTagSuffix(const char* suffix, xml_node const e)
  {
    return testSuffix(suffix, e.name());
  }

  bool hasChildElement(xml_node const e) 
  {
    xml_node temp = e.first_child();
    return temp && temp.type() == node_element;
  }

  void checkTag(const char* t, xml_node const e)
    throw (ParserException)
  {
    checkParserExceptionWithLocation(testTag(t, e),
                                     e,
                                     "XML parsing error: Expected <" << t << "> element, but got <" << e.name() << "> instead.");
  }

  void checkAttr(const char* t, xml_node const e)
    throw (ParserException)
  {
    checkParserExceptionWithLocation(e && e.type() == node_element && e.attribute(t),
                                     e,
                                     "XML parsing error: Expected an attribute named '" << t << "' in element <" << e.name() << ">");
  }

  void checkTagSuffix(const char* t, xml_node const e)
    throw (ParserException)
  {
    checkParserExceptionWithLocation(testTagSuffix(t, e),
                                     e,
                                     "XML parsing error: Expected an element ending in '" << t << "', but instead got <" << e.name() << ">");
  }

  // N.B. presumes e is not empty
  void checkNotEmpty(xml_node const e)
    throw (ParserException)
  {
    xml_node temp = e.first_child();
    checkParserExceptionWithLocation(temp
                                     && temp.type() == node_pcdata
                                     && *(temp.value()),
                                     e,
                                     "XML parsing error: Expected a non-empty text child of <" << e.name() << ">");
  }

  // N.B. presumes e is not empty
  void checkHasChildElement(xml_node const e)
    throw (ParserException)
  {
    checkParserExceptionWithLocation(hasChildElement(e),
                                     e,
                                     "XML parsing error: Expected a child element of <" << e.name() << ">");
  }

  bool isBoolean(const char* initval)
  {
    if (initval == NULL)
      return false;

    switch (*initval) {
    case '0':
    case '1':
      if (*++initval)
        return false;
      else
        return true;

    case 'f':
      return (0 == strcmp(++initval, "alse"));

    case 't':
      return (0 == strcmp(++initval, "rue"));

    default:
      return false;
    }
  }

  bool isInteger(const char* initval)
  {
    if (initval == NULL || !*initval)
      return false;

    // Check against XML 'integer'
    // [\-+]?[0-9]+
    if ('+' == *initval || '-' == *initval) {
      if (!*++initval)
        return false; // sign w/ nothing after it
    }
    if (isdigit(*initval)) {
      while (*++initval && isdigit(*initval)) {
      }
    }
    if (*initval)
      return false; // junk after number

    // TODO: add range check
    return true;
  }

  bool isDouble(const char* initval)
  {
    if (initval == NULL || !*initval)
      return false;
      
    // Check against XML 'double'
    // (\+|-)?([0-9]+(\.[0-9]*)?|\.[0-9]+)([Ee](\+|-)?[0-9]+)?|(\+|-)?INF|NaN

    if (strcmp(initval, "NaN") == 0) 
      return true;

    if ('+' == *initval || '-' == *initval) {
      if (!*++initval)
        return false; // sign w/ nothing after it
    }

    if (*initval && strcmp(initval, "INF") == 0)
      return true;

    // ([0-9]+(\.[0-9]*)?|\.[0-9]+)([Ee](\+|-)?[0-9]+)?
    bool digitsSeen = false;
    if (*initval && isdigit(*initval)) {
      digitsSeen = true;
      while (*++initval && isdigit(*initval)) {
      }
    }
    if (*initval && '.' == *initval) {
      if (*++initval && isdigit(*initval)) {
        digitsSeen = true;        
        while (*++initval && isdigit(*initval)) {
        }
      }
    }
    if (!digitsSeen)
      return false;

    // Optional exponent
    if (*initval && ('E' == *initval || 'e' == *initval)) {
      if (!*++initval)
        return false; 
      if ('+' == *initval || '-' == *initval) {
        if (!*++initval)
          return false; 
      }
      if (isdigit(*initval)) {
        while (*++initval && isdigit(*initval)) {
        }
      }
    }

    if (*initval)
      return false; // junk after number

    // FIXME: add range check?
    return true;
  }

  static bool findSourceLocation(xml_node here, char const *&filename, int &line, int &col)
  {
    // File name is now only on PlexilPlan node
    filename = NULL;
    pugi::xml_node planNode = here.root().child(PLEXIL_PLAN_TAG); // should be PlexilPlan node
    if (planNode) {
      xml_attribute fileAttr = planNode.attribute(FILE_NAME_ATTR);
      if (fileAttr) {
        filename = fileAttr.value();
      }
    }
    // else input is bad or pugi is broken, but ignore for our purposes

    bool lineSeen = false, colSeen = false;
    while (here && !(lineSeen && colSeen)) {
      if (!lineSeen) {
        xml_attribute lineno = here.attribute(LINE_NO_ATTR);
        if (lineno) {
          line = lineno.as_int();
          lineSeen = true;
        }
      }
      if (!colSeen) {
        xml_attribute colno = here.attribute(COL_NO_ATTR);
        if (colno) {
          col = colno.as_int();
          colSeen = true;
        }
      }
      here = here.parent();
    }

    // got to root and found nothing
    if (!lineSeen)
      line = 0;
    if (!colSeen)
      col = 0;
    return (lineSeen || colSeen);
  }

  void reportParserException(std::string const &msg, xml_node location)
    throw (ParserException)
  {
    char const *sourcefile = NULL;
    int line = 0, col = 0;
    if (findSourceLocation(location, sourcefile, line, col))
      throw ParserException(msg.c_str(), sourcefile, line, col);
    else {
      std::ostringstream msgWithXml;
      msgWithXml << msg << "\n In\n";
      location.print(msgWithXml, " ");
      throw ParserException(msgWithXml.str().c_str());
    }
  }

} // namespace PLEXIL

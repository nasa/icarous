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

#include "Error.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"
#include "SymbolTable.hh"

#include "pugixml.hpp"

#include <cstring>

using pugi::xml_node;

namespace PLEXIL
{

  static ValueType parseValueDeclaration(xml_node const elt)
    throw (ParserException)
  {
    // We only care about Type and MaxSize
    xml_node typeElt = elt.child(TYPE_TAG);
    checkParserExceptionWithLocation(typeElt,
                                     elt,
                                     elt.name() << " with no " << TYPE_TAG << " element");

    char const *typnam = elt.child(TYPE_TAG).child_value();
    checkParserExceptionWithLocation(typnam && *typnam,
                                     typeElt,
                                     elt.name() << " with empty " << TYPE_TAG << " element");

    // Allow "Any" type
    ValueType typ = parseValueType(typnam);
    if (typ == UNKNOWN_TYPE) {
      checkParserExceptionWithLocation(!strcmp(typnam, ANY_VAL),
                                       typeElt,
                                       elt.name() << " has invalid type name " << typnam);
    }

    // Is it an array?
    xml_node maxElt = elt.child(MAX_SIZE_TAG);
    if (maxElt) {
      typ = arrayType(typ);
      checkParserExceptionWithLocation(typ != UNKNOWN_TYPE,
                                       typeElt,
                                       elt.name() << " has illegal array element type " << typnam);
    }
    return typ;
  }

  static void parseCommandDeclaration(xml_node const declXml)
    throw (ParserException)
  {
    xml_node elt = declXml.first_child();
    checkTag(NAME_TAG, elt);

    char const *name = elt.child_value();
    checkParserExceptionWithLocation(name && *name,
                                     elt,
                                     declXml.name() << " " << NAME_TAG << " is empty");

    Symbol *cmd = g_symbolTable->addCommand(name);
    checkParserExceptionWithLocation(cmd,
                                     elt,
                                     declXml.name() << " " << name << " is already declared");

    elt = elt.next_sibling();
    if (!elt)
      return; // all done

    if (testTag(RETURN_TAG, elt)) {
      cmd->setReturnType(parseValueDeclaration(elt));
      elt = elt.next_sibling();
    }

    while (elt && testTag(PARAMETER_TAG, elt)) {
      cmd->addParameterType(parseValueDeclaration(elt));
      elt = elt.next_sibling();
    }

    if (elt && testTag(ANY_PARAMETERS_TAG, elt)) {
      cmd->setAnyParameters();
      elt = elt.next_sibling();
    }

    if (elt && testTag(RESOURCE_LIST_TAG, elt)) {
      // TODO parse ResourceList
    }
  }

  static void parseStateDeclaration(xml_node const declXml)
    throw (ParserException)
  {
    xml_node elt = declXml.first_child();
    checkTag(NAME_TAG, elt);

    char const *name = elt.child_value();
    checkParserExceptionWithLocation(name && *name,
                                     elt,
                                     declXml.name() << " " << NAME_TAG << " is empty");

    Symbol *state = g_symbolTable->addLookup(name);
    checkParserExceptionWithLocation(state,
                                     elt,
                                     declXml.name() << " " << name << " is already declared");

    elt = elt.next_sibling();
    checkTag(RETURN_TAG, elt);
    state->setReturnType(parseValueDeclaration(elt));
    elt = elt.next_sibling();

    while (elt && testTag(PARAMETER_TAG, elt)) {
      state->addParameterType(parseValueDeclaration(elt));
      elt = elt.next_sibling();
    }

    if (elt && testTag(ANY_PARAMETERS_TAG, elt)) {
      state->setAnyParameters();
    }
  }

  static void parseInterfaceVariable(xml_node const varDecl, bool isInOut, LibraryNodeSymbol *lib)
    throw (ParserException)
  {
    xml_node elt = varDecl.first_child();
    checkTag(NAME_TAG, elt);

    char const *varName = elt.child_value();
    checkParserExceptionWithLocation(varName && *varName,
                                     elt,
                                     "Empty " << NAME_TAG << " element in " << varDecl.name());
    checkParserExceptionWithLocation(!lib->isParameterDeclared(varName),
                                     varDecl,
                                     "Interface variable " << varName << " is already declared");

    elt = elt.next_sibling();
    checkTag(TYPE_TAG, elt);

    char const *typeName = elt.child_value();
    checkParserExceptionWithLocation(typeName && *typeName,
                                     varDecl,
                                     "Empty " << TYPE_TAG
                                     << " element in interface variable " << varName);
    ValueType t = parseValueType(typeName);
    checkParserExceptionWithLocation(isScalarType(t),
                                     elt,
                                     "Invalid or illegal type " << typeName << " for interface variable " << varName);
    
    lib->addParameter(varName, t, isInOut);
  }

  static void parseInterfaceArrayVariable(xml_node const varDecl, bool isInOut, LibraryNodeSymbol *lib)
    throw (ParserException)
  {
    xml_node elt = varDecl.first_child();
    checkTag(NAME_TAG, elt);

    char const *varName = elt.child_value();
    checkParserExceptionWithLocation(varName && *varName,
                                     elt,
                                     "Empty " << NAME_TAG << " element in " << varDecl.name());
    checkParserExceptionWithLocation(!lib->isParameterDeclared(varName),
                                     varDecl,
                                     "Interface variable " << varName << " is already declared");

    elt = elt.next_sibling();
    checkTag(TYPE_TAG, elt);

    char const *typeName = elt.child_value();
    checkParserExceptionWithLocation(typeName && *typeName,
                                     elt,
                                     "Empty " << TYPE_TAG
                                     << " element in interface array variable " << varName);
    ValueType t = parseValueType(typeName);
    ValueType at = arrayType(t);
    checkParserExceptionWithLocation(isScalarType(t) && at != UNKNOWN_TYPE,
                                     varDecl,
                                     "Invalid or illegal element type " << typeName
                                     << " for interface array variable " << varName);
    
    lib->addParameter(varName, at, isInOut);
  }

  static void parseInDeclaration(xml_node const inDecl, LibraryNodeSymbol *lib)
    throw (ParserException)
  {
    for (xml_node decl = inDecl.first_child(); decl; decl = decl.next_sibling()) {
      if (testTag(DECL_VAR_TAG, decl))
        parseInterfaceVariable(decl, false, lib);
      else if (testTag(DECL_ARRAY_TAG, decl))
        parseInterfaceArrayVariable(decl, false, lib);
      else {
        reportParserExceptionWithLocation(decl,
                                          LIBRARY_NODE_DECLARATION_TAG << " " << lib->name()
                                          << ": Junk in " << inDecl.name() << " element");
      }
    }
  }

  static void parseInOutDeclaration(xml_node const inOutDecl, LibraryNodeSymbol *lib)
    throw (ParserException)
  {
    for (xml_node decl = inOutDecl.first_child(); decl; decl = decl.next_sibling()) {
      if (testTag(DECL_VAR_TAG, decl))
        parseInterfaceVariable(decl, true, lib);
      else if (testTag(DECL_ARRAY_TAG, decl))
        parseInterfaceArrayVariable(decl, true, lib);
      else {
        reportParserExceptionWithLocation(decl,
                                          LIBRARY_NODE_DECLARATION_TAG << " " << lib->name()
                                          << ": Junk in " << inOutDecl.name() << " element");
      }
    }
  }
  
  static void parseLibraryNodeDeclaration(xml_node const declXml)
    throw (ParserException)
  {
    xml_node elt = declXml.first_child();
    checkTag(NAME_TAG, elt);

    char const *name = elt.child_value();
    checkParserExceptionWithLocation(name && *name,
                                     elt,
                                     declXml.name() << " " << NAME_TAG << " is empty");

    LibraryNodeSymbol *lib = g_symbolTable->addLibraryNode(name);
    checkParserExceptionWithLocation(lib,
                                     elt,
                                     declXml.name() << " " << name << " is already declared");

    // Interface
    elt = elt.next_sibling();
    if (testTag(INTERFACE_TAG, elt)) {
      for (xml_node child = elt.first_child();
           child;
           child = child.next_sibling()) {
        if (testTag(IN_TAG, child))
          parseInDeclaration(child, lib);
        else if (testTag(INOUT_TAG, child))
          parseInOutDeclaration(child, lib);
        else {
          reportParserExceptionWithLocation(child,
                                            declXml.name() << " " << name
                                            << ": illegal element " << child.name()
                                            << " in " << INTERFACE_TAG);
        }
      }
    }
  }

  void parseGlobalDeclarations(xml_node const &declsXml)
    throw (ParserException)
  {
    for (xml_node decl = declsXml.first_child();
         decl;
         decl = decl.next_sibling()) {
      if (testTag(COMMAND_DECLARATION_TAG, decl))
        parseCommandDeclaration(decl);
      else if (testTag(STATE_DECLARATION_TAG, decl))
        parseStateDeclaration(decl);
      else if (testTag(LIBRARY_NODE_DECLARATION_TAG, decl))
        parseLibraryNodeDeclaration(decl);
      // *** DEBUG ONLY ***
      else
        warn("Unexpected element " << decl.name() << " found in " << GLOBAL_DECLARATIONS_TAG);
    }
  }

}

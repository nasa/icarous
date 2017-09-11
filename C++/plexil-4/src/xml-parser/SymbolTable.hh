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

#ifndef PLEXIL_SYMBOL_TABLE_HH
#define PLEXIL_SYMBOL_TABLE_HH

#include "plexil-config.h"

#include "ParserException.hh"
#include "ValueType.hh"

#include <map>
#include <string>
#include <vector>

#ifdef HAVE_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#elif defined(__VXWORKS__)
#include <vxWorks.h>
#endif

namespace PLEXIL
{
  enum SymbolType {
    NO_SYMBOL_TYPE = 0,
    COMMAND_TYPE,
    LOOKUP_TYPE,
    LIBRARY_NODE_TYPE,
    // FUNCTION_TYPE, // future
      
    SYMBOL_TYPE_MAX
  };

  class Symbol
  {
  public:
    Symbol();
    Symbol(Symbol const &orig);
    Symbol(char const *name, SymbolType t);

    Symbol &operator=(Symbol const &orig);

    ~Symbol();

    std::string const &name() const;

    SymbolType symbolType() const;

    void setReturnType(ValueType t);
    ValueType returnType() const;

    // A Symbol may have 0 or more required, typed parameters.
    void addParameterType(ValueType t);
    ValueType parameterType(size_t n) const;
    size_t parameterCount() const;

    // A Symbol may also have an unlimited number of parameters of any type
    // following the required parameters.
    void setAnyParameters();
    bool anyParameters() const;

  private:

    std::string m_name;
    std::vector<uint8_t> m_paramTypes;
    uint8_t m_symbolType;
    uint8_t m_returnType;
    bool m_anyParams;
  };

  class LibraryNodeSymbol
  {
  public:
    LibraryNodeSymbol();
    LibraryNodeSymbol(LibraryNodeSymbol const &orig);
    LibraryNodeSymbol(char const *name);

    LibraryNodeSymbol &operator=(LibraryNodeSymbol const &orig);

    ~LibraryNodeSymbol();

    std::string const &name() const;

    SymbolType symbolType() const;

    void addParameter(char const *pname, ValueType t, bool isInOut)
      throw (ParserException);

    bool isParameterDeclared(char const *pname);
    bool isParameterInOut(char const *pname);
    ValueType parameterValueType(char const *pname);

  private:

    std::string m_name;
    std::map<std::string, bool> m_paramInOutMap;
    std::map<std::string, ValueType> m_paramTypeMap;
  };
  
  class SymbolTable
  {
  public:
    virtual ~SymbolTable()
    {
    }

    // These return NULL if name is a duplicate.
    virtual Symbol *addCommand(char const *name) = 0;
    virtual Symbol *addLookup(char const *name) = 0;
    virtual LibraryNodeSymbol *addLibraryNode(char const *name) = 0;

    virtual Symbol const *getCommand(char const *name) = 0;
    virtual Symbol const *getLookup(char const *name) = 0;
    virtual LibraryNodeSymbol const *getLibraryNode(char const *name) = 0;

  protected: 
    SymbolTable()
    {
    }

  private:
    // Not implemented
    SymbolTable(SymbolTable const &);
    SymbolTable &operator=(SymbolTable const &);

  };

  extern SymbolTable *makeSymbolTable();

  extern SymbolTable *g_symbolTable;

}

#endif // PLEXIL_SYMBOL_TABLE_HH

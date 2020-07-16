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

#include "SymbolTable.hh"

namespace PLEXIL
{
  //
  // Symbol
  //

  Symbol::Symbol()
    : m_name(),
      m_paramTypes(),
      m_symbolType(NO_SYMBOL_TYPE),
      m_returnType(UNKNOWN_TYPE),
      m_anyParams(false)
  {
  }

  Symbol::Symbol(Symbol const &orig)
    : m_name(orig.m_name),
      m_paramTypes(orig.m_paramTypes),
      m_symbolType(orig.m_symbolType),
      m_returnType(orig.m_returnType),
      m_anyParams(orig.m_anyParams)
  {
  }

  Symbol::Symbol(char const *name, SymbolType t)
    : m_name(name),
      m_paramTypes(),
      m_symbolType(t),
      m_returnType(UNKNOWN_TYPE),
      m_anyParams(false)
  {
  }

  Symbol::~Symbol()
  {
  }

  Symbol &Symbol::operator=(Symbol const &orig)
  {
    m_name = orig.m_name;
    m_paramTypes = orig.m_paramTypes;
    m_symbolType = orig.m_symbolType;
    m_returnType = orig.m_returnType;
    m_anyParams = orig.m_anyParams;
    return *this;
  }

  std::string const &Symbol::name() const
  {
    return m_name;
  }

  SymbolType Symbol::symbolType() const
  {
    return (SymbolType) m_symbolType;
  }

  ValueType Symbol::returnType() const
  {
    return (ValueType) m_returnType;
  }

  ValueType Symbol::parameterType(size_t n) const
  {
    if (n > m_paramTypes.size())
      return UNKNOWN_TYPE;
    return (ValueType) m_paramTypes[n];
  }

  void Symbol::setReturnType(ValueType t)
  {
    m_returnType = (uint8_t) t;
  }

  void Symbol::setAnyParameters()
  {
    m_anyParams = true;
  }

  bool Symbol::anyParameters() const
  {
    return m_anyParams;
  }

  void Symbol::addParameterType(ValueType t)
  {
    m_paramTypes.push_back((uint8_t) t);
  }

  size_t Symbol::parameterCount() const
  {
    return m_paramTypes.size();
  }

  //
  // LibraryNodeSymbol
  //

  LibraryNodeSymbol::LibraryNodeSymbol()
    : m_name(),
      m_paramInOutMap(),
      m_paramTypeMap()
  {
  }

  LibraryNodeSymbol::LibraryNodeSymbol(LibraryNodeSymbol const &orig)
    : m_name(orig.m_name),
      m_paramInOutMap(orig.m_paramInOutMap),
      m_paramTypeMap(orig.m_paramTypeMap)
  {
  }

  LibraryNodeSymbol::LibraryNodeSymbol(char const *name)
    : m_name(name),
      m_paramInOutMap(),
      m_paramTypeMap()
  {
  }

  LibraryNodeSymbol::~LibraryNodeSymbol()
  {
  }

  LibraryNodeSymbol &LibraryNodeSymbol::operator=(LibraryNodeSymbol const &orig)
  {
    m_name = orig.m_name;
    m_paramInOutMap = orig.m_paramInOutMap;
    m_paramTypeMap = orig.m_paramTypeMap;
    return *this;
  }

  std::string const &LibraryNodeSymbol::name() const
  {
    return m_name;
  }

  SymbolType LibraryNodeSymbol::symbolType() const
  {
    return LIBRARY_NODE_TYPE;
  }

  void LibraryNodeSymbol::addParameter(char const *pname,
                                       ValueType t,
                                       bool isInOut)
    throw (ParserException)
  {
    std::string const pnameStr(pname);
    std::map<std::string, bool>::const_iterator it =
      m_paramInOutMap.find(pnameStr);
    if (it != m_paramInOutMap.end()) {
      // TODO Parser exception - duplicate name
    }
    m_paramInOutMap[pnameStr] = isInOut;
    m_paramTypeMap[pnameStr] = t;
  }

  bool LibraryNodeSymbol::isParameterDeclared(char const *pname)
  {
    std::string const pnameStr(pname);
    std::map<std::string, bool>::const_iterator it =
      m_paramInOutMap.find(pnameStr);
    return it != m_paramInOutMap.end();
  }

  bool LibraryNodeSymbol::isParameterInOut(char const *pname)
  {
    std::string const pnameStr(pname);
    std::map<std::string, bool>::const_iterator it =
      m_paramInOutMap.find(pnameStr);
    if (it == m_paramInOutMap.end())
      return false;
    return it->second;
  }

  ValueType LibraryNodeSymbol::parameterValueType(char const *pname)
  {
    std::string const pnameStr(pname);
    std::map<std::string, ValueType>::const_iterator it =
      m_paramTypeMap.find(pnameStr);
    if (it == m_paramTypeMap.end())
      return UNKNOWN_TYPE;
    return it->second;
  }

  //
  // SymbolTableImpl
  //

  class SymbolTableImpl
    : public SymbolTable
  {
  private:
    typedef std::map<std::string, Symbol *> SymbolMap;
    typedef std::map<std::string, LibraryNodeSymbol *> LibraryMap;

    SymbolMap m_commandMap;
    SymbolMap m_lookupMap;
    // SymbolMap m_functionMap; // future
    LibraryMap m_libraryMap;

  public:
    SymbolTableImpl()
      : SymbolTable()
    {
    }

    ~SymbolTableImpl()
    {
      SymbolMap::iterator it = m_commandMap.begin();
      while (it != m_commandMap.end()) {
        delete it->second;
        m_commandMap.erase(it);
        it = m_commandMap.begin();
      }
      it = m_lookupMap.begin();
      while (it != m_lookupMap.end()) {
        delete it->second;
        m_lookupMap.erase(it);
        it = m_lookupMap.begin();
      }
      LibraryMap::iterator lit = m_libraryMap.begin();
      while (lit != m_libraryMap.end()) {
        delete lit->second;
        m_libraryMap.erase(lit);
        lit = m_libraryMap.begin();
      }
    }

    Symbol *addCommand(char const *name)
    {
      std::string const namestr(name);
      SymbolMap::const_iterator it =
        m_commandMap.find(namestr);
      if (it != m_commandMap.end())
        return NULL; // duplicate
      return (m_commandMap[namestr] = new Symbol(name, COMMAND_TYPE));
    }

    Symbol *addLookup(char const *name)
      throw (ParserException)
    {
      std::string const namestr(name);
      SymbolMap::const_iterator it =
        m_lookupMap.find(namestr);
      if (it != m_lookupMap.end())
        return NULL; // duplicate
      return (m_lookupMap[namestr] = new Symbol(name, LOOKUP_TYPE));
    }

    LibraryNodeSymbol *addLibraryNode(char const *name)
      throw (ParserException)
    {
      std::string const namestr(name);
      LibraryMap::const_iterator it =
        m_libraryMap.find(namestr);
      if (it != m_libraryMap.end())
        return NULL; // duplicate
      return (m_libraryMap[namestr] = new LibraryNodeSymbol(name));
    }

    Symbol const *getCommand(char const *name)
    {
      std::string const namestr(name);
      SymbolMap::const_iterator it =
        m_commandMap.find(namestr);
      if (it == m_commandMap.end())
        return NULL;
      return it->second;
    }

    Symbol const *getLookup(char const *name)
    {
      std::string const namestr(name);
      SymbolMap::const_iterator it =
        m_lookupMap.find(namestr);
      if (it == m_lookupMap.end())
        return NULL;
      return it->second;
    }

    LibraryNodeSymbol const *getLibraryNode(char const *name)
    {
      std::string const namestr(name);
      LibraryMap::const_iterator it =
        m_libraryMap.find(namestr);
      if (it == m_libraryMap.end())
        return NULL;
      return it->second;
    }

  };

  SymbolTable *makeSymbolTable()
  {
    return new SymbolTableImpl();
  }

  SymbolTable *g_symbolTable = NULL;
  
}

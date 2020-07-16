/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.6 $  $Author: reids $  $Date: 2011/08/17 01:11:43 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  
 *
 * (c) Copyright 2001 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

#ifndef INCXDRTree_h
#define INCXDRTree_h

#include <sys/stat.h>

#include <string>
#include <cstring>
#include <list>

#include <sstream>
#include "RefCounter.h"
#include "ParsePos.h"

// all parse nodes will do reference counting.  may add other functionality
//    to XDRParseNode later
struct XDRParseNode : public RefCounter {
};

// if you want position information for reserved word and punctuation
//    characters, you can make them have type XDRReservedToken
//    in the bison parser
struct XDRReservedToken : public XDRParseNode {
  ParsePos pos;
  XDRReservedToken(ParsePos _pos) : pos(_pos) { }
};

template <class T>
struct XDRToken : public XDRParseNode {
  T val;
  ParsePos pos;

  XDRToken(T _val, ParsePos _pos)
    : val(_val), pos(_pos) { }
};

typedef XDRToken<string> XDRStringToken;
typedef XDRToken<long> XDRLongToken;

#if XDR_SUPPORT_CONST
// there's no point in defining XDRValueNode if we don't also
//   support const declarations.  if we finish writing support
//   for const declarations, this code may become useful

enum XDRValueEnum {
  V_LONG, V_IDENT
};

// i make inherited classes instead of doing a union because
//   i want to make sure the destructor does the right thing
struct XDRValueNode : public XDRParseNode {
  XDRValueEnum type;
};

struct XDRValueLongNode : public XDRValueNode {
  XDRLongToken *longVal;
  XDRValueLongNode(XDRLongToken *_longVal) : longVal(_longVal) {
    type = V_LONG;
    longVal->ref();
  }
protected:
  ~XDRValueLongNode(void) {
    longVal->unref();
  }
};

struct XDRValueIdentNode : public XDRValueNode {
  XDRStringToken *identVal;
  XDRValueIdentNode(XDRStringToken *_identVal) : identVal(_identVal) {
    type = V_IDENT;
    identVal->ref();
  }
protected:
  ~XDRValueIdentNode(void) {
    identVal->unref();
  }
};

#endif // XDR_SUPPORT_CONST

enum XDRTypeEnum {
  T_STRUCT, T_IDENT, T_ENUM, T_STRING,
  T_UCHAR, T_CHAR, T_UINT, T_INT, T_BOOL, T_FLOAT, T_DOUBLE
};

enum XDRArrayEnum {
  A_SINGLE, A_FIXED_ARRAY, A_VAR_ARRAY
};

struct XDRArrayDimNode : public XDRParseNode {
  XDRLongToken *maxLength;
  XDRArrayDimNode(XDRLongToken *_maxLength = 0)
    : maxLength(_maxLength) {
    if (0 != maxLength) maxLength->ref();
  }
protected:
  ~XDRArrayDimNode(void) {
    if (0 != maxLength) maxLength->unref();
  }
};

template <class T>
struct XDRListNode : public XDRParseNode {
  const list<T *> &getItems(void) { return items; }

  void push_back(T *item) {
    items.push_back(item);
    item->ref();
  }
  void pop_back(void) {
    items.back()->unref();
    items.pop_back();
  }
protected:
  ~XDRListNode(void) {
    typename list<T *>::iterator i;
    for (i=items.begin(); i!=items.end(); i++) {
      (*i)->unref();
    }
  }
  list<T *> items;
};

typedef XDRListNode<XDRArrayDimNode> XDRArrayDimsNode;

enum HeaderLangEnum {
  LANG_C, LANG_CPLUSPLUS, LANG_LISP
};

struct XDRTypeSpecNode : public XDRParseNode {
  XDRTypeEnum type;
  XDRArrayEnum arrayEnum;
  XDRArrayDimsNode *dims;

  XDRTypeSpecNode(XDRTypeEnum _type)
    : type(_type),
      arrayEnum(A_SINGLE) {
    dims = 0;
  }
  void addDims(XDRArrayEnum _arrayEnum, XDRArrayDimsNode *_dims) {
    assert(0 == dims);
    arrayEnum = _arrayEnum;
    dims = _dims;
    dims->ref();
  }
  static const char *nameOfType(XDRTypeEnum tenum);
  static const char *nameOfTypeHeader(XDRTypeEnum tenum, HeaderLangEnum lang);
  static const char *nameOfTypeIPC(XDRTypeEnum tenum);
  const char *nameOfType(void);
  const char *nameOfTypeIPC(void) { return nameOfTypeIPC(type); }
  const char *nameOfTypeHeader(HeaderLangEnum lang);
protected:
  ~XDRTypeSpecNode(void) {
    if (0 != dims) dims->unref();
  }
};

struct XDRIPCTypeNode : public XDRParseNode {
  XDRStringToken *typeName;
  XDRStringToken *formatString;
  XDRIPCTypeNode(XDRStringToken *_typeName)
    : typeName(_typeName) {
    formatString = 0;
  }
  void addFormatString(XDRStringToken *_formatString) {
    assert(0 == formatString);
    formatString = _formatString;
    formatString->ref();
  }
protected:
  ~XDRIPCTypeNode(void) {
    typeName->unref();
    if (0 != formatString) formatString->unref();
  }
};

struct XDRTypeSpecIdentNode : public XDRTypeSpecNode {
  XDRStringToken *ident;
  XDRIPCTypeNode *resolvedType;
  XDRTypeSpecIdentNode(XDRStringToken *_ident)
    : XDRTypeSpecNode(T_IDENT),
      ident(_ident) {
    resolvedType = 0;
    ident->ref();
  }
  void addResolvedType(XDRIPCTypeNode *_resolvedType) {
    assert(0 == resolvedType);
    resolvedType = _resolvedType;
    resolvedType->ref();
  }
protected:
  ~XDRTypeSpecIdentNode(void) {
    ident->unref();
    if (0 != resolvedType) resolvedType->unref();
  }
};

struct XDRTypeSpecStringNode : public XDRTypeSpecNode {
  XDRArrayDimNode *dim;
  XDRTypeSpecStringNode(void) : XDRTypeSpecNode(T_STRING) {
    dim = 0;
  }
  void addStringArrayDim(XDRArrayDimNode *_dim) {
    assert(0 == dim);
    dim = _dim;
    dim->ref();
  }
protected:
  ~XDRTypeSpecStringNode(void) {
    if (0 != dim) dim->unref();
  }
};

struct XDRDeclNode : public XDRParseNode {
  XDRStringToken *fieldName;
  XDRTypeSpecNode *typeSpec;

  XDRDeclNode(XDRStringToken *_fieldName,
	      XDRTypeSpecNode *_typeSpec)
    : fieldName(_fieldName),
      typeSpec(_typeSpec) {
    fieldName->ref();
    typeSpec->ref();
  }
protected:
  ~XDRDeclNode(void) {
    fieldName->unref();
    typeSpec->unref();
  }
};

typedef XDRListNode<XDRDeclNode> XDRDeclListNode;

struct XDRPassThroughCodeNode : public XDRParseNode {
  ParsePos beginPos, endPos;
  ostringstream code;
  char *codeStr;
  XDRPassThroughCodeNode(ParsePos _beginPos)
    : beginPos(_beginPos) {
    codeStr = 0;
  }
  string getCode(void) {
    if (0 == codeStr) codeStr = strdup(code.str().c_str());
    return codeStr;
  }
protected:
  ~XDRPassThroughCodeNode(void) {
    delete codeStr;
  }
};

struct XDRTypeSpecStructNode : public XDRTypeSpecNode {
  XDRDeclListNode *decls;
  XDRPassThroughCodeNode *afterDecls;
  XDRTypeSpecStructNode(XDRDeclListNode *_decls)
    : XDRTypeSpecNode(T_STRUCT),
      decls(_decls) {
    decls->ref();
    afterDecls = 0;
  }
  void addAfterDecls(XDRPassThroughCodeNode *_afterDecls) {
    assert(0 == afterDecls);
    afterDecls = _afterDecls;
    afterDecls->ref();
  }

protected:
  ~XDRTypeSpecStructNode(void) {
    decls->unref();
    if (0 != afterDecls) afterDecls->unref();
  }
};

struct XDREnumEntryNode : public XDRParseNode {
  XDRStringToken *ident;
  XDRLongToken *value;
  XDREnumEntryNode(XDRStringToken *_ident)
    : ident(_ident) {
    ident->ref();
    value = 0;
  }
  void addValue(XDRLongToken *_value) {
    assert(0 == value);
    value = _value;
    value->ref();
  }
protected:
  ~XDREnumEntryNode(void) {
    ident->unref();
    if (0 != value) value->unref();
  }
};

typedef XDRListNode<XDREnumEntryNode> XDREnumListNode;

struct XDRTypeSpecEnumNode : public XDRTypeSpecNode {
  XDREnumListNode *enumList;
  XDRTypeSpecEnumNode(XDREnumListNode *_enumList)
    : XDRTypeSpecNode(T_ENUM),
      enumList(_enumList) {
    enumList->ref();
  }
protected:
  ~XDRTypeSpecEnumNode(void) {
    enumList->unref();
  }
};

typedef XDRDeclNode XDRTypeDefNode;

enum XDRDefinitionEnum {
  DEF_TYPEDEF,
  DEF_IPC_TYPE,
  DEF_CODE
};

struct XDRDefinitionNode : public XDRParseNode {
  XDRDefinitionEnum defType;
  XDRDefinitionNode(XDRDefinitionEnum _defType)
    : defType(_defType) { }
};

struct XDRDefinitionTypeDefNode : public XDRDefinitionNode {
  XDRTypeDefNode *typeDef;
  XDRDefinitionTypeDefNode(XDRTypeDefNode *_typeDef)
    : XDRDefinitionNode(DEF_TYPEDEF),
      typeDef(_typeDef) {
    typeDef->ref();
  }
protected:
  ~XDRDefinitionTypeDefNode(void) {
    typeDef->unref();
  }
};

struct XDRDefinitionIPCTypeNode : public XDRDefinitionNode {
  XDRIPCTypeNode *ipcType;
  XDRDefinitionIPCTypeNode(XDRIPCTypeNode *_ipcType)
    : XDRDefinitionNode(DEF_IPC_TYPE),
      ipcType(_ipcType) {
    ipcType->ref();
  }
protected:
  ~XDRDefinitionIPCTypeNode(void) {
    ipcType->unref();
  }
};

struct XDRDefinitionCodeNode : public XDRDefinitionNode {
  XDRPassThroughCodeNode *code;
  XDRDefinitionCodeNode(XDRPassThroughCodeNode *_code)
    : XDRDefinitionNode(DEF_CODE),
      code(_code) {
    code->ref();
  }
protected:
  ~XDRDefinitionCodeNode(void) {
    code->unref();
  }
};

typedef XDRListNode<XDRDefinitionNode> XDRDefinitionListNode;

struct XDRSpecification : public XDRParseNode {
  XDRDefinitionListNode *defs;
  string specFileName;
  /* results of fstat() on the spec file at end of parsing */
  struct stat specFileStat;
  XDRSpecification(XDRDefinitionListNode *_defs, string _specFileName)
    : defs(_defs),
      specFileName(_specFileName) {
    defs->ref();
  }
protected:
  ~XDRSpecification(void) {
    defs->unref();
  }
};

/***************************************************************************
 * FUNCTIONS AND EXTERN VARIABLES
 ***************************************************************************/

ostream &operator <<(ostream &out, XDRPassThroughCodeNode *code);

#endif // INCXDRTree_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRTree.h,v $
 * Revision 1.6  2011/08/17 01:11:43  reids
 * Took out unused and LISP-specific functions
 *
 * Revision 1.5  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.4  2004/04/06 15:06:09  trey
 * updated for more recent bison and g++
 *
 * Revision 1.3  2002/05/09 01:46:24  trey
 * added --lang=lisp option for xdr
 *
 * Revision 1.2  2002/02/14 21:46:29  reids
 * Incorporating various changes that Trey made in the FIRE version
 *
 * Revision 1.2  2001/11/26 22:17:43  trey
 * fixed line directives bug
 *
 * Revision 1.1  2001/03/16 17:56:05  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.5  2001/02/08 04:14:57  trey
 * added enumerated types and fixed C language output
 *
 * Revision 1.4  2001/02/08 00:41:58  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.3  2001/02/06 04:14:16  trey
 * example.xdr
 *
 * Revision 1.2  2001/02/06 02:01:55  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:51  trey
 * initial check-in
 *
 *
 ***************************************************************************/

/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.2 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  
 *
 * (c) Copyright 2001 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

#ifndef INCXDRIterator_h
#define INCXDRIterator_h

#include "XDRTree.h"
#include <stack>

class XDRTypeIterator {
public:
  virtual ~XDRTypeIterator(void) {}
  void iterateTypeSpec(XDRTypeSpecNode *typeSpec);

protected:
  stack<XDRTypeSpecNode *> typeSpecContextStack;
  stack<XDRTypeSpecStructNode *> structContextStack;
  stack<XDRDeclNode *> structFieldContextStack;

  virtual void beginTypeSpec(XDRTypeSpecNode *typeSpec) {}
  virtual void endTypeSpec(XDRTypeSpecNode *typeSpec) {}
  virtual void beginStruct(XDRTypeSpecStructNode *structNode) {}
  virtual void endStruct(XDRTypeSpecStructNode *structNode) {}
  virtual void beginStructField(XDRDeclNode *decl, int index, int numDecls) {}
  virtual void endStructField(XDRDeclNode *decl, int index, int numDecls) {}
  virtual void beforeArrayDims(XDRTypeSpecNode *typeSpec) {}
  virtual void processArrayDim(XDRArrayDimNode *arrayDim, int index,
			       int numArrayDims) {}
  virtual void beginEnumSpec(XDRTypeSpecEnumNode *typeEnum) {}
  virtual void endEnumSpec(XDRTypeSpecEnumNode *typeEnum) {}
  virtual void processEnum(XDREnumEntryNode *entry, int index,
			   int numEntries) {}

  void iterateDeclList(XDRDeclListNode *decls);

  XDRTypeSpecNode *getTypeSpecContext(void) {
    return typeSpecContextStack.top();
  }
  XDRTypeSpecStructNode *getStructContext(void) {
    return structContextStack.top();
  }
  virtual XDRDeclNode *getStructFieldContext(void) {
    return structFieldContextStack.top();
  }
};

class XDRIterator : public XDRTypeIterator {
public:
  void iterate(XDRSpecification *spec);

protected:
  XDRSpecification *theSpec;
  XDRDeclNode *typeDefContext;

  virtual void beginSpec(XDRSpecification *spec) {}
  virtual void endSpec(XDRSpecification *spec) {}
  virtual void beginTypeDef(XDRDeclNode *decl, int index, int numDefs) {}
  virtual void endTypeDef(XDRDeclNode *decl, int index, int numDefs) {}
  virtual void processIPCType(XDRIPCTypeNode *ipcType, int index,
			      int numDefs) {}
  virtual void processCode(XDRPassThroughCodeNode *code, int index,
			   int numDefs) {}

  void iterateDefinitionList(XDRDefinitionListNode *decls);

  XDRSpecification *getSpecificationContext(void) {
    return theSpec;
  }
  XDRDeclNode *getTypeDefContext(void) {
    return typeDefContext;
  }
  XDRDeclNode *getDeclContext(void) {
    if (structFieldContextStack.empty()) return typeDefContext;
    else return structFieldContextStack.top();
  }
};

#endif // INCXDRIterator_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRIterator.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:03  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.3  2001/02/08 04:14:56  trey
 * added enumerated types and fixed C language output
 *
 * Revision 1.2  2001/02/08 00:41:57  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.1  2001/02/05 21:10:48  trey
 * initial check-in
 *
 *
 ***************************************************************************/

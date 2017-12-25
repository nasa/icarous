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

#ifndef INCXDROutputLisp_h
#define INCXDROutputLisp_h

#include "XDRIterator.h"
#include "XDRFormatIPC.h"
#include <stack>

class XDROutputLisp : public XDRIterator {
public:
  XDROutputLisp(ostream &_out, const char *_outFileName)
    : out(_out),
      outFileName(_outFileName == 0 ? "" : _outFileName) { }

protected:
  ostream &out;
  string outFileName;
  XDRFormatIPC ipcFormatter;
  int indentLevel;
  void setIndentLevel(int newIndentLevel);
  string indent;
  string includeGuardName;
  
  virtual void beginSpec(XDRSpecification *spec);
  virtual void endSpec(XDRSpecification *spec);
  virtual void beginTypeDef(XDRDeclNode *decl, int index, int numTypeDefs);
  virtual void endTypeDef(XDRDeclNode *decl, int index, int numTypeDefs);
  virtual void beginStructField(XDRDeclNode *decl, int index, int numDecls);
  virtual void endStructField(XDRDeclNode *decl, int index, int numDecls);
  virtual void beginTypeSpec(XDRTypeSpecNode *typeSpec);
  virtual void endTypeSpec(XDRTypeSpecNode *typeSpec);
  virtual void beginStruct(XDRTypeSpecStructNode *structNode);
  virtual void endStruct(XDRTypeSpecStructNode *structNode);
  virtual void beforeArrayDims(XDRTypeSpecNode *typeSpec);
  virtual void processArrayDim(XDRArrayDimNode *arrayDim, int index,
			       int numArrayDims);
  virtual void processCode(XDRPassThroughCodeNode *code, int index,
			   int numDefs);
  virtual void beginEnumSpec(XDRTypeSpecEnumNode *typeEnum);
  virtual void endEnumSpec(XDRTypeSpecEnumNode *typeEnum);
  virtual void processEnum(XDREnumEntryNode *entry, int index,
			   int numEntries);

  void outputFormatDefinition(XDRDeclNode *typeDef);
};

#endif // INCXDROutputLisp_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDROutputLisp.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2002/05/09 01:46:04  trey
 * initial check-in
 *
 *
 ***************************************************************************/

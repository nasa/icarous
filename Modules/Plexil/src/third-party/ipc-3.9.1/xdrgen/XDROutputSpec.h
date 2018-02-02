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

#ifndef INCXDROutputSpec_h
#define INCXDROutputSpec_h

#include "XDRIterator.h"
#include <stack>

class XDROutputSpec : public XDRIterator {
public:
  XDROutputSpec(ostream &_out)
    : out(_out) { }

protected:
  ostream &out;
  int indentLevel;
  void setIndentLevel(int newIndentLevel);
  string indent;
  
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
};

#endif // INCXDROutputSpec_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDROutputSpec.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:04  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.1  2001/02/05 21:10:49  trey
 * initial check-in
 *
 *
 ***************************************************************************/

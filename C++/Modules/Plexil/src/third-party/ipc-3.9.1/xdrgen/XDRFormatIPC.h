/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.3 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  
 *
 * (c) Copyright 2001 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

#ifndef INCXDRFormatIPC_h
#define INCXDRFormatIPC_h

#include "XDRIterator.h"
#include <stack>

class XDRFormatIPC : public XDRTypeIterator {
public:
  XDRFormatIPC(void) { }
  string getFormat(XDRDeclNode *_typeDef);
  void outputFormat(XDRDeclNode *_typeDef, ostream *_out);

protected:
  ostream *out;
  XDRDeclNode *typeDef;
  bool useEnumStyle;
  unsigned int structFieldIndex;

  virtual void beginStructField(XDRDeclNode *decl, int index, int numDecls);
  virtual void endStructField(XDRDeclNode *decl, int index, int numDecls);
  virtual void beginTypeSpec(XDRTypeSpecNode *typeSpec);
  virtual void endTypeSpec(XDRTypeSpecNode *typeSpec);
  virtual void beginStruct(XDRTypeSpecStructNode *structNode);
  virtual void endStruct(XDRTypeSpecStructNode *structNode);
  virtual void beforeArrayDims(XDRTypeSpecNode *typeSpec);
  virtual void processArrayDim(XDRArrayDimNode *arrayDim, int index,
			       int numArrayDims);
  virtual void beginEnumSpec(XDRTypeSpecEnumNode *typeEnum);
  virtual void endEnumSpec(XDRTypeSpecEnumNode *typeEnum);
  virtual void processEnum(XDREnumEntryNode *entry, int index,
			   int numEntries);
};

class DebugFormatIPC : public XDRIterator {
public:
  DebugFormatIPC(ostream &_out) : out(_out) { }
  ~DebugFormatIPC(void) { }
protected:
  XDRFormatIPC formatter;
  ostream &out;

  virtual void beginTypeDef(XDRDeclNode *decl, int index, int numTypeDefs);
};

#endif // INCXDRFormatIPC_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRFormatIPC.h,v $
 * Revision 1.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2002/02/14 21:46:29  reids
 * Incorporating various changes that Trey made in the FIRE version
 *
 * Revision 1.2  2001/11/17 03:48:59  trey
 * extended permissible var-array syntax, because why not?
 *
 * Revision 1.1  2001/03/16 17:56:02  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/02/08 04:14:56  trey
 * added enumerated types and fixed C language output
 *
 * Revision 1.1  2001/02/05 21:10:47  trey
 * initial check-in
 *
 *
 ***************************************************************************/

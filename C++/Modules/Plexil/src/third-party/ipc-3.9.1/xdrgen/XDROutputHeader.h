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

#ifndef INCXDROutputHeader_h
#define INCXDROutputHeader_h

#include "XDRIterator.h"
#include "XDRFormatIPC.h"
#include <stack>

class XDROutputHeader : public XDRIterator {
public:
  XDROutputHeader(ostream &_out, const char *_outFileName,
		  HeaderLangEnum _lang)
    : out(_out),
      outFileName(_outFileName == 0 ? "" : _outFileName),
      lang(_lang) { }

protected:
  ostream &out;
  string outFileName;
  HeaderLangEnum lang;
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

#endif // INCXDROutputHeader_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDROutputHeader.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:04  reids
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
 * Revision 1.1  2001/02/05 21:10:49  trey
 * initial check-in
 *
 *
 ***************************************************************************/

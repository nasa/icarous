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

#ifndef INCXDRMapNames_h
#define INCXDRMapNames_h

#include "XDRIterator.h"
#include <map>

class XDRMapNames : public XDRIterator {
public:
  XDRMapNames(void) {}

protected:
  typedef map<string, XDRIPCTypeNode *> XDRNameSpace;
  XDRNameSpace nmap;

  virtual ~XDRMapNames(void);

  virtual void beginTypeDef(XDRDeclNode *decl, int index, int numDefs);
  virtual void beginTypeSpec(XDRTypeSpecNode *typeSpec);
  virtual void processIPCType(XDRIPCTypeNode *ipcType, int index,
			      int numDefs);
};

#endif // INCXDRMapNames_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRMapNames.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:03  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/02/08 00:41:58  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.1  2001/02/05 21:10:48  trey
 * initial check-in
 *
 *
 ***************************************************************************/

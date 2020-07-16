/****************************************************************************
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

/***************************************************************************
 * INCLUDES
 ***************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include "XDRMapNames.h"
#include "ParseError.h"

/***************************************************************************
 * GLOBAL VARIABLES AND STATIC CLASS MEMBERS
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

XDRMapNames::~XDRMapNames(void) {
  XDRNameSpace::iterator nmi;
  for (nmi = nmap.begin(); nmi != nmap.end(); nmi++) {
    nmi->second->unref();
  }
}

void
XDRMapNames::beginTypeDef(XDRDeclNode *decl, int index, int numDefs) {
  XDRIPCTypeNode *ipcType = new XDRIPCTypeNode(decl->fieldName);
  ipcType->ref();
  nmap[decl->fieldName->val] = ipcType;
}

void
XDRMapNames::beginTypeSpec(XDRTypeSpecNode *typeSpec) {
  if (T_IDENT == typeSpec->type) {
    XDRTypeSpecIdentNode *ts = (XDRTypeSpecIdentNode *) typeSpec;
    XDRIPCTypeNode *ipcType;

    XDRNameSpace::iterator nmi = nmap.find(ts->ident->val);
    if (nmap.end() == nmi) {
      string errorText = "undeclared type `" + ts->ident->val + "'";
      cerr << ParseError(errorText, ts->ident->pos,
			 /* showToken = */ false).asWarning();
      ipcType = new XDRIPCTypeNode(ts->ident);
    } else {
      ipcType = nmi->second;
    }
    ts->addResolvedType(ipcType);
  }
}

void
XDRMapNames::processIPCType(XDRIPCTypeNode *ipcType, int index, int numDefs) {
  ipcType->ref();
  nmap[ipcType->typeName->val] = ipcType;
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRMapNames.cc,v $
 * Revision 1.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2004/04/06 15:06:08  trey
 * updated for more recent bison and g++
 *
 * Revision 1.1  2001/03/16 17:56:03  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/02/08 00:41:57  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.1  2001/02/05 21:10:48  trey
 * initial check-in
 *
 *
 ***************************************************************************/

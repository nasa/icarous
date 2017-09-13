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

#include "XDRIterator.h"

/***************************************************************************
 * GLOBAL VARIABLES AND STATIC CLASS MEMBERS
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

void
XDRIterator::iterate(XDRSpecification *spec) {
  theSpec = spec;
  beginSpec(spec);
  iterateDefinitionList(spec->defs);
  endSpec(spec);
}

void
XDRIterator::iterateDefinitionList(XDRDefinitionListNode *decls) {
  const list<XDRDefinitionNode *> &ditems = decls->getItems();
  list<XDRDefinitionNode *>::const_iterator di;
  int i;
  for (di = ditems.begin(), i=0; di != ditems.end(); di++, i++) {
    switch ((*di)->defType) {
    case DEF_TYPEDEF:
      XDRTypeDefNode *typeDef;
      typeDef = ((XDRDefinitionTypeDefNode *) *di)->typeDef;
      typeDefContext = typeDef;
      beginTypeDef(typeDef,i,ditems.size());
      iterateTypeSpec(typeDef->typeSpec);
      endTypeDef(typeDef,i,ditems.size());
      break;
    case DEF_IPC_TYPE:
      processIPCType(((XDRDefinitionIPCTypeNode *) *di)->ipcType,
		     i,ditems.size());
      break;
    case DEF_CODE:
      processCode(((XDRDefinitionCodeNode *) *di)->code,
		  i,ditems.size());
      break;
    default: abort(); /* bad defType */
    }
  }
}

// identical to iterateTypeDefList except for which function gets called
void
XDRTypeIterator::iterateDeclList(XDRDeclListNode *decls) {
  const list<XDRDeclNode *> &ditems = decls->getItems();
  list<XDRDeclNode *>::const_iterator di;
  int i;
  for (di = ditems.begin(), i=0; di != ditems.end(); di++, i++) {
    structFieldContextStack.push(*di);
    beginStructField(*di,i,ditems.size());
    iterateTypeSpec((*di)->typeSpec);
    endStructField(*di,i,ditems.size());
    structFieldContextStack.pop();
  }
}

void
XDRTypeIterator::iterateTypeSpec(XDRTypeSpecNode *typeSpec) {
  int i;
  typeSpecContextStack.push(typeSpec);
  beginTypeSpec(typeSpec);

  if (T_STRUCT == typeSpec->type) {
    XDRTypeSpecStructNode *sn = (XDRTypeSpecStructNode *) typeSpec;
    structContextStack.push(sn);
    beginStruct(sn);
    iterateDeclList(sn->decls);
    endStruct(sn);
    structContextStack.pop();
  } else if (T_ENUM == typeSpec->type) {
    XDRTypeSpecEnumNode *en = (XDRTypeSpecEnumNode *) typeSpec;
    beginEnumSpec(en);
    const list<XDREnumEntryNode *> &eitems = en->enumList->getItems();
    list<XDREnumEntryNode *>::const_iterator ei;
    for (ei = eitems.begin(), i=0; ei != eitems.end(); ei++, i++) {
      processEnum(*ei,i,eitems.size());
    }
    endEnumSpec(en);
  } else {
    /* other types don't need deeper iteration */
  }

  beforeArrayDims(typeSpec);
  if (A_SINGLE != typeSpec->arrayEnum) {
    const list<XDRArrayDimNode *> &ditems = typeSpec->dims->getItems();
    list<XDRArrayDimNode *>::const_iterator di;
    for (di = ditems.begin(), i=0; di != ditems.end(); di++, i++) {
      processArrayDim(*di,i,ditems.size());
    }
    // special call for the last var-array dimension on a string declaration
    if (T_STRING == typeSpec->type) {
      processArrayDim(((XDRTypeSpecStringNode *) typeSpec)->dim,
		      ditems.size(), ditems.size());
    }
  } else {
    // special call for the last var-array dimension on a string declaration
    if (T_STRING == typeSpec->type) {
      processArrayDim(((XDRTypeSpecStringNode *) typeSpec)->dim, 0, 0);
    }
  }
  endTypeSpec(typeSpec);
  typeSpecContextStack.pop();
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRIterator.cc,v $
 * Revision 1.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2004/04/06 15:06:08  trey
 * updated for more recent bison and g++
 *
 * Revision 1.1  2001/03/16 17:56:02  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.4  2001/02/08 04:14:56  trey
 * added enumerated types and fixed C language output
 *
 * Revision 1.3  2001/02/08 00:41:57  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.2  2001/02/06 02:01:54  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:47  trey
 * initial check-in
 *
 * Revision 1.3  2001/01/29 19:39:27  trey
 * updated template year again
 *
 *
 ***************************************************************************/

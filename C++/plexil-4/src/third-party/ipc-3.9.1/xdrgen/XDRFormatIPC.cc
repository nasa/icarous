/****************************************************************************
 * $Revision: 1.4 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
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
#include <sstream>

#include "XDRFormatIPC.h"
#include "ParseError.h"

/***************************************************************************
 * GLOBAL VARIABLES AND STATIC CLASS MEMBERS
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

string
XDRFormatIPC::getFormat(XDRDeclNode *_typeDef) {
  typeDef = _typeDef;
  ostringstream *oss;
  out = oss = new ostringstream();
  iterateTypeSpec(typeDef->typeSpec);
  char *cretVal = strdup(oss->str().c_str());
  string retVal = cretVal;
  delete cretVal;
  delete oss;
  return retVal;
}

void
XDRFormatIPC::outputFormat(XDRDeclNode *_typeDef, ostream *_out) {
  typeDef = _typeDef;
  out = _out;
  iterateTypeSpec(typeDef->typeSpec);
  out = 0;
}

void
XDRFormatIPC::beginTypeSpec(XDRTypeSpecNode *typeSpec) {
  if (typeSpecContextStack.size() < 2) {
    (*out) << "\"";
  }

  if (A_SINGLE != typeSpec->arrayEnum) {
    (*out) << ((A_FIXED_ARRAY == typeSpec->arrayEnum) ? "[" : "<");

    if (A_VAR_ARRAY == typeSpec->arrayEnum) {
      // do some var-length array error checking
      string errorText;

      // first off, the var-array must be a field of a struct
      if (structFieldContextStack.empty()) {
	errorText = "variable-length array `"
	  + typeDef->fieldName->val
	  + "' declared outside a struct";
	cerr << ParseError(errorText, typeDef->fieldName->pos,
			   /* displayToken = */ false);
	exit(EXIT_FAILURE);
      }

      // next, if there are n var-array dimensions, there must be
      // n 'int' or 'unsigned int' fields preceding the var-array
      // field
      bool dimMismatch = false;
      const list<XDRDeclNode *> &structFields
	= getStructContext()->decls->getItems();
      const list<XDRArrayDimNode *> &varArrayDims
	= typeSpec->dims->getItems();
      unsigned int numIntFieldsRequired = varArrayDims.size();

      if (structFieldIndex < numIntFieldsRequired) {
	dimMismatch = true;
      } else {
	list<XDRDeclNode *>::const_iterator sfi;
	unsigned int i;
	XDRTypeEnum type;

	// skip fields preceding the ones required to be ints
	for (sfi = structFields.begin(), i=0;
	     i < structFieldIndex-numIntFieldsRequired; sfi++, i++);
	// start checking that they are indeed int fields
	for (; i < numIntFieldsRequired; sfi++, i++) {
	  type = (*sfi)->typeSpec->type;
	  if (! (T_INT == type || T_UINT == type)) {
	    dimMismatch = true;
	    break;
	  }
	}
      }
      if (dimMismatch) {
	errorText = "variable-length array `"
	  + getStructFieldContext()->fieldName->val
	  + "' isn't preceded by the right number of int or unsigned int fields";
	cerr << ParseError(errorText, getStructFieldContext()->fieldName->pos,
			   /* displayToken = */ false);
	exit(EXIT_FAILURE);
      }
    }
  }
  XDRIPCTypeNode *ipcType;
  switch (typeSpec->type) {
  case T_STRUCT:
    /* do nothing */
    break;
  case T_ENUM:
    /* do nothing */
    break;
  case T_IDENT:
    ipcType = ((XDRTypeSpecIdentNode *)typeSpec)->resolvedType;
    /* FIX need to fix quoting before this will really work */
    if (0 != ipcType->formatString) {
      (*out) << ipcType->formatString->val;
    } else {
      (*out) << "\" " << ipcType->typeName->val
	     << "_IPC_FORMAT \"";
    }
    break;
  default:
    (*out) << typeSpec->nameOfTypeIPC();
  }
}

void
XDRFormatIPC::endTypeSpec(XDRTypeSpecNode *typeSpec) {
  if (A_SINGLE != typeSpec->arrayEnum) {
    (*out) << ((A_FIXED_ARRAY == typeSpec->arrayEnum) ? "]" : ">");
  }
  if (typeSpecContextStack.size() < 2) {
    (*out) << "\"";
  }
}

void
XDRFormatIPC::beginStructField(XDRDeclNode *decl, int index, int numDecls) {
  structFieldIndex = index;
}

void
XDRFormatIPC::endStructField(XDRDeclNode *decl, int index, int numDecls) {
  if (index < numDecls-1) (*out) << ",";
}

void
XDRFormatIPC::beginStruct(XDRTypeSpecStructNode *structNode) {
  (*out) << "{";
}

void
XDRFormatIPC::endStruct(XDRTypeSpecStructNode *structNode) {
  (*out) << "}";
}

void
XDRFormatIPC::beginEnumSpec(XDRTypeSpecEnumNode *typeEnum) {
  // if all of the identifiers in the enum do not have associated
  // values, then we'll use the IPC format style "{enum A,B,C,D}".
  // otherwise, we'll just use "int".
  useEnumStyle = true;
  int i;
  const list<XDREnumEntryNode *> &eitems = typeEnum->enumList->getItems();
  list<XDREnumEntryNode *>::const_iterator ei;
  for (ei = eitems.begin(), i=0; ei != eitems.end(); ei++, i++) {
    if (0 != (*ei)->value) {
      useEnumStyle = false;
      break;
    }
  }

  if (useEnumStyle) {
    (*out) << "{enum ";
  } else {
    (*out) << "int";
  }
}

void
XDRFormatIPC::endEnumSpec(XDRTypeSpecEnumNode *typeEnum) {
  if (useEnumStyle) (*out) << "}";
}

void
XDRFormatIPC::processEnum(XDREnumEntryNode *entry, int index,
			  int numEntries) {
  if (useEnumStyle) {
    (*out) << entry->ident->val;
    if (index < numEntries-1) (*out) << ",";
  }
}

void
XDRFormatIPC::beforeArrayDims(XDRTypeSpecNode *typeSpec) {
  if (A_SINGLE != typeSpec->arrayEnum) (*out) << ":";
}

void
XDRFormatIPC::processArrayDim(XDRArrayDimNode *arrayDim, int index,
			      int numArrayDims) {
  // the special last var-array dimension after a string will generate
  //    a call to processArrayDim with index == numArrayDims; we
  //    don't want to output that one in the IPC format
  if (index < numArrayDims) {
    switch (getTypeSpecContext()->arrayEnum) {
    case A_FIXED_ARRAY:
      (*out) << arrayDim->maxLength->val;
      break;
    case A_VAR_ARRAY:
      (*out) << (structFieldIndex-numArrayDims+index+1);
      break;
    default: abort(); /* bad arrayEnum */
    };
    if (index < numArrayDims-1) (*out) << ",";
  }
}

void
DebugFormatIPC::beginTypeDef(XDRDeclNode *decl, int index, int numTypeDefs) {
  out << decl->fieldName->val << ": \"";
  formatter.outputFormat(decl,&out);
  out << "\"" << endl;
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRFormatIPC.cc,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2004/04/06 15:06:08  trey
 * updated for more recent bison and g++
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
 *
 ***************************************************************************/

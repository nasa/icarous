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

#include "XDRTree.h"

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

const char *
XDRTypeSpecNode::nameOfType(XDRTypeEnum tenum) {
  switch (tenum) {
  case T_STRUCT: return "struct"; break;
  case T_STRING: return "string"; break;
  case T_ENUM: return "enum"; break;
  case T_UCHAR: return "unsigned char"; break;
  case T_CHAR: return "char"; break;
  case T_UINT: return "unsigned int"; break;
  case T_INT: return "int"; break;
  case T_BOOL: return "bool"; break;
  case T_FLOAT: return "float"; break;
  case T_DOUBLE: return "double"; break;
  default: abort(); /* bad tenum value */
  }
}

const char *
XDRTypeSpecNode::nameOfTypeHeader(XDRTypeEnum tenum, HeaderLangEnum lang) {
  switch (tenum) {
  case T_STRING: return "char";
  default: return nameOfType(tenum);
  }
}

const char *
XDRTypeSpecNode::nameOfTypeIPC(XDRTypeEnum tenum) {
  switch (tenum) {
  case T_STRING: return "string"; break;
  case T_UCHAR: return "uchar"; break;
  case T_CHAR: return "char"; break;
  case T_UINT: return "uint"; break;
  case T_INT: return "int"; break;
  case T_BOOL: return "char"; break;
  case T_FLOAT: return "float"; break;
  case T_DOUBLE: return "double"; break;
  default: abort(); /* bad tenum value */
  }
}

const char *
XDRTypeSpecNode::nameOfType(void) {
  if (T_IDENT == type) {
    return ((XDRTypeSpecIdentNode *) this)->ident->val.c_str();
  } else {
    return nameOfType(type);
  }
}

const char *
XDRTypeSpecNode::nameOfTypeHeader(HeaderLangEnum lang) {
  if (T_IDENT == type) {
    return ((XDRTypeSpecIdentNode *) this)->ident->val.c_str();
  } else {
    return nameOfTypeHeader(type,lang);
  }
}

ostream &
operator <<(ostream &out, XDRPassThroughCodeNode *code) {
  if (0 == code) return out;

  const string &s = code->getCode();
  // if the first line is empty except for whitespace, don't print it
  int firstPrint = 0;
  int firstNonWS = s.find_first_not_of(" \t");
  if (s[firstNonWS] == '\n') {
    firstPrint = firstNonWS+1;
    // correct for the missing \n
    code->beginPos.lineNumber--;
  }
  // similarly, don't print trailing whitespace
  int lastPrint = s.find_last_not_of(" \t");
  code->beginPos.putLineDirective(out);
  out << s.substr(firstPrint, lastPrint-firstPrint+1);
  // i can't think of a good way to report where errors in the generated
  // code come from.  TDLC prints out the position in the generated file,
  // but i'm not sure how to get the current line number from an ostream.
  // this will pretend that the generated code comes from somewhere in
  // the xdr spec file which hopefully should be close to the spec text
  // that corresponds to the code.
  code->endPos.putLineDirective(out, /* enteringFile = */ false);
  return out;
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRTree.cc,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2004/04/06 15:06:09  trey
 * updated for more recent bison and g++
 *
 * Revision 1.2  2001/11/14 17:58:54  reids
 * Incorporating various changes that Trey made in the DIRA/FIRE versions.
 *
 * Revision 1.2  2001/11/06 22:01:52  trey
 * changed bool format for binary compatibility with C++ bool representation
 *
 * Revision 1.1  2001/03/16 17:56:05  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.6  2001/02/13 23:41:06  trey
 * changed IPC formats to use uchar instead of ubyte (just more straightforward)
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
 * Revision 1.1  2001/02/05 21:10:50  trey
 * initial check-in
 *
 *
 ***************************************************************************/


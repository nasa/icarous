/********** tell emacs we use -*- c++ -*- style comments *******************
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

#ifndef INCParsePos_h
#define INCParsePos_h

#include "RefCounter.h"
#include <string>
#include <vector>

using namespace std;

enum HeaderDirectivesEnum {
  DIRECTIVES_GCC, DIRECTIVES_PORTABLE, DIRECTIVES_NONE
};

struct ParseFilePos {
  string fileName;
  int lineNumber;

  ParseFilePos(string _fileName, int _lineNumber)
    : fileName(_fileName), lineNumber(_lineNumber) { }
};

struct ParsePosSharedInfo : public RefCounter {
  vector<ParseFilePos> stack;
  string fileName;

  ParsePosSharedInfo(string _fileName)
    : fileName(_fileName) { }
};

struct ParsePos {
  static HeaderDirectivesEnum directivesMode;

  string beforeToken;
  int lineNumber;
  ParsePosSharedInfo *sharedInfo;

  ParsePos(void) { sharedInfo = 0; }
  ParsePos(string _beforeToken, int _lineNumber,
	   ParsePosSharedInfo *_sharedInfo)
    : beforeToken(_beforeToken),
      lineNumber(_lineNumber),
      sharedInfo(_sharedInfo) {
    sharedInfo->ref();
  }
  ParsePos(const ParsePos &pr)
    : beforeToken(pr.beforeToken),
      lineNumber(pr.lineNumber),
      sharedInfo(pr.sharedInfo) {
    sharedInfo->ref();
  }
  ~ParsePos(void) {
    if (0 != sharedInfo) sharedInfo->unref();
  }
  string getFileName(void) { return sharedInfo->fileName; }

  static void putLineDirective(ostream &out, int _lineNumber,
			       const string& _fileName,
			       bool enteringFile = true,
			       bool portableMode = false);
  void putLineDirective(ostream &out, bool enteringFile = true,
			bool portableMode = false);
};

#endif // INCParsePos_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: ParsePos.h,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2003/02/13 20:47:18  reids
 * Updated to compile under gcc 3.0.1
 *
 * Revision 1.2  2002/02/14 21:46:29  reids
 * Incorporating various changes that Trey made in the FIRE version
 *
 * Revision 1.2  2001/11/26 22:17:42  trey
 * fixed line directives bug
 *
 * Revision 1.1  2001/03/16 17:56:01  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/02/06 02:01:54  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:46  trey
 * initial check-in
 *
 *
 ***************************************************************************/

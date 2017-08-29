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

#ifndef INCParseError_h
#define INCParseError_h

#include <string>
#include "ParsePos.h"

struct ParseError {
  string errorText;
  ParsePos pos;
  bool showToken;
  bool isWarning;

  ParseError(string _errorText, ParsePos _pos,
	     bool _showToken = true)
    : errorText(_errorText),
      pos(_pos),
      showToken(_showToken) {
    isWarning = false;
  }
  const ParseError &asWarning(void) {
    isWarning = true;
    return *this;
  }
  friend ostream &operator<<(ostream &errorStream, const ParseError &perror);
};

#endif // INCParseError_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: ParseError.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:00  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/02/08 00:41:56  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.1  2001/02/05 21:10:45  trey
 * initial check-in
 *
 *
 ***************************************************************************/

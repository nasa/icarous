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

#include "ParseError.h"

/***************************************************************************
 * MACROS
 ***************************************************************************/

/***************************************************************************
 * CLASSES AND TYPEDEFS
 ***************************************************************************/

/***************************************************************************
 * GLOBAL VARIABLES AND STATIC CLASS MEMBERS
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

// Error format (stolen from gcc) --
// In file included from foo.config:5,
//                  from my2.config:1:
// goo.config:3: before `zounds': parse error, expecting `LONG' or `INT'
ostream &
operator<<(ostream &errorStream, const ParseError &e) {
  vector<ParseFilePos> &stack = e.pos.sharedInfo->stack;
  vector<ParseFilePos>::reverse_iterator sti;
  for (sti=stack.rbegin(); sti != stack.rend(); sti++) {
    if (sti == stack.rbegin()) {
      errorStream << "In file included ";
    } else {
      errorStream << "                 ";
    }
    errorStream << "from " << sti->fileName << ":" << sti->lineNumber
		<< ((sti+1 == stack.rend()) ? ":" : ",") << endl;

  }
  errorStream << e.pos.sharedInfo->fileName << ":"
	      << e.pos.lineNumber << ":";
  if (e.isWarning) errorStream << " warning:";
  if (e.showToken) {
    errorStream << " before `" << e.pos.beforeToken << "':";
  }
  errorStream << " " << e.errorText << endl;
  return errorStream;
}


/***************************************************************************
 * REVISION HISTORY:
 * $Log: ParseError.cc,v $
 * Revision 1.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2004/04/06 15:06:08  trey
 * updated for more recent bison and g++
 *
 * Revision 1.1  2001/03/16 17:56:00  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.3  2001/02/08 00:41:56  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.2  2001/02/06 02:01:54  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:45  trey
 * initial check-in
 *
 *
 ***************************************************************************/

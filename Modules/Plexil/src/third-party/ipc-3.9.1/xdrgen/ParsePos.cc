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

#include "ParsePos.h"

/***************************************************************************
 * STATIC MEMBER VARIABLES
 ***************************************************************************/

HeaderDirectivesEnum ParsePos::directivesMode;

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

void
ParsePos::putLineDirective(ostream &out, int _lineNumber,
			   const string& _fileName, bool enteringFile,
			   bool portableMode) {
  int mode = directivesMode;
  // if portableMode arg is set, override mode setting
  if (DIRECTIVES_NONE != mode && portableMode) mode = DIRECTIVES_PORTABLE;

  switch (mode) {
  case DIRECTIVES_GCC:
    out << "# " << _lineNumber << " \"" << _fileName
	<< "\" " << (enteringFile ? 1 : 2);
    break;
  case DIRECTIVES_PORTABLE:
    out << "#line " << _lineNumber << " \"" << _fileName << "\"";
    break;
  case DIRECTIVES_NONE:
    /* do nothing */
    break;
  default:
    abort(); /* never reach this point */
  }
}

void
ParsePos::putLineDirective(ostream &out, bool enteringFile,
			   bool portableMode) {
  putLineDirective(out, lineNumber, sharedInfo->fileName.c_str(),
		   enteringFile, portableMode);
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: ParsePos.cc,v $
 * Revision 1.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2002/02/14 21:46:27  reids
 * Incorporating various changes that Trey made in the FIRE version
 *
 * Revision 1.2  2001/11/26 22:17:42  trey
 * fixed line directives bug
 *
 * Revision 1.1  2001/03/16 17:56:00  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.1  2001/02/06 02:00:54  trey
 * initial check-in
 *
 * Revision 1.3  2001/01/29 19:39:27  trey
 * updated template year again
 *
 *
 ***************************************************************************/

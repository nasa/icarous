/****************************************************************************
 * $Revision: 1.4 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  
 *
 * (c) Copyright 1999 CMU. All rights reserved.
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

#include "XDRParser.h"
#include "XDRInternal.h"  // yyin, yyTree
#include "runCPP.h"

/***************************************************************************
 * LOCAL MACROS and DEFINES
 ***************************************************************************/

/****************************************************************************
 * FUNCTION:	XDRParser
 * DESCRIPTION: constructor
 ****************************************************************************/
XDRParser::XDRParser(void) {
  syntaxTree = 0;
  verbose = true;
}

/****************************************************************************
 * FUNCTION:	parseFile
 * DESCRIPTION: parses FILENAME and caches the syntax tree
 * INPUTS:    	CPPENVVARIABLES is a null-terminated array of strings
 *              which name environment variables whose values should be
 *              passed to CPP using -Dvar=value flags.  Use 0 to indicate
 *              an empty list.
 * RETURN:	false on error, true otherwise
 ****************************************************************************/
bool
XDRParser::parseFile(const char *xdrFile, list<string> &cppArgs) {
  yyinName = xdrFile;
#ifdef XDRCPP
  /* uncomment this to use the C pre-processor on the file.
     i've disabled it so that the pass-through code sections in
     the xdr spec can define macros and include system headers.
     this seems to be more important than having macros processed
     before the xdr parser runs. */
  int fd;
  if (-1 == (fd = runCPP(xdrFile,cppArgs))) {
    if (verbose) {
      fprintf(stderr, "XDRParser::parseFile: couldn't run cpp on `%s'.\n",
	      xdrFile);
    }
    return false;
  }
  if (0 == (yyin = fdopen(fd,"r"))) {
    if (verbose) {
      fprintf(stderr,"XDRParser::parseFile: couldn't fdopen cpp pipe "
	      "(reading `%s').\n",xdrFile);
    }
    return false;
  }
#if 0
  // dump cpp output
  char lineBuf[1024];
  printf("DEBUG: dumping the cpp output\n");
  while (fgets(lineBuf,1024,yyin)) {
    printf(lineBuf);
  }
  printf("DEBUG: done dumping cpp output\n");
  return false;
#endif

#else
  if (0 == (yyin = fopen(xdrFile, "r"))) {
    cerr << "XDRParser::parseFile: couldn't open `" << xdrFile
	 << "' for reading" << endl;
    return false;
  }
#endif

#if 0
  // get parser trace
  yydebug = 1;
#endif

  lexinit();
  if (0 != yyparse()) {
    if (verbose) {
      fprintf(stderr, "XDRParser::parseFile: parsing of `%s' unsuccessful.\n",
	      xdrFile);
    }
    return false;
  }

  // the bison parser sets this global variable
  syntaxTree = yyTree;

  return true;
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRParser.cc,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2004/04/06 15:06:09  trey
 * updated for more recent bison and g++
 *
 * Revision 1.2  2001/11/14 17:58:54  reids
 * Incorporating various changes that Trey made in the DIRA/FIRE versions.
 *
 * Revision 1.1  2001/03/16 17:56:05  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.3  2001/03/21 19:26:15  trey
 * re-enabled using runCPP call to pre-process input file before parsing
 *
 * Revision 1.2  2001/02/06 02:01:55  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:50  trey
 * initial check-in
 *
 *
 ***************************************************************************/

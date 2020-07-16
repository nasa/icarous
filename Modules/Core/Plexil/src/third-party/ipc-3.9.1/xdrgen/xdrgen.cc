/****************************************************************************
 * $Revision: 1.7 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
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

#include <fstream>
#include <iostream>
using namespace std;

#include "XDRParser.h"
#include "XDROutputSpec.h"
#include "XDRMapNames.h"
#include "XDRFormatIPC.h"
#include "XDROutputHeader.h"
#include "XDROutputLisp.h"
#include "xdrgen.h"

/***************************************************************************
 * GLOBAL VARIABLES AND STATIC MEMBER FIELDS
 ***************************************************************************/

string defaultLangG = "c++";
string xdrgenVersionG = "$Revision: 1.7 $ $Date: 2009/01/12 15:54:59 $";
string defaultDirectivesG = "portable";

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

/*

I believe I've fixed the xdrgen problem we were seeing with newer gcc
versions.  The way cpp parses line number directives seems to have
changed.  In case it changes again, I've put in a new xdrgen option,

   --directives=[gcc,portable,none]

- if the setting is "gcc", the directives should give the most useful
  debugging output in case of an error, and should work with both 2.95
  and newer versions of gcc.  but there is no telling when the parsing
  might change again.  this is the default.

- if the setting is "portable", the debugging output will be less nice
  because it won't include a stack trace of where the erroneous file was
  included from.  but it uses the more portable #line directive, which
  should work under (for instance) IRIX cc, and is less likely to change
  out from under us.

- if the setting is none, no line number directives are output.

*/

void
usage(void) {
  cerr << "usage: xdrgen OPTIONS <xdrFile> [outputHeaderFile]" << endl
       << " -h or --help        Print this help" << endl
       << " --lang=[c,c++,lisp] Change language for header output" << endl
       << "                     (default: " << defaultLangG << ")" << endl
       << " --directives=[gcc,portable,none]" << endl
       << "                     Control syntax of #line directives" << endl
       << "                     that help to trace location of errors" << endl
       << "                     (default: " << defaultDirectivesG << ")" << endl
       << " -D<name>=<value>, -I<directory>" << endl
       << "                     These options are passed on to the C" << endl
       << "                     pre-processor before xdrgen parses the" << endl
       << "                     input file (see cpp(1) or 'info cpp')" << endl
       << "### DEBUGGING OPTIONS" << endl
       << " -v or --verbose     Talk about parse steps" << endl
       << " -n or --no-header   Don't output the header" << endl
       << " --spec              Print out parse tree as XDR spec" << endl
       << " --formats           Print out IPC formats for all types" << endl;
  exit(EXIT_FAILURE);
}

static HeaderLangEnum
langFromString(string langString) {
  if (langString == "c++") {
    return LANG_CPLUSPLUS;
  } else if (langString == "c") {
    return LANG_C;
  } else if (langString == "lisp") {
    return LANG_LISP;
  } else {
    cerr << "xdrgen: unknown language " << langString << " specified"
	 << endl;
    usage();
    return LANG_C; // will never reach here
  }
}

static HeaderDirectivesEnum
directivesFromString(string directivesString) {
  if (directivesString == "gcc") {
    return DIRECTIVES_GCC;
  } else if (directivesString == "portable") {
    return DIRECTIVES_PORTABLE;
  } else if (directivesString == "none") {
    return DIRECTIVES_NONE;
  } else {
    cerr << "xdrgen: unknown --directives syntax " << directivesString << endl;
    usage();
    return DIRECTIVES_GCC; // will never reach here
  }
}

int
main(int argc, char *argv[]) {
  int argi;
  string args;
  bool pastOptions = false;
  bool noHeader = false;
  bool debugPrintSpec = false;
  bool debugPrintFormats = false;
  bool verbose = false;
  HeaderLangEnum lang = langFromString(defaultLangG.c_str());
  char *inputFileName = 0;
  char *outputFileName = 0;
  list<string> cppArgs;

  ParsePos::directivesMode = directivesFromString(defaultDirectivesG.c_str());
  for (argi=1; argi < argc; argi++) {
    args = argv[argi];

    if (args == "--") {
      pastOptions = true;
    }
    if (!pastOptions && args.find("-") == 0) {
      if (args == "-h" || args == "--help") {
	usage();
      } else if (args.find("--lang=") == 0) {
	string langString = args.substr(string("--lang=").size());
	lang = langFromString(langString);
      } else if (args.find("--directives=") == 0) {
	string directivesString = args.substr(string("--directives=").size());
	ParsePos::directivesMode = directivesFromString(directivesString);
      } else if (args == "-v" || args == "--verbose") {
	verbose = true;
      } else if (args == "-n" || args == "--no-header") {
	noHeader = true;
      } else if (args == "--spec") {
	debugPrintSpec = true;
      } else if (args == "--formats") {
	debugPrintFormats = true;
      } else if (   args.find("-D") == 0
		 || args.find("-I") == 0) {
	cppArgs.push_back(args);
      } else {
	cerr << "xdrgen: unknown option " << args << endl;
	usage();
      }
    } else {
      if (0 != inputFileName) {
	if (0 != outputFileName) {
	  cerr << "xdrgen: too many arguments" << endl;
	  usage();
	} else {
	  outputFileName = argv[argi];
	}
      } else {
	inputFileName = argv[argi];
      }
    }
  }
  if (0 == inputFileName) {
    cerr << "xdrgen: not enough arguments" << endl;
    usage();
  }

  XDRParser *parser = new XDRParser();

  if (verbose) cout << endl << "--- parsing" << endl;
  if (!parser->parseFile(inputFileName,cppArgs)) {
    fprintf(stderr, "couldn't parse file\n");
    exit(EXIT_FAILURE);
  }

  XDRSpecification *spec = parser->getSyntaxTree();
  if (debugPrintSpec) {
    if (verbose) cout << endl << "--- printing parse tree as XDR spec" << endl;
    XDROutputSpec outSpec(cout);
    outSpec.iterate(spec);
  }

  // name mapping is required before generating IPC formats.
  // it modifies the parse tree so that type name identifiers
  // resolve to the right types.
  if (verbose) cout << endl << "--- mapping identifier names" << endl;
  XDRMapNames *nameMapper = new XDRMapNames;
  nameMapper->iterate(spec);

  if (debugPrintFormats) {
    if (verbose) cout << endl << "--- outputting IPC formats" << endl;
    DebugFormatIPC debugFmt(cout);
    debugFmt.iterate(spec);
  }
  
  if (!noHeader) {
    if (verbose) cout << endl << "--- outputting C header" << endl;
    ostream *outStreamP;
    if (0 == outputFileName || outputFileName == "-") {
      outStreamP = &cout;
    } else {
      outStreamP = new ofstream(outputFileName);
      if (! (*outStreamP)) {
	cerr << "xdrgen: couldn't open `" << outputFileName
	     << "' for writing" << endl;
	exit(EXIT_FAILURE);
      }
    }
    if (lang == LANG_LISP) {
      XDROutputLisp outLisp(*outStreamP,outputFileName);
      outLisp.iterate(spec);
    } else {
      XDROutputHeader outHeader(*outStreamP,outputFileName,lang);
      outHeader.iterate(spec);
    }
  }

  return EXIT_SUCCESS;
}

/***************************************************************************
 * REVISION HISTORY:
 * $Log: xdrgen.cc,v $
 * Revision 1.7  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.6  2003/02/13 20:47:19  reids
 * Updated to compile under gcc 3.0.1
 *
 * Revision 1.5  2002/05/09 01:46:25  trey
 * added --lang=lisp option for xdr
 *
 * Revision 1.4  2002/04/29 22:23:32  trey
 * corrected some html errors; changed defaudefault --directives to be portable
 *
 * Revision 1.3  2002/02/14 21:46:30  reids
 * Incorporating various changes that Trey made in the FIRE version
 *
 * Revision 1.4  2001/11/26 22:49:51  trey
 * added comment
 *
 * Revision 1.3  2001/11/26 22:17:43  trey
 * fixed line directives bug
 *
 * Revision 1.2  2001/11/14 17:58:54  reids
 * Incorporating various changes that Trey made in the DIRA/FIRE versions.
 *
 * Revision 1.1  2001/03/16 17:56:07  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/08/27 19:20:30  trey
 * fixed unnecessary dependency on DIRA logging.h
 *
 * Revision 1.1  2001/08/27 18:44:03  trey
 * initial check-in
 *
 * Revision 1.5  2001/03/21 19:26:55  trey
 * added ability of xdrgen to pass on -D and -I flags to xdrcpp
 *
 * Revision 1.4  2001/02/13 16:33:53  trey
 * switched include of headers under src to use "dir/foo.h" instead of <dir/foo.h>; dependency checking is now easier. made some corresponding makefile changes
 *
 * Revision 1.3  2001/02/08 00:41:59  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.2  2001/02/06 02:01:55  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:52  trey
 * initial check-in
 *
 * Revision 1.3  2000/02/25 22:14:12  trey
 * added sanity check of parseFile() results
 *
 * Revision 1.2  1999/11/11 15:14:59  trey
 * changed to support logging
 *
 * Revision 1.1  1999/11/08 15:44:50  trey
 * renamed RCLmain to rcl and made it rely on RCLParser
 *
 * Revision 1.1  1999/11/03 19:31:37  trey
 * initial check-in
 *
 * Revision 1.1  1999/11/01 17:30:14  trey
 * initial check-in
 *
 *
 ***************************************************************************/

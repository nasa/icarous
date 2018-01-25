/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.2 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  Make available some functions and globals defined in
 *               XDR.y, XDR.l
 *
 * (c) Copyright 1999 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

#ifndef INCXDRInternal_h
#define INCXDRInternal_h

/***************************************************************************
 * INCLUDES
 ***************************************************************************/

#include "XDRTree.h"
#include "ParsePos.h"

/***************************************************************************
 * MACROS
 ***************************************************************************/

/***************************************************************************
 * GLOBALS AND FUNCTION PROTOTYPES
 ***************************************************************************/

extern int yydebug; // XDR.tab.cc - set to 1 for a bison parser trace
extern FILE *yyin;  // XDR.yy.cc - point this to the input file
extern XDRSpecification *yyTree; // XDR.y - the output global
extern const char *yyinName; // XDR.y: useful for error output

void lexinit	(void); // XDR.l - initializes the lexer state

void yyerror (const char *s);
int  yylex   (void);
int  yyparse (void);
ParsePos yypos   (void);


#endif // INCXDRInternal_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDRInternal.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:02  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/02/06 02:01:54  trey
 * fixed bugs discovered while generating commonTypes message formats
 *
 * Revision 1.1  2001/02/05 21:10:47  trey
 * initial check-in
 *
 *
 ***************************************************************************/

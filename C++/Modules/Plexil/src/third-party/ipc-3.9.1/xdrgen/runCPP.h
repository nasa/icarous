/* Put is line in first if it is a C++ only header.  -*- C++ -*- */

/* CVS or RCS keyword replacement variables: */
/* $Revision: 1.4 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $ */

/****************************************************************************
 *
 * PROJECT: 
 *
 * MODULE:  
 *
 * FILE:    
 *
 * DESCRIPTION: 
 *  
 *
 * EXPORTS:  
 *
 * NOTES:        
 *
 * REVISION HISTORY:
 * $Log: runCPP.h,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2003/02/13 20:47:19  reids
 * Updated to compile under gcc 3.0.1
 *
 * Revision 1.2  2001/11/14 17:58:54  reids
 * Incorporating various changes that Trey made in the DIRA/FIRE versions.
 *
 * Revision 1.1  2001/03/16 17:56:06  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.2  2001/03/21 19:25:40  trey
 * modified runCPP to take arbitrary arguments for passing on to cpp, and it now runs xdrcpp by default instead of cpp
 *
 * Revision 1.1  2001/02/05 21:10:51  trey
 * initial check-in
 *
 * Revision 1.2  1999/11/08 15:35:03  trey
 * major overhaul
 *
 * Revision 1.1  1999/11/03 19:31:37  trey
 * initial check-in
 *
 * Revision 1.2  1999/09/27 15:44:12  trey
 * enabled string concatenation and made marsenv variables visible in the .p file
 *
 * Revision 1.1  1999/03/25 23:44:14  schwehr
 * Runs the c pre-processor on a file and returns a file descriptor to
 * read that output.
 *
 * (c) Copyright 1999 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

/* Include Guard */
#ifndef INCrunCPP_h
#define INCrunCPP_h

#include <list>
#include <string>

using namespace std;

int runCPP(const char *fileName, list<string> &extraArgs);

#endif /* INCrunCPP_h -- end of the Include Guard*/

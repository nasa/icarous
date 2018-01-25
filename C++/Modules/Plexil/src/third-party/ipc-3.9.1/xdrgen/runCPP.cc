/****************************************************************************
 * $Revision: 1.4 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  Run the C pre-processor on a file.
 * CREDITS:      This is mostly stuff Kurt Schwehr wrote at Ames and CMU.
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
#include <stdio.h>
#include <unistd.h>	/* for STDIN_FILENO */
#include <string.h>	/* for strrchr() */
#include <ctype.h>	// isspace()

#include "runCPP.h"	// Make sure we use the c linking style.

//#include <iostream>

/***************************************************************************
 * EXTERNS
 ***************************************************************************/

/***************************************************************************
 * LOCAL MACROS and DEFINES
 ***************************************************************************/

#ifdef SOLARIS
#  define OS_CPP "/usr/ccs/lib/cpp"  // FIX: Is this right?
#else
#  define OS_CPP "/lib/cpp"
#endif

// Use this if getenv("XDRCPP") fails.
#ifdef XDRCPP
#  define DEFAULT_CPP XDRCPP
#else
#  define DEFAULT_CPP OS_CPP
#endif

/***************************************************************************
 * GLOBALS
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

static void
printArgs (int argc, char *const*argv)
{
  assert (argv);
  assert (0 < argc);

#if 0
  for (int i=0; i < argc; i++)
    fprintf (stderr,"%s ",argv[i]);
  fprintf (stderr,"\n");
#endif
}

/****************************************************************************
 * FUNCTION:	runCPP
 * DESCRIPTION: Forks a process to run the c preprocessor on source code.
 *		Will use the environment variable CC if it exists.
 * INPUTS:    	File name of a file to run through the preprocessor.
 *              A null-terminated list of environment variables to pass to
 *              cpp in -Dfoo=goo form (0 indicates an empty list).
 * OUTPUTS:	Output comes through a pipe.
 * RETURN:	-1 on error, otherwise file descriptor of the pipe to read from
 * EXCEPTIONS:	
 * DESIGN:	Uses the c compiler with the -E option to envoke cpp.
 * NOTES:	Most errors cause the program to exit.  Not very graceful.
 *		Might want to add the ability to wait on the child in a
 *		separate function.  Decided to skip that for now.
 ****************************************************************************/
int
runCPP(const char *fileName, list<string> &extraArgs)
{
    int   fd[2];
    pid_t pid;

    assert (fileName);

    // probably redundant, but let's be safe.
    if (0 != access (fileName,R_OK)) {
      char buf[80];
      sprintf (buf,"Failed to open config file (%s)",fileName);
      perror (buf);
      return -1;
    }

    if (pipe(fd) < 0) {
	perror ("runCPP: failed to open pipe.");
	exit (EXIT_FAILURE);
    }

    if ( (pid=fork()) <0 ) {
	perror ("runCPP: failed to fork.");
	exit (EXIT_FAILURE);
    }

    if (pid > 0) {		/* Parent */
	close (fd[1]);		/* Close the write end of the pipe */
	/* Nothing else to really do.  Just need to return fd[0] */
    } else {			/* Child */
	char *cpp;		/* Full path to cpp executable program. */

	close (fd[0]);		/* Close the read end of the pipe */

	if (fd[1] != STDOUT_FILENO) {
	    if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {
		perror ("runCPP: dup2 failed w/ stdout.");
		exit (EXIT_FAILURE);
	    }
	    close (fd[1]);	/* Don't need this after stdin set with dup2 */
	}

	if (NULL == (cpp = getenv ("XDRCPP")))
	    cpp = DEFAULT_CPP;

	const size_t argvMaxNum = 50;  // Can have up to 50 arguments.
	char *  *argv = new char *[255];//(char * const *arg
	memset ((void *)argv,0,argvMaxNum);

	size_t i = 0;
	argv[i++] = cpp;
	// argv[i++] = "-P";	// Don't produce line control info.
#ifdef IRIX
	argv[i++] = "-I"; // don't look in standard include directories
#endif
#ifdef LINUX
	argv[i++] = "-nostdinc"; // don't look in standard include directories
#endif
	argv[i++] = "-C"; // copy comments

	// add in extraArgs
	list<string>::iterator ei;
	for (ei=extraArgs.begin(); ei != extraArgs.end(); ei++) {
	  argv[i++] = strdup(ei->c_str());
	}

	argv[i++] = strdup(fileName);

	printArgs (i, argv);

	if (execvp(cpp, argv) < 0) {
	    char buff [350];
	    sprintf (buff, "runCPP: execl of %s failed.", cpp);
	    perror (buff);
	    exit(EXIT_FAILURE);
	}

	/* Should never reach here! */
	assert(0);

    } /* End of Child */

    return (fd[0]);	/* Return the file descriptor to read the */
			/* output of cpp from */

} /* End of runCPP() */

/***************************************************************************
 * REVISION HISTORY:
 * $Log: runCPP.cc,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2004/04/06 15:06:09  trey
 * updated for more recent bison and g++
 *
 * Revision 1.2  2001/11/14 17:58:54  reids
 * Incorporating various changes that Trey made in the DIRA/FIRE versions.
 *
 * Revision 1.1  2001/03/16 17:56:06  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.3  2001/03/23 19:09:33  trey
 * made sure we don't run cpp by default in xdrgen
 *
 * Revision 1.2  2001/03/21 19:25:40  trey
 * modified runCPP to take arbitrary arguments for passing on to cpp, and it now runs xdrcpp by default instead of cpp
 *
 * Revision 1.1  2001/02/05 21:10:51  trey
 * initial check-in
 *
 * Revision 1.3  2000/02/25 22:13:48  trey
 * fixed cpp flags bug under linux
 *
 * Revision 1.2  1999/11/08 15:35:03  trey
 * major overhaul
 *
 * Revision 1.1  1999/11/03 19:31:37  trey
 * initial check-in
 *
 * Revision 1.1  1999/11/01 17:30:14  trey
 * initial check-in
 *
 *
 ***************************************************************************/

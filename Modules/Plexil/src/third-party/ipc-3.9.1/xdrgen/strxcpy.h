/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.2 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  Replace strcadd() and streadd() libgen functionality
 *               for non-IRIX platforms.
 *
 * (c) Copyright 1999 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

#ifndef INCstrxcpy_h
#define INCstrxcpy_h

/***************************************************************************
 * GLOBALS AND FUNCTION PROTOTYPES
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * FUNCTION:	strcadd
 * DESCRIPTION:
 *  copies the input string, up to a null byte, to the output string,
 *  compressing the C-language escape sequences (for example, \n, \001) to
 *  the equivalent character.  A null byte is appended to the output.  The
 *  output argument must point to a space big enough to accommodate the
 *  result.  If it is as big as the space pointed to by input it is
 *  guaranteed to be big enough.
 * RETURN: the pointer to the null byte that terminates the output.
 ****************************************************************************/
char *strcadd(char *output, const char *input);

/****************************************************************************
 * FUNCTION:	streadd
 * DESCRIPTION:
 *  streadd copies the input string, up to a null byte, to the output string,
 *  expanding non-graphic characters to their equivalent C-language escape
 *  sequences (for example, \n, \001).  The output argument must point to a
 *  space big enough to accommodate the result; four times the space pointed
 *  to by input is guaranteed to be big enough (each character could become \
 *  and 3 digits).  Characters in the exceptions string are not expanded.
 *  The exceptions argument may be zero, meaning all non-graphic characters
 *  are expanded.
 * RETURN: the pointer to the null byte that terminates the output.
 ****************************************************************************/
char *streadd (char *output, const char *input, const char *exceptions);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif // INCstrxcpy_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: strxcpy.h,v $
 * Revision 1.2  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.1  2001/03/16 17:56:06  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.1  2001/02/05 21:10:52  trey
 * initial check-in
 *
 * Revision 1.1  1999/11/03 19:31:38  trey
 * initial check-in
 *
 * Revision 1.1  1999/11/01 17:30:15  trey
 * initial check-in
 *
 *
 ***************************************************************************/

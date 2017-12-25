/****************************************************************************
 * $Revision: 1.3 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
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

#ifndef FOO // IRIX

/***************************************************************************
 * INCLUDES
 ***************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "strxcpy.h"

/***************************************************************************
 * LOCAL MACROS and DEFINES
 ***************************************************************************/

typedef struct {
  char input;
  char output;
} escapePairType;

/***************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************/

static escapePairType pairs[] = {
  {'n', '\n'},
  {'t', '\t'},
  {'v', '\v'},
  {'b', '\b'},
  {'r', '\r'},
  {'f', '\f'},
  {'a', '\a'},
  {'\'', '\''},
  {'\\', '\\'},
  {'"', '"'},
  {'\0', '\0'}
};

/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/

static char
compressesTo(char input) {
  escapePairType *p;
  for (p = pairs; p->input; p++) {
    if (input == p->input) return p->output;
  }
  return '\0';
}

static char
expandsTo(char output) {
  escapePairType *p;
  for (p = pairs; p->input; p++) {
    if (output == p->output) return p->input;
  }
  return '\0';
}

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
char *
strcadd(char *output, const char *input) {
  const char *ip;
  char *op, c, cmp;
  unsigned int digits, cmpInt;
  char digBuf[4];

  for (ip=input, op=output; '\0' != (c = *ip); ip++, op++) {
    if ('\\' != c) {
      *op = *ip;
    } else {
      digits = 0;
      while (digits < 3 && isdigit(ip[digits+1])) digits++;
      if (digits > 0) {
	/* numeric escape sequence */
	strncpy(digBuf,ip+1,digits);
	digBuf[digits] = '\0';
	sscanf(digBuf,"%o",&cmpInt);
	*op = (char) cmpInt;
	ip += digits; /* we used digits+1 bytes instead of 1 */
      } else {
	/* escape sequence like \n or \r */
	cmp = compressesTo(c = ip[1]);
	if ('\0' == cmp) {
	  /* no match... just repeat the given character */
	  *op = c;
	} else {
	  /* plug in what we got back */
	  *op = cmp;
	}
	ip++; /* we used 2 bytes instead of 1 */
      }  
    }
  }
  *op = '\0';

  return op;
}

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
char *
streadd (char *output, const char *input, const char *exceptions) {
  const char *ip;
  char *op, c, exp;
  char digBuf[5];

  for (ip=input, op=output; '\0' != (c = *ip); ip++, op++) {
    exp = expandsTo(c);
    if ((isprint(c) && ('\0' == exp))
	|| (exceptions && (0 != strchr(exceptions,c)))) {
      *op = *ip;
    } else {
      if ('\0' == exp) {
	/* no match... use numeric escape sequence */
	sprintf(digBuf, "\\%03o", (unsigned int) c);
	strncpy(op, digBuf, 4);
	op += 3; /* copied in 4 bytes instead of 1 */
      } else {
	/* escape sequence like \n or \r */
	op[0] = '\\';
	op[1] = exp;
	op++; /* copied in 2 bytes instead of 1 */
      }
    }
  }
  *op = '\0';

  return op;
}

#endif // ifndef IRIX

/***************************************************************************
 * REVISION HISTORY:
 * $Log: strxcpy.c,v $
 * Revision 1.3  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.2  2002/06/25 21:20:08  reids
 * Fixed compiler warnings
 *
 * Revision 1.1  2001/03/16 17:56:06  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.1  2001/02/05 21:10:52  trey
 * initial check-in
 *
 * Revision 1.2  1999/11/08 15:35:04  trey
 * major overhaul
 *
 * Revision 1.1  1999/11/03 19:31:38  trey
 * initial check-in
 *
 ***************************************************************************/

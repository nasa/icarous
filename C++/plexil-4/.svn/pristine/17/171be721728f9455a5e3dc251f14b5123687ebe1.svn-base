/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: module.h
 *
 * ABSTRACT: Include file for IPC test programs
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:58 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: module.h,v $
 * Revision 2.3  2009/01/12 15:54:58  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/01/27 20:47:11  reids
 * Removed compiler warnings for RedHat Linux
 *
 * Revision 2.1.1.1  1999/11/23 19:07:37  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/06/17 18:43:33  rouquett
 * ipc test files
 *
 * Revision 1.3  1996/04/03 03:14:32  reids
 * Corrected the extern definition of printf.
 *
 * Revision 1.2  1996/03/12 03:06:04  reids
 * Test programs now illustrate use of "enum" format;
 * Handlers now free data.
 *
 * Revision 1.1  1996/03/06 20:19:22  reids
 * New test programs for passing data between C and LISP modules
 *
 ****************************************************************/

#include <stdlib.h>
#ifdef macintosh
extern int fileno(FILE *);
#endif
#if !defined(__sgi) && !defined(WIN32) && !defined(REDHAT_52)
#define UNUSED_PRAGMA
#endif

typedef enum { WaitVal, SendVal, ReceiveVal, ListenVal } STATUS_ENUM;

typedef struct { int i1;
		 STATUS_ENUM status;
		 double matrix[2][3];
		 double d1;
	       } T1_TYPE, *T1_PTR;

#define T1_NAME   "T1"
/* First form of "enum". 3 is the maximum value -- i.e., the value of WaitVal */
#define T1_FORMAT "{int, {enum : 3}, [double:2,3], double}"

typedef struct { char *str1;
		 int count;
		 T1_TYPE *t1; /* Variable length array of type T1_TYPE */
		 STATUS_ENUM status;
	       } T2_TYPE, *T2_PTR;

#define T2_NAME "T2"
/* Alternate form of "enum". */
#define T2_FORMAT \
  "{string, int, <T1:2>, {enum WaitVal, SendVal, ReceiveVal, ListenVal}}"

typedef int MSG1_TYPE, *MSG1_PTR;
#define MSG1        "message1"
#define MSG1_FORMAT "int"

typedef char *MSG2_TYPE, **MSG2_PTR;
#define MSG2        "message2"
#define MSG2_FORMAT "string"

typedef T1_TYPE QUERY1_TYPE, *QUERY1_PTR;
#define QUERY1        "query1"
#define QUERY1_FORMAT T1_NAME

typedef T2_TYPE RESPONSE1_TYPE, *RESPONSE1_PTR;
#define RESPONSE1        "response1"
#define RESPONSE1_FORMAT T2_NAME

#define MODULE1_NAME "module1"
#define MODULE2_NAME "module2"
#define MODULE3_NAME "module3"

/* These don't seem to be defined explicitly */
extern int printf(const char *format, ...);
extern int fflush(FILE *stream);

/******************************************************************************
 * PROJECT: IPC (Interprocess Communication) Package
 *
 * (c) Copyright 2011 Reid Simmons.  All rights reserved.
 *
 * FILE: ipcFFI.h
 *
 * ABSTRACT: Header file used by SWIG for foreign-function interface (FFI)
 *           between IPC (C) and other languages (Python, Java, Lisp)
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: ipcFFI.h,v $
 * Revision 2.2  2011/08/17 01:11:38  reids
 * Took out unused and LISP-specific functions
 *
 * Revision 2.1  2011/08/16 16:01:54  reids
 * Adding Python interface to IPC, plus some minor bug fixes
 *
 ****************************************************************/

#ifndef IPC_FFI_H
#define IPC_FFI_H

#ifdef DEFINE_FOR_SWIG
/*
 * The following are defined elsewhere, but SWIG needs them defined here
 * to generate things correctly
 */
typedef struct _FORMAT_TYPE FORMAT_TYPE, *FORMAT_PTR;
typedef const FORMAT_TYPE *CONST_FORMAT_PTR;
typedef int BOOLEAN;
typedef int int32;
// SWIG not handling unions reasonably - simplify
typedef //union {
  //int32 i;
  struct _FORMAT_TYPE //*f;
/*}*/ FORMAT_ARRAY_TYPE, *FORMAT_ARRAY_PTR;
typedef struct _X_IPC_REF *X_IPC_REF_PTR;
typedef struct _BUFFER_TYPE BUFFER_TYPE, *BUFFER_PTR;
typedef IPC_VARCONTENT_TYPE *IPC_VARCONTENT_PTR;
#endif

#ifdef macintosh
#pragma export on
#endif

int32 formatPrimitiveProc(FORMAT_PTR format);
int32 formatType(FORMAT_PTR format);
FORMAT_PTR formatChoosePtrFormat(CONST_FORMAT_PTR format,
				 FORMAT_PTR parentFormat);
FORMAT_ARRAY_PTR formatFormatArray(FORMAT_PTR format);
int32 formatFormatArrayMax(FORMAT_ARRAY_PTR formatArray);
FORMAT_PTR formatFormatArrayItem(FORMAT_ARRAY_PTR formatArray, int32 i);

void formatPutChar(BUFFER_PTR buffer, char c);
char formatGetChar(BUFFER_PTR buffer);
void formatPutByte(BUFFER_PTR buffer, int32 i);
int32 formatGetByte(BUFFER_PTR buffer);
void formatPutUByte(BUFFER_PTR buffer, int32 i);
int32 formatGetUByte(BUFFER_PTR buffer);
void formatPutShort(BUFFER_PTR buffer, int32 i);
int32 formatGetShort(BUFFER_PTR buffer);
void formatPutInt(BUFFER_PTR buffer, int32 i);
int32 formatGetInt(BUFFER_PTR buffer);
double formatGetFloat(BUFFER_PTR buffer);
void formatPutFloat(BUFFER_PTR buffer, double f);
double formatGetDouble(BUFFER_PTR buffer);

void formatPutDouble(BUFFER_PTR buffer, double d);
void formatPutUShort(BUFFER_PTR buffer, int32 i);
unsigned int formatGetUShort(BUFFER_PTR buffer);
void formatPutUInt(BUFFER_PTR buffer, int32 i);
unsigned int formatGetUInt(BUFFER_PTR buffer);

const char * formatGetString (BUFFER_PTR buffer);
void formatPutString (BUFFER_PTR buffer, const char *theString);

CONST_FORMAT_PTR findNamedFormat (CONST_FORMAT_PTR format);

int formatFormatArrayItemInt(FORMAT_ARRAY_PTR formatArray, int i);
FORMAT_PTR formatFormatArrayItemPtr(FORMAT_ARRAY_PTR formatArray, int i);
void *createByteArray (int length);
BUFFER_PTR createBuffer(void *byteArray);
void freeBuffer (BUFFER_PTR buffer);
int32 bufferLength (BUFFER_PTR buffer);
BOOLEAN validFormatter (FORMAT_PTR format);
unsigned int maxTriggers (TIMER_REF timerRef);

BOOLEAN checkMarshallStatus (FORMATTER_PTR formatter);

#ifdef NEED_DEBUGGING
/* The following functions are for debugging purposes, only */
void rewindBuffer (BUFFER_PTR buffer);
void printBuffer (BUFFER_PTR buffer);
void printByteArray (void *byteArray, int length);
#endif

#ifdef macintosh
#pragma export off
#endif

#endif // IPC_FFI_H

/*****************************************************************************
 * PROJECT: IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996, 2011 Reid Simmons.  All rights reserved.
 * 
 * FILE: ipcFFI.c
 *
 * ABSTRACT: Generic functions needed to interface between the C version 
 *           of IPC and other languages (Python, Java, Lisp).  In addition
 *           to these generic functions, each language will likely have its
 *           own set of interface functions (e.g., ipcPython.{c,py}
 *           Still in a state of transition from manually-coded interfaces
 *           to use of SWIG.
 *           Adapted from ipcLisp.c, generalized for multiple languages
 * 
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: ipcFFI.c,v $
 * Revision 2.2  2011/08/17 01:11:38  reids
 * Took out unused and LISP-specific functions
 *
 * Revision 2.1  2011/08/16 16:01:53  reids
 * Adding Python interface to IPC, plus some minor bug fixes
 *
 * $Revision: 2.2 $
 * $Date: 2011/08/17 01:11:38 $
 * $Author: reids $
 *****************************************************************************/

#include <stdio.h>

#include "globalM.h"
#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif

#include "ipcFFI.h"

int32 formatPrimitiveProc(FORMAT_PTR format)
{
  return(format->formatter.i);
}

int32 formatType(FORMAT_PTR format)
{
  return(format->type);
}

FORMAT_PTR formatChoosePtrFormat(CONST_FORMAT_PTR format,
				 FORMAT_PTR parentFormat)
{
  return(CHOOSE_PTR_FORMAT(format, parentFormat));
}

/* 14-Feb-91: fedor: format array sillyness from StructFMT loops */
FORMAT_ARRAY_PTR formatFormatArray(FORMAT_PTR format)
{
  return(format->formatter.a);
}

int32 formatFormatArrayMax(FORMAT_ARRAY_PTR formatArray)
{
  return(formatArray[0].i);
}

FORMAT_PTR formatFormatArrayItem(FORMAT_ARRAY_PTR formatArray, int32 i)
{
  return(formatArray[i].f);
}

void formatPutChar(BUFFER_PTR buffer, char c)
{
  *((char *)buffer->buffer+buffer->bstart) = c;
  buffer->bstart += sizeof(char);
}

char formatGetChar(BUFFER_PTR buffer)
{
  char c;
  
  c = *((char *)(buffer->buffer+buffer->bstart));
  buffer->bstart += sizeof(char);
  
  return c;
}

void formatPutByte(BUFFER_PTR buffer, int32 i)
{
  signed char byte = (signed char)i;

  buffer->bstart += x_ipc_BYTE_Trans_Encode((GENERIC_DATA_PTR)&byte,
				      0, buffer->buffer, buffer->bstart);
}

int32 formatGetByte(BUFFER_PTR buffer)
{
  signed char byte;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;

  buffer->bstart += x_ipc_BYTE_Trans_Decode((GENERIC_DATA_PTR)&byte,
					    0, buffer->buffer, buffer->bstart,
					    byteOrder, alignment);
  return (int32)byte;
}

void formatPutUByte(BUFFER_PTR buffer, int32 i)
{
  unsigned char byte = (unsigned char)i;

  buffer->bstart += x_ipc_BYTE_Trans_Encode((GENERIC_DATA_PTR)&byte,
					    0, buffer->buffer, buffer->bstart);
}

int32 formatGetUByte(BUFFER_PTR buffer)
{
  unsigned char byte;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;
  
  buffer->bstart += x_ipc_BYTE_Trans_Decode((GENERIC_DATA_PTR)&byte,
					    0, buffer->buffer, buffer->bstart,
					    byteOrder, alignment);
  return (int32)byte;
}

void formatPutShort(BUFFER_PTR buffer, int32 i)
{
  short s = (short)i;

  buffer->bstart += x_ipc_SHORT_Trans_Encode((GENERIC_DATA_PTR)&s,
				       0, buffer->buffer, buffer->bstart);
}

int32 formatGetShort(BUFFER_PTR buffer)
{
  short s;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;

  buffer->bstart += x_ipc_SHORT_Trans_Decode((GENERIC_DATA_PTR)&s,
					     0, buffer->buffer, buffer->bstart,
					     byteOrder, alignment);
  return (int32)s;
}

void formatPutInt(BUFFER_PTR buffer, int32 i)
{
  buffer->bstart += x_ipc_INT_Trans_Encode((GENERIC_DATA_PTR)&i,
				     0, buffer->buffer, buffer->bstart
				     );
}

int32 formatGetInt(BUFFER_PTR buffer)
{
  int32 i;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;
  
  buffer->bstart += x_ipc_INT_Trans_Decode((GENERIC_DATA_PTR)&i, 
					   0, buffer->buffer, buffer->bstart,
					   byteOrder, alignment);
  return i;
}

double formatGetFloat(BUFFER_PTR buffer)
{
  float f;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;
  
  buffer->bstart += x_ipc_FLOAT_Trans_Decode((GENERIC_DATA_PTR)&f, 
					     0, buffer->buffer, buffer->bstart,
					     byteOrder, alignment);
  return (double)f;
}

void formatPutFloat(BUFFER_PTR buffer, double f)
{
  /* 14-Mar-91: fedor: this sillyness forces a pointer to a float
     without changing that float into a double. This is needed
     because FLOAT_Trans will transfer only 4 bytes but the conversion
     to a double will only transfer the first 4 bytes of the correct value. */
  
  float f2;
  
  f2 = (float)f;
  buffer->bstart += x_ipc_FLOAT_Trans_Encode((GENERIC_DATA_PTR)&f2,
				       0, buffer->buffer, buffer->bstart
				       );
}

void formatPutDouble(BUFFER_PTR buffer, double d)
{
  buffer->bstart += x_ipc_DOUBLE_Trans_Encode((GENERIC_DATA_PTR)&d,
					0, buffer->buffer, buffer->bstart
					);
}

double formatGetDouble(BUFFER_PTR buffer)
{
  double d;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;
  
  buffer->bstart += x_ipc_DOUBLE_Trans_Decode((GENERIC_DATA_PTR)&d, 0,
					      buffer->buffer, buffer->bstart,
					      byteOrder, alignment);
  return d;
}

void formatPutUShort(BUFFER_PTR buffer, int32 i)
{
  formatPutShort(buffer, i);
}

unsigned int formatGetUShort(BUFFER_PTR buffer)
{
  unsigned short us;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;

  buffer->bstart += x_ipc_SHORT_Trans_Decode((GENERIC_DATA_PTR)&us,
					     0, buffer->buffer, buffer->bstart,
					     byteOrder, alignment);
  return (int32)us;
}

void formatPutUInt(BUFFER_PTR buffer, int32 i)
{
  formatPutInt(buffer, i);
}

unsigned int formatGetUInt(BUFFER_PTR buffer)
{
  unsigned int ui;
  int byteOrder;
  ALIGNMENT_TYPE alignment;

  LOCK_M_MUTEX;
  byteOrder = GET_M_GLOBAL(byteOrder);
  alignment = GET_M_GLOBAL(alignment);
  UNLOCK_M_MUTEX;
  
  buffer->bstart += x_ipc_INT_Trans_Decode((GENERIC_DATA_PTR)&ui,
					   0, buffer->buffer, buffer->bstart,
					   byteOrder, alignment);
  return (unsigned int)ui;
}

CONST_FORMAT_PTR findNamedFormat (CONST_FORMAT_PTR format)
{
  return (format->type == NamedFMT ? x_ipc_fmtFind(format->formatter.name) : NULL);
}

#include "ipcPriv.h"
#ifdef macintosh
__declspec(export)
#endif

BOOLEAN checkMarshallStatus (FORMATTER_PTR formatter)
{
  if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else {
    return IPC_OK;
  }
}

int formatFormatArrayItemInt(FORMAT_ARRAY_PTR formatArray, int i)
{
  return (int)(long)formatFormatArrayItem(formatArray, i);
}
FORMAT_PTR formatFormatArrayItemPtr(FORMAT_ARRAY_PTR formatArray, int i)
{
  return formatFormatArrayItem(formatArray, i);
}

void *createByteArray (int length)
{
  return x_ipcMalloc(length);
}

BUFFER_PTR createBuffer(void *byteArray)
{
  BUFFER_PTR buffer = NEW(BUFFER_TYPE);

  buffer->bstart = 0;
  buffer->buffer = (char *)(size_t)byteArray;

  return buffer;
}

void freeBuffer (BUFFER_PTR buffer)
{
  free((void *)buffer);
}

int32 bufferLength (BUFFER_PTR buffer)
{
  return buffer->bstart;
}

const char * formatGetString (BUFFER_PTR buffer)
{
#define CBUF_LEN 100
  static int bufLen = CBUF_LEN;
  static char *charBuffer = NULL;
  if (charBuffer == NULL) charBuffer = (char *)malloc(bufLen);

  int length = formatGetInt(buffer);

  if (length == 0) {
    charBuffer[0] = '\0';
    formatGetChar(buffer); // The 'Z'
  } else {
    if (length >= bufLen) {
      /* Need to re-allocate this array, with room for a null termination */
      free(charBuffer);
      bufLen = length+1;
      charBuffer = (char *)malloc(bufLen);
    }
    BCOPY(buffer->buffer+buffer->bstart, charBuffer, length);
    buffer->bstart += length;
    charBuffer[length] = '\0';
  }
  return charBuffer;
}

void formatPutString (BUFFER_PTR buffer, const char * theString)
{
  int length = strlen(theString);
  formatPutInt(buffer, length);
  if (length == 0) {
    formatPutChar(buffer, 'Z');
  } else {
    BCOPY(theString, buffer->buffer+buffer->bstart, length);
    buffer->bstart += length;
  }
}

BOOLEAN validFormatter (FORMAT_PTR format)
{
  return format != NULL;
}

#ifdef NEED_DEBUGGING

void rewindBuffer (BUFFER_PTR buffer)
{
  buffer->bstart = 0;
}

void printBuffer (BUFFER_PTR buffer)
{
  int i;
  fprintf(stderr, "BUFFER: (%d) ", buffer->bstart);
  for (i = 0; i<buffer->bstart; i++) {
    fprintf(stderr, "%2X ", buffer->buffer[i]);
  }
  fprintf(stderr, "\n");
}

void printByteArray (void *byteArray, int length)
{
  int i;
  fprintf(stderr, "BYTE ARRAY: (%d) ", (int)length);
  for (i = 0; i<length; i++) {
    fprintf(stderr, "%2X ", ((unsigned char *)byteArray)[i]);
  }
  fprintf(stderr, "\n");
}
#endif

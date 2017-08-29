#if defined(LISP) || defined(JAVA)
/*****************************************************************************
 * PROJECT: New Millennium
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 * 
 * FILE: ipcLisp.c
 *
 * ABSTRACT: Functions needed specifically for LISP version of IPC.
 *           The idea is to put them in their own separate file, so that 
 *           they won't be linked into C programs.  This is especially
 *           necessary for Allegro, which depends on a function, lisp_value,
 *           that is found only in the Allegro CL environment.
 * 
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: ipcLisp.c,v $
 * Revision 2.5  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2002/01/03 20:52:12  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.3  2001/01/10 15:32:50  reids
 * Added the function IPC_subscribeData that automatically unmarshalls
 *   the data before invoking the handler.
 *
 * Revision 2.2  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.7  1997/03/07 17:49:43  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.1.2.6  1997/01/27 20:39:43  reids
 * For Lisp, force all format enum values to uppercase; Yields more efficient
 *   C <=> Lisp conversion of enumerated types.
 *
 * Revision 1.1.2.5  1997/01/27 20:09:34  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.3  1997/01/21 17:21:13  reids
 * Fixed stringEqualNoCase for VxWorks version.
 *
 * Revision 1.1.2.2  1997/01/16 22:18:18  reids
 * Added error checking code for use by Lisp.
 *
 * Revision 1.1.2.1  1997/01/11 01:21:03  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.2  1996/12/27 19:26:03  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.1.2.1  1996/12/24 14:41:40  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *

 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include <stdio.h>

#include "globalM.h"
#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif

#if 0
#ifdef PC_ALLEGRO
#include "C:\acl432\lisp.h"
extern int lisp_value(int a);
#endif /* PC_ALLEGRO */
#endif

#if defined(ALLEGRO)
#include "lisp.h"

extern int lisp_value(int a);
#endif /* ALLEGRO */

/******************************************************************************
 * Forward Declarations
 *****************************************************************************/

#ifdef macintosh
#pragma export on
#endif

X_IPC_REF_PTR returnRootGlobal(void);

void x_ipcSetSizeEncodeDecode(int (* x_ipc_bufferSize)(int32 *, CONST_FORMAT_PTR),
			    long (* encode)(CONST_FORMAT_PTR, BUFFER_PTR),
			    long (* decode)(CONST_FORMAT_PTR, BUFFER_PTR), 
			    int (* lispExit)(void)
#ifdef NMP_IPC
			    ,int (* lispQueryResponse)(char *, CONST_FORMAT_PTR)
#endif
			    );
X_IPC_REF_PTR x_ipcQuerySendRefLISPC (void);

int32 x_ipcRefDataLisp(X_IPC_REF_PTR ref);

char *lisp_Data_Flag(void);

void blockCopyToArray(BUFFER_PTR buffer, char *array, int32 amount);
void blockCopyFromArray(BUFFER_PTR buffer, char *array, int32 amount);
void setIntValue(int32 *item, int32 value);
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

#ifdef PC_ALLEGRO
void formatGetFloat(BUFFER_PTR buffer, float *fVal);
void formatPutFloat(BUFFER_PTR buffer, float f);
void formatGetDouble(BUFFER_PTR buffer, double *dVal);
#else
double formatGetFloat(BUFFER_PTR buffer);
void formatPutFloat(BUFFER_PTR buffer, double f);
double formatGetDouble(BUFFER_PTR buffer);
#endif /* PC_ALLEGRO */

void formatPutDouble(BUFFER_PTR buffer, double d);
void formatPutUShort(BUFFER_PTR buffer, int32 i);
unsigned int formatGetUShort(BUFFER_PTR buffer);
void formatPutUInt(BUFFER_PTR buffer, int32 i);
unsigned int formatGetUInt(BUFFER_PTR buffer);

CONST_FORMAT_PTR findNamedFormat (CONST_FORMAT_PTR format);
int32 enumStringIndex (CONST_FORMAT_PTR format, char *enumString);
char *enumValString (CONST_FORMAT_PTR format, int32 enumVal);

int msgByteOrder (void);
int hostByteOrder (void);

int32 DecodeInteger(GENERIC_DATA_PTR DataArray, int32 Start);
double DecodeFloat(GENERIC_DATA_PTR DataArray, int32 Start);
void EncodeInteger(int32 Integer, GENERIC_DATA_PTR DataArray, int32 Start);
double DecodeFloat(GENERIC_DATA_PTR DataArray, int32 Start);
void EncodeFloat(double Float, GENERIC_DATA_PTR DataArray, int32 Start);
float DecodeDouble(GENERIC_DATA_PTR DataArray, int32 Start);
void EncodeDouble(double Double, GENERIC_DATA_PTR DataArray, int32 Start);
void BCOPY_Bytes(char Array1[], int32 Start1, char Array2[], int32 Start2,
		 int32 Num_Bytes);

#ifdef macintosh
#pragma export off
#endif

X_IPC_REF_PTR returnRootGlobal(void)
{
  return(x_ipcRootNode());
}

#ifdef LISP
void x_ipcSetSizeEncodeDecode(int (* x_ipc_bufferSize)(int32 *, CONST_FORMAT_PTR),
			    long (* encode)(CONST_FORMAT_PTR, BUFFER_PTR),
			    long (* decode)(CONST_FORMAT_PTR, BUFFER_PTR), 
			    int (* lispExit)(void)
#ifdef NMP_IPC
			    ,int (* lispQueryResponse)(char *, CONST_FORMAT_PTR)
#endif
			    )
{
  LOCK_M_MUTEX;
  (GET_M_GLOBAL(lispBufferSizeGlobal)) = x_ipc_bufferSize;
  (GET_M_GLOBAL(lispEncodeMsgGlobal)) = encode;
  (GET_M_GLOBAL(lispDecodeMsgGlobal)) = decode;
  (GET_M_GLOBAL(lispExitGlobal)) = lispExit;
#ifdef NMP_IPC
  (GET_M_GLOBAL(lispQueryResponseGlobal)) = lispQueryResponse;
#endif
  UNLOCK_M_MUTEX;
}

/*****************************************************************
 * Return to LISP the reference created for a x_ipcQuerySend or x_ipcMultiQuery.
 * There may be a cleaner way to do this, but this seems the easiest
 * and quickest to implement (reids 11/13/92)
 ****************************************************************/
X_IPC_REF_PTR x_ipcQuerySendRefLISPC (void)
{
  X_IPC_REF_PTR result;

  LOCK_M_MUTEX;
  result = GET_M_GLOBAL(lispRefSaveGlobal);
  UNLOCK_M_MUTEX;

  return result;
}

/* 25-Jun-91: fedor: this is really a cheap shot at making ref data work
   for lisp - lets hope it works. */
int32 x_ipcRefDataLisp(X_IPC_REF_PTR ref)
{
  int32 refId, sd;
  MSG_PTR msg;
  X_IPC_REF_PTR waitRef;
  X_IPC_RETURN_VALUE_TYPE returnValue;
  char *lispDataFlag;
  
  msg = x_ipc_msgFind(X_IPC_REF_DATA_QUERY);
  if (msg == NULL) return 0;
  
  if (!ref->msg) {
    if (!ref->name) {
      /* 17-Jun-91: fedor: start enforcing correct refs */
      X_IPC_MOD_ERROR1("ERROR: x_ipcReferenceData: Badly Formed Reference: %d\n",
		  ref->refId);
      return 0;
    }
    ref->msg = x_ipc_msgFind(ref->name);
    if (ref->msg == NULL) return 0;
  }
  
  /* 17-Jun-91: fedor: check if any message form */
  if (!ref->msg->msgData->msgFormat)
    return 0;
  
  refId = x_ipc_nextSendMessageRef();
  returnValue = x_ipc_sendMessage((X_IPC_REF_PTR)NULL, msg, 
				  (char *)&ref->refId, (char *)NULL, refId);
  
  if (returnValue != Success) {
    X_IPC_MOD_ERROR("ERROR: x_ipcReferenceData: x_ipc_sendMessage Failed.\n");
    return 0;
  }
  waitRef = x_ipcRefCreate(ref->msg, ref->name, refId);
  
  LOCK_CM_MUTEX;
  sd = GET_C_GLOBAL(serverRead);
  lispDataFlag = LISP_DATA_FLAG();
  UNLOCK_CM_MUTEX;
  returnValue = x_ipc_waitForReplyFrom(waitRef, lispDataFlag, TRUE,
				       WAITFOREVER, sd);
  
  x_ipcRefFree(waitRef);
  
  if (returnValue == NullReply) {
    /* 17-Jun-91: fedor: if NullReply then nothing else was malloced. */
    return 0;
  }
  else
    return 1;
}

char *lisp_Data_Flag(void)
{
  char *lispDataFlag;

  LOCK_M_MUTEX;
  lispDataFlag = LISP_DATA_FLAG();
  UNLOCK_M_MUTEX;

  return lispDataFlag;
}

#ifdef LISPWORKS_FFI_HACK
/* To deal with the fact that Lispworks on VxWorks cannot call Lisp functions
   from C */
/* Apparently have the same problem with Mac Common Lisp (at least using CodeWarrior C) */
#ifdef macintosh
#pragma export on
#endif
EXECHND_STATE_ENUM execHndState_state(void);
void setExecHndState_state(EXECHND_STATE_ENUM state);
X_IPC_REF_PTR execHndState_x_ipcRef(void);
const char *execHndState_hndName(void);
void *execHndState_data(void);
void *execHndState_clientData(void);
CONNECTION_PTR execHndState_connection(void);
MSG_PTR execHndState_msg(void);
int execHndState_tmpParentRef(void);
int execHndState_tmpResponse(void);
#ifdef macintosh
#pragma export off
#endif

EXECHND_STATE_ENUM execHndState_state(void)
{
  EXECHND_STATE_ENUM result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).state;
  UNLOCK_CM_MUTEX;

  return result;
}

void setExecHndState_state(EXECHND_STATE_ENUM state)
{
  LOCK_CM_MUTEX;
  GET_C_GLOBAL(execHndState).state = state;
  UNLOCK_CM_MUTEX;
}

X_IPC_REF_PTR execHndState_x_ipcRef(void)
{
  X_IPC_REF_PTR result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).x_ipcRef;
  UNLOCK_CM_MUTEX;

  return result;
}

const char *execHndState_hndName(void)
{
  const char *result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).hndName;
  UNLOCK_CM_MUTEX;

  return result;
}

void *execHndState_data(void)
{
  void *result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).data;
  UNLOCK_CM_MUTEX;

  return result;
}

void *execHndState_clientData(void)
{
  void *result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).clientData;
  UNLOCK_CM_MUTEX;

  return result;
}

CONNECTION_PTR execHndState_connection(void)
{
  void *result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).clientData;
  UNLOCK_CM_MUTEX;

  return result;
}

MSG_PTR execHndState_msg(void)
{
  MSG_PTR result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).msg;
  UNLOCK_CM_MUTEX;

  return result;
}

int execHndState_tmpParentRef(void)
{
  int result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).tmpParentRef;
  UNLOCK_CM_MUTEX;

  return result;
}

int execHndState_tmpResponse(void)
{
  int result;

  LOCK_CM_MUTEX;
  result = GET_C_GLOBAL(execHndState).tmpResponse;
  UNLOCK_CM_MUTEX;

  return result;
}
#endif /* LISPWORKS_FFI_HACK */
#endif

void blockCopyToArray(BUFFER_PTR buffer, char *array, int32 amount)
{
  BCOPY(buffer->buffer+buffer->bstart, array, amount);
  buffer->bstart += amount;
}

void blockCopyFromArray(BUFFER_PTR buffer, char *array, int32 amount)
{
  BCOPY(array, buffer->buffer+buffer->bstart, amount);
  buffer->bstart += amount;
}

void setIntValue(int32 *item, int32 value)
{
  *item = value;
}

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

/*  "single-float" LISP arguments do not seem to work 
    with Unix gcc or acc compiler. "double-float" LISP
    arguments do not seem to work with PC Allegro.  */

#ifdef PC_ALLEGRO
void formatGetFloat(BUFFER_PTR buffer, float *fVal)
#else
double formatGetFloat(BUFFER_PTR buffer)
#endif /* PC_ALLEGRO */
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
#ifdef PC_ALLEGRO
  *fVal = f;
#else
  return (double)f;
#endif /* PC_ALLEGRO */
}

/*  "single-float" LISP arguments do not seem to work with 
    gcc or acc compiler. "double-float" LISP arguments do
    not seem to work with PC Allegro. */

#ifdef PC_ALLEGRO
void formatPutFloat(BUFFER_PTR buffer, float f)
#else
void formatPutFloat(BUFFER_PTR buffer, double f)
#endif /* PC_ALLEGRO */
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

#ifdef PC_ALLEGRO
void formatGetDouble(BUFFER_PTR buffer, double *dVal)
#else
double formatGetDouble(BUFFER_PTR buffer)
#endif /* PC_ALLEGRO */
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
#ifdef PC_ALLEGRO
  *dVal = d;
#else
  return d;
#endif /* PC_ALLEGRO */
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

/* Find, and return, the index of the string within the enum's names.
   Return -1 if not found.  
   Note: Change on 1/23/97 forces all Lisp enum names to uppercase, 
         so can use regular string comparison. */
int32 enumStringIndex (CONST_FORMAT_PTR format, char *enumString)
{
  int i;

  for (i=2; i<format->formatter.a[0].i; i++) {
    if (STREQ(format->formatter.a[i].f->formatter.name, enumString)) {
      return (i-2);
    }
  }
  return -1;
}

char *enumValString (CONST_FORMAT_PTR format, int32 enumVal)
{
  if (format->formatter.a[0].i > 2 &&
      0 <= enumVal && enumVal <= ENUM_MAX_VAL(format)) {
    return (format->formatter.a[enumVal+2].f->formatter.name);
  } else {
    return "";
  }
}


int msgByteOrder (void)
{
  int result;

  LOCK_M_MUTEX;
  result = GET_M_GLOBAL(byteOrder);
  UNLOCK_M_MUTEX;

  return result;
}

int hostByteOrder (void)
{
  return BYTE_ORDER;
}

int32 DecodeInteger(GENERIC_DATA_PTR DataArray, int32 Start)
{ 
  int32 Integer;
  
  DataArray += Start;
  Integer = *((int32 *) DataArray);
  return Integer;
}

void EncodeInteger(int32 Integer, GENERIC_DATA_PTR DataArray, int32 Start)
{ 
  DataArray += Start;
  *((int32 *) DataArray) = Integer;
}

/* LISP floating point numbers are equivalent to C "double" format */
double DecodeFloat(GENERIC_DATA_PTR DataArray, int32 Start)
{ 
  float Float;
  
  DataArray += Start;
  BCOPY(DataArray, (char *)&Float, sizeof(float));
  return (double)Float;
}

/* LISP floating point numbers are equivalent to C "double" format */
void EncodeFloat(double Float, GENERIC_DATA_PTR DataArray, int32 Start)
{ 
  float RealFloat;
  
  DataArray += Start;
  RealFloat = (float)Float;
  BCOPY((char *)&RealFloat, DataArray, sizeof(float));
}

float DecodeDouble(GENERIC_DATA_PTR DataArray, int32 Start)
{ 
  double Double;
  
  DataArray += Start;
  BCOPY(DataArray, (char *)&Double, sizeof(double));
  return Double;
}

void EncodeDouble(double Double, GENERIC_DATA_PTR DataArray, int32 Start)
{ 
  DataArray += Start;
  BCOPY((char *)&Double, DataArray, sizeof(double));
}

void BCOPY_Bytes(char Array1[], int32 Start1, char Array2[], int32 Start2,
		 int32 Num_Bytes)
{ 
  BCOPY(Array1+Start1, Array2+Start2, Num_Bytes);
}

#ifdef NMP_IPC
#include "ipcPriv.h"
#ifdef macintosh
__declspec(export)
#endif
BOOLEAN checkMarshallStatus (FORMATTER_PTR formatter); /* Prototype defn */

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
#endif

#if defined(ALLEGRO) && !defined(PC_ALLEGRO)

/****************************************************************
 *
 *  Allegro-specific functions
 *
 ****************************************************************/

/* Forward Declarations */

int32 x_ipcRefNameLength(X_IPC_REF_PTR ref);
char *x_ipcRefNameLisp(X_IPC_REF_PTR ref, int32 length);

int32 enumValNameLength (CONST_FORMAT_PTR format, int32 enumVal);
char *enumValNameString (CONST_FORMAT_PTR format, int32 enumVal);

int32 x_ipcRefNameLength(X_IPC_REF_PTR ref)
{
  if (ref && ref->name)
    return strlen(ref->name);
  else
    return 0;
}

char *x_ipcRefNameLisp(X_IPC_REF_PTR ref, int32 length)
{
  BCOPY(ref->name, (char *)Vecdata(SymbolValue(lisp_value(0))), length);
  
  return (char *)SymbolValue(lisp_value(0));
}

/* Find string with the given enumerated value and return the string length;
   Return 0 if value outside range or enum has no strings. */
int32 enumValNameLength (CONST_FORMAT_PTR format, int32 enumVal)
{
  if (format->formatter.a[0].i > 2 &&
      0 <= enumVal && enumVal <= ENUM_MAX_VAL(format)) {
    return strlen(format->formatter.a[enumVal+2].f->formatter.name);
  } else {
    return 0;
  }
}

/* Find string with the given enumerated value, and copy the string into
   the (already allocated) LISP vector */
char *enumValNameString (CONST_FORMAT_PTR format, int32 enumVal)
{
  char *name;

  if (format->formatter.a[0].i > 2 &&
      0 <= enumVal && enumVal <= ENUM_MAX_VAL(format)) {
    name = format->formatter.a[enumVal+2].f->formatter.name;
    BCOPY(name, (char *)Vecdata(SymbolValue(lisp_value(0))), strlen(name));
  }  
  return (char *)SymbolValue(lisp_value(0));
}

#endif /* ALLEGRO */

#endif /* LISP */

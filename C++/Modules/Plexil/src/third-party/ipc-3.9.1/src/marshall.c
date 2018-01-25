/******************************************************************************
 * PROJECT: New Millennium, DS1
 *          IPC (Interprocess Communication) Package
 *
 * (c) Copyright 1996 Reid Simmons.  All rights reserved.
 *
 * FILE: marshall.c
 *
 * ABSTRACT: Implementation of the marshalling/unmarshalling functions of
 *           the IPC, using (modified) X_IPC library
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: marshall.c,v $
 * Revision 2.7  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2002/01/03 20:52:13  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/06/01 18:51:19  reids
 * Don't try to free NULL data
 *
 * Revision 2.4  2001/02/28 03:13:24  trey
 * added explicit cast to avoid warning
 *
 * Revision 2.3  2001/02/09 16:24:21  reids
 * Added IPC_getConnections to return list of ports that IPC is listening to.
 * Added IPC_freeDataElements to free the substructure (pointers) of a struct.
 *
 * Revision 2.2  2000/07/03 17:03:26  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.5.2.12  1997/03/07 17:49:49  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.5.2.11  1997/01/27 20:40:32  reids
 * Implement a function to check whether a given format string matches the
 *   one registered for a given message.
 *
 * Revision 1.5.2.10  1997/01/27 20:09:42  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.5.2.8  1997/01/16 22:19:36  reids
 * Took out restriction that Lisp marshalling was non-reentrant.
 *
 * Revision 1.5.2.7  1997/01/11 01:21:08  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.5.2.6.4.1  1996/12/24 14:41:44  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.5.2.6  1996/12/18 15:12:57  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.5.2.5  1996/10/29 14:51:18  reids
 * Added IPC_unmarshallData.
 * IPC_freeByteArray function available to C, not just LISP.
 * Use x_ipcMalloc instead of malloc.
 *
 * Revision 1.5.2.4  1996/10/24 17:26:37  reids
 * Replace fprintf with x_ipcModWarning.
 *
 * Revision 1.5.2.3  1996/10/24 15:19:21  reids
 * Make everything use x_ipcMalloc/x_ipcFree.
 *
 * Revision 1.5.2.2  1996/10/18 18:10:19  reids
 * Better error checking and handling.
 *
 * Revision 1.5.2.1  1996/10/02 20:56:42  reids
 * Fixed the procedure for dealing with named formatters.
 *
 * Revision 1.6  1996/09/06 22:30:34  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.5  1996/05/24 20:02:09  rouquett
 * swapped include order between ipc.h globalM.h for solaris compilation
 *
 * Revision 1.4  1996/05/09 01:01:38  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/24 19:13:51  reids
 * Changes to support vxworks version.
 *
 * Revision 1.2  1996/03/06 20:20:46  reids
 * Version 2.3 adds two new functions: IPC_defineFormat and IPC_isConnected
 *
 * Revision 1.1  1996/03/03 04:36:21  reids
 * First release of IPC files.  Corresponds to IPC Specifiction 2.2, except
 * that IPC_readData is not yet implemented.  Also contains "cover" functions
 * for the xipc interface.
 *
 ****************************************************************/

#include "globalM.h"
#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#include "formatte.h"
#include "parseFmt.h"
#include "printDat.h"
#else
#include "primFmttrs.h"
#include "formatters.h"
#include "parseFmttrs.h"
#include "printData.h"
#endif
#include "ipc.h"
#include "ipcPriv.h"

FORMATTER_PTR IPC_parseFormat (const char *formatString)
{
  FORMATTER_PTR format;

  if (!formatString || strlen(formatString) == 0) {
    return NULL;
  } else if (!X_IPC_INITIALIZED()) {
    ipcSetError(IPC_Not_Initialized);
    return NULL;
  } else {
    format = ParseFormatString(formatString);
    if (format && format->type == BadFormatFMT) {
      ipcSetError(IPC_Illegal_Formatter);
      return NULL;
    } else {
      return format;
    }
  }
}

FORMATTER_PTR IPC_msgFormatter (const char *msgName)
{
  MSG_PTR msg;
  FORMATTER_PTR format = NULL;

  if (!msgName || strlen(msgName) == 0) {
    ipcSetError(IPC_Null_Argument);
  } else if (!X_IPC_CONNECTED()) {
    ipcSetError(IPC_Not_Connected);
  } else {
    msg = x_ipc_msgFind(msgName);
    if (!msg) {
      ipcSetError(IPC_Message_Not_Defined);
    } else {
      format = msg->msgData->resFormat;
      if (format && format->type == BadFormatFMT) {
	ipcSetError(IPC_Illegal_Formatter);
	format = NULL;
      }
    }
  }
  return format;
}

/* Equivalent to, but more efficient than, 
   IPC_msgFormatter(IPC_msgInstanceName(msgInstance)); */
FORMATTER_PTR IPC_msgInstanceFormatter (MSG_INSTANCE msgInstance)
{
  FORMATTER_PTR format;

  if (!msgInstance) {
    ipcSetError(IPC_Null_Argument);
    return NULL;
  } else {
    format = msgInstance->msg->msgData->resFormat;
    if (format && format->type == BadFormatFMT) {
      ipcSetError(IPC_Illegal_Formatter);
      return NULL;
    } else {
      return format;
    }
  }
}

IPC_RETURN_TYPE IPC_defineFormat (const char *formatName,
				  const char *formatString)
{
  if (!formatName || strlen(formatName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (X_IPC_INITIALIZED()) {
    if (X_IPC_CONNECTED()) {
      x_ipcRegisterNamedFormatter(formatName, formatString);
    }
    x_ipc_addFormatStringToTable(strdup(formatName), strdup(formatString));
    return IPC_OK;
  } else {
    RETURN_ERROR(IPC_Not_Connected);
  }
}

static IPC_RETURN_TYPE _IPC_marshall (FORMATTER_PTR formatter,
				      void *dataptr, 
				      IPC_VARCONTENT_PTR varcontent,
				      BOOLEAN mallocData)
{
  unsigned int length;

  if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else if (varcontent == NULL) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    varcontent->length = length = (unsigned)x_ipc_bufferSize(formatter, 
							     dataptr);
    if (length > 0) {
      if (!mallocData && x_ipc_sameFixedSizeDataBuffer(formatter)) {
	/* The data structure is equivalent to the byte-array */
	varcontent->content = dataptr;
      } else {
	varcontent->content = x_ipcMalloc(length);
	x_ipc_encodeData(formatter, dataptr, (char *)varcontent->content, 
			 0, length);
      }
    } else {
      varcontent->content = NULL;
    }
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_marshall (FORMATTER_PTR formatter,
			      void *dataptr, 
			      IPC_VARCONTENT_PTR varcontent)
{
  return _IPC_marshall(formatter, dataptr, varcontent, TRUE);
}

static IPC_RETURN_TYPE _IPC_unmarshall (FORMATTER_PTR formatter,
					BYTE_ARRAY byteArray, 
					void **dataHandle,
					BOOLEAN mallocData)
{
  int32 dataSize, byteOrder;
  ALIGNMENT_TYPE alignment;

  if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else {
    dataSize = x_ipc_dataStructureSize(formatter);
    if (dataSize == 0) {
      *dataHandle = NULL;
    } else {
      if (mallocData) {
	*dataHandle = x_ipcMalloc((unsigned)dataSize);
      }
      LOCK_M_MUTEX;
      byteOrder = GET_M_GLOBAL(byteOrder);
      alignment = GET_M_GLOBAL(alignment);
      UNLOCK_M_MUTEX;
      if (byteOrder == BYTE_ORDER &&
	  x_ipc_sameFixedSizeDataBuffer(formatter)) {
	BCOPY(byteArray, *dataHandle, dataSize);
      } else {
	x_ipc_decodeData(formatter, (char *)byteArray, 0, (char *)*dataHandle, 
			 byteOrder, alignment, -1);
      }
    }
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_unmarshall (FORMATTER_PTR formatter,
				BYTE_ARRAY byteArray, 
				void **dataHandle)
{
  return _IPC_unmarshall(formatter, byteArray, dataHandle, TRUE);
}

IPC_RETURN_TYPE IPC_unmarshallData(FORMATTER_PTR formatter,
				   BYTE_ARRAY byteArray,
				   void *dataHandle,
				   int dataSize)
{
  if (!formatter) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (dataSize != x_ipc_dataStructureSize(formatter)) {
    RETURN_ERROR(IPC_Wrong_Buffer_Length);
  } else {
    return _IPC_unmarshall(formatter, byteArray, &dataHandle, FALSE);
  }
}

IPC_RETURN_TYPE IPC_publishData (const char *msgName, void *dataptr)
{
  IPC_VARCONTENT_TYPE varcontent;
  IPC_RETURN_TYPE retVal;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (_IPC_marshall(IPC_msgFormatter(msgName), 
			   dataptr, &varcontent, FALSE) != IPC_OK){
    PASS_ON_ERROR();
  } else {
    retVal = IPC_publishVC(msgName, &varcontent);
    if (varcontent.content != dataptr) x_ipcFree(varcontent.content);
    return retVal;
  }
}

IPC_RETURN_TYPE IPC_respondData (MSG_INSTANCE msgInstance,
				 const char *msgName, void *dataptr)
{
  IPC_VARCONTENT_TYPE varcontent;
  IPC_RETURN_TYPE retVal;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (!msgInstance) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (_IPC_marshall(IPC_msgFormatter(msgName),
			   dataptr, &varcontent, FALSE) != IPC_OK) {
    PASS_ON_ERROR();
  } else {
    retVal = IPC_respondVC(msgInstance, msgName, &varcontent);
    if (varcontent.content != dataptr) x_ipcFree(varcontent.content);
    return retVal;
  }
}

IPC_RETURN_TYPE IPC_queryNotifyData (const char *msgName, void *dataptr,
				     HANDLER_TYPE handler, void *clientData)
{
  IPC_VARCONTENT_TYPE varcontent;
  IPC_RETURN_TYPE retVal;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (_IPC_marshall(IPC_msgFormatter(msgName), 
			   dataptr, &varcontent, FALSE) != IPC_OK) {
    PASS_ON_ERROR();
  } else {
    retVal = IPC_queryNotifyVC(msgName, &varcontent, handler, clientData);
    if (varcontent.content != dataptr) x_ipcFree(varcontent.content);
    return retVal;
  }
}

IPC_RETURN_TYPE IPC_queryResponseData (const char *msgName, 
				       void *dataptr, void **replyData,
				       unsigned int timeoutMsecs)
{
  IPC_VARCONTENT_TYPE varcontent;
  IPC_RETURN_TYPE retVal;
  BYTE_ARRAY replyByteArray;
  CONST_FORMAT_PTR decodeFormat;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (_IPC_marshall(IPC_msgFormatter(msgName), 
			   dataptr, &varcontent, FALSE) != IPC_OK) {
    PASS_ON_ERROR();
  } else {
    retVal = _IPC_queryResponse(msgName, varcontent.length, varcontent.content,
				&replyByteArray, &decodeFormat, timeoutMsecs);
    if (retVal == IPC_OK) {
      retVal = _IPC_unmarshall(decodeFormat, replyByteArray, replyData, TRUE);
      if (replyByteArray != replyData) x_ipcFree(replyByteArray);
    } else {
      *replyData = NULL;
    }
    if (varcontent.content != dataptr) x_ipcFree(varcontent.content);
    return retVal;
  }
}

IPC_RETURN_TYPE IPC_freeData (FORMATTER_PTR formatter, void *dataptr)
{
  if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else if ((formatter && !dataptr) || (dataptr && !formatter)) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    if (dataptr) x_ipc_freeDataStructure(formatter, dataptr);
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_freeDataElements (FORMATTER_PTR formatter, void *dataptr)
{
  if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else if ((formatter && !dataptr) || (dataptr && !formatter)) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    x_ipc_freeDataElements(formatter, (GENERIC_DATA_PTR)dataptr, 0, NULL);
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_printData (FORMATTER_PTR formatter, FILE *stream,
			       void *dataptr)
{
  if (!X_IPC_INITIALIZED()) {
    RETURN_ERROR(IPC_Not_Initialized);
  } else if (formatter && formatter->type == BadFormatFMT) {
    RETURN_ERROR(IPC_Illegal_Formatter);
  } else if (!stream || (formatter && !dataptr) || (dataptr && !formatter)) {
    RETURN_ERROR(IPC_Null_Argument);
  } else {
    Print_Formatted_Data(stream, formatter, dataptr);
    return IPC_OK;
  }
}

IPC_RETURN_TYPE IPC_readData (FORMATTER_PTR formatter, FILE *stream,
			      void **dataHandle)
{
#ifdef UNUSED_PRAGMA
#pragma unused(formatter, stream, dataHandle)
#endif
  X_IPC_MOD_ERROR("IPC_readData: Not yet implemented\n");
  return IPC_OK;
}

IPC_RETURN_TYPE IPC_checkMsgFormats (const char *msgName,
				     const char *formatString)
{
  MSG_PTR msg;
  CONST_FORMAT_PTR format;
  BOOLEAN sameP;

  if (!msgName || strlen(msgName) == 0) {
    RETURN_ERROR(IPC_Null_Argument);
  } else if (!X_IPC_CONNECTED()) {
    RETURN_ERROR(IPC_Not_Connected);
  } else {
    msg = x_ipc_msgFind(msgName);
    if (!msg) {
      RETURN_ERROR(IPC_Message_Not_Defined);
    } else {
      format = IPC_parseFormat(formatString);
      sameP = formatsEqual(format, msg->msgData->resFormat);
      x_ipc_freeFormatter(&format);
      if (sameP) {
	return IPC_OK;
      } else {
	RETURN_ERROR(IPC_Mismatched_Formatter);
      }
    }
  }
}

void IPC_freeByteArray (BYTE_ARRAY byteArray)
{
  x_ipcFree(byteArray);
}

#ifdef LISP
/* Prototypes, to keep compiler happy */
#ifdef macintosh
#pragma export on
#endif
BYTE_ARRAY IPC_createByteArray (unsigned int length);
BUFFER_PTR ipcSetEncodeBuffer (BYTE_ARRAY byteArray);
#ifdef macintosh
#pragma export off
#endif

BYTE_ARRAY IPC_createByteArray (unsigned int length)
{
  return x_ipcMalloc(length);
}

/* Need a scheme that does not malloc memory */
BUFFER_PTR ipcSetEncodeBuffer (BYTE_ARRAY byteArray)
{
  BUFFER_PTR buffer = NEW(BUFFER_TYPE);

  buffer->bstart = 0;
  buffer->buffer = (char *)byteArray;

  return buffer;
}
#endif /* LISP */

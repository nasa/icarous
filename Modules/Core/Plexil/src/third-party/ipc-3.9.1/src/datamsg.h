/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: datamsg
 *
 * FILE: datamsg.h
 *
 * ABSTRACT:
 * 
 * A datamsg represents the encoded form of a message and is used to perform
 * the actual sending and receiving of messages.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: datamsg.h,v $
 * Revision 2.3  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.2  2000/07/03 17:03:22  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:19  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:31:14  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.20  1996/01/30  15:04:00  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.19  1996/01/05  16:31:18  rich
 * Added windows NT port.
 *
 * Revision 1.18  1995/08/14  21:31:34  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.17  1995/08/05  21:11:51  reids
 * The "sharedBuffers" flag was not always being set.
 *
 * Revision 1.16  1995/08/05  18:13:18  rich
 * Fixed problem with x_ipc_writeNBuffers on partial writes.
 * Added "sharedBuffers" flag to the dataMsg structure, rather than
 * checking to see if the dataStruct pointer and the message data pointer
 * are the same.  This allows central to clear the dataStruc pointer so
 * that messages don't try to access old data structures that might have
 * changed since the  message was created.
 *
 * Revision 1.15  1995/07/12  04:54:40  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.14  1995/05/31  19:35:18  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.13  1995/04/19  14:28:00  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.12  1995/04/04  19:42:02  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.11  1995/03/30  15:42:44  rich
 * DBMALLOC works.  To use "gmake -k -w DBMALLOC=DBMALLOC install"
 * Added simple list of strings data structure that can be passed via x_ipc
 * messages.
 * Use the string list to maintain a global variable of messages with taps.
 * Tapped messages are not sent via direct connections.
 * Implemented code to vectorize data to be sent so that it does not have
 * to be copied.  Currently, only flat, packed data structures are
 * vectored.  This can now be easily extended.
 * Changed Boolean -> BOOLEAN for consistency and to avoid conflicts with x11.
 * Fixed bug were central would try and free the "***New Module***" and
 * "*** Unkown Host***" strings when a module crashed on startup.
 * Fixed a bug reported by Jay Gowdy where the code to find the size of a
 * variable lenght array would access already freed data when called from
 * x_ipcFreeData.
 *
 * Revision 1.10  1995/03/16  18:05:17  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.9  1995/01/30  16:17:49  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.8  1995/01/18  22:40:11  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/17  23:15:35  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1994/04/16  19:41:58  rich
 * First release of X_IPC for the DEC alpha.
 * Changes were needed because longs are 64 bits.
 * Fixed alignment assumption in the data message format.
 * Fixed the way offsets are calculated for variable length arrays.  This
 * was a problem even without 64 bit longs and pointers.
 *
 * Added the commit date to the version information printed out with the -v
 * option.
 *
 * Now uses standard defines for byte order
 * (BYTE_ORDER = BIG_ENDIAN, LITTLE_ENDIAN or PDP_ENDIAN)
 *
 * Defined alignment types: ALIGN_INT ALINE_LONGEST and ALIGN_WORD.
 *
 * *** WARNING ***
 * sending longs between alphas and non-alpha machines will probably not work.
 * *** WARNING ***
 *
 * Revision 1.5  1993/12/01  18:03:18  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:17:32  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:14:31  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:09  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:42  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:31  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:27  fedor
 * Added Logging.
 *
 *  5-Jul-90 Christopher Fedor, School of Computer Science, CMU
 * Added refCount for dealing with datamsg garbage collection.
 *
 * 13-Jun-90 Christopher Fedor, School of Computer Science, CMU
 * Changed parameter names to reflect new design. Replaced issuer, intent, 
 * select, object, msgRef with parentRef, intent, classId, dispatchRef, msgRef.
 * msgRef is a x_ipc generated msg id on sending, it is primarily used for
 * query until a similar class data idea is designed for the module side.
 *
 * 29-May-90 Christopher Fedor, School of Computer Science, CMU
 * Revised for x_ipc 5.x
 *
 * 26-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Modified to track two seperate data encodings.
 *
 * 24-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.3 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCdatamsg
#define INCdatamsg

/* The order and number of items in the DataMsg struct is
   CRIX_IPCL to its CORRECT USE - see notes */

typedef struct _DM {
  /* Pointers to the buffers. */
  char *classData;
  char *msgData;
  const char *dataStruct;
  /* Information about the message. */
  int32 *dataRefCountPtr;
  struct iovec *vec;
  int32 refCount;
  int32 dataByteOrder;  /* of the dataStruct buffer */
  int32 classByteOrder; /* of the dataStruct buffer */
  ALIGNMENT_TYPE alignment;      /* of the dataStruct buffer */
  /* Start the of transmitted part. */
  /* The rest of the data structure must be tightly packed. */
  int32 classTotal;
  int32 msgTotal;
  int32 parentRef;
  int32 intent;
  int32 classId;      /* Endian and packing are in the upper two bytes. */
  int32 dispatchRef;
  int32 msgRef;
} DATA_MSG_TYPE, *DATA_MSG_PTR;

/* How much header information to send/receive */
#define HEADER_SIZE() (7*sizeof(int32))

/***********************************************************************/

typedef struct {
  int32 size, allocated;
  DATA_MSG_PTR dataMsg;
} DATA_MSG_BUF_TYPE, *DATA_MSG_BUF_PTR;

X_IPC_RETURN_STATUS_TYPE x_ipc_readNBytes(int sd, char *buf, int32 nbytes);
X_IPC_RETURN_STATUS_TYPE x_ipc_writeNBytes(int sd, char *buf, int32 nbytes);

void x_ipc_dataMsgInitialize(void);
X_IPC_RETURN_STATUS_TYPE x_ipc_dataMsgRecv(int sd, DATA_MSG_PTR *dataMsg,
				   int32 replyRef, void *replyBuf, int32 replyLen);
X_IPC_RETURN_STATUS_TYPE x_ipc_dataMsgSend(int sd, DATA_MSG_PTR dataMsg);
DATA_MSG_PTR x_ipc_dataMsgCreate(int32 parentRef, int32 intent, int32 classId, 
			   int32 dispatchRef, int32 msgRef,
			   CONST_FORMAT_PTR msgFormat,
			   const void *msgData, 
			   CONST_FORMAT_PTR classFormat, 
			   const void *classData);

void *x_ipc_dataMsgDecodeMsg(CONST_FORMAT_PTR msgFormat,DATA_MSG_PTR dataMsg,
		       BOOLEAN keepData);
void *x_ipc_dataMsgDecodeClass(CONST_FORMAT_PTR classFormat,
			 DATA_MSG_PTR dataMsg);
DATA_MSG_PTR x_ipc_dataMsgFree(DATA_MSG_PTR dataMsg);
BOOLEAN x_ipc_dataMsgTestMsgData(DATA_MSG_PTR dataMsg);
void x_ipc_encodeMsgData(CONST_FORMAT_PTR Format, const void *DataStruct, 
		   DATA_MSG_PTR dataMsg, int32 BStart);
void *x_ipc_decodeMsgData(CONST_FORMAT_PTR Format,  DATA_MSG_PTR dataMsg,
		    BOOLEAN keepData);
DATA_MSG_PTR x_ipc_dataMsgReplaceClassData(CONST_FORMAT_PTR classFormat,
				     void *data,
				     DATA_MSG_PTR dataMsg,
				     CONST_FORMAT_PTR msgFormat);
void x_ipc_dataMsgDisplayStats(FILE *stream);

#endif /* INCdatamsg */

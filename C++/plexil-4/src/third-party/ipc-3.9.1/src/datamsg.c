/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: datamsg
 *
 * FILE: datamsg.c
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
 * $Log: datamsg.c,v $
 * Revision 2.7  2010/12/17 19:20:23  reids
 * Split IO mutex into separate read and write mutexes, to help minimize
 *   probability of deadlock when reading/writing very big messages.
 * Fixed a bug in multi-threaded version where a timeout is not reported
 *   correctly (which could cause IPC_listenClear into a very long loop).
 *
 * Revision 2.6  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.5  2003/04/14 15:31:01  reids
 * Updated for Windows XP
 *
 * Revision 2.4  2002/06/25 16:45:26  reids
 * Added casts to satisfy compiler.
 *
 * Revision 2.3  2002/01/03 20:52:11  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:22  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.10  1997/03/07 17:49:35  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.1.2.9  1997/01/27 20:09:16  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.7  1997/01/16 22:16:27  reids
 * Improved the way usage stats are reported.
 *
 * Revision 1.1.2.6  1997/01/11 01:20:52  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.5.4.1  1996/12/24 14:41:32  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.1.2.5  1996/12/18 15:12:43  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.4  1996/10/29 14:52:04  reids
 * Make "byteOrder" and "alignment" vars available to C, not just LISP.
 *
 * Revision 1.1.2.3  1996/10/18 18:00:44  reids
 * Record broadcast messages if handler is registered before message.
 * Fixed reading of large messages.
 * Fixed memory problems.
 *
 * Revision 1.1.2.2  1996/10/16 15:10:28  reids
 * Added an argument to x_ipc_encodeData to perform a sanity check.
 *
 * Revision 1.1.2.1  1996/10/02 20:58:25  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.2  1996/09/13 21:06:14  udo
 *  fixed data type for sgi IRIX62
 *
 * Revision 1.1  1996/05/09 01:01:19  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/12 03:19:38  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:12  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.46  1996/07/19  18:13:53  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.45  1996/06/25  20:50:26  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.44  1996/05/09  18:30:45  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.43  1996/05/07  16:49:20  rich
 * Changes for clisp.
 *
 * Revision 1.42  1996/03/09  06:13:13  rich
 * Fixed problem where lisp could use the wrong byte order if it had to
 * query for a message format.  Also fixed some memory leaks.
 *
 * Revision 1.41  1996/02/10  16:49:45  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.40  1996/02/06  19:04:31  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.39  1996/01/27  21:53:14  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.38  1996/01/10  03:16:20  rich
 * Fixed libx_ipc_lisp.a to work with dbmalloc.  Added central commands to
 * show resource state and to unlock locked resouces.  Fixed a bug where
 * dispatches were not freed when handlers were cleared. Reset errno variable.
 *
 * Revision 1.37  1996/01/05  16:31:14  rich
 * Added windows NT port.
 *
 * Revision 1.36  1995/08/14  21:31:30  rich
 * Got rid of the "sharedBuffers" flag on the dataMessages.  It was not the
 * right solution, and possibly caused a memory leak.
 * Limit pending for X_IPC_TAPPED_MSG_VAR to 1.
 *
 * Revision 1.35  1995/08/05  21:11:41  reids
 * The "sharedBuffers" flag was not always being set.
 *
 * Revision 1.34  1995/08/05  18:16:27  rich
 * Removed debugging code.
 *
 * Revision 1.33  1995/08/05  18:13:14  rich
 * Fixed problem with x_ipc_writeNBuffers on partial writes.
 * Added "sharedBuffers" flag to the dataMsg structure, rather than
 * checking to see if the dataStruct pointer and the message data pointer
 * are the same.  This allows central to clear the dataStruc pointer so
 * that messages don't try to access old data structures that might have
 * changed since the  message was created.
 *
 * Revision 1.32  1995/08/04  16:40:59  rich
 * Fixed problem with in place translation of doubles from Big to little endian.
 *
 * Revision 1.31  1995/07/19  14:26:06  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.30  1995/07/12  04:54:37  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.29  1995/07/10  16:17:11  rich
 * Interm save.
 *
 * Revision 1.28  1995/07/06  21:39:36  rich
 * Fixes ported from 7.9.
 *
 * Revision 1.27  1995/07/06  21:15:59  rich
 * Solaris and Linux changes.
 *
 * Revision 1.26  1995/06/05  23:59:00  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.25  1995/04/19  14:27:58  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.24  1995/04/07  05:03:04  rich
 * Fixed GNUmakefiles to find the release directory.
 * Cleaned up libc.h file for sgi and vxworks.  Moved all system includes
 * into libc.h
 * Got direct queries to work.
 * Fixed problem in allocating/initializing generic mats.
 * The direct flag (-c) now mostly works.  Connect message has been extended to
 * indicate when direct connections are the default.
 * Problem with failures on sunOS machines.
 * Fixed problem where x_ipcError would not print out its message if logging had
 * not been initialized.
 * Fixed another memory problem in modVar.c.
 * Fixed problems found in by sgi cc compiler.  Many type problems.
 *
 * Revision 1.23  1995/04/04  19:42:00  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.22  1995/03/30  15:42:42  rich
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
 * Revision 1.21  1995/03/16  18:05:15  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.20  1995/03/14  22:36:41  rich
 * Fixed problem with multiple read needed when doing a vector read. (Fix
 * to version 7.9)
 * Also fixed the data size problem from 7.9.
 *
 * Revision 1.19  1995/01/30  16:17:45  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.18.2.2  1995/03/14  03:57:26  rich
 * Added a data reference count to the dataMsg type.  It is used to decide
 * when to free the data associated with a message.  Messages can share
 * data buffers.
 * Fixed bug in the vector read routine (x_ipc_read2Buffers).  It would not
 * correctly update the buffer pointers if multiple reads were needed.
 *
 * Revision 1.18.2.1  1995/02/26  22:45:22  rich
 * I thought the class data came after the message data in transmission
 * like it does in the message data structure, but it does not.
 * The data transmission order is header->msgData->classData.
 * This has been changed in versin 8.0.
 *
 * Revision 1.18  1995/01/18  22:40:08  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.17  1994/05/31  03:23:47  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.16  1994/05/17  23:15:33  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.15  1994/05/11  01:57:18  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.14  1994/04/28  16:15:43  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.13  1994/04/26  16:23:23  rich
 * Now you can register an exit handler before anything else and it will
 * get called if connecting to central fails.
 * Also added code to handle pipe breaks during writes.
 *
 * Revision 1.12  1994/04/16  19:41:56  rich
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
 * Revision 1.11  1994/04/04  16:01:10  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.10  1993/12/14  17:33:18  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.9  1993/12/01  18:03:11  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.8  1993/11/21  20:17:30  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.7  1993/10/20  19:00:30  rich
 * Fixed bug with self registed messages in the lisp version.
 * Added new routine : x_ipcGetServerGlobal to get the server socket.
 * Fixed some bad global references for the lisp version.
 * Updated some prototypes.
 *
 * Revision 1.6  1993/08/30  23:13:45  fedor
 * Added SUN4 as well as sun4 compile flag.
 * Corrected Top level failure message name display with a define in dispatch.c
 *
 * Revision 1.4  1993/08/27  07:14:29  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/20  23:06:46  fedor
 * Minor changes for merge. Mostly added htons and removed cfree calls.
 *
 * Revision 1.2  1993/05/26  23:17:06  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:19  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:26  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:25  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 6-May-92 Christopher Fedor, School of Computer Science, CMU
 * Added EWOULDBLOCK detection - for now simply continue to try to write
 * where it left off. THis may not be correct for all applications and can
 * cause the central server to simply wedge.
 *
 *  9-Jul-1991 Reid Simmons, School of Computer Science, CMU
 * Commented out stuff to store data message buffers.
 *
 *  8-Jul-1991 Reid Simmons, School of Computer Science, CMU
 * Added routine to pre-allocate a datamsg buffer
 *
 * 25-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Replaced fixed number of datamsg buffers with Reid's datamsg code.
 * Just postponing the eventual memory lossage.
 *
 * 11-Jun-91 Christopher Fedor, School of Computer Science, CMU
 * Set a fixed number of datamsg buffers.
 *
 * 30-Jan-91 Christopher Fedor, School of Computer Science, CMU
 * Added fflush(stdout) to printf for module code calls from lisp
 *
 * 17-Sep-90 Christopher Fedor, School of Computer Science, CMU
 * Added stats for tracking datamsg memory usage.
 *
 *  5-Jul-90 Christopher Fedor, School of Computer Science, CMU
 * Added refCount for dealing with datamsg garbage collection.
 *
 *  4-Jun-90 Christopher Fedor, School of Computer Science, CMU
 * Revised to Software Standards.
 *
 * 29-May-90 Christopher Fedor, School of Computer Science, CMU
 * Revised double encoding of datamsg to support class data for x_ipc 5.x
 *
 * 26-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Changed to support double encoding.
 *
 * 23-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Interface to Communications by dataMsgs
 *
 *    Dec-88 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * 15-Nov-88 Christopher Fedor  created x_ipc_readNBytes
 *
 * NOTES:
 *
 * 26-Apr-89 Christopher Fedor, School of Computer Science, CMU
 * Need to insure that central will not block on socket reads and writes. 
 *
 * $Revision: 2.7 $
 * $Date: 2010/12/17 19:20:23 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

/* Correctly accounts for EINTR errors during read's or write's */

#ifndef _WINSOCK_
#define SAFE_IO(status, fn_call) \
  do { status = fn_call;} while (status < 0 && errno == EINTR);
#else
#define SAFE_IO(status, fn_call) \
  do { status = fn_call;} \
  while (status == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);
#endif

/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_STATUS_TYPE x_ipc_readNBytes(sd, buf, nbytes)
 *
 * DESCRIPTION:
 * Read nbytes of data from sd into buf. Continue to read until
 * nbytes have been read.
 *
 * INPUTS:
 * int sd;
 * char *buf;
 * int32 nbytes;
 *
 * OUTPUTS: X_IPC_RETURN_STATUS
 *
 * NOTES:
 *
 * buf is a preallocated pointer to storage equal to nbytes.
 * Propagation of low level errors - failures on read/writes still an issue.
 *
 *****************************************************************************/

X_IPC_RETURN_STATUS_TYPE x_ipc_readNBytes(int sd, char *buf, int32 nbytes)
{
  int32 amountRead, amountToRead;
  
  amountToRead = nbytes;
  LOCK_IO_READ_MUTEX;
  for(;;){
#ifdef _WINSOCK_
    SAFE_IO(amountRead, recv(sd, buf, amountToRead, 0));
#else
#ifdef OS2
    SAFE_IO(amountRead, read(sd, buf, MIN(amountToRead,MAX_SOCKET_PACKET)));
#else
    SAFE_IO(amountRead, read(sd, buf, amountToRead));
#endif
#endif
    if (amountRead < 0) {
      UNLOCK_IO_READ_MUTEX;
      return StatError;
    }
    if (!amountRead) {
      UNLOCK_IO_READ_MUTEX;
      return StatEOF;
    }
    amountToRead -= amountRead;
    if (!amountToRead) {
      UNLOCK_IO_READ_MUTEX;
      return StatOK;
    }
    buf += amountRead;
  }
  UNLOCK_IO_READ_MUTEX;
}

#if defined(NEED_READV)

#define readv my_readv
#define writev my_writev

static int32 my_readv (int fd, struct iovec *iov, int32 iovcnt)
{
  int32 i, totalCharsRead=0, charsRead;
  int32 readResult;

  for(i=0;i<iovcnt; i++){
    charsRead = 0;
    while (charsRead < iov[i].iov_len) {
#ifdef _WINSOCK_
      SAFE_IO(readResult, 
	      recv(fd,iov[i].iov_base+charsRead,
		   min(iov[i].iov_len-charsRead,MAX_SOCKET_PACKET), 0));
#else
      SAFE_IO(readResult, 
	      read(fd,iov[i].iov_base+charsRead,
		   MIN(iov[i].iov_len-charsRead,MAX_SOCKET_PACKET)));
#endif
      if (readResult < 0) {
	/* Errror, return and  */
	return readResult;
      } else {
	/* Read some, update amounts. */
	charsRead += readResult;
	totalCharsRead += readResult;
      }
    }
  }
  return totalCharsRead;
}

static int32 my_writev (int fd, struct iovec *iov, int32 iovcnt)
{
  int32 i, totalCharsWrite=0, charsWrite;
  int32 writeResult;
  
  for(i=0;i<iovcnt; i++){
    charsWrite = 0;
    while (charsWrite <  iov[i].iov_len) {
#ifdef _WINSOCK_
      SAFE_IO(writeResult, 
	      send(fd,iov[i].iov_base+charsWrite,
		   min(iov[i].iov_len-charsWrite,MAX_SOCKET_PACKET),0));
#else
      SAFE_IO(writeResult, 
	      write(fd,iov[i].iov_base+charsWrite,
		    MIN(iov[i].iov_len-charsWrite,MAX_SOCKET_PACKET)));
#endif
      if (writeResult < 0) {
	/* Errror, return and  */
	return writeResult;
      } else {
	/* Write some, update amounts. */
	charsWrite += writeResult;
	totalCharsWrite += writeResult;
      }
    }
  }
  return totalCharsWrite;
  
}

#endif


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_STATUS_TYPE x_ipc_read2Buffers(sd, buf1, nbytes,
 *                                               buf2, nbytes)
 *
 * DESCRIPTION:
 * Read nbytes of data from sd into buf. Continue to read until
 * nbytes have been read.
 *
 * INPUTS:
 * int sd;
 * char *buf1;
 * int32 nbytes1;
 * char *buf2;
 * int32 nbytes2;
 *
 * OUTPUTS: X_IPC_RETURN_STATUS
 *
 * NOTES:
 *
 * buf is a preallocated pointer to storage equal to nbytes.
 * Propagation of low level errors - failures on read/writes still an issue.
 *
 *****************************************************************************/

static X_IPC_RETURN_STATUS_TYPE x_ipc_read2Buffers(int sd,
						   char *buf1, int32 nbytes1,
						   char *buf2, int32 nbytes2)
{
  int32 amountRead, amountToRead;
  int32 numBuffers = 2;
  
  struct iovec vec[2];
  
  vec[0].iov_base = buf1;
  vec[0].iov_len = nbytes1;
  vec[1].iov_base = buf2;
  vec[1].iov_len = nbytes2;
  
  amountToRead = nbytes1 + nbytes2;
  LOCK_IO_READ_MUTEX;
  for(;;){
    SAFE_IO(amountRead, readv(sd, vec, numBuffers));
    if (amountRead < 0) {
      UNLOCK_IO_READ_MUTEX;
      return StatError;
    }
    if (!amountRead) {
      UNLOCK_IO_READ_MUTEX;
      return StatEOF;
    }
    amountToRead -= amountRead;
    if (!amountToRead) {
      UNLOCK_IO_READ_MUTEX;
      return StatOK;
    }
    /* Need to adjust buffers */
    if (amountToRead <= vec[1].iov_len) {
      /* Only need to use one buffer */
      numBuffers = 1;
      vec[0].iov_base = buf2 + nbytes2 - amountToRead;
      vec[0].iov_len = amountToRead;
    } else {
      /* Still need both buffers. */
      vec[0].iov_base = ((char *) vec[0].iov_base) + amountRead;
      vec[0].iov_len -= amountRead;
    }
  }
  UNLOCK_IO_READ_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_STATUS_TYPE x_ipc_writeNBytes(sd, buf, nbytes)
 *
 * DESCRIPTION: This routine primarily calls the system function write.
 *
 * INPUTS:
 * int sd;
 * char *buf;
 * int32 nbytes;
 *
 * OUTPUTS: X_IPC_RETURN_STATUS_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_STATUS_TYPE x_ipc_writeNBytes(int sd, char *buf, int32 nbytes)
{
  int32 amountWritten = 0;
  BOOLEAN *pipeBrokenPtr;
  
  LOCK_IO_MUTEX;
  LOCK_M_MUTEX;
  pipeBrokenPtr = &GET_M_GLOBAL(pipeBroken);
  UNLOCK_M_MUTEX;
  while (nbytes > 0) {
    *pipeBrokenPtr = FALSE;
#ifndef OS2
    errno = 0;
#endif
#ifdef _WINSOCK_
    SAFE_IO(amountWritten, send(sd, buf, nbytes,0));
#else
    SAFE_IO(amountWritten, write(sd, buf, nbytes));
#endif
    if (*pipeBrokenPtr || (errno == EPIPE)) {
      X_IPC_MOD_WARNING( "\nWARNING: pipe broken!\n");
      UNLOCK_IO_MUTEX;
      return StatError;
    } else if (amountWritten < 0) {
#if defined(VXWORKS) || defined(THINK_C) || defined(macintosh)
      UNLOCK_IO_MUTEX;
      return StatError;
#else
#ifdef _WINSOCK_
      if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
	if (errno == EWOULDBLOCK)
#endif
	  {
	    X_IPC_MOD_WARNING(
			  "\nWARNING: x_ipc_writeNBytes: EWOULDBLOCK: trying again!\n");
	    PAUSE_MIN_DELAY();
	} else {
	  UNLOCK_IO_MUTEX;
	  return StatError;
	}
#endif
    } else {
      nbytes -= amountWritten;
      buf += amountWritten;
    }
  }
  UNLOCK_IO_MUTEX;
  return StatOK;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_STATUS_TYPE
 *           x_ipc_writeNBuffers(sd, struct iovec *vec)
 *
 * DESCRIPTION: This routine primarily calls the system function write.
 *
 * INPUTS:
 * int sd;
 * char *buf;
 * int32 nbytes;
 *
 * OUTPUTS: X_IPC_RETURN_STATUS_TYPE
 *
 *****************************************************************************/

static X_IPC_RETURN_STATUS_TYPE x_ipc_writeNBuffers(int sd, struct iovec *vec,
						    int32 amount)
{
  int32 amountWritten = 0;
  int32 numBuffers=0;
  int32 amountToWrite=0;
  int32 i, start=0;
  BOOLEAN *pipeBrokenPtr;
  
  for (i=0; (vec[i].iov_base != NULL); i++) {
    numBuffers++;
    amountToWrite += vec[i].iov_len;
  }
  
  if (amountToWrite != amount) {
    X_IPC_MOD_ERROR("Internal Error: Amounts incorrect in msg send.\n");
    return StatError;
  }
  
  LOCK_IO_MUTEX;
  LOCK_M_MUTEX;
  pipeBrokenPtr = &GET_M_GLOBAL(pipeBroken);
  UNLOCK_M_MUTEX;
  while (amountToWrite > 0) {
    *pipeBrokenPtr = FALSE;
#ifndef OS2
    errno = 0;
#endif
    SAFE_IO(amountWritten, writev(sd, &vec[start], numBuffers));
    if (*pipeBrokenPtr || (errno == EPIPE)) {
      X_IPC_MOD_WARNING( "\nWARNING: pipe broken!\n");
      x_ipcFree((char *)vec);
      UNLOCK_IO_MUTEX;
      return StatError;
    } else if (amountWritten < 0) {
#if defined(VXWORKS) || defined(THINK_C) || defined(macintosh)
      x_ipcFree((char *)vec);
      UNLOCK_IO_MUTEX;
      return StatError;
#else
#ifdef _WINSOCK_
      if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
	if (errno == EWOULDBLOCK)
#endif
	  {
	    X_IPC_MOD_WARNING(
			  "\nWARNING: x_ipc_writeNBytes: EWOULDBLOCK: trying again!\n");
	    PAUSE_MIN_DELAY();
	} else {
	  x_ipcFree((char *)vec);
	  UNLOCK_IO_MUTEX;
	  return StatError;
	}
#endif
    } else {
      amountToWrite -= amountWritten;
      if (amountToWrite > 0) {
	while (amountWritten > 0) {
	  if (vec[start].iov_len <= amountWritten) {
	    amountWritten -= vec[start].iov_len;
	    start++;
	    numBuffers--;
	  } else if (vec[start].iov_len > amountWritten) {
	    vec[start].iov_len -= amountWritten;
#ifndef _SGI_SOURCE
	    vec[start].iov_base += amountWritten;
#else
	    vec[start].iov_base =
	      (caddr_t)((int32)vec[start].iov_base + amountWritten);
#endif
	    amountWritten = 0;
	  }
	}
      }
    }
  }
  UNLOCK_IO_MUTEX;
  x_ipcFree((char *)vec);
  return StatOK;
}

/*****************************************************************************/

/*****************************************************************************/

void x_ipc_dataMsgInitialize(void)
{
  LOCK_M_MUTEX;
  GET_M_GLOBAL(DMFree) = 0;
  GET_M_GLOBAL(DMTotal) = 0;
  GET_M_GLOBAL(DMmin) = 2000000000;
  GET_M_GLOBAL(DMmax) = -1;
  
  GET_M_GLOBAL(dataMsgBufferList) = x_ipc_listCreate();
  UNLOCK_M_MUTEX;
}

/*****************************************************************************/

/*****************************************************************************/

static DATA_MSG_PTR x_ipc_dataMsgAlloc(int32 size)
{
  DATA_MSG_PTR dataMsg;
  
  dataMsg = (DATA_MSG_PTR)x_ipcMalloc((unsigned)size);
  
  LOCK_M_MUTEX;
  (GET_M_GLOBAL(DMTotal))++;
  
  if (size > (GET_M_GLOBAL(DMmax))) (GET_M_GLOBAL(DMmax)) = size;
  if (size < (GET_M_GLOBAL(DMmin))) (GET_M_GLOBAL(DMmin)) = size;
  UNLOCK_M_MUTEX;
  
  return (dataMsg);
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_dataMsgFree(dataMsg)
 *
 * DESCRIPTION: Recycle a datamsg.
 *
 * INPUTS: DATA_MSG_PTR dataMsg
 *
 * OUTPUTS: none.
 *
 *****************************************************************************/

DATA_MSG_PTR x_ipc_dataMsgFree(DATA_MSG_PTR dataMsg)
{
  if (dataMsg) {
    (dataMsg->refCount)--;
    if (dataMsg->refCount < 1) {
      if (dataMsg->dataRefCountPtr != NULL) {
	(*(dataMsg->dataRefCountPtr))--;
	if (*(dataMsg->dataRefCountPtr) < 1) {
	  /* Can now free the data. */
	  x_ipcFree((char *)dataMsg->dataRefCountPtr);
	  dataMsg->dataRefCountPtr = NULL;
 	  if ((dataMsg->msgData != NULL) &&
 	      (dataMsg->msgData != dataMsg->dataStruct)) {
	    x_ipcFree((char *)dataMsg->msgData);
	  }
	}
      }
      if (dataMsg->vec != NULL)
	x_ipcFree((char *)dataMsg->vec);
      dataMsg->vec = NULL;
      
      x_ipcFree((char *)dataMsg);
      LOCK_M_MUTEX;
      (GET_M_GLOBAL(DMFree))++;
      UNLOCK_M_MUTEX;
      dataMsg = NULL;
    }
  }
  return dataMsg;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_STATUS_TYPE x_ipc_dataMsgRecv(sd, dataMsg)
 *
 * DESCRIPTION:
 * Creates a dataMsg and initialize it with information from the socket
 * specified by the value sd. If there is an error in reading the
 * information dataMsg will be initialized to NULL. The communication
 * status is returned.
 *
 * INPUTS:
 * int sd;
 * DATA_MSG_PTR *dataMsg;
 *
 * OUTPUTS: X_IPC_RETURN_STATUS_TYPE
 *
 *****************************************************************************/

X_IPC_RETURN_STATUS_TYPE x_ipc_dataMsgRecv(int sd, DATA_MSG_PTR *dataMsg,
					   int32 replyRef, void *replyBuf, 
					   int32 replyLen)
{
  X_IPC_RETURN_STATUS_TYPE status;
  
  DATA_MSG_TYPE header;

  *dataMsg = NULL;
  
  //LOCK_IO_READ_MUTEX;
  status = x_ipc_readNBytes(sd, (char *)&(header.classTotal), HEADER_SIZE());
  if (status != StatOK) {
    *dataMsg = NULL;
    //UNLOCK_IO_READ_MUTEX;
    return status;
  }
  
  NET_INT_TO_INT(header.classTotal);
  NET_INT_TO_INT(header.msgTotal);

  *dataMsg = x_ipc_dataMsgAlloc(header.classTotal + sizeof(DATA_MSG_TYPE));
  **dataMsg = header;
  
  NET_INT_TO_INT((*dataMsg)->parentRef);
  NET_INT_TO_INT((*dataMsg)->intent);
  NET_INT_TO_INT((*dataMsg)->classId);
  NET_INT_TO_INT((*dataMsg)->dispatchRef);
  NET_INT_TO_INT((*dataMsg)->msgRef);
  
  if( header.msgTotal > 0) {
    (*dataMsg)->dataRefCountPtr = (int32 *)x_ipcMalloc(sizeof(int32));
    *((*dataMsg)->dataRefCountPtr) = 1;
  } else {
    (*dataMsg)->dataRefCountPtr = NULL;
  }
  (*dataMsg)->refCount = 0;
  (*dataMsg)->dataStruct = NULL;
  (*dataMsg)->dataByteOrder = GET_DATA_ENDIAN((*dataMsg)->classId);
  (*dataMsg)->classByteOrder = GET_CLASS_ENDIAN((*dataMsg)->classId);
  (*dataMsg)->alignment = (ALIGNMENT_TYPE)GET_ALIGNMENT((*dataMsg)->classId);
  (*dataMsg)->classId = GET_CLASSID((*dataMsg)->classId);
  LOCK_M_MUTEX;
  GET_M_GLOBAL(byteOrder) = (*dataMsg)->dataByteOrder;
  GET_M_GLOBAL(alignment) = (*dataMsg)->alignment;
  UNLOCK_M_MUTEX;
  
  if (header.classTotal > 0)
    (*dataMsg)->classData = ((char *)*dataMsg + sizeof(DATA_MSG_TYPE));
  else
    (*dataMsg)->classData = NULL;
  
  /*  For now, we only handle packed data. */
  if ((*dataMsg)->alignment != ALIGN_PACKED) {
    X_IPC_MOD_ERROR("ERROR: received message with data that is not packed.");
    //UNLOCK_IO_READ_MUTEX;
    return StatError;
  }
  
  /* Want to be able to use the already allocated buffer, if possible. */
  
  if (((*dataMsg)->msgRef == replyRef) && (replyBuf != NULL) &&
      (replyLen == header.msgTotal)) {
    (*dataMsg)->msgData = (char *)replyBuf;
    (*dataMsg)->dataStruct = (char *)replyBuf;
  } else if (header.msgTotal > 0)
    (*dataMsg)->msgData = (char *)x_ipcMalloc((unsigned) header.msgTotal);
  else
    (*dataMsg)->msgData = NULL;
  
  if ((header.msgTotal > 0) && (header.classTotal >0)) {
    status = x_ipc_read2Buffers(sd, (*dataMsg)->classData, header.classTotal,
				(*dataMsg)->msgData, header.msgTotal);
  } else if (header.classTotal > 0) {
    status = x_ipc_readNBytes(sd, (*dataMsg)->classData, header.classTotal);
  } else if (header.msgTotal > 0) {
    status = x_ipc_readNBytes(sd, (*dataMsg)->msgData, header.msgTotal);
  }
  
  /* Need to create the vector here.  */
  (*dataMsg)->vec = (struct iovec *)x_ipcMalloc(2 * sizeof(struct iovec));
  
  (*dataMsg)->vec[0].iov_base = (*dataMsg)->msgData;
  (*dataMsg)->vec[0].iov_len = (*dataMsg)->msgTotal;
  
  (*dataMsg)->vec[1].iov_base = NULL;
  (*dataMsg)->vec[1].iov_len = 0;
  
  //UNLOCK_IO_READ_MUTEX;
  return status;
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_STATUS_TYPE x_ipc_dataMsgSend(sd, dataMsg)
 *
 * DESCRIPTION:
 * Sends the information in dataMsg to the socket specified by sd. The
 * status of the communication will be returned.
 *
 * INPUTS:
 * int sd;
 * DATA_MSG_PTR dataMsg;
 *
 * OUTPUTS: X_IPC_RETURN_STATUS_TYPE
 * NOTES: Switch byte order for 486 and pmax machines, so they can talk
 *        to SUN machines.
 *
 *****************************************************************************/

X_IPC_RETURN_STATUS_TYPE x_ipc_dataMsgSend(int sd, DATA_MSG_PTR dataMsg)
{
  int32 headerAmount, classAmount, dataAmount;
  X_IPC_RETURN_STATUS_TYPE res;
  char *sendInfo;
  struct iovec *tmpVec;
  
  //  LOCK_IO_MUTEX;
  headerAmount = HEADER_SIZE();
  classAmount = dataMsg->classTotal;
  dataAmount = dataMsg->msgTotal;
  
  sendInfo = (char *)&(dataMsg->classTotal);
  dataMsg->classId = SET_DATA_ENDIAN(dataMsg->classId,dataMsg->dataByteOrder);
  dataMsg->classId = SET_CLASS_ENDIAN(dataMsg->classId,
				      dataMsg->classByteOrder);
  dataMsg->classId = SET_ALIGNMENT(dataMsg->classId);
  
  INT_TO_NET_INT(dataMsg->classTotal);
  INT_TO_NET_INT(dataMsg->msgTotal);
  INT_TO_NET_INT(dataMsg->parentRef);
  INT_TO_NET_INT(dataMsg->intent);
  INT_TO_NET_INT(dataMsg->classId);
  INT_TO_NET_INT(dataMsg->dispatchRef);
  INT_TO_NET_INT(dataMsg->msgRef);
  
  if (classAmount > 0)  {
    tmpVec = x_ipc_copyVectorization(dataMsg->vec,2);
    tmpVec[0].iov_base = sendInfo;
    tmpVec[0].iov_len = headerAmount;
    tmpVec[1].iov_base = dataMsg->classData;
    tmpVec[1].iov_len = classAmount;
    res = x_ipc_writeNBuffers(sd, tmpVec,headerAmount+classAmount+dataAmount);
  } else if (dataAmount > 0) {
    tmpVec = x_ipc_copyVectorization(dataMsg->vec,1);
    tmpVec[0].iov_base = sendInfo;
    tmpVec[0].iov_len = headerAmount;
    res = x_ipc_writeNBuffers(sd, tmpVec,headerAmount+classAmount+dataAmount);
  } else {
    res = x_ipc_writeNBytes(sd, sendInfo, headerAmount);
  }

  NET_INT_TO_INT(dataMsg->classTotal);
  NET_INT_TO_INT(dataMsg->msgTotal);
  NET_INT_TO_INT(dataMsg->parentRef);
  NET_INT_TO_INT(dataMsg->intent);
  NET_INT_TO_INT(dataMsg->classId);
  NET_INT_TO_INT(dataMsg->dispatchRef);
  NET_INT_TO_INT(dataMsg->msgRef);
  
  dataMsg->classId = GET_CLASSID(dataMsg->classId);
  
  // UNLOCK_IO_MUTEX;
  return res;
}


/******************************************************************************
 *
 * FUNCTION:
 *
 * DESCRIPTION:
 * Creates a new dataMsg and encodes data for sending based on Format. If
 * data, or Format or both are NULL then a new dataMsg is still created
 * but only Header Information is initialized.
 *
 * INPUTS:
 *
 * OUTPUTS:
 *
 *****************************************************************************/

DATA_MSG_PTR x_ipc_dataMsgCreate(int32 parentRef, int32 intent, int32 classId,
				 int32 dispatchRef, int32 msgRef,
				 CONST_FORMAT_PTR msgFormat,
				 const void *msgData,
				 CONST_FORMAT_PTR classFormat,
				 const void *classData)
{
#ifdef LISP
  BUFFER_TYPE buffer;
  char *lispDataFlag;
#endif /* LISP */
  DATA_MSG_PTR dataMsg;
  int32 classTotal, msgTotal;
  
  msgTotal = 0;
  
  if (msgData && msgFormat) {
    if (msgFormat->type == BadFormatFMT) return NULL;
#ifdef LISP
    LOCK_M_MUTEX;
    if (msgData == LISP_DATA_FLAG()) {
      (*(GET_M_GLOBAL(lispBufferSizeGlobal)))(&msgTotal, msgFormat);
      UNLOCK_M_MUTEX;
    } else
      UNLOCK_M_MUTEX;
#endif /* LISP */
      msgTotal = x_ipc_bufferSize(msgFormat, msgData);
  }
  
  if (classData && classFormat)
    classTotal = x_ipc_bufferSize(classFormat, classData);
  else
    classTotal = 0;
  
  dataMsg = x_ipc_dataMsgAlloc(classTotal + sizeof(DATA_MSG_TYPE));
  
  dataMsg->dataRefCountPtr = (int32 *)x_ipcMalloc(sizeof(int32));
  *(dataMsg->dataRefCountPtr) = 1;
  dataMsg->refCount = 0;
  dataMsg->vec = NULL;
  
  dataMsg->msgTotal = msgTotal;
  dataMsg->classTotal = classTotal;
  
  dataMsg->parentRef = parentRef;
  dataMsg->intent = intent;
  dataMsg->classId = classId;
  dataMsg->dispatchRef = dispatchRef;
  dataMsg->msgRef = msgRef;
  
#ifdef LISP
  LOCK_M_MUTEX;
  lispDataFlag = LISP_DATA_FLAG();
  UNLOCK_M_MUTEX;
  if ((msgTotal != 0) && (msgData == lispDataFlag)) {
    dataMsg->msgData = (char *)x_ipcMalloc(msgTotal);
    buffer.buffer = dataMsg->msgData;
    buffer.bstart = 0;
    LOCK_M_MUTEX;
    (*(GET_M_GLOBAL(lispEncodeMsgGlobal)))(msgFormat, &buffer);
    UNLOCK_M_MUTEX;
    dataMsg->vec = x_ipc_createVectorization(msgFormat,
					     (char *)msgData,dataMsg->msgData,
					     msgTotal);
  } else
#endif
    { 
      x_ipc_encodeMsgData(msgFormat, msgData, dataMsg, 0);
    }
  
  if (classTotal) {
    dataMsg->classData = (char *)dataMsg + sizeof(DATA_MSG_TYPE);
    x_ipc_encodeData(classFormat, classData, dataMsg->classData, 0, classTotal);
  }
  else
    dataMsg->classData = NULL;
  
  /* Include the pointer to the original data. */
  dataMsg->dataStruct = (const char *)msgData;
  dataMsg->dataByteOrder = BYTE_ORDER;
  dataMsg->classByteOrder = BYTE_ORDER;
  dataMsg->alignment = (ALIGNMENT_TYPE)IPC_ALIGN;
  
  return dataMsg;
}

void x_ipc_encodeMsgData(CONST_FORMAT_PTR Format, const void *DataStruct,
		   DATA_MSG_PTR dataMsg, int32 BStart)
{
  dataMsg->dataStruct = (const char *)DataStruct;
  
  if (Format == NULL) {
    dataMsg->msgData = NULL;
  } else {
    BOOLEAN isLisp;
    LOCK_M_MUTEX;
    isLisp = IS_LISP_MODULE();
    UNLOCK_M_MUTEX;
    if (!isLisp && x_ipc_sameFixedSizeDataBuffer(Format)) {
      dataMsg->msgData = (char *)DataStruct;
    } else {
      dataMsg->msgData = (char *)x_ipcMalloc((unsigned)dataMsg->msgTotal);
      x_ipc_encodeData(Format, DataStruct, dataMsg->msgData, BStart, 
		       dataMsg->msgTotal);
    }
  }
  dataMsg->vec = x_ipc_createVectorization(Format,
				     (const char *)DataStruct,dataMsg->msgData,
				     dataMsg->msgTotal);
}

/*************************************************************/

void *x_ipc_decodeMsgData(CONST_FORMAT_PTR Format,  DATA_MSG_PTR dataMsg,
		    BOOLEAN keepData)
{
  char *DataStruct=NULL;
#if (!defined(LISP))
  if (x_ipc_sameFixedSizeDataBuffer(Format)) {
    dataMsg->dataStruct = dataMsg->msgData;
    DataStruct = dataMsg->msgData;
    if (dataMsg->dataByteOrder == BYTE_ORDER)
      /* This is now a no op */
      return ((void *)dataMsg->msgData);
  }
#endif
  /* Have to keep it anyway, probably for sending after printing.*/
  if (keepData) {
    DataStruct = NULL;
    dataMsg->dataStruct = NULL;
  }
  
  return x_ipc_decodeData(Format, dataMsg->msgData, 0, DataStruct,
		    dataMsg->dataByteOrder, dataMsg->alignment, 
		    dataMsg->msgTotal);
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipc_dataMsgDecodeMsg(msgFormat, dataMsg)
 *
 * DESCRIPTION:
 * Returns the message data defined by msgFormat from the information
 * encoded in dataMsg. If there is no data to decode, x_ipc_dataMsgDecodeMsg
 * returns NULL.
 *
 * INPUTS:
 * CONST_FORMAT_PTR msgFormat;
 * DATA_MSG_PTR dataMsg;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

void *x_ipc_dataMsgDecodeMsg(CONST_FORMAT_PTR msgFormat, DATA_MSG_PTR dataMsg,
		       BOOLEAN keepData)
{
  if (!msgFormat || (msgFormat == BAD_FORMAT) || !dataMsg->msgTotal)
    return NULL;
  else
    return x_ipc_decodeMsgData(msgFormat, dataMsg, keepData);
}


/******************************************************************************
 *
 * FUNCTION: char *x_ipc_dataMsgDecodeClass(classFormat, dataMsg)
 *
 * DESCRIPTION:
 * Returns the message data defined by classFormat from the information
 * encoded in dataMsg. If there is no data to decode, x_ipc_dataMsgDecodeMsg
 * returns NULL.
 *
 * INPUTS:
 * CONST_FORMAT_PTR classFormat;
 * DATA_MSG_PTR dataMsg;
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

void *x_ipc_dataMsgDecodeClass(CONST_FORMAT_PTR classFormat, DATA_MSG_PTR dataMsg)
{
  if (!classFormat || !dataMsg->classTotal)
    return NULL;
  else
    return x_ipc_decodeData(classFormat, dataMsg->classData, 0, NULL,
		      dataMsg->classByteOrder, dataMsg->alignment,
		      dataMsg->classTotal);
}


/******************************************************************************
 *
 * FUNCTION: int32 x_ipc_dataMsgTestMsgData(dataMsg)
 *
 * DESCRIPTION:
 * Returns TRUE if the dataMsg number of message bytes is greater than zero.
 * Returns FALSE otherwise or if dataMsg is NULL.
 *
 * INPUTS: DATA_MSG_PTR dataMsg
 *
 * OUTPUTS: int32.
 *
 *****************************************************************************/

BOOLEAN x_ipc_dataMsgTestMsgData(DATA_MSG_PTR dataMsg)
{
  if (dataMsg && dataMsg->msgTotal > 0)
    return TRUE;
  else
    return FALSE;
}


/******************************************************************************
 *
 * FUNCTION: DATA_MSG_PTR x_ipc_dataMsgReplaceClassData(format, data, dataMsg)
 *
 * DESCRIPTION:
 * Replaces class information on an existing dataMsg with data.
 *
 * INPUTS:
 * CONST_FORMAT_PTR format;
 * char *data;
 * DATA_MSG_PTR dataMsg;
 *
 * OUTPUTS: DATA_MSG_PTR
 *
 *****************************************************************************/

DATA_MSG_PTR x_ipc_dataMsgReplaceClassData(CONST_FORMAT_PTR classFormat,
				     void *data,
				     DATA_MSG_PTR dataMsg,
				     CONST_FORMAT_PTR msgFormat)
{
#ifdef UNUSED_PRAGMA
#pragma unused(msgFormat)
#endif
  DATA_MSG_PTR newDataMsg;
  int32 classTotal=0;
  
  if (data && classFormat)
    classTotal = x_ipc_bufferSize(classFormat, data);
  
  newDataMsg = x_ipc_dataMsgAlloc(sizeof(DATA_MSG_TYPE)+classTotal);
  
  /* 3-Sep-90: fedor: this should work because class info is in the
     dataMsg struture as the last item. */
  
  BCOPY((char *)dataMsg, (char *)newDataMsg, sizeof(DATA_MSG_TYPE));
  
  if (classTotal) {
    newDataMsg->classData = ((char *)newDataMsg + sizeof(DATA_MSG_TYPE));
    x_ipc_encodeData(classFormat, data, newDataMsg->classData, 0, classTotal);
    newDataMsg->classByteOrder = BYTE_ORDER;
  }
  else
    newDataMsg->classData = NULL;
  
  /* reset msgData pointer */
  newDataMsg->msgData = dataMsg->msgData;
  newDataMsg->dataStruct = dataMsg->dataStruct;
  dataMsg->msgData = NULL;
  
  /* Need to copy the vector data. */
  if (dataMsg->vec != NULL)
    newDataMsg->vec = x_ipc_copyVectorization(dataMsg->vec,0);
  else {
    X_IPC_MOD_ERROR("Internal Error: Missing data Vector\n");
    return NULL;
  }
  
  /* set refCount */
  newDataMsg->dataRefCountPtr = dataMsg->dataRefCountPtr;
  if ( newDataMsg->dataRefCountPtr != NULL)
    (*(newDataMsg->dataRefCountPtr))++;
  newDataMsg->refCount = 0;
  
  newDataMsg->classTotal = classTotal;
  
  if (x_ipc_dataMsgFree(dataMsg) != NULL)
    dataMsg->msgData = newDataMsg->msgData;
  
  return newDataMsg;
}


/******************************************************************************
 *
 * FUNCTION: void x_ipc_dataMsgDisplayStats(stream)
 *
 * DESCRIPTION:
 * Display datamsg memory usage stats.
 *
 * INPUTS: none.
 *
 * OUTPUTS: none.
 *
 *****************************************************************************/

#if 0
static int32 countDataMessages(int32 allocatedP, DATA_MSG_BUF_PTR dataMsgBuf)
{
  if (dataMsgBuf->allocated == allocatedP) {
    LOCK_M_MUTEX;
    GET_M_GLOBAL(numAllocatedDM)++;
    GET_M_GLOBAL(sizeDM) += dataMsgBuf->size;
    UNLOCK_M_MUTEX;
  }
  return TRUE;
}
#endif

void x_ipc_dataMsgDisplayStats(FILE *stream)
{
  LOCK_M_MUTEX;
  fprintf(stream,"Data Msg Buffer Stats:\n");
  fprintf(stream,"  Total Alloc  : %d\n", GET_M_GLOBAL(DMTotal));
  fprintf(stream,"  Total Freed  : %d\n", GET_M_GLOBAL(DMFree));
  fprintf(stream,"  Min Request  : %d\n", GET_M_GLOBAL(DMmin));
  fprintf(stream,"  Max Request  : %d\n", GET_M_GLOBAL(DMmax));

#if 0
  fprintf(stream,"Buf List Size: %d\n",
	  x_ipc_listLength(GET_M_GLOBAL(dataMsgBufferList)));
  
  GET_M_GLOBAL(numAllocatedDM) = GET_M_GLOBAL(sizeDM) = 0;
  x_ipc_listIterateFromFirst((LIST_ITER_FN)countDataMessages, (char *)TRUE,
		       (GET_M_GLOBAL(dataMsgBufferList)));
  fprintf(stream,"  %d allocated, with %d total bytes\n",
	  GET_M_GLOBAL(numAllocatedDM),
	  GET_M_GLOBAL(sizeDM));
  GET_M_GLOBAL(numAllocatedDM) = GET_M_GLOBAL(sizeDM) = 0;
  x_ipc_listIterateFromFirst((LIST_ITER_FN)countDataMessages, FALSE,
		       (GET_M_GLOBAL(dataMsgBufferList)));
  fprintf(stream,"  %d deallocated, with %d total bytes\n",
	  GET_M_GLOBAL(numAllocatedDM),
	  GET_M_GLOBAL(sizeDM));
#endif  
  UNLOCK_M_MUTEX;
  FLUSH_IF_NEEDED(stream);
}


/******************************************************************************
 *
 * FUNCTION: X_IPC_RETURN_VALUE_TYPE x_ipcAllocateDataBuffer()
 *
 * DESCRIPTION:
 * Pre-allocate a data message buffer of the requested size
 *
 * INPUTS: int32 size -- number of bytes to pre-allocate.
 *
 * OUTPUTS: Returns "Success" if the buffer could be allocated,
 *          "Failure" otherwise
 *
 * NOTES: Currently, since "dataMsgBufferCreate" has no return value,
 *        "x_ipcAllocateDataBuffer" always returns Success (or bombs
 *        out within "dataMsgBufferCreate").
 *****************************************************************************/

#if 0
/* no longer used */
static X_IPC_RETURN_VALUE_TYPE x_ipcAllocateDataBuffer(int32 size)
{
  DATA_MSG_BUF_PTR dataMsgBuf;
  
  /* Creates an unallocated data message buffer */
  dataMsgBuf = dataMsgBufferCreate(size);
  LOCK_M_MUTEX;
  x_ipc_listInsertItem((char *)dataMsgBuf, (GET_M_GLOBAL(dataMsgBufferList)));
  
  if (size > (GET_M_GLOBAL(DMmax))) (GET_M_GLOBAL(DMmax)) = size;
  if (size < (GET_M_GLOBAL(DMmin))) (GET_M_GLOBAL(DMmin)) = size;
  UNLOCK_M_MUTEX;
  
  return Success;
}
#endif

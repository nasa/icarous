/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: comModule.c
 *
 * ABSTRACT:
 * 
 * Module Communications
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/com.h,v $ 
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:55 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: com.h,v $
 * Revision 2.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2003/07/23 20:25:04  reids
 * Fixed bug in handling message that is received but already unsubscribed.
 * Removed compiler warning.
 *
 * Revision 2.2  2000/07/03 17:03:21  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.3  1997/03/07 17:49:29  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.1.2.2  1996/10/18 17:59:28  reids
 * Update to support port to Windows95.
 *
 * Revision 1.1.2.1  1996/10/15 20:55:34  reids
 * Changed x_ipc_timeInMsecs from long to unsigned long.
 *
 * Revision 1.1  1996/05/09 01:01:15  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/04/24 19:10:59  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.1  1996/03/03 04:31:05  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.7  1996/02/21  18:30:05  rich
 * Created single event loop.
 *
 * Revision 1.6  1996/02/06  19:04:15  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.5  1995/06/05  23:58:40  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.4  1995/05/31  19:35:07  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.3  1995/04/09  20:30:05  rich
 * Added /usr/local/include and /usr/local/lib to the paths for compiling
 * for sunOS machines. (Support for new vendor OS).
 * Create a x_ipc directory in /tmp and put the socket in there so others can
 * delete dead sockets.  The /tmp directory has the sticky bit set so you
 * can't delete files even if you have write permission on the directory.
 * Fixes to libc.h to use the new declarations in the gcc header files and
 * avoid problems with dbmalloc.
 *
 * Revision 1.2  1995/04/07  05:02:53  rich
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
 * Revision 1.1  1995/04/04  19:53:06  rich
 * Forgot com.h and com.c.
 *
 *
 ****************************************************************/

#ifndef INCcom
#define INCcom

/* #define TCP 6 is really IPPROTO_TCP*/
#ifdef applec
#define IPPROTO_TCP 0
#endif /* applec */

#define BACKLOG 5
#define PROTOCOL_NAME "tcp"
#define HOST_NAME_SIZE   32

#define TCP_SOCKET_BUFFER (16*1024)
#define UNIX_SOCKET_BUFFER (32*1024)

/* The full name must be less than 14 characters. */
#ifdef macintosh
#define UNIX_DIR_NAME "::IPC Socket:"
#define UNIX_SOCKET_NAME UNIX_DIR_NAME "%d"
#else
#define UNIX_DIR_NAME "/tmp/x_ipc/"
#define UNIX_SOCKET_NAME UNIX_DIR_NAME "%d"
#endif
#define VX_PIPE_NAME "/pipe/x_ipc/%s_%s"
#define VX_PIPE_BUFFER (32*1024)
/*#define VX_PIPE_BUFFER (1024*1024)*/
#define VX_PIPE_NUM_BUF 6

#ifdef OS2
#define MAX_SOCKET_PACKET (32*1024-100)
#else
#define MAX_SOCKET_PACKET UNIX_SOCKET_BUFFER
#endif

BOOLEAN x_ipc_connectAtPort(const char *machine, int32 port,
		      int *readSd, int *writeSd);
BOOLEAN x_ipc_connectAtSocket(const char *machine, int32 port,
			int *readSd, int *writeSd);
BOOLEAN x_ipc_connectAt(const char *machine, int32 port,
		  int *readSd, int *writeSd);
BOOLEAN x_ipc_connectSocket(const char *servHost, int *readSd, int *writeSd);
BOOLEAN x_ipc_listenAtPort(int32 *port, int *sd);
BOOLEAN x_ipc_listenAtSocket(int32 port, int *sd);
#if defined(_WINSOCK_) & defined(WIN95)
void x_ipc_closeSocket(int port);
#else
void x_ipc_closeSocket(int32 port);
#endif

/*****************************************************************
 * Time-based functions
 *****************************************************************/

#define MSECS_TO_TIME(msecs, timeval) \
  { (timeval).tv_sec = (msecs)/1000; (timeval).tv_usec = 1000*((msecs)%1000); }

/* Have to truncate seconds to prevent overflow */
#define TIME_TO_MSECS(timeval, msecs) \
  (msecs) = ((timeval).tv_sec == WAITFOREVER ? WAITFOREVER \
	     : (1000*(0x1FFFFF&(timeval).tv_sec) + (timeval).tv_usec/1000))

#if defined(VXWORKS) | defined(_WINSOCK_)
/* Not defined in many environments */ 
extern struct timeval *gettimeofday(struct timeval *, void *);
#endif
extern unsigned long x_ipc_timeInMsecs (void);

/* Needed to initialize GUSI (provides Unix-type sockets on Macs) */
#if defined(macintosh)
void initGUSI(void);
#endif

#endif /* INCcom */

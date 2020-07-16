/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: communications
 *
 * FILE: com.c
 *
 * ABSTRACT:
 * 
 * Module Communications
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY 
 *
 * $Log: com.c,v $
 * Revision 2.10  2009/05/04 19:03:41  reids
 * Changed to using snprintf to avoid corrupting the stack on overflow
 *
 * Revision 2.9  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.8  2003/02/13 20:41:10  reids
 * Fixed compiler warnings.
 *
 * Revision 2.7  2002/06/25 16:45:25  reids
 * Added casts to satisfy compiler.
 *
 * Revision 2.6  2002/01/03 20:52:10  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2000/08/14 21:28:33  reids
 * Added support for making under Windows.
 *
 * Revision 2.4  2000/07/19 20:54:04  reids
 * Took out extraneous warning message
 *
 * Revision 2.3  2000/07/03 17:06:09  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/05/30 17:10:12  reids
 * Found a long-standing bug that, amazingly, never bit us before: was using
 *   sizeof(sockaddr_in) instead of sizeof(sockaddr_un) in listenAtSocket.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.6  1997/03/07 17:49:27  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.1.2.5  1996/12/18 15:09:59  reids
 * Changed logging code to remove VxWorks dependence on varargs
 * Defined common macros to clean up code
 *
 * Revision 1.1.2.4  1996/11/05 15:30:03  reids
 * Added the missing "index" function to Solaris version.
 *
 * Revision 1.1.2.3  1996/10/29 14:53:15  reids
 * Don't redefine index for Sparcs.
 *
 * Revision 1.1.2.2  1996/10/22 18:09:22  reids
 * Removed debugging print statements.
 *
 * Revision 1.1.2.1  1996/10/15 20:55:32  reids
 * Changed x_ipc_timeInMsecs from long to unsigned long.
 *
 * Revision 1.1  1996/05/09 01:01:15  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/04/24 19:10:59  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.2  1996/03/19 03:38:34  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.1  1996/03/03 04:31:03  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.20  1996/07/23  19:32:19  reids
 * Changes to support Windows 95.
 *
 * Revision 1.19  1996/06/30  20:17:25  reids
 * Handling of polling monitors was severely broken.
 *
 * Revision 1.18  1996/05/09  18:30:26  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.17  1996/03/15  21:15:46  reids
 * Fixed a problem with null-terminated strings when using the "host:port"
 *   server-host option.
 *
 * Revision 1.16  1996/02/21  18:30:04  rich
 * Created single event loop.
 *
 * Revision 1.15  1996/02/12  00:53:53  rich
 * Get VX works compile to work with GNUmakefiles.
 *
 * Revision 1.14  1996/02/10  16:49:33  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.13  1996/02/07  00:27:35  rich
 * Add prefix to VERSION_DATE and COMMIT_DATE.
 *
 * Revision 1.12  1996/02/06  19:04:13  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.11  1996/01/27  21:52:57  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.10  1996/01/08  18:16:52  rich
 * Changes for vxWorks 5.2.
 *
 * Revision 1.9  1996/01/05  16:31:02  rich
 * Added windows NT port.
 *
 * Revision 1.8  1995/07/19  14:25:49  rich
 * Added display and dump to the central interface.
 * Fixed problem with direct querries not returning to the correct module.
 * Added Argv versions of provides and requires.
 *
 * Revision 1.7  1995/07/10  16:16:50  rich
 * Interm save.
 *
 * Revision 1.6  1995/07/08  17:50:54  rich
 * Linux Changes.  Also added GNUmakefile.defs.
 *
 * Revision 1.5  1995/07/06  21:15:44  rich
 * Solaris and Linux changes.
 *
 * Revision 1.4  1995/06/05  23:58:38  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.3  1995/04/09  20:30:03  rich
 * Added /usr/local/include and /usr/local/lib to the paths for compiling
 * for sunOS machines. (Support for new vendor OS).
 * Create a x_ipc directory in /tmp and put the socket in there so others can
 * delete dead sockets.  The /tmp directory has the sticky bit set so you
 * can't delete files even if you have write permission on the directory.
 * Fixes to libc.h to use the new declarations in the gcc header files and
 * avoid problems with dbmalloc.
 *
 * Revision 1.2  1995/04/07  05:02:50  rich
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
 * Revision 1.1  1995/04/04  19:53:04  rich
 * Forgot com.h and com.c.
 *
 *
 * $Revision: 2.10 $
 * $Date: 2009/05/04 19:03:41 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

/******************************************************************************
 *
 * FUNCTION: BOOLEAN x_ipc_connectAtPort(const char *machine, int port, int *sd)
 *
 * DESCRIPTION: Connect at the TCP/IP connection.
 *
 * INPUTS: none.
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

BOOLEAN x_ipc_connectAtPort(const char *machine, int32 port,
			    int *readSd, int *writeSd)
{
#ifndef VXWORKS
  struct hostent *hp;
#endif
  struct sockaddr_in server;
  
  bzero((char *)&server, sizeof(struct sockaddr_in));
  
  server.sin_family = AF_INET;
  server.sin_port = htons((u_short)port);

  if (isdigit((int)machine[0])) {
    /* printf("digit! \n");*/
#ifdef macintosh
    server.sin_addr = inet_addr(machine);
    // x_ipcModError("Currently cannot use IP address for Mac");
#else
    server.sin_addr.s_addr = inet_addr(machine);
#endif
  } else {
#ifndef VXWORKS
    if ((hp = gethostbyname((char *)machine)) == NULL) {
      *readSd = NO_FD;
      *writeSd = NO_FD;
      return FALSE;
    }
    BCOPY((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
#else
    if ((server.sin_addr.s_addr = hostGetByName((char *)machine)) == ERROR) {
      *readSd = NO_FD;
      *writeSd = NO_FD;
      return FALSE;
    }
#endif
  }
  
  if ((*readSd = *writeSd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    *readSd = NO_FD;
    *writeSd = NO_FD;
    return FALSE;
  }
  
  /******* 17-aug-91: fedor: causes NeXT machine to fail connecting **
    if ((setsockopt(*sd, SOL_SOCKET, SO_REUSEADDR, (char *)0, 0)) < 0)
    return FALSE;
    *****/
  
  /******
    item = 0;
    if ((setsockopt(*sd, SOL_SOCKET, SO_LINGER, &item, sizeof(int))) < 0)
    return FALSE;
    *****/
  
#if !defined(macintosh) && !(defined(__TURBOC__) || defined(_WINSOCK_))
  { int item = 1;
    if ((setsockopt(*readSd, SOL_SOCKET, SO_REUSEADDR,
		    (char *)&item, sizeof(int))) < 0) {
      close(*readSd);
      *readSd = NO_FD;
      return FALSE;
    }
  }
#endif /* macintosh */
  
#if !defined(macintosh)
  { int value = TCP_SOCKET_BUFFER;
    if ((setsockopt(*readSd, IPPROTO_TCP, TCP_NODELAY,
		    (char *)&value, sizeof(int))) < 0) {
      close(*readSd);
      if (*readSd != *writeSd) {
	close(*writeSd);
      }
      *readSd = NO_FD;
      *writeSd = NO_FD;
      return FALSE;
    }
    
    setsockopt(*writeSd, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(int));
    setsockopt(*readSd, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof(int));
    
  }
#endif /* macintosh */
  
  /* 24-Aug-90: fedor: VxWorks sockaddr_in is defined differently
     this should be checked to make a VxWorks version. */
  if (connect(*readSd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    CLOSE_SOCKET(*readSd);
    if (*readSd != *writeSd) {
      CLOSE_SOCKET(*writeSd);
    }
    *readSd = NO_FD;
    *writeSd = NO_FD;
    return FALSE;
  }
  return TRUE;
}

/******************************************************************************
 *
 * FUNCTION: BOOLEAN x_ipc_connectAtSocket(const char *machine, int port, int *sd)
 *
 * DESCRIPTION: Connect at the unix socket.
 *
 * INPUTS: none.
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

#ifndef VXWORKS
BOOLEAN x_ipc_connectAtSocket(const char *machine, int32 port,
			int *readSd, int *writeSd)
{
#ifdef macintosh
#pragma unused(machine, port, readSd, writeSd)
// Don't use Unix sockets on Mac -- seems to be incredibly slow!
  return FALSE;
#else
  int value;
  struct sockaddr_un unix_server;
  
  bzero((char *)&unix_server, sizeof(struct sockaddr_un));
  unix_server.sun_family = AF_UNIX;
  snprintf(unix_server.sun_path, sizeof(unix_server.sun_path)-1,
	   UNIX_SOCKET_NAME, port);
  
  if ((*writeSd = *readSd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    return FALSE;
  value = UNIX_SOCKET_BUFFER;
  setsockopt(*writeSd, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(int));
  setsockopt(*readSd, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof(int));
  
  /* 24-Aug-90: fedor: VxWorks sockaddr_in is defined differently
   * this should be checked to make a VxWorks version.
   */
  if (connect(*readSd, (struct sockaddr *)&unix_server, sizeof(unix_server)) < 0) {
    CLOSE_SOCKET(*readSd);
    if (*readSd != *writeSd) {
      CLOSE_SOCKET(*writeSd);
    }
    return FALSE;
  }
  
  return TRUE;
#endif
}

#else

/* Use VX pipes. */
BOOLEAN x_ipc_connectAtSocket(const char *machine, int32 port,
			int *readSd, int *writeSd)
{
  struct timeval time;
  int ret;
  int acceptSd;
  char socketName[80];
  char portNum[80];
  fd_set readMask; 
  
  LOCK_IO_MUTEX;
  bzero(portNum, sizeof(portNum));
  bzero(socketName, sizeof(socketName));
  snprintf(socketName, sizeof(socketName)-1, UNIX_SOCKET_NAME, port);
  acceptSd = open(socketName, O_RDWR, 0644);
  
  snprintf(portNum, sizeof(portNum)-1, "%d", port);
  LOCK_M_MUTEX;
  snprintf(socketName, sizeof(socketName)-1, VX_PIPE_NAME, portNum,
	   GET_M_GLOBAL(modNameGlobal));
  UNLOCK_M_MUTEX;
  pipeDevCreate(socketName, VX_PIPE_NUM_BUF, VX_PIPE_BUFFER);
  *writeSd = open(socketName, O_WRONLY, 0644);
  if (*writeSd <= 0) {
    UNLOCK_IO_MUTEX;
    X_IPC_MOD_WARNING("Open Failed\n");
    return FALSE;
  }
  
  LOCK_M_MUTEX;
  snprintf(socketName, sizeof(socketName)-1, VX_PIPE_NAME,
	   GET_M_GLOBAL(modNameGlobal),portNum);
  UNLOCK_M_MUTEX;
  pipeDevCreate(socketName, VX_PIPE_NUM_BUF, VX_PIPE_BUFFER);
  *readSd = open(socketName, O_RDONLY, 0644);
  if (*readSd <= 0) {
    UNLOCK_IO_MUTEX;
    X_IPC_MOD_WARNING("Open Failed\n");
    return FALSE;
  }
  
  LOCK_M_MUTEX;
  snprintf(portNum, sizeof(portNum)-1, "%s",  GET_M_GLOBAL(modNameGlobal));
  UNLOCK_M_MUTEX;
  x_ipc_writeNBytes(acceptSd, portNum, 80);
  
  FD_ZERO(&readMask);
  FD_SET(*readSd, &readMask);
  
  time.tv_usec = 0;
  time.tv_sec = 20;
  do {
    ret = select(FD_SETSIZE, &readMask, (fd_set *)NULL,(fd_set *)NULL, &time);
  }
  while (ret < 0 && errno == EINTR);
  if (ret < 1) {
    UNLOCK_IO_MUTEX;
    return FALSE;
  }

  x_ipc_readNBytes(*readSd,socketName, 80);
  close(acceptSd);
  
  UNLOCK_IO_MUTEX;
  return TRUE;
}
#endif

/******************************************************************************
 *
 * FUNCTION: Bolean x_ipc_connectAt(const char *machine, int port, int *sd)
 *
 * DESCRIPTION: Connect at the unix socket if possible, otherwise, at the 
 *              TCP/IP connection.
 *
 * INPUTS: none.
 *
 * OUTPUTS: char *
 *
 *****************************************************************************/

BOOLEAN x_ipc_connectAt(const char *machine, int32 port,
			int *readSd, int *writeSd)
{
  char thisHost[HOST_NAME_SIZE+1];
  
  bzero(&(thisHost[0]),HOST_NAME_SIZE+1);
  gethostname(thisHost, HOST_NAME_SIZE);
#if !defined(NO_UNIX_SOCKETS) || defined(VX_PIPES)
  /* If the names are the same, then check for the socket. */
  if ((strncmp(thisHost,machine,strlen(machine)) == 0) &&
      ((strlen(machine) == strlen(thisHost)) ||
       ((strlen(machine) < strlen(thisHost)) &&
	(thisHost[strlen(machine)] == '.'))))
    { /* Can try to connect at the unix socket. */
      if (!x_ipc_connectAtSocket(machine, port, readSd, writeSd)) {
	return(x_ipc_connectAtPort(machine, port, readSd, writeSd));
      } else {
	return TRUE;
      }
  } else {
    /* Have to connect at the TCP/IP socket. */
    return(x_ipc_connectAtPort(machine, port, readSd, writeSd));
  }
#else
  /* Have to connect at the TCP/IP socket. */
  return(x_ipc_connectAtPort(machine, port, readSd, writeSd));
#endif
}

/**********************************************************************
 *
 *  FUNCTION:  BOOLEAN x_ipc_listenAtPort(int port, int *sd)
 *
 *  DESCRIPTION: 
 *
 *		Create a socket for listening for new connections at a given
 * port value. If the value of port is 0, return the value chosen in port.
 *
 * Return 1 on success, else 0.
 *
 * NOTES:
 * 7-Jun-91: fedor: may need some changes for vxworks 
 *
 *********************************************************************/

BOOLEAN x_ipc_listenAtPort(int32 *port, int *sd)
{
  int reuse;
  int32 length;
  struct sockaddr_in server;
  
  bzero((char *)&server, sizeof(struct sockaddr_in));
  
  server.sin_port = htons((unsigned short)*port);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  
  if ((*sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    X_IPC_MOD_WARNING1("socket fail (%d)\n", errno);
    return FALSE;
  }
  
  reuse = 1;
  
#ifndef macintosh
  if ((setsockopt(*sd, SOL_SOCKET, SO_REUSEADDR,
		  (char *)&reuse, sizeof(int))) < 0) {
    X_IPC_MOD_WARNING("setsockopt fail\n");
    return FALSE;
  }
#endif /* macintosh */
  
  reuse = 1;
  
#if !defined(macintosh)
  { int value = TCP_SOCKET_BUFFER;
    if ((setsockopt(*sd, IPPROTO_TCP, TCP_NODELAY,
		    (char *)&value, sizeof(int))) < 0)
      return FALSE;
    setsockopt(*sd, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(int));
    setsockopt(*sd, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof(int));
  }
#endif /* !macintosh */
  
  if ((bind(*sd, (struct sockaddr *)&server, 
	    sizeof(struct sockaddr_in))) < 0) {
    return FALSE;
  }
  
  if (listen(*sd, BACKLOG) < 0) {
    X_IPC_MOD_WARNING( "listen fail\n");
    return FALSE;
  }
  
  /* find what port is if not set */
  if (!*port) {
    length = sizeof(server);
    if (getsockname(*sd, (struct sockaddr *)&server,
		    (socklen_t *)&length) < 0) {
      X_IPC_MOD_WARNING( "getsockname fail\n");
      return FALSE;
    }
    
    *port = ntohs(server.sin_port);
  }
  
  return TRUE;
}

#if !(defined(NO_UNIX_SOCKETS) || defined(VX_PIPES))
/**********************************************************************
 *
 *  FUNCTION:  BOOLEAN x_ipc_listenAtSocket(int32 port, int *sd)
 *
 *  DESCRIPTION: 
 *
 *		Create a socket for listening for new connections at a given
 * port value. If the value of port is 0, return the value chosen in port.
 *
 * Return 1 on success, else 0.
 *
 * NOTES:
 * 7-Jun-91: fedor: may need some changes for vxworks 
 *
 *********************************************************************/

BOOLEAN x_ipc_listenAtSocket(int32 port, int *sd)
{
  int result;
#ifndef macintosh
  int oldUmask;
#endif  
  struct sockaddr_un unix_server;
  
  bzero((char *)&unix_server, sizeof(struct sockaddr_un));
  unix_server.sun_family = AF_UNIX;
  snprintf(unix_server.sun_path, sizeof(unix_server.sun_path)-1,
	   UNIX_SOCKET_NAME, port);
  
  /* Get rid of old links. */
  result = unlink(unix_server.sun_path);
  
  if (((result) != 0) && ((errno) != ENOENT))
    X_IPC_MOD_WARNING1("Old socket %s could not be deleted. \n Is server still running?\n",
		  unix_server.sun_path);

#ifndef macintosh  
  oldUmask = umask(!(S_IRUSR | S_IWUSR | S_IXUSR |
		     S_IRGRP | S_IWGRP | S_IXGRP |
		     S_IROTH | S_IWOTH | S_IXOTH));
#endif
  
  /* Create directory. */
#if defined(VXWORKS) || defined(OS2) || defined(macintosh)
  result = mkdir(UNIX_DIR_NAME);
#else
  result = mkdir(UNIX_DIR_NAME,
		 (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP |
		  S_IROTH | S_IWOTH | S_IXOTH));
#endif
  if (((result) != 0) && ((errno) != EEXIST))
    X_IPC_MOD_WARNING1("Can not create directory (%s)to hold socket connections.\n",
		  UNIX_DIR_NAME);
  
  if ((*sd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    X_IPC_MOD_WARNING("socket fail\n");
    return FALSE;
  }
  
  /* Change the mode of the socket so it can be deleted by others if needed.
   * This is safe since we alway try to do an tcp/ip create first and only
   * try and unlink the socket if the tcp/ip connection succeeds.
   */
#ifndef macintosh
  fchmod(*sd,(S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP |
	      S_IROTH | S_IWOTH | S_IXOTH));
  oldUmask = umask(oldUmask);
#endif
  
#ifndef macintosh
  { int value = UNIX_SOCKET_BUFFER;
    setsockopt(*sd, SOL_SOCKET, SO_SNDBUF, (char *)&value, sizeof(int));
    setsockopt(*sd, SOL_SOCKET, SO_RCVBUF, (char *)&value, sizeof(int));
  }
#endif /* !macintosh */
  
  if ((bind(*sd, (struct sockaddr *)&unix_server,
#ifndef macintosh
	    sizeof(struct sockaddr_un))) < 0)
#else /* macintosh */ /* For GUSI, the last argument is 2+|path name| */
	    strlen(unix_server.sun_path)+2)) < 0)
#endif
    {
#if defined(VXWORKS)
      printErr("bind fail\n");
/*      printErrno(errno); */
#endif
      return FALSE;
    }
  
  if (listen(*sd, BACKLOG) < 0) {
    X_IPC_MOD_WARNING( "listen fail\n");
    return FALSE;
  }
  
  return TRUE;
}

/**********************************************************************
 *
 *  FUNCTION:  BOOLEAN x_ipc_closeSocket(int port)
 *
 *  DESCRIPTION: 
 *
 *		Create a socket for listening for new connections at a given
 * port value. If the value of port is 0, return the value chosen in port.
 *
 * Return 1 on success, else 0.
 *
 * NOTES:
 * 7-Jun-91: fedor: may need some changes for vxworks 
 *
 *********************************************************************/

void x_ipc_closeSocket(int port)
{
  struct sockaddr_un unix_server;
  
  bzero((char *)&unix_server, sizeof(struct sockaddr_un));
  unix_server.sun_family = AF_UNIX;
  snprintf(unix_server.sun_path, sizeof(unix_server.sun_path)-1,
	   UNIX_SOCKET_NAME, port);
  
  /* Get rid of old links. */
  unlink(unix_server.sun_path);
}

#elif defined(VX_PIPES)

/**********************************************************************
 *
 *  FUNCTION:  BOOLEAN x_ipc_listenAtSocket(int32 port, int *sd)
 *
 *  DESCRIPTION: 
 *		Uses VxWorks pipes instead of unix sockets.
 *
 * Return 1 on success, else 0.
 *
 *
 *********************************************************************/

BOOLEAN x_ipc_listenAtSocket(int32 port, int *sd)
{
  char socketName[80];
  
  /* Initialize the use of vx pipes. */
  /* X_IPC_MOD_WARNING("pipeDrv\n");*/
  /* pipeDrv();*/
  
  bzero(socketName, sizeof(socketName));
  snprintf(socketName, sizeof(socketName)-1, UNIX_SOCKET_NAME, port);
#ifdef DEBUG
  X_IPC_MOD_WARNING1("pipeDevCreate %s, 3, 80\n", socketName);
#endif
  pipeDevCreate(socketName, 3, 80);
  
#ifdef DEBUG
  X_IPC_MOD_WARNING1("Opening %s\n", socketName);
#endif
  *sd = open(socketName, O_RDWR, 0644);
  if (*sd <= 0) {
    X_IPC_MOD_WARNING("Open Failed\n");
    return FALSE;
  }
  return TRUE;
}

/**********************************************************************
 *
 *  FUNCTION:  BOOLEAN x_ipc_closeSocket(int port)
 *
 *  DESCRIPTION: 
 *		Uses VxWorks pipes instead of unix sockets.
 *
 * Return 1 on success, else 0.
 *
 *********************************************************************/

void x_ipc_closeSocket(int port)
{
  close(port);
}
#else
BOOLEAN x_ipc_listenAtSocket(int32 port, int *sd)
{
  return FALSE;
}

void x_ipc_closeSocket(int32 port)
{
  return;
}
#endif /* NO_UNIX_SOCKETS */

/***************************************************************************/



/******************************************************************************
 *
 * FUNCTION int x_ipc_connectSocket(servHost, sd)
 *
 * DESCRIPTION: 
 * Returns TRUE if a connection was successfully opened to servHost, 
 * FALSE otherwise. sd is set to the value of the opened connection.
 *
 * INPUTS: 
 * char *servHost;
 * int *sd;
 *
 * OUTPUTS: int (TRUE or FALSE)
 *
 *****************************************************************************/

#if defined(__TURBOC__) || defined(SYSV) || defined(_WIN95_MSC_) || defined(WIN32)
char *index(const char *str, char find)
{
  char *returnVal;

  for(returnVal =(char *)str; (*returnVal != find); returnVal++)
    if(*returnVal == '\0') return NULL;
  return returnVal;
}
#endif

BOOLEAN x_ipc_connectSocket(const char *servHost, int *readSd, int *writeSd)
{ 
  char serverHostName[80], *colon;
  int serverPort;
  
  colon = index(servHost,':');
  if(colon != NULL) {
    /* servHost is <hostName>:<port> */
    serverPort = atoi(colon+1);
    /* Copy that portion of the name which is the real host name */
    bzero(serverHostName, sizeof(serverHostName));
    strncpy(serverHostName, servHost, strlen(servHost)-strlen(colon));
    return x_ipc_connectAt(serverHostName, serverPort, readSd, writeSd);
  } else {
    return x_ipc_connectAt(servHost, SERVER_PORT, readSd, writeSd);
  }
}

/*****************************************************************
 * Time-based functions
 *****************************************************************/

#if defined(macintosh)
/* Not defined in Mac environment */
unsigned long x_ipc_timeInMsecs (void)
{
  return (clock()%CLOCKS_PER_SEC)*1000/CLOCKS_PER_SEC;
}

struct timeval *gettimeofday(struct timeval *daytime, void *dummy)
{
#pragma unused(dummy)
  clock_t time;
  
  time = clock();
  daytime->tv_usec = (time%CLOCKS_PER_SEC)*1000000/CLOCKS_PER_SEC;
  daytime->tv_sec = time/CLOCKS_PER_SEC;
  return daytime;
}

#elif defined(VXWORKS)
/* Not defined in vxworks environment */
struct timeval *gettimeofday(struct timeval *daytime, void *dummy)
{
  ULONG time;
  register int ticksPerSec = sysClkRateGet();

  time = tickGet();
  daytime->tv_usec = (time%ticksPerSec)*1000000/ticksPerSec;
  daytime->tv_sec = time/ticksPerSec;
  return daytime;
}

unsigned long x_ipc_timeInMsecs (void)
{
  return tickGet()*1000/sysClkRateGet();
}
#elif defined(_WINSOCK_)
static SYSTEMTIME theTime;

unsigned long x_ipc_timeInMsecs (void)
{
  GetSystemTime(&theTime);
  return (theTime.wMilliseconds + 1000*theTime.wSecond);
}

/* Not defined in Windows environment */
struct timeval *gettimeofday(struct timeval *daytime, void *dummy)
{
#ifndef WIN32
#pragma unused(dummy)
#endif
  clock_t theTime;
  
  theTime = clock();
  daytime->tv_usec = (int)theTime % (int)CLK_TCK * 1000000 / (int)CLK_TCK;
  daytime->tv_sec  = (int)theTime / (int)CLK_TCK;
  return (daytime);
}
#else
unsigned long x_ipc_timeInMsecs (void)
{
  struct timeval timeBlock;
  unsigned long now;

  gettimeofday(&timeBlock, NULL);
  TIME_TO_MSECS(timeBlock, now);

  return now;
}
#endif

#ifdef macintosh
/* Needed functions that macintosh does not define */

char *strdup(const char *str)
{
  char *newString = x_ipcMalloc(sizeof(char)*(strlen(str)+1));
  strcpy(newString, str);
  return newString;
}

#include <Events.h>
#include <SIOUX.h>
/* The right thing to do is to include GUSI.h, but that 
   includes other files that conflict with some of my declarations */
void GUSIwithInternetSockets();
void GUSIwithUnixSockets();
void GUSIwithSIOUXSockets();
void GUSISetup(void (*socketfamily)());

void initGUSI(void)
{
  GUSISetup(GUSIwithInternetSockets);
  GUSISetup(GUSIwithUnixSockets);
  // Handle stdin, and sets up event handlers
  GUSISetup(GUSIwithSIOUXSockets);
}

#endif

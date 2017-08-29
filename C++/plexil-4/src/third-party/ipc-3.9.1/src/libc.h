/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE:
 *
 * FILE:
 *
 * ABSTRACT:
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/libc.h,v $ 
 * $Revision: 2.13 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 * $Log: libc.h,v $
 * Revision 2.13  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.12  2005/12/30 17:01:44  reids
 * Support for Mac OSX
 *
 * Revision 2.11  2003/04/14 15:31:37  reids
 * Fixed bug in Cygwin version (wrong Endian);
 * Updated for Windows XP
 *
 * Revision 2.10  2003/02/13 20:38:20  reids
 * Updated to work under Solaris.
 *
 * Revision 2.9  2001/07/24 12:13:43  reids
 * Changes to support compiling under RedHat 7.1 (kernel 2.4)
 *
 * Revision 2.8  2001/01/06 02:59:57  reids
 * Still another fix to make it compile under solaris
 *
 * Revision 2.7  2001/01/05 22:43:58  reids
 * Minor changes to enable compiling under Visual C++, Cygwin, & Solaris.
 *
 * Revision 2.6  2000/12/19 22:02:26  reids
 * Fixes by Jeff Mishler for compiling under Windows
 *
 * Revision 2.5  2000/12/11 16:12:17  reids
 * Extended for compilation under CYGWIN and RedHat 6.
 *
 * Revision 2.4  2000/08/14 21:28:34  reids
 * Added support for making under Windows.
 *
 * Revision 2.3  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/01/27 20:51:48  reids
 * Changes for RedHat 6 (and also to remove compiler warnings).
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.9.2.8  1997/03/07 17:49:46  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.9.2.7  1997/01/27 20:09:39  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.9.2.5  1997/01/11 01:21:07  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.9.2.4.6.1  1996/12/24 14:41:43  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.9.2.4  1996/10/29 14:55:41  reids
 * Extern some definitions to avoid compiler warnings.
 *
 * Revision 1.9.2.3  1996/10/25 04:40:15  reids
 * Got rid of several compiler warnings.
 *
 * Revision 1.9.2.2  1996/10/18 18:08:02  reids
 * Added comments to endifs.
 *
 * Revision 1.9.2.1  1996/10/08 14:16:10  reids
 * Include string.h for PPC compilation (removes compiler warnings).
 *
 * Revision 1.9  1996/08/10 00:45:51  kamyar
 * console
 *
 * Revision 1.6.4.2  1996/08/09 18:41:49  kamyar
 * checking for green hills compiler __ghs to avoid duplicate declarations
 *
 * Revision 1.6.4.1  1996/07/31 22:28:07  eldred
 * Restored makefile from tagged S3 directory
 *
 * Revision 1.6  1996/06/20 19:54:43  ebg
 * sunos 5.5 compilation with sparcworks c 4.0 requires ifdef/endif \
 * around strcasecmp
 *
 * Revision 1.5  1996/05/24 20:08:03  rouquett
 * SYSV, __svr4__
 *
 * Revision 1.4  1996/05/24 20:03:16  reids
 * sys/fcntlcom.h does not exist for Solaris 2.x (as per roquett)
 *
 * Revision 1.3  1996/05/24 18:41:26  rouquett
 * The test for SUNOS vs Solaris failed on Solaris
 *
 * Revision 1.2  1996/05/09 18:19:29  reids
 * Changes to support CLISP.
 *
 * Revision 1.1  1996/05/09 01:01:35  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/04/24 19:11:06  reids
 * Support for the vxworks version.  Main changes to the way getting time is
 *   handled and parsing of command line options.
 *
 * Revision 1.1  1996/03/03 04:31:45  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.50  1996/08/05  16:06:58  rich
 * Added comments to endifs.
 *
 * Revision 1.49  1996/07/23  19:32:28  reids
 * Changes to support Windows 95.
 *
 * Revision 1.48  1996/06/20  15:46:37  rich
 * Small changes for new os versions for linux and OSF1.
 *
 * Revision 1.47  1996/05/15  02:25:54  rich
 * Lisp does not mean sparc now.
 *
 * Revision 1.46  1996/05/09  18:31:07  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.45  1996/05/07  16:49:37  rich
 * Changes for clisp.
 *
 * Revision 1.44  1996/04/04  18:18:08  rich
 * Fixed problems found by the SGI compiler.
 *
 * Revision 1.43  1996/02/14  22:12:33  rich
 * Eliminate extra variable logging on startup.
 *
 * Revision 1.42  1996/02/14  03:43:23  rich
 * Added setpgrp for sunos.
 *
 * Revision 1.41  1996/02/13  21:29:01  rich
 * Added environment for linux.
 *
 * Revision 1.40  1996/02/10  16:50:03  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.39  1996/02/06  19:04:54  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.38  1996/01/30  15:04:27  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.37  1996/01/05  16:31:29  rich
 * Added windows NT port.
 *
 * Revision 1.36  1995/12/17  20:21:32  rich
 * Have free routines set pointers to NULL.
 * Removed old makefiles.
 *
 * Revision 1.35  1995/07/25  20:09:08  rich
 * Changes for NeXT machine (3.0).
 *
 * Revision 1.34  1995/07/10  16:17:39  rich
 * Interm save.
 *
 * Revision 1.33  1995/07/08  18:24:47  rich
 * Change all /afs/cs to /afs/cs.cmu.edu to get ride of conflict problems.
 *
 * Revision 1.32  1995/07/08  17:51:09  rich
 * Linux Changes.  Also added GNUmakefile.defs.
 *
 * Revision 1.31  1995/07/06  21:16:41  rich
 * Solaris and Linux changes.
 *
 * Revision 1.30  1995/06/14  17:51:00  rich
 * Fixes for Linux 1.2.
 *
 * Revision 1.29  1995/06/05  23:59:03  rich
 * Improve support of detecting broken pipes.  Add support for OSF 2.
 * Add return types to the global variable routines.
 *
 * Revision 1.28  1995/05/31  19:35:49  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.27  1995/04/17  16:33:37  rich
 * Adding lisp as a subdirectory so it gets included in the tar file.
 *
 * Revision 1.26  1995/04/09  20:30:10  rich
 * Added /usr/local/include and /usr/local/lib to the paths for compiling
 * for sunOS machines. (Support for new vendor OS).
 * Create a x_ipc directory in /tmp and put the socket in there so others can
 * delete dead sockets.  The /tmp directory has the sticky bit set so you
 * can't delete files even if you have write permission on the directory.
 * Fixes to libc.h to use the new declarations in the gcc header files and
 * avoid problems with dbmalloc.
 *
 * Revision 1.25  1995/04/08  02:06:24  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.24  1995/04/07  05:03:19  rich
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
 * Revision 1.23  1995/04/04  19:42:27  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.22  1995/03/19  19:39:34  rich
 * Implemented direct connections using x_ipcDirectResouce call.
 * Also made the basics.h file a module include.
 * Changed class in the interval structure to be interval_class to avoid a
 * conflict with C++.
 *
 * Revision 1.21  1995/03/16  18:05:28  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.20  1995/01/30  16:18:06  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.19  1995/01/25  00:01:20  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.18  1995/01/18  22:41:00  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.17  1994/11/08  05:14:39  rich
 * Change for sun4_mach.
 *
 * Revision 1.16  1994/11/03  06:48:08  rich
 * Fixed some problems with MACH machines.
 *
 * Revision 1.15  1994/11/02  21:34:25  rich
 * Now works for linux machines (i486).
 * Got afs to work on alpha (and hopefully other vendor OS's)
 * Added generic Makefile.
 * Made libc.h and x_ipcMatrix.h module includes.
 * Reduced the size of libc.h by using more system includes.
 *
 * Revision 1.14  1994/06/07  02:30:26  rich
 * Include the top level README and other files in the tarfile.
 * Include extra header infomation to get rid of warnings.
 *
 * Revision 1.13  1994/05/18  02:30:07  rich
 * Accidently flipped the conditions on the include #ifdef.
 *
 * Revision 1.12  1994/05/17  23:16:15  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.11  1994/05/06  07:12:11  rich
 * Fixed makefile so it includes centralIO.[hc].
 * Updated INSTALL for systems without gunzip and gmake.
 *
 * Revision 1.10  1994/05/05  00:46:20  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.9  1994/04/28  16:16:15  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.8  1994/04/16  19:42:26  rich
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
 * Revision 1.7  1994/04/04  16:01:18  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.6  1994/03/28  02:22:58  rich
 * parseFmttrs needs to be in the server objects and not the module objects.
 *
 * Revision 1.5  1994/03/27  22:50:32  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.4  1994/01/31  18:28:00  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.3  1993/11/21  20:18:09  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.2  1993/10/21  03:55:55  rich
 * Removed junk from libc.h
 *
 * Revision 1.1  1993/08/27  07:15:21  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.5  1993/08/23  17:38:47  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.4  1993/07/08  05:38:40  rich
 * Added function prototypes
 *
 * Revision 1.3  1993/06/22  13:59:19  rich
 * Added makefile.depend.  Dependencies automatically generated using gcc.
 * Fixed some warnings.
 * Updated the -D<arch> flags to correspond to those generated
 * automatically by the makefile.
 * Changed system includes to the proper format "stdio.h" -> <stdio.h>.
 * This was needed so that the automatic dependency generation can
 * distinguish between "local" and system headers.  The location of the
 * system headers changes from architecture to architecture and should not
 * be included in the dependency list.
 *
 * Revision 1.2  1993/05/27  22:17:50  rich
 * Added automatic logging.
 *
 * Revision 1.1.1.1  1993/05/20  05:42:28  rich
 * Importing x_ipc version 7
 *
 * Revision 1.2  1993/02/19  21:41:04  rich
 * RTG - fixed libc.h for pmax
 *
 * Revision 1.1  1993/02/19  20:16:26  rich
 * RTG - forgot libc.h
 *
 * Revision 1.7  89/04/03  11:10:45  vanryzin
 * 	Changed definition of qsort for c++ to indicate the procedure
 * 	passed to qsort has parameters.  Since we were unsure if ANSI C
 * 	could handle the syntax I placed the new definition within #if
 * 	defined(c_plusplus) conditionals.  This may not be necessary
 * 	and perhaps should be fixed at a later time.
 * 	[89/04/03            vanryzin]
 * 
 * Revision 1.6  89/02/05  15:55:57  gm0w
 * 	Added extern char *errmsg().
 * 	[89/02/04            gm0w]
 * 
 * Revision 1.5  89/01/20  15:34:40  gm0w
 * 	Moved all of the STDC changes to other existing include files
 * 	back into this one.  Added non-STDC extern declarations for
 * 	all functions without int return values to match those defined
 * 	by STDC.  Added include of sysent.h.  Removed obsolete cdate
 * 	extern declaration.
 * 	[88/12/17            gm0w]
 * 
 * Revision 1.4  88/12/22  16:58:56  mja
 * 	Correct __STDC__ parameter type for getenv().
 * 	[88/12/20            dld]
 * 
 * Revision 1.3  88/12/14  23:31:42  mja
 * 	Made file reentrant.  Added declarations for __STDC__.
 * 	[88/01/06            jjk]
 * 
 * 30-Apr-88  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added pathof() extern.
 *
 * 01-Dec-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added getname() extern.
 *
 * 29-Nov-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added lseek() extern.
 *
 * 02-Nov-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added salloc() extern.
 *
 * 14-Aug-81  Mike Accetta (mja) at Carnegie-Mellon University
 *	Created.
 *
 ***********************************************************************/

#ifndef _T_LIBC_H_
#define	_T_LIBC_H_ 1

#ifndef _PARAMS
#if defined(__STDC__) || defined(__cplusplus)
#define _PARAMS(ARGS) ARGS
#else
#define _PARAMS(ARGS) ()
#endif
#endif /* _PARAMS */

/* Standard includes that should be available */

/* The newest gcc compiler will give the prototypes if this is defined. */
#define __USE_FIXED_PROTOTYPES__ 1
/*#define __STDC__ 1*/

/* Include the debugging malloc library, if requested. */

#ifdef DBMALLOC
#include "dbmalloc/malloc.h"
#define	__string_h
#endif /* DBMALLOC */

/* Standard UNIX includes that should be available */

#if defined(__unix__) && !defined(linux) && !defined(VXWORKS) && !defined(__osf__) && !defined(MACH) && !defined(__MACH__) && !defined(__sgi) && !defined(OS2)
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#if !defined(DBMALLOC) && !defined(__cplusplus)
#include <string.h>
#endif
#include <unistd.h>
#include <memory.h>
#include <sys/socket.h>
#include <ctype.h>
#if !defined(sparc) && !defined(__CYGWIN__) && !defined(__FreeBSD__)
#include <sys/stream.h>		/* TNgo, 11/17/97 */
#endif
#if !defined(sparc) && !defined(__CYGWIN__)
#include <sys/socketvar.h>
#endif
#include <sys/stat.h>
#if !defined(__CYGWIN__) && !defined(__FreeBSD__)
#include <sys/ttold.h>
#include <sys/ttydev.h>
#endif
#include <netinet/in.h>
#include <netinet/tcp.h>
#if !defined(__CYGWIN__)
#undef __P
#endif
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#ifndef _REENTRANT
extern int errno;
#endif
#include <signal.h>
#include <sys/time.h>
#include <sys/uio.h>
#if !defined(__CYGWIN__)
#include <sys/filio.h>
#endif
#include <stdarg.h>
#ifndef vfprintf
extern int vfprintf _PARAMS((FILE *, const char *, va_list));
#endif
#ifndef vsprintf
extern int vsprintf _PARAMS((char *, const char *, va_list));
#endif

#if defined(SYSV)
extern char *index(const char*, char);
extern void cfree _PARAMS((void *));
extern int gethostname _PARAMS((char *name, int namelen));
#endif

#if defined(__CYGWIN__)
#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#undef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#endif /* unix */

#if defined(macintosh)
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
char *	strdup(const char *str);
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
// #include <arpa/inet.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sys/uio.h>
#include <netdb.h>
#include <machine/endian.h>
#include <errno.h>
#include <sys/errno.h>

extern int select _PARAMS((int, fd_set*, fd_set*, fd_set*,
			   				const struct timeval*));

/* Needed by some of the Lisp test programs */
#pragma export on
extern struct timeval *gettimeofday _PARAMS((struct timeval *, void *dummy));
#pragma export off

extern void gethostname(char *hostName, int size);
extern int fileno(FILE *);
struct in_addr inet_addr(const char *address);

extern int close _PARAMS((int));
extern int read _PARAMS((int, void *, int));
extern int write _PARAMS((int, void *, int));
extern int unlink _PARAMS((char *));

#define FORCE_32BIT_ENUM

#endif /* macintosh */

#if defined(__APPLE__) // OSX, hopefully
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#undef IPC_ALIGN
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <netdb.h>
#include <errno.h>

#endif // __APPLE__ / OSX

#if defined(Next) ||  defined(__NeXT__)
#include <stdio.h>
#include <stdlib.h>
#include <bsd/libc.h>
#include <signal.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <errno.h>
extern int errno;
#include <sys/uio.h>
#include <bsd/netinet/in.h>
#include <bsd/netinet/tcp.h>
#include <bsd/netdb.h>
#include <ansi/string.h>
#include <ansi/ctype.h>
#define	S_IRWXU 	0000700	/* rwx, owner */
#define		S_IRUSR	0000400	/* read permission, owner */
#define		S_IWUSR	0000200	/* write permission, owner */
#define		S_IXUSR	0000100	/* execute/search permission, owner */
#define	S_IRWXG		0000070	/* rwx, group */
#define		S_IRGRP	0000040	/* read permission, group */
#define		S_IWGRP	0000020	/* write permission, grougroup */
#define		S_IXGRP	0000010	/* execute/search permission, group */
#define	S_IRWXO		0000007	/* rwx, other */
#define		S_IROTH	0000004	/* read permission, other */
#define		S_IWOTH	0000002	/* write permission, other */
#define		S_IXOTH	0000001	/* execute/search permission, other */
#include <machine/endian.h>
#endif /* Next */

#if defined(linux)
#define __NO_MATH_INLINES
#if !defined(REDHAT_52) && !defined(REDHAT_6)
#include <bsd/bsd.h>
#endif /* REDHAT_52 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#ifndef DBMALLOC
#include <string.h>
#endif
#include <ctype.h>
#include <endian.h>
#include <signal.h>
#if defined(REDHAT_52) || defined(REDHAT_6)
#include <sys/time.h>
#include <sys/timeb.h>
#endif
#if defined(REDHAT_71)
#include <time.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <sys/uio.h>
#if !defined(REDHAT_52) && !defined(REDHAT_6)
#include <bsd/sgtty.h>
#else /* !REDHAT_52 */
#include <termios.h>
#include <fcntl.h>
#ifndef REDHAT_6
#include <ioctls.h>
#else
#include <sys/select.h>
#include <sys/ioctl.h>
#endif
#include <sys/uio.h>
#endif /* REDHAT_52 */
#define TIOCSDTR _IO('t', 121)          /* set data terminal ready */
#define TIOCCDTR _IO('t', 120)		/* clear data terminal ready */
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/resource.h>

extern char **environment;

/* Added by CFarmer, 7/2/97 */
#if !defined (va_list) && defined (__GNUC__)
#define __need_va_list
#include <stdarg.h>
#define va_list __gnuc_va_list
#endif /* va_list, __GNUC__ */

#endif /* linux */

#if defined(__sgi)

#ifndef _SVR4_SOURCE
#define _SVR4_SOURCE
#endif /* _SVR4_SOURCE */

#ifndef _SGI_SOURCE
#define _SGI_SOURCE
#endif /* _SGI_SOURCE */

/*#ifndef _XOPEN_SOURCE*/
/*#define _XOPEN_SOURCE*/
/*#endif*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <bstring.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#define _KERNEL /* Needed to avoid conflicting definition of readv */
#include <sys/uio.h>
#undef _KERNEL
#include <sys/stat.h>
#include <sys/ttold.h>
#include <sys/ttydev.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
extern int errno;
#include <fcntl.h>
#include <sys/filio.h>
#include <strings.h>
#ifndef DBMALLOC
#include <string.h>
#endif /* !DBMALLOC */
extern long	random(void);

#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#undef BYTE_ORDER
#define BYTE_ORDER BIG_ENDIAN

#endif /* __sgi */

#if defined(VXWORKS)
#ifdef _GREEN_TOOL
#include <stdarg.h>
#endif /* _GREEN_TOOL */
#include <vxWorks.h>
#include <stdioLib.h>
#include <stdlib.h>
#include <ctype.h>
#include <socket.h>
#include <in.h>
#include <sigLib.h>
#include <errno.h>
#ifdef m68k_vx52_gnu27x
#include <string.h>
#endif /* m68k_vx52_gnu27x */
#include <systime.h>
#ifdef VX_5_0_2B
#include <sys/uio.h>
#include <net/tcp.h>
#include <ioLib.h>
#include <sys/un.h>
#else /* VX_5_0_2B */
#include <arpa/inet.h>
#include <netinet/tcp.h>
#ifndef SYSV
#include <sys/fcntlcom.h>
#endif /* SYSV */
#include <sys/stat.h>
#include <net/uio.h>
#include <sockLib.h>
#include <hostLib.h>
#include <fcntl.h>
#ifdef i386
#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER LITTLE_ENDIAN
#else /* !i386 */
#define BYTE_ORDER _BYTE_ORDER
#define LITTLE_ENDIAN _LITTLE_ENDIAN
#define BIG_ENDIAN _BIG_ENDIAN
#define	PDP_ENDIAN 3412	/* LSB first in word, MSW first in long (pdp) */
#endif /* i386 */
#include <selectLib.h>
#include <usrLib.h>
#include <taskVarLib.h>
#include <tickLib.h>
#include <sysLib.h>
#endif /* VX_5_0_2B */

/* TNgo, 8/31/98, corrected by request of Jeff Kowing. */
#define strdup(str)  (strcpy(malloc(strlen(str) + 1), (str)))

#define NO_UNIX_SOCKETS 1
#define VX_PIPES 1
#include <pipeDrv.h>
#define NEED_READV 1

#endif /* VXWORKS */

/* SUN Sparcstation definitions */

#if defined(sparc) && ! defined(MACH) && ! defined(__svr4__) && ! defined(SYSV)
/* sparc, sunOS */

#ifndef DBMALLOC
#include <malloc.h>
#ifndef __ghs
extern	malloc_t	valloc _PARAMS((__SIZE_TYPE__ ));
#endif /* __ghs */
#else /* DBMALLOC */
DATATYPE	* valloc __stdcargs((SIZETYPE));
#endif /* DBMALLOC */

#include <sys/mman.h>
#ifndef SYSV
#include <sys/fcntlcom.h>
#endif /* SYSV */

#include <time.h>
#include <sys/ttold.h>
#include <sys/ttydev.h>

#define	LITTLE_ENDIAN	1234	/* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321	/* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412	/* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER BIG_ENDIAN

extern int fclose _PARAMS((FILE *));
extern int fflush _PARAMS((FILE *));
extern int fgetc _PARAMS((FILE *));
#ifndef __ghs
extern int fgetpos _PARAMS((FILE *, long *));
#endif /* __ghs */
extern int fprintf _PARAMS((FILE *, const char *, ...));
extern int fputc _PARAMS((int, FILE *));
extern int fputs _PARAMS((const char *, FILE *));
#ifndef __ghs
extern long unsigned int fread _PARAMS((void *,
					long unsigned int,
					long unsigned int , FILE *));
#endif /* __ghs */
extern int fscanf _PARAMS((FILE *, const char *, ...));
extern int fseek _PARAMS((FILE *, long int, int));
#ifndef __ghs
extern int fsetpos _PARAMS((FILE *, const long *));
extern long unsigned int fwrite _PARAMS((const void *,
					 long unsigned int,
					 long unsigned int , FILE *));
#endif /* __ghs */
extern void perror _PARAMS((const char *));
extern int printf _PARAMS((const char *, ...));
extern int puts _PARAMS((const char *));
extern int remove _PARAMS((const char *));
extern int rename _PARAMS((const char *, const char *));
extern void rewind _PARAMS((FILE *));
extern int scanf _PARAMS((const char *, ...));
extern void setbuf _PARAMS((FILE *, char *));
#ifndef __ghs
extern int setvbuf _PARAMS((FILE *, char *, int, long unsigned int ));
#endif /* __ghs */
extern int sscanf _PARAMS((const char *, const char *, ...));
extern int ungetc _PARAMS((int, FILE *));
extern int _filbuf _PARAMS((FILE *));

extern unsigned int usleep _PARAMS((unsigned useconds));

extern int bcmp _PARAMS((const void *, const void *, int));
#ifndef __ghs  
extern void bcopy _PARAMS((const void *src, const void *dest, int len));
extern void bzero _PARAMS((void *ptr, size_t len));
#endif /* __ghs */

/* defines that should be in <sys/socket.h> */
extern int accept _PARAMS((int, struct sockaddr *, int *));
extern int bind _PARAMS((int, struct sockaddr *, int));
#ifndef __ghs
extern int connect _PARAMS((int s, const struct sockaddr *name, int namelen));
extern int getsockopt _PARAMS((int, int, int, void *optval, int *));
#endif /* __ghs */
extern int setsockopt _PARAMS((int s, int level, int optname,
			       const char *optval, int optlen));
extern int listen _PARAMS((int, int));
extern int socket _PARAMS((int, int, int));
extern int gethostname _PARAMS((char *name, int namelen));
extern int getsockname _PARAMS((int, struct sockaddr *, int *));
#ifndef __ghs
extern int select _PARAMS((int, fd_set*, fd_set*, fd_set*,
			   const struct timeval*));
#endif /* __ghs */

extern int gettimeofday _PARAMS((struct timeval *, struct timezone *));
extern long time _PARAMS((long*));

extern shutdown _PARAMS((int s, int how));
#ifndef __ghs
extern void cfree _PARAMS((void *));
#endif /* __ghs */

extern int ioctl _PARAMS((int fd, int request, ...));
extern int brk _PARAMS((caddr_t addr));

extern caddr_t sbrk _PARAMS((int incr));

extern void srandom _PARAMS((unsigned seed));
extern long random _PARAMS((void));

extern int readv _PARAMS((int, struct iovec *, int));
extern int writev _PARAMS((int, struct iovec *, int));

extern int fchmod _PARAMS((int, int));

#ifndef DBMALLOC
extern char *index(const char*, char);
#endif /* DBMALLOC */

#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/param.h>
#ifndef __ghs 
int wait4(int pid, union wait *status, int options, struct rusage *rusage);
#endif /* __ghs */

extern char **environment;
extern int setpgrp(int, int);

#endif /* sparc, sunOS */

#if defined(sparc) && ( defined(__svr4__) || defined(SYSV) )
/* sparc, Solaris */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#ifndef DBMALLOC
#include <string.h>
#endif /* !DBMALLOC */
#include <strings.h>
#include <unistd.h>
#include <memory.h>
#include <sys/sunddi.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <sys/ttold.h>
#include <sys/ttydev.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#ifndef _REENTRANT
extern int errno;
#endif /* !_REEANTRANT */
#include <signal.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/filio.h>

#ifndef DBMALLOC
#include <malloc.h>
#endif /* !DBMALLOC */
#include <sys/ttold.h>
#include <sys/ttydev.h>
#include <fcntl.h>

#define	LITTLE_ENDIAN	1234	/* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321	/* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412	/* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER BIG_ENDIAN

#undef bzero
#define bzero(buf, len) memset((void *)(buf), 0, (size_t)(len))

#undef bcopy
#define bcopy(from, to, len) memcpy((void *)(to), (void *)(from), (size_t)len)

extern int gethostname(char *name, int namelen);

#endif /* sparc, Solaris */
/* Mach definitions */

#if defined(MACH)
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#ifndef DBMALLOC
#include <string.h>
#endif /* !DBMALLOC */
/*#include <unistd.h>*/
#include <memory.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
extern int errno;
#include <signal.h>
#include <sys/time.h>
#include <machine/endian.h>
#include <time.h>
#include <sys/file.h>
#include <sysent.h>
#include <sys/uio.h>
#include "/usr/cs/include/libc.h"

extern int read _PARAMS((int, void *, int));
extern int write _PARAMS((int, void *, int));
extern void usleep _PARAMS((unsigned));
extern void sleep _PARAMS((unsigned));

extern int bcmp _PARAMS((const void *, const void *, int));
extern void bcopy _PARAMS((const void *, void *, int));
extern void bzero _PARAMS((void *, int));

extern int close _PARAMS((int));

/* defines that should be in <sys/socket.h> */
extern int gethostname _PARAMS((char *name, int namelen));
extern shutdown _PARAMS((int s, int how));
extern int select _PARAMS((int, fd_set*, fd_set*, fd_set*,
			   struct timeval*));

#ifndef vprintf
extern int vprintf _PARAMS((const char *, void * ));
#endif
#ifndef vsprintf
extern char	*vsprintf _PARAMS((char *, const char *, void *));
#endif
#ifndef vfprintf
extern int	vfprintf _PARAMS((FILE *, const char *, void *));
#endif

#define	S_IRWXU 	0000700	/* rwx, owner */
#define		S_IRUSR	0000400	/* read permission, owner */
#define		S_IWUSR	0000200	/* write permission, owner */
#define		S_IXUSR	0000100	/* execute/search permission, owner */
#define	S_IRWXG		0000070	/* rwx, group */
#define		S_IRGRP	0000040	/* read permission, group */
#define		S_IWGRP	0000020	/* write permission, grougroup */
#define		S_IXGRP	0000010	/* execute/search permission, group */
#define	S_IRWXO		0000007	/* rwx, other */
#define		S_IROTH	0000004	/* read permission, other */
#define		S_IWOTH	0000002	/* write permission, other */
#define		S_IXOTH	0000001	/* execute/search permission, other */

/* From ttold.t */
#define		O_ECHO		0x00000008	/* echo input */
#define		O_RAW		0x00000020	/* no i/o processing */

#define HUGE_VAL HUGE
#endif /* Mach */

#if defined(__osf__)
#define _XOPEN_SOURCE
#define _OSF_SOURCE
#include <stdio.h>
#include <stdlib.h>
/*#include <math.h>*/
#include <limits.h>
#ifndef DBMALLOC
#include <string.h>
#endif
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/socketvar.h>
#include <sys/stat.h>
#include <netinet/in.h>
#ifndef TCPOPT_NOP
#include <netinet/tcp.h>
#endif
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
extern int errno;
#include <signal.h>
#include <sys/time.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/uio.h>
/* This should be changed when we rename libc.h */
/* #include "/usr/local/include/libc.h" */   /* TNgo, 8/6/97 */
/* Ones that are missed. */

/*extern int srandom _PARAMS((int));*/
/*extern long random _PARAMS((void));*/

/*extern int bcmp _PARAMS((const void *, const void *, int));*/
/*extern void bcopy _PARAMS((const void *, void *, int));*/
/*extern void bzero _PARAMS((void *, int));*/

/* defines that should be in <sys/socket.h> */
/* extern int accept _PARAMS((int, struct sockaddr *, int *)); */
/* extern int bind _PARAMS((int, struct sockaddr *, int)); */
/* extern int connect _PARAMS((int s, const struct sockaddr *name,
			       int namelen)); */
/* extern int getsockopt _PARAMS((int, int, int, void *optval, int *)); */
/* extern int setsockopt _PARAMS((int s, int level, int optname,
			          const char *optval, int optlen)); */
/* extern int listen _PARAMS((int, int)); */
/* extern int socket _PARAMS((int, int, int)); */
/* extern int gethostname _PARAMS((char *name, int namelen)); */
/* extern int getsockname _PARAMS((int, struct sockaddr *, int *)); */

/* extern int select _PARAMS((int, fd_set*, fd_set*, fd_set*,
			   const struct timeval*)); */

extern void usleep _PARAMS((unsigned));

/* extern int ioctl _PARAMS((int fd, int request, ...)); */
/* extern void cfree _PARAMS((void *)); */

/* extern shutdown _PARAMS((int s, int how)); */

/* extern long unsigned int strftime _PARAMS((char *,
					   long unsigned int,
					   const char *, const struct tm *)); */
/* extern int fchmod _PARAMS((int, int)); */

#include <curses.h>
#include <sys/ioctl.h>
#undef NOFLSH
#include <sys/ioctl_compat.h>
#define  O_RAW           0x00000020     /* no i/o processing */
#define  O_ECHO           0x00000008      /* echo input */

#define _BSD
#include <sys/wait.h>
#define setpgrp tcsetpgrp

#include <stdarg.h>

#endif /* __osf__ */

#ifdef THINK_C
#include <console.h>
#include <unix.h>
#endif /* THINK_C */

#ifdef THINK_C
#include <unix.h>
#undef bzero
#define bzero(buf, len) memset((void *)(buf), 0, (size_t)(len))

#undef bcopy
#define bcopy(from, to, len) memcpy((void *)(to), (void *)(from), (size_t)len)
#endif /* THINK_C */

#if defined(WIN32)
#define _WINSOCK_
#endif

#if defined(__TURBOC__) && !defined(_WINSOCK_)

#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>
#include <alloc.h>

#define AF_INET         2               /* internetwork: UDP, TCP, etc. */

#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER LITTLE_ENDIAN

struct	iovec {
	caddr_t	iov_base;
	long	iov_len;
};

struct	sockaddr_un {
	short	sun_family;		/* AF_UNIX */
	char	sun_path[108];		/* path name (gag) */
};

#undef bzero
#define bzero(buf, len) memset((void *)(buf), 0, (size_t)(len))

#undef bcopy
#define bcopy(from, to, len) memcpy((void *)(to), (void *)(from), (size_t)len)


#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define SOCKETS
#include <arpa/inet.h>

#define SOCK_STREAM     1               /* stream socket */
/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET      0xffff          /* options for socket level */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */

/*
 * Additional options.
 */
#define SO_SNDBUF       0x1001          /* send buffer size */
#define SO_RCVBUF       0x1002          /* receive buffer size */
#define SO_SNDLOWAT     0x1003          /* send low-water mark */
#define SO_RCVLOWAT     0x1004          /* receive low-water mark */
#define SO_SNDTIMEO     0x1005          /* send timeout */
#define SO_RCVTIMEO     0x1006          /* receive timeout */
#define SO_ERROR        0x1007          /* get error status and clear */
#define SO_TYPE         0x1008          /* get socket type */

/*
 * TCP options.
 */
#define TCP_NODELAY     0x0001
#define AF_UNIX         1               /* local to host (pipes, portals) */

#define	S_IRWXU 	0000700	/* rwx, owner */
#define		S_IRUSR	0000400	/* read permission, owner */
#define		S_IWUSR	0000200	/* write permission, owner */
#define		S_IXUSR	0000100	/* execute/search permission, owner */
#define	S_IRWXG		0000070	/* rwx, group */
#define		S_IRGRP	0000040	/* read permission, group */
#define		S_IWGRP	0000020	/* write permission, grougroup */
#define		S_IXGRP	0000010	/* execute/search permission, group */
#define	S_IRWXO		0000007	/* rwx, other */
#define		S_IROTH	0000004	/* read permission, other */
#define		S_IWOTH	0000002	/* write permission, other */
#define		S_IXOTH	0000001	/* execute/search permission, other */

/* #define int long */
#define int32 long
#define int16 short
#define NO_UNIX_SOCKETS 1
#define NEED_READV 1
#endif /* __TURBOC__ */

#if  defined(_WINSOCK_) && !defined (_WIN95_MSC_) && !defined(WINNT)
/* Begin Winsock defines */

#include <winsock.h>
#include <dos.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>  /* Time types */
#include <time.h>			/* Time defines */
#include <limits.h>
#ifndef WIN32
#include <alloc.h>
#include <arpa/inet.h>

#include <sys/un.h>
#else
extern char *index(const char*, char);
typedef int socklen_t;
#endif /* WIN32 */

#define  NO_UNIX_SOCKETS 1
#define  NEED_READV 1

#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER LITTLE_ENDIAN

struct	iovec {
	char*	iov_base;
	long	iov_len;
};

struct	sockaddr_un {
	short	sun_family;		/* AF_UNIX */
	char	sun_path[108];		/* path name (gag)*/
};

#undef bzero
#define bzero(buf, len) memset((void *)(buf), 0, (size_t)(len))

#undef bcopy
#define bcopy(from, to, len) memcpy((void *)(to), (void *)(from), (size_t)len)

/* #define int long */
#define int32 long
#define int16 short

/* JSM Added for Windows 98 compatibility */
#undef BOOLEAN			/* Need int_32 sized booleans instead of char size */
#define BOOLEAN int		/* This gets typedef'd in the MSC includes */

#endif /* _WINSOCK */

#ifdef LISP
//extern int strcasecmp(char *, char *);
#endif /* LISP */

#if defined(OS2) 
/* #define __STDC__ 1 */	/* Already predefined in IBM Visual Age CPP */

#define BSD_SELECT
#define INCL_DOS
#define INCL_BASE
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <types.h>			/* Want the MPTN (TCP) "types" before the IBMCPP "types" */
#include <utils.h>
#include <netinet\in.h>
#include <sys\ioctl.h>
#include <sys\select.h>
#include <sys\socket.h>
#include <sys\UN.h>
#include <time.h>
#include <io.h>
#include <sys\time.h>
#include <sys\stat.h>
#include <netdb.h>

#include <sys/types.h>		/* IBMCPP "types"*/
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>
#include <direct.h>
#include <nerrno.h>

/* OS/2 uses other variables for getting the socket errors, so redefine
   errno and perror to their OS/2 counterparts */

#ifdef errno
#undef errno
#endif
#define errno sock_errno()
#define perror psock_errno

#define NO_UNIX_SOCKETS 1
#define TCP_NODELAY 0x01
#define EPIPE ERROR_BROKEN_PIPE

#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER LITTLE_ENDIAN


#define	S_IRWXU 		S_IREAD
#define		S_IRUSR		S_IREAD
#define		S_IWUSR		S_IREAD
#define		S_IXUSR		S_IREAD
#define	S_IRWXG			S_IREAD
#define		S_IRGRP		S_IREAD
#define		S_IWGRP		S_IREAD
#define		S_IXGRP		S_IREAD
#define	S_IRWXO			S_IREAD
#define		S_IROTH		S_IREAD
#define		S_IWOTH		S_IREAD
#define		S_IXOTH		S_IREAD


/* OS/2 treats tcp sockets different than other file descriptors 
   so redefine read/write/close to socket recv/send/soclose compatiable calls.
   In addition, only socket file descriptors work in the "select" calls
   so "waiting" on normal c file descriptors (stdin, stdout, disk files, etc.)
   cause an immediate error return */ 

#define read(a,b,c) recv((int)(a),(char *)(b),(int)(c),(int)(0))
#define write(a,b,c) send((int)(a),(char *)(b),(int)(c),(int)(0))
#define close(a) soclose((int)(a))

#define  fchmod(a,b) 					/* Don't have one of these */


#endif     /* OS/2 defines */


#if  defined(_WINSOCK_) && (defined (_WIN95_MSC_) || defined(WINNT))
/* Begin Winsock and Windows 95 with Microsoft 32bit Complier defines */

#include <winsock.h>
#ifndef WINNT
#include <dos.h>
#endif /* !WINNT */
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>  /* Time types */
#include <time.h>			/* Time defines */
#include <limits.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>


#define NO_UNIX_SOCKETS 1
#define NEED_READV 1

#define	LITTLE_ENDIAN	1234 /* least-significant byte first (vax) */
#define	BIG_ENDIAN	4321 /* most-significant byte first (IBM, net) */
#define	PDP_ENDIAN	3412 /* LSB first in word, MSW first in long (pdp) */
#define BYTE_ORDER LITTLE_ENDIAN

struct	iovec {
	char*	iov_base;
	long	iov_len;
};

struct	sockaddr_un {
	short	sun_family;		/* AF_UNIX */
	char	sun_path[108];		/* path name (gag)*/
};

#undef bzero
#define bzero(buf, len) memset((void *)(buf), 0, (size_t)(len))

#undef bcopy
#define bcopy(from, to, len) memcpy((void *)(to), (void *)(from), (size_t)len)

#undef BOOLEAN			/* Need int_32 sized booleans instead of char size */
#define BOOLEAN int		/* This gets typedef'd in the MSC includes */

#endif /* _WINSOCK_ and WINDOWS 95 Microsoft C 32bit complier */

#if !defined(__sgi) && !defined(WIN32) && !defined(REDHAT_52) && !defined(__svr4__) && !defined(__CYGWIN__)
#define UNUSED_PRAGMA
#endif


#endif	/* not _T_LIBC_H_ */

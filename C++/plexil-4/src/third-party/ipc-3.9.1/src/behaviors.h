/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 *
 * MODULE: behaviors
 *
 * FILE: behaviors.c
 *
 * ABSTRACT:
 * 
 * x_ipc behavior level.
 *
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/behaviors.h,v $ 
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
 * $Log: behaviors.h,v $
 * Revision 2.4  2009/01/12 15:54:55  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/06/25 16:44:58  reids
 * Fixed the way memory is freed when responses are handled;
 *   Allowed me to remove "responseIssuedGlobal".
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
 * Revision 1.2.2.7  1997/01/27 20:09:05  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.5  1997/01/11 01:20:43  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.2.4.6.1  1996/12/24 14:41:29  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.2.2.4  1996/10/22 18:49:25  reids
 * Point-to-point broadcast messages.
 *
 * Revision 1.2.2.3  1996/10/14 15:01:11  reids
 * Changes to fix freeing of formatters, and to handle errors more cleanly.
 *
 * Revision 1.2.2.2  1996/10/08 14:23:33  reids
 * Changes for IPC operating under Lispworks on the PPC.  Mainly changes
 * (LISPWORKS_FFI_HACK) due to the fact that Lispworks on the PPC is currently
 * missing the foreign-callable function.
 *
 * Revision 1.2.2.1  1996/10/02 20:58:20  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.2  1996/05/09 18:19:27  reids
 * Changes to support CLISP.
 *
 * Revision 1.1  1996/05/09 01:01:10  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/04/01 02:36:53  reids
 * Needed to make x_ipc_queryNotifySend globally accessible for IPC
 *
 * Revision 1.3  1996/03/19 03:38:33  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:29  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:30:54  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.17  1996/05/09  18:30:20  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.16  1996/05/07  16:49:16  rich
 * Changes for clisp.
 *
 * Revision 1.15  1996/03/15  21:13:19  reids
 * Added x_ipcQueryNotify and plugged a memory leak -- ref was not being
 *   freed when inform/broadcast handler completed.
 *
 * Revision 1.14  1996/03/05  05:04:14  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.13  1996/02/10  16:49:28  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.12  1996/01/27  21:52:53  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.11  1995/07/12  04:54:12  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.10  1995/05/31  19:35:00  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.9  1995/04/08  02:06:16  rich
 * Added x_ipc_waitForReplyFrom to be able to block on replies from only one
 * source.  Useful when querying for the msg info information.  Added a
 * x_ipcQueryCentral that only accepts input from the central server.  Fixed
 * timing problems with direct connections.
 *
 * Revision 1.8  1995/01/18  22:39:38  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/17  23:15:09  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1994/04/16  19:41:36  rich
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
 * Revision 1.5  1993/12/01  18:02:37  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:17:09  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/30  21:53:02  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.2  1993/08/27  08:38:20  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.1  1993/08/27  07:14:07  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/08/23  17:37:31  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.3  1993/07/08  05:38:08  rich
 * Added function prototypes
 *
 * Revision 1.2  1993/05/27  22:16:08  rich
 * Added automatic logging.
 *
 *
 ****************************************************************/

#ifndef INCbehaviors
#define INCbehaviors

const char *x_ipc_messageClassName(X_IPC_MSG_CLASS_TYPE msg_class);
void x_ipc_checkMessageClass(MSG_PTR msg, X_IPC_MSG_CLASS_TYPE msg_class);
void x_ipc_execHnd(CONNECTION_PTR connection, DATA_MSG_PTR dataMsg);
BOOLEAN x_ipc_execFdHnd(int fd);
MSG_PTR x_ipc_msgFind(const char *name);
CONST_FORMAT_PTR x_ipc_fmtFind(const char *name);
int32 testInconsistent(int32 tplConstraints);

X_IPC_RETURN_VALUE_TYPE x_ipcQueryFd(const char *name,
				 void *queryData,
				 void *replyData,
				 int fd);

X_IPC_RETURN_VALUE_TYPE x_ipcQueryCentral(const char *name,
				      void *queryData,
				      void *replyData);

MSG_PTR x_ipc_msgFind2(const char *name, const char *hndName);

X_IPC_RETURN_VALUE_TYPE _x_ipcQueryNotify(const char *name, void *query, 
				      REPLY_HANDLER_FN replyHandler, 
				      HND_LANGUAGE_ENUM language,
				      void *clientData);

#ifdef NMP_IPC
/* Three-argument handler type, for NMP IPC */
typedef void (*IPC_HANDLER_TYPE)(X_IPC_REF_PTR ref, void *callData,
				 void *clientData);

typedef struct { int length;
		 void *content;
	       } *IPC_VAR_DATA_PTR;

FORMAT_CLASS_TYPE ipcFormatClassType (CONST_FORMAT_PTR format);

/* Pick out the byte array in the data -- eventually replace */
char *ipcData (CONST_FORMAT_PTR formatter, char *data);
#endif /* NMP_IPC */

X_IPC_RETURN_VALUE_TYPE x_ipc_queryNotifySend (MSG_PTR msg, const char *name,
				       void *query,
				       REPLY_HANDLER_FN replyHandler, 
				       HND_LANGUAGE_ENUM language,
				       void *clientData);

/* Clean-up stuff to do when a handler has completed */
void endExecHandler (X_IPC_REF_PTR x_ipcRef, CONNECTION_PTR connection, 
		     MSG_PTR msg, int tmpParentRef);

#endif /* INCbehaviors */

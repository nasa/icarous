/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: formatters
 *
 * FILE: formatters.c
 *
 * ABSTRACT:
 *
 * Data Format Routines.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: formatters.c,v $
 * Revision 2.8  2009/09/04 19:12:57  reids
 * Port for ARM
 *
 * Revision 2.7  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2005/12/30 17:01:44  reids
 * Support for Mac OSX
 *
 * Revision 2.5  2002/01/03 20:52:11  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.4  2001/02/09 16:24:19  reids
 * Added IPC_getConnections to return list of ports that IPC is listening to.
 * Added IPC_freeDataElements to free the substructure (pointers) of a struct.
 *
 * Revision 2.3  2000/08/14 21:28:33  reids
 * Added support for making under Windows.
 *
 * Revision 2.2  2000/07/03 17:03:23  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.8  1997/01/27 20:39:58  reids
 * Implement a function to check whether two formats are the same.
 *
 * Revision 1.2.2.7  1997/01/27 20:09:23  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.5  1997/01/11 01:20:54  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.2.4.4.3  1996/12/28 01:06:30  reids
 * Fixed a bug in x_ipc_alignField having to do with the way ALIGN_WORD handles
 *   fixed length arrays
 *
 * Revision 1.2.2.4.4.2  1996/12/27 19:25:58  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.2.2.4.4.1  1996/12/24 14:41:34  reids
 * Merge the C and Lisp IPC libraries (both C and Lisp modules can now
 *   link with the same libipc.a).
 * Moved the Lisp-specific code to ipcLisp.c: Cleaner design and it will
 *   not be linked into C modules this way.
 *
 * Revision 1.2.2.4  1996/12/18 15:12:47  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.3  1996/10/16 15:17:45  reids
 * Cache format attribute values (much more efficient).
 * Transfer structure in one go if it does not contain pointers or padding.
 * Add short, byte and ubyte formats for Lisp.
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.2.2.2  1996/10/08 14:21:05  reids
 * Change argument order of formatPutX functions, since Lispworks was
 * corrupting the stack when a double was the first argument to a foreign fn.
 *
 * Revision 1.2.2.1  1996/10/02 20:58:27  reids
 * Changes to support LISPWORKS.
 *
 * Revision 1.2  1996/05/09 18:19:28  reids
 * Changes to support CLISP.
 *
 * Revision 1.1  1996/05/09 01:01:23  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/03/19 03:38:40  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.3  1996/03/12 05:18:37  reids
 * Fix external declaration for Solaris.
 *
 * Revision 1.2  1996/03/12 03:19:40  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:23  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.48  1996/07/24  13:46:01  reids
 * Support for Windows95 (Thanks to Tam Ngo, JSC)
 * When module goes down, do not remove dispatch if still have task tree node.
 * Handle NULL format in x_ipc_dataStructureSize.
 * Add short, byte and ubyte formats for Lisp.
 * Ignore stdin when checking sockets for input.
 *
 * Revision 1.47  1996/07/19  18:14:04  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.46  1996/07/11  17:09:04  reids
 * Need to make the host and message byte order available to LISP
 *
 * Revision 1.45  1996/06/25  20:50:33  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.44  1996/05/09  18:30:53  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.43  1996/05/07  16:49:26  rich
 * Changes for clisp.
 *
 * Revision 1.42  1996/03/19  02:29:18  reids
 * Plugged some more memory leaks; Added test code for enum formatters.
 * Added code to free formatter data structures.
 *
 * Revision 1.41  1996/03/15  21:18:00  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.40  1996/03/05  05:04:24  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.39  1996/02/10  16:49:57  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.38  1996/01/30  15:04:05  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.37  1996/01/27  21:53:24  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.36  1995/11/03  03:04:31  rich
 * Changed x_ipc_msgFind to keep if from going into an infinite loop if there is no
 * central connection.  This only happens when an exit procedure that does
 * not exit is registered.  x_ipc_msgFind can now return NULL, so I added some
 * checks for the return value to keep modules from seg-faulting.
 *
 * Revision 1.35  1995/10/29  18:26:44  rich
 * Initial creation of 8.3. Added changes made after 8.2 branch was
 * created. These mostly have to do with context switching.
 *
 * Revision 1.34  1995/10/25  22:48:18  rich
 * Fixed problems with context switching.  Now the context is a separate
 * data structure accessed from the module data structure, using the
 * currentContext field.  GET_C_GLOBAL is used instead of GET_M_GLOBAL for
 * the context dependent fields.
 *
 * Revision 1.33  1995/10/03  22:18:42  rich
 * Fixed error where array size was calculated incorrectly when transfering
 * data for byte arrays.
 *
 * Revision 1.32  1995/07/12  04:54:47  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.31  1995/07/10  16:17:18  rich
 * Interm save.
 *
 * Revision 1.30  1995/07/06  21:16:12  rich
 * Solaris and Linux changes.
 *
 * Revision 1.29  1995/05/31  19:35:30  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.28  1995/04/19  14:28:10  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.27  1995/04/05  19:10:42  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.26  1995/04/04  19:42:06  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.25  1995/03/30  15:42:56  rich
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
 * Revision 1.24  1995/03/16  18:05:22  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.23  1995/03/14  22:36:45  rich
 * Fixed problem with multiple read needed when doing a vector read. (Fix
 * to version 7.9)
 * Also fixed the data size problem from 7.9.
 *
 * Revision 1.22  1995/01/30  16:17:53  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.21  1995/01/25  00:01:10  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.20  1995/01/18  22:40:25  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.19  1994/10/25  17:09:53  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.18  1994/05/31  03:23:54  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.17  1994/05/17  23:15:48  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.16  1994/05/11  01:57:24  rich
 * Now set an invalid x_ipcServerGlobal (a socket fd) to -1 rather than 0
 * which is stdout.
 * Added checks to make sure x_ipcServerGlobal is a valid socket before
 * sending messages or waiting for messages.
 *
 * Revision 1.15  1994/05/05  00:46:06  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.14  1994/04/28  16:16:00  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.13  1994/04/16  19:42:08  rich
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
 * Revision 1.12  1994/04/04  16:01:15  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.11  1994/03/27  22:50:26  rich
 * Fixed problem with lisp version not working because of some compiler
 * flags used for the shared library version.
 * X_IPC now compiles for alphas, but does not run.
 *
 * Revision 1.10  1994/01/31  18:27:47  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.9  1993/12/14  17:33:36  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.8  1993/11/21  20:17:50  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.7  1993/08/30  23:13:58  fedor
 * Added SUN4 as well as sun4 compile flag.
 * Corrected Top level failure message name display with a define in dispatch.c
 *
 * Revision 1.6  1993/08/30  21:53:27  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.5  1993/08/27  07:14:47  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/08/20  23:06:55  fedor
 * Minor changes for merge. Mostly added htons and removed cfree calls.
 *
 * Revision 1.3  1993/06/13  23:28:06  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.2  1993/05/26  23:17:30  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:22  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:49  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:46  fedor
 * Added Logging.
 *
 *  9-Jul-91 Christopher Fedor, School of Computer Science, CMU
 * Removed isIntFormat test routine because we now have string formats to test
 * and this allows testing of formats without first parsing it.
 *
 *  2-Jul-91 Reid Simmons at School of Computer Science, CMU
 * Added code for freeing class data.
 *
 * 30-Jan-91 Christopher Fedor, School of Computer Science, CMU
 * Added fflush(stdout) to printf for module code calls from lisp
 *
 * 23-Oct-90 Christopher Fedor at School of Computer Science, CMU
 * Moved isIntFormat from mon.c to here - it logically belongs here 
 * even though it increases module code a little.
 * format needs a complete rewrite! flush SIZES_TYPE, flush the union
 * - saves little space and makes debugging very difficult ... additional 
 * problems.
 *
 * 15-Oct-90 Christopher Fedor at School of Computer Science, CMU
 * Moved x_ipcRegisterLengthFormatter and x_ipcRegisterNamedFormatter
 * to module routines.
 *
 * 18-Jul-90 Reid Simmons at School of Computer Science, CMU
 * Added code for freeing data structures.
 *
 * 10-Apr-90 Christopher Fedor at School of Computer Science, CMU
 * Changed ifdef SUN3 to ifndef SUN4.
 *
 *  9-Apr-90 Reid Simmons at School of Computer Science, CMU
 * Modified "x_ipc_alignField" to work for both Sun3 and Sun4 (Sparc) versions.
 *
 *  2-Apr-90 Christopher Fedor at School of Computer Science, CMU
 * Revised to software standards.
 *
 * 11-Mar-89 Christopher Fedor at School of Computer Science, CMU
 * Added x_ipcRegisterLengthFormatter
 *
 *  9-Feb-89 Reid Simmons at School of Computer Science, CMU
 * Split into 2 files -- "PrimFmttrs" and "Formatters"; changed how 
 * structured formatters are represented.  Added parser to take
 * formatter string and create TC_FORMAT_PTR structures.
 * INCOMPATIBLE CHANGE WITH MAJOR VERSION 1.x
 *
 *  6-Feb-89 Reid Simmons at School of Computer Science, CMU
 * Added structured formatters.
 *
 *  3-Feb-89 Reid Simmons at School of Computer Science, CMU
 * Standardized C and LISP interface to formatter functions.
 *
 *  2-Jan-89 Reid Simmons at School of Computer Science, CMU
 * (1.1) Added INT and FLOAT formats for LISP interface.
 *
 *    Dec-88 Christopher Fedor at School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.8 $
 * $Date: 2009/09/04 19:12:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalM.h"

#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif


FORMAT_PTR newFormatter(void)
{
  FORMAT_PTR format = NEW(FORMAT_TYPE);
  format->structSize = NOT_CACHED;
  format->flatBufferSize = NOT_CACHED;
  format->fixedSize = (BOOLEAN)NOT_CACHED;
  return format;
}

FORMAT_PTR x_ipc_createIntegerFormat(FORMAT_CLASS_TYPE type, int32 integer)
{ 
  FORMAT_PTR format;
  
  format = NEW_FORMATTER();
  format->type = type;
  format->formatter.i = integer;
  return format;
}

static FORMAT_PTR x_ipc_createPrimitiveFormat(int32 format_number)
{ 
  return x_ipc_createIntegerFormat(PrimitiveFMT, format_number);
}

static int32 x_ipc_formatterHashFN(char *key)
{ 
  int32 i, sum;
  
  for(i=0, sum=0; key[i] != '\0'; i++) 
    sum += key[i];
  return sum;
}

static int32 x_ipc_formatterEqFN(char *key1, char *key2)
{ 
  return(STREQ(key1, key2));
}

/* KeyLength is calculated assuming "name" is a null-terminated string */
void x_ipc_addFormatToTable(const char *name, CONST_FORMAT_PTR format)
{ 
  NAMED_FORMAT_PTR namedFormatter;
  
  namedFormatter = NEW(NAMED_FORMAT_TYPE);
  namedFormatter->definition = (char *)name;
  namedFormatter->format = (FORMAT_PTR)format;
  namedFormatter->parsed = TRUE;
  
  LOCK_M_MUTEX;
  x_ipc_hashTableInsert((void *)name, 1+strlen(name), 
			(void *)namedFormatter, GET_M_GLOBAL(formatNamesTable));
  UNLOCK_M_MUTEX;
}

/* Only parse named formatter when it is needed to be used 
   (see parseFmtters.c) */ 
void x_ipc_addFormatStringToTable(char *name, char *formatString)
{ 
  NAMED_FORMAT_PTR namedFormatter, oldNamedFormatter;
  CONST_FORMAT_PTR format;
  
  namedFormatter = NEW(NAMED_FORMAT_TYPE);
  namedFormatter->definition = formatString;
  namedFormatter->format = NULL;
  namedFormatter->parsed = FALSE;
  
  LOCK_M_MUTEX;
  oldNamedFormatter = 
    (NAMED_FORMAT_PTR)x_ipc_hashTableInsert((void *)name, 1+strlen(name), 
					    (void *)namedFormatter,
					    GET_M_GLOBAL(formatNamesTable));
  UNLOCK_M_MUTEX;
  if (oldNamedFormatter) {
    x_ipcFree(oldNamedFormatter->definition);
    if (oldNamedFormatter->format) {
      format = oldNamedFormatter->format;
      x_ipc_freeFormatter(&format);
      oldNamedFormatter->format = NULL;
    }
    x_ipcFree((void *)oldNamedFormatter);
  }
}

/*****************************************************************************
 *
 * FUNCTION: void x_ipc_registerPrimitiveFormat(formatter, formatNumber, FMT_Encode,
 *				          FMT_Decode, FMT_ELength, FMT_ALength, 
 *                                        FMT_RLength, FMT_SimpleType,
 *				          FMT_DPrint, FMT_DFree)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *  char *formatter;
 *  int formatNumber;
 *  TRANSLATE_FN_ENCODE FMT_Encode;
 *  TRANSLATE_FN_DECODE FMT_Decode;
 *  TRANSLATE_FN_ELENGTH FMT_ELength;
 *  TRANSLATE_FN_ALENGTH FMT_ALength; 
 *  TRANSLATE_FN_RLENGTH FMT_RLength;
 *  Boolean FMT_SimpleType;
 *  TRANSLATE_FN_DPRINT FMT_DPrint;
 *  TRANSLATE_FN_DFREE FMT_DFree;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void x_ipc_registerPrimitiveFormat(char *formatter, int32 formatNumber,
					  TRANSLATE_FN_ENCODE FMT_Encode,
					  TRANSLATE_FN_DECODE FMT_Decode,
					  TRANSLATE_FN_ELENGTH FMT_ELength,
					  TRANSLATE_FN_ALENGTH FMT_ALength, 
					  TRANSLATE_FN_RLENGTH FMT_RLength,
					  BOOLEAN FMT_SimpleType,
					  TRANSLATE_FN_DPRINT FMT_DPrint,
					  TRANSLATE_FN_DFREE FMT_DFree)
{ 
  LOCK_M_MUTEX;
  if ((formatNumber < 1) || (formatNumber > MAXFORMATTERS)) {
    X_IPC_MOD_ERROR2("Internal Error: RegisterPrimitiveFormatter: 0 < formatter number: %d < %d\n",
		formatNumber, MAXFORMATTERS+1);
    return;
  } else if (GET_M_GLOBAL(TransTable)[formatNumber].Encode !=  NULL) {
    X_IPC_MOD_ERROR1("Internal Error: RegisterFormatter: Formatter number %d already in use", 
		formatNumber);
    return;
  } else {
    GET_M_GLOBAL(TransTable)[formatNumber].Encode = FMT_Encode;
    GET_M_GLOBAL(TransTable)[formatNumber].Decode = FMT_Decode;
    GET_M_GLOBAL(TransTable)[formatNumber].ELength = FMT_ELength;
    GET_M_GLOBAL(TransTable)[formatNumber].ALength = FMT_ALength;
    GET_M_GLOBAL(TransTable)[formatNumber].RLength = FMT_RLength;
    GET_M_GLOBAL(TransTable)[formatNumber].SimpleType = FMT_SimpleType;
    GET_M_GLOBAL(TransTable)[formatNumber].DPrint = FMT_DPrint;
    GET_M_GLOBAL(TransTable)[formatNumber].DFree = FMT_DFree;
    x_ipc_addFormatToTable(formatter, x_ipc_createPrimitiveFormat(formatNumber));
  }
  UNLOCK_M_MUTEX;
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipc_formatInitialize()
 *
 * DESCRIPTION:
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipc_formatInitialize(void)
{ 
  int32 i;
  
  LOCK_M_MUTEX;
  GET_M_GLOBAL(formatNamesTable) =
    x_ipc_hashTableCreate(51, (HASH_FN) x_ipc_formatterHashFN, 
			  (EQ_HASH_FN) x_ipc_formatterEqFN);
  
  for(i=0; i< MAXFORMATTERS; i++) 
    GET_M_GLOBAL(TransTable)[i].Encode = NULL;
  UNLOCK_M_MUTEX;
  
  x_ipc_registerPrimitiveFormat(INT_FMT_NAME, INT_FMT,
				x_ipc_INT_Trans_Encode,
				x_ipc_INT_Trans_Decode,
				x_ipc_INT_Trans_ELength,
				x_ipc_INT_Trans_ALength,
				x_ipc_INT_Trans_RLength,
				INT_TRANS_SIMPLETYPE,
				x_ipc_INT_Trans_DPrint,
				x_ipc_INT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(BOOLEAN_FMT_NAME, BOOLEAN_FMT,
				x_ipc_BOOLEAN_Trans_Encode,
				x_ipc_BOOLEAN_Trans_Decode,
				x_ipc_BOOLEAN_Trans_ELength,
				x_ipc_BOOLEAN_Trans_ALength,
				x_ipc_BOOLEAN_Trans_RLength,
				BOOLEAN_TRANS_SIMPLETYPE,
				x_ipc_BOOLEAN_Trans_DPrint,
				x_ipc_BOOLEAN_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(FLOAT_FMT_NAME, FLOAT_FMT,
				x_ipc_FLOAT_Trans_Encode,
				x_ipc_FLOAT_Trans_Decode,
				x_ipc_FLOAT_Trans_ELength,
				x_ipc_FLOAT_Trans_ALength,
				x_ipc_FLOAT_Trans_RLength,
				FLOAT_TRANS_SIMPLETYPE,
				x_ipc_FLOAT_Trans_DPrint,
				x_ipc_FLOAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(DOUBLE_FMT_NAME, DOUBLE_FMT,
				x_ipc_DOUBLE_Trans_Encode,
				x_ipc_DOUBLE_Trans_Decode,
				x_ipc_DOUBLE_Trans_ELength,
				x_ipc_DOUBLE_Trans_ALength,
				x_ipc_DOUBLE_Trans_RLength,
				DOUBLE_TRANS_SIMPLETYPE,
				x_ipc_DOUBLE_Trans_DPrint,
				x_ipc_DOUBLE_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(BYTE_FMT_NAME, BYTE_FMT,
				x_ipc_BYTE_Trans_Encode,
				x_ipc_BYTE_Trans_Decode,
				x_ipc_BYTE_Trans_ELength,
				x_ipc_BYTE_Trans_ALength,
				x_ipc_BYTE_Trans_RLength,
				BYTE_TRANS_SIMPLETYPE,
				x_ipc_BYTE_Trans_DPrint,
				x_ipc_BYTE_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(TWOBYTE_FMT_NAME, TWOBYTE_FMT,
				x_ipc_TWOBYTE_Trans_Encode,
				x_ipc_TWOBYTE_Trans_Decode,
				x_ipc_TWOBYTE_Trans_ELength,
				x_ipc_TWOBYTE_Trans_ALength,
				x_ipc_TWOBYTE_Trans_RLength,
				TWOBYTE_TRANS_SIMPLETYPE,
				x_ipc_TWOBYTE_Trans_DPrint,
				x_ipc_TWOBYTE_Trans_DFree
				);

  x_ipc_registerPrimitiveFormat(STR_FMT_NAME, STR_FMT,
				x_ipc_STR_Trans_Encode,
				x_ipc_STR_Trans_Decode,
				x_ipc_STR_Trans_ELength,
				x_ipc_STR_Trans_ALength,
				x_ipc_STR_Trans_RLength,
				STR_TRANS_SIMPLETYPE,
				x_ipc_STR_Trans_DPrint,
				x_ipc_STR_Trans_DFree
				);

  x_ipc_registerPrimitiveFormat(FORMAT_FMT_NAME, FORMAT_FMT,
				x_ipc_FORMAT_Trans_Encode,
				x_ipc_FORMAT_Trans_Decode,
				x_ipc_FORMAT_Trans_ELength,
				x_ipc_FORMAT_Trans_ALength,
				x_ipc_FORMAT_Trans_RLength,
				FORMAT_TRANS_SIMPLETYPE,
				x_ipc_FORMAT_Trans_DPrint,
				x_ipc_FORMAT_Trans_DFree
				);
  
  /* 
     Added 18-Aug-89, used in HERO LISP system. "ubyte" (unsigned byte) is
     treated the same as a regular C byte.
  */
  x_ipc_registerPrimitiveFormat(UBYTE_FMT_NAME, UBYTE_FMT,
				x_ipc_UBYTE_Trans_Encode,
				x_ipc_UBYTE_Trans_Decode,
				x_ipc_UBYTE_Trans_ELength,
				x_ipc_UBYTE_Trans_ALength,
				x_ipc_UBYTE_Trans_RLength,
				UBYTE_TRANS_SIMPLETYPE,
				x_ipc_UBYTE_Trans_DPrint,
				x_ipc_UBYTE_Trans_DFree
				);

#ifndef NMP_IPC /* NMP doesn't need all this baggage */
  x_ipc_registerPrimitiveFormat(UCMAT_FMT_NAME, UCMAT_FMT,
				UCMAT_Trans_Encode,
				UCMAT_Trans_Decode,
				UCMAT_Trans_ELength,
				UCMAT_Trans_ALength,
				UCMAT_Trans_RLength,
				UCMAT_TRANS_SIMPLETYPE,
				UCMAT_Trans_DPrint,
				UCMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(CMAT_FMT_NAME, CMAT_FMT,
				CMAT_Trans_Encode,
				CMAT_Trans_Decode,
				CMAT_Trans_ELength,
				CMAT_Trans_ALength,
				CMAT_Trans_RLength,
				CMAT_TRANS_SIMPLETYPE,
				CMAT_Trans_DPrint,
				CMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SMAT_FMT_NAME, SMAT_FMT,
				SMAT_Trans_Encode,
				SMAT_Trans_Decode,
				SMAT_Trans_ELength,
				SMAT_Trans_ALength,
				SMAT_Trans_RLength,
				SMAT_TRANS_SIMPLETYPE,
				SMAT_Trans_DPrint,
				SMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(IMAT_FMT_NAME, IMAT_FMT,
				IMAT_Trans_Encode,
				IMAT_Trans_Decode,
				IMAT_Trans_ELength,
				IMAT_Trans_ALength,
				IMAT_Trans_RLength,
				IMAT_TRANS_SIMPLETYPE,
				IMAT_Trans_DPrint,
				IMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(LMAT_FMT_NAME, LMAT_FMT,
				LMAT_Trans_Encode,
				LMAT_Trans_Decode,
				LMAT_Trans_ELength,
				LMAT_Trans_ALength,
				LMAT_Trans_RLength,
				LMAT_TRANS_SIMPLETYPE,
				LMAT_Trans_DPrint,
				LMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(FMAT_FMT_NAME, FMAT_FMT,
				FMAT_Trans_Encode,
				FMAT_Trans_Decode,
				FMAT_Trans_ELength,
				FMAT_Trans_ALength,
				FMAT_Trans_RLength,
				FMAT_TRANS_SIMPLETYPE,
				FMAT_Trans_DPrint,
				FMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(DMAT_FMT_NAME, DMAT_FMT,
				DMAT_Trans_Encode,
				DMAT_Trans_Decode,
				DMAT_Trans_ELength,
				DMAT_Trans_ALength,
				DMAT_Trans_RLength,
				DMAT_TRANS_SIMPLETYPE,
				DMAT_Trans_DPrint,
				DMAT_Trans_DFree
				);
  
#if !(defined(THINK_C) || defined(__TURBOC__))
  x_ipc_registerPrimitiveFormat(SIUCMAT_FMT_NAME, SIUCMAT_FMT,
				SIUCMAT_Trans_Encode,
				SIUCMAT_Trans_Decode,
				SIUCMAT_Trans_ELength,
				SIUCMAT_Trans_ALength,
				SIUCMAT_Trans_RLength,
				SIUCMAT_TRANS_SIMPLETYPE,
				SIUCMAT_Trans_DPrint,
				SIUCMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SICMAT_FMT_NAME, SICMAT_FMT,
				SICMAT_Trans_Encode,
				SICMAT_Trans_Decode,
				SICMAT_Trans_ELength,
				SICMAT_Trans_ALength,
				SICMAT_Trans_RLength,
				SICMAT_TRANS_SIMPLETYPE,
				SICMAT_Trans_DPrint,
				SICMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SISMAT_FMT_NAME, SISMAT_FMT,
				SISMAT_Trans_Encode,
				SISMAT_Trans_Decode,
				SISMAT_Trans_ELength,
				SISMAT_Trans_ALength,
				SISMAT_Trans_RLength,
				SISMAT_TRANS_SIMPLETYPE,
				SISMAT_Trans_DPrint,
				SISMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SIIMAT_FMT_NAME, SIIMAT_FMT,
				SIIMAT_Trans_Encode,
				SIIMAT_Trans_Decode,
				SIIMAT_Trans_ELength,
				SIIMAT_Trans_ALength,
				SIIMAT_Trans_RLength,
				SIIMAT_TRANS_SIMPLETYPE,
				SIIMAT_Trans_DPrint,
				SIIMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SILMAT_FMT_NAME, SILMAT_FMT,
				SILMAT_Trans_Encode,
				SILMAT_Trans_Decode,
				SILMAT_Trans_ELength,
				SILMAT_Trans_ALength,
				SILMAT_Trans_RLength,
				SILMAT_TRANS_SIMPLETYPE,
				SILMAT_Trans_DPrint,
				SILMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SIFMAT_FMT_NAME, SIFMAT_FMT,
				SIFMAT_Trans_Encode,
				SIFMAT_Trans_Decode,
				SIFMAT_Trans_ELength,
				SIFMAT_Trans_ALength,
				SIFMAT_Trans_RLength,
				SIFMAT_TRANS_SIMPLETYPE,
				SIFMAT_Trans_DPrint,
				SIFMAT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SIDMAT_FMT_NAME, SIDMAT_FMT,
				SIDMAT_Trans_Encode,
				SIDMAT_Trans_Decode,
				SIDMAT_Trans_ELength,
				SIDMAT_Trans_ALength,
				SIDMAT_Trans_RLength,
				SIDMAT_TRANS_SIMPLETYPE,
				SIDMAT_Trans_DPrint,
				SIDMAT_Trans_DFree
				);
#endif
#endif /* NMP_IPC */

  x_ipc_registerPrimitiveFormat(CHAR_FMT_NAME, CHAR_FMT,
				x_ipc_CHAR_Trans_Encode,
				x_ipc_CHAR_Trans_Decode,
				x_ipc_CHAR_Trans_ELength,
				x_ipc_CHAR_Trans_ALength,
				x_ipc_CHAR_Trans_RLength,
				CHAR_TRANS_SIMPLETYPE,
				x_ipc_CHAR_Trans_DPrint,
				x_ipc_CHAR_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(SHORT_FMT_NAME, SHORT_FMT,
				x_ipc_SHORT_Trans_Encode,
				x_ipc_SHORT_Trans_Decode,
				x_ipc_SHORT_Trans_ELength,
				x_ipc_SHORT_Trans_ALength,
				x_ipc_SHORT_Trans_RLength,
				SHORT_TRANS_SIMPLETYPE,
				x_ipc_SHORT_Trans_DPrint,
				x_ipc_SHORT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(LONG_FMT_NAME, LONG_FMT,
				x_ipc_LONG_Trans_Encode,
				x_ipc_LONG_Trans_Decode,
				x_ipc_LONG_Trans_ELength,
				x_ipc_LONG_Trans_ALength,
				x_ipc_LONG_Trans_RLength,
				LONG_TRANS_SIMPLETYPE,
				x_ipc_LONG_Trans_DPrint,
				x_ipc_LONG_Trans_DFree
				);
  
  x_ipc_registerPrimitiveFormat(X_IPC_REF_PTR_FMT_NAME, X_IPC_REF_PTR_FMT,
				X_IPC_REF_PTR_Trans_Encode,
				X_IPC_REF_PTR_Trans_Decode,
				X_IPC_REF_PTR_Trans_ELength,
				X_IPC_REF_PTR_Trans_ALength,
				X_IPC_REF_PTR_Trans_RLength,
				X_IPC_REF_PTR_TRANS_SIMPLETYPE,
				X_IPC_REF_PTR_Trans_DPrint,
				X_IPC_REF_PTR_Trans_DFree
				);

  /*****************************************************************
   * In C the unsigned versions of the formatters behave just like the 
   *   signed versions (except for printing).  The main difference is
   *   in the Lisp version.
   ****************************************************************/

  x_ipc_registerPrimitiveFormat(UINT_FMT_NAME, UINT_FMT,
				x_ipc_INT_Trans_Encode,
				x_ipc_INT_Trans_Decode,
				x_ipc_INT_Trans_ELength,
				x_ipc_INT_Trans_ALength,
				x_ipc_INT_Trans_RLength,
				INT_TRANS_SIMPLETYPE,
				UINT_Trans_DPrint,
				x_ipc_INT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(USHORT_FMT_NAME, USHORT_FMT,
				x_ipc_SHORT_Trans_Encode,
				x_ipc_SHORT_Trans_Decode,
				x_ipc_SHORT_Trans_ELength,
				x_ipc_SHORT_Trans_ALength,
				x_ipc_SHORT_Trans_RLength,
				SHORT_TRANS_SIMPLETYPE,
				USHORT_Trans_DPrint,
				x_ipc_SHORT_Trans_DFree
				);
  x_ipc_registerPrimitiveFormat(ULONG_FMT_NAME, ULONG_FMT,
				x_ipc_LONG_Trans_Encode,
				x_ipc_LONG_Trans_Decode,
				x_ipc_LONG_Trans_ELength,
				x_ipc_LONG_Trans_ALength,
				x_ipc_LONG_Trans_RLength,
				LONG_TRANS_SIMPLETYPE,
				ULONG_Trans_DPrint,
				x_ipc_LONG_Trans_DFree
				);
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipc_formatFreeEntry(char *name, NAMED_FORMAT_PTR namedFormatter)
 *
 * DESCRIPTION:
 *
 * INPUTS: none.
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipc_formatFreeEntry(char *name, NAMED_FORMAT_PTR namedFormatter)
{
#ifdef UNUSED_PRAGMA
#pragma unused(name)
#endif
  CONST_FORMAT_PTR format;

  if (namedFormatter) {
    if (namedFormatter->format) {
      format = namedFormatter->format;
      x_ipc_freeFormatter(&format);
      namedFormatter->format = NULL;
    }
    x_ipcFree((char *)namedFormatter);
  }
}

/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_fixedArraySize(formatArray) 
 *
 * DESCRIPTION:
 * First element in a FORMAT_ARRAY_PTR is the size of the array.
 * For a FixedArrayFMT (and VarArrayFMT) the second element is the 
 * formatter, the rest of the elements are the dimensions.
 *
 * INPUTS: FORMAT_ARRAY_PTR formatArray;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

static int32 x_ipc_fixedArraySize(FORMAT_ARRAY_PTR formatArray)
{
  int32 i, arraySize;
  
  for(i=3, arraySize=formatArray[2].i; i<formatArray[0].i; i++)
    arraySize *= formatArray[i].i;
  return arraySize;
}


/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_varArraySize(formatArray, parentFormat, dataStruct, dStart)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * FORMAT_ARRAY_PTR formatArray; 
 * FORMAT_PTR parentFormat;
 * void *dataStruct; 
 * int32 dStart;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

/* RTG: The method of finding the size of the variable array was not quite 
 * correct.  Because of the way padding can be added before the pointer
 * to the variable array, just backing up or going forward the minimum
 * number of bytes is not always valid.  This is more apparent when running
 * on an alpha because pointers and ints are different sizes.
 */

static int32 x_ipc_varArraySize(FORMAT_ARRAY_PTR formatArray, 
			  CONST_FORMAT_PTR parentFormat,
			  CONST_GENERIC_DATA_PTR dataStruct, int32 dStart)
{ 
  int32 i,j, arraySize=1, size=0;
  FORMAT_ARRAY_PTR parentStructArray;
  int32 currentPlace, sizePlace, foundPlace;
  
  int32 sizeOffset=0, currentOffset=0, offset=0;
  int32 dataSize;
  
  parentStructArray = parentFormat->formatter.a;
  foundPlace = 0;
  for (currentPlace=1; !foundPlace; currentPlace++)
    foundPlace = ((parentStructArray[currentPlace].f->type == VarArrayFMT) &&
		  (parentStructArray[currentPlace].f->formatter.a
		   == formatArray));
  currentPlace--;
  
  /* need to find the index of the byte of sizePlace and of currentPlace
   * then subtract the two to find the required offset.
   */
  
  for (i=2; i<formatArray[0].i; i++) {
    sizePlace = formatArray[i].i;
    offset = 0;
    sizeOffset = 0;
    j=1;
    while((j <= sizePlace) || (j <= currentPlace)) {
      switch (parentStructArray[j].f->type) {
      case VarArrayFMT:
	dataSize = sizeof(GENERIC_DATA_PTR);
	break;
      default:
	dataSize = x_ipc_dataStructureSize(parentStructArray[j].f);
	break;
      }
      offset += dataSize;
      offset = x_ipc_alignField(parentFormat,j,offset);
      j++;
      if (j <= sizePlace) {
	sizeOffset = offset;
      }
      if (j <= currentPlace) {
	currentOffset = offset;
      }
    }
    
    offset = sizeOffset - currentOffset;
    
    BCOPY(dataStruct+dStart+offset, &size, sizeof(int32));
    arraySize *= size;
  }
  return arraySize;
}

/* Find the max value of the enum, and see how the compiler treats it */
/* Assumes enum values are >= 0 */
static int32 x_ipc_enumSize (CONST_FORMAT_PTR enumFormat)
{
  int32 maxVal = ENUM_MAX_VAL(enumFormat);

  return (maxVal <= MAX_BYTE ? BYTE_ENUM_SIZE 
	  : maxVal <= MAX_SHORT ? SHORT_ENUM_SIZE : INT_ENUM_SIZE);
}

/* Convert the dataStruct enum to an int32; 
   Returns the int32 and updates DStart */
int32 x_ipc_enumToInt (CONST_FORMAT_PTR format,
		 CONST_GENERIC_DATA_PTR dataStruct, int32 *DStart)
{
  int32 eSize, eVal;

  dataStruct += (*DStart);
  eSize = x_ipc_enumSize(format);
  switch (eSize) {
  case 1: eVal = (int32)(*(char *)dataStruct); break;
  case 2: eVal = (int32)(*(short *)dataStruct); break;
  default: eVal = (int32)(*(int32 *)dataStruct); break;
  }
  *DStart += eSize;
  return eVal;
}

/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_elementSize(format) 
 *
 * DESCRIPTION:
 * Returns the size (ALength) of the format's element.
 * If the format is a structured type, returns 0 unless all the elements
 * have the same length.
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

static int32 x_ipc_elementSize(CONST_FORMAT_PTR format)
{
  int32 firstSize, i;
  
  switch (format->type) {
  case LengthFMT: 
    return format->formatter.i;
  case PrimitiveFMT: {
    TRANSLATE_FN_ALENGTH trans;
    LOCK_M_MUTEX;
    trans = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
    UNLOCK_M_MUTEX;
    return (*trans)();
  }
  case PointerFMT:
  case VarArrayFMT: 
    return sizeof(GENERIC_DATA_PTR);
  case FixedArrayFMT: 
    return x_ipc_elementSize(format->formatter.a[1].f);
  case StructFMT:
    firstSize = x_ipc_elementSize(format->formatter.a[1].f);
    if (firstSize != 0) {
      for (i=2; i<format->formatter.a[0].i; i++) {
	if (firstSize != x_ipc_elementSize(format->formatter.a[i].f))
	  return 0;
      }
    }
    return firstSize;
  case NamedFMT:
    return x_ipc_elementSize(x_ipc_fmtFind(format->formatter.name));
    break;
  case BadFormatFMT: 
    return 0;
  case EnumFMT:
    return x_ipc_enumSize(format);

#ifndef TEST_CASE_COVERAGE
  default:
    /* NOT REACHED */
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_elementSize %d",format->type);
    break;
#endif
  }
  return 0;
}


/*****************************************************************************
 *
 * FUNCTION: int32 fixedLengthFormat(format) 
 *
 * DESCRIPTION:
 * Returns TRUE (1) if the Format contains no pointers.
 * (i.e., it is composed only of simple types).
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/
#if 0
static int32 fixedLengthFormat(CONST_FORMAT_PTR format)
{ 
  int32 i, subelementSize=0;
  
  switch(format->type) {
  case BadFormatFMT: 
  case LengthFMT: 
    return TRUE;
  case PrimitiveFMT: {
    BOOLEAN result;
    LOCK_M_MUTEX;
    result = GET_M_GLOBAL(TransTable)[format->formatter.i].SimpleType;
    UNLOCK_M_MUTEX;
    return result;
  }
  case PointerFMT:
  case VarArrayFMT: 
    return FALSE;
  case FixedArrayFMT: 
    return fixedLengthFormat(format->formatter.a[1].f);
  case StructFMT:
    /* For compatibility between Sun3's and Sun4's, in addition to all
     *  being simple types, the subelements of a structure must be the
     *  same size in order for the structure to be considered "fixed length" 
     */
    for (i=1; i<format->formatter.a[0].i; i++) {
      if (!fixedLengthFormat(format->formatter.a[i].f)) {
	return FALSE;
      }
      else if (i==1) { /* First element */
	subelementSize = x_ipc_elementSize(format->formatter.a[1].f);
	if (subelementSize == 0) return FALSE;
      }
      else if (subelementSize != x_ipc_elementSize(format->formatter.a[i].f))
	return FALSE;
    }
    return TRUE;
  case NamedFMT:
    return fixedLengthFormat(x_ipc_fmtFind(format->formatter.name));
  case EnumFMT:
    return TRUE;

#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown fixedLengthFormat Type %d",format->type);
    break;
    /*NOTREACHED*/
#endif
  }
  return 0;
}
#endif

/*****************************************************************************
 *
 * FUNCTION: BOOLEAN formatContainsPointers(format) 
 *
 * DESCRIPTION:
 * Returns TRUE (1) if the Format contains any pointers (i.e., it is not flat)
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: BOOLEAN
 *
 *****************************************************************************/
BOOLEAN formatContainsPointers(CONST_FORMAT_PTR format)
{ 
  int32 i;

  switch(format->type) {
  case BadFormatFMT: 
  case LengthFMT: 
    return FALSE;
  case PrimitiveFMT: {
    BOOLEAN result;
    LOCK_M_MUTEX;
    result = !(GET_M_GLOBAL(TransTable)[format->formatter.i].SimpleType);
    UNLOCK_M_MUTEX;
    return result;
  }
  case PointerFMT:
  case VarArrayFMT: 
    return TRUE;
  case FixedArrayFMT: 
    return formatContainsPointers(format->formatter.a[1].f);
  case StructFMT:
    for (i=1; i<format->formatter.a[0].i; i++) {
      if (formatContainsPointers(format->formatter.a[i].f)) {
	return TRUE;
      }
    }
    return FALSE;
  case NamedFMT:
    return formatContainsPointers(x_ipc_fmtFind(format->formatter.name));
  case EnumFMT:
    return FALSE;
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown formatContainsPointers Type %d",format->type);
    break;
    /*NOTREACHED*/
#endif
  }
  return TRUE; /* Most general case */
}

/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_sameFixedSizeDataBuffer(format) 
 *
 * DESCRIPTION:
 * Returns TRUE (1) if the Format contains no pointers.
 * (i.e., it is composed only of simple types).
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

BOOLEAN x_ipc_sameFixedSizeDataBuffer(CONST_FORMAT_PTR format)
{ 
  int32 i, offset=0;

  if (format->fixedSize != (BOOLEAN)NOT_CACHED) {
    return format->fixedSize;
  }
  switch(format->type) {
  case BadFormatFMT: 
  case LengthFMT: 
    return TRUE;
  case PrimitiveFMT: {
    BOOLEAN result;
    LOCK_M_MUTEX;
    result = GET_M_GLOBAL(TransTable)[format->formatter.i].SimpleType;
    UNLOCK_M_MUTEX;
    return result;
  }
  case PointerFMT:
  case VarArrayFMT: 
    break;
  case FixedArrayFMT: 
    return x_ipc_sameFixedSizeDataBuffer(format->formatter.a[1].f);
  case StructFMT:
    for (i=1; i<format->formatter.a[0].i; i++) {
      if (!x_ipc_sameFixedSizeDataBuffer(format->formatter.a[i].f)) {
	return FALSE;
      }
      /* Need to find out if there is any padding needed.*/
      offset = offset + x_ipc_dataStructureSize(format->formatter.a[i].f);
      if (x_ipc_alignField(format, i, offset) != offset)
	return FALSE;
    }
    return TRUE;
  case NamedFMT:
    return x_ipc_sameFixedSizeDataBuffer(x_ipc_fmtFind(format->formatter.name));
  case EnumFMT: 
    /* Can use the enum directly if the compiler allocates a full int for it */
    return x_ipc_enumSize(format) == sizeof(int32);
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_sameFixedSizeDataBuffer Type %d",format->type);
    break;
#endif
  }
  return FALSE;
}

/*****************************************************************************
 *
 * FUNCTION: BOOLEAN formatsEqual(CONST_FORMAT_PTR format1, 
 *                                CONST_FORMAT_PTR format2)
 *
 * DESCRIPTION:  Returns TRUE (1) if the two formats are the same.
 *
 * INPUTS: CONST_FORMAT_PTR format1, format2;
 *
 * OUTPUTS: BOOLEAN
 *
 *****************************************************************************/

BOOLEAN formatsEqual(CONST_FORMAT_PTR format1, CONST_FORMAT_PTR format2)
{ 
  FORMAT_ARRAY_PTR formatArray1, formatArray2;
  int i;

  if (!format1 && !format2) {
    return TRUE;
  } else if ((format1 && !format2) || (!format1 && format2) ||
	     (format1->type != format2->type)) {
    return FALSE;
  } else {
    switch(format1->type) {
    case LengthFMT: 
      return (format1->formatter.i == format2->formatter.i);

    case PrimitiveFMT:
      return (format1->formatter.i == format2->formatter.i);

    case PointerFMT:
      return formatsEqual(format1->formatter.f, format2->formatter.f);

    case StructFMT:
      formatArray1 = format1->formatter.a;
      formatArray2 = format2->formatter.a;
      if (formatArray1[0].i != formatArray2[0].i) {
	return FALSE;
      } else {
	for (i=1; i<formatArray1[0].i; i++) {
	  if (!formatsEqual(formatArray1[i].f, formatArray2[i].f))
	    return FALSE;
	}
	return TRUE;
      }

    case FixedArrayFMT: 
    case VarArrayFMT: 
      formatArray1 = format1->formatter.a;
      formatArray2 = format2->formatter.a;
      if (formatArray1[0].i != formatArray2[0].i || 
	  !formatsEqual(formatArray1[1].f, formatArray2[1].f)) {
	return FALSE;
      } else {
	for (i=2; i<formatArray1[0].i; i++) {
	  if (formatArray1[i].i != formatArray2[i].i)
	    return FALSE;
	}
	return TRUE;
      }

    case NamedFMT:
      return STREQ(format1->formatter.name, format2->formatter.name);

    case EnumFMT: 
      formatArray1 = format1->formatter.a;
      formatArray2 = format2->formatter.a;
      if (formatArray1[0].i != formatArray2[0].i || 
	  formatArray1[1].i != formatArray2[1].i) {
	return FALSE;
      } else {
	for (i=2; i<formatArray1[0].i; i++) {
	  if (!STREQ(formatArray1[i].f->formatter.name, 
		     formatArray2[i].f->formatter.name))
	    return FALSE;
	}
	return TRUE;
      }

    case BadFormatFMT: 
      return FALSE;

#ifndef TEST_CASE_COVERAGE
    default:
      X_IPC_MOD_ERROR1("formatEqual: Unknown type %d", format1->type);
      return FALSE;
#endif
    }
  }
}

/*****************************************************************************
 *
 * FUNCTION: BOOLEAN x_ipc_canVectorize(format) 
 *
 * DESCRIPTION:
 * Returns TRUE (1) if the Format can be read using iovec's
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

static BOOLEAN x_ipc_canVectorize(CONST_FORMAT_PTR format)
{ 
  int32 i, offset=0;
  
  switch(format->type) {
  case LengthFMT:
    return TRUE;
  case PrimitiveFMT: {
    BOOLEAN result;
    LOCK_M_MUTEX;
    result = GET_M_GLOBAL(TransTable)[format->formatter.i].SimpleType;
    UNLOCK_M_MUTEX;
    return result;
  }
  case PointerFMT:
  case VarArrayFMT: 
    break;
  case FixedArrayFMT: 
    return x_ipc_canVectorize(format->formatter.a[1].f);
  case StructFMT:
    for (i=1; i<format->formatter.a[0].i; i++) {
      if (!x_ipc_canVectorize(format->formatter.a[i].f)) {
	return FALSE;
      }
      /* Need to find out if there is any padding needed.*/
      offset = offset + x_ipc_dataStructureSize(format->formatter.a[i].f);
      if (x_ipc_alignField(format, i, offset) != offset)
	return FALSE;
    }
    return TRUE;
  case NamedFMT:
    return x_ipc_canVectorize(x_ipc_fmtFind(format->formatter.name));
  case BadFormatFMT: 
    return FALSE;
  case EnumFMT: 
    /* Some compilers allocate less space for short enums -- thus, cannot
       send directly unless they are long enough to *always* be ints */
    return ENUM_MAX_VAL(format) > MAX_SHORT;
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_canVectorize Type %d",format->type);
    break;
#endif
  }
  return FALSE;
}


/*****************************************************************************
 *
 * FUNCTION: iovec *x_ipc_createVectorization(format,data,count) 
 *
 * DESCRIPTION:
 * Returns vector of pointers and lengths of data.
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

struct iovec *x_ipc_createVectorization(CONST_FORMAT_PTR format, 
				  const char *dataStruct,
				  char *buffer,
				  int32 msgTotal)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(format, dataStruct)
#endif
  struct iovec *tmpVec;
  
  /* Since only sameFixedSize formats can be vectorized. */
  
  tmpVec = (struct iovec *) x_ipcMalloc((2) * sizeof(struct iovec));
  
  tmpVec[0].iov_base = buffer;
/*  tmpVec[0].iov_len = x_ipc_bufferSize(format,dataStruct);*/
  tmpVec[0].iov_len = msgTotal;

  tmpVec[1].iov_base = NULL;
  tmpVec[1].iov_len = 0;
  
  return tmpVec;
}


/*****************************************************************************
 *
 * FUNCTION: iovec *x_ipc_copyVectorization(vec) 
 *
 * DESCRIPTION:
 * Returns vector of pointers and lengths of data.
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

struct iovec *x_ipc_copyVectorization(const struct iovec *vec, int32 space)
{ 
  struct iovec *tmpVec;
  int32 count,i;

  if (vec == NULL)
    return NULL;

  for (count=0; vec[count].iov_base != NULL; count++) {};
  
  tmpVec = (struct iovec *) x_ipcMalloc((count+1+space) * sizeof(struct iovec));
  
  for (i=0; i<=count; i++)
    tmpVec[i+space] = vec[i];
  
  return tmpVec;
}


/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_mostRestrictiveElement(format) 
 *
 * DESCRIPTION:
 * For sun4/pmax version: Returns the longest element (ALength) of the format.
 * For Mach/486 version: Returns the longest element (ALength) of the format,
 *                       up to length of integer (4-bytes).
 *
 * INPUTS: CONST_FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

#if ((IPC_ALIGN & ALIGN_LONGEST) || (IPC_ALIGN & ALIGN_INT) || (IPC_ALIGN & ALIGN_MAC_PPC)\
     || (IPC_ALIGN & ALIGN_ARM))
static int32 x_ipc_mostRestrictiveElement(CONST_FORMAT_PTR format)
{
  int32 maxSize=0;
  
  switch (format->type) {
  case LengthFMT: return format->formatter.i;
    
  case PrimitiveFMT:
    LOCK_M_MUTEX;
    maxSize = (GET_M_GLOBAL(TransTable)[format->formatter.i].RLength)();
    UNLOCK_M_MUTEX;
#if (IPC_ALIGN & ALIGN_LONGEST)
    return maxSize;
#elif (IPC_ALIGN & ALIGN_INT) || (IPC_ALIGN & ALIGN_MAC_PPC) || (IPC_ALIGN & ALIGN_ARM)
    return (maxSize < (int32)sizeof(int32) ? maxSize : sizeof(int32));
#else
    /* should never get here. */
#endif

  case PointerFMT:
  case VarArrayFMT: 
    return sizeof(GENERIC_DATA_PTR);
    
  case FixedArrayFMT: 
    return x_ipc_mostRestrictiveElement(format->formatter.a[1].f);
    
  case StructFMT:
#if (IPC_ALIGN & ALIGN_ARM)
    return (int32)sizeof(int32);
#else
    {
      int32 nextSize, i;
      maxSize = x_ipc_mostRestrictiveElement(format->formatter.a[1].f);
      for (i=2; i<format->formatter.a[0].i; i++) {
	nextSize = x_ipc_mostRestrictiveElement(format->formatter.a[i].f);
	if (nextSize > maxSize) maxSize = nextSize;
      }
#if (IPC_ALIGN & ALIGN_LONGEST)
      return maxSize;
#elif (IPC_ALIGN & ALIGN_INT) || (IPC_ALIGN & ALIGN_MAC_PPC)
      return (maxSize < (int32)sizeof(int32) ? maxSize : sizeof(int32));
#endif
    }
#endif
  case NamedFMT:
    return x_ipc_mostRestrictiveElement(x_ipc_fmtFind(format->formatter.name));
  case BadFormatFMT: 
    return sizeof(GENERIC_DATA_PTR);
  case EnumFMT:
    return x_ipc_enumSize(format);

#ifndef TEST_CASE_COVERAGE
  default:
    return 0;
#endif
  }
}
#endif

/*****************************************************************************
 *
 * FUNCTION: int32 firstElementSize(format) 
 *
 * DESCRIPTION:
 * For Macintosh PPC: Returns the Rlength of the first element of the format.
 *
 * INPUTS: CONST_FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

#if (IPC_ALIGN & ALIGN_MAC_PPC)
static int32 firstElementSize(CONST_FORMAT_PTR format)
{  
  switch (format->type) {
  case LengthFMT: return format->formatter.i;
    
  case PrimitiveFMT: {
    int32 result;
    LOCK_M_MUTEX;
    result = (GET_M_GLOBAL(TransTable)[format->formatter.i].RLength)();
    UNLOCK_M_MUTEX;
    return result;
  }
  case PointerFMT:
  case VarArrayFMT: 
    return sizeof(GENERIC_DATA_PTR);
    
  case FixedArrayFMT: 
    return firstElementSize(format->formatter.a[1].f);
    
  case StructFMT:
    return firstElementSize(format->formatter.a[1].f);
  case NamedFMT:
    return firstElementSize(x_ipc_fmtFind(format->formatter.name));
  case BadFormatFMT: 
    return sizeof(GENERIC_DATA_PTR);
  case EnumFMT:
    return x_ipc_enumSize(format);

#ifndef TEST_CASE_COVERAGE
  default:
    return 0;
#endif
  }
}
#endif

#if (IPC_ALIGN & ALIGN_WORD)
/* TRUE if the format is "simple" and the elements are odd-length bytes */
static int32 oddSimpleFormat (CONST_FORMAT_PTR format)
{
  switch (format->type) {
  case LengthFMT: return ODDPTR(format->formatter.i);

  case PrimitiveFMT: {
    TRANSLATE_FN_ALENGTH trans;
    LOCK_M_MUTEX;
    trans = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
    UNLOCK_M_MUTEX;
    return ODDPTR((*trans)());
  }

  case FixedArrayFMT: return oddSimpleFormat(format->formatter.a[1].f);

  default: return FALSE;
  }
}
#endif

/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_alignField(format, currentField, currentDataSize)
 *
 * DESCRIPTION:
 * Returns either "currentDataSize" or 1 + "currentDataSize", to reflect
 * how C would align fields in a structure.
 *
 * Sun3 version:
 * This function works on the (empirical) model that the C compiler used on the
 * Sun3s aligns fields in structures on word boundaries (unless the next 
 * field in the structure is an odd number of bytes).
 *
 * sun4/pmax version:
 * Fields must be aligned on the "appropriate" boundaries - e.g., ints on 4
 * byte boundaries, doubles on 8 byte boundaries.  Structures are padded at
 * the end to align with the most restrictive (longest) field in the structure.
 *
 * Mach/486 version:
 * Based on observation with the Mach cc compiler on 486 (and 386) machines.
 * Similar to sun4 version: Fields must be aligned on the "appropriate" 
 * boundaries, except the relevant boundaries are 1-byte (char), 
 * 2-bytes (short),
 * and 4-bytes (everything else).  Structures are padded at the end 
 * to align with the most restrictive (longest) field in the structure.
 *
 * INPUTS:
 * FORMAT_PTR format; 
 * int32 currentField, currentDataSize;
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

int32 x_ipc_alignField(CONST_FORMAT_PTR format, int32 currentField, 
		 int32 currentDataSize)
{ 
#if (IPC_ALIGN & ALIGN_WORD)
  int32 nextField;
  FORMAT_PTR nextFormat;
  FORMAT_ARRAY_PTR formatArray; 
  
  if (!ODDPTR(currentDataSize)) 
    return currentDataSize;
  else {
    formatArray = format->formatter.a;
    nextField = 1 + currentField;
    if (nextField == formatArray[0].i)
      /* end of structure */
      return 1+currentDataSize;
    else {
      nextFormat = formatArray[nextField].f;
      return (oddSimpleFormat(nextFormat) 
	      ? currentDataSize : currentDataSize + 1);
    }
  }
#elif ((IPC_ALIGN & ALIGN_LONGEST) | (IPC_ALIGN & ALIGN_INT) | (IPC_ALIGN & ALIGN_MAC_PPC)\
       | (IPC_ALIGN & ALIGN_ARM))
  int32 nextField, appropriateSize, rem;
  FORMAT_ARRAY_PTR formatArray;   
  
  formatArray = format->formatter.a;
  nextField = 1+currentField;
  if (nextField == formatArray[0].i) {
    /* end of structure; pad to appropriate boundary of longest subelement */
    appropriateSize = x_ipc_mostRestrictiveElement(format);
#if (IPC_ALIGN & ALIGN_MAC_PPC)
    /* CodeWarrior seems to have a strange padding rule: It uses ALIGN_INT,
       except that it pads to 8-byte boundaries if the first element in
       the structure is a double! */
    appropriateSize = MAX(appropriateSize, firstElementSize(format));
#endif
  }
  else {
    /* on Sparc (and Mach/486 machines), element must start on boundary
       compatible with size of largest element within the sub-structure */
    appropriateSize = x_ipc_mostRestrictiveElement(formatArray[nextField].f);
  }
  /* Round up to the next even multiple of "appropriateSize" */
  rem = currentDataSize % appropriateSize;
  if (rem != 0) 
    currentDataSize += appropriateSize - rem;
  
  return currentDataSize;
#else
  /* Should be an error. */
#endif
}
#if 0

/*****************************************************************************
 *
 * FUNCTION: FORMAT_PTR optimizeFormatter(format) 
 *
 * DESCRIPTION:
 * If the formatter (or any sub-formatter of it) is a fixed length format, 
 * replace the definition with the appropriate length format.
 * For example, "{{int, float}, string}" gets optimized to "{4, 4, string}"
 * and "{int, float, [int:3]}" gets optimized to "20".
 *
 * Returns the new formatter, or the original one if no change occurs.
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: FORMAT_PTR
 *
 *****************************************************************************/

static FORMAT_PTR optimizeFormatter(FORMAT_PTR format)
{ 
  int32 i;
  FORMAT_ARRAY_PTR formatArray;
  
  if (fixedLengthFormat(format))
    return x_ipc_createIntegerFormat(LengthFMT, x_ipc_dataStructureSize(format));
  else {
    switch(format->type) {
    case StructFMT: 
      formatArray = format->formatter.a;
      for(i=1;i < formatArray[0].i;i++) 
	formatArray[i].f = optimizeFormatter(formatArray[i].f);
      break;
    case PointerFMT:
      if (format->formatter.f)
	format->formatter.f = optimizeFormatter(format->formatter.f);
      break;
    case VarArrayFMT:
    case FixedArrayFMT: 
      format->formatter.a[1].f = optimizeFormatter(format->formatter.a[1].f);
      break;
    case PrimitiveFMT:
    case LengthFMT:
    case BadFormatFMT:
    case NamedFMT:
    case EnumFMT:
      break;
#ifndef TEST_CASE_COVERAGE
    default:
      break;
#endif
    }
    return format;
  }
#ifndef __sgi
  return NULL;
#endif
}
#endif

/*****************************************************************************
 *
 * FUNCTION: SIZES_TYPE x_ipc_bufferSize1(format, dataStruct, dStart, parentFormat)
 *
 * DESCRIPTION:
 * "ParentFormat" is needed by SelfPtr ("*!") and VarArray ("<..>") formatters.
 *  Both these formatters can only be embedded in a Struct format ("{...}").
 *
 * INPUTS:
 * CONST_FORMAT_PTR format, parentFormat;
 * GENERIC_DATA_PTR dataStruct; 
 * int32 dStart; 
 *
 * OUTPUTS: SIZES_TYPE
 *
 *****************************************************************************/

SIZES_TYPE x_ipc_bufferSize1(CONST_FORMAT_PTR format,
		       CONST_GENERIC_DATA_PTR dataStruct,
		       int32 dStart, 
		       CONST_FORMAT_PTR parentFormat)
{ 
  SIZES_TYPE sizes;
  TRANSLATE_FN_ELENGTH eLengthProc;
  TRANSLATE_FN_ALENGTH aLengthProc;
  GENERIC_DATA_PTR structPtr;
  CONST_FORMAT_PTR nextFormat;
  FORMAT_ARRAY_PTR formatArray;
  int32 i, x_ipc_bufferSize, currentData, arraySize, structStart, elements;

  /* For flat formats, the buffer size can be predetermined */
  if (format->flatBufferSize != NOT_CACHED) {
    sizes.buffer = format->flatBufferSize;
    sizes.data = x_ipc_dataStructureSize(format);
    return sizes;
  }
  
  x_ipc_bufferSize = 0;
  currentData = dStart;

  switch (format->type) {
  case LengthFMT:
    x_ipc_bufferSize += format->formatter.i;
    currentData += format->formatter.i;
    break;
  case PrimitiveFMT: {
    LOCK_M_MUTEX;
    eLengthProc = GET_M_GLOBAL(TransTable)[format->formatter.i].ELength;
    aLengthProc = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
    UNLOCK_M_MUTEX;
    x_ipc_bufferSize += (* eLengthProc)((char *)dataStruct, currentData);
    currentData += (* aLengthProc)();
    break;
  }
  case PointerFMT:
    structPtr = REF(GENERIC_DATA_PTR, dataStruct, currentData);
    x_ipc_bufferSize += sizeof(char);
    if (structPtr) { 
      nextFormat = CHOOSE_PTR_FORMAT(format, parentFormat);
      sizes = x_ipc_bufferSize1(nextFormat, structPtr, 0, (FORMAT_PTR)NULL);
      x_ipc_bufferSize += sizes.buffer;
    }
    currentData += sizeof(GENERIC_DATA_PTR);
    break;
  case StructFMT:
    formatArray = format->formatter.a;
    for(i=1;i < formatArray[0].i;i++) {
      sizes = x_ipc_bufferSize1(formatArray[i].f, dataStruct+dStart,
			  currentData-dStart, format);
      x_ipc_bufferSize += sizes.buffer;
      currentData = x_ipc_alignField(format, i, currentData+sizes.data);
    }
    break;
  case FixedArrayFMT:
    formatArray = format->formatter.a;
    arraySize = x_ipc_fixedArraySize(formatArray);
    nextFormat = formatArray[1].f;
    if (x_ipc_sameFixedSizeDataBuffer(nextFormat)) {
      elements = arraySize * x_ipc_dataStructureSize(nextFormat);
      x_ipc_bufferSize += elements;
      currentData += elements;
    }
    else {
      for(i=0;i < arraySize;i++) {
	sizes = x_ipc_bufferSize1(nextFormat, dataStruct, currentData, (FORMAT_PTR)NULL);
	x_ipc_bufferSize += sizes.buffer;
	currentData += sizes.data;
      }
    }
    break;
  case VarArrayFMT:
    formatArray = format->formatter.a;
    nextFormat = formatArray[1].f;
    arraySize = x_ipc_varArraySize(formatArray, parentFormat, 
			     dataStruct, currentData);
    x_ipc_bufferSize += sizeof(int32); /* for the size of the array */
    if (x_ipc_sameFixedSizeDataBuffer(nextFormat))
      x_ipc_bufferSize += arraySize * x_ipc_dataStructureSize(nextFormat);
    else {
      structPtr = REF(GENERIC_DATA_PTR, dataStruct, currentData);
      structStart = 0;
      if (structPtr != NULL) {
	for(i=0;i < arraySize;i++) {
	  sizes = x_ipc_bufferSize1(nextFormat, structPtr, structStart, (FORMAT_PTR)NULL);
	  x_ipc_bufferSize += sizes.buffer;
	  structStart += sizes.data;
	}
      }
    }
    /* skip over the pointer to the array */
    currentData += sizeof(GENERIC_DATA_PTR);
    break;
  case NamedFMT:
    return x_ipc_bufferSize1(x_ipc_fmtFind(format->formatter.name),
		       dataStruct, dStart, parentFormat);
  case BadFormatFMT: 
    break;
  case EnumFMT:
    x_ipc_bufferSize += sizeof(int32);
    currentData += x_ipc_enumSize(format);
    break;

#ifndef TEST_CASE_COVERAGE
  default: 
    X_IPC_MOD_ERROR1("Internal Error: Unknown Format Type %d",format->type);
    break;
#endif
  }
  
  sizes.buffer = x_ipc_bufferSize;
  sizes.data = currentData - dStart;
  return sizes;
}

/*****************************************************************************
 *
 * FUNCTION: int32 x_ipc_dataStructureSize(format) 
 *
 * DESCRIPTION:
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_dataStructureSize(CONST_FORMAT_PTR format)
{ 
  int32 i, size;
  FORMAT_ARRAY_PTR formatArray;
  
  size = 0;
  
  if (format) {
    if (format->structSize != NOT_CACHED) {
      return format->structSize;
    }
    switch(format->type) {
    case LengthFMT: 
      size += format->formatter.i; 
      break;
    case PrimitiveFMT: {
      TRANSLATE_FN_ALENGTH trans;
      LOCK_M_MUTEX;
      trans = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
      UNLOCK_M_MUTEX;
      size += (*trans)();
      break;
    }
    case PointerFMT: 
    case VarArrayFMT: 
      size += sizeof(GENERIC_DATA_PTR); 
      break;
    case StructFMT: 
      formatArray = format->formatter.a;
      for(i=1;i < formatArray[0].i;i++) 
	size = x_ipc_alignField(format, i, size+x_ipc_dataStructureSize(formatArray[i].f));
      break;
    case FixedArrayFMT:
      formatArray = format->formatter.a;
      size += x_ipc_fixedArraySize(formatArray) * 
	x_ipc_dataStructureSize(formatArray[1].f);
      break;
    case NamedFMT:
      return x_ipc_dataStructureSize(x_ipc_fmtFind(format->formatter.name));
    case BadFormatFMT: 
      break;
    case EnumFMT:
      return x_ipc_enumSize(format);
#ifndef TEST_CASE_COVERAGE
    default:
      X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_dataStructureSize Type %d",format->type);
      break;
#endif
    }
  }
  return size;
}


/*****************************************************************************
 *
 * FUNCTION: SIZES_TYPE x_ipc_transferToBuffer(format, dataStruct, dStart, buffer, 
 *                                       bStart, parentFormat)
 *
 * DESCRIPTION:
 *
 * INPUTS:
 * CONST_FORMAT_PTR format, parentFormat;
 * void *dataStruct; 
 * int32 dStart, bStart; 
 * char *buffer;
 *
 * OUTPUTS: SIZES_TYPE
 *
 *****************************************************************************/

static SIZES_TYPE x_ipc_transferToBuffer(CONST_FORMAT_PTR format,
				   CONST_GENERIC_DATA_PTR dataStruct,
				   int32 dStart,
				   char *buffer, int32 bStart,
				   CONST_FORMAT_PTR parentFormat)
{ 
  SIZES_TYPE sizes = {0,0};
  TRANSLATE_FN_ENCODE encodeProc;
  TRANSLATE_FN_ALENGTH aLengthProc;
  GENERIC_DATA_PTR structPtr;
  CONST_FORMAT_PTR nextFormat;
  FORMAT_ARRAY_PTR formatArray;
  int32 i, currentByte, currentData, structStart, arraySize;
  int32 elements;
  
  currentByte = bStart;
  currentData = dStart;
  
  if (format == BAD_FORMAT) return sizes;
  switch(format->type) {
  case LengthFMT:
    TO_BUFFER_AND_ADVANCE(dataStruct+currentData, buffer, currentByte, 
			  format->formatter.i);
    currentData += format->formatter.i;
    break;
  case PrimitiveFMT:
    LOCK_M_MUTEX;
    encodeProc = GET_M_GLOBAL(TransTable)[format->formatter.i].Encode;
    aLengthProc = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
    UNLOCK_M_MUTEX;
    currentByte += (* encodeProc)(dataStruct, currentData,
				  buffer, currentByte);
    currentData += (* aLengthProc)();
    break;
  case PointerFMT:
    structPtr = REF(GENERIC_DATA_PTR, dataStruct, currentData);
    /* Z means data, 0 means NULL*/
    buffer[currentByte] = (structPtr) ? 'Z' : '\0';
    
    currentByte += sizeof(char);
    currentData += sizeof(GENERIC_DATA_PTR);
    if (structPtr) { 
      nextFormat = CHOOSE_PTR_FORMAT(format, parentFormat);
      sizes = x_ipc_transferToBuffer(nextFormat, structPtr, 0, 
			       buffer, currentByte, (FORMAT_PTR)NULL);
      currentByte += sizes.buffer;
    }
    break;
  case StructFMT:
    if (x_ipc_sameFixedSizeDataBuffer(format)) {
      sizes.data = x_ipc_dataStructureSize(format);
      TO_BUFFER_AND_ADVANCE(dataStruct+currentData, buffer, currentByte, 
			    sizes.data);
      currentData += sizes.data;
    } else {
      formatArray = format->formatter.a;
      for(i=1;i < formatArray[0].i;i++) {
	sizes = x_ipc_transferToBuffer(formatArray[i].f, dataStruct+dStart,
				 currentData-dStart,
				 buffer, currentByte, format);
	currentByte += sizes.buffer;
	currentData = x_ipc_alignField(format, i, currentData+sizes.data);
      }
    }
    break;
  case FixedArrayFMT:
    formatArray = format->formatter.a;
    arraySize = x_ipc_fixedArraySize(formatArray);
    nextFormat = formatArray[1].f;
    if (x_ipc_sameFixedSizeDataBuffer(nextFormat)) {
      elements = arraySize * x_ipc_dataStructureSize(nextFormat);
      BCOPY(dataStruct+currentData, buffer+currentByte, elements);
      currentByte += elements;
      currentData += elements;
    } else {
      for (i=0;i < arraySize;i++) {
	sizes = x_ipc_transferToBuffer(nextFormat, dataStruct, currentData,
				 buffer, currentByte, (FORMAT_PTR)NULL);
	currentByte += sizes.buffer;
	currentData += sizes.data;
      }
    }
    break;
  case VarArrayFMT:
    formatArray = format->formatter.a;
    arraySize = x_ipc_varArraySize(formatArray, parentFormat,
			     dataStruct, currentData);
    nextFormat = formatArray[1].f;
    
    intToNetBytes(arraySize, buffer+currentByte);
    currentByte += sizeof(int32);
    structPtr = REF(GENERIC_DATA_PTR, dataStruct, currentData);
    if (x_ipc_sameFixedSizeDataBuffer(nextFormat)) {
      elements = arraySize * x_ipc_dataStructureSize(nextFormat);
      BCOPY(structPtr, buffer+currentByte, elements);
      currentByte += elements;
    } else {
      structStart = 0;
      for (i=0;i < arraySize;i++) {
	sizes = x_ipc_transferToBuffer(nextFormat, structPtr, structStart,
				 buffer, currentByte, (FORMAT_PTR)NULL);
	currentByte += sizes.buffer;
	structStart += sizes.data;
      }
    }
    currentData += sizeof(GENERIC_DATA_PTR);
    break;
  case NamedFMT:
    return x_ipc_transferToBuffer(x_ipc_fmtFind(format->formatter.name),
			    dataStruct, dStart, buffer, bStart, parentFormat);
  case BadFormatFMT: 
    break;
  case EnumFMT:
    { int32 eVal;
      eVal = x_ipc_enumToInt(format, dataStruct, &currentData);
      currentByte += x_ipc_INT_Trans_Encode((CONST_GENERIC_DATA_PTR)&eVal, 0,
				      buffer, currentByte);
      break;
    }
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_transferToBuffer Type %d",format->type);
    break;
#endif
  }
  
  sizes.buffer = currentByte - bStart;
  sizes.data = currentData - dStart;
  return sizes;  
}

/*****************************************************************************
 *
 * FUNCTION: 
 *
 * DESCRIPTION:
 *
 * INPUTS:
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static SIZES_TYPE x_ipc_transferToDataStructure(CONST_FORMAT_PTR format, 
					  GENERIC_DATA_PTR dataStruct,
					  int32 dStart, char *buffer,
					  int32 bStart,
					  CONST_FORMAT_PTR parentFormat,
					  int32 byteOrder,
					  ALIGNMENT_TYPE alignment
					  )
{
  SIZES_TYPE sizes;
  TRANSLATE_FN_DECODE decodeProc;
  TRANSLATE_FN_ALENGTH aLengthProc;
  char ptrVal;
  GENERIC_DATA_PTR newStruct;
  CONST_FORMAT_PTR nextFormat;
  FORMAT_ARRAY_PTR formatArray;
  int32 i, currentByte, currentData, structStart, arraySize;
  int32 elements;
  
  currentByte = bStart;
  currentData = dStart;
  
  switch(format->type) {
  case LengthFMT:
    FROM_BUFFER_AND_ADVANCE(dataStruct+currentData, buffer, currentByte, 
			    format->formatter.i);
    currentData += format->formatter.i;
    break;
  case PrimitiveFMT:
    LOCK_M_MUTEX;
    decodeProc = GET_M_GLOBAL(TransTable)[format->formatter.i].Decode;
    aLengthProc = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
    UNLOCK_M_MUTEX;
    currentByte += (* decodeProc)(dataStruct, currentData,
				  buffer, currentByte, byteOrder, alignment);
    currentData += (* aLengthProc)();
    break;
  case PointerFMT:
    FROM_BUFFER_AND_ADVANCE(&ptrVal, buffer, currentByte, sizeof(char));
    if (ptrVal == '\0') 
      newStruct = NULL;
    else {
      nextFormat = CHOOSE_PTR_FORMAT(format, parentFormat);    
      newStruct = (GENERIC_DATA_PTR)x_ipcMalloc((unsigned)
					      x_ipc_dataStructureSize(nextFormat));
      sizes = x_ipc_transferToDataStructure(nextFormat, newStruct, 0,
				      buffer, currentByte, (FORMAT_PTR)NULL,
				      byteOrder, alignment);
      currentByte += sizes.buffer;
    }
    TO_BUFFER_AND_ADVANCE(&newStruct, dataStruct, currentData,
			  sizeof(GENERIC_DATA_PTR));
    break;
  case StructFMT:
    if ((byteOrder == BYTE_ORDER) &&
	(x_ipc_sameFixedSizeDataBuffer(format))) {
      sizes.buffer = x_ipc_dataStructureSize(format);
      FROM_BUFFER_AND_ADVANCE(dataStruct+currentData, buffer, currentByte, 
			    sizes.buffer);
      currentData += sizes.buffer;
    } else {
      formatArray = format->formatter.a;
      for(i=1;i < formatArray[0].i;i++) {
	sizes = x_ipc_transferToDataStructure(formatArray[i].f, dataStruct+dStart,
					currentData-dStart, buffer,
					currentByte, format,
					byteOrder,alignment);
	currentByte += sizes.buffer;
	currentData = x_ipc_alignField(format, i, currentData+sizes.data);
      }
    }
    break;
  case FixedArrayFMT:
    formatArray = format->formatter.a;
    arraySize = x_ipc_fixedArraySize(formatArray);
    nextFormat = formatArray[1].f;
    if ((byteOrder == BYTE_ORDER) &&
	(x_ipc_sameFixedSizeDataBuffer(nextFormat))) {
      elements = arraySize * x_ipc_dataStructureSize(nextFormat);
      BCOPY(buffer+currentByte, dataStruct+currentData, elements);
      currentByte += elements;
      currentData += elements;
    } else {
      /* Arrays are often primitives. Inline the code for primitive types. */
      if (nextFormat->type == PrimitiveFMT) {
	int32 alength;
	BOOLEAN simple;
	LOCK_M_MUTEX;
	aLengthProc = GET_M_GLOBAL(TransTable)[nextFormat->formatter.i].ALength;
	decodeProc = GET_M_GLOBAL(TransTable)[nextFormat->formatter.i].Decode;
	simple = GET_M_GLOBAL(TransTable)[nextFormat->formatter.i].SimpleType;
	UNLOCK_M_MUTEX;
	if (simple) {
	  alength = (* aLengthProc)();
	  switch (alength) {
	  case sizeof(char): 
	    /* Characters, do not need to do anything but copy. */
	    {register char *bytes = (buffer+currentByte);
	     register char *datas = (dataStruct+currentData);
	     for(i=0;i < arraySize;i++) {
	       netBytesToChar((bytes),(char *)(datas));
	       bytes += sizeof(char);
	       datas += sizeof(char);
	     }
	     currentByte += sizeof(char) * arraySize;
	     currentData += sizeof(char) * arraySize;
	     break;
	   }
	  case sizeof(int16): /* Shorts, . */ {
	    if ((buffer+currentByte) == (dataStruct+currentData)) {
	      /* The copy is in place, just need to rearrange. */
	      register char *datas = 
		(char *)(dataStruct+currentData);
	      for(i=0; i<arraySize; i++) {
		netBytesToShort((datas),(int16 *)(datas));
		datas += sizeof(int16);
	      }
	    } else {
	      register char *bytes = (buffer+currentByte);
	      register char *datas = (dataStruct+currentData);
	      for(i=0;i < arraySize;i++) {
		netBytesToShort((bytes),(int16 *)(datas));
		bytes += sizeof(int16);
		datas += sizeof(int16);
	      }
	    }
	    currentByte += sizeof(int16) * arraySize;
	    currentData += sizeof(int16) * arraySize;
	    break;
	  }
	  case sizeof(int32): {
	    if ((buffer+currentByte) == (dataStruct+currentData)) {
	      /* The copy is in place, just need to rearrange. */
	      register char *datas = 
		(char *) (dataStruct+currentData);
	      for(i=0; i<arraySize; i++) {
		netBytesToInt((datas),(int32 *)(datas));
		datas += sizeof(int32);
	      }
	    } else {
	      register char *bytes = (buffer+currentByte);
	      register char *datas = (dataStruct+currentData);
	      for(i=0;i < arraySize;i++) {
		netBytesToInt((bytes),(int32 *)(datas));
		bytes += sizeof(int32);
		datas += sizeof(int32);
	      }
	    }
	    currentByte += sizeof(int32) * arraySize;
	    currentData += sizeof(int32) * arraySize;
	    break;
	  }
	  case sizeof(double): {
	    if ((buffer+currentByte) == (dataStruct+currentData)) {
	      /* The copy is in place, just need to rearrange. */
	      register char *datas = 
		(char *) (dataStruct+currentData);
	      for(i=0; i<arraySize; i++) {
		netBytesToDouble(datas,(double *)datas);
		datas += sizeof(double);
	      }
	    } else {
	      char *bytes = (buffer+currentByte);
	      char *datas = (dataStruct+currentData);
	      for(i=0;i < arraySize;i++) {
		netBytesToDouble((bytes),(double *)(datas));
		bytes += sizeof(double);
		datas += sizeof(double);
	      }
	    }
	    currentByte += sizeof(double) * arraySize;
	    currentData += sizeof(double) * arraySize;
	    break;
	  }
#ifndef TEST_CASE_COVERAGE
	  default:
	    X_IPC_MOD_ERROR1("Internal Error: Unhandled primitive element of size %d \n",
			alength);
#endif
	  }
	} else {
	  for(i=0;i < arraySize;i++) {
	    sizes.buffer = (* decodeProc)(dataStruct, currentData,
					  buffer, currentByte,
					  byteOrder, alignment);
	    currentData += (* aLengthProc)();
	    currentByte += sizes.buffer;
	  }
	}
      } else {
	for(i=0;i < arraySize;i++) {
	  sizes = x_ipc_transferToDataStructure(nextFormat, dataStruct,
					  currentData, buffer, currentByte,
					  (FORMAT_PTR)NULL,
					  byteOrder, alignment);
	  currentByte += sizes.buffer;
	  currentData += sizes.data;
	}
      }
    }
    break;
  case VarArrayFMT:
    formatArray = format->formatter.a;
    netBytesToInt(buffer+currentByte, &arraySize);
    currentByte += sizeof(int32);
    nextFormat = formatArray[1].f;
    newStruct = 
      ((arraySize == 0) ? NULL :
       (GENERIC_DATA_PTR)x_ipcMalloc((unsigned)(arraySize*
					      x_ipc_dataStructureSize(nextFormat))));
    TO_BUFFER_AND_ADVANCE(&newStruct, dataStruct, currentData, 
			  sizeof(GENERIC_DATA_PTR));
    if (newStruct) {
      if ((byteOrder == BYTE_ORDER) &&
	  (x_ipc_sameFixedSizeDataBuffer(nextFormat))) {
	elements = arraySize * x_ipc_dataStructureSize(nextFormat);
	BCOPY(buffer+currentByte, newStruct, elements);
	currentByte += elements;
      } else {
	structStart = 0;
	/* Arrays are often primitives. Inline the code for primitive types. */
	/* This could be done much more efficiently by just calling the 
	 * appropriate lower level function. 
	 */
	if (nextFormat->type == PrimitiveFMT) {
	  int32 alength;
	  BOOLEAN simple;
	  LOCK_M_MUTEX;
	  aLengthProc =
	    GET_M_GLOBAL(TransTable)[nextFormat->formatter.i].ALength;
	  decodeProc = GET_M_GLOBAL(TransTable)[nextFormat->formatter.i].Decode;
	  simple = GET_M_GLOBAL(TransTable)[nextFormat->formatter.i].SimpleType;
	  UNLOCK_M_MUTEX;
	  alength = (* aLengthProc)();
	  if (simple) {
	    alength = (* aLengthProc)();
	    switch (alength) {
	    case sizeof(char): 
	      /* Characters, do not need to do anything but copy. */
	      {register char *bytes = (buffer+currentByte);
	       register char *datas = (newStruct+structStart);
	       for(i=0;i < arraySize;i++) {
		 netBytesToChar((bytes),(char *)(datas));
		 bytes += sizeof(char);
		 datas += sizeof(char);
	       }
	       currentByte += sizeof(char) * arraySize;
	       structStart += sizeof(char) * arraySize;
	       break;
	     }
	    case sizeof(int16): /* Shorts, . */ {
	      if ((buffer+currentByte) == (newStruct+structStart)) {
		/* The copy is in place, just need to rearrange. */
		register char *datas = 
		  (char *)(newStruct+structStart);
		for(i=0; i<arraySize; i++) {
		  netBytesToShort((datas),(int16 *)(datas));
		  datas += sizeof(int16);
		}
	      } else {
		register char *bytes = (buffer+currentByte);
		register char *datas = (newStruct+structStart);
		for(i=0;i < arraySize;i++) {
		  netBytesToShort((bytes),(int16 *)(datas));
		  bytes += sizeof(int16);
		  datas += sizeof(int16);
		}
	      }
	      currentByte += sizeof(int16) * arraySize;
	      structStart += sizeof(int16) * arraySize;
	      break;
	    }
	    case sizeof(int32): {
	      if ((buffer+currentByte) == (newStruct+structStart)) {
		/* The copy is in place, just need to rearrange. */
		register char *datas = 
		  (char *) (newStruct+structStart);
		for(i=0; i<arraySize; i++) {
		  netBytesToInt((datas),(int32 *)(datas));
		  datas += sizeof(int32);
		}
	      } else {
		register char *bytes = (buffer+currentByte);
		register char *datas = (newStruct+structStart);
		for(i=0;i < arraySize;i++) {
		  netBytesToInt((bytes),(int32 *)(datas));
		  bytes += sizeof(int32);
		  datas += sizeof(int32);
		}
	      }
	      currentByte += sizeof(int32) * arraySize;
	      structStart += sizeof(int32) * arraySize;
	      break;
	    }
	    case sizeof(double): {
	      if ((buffer+currentByte) == (newStruct+structStart)) {
		/* The copy is in place, just need to rearrange. */
		register char *datas = 
		  (char *) (newStruct+structStart);
		for(i=0; i<arraySize; i++) {
		  netBytesToDouble((datas),(double *)(datas));
		  datas += sizeof(double);
		}
	      } else {
		char *bytes = (buffer+currentByte);
		char *datas = (newStruct+structStart);
		for(i=0;i < arraySize;i++) {
		  netBytesToDouble((bytes),(double *)(datas));
		  bytes += sizeof(double);
		  datas += sizeof(double);
		}
	      }
	      currentByte += sizeof(double) * arraySize;
	      structStart += sizeof(double) * arraySize;
	      break;
	    }
#ifndef TEST_CASE_COVERAGE
	    default:
	      X_IPC_MOD_ERROR1("Internal Error: Unhandled primitive element of size %d \n",
			  alength);
#endif
	    }
	  } else {
	    for(i=0;i < arraySize;i++) {
	      sizes.buffer = (* decodeProc)(newStruct, structStart,
					    buffer, currentByte,
					    byteOrder, alignment);
	      structStart += (* aLengthProc)();
	      currentByte += sizes.buffer;
	    }
	  }
	} else {
	  for(i=0;i < arraySize;i++) {
	    sizes = x_ipc_transferToDataStructure(nextFormat, newStruct,
					    structStart, buffer, currentByte,
					    (FORMAT_PTR)NULL,
					    byteOrder, alignment);
	    currentByte += sizes.buffer;
	    structStart += sizes.data;
	  }
	}
      }
    }
    break;
  case NamedFMT:
    return x_ipc_transferToDataStructure(x_ipc_fmtFind(format->formatter.name),
				   dataStruct, dStart, buffer, bStart,
				   parentFormat, byteOrder, alignment);
  case BadFormatFMT: 
    break;
  case EnumFMT:
    { int32 eSize, eVal;
      eSize = x_ipc_enumSize(format);
      currentByte += x_ipc_INT_Trans_Decode((GENERIC_DATA_PTR)&eVal, 0, buffer,
				      currentByte, byteOrder, alignment);
      switch (eSize) {
      case 1: *(char *)(dataStruct+currentData) = (char)eVal; break;
      case 2: *(short *)(dataStruct+currentData) = (short)eVal; break;
      default: *(int32 *)(dataStruct+currentData) = eVal; break;
      }
      currentData += eSize;
      break;
    }
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_transferToDataStructure Type %d",format->type);
    break;
#endif
  }
  
  sizes.buffer = currentByte - bStart;
  sizes.data = currentData - dStart;
  return sizes;  
}


/*************************************************************
  
  THESE FUNCTIONS FORM THE INTERFACE TO THE REST OF THE SYSTEM
  
  *************************************************************/

int32 x_ipc_bufferSize(CONST_FORMAT_PTR Format, const void *DataStruct)
{ 
  SIZES_TYPE sizes;
  
  if ((Format == NULL) || (Format == BAD_FORMAT))
    return 0;

  sizes = x_ipc_bufferSize1(Format, (CONST_GENERIC_DATA_PTR)DataStruct,
		      0, (FORMAT_PTR)NULL);
  return sizes.buffer;
}

/*************************************************************/

void x_ipc_encodeData(CONST_FORMAT_PTR Format, const void *DataStruct,
		      char *Buffer, int32 BStart, int32 x_ipc_bufferSize)
{
  SIZES_TYPE sizes;

  sizes = x_ipc_transferToBuffer(Format, (CONST_GENERIC_DATA_PTR)DataStruct,
				 0, Buffer, BStart, (FORMAT_PTR)NULL); 
  /* Sanity check */
  if (x_ipc_bufferSize != sizes.buffer) {
    X_IPC_MOD_ERROR2("Mismatch between buffer size (%d) and encoded data (%d)\n",
		     x_ipc_bufferSize, sizes.buffer);
  }
}

/*************************************************************/


void *x_ipc_decodeData(CONST_FORMAT_PTR Format, char *Buffer, int32 BStart, 
		 char *DataStruct,
		 int32 byteOrder, ALIGNMENT_TYPE alignment, int32 x_ipc_bufferSize)
{
  SIZES_TYPE sizes;
  int32 dataSize = -1;

  if (DataStruct == NULL) {
    dataSize = x_ipc_dataStructureSize(Format);
    DataStruct = (char *)x_ipcMalloc((unsigned)dataSize);
  }
  
  sizes = x_ipc_transferToDataStructure(Format, DataStruct, 0, Buffer, BStart,
				  (FORMAT_PTR)NULL, byteOrder, alignment);
  /* Sanity checks (the "-1" is for IPC to work) */
  if (x_ipc_bufferSize != -1 && x_ipc_bufferSize != sizes.buffer) {
    X_IPC_MOD_ERROR2("Mismatch between buffer size (%d) and decoded data (%d)\n",
		x_ipc_bufferSize, sizes.buffer);
  } else if (dataSize != -1 && dataSize != sizes.data) {
    X_IPC_MOD_ERROR2("Mismatch between structure size (%d) and decoded data (%d)\n",
		dataSize, sizes.data);
  }

  return DataStruct;
}


/*****************************************************************************
 *
 * FUNCTION: x_ipc_freeDataElements
 *
 * DESCRIPTION: Frees the data elements "malloc"ed by the equivalent call
 *              to "TransferToDataStructure"
 *
 * INPUTS: format of data structure
 *         pointer data structure itself
 *         start of relevant part of data structure
 *         format of parent data structure (or NULL)
 *
 * OUTPUTS: number of bytes processed in the top-level dataStruct.
 *
 *****************************************************************************/

int32 x_ipc_freeDataElements(CONST_FORMAT_PTR format,
			     GENERIC_DATA_PTR dataStruct,
			     int32 dStart, CONST_FORMAT_PTR parentFormat)
{ 
  TRANSLATE_FN_DFREE freeProc;
  TRANSLATE_FN_ALENGTH aLengthProc;
  GENERIC_DATA_PTR *structPtr;
  CONST_FORMAT_PTR nextFormat;
  FORMAT_ARRAY_PTR formatArray;
  int32 size, i, currentData, structStart, arraySize;
  
  currentData = dStart;
  
  if (format == BAD_FORMAT) return dStart;

  switch(format->type) {
  case LengthFMT:
    currentData += format->formatter.i;
    break;
    
  case PrimitiveFMT:
    LOCK_M_MUTEX;
    freeProc = GET_M_GLOBAL(TransTable)[format->formatter.i].DFree;
    aLengthProc = GET_M_GLOBAL(TransTable)[format->formatter.i].ALength;
    UNLOCK_M_MUTEX;
    (void)(* freeProc)(dataStruct, currentData);
    currentData += (* aLengthProc)();
    break;
    
  case PointerFMT:
    structPtr = &(REF(GENERIC_DATA_PTR, dataStruct, dStart));
    currentData += sizeof(GENERIC_DATA_PTR);
    if (*structPtr) {
      nextFormat = CHOOSE_PTR_FORMAT(format, parentFormat);    
      size = x_ipc_freeDataElements(nextFormat, *structPtr, 0, (FORMAT_PTR)NULL);
      x_ipcFree((char *)*structPtr);
      *structPtr = NULL;
    }
    break;
    
  case StructFMT:
    formatArray = format->formatter.a;
    for(i=1;i < formatArray[0].i;i++) {
      size = x_ipc_freeDataElements(formatArray[i].f, dataStruct+dStart,
			      currentData-dStart, format);
      currentData = x_ipc_alignField(format, i, currentData+size);
    }
    break;
    
  case FixedArrayFMT:
    formatArray = format->formatter.a;
    arraySize = x_ipc_fixedArraySize(formatArray);
    nextFormat = formatArray[1].f;
    if (x_ipc_sameFixedSizeDataBuffer(nextFormat)) {
      currentData += arraySize * x_ipc_dataStructureSize(nextFormat);
    }
    else {
      for(i=0;i < arraySize;i++) {
	size = x_ipc_freeDataElements(nextFormat, dataStruct, currentData,
				(FORMAT_PTR)NULL);
	currentData += size;
      }
    }
    break;
    
  case VarArrayFMT:
    structPtr = &(REF(GENERIC_DATA_PTR, dataStruct, currentData));
    if (*structPtr) {
      formatArray = format->formatter.a;
      arraySize = x_ipc_varArraySize(formatArray, parentFormat,
			       dataStruct, currentData);
      nextFormat = formatArray[1].f;
      if (!x_ipc_sameFixedSizeDataBuffer(nextFormat)) {
	structStart = 0;
	for(i=0;i < arraySize;i++) {
	  size = x_ipc_freeDataElements(nextFormat, *structPtr, structStart,
				  (FORMAT_PTR)NULL);
	  structStart += size;
	}
      }
      x_ipcFree((char *)*structPtr);
      *structPtr = NULL;
    }
    currentData += sizeof(void *);
    break;
  case NamedFMT:
    return x_ipc_freeDataElements(x_ipc_fmtFind(format->formatter.name),
			    dataStruct, dStart, parentFormat);
  case BadFormatFMT: 
    break;
  case EnumFMT:
    currentData += x_ipc_enumSize(format);
    break;

#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_freeDataElements Type %d",format->type);
    break;
#endif
  }
  
  return currentData - dStart;
}

/*************************************************************/

void x_ipc_freeDataStructure(CONST_FORMAT_PTR format, void *dataStruct)
{
  (void)x_ipc_freeDataElements(format, (GENERIC_DATA_PTR)dataStruct,
			 0, (FORMAT_PTR)NULL);
  
  x_ipcFree((char *)dataStruct);
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipcFreeData(msgName, dataStruct)
 *
 * DESCRIPTION: 
 * Using the queryForm format of the message, free the data structure
 * and all of its allocated subelements
 *
 * INPUTS: 
 * char *msgName; 
 * void *dataStruct;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void x_ipcFreeData(const char *msgName, void *dataStruct)
{
  MSG_PTR msg;
  
  if (msgName == NULL) {
    X_IPC_MOD_ERROR("ERROR: message name is NULL in call to x_ipcFreeData.");
  } else if (dataStruct) {
    
    msg = x_ipc_msgFind(msgName);
    if (msg == NULL) return;
    
    if (msg->msgData->msgFormat) {
      
      x_ipc_freeDataStructure(msg->msgData->msgFormat, (char *)dataStruct);
    }
  }
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipcAllocateReply(msgName)
 *
 * DESCRIPTION: 
 *
 * INPUTS:
 * char *msgName; 
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void *x_ipcAllocateReply(const char *msgName)
{
  MSG_PTR msg;
  
  if (msgName == NULL) {
    X_IPC_MOD_WARNING("ERROR: message name is NULL in call to x_ipcAllocateReply.");
    return NULL;
  } 
  
  msg = x_ipc_msgFind(msgName);
  if (msg == NULL) return NULL;
  
  if ((msg!= NULL) && (msg->msgData->resFormat))
    return (void *)
      x_ipcMalloc((unsigned)x_ipc_dataStructureSize(msg->msgData->resFormat));
  
  return NULL;
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipcFreeReply(msgName, replyData)
 *
 * DESCRIPTION: 
 * Using the replyForm format of the message, free the all of the allocated
 * subelements of the data structure, but do not free the data structure
 * itself.
 *
 * INPUTS:
 * char *msgName; 
 * void *replyData;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void x_ipcFreeReply(const char *msgName, void *replyData)
{
  MSG_PTR msg;
  
  if (msgName == NULL) {
    X_IPC_MOD_ERROR("ERROR: message name is NULL in call to x_ipcFreeReply.");
  } else if (replyData) {
    
    msg = x_ipc_msgFind(msgName);
    if (msg == NULL) return;
    
    if (msg->msgData->resFormat) {
      
      (void)x_ipc_freeDataElements(msg->msgData->resFormat, (char *)replyData, 0,
			     (FORMAT_PTR)NULL);
    }
  }
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipc_classDataFree(class, classData)
 *
 * DESCRIPTION: 
 * Finds the class format associated with the class, and frees the data.
 *
 * INPUTS:
 * X_IPC_MSG_CLASS_TYPE msg_class;
 * char *classData;
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void x_ipc_classDataFree(X_IPC_MSG_CLASS_TYPE msg_class, void *classData)
{
  CLASS_FORM_PTR classForm;
  
  if (classData) {
    LOCK_M_MUTEX;
    classForm = GET_CLASS_FORMAT(&msg_class);
    UNLOCK_M_MUTEX;
    if (!classForm) {
      X_IPC_MOD_ERROR("ERROR: Missing class format in freeClassData.");
    } else {
      x_ipc_freeDataStructure(classForm->format, classData);
    }
  }
}


/*****************************************************************************
 *
 * FUNCTION: void x_ipc_classEntryFree(name, format)
 *
 * DESCRIPTION: 
 * Cleans up the hash table entry for a class.
 *
 * INPUTS:
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void x_ipc_classEntryFree(char *name, CLASS_FORM_PTR classFormat)
{
#ifdef UNUSED_PRAGMA
#pragma unused(name)
#endif
  if (classFormat) {
    x_ipc_freeFormatter(&(classFormat->format));
    x_ipcFree((char *)classFormat);
  }
}


/*****************************************************************************
 *
 * FUNCTION: void cacheFormatterAttributes(FORMAT_PTR format)
 *
 * DESCRIPTION: 
 * Cache certain values that are asked for repeatedly
 *
 * INPUTS:
 *
 * OUTPUTS: none
 *
 *****************************************************************************/

void cacheFormatterAttributes(FORMAT_PTR format)
{
  format->fixedSize = x_ipc_sameFixedSizeDataBuffer(format);
  format->structSize = x_ipc_dataStructureSize(format);
  /* If the format is flat (contains no pointers), then the 
     buffer size is constant, and can be predetermined */
  if (!formatContainsPointers(format))
    format->flatBufferSize = x_ipc_bufferSize(format, NULL);
}

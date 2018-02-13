/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: formatters
 *
 * FILE: formatters.h
 *
 * ABSTRACT:
 *
 * Data Format Routines. Include File.
 * 
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: formatters.h,v $
 * Revision 2.12  2009/09/04 19:12:57  reids
 * Port for ARM
 *
 * Revision 2.11  2009/05/04 19:02:54  reids
 * Fixed bug in dealing with longs and doubles for 64 bit machines
 *
 * Revision 2.10  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.9  2009/01/12 15:10:38  reids
 * Compile under x86_64
 *
 * Revision 2.8  2009/01/09 21:47:19  reids
 * Potential bug squashed
 *
 * Revision 2.7  2005/12/30 17:01:44  reids
 * Support for Mac OSX
 *
 * Revision 2.6  2002/01/03 20:52:11  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/02/09 16:24:20  reids
 * Added IPC_getConnections to return list of ports that IPC is listening to.
 * Added IPC_freeDataElements to free the substructure (pointers) of a struct.
 *
 * Revision 2.4  2001/01/05 22:43:57  reids
 * Minor changes to enable compiling under Visual C++, Cygwin, & Solaris.
 *
 * Revision 2.3  2000/12/11 16:12:18  reids
 * Extended for compilation under CYGWIN and RedHat 6.
 *
 * Revision 2.2  2000/07/03 17:03:23  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.4.2.7  1997/03/07 17:49:37  reids
 * Added support for OS2, needed by JSC team (thanks to Bob Goode).
 * Also fixed bug when passing between machines of different endianness.
 *
 * Revision 1.4.2.6  1997/01/27 20:40:01  reids
 * Implement a function to check whether two formats are the same.
 *
 * Revision 1.4.2.5  1997/01/27 20:09:25  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.4.2.3  1997/01/11 01:20:56  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.4.2.2.6.1  1996/12/27 19:25:59  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.4.2.2  1996/10/18 18:02:45  reids
 * Added tags to endifs.
 *
 * Revision 1.4.2.1  1996/10/16 15:17:43  reids
 * Cache format attribute values (much more efficient).
 * Transfer structure in one go if it does not contain pointers or padding.
 * Add short, byte and ubyte formats for Lisp.
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.4  1996/08/07 20:42:35  eldred
 * Merged S3 back to main
 *
 * Revision 1.2.4.1  1996/07/31 22:28:06  eldred
 * Restored makefile from tagged S3 directory
 *
 * Revision 1.2  1996/06/07 19:36:27  vicki
 * *** empty log message ***
 *
 * Revision 1.1  1996/05/09 01:01:24  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/03/19 03:38:42  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:42  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:25  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.34  1996/07/19  18:14:08  reids
 * Record broadcast messages if handler is registered before message.
 * Transfer any pending messages to the new resource under "addHndToResource"
 * Fixed x_ipcDelayCommand (wrong time units).
 * Fixed logging of refid's (have to distinguish whether they are part of
 *   a status, message, or "always" log).
 * Sanity check for encoding/decoding messages.
 *
 * Revision 1.33  1996/06/25  20:50:37  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.32  1996/03/15  21:18:06  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.31  1996/02/06  19:04:41  rich
 * Changes for VXWORKS pipes.  Note: the read and write sockets descriptors
 * can be different.
 *
 * Revision 1.30  1996/02/01  04:03:37  rich
 * Generalized updateVersion and added recursion.
 *
 * Revision 1.29  1996/01/30  15:04:10  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.28  1996/01/27  21:53:29  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.27  1995/08/04  16:41:05  rich
 * Fixed problem with in place translation of doubles from Big to little endian.
 *
 * Revision 1.26  1995/07/25  20:08:58  rich
 * Changes for NeXT machine (3.0).
 *
 * Revision 1.25  1995/07/12  04:54:52  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.24  1995/07/10  16:17:23  rich
 * Interm save.
 *
 * Revision 1.23  1995/07/06  21:16:19  rich
 * Solaris and Linux changes.
 *
 * Revision 1.22  1995/05/31  19:35:34  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.21  1995/04/19  14:28:13  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.20  1995/04/05  19:10:46  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.19  1995/04/04  19:42:11  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.18  1995/03/30  15:42:59  rich
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
 * Revision 1.17  1995/03/18  15:11:02  rich
 * Fixed updateVersion script so it can be run from any directory.
 *
 * Revision 1.16  1995/03/16  18:05:26  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.15  1995/01/30  16:17:58  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.14  1995/01/25  00:01:15  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.13  1995/01/18  22:40:31  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.12  1994/05/31  03:23:57  rich
 * Removed CFLAGS_sun4.
 * Removed cfree and bzero from mem routines.
 * Set zero wait on both sides of the pipe.  Can connect to host using inet
 * number.
 *
 * Revision 1.11  1994/05/17  23:15:51  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.10  1994/05/05  00:46:10  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for 
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.9  1994/04/28  16:16:03  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.8  1994/04/16  19:42:12  rich
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
 * Revision 1.7  1994/01/31  18:27:50  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of 
 * the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.6  1993/12/01  18:03:28  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.5  1993/11/21  20:17:52  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/27  08:38:35  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.3  1993/08/27  07:14:52  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:33  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:43  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:29:54  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:23:48  fedor
 * Added Logging.
 *
 * $Revision: 2.12 $
 * $Date: 2009/09/04 19:12:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCformatters
#define INCformatters

typedef enum {
  Encode=0, Decode=1, ELength=2,
  ALength=3, RLength=4, SimpleType=5, DPrint=6, DFree=7,
  MAX_TRANS_OP_TYPE
#ifdef FORCE_32BIT_ENUM
    , dummyTransOp = 0x7FFFFFFF
#endif
} TRANS_OP_TYPE;

#ifndef __TURBOC__
typedef enum {
  PrimitiveFMT=0, 
  LengthFMT=1, 
  StructFMT=2,
  PointerFMT=3,
  FixedArrayFMT=4,
  VarArrayFMT=5,
  BadFormatFMT=6,
  NamedFMT=7,
  EnumFMT=8
#ifdef FORCE_32BIT_ENUM
    , dummyFormatClass = 0x7FFFFFFF
#endif
} FORMAT_CLASS_TYPE;
#else
typedef int32 FORMAT_CLASS_TYPE;
#define PrimitiveFMT  0
#define LengthFMT     1
#define StructFMT     2
#define PointerFMT    3
#define FixedArrayFMT 4
#define VarArrayFMT   5
#define BadFormatFMT  6
#define NamedFMT      7
#define EnumFMT       8
#endif /* __TURBOC__ */

#ifndef __TURBOC__
typedef enum {
  INT_FMT =     1,
  BOOLEAN_FMT = 2,
  FLOAT_FMT =   3,
  DOUBLE_FMT =  4,
  BYTE_FMT =    5,
  TWOBYTE_FMT = 6,
  STR_FMT =     7,
  FORMAT_FMT =  8,
  UBYTE_FMT =   9,
  CMAT_FMT =    10,
  SMAT_FMT =    11,
  IMAT_FMT =    12,
  LMAT_FMT =    13,
  FMAT_FMT =    14,
  DMAT_FMT =    15,
  CHAR_FMT =    16,
  SHORT_FMT =   17,
  LONG_FMT =    18,
  UCMAT_FMT =   19,
  X_IPC_REF_PTR_FMT = 20,
  
  SIUCMAT_FMT = 21,
  SICMAT_FMT =  22,
  SISMAT_FMT =  23,
  SIIMAT_FMT =  24,
  SILMAT_FMT =  25,
  SIFMAT_FMT =  26,
  SIDMAT_FMT =  27,

  USHORT_FMT =  28,
  UINT_FMT =    29,
  ULONG_FMT =   30,

  MAXFORMATTERS = 31
#ifdef FORCE_32BIT_ENUM
    , dummyPrimFormat = 0x7FFFFFFF
#endif
} PRIM_FORMAT_TYPE;
#else

#define PRIM_FORMAT_TYPE int32

#define INT_FMT     1
#define BOOLEAN_FMT 2
#define FLOAT_FMT   3
#define DOUBLE_FMT  4
#define BYTE_FMT    5
#define TWOBYTE_FMT 6
#define STR_FMT     7
#define FORMAT_FMT  8
#define UBYTE_FMT   9
#define CMAT_FMT    10
#define SMAT_FMT    11
#define IMAT_FMT    12
#define LMAT_FMT    13
#define FMAT_FMT    14
#define DMAT_FMT    15
#define CHAR_FMT    16
#define SHORT_FMT   17
#define LONG_FMT    18
#define UCMAT_FMT   19
#define X_IPC_REF_PTR_FMT 20

#define SIUCMAT_FMT 21
#define SICMAT_FMT  22
#define SISMAT_FMT  23
#define SIIMAT_FMT  24
#define SILMAT_FMT  25
#define SIFMAT_FMT  26
#define SIDMAT_FMT  27

#define USHORT_FMT  28
#define UINT_FMT    29
#define ULONG_FMT   30

#define MAXFORMATTERS 31

#endif /* __TURBOC__ */

#define INT_FMT_NAME     "int"
#define BOOLEAN_FMT_NAME "boolean"
#define FLOAT_FMT_NAME   "float"
#define DOUBLE_FMT_NAME  "double"
#define BYTE_FMT_NAME    "byte"
#define TWOBYTE_FMT_NAME "twobyte"
#define STR_FMT_NAME     "string"
#define FORMAT_FMT_NAME  "format"
#define UBYTE_FMT_NAME   "ubyte"
#define CMAT_FMT_NAME    "cmat"
#define SMAT_FMT_NAME    "smat"
#define IMAT_FMT_NAME    "imat"
#define LMAT_FMT_NAME    "lmat"
#define FMAT_FMT_NAME    "fmat"
#define DMAT_FMT_NAME    "dmat"
#define CHAR_FMT_NAME    "char"
#define SHORT_FMT_NAME   "short"
#define LONG_FMT_NAME    "long"
#define UCMAT_FMT_NAME   "ucmat"
#define X_IPC_REF_PTR_FMT_NAME  "X_IPC_REF_PTR"

#define SIUCMAT_FMT_NAME "siucmat"
#define SICMAT_FMT_NAME  "sicmat"
#define SISMAT_FMT_NAME  "sismat"
#define SIIMAT_FMT_NAME  "siimat"
#define SILMAT_FMT_NAME  "silmat"
#define SIFMAT_FMT_NAME  "sifmat"
#define SIDMAT_FMT_NAME  "sidmat"

#define USHORT_FMT_NAME  "ushort"
#define UINT_FMT_NAME    "uint"
#define ULONG_FMT_NAME   "ulong"


#define BAD_FORMAT ((CONST_FORMAT_PTR)-1)

typedef union {
  int32 i;
  struct _FORMAT_TYPE *f;
} FORMAT_ARRAY_TYPE, *FORMAT_ARRAY_PTR;

typedef union { 
  int32 i;
  const struct _FORMAT_TYPE *f;
} CONST_FORMAT_ARRAY_TYPE, *CONST_FORMAT_ARRAY_PTR;

typedef union {
  int32                i;
  PRIM_FORMAT_TYPE     p;
  struct _FORMAT_TYPE *f;
  FORMAT_ARRAY_PTR     a;
  char                *name;
} FMT_ELEMENT_TYPE;

#define NOT_CACHED     (-1)

typedef struct _FORMAT_TYPE {
  FORMAT_CLASS_TYPE type;
  FMT_ELEMENT_TYPE formatter;
  int32 structSize;
  int32 flatBufferSize;
  BOOLEAN fixedSize;
} FORMAT_TYPE, *FORMAT_PTR;

typedef const FORMAT_TYPE *CONST_FORMAT_PTR;

typedef struct {
  char *definition;
  FORMAT_PTR format;
  int32 parsed;
} NAMED_FORMAT_TYPE, *NAMED_FORMAT_PTR;

typedef struct {
  X_IPC_MSG_CLASS_TYPE className;
  CONST_FORMAT_PTR format;
} CLASS_FORM_TYPE, *CLASS_FORM_PTR;

typedef struct {
  int32 bstart;
  char *buffer;
} BUFFER_TYPE, *BUFFER_PTR;

typedef struct{
  int32 buffer, data;
} SIZES_TYPE;

/* Variables needed for printing data structures */
typedef struct {
  int32 lineNumGlobal;
  int32 cursorPosGlobal;
  int32 truncatedGlobal;
  int32 indentGlobal;
  /* main scratch buffer for printing */
  char  buffer[400];
} Print_Data_Type, *Print_Data_Ptr;

typedef char *GENERIC_DATA_PTR;

typedef const char *CONST_GENERIC_DATA_PTR;

/*
 * The format code depends on how data structures are layed out in memory.
 * Below are a set of constants that control how the formatting will be done.
 */

#define ALIGN_PACKED  0x00
#define ALIGN_WORD    0x01
#define ALIGN_LONGEST 0x02
#define ALIGN_INT     0x04
#define ALIGN_MAC_PPC 0x10
#define ALIGN_ARM     0x20

#ifndef __TURBOC__
typedef enum 
{
  /* 
   * Align things on word boundaries unless next item uses an odd number 
   * of bytes.
   */
  PACKED_ALIGNMENT = ALIGN_PACKED,
  
  /* 
   * Align things on word boundaries unless next item uses an odd number 
   * of bytes.
   */
  WORD_ALIGNMENT = ALIGN_WORD,
  
  /* 
   * Align things on the boundary of the longest type in the struct.
   */
  LONGEST_ALIGNMENT = ALIGN_LONGEST,
  
  /* 
   * Align things on the boundary of the longest type in the struct, up to the
   * size of an int.
   */
  INT_ALIGNMENT = ALIGN_INT,

  /* 
   * Similar to ALIGN_INT (above) except: If first element is a "double", pad
   * the structure to 8 byte boundary.  Used by CodeWarrior, at least, for the
   * Macintosh PPC.
   */
  MAC_PPC_ALIGNMENT = ALIGN_MAC_PPC,

  /* 
   * Similar to ALIGN_LONGEST (above) except structures must be even 
   * word lengths (multiple of 4) long.  Used for ARM compiler.
   */
  ARM_ALIGNMENT = ALIGN_ARM

#ifdef FORCE_32BIT_ENUM
    , dummyAlignment = 0x7FFFFFFF
#endif
} ALIGNMENT_TYPE;
#else
#define ALIGNMENT_TYPE int32
#define PACKED_ALIGNMENT ALIGN_PACKED
#define WORD_ALIGNMENT ALIGN_WORD
#define LONGEST_ALIGNMENT ALIGN_LONGEST
#define INT_ALIGNMENT ALIGN_INT
#define MAC_PPC_ALIGNMENT ALIGN_MAC_PPC
#define ARM_ALIGNMENT ALIGN_ARM
#endif /* __TURBOC__ */

#if defined(M68K) || defined(__m68k__) || defined(__mc68000__)
#define IPC_ALIGN ALIGN_WORD
#elif defined (i386) && !defined(__CYGWIN__)         /* Get this before PPC603 which comes from ? */
#define IPC_ALIGN ALIGN_INT
#elif defined(__x86_64)
#define IPC_ALIGN ALIGN_LONGEST
#elif defined (i386) && defined(__CYGWIN__)
#define IPC_ALIGN ALIGN_LONGEST
#elif defined(ppc) || defined(PPC603)
#define IPC_ALIGN ALIGN_LONGEST
#elif defined(THINK_C) || defined(sun3) || defined(SUN3)
#define IPC_ALIGN ALIGN_WORD
#elif defined(macintosh) && defined(__POWERPC__)
#define IPC_ALIGN ALIGN_MAC_PPC
#elif defined(__APPLE__) && defined(_ARCH_PPC)
#define IPC_ALIGN ALIGN_MAC_PPC
#elif defined(sun4) || defined(SUN4) || defined(sparc) || defined(__sparc)
#define IPC_ALIGN ALIGN_LONGEST
#elif defined(pmax) || defined(alpha) || defined(__alpha) || defined(__alpha__)
#define IPC_ALIGN ALIGN_LONGEST
#elif _MSC_VER == 1200  /* JSM Microsoft VC++ Version 6.0 aligns on 8-byte boundaries */
#define IPC_ALIGN ALIGN_LONGEST
#elif defined(i386) || defined(__TURBOC__) || defined(OS2) || defined(_WIN95_MSC_) || defined(WIN32)
#define IPC_ALIGN ALIGN_INT
#elif defined(__sgi) || defined(MIPSEB)
#define IPC_ALIGN ALIGN_LONGEST
/* Note, the next line is only valid for gcc, but will only be evaluated 
 * if the machine type is unknown.
 */
#elif #machine (sparc)
#define IPC_ALIGN ALIGN_LONGEST
#elif #machine (arm)
#define IPC_ALIGN ALIGN_ARM
#else 
#undef IPC_ALIGN
#endif

#ifdef BOB_DOESNT_KNOW_WHAT_HES_DOING
#ifdef AERCAMII					/* Nasa-JSC Aercam II project, needs all alignments the same  */
#undef IPC_ALIGN					/* Across Sparc, Pentiums, Mc680x0, hardware and */
#define IPC_ALIGN ALIGN_INT			/* OS/2, Win95, VxWorks, SunOS, Solaris operating systems */
/*#define IPC_ALIGN ALIGN_WORD */			/* OS/2, Win95, VxWorks, SunOS, Solaris operating systems */
#endif
#endif



#define NETWORK_BYTE_ORDER BIG_ENDIAN
/* Use this for testing.*/
/*#define NETWORK_BYTE_ORDER LITTLE_ENDIAN*/
#define NETWORK_ALIGNMENT ALIGN_WORD

/* Define constants for communicating endian.*/
#define DATA_ENDIAN_MASK  0x0F000000
#define CLASS_ENDIAN_MASK 0xF0000000

typedef enum {
  BIG_ENDIAN_MASK    = 0x1,
  LITTLE_ENDIAN_MASK = 0x2,
  PDP_ENDIAN_MASK    = 0x3
#ifdef FORCE_32BIT_ENUM
    , dummyEndian = 0x7FFFFFFF
#endif
} ENDIAN_TYPE;

/* Define mask for communicating alignment.*/
#define ALIGNMENT_MASK  0x00FF0000

#define GET_DATA_ENDIAN(msgFormat) \
((((msgFormat) & ((int32)DATA_ENDIAN_MASK )) == ((int32)LITTLE_ENDIAN_MASK << 24)) ?\
 LITTLE_ENDIAN :\
 ((((msgFormat) & ((int32)DATA_ENDIAN_MASK )) == ((int32)BIG_ENDIAN_MASK << 24)) ?\
  BIG_ENDIAN :\
  ((((msgFormat) & ((int32)DATA_ENDIAN_MASK )) == ((int32)PDP_ENDIAN_MASK << 24)) ?\
   PDP_ENDIAN :\
   0)))

#define SET_DATA_ENDIAN(msgFormat,byteOrder) \
((byteOrder == LITTLE_ENDIAN) ? ((msgFormat) | ((int32)LITTLE_ENDIAN_MASK << 24)):\
 ((byteOrder == BIG_ENDIAN) ? ((msgFormat) | ((int32)BIG_ENDIAN_MASK << 24)):\
  ((byteOrder == PDP_ENDIAN) ? ((msgFormat) | ((int32)PDP_ENDIAN_MASK << 24)):\
   0)))

#define GET_CLASS_ENDIAN(msgFormat) \
((((msgFormat) & ((int32)CLASS_ENDIAN_MASK)) == ((int32)LITTLE_ENDIAN_MASK << 28)) ?\
 LITTLE_ENDIAN :\
 ((((msgFormat) & ((int32)CLASS_ENDIAN_MASK)) == ((int32)BIG_ENDIAN_MASK << 28)) ?\
  BIG_ENDIAN :\
  ((((msgFormat) & ((int32)CLASS_ENDIAN_MASK)) == ((int32)PDP_ENDIAN_MASK << 28)) ?\
   PDP_ENDIAN :\
   0)))

#define SET_CLASS_ENDIAN(msgFormat,byteOrder) \
((byteOrder == LITTLE_ENDIAN) ? ((msgFormat) | ((int32)LITTLE_ENDIAN_MASK << 28)):\
 ((byteOrder == BIG_ENDIAN) ? ((msgFormat) | ((int32)BIG_ENDIAN_MASK << 28)):\
  ((byteOrder == PDP_ENDIAN) ? ((msgFormat) | ((int32)PDP_ENDIAN_MASK << 28)):\
   0)))

#define GET_ALIGNMENT(msgFormat) \
(((msgFormat) & ALIGNMENT_MASK) >>16)

#define SET_ALIGNMENT(msgFormat) \
((msgFormat & ~ALIGNMENT_MASK) | ((int32)ALIGN_PACKED << 16))

#define GET_CLASSID(msgFormat) ((msgFormat) & 0x0000FFFF)

#define REF(type, datastruct, dstart) *(type *)(datastruct+dstart)

#define TO_BUFFER_AND_ADVANCE(data, buffer, bstart, length) \
{BCOPY((char *)data, (buffer)+bstart, length); bstart += length;}

#define FROM_BUFFER_AND_ADVANCE(data, buffer, bstart, length) \
{BCOPY((buffer)+bstart, (char *)data, length); bstart += length;}

#define NEW_FORMATTER() newFormatter()

#define NEW_FORMAT_ARRAY(size) \
(FORMAT_ARRAY_PTR)x_ipcMalloc((unsigned)(size) * sizeof(FORMAT_ARRAY_TYPE))

/* If there is no formatter, it means that the ptr is recursive (self-ptr) */
#define CHOOSE_PTR_FORMAT(format, parentFormat) \
(((format)->formatter.f) ? ((format)->formatter.f) : \
 (FORMAT_PTR)parentFormat)

#define ODDPTR(x) ((x) & 1)

#define WORD_SIZE 4
#define WORD_SIZEP(x) (((int32)(x) & (WORD_SIZE-1)) == 0)


#define shortToBytes(s,shortBytes) \
{\
   register const unsigned char *src= ((unsigned char *) &(s));\
   register unsigned char *dest= (unsigned char *)(shortBytes);\
   *dest++ = *src++;\
   *dest = *src;\
}

#define intToBytes(i,intBytes) \
{\
   register const unsigned char *src= ((unsigned char *) &(i));\
   register unsigned char *dest= (unsigned char *)(intBytes);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define longToBytes(l,longBytes) \
{\
   register const unsigned char *src= ((unsigned char *) &(l));\
   register unsigned char *dest= (unsigned char *)(longBytes);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define floatToBytes(f,floatBytes) \
{\
   register const unsigned char *src= ((unsigned char *) &(f));\
   register unsigned char *dest= (unsigned char *)(floatBytes);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define doubleToBytes(d,doubleBytes) \
{\
   register const unsigned char *src= ((unsigned char *) &(d));\
   register unsigned char *dest= (unsigned char *)(doubleBytes);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define bytesToShort(shortBytes, sPtr) \
{\
   register const unsigned char *src= ((unsigned char *) (shortBytes));\
   register unsigned char *dest= (unsigned char *)(sPtr);\
   *dest++ = *src++;\
   *dest = *src;\
}

#define bytesToInt(intBytes, iPtr) \
{\
   register const unsigned char *src= ((unsigned char *) (intBytes));\
   register unsigned char *dest= (unsigned char *)(iPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define bytesToLong(longBytes, lPtr) \
{\
   register const unsigned char *src= ((unsigned char *) (longBytes));\
   register unsigned char *dest= (unsigned char *)(lPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define bytesToFloat(floatBytes, fPtr) \
{\
   register const unsigned char *src= ((unsigned char *) (floatBytes));\
   register unsigned char *dest= (unsigned char *)(fPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#define bytesToDouble(doubleBytes,dPtr) \
{\
   register const unsigned char *src= ((unsigned char *) (doubleBytes));\
   register unsigned char *dest= (unsigned char *)(dPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
}

#if ((BYTE_ORDER == LITTLE_ENDIAN) || (BYTE_ORDER == BIG_ENDIAN))

/* The little endian is just reversed from the big endian, so we can
 * do the exchange when we do the copy and save calling hton*
 */

#define shortToRevBytes(s,shortBytes) \
{\
   register const unsigned char *src= ((unsigned char *) &s);\
   register unsigned char *dest= (unsigned char *)(shortBytes);\
   register unsigned char s0;\
   s0 = *src++;\
   *dest++ = *src;\
   *dest = s0;\
}

#define intToRevBytes(i,intBytes) \
{\
   register unsigned char *src= ((unsigned char *) &(i));\
   register unsigned char *dest = (intBytes);\
   register unsigned char s0,s1;\
   s0 = src[0];\
   s1 = src[1];\
   dest[0] = src[3];\
   dest[1] = src[2];\
   dest[2] = s1;\
   dest[3] = s0;\
}

#define longToRevBytes(l,longBytes) intToRevBytes(l,longBytes)

#define floatToRevBytes(f,floatBytes) intToRevBytes(f,floatBytes)

#define doubleToRevBytes(d,doubleBytes) \
{\
   register int32 *Integer=((int32 *)&(d));\
   intToRevBytes(*Integer,((doubleBytes)+sizeof(int32)));\
   Integer++;\
   intToRevBytes(*Integer,((doubleBytes)));\
}


#define revBytesToShort(shortBytes, sPtr) \
{\
   register const unsigned char *src= (unsigned char *) (shortBytes);\
   register unsigned char *dest= (unsigned char *)(sPtr);\
   register unsigned char s0;\
   s0 = src[0];\
   dest[0] = src[1];\
   dest[1] = s0;\
}

#define revBytesToInt(intBytes, iPtr) \
{\
   register unsigned char *src= (unsigned char *)(intBytes);\
   register unsigned char *dest= ((unsigned char *) (iPtr));\
   register unsigned char s0,s1;\
   s0 = src[0];\
   s1 = src[1];\
   dest[0] = src[3];\
   dest[1] = src[2];\
   dest[2] = s1;\
   dest[3] = s0;\
}

#define revBytesToLong(longBytes, lPtr) revBytesToInt(longBytes,(int *)(lPtr)) 

#define revBytesToFloat(floatBytes, fPtr) \
revBytesToInt((floatBytes), (int32 *)(fPtr)) 
     
#define revBytesToDouble(doubleBytes,dPtr) \
{\
   register unsigned char *src= (unsigned char *)(doubleBytes);\
   register unsigned char *dest= ((unsigned char *) (dPtr));\
   register unsigned char s0,s1,s2,s3;\
   s0 = src[0];\
   s1 = src[1];\
   s2 = src[2];\
   s3 = src[3];\
   dest[0] = src[7];\
   dest[1] = src[6];\
   dest[2] = src[5];\
   dest[3] = src[4];\
   dest[4] = s3;\
   dest[5] = s2;\
   dest[6] = s1;\
   dest[7] = s0;\
}

#else /* not little endian or big endian, must be pdp endian.*/

#define shortToRevBytes(s,shortBytes) \
{\
   register const unsigned char *src= (unsigned char *) &(s);\
   register unsigned char *dest= (shortBytes);\
   register int16 temp = (s);\
   s = htons(s);\
   *dest++ = *src++;\
   *dest = *src; \
   s = temp;\
}

#define intToRevBytes(i,intBytes) \
{\
   register const unsigned char *src= (unsigned char *) &(i);\
   register unsigned char *dest = (intBytes);\
   register int32 temp = (i);\
   i = htonl(i);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src; \
   i = temp;\
}

/*#if (sizeof(long) == 4)*/
#if TRUE

#define longToRevBytes(l,longBytes) \
{\
   register const unsigned char *src= (unsigned char *) &l;\
   register unsigned char *dest= (longBytes);\
   register long temp = (l);\
   l = htonl(l);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src; \
   l = temp;\
}

/*#elif (sizeof(long) == 8)*/
#else

#define longToRevBytes(l,longBytes) \
{\
   int32 *Integer= ((int32 *)&l);\
   register const unsigned char *src= (unsigned char *) Integer;\
   register unsigned char *dest= (longBytes);\
   register long temp = l;\
   *Integer = htonl(*Integer);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
   l = temp;\
}

#endif /* TRUE */

#define floatToRevBytes(f,floatBytes) \
{\
   int32 *Integer= ((int32 *)&f);\
   register const unsigned char *src= (unsigned char *) Integer;\
   register unsigned char *dest= (floatBytes);\
   register float temp = (f);\
   *Integer= htonl(*Integer);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src; \
   f = temp;\
}

#define doubleToRevBytes(d,doubleBytes) \
{\
   int32 *Integer=((int32 *)&d);\
   register const unsigned char *src= (unsigned char *) Integer;\
   register unsigned char *dest = (doubleBytes);\
   register double temp = (d);\
   *Integer = htonl(*Integer);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src;\
   *Integer = htonl(*Integer);\
   src = (const unsigned char *) Integer;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src; \
   d = temp;\
}


#define revBytesToShort(shortBytes, sPtr) \
{\
   register const unsigned char *src= (shortBytes);\
   register unsigned char *dest= (unsigned char *) sPtr;\
   *dest++ = *src++;\
   *dest = *src;\
   *sPtr = htons(*sPtr); }

#define revBytesToInt(intBytes, iPtr) \
{\
   register const unsigned char *src= (intBytes);\
   register unsigned char *dest= (unsigned char *) (iPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
   *iPtr = htonl(*iPtr);}

/*#if (sizeof(long) == 4)*/
#if TRUE

#define revBytesToLong(longBytes, lPtr) \
{\
   register const unsigned char *src= (longBytes);\
   register unsigned char *dest= (unsigned char *) (lPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
   *lPtr = htonl(*lPtr); }

/*#elif (sizeof(long) == 8)*/
#else

#define revBytesToLong(longBytes, lPtr) \
{\
   register const unsigned char *src= (longBytes);\
   register unsigned char *dest= (unsigned char *) (lPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
   *lPtr = htonl(*lPtr); }
#endif /* TRUE */

#define revBytesToFloat(floatBytes, fPtr) \
{\
   register const unsigned char *src= (floatBytes);\
   register unsigned char *dest= (unsigned char *) (fPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
   *((int32 *)fPtr) = htonl(*((int32 *)fPtr));\
}

#define revBytesToDouble(doubleBytes,dPtr) \
{\
   register const unsigned char *src= (doubleBytes);\
   register unsigned char *dest= (unsigned char *) (dPtr);\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest++ = *src++;\
   *dest = *src;\
   *((int32 *)dPtr) = htonl(*((int32 *)dPtr));\
   *(((int32 *)dPtr)+1) = htonl(*(((int32 *)dPtr)+1));\
}

#endif /* LITTLE_ENDIAN || BIG_ENDIAN */

/* Define the encoding/decoding functions. */

/* Characters never need to be reordered. */
#define charToNetBytes(c,charBytes) {*((unsigned char *)(charBytes)) = (unsigned char)(c);}

#define netBytesToChar(charBytes, cPtr) \
{*((unsigned char *)(cPtr)) = *((unsigned char *)(charBytes));}

#define shortToNetBytes(s,shortBytes) shortToBytes(s,shortBytes) 

#define netBytesToShort(shortBytes, sPtr) \
{if (byteOrder == BYTE_ORDER) \
   {bytesToShort(shortBytes,sPtr)}\
else \
   {revBytesToShort(shortBytes,sPtr)}\
}

#define intToNetBytes(i,intBytes) intToBytes(i,intBytes) 

#define netBytesToInt(intBytes, iPtr) \
{if (byteOrder == BYTE_ORDER) \
   {bytesToInt(intBytes,iPtr)}\
else \
   {revBytesToInt(intBytes,iPtr)}\
}

#define longToNetBytes(l,longBytes) longToBytes(l,longBytes) 

#define netBytesToLong(longBytes, lPtr) \
{if (byteOrder == BYTE_ORDER) \
   {bytesToLong(longBytes,lPtr)}\
else \
   {revBytesToLong(longBytes,lPtr)}\
}

#define floatToNetBytes(f,floatBytes) floatToBytes(f,floatBytes) 

#define netBytesToFloat(floatBytes, fPtr) \
{if (byteOrder == BYTE_ORDER) \
   {bytesToFloat(floatBytes,fPtr)}\
else \
   {revBytesToFloat(floatBytes,fPtr)}\
}

#define doubleToNetBytes(d,doubleBytes) doubleToBytes(d,doubleBytes) 

#define netBytesToDouble(doubleBytes, dPtr) \
{if (byteOrder == BYTE_ORDER) \
   {bytesToDouble(doubleBytes,dPtr)}\
else \
   {revBytesToDouble(doubleBytes,dPtr)}\
}

#if (BYTE_ORDER == NETWORK_BYTE_ORDER)
/* No ops */
#define INT_TO_NET_INT(i)
#define NET_INT_TO_INT(i)
#elif ((BYTE_ORDER == LITTLE_ENDIAN) || (BYTE_ORDER == BIG_ENDIAN))
/* Eventually, this should be replaced.*/
#define INT_TO_NET_INT(i) { i = htonl(i);}
#define NET_INT_TO_INT(i) { i = ntohl(i);}
#else
#define INT_TO_NET_INT(i) { i = htonl(i);}
#define NET_INT_TO_INT(i) { i = ntohl(i);}
#endif

/****************************************************************
 *                Stuff Needed For "Enum"
 ****************************************************************/

#define MAX_BYTE  0xFF
#define MAX_SHORT 0xFFFF
#define MAX_INT   0xFFFFFFFF

/* Dummy typedef's to determine how the compiler treats enum's */
typedef enum { ByteEnum0=0, ByteEnumLast=MAX_BYTE} ByteEnum;
typedef enum { ShortEnum0=0, ShortEnumLast=MAX_SHORT} ShortEnum;
typedef enum { IntEnum0=0, IntEnumLast=MAX_INT} IntEnum;

/* Size of different types of enum's */
#define BYTE_ENUM_SIZE  sizeof(ByteEnum)
#define SHORT_ENUM_SIZE sizeof(ShortEnum)
#define INT_ENUM_SIZE   sizeof(IntEnum)

#define ENUM_MAX_VAL(format) ((format)->formatter.a[1].i)

extern int32 x_ipc_enumToInt (CONST_FORMAT_PTR format,
			CONST_GENERIC_DATA_PTR dataStruct, int32 *DStart);

typedef int32 (* TRANSLATE_FN_ENCODE)(CONST_GENERIC_DATA_PTR, int32,
				    char *, int32);
typedef int32 (* TRANSLATE_FN_DECODE)(GENERIC_DATA_PTR, int32, char *, int32,
				    int32, ALIGNMENT_TYPE);
typedef int32 (* TRANSLATE_FN_ELENGTH)(CONST_GENERIC_DATA_PTR, int32);
typedef int32 (* TRANSLATE_FN_ALENGTH)(void);
typedef int32 (* TRANSLATE_FN_RLENGTH)(void);
typedef int32 (* TRANSLATE_FN_DPRINT)(CONST_GENERIC_DATA_PTR, int32, 
				      FILE *, Print_Data_Ptr, int32);
typedef int32 (* TRANSLATE_FN_DFREE)(GENERIC_DATA_PTR, int32);

typedef struct {
  TRANSLATE_FN_ENCODE Encode;
  TRANSLATE_FN_DECODE Decode;
  TRANSLATE_FN_ELENGTH ELength;
  TRANSLATE_FN_ALENGTH ALength; 
  TRANSLATE_FN_RLENGTH RLength;
  BOOLEAN SimpleType;
  TRANSLATE_FN_DPRINT DPrint;
  TRANSLATE_FN_DFREE DFree;
} TRANSLATE_TYPE, *TRANLATE_PTR;

FORMAT_PTR newFormatter(void);

void x_ipc_formatInitialize(void);
void x_ipc_addFormatToTable(const char *name, CONST_FORMAT_PTR format);
void x_ipc_addFormatStringToTable(char *name, char *formatString);
FORMAT_PTR x_ipc_createIntegerFormat(FORMAT_CLASS_TYPE type, int32 integer);

int32 x_ipc_alignField(CONST_FORMAT_PTR format, int32 currentField, 
		 int32 currentDataSize);

int32 x_ipc_dataStructureSize(CONST_FORMAT_PTR format);
int32 x_ipc_bufferSize(CONST_FORMAT_PTR Format, const void *DataStruct);
SIZES_TYPE x_ipc_bufferSize1(CONST_FORMAT_PTR format,
		       CONST_GENERIC_DATA_PTR dataStruct,
		       int32 dStart, 
		       CONST_FORMAT_PTR parentFormat);
void x_ipc_encodeData(CONST_FORMAT_PTR Format, const void *DataStruct,
		char *Buffer, int32 BStart, int32 x_ipc_bufferSize);
void *x_ipc_decodeData(CONST_FORMAT_PTR Format, char *Buffer, int32 BStart, 
		 char *DataStruct,
		 int32 byteOrder, ALIGNMENT_TYPE alignment, int32 x_ipc_bufferSize);
void x_ipc_freeDataStructure(CONST_FORMAT_PTR format, void *dataStruct);
int32 x_ipc_freeDataElements(CONST_FORMAT_PTR format,
			     GENERIC_DATA_PTR dataStruct,
			     int32 dStart, CONST_FORMAT_PTR parentFormat);
void x_ipc_classDataFree(X_IPC_MSG_CLASS_TYPE msg_class, void *classData);
BOOLEAN formatContainsPointers(CONST_FORMAT_PTR format);
BOOLEAN x_ipc_sameFixedSizeDataBuffer(CONST_FORMAT_PTR format);
BOOLEAN formatsEqual(CONST_FORMAT_PTR format1, CONST_FORMAT_PTR format2);
BOOLEAN canVectorizeData(CONST_FORMAT_PTR format);
struct iovec *x_ipc_createVectorization(CONST_FORMAT_PTR format, 
				  const char *dataStruct,
				  char *buffer,
				  int32 msgTotal);
struct iovec *x_ipc_copyVectorization(const struct iovec *vec, int32 space);

void x_ipc_formatFreeEntry(char *name, NAMED_FORMAT_PTR namedFormatter);
void x_ipc_classEntryFree(char *name, CLASS_FORM_PTR classFormat);
void cacheFormatterAttributes(FORMAT_PTR format);

#endif /* INCformatters */

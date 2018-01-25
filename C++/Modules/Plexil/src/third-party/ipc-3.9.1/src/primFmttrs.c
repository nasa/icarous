/*****************************************************************************
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture
 *
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: formatters
 *
 * FILE: primFmttrs.c
 *
 * ABSTRACT:
 *
 * Primitive Data Formatters
 *
 * Formatter functions take five arguments -- an "op," a pointer to the user's
 *"DataStruct" (the top-level structure being encoded or decoded), the "DStart"
 * (where in "DataStruct" to start encoding/decoding), a pointer to a "Buffer" 
 * (the data that will be sent/was received over the communications channel), 
 * and the "BStart" (where in "Buffer" to start encoding/decoding).
 *
 * The functions all return an integer, whose interpretation 
 * depends on the "op."
 *
 * The "op"s are:
 *
 * Encode:  Linearize the user's "DataStruct" (starting at "DStart") and place 
 *          the bytes in the "Buffer" (starting at "BStart").  
 *          Returns the number of bytes linearized.
 *
 * ELength: Returns the number of bytes that would be linearized by "Encode."
 *
 * Decode:  Using the "Buffer" (starting at "BStart"), fill in the user's
 *          "DataStruct" structure (starting at "DStart").  
 *          Encode and Decode are inverse functions.
 *          Returns the number of bytes used up from the "Buffer".
 *
 * ALength: Returns the Advance length, that is, the number of bytes taken 
 *          up by the top-level structure.  Equal to the "sizeof" the 
 *          data type, which is not necessarily equal to the ELength.  
 *          For example, STRING_FMT has an ALength
 *          of 4 (i.e., sizeof(char *)), but its ELength depend on the number
 *          of characters in the string.
 *
 * RLength: Restritive Length. Similar to ALength used by the sun4 for
 *          the mostRestiveElement call. Matrices most restrictive element 
 *          is actually 4 but ALength returned the sizeof the struct which
 *          is 24. This is mainly because matrix code uses special transfer
 *          functions.
 *
 * SimpleType: Returns TRUE (1) if the format is a fixed length type (i.e., the
 *          ELength always equals the ALength).
 *
 * DPrint:  Print out the data, using the "Print_" functions defined in file
 *          print_data.  For DPrint, the "buffer" argument is the stream to be
 *          printed on, and the "bstart" argument is the "keep_with_next"
 *          number of characters needed to complete a line 
 *          (see file "print_data").
 *
 * DFree:   Free the data structure if it was "malloc"ed while doing a
 *          "Decode" operation. 
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: primFmttrs.c,v $
 * Revision 2.6  2009/05/04 19:02:53  reids
 * Fixed bug in dealing with longs and doubles for 64 bit machines
 *
 * Revision 2.5  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2003/02/13 20:41:11  reids
 * Fixed compiler warnings.
 *
 * Revision 2.3  2002/01/03 20:52:15  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:27  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.6  1997/01/27 20:39:39  reids
 * For Lisp, force all format enum values to uppercase; Yields more efficient
 *   C <=> Lisp conversion of enumerated types.
 *
 * Revision 1.1.2.5  1997/01/27 20:09:49  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.3  1997/01/11 01:21:12  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.2.4.2  1996/12/27 19:26:05  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.1.2.2.4.1  1996/12/24 14:38:26  reids
 * Removed Lisp-specific code that was actually no longer being used.
 *
 * Revision 1.1.2.2  1996/12/18 15:13:02  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.1  1996/10/18 18:15:34  reids
 * Fixed freeing of formatters.
 *
 * Revision 1.1  1996/05/09 01:01:48  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/03/19 03:38:48  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:54  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:10  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.31  1996/06/25  20:51:07  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.30  1996/05/09  18:31:22  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.29  1996/03/19  02:29:25  reids
 * Plugged some more memory leaks; Added test code for enum formatters.
 * Added code to free formatter data structures.
 *
 * Revision 1.28  1996/03/15  21:18:17  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.27  1996/03/05  05:04:41  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.26  1996/02/12  17:42:13  rich
 * Handle direct connection disconnect/reconnect.
 *
 * Revision 1.25  1996/02/10  16:50:24  rich
 * Fixed header problems and a crash related to direct connections.
 *
 * Revision 1.24  1996/01/30  15:04:39  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.23  1996/01/27  21:53:51  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.22  1995/07/12  04:55:09  rich
 * Release of 8.0.
 * Fixed problems with sending between machines of different endien.
 *
 * Revision 1.21  1995/07/10  16:18:16  rich
 * Interm save.
 *
 * Revision 1.20  1995/06/14  03:21:48  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.19  1995/04/19  14:28:33  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.18  1995/04/05  19:10:56  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.17  1995/04/04  19:42:48  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.16  1995/03/30  15:43:47  rich
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
 * Revision 1.15  1995/03/16  18:05:38  rich
 * Merged in changes to the 7.9 branch.
 * Changed the VERSION_ to X_IPC_VERSION_
 *
 * Revision 1.14  1995/01/30  16:18:13  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.13  1995/01/25  00:01:29  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.12  1995/01/18  22:41:42  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.11  1994/10/25  17:10:28  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.10  1994/05/17  23:16:59  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.9  1994/04/28  16:16:43  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.8  1994/04/16  19:42:56  rich
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
 * Revision 1.7  1994/04/04  16:01:25  reids
 * Fixed the way data transfer occurred from/to big and little Endian machines
 *
 * Revision 1.6  1993/12/14  17:34:29  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.5  1993/11/21  20:18:53  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/30  23:14:03  fedor
 * Added SUN4 as well as sun4 compile flag.
 * Corrected Top level failure message name display with a define in dispatch.c
 *
 * Revision 1.2  1993/08/27  07:16:02  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.1.1.1  1993/05/20  05:45:29  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:31:31  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:57  fedor
 * Added Logging.
 *
 * $Revision: 2.6 $
 * $Date: 2009/05/04 19:02:53 $
 * $Author: reids $
 *
 *
 *    Feb-93 Reid Simmons at School of Computer Science, CMU
 * Added code for Mach/486 and PMAX versions (different byte orderings).
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 28-Feb-91 Christopher Fedor at School of Computer Science, CMU
 * Removed NULL matrix as a silly idea since a matrix is never really NULL.
 * A matrix with all bounds equal 0 is still a matrix of one element.
 * Made sure all printfs were follwed by a fflush(stdout) for lisp code.
 *
 * Added "sub-image" primitive formatters:
 * siucmat, sicmat, sismat, siimat, silmat, sifmat, sidmat.
 *
 * 27-Feb-91 Christopher Fedor at School of Computer Science, CMU
 * Added code to all matrix formatters so that a NULL matrix could be
 * sent on user defined error conditions. A NULL matrix is one with
 * all bounds equal 0 but mat storage equal NULL.
 *
 * 28-Oct-90 Christopher Fedor at School of Computer Science, CMU
 * Removed logging and print data routines to minimize module size.
 *
 * 19-Oct-90 Christopher Fedor at School of Computer Science, CMU
 * Added RLength to deal with sun4 transfer problem of "{int, dmat}".
 *
 * 15-Oct-90 Christopher Fedor at School of Computer Science, CMU
 * Defined byteFormat, charFormat, shortFormat, intFormat, longFormat,
 * floatFormat and doubleFormat for mapPrint to avoid calling
 * ParseFormatString in this file. 
 *
 * Added code to deal with NULL or empty strings in STR_Trans.
 * If a NULL or empty string is encoded it is decoded as a NULL.
 * "string" decodes to a pointer to NULL
 * {string} decodes as a struct whose element's value is a NULL pointer.
 *
 * 18-Jul-90 Reid Simmons at School of Computer Science, CMU
 * Added code for freeing data structures.
 *
 *  3-Apr-90 Christopher Fedor at School of Computer Science, CMU
 * Added expanded matrix support for perception.
 *
 *  9-Mar-89 Reid Simmons at School of Computer Science, CMU
 * Added SimpleType and DPrint ops
 *
 * 28-Feb-89 Christopher Fedor at School of Computer Science, CMU
 * Changed newfmat to newfmat2 to avoid GIL conflict
 *
 *  9-Feb-89 Reid Simmons at School of Computer Science, CMU
 * Split into 2 files -- "PrimFmttrs" and "Formatters"
 *
 *  6-Feb-89 Reid Simmons at School of Computer Science, CMU
 * Added structured formatters.
 *
 *  3-Feb-89 Reid Simmons at School of Computer Science, CMU
 * Standardized C and LISP interface to formatter functions.
 *
 *  2-Jan-89 Reid Simmons at School of Computer Science, CMU
 * Added INT and FLOAT formats for LISP interface.
 *
 *    Dec-88 Christopher Fedor at School of Computer Science, CMU
 * created.
 *
 *****************************************************************************/

#include "globalM.h"

#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif

/* 7-Nov-89: fedor: kept for REF macro - both should go away */
typedef char *StringType;


/******************************************************************************
 *
 * FUNCTION: void upcase (char *str)
 *
 * DESCRIPTION: Force the string to be uppercase
 *
 * INPUTS: char *str;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void upcase (char *str)
{
  char c;

  while (*str != '\0') {
    c = *str;
    if (isalpha((int)c) && islower((int)c)) *str = toupper((int)c);
    str++;
  }
}

/******************************************************************************
 *
 * FUNCTION: int32 x_ipcStrLen(s)
 *
 * DESCRIPTION: Returns strlen of string or 0 if string, s is NULL.
 *
 * INPUTS: const char *s;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

static INLINE int32 x_ipcStrLen(const char *s)
{
  return (s ? (int32)strlen(s) : 0);
}


/******************************************************************************
 *
 * FUNCTION: int32 STR_Trans(op, datastruct, dstart, buffer, bstart)
 *
 * DESCRIPTION:
 * "StringType" is a null terminated array of characters.
 * Decodes to:  length of string, Character-List
 *
 * INPUTS: 
 * TRANS_OP_TYPE op; 
 * GENERIC_DATA_PTR datastruct; 
 * int32 dstart; 
 * char *buffer; 
 * int32 bstart;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

int32 x_ipc_STR_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		     char *buffer, int32 bstart)
{ 
  char *pString;
  int32 current_byte, length;
  
  current_byte = bstart;
  pString = REF(char *, datastruct, dstart);
  length = x_ipcStrLen(pString)*sizeof(char);
  intToNetBytes(length, buffer+current_byte);
  current_byte += sizeof(int32);
  if (length) {
    TO_BUFFER_AND_ADVANCE(pString, buffer, current_byte, length);
  }
  else {
    TO_BUFFER_AND_ADVANCE("Z", buffer, current_byte, sizeof(char));
  }
  
  return current_byte - bstart;
}

int32 x_ipc_STR_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			     char *buffer, int32 bstart,
			     int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(alignment)
#endif
  char *pString, tmp;
  int32 current_byte, length;
  
  current_byte = bstart;
  netBytesToInt(buffer+current_byte, &length);
  current_byte += sizeof(int32);    
  if (length > 0) {
    pString = (char *)x_ipcMalloc((unsigned)(length+1));
    *((char **)(datastruct+dstart)) = pString;
    FROM_BUFFER_AND_ADVANCE(pString, buffer, current_byte, length);
    pString[length/sizeof(char)] = '\0';
  } else {
    pString = NULL;
    *((char **)(datastruct+dstart)) = pString;
    FROM_BUFFER_AND_ADVANCE(&tmp, buffer, current_byte, 1);
  }
  
  return current_byte - bstart;
}

int32 x_ipc_STR_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  int32 length;
  
  if (REF(char *, datastruct, dstart) == NULL) {
/*    X_IPC_MOD_WARNING("ERROR: A null pointer is not a string");*/
    length = 0;
  } else {
    length = x_ipcStrLen(REF(char *, datastruct, dstart)) * sizeof(char);
  }
  return sizeof(int32) + (length>0 ? length : sizeof(char));
}

int32 x_ipc_STR_Trans_RLength(void)
{ 
  return sizeof(char *);
}

int32 x_ipc_STR_Trans_ALength(void)
{ 
  return sizeof(char *);
}

int32 x_ipc_STR_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			     FILE *stream, Print_Data_Ptr printer, int32 next)
{
  (*(GET_M_GLOBAL(dPrintSTR_FN)))
    (stream, printer, REF(char *, datastruct,dstart), next);
  return TRUE;
}

int32 x_ipc_STR_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  char *pString=NULL;
  
  /* Free the pString */
  pString = REF(char *, datastruct, dstart);
  if (pString)
    x_ipcFree(pString);
  return TRUE;
}

/******************************************************************************
 *
 * FUNCTION: int32 x_ipc_FORMAT_Trans_Encode1(format, buffer, bstart)
 *
 * DESCRIPTION: 
 * A format is a (recursive) structure of {type, union{int, array, format}}
 *
 * INPUTS: 
 * FORMAT_PTR format;
 * char *buffer; 
 * int32 bstart;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

static int32 x_ipc_FORMAT_Trans_Encode1(CONST_FORMAT_PTR format,
				  char *buffer, int32 bstart)
{ 
  int32 i, current_byte;
  char PtrVal;
  FORMAT_ARRAY_PTR format_array;
  
  current_byte = bstart;
  
  intToNetBytes(format->type, buffer+current_byte);
  current_byte += sizeof(int32);
  
  switch (format->type) {
  case LengthFMT: 
  case PrimitiveFMT:
    intToNetBytes(format->formatter.i, buffer+current_byte);
    current_byte += sizeof(int32);
    break;
  case PointerFMT: 
    PtrVal = (format->formatter.f) ? 'Z' : '\0'; 
    TO_BUFFER_AND_ADVANCE(&PtrVal, buffer, current_byte, sizeof(char));
    if (format->formatter.f)
      current_byte += x_ipc_FORMAT_Trans_Encode1(format->formatter.f, 
					   buffer, current_byte);
    break;
  case StructFMT: 
    format_array = format->formatter.a;
    intToNetBytes(format_array[0].i, buffer+current_byte);
    current_byte += sizeof(int32);
    for(i=1;i<format_array[0].i;i++)
      current_byte += x_ipc_FORMAT_Trans_Encode1(format_array[i].f, buffer, 
					   current_byte);
    break;
  case VarArrayFMT:
  case FixedArrayFMT:
    format_array = format->formatter.a;
    intToNetBytes(format_array[0].i, buffer+current_byte);
    current_byte += sizeof(int32);
    current_byte += x_ipc_FORMAT_Trans_Encode1(format_array[1].f, buffer, 
					 current_byte);
    for(i=2;i<format_array[0].i;i++) {
      intToNetBytes(format_array[i].i, buffer+current_byte);
      current_byte += sizeof(int32);
    }
    break;
  case NamedFMT:
    current_byte += x_ipc_STR_Trans_Encode((GENERIC_DATA_PTR)&format->formatter.name,
				     0, buffer, current_byte);
    break;
  case BadFormatFMT:
    X_IPC_MOD_WARNING("Trying to encode a message with a bad format \n");
    break;
  case EnumFMT: 
    format_array = format->formatter.a;
    intToNetBytes(format_array[0].i, buffer+current_byte);
    current_byte += sizeof(int32);
    intToNetBytes(format_array[1].i, buffer+current_byte);
    current_byte += sizeof(int32);
    for(i=2;i<format_array[0].i;i++)
      current_byte += x_ipc_STR_Trans_Encode((GENERIC_DATA_PTR)
				       &format_array[i].f->formatter.name,
				       0, buffer, current_byte);
    break;
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR("Trying to encode a message with a unknown format \n");
    break;
#endif
  }
  return current_byte - bstart;
}


/******************************************************************************
 *
 * FUNCTION: int32 x_ipc_FORMAT_Trans_Decode1(format, buffer, bstart)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FORMAT_PTR format;
 * char *buffer; 
 * int32 bstart;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

static int32 x_ipc_FORMAT_Trans_Decode1(FORMAT_PTR format, char *buffer,
				  int32 bstart,
				  int32 byteOrder, 
				  ALIGNMENT_TYPE alignment)
{ 
  int32 i, current_byte, array_size;
  char PtrVal;
  FORMAT_ARRAY_PTR format_array;
  
  current_byte = bstart;
  
  netBytesToInt(buffer+current_byte, &i);
  format->type = (FORMAT_CLASS_TYPE)i;
  current_byte += sizeof(int32);
  
  switch (format->type) {
  case LengthFMT: 
  case PrimitiveFMT:
    netBytesToInt(buffer+current_byte, &format->formatter.i);
    current_byte += sizeof(int32);
    break;
    
  case PointerFMT: 
    FROM_BUFFER_AND_ADVANCE(&PtrVal, buffer, current_byte, sizeof(char));
    if (PtrVal == '\0') format->formatter.f = NULL;
    else {
      format->formatter.f = NEW_FORMATTER();
      current_byte += x_ipc_FORMAT_Trans_Decode1(format->formatter.f,
					   buffer, current_byte, 
					   byteOrder,alignment);
    }
    break;
    
  case StructFMT: 
    netBytesToInt(buffer+current_byte, &array_size);
    current_byte += sizeof(int32);
    format_array = format->formatter.a = NEW_FORMAT_ARRAY(array_size);
    format_array[0].i = array_size;
    for(i=1;i<array_size;i++) {
      format_array[i].f = NEW_FORMATTER();
      current_byte += x_ipc_FORMAT_Trans_Decode1(format_array[i].f, buffer,
					   current_byte, byteOrder, alignment);
    }
    break;
    
  case VarArrayFMT:
  case FixedArrayFMT:
    netBytesToInt(buffer+current_byte, &array_size);
    current_byte += sizeof(int32);
    format_array = format->formatter.a = NEW_FORMAT_ARRAY(array_size);
    format_array[0].i = array_size;
    format_array[1].f = NEW_FORMATTER();
    current_byte += x_ipc_FORMAT_Trans_Decode1(format_array[1].f, buffer, 
					 current_byte, byteOrder, alignment);
    for(i=2;i<format_array[0].i;i++) {
      netBytesToInt(buffer+current_byte, &format_array[i].i);
      current_byte += sizeof(int32);
    }
    break;
  case NamedFMT:
    current_byte += x_ipc_STR_Trans_Decode((GENERIC_DATA_PTR)
				     &format->formatter.name, 0,
				     buffer, current_byte,
				     byteOrder, alignment);
    break;
  case BadFormatFMT:
    break;

  case EnumFMT: 
    netBytesToInt(buffer+current_byte, &array_size);
    current_byte += sizeof(int32);
    format_array = format->formatter.a = NEW_FORMAT_ARRAY(array_size);
    format_array[0].i = array_size;
    netBytesToInt(buffer+current_byte, &format_array[1].i);
    current_byte += sizeof(int32);
    for(i=2;i<array_size;i++) {
      format_array[i].f = NEW_FORMATTER();
      format_array[i].f->type = NamedFMT;
      current_byte += x_ipc_STR_Trans_Decode((GENERIC_DATA_PTR)
				       &format_array[i].f->formatter.name,
				       0, buffer, current_byte,
				       byteOrder, alignment);
      LOCK_M_MUTEX;
      if (IS_LISP_MODULE())
	upcase(format_array[i].f->formatter.name);
      UNLOCK_M_MUTEX;
    }
    break;

#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_FORMAT_Trans_Decode1 Type %d",format->type);
    break;
#endif
  }
  return current_byte - bstart;
}


/******************************************************************************
 *
 * FUNCTION: int32 x_ipc_FORMAT_Trans_ELength(format) 
 *
 * DESCRIPTION:
 *
 * INPUTS: FORMAT_PTR format;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

static int32 x_ipc_FORMAT_Trans_ELength1(CONST_FORMAT_PTR format)
{ 
  int32 i, size;
  FORMAT_ARRAY_PTR format_array;
  
  size = sizeof(int32);
  
  switch (format->type) {
  case LengthFMT: 
  case PrimitiveFMT: 
    size += sizeof(int32); 
    break;
  case PointerFMT: 
    size += sizeof(char);
    if (format->formatter.f) size += 
      x_ipc_FORMAT_Trans_ELength1(format->formatter.f);
    break;
  case StructFMT: 
    format_array = format->formatter.a;
    size += sizeof(int32);
    for(i=1;i<format_array[0].i;i++)
      size += x_ipc_FORMAT_Trans_ELength1(format_array[i].f);
    break;
  case VarArrayFMT:
  case FixedArrayFMT:
    format_array = format->formatter.a;
    size += (sizeof(int32) + x_ipc_FORMAT_Trans_ELength1(format_array[1].f)
	     + (format_array[0].i-2)*sizeof(int32));
    break;
  case NamedFMT:
    size += x_ipc_STR_Trans_ELength((CONST_GENERIC_DATA_PTR)&format->formatter.name,
			      0);
    break;
  case BadFormatFMT:
    break;
  case EnumFMT:
    format_array = format->formatter.a;
    size += 2*sizeof(int32);
    for(i=2;i<format_array[0].i;i++)
      size += x_ipc_STR_Trans_ELength((CONST_GENERIC_DATA_PTR)
				&format_array[i].f->formatter.name, 0);
    break;
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Internal Error: Unknown x_ipc_FORMAT_Trans_ELength1 Type %d",format->type);
    break;
#endif
  }
  
  return size;
}


/******************************************************************************
 *
 * FUNCTION: int32 FORMAT_Trans(op, datastruct, dstart, buffer, bstart)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * TRANS_OP_TYPE op; 
 * GENERIC_DATA_PTR datastruct; 
 * int32 dstart; 
 * char *buffer; 
 * int32 bstart;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 x_ipc_FORMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{ 
  FORMAT_PTR format;
  int32 null_flag = -1;
  
  format = REF(FORMAT_PTR, datastruct, dstart);
  if (format == NULL) {
    intToNetBytes(null_flag,(buffer+bstart));
    return sizeof(int32);
  }
  else 
    return x_ipc_FORMAT_Trans_Encode1(format, buffer, bstart);
}

int32 x_ipc_FORMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  FORMAT_PTR format;
  int32 size, null_flag = -1;
  
  netBytesToInt((buffer+bstart),&null_flag);
  if (null_flag == -1) {
    format = NULL;
    size = sizeof(int32);
  }
  else {
    format = NEW_FORMATTER();
    size = x_ipc_FORMAT_Trans_Decode1(format, buffer, bstart, byteOrder, alignment);
  }
  
  BCOPY((char *)&format, (datastruct+dstart), sizeof(FORMAT_PTR));
  
  return size;
}

int32 x_ipc_FORMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  FORMAT_PTR format;
  
  format = REF(FORMAT_PTR, datastruct, dstart);
  if (format) 
    return x_ipc_FORMAT_Trans_ELength1(format);
  else 
    return sizeof(int32);
  
}

int32 x_ipc_FORMAT_Trans_RLength(void)
{ 
  return sizeof(FORMAT_PTR);
}

int32 x_ipc_FORMAT_Trans_ALength(void)
{ 
  return sizeof(FORMAT_PTR);
}

int32 x_ipc_FORMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
				FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  GET_M_GLOBAL(dPrintFORMAT_FN)(stream, printer,
				REF(FORMAT_PTR, datastruct, dstart), next);
  return TRUE; 
}

void x_ipc_freeFormatter(CONST_FORMAT_PTR *format)
{
  int i;
  FORMAT_ARRAY_PTR format_array;
  CONST_FORMAT_PTR subFormat;
  
  if (!*format) return;
  switch ((*format)->type) {
  case PrimitiveFMT:
  case LengthFMT: 
  case BadFormatFMT:
    break;
    
  case StructFMT:
    format_array = (*format)->formatter.a;
    for (i=1; i<format_array[0].i; i++) {
      subFormat = format_array[i].f;
      x_ipc_freeFormatter(&subFormat);
    }
    x_ipcFree((void *)format_array);
    break;
    
  case PointerFMT: 
    if ((*format)->formatter.f) {
      subFormat = (*format)->formatter.f;
      x_ipc_freeFormatter(&subFormat); 
    }
    break;
    
  case FixedArrayFMT:
  case VarArrayFMT:
    format_array = (*format)->formatter.a;
    subFormat = format_array[1].f;
    x_ipc_freeFormatter(&subFormat);
    x_ipcFree((void *)format_array);
    break;
    
  case NamedFMT: 
    x_ipcFree((*format)->formatter.name);
    break;
    
  case EnumFMT:
    format_array = (*format)->formatter.a;
    for (i=2; i<format_array[0].i; i++) {
      subFormat = format_array[i].f;
      x_ipc_freeFormatter(&subFormat);
    }
    x_ipcFree((void *)format_array);
    break;
  }
  x_ipcFree((void *)(*format));
  *format = NULL;
}

int32 x_ipc_FORMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  CONST_FORMAT_PTR format = (FORMAT_PTR)(datastruct+dstart);
  x_ipc_freeFormatter(&format);
  return TRUE;
}

/******************************************************************************
 *
 * FUNCTION: int32 X_IPC_REF_PTR_Trans(op, datastruct, dstart, buffer, bstart)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * TRANS_OP_TYPE op; 
 * GENERIC_DATA_PTR datastruct; 
 * int32 dstart; 
 * char *buffer; 
 * int32 bstart;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

int32 X_IPC_REF_PTR_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			     int32 dstart,
			     char *buffer, int32 bstart)
{ 
  X_IPC_REF_PTR *ref2, ref;
  int32 length, current_byte;
  
  current_byte = bstart;
  
  ref2 = (X_IPC_REF_PTR *)(datastruct+dstart);
  ref = *ref2;
  
  intToNetBytes(ref->refId, buffer+current_byte);
  current_byte += sizeof(int32);
  
  length = x_ipcStrLen(ref->name)*sizeof(char);
  intToNetBytes(length, buffer+current_byte);
  current_byte += sizeof(int32);
  
  if (length) {
    TO_BUFFER_AND_ADVANCE(ref->name, buffer, current_byte, length);
  }
  else {
    TO_BUFFER_AND_ADVANCE("Z", buffer, current_byte, sizeof(char));
  }
  
  return current_byte - bstart;
}

int32 X_IPC_REF_PTR_Trans_Decode(GENERIC_DATA_PTR datastruct,
			     int32 dstart,
			     char *buffer, int32 bstart,
			     int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(alignment)
#endif
  char tmp;
  X_IPC_REF_PTR ref;
  int32 length, current_byte;
  
  current_byte = bstart;
  
  ref = x_ipcRefCreate(NULL, NULL, 0);
  
  netBytesToInt(buffer+current_byte, &ref->refId);
  current_byte += sizeof(int32);
  
  netBytesToInt(buffer+current_byte, &length);
  current_byte += sizeof(int32);
  
  if (length) {
    char *name;
    name = (char *)x_ipcMalloc((unsigned)(length+1));
    FROM_BUFFER_AND_ADVANCE(name, buffer, current_byte, length);
    name[length/sizeof(char)] = '\0';
    ref->name = (const char *)name;
  }
  else {
    FROM_BUFFER_AND_ADVANCE(&tmp, buffer, current_byte, 1);
  }
  
  BCOPY((char *)&ref, (datastruct+dstart), sizeof(X_IPC_REF_PTR));
  
  return current_byte - bstart;
  
}

int32 X_IPC_REF_PTR_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  X_IPC_REF_PTR *ref2, ref;
  int32 length;
  
  ref2 = (X_IPC_REF_PTR *)(datastruct+dstart);
  ref = *ref2;
  
  length = x_ipcStrLen(ref->name)*sizeof(char);
  
  if (length)
    return(sizeof(int32)+length+sizeof(int32));
  else
    return(sizeof(int32)+sizeof(char)+sizeof(int32));
  
}

int32 X_IPC_REF_PTR_Trans_ALength(void)
{ 
  return sizeof(X_IPC_REF_PTR);
}

int32 X_IPC_REF_PTR_Trans_RLength(void)
{ 
  return sizeof(X_IPC_REF_PTR);
}

int32 X_IPC_REF_PTR_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, 
				 int32 dstart, FILE *stream,
				 Print_Data_Ptr printer, int32 next)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(next)
#endif
  X_IPC_REF_PTR *ref2 = (X_IPC_REF_PTR *)(datastruct+dstart);

  GET_M_GLOBAL(dPrintX_IPC_FN)(stream, printer, *ref2, 0);
  return TRUE; 
}

int32 X_IPC_REF_PTR_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  X_IPC_REF_PTR *ref2 = (X_IPC_REF_PTR *)(datastruct+dstart);

  x_ipcRefFree(*ref2);
  return TRUE;
}

/******************************************************************************
 *
 * FUNCTIONS:
 *  int32 Basic_Trans_Encode(datastruct, dstart, buffer, bstart, x_ipc_elementSize)
 *  int32 Basic_Trans_Decode(datastruct, dstart, buffer, bstart, 
 *                         byteOrder, alignment, x_ipc_elementSize)
 *  int32 Basic_Trans_ELength(x_ipc_elementSize)
 *  int32 Basic_Trans_RLength(x_ipc_elementSize)
 *  int32 Basic_Trans_ALength(x_ipc_elementSize)
 *  int32 x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, next, printFn)
 *  int32 Basic_Trans_DFree(datastruct, dstart)
 *  int32 Basic_Trans(op, datastruct, dstart, buffer, bstart,
 *                  x_ipc_elementSize, elementFormat)
 *
 * DESCRIPTION: Generic Translation Functions for the Basic Data Types
 *
 *****************************************************************************/

static INLINE int32 Basic_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
				     int32 dstart, char *buffer,
				     int32 bstart, int32 size)
{ 
  if (size == 2 /* bytes */) {
    shortToNetBytes(*(int16 *)(datastruct+dstart), (buffer+bstart));
  } else if (size == 4 /* bytes */) {
    intToNetBytes(*(int32 *)(datastruct+dstart), (buffer+bstart));
  } else if (size == 8 /* bytes */) {
    doubleToNetBytes(*(double *)(datastruct+dstart), (buffer+bstart));
  } else {
    BCOPY((datastruct+dstart), (buffer+bstart), size);
  }
  return size;
}

static INLINE int32 Basic_Trans_Decode(GENERIC_DATA_PTR datastruct,
				       int32 dstart,
				       char *buffer, int32 bstart,
				       int32 byteOrder,
				       ALIGNMENT_TYPE alignment,
				       int32 size)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(alignment)
#endif
  if (size == 2 /* bytes */) {
    netBytesToShort((buffer+bstart), (short *)(datastruct+dstart));
  } else if (size == 4 /* bytes */) {
    netBytesToInt((buffer+bstart), (int *)(datastruct+dstart));
  } else if (size == 8 /* bytes */) {
    netBytesToDouble((buffer+bstart), (double *)(datastruct+dstart));
  } else {
    BCOPY((buffer+bstart), (datastruct+dstart), size);
  }
  return size;  
}

static INLINE int32 Basic_Trans_ELength(int32 x_ipc_elementSize)
{ 
  return x_ipc_elementSize;
}

static INLINE int32 Basic_Trans_RLength(int32 x_ipc_elementSize)
{ 
  return x_ipc_elementSize;
}

static INLINE int32 Basic_Trans_ALength(int32 x_ipc_elementSize)
{ 
  return x_ipc_elementSize;
}

static int32 x_ipc_Basic_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				      int32 dstart,
				      FILE *stream, Print_Data_Ptr printer,
				      int32 next, PRINT_FN printFn)
{ 
  (printFn)(stream, printer, (datastruct+dstart), next);
  FLUSH_IF_NEEDED(stdout);
  return TRUE;
}

static INLINE int32 Basic_Trans_DFree(GENERIC_DATA_PTR datastruct,
				      int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return FALSE;
}

/*****************************************************************
 * 
 * Basic CHAR Format Functions
 *
 ****************************************************************/

int32 x_ipc_CHAR_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(char));
}

int32 x_ipc_CHAR_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(char));
}

int32 x_ipc_CHAR_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(char));
}

int32 x_ipc_CHAR_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(char));
}

int32 x_ipc_CHAR_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(char));
}

int32 x_ipc_CHAR_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			      FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintCHAR_FN));
}

int32 x_ipc_CHAR_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic SHORT Format Functions
 *
 ****************************************************************/

int32 x_ipc_SHORT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(int16));
}

int32 x_ipc_SHORT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(int16));
}

int32 x_ipc_SHORT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(int16));
}

int32 x_ipc_SHORT_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(int16));
}

int32 x_ipc_SHORT_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(int16));
}

int32 x_ipc_SHORT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			       FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintSHORT_FN));
}

int32 x_ipc_SHORT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic INT32 Format Functions
 *
 ****************************************************************/

int32 x_ipc_INT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		     char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(int32));
}

int32 x_ipc_INT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		     char *buffer, int32 bstart,
		     int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(int32));
}

int32 x_ipc_INT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(int32));
}

int32 x_ipc_INT_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(int32));
}

int32 x_ipc_INT_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(int32));
}

int32 x_ipc_INT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			     FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next,
				  (PRINT_FN)GET_M_GLOBAL(dPrintINT_FN));
}

int32 x_ipc_INT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic LONG Format Functions
 *
 ****************************************************************/

int32 x_ipc_LONG_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(long));
}

int32 x_ipc_LONG_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(long));
}

int32 x_ipc_LONG_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(long));
}

int32 x_ipc_LONG_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(long));
}

int32 x_ipc_LONG_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(long));
}

int32 x_ipc_LONG_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			      FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintLONG_FN));
}

int32 x_ipc_LONG_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic FLOAT Format Functions
 *
 ****************************************************************/

int32 x_ipc_FLOAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(float));
}

int32 x_ipc_FLOAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(float));
}

int32 x_ipc_FLOAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(float));
}

int32 x_ipc_FLOAT_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(float));
}

int32 x_ipc_FLOAT_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(float));
}

int32 x_ipc_FLOAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			       FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintFLOAT_FN));
}

int32 x_ipc_FLOAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic DOUBLE Format Functions
 *
 ****************************************************************/

int32 x_ipc_DOUBLE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(double));
}

int32 x_ipc_DOUBLE_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(double));
}

int32 x_ipc_DOUBLE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(double));
}

int32 x_ipc_DOUBLE_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(double));
}

int32 x_ipc_DOUBLE_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(double));
}

int32 x_ipc_DOUBLE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
				FILE *stream, Print_Data_Ptr printer,
				int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintDOUBLE_FN));
}

int32 x_ipc_DOUBLE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic BOOLEAN Format Functions
 *
 ****************************************************************/

int32 x_ipc_BOOLEAN_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, 
			    sizeof(BOOLEAN));
}

int32 x_ipc_BOOLEAN_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			 char *buffer, int32 bstart,
			 int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(BOOLEAN));
}

int32 x_ipc_BOOLEAN_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(BOOLEAN));
}

int32 x_ipc_BOOLEAN_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(BOOLEAN));
}

int32 x_ipc_BOOLEAN_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(BOOLEAN));
}

int32 x_ipc_BOOLEAN_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart, FILE *stream,
				 Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintBOOLEAN_FN));
}

int32 x_ipc_BOOLEAN_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic UBYTE Format Functions
 *
 ****************************************************************/

int32 x_ipc_UBYTE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, 
			    sizeof(unsigned char));
}

int32 x_ipc_UBYTE_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(unsigned char));
}

int32 x_ipc_UBYTE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(unsigned char));
}

int32 x_ipc_UBYTE_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(unsigned char));
}

int32 x_ipc_UBYTE_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(unsigned char));
}

int32 x_ipc_UBYTE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			       FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintUBYTE_FN));
}

int32 x_ipc_UBYTE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic BYTE Format Functions
 *
 ****************************************************************/

int32 x_ipc_BYTE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart,
			    sizeof(signed char));
}

int32 x_ipc_BYTE_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(signed char));
}

int32 x_ipc_BYTE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(signed char));
}

int32 x_ipc_BYTE_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(signed char));
}

int32 x_ipc_BYTE_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(signed char));
}

int32 x_ipc_BYTE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			      FILE *stream, Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
			    (PRINT_FN)GET_M_GLOBAL(dPrintBYTE_FN));
}

int32 x_ipc_BYTE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * Basic TWOBYTE Format Functions
 *
 ****************************************************************/

int32 x_ipc_TWOBYTE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 char *buffer, int32 bstart)
{ 
  return Basic_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(int16));
}

int32 x_ipc_TWOBYTE_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			 char *buffer, int32 bstart,
			 int32 byteOrder, ALIGNMENT_TYPE alignment)
{ 
  return Basic_Trans_Decode(datastruct, dstart, buffer, bstart,
			    byteOrder, alignment, sizeof(int16));
}

int32 x_ipc_TWOBYTE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
#ifdef UNUSED_PRAGMA
#pragma unused(datastruct,dstart)
#endif
  return Basic_Trans_ELength(sizeof(int16));
}

int32 x_ipc_TWOBYTE_Trans_ALength(void)
{ 
  return Basic_Trans_ALength(sizeof(int16));
}

int32 x_ipc_TWOBYTE_Trans_RLength(void)
{ 
  return Basic_Trans_RLength(sizeof(int16));
}

int32 x_ipc_TWOBYTE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart, FILE *stream,
				 Print_Data_Ptr printer, int32 next)
{ 
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next, 
				  (PRINT_FN)GET_M_GLOBAL(dPrintTWOBYTE_FN));
}

int32 x_ipc_TWOBYTE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  return Basic_Trans_DFree(datastruct, dstart);
}
#ifndef NMP_IPC /* NMP doesn't need all this baggage */

/******************************************************************************
 *
 * FUNCTION:
 * int32 matrixLength(x_ipc_elementSize, lb1, ub1, lb2, ub2)
 *
 * DESCRIPTION: Calculate matrix encode/decode size.
 *
 * INPUTS: int32 x_ipc_elementSize, lb1, ub1, lb2, ub2;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

static INLINE int32 matrixLength(int32 x_ipc_elementSize,
			       int32 lb1, int32 ub1, int32 lb2, int32 ub2)
{ 
  return x_ipc_elementSize * (ub1 - lb1 + 1) * (ub2 - lb2 + 1);
}


/******************************************************************************
 *
 * FUNCTION:
 * int32 matrixSize(x_ipc_elementSize, lb1, ub1, lb2, ub2)
 *
 * DESCRIPTION: Calculate the number of element in the matrix.
 *
 * INPUTS: int32 x_ipc_elementSize, lb1, ub1, lb2, ub2;
 *
 * OUTPUTS: int
 *
 *****************************************************************************/

static INLINE int32 matrixSize(int32 lb1, int32 ub1, int32 lb2, int32 ub2)
{ 
  return (ub1 - lb1 + 1) * (ub2 - lb2 + 1);
}

static INLINE void transferMapBounds (int32 lb1, int32 ub1, int32 lb2,
				      int32 ub2, 
				      char *buffer, int32 *currentBytePtr)
{
  intToNetBytes(lb1, buffer+(*currentBytePtr));
  *currentBytePtr += sizeof(int32);
  intToNetBytes(ub1, buffer+(*currentBytePtr));
  *currentBytePtr += sizeof(int32);
  intToNetBytes(lb2, buffer+(*currentBytePtr));
  *currentBytePtr += sizeof(int32);
  intToNetBytes(ub2, buffer+(*currentBytePtr));
  *currentBytePtr += sizeof(int32);
}

static INLINE void retrieveMapBounds (int32 *lb1, int32 *ub1,
				      int32 *lb2, int32 *ub2,
				      char *buffer, int32 *currentBytePtr,
				      int32 byteOrder,
				      ALIGNMENT_TYPE alignment)
{
  netBytesToInt(buffer+*currentBytePtr, lb1);
  *currentBytePtr += sizeof(int32);
  netBytesToInt(buffer+*currentBytePtr, ub1);
  *currentBytePtr += sizeof(int32);
  netBytesToInt(buffer+*currentBytePtr, lb2);
  *currentBytePtr += sizeof(int32);
  netBytesToInt(buffer+*currentBytePtr, ub2);
  *currentBytePtr += sizeof(int32);
}

/******************************************************************************
 *
 * FUNCTIONS:
 *  int32 Matrix_Trans_Encode(datastruct, dstart, buffer, bstart, x_ipc_elementSize)
 *  int32 Matrix_Trans_Decode(datastruct, dstart, buffer, bstart, 
 *                          byteOrder, alignment, x_ipc_elementSize)
 *  int32 Matrix_Trans_ELength(datastruct, dstart, x_ipc_elementSize)
 *  int32 Matrix_Trans_RLength(void)
 *  int32 Matrix_Trans_ALength(void)
 *  int32 Matrix_Trans_DPrint(datastruct, dstart, stream, next,
 *                          x_ipc_elementSize, elementFormat)
 *  int32 Matrix_Trans_DFree(datastruct, dstart)
 *  int32 Matrix_Trans(op, datastruct, dstart, buffer, bstart,
 *                   x_ipc_elementSize, elementFormat)
 *
 * DESCRIPTION: Generic Matrix Translation Functions
 *
 *****************************************************************************/

static int32 Matrix_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart,
				 char *buffer, int32 bstart, int32 x_ipc_elementSize)
{ 
  genericMatrix *Map;
  int32 length, current_byte;
  char *mapElement;
  
  current_byte = bstart;
  Map = (genericMatrix *)(datastruct+dstart);
  length = matrixLength(x_ipc_elementSize, Map->lb1, Map->ub1, Map->lb2, Map->ub2);
  transferMapBounds(Map->lb1, Map->ub1, Map->lb2, Map->ub2, 
		    buffer, &current_byte);
  mapElement = MATRIX_FIRST_ELEMENT(*Map, x_ipc_elementSize);
  TO_BUFFER_AND_ADVANCE(mapElement, buffer, current_byte, length);
  return current_byte - bstart;
}

static int32 Matrix_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
				 char *buffer, int32 bstart, int32 byteOrder,
				 ALIGNMENT_TYPE alignment, int32 x_ipc_elementSize)
{ 
  genericMatrix *Map;
  int32 current_byte, lb1, ub1, lb2, ub2, length, i, elemIndex;
  int32 error;
  char *mapElement;

  current_byte = bstart;
  retrieveMapBounds(&lb1, &ub1, &lb2, &ub2, buffer, &current_byte,
		    byteOrder, alignment);
  Map = (genericMatrix *)(datastruct+dstart);
  *Map = newmat(x_ipc_elementSize, lb1, ub1, lb2, ub2, &error);
  if (error) {
    X_IPC_MOD_ERROR("Matrix allocation error");
  } else if (!Map) { 
    X_IPC_MOD_ERROR("Matrix_Trans_Decode. Map cannot be allocated");
  } else { 
    mapElement = MATRIX_ELEMENT(*Map, lb1, lb2, x_ipc_elementSize);
    if (byteOrder == BYTE_ORDER || x_ipc_elementSize == 1) {
      length = matrixLength(x_ipc_elementSize, lb1, ub1, lb2, ub2);
      FROM_BUFFER_AND_ADVANCE(mapElement, buffer, current_byte, length);
    } else {
      length = matrixSize(lb1, ub1, lb2, ub2);
      for (i=0, elemIndex=0; i<length; i++) {
	Basic_Trans_Decode(mapElement, elemIndex, buffer, current_byte,
			  byteOrder, alignment,  x_ipc_elementSize);
	current_byte += x_ipc_elementSize;
	elemIndex += x_ipc_elementSize;
      }
    }
  }
  return current_byte - bstart;
}

static int32 Matrix_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct,
				  int32 dstart,
				  int32 x_ipc_elementSize)
{ 
  genericMatrix *Map;
  
  Map = (genericMatrix *)(datastruct+dstart);
  return (matrixLength(x_ipc_elementSize, Map->lb1, Map->ub1, Map->lb2, Map->ub2)
	  + 4*sizeof(int32));
}

static INLINE int32 Matrix_Trans_RLength(void)
{ 
  return sizeof(int32 *);
}

static INLINE int32 Matrix_Trans_ALength(void)
{ 
  return sizeof(genericMatrix);
}

static int32 Matrix_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart,
				 FILE *stream, int32 next,
				 int32 x_ipc_elementSize,
				 CONST_FORMAT_PTR elementFormat)
{ 
  genericMatrix *Map = (genericMatrix *)(datastruct+dstart);
  GET_M_GLOBAL(dPrintMAP_FN)(stream, Map, elementFormat, next,
			     MATRIX_FIRST_ELEMENT(*Map, x_ipc_elementSize));
  return TRUE; /* dummy */
}

static int32 Matrix_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{ 
  genericMatrix *Map;
  
  Map = (genericMatrix *)(datastruct+dstart);
  FREEMAT(*Map);
  return TRUE;
}

/*****************************************************************
 * 
 * UCMAT (unsigned char) Matrix Format Functions
 *
 ****************************************************************/

int32 UCMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			     sizeof(unsigned char));
}

int32 UCMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		       char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(unsigned char));
}

int32 UCMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(unsigned char));
}

int32 UCMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 UCMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 UCMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		       FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(unsigned char), GET_M_GLOBAL(byteFormat));
}

int32 UCMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * CMAT (char) Matrix Format Functions
 *
 ****************************************************************/

int32 CMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(char));
}

int32 CMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(char));
}

int32 CMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(char));
}

int32 CMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 CMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 CMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(char), GET_M_GLOBAL(charFormat));
}

int32 CMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SMAT (short) Matrix Format Functions
 *
 ****************************************************************/

int32 SMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			     sizeof(int16));
}

int32 SMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(int16));
}

int32 SMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(int16));
}

int32 SMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(int16), GET_M_GLOBAL(shortFormat));
}

int32 SMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * IMAT (int32) Matrix Format Functions
 *
 ****************************************************************/

int32 IMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			     sizeof(int32));
}

int32 IMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(int32));
}

int32 IMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(int32));
}

int32 IMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 IMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 IMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(int32), GET_M_GLOBAL(intFormat));
}

int32 IMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * LMAT (long) Matrix Format Functions
 *
 ****************************************************************/

int32 LMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart, sizeof(long));
}

int32 LMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(long));
}

int32 LMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(long));
}

int32 LMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 LMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 LMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(long), GET_M_GLOBAL(longFormat));
}

int32 LMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * FMAT (float) Matrix Format Functions
 *
 ****************************************************************/

int32 FMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			     sizeof(float));
}

int32 FMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(float));
}

int32 FMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(float));
}

int32 FMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 FMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 FMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(float), GET_M_GLOBAL(floatFormat));
}

int32 FMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * DMAT (double) Matrix Format Functions
 *
 ****************************************************************/

int32 DMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart)
{
  return Matrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			     sizeof(double));
}

int32 DMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
		      char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(double));
}

int32 DMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(double));
}

int32 DMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 DMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 DMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
		      FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(double), GET_M_GLOBAL(doubleFormat));
}

int32 DMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/******************************************************************************
 *
 * FUNCTIONS:
 *  int32 SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart, x_ipc_elementSize)
 *  int32 SIMatrix_Trans(op, datastruct, dstart, buffer, bstart,
 *                     x_ipc_elementSize, elementFormat)
 *
 * DESCRIPTION: Generic Matrix Translation Functions for Sub-Matrices
 *              (the Decode, ELength, ALength, RLength, DFree, DPrint 
 *              functions are the same as those used to translate the
 *              full-sized matrices)
 *
 *****************************************************************************/

static int32 SIMatrix_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
				   int32 dstart,
				   char *buffer, int32 bstart,
				   int32 x_ipc_elementSize)
{
  genericMatrix *Map;
  int32 i, length, current_byte;
  char *mapElement;
  
  current_byte = bstart;
  Map = (genericMatrix *)(datastruct+dstart);
  length = matrixLength(x_ipc_elementSize, Map->lb1, Map->ub1, Map->lb2, Map->ub2);
  transferMapBounds(Map->lb1, Map->ub1, Map->lb2, Map->ub2, 
		    buffer, &current_byte);

  length = x_ipc_elementSize * (Map->ub2 - Map->lb2 + 1);

  for (i=Map->lb1; i<= Map->ub1;i++) {
    mapElement = MATRIX_ELEMENT(*Map, i, Map->lb2, x_ipc_elementSize);
    TO_BUFFER_AND_ADVANCE(mapElement, buffer, current_byte, length);
  }
  return current_byte - bstart;
}

/*****************************************************************
 * 
 * SIUCMAT (unsigned char) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SIUCMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			       sizeof(unsigned char));
}

int32 SIUCMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			 char *buffer, int32 bstart,
			 int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(unsigned char));
}

int32 SIUCMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(unsigned char));
}

int32 SIUCMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SIUCMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SIUCMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(unsigned char), GET_M_GLOBAL(byteFormat));
}

int32 SIUCMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SICMAT (char) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SICMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart, 
			       sizeof(char));
}

int32 SICMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(char));
}

int32 SICMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(char));
}

int32 SICMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SICMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SICMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(char), GET_M_GLOBAL(charFormat));
}

int32 SICMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SISMAT (short) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SISMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			       sizeof(int16));
}

int32 SISMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(int16));
}

int32 SISMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(int16));
}

int32 SISMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SISMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SISMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(int16), GET_M_GLOBAL(shortFormat));
}

int32 SISMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SIIMAT (int32) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SIIMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			       sizeof(int32));
}

int32 SIIMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(int32));
}

int32 SIIMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(int32));
}

int32 SIIMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SIIMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SIIMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(int32), GET_M_GLOBAL(intFormat));
}

int32 SIIMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SILMAT (long) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SILMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			       sizeof(long));
}

int32 SILMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(long));
}

int32 SILMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(long));
}

int32 SILMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SILMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SILMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(long), GET_M_GLOBAL(longFormat));
}

int32 SILMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SIFMAT (float) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SIFMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			       sizeof(float));
}

int32 SIFMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(float));
}

int32 SIFMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(float));
}

int32 SIFMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SIFMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SIFMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(float), GET_M_GLOBAL(floatFormat));
}

int32 SIFMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}

/*****************************************************************
 * 
 * SIDMAT (double) Sub-Matrix Format Functions
 *
 ****************************************************************/

int32 SIDMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart)
{
  return SIMatrix_Trans_Encode(datastruct, dstart, buffer, bstart,
			       sizeof(double));
}

int32 SIDMAT_Trans_Decode(GENERIC_DATA_PTR datastruct, int32 dstart,
			char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment)
{
  return Matrix_Trans_Decode(datastruct, dstart, buffer, bstart,
			     byteOrder, alignment, sizeof(double));
}

int32 SIDMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_ELength(datastruct, dstart, sizeof(double));
}

int32 SIDMAT_Trans_RLength(void)
{ 
  return Matrix_Trans_RLength();
}

int32 SIDMAT_Trans_ALength(void)
{ 
  return Matrix_Trans_ALength();
}

int32 SIDMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, int32 next)
{
  return Matrix_Trans_DPrint(datastruct, dstart, stream, next,
			     sizeof(double), GET_M_GLOBAL(doubleFormat));
}

int32 SIDMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart)
{
  return Matrix_Trans_DFree(datastruct, dstart);
}
#endif /* NMP_IPC */

/*****************************************************************
 * In C the unsigned versions of the formatters behave just like the 
 *   signed versions, except for printing.
 ****************************************************************/

int32 USHORT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next)
{
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next,
			    (PRINT_FN)GET_M_GLOBAL(dPrintUSHORT_FN));
}

int32 UINT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next)
{
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next,
			    (PRINT_FN)GET_M_GLOBAL(dPrintUINT_FN));
}

int32 ULONG_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 FILE *stream, Print_Data_Ptr printer, int32 next)
{
  return x_ipc_Basic_Trans_DPrint(datastruct, dstart, stream, printer, next,
			    (PRINT_FN)GET_M_GLOBAL(dPrintULONG_FN));
}

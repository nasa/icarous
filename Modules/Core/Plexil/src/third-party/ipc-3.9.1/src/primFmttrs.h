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
 * $Source: /afs/cs.cmu.edu/project/TCA/Master/ipc/src/primFmttrs.h,v $ 
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY:
 *
 * $Log: primFmttrs.h,v $
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
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
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.5  1997/01/27 20:39:42  reids
 * For Lisp, force all format enum values to uppercase; Yields more efficient
 *   C <=> Lisp conversion of enumerated types.
 *
 * Revision 1.1.2.4  1997/01/27 20:09:51  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.2  1997/01/11 01:21:14  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.1.6.2  1996/12/27 19:26:06  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.1.2.1.6.1  1996/12/24 14:38:28  reids
 * Removed Lisp-specific code that was actually no longer being used.
 *
 * Revision 1.1.2.1  1996/10/18 18:15:32  reids
 * Fixed freeing of formatters.
 *
 * Revision 1.1  1996/05/09 01:01:48  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.2  1996/03/19 03:38:49  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.1  1996/03/03 04:32:12  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.11  1996/06/25  20:51:13  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.10  1996/05/09  18:31:28  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.9  1996/03/19  02:29:31  reids
 * Plugged some more memory leaks; Added test code for enum formatters.
 * Added code to free formatter data structures.
 *
 * Revision 1.8  1995/05/31  19:36:10  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.7  1995/04/19  14:28:39  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.6  1995/04/05  19:11:03  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.5  1995/01/30  16:18:23  rich
 * Added indications of message byte order and alignment to the message
 * header in the upper two bytes of the classID.
 * Now the receiver translates the byte order but must receive data in
 * "PACKED" alignment.
 * Made -gstabs come after -g for i386_mach machines so the debugger will work.
 *
 * Revision 1.4  1995/01/18  22:41:53  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.3  1994/05/17  23:17:03  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.2  1993/11/21  20:18:56  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.1  1993/08/27  07:16:07  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/08/23  17:39:41  rich
 * Fixed the type definitions for function pointers. Added const
 * declarations.  Removed definitions VOID_FN and INT_FN.
 *
 * Revision 1.2  1993/05/27  22:19:22  rich
 * Added automatic logging.
 *
 ****************************************************************/

#ifndef INCprimFmttrs
#define INCprimFmttrs

typedef void (* PRINT_FN)(FILE *, Print_Data_Ptr, const void *, int32);

#define BASIC_TRANS_SIMPLETYPE       TRUE
#define STR_TRANS_SIMPLETYPE         FALSE
#define FORMAT_TRANS_SIMPLETYPE      FALSE
#define MATRIX_TRANS_SIMPLETYPE      FALSE
#define X_IPC_REF_PTR_TRANS_SIMPLETYPE FALSE

#define CHAR_TRANS_SIMPLETYPE    BASIC_TRANS_SIMPLETYPE
#define SHORT_TRANS_SIMPLETYPE   BASIC_TRANS_SIMPLETYPE
#define LONG_TRANS_SIMPLETYPE    BASIC_TRANS_SIMPLETYPE
#define INT_TRANS_SIMPLETYPE     BASIC_TRANS_SIMPLETYPE
#define FLOAT_TRANS_SIMPLETYPE   BASIC_TRANS_SIMPLETYPE
#define BOOLEAN_TRANS_SIMPLETYPE BASIC_TRANS_SIMPLETYPE
#define DOUBLE_TRANS_SIMPLETYPE  BASIC_TRANS_SIMPLETYPE
#define BYTE_TRANS_SIMPLETYPE    BASIC_TRANS_SIMPLETYPE
#define UBYTE_TRANS_SIMPLETYPE   BASIC_TRANS_SIMPLETYPE
#define TWOBYTE_TRANS_SIMPLETYPE BASIC_TRANS_SIMPLETYPE

#define UCMAT_TRANS_SIMPLETYPE MATRIX_TRANS_SIMPLETYPE
#define CMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define SMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define IMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define LMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define FMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define DMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE

#define SIUCMAT_TRANS_SIMPLETYPE MATRIX_TRANS_SIMPLETYPE
#define SICMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define SISMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define SIIMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define SILMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define SIFMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE
#define SIDMAT_TRANS_SIMPLETYPE  MATRIX_TRANS_SIMPLETYPE

void upcase (char *str);

int32 x_ipc_STR_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		     int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_STR_Trans_Decode(GENERIC_DATA_PTR datastruct,
		     int32 dstart, char *buffer, int32 bstart,
		     int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_STR_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_STR_Trans_ALength(void);
int32 x_ipc_STR_Trans_RLength(void);
int32 x_ipc_STR_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			     FILE *stream, Print_Data_Ptr printer, int32 next);
int32 x_ipc_STR_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_FORMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_FORMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_FORMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_FORMAT_Trans_ALength(void);
int32 x_ipc_FORMAT_Trans_RLength(void);
int32 x_ipc_FORMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
				FILE *stream, Print_Data_Ptr printer,
				int32 next);
void x_ipc_freeFormatter(CONST_FORMAT_PTR *format);
int32 x_ipc_FORMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 UCMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart);
int32 UCMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 UCMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 UCMAT_Trans_ALength(void);
int32 UCMAT_Trans_RLength(void);
int32 UCMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 FILE *stream, Print_Data_Ptr printer, int32 next);
int32 UCMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 CMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 CMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 CMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 CMAT_Trans_ALength(void);
int32 CMAT_Trans_RLength(void);
int32 CMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 CMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 SMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SMAT_Trans_ALength(void);
int32 SMAT_Trans_RLength(void);
int32 SMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 IMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 IMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 IMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 IMAT_Trans_ALength(void);
int32 IMAT_Trans_RLength(void);
int32 IMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 IMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 LMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 LMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 LMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 LMAT_Trans_ALength(void);
int32 LMAT_Trans_RLength(void);
int32 LMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 LMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 FMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 FMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 FMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 FMAT_Trans_ALength(void);
int32 FMAT_Trans_RLength(void);
int32 FMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 FMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 DMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 DMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 DMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 DMAT_Trans_ALength(void);
int32 DMAT_Trans_RLength(void);
int32 DMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 DMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SIUCMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			 int32 dstart, char *buffer, int32 bstart);
int32 SIUCMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			 int32 dstart, char *buffer, int32 bstart,
			 int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SIUCMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SIUCMAT_Trans_ALength(void);
int32 SIUCMAT_Trans_RLength(void);
int32 SIUCMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			   FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SIUCMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SICMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SICMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SICMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SICMAT_Trans_ALength(void);
int32 SICMAT_Trans_RLength(void);
int32 SICMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SICMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SICMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SICMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SICMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SICMAT_Trans_ALength(void);
int32 SICMAT_Trans_RLength(void);
int32 SICMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SICMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SISMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SISMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SISMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SISMAT_Trans_ALength(void);
int32 SISMAT_Trans_RLength(void);
int32 SISMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SISMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SIIMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SIIMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SIIMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SIIMAT_Trans_ALength(void);
int32 SIIMAT_Trans_RLength(void);
int32 SIIMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SIIMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SILMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SILMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SILMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SILMAT_Trans_ALength(void);
int32 SILMAT_Trans_RLength(void);
int32 SILMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SILMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SIFMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SIFMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SIFMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SIFMAT_Trans_ALength(void);
int32 SIFMAT_Trans_RLength(void);
int32 SIFMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SIFMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 SIDMAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 SIDMAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 SIDMAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 SIDMAT_Trans_ALength(void);
int32 SIDMAT_Trans_RLength(void);
int32 SIDMAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 SIDMAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 X_IPC_REF_PTR_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			     int32 dstart, char *buffer, int32 bstart);
int32 X_IPC_REF_PTR_Trans_Decode(GENERIC_DATA_PTR datastruct,
			     int32 dstart, char *buffer, int32 bstart,
			     int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 X_IPC_REF_PTR_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 X_IPC_REF_PTR_Trans_ALength(void);
int32 X_IPC_REF_PTR_Trans_RLength(void);
int32 X_IPC_REF_PTR_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart, FILE *stream,
				 Print_Data_Ptr printer, int32 next);
int32 X_IPC_REF_PTR_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_CHAR_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_CHAR_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_CHAR_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_CHAR_Trans_ALength(void);
int32 x_ipc_CHAR_Trans_RLength(void);
int32 x_ipc_CHAR_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			      FILE *stream, Print_Data_Ptr printer, int32 next);
int32 x_ipc_CHAR_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_SHORT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_SHORT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_SHORT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_SHORT_Trans_ALength(void);
int32 x_ipc_SHORT_Trans_RLength(void);
int32 x_ipc_SHORT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			       FILE *stream, Print_Data_Ptr printer,
			       int32 next);
int32 x_ipc_SHORT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_LONG_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_LONG_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_LONG_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_LONG_Trans_ALength(void);
int32 x_ipc_LONG_Trans_RLength(void);
int32 x_ipc_LONG_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			      FILE *stream, Print_Data_Ptr printer, int32 next);
int32 x_ipc_LONG_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_INT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		     int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_INT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		     int32 dstart, char *buffer, int32 bstart,
		     int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_INT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_INT_Trans_ALength(void);
int32 x_ipc_INT_Trans_RLength(void);
int32 x_ipc_INT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			     FILE *stream, Print_Data_Ptr printer, int32 next);
int32 x_ipc_INT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_FLOAT_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_FLOAT_Trans_Decode(GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_FLOAT_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_FLOAT_Trans_ALength(void);
int32 x_ipc_FLOAT_Trans_RLength(void);
int32 x_ipc_FLOAT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			       FILE *stream, Print_Data_Ptr printer,
			       int32 next);
int32 x_ipc_FLOAT_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_DOUBLE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_DOUBLE_Trans_Decode(GENERIC_DATA_PTR datastruct,
			int32 dstart, char *buffer, int32 bstart,
			int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_DOUBLE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_DOUBLE_Trans_ALength(void);
int32 x_ipc_DOUBLE_Trans_RLength(void);
int32 x_ipc_DOUBLE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
				FILE *stream, Print_Data_Ptr printer,
				int32 next);
int32 x_ipc_DOUBLE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_BOOLEAN_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			 int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_BOOLEAN_Trans_Decode(GENERIC_DATA_PTR datastruct,
			 int32 dstart, char *buffer, int32 bstart,
			 int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_BOOLEAN_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_BOOLEAN_Trans_ALength(void);
int32 x_ipc_BOOLEAN_Trans_RLength(void);
int32 x_ipc_BOOLEAN_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart, FILE *stream,
				 Print_Data_Ptr printer, int32 next);
int32 x_ipc_BOOLEAN_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_BYTE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_BYTE_Trans_Decode(GENERIC_DATA_PTR datastruct,
		      int32 dstart, char *buffer, int32 bstart,
		      int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_BYTE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_BYTE_Trans_ALength(void);
int32 x_ipc_BYTE_Trans_RLength(void);
int32 x_ipc_BYTE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			      FILE *stream, Print_Data_Ptr printer, int32 next);
int32 x_ipc_BYTE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_UBYTE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_UBYTE_Trans_Decode(GENERIC_DATA_PTR datastruct,
		       int32 dstart, char *buffer, int32 bstart,
		       int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_UBYTE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_UBYTE_Trans_ALength(void);
int32 x_ipc_UBYTE_Trans_RLength(void);
int32 x_ipc_UBYTE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			       FILE *stream, Print_Data_Ptr printer,
			       int32 next);
int32 x_ipc_UBYTE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

int32 x_ipc_TWOBYTE_Trans_Encode(CONST_GENERIC_DATA_PTR datastruct,
			 int32 dstart, char *buffer, int32 bstart);
int32 x_ipc_TWOBYTE_Trans_Decode(GENERIC_DATA_PTR datastruct,
			 int32 dstart, char *buffer, int32 bstart,
			 int32 byteOrder, ALIGNMENT_TYPE alignment);
int32 x_ipc_TWOBYTE_Trans_ELength(CONST_GENERIC_DATA_PTR datastruct, int32 dstart);
int32 x_ipc_TWOBYTE_Trans_ALength(void);
int32 x_ipc_TWOBYTE_Trans_RLength(void);
int32 x_ipc_TWOBYTE_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct,
				 int32 dstart, FILE *stream,
				 Print_Data_Ptr printer, int32 next);
int32 x_ipc_TWOBYTE_Trans_DFree(GENERIC_DATA_PTR datastruct, int32 dstart);

/*****************************************************************
 * In C the unsigned versions of the formatters behave just like the 
 *   signed versions, except for printing.
 ****************************************************************/
int32 UINT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			FILE *stream, Print_Data_Ptr printer, int32 next);
int32 USHORT_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			  FILE *stream, Print_Data_Ptr printer, int32 next);
int32 ULONG_Trans_DPrint(CONST_GENERIC_DATA_PTR datastruct, int32 dstart,
			 FILE *stream, Print_Data_Ptr printer, int32 next);

#endif /* INCprimFmttrs */

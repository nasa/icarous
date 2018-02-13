/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: print data
 *
 * FILE: printData.h
 *
 * ABSTRACT:
 * 
 * Print Data Interface Routines - central server only.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: printData.h,v $
 * Revision 2.4  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:16  reids
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
 * Revision 2.1.1.1  1999/11/23 19:07:34  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.3  1997/01/27 20:09:54  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.1  1997/01/11 01:21:17  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.16.1  1996/12/27 19:26:08  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.2  1996/05/09 16:53:43  reids
 * Remove (conditionally) references to matrix format.
 *
 * Revision 1.1  1996/05/09 01:01:50  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.1  1996/03/03 04:32:14  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.11  1995/07/10  16:18:26  rich
 * Interm save.
 *
 * Revision 1.10  1995/05/31  19:36:12  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.9  1995/04/19  14:28:45  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.8  1995/04/05  19:11:12  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.7  1995/01/18  22:42:01  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.6  1994/05/17  23:17:08  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.5  1993/12/01  18:04:07  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:19:01  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:16:14  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:36  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:46  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:31:38  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:25:03  fedor
 * Added Logging.
 *
 * 28-Oct-90 Christopher Fedor, School of Computer Science, CMU
 * (slightly) revised to standards.
 *
 * 24-Jul-89 Reid Simmons, School of Computer Science, CMU
 * created.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCprintData
#define INCprintData

/*
 * Number of characters to keep on a line following the beginning of a
 * structured formatter. 10 was choosen arbitrarily. Might not always
 * work
 */
#define DEFAULT_KEEP 10   

#define LINE_LENGTH  80  /* Number of characters per line. */
#define PRINT_LENGTH  5  /* Maximum number of lines printed. */
#define DEC_PLACES    2  /* Number of decimal places for floats and doubles. */

void printString(FILE *stream, Print_Data_Ptr printer,
		 const char *pString, int32 keepWithNext);
void printInt(FILE *stream, Print_Data_Ptr printer, 
	      const int32 *intPtr, int32 keepWithNext);
void printBoolean(FILE *stream, Print_Data_Ptr printer,
		  const int32 *intPtr, int32 keepWithNext);
void printChar(FILE *stream, Print_Data_Ptr printer,
	       const char *charPtr, int32 keepWithNext);
void printShort(FILE *stream, Print_Data_Ptr printer,
		const int16 *shortPtr, int32 keepWithNext);
void printLong(FILE *stream, Print_Data_Ptr printer,
	       const long *longPtr, int32 keepWithNext);
void printFloat(FILE *stream, Print_Data_Ptr printer,
		const float *floatPtr, int32 keepWithNext);
void printDouble(FILE *stream, Print_Data_Ptr printer,
		 const double *doublePtr, int32 keepWithNext);
void printByte(FILE *stream, Print_Data_Ptr printer,
	       const char *bytePtr, int32 keepWithNext);
void printUByte(FILE *stream, Print_Data_Ptr printer,
		const char *bytePtr, int32 keepWithNext);
void printTwoByte(FILE *stream, Print_Data_Ptr printer,
		  const char *twoBytePtr, int32 keepWithNext);

void Print_Formatted_Data(FILE *stream, CONST_FORMAT_PTR format, 
			  const void *dataPtr);
void Print_Data(const char *formatString, const void *dataPtr);
void Print_Formatter(FILE *stream, CONST_FORMAT_PTR format);

void dPrintSTR(FILE *stream, Print_Data_Ptr printer,
	       const char *pString, int32 next);
void dPrintFORMAT(FILE *stream, Print_Data_Ptr printer,
		  CONST_FORMAT_PTR format, int32 next);
#ifndef NMP_IPC
void mapPrint(FILE *stream, const genericMatrix *map, CONST_FORMAT_PTR format,
	      int32 keepWithNext, const void *mapElements);
#endif
void dPrintX_IPC(FILE *stream, Print_Data_Ptr printer,
		 const X_IPC_REF_TYPE *ref, int32 next);

/*****************************************************************
 * In C, the unsigned versions of the formatters must print differently
 ****************************************************************/
void printUShort(FILE *stream, Print_Data_Ptr printer,
		 const unsigned short *shortPtr, int32 keepWithNext);
void printUInt(FILE *stream, Print_Data_Ptr printer,
	       const unsigned int *intPtr, int32 keepWithNext);
void printULong(FILE *stream, Print_Data_Ptr printer,
		const unsigned long *longPtr, int32 keepWithNext);

#endif /* INCprintData */

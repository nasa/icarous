/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: Printing Data.
 *
 * FILE: printData.c
 *
 * ABSTRACT:
 * Print Out Data According to Formatter Specification.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: printData.c,v $
 * Revision 2.6  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.5  2002/01/03 21:00:59  reids
 * Ooops -- mistyped a variable name.
 *
 * Revision 2.4  2002/01/03 20:52:16  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.3  2000/07/03 17:03:27  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.2  2000/02/25 14:07:26  reids
 * Use of UNUSED_PRAGMA for compilers that do not support "#pragma unused"
 *
 * Revision 2.1.1.1  1999/11/23 19:07:36  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.2.2.6  1997/01/27 20:09:52  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.2.2.4  1997/01/11 01:21:16  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.2.2.3.4.1  1996/12/27 19:26:07  reids
 * Added formatters for unsigned short, int and long.
 * Fixed the way Lisp is passed integer values of various sizes.
 *
 * Revision 1.2.2.3  1996/12/18 15:13:03  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.2.2.2  1996/10/18 18:16:01  reids
 * Better printing of formatter data structures.
 *
 * Revision 1.2.2.1  1996/10/14 00:23:03  reids
 * Better way to print out LengthFMT data.
 *
 * Revision 1.2  1996/05/09 16:53:42  reids
 * Remove (conditionally) references to matrix format.
 *
 * Revision 1.1  1996/05/09 01:01:49  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/03/19 03:38:50  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:56  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:13  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.28  1996/06/25  20:51:16  rich
 * Fixed memory and other problems found with purify.
 *
 * Revision 1.27  1996/05/09  18:31:30  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.26  1996/03/15  21:18:25  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.25  1996/03/05  05:04:47  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.24  1996/01/30  15:04:48  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.23  1996/01/27  21:53:57  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.22  1995/07/10  16:18:22  rich
 * Interm save.
 *
 * Revision 1.21  1995/07/06  21:16:55  rich
 * Solaris and Linux changes.
 *
 * Revision 1.20  1995/06/14  03:21:54  rich
 * Added DBMALLOC_DIR.
 * More support for DOS.  Fixed some problems with direct connections.
 *
 * Revision 1.19  1995/04/19  14:28:42  rich
 * Fixed problems with lisp encode/decode functions.
 * Added types int32 and int16 for use where the size of the integer matters.
 *
 * Revision 1.18  1995/04/05  19:11:09  rich
 * Adding Reid's Changes.
 * Consolidating the matrix code.
 *
 * Revision 1.17  1995/04/04  19:42:54  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.16  1995/03/28  01:14:52  rich
 * - Added ability to log data with direct connections.  Also fixed some
 * problems with global variables. It now uses broadcasts for watching variables.
 * - Added preliminary memory recovery routines to handle out of memory
 * conditions.  It currently purges items from resource queues.  Needs to
 * be tested.
 * - If the CENTRALHOST environment variable is not set, try the current
 * host.
 * - Fixed a problem with central registered messages that caused the parsed
 * formatters to be lost.
 * - Added const declarations where needed to the prototypes in x_ipc.h.
 * - x_ipcGetConnections: Get the fd_set.  Needed for direct connections.
 * - Added x_ipcExecute and x_ipcExecuteWithConstraints.  Can "execute" a goal
 *   or command.
 * - x_ipcPreloadMessage: Preload the definition of a message from the
 *   central server.
 *
 * Revision 1.15  1995/01/25  00:01:38  rich
 * Release of x_ipc 7.9.  Mostly speed improvements.
 * The cvs binaries may now be located in /usr/local.
 * Fixed problems with little endian translation.
 *
 * Revision 1.14  1995/01/18  22:41:57  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.13  1994/10/25  17:10:32  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.12  1994/05/17  23:17:06  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.11  1994/04/28  16:16:50  reids
 * Changes in X_IPC Version 7.6:
 *  1) New functions: x_ipcIgnoreLogging and x_ipcResumeLogging
 *  2) Code for MacIntosh (MPW) version of X_IPC
 *
 * Revision 1.10  1994/04/16  19:43:01  rich
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
 * Revision 1.9  1994/01/31  18:28:36  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.8  1993/12/14  17:34:35  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.7  1993/11/21  20:18:59  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.6  1993/08/30  21:54:10  fedor
 * V7+V6+VXWORKS Everything compiles but there are initialization problems.
 *
 * Revision 1.5  1993/08/27  07:16:12  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.4  1993/08/20  23:06:58  fedor
 * Minor changes for merge. Mostly added htons and removed cfree calls.
 *
 * Revision 1.3  1993/06/13  23:28:17  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.2  1993/05/26  23:18:33  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:30  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:31:35  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:25:01  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 13-Sep-91 Christopher Fedor, School of Computer Science, CMU
 * Made buffer a single global scratch area. Increased it to hold 400
 * characters which should hold big ugly numbers without sprintf death.
 * buffer as a global should only be used if the sprintf is followed by
 * the printString routine. There is no recursion at this point so this should
 * work. The termination character for the buffer is provided by sprintf -
 * or one could explicitly set buffer[30] = '\0'.
 * Made sure all fprintf routines had the correct number of arguments.
 * Additional changes to software standards.
 *
 *  2-May-90 Christopher Fedor, School of Computer Science, CMU
 * Added call to alignFied in Print_Structured_Data so that SUN4 version
 * of central could correctly display data. Note: should look into having
 * DPrint option of primFmttrs return correct number of bytes instead of
 * a dummy of 1 to be consistent with other _TRANS operations.
 * Perhaps I am just unsure why this was done.
 *
 * 28-Nov-89 Reid Simmons, School of Computer Science, CMU
 * Added code to abort print out if PRINT_LENGTH exceeded.
 *
 * 24-Jul-89 Reid Simmons, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.6 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"

/* Defined below; forward reference */
static void Print_Formatter1(FILE *stream, Print_Data_Ptr printer,
			     CONST_FORMAT_PTR format, int32 keepWithNext);

static void Print_Structured_Data (FILE *Stream, Print_Data_Ptr printer,
				   CONST_FORMAT_PTR Format,
				   CONST_GENERIC_DATA_PTR Data_Ptr, int32 DStart,
				   CONST_FORMAT_PTR parentFormat, 
				   int32 Keep_With_Next);

/******************************************************************************
 *
 * FUNCTION: void newLine(stream)
 *
 * DESCRIPTION:
 *
 * INPUTS: FILE *stream;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void newLine(FILE *stream, Print_Data_Ptr printer)
{
  (void)fprintf(stream, "\n");
  printer->lineNumGlobal++;
  printer->cursorPosGlobal = 0;
}


/******************************************************************************
 *
 * FUNCTION: void printTab(stream, tabPosition)
 *
 * DESCRIPTION: 
 * Print spaces from current cursorPosGlobal to Tab_Position.
 * Does nothing if position already passed.
 *
 * INPUTS: 
 * FILE *stream;
 * int tabPosition;
 *
 * OUTPUTS: void.
 *
 * NOTES: makes use of printf min field width.
 *
 *****************************************************************************/

static void printTab(FILE *stream, Print_Data_Ptr printer, int32 tabPosition)
{ 
  int32 spaces;
  
  if (!printer->truncatedGlobal) {
    if (printer->cursorPosGlobal < tabPosition) {
      spaces = tabPosition - printer->cursorPosGlobal;
      (void)fprintf(stream, "%*s", spaces, " ");
      printer->cursorPosGlobal += spaces;
    }
  }
}


/******************************************************************************
 *
 * FUNCTION: void printSpace(stream)
 *
 * DESCRIPTION:
 * Print out a space to the stream, unless at the end of a line,
 * in which case, go to the next line.
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void printSpace(FILE *stream, Print_Data_Ptr printer)
{
  if (!printer->truncatedGlobal) {
    if (printer->cursorPosGlobal == LINE_LENGTH) {
      newLine(stream, printer);
      printTab(stream, printer, GET_M_GLOBAL(indentGlobal));
    }
    
    (void)fprintf(stream, "%c", ' ');
    printer->cursorPosGlobal++;
  }
}


/******************************************************************************
 *
 * FUNCTION: void printString(stream, string, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the string to the stream.
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the string.
 *
 * INPUTS: 
 * FILE *stream;
 * char *string;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printString(FILE *stream, Print_Data_Ptr printer,
		 const char *pString, int32 keepWithNext)
{ 
  int32 length;
  
  if (!printer->truncatedGlobal) {
    if (pString)
      length = strlen(pString);
    else
      length = 0;
    
    if ((printer->cursorPosGlobal + length + keepWithNext) > LINE_LENGTH) 
      {
	newLine(stream, printer);
	printTab(stream, printer, GET_M_GLOBAL(indentGlobal)+1);
      }
    if (printer->lineNumGlobal < PRINT_LENGTH) {
      if (pString) {
	(void)fprintf(stream, "%s", pString);
	printer->cursorPosGlobal += length;
      }
      else {
	(void)fprintf(stream, "NULL");
	printer->cursorPosGlobal += 4;
      }
    }
    else {
      printer->truncatedGlobal = 1;
      (void)fprintf(stream, "...");
    }
  }
}


/******************************************************************************
 *
 * FUNCTION: void printInt(stream, intPtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the integer to the stream.
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the integer.
 *
 * INPUTS: 
 * FILE *stream;
 * int *intPtr;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printInt(FILE *stream, Print_Data_Ptr printer,
	      const int32 *intPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%d", *intPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printBoolean(stream, intPtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the integer to the stream.
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the integer.
 *
 * INPUTS: 
 * FILE *stream;
 * int *intPtr;
 * int keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printBoolean(FILE *stream, Print_Data_Ptr printer,
		  const int32 *booleanPtr, int32 keepWithNext)
{ 
  if (*booleanPtr)
    printString(stream, printer, "True", keepWithNext);
  else
    printString(stream, printer, "False", keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printChar(stream, charPtr, keepWithNext)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * char *charPtr;
 * int keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printChar(FILE *stream, Print_Data_Ptr printer,
	       const char *charPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%c", *charPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printShort(stream, shortPtr, keepWithNext)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * short *shortPtr;
 * int keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printShort(FILE *stream, Print_Data_Ptr printer,
		const int16 *shortPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%d", *shortPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printLong(stream, longPtr, keepWithNext)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * long *longPtr;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printLong(FILE *stream, Print_Data_Ptr printer,
	       const long *longPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%ld", *longPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printFloat(stream, floatPtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the floating_point number to the stream, with 
 * accuracy of "DEC_PLACES".
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the float.
 *
 * INPUTS: 
 * FILE *stream;
 * float *floatPtr;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printFloat(FILE *stream, Print_Data_Ptr printer,
		const float *floatPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%.*f", DEC_PLACES, *floatPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printDouble(stream, doublePtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the double floating_point number to the stream, with
 * accuracy of "DEC_PLACES".
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the double.
 *
 * INPUTS: 
 * FILE *stream;
 * double *doublePtr;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printDouble(FILE *stream, Print_Data_Ptr printer,
		 const double *doublePtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%.*f", DEC_PLACES, *doublePtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printHex(stream, intPtr, keepWithNext)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * unsigned *intPtr;
 * int KeepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void printHex(FILE *stream, Print_Data_Ptr printer,
		     const u_int32 *intPtr, int32 keepWithNext)
{
  (void)sprintf(printer->buffer, "%#x", *intPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printByte(stream, bytePtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the byte (in hex) to the stream.
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the byte.
 *
 * INPUTS: 
 * FILE *stream;
 * char *bytePtr;
 * int keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printByte(FILE *stream, Print_Data_Ptr printer,
	       const char *bytePtr, int32 keepWithNext)
{ 
  u_int32 byte =0;
  
  *(((char *)&byte)+sizeof(u_int32)-sizeof(char)) = *bytePtr;
  printHex(stream, printer, &byte, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printUByte(stream, ubytePtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the byte (in hex) to the stream.
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the byte.
 *
 * INPUTS: 
 * FILE *stream;
 * char *bytePtr;
 * int keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printUByte(FILE *stream, Print_Data_Ptr printer,
		const char *ubytePtr, int32 keepWithNext)
{ 
  u_int32 ubyte =0;
  
  *(((char *)&ubyte)+sizeof(u_int32)-sizeof(char)) = *ubytePtr;
  printHex(stream, printer, &ubyte, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printTwoByte(stream, twoBytePtr, keepWithNext)
 *
 * DESCRIPTION:
 * Print out the double byte (in hex) to the stream.
 * Move to next line if less than "keep_with_next" characters would
 * remain on the current line after printing the bytes.
 *
 * INPUTS: 
 * FILE *stream;
 * char *twoBytePtr;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void printTwoByte(FILE *stream, Print_Data_Ptr printer,
		  const char *twoBytePtr, int32 keepWithNext)
{ 
  unsigned twoByte=0;
  
  *(int16 *)(((char *)&twoByte)+sizeof(int32)-2*sizeof(char)) =
    *(int16 *) twoBytePtr;
  printHex(stream, printer, &twoByte, keepWithNext);
}


/******************************************************************************
 *
 * FUNCTION: void printCommaSpace(stream)
 *
 * DESCRIPTION:
 *
 * INPUTS: FILE *stream;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void printCommaSpace(FILE *stream, Print_Data_Ptr printer)
{
  printString(stream, printer, ",", 0);
  printSpace(stream, printer);
}


/******************************************************************************
 *
 * FUNCTION: void startPrint(stream)
 *
 * DESCRIPTION:
 *
 * INPUTS: FILE *stream;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void startPrint(FILE *stream, Print_Data_Ptr printer)
{
  printer->cursorPosGlobal = 0;
  printer->lineNumGlobal = 0;
  printer->truncatedGlobal = 0;
  printTab(stream, printer, GET_M_GLOBAL(indentGlobal));
}


/******************************************************************************
 *
 * FUNCTION: void endPrint(stream)
 *
 * DESCRIPTION:
 *
 * INPUTS: FILE *stream;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void endPrint(FILE *stream, Print_Data_Ptr printer)
{
  newLine(stream, printer);
  printer->truncatedGlobal = 0;
}


/******************************************************************************
 *
 * FUNCTION: int32 printArrayData(stream, format, dataArray, AStart, 
 *                  formatArray, dimension, keepWithNext, startToken, endToken)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * FORMAT_PTR format;
 * char *dataArray;
 * int32 AStart, dimension, keepWithNext;
 * FORMAT_ARRAY_PTR formatArray;
 * char *startToken, *endToken;
 *
 * OUTPUTS: int32
 *
 *****************************************************************************/

static int32 printArrayData(FILE *stream, Print_Data_Ptr printer,
			    CONST_FORMAT_PTR format, 
			    const void *dataArray,
			    int32 AStart, FORMAT_ARRAY_PTR formatArray,
			    int32 dimension, int32 keepWithNext,
			    char *startToken, char *endToken)
{ 
  int32 i, element_size=0, last_dimension_p, last_element_p;
  int32 vector_length, next_keep;
  
  printString(stream, printer, startToken, DEFAULT_KEEP);
  
  last_dimension_p = (dimension+1 == formatArray[0].i);
  if (last_dimension_p)
    element_size = x_ipc_dataStructureSize(format);
  
  vector_length = formatArray[dimension].i;
  
  for (i=0; i<vector_length; i++) {
    if (printer->truncatedGlobal) 
      return 0;
    else {
      last_element_p = (i+1 == vector_length);
      next_keep = ((last_element_p) ? keepWithNext+1 : 1);
      if (last_dimension_p) {
	Print_Structured_Data(stream, printer, format,
			      (CONST_GENERIC_DATA_PTR)dataArray,
			      AStart, (FORMAT_PTR)NULL, next_keep);
	AStart += element_size;
	if (!last_element_p)
	  printCommaSpace(stream, printer);
      }
      else {
	AStart = printArrayData(stream, printer, format, dataArray, AStart, 
				formatArray, dimension+1, next_keep, 
				startToken, endToken);
	if (!last_element_p)
	  printCommaSpace(stream, printer);
      }
    }
  }
  
  printString(stream, printer, endToken, keepWithNext);
  
  return AStart;
}


/******************************************************************************
 *
 * FUNCTION: FORMAT_ARRAY_PTR fix_format_array(varFormatArray, 
 *                           parentStructArray, dataPtr, DStart)
 *
 * DESCRIPTION:
 * Create a fixed format array that contains all the dimensions of
 * the variable length array.
 *
 * INPUTS: 
 * FORMAT_ARRAY_PTR varFormatArray, parentStructArray;
 * GENERIC_DATA_PTR dataPtr;
 * int32 DStart;
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

/* RTG: The method of finding the size of the variable array was not quite 
 * correct.  Because of the way padding can be added before the pointer
 * to the variable array, just backing up or going forward the minimum
 * number of bytes is not always valid.  This is more apparent when running
 * on an alpha because pointers and ints are different sizes.
 */

static FORMAT_ARRAY_PTR fix_format_array(FORMAT_ARRAY_PTR varFormatArray, 
					 CONST_FORMAT_PTR parentFormat,
					 CONST_GENERIC_DATA_PTR dataPtr,
					 int32 DStart)
{ 
  FORMAT_ARRAY_PTR fixed_format_array;
  FORMAT_ARRAY_PTR parentStructArray;
  int32 arraySize, foundPlace, currentPlace, i, j, sizePlace;
  int32 offset=0, size, sizeOffset=0, currentOffset=0;
  
  SIZES_TYPE sizes;
  
  parentStructArray = parentFormat->formatter.a;
  foundPlace = 0;
  for (currentPlace=1; !foundPlace; currentPlace++)
    foundPlace = ((parentStructArray[currentPlace].f->type == VarArrayFMT) &&
		  (parentStructArray[currentPlace].f->formatter.a
		   == varFormatArray));
  currentPlace--;
  
  arraySize = varFormatArray[0].i;
  fixed_format_array = (FORMAT_ARRAY_PTR)x_ipcMalloc((unsigned)arraySize * 
						   sizeof(FORMAT_ARRAY_TYPE));
  fixed_format_array[0].i = arraySize;
  fixed_format_array[1].f = varFormatArray[1].f;
  for (i=2; i<arraySize; i++) {
    sizePlace = varFormatArray[i].i;
    offset = 0;
    sizeOffset = 0;
    j=1;
    while((j <= sizePlace) || (j <= currentPlace)) {
      switch (parentStructArray[j].f->type) {
      case VarArrayFMT:
	sizes.data = sizeof(GENERIC_DATA_PTR);
	break;
      default:
	sizes = x_ipc_bufferSize1(parentStructArray[j].f,dataPtr,
			    sizeOffset,parentFormat);
	break;
      }
      offset += sizes.data;
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
    size = *((int32 *)(((char *)dataPtr)+DStart+offset));
    fixed_format_array[i].i = size;
  }
  
  return fixed_format_array;
}


/******************************************************************************
 *
 * FUNCTION:
 *
 * DESCRIPTION:
 * If there is no formatter, it means that the ptr is recursive (self-ptr)
 *
 * INPUTS: 
 *
 * OUTPUTS: 
 *
 *****************************************************************************/

static void Print_Structured_Data(FILE *Stream, Print_Data_Ptr printer,
				  CONST_FORMAT_PTR Format,
				  CONST_GENERIC_DATA_PTR Data_Ptr, 
				  int32 DStart,
				  CONST_FORMAT_PTR parentFormat, 
				  int32 Keep_With_Next)
{ 
  TRANSLATE_FN_DPRINT printProc;
  int32 i, last_element_p, next_keep, length, currentDStart;
  u_int32 byte;
  void *StructPtr, *ArrayPtr;
  FORMAT_ARRAY_PTR format_array, fixed_format_array;

  currentDStart = DStart;
  switch (Format->type) {
  case LengthFMT:
    length = Format->formatter.i;
    printString(Stream, printer, "(", DEFAULT_KEEP);
    for (i=0; i<length; i++) {
      if (printer->truncatedGlobal) {
	return;
      } else {
	byte = 0;
	*(((u_char *)&byte)+sizeof(u_int32)-sizeof(u_char)) =
	  *(u_char *)(Data_Ptr+currentDStart+i);
	printHex(Stream, printer, &byte,
		 (i == length-1 ? Keep_With_Next+1 : 1));
	if (i == length-1) printString(Stream, printer, ")", Keep_With_Next);
	else printCommaSpace(Stream, printer);
      }
    }
    break;
    
  case PrimitiveFMT: 
    printProc = GET_M_GLOBAL(TransTable)[Format->formatter.i].DPrint;
    (* printProc)(Data_Ptr, currentDStart, Stream, printer, Keep_With_Next);
    break;
    
  case PointerFMT:
    StructPtr = REF(GENERIC_DATA_PTR, Data_Ptr, currentDStart);
    if (StructPtr) { 
      printString(Stream, printer, "*", DEFAULT_KEEP);
      Print_Structured_Data(Stream, printer,
			    CHOOSE_PTR_FORMAT(Format, parentFormat),
			    (CONST_GENERIC_DATA_PTR)StructPtr, 0,
			    (CONST_FORMAT_PTR)NULL, Keep_With_Next);
    }
    else
      printString(Stream, printer, "NULL", Keep_With_Next);
    break;
    
  case StructFMT:
    printString(Stream, printer, "{", DEFAULT_KEEP);
    format_array = Format->formatter.a;
    for (i=1;i<format_array[0].i;i++) {
      if (printer->truncatedGlobal)
	return;
      else {
	last_element_p = (i+1 == format_array[0].i);
	next_keep = ((last_element_p) ? Keep_With_Next+1 : 1);
	Print_Structured_Data(Stream, printer, format_array[i].f, 
			      Data_Ptr+DStart, currentDStart-DStart, 
			      Format, next_keep);
	currentDStart += x_ipc_dataStructureSize(format_array[i].f);
	currentDStart = x_ipc_alignField(Format, i, currentDStart);
	if (last_element_p)
	  printString(Stream, printer, "}", Keep_With_Next);
	else
	  printCommaSpace(Stream, printer);
      }
    }
    break;
    
  case FixedArrayFMT:
    (void)printArrayData(Stream, printer, Format->formatter.a[1].f,
			 Data_Ptr, currentDStart, 
			 Format->formatter.a, 2, Keep_With_Next, "[", "]");
    break;
    
  case VarArrayFMT:
    ArrayPtr = REF(GENERIC_DATA_PTR, Data_Ptr, currentDStart);
    format_array = Format->formatter.a;
    if (ArrayPtr) {
      fixed_format_array = fix_format_array(format_array, 
					    parentFormat,
					    Data_Ptr, currentDStart);
      (void)printArrayData(Stream, printer, format_array[1].f, ArrayPtr, 0, 
			   fixed_format_array, 2, Keep_With_Next, "<", ">");
      x_ipcFree((char *)fixed_format_array);
    }
    else
      printString(Stream, printer, "NULL", Keep_With_Next);
    break;
  case NamedFMT:
    Print_Structured_Data(Stream, printer,
			  x_ipc_fmtFind(Format->formatter.name),
			  Data_Ptr, currentDStart,
			  parentFormat, Keep_With_Next);
    break;
  case BadFormatFMT:
    printString(Stream, printer, "Bad Format", Keep_With_Next);
  case EnumFMT:
    { int32 eVal;
      eVal = x_ipc_enumToInt(Format, Data_Ptr, &currentDStart);
      if (Format->formatter.a[0].i > 2 &&
	  /* enum value within range */
	  0 <= eVal && eVal <= ENUM_MAX_VAL(Format)) {
	printString(Stream, printer,
		    Format->formatter.a[eVal+2].f->formatter.name,
		    Keep_With_Next);
      } else {
	printInt(Stream, printer, &eVal, Keep_With_Next);
      }
      break;
    }

#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Unknown Print_Structured_Data Type %d", Format->type);
    break;
#endif
  }
}


/******************************************************************************
 *
 * FUNCTION: void Print_Formatted_Data(stream, format, dataPtr)
 *
 * DESCRIPTION:
 * Print out some data (linearally) according to the given format.
 * "Data_Ptr" is a pointer to some arbitrary data structure.
 * "Format" is the format of the data
 * "Stream" is where to print the data (e.g. stdout)
 *
 * INPUTS: 
 * FILE *stream;
 * FORMAT_PTR format;
 * char **dataPtr;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void Print_Formatted_Data(FILE *stream, CONST_FORMAT_PTR format, 
			  const void *dataPtr)
{
  Print_Data_Type printer;

  LOCK_CM_MUTEX;
  startPrint(stream, &printer);
  Print_Structured_Data(stream, &printer, format,
			(CONST_GENERIC_DATA_PTR)dataPtr, 0,
			(FORMAT_PTR)NULL, 0);
  endPrint(stream, &printer);
  UNLOCK_CM_MUTEX;
}


/******************************************************************************
 *
 * FUNCTION: void Print_Data(formatString, dataPtr)
 *
 * DESCRIPTION:
 * Print the data (pointed to be "Data_Ptr") according to the
 * "Format_String" to the standard output (stdout).
 *
 * INPUTS: 
 * char *formatString;
 * char **dataPtr;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void Print_Data(const char *formatString, const void *dataPtr)
{
  Print_Formatted_Data(stdout, ParseFormatString(formatString), dataPtr);
}


/******************************************************************************
 *
 * FUNCTION: void PrintArrayFormat(stream, startToken, formatArray, 
 *                                 endToken, keepWithNext)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * char *startToken, *endToken;
 * FORMAT_ARRAY_PTR formatArray;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void PrintArrayFormat(FILE *stream, Print_Data_Ptr printer,
			     char *startToken, FORMAT_ARRAY_PTR formatArray,
			     char *endToken, int32 keepWithNext)
{ 
  int32 i, lastElementP;
  
  printString(stream, printer, startToken, DEFAULT_KEEP);
  Print_Formatter1(stream, printer, formatArray[1].f, keepWithNext);
  printString(stream, printer, " : ", DEFAULT_KEEP);
  for (i=2;i<formatArray[0].i;i++) {
    if (printer->truncatedGlobal)
      return;
    else {
      lastElementP = (i+1 == formatArray[0].i);
      printInt(stream, printer, &(formatArray[i].i),
	       ((lastElementP) ? keepWithNext+1 : 1));
      if (lastElementP)
	printString(stream, printer, endToken, keepWithNext);
      else
	printCommaSpace(stream, printer);
    }
  }
}


/******************************************************************************
 *
 * FUNCTION: void Print_Formatter1(stream, format, keepWithNext)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * FORMAT_PTR format;
 * int32 keepWithNext;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

static void Print_Formatter1(FILE *stream, Print_Data_Ptr printer,
			     CONST_FORMAT_PTR format, int32 keepWithNext)
{ 
  int32 i, last_element_p;
  FORMAT_ARRAY_PTR format_array;
  const char *primFormatName;
  
  switch(format->type) {
  case LengthFMT: 
    printInt(stream, printer, &(format->formatter.i), keepWithNext);
    break;
  case PrimitiveFMT: 
    switch(format->formatter.p) {
    case  INT_FMT: primFormatName = INT_FMT_NAME; break;
    case  BOOLEAN_FMT: primFormatName = BOOLEAN_FMT_NAME; break;
    case  FLOAT_FMT: primFormatName = FLOAT_FMT_NAME; break;
    case  DOUBLE_FMT: primFormatName = DOUBLE_FMT_NAME; break;
    case  BYTE_FMT: primFormatName = BYTE_FMT_NAME; break;
    case  TWOBYTE_FMT: primFormatName = TWOBYTE_FMT_NAME; break;
    case  STR_FMT: primFormatName = STR_FMT_NAME; break;
    case  FORMAT_FMT: primFormatName = FORMAT_FMT_NAME; break;
    case  UBYTE_FMT: primFormatName = UBYTE_FMT_NAME; break;
    case  CMAT_FMT: primFormatName = CMAT_FMT_NAME; break;
    case  SMAT_FMT: primFormatName = SMAT_FMT_NAME; break;
    case  IMAT_FMT: primFormatName = IMAT_FMT_NAME; break;
    case  LMAT_FMT: primFormatName = LMAT_FMT_NAME; break;
    case  FMAT_FMT: primFormatName = FMAT_FMT_NAME; break;
    case  DMAT_FMT: primFormatName = DMAT_FMT_NAME; break;
    case  CHAR_FMT: primFormatName = CHAR_FMT_NAME; break;
    case  SHORT_FMT: primFormatName = SHORT_FMT_NAME; break;
    case  LONG_FMT: primFormatName = LONG_FMT_NAME; break;
    case  UCMAT_FMT: primFormatName = UCMAT_FMT_NAME; break;
    case  X_IPC_REF_PTR_FMT: primFormatName = X_IPC_REF_PTR_FMT_NAME; break;
    case  SIUCMAT_FMT: primFormatName = SIUCMAT_FMT_NAME; break;
    case  SICMAT_FMT: primFormatName = SICMAT_FMT_NAME; break;
    case  SISMAT_FMT: primFormatName = SISMAT_FMT_NAME; break;
    case  SIIMAT_FMT: primFormatName = SIIMAT_FMT_NAME; break;
    case  SILMAT_FMT: primFormatName = SILMAT_FMT_NAME; break;
    case  SIFMAT_FMT: primFormatName = SIFMAT_FMT_NAME; break;
    case  SIDMAT_FMT: primFormatName = SIDMAT_FMT_NAME; break;

    case  USHORT_FMT: primFormatName = USHORT_FMT_NAME; break;
    case  UINT_FMT:   primFormatName = UINT_FMT_NAME; break;
    case  ULONG_FMT:  primFormatName = ULONG_FMT_NAME; break;

#ifndef TEST_CASE_COVERAGE
    default: primFormatName = "UNKNOWN"; break;
#endif
    }
    printString(stream, printer, primFormatName, keepWithNext);
    break;
    
  case PointerFMT:
    printString(stream, printer, "*", DEFAULT_KEEP);
    if (format->formatter.f)
      Print_Formatter1(stream, printer, format->formatter.f, keepWithNext);
    else
      printString(stream, printer, "!", keepWithNext);
    break;
    
  case StructFMT:
    format_array = format->formatter.a;
    printString(stream, printer, "{", DEFAULT_KEEP);
    for (i=1; i<format_array[0].i; i++) {
      if (printer->truncatedGlobal)
	return;
      else {
	last_element_p = (i+1 == format_array[0].i);
	Print_Formatter1(stream, printer, format_array[i].f,
			 ((last_element_p) ? keepWithNext+1 : 1));
	if (last_element_p)
	  printString(stream, printer, "}", keepWithNext);
	else
	  printCommaSpace(stream, printer);
      }
    }
    break;
    
  case FixedArrayFMT: 
    PrintArrayFormat(stream, printer, "[", format->formatter.a, "]",
		     keepWithNext);
    break;
    
  case VarArrayFMT: 
    PrintArrayFormat(stream, printer, "<", format->formatter.a, ">",
		     keepWithNext);
    break;
  case NamedFMT:
    printString(stream, printer, format->formatter.name, keepWithNext);
    break;
  case BadFormatFMT:
    printString(stream, printer, "Bad Format", keepWithNext);
    break;
  case EnumFMT:
    format_array = format->formatter.a;
    printString(stream, printer, "{enum ", DEFAULT_KEEP);
    if (format_array[0].i == 2) {
      printString(stream, printer, ": ", keepWithNext);
      printInt(stream, printer, &(format_array[1].i), keepWithNext);
    } else {
      for (i=2; i<format_array[0].i; i++) {
	if (printer->truncatedGlobal)
	  return;
	else {
	  last_element_p = (i+1 == format_array[0].i);
	  printString(stream, printer, format_array[i].f->formatter.name,
		      ((last_element_p) ? keepWithNext+1 : 1));
	  if (!last_element_p) printCommaSpace(stream, printer);
	}
      }
    }
    printString(stream, printer, "}", keepWithNext);
    break;
    
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_ERROR1("Unknown Print_Formatter1 Type %d", format->type);
    break;
#endif
  }
}


/******************************************************************************
 *
 * FUNCTION: void Print_Formatter(stream, format)
 *
 * DESCRIPTION: Print a representation of the "Format" on the "Stream".
 *
 * INPUTS: 
 * FILE *stream;
 * FORMAT_PTR format;
 *
 * OUTPUTS: void.
 *
 *****************************************************************************/

void Print_Formatter(FILE *stream, CONST_FORMAT_PTR format)
{
  Print_Data_Type printer;

  LOCK_CM_MUTEX;
  startPrint(stream, &printer);
  if (format) 
    Print_Formatter1(stream, &printer, format, 0);
  else
    printString(stream, &printer, "NULL Format", 0);
  endPrint(stream, &printer);
  UNLOCK_CM_MUTEX;
}

/************************************/

void dPrintSTR(FILE *stream, Print_Data_Ptr printer, 
	       const char *pString, int32 next)
{
  printString(stream, printer, "\"", next);
  printString(stream, printer, pString, next);
  printString(stream, printer, "\"", next);
}

void dPrintFORMAT(FILE *stream, Print_Data_Ptr printer,
		  CONST_FORMAT_PTR format, int32 next)
{
  if (format)
    Print_Formatter1(stream, printer, format, next);
  else
    printString(stream, printer, "NULLFormat", next);
}

#ifndef NMP_IPC
/******************************************************************************
 *
 * FUNCTION: void mapPrint(stream, map, format, keepWithNext, mapElements)
 *
 * DESCRIPTION:
 *
 * INPUTS: 
 * FILE *stream;
 * fmat *map;
 * FORMAT_PTR format;
 * int32 keepWithNext;
 * char *mapElements;
 *
 * OUTPUTS: void
 *
 *****************************************************************************/

void mapPrint(FILE *stream, const genericMatrix *map, CONST_FORMAT_PTR format,
	      int32 keepWithNext, const void *mapElements)
{ 
  FORMAT_ARRAY_TYPE mapFormatArray[4];
  
  mapFormatArray[0].i = 4;
  mapFormatArray[1].f = (FORMAT_PTR)format;
  mapFormatArray[2].i = map->ub1 - map->lb1 + 1;
  mapFormatArray[3].i = map->ub2 - map->lb2 + 1;
  
  /* Print header: "[Map (lb1, ub1)(lb2, ub2): " */
  printString(stream, printer, "[Map (", 5);
  printInt(stream, &(map->lb1), 1);
  printCommaSpace(stream);
  printInt(stream, &(map->ub1), 1);
  printString(stream, printer, ")", 0);
  printString(stream, printer, "(", 5);
  printInt(stream, &(map->lb2), 1);
  printCommaSpace(stream);
  printInt(stream, &(map->ub2), 2);
  printString(stream, printer, "):", 0);
  printSpace(stream);
  
  (void)printArrayData(stream, mapFormatArray[1].f, mapElements,
		       /* &(map->el[map->lb1][map->lb2]),*/
		       0, mapFormatArray, 2, keepWithNext+1, "[", "]");
}
#endif

void dPrintX_IPC(FILE *stream, Print_Data_Ptr printer,
		 const X_IPC_REF_TYPE *ref, int32 next)
{
#ifdef UNUSED_PRAGMA
#pragma unused(next)
#endif
  printString(stream, printer, "[X_IPC_REF (", 5);
  printInt(stream, printer, &(ref->refId), 1);
  printString(stream, printer, ", ", 3);
  printString(stream, printer, ref->name, 10);
  printString(stream, printer, ")]", 5);
}

/*****************************************************************
 * In C, the unsigned versions of the formatters must print differently
 ****************************************************************/
void printUShort(FILE *stream, Print_Data_Ptr printer,
		 const unsigned short *shortPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%u", *shortPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}

void printUInt(FILE *stream, Print_Data_Ptr printer,
	       const unsigned int *intPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%u", *intPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}

void printULong(FILE *stream, Print_Data_Ptr printer,
		const unsigned long *longPtr, int32 keepWithNext)
{ 
  (void)sprintf(printer->buffer, "%lu", *longPtr);
  printString(stream, printer, printer->buffer, keepWithNext);
}

#ifdef DEBUG
/* Prototype, to keep compiler happy */
void Print_Format(CONST_FORMAT_PTR format);
 
void Print_Format(CONST_FORMAT_PTR format)
{
  Print_Formatter(stderr, format);
}
#endif

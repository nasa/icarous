/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: lex
 *
 * FILE: lex.h
 *
 * ABSTRACT:
 * Lexer include
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: lex.h,v $
 * Revision 2.4  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.3  2002/01/03 20:52:13  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.2  2000/07/03 17:03:25  hersh
 * Removed all instances of "tca" in symbols and messages, plus changed
 * the names of all other symbols which conflicted with TCA.  This new
 * version of IPC should be able to interoperate TCA fully.  Client
 * programs can now link to both tca and ipc.
 *
 * Revision 2.1.1.1  1999/11/23 19:07:33  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1  1996/05/09 01:01:34  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/03/19 03:38:43  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:48  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:44  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.13  1996/03/15  21:18:10  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.12  1996/01/30  15:04:24  rich
 * Fixed var array index problem.  Index refers to the enclosing structure.
 * Added ability to force 32 bit enums and changed some #defs to enums to
 * ease debugging.  Fixed initialization problems for central.
 *
 * Revision 1.11  1996/01/27  21:53:34  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.10  1996/01/05  16:31:27  rich
 * Added windows NT port.
 *
 * Revision 1.9  1995/05/31  19:35:46  rich
 * Fixed problem with reply data being freed early from replys.
 * Initial work on getting the PC version to work.
 *
 * Revision 1.8  1995/01/18  22:40:58  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.7  1994/05/17  23:16:13  rich
 * Added global variables and associated routines.
 * Added some error checking.  The central connection is now set to -1
 * rather than zero to prevent x_ipc messages from being send to stdout.
 * Now compiles on the sgi machines.  Still need to have the endian and
 * alignment figured out automatically.
 *
 * Revision 1.6  1993/12/14  17:33:56  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.5  1993/12/01  18:03:47  rich
 * Fixed a problem with the port number being double converted to network
 * byte order.
 * Some general cleanup.
 *
 * Revision 1.4  1993/11/21  20:18:07  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.3  1993/08/27  07:15:20  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:17:51  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:44  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:21  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:17  fedor
 * Added Logging.
 *
 * 10-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.4 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INClex
#define INClex

typedef enum {
  LBRACE_TOK=0, RBRACE_TOK=1, COMMA_TOK=2, LT_TOK=3, GT_TOK=4,
  STAR_TOK=5, BANG_TOK=6, COLON_TOK=7, LBRACK_TOK=8, 
  RBRACK_TOK=9, INT_TOK=10, STR_TOK=11, PRIMITIVE_TOK=12,
  EOS_TOK=13
#ifdef FORCE_32BIT_ENUM
    , dummyTok = 0x7FFFFFFF
#endif
} TokType;

typedef struct _Tok{
  int32 Loc;
  TokType Type;
  union {
    char *str;
    int32 num;
    /* INT_FN FormatFunc; */
  } value;
  struct _Tok *next;
} Token_Type, *TokenPtr;

/* Variables needed for parsing formats */
typedef struct {
  int32       Location;
  TokenPtr    TokenList;
  TokenPtr    TokenListHead;
  const char *ParseString;
  STR_LIST_PTR formatStack;
} Format_Parse_Type, *Format_Parse_Ptr;

#define NEW_TOKEN(Parser, Token, Typ) Token = (NEW(Token_Type));\
Token->Type = Typ;\
Token->Loc = Parser->Location;\
Token->next = NULL

void LexString(Format_Parse_Ptr parser, const char *s);
TokenPtr NextToken(Format_Parse_Ptr parser);
void UngetToken(Format_Parse_Ptr parser, TokenPtr token);

#endif /* INClex */

/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: lex
 *
 * FILE: lex.c
 *
 * ABSTRACT:
 * 
 * primitve lexer
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: lex.c,v $
 * Revision 2.5  2009/01/12 15:54:56  reids
 * Added BSD Open Source license info
 *
 * Revision 2.4  2003/02/13 20:41:10  reids
 * Fixed compiler warnings.
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
 * Revision 2.1.1.1  1999/11/23 19:07:35  reids
 * Putting IPC Version 2.9.0 under local (CMU) CVS control.
 *
 * Revision 1.1.2.1  1996/12/18 15:12:52  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1  1996/05/09 01:01:33  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.3  1996/03/19 03:38:43  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.2  1996/03/12 03:19:47  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:31:43  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.12  1996/03/15  21:18:08  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.11  1996/03/05  05:04:33  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.10  1996/01/27  21:53:32  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.9  1995/01/18  22:40:55  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.8  1994/04/16  19:42:24  rich
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
 * Revision 1.7  1993/12/14  17:33:54  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.6  1993/11/21  20:18:05  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.5  1993/10/21  16:13:58  rich
 * Fixed compiler warnings.
 *
 * Revision 1.4  1993/08/27  07:15:18  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.3  1993/06/13  23:28:11  rich
 * Made changes for lisp needed for vx works style global variables.
 * Fixed some random compiler warnings.
 * Moved test routines to test directory.
 *
 * Revision 1.2  1993/05/26  23:17:48  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:24  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:30:19  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:15  fedor
 * Added Logging.
 *
 * 13-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Changed to lex the whole string at once.
 *
 * 10-Feb-89 Christopher Fedor, School of Computer Science, CMU
 * Created.
 *
 * NOTES: 
 * 17-Jun-91: fedor:
 * Needs to be rewritten to take advantage of abstract data type routines
 * and to start looking at memory use. Blah this is a mess!
 *
 * $Revision: 2.5 $
 * $Date: 2009/01/12 15:54:56 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"
#include "globalM.h"

#include "lex.h"
#include "tcaMem.h"

static TokenPtr ReadString(Format_Parse_Ptr parser, const char *s)
{
  const char *t2;
  TokenPtr tmp;
  int i, amount;
  
  for (i=parser->Location;
       (s[i] != ',' && s[i] != '}' && s[i] != '{' && s[i] != '[' 
	&& s[i] != ']' && s[i] != ':' && s[i] != ' ' && s[i] != '>'
	&& s[i] != '<' && s[i] != '\0');
       i++) {};
  
  amount = i-parser->Location;
  NEW_TOKEN(parser, tmp, STR_TOK);
  tmp->value.str = (char *)x_ipcMalloc(sizeof(char)*(unsigned)(amount+1));
  t2 = s+parser->Location;
  strncpy(tmp->value.str, t2, amount);
  tmp->value.str[amount] = '\0';
  
  parser->Location += amount;
  
  return tmp;
}

static TokenPtr ReadInt(Format_Parse_Ptr parser, const char *s)
{
  char *t;
  const char *t2;
  TokenPtr tmp;
  int i, amount;
#if defined(VXWORKS)
  int num;
#endif
  
  for (i=parser->Location;
       (isdigit((int)s[i]) && s[i] != ',' && s[i] != '}' && s[i] != '{' 
	&& s[i] != '[' && s[i] != ']' && s[i] != ':' && s[i] != ' ' 
	&& s[i] != '>' && s[i] != '<' && s[i] != '\0');
       i++) {};
  
  amount = i-parser->Location;
  NEW_TOKEN(parser, tmp, INT_TOK);
  t = (char *)x_ipcMalloc(sizeof(char)*(unsigned)(amount+1));
  t2 = s+parser->Location;
  strncpy(t, t2, amount);
  t[amount] = '\0';
  
#ifndef VXWORKS
  tmp->value.num = atoi(t);
#else
  sscanf(t, "%d", &num);
  tmp->value.num = num;
#endif
  
  x_ipcFree(t);
  
  parser->Location += amount;
  
  return tmp;
}    

static TokenPtr BuildToken(Format_Parse_Ptr parser, const char *s)
{
  TokenPtr Token;
  
  if (!s) {
    NEW_TOKEN(parser, Token, EOS_TOK);
    return Token;
  }
  
 Start:
  switch(s[parser->Location]) {
  case ' ':
  case '\t':
  case '\f':
    parser->Location++;
    goto Start;
  case '{':
    NEW_TOKEN(parser, Token, LBRACE_TOK);
    parser->Location++;
    break;
  case '}':
    NEW_TOKEN(parser, Token, RBRACE_TOK);
    parser->Location++;
    break;
  case '[':
    NEW_TOKEN(parser, Token, LBRACK_TOK);
    parser->Location++;
    break;
  case ']':
    NEW_TOKEN(parser, Token, RBRACK_TOK);
    parser->Location++;
    break;
  case '*':
    NEW_TOKEN(parser, Token, STAR_TOK);
    parser->Location++;
    break;
  case '!':
    NEW_TOKEN(parser, Token, BANG_TOK);
    parser->Location++;
    break;
  case ',':
    NEW_TOKEN(parser, Token, COMMA_TOK);
    parser->Location++;
    break;
  case '<':
    NEW_TOKEN(parser, Token, LT_TOK);
    parser->Location++;
    break;
  case '>':
    NEW_TOKEN(parser, Token, GT_TOK);
    parser->Location++;
    break;
  case ':':
    NEW_TOKEN(parser, Token, COLON_TOK);
    parser->Location++;
    break;
  case '\0':
    NEW_TOKEN(parser, Token, EOS_TOK);
    parser->Location++;
    break;
  default:
    if (isdigit((int)s[parser->Location]))
      Token = ReadInt(parser, s);
    else 
      Token = ReadString(parser, s);
  }
  
  return Token;
}

#if 0
/* No longer used */
static void PrintToken(TokenPtr Token)
{
  X_IPC_MOD_WARNING1( "Loc: %d: Type: ", Token->Loc);
  
  switch(Token->Type) {
  case LBRACE_TOK:
    X_IPC_MOD_WARNING( "LBRACE_TOK\n");
    break;
  case RBRACE_TOK:
    X_IPC_MOD_WARNING( "RBRACE_TOK\n");
    break;
  case LBRACK_TOK:
    X_IPC_MOD_WARNING( "LBRACK_TOK\n");
    break;
  case RBRACK_TOK:
    X_IPC_MOD_WARNING( "RBRACK_TOK\n");
    break;
  case STAR_TOK:
    X_IPC_MOD_WARNING( "STAR_TOK\n");
    break;
  case BANG_TOK:
    X_IPC_MOD_WARNING( "BANG_TOK\n");
    break;
  case COMMA_TOK:
    X_IPC_MOD_WARNING( "COMMA_TOK\n");
    break;
  case LT_TOK:
    X_IPC_MOD_WARNING( "LT_TOK\n");
    break;
  case GT_TOK:
    X_IPC_MOD_WARNING( "GT_TOK\n");
    break;
  case COLON_TOK:
    X_IPC_MOD_WARNING( "COLON_TOK\n");
    break;
  case EOS_TOK:
    X_IPC_MOD_WARNING( "EOS_TOK\n");
    break;
  case INT_TOK:
    X_IPC_MOD_WARNING( "INT_TOK: %d\n", Token->value.num);
    break;
  case STR_TOK:
    X_IPC_MOD_WARNING( "STR_TOK: %s\n", Token->value.str);
    break;
  case PRIMITIVE_TOK:
    X_IPC_MOD_WARNING( "PRIMITIVE_TOK: %s\n", Token->value.str);
    break;
#ifndef TEST_CASE_COVERAGE
  default:
    X_IPC_MOD_WARNING( "Token of Unknown type: %d\n", Token->Type);
    break;
#endif
  }
}
#endif

#if 0
/* No longer used */
static void PrintTokenList(TokenPtr Token)
{
  TokenPtr tmp;
  
  tmp = Token;
  while (tmp) {
    PrintToken(tmp);
    tmp = tmp->next;
  }
}
#endif

void LexString(Format_Parse_Ptr parser, const char *s)
{
  TokenPtr Token;
  
  parser->Location = 0;
  
  parser->TokenList = BuildToken(parser, s);
  
  Token = parser->TokenList;
  while (Token->Type != EOS_TOK) {
    Token->next = BuildToken(parser, s);
    Token = Token->next;
  }
}

TokenPtr NextToken(Format_Parse_Ptr parser)
{
  TokenPtr tmp;
  
  tmp = parser->TokenList;
  parser->TokenList = parser->TokenList->next;
  
  return tmp;
}

void UngetToken(Format_Parse_Ptr parser, TokenPtr token)
{
  token->next = parser->TokenList;
  parser->TokenList = token;
}

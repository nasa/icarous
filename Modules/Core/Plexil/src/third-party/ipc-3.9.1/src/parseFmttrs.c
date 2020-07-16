/******************************************************************************
 *
 * PROJECT: Carnegie Mellon Planetary Rover Project
 *          Task Control Architecture 
 * 
 * (c) Copyright 1991 Christopher Fedor and Reid Simmons.  All rights reserved.
 * 
 * MODULE: Formatters
 *
 * FILE: parseFmttrs.c
 *
 * ABSTRACT:
 * Parser and Printer for Formatter Data Structures
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: parseFmttrs.c,v $
 * Revision 2.7  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.6  2002/01/03 20:52:15  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 * Revision 2.5  2001/02/28 03:14:58  trey
 * modified formatter parser so that the format "{}" is valid.  the empty struct format can be useful when automatically generating composite formats
 *
 * Revision 2.4  2000/12/11 16:10:39  reids
 * Added parsing rule to allow compatibility with RTC fixed array formats.
 *
 * Revision 2.3  2000/07/27 16:59:11  reids
 * Added function IPC_setMsgQueueLength.
 * Made IPC_setMsgQueueLength and IPC_setMsgPriority work with point-to-point
 *   messages (used to work only with centrally-routed messages).
 * Made direct connections a bit more robust.
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
 * Revision 1.1.2.7  1997/01/27 20:39:37  reids
 * For Lisp, force all format enum values to uppercase; Yields more efficient
 *   C <=> Lisp conversion of enumerated types.
 *
 * Revision 1.1.2.6  1997/01/27 20:09:47  udo
 * ipc_2_6_006 to r3_Dev merge
 *
 * Revision 1.1.2.4  1997/01/11 01:21:10  udo
 * ipc 2.6.002 to r3_dev merge
 *
 * Revision 1.1.2.3.4.1  1996/12/27 18:48:33  reids
 * Check if connected to network when looking for a named formatter.
 *
 * Revision 1.1.2.3  1996/12/18 15:13:00  reids
 * Changed logging code to remove VxWorks dependence on varargs
 *
 * Revision 1.1.2.2  1996/10/18 18:15:10  reids
 * Cache format attribute values (much more efficient).
 *
 * Revision 1.1.2.1  1996/10/02 20:56:44  reids
 * Fixed the procedure for dealing with named formatters.
 *
 * Revision 1.2  1996/09/06 22:30:36  pgluck
 * Removed static declarations for VxWorks
 *
 * Revision 1.1  1996/05/09 01:01:46  reids
 * Moved all the X_IPC files over to the IPC directory.
 * Fixed problem with sending NULL data.
 * Added function IPC_setCapacity.
 * VxWorks m68k version released.
 *
 * Revision 1.4  1996/03/19 03:38:46  reids
 * Plugging more memory leaks; Able to free formatter data structures.
 *
 * Revision 1.3  1996/03/12 05:23:16  reids
 * Ooops.  One-off in calculating the maximum value for enum format.
 *
 * Revision 1.2  1996/03/12 03:19:53  reids
 * Added "enum" format type.
 * Plugged memory leaks (using Purify).
 *
 * Revision 1.1  1996/03/03 04:32:07  reids
 * First release of IPC files.  X_IPC code (8.5), modified to support NM-DS1 IPC.
 *
 * Revision 1.18  1996/05/09  18:31:14  reids
 * Changes to keep X_IPC consistent with the NASA IPC package.
 * Some bug fixes (mainly to do with freeing formatters).
 *
 * Revision 1.17  1996/03/15  21:18:11  reids
 * Added support for "enum" format type.
 *   Also, printData was not counting characters per line correctly.
 *
 * Revision 1.16  1996/03/05  05:04:35  reids
 * Changes (mainly delineated by NMP_IPC conditionals) to support the
 *   New Millennium IPC.
 *
 * Revision 1.15  1996/03/02  03:21:50  rich
 * Fixed memory leaks found using purify.
 *
 * Revision 1.14  1996/01/27  21:53:40  rich
 * Pre-release of 8.4.
 * Added recursive named formatters and "BAD" formats.  Also incorporated
 * Iain's windows changes.
 *
 * Revision 1.13  1995/07/10  16:18:06  rich
 * Interm save.
 *
 * Revision 1.12  1995/04/04  19:42:42  rich
 * Added sgi support.
 * Split low level com routines out to be used in devUtils.
 * Improved some error messages.
 * Added central switch to default to direct connections.  Does not work yet.
 * Fixed the vectorization code.
 *
 * Revision 1.11  1995/01/18  22:41:38  rich
 * X_IPC 7.9: Speed improvements.
 * Use unix sockets for communication on the same machine.
 * Eliminate copying.
 * Optimize loop for arrays, especially simple, primitive arrays.
 * Optimize the buffer size.
 *
 * Revision 1.10  1994/10/25  17:10:18  reids
 * Changed the logging functions to accept variable number of arguments.
 *
 * Revision 1.9  1994/05/05  00:46:28  rich
 * Added a gmake makefile GNUmakefile so that the system can be easily
 * compiled on different machines.
 * Can now create the targets: tarfile and ftp for creating versions for
 * export.
 *
 * Fixed a number of places were x_ipcExitHnd was not expected to return.
 * Set the x_ipcSeverGlobal to 0 when the socket is closed.
 *
 * Revision 1.8  1994/04/16  19:42:52  rich
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
 * Revision 1.7  1994/01/31  18:28:34  reids
 * Several major changes (and some minor ones)
 * 1. x_ipcFreeData and x_ipcFreeReply now work even if the data or message format
 *    is NULL
 * 2. Using the "-t" option in central, message taps are added as a child of
 *    the task tree node that was tapped.
 * 3. Named formatters are now expanded only when needed
 * For more details, see ../doc/x_ipc-7-4.release.notes
 *
 * Revision 1.6  1993/12/14  17:34:25  rich
 * Changed getMGlobal to GET_M_GLOBAL and changed getSGlobal to
 * GET_S_GLOBAL to conform to Chris' software standards.
 *
 * Patched problem with connecting between machines with different byte
 * orders.  The real fix requires changing the way formats are stored.
 * Searching for structural similar formats does not guarantee that you
 * find the right format.
 *
 * Revision 1.5  1993/11/21  20:18:46  rich
 * Added shared library for sun4c_411 sunos machines.
 * Added install to the makefile.
 * Fixed problems with global variables.
 *
 * Revision 1.4  1993/08/27  08:38:50  fedor
 * Pass 2 aat a V7+V6+VxWorks merge. Many many problems with pointless casting.
 *
 * Revision 1.3  1993/08/27  07:15:56  fedor
 * First Pass at V7 and V6+VXWORKS merge
 *
 * Revision 1.2  1993/05/26  23:18:30  rich
 * Fixed up the comments at the top of the file.
 *
 * Revision 1.1.1.1  1993/05/20  05:45:28  rich
 * Importing x_ipc version 8
 *
 * Revision 7.1  1993/05/20  00:31:26  rich
 * RTG - initial checkin of Chris Fedor's version 8 of x_ipc
 *
 * Revision 1.2  1993/05/19  17:24:54  fedor
 * Added Logging.
 *
 * 27-Oct-92 Richard Goodwin, School of Computer Science, CMU
 * Changed printf to fprintf(stderr... for warning messages.
 *
 * 10-Feb-89 Reid Simmons, School of Computer Science, CMU
 * Created.
 *
 * $Revision: 2.7 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#include "globalS.h"
#ifdef DOS_FILE_NAMES
#include "primFmtt.h"
#else
#include "primFmttrs.h"
#endif

static FORMAT_PTR Parse(Format_Parse_Ptr parser, int32 within_struct_flag,
			BOOLEAN *error);

static void initFormatParser (Format_Parse_Ptr formatParser)
{
  formatParser->Location = 0;
  formatParser->TokenList = NULL;
  formatParser->TokenListHead = NULL;
  formatParser->ParseString = NULL;
  formatParser->formatStack = NULL;
}

static void clearFormatParser (Format_Parse_Ptr formatParser)
{
  if (formatParser->formatStack != NULL)
    x_ipc_strListFree(&formatParser->formatStack, FALSE);
}

static void FreeTokenList (TokenPtr TokenList)
{ 
  TokenPtr LastToken;
  
  while (TokenList) {
    LastToken = TokenList;
    TokenList = TokenList->next;
    if (LastToken->Type == STR_TOK)
      x_ipcFree(LastToken->value.str);
    x_ipcFree((char *)LastToken);
  }
}

static void ParserError(TokenPtr bad_token, Format_Parse_Ptr parser,
			char *expecting)
{ 
  int i;
  
  X_IPC_MOD_WARNING2( "Format Parsing Error: Expected %s.\n%s\n", 
		expecting, parser->ParseString);
  for (i=0; i<bad_token->Loc; i++) X_IPC_MOD_WARNING( " ");
/*  X_IPC_MOD_ERROR("^\n");*/
}

static FORMAT_PTR new_f_formatter(FORMAT_CLASS_TYPE type, FORMAT_PTR formatter)
{ 
  FORMAT_PTR new_formatter;
  
  new_formatter = NEW_FORMATTER();
  new_formatter->type = type;
  new_formatter->formatter.f = formatter;
  return new_formatter;
}

/* First element in a FORMAT_ARRAY_PTR is the size of the array. */
static FORMAT_PTR new_a_formatter(FORMAT_CLASS_TYPE type, int array_size)
{ 
  FORMAT_PTR new_formatter;
  
  new_formatter = NEW_FORMATTER();
  new_formatter->type = type;
  new_formatter->formatter.a = NEW_FORMAT_ARRAY(array_size);
  new_formatter->formatter.a[0].i = array_size;
  return new_formatter;
}

static FORMAT_PTR new_n_formatter(char *name)
{ 
  FORMAT_PTR new_formatter;
  
  new_formatter = NEW_FORMATTER();
  new_formatter->type = NamedFMT;
  new_formatter->formatter.name = strdup(name);
  return new_formatter;
}

/* Needed by Named_Format */
static FORMAT_ARRAY_PTR copyFormatArray (CONST_FORMAT_PTR format)
{
  int size;
  FORMAT_ARRAY_PTR copiedFormatArray;

  size = format->formatter.a[0].i;
  copiedFormatArray = NEW_FORMAT_ARRAY(size);
  copiedFormatArray[0].i = size;
  return copiedFormatArray;
}

static FORMAT_PTR copyFormatter (CONST_FORMAT_PTR format)
{
  int i;
  FORMAT_ARRAY_PTR format_array;
  FORMAT_PTR copiedFormat = NULL;

  if (format) {
    copiedFormat = NEW_FORMATTER();
    *copiedFormat = *format;
    switch (format->type) {
    case PrimitiveFMT:
    case LengthFMT: 
    case BadFormatFMT:
      break;

    case PointerFMT: 
      copiedFormat->formatter.f = copyFormatter(format->formatter.f);
      break;

    case StructFMT:
      copiedFormat->formatter.a = format_array = copyFormatArray(format);
      for (i=1; i<format_array[0].i; i++) {
	format_array[i].f = copyFormatter(format->formatter.a[i].f);
      }
      break;

    case FixedArrayFMT:
    case VarArrayFMT:
      copiedFormat->formatter.a = format_array = copyFormatArray(format);
      format_array[1].f = copyFormatter(format->formatter.a[1].f);
      for (i=2; i<format_array[0].i; i++) {
	format_array[i].i = format->formatter.a[i].i;
      }
      break;

    case NamedFMT: 
      copiedFormat->formatter.name = strdup(format->formatter.name);
      break;

    case EnumFMT:
      copiedFormat->formatter.a = format_array = copyFormatArray(format);
      format_array[1].i = format->formatter.a[1].i;
      for (i=2; i<format_array[0].i; i++) {
	format_array[i].f = copyFormatter(format->formatter.a[i].f);
      }
      break;
    }
  }
  return copiedFormat;
}

static FORMAT_PTR Struct_Format(Format_Parse_Ptr parser, BOOLEAN *error)
{ 
  FORMAT_PTR Form, subform;
  LIST_PTR format_list;
  int num_formats, i;
  
  format_list = x_ipc_listCreate();
  num_formats = 0;
  
  if (parser->TokenList->Type != RBRACE_TOK) {
    while (1) {
      x_ipc_listInsertItem((char *)Parse(parser, TRUE, error), format_list);
      num_formats++;
      if (parser->TokenList->Type == COMMA_TOK) {
	(void)NextToken(parser);
      } else if (parser->TokenList->Type == RBRACE_TOK) {
	break;
      } else {
	*error = TRUE;
	ParserError(NextToken(parser), parser, "','");
	return NULL;
      }
    }
  }
  
  Form = new_a_formatter(StructFMT, num_formats+1);
  /* Index from high to low since "format_list" 
     has formatters in reverse order */
  subform = (FORMAT_PTR)x_ipc_listFirst(format_list);
  for(i=num_formats;i>0;i--) {
    Form->formatter.a[i].f = subform;
    subform = (FORMAT_PTR)x_ipc_listNext(format_list);
  }
  
  x_ipc_listFree(&format_list);
  return Form;
}

static FORMAT_PTR Enum_Format(Format_Parse_Ptr parser, BOOLEAN *error)
{ 
  TokenPtr Token;
  FORMAT_PTR Form, subform;
  LIST_PTR format_list;
  int num_formats, i, maxVal;
  
  num_formats = 0;
  Token = NextToken(parser);
  if (Token->Type == COLON_TOK) {
    Token = NextToken(parser);
    if (Token->Type != INT_TOK) {
      *error = TRUE;
      ParserError(Token, parser, "an integer");
      return NULL;
    } else {
      maxVal = Token->value.num;
    }
  } else {
    format_list = x_ipc_listCreate();
    do {
      if (num_formats > 0) Token = NextToken(parser);
      if (Token->Type != STR_TOK) {
	*error = TRUE;
	ParserError(Token, parser, "a string");
	return NULL;
      } else {
	Form = new_n_formatter(Token->value.str);
	/* More efficient for Lisp if all enum format names are upper case */
	LOCK_M_MUTEX;
	if (IS_LISP_MODULE()) {
	  upcase(Form->formatter.name);
	}
	UNLOCK_M_MUTEX;
	x_ipc_listInsertItem((char *)Form, format_list);
	num_formats++;
      }
      Token = NextToken(parser);
    } while (Token->Type == COMMA_TOK);
    UngetToken(parser, Token);
    maxVal = num_formats - 1;
  }
  
  Form = new_a_formatter(EnumFMT, num_formats+2);
  Form->formatter.a[1].i = maxVal;
  if (num_formats > 0) {
    /* Index from high to low since "format_list" 
       has formatters in reverse order */
    subform = (FORMAT_PTR)x_ipc_listFirst(format_list);
    for(i=num_formats;i>0;i--) {
      Form->formatter.a[i+1].f = subform;
      subform = (FORMAT_PTR)x_ipc_listNext(format_list);
    }
    x_ipc_listFree(&format_list);
  }
  return Form;
}

/* this parsing rule allows compatibility with RTC fixed array formats
   like [17:char].  note it doesn't handle more general formats like
   [3,4:char]. use of these formats is deprecated: use, for example,
   [char:17] instead */
static FORMAT_PTR Backwards_Fixed_Array_Format(Format_Parse_Ptr parser, 
					       BOOLEAN *error)
{ 
  FORMAT_PTR Form, next_format;
  TokenPtr tmp, arraySizeToken;
  
  arraySizeToken = NextToken(parser);
  if (INT_TOK != arraySizeToken->Type) {
    ParserError(arraySizeToken, parser, "an integer value");
    return NULL;
  }
  tmp = NextToken(parser);
  if (COLON_TOK != tmp->Type) {
    ParserError(tmp, parser, "':'");
    return NULL;
  }
  next_format = Parse(parser, FALSE, error); /* Formatter */
  tmp = NextToken(parser);
  if (RBRACK_TOK != tmp->Type) {
    ParserError(tmp, parser, "']'");
    return NULL;
  }

  Form = new_a_formatter(FixedArrayFMT, 3);
  Form->formatter.a[1].f = next_format;
  Form->formatter.a[2].i = arraySizeToken->value.num;
  
  return Form;
}

static FORMAT_PTR Fixed_Array_Format(Format_Parse_Ptr parser, BOOLEAN *error)
{ 
  FORMAT_PTR Form, next_format;
  TokenPtr Token, tmp;
  int NumberOfIndexes, Continue, array_index;
  
  tmp = parser->TokenList;
  if (INT_TOK == tmp->Type) {
    return Backwards_Fixed_Array_Format(parser, error);
  }
  next_format = Parse(parser, FALSE, error); /* Formatter */
  
  Token = NextToken(parser);
  if (Token->Type != COLON_TOK) {
    ParserError(Token, parser, "':'");
    return NULL;
  }
  
  tmp = parser->TokenList;
  NumberOfIndexes = 0;
  Continue = TRUE;
  do {
    if (tmp->Type != INT_TOK) {
      ParserError(tmp, parser, "an integer value");
      return NULL;
    }
    else {
      NumberOfIndexes++;
      tmp = tmp->next;
      if (tmp->Type == COMMA_TOK) {
	tmp = tmp->next;
	Continue = TRUE;
      } else if (tmp->Type == RBRACK_TOK) {
	Continue = FALSE;
      } else {
	ParserError(tmp, parser, "a ',' or ']'");
	return NULL;
      }
    }
  } while (Continue);
  
  Form = new_a_formatter(FixedArrayFMT, NumberOfIndexes+2);
  Form->formatter.a[1].f = next_format;
  
  /* this time munch tokens */
  
  NumberOfIndexes += 2;
  for (array_index=2; array_index < NumberOfIndexes; array_index++) {
    Token = NextToken(parser); /* This is an INT_TOK */
    Form->formatter.a[array_index].i = Token->value.num;
    
    Token = NextToken(parser); /* This is a COMMA_TOK or a RBRACK_TOK */
  }
  
  return Form;
}

static FORMAT_PTR Var_Array_Format(Format_Parse_Ptr parser, BOOLEAN *error)
{ 
  FORMAT_PTR Form, next_format;
  TokenPtr Token, tmp;
  int NumberOfIndexes, Continue, array_index;
  
  next_format = Parse(parser, FALSE, error); /* Formatter */
  
  Token = NextToken(parser);
  if (Token->Type != COLON_TOK) {
    ParserError(Token, parser, "':'");
    return NULL;
  }
  
  tmp = parser->TokenList;
  NumberOfIndexes = 0;
  Continue = TRUE;
  do {
    if (tmp->Type != INT_TOK) {
      ParserError(tmp, parser, "an integer value");
      return NULL;
    } else {
      NumberOfIndexes++;
      tmp = tmp->next;
      if (tmp->Type == COMMA_TOK) {
	tmp = tmp->next;
	Continue = TRUE;
      } else if (tmp->Type == GT_TOK) {
	Continue = FALSE;
      } else {
	ParserError(tmp, parser, "a ',' or '>'");
	return NULL;
      }
    }
  } while (Continue);
  
  Form = new_a_formatter(VarArrayFMT, NumberOfIndexes+2);
  Form->formatter.a[1].f = next_format;
  
  /* this time munch tokens */
  
  NumberOfIndexes += 2;
  for (array_index=2; array_index < NumberOfIndexes; array_index++) {
    Token = NextToken(parser); /* This is an INT_TOK */
    Form->formatter.a[array_index].i = Token->value.num;
    
    Token = NextToken(parser); /* This is a COMMA_TOK or a GT_TOK */
  }
  
  return Form;
}

static FORMAT_PTR SelfPtr_Format(void)
{ 
  return new_f_formatter(PointerFMT, (FORMAT_PTR)NULL);
}

static FORMAT_PTR Ptr_Format(Format_Parse_Ptr parser, BOOLEAN *error)
{ 
  return new_f_formatter(PointerFMT, Parse(parser, FALSE, error));
}

static FORMAT_PTR Length_Format(TokenPtr Token)
{ 
  return x_ipc_createIntegerFormat(LengthFMT, Token->value.num);
}

static FORMAT_PTR Named_Format(Format_Parse_Ptr parser, TokenPtr Token)
{ 
  NAMED_FORMAT_PTR namedFormat;
  FORMAT_PTR format;
  
  if (parser->formatStack == NULL) {
    parser->formatStack = x_ipc_strListCreate();
  }
  
  if (x_ipc_strListMemberItem(Token->value.str, parser->formatStack)) {
    /* found a recursive definition. just return pointer */
    return new_n_formatter(Token->value.str);
  }
  
  x_ipc_strListPush(strdup(Token->value.str), parser->formatStack);

  LOCK_M_MUTEX;
  namedFormat = 
    (NAMED_FORMAT_PTR)x_ipc_hashTableFind(Token->value.str,
				    GET_M_GLOBAL(formatNamesTable));
  UNLOCK_M_MUTEX;
  if (!namedFormat) {
#ifdef NMP_IPC
    if (GET_C_GLOBAL(serverRead) != CENTRAL_SERVER_ID) {
      if (x_ipcQueryCentral(X_IPC_NAMED_FORM_QUERY,
			  &Token->value.str, &format) != Success ||
	  !format) {
        X_IPC_MOD_WARNING2("Warning: Named Format %s is not registered\n%s\n",
			   Token->value.str, parser->ParseString);
        format = new_f_formatter(BadFormatFMT, NULL);
      }
    } else
#endif
      {
	X_IPC_MOD_WARNING2("Warning: Named Format %s is not registered\n%s\n",
			   Token->value.str, parser->ParseString);
	format = new_f_formatter(BadFormatFMT, NULL);
      }
  } else {
    /* Need to use the named formatter -- parse it now */
    if (!namedFormat->parsed) {
      namedFormat->format = 
	(FORMAT_PTR)ParseFormatString(namedFormat->definition);
      namedFormat->parsed = TRUE;
    }
    format = copyFormatter(namedFormat->format);
  }
  
  x_ipcFree((void *)x_ipc_strListPopItem(parser->formatStack));
  return format;
}

static FORMAT_PTR Primitive_Format(Format_Parse_Ptr parser, TokenPtr Token)
{ 
  return Named_Format(parser, Token);
}

/*********************************************/

/* RGS 11/12/93: Made format string parser re-entrant: 
 *               Needed for lazy evaluation of named formatters
 */
CONST_FORMAT_PTR ParseFormatString(const char *FormString)
{ 
  CONST_FORMAT_PTR Formatter;
  BOOLEAN error = FALSE;
  Format_Parse_Type parser;
  
  initFormatParser(&parser);
  
  parser.ParseString = FormString;
  LexString(&parser, FormString);
  parser.TokenListHead = parser.TokenList;
  Formatter = Parse(&parser, FALSE, &error);
  if (error) Formatter = new_f_formatter(BadFormatFMT, NULL);
  FreeTokenList(parser.TokenListHead);
  
  clearFormatParser(&parser);

  return Formatter;
}


/* "within_struct_flag" is TRUE if the enclosing format is "{...}" */
static FORMAT_PTR Parse(Format_Parse_Ptr parser, int32 within_struct_flag,
			BOOLEAN *error)
{ 
  TokenPtr Token;
  FORMAT_PTR ReturnForm=NULL;
  
  Token = NextToken(parser);
  
  switch(Token->Type) {
  case LBRACE_TOK:
    Token = NextToken(parser);
    if (Token->Type == STR_TOK && !strcmp(Token->value.str, "enum")) {
      ReturnForm = Enum_Format(parser, error);
    } else {
      UngetToken(parser, Token);
      ReturnForm = Struct_Format(parser, error);
    }
    Token = NextToken(parser);
    if (Token->Type != RBRACE_TOK) {
      ParserError(Token, parser, "'}'");
      *error = TRUE;
      return NULL;
    }
    break;
  case LBRACK_TOK:
    ReturnForm = Fixed_Array_Format(parser, error);
    break;
  case LT_TOK:
    if (!within_struct_flag) {
      ParserError(Token, parser,
		  "var array format '<..>' not embedded within a structure '{..}'");
      *error = TRUE;
      return NULL;
    } else {
      ReturnForm = Var_Array_Format(parser, error);
    }
    break;
  case STAR_TOK:
    if (parser->TokenList->Type == BANG_TOK) {
      Token = NextToken(parser);	/* eat BANG_TOK */ 
      if (!within_struct_flag) {
	ParserError(Token, parser, 
		    "self pointer '*!' embedded within a structure '{..}'");
	*error = TRUE;
	return NULL;
      } else {
	ReturnForm = SelfPtr_Format();
      }
    }
    else
      ReturnForm = Ptr_Format(parser, error);
    break;
  case INT_TOK:
    ReturnForm = Length_Format(Token);
    break;
  case STR_TOK:
    ReturnForm = Named_Format(parser, Token);
    if (ReturnForm->type == BadFormatFMT) {
      *error = TRUE;
      return NULL;
    }
    break;
  case PRIMITIVE_TOK:
    ReturnForm = Primitive_Format(parser, Token);
    if (ReturnForm->type == BadFormatFMT) {
      *error = TRUE;
      return NULL;
    }
    break;
  case EOS_TOK: 
    ParserError(Token, parser, 
		"additional tokens; premature end of string encountered");
    *error = TRUE;
    return NULL;
    break;

  default:
    ParserError(Token, parser, "a different token type");
    *error = TRUE;
    return NULL;
  }
  
  cacheFormatterAttributes(ReturnForm);

  return ReturnForm;
}

/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.5 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  
 * COMMENTS:     large parts of this code are taken from a flex spec
 *               written by Kurt Schwehr at Ames 7/96
 *
 * (c) Copyright 1999 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

%{
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <iostream>

#include "strxcpy.h"
#include "ParseError.h"
#include "XDRTree.h"
#include "XDRInternal.h"
#include "gen/XDR.tab.hh"

#define YY_NO_UNPUT /* Stop getting a warning about it not getting used */

#define YY_MAX_INCLUDES 10

using namespace std;

static ParsePosSharedInfo *yyPosSharedInfoG = 0;
static int yyLineNumberG = -1;

// this function gets called when we notice a cpp line annotation that
// indicates we're looking at a new file (cpp creates these whenever
// there are #include statements in the input file).  we'll keep track
// of the stack of files that included the one we're currently looking at,
// so we can give a trace back to the user 
static void
yyNewFile(char *cppTag) {
  char *tagP, cppFileName[PATH_MAX];
  int cppLineNumber;

  if (0 == strncmp(cppTag,"#line",5)) {
    tagP = cppTag+6;
  } else {
    tagP = cppTag+2;
  }

  sscanf (tagP,"%d \"%[^\"]\"", &cppLineNumber, cppFileName);

  // take off any leading ./
  if (0 == strncmp(cppFileName,"./",2)) {
    strcpy(cppFileName, cppFileName+2);
  }

  // make a copy of the shared info and update reference counts.
  // afterwards we will be free to modify shared info without affecting
  // other data structures which may refer to the old copy
  ParsePosSharedInfo *oldShared = yyPosSharedInfoG;
  yyPosSharedInfoG = new ParsePosSharedInfo(*oldShared);
  yyPosSharedInfoG->ref();
  oldShared->unref();

  // either add a file to the stack, or pop a file off (depending
  // on the cpp directive)
  if (yyPosSharedInfoG->stack.size() > 0
      && yyPosSharedInfoG->stack.back().fileName == cppFileName) {
    // popping a file off the stack
    ParseFilePos includingFilePos = yyPosSharedInfoG->stack.back();
    yyPosSharedInfoG->stack.pop_back();
    yyPosSharedInfoG->fileName = includingFilePos.fileName;
    yyLineNumberG = includingFilePos.lineNumber;
  } else {
    // adding a file to the stack
    yyPosSharedInfoG->stack.
      push_back(ParseFilePos(yyPosSharedInfoG->fileName, yyLineNumberG));
    yyPosSharedInfoG->fileName = cppFileName;
    yyLineNumberG = cppLineNumber;
  }
}

#define DO_DEBUG_TOKENS 0

#if DO_DEBUG_TOKENS
/* echo token */
static void
DB(const char *s) {
#ifndef NDEBUG
    if (strcmp(s,"\n")!=0)
	printf ("[%s] ",s);
    else
	printf ("\n");	/* Don't do square brackets for new lines. */
#endif
}

#else // if DO_DEBUG_TOKENS
/* no op */
#define DB(s)
#endif // if DO_DEBUG_TOKENS

static char *
stringFromChar(char c) {
  static char s[2];
  s[0] = c;
  s[1] = '\0';
  return s;
}

static void
notImplemented(char *s) {
  char buf[1024];
  snprintf(buf,sizeof(buf),"XDR token `%s' not supported.",s);
  yyerror(buf);
}

#define NOT_IMPLEMENTED(s) \
  notImplemented(#s); \
  yyterminate();

#define NOT_IMPLEMENTED_PUNCT(c) \
  notImplemented(stringFromChar(c)); \
  yyterminate();

ParsePos
yypos(void) {
  return ParsePos(yytext,yyLineNumberG,yyPosSharedInfoG);
}

#define RETURN_RESERVED(t) \
  DB(#t); \
  return (t);

#define RETURN_PUNCT(t) \
  DB(stringFromChar(t)); \
  return ((int) t);

%}
/* %start */

digit = [0-9];
ws = [\ \t];
identifier = [A-Za-z][A-Za-z0-9_]*;

    /* Handy regular expression substitutions. */
WS		([ \t])
LETTER		([a-zA-Z])
DIGIT		([0-9])
INTEGER		(("+"|"-")?{DIGIT}+)
SCALEFACTOR	("E"("+"|"-")?{DIGIT}+)
REAL		(("+"|"-")?{DIGIT}+"."{DIGIT}*{SCALEFACTOR}?)
WORD		({LETTER}({LETTER}|{DIGIT})*)

	/* Variable names/identifiers */
VARLETTER	([a-zA-Z_])
VARWORD		({VARLETTER}({VARLETTER}|{DIGIT})*)

  /* char and strings */
CCHAR		(\'(\\[ntvbrfa\'\"\\]|\\[0-9]{3}|[^\'\\])\')
CSCHR		(\\[ntvbrfa\'\"\\]|\\[0-9]{3}|[^\"\\])
CSTRING         (\"{CSCHR}*\")

  /* start states */
%s norm
%x comment passThroughCode

%%

"//".*\n	{
                  /* single-line comment... do nothing */
                  yyLineNumberG++; DB("\n");
                }


  /********** RULES FOR 'comment' START STATE **********/
"/*"            { BEGIN(comment); }

<comment>{
  [^*\n]*         { /* eat anything that's not a '*' */ }
  "*"+[^*/\n]*    { /* eat up '*'s not followed by '/'s */ }
  \n 		  { yyLineNumberG++; DB("\n"); }
  "*"+"/"         { BEGIN(norm); }
}
  /********** END RULES FOR 'comment' START STATE **********/


  /********** RULES FOR 'passThroughCode' START STATE **********/
"%{"            {
		  yylval.passThroughVal = new XDRPassThroughCodeNode(yypos());
                  BEGIN(passThroughCode);
                }

<passThroughCode>{
  [^%\n]*       {
                  /* eat anything that's not a '%' */
                  yylval.passThroughVal->code << yytext;
	        }
  "%"+[^%}\n]*  {
                  /* eat up '%'s not followed by '}'s */
                  yylval.passThroughVal->code << yytext;
                }
  \n 		{
                  yylval.passThroughVal->code << yytext;
                  yyLineNumberG++; DB("\n");
                }
  "%}"          {
                  yylval.passThroughVal->code << ends;
                  yylval.passThroughVal->endPos = yypos();
                  BEGIN(norm);
	          return PASS_THROUGH_CODE;
                }
}
  /********** END RULES FOR 'passThroughCode' START STATE **********/


  /* rules for dealing with cpp line directives (not really applicable
     now, but i'm keeping them around to use as an example of
     how to do it) */
^#{WS}{INTEGER}{WS}{CSTRING}{WS}{INTEGER}\n {
  DB("CPP-LINE-MARK");
  yyNewFile(yytext);
}

^#{WS}{INTEGER}{WS}{CSTRING}\n {
  DB("CPP-LINE-MARK");
  yyNewFile(yytext);
}

^#line{WS}{INTEGER}{WS}{CSTRING}\n {
  DB("CPP-LINE-MARK");
  yyNewFile(yytext);
}

bool		  {RETURN_RESERVED(BOOL);}
char		  {RETURN_RESERVED(CHAR);}
double		  {RETURN_RESERVED(DOUBLE);}
float		  {RETURN_RESERVED(FLOAT);}
string		  {RETURN_RESERVED(STRING);}
struct		  {RETURN_RESERVED(STRUCT);}
typedef		  {RETURN_RESERVED(TYPEDEF);}
ipc_type	  {RETURN_RESERVED(IPC_TYPE);}
unsigned	  {RETURN_RESERVED(UNSIGNED);}
void		  {RETURN_RESERVED(VOID);}
opaque		  {RETURN_RESERVED(OPAQUE);}
long              {RETURN_RESERVED(LONG);}
int               {RETURN_RESERVED(INT);}
short             {RETURN_RESERVED(SHORT);}
enum              {RETURN_RESERVED(ENUM);}

const		  {NOT_IMPLEMENTED(CONST);}
default           {NOT_IMPLEMENTED(DEFAULT);}
hyper             {NOT_IMPLEMENTED(HYPER);}
switch            {NOT_IMPLEMENTED(SWITCH);}
union             {NOT_IMPLEMENTED(UNION);}
case              {NOT_IMPLEMENTED(CASE);}

"["               {RETURN_PUNCT('[');}
"]"               {RETURN_PUNCT(']');}
"{"               {RETURN_PUNCT('{');}
"}"               {RETURN_PUNCT('}');}
"<"               {RETURN_PUNCT('<');}
">"               {RETURN_PUNCT('>');}
"("               {RETURN_PUNCT('(');}
")"               {RETURN_PUNCT(')');}
"."               {RETURN_PUNCT('.');}
","               {RETURN_PUNCT(',');}
";"               {RETURN_PUNCT(';');}
":"               {RETURN_PUNCT(':');}
"="               {RETURN_PUNCT('=');}


"*"               {NOT_IMPLEMENTED_PUNCT('*');}

{VARWORD}		{
                          yylval.stringVal = new XDRStringToken
			    (string(yytext),yypos());
			  DB("IDENTIFIER");
			  DB(yytext);
			  return (IDENTIFIER);
                        }
{INTEGER}		{
  			  yylval.longVal = new XDRLongToken
			    (strtol(yytext,(char **) NULL,10),
			     yypos());
			  DB("LONGVAL");
			  DB(yytext);
			  return (LONGVAL);
                        }
{CSTRING}		{
                          string stext(yytext);
			  stext = stext.substr(1,stext.size()-2);
			  char *tmp = new char[(unsigned) stext.size()+1];
			  // strcadd handles C escape sequences like \n
			  strcadd(tmp,stext.c_str());
			  yylval.stringVal =
			    new XDRStringToken(tmp,yypos());
			  delete tmp;
			  DB("STRINGVAL");
			  DB(yytext);
			  return (STRINGVAL);
			}			  

\n 		 	{ yyLineNumberG++; DB("\n"); }
{WS}		        { /* NOP -> Suck up white space */ }

<<EOF>>         	{ yyterminate(); }

%%  /* C code section */

/* lexinit - starts the Lexical Analyzer off in the right start condition */
void
lexinit(void) {
  yyPosSharedInfoG = new ParsePosSharedInfo(yyinName);
  yyPosSharedInfoG->ref();
  yyLineNumberG = 1;
  return;
}

void
yyerror(const char *s) { 
  ParseError e(s, yypos());
  cerr << e;
}

/* This is a kludge so that we don't need to link against libfl.
   If we don't do this *and* we don't link against libfl, it complains
   that yywrap() is undefined.

   This definition is exactly what is in libyywrap.c in flex-2.5.4a */
int
yywrap(void)
{
    return (1);
}

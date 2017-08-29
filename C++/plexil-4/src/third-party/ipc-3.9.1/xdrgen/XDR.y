/****************************************************************************
 * $Revision: 1.4 $  $Author: reids $  $Date: 2009/01/12 15:54:59 $
 *
 * PROJECT:      Distributed Robotic Agents
 * DESCRIPTION:  Bison(yacc) source grammar for parsing config files.
 *               Builds a parse tree.
 *
 * (c) Copyright 1999 CMU. All rights reserved.
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 ***************************************************************************/

%{ /* C code section */

#define YYERROR_VERBOSE 1
#define YYDEBUG 1

#include <stdlib.h>	/* For malloc */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
/* #include <ctype.h> */
#include <iostream>

#include "XDRTree.h"
#include "XDRInternal.h"

#ifndef IRIX
extern void __yy_memcpy (char *to, char *from, int count);
#endif

 using namespace std;

/*** EXPORTED DATA STRUCTURE ***/

XDRSpecification *yyTree;
const char *yyinName;

%} /* End C code section */

%start specification

%union          yystacktype
                {
		  XDRLongToken		  *longVal;
		  XDRStringToken	  *stringVal;

		  XDRDeclNode             *declVal;
		  XDRArrayDimNode         *arrayDimVal;
		  XDRArrayDimsNode        *arrayDimsVal;
		  XDRTypeSpecNode         *typeVal;
		  XDRTypeSpecStringNode   *typeStringVal;
		  XDRTypeSpecStructNode   *typeStructVal;
		  XDRDeclListNode         *declListVal;
		  XDRTypeDefNode          *typeDefVal;
		  XDRIPCTypeNode          *ipcTypeVal;
		  XDRDefinitionNode       *defVal;
		  XDRDefinitionListNode   *defListVal;
		  XDRSpecification        *specVal;
		  XDRPassThroughCodeNode  *passThroughVal;
		  XDREnumListNode         *enumListVal;
		  XDREnumEntryNode        *enumEntryVal;
		}


%token <stringVal> IDENTIFIER STRINGVAL
%token <longVal> LONGVAL
%token <passThroughVal> PASS_THROUGH_CODE

/* Reserved words */
%token BOOL CHAR CONST DOUBLE FLOAT STRING STRUCT
%token TYPEDEF IPC_TYPE UNSIGNED VOID OPAQUE LONG INT SHORT
%token ENUM

/* Reserved words which aren't implemented */
%token DEFAULT ENUM HYPER SWITCH UNION CASE

/* figure out actual types later */

%type <declVal> decl
%type <longVal> value
%type <arrayDimVal> varArray fixArray
%type <arrayDimsVal> varArrayDims fixArrayDims
%type <typeVal> typeSpecifier
%type <typeStringVal> stringTypeSpec
%type <typeStructVal> structBody structInternal
%type <typeVal> enumTypeSpec enumBody
%type <declListVal> declList
%type <defVal> definition
%type <defListVal> definitionList
%type <typeDefVal> typeDef
%type <passThroughVal> optCode
%type <ipcTypeVal> ipcType
%type <enumEntryVal> enumEntry
%type <enumListVal> enumList

/* overall specification returned in yyTree global variable */
%type <specVal> specification

/* there is one known and ok shift/reduce conflict; %expect suppresses
   the warning */
/* %expect 1 */

%%  /* -------------------------------------------------------------------- */
    /* ------------  Begin the rule section of the yacc/bison file -------- */
    /* -------------------------------------------------------------------- */

ipcType        : IPC_TYPE IDENTIFIER ';'
               { $$ = new XDRIPCTypeNode($2); }
               | IPC_TYPE IDENTIFIER '=' STRINGVAL ';'
               {
		 $$ = new XDRIPCTypeNode($2);
		 $$->addFormatString($4);
	       };

decl           : typeSpecifier IDENTIFIER
                 {
		   $$ = new XDRDeclNode($2,$1);
                 }
               /* these top rules form a shift/reduce conflict.
                  we want to shift in this conflict, which is bison's
                  default action, so we can ignore the warning */
               | typeSpecifier IDENTIFIER fixArrayDims
                 {
		   $1->addDims(A_FIXED_ARRAY,$3);
		   $$ = new XDRDeclNode($2,$1);
                 }
               | typeSpecifier IDENTIFIER varArrayDims
                 {
		   $1->addDims(A_VAR_ARRAY,$3);
		   $$ = new XDRDeclNode($2,$1);
                 }
               /* special case: last array dimension of a string must be <> */
               | stringTypeSpec IDENTIFIER varArrayDims
                 {
                   /* take the last var array dimension off the back
		      and make it the string array dim */
		   $1->addStringArrayDim($3->getItems().back());
		   $3->pop_back();
		   if ($3->getItems().size() > 0) $1->addDims(A_VAR_ARRAY,$3);
		   $3->checkRef();
		   $$ = new XDRDeclNode($2,$1);
		 }
               | stringTypeSpec IDENTIFIER fixArrayDims varArray
                 {
		   $1->addDims(A_FIXED_ARRAY,$3);
		   $1->addStringArrayDim($4);
		   $$ = new XDRDeclNode($2,$1);
		 };
/*
               | typeSpecifier '*' IDENTIFIER
*/

varArrayDims   : varArray
                 {
		   $$ = new XDRArrayDimsNode;
		   $$->push_back($1);
                 }
               | varArrayDims varArray
                 {
		   $$ = $1;
		   $$->push_back($2);
		 };

fixArrayDims   : fixArray
                 {
		   $$ = new XDRArrayDimsNode;
		   $$->push_back($1);
                 }
               | fixArrayDims fixArray
                 {
		   $$ = $1;
		   $$->push_back($2);
		 };

varArray       : '<' value '>'
                 { $$ = new XDRArrayDimNode($2); }
               | '<'       '>'
                 { $$ = new XDRArrayDimNode(); };

fixArray       : '[' value ']'
                 { $$ = new XDRArrayDimNode($2); };

value          : LONGVAL
                 { $$ = $1; };
/*
               | IDENTIFIER
*/


stringTypeSpec : STRING
                 { $$ = new XDRTypeSpecStringNode; };

typeSpecifier  : UNSIGNED INT
	         { $$ = new XDRTypeSpecNode(T_UINT); }
               | INT
                 { $$ = new XDRTypeSpecNode(T_INT); }

/*
               | UNSIGNED HYPER
               |          HYPER
*/
               | FLOAT
	         { $$ = new XDRTypeSpecNode(T_FLOAT); }
               | DOUBLE
	         { $$ = new XDRTypeSpecNode(T_DOUBLE); }
               | BOOL
	         { $$ = new XDRTypeSpecNode(T_BOOL); }
               | STRUCT structBody
	         { $$ = $2; }
               | IDENTIFIER
	         { $$ = new XDRTypeSpecIdentNode($1); }
               /* XDR only defines the 'opaque' keyword for byte arrays.
		  here we add 'char' and 'unsigned char' to give more
		  control over the C header output (and because it's
		  more natural for C programmers) */
               | OPAQUE
                 { $$ = new XDRTypeSpecNode(T_UCHAR); }
               | UNSIGNED CHAR
                 { $$ = new XDRTypeSpecNode(T_UCHAR); }
               | CHAR
                 { $$ = new XDRTypeSpecNode(T_CHAR); }
               | enumTypeSpec;
/*
               | unionTypeSpec
*/

enumTypeSpec   : ENUM enumBody
                 { $$ = $2; };

enumBody       : '{' enumList '}'
                 { $$ = new XDRTypeSpecEnumNode($2); };

enumList       : enumEntry
                 {
		   $$ = new XDREnumListNode;
		   $$->push_back($1);
		 }
               | enumList ',' enumEntry
                 {
		   $1->push_back($3);
		   $$ = $1;
		 };

enumEntry      : IDENTIFIER
                 { $$ = new XDREnumEntryNode($1); }
               | IDENTIFIER '=' value
                 {
		   $$ = new XDREnumEntryNode($1);
		   $$->addValue($3);
		 };

structInternal : declList
	         { $$ = new XDRTypeSpecStructNode($1); }
               | VOID ';'
                 {
		   XDRDeclListNode *dl = new XDRDeclListNode;
		   $$ = new XDRTypeSpecStructNode(dl);
		 };

structBody     : '{' structInternal optCode '}'
                 {
		   if (0 != $3) $2->addAfterDecls($3);
		   $$ = $2;
                 };

declList       : decl ';'
	         {
		   $$ = new XDRDeclListNode;
		   $$->push_back($1);
		 }
               | declList decl ';'
	         {
		   $1->push_back($2);
		   $$ = $1;
		 };

/**********************************************************************
 UNION AND CONST NOT IMPLEMENTED

unionTypeSpec  : UNION unionBody ;

unionBody      : SWITCH '(' decl ')' '{' caseList defaultEntry '}'
               | SWITCH '(' decl ')' '{' caseList              '}' ;

caseList       : caseEntry
               | caseList caseEntry ;

caseEntry      : CASE value ':' decl ';' ;

defaultEntry   : DEFAULT value ':' decl ';' ;

constantDef    : CONST IDENTIFIER '=' constant ';' ;

 END OF UNION AND CONST RULES
 **********************************************************************/

typeDef        : TYPEDEF decl ';'
	         { $$ = $2; }
               | STRUCT IDENTIFIER structBody ';'
	         { $$ = new XDRTypeDefNode($2,$3); }
               | ENUM IDENTIFIER enumBody ';'
                 { $$ = new XDRTypeDefNode($2,$3); } ;
/*
               | UNION IDENTIFIER unionBody ';'
*/

definition     : typeDef
	         { $$ = new XDRDefinitionTypeDefNode($1); }
               | ipcType
	         { $$ = new XDRDefinitionIPCTypeNode($1); }
               | PASS_THROUGH_CODE
                 { $$ = new XDRDefinitionCodeNode($1)} ;
/*
               | constantDef
*/

definitionList : /* EMPTY */
	         { $$ = new XDRDefinitionListNode; }
               | definitionList definition
                 {
		   $1->push_back($2);
		   $$ = $1;
		 } ;

optCode        : /* EMPTY */
                 { $$ = 0; }
               | PASS_THROUGH_CODE
                 { $$ = $1; } ;

specification  : definitionList
                 { 
		   $$ = new XDRSpecification($1,yyinName);
		   if (0 != fstat(fileno(yyin), &($$->specFileStat))) {
		     cerr << "xdrgen: couldn't fstat " << yyinName << ": "
			  << strerror(errno) << endl;
		     exit(EXIT_FAILURE);
		   }
		   yyTree = $$;
		   yyTree->ref();
		 } ;

%%
/* C code section */

/***************************************************************************
 * REVISION HISTORY:
 * $Log: XDR.y,v $
 * Revision 1.4  2009/01/12 15:54:59  reids
 * Added BSD Open Source license info
 *
 * Revision 1.3  2004/04/06 15:06:08  trey
 * updated for more recent bison and g++
 *
 * Revision 1.2  2003/02/13 20:47:19  reids
 * Updated to compile under gcc 3.0.1
 *
 * Revision 1.1  2001/03/16 17:56:01  reids
 * Release of Trey's code to generate IPC format strings from XDR definitions.
 *
 * Revision 1.3  2001/02/08 04:14:56  trey
 * added enumerated types and fixed C language output
 *
 * Revision 1.2  2001/02/08 00:41:57  trey
 * added external IPC format feature to xdrgen; we also now tag the generated file with a version string
 *
 * Revision 1.1  2001/02/06 02:00:55  trey
 * initial check-in
 *
 * Revision 1.2  1999/11/08 15:34:59  trey
 * major overhaul
 *
 * Revision 1.1  1999/11/03 19:31:34  trey
 * initial check-in
 *
 *
 ***************************************************************************/

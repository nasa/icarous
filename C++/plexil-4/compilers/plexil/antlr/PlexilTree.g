// Copyright (c) 2006-2011, Universities Space Research Association (USRA).
//  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//    * Neither the name of the Universities Space Research Association nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

tree grammar PlexilTree;

options
{
    tokenVocab = Plexil;
    ASTLabelType = PlexilTreeNode;
    output = AST;
    language = Java;
	rewrite = false;
}

@header
{
package plexil;

import plexil.*;
}

@members
{
    GlobalContext m_globalContext = GlobalContext.getGlobalContext();
    NodeContext m_currentContext = m_globalContext;
}


////////////////////////////////
// *** BEGIN TREE GRAMMAR *** //
////////////////////////////////

plexilPlan :
    ^(PLEXIL 
      declarations? 
	  action)
 ;

declarations : 
    ^(GLOBAL_DECLARATIONS declaration+)
 ;

declaration :
    commandDeclaration
  | lookupDeclaration
  | libraryActionDeclaration
 ;

commandDeclaration :
    ^(COMMAND_KYWD NCNAME paramsSpec? returnsSpec?)
 ;

lookupDeclaration :
    ^(LOOKUP_KYWD NCNAME paramsSpec? returnsSpec)
 ;

paramsSpec : ^(PARAMETERS paramSpec+ ) ;

returnsSpec : ^(RETURNS_KYWD paramSpec+ ) ;

paramSpec : ^(typeName NCNAME?) ;

libraryActionDeclaration :
    ^(LIBRARY_ACTION_KYWD NCNAME libraryParamsSpec?)
 ;

libraryParamsSpec : ^(PARAMETERS ( (IN_KYWD | IN_OUT_KYWD) typeName NCNAME INT? )* ) ;

//
// Actions
//

action :
    ^(ACTION
      (id=NCNAME
       { /* check that id is unique in current context */ } 
      )?
	  actionBody)
 ;

actionBody : compoundAction | simpleAction | block ;

compoundAction : forAction | ifAction | onCommandAction | onMessageAction | whileAction ;

forAction : 
    ^(FOR_KYWD
      { /* push new variable binding context */ }
      typeName NCNAME
      { /* declare loop variable */ }
      loopvarinit=expression
      { /* check that loopvarinit is compatible with loop var type */ }
	  endtest=expression
	  { /* check that endtest is a Boolean expression */ }
      loopvarupdate=expression
	  { /* check that loopvarupdate is compatible with loop var type */ }
	  action)
     { /* pop variable binding context */ }
 ;

ifAction :
    ^(IF_KYWD
	  expression
      { /* Check that expression is Boolean */ }
      action
      action?
     )
 ;

onCommandAction :
    ^(ON_COMMAND_KYWD
      { /* push new variable binding context */ }
      expression
      paramsSpec?
      { /* create new local variables */ }
      action)
    { /* pop variable binding context */ }
 ;

onMessageAction :
    ^(ON_MESSAGE_KYWD
      expression
      action)
 ;

whileAction :
    ^(WHILE_KYWD
      expression
      { /* Check that expression is Boolean */ }
      action)
 ;

simpleAction : assignment | commandInvocation | libraryCall | request | update ;

// *** TODO: conversion to commandInvocation
assignment :
    ^(ASSIGNMENT assignmentLHS ( expression | commandInvocation ))
 ;

assignmentLHS : arrayReference | variable ;

commandInvocation :
    ^(COMMAND
      ( expression | ^(COMMAND_KYWD NCNAME) )
      argumentList?
     )
 ;

libraryCall :
    ^(LIBRARY_CALL_KYWD 
      NCNAME
      { /* check that name is declared a library action */ }
      aliasSpecs?
      { /* check aliasSpecs against library action def'n */ }
     )
 ;

aliasSpecs : 
    ^(ALIASES aliasSpec*) 
    { /* check for duplicate alias names */ }
 ;

aliasSpec :
    ^(CONST_ALIAS NCNAME literalValue)
  | ^(VARIABLE_ALIAS NCNAME varname=NCNAME) { /* check whether varname exists in current context */ }
 ;

request :
    ^(REQUEST_KYWD
      NCNAME
      (NCNAME expression)*
     )      
 ;

update :
    ^(UPDATE_KYWD
      (NCNAME expression)*
     )      
 ;

block :
    ^((BLOCK | CONCURRENCE_KYWD | SEQUENCE_KYWD
       | UNCHECKED_SEQUENCE_KYWD | TRY_KYWD)
      comment?
      nodeDeclaration*
	  nodeAttribute*
	  action*
      { /* if zero actions, turn into empty node;
           if one action, turn into appropriate node/action type */
      }
     )
 ;

comment : ^(COMMENT_KYWD STRING) ;

nodeDeclaration : interfaceDeclaration | variableDeclaration ;

interfaceDeclaration :
    ^((IN_KYWD | IN_OUT_KYWD)
      typeName?
      NCNAME*)
     { /* check that each variable is already declared and has consistent type */
       /* restrict variable access as appropriate */
     }
 ;

variableDeclaration : 
    ^(VARIABLE_DECLARATIONS
      scalarVariableDecl*
      arrayVariableDecl*
     )
 ;

scalarVariableDecl :
    ^(typeName NCNAME
      { /* check that NCNAME is not duplicate in context */
        /* add variable to context */ }
      literalScalarValue?
      { /* check that initial value is of appropriate type */ }
     )
 ;

arrayVariableDecl :
    ^(ARRAY_VARIABLE typeName NCNAME INT
      { /* check that NCNAME is not duplicate in context */
        /* check that INT is non-negative */
        /* add variable to context */ }
      ( literalScalarValue
        { /* check that type is consistent */
          /* add initial value to variable */ }
      | literalArrayValue
        { /* check that type is consistent */
          /* check that length is <= declared length */
          /* add initial value to variable */ }
      )?
     )
 ;

typeName :
    BOOLEAN_KYWD
  | INTEGER_KYWD
  | REAL_KYWD
  | STRING_KYWD
  | DURATION_KYWD 
  | DATE_KYWD
  ;

nodeAttribute :
    nodeCondition
  | priority
  | resource
 ;

nodeCondition : 
   ^(conditionKywd expression) /* check that expression is Boolean */
 ;

conditionKywd :
    END_CONDITION_KYWD
  | INVARIANT_CONDITION_KYWD
  | POST_CONDITION_KYWD
  | PRE_CONDITION_KYWD
  | REPEAT_CONDITION_KYWD
  | SKIP_CONDITION_KYWD
  | START_CONDITION_KYWD
 ;

priority : 
    ^(PRIORITY_KYWD INT) /* check that INT is non-negative */
 ;

resource :
    ^(RESOURCE_KYWD name=expression { /* check that expression is string */ }
                    resourceOption+
     )
     { /* check that priority is one of supplied options */ }
 ;

resourceOption :
    ( ( LOWER_BOUND_KYWD | UPPER_BOUND_KYWD | PRIORITY_KYWD ) expression )
      { /* check that expression is numeric */ }
    | ( RELEASE_AT_TERM_KYWD expression )
      { /* check that expression is Boolean */ }
 ;

///////////////////
//  EXPRESSIONS  //
///////////////////

// No operator hierarchy needed at tree level; surface parser takes care of that

// *** FIXME: resolve ambiguity between binaryOp, unaryOp cases ***

expression
options { greedy=true; backtrack=true; }
 :
    ^(binaryOp expression expression) { /* check type consistency */ }
  | ^(unaryOp expression) { /* check type consistency */}
  | ^(MESSAGE_RECEIVED_KYWD STRING)
  | arrayReference
  | internalVariableReference
  | isKnownExp
  | literalValue
  | lookupExpr
  | nodeCommandHandleKywd
  | nodeFailureKywd
  | nodeOutcomeKywd
  | nodeStateKywd
  | variable
 ;

arrayReference :
    ^(ARRAY_REF
      variable { /* check that variable exists in context and is an array */ }
      expression { /* check that expression is numeric */ }
     )
 ;

internalVariableReference :
    ( ^(COMMAND_HANDLE_KYWD nodeReference) { /* check that node is a COMMAND node */ }
    | ^(nodeStatePredicate nodeReference) { /* check that node exists */ }
    | ^(FAILURE_KYWD nodeReference) { /* check that node exists */ }
    | ^(OUTCOME_KYWD nodeReference) { /* check that node exists */ }
    | ^(STATE_KYWD nodeReference) { /* check that node exists */ }
    | ^(NODE_TIMEPOINT_VALUE nodeReference nodeStateKywd timepoint) { /* check that node exists */ }
    )
 ;

nodeReference :
    SELF_KYWD
  | PARENT_KYWD
  | NCNAME
  | ^(CHILD_KYWD NCNAME)
  | ^(SIBLING_KYWD NCNAME)
 ;
  

timepoint : START_KYWD | END_KYWD ;

nodeStatePredicate :
    NODE_EXECUTING_KYWD
  | NODE_FAILED_KYWD
  | NODE_FINISHED_KYWD
  | NODE_INACTIVE_KYWD
  | NODE_INVARIANT_FAILED_KYWD
  | NODE_ITERATION_ENDED_KYWD
  | NODE_ITERATION_FAILED_KYWD
  | NODE_ITERATION_SUCCEEDED_KYWD
  | NODE_PARENT_FAILED_KYWD
  | NODE_POSTCONDITION_FAILED_KYWD
  | NODE_PRECONDITION_FAILED_KYWD
  | NODE_SKIPPED_KYWD
  | NODE_SUCCEEDED_KYWD
  | NODE_WAITING_KYWD
 ;

nodeCommandHandleKywd :
    COMMAND_ACCEPTED_KYWD
  | COMMAND_DENIED_KYWD
  | COMMAND_FAILED_KYWD
  | COMMAND_RCVD_KYWD
  | COMMAND_SENT_KYWD
  | COMMAND_SUCCESS_KYWD
 ;

nodeFailureKywd :
    PRE_CONDITION_FAILED_KYWD
  | POST_CONDITION_FAILED_KYWD
  | INVARIANT_CONDITION_FAILED_KYWD
  | PARENT_FAILED_KYWD
 ;

nodeOutcomeKywd :
    SUCCESS_OUTCOME_KYWD
  | FAILURE_OUTCOME_KYWD
  | SKIPPED_OUTCOME_KYWD
;

nodeStateKywd : 
   WAITING_STATE_KYWD
   | EXECUTING_STATE_KYWD
   | FINISHING_STATE_KYWD
   | FAILING_STATE_KYWD
   | FINISHED_STATE_KYWD
   | ITERATION_ENDED_STATE_KYWD
   | INACTIVE_STATE_KYWD ;

isKnownExp :
    ^(IS_KNOWN_KYWD
      ( internalVariableReference
      | arrayReference
      | variable
      )
     )
 ;

literalValue : literalScalarValue | literalArrayValue ;

literalScalarValue : 
    booleanLiteral
  | INT
  | DOUBLE
  | STRING
  ;

literalArrayValue : ^(ARRAY_LITERAL literalScalarValue*) ;

booleanLiteral : TRUE_KYWD | FALSE_KYWD ;

realValue : DOUBLE | INT ;

lookupExpr : lookupOnChange | lookupNow | lookup ;

lookupOnChange :
    ^(LOOKUP_ON_CHANGE_KYWD lookupInvocation tolerance?)
 ;

lookup : 
    ^(LOOKUP_KYWD lookupInvocation tolerance?)
 ;

lookupNow :
    ^(LOOKUP_NOW_KYWD lookupInvocation)
 ;

lookupInvocation :
    ( stateName argumentList?
      { /* check arglist against declared params for stateName */ }
    | expression argumentList?
    )
 ;

stateName : ^(STATE_NAME NCNAME) ;

argumentList : ^(ARGUMENT_LIST expression*) ;

tolerance : realValue | variable ;

variable : 
    NCNAME
    { /* check if declared */ }
 ; 

binaryOp : 
    OR_KYWD | AND_KYWD
  | DEQUALS | NEQUALS
  | GREATER | GEQ | LESS | LEQ
  | PLUS | MINUS
  | ASTERISK | SLASH | PERCENT
  | MAX_KYWD | MIN_KYWD | MOD_KYWD
 ;

unaryOp :
 | NOT_KYWD
 | SQRT_KYWD | ABS_KYWD
 ;

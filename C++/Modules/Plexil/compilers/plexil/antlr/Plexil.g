// Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

grammar Plexil;

options
{ 
    output = AST;
    language = Java; 
    ASTLabelType = PlexilTreeNode;
//    TokenLabelType = PlexilToken;
    backtrack = false; /* for now */
    memoize = false;   /* for now */
	k = 2; /* for now */
}

tokens
{

//
// Keywords
//

COMMENT_KYWD = 'Comment';

// Kinds of declarations
COMMAND_KYWD = 'Command';
LOOKUP_KYWD =  'Lookup';

RETURNS_KYWD = 'Returns';

// Resources
RESOURCE_KYWD = 'Resource';
NAME_KYWD = 'Name';
UPPER_BOUND_KYWD = 'UpperBound';
LOWER_BOUND_KYWD = 'LowerBound';
RELEASE_AT_TERM_KYWD = 'ReleaseAtTermination';

PRIORITY_KYWD = 'Priority';

// Interface declarations
IN_KYWD = 'In';
IN_OUT_KYWD = 'InOut';

// Types
ANY_KYWD = 'Any';
BOOLEAN_KYWD = 'Boolean';
INTEGER_KYWD = 'Integer';
REAL_KYWD = 'Real';
STRING_KYWD = 'String';
DATE_KYWD = 'Date';
DURATION_KYWD = 'Duration';

// node types
UPDATE_KYWD = 'Update';
REQUEST_KYWD = 'Request';
LIBRARY_CALL_KYWD = 'LibraryCall';
LIBRARY_ACTION_KYWD = 'LibraryAction';

// node variables
STATE_KYWD = 'state';
OUTCOME_KYWD = 'outcome';
COMMAND_HANDLE_KYWD = 'command_handle';
FAILURE_KYWD = 'failure';

// node states
WAITING_STATE_KYWD = 'WAITING';
EXECUTING_STATE_KYWD = 'EXECUTING';
FINISHING_STATE_KYWD = 'FINISHING';
FAILING_STATE_KYWD = 'FAILING';
FINISHED_STATE_KYWD = 'FINISHED';
ITERATION_ENDED_STATE_KYWD = 'ITERATION_ENDED';
INACTIVE_STATE_KYWD = 'INACTIVE';

// node outcome values
SUCCESS_OUTCOME_KYWD = 'SUCCESS';
FAILURE_OUTCOME_KYWD = 'FAILURE';
SKIPPED_OUTCOME_KYWD = 'SKIPPED';
INTERRUPTED_OUTCOME_KYWD = 'INTERRUPTED';

// command handle values
COMMAND_ACCEPTED_KYWD     = 'COMMAND_ACCEPTED';
COMMAND_DENIED_KYWD       = 'COMMAND_DENIED';
COMMAND_FAILED_KYWD       = 'COMMAND_FAILED';
COMMAND_RCVD_KYWD         = 'COMMAND_RCVD_BY_SYSTEM';
COMMAND_SENT_KYWD         = 'COMMAND_SENT_TO_SYSTEM';
COMMAND_SUCCESS_KYWD      = 'COMMAND_SUCCESS';

// failure values
PRE_CONDITION_FAILED_KYWD = 'PRE_CONDITION_FAILED';
POST_CONDITION_FAILED_KYWD = 'POST_CONDITION_FAILED';
INVARIANT_CONDITION_FAILED_KYWD = 'INVARIANT_CONDITION_FAILED';
PARENT_FAILED_KYWD = 'PARENT_FAILED';
PARENT_EXITED_KYWD = 'PARENT_EXITED';
EXITED_KYWD = 'EXITED';

// Boolean values
TRUE_KYWD = 'true';
FALSE_KYWD = 'false';

// these are confusingly similar to condition synonyms
START_KYWD = 'START';
END_KYWD = 'END';

// *** DEPRECATED ***
LOOKUP_ON_CHANGE_KYWD = 'LookupOnChange';
LOOKUP_NOW_KYWD = 'LookupNow';

// Boolean expressions
XOR_KYWD = 'XOR';

// numerical expressions
ABS_KYWD = 'abs';
IS_KNOWN_KYWD = 'isKnown';
SQRT_KYWD = 'sqrt';
MAX_KYWD = 'max';
MIN_KYWD = 'min';
MOD_KYWD = 'mod';
CEIL_KYWD = 'ceil';
FLOOR_KYWD = 'floor';
ROUND_KYWD = 'round';
TRUNC_KYWD = 'trunc';
REAL_TO_INT_KYWD = 'real_to_int';

// String functions
STRLEN_KYWD = 'strlen';

// Array functions
ARRAY_SIZE_KYWD = 'arraySize';
ARRAY_MAX_SIZE_KYWD = 'arrayMaxSize';

// NodeRef directions
CHILD_KYWD = 'Child';
PARENT_KYWD = 'Parent';
SELF_KYWD = 'Self';
SIBLING_KYWD = 'Sibling';

// Node state predicates (Extended Plexil)
NODE_EXECUTING_KYWD = 'NodeExecuting';
NODE_FAILED_KYWD = 'NodeFailed';
NODE_FINISHED_KYWD = 'NodeFinished';
NODE_INACTIVE_KYWD = 'NodeInactive';
NODE_INVARIANT_FAILED_KYWD = 'NodeInvariantFailed';
NODE_ITERATION_ENDED_KYWD = 'NodeIterationEnded';
NODE_ITERATION_FAILED_KYWD = 'NodeIterationFailed';
NODE_ITERATION_SUCCEEDED_KYWD = 'NodeIterationSucceeded';
NODE_PARENT_FAILED_KYWD = 'NodeParentFailed';
NODE_POSTCONDITION_FAILED_KYWD = 'NodePostconditionFailed';
NODE_PRECONDITION_FAILED_KYWD = 'NodePreconditionFailed';
NODE_SKIPPED_KYWD = 'NodeSkipped';
NODE_SUCCEEDED_KYWD = 'NodeSucceeded';
NODE_WAITING_KYWD = 'NodeWaiting';

// Extended Plexil keywords
CONCURRENCE_KYWD = 'Concurrence';
ON_COMMAND_KYWD = 'OnCommand';
ON_MESSAGE_KYWD = 'OnMessage';
SYNCHRONOUS_COMMAND_KYWD = 'SynchronousCommand';
TIMEOUT_KYWD = 'Timeout';
TRY_KYWD = 'Try';
UNCHECKED_SEQUENCE_KYWD = 'UncheckedSequence';
SEQUENCE_KYWD = 'Sequence';
WAIT_KYWD = 'Wait';

ELSE_KYWD = 'else';
ELSEIF_KYWD = 'elseif';
ENDIF_KYWD = 'endif';
FOR_KYWD = 'for';
IF_KYWD = 'if';
WHILE_KYWD = 'while';

// Extended Plexil message passing
MESSAGE_RECEIVED_KYWD = 'MessageReceived';

LBRACKET = '[';
RBRACKET = ']';
LBRACE = '{';
RBRACE = '}';
LPAREN = '(';
RPAREN = ')';
BAR = '|';
LESS = '<' ;
GREATER = '>' ;
LEQ = '<=' ;
GEQ = '>=' ;
COLON = ':';
DEQUALS = '==';
NEQUALS = '!=';
EQUALS = '=';
ASTERISK = '*';
SLASH = '/';
PERCENT = '%';
HASHPAREN = '#(';
ELLIPSIS = '...';

SEMICOLON = ';';
COMMA = ',';

// Numeric types (see lexer grammar)
INT;
DOUBLE;

//
// "Imaginary" tokens
//

PLEXIL; // top node of parse tree

ACTION;
ALIAS;
ALIASES;
ARGUMENT_LIST;
ARRAY_TYPE;
ARRAY_LITERAL;
ARRAY_REF;
ARRAY_VARIABLE_DECLARATION;
ASSIGNMENT;
BLOCK;
COMMAND;
CONCAT;
DATE_LITERAL;
DURATION_LITERAL;
GLOBAL_DECLARATIONS;
NODE_ID;
NODE_TIMEPOINT_VALUE;
PARAMETERS;
SEQUENCE; // used in tree parser
STATE_NAME;
STRING_COMPARISON;
VARIABLE_DECLARATION;
VARIABLE_DECLARATIONS;
NEG_INT;
NEG_DOUBLE;
}

//
// 'scope' decls go here
//

@header
{
package plexil;

import plexil.*;
}

@lexer::header 
{ 
package plexil;
}

@members
{
    GlobalContext m_globalContext = new GlobalContext();
    NodeContext m_context = m_globalContext;
    Stack<String> m_paraphrases = new Stack<String>();

	// Overrides to enhance error reporting
	public String getErrorMessage(RecognitionException e,
		   		  				  String[] tokenNames)
	{
	  String msg = super.getErrorMessage(e, tokenNames);
	  if (m_paraphrases.size() > 0) {
		 msg = msg + " " + m_paraphrases.peek();
	  }
      return msg;
	}

	public void displayRecognitionError(String[] tokenNames,
										RecognitionException e)
	{
	  CompilerState.getCompilerState().addDiagnostic((PlexilTreeNode) e.node,
													 getErrorHeader(e) + " " + getErrorMessage(e, tokenNames),
													 Severity.ERROR);
	}

}

///////////////////////////
// *** BEGIN GRAMMAR *** //
///////////////////////////

plexilPlan
@init { m_paraphrases.push("in plan"); }
@after { m_paraphrases.pop(); }
 :
    declarations? action EOF
    -> ^(PLEXIL<PlexilPlanNode> declarations? action)
 ;


declarations : 
    declaration+
    -> ^(GLOBAL_DECLARATIONS<GlobalDeclarationsNode> declaration+) ;

declaration
options { k=5; } // handles initial ambiguity for array typed decls
 :
    commandDeclaration
  | lookupDeclaration
  | libraryActionDeclaration
 ;

// should generate ^(COMMAND_KYWD NCNAME (RETURNS_KYWD paramSpec)? (paramsSpec)?)
// return type may be null!
// *** TODO: add resource lists ***

commandDeclaration
@init { m_paraphrases.push("in command declaration"); }
@after { m_paraphrases.pop(); }
 :
    ( 
      // no-return-value variant
      ( COMMAND_KYWD NCNAME paramsSpec? SEMICOLON
        -> ^(COMMAND_KYWD<CommandDeclarationNode> NCNAME paramsSpec?)
	  )
    |
      // return value variant
      ( returnType COMMAND_KYWD NCNAME paramsSpec? SEMICOLON
         -> ^(COMMAND_KYWD<CommandDeclarationNode> NCNAME paramsSpec? returnType)
      )
    )
  ;

// should generate #(LOOKUP_KYWD stateName (RETURNS_KYWD paramSpec)? (paramsSpec)*)

lookupDeclaration
@init { m_paraphrases.push("in lookup declaration"); }
@after { m_paraphrases.pop(); }
 : 
    // old style single return syntax
    returnType LOOKUP_KYWD NCNAME paramsSpec? SEMICOLON
    -> ^(LOOKUP_KYWD<LookupDeclarationNode> NCNAME returnType paramsSpec?)
  ;

paramsSpec :
    LPAREN paramsSpecGuts? RPAREN
  -> ^(PARAMETERS paramsSpecGuts?)
 ;

paramsSpecGuts :
      ( paramSpec ( COMMA! paramSpec )* ( COMMA! ELLIPSIS )? )
      | ELLIPSIS
 ;

paramSpec
options { k = 2; }
 : 
    (baseTypeName LBRACKET) =>
      baseTypeName LBRACKET INT RBRACKET -> ^(ARRAY_TYPE baseTypeName INT)
  | (baseTypeName NCNAME LBRACKET) =>
      baseTypeName NCNAME LBRACKET INT RBRACKET -> ^(ARRAY_TYPE baseTypeName INT NCNAME)
  | paramTypeName^ NCNAME?
 ;

paramTypeName
    : ANY_KYWD
    | BOOLEAN_KYWD
    | INTEGER_KYWD
    | REAL_KYWD
    | STRING_KYWD
    | DATE_KYWD
    | DURATION_KYWD
    ;

returnType :
    returnTypeSpec -> ^(RETURNS_KYWD returnTypeSpec)
 ;

returnTypeSpec :
    (baseTypeName LBRACKET) =>
      baseTypeName LBRACKET INT RBRACKET -> ^(ARRAY_TYPE baseTypeName INT)
  | baseTypeName
 ;

baseTypeName :
    BOOLEAN_KYWD
  | INTEGER_KYWD
  | REAL_KYWD
  | STRING_KYWD
  | DATE_KYWD
  | DURATION_KYWD
  ;

libraryActionDeclaration
@init { m_paraphrases.push("in library action declaration"); }
@after { m_paraphrases.pop(); }
 :
    LIBRARY_ACTION_KYWD^ NCNAME libraryInterfaceSpec? SEMICOLON!
;

libraryInterfaceSpec
@init { m_paraphrases.push("in library action interface declaration"); }
@after { m_paraphrases.pop(); }
 :
    LPAREN ( libraryParamSpec ( COMMA libraryParamSpec )* )? RPAREN
    -> ^(PARAMETERS libraryParamSpec*)
 ;

libraryParamSpec :
 ( IN_KYWD^ | IN_OUT_KYWD^ )
 baseTypeName
 ( (NCNAME LBRACKET) => NCNAME LBRACKET! INT RBRACKET! 
 | NCNAME
 )
 ;

//
// Actions
// 

action
@init { m_paraphrases.push("in action"); }
@after { m_paraphrases.pop(); }
 :
    (actionId=NCNAME COLON)?
	rest=baseAction
    -> ^(ACTION $actionId? $rest)
 ;

baseAction : compoundAction | simpleAction | block ; 

compoundAction : forAction | ifAction | onCommandAction | onMessageAction | whileAction ;

// One-liner actions
simpleAction :
    (NCNAME (LBRACKET | EQUALS)) => assignment
  | ((NCNAME LPAREN) | LPAREN) => commandInvocation SEMICOLON!
  | libraryCall
  | request
  | update
  | synchCmd
  | waitBuiltin
 ;

forAction
@init { m_paraphrases.push("in \"for\" statement"); }
@after { m_paraphrases.pop(); }
 :
    FOR_KYWD 
    LPAREN baseTypeName NCNAME EQUALS loopvarinit=expression
    SEMICOLON endtest=expression
    SEMICOLON loopvarupdate=expression
    RPAREN
    action
    -> ^(FOR_KYWD ^(VARIABLE_DECLARATION baseTypeName NCNAME $loopvarinit) $endtest $loopvarupdate action)
 ;

ifAction
@init { m_paraphrases.push("in \"if\" statement"); }
@after { m_paraphrases.pop(); }
 :
    IF_KYWD^ expression action
    (ELSEIF_KYWD! expression action)*
    (ELSE_KYWD! action)?
    ENDIF_KYWD!
    SEMICOLON!?
 ;

onCommandAction
@init { m_paraphrases.push("in \"OnCommand\" statement"); }
@after { m_paraphrases.pop(); }
 : 
    ON_COMMAND_KYWD^ expression paramsSpec? action
 ;

onMessageAction
@init { m_paraphrases.push("in \"OnMessage\" statement"); }
@after { m_paraphrases.pop(); }
 :
    ON_MESSAGE_KYWD<OnMessageNode>^ expression action
 ;

whileAction
@init { m_paraphrases.push("in \"while\" statement"); }
@after { m_paraphrases.pop(); }
 :
    WHILE_KYWD^ expression action
 ;

synchCmd
@init { m_paraphrases.push("in \"SynchronousCommand\" statement"); }
@after { m_paraphrases.pop(); }
 :
    SYNCHRONOUS_COMMAND_KYWD^
    ( commandWithAssignment | commandInvocation )
	( TIMEOUT_KYWD! expression ( COMMA! expression )? )?
	SEMICOLON!
 ;

waitBuiltin
@init { m_paraphrases.push("in \"Wait\" statement"); }
@after { m_paraphrases.pop(); }
 :
	WAIT_KYWD^ expression (COMMA! (variable|INT|DOUBLE))? SEMICOLON!
 ;

// *** N.B. The supported schema does not require the strict sequencing of
// the elements inside a block, nor does the XML parser.

block
@init { m_paraphrases.push("in block"); }
@after { m_paraphrases.pop(); }
 : 
    (variant=sequenceVariantKywd LBRACE -> $variant
     | LBRACE -> BLOCK)
    comment?
    nodeDeclaration*
    nodeAttribute*
    action*
    RBRACE
	-> ^($block comment? nodeDeclaration* nodeAttribute* action*)
;

sequenceVariantKywd : 
    CONCURRENCE_KYWD
  | SEQUENCE_KYWD
  | UNCHECKED_SEQUENCE_KYWD
  | TRY_KYWD
 ;

comment : COMMENT_KYWD^ STRING SEMICOLON! ;

nodeDeclaration :
    interfaceDeclaration
  | variableDeclaration;

nodeAttribute :
    nodeCondition
  | priority
  | resource ;

nodeCondition
@init { m_paraphrases.push("in condition"); }
@after { m_paraphrases.pop(); }
 :
    conditionKywd^ expression SEMICOLON! ;

conditionKywd :
    END_CONDITION_KYWD
  | EXIT_CONDITION_KYWD
  | INVARIANT_CONDITION_KYWD
  | POST_CONDITION_KYWD
  | PRE_CONDITION_KYWD
  | REPEAT_CONDITION_KYWD
  | SKIP_CONDITION_KYWD
  | START_CONDITION_KYWD
 ;

resource
@init { m_paraphrases.push("in resource"); }
@after { m_paraphrases.pop(); }
 :
    RESOURCE_KYWD^ NAME_KYWD! EQUALS! expression
        ( COMMA!
          ( LOWER_BOUND_KYWD EQUALS! expression
          | UPPER_BOUND_KYWD EQUALS! expression
  		  | RELEASE_AT_TERM_KYWD EQUALS! expression
 		  | PRIORITY_KYWD EQUALS! pe=expression
          )
        )*
        SEMICOLON!
 ;

priority
@init { m_paraphrases.push("in priority"); }
@after { m_paraphrases.pop(); }
 : PRIORITY_KYWD<PriorityNode>^ INT SEMICOLON! ;

interfaceDeclaration : in | inOut ;

// The rule is apparently that all variables declared in a node 
// are automatically in/out interface vars for all of that node's descendants.
// So this may only be of use in library node definitions.

in
@init { m_paraphrases.push("in \"In\" declaration"); }
@after { m_paraphrases.pop(); }
 : 
    IN_KYWD^ 
      ( (NCNAME (COMMA! NCNAME)*)
	  | interfaceDeclarations
      ) 
    SEMICOLON!
  ;

inOut
@init { m_paraphrases.push("in \"InOut\" declaration"); }
@after { m_paraphrases.pop(); }
 :
    IN_OUT_KYWD^
      ( (NCNAME (COMMA! NCNAME)*)
	  | interfaceDeclarations
      ) 
    SEMICOLON!
  ;

interfaceDeclarations :
    tn=baseTypeName!
    ( (NCNAME LBRACKET) => arrayVariableDecl[$tn.start] 
    | scalarVariableDecl[$tn.start]
    )
	( COMMA!
      ( (NCNAME LBRACKET) => arrayVariableDecl[$tn.start] 
      | scalarVariableDecl[$tn.start]
      )
	)*
  ;

variable : NCNAME<VariableNode> ;

variableDeclaration
@init { m_paraphrases.push("in variable declaration"); }
@after { m_paraphrases.pop(); }
 : 
    tn=baseTypeName
    ( (NCNAME LBRACKET) => arrayVariableDecl[$tn.start] 
    | scalarVariableDecl[$tn.start]
    )
    ( COMMA 
	  ( (NCNAME LBRACKET) => arrayVariableDecl[$tn.start] 
	  | scalarVariableDecl[$tn.start]
	  )
	)*
    SEMICOLON
    -> ^(VARIABLE_DECLARATIONS scalarVariableDecl* arrayVariableDecl*)
  ;

scalarVariableDecl[Token typeName] :
    NCNAME ( EQUALS literalScalarValue )?
    -> ^(VARIABLE_DECLARATION {new PlexilTreeNode($typeName)} NCNAME literalScalarValue?)
  ;

arrayVariableDecl[Token typeName] :
    NCNAME LBRACKET INT RBRACKET ( EQUALS literalValue ) ?
	-> ^(ARRAY_VARIABLE_DECLARATION {new PlexilTreeNode($typeName)} NCNAME INT literalValue?)
  ;

literalScalarValue : 
    booleanLiteral | INT | DOUBLE | STRING | unaryMinus |
    dateLiteral | durationLiteral ;

literalArrayValue :
    HASHPAREN literalScalarValue* RPAREN
    -> ^(ARRAY_LITERAL literalScalarValue*)
  ;

literalValue : literalScalarValue | literalArrayValue ;

booleanLiteral : TRUE_KYWD | FALSE_KYWD ;

realValue : DOUBLE | INT ;

// TODO: extend to other expressions

lookupArrayReference :
    lookup LBRACKET expression RBRACKET
    -> ^(ARRAY_REF lookup expression)
  ;

simpleArrayReference :
    variable LBRACKET expression RBRACKET
    -> ^(ARRAY_REF variable expression)
  ;

commandInvocation
@init { m_paraphrases.push("in command"); }
@after { m_paraphrases.pop(); }
 :
    ( NCNAME -> ^(COMMAND_KYWD NCNAME)
    | LPAREN expression RPAREN -> expression
    )
    LPAREN argumentList? RPAREN
    -> ^(COMMAND<CommandNode> $commandInvocation argumentList?)
 ;

// Used only in synchCmd
commandWithAssignment
@init { m_paraphrases.push("in command"); }
@after { m_paraphrases.pop(); }
 :
    assignmentLHS EQUALS commandInvocation
	-> ^(ASSIGNMENT assignmentLHS commandInvocation)    
 ;

argumentList : 
    argument (COMMA argument)*
    -> ^(ARGUMENT_LIST argument*)
  ;

argument : expression ;

assignment
@init { m_paraphrases.push("in assignment statement"); }
@after { m_paraphrases.pop(); }
 :
    assignmentLHS EQUALS assignmentRHS SEMICOLON
    -> ^(ASSIGNMENT assignmentLHS assignmentRHS)
 ;

assignmentLHS : 
    ( NCNAME LBRACKET )
    => simpleArrayReference
  | 
    variable
;

// *** Note ambiguity in RHS ***
assignmentRHS :
   (NCNAME LPAREN) => commandInvocation
 |
   (LPAREN expression RPAREN LPAREN) => commandInvocation
 |
   expression
 ;

//
// Update nodes
//

update
@init { m_paraphrases.push("in \"Update\" statement"); }
@after { m_paraphrases.pop(); }
 :
    UPDATE_KYWD^ ( pair ( COMMA! pair )* )? SEMICOLON! ;

//
// Request nodes
//
// Note that the node name need not be known
//

request
@init { m_paraphrases.push("in \"Request\" statement"); }
@after { m_paraphrases.pop(); }
 : REQUEST_KYWD^ NCNAME ( pair ( COMMA! pair )* )? SEMICOLON! ;

// common to both update and request nodes

pair : NCNAME EQUALS! expression ;

//
// Library Call nodes
//

libraryCall
@init { m_paraphrases.push("in library action call"); }
@after { m_paraphrases.pop(); }
 :
  LIBRARY_CALL_KYWD^ libraryNodeIdRef ( aliasSpecs )? SEMICOLON! ;

libraryNodeIdRef : NCNAME ;

aliasSpecs :
  LPAREN ( aliasSpec ( COMMA aliasSpec )* )? RPAREN
  -> ^(ALIASES aliasSpec*)
  ;

aliasSpec :
  NCNAME EQUALS expression
        -> ^(ALIAS NCNAME expression)
 ;

nodeParameterName : NCNAME ;

///////////////////
//  EXPRESSIONS  //
///////////////////

//
// Implement operator precedence as in C(++)/Java
// Precedence taken from Harbison & Steele, 1995
//

expression
@init { m_paraphrases.push("in expression"); }
@after { m_paraphrases.pop(); }
 : logicalOr ;

// 1 sequential evaluation (,) - not implemented here
// 2 assignment (=, +=, and friends) - not implemented here
// 3 conditional (? :) - not in the Plexil language

// 4 logical OR

logicalOr :
   ( o1=logicalXOR -> $o1 )
   ( OR_KYWD on=logicalXOR
     -> ^(OR_KYWD $logicalOr $on)
   )*
 ;

// 4.5 logical XOR (not in C)

logicalXOR :
    ( x1=logicalAnd -> $x1 )
    ( XOR_KYWD xn=logicalAnd
      -> ^(XOR_KYWD $logicalXOR $xn)
    )*
 ;

// 5 logical AND

logicalAnd : 
   ( a1=equality -> $a1 )
   ( AND_KYWD an=equality
     -> ^(AND_KYWD $logicalAnd $an)
   )*
 ;

// 6 bitwise OR - not in the Plexil language
// 7 bitwise XOR - not in the Plexil language
// 8 bitwise AND - not in the Plexil language

// 9 equality/inequality

equality :
    ( e1=relational -> $e1)
    ( equalityOp en=relational 
	  -> ^(equalityOp $equality $en)
	)?
 ;

equalityOp :
    DEQUALS
  | NEQUALS
 ;

// 10 relational (<, <=, >, >=)

relational :
    ( a1=additive -> $a1 )
	( relationalOp an=additive
	  -> ^(relationalOp $relational $an)
    )?
 ;

relationalOp : 
    GREATER
  | GEQ
  | LESS
  | LEQ
 ;

// 11 left/right shift - not in the Plexil language

// 12 additive (+, -)

additive :
    ( m1=multiplicative -> $m1 )
    ( addOp mn=multiplicative
	  -> ^(addOp $additive $mn)
	)* 
 ;

addOp : 
    PLUS
  | MINUS
 ;

// 13 multiplicative (*, /, %, mod)

multiplicative :
    ( u1=unary -> $u1 )
    ( multOp un=unary
      -> ^(multOp $multiplicative $un)
    )*
 ;

multOp :
    ASTERISK
  | SLASH
  | PERCENT
  | MOD_KYWD
 ;

// 14 casts - not in the Plexil language

// 15 indirection (*), address of(&), sizeof, preincrement/decrement (++x, --x)
//    - not in the Plexil language

// 15 prefix unary - arithmetic negation, plus, logical not

unary : unaryMinus
      | unaryOp^ quantity
      | quantity
      ;

unaryOp : NOT_KYWD ;

unaryMinus : (MINUS i=INT) -> ^(NEG_INT $i)
           | (MINUS d=DOUBLE) -> ^(NEG_DOUBLE $d)
           ;

dateLiteral : (DATE_KYWD LPAREN s=STRING RPAREN) -> ^(DATE_LITERAL $s) ;

durationLiteral : (DURATION_KYWD LPAREN s=STRING RPAREN) -> ^(DURATION_LITERAL $s) ;


// 17 postincrement/decrement (x++, x--), indirect selection (->), function call
// 	  - not in the Plexil language

// 17 Subscripting, direct selection, simple tokens

quantity :
    LPAREN! expression RPAREN!
  | BAR expression BAR -> ^(ABS_KYWD expression)
  | oneArgFn^ LPAREN! expression RPAREN!
  | twoArgFn^ LPAREN! expression COMMA! expression RPAREN!
  | isKnownExp
  | (lookupExpr LBRACKET) => lookupArrayReference
  | lookupExpr
  | messageReceivedExp
  | nodeStatePredicateExp
  | ( (NCNAME | SELF_KYWD | PARENT_KYWD) PERIOD COMMAND_HANDLE_KYWD) => nodeCommandHandleVariable
  | ( (NCNAME | SELF_KYWD | PARENT_KYWD) PERIOD FAILURE_KYWD) => nodeFailureVariable
  | ( (NCNAME | SELF_KYWD | PARENT_KYWD) PERIOD OUTCOME_KYWD) => nodeOutcomeVariable
  | ( (NCNAME | SELF_KYWD | PARENT_KYWD) PERIOD STATE_KYWD) => nodeStateVariable
  | ( (NCNAME | SELF_KYWD | PARENT_KYWD) PERIOD nodeStateKywd) => nodeTimepointValue
  | (NCNAME LBRACKET) => simpleArrayReference
  | variable
// These are for the nodeRef variants
  | ( (CHILD_KYWD | SIBLING_KYWD) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD) => nodeCommandHandleVariable
  | ( (CHILD_KYWD | SIBLING_KYWD) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD) => nodeFailureVariable
  | ( (CHILD_KYWD | SIBLING_KYWD) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD) => nodeOutcomeVariable
  | ( (CHILD_KYWD | SIBLING_KYWD) LPAREN NCNAME RPAREN PERIOD STATE_KYWD) => nodeStateVariable
  | ( (CHILD_KYWD | SIBLING_KYWD) LPAREN NCNAME RPAREN PERIOD nodeStateKywd) => nodeTimepointValue
  | literalValue
  | nodeCommandHandleKywd
  | nodeFailureKywd
  | nodeStateKywd
  | nodeOutcomeKywd
 ;

// can add more later
oneArgFn : 
    SQRT_KYWD
  | ABS_KYWD
  | CEIL_KYWD
  | FLOOR_KYWD
  | ROUND_KYWD
  | TRUNC_KYWD
  | REAL_TO_INT_KYWD
  | STRLEN_KYWD
  | ARRAY_SIZE_KYWD
  | ARRAY_MAX_SIZE_KYWD
 ;

twoArgFn :
    MAX_KYWD
  | MIN_KYWD;

isKnownExp :
   IS_KNOWN_KYWD<IsKnownNode>^ LPAREN! quantity RPAREN! ;

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

nodeStatePredicateExp : nodeStatePredicate^ LPAREN! nodeReference RPAREN! ;

nodeStateKywd : 
     EXECUTING_STATE_KYWD
   | FAILING_STATE_KYWD
   | FINISHED_STATE_KYWD
   | FINISHING_STATE_KYWD
   | INACTIVE_STATE_KYWD
   | ITERATION_ENDED_STATE_KYWD
   | WAITING_STATE_KYWD
 ;

messageReceivedExp :
  MESSAGE_RECEIVED_KYWD^ LPAREN! STRING RPAREN!
 ;

nodeState : nodeStateVariable | nodeStateKywd ;

nodeStateVariable : nodeReference PERIOD! STATE_KYWD<NodeVariableNode>^ ;

nodeOutcome : nodeOutcomeVariable | nodeOutcomeKywd ;

nodeOutcomeVariable : nodeReference PERIOD! OUTCOME_KYWD<NodeVariableNode>^ ;

nodeOutcomeKywd :
    SUCCESS_OUTCOME_KYWD
  | FAILURE_OUTCOME_KYWD
  | SKIPPED_OUTCOME_KYWD
  | INTERRUPTED_OUTCOME_KYWD
;

nodeCommandHandle : nodeCommandHandleVariable | nodeCommandHandleKywd ;

nodeCommandHandleVariable : nodeReference PERIOD! COMMAND_HANDLE_KYWD<NodeVariableNode>^ ;

nodeCommandHandleKywd :
    COMMAND_ACCEPTED_KYWD
  | COMMAND_DENIED_KYWD
  | COMMAND_FAILED_KYWD
  | COMMAND_RCVD_KYWD
  | COMMAND_SENT_KYWD
  | COMMAND_SUCCESS_KYWD
 ;

nodeFailure : nodeFailureVariable | nodeFailureKywd ;

nodeFailureVariable : nodeReference PERIOD! FAILURE_KYWD<NodeVariableNode>^ ;

nodeFailureKywd :
    PRE_CONDITION_FAILED_KYWD
  | POST_CONDITION_FAILED_KYWD
  | INVARIANT_CONDITION_FAILED_KYWD
  | PARENT_FAILED_KYWD
  | PARENT_EXITED_KYWD
  | EXITED_KYWD
 ;

nodeTimepointValue :
   nodeReference PERIOD nodeStateKywd PERIOD timepoint
   -> ^(NODE_TIMEPOINT_VALUE nodeReference nodeStateKywd timepoint)
 ;

timepoint : START_KYWD | END_KYWD ;

nodeReference : 
    nodeId
  | CHILD_KYWD^ LPAREN! NCNAME RPAREN!
  | SIBLING_KYWD^ LPAREN! NCNAME RPAREN!
 ;

nodeId : 
    SELF_KYWD
  | PARENT_KYWD
  | NCNAME 
 ;

//
// Lookups
//

lookupExpr : lookupOnChange | lookupNow | lookup ;

// should produce an AST of the form
// #(LOOKUP_ON_CHANGE_KYWD lookupInvocation (tolerance)? )
// N.b. tolerance is optional

lookupOnChange
@init { m_paraphrases.push("in \"LookupOnChange\" expression"); }
@after { m_paraphrases.pop(); }
 :
       LOOKUP_ON_CHANGE_KYWD^ LPAREN! lookupInvocation (COMMA! tolerance)? RPAREN!
;

tolerance : realValue | durationLiteral | variable ;

// should produce an AST of the form
// #(LOOKUP_NOW_KYWD stateNameExp (argumentList)? )

lookupNow
@init { m_paraphrases.push("in \"LookupNow\" expression"); }
@after { m_paraphrases.pop(); }
 :
    LOOKUP_NOW_KYWD^ LPAREN! lookupInvocation RPAREN! ;

// new generic lookup
// should produce an AST of the form
// #(LOOKUP_KYWD lookupInvocation (tolerance)? )
// N.b. tolerance is optional

lookup
@init { m_paraphrases.push("in \"Lookup\" expression"); }
@after { m_paraphrases.pop(); }
 :
    LOOKUP_KYWD^ LPAREN! lookupInvocation (COMMA! tolerance)? RPAREN!
  ;

lookupInvocation :
  ( stateName
    |
    ( LPAREN! stateNameExp RPAREN! )
  )
  ( LPAREN! (argumentList)? RPAREN! )?
 ;

stateName :
    NCNAME -> ^(STATE_NAME NCNAME)
 ;

stateNameExp : expression ;


//
// ****************************************
// ********* BEGIN LEXER GRAMMAR **********
// ****************************************
//

//
// Tokens with synonyms
//

// Conditions
START_CONDITION_KYWD : 'StartCondition' | 'Start' ;
REPEAT_CONDITION_KYWD : 'RepeatCondition' | 'Repeat' ;
SKIP_CONDITION_KYWD : 'SkipCondition' | 'Skip' ;
PRE_CONDITION_KYWD : 'PreCondition' | 'Pre' ;
POST_CONDITION_KYWD : 'PostCondition' | 'Post';
INVARIANT_CONDITION_KYWD : 'InvariantCondition' | 'Invariant';
END_CONDITION_KYWD : 'EndCondition' | 'End' ;
EXIT_CONDITION_KYWD : 'ExitCondition' | 'Exit' ;

// Logical operators
AND_KYWD : 'AND' | '&&' ;
OR_KYWD : 'OR' | '||' ;
NOT_KYWD : 'NOT' | '!' ;

STRING: '"' (Escape|~('"'|'\\'))* '"'
      | '\'' (Escape|~('\''|'\\'))* '\''
      ;
fragment Escape:
  '\\'
  ('n' | 't' | 'b' | 'f' |'\n' | '\r' | '"' | '\'' | '\\' | UnicodeEscape | OctalEscape);

fragment UnicodeEscape: 
  'u' HexDigit HexDigit HexDigit HexDigit;

fragment OctalEscape: 
  QuadDigit ( OctalDigit OctalDigit? )?
  | OctalDigit OctalDigit? ;

fragment QuadDigit: ('0'..'3') ;
fragment OctalDigit: ('0'..'7') ;
fragment Digit: ('0'..'9') ;
fragment HexDigit: (Digit|'A'..'F'|'a'..'f') ;

// a few fragment methods to assist in matching floating point numbers
fragment Exponent:  ('e'|'E') (PLUS | MINUS)? (Digit)+ ;

PLUS : '+';
MINUS : '-';
PERIOD : '.';

// the following returns tokens DOUBLE, INT

// *** Chuck's notes:
// prefixes - 
//  +/- is always decimal, can be either int or double
//  0x is always hexadecimal (must not be signed, no decimal point)
//  0o is always octal (must not be signed, no decimal point)
//  0b is always binary (must not be signed, no decimal point)
//  . is always double (decimal)

// *** TODO: set base for non-decimal INTs ***

INT_OR_DOUBLE
@init { int base = 10; } :
   (
    ( '0' { $type = INT; } // special case for just '0'
     ( ( ('x'|'X') { base = 16; } // hex
         (HexDigit)+
         )
       |
       ( ('o'|'O') { base = 8; } // octal
         (OctalDigit)+
       )
       |
       ( ('b'|'B') { base = 2; } // binary
         ('0'|'1')+   
       )
       |
       ( PERIOD { $type = DOUBLE; }
         Digit* Exponent?
       )
       |
       ( Exponent { $type = DOUBLE; } )
     )?
   )
   | 
   ( PERIOD ) =>
   ( PERIOD { $type = DOUBLE; }
     Digit+ Exponent? 
   )
   |
   ( Digit+ { $type = INT; } 
     (PERIOD (Digit)* { $type = DOUBLE; } )?
     (Exponent { $type = DOUBLE; } )?
   )
   )
 ;

// XML spec says:
// NCName ::= (Letter | '_') (NCNameChar)*
// NCNameChar ::= Letter | Digit | '.' | '-' | '_' | CombiningChar | Extender
// (see http://www.w3.org/TR/xml/ for definitions of Letter, Digit, CombiningChar, and Extender)

// *** ensure this does not conflict with keywords! ***

NCNAME :

// *** KMD: disallowing dots for now, as this breaks a number of rules above.
// Figure out a way to support them again.  Consider these cases:
//   foo.bar
//   foo.start
//   foo.bar.start
//   foo.start.start
//   start
//  (Letter|'_') (Letter|Digit|PERIOD|'_')*
  (Letter|'_') (Letter|Digit|'_')*
  ;

fragment Letter : 'a'..'z'|'A'..'Z' ;

// Whitespace -- ignored
WS :
    ( ' ' | '\t' | '\f' | '\n' | '\r' )+
    { $channel = HIDDEN; }
  ;

// Single-line comments
SL_COMMENT :
    ( '//' ~('\r' | '\n')* ('\r\n' | '\r' | '\n')
      { $channel = HIDDEN; }
	| '//' ~('\r' | '\n')*
      { $channel = HIDDEN; }
    )
  ;

// multiple-line comments
ML_COMMENT :
    '/*' ( options {greedy=false;} : . )* '*/'
    { $channel=HIDDEN; }
  ;

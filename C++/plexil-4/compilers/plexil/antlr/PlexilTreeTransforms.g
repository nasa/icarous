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

// Tree-rewriting rules for Plexil AST
// See https://theantlrguy.atlassian.net/wiki/display/ANTLR3/Tree+pattern+matching

tree grammar PlexilTreeTransforms;

options
{
    language = Java;
    tokenVocab = Plexil;
    ASTLabelType = PlexilTreeNode;
    output = AST;
    filter = true;
	rewrite = true;
    backtrack = true;
    k = 2; // this is essential!
}

@header
{
package plexil;

import plexil.*;
}

@members
{
    GlobalContext m_globalContext = GlobalContext.getGlobalContext();
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

    // Debugging aid.
//    public void reportTransformation(Object oldTree, Object newTree)
//    {
//        System.out.println("Optimizer: transforming\n  "
//                           + ((PlexilTreeNode) oldTree).toStringTree()
//                           + "\n to\n  "
//                           + ((PlexilTreeNode) newTree).toStringTree());
//    }

}

////////////////////////////////////////
// *** BEGIN TREE TRANSFORMATIONS *** //
////////////////////////////////////////

topdown:
        enterContext
    ;

bottomup:
        trivialReduction    
    |   booleanIdentityElision
    |   integerIdentityElision
    |   realIdentityElision
    |   booleanEqualityNegation
    |   rightAssociativeReduction
    |   leftAssociativeReduction
    |   flattenTrivialBlocks
    |   flattenTrivialSequences
    |   flattenTrivialUncheckedSequences
    |   flattenTrivialConcurrences
    |   exitContext
    ;

//
// Top-down transforms
//

enterContext:
        bindingContextNode
        {
            m_context = $bindingContextNode.start.getContext();
//            System.out.println("Enter context " + m_context.getNodeName()); // DEBUG
        } ;

//
// Bottom-up transforms
//

exitContext:
        bindingContextNode
        {
//            System.out.println("Exit context " + m_context.getNodeName()); // DEBUG
            m_context = m_context.getParentContext();
//            System.out.println("Restore context " + m_context.getNodeName()); // DEBUG
        } ;

//
// Expression simplification
//

// Combine instances of same binary operator when appropriate

rightAssociativeReduction:
        ^(AND_KYWD args+=. ^(AND_KYWD (args+=.)+) ) -> ^(AND_KYWD $args+)
    |   ^(OR_KYWD args+=. ^(OR_KYWD (args+=.)+))   -> ^(OR_KYWD $args+)
    |   ^(PLUS args+=. ^(PLUS (args+=.)+))         -> ^(PLUS $args+)
    |   ^(ASTERISK args+=. ^(ASTERISK (args+=.)+)) -> ^(ASTERISK $args+)
    |   ^(MAX_KYWD args+=. ^(MAX_KYWD (args+=.)+)) -> ^(MAX_KYWD $args+)
    |   ^(MIN_KYWD args+=. ^(MIN_KYWD (args+=.)+)) -> ^(MIN_KYWD $args+)
    ;

leftAssociativeReduction:
        ^(AND_KYWD ^(AND_KYWD (args+=.)+) (args+=.)+ )   -> ^(AND_KYWD $args+)
    |   ^(OR_KYWD ^(OR_KYWD (args+=.)+) (args+=.)+ )     -> ^(OR_KYWD $args+)
    |   ^(PLUS ^(PLUS (args+=.)+) (args+=.)+ )           -> ^(PLUS $args+)
    |   ^(MINUS ^(MINUS args+=. (args+=.)+) (args+=.)+ ) -> ^(MINUS $args+)
    |   ^(ASTERISK ^(ASTERISK (args+=.)+) (args+=.)+ )   -> ^(ASTERISK $args+)
    |   ^(MAX_KYWD ^(MAX_KYWD (args+=.)+) (args+=.)+ )   -> ^(MAX_KYWD $args+)
    |   ^(MIN_KYWD ^(MIN_KYWD (args+=.)+) (args+=.)+ ) -> ^(MIN_KYWD $args+)
    ;

// I don't think the grammar generates these, but the reductions above might.
trivialReduction:
        ^(AND_KYWD arg=.) -> $arg
    |   ^(OR_KYWD arg=.) -> $arg
    |   ^(PLUS arg=.) -> $arg
    |   ^(MINUS arg=.) -> $arg
    |   ^(ASTERISK arg=.) -> $arg
    |   ^(SLASH arg=.) -> $arg
    |   ^(MAX_KYWD arg=.) -> $arg
    |   ^(MIN_KYWD arg=.) -> $arg
    ;

//
// Constant propagation
//

booleanIdentityElision:
        ^(AND_KYWD TRUE_KYWD arg=.) -> $arg
    |   ^(AND_KYWD arg=. TRUE_KYWD) -> $arg
    |   ^(OR_KYWD FALSE_KYWD arg=.) -> $arg
    |   ^(OR_KYWD arg=. FALSE_KYWD) -> $arg
    |   ^(DEQUALS TRUE_KYWD arg=.) -> $arg
    |   ^(DEQUALS arg=. TRUE_KYWD) -> $arg
    |   ^(NEQUALS FALSE_KYWD arg=.) -> $arg
    |   ^(NEQUALS arg=. FALSE_KYWD) -> $arg
    ;
    
integerIdentityElision:
        ^(PLUS i=INT arg=. {Integer.valueOf($i.text) == 0}?) -> $arg
    |   ^(PLUS arg=. i=INT {Integer.valueOf($i.text) == 0}?) -> $arg
    |   ^(MINUS arg=. i=INT {Integer.valueOf($i.text) == 0}?) -> $arg
    |   ^(ASTERISK i=INT arg=. {Integer.valueOf($i.text) == 1}?) -> $arg
    |   ^(ASTERISK arg=. i=INT {Integer.valueOf($i.text) == 1}?) -> $arg
    |   ^(SLASH arg=. i=INT {Integer.valueOf($i.text) == 1}?) -> $arg
    ;

realIdentityElision:
        ^(PLUS i=DOUBLE arg=. {Double.valueOf($i.text) == 0.0}?) -> $arg
    |   ^(PLUS arg=. i=DOUBLE {Double.valueOf($i.text) == 0.0}?) -> $arg
    |   ^(MINUS arg=. i=DOUBLE {Double.valueOf($i.text) == 0.0}?) -> $arg
    |   ^(ASTERISK i=DOUBLE arg=. {Double.valueOf($i.text) == 1.0}?) -> $arg
    |   ^(ASTERISK arg=. i=DOUBLE {Double.valueOf($i.text) == 1.0}?) -> $arg
    |   ^(SLASH arg=. i=DOUBLE {Double.valueOf($i.text) == 1.0}?) -> $arg
    ;

booleanEqualityNegation:
        ^(DEQUALS FALSE_KYWD arg=.) -> ^(NOT_KYWD $arg)
    |   ^(DEQUALS arg=. FALSE_KYWD) -> ^(NOT_KYWD $arg)
    |   ^(NEQUALS TRUE_KYWD arg=.)  -> ^(NOT_KYWD $arg)
    |   ^(NEQUALS arg=. TRUE_KYWD)  -> ^(NOT_KYWD $arg)
    ;
        
// Block flattening

flattenTrivialBlocks:
        ^(ACTION ^(BLOCK innerUnnamed=unnamedAction))
        -> $innerUnnamed
    |   ^(ACTION ^(BLOCK innerNamed=namedAction))
        -> $innerNamed
    |   ^(ACTION NCNAME ^(BLOCK namedAction)) // no transform
    |   ^(ACTION outerId=NCNAME ^(BLOCK ^(ACTION body=.)))
        -> ^(ACTION $outerId $body)
    ;

flattenTrivialSequences:
        ^(ACTION ^(SEQUENCE_KYWD innerUnnamed=unnamedAction))
        -> $innerUnnamed
    |   ^(ACTION ^(SEQUENCE_KYWD innerNamed=namedAction))
        -> $innerNamed
    |   ^(ACTION NCNAME ^(SEQUENCE_KYWD namedAction)) // no transform
    |   ^(ACTION outerId=NCNAME ^(SEQUENCE_KYWD ^(ACTION body=.)))
        -> ^(ACTION $outerId $body)
    ;

flattenTrivialUncheckedSequences:
        ^(ACTION ^(UNCHECKED_SEQUENCE_KYWD innerUnnamed=unnamedAction))
        -> $innerUnnamed
    |   ^(ACTION ^(UNCHECKED_SEQUENCE_KYWD innerNamed=namedAction))
        -> $innerNamed
    |   ^(ACTION NCNAME ^(UNCHECKED_SEQUENCE_KYWD namedAction)) // no transform
    |   ^(ACTION outerId=NCNAME ^(UNCHECKED_SEQUENCE_KYWD ^(ACTION body=.)))
        -> ^(ACTION $outerId $body)
    ;

flattenTrivialConcurrences:
        ^(ACTION ^(CONCURRENCE_KYWD innerUnnamed=unnamedAction))
        -> $innerUnnamed
    |   ^(ACTION ^(CONCURRENCE_KYWD innerNamed=namedAction))
        -> $innerNamed
    |   ^(ACTION NCNAME ^(CONCURRENCE_KYWD namedAction)) // no transform
    |   ^(ACTION outerId=NCNAME ^(CONCURRENCE_KYWD ^(ACTION body=.)))
        -> ^(ACTION $outerId $body)
    ;

//
// Recognizer rules
//

bindingContextNode:
        BLOCK
    | CONCURRENCE_KYWD
    | SEQUENCE_KYWD
    | UNCHECKED_SEQUENCE_KYWD
    | TRY_KYWD
    | FOR_KYWD
    ;

// Binary operators that can be used as N-ary
associativeOp:
        OR_KYWD
    |   AND_KYWD
    |   PLUS
    |   MINUS
    |   ASTERISK
    |   MAX_KYWD
    |   MIN_KYWD
    ;

namedAction:
        ^(ACTION NCNAME .)
 ;

unnamedAction:
        ^(ACTION .)
 ;

condition:
        ^(conditionKywd .)
 ;


conditionKywd:
        END_CONDITION_KYWD
    |   EXIT_CONDITION_KYWD
    |   INVARIANT_CONDITION_KYWD
    |   POST_CONDITION_KYWD
    |   PRE_CONDITION_KYWD
    |   REPEAT_CONDITION_KYWD
    |   SKIP_CONDITION_KYWD
    |   START_CONDITION_KYWD
    ;

        

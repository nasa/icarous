// Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

package plexil;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

public class PlexilTreeAdaptor extends org.antlr.runtime.tree.CommonTreeAdaptor
{
    public PlexilTreeAdaptor()
    {
        super();
    }

    public Object create(Token payload)
    {
        if (payload == null)
            return new PlexilTreeNode(payload);

        return this.create(payload.getType(), payload);
    }

    public Object create(int tokenType, String text)
    {
        return this.create(tokenType,
                           this.createToken(tokenType, text));
    }

    public Object create(int tokenType, Token fromToken, String text)
    {
        Token t = this.createToken(fromToken);
        t.setType(tokenType);
        t.setText(text);
        return this.create(tokenType, t);
    }

    public Object create(int tokenType, Token payload)
    {
        switch (tokenType) {
            // Arithmetic operators
        case PlexilLexer.ABS_KYWD:
        case PlexilLexer.ARRAY_MAX_SIZE_KYWD:
        case PlexilLexer.ARRAY_SIZE_KYWD:
        case PlexilLexer.ASTERISK:
        case PlexilLexer.CEIL_KYWD:
        case PlexilLexer.FLOOR_KYWD:
        case PlexilLexer.MAX_KYWD:
        case PlexilLexer.MIN_KYWD:
        case PlexilLexer.MINUS:
        case PlexilLexer.MOD_KYWD:
        case PlexilLexer.PERCENT:
        case PlexilLexer.PLUS:
        case PlexilLexer.REAL_TO_INT_KYWD:
        case PlexilLexer.ROUND_KYWD:
        case PlexilLexer.SLASH:
        case PlexilLexer.SQRT_KYWD:
        case PlexilLexer.STRLEN_KYWD:
        case PlexilLexer.TRUNC_KYWD:
            return new ArithmeticOperatorNode(payload);

            // Logical operators
        case PlexilLexer.AND_KYWD:
        case PlexilLexer.NOT_KYWD:
        case PlexilLexer.OR_KYWD:
        case PlexilLexer.XOR_KYWD:
            return new LogicalOperatorNode(payload);

            // Equality comparisons
        case PlexilLexer.DEQUALS:
        case PlexilLexer.NEQUALS:
            return new EqualityNode(payload);

            // Numeric comparisons
        case PlexilLexer.GREATER:
        case PlexilLexer.GEQ:
        case PlexilLexer.LESS:
        case PlexilLexer.LEQ:
            return new RelationalNode(payload);
            
            // Lookups
        case PlexilLexer.LOOKUP_KYWD:
        case PlexilLexer.LOOKUP_NOW_KYWD:
        case PlexilLexer.LOOKUP_ON_CHANGE_KYWD:
            return new LookupNode(payload);

            // Block types
        case PlexilLexer.BLOCK:
        case PlexilLexer.CONCURRENCE_KYWD:
        case PlexilLexer.TRY_KYWD:
        case PlexilLexer.UNCHECKED_SEQUENCE_KYWD:
        case PlexilLexer.SEQUENCE_KYWD:
            return new BlockNode(payload);

            // Conditions
        case PlexilLexer.END_CONDITION_KYWD:
        case PlexilLexer.EXIT_CONDITION_KYWD:
        case PlexilLexer.INVARIANT_CONDITION_KYWD:
        case PlexilLexer.POST_CONDITION_KYWD:
        case PlexilLexer.PRE_CONDITION_KYWD:
        case PlexilLexer.REPEAT_CONDITION_KYWD:
        case PlexilLexer.SKIP_CONDITION_KYWD:
        case PlexilLexer.START_CONDITION_KYWD:
            return new ConditionNode(payload);

			// Node ref directions
		case PlexilLexer.CHILD_KYWD:
		case PlexilLexer.PARENT_KYWD:
		case PlexilLexer.SELF_KYWD:
		case PlexilLexer.SIBLING_KYWD:
			return new NodeRefNode(payload);

			// Node state predicates
		case PlexilLexer.NODE_EXECUTING_KYWD:
		case PlexilLexer.NODE_FAILED_KYWD:
		case PlexilLexer.NODE_FINISHED_KYWD:
		case PlexilLexer.NODE_INACTIVE_KYWD:
		case PlexilLexer.NODE_INVARIANT_FAILED_KYWD:
		case PlexilLexer.NODE_ITERATION_ENDED_KYWD:
		case PlexilLexer.NODE_ITERATION_FAILED_KYWD:
		case PlexilLexer.NODE_ITERATION_SUCCEEDED_KYWD:
		case PlexilLexer.NODE_PARENT_FAILED_KYWD:
		case PlexilLexer.NODE_POSTCONDITION_FAILED_KYWD:
		case PlexilLexer.NODE_PRECONDITION_FAILED_KYWD:
		case PlexilLexer.NODE_SKIPPED_KYWD:
		case PlexilLexer.NODE_SUCCEEDED_KYWD:
		case PlexilLexer.NODE_WAITING_KYWD:
			return new NodeStatePredicateNode(payload);

            // Other syntactic features
        case PlexilLexer.ARGUMENT_LIST:
            return new ArgumentListNode(payload);

        case PlexilLexer.FOR_KYWD:
            return new ForNode(payload);

        case PlexilLexer.IF_KYWD:
            return new IfNode(payload);

        case PlexilLexer.IN_KYWD:
        case PlexilLexer.IN_OUT_KYWD:
            return new InterfaceDeclNode(payload);

        case PlexilLexer.LIBRARY_ACTION_KYWD:
            return new LibraryDeclarationNode(payload);

        case PlexilLexer.LIBRARY_CALL_KYWD:
            return new LibraryCallNode(payload);

        case PlexilLexer.ON_COMMAND_KYWD:
            return new OnCommandNode(payload);

        case PlexilLexer.PARAMETERS:
            return new ParameterSpecNode(payload);

		case PlexilLexer.RESOURCE_KYWD:
			return new ResourceNode(payload);

        case PlexilLexer.RETURNS_KYWD:
            return new ReturnSpecNode(payload);

        case PlexilLexer.SYNCHRONOUS_COMMAND_KYWD:
            return new SynchronousCommandNode(payload);

		case PlexilLexer.UPDATE_KYWD:
			return new UpdateNode(payload);

        case PlexilLexer.WAIT_KYWD:
            return new WaitNode(payload);

        case PlexilLexer.WHILE_KYWD:
            return new WhileNode(payload);

            // Internal tokens
        case PlexilLexer.ACTION:
            return new ActionNode(payload);

        case PlexilLexer.ASSIGNMENT:
            return new AssignmentNode(payload);

        case PlexilLexer.ARRAY_REF:
            return new ArrayReferenceNode(payload);

        case PlexilLexer.ARRAY_VARIABLE_DECLARATION:
            return new ArrayVariableDeclNode(payload);

		case PlexilLexer.NODE_TIMEPOINT_VALUE:
			return new NodeVariableNode(payload);

        case PlexilLexer.VARIABLE_DECLARATION:
            return new VariableDeclNode(payload);

            // Assorted literals
            // Boolean
        case PlexilLexer.TRUE_KYWD:
        case PlexilLexer.FALSE_KYWD:
            // Numeric
        case PlexilLexer.INT:
        case PlexilLexer.NEG_INT:
        case PlexilLexer.DOUBLE:
        case PlexilLexer.NEG_DOUBLE:
            // Date/duration
        case PlexilLexer.DATE_LITERAL:
        case PlexilLexer.DURATION_LITERAL:
            // Command handle
        case PlexilLexer.COMMAND_ACCEPTED_KYWD:
        case PlexilLexer.COMMAND_DENIED_KYWD:
        case PlexilLexer.COMMAND_FAILED_KYWD:
        case PlexilLexer.COMMAND_RCVD_KYWD:
        case PlexilLexer.COMMAND_SENT_KYWD:
        case PlexilLexer.COMMAND_SUCCESS_KYWD:
            // Node states
        case PlexilLexer.EXECUTING_STATE_KYWD:
        case PlexilLexer.FAILING_STATE_KYWD:
        case PlexilLexer.FINISHED_STATE_KYWD:
        case PlexilLexer.FINISHING_STATE_KYWD:
        case PlexilLexer.INACTIVE_STATE_KYWD:
        case PlexilLexer.ITERATION_ENDED_STATE_KYWD:
        case PlexilLexer.WAITING_STATE_KYWD:
            // Node outcomes
        case PlexilLexer.SUCCESS_OUTCOME_KYWD:
        case PlexilLexer.FAILURE_OUTCOME_KYWD:
        case PlexilLexer.SKIPPED_OUTCOME_KYWD:
        case PlexilLexer.INTERRUPTED_OUTCOME_KYWD:
            // Node failure types
        case PlexilLexer.PRE_CONDITION_FAILED_KYWD:
        case PlexilLexer.POST_CONDITION_FAILED_KYWD:
        case PlexilLexer.INVARIANT_CONDITION_FAILED_KYWD:
        case PlexilLexer.PARENT_FAILED_KYWD:
        case PlexilLexer.PARENT_EXITED_KYWD:
        case PlexilLexer.EXITED_KYWD:

            return new LiteralNode(payload);

        case PlexilLexer.ARRAY_LITERAL:
            return new ArrayLiteralNode(payload);

        case PlexilLexer.STRING:
            return new StringLiteralNode(payload);

        default:
            return new PlexilTreeNode(payload);
        }
    }

    public Object errorNode(TokenStream input,
                            Token start,
                            Token stop,
                            RecognitionException e)
    {
        PlexilErrorNode t = new PlexilErrorNode(input, start, stop, e);
        return t;
    }

}

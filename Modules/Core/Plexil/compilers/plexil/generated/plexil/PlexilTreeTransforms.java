// $ANTLR 3.5.2 antlr/PlexilTreeTransforms.g 2019-10-17 11:57:36

package plexil;

import plexil.*;


import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;


@SuppressWarnings("all")
public class PlexilTreeTransforms extends TreeRewriter {
	public static final String[] tokenNames = new String[] {
		"<invalid>", "<EOR>", "<DOWN>", "<UP>", "ABS_KYWD", "ACTION", "ALIAS", 
		"ALIASES", "AND_KYWD", "ANY_KYWD", "ARGUMENT_LIST", "ARRAY_LITERAL", "ARRAY_MAX_SIZE_KYWD", 
		"ARRAY_REF", "ARRAY_SIZE_KYWD", "ARRAY_TYPE", "ARRAY_VARIABLE_DECLARATION", 
		"ASSIGNMENT", "ASTERISK", "BAR", "BLOCK", "BOOLEAN_KYWD", "CEIL_KYWD", 
		"CHILD_KYWD", "COLON", "COMMA", "COMMAND", "COMMAND_ACCEPTED_KYWD", "COMMAND_DENIED_KYWD", 
		"COMMAND_FAILED_KYWD", "COMMAND_HANDLE_KYWD", "COMMAND_KYWD", "COMMAND_RCVD_KYWD", 
		"COMMAND_SENT_KYWD", "COMMAND_SUCCESS_KYWD", "COMMENT_KYWD", "CONCAT", 
		"CONCURRENCE_KYWD", "DATE_KYWD", "DATE_LITERAL", "DEQUALS", "DOUBLE", 
		"DURATION_KYWD", "DURATION_LITERAL", "Digit", "ELLIPSIS", "ELSEIF_KYWD", 
		"ELSE_KYWD", "ENDIF_KYWD", "END_CONDITION_KYWD", "END_KYWD", "EQUALS", 
		"EXECUTING_STATE_KYWD", "EXITED_KYWD", "EXIT_CONDITION_KYWD", "Escape", 
		"Exponent", "FAILING_STATE_KYWD", "FAILURE_KYWD", "FAILURE_OUTCOME_KYWD", 
		"FALSE_KYWD", "FINISHED_STATE_KYWD", "FINISHING_STATE_KYWD", "FLOOR_KYWD", 
		"FOR_KYWD", "GEQ", "GLOBAL_DECLARATIONS", "GREATER", "HASHPAREN", "HexDigit", 
		"IF_KYWD", "INACTIVE_STATE_KYWD", "INT", "INTEGER_KYWD", "INTERRUPTED_OUTCOME_KYWD", 
		"INT_OR_DOUBLE", "INVARIANT_CONDITION_FAILED_KYWD", "INVARIANT_CONDITION_KYWD", 
		"IN_KYWD", "IN_OUT_KYWD", "IS_KNOWN_KYWD", "ITERATION_ENDED_STATE_KYWD", 
		"LBRACE", "LBRACKET", "LEQ", "LESS", "LIBRARY_ACTION_KYWD", "LIBRARY_CALL_KYWD", 
		"LOOKUP_KYWD", "LOOKUP_NOW_KYWD", "LOOKUP_ON_CHANGE_KYWD", "LOWER_BOUND_KYWD", 
		"LPAREN", "Letter", "MAX_KYWD", "MESSAGE_RECEIVED_KYWD", "MINUS", "MIN_KYWD", 
		"ML_COMMENT", "MOD_KYWD", "NAME_KYWD", "NCNAME", "NEG_DOUBLE", "NEG_INT", 
		"NEQUALS", "NODE_EXECUTING_KYWD", "NODE_FAILED_KYWD", "NODE_FINISHED_KYWD", 
		"NODE_ID", "NODE_INACTIVE_KYWD", "NODE_INVARIANT_FAILED_KYWD", "NODE_ITERATION_ENDED_KYWD", 
		"NODE_ITERATION_FAILED_KYWD", "NODE_ITERATION_SUCCEEDED_KYWD", "NODE_PARENT_FAILED_KYWD", 
		"NODE_POSTCONDITION_FAILED_KYWD", "NODE_PRECONDITION_FAILED_KYWD", "NODE_SKIPPED_KYWD", 
		"NODE_SUCCEEDED_KYWD", "NODE_TIMEPOINT_VALUE", "NODE_WAITING_KYWD", "NOT_KYWD", 
		"ON_COMMAND_KYWD", "ON_MESSAGE_KYWD", "OR_KYWD", "OUTCOME_KYWD", "OctalDigit", 
		"OctalEscape", "PARAMETERS", "PARENT_EXITED_KYWD", "PARENT_FAILED_KYWD", 
		"PARENT_KYWD", "PERCENT", "PERIOD", "PLEXIL", "PLUS", "POST_CONDITION_FAILED_KYWD", 
		"POST_CONDITION_KYWD", "PRE_CONDITION_FAILED_KYWD", "PRE_CONDITION_KYWD", 
		"PRIORITY_KYWD", "QuadDigit", "RBRACE", "RBRACKET", "REAL_KYWD", "REAL_TO_INT_KYWD", 
		"RELEASE_AT_TERM_KYWD", "REPEAT_CONDITION_KYWD", "REQUEST_KYWD", "RESOURCE_KYWD", 
		"RETURNS_KYWD", "ROUND_KYWD", "RPAREN", "SELF_KYWD", "SEMICOLON", "SEQUENCE", 
		"SEQUENCE_KYWD", "SIBLING_KYWD", "SKIPPED_OUTCOME_KYWD", "SKIP_CONDITION_KYWD", 
		"SLASH", "SL_COMMENT", "SQRT_KYWD", "START_CONDITION_KYWD", "START_KYWD", 
		"STATE_KYWD", "STATE_NAME", "STRING", "STRING_COMPARISON", "STRING_KYWD", 
		"STRLEN_KYWD", "SUCCESS_OUTCOME_KYWD", "SYNCHRONOUS_COMMAND_KYWD", "TIMEOUT_KYWD", 
		"TRUE_KYWD", "TRUNC_KYWD", "TRY_KYWD", "UNCHECKED_SEQUENCE_KYWD", "UPDATE_KYWD", 
		"UPPER_BOUND_KYWD", "UnicodeEscape", "VARIABLE_DECLARATION", "VARIABLE_DECLARATIONS", 
		"WAITING_STATE_KYWD", "WAIT_KYWD", "WHILE_KYWD", "WS", "XOR_KYWD"
	};
	public static final int EOF=-1;
	public static final int ABS_KYWD=4;
	public static final int ACTION=5;
	public static final int ALIAS=6;
	public static final int ALIASES=7;
	public static final int AND_KYWD=8;
	public static final int ANY_KYWD=9;
	public static final int ARGUMENT_LIST=10;
	public static final int ARRAY_LITERAL=11;
	public static final int ARRAY_MAX_SIZE_KYWD=12;
	public static final int ARRAY_REF=13;
	public static final int ARRAY_SIZE_KYWD=14;
	public static final int ARRAY_TYPE=15;
	public static final int ARRAY_VARIABLE_DECLARATION=16;
	public static final int ASSIGNMENT=17;
	public static final int ASTERISK=18;
	public static final int BAR=19;
	public static final int BLOCK=20;
	public static final int BOOLEAN_KYWD=21;
	public static final int CEIL_KYWD=22;
	public static final int CHILD_KYWD=23;
	public static final int COLON=24;
	public static final int COMMA=25;
	public static final int COMMAND=26;
	public static final int COMMAND_ACCEPTED_KYWD=27;
	public static final int COMMAND_DENIED_KYWD=28;
	public static final int COMMAND_FAILED_KYWD=29;
	public static final int COMMAND_HANDLE_KYWD=30;
	public static final int COMMAND_KYWD=31;
	public static final int COMMAND_RCVD_KYWD=32;
	public static final int COMMAND_SENT_KYWD=33;
	public static final int COMMAND_SUCCESS_KYWD=34;
	public static final int COMMENT_KYWD=35;
	public static final int CONCAT=36;
	public static final int CONCURRENCE_KYWD=37;
	public static final int DATE_KYWD=38;
	public static final int DATE_LITERAL=39;
	public static final int DEQUALS=40;
	public static final int DOUBLE=41;
	public static final int DURATION_KYWD=42;
	public static final int DURATION_LITERAL=43;
	public static final int Digit=44;
	public static final int ELLIPSIS=45;
	public static final int ELSEIF_KYWD=46;
	public static final int ELSE_KYWD=47;
	public static final int ENDIF_KYWD=48;
	public static final int END_CONDITION_KYWD=49;
	public static final int END_KYWD=50;
	public static final int EQUALS=51;
	public static final int EXECUTING_STATE_KYWD=52;
	public static final int EXITED_KYWD=53;
	public static final int EXIT_CONDITION_KYWD=54;
	public static final int Escape=55;
	public static final int Exponent=56;
	public static final int FAILING_STATE_KYWD=57;
	public static final int FAILURE_KYWD=58;
	public static final int FAILURE_OUTCOME_KYWD=59;
	public static final int FALSE_KYWD=60;
	public static final int FINISHED_STATE_KYWD=61;
	public static final int FINISHING_STATE_KYWD=62;
	public static final int FLOOR_KYWD=63;
	public static final int FOR_KYWD=64;
	public static final int GEQ=65;
	public static final int GLOBAL_DECLARATIONS=66;
	public static final int GREATER=67;
	public static final int HASHPAREN=68;
	public static final int HexDigit=69;
	public static final int IF_KYWD=70;
	public static final int INACTIVE_STATE_KYWD=71;
	public static final int INT=72;
	public static final int INTEGER_KYWD=73;
	public static final int INTERRUPTED_OUTCOME_KYWD=74;
	public static final int INT_OR_DOUBLE=75;
	public static final int INVARIANT_CONDITION_FAILED_KYWD=76;
	public static final int INVARIANT_CONDITION_KYWD=77;
	public static final int IN_KYWD=78;
	public static final int IN_OUT_KYWD=79;
	public static final int IS_KNOWN_KYWD=80;
	public static final int ITERATION_ENDED_STATE_KYWD=81;
	public static final int LBRACE=82;
	public static final int LBRACKET=83;
	public static final int LEQ=84;
	public static final int LESS=85;
	public static final int LIBRARY_ACTION_KYWD=86;
	public static final int LIBRARY_CALL_KYWD=87;
	public static final int LOOKUP_KYWD=88;
	public static final int LOOKUP_NOW_KYWD=89;
	public static final int LOOKUP_ON_CHANGE_KYWD=90;
	public static final int LOWER_BOUND_KYWD=91;
	public static final int LPAREN=92;
	public static final int Letter=93;
	public static final int MAX_KYWD=94;
	public static final int MESSAGE_RECEIVED_KYWD=95;
	public static final int MINUS=96;
	public static final int MIN_KYWD=97;
	public static final int ML_COMMENT=98;
	public static final int MOD_KYWD=99;
	public static final int NAME_KYWD=100;
	public static final int NCNAME=101;
	public static final int NEG_DOUBLE=102;
	public static final int NEG_INT=103;
	public static final int NEQUALS=104;
	public static final int NODE_EXECUTING_KYWD=105;
	public static final int NODE_FAILED_KYWD=106;
	public static final int NODE_FINISHED_KYWD=107;
	public static final int NODE_ID=108;
	public static final int NODE_INACTIVE_KYWD=109;
	public static final int NODE_INVARIANT_FAILED_KYWD=110;
	public static final int NODE_ITERATION_ENDED_KYWD=111;
	public static final int NODE_ITERATION_FAILED_KYWD=112;
	public static final int NODE_ITERATION_SUCCEEDED_KYWD=113;
	public static final int NODE_PARENT_FAILED_KYWD=114;
	public static final int NODE_POSTCONDITION_FAILED_KYWD=115;
	public static final int NODE_PRECONDITION_FAILED_KYWD=116;
	public static final int NODE_SKIPPED_KYWD=117;
	public static final int NODE_SUCCEEDED_KYWD=118;
	public static final int NODE_TIMEPOINT_VALUE=119;
	public static final int NODE_WAITING_KYWD=120;
	public static final int NOT_KYWD=121;
	public static final int ON_COMMAND_KYWD=122;
	public static final int ON_MESSAGE_KYWD=123;
	public static final int OR_KYWD=124;
	public static final int OUTCOME_KYWD=125;
	public static final int OctalDigit=126;
	public static final int OctalEscape=127;
	public static final int PARAMETERS=128;
	public static final int PARENT_EXITED_KYWD=129;
	public static final int PARENT_FAILED_KYWD=130;
	public static final int PARENT_KYWD=131;
	public static final int PERCENT=132;
	public static final int PERIOD=133;
	public static final int PLEXIL=134;
	public static final int PLUS=135;
	public static final int POST_CONDITION_FAILED_KYWD=136;
	public static final int POST_CONDITION_KYWD=137;
	public static final int PRE_CONDITION_FAILED_KYWD=138;
	public static final int PRE_CONDITION_KYWD=139;
	public static final int PRIORITY_KYWD=140;
	public static final int QuadDigit=141;
	public static final int RBRACE=142;
	public static final int RBRACKET=143;
	public static final int REAL_KYWD=144;
	public static final int REAL_TO_INT_KYWD=145;
	public static final int RELEASE_AT_TERM_KYWD=146;
	public static final int REPEAT_CONDITION_KYWD=147;
	public static final int REQUEST_KYWD=148;
	public static final int RESOURCE_KYWD=149;
	public static final int RETURNS_KYWD=150;
	public static final int ROUND_KYWD=151;
	public static final int RPAREN=152;
	public static final int SELF_KYWD=153;
	public static final int SEMICOLON=154;
	public static final int SEQUENCE=155;
	public static final int SEQUENCE_KYWD=156;
	public static final int SIBLING_KYWD=157;
	public static final int SKIPPED_OUTCOME_KYWD=158;
	public static final int SKIP_CONDITION_KYWD=159;
	public static final int SLASH=160;
	public static final int SL_COMMENT=161;
	public static final int SQRT_KYWD=162;
	public static final int START_CONDITION_KYWD=163;
	public static final int START_KYWD=164;
	public static final int STATE_KYWD=165;
	public static final int STATE_NAME=166;
	public static final int STRING=167;
	public static final int STRING_COMPARISON=168;
	public static final int STRING_KYWD=169;
	public static final int STRLEN_KYWD=170;
	public static final int SUCCESS_OUTCOME_KYWD=171;
	public static final int SYNCHRONOUS_COMMAND_KYWD=172;
	public static final int TIMEOUT_KYWD=173;
	public static final int TRUE_KYWD=174;
	public static final int TRUNC_KYWD=175;
	public static final int TRY_KYWD=176;
	public static final int UNCHECKED_SEQUENCE_KYWD=177;
	public static final int UPDATE_KYWD=178;
	public static final int UPPER_BOUND_KYWD=179;
	public static final int UnicodeEscape=180;
	public static final int VARIABLE_DECLARATION=181;
	public static final int VARIABLE_DECLARATIONS=182;
	public static final int WAITING_STATE_KYWD=183;
	public static final int WAIT_KYWD=184;
	public static final int WHILE_KYWD=185;
	public static final int WS=186;
	public static final int XOR_KYWD=187;

	// delegates
	public TreeRewriter[] getDelegates() {
		return new TreeRewriter[] {};
	}

	// delegators


	public PlexilTreeTransforms(TreeNodeStream input) {
		this(input, new RecognizerSharedState());
	}
	public PlexilTreeTransforms(TreeNodeStream input, RecognizerSharedState state) {
		super(input, state);
	}

	protected TreeAdaptor adaptor = new CommonTreeAdaptor();

	public void setTreeAdaptor(TreeAdaptor adaptor) {
		this.adaptor = adaptor;
	}
	public TreeAdaptor getTreeAdaptor() {
		return adaptor;
	}
	@Override public String[] getTokenNames() { return PlexilTreeTransforms.tokenNames; }
	@Override public String getGrammarFileName() { return "antlr/PlexilTreeTransforms.g"; }


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



	public static class topdown_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "topdown"
	// antlr/PlexilTreeTransforms.g:90:1: topdown : enterContext ;
	@Override
	public final PlexilTreeTransforms.topdown_return topdown() throws RecognitionException {
		PlexilTreeTransforms.topdown_return retval = new PlexilTreeTransforms.topdown_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		TreeRuleReturnScope enterContext1 =null;


		try {
			// antlr/PlexilTreeTransforms.g:90:8: ( enterContext )
			// antlr/PlexilTreeTransforms.g:91:9: enterContext
			{
			_last = (PlexilTreeNode)input.LT(1);
			pushFollow(FOLLOW_enterContext_in_topdown154);
			enterContext1=enterContext();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==1 ) 
			 
			if ( _first_0==null ) _first_0 = (PlexilTreeNode)enterContext1.getTree();

			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "topdown"


	public static class bottomup_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "bottomup"
	// antlr/PlexilTreeTransforms.g:94:1: bottomup : ( trivialReduction | booleanIdentityElision | integerIdentityElision | realIdentityElision | booleanEqualityNegation | rightAssociativeReduction | leftAssociativeReduction | flattenTrivialBlocks | flattenTrivialSequences | flattenTrivialUncheckedSequences | flattenTrivialConcurrences | exitContext );
	@Override
	public final PlexilTreeTransforms.bottomup_return bottomup() throws RecognitionException {
		PlexilTreeTransforms.bottomup_return retval = new PlexilTreeTransforms.bottomup_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		TreeRuleReturnScope trivialReduction2 =null;
		TreeRuleReturnScope booleanIdentityElision3 =null;
		TreeRuleReturnScope integerIdentityElision4 =null;
		TreeRuleReturnScope realIdentityElision5 =null;
		TreeRuleReturnScope booleanEqualityNegation6 =null;
		TreeRuleReturnScope rightAssociativeReduction7 =null;
		TreeRuleReturnScope leftAssociativeReduction8 =null;
		TreeRuleReturnScope flattenTrivialBlocks9 =null;
		TreeRuleReturnScope flattenTrivialSequences10 =null;
		TreeRuleReturnScope flattenTrivialUncheckedSequences11 =null;
		TreeRuleReturnScope flattenTrivialConcurrences12 =null;
		TreeRuleReturnScope exitContext13 =null;


		try {
			// antlr/PlexilTreeTransforms.g:94:9: ( trivialReduction | booleanIdentityElision | integerIdentityElision | realIdentityElision | booleanEqualityNegation | rightAssociativeReduction | leftAssociativeReduction | flattenTrivialBlocks | flattenTrivialSequences | flattenTrivialUncheckedSequences | flattenTrivialConcurrences | exitContext )
			int alt1=12;
			switch ( input.LA(1) ) {
			case AND_KYWD:
				{
				int LA1_1 = input.LA(2);
				if ( (LA1_1==DOWN) ) {
					int LA1_13 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred2_PlexilTreeTransforms()) ) {
						alt1=2;
					}
					else if ( (synpred6_PlexilTreeTransforms()) ) {
						alt1=6;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 13, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case OR_KYWD:
				{
				int LA1_2 = input.LA(2);
				if ( (LA1_2==DOWN) ) {
					int LA1_14 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred2_PlexilTreeTransforms()) ) {
						alt1=2;
					}
					else if ( (synpred6_PlexilTreeTransforms()) ) {
						alt1=6;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 14, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 2, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case PLUS:
				{
				int LA1_3 = input.LA(2);
				if ( (LA1_3==DOWN) ) {
					int LA1_15 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred3_PlexilTreeTransforms()) ) {
						alt1=3;
					}
					else if ( (synpred4_PlexilTreeTransforms()) ) {
						alt1=4;
					}
					else if ( (synpred6_PlexilTreeTransforms()) ) {
						alt1=6;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 15, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 3, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case MINUS:
				{
				int LA1_4 = input.LA(2);
				if ( (LA1_4==DOWN) ) {
					int LA1_16 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred3_PlexilTreeTransforms()) ) {
						alt1=3;
					}
					else if ( (synpred4_PlexilTreeTransforms()) ) {
						alt1=4;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 16, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 4, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case ASTERISK:
				{
				int LA1_5 = input.LA(2);
				if ( (LA1_5==DOWN) ) {
					int LA1_17 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred3_PlexilTreeTransforms()) ) {
						alt1=3;
					}
					else if ( (synpred4_PlexilTreeTransforms()) ) {
						alt1=4;
					}
					else if ( (synpred6_PlexilTreeTransforms()) ) {
						alt1=6;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 17, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 5, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case SLASH:
				{
				int LA1_6 = input.LA(2);
				if ( (LA1_6==DOWN) ) {
					int LA1_18 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred3_PlexilTreeTransforms()) ) {
						alt1=3;
					}
					else if ( (synpred4_PlexilTreeTransforms()) ) {
						alt1=4;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 18, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 6, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case MAX_KYWD:
				{
				int LA1_7 = input.LA(2);
				if ( (LA1_7==DOWN) ) {
					int LA1_19 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred6_PlexilTreeTransforms()) ) {
						alt1=6;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 19, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 7, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case MIN_KYWD:
				{
				int LA1_8 = input.LA(2);
				if ( (LA1_8==DOWN) ) {
					int LA1_20 = input.LA(3);
					if ( (synpred1_PlexilTreeTransforms()) ) {
						alt1=1;
					}
					else if ( (synpred6_PlexilTreeTransforms()) ) {
						alt1=6;
					}
					else if ( (synpred7_PlexilTreeTransforms()) ) {
						alt1=7;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 20, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 8, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case DEQUALS:
				{
				int LA1_9 = input.LA(2);
				if ( (LA1_9==DOWN) ) {
					int LA1_21 = input.LA(3);
					if ( (synpred2_PlexilTreeTransforms()) ) {
						alt1=2;
					}
					else if ( (synpred5_PlexilTreeTransforms()) ) {
						alt1=5;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 21, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 9, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case NEQUALS:
				{
				int LA1_10 = input.LA(2);
				if ( (LA1_10==DOWN) ) {
					int LA1_22 = input.LA(3);
					if ( (synpred2_PlexilTreeTransforms()) ) {
						alt1=2;
					}
					else if ( (synpred5_PlexilTreeTransforms()) ) {
						alt1=5;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 22, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 10, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case ACTION:
				{
				int LA1_11 = input.LA(2);
				if ( (LA1_11==DOWN) ) {
					int LA1_23 = input.LA(3);
					if ( (synpred8_PlexilTreeTransforms()) ) {
						alt1=8;
					}
					else if ( (synpred9_PlexilTreeTransforms()) ) {
						alt1=9;
					}
					else if ( (synpred10_PlexilTreeTransforms()) ) {
						alt1=10;
					}
					else if ( (synpred11_PlexilTreeTransforms()) ) {
						alt1=11;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 1, 23, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 1, 11, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case BLOCK:
			case CONCURRENCE_KYWD:
			case FOR_KYWD:
			case SEQUENCE_KYWD:
			case TRY_KYWD:
			case UNCHECKED_SEQUENCE_KYWD:
				{
				alt1=12;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 1, 0, input);
				throw nvae;
			}
			switch (alt1) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:95:9: trivialReduction
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_trivialReduction_in_bottomup174);
					trivialReduction2=trivialReduction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)trivialReduction2.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:96:9: booleanIdentityElision
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_booleanIdentityElision_in_bottomup188);
					booleanIdentityElision3=booleanIdentityElision();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)booleanIdentityElision3.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:97:9: integerIdentityElision
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_integerIdentityElision_in_bottomup198);
					integerIdentityElision4=integerIdentityElision();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)integerIdentityElision4.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:98:9: realIdentityElision
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_realIdentityElision_in_bottomup208);
					realIdentityElision5=realIdentityElision();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)realIdentityElision5.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:99:9: booleanEqualityNegation
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_booleanEqualityNegation_in_bottomup218);
					booleanEqualityNegation6=booleanEqualityNegation();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)booleanEqualityNegation6.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:100:9: rightAssociativeReduction
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_rightAssociativeReduction_in_bottomup228);
					rightAssociativeReduction7=rightAssociativeReduction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)rightAssociativeReduction7.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 7 :
					// antlr/PlexilTreeTransforms.g:101:9: leftAssociativeReduction
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_leftAssociativeReduction_in_bottomup238);
					leftAssociativeReduction8=leftAssociativeReduction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)leftAssociativeReduction8.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 8 :
					// antlr/PlexilTreeTransforms.g:102:9: flattenTrivialBlocks
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_flattenTrivialBlocks_in_bottomup248);
					flattenTrivialBlocks9=flattenTrivialBlocks();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)flattenTrivialBlocks9.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 9 :
					// antlr/PlexilTreeTransforms.g:103:9: flattenTrivialSequences
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_flattenTrivialSequences_in_bottomup258);
					flattenTrivialSequences10=flattenTrivialSequences();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)flattenTrivialSequences10.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 10 :
					// antlr/PlexilTreeTransforms.g:104:9: flattenTrivialUncheckedSequences
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_flattenTrivialUncheckedSequences_in_bottomup268);
					flattenTrivialUncheckedSequences11=flattenTrivialUncheckedSequences();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)flattenTrivialUncheckedSequences11.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 11 :
					// antlr/PlexilTreeTransforms.g:105:9: flattenTrivialConcurrences
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_flattenTrivialConcurrences_in_bottomup278);
					flattenTrivialConcurrences12=flattenTrivialConcurrences();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)flattenTrivialConcurrences12.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 12 :
					// antlr/PlexilTreeTransforms.g:106:9: exitContext
					{
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_exitContext_in_bottomup288);
					exitContext13=exitContext();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_0==null ) _first_0 = (PlexilTreeNode)exitContext13.getTree();

					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "bottomup"


	public static class enterContext_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "enterContext"
	// antlr/PlexilTreeTransforms.g:113:1: enterContext : bindingContextNode ;
	public final PlexilTreeTransforms.enterContext_return enterContext() throws RecognitionException {
		PlexilTreeTransforms.enterContext_return retval = new PlexilTreeTransforms.enterContext_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		TreeRuleReturnScope bindingContextNode14 =null;


		try {
			// antlr/PlexilTreeTransforms.g:113:13: ( bindingContextNode )
			// antlr/PlexilTreeTransforms.g:114:9: bindingContextNode
			{
			_last = (PlexilTreeNode)input.LT(1);
			pushFollow(FOLLOW_bindingContextNode_in_enterContext312);
			bindingContextNode14=bindingContextNode();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==1 ) 
			 
			if ( _first_0==null ) _first_0 = (PlexilTreeNode)bindingContextNode14.getTree();

			if ( state.backtracking==1 ) {
			            m_context = (bindingContextNode14!=null?((PlexilTreeNode)bindingContextNode14.start):null).getContext();
			//            System.out.println("Enter context " + m_context.getNodeName()); // DEBUG
			        }
			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "enterContext"


	public static class exitContext_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "exitContext"
	// antlr/PlexilTreeTransforms.g:124:1: exitContext : bindingContextNode ;
	public final PlexilTreeTransforms.exitContext_return exitContext() throws RecognitionException {
		PlexilTreeTransforms.exitContext_return retval = new PlexilTreeTransforms.exitContext_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		TreeRuleReturnScope bindingContextNode15 =null;


		try {
			// antlr/PlexilTreeTransforms.g:124:12: ( bindingContextNode )
			// antlr/PlexilTreeTransforms.g:125:9: bindingContextNode
			{
			_last = (PlexilTreeNode)input.LT(1);
			pushFollow(FOLLOW_bindingContextNode_in_exitContext342);
			bindingContextNode15=bindingContextNode();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==1 ) 
			 
			if ( _first_0==null ) _first_0 = (PlexilTreeNode)bindingContextNode15.getTree();

			if ( state.backtracking==1 ) {
			//            System.out.println("Exit context " + m_context.getNodeName()); // DEBUG
			            m_context = m_context.getParentContext();
			//            System.out.println("Restore context " + m_context.getNodeName()); // DEBUG
			        }
			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "exitContext"


	public static class rightAssociativeReduction_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "rightAssociativeReduction"
	// antlr/PlexilTreeTransforms.g:138:1: rightAssociativeReduction : ( ^( AND_KYWD args+= . ^( AND_KYWD (args+= . )+ ) ) -> ^( AND_KYWD ( $args)+ ) | ^( OR_KYWD args+= . ^( OR_KYWD (args+= . )+ ) ) -> ^( OR_KYWD ( $args)+ ) | ^( PLUS args+= . ^( PLUS (args+= . )+ ) ) -> ^( PLUS ( $args)+ ) | ^( ASTERISK args+= . ^( ASTERISK (args+= . )+ ) ) -> ^( ASTERISK ( $args)+ ) | ^( MAX_KYWD args+= . ^( MAX_KYWD (args+= . )+ ) ) -> ^( MAX_KYWD ( $args)+ ) | ^( MIN_KYWD args+= . ^( MIN_KYWD (args+= . )+ ) ) -> ^( MIN_KYWD ( $args)+ ) );
	public final PlexilTreeTransforms.rightAssociativeReduction_return rightAssociativeReduction() throws RecognitionException {
		PlexilTreeTransforms.rightAssociativeReduction_return retval = new PlexilTreeTransforms.rightAssociativeReduction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode AND_KYWD16=null;
		PlexilTreeNode AND_KYWD17=null;
		PlexilTreeNode OR_KYWD18=null;
		PlexilTreeNode OR_KYWD19=null;
		PlexilTreeNode PLUS20=null;
		PlexilTreeNode PLUS21=null;
		PlexilTreeNode ASTERISK22=null;
		PlexilTreeNode ASTERISK23=null;
		PlexilTreeNode MAX_KYWD24=null;
		PlexilTreeNode MAX_KYWD25=null;
		PlexilTreeNode MIN_KYWD26=null;
		PlexilTreeNode MIN_KYWD27=null;
		PlexilTreeNode args=null;
		List<Object> list_args=null;

		PlexilTreeNode AND_KYWD16_tree=null;
		PlexilTreeNode AND_KYWD17_tree=null;
		PlexilTreeNode OR_KYWD18_tree=null;
		PlexilTreeNode OR_KYWD19_tree=null;
		PlexilTreeNode PLUS20_tree=null;
		PlexilTreeNode PLUS21_tree=null;
		PlexilTreeNode ASTERISK22_tree=null;
		PlexilTreeNode ASTERISK23_tree=null;
		PlexilTreeNode MAX_KYWD24_tree=null;
		PlexilTreeNode MAX_KYWD25_tree=null;
		PlexilTreeNode MIN_KYWD26_tree=null;
		PlexilTreeNode MIN_KYWD27_tree=null;
		PlexilTreeNode args_tree=null;
		RewriteRuleNodeStream stream_OR_KYWD=new RewriteRuleNodeStream(adaptor,"token OR_KYWD");
		RewriteRuleNodeStream stream_MIN_KYWD=new RewriteRuleNodeStream(adaptor,"token MIN_KYWD");
		RewriteRuleNodeStream stream_AND_KYWD=new RewriteRuleNodeStream(adaptor,"token AND_KYWD");
		RewriteRuleNodeStream stream_ASTERISK=new RewriteRuleNodeStream(adaptor,"token ASTERISK");
		RewriteRuleNodeStream stream_MAX_KYWD=new RewriteRuleNodeStream(adaptor,"token MAX_KYWD");
		RewriteRuleNodeStream stream_PLUS=new RewriteRuleNodeStream(adaptor,"token PLUS");

		try {
			// antlr/PlexilTreeTransforms.g:138:26: ( ^( AND_KYWD args+= . ^( AND_KYWD (args+= . )+ ) ) -> ^( AND_KYWD ( $args)+ ) | ^( OR_KYWD args+= . ^( OR_KYWD (args+= . )+ ) ) -> ^( OR_KYWD ( $args)+ ) | ^( PLUS args+= . ^( PLUS (args+= . )+ ) ) -> ^( PLUS ( $args)+ ) | ^( ASTERISK args+= . ^( ASTERISK (args+= . )+ ) ) -> ^( ASTERISK ( $args)+ ) | ^( MAX_KYWD args+= . ^( MAX_KYWD (args+= . )+ ) ) -> ^( MAX_KYWD ( $args)+ ) | ^( MIN_KYWD args+= . ^( MIN_KYWD (args+= . )+ ) ) -> ^( MIN_KYWD ( $args)+ ) )
			int alt8=6;
			switch ( input.LA(1) ) {
			case AND_KYWD:
				{
				alt8=1;
				}
				break;
			case OR_KYWD:
				{
				alt8=2;
				}
				break;
			case PLUS:
				{
				alt8=3;
				}
				break;
			case ASTERISK:
				{
				alt8=4;
				}
				break;
			case MAX_KYWD:
				{
				alt8=5;
				}
				break;
			case MIN_KYWD:
				{
				alt8=6;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 8, 0, input);
				throw nvae;
			}
			switch (alt8) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:139:9: ^( AND_KYWD args+= . ^( AND_KYWD (args+= . )+ ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD16=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_rightAssociativeReduction375); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD16);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = AND_KYWD16;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD17=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_rightAssociativeReduction382); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD17);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = AND_KYWD17;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:139:39: (args+= . )+
					int cnt2=0;
					loop2:
					while (true) {
						int alt2=2;
						int LA2_0 = input.LA(1);
						if ( ((LA2_0 >= ABS_KYWD && LA2_0 <= XOR_KYWD)) ) {
							alt2=1;
						}

						switch (alt2) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:139:40: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt2 >= 1 ) break loop2;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(2, input);
							throw eee;
						}
						cnt2++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: args, AND_KYWD
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 139:53: -> ^( AND_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:139:56: ^( AND_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_AND_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:140:9: ^( OR_KYWD args+= . ^( OR_KYWD (args+= . )+ ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD18=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_rightAssociativeReduction413); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD18);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = OR_KYWD18;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD19=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_rightAssociativeReduction420); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD19);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = OR_KYWD19;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:140:37: (args+= . )+
					int cnt3=0;
					loop3:
					while (true) {
						int alt3=2;
						int LA3_0 = input.LA(1);
						if ( ((LA3_0 >= ABS_KYWD && LA3_0 <= XOR_KYWD)) ) {
							alt3=1;
						}

						switch (alt3) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:140:38: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt3 >= 1 ) break loop3;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(3, input);
							throw eee;
						}
						cnt3++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: OR_KYWD, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 140:52: -> ^( OR_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:140:55: ^( OR_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_OR_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:141:9: ^( PLUS args+= . ^( PLUS (args+= . )+ ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS20=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_rightAssociativeReduction452); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS20);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS20;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS21=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_rightAssociativeReduction459); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS21);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = PLUS21;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:141:31: (args+= . )+
					int cnt4=0;
					loop4:
					while (true) {
						int alt4=2;
						int LA4_0 = input.LA(1);
						if ( ((LA4_0 >= ABS_KYWD && LA4_0 <= XOR_KYWD)) ) {
							alt4=1;
						}

						switch (alt4) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:141:32: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt4 >= 1 ) break loop4;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(4, input);
							throw eee;
						}
						cnt4++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: args, PLUS
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 141:52: -> ^( PLUS ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:141:55: ^( PLUS ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_PLUS.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:142:9: ^( ASTERISK args+= . ^( ASTERISK (args+= . )+ ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK22=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_rightAssociativeReduction497); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK22);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK22;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK23=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_rightAssociativeReduction504); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK23);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = ASTERISK23;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:142:39: (args+= . )+
					int cnt5=0;
					loop5:
					while (true) {
						int alt5=2;
						int LA5_0 = input.LA(1);
						if ( ((LA5_0 >= ABS_KYWD && LA5_0 <= XOR_KYWD)) ) {
							alt5=1;
						}

						switch (alt5) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:142:40: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt5 >= 1 ) break loop5;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(5, input);
							throw eee;
						}
						cnt5++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: args, ASTERISK
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 142:52: -> ^( ASTERISK ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:142:55: ^( ASTERISK ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_ASTERISK.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:143:9: ^( MAX_KYWD args+= . ^( MAX_KYWD (args+= . )+ ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MAX_KYWD24=(PlexilTreeNode)match(input,MAX_KYWD,FOLLOW_MAX_KYWD_in_rightAssociativeReduction534); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MAX_KYWD.add(MAX_KYWD24);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MAX_KYWD24;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MAX_KYWD25=(PlexilTreeNode)match(input,MAX_KYWD,FOLLOW_MAX_KYWD_in_rightAssociativeReduction541); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MAX_KYWD.add(MAX_KYWD25);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = MAX_KYWD25;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:143:39: (args+= . )+
					int cnt6=0;
					loop6:
					while (true) {
						int alt6=2;
						int LA6_0 = input.LA(1);
						if ( ((LA6_0 >= ABS_KYWD && LA6_0 <= XOR_KYWD)) ) {
							alt6=1;
						}

						switch (alt6) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:143:40: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt6 >= 1 ) break loop6;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(6, input);
							throw eee;
						}
						cnt6++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: args, MAX_KYWD
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 143:52: -> ^( MAX_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:143:55: ^( MAX_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_MAX_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:144:9: ^( MIN_KYWD args+= . ^( MIN_KYWD (args+= . )+ ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MIN_KYWD26=(PlexilTreeNode)match(input,MIN_KYWD,FOLLOW_MIN_KYWD_in_rightAssociativeReduction571); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MIN_KYWD.add(MIN_KYWD26);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MIN_KYWD26;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MIN_KYWD27=(PlexilTreeNode)match(input,MIN_KYWD,FOLLOW_MIN_KYWD_in_rightAssociativeReduction578); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MIN_KYWD.add(MIN_KYWD27);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = MIN_KYWD27;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:144:39: (args+= . )+
					int cnt7=0;
					loop7:
					while (true) {
						int alt7=2;
						int LA7_0 = input.LA(1);
						if ( ((LA7_0 >= ABS_KYWD && LA7_0 <= XOR_KYWD)) ) {
							alt7=1;
						}

						switch (alt7) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:144:40: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt7 >= 1 ) break loop7;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(7, input);
							throw eee;
						}
						cnt7++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: MIN_KYWD, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 144:52: -> ^( MIN_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:144:55: ^( MIN_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_MIN_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "rightAssociativeReduction"


	public static class leftAssociativeReduction_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "leftAssociativeReduction"
	// antlr/PlexilTreeTransforms.g:147:1: leftAssociativeReduction : ( ^( AND_KYWD ^( AND_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( AND_KYWD ( $args)+ ) | ^( OR_KYWD ^( OR_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( OR_KYWD ( $args)+ ) | ^( PLUS ^( PLUS (args+= . )+ ) (args+= . )+ ) -> ^( PLUS ( $args)+ ) | ^( MINUS ^( MINUS args+= . (args+= . )+ ) (args+= . )+ ) -> ^( MINUS ( $args)+ ) | ^( ASTERISK ^( ASTERISK (args+= . )+ ) (args+= . )+ ) -> ^( ASTERISK ( $args)+ ) | ^( MAX_KYWD ^( MAX_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( MAX_KYWD ( $args)+ ) | ^( MIN_KYWD ^( MIN_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( MIN_KYWD ( $args)+ ) );
	public final PlexilTreeTransforms.leftAssociativeReduction_return leftAssociativeReduction() throws RecognitionException {
		PlexilTreeTransforms.leftAssociativeReduction_return retval = new PlexilTreeTransforms.leftAssociativeReduction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode AND_KYWD28=null;
		PlexilTreeNode AND_KYWD29=null;
		PlexilTreeNode OR_KYWD30=null;
		PlexilTreeNode OR_KYWD31=null;
		PlexilTreeNode PLUS32=null;
		PlexilTreeNode PLUS33=null;
		PlexilTreeNode MINUS34=null;
		PlexilTreeNode MINUS35=null;
		PlexilTreeNode ASTERISK36=null;
		PlexilTreeNode ASTERISK37=null;
		PlexilTreeNode MAX_KYWD38=null;
		PlexilTreeNode MAX_KYWD39=null;
		PlexilTreeNode MIN_KYWD40=null;
		PlexilTreeNode MIN_KYWD41=null;
		PlexilTreeNode args=null;
		List<Object> list_args=null;

		PlexilTreeNode AND_KYWD28_tree=null;
		PlexilTreeNode AND_KYWD29_tree=null;
		PlexilTreeNode OR_KYWD30_tree=null;
		PlexilTreeNode OR_KYWD31_tree=null;
		PlexilTreeNode PLUS32_tree=null;
		PlexilTreeNode PLUS33_tree=null;
		PlexilTreeNode MINUS34_tree=null;
		PlexilTreeNode MINUS35_tree=null;
		PlexilTreeNode ASTERISK36_tree=null;
		PlexilTreeNode ASTERISK37_tree=null;
		PlexilTreeNode MAX_KYWD38_tree=null;
		PlexilTreeNode MAX_KYWD39_tree=null;
		PlexilTreeNode MIN_KYWD40_tree=null;
		PlexilTreeNode MIN_KYWD41_tree=null;
		PlexilTreeNode args_tree=null;
		RewriteRuleNodeStream stream_OR_KYWD=new RewriteRuleNodeStream(adaptor,"token OR_KYWD");
		RewriteRuleNodeStream stream_MIN_KYWD=new RewriteRuleNodeStream(adaptor,"token MIN_KYWD");
		RewriteRuleNodeStream stream_AND_KYWD=new RewriteRuleNodeStream(adaptor,"token AND_KYWD");
		RewriteRuleNodeStream stream_ASTERISK=new RewriteRuleNodeStream(adaptor,"token ASTERISK");
		RewriteRuleNodeStream stream_MAX_KYWD=new RewriteRuleNodeStream(adaptor,"token MAX_KYWD");
		RewriteRuleNodeStream stream_PLUS=new RewriteRuleNodeStream(adaptor,"token PLUS");
		RewriteRuleNodeStream stream_MINUS=new RewriteRuleNodeStream(adaptor,"token MINUS");

		try {
			// antlr/PlexilTreeTransforms.g:147:25: ( ^( AND_KYWD ^( AND_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( AND_KYWD ( $args)+ ) | ^( OR_KYWD ^( OR_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( OR_KYWD ( $args)+ ) | ^( PLUS ^( PLUS (args+= . )+ ) (args+= . )+ ) -> ^( PLUS ( $args)+ ) | ^( MINUS ^( MINUS args+= . (args+= . )+ ) (args+= . )+ ) -> ^( MINUS ( $args)+ ) | ^( ASTERISK ^( ASTERISK (args+= . )+ ) (args+= . )+ ) -> ^( ASTERISK ( $args)+ ) | ^( MAX_KYWD ^( MAX_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( MAX_KYWD ( $args)+ ) | ^( MIN_KYWD ^( MIN_KYWD (args+= . )+ ) (args+= . )+ ) -> ^( MIN_KYWD ( $args)+ ) )
			int alt23=7;
			switch ( input.LA(1) ) {
			case AND_KYWD:
				{
				alt23=1;
				}
				break;
			case OR_KYWD:
				{
				alt23=2;
				}
				break;
			case PLUS:
				{
				alt23=3;
				}
				break;
			case MINUS:
				{
				alt23=4;
				}
				break;
			case ASTERISK:
				{
				alt23=5;
				}
				break;
			case MAX_KYWD:
				{
				alt23=6;
				}
				break;
			case MIN_KYWD:
				{
				alt23=7;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 23, 0, input);
				throw nvae;
			}
			switch (alt23) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:148:9: ^( AND_KYWD ^( AND_KYWD (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD28=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_leftAssociativeReduction618); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD28);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = AND_KYWD28;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD29=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_leftAssociativeReduction621); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD29);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = AND_KYWD29;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:148:31: (args+= . )+
					int cnt9=0;
					loop9:
					while (true) {
						int alt9=2;
						int LA9_0 = input.LA(1);
						if ( ((LA9_0 >= ABS_KYWD && LA9_0 <= XOR_KYWD)) ) {
							alt9=1;
						}

						switch (alt9) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:148:32: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt9 >= 1 ) break loop9;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(9, input);
							throw eee;
						}
						cnt9++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:148:43: (args+= . )+
					int cnt10=0;
					loop10:
					while (true) {
						int alt10=2;
						int LA10_0 = input.LA(1);
						if ( ((LA10_0 >= ABS_KYWD && LA10_0 <= XOR_KYWD)) ) {
							alt10=1;
						}

						switch (alt10) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:148:44: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt10 >= 1 ) break loop10;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(10, input);
							throw eee;
						}
						cnt10++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: AND_KYWD, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 148:58: -> ^( AND_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:148:61: ^( AND_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_AND_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:149:9: ^( OR_KYWD ^( OR_KYWD (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD30=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_leftAssociativeReduction661); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD30);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = OR_KYWD30;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD31=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_leftAssociativeReduction664); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD31);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = OR_KYWD31;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:149:29: (args+= . )+
					int cnt11=0;
					loop11:
					while (true) {
						int alt11=2;
						int LA11_0 = input.LA(1);
						if ( ((LA11_0 >= ABS_KYWD && LA11_0 <= XOR_KYWD)) ) {
							alt11=1;
						}

						switch (alt11) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:149:30: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt11 >= 1 ) break loop11;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(11, input);
							throw eee;
						}
						cnt11++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:149:41: (args+= . )+
					int cnt12=0;
					loop12:
					while (true) {
						int alt12=2;
						int LA12_0 = input.LA(1);
						if ( ((LA12_0 >= ABS_KYWD && LA12_0 <= XOR_KYWD)) ) {
							alt12=1;
						}

						switch (alt12) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:149:42: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt12 >= 1 ) break loop12;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(12, input);
							throw eee;
						}
						cnt12++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: OR_KYWD, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 149:58: -> ^( OR_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:149:61: ^( OR_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_OR_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:150:9: ^( PLUS ^( PLUS (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS32=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_leftAssociativeReduction706); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS32);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS32;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS33=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_leftAssociativeReduction709); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS33);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = PLUS33;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:150:23: (args+= . )+
					int cnt13=0;
					loop13:
					while (true) {
						int alt13=2;
						int LA13_0 = input.LA(1);
						if ( ((LA13_0 >= ABS_KYWD && LA13_0 <= XOR_KYWD)) ) {
							alt13=1;
						}

						switch (alt13) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:150:24: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt13 >= 1 ) break loop13;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(13, input);
							throw eee;
						}
						cnt13++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:150:35: (args+= . )+
					int cnt14=0;
					loop14:
					while (true) {
						int alt14=2;
						int LA14_0 = input.LA(1);
						if ( ((LA14_0 >= ABS_KYWD && LA14_0 <= XOR_KYWD)) ) {
							alt14=1;
						}

						switch (alt14) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:150:36: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt14 >= 1 ) break loop14;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(14, input);
							throw eee;
						}
						cnt14++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: PLUS, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 150:58: -> ^( PLUS ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:150:61: ^( PLUS ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_PLUS.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:151:9: ^( MINUS ^( MINUS args+= . (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MINUS34=(PlexilTreeNode)match(input,MINUS,FOLLOW_MINUS_in_leftAssociativeReduction757); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MINUS.add(MINUS34);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MINUS34;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MINUS35=(PlexilTreeNode)match(input,MINUS,FOLLOW_MINUS_in_leftAssociativeReduction760); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MINUS.add(MINUS35);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = MINUS35;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					args=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_2==null ) _first_2 = args;

					if (list_args==null) list_args=new ArrayList<Object>();
					list_args.add(args);
					// antlr/PlexilTreeTransforms.g:151:33: (args+= . )+
					int cnt15=0;
					loop15:
					while (true) {
						int alt15=2;
						int LA15_0 = input.LA(1);
						if ( ((LA15_0 >= ABS_KYWD && LA15_0 <= XOR_KYWD)) ) {
							alt15=1;
						}

						switch (alt15) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:151:34: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt15 >= 1 ) break loop15;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(15, input);
							throw eee;
						}
						cnt15++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:151:45: (args+= . )+
					int cnt16=0;
					loop16:
					while (true) {
						int alt16=2;
						int LA16_0 = input.LA(1);
						if ( ((LA16_0 >= ABS_KYWD && LA16_0 <= XOR_KYWD)) ) {
							alt16=1;
						}

						switch (alt16) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:151:46: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt16 >= 1 ) break loop16;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(16, input);
							throw eee;
						}
						cnt16++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: MINUS, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 151:58: -> ^( MINUS ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:151:61: ^( MINUS ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_MINUS.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:152:9: ^( ASTERISK ^( ASTERISK (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK36=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_leftAssociativeReduction802); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK36);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK36;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK37=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_leftAssociativeReduction805); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK37);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = ASTERISK37;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:152:31: (args+= . )+
					int cnt17=0;
					loop17:
					while (true) {
						int alt17=2;
						int LA17_0 = input.LA(1);
						if ( ((LA17_0 >= ABS_KYWD && LA17_0 <= XOR_KYWD)) ) {
							alt17=1;
						}

						switch (alt17) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:152:32: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt17 >= 1 ) break loop17;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(17, input);
							throw eee;
						}
						cnt17++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:152:43: (args+= . )+
					int cnt18=0;
					loop18:
					while (true) {
						int alt18=2;
						int LA18_0 = input.LA(1);
						if ( ((LA18_0 >= ABS_KYWD && LA18_0 <= XOR_KYWD)) ) {
							alt18=1;
						}

						switch (alt18) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:152:44: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt18 >= 1 ) break loop18;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(18, input);
							throw eee;
						}
						cnt18++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: ASTERISK, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 152:58: -> ^( ASTERISK ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:152:61: ^( ASTERISK ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_ASTERISK.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:153:9: ^( MAX_KYWD ^( MAX_KYWD (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MAX_KYWD38=(PlexilTreeNode)match(input,MAX_KYWD,FOLLOW_MAX_KYWD_in_leftAssociativeReduction845); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MAX_KYWD.add(MAX_KYWD38);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MAX_KYWD38;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MAX_KYWD39=(PlexilTreeNode)match(input,MAX_KYWD,FOLLOW_MAX_KYWD_in_leftAssociativeReduction848); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MAX_KYWD.add(MAX_KYWD39);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = MAX_KYWD39;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:153:31: (args+= . )+
					int cnt19=0;
					loop19:
					while (true) {
						int alt19=2;
						int LA19_0 = input.LA(1);
						if ( ((LA19_0 >= ABS_KYWD && LA19_0 <= XOR_KYWD)) ) {
							alt19=1;
						}

						switch (alt19) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:153:32: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt19 >= 1 ) break loop19;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(19, input);
							throw eee;
						}
						cnt19++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:153:43: (args+= . )+
					int cnt20=0;
					loop20:
					while (true) {
						int alt20=2;
						int LA20_0 = input.LA(1);
						if ( ((LA20_0 >= ABS_KYWD && LA20_0 <= XOR_KYWD)) ) {
							alt20=1;
						}

						switch (alt20) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:153:44: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt20 >= 1 ) break loop20;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(20, input);
							throw eee;
						}
						cnt20++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: args, MAX_KYWD
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 153:58: -> ^( MAX_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:153:61: ^( MAX_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_MAX_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 7 :
					// antlr/PlexilTreeTransforms.g:154:9: ^( MIN_KYWD ^( MIN_KYWD (args+= . )+ ) (args+= . )+ )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MIN_KYWD40=(PlexilTreeNode)match(input,MIN_KYWD,FOLLOW_MIN_KYWD_in_leftAssociativeReduction888); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MIN_KYWD.add(MIN_KYWD40);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MIN_KYWD40;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MIN_KYWD41=(PlexilTreeNode)match(input,MIN_KYWD,FOLLOW_MIN_KYWD_in_leftAssociativeReduction891); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MIN_KYWD.add(MIN_KYWD41);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = MIN_KYWD41;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					// antlr/PlexilTreeTransforms.g:154:31: (args+= . )+
					int cnt21=0;
					loop21:
					while (true) {
						int alt21=2;
						int LA21_0 = input.LA(1);
						if ( ((LA21_0 >= ABS_KYWD && LA21_0 <= XOR_KYWD)) ) {
							alt21=1;
						}

						switch (alt21) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:154:32: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_2==null ) _first_2 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt21 >= 1 ) break loop21;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(21, input);
							throw eee;
						}
						cnt21++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					// antlr/PlexilTreeTransforms.g:154:43: (args+= . )+
					int cnt22=0;
					loop22:
					while (true) {
						int alt22=2;
						int LA22_0 = input.LA(1);
						if ( ((LA22_0 >= ABS_KYWD && LA22_0 <= XOR_KYWD)) ) {
							alt22=1;
						}

						switch (alt22) {
						case 1 :
							// antlr/PlexilTreeTransforms.g:154:44: args+= .
							{
							_last = (PlexilTreeNode)input.LT(1);
							args=(PlexilTreeNode)input.LT(1);
							matchAny(input); if (state.failed) return retval;
							 
							if ( state.backtracking==1 )
							if ( _first_1==null ) _first_1 = args;

							if (list_args==null) list_args=new ArrayList<Object>();
							list_args.add(args);
							if ( state.backtracking==1 ) {
							retval.tree = _first_0;
							if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
								retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
							}

							}
							break;

						default :
							if ( cnt22 >= 1 ) break loop22;
							if (state.backtracking>0) {state.failed=true; return retval;}
							EarlyExitException eee = new EarlyExitException(22, input);
							throw eee;
						}
						cnt22++;
					}

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: MIN_KYWD, args
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: args
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_args=new RewriteRuleSubtreeStream(adaptor,"wildcard args",list_args);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 154:56: -> ^( MIN_KYWD ( $args)+ )
					{
						// antlr/PlexilTreeTransforms.g:154:59: ^( MIN_KYWD ( $args)+ )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_MIN_KYWD.nextNode()
						, root_1);
						if ( !(stream_args.hasNext()) ) {
							throw new RewriteEarlyExitException();
						}
						while ( stream_args.hasNext() ) {
							adaptor.addChild(root_1, stream_args.nextTree());
						}
						stream_args.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "leftAssociativeReduction"


	public static class trivialReduction_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "trivialReduction"
	// antlr/PlexilTreeTransforms.g:158:1: trivialReduction : ( ^( AND_KYWD arg= . ) -> $arg| ^( OR_KYWD arg= . ) -> $arg| ^( PLUS arg= . ) -> $arg| ^( MINUS arg= . ) -> $arg| ^( ASTERISK arg= . ) -> $arg| ^( SLASH arg= . ) -> $arg| ^( MAX_KYWD arg= . ) -> $arg| ^( MIN_KYWD arg= . ) -> $arg);
	public final PlexilTreeTransforms.trivialReduction_return trivialReduction() throws RecognitionException {
		PlexilTreeTransforms.trivialReduction_return retval = new PlexilTreeTransforms.trivialReduction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode AND_KYWD42=null;
		PlexilTreeNode OR_KYWD43=null;
		PlexilTreeNode PLUS44=null;
		PlexilTreeNode MINUS45=null;
		PlexilTreeNode ASTERISK46=null;
		PlexilTreeNode SLASH47=null;
		PlexilTreeNode MAX_KYWD48=null;
		PlexilTreeNode MIN_KYWD49=null;
		PlexilTreeNode arg=null;

		PlexilTreeNode AND_KYWD42_tree=null;
		PlexilTreeNode OR_KYWD43_tree=null;
		PlexilTreeNode PLUS44_tree=null;
		PlexilTreeNode MINUS45_tree=null;
		PlexilTreeNode ASTERISK46_tree=null;
		PlexilTreeNode SLASH47_tree=null;
		PlexilTreeNode MAX_KYWD48_tree=null;
		PlexilTreeNode MIN_KYWD49_tree=null;
		PlexilTreeNode arg_tree=null;
		RewriteRuleNodeStream stream_OR_KYWD=new RewriteRuleNodeStream(adaptor,"token OR_KYWD");
		RewriteRuleNodeStream stream_MIN_KYWD=new RewriteRuleNodeStream(adaptor,"token MIN_KYWD");
		RewriteRuleNodeStream stream_SLASH=new RewriteRuleNodeStream(adaptor,"token SLASH");
		RewriteRuleNodeStream stream_AND_KYWD=new RewriteRuleNodeStream(adaptor,"token AND_KYWD");
		RewriteRuleNodeStream stream_ASTERISK=new RewriteRuleNodeStream(adaptor,"token ASTERISK");
		RewriteRuleNodeStream stream_MAX_KYWD=new RewriteRuleNodeStream(adaptor,"token MAX_KYWD");
		RewriteRuleNodeStream stream_PLUS=new RewriteRuleNodeStream(adaptor,"token PLUS");
		RewriteRuleNodeStream stream_MINUS=new RewriteRuleNodeStream(adaptor,"token MINUS");

		try {
			// antlr/PlexilTreeTransforms.g:158:17: ( ^( AND_KYWD arg= . ) -> $arg| ^( OR_KYWD arg= . ) -> $arg| ^( PLUS arg= . ) -> $arg| ^( MINUS arg= . ) -> $arg| ^( ASTERISK arg= . ) -> $arg| ^( SLASH arg= . ) -> $arg| ^( MAX_KYWD arg= . ) -> $arg| ^( MIN_KYWD arg= . ) -> $arg)
			int alt24=8;
			switch ( input.LA(1) ) {
			case AND_KYWD:
				{
				alt24=1;
				}
				break;
			case OR_KYWD:
				{
				alt24=2;
				}
				break;
			case PLUS:
				{
				alt24=3;
				}
				break;
			case MINUS:
				{
				alt24=4;
				}
				break;
			case ASTERISK:
				{
				alt24=5;
				}
				break;
			case SLASH:
				{
				alt24=6;
				}
				break;
			case MAX_KYWD:
				{
				alt24=7;
				}
				break;
			case MIN_KYWD:
				{
				alt24=8;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 24, 0, input);
				throw nvae;
			}
			switch (alt24) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:159:9: ^( AND_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD42=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_trivialReduction940); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD42);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = AND_KYWD42;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 159:27: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:160:9: ^( OR_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD43=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_trivialReduction961); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD43);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = OR_KYWD43;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 160:26: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:161:9: ^( PLUS arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS44=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_trivialReduction982); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS44);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS44;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 161:23: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:162:9: ^( MINUS arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MINUS45=(PlexilTreeNode)match(input,MINUS,FOLLOW_MINUS_in_trivialReduction1003); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MINUS.add(MINUS45);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MINUS45;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 162:24: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:163:9: ^( ASTERISK arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK46=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_trivialReduction1024); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK46);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK46;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 163:27: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:164:9: ^( SLASH arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SLASH47=(PlexilTreeNode)match(input,SLASH,FOLLOW_SLASH_in_trivialReduction1045); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_SLASH.add(SLASH47);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = SLASH47;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 164:24: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 7 :
					// antlr/PlexilTreeTransforms.g:165:9: ^( MAX_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MAX_KYWD48=(PlexilTreeNode)match(input,MAX_KYWD,FOLLOW_MAX_KYWD_in_trivialReduction1066); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MAX_KYWD.add(MAX_KYWD48);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MAX_KYWD48;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 165:27: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 8 :
					// antlr/PlexilTreeTransforms.g:166:9: ^( MIN_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MIN_KYWD49=(PlexilTreeNode)match(input,MIN_KYWD,FOLLOW_MIN_KYWD_in_trivialReduction1087); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MIN_KYWD.add(MIN_KYWD49);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MIN_KYWD49;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 166:27: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "trivialReduction"


	public static class booleanIdentityElision_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "booleanIdentityElision"
	// antlr/PlexilTreeTransforms.g:173:1: booleanIdentityElision : ( ^( AND_KYWD TRUE_KYWD arg= . ) -> $arg| ^( AND_KYWD arg= . TRUE_KYWD ) -> $arg| ^( OR_KYWD FALSE_KYWD arg= . ) -> $arg| ^( OR_KYWD arg= . FALSE_KYWD ) -> $arg| ^( DEQUALS TRUE_KYWD arg= . ) -> $arg| ^( DEQUALS arg= . TRUE_KYWD ) -> $arg| ^( NEQUALS FALSE_KYWD arg= . ) -> $arg| ^( NEQUALS arg= . FALSE_KYWD ) -> $arg);
	public final PlexilTreeTransforms.booleanIdentityElision_return booleanIdentityElision() throws RecognitionException {
		PlexilTreeTransforms.booleanIdentityElision_return retval = new PlexilTreeTransforms.booleanIdentityElision_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode AND_KYWD50=null;
		PlexilTreeNode TRUE_KYWD51=null;
		PlexilTreeNode AND_KYWD52=null;
		PlexilTreeNode TRUE_KYWD53=null;
		PlexilTreeNode OR_KYWD54=null;
		PlexilTreeNode FALSE_KYWD55=null;
		PlexilTreeNode OR_KYWD56=null;
		PlexilTreeNode FALSE_KYWD57=null;
		PlexilTreeNode DEQUALS58=null;
		PlexilTreeNode TRUE_KYWD59=null;
		PlexilTreeNode DEQUALS60=null;
		PlexilTreeNode TRUE_KYWD61=null;
		PlexilTreeNode NEQUALS62=null;
		PlexilTreeNode FALSE_KYWD63=null;
		PlexilTreeNode NEQUALS64=null;
		PlexilTreeNode FALSE_KYWD65=null;
		PlexilTreeNode arg=null;

		PlexilTreeNode AND_KYWD50_tree=null;
		PlexilTreeNode TRUE_KYWD51_tree=null;
		PlexilTreeNode AND_KYWD52_tree=null;
		PlexilTreeNode TRUE_KYWD53_tree=null;
		PlexilTreeNode OR_KYWD54_tree=null;
		PlexilTreeNode FALSE_KYWD55_tree=null;
		PlexilTreeNode OR_KYWD56_tree=null;
		PlexilTreeNode FALSE_KYWD57_tree=null;
		PlexilTreeNode DEQUALS58_tree=null;
		PlexilTreeNode TRUE_KYWD59_tree=null;
		PlexilTreeNode DEQUALS60_tree=null;
		PlexilTreeNode TRUE_KYWD61_tree=null;
		PlexilTreeNode NEQUALS62_tree=null;
		PlexilTreeNode FALSE_KYWD63_tree=null;
		PlexilTreeNode NEQUALS64_tree=null;
		PlexilTreeNode FALSE_KYWD65_tree=null;
		PlexilTreeNode arg_tree=null;
		RewriteRuleNodeStream stream_OR_KYWD=new RewriteRuleNodeStream(adaptor,"token OR_KYWD");
		RewriteRuleNodeStream stream_DEQUALS=new RewriteRuleNodeStream(adaptor,"token DEQUALS");
		RewriteRuleNodeStream stream_FALSE_KYWD=new RewriteRuleNodeStream(adaptor,"token FALSE_KYWD");
		RewriteRuleNodeStream stream_AND_KYWD=new RewriteRuleNodeStream(adaptor,"token AND_KYWD");
		RewriteRuleNodeStream stream_NEQUALS=new RewriteRuleNodeStream(adaptor,"token NEQUALS");
		RewriteRuleNodeStream stream_TRUE_KYWD=new RewriteRuleNodeStream(adaptor,"token TRUE_KYWD");

		try {
			// antlr/PlexilTreeTransforms.g:173:23: ( ^( AND_KYWD TRUE_KYWD arg= . ) -> $arg| ^( AND_KYWD arg= . TRUE_KYWD ) -> $arg| ^( OR_KYWD FALSE_KYWD arg= . ) -> $arg| ^( OR_KYWD arg= . FALSE_KYWD ) -> $arg| ^( DEQUALS TRUE_KYWD arg= . ) -> $arg| ^( DEQUALS arg= . TRUE_KYWD ) -> $arg| ^( NEQUALS FALSE_KYWD arg= . ) -> $arg| ^( NEQUALS arg= . FALSE_KYWD ) -> $arg)
			int alt25=8;
			switch ( input.LA(1) ) {
			case AND_KYWD:
				{
				int LA25_1 = input.LA(2);
				if ( (LA25_1==DOWN) ) {
					int LA25_5 = input.LA(3);
					if ( (synpred50_PlexilTreeTransforms()) ) {
						alt25=1;
					}
					else if ( (synpred51_PlexilTreeTransforms()) ) {
						alt25=2;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 25, 5, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 25, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case OR_KYWD:
				{
				int LA25_2 = input.LA(2);
				if ( (LA25_2==DOWN) ) {
					int LA25_6 = input.LA(3);
					if ( (synpred52_PlexilTreeTransforms()) ) {
						alt25=3;
					}
					else if ( (synpred53_PlexilTreeTransforms()) ) {
						alt25=4;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 25, 6, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 25, 2, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case DEQUALS:
				{
				int LA25_3 = input.LA(2);
				if ( (LA25_3==DOWN) ) {
					int LA25_7 = input.LA(3);
					if ( (synpred54_PlexilTreeTransforms()) ) {
						alt25=5;
					}
					else if ( (synpred55_PlexilTreeTransforms()) ) {
						alt25=6;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 25, 7, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 25, 3, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case NEQUALS:
				{
				int LA25_4 = input.LA(2);
				if ( (LA25_4==DOWN) ) {
					int LA25_8 = input.LA(3);
					if ( (synpred56_PlexilTreeTransforms()) ) {
						alt25=7;
					}
					else if ( (true) ) {
						alt25=8;
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 25, 4, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 25, 0, input);
				throw nvae;
			}
			switch (alt25) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:174:9: ^( AND_KYWD TRUE_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD50=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_booleanIdentityElision1122); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD50);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = AND_KYWD50;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					TRUE_KYWD51=(PlexilTreeNode)match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_booleanIdentityElision1124); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_TRUE_KYWD.add(TRUE_KYWD51);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 174:37: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:175:9: ^( AND_KYWD arg= . TRUE_KYWD )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					AND_KYWD52=(PlexilTreeNode)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_booleanIdentityElision1145); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_AND_KYWD.add(AND_KYWD52);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = AND_KYWD52;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					TRUE_KYWD53=(PlexilTreeNode)match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_booleanIdentityElision1151); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_TRUE_KYWD.add(TRUE_KYWD53);

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 175:37: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:176:9: ^( OR_KYWD FALSE_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD54=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_booleanIdentityElision1168); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD54);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = OR_KYWD54;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					FALSE_KYWD55=(PlexilTreeNode)match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_booleanIdentityElision1170); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_FALSE_KYWD.add(FALSE_KYWD55);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 176:37: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:177:9: ^( OR_KYWD arg= . FALSE_KYWD )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					OR_KYWD56=(PlexilTreeNode)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_booleanIdentityElision1191); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_OR_KYWD.add(OR_KYWD56);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = OR_KYWD56;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					FALSE_KYWD57=(PlexilTreeNode)match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_booleanIdentityElision1197); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_FALSE_KYWD.add(FALSE_KYWD57);

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 177:37: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:178:9: ^( DEQUALS TRUE_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					DEQUALS58=(PlexilTreeNode)match(input,DEQUALS,FOLLOW_DEQUALS_in_booleanIdentityElision1214); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DEQUALS.add(DEQUALS58);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = DEQUALS58;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					TRUE_KYWD59=(PlexilTreeNode)match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_booleanIdentityElision1216); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_TRUE_KYWD.add(TRUE_KYWD59);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 178:36: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:179:9: ^( DEQUALS arg= . TRUE_KYWD )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					DEQUALS60=(PlexilTreeNode)match(input,DEQUALS,FOLLOW_DEQUALS_in_booleanIdentityElision1237); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DEQUALS.add(DEQUALS60);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = DEQUALS60;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					TRUE_KYWD61=(PlexilTreeNode)match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_booleanIdentityElision1243); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_TRUE_KYWD.add(TRUE_KYWD61);

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 179:36: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 7 :
					// antlr/PlexilTreeTransforms.g:180:9: ^( NEQUALS FALSE_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					NEQUALS62=(PlexilTreeNode)match(input,NEQUALS,FOLLOW_NEQUALS_in_booleanIdentityElision1260); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NEQUALS.add(NEQUALS62);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = NEQUALS62;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					FALSE_KYWD63=(PlexilTreeNode)match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_booleanIdentityElision1262); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_FALSE_KYWD.add(FALSE_KYWD63);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 180:37: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 8 :
					// antlr/PlexilTreeTransforms.g:181:9: ^( NEQUALS arg= . FALSE_KYWD )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					NEQUALS64=(PlexilTreeNode)match(input,NEQUALS,FOLLOW_NEQUALS_in_booleanIdentityElision1283); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NEQUALS.add(NEQUALS64);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = NEQUALS64;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					FALSE_KYWD65=(PlexilTreeNode)match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_booleanIdentityElision1289); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_FALSE_KYWD.add(FALSE_KYWD65);

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 181:37: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "booleanIdentityElision"


	public static class integerIdentityElision_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "integerIdentityElision"
	// antlr/PlexilTreeTransforms.g:184:1: integerIdentityElision : ( ^( PLUS i= INT arg= . {...}?) -> $arg| ^( PLUS arg= . i= INT {...}?) -> $arg| ^( MINUS arg= . i= INT {...}?) -> $arg| ^( ASTERISK i= INT arg= . {...}?) -> $arg| ^( ASTERISK arg= . i= INT {...}?) -> $arg| ^( SLASH arg= . i= INT {...}?) -> $arg);
	public final PlexilTreeTransforms.integerIdentityElision_return integerIdentityElision() throws RecognitionException {
		PlexilTreeTransforms.integerIdentityElision_return retval = new PlexilTreeTransforms.integerIdentityElision_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode i=null;
		PlexilTreeNode PLUS66=null;
		PlexilTreeNode PLUS67=null;
		PlexilTreeNode MINUS68=null;
		PlexilTreeNode ASTERISK69=null;
		PlexilTreeNode ASTERISK70=null;
		PlexilTreeNode SLASH71=null;
		PlexilTreeNode arg=null;

		PlexilTreeNode i_tree=null;
		PlexilTreeNode PLUS66_tree=null;
		PlexilTreeNode PLUS67_tree=null;
		PlexilTreeNode MINUS68_tree=null;
		PlexilTreeNode ASTERISK69_tree=null;
		PlexilTreeNode ASTERISK70_tree=null;
		PlexilTreeNode SLASH71_tree=null;
		PlexilTreeNode arg_tree=null;
		RewriteRuleNodeStream stream_SLASH=new RewriteRuleNodeStream(adaptor,"token SLASH");
		RewriteRuleNodeStream stream_ASTERISK=new RewriteRuleNodeStream(adaptor,"token ASTERISK");
		RewriteRuleNodeStream stream_INT=new RewriteRuleNodeStream(adaptor,"token INT");
		RewriteRuleNodeStream stream_PLUS=new RewriteRuleNodeStream(adaptor,"token PLUS");
		RewriteRuleNodeStream stream_MINUS=new RewriteRuleNodeStream(adaptor,"token MINUS");

		try {
			// antlr/PlexilTreeTransforms.g:184:23: ( ^( PLUS i= INT arg= . {...}?) -> $arg| ^( PLUS arg= . i= INT {...}?) -> $arg| ^( MINUS arg= . i= INT {...}?) -> $arg| ^( ASTERISK i= INT arg= . {...}?) -> $arg| ^( ASTERISK arg= . i= INT {...}?) -> $arg| ^( SLASH arg= . i= INT {...}?) -> $arg)
			int alt26=6;
			switch ( input.LA(1) ) {
			case PLUS:
				{
				int LA26_1 = input.LA(2);
				if ( (LA26_1==DOWN) ) {
					int LA26_5 = input.LA(3);
					if ( (synpred57_PlexilTreeTransforms()) ) {
						alt26=1;
					}
					else if ( (synpred58_PlexilTreeTransforms()) ) {
						alt26=2;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 26, 5, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 26, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case MINUS:
				{
				alt26=3;
				}
				break;
			case ASTERISK:
				{
				int LA26_3 = input.LA(2);
				if ( (LA26_3==DOWN) ) {
					int LA26_6 = input.LA(3);
					if ( (synpred60_PlexilTreeTransforms()) ) {
						alt26=4;
					}
					else if ( (synpred61_PlexilTreeTransforms()) ) {
						alt26=5;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 26, 6, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 26, 3, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case SLASH:
				{
				alt26=6;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 26, 0, input);
				throw nvae;
			}
			switch (alt26) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:185:9: ^( PLUS i= INT arg= . {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS66=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_integerIdentityElision1320); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS66);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS66;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_integerIdentityElision1324); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_INT.add(i);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					if ( !((Integer.valueOf((i!=null?i.getText():null)) == 0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "integerIdentityElision", "Integer.valueOf($i.text) == 0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 185:62: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:186:9: ^( PLUS arg= . i= INT {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS67=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_integerIdentityElision1347); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS67);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS67;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_integerIdentityElision1355); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_INT.add(i);

					if ( !((Integer.valueOf((i!=null?i.getText():null)) == 0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "integerIdentityElision", "Integer.valueOf($i.text) == 0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 186:62: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:187:9: ^( MINUS arg= . i= INT {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MINUS68=(PlexilTreeNode)match(input,MINUS,FOLLOW_MINUS_in_integerIdentityElision1374); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MINUS.add(MINUS68);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MINUS68;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_integerIdentityElision1382); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_INT.add(i);

					if ( !((Integer.valueOf((i!=null?i.getText():null)) == 0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "integerIdentityElision", "Integer.valueOf($i.text) == 0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 187:63: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:188:9: ^( ASTERISK i= INT arg= . {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK69=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_integerIdentityElision1401); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK69);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK69;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_integerIdentityElision1405); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_INT.add(i);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					if ( !((Integer.valueOf((i!=null?i.getText():null)) == 1)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "integerIdentityElision", "Integer.valueOf($i.text) == 1");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 188:66: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:189:9: ^( ASTERISK arg= . i= INT {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK70=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_integerIdentityElision1428); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK70);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK70;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_integerIdentityElision1436); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_INT.add(i);

					if ( !((Integer.valueOf((i!=null?i.getText():null)) == 1)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "integerIdentityElision", "Integer.valueOf($i.text) == 1");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 189:66: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:190:9: ^( SLASH arg= . i= INT {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SLASH71=(PlexilTreeNode)match(input,SLASH,FOLLOW_SLASH_in_integerIdentityElision1455); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_SLASH.add(SLASH71);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = SLASH71;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_integerIdentityElision1463); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_INT.add(i);

					if ( !((Integer.valueOf((i!=null?i.getText():null)) == 1)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "integerIdentityElision", "Integer.valueOf($i.text) == 1");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 190:63: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "integerIdentityElision"


	public static class realIdentityElision_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "realIdentityElision"
	// antlr/PlexilTreeTransforms.g:193:1: realIdentityElision : ( ^( PLUS i= DOUBLE arg= . {...}?) -> $arg| ^( PLUS arg= . i= DOUBLE {...}?) -> $arg| ^( MINUS arg= . i= DOUBLE {...}?) -> $arg| ^( ASTERISK i= DOUBLE arg= . {...}?) -> $arg| ^( ASTERISK arg= . i= DOUBLE {...}?) -> $arg| ^( SLASH arg= . i= DOUBLE {...}?) -> $arg);
	public final PlexilTreeTransforms.realIdentityElision_return realIdentityElision() throws RecognitionException {
		PlexilTreeTransforms.realIdentityElision_return retval = new PlexilTreeTransforms.realIdentityElision_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode i=null;
		PlexilTreeNode PLUS72=null;
		PlexilTreeNode PLUS73=null;
		PlexilTreeNode MINUS74=null;
		PlexilTreeNode ASTERISK75=null;
		PlexilTreeNode ASTERISK76=null;
		PlexilTreeNode SLASH77=null;
		PlexilTreeNode arg=null;

		PlexilTreeNode i_tree=null;
		PlexilTreeNode PLUS72_tree=null;
		PlexilTreeNode PLUS73_tree=null;
		PlexilTreeNode MINUS74_tree=null;
		PlexilTreeNode ASTERISK75_tree=null;
		PlexilTreeNode ASTERISK76_tree=null;
		PlexilTreeNode SLASH77_tree=null;
		PlexilTreeNode arg_tree=null;
		RewriteRuleNodeStream stream_SLASH=new RewriteRuleNodeStream(adaptor,"token SLASH");
		RewriteRuleNodeStream stream_ASTERISK=new RewriteRuleNodeStream(adaptor,"token ASTERISK");
		RewriteRuleNodeStream stream_DOUBLE=new RewriteRuleNodeStream(adaptor,"token DOUBLE");
		RewriteRuleNodeStream stream_PLUS=new RewriteRuleNodeStream(adaptor,"token PLUS");
		RewriteRuleNodeStream stream_MINUS=new RewriteRuleNodeStream(adaptor,"token MINUS");

		try {
			// antlr/PlexilTreeTransforms.g:193:20: ( ^( PLUS i= DOUBLE arg= . {...}?) -> $arg| ^( PLUS arg= . i= DOUBLE {...}?) -> $arg| ^( MINUS arg= . i= DOUBLE {...}?) -> $arg| ^( ASTERISK i= DOUBLE arg= . {...}?) -> $arg| ^( ASTERISK arg= . i= DOUBLE {...}?) -> $arg| ^( SLASH arg= . i= DOUBLE {...}?) -> $arg)
			int alt27=6;
			switch ( input.LA(1) ) {
			case PLUS:
				{
				int LA27_1 = input.LA(2);
				if ( (LA27_1==DOWN) ) {
					int LA27_5 = input.LA(3);
					if ( (synpred62_PlexilTreeTransforms()) ) {
						alt27=1;
					}
					else if ( (synpred63_PlexilTreeTransforms()) ) {
						alt27=2;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 27, 5, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 27, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case MINUS:
				{
				alt27=3;
				}
				break;
			case ASTERISK:
				{
				int LA27_3 = input.LA(2);
				if ( (LA27_3==DOWN) ) {
					int LA27_6 = input.LA(3);
					if ( (synpred65_PlexilTreeTransforms()) ) {
						alt27=4;
					}
					else if ( (synpred66_PlexilTreeTransforms()) ) {
						alt27=5;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 27, 6, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 27, 3, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case SLASH:
				{
				alt27=6;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 27, 0, input);
				throw nvae;
			}
			switch (alt27) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:194:9: ^( PLUS i= DOUBLE arg= . {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS72=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_realIdentityElision1492); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS72);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS72;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_realIdentityElision1496); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DOUBLE.add(i);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					if ( !((Double.valueOf((i!=null?i.getText():null)) == 0.0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "realIdentityElision", "Double.valueOf($i.text) == 0.0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 194:66: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:195:9: ^( PLUS arg= . i= DOUBLE {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					PLUS73=(PlexilTreeNode)match(input,PLUS,FOLLOW_PLUS_in_realIdentityElision1519); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_PLUS.add(PLUS73);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = PLUS73;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_realIdentityElision1527); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DOUBLE.add(i);

					if ( !((Double.valueOf((i!=null?i.getText():null)) == 0.0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "realIdentityElision", "Double.valueOf($i.text) == 0.0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 195:66: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:196:9: ^( MINUS arg= . i= DOUBLE {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					MINUS74=(PlexilTreeNode)match(input,MINUS,FOLLOW_MINUS_in_realIdentityElision1546); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_MINUS.add(MINUS74);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = MINUS74;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_realIdentityElision1554); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DOUBLE.add(i);

					if ( !((Double.valueOf((i!=null?i.getText():null)) == 0.0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "realIdentityElision", "Double.valueOf($i.text) == 0.0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 196:67: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:197:9: ^( ASTERISK i= DOUBLE arg= . {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK75=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_realIdentityElision1573); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK75);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK75;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_realIdentityElision1577); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DOUBLE.add(i);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					if ( !((Double.valueOf((i!=null?i.getText():null)) == 1.0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "realIdentityElision", "Double.valueOf($i.text) == 1.0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 197:70: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 5 :
					// antlr/PlexilTreeTransforms.g:198:9: ^( ASTERISK arg= . i= DOUBLE {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ASTERISK76=(PlexilTreeNode)match(input,ASTERISK,FOLLOW_ASTERISK_in_realIdentityElision1600); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ASTERISK.add(ASTERISK76);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ASTERISK76;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_realIdentityElision1608); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DOUBLE.add(i);

					if ( !((Double.valueOf((i!=null?i.getText():null)) == 1.0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "realIdentityElision", "Double.valueOf($i.text) == 1.0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 198:70: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 6 :
					// antlr/PlexilTreeTransforms.g:199:9: ^( SLASH arg= . i= DOUBLE {...}?)
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SLASH77=(PlexilTreeNode)match(input,SLASH,FOLLOW_SLASH_in_realIdentityElision1627); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_SLASH.add(SLASH77);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = SLASH77;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_realIdentityElision1635); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DOUBLE.add(i);

					if ( !((Double.valueOf((i!=null?i.getText():null)) == 1.0)) ) {
						if (state.backtracking>0) {state.failed=true; return retval;}
						throw new FailedPredicateException(input, "realIdentityElision", "Double.valueOf($i.text) == 1.0");
					}
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 199:67: -> $arg
					{
						adaptor.addChild(root_0, stream_arg.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "realIdentityElision"


	public static class booleanEqualityNegation_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "booleanEqualityNegation"
	// antlr/PlexilTreeTransforms.g:202:1: booleanEqualityNegation : ( ^( DEQUALS FALSE_KYWD arg= . ) -> ^( NOT_KYWD $arg) | ^( DEQUALS arg= . FALSE_KYWD ) -> ^( NOT_KYWD $arg) | ^( NEQUALS TRUE_KYWD arg= . ) -> ^( NOT_KYWD $arg) | ^( NEQUALS arg= . TRUE_KYWD ) -> ^( NOT_KYWD $arg) );
	public final PlexilTreeTransforms.booleanEqualityNegation_return booleanEqualityNegation() throws RecognitionException {
		PlexilTreeTransforms.booleanEqualityNegation_return retval = new PlexilTreeTransforms.booleanEqualityNegation_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode DEQUALS78=null;
		PlexilTreeNode FALSE_KYWD79=null;
		PlexilTreeNode DEQUALS80=null;
		PlexilTreeNode FALSE_KYWD81=null;
		PlexilTreeNode NEQUALS82=null;
		PlexilTreeNode TRUE_KYWD83=null;
		PlexilTreeNode NEQUALS84=null;
		PlexilTreeNode TRUE_KYWD85=null;
		PlexilTreeNode arg=null;

		PlexilTreeNode DEQUALS78_tree=null;
		PlexilTreeNode FALSE_KYWD79_tree=null;
		PlexilTreeNode DEQUALS80_tree=null;
		PlexilTreeNode FALSE_KYWD81_tree=null;
		PlexilTreeNode NEQUALS82_tree=null;
		PlexilTreeNode TRUE_KYWD83_tree=null;
		PlexilTreeNode NEQUALS84_tree=null;
		PlexilTreeNode TRUE_KYWD85_tree=null;
		PlexilTreeNode arg_tree=null;
		RewriteRuleNodeStream stream_DEQUALS=new RewriteRuleNodeStream(adaptor,"token DEQUALS");
		RewriteRuleNodeStream stream_FALSE_KYWD=new RewriteRuleNodeStream(adaptor,"token FALSE_KYWD");
		RewriteRuleNodeStream stream_NEQUALS=new RewriteRuleNodeStream(adaptor,"token NEQUALS");
		RewriteRuleNodeStream stream_TRUE_KYWD=new RewriteRuleNodeStream(adaptor,"token TRUE_KYWD");

		try {
			// antlr/PlexilTreeTransforms.g:202:24: ( ^( DEQUALS FALSE_KYWD arg= . ) -> ^( NOT_KYWD $arg) | ^( DEQUALS arg= . FALSE_KYWD ) -> ^( NOT_KYWD $arg) | ^( NEQUALS TRUE_KYWD arg= . ) -> ^( NOT_KYWD $arg) | ^( NEQUALS arg= . TRUE_KYWD ) -> ^( NOT_KYWD $arg) )
			int alt28=4;
			int LA28_0 = input.LA(1);
			if ( (LA28_0==DEQUALS) ) {
				int LA28_1 = input.LA(2);
				if ( (LA28_1==DOWN) ) {
					int LA28_3 = input.LA(3);
					if ( (synpred67_PlexilTreeTransforms()) ) {
						alt28=1;
					}
					else if ( (synpred68_PlexilTreeTransforms()) ) {
						alt28=2;
					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						int nvaeMark = input.mark();
						try {
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 28, 3, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 28, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}
			else if ( (LA28_0==NEQUALS) ) {
				int LA28_2 = input.LA(2);
				if ( (LA28_2==DOWN) ) {
					int LA28_4 = input.LA(3);
					if ( (synpred69_PlexilTreeTransforms()) ) {
						alt28=3;
					}
					else if ( (true) ) {
						alt28=4;
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 28, 2, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 28, 0, input);
				throw nvae;
			}

			switch (alt28) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:203:9: ^( DEQUALS FALSE_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					DEQUALS78=(PlexilTreeNode)match(input,DEQUALS,FOLLOW_DEQUALS_in_booleanEqualityNegation1664); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DEQUALS.add(DEQUALS78);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = DEQUALS78;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					FALSE_KYWD79=(PlexilTreeNode)match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_booleanEqualityNegation1666); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_FALSE_KYWD.add(FALSE_KYWD79);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 203:37: -> ^( NOT_KYWD $arg)
					{
						// antlr/PlexilTreeTransforms.g:203:40: ^( NOT_KYWD $arg)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NOT_KYWD, "NOT_KYWD"), root_1);
						adaptor.addChild(root_1, stream_arg.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:204:9: ^( DEQUALS arg= . FALSE_KYWD )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					DEQUALS80=(PlexilTreeNode)match(input,DEQUALS,FOLLOW_DEQUALS_in_booleanEqualityNegation1691); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_DEQUALS.add(DEQUALS80);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = DEQUALS80;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					FALSE_KYWD81=(PlexilTreeNode)match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_booleanEqualityNegation1697); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_FALSE_KYWD.add(FALSE_KYWD81);

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 204:37: -> ^( NOT_KYWD $arg)
					{
						// antlr/PlexilTreeTransforms.g:204:40: ^( NOT_KYWD $arg)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NOT_KYWD, "NOT_KYWD"), root_1);
						adaptor.addChild(root_1, stream_arg.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:205:9: ^( NEQUALS TRUE_KYWD arg= . )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					NEQUALS82=(PlexilTreeNode)match(input,NEQUALS,FOLLOW_NEQUALS_in_booleanEqualityNegation1718); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NEQUALS.add(NEQUALS82);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = NEQUALS82;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					TRUE_KYWD83=(PlexilTreeNode)match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_booleanEqualityNegation1720); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_TRUE_KYWD.add(TRUE_KYWD83);

					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 205:37: -> ^( NOT_KYWD $arg)
					{
						// antlr/PlexilTreeTransforms.g:205:40: ^( NOT_KYWD $arg)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NOT_KYWD, "NOT_KYWD"), root_1);
						adaptor.addChild(root_1, stream_arg.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:206:9: ^( NEQUALS arg= . TRUE_KYWD )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					NEQUALS84=(PlexilTreeNode)match(input,NEQUALS,FOLLOW_NEQUALS_in_booleanEqualityNegation1746); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NEQUALS.add(NEQUALS84);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = NEQUALS84;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					arg=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = arg;

					_last = (PlexilTreeNode)input.LT(1);
					TRUE_KYWD85=(PlexilTreeNode)match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_booleanEqualityNegation1752); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_TRUE_KYWD.add(TRUE_KYWD85);

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: arg
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: arg
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_arg=new RewriteRuleSubtreeStream(adaptor,"wildcard arg",arg);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 206:37: -> ^( NOT_KYWD $arg)
					{
						// antlr/PlexilTreeTransforms.g:206:40: ^( NOT_KYWD $arg)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NOT_KYWD, "NOT_KYWD"), root_1);
						adaptor.addChild(root_1, stream_arg.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "booleanEqualityNegation"


	public static class flattenTrivialBlocks_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "flattenTrivialBlocks"
	// antlr/PlexilTreeTransforms.g:211:1: flattenTrivialBlocks : ( ^( ACTION ^( BLOCK innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( BLOCK innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( BLOCK namedAction ) ) | ^( ACTION outerId= NCNAME ^( BLOCK ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) );
	public final PlexilTreeTransforms.flattenTrivialBlocks_return flattenTrivialBlocks() throws RecognitionException {
		PlexilTreeTransforms.flattenTrivialBlocks_return retval = new PlexilTreeTransforms.flattenTrivialBlocks_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode outerId=null;
		PlexilTreeNode ACTION86=null;
		PlexilTreeNode BLOCK87=null;
		PlexilTreeNode ACTION88=null;
		PlexilTreeNode BLOCK89=null;
		PlexilTreeNode ACTION90=null;
		PlexilTreeNode NCNAME91=null;
		PlexilTreeNode BLOCK92=null;
		PlexilTreeNode ACTION94=null;
		PlexilTreeNode BLOCK95=null;
		PlexilTreeNode ACTION96=null;
		PlexilTreeNode body=null;
		TreeRuleReturnScope innerUnnamed =null;
		TreeRuleReturnScope innerNamed =null;
		TreeRuleReturnScope namedAction93 =null;

		PlexilTreeNode outerId_tree=null;
		PlexilTreeNode ACTION86_tree=null;
		PlexilTreeNode BLOCK87_tree=null;
		PlexilTreeNode ACTION88_tree=null;
		PlexilTreeNode BLOCK89_tree=null;
		PlexilTreeNode ACTION90_tree=null;
		PlexilTreeNode NCNAME91_tree=null;
		PlexilTreeNode BLOCK92_tree=null;
		PlexilTreeNode ACTION94_tree=null;
		PlexilTreeNode BLOCK95_tree=null;
		PlexilTreeNode ACTION96_tree=null;
		PlexilTreeNode body_tree=null;
		RewriteRuleNodeStream stream_ACTION=new RewriteRuleNodeStream(adaptor,"token ACTION");
		RewriteRuleNodeStream stream_BLOCK=new RewriteRuleNodeStream(adaptor,"token BLOCK");
		RewriteRuleNodeStream stream_NCNAME=new RewriteRuleNodeStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_namedAction=new RewriteRuleSubtreeStream(adaptor,"rule namedAction");
		RewriteRuleSubtreeStream stream_unnamedAction=new RewriteRuleSubtreeStream(adaptor,"rule unnamedAction");

		try {
			// antlr/PlexilTreeTransforms.g:211:21: ( ^( ACTION ^( BLOCK innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( BLOCK innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( BLOCK namedAction ) ) | ^( ACTION outerId= NCNAME ^( BLOCK ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) )
			int alt29=4;
			int LA29_0 = input.LA(1);
			if ( (LA29_0==ACTION) ) {
				int LA29_1 = input.LA(2);
				if ( (LA29_1==DOWN) ) {
					int LA29_2 = input.LA(3);
					if ( (synpred70_PlexilTreeTransforms()) ) {
						alt29=1;
					}
					else if ( (synpred71_PlexilTreeTransforms()) ) {
						alt29=2;
					}
					else if ( (synpred72_PlexilTreeTransforms()) ) {
						alt29=3;
					}
					else if ( (true) ) {
						alt29=4;
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 29, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 29, 0, input);
				throw nvae;
			}

			switch (alt29) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:212:9: ^( ACTION ^( BLOCK innerUnnamed= unnamedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION86=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialBlocks1794); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION86);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION86;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					BLOCK87=(PlexilTreeNode)match(input,BLOCK,FOLLOW_BLOCK_in_flattenTrivialBlocks1797); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_BLOCK.add(BLOCK87);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = BLOCK87;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_unnamedAction_in_flattenTrivialBlocks1801);
					innerUnnamed=unnamedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_unnamedAction.add(innerUnnamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerUnnamed
					// token labels: 
					// rule labels: innerUnnamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerUnnamed=new RewriteRuleSubtreeStream(adaptor,"rule innerUnnamed",innerUnnamed!=null?innerUnnamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 213:9: -> $innerUnnamed
					{
						adaptor.addChild(root_0, stream_innerUnnamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:214:9: ^( ACTION ^( BLOCK innerNamed= namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION88=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialBlocks1827); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION88);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION88;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					BLOCK89=(PlexilTreeNode)match(input,BLOCK,FOLLOW_BLOCK_in_flattenTrivialBlocks1830); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_BLOCK.add(BLOCK89);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = BLOCK89;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialBlocks1834);
					innerNamed=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_namedAction.add(innerNamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerNamed
					// token labels: 
					// rule labels: innerNamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerNamed=new RewriteRuleSubtreeStream(adaptor,"rule innerNamed",innerNamed!=null?innerNamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 215:9: -> $innerNamed
					{
						adaptor.addChild(root_0, stream_innerNamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:216:9: ^( ACTION NCNAME ^( BLOCK namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION90=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialBlocks1860); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION90;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					NCNAME91=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialBlocks1862); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = NCNAME91;

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					BLOCK92=(PlexilTreeNode)match(input,BLOCK,FOLLOW_BLOCK_in_flattenTrivialBlocks1865); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = BLOCK92;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialBlocks1867);
					namedAction93=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_2==null ) _first_2 = (PlexilTreeNode)namedAction93.getTree();

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:217:9: ^( ACTION outerId= NCNAME ^( BLOCK ^( ACTION body= . ) ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION94=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialBlocks1881); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION94);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION94;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					outerId=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialBlocks1885); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NCNAME.add(outerId);

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					BLOCK95=(PlexilTreeNode)match(input,BLOCK,FOLLOW_BLOCK_in_flattenTrivialBlocks1888); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_BLOCK.add(BLOCK95);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = BLOCK95;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_3 = _last;
					PlexilTreeNode _first_3 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION96=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialBlocks1891); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION96);

					if ( state.backtracking==1 )
					if ( _first_2==null ) _first_2 = ACTION96;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					body=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_3==null ) _first_3 = body;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_3;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: ACTION, body, outerId
					// token labels: outerId
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: body
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleNodeStream stream_outerId=new RewriteRuleNodeStream(adaptor,"token outerId",outerId);
					RewriteRuleSubtreeStream stream_body=new RewriteRuleSubtreeStream(adaptor,"wildcard body",body);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 218:9: -> ^( ACTION $outerId $body)
					{
						// antlr/PlexilTreeTransforms.g:218:12: ^( ACTION $outerId $body)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_ACTION.nextNode()
						, root_1);
						adaptor.addChild(root_1, stream_outerId.nextNode());
						adaptor.addChild(root_1, stream_body.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "flattenTrivialBlocks"


	public static class flattenTrivialSequences_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "flattenTrivialSequences"
	// antlr/PlexilTreeTransforms.g:221:1: flattenTrivialSequences : ( ^( ACTION ^( SEQUENCE_KYWD innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( SEQUENCE_KYWD innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( SEQUENCE_KYWD namedAction ) ) | ^( ACTION outerId= NCNAME ^( SEQUENCE_KYWD ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) );
	public final PlexilTreeTransforms.flattenTrivialSequences_return flattenTrivialSequences() throws RecognitionException {
		PlexilTreeTransforms.flattenTrivialSequences_return retval = new PlexilTreeTransforms.flattenTrivialSequences_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode outerId=null;
		PlexilTreeNode ACTION97=null;
		PlexilTreeNode SEQUENCE_KYWD98=null;
		PlexilTreeNode ACTION99=null;
		PlexilTreeNode SEQUENCE_KYWD100=null;
		PlexilTreeNode ACTION101=null;
		PlexilTreeNode NCNAME102=null;
		PlexilTreeNode SEQUENCE_KYWD103=null;
		PlexilTreeNode ACTION105=null;
		PlexilTreeNode SEQUENCE_KYWD106=null;
		PlexilTreeNode ACTION107=null;
		PlexilTreeNode body=null;
		TreeRuleReturnScope innerUnnamed =null;
		TreeRuleReturnScope innerNamed =null;
		TreeRuleReturnScope namedAction104 =null;

		PlexilTreeNode outerId_tree=null;
		PlexilTreeNode ACTION97_tree=null;
		PlexilTreeNode SEQUENCE_KYWD98_tree=null;
		PlexilTreeNode ACTION99_tree=null;
		PlexilTreeNode SEQUENCE_KYWD100_tree=null;
		PlexilTreeNode ACTION101_tree=null;
		PlexilTreeNode NCNAME102_tree=null;
		PlexilTreeNode SEQUENCE_KYWD103_tree=null;
		PlexilTreeNode ACTION105_tree=null;
		PlexilTreeNode SEQUENCE_KYWD106_tree=null;
		PlexilTreeNode ACTION107_tree=null;
		PlexilTreeNode body_tree=null;
		RewriteRuleNodeStream stream_ACTION=new RewriteRuleNodeStream(adaptor,"token ACTION");
		RewriteRuleNodeStream stream_SEQUENCE_KYWD=new RewriteRuleNodeStream(adaptor,"token SEQUENCE_KYWD");
		RewriteRuleNodeStream stream_NCNAME=new RewriteRuleNodeStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_namedAction=new RewriteRuleSubtreeStream(adaptor,"rule namedAction");
		RewriteRuleSubtreeStream stream_unnamedAction=new RewriteRuleSubtreeStream(adaptor,"rule unnamedAction");

		try {
			// antlr/PlexilTreeTransforms.g:221:24: ( ^( ACTION ^( SEQUENCE_KYWD innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( SEQUENCE_KYWD innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( SEQUENCE_KYWD namedAction ) ) | ^( ACTION outerId= NCNAME ^( SEQUENCE_KYWD ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) )
			int alt30=4;
			int LA30_0 = input.LA(1);
			if ( (LA30_0==ACTION) ) {
				int LA30_1 = input.LA(2);
				if ( (LA30_1==DOWN) ) {
					int LA30_2 = input.LA(3);
					if ( (synpred73_PlexilTreeTransforms()) ) {
						alt30=1;
					}
					else if ( (synpred74_PlexilTreeTransforms()) ) {
						alt30=2;
					}
					else if ( (synpred75_PlexilTreeTransforms()) ) {
						alt30=3;
					}
					else if ( (true) ) {
						alt30=4;
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 30, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 30, 0, input);
				throw nvae;
			}

			switch (alt30) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:222:9: ^( ACTION ^( SEQUENCE_KYWD innerUnnamed= unnamedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION97=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialSequences1939); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION97);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION97;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SEQUENCE_KYWD98=(PlexilTreeNode)match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences1942); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_SEQUENCE_KYWD.add(SEQUENCE_KYWD98);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = SEQUENCE_KYWD98;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_unnamedAction_in_flattenTrivialSequences1946);
					innerUnnamed=unnamedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_unnamedAction.add(innerUnnamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerUnnamed
					// token labels: 
					// rule labels: innerUnnamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerUnnamed=new RewriteRuleSubtreeStream(adaptor,"rule innerUnnamed",innerUnnamed!=null?innerUnnamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 223:9: -> $innerUnnamed
					{
						adaptor.addChild(root_0, stream_innerUnnamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:224:9: ^( ACTION ^( SEQUENCE_KYWD innerNamed= namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION99=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialSequences1972); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION99);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION99;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SEQUENCE_KYWD100=(PlexilTreeNode)match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences1975); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_SEQUENCE_KYWD.add(SEQUENCE_KYWD100);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = SEQUENCE_KYWD100;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialSequences1979);
					innerNamed=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_namedAction.add(innerNamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerNamed
					// token labels: 
					// rule labels: innerNamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerNamed=new RewriteRuleSubtreeStream(adaptor,"rule innerNamed",innerNamed!=null?innerNamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 225:9: -> $innerNamed
					{
						adaptor.addChild(root_0, stream_innerNamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:226:9: ^( ACTION NCNAME ^( SEQUENCE_KYWD namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION101=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialSequences2005); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION101;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					NCNAME102=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialSequences2007); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = NCNAME102;

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SEQUENCE_KYWD103=(PlexilTreeNode)match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences2010); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = SEQUENCE_KYWD103;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialSequences2012);
					namedAction104=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_2==null ) _first_2 = (PlexilTreeNode)namedAction104.getTree();

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:227:9: ^( ACTION outerId= NCNAME ^( SEQUENCE_KYWD ^( ACTION body= . ) ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION105=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialSequences2026); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION105);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION105;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					outerId=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialSequences2030); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NCNAME.add(outerId);

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					SEQUENCE_KYWD106=(PlexilTreeNode)match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences2033); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_SEQUENCE_KYWD.add(SEQUENCE_KYWD106);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = SEQUENCE_KYWD106;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_3 = _last;
					PlexilTreeNode _first_3 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION107=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialSequences2036); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION107);

					if ( state.backtracking==1 )
					if ( _first_2==null ) _first_2 = ACTION107;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					body=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_3==null ) _first_3 = body;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_3;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: outerId, ACTION, body
					// token labels: outerId
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: body
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleNodeStream stream_outerId=new RewriteRuleNodeStream(adaptor,"token outerId",outerId);
					RewriteRuleSubtreeStream stream_body=new RewriteRuleSubtreeStream(adaptor,"wildcard body",body);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 228:9: -> ^( ACTION $outerId $body)
					{
						// antlr/PlexilTreeTransforms.g:228:12: ^( ACTION $outerId $body)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_ACTION.nextNode()
						, root_1);
						adaptor.addChild(root_1, stream_outerId.nextNode());
						adaptor.addChild(root_1, stream_body.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "flattenTrivialSequences"


	public static class flattenTrivialUncheckedSequences_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "flattenTrivialUncheckedSequences"
	// antlr/PlexilTreeTransforms.g:231:1: flattenTrivialUncheckedSequences : ( ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( UNCHECKED_SEQUENCE_KYWD namedAction ) ) | ^( ACTION outerId= NCNAME ^( UNCHECKED_SEQUENCE_KYWD ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) );
	public final PlexilTreeTransforms.flattenTrivialUncheckedSequences_return flattenTrivialUncheckedSequences() throws RecognitionException {
		PlexilTreeTransforms.flattenTrivialUncheckedSequences_return retval = new PlexilTreeTransforms.flattenTrivialUncheckedSequences_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode outerId=null;
		PlexilTreeNode ACTION108=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD109=null;
		PlexilTreeNode ACTION110=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD111=null;
		PlexilTreeNode ACTION112=null;
		PlexilTreeNode NCNAME113=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD114=null;
		PlexilTreeNode ACTION116=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD117=null;
		PlexilTreeNode ACTION118=null;
		PlexilTreeNode body=null;
		TreeRuleReturnScope innerUnnamed =null;
		TreeRuleReturnScope innerNamed =null;
		TreeRuleReturnScope namedAction115 =null;

		PlexilTreeNode outerId_tree=null;
		PlexilTreeNode ACTION108_tree=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD109_tree=null;
		PlexilTreeNode ACTION110_tree=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD111_tree=null;
		PlexilTreeNode ACTION112_tree=null;
		PlexilTreeNode NCNAME113_tree=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD114_tree=null;
		PlexilTreeNode ACTION116_tree=null;
		PlexilTreeNode UNCHECKED_SEQUENCE_KYWD117_tree=null;
		PlexilTreeNode ACTION118_tree=null;
		PlexilTreeNode body_tree=null;
		RewriteRuleNodeStream stream_ACTION=new RewriteRuleNodeStream(adaptor,"token ACTION");
		RewriteRuleNodeStream stream_UNCHECKED_SEQUENCE_KYWD=new RewriteRuleNodeStream(adaptor,"token UNCHECKED_SEQUENCE_KYWD");
		RewriteRuleNodeStream stream_NCNAME=new RewriteRuleNodeStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_namedAction=new RewriteRuleSubtreeStream(adaptor,"rule namedAction");
		RewriteRuleSubtreeStream stream_unnamedAction=new RewriteRuleSubtreeStream(adaptor,"rule unnamedAction");

		try {
			// antlr/PlexilTreeTransforms.g:231:33: ( ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( UNCHECKED_SEQUENCE_KYWD namedAction ) ) | ^( ACTION outerId= NCNAME ^( UNCHECKED_SEQUENCE_KYWD ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) )
			int alt31=4;
			int LA31_0 = input.LA(1);
			if ( (LA31_0==ACTION) ) {
				int LA31_1 = input.LA(2);
				if ( (LA31_1==DOWN) ) {
					int LA31_2 = input.LA(3);
					if ( (synpred76_PlexilTreeTransforms()) ) {
						alt31=1;
					}
					else if ( (synpred77_PlexilTreeTransforms()) ) {
						alt31=2;
					}
					else if ( (synpred78_PlexilTreeTransforms()) ) {
						alt31=3;
					}
					else if ( (true) ) {
						alt31=4;
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 31, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 31, 0, input);
				throw nvae;
			}

			switch (alt31) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:232:9: ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerUnnamed= unnamedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION108=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2084); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION108);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION108;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					UNCHECKED_SEQUENCE_KYWD109=(PlexilTreeNode)match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2087); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_UNCHECKED_SEQUENCE_KYWD.add(UNCHECKED_SEQUENCE_KYWD109);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = UNCHECKED_SEQUENCE_KYWD109;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_unnamedAction_in_flattenTrivialUncheckedSequences2091);
					innerUnnamed=unnamedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_unnamedAction.add(innerUnnamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerUnnamed
					// token labels: 
					// rule labels: innerUnnamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerUnnamed=new RewriteRuleSubtreeStream(adaptor,"rule innerUnnamed",innerUnnamed!=null?innerUnnamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 233:9: -> $innerUnnamed
					{
						adaptor.addChild(root_0, stream_innerUnnamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:234:9: ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerNamed= namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION110=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2117); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION110);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION110;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					UNCHECKED_SEQUENCE_KYWD111=(PlexilTreeNode)match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2120); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_UNCHECKED_SEQUENCE_KYWD.add(UNCHECKED_SEQUENCE_KYWD111);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = UNCHECKED_SEQUENCE_KYWD111;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialUncheckedSequences2124);
					innerNamed=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_namedAction.add(innerNamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerNamed
					// token labels: 
					// rule labels: innerNamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerNamed=new RewriteRuleSubtreeStream(adaptor,"rule innerNamed",innerNamed!=null?innerNamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 235:9: -> $innerNamed
					{
						adaptor.addChild(root_0, stream_innerNamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:236:9: ^( ACTION NCNAME ^( UNCHECKED_SEQUENCE_KYWD namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION112=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2150); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION112;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					NCNAME113=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialUncheckedSequences2152); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = NCNAME113;

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					UNCHECKED_SEQUENCE_KYWD114=(PlexilTreeNode)match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2155); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = UNCHECKED_SEQUENCE_KYWD114;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialUncheckedSequences2157);
					namedAction115=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_2==null ) _first_2 = (PlexilTreeNode)namedAction115.getTree();

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:237:9: ^( ACTION outerId= NCNAME ^( UNCHECKED_SEQUENCE_KYWD ^( ACTION body= . ) ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION116=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2171); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION116);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION116;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					outerId=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialUncheckedSequences2175); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NCNAME.add(outerId);

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					UNCHECKED_SEQUENCE_KYWD117=(PlexilTreeNode)match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2178); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_UNCHECKED_SEQUENCE_KYWD.add(UNCHECKED_SEQUENCE_KYWD117);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = UNCHECKED_SEQUENCE_KYWD117;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_3 = _last;
					PlexilTreeNode _first_3 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION118=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2181); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION118);

					if ( state.backtracking==1 )
					if ( _first_2==null ) _first_2 = ACTION118;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					body=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_3==null ) _first_3 = body;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_3;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: ACTION, body, outerId
					// token labels: outerId
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: body
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleNodeStream stream_outerId=new RewriteRuleNodeStream(adaptor,"token outerId",outerId);
					RewriteRuleSubtreeStream stream_body=new RewriteRuleSubtreeStream(adaptor,"wildcard body",body);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 238:9: -> ^( ACTION $outerId $body)
					{
						// antlr/PlexilTreeTransforms.g:238:12: ^( ACTION $outerId $body)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_ACTION.nextNode()
						, root_1);
						adaptor.addChild(root_1, stream_outerId.nextNode());
						adaptor.addChild(root_1, stream_body.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "flattenTrivialUncheckedSequences"


	public static class flattenTrivialConcurrences_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "flattenTrivialConcurrences"
	// antlr/PlexilTreeTransforms.g:241:1: flattenTrivialConcurrences : ( ^( ACTION ^( CONCURRENCE_KYWD innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( CONCURRENCE_KYWD innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( CONCURRENCE_KYWD namedAction ) ) | ^( ACTION outerId= NCNAME ^( CONCURRENCE_KYWD ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) );
	public final PlexilTreeTransforms.flattenTrivialConcurrences_return flattenTrivialConcurrences() throws RecognitionException {
		PlexilTreeTransforms.flattenTrivialConcurrences_return retval = new PlexilTreeTransforms.flattenTrivialConcurrences_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode outerId=null;
		PlexilTreeNode ACTION119=null;
		PlexilTreeNode CONCURRENCE_KYWD120=null;
		PlexilTreeNode ACTION121=null;
		PlexilTreeNode CONCURRENCE_KYWD122=null;
		PlexilTreeNode ACTION123=null;
		PlexilTreeNode NCNAME124=null;
		PlexilTreeNode CONCURRENCE_KYWD125=null;
		PlexilTreeNode ACTION127=null;
		PlexilTreeNode CONCURRENCE_KYWD128=null;
		PlexilTreeNode ACTION129=null;
		PlexilTreeNode body=null;
		TreeRuleReturnScope innerUnnamed =null;
		TreeRuleReturnScope innerNamed =null;
		TreeRuleReturnScope namedAction126 =null;

		PlexilTreeNode outerId_tree=null;
		PlexilTreeNode ACTION119_tree=null;
		PlexilTreeNode CONCURRENCE_KYWD120_tree=null;
		PlexilTreeNode ACTION121_tree=null;
		PlexilTreeNode CONCURRENCE_KYWD122_tree=null;
		PlexilTreeNode ACTION123_tree=null;
		PlexilTreeNode NCNAME124_tree=null;
		PlexilTreeNode CONCURRENCE_KYWD125_tree=null;
		PlexilTreeNode ACTION127_tree=null;
		PlexilTreeNode CONCURRENCE_KYWD128_tree=null;
		PlexilTreeNode ACTION129_tree=null;
		PlexilTreeNode body_tree=null;
		RewriteRuleNodeStream stream_ACTION=new RewriteRuleNodeStream(adaptor,"token ACTION");
		RewriteRuleNodeStream stream_CONCURRENCE_KYWD=new RewriteRuleNodeStream(adaptor,"token CONCURRENCE_KYWD");
		RewriteRuleNodeStream stream_NCNAME=new RewriteRuleNodeStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_namedAction=new RewriteRuleSubtreeStream(adaptor,"rule namedAction");
		RewriteRuleSubtreeStream stream_unnamedAction=new RewriteRuleSubtreeStream(adaptor,"rule unnamedAction");

		try {
			// antlr/PlexilTreeTransforms.g:241:27: ( ^( ACTION ^( CONCURRENCE_KYWD innerUnnamed= unnamedAction ) ) -> $innerUnnamed| ^( ACTION ^( CONCURRENCE_KYWD innerNamed= namedAction ) ) -> $innerNamed| ^( ACTION NCNAME ^( CONCURRENCE_KYWD namedAction ) ) | ^( ACTION outerId= NCNAME ^( CONCURRENCE_KYWD ^( ACTION body= . ) ) ) -> ^( ACTION $outerId $body) )
			int alt32=4;
			int LA32_0 = input.LA(1);
			if ( (LA32_0==ACTION) ) {
				int LA32_1 = input.LA(2);
				if ( (LA32_1==DOWN) ) {
					int LA32_2 = input.LA(3);
					if ( (synpred79_PlexilTreeTransforms()) ) {
						alt32=1;
					}
					else if ( (synpred80_PlexilTreeTransforms()) ) {
						alt32=2;
					}
					else if ( (synpred81_PlexilTreeTransforms()) ) {
						alt32=3;
					}
					else if ( (true) ) {
						alt32=4;
					}

				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 32, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 32, 0, input);
				throw nvae;
			}

			switch (alt32) {
				case 1 :
					// antlr/PlexilTreeTransforms.g:242:9: ^( ACTION ^( CONCURRENCE_KYWD innerUnnamed= unnamedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION119=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialConcurrences2229); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION119);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION119;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					CONCURRENCE_KYWD120=(PlexilTreeNode)match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2232); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_CONCURRENCE_KYWD.add(CONCURRENCE_KYWD120);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = CONCURRENCE_KYWD120;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_unnamedAction_in_flattenTrivialConcurrences2236);
					innerUnnamed=unnamedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_unnamedAction.add(innerUnnamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerUnnamed
					// token labels: 
					// rule labels: innerUnnamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerUnnamed=new RewriteRuleSubtreeStream(adaptor,"rule innerUnnamed",innerUnnamed!=null?innerUnnamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 243:9: -> $innerUnnamed
					{
						adaptor.addChild(root_0, stream_innerUnnamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 2 :
					// antlr/PlexilTreeTransforms.g:244:9: ^( ACTION ^( CONCURRENCE_KYWD innerNamed= namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION121=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialConcurrences2262); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION121);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION121;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					CONCURRENCE_KYWD122=(PlexilTreeNode)match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2265); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_CONCURRENCE_KYWD.add(CONCURRENCE_KYWD122);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = CONCURRENCE_KYWD122;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialConcurrences2269);
					innerNamed=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) stream_namedAction.add(innerNamed.getTree());
					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: innerNamed
					// token labels: 
					// rule labels: innerNamed, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_innerNamed=new RewriteRuleSubtreeStream(adaptor,"rule innerNamed",innerNamed!=null?innerNamed.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 245:9: -> $innerNamed
					{
						adaptor.addChild(root_0, stream_innerNamed.nextTree());
					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;
				case 3 :
					// antlr/PlexilTreeTransforms.g:246:9: ^( ACTION NCNAME ^( CONCURRENCE_KYWD namedAction ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION123=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialConcurrences2295); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION123;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					NCNAME124=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialConcurrences2297); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = NCNAME124;

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					CONCURRENCE_KYWD125=(PlexilTreeNode)match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2300); if (state.failed) return retval;

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = CONCURRENCE_KYWD125;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					pushFollow(FOLLOW_namedAction_in_flattenTrivialConcurrences2302);
					namedAction126=namedAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==1 ) 
					 
					if ( _first_2==null ) _first_2 = (PlexilTreeNode)namedAction126.getTree();

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					if ( state.backtracking==1 ) {
					retval.tree = _first_0;
					if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
						retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
					}

					}
					break;
				case 4 :
					// antlr/PlexilTreeTransforms.g:247:9: ^( ACTION outerId= NCNAME ^( CONCURRENCE_KYWD ^( ACTION body= . ) ) )
					{
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_1 = _last;
					PlexilTreeNode _first_1 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION127=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialConcurrences2316); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION127);

					if ( state.backtracking==1 )
					if ( _first_0==null ) _first_0 = ACTION127;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					outerId=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_flattenTrivialConcurrences2320); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_NCNAME.add(outerId);

					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_2 = _last;
					PlexilTreeNode _first_2 = null;
					_last = (PlexilTreeNode)input.LT(1);
					CONCURRENCE_KYWD128=(PlexilTreeNode)match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2323); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_CONCURRENCE_KYWD.add(CONCURRENCE_KYWD128);

					if ( state.backtracking==1 )
					if ( _first_1==null ) _first_1 = CONCURRENCE_KYWD128;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					{
					PlexilTreeNode _save_last_3 = _last;
					PlexilTreeNode _first_3 = null;
					_last = (PlexilTreeNode)input.LT(1);
					ACTION129=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_flattenTrivialConcurrences2326); if (state.failed) return retval;
					 
					if ( state.backtracking==1 ) stream_ACTION.add(ACTION129);

					if ( state.backtracking==1 )
					if ( _first_2==null ) _first_2 = ACTION129;
					match(input, Token.DOWN, null); if (state.failed) return retval;
					_last = (PlexilTreeNode)input.LT(1);
					body=(PlexilTreeNode)input.LT(1);
					matchAny(input); if (state.failed) return retval;
					 
					if ( state.backtracking==1 )
					if ( _first_3==null ) _first_3 = body;

					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_3;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_2;
					}


					match(input, Token.UP, null); if (state.failed) return retval;
					_last = _save_last_1;
					}


					// AST REWRITE
					// elements: body, ACTION, outerId
					// token labels: outerId
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: body
					if ( state.backtracking==1 ) {
					retval.tree = root_0;
					RewriteRuleNodeStream stream_outerId=new RewriteRuleNodeStream(adaptor,"token outerId",outerId);
					RewriteRuleSubtreeStream stream_body=new RewriteRuleSubtreeStream(adaptor,"wildcard body",body);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 248:9: -> ^( ACTION $outerId $body)
					{
						// antlr/PlexilTreeTransforms.g:248:12: ^( ACTION $outerId $body)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(
						stream_ACTION.nextNode()
						, root_1);
						adaptor.addChild(root_1, stream_outerId.nextNode());
						adaptor.addChild(root_1, stream_body.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
					input.replaceChildren(adaptor.getParent(retval.start),
										  adaptor.getChildIndex(retval.start),
										  adaptor.getChildIndex(_last),
										  retval.tree);
					}

					}
					break;

			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "flattenTrivialConcurrences"


	public static class bindingContextNode_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "bindingContextNode"
	// antlr/PlexilTreeTransforms.g:255:1: bindingContextNode : ( BLOCK | CONCURRENCE_KYWD | SEQUENCE_KYWD | UNCHECKED_SEQUENCE_KYWD | TRY_KYWD | FOR_KYWD );
	public final PlexilTreeTransforms.bindingContextNode_return bindingContextNode() throws RecognitionException {
		PlexilTreeTransforms.bindingContextNode_return retval = new PlexilTreeTransforms.bindingContextNode_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode set130=null;

		PlexilTreeNode set130_tree=null;

		try {
			// antlr/PlexilTreeTransforms.g:255:19: ( BLOCK | CONCURRENCE_KYWD | SEQUENCE_KYWD | UNCHECKED_SEQUENCE_KYWD | TRY_KYWD | FOR_KYWD )
			// antlr/PlexilTreeTransforms.g:
			{
			_last = (PlexilTreeNode)input.LT(1);
			set130=(PlexilTreeNode)input.LT(1);
			if ( input.LA(1)==BLOCK||input.LA(1)==CONCURRENCE_KYWD||input.LA(1)==FOR_KYWD||input.LA(1)==SEQUENCE_KYWD||(input.LA(1) >= TRY_KYWD && input.LA(1) <= UNCHECKED_SEQUENCE_KYWD) ) {
				input.consume();
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}

			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}
			 

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "bindingContextNode"


	public static class associativeOp_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "associativeOp"
	// antlr/PlexilTreeTransforms.g:265:1: associativeOp : ( OR_KYWD | AND_KYWD | PLUS | MINUS | ASTERISK | MAX_KYWD | MIN_KYWD );
	public final PlexilTreeTransforms.associativeOp_return associativeOp() throws RecognitionException {
		PlexilTreeTransforms.associativeOp_return retval = new PlexilTreeTransforms.associativeOp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode set131=null;

		PlexilTreeNode set131_tree=null;

		try {
			// antlr/PlexilTreeTransforms.g:265:14: ( OR_KYWD | AND_KYWD | PLUS | MINUS | ASTERISK | MAX_KYWD | MIN_KYWD )
			// antlr/PlexilTreeTransforms.g:
			{
			_last = (PlexilTreeNode)input.LT(1);
			set131=(PlexilTreeNode)input.LT(1);
			if ( input.LA(1)==AND_KYWD||input.LA(1)==ASTERISK||input.LA(1)==MAX_KYWD||(input.LA(1) >= MINUS && input.LA(1) <= MIN_KYWD)||input.LA(1)==OR_KYWD||input.LA(1)==PLUS ) {
				input.consume();
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}

			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}
			 

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "associativeOp"


	public static class namedAction_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "namedAction"
	// antlr/PlexilTreeTransforms.g:275:1: namedAction : ^( ACTION NCNAME . ) ;
	public final PlexilTreeTransforms.namedAction_return namedAction() throws RecognitionException {
		PlexilTreeTransforms.namedAction_return retval = new PlexilTreeTransforms.namedAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode ACTION132=null;
		PlexilTreeNode NCNAME133=null;
		PlexilTreeNode wildcard134=null;

		PlexilTreeNode ACTION132_tree=null;
		PlexilTreeNode NCNAME133_tree=null;
		PlexilTreeNode wildcard134_tree=null;

		try {
			// antlr/PlexilTreeTransforms.g:275:12: ( ^( ACTION NCNAME . ) )
			// antlr/PlexilTreeTransforms.g:276:9: ^( ACTION NCNAME . )
			{
			_last = (PlexilTreeNode)input.LT(1);
			{
			PlexilTreeNode _save_last_1 = _last;
			PlexilTreeNode _first_1 = null;
			_last = (PlexilTreeNode)input.LT(1);
			ACTION132=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_namedAction2519); if (state.failed) return retval;

			if ( state.backtracking==1 )
			if ( _first_0==null ) _first_0 = ACTION132;
			match(input, Token.DOWN, null); if (state.failed) return retval;
			_last = (PlexilTreeNode)input.LT(1);
			NCNAME133=(PlexilTreeNode)match(input,NCNAME,FOLLOW_NCNAME_in_namedAction2521); if (state.failed) return retval;
			 
			if ( state.backtracking==1 )
			if ( _first_1==null ) _first_1 = NCNAME133;

			_last = (PlexilTreeNode)input.LT(1);
			wildcard134=(PlexilTreeNode)input.LT(1);
			matchAny(input); if (state.failed) return retval;
			 
			if ( state.backtracking==1 )
			if ( _first_1==null ) _first_1 = wildcard134;

			match(input, Token.UP, null); if (state.failed) return retval;
			_last = _save_last_1;
			}


			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "namedAction"


	public static class unnamedAction_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "unnamedAction"
	// antlr/PlexilTreeTransforms.g:279:1: unnamedAction : ^( ACTION . ) ;
	public final PlexilTreeTransforms.unnamedAction_return unnamedAction() throws RecognitionException {
		PlexilTreeTransforms.unnamedAction_return retval = new PlexilTreeTransforms.unnamedAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode ACTION135=null;
		PlexilTreeNode wildcard136=null;

		PlexilTreeNode ACTION135_tree=null;
		PlexilTreeNode wildcard136_tree=null;

		try {
			// antlr/PlexilTreeTransforms.g:279:14: ( ^( ACTION . ) )
			// antlr/PlexilTreeTransforms.g:280:9: ^( ACTION . )
			{
			_last = (PlexilTreeNode)input.LT(1);
			{
			PlexilTreeNode _save_last_1 = _last;
			PlexilTreeNode _first_1 = null;
			_last = (PlexilTreeNode)input.LT(1);
			ACTION135=(PlexilTreeNode)match(input,ACTION,FOLLOW_ACTION_in_unnamedAction2542); if (state.failed) return retval;

			if ( state.backtracking==1 )
			if ( _first_0==null ) _first_0 = ACTION135;
			match(input, Token.DOWN, null); if (state.failed) return retval;
			_last = (PlexilTreeNode)input.LT(1);
			wildcard136=(PlexilTreeNode)input.LT(1);
			matchAny(input); if (state.failed) return retval;
			 
			if ( state.backtracking==1 )
			if ( _first_1==null ) _first_1 = wildcard136;

			match(input, Token.UP, null); if (state.failed) return retval;
			_last = _save_last_1;
			}


			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "unnamedAction"


	public static class condition_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "condition"
	// antlr/PlexilTreeTransforms.g:283:1: condition : ^( conditionKywd . ) ;
	public final PlexilTreeTransforms.condition_return condition() throws RecognitionException {
		PlexilTreeTransforms.condition_return retval = new PlexilTreeTransforms.condition_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode wildcard138=null;
		TreeRuleReturnScope conditionKywd137 =null;

		PlexilTreeNode wildcard138_tree=null;

		try {
			// antlr/PlexilTreeTransforms.g:283:10: ( ^( conditionKywd . ) )
			// antlr/PlexilTreeTransforms.g:284:9: ^( conditionKywd . )
			{
			_last = (PlexilTreeNode)input.LT(1);
			{
			PlexilTreeNode _save_last_1 = _last;
			PlexilTreeNode _first_1 = null;
			_last = (PlexilTreeNode)input.LT(1);
			pushFollow(FOLLOW_conditionKywd_in_condition2563);
			conditionKywd137=conditionKywd();
			state._fsp--;
			if (state.failed) return retval;

			if ( state.backtracking==1 )
			if ( _first_0==null ) _first_0 = (PlexilTreeNode)conditionKywd137.getTree();
			match(input, Token.DOWN, null); if (state.failed) return retval;
			_last = (PlexilTreeNode)input.LT(1);
			wildcard138=(PlexilTreeNode)input.LT(1);
			matchAny(input); if (state.failed) return retval;
			 
			if ( state.backtracking==1 )
			if ( _first_1==null ) _first_1 = wildcard138;

			match(input, Token.UP, null); if (state.failed) return retval;
			_last = _save_last_1;
			}


			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "condition"


	public static class conditionKywd_return extends TreeRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "conditionKywd"
	// antlr/PlexilTreeTransforms.g:288:1: conditionKywd : ( END_CONDITION_KYWD | EXIT_CONDITION_KYWD | INVARIANT_CONDITION_KYWD | POST_CONDITION_KYWD | PRE_CONDITION_KYWD | REPEAT_CONDITION_KYWD | SKIP_CONDITION_KYWD | START_CONDITION_KYWD );
	public final PlexilTreeTransforms.conditionKywd_return conditionKywd() throws RecognitionException {
		PlexilTreeTransforms.conditionKywd_return retval = new PlexilTreeTransforms.conditionKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		PlexilTreeNode _first_0 = null;
		PlexilTreeNode _last = null;


		PlexilTreeNode set139=null;

		PlexilTreeNode set139_tree=null;

		try {
			// antlr/PlexilTreeTransforms.g:288:14: ( END_CONDITION_KYWD | EXIT_CONDITION_KYWD | INVARIANT_CONDITION_KYWD | POST_CONDITION_KYWD | PRE_CONDITION_KYWD | REPEAT_CONDITION_KYWD | SKIP_CONDITION_KYWD | START_CONDITION_KYWD )
			// antlr/PlexilTreeTransforms.g:
			{
			_last = (PlexilTreeNode)input.LT(1);
			set139=(PlexilTreeNode)input.LT(1);
			if ( input.LA(1)==END_CONDITION_KYWD||input.LA(1)==EXIT_CONDITION_KYWD||input.LA(1)==INVARIANT_CONDITION_KYWD||input.LA(1)==POST_CONDITION_KYWD||input.LA(1)==PRE_CONDITION_KYWD||input.LA(1)==REPEAT_CONDITION_KYWD||input.LA(1)==SKIP_CONDITION_KYWD||input.LA(1)==START_CONDITION_KYWD ) {
				input.consume();
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}

			if ( state.backtracking==1 ) {
			retval.tree = _first_0;
			if ( adaptor.getParent(retval.tree)!=null && adaptor.isNil( adaptor.getParent(retval.tree) ) )
				retval.tree = (PlexilTreeNode)adaptor.getParent(retval.tree);
			}
			 

			}

		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "conditionKywd"

	// $ANTLR start synpred1_PlexilTreeTransforms
	public final void synpred1_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:95:9: ( trivialReduction )
		// antlr/PlexilTreeTransforms.g:95:9: trivialReduction
		{
		pushFollow(FOLLOW_trivialReduction_in_synpred1_PlexilTreeTransforms174);
		trivialReduction();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred1_PlexilTreeTransforms

	// $ANTLR start synpred2_PlexilTreeTransforms
	public final void synpred2_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:96:9: ( booleanIdentityElision )
		// antlr/PlexilTreeTransforms.g:96:9: booleanIdentityElision
		{
		pushFollow(FOLLOW_booleanIdentityElision_in_synpred2_PlexilTreeTransforms188);
		booleanIdentityElision();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred2_PlexilTreeTransforms

	// $ANTLR start synpred3_PlexilTreeTransforms
	public final void synpred3_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:97:9: ( integerIdentityElision )
		// antlr/PlexilTreeTransforms.g:97:9: integerIdentityElision
		{
		pushFollow(FOLLOW_integerIdentityElision_in_synpred3_PlexilTreeTransforms198);
		integerIdentityElision();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred3_PlexilTreeTransforms

	// $ANTLR start synpred4_PlexilTreeTransforms
	public final void synpred4_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:98:9: ( realIdentityElision )
		// antlr/PlexilTreeTransforms.g:98:9: realIdentityElision
		{
		pushFollow(FOLLOW_realIdentityElision_in_synpred4_PlexilTreeTransforms208);
		realIdentityElision();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred4_PlexilTreeTransforms

	// $ANTLR start synpred5_PlexilTreeTransforms
	public final void synpred5_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:99:9: ( booleanEqualityNegation )
		// antlr/PlexilTreeTransforms.g:99:9: booleanEqualityNegation
		{
		pushFollow(FOLLOW_booleanEqualityNegation_in_synpred5_PlexilTreeTransforms218);
		booleanEqualityNegation();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred5_PlexilTreeTransforms

	// $ANTLR start synpred6_PlexilTreeTransforms
	public final void synpred6_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:100:9: ( rightAssociativeReduction )
		// antlr/PlexilTreeTransforms.g:100:9: rightAssociativeReduction
		{
		pushFollow(FOLLOW_rightAssociativeReduction_in_synpred6_PlexilTreeTransforms228);
		rightAssociativeReduction();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred6_PlexilTreeTransforms

	// $ANTLR start synpred7_PlexilTreeTransforms
	public final void synpred7_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:101:9: ( leftAssociativeReduction )
		// antlr/PlexilTreeTransforms.g:101:9: leftAssociativeReduction
		{
		pushFollow(FOLLOW_leftAssociativeReduction_in_synpred7_PlexilTreeTransforms238);
		leftAssociativeReduction();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred7_PlexilTreeTransforms

	// $ANTLR start synpred8_PlexilTreeTransforms
	public final void synpred8_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:102:9: ( flattenTrivialBlocks )
		// antlr/PlexilTreeTransforms.g:102:9: flattenTrivialBlocks
		{
		pushFollow(FOLLOW_flattenTrivialBlocks_in_synpred8_PlexilTreeTransforms248);
		flattenTrivialBlocks();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred8_PlexilTreeTransforms

	// $ANTLR start synpred9_PlexilTreeTransforms
	public final void synpred9_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:103:9: ( flattenTrivialSequences )
		// antlr/PlexilTreeTransforms.g:103:9: flattenTrivialSequences
		{
		pushFollow(FOLLOW_flattenTrivialSequences_in_synpred9_PlexilTreeTransforms258);
		flattenTrivialSequences();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred9_PlexilTreeTransforms

	// $ANTLR start synpred10_PlexilTreeTransforms
	public final void synpred10_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:104:9: ( flattenTrivialUncheckedSequences )
		// antlr/PlexilTreeTransforms.g:104:9: flattenTrivialUncheckedSequences
		{
		pushFollow(FOLLOW_flattenTrivialUncheckedSequences_in_synpred10_PlexilTreeTransforms268);
		flattenTrivialUncheckedSequences();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred10_PlexilTreeTransforms

	// $ANTLR start synpred11_PlexilTreeTransforms
	public final void synpred11_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:105:9: ( flattenTrivialConcurrences )
		// antlr/PlexilTreeTransforms.g:105:9: flattenTrivialConcurrences
		{
		pushFollow(FOLLOW_flattenTrivialConcurrences_in_synpred11_PlexilTreeTransforms278);
		flattenTrivialConcurrences();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred11_PlexilTreeTransforms

	// $ANTLR start synpred50_PlexilTreeTransforms
	public final void synpred50_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:174:9: ( ^( AND_KYWD TRUE_KYWD arg= . ) )
		// antlr/PlexilTreeTransforms.g:174:9: ^( AND_KYWD TRUE_KYWD arg= . )
		{
		match(input,AND_KYWD,FOLLOW_AND_KYWD_in_synpred50_PlexilTreeTransforms1122); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_synpred50_PlexilTreeTransforms1124); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred50_PlexilTreeTransforms

	// $ANTLR start synpred51_PlexilTreeTransforms
	public final void synpred51_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:175:9: ( ^( AND_KYWD arg= . TRUE_KYWD ) )
		// antlr/PlexilTreeTransforms.g:175:9: ^( AND_KYWD arg= . TRUE_KYWD )
		{
		match(input,AND_KYWD,FOLLOW_AND_KYWD_in_synpred51_PlexilTreeTransforms1145); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_synpred51_PlexilTreeTransforms1151); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred51_PlexilTreeTransforms

	// $ANTLR start synpred52_PlexilTreeTransforms
	public final void synpred52_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:176:9: ( ^( OR_KYWD FALSE_KYWD arg= . ) )
		// antlr/PlexilTreeTransforms.g:176:9: ^( OR_KYWD FALSE_KYWD arg= . )
		{
		match(input,OR_KYWD,FOLLOW_OR_KYWD_in_synpred52_PlexilTreeTransforms1168); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_synpred52_PlexilTreeTransforms1170); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred52_PlexilTreeTransforms

	// $ANTLR start synpred53_PlexilTreeTransforms
	public final void synpred53_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:177:9: ( ^( OR_KYWD arg= . FALSE_KYWD ) )
		// antlr/PlexilTreeTransforms.g:177:9: ^( OR_KYWD arg= . FALSE_KYWD )
		{
		match(input,OR_KYWD,FOLLOW_OR_KYWD_in_synpred53_PlexilTreeTransforms1191); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_synpred53_PlexilTreeTransforms1197); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred53_PlexilTreeTransforms

	// $ANTLR start synpred54_PlexilTreeTransforms
	public final void synpred54_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:178:9: ( ^( DEQUALS TRUE_KYWD arg= . ) )
		// antlr/PlexilTreeTransforms.g:178:9: ^( DEQUALS TRUE_KYWD arg= . )
		{
		match(input,DEQUALS,FOLLOW_DEQUALS_in_synpred54_PlexilTreeTransforms1214); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_synpred54_PlexilTreeTransforms1216); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred54_PlexilTreeTransforms

	// $ANTLR start synpred55_PlexilTreeTransforms
	public final void synpred55_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:179:9: ( ^( DEQUALS arg= . TRUE_KYWD ) )
		// antlr/PlexilTreeTransforms.g:179:9: ^( DEQUALS arg= . TRUE_KYWD )
		{
		match(input,DEQUALS,FOLLOW_DEQUALS_in_synpred55_PlexilTreeTransforms1237); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_synpred55_PlexilTreeTransforms1243); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred55_PlexilTreeTransforms

	// $ANTLR start synpred56_PlexilTreeTransforms
	public final void synpred56_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:180:9: ( ^( NEQUALS FALSE_KYWD arg= . ) )
		// antlr/PlexilTreeTransforms.g:180:9: ^( NEQUALS FALSE_KYWD arg= . )
		{
		match(input,NEQUALS,FOLLOW_NEQUALS_in_synpred56_PlexilTreeTransforms1260); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_synpred56_PlexilTreeTransforms1262); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred56_PlexilTreeTransforms

	// $ANTLR start synpred57_PlexilTreeTransforms
	public final void synpred57_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:185:9: ( ^( PLUS i= INT arg= . {...}?) )
		// antlr/PlexilTreeTransforms.g:185:9: ^( PLUS i= INT arg= . {...}?)
		{
		match(input,PLUS,FOLLOW_PLUS_in_synpred57_PlexilTreeTransforms1320); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_synpred57_PlexilTreeTransforms1324); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		if ( !((Integer.valueOf((i!=null?i.getText():null)) == 0)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred57_PlexilTreeTransforms", "Integer.valueOf($i.text) == 0");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred57_PlexilTreeTransforms

	// $ANTLR start synpred58_PlexilTreeTransforms
	public final void synpred58_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:186:9: ( ^( PLUS arg= . i= INT {...}?) )
		// antlr/PlexilTreeTransforms.g:186:9: ^( PLUS arg= . i= INT {...}?)
		{
		match(input,PLUS,FOLLOW_PLUS_in_synpred58_PlexilTreeTransforms1347); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_synpred58_PlexilTreeTransforms1355); if (state.failed) return;

		if ( !((Integer.valueOf((i!=null?i.getText():null)) == 0)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred58_PlexilTreeTransforms", "Integer.valueOf($i.text) == 0");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred58_PlexilTreeTransforms

	// $ANTLR start synpred60_PlexilTreeTransforms
	public final void synpred60_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:188:9: ( ^( ASTERISK i= INT arg= . {...}?) )
		// antlr/PlexilTreeTransforms.g:188:9: ^( ASTERISK i= INT arg= . {...}?)
		{
		match(input,ASTERISK,FOLLOW_ASTERISK_in_synpred60_PlexilTreeTransforms1401); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_synpred60_PlexilTreeTransforms1405); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		if ( !((Integer.valueOf((i!=null?i.getText():null)) == 1)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred60_PlexilTreeTransforms", "Integer.valueOf($i.text) == 1");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred60_PlexilTreeTransforms

	// $ANTLR start synpred61_PlexilTreeTransforms
	public final void synpred61_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:189:9: ( ^( ASTERISK arg= . i= INT {...}?) )
		// antlr/PlexilTreeTransforms.g:189:9: ^( ASTERISK arg= . i= INT {...}?)
		{
		match(input,ASTERISK,FOLLOW_ASTERISK_in_synpred61_PlexilTreeTransforms1428); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		i=(PlexilTreeNode)match(input,INT,FOLLOW_INT_in_synpred61_PlexilTreeTransforms1436); if (state.failed) return;

		if ( !((Integer.valueOf((i!=null?i.getText():null)) == 1)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred61_PlexilTreeTransforms", "Integer.valueOf($i.text) == 1");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred61_PlexilTreeTransforms

	// $ANTLR start synpred62_PlexilTreeTransforms
	public final void synpred62_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:194:9: ( ^( PLUS i= DOUBLE arg= . {...}?) )
		// antlr/PlexilTreeTransforms.g:194:9: ^( PLUS i= DOUBLE arg= . {...}?)
		{
		match(input,PLUS,FOLLOW_PLUS_in_synpred62_PlexilTreeTransforms1492); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_synpred62_PlexilTreeTransforms1496); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		if ( !((Double.valueOf((i!=null?i.getText():null)) == 0.0)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred62_PlexilTreeTransforms", "Double.valueOf($i.text) == 0.0");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred62_PlexilTreeTransforms

	// $ANTLR start synpred63_PlexilTreeTransforms
	public final void synpred63_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:195:9: ( ^( PLUS arg= . i= DOUBLE {...}?) )
		// antlr/PlexilTreeTransforms.g:195:9: ^( PLUS arg= . i= DOUBLE {...}?)
		{
		match(input,PLUS,FOLLOW_PLUS_in_synpred63_PlexilTreeTransforms1519); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_synpred63_PlexilTreeTransforms1527); if (state.failed) return;

		if ( !((Double.valueOf((i!=null?i.getText():null)) == 0.0)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred63_PlexilTreeTransforms", "Double.valueOf($i.text) == 0.0");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred63_PlexilTreeTransforms

	// $ANTLR start synpred65_PlexilTreeTransforms
	public final void synpred65_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:197:9: ( ^( ASTERISK i= DOUBLE arg= . {...}?) )
		// antlr/PlexilTreeTransforms.g:197:9: ^( ASTERISK i= DOUBLE arg= . {...}?)
		{
		match(input,ASTERISK,FOLLOW_ASTERISK_in_synpred65_PlexilTreeTransforms1573); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_synpred65_PlexilTreeTransforms1577); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		if ( !((Double.valueOf((i!=null?i.getText():null)) == 1.0)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred65_PlexilTreeTransforms", "Double.valueOf($i.text) == 1.0");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred65_PlexilTreeTransforms

	// $ANTLR start synpred66_PlexilTreeTransforms
	public final void synpred66_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode i=null;
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:198:9: ( ^( ASTERISK arg= . i= DOUBLE {...}?) )
		// antlr/PlexilTreeTransforms.g:198:9: ^( ASTERISK arg= . i= DOUBLE {...}?)
		{
		match(input,ASTERISK,FOLLOW_ASTERISK_in_synpred66_PlexilTreeTransforms1600); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		i=(PlexilTreeNode)match(input,DOUBLE,FOLLOW_DOUBLE_in_synpred66_PlexilTreeTransforms1608); if (state.failed) return;

		if ( !((Double.valueOf((i!=null?i.getText():null)) == 1.0)) ) {
			if (state.backtracking>0) {state.failed=true; return;}
			throw new FailedPredicateException(input, "synpred66_PlexilTreeTransforms", "Double.valueOf($i.text) == 1.0");
		}
		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred66_PlexilTreeTransforms

	// $ANTLR start synpred67_PlexilTreeTransforms
	public final void synpred67_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:203:9: ( ^( DEQUALS FALSE_KYWD arg= . ) )
		// antlr/PlexilTreeTransforms.g:203:9: ^( DEQUALS FALSE_KYWD arg= . )
		{
		match(input,DEQUALS,FOLLOW_DEQUALS_in_synpred67_PlexilTreeTransforms1664); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_synpred67_PlexilTreeTransforms1666); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred67_PlexilTreeTransforms

	// $ANTLR start synpred68_PlexilTreeTransforms
	public final void synpred68_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:204:9: ( ^( DEQUALS arg= . FALSE_KYWD ) )
		// antlr/PlexilTreeTransforms.g:204:9: ^( DEQUALS arg= . FALSE_KYWD )
		{
		match(input,DEQUALS,FOLLOW_DEQUALS_in_synpred68_PlexilTreeTransforms1691); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input,FALSE_KYWD,FOLLOW_FALSE_KYWD_in_synpred68_PlexilTreeTransforms1697); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred68_PlexilTreeTransforms

	// $ANTLR start synpred69_PlexilTreeTransforms
	public final void synpred69_PlexilTreeTransforms_fragment() throws RecognitionException {
		PlexilTreeNode arg=null;


		// antlr/PlexilTreeTransforms.g:205:9: ( ^( NEQUALS TRUE_KYWD arg= . ) )
		// antlr/PlexilTreeTransforms.g:205:9: ^( NEQUALS TRUE_KYWD arg= . )
		{
		match(input,NEQUALS,FOLLOW_NEQUALS_in_synpred69_PlexilTreeTransforms1718); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,TRUE_KYWD,FOLLOW_TRUE_KYWD_in_synpred69_PlexilTreeTransforms1720); if (state.failed) return;

		arg=(PlexilTreeNode)input.LT(1);
		matchAny(input); if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred69_PlexilTreeTransforms

	// $ANTLR start synpred70_PlexilTreeTransforms
	public final void synpred70_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerUnnamed =null;


		// antlr/PlexilTreeTransforms.g:212:9: ( ^( ACTION ^( BLOCK innerUnnamed= unnamedAction ) ) )
		// antlr/PlexilTreeTransforms.g:212:9: ^( ACTION ^( BLOCK innerUnnamed= unnamedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred70_PlexilTreeTransforms1794); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,BLOCK,FOLLOW_BLOCK_in_synpred70_PlexilTreeTransforms1797); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_unnamedAction_in_synpred70_PlexilTreeTransforms1801);
		innerUnnamed=unnamedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred70_PlexilTreeTransforms

	// $ANTLR start synpred71_PlexilTreeTransforms
	public final void synpred71_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerNamed =null;


		// antlr/PlexilTreeTransforms.g:214:9: ( ^( ACTION ^( BLOCK innerNamed= namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:214:9: ^( ACTION ^( BLOCK innerNamed= namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred71_PlexilTreeTransforms1827); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,BLOCK,FOLLOW_BLOCK_in_synpred71_PlexilTreeTransforms1830); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred71_PlexilTreeTransforms1834);
		innerNamed=namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred71_PlexilTreeTransforms

	// $ANTLR start synpred72_PlexilTreeTransforms
	public final void synpred72_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:216:9: ( ^( ACTION NCNAME ^( BLOCK namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:216:9: ^( ACTION NCNAME ^( BLOCK namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred72_PlexilTreeTransforms1860); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred72_PlexilTreeTransforms1862); if (state.failed) return;

		match(input,BLOCK,FOLLOW_BLOCK_in_synpred72_PlexilTreeTransforms1865); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred72_PlexilTreeTransforms1867);
		namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred72_PlexilTreeTransforms

	// $ANTLR start synpred73_PlexilTreeTransforms
	public final void synpred73_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerUnnamed =null;


		// antlr/PlexilTreeTransforms.g:222:9: ( ^( ACTION ^( SEQUENCE_KYWD innerUnnamed= unnamedAction ) ) )
		// antlr/PlexilTreeTransforms.g:222:9: ^( ACTION ^( SEQUENCE_KYWD innerUnnamed= unnamedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred73_PlexilTreeTransforms1939); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_synpred73_PlexilTreeTransforms1942); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_unnamedAction_in_synpred73_PlexilTreeTransforms1946);
		innerUnnamed=unnamedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred73_PlexilTreeTransforms

	// $ANTLR start synpred74_PlexilTreeTransforms
	public final void synpred74_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerNamed =null;


		// antlr/PlexilTreeTransforms.g:224:9: ( ^( ACTION ^( SEQUENCE_KYWD innerNamed= namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:224:9: ^( ACTION ^( SEQUENCE_KYWD innerNamed= namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred74_PlexilTreeTransforms1972); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_synpred74_PlexilTreeTransforms1975); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred74_PlexilTreeTransforms1979);
		innerNamed=namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred74_PlexilTreeTransforms

	// $ANTLR start synpred75_PlexilTreeTransforms
	public final void synpred75_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:226:9: ( ^( ACTION NCNAME ^( SEQUENCE_KYWD namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:226:9: ^( ACTION NCNAME ^( SEQUENCE_KYWD namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred75_PlexilTreeTransforms2005); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred75_PlexilTreeTransforms2007); if (state.failed) return;

		match(input,SEQUENCE_KYWD,FOLLOW_SEQUENCE_KYWD_in_synpred75_PlexilTreeTransforms2010); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred75_PlexilTreeTransforms2012);
		namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred75_PlexilTreeTransforms

	// $ANTLR start synpred76_PlexilTreeTransforms
	public final void synpred76_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerUnnamed =null;


		// antlr/PlexilTreeTransforms.g:232:9: ( ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerUnnamed= unnamedAction ) ) )
		// antlr/PlexilTreeTransforms.g:232:9: ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerUnnamed= unnamedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred76_PlexilTreeTransforms2084); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_synpred76_PlexilTreeTransforms2087); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_unnamedAction_in_synpred76_PlexilTreeTransforms2091);
		innerUnnamed=unnamedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred76_PlexilTreeTransforms

	// $ANTLR start synpred77_PlexilTreeTransforms
	public final void synpred77_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerNamed =null;


		// antlr/PlexilTreeTransforms.g:234:9: ( ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerNamed= namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:234:9: ^( ACTION ^( UNCHECKED_SEQUENCE_KYWD innerNamed= namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred77_PlexilTreeTransforms2117); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_synpred77_PlexilTreeTransforms2120); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred77_PlexilTreeTransforms2124);
		innerNamed=namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred77_PlexilTreeTransforms

	// $ANTLR start synpred78_PlexilTreeTransforms
	public final void synpred78_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:236:9: ( ^( ACTION NCNAME ^( UNCHECKED_SEQUENCE_KYWD namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:236:9: ^( ACTION NCNAME ^( UNCHECKED_SEQUENCE_KYWD namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred78_PlexilTreeTransforms2150); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred78_PlexilTreeTransforms2152); if (state.failed) return;

		match(input,UNCHECKED_SEQUENCE_KYWD,FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_synpred78_PlexilTreeTransforms2155); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred78_PlexilTreeTransforms2157);
		namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred78_PlexilTreeTransforms

	// $ANTLR start synpred79_PlexilTreeTransforms
	public final void synpred79_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerUnnamed =null;


		// antlr/PlexilTreeTransforms.g:242:9: ( ^( ACTION ^( CONCURRENCE_KYWD innerUnnamed= unnamedAction ) ) )
		// antlr/PlexilTreeTransforms.g:242:9: ^( ACTION ^( CONCURRENCE_KYWD innerUnnamed= unnamedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred79_PlexilTreeTransforms2229); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_synpred79_PlexilTreeTransforms2232); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_unnamedAction_in_synpred79_PlexilTreeTransforms2236);
		innerUnnamed=unnamedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred79_PlexilTreeTransforms

	// $ANTLR start synpred80_PlexilTreeTransforms
	public final void synpred80_PlexilTreeTransforms_fragment() throws RecognitionException {
		TreeRuleReturnScope innerNamed =null;


		// antlr/PlexilTreeTransforms.g:244:9: ( ^( ACTION ^( CONCURRENCE_KYWD innerNamed= namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:244:9: ^( ACTION ^( CONCURRENCE_KYWD innerNamed= namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred80_PlexilTreeTransforms2262); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_synpred80_PlexilTreeTransforms2265); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred80_PlexilTreeTransforms2269);
		innerNamed=namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred80_PlexilTreeTransforms

	// $ANTLR start synpred81_PlexilTreeTransforms
	public final void synpred81_PlexilTreeTransforms_fragment() throws RecognitionException {
		// antlr/PlexilTreeTransforms.g:246:9: ( ^( ACTION NCNAME ^( CONCURRENCE_KYWD namedAction ) ) )
		// antlr/PlexilTreeTransforms.g:246:9: ^( ACTION NCNAME ^( CONCURRENCE_KYWD namedAction ) )
		{
		match(input,ACTION,FOLLOW_ACTION_in_synpred81_PlexilTreeTransforms2295); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred81_PlexilTreeTransforms2297); if (state.failed) return;

		match(input,CONCURRENCE_KYWD,FOLLOW_CONCURRENCE_KYWD_in_synpred81_PlexilTreeTransforms2300); if (state.failed) return;

		match(input, Token.DOWN, null); if (state.failed) return;
		pushFollow(FOLLOW_namedAction_in_synpred81_PlexilTreeTransforms2302);
		namedAction();
		state._fsp--;
		if (state.failed) return;

		match(input, Token.UP, null); if (state.failed) return;


		match(input, Token.UP, null); if (state.failed) return;


		}

	}
	// $ANTLR end synpred81_PlexilTreeTransforms

	// Delegated rules

	public final boolean synpred6_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred6_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred65_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred65_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred60_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred60_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred1_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred1_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred58_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred58_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred77_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred77_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred74_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred74_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred68_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred68_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred62_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred62_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred56_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred56_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred75_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred75_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred81_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred81_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred55_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred55_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred8_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred8_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred63_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred63_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred3_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred3_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred10_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred10_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred67_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred67_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred51_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred51_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred70_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred70_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred79_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred79_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred72_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred72_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred53_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred53_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred80_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred80_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred57_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred57_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred73_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred73_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred61_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred61_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred7_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred7_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred76_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred76_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred5_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred5_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred69_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred69_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred11_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred11_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred50_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred50_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred4_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred4_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred54_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred54_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred66_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred66_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred71_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred71_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred9_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred9_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred52_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred52_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred78_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred78_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred2_PlexilTreeTransforms() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred2_PlexilTreeTransforms_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}



	public static final BitSet FOLLOW_enterContext_in_topdown154 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_trivialReduction_in_bottomup174 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_booleanIdentityElision_in_bottomup188 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_integerIdentityElision_in_bottomup198 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_realIdentityElision_in_bottomup208 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_booleanEqualityNegation_in_bottomup218 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_rightAssociativeReduction_in_bottomup228 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_leftAssociativeReduction_in_bottomup238 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialBlocks_in_bottomup248 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialSequences_in_bottomup258 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialUncheckedSequences_in_bottomup268 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialConcurrences_in_bottomup278 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_exitContext_in_bottomup288 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_bindingContextNode_in_enterContext312 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_bindingContextNode_in_exitContext342 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_AND_KYWD_in_rightAssociativeReduction375 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_AND_KYWD_in_rightAssociativeReduction382 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_OR_KYWD_in_rightAssociativeReduction413 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_OR_KYWD_in_rightAssociativeReduction420 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_PLUS_in_rightAssociativeReduction452 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_PLUS_in_rightAssociativeReduction459 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ASTERISK_in_rightAssociativeReduction497 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ASTERISK_in_rightAssociativeReduction504 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MAX_KYWD_in_rightAssociativeReduction534 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MAX_KYWD_in_rightAssociativeReduction541 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MIN_KYWD_in_rightAssociativeReduction571 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MIN_KYWD_in_rightAssociativeReduction578 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_AND_KYWD_in_leftAssociativeReduction618 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_AND_KYWD_in_leftAssociativeReduction621 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_OR_KYWD_in_leftAssociativeReduction661 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_OR_KYWD_in_leftAssociativeReduction664 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_PLUS_in_leftAssociativeReduction706 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_PLUS_in_leftAssociativeReduction709 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MINUS_in_leftAssociativeReduction757 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MINUS_in_leftAssociativeReduction760 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ASTERISK_in_leftAssociativeReduction802 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ASTERISK_in_leftAssociativeReduction805 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MAX_KYWD_in_leftAssociativeReduction845 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MAX_KYWD_in_leftAssociativeReduction848 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MIN_KYWD_in_leftAssociativeReduction888 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MIN_KYWD_in_leftAssociativeReduction891 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_AND_KYWD_in_trivialReduction940 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_OR_KYWD_in_trivialReduction961 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_PLUS_in_trivialReduction982 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MINUS_in_trivialReduction1003 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ASTERISK_in_trivialReduction1024 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_SLASH_in_trivialReduction1045 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MAX_KYWD_in_trivialReduction1066 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_MIN_KYWD_in_trivialReduction1087 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_AND_KYWD_in_booleanIdentityElision1122 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_booleanIdentityElision1124 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_AND_KYWD_in_booleanIdentityElision1145 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_booleanIdentityElision1151 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_OR_KYWD_in_booleanIdentityElision1168 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_booleanIdentityElision1170 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_OR_KYWD_in_booleanIdentityElision1191 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_booleanIdentityElision1197 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_DEQUALS_in_booleanIdentityElision1214 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_booleanIdentityElision1216 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_DEQUALS_in_booleanIdentityElision1237 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_booleanIdentityElision1243 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_NEQUALS_in_booleanIdentityElision1260 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_booleanIdentityElision1262 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_NEQUALS_in_booleanIdentityElision1283 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_booleanIdentityElision1289 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_PLUS_in_integerIdentityElision1320 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_integerIdentityElision1324 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_PLUS_in_integerIdentityElision1347 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_integerIdentityElision1355 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_MINUS_in_integerIdentityElision1374 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_integerIdentityElision1382 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ASTERISK_in_integerIdentityElision1401 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_integerIdentityElision1405 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_ASTERISK_in_integerIdentityElision1428 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_integerIdentityElision1436 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_SLASH_in_integerIdentityElision1455 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_integerIdentityElision1463 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_PLUS_in_realIdentityElision1492 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_realIdentityElision1496 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_PLUS_in_realIdentityElision1519 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_realIdentityElision1527 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_MINUS_in_realIdentityElision1546 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_realIdentityElision1554 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ASTERISK_in_realIdentityElision1573 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_realIdentityElision1577 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_ASTERISK_in_realIdentityElision1600 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_realIdentityElision1608 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_SLASH_in_realIdentityElision1627 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_realIdentityElision1635 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_DEQUALS_in_booleanEqualityNegation1664 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_booleanEqualityNegation1666 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_DEQUALS_in_booleanEqualityNegation1691 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_booleanEqualityNegation1697 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_NEQUALS_in_booleanEqualityNegation1718 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_booleanEqualityNegation1720 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_NEQUALS_in_booleanEqualityNegation1746 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_booleanEqualityNegation1752 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialBlocks1794 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_BLOCK_in_flattenTrivialBlocks1797 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_flattenTrivialBlocks1801 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialBlocks1827 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_BLOCK_in_flattenTrivialBlocks1830 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialBlocks1834 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialBlocks1860 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialBlocks1862 = new BitSet(new long[]{0x0000000000100000L});
	public static final BitSet FOLLOW_BLOCK_in_flattenTrivialBlocks1865 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialBlocks1867 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialBlocks1881 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialBlocks1885 = new BitSet(new long[]{0x0000000000100000L});
	public static final BitSet FOLLOW_BLOCK_in_flattenTrivialBlocks1888 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialBlocks1891 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialSequences1939 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences1942 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_flattenTrivialSequences1946 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialSequences1972 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences1975 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialSequences1979 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialSequences2005 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialSequences2007 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences2010 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialSequences2012 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialSequences2026 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialSequences2030 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_flattenTrivialSequences2033 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialSequences2036 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2084 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2087 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_flattenTrivialUncheckedSequences2091 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2117 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2120 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialUncheckedSequences2124 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2150 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialUncheckedSequences2152 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0002000000000000L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2155 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialUncheckedSequences2157 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2171 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialUncheckedSequences2175 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0002000000000000L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_flattenTrivialUncheckedSequences2178 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialUncheckedSequences2181 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialConcurrences2229 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2232 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_flattenTrivialConcurrences2236 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialConcurrences2262 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2265 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialConcurrences2269 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialConcurrences2295 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialConcurrences2297 = new BitSet(new long[]{0x0000002000000000L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2300 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_flattenTrivialConcurrences2302 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialConcurrences2316 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_flattenTrivialConcurrences2320 = new BitSet(new long[]{0x0000002000000000L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_flattenTrivialConcurrences2323 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_flattenTrivialConcurrences2326 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_ACTION_in_namedAction2519 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_namedAction2521 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_ACTION_in_unnamedAction2542 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_conditionKywd_in_condition2563 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_trivialReduction_in_synpred1_PlexilTreeTransforms174 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_booleanIdentityElision_in_synpred2_PlexilTreeTransforms188 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_integerIdentityElision_in_synpred3_PlexilTreeTransforms198 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_realIdentityElision_in_synpred4_PlexilTreeTransforms208 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_booleanEqualityNegation_in_synpred5_PlexilTreeTransforms218 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_rightAssociativeReduction_in_synpred6_PlexilTreeTransforms228 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_leftAssociativeReduction_in_synpred7_PlexilTreeTransforms238 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialBlocks_in_synpred8_PlexilTreeTransforms248 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialSequences_in_synpred9_PlexilTreeTransforms258 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialUncheckedSequences_in_synpred10_PlexilTreeTransforms268 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_flattenTrivialConcurrences_in_synpred11_PlexilTreeTransforms278 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_AND_KYWD_in_synpred50_PlexilTreeTransforms1122 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_synpred50_PlexilTreeTransforms1124 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_AND_KYWD_in_synpred51_PlexilTreeTransforms1145 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_synpred51_PlexilTreeTransforms1151 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_OR_KYWD_in_synpred52_PlexilTreeTransforms1168 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_synpred52_PlexilTreeTransforms1170 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_OR_KYWD_in_synpred53_PlexilTreeTransforms1191 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_synpred53_PlexilTreeTransforms1197 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_DEQUALS_in_synpred54_PlexilTreeTransforms1214 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_synpred54_PlexilTreeTransforms1216 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_DEQUALS_in_synpred55_PlexilTreeTransforms1237 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_synpred55_PlexilTreeTransforms1243 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_NEQUALS_in_synpred56_PlexilTreeTransforms1260 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_synpred56_PlexilTreeTransforms1262 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_PLUS_in_synpred57_PlexilTreeTransforms1320 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_synpred57_PlexilTreeTransforms1324 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_PLUS_in_synpred58_PlexilTreeTransforms1347 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_synpred58_PlexilTreeTransforms1355 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ASTERISK_in_synpred60_PlexilTreeTransforms1401 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_synpred60_PlexilTreeTransforms1405 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_ASTERISK_in_synpred61_PlexilTreeTransforms1428 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_INT_in_synpred61_PlexilTreeTransforms1436 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_PLUS_in_synpred62_PlexilTreeTransforms1492 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_synpred62_PlexilTreeTransforms1496 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_PLUS_in_synpred63_PlexilTreeTransforms1519 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_synpred63_PlexilTreeTransforms1527 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ASTERISK_in_synpred65_PlexilTreeTransforms1573 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_synpred65_PlexilTreeTransforms1577 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_ASTERISK_in_synpred66_PlexilTreeTransforms1600 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_DOUBLE_in_synpred66_PlexilTreeTransforms1608 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_DEQUALS_in_synpred67_PlexilTreeTransforms1664 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_synpred67_PlexilTreeTransforms1666 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_DEQUALS_in_synpred68_PlexilTreeTransforms1691 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_FALSE_KYWD_in_synpred68_PlexilTreeTransforms1697 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_NEQUALS_in_synpred69_PlexilTreeTransforms1718 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_TRUE_KYWD_in_synpred69_PlexilTreeTransforms1720 = new BitSet(new long[]{0xFFFFFFFFFFFFFFF0L,0xFFFFFFFFFFFFFFFFL,0x0FFFFFFFFFFFFFFFL});
	public static final BitSet FOLLOW_ACTION_in_synpred70_PlexilTreeTransforms1794 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_BLOCK_in_synpred70_PlexilTreeTransforms1797 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_synpred70_PlexilTreeTransforms1801 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred71_PlexilTreeTransforms1827 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_BLOCK_in_synpred71_PlexilTreeTransforms1830 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred71_PlexilTreeTransforms1834 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred72_PlexilTreeTransforms1860 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_synpred72_PlexilTreeTransforms1862 = new BitSet(new long[]{0x0000000000100000L});
	public static final BitSet FOLLOW_BLOCK_in_synpred72_PlexilTreeTransforms1865 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred72_PlexilTreeTransforms1867 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred73_PlexilTreeTransforms1939 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_synpred73_PlexilTreeTransforms1942 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_synpred73_PlexilTreeTransforms1946 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred74_PlexilTreeTransforms1972 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_synpred74_PlexilTreeTransforms1975 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred74_PlexilTreeTransforms1979 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred75_PlexilTreeTransforms2005 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_synpred75_PlexilTreeTransforms2007 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_SEQUENCE_KYWD_in_synpred75_PlexilTreeTransforms2010 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred75_PlexilTreeTransforms2012 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred76_PlexilTreeTransforms2084 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_synpred76_PlexilTreeTransforms2087 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_synpred76_PlexilTreeTransforms2091 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred77_PlexilTreeTransforms2117 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_synpred77_PlexilTreeTransforms2120 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred77_PlexilTreeTransforms2124 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred78_PlexilTreeTransforms2150 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_synpred78_PlexilTreeTransforms2152 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0002000000000000L});
	public static final BitSet FOLLOW_UNCHECKED_SEQUENCE_KYWD_in_synpred78_PlexilTreeTransforms2155 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred78_PlexilTreeTransforms2157 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred79_PlexilTreeTransforms2229 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_synpred79_PlexilTreeTransforms2232 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_unnamedAction_in_synpred79_PlexilTreeTransforms2236 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred80_PlexilTreeTransforms2262 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_synpred80_PlexilTreeTransforms2265 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred80_PlexilTreeTransforms2269 = new BitSet(new long[]{0x0000000000000008L});
	public static final BitSet FOLLOW_ACTION_in_synpred81_PlexilTreeTransforms2295 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_NCNAME_in_synpred81_PlexilTreeTransforms2297 = new BitSet(new long[]{0x0000002000000000L});
	public static final BitSet FOLLOW_CONCURRENCE_KYWD_in_synpred81_PlexilTreeTransforms2300 = new BitSet(new long[]{0x0000000000000004L});
	public static final BitSet FOLLOW_namedAction_in_synpred81_PlexilTreeTransforms2302 = new BitSet(new long[]{0x0000000000000008L});
}

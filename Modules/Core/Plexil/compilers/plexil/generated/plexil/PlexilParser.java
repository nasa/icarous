// $ANTLR 3.5.2 antlr/Plexil.g 2019-10-17 11:56:37

package plexil;

import plexil.*;


import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

import org.antlr.runtime.tree.*;


@SuppressWarnings("all")
public class PlexilParser extends Parser {
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
	public Parser[] getDelegates() {
		return new Parser[] {};
	}

	// delegators


	public PlexilParser(TokenStream input) {
		this(input, new RecognizerSharedState());
	}
	public PlexilParser(TokenStream input, RecognizerSharedState state) {
		super(input, state);
	}

	protected TreeAdaptor adaptor = new CommonTreeAdaptor();

	public void setTreeAdaptor(TreeAdaptor adaptor) {
		this.adaptor = adaptor;
	}
	public TreeAdaptor getTreeAdaptor() {
		return adaptor;
	}
	@Override public String[] getTokenNames() { return PlexilParser.tokenNames; }
	@Override public String getGrammarFileName() { return "antlr/Plexil.g"; }


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



	public static class plexilPlan_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "plexilPlan"
	// antlr/Plexil.g:305:1: plexilPlan : ( declarations )? action EOF -> ^( PLEXIL ( declarations )? action ) ;
	public final PlexilParser.plexilPlan_return plexilPlan() throws RecognitionException {
		PlexilParser.plexilPlan_return retval = new PlexilParser.plexilPlan_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token EOF3=null;
		ParserRuleReturnScope declarations1 =null;
		ParserRuleReturnScope action2 =null;

		PlexilTreeNode EOF3_tree=null;
		RewriteRuleTokenStream stream_EOF=new RewriteRuleTokenStream(adaptor,"token EOF");
		RewriteRuleSubtreeStream stream_action=new RewriteRuleSubtreeStream(adaptor,"rule action");
		RewriteRuleSubtreeStream stream_declarations=new RewriteRuleSubtreeStream(adaptor,"rule declarations");

		 m_paraphrases.push("in plan"); 
		try {
			// antlr/Plexil.g:308:2: ( ( declarations )? action EOF -> ^( PLEXIL ( declarations )? action ) )
			// antlr/Plexil.g:309:5: ( declarations )? action EOF
			{
			// antlr/Plexil.g:309:5: ( declarations )?
			int alt1=2;
			int LA1_0 = input.LA(1);
			if ( (LA1_0==BOOLEAN_KYWD||LA1_0==COMMAND_KYWD||LA1_0==DATE_KYWD||LA1_0==DURATION_KYWD||LA1_0==INTEGER_KYWD||LA1_0==LIBRARY_ACTION_KYWD||LA1_0==REAL_KYWD||LA1_0==STRING_KYWD) ) {
				alt1=1;
			}
			switch (alt1) {
				case 1 :
					// antlr/Plexil.g:309:5: declarations
					{
					pushFollow(FOLLOW_declarations_in_plexilPlan1241);
					declarations1=declarations();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_declarations.add(declarations1.getTree());
					}
					break;

			}

			pushFollow(FOLLOW_action_in_plexilPlan1244);
			action2=action();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_action.add(action2.getTree());
			EOF3=(Token)match(input,EOF,FOLLOW_EOF_in_plexilPlan1246); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_EOF.add(EOF3);

			// AST REWRITE
			// elements: action, declarations
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 310:5: -> ^( PLEXIL ( declarations )? action )
			{
				// antlr/Plexil.g:310:8: ^( PLEXIL ( declarations )? action )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot(new PlexilPlanNode(PLEXIL), root_1);
				// antlr/Plexil.g:310:33: ( declarations )?
				if ( stream_declarations.hasNext() ) {
					adaptor.addChild(root_1, stream_declarations.nextTree());
				}
				stream_declarations.reset();

				adaptor.addChild(root_1, stream_action.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "plexilPlan"


	public static class declarations_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "declarations"
	// antlr/Plexil.g:314:1: declarations : ( declaration )+ -> ^( GLOBAL_DECLARATIONS ( declaration )+ ) ;
	public final PlexilParser.declarations_return declarations() throws RecognitionException {
		PlexilParser.declarations_return retval = new PlexilParser.declarations_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope declaration4 =null;

		RewriteRuleSubtreeStream stream_declaration=new RewriteRuleSubtreeStream(adaptor,"rule declaration");

		try {
			// antlr/Plexil.g:314:14: ( ( declaration )+ -> ^( GLOBAL_DECLARATIONS ( declaration )+ ) )
			// antlr/Plexil.g:315:5: ( declaration )+
			{
			// antlr/Plexil.g:315:5: ( declaration )+
			int cnt2=0;
			loop2:
			while (true) {
				int alt2=2;
				int LA2_0 = input.LA(1);
				if ( (LA2_0==BOOLEAN_KYWD||LA2_0==COMMAND_KYWD||LA2_0==DATE_KYWD||LA2_0==DURATION_KYWD||LA2_0==INTEGER_KYWD||LA2_0==LIBRARY_ACTION_KYWD||LA2_0==REAL_KYWD||LA2_0==STRING_KYWD) ) {
					alt2=1;
				}

				switch (alt2) {
				case 1 :
					// antlr/Plexil.g:315:5: declaration
					{
					pushFollow(FOLLOW_declaration_in_declarations1280);
					declaration4=declaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_declaration.add(declaration4.getTree());
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

			// AST REWRITE
			// elements: declaration
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 316:5: -> ^( GLOBAL_DECLARATIONS ( declaration )+ )
			{
				// antlr/Plexil.g:316:8: ^( GLOBAL_DECLARATIONS ( declaration )+ )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot(new GlobalDeclarationsNode(GLOBAL_DECLARATIONS), root_1);
				if ( !(stream_declaration.hasNext()) ) {
					throw new RewriteEarlyExitException();
				}
				while ( stream_declaration.hasNext() ) {
					adaptor.addChild(root_1, stream_declaration.nextTree());
				}
				stream_declaration.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "declarations"


	public static class declaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "declaration"
	// antlr/Plexil.g:318:1: declaration options {k=5; } : ( commandDeclaration | lookupDeclaration | libraryActionDeclaration );
	public final PlexilParser.declaration_return declaration() throws RecognitionException {
		PlexilParser.declaration_return retval = new PlexilParser.declaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope commandDeclaration5 =null;
		ParserRuleReturnScope lookupDeclaration6 =null;
		ParserRuleReturnScope libraryActionDeclaration7 =null;


		try {
			// antlr/Plexil.g:320:2: ( commandDeclaration | lookupDeclaration | libraryActionDeclaration )
			int alt3=3;
			switch ( input.LA(1) ) {
			case COMMAND_KYWD:
				{
				alt3=1;
				}
				break;
			case BOOLEAN_KYWD:
			case DATE_KYWD:
			case DURATION_KYWD:
			case INTEGER_KYWD:
			case REAL_KYWD:
			case STRING_KYWD:
				{
				switch ( input.LA(2) ) {
				case LBRACKET:
					{
					int LA3_4 = input.LA(3);
					if ( (LA3_4==INT) ) {
						int LA3_7 = input.LA(4);
						if ( (LA3_7==RBRACKET) ) {
							int LA3_8 = input.LA(5);
							if ( (LA3_8==COMMAND_KYWD) ) {
								alt3=1;
							}
							else if ( (LA3_8==LOOKUP_KYWD) ) {
								alt3=2;
							}

							else {
								if (state.backtracking>0) {state.failed=true; return retval;}
								int nvaeMark = input.mark();
								try {
									for (int nvaeConsume = 0; nvaeConsume < 5 - 1; nvaeConsume++) {
										input.consume();
									}
									NoViableAltException nvae =
										new NoViableAltException("", 3, 8, input);
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
								for (int nvaeConsume = 0; nvaeConsume < 4 - 1; nvaeConsume++) {
									input.consume();
								}
								NoViableAltException nvae =
									new NoViableAltException("", 3, 7, input);
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
							for (int nvaeConsume = 0; nvaeConsume < 3 - 1; nvaeConsume++) {
								input.consume();
							}
							NoViableAltException nvae =
								new NoViableAltException("", 3, 4, input);
							throw nvae;
						} finally {
							input.rewind(nvaeMark);
						}
					}

					}
					break;
				case COMMAND_KYWD:
					{
					alt3=1;
					}
					break;
				case LOOKUP_KYWD:
					{
					alt3=2;
					}
					break;
				default:
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 3, 2, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}
				}
				break;
			case LIBRARY_ACTION_KYWD:
				{
				alt3=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 3, 0, input);
				throw nvae;
			}
			switch (alt3) {
				case 1 :
					// antlr/Plexil.g:321:5: commandDeclaration
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_commandDeclaration_in_declaration1321);
					commandDeclaration5=commandDeclaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, commandDeclaration5.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:322:5: lookupDeclaration
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_lookupDeclaration_in_declaration1327);
					lookupDeclaration6=lookupDeclaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupDeclaration6.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:323:5: libraryActionDeclaration
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_libraryActionDeclaration_in_declaration1333);
					libraryActionDeclaration7=libraryActionDeclaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, libraryActionDeclaration7.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "declaration"


	public static class commandDeclaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "commandDeclaration"
	// antlr/Plexil.g:330:1: commandDeclaration : ( ( COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? ) ) | ( returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType ) ) ) ;
	public final PlexilParser.commandDeclaration_return commandDeclaration() throws RecognitionException {
		PlexilParser.commandDeclaration_return retval = new PlexilParser.commandDeclaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token COMMAND_KYWD8=null;
		Token NCNAME9=null;
		Token SEMICOLON11=null;
		Token COMMAND_KYWD13=null;
		Token NCNAME14=null;
		Token SEMICOLON16=null;
		ParserRuleReturnScope paramsSpec10 =null;
		ParserRuleReturnScope returnType12 =null;
		ParserRuleReturnScope paramsSpec15 =null;

		PlexilTreeNode COMMAND_KYWD8_tree=null;
		PlexilTreeNode NCNAME9_tree=null;
		PlexilTreeNode SEMICOLON11_tree=null;
		PlexilTreeNode COMMAND_KYWD13_tree=null;
		PlexilTreeNode NCNAME14_tree=null;
		PlexilTreeNode SEMICOLON16_tree=null;
		RewriteRuleTokenStream stream_SEMICOLON=new RewriteRuleTokenStream(adaptor,"token SEMICOLON");
		RewriteRuleTokenStream stream_COMMAND_KYWD=new RewriteRuleTokenStream(adaptor,"token COMMAND_KYWD");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_returnType=new RewriteRuleSubtreeStream(adaptor,"rule returnType");
		RewriteRuleSubtreeStream stream_paramsSpec=new RewriteRuleSubtreeStream(adaptor,"rule paramsSpec");

		 m_paraphrases.push("in command declaration"); 
		try {
			// antlr/Plexil.g:333:2: ( ( ( COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? ) ) | ( returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType ) ) ) )
			// antlr/Plexil.g:334:5: ( ( COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? ) ) | ( returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType ) ) )
			{
			// antlr/Plexil.g:334:5: ( ( COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? ) ) | ( returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType ) ) )
			int alt6=2;
			int LA6_0 = input.LA(1);
			if ( (LA6_0==COMMAND_KYWD) ) {
				alt6=1;
			}
			else if ( (LA6_0==BOOLEAN_KYWD||LA6_0==DATE_KYWD||LA6_0==DURATION_KYWD||LA6_0==INTEGER_KYWD||LA6_0==REAL_KYWD||LA6_0==STRING_KYWD) ) {
				alt6=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 6, 0, input);
				throw nvae;
			}

			switch (alt6) {
				case 1 :
					// antlr/Plexil.g:336:7: ( COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? ) )
					{
					// antlr/Plexil.g:336:7: ( COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? ) )
					// antlr/Plexil.g:336:9: COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON
					{
					COMMAND_KYWD8=(Token)match(input,COMMAND_KYWD,FOLLOW_COMMAND_KYWD_in_commandDeclaration1380); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_COMMAND_KYWD.add(COMMAND_KYWD8);

					NCNAME9=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_commandDeclaration1382); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME9);

					// antlr/Plexil.g:336:29: ( paramsSpec )?
					int alt4=2;
					int LA4_0 = input.LA(1);
					if ( (LA4_0==LPAREN) ) {
						alt4=1;
					}
					switch (alt4) {
						case 1 :
							// antlr/Plexil.g:336:29: paramsSpec
							{
							pushFollow(FOLLOW_paramsSpec_in_commandDeclaration1384);
							paramsSpec10=paramsSpec();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) stream_paramsSpec.add(paramsSpec10.getTree());
							}
							break;

					}

					SEMICOLON11=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_commandDeclaration1387); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON11);

					// AST REWRITE
					// elements: NCNAME, COMMAND_KYWD, paramsSpec
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 337:9: -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? )
					{
						// antlr/Plexil.g:337:12: ^( COMMAND_KYWD NCNAME ( paramsSpec )? )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(new CommandDeclarationNode(stream_COMMAND_KYWD.nextToken()), root_1);
						adaptor.addChild(root_1, stream_NCNAME.nextNode());
						// antlr/Plexil.g:337:58: ( paramsSpec )?
						if ( stream_paramsSpec.hasNext() ) {
							adaptor.addChild(root_1, stream_paramsSpec.nextTree());
						}
						stream_paramsSpec.reset();

						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:341:7: ( returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType ) )
					{
					// antlr/Plexil.g:341:7: ( returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType ) )
					// antlr/Plexil.g:341:9: returnType COMMAND_KYWD NCNAME ( paramsSpec )? SEMICOLON
					{
					pushFollow(FOLLOW_returnType_in_commandDeclaration1437);
					returnType12=returnType();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_returnType.add(returnType12.getTree());
					COMMAND_KYWD13=(Token)match(input,COMMAND_KYWD,FOLLOW_COMMAND_KYWD_in_commandDeclaration1439); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_COMMAND_KYWD.add(COMMAND_KYWD13);

					NCNAME14=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_commandDeclaration1441); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME14);

					// antlr/Plexil.g:341:40: ( paramsSpec )?
					int alt5=2;
					int LA5_0 = input.LA(1);
					if ( (LA5_0==LPAREN) ) {
						alt5=1;
					}
					switch (alt5) {
						case 1 :
							// antlr/Plexil.g:341:40: paramsSpec
							{
							pushFollow(FOLLOW_paramsSpec_in_commandDeclaration1443);
							paramsSpec15=paramsSpec();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) stream_paramsSpec.add(paramsSpec15.getTree());
							}
							break;

					}

					SEMICOLON16=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_commandDeclaration1446); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON16);

					// AST REWRITE
					// elements: NCNAME, returnType, COMMAND_KYWD, paramsSpec
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 342:10: -> ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType )
					{
						// antlr/Plexil.g:342:13: ^( COMMAND_KYWD NCNAME ( paramsSpec )? returnType )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(new CommandDeclarationNode(stream_COMMAND_KYWD.nextToken()), root_1);
						adaptor.addChild(root_1, stream_NCNAME.nextNode());
						// antlr/Plexil.g:342:59: ( paramsSpec )?
						if ( stream_paramsSpec.hasNext() ) {
							adaptor.addChild(root_1, stream_paramsSpec.nextTree());
						}
						stream_paramsSpec.reset();

						adaptor.addChild(root_1, stream_returnType.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}

					}
					break;

			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "commandDeclaration"


	public static class lookupDeclaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookupDeclaration"
	// antlr/Plexil.g:349:1: lookupDeclaration : returnType LOOKUP_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( LOOKUP_KYWD NCNAME returnType ( paramsSpec )? ) ;
	public final PlexilParser.lookupDeclaration_return lookupDeclaration() throws RecognitionException {
		PlexilParser.lookupDeclaration_return retval = new PlexilParser.lookupDeclaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LOOKUP_KYWD18=null;
		Token NCNAME19=null;
		Token SEMICOLON21=null;
		ParserRuleReturnScope returnType17 =null;
		ParserRuleReturnScope paramsSpec20 =null;

		PlexilTreeNode LOOKUP_KYWD18_tree=null;
		PlexilTreeNode NCNAME19_tree=null;
		PlexilTreeNode SEMICOLON21_tree=null;
		RewriteRuleTokenStream stream_SEMICOLON=new RewriteRuleTokenStream(adaptor,"token SEMICOLON");
		RewriteRuleTokenStream stream_LOOKUP_KYWD=new RewriteRuleTokenStream(adaptor,"token LOOKUP_KYWD");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_returnType=new RewriteRuleSubtreeStream(adaptor,"rule returnType");
		RewriteRuleSubtreeStream stream_paramsSpec=new RewriteRuleSubtreeStream(adaptor,"rule paramsSpec");

		 m_paraphrases.push("in lookup declaration"); 
		try {
			// antlr/Plexil.g:352:2: ( returnType LOOKUP_KYWD NCNAME ( paramsSpec )? SEMICOLON -> ^( LOOKUP_KYWD NCNAME returnType ( paramsSpec )? ) )
			// antlr/Plexil.g:354:5: returnType LOOKUP_KYWD NCNAME ( paramsSpec )? SEMICOLON
			{
			pushFollow(FOLLOW_returnType_in_lookupDeclaration1519);
			returnType17=returnType();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_returnType.add(returnType17.getTree());
			LOOKUP_KYWD18=(Token)match(input,LOOKUP_KYWD,FOLLOW_LOOKUP_KYWD_in_lookupDeclaration1521); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LOOKUP_KYWD.add(LOOKUP_KYWD18);

			NCNAME19=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_lookupDeclaration1523); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME19);

			// antlr/Plexil.g:354:35: ( paramsSpec )?
			int alt7=2;
			int LA7_0 = input.LA(1);
			if ( (LA7_0==LPAREN) ) {
				alt7=1;
			}
			switch (alt7) {
				case 1 :
					// antlr/Plexil.g:354:35: paramsSpec
					{
					pushFollow(FOLLOW_paramsSpec_in_lookupDeclaration1525);
					paramsSpec20=paramsSpec();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_paramsSpec.add(paramsSpec20.getTree());
					}
					break;

			}

			SEMICOLON21=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_lookupDeclaration1528); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON21);

			// AST REWRITE
			// elements: returnType, LOOKUP_KYWD, paramsSpec, NCNAME
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 355:5: -> ^( LOOKUP_KYWD NCNAME returnType ( paramsSpec )? )
			{
				// antlr/Plexil.g:355:8: ^( LOOKUP_KYWD NCNAME returnType ( paramsSpec )? )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot(new LookupDeclarationNode(stream_LOOKUP_KYWD.nextToken()), root_1);
				adaptor.addChild(root_1, stream_NCNAME.nextNode());
				adaptor.addChild(root_1, stream_returnType.nextTree());
				// antlr/Plexil.g:355:63: ( paramsSpec )?
				if ( stream_paramsSpec.hasNext() ) {
					adaptor.addChild(root_1, stream_paramsSpec.nextTree());
				}
				stream_paramsSpec.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookupDeclaration"


	public static class paramsSpec_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "paramsSpec"
	// antlr/Plexil.g:358:1: paramsSpec : LPAREN ( paramsSpecGuts )? RPAREN -> ^( PARAMETERS ( paramsSpecGuts )? ) ;
	public final PlexilParser.paramsSpec_return paramsSpec() throws RecognitionException {
		PlexilParser.paramsSpec_return retval = new PlexilParser.paramsSpec_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LPAREN22=null;
		Token RPAREN24=null;
		ParserRuleReturnScope paramsSpecGuts23 =null;

		PlexilTreeNode LPAREN22_tree=null;
		PlexilTreeNode RPAREN24_tree=null;
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleSubtreeStream stream_paramsSpecGuts=new RewriteRuleSubtreeStream(adaptor,"rule paramsSpecGuts");

		try {
			// antlr/Plexil.g:358:12: ( LPAREN ( paramsSpecGuts )? RPAREN -> ^( PARAMETERS ( paramsSpecGuts )? ) )
			// antlr/Plexil.g:359:5: LPAREN ( paramsSpecGuts )? RPAREN
			{
			LPAREN22=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_paramsSpec1563); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN22);

			// antlr/Plexil.g:359:12: ( paramsSpecGuts )?
			int alt8=2;
			int LA8_0 = input.LA(1);
			if ( (LA8_0==ANY_KYWD||LA8_0==BOOLEAN_KYWD||LA8_0==DATE_KYWD||LA8_0==DURATION_KYWD||LA8_0==ELLIPSIS||LA8_0==INTEGER_KYWD||LA8_0==REAL_KYWD||LA8_0==STRING_KYWD) ) {
				alt8=1;
			}
			switch (alt8) {
				case 1 :
					// antlr/Plexil.g:359:12: paramsSpecGuts
					{
					pushFollow(FOLLOW_paramsSpecGuts_in_paramsSpec1565);
					paramsSpecGuts23=paramsSpecGuts();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_paramsSpecGuts.add(paramsSpecGuts23.getTree());
					}
					break;

			}

			RPAREN24=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_paramsSpec1568); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN24);

			// AST REWRITE
			// elements: paramsSpecGuts
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 360:3: -> ^( PARAMETERS ( paramsSpecGuts )? )
			{
				// antlr/Plexil.g:360:6: ^( PARAMETERS ( paramsSpecGuts )? )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(PARAMETERS, "PARAMETERS"), root_1);
				// antlr/Plexil.g:360:19: ( paramsSpecGuts )?
				if ( stream_paramsSpecGuts.hasNext() ) {
					adaptor.addChild(root_1, stream_paramsSpecGuts.nextTree());
				}
				stream_paramsSpecGuts.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "paramsSpec"


	public static class paramsSpecGuts_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "paramsSpecGuts"
	// antlr/Plexil.g:363:1: paramsSpecGuts : ( ( paramSpec ( COMMA ! paramSpec )* ( COMMA ! ELLIPSIS )? ) | ELLIPSIS );
	public final PlexilParser.paramsSpecGuts_return paramsSpecGuts() throws RecognitionException {
		PlexilParser.paramsSpecGuts_return retval = new PlexilParser.paramsSpecGuts_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token COMMA26=null;
		Token COMMA28=null;
		Token ELLIPSIS29=null;
		Token ELLIPSIS30=null;
		ParserRuleReturnScope paramSpec25 =null;
		ParserRuleReturnScope paramSpec27 =null;

		PlexilTreeNode COMMA26_tree=null;
		PlexilTreeNode COMMA28_tree=null;
		PlexilTreeNode ELLIPSIS29_tree=null;
		PlexilTreeNode ELLIPSIS30_tree=null;

		try {
			// antlr/Plexil.g:363:16: ( ( paramSpec ( COMMA ! paramSpec )* ( COMMA ! ELLIPSIS )? ) | ELLIPSIS )
			int alt11=2;
			int LA11_0 = input.LA(1);
			if ( (LA11_0==ANY_KYWD||LA11_0==BOOLEAN_KYWD||LA11_0==DATE_KYWD||LA11_0==DURATION_KYWD||LA11_0==INTEGER_KYWD||LA11_0==REAL_KYWD||LA11_0==STRING_KYWD) ) {
				alt11=1;
			}
			else if ( (LA11_0==ELLIPSIS) ) {
				alt11=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 11, 0, input);
				throw nvae;
			}

			switch (alt11) {
				case 1 :
					// antlr/Plexil.g:364:7: ( paramSpec ( COMMA ! paramSpec )* ( COMMA ! ELLIPSIS )? )
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					// antlr/Plexil.g:364:7: ( paramSpec ( COMMA ! paramSpec )* ( COMMA ! ELLIPSIS )? )
					// antlr/Plexil.g:364:9: paramSpec ( COMMA ! paramSpec )* ( COMMA ! ELLIPSIS )?
					{
					pushFollow(FOLLOW_paramSpec_in_paramsSpecGuts1597);
					paramSpec25=paramSpec();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, paramSpec25.getTree());

					// antlr/Plexil.g:364:19: ( COMMA ! paramSpec )*
					loop9:
					while (true) {
						int alt9=2;
						int LA9_0 = input.LA(1);
						if ( (LA9_0==COMMA) ) {
							int LA9_1 = input.LA(2);
							if ( (LA9_1==ANY_KYWD||LA9_1==BOOLEAN_KYWD||LA9_1==DATE_KYWD||LA9_1==DURATION_KYWD||LA9_1==INTEGER_KYWD||LA9_1==REAL_KYWD||LA9_1==STRING_KYWD) ) {
								alt9=1;
							}

						}

						switch (alt9) {
						case 1 :
							// antlr/Plexil.g:364:21: COMMA ! paramSpec
							{
							COMMA26=(Token)match(input,COMMA,FOLLOW_COMMA_in_paramsSpecGuts1601); if (state.failed) return retval;
							pushFollow(FOLLOW_paramSpec_in_paramsSpecGuts1604);
							paramSpec27=paramSpec();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, paramSpec27.getTree());

							}
							break;

						default :
							break loop9;
						}
					}

					// antlr/Plexil.g:364:41: ( COMMA ! ELLIPSIS )?
					int alt10=2;
					int LA10_0 = input.LA(1);
					if ( (LA10_0==COMMA) ) {
						alt10=1;
					}
					switch (alt10) {
						case 1 :
							// antlr/Plexil.g:364:43: COMMA ! ELLIPSIS
							{
							COMMA28=(Token)match(input,COMMA,FOLLOW_COMMA_in_paramsSpecGuts1611); if (state.failed) return retval;
							ELLIPSIS29=(Token)match(input,ELLIPSIS,FOLLOW_ELLIPSIS_in_paramsSpecGuts1614); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							ELLIPSIS29_tree = (PlexilTreeNode)adaptor.create(ELLIPSIS29);
							adaptor.addChild(root_0, ELLIPSIS29_tree);
							}

							}
							break;

					}

					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:365:9: ELLIPSIS
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					ELLIPSIS30=(Token)match(input,ELLIPSIS,FOLLOW_ELLIPSIS_in_paramsSpecGuts1629); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					ELLIPSIS30_tree = (PlexilTreeNode)adaptor.create(ELLIPSIS30);
					adaptor.addChild(root_0, ELLIPSIS30_tree);
					}

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "paramsSpecGuts"


	public static class paramSpec_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "paramSpec"
	// antlr/Plexil.g:368:1: paramSpec options {k=2; } : ( ( baseTypeName LBRACKET )=> baseTypeName LBRACKET INT RBRACKET -> ^( ARRAY_TYPE baseTypeName INT ) | ( baseTypeName NCNAME LBRACKET )=> baseTypeName NCNAME LBRACKET INT RBRACKET -> ^( ARRAY_TYPE baseTypeName INT NCNAME ) | paramTypeName ^ ( NCNAME )? );
	public final PlexilParser.paramSpec_return paramSpec() throws RecognitionException {
		PlexilParser.paramSpec_return retval = new PlexilParser.paramSpec_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LBRACKET32=null;
		Token INT33=null;
		Token RBRACKET34=null;
		Token NCNAME36=null;
		Token LBRACKET37=null;
		Token INT38=null;
		Token RBRACKET39=null;
		Token NCNAME41=null;
		ParserRuleReturnScope baseTypeName31 =null;
		ParserRuleReturnScope baseTypeName35 =null;
		ParserRuleReturnScope paramTypeName40 =null;

		PlexilTreeNode LBRACKET32_tree=null;
		PlexilTreeNode INT33_tree=null;
		PlexilTreeNode RBRACKET34_tree=null;
		PlexilTreeNode NCNAME36_tree=null;
		PlexilTreeNode LBRACKET37_tree=null;
		PlexilTreeNode INT38_tree=null;
		PlexilTreeNode RBRACKET39_tree=null;
		PlexilTreeNode NCNAME41_tree=null;
		RewriteRuleTokenStream stream_LBRACKET=new RewriteRuleTokenStream(adaptor,"token LBRACKET");
		RewriteRuleTokenStream stream_RBRACKET=new RewriteRuleTokenStream(adaptor,"token RBRACKET");
		RewriteRuleTokenStream stream_INT=new RewriteRuleTokenStream(adaptor,"token INT");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_baseTypeName=new RewriteRuleSubtreeStream(adaptor,"rule baseTypeName");

		try {
			// antlr/Plexil.g:370:2: ( ( baseTypeName LBRACKET )=> baseTypeName LBRACKET INT RBRACKET -> ^( ARRAY_TYPE baseTypeName INT ) | ( baseTypeName NCNAME LBRACKET )=> baseTypeName NCNAME LBRACKET INT RBRACKET -> ^( ARRAY_TYPE baseTypeName INT NCNAME ) | paramTypeName ^ ( NCNAME )? )
			int alt13=3;
			int LA13_0 = input.LA(1);
			if ( (LA13_0==BOOLEAN_KYWD||LA13_0==DATE_KYWD||LA13_0==DURATION_KYWD||LA13_0==INTEGER_KYWD||LA13_0==REAL_KYWD||LA13_0==STRING_KYWD) ) {
				int LA13_1 = input.LA(2);
				if ( (LA13_1==LBRACKET) && (synpred1_Plexil())) {
					alt13=1;
				}
				else if ( (LA13_1==NCNAME) ) {
					int LA13_4 = input.LA(3);
					if ( (synpred2_Plexil()) ) {
						alt13=2;
					}
					else if ( (true) ) {
						alt13=3;
					}

				}
				else if ( (LA13_1==COMMA||LA13_1==RPAREN) ) {
					alt13=3;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 13, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}
			else if ( (LA13_0==ANY_KYWD) ) {
				alt13=3;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 13, 0, input);
				throw nvae;
			}

			switch (alt13) {
				case 1 :
					// antlr/Plexil.g:371:5: ( baseTypeName LBRACKET )=> baseTypeName LBRACKET INT RBRACKET
					{
					pushFollow(FOLLOW_baseTypeName_in_paramSpec1670);
					baseTypeName31=baseTypeName();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_baseTypeName.add(baseTypeName31.getTree());
					LBRACKET32=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_paramSpec1672); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_LBRACKET.add(LBRACKET32);

					INT33=(Token)match(input,INT,FOLLOW_INT_in_paramSpec1674); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_INT.add(INT33);

					RBRACKET34=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_paramSpec1676); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_RBRACKET.add(RBRACKET34);

					// AST REWRITE
					// elements: baseTypeName, INT
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 372:42: -> ^( ARRAY_TYPE baseTypeName INT )
					{
						// antlr/Plexil.g:372:45: ^( ARRAY_TYPE baseTypeName INT )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_TYPE, "ARRAY_TYPE"), root_1);
						adaptor.addChild(root_1, stream_baseTypeName.nextTree());
						adaptor.addChild(root_1, stream_INT.nextNode());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:373:5: ( baseTypeName NCNAME LBRACKET )=> baseTypeName NCNAME LBRACKET INT RBRACKET
					{
					pushFollow(FOLLOW_baseTypeName_in_paramSpec1708);
					baseTypeName35=baseTypeName();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_baseTypeName.add(baseTypeName35.getTree());
					NCNAME36=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_paramSpec1710); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME36);

					LBRACKET37=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_paramSpec1712); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_LBRACKET.add(LBRACKET37);

					INT38=(Token)match(input,INT,FOLLOW_INT_in_paramSpec1714); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_INT.add(INT38);

					RBRACKET39=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_paramSpec1716); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_RBRACKET.add(RBRACKET39);

					// AST REWRITE
					// elements: NCNAME, baseTypeName, INT
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 374:49: -> ^( ARRAY_TYPE baseTypeName INT NCNAME )
					{
						// antlr/Plexil.g:374:52: ^( ARRAY_TYPE baseTypeName INT NCNAME )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_TYPE, "ARRAY_TYPE"), root_1);
						adaptor.addChild(root_1, stream_baseTypeName.nextTree());
						adaptor.addChild(root_1, stream_INT.nextNode());
						adaptor.addChild(root_1, stream_NCNAME.nextNode());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;
				case 3 :
					// antlr/Plexil.g:375:5: paramTypeName ^ ( NCNAME )?
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_paramTypeName_in_paramSpec1734);
					paramTypeName40=paramTypeName();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) root_0 = (PlexilTreeNode)adaptor.becomeRoot(paramTypeName40.getTree(), root_0);
					// antlr/Plexil.g:375:20: ( NCNAME )?
					int alt12=2;
					int LA12_0 = input.LA(1);
					if ( (LA12_0==NCNAME) ) {
						alt12=1;
					}
					switch (alt12) {
						case 1 :
							// antlr/Plexil.g:375:20: NCNAME
							{
							NCNAME41=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_paramSpec1737); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							NCNAME41_tree = (PlexilTreeNode)adaptor.create(NCNAME41);
							adaptor.addChild(root_0, NCNAME41_tree);
							}

							}
							break;

					}

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "paramSpec"


	public static class paramTypeName_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "paramTypeName"
	// antlr/Plexil.g:378:1: paramTypeName : ( ANY_KYWD | BOOLEAN_KYWD | INTEGER_KYWD | REAL_KYWD | STRING_KYWD | DATE_KYWD | DURATION_KYWD );
	public final PlexilParser.paramTypeName_return paramTypeName() throws RecognitionException {
		PlexilParser.paramTypeName_return retval = new PlexilParser.paramTypeName_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set42=null;

		PlexilTreeNode set42_tree=null;

		try {
			// antlr/Plexil.g:379:5: ( ANY_KYWD | BOOLEAN_KYWD | INTEGER_KYWD | REAL_KYWD | STRING_KYWD | DATE_KYWD | DURATION_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set42=input.LT(1);
			if ( input.LA(1)==ANY_KYWD||input.LA(1)==BOOLEAN_KYWD||input.LA(1)==DATE_KYWD||input.LA(1)==DURATION_KYWD||input.LA(1)==INTEGER_KYWD||input.LA(1)==REAL_KYWD||input.LA(1)==STRING_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set42));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "paramTypeName"


	public static class returnType_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "returnType"
	// antlr/Plexil.g:388:1: returnType : returnTypeSpec -> ^( RETURNS_KYWD returnTypeSpec ) ;
	public final PlexilParser.returnType_return returnType() throws RecognitionException {
		PlexilParser.returnType_return retval = new PlexilParser.returnType_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope returnTypeSpec43 =null;

		RewriteRuleSubtreeStream stream_returnTypeSpec=new RewriteRuleSubtreeStream(adaptor,"rule returnTypeSpec");

		try {
			// antlr/Plexil.g:388:12: ( returnTypeSpec -> ^( RETURNS_KYWD returnTypeSpec ) )
			// antlr/Plexil.g:389:5: returnTypeSpec
			{
			pushFollow(FOLLOW_returnTypeSpec_in_returnType1817);
			returnTypeSpec43=returnTypeSpec();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_returnTypeSpec.add(returnTypeSpec43.getTree());
			// AST REWRITE
			// elements: returnTypeSpec
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 389:20: -> ^( RETURNS_KYWD returnTypeSpec )
			{
				// antlr/Plexil.g:389:23: ^( RETURNS_KYWD returnTypeSpec )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(RETURNS_KYWD, "RETURNS_KYWD"), root_1);
				adaptor.addChild(root_1, stream_returnTypeSpec.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "returnType"


	public static class returnTypeSpec_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "returnTypeSpec"
	// antlr/Plexil.g:392:1: returnTypeSpec : ( ( baseTypeName LBRACKET )=> baseTypeName LBRACKET INT RBRACKET -> ^( ARRAY_TYPE baseTypeName INT ) | baseTypeName );
	public final PlexilParser.returnTypeSpec_return returnTypeSpec() throws RecognitionException {
		PlexilParser.returnTypeSpec_return retval = new PlexilParser.returnTypeSpec_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LBRACKET45=null;
		Token INT46=null;
		Token RBRACKET47=null;
		ParserRuleReturnScope baseTypeName44 =null;
		ParserRuleReturnScope baseTypeName48 =null;

		PlexilTreeNode LBRACKET45_tree=null;
		PlexilTreeNode INT46_tree=null;
		PlexilTreeNode RBRACKET47_tree=null;
		RewriteRuleTokenStream stream_LBRACKET=new RewriteRuleTokenStream(adaptor,"token LBRACKET");
		RewriteRuleTokenStream stream_RBRACKET=new RewriteRuleTokenStream(adaptor,"token RBRACKET");
		RewriteRuleTokenStream stream_INT=new RewriteRuleTokenStream(adaptor,"token INT");
		RewriteRuleSubtreeStream stream_baseTypeName=new RewriteRuleSubtreeStream(adaptor,"rule baseTypeName");

		try {
			// antlr/Plexil.g:392:16: ( ( baseTypeName LBRACKET )=> baseTypeName LBRACKET INT RBRACKET -> ^( ARRAY_TYPE baseTypeName INT ) | baseTypeName )
			int alt14=2;
			int LA14_0 = input.LA(1);
			if ( (LA14_0==BOOLEAN_KYWD||LA14_0==DATE_KYWD||LA14_0==DURATION_KYWD||LA14_0==INTEGER_KYWD||LA14_0==REAL_KYWD||LA14_0==STRING_KYWD) ) {
				int LA14_1 = input.LA(2);
				if ( (LA14_1==LBRACKET) && (synpred3_Plexil())) {
					alt14=1;
				}
				else if ( (LA14_1==COMMAND_KYWD||LA14_1==LOOKUP_KYWD) ) {
					alt14=2;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 14, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 14, 0, input);
				throw nvae;
			}

			switch (alt14) {
				case 1 :
					// antlr/Plexil.g:393:5: ( baseTypeName LBRACKET )=> baseTypeName LBRACKET INT RBRACKET
					{
					pushFollow(FOLLOW_baseTypeName_in_returnTypeSpec1853);
					baseTypeName44=baseTypeName();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_baseTypeName.add(baseTypeName44.getTree());
					LBRACKET45=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_returnTypeSpec1855); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_LBRACKET.add(LBRACKET45);

					INT46=(Token)match(input,INT,FOLLOW_INT_in_returnTypeSpec1857); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_INT.add(INT46);

					RBRACKET47=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_returnTypeSpec1859); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_RBRACKET.add(RBRACKET47);

					// AST REWRITE
					// elements: baseTypeName, INT
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 394:42: -> ^( ARRAY_TYPE baseTypeName INT )
					{
						// antlr/Plexil.g:394:45: ^( ARRAY_TYPE baseTypeName INT )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_TYPE, "ARRAY_TYPE"), root_1);
						adaptor.addChild(root_1, stream_baseTypeName.nextTree());
						adaptor.addChild(root_1, stream_INT.nextNode());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:395:5: baseTypeName
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_baseTypeName_in_returnTypeSpec1875);
					baseTypeName48=baseTypeName();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, baseTypeName48.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "returnTypeSpec"


	public static class baseTypeName_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "baseTypeName"
	// antlr/Plexil.g:398:1: baseTypeName : ( BOOLEAN_KYWD | INTEGER_KYWD | REAL_KYWD | STRING_KYWD | DATE_KYWD | DURATION_KYWD );
	public final PlexilParser.baseTypeName_return baseTypeName() throws RecognitionException {
		PlexilParser.baseTypeName_return retval = new PlexilParser.baseTypeName_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set49=null;

		PlexilTreeNode set49_tree=null;

		try {
			// antlr/Plexil.g:398:14: ( BOOLEAN_KYWD | INTEGER_KYWD | REAL_KYWD | STRING_KYWD | DATE_KYWD | DURATION_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set49=input.LT(1);
			if ( input.LA(1)==BOOLEAN_KYWD||input.LA(1)==DATE_KYWD||input.LA(1)==DURATION_KYWD||input.LA(1)==INTEGER_KYWD||input.LA(1)==REAL_KYWD||input.LA(1)==STRING_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set49));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "baseTypeName"


	public static class libraryActionDeclaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "libraryActionDeclaration"
	// antlr/Plexil.g:407:1: libraryActionDeclaration : LIBRARY_ACTION_KYWD ^ NCNAME ( libraryInterfaceSpec )? SEMICOLON !;
	public final PlexilParser.libraryActionDeclaration_return libraryActionDeclaration() throws RecognitionException {
		PlexilParser.libraryActionDeclaration_return retval = new PlexilParser.libraryActionDeclaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LIBRARY_ACTION_KYWD50=null;
		Token NCNAME51=null;
		Token SEMICOLON53=null;
		ParserRuleReturnScope libraryInterfaceSpec52 =null;

		PlexilTreeNode LIBRARY_ACTION_KYWD50_tree=null;
		PlexilTreeNode NCNAME51_tree=null;
		PlexilTreeNode SEMICOLON53_tree=null;

		 m_paraphrases.push("in library action declaration"); 
		try {
			// antlr/Plexil.g:410:2: ( LIBRARY_ACTION_KYWD ^ NCNAME ( libraryInterfaceSpec )? SEMICOLON !)
			// antlr/Plexil.g:411:5: LIBRARY_ACTION_KYWD ^ NCNAME ( libraryInterfaceSpec )? SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			LIBRARY_ACTION_KYWD50=(Token)match(input,LIBRARY_ACTION_KYWD,FOLLOW_LIBRARY_ACTION_KYWD_in_libraryActionDeclaration1945); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			LIBRARY_ACTION_KYWD50_tree = (PlexilTreeNode)adaptor.create(LIBRARY_ACTION_KYWD50);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(LIBRARY_ACTION_KYWD50_tree, root_0);
			}

			NCNAME51=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_libraryActionDeclaration1948); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NCNAME51_tree = (PlexilTreeNode)adaptor.create(NCNAME51);
			adaptor.addChild(root_0, NCNAME51_tree);
			}

			// antlr/Plexil.g:411:33: ( libraryInterfaceSpec )?
			int alt15=2;
			int LA15_0 = input.LA(1);
			if ( (LA15_0==LPAREN) ) {
				alt15=1;
			}
			switch (alt15) {
				case 1 :
					// antlr/Plexil.g:411:33: libraryInterfaceSpec
					{
					pushFollow(FOLLOW_libraryInterfaceSpec_in_libraryActionDeclaration1950);
					libraryInterfaceSpec52=libraryInterfaceSpec();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, libraryInterfaceSpec52.getTree());

					}
					break;

			}

			SEMICOLON53=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_libraryActionDeclaration1953); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "libraryActionDeclaration"


	public static class libraryInterfaceSpec_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "libraryInterfaceSpec"
	// antlr/Plexil.g:414:1: libraryInterfaceSpec : LPAREN ( libraryParamSpec ( COMMA libraryParamSpec )* )? RPAREN -> ^( PARAMETERS ( libraryParamSpec )* ) ;
	public final PlexilParser.libraryInterfaceSpec_return libraryInterfaceSpec() throws RecognitionException {
		PlexilParser.libraryInterfaceSpec_return retval = new PlexilParser.libraryInterfaceSpec_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LPAREN54=null;
		Token COMMA56=null;
		Token RPAREN58=null;
		ParserRuleReturnScope libraryParamSpec55 =null;
		ParserRuleReturnScope libraryParamSpec57 =null;

		PlexilTreeNode LPAREN54_tree=null;
		PlexilTreeNode COMMA56_tree=null;
		PlexilTreeNode RPAREN58_tree=null;
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleSubtreeStream stream_libraryParamSpec=new RewriteRuleSubtreeStream(adaptor,"rule libraryParamSpec");

		 m_paraphrases.push("in library action interface declaration"); 
		try {
			// antlr/Plexil.g:417:2: ( LPAREN ( libraryParamSpec ( COMMA libraryParamSpec )* )? RPAREN -> ^( PARAMETERS ( libraryParamSpec )* ) )
			// antlr/Plexil.g:418:5: LPAREN ( libraryParamSpec ( COMMA libraryParamSpec )* )? RPAREN
			{
			LPAREN54=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_libraryInterfaceSpec1978); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN54);

			// antlr/Plexil.g:418:12: ( libraryParamSpec ( COMMA libraryParamSpec )* )?
			int alt17=2;
			int LA17_0 = input.LA(1);
			if ( ((LA17_0 >= IN_KYWD && LA17_0 <= IN_OUT_KYWD)) ) {
				alt17=1;
			}
			switch (alt17) {
				case 1 :
					// antlr/Plexil.g:418:14: libraryParamSpec ( COMMA libraryParamSpec )*
					{
					pushFollow(FOLLOW_libraryParamSpec_in_libraryInterfaceSpec1982);
					libraryParamSpec55=libraryParamSpec();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_libraryParamSpec.add(libraryParamSpec55.getTree());
					// antlr/Plexil.g:418:31: ( COMMA libraryParamSpec )*
					loop16:
					while (true) {
						int alt16=2;
						int LA16_0 = input.LA(1);
						if ( (LA16_0==COMMA) ) {
							alt16=1;
						}

						switch (alt16) {
						case 1 :
							// antlr/Plexil.g:418:33: COMMA libraryParamSpec
							{
							COMMA56=(Token)match(input,COMMA,FOLLOW_COMMA_in_libraryInterfaceSpec1986); if (state.failed) return retval; 
							if ( state.backtracking==0 ) stream_COMMA.add(COMMA56);

							pushFollow(FOLLOW_libraryParamSpec_in_libraryInterfaceSpec1988);
							libraryParamSpec57=libraryParamSpec();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) stream_libraryParamSpec.add(libraryParamSpec57.getTree());
							}
							break;

						default :
							break loop16;
						}
					}

					}
					break;

			}

			RPAREN58=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_libraryInterfaceSpec1996); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN58);

			// AST REWRITE
			// elements: libraryParamSpec
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 419:5: -> ^( PARAMETERS ( libraryParamSpec )* )
			{
				// antlr/Plexil.g:419:8: ^( PARAMETERS ( libraryParamSpec )* )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(PARAMETERS, "PARAMETERS"), root_1);
				// antlr/Plexil.g:419:21: ( libraryParamSpec )*
				while ( stream_libraryParamSpec.hasNext() ) {
					adaptor.addChild(root_1, stream_libraryParamSpec.nextTree());
				}
				stream_libraryParamSpec.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "libraryInterfaceSpec"


	public static class libraryParamSpec_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "libraryParamSpec"
	// antlr/Plexil.g:422:1: libraryParamSpec : ( IN_KYWD ^| IN_OUT_KYWD ^) baseTypeName ( ( NCNAME LBRACKET )=> NCNAME LBRACKET ! INT RBRACKET !| NCNAME ) ;
	public final PlexilParser.libraryParamSpec_return libraryParamSpec() throws RecognitionException {
		PlexilParser.libraryParamSpec_return retval = new PlexilParser.libraryParamSpec_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token IN_KYWD59=null;
		Token IN_OUT_KYWD60=null;
		Token NCNAME62=null;
		Token LBRACKET63=null;
		Token INT64=null;
		Token RBRACKET65=null;
		Token NCNAME66=null;
		ParserRuleReturnScope baseTypeName61 =null;

		PlexilTreeNode IN_KYWD59_tree=null;
		PlexilTreeNode IN_OUT_KYWD60_tree=null;
		PlexilTreeNode NCNAME62_tree=null;
		PlexilTreeNode LBRACKET63_tree=null;
		PlexilTreeNode INT64_tree=null;
		PlexilTreeNode RBRACKET65_tree=null;
		PlexilTreeNode NCNAME66_tree=null;

		try {
			// antlr/Plexil.g:422:18: ( ( IN_KYWD ^| IN_OUT_KYWD ^) baseTypeName ( ( NCNAME LBRACKET )=> NCNAME LBRACKET ! INT RBRACKET !| NCNAME ) )
			// antlr/Plexil.g:423:2: ( IN_KYWD ^| IN_OUT_KYWD ^) baseTypeName ( ( NCNAME LBRACKET )=> NCNAME LBRACKET ! INT RBRACKET !| NCNAME )
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			// antlr/Plexil.g:423:2: ( IN_KYWD ^| IN_OUT_KYWD ^)
			int alt18=2;
			int LA18_0 = input.LA(1);
			if ( (LA18_0==IN_KYWD) ) {
				alt18=1;
			}
			else if ( (LA18_0==IN_OUT_KYWD) ) {
				alt18=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 18, 0, input);
				throw nvae;
			}

			switch (alt18) {
				case 1 :
					// antlr/Plexil.g:423:4: IN_KYWD ^
					{
					IN_KYWD59=(Token)match(input,IN_KYWD,FOLLOW_IN_KYWD_in_libraryParamSpec2022); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					IN_KYWD59_tree = (PlexilTreeNode)adaptor.create(IN_KYWD59);
					root_0 = (PlexilTreeNode)adaptor.becomeRoot(IN_KYWD59_tree, root_0);
					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:423:15: IN_OUT_KYWD ^
					{
					IN_OUT_KYWD60=(Token)match(input,IN_OUT_KYWD,FOLLOW_IN_OUT_KYWD_in_libraryParamSpec2027); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					IN_OUT_KYWD60_tree = (PlexilTreeNode)adaptor.create(IN_OUT_KYWD60);
					root_0 = (PlexilTreeNode)adaptor.becomeRoot(IN_OUT_KYWD60_tree, root_0);
					}

					}
					break;

			}

			pushFollow(FOLLOW_baseTypeName_in_libraryParamSpec2033);
			baseTypeName61=baseTypeName();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, baseTypeName61.getTree());

			// antlr/Plexil.g:425:2: ( ( NCNAME LBRACKET )=> NCNAME LBRACKET ! INT RBRACKET !| NCNAME )
			int alt19=2;
			int LA19_0 = input.LA(1);
			if ( (LA19_0==NCNAME) ) {
				int LA19_1 = input.LA(2);
				if ( (LA19_1==LBRACKET) && (synpred4_Plexil())) {
					alt19=1;
				}
				else if ( (LA19_1==COMMA||LA19_1==RPAREN) ) {
					alt19=2;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 19, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 19, 0, input);
				throw nvae;
			}

			switch (alt19) {
				case 1 :
					// antlr/Plexil.g:425:4: ( NCNAME LBRACKET )=> NCNAME LBRACKET ! INT RBRACKET !
					{
					NCNAME62=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_libraryParamSpec2046); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					NCNAME62_tree = (PlexilTreeNode)adaptor.create(NCNAME62);
					adaptor.addChild(root_0, NCNAME62_tree);
					}

					LBRACKET63=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_libraryParamSpec2048); if (state.failed) return retval;
					INT64=(Token)match(input,INT,FOLLOW_INT_in_libraryParamSpec2051); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					INT64_tree = (PlexilTreeNode)adaptor.create(INT64);
					adaptor.addChild(root_0, INT64_tree);
					}

					RBRACKET65=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_libraryParamSpec2053); if (state.failed) return retval;
					}
					break;
				case 2 :
					// antlr/Plexil.g:426:4: NCNAME
					{
					NCNAME66=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_libraryParamSpec2060); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					NCNAME66_tree = (PlexilTreeNode)adaptor.create(NCNAME66);
					adaptor.addChild(root_0, NCNAME66_tree);
					}

					}
					break;

			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "libraryParamSpec"


	public static class action_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "action"
	// antlr/Plexil.g:434:1: action : (actionId= NCNAME COLON )? rest= baseAction -> ^( ACTION ( $actionId)? $rest) ;
	public final PlexilParser.action_return action() throws RecognitionException {
		PlexilParser.action_return retval = new PlexilParser.action_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token actionId=null;
		Token COLON67=null;
		ParserRuleReturnScope rest =null;

		PlexilTreeNode actionId_tree=null;
		PlexilTreeNode COLON67_tree=null;
		RewriteRuleTokenStream stream_COLON=new RewriteRuleTokenStream(adaptor,"token COLON");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_baseAction=new RewriteRuleSubtreeStream(adaptor,"rule baseAction");

		 m_paraphrases.push("in action"); 
		try {
			// antlr/Plexil.g:437:2: ( (actionId= NCNAME COLON )? rest= baseAction -> ^( ACTION ( $actionId)? $rest) )
			// antlr/Plexil.g:438:5: (actionId= NCNAME COLON )? rest= baseAction
			{
			// antlr/Plexil.g:438:5: (actionId= NCNAME COLON )?
			int alt20=2;
			int LA20_0 = input.LA(1);
			if ( (LA20_0==NCNAME) ) {
				int LA20_1 = input.LA(2);
				if ( (LA20_1==COLON) ) {
					alt20=1;
				}
			}
			switch (alt20) {
				case 1 :
					// antlr/Plexil.g:438:6: actionId= NCNAME COLON
					{
					actionId=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_action2095); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_NCNAME.add(actionId);

					COLON67=(Token)match(input,COLON,FOLLOW_COLON_in_action2097); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_COLON.add(COLON67);

					}
					break;

			}

			pushFollow(FOLLOW_baseAction_in_action2104);
			rest=baseAction();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_baseAction.add(rest.getTree());
			// AST REWRITE
			// elements: rest, actionId
			// token labels: actionId
			// rule labels: rest, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleTokenStream stream_actionId=new RewriteRuleTokenStream(adaptor,"token actionId",actionId);
			RewriteRuleSubtreeStream stream_rest=new RewriteRuleSubtreeStream(adaptor,"rule rest",rest!=null?rest.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 440:5: -> ^( ACTION ( $actionId)? $rest)
			{
				// antlr/Plexil.g:440:8: ^( ACTION ( $actionId)? $rest)
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ACTION, "ACTION"), root_1);
				// antlr/Plexil.g:440:18: ( $actionId)?
				if ( stream_actionId.hasNext() ) {
					adaptor.addChild(root_1, stream_actionId.nextNode());
				}
				stream_actionId.reset();

				adaptor.addChild(root_1, stream_rest.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "action"


	public static class baseAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "baseAction"
	// antlr/Plexil.g:443:1: baseAction : ( compoundAction | simpleAction | block );
	public final PlexilParser.baseAction_return baseAction() throws RecognitionException {
		PlexilParser.baseAction_return retval = new PlexilParser.baseAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope compoundAction68 =null;
		ParserRuleReturnScope simpleAction69 =null;
		ParserRuleReturnScope block70 =null;


		try {
			// antlr/Plexil.g:443:12: ( compoundAction | simpleAction | block )
			int alt21=3;
			switch ( input.LA(1) ) {
			case FOR_KYWD:
			case IF_KYWD:
			case ON_COMMAND_KYWD:
			case ON_MESSAGE_KYWD:
			case WHILE_KYWD:
				{
				alt21=1;
				}
				break;
			case LIBRARY_CALL_KYWD:
			case LPAREN:
			case NCNAME:
			case REQUEST_KYWD:
			case SYNCHRONOUS_COMMAND_KYWD:
			case UPDATE_KYWD:
			case WAIT_KYWD:
				{
				alt21=2;
				}
				break;
			case CONCURRENCE_KYWD:
			case LBRACE:
			case SEQUENCE_KYWD:
			case TRY_KYWD:
			case UNCHECKED_SEQUENCE_KYWD:
				{
				alt21=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 21, 0, input);
				throw nvae;
			}
			switch (alt21) {
				case 1 :
					// antlr/Plexil.g:443:14: compoundAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_compoundAction_in_baseAction2131);
					compoundAction68=compoundAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, compoundAction68.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:443:31: simpleAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_simpleAction_in_baseAction2135);
					simpleAction69=simpleAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, simpleAction69.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:443:46: block
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_block_in_baseAction2139);
					block70=block();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, block70.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "baseAction"


	public static class compoundAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "compoundAction"
	// antlr/Plexil.g:445:1: compoundAction : ( forAction | ifAction | onCommandAction | onMessageAction | whileAction );
	public final PlexilParser.compoundAction_return compoundAction() throws RecognitionException {
		PlexilParser.compoundAction_return retval = new PlexilParser.compoundAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope forAction71 =null;
		ParserRuleReturnScope ifAction72 =null;
		ParserRuleReturnScope onCommandAction73 =null;
		ParserRuleReturnScope onMessageAction74 =null;
		ParserRuleReturnScope whileAction75 =null;


		try {
			// antlr/Plexil.g:445:16: ( forAction | ifAction | onCommandAction | onMessageAction | whileAction )
			int alt22=5;
			switch ( input.LA(1) ) {
			case FOR_KYWD:
				{
				alt22=1;
				}
				break;
			case IF_KYWD:
				{
				alt22=2;
				}
				break;
			case ON_COMMAND_KYWD:
				{
				alt22=3;
				}
				break;
			case ON_MESSAGE_KYWD:
				{
				alt22=4;
				}
				break;
			case WHILE_KYWD:
				{
				alt22=5;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 22, 0, input);
				throw nvae;
			}
			switch (alt22) {
				case 1 :
					// antlr/Plexil.g:445:18: forAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_forAction_in_compoundAction2149);
					forAction71=forAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, forAction71.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:445:30: ifAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_ifAction_in_compoundAction2153);
					ifAction72=ifAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, ifAction72.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:445:41: onCommandAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_onCommandAction_in_compoundAction2157);
					onCommandAction73=onCommandAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, onCommandAction73.getTree());

					}
					break;
				case 4 :
					// antlr/Plexil.g:445:59: onMessageAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_onMessageAction_in_compoundAction2161);
					onMessageAction74=onMessageAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, onMessageAction74.getTree());

					}
					break;
				case 5 :
					// antlr/Plexil.g:445:77: whileAction
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_whileAction_in_compoundAction2165);
					whileAction75=whileAction();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, whileAction75.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "compoundAction"


	public static class simpleAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "simpleAction"
	// antlr/Plexil.g:448:1: simpleAction : ( ( NCNAME ( LBRACKET | EQUALS ) )=> assignment | ( ( NCNAME LPAREN ) | LPAREN )=> commandInvocation SEMICOLON !| libraryCall | request | update | synchCmd | waitBuiltin );
	public final PlexilParser.simpleAction_return simpleAction() throws RecognitionException {
		PlexilParser.simpleAction_return retval = new PlexilParser.simpleAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token SEMICOLON78=null;
		ParserRuleReturnScope assignment76 =null;
		ParserRuleReturnScope commandInvocation77 =null;
		ParserRuleReturnScope libraryCall79 =null;
		ParserRuleReturnScope request80 =null;
		ParserRuleReturnScope update81 =null;
		ParserRuleReturnScope synchCmd82 =null;
		ParserRuleReturnScope waitBuiltin83 =null;

		PlexilTreeNode SEMICOLON78_tree=null;

		try {
			// antlr/Plexil.g:448:14: ( ( NCNAME ( LBRACKET | EQUALS ) )=> assignment | ( ( NCNAME LPAREN ) | LPAREN )=> commandInvocation SEMICOLON !| libraryCall | request | update | synchCmd | waitBuiltin )
			int alt23=7;
			int LA23_0 = input.LA(1);
			if ( (LA23_0==NCNAME) ) {
				int LA23_1 = input.LA(2);
				if ( (LA23_1==LBRACKET) && (synpred5_Plexil())) {
					alt23=1;
				}
				else if ( (LA23_1==EQUALS) && (synpred5_Plexil())) {
					alt23=1;
				}
				else if ( (LA23_1==LPAREN) && (synpred6_Plexil())) {
					alt23=2;
				}

			}
			else if ( (LA23_0==LPAREN) && (synpred6_Plexil())) {
				alt23=2;
			}
			else if ( (LA23_0==LIBRARY_CALL_KYWD) ) {
				alt23=3;
			}
			else if ( (LA23_0==REQUEST_KYWD) ) {
				alt23=4;
			}
			else if ( (LA23_0==UPDATE_KYWD) ) {
				alt23=5;
			}
			else if ( (LA23_0==SYNCHRONOUS_COMMAND_KYWD) ) {
				alt23=6;
			}
			else if ( (LA23_0==WAIT_KYWD) ) {
				alt23=7;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 23, 0, input);
				throw nvae;
			}

			switch (alt23) {
				case 1 :
					// antlr/Plexil.g:449:5: ( NCNAME ( LBRACKET | EQUALS ) )=> assignment
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_assignment_in_simpleAction2193);
					assignment76=assignment();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, assignment76.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:450:5: ( ( NCNAME LPAREN ) | LPAREN )=> commandInvocation SEMICOLON !
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_commandInvocation_in_simpleAction2213);
					commandInvocation77=commandInvocation();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, commandInvocation77.getTree());

					SEMICOLON78=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_simpleAction2215); if (state.failed) return retval;
					}
					break;
				case 3 :
					// antlr/Plexil.g:451:5: libraryCall
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_libraryCall_in_simpleAction2222);
					libraryCall79=libraryCall();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, libraryCall79.getTree());

					}
					break;
				case 4 :
					// antlr/Plexil.g:452:5: request
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_request_in_simpleAction2228);
					request80=request();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, request80.getTree());

					}
					break;
				case 5 :
					// antlr/Plexil.g:453:5: update
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_update_in_simpleAction2234);
					update81=update();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, update81.getTree());

					}
					break;
				case 6 :
					// antlr/Plexil.g:454:5: synchCmd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_synchCmd_in_simpleAction2240);
					synchCmd82=synchCmd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, synchCmd82.getTree());

					}
					break;
				case 7 :
					// antlr/Plexil.g:455:5: waitBuiltin
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_waitBuiltin_in_simpleAction2246);
					waitBuiltin83=waitBuiltin();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, waitBuiltin83.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "simpleAction"


	public static class forAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "forAction"
	// antlr/Plexil.g:458:1: forAction : FOR_KYWD LPAREN baseTypeName NCNAME EQUALS loopvarinit= expression SEMICOLON endtest= expression SEMICOLON loopvarupdate= expression RPAREN action -> ^( FOR_KYWD ^( VARIABLE_DECLARATION baseTypeName NCNAME $loopvarinit) $endtest $loopvarupdate action ) ;
	public final PlexilParser.forAction_return forAction() throws RecognitionException {
		PlexilParser.forAction_return retval = new PlexilParser.forAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token FOR_KYWD84=null;
		Token LPAREN85=null;
		Token NCNAME87=null;
		Token EQUALS88=null;
		Token SEMICOLON89=null;
		Token SEMICOLON90=null;
		Token RPAREN91=null;
		ParserRuleReturnScope loopvarinit =null;
		ParserRuleReturnScope endtest =null;
		ParserRuleReturnScope loopvarupdate =null;
		ParserRuleReturnScope baseTypeName86 =null;
		ParserRuleReturnScope action92 =null;

		PlexilTreeNode FOR_KYWD84_tree=null;
		PlexilTreeNode LPAREN85_tree=null;
		PlexilTreeNode NCNAME87_tree=null;
		PlexilTreeNode EQUALS88_tree=null;
		PlexilTreeNode SEMICOLON89_tree=null;
		PlexilTreeNode SEMICOLON90_tree=null;
		PlexilTreeNode RPAREN91_tree=null;
		RewriteRuleTokenStream stream_EQUALS=new RewriteRuleTokenStream(adaptor,"token EQUALS");
		RewriteRuleTokenStream stream_SEMICOLON=new RewriteRuleTokenStream(adaptor,"token SEMICOLON");
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_FOR_KYWD=new RewriteRuleTokenStream(adaptor,"token FOR_KYWD");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_expression=new RewriteRuleSubtreeStream(adaptor,"rule expression");
		RewriteRuleSubtreeStream stream_baseTypeName=new RewriteRuleSubtreeStream(adaptor,"rule baseTypeName");
		RewriteRuleSubtreeStream stream_action=new RewriteRuleSubtreeStream(adaptor,"rule action");

		 m_paraphrases.push("in \"for\" statement"); 
		try {
			// antlr/Plexil.g:461:2: ( FOR_KYWD LPAREN baseTypeName NCNAME EQUALS loopvarinit= expression SEMICOLON endtest= expression SEMICOLON loopvarupdate= expression RPAREN action -> ^( FOR_KYWD ^( VARIABLE_DECLARATION baseTypeName NCNAME $loopvarinit) $endtest $loopvarupdate action ) )
			// antlr/Plexil.g:462:5: FOR_KYWD LPAREN baseTypeName NCNAME EQUALS loopvarinit= expression SEMICOLON endtest= expression SEMICOLON loopvarupdate= expression RPAREN action
			{
			FOR_KYWD84=(Token)match(input,FOR_KYWD,FOLLOW_FOR_KYWD_in_forAction2271); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_FOR_KYWD.add(FOR_KYWD84);

			LPAREN85=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_forAction2278); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN85);

			pushFollow(FOLLOW_baseTypeName_in_forAction2280);
			baseTypeName86=baseTypeName();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_baseTypeName.add(baseTypeName86.getTree());
			NCNAME87=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_forAction2282); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME87);

			EQUALS88=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_forAction2284); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_EQUALS.add(EQUALS88);

			pushFollow(FOLLOW_expression_in_forAction2288);
			loopvarinit=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_expression.add(loopvarinit.getTree());
			SEMICOLON89=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_forAction2294); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON89);

			pushFollow(FOLLOW_expression_in_forAction2298);
			endtest=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_expression.add(endtest.getTree());
			SEMICOLON90=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_forAction2304); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON90);

			pushFollow(FOLLOW_expression_in_forAction2308);
			loopvarupdate=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_expression.add(loopvarupdate.getTree());
			RPAREN91=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_forAction2314); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN91);

			pushFollow(FOLLOW_action_in_forAction2320);
			action92=action();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_action.add(action92.getTree());
			// AST REWRITE
			// elements: loopvarupdate, loopvarinit, FOR_KYWD, endtest, action, NCNAME, baseTypeName
			// token labels: 
			// rule labels: loopvarupdate, loopvarinit, endtest, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_loopvarupdate=new RewriteRuleSubtreeStream(adaptor,"rule loopvarupdate",loopvarupdate!=null?loopvarupdate.getTree():null);
			RewriteRuleSubtreeStream stream_loopvarinit=new RewriteRuleSubtreeStream(adaptor,"rule loopvarinit",loopvarinit!=null?loopvarinit.getTree():null);
			RewriteRuleSubtreeStream stream_endtest=new RewriteRuleSubtreeStream(adaptor,"rule endtest",endtest!=null?endtest.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 468:5: -> ^( FOR_KYWD ^( VARIABLE_DECLARATION baseTypeName NCNAME $loopvarinit) $endtest $loopvarupdate action )
			{
				// antlr/Plexil.g:468:8: ^( FOR_KYWD ^( VARIABLE_DECLARATION baseTypeName NCNAME $loopvarinit) $endtest $loopvarupdate action )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_FOR_KYWD.nextNode(), root_1);
				// antlr/Plexil.g:468:19: ^( VARIABLE_DECLARATION baseTypeName NCNAME $loopvarinit)
				{
				PlexilTreeNode root_2 = (PlexilTreeNode)adaptor.nil();
				root_2 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(VARIABLE_DECLARATION, "VARIABLE_DECLARATION"), root_2);
				adaptor.addChild(root_2, stream_baseTypeName.nextTree());
				adaptor.addChild(root_2, stream_NCNAME.nextNode());
				adaptor.addChild(root_2, stream_loopvarinit.nextTree());
				adaptor.addChild(root_1, root_2);
				}

				adaptor.addChild(root_1, stream_endtest.nextTree());
				adaptor.addChild(root_1, stream_loopvarupdate.nextTree());
				adaptor.addChild(root_1, stream_action.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "forAction"


	public static class ifAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "ifAction"
	// antlr/Plexil.g:471:1: ifAction : IF_KYWD ^ expression action ( ELSEIF_KYWD ! expression action )* ( ELSE_KYWD ! action )? ENDIF_KYWD ! ( SEMICOLON !)? ;
	public final PlexilParser.ifAction_return ifAction() throws RecognitionException {
		PlexilParser.ifAction_return retval = new PlexilParser.ifAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token IF_KYWD93=null;
		Token ELSEIF_KYWD96=null;
		Token ELSE_KYWD99=null;
		Token ENDIF_KYWD101=null;
		Token SEMICOLON102=null;
		ParserRuleReturnScope expression94 =null;
		ParserRuleReturnScope action95 =null;
		ParserRuleReturnScope expression97 =null;
		ParserRuleReturnScope action98 =null;
		ParserRuleReturnScope action100 =null;

		PlexilTreeNode IF_KYWD93_tree=null;
		PlexilTreeNode ELSEIF_KYWD96_tree=null;
		PlexilTreeNode ELSE_KYWD99_tree=null;
		PlexilTreeNode ENDIF_KYWD101_tree=null;
		PlexilTreeNode SEMICOLON102_tree=null;

		 m_paraphrases.push("in \"if\" statement"); 
		try {
			// antlr/Plexil.g:474:2: ( IF_KYWD ^ expression action ( ELSEIF_KYWD ! expression action )* ( ELSE_KYWD ! action )? ENDIF_KYWD ! ( SEMICOLON !)? )
			// antlr/Plexil.g:475:5: IF_KYWD ^ expression action ( ELSEIF_KYWD ! expression action )* ( ELSE_KYWD ! action )? ENDIF_KYWD ! ( SEMICOLON !)?
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			IF_KYWD93=(Token)match(input,IF_KYWD,FOLLOW_IF_KYWD_in_ifAction2374); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			IF_KYWD93_tree = (PlexilTreeNode)adaptor.create(IF_KYWD93);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(IF_KYWD93_tree, root_0);
			}

			pushFollow(FOLLOW_expression_in_ifAction2377);
			expression94=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression94.getTree());

			pushFollow(FOLLOW_action_in_ifAction2379);
			action95=action();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, action95.getTree());

			// antlr/Plexil.g:476:5: ( ELSEIF_KYWD ! expression action )*
			loop24:
			while (true) {
				int alt24=2;
				int LA24_0 = input.LA(1);
				if ( (LA24_0==ELSEIF_KYWD) ) {
					alt24=1;
				}

				switch (alt24) {
				case 1 :
					// antlr/Plexil.g:476:6: ELSEIF_KYWD ! expression action
					{
					ELSEIF_KYWD96=(Token)match(input,ELSEIF_KYWD,FOLLOW_ELSEIF_KYWD_in_ifAction2386); if (state.failed) return retval;
					pushFollow(FOLLOW_expression_in_ifAction2389);
					expression97=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression97.getTree());

					pushFollow(FOLLOW_action_in_ifAction2391);
					action98=action();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, action98.getTree());

					}
					break;

				default :
					break loop24;
				}
			}

			// antlr/Plexil.g:477:5: ( ELSE_KYWD ! action )?
			int alt25=2;
			int LA25_0 = input.LA(1);
			if ( (LA25_0==ELSE_KYWD) ) {
				alt25=1;
			}
			switch (alt25) {
				case 1 :
					// antlr/Plexil.g:477:6: ELSE_KYWD ! action
					{
					ELSE_KYWD99=(Token)match(input,ELSE_KYWD,FOLLOW_ELSE_KYWD_in_ifAction2400); if (state.failed) return retval;
					pushFollow(FOLLOW_action_in_ifAction2403);
					action100=action();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, action100.getTree());

					}
					break;

			}

			ENDIF_KYWD101=(Token)match(input,ENDIF_KYWD,FOLLOW_ENDIF_KYWD_in_ifAction2411); if (state.failed) return retval;
			// antlr/Plexil.g:479:14: ( SEMICOLON !)?
			int alt26=2;
			int LA26_0 = input.LA(1);
			if ( (LA26_0==SEMICOLON) ) {
				alt26=1;
			}
			switch (alt26) {
				case 1 :
					// antlr/Plexil.g:479:14: SEMICOLON !
					{
					SEMICOLON102=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_ifAction2418); if (state.failed) return retval;
					}
					break;

			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "ifAction"


	public static class onCommandAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "onCommandAction"
	// antlr/Plexil.g:482:1: onCommandAction : ON_COMMAND_KYWD ^ expression ( paramsSpec )? action ;
	public final PlexilParser.onCommandAction_return onCommandAction() throws RecognitionException {
		PlexilParser.onCommandAction_return retval = new PlexilParser.onCommandAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token ON_COMMAND_KYWD103=null;
		ParserRuleReturnScope expression104 =null;
		ParserRuleReturnScope paramsSpec105 =null;
		ParserRuleReturnScope action106 =null;

		PlexilTreeNode ON_COMMAND_KYWD103_tree=null;

		 m_paraphrases.push("in \"OnCommand\" statement"); 
		try {
			// antlr/Plexil.g:485:2: ( ON_COMMAND_KYWD ^ expression ( paramsSpec )? action )
			// antlr/Plexil.g:486:5: ON_COMMAND_KYWD ^ expression ( paramsSpec )? action
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			ON_COMMAND_KYWD103=(Token)match(input,ON_COMMAND_KYWD,FOLLOW_ON_COMMAND_KYWD_in_onCommandAction2446); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			ON_COMMAND_KYWD103_tree = (PlexilTreeNode)adaptor.create(ON_COMMAND_KYWD103);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(ON_COMMAND_KYWD103_tree, root_0);
			}

			pushFollow(FOLLOW_expression_in_onCommandAction2449);
			expression104=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression104.getTree());

			// antlr/Plexil.g:486:33: ( paramsSpec )?
			int alt27=2;
			int LA27_0 = input.LA(1);
			if ( (LA27_0==LPAREN) ) {
				switch ( input.LA(2) ) {
					case DATE_KYWD:
						{
						alt27=1;
						}
						break;
					case ANY_KYWD:
					case BOOLEAN_KYWD:
					case ELLIPSIS:
					case INTEGER_KYWD:
					case REAL_KYWD:
					case RPAREN:
					case STRING_KYWD:
						{
						alt27=1;
						}
						break;
					case DURATION_KYWD:
						{
						alt27=1;
						}
						break;
				}
			}
			switch (alt27) {
				case 1 :
					// antlr/Plexil.g:486:33: paramsSpec
					{
					pushFollow(FOLLOW_paramsSpec_in_onCommandAction2451);
					paramsSpec105=paramsSpec();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, paramsSpec105.getTree());

					}
					break;

			}

			pushFollow(FOLLOW_action_in_onCommandAction2454);
			action106=action();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, action106.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "onCommandAction"


	public static class onMessageAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "onMessageAction"
	// antlr/Plexil.g:489:1: onMessageAction : ON_MESSAGE_KYWD ^ expression action ;
	public final PlexilParser.onMessageAction_return onMessageAction() throws RecognitionException {
		PlexilParser.onMessageAction_return retval = new PlexilParser.onMessageAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token ON_MESSAGE_KYWD107=null;
		ParserRuleReturnScope expression108 =null;
		ParserRuleReturnScope action109 =null;

		PlexilTreeNode ON_MESSAGE_KYWD107_tree=null;

		 m_paraphrases.push("in \"OnMessage\" statement"); 
		try {
			// antlr/Plexil.g:492:2: ( ON_MESSAGE_KYWD ^ expression action )
			// antlr/Plexil.g:493:5: ON_MESSAGE_KYWD ^ expression action
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			ON_MESSAGE_KYWD107=(Token)match(input,ON_MESSAGE_KYWD,FOLLOW_ON_MESSAGE_KYWD_in_onMessageAction2479); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			ON_MESSAGE_KYWD107_tree = new OnMessageNode(ON_MESSAGE_KYWD107) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(ON_MESSAGE_KYWD107_tree, root_0);
			}

			pushFollow(FOLLOW_expression_in_onMessageAction2485);
			expression108=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression108.getTree());

			pushFollow(FOLLOW_action_in_onMessageAction2487);
			action109=action();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, action109.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "onMessageAction"


	public static class whileAction_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "whileAction"
	// antlr/Plexil.g:496:1: whileAction : WHILE_KYWD ^ expression action ;
	public final PlexilParser.whileAction_return whileAction() throws RecognitionException {
		PlexilParser.whileAction_return retval = new PlexilParser.whileAction_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token WHILE_KYWD110=null;
		ParserRuleReturnScope expression111 =null;
		ParserRuleReturnScope action112 =null;

		PlexilTreeNode WHILE_KYWD110_tree=null;

		 m_paraphrases.push("in \"while\" statement"); 
		try {
			// antlr/Plexil.g:499:2: ( WHILE_KYWD ^ expression action )
			// antlr/Plexil.g:500:5: WHILE_KYWD ^ expression action
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			WHILE_KYWD110=(Token)match(input,WHILE_KYWD,FOLLOW_WHILE_KYWD_in_whileAction2512); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			WHILE_KYWD110_tree = (PlexilTreeNode)adaptor.create(WHILE_KYWD110);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(WHILE_KYWD110_tree, root_0);
			}

			pushFollow(FOLLOW_expression_in_whileAction2515);
			expression111=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression111.getTree());

			pushFollow(FOLLOW_action_in_whileAction2517);
			action112=action();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, action112.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "whileAction"


	public static class synchCmd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "synchCmd"
	// antlr/Plexil.g:503:1: synchCmd : SYNCHRONOUS_COMMAND_KYWD ^ ( commandWithAssignment | commandInvocation ) ( TIMEOUT_KYWD ! expression ( COMMA ! expression )? )? SEMICOLON !;
	public final PlexilParser.synchCmd_return synchCmd() throws RecognitionException {
		PlexilParser.synchCmd_return retval = new PlexilParser.synchCmd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token SYNCHRONOUS_COMMAND_KYWD113=null;
		Token TIMEOUT_KYWD116=null;
		Token COMMA118=null;
		Token SEMICOLON120=null;
		ParserRuleReturnScope commandWithAssignment114 =null;
		ParserRuleReturnScope commandInvocation115 =null;
		ParserRuleReturnScope expression117 =null;
		ParserRuleReturnScope expression119 =null;

		PlexilTreeNode SYNCHRONOUS_COMMAND_KYWD113_tree=null;
		PlexilTreeNode TIMEOUT_KYWD116_tree=null;
		PlexilTreeNode COMMA118_tree=null;
		PlexilTreeNode SEMICOLON120_tree=null;

		 m_paraphrases.push("in \"SynchronousCommand\" statement"); 
		try {
			// antlr/Plexil.g:506:2: ( SYNCHRONOUS_COMMAND_KYWD ^ ( commandWithAssignment | commandInvocation ) ( TIMEOUT_KYWD ! expression ( COMMA ! expression )? )? SEMICOLON !)
			// antlr/Plexil.g:507:5: SYNCHRONOUS_COMMAND_KYWD ^ ( commandWithAssignment | commandInvocation ) ( TIMEOUT_KYWD ! expression ( COMMA ! expression )? )? SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			SYNCHRONOUS_COMMAND_KYWD113=(Token)match(input,SYNCHRONOUS_COMMAND_KYWD,FOLLOW_SYNCHRONOUS_COMMAND_KYWD_in_synchCmd2542); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			SYNCHRONOUS_COMMAND_KYWD113_tree = (PlexilTreeNode)adaptor.create(SYNCHRONOUS_COMMAND_KYWD113);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(SYNCHRONOUS_COMMAND_KYWD113_tree, root_0);
			}

			// antlr/Plexil.g:508:5: ( commandWithAssignment | commandInvocation )
			int alt28=2;
			int LA28_0 = input.LA(1);
			if ( (LA28_0==NCNAME) ) {
				int LA28_1 = input.LA(2);
				if ( (LA28_1==EQUALS||LA28_1==LBRACKET) ) {
					alt28=1;
				}
				else if ( (LA28_1==LPAREN) ) {
					alt28=2;
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
			else if ( (LA28_0==LPAREN) ) {
				alt28=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 28, 0, input);
				throw nvae;
			}

			switch (alt28) {
				case 1 :
					// antlr/Plexil.g:508:7: commandWithAssignment
					{
					pushFollow(FOLLOW_commandWithAssignment_in_synchCmd2551);
					commandWithAssignment114=commandWithAssignment();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, commandWithAssignment114.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:508:31: commandInvocation
					{
					pushFollow(FOLLOW_commandInvocation_in_synchCmd2555);
					commandInvocation115=commandInvocation();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, commandInvocation115.getTree());

					}
					break;

			}

			// antlr/Plexil.g:509:2: ( TIMEOUT_KYWD ! expression ( COMMA ! expression )? )?
			int alt30=2;
			int LA30_0 = input.LA(1);
			if ( (LA30_0==TIMEOUT_KYWD) ) {
				alt30=1;
			}
			switch (alt30) {
				case 1 :
					// antlr/Plexil.g:509:4: TIMEOUT_KYWD ! expression ( COMMA ! expression )?
					{
					TIMEOUT_KYWD116=(Token)match(input,TIMEOUT_KYWD,FOLLOW_TIMEOUT_KYWD_in_synchCmd2562); if (state.failed) return retval;
					pushFollow(FOLLOW_expression_in_synchCmd2565);
					expression117=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression117.getTree());

					// antlr/Plexil.g:509:29: ( COMMA ! expression )?
					int alt29=2;
					int LA29_0 = input.LA(1);
					if ( (LA29_0==COMMA) ) {
						alt29=1;
					}
					switch (alt29) {
						case 1 :
							// antlr/Plexil.g:509:31: COMMA ! expression
							{
							COMMA118=(Token)match(input,COMMA,FOLLOW_COMMA_in_synchCmd2569); if (state.failed) return retval;
							pushFollow(FOLLOW_expression_in_synchCmd2572);
							expression119=expression();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, expression119.getTree());

							}
							break;

					}

					}
					break;

			}

			SEMICOLON120=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_synchCmd2581); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "synchCmd"


	public static class waitBuiltin_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "waitBuiltin"
	// antlr/Plexil.g:513:1: waitBuiltin : WAIT_KYWD ^ expression ( COMMA ! ( variable | INT | DOUBLE ) )? SEMICOLON !;
	public final PlexilParser.waitBuiltin_return waitBuiltin() throws RecognitionException {
		PlexilParser.waitBuiltin_return retval = new PlexilParser.waitBuiltin_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token WAIT_KYWD121=null;
		Token COMMA123=null;
		Token INT125=null;
		Token DOUBLE126=null;
		Token SEMICOLON127=null;
		ParserRuleReturnScope expression122 =null;
		ParserRuleReturnScope variable124 =null;

		PlexilTreeNode WAIT_KYWD121_tree=null;
		PlexilTreeNode COMMA123_tree=null;
		PlexilTreeNode INT125_tree=null;
		PlexilTreeNode DOUBLE126_tree=null;
		PlexilTreeNode SEMICOLON127_tree=null;

		 m_paraphrases.push("in \"Wait\" statement"); 
		try {
			// antlr/Plexil.g:516:2: ( WAIT_KYWD ^ expression ( COMMA ! ( variable | INT | DOUBLE ) )? SEMICOLON !)
			// antlr/Plexil.g:517:2: WAIT_KYWD ^ expression ( COMMA ! ( variable | INT | DOUBLE ) )? SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			WAIT_KYWD121=(Token)match(input,WAIT_KYWD,FOLLOW_WAIT_KYWD_in_waitBuiltin2604); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			WAIT_KYWD121_tree = (PlexilTreeNode)adaptor.create(WAIT_KYWD121);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(WAIT_KYWD121_tree, root_0);
			}

			pushFollow(FOLLOW_expression_in_waitBuiltin2607);
			expression122=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression122.getTree());

			// antlr/Plexil.g:517:24: ( COMMA ! ( variable | INT | DOUBLE ) )?
			int alt32=2;
			int LA32_0 = input.LA(1);
			if ( (LA32_0==COMMA) ) {
				alt32=1;
			}
			switch (alt32) {
				case 1 :
					// antlr/Plexil.g:517:25: COMMA ! ( variable | INT | DOUBLE )
					{
					COMMA123=(Token)match(input,COMMA,FOLLOW_COMMA_in_waitBuiltin2610); if (state.failed) return retval;
					// antlr/Plexil.g:517:32: ( variable | INT | DOUBLE )
					int alt31=3;
					switch ( input.LA(1) ) {
					case NCNAME:
						{
						alt31=1;
						}
						break;
					case INT:
						{
						alt31=2;
						}
						break;
					case DOUBLE:
						{
						alt31=3;
						}
						break;
					default:
						if (state.backtracking>0) {state.failed=true; return retval;}
						NoViableAltException nvae =
							new NoViableAltException("", 31, 0, input);
						throw nvae;
					}
					switch (alt31) {
						case 1 :
							// antlr/Plexil.g:517:33: variable
							{
							pushFollow(FOLLOW_variable_in_waitBuiltin2614);
							variable124=variable();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, variable124.getTree());

							}
							break;
						case 2 :
							// antlr/Plexil.g:517:42: INT
							{
							INT125=(Token)match(input,INT,FOLLOW_INT_in_waitBuiltin2616); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							INT125_tree = (PlexilTreeNode)adaptor.create(INT125);
							adaptor.addChild(root_0, INT125_tree);
							}

							}
							break;
						case 3 :
							// antlr/Plexil.g:517:46: DOUBLE
							{
							DOUBLE126=(Token)match(input,DOUBLE,FOLLOW_DOUBLE_in_waitBuiltin2618); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							DOUBLE126_tree = (PlexilTreeNode)adaptor.create(DOUBLE126);
							adaptor.addChild(root_0, DOUBLE126_tree);
							}

							}
							break;

					}

					}
					break;

			}

			SEMICOLON127=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_waitBuiltin2623); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "waitBuiltin"


	public static class block_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "block"
	// antlr/Plexil.g:523:1: block : (variant= sequenceVariantKywd LBRACE -> $variant| LBRACE -> BLOCK ) ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* RBRACE -> ^( $block ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* ) ;
	public final PlexilParser.block_return block() throws RecognitionException {
		PlexilParser.block_return retval = new PlexilParser.block_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LBRACE128=null;
		Token LBRACE129=null;
		Token RBRACE134=null;
		ParserRuleReturnScope variant =null;
		ParserRuleReturnScope comment130 =null;
		ParserRuleReturnScope nodeDeclaration131 =null;
		ParserRuleReturnScope nodeAttribute132 =null;
		ParserRuleReturnScope action133 =null;

		PlexilTreeNode LBRACE128_tree=null;
		PlexilTreeNode LBRACE129_tree=null;
		PlexilTreeNode RBRACE134_tree=null;
		RewriteRuleTokenStream stream_RBRACE=new RewriteRuleTokenStream(adaptor,"token RBRACE");
		RewriteRuleTokenStream stream_LBRACE=new RewriteRuleTokenStream(adaptor,"token LBRACE");
		RewriteRuleSubtreeStream stream_nodeAttribute=new RewriteRuleSubtreeStream(adaptor,"rule nodeAttribute");
		RewriteRuleSubtreeStream stream_action=new RewriteRuleSubtreeStream(adaptor,"rule action");
		RewriteRuleSubtreeStream stream_comment=new RewriteRuleSubtreeStream(adaptor,"rule comment");
		RewriteRuleSubtreeStream stream_sequenceVariantKywd=new RewriteRuleSubtreeStream(adaptor,"rule sequenceVariantKywd");
		RewriteRuleSubtreeStream stream_nodeDeclaration=new RewriteRuleSubtreeStream(adaptor,"rule nodeDeclaration");

		 m_paraphrases.push("in block"); 
		try {
			// antlr/Plexil.g:526:2: ( (variant= sequenceVariantKywd LBRACE -> $variant| LBRACE -> BLOCK ) ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* RBRACE -> ^( $block ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* ) )
			// antlr/Plexil.g:527:5: (variant= sequenceVariantKywd LBRACE -> $variant| LBRACE -> BLOCK ) ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* RBRACE
			{
			// antlr/Plexil.g:527:5: (variant= sequenceVariantKywd LBRACE -> $variant| LBRACE -> BLOCK )
			int alt33=2;
			int LA33_0 = input.LA(1);
			if ( (LA33_0==CONCURRENCE_KYWD||LA33_0==SEQUENCE_KYWD||(LA33_0 >= TRY_KYWD && LA33_0 <= UNCHECKED_SEQUENCE_KYWD)) ) {
				alt33=1;
			}
			else if ( (LA33_0==LBRACE) ) {
				alt33=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 33, 0, input);
				throw nvae;
			}

			switch (alt33) {
				case 1 :
					// antlr/Plexil.g:527:6: variant= sequenceVariantKywd LBRACE
					{
					pushFollow(FOLLOW_sequenceVariantKywd_in_block2656);
					variant=sequenceVariantKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_sequenceVariantKywd.add(variant.getTree());
					LBRACE128=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_block2658); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_LBRACE.add(LBRACE128);

					// AST REWRITE
					// elements: variant
					// token labels: 
					// rule labels: variant, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_variant=new RewriteRuleSubtreeStream(adaptor,"rule variant",variant!=null?variant.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 527:41: -> $variant
					{
						adaptor.addChild(root_0, stream_variant.nextTree());
					}


					retval.tree = root_0;
					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:528:8: LBRACE
					{
					LBRACE129=(Token)match(input,LBRACE,FOLLOW_LBRACE_in_block2672); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_LBRACE.add(LBRACE129);

					// AST REWRITE
					// elements: 
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 528:15: -> BLOCK
					{
						adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(BLOCK, "BLOCK"));
					}


					retval.tree = root_0;
					}

					}
					break;

			}

			// antlr/Plexil.g:529:5: ( comment )?
			int alt34=2;
			int LA34_0 = input.LA(1);
			if ( (LA34_0==COMMENT_KYWD) ) {
				alt34=1;
			}
			switch (alt34) {
				case 1 :
					// antlr/Plexil.g:529:5: comment
					{
					pushFollow(FOLLOW_comment_in_block2683);
					comment130=comment();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_comment.add(comment130.getTree());
					}
					break;

			}

			// antlr/Plexil.g:530:5: ( nodeDeclaration )*
			loop35:
			while (true) {
				int alt35=2;
				int LA35_0 = input.LA(1);
				if ( (LA35_0==BOOLEAN_KYWD||LA35_0==DATE_KYWD||LA35_0==DURATION_KYWD||LA35_0==INTEGER_KYWD||(LA35_0 >= IN_KYWD && LA35_0 <= IN_OUT_KYWD)||LA35_0==REAL_KYWD||LA35_0==STRING_KYWD) ) {
					alt35=1;
				}

				switch (alt35) {
				case 1 :
					// antlr/Plexil.g:530:5: nodeDeclaration
					{
					pushFollow(FOLLOW_nodeDeclaration_in_block2690);
					nodeDeclaration131=nodeDeclaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_nodeDeclaration.add(nodeDeclaration131.getTree());
					}
					break;

				default :
					break loop35;
				}
			}

			// antlr/Plexil.g:531:5: ( nodeAttribute )*
			loop36:
			while (true) {
				int alt36=2;
				int LA36_0 = input.LA(1);
				if ( (LA36_0==END_CONDITION_KYWD||LA36_0==EXIT_CONDITION_KYWD||LA36_0==INVARIANT_CONDITION_KYWD||LA36_0==POST_CONDITION_KYWD||(LA36_0 >= PRE_CONDITION_KYWD && LA36_0 <= PRIORITY_KYWD)||LA36_0==REPEAT_CONDITION_KYWD||LA36_0==RESOURCE_KYWD||LA36_0==SKIP_CONDITION_KYWD||LA36_0==START_CONDITION_KYWD) ) {
					alt36=1;
				}

				switch (alt36) {
				case 1 :
					// antlr/Plexil.g:531:5: nodeAttribute
					{
					pushFollow(FOLLOW_nodeAttribute_in_block2697);
					nodeAttribute132=nodeAttribute();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_nodeAttribute.add(nodeAttribute132.getTree());
					}
					break;

				default :
					break loop36;
				}
			}

			// antlr/Plexil.g:532:5: ( action )*
			loop37:
			while (true) {
				int alt37=2;
				int LA37_0 = input.LA(1);
				if ( (LA37_0==CONCURRENCE_KYWD||LA37_0==FOR_KYWD||LA37_0==IF_KYWD||LA37_0==LBRACE||LA37_0==LIBRARY_CALL_KYWD||LA37_0==LPAREN||LA37_0==NCNAME||(LA37_0 >= ON_COMMAND_KYWD && LA37_0 <= ON_MESSAGE_KYWD)||LA37_0==REQUEST_KYWD||LA37_0==SEQUENCE_KYWD||LA37_0==SYNCHRONOUS_COMMAND_KYWD||(LA37_0 >= TRY_KYWD && LA37_0 <= UPDATE_KYWD)||(LA37_0 >= WAIT_KYWD && LA37_0 <= WHILE_KYWD)) ) {
					alt37=1;
				}

				switch (alt37) {
				case 1 :
					// antlr/Plexil.g:532:5: action
					{
					pushFollow(FOLLOW_action_in_block2704);
					action133=action();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_action.add(action133.getTree());
					}
					break;

				default :
					break loop37;
				}
			}

			RBRACE134=(Token)match(input,RBRACE,FOLLOW_RBRACE_in_block2711); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RBRACE.add(RBRACE134);

			// AST REWRITE
			// elements: comment, nodeDeclaration, action, nodeAttribute, block
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 534:2: -> ^( $block ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* )
			{
				// antlr/Plexil.g:534:5: ^( $block ( comment )? ( nodeDeclaration )* ( nodeAttribute )* ( action )* )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_retval.nextNode(), root_1);
				// antlr/Plexil.g:534:14: ( comment )?
				if ( stream_comment.hasNext() ) {
					adaptor.addChild(root_1, stream_comment.nextTree());
				}
				stream_comment.reset();

				// antlr/Plexil.g:534:23: ( nodeDeclaration )*
				while ( stream_nodeDeclaration.hasNext() ) {
					adaptor.addChild(root_1, stream_nodeDeclaration.nextTree());
				}
				stream_nodeDeclaration.reset();

				// antlr/Plexil.g:534:40: ( nodeAttribute )*
				while ( stream_nodeAttribute.hasNext() ) {
					adaptor.addChild(root_1, stream_nodeAttribute.nextTree());
				}
				stream_nodeAttribute.reset();

				// antlr/Plexil.g:534:55: ( action )*
				while ( stream_action.hasNext() ) {
					adaptor.addChild(root_1, stream_action.nextTree());
				}
				stream_action.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "block"


	public static class sequenceVariantKywd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "sequenceVariantKywd"
	// antlr/Plexil.g:537:1: sequenceVariantKywd : ( CONCURRENCE_KYWD | SEQUENCE_KYWD | UNCHECKED_SEQUENCE_KYWD | TRY_KYWD );
	public final PlexilParser.sequenceVariantKywd_return sequenceVariantKywd() throws RecognitionException {
		PlexilParser.sequenceVariantKywd_return retval = new PlexilParser.sequenceVariantKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set135=null;

		PlexilTreeNode set135_tree=null;

		try {
			// antlr/Plexil.g:537:21: ( CONCURRENCE_KYWD | SEQUENCE_KYWD | UNCHECKED_SEQUENCE_KYWD | TRY_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set135=input.LT(1);
			if ( input.LA(1)==CONCURRENCE_KYWD||input.LA(1)==SEQUENCE_KYWD||(input.LA(1) >= TRY_KYWD && input.LA(1) <= UNCHECKED_SEQUENCE_KYWD) ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set135));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "sequenceVariantKywd"


	public static class comment_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "comment"
	// antlr/Plexil.g:544:1: comment : COMMENT_KYWD ^ STRING SEMICOLON !;
	public final PlexilParser.comment_return comment() throws RecognitionException {
		PlexilParser.comment_return retval = new PlexilParser.comment_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token COMMENT_KYWD136=null;
		Token STRING137=null;
		Token SEMICOLON138=null;

		PlexilTreeNode COMMENT_KYWD136_tree=null;
		PlexilTreeNode STRING137_tree=null;
		PlexilTreeNode SEMICOLON138_tree=null;

		try {
			// antlr/Plexil.g:544:9: ( COMMENT_KYWD ^ STRING SEMICOLON !)
			// antlr/Plexil.g:544:11: COMMENT_KYWD ^ STRING SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			COMMENT_KYWD136=(Token)match(input,COMMENT_KYWD,FOLLOW_COMMENT_KYWD_in_comment2773); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			COMMENT_KYWD136_tree = (PlexilTreeNode)adaptor.create(COMMENT_KYWD136);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(COMMENT_KYWD136_tree, root_0);
			}

			STRING137=(Token)match(input,STRING,FOLLOW_STRING_in_comment2776); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			STRING137_tree = (PlexilTreeNode)adaptor.create(STRING137);
			adaptor.addChild(root_0, STRING137_tree);
			}

			SEMICOLON138=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_comment2778); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "comment"


	public static class nodeDeclaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeDeclaration"
	// antlr/Plexil.g:546:1: nodeDeclaration : ( interfaceDeclaration | variableDeclaration );
	public final PlexilParser.nodeDeclaration_return nodeDeclaration() throws RecognitionException {
		PlexilParser.nodeDeclaration_return retval = new PlexilParser.nodeDeclaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope interfaceDeclaration139 =null;
		ParserRuleReturnScope variableDeclaration140 =null;


		try {
			// antlr/Plexil.g:546:17: ( interfaceDeclaration | variableDeclaration )
			int alt38=2;
			int LA38_0 = input.LA(1);
			if ( ((LA38_0 >= IN_KYWD && LA38_0 <= IN_OUT_KYWD)) ) {
				alt38=1;
			}
			else if ( (LA38_0==BOOLEAN_KYWD||LA38_0==DATE_KYWD||LA38_0==DURATION_KYWD||LA38_0==INTEGER_KYWD||LA38_0==REAL_KYWD||LA38_0==STRING_KYWD) ) {
				alt38=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 38, 0, input);
				throw nvae;
			}

			switch (alt38) {
				case 1 :
					// antlr/Plexil.g:547:5: interfaceDeclaration
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_interfaceDeclaration_in_nodeDeclaration2792);
					interfaceDeclaration139=interfaceDeclaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, interfaceDeclaration139.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:548:5: variableDeclaration
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_variableDeclaration_in_nodeDeclaration2798);
					variableDeclaration140=variableDeclaration();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, variableDeclaration140.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeDeclaration"


	public static class nodeAttribute_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeAttribute"
	// antlr/Plexil.g:550:1: nodeAttribute : ( nodeCondition | priority | resource );
	public final PlexilParser.nodeAttribute_return nodeAttribute() throws RecognitionException {
		PlexilParser.nodeAttribute_return retval = new PlexilParser.nodeAttribute_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope nodeCondition141 =null;
		ParserRuleReturnScope priority142 =null;
		ParserRuleReturnScope resource143 =null;


		try {
			// antlr/Plexil.g:550:15: ( nodeCondition | priority | resource )
			int alt39=3;
			switch ( input.LA(1) ) {
			case END_CONDITION_KYWD:
			case EXIT_CONDITION_KYWD:
			case INVARIANT_CONDITION_KYWD:
			case POST_CONDITION_KYWD:
			case PRE_CONDITION_KYWD:
			case REPEAT_CONDITION_KYWD:
			case SKIP_CONDITION_KYWD:
			case START_CONDITION_KYWD:
				{
				alt39=1;
				}
				break;
			case PRIORITY_KYWD:
				{
				alt39=2;
				}
				break;
			case RESOURCE_KYWD:
				{
				alt39=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 39, 0, input);
				throw nvae;
			}
			switch (alt39) {
				case 1 :
					// antlr/Plexil.g:551:5: nodeCondition
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeCondition_in_nodeAttribute2810);
					nodeCondition141=nodeCondition();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeCondition141.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:552:5: priority
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_priority_in_nodeAttribute2816);
					priority142=priority();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, priority142.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:553:5: resource
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_resource_in_nodeAttribute2822);
					resource143=resource();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, resource143.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeAttribute"


	public static class nodeCondition_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeCondition"
	// antlr/Plexil.g:555:1: nodeCondition : conditionKywd ^ expression SEMICOLON !;
	public final PlexilParser.nodeCondition_return nodeCondition() throws RecognitionException {
		PlexilParser.nodeCondition_return retval = new PlexilParser.nodeCondition_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token SEMICOLON146=null;
		ParserRuleReturnScope conditionKywd144 =null;
		ParserRuleReturnScope expression145 =null;

		PlexilTreeNode SEMICOLON146_tree=null;

		 m_paraphrases.push("in condition"); 
		try {
			// antlr/Plexil.g:558:2: ( conditionKywd ^ expression SEMICOLON !)
			// antlr/Plexil.g:559:5: conditionKywd ^ expression SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_conditionKywd_in_nodeCondition2846);
			conditionKywd144=conditionKywd();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) root_0 = (PlexilTreeNode)adaptor.becomeRoot(conditionKywd144.getTree(), root_0);
			pushFollow(FOLLOW_expression_in_nodeCondition2849);
			expression145=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression145.getTree());

			SEMICOLON146=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_nodeCondition2851); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeCondition"


	public static class conditionKywd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "conditionKywd"
	// antlr/Plexil.g:561:1: conditionKywd : ( END_CONDITION_KYWD | EXIT_CONDITION_KYWD | INVARIANT_CONDITION_KYWD | POST_CONDITION_KYWD | PRE_CONDITION_KYWD | REPEAT_CONDITION_KYWD | SKIP_CONDITION_KYWD | START_CONDITION_KYWD );
	public final PlexilParser.conditionKywd_return conditionKywd() throws RecognitionException {
		PlexilParser.conditionKywd_return retval = new PlexilParser.conditionKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set147=null;

		PlexilTreeNode set147_tree=null;

		try {
			// antlr/Plexil.g:561:15: ( END_CONDITION_KYWD | EXIT_CONDITION_KYWD | INVARIANT_CONDITION_KYWD | POST_CONDITION_KYWD | PRE_CONDITION_KYWD | REPEAT_CONDITION_KYWD | SKIP_CONDITION_KYWD | START_CONDITION_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set147=input.LT(1);
			if ( input.LA(1)==END_CONDITION_KYWD||input.LA(1)==EXIT_CONDITION_KYWD||input.LA(1)==INVARIANT_CONDITION_KYWD||input.LA(1)==POST_CONDITION_KYWD||input.LA(1)==PRE_CONDITION_KYWD||input.LA(1)==REPEAT_CONDITION_KYWD||input.LA(1)==SKIP_CONDITION_KYWD||input.LA(1)==START_CONDITION_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set147));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "conditionKywd"


	public static class resource_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "resource"
	// antlr/Plexil.g:572:1: resource : RESOURCE_KYWD ^ NAME_KYWD ! EQUALS ! expression ( COMMA ! ( LOWER_BOUND_KYWD EQUALS ! expression | UPPER_BOUND_KYWD EQUALS ! expression | RELEASE_AT_TERM_KYWD EQUALS ! expression | PRIORITY_KYWD EQUALS !pe= expression ) )* SEMICOLON !;
	public final PlexilParser.resource_return resource() throws RecognitionException {
		PlexilParser.resource_return retval = new PlexilParser.resource_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token RESOURCE_KYWD148=null;
		Token NAME_KYWD149=null;
		Token EQUALS150=null;
		Token COMMA152=null;
		Token LOWER_BOUND_KYWD153=null;
		Token EQUALS154=null;
		Token UPPER_BOUND_KYWD156=null;
		Token EQUALS157=null;
		Token RELEASE_AT_TERM_KYWD159=null;
		Token EQUALS160=null;
		Token PRIORITY_KYWD162=null;
		Token EQUALS163=null;
		Token SEMICOLON164=null;
		ParserRuleReturnScope pe =null;
		ParserRuleReturnScope expression151 =null;
		ParserRuleReturnScope expression155 =null;
		ParserRuleReturnScope expression158 =null;
		ParserRuleReturnScope expression161 =null;

		PlexilTreeNode RESOURCE_KYWD148_tree=null;
		PlexilTreeNode NAME_KYWD149_tree=null;
		PlexilTreeNode EQUALS150_tree=null;
		PlexilTreeNode COMMA152_tree=null;
		PlexilTreeNode LOWER_BOUND_KYWD153_tree=null;
		PlexilTreeNode EQUALS154_tree=null;
		PlexilTreeNode UPPER_BOUND_KYWD156_tree=null;
		PlexilTreeNode EQUALS157_tree=null;
		PlexilTreeNode RELEASE_AT_TERM_KYWD159_tree=null;
		PlexilTreeNode EQUALS160_tree=null;
		PlexilTreeNode PRIORITY_KYWD162_tree=null;
		PlexilTreeNode EQUALS163_tree=null;
		PlexilTreeNode SEMICOLON164_tree=null;

		 m_paraphrases.push("in resource"); 
		try {
			// antlr/Plexil.g:575:2: ( RESOURCE_KYWD ^ NAME_KYWD ! EQUALS ! expression ( COMMA ! ( LOWER_BOUND_KYWD EQUALS ! expression | UPPER_BOUND_KYWD EQUALS ! expression | RELEASE_AT_TERM_KYWD EQUALS ! expression | PRIORITY_KYWD EQUALS !pe= expression ) )* SEMICOLON !)
			// antlr/Plexil.g:576:5: RESOURCE_KYWD ^ NAME_KYWD ! EQUALS ! expression ( COMMA ! ( LOWER_BOUND_KYWD EQUALS ! expression | UPPER_BOUND_KYWD EQUALS ! expression | RELEASE_AT_TERM_KYWD EQUALS ! expression | PRIORITY_KYWD EQUALS !pe= expression ) )* SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			RESOURCE_KYWD148=(Token)match(input,RESOURCE_KYWD,FOLLOW_RESOURCE_KYWD_in_resource2932); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			RESOURCE_KYWD148_tree = (PlexilTreeNode)adaptor.create(RESOURCE_KYWD148);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(RESOURCE_KYWD148_tree, root_0);
			}

			NAME_KYWD149=(Token)match(input,NAME_KYWD,FOLLOW_NAME_KYWD_in_resource2935); if (state.failed) return retval;
			EQUALS150=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_resource2938); if (state.failed) return retval;
			pushFollow(FOLLOW_expression_in_resource2941);
			expression151=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression151.getTree());

			// antlr/Plexil.g:577:9: ( COMMA ! ( LOWER_BOUND_KYWD EQUALS ! expression | UPPER_BOUND_KYWD EQUALS ! expression | RELEASE_AT_TERM_KYWD EQUALS ! expression | PRIORITY_KYWD EQUALS !pe= expression ) )*
			loop41:
			while (true) {
				int alt41=2;
				int LA41_0 = input.LA(1);
				if ( (LA41_0==COMMA) ) {
					alt41=1;
				}

				switch (alt41) {
				case 1 :
					// antlr/Plexil.g:577:11: COMMA ! ( LOWER_BOUND_KYWD EQUALS ! expression | UPPER_BOUND_KYWD EQUALS ! expression | RELEASE_AT_TERM_KYWD EQUALS ! expression | PRIORITY_KYWD EQUALS !pe= expression )
					{
					COMMA152=(Token)match(input,COMMA,FOLLOW_COMMA_in_resource2953); if (state.failed) return retval;
					// antlr/Plexil.g:578:11: ( LOWER_BOUND_KYWD EQUALS ! expression | UPPER_BOUND_KYWD EQUALS ! expression | RELEASE_AT_TERM_KYWD EQUALS ! expression | PRIORITY_KYWD EQUALS !pe= expression )
					int alt40=4;
					switch ( input.LA(1) ) {
					case LOWER_BOUND_KYWD:
						{
						alt40=1;
						}
						break;
					case UPPER_BOUND_KYWD:
						{
						alt40=2;
						}
						break;
					case RELEASE_AT_TERM_KYWD:
						{
						alt40=3;
						}
						break;
					case PRIORITY_KYWD:
						{
						alt40=4;
						}
						break;
					default:
						if (state.backtracking>0) {state.failed=true; return retval;}
						NoViableAltException nvae =
							new NoViableAltException("", 40, 0, input);
						throw nvae;
					}
					switch (alt40) {
						case 1 :
							// antlr/Plexil.g:578:13: LOWER_BOUND_KYWD EQUALS ! expression
							{
							LOWER_BOUND_KYWD153=(Token)match(input,LOWER_BOUND_KYWD,FOLLOW_LOWER_BOUND_KYWD_in_resource2968); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							LOWER_BOUND_KYWD153_tree = (PlexilTreeNode)adaptor.create(LOWER_BOUND_KYWD153);
							adaptor.addChild(root_0, LOWER_BOUND_KYWD153_tree);
							}

							EQUALS154=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_resource2970); if (state.failed) return retval;
							pushFollow(FOLLOW_expression_in_resource2973);
							expression155=expression();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, expression155.getTree());

							}
							break;
						case 2 :
							// antlr/Plexil.g:579:13: UPPER_BOUND_KYWD EQUALS ! expression
							{
							UPPER_BOUND_KYWD156=(Token)match(input,UPPER_BOUND_KYWD,FOLLOW_UPPER_BOUND_KYWD_in_resource2987); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							UPPER_BOUND_KYWD156_tree = (PlexilTreeNode)adaptor.create(UPPER_BOUND_KYWD156);
							adaptor.addChild(root_0, UPPER_BOUND_KYWD156_tree);
							}

							EQUALS157=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_resource2989); if (state.failed) return retval;
							pushFollow(FOLLOW_expression_in_resource2992);
							expression158=expression();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, expression158.getTree());

							}
							break;
						case 3 :
							// antlr/Plexil.g:580:9: RELEASE_AT_TERM_KYWD EQUALS ! expression
							{
							RELEASE_AT_TERM_KYWD159=(Token)match(input,RELEASE_AT_TERM_KYWD,FOLLOW_RELEASE_AT_TERM_KYWD_in_resource3002); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							RELEASE_AT_TERM_KYWD159_tree = (PlexilTreeNode)adaptor.create(RELEASE_AT_TERM_KYWD159);
							adaptor.addChild(root_0, RELEASE_AT_TERM_KYWD159_tree);
							}

							EQUALS160=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_resource3004); if (state.failed) return retval;
							pushFollow(FOLLOW_expression_in_resource3007);
							expression161=expression();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, expression161.getTree());

							}
							break;
						case 4 :
							// antlr/Plexil.g:581:8: PRIORITY_KYWD EQUALS !pe= expression
							{
							PRIORITY_KYWD162=(Token)match(input,PRIORITY_KYWD,FOLLOW_PRIORITY_KYWD_in_resource3016); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							PRIORITY_KYWD162_tree = (PlexilTreeNode)adaptor.create(PRIORITY_KYWD162);
							adaptor.addChild(root_0, PRIORITY_KYWD162_tree);
							}

							EQUALS163=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_resource3018); if (state.failed) return retval;
							pushFollow(FOLLOW_expression_in_resource3023);
							pe=expression();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, pe.getTree());

							}
							break;

					}

					}
					break;

				default :
					break loop41;
				}
			}

			SEMICOLON164=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_resource3056); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "resource"


	public static class priority_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "priority"
	// antlr/Plexil.g:587:1: priority : PRIORITY_KYWD ^ INT SEMICOLON !;
	public final PlexilParser.priority_return priority() throws RecognitionException {
		PlexilParser.priority_return retval = new PlexilParser.priority_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token PRIORITY_KYWD165=null;
		Token INT166=null;
		Token SEMICOLON167=null;

		PlexilTreeNode PRIORITY_KYWD165_tree=null;
		PlexilTreeNode INT166_tree=null;
		PlexilTreeNode SEMICOLON167_tree=null;

		 m_paraphrases.push("in priority"); 
		try {
			// antlr/Plexil.g:590:2: ( PRIORITY_KYWD ^ INT SEMICOLON !)
			// antlr/Plexil.g:590:4: PRIORITY_KYWD ^ INT SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			PRIORITY_KYWD165=(Token)match(input,PRIORITY_KYWD,FOLLOW_PRIORITY_KYWD_in_priority3078); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			PRIORITY_KYWD165_tree = new PriorityNode(PRIORITY_KYWD165) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(PRIORITY_KYWD165_tree, root_0);
			}

			INT166=(Token)match(input,INT,FOLLOW_INT_in_priority3084); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			INT166_tree = (PlexilTreeNode)adaptor.create(INT166);
			adaptor.addChild(root_0, INT166_tree);
			}

			SEMICOLON167=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_priority3086); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "priority"


	public static class interfaceDeclaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "interfaceDeclaration"
	// antlr/Plexil.g:592:1: interfaceDeclaration : ( in | inOut );
	public final PlexilParser.interfaceDeclaration_return interfaceDeclaration() throws RecognitionException {
		PlexilParser.interfaceDeclaration_return retval = new PlexilParser.interfaceDeclaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope in168 =null;
		ParserRuleReturnScope inOut169 =null;


		try {
			// antlr/Plexil.g:592:22: ( in | inOut )
			int alt42=2;
			int LA42_0 = input.LA(1);
			if ( (LA42_0==IN_KYWD) ) {
				alt42=1;
			}
			else if ( (LA42_0==IN_OUT_KYWD) ) {
				alt42=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 42, 0, input);
				throw nvae;
			}

			switch (alt42) {
				case 1 :
					// antlr/Plexil.g:592:24: in
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_in_in_interfaceDeclaration3096);
					in168=in();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, in168.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:592:29: inOut
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_inOut_in_interfaceDeclaration3100);
					inOut169=inOut();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, inOut169.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "interfaceDeclaration"


	public static class in_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "in"
	// antlr/Plexil.g:598:1: in : IN_KYWD ^ ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations ) SEMICOLON !;
	public final PlexilParser.in_return in() throws RecognitionException {
		PlexilParser.in_return retval = new PlexilParser.in_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token IN_KYWD170=null;
		Token NCNAME171=null;
		Token COMMA172=null;
		Token NCNAME173=null;
		Token SEMICOLON175=null;
		ParserRuleReturnScope interfaceDeclarations174 =null;

		PlexilTreeNode IN_KYWD170_tree=null;
		PlexilTreeNode NCNAME171_tree=null;
		PlexilTreeNode COMMA172_tree=null;
		PlexilTreeNode NCNAME173_tree=null;
		PlexilTreeNode SEMICOLON175_tree=null;

		 m_paraphrases.push("in \"In\" declaration"); 
		try {
			// antlr/Plexil.g:601:2: ( IN_KYWD ^ ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations ) SEMICOLON !)
			// antlr/Plexil.g:602:5: IN_KYWD ^ ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations ) SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			IN_KYWD170=(Token)match(input,IN_KYWD,FOLLOW_IN_KYWD_in_in3129); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			IN_KYWD170_tree = (PlexilTreeNode)adaptor.create(IN_KYWD170);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(IN_KYWD170_tree, root_0);
			}

			// antlr/Plexil.g:603:7: ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations )
			int alt44=2;
			int LA44_0 = input.LA(1);
			if ( (LA44_0==NCNAME) ) {
				alt44=1;
			}
			else if ( (LA44_0==BOOLEAN_KYWD||LA44_0==DATE_KYWD||LA44_0==DURATION_KYWD||LA44_0==INTEGER_KYWD||LA44_0==REAL_KYWD||LA44_0==STRING_KYWD) ) {
				alt44=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 44, 0, input);
				throw nvae;
			}

			switch (alt44) {
				case 1 :
					// antlr/Plexil.g:603:9: ( NCNAME ( COMMA ! NCNAME )* )
					{
					// antlr/Plexil.g:603:9: ( NCNAME ( COMMA ! NCNAME )* )
					// antlr/Plexil.g:603:10: NCNAME ( COMMA ! NCNAME )*
					{
					NCNAME171=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_in3142); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					NCNAME171_tree = (PlexilTreeNode)adaptor.create(NCNAME171);
					adaptor.addChild(root_0, NCNAME171_tree);
					}

					// antlr/Plexil.g:603:17: ( COMMA ! NCNAME )*
					loop43:
					while (true) {
						int alt43=2;
						int LA43_0 = input.LA(1);
						if ( (LA43_0==COMMA) ) {
							alt43=1;
						}

						switch (alt43) {
						case 1 :
							// antlr/Plexil.g:603:18: COMMA ! NCNAME
							{
							COMMA172=(Token)match(input,COMMA,FOLLOW_COMMA_in_in3145); if (state.failed) return retval;
							NCNAME173=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_in3148); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							NCNAME173_tree = (PlexilTreeNode)adaptor.create(NCNAME173);
							adaptor.addChild(root_0, NCNAME173_tree);
							}

							}
							break;

						default :
							break loop43;
						}
					}

					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:604:6: interfaceDeclarations
					{
					pushFollow(FOLLOW_interfaceDeclarations_in_in3158);
					interfaceDeclarations174=interfaceDeclarations();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, interfaceDeclarations174.getTree());

					}
					break;

			}

			SEMICOLON175=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_in3173); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "in"


	public static class inOut_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "inOut"
	// antlr/Plexil.g:609:1: inOut : IN_OUT_KYWD ^ ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations ) SEMICOLON !;
	public final PlexilParser.inOut_return inOut() throws RecognitionException {
		PlexilParser.inOut_return retval = new PlexilParser.inOut_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token IN_OUT_KYWD176=null;
		Token NCNAME177=null;
		Token COMMA178=null;
		Token NCNAME179=null;
		Token SEMICOLON181=null;
		ParserRuleReturnScope interfaceDeclarations180 =null;

		PlexilTreeNode IN_OUT_KYWD176_tree=null;
		PlexilTreeNode NCNAME177_tree=null;
		PlexilTreeNode COMMA178_tree=null;
		PlexilTreeNode NCNAME179_tree=null;
		PlexilTreeNode SEMICOLON181_tree=null;

		 m_paraphrases.push("in \"InOut\" declaration"); 
		try {
			// antlr/Plexil.g:612:2: ( IN_OUT_KYWD ^ ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations ) SEMICOLON !)
			// antlr/Plexil.g:613:5: IN_OUT_KYWD ^ ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations ) SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			IN_OUT_KYWD176=(Token)match(input,IN_OUT_KYWD,FOLLOW_IN_OUT_KYWD_in_inOut3200); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			IN_OUT_KYWD176_tree = (PlexilTreeNode)adaptor.create(IN_OUT_KYWD176);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(IN_OUT_KYWD176_tree, root_0);
			}

			// antlr/Plexil.g:614:7: ( ( NCNAME ( COMMA ! NCNAME )* ) | interfaceDeclarations )
			int alt46=2;
			int LA46_0 = input.LA(1);
			if ( (LA46_0==NCNAME) ) {
				alt46=1;
			}
			else if ( (LA46_0==BOOLEAN_KYWD||LA46_0==DATE_KYWD||LA46_0==DURATION_KYWD||LA46_0==INTEGER_KYWD||LA46_0==REAL_KYWD||LA46_0==STRING_KYWD) ) {
				alt46=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 46, 0, input);
				throw nvae;
			}

			switch (alt46) {
				case 1 :
					// antlr/Plexil.g:614:9: ( NCNAME ( COMMA ! NCNAME )* )
					{
					// antlr/Plexil.g:614:9: ( NCNAME ( COMMA ! NCNAME )* )
					// antlr/Plexil.g:614:10: NCNAME ( COMMA ! NCNAME )*
					{
					NCNAME177=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_inOut3212); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					NCNAME177_tree = (PlexilTreeNode)adaptor.create(NCNAME177);
					adaptor.addChild(root_0, NCNAME177_tree);
					}

					// antlr/Plexil.g:614:17: ( COMMA ! NCNAME )*
					loop45:
					while (true) {
						int alt45=2;
						int LA45_0 = input.LA(1);
						if ( (LA45_0==COMMA) ) {
							alt45=1;
						}

						switch (alt45) {
						case 1 :
							// antlr/Plexil.g:614:18: COMMA ! NCNAME
							{
							COMMA178=(Token)match(input,COMMA,FOLLOW_COMMA_in_inOut3215); if (state.failed) return retval;
							NCNAME179=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_inOut3218); if (state.failed) return retval;
							if ( state.backtracking==0 ) {
							NCNAME179_tree = (PlexilTreeNode)adaptor.create(NCNAME179);
							adaptor.addChild(root_0, NCNAME179_tree);
							}

							}
							break;

						default :
							break loop45;
						}
					}

					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:615:6: interfaceDeclarations
					{
					pushFollow(FOLLOW_interfaceDeclarations_in_inOut3228);
					interfaceDeclarations180=interfaceDeclarations();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, interfaceDeclarations180.getTree());

					}
					break;

			}

			SEMICOLON181=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_inOut3243); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "inOut"


	public static class interfaceDeclarations_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "interfaceDeclarations"
	// antlr/Plexil.g:620:1: interfaceDeclarations : tn= baseTypeName ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) ( COMMA ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )* ;
	public final PlexilParser.interfaceDeclarations_return interfaceDeclarations() throws RecognitionException {
		PlexilParser.interfaceDeclarations_return retval = new PlexilParser.interfaceDeclarations_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token COMMA184=null;
		ParserRuleReturnScope tn =null;
		ParserRuleReturnScope arrayVariableDecl182 =null;
		ParserRuleReturnScope scalarVariableDecl183 =null;
		ParserRuleReturnScope arrayVariableDecl185 =null;
		ParserRuleReturnScope scalarVariableDecl186 =null;

		PlexilTreeNode COMMA184_tree=null;

		try {
			// antlr/Plexil.g:620:23: (tn= baseTypeName ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) ( COMMA ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )* )
			// antlr/Plexil.g:621:5: tn= baseTypeName ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) ( COMMA ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )*
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_baseTypeName_in_interfaceDeclarations3261);
			tn=baseTypeName();
			state._fsp--;
			if (state.failed) return retval;
			// antlr/Plexil.g:622:5: ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] )
			int alt47=2;
			int LA47_0 = input.LA(1);
			if ( (LA47_0==NCNAME) ) {
				int LA47_1 = input.LA(2);
				if ( (LA47_1==LBRACKET) && (synpred7_Plexil())) {
					alt47=1;
				}
				else if ( (LA47_1==COMMA||LA47_1==EQUALS||LA47_1==SEMICOLON) ) {
					alt47=2;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 47, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 47, 0, input);
				throw nvae;
			}

			switch (alt47) {
				case 1 :
					// antlr/Plexil.g:622:7: ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start]
					{
					pushFollow(FOLLOW_arrayVariableDecl_in_interfaceDeclarations3278);
					arrayVariableDecl182=arrayVariableDecl((tn!=null?(tn.start):null));
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, arrayVariableDecl182.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:623:7: scalarVariableDecl[$tn.start]
					{
					pushFollow(FOLLOW_scalarVariableDecl_in_interfaceDeclarations3288);
					scalarVariableDecl183=scalarVariableDecl((tn!=null?(tn.start):null));
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, scalarVariableDecl183.getTree());

					}
					break;

			}

			// antlr/Plexil.g:625:2: ( COMMA ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )*
			loop49:
			while (true) {
				int alt49=2;
				int LA49_0 = input.LA(1);
				if ( (LA49_0==COMMA) ) {
					alt49=1;
				}

				switch (alt49) {
				case 1 :
					// antlr/Plexil.g:625:4: COMMA ! ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] )
					{
					COMMA184=(Token)match(input,COMMA,FOLLOW_COMMA_in_interfaceDeclarations3300); if (state.failed) return retval;
					// antlr/Plexil.g:626:7: ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] )
					int alt48=2;
					int LA48_0 = input.LA(1);
					if ( (LA48_0==NCNAME) ) {
						int LA48_1 = input.LA(2);
						if ( (LA48_1==LBRACKET) && (synpred8_Plexil())) {
							alt48=1;
						}
						else if ( (LA48_1==COMMA||LA48_1==EQUALS||LA48_1==SEMICOLON) ) {
							alt48=2;
						}

						else {
							if (state.backtracking>0) {state.failed=true; return retval;}
							int nvaeMark = input.mark();
							try {
								input.consume();
								NoViableAltException nvae =
									new NoViableAltException("", 48, 1, input);
								throw nvae;
							} finally {
								input.rewind(nvaeMark);
							}
						}

					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						NoViableAltException nvae =
							new NoViableAltException("", 48, 0, input);
						throw nvae;
					}

					switch (alt48) {
						case 1 :
							// antlr/Plexil.g:626:9: ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start]
							{
							pushFollow(FOLLOW_arrayVariableDecl_in_interfaceDeclarations3319);
							arrayVariableDecl185=arrayVariableDecl((tn!=null?(tn.start):null));
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, arrayVariableDecl185.getTree());

							}
							break;
						case 2 :
							// antlr/Plexil.g:627:9: scalarVariableDecl[$tn.start]
							{
							pushFollow(FOLLOW_scalarVariableDecl_in_interfaceDeclarations3331);
							scalarVariableDecl186=scalarVariableDecl((tn!=null?(tn.start):null));
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, scalarVariableDecl186.getTree());

							}
							break;

					}

					}
					break;

				default :
					break loop49;
				}
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "interfaceDeclarations"


	public static class variable_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "variable"
	// antlr/Plexil.g:632:1: variable : NCNAME ;
	public final PlexilParser.variable_return variable() throws RecognitionException {
		PlexilParser.variable_return retval = new PlexilParser.variable_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME187=null;

		PlexilTreeNode NCNAME187_tree=null;

		try {
			// antlr/Plexil.g:632:10: ( NCNAME )
			// antlr/Plexil.g:632:12: NCNAME
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			NCNAME187=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_variable3355); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NCNAME187_tree = new VariableNode(NCNAME187) ;
			adaptor.addChild(root_0, NCNAME187_tree);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "variable"


	public static class variableDeclaration_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "variableDeclaration"
	// antlr/Plexil.g:634:1: variableDeclaration : tn= baseTypeName ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) ( COMMA ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )* SEMICOLON -> ^( VARIABLE_DECLARATIONS ( scalarVariableDecl )* ( arrayVariableDecl )* ) ;
	public final PlexilParser.variableDeclaration_return variableDeclaration() throws RecognitionException {
		PlexilParser.variableDeclaration_return retval = new PlexilParser.variableDeclaration_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token COMMA190=null;
		Token SEMICOLON193=null;
		ParserRuleReturnScope tn =null;
		ParserRuleReturnScope arrayVariableDecl188 =null;
		ParserRuleReturnScope scalarVariableDecl189 =null;
		ParserRuleReturnScope arrayVariableDecl191 =null;
		ParserRuleReturnScope scalarVariableDecl192 =null;

		PlexilTreeNode COMMA190_tree=null;
		PlexilTreeNode SEMICOLON193_tree=null;
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_SEMICOLON=new RewriteRuleTokenStream(adaptor,"token SEMICOLON");
		RewriteRuleSubtreeStream stream_arrayVariableDecl=new RewriteRuleSubtreeStream(adaptor,"rule arrayVariableDecl");
		RewriteRuleSubtreeStream stream_baseTypeName=new RewriteRuleSubtreeStream(adaptor,"rule baseTypeName");
		RewriteRuleSubtreeStream stream_scalarVariableDecl=new RewriteRuleSubtreeStream(adaptor,"rule scalarVariableDecl");

		 m_paraphrases.push("in variable declaration"); 
		try {
			// antlr/Plexil.g:637:2: (tn= baseTypeName ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) ( COMMA ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )* SEMICOLON -> ^( VARIABLE_DECLARATIONS ( scalarVariableDecl )* ( arrayVariableDecl )* ) )
			// antlr/Plexil.g:638:5: tn= baseTypeName ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) ( COMMA ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )* SEMICOLON
			{
			pushFollow(FOLLOW_baseTypeName_in_variableDeclaration3385);
			tn=baseTypeName();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_baseTypeName.add(tn.getTree());
			// antlr/Plexil.g:639:5: ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] )
			int alt50=2;
			int LA50_0 = input.LA(1);
			if ( (LA50_0==NCNAME) ) {
				int LA50_1 = input.LA(2);
				if ( (LA50_1==LBRACKET) && (synpred9_Plexil())) {
					alt50=1;
				}
				else if ( (LA50_1==COMMA||LA50_1==EQUALS||LA50_1==SEMICOLON) ) {
					alt50=2;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 50, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 50, 0, input);
				throw nvae;
			}

			switch (alt50) {
				case 1 :
					// antlr/Plexil.g:639:7: ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start]
					{
					pushFollow(FOLLOW_arrayVariableDecl_in_variableDeclaration3401);
					arrayVariableDecl188=arrayVariableDecl((tn!=null?(tn.start):null));
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_arrayVariableDecl.add(arrayVariableDecl188.getTree());
					}
					break;
				case 2 :
					// antlr/Plexil.g:640:7: scalarVariableDecl[$tn.start]
					{
					pushFollow(FOLLOW_scalarVariableDecl_in_variableDeclaration3411);
					scalarVariableDecl189=scalarVariableDecl((tn!=null?(tn.start):null));
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_scalarVariableDecl.add(scalarVariableDecl189.getTree());
					}
					break;

			}

			// antlr/Plexil.g:642:5: ( COMMA ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] ) )*
			loop52:
			while (true) {
				int alt52=2;
				int LA52_0 = input.LA(1);
				if ( (LA52_0==COMMA) ) {
					alt52=1;
				}

				switch (alt52) {
				case 1 :
					// antlr/Plexil.g:642:7: COMMA ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] )
					{
					COMMA190=(Token)match(input,COMMA,FOLLOW_COMMA_in_variableDeclaration3426); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_COMMA.add(COMMA190);

					// antlr/Plexil.g:643:4: ( ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start] | scalarVariableDecl[$tn.start] )
					int alt51=2;
					int LA51_0 = input.LA(1);
					if ( (LA51_0==NCNAME) ) {
						int LA51_1 = input.LA(2);
						if ( (LA51_1==LBRACKET) && (synpred10_Plexil())) {
							alt51=1;
						}
						else if ( (LA51_1==COMMA||LA51_1==EQUALS||LA51_1==SEMICOLON) ) {
							alt51=2;
						}

						else {
							if (state.backtracking>0) {state.failed=true; return retval;}
							int nvaeMark = input.mark();
							try {
								input.consume();
								NoViableAltException nvae =
									new NoViableAltException("", 51, 1, input);
								throw nvae;
							} finally {
								input.rewind(nvaeMark);
							}
						}

					}

					else {
						if (state.backtracking>0) {state.failed=true; return retval;}
						NoViableAltException nvae =
							new NoViableAltException("", 51, 0, input);
						throw nvae;
					}

					switch (alt51) {
						case 1 :
							// antlr/Plexil.g:643:6: ( NCNAME LBRACKET )=> arrayVariableDecl[$tn.start]
							{
							pushFollow(FOLLOW_arrayVariableDecl_in_variableDeclaration3442);
							arrayVariableDecl191=arrayVariableDecl((tn!=null?(tn.start):null));
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) stream_arrayVariableDecl.add(arrayVariableDecl191.getTree());
							}
							break;
						case 2 :
							// antlr/Plexil.g:644:6: scalarVariableDecl[$tn.start]
							{
							pushFollow(FOLLOW_scalarVariableDecl_in_variableDeclaration3451);
							scalarVariableDecl192=scalarVariableDecl((tn!=null?(tn.start):null));
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) stream_scalarVariableDecl.add(scalarVariableDecl192.getTree());
							}
							break;

					}

					}
					break;

				default :
					break loop52;
				}
			}

			SEMICOLON193=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_variableDeclaration3467); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON193);

			// AST REWRITE
			// elements: arrayVariableDecl, scalarVariableDecl
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 648:5: -> ^( VARIABLE_DECLARATIONS ( scalarVariableDecl )* ( arrayVariableDecl )* )
			{
				// antlr/Plexil.g:648:8: ^( VARIABLE_DECLARATIONS ( scalarVariableDecl )* ( arrayVariableDecl )* )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(VARIABLE_DECLARATIONS, "VARIABLE_DECLARATIONS"), root_1);
				// antlr/Plexil.g:648:32: ( scalarVariableDecl )*
				while ( stream_scalarVariableDecl.hasNext() ) {
					adaptor.addChild(root_1, stream_scalarVariableDecl.nextTree());
				}
				stream_scalarVariableDecl.reset();

				// antlr/Plexil.g:648:52: ( arrayVariableDecl )*
				while ( stream_arrayVariableDecl.hasNext() ) {
					adaptor.addChild(root_1, stream_arrayVariableDecl.nextTree());
				}
				stream_arrayVariableDecl.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "variableDeclaration"


	public static class scalarVariableDecl_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "scalarVariableDecl"
	// antlr/Plexil.g:651:1: scalarVariableDecl[Token typeName] : NCNAME ( EQUALS literalScalarValue )? -> ^( VARIABLE_DECLARATION NCNAME ( literalScalarValue )? ) ;
	public final PlexilParser.scalarVariableDecl_return scalarVariableDecl(Token typeName) throws RecognitionException {
		PlexilParser.scalarVariableDecl_return retval = new PlexilParser.scalarVariableDecl_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME194=null;
		Token EQUALS195=null;
		ParserRuleReturnScope literalScalarValue196 =null;

		PlexilTreeNode NCNAME194_tree=null;
		PlexilTreeNode EQUALS195_tree=null;
		RewriteRuleTokenStream stream_EQUALS=new RewriteRuleTokenStream(adaptor,"token EQUALS");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_literalScalarValue=new RewriteRuleSubtreeStream(adaptor,"rule literalScalarValue");

		try {
			// antlr/Plexil.g:651:36: ( NCNAME ( EQUALS literalScalarValue )? -> ^( VARIABLE_DECLARATION NCNAME ( literalScalarValue )? ) )
			// antlr/Plexil.g:652:5: NCNAME ( EQUALS literalScalarValue )?
			{
			NCNAME194=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_scalarVariableDecl3499); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME194);

			// antlr/Plexil.g:652:12: ( EQUALS literalScalarValue )?
			int alt53=2;
			int LA53_0 = input.LA(1);
			if ( (LA53_0==EQUALS) ) {
				alt53=1;
			}
			switch (alt53) {
				case 1 :
					// antlr/Plexil.g:652:14: EQUALS literalScalarValue
					{
					EQUALS195=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_scalarVariableDecl3503); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_EQUALS.add(EQUALS195);

					pushFollow(FOLLOW_literalScalarValue_in_scalarVariableDecl3505);
					literalScalarValue196=literalScalarValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_literalScalarValue.add(literalScalarValue196.getTree());
					}
					break;

			}

			// AST REWRITE
			// elements: NCNAME, literalScalarValue
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 653:5: -> ^( VARIABLE_DECLARATION NCNAME ( literalScalarValue )? )
			{
				// antlr/Plexil.g:653:8: ^( VARIABLE_DECLARATION NCNAME ( literalScalarValue )? )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(VARIABLE_DECLARATION, "VARIABLE_DECLARATION"), root_1);
				adaptor.addChild(root_1, new PlexilTreeNode(typeName));
				adaptor.addChild(root_1, stream_NCNAME.nextNode());
				// antlr/Plexil.g:653:70: ( literalScalarValue )?
				if ( stream_literalScalarValue.hasNext() ) {
					adaptor.addChild(root_1, stream_literalScalarValue.nextTree());
				}
				stream_literalScalarValue.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "scalarVariableDecl"


	public static class arrayVariableDecl_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "arrayVariableDecl"
	// antlr/Plexil.g:656:1: arrayVariableDecl[Token typeName] : NCNAME LBRACKET INT RBRACKET ( EQUALS literalValue )? -> ^( ARRAY_VARIABLE_DECLARATION NCNAME INT ( literalValue )? ) ;
	public final PlexilParser.arrayVariableDecl_return arrayVariableDecl(Token typeName) throws RecognitionException {
		PlexilParser.arrayVariableDecl_return retval = new PlexilParser.arrayVariableDecl_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME197=null;
		Token LBRACKET198=null;
		Token INT199=null;
		Token RBRACKET200=null;
		Token EQUALS201=null;
		ParserRuleReturnScope literalValue202 =null;

		PlexilTreeNode NCNAME197_tree=null;
		PlexilTreeNode LBRACKET198_tree=null;
		PlexilTreeNode INT199_tree=null;
		PlexilTreeNode RBRACKET200_tree=null;
		PlexilTreeNode EQUALS201_tree=null;
		RewriteRuleTokenStream stream_EQUALS=new RewriteRuleTokenStream(adaptor,"token EQUALS");
		RewriteRuleTokenStream stream_LBRACKET=new RewriteRuleTokenStream(adaptor,"token LBRACKET");
		RewriteRuleTokenStream stream_RBRACKET=new RewriteRuleTokenStream(adaptor,"token RBRACKET");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleTokenStream stream_INT=new RewriteRuleTokenStream(adaptor,"token INT");
		RewriteRuleSubtreeStream stream_literalValue=new RewriteRuleSubtreeStream(adaptor,"rule literalValue");

		try {
			// antlr/Plexil.g:656:35: ( NCNAME LBRACKET INT RBRACKET ( EQUALS literalValue )? -> ^( ARRAY_VARIABLE_DECLARATION NCNAME INT ( literalValue )? ) )
			// antlr/Plexil.g:657:5: NCNAME LBRACKET INT RBRACKET ( EQUALS literalValue )?
			{
			NCNAME197=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_arrayVariableDecl3541); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME197);

			LBRACKET198=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_arrayVariableDecl3543); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LBRACKET.add(LBRACKET198);

			INT199=(Token)match(input,INT,FOLLOW_INT_in_arrayVariableDecl3545); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_INT.add(INT199);

			RBRACKET200=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_arrayVariableDecl3547); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RBRACKET.add(RBRACKET200);

			// antlr/Plexil.g:657:34: ( EQUALS literalValue )?
			int alt54=2;
			int LA54_0 = input.LA(1);
			if ( (LA54_0==EQUALS) ) {
				alt54=1;
			}
			switch (alt54) {
				case 1 :
					// antlr/Plexil.g:657:36: EQUALS literalValue
					{
					EQUALS201=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_arrayVariableDecl3551); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_EQUALS.add(EQUALS201);

					pushFollow(FOLLOW_literalValue_in_arrayVariableDecl3553);
					literalValue202=literalValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_literalValue.add(literalValue202.getTree());
					}
					break;

			}

			// AST REWRITE
			// elements: literalValue, NCNAME, INT
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 658:2: -> ^( ARRAY_VARIABLE_DECLARATION NCNAME INT ( literalValue )? )
			{
				// antlr/Plexil.g:658:5: ^( ARRAY_VARIABLE_DECLARATION NCNAME INT ( literalValue )? )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_VARIABLE_DECLARATION, "ARRAY_VARIABLE_DECLARATION"), root_1);
				adaptor.addChild(root_1, new PlexilTreeNode(typeName));
				adaptor.addChild(root_1, stream_NCNAME.nextNode());
				adaptor.addChild(root_1, stream_INT.nextNode());
				// antlr/Plexil.g:658:77: ( literalValue )?
				if ( stream_literalValue.hasNext() ) {
					adaptor.addChild(root_1, stream_literalValue.nextTree());
				}
				stream_literalValue.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "arrayVariableDecl"


	public static class literalScalarValue_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "literalScalarValue"
	// antlr/Plexil.g:661:1: literalScalarValue : ( booleanLiteral | INT | DOUBLE | STRING | unaryMinus | dateLiteral | durationLiteral );
	public final PlexilParser.literalScalarValue_return literalScalarValue() throws RecognitionException {
		PlexilParser.literalScalarValue_return retval = new PlexilParser.literalScalarValue_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token INT204=null;
		Token DOUBLE205=null;
		Token STRING206=null;
		ParserRuleReturnScope booleanLiteral203 =null;
		ParserRuleReturnScope unaryMinus207 =null;
		ParserRuleReturnScope dateLiteral208 =null;
		ParserRuleReturnScope durationLiteral209 =null;

		PlexilTreeNode INT204_tree=null;
		PlexilTreeNode DOUBLE205_tree=null;
		PlexilTreeNode STRING206_tree=null;

		try {
			// antlr/Plexil.g:661:20: ( booleanLiteral | INT | DOUBLE | STRING | unaryMinus | dateLiteral | durationLiteral )
			int alt55=7;
			switch ( input.LA(1) ) {
			case FALSE_KYWD:
			case TRUE_KYWD:
				{
				alt55=1;
				}
				break;
			case INT:
				{
				alt55=2;
				}
				break;
			case DOUBLE:
				{
				alt55=3;
				}
				break;
			case STRING:
				{
				alt55=4;
				}
				break;
			case MINUS:
				{
				alt55=5;
				}
				break;
			case DATE_KYWD:
				{
				alt55=6;
				}
				break;
			case DURATION_KYWD:
				{
				alt55=7;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 55, 0, input);
				throw nvae;
			}
			switch (alt55) {
				case 1 :
					// antlr/Plexil.g:662:5: booleanLiteral
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_booleanLiteral_in_literalScalarValue3589);
					booleanLiteral203=booleanLiteral();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, booleanLiteral203.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:662:22: INT
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					INT204=(Token)match(input,INT,FOLLOW_INT_in_literalScalarValue3593); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					INT204_tree = (PlexilTreeNode)adaptor.create(INT204);
					adaptor.addChild(root_0, INT204_tree);
					}

					}
					break;
				case 3 :
					// antlr/Plexil.g:662:28: DOUBLE
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					DOUBLE205=(Token)match(input,DOUBLE,FOLLOW_DOUBLE_in_literalScalarValue3597); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					DOUBLE205_tree = (PlexilTreeNode)adaptor.create(DOUBLE205);
					adaptor.addChild(root_0, DOUBLE205_tree);
					}

					}
					break;
				case 4 :
					// antlr/Plexil.g:662:37: STRING
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					STRING206=(Token)match(input,STRING,FOLLOW_STRING_in_literalScalarValue3601); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					STRING206_tree = (PlexilTreeNode)adaptor.create(STRING206);
					adaptor.addChild(root_0, STRING206_tree);
					}

					}
					break;
				case 5 :
					// antlr/Plexil.g:662:46: unaryMinus
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_unaryMinus_in_literalScalarValue3605);
					unaryMinus207=unaryMinus();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, unaryMinus207.getTree());

					}
					break;
				case 6 :
					// antlr/Plexil.g:663:5: dateLiteral
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_dateLiteral_in_literalScalarValue3613);
					dateLiteral208=dateLiteral();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, dateLiteral208.getTree());

					}
					break;
				case 7 :
					// antlr/Plexil.g:663:19: durationLiteral
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_durationLiteral_in_literalScalarValue3617);
					durationLiteral209=durationLiteral();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, durationLiteral209.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "literalScalarValue"


	public static class literalArrayValue_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "literalArrayValue"
	// antlr/Plexil.g:665:1: literalArrayValue : HASHPAREN ( literalScalarValue )* RPAREN -> ^( ARRAY_LITERAL ( literalScalarValue )* ) ;
	public final PlexilParser.literalArrayValue_return literalArrayValue() throws RecognitionException {
		PlexilParser.literalArrayValue_return retval = new PlexilParser.literalArrayValue_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token HASHPAREN210=null;
		Token RPAREN212=null;
		ParserRuleReturnScope literalScalarValue211 =null;

		PlexilTreeNode HASHPAREN210_tree=null;
		PlexilTreeNode RPAREN212_tree=null;
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleTokenStream stream_HASHPAREN=new RewriteRuleTokenStream(adaptor,"token HASHPAREN");
		RewriteRuleSubtreeStream stream_literalScalarValue=new RewriteRuleSubtreeStream(adaptor,"rule literalScalarValue");

		try {
			// antlr/Plexil.g:665:19: ( HASHPAREN ( literalScalarValue )* RPAREN -> ^( ARRAY_LITERAL ( literalScalarValue )* ) )
			// antlr/Plexil.g:666:5: HASHPAREN ( literalScalarValue )* RPAREN
			{
			HASHPAREN210=(Token)match(input,HASHPAREN,FOLLOW_HASHPAREN_in_literalArrayValue3630); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_HASHPAREN.add(HASHPAREN210);

			// antlr/Plexil.g:666:15: ( literalScalarValue )*
			loop56:
			while (true) {
				int alt56=2;
				int LA56_0 = input.LA(1);
				if ( (LA56_0==DATE_KYWD||(LA56_0 >= DOUBLE && LA56_0 <= DURATION_KYWD)||LA56_0==FALSE_KYWD||LA56_0==INT||LA56_0==MINUS||LA56_0==STRING||LA56_0==TRUE_KYWD) ) {
					alt56=1;
				}

				switch (alt56) {
				case 1 :
					// antlr/Plexil.g:666:15: literalScalarValue
					{
					pushFollow(FOLLOW_literalScalarValue_in_literalArrayValue3632);
					literalScalarValue211=literalScalarValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_literalScalarValue.add(literalScalarValue211.getTree());
					}
					break;

				default :
					break loop56;
				}
			}

			RPAREN212=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_literalArrayValue3635); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN212);

			// AST REWRITE
			// elements: literalScalarValue
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 667:5: -> ^( ARRAY_LITERAL ( literalScalarValue )* )
			{
				// antlr/Plexil.g:667:8: ^( ARRAY_LITERAL ( literalScalarValue )* )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_LITERAL, "ARRAY_LITERAL"), root_1);
				// antlr/Plexil.g:667:24: ( literalScalarValue )*
				while ( stream_literalScalarValue.hasNext() ) {
					adaptor.addChild(root_1, stream_literalScalarValue.nextTree());
				}
				stream_literalScalarValue.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "literalArrayValue"


	public static class literalValue_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "literalValue"
	// antlr/Plexil.g:670:1: literalValue : ( literalScalarValue | literalArrayValue );
	public final PlexilParser.literalValue_return literalValue() throws RecognitionException {
		PlexilParser.literalValue_return retval = new PlexilParser.literalValue_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope literalScalarValue213 =null;
		ParserRuleReturnScope literalArrayValue214 =null;


		try {
			// antlr/Plexil.g:670:14: ( literalScalarValue | literalArrayValue )
			int alt57=2;
			int LA57_0 = input.LA(1);
			if ( (LA57_0==DATE_KYWD||(LA57_0 >= DOUBLE && LA57_0 <= DURATION_KYWD)||LA57_0==FALSE_KYWD||LA57_0==INT||LA57_0==MINUS||LA57_0==STRING||LA57_0==TRUE_KYWD) ) {
				alt57=1;
			}
			else if ( (LA57_0==HASHPAREN) ) {
				alt57=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 57, 0, input);
				throw nvae;
			}

			switch (alt57) {
				case 1 :
					// antlr/Plexil.g:670:16: literalScalarValue
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_literalScalarValue_in_literalValue3659);
					literalScalarValue213=literalScalarValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, literalScalarValue213.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:670:37: literalArrayValue
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_literalArrayValue_in_literalValue3663);
					literalArrayValue214=literalArrayValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, literalArrayValue214.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "literalValue"


	public static class booleanLiteral_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "booleanLiteral"
	// antlr/Plexil.g:672:1: booleanLiteral : ( TRUE_KYWD | FALSE_KYWD );
	public final PlexilParser.booleanLiteral_return booleanLiteral() throws RecognitionException {
		PlexilParser.booleanLiteral_return retval = new PlexilParser.booleanLiteral_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set215=null;

		PlexilTreeNode set215_tree=null;

		try {
			// antlr/Plexil.g:672:16: ( TRUE_KYWD | FALSE_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set215=input.LT(1);
			if ( input.LA(1)==FALSE_KYWD||input.LA(1)==TRUE_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set215));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "booleanLiteral"


	public static class realValue_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "realValue"
	// antlr/Plexil.g:674:1: realValue : ( DOUBLE | INT );
	public final PlexilParser.realValue_return realValue() throws RecognitionException {
		PlexilParser.realValue_return retval = new PlexilParser.realValue_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set216=null;

		PlexilTreeNode set216_tree=null;

		try {
			// antlr/Plexil.g:674:11: ( DOUBLE | INT )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set216=input.LT(1);
			if ( input.LA(1)==DOUBLE||input.LA(1)==INT ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set216));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "realValue"


	public static class lookupArrayReference_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookupArrayReference"
	// antlr/Plexil.g:678:1: lookupArrayReference : lookup LBRACKET expression RBRACKET -> ^( ARRAY_REF lookup expression ) ;
	public final PlexilParser.lookupArrayReference_return lookupArrayReference() throws RecognitionException {
		PlexilParser.lookupArrayReference_return retval = new PlexilParser.lookupArrayReference_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LBRACKET218=null;
		Token RBRACKET220=null;
		ParserRuleReturnScope lookup217 =null;
		ParserRuleReturnScope expression219 =null;

		PlexilTreeNode LBRACKET218_tree=null;
		PlexilTreeNode RBRACKET220_tree=null;
		RewriteRuleTokenStream stream_LBRACKET=new RewriteRuleTokenStream(adaptor,"token LBRACKET");
		RewriteRuleTokenStream stream_RBRACKET=new RewriteRuleTokenStream(adaptor,"token RBRACKET");
		RewriteRuleSubtreeStream stream_lookup=new RewriteRuleSubtreeStream(adaptor,"rule lookup");
		RewriteRuleSubtreeStream stream_expression=new RewriteRuleSubtreeStream(adaptor,"rule expression");

		try {
			// antlr/Plexil.g:678:22: ( lookup LBRACKET expression RBRACKET -> ^( ARRAY_REF lookup expression ) )
			// antlr/Plexil.g:679:5: lookup LBRACKET expression RBRACKET
			{
			pushFollow(FOLLOW_lookup_in_lookupArrayReference3704);
			lookup217=lookup();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_lookup.add(lookup217.getTree());
			LBRACKET218=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_lookupArrayReference3706); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LBRACKET.add(LBRACKET218);

			pushFollow(FOLLOW_expression_in_lookupArrayReference3708);
			expression219=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_expression.add(expression219.getTree());
			RBRACKET220=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_lookupArrayReference3710); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RBRACKET.add(RBRACKET220);

			// AST REWRITE
			// elements: lookup, expression
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 680:5: -> ^( ARRAY_REF lookup expression )
			{
				// antlr/Plexil.g:680:8: ^( ARRAY_REF lookup expression )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_REF, "ARRAY_REF"), root_1);
				adaptor.addChild(root_1, stream_lookup.nextTree());
				adaptor.addChild(root_1, stream_expression.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookupArrayReference"


	public static class simpleArrayReference_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "simpleArrayReference"
	// antlr/Plexil.g:683:1: simpleArrayReference : variable LBRACKET expression RBRACKET -> ^( ARRAY_REF variable expression ) ;
	public final PlexilParser.simpleArrayReference_return simpleArrayReference() throws RecognitionException {
		PlexilParser.simpleArrayReference_return retval = new PlexilParser.simpleArrayReference_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LBRACKET222=null;
		Token RBRACKET224=null;
		ParserRuleReturnScope variable221 =null;
		ParserRuleReturnScope expression223 =null;

		PlexilTreeNode LBRACKET222_tree=null;
		PlexilTreeNode RBRACKET224_tree=null;
		RewriteRuleTokenStream stream_LBRACKET=new RewriteRuleTokenStream(adaptor,"token LBRACKET");
		RewriteRuleTokenStream stream_RBRACKET=new RewriteRuleTokenStream(adaptor,"token RBRACKET");
		RewriteRuleSubtreeStream stream_expression=new RewriteRuleSubtreeStream(adaptor,"rule expression");
		RewriteRuleSubtreeStream stream_variable=new RewriteRuleSubtreeStream(adaptor,"rule variable");

		try {
			// antlr/Plexil.g:683:22: ( variable LBRACKET expression RBRACKET -> ^( ARRAY_REF variable expression ) )
			// antlr/Plexil.g:684:5: variable LBRACKET expression RBRACKET
			{
			pushFollow(FOLLOW_variable_in_simpleArrayReference3739);
			variable221=variable();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_variable.add(variable221.getTree());
			LBRACKET222=(Token)match(input,LBRACKET,FOLLOW_LBRACKET_in_simpleArrayReference3741); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LBRACKET.add(LBRACKET222);

			pushFollow(FOLLOW_expression_in_simpleArrayReference3743);
			expression223=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_expression.add(expression223.getTree());
			RBRACKET224=(Token)match(input,RBRACKET,FOLLOW_RBRACKET_in_simpleArrayReference3745); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RBRACKET.add(RBRACKET224);

			// AST REWRITE
			// elements: variable, expression
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 685:5: -> ^( ARRAY_REF variable expression )
			{
				// antlr/Plexil.g:685:8: ^( ARRAY_REF variable expression )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARRAY_REF, "ARRAY_REF"), root_1);
				adaptor.addChild(root_1, stream_variable.nextTree());
				adaptor.addChild(root_1, stream_expression.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "simpleArrayReference"


	public static class commandInvocation_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "commandInvocation"
	// antlr/Plexil.g:688:1: commandInvocation : ( NCNAME -> ^( COMMAND_KYWD NCNAME ) | LPAREN expression RPAREN -> expression ) LPAREN ( argumentList )? RPAREN -> ^( COMMAND $commandInvocation ( argumentList )? ) ;
	public final PlexilParser.commandInvocation_return commandInvocation() throws RecognitionException {
		PlexilParser.commandInvocation_return retval = new PlexilParser.commandInvocation_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME225=null;
		Token LPAREN226=null;
		Token RPAREN228=null;
		Token LPAREN229=null;
		Token RPAREN231=null;
		ParserRuleReturnScope expression227 =null;
		ParserRuleReturnScope argumentList230 =null;

		PlexilTreeNode NCNAME225_tree=null;
		PlexilTreeNode LPAREN226_tree=null;
		PlexilTreeNode RPAREN228_tree=null;
		PlexilTreeNode LPAREN229_tree=null;
		PlexilTreeNode RPAREN231_tree=null;
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_argumentList=new RewriteRuleSubtreeStream(adaptor,"rule argumentList");
		RewriteRuleSubtreeStream stream_expression=new RewriteRuleSubtreeStream(adaptor,"rule expression");

		 m_paraphrases.push("in command"); 
		try {
			// antlr/Plexil.g:691:2: ( ( NCNAME -> ^( COMMAND_KYWD NCNAME ) | LPAREN expression RPAREN -> expression ) LPAREN ( argumentList )? RPAREN -> ^( COMMAND $commandInvocation ( argumentList )? ) )
			// antlr/Plexil.g:692:5: ( NCNAME -> ^( COMMAND_KYWD NCNAME ) | LPAREN expression RPAREN -> expression ) LPAREN ( argumentList )? RPAREN
			{
			// antlr/Plexil.g:692:5: ( NCNAME -> ^( COMMAND_KYWD NCNAME ) | LPAREN expression RPAREN -> expression )
			int alt58=2;
			int LA58_0 = input.LA(1);
			if ( (LA58_0==NCNAME) ) {
				alt58=1;
			}
			else if ( (LA58_0==LPAREN) ) {
				alt58=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 58, 0, input);
				throw nvae;
			}

			switch (alt58) {
				case 1 :
					// antlr/Plexil.g:692:7: NCNAME
					{
					NCNAME225=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_commandInvocation3787); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME225);

					// AST REWRITE
					// elements: NCNAME
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 692:14: -> ^( COMMAND_KYWD NCNAME )
					{
						// antlr/Plexil.g:692:17: ^( COMMAND_KYWD NCNAME )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(COMMAND_KYWD, "COMMAND_KYWD"), root_1);
						adaptor.addChild(root_1, stream_NCNAME.nextNode());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:693:7: LPAREN expression RPAREN
					{
					LPAREN226=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_commandInvocation3803); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN226);

					pushFollow(FOLLOW_expression_in_commandInvocation3805);
					expression227=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_expression.add(expression227.getTree());
					RPAREN228=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_commandInvocation3807); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN228);

					// AST REWRITE
					// elements: expression
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 693:32: -> expression
					{
						adaptor.addChild(root_0, stream_expression.nextTree());
					}


					retval.tree = root_0;
					}

					}
					break;

			}

			LPAREN229=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_commandInvocation3823); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN229);

			// antlr/Plexil.g:695:12: ( argumentList )?
			int alt59=2;
			int LA59_0 = input.LA(1);
			if ( (LA59_0==ABS_KYWD||LA59_0==ARRAY_MAX_SIZE_KYWD||LA59_0==ARRAY_SIZE_KYWD||LA59_0==BAR||(LA59_0 >= CEIL_KYWD && LA59_0 <= CHILD_KYWD)||(LA59_0 >= COMMAND_ACCEPTED_KYWD && LA59_0 <= COMMAND_FAILED_KYWD)||(LA59_0 >= COMMAND_RCVD_KYWD && LA59_0 <= COMMAND_SUCCESS_KYWD)||LA59_0==DATE_KYWD||(LA59_0 >= DOUBLE && LA59_0 <= DURATION_KYWD)||(LA59_0 >= EXECUTING_STATE_KYWD && LA59_0 <= EXITED_KYWD)||LA59_0==FAILING_STATE_KYWD||(LA59_0 >= FAILURE_OUTCOME_KYWD && LA59_0 <= FLOOR_KYWD)||LA59_0==HASHPAREN||(LA59_0 >= INACTIVE_STATE_KYWD && LA59_0 <= INT)||LA59_0==INTERRUPTED_OUTCOME_KYWD||LA59_0==INVARIANT_CONDITION_FAILED_KYWD||(LA59_0 >= IS_KNOWN_KYWD && LA59_0 <= ITERATION_ENDED_STATE_KYWD)||(LA59_0 >= LOOKUP_KYWD && LA59_0 <= LOOKUP_ON_CHANGE_KYWD)||LA59_0==LPAREN||(LA59_0 >= MAX_KYWD && LA59_0 <= MIN_KYWD)||LA59_0==NCNAME||(LA59_0 >= NODE_EXECUTING_KYWD && LA59_0 <= NODE_FINISHED_KYWD)||(LA59_0 >= NODE_INACTIVE_KYWD && LA59_0 <= NODE_SUCCEEDED_KYWD)||(LA59_0 >= NODE_WAITING_KYWD && LA59_0 <= NOT_KYWD)||(LA59_0 >= PARENT_EXITED_KYWD && LA59_0 <= PARENT_KYWD)||LA59_0==POST_CONDITION_FAILED_KYWD||LA59_0==PRE_CONDITION_FAILED_KYWD||LA59_0==REAL_TO_INT_KYWD||LA59_0==ROUND_KYWD||LA59_0==SELF_KYWD||(LA59_0 >= SIBLING_KYWD && LA59_0 <= SKIPPED_OUTCOME_KYWD)||LA59_0==SQRT_KYWD||LA59_0==STRING||(LA59_0 >= STRLEN_KYWD && LA59_0 <= SUCCESS_OUTCOME_KYWD)||(LA59_0 >= TRUE_KYWD && LA59_0 <= TRUNC_KYWD)||LA59_0==WAITING_STATE_KYWD) ) {
				alt59=1;
			}
			switch (alt59) {
				case 1 :
					// antlr/Plexil.g:695:12: argumentList
					{
					pushFollow(FOLLOW_argumentList_in_commandInvocation3825);
					argumentList230=argumentList();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_argumentList.add(argumentList230.getTree());
					}
					break;

			}

			RPAREN231=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_commandInvocation3828); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN231);

			// AST REWRITE
			// elements: argumentList, commandInvocation
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 696:5: -> ^( COMMAND $commandInvocation ( argumentList )? )
			{
				// antlr/Plexil.g:696:8: ^( COMMAND $commandInvocation ( argumentList )? )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot(new CommandNode(COMMAND), root_1);
				adaptor.addChild(root_1, stream_retval.nextTree());
				// antlr/Plexil.g:696:50: ( argumentList )?
				if ( stream_argumentList.hasNext() ) {
					adaptor.addChild(root_1, stream_argumentList.nextTree());
				}
				stream_argumentList.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "commandInvocation"


	public static class commandWithAssignment_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "commandWithAssignment"
	// antlr/Plexil.g:700:1: commandWithAssignment : assignmentLHS EQUALS commandInvocation -> ^( ASSIGNMENT assignmentLHS commandInvocation ) ;
	public final PlexilParser.commandWithAssignment_return commandWithAssignment() throws RecognitionException {
		PlexilParser.commandWithAssignment_return retval = new PlexilParser.commandWithAssignment_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token EQUALS233=null;
		ParserRuleReturnScope assignmentLHS232 =null;
		ParserRuleReturnScope commandInvocation234 =null;

		PlexilTreeNode EQUALS233_tree=null;
		RewriteRuleTokenStream stream_EQUALS=new RewriteRuleTokenStream(adaptor,"token EQUALS");
		RewriteRuleSubtreeStream stream_commandInvocation=new RewriteRuleSubtreeStream(adaptor,"rule commandInvocation");
		RewriteRuleSubtreeStream stream_assignmentLHS=new RewriteRuleSubtreeStream(adaptor,"rule assignmentLHS");

		 m_paraphrases.push("in command"); 
		try {
			// antlr/Plexil.g:703:2: ( assignmentLHS EQUALS commandInvocation -> ^( ASSIGNMENT assignmentLHS commandInvocation ) )
			// antlr/Plexil.g:704:5: assignmentLHS EQUALS commandInvocation
			{
			pushFollow(FOLLOW_assignmentLHS_in_commandWithAssignment3873);
			assignmentLHS232=assignmentLHS();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_assignmentLHS.add(assignmentLHS232.getTree());
			EQUALS233=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_commandWithAssignment3875); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_EQUALS.add(EQUALS233);

			pushFollow(FOLLOW_commandInvocation_in_commandWithAssignment3877);
			commandInvocation234=commandInvocation();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_commandInvocation.add(commandInvocation234.getTree());
			// AST REWRITE
			// elements: commandInvocation, assignmentLHS
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 705:2: -> ^( ASSIGNMENT assignmentLHS commandInvocation )
			{
				// antlr/Plexil.g:705:5: ^( ASSIGNMENT assignmentLHS commandInvocation )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ASSIGNMENT, "ASSIGNMENT"), root_1);
				adaptor.addChild(root_1, stream_assignmentLHS.nextTree());
				adaptor.addChild(root_1, stream_commandInvocation.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "commandWithAssignment"


	public static class argumentList_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "argumentList"
	// antlr/Plexil.g:708:1: argumentList : argument ( COMMA argument )* -> ^( ARGUMENT_LIST ( argument )* ) ;
	public final PlexilParser.argumentList_return argumentList() throws RecognitionException {
		PlexilParser.argumentList_return retval = new PlexilParser.argumentList_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token COMMA236=null;
		ParserRuleReturnScope argument235 =null;
		ParserRuleReturnScope argument237 =null;

		PlexilTreeNode COMMA236_tree=null;
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleSubtreeStream stream_argument=new RewriteRuleSubtreeStream(adaptor,"rule argument");

		try {
			// antlr/Plexil.g:708:14: ( argument ( COMMA argument )* -> ^( ARGUMENT_LIST ( argument )* ) )
			// antlr/Plexil.g:709:5: argument ( COMMA argument )*
			{
			pushFollow(FOLLOW_argument_in_argumentList3907);
			argument235=argument();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_argument.add(argument235.getTree());
			// antlr/Plexil.g:709:14: ( COMMA argument )*
			loop60:
			while (true) {
				int alt60=2;
				int LA60_0 = input.LA(1);
				if ( (LA60_0==COMMA) ) {
					alt60=1;
				}

				switch (alt60) {
				case 1 :
					// antlr/Plexil.g:709:15: COMMA argument
					{
					COMMA236=(Token)match(input,COMMA,FOLLOW_COMMA_in_argumentList3910); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_COMMA.add(COMMA236);

					pushFollow(FOLLOW_argument_in_argumentList3912);
					argument237=argument();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_argument.add(argument237.getTree());
					}
					break;

				default :
					break loop60;
				}
			}

			// AST REWRITE
			// elements: argument
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 710:5: -> ^( ARGUMENT_LIST ( argument )* )
			{
				// antlr/Plexil.g:710:8: ^( ARGUMENT_LIST ( argument )* )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ARGUMENT_LIST, "ARGUMENT_LIST"), root_1);
				// antlr/Plexil.g:710:24: ( argument )*
				while ( stream_argument.hasNext() ) {
					adaptor.addChild(root_1, stream_argument.nextTree());
				}
				stream_argument.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "argumentList"


	public static class argument_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "argument"
	// antlr/Plexil.g:713:1: argument : expression ;
	public final PlexilParser.argument_return argument() throws RecognitionException {
		PlexilParser.argument_return retval = new PlexilParser.argument_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope expression238 =null;


		try {
			// antlr/Plexil.g:713:10: ( expression )
			// antlr/Plexil.g:713:12: expression
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_expression_in_argument3938);
			expression238=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression238.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "argument"


	public static class assignment_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "assignment"
	// antlr/Plexil.g:715:1: assignment : assignmentLHS EQUALS assignmentRHS SEMICOLON -> ^( ASSIGNMENT assignmentLHS assignmentRHS ) ;
	public final PlexilParser.assignment_return assignment() throws RecognitionException {
		PlexilParser.assignment_return retval = new PlexilParser.assignment_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token EQUALS240=null;
		Token SEMICOLON242=null;
		ParserRuleReturnScope assignmentLHS239 =null;
		ParserRuleReturnScope assignmentRHS241 =null;

		PlexilTreeNode EQUALS240_tree=null;
		PlexilTreeNode SEMICOLON242_tree=null;
		RewriteRuleTokenStream stream_EQUALS=new RewriteRuleTokenStream(adaptor,"token EQUALS");
		RewriteRuleTokenStream stream_SEMICOLON=new RewriteRuleTokenStream(adaptor,"token SEMICOLON");
		RewriteRuleSubtreeStream stream_assignmentLHS=new RewriteRuleSubtreeStream(adaptor,"rule assignmentLHS");
		RewriteRuleSubtreeStream stream_assignmentRHS=new RewriteRuleSubtreeStream(adaptor,"rule assignmentRHS");

		 m_paraphrases.push("in assignment statement"); 
		try {
			// antlr/Plexil.g:718:2: ( assignmentLHS EQUALS assignmentRHS SEMICOLON -> ^( ASSIGNMENT assignmentLHS assignmentRHS ) )
			// antlr/Plexil.g:719:5: assignmentLHS EQUALS assignmentRHS SEMICOLON
			{
			pushFollow(FOLLOW_assignmentLHS_in_assignment3962);
			assignmentLHS239=assignmentLHS();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_assignmentLHS.add(assignmentLHS239.getTree());
			EQUALS240=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_assignment3964); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_EQUALS.add(EQUALS240);

			pushFollow(FOLLOW_assignmentRHS_in_assignment3966);
			assignmentRHS241=assignmentRHS();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_assignmentRHS.add(assignmentRHS241.getTree());
			SEMICOLON242=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_assignment3968); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_SEMICOLON.add(SEMICOLON242);

			// AST REWRITE
			// elements: assignmentLHS, assignmentRHS
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 720:5: -> ^( ASSIGNMENT assignmentLHS assignmentRHS )
			{
				// antlr/Plexil.g:720:8: ^( ASSIGNMENT assignmentLHS assignmentRHS )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ASSIGNMENT, "ASSIGNMENT"), root_1);
				adaptor.addChild(root_1, stream_assignmentLHS.nextTree());
				adaptor.addChild(root_1, stream_assignmentRHS.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "assignment"


	public static class assignmentLHS_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "assignmentLHS"
	// antlr/Plexil.g:723:1: assignmentLHS : ( ( NCNAME LBRACKET )=> simpleArrayReference | variable );
	public final PlexilParser.assignmentLHS_return assignmentLHS() throws RecognitionException {
		PlexilParser.assignmentLHS_return retval = new PlexilParser.assignmentLHS_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope simpleArrayReference243 =null;
		ParserRuleReturnScope variable244 =null;


		try {
			// antlr/Plexil.g:723:15: ( ( NCNAME LBRACKET )=> simpleArrayReference | variable )
			int alt61=2;
			int LA61_0 = input.LA(1);
			if ( (LA61_0==NCNAME) ) {
				int LA61_1 = input.LA(2);
				if ( (LA61_1==LBRACKET) && (synpred11_Plexil())) {
					alt61=1;
				}
				else if ( (LA61_1==EQUALS) ) {
					alt61=2;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 61, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 61, 0, input);
				throw nvae;
			}

			switch (alt61) {
				case 1 :
					// antlr/Plexil.g:724:5: ( NCNAME LBRACKET )=> simpleArrayReference
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_simpleArrayReference_in_assignmentLHS4011);
					simpleArrayReference243=simpleArrayReference();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, simpleArrayReference243.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:727:5: variable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_variable_in_assignmentLHS4022);
					variable244=variable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, variable244.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "assignmentLHS"


	public static class assignmentRHS_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "assignmentRHS"
	// antlr/Plexil.g:731:1: assignmentRHS : ( ( NCNAME LPAREN )=> commandInvocation | ( LPAREN expression RPAREN LPAREN )=> commandInvocation | expression );
	public final PlexilParser.assignmentRHS_return assignmentRHS() throws RecognitionException {
		PlexilParser.assignmentRHS_return retval = new PlexilParser.assignmentRHS_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope commandInvocation245 =null;
		ParserRuleReturnScope commandInvocation246 =null;
		ParserRuleReturnScope expression247 =null;


		try {
			// antlr/Plexil.g:731:15: ( ( NCNAME LPAREN )=> commandInvocation | ( LPAREN expression RPAREN LPAREN )=> commandInvocation | expression )
			int alt62=3;
			alt62 = dfa62.predict(input);
			switch (alt62) {
				case 1 :
					// antlr/Plexil.g:732:4: ( NCNAME LPAREN )=> commandInvocation
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_commandInvocation_in_assignmentRHS4043);
					commandInvocation245=commandInvocation();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, commandInvocation245.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:734:4: ( LPAREN expression RPAREN LPAREN )=> commandInvocation
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_commandInvocation_in_assignmentRHS4063);
					commandInvocation246=commandInvocation();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, commandInvocation246.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:736:4: expression
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_expression_in_assignmentRHS4071);
					expression247=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression247.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "assignmentRHS"


	public static class update_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "update"
	// antlr/Plexil.g:743:1: update : UPDATE_KYWD ^ ( pair ( COMMA ! pair )* )? SEMICOLON !;
	public final PlexilParser.update_return update() throws RecognitionException {
		PlexilParser.update_return retval = new PlexilParser.update_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token UPDATE_KYWD248=null;
		Token COMMA250=null;
		Token SEMICOLON252=null;
		ParserRuleReturnScope pair249 =null;
		ParserRuleReturnScope pair251 =null;

		PlexilTreeNode UPDATE_KYWD248_tree=null;
		PlexilTreeNode COMMA250_tree=null;
		PlexilTreeNode SEMICOLON252_tree=null;

		 m_paraphrases.push("in \"Update\" statement"); 
		try {
			// antlr/Plexil.g:746:2: ( UPDATE_KYWD ^ ( pair ( COMMA ! pair )* )? SEMICOLON !)
			// antlr/Plexil.g:747:5: UPDATE_KYWD ^ ( pair ( COMMA ! pair )* )? SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			UPDATE_KYWD248=(Token)match(input,UPDATE_KYWD,FOLLOW_UPDATE_KYWD_in_update4100); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			UPDATE_KYWD248_tree = (PlexilTreeNode)adaptor.create(UPDATE_KYWD248);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(UPDATE_KYWD248_tree, root_0);
			}

			// antlr/Plexil.g:747:18: ( pair ( COMMA ! pair )* )?
			int alt64=2;
			int LA64_0 = input.LA(1);
			if ( (LA64_0==NCNAME) ) {
				alt64=1;
			}
			switch (alt64) {
				case 1 :
					// antlr/Plexil.g:747:20: pair ( COMMA ! pair )*
					{
					pushFollow(FOLLOW_pair_in_update4105);
					pair249=pair();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, pair249.getTree());

					// antlr/Plexil.g:747:25: ( COMMA ! pair )*
					loop63:
					while (true) {
						int alt63=2;
						int LA63_0 = input.LA(1);
						if ( (LA63_0==COMMA) ) {
							alt63=1;
						}

						switch (alt63) {
						case 1 :
							// antlr/Plexil.g:747:27: COMMA ! pair
							{
							COMMA250=(Token)match(input,COMMA,FOLLOW_COMMA_in_update4109); if (state.failed) return retval;
							pushFollow(FOLLOW_pair_in_update4112);
							pair251=pair();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, pair251.getTree());

							}
							break;

						default :
							break loop63;
						}
					}

					}
					break;

			}

			SEMICOLON252=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_update4120); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "update"


	public static class request_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "request"
	// antlr/Plexil.g:755:1: request : REQUEST_KYWD ^ NCNAME ( pair ( COMMA ! pair )* )? SEMICOLON !;
	public final PlexilParser.request_return request() throws RecognitionException {
		PlexilParser.request_return retval = new PlexilParser.request_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token REQUEST_KYWD253=null;
		Token NCNAME254=null;
		Token COMMA256=null;
		Token SEMICOLON258=null;
		ParserRuleReturnScope pair255 =null;
		ParserRuleReturnScope pair257 =null;

		PlexilTreeNode REQUEST_KYWD253_tree=null;
		PlexilTreeNode NCNAME254_tree=null;
		PlexilTreeNode COMMA256_tree=null;
		PlexilTreeNode SEMICOLON258_tree=null;

		 m_paraphrases.push("in \"Request\" statement"); 
		try {
			// antlr/Plexil.g:758:2: ( REQUEST_KYWD ^ NCNAME ( pair ( COMMA ! pair )* )? SEMICOLON !)
			// antlr/Plexil.g:758:4: REQUEST_KYWD ^ NCNAME ( pair ( COMMA ! pair )* )? SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			REQUEST_KYWD253=(Token)match(input,REQUEST_KYWD,FOLLOW_REQUEST_KYWD_in_request4147); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			REQUEST_KYWD253_tree = (PlexilTreeNode)adaptor.create(REQUEST_KYWD253);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(REQUEST_KYWD253_tree, root_0);
			}

			NCNAME254=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_request4150); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NCNAME254_tree = (PlexilTreeNode)adaptor.create(NCNAME254);
			adaptor.addChild(root_0, NCNAME254_tree);
			}

			// antlr/Plexil.g:758:25: ( pair ( COMMA ! pair )* )?
			int alt66=2;
			int LA66_0 = input.LA(1);
			if ( (LA66_0==NCNAME) ) {
				alt66=1;
			}
			switch (alt66) {
				case 1 :
					// antlr/Plexil.g:758:27: pair ( COMMA ! pair )*
					{
					pushFollow(FOLLOW_pair_in_request4154);
					pair255=pair();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, pair255.getTree());

					// antlr/Plexil.g:758:32: ( COMMA ! pair )*
					loop65:
					while (true) {
						int alt65=2;
						int LA65_0 = input.LA(1);
						if ( (LA65_0==COMMA) ) {
							alt65=1;
						}

						switch (alt65) {
						case 1 :
							// antlr/Plexil.g:758:34: COMMA ! pair
							{
							COMMA256=(Token)match(input,COMMA,FOLLOW_COMMA_in_request4158); if (state.failed) return retval;
							pushFollow(FOLLOW_pair_in_request4161);
							pair257=pair();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, pair257.getTree());

							}
							break;

						default :
							break loop65;
						}
					}

					}
					break;

			}

			SEMICOLON258=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_request4169); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "request"


	public static class pair_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "pair"
	// antlr/Plexil.g:762:1: pair : NCNAME EQUALS ! expression ;
	public final PlexilParser.pair_return pair() throws RecognitionException {
		PlexilParser.pair_return retval = new PlexilParser.pair_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME259=null;
		Token EQUALS260=null;
		ParserRuleReturnScope expression261 =null;

		PlexilTreeNode NCNAME259_tree=null;
		PlexilTreeNode EQUALS260_tree=null;

		try {
			// antlr/Plexil.g:762:6: ( NCNAME EQUALS ! expression )
			// antlr/Plexil.g:762:8: NCNAME EQUALS ! expression
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			NCNAME259=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_pair4181); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NCNAME259_tree = (PlexilTreeNode)adaptor.create(NCNAME259);
			adaptor.addChild(root_0, NCNAME259_tree);
			}

			EQUALS260=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_pair4183); if (state.failed) return retval;
			pushFollow(FOLLOW_expression_in_pair4186);
			expression261=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression261.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "pair"


	public static class libraryCall_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "libraryCall"
	// antlr/Plexil.g:768:1: libraryCall : LIBRARY_CALL_KYWD ^ libraryNodeIdRef ( aliasSpecs )? SEMICOLON !;
	public final PlexilParser.libraryCall_return libraryCall() throws RecognitionException {
		PlexilParser.libraryCall_return retval = new PlexilParser.libraryCall_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LIBRARY_CALL_KYWD262=null;
		Token SEMICOLON265=null;
		ParserRuleReturnScope libraryNodeIdRef263 =null;
		ParserRuleReturnScope aliasSpecs264 =null;

		PlexilTreeNode LIBRARY_CALL_KYWD262_tree=null;
		PlexilTreeNode SEMICOLON265_tree=null;

		 m_paraphrases.push("in library action call"); 
		try {
			// antlr/Plexil.g:771:2: ( LIBRARY_CALL_KYWD ^ libraryNodeIdRef ( aliasSpecs )? SEMICOLON !)
			// antlr/Plexil.g:772:3: LIBRARY_CALL_KYWD ^ libraryNodeIdRef ( aliasSpecs )? SEMICOLON !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			LIBRARY_CALL_KYWD262=(Token)match(input,LIBRARY_CALL_KYWD,FOLLOW_LIBRARY_CALL_KYWD_in_libraryCall4212); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			LIBRARY_CALL_KYWD262_tree = (PlexilTreeNode)adaptor.create(LIBRARY_CALL_KYWD262);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(LIBRARY_CALL_KYWD262_tree, root_0);
			}

			pushFollow(FOLLOW_libraryNodeIdRef_in_libraryCall4215);
			libraryNodeIdRef263=libraryNodeIdRef();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, libraryNodeIdRef263.getTree());

			// antlr/Plexil.g:772:39: ( aliasSpecs )?
			int alt67=2;
			int LA67_0 = input.LA(1);
			if ( (LA67_0==LPAREN) ) {
				alt67=1;
			}
			switch (alt67) {
				case 1 :
					// antlr/Plexil.g:772:41: aliasSpecs
					{
					pushFollow(FOLLOW_aliasSpecs_in_libraryCall4219);
					aliasSpecs264=aliasSpecs();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, aliasSpecs264.getTree());

					}
					break;

			}

			SEMICOLON265=(Token)match(input,SEMICOLON,FOLLOW_SEMICOLON_in_libraryCall4224); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "libraryCall"


	public static class libraryNodeIdRef_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "libraryNodeIdRef"
	// antlr/Plexil.g:774:1: libraryNodeIdRef : NCNAME ;
	public final PlexilParser.libraryNodeIdRef_return libraryNodeIdRef() throws RecognitionException {
		PlexilParser.libraryNodeIdRef_return retval = new PlexilParser.libraryNodeIdRef_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME266=null;

		PlexilTreeNode NCNAME266_tree=null;

		try {
			// antlr/Plexil.g:774:18: ( NCNAME )
			// antlr/Plexil.g:774:20: NCNAME
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			NCNAME266=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_libraryNodeIdRef4234); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NCNAME266_tree = (PlexilTreeNode)adaptor.create(NCNAME266);
			adaptor.addChild(root_0, NCNAME266_tree);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "libraryNodeIdRef"


	public static class aliasSpecs_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "aliasSpecs"
	// antlr/Plexil.g:776:1: aliasSpecs : LPAREN ( aliasSpec ( COMMA aliasSpec )* )? RPAREN -> ^( ALIASES ( aliasSpec )* ) ;
	public final PlexilParser.aliasSpecs_return aliasSpecs() throws RecognitionException {
		PlexilParser.aliasSpecs_return retval = new PlexilParser.aliasSpecs_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LPAREN267=null;
		Token COMMA269=null;
		Token RPAREN271=null;
		ParserRuleReturnScope aliasSpec268 =null;
		ParserRuleReturnScope aliasSpec270 =null;

		PlexilTreeNode LPAREN267_tree=null;
		PlexilTreeNode COMMA269_tree=null;
		PlexilTreeNode RPAREN271_tree=null;
		RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleSubtreeStream stream_aliasSpec=new RewriteRuleSubtreeStream(adaptor,"rule aliasSpec");

		try {
			// antlr/Plexil.g:776:12: ( LPAREN ( aliasSpec ( COMMA aliasSpec )* )? RPAREN -> ^( ALIASES ( aliasSpec )* ) )
			// antlr/Plexil.g:777:3: LPAREN ( aliasSpec ( COMMA aliasSpec )* )? RPAREN
			{
			LPAREN267=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aliasSpecs4245); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN267);

			// antlr/Plexil.g:777:10: ( aliasSpec ( COMMA aliasSpec )* )?
			int alt69=2;
			int LA69_0 = input.LA(1);
			if ( (LA69_0==NCNAME) ) {
				alt69=1;
			}
			switch (alt69) {
				case 1 :
					// antlr/Plexil.g:777:12: aliasSpec ( COMMA aliasSpec )*
					{
					pushFollow(FOLLOW_aliasSpec_in_aliasSpecs4249);
					aliasSpec268=aliasSpec();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_aliasSpec.add(aliasSpec268.getTree());
					// antlr/Plexil.g:777:22: ( COMMA aliasSpec )*
					loop68:
					while (true) {
						int alt68=2;
						int LA68_0 = input.LA(1);
						if ( (LA68_0==COMMA) ) {
							alt68=1;
						}

						switch (alt68) {
						case 1 :
							// antlr/Plexil.g:777:24: COMMA aliasSpec
							{
							COMMA269=(Token)match(input,COMMA,FOLLOW_COMMA_in_aliasSpecs4253); if (state.failed) return retval; 
							if ( state.backtracking==0 ) stream_COMMA.add(COMMA269);

							pushFollow(FOLLOW_aliasSpec_in_aliasSpecs4255);
							aliasSpec270=aliasSpec();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) stream_aliasSpec.add(aliasSpec270.getTree());
							}
							break;

						default :
							break loop68;
						}
					}

					}
					break;

			}

			RPAREN271=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_aliasSpecs4263); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN271);

			// AST REWRITE
			// elements: aliasSpec
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 778:3: -> ^( ALIASES ( aliasSpec )* )
			{
				// antlr/Plexil.g:778:6: ^( ALIASES ( aliasSpec )* )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ALIASES, "ALIASES"), root_1);
				// antlr/Plexil.g:778:16: ( aliasSpec )*
				while ( stream_aliasSpec.hasNext() ) {
					adaptor.addChild(root_1, stream_aliasSpec.nextTree());
				}
				stream_aliasSpec.reset();

				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "aliasSpecs"


	public static class aliasSpec_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "aliasSpec"
	// antlr/Plexil.g:781:1: aliasSpec : NCNAME EQUALS expression -> ^( ALIAS NCNAME expression ) ;
	public final PlexilParser.aliasSpec_return aliasSpec() throws RecognitionException {
		PlexilParser.aliasSpec_return retval = new PlexilParser.aliasSpec_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME272=null;
		Token EQUALS273=null;
		ParserRuleReturnScope expression274 =null;

		PlexilTreeNode NCNAME272_tree=null;
		PlexilTreeNode EQUALS273_tree=null;
		RewriteRuleTokenStream stream_EQUALS=new RewriteRuleTokenStream(adaptor,"token EQUALS");
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");
		RewriteRuleSubtreeStream stream_expression=new RewriteRuleSubtreeStream(adaptor,"rule expression");

		try {
			// antlr/Plexil.g:781:11: ( NCNAME EQUALS expression -> ^( ALIAS NCNAME expression ) )
			// antlr/Plexil.g:782:3: NCNAME EQUALS expression
			{
			NCNAME272=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_aliasSpec4287); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME272);

			EQUALS273=(Token)match(input,EQUALS,FOLLOW_EQUALS_in_aliasSpec4289); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_EQUALS.add(EQUALS273);

			pushFollow(FOLLOW_expression_in_aliasSpec4291);
			expression274=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_expression.add(expression274.getTree());
			// AST REWRITE
			// elements: NCNAME, expression
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 783:9: -> ^( ALIAS NCNAME expression )
			{
				// antlr/Plexil.g:783:12: ^( ALIAS NCNAME expression )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ALIAS, "ALIAS"), root_1);
				adaptor.addChild(root_1, stream_NCNAME.nextNode());
				adaptor.addChild(root_1, stream_expression.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "aliasSpec"


	public static class nodeParameterName_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeParameterName"
	// antlr/Plexil.g:786:1: nodeParameterName : NCNAME ;
	public final PlexilParser.nodeParameterName_return nodeParameterName() throws RecognitionException {
		PlexilParser.nodeParameterName_return retval = new PlexilParser.nodeParameterName_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME275=null;

		PlexilTreeNode NCNAME275_tree=null;

		try {
			// antlr/Plexil.g:786:19: ( NCNAME )
			// antlr/Plexil.g:786:21: NCNAME
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			NCNAME275=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_nodeParameterName4319); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NCNAME275_tree = (PlexilTreeNode)adaptor.create(NCNAME275);
			adaptor.addChild(root_0, NCNAME275_tree);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeParameterName"


	public static class expression_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "expression"
	// antlr/Plexil.g:797:1: expression : logicalOr ;
	public final PlexilParser.expression_return expression() throws RecognitionException {
		PlexilParser.expression_return retval = new PlexilParser.expression_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope logicalOr276 =null;


		 m_paraphrases.push("in expression"); 
		try {
			// antlr/Plexil.g:800:2: ( logicalOr )
			// antlr/Plexil.g:800:4: logicalOr
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_logicalOr_in_expression4348);
			logicalOr276=logicalOr();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, logicalOr276.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "expression"


	public static class logicalOr_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "logicalOr"
	// antlr/Plexil.g:808:1: logicalOr : (o1= logicalXOR -> $o1) ( OR_KYWD on= logicalXOR -> ^( OR_KYWD $logicalOr $on) )* ;
	public final PlexilParser.logicalOr_return logicalOr() throws RecognitionException {
		PlexilParser.logicalOr_return retval = new PlexilParser.logicalOr_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token OR_KYWD277=null;
		ParserRuleReturnScope o1 =null;
		ParserRuleReturnScope on =null;

		PlexilTreeNode OR_KYWD277_tree=null;
		RewriteRuleTokenStream stream_OR_KYWD=new RewriteRuleTokenStream(adaptor,"token OR_KYWD");
		RewriteRuleSubtreeStream stream_logicalXOR=new RewriteRuleSubtreeStream(adaptor,"rule logicalXOR");

		try {
			// antlr/Plexil.g:808:11: ( (o1= logicalXOR -> $o1) ( OR_KYWD on= logicalXOR -> ^( OR_KYWD $logicalOr $on) )* )
			// antlr/Plexil.g:809:4: (o1= logicalXOR -> $o1) ( OR_KYWD on= logicalXOR -> ^( OR_KYWD $logicalOr $on) )*
			{
			// antlr/Plexil.g:809:4: (o1= logicalXOR -> $o1)
			// antlr/Plexil.g:809:6: o1= logicalXOR
			{
			pushFollow(FOLLOW_logicalXOR_in_logicalOr4370);
			o1=logicalXOR();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_logicalXOR.add(o1.getTree());
			// AST REWRITE
			// elements: o1
			// token labels: 
			// rule labels: o1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_o1=new RewriteRuleSubtreeStream(adaptor,"rule o1",o1!=null?o1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 809:20: -> $o1
			{
				adaptor.addChild(root_0, stream_o1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:810:4: ( OR_KYWD on= logicalXOR -> ^( OR_KYWD $logicalOr $on) )*
			loop70:
			while (true) {
				int alt70=2;
				int LA70_0 = input.LA(1);
				if ( (LA70_0==OR_KYWD) ) {
					alt70=1;
				}

				switch (alt70) {
				case 1 :
					// antlr/Plexil.g:810:6: OR_KYWD on= logicalXOR
					{
					OR_KYWD277=(Token)match(input,OR_KYWD,FOLLOW_OR_KYWD_in_logicalOr4384); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_OR_KYWD.add(OR_KYWD277);

					pushFollow(FOLLOW_logicalXOR_in_logicalOr4388);
					on=logicalXOR();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_logicalXOR.add(on.getTree());
					// AST REWRITE
					// elements: OR_KYWD, logicalOr, on
					// token labels: 
					// rule labels: retval, on
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);
					RewriteRuleSubtreeStream stream_on=new RewriteRuleSubtreeStream(adaptor,"rule on",on!=null?on.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 811:6: -> ^( OR_KYWD $logicalOr $on)
					{
						// antlr/Plexil.g:811:9: ^( OR_KYWD $logicalOr $on)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_OR_KYWD.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_on.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

				default :
					break loop70;
				}
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "logicalOr"


	public static class logicalXOR_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "logicalXOR"
	// antlr/Plexil.g:817:1: logicalXOR : (x1= logicalAnd -> $x1) ( XOR_KYWD xn= logicalAnd -> ^( XOR_KYWD $logicalXOR $xn) )* ;
	public final PlexilParser.logicalXOR_return logicalXOR() throws RecognitionException {
		PlexilParser.logicalXOR_return retval = new PlexilParser.logicalXOR_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token XOR_KYWD278=null;
		ParserRuleReturnScope x1 =null;
		ParserRuleReturnScope xn =null;

		PlexilTreeNode XOR_KYWD278_tree=null;
		RewriteRuleTokenStream stream_XOR_KYWD=new RewriteRuleTokenStream(adaptor,"token XOR_KYWD");
		RewriteRuleSubtreeStream stream_logicalAnd=new RewriteRuleSubtreeStream(adaptor,"rule logicalAnd");

		try {
			// antlr/Plexil.g:817:12: ( (x1= logicalAnd -> $x1) ( XOR_KYWD xn= logicalAnd -> ^( XOR_KYWD $logicalXOR $xn) )* )
			// antlr/Plexil.g:818:5: (x1= logicalAnd -> $x1) ( XOR_KYWD xn= logicalAnd -> ^( XOR_KYWD $logicalXOR $xn) )*
			{
			// antlr/Plexil.g:818:5: (x1= logicalAnd -> $x1)
			// antlr/Plexil.g:818:7: x1= logicalAnd
			{
			pushFollow(FOLLOW_logicalAnd_in_logicalXOR4431);
			x1=logicalAnd();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_logicalAnd.add(x1.getTree());
			// AST REWRITE
			// elements: x1
			// token labels: 
			// rule labels: x1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_x1=new RewriteRuleSubtreeStream(adaptor,"rule x1",x1!=null?x1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 818:21: -> $x1
			{
				adaptor.addChild(root_0, stream_x1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:819:5: ( XOR_KYWD xn= logicalAnd -> ^( XOR_KYWD $logicalXOR $xn) )*
			loop71:
			while (true) {
				int alt71=2;
				int LA71_0 = input.LA(1);
				if ( (LA71_0==XOR_KYWD) ) {
					alt71=1;
				}

				switch (alt71) {
				case 1 :
					// antlr/Plexil.g:819:7: XOR_KYWD xn= logicalAnd
					{
					XOR_KYWD278=(Token)match(input,XOR_KYWD,FOLLOW_XOR_KYWD_in_logicalXOR4446); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_XOR_KYWD.add(XOR_KYWD278);

					pushFollow(FOLLOW_logicalAnd_in_logicalXOR4450);
					xn=logicalAnd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_logicalAnd.add(xn.getTree());
					// AST REWRITE
					// elements: xn, logicalXOR, XOR_KYWD
					// token labels: 
					// rule labels: xn, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_xn=new RewriteRuleSubtreeStream(adaptor,"rule xn",xn!=null?xn.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 820:7: -> ^( XOR_KYWD $logicalXOR $xn)
					{
						// antlr/Plexil.g:820:10: ^( XOR_KYWD $logicalXOR $xn)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_XOR_KYWD.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_xn.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

				default :
					break loop71;
				}
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "logicalXOR"


	public static class logicalAnd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "logicalAnd"
	// antlr/Plexil.g:826:1: logicalAnd : (a1= equality -> $a1) ( AND_KYWD an= equality -> ^( AND_KYWD $logicalAnd $an) )* ;
	public final PlexilParser.logicalAnd_return logicalAnd() throws RecognitionException {
		PlexilParser.logicalAnd_return retval = new PlexilParser.logicalAnd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token AND_KYWD279=null;
		ParserRuleReturnScope a1 =null;
		ParserRuleReturnScope an =null;

		PlexilTreeNode AND_KYWD279_tree=null;
		RewriteRuleTokenStream stream_AND_KYWD=new RewriteRuleTokenStream(adaptor,"token AND_KYWD");
		RewriteRuleSubtreeStream stream_equality=new RewriteRuleSubtreeStream(adaptor,"rule equality");

		try {
			// antlr/Plexil.g:826:12: ( (a1= equality -> $a1) ( AND_KYWD an= equality -> ^( AND_KYWD $logicalAnd $an) )* )
			// antlr/Plexil.g:827:4: (a1= equality -> $a1) ( AND_KYWD an= equality -> ^( AND_KYWD $logicalAnd $an) )*
			{
			// antlr/Plexil.g:827:4: (a1= equality -> $a1)
			// antlr/Plexil.g:827:6: a1= equality
			{
			pushFollow(FOLLOW_equality_in_logicalAnd4495);
			a1=equality();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_equality.add(a1.getTree());
			// AST REWRITE
			// elements: a1
			// token labels: 
			// rule labels: a1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_a1=new RewriteRuleSubtreeStream(adaptor,"rule a1",a1!=null?a1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 827:18: -> $a1
			{
				adaptor.addChild(root_0, stream_a1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:828:4: ( AND_KYWD an= equality -> ^( AND_KYWD $logicalAnd $an) )*
			loop72:
			while (true) {
				int alt72=2;
				int LA72_0 = input.LA(1);
				if ( (LA72_0==AND_KYWD) ) {
					alt72=1;
				}

				switch (alt72) {
				case 1 :
					// antlr/Plexil.g:828:6: AND_KYWD an= equality
					{
					AND_KYWD279=(Token)match(input,AND_KYWD,FOLLOW_AND_KYWD_in_logicalAnd4509); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_AND_KYWD.add(AND_KYWD279);

					pushFollow(FOLLOW_equality_in_logicalAnd4513);
					an=equality();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_equality.add(an.getTree());
					// AST REWRITE
					// elements: logicalAnd, AND_KYWD, an
					// token labels: 
					// rule labels: an, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_an=new RewriteRuleSubtreeStream(adaptor,"rule an",an!=null?an.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 829:6: -> ^( AND_KYWD $logicalAnd $an)
					{
						// antlr/Plexil.g:829:9: ^( AND_KYWD $logicalAnd $an)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_AND_KYWD.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_an.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

				default :
					break loop72;
				}
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "logicalAnd"


	public static class equality_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "equality"
	// antlr/Plexil.g:839:1: equality : (e1= relational -> $e1) ( equalityOp en= relational -> ^( equalityOp $equality $en) )? ;
	public final PlexilParser.equality_return equality() throws RecognitionException {
		PlexilParser.equality_return retval = new PlexilParser.equality_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope e1 =null;
		ParserRuleReturnScope en =null;
		ParserRuleReturnScope equalityOp280 =null;

		RewriteRuleSubtreeStream stream_equalityOp=new RewriteRuleSubtreeStream(adaptor,"rule equalityOp");
		RewriteRuleSubtreeStream stream_relational=new RewriteRuleSubtreeStream(adaptor,"rule relational");

		try {
			// antlr/Plexil.g:839:10: ( (e1= relational -> $e1) ( equalityOp en= relational -> ^( equalityOp $equality $en) )? )
			// antlr/Plexil.g:840:5: (e1= relational -> $e1) ( equalityOp en= relational -> ^( equalityOp $equality $en) )?
			{
			// antlr/Plexil.g:840:5: (e1= relational -> $e1)
			// antlr/Plexil.g:840:7: e1= relational
			{
			pushFollow(FOLLOW_relational_in_equality4560);
			e1=relational();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_relational.add(e1.getTree());
			// AST REWRITE
			// elements: e1
			// token labels: 
			// rule labels: e1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_e1=new RewriteRuleSubtreeStream(adaptor,"rule e1",e1!=null?e1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 840:21: -> $e1
			{
				adaptor.addChild(root_0, stream_e1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:841:5: ( equalityOp en= relational -> ^( equalityOp $equality $en) )?
			int alt73=2;
			int LA73_0 = input.LA(1);
			if ( (LA73_0==DEQUALS||LA73_0==NEQUALS) ) {
				alt73=1;
			}
			switch (alt73) {
				case 1 :
					// antlr/Plexil.g:841:7: equalityOp en= relational
					{
					pushFollow(FOLLOW_equalityOp_in_equality4574);
					equalityOp280=equalityOp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_equalityOp.add(equalityOp280.getTree());
					pushFollow(FOLLOW_relational_in_equality4578);
					en=relational();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_relational.add(en.getTree());
					// AST REWRITE
					// elements: equality, equalityOp, en
					// token labels: 
					// rule labels: en, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_en=new RewriteRuleSubtreeStream(adaptor,"rule en",en!=null?en.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 842:4: -> ^( equalityOp $equality $en)
					{
						// antlr/Plexil.g:842:7: ^( equalityOp $equality $en)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_equalityOp.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_en.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "equality"


	public static class equalityOp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "equalityOp"
	// antlr/Plexil.g:846:1: equalityOp : ( DEQUALS | NEQUALS );
	public final PlexilParser.equalityOp_return equalityOp() throws RecognitionException {
		PlexilParser.equalityOp_return retval = new PlexilParser.equalityOp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set281=null;

		PlexilTreeNode set281_tree=null;

		try {
			// antlr/Plexil.g:846:12: ( DEQUALS | NEQUALS )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set281=input.LT(1);
			if ( input.LA(1)==DEQUALS||input.LA(1)==NEQUALS ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set281));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "equalityOp"


	public static class relational_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "relational"
	// antlr/Plexil.g:853:1: relational : (a1= additive -> $a1) ( relationalOp an= additive -> ^( relationalOp $relational $an) )? ;
	public final PlexilParser.relational_return relational() throws RecognitionException {
		PlexilParser.relational_return retval = new PlexilParser.relational_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope a1 =null;
		ParserRuleReturnScope an =null;
		ParserRuleReturnScope relationalOp282 =null;

		RewriteRuleSubtreeStream stream_relationalOp=new RewriteRuleSubtreeStream(adaptor,"rule relationalOp");
		RewriteRuleSubtreeStream stream_additive=new RewriteRuleSubtreeStream(adaptor,"rule additive");

		try {
			// antlr/Plexil.g:853:12: ( (a1= additive -> $a1) ( relationalOp an= additive -> ^( relationalOp $relational $an) )? )
			// antlr/Plexil.g:854:5: (a1= additive -> $a1) ( relationalOp an= additive -> ^( relationalOp $relational $an) )?
			{
			// antlr/Plexil.g:854:5: (a1= additive -> $a1)
			// antlr/Plexil.g:854:7: a1= additive
			{
			pushFollow(FOLLOW_additive_in_relational4638);
			a1=additive();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_additive.add(a1.getTree());
			// AST REWRITE
			// elements: a1
			// token labels: 
			// rule labels: a1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_a1=new RewriteRuleSubtreeStream(adaptor,"rule a1",a1!=null?a1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 854:19: -> $a1
			{
				adaptor.addChild(root_0, stream_a1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:855:2: ( relationalOp an= additive -> ^( relationalOp $relational $an) )?
			int alt74=2;
			int LA74_0 = input.LA(1);
			if ( (LA74_0==GEQ||LA74_0==GREATER||(LA74_0 >= LEQ && LA74_0 <= LESS)) ) {
				alt74=1;
			}
			switch (alt74) {
				case 1 :
					// antlr/Plexil.g:855:4: relationalOp an= additive
					{
					pushFollow(FOLLOW_relationalOp_in_relational4650);
					relationalOp282=relationalOp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_relationalOp.add(relationalOp282.getTree());
					pushFollow(FOLLOW_additive_in_relational4654);
					an=additive();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_additive.add(an.getTree());
					// AST REWRITE
					// elements: relational, relationalOp, an
					// token labels: 
					// rule labels: an, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_an=new RewriteRuleSubtreeStream(adaptor,"rule an",an!=null?an.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 856:4: -> ^( relationalOp $relational $an)
					{
						// antlr/Plexil.g:856:7: ^( relationalOp $relational $an)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_relationalOp.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_an.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "relational"


	public static class relationalOp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "relationalOp"
	// antlr/Plexil.g:860:1: relationalOp : ( GREATER | GEQ | LESS | LEQ );
	public final PlexilParser.relationalOp_return relationalOp() throws RecognitionException {
		PlexilParser.relationalOp_return retval = new PlexilParser.relationalOp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set283=null;

		PlexilTreeNode set283_tree=null;

		try {
			// antlr/Plexil.g:860:14: ( GREATER | GEQ | LESS | LEQ )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set283=input.LT(1);
			if ( input.LA(1)==GEQ||input.LA(1)==GREATER||(input.LA(1) >= LEQ && input.LA(1) <= LESS) ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set283));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "relationalOp"


	public static class additive_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "additive"
	// antlr/Plexil.g:871:1: additive : (m1= multiplicative -> $m1) ( addOp mn= multiplicative -> ^( addOp $additive $mn) )* ;
	public final PlexilParser.additive_return additive() throws RecognitionException {
		PlexilParser.additive_return retval = new PlexilParser.additive_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope m1 =null;
		ParserRuleReturnScope mn =null;
		ParserRuleReturnScope addOp284 =null;

		RewriteRuleSubtreeStream stream_addOp=new RewriteRuleSubtreeStream(adaptor,"rule addOp");
		RewriteRuleSubtreeStream stream_multiplicative=new RewriteRuleSubtreeStream(adaptor,"rule multiplicative");

		try {
			// antlr/Plexil.g:871:10: ( (m1= multiplicative -> $m1) ( addOp mn= multiplicative -> ^( addOp $additive $mn) )* )
			// antlr/Plexil.g:872:5: (m1= multiplicative -> $m1) ( addOp mn= multiplicative -> ^( addOp $additive $mn) )*
			{
			// antlr/Plexil.g:872:5: (m1= multiplicative -> $m1)
			// antlr/Plexil.g:872:7: m1= multiplicative
			{
			pushFollow(FOLLOW_multiplicative_in_additive4731);
			m1=multiplicative();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_multiplicative.add(m1.getTree());
			// AST REWRITE
			// elements: m1
			// token labels: 
			// rule labels: m1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_m1=new RewriteRuleSubtreeStream(adaptor,"rule m1",m1!=null?m1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 872:25: -> $m1
			{
				adaptor.addChild(root_0, stream_m1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:873:5: ( addOp mn= multiplicative -> ^( addOp $additive $mn) )*
			loop75:
			while (true) {
				int alt75=2;
				int LA75_0 = input.LA(1);
				if ( (LA75_0==MINUS||LA75_0==PLUS) ) {
					alt75=1;
				}

				switch (alt75) {
				case 1 :
					// antlr/Plexil.g:873:7: addOp mn= multiplicative
					{
					pushFollow(FOLLOW_addOp_in_additive4746);
					addOp284=addOp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_addOp.add(addOp284.getTree());
					pushFollow(FOLLOW_multiplicative_in_additive4750);
					mn=multiplicative();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_multiplicative.add(mn.getTree());
					// AST REWRITE
					// elements: addOp, mn, additive
					// token labels: 
					// rule labels: mn, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_mn=new RewriteRuleSubtreeStream(adaptor,"rule mn",mn!=null?mn.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 874:4: -> ^( addOp $additive $mn)
					{
						// antlr/Plexil.g:874:7: ^( addOp $additive $mn)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_addOp.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_mn.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

				default :
					break loop75;
				}
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "additive"


	public static class addOp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "addOp"
	// antlr/Plexil.g:878:1: addOp : ( PLUS | MINUS );
	public final PlexilParser.addOp_return addOp() throws RecognitionException {
		PlexilParser.addOp_return retval = new PlexilParser.addOp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set285=null;

		PlexilTreeNode set285_tree=null;

		try {
			// antlr/Plexil.g:878:7: ( PLUS | MINUS )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set285=input.LT(1);
			if ( input.LA(1)==MINUS||input.LA(1)==PLUS ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set285));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "addOp"


	public static class multiplicative_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "multiplicative"
	// antlr/Plexil.g:885:1: multiplicative : (u1= unary -> $u1) ( multOp un= unary -> ^( multOp $multiplicative $un) )* ;
	public final PlexilParser.multiplicative_return multiplicative() throws RecognitionException {
		PlexilParser.multiplicative_return retval = new PlexilParser.multiplicative_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope u1 =null;
		ParserRuleReturnScope un =null;
		ParserRuleReturnScope multOp286 =null;

		RewriteRuleSubtreeStream stream_multOp=new RewriteRuleSubtreeStream(adaptor,"rule multOp");
		RewriteRuleSubtreeStream stream_unary=new RewriteRuleSubtreeStream(adaptor,"rule unary");

		try {
			// antlr/Plexil.g:885:16: ( (u1= unary -> $u1) ( multOp un= unary -> ^( multOp $multiplicative $un) )* )
			// antlr/Plexil.g:886:5: (u1= unary -> $u1) ( multOp un= unary -> ^( multOp $multiplicative $un) )*
			{
			// antlr/Plexil.g:886:5: (u1= unary -> $u1)
			// antlr/Plexil.g:886:7: u1= unary
			{
			pushFollow(FOLLOW_unary_in_multiplicative4811);
			u1=unary();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_unary.add(u1.getTree());
			// AST REWRITE
			// elements: u1
			// token labels: 
			// rule labels: u1, retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_u1=new RewriteRuleSubtreeStream(adaptor,"rule u1",u1!=null?u1.getTree():null);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 886:16: -> $u1
			{
				adaptor.addChild(root_0, stream_u1.nextTree());
			}


			retval.tree = root_0;
			}

			}

			// antlr/Plexil.g:887:5: ( multOp un= unary -> ^( multOp $multiplicative $un) )*
			loop76:
			while (true) {
				int alt76=2;
				int LA76_0 = input.LA(1);
				if ( (LA76_0==ASTERISK||LA76_0==MOD_KYWD||LA76_0==PERCENT||LA76_0==SLASH) ) {
					alt76=1;
				}

				switch (alt76) {
				case 1 :
					// antlr/Plexil.g:887:7: multOp un= unary
					{
					pushFollow(FOLLOW_multOp_in_multiplicative4826);
					multOp286=multOp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_multOp.add(multOp286.getTree());
					pushFollow(FOLLOW_unary_in_multiplicative4830);
					un=unary();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_unary.add(un.getTree());
					// AST REWRITE
					// elements: multOp, un, multiplicative
					// token labels: 
					// rule labels: un, retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_un=new RewriteRuleSubtreeStream(adaptor,"rule un",un!=null?un.getTree():null);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 888:7: -> ^( multOp $multiplicative $un)
					{
						// antlr/Plexil.g:888:10: ^( multOp $multiplicative $un)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot(stream_multOp.nextNode(), root_1);
						adaptor.addChild(root_1, stream_retval.nextTree());
						adaptor.addChild(root_1, stream_un.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

				default :
					break loop76;
				}
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "multiplicative"


	public static class multOp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "multOp"
	// antlr/Plexil.g:892:1: multOp : ( ASTERISK | SLASH | PERCENT | MOD_KYWD );
	public final PlexilParser.multOp_return multOp() throws RecognitionException {
		PlexilParser.multOp_return retval = new PlexilParser.multOp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set287=null;

		PlexilTreeNode set287_tree=null;

		try {
			// antlr/Plexil.g:892:8: ( ASTERISK | SLASH | PERCENT | MOD_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set287=input.LT(1);
			if ( input.LA(1)==ASTERISK||input.LA(1)==MOD_KYWD||input.LA(1)==PERCENT||input.LA(1)==SLASH ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set287));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "multOp"


	public static class unary_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "unary"
	// antlr/Plexil.g:906:1: unary : ( unaryMinus | unaryOp ^ quantity | quantity );
	public final PlexilParser.unary_return unary() throws RecognitionException {
		PlexilParser.unary_return retval = new PlexilParser.unary_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope unaryMinus288 =null;
		ParserRuleReturnScope unaryOp289 =null;
		ParserRuleReturnScope quantity290 =null;
		ParserRuleReturnScope quantity291 =null;


		try {
			// antlr/Plexil.g:906:7: ( unaryMinus | unaryOp ^ quantity | quantity )
			int alt77=3;
			switch ( input.LA(1) ) {
			case MINUS:
				{
				int LA77_1 = input.LA(2);
				if ( (LA77_1==INT) ) {
					alt77=1;
				}
				else if ( (LA77_1==DOUBLE) ) {
					alt77=1;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 77, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

				}
				break;
			case NOT_KYWD:
				{
				alt77=2;
				}
				break;
			case ABS_KYWD:
			case ARRAY_MAX_SIZE_KYWD:
			case ARRAY_SIZE_KYWD:
			case BAR:
			case CEIL_KYWD:
			case CHILD_KYWD:
			case COMMAND_ACCEPTED_KYWD:
			case COMMAND_DENIED_KYWD:
			case COMMAND_FAILED_KYWD:
			case COMMAND_RCVD_KYWD:
			case COMMAND_SENT_KYWD:
			case COMMAND_SUCCESS_KYWD:
			case DATE_KYWD:
			case DOUBLE:
			case DURATION_KYWD:
			case EXECUTING_STATE_KYWD:
			case EXITED_KYWD:
			case FAILING_STATE_KYWD:
			case FAILURE_OUTCOME_KYWD:
			case FALSE_KYWD:
			case FINISHED_STATE_KYWD:
			case FINISHING_STATE_KYWD:
			case FLOOR_KYWD:
			case HASHPAREN:
			case INACTIVE_STATE_KYWD:
			case INT:
			case INTERRUPTED_OUTCOME_KYWD:
			case INVARIANT_CONDITION_FAILED_KYWD:
			case IS_KNOWN_KYWD:
			case ITERATION_ENDED_STATE_KYWD:
			case LOOKUP_KYWD:
			case LOOKUP_NOW_KYWD:
			case LOOKUP_ON_CHANGE_KYWD:
			case LPAREN:
			case MAX_KYWD:
			case MESSAGE_RECEIVED_KYWD:
			case MIN_KYWD:
			case NCNAME:
			case NODE_EXECUTING_KYWD:
			case NODE_FAILED_KYWD:
			case NODE_FINISHED_KYWD:
			case NODE_INACTIVE_KYWD:
			case NODE_INVARIANT_FAILED_KYWD:
			case NODE_ITERATION_ENDED_KYWD:
			case NODE_ITERATION_FAILED_KYWD:
			case NODE_ITERATION_SUCCEEDED_KYWD:
			case NODE_PARENT_FAILED_KYWD:
			case NODE_POSTCONDITION_FAILED_KYWD:
			case NODE_PRECONDITION_FAILED_KYWD:
			case NODE_SKIPPED_KYWD:
			case NODE_SUCCEEDED_KYWD:
			case NODE_WAITING_KYWD:
			case PARENT_EXITED_KYWD:
			case PARENT_FAILED_KYWD:
			case PARENT_KYWD:
			case POST_CONDITION_FAILED_KYWD:
			case PRE_CONDITION_FAILED_KYWD:
			case REAL_TO_INT_KYWD:
			case ROUND_KYWD:
			case SELF_KYWD:
			case SIBLING_KYWD:
			case SKIPPED_OUTCOME_KYWD:
			case SQRT_KYWD:
			case STRING:
			case STRLEN_KYWD:
			case SUCCESS_OUTCOME_KYWD:
			case TRUE_KYWD:
			case TRUNC_KYWD:
			case WAITING_STATE_KYWD:
				{
				alt77=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 77, 0, input);
				throw nvae;
			}
			switch (alt77) {
				case 1 :
					// antlr/Plexil.g:906:9: unaryMinus
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_unaryMinus_in_unary4904);
					unaryMinus288=unaryMinus();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, unaryMinus288.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:907:9: unaryOp ^ quantity
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_unaryOp_in_unary4914);
					unaryOp289=unaryOp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) root_0 = (PlexilTreeNode)adaptor.becomeRoot(unaryOp289.getTree(), root_0);
					pushFollow(FOLLOW_quantity_in_unary4917);
					quantity290=quantity();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, quantity290.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:908:9: quantity
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_quantity_in_unary4927);
					quantity291=quantity();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, quantity291.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "unary"


	public static class unaryOp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "unaryOp"
	// antlr/Plexil.g:911:1: unaryOp : NOT_KYWD ;
	public final PlexilParser.unaryOp_return unaryOp() throws RecognitionException {
		PlexilParser.unaryOp_return retval = new PlexilParser.unaryOp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NOT_KYWD292=null;

		PlexilTreeNode NOT_KYWD292_tree=null;

		try {
			// antlr/Plexil.g:911:9: ( NOT_KYWD )
			// antlr/Plexil.g:911:11: NOT_KYWD
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			NOT_KYWD292=(Token)match(input,NOT_KYWD,FOLLOW_NOT_KYWD_in_unaryOp4942); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			NOT_KYWD292_tree = (PlexilTreeNode)adaptor.create(NOT_KYWD292);
			adaptor.addChild(root_0, NOT_KYWD292_tree);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "unaryOp"


	public static class unaryMinus_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "unaryMinus"
	// antlr/Plexil.g:913:1: unaryMinus : ( ( MINUS i= INT ) -> ^( NEG_INT $i) | ( MINUS d= DOUBLE ) -> ^( NEG_DOUBLE $d) );
	public final PlexilParser.unaryMinus_return unaryMinus() throws RecognitionException {
		PlexilParser.unaryMinus_return retval = new PlexilParser.unaryMinus_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token i=null;
		Token d=null;
		Token MINUS293=null;
		Token MINUS294=null;

		PlexilTreeNode i_tree=null;
		PlexilTreeNode d_tree=null;
		PlexilTreeNode MINUS293_tree=null;
		PlexilTreeNode MINUS294_tree=null;
		RewriteRuleTokenStream stream_DOUBLE=new RewriteRuleTokenStream(adaptor,"token DOUBLE");
		RewriteRuleTokenStream stream_INT=new RewriteRuleTokenStream(adaptor,"token INT");
		RewriteRuleTokenStream stream_MINUS=new RewriteRuleTokenStream(adaptor,"token MINUS");

		try {
			// antlr/Plexil.g:913:12: ( ( MINUS i= INT ) -> ^( NEG_INT $i) | ( MINUS d= DOUBLE ) -> ^( NEG_DOUBLE $d) )
			int alt78=2;
			int LA78_0 = input.LA(1);
			if ( (LA78_0==MINUS) ) {
				int LA78_1 = input.LA(2);
				if ( (LA78_1==INT) ) {
					alt78=1;
				}
				else if ( (LA78_1==DOUBLE) ) {
					alt78=2;
				}

				else {
					if (state.backtracking>0) {state.failed=true; return retval;}
					int nvaeMark = input.mark();
					try {
						input.consume();
						NoViableAltException nvae =
							new NoViableAltException("", 78, 1, input);
						throw nvae;
					} finally {
						input.rewind(nvaeMark);
					}
				}

			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 78, 0, input);
				throw nvae;
			}

			switch (alt78) {
				case 1 :
					// antlr/Plexil.g:913:14: ( MINUS i= INT )
					{
					// antlr/Plexil.g:913:14: ( MINUS i= INT )
					// antlr/Plexil.g:913:15: MINUS i= INT
					{
					MINUS293=(Token)match(input,MINUS,FOLLOW_MINUS_in_unaryMinus4952); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_MINUS.add(MINUS293);

					i=(Token)match(input,INT,FOLLOW_INT_in_unaryMinus4956); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_INT.add(i);

					}

					// AST REWRITE
					// elements: i
					// token labels: i
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleTokenStream stream_i=new RewriteRuleTokenStream(adaptor,"token i",i);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 913:28: -> ^( NEG_INT $i)
					{
						// antlr/Plexil.g:913:31: ^( NEG_INT $i)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NEG_INT, "NEG_INT"), root_1);
						adaptor.addChild(root_1, stream_i.nextNode());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;
				case 2 :
					// antlr/Plexil.g:914:14: ( MINUS d= DOUBLE )
					{
					// antlr/Plexil.g:914:14: ( MINUS d= DOUBLE )
					// antlr/Plexil.g:914:15: MINUS d= DOUBLE
					{
					MINUS294=(Token)match(input,MINUS,FOLLOW_MINUS_in_unaryMinus4982); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_MINUS.add(MINUS294);

					d=(Token)match(input,DOUBLE,FOLLOW_DOUBLE_in_unaryMinus4986); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_DOUBLE.add(d);

					}

					// AST REWRITE
					// elements: d
					// token labels: d
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleTokenStream stream_d=new RewriteRuleTokenStream(adaptor,"token d",d);
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 914:31: -> ^( NEG_DOUBLE $d)
					{
						// antlr/Plexil.g:914:34: ^( NEG_DOUBLE $d)
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NEG_DOUBLE, "NEG_DOUBLE"), root_1);
						adaptor.addChild(root_1, stream_d.nextNode());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "unaryMinus"


	public static class dateLiteral_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "dateLiteral"
	// antlr/Plexil.g:917:1: dateLiteral : ( DATE_KYWD LPAREN s= STRING RPAREN ) -> ^( DATE_LITERAL $s) ;
	public final PlexilParser.dateLiteral_return dateLiteral() throws RecognitionException {
		PlexilParser.dateLiteral_return retval = new PlexilParser.dateLiteral_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token s=null;
		Token DATE_KYWD295=null;
		Token LPAREN296=null;
		Token RPAREN297=null;

		PlexilTreeNode s_tree=null;
		PlexilTreeNode DATE_KYWD295_tree=null;
		PlexilTreeNode LPAREN296_tree=null;
		PlexilTreeNode RPAREN297_tree=null;
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_STRING=new RewriteRuleTokenStream(adaptor,"token STRING");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
		RewriteRuleTokenStream stream_DATE_KYWD=new RewriteRuleTokenStream(adaptor,"token DATE_KYWD");

		try {
			// antlr/Plexil.g:917:13: ( ( DATE_KYWD LPAREN s= STRING RPAREN ) -> ^( DATE_LITERAL $s) )
			// antlr/Plexil.g:917:15: ( DATE_KYWD LPAREN s= STRING RPAREN )
			{
			// antlr/Plexil.g:917:15: ( DATE_KYWD LPAREN s= STRING RPAREN )
			// antlr/Plexil.g:917:16: DATE_KYWD LPAREN s= STRING RPAREN
			{
			DATE_KYWD295=(Token)match(input,DATE_KYWD,FOLLOW_DATE_KYWD_in_dateLiteral5017); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_DATE_KYWD.add(DATE_KYWD295);

			LPAREN296=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_dateLiteral5019); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN296);

			s=(Token)match(input,STRING,FOLLOW_STRING_in_dateLiteral5023); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_STRING.add(s);

			RPAREN297=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_dateLiteral5025); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN297);

			}

			// AST REWRITE
			// elements: s
			// token labels: s
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleTokenStream stream_s=new RewriteRuleTokenStream(adaptor,"token s",s);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 917:50: -> ^( DATE_LITERAL $s)
			{
				// antlr/Plexil.g:917:53: ^( DATE_LITERAL $s)
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(DATE_LITERAL, "DATE_LITERAL"), root_1);
				adaptor.addChild(root_1, stream_s.nextNode());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "dateLiteral"


	public static class durationLiteral_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "durationLiteral"
	// antlr/Plexil.g:919:1: durationLiteral : ( DURATION_KYWD LPAREN s= STRING RPAREN ) -> ^( DURATION_LITERAL $s) ;
	public final PlexilParser.durationLiteral_return durationLiteral() throws RecognitionException {
		PlexilParser.durationLiteral_return retval = new PlexilParser.durationLiteral_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token s=null;
		Token DURATION_KYWD298=null;
		Token LPAREN299=null;
		Token RPAREN300=null;

		PlexilTreeNode s_tree=null;
		PlexilTreeNode DURATION_KYWD298_tree=null;
		PlexilTreeNode LPAREN299_tree=null;
		PlexilTreeNode RPAREN300_tree=null;
		RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
		RewriteRuleTokenStream stream_DURATION_KYWD=new RewriteRuleTokenStream(adaptor,"token DURATION_KYWD");
		RewriteRuleTokenStream stream_STRING=new RewriteRuleTokenStream(adaptor,"token STRING");
		RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");

		try {
			// antlr/Plexil.g:919:17: ( ( DURATION_KYWD LPAREN s= STRING RPAREN ) -> ^( DURATION_LITERAL $s) )
			// antlr/Plexil.g:919:19: ( DURATION_KYWD LPAREN s= STRING RPAREN )
			{
			// antlr/Plexil.g:919:19: ( DURATION_KYWD LPAREN s= STRING RPAREN )
			// antlr/Plexil.g:919:20: DURATION_KYWD LPAREN s= STRING RPAREN
			{
			DURATION_KYWD298=(Token)match(input,DURATION_KYWD,FOLLOW_DURATION_KYWD_in_durationLiteral5045); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_DURATION_KYWD.add(DURATION_KYWD298);

			LPAREN299=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_durationLiteral5047); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN299);

			s=(Token)match(input,STRING,FOLLOW_STRING_in_durationLiteral5051); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_STRING.add(s);

			RPAREN300=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_durationLiteral5053); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN300);

			}

			// AST REWRITE
			// elements: s
			// token labels: s
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleTokenStream stream_s=new RewriteRuleTokenStream(adaptor,"token s",s);
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 919:58: -> ^( DURATION_LITERAL $s)
			{
				// antlr/Plexil.g:919:61: ^( DURATION_LITERAL $s)
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(DURATION_LITERAL, "DURATION_LITERAL"), root_1);
				adaptor.addChild(root_1, stream_s.nextNode());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "durationLiteral"


	public static class quantity_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "quantity"
	// antlr/Plexil.g:927:1: quantity : ( LPAREN ! expression RPAREN !| BAR expression BAR -> ^( ABS_KYWD expression ) | oneArgFn ^ LPAREN ! expression RPAREN !| twoArgFn ^ LPAREN ! expression COMMA ! expression RPAREN !| isKnownExp | ( lookupExpr LBRACKET )=> lookupArrayReference | lookupExpr | messageReceivedExp | nodeStatePredicateExp | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD FAILURE_KYWD )=> nodeFailureVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD STATE_KYWD )=> nodeStateVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD nodeStateKywd )=> nodeTimepointValue | ( NCNAME LBRACKET )=> simpleArrayReference | variable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD )=> nodeFailureVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD STATE_KYWD )=> nodeStateVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD nodeStateKywd )=> nodeTimepointValue | literalValue | nodeCommandHandleKywd | nodeFailureKywd | nodeStateKywd | nodeOutcomeKywd );
	public final PlexilParser.quantity_return quantity() throws RecognitionException {
		PlexilParser.quantity_return retval = new PlexilParser.quantity_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LPAREN301=null;
		Token RPAREN303=null;
		Token BAR304=null;
		Token BAR306=null;
		Token LPAREN308=null;
		Token RPAREN310=null;
		Token LPAREN312=null;
		Token COMMA314=null;
		Token RPAREN316=null;
		ParserRuleReturnScope expression302 =null;
		ParserRuleReturnScope expression305 =null;
		ParserRuleReturnScope oneArgFn307 =null;
		ParserRuleReturnScope expression309 =null;
		ParserRuleReturnScope twoArgFn311 =null;
		ParserRuleReturnScope expression313 =null;
		ParserRuleReturnScope expression315 =null;
		ParserRuleReturnScope isKnownExp317 =null;
		ParserRuleReturnScope lookupArrayReference318 =null;
		ParserRuleReturnScope lookupExpr319 =null;
		ParserRuleReturnScope messageReceivedExp320 =null;
		ParserRuleReturnScope nodeStatePredicateExp321 =null;
		ParserRuleReturnScope nodeCommandHandleVariable322 =null;
		ParserRuleReturnScope nodeFailureVariable323 =null;
		ParserRuleReturnScope nodeOutcomeVariable324 =null;
		ParserRuleReturnScope nodeStateVariable325 =null;
		ParserRuleReturnScope nodeTimepointValue326 =null;
		ParserRuleReturnScope simpleArrayReference327 =null;
		ParserRuleReturnScope variable328 =null;
		ParserRuleReturnScope nodeCommandHandleVariable329 =null;
		ParserRuleReturnScope nodeFailureVariable330 =null;
		ParserRuleReturnScope nodeOutcomeVariable331 =null;
		ParserRuleReturnScope nodeStateVariable332 =null;
		ParserRuleReturnScope nodeTimepointValue333 =null;
		ParserRuleReturnScope literalValue334 =null;
		ParserRuleReturnScope nodeCommandHandleKywd335 =null;
		ParserRuleReturnScope nodeFailureKywd336 =null;
		ParserRuleReturnScope nodeStateKywd337 =null;
		ParserRuleReturnScope nodeOutcomeKywd338 =null;

		PlexilTreeNode LPAREN301_tree=null;
		PlexilTreeNode RPAREN303_tree=null;
		PlexilTreeNode BAR304_tree=null;
		PlexilTreeNode BAR306_tree=null;
		PlexilTreeNode LPAREN308_tree=null;
		PlexilTreeNode RPAREN310_tree=null;
		PlexilTreeNode LPAREN312_tree=null;
		PlexilTreeNode COMMA314_tree=null;
		PlexilTreeNode RPAREN316_tree=null;
		RewriteRuleTokenStream stream_BAR=new RewriteRuleTokenStream(adaptor,"token BAR");
		RewriteRuleSubtreeStream stream_expression=new RewriteRuleSubtreeStream(adaptor,"rule expression");

		try {
			// antlr/Plexil.g:927:10: ( LPAREN ! expression RPAREN !| BAR expression BAR -> ^( ABS_KYWD expression ) | oneArgFn ^ LPAREN ! expression RPAREN !| twoArgFn ^ LPAREN ! expression COMMA ! expression RPAREN !| isKnownExp | ( lookupExpr LBRACKET )=> lookupArrayReference | lookupExpr | messageReceivedExp | nodeStatePredicateExp | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD FAILURE_KYWD )=> nodeFailureVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD STATE_KYWD )=> nodeStateVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD nodeStateKywd )=> nodeTimepointValue | ( NCNAME LBRACKET )=> simpleArrayReference | variable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD )=> nodeFailureVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD STATE_KYWD )=> nodeStateVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD nodeStateKywd )=> nodeTimepointValue | literalValue | nodeCommandHandleKywd | nodeFailureKywd | nodeStateKywd | nodeOutcomeKywd )
			int alt79=26;
			alt79 = dfa79.predict(input);
			switch (alt79) {
				case 1 :
					// antlr/Plexil.g:928:5: LPAREN ! expression RPAREN !
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					LPAREN301=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_quantity5082); if (state.failed) return retval;
					pushFollow(FOLLOW_expression_in_quantity5085);
					expression302=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression302.getTree());

					RPAREN303=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_quantity5087); if (state.failed) return retval;
					}
					break;
				case 2 :
					// antlr/Plexil.g:929:5: BAR expression BAR
					{
					BAR304=(Token)match(input,BAR,FOLLOW_BAR_in_quantity5094); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_BAR.add(BAR304);

					pushFollow(FOLLOW_expression_in_quantity5096);
					expression305=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) stream_expression.add(expression305.getTree());
					BAR306=(Token)match(input,BAR,FOLLOW_BAR_in_quantity5098); if (state.failed) return retval; 
					if ( state.backtracking==0 ) stream_BAR.add(BAR306);

					// AST REWRITE
					// elements: expression
					// token labels: 
					// rule labels: retval
					// token list labels: 
					// rule list labels: 
					// wildcard labels: 
					if ( state.backtracking==0 ) {
					retval.tree = root_0;
					RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

					root_0 = (PlexilTreeNode)adaptor.nil();
					// 929:24: -> ^( ABS_KYWD expression )
					{
						// antlr/Plexil.g:929:27: ^( ABS_KYWD expression )
						{
						PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
						root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(ABS_KYWD, "ABS_KYWD"), root_1);
						adaptor.addChild(root_1, stream_expression.nextTree());
						adaptor.addChild(root_0, root_1);
						}

					}


					retval.tree = root_0;
					}

					}
					break;
				case 3 :
					// antlr/Plexil.g:930:5: oneArgFn ^ LPAREN ! expression RPAREN !
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_oneArgFn_in_quantity5112);
					oneArgFn307=oneArgFn();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) root_0 = (PlexilTreeNode)adaptor.becomeRoot(oneArgFn307.getTree(), root_0);
					LPAREN308=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_quantity5115); if (state.failed) return retval;
					pushFollow(FOLLOW_expression_in_quantity5118);
					expression309=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression309.getTree());

					RPAREN310=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_quantity5120); if (state.failed) return retval;
					}
					break;
				case 4 :
					// antlr/Plexil.g:931:5: twoArgFn ^ LPAREN ! expression COMMA ! expression RPAREN !
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_twoArgFn_in_quantity5127);
					twoArgFn311=twoArgFn();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) root_0 = (PlexilTreeNode)adaptor.becomeRoot(twoArgFn311.getTree(), root_0);
					LPAREN312=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_quantity5130); if (state.failed) return retval;
					pushFollow(FOLLOW_expression_in_quantity5133);
					expression313=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression313.getTree());

					COMMA314=(Token)match(input,COMMA,FOLLOW_COMMA_in_quantity5135); if (state.failed) return retval;
					pushFollow(FOLLOW_expression_in_quantity5138);
					expression315=expression();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, expression315.getTree());

					RPAREN316=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_quantity5140); if (state.failed) return retval;
					}
					break;
				case 5 :
					// antlr/Plexil.g:932:5: isKnownExp
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_isKnownExp_in_quantity5147);
					isKnownExp317=isKnownExp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, isKnownExp317.getTree());

					}
					break;
				case 6 :
					// antlr/Plexil.g:933:5: ( lookupExpr LBRACKET )=> lookupArrayReference
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_lookupArrayReference_in_quantity5161);
					lookupArrayReference318=lookupArrayReference();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupArrayReference318.getTree());

					}
					break;
				case 7 :
					// antlr/Plexil.g:934:5: lookupExpr
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_lookupExpr_in_quantity5167);
					lookupExpr319=lookupExpr();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupExpr319.getTree());

					}
					break;
				case 8 :
					// antlr/Plexil.g:935:5: messageReceivedExp
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_messageReceivedExp_in_quantity5173);
					messageReceivedExp320=messageReceivedExp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, messageReceivedExp320.getTree());

					}
					break;
				case 9 :
					// antlr/Plexil.g:936:5: nodeStatePredicateExp
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeStatePredicateExp_in_quantity5179);
					nodeStatePredicateExp321=nodeStatePredicateExp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeStatePredicateExp321.getTree());

					}
					break;
				case 10 :
					// antlr/Plexil.g:937:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeCommandHandleVariable_in_quantity5206);
					nodeCommandHandleVariable322=nodeCommandHandleVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeCommandHandleVariable322.getTree());

					}
					break;
				case 11 :
					// antlr/Plexil.g:938:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD FAILURE_KYWD )=> nodeFailureVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeFailureVariable_in_quantity5233);
					nodeFailureVariable323=nodeFailureVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeFailureVariable323.getTree());

					}
					break;
				case 12 :
					// antlr/Plexil.g:939:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeOutcomeVariable_in_quantity5260);
					nodeOutcomeVariable324=nodeOutcomeVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeOutcomeVariable324.getTree());

					}
					break;
				case 13 :
					// antlr/Plexil.g:940:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD STATE_KYWD )=> nodeStateVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeStateVariable_in_quantity5287);
					nodeStateVariable325=nodeStateVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeStateVariable325.getTree());

					}
					break;
				case 14 :
					// antlr/Plexil.g:941:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD nodeStateKywd )=> nodeTimepointValue
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeTimepointValue_in_quantity5314);
					nodeTimepointValue326=nodeTimepointValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeTimepointValue326.getTree());

					}
					break;
				case 15 :
					// antlr/Plexil.g:942:5: ( NCNAME LBRACKET )=> simpleArrayReference
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_simpleArrayReference_in_quantity5328);
					simpleArrayReference327=simpleArrayReference();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, simpleArrayReference327.getTree());

					}
					break;
				case 16 :
					// antlr/Plexil.g:943:5: variable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_variable_in_quantity5334);
					variable328=variable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, variable328.getTree());

					}
					break;
				case 17 :
					// antlr/Plexil.g:945:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeCommandHandleVariable_in_quantity5364);
					nodeCommandHandleVariable329=nodeCommandHandleVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeCommandHandleVariable329.getTree());

					}
					break;
				case 18 :
					// antlr/Plexil.g:946:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD )=> nodeFailureVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeFailureVariable_in_quantity5393);
					nodeFailureVariable330=nodeFailureVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeFailureVariable330.getTree());

					}
					break;
				case 19 :
					// antlr/Plexil.g:947:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeOutcomeVariable_in_quantity5422);
					nodeOutcomeVariable331=nodeOutcomeVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeOutcomeVariable331.getTree());

					}
					break;
				case 20 :
					// antlr/Plexil.g:948:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD STATE_KYWD )=> nodeStateVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeStateVariable_in_quantity5451);
					nodeStateVariable332=nodeStateVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeStateVariable332.getTree());

					}
					break;
				case 21 :
					// antlr/Plexil.g:949:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD nodeStateKywd )=> nodeTimepointValue
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeTimepointValue_in_quantity5480);
					nodeTimepointValue333=nodeTimepointValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeTimepointValue333.getTree());

					}
					break;
				case 22 :
					// antlr/Plexil.g:950:5: literalValue
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_literalValue_in_quantity5486);
					literalValue334=literalValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, literalValue334.getTree());

					}
					break;
				case 23 :
					// antlr/Plexil.g:951:5: nodeCommandHandleKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeCommandHandleKywd_in_quantity5492);
					nodeCommandHandleKywd335=nodeCommandHandleKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeCommandHandleKywd335.getTree());

					}
					break;
				case 24 :
					// antlr/Plexil.g:952:5: nodeFailureKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeFailureKywd_in_quantity5498);
					nodeFailureKywd336=nodeFailureKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeFailureKywd336.getTree());

					}
					break;
				case 25 :
					// antlr/Plexil.g:953:5: nodeStateKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeStateKywd_in_quantity5504);
					nodeStateKywd337=nodeStateKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeStateKywd337.getTree());

					}
					break;
				case 26 :
					// antlr/Plexil.g:954:5: nodeOutcomeKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeOutcomeKywd_in_quantity5510);
					nodeOutcomeKywd338=nodeOutcomeKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeOutcomeKywd338.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "quantity"


	public static class oneArgFn_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "oneArgFn"
	// antlr/Plexil.g:958:1: oneArgFn : ( SQRT_KYWD | ABS_KYWD | CEIL_KYWD | FLOOR_KYWD | ROUND_KYWD | TRUNC_KYWD | REAL_TO_INT_KYWD | STRLEN_KYWD | ARRAY_SIZE_KYWD | ARRAY_MAX_SIZE_KYWD );
	public final PlexilParser.oneArgFn_return oneArgFn() throws RecognitionException {
		PlexilParser.oneArgFn_return retval = new PlexilParser.oneArgFn_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set339=null;

		PlexilTreeNode set339_tree=null;

		try {
			// antlr/Plexil.g:958:10: ( SQRT_KYWD | ABS_KYWD | CEIL_KYWD | FLOOR_KYWD | ROUND_KYWD | TRUNC_KYWD | REAL_TO_INT_KYWD | STRLEN_KYWD | ARRAY_SIZE_KYWD | ARRAY_MAX_SIZE_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set339=input.LT(1);
			if ( input.LA(1)==ABS_KYWD||input.LA(1)==ARRAY_MAX_SIZE_KYWD||input.LA(1)==ARRAY_SIZE_KYWD||input.LA(1)==CEIL_KYWD||input.LA(1)==FLOOR_KYWD||input.LA(1)==REAL_TO_INT_KYWD||input.LA(1)==ROUND_KYWD||input.LA(1)==SQRT_KYWD||input.LA(1)==STRLEN_KYWD||input.LA(1)==TRUNC_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set339));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "oneArgFn"


	public static class twoArgFn_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "twoArgFn"
	// antlr/Plexil.g:971:1: twoArgFn : ( MAX_KYWD | MIN_KYWD );
	public final PlexilParser.twoArgFn_return twoArgFn() throws RecognitionException {
		PlexilParser.twoArgFn_return retval = new PlexilParser.twoArgFn_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set340=null;

		PlexilTreeNode set340_tree=null;

		try {
			// antlr/Plexil.g:971:10: ( MAX_KYWD | MIN_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set340=input.LT(1);
			if ( input.LA(1)==MAX_KYWD||input.LA(1)==MIN_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set340));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "twoArgFn"


	public static class isKnownExp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "isKnownExp"
	// antlr/Plexil.g:975:1: isKnownExp : IS_KNOWN_KYWD ^ LPAREN ! quantity RPAREN !;
	public final PlexilParser.isKnownExp_return isKnownExp() throws RecognitionException {
		PlexilParser.isKnownExp_return retval = new PlexilParser.isKnownExp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token IS_KNOWN_KYWD341=null;
		Token LPAREN342=null;
		Token RPAREN344=null;
		ParserRuleReturnScope quantity343 =null;

		PlexilTreeNode IS_KNOWN_KYWD341_tree=null;
		PlexilTreeNode LPAREN342_tree=null;
		PlexilTreeNode RPAREN344_tree=null;

		try {
			// antlr/Plexil.g:975:12: ( IS_KNOWN_KYWD ^ LPAREN ! quantity RPAREN !)
			// antlr/Plexil.g:976:4: IS_KNOWN_KYWD ^ LPAREN ! quantity RPAREN !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			IS_KNOWN_KYWD341=(Token)match(input,IS_KNOWN_KYWD,FOLLOW_IS_KNOWN_KYWD_in_isKnownExp5611); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			IS_KNOWN_KYWD341_tree = new IsKnownNode(IS_KNOWN_KYWD341) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(IS_KNOWN_KYWD341_tree, root_0);
			}

			LPAREN342=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_isKnownExp5617); if (state.failed) return retval;
			pushFollow(FOLLOW_quantity_in_isKnownExp5620);
			quantity343=quantity();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, quantity343.getTree());

			RPAREN344=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_isKnownExp5622); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "isKnownExp"


	public static class nodeStatePredicate_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeStatePredicate"
	// antlr/Plexil.g:978:1: nodeStatePredicate : ( NODE_EXECUTING_KYWD | NODE_FAILED_KYWD | NODE_FINISHED_KYWD | NODE_INACTIVE_KYWD | NODE_INVARIANT_FAILED_KYWD | NODE_ITERATION_ENDED_KYWD | NODE_ITERATION_FAILED_KYWD | NODE_ITERATION_SUCCEEDED_KYWD | NODE_PARENT_FAILED_KYWD | NODE_POSTCONDITION_FAILED_KYWD | NODE_PRECONDITION_FAILED_KYWD | NODE_SKIPPED_KYWD | NODE_SUCCEEDED_KYWD | NODE_WAITING_KYWD );
	public final PlexilParser.nodeStatePredicate_return nodeStatePredicate() throws RecognitionException {
		PlexilParser.nodeStatePredicate_return retval = new PlexilParser.nodeStatePredicate_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set345=null;

		PlexilTreeNode set345_tree=null;

		try {
			// antlr/Plexil.g:978:20: ( NODE_EXECUTING_KYWD | NODE_FAILED_KYWD | NODE_FINISHED_KYWD | NODE_INACTIVE_KYWD | NODE_INVARIANT_FAILED_KYWD | NODE_ITERATION_ENDED_KYWD | NODE_ITERATION_FAILED_KYWD | NODE_ITERATION_SUCCEEDED_KYWD | NODE_PARENT_FAILED_KYWD | NODE_POSTCONDITION_FAILED_KYWD | NODE_PRECONDITION_FAILED_KYWD | NODE_SKIPPED_KYWD | NODE_SUCCEEDED_KYWD | NODE_WAITING_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set345=input.LT(1);
			if ( (input.LA(1) >= NODE_EXECUTING_KYWD && input.LA(1) <= NODE_FINISHED_KYWD)||(input.LA(1) >= NODE_INACTIVE_KYWD && input.LA(1) <= NODE_SUCCEEDED_KYWD)||input.LA(1)==NODE_WAITING_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set345));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeStatePredicate"


	public static class nodeStatePredicateExp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeStatePredicateExp"
	// antlr/Plexil.g:995:1: nodeStatePredicateExp : nodeStatePredicate ^ LPAREN ! nodeReference RPAREN !;
	public final PlexilParser.nodeStatePredicateExp_return nodeStatePredicateExp() throws RecognitionException {
		PlexilParser.nodeStatePredicateExp_return retval = new PlexilParser.nodeStatePredicateExp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LPAREN347=null;
		Token RPAREN349=null;
		ParserRuleReturnScope nodeStatePredicate346 =null;
		ParserRuleReturnScope nodeReference348 =null;

		PlexilTreeNode LPAREN347_tree=null;
		PlexilTreeNode RPAREN349_tree=null;

		try {
			// antlr/Plexil.g:995:23: ( nodeStatePredicate ^ LPAREN ! nodeReference RPAREN !)
			// antlr/Plexil.g:995:25: nodeStatePredicate ^ LPAREN ! nodeReference RPAREN !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_nodeStatePredicate_in_nodeStatePredicateExp5724);
			nodeStatePredicate346=nodeStatePredicate();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) root_0 = (PlexilTreeNode)adaptor.becomeRoot(nodeStatePredicate346.getTree(), root_0);
			LPAREN347=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_nodeStatePredicateExp5727); if (state.failed) return retval;
			pushFollow(FOLLOW_nodeReference_in_nodeStatePredicateExp5730);
			nodeReference348=nodeReference();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeReference348.getTree());

			RPAREN349=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_nodeStatePredicateExp5732); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeStatePredicateExp"


	public static class nodeStateKywd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeStateKywd"
	// antlr/Plexil.g:997:1: nodeStateKywd : ( EXECUTING_STATE_KYWD | FAILING_STATE_KYWD | FINISHED_STATE_KYWD | FINISHING_STATE_KYWD | INACTIVE_STATE_KYWD | ITERATION_ENDED_STATE_KYWD | WAITING_STATE_KYWD );
	public final PlexilParser.nodeStateKywd_return nodeStateKywd() throws RecognitionException {
		PlexilParser.nodeStateKywd_return retval = new PlexilParser.nodeStateKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set350=null;

		PlexilTreeNode set350_tree=null;

		try {
			// antlr/Plexil.g:997:15: ( EXECUTING_STATE_KYWD | FAILING_STATE_KYWD | FINISHED_STATE_KYWD | FINISHING_STATE_KYWD | INACTIVE_STATE_KYWD | ITERATION_ENDED_STATE_KYWD | WAITING_STATE_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set350=input.LT(1);
			if ( input.LA(1)==EXECUTING_STATE_KYWD||input.LA(1)==FAILING_STATE_KYWD||(input.LA(1) >= FINISHED_STATE_KYWD && input.LA(1) <= FINISHING_STATE_KYWD)||input.LA(1)==INACTIVE_STATE_KYWD||input.LA(1)==ITERATION_ENDED_STATE_KYWD||input.LA(1)==WAITING_STATE_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set350));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeStateKywd"


	public static class messageReceivedExp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "messageReceivedExp"
	// antlr/Plexil.g:1007:1: messageReceivedExp : MESSAGE_RECEIVED_KYWD ^ LPAREN ! STRING RPAREN !;
	public final PlexilParser.messageReceivedExp_return messageReceivedExp() throws RecognitionException {
		PlexilParser.messageReceivedExp_return retval = new PlexilParser.messageReceivedExp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token MESSAGE_RECEIVED_KYWD351=null;
		Token LPAREN352=null;
		Token STRING353=null;
		Token RPAREN354=null;

		PlexilTreeNode MESSAGE_RECEIVED_KYWD351_tree=null;
		PlexilTreeNode LPAREN352_tree=null;
		PlexilTreeNode STRING353_tree=null;
		PlexilTreeNode RPAREN354_tree=null;

		try {
			// antlr/Plexil.g:1007:20: ( MESSAGE_RECEIVED_KYWD ^ LPAREN ! STRING RPAREN !)
			// antlr/Plexil.g:1008:3: MESSAGE_RECEIVED_KYWD ^ LPAREN ! STRING RPAREN !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			MESSAGE_RECEIVED_KYWD351=(Token)match(input,MESSAGE_RECEIVED_KYWD,FOLLOW_MESSAGE_RECEIVED_KYWD_in_messageReceivedExp5802); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			MESSAGE_RECEIVED_KYWD351_tree = (PlexilTreeNode)adaptor.create(MESSAGE_RECEIVED_KYWD351);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(MESSAGE_RECEIVED_KYWD351_tree, root_0);
			}

			LPAREN352=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_messageReceivedExp5805); if (state.failed) return retval;
			STRING353=(Token)match(input,STRING,FOLLOW_STRING_in_messageReceivedExp5808); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			STRING353_tree = (PlexilTreeNode)adaptor.create(STRING353);
			adaptor.addChild(root_0, STRING353_tree);
			}

			RPAREN354=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_messageReceivedExp5810); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "messageReceivedExp"


	public static class nodeState_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeState"
	// antlr/Plexil.g:1011:1: nodeState : ( nodeStateVariable | nodeStateKywd );
	public final PlexilParser.nodeState_return nodeState() throws RecognitionException {
		PlexilParser.nodeState_return retval = new PlexilParser.nodeState_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope nodeStateVariable355 =null;
		ParserRuleReturnScope nodeStateKywd356 =null;


		try {
			// antlr/Plexil.g:1011:11: ( nodeStateVariable | nodeStateKywd )
			int alt80=2;
			int LA80_0 = input.LA(1);
			if ( (LA80_0==CHILD_KYWD||LA80_0==NCNAME||LA80_0==PARENT_KYWD||LA80_0==SELF_KYWD||LA80_0==SIBLING_KYWD) ) {
				alt80=1;
			}
			else if ( (LA80_0==EXECUTING_STATE_KYWD||LA80_0==FAILING_STATE_KYWD||(LA80_0 >= FINISHED_STATE_KYWD && LA80_0 <= FINISHING_STATE_KYWD)||LA80_0==INACTIVE_STATE_KYWD||LA80_0==ITERATION_ENDED_STATE_KYWD||LA80_0==WAITING_STATE_KYWD) ) {
				alt80=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 80, 0, input);
				throw nvae;
			}

			switch (alt80) {
				case 1 :
					// antlr/Plexil.g:1011:13: nodeStateVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeStateVariable_in_nodeState5821);
					nodeStateVariable355=nodeStateVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeStateVariable355.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1011:33: nodeStateKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeStateKywd_in_nodeState5825);
					nodeStateKywd356=nodeStateKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeStateKywd356.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeState"


	public static class nodeStateVariable_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeStateVariable"
	// antlr/Plexil.g:1013:1: nodeStateVariable : nodeReference PERIOD ! STATE_KYWD ^;
	public final PlexilParser.nodeStateVariable_return nodeStateVariable() throws RecognitionException {
		PlexilParser.nodeStateVariable_return retval = new PlexilParser.nodeStateVariable_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token PERIOD358=null;
		Token STATE_KYWD359=null;
		ParserRuleReturnScope nodeReference357 =null;

		PlexilTreeNode PERIOD358_tree=null;
		PlexilTreeNode STATE_KYWD359_tree=null;

		try {
			// antlr/Plexil.g:1013:19: ( nodeReference PERIOD ! STATE_KYWD ^)
			// antlr/Plexil.g:1013:21: nodeReference PERIOD ! STATE_KYWD ^
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_nodeReference_in_nodeStateVariable5834);
			nodeReference357=nodeReference();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeReference357.getTree());

			PERIOD358=(Token)match(input,PERIOD,FOLLOW_PERIOD_in_nodeStateVariable5836); if (state.failed) return retval;
			STATE_KYWD359=(Token)match(input,STATE_KYWD,FOLLOW_STATE_KYWD_in_nodeStateVariable5839); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			STATE_KYWD359_tree = new NodeVariableNode(STATE_KYWD359) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(STATE_KYWD359_tree, root_0);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeStateVariable"


	public static class nodeOutcome_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeOutcome"
	// antlr/Plexil.g:1015:1: nodeOutcome : ( nodeOutcomeVariable | nodeOutcomeKywd );
	public final PlexilParser.nodeOutcome_return nodeOutcome() throws RecognitionException {
		PlexilParser.nodeOutcome_return retval = new PlexilParser.nodeOutcome_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope nodeOutcomeVariable360 =null;
		ParserRuleReturnScope nodeOutcomeKywd361 =null;


		try {
			// antlr/Plexil.g:1015:13: ( nodeOutcomeVariable | nodeOutcomeKywd )
			int alt81=2;
			int LA81_0 = input.LA(1);
			if ( (LA81_0==CHILD_KYWD||LA81_0==NCNAME||LA81_0==PARENT_KYWD||LA81_0==SELF_KYWD||LA81_0==SIBLING_KYWD) ) {
				alt81=1;
			}
			else if ( (LA81_0==FAILURE_OUTCOME_KYWD||LA81_0==INTERRUPTED_OUTCOME_KYWD||LA81_0==SKIPPED_OUTCOME_KYWD||LA81_0==SUCCESS_OUTCOME_KYWD) ) {
				alt81=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 81, 0, input);
				throw nvae;
			}

			switch (alt81) {
				case 1 :
					// antlr/Plexil.g:1015:15: nodeOutcomeVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeOutcomeVariable_in_nodeOutcome5852);
					nodeOutcomeVariable360=nodeOutcomeVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeOutcomeVariable360.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1015:37: nodeOutcomeKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeOutcomeKywd_in_nodeOutcome5856);
					nodeOutcomeKywd361=nodeOutcomeKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeOutcomeKywd361.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeOutcome"


	public static class nodeOutcomeVariable_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeOutcomeVariable"
	// antlr/Plexil.g:1017:1: nodeOutcomeVariable : nodeReference PERIOD ! OUTCOME_KYWD ^;
	public final PlexilParser.nodeOutcomeVariable_return nodeOutcomeVariable() throws RecognitionException {
		PlexilParser.nodeOutcomeVariable_return retval = new PlexilParser.nodeOutcomeVariable_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token PERIOD363=null;
		Token OUTCOME_KYWD364=null;
		ParserRuleReturnScope nodeReference362 =null;

		PlexilTreeNode PERIOD363_tree=null;
		PlexilTreeNode OUTCOME_KYWD364_tree=null;

		try {
			// antlr/Plexil.g:1017:21: ( nodeReference PERIOD ! OUTCOME_KYWD ^)
			// antlr/Plexil.g:1017:23: nodeReference PERIOD ! OUTCOME_KYWD ^
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_nodeReference_in_nodeOutcomeVariable5865);
			nodeReference362=nodeReference();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeReference362.getTree());

			PERIOD363=(Token)match(input,PERIOD,FOLLOW_PERIOD_in_nodeOutcomeVariable5867); if (state.failed) return retval;
			OUTCOME_KYWD364=(Token)match(input,OUTCOME_KYWD,FOLLOW_OUTCOME_KYWD_in_nodeOutcomeVariable5870); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			OUTCOME_KYWD364_tree = new NodeVariableNode(OUTCOME_KYWD364) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(OUTCOME_KYWD364_tree, root_0);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeOutcomeVariable"


	public static class nodeOutcomeKywd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeOutcomeKywd"
	// antlr/Plexil.g:1019:1: nodeOutcomeKywd : ( SUCCESS_OUTCOME_KYWD | FAILURE_OUTCOME_KYWD | SKIPPED_OUTCOME_KYWD | INTERRUPTED_OUTCOME_KYWD );
	public final PlexilParser.nodeOutcomeKywd_return nodeOutcomeKywd() throws RecognitionException {
		PlexilParser.nodeOutcomeKywd_return retval = new PlexilParser.nodeOutcomeKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set365=null;

		PlexilTreeNode set365_tree=null;

		try {
			// antlr/Plexil.g:1019:17: ( SUCCESS_OUTCOME_KYWD | FAILURE_OUTCOME_KYWD | SKIPPED_OUTCOME_KYWD | INTERRUPTED_OUTCOME_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set365=input.LT(1);
			if ( input.LA(1)==FAILURE_OUTCOME_KYWD||input.LA(1)==INTERRUPTED_OUTCOME_KYWD||input.LA(1)==SKIPPED_OUTCOME_KYWD||input.LA(1)==SUCCESS_OUTCOME_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set365));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeOutcomeKywd"


	public static class nodeCommandHandle_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeCommandHandle"
	// antlr/Plexil.g:1026:1: nodeCommandHandle : ( nodeCommandHandleVariable | nodeCommandHandleKywd );
	public final PlexilParser.nodeCommandHandle_return nodeCommandHandle() throws RecognitionException {
		PlexilParser.nodeCommandHandle_return retval = new PlexilParser.nodeCommandHandle_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope nodeCommandHandleVariable366 =null;
		ParserRuleReturnScope nodeCommandHandleKywd367 =null;


		try {
			// antlr/Plexil.g:1026:19: ( nodeCommandHandleVariable | nodeCommandHandleKywd )
			int alt82=2;
			int LA82_0 = input.LA(1);
			if ( (LA82_0==CHILD_KYWD||LA82_0==NCNAME||LA82_0==PARENT_KYWD||LA82_0==SELF_KYWD||LA82_0==SIBLING_KYWD) ) {
				alt82=1;
			}
			else if ( ((LA82_0 >= COMMAND_ACCEPTED_KYWD && LA82_0 <= COMMAND_FAILED_KYWD)||(LA82_0 >= COMMAND_RCVD_KYWD && LA82_0 <= COMMAND_SUCCESS_KYWD)) ) {
				alt82=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 82, 0, input);
				throw nvae;
			}

			switch (alt82) {
				case 1 :
					// antlr/Plexil.g:1026:21: nodeCommandHandleVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeCommandHandleVariable_in_nodeCommandHandle5914);
					nodeCommandHandleVariable366=nodeCommandHandleVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeCommandHandleVariable366.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1026:49: nodeCommandHandleKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeCommandHandleKywd_in_nodeCommandHandle5918);
					nodeCommandHandleKywd367=nodeCommandHandleKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeCommandHandleKywd367.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeCommandHandle"


	public static class nodeCommandHandleVariable_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeCommandHandleVariable"
	// antlr/Plexil.g:1028:1: nodeCommandHandleVariable : nodeReference PERIOD ! COMMAND_HANDLE_KYWD ^;
	public final PlexilParser.nodeCommandHandleVariable_return nodeCommandHandleVariable() throws RecognitionException {
		PlexilParser.nodeCommandHandleVariable_return retval = new PlexilParser.nodeCommandHandleVariable_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token PERIOD369=null;
		Token COMMAND_HANDLE_KYWD370=null;
		ParserRuleReturnScope nodeReference368 =null;

		PlexilTreeNode PERIOD369_tree=null;
		PlexilTreeNode COMMAND_HANDLE_KYWD370_tree=null;

		try {
			// antlr/Plexil.g:1028:27: ( nodeReference PERIOD ! COMMAND_HANDLE_KYWD ^)
			// antlr/Plexil.g:1028:29: nodeReference PERIOD ! COMMAND_HANDLE_KYWD ^
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_nodeReference_in_nodeCommandHandleVariable5927);
			nodeReference368=nodeReference();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeReference368.getTree());

			PERIOD369=(Token)match(input,PERIOD,FOLLOW_PERIOD_in_nodeCommandHandleVariable5929); if (state.failed) return retval;
			COMMAND_HANDLE_KYWD370=(Token)match(input,COMMAND_HANDLE_KYWD,FOLLOW_COMMAND_HANDLE_KYWD_in_nodeCommandHandleVariable5932); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			COMMAND_HANDLE_KYWD370_tree = new NodeVariableNode(COMMAND_HANDLE_KYWD370) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(COMMAND_HANDLE_KYWD370_tree, root_0);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeCommandHandleVariable"


	public static class nodeCommandHandleKywd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeCommandHandleKywd"
	// antlr/Plexil.g:1030:1: nodeCommandHandleKywd : ( COMMAND_ACCEPTED_KYWD | COMMAND_DENIED_KYWD | COMMAND_FAILED_KYWD | COMMAND_RCVD_KYWD | COMMAND_SENT_KYWD | COMMAND_SUCCESS_KYWD );
	public final PlexilParser.nodeCommandHandleKywd_return nodeCommandHandleKywd() throws RecognitionException {
		PlexilParser.nodeCommandHandleKywd_return retval = new PlexilParser.nodeCommandHandleKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set371=null;

		PlexilTreeNode set371_tree=null;

		try {
			// antlr/Plexil.g:1030:23: ( COMMAND_ACCEPTED_KYWD | COMMAND_DENIED_KYWD | COMMAND_FAILED_KYWD | COMMAND_RCVD_KYWD | COMMAND_SENT_KYWD | COMMAND_SUCCESS_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set371=input.LT(1);
			if ( (input.LA(1) >= COMMAND_ACCEPTED_KYWD && input.LA(1) <= COMMAND_FAILED_KYWD)||(input.LA(1) >= COMMAND_RCVD_KYWD && input.LA(1) <= COMMAND_SUCCESS_KYWD) ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set371));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeCommandHandleKywd"


	public static class nodeFailure_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeFailure"
	// antlr/Plexil.g:1039:1: nodeFailure : ( nodeFailureVariable | nodeFailureKywd );
	public final PlexilParser.nodeFailure_return nodeFailure() throws RecognitionException {
		PlexilParser.nodeFailure_return retval = new PlexilParser.nodeFailure_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope nodeFailureVariable372 =null;
		ParserRuleReturnScope nodeFailureKywd373 =null;


		try {
			// antlr/Plexil.g:1039:13: ( nodeFailureVariable | nodeFailureKywd )
			int alt83=2;
			int LA83_0 = input.LA(1);
			if ( (LA83_0==CHILD_KYWD||LA83_0==NCNAME||LA83_0==PARENT_KYWD||LA83_0==SELF_KYWD||LA83_0==SIBLING_KYWD) ) {
				alt83=1;
			}
			else if ( (LA83_0==EXITED_KYWD||LA83_0==INVARIANT_CONDITION_FAILED_KYWD||(LA83_0 >= PARENT_EXITED_KYWD && LA83_0 <= PARENT_FAILED_KYWD)||LA83_0==POST_CONDITION_FAILED_KYWD||LA83_0==PRE_CONDITION_FAILED_KYWD) ) {
				alt83=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 83, 0, input);
				throw nvae;
			}

			switch (alt83) {
				case 1 :
					// antlr/Plexil.g:1039:15: nodeFailureVariable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeFailureVariable_in_nodeFailure5989);
					nodeFailureVariable372=nodeFailureVariable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeFailureVariable372.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1039:37: nodeFailureKywd
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeFailureKywd_in_nodeFailure5993);
					nodeFailureKywd373=nodeFailureKywd();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeFailureKywd373.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeFailure"


	public static class nodeFailureVariable_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeFailureVariable"
	// antlr/Plexil.g:1041:1: nodeFailureVariable : nodeReference PERIOD ! FAILURE_KYWD ^;
	public final PlexilParser.nodeFailureVariable_return nodeFailureVariable() throws RecognitionException {
		PlexilParser.nodeFailureVariable_return retval = new PlexilParser.nodeFailureVariable_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token PERIOD375=null;
		Token FAILURE_KYWD376=null;
		ParserRuleReturnScope nodeReference374 =null;

		PlexilTreeNode PERIOD375_tree=null;
		PlexilTreeNode FAILURE_KYWD376_tree=null;

		try {
			// antlr/Plexil.g:1041:21: ( nodeReference PERIOD ! FAILURE_KYWD ^)
			// antlr/Plexil.g:1041:23: nodeReference PERIOD ! FAILURE_KYWD ^
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_nodeReference_in_nodeFailureVariable6002);
			nodeReference374=nodeReference();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeReference374.getTree());

			PERIOD375=(Token)match(input,PERIOD,FOLLOW_PERIOD_in_nodeFailureVariable6004); if (state.failed) return retval;
			FAILURE_KYWD376=(Token)match(input,FAILURE_KYWD,FOLLOW_FAILURE_KYWD_in_nodeFailureVariable6007); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			FAILURE_KYWD376_tree = new NodeVariableNode(FAILURE_KYWD376) ;
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(FAILURE_KYWD376_tree, root_0);
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeFailureVariable"


	public static class nodeFailureKywd_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeFailureKywd"
	// antlr/Plexil.g:1043:1: nodeFailureKywd : ( PRE_CONDITION_FAILED_KYWD | POST_CONDITION_FAILED_KYWD | INVARIANT_CONDITION_FAILED_KYWD | PARENT_FAILED_KYWD | PARENT_EXITED_KYWD | EXITED_KYWD );
	public final PlexilParser.nodeFailureKywd_return nodeFailureKywd() throws RecognitionException {
		PlexilParser.nodeFailureKywd_return retval = new PlexilParser.nodeFailureKywd_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set377=null;

		PlexilTreeNode set377_tree=null;

		try {
			// antlr/Plexil.g:1043:17: ( PRE_CONDITION_FAILED_KYWD | POST_CONDITION_FAILED_KYWD | INVARIANT_CONDITION_FAILED_KYWD | PARENT_FAILED_KYWD | PARENT_EXITED_KYWD | EXITED_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set377=input.LT(1);
			if ( input.LA(1)==EXITED_KYWD||input.LA(1)==INVARIANT_CONDITION_FAILED_KYWD||(input.LA(1) >= PARENT_EXITED_KYWD && input.LA(1) <= PARENT_FAILED_KYWD)||input.LA(1)==POST_CONDITION_FAILED_KYWD||input.LA(1)==PRE_CONDITION_FAILED_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set377));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeFailureKywd"


	public static class nodeTimepointValue_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeTimepointValue"
	// antlr/Plexil.g:1052:1: nodeTimepointValue : nodeReference PERIOD nodeStateKywd PERIOD timepoint -> ^( NODE_TIMEPOINT_VALUE nodeReference nodeStateKywd timepoint ) ;
	public final PlexilParser.nodeTimepointValue_return nodeTimepointValue() throws RecognitionException {
		PlexilParser.nodeTimepointValue_return retval = new PlexilParser.nodeTimepointValue_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token PERIOD379=null;
		Token PERIOD381=null;
		ParserRuleReturnScope nodeReference378 =null;
		ParserRuleReturnScope nodeStateKywd380 =null;
		ParserRuleReturnScope timepoint382 =null;

		PlexilTreeNode PERIOD379_tree=null;
		PlexilTreeNode PERIOD381_tree=null;
		RewriteRuleTokenStream stream_PERIOD=new RewriteRuleTokenStream(adaptor,"token PERIOD");
		RewriteRuleSubtreeStream stream_nodeStateKywd=new RewriteRuleSubtreeStream(adaptor,"rule nodeStateKywd");
		RewriteRuleSubtreeStream stream_nodeReference=new RewriteRuleSubtreeStream(adaptor,"rule nodeReference");
		RewriteRuleSubtreeStream stream_timepoint=new RewriteRuleSubtreeStream(adaptor,"rule timepoint");

		try {
			// antlr/Plexil.g:1052:20: ( nodeReference PERIOD nodeStateKywd PERIOD timepoint -> ^( NODE_TIMEPOINT_VALUE nodeReference nodeStateKywd timepoint ) )
			// antlr/Plexil.g:1053:4: nodeReference PERIOD nodeStateKywd PERIOD timepoint
			{
			pushFollow(FOLLOW_nodeReference_in_nodeTimepointValue6067);
			nodeReference378=nodeReference();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_nodeReference.add(nodeReference378.getTree());
			PERIOD379=(Token)match(input,PERIOD,FOLLOW_PERIOD_in_nodeTimepointValue6069); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_PERIOD.add(PERIOD379);

			pushFollow(FOLLOW_nodeStateKywd_in_nodeTimepointValue6071);
			nodeStateKywd380=nodeStateKywd();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_nodeStateKywd.add(nodeStateKywd380.getTree());
			PERIOD381=(Token)match(input,PERIOD,FOLLOW_PERIOD_in_nodeTimepointValue6073); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_PERIOD.add(PERIOD381);

			pushFollow(FOLLOW_timepoint_in_nodeTimepointValue6075);
			timepoint382=timepoint();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) stream_timepoint.add(timepoint382.getTree());
			// AST REWRITE
			// elements: timepoint, nodeReference, nodeStateKywd
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 1054:4: -> ^( NODE_TIMEPOINT_VALUE nodeReference nodeStateKywd timepoint )
			{
				// antlr/Plexil.g:1054:7: ^( NODE_TIMEPOINT_VALUE nodeReference nodeStateKywd timepoint )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(NODE_TIMEPOINT_VALUE, "NODE_TIMEPOINT_VALUE"), root_1);
				adaptor.addChild(root_1, stream_nodeReference.nextTree());
				adaptor.addChild(root_1, stream_nodeStateKywd.nextTree());
				adaptor.addChild(root_1, stream_timepoint.nextTree());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeTimepointValue"


	public static class timepoint_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "timepoint"
	// antlr/Plexil.g:1057:1: timepoint : ( START_KYWD | END_KYWD );
	public final PlexilParser.timepoint_return timepoint() throws RecognitionException {
		PlexilParser.timepoint_return retval = new PlexilParser.timepoint_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set383=null;

		PlexilTreeNode set383_tree=null;

		try {
			// antlr/Plexil.g:1057:11: ( START_KYWD | END_KYWD )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set383=input.LT(1);
			if ( input.LA(1)==END_KYWD||input.LA(1)==START_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set383));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "timepoint"


	public static class nodeReference_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeReference"
	// antlr/Plexil.g:1059:1: nodeReference : ( nodeId | CHILD_KYWD ^ LPAREN ! NCNAME RPAREN !| SIBLING_KYWD ^ LPAREN ! NCNAME RPAREN !);
	public final PlexilParser.nodeReference_return nodeReference() throws RecognitionException {
		PlexilParser.nodeReference_return retval = new PlexilParser.nodeReference_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token CHILD_KYWD385=null;
		Token LPAREN386=null;
		Token NCNAME387=null;
		Token RPAREN388=null;
		Token SIBLING_KYWD389=null;
		Token LPAREN390=null;
		Token NCNAME391=null;
		Token RPAREN392=null;
		ParserRuleReturnScope nodeId384 =null;

		PlexilTreeNode CHILD_KYWD385_tree=null;
		PlexilTreeNode LPAREN386_tree=null;
		PlexilTreeNode NCNAME387_tree=null;
		PlexilTreeNode RPAREN388_tree=null;
		PlexilTreeNode SIBLING_KYWD389_tree=null;
		PlexilTreeNode LPAREN390_tree=null;
		PlexilTreeNode NCNAME391_tree=null;
		PlexilTreeNode RPAREN392_tree=null;

		try {
			// antlr/Plexil.g:1059:15: ( nodeId | CHILD_KYWD ^ LPAREN ! NCNAME RPAREN !| SIBLING_KYWD ^ LPAREN ! NCNAME RPAREN !)
			int alt84=3;
			switch ( input.LA(1) ) {
			case NCNAME:
			case PARENT_KYWD:
			case SELF_KYWD:
				{
				alt84=1;
				}
				break;
			case CHILD_KYWD:
				{
				alt84=2;
				}
				break;
			case SIBLING_KYWD:
				{
				alt84=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 84, 0, input);
				throw nvae;
			}
			switch (alt84) {
				case 1 :
					// antlr/Plexil.g:1060:5: nodeId
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_nodeId_in_nodeReference6118);
					nodeId384=nodeId();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, nodeId384.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1061:5: CHILD_KYWD ^ LPAREN ! NCNAME RPAREN !
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					CHILD_KYWD385=(Token)match(input,CHILD_KYWD,FOLLOW_CHILD_KYWD_in_nodeReference6124); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					CHILD_KYWD385_tree = (PlexilTreeNode)adaptor.create(CHILD_KYWD385);
					root_0 = (PlexilTreeNode)adaptor.becomeRoot(CHILD_KYWD385_tree, root_0);
					}

					LPAREN386=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_nodeReference6127); if (state.failed) return retval;
					NCNAME387=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_nodeReference6130); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					NCNAME387_tree = (PlexilTreeNode)adaptor.create(NCNAME387);
					adaptor.addChild(root_0, NCNAME387_tree);
					}

					RPAREN388=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_nodeReference6132); if (state.failed) return retval;
					}
					break;
				case 3 :
					// antlr/Plexil.g:1062:5: SIBLING_KYWD ^ LPAREN ! NCNAME RPAREN !
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					SIBLING_KYWD389=(Token)match(input,SIBLING_KYWD,FOLLOW_SIBLING_KYWD_in_nodeReference6139); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					SIBLING_KYWD389_tree = (PlexilTreeNode)adaptor.create(SIBLING_KYWD389);
					root_0 = (PlexilTreeNode)adaptor.becomeRoot(SIBLING_KYWD389_tree, root_0);
					}

					LPAREN390=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_nodeReference6142); if (state.failed) return retval;
					NCNAME391=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_nodeReference6145); if (state.failed) return retval;
					if ( state.backtracking==0 ) {
					NCNAME391_tree = (PlexilTreeNode)adaptor.create(NCNAME391);
					adaptor.addChild(root_0, NCNAME391_tree);
					}

					RPAREN392=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_nodeReference6147); if (state.failed) return retval;
					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeReference"


	public static class nodeId_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "nodeId"
	// antlr/Plexil.g:1065:1: nodeId : ( SELF_KYWD | PARENT_KYWD | NCNAME );
	public final PlexilParser.nodeId_return nodeId() throws RecognitionException {
		PlexilParser.nodeId_return retval = new PlexilParser.nodeId_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token set393=null;

		PlexilTreeNode set393_tree=null;

		try {
			// antlr/Plexil.g:1065:8: ( SELF_KYWD | PARENT_KYWD | NCNAME )
			// antlr/Plexil.g:
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			set393=input.LT(1);
			if ( input.LA(1)==NCNAME||input.LA(1)==PARENT_KYWD||input.LA(1)==SELF_KYWD ) {
				input.consume();
				if ( state.backtracking==0 ) adaptor.addChild(root_0, (PlexilTreeNode)adaptor.create(set393));
				state.errorRecovery=false;
				state.failed=false;
			}
			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				MismatchedSetException mse = new MismatchedSetException(null,input);
				throw mse;
			}
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "nodeId"


	public static class lookupExpr_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookupExpr"
	// antlr/Plexil.g:1075:1: lookupExpr : ( lookupOnChange | lookupNow | lookup );
	public final PlexilParser.lookupExpr_return lookupExpr() throws RecognitionException {
		PlexilParser.lookupExpr_return retval = new PlexilParser.lookupExpr_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope lookupOnChange394 =null;
		ParserRuleReturnScope lookupNow395 =null;
		ParserRuleReturnScope lookup396 =null;


		try {
			// antlr/Plexil.g:1075:12: ( lookupOnChange | lookupNow | lookup )
			int alt85=3;
			switch ( input.LA(1) ) {
			case LOOKUP_ON_CHANGE_KYWD:
				{
				alt85=1;
				}
				break;
			case LOOKUP_NOW_KYWD:
				{
				alt85=2;
				}
				break;
			case LOOKUP_KYWD:
				{
				alt85=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 85, 0, input);
				throw nvae;
			}
			switch (alt85) {
				case 1 :
					// antlr/Plexil.g:1075:14: lookupOnChange
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_lookupOnChange_in_lookupExpr6190);
					lookupOnChange394=lookupOnChange();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupOnChange394.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1075:31: lookupNow
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_lookupNow_in_lookupExpr6194);
					lookupNow395=lookupNow();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupNow395.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:1075:43: lookup
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_lookup_in_lookupExpr6198);
					lookup396=lookup();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, lookup396.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookupExpr"


	public static class lookupOnChange_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookupOnChange"
	// antlr/Plexil.g:1081:1: lookupOnChange : LOOKUP_ON_CHANGE_KYWD ^ LPAREN ! lookupInvocation ( COMMA ! tolerance )? RPAREN !;
	public final PlexilParser.lookupOnChange_return lookupOnChange() throws RecognitionException {
		PlexilParser.lookupOnChange_return retval = new PlexilParser.lookupOnChange_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LOOKUP_ON_CHANGE_KYWD397=null;
		Token LPAREN398=null;
		Token COMMA400=null;
		Token RPAREN402=null;
		ParserRuleReturnScope lookupInvocation399 =null;
		ParserRuleReturnScope tolerance401 =null;

		PlexilTreeNode LOOKUP_ON_CHANGE_KYWD397_tree=null;
		PlexilTreeNode LPAREN398_tree=null;
		PlexilTreeNode COMMA400_tree=null;
		PlexilTreeNode RPAREN402_tree=null;

		 m_paraphrases.push("in \"LookupOnChange\" expression"); 
		try {
			// antlr/Plexil.g:1084:2: ( LOOKUP_ON_CHANGE_KYWD ^ LPAREN ! lookupInvocation ( COMMA ! tolerance )? RPAREN !)
			// antlr/Plexil.g:1085:8: LOOKUP_ON_CHANGE_KYWD ^ LPAREN ! lookupInvocation ( COMMA ! tolerance )? RPAREN !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			LOOKUP_ON_CHANGE_KYWD397=(Token)match(input,LOOKUP_ON_CHANGE_KYWD,FOLLOW_LOOKUP_ON_CHANGE_KYWD_in_lookupOnChange6229); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			LOOKUP_ON_CHANGE_KYWD397_tree = (PlexilTreeNode)adaptor.create(LOOKUP_ON_CHANGE_KYWD397);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(LOOKUP_ON_CHANGE_KYWD397_tree, root_0);
			}

			LPAREN398=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_lookupOnChange6232); if (state.failed) return retval;
			pushFollow(FOLLOW_lookupInvocation_in_lookupOnChange6235);
			lookupInvocation399=lookupInvocation();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupInvocation399.getTree());

			// antlr/Plexil.g:1085:56: ( COMMA ! tolerance )?
			int alt86=2;
			int LA86_0 = input.LA(1);
			if ( (LA86_0==COMMA) ) {
				alt86=1;
			}
			switch (alt86) {
				case 1 :
					// antlr/Plexil.g:1085:57: COMMA ! tolerance
					{
					COMMA400=(Token)match(input,COMMA,FOLLOW_COMMA_in_lookupOnChange6238); if (state.failed) return retval;
					pushFollow(FOLLOW_tolerance_in_lookupOnChange6241);
					tolerance401=tolerance();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, tolerance401.getTree());

					}
					break;

			}

			RPAREN402=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_lookupOnChange6245); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookupOnChange"


	public static class tolerance_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "tolerance"
	// antlr/Plexil.g:1088:1: tolerance : ( realValue | durationLiteral | variable );
	public final PlexilParser.tolerance_return tolerance() throws RecognitionException {
		PlexilParser.tolerance_return retval = new PlexilParser.tolerance_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope realValue403 =null;
		ParserRuleReturnScope durationLiteral404 =null;
		ParserRuleReturnScope variable405 =null;


		try {
			// antlr/Plexil.g:1088:11: ( realValue | durationLiteral | variable )
			int alt87=3;
			switch ( input.LA(1) ) {
			case DOUBLE:
			case INT:
				{
				alt87=1;
				}
				break;
			case DURATION_KYWD:
				{
				alt87=2;
				}
				break;
			case NCNAME:
				{
				alt87=3;
				}
				break;
			default:
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 87, 0, input);
				throw nvae;
			}
			switch (alt87) {
				case 1 :
					// antlr/Plexil.g:1088:13: realValue
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_realValue_in_tolerance6255);
					realValue403=realValue();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, realValue403.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1088:25: durationLiteral
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_durationLiteral_in_tolerance6259);
					durationLiteral404=durationLiteral();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, durationLiteral404.getTree());

					}
					break;
				case 3 :
					// antlr/Plexil.g:1088:43: variable
					{
					root_0 = (PlexilTreeNode)adaptor.nil();


					pushFollow(FOLLOW_variable_in_tolerance6263);
					variable405=variable();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, variable405.getTree());

					}
					break;

			}
			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "tolerance"


	public static class lookupNow_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookupNow"
	// antlr/Plexil.g:1093:1: lookupNow : LOOKUP_NOW_KYWD ^ LPAREN ! lookupInvocation RPAREN !;
	public final PlexilParser.lookupNow_return lookupNow() throws RecognitionException {
		PlexilParser.lookupNow_return retval = new PlexilParser.lookupNow_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LOOKUP_NOW_KYWD406=null;
		Token LPAREN407=null;
		Token RPAREN409=null;
		ParserRuleReturnScope lookupInvocation408 =null;

		PlexilTreeNode LOOKUP_NOW_KYWD406_tree=null;
		PlexilTreeNode LPAREN407_tree=null;
		PlexilTreeNode RPAREN409_tree=null;

		 m_paraphrases.push("in \"LookupNow\" expression"); 
		try {
			// antlr/Plexil.g:1096:2: ( LOOKUP_NOW_KYWD ^ LPAREN ! lookupInvocation RPAREN !)
			// antlr/Plexil.g:1097:5: LOOKUP_NOW_KYWD ^ LPAREN ! lookupInvocation RPAREN !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			LOOKUP_NOW_KYWD406=(Token)match(input,LOOKUP_NOW_KYWD,FOLLOW_LOOKUP_NOW_KYWD_in_lookupNow6290); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			LOOKUP_NOW_KYWD406_tree = (PlexilTreeNode)adaptor.create(LOOKUP_NOW_KYWD406);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(LOOKUP_NOW_KYWD406_tree, root_0);
			}

			LPAREN407=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_lookupNow6293); if (state.failed) return retval;
			pushFollow(FOLLOW_lookupInvocation_in_lookupNow6296);
			lookupInvocation408=lookupInvocation();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupInvocation408.getTree());

			RPAREN409=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_lookupNow6298); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookupNow"


	public static class lookup_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookup"
	// antlr/Plexil.g:1104:1: lookup : LOOKUP_KYWD ^ LPAREN ! lookupInvocation ( COMMA ! tolerance )? RPAREN !;
	public final PlexilParser.lookup_return lookup() throws RecognitionException {
		PlexilParser.lookup_return retval = new PlexilParser.lookup_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LOOKUP_KYWD410=null;
		Token LPAREN411=null;
		Token COMMA413=null;
		Token RPAREN415=null;
		ParserRuleReturnScope lookupInvocation412 =null;
		ParserRuleReturnScope tolerance414 =null;

		PlexilTreeNode LOOKUP_KYWD410_tree=null;
		PlexilTreeNode LPAREN411_tree=null;
		PlexilTreeNode COMMA413_tree=null;
		PlexilTreeNode RPAREN415_tree=null;

		 m_paraphrases.push("in \"Lookup\" expression"); 
		try {
			// antlr/Plexil.g:1107:2: ( LOOKUP_KYWD ^ LPAREN ! lookupInvocation ( COMMA ! tolerance )? RPAREN !)
			// antlr/Plexil.g:1108:5: LOOKUP_KYWD ^ LPAREN ! lookupInvocation ( COMMA ! tolerance )? RPAREN !
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			LOOKUP_KYWD410=(Token)match(input,LOOKUP_KYWD,FOLLOW_LOOKUP_KYWD_in_lookup6328); if (state.failed) return retval;
			if ( state.backtracking==0 ) {
			LOOKUP_KYWD410_tree = (PlexilTreeNode)adaptor.create(LOOKUP_KYWD410);
			root_0 = (PlexilTreeNode)adaptor.becomeRoot(LOOKUP_KYWD410_tree, root_0);
			}

			LPAREN411=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_lookup6331); if (state.failed) return retval;
			pushFollow(FOLLOW_lookupInvocation_in_lookup6334);
			lookupInvocation412=lookupInvocation();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, lookupInvocation412.getTree());

			// antlr/Plexil.g:1108:43: ( COMMA ! tolerance )?
			int alt88=2;
			int LA88_0 = input.LA(1);
			if ( (LA88_0==COMMA) ) {
				alt88=1;
			}
			switch (alt88) {
				case 1 :
					// antlr/Plexil.g:1108:44: COMMA ! tolerance
					{
					COMMA413=(Token)match(input,COMMA,FOLLOW_COMMA_in_lookup6337); if (state.failed) return retval;
					pushFollow(FOLLOW_tolerance_in_lookup6340);
					tolerance414=tolerance();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, tolerance414.getTree());

					}
					break;

			}

			RPAREN415=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_lookup6344); if (state.failed) return retval;
			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
			if ( state.backtracking==0 ) { m_paraphrases.pop(); }
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookup"


	public static class lookupInvocation_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "lookupInvocation"
	// antlr/Plexil.g:1111:1: lookupInvocation : ( stateName | ( LPAREN ! stateNameExp RPAREN !) ) ( LPAREN ! ( argumentList )? RPAREN !)? ;
	public final PlexilParser.lookupInvocation_return lookupInvocation() throws RecognitionException {
		PlexilParser.lookupInvocation_return retval = new PlexilParser.lookupInvocation_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token LPAREN417=null;
		Token RPAREN419=null;
		Token LPAREN420=null;
		Token RPAREN422=null;
		ParserRuleReturnScope stateName416 =null;
		ParserRuleReturnScope stateNameExp418 =null;
		ParserRuleReturnScope argumentList421 =null;

		PlexilTreeNode LPAREN417_tree=null;
		PlexilTreeNode RPAREN419_tree=null;
		PlexilTreeNode LPAREN420_tree=null;
		PlexilTreeNode RPAREN422_tree=null;

		try {
			// antlr/Plexil.g:1111:18: ( ( stateName | ( LPAREN ! stateNameExp RPAREN !) ) ( LPAREN ! ( argumentList )? RPAREN !)? )
			// antlr/Plexil.g:1112:3: ( stateName | ( LPAREN ! stateNameExp RPAREN !) ) ( LPAREN ! ( argumentList )? RPAREN !)?
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			// antlr/Plexil.g:1112:3: ( stateName | ( LPAREN ! stateNameExp RPAREN !) )
			int alt89=2;
			int LA89_0 = input.LA(1);
			if ( (LA89_0==NCNAME) ) {
				alt89=1;
			}
			else if ( (LA89_0==LPAREN) ) {
				alt89=2;
			}

			else {
				if (state.backtracking>0) {state.failed=true; return retval;}
				NoViableAltException nvae =
					new NoViableAltException("", 89, 0, input);
				throw nvae;
			}

			switch (alt89) {
				case 1 :
					// antlr/Plexil.g:1112:5: stateName
					{
					pushFollow(FOLLOW_stateName_in_lookupInvocation6360);
					stateName416=stateName();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, stateName416.getTree());

					}
					break;
				case 2 :
					// antlr/Plexil.g:1114:5: ( LPAREN ! stateNameExp RPAREN !)
					{
					// antlr/Plexil.g:1114:5: ( LPAREN ! stateNameExp RPAREN !)
					// antlr/Plexil.g:1114:7: LPAREN ! stateNameExp RPAREN !
					{
					LPAREN417=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_lookupInvocation6374); if (state.failed) return retval;
					pushFollow(FOLLOW_stateNameExp_in_lookupInvocation6377);
					stateNameExp418=stateNameExp();
					state._fsp--;
					if (state.failed) return retval;
					if ( state.backtracking==0 ) adaptor.addChild(root_0, stateNameExp418.getTree());

					RPAREN419=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_lookupInvocation6379); if (state.failed) return retval;
					}

					}
					break;

			}

			// antlr/Plexil.g:1116:3: ( LPAREN ! ( argumentList )? RPAREN !)?
			int alt91=2;
			int LA91_0 = input.LA(1);
			if ( (LA91_0==LPAREN) ) {
				alt91=1;
			}
			switch (alt91) {
				case 1 :
					// antlr/Plexil.g:1116:5: LPAREN ! ( argumentList )? RPAREN !
					{
					LPAREN420=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_lookupInvocation6392); if (state.failed) return retval;
					// antlr/Plexil.g:1116:13: ( argumentList )?
					int alt90=2;
					int LA90_0 = input.LA(1);
					if ( (LA90_0==ABS_KYWD||LA90_0==ARRAY_MAX_SIZE_KYWD||LA90_0==ARRAY_SIZE_KYWD||LA90_0==BAR||(LA90_0 >= CEIL_KYWD && LA90_0 <= CHILD_KYWD)||(LA90_0 >= COMMAND_ACCEPTED_KYWD && LA90_0 <= COMMAND_FAILED_KYWD)||(LA90_0 >= COMMAND_RCVD_KYWD && LA90_0 <= COMMAND_SUCCESS_KYWD)||LA90_0==DATE_KYWD||(LA90_0 >= DOUBLE && LA90_0 <= DURATION_KYWD)||(LA90_0 >= EXECUTING_STATE_KYWD && LA90_0 <= EXITED_KYWD)||LA90_0==FAILING_STATE_KYWD||(LA90_0 >= FAILURE_OUTCOME_KYWD && LA90_0 <= FLOOR_KYWD)||LA90_0==HASHPAREN||(LA90_0 >= INACTIVE_STATE_KYWD && LA90_0 <= INT)||LA90_0==INTERRUPTED_OUTCOME_KYWD||LA90_0==INVARIANT_CONDITION_FAILED_KYWD||(LA90_0 >= IS_KNOWN_KYWD && LA90_0 <= ITERATION_ENDED_STATE_KYWD)||(LA90_0 >= LOOKUP_KYWD && LA90_0 <= LOOKUP_ON_CHANGE_KYWD)||LA90_0==LPAREN||(LA90_0 >= MAX_KYWD && LA90_0 <= MIN_KYWD)||LA90_0==NCNAME||(LA90_0 >= NODE_EXECUTING_KYWD && LA90_0 <= NODE_FINISHED_KYWD)||(LA90_0 >= NODE_INACTIVE_KYWD && LA90_0 <= NODE_SUCCEEDED_KYWD)||(LA90_0 >= NODE_WAITING_KYWD && LA90_0 <= NOT_KYWD)||(LA90_0 >= PARENT_EXITED_KYWD && LA90_0 <= PARENT_KYWD)||LA90_0==POST_CONDITION_FAILED_KYWD||LA90_0==PRE_CONDITION_FAILED_KYWD||LA90_0==REAL_TO_INT_KYWD||LA90_0==ROUND_KYWD||LA90_0==SELF_KYWD||(LA90_0 >= SIBLING_KYWD && LA90_0 <= SKIPPED_OUTCOME_KYWD)||LA90_0==SQRT_KYWD||LA90_0==STRING||(LA90_0 >= STRLEN_KYWD && LA90_0 <= SUCCESS_OUTCOME_KYWD)||(LA90_0 >= TRUE_KYWD && LA90_0 <= TRUNC_KYWD)||LA90_0==WAITING_STATE_KYWD) ) {
						alt90=1;
					}
					switch (alt90) {
						case 1 :
							// antlr/Plexil.g:1116:14: argumentList
							{
							pushFollow(FOLLOW_argumentList_in_lookupInvocation6396);
							argumentList421=argumentList();
							state._fsp--;
							if (state.failed) return retval;
							if ( state.backtracking==0 ) adaptor.addChild(root_0, argumentList421.getTree());

							}
							break;

					}

					RPAREN422=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_lookupInvocation6400); if (state.failed) return retval;
					}
					break;

			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "lookupInvocation"


	public static class stateName_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "stateName"
	// antlr/Plexil.g:1119:1: stateName : NCNAME -> ^( STATE_NAME NCNAME ) ;
	public final PlexilParser.stateName_return stateName() throws RecognitionException {
		PlexilParser.stateName_return retval = new PlexilParser.stateName_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		Token NCNAME423=null;

		PlexilTreeNode NCNAME423_tree=null;
		RewriteRuleTokenStream stream_NCNAME=new RewriteRuleTokenStream(adaptor,"token NCNAME");

		try {
			// antlr/Plexil.g:1119:11: ( NCNAME -> ^( STATE_NAME NCNAME ) )
			// antlr/Plexil.g:1120:5: NCNAME
			{
			NCNAME423=(Token)match(input,NCNAME,FOLLOW_NCNAME_in_stateName6418); if (state.failed) return retval; 
			if ( state.backtracking==0 ) stream_NCNAME.add(NCNAME423);

			// AST REWRITE
			// elements: NCNAME
			// token labels: 
			// rule labels: retval
			// token list labels: 
			// rule list labels: 
			// wildcard labels: 
			if ( state.backtracking==0 ) {
			retval.tree = root_0;
			RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.getTree():null);

			root_0 = (PlexilTreeNode)adaptor.nil();
			// 1120:12: -> ^( STATE_NAME NCNAME )
			{
				// antlr/Plexil.g:1120:15: ^( STATE_NAME NCNAME )
				{
				PlexilTreeNode root_1 = (PlexilTreeNode)adaptor.nil();
				root_1 = (PlexilTreeNode)adaptor.becomeRoot((PlexilTreeNode)adaptor.create(STATE_NAME, "STATE_NAME"), root_1);
				adaptor.addChild(root_1, stream_NCNAME.nextNode());
				adaptor.addChild(root_0, root_1);
				}

			}


			retval.tree = root_0;
			}

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "stateName"


	public static class stateNameExp_return extends ParserRuleReturnScope {
		PlexilTreeNode tree;
		@Override
		public PlexilTreeNode getTree() { return tree; }
	};


	// $ANTLR start "stateNameExp"
	// antlr/Plexil.g:1123:1: stateNameExp : expression ;
	public final PlexilParser.stateNameExp_return stateNameExp() throws RecognitionException {
		PlexilParser.stateNameExp_return retval = new PlexilParser.stateNameExp_return();
		retval.start = input.LT(1);

		PlexilTreeNode root_0 = null;

		ParserRuleReturnScope expression424 =null;


		try {
			// antlr/Plexil.g:1123:14: ( expression )
			// antlr/Plexil.g:1123:16: expression
			{
			root_0 = (PlexilTreeNode)adaptor.nil();


			pushFollow(FOLLOW_expression_in_stateNameExp6436);
			expression424=expression();
			state._fsp--;
			if (state.failed) return retval;
			if ( state.backtracking==0 ) adaptor.addChild(root_0, expression424.getTree());

			}

			retval.stop = input.LT(-1);

			if ( state.backtracking==0 ) {
			retval.tree = (PlexilTreeNode)adaptor.rulePostProcessing(root_0);
			adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
			}
		}
		catch (RecognitionException re) {
			reportError(re);
			recover(input,re);
			retval.tree = (PlexilTreeNode)adaptor.errorNode(input, retval.start, input.LT(-1), re);
		}
		finally {
			// do for sure before leaving
		}
		return retval;
	}
	// $ANTLR end "stateNameExp"

	// $ANTLR start synpred1_Plexil
	public final void synpred1_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:371:5: ( baseTypeName LBRACKET )
		// antlr/Plexil.g:371:6: baseTypeName LBRACKET
		{
		pushFollow(FOLLOW_baseTypeName_in_synpred1_Plexil1657);
		baseTypeName();
		state._fsp--;
		if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred1_Plexil1659); if (state.failed) return;

		}

	}
	// $ANTLR end synpred1_Plexil

	// $ANTLR start synpred2_Plexil
	public final void synpred2_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:373:5: ( baseTypeName NCNAME LBRACKET )
		// antlr/Plexil.g:373:6: baseTypeName NCNAME LBRACKET
		{
		pushFollow(FOLLOW_baseTypeName_in_synpred2_Plexil1693);
		baseTypeName();
		state._fsp--;
		if (state.failed) return;

		match(input,NCNAME,FOLLOW_NCNAME_in_synpred2_Plexil1695); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred2_Plexil1697); if (state.failed) return;

		}

	}
	// $ANTLR end synpred2_Plexil

	// $ANTLR start synpred3_Plexil
	public final void synpred3_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:393:5: ( baseTypeName LBRACKET )
		// antlr/Plexil.g:393:6: baseTypeName LBRACKET
		{
		pushFollow(FOLLOW_baseTypeName_in_synpred3_Plexil1840);
		baseTypeName();
		state._fsp--;
		if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred3_Plexil1842); if (state.failed) return;

		}

	}
	// $ANTLR end synpred3_Plexil

	// $ANTLR start synpred4_Plexil
	public final void synpred4_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:425:4: ( NCNAME LBRACKET )
		// antlr/Plexil.g:425:5: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred4_Plexil2039); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred4_Plexil2041); if (state.failed) return;

		}

	}
	// $ANTLR end synpred4_Plexil

	// $ANTLR start synpred5_Plexil
	public final void synpred5_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:449:5: ( NCNAME ( LBRACKET | EQUALS ) )
		// antlr/Plexil.g:449:6: NCNAME ( LBRACKET | EQUALS )
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred5_Plexil2180); if (state.failed) return;

		if ( input.LA(1)==EQUALS||input.LA(1)==LBRACKET ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		}

	}
	// $ANTLR end synpred5_Plexil

	// $ANTLR start synpred6_Plexil
	public final void synpred6_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:450:5: ( ( NCNAME LPAREN ) | LPAREN )
		int alt92=2;
		int LA92_0 = input.LA(1);
		if ( (LA92_0==NCNAME) ) {
			alt92=1;
		}
		else if ( (LA92_0==LPAREN) ) {
			alt92=2;
		}

		else {
			if (state.backtracking>0) {state.failed=true; return;}
			NoViableAltException nvae =
				new NoViableAltException("", 92, 0, input);
			throw nvae;
		}

		switch (alt92) {
			case 1 :
				// antlr/Plexil.g:450:6: ( NCNAME LPAREN )
				{
				// antlr/Plexil.g:450:6: ( NCNAME LPAREN )
				// antlr/Plexil.g:450:7: NCNAME LPAREN
				{
				match(input,NCNAME,FOLLOW_NCNAME_in_synpred6_Plexil2201); if (state.failed) return;

				match(input,LPAREN,FOLLOW_LPAREN_in_synpred6_Plexil2203); if (state.failed) return;

				}

				}
				break;
			case 2 :
				// antlr/Plexil.g:450:24: LPAREN
				{
				match(input,LPAREN,FOLLOW_LPAREN_in_synpred6_Plexil2208); if (state.failed) return;

				}
				break;

		}
	}
	// $ANTLR end synpred6_Plexil

	// $ANTLR start synpred7_Plexil
	public final void synpred7_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:622:7: ( NCNAME LBRACKET )
		// antlr/Plexil.g:622:8: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred7_Plexil3271); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred7_Plexil3273); if (state.failed) return;

		}

	}
	// $ANTLR end synpred7_Plexil

	// $ANTLR start synpred8_Plexil
	public final void synpred8_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:626:9: ( NCNAME LBRACKET )
		// antlr/Plexil.g:626:10: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred8_Plexil3312); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred8_Plexil3314); if (state.failed) return;

		}

	}
	// $ANTLR end synpred8_Plexil

	// $ANTLR start synpred9_Plexil
	public final void synpred9_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:639:7: ( NCNAME LBRACKET )
		// antlr/Plexil.g:639:8: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred9_Plexil3394); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred9_Plexil3396); if (state.failed) return;

		}

	}
	// $ANTLR end synpred9_Plexil

	// $ANTLR start synpred10_Plexil
	public final void synpred10_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:643:6: ( NCNAME LBRACKET )
		// antlr/Plexil.g:643:7: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred10_Plexil3435); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred10_Plexil3437); if (state.failed) return;

		}

	}
	// $ANTLR end synpred10_Plexil

	// $ANTLR start synpred11_Plexil
	public final void synpred11_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:724:5: ( NCNAME LBRACKET )
		// antlr/Plexil.g:724:7: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred11_Plexil3999); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred11_Plexil4001); if (state.failed) return;

		}

	}
	// $ANTLR end synpred11_Plexil

	// $ANTLR start synpred12_Plexil
	public final void synpred12_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:732:4: ( NCNAME LPAREN )
		// antlr/Plexil.g:732:5: NCNAME LPAREN
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred12_Plexil4036); if (state.failed) return;

		match(input,LPAREN,FOLLOW_LPAREN_in_synpred12_Plexil4038); if (state.failed) return;

		}

	}
	// $ANTLR end synpred12_Plexil

	// $ANTLR start synpred13_Plexil
	public final void synpred13_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:734:4: ( LPAREN expression RPAREN LPAREN )
		// antlr/Plexil.g:734:5: LPAREN expression RPAREN LPAREN
		{
		match(input,LPAREN,FOLLOW_LPAREN_in_synpred13_Plexil4052); if (state.failed) return;

		pushFollow(FOLLOW_expression_in_synpred13_Plexil4054);
		expression();
		state._fsp--;
		if (state.failed) return;

		match(input,RPAREN,FOLLOW_RPAREN_in_synpred13_Plexil4056); if (state.failed) return;

		match(input,LPAREN,FOLLOW_LPAREN_in_synpred13_Plexil4058); if (state.failed) return;

		}

	}
	// $ANTLR end synpred13_Plexil

	// $ANTLR start synpred14_Plexil
	public final void synpred14_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:933:5: ( lookupExpr LBRACKET )
		// antlr/Plexil.g:933:6: lookupExpr LBRACKET
		{
		pushFollow(FOLLOW_lookupExpr_in_synpred14_Plexil5154);
		lookupExpr();
		state._fsp--;
		if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred14_Plexil5156); if (state.failed) return;

		}

	}
	// $ANTLR end synpred14_Plexil

	// $ANTLR start synpred15_Plexil
	public final void synpred15_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:937:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD COMMAND_HANDLE_KYWD )
		// antlr/Plexil.g:937:7: ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD COMMAND_HANDLE_KYWD
		{
		if ( input.LA(1)==NCNAME||input.LA(1)==PARENT_KYWD||input.LA(1)==SELF_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,PERIOD,FOLLOW_PERIOD_in_synpred15_Plexil5199); if (state.failed) return;

		match(input,COMMAND_HANDLE_KYWD,FOLLOW_COMMAND_HANDLE_KYWD_in_synpred15_Plexil5201); if (state.failed) return;

		}

	}
	// $ANTLR end synpred15_Plexil

	// $ANTLR start synpred16_Plexil
	public final void synpred16_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:938:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD FAILURE_KYWD )
		// antlr/Plexil.g:938:7: ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD FAILURE_KYWD
		{
		if ( input.LA(1)==NCNAME||input.LA(1)==PARENT_KYWD||input.LA(1)==SELF_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,PERIOD,FOLLOW_PERIOD_in_synpred16_Plexil5226); if (state.failed) return;

		match(input,FAILURE_KYWD,FOLLOW_FAILURE_KYWD_in_synpred16_Plexil5228); if (state.failed) return;

		}

	}
	// $ANTLR end synpred16_Plexil

	// $ANTLR start synpred17_Plexil
	public final void synpred17_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:939:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD OUTCOME_KYWD )
		// antlr/Plexil.g:939:7: ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD OUTCOME_KYWD
		{
		if ( input.LA(1)==NCNAME||input.LA(1)==PARENT_KYWD||input.LA(1)==SELF_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,PERIOD,FOLLOW_PERIOD_in_synpred17_Plexil5253); if (state.failed) return;

		match(input,OUTCOME_KYWD,FOLLOW_OUTCOME_KYWD_in_synpred17_Plexil5255); if (state.failed) return;

		}

	}
	// $ANTLR end synpred17_Plexil

	// $ANTLR start synpred18_Plexil
	public final void synpred18_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:940:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD STATE_KYWD )
		// antlr/Plexil.g:940:7: ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD STATE_KYWD
		{
		if ( input.LA(1)==NCNAME||input.LA(1)==PARENT_KYWD||input.LA(1)==SELF_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,PERIOD,FOLLOW_PERIOD_in_synpred18_Plexil5280); if (state.failed) return;

		match(input,STATE_KYWD,FOLLOW_STATE_KYWD_in_synpred18_Plexil5282); if (state.failed) return;

		}

	}
	// $ANTLR end synpred18_Plexil

	// $ANTLR start synpred19_Plexil
	public final void synpred19_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:941:5: ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD nodeStateKywd )
		// antlr/Plexil.g:941:7: ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD nodeStateKywd
		{
		if ( input.LA(1)==NCNAME||input.LA(1)==PARENT_KYWD||input.LA(1)==SELF_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,PERIOD,FOLLOW_PERIOD_in_synpred19_Plexil5307); if (state.failed) return;

		pushFollow(FOLLOW_nodeStateKywd_in_synpred19_Plexil5309);
		nodeStateKywd();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred19_Plexil

	// $ANTLR start synpred20_Plexil
	public final void synpred20_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:942:5: ( NCNAME LBRACKET )
		// antlr/Plexil.g:942:6: NCNAME LBRACKET
		{
		match(input,NCNAME,FOLLOW_NCNAME_in_synpred20_Plexil5321); if (state.failed) return;

		match(input,LBRACKET,FOLLOW_LBRACKET_in_synpred20_Plexil5323); if (state.failed) return;

		}

	}
	// $ANTLR end synpred20_Plexil

	// $ANTLR start synpred21_Plexil
	public final void synpred21_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:945:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD )
		// antlr/Plexil.g:945:7: ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD
		{
		if ( input.LA(1)==CHILD_KYWD||input.LA(1)==SIBLING_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,LPAREN,FOLLOW_LPAREN_in_synpred21_Plexil5351); if (state.failed) return;

		match(input,NCNAME,FOLLOW_NCNAME_in_synpred21_Plexil5353); if (state.failed) return;

		match(input,RPAREN,FOLLOW_RPAREN_in_synpred21_Plexil5355); if (state.failed) return;

		match(input,PERIOD,FOLLOW_PERIOD_in_synpred21_Plexil5357); if (state.failed) return;

		match(input,COMMAND_HANDLE_KYWD,FOLLOW_COMMAND_HANDLE_KYWD_in_synpred21_Plexil5359); if (state.failed) return;

		}

	}
	// $ANTLR end synpred21_Plexil

	// $ANTLR start synpred22_Plexil
	public final void synpred22_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:946:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD )
		// antlr/Plexil.g:946:7: ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD
		{
		if ( input.LA(1)==CHILD_KYWD||input.LA(1)==SIBLING_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,LPAREN,FOLLOW_LPAREN_in_synpred22_Plexil5380); if (state.failed) return;

		match(input,NCNAME,FOLLOW_NCNAME_in_synpred22_Plexil5382); if (state.failed) return;

		match(input,RPAREN,FOLLOW_RPAREN_in_synpred22_Plexil5384); if (state.failed) return;

		match(input,PERIOD,FOLLOW_PERIOD_in_synpred22_Plexil5386); if (state.failed) return;

		match(input,FAILURE_KYWD,FOLLOW_FAILURE_KYWD_in_synpred22_Plexil5388); if (state.failed) return;

		}

	}
	// $ANTLR end synpred22_Plexil

	// $ANTLR start synpred23_Plexil
	public final void synpred23_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:947:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD )
		// antlr/Plexil.g:947:7: ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD
		{
		if ( input.LA(1)==CHILD_KYWD||input.LA(1)==SIBLING_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,LPAREN,FOLLOW_LPAREN_in_synpred23_Plexil5409); if (state.failed) return;

		match(input,NCNAME,FOLLOW_NCNAME_in_synpred23_Plexil5411); if (state.failed) return;

		match(input,RPAREN,FOLLOW_RPAREN_in_synpred23_Plexil5413); if (state.failed) return;

		match(input,PERIOD,FOLLOW_PERIOD_in_synpred23_Plexil5415); if (state.failed) return;

		match(input,OUTCOME_KYWD,FOLLOW_OUTCOME_KYWD_in_synpred23_Plexil5417); if (state.failed) return;

		}

	}
	// $ANTLR end synpred23_Plexil

	// $ANTLR start synpred24_Plexil
	public final void synpred24_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:948:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD STATE_KYWD )
		// antlr/Plexil.g:948:7: ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD STATE_KYWD
		{
		if ( input.LA(1)==CHILD_KYWD||input.LA(1)==SIBLING_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,LPAREN,FOLLOW_LPAREN_in_synpred24_Plexil5438); if (state.failed) return;

		match(input,NCNAME,FOLLOW_NCNAME_in_synpred24_Plexil5440); if (state.failed) return;

		match(input,RPAREN,FOLLOW_RPAREN_in_synpred24_Plexil5442); if (state.failed) return;

		match(input,PERIOD,FOLLOW_PERIOD_in_synpred24_Plexil5444); if (state.failed) return;

		match(input,STATE_KYWD,FOLLOW_STATE_KYWD_in_synpred24_Plexil5446); if (state.failed) return;

		}

	}
	// $ANTLR end synpred24_Plexil

	// $ANTLR start synpred25_Plexil
	public final void synpred25_Plexil_fragment() throws RecognitionException {
		// antlr/Plexil.g:949:5: ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD nodeStateKywd )
		// antlr/Plexil.g:949:7: ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD nodeStateKywd
		{
		if ( input.LA(1)==CHILD_KYWD||input.LA(1)==SIBLING_KYWD ) {
			input.consume();
			state.errorRecovery=false;
			state.failed=false;
		}
		else {
			if (state.backtracking>0) {state.failed=true; return;}
			MismatchedSetException mse = new MismatchedSetException(null,input);
			throw mse;
		}
		match(input,LPAREN,FOLLOW_LPAREN_in_synpred25_Plexil5467); if (state.failed) return;

		match(input,NCNAME,FOLLOW_NCNAME_in_synpred25_Plexil5469); if (state.failed) return;

		match(input,RPAREN,FOLLOW_RPAREN_in_synpred25_Plexil5471); if (state.failed) return;

		match(input,PERIOD,FOLLOW_PERIOD_in_synpred25_Plexil5473); if (state.failed) return;

		pushFollow(FOLLOW_nodeStateKywd_in_synpred25_Plexil5475);
		nodeStateKywd();
		state._fsp--;
		if (state.failed) return;

		}

	}
	// $ANTLR end synpred25_Plexil

	// Delegated rules

	public final boolean synpred10_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred10_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred11_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred11_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred21_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred21_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred22_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred22_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred20_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred20_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred14_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred14_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred13_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred13_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred15_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred15_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred1_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred1_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred12_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred12_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred16_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred16_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred2_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred2_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred9_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred9_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred3_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred3_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred4_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred4_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred5_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred5_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred6_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred6_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred7_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred7_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred8_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred8_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred17_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred17_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred18_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred18_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred19_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred19_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred25_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred25_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred23_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred23_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}
	public final boolean synpred24_Plexil() {
		state.backtracking++;
		int start = input.mark();
		try {
			synpred24_Plexil_fragment(); // can never throw exception
		} catch (RecognitionException re) {
			System.err.println("impossible: "+re);
		}
		boolean success = !state.failed;
		input.rewind(start);
		state.backtracking--;
		state.failed=false;
		return success;
	}


	protected DFA62 dfa62 = new DFA62(this);
	protected DFA79 dfa79 = new DFA79(this);
	static final String DFA62_eotS =
		"\104\uffff";
	static final String DFA62_eofS =
		"\104\uffff";
	static final String DFA62_minS =
		"\1\4\1\10\1\4\31\uffff\1\0\12\uffff\33\0\2\uffff";
	static final String DFA62_maxS =
		"\1\u00b7\1\u00bb\1\u00b7\31\uffff\1\0\12\uffff\33\0\2\uffff";
	static final String DFA62_acceptS =
		"\3\uffff\1\3\76\uffff\1\1\1\2";
	static final String DFA62_specialS =
		"\34\uffff\1\0\12\uffff\1\1\1\2\1\3\1\4\1\5\1\6\1\7\1\10\1\11\1\12\1\13"+
		"\1\14\1\15\1\16\1\17\1\20\1\21\1\22\1\23\1\24\1\25\1\26\1\27\1\30\1\31"+
		"\1\32\1\33\2\uffff}>";
	static final String[] DFA62_transitionS = {
			"\1\3\7\uffff\1\3\1\uffff\1\3\4\uffff\1\3\2\uffff\2\3\3\uffff\3\3\2\uffff"+
			"\3\3\3\uffff\1\3\2\uffff\2\3\11\uffff\2\3\3\uffff\1\3\1\uffff\5\3\4\uffff"+
			"\1\3\2\uffff\2\3\1\uffff\1\3\1\uffff\1\3\3\uffff\2\3\6\uffff\3\3\1\uffff"+
			"\1\2\1\uffff\4\3\3\uffff\1\1\3\uffff\3\3\1\uffff\12\3\1\uffff\2\3\7\uffff"+
			"\3\3\4\uffff\1\3\1\uffff\1\3\6\uffff\1\3\5\uffff\1\3\1\uffff\1\3\3\uffff"+
			"\2\3\3\uffff\1\3\4\uffff\1\3\2\uffff\2\3\2\uffff\2\3\7\uffff\1\3",
			"\1\3\11\uffff\1\3\25\uffff\1\3\30\uffff\1\3\1\uffff\1\3\17\uffff\3\3"+
			"\6\uffff\1\34\3\uffff\1\3\2\uffff\1\3\4\uffff\1\3\23\uffff\1\3\7\uffff"+
			"\2\3\1\uffff\1\3\22\uffff\1\3\5\uffff\1\3\32\uffff\1\3",
			"\1\53\7\uffff\1\53\1\uffff\1\53\4\uffff\1\52\2\uffff\1\53\1\64\3\uffff"+
			"\3\76\2\uffff\3\76\3\uffff\1\73\2\uffff\1\71\1\74\11\uffff\1\100\1\77"+
			"\3\uffff\1\100\1\uffff\1\101\1\67\2\100\1\53\4\uffff\1\75\2\uffff\1\100"+
			"\1\70\1\uffff\1\101\1\uffff\1\77\3\uffff\1\55\1\100\6\uffff\1\56\1\60"+
			"\1\57\1\uffff\1\51\1\uffff\1\54\1\61\1\47\1\54\3\uffff\1\63\3\uffff\3"+
			"\62\1\uffff\12\62\1\uffff\1\62\1\50\7\uffff\2\77\1\66\4\uffff\1\77\1"+
			"\uffff\1\77\6\uffff\1\53\5\uffff\1\53\1\uffff\1\66\3\uffff\1\65\1\101"+
			"\3\uffff\1\53\4\uffff\1\72\2\uffff\1\53\1\101\2\uffff\1\67\1\53\7\uffff"+
			"\1\100",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"\1\uffff",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"",
			""
	};

	static final short[] DFA62_eot = DFA.unpackEncodedString(DFA62_eotS);
	static final short[] DFA62_eof = DFA.unpackEncodedString(DFA62_eofS);
	static final char[] DFA62_min = DFA.unpackEncodedStringToUnsignedChars(DFA62_minS);
	static final char[] DFA62_max = DFA.unpackEncodedStringToUnsignedChars(DFA62_maxS);
	static final short[] DFA62_accept = DFA.unpackEncodedString(DFA62_acceptS);
	static final short[] DFA62_special = DFA.unpackEncodedString(DFA62_specialS);
	static final short[][] DFA62_transition;

	static {
		int numStates = DFA62_transitionS.length;
		DFA62_transition = new short[numStates][];
		for (int i=0; i<numStates; i++) {
			DFA62_transition[i] = DFA.unpackEncodedString(DFA62_transitionS[i]);
		}
	}

	protected class DFA62 extends DFA {

		public DFA62(BaseRecognizer recognizer) {
			this.recognizer = recognizer;
			this.decisionNumber = 62;
			this.eot = DFA62_eot;
			this.eof = DFA62_eof;
			this.min = DFA62_min;
			this.max = DFA62_max;
			this.accept = DFA62_accept;
			this.special = DFA62_special;
			this.transition = DFA62_transition;
		}
		@Override
		public String getDescription() {
			return "731:1: assignmentRHS : ( ( NCNAME LPAREN )=> commandInvocation | ( LPAREN expression RPAREN LPAREN )=> commandInvocation | expression );";
		}
		@Override
		public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
			TokenStream input = (TokenStream)_input;
			int _s = s;
			switch ( s ) {
					case 0 : 
						int LA62_28 = input.LA(1);
						 
						int index62_28 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						 
						input.seek(index62_28);
						if ( s>=0 ) return s;
						break;

					case 1 : 
						int LA62_39 = input.LA(1);
						 
						int index62_39 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_39);
						if ( s>=0 ) return s;
						break;

					case 2 : 
						int LA62_40 = input.LA(1);
						 
						int index62_40 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_40);
						if ( s>=0 ) return s;
						break;

					case 3 : 
						int LA62_41 = input.LA(1);
						 
						int index62_41 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_41);
						if ( s>=0 ) return s;
						break;

					case 4 : 
						int LA62_42 = input.LA(1);
						 
						int index62_42 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_42);
						if ( s>=0 ) return s;
						break;

					case 5 : 
						int LA62_43 = input.LA(1);
						 
						int index62_43 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_43);
						if ( s>=0 ) return s;
						break;

					case 6 : 
						int LA62_44 = input.LA(1);
						 
						int index62_44 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_44);
						if ( s>=0 ) return s;
						break;

					case 7 : 
						int LA62_45 = input.LA(1);
						 
						int index62_45 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_45);
						if ( s>=0 ) return s;
						break;

					case 8 : 
						int LA62_46 = input.LA(1);
						 
						int index62_46 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_46);
						if ( s>=0 ) return s;
						break;

					case 9 : 
						int LA62_47 = input.LA(1);
						 
						int index62_47 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_47);
						if ( s>=0 ) return s;
						break;

					case 10 : 
						int LA62_48 = input.LA(1);
						 
						int index62_48 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_48);
						if ( s>=0 ) return s;
						break;

					case 11 : 
						int LA62_49 = input.LA(1);
						 
						int index62_49 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_49);
						if ( s>=0 ) return s;
						break;

					case 12 : 
						int LA62_50 = input.LA(1);
						 
						int index62_50 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_50);
						if ( s>=0 ) return s;
						break;

					case 13 : 
						int LA62_51 = input.LA(1);
						 
						int index62_51 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_51);
						if ( s>=0 ) return s;
						break;

					case 14 : 
						int LA62_52 = input.LA(1);
						 
						int index62_52 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_52);
						if ( s>=0 ) return s;
						break;

					case 15 : 
						int LA62_53 = input.LA(1);
						 
						int index62_53 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_53);
						if ( s>=0 ) return s;
						break;

					case 16 : 
						int LA62_54 = input.LA(1);
						 
						int index62_54 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_54);
						if ( s>=0 ) return s;
						break;

					case 17 : 
						int LA62_55 = input.LA(1);
						 
						int index62_55 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_55);
						if ( s>=0 ) return s;
						break;

					case 18 : 
						int LA62_56 = input.LA(1);
						 
						int index62_56 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_56);
						if ( s>=0 ) return s;
						break;

					case 19 : 
						int LA62_57 = input.LA(1);
						 
						int index62_57 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_57);
						if ( s>=0 ) return s;
						break;

					case 20 : 
						int LA62_58 = input.LA(1);
						 
						int index62_58 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_58);
						if ( s>=0 ) return s;
						break;

					case 21 : 
						int LA62_59 = input.LA(1);
						 
						int index62_59 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_59);
						if ( s>=0 ) return s;
						break;

					case 22 : 
						int LA62_60 = input.LA(1);
						 
						int index62_60 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_60);
						if ( s>=0 ) return s;
						break;

					case 23 : 
						int LA62_61 = input.LA(1);
						 
						int index62_61 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_61);
						if ( s>=0 ) return s;
						break;

					case 24 : 
						int LA62_62 = input.LA(1);
						 
						int index62_62 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_62);
						if ( s>=0 ) return s;
						break;

					case 25 : 
						int LA62_63 = input.LA(1);
						 
						int index62_63 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_63);
						if ( s>=0 ) return s;
						break;

					case 26 : 
						int LA62_64 = input.LA(1);
						 
						int index62_64 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_64);
						if ( s>=0 ) return s;
						break;

					case 27 : 
						int LA62_65 = input.LA(1);
						 
						int index62_65 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred12_Plexil()) ) {s = 66;}
						else if ( (synpred13_Plexil()) ) {s = 67;}
						else if ( (true) ) {s = 3;}
						 
						input.seek(index62_65);
						if ( s>=0 ) return s;
						break;
			}
			if (state.backtracking>0) {state.failed=true; return -1;}
			NoViableAltException nvae =
				new NoViableAltException(getDescription(), 62, _s, input);
			error(nvae);
			throw nvae;
		}
	}

	static final String DFA79_eotS =
		"\106\uffff";
	static final String DFA79_eofS =
		"\106\uffff";
	static final String DFA79_minS =
		"\1\4\5\uffff\1\134\4\uffff\1\10\2\134\1\u0085\14\uffff\2\0\33\uffff\3"+
		"\0\13\uffff";
	static final String DFA79_maxS =
		"\1\u00b7\5\uffff\1\134\4\uffff\1\u00bb\2\134\1\u0085\14\uffff\2\0\33\uffff"+
		"\3\0\13\uffff";
	static final String DFA79_acceptS =
		"\1\uffff\1\1\1\2\1\3\1\4\1\5\1\uffff\1\7\1\uffff\1\10\1\11\4\uffff\1\26"+
		"\7\uffff\1\27\1\30\1\31\1\32\2\uffff\1\17\1\20\34\uffff\1\6\1\12\1\13"+
		"\1\14\1\15\1\16\1\21\1\22\1\23\1\24\1\25";
	static final String DFA79_specialS =
		"\13\uffff\1\0\17\uffff\1\1\1\2\33\uffff\1\3\1\4\1\5\13\uffff}>";
	static final String[] DFA79_transitionS = {
			"\1\3\7\uffff\1\3\1\uffff\1\3\4\uffff\1\2\2\uffff\1\3\1\14\3\uffff\3\27"+
			"\2\uffff\3\27\3\uffff\1\17\2\uffff\2\17\11\uffff\1\31\1\30\3\uffff\1"+
			"\31\1\uffff\1\32\1\17\2\31\1\3\4\uffff\1\17\2\uffff\1\31\1\17\1\uffff"+
			"\1\32\1\uffff\1\30\3\uffff\1\5\1\31\6\uffff\1\6\2\7\1\uffff\1\1\1\uffff"+
			"\1\4\1\11\1\17\1\4\3\uffff\1\13\3\uffff\3\12\1\uffff\12\12\1\uffff\1"+
			"\12\10\uffff\2\30\1\16\4\uffff\1\30\1\uffff\1\30\6\uffff\1\3\5\uffff"+
			"\1\3\1\uffff\1\16\3\uffff\1\15\1\32\3\uffff\1\3\4\uffff\1\17\2\uffff"+
			"\1\3\1\32\2\uffff\1\17\1\3\7\uffff\1\31",
			"",
			"",
			"",
			"",
			"",
			"\1\33",
			"",
			"",
			"",
			"",
			"\1\36\11\uffff\2\36\5\uffff\1\36\13\uffff\1\36\2\uffff\1\36\27\uffff"+
			"\2\36\1\uffff\1\36\2\uffff\1\36\13\uffff\1\36\1\35\2\36\1\uffff\1\36"+
			"\4\uffff\1\36\3\uffff\1\36\2\uffff\1\36\1\uffff\1\36\2\uffff\1\36\21"+
			"\uffff\3\36\7\uffff\1\36\1\34\1\uffff\1\36\7\uffff\1\36\4\uffff\1\36"+
			"\3\uffff\1\36\1\uffff\1\36\1\uffff\1\36\3\uffff\1\36\13\uffff\1\36\3"+
			"\uffff\3\36\5\uffff\2\36\1\uffff\1\36",
			"\1\70",
			"\1\71",
			"\1\72",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"\1\uffff",
			"\1\uffff",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"\1\uffff",
			"\1\uffff",
			"\1\uffff",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			"",
			""
	};

	static final short[] DFA79_eot = DFA.unpackEncodedString(DFA79_eotS);
	static final short[] DFA79_eof = DFA.unpackEncodedString(DFA79_eofS);
	static final char[] DFA79_min = DFA.unpackEncodedStringToUnsignedChars(DFA79_minS);
	static final char[] DFA79_max = DFA.unpackEncodedStringToUnsignedChars(DFA79_maxS);
	static final short[] DFA79_accept = DFA.unpackEncodedString(DFA79_acceptS);
	static final short[] DFA79_special = DFA.unpackEncodedString(DFA79_specialS);
	static final short[][] DFA79_transition;

	static {
		int numStates = DFA79_transitionS.length;
		DFA79_transition = new short[numStates][];
		for (int i=0; i<numStates; i++) {
			DFA79_transition[i] = DFA.unpackEncodedString(DFA79_transitionS[i]);
		}
	}

	protected class DFA79 extends DFA {

		public DFA79(BaseRecognizer recognizer) {
			this.recognizer = recognizer;
			this.decisionNumber = 79;
			this.eot = DFA79_eot;
			this.eof = DFA79_eof;
			this.min = DFA79_min;
			this.max = DFA79_max;
			this.accept = DFA79_accept;
			this.special = DFA79_special;
			this.transition = DFA79_transition;
		}
		@Override
		public String getDescription() {
			return "927:1: quantity : ( LPAREN ! expression RPAREN !| BAR expression BAR -> ^( ABS_KYWD expression ) | oneArgFn ^ LPAREN ! expression RPAREN !| twoArgFn ^ LPAREN ! expression COMMA ! expression RPAREN !| isKnownExp | ( lookupExpr LBRACKET )=> lookupArrayReference | lookupExpr | messageReceivedExp | nodeStatePredicateExp | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD FAILURE_KYWD )=> nodeFailureVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD STATE_KYWD )=> nodeStateVariable | ( ( NCNAME | SELF_KYWD | PARENT_KYWD ) PERIOD nodeStateKywd )=> nodeTimepointValue | ( NCNAME LBRACKET )=> simpleArrayReference | variable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD COMMAND_HANDLE_KYWD )=> nodeCommandHandleVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD FAILURE_KYWD )=> nodeFailureVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD OUTCOME_KYWD )=> nodeOutcomeVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD STATE_KYWD )=> nodeStateVariable | ( ( CHILD_KYWD | SIBLING_KYWD ) LPAREN NCNAME RPAREN PERIOD nodeStateKywd )=> nodeTimepointValue | literalValue | nodeCommandHandleKywd | nodeFailureKywd | nodeStateKywd | nodeOutcomeKywd );";
		}
		@Override
		public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
			TokenStream input = (TokenStream)_input;
			int _s = s;
			switch ( s ) {
					case 0 : 
						int LA79_11 = input.LA(1);
						 
						int index79_11 = input.index();
						input.rewind();
						s = -1;
						if ( (LA79_11==PERIOD) ) {s = 28;}
						else if ( (LA79_11==LBRACKET) && (synpred20_Plexil())) {s = 29;}
						else if ( (LA79_11==AND_KYWD||(LA79_11 >= ASTERISK && LA79_11 <= BAR)||LA79_11==COMMA||LA79_11==CONCURRENCE_KYWD||LA79_11==DEQUALS||(LA79_11 >= FOR_KYWD && LA79_11 <= GEQ)||LA79_11==GREATER||LA79_11==IF_KYWD||LA79_11==LBRACE||(LA79_11 >= LEQ && LA79_11 <= LESS)||LA79_11==LIBRARY_CALL_KYWD||LA79_11==LPAREN||LA79_11==MINUS||LA79_11==MOD_KYWD||LA79_11==NCNAME||LA79_11==NEQUALS||(LA79_11 >= ON_COMMAND_KYWD && LA79_11 <= OR_KYWD)||LA79_11==PERCENT||LA79_11==PLUS||LA79_11==RBRACKET||LA79_11==REQUEST_KYWD||LA79_11==RPAREN||LA79_11==SEMICOLON||LA79_11==SEQUENCE_KYWD||LA79_11==SLASH||LA79_11==SYNCHRONOUS_COMMAND_KYWD||(LA79_11 >= TRY_KYWD && LA79_11 <= UPDATE_KYWD)||(LA79_11 >= WAIT_KYWD && LA79_11 <= WHILE_KYWD)||LA79_11==XOR_KYWD) ) {s = 30;}
						 
						input.seek(index79_11);
						if ( s>=0 ) return s;
						break;

					case 1 : 
						int LA79_27 = input.LA(1);
						 
						int index79_27 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred14_Plexil()) ) {s = 59;}
						else if ( (true) ) {s = 7;}
						 
						input.seek(index79_27);
						if ( s>=0 ) return s;
						break;

					case 2 : 
						int LA79_28 = input.LA(1);
						 
						int index79_28 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred15_Plexil()) ) {s = 60;}
						else if ( (synpred16_Plexil()) ) {s = 61;}
						else if ( (synpred17_Plexil()) ) {s = 62;}
						else if ( (synpred18_Plexil()) ) {s = 63;}
						else if ( (synpred19_Plexil()) ) {s = 64;}
						else if ( (synpred21_Plexil()) ) {s = 65;}
						else if ( (synpred22_Plexil()) ) {s = 66;}
						else if ( (synpred23_Plexil()) ) {s = 67;}
						else if ( (synpred24_Plexil()) ) {s = 68;}
						else if ( (synpred25_Plexil()) ) {s = 69;}
						 
						input.seek(index79_28);
						if ( s>=0 ) return s;
						break;

					case 3 : 
						int LA79_56 = input.LA(1);
						 
						int index79_56 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred15_Plexil()) ) {s = 60;}
						else if ( (synpred16_Plexil()) ) {s = 61;}
						else if ( (synpred17_Plexil()) ) {s = 62;}
						else if ( (synpred18_Plexil()) ) {s = 63;}
						else if ( (synpred19_Plexil()) ) {s = 64;}
						else if ( (synpred21_Plexil()) ) {s = 65;}
						else if ( (synpred22_Plexil()) ) {s = 66;}
						else if ( (synpred23_Plexil()) ) {s = 67;}
						else if ( (synpred24_Plexil()) ) {s = 68;}
						else if ( (synpred25_Plexil()) ) {s = 69;}
						 
						input.seek(index79_56);
						if ( s>=0 ) return s;
						break;

					case 4 : 
						int LA79_57 = input.LA(1);
						 
						int index79_57 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred15_Plexil()) ) {s = 60;}
						else if ( (synpred16_Plexil()) ) {s = 61;}
						else if ( (synpred17_Plexil()) ) {s = 62;}
						else if ( (synpred18_Plexil()) ) {s = 63;}
						else if ( (synpred19_Plexil()) ) {s = 64;}
						else if ( (synpred21_Plexil()) ) {s = 65;}
						else if ( (synpred22_Plexil()) ) {s = 66;}
						else if ( (synpred23_Plexil()) ) {s = 67;}
						else if ( (synpred24_Plexil()) ) {s = 68;}
						else if ( (synpred25_Plexil()) ) {s = 69;}
						 
						input.seek(index79_57);
						if ( s>=0 ) return s;
						break;

					case 5 : 
						int LA79_58 = input.LA(1);
						 
						int index79_58 = input.index();
						input.rewind();
						s = -1;
						if ( (synpred15_Plexil()) ) {s = 60;}
						else if ( (synpred16_Plexil()) ) {s = 61;}
						else if ( (synpred17_Plexil()) ) {s = 62;}
						else if ( (synpred18_Plexil()) ) {s = 63;}
						else if ( (synpred19_Plexil()) ) {s = 64;}
						else if ( (synpred21_Plexil()) ) {s = 65;}
						else if ( (synpred22_Plexil()) ) {s = 66;}
						else if ( (synpred23_Plexil()) ) {s = 67;}
						else if ( (synpred24_Plexil()) ) {s = 68;}
						else if ( (synpred25_Plexil()) ) {s = 69;}
						 
						input.seek(index79_58);
						if ( s>=0 ) return s;
						break;
			}
			if (state.backtracking>0) {state.failed=true; return -1;}
			NoViableAltException nvae =
				new NoViableAltException(getDescription(), 79, _s, input);
			error(nvae);
			throw nvae;
		}
	}

	public static final BitSet FOLLOW_declarations_in_plexilPlan1241 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_plexilPlan1244 = new BitSet(new long[]{0x0000000000000000L});
	public static final BitSet FOLLOW_EOF_in_plexilPlan1246 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_declaration_in_declarations1280 = new BitSet(new long[]{0x0000044080200002L,0x0000000000400200L,0x0000020000010000L});
	public static final BitSet FOLLOW_commandDeclaration_in_declaration1321 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookupDeclaration_in_declaration1327 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_libraryActionDeclaration_in_declaration1333 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_COMMAND_KYWD_in_commandDeclaration1380 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_commandDeclaration1382 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_paramsSpec_in_commandDeclaration1384 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_commandDeclaration1387 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_returnType_in_commandDeclaration1437 = new BitSet(new long[]{0x0000000080000000L});
	public static final BitSet FOLLOW_COMMAND_KYWD_in_commandDeclaration1439 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_commandDeclaration1441 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_paramsSpec_in_commandDeclaration1443 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_commandDeclaration1446 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_returnType_in_lookupDeclaration1519 = new BitSet(new long[]{0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_LOOKUP_KYWD_in_lookupDeclaration1521 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_lookupDeclaration1523 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_paramsSpec_in_lookupDeclaration1525 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_lookupDeclaration1528 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LPAREN_in_paramsSpec1563 = new BitSet(new long[]{0x0000244000200200L,0x0000000000000200L,0x0000020001010000L});
	public static final BitSet FOLLOW_paramsSpecGuts_in_paramsSpec1565 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_paramsSpec1568 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_paramSpec_in_paramsSpecGuts1597 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_COMMA_in_paramsSpecGuts1601 = new BitSet(new long[]{0x0000044000200200L,0x0000000000000200L,0x0000020000010000L});
	public static final BitSet FOLLOW_paramSpec_in_paramsSpecGuts1604 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_COMMA_in_paramsSpecGuts1611 = new BitSet(new long[]{0x0000200000000000L});
	public static final BitSet FOLLOW_ELLIPSIS_in_paramsSpecGuts1614 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_ELLIPSIS_in_paramsSpecGuts1629 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_paramSpec1670 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_paramSpec1672 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_paramSpec1674 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_paramSpec1676 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_paramSpec1708 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_paramSpec1710 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_paramSpec1712 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_paramSpec1714 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_paramSpec1716 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_paramTypeName_in_paramSpec1734 = new BitSet(new long[]{0x0000000000000002L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_paramSpec1737 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_returnTypeSpec_in_returnType1817 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_returnTypeSpec1853 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_returnTypeSpec1855 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_returnTypeSpec1857 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_returnTypeSpec1859 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_returnTypeSpec1875 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LIBRARY_ACTION_KYWD_in_libraryActionDeclaration1945 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_libraryActionDeclaration1948 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_libraryInterfaceSpec_in_libraryActionDeclaration1950 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_libraryActionDeclaration1953 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LPAREN_in_libraryInterfaceSpec1978 = new BitSet(new long[]{0x0000000000000000L,0x000000000000C000L,0x0000000001000000L});
	public static final BitSet FOLLOW_libraryParamSpec_in_libraryInterfaceSpec1982 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_COMMA_in_libraryInterfaceSpec1986 = new BitSet(new long[]{0x0000000000000000L,0x000000000000C000L});
	public static final BitSet FOLLOW_libraryParamSpec_in_libraryInterfaceSpec1988 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_libraryInterfaceSpec1996 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_IN_KYWD_in_libraryParamSpec2022 = new BitSet(new long[]{0x0000044000200000L,0x0000000000000200L,0x0000020000010000L});
	public static final BitSet FOLLOW_IN_OUT_KYWD_in_libraryParamSpec2027 = new BitSet(new long[]{0x0000044000200000L,0x0000000000000200L,0x0000020000010000L});
	public static final BitSet FOLLOW_baseTypeName_in_libraryParamSpec2033 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_libraryParamSpec2046 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_libraryParamSpec2048 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_libraryParamSpec2051 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_libraryParamSpec2053 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_libraryParamSpec2060 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_action2095 = new BitSet(new long[]{0x0000000001000000L});
	public static final BitSet FOLLOW_COLON_in_action2097 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_baseAction_in_action2104 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_compoundAction_in_baseAction2131 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_simpleAction_in_baseAction2135 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_block_in_baseAction2139 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_forAction_in_compoundAction2149 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_ifAction_in_compoundAction2153 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_onCommandAction_in_compoundAction2157 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_onMessageAction_in_compoundAction2161 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_whileAction_in_compoundAction2165 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_assignment_in_simpleAction2193 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_commandInvocation_in_simpleAction2213 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_simpleAction2215 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_libraryCall_in_simpleAction2222 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_request_in_simpleAction2228 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_update_in_simpleAction2234 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_synchCmd_in_simpleAction2240 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_waitBuiltin_in_simpleAction2246 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_FOR_KYWD_in_forAction2271 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_forAction2278 = new BitSet(new long[]{0x0000044000200000L,0x0000000000000200L,0x0000020000010000L});
	public static final BitSet FOLLOW_baseTypeName_in_forAction2280 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_forAction2282 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_forAction2284 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_forAction2288 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_forAction2294 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_forAction2298 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_forAction2304 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_forAction2308 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_forAction2314 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_forAction2320 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_IF_KYWD_in_ifAction2374 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_ifAction2377 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_ifAction2379 = new BitSet(new long[]{0x0001C00000000000L});
	public static final BitSet FOLLOW_ELSEIF_KYWD_in_ifAction2386 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_ifAction2389 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_ifAction2391 = new BitSet(new long[]{0x0001C00000000000L});
	public static final BitSet FOLLOW_ELSE_KYWD_in_ifAction2400 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_ifAction2403 = new BitSet(new long[]{0x0001000000000000L});
	public static final BitSet FOLLOW_ENDIF_KYWD_in_ifAction2411 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_ifAction2418 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_ON_COMMAND_KYWD_in_onCommandAction2446 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_onCommandAction2449 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_paramsSpec_in_onCommandAction2451 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_onCommandAction2454 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_ON_MESSAGE_KYWD_in_onMessageAction2479 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_onMessageAction2485 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_onMessageAction2487 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_WHILE_KYWD_in_whileAction2512 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_whileAction2515 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010100000L});
	public static final BitSet FOLLOW_action_in_whileAction2517 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_SYNCHRONOUS_COMMAND_KYWD_in_synchCmd2542 = new BitSet(new long[]{0x0000000000000000L,0x0000002010000000L});
	public static final BitSet FOLLOW_commandWithAssignment_in_synchCmd2551 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000200004000000L});
	public static final BitSet FOLLOW_commandInvocation_in_synchCmd2555 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000200004000000L});
	public static final BitSet FOLLOW_TIMEOUT_KYWD_in_synchCmd2562 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_synchCmd2565 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_synchCmd2569 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_synchCmd2572 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_synchCmd2581 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_WAIT_KYWD_in_waitBuiltin2604 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_waitBuiltin2607 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_waitBuiltin2610 = new BitSet(new long[]{0x0000020000000000L,0x0000002000000100L});
	public static final BitSet FOLLOW_variable_in_waitBuiltin2614 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_INT_in_waitBuiltin2616 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_DOUBLE_in_waitBuiltin2618 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_waitBuiltin2623 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_sequenceVariantKywd_in_block2656 = new BitSet(new long[]{0x0000000000000000L,0x0000000000040000L});
	public static final BitSet FOLLOW_LBRACE_in_block2658 = new BitSet(new long[]{0x0042046800200000L,0x0C0000201084E241L,0x0307120890395A00L});
	public static final BitSet FOLLOW_LBRACE_in_block2672 = new BitSet(new long[]{0x0042046800200000L,0x0C0000201084E241L,0x0307120890395A00L});
	public static final BitSet FOLLOW_comment_in_block2683 = new BitSet(new long[]{0x0042046000200000L,0x0C0000201084E241L,0x0307120890395A00L});
	public static final BitSet FOLLOW_nodeDeclaration_in_block2690 = new BitSet(new long[]{0x0042046000200000L,0x0C0000201084E241L,0x0307120890395A00L});
	public static final BitSet FOLLOW_nodeAttribute_in_block2697 = new BitSet(new long[]{0x0042002000000000L,0x0C00002010842041L,0x0307100890385A00L});
	public static final BitSet FOLLOW_action_in_block2704 = new BitSet(new long[]{0x0000002000000000L,0x0C00002010840041L,0x0307100010104000L});
	public static final BitSet FOLLOW_RBRACE_in_block2711 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_COMMENT_KYWD_in_comment2773 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000008000000000L});
	public static final BitSet FOLLOW_STRING_in_comment2776 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_comment2778 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_interfaceDeclaration_in_nodeDeclaration2792 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_variableDeclaration_in_nodeDeclaration2798 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeCondition_in_nodeAttribute2810 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_priority_in_nodeAttribute2816 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_resource_in_nodeAttribute2822 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_conditionKywd_in_nodeCondition2846 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_nodeCondition2849 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_nodeCondition2851 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_RESOURCE_KYWD_in_resource2932 = new BitSet(new long[]{0x0000000000000000L,0x0000001000000000L});
	public static final BitSet FOLLOW_NAME_KYWD_in_resource2935 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_resource2938 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_resource2941 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_resource2953 = new BitSet(new long[]{0x0000000000000000L,0x0000000008000000L,0x0008000000041000L});
	public static final BitSet FOLLOW_LOWER_BOUND_KYWD_in_resource2968 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_resource2970 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_resource2973 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_UPPER_BOUND_KYWD_in_resource2987 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_resource2989 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_resource2992 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_RELEASE_AT_TERM_KYWD_in_resource3002 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_resource3004 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_resource3007 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_PRIORITY_KYWD_in_resource3016 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_resource3018 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_resource3023 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_resource3056 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_PRIORITY_KYWD_in_priority3078 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_priority3084 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_priority3086 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_in_in_interfaceDeclaration3096 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_inOut_in_interfaceDeclaration3100 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_IN_KYWD_in_in3129 = new BitSet(new long[]{0x0000044000200000L,0x0000002000000200L,0x0000020000010000L});
	public static final BitSet FOLLOW_NCNAME_in_in3142 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_in3145 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_in3148 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_interfaceDeclarations_in_in3158 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_in3173 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_IN_OUT_KYWD_in_inOut3200 = new BitSet(new long[]{0x0000044000200000L,0x0000002000000200L,0x0000020000010000L});
	public static final BitSet FOLLOW_NCNAME_in_inOut3212 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_inOut3215 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_inOut3218 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_interfaceDeclarations_in_inOut3228 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_inOut3243 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_interfaceDeclarations3261 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_arrayVariableDecl_in_interfaceDeclarations3278 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_scalarVariableDecl_in_interfaceDeclarations3288 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_COMMA_in_interfaceDeclarations3300 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_arrayVariableDecl_in_interfaceDeclarations3319 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_scalarVariableDecl_in_interfaceDeclarations3331 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_NCNAME_in_variable3355 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_variableDeclaration3385 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_arrayVariableDecl_in_variableDeclaration3401 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_scalarVariableDecl_in_variableDeclaration3411 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_variableDeclaration3426 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_arrayVariableDecl_in_variableDeclaration3442 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_scalarVariableDecl_in_variableDeclaration3451 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_variableDeclaration3467 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_scalarVariableDecl3499 = new BitSet(new long[]{0x0008000000000002L});
	public static final BitSet FOLLOW_EQUALS_in_scalarVariableDecl3503 = new BitSet(new long[]{0x1000064000000000L,0x0000000100000100L,0x0000408000000000L});
	public static final BitSet FOLLOW_literalScalarValue_in_scalarVariableDecl3505 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_arrayVariableDecl3541 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_arrayVariableDecl3543 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_arrayVariableDecl3545 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_arrayVariableDecl3547 = new BitSet(new long[]{0x0008000000000002L});
	public static final BitSet FOLLOW_EQUALS_in_arrayVariableDecl3551 = new BitSet(new long[]{0x1000064000000000L,0x0000000100000110L,0x0000408000000000L});
	public static final BitSet FOLLOW_literalValue_in_arrayVariableDecl3553 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_booleanLiteral_in_literalScalarValue3589 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_INT_in_literalScalarValue3593 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_DOUBLE_in_literalScalarValue3597 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_STRING_in_literalScalarValue3601 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_unaryMinus_in_literalScalarValue3605 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_dateLiteral_in_literalScalarValue3613 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_durationLiteral_in_literalScalarValue3617 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_HASHPAREN_in_literalArrayValue3630 = new BitSet(new long[]{0x1000064000000000L,0x0000000100000100L,0x0000408001000000L});
	public static final BitSet FOLLOW_literalScalarValue_in_literalArrayValue3632 = new BitSet(new long[]{0x1000064000000000L,0x0000000100000100L,0x0000408001000000L});
	public static final BitSet FOLLOW_RPAREN_in_literalArrayValue3635 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_literalScalarValue_in_literalValue3659 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_literalArrayValue_in_literalValue3663 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookup_in_lookupArrayReference3704 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_lookupArrayReference3706 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_lookupArrayReference3708 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_lookupArrayReference3710 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_variable_in_simpleArrayReference3739 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_simpleArrayReference3741 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_simpleArrayReference3743 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
	public static final BitSet FOLLOW_RBRACKET_in_simpleArrayReference3745 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_commandInvocation3787 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_commandInvocation3803 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_commandInvocation3805 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_commandInvocation3807 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_commandInvocation3823 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846382050EL});
	public static final BitSet FOLLOW_argumentList_in_commandInvocation3825 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_commandInvocation3828 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_assignmentLHS_in_commandWithAssignment3873 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_commandWithAssignment3875 = new BitSet(new long[]{0x0000000000000000L,0x0000002010000000L});
	public static final BitSet FOLLOW_commandInvocation_in_commandWithAssignment3877 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_argument_in_argumentList3907 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_COMMA_in_argumentList3910 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_argument_in_argumentList3912 = new BitSet(new long[]{0x0000000002000002L});
	public static final BitSet FOLLOW_expression_in_argument3938 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_assignmentLHS_in_assignment3962 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_assignment3964 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_assignmentRHS_in_assignment3966 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_assignment3968 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_simpleArrayReference_in_assignmentLHS4011 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_variable_in_assignmentLHS4022 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_commandInvocation_in_assignmentRHS4043 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_commandInvocation_in_assignmentRHS4063 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_expression_in_assignmentRHS4071 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_UPDATE_KYWD_in_update4100 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_pair_in_update4105 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_update4109 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_pair_in_update4112 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_update4120 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_REQUEST_KYWD_in_request4147 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_request4150 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_pair_in_request4154 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_COMMA_in_request4158 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_pair_in_request4161 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_request4169 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_pair4181 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_pair4183 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_pair4186 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LIBRARY_CALL_KYWD_in_libraryCall4212 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_libraryNodeIdRef_in_libraryCall4215 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_aliasSpecs_in_libraryCall4219 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000004000000L});
	public static final BitSet FOLLOW_SEMICOLON_in_libraryCall4224 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_libraryNodeIdRef4234 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LPAREN_in_aliasSpecs4245 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_aliasSpec_in_aliasSpecs4249 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_COMMA_in_aliasSpecs4253 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_aliasSpec_in_aliasSpecs4255 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_aliasSpecs4263 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_aliasSpec4287 = new BitSet(new long[]{0x0008000000000000L});
	public static final BitSet FOLLOW_EQUALS_in_aliasSpec4289 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_aliasSpec4291 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_nodeParameterName4319 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_logicalOr_in_expression4348 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_logicalXOR_in_logicalOr4370 = new BitSet(new long[]{0x0000000000000002L,0x1000000000000000L});
	public static final BitSet FOLLOW_OR_KYWD_in_logicalOr4384 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_logicalXOR_in_logicalOr4388 = new BitSet(new long[]{0x0000000000000002L,0x1000000000000000L});
	public static final BitSet FOLLOW_logicalAnd_in_logicalXOR4431 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000000L,0x0800000000000000L});
	public static final BitSet FOLLOW_XOR_KYWD_in_logicalXOR4446 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_logicalAnd_in_logicalXOR4450 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000000L,0x0800000000000000L});
	public static final BitSet FOLLOW_equality_in_logicalAnd4495 = new BitSet(new long[]{0x0000000000000102L});
	public static final BitSet FOLLOW_AND_KYWD_in_logicalAnd4509 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_equality_in_logicalAnd4513 = new BitSet(new long[]{0x0000000000000102L});
	public static final BitSet FOLLOW_relational_in_equality4560 = new BitSet(new long[]{0x0000010000000002L,0x0000010000000000L});
	public static final BitSet FOLLOW_equalityOp_in_equality4574 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_relational_in_equality4578 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_additive_in_relational4638 = new BitSet(new long[]{0x0000000000000002L,0x000000000030000AL});
	public static final BitSet FOLLOW_relationalOp_in_relational4650 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_additive_in_relational4654 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_multiplicative_in_additive4731 = new BitSet(new long[]{0x0000000000000002L,0x0000000100000000L,0x0000000000000080L});
	public static final BitSet FOLLOW_addOp_in_additive4746 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_multiplicative_in_additive4750 = new BitSet(new long[]{0x0000000000000002L,0x0000000100000000L,0x0000000000000080L});
	public static final BitSet FOLLOW_unary_in_multiplicative4811 = new BitSet(new long[]{0x0000000000040002L,0x0000000800000000L,0x0000000100000010L});
	public static final BitSet FOLLOW_multOp_in_multiplicative4826 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_unary_in_multiplicative4830 = new BitSet(new long[]{0x0000000000040002L,0x0000000800000000L,0x0000000100000010L});
	public static final BitSet FOLLOW_unaryMinus_in_unary4904 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_unaryOp_in_unary4914 = new BitSet(new long[]{0xFA30064738C85010L,0x017FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_quantity_in_unary4917 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_quantity_in_unary4927 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NOT_KYWD_in_unaryOp4942 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_MINUS_in_unaryMinus4952 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000100L});
	public static final BitSet FOLLOW_INT_in_unaryMinus4956 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_MINUS_in_unaryMinus4982 = new BitSet(new long[]{0x0000020000000000L});
	public static final BitSet FOLLOW_DOUBLE_in_unaryMinus4986 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_DATE_KYWD_in_dateLiteral5017 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_dateLiteral5019 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000008000000000L});
	public static final BitSet FOLLOW_STRING_in_dateLiteral5023 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_dateLiteral5025 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_DURATION_KYWD_in_durationLiteral5045 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_durationLiteral5047 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000008000000000L});
	public static final BitSet FOLLOW_STRING_in_durationLiteral5051 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_durationLiteral5053 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LPAREN_in_quantity5082 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_quantity5085 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_quantity5087 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_BAR_in_quantity5094 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_quantity5096 = new BitSet(new long[]{0x0000000000080000L});
	public static final BitSet FOLLOW_BAR_in_quantity5098 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_oneArgFn_in_quantity5112 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_quantity5115 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_quantity5118 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_quantity5120 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_twoArgFn_in_quantity5127 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_quantity5130 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_quantity5133 = new BitSet(new long[]{0x0000000002000000L});
	public static final BitSet FOLLOW_COMMA_in_quantity5135 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_quantity5138 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_quantity5140 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_isKnownExp_in_quantity5147 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookupArrayReference_in_quantity5161 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookupExpr_in_quantity5167 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_messageReceivedExp_in_quantity5173 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStatePredicateExp_in_quantity5179 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeCommandHandleVariable_in_quantity5206 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeFailureVariable_in_quantity5233 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeOutcomeVariable_in_quantity5260 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStateVariable_in_quantity5287 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeTimepointValue_in_quantity5314 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_simpleArrayReference_in_quantity5328 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_variable_in_quantity5334 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeCommandHandleVariable_in_quantity5364 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeFailureVariable_in_quantity5393 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeOutcomeVariable_in_quantity5422 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStateVariable_in_quantity5451 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeTimepointValue_in_quantity5480 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_literalValue_in_quantity5486 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeCommandHandleKywd_in_quantity5492 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeFailureKywd_in_quantity5498 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStateKywd_in_quantity5504 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeOutcomeKywd_in_quantity5510 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_IS_KNOWN_KYWD_in_isKnownExp5611 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_isKnownExp5617 = new BitSet(new long[]{0xFA30064738C85010L,0x017FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_quantity_in_isKnownExp5620 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_isKnownExp5622 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStatePredicate_in_nodeStatePredicateExp5724 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_nodeStatePredicateExp5727 = new BitSet(new long[]{0x0000000000800000L,0x0000002000000000L,0x0000000022000008L});
	public static final BitSet FOLLOW_nodeReference_in_nodeStatePredicateExp5730 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_nodeStatePredicateExp5732 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_MESSAGE_RECEIVED_KYWD_in_messageReceivedExp5802 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_messageReceivedExp5805 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000008000000000L});
	public static final BitSet FOLLOW_STRING_in_messageReceivedExp5808 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_messageReceivedExp5810 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStateVariable_in_nodeState5821 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeStateKywd_in_nodeState5825 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeReference_in_nodeStateVariable5834 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_nodeStateVariable5836 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_STATE_KYWD_in_nodeStateVariable5839 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeOutcomeVariable_in_nodeOutcome5852 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeOutcomeKywd_in_nodeOutcome5856 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeReference_in_nodeOutcomeVariable5865 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_nodeOutcomeVariable5867 = new BitSet(new long[]{0x0000000000000000L,0x2000000000000000L});
	public static final BitSet FOLLOW_OUTCOME_KYWD_in_nodeOutcomeVariable5870 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeCommandHandleVariable_in_nodeCommandHandle5914 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeCommandHandleKywd_in_nodeCommandHandle5918 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeReference_in_nodeCommandHandleVariable5927 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_nodeCommandHandleVariable5929 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_COMMAND_HANDLE_KYWD_in_nodeCommandHandleVariable5932 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeFailureVariable_in_nodeFailure5989 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeFailureKywd_in_nodeFailure5993 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeReference_in_nodeFailureVariable6002 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_nodeFailureVariable6004 = new BitSet(new long[]{0x0400000000000000L});
	public static final BitSet FOLLOW_FAILURE_KYWD_in_nodeFailureVariable6007 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeReference_in_nodeTimepointValue6067 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_nodeTimepointValue6069 = new BitSet(new long[]{0x6210000000000000L,0x0000000000020080L,0x0080000000000000L});
	public static final BitSet FOLLOW_nodeStateKywd_in_nodeTimepointValue6071 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_nodeTimepointValue6073 = new BitSet(new long[]{0x0004000000000000L,0x0000000000000000L,0x0000001000000000L});
	public static final BitSet FOLLOW_timepoint_in_nodeTimepointValue6075 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_nodeId_in_nodeReference6118 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_CHILD_KYWD_in_nodeReference6124 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_nodeReference6127 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_nodeReference6130 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_nodeReference6132 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_SIBLING_KYWD_in_nodeReference6139 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_nodeReference6142 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_nodeReference6145 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_nodeReference6147 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookupOnChange_in_lookupExpr6190 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookupNow_in_lookupExpr6194 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookup_in_lookupExpr6198 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LOOKUP_ON_CHANGE_KYWD_in_lookupOnChange6229 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_lookupOnChange6232 = new BitSet(new long[]{0x0000000000000000L,0x0000002010000000L});
	public static final BitSet FOLLOW_lookupInvocation_in_lookupOnChange6235 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_COMMA_in_lookupOnChange6238 = new BitSet(new long[]{0x0000060000000000L,0x0000002000000100L});
	public static final BitSet FOLLOW_tolerance_in_lookupOnChange6241 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_lookupOnChange6245 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_realValue_in_tolerance6255 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_durationLiteral_in_tolerance6259 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_variable_in_tolerance6263 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LOOKUP_NOW_KYWD_in_lookupNow6290 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_lookupNow6293 = new BitSet(new long[]{0x0000000000000000L,0x0000002010000000L});
	public static final BitSet FOLLOW_lookupInvocation_in_lookupNow6296 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_lookupNow6298 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LOOKUP_KYWD_in_lookup6328 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_lookup6331 = new BitSet(new long[]{0x0000000000000000L,0x0000002010000000L});
	public static final BitSet FOLLOW_lookupInvocation_in_lookup6334 = new BitSet(new long[]{0x0000000002000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_COMMA_in_lookup6337 = new BitSet(new long[]{0x0000060000000000L,0x0000002000000100L});
	public static final BitSet FOLLOW_tolerance_in_lookup6340 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_lookup6344 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_stateName_in_lookupInvocation6360 = new BitSet(new long[]{0x0000000000000002L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_lookupInvocation6374 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_stateNameExp_in_lookupInvocation6377 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_lookupInvocation6379 = new BitSet(new long[]{0x0000000000000002L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_lookupInvocation6392 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846382050EL});
	public static final BitSet FOLLOW_argumentList_in_lookupInvocation6396 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_lookupInvocation6400 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_stateName6418 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_expression_in_stateNameExp6436 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_synpred1_Plexil1657 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred1_Plexil1659 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_synpred2_Plexil1693 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_synpred2_Plexil1695 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred2_Plexil1697 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_baseTypeName_in_synpred3_Plexil1840 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred3_Plexil1842 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred4_Plexil2039 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred4_Plexil2041 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred5_Plexil2180 = new BitSet(new long[]{0x0008000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_set_in_synpred5_Plexil2182 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred6_Plexil2201 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred6_Plexil2203 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LPAREN_in_synpred6_Plexil2208 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred7_Plexil3271 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred7_Plexil3273 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred8_Plexil3312 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred8_Plexil3314 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred9_Plexil3394 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred9_Plexil3396 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred10_Plexil3435 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred10_Plexil3437 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred11_Plexil3999 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred11_Plexil4001 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred12_Plexil4036 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred12_Plexil4038 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_LPAREN_in_synpred13_Plexil4052 = new BitSet(new long[]{0xFA30064738C85010L,0x037FEE23D7031590L,0x0080CC846282050EL});
	public static final BitSet FOLLOW_expression_in_synpred13_Plexil4054 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_synpred13_Plexil4056 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred13_Plexil4058 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_lookupExpr_in_synpred14_Plexil5154 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred14_Plexil5156 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred15_Plexil5187 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred15_Plexil5199 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_COMMAND_HANDLE_KYWD_in_synpred15_Plexil5201 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred16_Plexil5214 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred16_Plexil5226 = new BitSet(new long[]{0x0400000000000000L});
	public static final BitSet FOLLOW_FAILURE_KYWD_in_synpred16_Plexil5228 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred17_Plexil5241 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred17_Plexil5253 = new BitSet(new long[]{0x0000000000000000L,0x2000000000000000L});
	public static final BitSet FOLLOW_OUTCOME_KYWD_in_synpred17_Plexil5255 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred18_Plexil5268 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred18_Plexil5280 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_STATE_KYWD_in_synpred18_Plexil5282 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred19_Plexil5295 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred19_Plexil5307 = new BitSet(new long[]{0x6210000000000000L,0x0000000000020080L,0x0080000000000000L});
	public static final BitSet FOLLOW_nodeStateKywd_in_synpred19_Plexil5309 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_NCNAME_in_synpred20_Plexil5321 = new BitSet(new long[]{0x0000000000000000L,0x0000000000080000L});
	public static final BitSet FOLLOW_LBRACKET_in_synpred20_Plexil5323 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred21_Plexil5343 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred21_Plexil5351 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_synpred21_Plexil5353 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_synpred21_Plexil5355 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred21_Plexil5357 = new BitSet(new long[]{0x0000000040000000L});
	public static final BitSet FOLLOW_COMMAND_HANDLE_KYWD_in_synpred21_Plexil5359 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred22_Plexil5372 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred22_Plexil5380 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_synpred22_Plexil5382 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_synpred22_Plexil5384 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred22_Plexil5386 = new BitSet(new long[]{0x0400000000000000L});
	public static final BitSet FOLLOW_FAILURE_KYWD_in_synpred22_Plexil5388 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred23_Plexil5401 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred23_Plexil5409 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_synpred23_Plexil5411 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_synpred23_Plexil5413 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred23_Plexil5415 = new BitSet(new long[]{0x0000000000000000L,0x2000000000000000L});
	public static final BitSet FOLLOW_OUTCOME_KYWD_in_synpred23_Plexil5417 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred24_Plexil5430 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred24_Plexil5438 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_synpred24_Plexil5440 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_synpred24_Plexil5442 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred24_Plexil5444 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_STATE_KYWD_in_synpred24_Plexil5446 = new BitSet(new long[]{0x0000000000000002L});
	public static final BitSet FOLLOW_set_in_synpred25_Plexil5459 = new BitSet(new long[]{0x0000000000000000L,0x0000000010000000L});
	public static final BitSet FOLLOW_LPAREN_in_synpred25_Plexil5467 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
	public static final BitSet FOLLOW_NCNAME_in_synpred25_Plexil5469 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000001000000L});
	public static final BitSet FOLLOW_RPAREN_in_synpred25_Plexil5471 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
	public static final BitSet FOLLOW_PERIOD_in_synpred25_Plexil5473 = new BitSet(new long[]{0x6210000000000000L,0x0000000000020080L,0x0080000000000000L});
	public static final BitSet FOLLOW_nodeStateKywd_in_synpred25_Plexil5475 = new BitSet(new long[]{0x0000000000000002L});
}

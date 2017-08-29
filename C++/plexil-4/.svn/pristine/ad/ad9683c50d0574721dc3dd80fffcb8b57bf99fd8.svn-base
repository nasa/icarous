/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package gov.nasa.luv;

/**
 * Class containing constants from the Core PLEXIL XML schema
 */

public class PlexilSchema
{
    //
    // Tags
    //

    public static final String PLEXIL_PLAN = "PlexilPlan";
    
    public static final String GLOBAL_DECLARATIONS = "GlobalDeclarations";

    public static final String LIBRARY_NODE_DECLARATION = "LibraryNodeDeclaration";
    public static final String COMMAND_DECLARATION = "CommandDeclaration";
    public static final String STATE_DECLARATION = "StateDeclaration";

    public static final String PARAMETER = "Parameter";
    public static final String RETURN = "Return";

    public static final String NODE = "Node";
    public static final String NODE_ID = "NodeId";
    public static final String PRIORITY = "Priority";

    public static final String INTERFACE = "Interface";
    public static final String IN = "In";
    public static final String INOUT = "InOut";

    public static final String VAR_DECLS = "VariableDeclarations";

    public static final String DECL_VAR = "DeclareVariable";
    public static final String TYPE = "Type";

    public static final String DECL_ARRAY = "DeclareArray";
    public static final String MAX_SIZE = "MaxSize";
    public static final String INITIAL_VALUE = "InitialValue";

    // Node body

    public static final String NODE_BODY = "NodeBody";

    // Used as both NodeBody tags and NodeType attribute values.
    public static final String ASSIGNMENT = "Assignment";
    public static final String COMMAND = "Command";
    public static final String LIBRARYNODECALL = "LibraryNodeCall";
    public static final String NODELIST = "NodeList";
    public static final String UPDATE = "Update";

    public enum NodeType {
        Assignment,
        Command,
        Empty,
        LibraryNodeCall,
        NodeList,
        Update;
    }

    // LibraryNodeCall body tags
    public static final String ALIAS = "Alias";
    public static final String NODE_PARAMETER = "NodeParameter";

    // Update body tags
    public static final String PAIR = "Pair";

    // Command body tags
    public static final String RESOURCE_LIST = "ResourceList";
    public static final String RESOURCE = "Resource";
    public static final String RESOURCE_NAME = "ResourceName";
    public static final String RESOURCE_PRIORITY = "ResourcePriority";
    public static final String RESOURCE_LOWER_BOUND = "ResourceLowerBound";
    public static final String RESOURCE_UPPER_BOUND = "ResourceUpperBound";
    public static final String RESOURCE_RELEASE_AT_TERM = "ResourceReleaseAtTermination";

    // Used in both Command and lookups.
    public static final String NAME = "Name";
    public static final String ARGS = "Arguments";

    // Arithmetic expressions

    public static final String ADD = "ADD";
    public static final String SUB = "SUB";
    public static final String MUL = "MUL";
    public static final String DIV = "DIV";
    public static final String MOD = "MOD";
    public static final String MAX = "MAX";
    public static final String MIN = "MIN";
    public static final String SQRT = "SQRT";
    public static final String ABS = "ABS";
    public static final String CEIL = "CEIL";
    public static final String FLOOR = "FLOOR";
    public static final String ROUND = "ROUND";
    public static final String TRUNC = "TRUNC";
    public static final String REAL_TO_INT = "REAL_TO_INT";

    // Comparisons
    
    public static final String EQ_NUMERIC = "EQNumeric";
    public static final String EQ_STRING = "EQString";
    public static final String EQ_BOOLEAN = "EQBoolean";
    public static final String EQ_INTERNAL = "EQInternal";
    public static final String NE_NUMERIC = "NENumeric";
    public static final String NE_STRING = "NEString";
    public static final String NE_BOOLEAN = "NEBoolean";
    public static final String NE_INTERNAL = "NEInternal";

    public static final String LT = "LT";
    public static final String LE = "LE";
    public static final String GT = "GT";
    public static final String GE = "GE";

    // Node references

    public static final String NODE_REF = "NodeRef";
    public static final String TIMEPOINT = "Timepoint";
      
    // Conditions

    // IMPORTANT! Must be kept in same order as C++ Node enum ConditionIndex.
    // See $PLEXIL_HOME/src/exec/Node.hh
    public enum Condition {
        AncestorEndCondition(0),
        AncestorInvariantCondition(1),
        AncestorExitCondition(2),
        SkipCondition(3),
        StartCondition(4),
        PreCondition(5),
        ExitCondition(6),
        InvariantCondition(7),
        EndCondition(8),
        PostCondition(9),
        RepeatCondition(10),
        ActionCompleteCondition(11),
        AbortCompleteCondition(12);

        private int index;

        Condition(int idx) {
            index = idx;
        }

        public int getIndex() {
            return index;
        }
    }

    // Expressions

    // Literals
    public static final String ARRAY_VAL = "ArrayValue";
    public static final String BOOL_VAL = "BooleanValue";
    public static final String INT_VAL = "IntegerValue";
    public static final String REAL_VAL = "RealValue";
    public static final String STRING_VAL = "StringValue";

    public static final String NODE_STATE_VAL = "NodeStateValue";
    public static final String NODE_OUTCOME_VAL = "NodeOutcomeValue";
    public static final String NODE_FAILURE_VAL = "NodeFailureValue";
    public static final String NODE_CMD_HANDLE_VAL = "NodeCommandHandleValue";

    // Variable references
    public static final String ARRAY_VAR = "ArrayVariable";      
    public static final String BOOL_VAR = "BooleanVariable";
    public static final String INT_VAR = "IntegerVariable";
    public static final String REAL_VAR = "RealVariable";
    public static final String STRING_VAR = "StringVariable";

    // Array accesors
    public static final String ARRAY_SIZE = "ARRAY_SIZE";
    public static final String ARRAY_MAX_SIZE = "ARRAY_MAX_SIZE";
    public static final String ARRAY_ELEMENT = "ArrayElement";
    public static final String INDEX = "Index";
    public static final String ALL_KNOWN = "ALL_KNOWN";
    public static final String ANY_KNOWN = "ANY_KNOWN";

    // Boolean operators
    public static final String AND = "AND";
    public static final String OR = "OR";
    public static final String XOR = "XOR";
    public static final String NOT = "NOT";

    // Operations on strings
    public static final String CONCAT = "Concat";
    public static final String STRLEN = "STRLEN";

    // Lookups
    public static final String LOOKUPNOW = "LookupNow";
    public static final String LOOKUPCHANGE = "LookupOnChange";
    public static final String TOLERANCE = "Tolerance";

    // Internal variables
    public static final String NODE_STATE_VAR = "NodeStateVariable";
    public static final String NODE_OUTCOME_VAR = "NodeOutcomeVariable";
    public static final String NODE_FAILURE_VAR = "NodeFailureVariable";
    public static final String NODE_CMD_HANDLE_VAR = "NodeCommandHandleVariable";
    public static final String NODE_TIMEPOINT_VAL = "NodeTimepointValue";

    // Misc
    public static final String IS_KNOWN = "IsKnown";



    //
    // Attribute names
    //

    // Source locators
    //* Original source file name.
    public static final String FILE_NAME_ATTR = "FileName";
    //* Line number in original source file.
    public static final String LINE_NO_ATTR = "LineNo";
    //* Column number in original source file.
    public static final String COL_NO_ATTR = "ColNo";

    // Node attributes
    //* NodeType attribute
    public static final String NODETYPE_ATTR = "NodeType";
    //* Extended Plexil attribute showing node's syntactic origin in the source
    public static final String EPX_ATTR = "epx";

    //* NodeRef direction attribute
    public static final String DIR_ATTR = "dir";

    //* ArrayValue element type attribute
    public static final String TYPE_ATTR = "Type";

    //
    // Attribute values
    //

    // NodeType attribute value w/o matching tag (for obvious reasons)
    public static final String EMPTY = "Empty";

    // epx attribute values
    public static final String AUX = "aux";
    public static final String AUX_THEN = "aux_then";
    public static final String AUX_ELSE = "aux_else";
    public static final String IF = "If";
    public static final String THEN  = "Then";
    public static final String ELSE  = "Else";
    public static final String WHILE  = "While";
    public static final String TRY  = "Try";
    public static final String FOR  = "For";
    public static final String SEQ  = "Sequence";
    public static final String UNCHKD_SEQ = "UncheckedSequence";
    public static final String CONCURRENCE = "Concurrence";
      
    // Node state values
    public enum NodeState {
        INACTIVE,
        WAITING,
        EXECUTING,
        ITERATION_ENDED,
        FINISHED,
        FAILING,
        FINISHING;
    }

    // Node outcome values
    public enum NodeOutcome {
        SUCCESS,
        FAILURE,
        SKIPPED,
        INTERRUPTED;
    }

    // Node failure type values
    public enum NodeFailureType {
        PRE_CONDITION_FAILED,
        POST_CONDITION_FAILED,
        INVARIANT_CONDITION_FAILED,
        PARENT_FAILED,
        EXITED,
        PARENT_EXITED;
    }

    // NodeRef directions
    public static final String PARENT = "parent";
    public static final String SIBLING = "sibling";
    public static final String CHILD = "child";
    public static final String SELF = "self";

}


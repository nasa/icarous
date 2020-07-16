/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

namespace PLEXIL
{
  char const *PLEXIL_PLAN_TAG = "PlexilPlan";
  char const *FILE_NAME_ATTR = "FileName";
  char const *LINE_NO_ATTR = "LineNo";
  char const *COL_NO_ATTR = "ColNo";

  char const *GLOBAL_DECLARATIONS_TAG = "GlobalDeclarations";
  char const *COMMAND_DECLARATION_TAG = "CommandDeclaration";
  char const *RETURN_TAG = "Return";
  char const *PARAMETER_TAG = "Parameter";
  char const *ANY_PARAMETERS_TAG = "AnyParameters";
  char const *RESOURCE_LIST_TAG = "ResourceList";
  char const *STATE_DECLARATION_TAG = "StateDeclaration";
  char const *LIBRARY_NODE_DECLARATION_TAG = "LibraryNodeDeclaration";

  // 'Noise words' added for use by analysis tools
  char const *ASSUME_TAG = "Assume";
  char const *DESIRE_TAG = "Desire";
  char const *EXPECT_TAG = "Expect";

  char const *NODE_TAG = "Node";
  char const *NODETYPE_ATTR = "NodeType";
  char const *NODEID_TAG = "NodeId";
  char const *COMMENT_TAG = "Comment";
  char const *PRIORITY_TAG = "Priority";

  char const *VAR_DECLS_TAG = "VariableDeclarations";
  char const *DECL_VAR_TAG = "DeclareVariable";
  char const *NAME_TAG = "Name";
  char const *TYPE_TAG = "Type";
  char const *INITIALVAL_TAG = "InitialValue";
  char const *DECL_ARRAY_TAG = "DeclareArray";
  char const *MAX_SIZE_TAG = "MaxSize";
  char const *ANY_VAL = "Any";

  char const *INTERFACE_TAG = "Interface";
  char const *IN_TAG = "In";
  char const *INOUT_TAG = "InOut";

  char const *BODY_TAG = "NodeBody";
  char const *ASSN_TAG = "Assignment";
  char const *RHS_TAG = "RHS";
  char const *LIBRARYNODECALL_TAG = "LibraryNodeCall";
  char const *ALIAS_TAG = "Alias";
  char const *NODE_PARAMETER_TAG = "NodeParameter";
  char const *NODELIST_TAG = "NodeList";

  // condition names
  char const *CONDITION_SUFFIX = "Condition";
  char const *START_CONDITION_TAG = "StartCondition";
  char const *REPEAT_CONDITION_TAG = "RepeatCondition";
  char const *PRE_CONDITION_TAG = "PreCondition";
  char const *POST_CONDITION_TAG = "PostCondition";
  char const *INVARIANT_CONDITION_TAG = "InvariantCondition";
  char const *END_CONDITION_TAG = "EndCondition";
  char const *EXIT_CONDITION_TAG = "ExitCondition";
  char const *SKIP_CONDITION_TAG = "SkipCondition";

  char const *NODEREF_TAG = "NodeRef";
  char const *DIR_ATTR = "dir";
  char const *PARENT_VAL = "parent";
  char const *CHILD_VAL = "child";
  char const *SIBLING_VAL = "sibling";
  char const *SELF_VAL = "self";

  char const *STATEVAL_TAG = "NodeStateValue";

  char const *TIMEPOINT_TAG = "Timepoint";
  char const *START_VAL = "START";
  char const *END_VAL = "END";

  char const *ARGS_TAG = "Arguments";
  char const *CMD_TAG = "Command";
  char const *LOOKUPNOW_TAG = "LookupNow";
  char const *LOOKUPCHANGE_TAG = "LookupOnChange";
  char const *PAIR_TAG = "Pair";
  char const *TOLERANCE_TAG = "Tolerance";
  char const *UPDATE_TAG = "Update";

  char const *ARRAYELEMENT_TAG = "ArrayElement";
  char const *INDEX_TAG = "Index";

  char const *ARRAY_VAL_TAG = "ArrayValue";
  char const *VAR_SUFFIX = "Variable";

}

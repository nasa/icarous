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

#ifndef PLEXIL_SCHEMA_HH
#define PLEXIL_SCHEMA_HH

namespace PLEXIL
{
  extern char const *PLEXIL_PLAN_TAG;
  extern char const *FILE_NAME_ATTR;
  extern char const *LINE_NO_ATTR;
  extern char const *COL_NO_ATTR;

  extern char const *GLOBAL_DECLARATIONS_TAG;
  extern char const *COMMAND_DECLARATION_TAG;
  extern char const *RETURN_TAG;
  extern char const *PARAMETER_TAG;
  extern char const *ANY_PARAMETERS_TAG;
  extern char const *RESOURCE_LIST_TAG;
  extern char const *STATE_DECLARATION_TAG;
  extern char const *LIBRARY_NODE_DECLARATION_TAG;

  // 'Noise words' added for use by analysis tools
  extern char const *ASSUME_TAG;
  extern char const *DESIRE_TAG;
  extern char const *EXPECT_TAG;

  extern char const *NODE_TAG;
  extern char const *NODETYPE_ATTR;
  extern char const *NODEID_TAG;
  extern char const *COMMENT_TAG;
  extern char const *PRIORITY_TAG;

  extern char const *VAR_DECLS_TAG;
  extern char const *DECL_VAR_TAG;
  extern char const *NAME_TAG;
  extern char const *TYPE_TAG;
  extern char const *INITIALVAL_TAG;
  extern char const *DECL_ARRAY_TAG;
  extern char const *MAX_SIZE_TAG;
  extern char const *ANY_VAL;

  extern char const *INTERFACE_TAG;
  extern char const *IN_TAG;
  extern char const *INOUT_TAG;

  extern char const *BODY_TAG;
  extern char const *ASSN_TAG;
  extern char const *RHS_TAG;
  extern char const *LIBRARYNODECALL_TAG;
  extern char const *ALIAS_TAG;
  extern char const *NODE_PARAMETER_TAG;
  extern char const *NODELIST_TAG;

  // condition names
  extern char const *CONDITION_SUFFIX;
  extern char const *START_CONDITION_TAG;
  extern char const *REPEAT_CONDITION_TAG;
  extern char const *PRE_CONDITION_TAG;
  extern char const *POST_CONDITION_TAG;
  extern char const *INVARIANT_CONDITION_TAG;
  extern char const *END_CONDITION_TAG;
  extern char const *EXIT_CONDITION_TAG;
  extern char const *SKIP_CONDITION_TAG;

  extern char const *NODEREF_TAG;
  extern char const *DIR_ATTR;
  extern char const *PARENT_VAL;
  extern char const *CHILD_VAL;
  extern char const *SIBLING_VAL;
  extern char const *SELF_VAL;

  extern char const *STATEVAL_TAG;

  extern char const *TIMEPOINT_TAG;
  extern char const *START_VAL;
  extern char const *END_VAL;

  // Interface tags
  extern char const *ARGS_TAG;
  extern char const *CMD_TAG;
  extern char const *LOOKUPNOW_TAG;
  extern char const *LOOKUPCHANGE_TAG;
  extern char const *PAIR_TAG;
  extern char const *TOLERANCE_TAG;
  extern char const *UPDATE_TAG;

  extern char const *ARRAYELEMENT_TAG;
  extern char const *INDEX_TAG;

  extern char const *ARRAY_VAL_TAG;
  extern char const *VAR_SUFFIX;

} // namespace PLEXIL

#endif // PLEXIL_SCHEMA_HH

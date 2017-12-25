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

package plexil;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class NodeStatePredicateNode extends ExpressionNode
{
	//
	// Constructors
	//
	public NodeStatePredicateNode(Token t)
	{
		super(t);
		this.setDataType(PlexilDataType.BOOLEAN_TYPE);
	}

	public NodeStatePredicateNode(NodeStatePredicateNode n)
	{
		super(n);
	}

	public Tree dupNode()
	{
		return new NodeStatePredicateNode(this);
	}

	public void earlyCheck(NodeContext context, CompilerState state)
	{
		// TODO: check that argument is a node ID or reference (??)
	}

    @Override
	public void checkChildren(NodeContext context, CompilerState state)
	{
		PlexilTreeNode child = this.getChild(0);
		switch (child.getToken().getType()) {
		case PlexilLexer.NCNAME: {
			// Check that argument is a unique, reachable node ID
            String nodeName = child.getToken().getText();
            PlexilTreeNode target = null;
            if (!context.isNodeIdReachable(nodeName)) {
                state.addDiagnostic(child,
                                    "No reachable node named \"" + nodeName + "\"",
                                    Severity.ERROR);
            }
            else if (!context.isNodeIdUnique(nodeName)) {
                state.addDiagnostic(child,
                                    "Node id \"" + nodeName + "\" is ambiguous",
                                    Severity.ERROR);
            }
        }
			break;

		case PlexilLexer.CHILD_KYWD:
		case PlexilLexer.PARENT_KYWD:
		case PlexilLexer.SELF_KYWD:
		case PlexilLexer.SIBLING_KYWD:
			// Check that argument is a valid node reference
			child.check(context, state);
			break;

		default:
			state.addDiagnostic(child,
								"Internal error: unexpected token type as argument to " + this.getToken().getText(),
								Severity.FATAL);
			break;
		}
	}

	protected String getXMLElementName()
	{
		switch (this.getToken().getType()) {

		case PlexilLexer.NODE_EXECUTING_KYWD:
			return "Executing";

		case PlexilLexer.NODE_FAILED_KYWD:
			return "Failed";

		case PlexilLexer.NODE_FINISHED_KYWD:
			return "Finished";

		case PlexilLexer.NODE_INACTIVE_KYWD:
			return "Inactive";

		case PlexilLexer.NODE_INVARIANT_FAILED_KYWD:
			return "InvariantFailed";

		case PlexilLexer.NODE_ITERATION_ENDED_KYWD:
			return "IterationEnded";

		case PlexilLexer.NODE_ITERATION_FAILED_KYWD:
			return "IterationFailed";

		case PlexilLexer.NODE_ITERATION_SUCCEEDED_KYWD:
			return "IterationSucceeded";

		case PlexilLexer.NODE_PARENT_FAILED_KYWD:
			return "ParentFailed";

		case PlexilLexer.NODE_POSTCONDITION_FAILED_KYWD:
			return "PostconditionFailed";

		case PlexilLexer.NODE_PRECONDITION_FAILED_KYWD:
			return "PreconditionFailed";

		case PlexilLexer.NODE_SKIPPED_KYWD:
			return "Skipped";

		case PlexilLexer.NODE_SUCCEEDED_KYWD:
			return "Succeeded";

		case PlexilLexer.NODE_WAITING_KYWD:
			return "Waiting";

		default:
			return "_INVALID_NODE_STATE_PREDICATE__" + this.getToken().getText();
		}
	}

	public void constructXML()
	{
		super.constructXML();

		PlexilTreeNode target = this.getChild(0);
		if (target.getToken().getType() == PlexilLexer.NCNAME) {
			IXMLElement nodeId = new XMLElement("NodeId");
			nodeId.setContent(target.getToken().getText());
			m_xml.addChild(nodeId);
		}
		else if (target instanceof NodeRefNode) {
			m_xml.addChild(target.getXML());
		}
		else {
			m_xml.addChild(new XMLElement("_NODE_REF_ERROR_"));
		}
	}

}

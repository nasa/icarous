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

public class NodeRefNode extends PlexilTreeNode
{
	public NodeRefNode(Token t)
	{
		super(t);
	}

	public NodeRefNode(NodeRefNode n)
	{
		super(n);
	}

	public Tree dupNode()
	{
		return new NodeRefNode(this);
	}

    @Override
	public void earlyCheck(NodeContext context, CompilerState state)
	{
        // Nothing to check
	}

	public void check(NodeContext context, CompilerState state)
	{
		switch (this.getToken().getType()) {
		case PlexilLexer.CHILD_KYWD: 
			{
				String childName = this.getChild(0).getToken().getText();
				if (!context.isChildNodeId(childName)) {
					state.addDiagnostic(this.getChild(0),
										"Node has no child named \"" + childName + "\"",
										Severity.ERROR);
				}
			}
			break;

		case PlexilLexer.PARENT_KYWD:
			// Check whether we have a parent
			if (context.isRootContext()) {
				state.addDiagnostic(this,
									"Node has no parent",
									Severity.ERROR);
			}
			break;

		case PlexilLexer.SELF_KYWD:
			// no action necessary
			break;

		case PlexilLexer.SIBLING_KYWD:
			// Check whether we have a parent
			if (context.isRootContext()) {
				state.addDiagnostic(this,
									"Node has no parent and therefore no siblings",
									Severity.ERROR);
			}
			else {
				// Check whether the named sibling exists
				NodeContext parent = context.getParentContext();
				String siblingName = this.getChild(0).getToken().getText();
				if (!parent.isChildNodeId(siblingName)) {
					state.addDiagnostic(this.getChild(0),
										"Node has no sibling named \"" + siblingName + "\"",
										Severity.ERROR);
				}
			}
			break;

		default:
			state.addDiagnostic(this,
								"Internal error: unexpected token \"" + this.getToken().getText() + "\" in a NodeRef",
								Severity.FATAL);
			break;
		}
	}

	private String directionString()
	{
		switch (this.getToken().getType()) {
		case PlexilLexer.CHILD_KYWD:
			return "child";

		case PlexilLexer.PARENT_KYWD:
			return "parent";

		case PlexilLexer.SELF_KYWD:
			return "self";

		case PlexilLexer.SIBLING_KYWD:
			return "sibling";

		default:
			return "_NODE_REF_ERROR_";
		}
	}

	protected void constructXML()
	{
		m_xml = new XMLElement("NodeRef");
		m_xml.setAttribute("dir", directionString());

		// Set content if required
		switch (this.getToken().getType()) {
		case PlexilLexer.CHILD_KYWD:
		case PlexilLexer.SIBLING_KYWD:
			m_xml.setContent(this.getChild(0).getText());
			break;

		default:
			break;
		}
	}

}

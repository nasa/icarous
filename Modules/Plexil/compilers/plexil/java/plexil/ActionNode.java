// Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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

public class ActionNode extends PlexilTreeNode
{
    protected String m_nodeId = null;

    //
    // Constructors
    //

    public ActionNode(Token t)
    {
        super(t);
    }

	public ActionNode(ActionNode n)
	{
		super(n);
		m_nodeId = n.m_nodeId;
	}

    public boolean hasNodeId()
    {
        return this.getChild(0).getType() == PlexilLexer.NCNAME;
    }

    public String getNodeId()
    {
        return m_nodeId; 
    }

	public Tree dupNode()
	{
		return new ActionNode(this);
	}

    public void earlyCheckSelf(NodeContext context, CompilerState state)
    {
        // If supplied, get the node ID
        PlexilTreeNode firstChild = this.getChild(0);
        if (firstChild.getType() == PlexilLexer.NCNAME) {
            m_nodeId = firstChild.getText();
            // Check that node ID is locally unique
            if (context.isChildNodeId(m_nodeId)) {
                state.addDiagnostic(firstChild,
                                    "Node ID \"" + m_nodeId + "\" defined more than once in this context",
                                    Severity.ERROR);
                state.addDiagnostic(context.getChildNodeId(m_nodeId),
                                    "Original definition of node ID \"" + m_nodeId + "\" is here",
                                    Severity.NOTE);
            }
            else {
                context.addChildNodeId(firstChild);
            }
        }
        else {
            // Gensym a name but don't log it, since it never appeared in the source
            m_nodeId = context.generateChildNodeName(firstChild.getToken().getText());
        }
    }

    protected void constructXML()
    {
        // Get XML from last child
        PlexilTreeNode child = this.getChild(this.getChildCount() - 1);
        m_xml = child.getXML();
        // Insert Node ID element
        IXMLElement nodeIdElt = new XMLElement("NodeId");
        nodeIdElt.setContent(m_nodeId);
        ((XMLElement) m_xml).insertChild(nodeIdElt, 0);
    }

}


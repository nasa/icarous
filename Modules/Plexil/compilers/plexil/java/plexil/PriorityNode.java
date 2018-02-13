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
import org.antlr.runtime.Token;
import net.n3.nanoxml.*;

public class PriorityNode extends PlexilTreeNode
{
    public PriorityNode(Token t)
    {
        super(t);
    }

    public PriorityNode(PriorityNode n)
    {
        super(n);
    }

	public Tree dupNode()
	{
		return new PriorityNode(this);
	}

    public void check(NodeContext context, CompilerState state)
    {
        super.check (context, state);
        int p = Integer.parseInt (this.getChild(0).getText());
        if (p < 0) {
            state.addDiagnostic(this,
                                "Priority value is negative; must be non-negative",
                                Severity.ERROR);
        }
    }

    protected void addSourceLocatorAttributes()
    {
        // Don't.
    }

    public void constructXML()
    {
        super.constructXML();
        m_xml.setContent (this.getChild(0).getText());
    }

    public String getXMLElementName()
    {
        return "Priority";
    }

}

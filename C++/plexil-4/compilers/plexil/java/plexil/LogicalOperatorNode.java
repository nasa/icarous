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

public class LogicalOperatorNode extends ExpressionNode
{
    public LogicalOperatorNode(Token t)
    {
        super(t);
        m_dataType = PlexilDataType.BOOLEAN_TYPE;
    }

    public LogicalOperatorNode(LogicalOperatorNode n)
    {
        super(n);
    }

	public Tree dupNode()
	{
		return new LogicalOperatorNode(this);
	}

    // May have 1 or more args; all must be boolean
    public void check(NodeContext context, CompilerState myState)
    {
        for (int i = 0; i < this.getChildCount(); i++) {
            ExpressionNode operand = (ExpressionNode) this.getChild(i);
            if (!operand.assumeType(PlexilDataType.BOOLEAN_TYPE, myState)) {
                myState.addDiagnostic(operand,
                                      "The operand to the " + this.getToken().getText() + " operator is not Boolean",
                                      Severity.ERROR);
            }

            // run semantic checks on operand even if it failed type check
            operand.check(context, myState);
        }
    }

    public void constructXML()
    {
        super.constructXML();
        for (int i = 0; i < this.getChildCount(); i++) {
            m_xml.addChild(this.getChild(i).getXML());
        }
    }

    public String getXMLElementName()
    {
        switch (this.getType()) {
            // these have synonyms which don't match the required name
        case PlexilLexer.AND_KYWD:
            return "AND";
			
        case PlexilLexer.OR_KYWD:
            return "OR";

        case PlexilLexer.NOT_KYWD:
            return "NOT";

        default:
            return this.getToken().getText();
        }
    }

}
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

public class ConditionNode extends PlexilTreeNode
{

    public ConditionNode(Token t)
    {
        super(t);
    }

    public ConditionNode(ConditionNode n)
    {
        super(n);
    }

	public Tree dupNode()
	{
		return new ConditionNode(this);
	}

    public void checkSelf(NodeContext context, CompilerState myState)
    {
        ExpressionNode exp = (ExpressionNode) this.getChild(0);
        if (exp.getDataType() != PlexilDataType.BOOLEAN_TYPE) {
            myState.addDiagnostic(exp,
                                  this.getToken().getText() + " expression type is "
								  + exp.getDataType().typeName() + ", not Boolean",
                                  Severity.ERROR);
        }
    }

    public void constructXML()
    {
        super.constructXML();
        // Add expression
        m_xml.addChild(this.getChild(0).getXML());
    }

    public String getXMLElementName()
    {
        switch (this.getType()) {
        case PlexilLexer.END_CONDITION_KYWD:
            return "EndCondition";

        case PlexilLexer.EXIT_CONDITION_KYWD:
            return "ExitCondition";

        case PlexilLexer.INVARIANT_CONDITION_KYWD:
            return "InvariantCondition";

        case PlexilLexer.POST_CONDITION_KYWD:
            return "PostCondition";

        case PlexilLexer.PRE_CONDITION_KYWD:
            return "PreCondition";

        case PlexilLexer.REPEAT_CONDITION_KYWD:
            return "RepeatCondition";

        case PlexilLexer.SKIP_CONDITION_KYWD:
            return "SkipCondition";

        case PlexilLexer.START_CONDITION_KYWD:
            return "StartCondition";

        default:
            return null;
        }
    }

}

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

import java.util.Vector;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

public class ArgumentListNode extends PlexilTreeNode
{
    public ArgumentListNode(Token t)
    {
        super(t);
    }

	public ArgumentListNode(ArgumentListNode n)
	{
		super(n);
	}

	public Tree dupNode()
	{
		return new ArgumentListNode(this);
	}

    public void earlyCheckArgumentList(NodeContext context,
                                       CompilerState state, 
                                       String callType,
                                       String callName,
                                       Vector<VariableName> paramSpec)
    {
        // Check number of arguments only
        boolean wildcardSeen = false;
        for (int i = 0; i < this.getChildCount(); i++) {
            if (i >= paramSpec.size()) {
                state.addDiagnostic(this.getChild(i),
                                    callType + " \"" + callName +
                                    "\" expects " + Integer.toString(paramSpec.size())
                                    + " parameters, but " + Integer.toString(this.getChildCount()) 
                                    + " were supplied",
                                    Severity.ERROR);
                break; // no point in checking further
            }
            if (paramSpec.elementAt(i) instanceof WildcardVariableName) {
                wildcardSeen = true;
                break; // no need to check further
            }
            if (paramSpec.size() < this.getChildCount()
                && !wildcardSeen) {
                state.addDiagnostic(this.getChild(i),
                                    callType + " \"" + callName +
                                    "\" expects " + Integer.toString(paramSpec.size())
                                    + " parameters, but " + Integer.toString(this.getChildCount()) 
                                    + " were supplied",
                                    Severity.ERROR);
                break; // no need to check further
            }
            this.getChild(i).earlyCheck(context, state); // for effect
        }
    }

    public void checkArgumentList(NodeContext context,
                                  CompilerState state, 
                                  String callType,
                                  String callName,
                                  Vector<VariableName> paramSpec)
    {
        // Check types of arguments only
        for (int i = 0; i < this.getChildCount(); i++) {
            if (i >= paramSpec.size()
                || paramSpec.elementAt(i) instanceof WildcardVariableName) 
                break; // no further checking possible or needed

            PlexilDataType reqdType = paramSpec.elementAt(i).getVariableType();
            ExpressionNode parm = (ExpressionNode) this.getChild(i);
            parm.check(context, state);
            if (!parm.assumeType(reqdType, state)) {
                state.addDiagnostic(parm,
                                    "Parameter " + Integer.toString(i+1)
                                    + " to " + callType + " \"" + callName
                                    + "\" is not of the expected type "
                                    + reqdType.typeName(),
                                    Severity.ERROR);
            }
            // TODO: array size checks
        }
    }

    public void constructXML()
    {
        super.constructXML();
        for (int i = 0; i < this.getChildCount(); i++) 
            m_xml.addChild(this.getChild(i).getXML());
    }

    public String getXMLElementName() { return "Arguments"; }

}

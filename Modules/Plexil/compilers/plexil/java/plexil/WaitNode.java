// Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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

class WaitNode extends PlexilTreeNode
{
    public WaitNode(Token t)
    {
        super(t);
    }

    public WaitNode(WaitNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new WaitNode(this);
    }

//  N.B. Refactor the following with SynchronousCommandNode.java!

    public void check (NodeContext context, CompilerState state)
    {
        ExpressionNode delayExp = (ExpressionNode) this.getChild(0);
        delayExp.check (context, state);
        if (delayExp.assumeType(PlexilDataType.DURATION_TYPE, state)) {
            checkForDuration (context, state);
        }
        else if (delayExp.assumeType (PlexilDataType.REAL_TYPE, state) ||
                 delayExp.assumeType (PlexilDataType.INTEGER_TYPE, state)) {
            checkForReal (context, state);
        }
        else state.addDiagnostic(delayExp,
                                 "The delay argument to the Wait builtin, \""
                                 + delayExp.getText()
                                 + "\", is not a duration or number",
                                 Severity.ERROR);
    }
    

    private void checkForDuration (NodeContext context, CompilerState state)
    {
        if (this.getChildCount() > 1) {
            ExpressionNode toleranceExp = (ExpressionNode) this.getChild(1);
            if (toleranceExp instanceof LiteralNode
                && toleranceExp.assumeType(PlexilDataType.DURATION_TYPE, state)) {
                // it's good
            }
            else if (toleranceExp instanceof VariableNode
                     // simple variable reference:
                     && toleranceExp.getType() == PlexilLexer.NCNAME 
                     && toleranceExp.getDataType() == PlexilDataType.DURATION_TYPE) {
                // that's good too
            }
            else {
                state.addDiagnostic(toleranceExp,
                                    "The tolerance argument to the Wait builtin, \""
                                    + toleranceExp.getText()
                                    + "\", is not a Duration value or variable.",
                                    Severity.ERROR);
            }
            // check the delay expression for other faults
            toleranceExp.check(context, state);
        }
    }


    private void checkForReal (NodeContext context, CompilerState state)
    {
        if (this.getChildCount() > 1) {
            ExpressionNode toleranceExp = (ExpressionNode) this.getChild(1);
            if (toleranceExp instanceof LiteralNode
                && toleranceExp.assumeType(PlexilDataType.REAL_TYPE, state)) {
                // it's good
            }
            else if (toleranceExp instanceof VariableNode
                     // simple variable reference:
                     && toleranceExp.getType() == PlexilLexer.NCNAME 
                     && toleranceExp.getDataType() == PlexilDataType.REAL_TYPE) {
                // that's good too
            }
            else {
                state.addDiagnostic(toleranceExp,
                                    "The tolerance argument to the Wait builtin, \""
                                    + toleranceExp.getText()
                                    + "\", is not a Real value or variable.",
                                    Severity.ERROR);
            }
            // check the delay expression for other faults
            toleranceExp.check(context, state);
        }
    }

    protected void constructXML()
    {
        super.constructXML();
        IXMLElement unitsElt = new XMLElement("Units");
        m_xml.addChild(unitsElt);
        unitsElt.addChild(this.getChild(0).getXML());
        if (this.getChildCount() > 1) {
            IXMLElement toleranceElt = new XMLElement("Tolerance");
            m_xml.addChild(toleranceElt);
            toleranceElt.addChild(this.getChild(1).getXML());
        }
    }

    protected String getXMLElementName() { return "Wait"; }

}

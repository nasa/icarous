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

public class SynchronousCommandNode extends PlexilTreeNode
{
    public SynchronousCommandNode(Token t)
    {
        super(t);
    }

    public SynchronousCommandNode(SynchronousCommandNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new SynchronousCommandNode(this);
    }

    //
    // Format is
    // (SYNCHRONOUS_COMMAND_KYWD (assignment | commandInvocation) (expression expression?)? )

    //  N.B. Refactor the following with WaitNode.java!

    public void check (NodeContext context, CompilerState state)
    {
        if (this.getChildCount() > 1) {
            ExpressionNode timeout = (ExpressionNode) this.getChild(1);
            timeout.check (context, state);

            if (timeout.assumeType(PlexilDataType.DURATION_TYPE, state)) {
                checkForDuration (context, state);
            }
            else if (timeout.assumeType (PlexilDataType.REAL_TYPE, state) ||
                     timeout.assumeType (PlexilDataType.INTEGER_TYPE, state)) {
                checkForReal (context, state);
            }
            else state.addDiagnostic(timeout,
                                     "The timeout argument to SynchronousCommand, \""
                                     + timeout.getText()
                                     + "\", is not a duration or number",
                                     Severity.ERROR);
            this.checkChildren(context, state); // Necessary? (copied from old code)
        }
    }

    private void checkForDuration (NodeContext context, CompilerState state)
    {
        if (this.getChildCount() > 2) {
            ExpressionNode toleranceExp = (ExpressionNode) this.getChild(2);
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
                                    "The tolerance argument to SynchronousCommand, \""
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
        if (this.getChildCount() > 2) {
            ExpressionNode toleranceExp = (ExpressionNode) this.getChild(2);
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
                                    "The tolerance argument to SynchronousCommand, \""
                                    + toleranceExp.getText()
                                    + "\", is not a Real value or variable.",
                                    Severity.ERROR);
            }
            // check the delay expression for other faults
            toleranceExp.check(context, state);
        }
    }


    public void constructXML()
    {
        super.constructXML();

        // Generate XML for timeout if supplied
        if (this.getChildCount() > 1) {
            IXMLElement timeoutXML = new XMLElement("Timeout");
            m_xml.addChild(timeoutXML);
            timeoutXML.addChild(this.getChild(1).getXML());
            if (this.getChildCount() > 2) {
                IXMLElement tolXML = new XMLElement("Tolerance");
                m_xml.addChild(tolXML);
                tolXML.addChild(this.getChild(2).getXML());
            }
        }
		
        // Construct command XML by extracting from command node XML
        IXMLElement commandNodeXML = this.getChild(0).getXML();
        // command is inside NodeBody element
        IXMLElement commandXML = commandNodeXML.getFirstChildNamed("NodeBody").getChildAtIndex(0);
        m_xml.addChild(commandXML);
    }

}


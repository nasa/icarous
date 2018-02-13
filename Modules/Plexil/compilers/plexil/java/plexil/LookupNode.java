// Copyright (c) 2006-2013, Universities Space Research Association (USRA).
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

import net.n3.nanoxml.*;

public class LookupNode extends ExpressionNode
{
    private ArgumentListNode m_arguments = null;
    private ExpressionNode m_tolerance = null;
    private GlobalDeclaration m_state = null;

    public LookupNode(Token t)
    {
        super(t);
    }

    public LookupNode(LookupNode n)
    {
        super(n);
        m_arguments = n.m_arguments;
        m_tolerance = n.m_tolerance;
        m_state = n.m_state;
    }

    public Tree dupNode()
    {
        return new LookupNode(this);
    }

    // N.B. Only valid after earlyCheck().
    public GlobalDeclaration getState()
    {
        return m_state;
    }

    //
    // LookupNow is (LOOKUP_KYWD lookupNameExp argumentList?)
    // LookupOnChange is (LOOKUP_ON_CHANGE_KYWD lookupNameExp argumentList? tolerance?)
    // Lookup is (LOOKUP_KYWD lookupNameExp argumentList? tolerance?)
    //
    // lookupNameExp is (STATE_NAME NCNAME) or expression
    // argument list is (ARGUMENT_LIST expression*)
    // tolerance is REAL or NCNAME -> variable
    //

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        // Break down into subexpressions
        if (this.getChildCount() > 1) {
            PlexilTreeNode secondKid = this.getChild(1);
            if (secondKid.getType() == PlexilLexer.ARGUMENT_LIST) {
                m_arguments = (ArgumentListNode) secondKid;
                m_tolerance = (ExpressionNode) this.getChild(2);
            }
            else 
                m_tolerance = (ExpressionNode) secondKid;
        }

        if (m_tolerance != null) {
            // Enforce variable reference or literal restriction
            if (!(m_tolerance instanceof LiteralNode || m_tolerance instanceof VariableNode)) {
                state.addDiagnostic(m_tolerance,
                                    "Tolerance argument to " + this.getToken().getText()
                                    + " must be a variable reference or a literal",
                                    Severity.ERROR);
            }
            // Do additional checks
            m_tolerance.earlyCheck(context, state);
        }

        PlexilTreeNode invocation = this.getChild(0);
        if (invocation.getType() == PlexilLexer.STATE_NAME) {
            // Do additional checking if state name is a literal
            String stateName = invocation.getChild(0).getText();
            m_state = GlobalContext.getGlobalContext().getLookupDeclaration(stateName);
            if (m_state == null) {
                // FIXME: should this be an error instead?
                state.addDiagnostic(invocation.getChild(0),
                                    "State name \"" + stateName + "\" has not been declared",
                                    Severity.WARNING);
                // FIXME: add implicit declaration?
                m_dataType = PlexilDataType.ANY_TYPE;
            }
            else {
                // Set return value type
                m_dataType = m_state.getReturnType();

                // Check arglist
                Vector<VariableName> argSpecs = m_state.getParameterVariables();
                if (argSpecs == null) {
                    if (m_arguments != null) {
                        state.addDiagnostic(invocation,
                                            "State \"" + stateName + "\" requires 0 parameters, but "
                                            + Integer.toString(m_arguments.getChildCount()) + " were supplied",
                                            Severity.ERROR);
                    }
                    // else nothing needs to be done
                }
                else {
                    // State takes parameters
                    if (m_arguments == null) {
                        state.addDiagnostic(invocation,
                                            "State \"" + stateName + "\" requires "
                                            + Integer.toString(argSpecs.size())
                                            + " parameters, but none were supplied",
                                            Severity.ERROR);
                    }
                    // Check arg count
                    else {
                        m_arguments.earlyCheckArgumentList(context,
                                                           state,
                                                           "State",
                                                           stateName,
                                                           argSpecs);
                    }
                }
            } // end state is declared
        } // end state is a literal
        else {
            // no way to tell what type is, have to depend on context
            m_dataType = PlexilDataType.ANY_TYPE;
        }

        // Do recursive checks as well
        this.earlyCheckChildren(context, state);
    }

    public void check(NodeContext context, CompilerState state)
    {
        if (m_state != null) {
            // Check arglist
            // Formal vs. actual counts have already been done in checkSelf()
            Vector<VariableName> argSpecs = m_state.getParameterVariables();
            if (argSpecs != null
                && m_arguments != null) {
                // Check arg types
                m_arguments.checkArgumentList(context,
                                              state,
                                              "state",
                                              m_state.getName(),
                                              argSpecs);
            }

            // Type check tolerance if supplied and if state name is known
            if (m_tolerance != null) {
                if (m_dataType == PlexilDataType.DATE_TYPE &&
                    !m_tolerance.assumeType (PlexilDataType.DURATION_TYPE, state)) {
                    state.addDiagnostic(m_tolerance,
                                        "Tolerance supplied for state \"" + m_state.getName()
                                        + "\" has type " + m_tolerance.getDataType().typeName()
                                        + ", instead of Duration type ", Severity.ERROR);
                }
                else if (m_dataType.isNumeric() && !m_tolerance.assumeType (m_dataType, state)) {
                    state.addDiagnostic (m_tolerance,
                                         "Tolerance supplied for state \"" + m_state.getName()
                                         + "\" has type " + m_tolerance.getDataType().typeName()
                                         + ", which doesn't match the lookup's type " +
                                         m_dataType.typeName(), Severity.ERROR);
                }
            }
        }
        // Do recursive checks
        this.checkChildren(context, state);
    }

    protected void constructXML()
    {
        super.constructXML();

        // Add state
        IXMLElement nameXML = new XMLElement("Name");
        m_xml.addChild(nameXML);
        if (this.getChild(0).getType() == PlexilLexer.STATE_NAME) {
            // literal name
            IXMLElement literalNameXML = new XMLElement("StringValue");
            nameXML.addChild(literalNameXML);
            literalNameXML.setContent(this.getChild(0).getChild(0).getText());
        }
        else // name expression
            nameXML.addChild(this.getChild(0).getXML());

        // Add tolerance
        if (m_tolerance != null) {
            IXMLElement tolXML = new XMLElement("Tolerance");
            m_xml.addChild(tolXML);
            tolXML.addChild(m_tolerance.getXML());
        }

        // Add parameters
        if (m_arguments != null) {
            m_xml.addChild(m_arguments.getXML());
        }

    }


}
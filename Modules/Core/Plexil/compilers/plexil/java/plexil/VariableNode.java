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

public class VariableNode extends ExpressionNode
{
    private VariableName m_variable = null;

    public VariableNode(Token t)
    {
        super(t);
    }

    public VariableNode(VariableNode n)
    {
        super(n);
		m_variable = n.m_variable;
    }

	public Tree dupNode()
	{
		return new VariableNode(this);
	}

    public VariableName getVariableName()
    {
        return m_variable;
    }

    /**
     * @brief Prepare for the semantic check.
     */
    @Override
    public void earlyCheckSelf(NodeContext context, CompilerState state)
    {
        // Get variable from context, if possible
        m_variable = context.findVariable(this.getText());
        if (m_variable == null) {
            state.addDiagnostic(this,
                                "Variable \"" + this.getText() + "\" is not declared",
                                Severity.ERROR);
        }
        else {
            m_dataType = m_variable.getVariableType();
        }
    }

    protected void constructXML()
    {
        super.constructXML();
        m_xml.setContent(this.getText());
    }

    protected String getXMLElementName()
    {
        return m_dataType.typeName() + "Variable";
    }

    // Source locators are not allowed on variable elements.
    protected void addSourceLocatorAttributes() {}

    public boolean isAssignable()
    {
        if (m_variable == null)
            return true; // no way to know
        else 
            return m_variable.isAssignable();
    }

    public void checkAssignable(NodeContext context, CompilerState state)
    {
        if (m_variable == null
            || m_variable.isAssignable())
            return;
        // we have a variable and it's not assignable
        state.addDiagnostic(this,
                            "Variable \"" + this.getText() + "\" is declared In",
                            Severity.ERROR);
        state.addDiagnostic(m_variable.getDeclaration(),
                            "Variable \"" + this.getText() + "\" declared In here",
                            Severity.NOTE);
    }

    public PlexilTreeNode getDeclaration() 
    {
        if (m_variable == null)
            return null;
        return m_variable.getDeclaration();
    }

}

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

import java.util.Vector;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class OnCommandNode extends PlexilTreeNode
{
    // Name binding context
	NodeContext m_context = null;

    public OnCommandNode(Token t)
    {
        super(t);
    }

    public OnCommandNode(OnCommandNode n)
    {
        super(n);
		m_context = n.m_context;
    }

	public Tree dupNode()
	{
		return new OnCommandNode(this);
	}

    /**
     * @brief Get the containing name binding context for this branch of the parse tree.
     * @return A NodeContext instance, or the global context.
     */
    public NodeContext getContext()
    {
        return m_context;
    }

	// structure is:
	// ^(ON_COMMAND_KYWD expression paramsSpec? action)

	public void earlyCheck(NodeContext parentContext, CompilerState state)
	{
		// Check command name expression
		getChild(0).earlyCheck(parentContext, state);

        // get node ID
        String nodeId = null;
        PlexilTreeNode parent = this.getParent();
        if (parent != null && parent instanceof ActionNode) {
            nodeId = ((ActionNode) parent).getNodeId();
        }
        else {
            // should never happen
            state.addDiagnostic(this,
                                "Internal error: OnCommandNode instance has no parent ActionNode",
                                Severity.FATAL);
        }

		// Construct a context
		// Generated context name includes the command name
		m_context = new NodeContext(parentContext, nodeId + "_ON_COMMAND");

        // Parse parameter list, if supplied
        ParameterSpecNode parmAST = getParameters();
        if (parmAST != null) {
            parmAST.earlyCheck(m_context, state); // for effect
            Vector<VariableName> parmSpecs = parmAST.getParameterVector();
            if (parmSpecs != null) {
                for (VariableName vn : parmSpecs) {
					// Check that it is not an interface variable
					if (!vn.isLocal()) {
						state.addDiagnostic(vn.getDeclaration(),
											"Parameter \"" + vn.getName() + "\" to OnCommand was declared " +
											(vn.isAssignable() ? "InOut" : "In"),
											Severity.ERROR);
					}
					// add to context
					// TODO: added checks (e.g. duplicate names)?
					m_context.addVariable(vn);
				}
			}
		}

		// Check body with parameter variables defined
		getBody().earlyCheck(m_context, state);
	}

	public void check(NodeContext parentContext, CompilerState state)
	{
		checkSelf(parentContext, state);
		getChild(0).check(parentContext, state); // name expression
		// Check parameter list
		ParameterSpecNode specs = getParameters();
		if (specs != null)
			specs.check(parentContext, state);
		getBody().check(m_context, state);
	}

    public void checkSelf(NodeContext context, CompilerState state)
    {
		// Coerce name expression to string
        ExpressionNode nameExp = (ExpressionNode) this.getChild(0);
        if (!nameExp.assumeType(PlexilDataType.STRING_TYPE, state)) {
            state.addDiagnostic(nameExp,
								"The name expression to the " + this.getToken().getText()
								+ " statement was not a string expression",
								Severity.ERROR);
		}
    }

    public void constructXML()
    {
        super.constructXML();

        // Construct the name element, but don't output it yet.
        IXMLElement name = new XMLElement ("Name");
		name.addChild(this.getChild(0).getXML());

        // Second child: parameters (optional), or action
        ParameterSpecNode parms = getParameters();
		Vector<VariableName> parmSpecs = null;
        if (parms == null
			|| (parmSpecs = parms.getParameterVector()) == null) {
			// No parameter declarations
            m_xml.addChild(name);
            m_xml.addChild(getBody().getXML());
        } 
		else {
            // Parameters are output first, followed by name
			IXMLElement decls = new XMLElement("VariableDeclarations");
			for (VariableName vn : parmSpecs) {
				decls.addChild(vn.makeDeclarationXML());
			}
            m_xml.addChild(decls);
            m_xml.addChild(name);
            m_xml.addChild(getBody().getXML());
        }
    }

    protected ParameterSpecNode getParameters()
    {
        if (this.getChild(1).getType() != PlexilLexer.PARAMETERS)
            return null;
        return (ParameterSpecNode) this.getChild(1);
    }

	protected PlexilTreeNode getBody()
	{
		if (this.getChild(1).getType() == PlexilLexer.PARAMETERS)
			return (PlexilTreeNode) this.getChild(2);
		return (PlexilTreeNode) this.getChild(1);
	}

}


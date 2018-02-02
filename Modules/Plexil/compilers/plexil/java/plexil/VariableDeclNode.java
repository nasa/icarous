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

public class VariableDeclNode extends PlexilTreeNode
{
    protected VariableName m_variable = null;

    // for use by derived classes
    public VariableDeclNode(Token t)
    {
        super(t);
    }

	public VariableDeclNode(VariableDeclNode n)
	{
		super(n);
		m_variable = n.m_variable;
	}

	public Tree dupNode()
	{
		return new VariableDeclNode(this);
	}

    // Required by (e.g.) ForNode code generation
    public VariableName getVariableName() { return m_variable; }

    // Required by interface variable parsing
    public void setVariableName(VariableName v) { m_variable = v; }

    //
    // Next several utilities are required by InterfaceDeclNode as well as internally
    // 

    // Works for ArrayVariableDeclNode too.
    public PlexilTreeNode getNameNode()
    { 
        return this.getChild(1); 
    }

    public PlexilDataType getVariableType()
    {
        return PlexilDataType.findByName(this.getChild(0).getText());
    }

    public ExpressionNode getInitialValueNode()
    {
        if (this.getChildCount() > 2)
            return (ExpressionNode) this.getChild(2);
        else 
            return null;
    }

    // Various places expect the variable to be defined early
    public void earlyCheck(NodeContext context, CompilerState state)
    {
        earlyCheckCommon(context, state);	
        // Check for name conflict (issues diagnostics on failure)
        PlexilTreeNode nameNode = getNameNode();
        if (context.checkVariableName(nameNode))
            m_variable = context.addVariable(this, 
                                             nameNode,
                                             getVariableType(),
                                             getInitialValueNode());
    }

    // Also called by InterfaceDeclNode
    public void earlyCheckCommon(NodeContext context, CompilerState state)
    {
        // Check that type is valid - should be enforced by parser already
        if (getVariableType() == null) {
            state.addDiagnostic(this.getChild(0),
                                "Internal error: \"" + this.getChild(0).getText() + "\" is not a valid type name",
                                Severity.FATAL);
        }

        ExpressionNode initValNode = null;
        if (this.getChildCount() > 2)
            initValNode = (ExpressionNode) this.getChild(2);

        if (this.getChildCount() > 2)
            initValNode.earlyCheck(context, state);
    }

    /**
     * @brief Override PlexilTreeNode.check
     * @return true if check is successful, false otherwise.
     */
    public void check(NodeContext context, CompilerState state)
    {
        // If supplied, check initial value for type conflict
        ExpressionNode initValNode = null;
        if (this.getChildCount() > 2) {
            // check initial value for type conflict
            // N.B. we assume this is a LiteralNode,
            // but ExpressionNode supports the required method
            // and allows the syntax to be generalized later.
            initValNode = (ExpressionNode) this.getChild(2);
            PlexilDataType type = getVariableType();
            // FIXME: any chance initValNode could be null?
            PlexilDataType initType = initValNode.getDataType();
            // Allow integer initial val for real var (but not the other way around)
            if (initType == type ||
                (type == PlexilDataType.REAL_TYPE
                 && initType.isNumeric()
                 // N.B. side effect on initial value type!
                 && initValNode.assumeType(type, state)) ||
                (type == PlexilDataType.BOOLEAN_TYPE
                 // N.B. side effect on initial value type!
                 && initValNode.assumeType(type, state))
                // N.B. Need to learn how to cast a string initializer to date/duration
                // (type.isTemporal() && initType == PlexilDataType.STRING_TYPE)
                ) {
                // initial value type is consistent
            }
            else {
                state.addDiagnostic(initValNode,
                                    "For variable \"" + this.getChild(1).getText()
                                    + "\": Initial value type " + initType.typeName()
                                    + " is incompatible with variable type " + type.typeName(),
                                    Severity.ERROR);
            }
            // now that correct type is enforced, check it
            initValNode.check(context, state);
        }
    }

}
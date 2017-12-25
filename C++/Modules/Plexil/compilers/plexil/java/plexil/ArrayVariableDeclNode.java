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

public class ArrayVariableDeclNode extends VariableDeclNode
{

    public ArrayVariableDeclNode(Token t)
    {
        super(t);
    }

    public ArrayVariableDeclNode(ArrayVariableDeclNode n)
    {
        super(n);
    }

	public Tree dupNode()
	{
		return new ArrayVariableDeclNode(this);
	}

    public PlexilDataType getVariableType()
    {
        PlexilDataType eltType = PlexilDataType.findByName(this.getChild(0).getText());
        if (eltType == null)
            return null;
        else
            return eltType.arrayType();
    }

    public LiteralNode getArraySizeNode()
    {
        return (LiteralNode) this.getChild(2);
    }

    public int getArraySize()
    {
        return LiteralNode.parseIntegerValue(this.getChild(2).getText());
    }

    public ExpressionNode getInitialValueNode()
    {
        if (this.getChildCount() > 3)
            return (ExpressionNode) this.getChild(3);
        else
            return null;
    }

    // Various places expect the variable to be defined early
    public void earlyCheck(NodeContext context, CompilerState state)
    {
        earlyCheckCommon(context, state);

        // Check for name conflict (issues diagnostics on failure)
        // and define the variable if no conflict found
        PlexilTreeNode nameNode = getNameNode();
        if (context.checkVariableName(nameNode))
            m_variable = context.declareArrayVariable(this,
                                                      nameNode,
                                                      getVariableType(),
                                                      getArraySizeNode().getText(),
                                                      getInitialValueNode());
    }

    // Also called by InterfaceDeclNode
    public void earlyCheckCommon(NodeContext context, CompilerState state)
    {
        // N.B. type is restricted syntactically so *should be* no chance of error here
        // Check that type is valid - should be enforced by parser already
        PlexilDataType arrayType = getVariableType();
        if (arrayType == null) {
            state.addDiagnostic(this.getChild(0),
                                "Internal error: \"" + this.getChild(0).getText() + "\" is not a valid type name",
                                Severity.FATAL);
        }

        // Check max size for non-negative integer
        int size = getArraySize();
        if (size < 0) {
            state.addDiagnostic(getArraySizeNode(),
                                "For array variable \"" + getNameNode().getText()
                                + "\": size " + getArraySizeNode().getText() + " is negative",
                                Severity.ERROR);
        }

        ExpressionNode initValNode = getInitialValueNode();
        if (initValNode != null) 
            initValNode.earlyCheck(context, state);
    }

    // format is (ARRAY_VARIABLE_DECLARATION element-type NCNAME INT initialValue?)
    public void check(NodeContext context, CompilerState state)
    {
        PlexilTreeNode varNameNode = this.getChild(1);
        String varName = varNameNode.getText();

        // If supplied, check initial value for type conflict
        // Track success of this check separately
        // N.B. we assume this is a LiteralNode,
        // but ExpressionNode supports the required method
        // and allows the syntax to be generalized later.
        ExpressionNode initValNode = getInitialValueNode();
        if (initValNode != null) {
            // Perform general semantic check of init value
            initValNode.check(context, state);

            PlexilDataType arrayType = getVariableType();
            PlexilDataType elementType = null; 
            if (arrayType != null) 
                elementType = arrayType.arrayElementType();

            if (initValNode.getDataType().isArray()) {
                if (arrayType != null
                    && !initValNode.assumeType(arrayType, state)) {
                    // assumeType() already reported the cause of the failure 
                }

                // Check size < declared max
                String sizeString = getArraySizeNode().getText();
                int size = LiteralNode.parseIntegerValue(sizeString);
                if (initValNode.getChildCount() > size) {
                    state.addDiagnostic(initValNode,
                                        "For array variable \"" + varName
                                        + "\": Array size is " + Integer.toString(size)
                                        + ", but initial value has " + Integer.toString(initValNode.getChildCount())
                                        + " elements", 
                                        Severity.ERROR);
                }
            }
            // Check scalar init value type consistency
            else if (elementType != null 
                     && !initValNode.assumeType(elementType, state)) {
                state.addDiagnostic(initValNode,
                                    "For array variable \"" + varName
                                    + "\": Initial value's type, "
                                    + initValNode.getDataType().typeName()
                                    + ", does not match array element type "
                                    + elementType.typeName(),
                                    Severity.ERROR);
            }
        }
    }

}

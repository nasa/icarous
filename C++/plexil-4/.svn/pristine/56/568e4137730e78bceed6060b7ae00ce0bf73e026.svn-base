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

public class InterfaceDeclNode extends PlexilTreeNode
{
    Vector<InterfaceVariableName> m_variables = new Vector<InterfaceVariableName>();

    public InterfaceDeclNode(Token t)
    {
        super(t);
    }

    public InterfaceDeclNode(InterfaceDeclNode n)
    {
        super(n);
		m_variables = n.m_variables;
    }

	public Tree dupNode()
	{
		return new InterfaceDeclNode(this);
	}

    //
    // Format is:
    // (IN_KYWD ncname*) - referencing existing decls only
    //  or
    // (IN_KYWD variableDeclaration*) - referencing existing or establishing new decls
    // 
    // A variableDeclaration may be either:
    // (VARIABLE_DECLARATION typeName NCNAME initValue?)
    // or
    // (ARRAY_VARIABLE_DECLARATION typeName NCNAME INT initValue?)
    // 
    // Parent is always a BlockNode, which always has its own context.
    //

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        boolean isInOut = this.getType() == PlexilLexer.IN_OUT_KYWD;
        for (int i = 0; i < this.getChildCount(); i++) {
            PlexilTreeNode child = this.getChild(i);
            if (child.getType() == PlexilLexer.NCNAME) 
                earlyCheckNameOnly(child, isInOut, context, state);
            else 
                earlyCheckWithType((VariableDeclNode) child, isInOut, context, state);
        }
    }

    private void earlyCheckNameOnly(PlexilTreeNode nameNode,
                                    boolean isInOut,
                                    NodeContext context,
                                    CompilerState state)
    {
        // Must be inherited from containing node
        VariableName var = context.findInheritedVariable(nameNode.getText());
        if (var == null) {
            state.addDiagnostic(nameNode,
                                this.getToken().getText()
                                + " interface variable \""
                                + nameNode.getText()
                                + "\" was not previously declared",
                                Severity.ERROR);
            return;
        }

        // If InOut, check whether variable was previously declared In
        if (isInOut && !var.isAssignable()) {
            state.addDiagnostic(nameNode,
                                "InOut interface variable \""
                                + nameNode.getText()
                                + "\" was previously declared In",
                                Severity.ERROR);
            state.addDiagnostic(var.getDeclaration(),
                                "Previous declaration is here",
                                Severity.NOTE);
        }

        // Add interface variable to context, and log it
        m_variables.add(context.addInterfaceVariable(nameNode, 
                                                     nameNode,
                                                     isInOut,
                                                     var.getVariableType(),
                                                     var));
    }

    private void earlyCheckWithType(VariableDeclNode declNode,
                                    boolean isInOut,
                                    NodeContext context,
                                    CompilerState state)
    {
        // Do common semantic checks
        declNode.earlyCheckCommon(context, state);
		
        // Which case is this?
        PlexilTreeNode nameNode = declNode.getNameNode();
        VariableName var = context.findInheritedVariable(nameNode.getText());
        if (var == null) {
            // Interface variable is new
            if (!context.isLibraryNode()) {
                // Only library nodes can have new interface vars
                state.addDiagnostic(declNode,
                                    this.getToken().getText()
                                    + " interface variable \""
                                    + nameNode.getText()
                                    + "\" was not previously declared",
                                    Severity.ERROR);
                return;
            }
            // safe to add the variable
            PlexilDataType varType = declNode.getVariableType();
            // should NEVER be null, but just in case...
            if (varType != null) {
                InterfaceVariableName newvar = null;
                if (varType.isArray()) {
                    // declare as new array
                    ArrayVariableDeclNode adecl = (ArrayVariableDeclNode) declNode;
                    newvar = context.addInterfaceVariable(declNode,
                                                          nameNode,
                                                          isInOut,
                                                          varType,
                                                          adecl.getArraySizeNode().getText(),
                                                          adecl.getInitialValueNode());
                }
                else {
                    newvar = context.addInterfaceVariable(declNode, 
                                                          nameNode, 
                                                          isInOut,
                                                          varType);
                }
                m_variables.add(newvar);
                declNode.setVariableName(newvar);
            }
        }
        else {
            // Variable is defined
            // Check that interface declaration is consistent with existing
            PlexilDataType varType = declNode.getVariableType();
            if (varType != null) {
                // should NEVER be null, but just in case...
                if (varType != var.getVariableType()) {
                    state.addDiagnostic(declNode,
                                        this.getToken().getText()
                                        + " variable \"" + nameNode.getText()
                                        + "\" type " + declNode.getVariableType().typeName()
                                        + " differs from previous declaration",
                                        Severity.ERROR);
                    state.addDiagnostic(var.getDeclaration(),
                                        "Original declaration of \"" + nameNode.getText()
                                        + "\" is here",
                                        Severity.NOTE);
                }

                if (varType.isArray()) {
                    // Check length against existing
                    ArrayVariableDeclNode adecl = (ArrayVariableDeclNode) declNode;
                    int syz = adecl.getArraySize();
                    int oldSyz = var.getMaxSize();
                    if (syz != oldSyz) {
                        state.addDiagnostic(declNode,
                                            this.getToken().getText()
                                            + " variable \"" + nameNode.getText()
                                            + "\" max size " + Integer.toString(syz)
                                            + " differs from previous declared size " + Integer.toString(oldSyz),
                                            Severity.ERROR);
                        state.addDiagnostic(var.getDeclaration(),
                                            "Original declaration of \"" + nameNode.getText()
                                            + "\" is here",
                                            Severity.NOTE);
                    }
                }
            }

            // If InOut, check whether variable was previously declared In
            if (isInOut && !var.isAssignable()) {
                state.addDiagnostic(nameNode,
                                    "InOut interface variable \""
                                    + nameNode.getText()
                                    + "\" was previously declared In",
                                    Severity.ERROR);
                state.addDiagnostic(var.getDeclaration(),
                                    "Previous declaration is here",
                                    Severity.NOTE);
            }

            // Add interface variable to context
            InterfaceVariableName newvar = 
                context.addInterfaceVariable(declNode, 
                                             nameNode, 
                                             isInOut, 
                                             var.getVariableType(), 
                                             var);
            m_variables.add(newvar);
            declNode.setVariableName(newvar);
        }
    }

    // I don't think we need to do anything here
    public void checkSelf(NodeContext context, CompilerState state)
    {
    }

}

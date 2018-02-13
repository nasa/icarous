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

import java.util.Set;
import java.util.TreeSet;
import java.util.Vector;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class LibraryCallNode extends PlexilTreeNode
{
    public LibraryCallNode(Token t)
    {
        super(t);
    }

    public LibraryCallNode(LibraryCallNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new LibraryCallNode(this);
    }

    //
    // format is:
    // ^(LIBRARY_CALL_KYWD NCNAME aliasSpecs?)
    //

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        // Add library node as child so conditions can reference it
        String libName = this.getChild(0).getText();
        context.addChildNodeId(this.getChild(0)); // for effect
        
        // Be sure variables ref'd in aliases are set up
        earlyCheckChildren(context, state);

        // Check that library node is declared
        GlobalDeclaration libDecl =
            GlobalContext.getGlobalContext().getLibraryNodeDeclaration(libName);
        if (libDecl == null) {
            state.addDiagnostic(this.getChild(0),
                                "Library action \"" + libName + "\" is not declared",
                                Severity.ERROR);
        }
        else {
            // Check number and in/out status of aliases
            Vector<VariableName> paramSpecs = libDecl.getParameterVariables();

            PlexilTreeNode aliases = this.getChild(1);
            int argument_count = 
                aliases == null ? 0 : aliases.getChildCount();
            if (paramSpecs == null) {
                if (argument_count > 0) {
                    state.addDiagnostic(aliases,
                                        "Library action \"" + libName
                                        + "\" expects 0 arguments, but "
                                        + Integer.toString(argument_count)
                                        + " were supplied",
                                        Severity.ERROR);
                }
            }
            else if (argument_count == 0) {
                // Count parameters w/o default values
                int minArgs = 0;
                for (VariableName v : paramSpecs)
                    if (v.getInitialValue() == null)
                        minArgs++;
                if (minArgs > 0) {
                    state.addDiagnostic(this,
                                        "Library action \"" + libName
                                        + "\" expects at least "
                                        + Integer.toString(minArgs) 
                                        + " arguments, but 0 were supplied",
                                        Severity.ERROR);
                }
            }
            else {
                // Match up aliases with parameters
                // Do type and writable variable checking in check() below
                Set<String> used = new TreeSet<String>();
                for (int i = 0; i < aliases.getChildCount(); i++) {
                    PlexilTreeNode alias = aliases.getChild(i);
                    String paramName = alias.getChild(0).getText();
                    VariableName param = libDecl.getParameterByName(paramName);
                    ExpressionNode valueExp = (ExpressionNode) alias.getChild(1);

                    if (param == null) {
                        state.addDiagnostic(alias.getChild(0),
                                            "Library action \"" + libName
                                            + "\" has no parameter named \""
                                            + paramName + "\"",
                                            Severity.ERROR);
                        continue;
                    }
                    used.add(paramName);
                    if (param.isAssignable()) {
                        if (!valueExp.isAssignable()) {
                            state.addDiagnostic(alias,
                                                "Library action parameter \"" + paramName
                                                + "\" is declared InOut, but is aliased to a read-only expression",
                                                Severity.ERROR);
                        }
                    }
                }
                // See if any required params went unused
                Set<String> required = new TreeSet<String>();
                for (VariableName v : paramSpecs) 
                    if (!used.contains(v.getName()) && v.getInitialValue() != null)
                        required.add(v.getName());
                if (required.size() != 0) {
                    String missingNames = null;
                    for (String s : required)
                        if (missingNames == null)
                            missingNames = s;
                        else
                            missingNames = missingNames + ", " + s;
                    state.addDiagnostic(this,
                                        "Library action \"" + libName
                                        + "\" required paramater(s) "
                                        + missingNames + " were not supplied",
                                        Severity.ERROR);
                }
            }
        }
    }

    public void check(NodeContext context, CompilerState state)
    {
        // Check that library node is declared
        String libName = this.getChild(0).getText();
        GlobalDeclaration libDecl =
            GlobalContext.getGlobalContext().getLibraryNodeDeclaration(libName);
        if (libDecl != null) {
            Vector<VariableName> paramSpecs = libDecl.getParameterVariables();
            PlexilTreeNode aliases = this.getChild(1);
            if (paramSpecs != null && aliases != null) {
                // Check types of supplied aliases
                for (int i = 0; i < aliases.getChildCount(); i++) {
                    PlexilTreeNode alias = aliases.getChild(i);
                    VariableName param = libDecl.getParameterByName(alias.getChild(0).getText());
                    ExpressionNode valueExp = (ExpressionNode) alias.getChild(1);
                    if (param != null) // error already reported in earlyCheck
                        valueExp.assumeType(param.getVariableType(), state); // for effect
                }
            }
        }
        // Finish up type checking on aliases
        checkChildren(context, state);
    }

    public void constructXML()
    {
        m_xml = new XMLElement("Node");
        m_xml.setAttribute("NodeType", "LibraryNodeCall");
        IXMLElement bodyXML = new XMLElement("NodeBody");
        m_xml.addChild(bodyXML);
        IXMLElement callXML = new XMLElement("LibraryNodeCall");
        bodyXML.addChild(callXML);
        IXMLElement idXML = new XMLElement("NodeId");
        idXML.setContent(this.getChild(0).getText());
        callXML.addChild(idXML);
        if (this.getChildCount() > 1) {
            // TODO: Add aliases
            PlexilTreeNode aliases = this.getChild(1);
            for (int i = 0; i < aliases.getChildCount(); i++) {
                PlexilTreeNode alias = aliases.getChild(i);
                IXMLElement aliasXML = new XMLElement("Alias");
                callXML.addChild(aliasXML);
                IXMLElement nameXML = new XMLElement("NodeParameter");
                nameXML.setContent(alias.getChild(0).getText());
                aliasXML.addChild(nameXML);
                aliasXML.addChild(alias.getChild(1).getXML());
            }
        }
    }

}

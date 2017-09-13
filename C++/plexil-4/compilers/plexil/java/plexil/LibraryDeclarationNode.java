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

public class LibraryDeclarationNode extends PlexilTreeNode
{
    public LibraryDeclarationNode(Token t)
    {
        super(t);
    }

    public LibraryDeclarationNode(LibraryDeclarationNode n)
    {
        super(n);
    }

	public Tree dupNode()
	{
		return new LibraryDeclarationNode(this);
	}

    // structure is:
    // ^(LIBRARY_ACTION_KYWD NCNAME interfaceSpec?)

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        // check that name is not already defined
        String libraryName = this.getChild(0).getText();
        if (GlobalContext.getGlobalContext().isCommandName(libraryName)) {
            // Report duplicate definition
            state.addDiagnostic(this.getChild(0),
                                "Library action \"" + libraryName + "\" is already defined",
                                Severity.ERROR);
        }

        // Parse parameter list, if supplied
        Vector<VariableName> ifSpecs = null;
        ParameterSpecNode ifVarAST = (ParameterSpecNode) this.getChild(1);
        if (ifVarAST != null) {
            ifVarAST.earlyCheck(context, state); // for effect
            ifSpecs = ifVarAST.getParameterVector();
            if (ifSpecs != null) {
                for (VariableName vn : ifSpecs) {
                    if (vn.getVariableType() == PlexilDataType.ANY_TYPE) {
                        state.addDiagnostic(vn.getDeclaration(),
                                            "Illegal type for library action interface variable",
                                            Severity.ERROR);
                    }
                }
            }
        }

        // Define in global environment
        GlobalContext.getGlobalContext().addLibraryNode(this, libraryName, ifSpecs);
    }

    public void constructXML()
    {
        super.constructXML();
        IXMLElement nameXML = new XMLElement("Name");
        nameXML.setContent(this.getChild(0).getText());
        m_xml.addChild(nameXML);
        if (this.getChildCount() > 1 && this.getChild(1).getChildCount() > 0)
            m_xml.addChild(this.getChild(1).getXML());
    }

    public String getXMLElementName() { return "LibraryNodeDeclaration"; }

}

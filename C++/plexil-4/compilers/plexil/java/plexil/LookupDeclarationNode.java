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

import java.util.Vector;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class LookupDeclarationNode extends PlexilTreeNode
{
    public LookupDeclarationNode(Token t)
    {
        super(t);
    }

    public LookupDeclarationNode(LookupDeclarationNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new LookupDeclarationNode(this);
    }

    // structure is:
    // ^(LOOKUP_KYWD NCNAME returnsSpec paramsSpec?)

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        // check that name is not already defined
        String lookupName = this.getChild(0).getText();
        if (GlobalContext.getGlobalContext().isLookupName(lookupName)) {
            // Report duplicate definition
            state.addDiagnostic(this.getChild(0),
                                "Lookup \"" + lookupName + "\" is already defined",
                                Severity.ERROR);
        }

        // Parse return spec
        ReturnSpecNode returnAST = (ReturnSpecNode) this.getChild(1);
        returnAST.earlyCheck(context, state); // for effect
        Vector<VariableName> returnSpecs = returnAST.getReturnVector();

        // Parse parameter list, if supplied
        Vector<VariableName> parmSpecs = null;
        ParameterSpecNode parmAST = (ParameterSpecNode) this.getChild(2);
        if (parmAST != null) {
            parmAST.earlyCheck(context, state); // for effect
            parmSpecs = parmAST.getParameterVector();
            if (parmSpecs != null) {
                for (VariableName vn : parmSpecs) {
                    if (vn instanceof InterfaceVariableName) {
                        state.addDiagnostic(vn.getDeclaration(),
                                            (vn.isAssignable() ? "InOut" : "In")
                                            + " declaration is illegal in " +
                                            "lookup parameter declarations",
                                            Severity.ERROR);
                    }
                }
            }
        }

        // Define in global environment
        GlobalContext.getGlobalContext().addLookupName(this, lookupName, parmSpecs, returnSpecs);
    }

    public void constructXML()
    {

        super.constructXML();

        // add name
        PlexilTreeNode nameTree = this.getChild(0);
        IXMLElement nameXML = new XMLElement("Name");
        nameXML.setContent(nameTree.getText());
        m_xml.addChild(nameXML);

        // Add return spec
        ((ReturnSpecNode) this.getChild(1)).constructReturnXML(m_xml);

        // Add parameter spec(s) if provided
        ParameterSpecNode parametersSpec = (ParameterSpecNode) this.getChild(2);
        if (parametersSpec != null) 
            parametersSpec.constructParameterXML(m_xml);
    }

    public String getXMLElementName() { return "StateDeclaration"; }

}

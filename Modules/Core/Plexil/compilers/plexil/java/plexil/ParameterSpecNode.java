// Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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

public class ParameterSpecNode extends PlexilTreeNode
{
    private Vector<VariableName> m_parameterSpecs = null;

    public ParameterSpecNode(Token t)
    {
        super(t);
    }

    public ParameterSpecNode(ParameterSpecNode n)
    {
        super(n);
        m_parameterSpecs = n.m_parameterSpecs;
    }

    public Tree dupNode()
    {
        return new ParameterSpecNode(this);
    }

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        boolean ellipsisSeen = false;
        Set<String> names = new TreeSet<String>();
        if (this.getChildCount() > 0)
            m_parameterSpecs = new Vector<VariableName>();

        for (int parmIdx = 0; parmIdx < this.getChildCount(); parmIdx++) {
            String typeName = null;
            PlexilTreeNode nameSpec = null;
            String sizeSpec = null;

            PlexilTreeNode parm = this.getChild(parmIdx);
            int parmType = parm.getType();
            if (parmType == PlexilLexer.IN_KYWD
                || parmType == PlexilLexer.IN_OUT_KYWD) {
                // Library interface spec
                // ^((In | InOut) typename NCNAME INT?)
                typeName = parm.getChild(0).getText();
                nameSpec = parm.getChild(1);
                if (parm.getChild(2) != null) {
                    sizeSpec = parm.getChild(2).getText(); // array dimension
                    // check spec'd size
                    int arySiz = LiteralNode.parseIntegerValue(sizeSpec);
                    if (arySiz < 0) {
                        state.addDiagnostic(parm.getChild(1),
                                            "Array size must not be negative",
                                            Severity.ERROR);
                    }
                }
            }
            else if (parmType == PlexilLexer.ARRAY_TYPE) {
                // ^(ARRAY_TYPE eltTypeName INT NCNAME?)
                typeName = parm.getChild(0).getText();
                sizeSpec = parm.getChild(1).getText();
                nameSpec = parm.getChild(2); // may be null
                // check spec'd size
                int arySiz = LiteralNode.parseIntegerValue(sizeSpec);
                if (arySiz < 0) {
                    state.addDiagnostic(parm.getChild(1),
                                        "Array size must not be negative",
                                        Severity.ERROR);
                }
            }
            else {
                // typeName or ^(typeName NCNAME)
                typeName = parm.getToken().getText();
                if (parm.getChildCount() > 0)
                    nameSpec = parm.getChild(0);
            }

            String nam =
                (nameSpec == null) ? null : nameSpec.getText();

            // check for duplicate names and warn
            if (nam != null) {
                if (names.contains(nam)) {
                    // Report duplicate name warning
                    state.addDiagnostic(nameSpec,
                                        "Parameter name \"" + nam + "\" was used more than once",
                                        Severity.WARNING);
                }
                else {
                    names.add(nam);
                }
            }

            if (ellipsisSeen) {
                state.addDiagnostic(parm, 
                                    "Wildcard (ellipsis) parameter must be the last parameter",
                                    Severity.ERROR);
                ellipsisSeen = false; // turn off for subsequent parameters
            }

            // Construct parameter spec
            VariableName newParmVar = null;
            switch (parmType) {
                // In and InOut are only valid for library node declarations
            case PlexilLexer.IN_KYWD:
            case PlexilLexer.IN_OUT_KYWD:
                if (sizeSpec != null) {
                    // Array case
                    newParmVar = 
                        new InterfaceVariableName(parm,
                                                  nam,
                                                  parmType == PlexilLexer.IN_OUT_KYWD,
                                                  PlexilDataType.findByName(typeName).arrayType(),
                                                  sizeSpec,
                                                  null);
                }
                else {
                    newParmVar =
                        new InterfaceVariableName(parm,
                                                  nam,
                                                  parmType == PlexilLexer.IN_OUT_KYWD,
                                                  PlexilDataType.findByName(typeName));
                }
                break;

            case PlexilLexer.ARRAY_TYPE:
                newParmVar = new VariableName(parm,
                                              nam,
                                              PlexilDataType.findByName(typeName).arrayType(),
                                              sizeSpec,
                                              null);
                break;

            case PlexilLexer.ANY_KYWD:
            case PlexilLexer.BOOLEAN_KYWD:
            case PlexilLexer.INTEGER_KYWD:
            case PlexilLexer.REAL_KYWD:
            case PlexilLexer.STRING_KYWD:
            case PlexilLexer.DATE_KYWD:
            case PlexilLexer.DURATION_KYWD:
                newParmVar = new VariableName(parm,
                                              nam,
                                              PlexilDataType.findByName(typeName));
                break;

            case PlexilLexer.ELLIPSIS:
                newParmVar = new WildcardVariableName(parm);
                ellipsisSeen = true;
                break;

            default:
                state.addDiagnostic(parm, 
                                    "Invalid parameter descriptor \"" + parm.getText() + "\"",
                                    Severity.ERROR);
                // create a placeholder so other parsing can continue
                newParmVar = new VariableName(parm, 
                                              "INVALID_PARM_" + Integer.toString(parmIdx),
                                              PlexilDataType.ANY_TYPE);
                break;
            }

            m_parameterSpecs.add(newParmVar);
        }
    }

    public Vector<VariableName> getParameterVector() { return m_parameterSpecs; }

    public boolean containsAnyType()
    {
        if (m_parameterSpecs == null)
            return false;
        for (VariableName vn : m_parameterSpecs)
            if (vn.getVariableType() == PlexilDataType.ANY_TYPE)
                return true;
        return false;
    }

    // For command and lookup
    public void constructParameterXML(IXMLElement parent)
    {
        if (m_parameterSpecs != null) {
            for (VariableName vn : m_parameterSpecs) 
                parent.addChild(vn.makeGlobalDeclarationElement("Parameter"));
        }
    }

    // For library node
    public void constructXML()
    {
        super.constructXML();
        if (m_parameterSpecs != null) {
            Vector<VariableName> inVars = new Vector<VariableName>();
            Vector<VariableName> inOutVars = new Vector<VariableName>();
            for (VariableName vn : m_parameterSpecs) {
                if (vn.isAssignable())
                    inOutVars.add(vn);
                else
                    inVars.add(vn);
            }
            if (inVars.size() > 0) {
                IXMLElement inXML = new XMLElement("In");
                m_xml.addChild(inXML);
                for (VariableName inVar : inVars)
                    inXML.addChild(inVar.makeDeclarationXML());
            }
            if (inOutVars.size() > 0) {
                IXMLElement inOutXML = new XMLElement("InOut");
                m_xml.addChild(inOutXML);
                for (VariableName inOutVar : inOutVars)
                    inOutXML.addChild(inOutVar.makeDeclarationXML());
            }
        }
    }

    public String getXMLElementName() { return "Interface"; }

}

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

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

public class RelationalNode extends ExpressionNode
{
    public RelationalNode(Token t)
    {
        super(t);
        m_dataType = PlexilDataType.BOOLEAN_TYPE;
    }

    public RelationalNode(RelationalNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new RelationalNode(this);
    }

    /**
     * @brief Check the expression for type consistency.
     */
    public void checkTypeConsistency(NodeContext context, CompilerState myState)
    {
        ExpressionNode lhs = (ExpressionNode) this.getChild(0);
        ExpressionNode rhs = (ExpressionNode) this.getChild(1);
        PlexilDataType lhsType = lhs.getDataType();
        PlexilDataType rhsType = rhs.getDataType();
        if (!lhsType.isNumeric() && !lhsType.isTemporal()) {
            myState.addDiagnostic(lhs,
                                  "The first operand to the " +
                                  this.getToken().getText() +
                                  " operator is not numeric or temporal",
                                  Severity.ERROR);
        }
        if (!rhsType.isNumeric() && !rhsType.isTemporal()) {
            myState.addDiagnostic(rhs,
                                  "The second operand to the " +
                                  this.getToken().getText() +
                                  " operator is not numeric or temporal",
                                  Severity.ERROR);
        }
        if (lhsType.isTemporal() && rhsType.isTemporal() && lhsType != rhsType) {
            myState.addDiagnostic(rhs,
                                  "Cannot compare dates with durations!",
                                  Severity.ERROR);
        }
        if (lhsType.isTemporal() && !rhsType.isTemporal()) {
            myState.addDiagnostic(rhs,
                                  "Cannot compare temporal expression with anything else!",
                                  Severity.ERROR);
        }
    }

    /**
     * @brief Construct the XML representing this part of the parse tree,
     * and store it in m_xml.
     */
    protected void constructXML()
    {
        super.constructXML();
        m_xml.addChild(this.getChild(0).getXML());
        m_xml.addChild(this.getChild(1).getXML());
    }

    protected String getXMLElementName()
    {
        int tokType = this.getToken().getType();
        switch (tokType) {
        case PlexilLexer.LEQ:
            return "LE";

        case PlexilLexer.LESS:
            return "LT";

        case PlexilLexer.GEQ:
            return "GE";

        case PlexilLexer.GREATER:
            return "GT";

        default:
            return null;
        }
    }

}

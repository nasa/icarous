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

public class IfNode extends PlexilTreeNode
{
    private Vector<NodeContext> m_bodyContexts = null;

    public IfNode(Token t)
    {
        super(t);
    }

	public IfNode(IfNode n)
	{
		super(n);
		m_bodyContexts = n.m_bodyContexts;
	}

	public Tree dupNode()
	{
		return new IfNode(this);
	}

    //
    // N.B. The extra complexity in the checking logic is to ensure each consequent body
    // is contained in a separate name binding context from the if statement as a whole.
    //

    /**
     * @brief Prepare for the semantic check.
     */
    public void earlyCheck(NodeContext parentContext, CompilerState state)
    {
        earlyCheckSelf(parentContext, state);
        this.getChild(0).earlyCheck(parentContext, state); // test

        // See if we have a node ID
        String nodeId = null;
        PlexilTreeNode parent = this.getParent();
        if (parent != null && parent instanceof ActionNode) {
            nodeId = ((ActionNode) parent).getNodeId();
        }
        else {
            // should never happen
            state.addDiagnostic(this,
                                "Internal error: IfNode instance has no parent ActionNode",
                                Severity.FATAL);
        }

        // Allocate context vector
        int nContexts = (getChildCount() + 1) / 2;
        m_bodyContexts = new Vector<NodeContext>(nContexts);

        // Construct contexts for clauses
        // FIXME: change suffixes to match generated code!
        NodeContext tempCtxt = new NodeContext(parentContext, nodeId + "_THEN_CLAUSE");
        m_bodyContexts.add(tempCtxt);
        getChild(1).earlyCheck(tempCtxt, state); // then body

        int nkids = this.getChildCount();
        for (int i = 2; i < nkids; i += 2) {
            if (nkids - i > 1) {
                // Elseif test & clause
                getChild(i).earlyCheck(parentContext, state); // elseif test
                tempCtxt = new NodeContext(parentContext, nodeId + "_ELSEIF_CLAUSE_" + Integer.toString(i / 2));
                m_bodyContexts.add(tempCtxt);
                getChild(i + 1).earlyCheck(tempCtxt, state); // elseif body
            }
            else {
                // Final else clause
                tempCtxt = new NodeContext(parentContext, nodeId + "_ELSE_CLAUSE");
                m_bodyContexts.add(tempCtxt);
                getChild(i).earlyCheck(tempCtxt, state); // else body
            }
        }
    }

    /**
     * @brief Semantic check.
     * @note Uses separate contexts for then and else bodies.
     */
    public void check(NodeContext parentContext, CompilerState myState)
    {
        checkSelf(parentContext, myState);
        getChild(0).check(parentContext, myState); // test
        getChild(1).check(m_bodyContexts.elementAt(0), myState); // then body
        int nkids = getChildCount();
        int ctxt = 1;
        for (int i = 2; i < nkids; i += 2) {
            if (nkids - i > 1) {
                // Elseif test & clause
                getChild(i).check(parentContext, myState); // elseif test
                getChild(i + 1).check(m_bodyContexts.elementAt(ctxt), myState); // elseif body
            }
            else {
                // Final else clause
                getChild(i).check(m_bodyContexts.elementAt(ctxt), myState); // else body
            }
            ctxt++;
        }
    }

    public void checkSelf(NodeContext context, CompilerState myState)
    {
        // Assert Boolean type on tests
        if (!((ExpressionNode) getChild(0)).assumeType(PlexilDataType.BOOLEAN_TYPE, myState)) {
            myState.addDiagnostic(getChild(0),
                                  "If test expression is not Boolean",
                                  Severity.ERROR);
        }
        // Type check remaining tests
        int nkids = getChildCount();
        for (int i = 2; i < nkids; i += 2) {
            if (nkids - i > 1) {
                // Elseif test & clause
                if (!((ExpressionNode)getChild(i)).assumeType(PlexilDataType.BOOLEAN_TYPE, myState)) {
                    myState.addDiagnostic(getChild(i),
                                          "ElseIf test expression is not Boolean",
                                          Severity.ERROR);
                }
                else {
                    // final else clause
                }
            }
        }
    }

    protected void constructXML()
    {
        super.constructXML(); // constructs "If" element

        // Insert if-condition
        IXMLElement condition = new XMLElement("Condition");
        condition.addChild(getChild(0).getXML());
        m_xml.addChild(condition);

        // Insert then clause
        IXMLElement consequent = new XMLElement("Then");
        consequent.addChild(getChild(1).getXML());
        m_xml.addChild(consequent);

        int nkids = getChildCount();
        if (nkids == 2)
            return;
        
        int i = 2;
        for (; i < nkids; i += 2) {
            if (nkids - i > 1) {
                // Insert ElseIf clause(s)
                IXMLElement elseIfClause = new XMLElement("ElseIf");
                IXMLElement elseIfCondition = new XMLElement("Condition");
                elseIfCondition.addChild(getChild(i).getXML());
                elseIfClause.addChild(elseIfCondition);
                IXMLElement elseIfConsequent = new XMLElement("Then");
                elseIfConsequent.addChild(getChild(i + 1).getXML());
                elseIfClause.addChild(elseIfConsequent);
                m_xml.addChild(elseIfClause);
            }
            else {
                // insert final Else clause
                IXMLElement elseClause = new XMLElement("Else");
                elseClause.addChild(getChild(i).getXML());
                m_xml.addChild(elseClause);
            }
        }
    }

    protected String getXMLElementName() { return "If"; }

}

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

public class ResourceNode extends PlexilTreeNode
{
	private ExpressionNode m_name = null;
	private ExpressionNode m_lowerBound = null;
	private ExpressionNode m_upperBound = null;
	private ExpressionNode m_releaseAtTermination = null;
	private ExpressionNode m_priority = null;

	public ResourceNode(Token t)
	{
		super(t);
	}

	public ResourceNode(ResourceNode n)
	{
		super(n);
		m_name = n.m_name;
		m_lowerBound = n.m_lowerBound;
		m_upperBound = n.m_upperBound;
		m_releaseAtTermination = n.m_releaseAtTermination;
		m_priority = n.m_priority;
	}

	public Tree dupNode()
	{
		return new ResourceNode(this);
	}

	public void earlyCheckSelf(NodeContext context, CompilerState state)
	{
		// format is:
		// ^(RESOURCE_KYWD name_expr [ option_kywd value_expr ]* )
		m_name = (ExpressionNode) this.getChild(0);
		for (int i = 1; i < this.getChildCount(); i += 2) {
			PlexilTreeNode kywd = this.getChild(i);
			ExpressionNode valueExpr = (ExpressionNode) this.getChild(i + 1);
			if (valueExpr == null) {
				// TODO: complain if valueExpr null
				if (this.getChild(i + 1) != null) {
					state.addDiagnostic(this.getChild(i + 1),
										"The value supplied for the Resource option "
										+ kywd.getText()
										+ " was not an expression",
										Severity.ERROR);
				}
				else {
					// Parser internal error or syntax/tree mismatch
					state.addDiagnostic(this,
										"Internal error: Resource option "
										+ kywd.getText()
										+ " missing a value expression",
										Severity.FATAL);
				}
			}

			switch (kywd.getType()) {

			case PlexilLexer.LOWER_BOUND_KYWD:
				if (m_lowerBound != null) {
					// Repeated keyword error
					state.addDiagnostic(kywd,
										"The " + kywd.getText()
										+ " keyword may only appear once per Resource statement",
										Severity.ERROR);
				}
				m_lowerBound = valueExpr;
				break;

			case PlexilLexer.UPPER_BOUND_KYWD:
				if (m_upperBound != null) {
					// Repeated keyword error
					state.addDiagnostic(kywd,
										"The " + kywd.getText()
										+ " keyword may only appear once per Resource statement",
										Severity.ERROR);
				}
				m_upperBound = valueExpr;
				break;

			case PlexilLexer.RELEASE_AT_TERM_KYWD:
				if (m_releaseAtTermination != null) {
					// Repeated keyword error
					state.addDiagnostic(kywd,
										"The " + kywd.getText()
										+ " keyword may only appear once per Resource statement",
										Severity.ERROR);
				}
				m_releaseAtTermination = valueExpr;
				break;

			case PlexilLexer.PRIORITY_KYWD:
				if (m_priority != null) {
					// TODO: repeated keyword error	
					state.addDiagnostic(kywd,
										"The " + kywd.getText()
										+ " keyword may only appear once per Resource statement",
										Severity.ERROR);
				}
				m_priority = valueExpr;
				break;

			default:
				// TODO: complain of bogus keyword
				state.addDiagnostic(kywd,
									"Unexpected token " + kywd.getText()
									+ " in a Resource statement",
									Severity.ERROR);
				break;
			}
		}
        // Priority is required
        if (m_priority == null) {
            state.addDiagnostic(this,
                                "Resource statement missing Priority",
                                Severity.ERROR);
        }
	}

	public void checkSelf(NodeContext context, CompilerState state)
	{
		// Type check name
		if (m_name.getDataType() != PlexilDataType.STRING_TYPE) {
			state.addDiagnostic(m_name,
								"Resource name is not a String expression",
								Severity.ERROR);
		}
		
		// Type check bounds, if supplied
		if (m_lowerBound != null
			&& !m_lowerBound.getDataType().isNumeric()) {
			state.addDiagnostic(m_lowerBound,
								"Resource LowerBound value is not a numeric expression",
								Severity.ERROR);
		}
		if (m_upperBound != null
			&& !m_upperBound.getDataType().isNumeric()) {
			state.addDiagnostic(m_lowerBound,
								"Resource UpperBound value is not a numeric expression",
								Severity.ERROR);
		}

		// Type check release-at-termination
		if (m_releaseAtTermination != null
			&& m_releaseAtTermination.getDataType() != PlexilDataType.BOOLEAN_TYPE) {
			state.addDiagnostic(m_releaseAtTermination, 
								"Resource ReleaseAtTermination value is not a Boolean expression",
								Severity.ERROR);
		}

		// Type check priority, if supplied
		if (m_priority != null
			&& !m_priority.getDataType().isNumeric()) {
			state.addDiagnostic(m_priority,
								"Resource Priority value is not a numeric expression",
								Severity.ERROR);
		}
	}

	public void constructXML()
	{
		super.constructXML();
		IXMLElement nameElt = new XMLElement("ResourceName");
		nameElt.addChild(m_name.getXML());
		m_xml.addChild(nameElt);

        IXMLElement prio = new XMLElement("ResourcePriority");
        prio.addChild(m_priority.getXML());
        m_xml.addChild(prio);

		if (m_lowerBound != null) {
			IXMLElement lbound = new XMLElement("ResourceLowerBound");
			lbound.addChild(m_lowerBound.getXML());
			m_xml.addChild(lbound);
		}
		if (m_upperBound != null) {
			IXMLElement ubound = new XMLElement("ResourceUpperBound");
			ubound.addChild(m_upperBound.getXML());
			m_xml.addChild(ubound);
		}
		if (m_releaseAtTermination != null) {
			IXMLElement rat = new XMLElement("ResourceReleaseAtTermination");
			rat.addChild(m_releaseAtTermination.getXML());
			m_xml.addChild(rat);
		}
	}

}

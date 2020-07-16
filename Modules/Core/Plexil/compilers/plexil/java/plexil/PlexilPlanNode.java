// Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

public class PlexilPlanNode extends PlexilTreeNode
{
    public PlexilPlanNode(int ttype)
    {
        super(new CommonToken(ttype, "PLEXIL"));
    }

    public PlexilPlanNode(PlexilPlanNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new PlexilPlanNode(this);
    }

    /**
     * @brief Construct the XML representing this part of the parse tree, and store it in m_xml.
     */
    protected void constructXML()
    {
        super.constructXML();
        for (int i = 0; i < this.getChildCount(); i++) {
            // Allow for declarations to return null.
            IXMLElement childXML = this.getChild(i).getXML();
            if (childXML != null)
                m_xml.addChild(childXML);
        }
    }

    /**
     * @brief Get the string to use for the XML element's name.
     * @return A non-null String.
     * @note This is a base method. Derived classes should override it as required.
     */
    protected String getXMLElementName()
    {
        return "PlexilPlan";
    }


    /**
     * @brief Add new source locator attributes to m_xml, or replace the existing ones.
     */
    protected void addSourceLocatorAttributes()
    {
        super.addSourceLocatorAttributes();
        if (m_xml != null) {
            String fname = CompilerState.getCompilerState().getSourceFileName();
            if (fname != null)
                m_xml.setAttribute("FileName", fname);
        }
    }



}

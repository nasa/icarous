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

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class PlexilTreeNode extends org.antlr.runtime.tree.CommonTree
{
    protected IXMLElement m_xml = null;

    //
    // Constructors
    //
    public PlexilTreeNode()
    {
        super();
    }

    // *** N.B. m_xml is ignored.
    public PlexilTreeNode(PlexilTreeNode node)
    {
        super(node);
    }

    public PlexilTreeNode(Token t)
    {
        super(t);
    }

    public PlexilTreeNode(CommonTree node)
    {
        super(node);
    }


    //
    // Overrides
    //
	public Tree dupNode()
	{
		return new PlexilTreeNode(this);
	}

    public PlexilTreeNode getChild(int i)
    {
        return (PlexilTreeNode) super.getChild(i);
    }

    public PlexilTreeNode getParent()
    {
        return (PlexilTreeNode) super.getParent();
    }


    //
    // Extensions
    //

    /**
     * @brief Get the containing name binding context for this branch of the parse tree.
     * @return A NodeContext instance, or the global context.
     * @note Derived classes that implement new binding contexts should override this method.
     */
    public NodeContext getContext()
    {
        PlexilTreeNode parent = getParent();
        if (parent != null)
            return parent.getContext();
        else
            return GlobalContext.getGlobalContext();
    }

    /**
     * @brief Establish bindings and do initial checks in top-down order.
     * @note Derived classes that establish binding contexts should override or wrap this method.
     */
    public void earlyCheck(NodeContext context, CompilerState state)
    {
        earlyCheckSelf(context, state);
        earlyCheckChildren(context, state);
    }

    /**
     * @brief Establish local bindings and do initial self checks.
     * @note This default method does nothing. Derived classes should override it.
     */
    public void earlyCheckSelf(NodeContext context, CompilerState state)
    {
    }

    /**
     * @brief Establish bindings and do initial checks of this node's children.
     * @note Derived classes should override this as applicable.
     */
    public void earlyCheckChildren(NodeContext context, CompilerState state)
    {
        for (int i = 0; i < this.getChildCount(); i++)
            this.getChild(i).earlyCheck(context, state);
    }

    /**
     * @brief Perform a recursive semantic check in bottom-up order.
     */
    public void check(NodeContext context, CompilerState state)
    {
        checkChildren(context, state);
        checkSelf(context, state);
    }

    /**
     * @brief Perform a semantic check of this node's requirements.
     * @note This is a default method. Derived classes should implement their own. 
     */
    public void checkSelf(NodeContext context, CompilerState state)
    {
    }

    /**
     * @brief Perform semantic checks on the node's children.
     */
    public void checkChildren(NodeContext context, CompilerState state)
    {
        for (int i = 0; i < this.getChildCount(); i++) {
            this.getChild(i).check(context, state);
        }
    }
	
    //* Returns the NanoXML representation of this part of the parse tree.
    public IXMLElement getXML()
    {
        if (m_xml == null) {
            constructXML();
        }
        return m_xml;
    }


    /**
     * @brief Construct the XML representing this part of the parse tree, and store it in m_xml.
     * @note This is a base method. Derived classes should extend or override it as required.
     */
    protected void constructXML()
    {
        constructXMLBase();
    }

    protected void constructXMLBase()
    {
        m_xml = new XMLElement(this.getXMLElementName());
        this.addSourceLocatorAttributes();
    }

    /**
     * @brief Get the string to use for the XML element's name.
     * @return A non-null String.
     * @note This is a base method. Derived classes should override it as required.
     */
    protected String getXMLElementName()
    {
        return this.getToken().getText();
    }

    /**
     * @brief Add new source locator attributes to m_xml, or replace the existing ones.
     */
    protected void addSourceLocatorAttributes()
    {
        if (m_xml != null) {
            m_xml.setAttribute("LineNo", String.valueOf(1 + this.getLine()));
            m_xml.setAttribute("ColNo", String.valueOf(this.getCharPositionInLine()));
        }
    }

}

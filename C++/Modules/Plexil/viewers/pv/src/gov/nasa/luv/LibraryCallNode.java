/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package gov.nasa.luv;

import java.util.Enumeration;
import java.util.Vector;

import javax.swing.tree.TreeNode;

import static gov.nasa.luv.PlexilSchema.NodeType;

public class LibraryCallNode
    extends Node
    implements Cloneable {

    private Node child;
    private String libraryName;
    private Vector<Node> children;

    protected LibraryCallNode() {
        super(NodeType.LibraryNodeCall);
        child = null;
        libraryName = null;
        children = new Vector<Node>(1);
    }

    private LibraryCallNode(LibraryCallNode orig) {
        super(orig);
        child = null;
        libraryName = orig.libraryName;
        children = new Vector<Node>(1);
        if (orig.child != null)
            addChild(orig.child.clone());
    }

    public LibraryCallNode clone() {
        return new LibraryCallNode(this);
    }

    public int hashCode() {
        return super.hashCode() * 31
            + (libraryName == null ? 0 : libraryName.hashCode());
    }

    public boolean equals(LibraryCallNode other) {
        if (!super.equals(other))
            return false;

        if (libraryName == null) 
            return other.libraryName == null;

        return libraryName.equals(other.libraryName);
    }
        

    //
    // javax.swing.tree.TreeNode API
    //

    @Override
    public Enumeration children() {
        return children.elements();
    }

    @Override
    public boolean getAllowsChildren() {
        return true;
    }

    @Override
    public TreeNode getChildAt(int childIndex) {
        if (childIndex == 0)
            return child;
        else
            return null;
    }

    @Override
    public int getChildCount() {
        if (child == null)
            return 0;
        else
            return 1;
    }

    public int getIndex(TreeNode node) {
        for (int i = 0; i < children.size(); ++i)
            if (node.equals(children.get(i)))
                return i;
        return -1;
    }

    /** Returns the Vector of Node children of this Node. */
    @Override
    public Vector<Node> getChildren() {
        return children;
    }

    @Override
    public boolean hasChildren() {
        return child != null;
    }

    @Override
    public boolean isLeaf() {
        return false;
    }
    
    /**
     * Creates and adds the specified Node child to this Node.
     * @param child the node child
     */
    @Override
    public void addChild(Node newChild) {
        child = newChild;
        child.setParent(this);
        children.add(newChild);
    }

    @Override
    protected void updateTreePath(boolean isReRoot) {
        super.updateTreePath(isReRoot);
        if (isReRoot && child != null)
            child.updateTreePath(isReRoot);
    }

    /** Returns the Library Name for this Plexil Node. */
    public String getLibraryName() {
        return libraryName;
    }

    /**
     * Sets the specified string for the library name of this Node.
     */
    @Override
    public void setLibraryName(String libname) {
        libraryName = libname;
    }
    
    /**
     * Returns the Node that matches with the specified name.
     * @param name the name to match the Node to
     * @return the matching Node
     */
    @Override
    public Node findChildByName(String name) {
        if (child != null && child.nodeName.equals(name))
            return child;
        else
            return null;
    }

    /**
     * Resets all the properties of this Node and descendants 
     * to the initial values (pre-execution).
     */
    @Override
    public void reset() {
        super.reset();
        if (child != null)
            child.reset();
    }
        
    @Override
    protected void toStringInternal(StringBuilder s) {
        super.toStringInternal(s);
        if (child == null)
            return;
        s.append("[");
        child.toStringInternal(s);
        s.append("]");
    }
        
    // For convenience of Plan class.
    @Override
    public void findUnresolvedLibraryCalls(Plan plan) {
        if (child == null)
            plan.addLibraryReference(libraryName, this);
    }

    @Override
    public void addChangeListenerToAll(ChangeListener listener) {
        super.addChangeListenerToAll(listener);
        if (child != null)
            child.addChangeListenerToAll(listener);
    }

    @Override
    public void removeChangeListenerFromAll(ChangeListener listener) {
        super.removeChangeListenerFromAll(listener);
        if (child != null)
            child.removeChangeListenerFromAll(listener);
    }
        
    /**
     * Links a copy of the specified library into this Node.
     * @param library the library for this Node
     * @return whether or not the library was linked
     */
    @Override
    public boolean linkLibrary(Plan library) {
        if (libraryName.equals(library.getName())) {
            if (child != null) {
                child.setParent(null);
                children.clear();
            }
            addChild(library.getRootNode().clone());
            return true;
        }
        return false;
    }

}

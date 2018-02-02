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

public class ListNode
    extends Node {

    private Vector<Node> children;

    protected ListNode() {
        super(NodeType.NodeList);
        children = new Vector<Node>();
    }

    protected ListNode(ListNode orig) {
        super(orig);
        children = new Vector<Node>(orig.children.size());
        for (Node c : orig.children)
            addChild(c.clone());
    }

    public ListNode clone() {
        return new ListNode(this);
    }

    public int hashCode() {
        return super.hashCode() * 31
            + children.size();
    }

    public boolean equals(Object o) {
        if (!super.equals(o))
            return false;
        if (!(o instanceof ListNode)) {
            System.out.println("Not a ListNode: " + this + " != " + o);
            return false;
        }

        ListNode other = (ListNode) o;
        if (children.size() != other.children.size()) {
            System.out.println("Numbers of children differ: " + children.size() + " != " + other.children.size());
            return false;
        }

        for (int i = 0; i < children.size(); ++i)
            if (!children.get(i).equals(other.children.get(i)))
                return false;

        return true;
    }

    /** Returns the Vector of Node children of this Node. */
    @Override
    public Vector<Node> getChildren() {
        return children;
    }

    @Override
    public boolean hasChildren() {
        return children.size() > 0;
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
        return children.get(childIndex);
    }

    @Override
    public int getChildCount() {
        return children.size();
    }

    @Override
    public int getIndex(TreeNode node) {
        if (!(node instanceof Node))
            return -1;

        int result = children.indexOf((Node) node);

        // *** TEMP ***
        if (result >= 0 && children.get(result) != node) 
            System.out.println(children.get(result) + ".equals(" + node + ") = true!");

        return result;
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
    public void addChild(Node child) {
        children.add(child);
        child.setParent(this);
    }

    @Override
    protected void updateTreePath(boolean isReRoot) {
        super.updateTreePath(isReRoot);
        if (isReRoot)
            for (Node c : children)
                c.updateTreePath(isReRoot);
    }

    
    /**
     * Returns the Node that matches with the specified name.
     * @param name the name to match the Node to
     * @return the matching Node
     */
    @Override
    public Node findChildByName(String name) {
        for (Node child : children)
            if (child.nodeName.equals(name))
                return child;
        return null;
    }

    /**
     * Resets all the properties of this Node and descendants 
     * to the initial values (pre-execution).
     */
    @Override
    public void reset() {
        super.reset();
        for (Node child : children)
            child.reset();
    }
        
    @Override
    protected void toStringInternal(StringBuilder s) {
        super.toStringInternal(s);
        if (!children.isEmpty()) {
            Node lastChild = children.lastElement();
            s.append("[");
            for (Node child: children) {
                child.toStringInternal(s);
                if (child != lastChild)
                    s.append(", ");
            }
            s.append("]");
        }
    }
        
    // For convenience of Plan class.
    @Override
    public void findUnresolvedLibraryCalls(Plan plan) {
        for (Node c : children)
            c.findUnresolvedLibraryCalls(plan);
    }

    @Override
    public void addChangeListenerToAll(ChangeListener listener) {
        super.addChangeListenerToAll(listener);
        for (Node c : children)
            c.addChangeListenerToAll(listener);
    }

    @Override
    public void removeChangeListenerFromAll(ChangeListener listener) {
        super.removeChangeListenerFromAll(listener);
        for (Node c : children)
            c.removeChangeListenerFromAll(listener);
    }

}

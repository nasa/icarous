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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import java.util.Enumeration;
import java.util.Vector;

import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.event.TableModelEvent;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;

import org.netbeans.swing.outline.DefaultOutlineModel;
import org.netbeans.swing.outline.Outline;
import org.netbeans.swing.outline.OutlineModel;
import org.netbeans.swing.outline.RenderDataProvider;
import org.netbeans.swing.outline.RowModel;
import org.netbeans.swing.outline.TreePathSupport;

// TODO:
//  - replace magic numbers w/ named constants


/** 
 * The VariablesTab class provides methods for displaying a Plexil Node's local 
 * variable information. 
 */

public class VariablesTab
    extends JPanel { 

    private boolean flexState;
    
    /** 
     * Constructs a VariablesTab with the specified Plexil Node.
     *
     * @param node node on which the VariablesTab represents
     */
    
    public VariablesTab(Node node) {
        super(new BorderLayout());
        setPreferredSize(NodeInfoWindow.paneDimensions);
        VariableTreeTable treeTable = new VariableTreeTable(node);
        
        JPanel buttonBar = new JPanel();
        add(buttonBar, BorderLayout.NORTH);
        GridBagLayout gridbag = new GridBagLayout();
        buttonBar.setLayout(gridbag);
        GridBagConstraints c = new GridBagConstraints();
                
        //treeTable.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
        
        JScrollPane scrollPane = new JScrollPane(treeTable);
        add(scrollPane);

        flexState = false;
        JButton flex = new JButton("Expand All");        
        flex.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (flexState) {
                        treeTable.collapseAllNodes();
                        flex.setText("Expand All");	        	    
                        flexState = false;
                    }
                    else {
                        treeTable.expandAllNodes();        	    
                        flex.setText("Collapse All");
                        flexState = true;
                    }
                }
            });
                               
        c.fill = GridBagConstraints.HORIZONTAL;
        c.weightx = 0.02;
        gridbag.setConstraints(flex, c);
        buttonBar.add(flex);
        JLabel blank = new JLabel("");
        c.weightx = 1.0;
        gridbag.setConstraints(blank, c);
        buttonBar.add(blank);
    }

    private class VariableTreeTable
        extends Outline {

        // Column numbers in *row model* coordinates
        // Add 1 for Outline model coordinates
        private static final int IN_OUT_COL = 0;
        private static final int TYPE_COL = 1;
        private static final int VALUE_COL = 2;
	
        private RootNode root;
        private OutlineModel model;

        public VariableTreeTable(Node node) {
            super();
            setRenderDataProvider(new VariableRenderDataProvider());
            setDefaultRenderer(String.class, new VariableCellRenderer());

            DefaultTreeModel tm = new DefaultTreeModel(null, true);
            model = DefaultOutlineModel.createOutlineModel(tm,
                                                           new VariableRowModel(),
                                                           true, /* isLargeModel */
                                                           "Name");

            root = new RootNode(node);
            tm.setRoot(root);
            setModel(model);
            setRootVisible(true);

            // add mouse listener which puts up pop-up menus
            addMouseListener(new MouseAdapter() {
                    @Override
                    public void mousePressed(MouseEvent e) {   
                        if (e.getClickCount() == 2)
                            handleDoubleClickEvent(e);
                    
                    }
                });

            Enumeration<TableColumn> columns = getColumnModel().getColumns();
            columns.nextElement().setPreferredWidth(200);
            columns.nextElement().setPreferredWidth(50);
            columns.nextElement().setPreferredWidth(100);
            columns.nextElement().setPreferredWidth(550);
        }
    
        /**
         * Handles a double click and triggers a specified MouseEvent.
         * 
         * @param mouseEvent the double click event
         */

        public void handleDoubleClickEvent(MouseEvent mouseEvent) {
            TreePath nodePath = getClosestPathForLocation(mouseEvent.getX(), mouseEvent.getY());
            if (nodePath == null) {
                System.out.println("handleClickEvent: nodePath is null, event " + mouseEvent);
                return;
            }
            // *** TODO ***
            // int visible_row = tree.getRowForPath(nodePath);
            // if (tree.isExpanded(nodePath))
            //     tree.collapseRow(visible_row);  
            // else
            //     tree.expandRow(visible_row); 
        }
    
        /**
         * Expands all the nodes of this TreeTableView.
         */
        public void expandAllNodes() {
            // *** FIXME ***
            //     for (int i = 0; i < tree.getRowCount(); i++)
            //         tree.expandRow(i);
        }
    
        /**
         * Collapses all the node of this TreeTableView.
         */
        public void collapseAllNodes() {
            // *** FIXME ***
            // for (int i = tree.getRowCount() - 1; i > 0; i--)
            //     tree.collapseRow(i);
        }

        //
        // Tree data
        //

        abstract private class VNode
            implements TreeNode {
            abstract public VNode getRoot();
            abstract public VNode getChildByName(final String s);
            abstract public TreePath getTreePath();
            abstract public Icon getIcon();

            public VNode makeVariableNode(Variable v) {
                if (v instanceof ArrayVariable)
                    return new ArrayVariableNode((ArrayVariable) v, this);
                else
                    return new VariableNode(v, this);
            }
        }

        private final class RootNode
            extends VNode {
            private Node node;
            private Vector<VNode> children;

            public RootNode(Node node) {
                this.node = node;
                node.addChangeListener(new Node.ChangeAdapter() {
                        public void variableAssigned(Node n, String name, String value) {
                            if (n != node)
                                return; // shouldn't happen
                            VNode v = getChildByName(name);
                            if (v == null) {
                                // *** TEMP ***
                                System.out.println("Can't find variable " + name + " in node " + n.getNodeName());
                                return; // not supposed to happen
                            }
                            int row =
                                model.getLayout().getRowForPath(v.getTreePath());
                            if (row < 0) {
                                // *** TEMP ***
                                System.out.println("No row for variable " + name + " in node " + n.getNodeName());
                                return;
                            }
                            // For effect
                            model.setValueAt(value, row, VALUE_COL + 1);
                        }
                    });		

                Vector<Variable> vars = node.getVariableList();
                children = new Vector<VNode>(vars.size());
                for (Variable v : vars)
                    children.add(makeVariableNode(v));
            }

            public String toString() {
                return node.getNodeName();
            }

            public boolean equals(Object o) {
                if (this == o)
                    return true;
                if (!(o instanceof RootNode))
                    return false;
                RootNode other = (RootNode) o;
                return node.getNodeName().equals(other.node.getNodeName())
                    && children.size() == other.children.size(); 
            }

            public int hashCode() {
                return node.getNodeName().hashCode() * 31
                    + children.size();
            }

            //
            // TreeNode API
            //

            public Enumeration children() {
                return children.elements();
            }

            public boolean getAllowsChildren() {
                return true;
            }

            public TreeNode getChildAt(int idx) {
                return children.get(idx);
            }

            public int getChildCount() {
                return children.size();
            }

            public int getIndex(TreeNode node) {
                return children.indexOf(node);
            }

            public TreeNode getParent() {
                return null;
            }

            public boolean isLeaf() {
                return false;
            }

            //
            // VNode API
            //

            public VNode getRoot() {
                return this;
            }

            public TreePath getTreePath() {
                return new TreePath(this);
            }

            public VNode getChildByName(final String s) {
                if (s == null)
                    return null;
                VNode result = null;
                for (VNode v : children)
                    if (null != (result = v.getChildByName(s)))
                        return result;
                return null;
            }

            public Icon getIcon() {
                return Constants.getIcon(node.getType());
            }
        }

        private class VariableNode
            extends VNode {
            protected Variable m_var;
            private VNode parent;
            private TreePath treePath;
	
            public VariableNode(Variable var, VNode parent) {
                m_var = var;
                this.parent = parent;
                treePath = parent.getTreePath().pathByAddingChild(this);
            }

            public Variable getVariable() {
                return m_var;
            }

            public final String getName() {
                return m_var.getName();
            }

            public TreeNode getParent() {
                return parent;
            }

            public VNode getRoot() {
                return parent.getRoot();
            }

            public TreePath getTreePath() {
                return parent.getTreePath().pathByAddingChild(this);
            }

            public boolean equals(Object o) {
                if (this == o)
                    return true;
                if (!(o instanceof VariableNode))
                    return false;
                VariableNode other = (VariableNode) o;
                return getName().equals(other.getName());
            }

            public int hashCode() {
                return getName().hashCode();
            }

            //
            // TreeNode API
            //

            public boolean getAllowsChildren() {
                return false;
            }

            public boolean isLeaf() {
                return true;
            }

            public TreeNode getChildAt(int index) {
                return null;
            }
        
            public int getChildCount() {
                return 0;
            }

            public int getIndex(TreeNode node) {
                return -1;
            }

            //
            // VNode API
            //

            public String toString() {
                return m_var.getName();
            }
	
            public Enumeration<VNode> children() {
                return null;
            }

            public VNode getChildByName(final String s) {
                if (getName().equals(s))
                    return this;
                else
                    return null;
            }

            public Icon getIcon() {
                return DummyIcon.instance();
            }
        }

        private class ArrayVariableNode
            extends VariableNode {

            private Vector<VNode> children;

            public ArrayVariableNode(ArrayVariable v, VNode n) {
                super(v, n);
                children = new Vector<VNode>(v.size());
                for (Variable var : v.getVariables()) {
                    if (var != null)
                        children.add(makeVariableNode(var));
                }
            }

            //
            // Overrides to VariableNode behavior
            //

            @Override
            public boolean equals(Object o) {
                if (!super.equals(o))
                    return false;
                if (!(this instanceof ArrayVariableNode))
                    return false;
                ArrayVariableNode other = (ArrayVariableNode) o;
                return getChildCount() == other.getChildCount();
            }

            @Override
            public int hashCode() {
                return getName().hashCode() * 31
                    + children.size();
            }

            @Override
            public Enumeration<VNode> children() {
                return children.elements();
            }

            @Override
            public boolean getAllowsChildren() {
                return true;
            }

            @Override
            public TreeNode getChildAt(int index) {
                return children.get(index);
            }
        
            @Override
            public int getChildCount() {
                return children.size();
            }

            @Override
            public int getIndex(TreeNode node) {
                return children.indexOf(node);
            }

            @Override
            public boolean isLeaf() {
                return children == null;
            }

            @Override
            public VNode getChildByName(final String s) {
                if (m_var.getName().equals(s))
                    return this;
                VNode result = null;
                for (VNode v : children)
                    if (null != (result = v.getChildByName(s)))
                        return result;
                return null;
            }

        }

        private class VariableRowModel
            implements RowModel {

            public VariableRowModel() {
            }

            public int getColumnCount() {
                return 3;
            }

            public String getColumnName(int col) {
                switch (col) {
                case IN_OUT_COL:
                    return "In/InOut";
                case TYPE_COL:
                    return "Type";
                case VALUE_COL:
                    return "Value";
                default:
                    return null;
                }
            }

            public Class getColumnClass(int col) {
                return String.class;
            }

            public Object getValueFor(Object node, int column) {
                if (!(node instanceof VariableNode))
                    return null;

                Variable var = ((VariableNode) node).getVariable();
                switch(column) {
                case IN_OUT_COL:
                    return var.getInOut();
                case TYPE_COL:
                    return var.getType();
                case VALUE_COL:
                    return var.getValue();
                default:
                    return null;
                }
            }

            public void setValueFor(Object node, int column, Object value) {
            }

            public boolean isCellEditable(Object node, int column) {
                return false;
            }
        }

        //
        // VariableRenderDataProvider for tree column
        //

        private class VariableRenderDataProvider
            implements RenderDataProvider {

            public Color getBackground(Object o) {
                return null;
                // return getRowColor(??);
            }

            public Color getForeground(Object o) {
                return null;
            }

            public String getDisplayName(Object o) {
                return o.toString();
            }

            public Icon getIcon(Object o) {
                if (o instanceof VNode)
                    return ((VNode) o).getIcon();
                else
                    return null;
            }

            public String getTooltipText(Object o) {
                return null;
            }

            public boolean isHtmlDisplayName(Object o) {
                return false;
            }
        }

        //
        // VariableCellRenderer
        //
        // a table cell renderer for non-tree columns

        private class VariableCellRenderer
            extends DefaultTableCellRenderer {
            public Component getTableCellRendererComponent(JTable table,
                                                           Object value,
                                                           boolean isSelected,
                                                           boolean hasFocus,
                                                           int row,
                                                           int column) {
                Component component =
                    super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);
                String label = (String)value;
                Color color = Constants.lookupColor(label);
                component.setForeground(color != null
                                        ? color
                                        : Color.BLACK);
            
                component.setBackground(isSelected
                                        ? table.getSelectionBackground()
                                        : table.getBackground()); // was getRowColor(row);
                return component;
            }
        }
    
    }

}

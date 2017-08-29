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

import java.awt.Color;
import java.io.File;
import java.util.Enumeration;
import java.util.EnumMap;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.NoSuchElementException;
import java.util.Stack;
import java.util.Vector;

import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;

import static gov.nasa.luv.Constants.*;

import gov.nasa.luv.PlexilSchema.*;
import static gov.nasa.luv.PlexilSchema.*;

public class Node
    extends java.util.Properties
    implements TreeNode, Cloneable {

    /** A collection of all the possible Plexil Plan node types. */
    public static final Map<NodeType, String> NODE_TYPES =
        new HashMap<NodeType, String>(6) {
            {
                put(NodeType.NodeList,        "List Node");
                put(NodeType.Command,         "Command Node");
                put(NodeType.Assignment,      "Assignment Node");
                put(NodeType.Empty,           "Empty Node");
                put(NodeType.Update,          "Update Node");
                put(NodeType.LibraryNodeCall, "Library Node");
            }
        };

    protected String nodeName; 
    protected NodeType type;
    protected NodeState state;
    protected NodeOutcome outcome;
    protected NodeFailureType failureType;

    protected Node parent;

    protected TreePath treePath;

    protected NodeInfoWindow infoWindow;
    protected boolean highlight;

    protected Map<Condition, Vector<String> > conditionExprs;
    protected Map<Condition, String> conditionValues;
    protected Vector<Variable> variableList;
    protected Vector<String> actionList;
    protected Vector<LuvBreakPoint> breakPoints;
    protected Vector<ChangeListener> changeListeners;


    public static Node makeNode(NodeType typ) {
        switch (typ) {
        case NodeList:
            return new ListNode();

        case LibraryNodeCall:
            return new LibraryCallNode();

        default:
            return new Node(typ);
        }
    }

    protected Node(NodeType typ) {
        super();
        nodeName = null;
        type = typ;
        parent = null;
        treePath = null;
        infoWindow = null;
        highlight = false;

        conditionExprs = new EnumMap<Condition, Vector<String> >(Condition.EndCondition.getDeclaringClass());
        conditionValues = new EnumMap<Condition, String>(Condition.EndCondition.getDeclaringClass());
        variableList = new Vector<Variable>();
        actionList = new Vector<String>();
        breakPoints = new Vector<LuvBreakPoint>();
        changeListeners = new Vector<ChangeListener>();

        state = NodeState.INACTIVE;
        outcome = null;
        failureType = null;
    }

    /** Copy constructor. Used by clone(). */
    protected Node(Node orig) {
        super(orig); // copies properties
        nodeName = orig.nodeName;
        type = orig.type;
        state = orig.state;
        outcome = orig.outcome;
        failureType = orig.failureType;
        // parent will be supplied by caller
        conditionExprs = orig.conditionExprs;
        conditionValues = new EnumMap<Condition, String>(orig.conditionValues);
        variableList = new Vector<Variable>(orig.variableList.size());
        for (Variable v : orig.variableList) {
            variableList.add(v.clone());
        }

        actionList = orig.actionList;
        breakPoints = new Vector<LuvBreakPoint>();
        changeListeners = new Vector<ChangeListener>();
        infoWindow = null;
        highlight = false;
    }

    public Node clone() {
        return new Node(this);
    }

    public int hashCode() {
        int result = super.hashCode();
        result = result * 31 + getClass().getName().hashCode();
        result = result * 31 + type.hashCode();
        result = result * 31 +
            (nodeName == null ? 0 : nodeName.hashCode());
        return result;
    }

    public boolean equals(Object o) {
        if (this == o)
            return true; // identity

        if (!(o instanceof Node))
            return false;

        Node other = (Node) o;
        if (type != other.type) 
            return false;

        // nodeName should never be null when fully instantiated
        if (nodeName == null) {
            if (other.nodeName != null)
            return false;
        }
        else if (!nodeName.equals(other.nodeName))
            return false;

        // Redundant? Consider multiple library calls.
        if (treePath == null) {
            if (other.treePath != null)
                return false;
        }
        else if (other.treePath == null)
            return false;
        else if (treePath.getPathCount() != other.treePath.getPathCount())
            return false;

        if (!conditionExprs.equals(other.conditionExprs))
            return false;
        
        return super.equals(o); // compare properties
    }

    // For convenience of Plan class.
    public void findUnresolvedLibraryCalls(Plan plan) {
        // no-op for leaf nodes
    }

    public Enumeration children() {
        return null;
    }

    public boolean getAllowsChildren() {
        return false;
    }

    public TreeNode getChildAt(int childIndex) {
        return null;
    }

    public int getChildCount() {
        return 0;
    }

    public int getIndex(TreeNode node) {
        return -1;
    }

    public Node getParent() {
        return parent;
    }

    public boolean isLeaf() {
        return true;
    }

    //
    // Basic accessors
    //

    public String                       getNodeName()              { return nodeName; }
    public NodeType                     getNodeType()              { return type; }
    public NodeState                    getNodeState()             { return state; }
    public NodeOutcome                  getNodeOutcome()           { return outcome; }
    public NodeFailureType              getNodeFailureType()       { return failureType; }

    /** Returns the type of this Plexil Node as a String.
     *  @return the type */
    public String                       getType()                   { return type.toString(); }

    /** Returns the Vector of Node children of this Node.
     *  @return the children */
    public Vector<Node> getChildren() {
        return null;
    }

    public boolean hasChildren() {
        return false;
    }

    public TreePath getTreePath() {
        if (treePath == null)
            treePath = new TreePath(this); // must be root
        return treePath;
    }

    public Node getRootNode() {
        Node n = this;
        while (n.parent != null)
            n = n.parent;
        return n;
    }

    public NodeInfoWindow getInfoWindow() {
        return infoWindow;
    }

    public void setInfoWindow(NodeInfoWindow w) {
        infoWindow = w;
    }

    public boolean getHighlight() {
        return highlight;
    }

    public void setHighlight(boolean val) {
        highlight = val;
    }

    //
    // Breakpoint handling
    //

    public Vector<LuvBreakPoint> getBreakPoints() {
        return breakPoints;
    }

    public void clearBreakPoints() {
        breakPoints.clear();
    }

    public void addBreakPoint(LuvBreakPoint bp) {
        for (LuvBreakPoint l : breakPoints) {
            if (l.toString().equals(bp.toString())) {
                StatusMessageHandler.instance().showStatus("Breakpoint \"" + bp 
                                                           + "\" already set", Color.RED, 5000l);
                return;
            }
        }

        breakPoints.add(bp);
        addChangeListener(bp); // *** redundant? ***
        notifyAddBreakPoint(bp);
    }

    // Called from node menu
    public void removeBreakPoint(LuvBreakPoint bp) {
        // do UI bookkeeping first
        notifyRemoveBreakPoint(bp);
        deleteBreakPoint(bp);
    }

    // Called from PlanView remove-all-bps menu
    public void deleteBreakPoint(LuvBreakPoint bp) {
        removeChangeListener(bp);
        breakPoints.remove(bp);
    }

    synchronized private void notifyAddBreakPoint(LuvBreakPoint bp) {
        for (ChangeListener cl: changeListeners)
            cl.addBreakPoint(this, bp);
    }

    synchronized private void notifyRemoveBreakPoint(LuvBreakPoint bp) {
        for (ChangeListener cl: changeListeners)
            cl.removeBreakPoint(this, bp);
    }

    public String getState() {
        return state.toString();
    }

    public void setState(NodeState newval) {
        state = newval;
    }

    public String getOutcome() {
        return outcome.toString();
    }

    public void setOutcome(NodeOutcome newval) {
        outcome = newval;
    }

    public String getFailureType() {
        return failureType.toString();
    }

    public void setFailureType(NodeFailureType newval) {
        failureType = newval;
    }

    public void stateTransition(NodeState newState,
                                NodeOutcome newOutcome,
                                NodeFailureType newFailure,
                                Map<Condition, String> newConds) {
        if (state == newState)
            return;
        state = newState;
        if (outcome != newOutcome)
            outcome = newOutcome;
        if (failureType != newFailure)
            failureType = newFailure;
        notifyStateTransition(newState, newOutcome, newFailure, newConds);
    }

    synchronized private void notifyStateTransition(NodeState newState,
                                                    NodeOutcome newOutcome,
                                                    NodeFailureType newFailure,
                                                    Map<Condition, String> newConds) {
        for (ChangeListener cl: changeListeners)
            cl.stateTransition(this, newState, newOutcome, newFailure, newConds);
    }

    /** Returns the HashMap of Conditions for this Node.
     *  @return the Conditions */
    public Map<Condition, Vector<String> >  getConditionExprs() {
        return conditionExprs;
    }

    public Map<Condition, String> getConditionValues() {
        return conditionValues;
    }

    public String getConditionValue(Condition c) {
        return conditionValues.get(c);
    }

    /** Returns the vector of local variables for this Node.
     *  @return the variables */
    public Vector<Variable>     getVariableList()           { return variableList; }

    /** Returns the vector of actions for this Node.
     *  @return the actions */
    public Vector<String>            getActionList()             { return actionList; }

    /** Returns the Library Name for this Plexil Node. */
    public String getLibraryName() {
        // TODO
        return null;
    }

    /**
     * Sets the specified string for the library name of this Node.
     */
    public void setLibraryName(String libname) {
        // TODO
    }

    public boolean isRoot() {
        return parent == null;
    }

    /**
     * Returns the top level ancestor of this Node.
     * @return the top level ancestor
     */
    public Node topLevelNode() {
        if (parent == null)
            return this;
        return parent.topLevelNode();
    }
    
    /**
     * Returns whether or not this Node has local variables.
     * @return whether or not this Node has local variables
     */
    public boolean hasVariables()
    {
        return !variableList.isEmpty();
    }
    
    /**
     * Returns whether or not this Node has actions.
     * @return whether or not this Node has actions
     */
    public boolean hasAction()
    {
        return !actionList.isEmpty();
    }
    
    /**
     * Returns whether or not this Node has conditions.
     * @return whether or not this Node has conditions
     */
    public boolean hasConditions()
    {
        return !conditionExprs.isEmpty();
    }

    /**
     * Sets the specified name for this Node.
     * @param name the name for this Node
     */
    public void setNodeName(String name) {
        nodeName = name;
    }

    /**
     * Sets the specified Node as this Node's parent.
     * @param newParent the parent Node of this Node
     * @note Sets treePath as side effect 
     */

    public void setParent(Node newParent) {
        Node oldParent = parent;
        parent = newParent;
        updateTreePath(oldParent != null && oldParent != newParent);
    }

    // Re-rooting should only happen when linking a library into its caller.
    protected void updateTreePath(boolean isReRoot) {
        if (parent == null) {
            System.out.println("updateTreePath: new parent is null"); // shouldn't happen?
            treePath = new TreePath(this);
        }
        else
            treePath = parent.getTreePath().pathByAddingChild(this);            
    }
    
    /**
     * Sets the specified property value with key to this Node.
     * @param key the key to the specified value
     * @param value the value of a property for this Node
     * @return previous value of the property
     */
    public Object setProperty(String key, String value) {
        if (key == null || value == null) {
            // Properties.setProperty() throws an exception if either arg is null
            StatusMessageHandler.instance().showStatus("Warning: attempt to set property "
                                                       + (nodeName == null ? "" : " of node " + nodeName)
                                                       + "(key: " + key + ", type: " + type + ") to null");
            return null;
        }

        return super.setProperty(key, value);
    }

	public void setVariable(String vName, String value) {
        if (vName == null)
            return;

        setVariableInternal(vName, ArrayVariable.getBaseName(vName), value);
	}

    protected void setVariableInternal(String vName, String baseName, String value) {
        Variable var = findLocalVariable(baseName);
        if (var == null)
            return; // not found
        if (var instanceof ArrayVariable)
            ((ArrayVariable) var).setArrayIndexVariable(ArrayVariable.getIndex(vName), value);
        else 
            var.setValue(value);
        notifyVariableAssigned(vName, value);
    }

    // Subfunction of above
    protected Variable findLocalVariable(String vName) {
        for (Variable v : variableList)
            if (v.getName().equals(vName))
                return v;
        return null;
    }

    synchronized private void notifyVariableAssigned(String vName, String value) {
        for (ChangeListener cl : changeListeners)
            cl.variableAssigned(this, vName, value);
    }
    
    /**
     * Adds the specified condition and condition equation to the vector of conditions for this Node.
     * @param condition the condition type to add
     * @param conditionEquation the equation of the condition to add
     */
    public void addConditionInfo(Condition c, String conditionEquation) { 
        conditionExprs.put(c, formatCondition(conditionEquation));
    }
        
    /** 
     * Rewrites the condition information into standard Plexil syntax for
     * better user readability.
     *
     * @param condition the condition expression before it has been rewritten
     * @return the value the formatted condition expression
     */
    
    public static Vector<String> formatCondition(String condition)
    {
        String tempCondition = "";
        Vector<String> formattedCondition = new Vector<String>();
        
        if (condition != null)
            {
                if (condition.contains(SEPARATOR))
                    {
                        String array[] = condition.split(SEPARATOR); 

                        for (int i = 0; i < array.length; i++)
                            {
                                tempCondition += array[i] + " ";

                                if (array[i].equals("||") || array[i].equals("&&"))
                                    {
                                        formattedCondition.add(tempCondition);
                                        tempCondition = "";
                                    }
                            }

                        if (!tempCondition.equals(""))
                            formattedCondition.add(tempCondition);
                    }
                else if (!condition.equals(""))
                    {
                        formattedCondition.add(condition);
                    }
            }  
        else
            {
                formattedCondition.add("COULD NOT IDENTIFY CONDITION");
            }         
        
        return formattedCondition;
    }
    
    /** Rewrites the action information into standard Plexil syntax for
     *  better user readability.   
     *
     * @param expression action information before it has been rewritten
     */
     
    // FIXME: use StringBuilder?
    public static String formatAction(String expression) {
        String formattedExpression = "COULD NOT IDENTIFY ACTION";
  
        if (expression != null && expression.contains(SEPARATOR)) {
            String array[] = expression.split(SEPARATOR);      
            if (array.length > 0) {
                formattedExpression = "";
                for (int i = 0; i < array.length; i++)
                    formattedExpression += array[i] + " ";
            }
        }
        
        return formattedExpression;
    }

    /**
     * Adds the given variable to the list of variables for this Node.
     * @param v The local variable to add.
     */
    public void addVariableInfo(Variable v) {
    	variableList.add(v);
    }
    
    /**
     * Adds the specified action to the vector of actions for this Node.
     * @param action  the action to add
     */
    public void addActionInfo(String action) { 
        actionList.add(formatAction(action));
    }
    
    /**
     * Creates and adds the specified Node child to this Node.
     * @param child the node child
     */
    public void addChild(Node child) {
        // TODO: throw exception?
    }

    /**
     * Returns the Node pathway starting from the specified Node to the root Node.
     * @param node the node to return the pathway for
     * @return the pathway
     */
    public Stack<String> pathToNode(Node node) {
        Stack<String> node_path = new Stack<String>();
        while (!node.isRoot()) {
            if (!AbstractNodeFilter.isNodeFiltered(node))
                node_path.push(node.getNodeName());
            node = node.getParent();
        }
        return node_path;
    }
    
    
    /**
     * Returns the Node that matches with the specified name.
     * @param name the name to match the Node to
     * @return the matching Node
     */
    public Node findChildByName(String name) {
        return null;
    }

    // Meant to be called on the root node.
    public Node getNode(final String[] path) {
        if (path == null || path.length == 0)
            return null;
        if (!nodeName.equals(path[0]))
            return null; // wrong root
        Node n = this;
        for (int i = 1; i < path.length; ++i)
            if (null == (n = n.findChildByName(path[i])))
                return null;
        return n;
    }

    // Leaf node method
    synchronized public void addChangeListenerToAll(ChangeListener listener) {
        changeListeners.add(listener);
    }

    // Leaf node method
    synchronized public void removeChangeListenerFromAll(ChangeListener listener) {
        changeListeners.remove(listener);
    }
            
    /**
     * Adds a property change listener to this Node. 
     * @param listener the property change listener
     */
    synchronized public void addChangeListener(ChangeListener listener) {
        changeListeners.add(listener);
    }

    /**
     * Removes a property change listener to this Node. 
     * @param listener the property change listener
     */
    synchronized public void removeChangeListener(ChangeListener listener) {
        changeListeners.remove(listener);
    }
         
    /**
     * Resets all the properties of this Node to the beginning values (pre-execution).
     */
    public void reset() {
        setState(NodeState.INACTIVE);
        setOutcome(null);
        setFailureType(null);

        // Only reset values of properties this node has
        for (Condition c : conditionExprs.keySet())
            conditionValues.put(c, UNKNOWN);

        for (Variable v : variableList)
            setVariable(v.getName(), UNKNOWN);
    }

    public void setMainAttributesOfNode()
    {
        String rawType = getProperty(NODETYPE_ATTR);
        if (rawType != null) {
            String polishedType = NODE_TYPES.get(rawType);
            if (polishedType != null)
                setProperty(NODETYPE_ATTR, polishedType);
        }
    }

    public String toString() {
        return nodeName;
    }

    public String toStringVerbose() {
        StringBuilder s = new StringBuilder();
        toStringInternal(s);
        return s.toString();
    }

    protected void toStringInternal(StringBuilder s) {
        if (nodeName == null)
            s.append(type);
        else
            s.append(nodeName);

        s.append("(");
        for (Entry<Object, Object> property: entrySet()) {
            s.append(" ");
            s.append(property.getKey());
            s.append(" = ");
            s.append(property.getValue());
        }
        s.append(")");
    }
        
    /**
     * Links a copy of the specified library into this Node.
     * @param library the library for this Node
     * @return whether or not the library was linked
     */
    public boolean linkLibrary(Plan library) {
        return false;
    }
      
    /**
     * Creates a LuvBreakPoint which fires when the specified node state changes at all.
     * 
     * @param node the node on which the LuvBreakPoint fires
     * @return the LuvBreakPoint
     */
    public LuvBreakPoint createChangeBreakpoint()
    {
        return new LuvBreakPoint(this,
                                 new StateChangeFilter());
    }
      
    /**
     * Creates a LuvBreakPoint which fires when the specified node reaches the target state.
     * 
     * @param node the node on which the LuvBreakPoint fires
     * @return the LuvBreakPoint
     */
    public LuvBreakPoint createNodeStateValueBreakpoint(final NodeState targetState) {
        return new LuvBreakPoint(this,
                                 new StateValueFilter(targetState));
    }
      
    /**
     * Creates a LuvBreakPoint which fires when the specified node reaches the target outcome.
     * 
     * @param node the node on which the LuvBreakPoint fires
     * @return the LuvBreakPoint
     */
    public LuvBreakPoint createNodeOutcomeValueBreakpoint(final NodeOutcome targetOutcome) {
        return new LuvBreakPoint(this,
                                 new OutcomeValueFilter(targetOutcome));
    }

    /**
     * Creates a LuvBreakPoint which fires when the node reaches the target failure type.
     * 
     * @param node the node on which the LuvBreakPoint fires
     * @return the LuvBreakPoint
     */
    public LuvBreakPoint createNodeFailureValueBreakpoint(final NodeFailureType targetFailureType) {
        return new LuvBreakPoint(this,
                                 new FailureTypeValueFilter(targetFailureType));
    }
    
    /**
     * The ChangeListener is an abstract class that is signaled when the Plexil Node 
     * is changed in some way.
     */
    public static interface ChangeListener {
        public void stateTransition(Node node,
                                    NodeState state,
                                    NodeOutcome outcome,
                                    NodeFailureType failure,
                                    Map<Condition, String> conditions);
	
        public void variableAssigned(Node node, String variableName, String value);

        // UI
        public void addBreakPoint(Node node, LuvBreakPoint bp);
        public void removeBreakPoint(Node node, LuvBreakPoint bp);

    }

    /**
     * The ChangeAdapter class is signaled when the Plexil node is changed in some way.
     */
    public static class ChangeAdapter implements ChangeListener {
        public void stateTransition(Node node,
                                    NodeState state,
                                    NodeOutcome outcome,
                                    NodeFailureType failure,
                                    Map<Condition, String> conditions)
        {}
	
        public void variableAssigned(Node node, String variableName, String value) {}

        public void addBreakPoint(Node node, LuvBreakPoint bp) {};

        public void removeBreakPoint(Node node, LuvBreakPoint bp) {};
    }

    public static interface StateTransitionFilter {
        public boolean eventMatches(Node n, 
                                    NodeState state,
                                    NodeOutcome outcome,
                                    NodeFailureType failure);
        public String getDescription();
    }

    public static class StateChangeFilter
        implements StateTransitionFilter {

        StateChangeFilter() {
        }

        public boolean eventMatches(Node n,
                                    NodeState state,
                                    NodeOutcome outcome,
                                    NodeFailureType failure) {
            return true;
        }

        public String getDescription() {
            return " state changed";
        }
    }

    public static class StateValueFilter
        implements StateTransitionFilter {

        private final NodeState targetState;
        private final String description;

        StateValueFilter(final NodeState target) {
            targetState = target;
            description = " state changed to " + target.toString();
        }

        public boolean eventMatches(Node n,
                                    NodeState newState,
                                    NodeOutcome newOutcome,
                                    NodeFailureType newFailure) {
            return targetState == newState;
        }

        public String getDescription() {
            return description;
        }
    }

    public static class OutcomeValueFilter
        implements StateTransitionFilter {

        private final NodeOutcome targetOutcome;
        private final String description;

        OutcomeValueFilter(final NodeOutcome target) {
            targetOutcome = target;
            description = " outcome changed to " + target.toString();
        }

        public boolean eventMatches(Node n,
                                    NodeState newState,
                                    NodeOutcome newOutcome,
                                    NodeFailureType newFailure) {
            return targetOutcome == newOutcome;
        }

        public String getDescription() {
            return description;
        }
    }

    public static class FailureTypeValueFilter
        implements StateTransitionFilter {

        private final NodeFailureType targetFailureType;
        private final String description;

        FailureTypeValueFilter(final NodeFailureType target) {
            targetFailureType = target;
            description = " failure type changed to " + target.toString();
        }

        public boolean eventMatches(Node n,
                                    NodeState newState,
                                    NodeOutcome newOutcome,
                                    NodeFailureType newFailure) {
            return targetFailureType == newFailure;
        }

        public String getDescription() {
            return description;
        }
    }
    
}

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

package model;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import main.Log;
import model.Condition.ConditionType;
import model.expr.Expr;
import model.expr.ExprType;

public class Node {
	
	private String id;
	private Node parent;
	private NodeList children;
	private VarList varDefs;
	private ConditionList conditions;
	private Action action;

    public Node() {
        id = null;
        parent = null;
        children = null;
        varDefs = null;
        conditions = null;
        action = null;
    }
	
	public String getID() {
		return id;
	}

	public void setID(String id) {
		this.id = id;
	}

	public Node getParent() {
		return parent;
	}

	public void setParent(Node p) {
		parent = p;
	}

	public NodeList getChildren() {
		return children;
	}

	public void addChild(Node c) {
        if (children == null)
            children = new NodeList();
		children.add(c);
        c.setParent(this);
	}

	public VarList getVarDefs() {
		return varDefs;
	}

	public void addVarDef(Var v) {
        if (varDefs == null)
            varDefs = new VarList();
		varDefs.add(v);
	}

	public void addVarDefs(VarList v) {
        if (varDefs == null)
            varDefs = new VarList();
		this.varDefs.addAll(v);
	}

	public ConditionList getConditions() {
		return conditions;
	}

	public void addCondition(Condition c) {
        if (conditions == null)
            conditions = new ConditionList();
		conditions.add(c);
	}

	public Action getAction() {
		return action;
	}

	public void setAction(Action a) {
		action = a;
	}
	
	public boolean isNamed(String name) {
		if (name == null || id == null)
			return false;
		return id.equals(name);
	}
	
	public boolean hasChildNamed(String name) {
		if (name == null || children == null)
			return false;
		for (Node child : children)
			if (child.isNamed(name))
				return true;
		return false;
	}
	
	public Var findVarInScope(String id) {
        if (id == null)
            return null;

        if (varDefs != null)
            for (int i = 0; i < varDefs.size(); i++) {
                Var v = varDefs.get(i);
                if (v.getID().equals(id))
                    return v;
            }
		
		if (parent == null)
			return null;
		
		return parent.findVarInScope(id);
	}

	public void print() {
		System.out.println(this.toString());
		if (varDefs != null) {
			for (Var v : varDefs)
				System.out.println("  " + v.toString() + " Variable");
		}
		if (conditions != null) {
			for (Condition c : conditions)
				System.out.println("  " + c.getExpr().toString() + " :: " + c.getType().toString() + " Condition");
		}
	}
	
	@Override
	public String toString()
	{
		if (getAction() != null)
			 return (id + " :: " + action.getType().toString() + " Node");
		else
			return (id + " :: UnknownType Node");
	}

    public void check(GlobalDeclList decls, Vector<Log> errors) {
        checkNodeID(errors);
        if (varDefs != null) {
            // Check for duplicates
            Map<String, Var> vars = new HashMap<String, Var>(varDefs.size());
            for (Var v : varDefs) {
                String id = v.getID(); // it better have one! Can detect that during parse.
                if (id != null) {
                    Var x = vars.get(id);
                    if (x == null)
                        vars.put(id, v);
                    else
                        errors.add(varNameConflictMessage(x, v));
                }
                checkVarDecl(v, decls, errors);
            }
        }
        if (conditions != null) {
            // Check for duplicates
            Map<ConditionType, Condition> conds =
                new HashMap<ConditionType, Condition>(conditions.size());
            for (Condition c : conditions) {
                Condition x = conds.get(c.getType()); // it better have one! can detect during parse
                if (x == null)
                    conds.put(c.getType(), c);
                else
                    errors.add(conditionConflictMessage(c.getType()));
                checkCondition(c, decls, errors);
            }
        }

        if (action != null)
            action.check(this, decls, errors);

        // Recurse on children
        if (children != null)
            for (Node child : children)
                child.check(decls, errors);
    }

    private void checkNodeID(Vector<Log> errors) {
        // N.B. This check wouldn't be needed if the input was known to validate
        // against the Core PLEXIL schema.
        if (id == null)
            errors.add(missingNodeIDMessage());
        else {
            // Check that node ID differs from parent's
            if (parent != null) {
                String pid = parent.getID();
                if (pid != null && pid.equals(id)) {
                    errors.add(nodeIDSameAsChildMessage(parent));
                }
            }
        }

        // Check that children are uniquely named
        if (children != null) {
            Map<String, Node> ids = new HashMap<String, Node>(children.size());
            for (Node k : children) {
                String kid = k.getID();
                if (kid == null)
                    continue; // lack of ID will get reported later
                Node x = ids.get(kid);
                if (x == null)
                    ids.put(kid, k);
                else
                    errors.add(siblingNodesWithSameIDMessage(k, x));
            }
        }
    }

    private void checkVarDecl(Var v, GlobalDeclList decls, Vector<Log> errors) {
        Expr init = v.getInitializer();
        if (init != null) {
            ExprType t = init.check(parent, decls, "initial value of " + v.getID(), errors);
            if (t != null)
                Expr.checkType(init, t, v.getType(), "initial value of " + v.getID(), null, errors);
        }
    }

    private void checkCondition(Condition c, GlobalDeclList decls, Vector<Log> errors) {
        String contextMsg = c.getType().toString() + " Condition of node " + id;
        Expr cx = c.getExpr();
        ExprType t = cx.check(this, decls, contextMsg, errors);
        Expr.checkType(cx,
                       t,
                       ExprType.Bool,
                       contextMsg,
                       null,
                       errors);
    }

    private Log missingNodeIDMessage() {
        // FIXME: improve error report
        return Log.error("Node has no NodeId");
    }

    private Log nodeIDSameAsChildMessage(Node p) {
        // TODO?: improve error report
        return Log.error("Node " + p.getID() + " has a child with the same NodeId");
    }

    private Log siblingNodesWithSameIDMessage(Node a, Node b) {
        // TODO?: improve error report
        return Log.error("Child nodes of " + id + " have same NodeId " + a.getID());
    }

    private Log varNameConflictMessage(Var a, Var b) {
        return Log.error("Node " + id + " has multiple variables named " + a.getID());
    }

    private Log conditionConflictMessage(ConditionType c) {
        return Log.error("Node " + id + " has multiple " + c.toString() + " conditions");
    }

}

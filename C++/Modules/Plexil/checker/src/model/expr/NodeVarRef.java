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

package model.expr;

import java.util.Vector;

import main.Log;
import model.GlobalDeclList;
import model.Node;

public class NodeVarRef
    extends Expr {

    protected ExprType type;
    protected String id;
    protected NodeRefDir direction;

    public enum NodeRefDir {
        Self,
        Parent,
        Child,
        Sibling;

        public static NodeRefDir parse(String s) {
            if (s == null)
                return null;
            switch (s) {
            case "self":
                return Self;

            case "parent":
                return Parent;

            case "child":
                return Child;

            case "sibling":
                return Sibling;

            default:
                return null;
            }
        }
    }

    public NodeVarRef(ExprType t, String nodeId, NodeRefDir dir) {
        super();
        type = t;
        id = nodeId;
        direction = dir;
    }

    public ExprType getType() {
        return type;
    }

    public String toString() {
        StringBuilder s = new StringBuilder("(");
        s.append(typeAsExprName());
        s.append(" ");
        s.append(nodeRefToString());
        s.append(")");
        return s.toString();
    }

    protected String nodeRefToString() {
        if (direction == null)
            return "NodeId " + id;
        return "NodeRef "
            + direction.toString()
            + (id == null ? "" : " " + id);
    }

    protected String typeAsExprName() {
        switch (type) {
        case NodeState:
            return "NodeStateVar";
            
        case NodeOutcome:
            return "NodeOutcomeVar";

        case NodeFailureType:
            return "NodeFailureVar";

        case NodeTimepointValue:
            return "NodeTimepointValue";
            
        case NodeCommandHandle:
            return "NodeCommandHandleVar";

        default:
            return "NodeVarRef *ERROR* ";
        }
    }
    
    /**
     * @brief Check the expression for type and other errors.
     * @param n The node providing the variable binding context.
     * @param decls The plan's global declarations.
     * @param contextMsg String to append to any error messages generated.
     * @param errors (in/out parameter) Collection of errors recorded.
     */
    public ExprType check(Node n,
                          GlobalDeclList decls,
                          String contextMsg,
                          Vector<Log> errors) {
        checkNodeRef(n, contextMsg, errors);
        return type;
    }

    protected void checkNodeRef(Node n,
                                String contextMsg,
                                Vector<Log> errors) {
        if (direction == null) {
            // TODO: search for node
        }
        else {
            switch (direction) {
            case Self:
                if (id != null)
                    errors.add(Log.warning("NodeRef with dir of \"self\" must not have a node ID"));
                return;

            case Child:
                if (id == null) {
                    errors.add(Log.error("NodeRef with dir of \"child\" must have a node ID"));
                    return;
                }
                if (!n.hasChildNamed(id))
                    errors.add(Log.error("Node " + n.getID() + " has no child named " + id));
                return;

            case Parent:
                if (id != null)
                    errors.add(Log.warning("NodeRef with dir of \"parent\" must not have a node ID"));
                if (n.getParent() == null)
                    errors.add(Log.error("Node " + n.getID() + " has no parent"));
                return;

            case Sibling:
                if (id == null) {
                    errors.add(Log.error("NodeRef with dir of \"sibling\" must have a node ID"));
                    return;
                }
                if (n.getParent() == null) {
                    errors.add(Log.error("Node " + n.getID() + " has no parent, therefore no siblings"));
                    return;
                }
                if (!n.getParent().hasChildNamed(id))
                    errors.add(Log.error("Node " + n.getID() + " has no sibling named " + id));
                return;
            }
        }
    }

}

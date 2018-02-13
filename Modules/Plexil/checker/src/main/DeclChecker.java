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

package main;

import model.*;
import model.Action.ActionType;
import model.Condition;
import model.Condition.ConditionType;
import model.expr.*;
import model.expr.Expr.*;
import model.GlobalDecl.CallType;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

public class DeclChecker {
    
    public Vector<Log> checkPlan(Plan p) {
        Vector<Log> errors = new Vector<Log>();
        checkDeclRedundancy(p.getDecls(), errors);
        p.getNode().check(p.getDecls(), errors);
        return errors;
    }

    private void checkDeclRedundancy(GlobalDeclList decls, Vector<Log> errors) {
        Map<String, GlobalDecl> ids = new HashMap<String, GlobalDecl>(decls.size());
        for (GlobalDecl decl : decls) {
            GlobalDecl existing = ids.get(decl.getID());
            if (existing != null) 
                errors.add(nameConflictMessage(decl, existing));
            else
                ids.put(decl.getID(), decl);
        }
    }
    
    // Error message list: These could possibly be combined
    
    private Log nameConflictMessage(GlobalDecl a, GlobalDecl b) {
        return Log.error("Name conflict between declarations " + a.toString() + " and " + b.toString());
    }
    
    public Log nonStaticNameMessage(Expr e, Node n)
    {
        return Log.warning("Cannot statically determine correctness of name " + e.toString() + " in " + n.getID());
    }
    
    public Log noDeclarationMatchMessage(Node n)
    {
        return Log.warning("No declaration matches " + n.getAction().toString() + " in " + n.toString());
    }
    
    public Log inaccessibleNodeMessage(Node n, String name)
    {
        return Log.warning("Node " + name + " is not accessible from scope of node " + n.getID());
    }
    
    public Log expressionTypeErrorMessage(Expr e, ExprType type, ExprType expectedType)
    {
        String hasTypeMessage =
            (type == null) ? "unknown type" : "type " + type.toString();
        String expectedMessage =
            (expectedType == null) ? "unknown expected type" : "expected type" + expectedType.toString();
        return Log.error("Expr " + e.toString() + " with " + hasTypeMessage
                         + " does not match " + expectedMessage);
    }

    public Log noLookupDeclarationMessage(LookupExpr le, String extra)
    {
        String s = "No declaration matches " + le.getStaticID().toString();
        if (extra != null && !extra.equals(""))
            s += " " + extra;
        return Log.warning(s);
    }
    
    public Log lookupMismatchMessage(GlobalDecl d, LookupExpr le, String extra)
    {
        if (d == null)
            return noLookupDeclarationMessage(le, extra);
        String s = "Found Lookup " + d.getID() + ", but type " + d.getType().toString()
            + " does not match expected type " + le.getType().toString();
        if (extra != null && !extra.equals(""))
            s += " " + extra;
        return Log.error(s);
    }

    public Log assignmentMismatchMessage(Var v, ExprType e, Node n)
    {
        String s = "Var " + v.toString()
            + " does not match RHS expected type " + e.toString()
            + " in " + n.toString();
        return Log.error(s);
    }

    public Log arrayMisuseMessage(Var v, Expr e)
    {
        if (!(v instanceof VarArray))
            return Log.error(v.getID() + " in expression " + e.toString() + " is not an array.");
        else
            return Log.error(v.getID() + " in expression " + e.toString() + " needs to be accessed as a single array element.");
    }

}

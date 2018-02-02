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

import java.util.Vector;

import main.Log;
import model.expr.Expr;
import model.expr.ExprList;
import model.expr.ExprType;
import model.expr.LiteralExpr;
import model.GlobalDecl;
import model.GlobalDecl.CallType;

public class Command
    extends Action {

    private Expr id;
    private Expr lhs;
    private GlobalDecl decl;
    private ExprList arglist;

    public Command(Expr idExp, Expr returnVar, GlobalDecl dec) {
        super(ActionType.Command);
        id = idExp;
        lhs = returnVar;
        decl = dec;
        arglist = null;
    }

    public Expr getID() {
        return id;
    }

    @SuppressWarnings("unchecked")
    public String getStaticID() {
        if (id instanceof LiteralExpr
            && id.getType() == ExprType.Str) {
            return ((LiteralExpr) id).getValueString();
        }
        return null;
    }

    public Expr getLHS() {
        return lhs;
    }

    public ExprList getArgs() {
        return arglist;
    }

    public void addArgument(Expr e) {
        if (arglist == null)
            arglist = new ExprList();
        arglist.add(e);
    }

    public void check(Node node, GlobalDeclList decls, Vector<Log> errors) {
        ExprType idType = id.check(node,
                                   decls,
                                   "Command name expression",
                                   errors);
        String cmdName = null;
        // check that ID is a string expression
        switch (id.getType()) {
        case A:
            errors.add(Log.warning("Command name expression type is unknown, should be Str"));
            break;

        case Str:
            if (id instanceof LiteralExpr) {
                // This should have been determined by parser
                if (decl == null)
                    errors.add(Log.warning("Command " + getStaticID()
                                           + " is not declared"));
            }
            break;

        default:
            errors.add(Log.warning("Command name expression has type "
                                   + idType.toString() + ", should be Str"));
            break;
        }

        if (decl != null) {
            if (decl.getType() != CallType.Command) {
                errors.add(Log.error("Command name " + decl.getID()
                                     + " is declared as a " + decl.getType().toString()));
                decl = null; // can't do any more with it
            }
        }

        // check return expression if any
        if (decl != null) {
            if (lhs != null) {
                Var retDef = decl.getReturn();
                if (retDef == null)
                    errors.add(Log.error("Command " + decl.getID()
                                         + " called with a return variable, but doesn't return a value"));
                else 
                    Expr.checkType(lhs,
                                   lhs.getType(),
                                   retDef.getType(),
                                   "return assignment of Command " + decl.getID(),
                                   null,
                                   errors);
            }
        }

        // check arglist if any
        if (arglist == null) {
            if (decl != null && !decl.getArgs().isEmpty()) {
                errors.add(Log.error("Command " + decl.getID()
                                     + " called with no arguments, but expects "
                                     + decl.getArgs().size()));
            }
        }
        else {
            int nargs = arglist.size();
            Vector<ExprType> argtypes = new Vector<ExprType>(nargs);
            for (Expr arg : arglist) {
                argtypes.add(arg.check(node, decls, " in Command", errors));
            }
            if (decl != null) {
                VarList formalArgs = decl.getArgs();
                if (formalArgs.size() != nargs)
                    errors.add(Log.error("Command " + decl.getID()
                                         + " called with " + nargs + " arguments, but expects "
                                         + decl.getArgs().size()));
                else {
                    for (int i = 0; i < nargs; ++i) {
                        Expr actual = arglist.get(i);
                        Expr.checkType(actual,
                                       actual.getType(),
                                       formalArgs.get(i).getType(),
                                       "argument " + i + " of Command " + decl.getID(),
                                       null,
                                       errors);
                    }
                }
            }
        }
    }

    @Override
    public String toString() {
        StringBuilder s = new StringBuilder();
        if (lhs != null) {
            s.append(lhs.toString());
            s.append(" = ");
        }
        s.append("Command ");
        String sid = getStaticID();
        if (sid != null) {
            s.append(sid);
            s.append(" ");
        }
        if (arglist != null) {
            s.append("(");
            s.append(Integer.toString(arglist.size()));
            s.append(" args)");
        }
        return s.toString();
    }
}

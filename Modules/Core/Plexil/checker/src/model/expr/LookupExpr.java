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
import model.GlobalDecl;
import model.GlobalDecl.CallType;
import model.GlobalDeclList;
import model.Node;
import model.Var;
import model.VarList;

public class LookupExpr
    extends Expr {
	
	private ExprList arguments;
    private Expr stateName;
    private Expr tolerance;
    private GlobalDecl decl;

	public LookupExpr(Expr name, GlobalDecl def) {
        super();
        arguments = new ExprList();
        stateName = name;
        tolerance = null;
        decl = def;
	}

	public ExprList   getArgs() { return arguments; }
    public Expr       getStateName() { return stateName; }
    public Expr       getTolerance() { return tolerance; }
    public GlobalDecl getDecl() { return decl; }

    public void setTolerance(Expr t) {
        tolerance = t;
    }

    public void addArgument(Expr e) {
        arguments.add(e);
    }

    public ExprType getType() {
        if (decl == null)
            return ExprType.A;
        return decl.getReturn().getType();
    }

    @SuppressWarnings("unchecked")
    public String getStaticID() {
        if (stateName instanceof LiteralExpr)
            return ((LiteralExpr) stateName).getValueString(); // unchecked cast
        return null;
    }
	
    public String toString() {
        StringBuilder s = new StringBuilder("(Lookup");
        s.append(" ");

        String id = getStaticID();
        if (id == null)
            s.append(stateName.toString());
        else {
            s.append(getType().toString());
            s.append(" ");
            s.append(id);
        }

        int n = arguments.size();
        if (n != 0) {
            s.append("(");
            s.append(Integer.toString(n));
            s.append(" parameter");
            if (n > 1)
                s.append("s");
            s.append(")");
        }

        if (tolerance != null) {
            s.append(" tolerance ");
            s.append(tolerance.toString());
        }

        s.append(")");
        return s.toString();
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
        ExprType nameType = stateName.check(n, decls, contextMsg, errors);
        ExprType resultType = ExprType.A; // unknown type by default
        if (nameType != null) {
            switch (stateName.getType()) {
            case A:
                errors.add(Log.warning("Lookup state name type cannot be determined, should be Str",
                                       contextMsg));
                break;

            case Str:
                break;

            default:
                errors.add(Log.error("Lookup state name has type " + stateName.getType().toString()
                                     + ", should be Str",
                                     contextMsg));
                break;
            }
        }

        String name = getStaticID();
        if (name != null && decl == null)
            errors.add(Log.warning("Lookup " + name + " is not declared",
                                   contextMsg));
        
        if (decl != null) {
            if (decl.getType() == CallType.Lookup)
                resultType = decl.getReturn().getType();
            else {
                errors.add(Log.error("Lookup name " + decl.getID()
                                     + " is declared as a " + decl.getType().toString(),
                                     contextMsg));
                decl = null; // can't do any more with it
            }
        }
        if (tolerance != null)
            checkTolerance(n, decls, contextMsg, errors); // for effect
        checkArgs(n, decls, contextMsg, errors); // for effect
        return resultType;
    }

    private void checkArgs(Node n,
                           GlobalDeclList decls,
                           String contextMsg,
                           Vector<Log> errors) {
        // Precheck any supplied parameters
        int argSize = arguments.size();
        Vector<ExprType> argTypes = new Vector<ExprType>(argSize);
        for (Expr e : arguments)
            argTypes.add(e.check(n, decls, contextMsg, errors));

        if (decl != null) {
            VarList declArgs = decl.getArgs();
            int declSize = declArgs.size();
            if (declSize != argSize)
                errors.add(Log.error("Lookup " + decl.getID()
                                     + " requires " + declSize
                                     + " parameters, but " + argSize
                                     + " were supplied",
                                     contextMsg));
            else {
                for (int i = 0; i < argSize; ++i) {
                    Expr a = arguments.get(i);
                    Expr.checkType(a,
                                   a.getType(),
                                   declArgs.get(i).getType(),
                                   "argument " + i  + " of Lookup " + decl.getID(),
                                   contextMsg,
                                   errors);
                }
            }
        }
    }

    private void checkTolerance(Node n,
                                GlobalDeclList decls,
                                String contextMsg,
                                Vector<Log> errors) {
        ExprType tolType = tolerance.check(n, decls, contextMsg, errors);
        if (tolType == null)
            return; // error in check
        switch (tolType) {
        case A:
            errors.add(Log.warning("LookupOnChange tolerance type is unknown",
                                   contextMsg));
            return;

        case Real:
        case Int:
            break;

        default:
            errors.add(Log.error("Illegal LookupOnChange tolerance type " + tolType.toString(),
                                 contextMsg));
            return;
        }

        if (decl == null)
            return;

        // Check against declared type
        ExprType r = decl.getReturn().getType();
        if (r == null || r == ExprType.A)
            return; // nothing to learn
        switch (r) {
        case Int:
            if (tolType != r)
                errors.add(Log.error("Illegal Real tolerance for Int Lookup " + decl.getID(),
                                     contextMsg));
            return;

        case Real:
            return; // all OK

        default:
            errors.add(Log.error("Tolerance is illegal for Lookup " + decl.getID()
                                 + " of type " + r.toString(),
                                 contextMsg));
            return;
        }
    }

}

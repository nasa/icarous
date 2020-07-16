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

public abstract class Expr { 

	private Expr parent;

    // Should be used only by derived classes
    protected Expr() {
        parent = null;
    }
	
	public Expr getParent() {
		return parent;
	}

	public void setParent(Expr p) {
		parent = p;
	}

    abstract public ExprType getType();

    abstract public String toString();

    // Default method.
    public boolean isConstant() {
        return false;
    }

    // Default method.
    public boolean isAssignable() {
        return false;
    }

    /**
     * @brief Check the expression for type and other errors.
     * @param n The node providing the variable binding context.
     * @param decls The plan's global declarations.
     * @param contextMsg String to append to any error messages generated.
     * @param errors (in/out parameter) Collection of errors recorded.
     */
    abstract public ExprType check(Node n,
                                   GlobalDeclList decls,
                                   String contextMsg,
                                   Vector<Log> errors);


    //
    // Utilities for derived classes (should be protected rather than public?)
    //

    // Checks expression e with type t against a fixed expected type.
    public static void checkType(Expr e,
                                 ExprType t,
                                 ExprType expected,
                                 String parentDesc,
                                 String contextMsg,
                                 Vector<Log> errors) {
        if (t == null)
            return;
        if (t == ExprType.A)
            logUnknownTypeWarning(parentDesc,
                                  e.toString(),
                                  expected,
                                  contextMsg,
                                  errors);
        else if (expected == ExprType.Real)
            checkNumericType(e,
                             t,
                             parentDesc,
                             contextMsg,
                             errors);
        else if (t != expected)
                logTypeError(parentDesc,
                             e.toString(),
                             t,
                             expected,
                             contextMsg,
                             errors);
    }

    // Checks expression e with type t for a numeric type.
    public static void checkNumericType(Expr e,
                                        ExprType t,
                                        String parentDesc,
                                        String contextMsg,
                                        Vector<Log> errors) {
        if (t == null)
            return;
        if (t == ExprType.A)
            logNumericUnknownTypeWarning(parentDesc,
                                         e.toString(),
                                         contextMsg,
                                         errors);
        else if (!t.isNumeric())
            logNumericTypeError(parentDesc,
                                e.toString(),
                                t,
                                contextMsg,
                                errors);
    }

    // Checks expression e with type t for an array type.
    public static void checkArrayType(Expr e,
                                      ExprType t,
                                      String parentDesc,
                                      String contextMsg,
                                      Vector<Log> errors) {
        if (t == null)
            return;
        if (t == ExprType.A)
            logArrayUnknownTypeWarning(parentDesc,
                                       e.toString(),
                                       contextMsg,
                                       errors);
        else if (!t.isArrayType())
            logArrayTypeError(parentDesc,
                              e.toString(),
                              t,
                              contextMsg,
                              errors);
    }

    public static void logUnknownTypeWarning(String expDesc,
                                             String argDesc,
                                             ExprType expected,
                                             String contextMsg,
                                             Vector<Log> errors) {
        errors.add(Log.warning("In " + expDesc +
                               ": type of subexpression " + argDesc
                               + " is unknown"
                               + (expected == null
                                  ? ""
                                  : ", should be " + expected.toString()),
                               contextMsg));
    }

    public static void logNumericUnknownTypeWarning(String expDesc,
                                                    String argDesc,
                                                    String contextMsg,
                                                    Vector<Log> errors) {
        errors.add(Log.warning("In " + expDesc +
                               ": type of subexpression " + argDesc
                               + " is unknown, should be Int or Real",
                               contextMsg));
    }

    public static void logArrayUnknownTypeWarning(String expDesc,
                                                  String argDesc,
                                                  String contextMsg,
                                                  Vector<Log> errors) {
        errors.add(Log.warning("In " + expDesc +
                               ": type of subexpression " + argDesc
                               + " is unknown, should be an array",
                               contextMsg));
    }

    public static void logTypeError(String expDesc,
                                    String argDesc,
                                    ExprType actual,
                                    ExprType expected,
                                    String contextMsg,
                                    Vector<Log> errors) {
        errors.add(Log.error("In " + expDesc +
                             ": type of subexpression " + argDesc + " is "
                             + actual.toString() + ", should be "
                             + expected.toString(),
                             contextMsg));
    }

    public static void logNumericTypeError(String expDesc,
                                           String argDesc,
                                           ExprType actual,
                                           String contextMsg,
                                           Vector<Log> errors) {
        errors.add(Log.error("In " + expDesc +
                             ": type of subexpression " + argDesc + " is "
                             + actual.toString() + ", should be Int or Real",
                             contextMsg));
    }

    public static void logArrayTypeError(String expDesc,
                                         String argDesc,
                                         ExprType actual,
                                         String contextMsg,
                                         Vector<Log> errors) {
        errors.add(Log.error("In " + expDesc +
                             ": type of subexpression " + argDesc + " is "
                             + actual.toString() + ", should be an array",
                             contextMsg));
    }

}

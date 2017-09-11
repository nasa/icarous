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

public class ArrayLiteral
    extends LiteralExpr {

    protected ExprList elements;

    public ArrayLiteral(ExprType t) {
        super(t);
        elements = new ExprList();
    }

    @Override
    public String valueToString() {
        StringBuilder s = new StringBuilder();
        int n = elements.size();
        s.append(Integer.toString(n));
        s.append(" element");
        if (n != 1)
            s.append("s");
        return s.toString();
    }

    // Specific to ArrayLiteral

    public int getSize() {
        return elements.size();
    }

    public void addElement(Expr e) {
        elements.add(e);
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
        ExprType elemType = type.arrayElementType();
        for (Expr e : elements) {
            if (!(e instanceof LiteralExpr))
                errors.add(Log.error("Array element " + e.toString() + " is not a literal value",
                                     contextMsg));
            else if (e.getType() != elemType) {
                errors.add(Log.error("Array element " + e.toString()
                                     + " is not a " + elemType.toString() + " value",
                                     contextMsg));
            }
        }
        return type;
    }

}

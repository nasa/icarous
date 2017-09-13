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

import model.expr.Expr;
import model.expr.ExprType;

public class Var
{
	public enum VarMod {None, In, InOut};

	private String id;
	protected ExprType type; // needed by VarArray
	private VarMod mod;
    private Expr init;
	
	public String getID() { return id; }
	public ExprType getType() { return type; }
	public VarMod getMod() { return mod; }
    public void setMod(VarMod m) { mod = m; }
    public Expr getInitializer() { return init; }
    public void setInitializer(Expr i) { init = i; }

    // Only used in arglist parsing
	public Var(ExprType t) {
        id = "";
        type = t;
        mod = VarMod.None;
        init = null;
    }

	public Var(String i, ExprType t) {
        id = i;
        type = t;
        mod = VarMod.None;
        init = null;
    }

	public Var(String i, ExprType t, VarMod m) {
        id = i;
        type = t;
        mod = m;
        init = null;
    }
	
	@Override
	public String toString() {
		String result = id;
		if (!id.equals(""))
			result += " :: ";
		
		if (mod != VarMod.None)
			result += mod.toString() + " ";
		
		return result + type.toString();
	}

	public String toTypeString() {
		return type.toString();
	}
}

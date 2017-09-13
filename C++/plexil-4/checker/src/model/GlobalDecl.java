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
import model.expr.ExprList;

public class GlobalDecl {
	public enum CallType {Command, Lookup, LibraryCall};
	
	private CallType type;
	private String id;
	private VarList arguments;
	private VarList returns;
	
	public CallType getType() { return type; }
	public String   getID()   { return id; }
	public VarList  getArgs() { return arguments; }
	public VarList  getRets() { return returns; }

    public Var getReturn() {
        if (returns.isEmpty())
            return null;
        return returns.get(0);
    }
	
	public GlobalDecl(CallType d)
	{
		type = d;
		id = "";
		arguments = new VarList();
		returns = new VarList();
	}

	public GlobalDecl(CallType d, String i)
	{
		type = d;
		id = i;
		arguments = new VarList();
		returns = new VarList();
	}

	public GlobalDecl(CallType d, String i, VarList args)
	{
		type = d;
		id = i;
		arguments = args;
		returns = new VarList();
	}

	public GlobalDecl(CallType d, String i, VarList args, VarList ret)
	{
		type = d;
		id = i;
		arguments = args;
		returns = ret;
	}

	public void print()
	{
		System.out.println(id + " :: " + type);
		if (arguments != null)
		{
			System.out.println("  Arguments = ");
			for (int j = 0; j < arguments.size(); j++)
				System.out.println("    " + arguments.elementAt(j).toString());
		}
		if (returns != null)
		{
			System.out.println("  Returns = ");
			for (int j = 0; j < returns.size(); j++)
				System.out.println("    " + returns.elementAt(j).toString());
		}
	}
	
	@Override
	public String toString() {
		return type.toString() + " " + id.toString() + "(" + arguments.toTypeString() + ")";
	}

}

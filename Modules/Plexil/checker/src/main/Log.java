/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
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

import model.Action;
import model.GlobalDecl;
import model.Node;

/**
 *
 * @author straussp
 */
public class Log {

	private Log(String m, Severity sev) { setMessage(m); setSeverity(sev); }
	
	private String message;
	private Severity severity;
	
	public String getMessage() { return message; }
	public void setMessage(String m) { message = m; }
	public Severity getSeverity() { return severity; }
	public void setSeverity(Severity s) { severity = s; }
	
	public enum Severity {
		Error, Warning;
		public boolean greaterThan(Severity s)
		{
			return this.equals(Severity.Error) && s.equals(Severity.Warning);
		}
	}
	
	public static Log error(String message)
	{
		return new Log(message, Severity.Error);
	}
	
	public static Log error(String message, String extra)
	{
        if (extra == null || extra.isEmpty())
            return error(message);
        return error(message + " " + extra);
	}
	
	public static Log warning(String message)
	{
		return new Log(message, Severity.Warning);
	}
	
	public static Log warning(String message, String extra)
	{
        if (extra == null || extra.isEmpty())
            return warning(message);
        return warning(message + " in " + extra);
	}

    // Utilities for checking
    public static Log declarationMismatchMessage(Action a, Node n, GlobalDecl d) {
        return error(a.toString() + " in " + n.getID()
                     + " does not match "+ d.toString());
    }

	
	@Override
	public String toString()
	{
		return severity.toString() + ": " + message;
	}
}

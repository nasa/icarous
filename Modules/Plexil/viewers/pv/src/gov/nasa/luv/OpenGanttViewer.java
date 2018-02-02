/* Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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
*
* By Madan, Isaac A.
*/

package gov.nasa.luv;

import javax.swing.*;
import java.lang.reflect.Method;
import java.io.*;

public class OpenGanttViewer {
        public static void openURL() {
	    try {
		//File workDir = new File("~/");
		//get the latest file
		Process p1 = Runtime.getRuntime().exec("ls -t");
		BufferedReader file = new BufferedReader (new InputStreamReader (p1.getInputStream()));
		String filename = file.readLine();
		//open with Safari
		Process p = Runtime.getRuntime().exec("open " + filename + " -a  Safari");
		BufferedReader err = new BufferedReader (new InputStreamReader (p.getErrorStream()));
		String error = err.readLine();
		if(error != null) {
		    //if Safari failed, open with Firefox
		    Process p2 = Runtime.getRuntime().exec("open " + filename + " -a  Firefox");
		    BufferedReader err2 = new BufferedReader (new InputStreamReader (p2.getErrorStream()));
		    String error2 = err2.readLine();
		    //both Safari and Firefox failed, produce error
		    if(error2 != null)
			alert("no supported browsers");
		}
	    } catch (Exception e) {
		alert(e.getLocalizedMessage());
	    }
	}

    public static void alert(String text) {
	JOptionPane.showMessageDialog(null, "Notice in opening browser" + ":\n" + text);
    }
}
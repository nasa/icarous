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

import reader.XmlReader;
import net.n3.nanoxml.*;
import java.util.Vector;

public class DeclMain {
	public static void main(String [] args)
	{
		try
		{
			String fileName = "";
			boolean printDebug = false;
			
			// Read in arguments
			for (String s : args)
			{
				if (s.equals("-v") || s.equals("-d"))
					printDebug = true;
				else
					fileName = s;
			}
			
			if (fileName.equals(""))
			{
				fileName = "plans/test.plx";
				System.out.println("No specified input file. Using " + fileName);
			}
			
			// Convert XML file to nanoxml data structure
			IXMLParser xp = net.n3.nanoxml.XMLParserFactory.createDefaultXMLParser();
			IXMLReader xr = net.n3.nanoxml.StdXMLReader.fileReader(fileName);
			xp.setReader(xr);
			IXMLElement xml = (IXMLElement) xp.parse();
			
			// Read XML into model
			XmlReader parse = new XmlReader();
			parse.readPlan(xml);
			
			if (printDebug) {
				parse.printDeclCalls();
				System.out.println();
				parse.printNodeCalls();
			}
			
			DeclChecker d = new DeclChecker();
			Vector<Log> errors = d.checkPlan(parse.getPlan());
                int nerrors = 0;
                int nwarnings = 0;
			
			if (!errors.isEmpty()) {
				for (Log l : errors) {
					System.out.println(l.toString());
					if (l.getSeverity().equals(Log.Severity.Error))
                        ++nerrors;
                    else
                        ++nwarnings;
				}
			}
            if (nerrors > 0)
                System.out.print(Integer.toString(nerrors) + " error"
                                 + (nerrors == 1 ? "" : "s")
                                 + (nwarnings > 0 ? ", " : ""));
            if (nwarnings > 0)
                System.out.print(Integer.toString(nwarnings) + " warning"
                                 + (nwarnings == 1 ? "" : "s"));
            if (nerrors > 0 || nwarnings > 0)
                System.out.println();

            if (nerrors > 0)
                System.exit(1);
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.exit(2);
		}
	}

}


//
// CommandAST
//

package plexilscript;

import java.util.List;

public class CommandAST implements ElementAST {
    String             tag;
    String             resultTag;
    String             name;
    List<ParameterAST> parameters;
    String             type;
    List<String>       results;

    public CommandAST
	(String             tag0,
	 String             resultTag0,
	 String             name0,
	 List<ParameterAST> parameters0,
	 String             type0,
	 List<String>       results0)
    {
	tag        = tag0;
	resultTag  = resultTag0;
	name       = name0;
	parameters = parameters0;
	type       = type0;
	results    = results0;
    }

    public void print () {
	System.out.printf ("    <%s name=\"%s\" type=\"%s\">\n", tag, name, type);
	for (ParameterAST p : parameters)
	    p.print ();
	for (String r : results)
	    System.out.printf ("      <%s>%s</%s>\n", resultTag, r, resultTag);
	System.out.printf ("    </%s>\n", tag);
    }
}


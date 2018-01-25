
//
// PlexilScriptAST
//

package plexilscript;

import java.util.List;

public class PlexilScriptAST {
    List<ElementAST> es;

    public PlexilScriptAST (List<ElementAST> es0) {
	es = es0;
    }

    public void print () {
	System.out.printf ("<PLEXILScript>\n");
	for (ElementAST e : es)
	    e.print ();
	System.out.printf ("</PLEXILScript>\n");
    }
}


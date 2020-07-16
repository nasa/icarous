
//
// SimultaneousAST
//

package plexilscript;

import java.util.List;

public class SimultaneousAST implements ElementAST {
    List<ElementAST> es;

    public SimultaneousAST (List<ElementAST> es0) {
	es = es0;
    }

    public void print () {
	System.out.printf ("    <Simultaneous>\n");
	for (ElementAST e : es)
	    e.print ();
	System.out.printf ("    </Simultaneous>\n");
    }
}


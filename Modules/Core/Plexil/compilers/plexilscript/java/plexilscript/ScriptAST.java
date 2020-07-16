
//
// ScriptAST
//

package plexilscript;

import java.util.List;

public class ScriptAST implements ElementAST {
    List<ElementAST> es;

    public ScriptAST (List<ElementAST> es0) {
	es = es0;
    }

    public void print () {
        if (es.isEmpty()) {
            System.out.printf ("  <Script/>\n");
        }
        else {
            System.out.printf ("  <Script>\n");
            for (ElementAST e : es)
                e.print ();
            System.out.printf ("  </Script>\n");
        }
    }
}


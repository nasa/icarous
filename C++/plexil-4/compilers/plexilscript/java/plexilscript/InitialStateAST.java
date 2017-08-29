
//
// InitialStateAST
//

package plexilscript;

import java.util.List;

public class InitialStateAST implements ElementAST {
    List<ElementAST> es;

    public InitialStateAST (List<ElementAST> es0) {
	es = es0;
    }

    public void print () {
        if (es.isEmpty()) {
            System.out.printf ("  <InitialState/>\n");
        }
        else {
            System.out.printf ("  <InitialState>\n");
            for (ElementAST e : es)
                e.print ();
            System.out.printf ("  </InitialState>\n");
        }
    }
}


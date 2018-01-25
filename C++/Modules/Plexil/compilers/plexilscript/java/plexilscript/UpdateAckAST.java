
//
// UpdateAckAST
//

package plexilscript;

public class UpdateAckAST implements ElementAST {
    String name;

    public UpdateAckAST (String name0) {
	name = name0;
    }

    public void print () {
        System.out.printf ("    <UpdateAck name=\"%s\"/>\n", name);
    }
}


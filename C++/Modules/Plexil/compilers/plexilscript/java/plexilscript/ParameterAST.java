
//
// ParameterAST
//

package plexilscript;

public class ParameterAST {
    String value;
    String type;

    public ParameterAST (String value0, String type0) {
	value = value0;
	type  = type0;
    }

    public void print () {
	System.out.printf ("      <Param type=\"%s\">%s</Param>\n", type, value);
    }
}


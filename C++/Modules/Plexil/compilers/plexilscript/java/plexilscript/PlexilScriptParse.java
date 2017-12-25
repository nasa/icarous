
// import org.antlr.runtime.*;

package plexilscript;

import antlr.*;
import java.io.*;

public class PlexilScriptParse {

    public static void main(String[] args) throws Exception {
	DataInputStream    input  = new DataInputStream    (System.in);
	PlexilScriptLexer  lexer  = new PlexilScriptLexer  (input);
        PlexilScriptParser parser = new PlexilScriptParser (lexer);
	PlexilScriptAST    ps     = parser.plexilScript    ();
	ps.print ();
    }
}


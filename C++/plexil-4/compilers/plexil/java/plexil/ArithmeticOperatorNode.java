// Copyright (c) 2006-2016, Universities Space Research Association (USRA).
//  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//    * Neither the name of the Universities Space Research Association nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

package plexil;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

public class ArithmeticOperatorNode extends ExpressionNode
{
    public ArithmeticOperatorNode(ArithmeticOperatorNode a)
    {
        super(a);
    }

    public ArithmeticOperatorNode(Token t)
    {
        super(t);
    }

    public ArithmeticOperatorNode(int ttype)
    {
        super(new CommonToken(ttype, getTokenString(ttype)));
    }

    public Tree dupNode()
    {
        return new ArithmeticOperatorNode(this);
    }

    private static String getTokenString(int ttype)
    {
        switch (ttype) {
        case PlexilLexer.ABS_KYWD:
            return "ABS";
        default:
            return "UNKNOWN_ARITH_OP_" + String.valueOf(ttype);
        }
    }

    @Override
    public void check(NodeContext context, CompilerState state)
    {
        checkChildren(context, state);

        switch (this.getType()) {
        case PlexilLexer.PLUS:
            m_dataType = checkPlus (state); break;

        case PlexilLexer.MINUS:
            m_dataType = checkMinus (state); break;

        case PlexilLexer.ASTERISK:
            m_dataType = checkMult (state); break;

        case PlexilLexer.SLASH:
            m_dataType = checkDiv (state); break;

        case PlexilLexer.ABS_KYWD:
            m_dataType = checkAbs (state); break;

        case PlexilLexer.MAX_KYWD:
        case PlexilLexer.MIN_KYWD:
            m_dataType = checkMaxMin(state); break;

        case PlexilLexer.MOD_KYWD:
        case PlexilLexer.PERCENT:
            m_dataType = checkMod (state); break;

        case PlexilLexer.SQRT_KYWD:
            m_dataType = PlexilDataType.REAL_TYPE; break; 

        case PlexilLexer.CEIL_KYWD:
        case PlexilLexer.FLOOR_KYWD:
        case PlexilLexer.REAL_TO_INT_KYWD:
        case PlexilLexer.ROUND_KYWD:
        case PlexilLexer.TRUNC_KYWD:
            m_dataType = checkIntConversions(state); break;

        case PlexilLexer.STRLEN_KYWD:
            m_dataType = checkStringLength(state); break;

        case PlexilLexer.ARRAY_MAX_SIZE_KYWD:
        case PlexilLexer.ARRAY_SIZE_KYWD:
            m_dataType = checkArraySize(state); break;

        default:
            m_dataType = PlexilDataType.ERROR_TYPE;
        }

        if (m_dataType == PlexilDataType.ERROR_TYPE) {
            state.addDiagnostic(this,
                                "Shouldn't happen! Unknown expression type.",
                                Severity.ERROR);
        }
    }

    //
    // Individual checkers
    //

    private PlexilDataType checkAbs (CompilerState state)
    {
        // ABS has exactly one child
        PlexilDataType type = ((ExpressionNode) this.getChild(0)).getDataType();

        // It can be a number or duration
        if (type == PlexilDataType.DURATION_TYPE || type.isNumeric()) return type;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "ABS operator given invalid argument: " +
                                 type.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkDiv (CompilerState state)
    {
        // DIV has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can both be numbers...

        if (ltype.isNumeric() && rtype.isNumeric()) return PlexilDataType.REAL_TYPE;

        // They can be certain combinations of duration and number...

        else if (ltype == PlexilDataType.DURATION_TYPE && rtype.isNumeric()) return ltype;
        else if (ltype == PlexilDataType.DURATION_TYPE && rtype == ltype) return ltype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "Operator '/' given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkIntConversions(CompilerState state)
    {
        // Conversions have exactly one child
        PlexilDataType type = ((ExpressionNode) this.getChild(0)).getDataType();

        // It can be a number, duration, or date
        if (type == PlexilDataType.DURATION_TYPE || type == PlexilDataType.DATE_TYPE)
            return type;
        else if (type.isNumeric())
            return PlexilDataType.INTEGER_TYPE;

        // Otherwise, no good...
        else {
            state.addDiagnostic (this,
                                 "ABS operator given invalid argument: " +
                                 type.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkMaxMin(CompilerState state)
    {
        // MIN and MAX have exactly two children
        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // Two integers yield an integer
        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype)
            return ltype;
        // Two numbers return a real
        else if (ltype.isNumeric() && ltype.isNumeric())
            return PlexilDataType.REAL_TYPE;
        // Two durations return a duration
        else if (ltype == PlexilDataType.DURATION_TYPE && rtype == ltype)
            return ltype;
        // Two dates return a date
        else if (ltype == PlexilDataType.DATE_TYPE && rtype == ltype)
            return ltype;
        // Otherwise, no good...
        else {
            state.addDiagnostic (this,
                                 "Operator '" + this.getToken().getText() + "' given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkMinus (CompilerState state)
    {
        // MINUS has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && rtype.isNumeric()) return rtype;

        // They can be certain combinations of duration and date...

        else if (ltype == PlexilDataType.DATE_TYPE &&
                 rtype == PlexilDataType.DURATION_TYPE) return ltype;
        else if (ltype == PlexilDataType.DATE_TYPE &&
                 rtype == ltype) return PlexilDataType.DURATION_TYPE;
        else if (ltype == PlexilDataType.DURATION_TYPE &&
                 rtype == ltype) return rtype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "MINUS given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }


    private PlexilDataType checkMod (CompilerState state)
    {
        // MOD has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can both be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && ltype.isNumeric()) return PlexilDataType.REAL_TYPE;

        // They can be certain combinations of duration and number...

        else if (ltype == PlexilDataType.DURATION_TYPE && rtype.isNumeric()) return ltype;
        else if (ltype == PlexilDataType.DURATION_TYPE && rtype == ltype) return ltype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "Modulo operator given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkMult (CompilerState state)
    {
        // MULT has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && rtype.isNumeric()) return rtype;

        // They can be certain combinations of duration and number...

        else if (ltype == PlexilDataType.DURATION_TYPE && rtype.isNumeric()) return ltype;
        else if (ltype.isNumeric() && rtype == PlexilDataType.DURATION_TYPE) return rtype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "Operator '*' given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkPlus (CompilerState state)
    {
        // PLUS has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && rtype.isNumeric()) return rtype;

        // They can be certain combinations of duration and date...

        else if (ltype == PlexilDataType.DATE_TYPE &&
                 rtype == PlexilDataType.DURATION_TYPE) return ltype;
        else if (ltype == PlexilDataType.DURATION_TYPE &&
                 rtype == PlexilDataType.DATE_TYPE) return rtype;
        else if (ltype == PlexilDataType.DURATION_TYPE &&
                 rtype == PlexilDataType.DURATION_TYPE) return rtype;

        // They can be both be strings...

        else if (ltype == PlexilDataType.STRING_TYPE &&
                 rtype == PlexilDataType.STRING_TYPE)  return ltype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "PLUS given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType checkStringLength(CompilerState state)
    {
        // StringLength has exactly one child, which must be a string-valued expression
        PlexilDataType argType = ((ExpressionNode) this.getChild(0)).getDataType();
        if (argType != PlexilDataType.STRING_TYPE) {
            state.addDiagnostic(this.getChild(0),
                                "Non-string argument to " + this.getText() + " function",
                                Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
        else
            return PlexilDataType.INTEGER_TYPE;
    }

    private PlexilDataType checkArraySize(CompilerState state)
    {
        // ArraySize and ArrayMaxSize have exactly one child, which must be an array-valued expression
        PlexilDataType argType = ((ExpressionNode) this.getChild(0)).getDataType();
        if (!argType.isArray()) {
            state.addDiagnostic(this.getChild(0),
                                "Non-array argument to " + this.getText() + " function",
                                Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
        else
            return PlexilDataType.INTEGER_TYPE;
    }

    //
    // XML construction
    //

    public void constructXML()
    {
        super.constructXML();
        for (int i = 0; i < this.getChildCount(); i++) {
            m_xml.addChild(this.getChild(i).getXML());
        }
    }

    public String getXMLElementName()
    {
        switch (this.getType()) {
        case PlexilLexer.ABS_KYWD:
            return "ABS";

        case PlexilLexer.ARRAY_MAX_SIZE_KYWD:
            return "ArrayMaxSize";

        case PlexilLexer.ARRAY_SIZE_KYWD:
            return "ArraySize";

        case PlexilLexer.ASTERISK:
            return "MUL";

        case PlexilLexer.CEIL_KYWD:
            return "CEIL";

        case PlexilLexer.FLOOR_KYWD:
            return "FLOOR";

        case PlexilLexer.MINUS:
            return "SUB";

        case PlexilLexer.MAX_KYWD:
            return "MAX";

        case PlexilLexer.MIN_KYWD:
            return "MIN";

        case PlexilLexer.MOD_KYWD:
        case PlexilLexer.PERCENT:
            return "MOD";

        case PlexilLexer.PLUS:
            if (m_dataType == PlexilDataType.STRING_TYPE)
                return "Concat";
            else return "ADD";

        case PlexilLexer.REAL_TO_INT_KYWD:
            return "REAL_TO_INT";

        case PlexilLexer.ROUND_KYWD:
            return "ROUND";

        case PlexilLexer.SLASH:
            return "DIV";

        case PlexilLexer.SQRT_KYWD:
            return "SQRT";

        case PlexilLexer.STRLEN_KYWD:
            return "STRLEN";

        case PlexilLexer.TRUNC_KYWD:
            return "TRUNC";

        default:
            return this.getToken().getText();
        }
    }

}

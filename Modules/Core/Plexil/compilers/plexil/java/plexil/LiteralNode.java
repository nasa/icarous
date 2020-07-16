/* Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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

package plexil;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

// 
// A specialized AST node that does code generation for literals.
// The data type should be specified by the parser from the content.
// 

public class LiteralNode extends ExpressionNode
{
    public LiteralNode(Token t) 
    {
        super(t);
        setInitialDataTypeFromTokenType();
    }

	public LiteralNode(LiteralNode n)
	{
		super(n);
	}

	public Tree dupNode()
	{
		return new LiteralNode(this);
	}

    private void setInitialDataTypeFromTokenType()
    {
        switch (this.getToken().getType()) {

        case PlexilLexer.INT:
        case PlexilLexer.NEG_INT:
            m_dataType = PlexilDataType.INTEGER_TYPE;
            break;

        case PlexilLexer.DOUBLE:
        case PlexilLexer.NEG_DOUBLE:
            m_dataType = PlexilDataType.REAL_TYPE;
            break;

        case PlexilLexer.TRUE_KYWD:
        case PlexilLexer.FALSE_KYWD:
            m_dataType = PlexilDataType.BOOLEAN_TYPE;
            break;

        case PlexilLexer.STATE_NAME:
            m_dataType = PlexilDataType.STATE_NAME_TYPE;
            break;

            // Handled by ArrayLiteralNode
        case PlexilLexer.ARRAY_LITERAL:
            m_dataType = PlexilDataType.UNKNOWN_ARRAY_TYPE;
            break;

            // Handled by StringLiteralNode
        case PlexilLexer.STRING:
            m_dataType = PlexilDataType.STRING_TYPE;
            break;

        case PlexilLexer.DATE_LITERAL:
            m_dataType = PlexilDataType.DATE_TYPE;
            break;

        case PlexilLexer.DURATION_LITERAL:
            m_dataType = PlexilDataType.DURATION_TYPE;
            break;

            // internal data types

        case PlexilLexer.EXECUTING_STATE_KYWD:
        case PlexilLexer.FAILING_STATE_KYWD:
        case PlexilLexer.FINISHED_STATE_KYWD:
        case PlexilLexer.FINISHING_STATE_KYWD:
        case PlexilLexer.INACTIVE_STATE_KYWD:
        case PlexilLexer.ITERATION_ENDED_STATE_KYWD:
        case PlexilLexer.WAITING_STATE_KYWD:
            m_dataType = PlexilDataType.NODE_STATE_TYPE;
            break;


        case PlexilLexer.SUCCESS_OUTCOME_KYWD:
        case PlexilLexer.FAILURE_OUTCOME_KYWD:
        case PlexilLexer.SKIPPED_OUTCOME_KYWD:
        case PlexilLexer.INTERRUPTED_OUTCOME_KYWD:
            m_dataType = PlexilDataType.NODE_OUTCOME_TYPE;
            break;


        case PlexilLexer.PRE_CONDITION_FAILED_KYWD:
        case PlexilLexer.POST_CONDITION_FAILED_KYWD:
        case PlexilLexer.INVARIANT_CONDITION_FAILED_KYWD:
        case PlexilLexer.PARENT_FAILED_KYWD:
        case PlexilLexer.PARENT_EXITED_KYWD:
        case PlexilLexer.EXITED_KYWD:
            m_dataType = PlexilDataType.NODE_FAILURE_TYPE;
            break;

        case PlexilLexer.COMMAND_ACCEPTED_KYWD:
        case PlexilLexer.COMMAND_DENIED_KYWD:
        case PlexilLexer.COMMAND_FAILED_KYWD:
        case PlexilLexer.COMMAND_RCVD_KYWD:
        case PlexilLexer.COMMAND_SENT_KYWD:
        case PlexilLexer.COMMAND_SUCCESS_KYWD:
            m_dataType = PlexilDataType.COMMAND_HANDLE_TYPE;
            break;

        default:
            // debug aid
            CompilerState.getCompilerState().addDiagnostic(this,
                                                           "Internal error: LiteralNode cannot determine data type for \""
                                                           + this.getText() + "\"",
                                                           Severity.FATAL);
            m_dataType = PlexilDataType.ERROR_TYPE;
            break;
        }
    }

    public void earlyCheck(NodeContext context, CompilerState state)
    {
    }


    /**
     * @brief Persuade the expression to assume the specified data type
     * @return true if the expression can consistently assume the specified type, false otherwise.
     */
    protected boolean assumeType(PlexilDataType t, CompilerState state)
    {
        // If target type is Void, Error, or underspec'd array, fail.
        if (t == PlexilDataType.VOID_TYPE
            || t == PlexilDataType.ERROR_TYPE
            || t == PlexilDataType.UNKNOWN_ARRAY_TYPE) {
            state.addDiagnostic(null,
                                "Internal error: LiteralNode.assumeType called with illegal first argument of "
                                + t.typeName(),
                                Severity.FATAL);
            return false;
        }

        // If target type is Any, succeed.
        if (t == PlexilDataType.ANY_TYPE)
            return true;

        // If we are already the right type, succeed.
        if (m_dataType == t)
            return true;
        else if (m_dataType == PlexilDataType.INTEGER_TYPE
                 && t == PlexilDataType.REAL_TYPE) {
            // Promote to real
            m_dataType = t;
            return true;
        }
        else if (m_dataType == PlexilDataType.STRING_TYPE
                 && t.isTemporal()) {
            // do nothing: for now, all strings pass for dates and durations
            return true;
        }
        else if (t == PlexilDataType.BOOLEAN_TYPE
                 && m_dataType == PlexilDataType.INTEGER_TYPE) {
            int value = parseIntegerValue(this.getToken().getText());
            if (value == 0 || value == 1) {
                // OK to coerce it to boolean
                m_dataType = t;
                return true;
            }
            // else fall through to failure
        }

        // fall-through return
        return false;
    }

    // Assumes this has been called after assumeType().
    public void checkTypeConsistency(NodeContext context, CompilerState state)
    {
        if (m_dataType == PlexilDataType.INTEGER_TYPE) {
            // TODO: format check
            // TODO: range check
        }
        else if (m_dataType == PlexilDataType.REAL_TYPE) {
            // TODO: range check
        }
    }

    // Utility for use from various contexts
    static public int parseIntegerValue(String txt)
    {
        int radix = 10;
        // Look for prefixes
        if (txt.length() > 2
            && txt.charAt(0) == '0'
            && !isDigit(txt.charAt(1))) {
            switch (txt.charAt(1)) {
            case 'b':
            case 'B':
                // Binary
                radix = 2;
                txt = txt.substring(2);
                break;

            case 'o':
            case 'O':
                // Octal
                radix = 8;
                txt = txt.substring(2);
                break;

            case 'x':
            case 'X':
                // Hexadecimal
                radix = 16;
                txt = txt.substring(2);
                break;

            default:
                // *** should never get here ***
                System.err.println("In integer value \"" + txt
                                   + "\": Unknown numeric prefix \"" + txt.substring(0, 2)
                                   + "\", parsing as decimal");
            }
        }
        return Integer.parseInt(txt, radix);
    }

    public void constructXML()
    {
        super.constructXML();

        if (this.getType() == PlexilLexer.NEG_INT) {
            String txt = this.getChild(0).getText();            
            m_xml.setContent("-" + Integer.toString(parseIntegerValue(txt)));
        }

        else if (this.getType() == PlexilLexer.NEG_DOUBLE) {
            String txt = this.getChild(0).getText();            
            m_xml.setContent("-" + txt);
        }
        else if (this.getType() == PlexilLexer.DATE_LITERAL ||
                 this.getType() == PlexilLexer.DURATION_LITERAL) {
            String txt = this.getChild(0).getText();            
            m_xml.setContent(stripQuotes(txt));
        }
        else {
            String txt = this.getToken().getText();
            if (this.getType() == PlexilLexer.INT) {
                m_xml.setContent(Integer.toString(parseIntegerValue(txt)));
            } else m_xml.setContent(txt);
        }
    }

    public String getXMLElementName()
    {
        return m_dataType.typeName() + "Value"; 
    }

    // Literal nodes do not support source locators.
    protected void addSourceLocatorAttributes() {}

    // *** is this still necessary?? ***
    public IXMLElement getXML(String elementType)
    {
        IXMLElement result = new XMLElement(elementType);
        result.setContent(getText());
        return result;
    }

    // Helper methods to support parsing literals

    static protected boolean isQuadDigit(char c)
    {
        return (c >= '0' && c <= '3');
    }

    static protected boolean isOctalDigit(char c)
    {
        return (c >= '0' && c <= '7');
    }

    static protected boolean isDigit(char c)
    {
        return (c >= '0' && c <= '9');
    }

    static protected boolean isHexDigit(char c)
    {
        return (c >= '0' && c <= '9')
            || (c >= 'a' && c <= 'f')
            || (c >= 'A' && c <= 'F');
    }

    static protected int digitToInt(char c)
    {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        System.err.println("Error: '" + c + "' is not a digit");
        return -1;
    }

    static protected int hexDigitToInt(char c)
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        else if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        System.err.println("Error: '" + c + "' is not a hex digit");
        return -1;
    }

    static private String stripQuotes (String s)
    {
        // Hack alert!  I think we can safely assume the string begins
        // and ends with a double quote, since it was parsed as a string
        // literal.  Nevertheless, both this function, and its use,
        // seem crufty at best.

        return s.substring (1, s.length() - 1);
    }
}

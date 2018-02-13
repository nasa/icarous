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
// A specialized AST node that does code generation for string literals.
// 

public class StringLiteralNode extends LiteralNode
{
    // Needed for serializable
    private static final long serialVersionUID = -8806474295262342033L;

    public StringLiteralNode(Token t)
    {
        super(t);
    }

	public StringLiteralNode(StringLiteralNode n)
	{
		super(n);
	}

	public Tree dupNode()
	{
		return new StringLiteralNode(this);
	}

    public void constructXML()
    {
        super.constructXML();
        String myText = getText();
        // N.B. should be StringBuilder for Java 1.5 up
        StringBuffer myContent = new StringBuffer(myText.length());
        int index = 1;
        // check leading/trailing double quotes
        if (myText.charAt(0) != '"' || myText.charAt(myText.length() - 1) != '"') {
            // not supposed to happen, but...
            System.err.println("Invalid string format for \"" + myText + "\"");
            index = 0;
        }
        while (index < myText.length()) {
            int nextIdx = myText.indexOf('\\', index);
            if (nextIdx != -1) {
                // copy string up to \ escape char
                if (index != nextIdx)
                    myContent.append(myText.substring(index, nextIdx));
                // handle escape char
                nextIdx++;
                char escaped = myText.charAt(nextIdx++);
                int charcode = 0;
                switch (escaped) {
                    // \\u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]
                    // - Unicode escape
                case 'u':
                    // check for hex digits and compose character
                    for (int i = 0; i < 4; i++) {
                        char hex = myText.charAt(nextIdx++);
                        if (isHexDigit(hex)) {
                            charcode = (charcode * 16) + hexDigitToInt(hex);
                        }
                        else {
                            System.err.println("Invalid Unicode escape format for \""
                                               + myText + "\"");
                        }
                    }
                    myContent.append((char)charcode);
                    break;

                    // \[0-3]([0-7][0-7]?)?
                    // - Numeric escape
                case '0':
                case '1':
                case '2':
                case '3':
                    // 1-3 chars
                    charcode = digitToInt(escaped);
                    escaped = myText.charAt(nextIdx++);
                    if (isOctalDigit(escaped)) {
                        charcode = (charcode * 8) + digitToInt(escaped);
                        escaped = myText.charAt(nextIdx++);
                        if (isOctalDigit(escaped)) {
                            charcode = (charcode * 8) + digitToInt(escaped);
                        }
                        else {
                            nextIdx--;
                        }
                    }
                    else {
                        nextIdx--;
                    }
                    myContent.append((char) charcode);
                    break;
                        
                    // \[4-7][0-7]?
                    // - Numeric escape
                case '4':
                case '5':
                case '6':
                case '7':
                    // 1-2 chars
                    charcode = digitToInt(escaped);
                    escaped = myText.charAt(nextIdx++);
                    if (isOctalDigit(escaped)) {
                        charcode = (charcode * 8) + digitToInt(escaped);
                    }
                    else {
                        nextIdx--;
                    }
                    myContent.append((char) charcode);
                    break;

                    // \n - newline
                case 'n':
                    myContent.append('\n');
                    break;

                    // \t - tab
                case 't':
                    myContent.append('\t');
                    break;

                    // \b - backspace
                case 'b':
                    myContent.append('\b');
                    break;

                    // \f - form feed
                case 'f':
                    myContent.append('\f');
                    break;

                    // verbatim (includes backslash, single quote, double quote)
                default:
                    myContent.append(escaped);
                    break;
                }
            }
            else {
                // find end quote
                // *** throw exception if not found (NYI)
                nextIdx = myText.indexOf('"', index);
                if (nextIdx == -1)
                    nextIdx = myText.length();
                // copy to end
                myContent.append(myText.substring(index, nextIdx));
                nextIdx++;
            }
            index = nextIdx;
        }
        m_xml.setContent(myContent.toString());
    }

}

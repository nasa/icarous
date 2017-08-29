/* Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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

public class ArrayLiteralNode extends LiteralNode
{
    public ArrayLiteralNode(Token t) 
    {
        super(t);
    }

    public ArrayLiteralNode(ArrayLiteralNode n)
    {
        super(n);
    }

    public Tree dupNode()
    {
        return new ArrayLiteralNode(this);
    }

    public void earlyCheck(NodeContext context, CompilerState state)
    {
        // See if we can determine our own type from type of children
        PlexilDataType workingType = null;
        boolean workingTypeChanged = false;
        for (int i = 0; i < this.getChildCount(); i++) {
            LiteralNode elt = (LiteralNode) this.getChild(i);
            PlexilDataType eltType = elt.getDataType();

            if (workingType == null) // first child
                workingType = eltType;
            else if (eltType == PlexilDataType.ERROR_TYPE) {
                workingType = PlexilDataType.UNKNOWN_ARRAY_TYPE;
                break;
            }
            else if (workingType == eltType) {
                // do nothing
            }
            else if (eltType == PlexilDataType.INTEGER_TYPE
                     && (workingType == PlexilDataType.REAL_TYPE
                         || workingType == PlexilDataType.BOOLEAN_TYPE)
                     && elt.assumeType(workingType, state)) {
                // do nothing
            }
            else if (eltType == PlexilDataType.REAL_TYPE
                     && workingType == PlexilDataType.INTEGER_TYPE) {
                workingType = PlexilDataType.REAL_TYPE;
                workingTypeChanged = true;
            }
            else if (eltType == PlexilDataType.BOOLEAN_TYPE
                     && workingType == PlexilDataType.INTEGER_TYPE) {
                workingType = PlexilDataType.BOOLEAN_TYPE;
                workingTypeChanged = true;
            }
            else {
                state.addDiagnostic(elt,
                                    "earlyCheck: Element of " + workingType.typeName()
                                    + " array literal has inconsistent type "
                                    + eltType.typeName(),
                                    Severity.ERROR);
                workingType = PlexilDataType.ERROR_TYPE;
                break;
            }
        }

        if (workingType == PlexilDataType.ERROR_TYPE)
            m_dataType = PlexilDataType.UNKNOWN_ARRAY_TYPE; // FAIL
        else if (workingTypeChanged)
            // see if whole array can be persuaded to new type
            assumeType(workingType.arrayType(), state); // for effect
        else
            m_dataType = workingType.arrayType();
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
                                "Internal error: ArrayLiteralNode.assumeType called with illegal first argument of "
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

        // Can each element of this array be coerced to the desired type?
        boolean success = true;
        PlexilDataType eltType = t.arrayElementType();
        for (int childIdx = 0; childIdx < this.getChildCount(); childIdx++) {
            LiteralNode child = (LiteralNode) this.getChild(childIdx);
            if (!child.assumeType(eltType, state)) {
                state.addDiagnostic(child,
                                    "assumeType: Element of " + eltType.typeName()
                                    + " array literal has inconsistent type "
                                    + child.getDataType().typeName(),
                                    Severity.ERROR);
                success = false;
                break;
            }
        }

        if (success)
            m_dataType = t;
        return success;
    }


    // Assumes this has been called after assumeType().
    public void checkTypeConsistency(NodeContext context, CompilerState state)
    {
        // Check that children are type consistent
        PlexilDataType eltType = m_dataType.arrayElementType();
        for (int childIdx = 0; childIdx < this.getChildCount(); childIdx++) {
            LiteralNode child = (LiteralNode) this.getChild(childIdx);
            if (!child.assumeType(eltType, state)) {
                state.addDiagnostic(child,
                                    "checkTypeConsistency: Element of " + eltType.typeName()
                                    + " array literal has inconsistent type "
                                    + child.getDataType().typeName(),
                                    Severity.ERROR);
            }
        }
    }

    // Must override LiteralNode method
    public void constructXML()
    {
        // PlexilTreeNode base method
        constructXMLBase();

        m_xml.setAttribute("Type", m_dataType.arrayElementType().typeName());
        for (int childIdx = 0; childIdx < this.getChildCount(); childIdx++) {
            LiteralNode child = (LiteralNode) this.getChild(childIdx);
            m_xml.addChild(child.getXML());
        }
    }
        
    public String getXMLElementName() { return "ArrayValue"; }

}

/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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

package model.expr;


public enum ExprType {
    Int,
    Real,
    Bool,
    Str,
    GenericArray,
    IntArray,
    RealArray,
    BoolArray,
    StrArray,
    NodeState,
    NodeOutcome,
    NodeFailureType,
    NodeTimepointValue,
    NodeCommandHandle,
    A;  // A is generic, should match any other

        // Used in parser
        // Only defined for user scalar types at present
    public static ExprType typeForName(String s) {
        if (s == null)
            return null;
        switch (s) {
        case "Integer":
            return Int;

        case "Real":
            return Real;

        case "Boolean":
            return Bool;
                
        case "String":
            return Str;

        default:
            return null; // invalid or not yet implemented
        }
    }

    public ExprType arrayType() {
        switch (this) {
        case Int:
            return IntArray;

        case Real:
            return RealArray;

        case Bool:
            return BoolArray;

        case Str:
            return StrArray;

        case A:
            return GenericArray;
            
        default:
            return null;
        }
    }

    public ExprType arrayElementType() {
        switch (this) {
        case IntArray:
            return Int;

        case RealArray:
            return Real;

        case BoolArray:
            return Bool;

        case StrArray:
            return Str;

        case GenericArray:
            return A;

        default:
            return null;
        }
    }

    public boolean isArrayType() {
        switch (this) {
        case GenericArray:
        case IntArray:
        case RealArray:
        case BoolArray:
        case StrArray:
            return true;

        default:
            return false;
        }
    }

    public boolean isNumeric() {
        return (this == Int
                || this == Real
                || this == NodeTimepointValue);
    }

    public boolean isInternalType() {
        switch (this) {
        case NodeState:
        case NodeOutcome:
        case NodeFailureType:
        case NodeCommandHandle:
            return true;

        default:
            return false;
        }
    }

    public static ExprType intersection(ExprType l, ExprType r) {
        // Return empty if we have a null input
        if (l == null || r == null)
            return null;
		
        // They are the same type
        if (l.equals(r))
            return l;

        // Check for generic case, which matches anything
        if (l.equals(A))
            return r;
        if (r.equals(A))
            return l;

        // Numeric type contagion
        if (l.equals(Real) && r.equals(Int)
            || l.equals (Int) && r.equals(Real))
            return Real;

        // Check for generic array case, which matches any array
        if (l.equals(GenericArray) && r.isArrayType())
            return r;
        if (r.equals(GenericArray) && l.isArrayType())
            return l;
		
        // Different types, so intersection is empty
        return null;
    }
}

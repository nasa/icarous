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

public final class PlexilDataType
{
    private String m_id;
    private String m_typeName;
    private boolean m_isPrimitive;
    private boolean m_isArray;
    private boolean m_isNumeric;
    private PlexilDataType m_elementType;
    private PlexilDataType m_arrayType;
    public final int m_ord;
    private PlexilDataType m_prev;
    private PlexilDataType m_next;

    private static int s_upperBound = 0;
    private static PlexilDataType s_first = null;
    private static PlexilDataType s_last = null;

    // Define primitive types

    private PlexilDataType(String anId, String typeName, boolean numeric)
    {
        m_id = anId;
        m_typeName = typeName;
        m_isPrimitive = true;
        m_isArray = false;
        m_isNumeric = numeric;
        m_elementType = null;
        m_arrayType = null;

        // enumeration housekeeping
        m_ord = s_upperBound++;
        if (s_first == null) {
            s_first = this;
        }
        if (s_last != null) {
            m_prev = s_last;
            s_last.m_next = this;
        }
        s_last = this;
    }

    // Define array types

    private PlexilDataType(String anId, String typeName, PlexilDataType elementType)
    {
        m_id = anId;
        m_typeName = typeName;
        m_isPrimitive = false;
        m_isArray = true;
        m_isNumeric = false;
        m_elementType = elementType;
        elementType.m_arrayType = this;
        m_arrayType = null;

        // enumeration housekeeping
        m_ord = s_upperBound++;
        if (s_first == null) {
            s_first = this;
        }
        if (s_last != null) {
            m_prev = s_last;
            s_last.m_next = this;
        }
        s_last = this;
    }

    public static PlexilDataType findByName(String typeName)
    {
        for (PlexilDataType dt = s_first;
             dt != null;
             dt = dt.next()) {
            if (typeName.equals(dt.typeName()))
                return dt;
        }
        return null;
    }

    public String toString() { return m_id; }
    public String name() { return m_id; }
    public String typeName() { return m_typeName; }
    public boolean isPrimitive() { return m_isPrimitive; }
    public boolean isArray() { return m_isArray; }
    public boolean isNumeric() { return m_isNumeric; }
    public boolean isTemporal() { return (m_id == "DURATION_TYPE" ||
                                          m_id == "DATE_TYPE"); }
    public PlexilDataType arrayElementType() { return m_elementType; }
    public PlexilDataType arrayType() { return m_arrayType; }
    public static int size() { return s_upperBound; }
    public static PlexilDataType first() { return s_first; }
    public static PlexilDataType last() { return s_last; }
    public PlexilDataType next() { return m_next; }
    public PlexilDataType prev() { return m_prev; }

    // Invalid/temporary types
    public static final PlexilDataType ERROR_TYPE =
        new PlexilDataType("ERROR_TYPE", "Error", false);
    public static final PlexilDataType UNKNOWN_TYPE =
        new PlexilDataType("UNKNOWN_TYPE", "Unknown", false);

    // Primitive types
    public static final PlexilDataType BOOLEAN_TYPE =
        new PlexilDataType("BOOLEAN_TYPE", "Boolean", false);
    public static final PlexilDataType INTEGER_TYPE =
        new PlexilDataType("INTEGER_TYPE", "Integer", true);
    public static final PlexilDataType REAL_TYPE =
        new PlexilDataType("REAL_TYPE", "Real", true);
    public static final PlexilDataType STRING_TYPE =
        new PlexilDataType("STRING_TYPE", "String", false);
    public static final PlexilDataType DATE_TYPE =
        new PlexilDataType("DATE_TYPE", "Date", false);
    public static final PlexilDataType DURATION_TYPE =
        new PlexilDataType("DURATION_TYPE", "Duration", false);

    // Internal data types - users can create literals, access, and compare,
    // but not store as variables or otherwise manipulate
    public static final PlexilDataType COMMAND_HANDLE_TYPE =
        new PlexilDataType("COMMAND_HANDLE_TYPE", "NodeCommandHandle", false);
    public static final PlexilDataType NODE_STATE_TYPE =
        new PlexilDataType("NODE_STATE_TYPE", "NodeState", false);
    public static final PlexilDataType NODE_OUTCOME_TYPE =
        new PlexilDataType("NODE_OUTCOME_TYPE", "NodeOutcome", false);
    public static final PlexilDataType NODE_FAILURE_TYPE =
        new PlexilDataType("NODE_FAILURE_TYPE", "NodeFailure", false);
    public static final PlexilDataType STATE_NAME_TYPE =
        new PlexilDataType("STATE_NAME_TYPE", "StateName", false);

    // Has no value at all, e.g. commands that return no value
    public static final PlexilDataType VOID_TYPE = new PlexilDataType("VOID_TYPE", "Void", false);

    // May assume any type
    public static final PlexilDataType ANY_TYPE = new PlexilDataType("ANY_TYPE", "Any", true);

    // Array types
    public static final PlexilDataType BOOLEAN_ARRAY_TYPE =
        new PlexilDataType("BOOLEAN_ARRAY_TYPE", "Array", BOOLEAN_TYPE);
    public static final PlexilDataType INTEGER_ARRAY_TYPE =
        new PlexilDataType("INTEGER_ARRAY_TYPE", "Array", INTEGER_TYPE);
    public static final PlexilDataType REAL_ARRAY_TYPE =
        new PlexilDataType("REAL_ARRAY_TYPE", "Array", REAL_TYPE);
    public static final PlexilDataType STRING_ARRAY_TYPE =
        new PlexilDataType("STRING_ARRAY_TYPE", "Array", STRING_TYPE);
    public static final PlexilDataType DATE_ARRAY_TYPE =
        new PlexilDataType("DATE_ARRAY_TYPE", "Array", DATE_TYPE);
    public static final PlexilDataType DURATION_ARRAY_TYPE =
        new PlexilDataType("DURATION_ARRAY_TYPE", "Array", DURATION_TYPE);
    // this is for use before semantic checking has determined the value type
    public static final PlexilDataType UNKNOWN_ARRAY_TYPE =
        new PlexilDataType("UNKNOWN_ARRAY_TYPE", "Array", ERROR_TYPE);

}

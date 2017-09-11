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

/* Java 1.5 and up */
public enum NameType
{
    UNDEFINED_NAME("_UNDEFINED"),
        NODE_NAME("Node"),
        VARIABLE_NAME("Variable"),
        FUNCTION_NAME("Function"),
        COMMAND_NAME("Command"),
        STATE_NAME("State"),
        PARAMETER_NAME("Parameter"),
        LIBRARY_NODE_NAME("LibraryNode");

    public final String plexilName;
    NameType(String prettyName) {
        plexilName = prettyName;
    }
}

/* version backwardly compatible with Java 1.4
   import java.util.*;

   public final class NameType
   {
   private String id;
   private String prettyName;
   public final int ord;
   private NameType prev;
   private NameType next;

   private static int upperBound = 0;
   private static NameType first = null;
   private static NameType last = null;

   private NameType(String anId, String plexName)
   {
   this.id = anId;
   this.prettyName = plexName;
   this.ord = upperBound++;
   if (first == null)
   {
   first = this;
   }
   if (last != null)
   {
   this.prev = last;
   last.next = this;
   }
   last = this;
   }

   public static Enumeration elements()
   {
   return new Enumeration()
   {
   private NameType curr = first;
   public boolean hasMoreElements()
   {
   return curr != null;
   }
   public Object nextElement()
   {
   NameType c = curr;
   curr = curr.next();
   return c;
   }
   } ;
   }

   public String toString() { return this.id; }
   public String name() { return this.id; }
   public String plexilName() { return this.prettyName; }
   public static int size() { return upperBound; }
   public static NameType first() { return first; }
   public static NameType last() { return last; }
   public NameType next() { return this.next; }
   public NameType prev() { return this.prev; }

   //
   // Members of the enumeration class
   //
   public static final NameType UNDEFINED_NAME = new NameType("UNDEFINED_NAME", "_UNDEFINED_");
   public static final NameType NODE_NAME = new NameType("NODE_NAME", "Node");
   public static final NameType VARIABLE_NAME = new NameType("VARIABLE_NAME", "Variable");
   public static final NameType FUNCTION_NAME = new NameType("FUNCTION_NAME", "Function");
   public static final NameType COMMAND_NAME = new NameType("COMMAND_NAME", "Command");
   public static final NameType EXTERNAL_STRUCT_NAME = new NameType("EXTERNAL_STRUCT_NAME", "_EXTERNAL_STRUCT_");
   public static final NameType STATE_NAME = new NameType("STATE_NAME", "State");
   public static final NameType PARAMETER_NAME = new NameType("PARAMETER_NAME", "Parameter");
   public static final NameType LIBRARY_NODE_NAME = new NameType("LIBRARY_NODE_NAME", "LibraryNode");

   }

   end Java 1.4 compatibility */

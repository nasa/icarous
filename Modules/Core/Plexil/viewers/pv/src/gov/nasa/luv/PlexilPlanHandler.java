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

package gov.nasa.luv;

import java.io.InterruptedIOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

import org.xml.sax.Attributes;

import static gov.nasa.luv.Constants.*;
import static gov.nasa.luv.PlexilSchema.*;

 /** 
  * The PlexilPlanHandler class provide methods for processing and translating a
  * SAX Plexil Plan written in XML. It Collects condition, local variable and
  * action information to be available for display in the Luv applcaition.
  */

public class PlexilPlanHandler
    extends AbstractDispatchableHandler {

    // Clients are expected to implement this class to receive the plan or library
    // when loading is complete.

    public interface PlanReceiver {
        public void newPlan(Plan p);
        public void newLibrary(Plan p);
    }

    //
    // Trivial implementation of PlanReceiver
    //

    public static class SimplePlanCatcher
        implements PlanReceiver {

        public Plan plan;

        SimplePlanCatcher() {
            plan = null;
        }

        public void newPlan(Plan p) {
            plan = p;
        }

        public void newLibrary(Plan p) {
            plan = p;
        }
    }

    //
    // Local constants
    //
          
    /** Represents the XML tag indicating the loaded file is a Plexil Library.
        Used only in LuvListener stream, never in files. */
    public static final String PLEXIL_LIBRARY    = "PlexilLibrary";

    /** Represents the XML tag indicating the loaded file is a Plexil Script. */
    private static final String PLEXIL_SCRIPT     = "PLEXILScript";

    /** Represents the XML tag in the Plexil Plan marking the Plexil Script file name information. */
    private static final String SCRIPT_FILENAME = "ScriptFilename";

    //* Tags whose contents should be NCName data
    private static final String[] SIMPLE_LITERALS =
    {
        BOOL_VAL,
        INT_VAL,
        REAL_VAL,
        NODE_STATE_VAL,
        NODE_OUTCOME_VAL,
        NODE_FAILURE_VAL,
        NODE_CMD_HANDLE_VAL,
    };

    /** A collection of all variable reference tags. */
        private static final String[] VARIABLE_REFS =
    {
        ARRAY_VAR,
        BOOL_VAR,
        INT_VAR,
        REAL_VAR,
        STRING_VAR,
    };

    /** A collection of all the possible XML tags that represent Resource properties. */  
    private static final String[] RESOURCE_TAGS =
    {
        RESOURCE_NAME,
        RESOURCE_PRIORITY, 
        RESOURCE_LOWER_BOUND, 
        RESOURCE_UPPER_BOUND,
        RESOURCE_RELEASE_AT_TERM,
    };

    /** A collection of all the possible XML tags that represent Node State values. */   
    private static final String[] NODE_STATE_TAGS =
    {
        NODE_STATE_VAR,  
        NODE_OUTCOME_VAR, 
        NODE_FAILURE_VAR, 
        NODE_CMD_HANDLE_VAR,
    };

    private static final String[] UNARY_PREFIX_OPERATORS =
    {
        NOT,
    };

    private static final String[] UNARY_FUNCTIONS =
    {
        SQRT,
        ABS,
        CEIL,
        FLOOR,
        ROUND,
        TRUNC,
        REAL_TO_INT,
        STRLEN,
        ARRAY_SIZE,
        ARRAY_MAX_SIZE,
        IS_KNOWN,
        ALL_KNOWN,
        ANY_KNOWN,
    };

    private static final String[] BINARY_OPERATORS =
    {
        EQ_BOOLEAN,
        EQ_NUMERIC,
        EQ_STRING,
        EQ_INTERNAL,
        NE_BOOLEAN,
        NE_NUMERIC,
        NE_STRING,
        NE_INTERNAL,
        LE,
        LT,
        GE, 
        GT,
        MOD,
    };

    private static final String[] NARY_INFIX_OPERATORS =
    {
        ADD,
        SUB,
        MUL,
        DIV,
        AND,
        OR,
        XOR,
    };

    private static final String[] NARY_FUNCTIONS =
    {
        MAX,
        MIN,
    };
    
    /** A collection of all the possible XML tags that represent Plexil Plan Lookups. */  
    private static final String[] LOOKUP_TAGS =
    {
        LOOKUPNOW,
        LOOKUPCHANGE, 
    };

    /** A collection of all the possible XML tags that represent variable declarations. */  
    private static final String[] VARIABLE_DECL_TAGS =
    {
        DECL_VAR,
        DECL_ARRAY,
    };

    /** Map from tag name to pretty representation. */
    private static final Map<String, String> TAG_LITERAL_MAP =
        new HashMap<String, String>() {
            {
                put(EQ_NUMERIC,  "==");
                put(EQ_STRING,   "==");
                put(EQ_BOOLEAN,  "==");
                put(EQ_INTERNAL, "==");

                put(NE_NUMERIC,  "!=");
                put(NE_STRING,   "!=");
                put(NE_BOOLEAN,  "!=");
                put(NE_INTERNAL, "!=");

                put(LT,          "<");
                put(GT,          ">");
                put(LE,          "<=");
                put(GE,          ">=");

                put(ADD,         "+");
                put(SUB,         "-");
                put(MUL,         "*");
                put(DIV,         "/");

                put(CONCAT,      "+");

                put(AND,         "&&");
                put(OR,          "||");
                put(NOT,         "!");

                put(NODE_STATE_VAR,      ".state");
                put(NODE_OUTCOME_VAR,    ".outcome");
                put(NODE_FAILURE_VAR,    ".failure");
                put(NODE_CMD_HANDLE_VAR, ".command_handle");
                put(NODE_TIMEPOINT_VAL,  ".timepoint");

                put(RESOURCE_NAME,             "   Name" + SEPARATOR + "=");
                put(RESOURCE_LOWER_BOUND,      "   LowerBound" + SEPARATOR + "=");
                put(RESOURCE_UPPER_BOUND,      "   UpperBound" + SEPARATOR + "=");
                put(RESOURCE_RELEASE_AT_TERM,  "   ReleaseAtTermination" + SEPARATOR + "=");
            }
        };

    //
    // Instance variables
    //

    // variables and flags for collecting condition, local variable 
    // and action information
    private StringStack infoStack;
    private boolean nameExpr;

    private Stack<Node> nodeHolder;
    private Node currentNode;
    private Node topLevelNode;
    private Plan plan;

    private PlanReceiver receiver;
    
    /**
     * Constructs a PlexilPlanHandler.
     */
    public PlexilPlanHandler(PlanReceiver r)
    {
        super();

        // Parser setup
        setElementMap(PLAN_HANDLER_MAP);
        
        infoStack = new StringStack();
        nodeHolder = new Stack<Node>();
        plan = null;
        receiver = r;
    }

    /** {@inheritDoc} */
    public void startDocument()
    {
        infoStack.clear();
        nodeHolder.clear();
        nameExpr = false; 
        plan = new Plan();
        currentNode = topLevelNode = null;
    }

    //
    // Table driven XML parsing
    //

    // Simple handler for elements containing a single CNAME
    private final class NCNameHandler
        extends LuvElementHandler {
        public void elementEnd(String tagName, String tweenerText) {
            infoStack.push(tweenerText);
        }
    }    

    // Useful in a bunch of situations.
    private final class StackCleanupHandler
        extends LuvElementHandler {
        public void elementStart(String tagName, Attributes attributes) {
            infoStack.push(tagName);
        }
        public void elementEnd(String tagName, String tweenerText) {
            if (infoStack.search(tagName) < 0)
                System.err.println("ERROR: endElement for " + tagName + ": stack marker missing"); 
            else 
                do {}
                while (!tagName.equals(infoStack.pop()));
        }
    }

    private final class LookupHandler
        extends LuvElementHandler {
        public void elementStart(String tagName, Attributes attributes) {
            infoStack.push(tagName);
        }
        public void elementEnd(String tagName, String tweenerText) {
            // Expected stack contents:
            // LookupNow or LookupOnChange
            // Name
            //  TOLERANCE      (optional, LookupOnChange only)
            //  Tolerance expr (optional, LookupOnChange only)
            // Arguments (optional)

            int depth = infoStack.search(tagName);
            switch (depth) {
            case -1:
                System.err.println("endElement " + tagName + ": Info stack borked!");
                return;

            case 0:
                System.err.println("endElement " + tagName + ": Info stack missing state (at least)!");
                infoStack.pop();
                infoStack.push(tagName + "(*** ERROR not enough info ***)");
                return;

            case 1: {
                // state name only 
                String name = infoStack.pop();
                infoStack.pop(); // tag
                infoStack.push(tagName + "(" + name + ")");
                return;
            }

            case 2: {
                // state name plus args
                String args = infoStack.pop();
                String name = infoStack.pop();
                infoStack.pop(); // tag
                infoStack.push(tagName + "(" + name + "(" + args + ") )");
                return;
            }
            case 3: {
                // state plus tolerance
                String tolerance = infoStack.pop();
                infoStack.pop(); // TOLERANCE tag
                String name = infoStack.pop();
                infoStack.pop(); // tag
                infoStack.push(tagName + "(" + name + "), " + tolerance + ")");
                return;
            }
            case 4: {
                // state, tolerance, args
                String args = infoStack.pop();
                String tolerance = infoStack.pop();
                infoStack.pop(); // TOLERANCE tag
                String name = infoStack.pop();
                infoStack.pop(); // tag
                infoStack.push(tagName + "(" + name + "(" + args + "), " + tolerance + ")");
                return;
            }
            default:
                System.err.println("endElement " + tagName + ": Excess garbage on stack");
                do {}
                while (!tagName.equals(infoStack.pop()));
                infoStack.push(tagName + "(*** ERROR too much info ***)");
                return;
            }
        }
    }

    private final class ConditionHandler
        extends LuvElementHandler {
        public void elementEnd(String tagName, String tweenerText) {
            currentNode.addConditionInfo(Condition.valueOf(tagName),
                                         infoStack.pop());
        }
    }

    private final class ResourceOptionHandler
        extends LuvElementHandler {
        public void elementEnd(String tagName, String tweenerText) {
            String resource = convertTagNameToLiteral(tagName) + SEPARATOR + infoStack.pop();
            currentNode.addActionInfo(resource);
        }
    }

    //
    // Expressions
    //

    private final class UnaryPrefixHandler
        extends LuvElementHandler {
        // TODO: operator precedence?
        public void elementEnd(String tagName, String tweenerText) {
            String expr = infoStack.pop();
            infoStack.push(convertTagNameToLiteral(tagName) + SEPARATOR + expr);
        }
    }
    
    private final class UnaryFunctionHandler
        extends LuvElementHandler {
        public void elementEnd(String tagName, String tweenerText) {
            String expr = infoStack.pop();
            infoStack.push(convertTagNameToLiteral(tagName) + "(" + expr + ")");
        }
    }

    private final class BinaryInfixHandler
        extends LuvElementHandler {
        public void elementStart(String tagName, Attributes attributes) {
        }
        // FIXME: use StringBuilder
        public void elementEnd(String tagName, String tweenerText) {
            String result = infoStack.pop(); // should be tagName
            result = infoStack.pop() + SEPARATOR
                + convertTagNameToLiteral(tagName) + SEPARATOR
                + result;
            // TODO: Handle auto parenthesizing based on operator precedence
            infoStack.push(result);
        }
    }
    
    private final class NaryInfixHandler
        extends LuvElementHandler {
        public void elementStart(String tagName, Attributes attributes) {
            infoStack.push(tagName);
        }
        // FIXME: use StringBuilder
        public void elementEnd(String tagName, String tweenerText) {
            final String oper = convertTagNameToLiteral(tagName);
            String result = infoStack.pop();
            if (tagName.equals(infoStack.peek())) {
                // Unary invocation
                result = oper + SEPARATOR + result;
            }
            else {
                while (!tagName.equals(infoStack.peek())) {
                    result = infoStack.pop() + SEPARATOR + oper + SEPARATOR + result;
                }
            }
            infoStack.pop(); // should be tagName
            // TODO: Handle auto parenthesizing based on operator precedence
            infoStack.push(result);
        }
    }
    
    private final class NaryFunctionHandler
        extends LuvElementHandler {
        public void elementStart(String tagName, Attributes attributes) {
            infoStack.push(tagName);
        }
        // FIXME: use StringBuilder
        public void elementEnd(String tagName, String tweenerText) {
            final String oper = convertTagNameToLiteral(tagName);
            String args = infoStack.pop() + ")";
            while (!tagName.equals(infoStack.peek())) {
                args = infoStack.pop() + ", " + args;
            }
            infoStack.pop(); // better be tagName
            infoStack.push(convertTagNameToLiteral(tagName)
                                + "(" + args);
        }
    }

    private final class NodeVarHandler
        extends LuvElementHandler {
        public void elementEnd(String tagName, String tweenerText) {
            infoStack.push(infoStack.pop()
                           + convertTagNameToLiteral(tagName));
        }
    }

    private final class PairHandler
        extends LuvElementHandler {
        public void elementEnd(String tagName, String tweenerText) {
            String expr = infoStack.pop();
            infoStack.push(infoStack.pop() + " = " + expr);
        }
    }

    private final Map<String, LuvElementHandler> PLAN_HANDLER_MAP =
        new HashMap<String, LuvElementHandler>() {
            {
                put(PLEXIL_SCRIPT, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            StatusMessageHandler.instance().displayErrorMessage(null, "ERROR: loaded script instead of plan");           
                        }
                    });

                put(PLEXIL_PLAN, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            // Notify client
                            if (plan != null && plan.getRootNode() != null)
                                plan.setName(plan.getRootNode().getNodeName());
                            if (receiver != null) 
                                receiver.newPlan(plan);
                        }
                    });
                put(PLEXIL_LIBRARY, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            // Notify client
                            if (plan != null && plan.getRootNode() != null)
                                plan.setName(plan.getRootNode().getNodeName());
                            if (receiver != null) {
                                receiver.newLibrary(plan);
                            }
                        }
                    });

                put(NODE, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            Node parent = currentNode;
                            currentNode = startNode(parent, tagName, attributes);
                            nodeHolder.push(parent);
                            if (parent == null)
                                plan.setRootNode(currentNode);
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            currentNode.setMainAttributesOfNode();
                            currentNode = nodeHolder.pop(); // may be null
                        }
                    });
                put(NODE_ID, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            String parentTag = tagStack.peek();
                            if (NODE.equals(parentTag))
                                currentNode.setNodeName(tweenerText);
                            else // Is reference to other node
                                infoStack.push(tweenerText);
                        }
                    });

                put(NODE_REF, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            // Save direction on stack
                            infoStack.push(attributes.getValue(DIR_ATTR));
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            String direction = infoStack.pop();
                            if (SELF.equals(direction))
                                infoStack.push(SELF);
                            else if (PARENT.equals(direction))
                                infoStack.push(PARENT);
                            else {
                                infoStack.push(direction
                                                    + "(" + tweenerText + ")");
                            }
                        }
                    });

                put(DECL_VAR, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            int libdecl = infoStack.search(LIBRARY_NODE_DECLARATION);
                            if (libdecl > 0) {
                                // This is in a Library declaration (global declaration)
                                // and should be ignored - so clean up stack
                                while (!tagName.equals(infoStack.pop())) {
                                }
                                return;
                            }

                            // Stack should have:
                            // InitialValue (opt) (NYI)
                            // Type
                            // Name
                            // DeclareVariable
                            // In/InOut (opt)
                            String initval = null;
                            String type = infoStack.pop();
                            String name = infoStack.pop();
                            if (!tagName.equals(infoStack.peek())) {
                                // Has initial value - move all up 1
                                initval = type;
                                type = name;
                                name = infoStack.pop();
                            }
                            infoStack.pop(); // clear mark
                            // Is this in an interface declaration?
                            String parentTag = tagStack.peek();
                            String inOut = null;
                            if (IN.equals(parentTag) || (INOUT.equals(parentTag)))
                                inOut = parentTag;
                            else
                                inOut = "--";
                            currentNode.addVariableInfo(new Variable(inOut, name, type, initval));
                        }
                    });

                put(DECL_ARRAY, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            int libdecl = infoStack.search(LIBRARY_NODE_DECLARATION);
                            if (libdecl > 0) {
                                // This is in a Library declaration (global declaration)
                                // and should be ignored - so clean up stack
                                while (!tagName.equals(infoStack.pop())) {
                                }
                                return;
                            }

                            // Stack should have:
                            // InitialValue (opt)
                            // Type
                            // Name (possibly modified by MaxSize handler)
                            // DeclareArray
                            // In/InOut (opt)
                            String initval = null;
                            String type = infoStack.pop();
                            String name = infoStack.pop();
                            if (!tagName.equals(infoStack.peek())) {
                                // Has initial value - move all up 1
                                initval = type;
                                type = name;
                                name = infoStack.pop();
                            }
                            infoStack.pop(); // clear mark
                            // Is this in an interface declaration?
                            String inOut = tagStack.peek();
                            if (!IN.equals(inOut) && !INOUT.equals(inOut)) 
                                inOut = "--";
                            currentNode.addVariableInfo(new ArrayVariable(inOut, name, type, initval));
                        }
                    });

                put(INITIAL_VALUE, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            int depth = infoStack.search(tagName);
                            switch (depth) {
                            case -1:
                                System.err.println("ERROR: endElement " + tagName
                                                   + ": info stack is borked");
                                return;

                            case 0:
                                System.err.println("Warning: endElement " + tagName
                                                   + ": no value on stack");
                                infoStack.pop(); // tag
                                infoStack.push("*** ERROR initial value missing info ***");
                                return;

                            case 1: {
                                // Single item - pop marker and restore the item
                                String item = infoStack.pop();
                                infoStack.pop(); // s/b tagName
                                infoStack.push(item);
                                return;
                            }

                            default: {
                                // Array
                                StringBuilder s = new StringBuilder();
                                s.append(infoStack.pop()); // last value
                                String item = null;
                                while (!tagName.equals(infoStack.peek())) {
                                    s.insert(0, ", ");
                                    s.insert(0, infoStack.pop());
                                }
                                s.insert(0, '{');
                                s.append('}');
                                infoStack.pop(); // pop off InitialValue tag
                                infoStack.push(s.toString()); // replace with value
                                return;
                            }
                            }
                        }
                    });

                put(ARRAY_ELEMENT, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            // Stack should have:
                            // index
                            // array name
                            String index = infoStack.pop();
                            infoStack.push(infoStack.pop() + "[" + index + "]");
                        }
                    });

                // MAX_SIZE occurs in Parameter, Return, DeclareArray
                put(MAX_SIZE, new LuvElementHandler() {
                        // tweak array variable name on stack
                        public void elementEnd(String tagName, String tweenerText) {
                            if (infoStack.search(DECL_ARRAY) != 2)
                                return;

                            // Stack should have:
                            // Type           depth 0
                            // Name                 1
                            // DeclareArray         2
                            //
                            // We need to modify name
                            String type = infoStack.pop();
                            infoStack.push(infoStack.pop() + "[" + tweenerText + "]");
                            infoStack.push(type);
                        }
                    });

                put(LIBRARYNODECALL, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // use as marker
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            // Stack at this point:
                            // --- top ---
                            // alias-n   o
                            // ...       p
                            // alias-0   t.
                            // library name
                            // LibraryNodeCall marker
                            // ...
                            Stack<String> tempStack = new StringStack();
                            do {
                                tempStack.push(infoStack.pop());
                            } while (!tagName.equals(infoStack.peek()));

                            // tempStack now looks like:
                            // --- top ---
                            // libraryName
                            // alias-0
                            // ...
                            // alias-n
                            // --- bottom ---

                            // infoStack:
                            // --- top ---
                            // LibraryNodeCall marker
                            // ....

                            // safety check
                            if (currentNode instanceof LibraryCallNode) {
                                String libName = tempStack.pop();
                                currentNode.setLibraryName(libName);
                                plan.addLibraryReference(libName, (LibraryCallNode) currentNode);

                                if (tempStack.empty())
                                    currentNode.addActionInfo(libName);
                                else {
                                    // Process aliases
                                    StringBuilder b = new StringBuilder();
                                    b.append(libName); // library name
                                    b.append(SEPARATOR);
                                    b.append(tempStack.pop());
                                    while (!tempStack.empty()) {
                                        b.append(", ");
                                        b.append(tempStack.pop());
                                    }
                                    currentNode.addActionInfo(b.toString());
                                }
                            }
                            infoStack.pop(); // remove marker
                        }
                    });

                put(ASSIGNMENT, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            String rhs = infoStack.pop();
                            currentNode.addActionInfo(infoStack.pop() + SEPARATOR + "=" + SEPARATOR + rhs);
                        }
                    });

                put(COMMAND, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.clear();
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            if (!infoStack.peek().endsWith(")")) {
                                String closeParens = infoStack.pop() + SEPARATOR + "(" + SEPARATOR + ")";
                                infoStack.push(closeParens);
                            }
                            if (infoStack.size() == 2) {
                                String second = infoStack.pop();
                                String first = infoStack.pop();
                                first += SEPARATOR + "=" + SEPARATOR + second;
                                infoStack.push(first);
                            }

                            // FIXME
                            if (infoStack.size() > 1)
                                currentNode.addActionInfo(tagName + SEPARATOR + ":" + SEPARATOR + infoStack.pop());
                            else
                                currentNode.addActionInfo(tagName + SEPARATOR + ":" + SEPARATOR + infoStack.pop());                
                            infoStack.clear();
                        }
                    });;

                put(RESOURCE, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            currentNode.addActionInfo(RESOURCE + SEPARATOR + ":");  
                        }
                    });
                        
                put(UPDATE, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            String update = "";
                            while (!tagName.equals(infoStack.peek())) {
                                if (update.isEmpty())
                                    update = infoStack.pop();
                                else
                                    update = infoStack.pop() + ", " + update;
                            }
                            currentNode.addActionInfo(update);
                            infoStack.pop(); // remove mark
                        }
                    });

                // Command, LookupNow, LookupOnChange require expressions.
                // CommandDeclaration, StateDeclaration, Parameter, Return, Pair,
                // DeclareVariable, DeclareArray, ArrayElement (!) don't
                // and expect a CName instead.
                put(NAME, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            String parentTag = tagStack.peek();
                            if (parentTag.equals(LOOKUPNOW)
                                || parentTag.equals(LOOKUPCHANGE)
                                || parentTag.equals(COMMAND))
                            nameExpr = true;
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            // if nameExpr true, value or expr is already on stack 
                            if (!nameExpr)
                                infoStack.push(tweenerText);
                            nameExpr = false;
                        }
                    });

                put(STRING_VAL, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            if (NAME.equals(tagStack.peek())
                                && nameExpr)
                                infoStack.push(tweenerText); // unwrap in these contexts
                            else
                                infoStack.push("\"" + tweenerText + "\"");
                        }
                    });

                put(ARGS, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            String arguments = "";
                            while (!infoStack.isEmpty()) {
                                if (infoStack.peek().equals(ARGS)) {
                                    infoStack.pop();
                                    break;
                                }
                                else if (arguments.isEmpty())
                                    arguments = infoStack.pop() + SEPARATOR;
                                else
                                    arguments = infoStack.pop() + "," + SEPARATOR + arguments;
                            }

                            if (!arguments.isEmpty())
                                infoStack.push("(" + arguments + ")");
                        }
                    });

                put(NODE_TIMEPOINT_VAL,  new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            if (infoStack.size() > 2) {
                                String timepoint = infoStack.pop();
                                String state = infoStack.pop();
                                infoStack.push(infoStack.pop() + "." + state + "." + timepoint);
                            }
                        }
                    });
                put(TIMEPOINT, new LuvElementHandler() {
                        public void elementEnd(String tagName, String tweenerText) {
                            infoStack.push(tweenerText);
                        }
                    });

                // Mark for convenience of LookupHandler
                put(TOLERANCE, new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                    });

                LuvElementHandler handler = new PairHandler();
                put(ALIAS, handler);
                put(PAIR, handler);

                // In, InOut
                handler = new LuvElementHandler() {
                        public void elementStart(String tagName, Attributes attributes) {
                            infoStack.push(tagName); // mark stack
                        }
                        public void elementEnd(String tagName, String tweenerText) {
                            infoStack.pop(); // clear mark
                        }
                    };
                put(IN, handler);
                put(INOUT, handler);

                handler = new StackCleanupHandler();

                // TODO: GLOBAL_DECLARATIONS handler that ignores everything
                put(GLOBAL_DECLARATIONS, handler);
                put(COMMAND_DECLARATION, handler);
                put(LIBRARY_NODE_DECLARATION, handler);
                put(STATE_DECLARATION, handler);
                // These occur in CommandDeclaration, StateDeclaration
                put(PARAMETER, handler);
                put(RETURN, handler);

                handler = new ResourceOptionHandler();
                for (String r: RESOURCE_TAGS)
                    put(r, handler);

                handler = new ConditionHandler();
                for (Condition c: Condition.values())
                    put(c.toString(), handler);

                handler = new LookupHandler();
                for (String l: LOOKUP_TAGS)
                    put(l, handler);

                handler = new UnaryPrefixHandler();
                for (String u: UNARY_PREFIX_OPERATORS)
                    put(u, handler);

                handler = new UnaryFunctionHandler();
                for (String u: UNARY_FUNCTIONS)
                    put(u, handler);
                
                handler = new BinaryInfixHandler();
                for (String b: BINARY_OPERATORS)
                    put(b, handler);

                handler = new NaryInfixHandler();
                for (String n: NARY_INFIX_OPERATORS)
                    put(n, handler);

                handler = new NaryFunctionHandler();
                for (String n: NARY_FUNCTIONS)
                    put(n, handler);

                handler = new NodeVarHandler();
                for (String n: NODE_STATE_TAGS)
                    put(n, handler);

                // Tags with NCName content
                handler = new NCNameHandler();
                for (String v: VARIABLE_REFS)
                    put (v, handler);
                for (String l: SIMPLE_LITERALS)
                    put (l, handler);
                put(NODE_PARAMETER, handler);
                put(TYPE, handler);
            }
        };

    private Node startNode(Node parent, String tagName, Attributes attributes) {
        String nodeTypeAttr = attributes.getValue(NODETYPE_ATTR);
        NodeType ntype = NodeType.valueOf(nodeTypeAttr);
        Node node = Node.makeNode(ntype);

        // Set display type
        node.setProperty(NODETYPE_ATTR_PLX, nodeTypeAttr.toString());
        String parentType = null;
        if (parent != null)
            parentType = parent.getProperty(NODETYPE_ATTR);
        if (parentType != null) {
            if (parentType.equals(AUX_THEN))
                node.setProperty(NODETYPE_ATTR, THEN + nodeTypeAttr);
            else if (parentType.equals(AUX_ELSE))
                node.setProperty(NODETYPE_ATTR, ELSE + nodeTypeAttr);
            else
                node.setProperty(NODETYPE_ATTR, nodeTypeAttr);
        }
        else // root node
            node.setProperty(NODETYPE_ATTR, nodeTypeAttr);   

        if (parent != null)
            parent.addChild(node);

        // add attributes for this node
        // NODETYPE_ATTR identify which icon should be used when displaying plan
        // AUX is nodes that should be hidden if in EPX view
        for (int i = 0; i < attributes.getLength(); ++i) {
            String qname = attributes.getQName(i);
            String value = attributes.getValue(i);
            switch (qname) {
            case NODETYPE_ATTR:
                // already handled above
                break;

            case EPX_ATTR: {
                // save plexil core view icon
                String nodeType = node.getProperty(NODETYPE_ATTR);
                if (nodeType.contains(THEN) || nodeType.contains(ELSE))
                    node.setProperty(NODETYPE_ATTR_PLX, nodeType.substring(THEN.length())); // see above
                else
                    node.setProperty(NODETYPE_ATTR_PLX, nodeType);

                // insert epx view icon
                if (value.equals(THEN))
                    node.setProperty(NODETYPE_ATTR, AUX_THEN);
                else if (value.equals(ELSE))
                    node.setProperty(NODETYPE_ATTR, AUX_ELSE);
                else if (parent != null) {
                    if (parent.getProperty(NODETYPE_ATTR) != null &&
                        parent.getProperty(NODETYPE_ATTR).equals(AUX_THEN)) 
                        node.setProperty(NODETYPE_ATTR, THEN + value);                    
                    else if (parent.getProperty(NODETYPE_ATTR) != null &&
                             parent.getProperty(NODETYPE_ATTR).equals(AUX_ELSE)) 
                        node.setProperty(NODETYPE_ATTR, ELSE + value);
                    else 
                        node.setProperty(NODETYPE_ATTR, value);
                }
                else 
                    node.setProperty(NODETYPE_ATTR, value);
            }
                break;

            default:
                node.setProperty(qname, value);                
            }
        }
        return node;
    }

    /**
     * Returns the plan.
     */
    public Plan getPlan()
    {
        return plan;
    }

    private boolean isNodeState(String tag)
    {
        return null != NodeState.valueOf(tag);
    }

    private String convertTagNameToLiteral(String tag)
    {
        String result = TAG_LITERAL_MAP.get(tag);
        if (result != null)
            return result;
        return tag;
    }

}

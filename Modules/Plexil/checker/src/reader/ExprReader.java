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

package reader;

import java.util.*;

import net.n3.nanoxml.*;

import model.GlobalDecl;
import model.GlobalDecl.CallType;
import model.GlobalDeclList;
import model.Node;
import model.Plan;
import model.Var;
import model.VarList;
import model.expr.*;
import model.expr.NodeVarRef.NodeRefDir;

public class ExprReader {

	public ExprReader() {
	}
	
	public Expr xmlToExpr(IXMLElement xml, GlobalDeclList decls, Node node) {
		if (xml == null)
			return null;

		String tag = xml.getName();

        // Check common special cases first
        if (tag.endsWith("Value"))
            return parseLiteral(xml, node);
        if (tag.endsWith("Variable"))
            return parseVarRef(xml, node);
        if (tag.endsWith("RHS")) {
            if (!xml.hasChildren()) {
                System.out.println("Reader error: empty " + tag + " element");
                return null;
            }
            return xmlToExpr(xml.getChildAtIndex(0), decls, node);
        }
        if (tag.startsWith("EQ") || tag.startsWith("NE"))
            return parseEqualityExpr(xml, decls, node);

        switch (tag) {
        case "OR":
        case "XOR":
        case "AND":
        case "NOT":
            return parseLogicalExpr(xml, decls, node);

        case "GE":
        case "GT":
        case "LE":
        case "LT":
            return parseNumericComparison(xml, decls, node);

        case "ADD":
        case "SUB":
        case "MUL":
        case "DIV":
        case "MOD":
        case "MAX":
        case "MIN":
        case "SQRT": // ???
        case "ABS":
            return parseArithmeticExpr(xml, decls, node);

        case "CEIL":
        case "FLOOR":
        case "ROUND":
        case "TRUNC":
        case "REAL_TO_INT":
            return parseIntegerConversion(xml, decls, node);

        case "LookupNow":
        case "LookupOnChange":
            return parseLookup(xml, decls, node);

        case "ArrayElement":
            return parseArrayElement(xml, decls, node);

        case "ArraySize":
        case "ArrayMaxSize":
            return parseArraySize(xml, decls, node);

        case "STRLEN":
            return parseStrlen(xml, decls, node);

        case "Concat":
            return parseConcat(xml, decls, node);

        case "IsKnown":
            return parseIsKnown(xml, decls, node);

        default:
            System.out.println("Reader error: Unknown expression element " + tag);
            return null;
        }
    }

    private Expr parseLiteral(IXMLElement xml, Node node) {
        String tag = xml.getName();
        ExprType type = null;
        switch (tag) {
        case "ArrayValue":
            return parseArrayLiteral(xml, node); // special case

        case "NodeTimepointValue":
            return parseNodeVarRef(xml, node); // not a literal
            
        case "BooleanValue":
            type = ExprType.Bool;
            break;

        case "IntegerValue":
            type = ExprType.Int;
            break;

        case "NodeCommandHandleValue":
            type = ExprType.NodeCommandHandle;
            break;

        case "NodeFailureValue":
            type = ExprType.NodeFailureType;
            break;

        case "NodeOutcomeValue":
            type = ExprType.NodeOutcome;
            break;

        case "NodeStateValue":
            type = ExprType.NodeState;
            break;

        case "RealValue":
            type = ExprType.Real;
            break;
            
        case "StringValue":
            type = ExprType.Str;
            break;

        default:
            System.out.println("Reader error: " + tag + " is not a valid literal value");
            return null;
        }

        if (xml.hasChildren()) {
            System.out.println("Reader error: ill-formed " + tag + " element");
            return null;
        }

        String value = xml.getContent();
        if (type != ExprType.Str && (value == null || value.isEmpty())) {
            System.out.println("Reader error: empty " + tag + " element");
            return null;
        }

        if (type == ExprType.Str)
            return new StringLiteral(xml.getContent());
        else
            return new LiteralExpr(type, xml.getContent());
    }

    @SuppressWarnings("unchecked")
    private Expr parseArrayLiteral(IXMLElement xml, Node node) {
        String typename = xml.getAttribute("Type", null);
        if (typename == null) {
            System.out.println("Reader error: ArrayValue element missing required Type attribute");
            return null;
        }

        ExprType elementType = ExprType.typeForName(typename);
        if (elementType == null) {
            System.out.println("Reader error: illegal Type attribute value " + typename +
                               " in ArrayValue element");
            return null;
        }

        ArrayLiteral result = new ArrayLiteral(elementType.arrayType());
        for (Object o : xml.getChildren()) {
            IXMLElement x = (IXMLElement) o; // unchecked cast
            Expr e = parseLiteral(x, node);
            if (e == null)
                return null; // error already reported
            if (e.getType() != elementType) {
                System.out.println("Reader error: wrong type element " + x.getName()
                                   + " in ArrayValue");
                return null;
            }
            result.addElement(e);
        }

        return result;
    }

    private Expr parseVarRef(IXMLElement xml, Node node) {
        ExprType type = null;
        switch (xml.getName()) {
        case "ArrayVariable":
            type = ExprType.GenericArray;
            break;

        case "BooleanVariable":
            type = ExprType.Bool;
            break;

        case "IntegerVariable":
            type = ExprType.Int;
            break;

        case "RealVariable":
            type = ExprType.Real;
            break;

        case "StringVariable":
            type = ExprType.Str;
            break;

        default:
            if (xml.getName().startsWith("Node"))
                return parseNodeVarRef(xml, node);
            System.out.println("Reader error: found " + xml.getName()
                               + " element, was expecting variable reference");
            return null;
        }

        if (xml.hasChildren()) {
            System.out.println("Reader error: ill-formed " + xml.getName() + " element");
            return null;
        }

        String id = xml.getContent();
        if (id == null || id.isEmpty()) {
            System.out.println("Reader error: empty " + xml.getName() + " element");
            return null;
        }

        Var decl = null;
        if (node != null)
            decl = node.findVarInScope(id);

        return new VarRef(type, id, decl);
    }

    private Expr parseNodeVarRef(IXMLElement xml, Node node) {
        ExprType type = null;
        switch (xml.getName()) {
        case "NodeCommandHandleVariable":
            type = ExprType.NodeCommandHandle;
            break;
            
        case "NodeFailureVariable":
            type = ExprType.NodeFailureType;
            break;
            
        case "NodeOutcomeVariable":
            type = ExprType.NodeOutcome;
            break;
            
        case "NodeStateVariable":
            type = ExprType.NodeState;
            break;

        case "NodeTimepointValue":
            type = ExprType.NodeTimepointValue;
            break;

        default:
            System.out.println("Reader error: found " + xml.getName()
                               + " element, was expecting node variable reference");
            return null;
        }

        if (type == ExprType.NodeTimepointValue) {
            if (xml.getChildrenCount() != 3) {
                System.out.println("Reader error: ill-formed " + xml.getName() + " element");
                return null;
            }
        }
        else if (xml.getChildrenCount() != 1) {
            System.out.println("Reader error: ill-formed " + xml.getName()
                               + " element");
            return null;
        }

        IXMLElement noderef = xml.getChildAtIndex(0);
        String nodeId = noderef.getContent();
        String dirname = null;
        NodeRefDir dir = null;

        switch (noderef.getName()) {
        case "NodeId":
            nodeId = noderef.getContent();
            if (nodeId == null || nodeId.isEmpty()) {
                System.out.println("Reader error: ill-formed " + noderef.getName()
                                   + " element");
                return null;
            }
            break;
            
        case "NodeRef":
            dirname = noderef.getAttribute("dir", null);
            if (dirname == null || dirname.isEmpty()) {
                System.out.println("Reader error: missing dir attribute in " + noderef.getName()
                                   + " element");
                return null;
            }
            switch (dirname) {
            case "self":
                dir = NodeRefDir.Self;
                if (nodeId != null && !nodeId.isEmpty())
                    System.out.println("Reader warning: ignoring content in " + noderef.getName()
                                       + " element with dir = \"" + dirname + "\"");
                nodeId = null;
                break;

            case "parent":
                dir = NodeRefDir.Parent;
                if (nodeId != null && !nodeId.isEmpty())
                    System.out.println("Reader warning: ignoring content in " + noderef.getName()
                                       + " element with dir = \"" + dirname + "\"");
                nodeId = null;
                break;

            case "child":   // TODO
                dir = NodeRefDir.Child;
                if (nodeId != null && nodeId.isEmpty()) {
                    System.out.println("Reader error: " + noderef.getName()
                                       + " element with dir = \"" + dirname
                                       + "\" missing required node name");
                    return null;
                }
                break;

            case "sibling": // TODO
                dir = NodeRefDir.Sibling;
                if (nodeId != null && nodeId.isEmpty()) {
                    System.out.println("Reader error: " + noderef.getName()
                                       + " element with dir = \"" + dirname
                                       + "\" missing required node name");
                    return null;
                }
                break;

            default:
                System.out.println("Reader error: invalid value "
                                   + nodeId + " for dir attribute in " + noderef.getName());
                return null;
            }
        }

        if (type == ExprType.NodeTimepointValue) {
            IXMLElement stateXml = xml.getChildAtIndex(1);
            if (!"NodeStateValue".equals(stateXml.getName())) {
                System.out.println("Reader error: " + xml.getName()
                                   + " missing required NodeStateName element");
                return null;
            }
            if (!stateXml.isLeaf()) {
                System.out.println("Reader error: ill-formed " + stateXml.getName() + " element");
                return null;
            }
            String state = stateXml.getContent();
            if (state == null || state.isEmpty()) {
                System.out.println("Reader error: ill-formed " + stateXml.getName() + " element");
                return null;
            }
            // TODO: check state name content
            
            IXMLElement whichXml = xml.getChildAtIndex(2);
            if (!"Timepoint".equals(whichXml.getName())) {
                System.out.println("Reader error: " + xml.getName()
                                   + " missing required Timepoint element");
                return null;
            }
            if (!whichXml.isLeaf()) {
                System.out.println("Reader error: ill-formed " + whichXml.getName() + " element");
                return null;
            }
            String which = whichXml.getContent();
            if (which == null || which.isEmpty()) {
                System.out.println("Reader error: ill-formed " + whichXml.getName() + " element");
                return null;
            }
            // TODO: check timepoint name content

            return new NodeTimepointRef(nodeId, dir, state, which);
        }
        return new NodeVarRef(type, nodeId, dir);
    }

    private Expr parseEqualityExpr(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 2) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        Expr left = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (left == null)
            return null;
        Expr right = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (right == null)
            return null;
        return new Equality(xml.getName(), left, right);
    }

    @SuppressWarnings("unchecked")
    private Expr parseLogicalExpr(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (!xml.hasChildren()) {
            System.out.println("Reader error: no arguments for " + xml.getName());
            return null;
        }
        LogicalOperator result = new LogicalOperator(xml.getName());
        for (Object o : xml.getChildren()) {
            IXMLElement argXml = (IXMLElement) o; // unchecked cast
            Expr arg = xmlToExpr(argXml, decls, node);
            if (arg == null)
                return null;
            result.addSubExpr(arg);
        }
        return result;
    }

    private Expr parseNumericComparison(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 2) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        Expr left = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (left == null)
            return null;
        Expr right = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (right == null)
            return null;
        return new NumericComparison(xml.getName(), left, right);
    }

    @SuppressWarnings("unchecked")
    private Expr parseArithmeticExpr(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (!xml.hasChildren()) {
            System.out.println("Reader error: no arguments for " + xml.getName());
            return null;
        }
        ArithmeticOperator result = new ArithmeticOperator(xml.getName());
        for (Object o : xml.getChildren()) {
            IXMLElement argXml = (IXMLElement) o; // unchecked cast
            Expr arg = xmlToExpr(argXml, decls, node);
            if (arg == null)
                return null;
            result.addSubExpr(arg);
        }
        return result;
    }
    
    private Expr parseIntegerConversion(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 1) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        Expr arg = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (arg == null)
            return null;
        return new IntegerConversion(xml.getName(), arg);
    }

    @SuppressWarnings("unchecked")
    private Expr parseLookup(IXMLElement xml, GlobalDeclList decls, Node node) {
        IXMLElement nameXml = xml.getChildAtIndex(0);
        if (!"Name".equals(nameXml.getName()) || !nameXml.hasChildren())
            return null; // malformed XML
        Expr nameExpr = xmlToExpr(nameXml.getChildAtIndex(0), decls, node);

        // Try to get lookup name
        GlobalDecl d = null;
        if (nameExpr instanceof LiteralExpr
            && nameExpr.getType() == ExprType.Str) {
            // Get lookup type from declarations
            String stateName = ((LiteralExpr) nameExpr).getValueString(); // unchecked cast
            if (!stateName.isEmpty()) {
                d = decls.findCallById(stateName);
                if (d != null && d.getType() != CallType.Lookup) {
                    System.out.println("Reader warning: in " + xml.getName()
                                       + ", lookup name " + stateName + " is declared as a "
                                       + d.getType().toString());
                }
            }
        }
        
        LookupExpr result = new LookupExpr(nameExpr, d);

        // Process tolerance, arguments
        if (xml.getChildrenCount() > 1) {
            int argIdx = 1;
            if ("LookupOnChange".equals(xml.getName())) {
                IXMLElement tolXml = xml.getChildAtIndex(argIdx);
                if ("Tolerance".equals(tolXml.getName())) {
                    ++argIdx;
                    result.setTolerance(xmlToExpr(tolXml.getChildAtIndex(0),
                                                  decls,
                                                  node));
                }
            }
            if (argIdx < xml.getChildrenCount()) {
                IXMLElement argsXml = xml.getChildAtIndex(argIdx);
                if ("Arguments".equals(argsXml.getName()))
                    for (Object o : argsXml.getChildren()) {
                        IXMLElement argXml = (IXMLElement) o; // unchecked cast
                        result.addArgument(xmlToExpr(argXml, decls, node));
                    }
                else
                    System.out.println("Reader error: in " + xml.getName()
                                       + ", found " + argsXml.getName()
                                       + " element where Arguments expected");
            }
        }

        return result;
    }

    private Expr parseArrayElement(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 2) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        IXMLElement aryXml = xml.getChildAtIndex(0);
        Expr array = null;
        if ("Name".equals(aryXml.getName())) {
            // old style var reference
            String varName = aryXml.getContent();
            if (varName == null || varName.isEmpty()) {
                System.out.println("Reader error: empty Name element in " + xml.getName());
                return null;
            }
            Var def = node.findVarInScope(varName);
            ExprType arrayType = ExprType.GenericArray;
            // determine array type now, if possible
            if (def != null) {
                ExprType defType = def.getType();
                if (defType != null && defType.isArrayType())
                    arrayType = defType;
            }
            array = new VarRef(arrayType, varName, def);
        }
        else 
            array = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (array == null) // array parse failed
            return null;

        IXMLElement idxXml = xml.getChildAtIndex(1);
        if (!"Index".equals(idxXml.getName())) {
            System.out.println("Reader error in " + xml.getName()
                               + ": found " + idxXml.getName()
                               + " element where Index expected");
            return null;
        }
        Expr index = xmlToExpr(idxXml.getChildAtIndex(0), decls, node);
        if (index == null)
            return null;
        return new ArrayElement(array, index);
    }

    private Expr parseArraySize(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (!xml.isLeaf()) {
            System.out.println("Reader error: invalid " + xml.getName() + " element");
            return null;
        }

        // old-style var reference
        String varName = xml.getContent();
        if (varName == null || varName.isEmpty()) {
            System.out.println("Reader error: empty " + xml.getName() + " element");
            return null;
        }
        Var def = node.findVarInScope(varName);
        // precise type is irrelevant
        return new VarRef(ExprType.GenericArray, varName, def);
    }

    private Expr parseStrlen(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 1) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        Expr arg = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        return new Strlen(arg);
    }

    @SuppressWarnings("unchecked")
    private Expr parseConcat(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() < 2) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        Concat result = new Concat();
        for (Object o : xml.getChildren()) {
            IXMLElement argXml = (IXMLElement) o; // unchecked cast
            Expr arg = xmlToExpr(argXml, decls, node);
            if (arg == null)
                return null; // parse error
            result.addSubExpr(arg);
        }
        return result;
    }

    private Expr parseIsKnown(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 1) {
            System.out.println("Reader error: wrong number of arguments for " + xml.getName());
            return null;
        }
        Expr arg = xmlToExpr(xml.getChildAtIndex(0), decls, node);
        if (arg == null)
            return null; // parse error
        return new IsKnown(arg);
    }

}

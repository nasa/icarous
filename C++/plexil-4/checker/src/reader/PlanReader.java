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

import java.util.HashSet;
import java.util.Set;
import java.util.Vector;

import net.n3.nanoxml.*;

import model.*;
import model.Action.ActionType;
import model.expr.*;
import model.GlobalDecl.CallType;
import model.Var.VarMod;

public class PlanReader {
	public static final String NodeRootText = "Node";
	public static final String NodeIdText = "NodeId";
	public static final String NameText = "Name";
	public static final String NodeBodyText = "NodeBody";
	public static final String ArgumentsText = "Arguments";
	public static final String ReturnCallText = "Returns";
	public static final String AliasCallText = "Alias";
    public static final String InterfaceText = "Interface";
    public static final String InText = "In";
    public static final String InOutText = "InOut";
	public static final String VarListDeclarationText = "VariableDeclarations";
	public static final String VarDeclarationText = "DeclareVariable";
	public static final String ArrayDeclarationText = "DeclareArray";
	public static final String CommandCallText = "Command";
	public static final String LibraryNodeCallText = "LibraryNodeCall";
	public static final String LookupCallText = "Lookup";
	public static final String AssignmentText = "Assignment";
	public static final String UpdateText = "Update";
	public static final String NodeListText = "NodeList";
	public static final String BooleanTrueText = "true";
	public static final String BooleanFalseText = "false";
	public static final String ConditionText = "Condition";
	public static final String StartCondText = "StartCondition";
	public static final String EndCondText = "EndCondition";
	public static final String PreCondText = "PreCondition";
	public static final String PostCondText = "PostCondition";
	public static final String RepeatCondText = "RepeatCondition";
	public static final String InvarCondText = "InvariantCondition";
	public static final String SkipCondText = "SkipCondition";

	private ExprReader exprReader = XmlReader.getExprReader();

	public Node xmlToNode(IXMLElement xml, GlobalDeclList decls, Node parent) {
		// Need a valid node IXMLElement
		if (xml == null || !xml.getName().equals(NodeRootText))
			return null;

		Node n = new Node();
        n.setParent(parent);

		for (IXMLElement component : getChildren(xml)) {
			String compText = component.getName();
            switch (compText) {
            case "NodeId":
                n.setID(component.getContent());
                break;

            case "Interface":
                n.addVarDefs(buildInterfaceList(component));
                break;

            case "VariableDeclarations":
                n.addVarDefs(buildVarDefList(component));
                break;

            case "NodeBody":
                parseNodeBody(component.getChildAtIndex(0), decls, n); // for effect
                break;

            default:
                if (compText.endsWith(ConditionText))
                    n.addCondition(buildCondition(component, decls, n));
                // Priority, Comment ignored
                break;
            }
		}
		
		return n;
	}
	
    private Condition buildCondition(IXMLElement xml, GlobalDeclList decls, Node node)
	{
		Condition.ConditionType type = null;
        switch (xml.getName()) {
        case "StartCondition":
			type = Condition.ConditionType.Start;
            break;

        case "EndCondition":
			type = Condition.ConditionType.End;
            break;
            
        case "PreCondition":
			type = Condition.ConditionType.Pre;
            break;
            
        case "PostCondition":
			type = Condition.ConditionType.Post;
            break;

        case "InvariantCondition":
			type = Condition.ConditionType.Invar;
            break;
            
        case "RepeatCondition":
			type = Condition.ConditionType.Repeat;
            break;
            
        case "SkipCondition":
			type = Condition.ConditionType.Skip;
            break;

        case "ExitCondition":
            type = Condition.ConditionType.Exit;
            break;

        default:
            System.out.println("Reader error: in node " + node.getID()
                               + ", unknown condition name " + xml.getName());
            return null;
        }
		
		return new Condition(type, exprReader.xmlToExpr(xml.getChildAtIndex(0), decls, node));
	}

    public VarList buildInterfaceList(IXMLElement xml) {
        VarList result = new VarList();
        for (IXMLElement io : getChildren(xml)) {
            String ioTag = io.getName();
            if (ioTag.equals(InText)) {
                for (IXMLElement var : getChildren(io)) {
                    Var v = buildVarDef(var);
                    if (v != null) {
                        v.setMod(VarMod.In);
                        result.add(v);
                    }
                }
            }
            else if (ioTag.equals(InOutText)) {
                for (IXMLElement var : getChildren(io)) {
                    Var v = buildVarDef(var);
                    if (v != null) {
                        v.setMod(VarMod.InOut);
                        result.add(v);
                    }
                }
            }
        }
        return result;
    }

	private VarList buildVarDefList(IXMLElement xml)
	{
		VarList list = new VarList();
		for (IXMLElement var : getChildren(xml)) {
			Var v = buildVarDef(var);
			if (v != null)
				list.add(v);
		}
		return list;
	}
	
    private Var buildVarDef(IXMLElement xml) {
        String tag = xml.getName(); 
		String name = xml.getChildAtIndex(0).getContent();
		String typeName = xml.getChildAtIndex(1).getContent();
		ExprType varType = ExprType.typeForName(typeName);
        if (varType == null) {
            System.out.println("Reader error: " + typeName + " is not a valid Type name in " + tag);
            return null;
        }

		if (tag.equals(VarDeclarationText))
			return new Var(name, varType);
		
        // Must be an array declaration
		String arraySize = xml.getChildAtIndex(2).getContent();
        return new VarArray(name, varType, Integer.parseUnsignedInt(arraySize));
	}
	
	private void parseNodeBody(IXMLElement xml, GlobalDeclList decls, Node node) {
		if (xml == null)
			return; // empty node

        String action = xml.getName();
        switch (action) {
        case "Assignment":
			parseAssignment(xml, decls, node);
            return;

        case "Command":
            parseCommand(xml, decls, node);
            return;

        case "LibraryNodeCall":
            parseLibraryCall(xml, decls, node);
            return;

        case "NodeList":
            parseNodeList(xml, decls, node);
            return;

        case "Update":
            parseUpdate(xml, decls, node);
            return;

        default:
            System.out.println("Reader error: invalid NodeBody element " + action);
            return;
        }
    }
	
	public void parseAssignment(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.getChildrenCount() != 2) {
            System.out.println("Reader error: ill-formed " + xml.getName());
            return;
        }
        Expr lhs = parseLHS(xml.getChildAtIndex(0), decls, node);
        // Just check that the RHS is one of these keywords
        IXMLElement rhsXml = xml.getChildAtIndex(1);
        if (rhsXml.getChildrenCount() != 1) {
            System.out.println("Reader error: ill-formed Assignment value expression");
            return;
        }

        switch (rhsXml.getName()) {
        case "ArrayRHS":
        case "BooleanRHS":
        case "NumericRHS":
        case "StringRHS":
            break;

        default:
            System.out.println("Reader error: invalid element " + rhsXml.getName()
                               + " in right hand side of Assignment");
            return;
        }

		Expr rhs = exprReader.xmlToExpr(xml.getChildAtIndex(1), decls, node);
		node.setAction(new Assignment(lhs, rhs));
	}

    @SuppressWarnings("unchecked")
	public void parseCommand(IXMLElement xml, GlobalDeclList decls, Node node) {
        int i = 0;
        IXMLElement child = xml.getChildAtIndex(i);
        Expr lhs = null;
        if ("ResourceList".equals(child.getName())) {
            // TODO: parse ResourceList
            ++i;
            child = xml.getChildAtIndex(i);
        }
        if (!"Name".equals(child.getName())) {
            // Parse optional result variable
            lhs = parseLHS(child, decls, node);
            ++i;
            child = xml.getChildAtIndex(i);
        }
        if (!"Name".equals(child.getName())) {
            System.out.println("Reader error: in Command, found " + child.getName()
                               + " element where Name was expected");
            return;
        }
        if (child.isLeaf()) {
            System.out.println("Reader error: in Command, ill-formed " + child.getName()
                               + " element");
            return;
        }
        Expr nameExp = exprReader.xmlToExpr(child.getChildAtIndex(0), decls, node);
        GlobalDecl cmdDecl = null;
        if (nameExp instanceof LiteralExpr
            && nameExp.getType() == ExprType.Str) {
            String staticID = ((LiteralExpr) nameExp).getValueString(); // unchecked cast
            cmdDecl = decls.findCallById(staticID);
            if (cmdDecl != null && cmdDecl.getType() != CallType.Command) {
                System.out.println("Reader warning: in Command node " + node.getID()
                                   + ", command name " + staticID + " is declared as a "
                                   + cmdDecl.getType().toString());
            }
        }

        Command result = new Command(nameExp, lhs, cmdDecl);

        // Optional parameters
        ++i;
        if (i < xml.getChildrenCount()) {
            IXMLElement argsXml = xml.getChildAtIndex(i);
            if ("Arguments".equals(argsXml.getName()))
                for (Object o : argsXml.getChildren()) {
                    IXMLElement argXml = (IXMLElement) o; // unchecked cast
                    result.addArgument(exprReader.xmlToExpr(argXml, decls, node));
                }
            else
                System.out.println("Reader error: in " + xml.getName()
                                   + ", found " + argsXml.getName()
                                   + " element where Arguments expected");
        }

        node.setAction(result);
    }

    public Expr parseLHS(IXMLElement xml, GlobalDeclList decls, Node node) {
        Expr result = exprReader.xmlToExpr(xml, decls, node);
        if (result instanceof VarRef
            || result instanceof ArrayElement)
            return result;
        System.out.println("Reader error: " + xml.getName()
                           + " is invalid in left hand side of "
                           + xml.getParent().getName());
        return null;
    }

	public void parseLibraryCall(IXMLElement xml, GlobalDeclList decls, Node node) {
        if (xml.isLeaf()) {
            System.out.println("Reader error: ill-formed " + xml.getName() + " element");
            return;
        }
        IXMLElement nameXml = xml.getChildAtIndex(0);
        if (!"NodeId".equals(nameXml.getName())) {
            System.out.println("Reader error: in LibraryNodeCall, found "
                               + nameXml.getName() + " element where NodeId expected");
            return;
        }
        if (!nameXml.isLeaf()) {
            System.out.println("Reader error: in LibraryNodeCall, ill-formed "
                               + nameXml.getName() + " element");
            return;
        }
        String nodeId = nameXml.getContent();
        // N.B. declaration may be null, not worrying about that here
        LibraryCall result = new LibraryCall(nodeId, decls.findCallById(nodeId));

        // parse aliases
        for (int i = 1; i < xml.getChildrenCount(); ++i) {
            IXMLElement a = xml.getChildAtIndex(i);
            if (!"Alias".equals(a.getName())) {
                System.out.println("Reader error: in LibraryNodeCall, found "
                                   + a.getName() + " element where Alias expected");
                return;
            }
            if (a.getChildrenCount() != 2) {
                System.out.println("Reader error: in LibraryNodeCall, ill-formed "
                                   + a.getName() + " element");
                return;
            }

            IXMLElement parmNameXml = a.getChildAtIndex(0);
            if (!"NodeParameter".equals(parmNameXml.getName())) {
                System.out.println("Reader error: in LibraryNodeCall Alias, found "
                                   + parmNameXml.getName()
                                   + " element where NodeParameter expected");
                return;
            }
            if (!parmNameXml.isLeaf()) {
                System.out.println("Reader error: in LibraryNodeCall, ill-formed "
                                   + parmNameXml.getName() + " element");
                return;
            }
            String aliasName = parmNameXml.getContent();
            
            result.addAlias(aliasName,
                            exprReader.xmlToExpr(a.getChildAtIndex(1), decls, node));
        }
        node.setAction(result);
    }

    public void parseNodeList(IXMLElement xml, GlobalDeclList decls, Node node) {
        // TODO: add duplicate node ID check
        for (IXMLElement childXml : getChildren(xml)) {
            Node child = xmlToNode(childXml, decls, node);
            if (child != null)
                node.addChild(child);
        }
    }

	public void parseUpdate(IXMLElement xml, GlobalDeclList decls, Node node) {
        Update result = new Update();
        Set<String> names = new HashSet<String>(xml.getChildrenCount());
        for (IXMLElement pair : getChildren(xml)) {
            if (!"Pair".equals(pair.getName())) {
                System.out.println("Reader error: in Update, found \"" + pair.getName()
                                   + "\" element where Pair expected");
                return;
            }
            if (pair.getChildrenCount() != 2) {
                System.out.println("Reader error: ill-formed Pair");
                return;
            }

            // Name
            IXMLElement nameXml =  pair.getChildAtIndex(0);
            if (!"Name".equals(nameXml.getName())) {
                System.out.println("Reader error: in Pair, found " + nameXml.getName()
                                   + " element where Name expected");
                return;
            }
            if (!nameXml.isLeaf()) {
                System.out.println("Reader error: in Pair, ill-formed Name element");
                return;
            }
            String name = nameXml.getContent();
            if (names.contains(name))
                System.out.println("Reader warning: in Update, multiple Pairs with same name "
                                   + name);
            else
                names.add(name);

            // expr
            Expr e = exprReader.xmlToExpr(pair.getChildAtIndex(1), decls, node);
            if (e != null)
                result.addArgument(e);
        }
        node.setAction(result);
    }
	
	// IXMLElement.getChildren() only returns Vector. Having a typed vector is nicer, so we type cast here.
	@SuppressWarnings("unchecked")
	private static Vector<IXMLElement> getChildren(IXMLElement elem)
	{
		Vector<IXMLElement> children = elem.getChildren();
		return children;
	}
}

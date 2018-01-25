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

import net.n3.nanoxml.*;
import model.*;
import java.util.Vector;

public class XmlReader {

	private Plan plan = null;

    private static DeclReader declReader = null;
    private static ExprReader exprReader = null;
    private static PlanReader planReader = null;

    public static DeclReader getDeclReader() {
        if (declReader == null)
            declReader = new DeclReader();
        return declReader;
    }

    public static ExprReader getExprReader() {
        if (exprReader == null)
            exprReader = new ExprReader();
        return exprReader;
    }

    public static PlanReader getPlanReader() {
        if (planReader == null)
            planReader = new PlanReader();
        return planReader;
    }

	
	public Plan getPlan() { return plan; }
	public GlobalDeclList getDecls() { return plan.getDecls(); }

	@SuppressWarnings("unchecked")
	public void readPlan(IXMLElement p) {
		if (!p.getName().equals("PlexilPlan")) {
			System.out.println("XML document node is not a PlexilPlan. Exiting.");
			return;
		}

        plan = new Plan();

		Vector<IXMLElement> elts = p.getChildren();
        IXMLElement elt = elts.get(0);

        GlobalDeclList decls = null;

        // Global declarations are optional
        if ("GlobalDeclarations".equals(elt.getName())) {
            decls = getDeclReader().xmlToDecls(elt);
            plan.setDecls(decls);
            elt = elts.get(1);
        }
        else
            decls = new GlobalDeclList();

        if (!"Node".equals(elt.getName())) {
			System.out.println("PlexilPlan node has no root Node. Exiting.");
			return;
        }

        Node root = getPlanReader().xmlToNode(elt, decls, null);
        if (root != null)
            plan.setNode(root);
	}
	
	public void printDeclCalls()
	{
		for (GlobalDecl decl : plan.getDecls())
			decl.print();
	}

	public void printNodeCalls()
	{
        if (plan.getNode() == null)
            return;
        plan.getNode().print();
	}

	private void printNodeTree(IXMLElement node)
	{
		if (node == null)
			return;
		
		System.out.println(node.getName());
		printChildren(node, 1, true);
		System.out.println();
	}
	
	@SuppressWarnings("unchecked")
	private void printChildren(IXMLElement nodes, int indent, boolean recurse)
	{
		Vector<IXMLElement> nodeList = nodes.getChildren();
		for (IXMLElement currentNode : nodeList)
		{
			String text = currentNode.getName();
			for (int j = 0; j < indent; j++)
				System.out.print("  ");
			System.out.println(text);
			if (recurse)
				printChildren(currentNode, indent + 1, recurse);
		}
	}
}

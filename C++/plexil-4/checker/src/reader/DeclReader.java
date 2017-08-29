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

import java.util.Vector;

import net.n3.nanoxml.*;

import model.expr.Expr;
import model.expr.ExprType;
import model.GlobalDecl;
import model.GlobalDecl.CallType;
import model.GlobalDeclList;
import model.Var;
import model.Var.VarMod;
import model.VarArray;
import model.VarList;

public class DeclReader {
	public static final String NameDeclarationText = "Name";
	public static final String TypeDeclarationText = "Type";
	public static final String ParameterDeclarationText = "Parameter";
	public static final String InterfaceDeclarationText = "Interface";
	public static final String InDeclarationText = "In";
	public static final String InOutDeclarationText = "InOut";

	public GlobalDeclList xmlToDecls(IXMLElement declXml) {
		GlobalDeclList decls = new GlobalDeclList();
		if (declXml == null)
			return decls;
		
		for (IXMLElement child : getChildren(declXml)) {
			GlobalDecl d = convertXmlToDec(child);
			if (d != null)
				decls.add(d);
		}
		return decls;
	}

	private GlobalDecl convertXmlToDec(IXMLElement xml) {
        String tag = xml.getName();
		if (xml.isLeaf()) {
            System.out.println("Reader error: Empty " + tag + " declaration");
			return null;
        }

        switch (tag) {
		case "CommandDeclaration":
            return parseCommandDec(xml);

        case "StateDeclaration":
            return parseStateDec(xml);
            
        case "LibraryNodeDeclaration":
            return parseLibraryDec(xml);

        default:
            System.out.println("Reader error: unknown declaration type " + tag);
            return null;
        }
	}

	private GlobalDecl parseCommandDec(IXMLElement xml) {
		String id = null;
		VarList params = new VarList();
		VarList rets = new VarList();
		for (IXMLElement child : getChildren(xml)) {
			String text = child.getName();
            switch (text) {
            case "Parameter":
                params.add(convertXmlToParam(child));
                break;
                
            case "Return":
                rets.add(convertXmlToParam(child));
                break;

            case "Name":
                id = child.getContent();
                break;

            case "ResourceList":
                // TODO
                break;

            default:
                System.out.println("Reader error: illegal element " + text
                                   + " in " + xml.getName());
                break;
            }
		}

        // check that we have a name
        if (id == null) {
            System.out.println("DeclReader.convertXmlToLibraryDec: required Name element missing in "
                               + xml.getName() + " declaration");
            return null;
        }

		return new GlobalDecl(CallType.Command, id, params, rets);
    }

	private GlobalDecl parseStateDec(IXMLElement xml) {
		String id = null;
		VarList params = new VarList();
		VarList rets = new VarList();
		for (IXMLElement child : getChildren(xml)) {
			String text = child.getName();
            switch (text) {
            case "Parameter":
                params.add(convertXmlToParam(child));
                break;
                
            case "Return":
                rets.add(convertXmlToParam(child));
                break;

            case "Name":
                id = child.getContent();
                break;

            default:
                System.out.println("Reader error: illegal element " + text
                                   + " in " + xml.getName());
                break;
            }
		}

        // check that we have a name
        if (id == null) {
            System.out.println("DeclReader.convertXmlToLibraryDec: required Name element missing in "
                               + xml.getName() + " declaration");
            return null;
        }

		return new GlobalDecl(CallType.Lookup, id, params, rets);
    }

	private GlobalDecl parseLibraryDec(IXMLElement xml) {
		String id = null;
		VarList params = new VarList();
		for (IXMLElement child : getChildren(xml)) {
			String text = child.getName();
            switch (text) {
            case "Name":
                id = child.getContent();
                break;
                
            case "Interface":
                params.addAll(convertXmlToInterfaceVars(child));
                break;

            default:
                System.out.println("Reader error: illegal element " + text
                                   + " in " + xml.getName());
                break;
            }
		}

        // check that we have a name
        if (id == null) {
            System.out.println("Reader error: required Name element missing in "
                               + xml.getName());
            return null;
        }

        return new GlobalDecl(CallType.LibraryCall, id, params, null);
    }
	
	private Var convertXmlToParam(IXMLElement xml) {
		if (xml == null)
			return null;
        String typename = null;
        String name = null;
        String sizeSpec = null;
        for (IXMLElement child : getChildren(xml)) {
            switch (child.getName()) {
            case "Name":
                name = child.getContent();
                break;

            case "Type":
                typename = child.getContent();
                break;

            case "MaxSize": // Signifies an array
                sizeSpec = child.getContent();
                break;

            default:
                System.out.println("Reader error: Unexpected element " + child.getName()
                                   + " in parameter declaration");
                return null;
            }
        }

		if (typename == null) {
            System.out.println("Reader error: required Type element missing in parameter declaration");
            return null;
		}

		ExprType type = ExprType.typeForName(typename);
        if (type == null) {
            System.out.println("Reader error: invalid type name " + typename
                               + " in parameter declaration");
            return null;
        }

        if (sizeSpec != null) {
            // This is an array declaration
            int maxSize = -1;
            try {
                maxSize = Integer.parseUnsignedInt(sizeSpec);
            }
            catch (NumberFormatException e) {
                System.out.println("Reader error: MaxSize " + sizeSpec
                                   + " is not an unsigned integer in parameter declaration");
                return null;
            }

            if (name != null)
                return new VarArray(name, type, maxSize);
            else
                return new VarArray(type, maxSize);
        }
        else if (name != null)
            return new Var(name, type);
        else
            return new Var(type);
	}
	
	public IXMLElement findXmlByTerm(IXMLElement xml, String term)
	{
		if (xml == null)
			return null;
		
		if (xml.getName().equals(term))
			return xml;
		
		for (IXMLElement child : getChildren(xml))
		{
			IXMLElement result = findXmlByTerm(child, term);
			if (result != null)
				return result;
		}
		return null;
	}
	
	private VarList convertXmlToInterfaceVars(IXMLElement xml)
	{
		if (xml == null)
			return null;
		return XmlReader.getPlanReader().buildInterfaceList(xml);
	}
	
	// IXMLElement.getChildren() only returns Vector. Having a typed vector is nicer, so we type cast here.
	@SuppressWarnings("unchecked")
	private Vector<IXMLElement> getChildren(IXMLElement elem)
	{
		Vector<IXMLElement> children = elem.getChildren();
		return children;
	}

}

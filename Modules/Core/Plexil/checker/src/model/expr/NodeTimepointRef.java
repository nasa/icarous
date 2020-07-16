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


import java.util.Vector;

import main.Log;
import model.expr.NodeVarRef.NodeRefDir;
import model.GlobalDeclList;
import model.Node;

public class NodeTimepointRef
    extends NodeVarRef {

    // We don't have to do anything with these, yet.
    // Assume parser takes care of them.
    private String stateName;
    private String startOrEnd;

    public NodeTimepointRef(String nodeId, NodeRefDir dir, String state, String which) {
        super(ExprType.NodeTimepointValue, nodeId, dir);
        stateName = state;
        startOrEnd = which;
    }

    @Override
    public String toString() {
        StringBuilder s = new StringBuilder("(");
        s.append(typeAsExprName());
        s.append(" ");
        s.append(nodeRefToString());
        s.append(" ");
        s.append(stateName);
        s.append(" ");
        s.append(startOrEnd);
        s.append(")");
        return s.toString();
    }

}

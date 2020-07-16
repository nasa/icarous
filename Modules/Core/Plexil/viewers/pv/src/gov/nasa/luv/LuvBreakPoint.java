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

/** 
 * The LuvBreakPoint class is an abstract class that provides methods for 
 * breaking action in the event that a breakpoint is signaled.
 */

import static gov.nasa.luv.PlexilSchema.NodeState;
import static gov.nasa.luv.PlexilSchema.NodeOutcome;
import static gov.nasa.luv.PlexilSchema.NodeFailureType;

public class LuvBreakPoint
    extends Node.ChangeAdapter {

    private Node.StateTransitionFilter filter;

    /** enabled state of this breakpoint jsdhcb */
    private boolean enabled = true;

    // The following are protected for the convenience of derived classes.

    /** the node on which the break point operates */
    protected Node node;
    
    /** Old value, use to test for changes. */
    protected String oldValue = "";

    /** 
     * Constructs a Luv specific break point with the specified Plexil node.
     *
     * @param node the node on which the break point operates
     */
    public LuvBreakPoint(Node node, Node.StateTransitionFilter f) {
        filter = f;
        this.node = node;
        node.addBreakPoint(this);
    }

    /** {@inheritDoc} */
      
    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    /** {@inheritDoc} */

    public boolean isEnabled() {
        return enabled;
    }

    public String toString() {
        return node.getNodeName() + filter.getDescription();
    }

    /** detatch from node */
    // N.B. this is initiated from GUI on node
    public void unregister() {
        if (node != null)
            node.removeBreakPoint(this);
        node = null;
    }

    /** {@inheritDoc} */

    public Node getNode() {
        return node;
    }

    /** {@inheritDoc} */
    public void onBreak() {
        ExecutionHandler.instance().breakpointReached(this);
        node.setHighlight(true);
        PlanView view = Luv.getLuv().getViewForNode(node);
        if (view == null)
            return;
        view.setVisible(true);
        view.toFront();
        view.repaint(); // redundant?
    }

    public void onResume() {
        node.setHighlight(false);
    }

    //
    // Node.ChangeListener API
    //

    public void stateTransition(Node node,
                                NodeState newState,
                                NodeOutcome newOutcome,
                                NodeFailureType newFailure) {
        if (!enabled)
            return;
        if (filter.eventMatches(node, newState, newOutcome, newFailure))
            onBreak();
    }

}

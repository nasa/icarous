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

import java.util.EnumMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.Vector;

import org.xml.sax.Attributes;

import static gov.nasa.luv.PlexilSchema.*;
import static gov.nasa.luv.Constants.UNKNOWN;

/**
 * The NodeStateUpdateHandler class provides mothods for handling when the 
 * Plexil Plan is executing and sending updates to properties of the Plexil Model.
 */

public class NodeStateUpdateHandler
    extends AbstractDispatchableHandler {

    //* Top level XML tag name registered with DispatchHandler
    public static final String NODE_STATE_UPDATE = "NodeStateUpdate";

    // XML tags
    private static final String NODE_ID           = "NodeId";
    private static final String NODE_STATE        = "NodeState";
    private static final String NODE_OUTCOME      = "NodeOutcome";
    private static final String NODE_FAILURE_TYPE = "NodeFailureType";

    private Vector<String> path;
    private NodeState state;
    private NodeOutcome outcome;
    private NodeFailureType failureType;
    private Map<Condition, String> conditions;

    /**
     * Constructs a NodeStateUpdateHandler.
     */
    public NodeStateUpdateHandler() {
        super();

        // working state
        path = new Vector<String>();
        conditions = new EnumMap<Condition, String>(Condition.EndCondition.getDeclaringClass());

        setElementMap(new TreeMap<String, LuvElementHandler>() {
                {
                    put(NODE_ID, new LuvElementHandler() {
                            public void elementEnd(String tagName, String tweenerText) {
                                path.add(tweenerText);
                            }
                        });
                    put(NODE_STATE, new LuvElementHandler() {
                            public void elementEnd(String tagName, String tweenerText) {
                                state = NodeState.valueOf(tweenerText);
                            }
                        });
                    put(NODE_OUTCOME, new LuvElementHandler() {
                            public void elementEnd(String tagName, String tweenerText) {
                                outcome = NodeOutcome.valueOf(tweenerText);
                            }
                        });
                    put(NODE_FAILURE_TYPE, new LuvElementHandler() {
                            public void elementEnd(String tagName, String tweenerText) {
                                failureType = NodeFailureType.valueOf(tweenerText);
                            }
                        });
                    put(NODE_STATE_UPDATE, new LuvElementHandler() {
                            public void elementStart(String tagName, Attributes attributes) {
                                path.clear();
                                state = null;
                                outcome = null;
                                failureType = null;
                                conditions.clear();
                            }
                            public void elementEnd(String tagName, String tweenerText) {
                                if (path.isEmpty() || state == null)
                                    return;

                                PlanView view = Luv.getLuv().getPlanView(path.get(0));
                                if (view == null)
                                    return; // plan not currently displayed
                                view.nodeStateEvent(path, state, outcome, failureType, conditions);
                            }
                        });

                    LuvElementHandler conditionHandler = new LuvElementHandler() {
                            public void elementEnd(String tagName, String tweenerText) {
                                conditions.put(Condition.valueOf(tagName), tweenerText);
                            }
                        };
                    for (Condition c: Condition.values())
                        put(c.toString(), conditionHandler);
                }
            });
    }

    /**
     * Handles the end of the state update document.
     */
    public void endDocument() {
    }
}

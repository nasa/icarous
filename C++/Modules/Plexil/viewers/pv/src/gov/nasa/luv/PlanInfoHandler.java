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
 * The PlanInfoHandler class handles the Plexil Plan Info section within a XML Plexil Plan file.
 */

public class PlanInfoHandler extends AbstractDispatchableHandler
{
    //* Top level XML tag name registered with DispatchHandler
    public static final String PLAN_INFO     = "PlanInfo";
    
    private static final String VIEWER_BLOCKS = "ViewerBlocks";

    /** Constructs a PlanInfoHandler. */
    public PlanInfoHandler()
    {
        super();
        setElementMap(new java.util.TreeMap<String, LuvElementHandler>() {
                {
                    put(VIEWER_BLOCKS, new LuvElementHandler() {
                            public void elementEnd(String tagName, String tweenerText) {
                                boolean allowBreaks = Boolean.valueOf(tweenerText);
                                // Only set if different from current
                                if (allowBreaks != Settings.instance().blocksExec())
                                    Luv.getLuv().setBreaksAllowed(allowBreaks);
                            }
                        });
                }
            });
    }

}

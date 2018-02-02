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

import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import java.util.Map;
import java.util.TreeMap;

/**
 * @class DispatchHandler
 * @brief The DispatchHandler class dispatches XML messages to the correct handler from 
 * the set of provided handlers in the handler map. It is used to process the data stream
 * from the PLEXIL Exec.
 */

public class DispatchHandler
    extends DefaultHandler {

    //
    // Local constants
    //

    /** table of messages handlers */
    static Map<String, AbstractDispatchableHandler> handlerMap = 
        new TreeMap<String, AbstractDispatchableHandler>();
      
    /** The currently selected handler */
    AbstractDispatchableHandler currentHandler;
            
    /** 
     * Constructs a dispatch handler which operates on the provided
     * model.
     */
      
    public DispatchHandler() {
        // add each type of handler to the table of possible handlers
        registerHandler(PlanInfoHandler.PLAN_INFO, new PlanInfoHandler());
        registerHandler(NodeStateUpdateHandler.NODE_STATE_UPDATE, new NodeStateUpdateHandler());
        registerHandler(AssignmentHandler.ASSIGNMENT, new AssignmentHandler());
        
        AbstractDispatchableHandler planHandler =
            new PlexilPlanHandler(new PlexilPlanHandler.PlanReceiver() {
                    public void newPlan(Plan p) {
                        try {
                            javax.swing.SwingUtilities.invokeAndWait(new Runnable() {
                                    public void run() {
                                        Luv.getLuv().newPlanFromExec(p);
                                    }
                                });
                        }
                        catch (InterruptedException i) {
                        }
                        catch (Exception e) {
                            StatusMessageHandler.instance().displayErrorMessage(e, "While handling new plan from Exec");
                        }
                    }

                    public void newLibrary(Plan p) {
                        RootModel.libraryLoaded(p);
                    }
                }
                );
        registerHandler(PlexilSchema.PLEXIL_PLAN, planHandler);
        registerHandler(PlexilPlanHandler.PLEXIL_LIBRARY, planHandler);

        currentHandler = null;
    }

    /** 
     * Adds a new handler to the set of available handlers to dispatch.
     *
     * @param key the tag for which this handler is invoked
     * @param handler handler to add to the set available handers to dispatch.
     */

    public static void registerHandler(String key,
                                       AbstractDispatchableHandler handler) {
        handlerMap.put(key, handler);
    }

    /** Handles the start of a document event. */

    public void startDocument() {
        // at the start of the document ensure that handler is nulled out
        currentHandler = null;
    }

    /** 
     * Establishes the current handler then dispatch events to that handler. 
     */

    public void startElement(String uri, String localName, 
                             String qName, Attributes attributes) 
        throws SAXException {
        // if no handler has been identified, find one now!
        if (currentHandler == null) {
            currentHandler = handlerMap.get(localName);
            if (currentHandler == null) {
                StatusMessageHandler.instance().displayErrorMessage(null, "ERROR: unhandled XML tag: <" + localName + ">");
                throw(new Error("ERROR: unhandled XML tag: <" + localName + ">."));
            }
            
            // signal start of document (because we didn't know who to
            // send the event to before)
            currentHandler.startDocument();

        }
         
        // using the current handler dispatch the startElement action
        currentHandler.startElement(uri, localName, qName, attributes);
    }

    /** Handles escapes in character data. */
      
    @SuppressWarnings("static-access")
    public void characters(char[] ch, int start, int length)
        throws SAXException {
        String test = "";
        if (ch.length > 1) {
            if (start + length < ch.length) {
                test = test.valueOf(ch, start + length, 1);
                if (test.equals("&")) {
                    test = test.valueOf(ch, start + length, 6);
                    if (test.equals("&#x1d;") || test.equals("&#x1D;") || test.equals("&#x04;")) {
                        ch[start + length] = '&';
                        ch[start + length + 1] = 'a';
                        ch[start + length + 2] = 'm';
                        ch[start + length + 3] = 'p';
                        ch[start + length + 4] = ';';
                        ch[start + length + 5] = ch[start + length - 1];
                    }
                    else {
                        test = test.valueOf(ch, start + length, 5);
                        if (test.equals("&#x4;") || test.equals("&#x0;")) {
                            ch[start + length] = '&';
                            ch[start + length + 1] = 'a';
                            ch[start + length + 2] = 'm';
                            ch[start + length + 3] = 'p';
                            ch[start + length + 4] = ';';
                        }
                    }
                }
            }
        }
        else if (ch[0] == '&')
            ch[0] = ' ';
          
        currentHandler.characters(ch, start, length);
    }
      
    /** Dispatches the endElement action. */
      
    public void endElement(String uri, String localName, String qName)
        throws SAXException {
        currentHandler.endElement(uri, localName, qName);
    }

    /** Dispatches end of document event. */

    public void endDocument()
        throws SAXException {
        currentHandler.endDocument();
        // at the end of the document ensure that handler is nulled out
        // (belt & suspenders)
        currentHandler = null;
    }
}

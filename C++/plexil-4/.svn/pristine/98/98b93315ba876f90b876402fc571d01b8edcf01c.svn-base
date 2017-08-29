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

import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;

import java.util.HashMap;
import java.util.Map;

/**
 * The AbstractDispatchableHandler class is an abstract class where Dispatchable 
 * XML parsers will be derived. 
 */

public abstract class AbstractDispatchableHandler extends DefaultHandler
{
    //
    // Facilities for use by derived classes
    //

    //* Base class for handling a particular element, or class of elements
    protected class LuvElementHandler {
        public void elementStart(String tagName, Attributes attributes) {
        }
        public void elementEnd(String tagName, String tweenerText) {
        }
    }

    private Map<String, LuvElementHandler> elementMap;

    protected Map<String, LuvElementHandler> getElementMap() {
        return elementMap;
    }

    protected void setElementMap(Map<String, LuvElementHandler> newMap) {
        elementMap = newMap;
    }

    protected LuvElementHandler getElementHandler(String tag) {
        return elementMap.get(tag);
    }

    protected void setElementHandler(String tag, LuvElementHandler handler) {
        elementMap.put(tag, handler);
    }

    // Tracking where we are in tree
    protected StringStack tagStack;

    /** buffer which holds text between xml tags */
    private StringBuilder tweenerBuffer;
      
    /**
     * Constructs an AbstractDispatchableHandler by calling the parent 
     * DefaultHandler class default constructor. 
     */

    public AbstractDispatchableHandler() {
        super();
        tagStack = new StringStack();
        tweenerBuffer = new StringBuilder();
    }

    /** Handles data between XML element tags.
     *
     * @param ch character buffer
     * @param start index of start of data characters in buffer
     * @param length number of data characters in buffer
     */

    public void characters(char[] ch, int start, int length) {
        if (length > 0) {
            if (tweenerBuffer == null)
                tweenerBuffer = new StringBuilder();
            tweenerBuffer.append(ch, start, length);
        }
    }
      
    /** 
     * Collects text between XML element tags and trims any leading or 
     * trailing white space.
     * 
     * This is a destructive action and clears the buffer which holds
     * the text. Calling it again immediatly will always return null.
     *
     * @return the collected text or null if no such text exists
     */

    public String getTweenerText() {
        if (tweenerBuffer == null)
            return null;
          
        // if there is some text between tags (after trimming leading
        // and trailing white space), record that
        String text = tweenerBuffer.toString().trim();
        if (text.isEmpty())
            text = null;
        tweenerBuffer = null;
        return text;
    }

    /**
     * Handles the start of an XML element.
     * 
     * @param uri
     * @param tagName
     * @param qName
     * @param attributes
     *
     * @note This is a default method and can be overridden.
     */
    public void startElement(String uri, String tagName, String qName, Attributes attributes) {    
        try {
            LuvElementHandler handler = getElementHandler(tagName);
            if (handler != null)
                handler.elementStart(tagName, attributes);

            // Save context
            tagStack.push(tagName);
        }
        catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: Exception in startElement for " + tagName);
            tagStack.dump(System.err);
        }
    }

    /**
     * Handles the end of an XML element.
     * 
     * @param uri
     * @param tagName
     * @param qName
     *
     * @note This is a default method and can be overridden.
     */
    public void endElement(String uri, String tagName, String qName) {
        try {
            tagStack.pop();
            LuvElementHandler handler = getElementHandler(tagName);
            if (handler != null)
                handler.elementEnd(tagName, getTweenerText());
        }
        catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(e, "ERROR: Exception in endElement for " + tagName);
            tagStack.dump(System.err);
        }
    }

}

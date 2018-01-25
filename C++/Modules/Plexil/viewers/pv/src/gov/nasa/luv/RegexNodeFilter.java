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

import java.util.Vector;

import static gov.nasa.luv.Constants.*;
import static gov.nasa.luv.PlexilSchema.*;

/** 
 * The RegexNodeFilter class provides methods to fileter a Plexil Node.
 */

public class RegexNodeFilter extends AbstractNodeFilter
{
    // list of elements to filter from view    
    private Vector<String> regexList;

    /**
     * Constructs a RegexNodeFilter with the specified value to 
     * enable or disable the filtering.
     * 
     * @param enabled the value that either enables or disables the filtering
     */
    public RegexNodeFilter(boolean enabled) {
        super(enabled);
        regexList = Settings.instance().getStringList(PROP_HIDE_SHOW_LIST);
        if (regexList == null)
            regexList = new Vector<String>();
    }

    /** {@inheritDoc} */
    public boolean isFiltered(Node node)
    {
        String type = node.getProperty(NODETYPE_ATTR, UNKNOWN);
        String value = node.getNodeName();
        if (value == null && type.equals(UNKNOWN))
            return false;

        if ("HIDE".equals(Settings.instance().getProperty(type)))
            return true;
            
        for (String regex : regexList)
            if (value.matches(regex))
                return true;

        return false;
    }
    
    /**
     * Adds the specified element to filter the Plexil Node with.
     * 
     * @param regex the new element to be filtered
     */
    public void addRegex(String regex) {
        regexList.add(formatRegex(regex));
        Settings.instance().setStringList(PROP_HIDE_SHOW_LIST, regexList);
        // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***  
    }
    
    /**
     * Removes the specified filtering element from the Plexil Node.
     * @param regex the element to be removed
     */
    public void removeRegex(String regex)
    {
        regexList.remove(formatRegex(regex));
        Settings.instance().setStringList(PROP_HIDE_SHOW_LIST, regexList);
        // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***  
    }
    
    /**
     * Updates the list of elements that filter the Node, which occurs when
     * the Luv application is opened and looks to the saved list of regex
     * in the Luv properties file.
     */
    public void updateRegexList() {
        String namelist = Settings.instance().get(PROP_HIDE_SHOW_LIST);
        if (namelist != null && !namelist.isEmpty()) {
            String [] array = namelist.split(", ");
            for (int i = 0; i < array.length; i++)
                regexList.add(formatRegex(array[i]));
        }
    }

    public void extendedPlexilView() {
        Settings.instance().setProperty(AUX, "HIDE");
        Settings.instance().setProperty(AUX_THEN, "HIDE");
        Settings.instance().setProperty(AUX_ELSE, "HIDE");
    }

    public void corePlexilView() {
        Settings.instance().setProperty(AUX, "SHOW");
        Settings.instance().setProperty(AUX_THEN, "SHOW");
        Settings.instance().setProperty(AUX_ELSE, "SHOW");
    }
    
    private String formatRegex(String regex) {  
        StringBuilder formattedRegex = new StringBuilder();

        //regex* --> ^regex.*
        if (regex.charAt(0) != '*')
            formattedRegex.append('^');

        // Replace '*' with ".*" throughout
        int i = 0;
        do {
            int end = regex.indexOf('*', i);
            if (end < 0)
                break;
            formattedRegex.append(regex, i, end-1);
            formattedRegex.append(".*");
            i = end + 1;
        } while (i < regex.length());

        // put remainder of regex (if any) into buffer
        formattedRegex.append(regex, i, regex.length());

        return formattedRegex.toString();
    }
}


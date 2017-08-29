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
 *
/

/* Copyright (c) 1995 - 2008 Sun Microsystems, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Sun Microsystems nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// TODO:
// - Change base class to JFrame
// - Implement search support on PlanView
// - Get rid of static variables (except singleton instance var)

package gov.nasa.luv;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.Stack;
import javax.swing.*;
import static gov.nasa.luv.Constants.*;

/** 
 * The FindWindow class is an interface for the user to search for nodes within
 * the tree that match, by name, the complete or partial input string. 
 */

public class FindWindow
    extends JPanel
    implements KeyListener {
    private static JFrame       frame;  
    private static JComboBox    searchListHolder;
    private static String []    searchList;     
    private JPanel              leftHalf;
    private Box                 entryPanel;    
    private JLabel              message_to_user; 
    private Font                regularFont, italicFont;  
    private JTextField          searchListEditor;           
    private boolean             searchSet;
    private boolean             foundMatch;              
    private ArrayList<Stack>    foundNodes;
    private String              previousSearch;
    private int                 next;
    
    /** 
     * Constructs a FindWindow with the specified list of previous searches. 
     *
     * @param list the list of previous search strings and can be empty
     */

    public FindWindow(String list) 
    {
        searchSet = false;
        foundMatch = false;
        foundNodes = new ArrayList<Stack>();
        previousSearch = "";
        next = 0;
        regularFont = new Font("Dialog", Font.PLAIN, 12);
        italicFont = regularFont.deriveFont(Font.ITALIC);
    
        setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));

        add(createEntryField(list));
        add(createMessageDisplay());     
    }
    
    private Component createEntryField(String list) {
        setSearchList(list);
        
        leftHalf = new JPanel() {
                public Dimension getMaximumSize() {
                    Dimension pref = getPreferredSize();
                    return new Dimension(Integer.MAX_VALUE, pref.height);
                }
            };
        
        leftHalf.setLayout(new BoxLayout(leftHalf, BoxLayout.Y_AXIS));
     
        if (searchList != null)
            searchListHolder = new JComboBox<String>(searchList);
        else
            searchListHolder = new JComboBox<String>();
        searchListHolder.setEditable(true);  
        searchListHolder.setFont(searchListHolder.getFont().deriveFont(Font.PLAIN, 12.0f));
        
        searchListEditor = (JTextField) searchListHolder.getEditor().getEditorComponent();
        searchListEditor.addKeyListener(this);        
        searchListEditor.setFont(searchListEditor.getFont().deriveFont(Font.PLAIN, 12.0f));
        
        entryPanel = Box.createVerticalBox(); 
        entryPanel.add(searchListHolder);                
        entryPanel.setBorder(BorderFactory.createEmptyBorder(0, 10, 0, 10));
        
        leftHalf.add(entryPanel);
        
        return leftHalf;
    }
    
    private void setSearchList(String list) {
        if (list != null && !list.isEmpty()) {
            String[] getList;
            getList = list.split(", ");
            searchList = new String[getList.length + 1];
            for (int i = 0; i < getList.length; i++)
                searchList[i] = getList[i];
        }      
    }
    
    private JComponent createMessageDisplay() {
        JPanel panel = new JPanel(new BorderLayout());
        
        message_to_user = new JLabel();
        message_to_user.setHorizontalAlignment(JLabel.CENTER);
        
        message_to_user.setText(getMessageToUser());
        message_to_user.setFont(italicFont);
        message_to_user.setForeground(Color.gray);

        panel.setBorder(BorderFactory.createEmptyBorder(5, 0, 5, 0));
        panel.add(new JSeparator(JSeparator.VERTICAL),  BorderLayout.LINE_START);
        panel.add(message_to_user, BorderLayout.CENTER);
        panel.setPreferredSize(new Dimension(300, 50));

        return panel;
    }
    
    private String getMessageToUser() {
        if (!searchSet)
            return "No search set";

        StringBuilder sb = new StringBuilder();
        sb.append("<html><p align=left>");

        String name = (String) searchListHolder.getSelectedItem(); 
        if (!foundMatch) {
            sb.append("No matching nodes were found for <b>");
            sb.append(name);
            sb.append("</b>");
        }   
        else {
            sb.append("<b>" + foundNodes.size() + "</b>");
            if (foundNodes.size() > 1) {
                sb.append(" matches found for <b>");
                sb.append(name);
                sb.append("</b>");
                int match = next + 1;
                sb.append("<br>Match: ");
                sb.append(Integer.toString(match));
                sb.append("</br>");
            }
            else {
                sb.append(" match found for <b>");
                sb.append(name);
                sb.append("</b>");
            }         
        }

        sb.append("</p></html>");

        return sb.toString();
    }
    
    private void lookForNode() {
        String text = searchListEditor.getText(); 

        if (!text.equals(previousSearch))
            newSearch(text);
        
        if (foundMatch)
            {
                showUserNextNode();
                message_to_user.setFont(regularFont);
            }
        else
            {
                message_to_user.setText(getMessageToUser());
                message_to_user.setFont(italicFont);
            }  
    }
    
    private void newSearch(String text) {
        String search = text;
        boolean startsWith = false;
        boolean endsWith = false;
        boolean both = false;
                
        if (text.contains("*"))
            {
                if (text.startsWith("*") && text.endsWith("*"))
                    {
                        both = true;
                        search = text.substring(1, text.length() - 1);
                    }
                else if (text.startsWith("*"))
                    {              
                        endsWith = true;
                        search = text.substring(1, text.length());
                    }
                else if (text.endsWith("*"))
                    {
                        startsWith = true;
                        search = text.substring(0, text.length() - 1);
                    }
            }
        
        saveSearchWord(text);
        leftHalf.remove(entryPanel);
        leftHalf.add(createEntryField(Settings.instance().get(PROP_SEARCH_LIST)));
        foundNodes.clear();
        next = 0;
        // *** FIXME ***
        // TreeTableView.getCurrent().restartSearch();
        previousSearch = text;  
        foundMatch = false;
     
        // *** FIXME ***
        // findMatchInNode(Luv.getLuv().getCurrentPlan().getRootNode(),
        //                 search, both, startsWith, endsWith);
    }
    
    private void saveSearchWord(String searchWord)
    {
        String list = Settings.instance().get(PROP_SEARCH_LIST);
        
        if (list != null) {
            String [] array = list.split(", ");
            list = searchWord;
            for (int i = 0; i < array.length; i++) {
                if (i < 10 && !array[i].equals(searchWord) && !array[i].equals(""))
                    list += ", " + array[i];
            }
        }
        else
            list = searchWord;
        
        Settings.instance().set(PROP_SEARCH_LIST, list);
    }
    
    private void showUserNextNode()
    {
        if (next >= foundNodes.size()) {
                next = 0;
                // *** FIXME ***
                //TreeTableView.getCurrent().restartSearch();
            }

        Stack<String> node_path = new Stack<String>();
        
        Object[] obj = foundNodes.get(next).toArray();

        for (int i = 0; i < obj.length; i++)
            node_path.push((String) obj[i]);
     
        // *** FIXME ***
        // TreeTableView.getCurrent().showNode(node_path);        
        message_to_user.setText(getMessageToUser());
        next++;
    }
    
    private void findMatchInNode(Node node, String search, boolean both, boolean startsWith, boolean endsWith) {
        if (node.isRoot() || node.getParent().isRoot()) {
            if ((both        && node.getNodeName().contains(search))     ||
                (startsWith  && node.getNodeName().startsWith(search))   ||
                (endsWith    && node.getNodeName().endsWith(search))     ||
                (node.getNodeName().equals(search))) {
                Stack<String> node_path = node.pathToNode(node); 
                foundMatch = true;
                foundNodes.add(node_path);
            }
        }
            
        if (node.hasChildren())
            for (Node child: node.getChildren()) {
                if (!AbstractNodeFilter.isNodeFiltered(child)) {
                    if ((both        && child.getNodeName().contains(search))     ||
                        (startsWith  && child.getNodeName().startsWith(search))   ||
                        (endsWith    && child.getNodeName().endsWith(search))     ||
                        (child.getNodeName().equals(search))) {
                        Stack<String> node_path = child.pathToNode(child);    
                        foundMatch = true;
                        foundNodes.add(node_path);                    
                    }
                }
            
                findMatchInNode(child, search, both, startsWith, endsWith);       
            }
    }
    
    /** {@inheritDoc} */
    
    public void keyTyped(KeyEvent e) {
    }
    
    /** {@inheritDoc} */

    public void keyPressed(KeyEvent e) {
        if (e.getKeyCode() == (KeyEvent.VK_ENTER) && !searchListEditor.getText().equals("")) {
            searchSet = true; 
            lookForNode();
            searchListHolder.requestFocusInWindow();
            searchListEditor.selectAll();
        }
    }
    
    /** {@inheritDoc} */

    public void keyReleased(KeyEvent e) {
    }
    
    /** 
     * Creates an instance of a FindWindow.  
     *
     *  @param list the list of previous search strings
     */
    
    public static void open(String list) {
        if (frame != null && frame.isVisible())
            frame.setVisible(false);
        frame = new JFrame("Find Node");
        
        String name = null; // Luv.getLuv().getCurrentPlan().getName(); // *** FIXME ***
        if (name != null)
            frame.setTitle("Find Node in " + name);
     
        frame.add(new FindWindow(list));
        
        frame.setPreferredSize(Settings.instance().getDimension(PROP_FINDWIN_SIZE));
        frame.setLocation(Settings.instance().getPoint(PROP_FINDWIN_LOC));
        
        frame.pack();
        
        if (searchList != null)
            searchListHolder.setSelectedIndex(searchList.length - 1);
        
        frame.setVisible(true);  
    }
}

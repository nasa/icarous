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

import java.awt.*;
import java.awt.event.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.*;

import static gov.nasa.luv.Constants.*;
import static gov.nasa.luv.PlexilSchema.*;


/** 
 * The HideOrShowWindow class is an interface for the user to identify the nodes 
 * to hide, by name or node type.
 */

public class HideOrShowWindow extends JPanel implements ListSelectionListener
{
    private JFrame frame;
    private JPanel instructionsPane;
    private JScrollPane listScrollPane;
    private Box checkBoxList;
    private Box buttonPane;
    private JList list;
    private DefaultListModel<String> listModel; 
    private JTextField textField;
    private JButton showButton;          
    
    /**
     * Constructs a HideOrShowWindow with the specified list of saved elements
     * that are currently hidden.
     * 
     * @param regexList the list of currently hidden elements and clan be empty
     */
    public HideOrShowWindow(List<String> regexList) {
        super(new BorderLayout());
        init(regexList);
    }

    private void init(List<String> regexList) {
        createIntructionSection();
        createScrollListSection(regexList);
        createCheckBoxList();
        createHideShowButtonSection();
    }
    
    private void createIntructionSection() {
        String instructionText = "<html>Type the full or partial name of the nodes you want to hide." +
            "<br>Use (<b>*</b>) wildcard as a prefix and/or suffix to select multiple nodes." +
            "<br>" + 
            "<br>For example:" +
            "<br>Type <b>Child</b> to hide node <b>Child</b> only" +
            "<br>Type <b>Child*</b> to hide nodes Child<b>1</b>, Child<b>2</b>, and Child<b>3</b>" +
            "<br>Type <b>*Child</b> to hide nodes <b>a</b>Child, <b>b</b>Child, and <b>c</b>Child" +
            "<br>Type <b>*Child*</b> to hide nodes <b>a</b>Child<b>1</b>, <b>b</b>Child<b>2</b>, and <b>c</b>Child<b>3</b>";
        
        JLabel instructions = new JLabel(instructionText);
        instructions.setFont(new Font("Monospaced", Font.PLAIN, 12));
        instructionsPane = new JPanel();
        instructionsPane.add(instructions);
        add(instructionsPane, BorderLayout.NORTH);
    }
    
    private void createScrollListSection(List<String> regexList) {
        listModel = new DefaultListModel<String>();
        if (regexList != null)
            for (String s : regexList)
                listModel.addElement(s);
        list = new JList<String>(listModel);
        list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        list.setSelectedIndex(0);
        list.addListSelectionListener(this);
        list.setVisibleRowCount(5);   
        listScrollPane = new JScrollPane(list);  
        listScrollPane.setPreferredSize(new Dimension(300, 80));
        add(listScrollPane, BorderLayout.WEST);
    }
    
    private void createHideShowButtonSection()
    {
        JButton hideButton = new JButton("Hide");
        HideListener hideListener = new HideListener(hideButton);
        hideButton.setActionCommand("Hide");
        hideButton.addActionListener(hideListener);
        hideButton.setEnabled(false);      

        showButton = new JButton("Show");
        showButton.setActionCommand("Show");
        showButton.addActionListener(new ShowListener());       
        if (listModel.isEmpty())
            showButton.setEnabled(false);      
        else
            showButton.setEnabled(true);

        textField = new JTextField("[Type node name here]", 10);
        textField.setForeground(Color.lightGray);
        textField.addActionListener(hideListener);         
        textField.addFocusListener(new java.awt.event.FocusAdapter() {
                public void focusGained(java.awt.event.FocusEvent evt) {
                    if (textField.getText().equals("[Type node name here]"))
                        textField.setText("");
                }
            });        
        textField.getDocument().addDocumentListener(hideListener);  
        
        buttonPane = Box.createHorizontalBox();
        buttonPane.add(Box.createHorizontalGlue());
        buttonPane.add(showButton);
        buttonPane.add(Box.createHorizontalStrut(5));
        buttonPane.add(new JSeparator(SwingConstants.VERTICAL));
        buttonPane.add(Box.createHorizontalStrut(5));
        buttonPane.add(textField);
        buttonPane.add(hideButton);

        add(buttonPane, BorderLayout.SOUTH);
    }
    
    private void createCheckBoxList()
    {
        checkBoxList = Box.createHorizontalBox();
        
        JPanel iconSide = new JPanel();
        iconSide.setLayout(new GridLayout(16,1));
        
        JPanel checkBoxSide = new JPanel();
        checkBoxSide.setLayout(new GridLayout(16,1));
        
        Font checkBoxFont = new Font("Monospaced", Font.PLAIN, 12);

        final JCheckBox emptyBox = new JCheckBox("Show " + EMPTY + " Nodes", isBoxChecked(EMPTY)); 
        emptyBox.setFont(checkBoxFont);
        emptyBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setEmptyProperties(emptyBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(emptyBox);
        iconSide.add(new JLabel(getIcon(EMPTY), JLabel.LEFT));

        final JCheckBox assnBox = new JCheckBox("Show " + ASSIGNMENT + " Nodes", isBoxChecked(ASSIGNMENT));
        assnBox.setFont(checkBoxFont);
        assnBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setAssignmentProperties(assnBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(assnBox); 
        iconSide.add(new JLabel(getIcon(ASSIGNMENT), JLabel.LEFT));

        final JCheckBox cmdBox = new JCheckBox("Show " + COMMAND + " Nodes", isBoxChecked(COMMAND)); 
        cmdBox.setFont(checkBoxFont);
        cmdBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setCommandProperties(cmdBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(cmdBox); 
        iconSide.add(new JLabel(getIcon(COMMAND), JLabel.LEFT));

        final JCheckBox updateBox = new JCheckBox("Show " + UPDATE + " Nodes", isBoxChecked(UPDATE)); 
        updateBox.setFont(checkBoxFont);
        updateBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setUpdateProperties(updateBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(updateBox);
        iconSide.add(new JLabel(getIcon(UPDATE), JLabel.LEFT));

        final JCheckBox listBox = new JCheckBox("Show " + NODELIST + " Nodes", isBoxChecked(NODELIST)); 
        listBox.setFont(checkBoxFont);
        listBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setListProperties(listBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(listBox);
        iconSide.add(new JLabel(getIcon(NODELIST), JLabel.LEFT));

        final JCheckBox libBox = new JCheckBox("Show " + LIBRARYNODECALL + " Nodes", isBoxChecked(LIBRARYNODECALL));
        libBox.setFont(checkBoxFont);
        libBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setLibraryCallProperties(libBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(libBox);
        iconSide.add(new JLabel(getIcon(LIBRARYNODECALL), JLabel.LEFT));

        final JCheckBox ifBox = new JCheckBox("Show " + IF + " Actions", isBoxChecked(IF));
        ifBox.setFont(checkBoxFont);
        ifBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setIfProperties(ifBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(ifBox);
        iconSide.add(new JLabel(getIcon(IF), JLabel.LEFT));

        final JCheckBox thenBox = new JCheckBox("Show " + THEN + " Actions", isBoxChecked(THEN));
        thenBox.setFont(checkBoxFont);
        thenBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setThenProperties(thenBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(thenBox);
        iconSide.add(new JLabel(getIcon(THEN), JLabel.LEFT));

        final JCheckBox elseBox = new JCheckBox("Show " + ELSE + " Actions", isBoxChecked(ELSE));
        elseBox.setFont(checkBoxFont);
        elseBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setElseProperties(elseBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(elseBox);
        iconSide.add(new JLabel(getIcon(ELSE), JLabel.LEFT));

        final JCheckBox whileBox = new JCheckBox("Show " + WHILE + " Loop Actions", isBoxChecked(WHILE));
        whileBox.setFont(checkBoxFont);
        whileBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setWhileProperties(whileBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(whileBox);
        iconSide.add(new JLabel(getIcon(WHILE), JLabel.LEFT));

        final JCheckBox forBox = new JCheckBox("Show " + FOR + " Loop Actions", isBoxChecked(FOR));
        forBox.setFont(checkBoxFont);
        forBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setForProperties(forBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(forBox);
        iconSide.add(new JLabel(getIcon(FOR), JLabel.LEFT));

        final JCheckBox tryBox = new JCheckBox("Show " + TRY + " Actions", isBoxChecked(TRY));
        tryBox.setFont(checkBoxFont);
        tryBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setTryProperties(tryBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(tryBox);
        iconSide.add(new JLabel(getIcon(TRY), JLabel.LEFT));

        final JCheckBox seqBox = new JCheckBox("Show " + SEQ + " Actions", isBoxChecked(SEQ));
        seqBox.setFont(checkBoxFont);
        seqBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setSequenceProperties(seqBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(seqBox);
        iconSide.add(new JLabel(getIcon(SEQ), JLabel.LEFT));

        final JCheckBox unchkdSeqBox = new JCheckBox("Show " + UNCHKD_SEQ + " Actions", isBoxChecked(UNCHKD_SEQ));
        unchkdSeqBox.setFont(checkBoxFont);
        unchkdSeqBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setUncheckedSequenceProperties(unchkdSeqBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(unchkdSeqBox);
        iconSide.add(new JLabel(getIcon(UNCHKD_SEQ), JLabel.LEFT));
        
        final JCheckBox concBox = new JCheckBox("Show " + CONCURRENCE + " Actions", isBoxChecked(CONCURRENCE));
        concBox.setFont(checkBoxFont);
        concBox.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                setConcurrenceProperties(concBox.isSelected() ? "SHOW" : "HIDE");
                // Luv.getLuv().getViewHandler().refreshRegexView(); // *** FIXME ***
            }
        });
        checkBoxSide.add(concBox);
        iconSide.add(new JLabel(getIcon(CONCURRENCE), JLabel.LEFT));
        
        checkBoxList.add(iconSide);
        checkBoxList.add(checkBoxSide);

        add(checkBoxList, BorderLayout.EAST);
    }

    //
    // Utility functions for ActionListener methods above
    //

    private static void setAssignmentProperties(String value)
    {
        Settings.instance().setProperty(ASSIGNMENT, value);
        Settings.instance().setProperty(THEN_ASSN, value);
        Settings.instance().setProperty(ELSE_ASSN, value);
    }

    private static void setCommandProperties(String value)
    {
        Settings.instance().setProperty(COMMAND, value);
        Settings.instance().setProperty(THEN_CMD, value);
        Settings.instance().setProperty(ELSE_CMD, value);
    }

    private static void setUpdateProperties(String value)
    {
        Settings.instance().setProperty(UPDATE, value);
        Settings.instance().setProperty(THEN_UPDATE, value);
        Settings.instance().setProperty(ELSE_UPDATE, value);
    }

    private static void setEmptyProperties(String value)
    {
        Settings.instance().setProperty(EMPTY, value);
        Settings.instance().setProperty(THEN_EMPTY, value);
        Settings.instance().setProperty(ELSE_EMPTY, value);
    }

    private static void setListProperties(String value)
    {
        Settings.instance().setProperty(NODELIST, value);
        Settings.instance().setProperty(THEN_LIST, value);
        Settings.instance().setProperty(ELSE_LIST, value);
    }

    private static void setLibraryCallProperties(String value)
    {
        Settings.instance().setProperty(LIBRARYNODECALL, value);
        Settings.instance().setProperty(THEN_LIB, value);
        Settings.instance().setProperty(ELSE_LIB, value);
    }

    private static void setIfProperties(String value)
    {
        Settings.instance().setProperty(IF, value);
        Settings.instance().setProperty(THEN_IF, value);
        Settings.instance().setProperty(ELSE_IF, value);
    }

    private static void setThenProperties(String value)
    {
        Settings.instance().setProperty(THEN, value);
        Settings.instance().setProperty(THEN_LIST, value);
        Settings.instance().setProperty(THEN_ASSN, value);
        Settings.instance().setProperty(THEN_CMD, value);
        Settings.instance().setProperty(THEN_EMPTY, value);
        Settings.instance().setProperty(THEN_LIB, value);
        Settings.instance().setProperty(THEN_UPDATE, value);
        Settings.instance().setProperty(THEN_IF, value);
        Settings.instance().setProperty(THEN_WHILE, value);
        Settings.instance().setProperty(THEN_FOR, value);
        Settings.instance().setProperty(THEN_TRY, value);
        Settings.instance().setProperty(THEN_SEQ, value);
        Settings.instance().setProperty(THEN_UNCHKD_SEQ, value);
        Settings.instance().setProperty(THEN_CONCURRENCE, value);
    }

    private static void setElseProperties(String value)
    {
        Settings.instance().setProperty(ELSE, value);
        Settings.instance().setProperty(ELSE_LIST, value);
        Settings.instance().setProperty(ELSE_ASSN, value);
        Settings.instance().setProperty(ELSE_CMD, value);
        Settings.instance().setProperty(ELSE_EMPTY, value);
        Settings.instance().setProperty(ELSE_LIB, value);
        Settings.instance().setProperty(ELSE_UPDATE, value);
        Settings.instance().setProperty(ELSE_IF, value);
        Settings.instance().setProperty(ELSE_WHILE, value);
        Settings.instance().setProperty(ELSE_FOR, value);
        Settings.instance().setProperty(ELSE_TRY, value);
        Settings.instance().setProperty(ELSE_SEQ, value);
        Settings.instance().setProperty(ELSE_UNCHKD_SEQ, value);
        Settings.instance().setProperty(ELSE_CONCURRENCE, value);
    }

    private static void setWhileProperties(String value)
    {
        Settings.instance().setProperty(WHILE, value);
        Settings.instance().setProperty(THEN_WHILE, value);
        Settings.instance().setProperty(ELSE_WHILE, value);
    }

    private static void setForProperties(String value)
    {
        Settings.instance().setProperty(FOR, value);
        Settings.instance().setProperty(THEN_FOR, value);
        Settings.instance().setProperty(ELSE_FOR, value);
    }

    private static void setTryProperties(String value)
    {
        Settings.instance().setProperty(TRY, value);
        Settings.instance().setProperty(THEN_TRY, value);
        Settings.instance().setProperty(ELSE_TRY, value);
    }

    private static void setSequenceProperties(String value)
    {
        Settings.instance().setProperty(SEQ, value);
        Settings.instance().setProperty(THEN_SEQ, value);
        Settings.instance().setProperty(ELSE_SEQ, value);
    }

    private static void setUncheckedSequenceProperties(String value)
    {
        Settings.instance().setProperty(UNCHKD_SEQ, value);
        Settings.instance().setProperty(THEN_UNCHKD_SEQ, value);
        Settings.instance().setProperty(ELSE_UNCHKD_SEQ, value);
    }

    private static void setConcurrenceProperties(String value)
    {
        Settings.instance().setProperty(CONCURRENCE, value);
        Settings.instance().setProperty(THEN_CONCURRENCE, value);
        Settings.instance().setProperty(ELSE_CONCURRENCE, value);
    }

    private static boolean isBoxChecked(String type)
    {
        String prop = Settings.instance().get(type);
        return (prop == null || prop.equals("SHOW"));
    }

    /**
     * The ShowListener class provides a listener for when the user presses the 
     * "Show" button which activates the actionPerformed() method that will 
     * un-hide the element the user had selected from the list of hidden elements.
     */
    class ShowListener implements ActionListener 
    {
        /**
         * Reveals the elements in the list of hidden elements the user selected.
         * 
         * @param e the ActionEvent that represents when the user pressed the "Show" button
         */
        public void actionPerformed(ActionEvent e) 
        {
            int index = list.getSelectedIndex();
            int size = listModel.getSize();

            if (size == 0) 
            { 
                //Nobody's left, disable firing.
                showButton.setEnabled(false);
            } 
            else 
            { 
                showButton.setEnabled(true);
                String regex = (String) listModel.remove(index);
                // *** FIXME: there MUST be a better way to do this! ***
                Luv.getLuv().getRegexNodeFilter().removeRegex(regex);
            
                //Select an index.
                if (index == listModel.getSize()) 
                    //removed item in last position
                    index--;

                list.setSelectedIndex(index);
                list.ensureIndexIsVisible(index);
            }
        }
    }

    /**
     * The HideListener class is a listener shared by the text field and the 
     * hide button and listens for when the user pressed the "Hide" button.
     */
    class HideListener implements ActionListener, DocumentListener
    {
        private boolean alreadyEnabled = false;
        private JButton button;

        /**
         * Constructs a HideListener with the specified button.
         * 
         * @param button the "Hide" button
         */
        public HideListener(JButton button) 
        {
            this.button = button;
        }

        /**
         * Hides the element the user typed into the text field.
         * 
         * @param e the ActionEvent that represents when the user pressed the 
         * "Hide" button
         */
        public void actionPerformed(ActionEvent e) 
        {
            String regex = textField.getText();            

            // Ignore empty input
            if (regex.isEmpty())
                return;
            
            // User didn't type in a unique name
            if (alreadyInList(regex)) {
                JOptionPane.showMessageDialog(frame, 
                                              regex + " has already been entered", 
                                              "Error", 
                                              JOptionPane.ERROR_MESSAGE);
                textField.requestFocusInWindow();
                textField.selectAll();
                return;
            }

            Luv.getLuv().getRegexNodeFilter().addRegex(regex);

            int index = list.getSelectedIndex(); //get selected index
            if (index == -1) 
                //no selection, so insert at beginning
                index = 0;
            else 
                //add after the selected item
                index++;

            listModel.insertElementAt(textField.getText(), index);

            //Select the new item and make it visible.
            list.setSelectedIndex(index);
            list.ensureIndexIsVisible(index);

            //Reset the text field.
            textField.setText("");
            textField.requestFocusInWindow();
        }         

        /**
         * Tests for whether the specified string is already in the hidden list.
         * @param name the string the user entered to be hidden
         * @return whether or not the string is already present
         */
        protected boolean alreadyInList(String name) 
        {
            return listModel.contains(name);
        }

        /** Required by DocumentListener. */
        public void insertUpdate(DocumentEvent e) 
        {
            textField.setForeground(Color.BLACK);            
            enableButton();
        }

        /** Required by DocumentListener. */
        public void removeUpdate(DocumentEvent e) 
        {
            handleEmptyTextField(e);
        }

        /** Required by DocumentListener. */
        public void changedUpdate(DocumentEvent e) 
        {
            if (!handleEmptyTextField(e)) 
            {
                enableButton();
            }
        }

        private void enableButton() 
        {
            if (!alreadyEnabled) 
            {
                button.setEnabled(true);
            }
        }

        private boolean handleEmptyTextField(DocumentEvent e) 
        {
            if (e.getDocument().getLength() <= 0) 
            {
                button.setEnabled(false);
                alreadyEnabled = false;
                return true;
            }
            return false;
        }
    }
    
    /** Required by ListSelectionListener. */ 
    public void valueChanged(ListSelectionEvent e) 
    {
        if (e.getValueIsAdjusting() == false) 
        {
            if (list.getSelectedIndex() == -1) 
            {           
                //No selection, disable show button.
                showButton.setEnabled(false);
            } 
            else 
            {
                //Selection, enable the show button.
                showButton.setEnabled(true);
            }
        }
    }
    
    /**
     * Opens the window.  
     */
    public void open()
    {
        if (frame == null)
            createFrame();
        if (!frame.isVisible())
            frame.setVisible(true);
    }

    private void createFrame()
    {
        frame = new JFrame("Hide/Show Nodes");
        frame.add(this, BorderLayout.CENTER);
        frame.setSize(Settings.instance().getDimension(PROP_HIDESHOWWIN_SIZE));
        frame.setLocation(Settings.instance().getPoint(PROP_HIDESHOWWIN_LOC));
        frame.pack();
    }

}

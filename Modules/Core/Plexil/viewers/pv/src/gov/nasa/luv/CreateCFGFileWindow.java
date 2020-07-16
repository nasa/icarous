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


/**
 * This class was modeled after the CheckNodeTreeExample, CheckNode, CheckRenerer
 * classes from Tame Swing examplescreated by Nobuo Tamemasa.
 *
 * http://devdaily.com/java/swing/tame/
 * 
@author Nobuo Tamemasa
@version 1.0 01/11/99
*/

package gov.nasa.luv;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.Font;
import java.io.PrintStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Scanner;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTree;
import javax.swing.tree.TreeSelectionModel;
import javax.swing.UIManager;

import static gov.nasa.luv.Constants.COMPLETE_FLAG_LIST;
import static gov.nasa.luv.Constants.DEBUG_CFG_FILE;
import static gov.nasa.luv.Constants.PROP_CFGWIN_LOC;
import static gov.nasa.luv.Constants.PROP_CFGWIN_SIZE;
import static gov.nasa.luv.Constants.PYTHON_SCRIPT;


/** 
 * The CreateCFGFileWindow class is an interface for the user to create a
 * CFG File.
 */
public class CreateCFGFileWindow extends JFrame implements ItemListener {

    private static String topMessage = null;
    private static String infoMessage = null;

    private static CreateCFGFileWindow frame;
    private static boolean error;
    private JCheckBox enableCFGFile;
    private JPanel topSection;
    private JScrollPane checkBoxList;
    private JScrollPane previewArea;
    private JPanel buttonPane;
    private CheckNode[] nodes;
    private JTextArea preview;

    public CreateCFGFileWindow() {
    }

    /** Construct an DebugCFGWindow. 
     *
     * @param title the title of this DebugCFGWindow
     */
    public CreateCFGFileWindow(String title) throws FileNotFoundException {
        super(title);
        error = false;
        createCheckList();

        if (!error) {
            createPreviewArea();
            createTopSection();
            createButtons();

            getContentPane().add(topSection, BorderLayout.NORTH);
            getContentPane().add(checkBoxList, BorderLayout.WEST);
            getContentPane().add(previewArea, BorderLayout.EAST);
            getContentPane().add(buttonPane, BorderLayout.SOUTH);
        }
    }

    private void createCheckList() throws FileNotFoundException {
        ArrayList<String> list = new ArrayList<String>();
        JTree main_tree;

        // first gather debug flags from complete debug list
        try {
            Scanner scanner = new Scanner(new File(COMPLETE_FLAG_LIST));

            try {
                while (scanner.hasNextLine()) {
                    String line = scanner.nextLine().trim();
                    if (line.startsWith("#:")) {
                        list.add(line.substring(1, line.length()));
                    }
                }
            } finally {
                scanner.close();
            }
        } catch (FileNotFoundException ex) {
            StatusMessageHandler.instance().displayErrorMessage(this,
                                                                       ex,
                                                                       "ERROR: " + COMPLETE_FLAG_LIST +
                                                                       " not found.\nYou need to run the python script: " +
                                                                       PYTHON_SCRIPT + " and try again");

            error = true;
        }

        // attach each flag to a check box
        nodes = new CheckNode[list.size() + 1];

        // root node
        nodes[0] = new CheckNode("Check All");
        for (int a = 1; a < list.size(); a++) {
            nodes[a] = new CheckNode(list.get(a));
            nodes[0].add(nodes[a]);
        }

        // place check boxes into check box tree
        main_tree = new JTree(nodes[0]);
        main_tree.setCellRenderer(new CheckRenderer());
        main_tree.getSelectionModel().setSelectionMode(
                TreeSelectionModel.SINGLE_TREE_SELECTION);
        main_tree.addMouseListener(new NodeSelectionListener(main_tree));
        checkBoxList = new JScrollPane(main_tree);
        checkBoxList.setPreferredSize(new Dimension(450, 50));
    }

    private void createPreviewArea() throws FileNotFoundException {
        preview = new JTextArea();
        preview.setPreferredSize(new Dimension(435, 50));
        setPreviewOfCFGFile();
        preview.setEditable(false);
        previewArea = new JScrollPane(preview);
    }

    private void createTopSection() {
        JLabel topLabel = new JLabel();
        topLabel.setText(getTopMessage());
        topLabel.setFont(topLabel.getFont().deriveFont(Font.PLAIN, 12.0f));

        enableCFGFile = new JCheckBox("Enable Debug Messages");
        enableCFGFile.addItemListener(this);

        if (preview.getText().startsWith("#")) {
            enableCFGFile.setSelected(false);
        } else {
            enableCFGFile.setSelected(true);
        }

        topSection = new JPanel();
        topSection.setLayout(new BoxLayout(topSection, BoxLayout.PAGE_AXIS));
        topSection.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        topSection.add(topLabel);
        topSection.add(enableCFGFile);
    }

    private void createButtons() {
        JButton exitButton = new JButton("Exit");
        exitButton.addActionListener(new ButtonActionListener(nodes[0]));
        JButton infoButton = new JButton("Info");
        infoButton.addActionListener(new ButtonActionListener(nodes[0]));
        JButton clearButton = new JButton("Clear CFG file");
        clearButton.addActionListener(new ButtonActionListener(nodes[0]));
        JButton createCFGButton = new JButton("Create CFG file");
        createCFGButton.addActionListener(new ButtonActionListener(nodes[0]));

        // file location message
        JLabel locationMessage = new JLabel();
        locationMessage.setText("Debug CFG file location: " + DEBUG_CFG_FILE);
        locationMessage.setFont(locationMessage.getFont().deriveFont(
                Font.PLAIN, 12.0f));

        // Panel to hold buttons and file location message
        buttonPane = new JPanel();
        buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.LINE_AXIS));
        buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        buttonPane.add(locationMessage);
        buttonPane.add(Box.createHorizontalGlue());
        buttonPane.add(exitButton);
        buttonPane.add(Box.createHorizontalStrut(3));
        buttonPane.add(infoButton);
        buttonPane.add(Box.createHorizontalStrut(3));
        buttonPane.add(clearButton);
        buttonPane.add(Box.createHorizontalStrut(3));
        buttonPane.add(createCFGButton);
        buttonPane.add(Box.createRigidArea(new Dimension(10, 0)));
    }

    private ArrayList<String> readInDebugCFGFile() throws FileNotFoundException {
        ArrayList<String> lines = new ArrayList<String>();

        if (new File(DEBUG_CFG_FILE).exists()) {
            Scanner scanner = new Scanner(new File(DEBUG_CFG_FILE));

            try {
                String line;

                while (scanner.hasNextLine()) {
                    line = scanner.nextLine().trim();
                    lines.add(line);
                }
            } catch (Exception e) {
                StatusMessageHandler.instance().displayErrorMessage(this,
                                                                           e,
                                                                           "ERROR: exception occurred while reading "
                                                                           + DEBUG_CFG_FILE);
            } finally {
                scanner.close();
            }
        }

        return lines;
    }

    private void setPreviewOfCFGFile() throws FileNotFoundException {
        ArrayList<String> lines = new ArrayList<String>();
        lines = readInDebugCFGFile();

        if (!lines.isEmpty()) {
            preview.setText("");
            for (String line : lines) {
                preview.append(line + "\n");
            }
        } else {
            if (preview != null) {
                preview.setText("");
            }
        }
    }

    private static String getTopMessage() {
        if (topMessage == null) {
            StringBuilder sb = new StringBuilder();

            sb.append("<html><p align=left>");
            sb.append("<br><u>Steps for Creating and Customizing a Debug Configuration File</u></br>");
            sb.append("<br></br>");
            sb.append("<br><b>Step 1:</b> Check desired Debug Tags</br>");
            sb.append("<br><b>Step 2:</b> Press Create CFG file button</br>");
            sb.append("<br><b>Step 3:</b> Confirm preview is correct</br>");
            sb.append("<br><b>Step 4:</b> Exit or Clear CFG file and start over</br>");
            sb.append("<br></br>");
            sb.append("<br>If debug messages are disabled, the debug tags will be commented out in debug file.</br>");
            sb.append("<br></br>");
            sb.append("</p></html>");

            topMessage = sb.toString();
        }
        return topMessage;
    }

    private static String getInfoMessage() {
        if (infoMessage == null) {
            StringBuilder sb = new StringBuilder();

            sb.append("<html><p align=left>");
            sb.append("<br><b>How Does the Debug Configuration File Tool Work?</b></br>");
            sb.append("<hr>");
            sb.append("<br>This tool creates a <b>Debug Configuration File</b> in synchronization with the checked</br>");
            sb.append("<br><b>Debug Tags</b> in the check box tree. The check box tree represents every possible</br>");
            sb.append("<br>Debug Tag that can be listed in the Debug Configuration File. Each element in the</br>");
            sb.append("<br>check box tree represents a single Debug Tag.</br>");
            sb.append("<br></br>");

            sb.append("<br><b>What is a Debug Tag?</b></br>");
            sb.append("<hr>");
            sb.append("<br>During the execution of a plan, Debug Tags trigger debugging messages to be sent</br>");
            sb.append("<br>from the Universal Executive. For example, the following Debug Tags would tell the</b>");
            sb.append("<br>UE to send messages for the state transitions and the final outcomes of every node:</br>");
            sb.append("<br></br>");
            sb.append("<pre>:Node:transition");
            sb.append("<br>:Node:outcome</pre>");

            sb.append("<br><u>Depending on the level of the tag you select, different messages will be sent</u></br>");
            sb.append("<br></br>");
            sb.append("<br>The following is a bottom level tag or child tag, which will trigger messages matching</br>");
            sb.append("<br>only this Debug Tag:</br>");
            sb.append("<pre>:Expression:activate</pre>");
            sb.append("<br>The following is a mid level tag or parent tag, which will trigger messages matching</br>");
            sb.append("<br>this Debug Tag and all the children of this Debug Tag:</br>");
            sb.append("<pre>:Expression:</pre>");
            sb.append("<br>The following is also a top level tag or parent tag but notice that it does not have a</br>");
            sb.append("<br>colon(:) at the end. Without the (:) this tag will trigger messages matching this Debug</br>");
            sb.append("<br>Tag and ANY Debug Tag starting with <i>Expression</i>:</br>");
            sb.append("<pre>:Expression</pre>");

            sb.append("<br><u>Important to Note</u></br>");
            sb.append("<br></br>");
            sb.append("<br>If parent tag is selected, only the parent tag appears in the debug file since printing</br>");
            sb.append("<br>the child tags would be redundant.</br>");
            sb.append("<br></br>");
            sb.append("<br>The more Debug Tags that are enabled, the slower your plan will run.</br>");
            sb.append("<br></br>");

            sb.append("<br><b>What Is The Debug Configuration File?</b></br>");
            sb.append("<hr>");
            sb.append("<br>The Debug Configuration File (Debug.cfg) is a text file. Debug.cfg contains lines</br>");
            sb.append("<br>starting with either a comment character ('#') or a colon (':') followed by a Debug</br>");
            sb.append("<br>Tag. Debugging messages matching any of the Debug Tags are printed to the Debug</br>");
            sb.append("<br>Window at run time. The following are example Debugging messages:</br>");
            sb.append("<br></br>");
            sb.append("<pre>[Node:transition]Transitioning 'root' from INACTIVE to WAITING");
            sb.append("<br>[Node:transition]Transitioning 'root' from WAITING to EXECUTING");
            sb.append("<br>[Node:transition]Transitioning 'library6' from INACTIVE to WAITING");
            sb.append("<br>[Node:transition]Transitioning 'library6' from WAITING to EXECUTING");
            sb.append("<br>[Node:transition]Transitioning 'library6' from EXECUTING to ITERATION_ENDED");
            sb.append("<br>[Node:iterationOutcome]Outcome of 'library6' is SUCCESS");
            sb.append("<br>[Node:transition]Transitioning 'library6' from ITERATION_ENDED to FINISHED");
            sb.append("<br>[Node:outcome]Outcome of 'library6' is SUCCESS</pre>");
            sb.append("</p></html>");

            infoMessage = sb.toString();
        }
        return infoMessage;
    }

    class ButtonActionListener implements ActionListener {

        CheckNode root;

        ButtonActionListener(final CheckNode root) {
            this.root = root;
        }

        public void actionPerformed(ActionEvent ev) {
            if (ev.getActionCommand().equals("Exit")) {
                frame.setVisible(false);
            } else if (ev.getActionCommand().equals("Info")) {
                JFrame infoWindow = new JFrame("Info");

                if (infoWindow != null && infoWindow.isVisible()) {
                    infoWindow.setVisible(false);
                }

                JLabel info = new JLabel();
                info.setText(getInfoMessage());
                info.setFont(info.getFont().deriveFont(Font.PLAIN, 12.0f));

                JPanel panel = new JPanel();
                panel.add(info);

                infoWindow.add(panel);
                infoWindow.pack();
                infoWindow.setVisible(true);
            } else if (ev.getActionCommand().equals("Clear CFG file")) {
                // verify that the user wants to clear CFG file
                Object[] options = {"Yes", "No"};

                int clear =
                        JOptionPane.showOptionDialog(CreateCFGFileWindow.this,
                        "Are you sure you want to clear the Debug CFG file?",
                        "Clear CFG file",
                        JOptionPane.YES_NO_CANCEL_OPTION,
                        JOptionPane.WARNING_MESSAGE,
                        null,
                        options,
                        options[0]);

                if (clear == 0) {
                    // clear text preview area
                    preview.setText(null);
                    // write 'nothing' to file (i.e. clear CFG file)
                    writeToDebugCFGFile(preview);
                }
            } else if (ev.getActionCommand().equals("Create CFG file")) {
                preview.setText("");
                ArrayList<String> parentSelected = new ArrayList<String>();
                Enumeration e = root.breadthFirstEnumeration();

                while (e.hasMoreElements()) {
                    CheckNode node = (CheckNode) e.nextElement();

                    if (node.isSelected()) {
                        if (!enableCFGFile.isSelected()) {
                            preview.append("#");
                        }

                        preview.append(node.toString() + "\n");
                        parentSelected.add(node.toString());
                    }
                }

                writeToDebugCFGFile(preview);
            }
        }

        private void writeToDebugCFGFile(JTextArea preview) {
            FileOutputStream out;
            PrintStream p;

            try {
                out = new FileOutputStream(DEBUG_CFG_FILE);
                p = new PrintStream(out);
                p.println(preview.getText());
                p.close();
            } catch (Exception e) {
                StatusMessageHandler.instance().displayErrorMessage(CreateCFGFileWindow.this,
                                                                           e,
                                                                           "ERROR: exception occurred while writing to " + DEBUG_CFG_FILE);
            }
        }
    }

    /** Enables or Disables the debug configuration file.  
     *
     *  @param enable indicates whether or not to enable the debug configuration file
     */
    public void enableDebugMessages(boolean enable) throws FileNotFoundException {
        if (new File(DEBUG_CFG_FILE).exists()) {
            Scanner scanner = new Scanner(new File(DEBUG_CFG_FILE));
            ArrayList<String> commented_lines = new ArrayList<String>();

            try {
                while (scanner.hasNextLine()) {
                    String line = scanner.nextLine().trim();
                    if (!line.equals("")) {
                        if (enable) {
                            commented_lines.add(line.replace("#", ""));
                        } else {
                            commented_lines.add("#" + line);
                        }
                    }
                }

                if (!commented_lines.isEmpty()) {
                    FileOutputStream out = new FileOutputStream(DEBUG_CFG_FILE);
                    PrintStream p = new PrintStream(out);

                    for (String line : commented_lines) {
                        p.println(line);
                    }

                    p.close();
                }
            } catch (Exception e) {
                StatusMessageHandler.instance().displayErrorMessage(this,
                                                                           e,
                                                                           "ERROR: exception occurred while enabling/disabling " + DEBUG_CFG_FILE);
            } finally {
                scanner.close();
            }
        }
    }

    /** Creates an instance of a CreateCFGFileWindow.  */
    public void open() throws FileNotFoundException {
        if (frame != null && frame.isVisible()) {
            frame.setVisible(false);
        }

        frame = new CreateCFGFileWindow("Create Debug Configuration File");

        if (!error) {
            frame.setPreferredSize(Settings.instance().getDimension(PROP_CFGWIN_SIZE));
            frame.setLocation(Settings.instance().getPoint(PROP_CFGWIN_LOC));
            frame.pack();
            frame.setVisible(true);
        }
    }

    /** {@inheritDoc} */
    public void itemStateChanged(ItemEvent e) {
        boolean enable = enableCFGFile.isSelected();

        try {
            if (frame != null) {
                frame.enableDebugMessages(enable);
                frame.setPreviewOfCFGFile();
            }
        } catch (FileNotFoundException ex) {
            StatusMessageHandler.instance().displayErrorMessage(this,
                                                                ex,
                                                                "ERROR: " + DEBUG_CFG_FILE + " not found");
        }
    }
}

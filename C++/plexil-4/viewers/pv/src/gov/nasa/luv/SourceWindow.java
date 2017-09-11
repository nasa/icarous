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

// TODO:
// Add menu bar with (at least) 'Refresh' option
// Handle source vs. executable for both plan, script

package gov.nasa.luv;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import javax.swing.Box;
import javax.swing.BorderFactory;
import javax.swing.DefaultListModel;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JTable;
import javax.swing.ListModel;
import javax.swing.SwingConstants;

import static javax.swing.ScrollPaneConstants.*;

import static gov.nasa.luv.Constants.*;

/**
 * The SourceWindow class is an interface for the user to view source file content.
 */
public class SourceWindow extends JFrame 
{
    private JComponent planPanel = null;
    private JLabel planLabel = null;
    private JScrollPane planScroller = null;
    private JList<String> planList = null;
    private JComponent scriptPanel = null;
    private JLabel scriptLabel = null;
    private JScrollPane scriptScroller = null;
    private JList<String> scriptList = null;
    private Plan plan = null;
    private File planFile = null;
    private File scriptFile = null;
    // private File planSource = null;
    // private File scriptSource = null;

    public SourceWindow() {
        super("Source Window");
        constructWindow();
    }

    public SourceWindow(Plan p) {
        super();
        constructWindow();
        open(p);
    }

    private void constructWindow() {
        constructPlanPanel();
        constructScriptPanel();
        JSplitPane split = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                                          planPanel,
                                          scriptPanel);
        split.setResizeWeight(0.5);
        getContentPane().add(split, BorderLayout.CENTER);
        setLocation(Settings.instance().getPoint(PROP_CFGWIN_LOC));
    }

    private void constructPlanPanel() {
        planPanel = Box.createVerticalBox();
        planLabel = new JLabel("");
        planLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        planPanel.add(planLabel);
        planScroller = new JScrollPane(VERTICAL_SCROLLBAR_AS_NEEDED,
                                       HORIZONTAL_SCROLLBAR_AS_NEEDED);
        planList = new JList<String>();
        planList.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 10));
        planScroller.setViewportView(planList);
        planPanel.add(planScroller);
        planPanel.setBorder(BorderFactory.createEmptyBorder(6, 6, 6, 6));
        planPanel.setPreferredSize(new Dimension(400, 500));
        planPanel.setMinimumSize(new Dimension(100, 50));
    }

    private void constructScriptPanel() {
        scriptPanel = Box.createVerticalBox();
        scriptLabel = new JLabel("");
        scriptLabel.setHorizontalTextPosition(SwingConstants.CENTER);
        scriptPanel.add(scriptLabel);
        scriptScroller = new JScrollPane(VERTICAL_SCROLLBAR_AS_NEEDED,
                                         HORIZONTAL_SCROLLBAR_ALWAYS);
        scriptList = new JList<String>();
        scriptList.setBorder(BorderFactory.createEmptyBorder(0, 0, 10, 10));
        scriptScroller.setViewportView(scriptList);
        scriptPanel.add(scriptScroller);
        scriptPanel.setBorder(BorderFactory.createEmptyBorder(6, 6, 6, 6));
        scriptPanel.setPreferredSize(new Dimension(400, 500));
        scriptPanel.setMinimumSize(new Dimension(100, 50));
    }

    // Presumes f is a readable plain file.
    private ListModel<String> loadFileModel(File f) {
        DefaultListModel<String> result = new DefaultListModel<String>();
        try {
            FileInputStream fstream = new FileInputStream(f);
            DataInputStream in = new DataInputStream(fstream);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            String line;

            while ((line = br.readLine()) != null)
                result.addElement(line);
	
            in.close();
        }
        catch (Exception e) {
            StatusMessageHandler.instance().displayErrorMessage(this,
                                                                e,
                                                                "ERROR: source window unable to load " + f.toString());
            return null;
        }        

        return result;
    }

    /**
     * Displays the specified Plexil Plan in the source window.
     */
    public void open(Plan p) {
        if (isVisible())
            setVisible(false);

        boolean loaded = false;
        if (p != null) {
            plan = p;
            planFile = p.getPlanFile();
            if (planFile != null && planFile.isFile() && planFile.canRead()) {
                ListModel<String> planModel = loadFileModel(planFile);
                if (planModel != null) {
                    planList.setModel(planModel);
                    planLabel.setText(planFile.toString());
                    loaded = true;
                }
            }
            scriptFile = p.getScriptFile();
            if (scriptFile != null && scriptFile.isFile() && scriptFile.canRead()) {
                ListModel<String> scriptModel = loadFileModel(scriptFile);
                if (scriptModel != null) {
                    scriptList.setModel(scriptModel);
                    scriptLabel.setText(scriptFile.toString());
                    loaded = true;
                }
            }
        }
        if (loaded) {
            setTitle("Source Window - " + p.getName());
            pack();
        	setVisible(true);
        }
        else
        	StatusMessageHandler.instance().showStatus("No Data available", Color.RED, 10000);
    }
    
    /**
     * Updates the current contents of the SourceWindow.
     */

    public void refresh() {
        open(plan);
    }
}

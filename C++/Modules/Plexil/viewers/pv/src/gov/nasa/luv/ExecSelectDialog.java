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

import static java.awt.event.InputEvent.META_DOWN_MASK;
import static java.awt.event.KeyEvent.*;
import static java.awt.GridBagConstraints.*;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.SwingConstants;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;

import java.util.Vector;

import gov.nasa.luv.Constants.AppType;
import static gov.nasa.luv.Constants.AppType.*;

public class ExecSelectDialog
    extends JFrame {

    private static final Dimension FILE_LIST_DIALOG_SIZE = new Dimension(600, 400);
    private static final Point LIBRARY_DIALOG_LOC = new Point(300, 300);
    private static final Point PATH_DIALOG_LOC = new Point(400, 400);

    private static final File DEV_NULL = new File("/dev/null");

    private ButtonGroup execGroup;
	private JRadioButton plexilTest, plexilExec, plexilSim, userDefined, externalApp;
	private JButton debugBut, configBut, scriptBut, saveBut;
    private JButton defaultScriptBut, defaultConfigBut;
	private JLabel configLab, debugLab, scriptLab, libLab, pathLab;
    private JButton defaultDebugBut, noneDebugBut, generateDebugBut;
	private FileFilter planFilter, debugFilter, configFilter, teScriptFilter;

    private LibraryListEditor libEditor;
    private LibraryPathEditor pathEditor;

    private Plan plan;
    private PlanView view;

    //
    // Local temporary storage
    //

    private AppType mode;
    private File debugFile, scriptFile, configFile;
    private Vector<File> libraryPath, libraryFiles;

    public ExecSelectDialog(PlanView parent) {
        super();
        view = parent;
        plan = null;

		planFilter = new FileNameExtensionFilter("Plexil plan", "plx");
		debugFilter = new FileNameExtensionFilter("Debug config file", "cfg");
		configFilter = new FileNameExtensionFilter("PlexilExec config file", "xml");
		teScriptFilter = new FileNameExtensionFilter("PlexilTest sim script", "psx");

        libEditor = null;
        pathEditor = null;

        constructFrame();
	}

    private void constructFrame() {
        JPanel panel = new JPanel();
		panel.add(constructPatternPanel());				
		panel.setBorder(BorderFactory.createEmptyBorder(20, 40, 20, 40));		
        panel.setOpaque(true);
        setContentPane(panel);
    }

	private JPanel constructPatternPanel() {
		JPanel patternPanel = new JPanel();
		GridBagLayout gridbag = new GridBagLayout();
		patternPanel.setLayout(gridbag);

        // Radio buttons
		int row = 0;
		execGroup = new ButtonGroup(); 

		plexilTest = new JRadioButton("PlexilTest");
		plexilTest.setToolTipText("Execute a plan using the scripted Test Executive");
		execGroup.add(plexilTest);
		plexilTest.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setMode(PLEXIL_TEST);
                    view.showMessage("Use PlexilTest", Color.GREEN.darker());
                    refresh();
                }
            });

		plexilExec = new JRadioButton("PlexilExec");
		plexilExec.setToolTipText("Execute a plan with PlexilExec");
		execGroup.add(plexilExec);
		plexilExec.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setMode(PLEXIL_EXEC);
                    view.showMessage("Use PlexilExec", Color.GREEN.darker());
                    refresh();
                }
            });

		plexilSim = new JRadioButton("PlexilSim");
		plexilSim.setToolTipText("Execute a plan using PlexilExec and PlexilSimulator");
		execGroup.add(plexilSim);
		plexilSim.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setMode(PLEXIL_SIM);
                    view.showMessage("Use PlexilSim", Color.GREEN.darker());
                    refresh();
                }
            });

		userDefined = new JRadioButton("User defined");
		userDefined.setToolTipText("Execute a plan with a user-defined executive");
		execGroup.add(userDefined);
		userDefined.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setMode(USER_SPECIFIED);
                    view.showMessage("Use user-defined exec", Color.GREEN.darker());
                    refresh();
                }
            });

		externalApp = new JRadioButton("External");
		externalApp.setToolTipText("Monitor plan execution on an external executive");
		execGroup.add(externalApp);
		externalApp.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setMode(EXTERNAL_APP);
                    view.showMessage("Monitor external exec", Color.GREEN.darker());
                    refresh();
                }
            });

		gridbag.setConstraints(plexilTest, makeConstraints(0, row, 1, 1, 1, 1));
		gridbag.setConstraints(plexilExec, makeConstraints(1, row, 1, 1, 1, 1));
		gridbag.setConstraints(plexilSim, makeConstraints(2, row, 1, 1, 1, 1));
		gridbag.setConstraints(externalApp, makeConstraints(3, row, 1, 1, 1, 1));
		gridbag.setConstraints(userDefined, makeConstraints(4, row, 1, 1, 1, 1));

        patternPanel.add(plexilTest);
        patternPanel.add(plexilExec);
        patternPanel.add(plexilSim);
        patternPanel.add(userDefined);
        patternPanel.add(externalApp);

        // Libraries button
		row++; // 1
		JButton libBut = new JButton("Libraries");
        libBut.setToolTipText("Select library files for the Exec to load.");
		libBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (libEditor == null)
                        libEditor = new LibraryListEditor();
                    libEditor.display();
                }
            });
        libLab = new JLabel();
		gridbag.setConstraints(libBut, makeConstraints(0, row, 1, 1, 1, 1));
		gridbag.setConstraints(libLab, makeConstraints(1, row, 1, 5, 1, 1));
        patternPanel.add(libBut);
        patternPanel.add(libLab);

        // Library path button
		row++; // 2
		JButton pathBut = new JButton("Library path");
		pathBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (pathEditor == null)
                        pathEditor = new LibraryPathEditor();
                    pathEditor.display();
                }
            });
        pathLab = new JLabel();
		gridbag.setConstraints(pathBut, makeConstraints(0, row, 1, 1, 1, 1));
		gridbag.setConstraints(pathLab, makeConstraints(1, row, 1, 5, 1, 1));
        patternPanel.add(pathBut);
        patternPanel.add(pathLab);
			
        // Configuration
		row++; //3
		configLab = new JLabel("");
		configBut =
            new JButton(new LuvAction("Interface Config",
                                      "Choose an Interface Config file.",
                                      VK_E,
                                      META_DOWN_MASK) {

                    public void actionPerformed(ActionEvent e) {
                        File c = chooseFile(configFile,
                                            configFilter,
                                            "Select Configuration");
                        if (c != null) {
                            configFile = c;
                            updateLabel(configLab, c);
                        }
                    }
                });
		configBut.setEnabled(false);
		configBut.setVisible(false);
		defaultConfigBut = new JButton("Use Default");
		defaultConfigBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (mode == PLEXIL_EXEC || mode == PLEXIL_SIM) {
                        configFile = new File(Constants.DEFAULT_CONFIG_PATH, Constants.DEFAULT_CONFIG_NAME);
                        updateLabel(configLab, configFile);
                        view.showMessage("Default Config");
                    }
                }
            });
		defaultConfigBut.setVisible(false);
		gridbag.setConstraints(configBut, makeConstraints(0, row, 1, 1, 1, 1));
		gridbag.setConstraints(configLab, makeConstraints(1, row, 1, 5, 1, 1));		
		gridbag.setConstraints(defaultConfigBut, makeConstraints(2, row, 1, 10, 1, 1));
        patternPanel.add(configBut);
        patternPanel.add(configLab);
        patternPanel.add(defaultConfigBut);
				
        // Sim script
		row++;//4
		scriptLab = new JLabel("");	
		scriptBut =
            new JButton(new LuvAction("Sim Script",
                                      "Choose a script file.",
                                      VK_E,
                                      META_DOWN_MASK) {
                    public void actionPerformed(ActionEvent e) {
                        chooseScriptFile(scriptFile);
                    }
                });
		defaultScriptBut = new JButton("Use Default");
        defaultScriptBut.setToolTipText("Use an empty sim script");
		defaultScriptBut.addActionListener(new ActionListener() {
                // Dumbed down for simplicity's sake
                public void actionPerformed(ActionEvent e) {
                    scriptFile = Settings.instance().defaultEmptyScriptFile(mode);
                    updateLabel(scriptLab, scriptFile);
                    view.showMessage("Default Script");
                }
            }
            );
		gridbag.setConstraints(scriptBut, makeConstraints(0, row, 1, 1, 1, 1));
		gridbag.setConstraints(scriptLab, makeConstraints(1, row, 1, 5, 1, 1));
		gridbag.setConstraints(defaultScriptBut, makeConstraints(2, row, 1, 10, 1, 1));
        patternPanel.add(scriptBut);
        patternPanel.add(scriptLab);
        patternPanel.add(defaultScriptBut);
		
        // Debug config
        row++;//5
		debugLab = new JLabel(" ");
		debugBut =
            new JButton(new LuvAction("Debug",
                                      "Choose a debug file.",
                                      VK_D,
                                      META_DOWN_MASK) {
                    public void actionPerformed(ActionEvent e) {
                        chooseDebugFile(debugFile);
                    }
                });
		defaultDebugBut = new JButton("Use Default");
        defaultDebugBut.setToolTipText("Use standard debug file location");
		defaultDebugBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    String debugDir = System.getProperty("user.dir");
                    if (debugDir == null)
                        debugDir = plan.getPlanFile().getParent();
                    debugFile = new File(debugDir, "Debug.cfg");
                    updateLabel(debugLab, debugFile);
                    view.showMessage("Selected default debug file " + debugFile);
                }
            });
		noneDebugBut = new JButton("None");
        noneDebugBut.setToolTipText("Don't use a debug file at all");
		noneDebugBut.addActionListener(new ActionListener() {
                // Dumbed down for simplicity's sake
                public void actionPerformed(ActionEvent e) {
                    debugFile = DEV_NULL;
                    debugLab.setText("(none)");
                    view.showMessage("Selected no debug file");
                }
            });
        generateDebugBut = new JButton("Generate");
        generateDebugBut.setToolTipText("Generate a debug file from a menu");
        generateDebugBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    // *** TODO ***
                    File f = generateDebugFile();
                    if (f == null) {
                        view.showMessage("Canceled generate debug file");
                    }
                    else {
                        debugFile = f;
                        updateLabel(debugLab, debugFile);
                        view.showMessage("Generated debug file " + f);
                    }
                }
            });
        
		gridbag.setConstraints(debugBut, makeConstraints(0, row, 1, 1, 1, 1));
		gridbag.setConstraints(debugLab, makeConstraints(1, row, 1, 5, 1, 1));
		gridbag.setConstraints(defaultDebugBut, makeConstraints(2, row, 1, 10, 1, 1));
		gridbag.setConstraints(noneDebugBut, makeConstraints(3, row, 1, 10, 1, 1));
		gridbag.setConstraints(generateDebugBut, makeConstraints(4, row, 1, 10, 1, 1));
        patternPanel.add(debugBut);
        patternPanel.add(debugLab);
        patternPanel.add(defaultDebugBut);
        patternPanel.add(noneDebugBut);
        patternPanel.add(generateDebugBut);

		row+=2;//7
		JButton cancelBut = new JButton("Cancel");
		cancelBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setVisible(false);
                    view.showMessage("Reverted Configuration");
                }
            }
            );
		saveBut = new JButton("OK");
		saveBut.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    saveSettings();
                    setVisible(false);
                }
            });
		gridbag.setConstraints(cancelBut, makeConstraints(1, row, 10, 10, 1, 10));
		gridbag.setConstraints(saveBut, makeConstraints(2, row, 10, 10, 1, 10));
        patternPanel.add(cancelBut);
        patternPanel.add(saveBut);

        return patternPanel;
	}
	
	/*
     * Extended Gridbag Configure component method
     * @param x and y are position in panel. top, left, bot, right refer to spacing around object
     */

	private static GridBagConstraints makeConstraints(int x, int y, int top, int left, int bot, int right) {
		GridBagConstraints c = new GridBagConstraints();		
        c.insets = new Insets(top , left , bot , right);
		c.weightx = 0.5;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = x;
		c.gridy = y;
        return c;
	}

    public void enableSaveButton() {
        saveBut.setEnabled(true);
    }

    public void disableSaveButton() {
        saveBut.setEnabled(false);
    }

	public JButton getDefaultScriptBut() {
		return defaultScriptBut;
	}

	public void activate() {
        Plan plan = view.getPlan();
        // Copy plan settings to local
        libraryPath = new Vector<File>(plan.getLibraryPath());
        libraryFiles = new Vector<File>(plan.getLibraryFiles());
        debugFile = plan.getDebugFile();
        scriptFile = plan.getScriptFile();
        configFile = plan.getConfigFile();

        setTitle("Settings for plan " + plan.getName());
        AppType t = plan.getAppType();
        if (t == NO_APP) // new plan
            t = Settings.instance().getAppMode(); // use last default
        setMode(t);
        refresh();
        setVisible(true);
	}
	
    // Refreshes the dialog display on update
    public void refresh() {
        updateLabel(debugLab, debugFile);

        switch (libraryFiles.size()) {
        case 0:
            libLab.setText("(empty)");
            libLab.setToolTipText(null);
            break;

        case 1:
            libLab.setText("1 file");
            libLab.setToolTipText(libraryFiles.get(0).toString());
            break;

        default:
            libLab.setText(libraryFiles.size() + " files");
            libLab.setToolTipText(null);
            break;
        }

        switch (libraryPath.size()) {
        case 0:
            pathLab.setText("(empty)");
            pathLab.setToolTipText(null);
            break;

        case 1:
            pathLab.setText("1 directory");
            pathLab.setToolTipText(libraryPath.get(0).toString());
            break;

        default:
            pathLab.setText(libraryPath.size() + " directories");
            pathLab.setToolTipText(null);
            break;
        }

        updateMode();
        repaint();
	}

    private void updateMode() {
		// Set radio buttons
        switch (mode) {
        case EXTERNAL_APP:
            externalApp.setSelected(true);
            break;

        case PLEXIL_EXEC:
            plexilExec.setSelected(true);
            break;

        case PLEXIL_TEST:
            plexilTest.setSelected(true);
            break;

        case PLEXIL_SIM:
            plexilSim.setSelected(true);
            break;

        case USER_SPECIFIED:
            userDefined.setSelected(true);
            break;

        default:
            execGroup.clearSelection();
            break;
		}

        // Update script location if appropriate to mode
        if (mode == PLEXIL_SIM || mode == PLEXIL_TEST)
            updateLabel(scriptLab, scriptFile);
        else 
            updateLabel(scriptLab, null);
        
        // Update config location if appropriate to mode
        if (mode == PLEXIL_EXEC || mode == PLEXIL_SIM) 
            updateLabel(configLab, configFile);
        else
            updateLabel(configLab, null);
        
        updateButtonVisibility();
        pack();
    }

    private void updateButtonVisibility() {
        // Handle buttons and labels
        // Plan, library buttons always enabled and visible
        switch (mode) {
        case PLEXIL_EXEC:
            setDebugFileVisibility(true);
            setConfigFileVisibility(true);
            setScriptFileVisibility(false);
            break;

        case PLEXIL_TEST:
            setDebugFileVisibility(true);
            setConfigFileVisibility(false);
            setScriptFileVisibility(true);
            break;

        case PLEXIL_SIM:
            setDebugFileVisibility(true);
            setConfigFileVisibility(true);
            setScriptFileVisibility(true);
            break;

        case USER_SPECIFIED:
            // TODO
            break;

        default:
            setDebugFileVisibility(false);
            setConfigFileVisibility(false);
            setScriptFileVisibility(false);
            break;
        }
    }

    private void setDebugFileVisibility(boolean val) {
        debugLab.setVisible(val);
        debugBut.setVisible(val);
        debugBut.setEnabled(val);
        defaultDebugBut.setVisible(val);
        defaultDebugBut.setEnabled(val);
        noneDebugBut.setVisible(val);
        noneDebugBut.setEnabled(val);
        generateDebugBut.setVisible(val);
        generateDebugBut.setEnabled(val);
    }

    private void setConfigFileVisibility(boolean val) {
        configBut.setVisible(val);
        configBut.setEnabled(val);
        configLab.setVisible(val);
        defaultConfigBut.setEnabled(val);
        defaultConfigBut.setVisible(val);
    }

    private void setScriptFileVisibility(boolean val) {
        scriptBut.setVisible(val);
        scriptBut.setEnabled(val);
        scriptLab.setVisible(val);
        defaultScriptBut.setEnabled(val);
        defaultScriptBut.setVisible(val);
    }

    private static void updateLabel(JLabel label, File filename) {
        if (filename == null) {
            label.setText("");
            label.setToolTipText("");
        }
        else if (DEV_NULL.equals(filename)) {
            label.setText("(none)");
            label.setToolTipText("/dev/null");
        }
        else {
            label.setText(filename.getName());
            label.setToolTipText(filename.toString());
        }
    }
	
	/*
	 * Changes configuration mode
	 * @param Plexil mode constant
	 */
	private void setMode(AppType newMode) {
        mode = newMode;
        updateMode();
	}

    // Called by save button listener
    private void saveSettings() {
        Plan plan = view.getPlan();
        plan.setAppType(mode);
        plan.setLibraryPath(libraryPath);
        plan.setLibraryFiles(libraryFiles);
        plan.setConfigFile(configFile);
        plan.setScriptFile(scriptFile);
        plan.setDebugFile(debugFile);
        
        // Commit settings
        Settings.instance().setPlanDefaults(plan);
    }

    //
    // Dependent dialogs
    //

    private void chooseScriptFile(File dflt) {
        if (dflt == null) {
            dflt = view.getPlan().getPlanFile();
            if (dflt != null) 
                dflt = new File(dflt.getParent(), dflt.getName().replace(".plx", ".psx"));
            else 
                dflt = new File(System.getProperty("user.dir"),
                                "script.psx");
        }
        File s = chooseFile(dflt,
                            (mode == PLEXIL_TEST) ? teScriptFilter : null,
                            "Select Sim Script");
        if (s != null) {
            scriptFile = s;
            updateLabel(scriptLab, scriptFile);
        }
    }

    private void chooseDebugFile(File dflt) {
        if (dflt == null) {
            dflt = view.getPlan().getPlanFile();
            if (dflt != null) 
                dflt = new File(dflt.getParentFile(),
                                dflt.getName().replace(".plx", ".cfg"));
            else 
                dflt = new File(System.getProperty("user.dir"), "Debug.cfg");
        }

        // FIXME: No file is also a valid choice
        File d = chooseFile(dflt, debugFilter, "Select Debug");
        debugFile = d;
        updateLabel(debugLab, debugFile);
    }

    private File chooseDirectory(File dflt, String buttonText) {
        if (dflt == null)
            dflt = new File(System.getProperty("user.dir"));
        else if (!dflt.isDirectory())
            dflt = dflt.getParentFile();
        JFileChooser dc = new JFileChooser(dflt);
        dc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        int selection = dc.showDialog(this, buttonText);
        if (selection == JFileChooser.APPROVE_OPTION) 
            return dc.getSelectedFile();
        else
            return null;
    }
	
    /*
     * universal open file method
     */
    private File chooseFile(File dflt, FileFilter pf, String buttonText) {
        final JFileChooser fc;
        
        // If the default is a file rather than a directory, get its parent directory.
        if (dflt != null && !dflt.isDirectory())
            dflt = dflt.getParentFile();

        fc = new JFileChooser(dflt);
        if (pf != null)
            fc.setFileFilter(pf);

        int returnVal = fc.showDialog(this, buttonText);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            view.showMessage("Selected " + file.getAbsolutePath());
            return file;
        }
        return null;
    }

    private File generateDebugFile() {
        // *** TODO ***
        return null;
    }

    private class LibraryListEditor
        extends JFrame {

        FileListPanel libList;

        public LibraryListEditor() {
            super("Select Libraries");

            GridBagLayout layout = new GridBagLayout();
            getContentPane().setLayout(layout);

            Box heading = Box.createHorizontalBox();
            heading.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

            JLabel label = new JLabel("Libraries", SwingConstants.LEFT);
            heading.add(label);
            heading.add(Box.createHorizontalGlue());

            JButton libButton = new JButton("Add");
            libButton.setToolTipText("Insert library after selection, or at top if none selected");
            libButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) { 
                        File dflt = libList.getSelection();
                        if (dflt == null) {
                            File planLoc = view.getPlan().getPlanFile();
                            if (planLoc != null)
                                dflt = planLoc.getParentFile();
                            else
                                dflt = new File(System.getenv("HOME"));
                        }
                        JFileChooser fc = new JFileChooser(dflt);
                        fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
                        fc.setFileFilter(planFilter);
                        int returnVal = fc.showDialog(LibraryListEditor.this, "Choose Library File");
                        if (returnVal == JFileChooser.APPROVE_OPTION) {
                            File lib = fc.getSelectedFile();
                            libList.insertAfterSelection(lib);
                            libList.setSelection(lib);
                            view.showMessage("Added Library File " + lib.getAbsolutePath());
                        }            	            	            	
                    }
                });
            heading.add(libButton);
            heading.add(Box.createHorizontalStrut(3));

            JButton removeButton = new JButton("Remove");
            removeButton.setToolTipText("Remove selected library");
            removeButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        if (libList.getSelectionIndex() >= 0) {
                            // Confirm that the user wants to clear Libraries
                            Object[] options = {"OK", "Cancel"};
                            int del =
                                JOptionPane.showOptionDialog(LibraryListEditor.this,
                                                             "Remove library" + libList.getSelection().toString() + "?",
                                                             "Confirm Remove Library",
                                                             JOptionPane.OK_CANCEL_OPTION,
                                                             JOptionPane.WARNING_MESSAGE,
                                                             null,
                                                             options,
                                                             options[1]);

                            if (del == 0)
                                libList.removeSelection();
                        }
                    }
                });
            heading.add(removeButton);
            heading.add(Box.createHorizontalStrut(3));
        
            JButton clearButton = new JButton("Clear");
            clearButton.setToolTipText("Clear libraries");
            clearButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        Object[] options = {"OK", "Cancel"};
                        int clear =
                            JOptionPane.showOptionDialog(LibraryListEditor.this,
                                                         "Really clear all libraries?",
                                                         "Confirm Clear Libraries",
                                                         JOptionPane.OK_CANCEL_OPTION,
                                                         JOptionPane.WARNING_MESSAGE,
                                                         null,
                                                         options,
                                                         options[1]);

                        if (clear == 0)
                            libList.clearFiles();
                    }
                });
            heading.add(clearButton);

            GridBagConstraints headingConstraints = new GridBagConstraints();
            headingConstraints.gridx = 1;
            headingConstraints.gridy = 0;
            headingConstraints.fill = HORIZONTAL;
            layout.setConstraints(heading, headingConstraints);
            getContentPane().add(heading);

            libList = new FileListPanel();
            GridBagConstraints libConstraints = new GridBagConstraints();
            libConstraints.gridx = 1;
            libConstraints.gridy = 1;
            libConstraints.weightx = 1.0;
            libConstraints.weighty = 1.0;
            libConstraints.fill = BOTH;
            layout.setConstraints(libList, libConstraints);
            getContentPane().add(libList);

            JButton cancelButton = new JButton("Cancel");
            cancelButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        setVisible(false);
                    }
                }
                );

            JButton createCFGButton = new JButton("OK");
            createCFGButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        libraryFiles = libList.getFiles();
                        setVisible(false);
                        refresh();
                    }
                });

            // Panel to hold buttons and file location message
            JPanel buttonPane = new JPanel();
            buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.LINE_AXIS));
            buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            buttonPane.add(Box.createHorizontalGlue());
            buttonPane.add(cancelButton);
            buttonPane.add(Box.createHorizontalStrut(3));
            buttonPane.add(createCFGButton);
            buttonPane.add(Box.createHorizontalGlue());

            GridBagConstraints buttonConstraints = new GridBagConstraints();
            buttonConstraints.gridx = 0;
            buttonConstraints.gridy = 2;
            buttonConstraints.gridwidth = REMAINDER;
            buttonConstraints.fill = HORIZONTAL;
            layout.setConstraints(buttonPane, buttonConstraints);
            getContentPane().add(buttonPane);

            // *** FIXME *** Make settable
            setPreferredSize(FILE_LIST_DIALOG_SIZE);
            setLocation(LIBRARY_DIALOG_LOC);
            pack();
        }

        // Update contents from current settings
        private void refreshFileList() {
            libList.setFiles(libraryFiles);
        }

        public void display() {
            setVisible(false);
            refreshFileList();
            setVisible(true);
        }
    }

    private class LibraryPathEditor
        extends JFrame {

        FileListPanel pathList;

        public LibraryPathEditor() {
            super("Select Library Path");

            GridBagLayout layout = new GridBagLayout();
            getContentPane().setLayout(layout);

            Box heading = Box.createHorizontalBox();
            heading.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

            JLabel label = new JLabel("Library Search Path", SwingConstants.LEFT);
            heading.add(label);
            heading.add(Box.createHorizontalGlue());
            JButton dirButton = new JButton("Add");
            dirButton.setToolTipText("Insert directory after selection, or at top if none selected");
            dirButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) { 
                        File dflt = pathList.getSelection();
                        if (dflt == null) {
                            File planLoc = view.getPlan().getPlanFile();
                            if (planLoc != null)
                                dflt = planLoc.getParentFile();
                            else
                                dflt = new File(System.getProperty("user.dir"));
                        }
                    
                        JFileChooser dc = new JFileChooser(dflt.getParentFile());
                        dc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                        dc.setSelectedFile(dflt);
                        if (dc.showDialog(LibraryPathEditor.this, "Add Directory") ==
                            JFileChooser.APPROVE_OPTION) {
                            File dir = dc.getSelectedFile();
                            pathList.insertAfterSelection(dir);
                            pathList.setSelection(dir);
                            view.showMessage("Added Library Directory " + dir.getAbsolutePath());
                        }
                    }
                }
                );
            heading.add(dirButton);
            heading.add(Box.createHorizontalStrut(3));

            JButton removeButton = new JButton("Remove");
            removeButton.setToolTipText("Remove selection from search path");
            removeButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        if (pathList.getSelectionIndex() >= 0) {
                            // Confirm that the user wants to clear Libraries
                            Object[] options = {"OK", "Cancel"};
                            int del =
                                JOptionPane.showOptionDialog(LibraryPathEditor.this,
                                                             "Remove " + pathList.getSelection().toString() + " from path?",
                                                             "Confirm Remove From Path",
                                                             JOptionPane.OK_CANCEL_OPTION,
                                                             JOptionPane.WARNING_MESSAGE,
                                                             null,
                                                             options,
                                                             options[1]);

                            if (del == 0)
                                pathList.removeSelection();
                        }
                    }
                }
                );
            heading.add(removeButton);
            heading.add(Box.createHorizontalStrut(3));

            JButton clearDirsButton = new JButton("Clear");
            clearDirsButton.setToolTipText("Clear library search path");
            clearDirsButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        Object[] options = {"OK", "Cancel"};
                        int clear =
                            JOptionPane.showOptionDialog(LibraryPathEditor.this,
                                                         "Are you sure you want to clear all Library directories?",
                                                         "Clear Path",
                                                         JOptionPane.OK_CANCEL_OPTION,
                                                         JOptionPane.WARNING_MESSAGE,
                                                         null,
                                                         options,
                                                         options[1]);

                        if (clear == 0)
                            pathList.clearFiles();
                    }
                }
                );
            heading.add(clearDirsButton);

            GridBagConstraints headingConstraints = new GridBagConstraints();
            headingConstraints.gridx = 0;
            headingConstraints.gridy = 0;
            headingConstraints.fill = HORIZONTAL;
            layout.setConstraints(heading, headingConstraints);
            getContentPane().add(heading);

            pathList = new FileListPanel();
            GridBagConstraints listConstraints = new GridBagConstraints();
            listConstraints.gridx = 0;
            listConstraints.gridy = 1;
            listConstraints.weightx = 1.0;
            listConstraints.weighty = 1.0;
            listConstraints.fill = BOTH;
            layout.setConstraints(pathList, listConstraints);
            getContentPane().add(pathList);

            JButton cancelButton = new JButton("Cancel");
            cancelButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        setVisible(false);
                    }
                }
                );

            JButton createCFGButton = new JButton("OK");
            createCFGButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent ev) {
                        libraryPath = pathList.getFiles();
                        setVisible(false);
                        refresh();
                    }
                });

            // Panel to hold buttons and file location message
            JPanel buttonPane = new JPanel();
            buttonPane.setLayout(new BoxLayout(buttonPane, BoxLayout.LINE_AXIS));
            buttonPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            buttonPane.add(Box.createHorizontalGlue());
            buttonPane.add(cancelButton);
            buttonPane.add(Box.createHorizontalStrut(3));
            buttonPane.add(createCFGButton);
            buttonPane.add(Box.createHorizontalGlue());

            GridBagConstraints buttonConstraints = new GridBagConstraints();
            buttonConstraints.gridx = 0;
            buttonConstraints.gridy = 2;
            buttonConstraints.gridwidth = REMAINDER;
            buttonConstraints.fill = HORIZONTAL;
            layout.setConstraints(buttonPane, buttonConstraints);
            getContentPane().add(buttonPane);

            // *** FIXME *** Make settable
            setPreferredSize(FILE_LIST_DIALOG_SIZE);
            setLocation(PATH_DIALOG_LOC);

            pack();
        }

        // Update contents from current settings
        private void refreshPathList() {
            pathList.setFiles(libraryPath);
        }

        public void display() {
            setVisible(false);
            refreshPathList();
            setVisible(true);
        }
    }
}

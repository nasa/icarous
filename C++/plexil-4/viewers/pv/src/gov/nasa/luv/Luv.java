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

import static java.lang.System.*; // ??

import java.awt.Container;
import java.awt.Color;
import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.print.PrinterException;
import static java.awt.BorderLayout.*;
import static java.awt.event.KeyEvent.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Semaphore;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;
import javax.swing.border.EmptyBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.tree.TreePath;
import static javax.swing.JFileChooser.APPROVE_OPTION;

import static gov.nasa.luv.Constants.*;

/**
 * The Luv class is the starting point of the Luv application and creates a 
 * containing frame for the Lightweight Universal Executive Viewer.
 */
public class Luv extends JFrame {

    //
    // Local constants
    //

    private static final String NAME = "Name";
    
    // Components of Luv window
    private JTextArea     console;
    private JScrollPane   debugScrollPane;

    private JMenu appMenu;
    private JMenu fileMenu;
    private JMenu editMenu;
    private JMenu debugMenu;

    private JMenuItem exitMenuItem;

    // instances to manage Luv features
    private StatusMessageHandler statusMessageHandler;
    private HideOrShowWindow hideOrShowWindow;
    private CreateCFGFileWindow createCFGFileWindow;
    private LuvPortGUI portGui;
    private RegexNodeFilter regexFilter;
    private int luvPrevPort = 0;

    //Gantt Viewer
    private OpenGanttViewer openGanttViewer;

    private Map<String, PlanView> planViews;

    // current working instance of luv
    private static Luv theLuv;

    /** Entry point for the Luv application. */
    public static void main(String[] args) {    	
        runApp(args);
    }

    /** Creates a new instance of the Luv application. */
    private static void runApp(String[] args) {
        // In MacOS, don't use system menu bar
        System.setProperty("apple.laf.useScreenMenuBar", "false");
        try {
            new Luv(args);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /** 
     * Constructs Luv and initializes the many features of the Luv application
     * such as creating the main viewing window, putting Luv in a 'start state'
     * and initializing a SocketServer to start listening for events from the 
     * Universal Executive. Starts socket server with requested port.
     */
    public Luv(String[] args) throws IOException
    {
        earlyInit(args);
        lateInit();

        // Now can start running stuff
        LuvSocketServer.startServer(Settings.instance().getPort());
        startState();

        // Display the plan if one was named on the command line
        // *** FIXME ***
        if (Settings.instance().getPlanSupplied()
            && Settings.instance().getPlanLocation().isFile()) {
            loadPlan(Settings.instance().getPlanLocation());
            readyState();

            // Go ahead and run it if requested
            // *** FIXME ***
            // if (Settings.instance().getAutoRun()) {
            //     if (!ExecutionHandler.instance().runExec()) {
            //         statusMessageHandler.showStatus("Stopped execution", Color.lightGray, 1000);
            //         readyState();
            //     }
            // }
        }
    }

    private void earlyInit(String[] cmdLineArgs) {
        theLuv = this;
        planViews = new HashMap<String, PlanView>();
        Settings.instance().load(); // load saved preferences
        Settings.instance().parseCommandOptions(cmdLineArgs);
        Settings.instance().save(); // save prefs

        // Build console now so we can see what else goes wrong.
        constructFrame();
    }

    private void lateInit()
    {
        // Create these on demand
        portGui = null;
        hideOrShowWindow = null;
        createCFGFileWindow = null;
        openGanttViewer = null;

        regexFilter = new RegexNodeFilter(true);
        regexFilter.extendedPlexilView();
        regexFilter.updateRegexList();
    }

    private void constructFrame() {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        setBackground(Settings.instance().getColor(PROP_WIN_BCLR));

        console = new javax.swing.JTextArea();
        console.setEditable(false);
        console.setFont(new Font("Monospaced", Font.PLAIN, 12));
        console.setLineWrap(true);
        console.setWrapStyleWord(true);

        debugScrollPane = new javax.swing.JScrollPane();
        debugScrollPane.setViewportView(console);
         
        getContentPane().add(debugScrollPane, CENTER);

        // create a menu bar
        createMenuBar();

        //allocate area for status bars
        JPanel infoBar = new JPanel();
        getContentPane().add(infoBar, SOUTH);
        GridBagLayout gridbag = new GridBagLayout();
        infoBar.setLayout(gridbag);
        GridBagConstraints c = new GridBagConstraints();
                
        // add the status bar
        statusMessageHandler = StatusMessageHandler.instance();
        final JLabel statusBar = statusMessageHandler.getStatusBar();
        infoBar.add(statusBar);
        c.fill = GridBagConstraints.HORIZONTAL;
        c.weightx = 1.0;
        gridbag.setConstraints(statusBar, c);
        
        // add the port message bar
        final JLabel portBar = statusMessageHandler.getPortBar();
        infoBar.add(portBar);
        c.weightx = 0.02;
        gridbag.setConstraints(portBar,c);

        setLocation(Settings.instance().getPoint(PROP_WIN_LOC));
        setPreferredSize(Settings.instance().getDimension(PROP_WIN_SIZE));

        setTitle("Plexil Viewer Console");
        pack();

        setVisible(true);

        PrintStream ds = new PrintStream(new TextAreaOutputStream(console), true);
        System.setErr(ds);
        System.setOut(ds);

        // save preferred window sizes when Luv application closes
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                Settings.instance().set(PROP_WIN_LOC, getLocation());
                Settings.instance().set(PROP_WIN_SIZE, getSize());
                Settings.instance().save();
            }
        });
    }

    private void createMenuBar() {
        JMenuBar menuBar = new JMenuBar();
        setJMenuBar(menuBar);
        appMenu = new JMenu("Viewer");
        menuBar.add(appMenu);
        appMenu.add(new LuvAction("About Plexil Viewer",
                                   "Show window with Plexil Viewer about information.") {
                public void actionPerformed(ActionEvent e) {
                    showAboutWindow();
                }
            });
        appMenu.add(new LuvAction("Change Server port",
                                  "Change viewer server listening port.",
                                  VK_P,
                                  ALT_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    getPortGUI().activate();
                }
            });
        exitMenuItem =
            appMenu.add(new LuvAction("Exit", "Terminate this program.", VK_ESCAPE) {
                    public void actionPerformed(ActionEvent e) {                  
                        System.exit(0);                    
                    }
                });
        
        fileMenu = new JMenu("File");
        menuBar.add(fileMenu);        
        
        fileMenu.add(new LuvAction("Open Plan",
                                   "View a PLEXIL plan.",
                                   VK_O,
                                   META_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    openPlanFileDialog();
                }
            });

        fileMenu.add(new JSeparator());
        fileMenu.add(new LuvAction("Save Output",
                                   "Save console output to a file.",
                                   VK_S,
                                   META_DOWN_MASK) {
                public void actionPerformed(ActionEvent actionEvent) {
                    saveConsoleToFileDialog();
                }
            });
        fileMenu.add(new LuvAction("Print",
                                   "Print contents of console window.",
                                   VK_P,
                                   META_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    try {
                        console.print(); // TODO: get fancier
                    }
                    catch (PrinterException pe) {
                        statusMessageHandler.displayErrorMessage(pe, "ERROR: exception occurred while printing console window");
                    }
                }
            });

        editMenu = new JMenu("Edit");
        menuBar.add(editMenu);
        editMenu.add(new LuvAction("Clear Console",
                                   "Clear all console output.",
                                   VK_DELETE,
                                   META_DOWN_MASK) {
                public void actionPerformed(ActionEvent actionEvent) {
                    console.setText("");
                }
            });
        editMenu.add(new LuvAction("Copy",
                                   "Copy selected console output to the clipboard.",
                                   VK_C,
                                   META_DOWN_MASK) {
                public void actionPerformed(ActionEvent actionEvent) {
                    console.copy();
                }
            });
        editMenu.add(new LuvAction("Copy All",
                                   "Copy all console output to the clipboard.",
                                   VK_C,
                                   META_DOWN_MASK | SHIFT_DOWN_MASK) {
                public void actionPerformed(ActionEvent actionEvent) {
                    console.selectAll();
                    console.copy();
                }
            });
                

        // *** TODO: Remove comments once replacements reimplemented on PlanView ***
        // viewMenu = new JMenu("View");
        // menuBar.add(viewMenu);
        // viewMenu.add(new LuvAction("Hide/Show Nodes...",
        //                            "Hide or Show specific nodes by full or partial name.",
        //                            VK_H,
        //                            META_DOWN_MASK) {

        //         public void actionPerformed(ActionEvent e) {
        //             getHideOrShowWindow().open();
        //         }
        //     }
        //              );
        // viewMenu.add(new LuvAction("Find...",
        //                            "Find node by name.",
        //                            VK_F,
        //                            META_DOWN_MASK) {
        //         public void actionPerformed(ActionEvent e) {
        //             FindWindow.open(Settings.instance().get(PROP_SEARCH_LIST));
        //         }
        //     }
        //              );
        // viewMenu.add(new JSeparator());
        // LuvAction extendedViewAction = new LuvAction("Switch to Core Plexil View",
        //                                    "Switches between Normal or Core Plexil views. Normal is the default.",
        //                                    VK_F8) {
        //         public void actionPerformed(ActionEvent e) {
        //             if (extendedViewOn) {
        //                 extendedViewAction.putValue(NAME, "Switch to Normal Plexil View");
        //                 // TODO
        //                 //getRegexNodeFilter().corePlexilView();
        //                 //viewHandler.refreshRegexView();
        //             } else {
        //                 extendedViewAction.putValue(NAME, "Switch to Core Plexil View");
        //                 //TODO
        //                 //getRegexNodeFilter().extendedPlexilView();
        //                 //viewHandler.refreshRegexView(); 
        //             }
        //             setExtendedViewOn(!extendedViewOn);
        //         }
        //     };
        // viewMenu.add(extendedViewAction);
        // viewMenu.add(new JSeparator());
        // viewMenu.add(new LuvAction("Show Gantt Viewer",
        //                            "Show viewer window with timeline/Gantt views of a plan post-execution.",
        //                            VK_V,
        //                            META_DOWN_MASK) {
        //         public void actionPerformed(ActionEvent e) {	
        //             getGanttViewer().openURL();
        //         }
        //     }
        //              );

        debugMenu = new JMenu("Debug");
        menuBar.add(debugMenu);
    }

    // *** TEMP ***
    public JTextArea getConsoleTextArea() {
        return console;
    }

    public PlanView getViewForNode(Node n) {
        Node r = n.getRootNode();
        if (r == null)
            return null;
        PlanView v = getPlanView(r.getNodeName());
        if (v == null
            || v.getPlan().getRootNode() != r)
            return null;
        return v;
    }

    public PlanView getPlanView(String name) {
        return planViews.get(name);
    }

    public Collection<PlanView> getAllPlanViews() {
        return planViews.values();
    }

    private void showAboutWindow() {
        String info =
            "Application:   PLEXIL " + Constants.PLEXIL_VERSION
            + " " + Constants.PLEXIL_COPYRIGHT
            + "Website:   "+ Constants.PLEXIL_WEBSITE + "\n"
            + "\nJava:        " + System.getProperty("java.version")
            + "; " + System.getProperty("java.vm.name")
            + " " + System.getProperty("java.vm.version")
            + "\nSystem:    " + System.getProperty("os.name")
            + " version " + System.getProperty("os.version")
            + " running on " + System.getProperty("os.arch")
            + "\nUserdir:    " + System.getProperty("user.dir") + "\n";

        ImageIcon icon = getIcon(ABOUT_LOGO);
        JOptionPane.showMessageDialog(theLuv,
                                      info,
                                      "About Plexil Viewer",
                                      JOptionPane.INFORMATION_MESSAGE,
                                      icon);
    }

    private void openPlanFileDialog() {
        File initialDir = null;
        File last = Settings.instance().getPlanLocation();
        if (last != null)
            initialDir = last.getParentFile();
        else {
            String cwd = System.getProperty("user.dir");
            if (cwd != null)
                initialDir = new File(cwd);
        }

        JFileChooser c = new JFileChooser(initialDir);
        FileNameExtensionFilter f =
            new FileNameExtensionFilter("PLEXIL plans", "plx");
        c.setFileFilter(f);
        c.setApproveButtonText("Load Plan");
        switch (c.showOpenDialog(Luv.this)) {
        case APPROVE_OPTION: {
            File p = c.getSelectedFile();
            statusMessageHandler.showStatus("Loading plan " + p, 1000);
            loadPlan(p);
            Settings.instance().setPlanLocation(p);
            return;
        }

        default:
            return;
        }
    }

    private void saveConsoleToFileDialog() {
        JFileChooser c = new JFileChooser(System.getProperty("user.dir")); // TODO: make fancier
        c.setApproveButtonText("Save");
        switch (c.showOpenDialog(Luv.this)) {
        case APPROVE_OPTION: {
            PrintStream s;
            try {
                s = new PrintStream(c.getSelectedFile());
            }
            catch (FileNotFoundException nf) {
                statusMessageHandler.displayErrorMessage(nf, "Unable to open file " + c.getSelectedFile());
                return;
            }
            catch (SecurityException se) {
                statusMessageHandler.displayErrorMessage(se, "Unable to write to file " + c.getSelectedFile());
                return;
            }
            s.print(console.getText());
            s.close();
            statusMessageHandler.showStatus("Saved console output to " + c.getSelectedFile(), 1000);
            return;
        }

        default:
            return;
        }
    }

    /** 
     * Redefines port argument based upon string argument.
     * Restarts listening server with new port. 
     * Sets port in settings.
     */
    public void changePort(int newPort) {
        if (LuvSocketServer.portFree(newPort)) {
            // Shut down old
            int oldPort = Settings.instance().getPort();
            try {
                LuvSocketServer.stopServer();
            } catch (Exception e) {
                statusMessageHandler.displayErrorMessage(e, "Error occured while stopping server on port " + oldPort);
                return;
            }

            // Launch new
            try {
                LuvSocketServer.startServer(newPort);
            } catch (Exception e) {
                statusMessageHandler.displayErrorMessage(e, "Error occured while starting server on port " + newPort);
            }
            Settings.instance().setPort(newPort);
            Settings.instance().setPortSupplied(true); // because this code only called as a result of user action
            Settings.instance().save();
        }
    }

    // Called when the user selects or reloads a plan
    // TODO: run as a SwingWorker
    public void loadPlan(File f) {
        Plan m = FileHandler.readPlan(f);
        if (m != null)
            handleNewPlan(m);
        readyState();
    }

    /** Handles the Plexil plan being loaded into the Luv application. 
     *  Called from PlexilPlanHandler.endElement(), which will be invoked 
     *  by both Luv (directly) and the Universal Executive (via the Luv listener stream).
     *
     * @param plan the Plexil plan to be loaded into the Luv application
     */

    // *** N.B. Called in two circumstances:
    //  1. User has just requested loading or reloading a plan.
    //  2. Exec has just transmitted the plan it is about to execute.
    // If the Exec is running as a slave of Luv, both events will happen, in that order. In different threads.

    // FIXME: Move model mangling to RootModel class

    public void handleNewPlan(Plan plan) {
        String name = plan.getName();
        Plan existing = RootModel.getPlan(name);
        PlanView view = getPlanView(name);
        if (existing != null) {
            if (existing.equals(plan)) {
                // same 'plan' already loaded, so use it, 
                // but refresh view
                if (view != null) {
                    //System.out.println("handleNewPlan: Received same plan from exec, refreshing it");
                    view.resetEvent();
                    return;
                }
                // else fall through and construct new view
            }
            else {
                // plan has changed
                //System.out.println("handleNewPlan: Received plan has same name but differs, loading new");
                RootModel.removePlan(existing);
                RootModel.addPlan(plan);
                view.newPlanEvent(plan);
                return;
            }
        }
        else {
            RootModel.addPlan(plan); // totally new
        }

        // At this point either we have a new plan,
        // or an old plan with no view that is about to run.
        //System.out.println("handleNewPlan: Constructing new PlanView");
        view = new PlanView(plan); // FIXME
        planViews.put(plan.getRootNode().getNodeName(), view);
        view.setVisible(true);
    }

    // Bookkeeping for when a window is disposed
    public void deleteView(PlanView view) {
        Plan p = view.getPlan();
        if (p == null)
            return;
        planViews.remove(p.getName());
        RootModel.removePlan(p);
    }

    // Called when a plan is received from the Exec.
    public void newPlanFromExec(Plan plan) {
        handleNewPlan(plan);

        // *** FIXME ***
        // readyState();
        // preExecutionState();
        // executionState();

        // Determine if the Luv Viewer should pause before executing.
        // FIXME: move logic elsewhere
        // if (ExecutionHandler.instance().isExecuting()
        //     && Settings.instance().blocksExec())
        //     pausedState();
    }

    /** Returns the current instance of the Luv application. 
     *  @return the current instance of the Luv application */
    public static Luv getLuv() {
        return theLuv;
    }
    
    /** Gantt Viewer
     *  Opens the Gantt Viewer in a browser.
     *  @return OpenGanttViewer */
    public OpenGanttViewer getGanttViewer() {
        if (openGanttViewer == null)
            openGanttViewer = new OpenGanttViewer();
        return openGanttViewer;
    }

    /** Returns the current instance of the Luv HideOrShowWindow.
     *  @return the current instance of the Luv HideOrShowWindow */
    public HideOrShowWindow getHideOrShowWindow()
    {
        if (hideOrShowWindow == null)
            hideOrShowWindow =
                new HideOrShowWindow(Settings.instance().getStringList(PROP_HIDE_SHOW_LIST));
        return hideOrShowWindow;
    }
    
    /** Returns the current instance of the Listener Port GUI.
     *  @return the current instance of the Listener Port GUI */
    public LuvPortGUI getPortGUI() {
        if (portGui == null)
            portGui = new LuvPortGUI();
        return portGui;
    }

    /** Returns the current instance of the Luv DebugCFGWindow.
     *  @return the current instance of the Luv DebugCFGWindow */
    public CreateCFGFileWindow getCreateCFGFileWindow() {
        if (createCFGFileWindow == null)
            createCFGFileWindow = new CreateCFGFileWindow();
        return createCFGFileWindow;
    }

    /** Returns the current instance of the Luv RegexNodeFilter.
     *  @return the current instance of the Luv RegexNodeFilter */
    public RegexNodeFilter getRegexNodeFilter() {
        return regexFilter;
    }

    public AppType getAppMode() {
        return Settings.instance().getAppMode();
    }

    // Used by ExecSelectDialog, others?
    public void setAppMode(AppType mode) {
        if (mode == Settings.instance().getAppMode())
            return;
        // *** TODO: actually switch mode ***
    }
    
    public int getPort() {
    	return Settings.instance().getPort();
    }

    public int getPrevPort() {
    	return luvPrevPort;
    }
    
    /** Returns whether viewer invokes static checker.
     *  @return the current instance of checkPlan */
    public boolean checkPlan() {
        return Settings.instance().checkPlan();
    }
    
    /** Sets the flag that indicates whether the application is currently
     *  statically checking the plan.
     *  @param value sets the flag that indicates plan check
     */
    public void setCheckPlan(boolean value) {
        Settings.instance().setCheckPlan(value);
    }

    public void setBreaksAllowed(boolean value) {
        Settings.instance().setBlocksExec(value);
        for (PlanView v : getAllPlanViews())
            v.setBreaksEnabled(value);
    }

    public void toggleBreaksEnabled() {
        if (!ExecutionHandler.instance().isExecuting())
            setBreaksAllowed(!Settings.instance().blocksExec());
    }

    //
    // State management
    //

	/**
	 * Sets the Luv application to a Start State, this occurs as when the Luv
	 * application opens for the first time.
     * @note Should only be called from within this class.
	 */
	private void startState()
    {
        setLocation(theLuv.getLocation());
        setPreferredSize(theLuv.getSize());
		disableAllMenus();
		exitMenuItem.setEnabled(true);
		fileMenu.setEnabled(true);
		debugMenu.setEnabled(true);
        setVisible(true);
	}

	/**
	 * Sets the Luv application to a Ready State.
	 */
	public void readyState()
    {
		// set only certain luv viewer variables
		FileHandler.instance().setStopSearchForMissingLibs(false);
		exitMenuItem.setEnabled(true);
		fileMenu.setEnabled(true);
		debugMenu.setEnabled(true);
	}

	/**
	 * Sets the Luv application to a Finished Execution State and occurs when
	 * EOF on the LuvListener stream is received.
	 */
	public void finishedExecutionState() {
		FileHandler.instance().setStopSearchForMissingLibs(false);

		exitMenuItem.setEnabled(true);
		fileMenu.setEnabled(true);

		// updateBlockingMenuItems(); // *** FIXME ***

		debugMenu.setEnabled(true);

		statusMessageHandler.showChangeOnPort("Listening on port " + Settings.instance().getPort());
	}

    // Utility used in startState()
	private void disableAllMenus()
    {
		exitMenuItem.setEnabled(false);
		fileMenu.setEnabled(false);

		debugMenu.setEnabled(false);
	}

}

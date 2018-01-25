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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle; // *** TEMP? ***
import java.awt.Window; // *** TEMP? ***
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import static java.awt.event.InputEvent.*;
import static java.awt.event.KeyEvent.*;

import java.lang.reflect.InvocationTargetException;

import java.util.Enumeration;
import java.util.EnumMap;
import java.util.Map;
import java.util.Queue;
import java.util.Set;
import java.util.TreeSet;
import java.util.Vector;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Semaphore;

import javax.swing.AbstractAction;
import javax.swing.Icon;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTable;
import javax.swing.JTextArea; // *** TEMP ***
import javax.swing.RepaintManager;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;
import javax.swing.border.EmptyBorder;
import javax.swing.border.LineBorder;
import javax.swing.event.TableModelEvent;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;

import static javax.swing.WindowConstants.*;

import org.netbeans.swing.outline.DefaultOutlineModel;
import org.netbeans.swing.outline.Outline;
import org.netbeans.swing.outline.OutlineModel;
import org.netbeans.swing.outline.RenderDataProvider;
import org.netbeans.swing.outline.RowModel;
import org.netbeans.swing.outline.TreePathSupport;

import static gov.nasa.luv.Constants.*;

import static gov.nasa.luv.PlexilSchema.*;

/** A standalone plan viewer window using the NetBeans Outline class. */

public class PlanView
    extends JFrame {

    private static final String NAME_COL_NAME = "Name";

    private static final int STATE_COL_NUM        = 0;
    private static final int OUTCOME_COL_NUM      = 1;
    private static final int FAILURE_TYPE_COL_NUM = 2;

    private Plan plan;

    private JScrollPane scrollPane;
    private Outline outline;
    private JLabel messageBar;
    private JLabel statusBar;

    private ExecSelectDialog configDialog;

    private Node.ChangeListener nodeChangeListener;

    //
    // Menus and menu items
    //

    private JMenuBar menuBar;
    
    private JMenu fileMenu;
    private JMenuItem configItem;
    private JMenuItem linkItem;
    private JMenuItem reloadItem;

    private JMenu runMenu;
    private JMenuItem allowBreaksItem;
    private JMenuItem execItem;
    private JMenuItem pauseExecutionItem;
    private JMenuItem resetItem;
    private JMenuItem stepExecutionItem;
    private JMenuItem removeBPsItem;

    private JMenu viewMenu;

    // State
    private OutlineModel model;
    private DefaultTreeModel treeModel;
    private Set<LuvBreakPoint> breakPointSet;
    private boolean executionPaused;

    // Display event queue
    private Queue<PlanEvent> eventQueue;
    private Semaphore displaySem;

    public PlanView(Plan p) {
        super();

        plan = null;
        executionPaused = false;
        configDialog = null;
        breakPointSet = new TreeSet<LuvBreakPoint>();
        eventQueue = new ConcurrentLinkedQueue<PlanEvent>();
        displaySem = new Semaphore(1);
        nodeChangeListener = makeNodeChangeListener();
        constructFrame();
        setPlan(p);
        String displayName =
            p.getPlanFile() != null
            ? p.getPlanFile().toString()
            : p.getName();
        readyState("Loaded " + displayName, Color.GREEN.darker());
    }

    private void constructFrame() {
        addWindowListener(makeWindowListener());

        setDefaultCloseOperation(DISPOSE_ON_CLOSE);

        constructMenuBar();

        JPanel panel = new JPanel(new BorderLayout());
        panel.setOpaque(true);
        setContentPane(panel);
        Settings s = Settings.instance();
        Color bgColor = s.getColor(PROP_WIN_BCLR);
        panel.setPreferredSize(new Dimension(1000, 500)); // *** FIXME *** parameterize and save in settings
        panel.setBackground(bgColor);

        // Build outline
        outline = new Outline();
        outline.setFocusable(false);
        outline.setRenderDataProvider(new PlanRenderDataProvider());
        outline.setDefaultRenderer(NodeState.class, new NodeStateRenderer());
        outline.setDefaultRenderer(NodeOutcome.class, new NodeOutcomeRenderer());
        outline.setDefaultRenderer(NodeFailureType.class, new NodeFailureTypeRenderer());

        treeModel = new DefaultTreeModel(null, true);
        model =
            DefaultOutlineModel.createOutlineModel(treeModel,
                                                   new PlanRowModel(),
                                                   true, /* isLargeModel */
                                                   NAME_COL_NAME);
        outline.setModel(model);

        outline.addMouseListener(new MouseAdapter() {
                @Override
                public void mousePressed(MouseEvent e) {
                    if (e.isPopupTrigger())
                        handlePopupEvent(e);
                }
                @Override
                public void mouseClicked(MouseEvent e) {
                    if (e.getClickCount() == 2)
                        handleClickEvent(e);
                }
            });

        // set preferred column widths
        // TODO: set widths based on longest strings per column
        Enumeration<TableColumn> columns = outline.getColumnModel().getColumns();
        columns.nextElement().setPreferredWidth(1000 - 150 - 100 - 230); // tree
        columns.nextElement().setPreferredWidth(150); // state
        columns.nextElement().setPreferredWidth(100); // outcome
        columns.nextElement().setPreferredWidth(230); // failure type
        
        scrollPane = new JScrollPane(outline);
        scrollPane.setBackground(bgColor);
        panel.add(scrollPane, BorderLayout.CENTER);

        // Build local message area

        JPanel infoBar = new JPanel(new BorderLayout());
        infoBar.setFocusable(false);
        infoBar.setBackground(Settings.instance().getColor(PROP_WIN_BCLR));
        infoBar.setBorder(new LineBorder(Color.GRAY, 2));
        messageBar = new JLabel(" ");
        messageBar.setFocusable(false);
        Font infoFont = messageBar.getFont().deriveFont(Font.PLAIN, 12.0f);
        messageBar.setFont(infoFont);
        messageBar.setBorder(new EmptyBorder(2, 2, 2, 2));        
        messageBar.setHorizontalAlignment(SwingConstants.LEFT);
        infoBar.add(messageBar, BorderLayout.WEST);

        // TODO? set fixed width for status bar based on width of longest string
        statusBar = new JLabel(" ");
        statusBar.setFocusable(false);
        statusBar.setFont(infoFont);
        statusBar.setBorder(new EmptyBorder(2, 2, 2, 2));        
        statusBar.setHorizontalAlignment(SwingConstants.RIGHT);
        infoBar.add(statusBar, BorderLayout.EAST);
        panel.add(infoBar, BorderLayout.SOUTH);

        pack();
    }
    
    private void constructMenuBar() {
        menuBar = new JMenuBar();

        // File menu
        fileMenu = new JMenu("File");
        configItem = fileMenu.add(new LuvAction("Plan Settings",
                                                "Configure execution settings for this plan.",
                                                VK_COMMA,
                                                META_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    if (configDialog == null)
                        configDialog = new ExecSelectDialog(PlanView.this);
                    configDialog.activate();
                }
            });
        linkItem = fileMenu.add(new LuvAction("Link Libraries",
                                              "Include expansions of library nodes in the display.",
                                              VK_L,
                                              META_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    linkLibraries();
                }
            });
        reloadItem = fileMenu.add(new LuvAction("Reload",
                                                "Reload this plan from its file.",
                                                VK_R,
                                                META_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    reload();
                }
            });
        menuBar.add(fileMenu);

        // Run menu
        runMenu = new JMenu("Run");
        allowBreaksItem = runMenu.add(new LuvAction("Enable Breaks",
                                                    "Toggle whether breakpoints, pausing, stepping are enabled or disabled.",
                                                    VK_F5) {
                public void actionPerformed(ActionEvent e) {
                    Luv.getLuv().toggleBreaksEnabled();
                }
            });
        removeBPsItem = runMenu.add(new LuvAction("Remove All Breakpoints",
                                                  "Clear all breakpoints in this plan",
                                                  VK_DOLLAR) {// *** TODO: pick better shortcut ***
                public void actionPerformed(ActionEvent e) {
                    removeAllBreakPoints();
                }
            });

        runMenu.addSeparator();
        pauseExecutionItem = runMenu.add(new LuvAction("Pause execution",
                                                       "Pause or resume an executing plan, if breaks are enabled.",
                                                       VK_ENTER) {
                public void actionPerformed(ActionEvent e) {
                    pauseOrResume();
                }
            });
        stepExecutionItem = runMenu.add(new LuvAction("Step execution",
                                                      "Step a plan through individual state transitions.",
                                                      VK_SPACE) {
                public void actionPerformed(ActionEvent e) {
                    stepPlan();
                }
            });

        runMenu.addSeparator();
        execItem = runMenu.add(new LuvAction("Execute Plan",
                                             "Execute this plan.",
                                             VK_F6) {

                public void actionPerformed(ActionEvent e) {
                    startPlan();
                }
            });

        runMenu.addSeparator();
        resetItem = runMenu.add(new LuvAction("Reset plan",
                                              "Halt execution and reset this plan.",
                                              VK_F7) { // FIXME: better accelerator key
                public void actionPerformed(ActionEvent e) {
                    reset();
                }
            });
        menuBar.add(runMenu);

        // View menu
        viewMenu = new JMenu("View");
        viewMenu.add(new LuvAction("Expand All",
                                   "Expand all list and library call nodes.",
                                   VK_PLUS, SHIFT_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    expandAllRecursively(new TreePath(plan.getRootNode()));
                }
            });
        viewMenu.add(new LuvAction("Expand List Nodes",
                                   "Expand all list nodes.",
                                   VK_EQUALS) {
                public void actionPerformed(ActionEvent e) {
                    expandListNodes(new TreePath(plan.getRootNode()));
                }
            });
        viewMenu.add(new LuvAction("Collapse All",
                                   "Collapse all list and library call nodes.",
                                   VK_UNDERSCORE, SHIFT_DOWN_MASK) {
                public void actionPerformed(ActionEvent e) {
                    collapseAllRecursively(new TreePath(plan.getRootNode()));
                }
            });
        viewMenu.add(new LuvAction("Collapse List Nodes",
                                   "Collapse all list nodes.",
                                   VK_MINUS) {
                public void actionPerformed(ActionEvent e) {
                    collapseListNodes(new TreePath(plan.getRootNode()));
                }
            });
        viewMenu.add(new JSeparator());
        viewMenu.add(new LuvAction("View Source File",
                                   "Display source file for this plan.",
                                   VK_F9) {
                public void actionPerformed(ActionEvent e) {
                    // TODO
                }
            });
        menuBar.add(viewMenu);

        // TODO more

        menuBar.setRequestFocusEnabled(true);
        setJMenuBar(menuBar);
    }

    public void unsetPlan() {
        if (plan == null)
            return;
            
        // TODO: remove from root model?
        plan.getRootNode().removeChangeListenerFromAll(nodeChangeListener);
        plan = null;
    }

    public void setPlan(Plan p) {
        unsetPlan();

        plan = p;

        setTitle("Plan View - " + plan.getName());
        plan.getRootNode().addChangeListenerToAll(nodeChangeListener);

        treeModel.setRoot(plan.getRootNode());
        outline.setRootVisible(true);
        linkItem.setEnabled(plan.hasLibraryCalls());
    }

    private WindowListener makeWindowListener() {
        return new WindowAdapter() {
            // when dispose called
            @Override
            public void windowClosed(WindowEvent e) {
                Luv.getLuv().deleteView(PlanView.this);
                unsetPlan();
            }

            // when closed from system menu - per dox:
            // "If the program does not explicitly hide or
            //  dispose the window while processing this event,
            //  the window close operation will be cancelled."
            @Override
            public void windowClosing(WindowEvent e) {
                dispose();
            }

            @Override
            public void windowGainedFocus(WindowEvent e) {
                menuBar.requestFocusInWindow();
            }
        };

    }

    private Node.ChangeListener makeNodeChangeListener() {
        return new Node.ChangeAdapter() {
            @Override
            public void stateTransition(Node node,
                                        NodeState state,
                                        NodeOutcome outcome,
                                        NodeFailureType failure,
                                        Map<Condition, String> conditions) {
                int row = model.getLayout().getRowForPath(node.getTreePath());
                if (row < 0) {
                    // node not visible
                    System.out.println("stateTransition: Node " + node.getNodeName() + " not visible, tree path "
                                       + node.getTreePath());
                    return;
                }
                
                // Call the table setter functions to properly record the update event.
                model.setValueAt(state, row, STATE_COL_NUM + 1);
                if (outcome != null) 
                    model.setValueAt(outcome, row, OUTCOME_COL_NUM + 1);
                if (failure != null)
                    model.setValueAt(failure, row, FAILURE_TYPE_COL_NUM + 1);
            }

            @Override
            public void addBreakPoint(Node node, LuvBreakPoint bp) {
                newBreakPoint(bp);
            }

            @Override
            public void removeBreakPoint(Node node, LuvBreakPoint bp) {
                deleteBreakPoint(bp);
            }
        };
    }

    // TODO: run in SwingWorker
    private void linkLibraries() {
        if (!plan.hasLibraryCalls()) {
            showMessage("No library calls to link.");
            linkItem.setEnabled(false);
            return;
        }
        else if (!plan.hasUnresolvedLibraryCalls()) {
            showMessage("Libraries already linked.");
            linkItem.setEnabled(false);
            return;
        }
        else {
            boolean allFound = plan.resolveLibraryCalls();
            treeModel.reload();
            if (allFound) {
                showMessage("All library nodes linked.", Color.GREEN.darker());
                linkItem.setEnabled(false);
            }
            else // TODO: try to locate missing libraries
                showMessage("Linking incomplete; some libraries could not be located.", Color.YELLOW);
        }
    }

    // Runs in AWT event thread
    private void reload() {
        if (plan.getPlanFile() == null) {
            reset();
            readyState("Can't reload plan without a plan file", Color.RED);
            return;
        }

        SwingWorker<Integer, Object> worker =
            new SwingWorker<Integer, Object>() {

                private static final int LOAD_FAILED = -1;
                private static final int NO_CHANGE = 0;
                private static final int NEW_PLAN = 1;
                
                @Override
                public Integer doInBackground() {
                    Plan newPlan = FileHandler.readPlan(plan.getPlanFile());
                    if (newPlan == null)
                        return LOAD_FAILED; // couldn't load
                    else if (newPlan.equals(plan) && !plan.hasLibraryCalls())
                        return NO_CHANGE; // no change
                    else {
                        Plan oldPlan = plan;
                        unsetPlan();
                        oldPlan.merge(newPlan);
                        setPlan(oldPlan);
                        return NEW_PLAN;
                    }
                }

                @Override
                protected void done() {
                    int result = -1;
                    try {
                        result = get().intValue();
                    }
                    catch (InterruptedException i) {
                        // punt for now
                    }
                    catch (ExecutionException e) {
                        reset();
                        showErrorMessage(e, "Reloading " + plan.getPlanFile() + " failed");
                        readyState("Reloading " + plan.getPlanFile() + " failed", Color.RED);
                    }

                    reset(); // can't avoid it!

                    switch (result) {
                    case NO_CHANGE:
                        linkItem.setEnabled(plan.hasUnresolvedLibraryCalls());
                        readyState("Plan " + plan.getPlanFile() + " is unchanged");
                        return;

                    case NEW_PLAN:
                        repaint(); // redundant?
                        linkItem.setEnabled(plan.hasLibraryCalls());
                        readyState("Reloaded " + plan.getPlanFile(), Color.GREEN.darker());
                        return;

                    default:
                        readyState("Unable to reload plan file " + plan.getPlanFile(), Color.RED);
                        return;
                    }
                }
            };
        worker.execute();
    }

    private void expandAll() {
        expandAllRecursively(new TreePath(plan.getRootNode()));
    }

    // TODO ensure these are correct
    private void expandAllRecursively(TreePath p) {
        TreeNode n = (TreeNode) p.getLastPathComponent();
        if (!n.isLeaf()) {
            if (!outline.isExpanded(p))
                outline.expandPath(p);
            Enumeration e = n.children();
            while (e.hasMoreElements())
                expandAllRecursively(p.pathByAddingChild(e.nextElement()));
        }
    }

    private void expandLibraryCalls(TreePath p) {
        Node n = (Node) p.getLastPathComponent();
        if (!n.isLeaf()) {
            if (LIBRARYNODECALL.equals(n.getType())
                && !outline.isExpanded(p))
                outline.expandPath(p);
            Enumeration e = n.children();
            while (e.hasMoreElements())
                expandLibraryCalls(p.pathByAddingChild(e.nextElement()));
        }
    }

    private void expandListNodes(TreePath p) {
        Node n = (Node) p.getLastPathComponent();
        if (!n.isLeaf()) {
            if (NODELIST.equals(n.getType())
                && !outline.isExpanded(p))
                outline.expandPath(p);
            Enumeration e = n.children();
            while (e.hasMoreElements())
                expandListNodes(p.pathByAddingChild(e.nextElement()));
        }
        readyState("Expanded List nodes");
    }

    private void collapseAllRecursively(TreePath p) {
        TreeNode n = (TreeNode) p.getLastPathComponent();
        if (!n.isLeaf()) {
            Enumeration e = n.children();
            while (e.hasMoreElements())
                collapseAllRecursively(p.pathByAddingChild(e.nextElement()));
            if (outline.isExpanded(p))
                outline.collapsePath(p);
        }
    }

    private void collapseLibraryCalls(TreePath p) {
        Node n = (Node) p.getLastPathComponent();
        if (!n.isLeaf()) {
            Enumeration e = n.children();
            while (e.hasMoreElements())
                collapseLibraryCalls(p.pathByAddingChild(e.nextElement()));
            if (LIBRARYNODECALL.equals(n.getType())
                && outline.isExpanded(p))
                outline.collapsePath(p);
        }
    }

    private void collapseListNodes(TreePath p) {
        Node n = (Node) p.getLastPathComponent();
        if (!n.isLeaf()) {
            Enumeration e = n.children();
            while (e.hasMoreElements())
                collapseListNodes(p.pathByAddingChild(e.nextElement()));
            if (NODELIST.equals(n.getType())
                && outline.isExpanded(p))
                outline.collapsePath(p);
        }
        readyState("Collapsed List nodes");
    }

    public Plan getPlan() {
        return plan;
    }

    private void handlePopupEvent(MouseEvent e) {
        e.consume();
        if (Settings.instance().blocksExec()) {          
           TreePath nodePath = outline.getClosestPathForLocation(e.getX(), e.getY());
           if (nodePath == null) {
               // *** TEMP ***
               System.out.println("handlePopupEvent: nodePath is null, event " + e);
               return;
           }
           Object o = nodePath.getLastPathComponent();
           if (!(o instanceof Node)) {
               // *** TEMP ***
               System.out.println("handlePopupEvent: not a Node, path is " + nodePath);
               return;
           }
           JPopupMenu popup = constructNodePopupBreakPointMenu((Node) o);
           popup.show(e.getComponent(), e.getX(), e.getY());
        }
    }

    private void handleClickEvent(MouseEvent e) {
        e.consume();

        TreePath nodePath = outline.getClosestPathForLocation(e.getX(), e.getY());
        if (nodePath == null) {
            // *** TEMP ***
            System.out.println("handleClickEvent: nodePath is null, event " + e);
            return;
        }
        Node node = (Node) nodePath.getLastPathComponent();
        if (node.hasConditions() || node.hasVariables() || node.hasAction())
            NodeInfoWindow.open(node);
        else
            showMessage("No additional information is available for " + node.getNodeName());
    }

    // *** FIXME: Scrutinize and streamline ***
    private JPopupMenu constructNodePopupBreakPointMenu(final Node node)
    {
        // get the node name
        final String name = node.getNodeName();

        // construct the node popup menu 
        JPopupMenu popup = new JPopupMenu("Breakpoint Menu - " + name);

        // get the break points for this node       
        // add node state change breakpoint
        popup.add(new LuvAction("Add State Change Breakpoint",
                                "Break whenever " + name + " changes state.") {
                public void actionPerformed(ActionEvent e) {
                    node.createChangeBreakpoint();
                }
            });
         
        // add target state break points menu
        JMenu stateMenu = new JMenu("Add State Breakpoint");
        stateMenu.setToolTipText(
                                 "Break when " + name + " reaches a specific state.");
        popup.add(stateMenu);
        for (final NodeState state: NodeState.values())
            stateMenu.add(new LuvAction(state.toString(),
                                        "Break when " + name + 
                                        " reaches the " + state.toString() + " state.") {
                    public void actionPerformed(ActionEvent e) {
                        node.createNodeStateValueBreakpoint(state);
                    }
                });

        // add target outcome break points menu
        JMenu outcomeMenu = new JMenu("Add Outcome Breakpoint");
        outcomeMenu.setToolTipText("Break when " + name + " reaches a specific outcome.");
        popup.add(outcomeMenu);
        for (final NodeOutcome outcome: NodeOutcome.values())
            outcomeMenu.add(new LuvAction(outcome.toString(),
                                          "Break when " + name + 
                                          " reaches the " + outcome.toString() + " outcome.") {
                    public void actionPerformed(ActionEvent e) {
                        node.createNodeOutcomeValueBreakpoint(outcome);
                    }
                }); 

        // add target failure type break points menu
        JMenu failureTypeMenu = new JMenu("Add Failure Type Breakpoint");
        failureTypeMenu.setToolTipText("Break when " + name + " reaches a specific failure type.");
        popup.add(failureTypeMenu);

        for (final NodeFailureType failureType: NodeFailureType.values())
            failureTypeMenu.add(new LuvAction(
                                              failureType.toString(),
                                              "Break when " + name + 
                                              " reaches the " + failureType.toString() +
                                              " failure type.") {
                    public void actionPerformed(ActionEvent e) {
                        node.createNodeFailureValueBreakpoint(failureType);
                    }
                }); 

        // if Add enable/disable & remove item for each breakpoint
        final Vector<LuvBreakPoint> bps = node.getBreakPoints();
        if (!bps.isEmpty()) {
            // add the breakpoints
            popup.add(new JSeparator());
            for (final LuvBreakPoint bp: bps) {
                String action = bp.isEnabled() ? "Disable" : "Enable";
                popup.add(new LuvAction(action + " " + bp,
                                        action + " the breakpoint " + 
                                        bp + ".") {
                        public void actionPerformed(ActionEvent e) {
                            bp.setEnabled(!bp.isEnabled());
                            // TODO Highlight node by setting appropriately colored border
                            repaint(); // remove when highlighting added
                        }
                    }); 
            }

            // add the breakpoints
            popup.add(new JSeparator());
            for (final LuvBreakPoint bp2: bps) {
                popup.add(new LuvAction("Remove " + bp2,
                                        "Permanently remove the breakpoint " + 
                                        bp2 + ".") {
                        public void actionPerformed(ActionEvent e) {
                            bp2.unregister();
                        }
                    }); 
            }
        }

        // if there are multiple breakpoints, add a remove all item
        if (bps.size() > 1) {
            // add the remove all action           
            popup.add(new JSeparator());
            popup.add(new LuvAction("Remove All Breakpoints On Node",
                                    "Permanently remove all breakpoints from " + name + ".") {
                    public void actionPerformed(ActionEvent e) {
                        for (final LuvBreakPoint bp3: new Vector<LuvBreakPoint>(bps)) {
                            bp3.unregister();
                        }
                    }
                });
        }
               
        // return our freshly created popup menu
        return popup;
    }

    //
    // User messaging
    //

    public void showStatus(String s) {
        showStatus(s, Settings.instance().getColor(PROP_WIN_FCLR));
    }

    public void showStatus(String s, Color c) {
        statusBar.setForeground(c);
        statusBar.setText(s);
    }

    public void showMessage(String s) {
        showMessage(s, Settings.instance().getColor(PROP_WIN_FCLR));
    }

    public void showMessage(String s, Color c) {
        messageBar.setForeground(c);
        messageBar.setText(s);
    }

    public void showErrorMessage(Exception e, String msg) {
        if (e != null) {
            JOptionPane.showMessageDialog(this, 
                                          msg + ".\nPlease see Viewer console.", 
                                          "Error", 
                                          JOptionPane.ERROR_MESSAGE);
            System.out.println(msg + "\n" + e.getMessage());
            e.printStackTrace(System.out);
            // make console visible
            Luv.getLuv().setVisible(true);
            Luv.getLuv().toFront();
        }
        else {
            JOptionPane.showMessageDialog(this,
                                          msg, 
                                          "Error", 
                                          JOptionPane.ERROR_MESSAGE);

            System.out.println(msg);
        }
        showMessage(msg, Color.RED);
    }

    public void showWarningDialog(String title, String msg) {
        JOptionPane.showMessageDialog(this,
                                      msg,
                                      title,
                                      JOptionPane.WARNING_MESSAGE);
        System.out.println("WARNING: " + msg);
    }

    public void showInfoDialog(String title, String msg) {
        JOptionPane.showMessageDialog(this,
                                      msg,
                                      title,
                                      JOptionPane.INFORMATION_MESSAGE);
        System.out.println("INFO: " + msg);
    }

    //* Reset the plan to its start state.
    public void reset() {
        if (ExecutionHandler.instance().getPlanView() == this
            && ExecutionHandler.instance().isExecuting()) {
            ExecutionHandler.instance().stop();
        }
        resetView();
    }

    //* Reset the plan display to its start state.
    // Should only be executed on AWT event thread.
    private void resetView() {
        plan.getRootNode().reset();
        outline.tableChanged(new TableModelEvent(model, 0, model.getLayout().getRowCount() - 1));
    }

    //
    // Event queue
    //

    public void resetEvent() {
        eventQueue.add(new ResetEvent());
        startEventProcessing();
    }

    public void newPlanEvent(Plan p) {
        eventQueue.add(new NewPlanEvent(p));
        startEventProcessing();
    }

    public void nodeStateEvent(Vector<String> path,
                               NodeState state,
                               NodeOutcome outcome,
                               NodeFailureType failureType,
                               Map<Condition, String> conditions) {
        Node n = plan.getRootNode().getNode(path.toArray(new String[path.size()]));
        if (n == null)
            return; // couldn't find node, ignore

        NodeStateEvent e =
            new NodeStateEvent(n, state, outcome, failureType, conditions);
        // *** TODO: EVALUATE BREAKPOINTS HERE ***
        eventQueue.add(e);
        startEventProcessing();
    }

    public void assignmentEvent(Vector<String> path,
                                String var,
                                String val) {
        Node n = plan.getRootNode().getNode(path.toArray(new String[path.size()]));
        if (n == null)
            return; // couldn't find node, ignore

        AssignmentEvent a = new AssignmentEvent(n, var, val);
        // TODO: could evaluate assignment breakpoints here
        eventQueue.add(a);
        startEventProcessing();
    }

    // TODO: return immediately if view not ready
    private void startEventProcessing() {
        // Don't bother if already scheduled but not yet run
        if (!displaySem.tryAcquire())
            return;

        SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    displaySem.release();
                    processEventQueue();
                }
            });
    }

    // Must be called in AWT event thread.
    // TODO: return immediately if view not ready
    private void processEventQueue() {
        PlanEvent e = eventQueue.poll();
        if (e == null)
            return;

        while (e != null) {
            e.display();
            e = eventQueue.poll();
        }

        RepaintManager.currentManager(this).paintDirtyRegions();
    }

    private void refresh() {
        if (SwingUtilities.isEventDispatchThread())
            processEventQueue();
        else
            try {
            SwingUtilities.invokeAndWait(new Runnable() {
                    public void run() {
                        processEventQueue();
                    }
                });
            }
            catch (InterruptedException e) {
            }
            catch (InvocationTargetException x) {
                StatusMessageHandler.instance().displayErrorMessage(x, "Error in refreshing plan view");
            }
    }

    //
    // Menu mode control
    //

    public void setBreaksEnabled(boolean value) {
        allowBreaksItem.setText(value ? "Disable Breaks" : "Enable Breaks");
        if (value) 
            showMessage("Enabled breaks", Color.GREEN.darker());
        else
            showMessage("Disabled breaks", Color.RED);
    }

    //
    // UI actions
    //

    private void startPlan() {
        AppType app = plan.getAppType();
        if (app == AppType.NO_APP) {
            JOptionPane.showMessageDialog(this,
                                          "Can't execute " + plan.getName() + ".\nNo executive has been selected for this plan.",
                                          "Settings error",
                                          JOptionPane.ERROR_MESSAGE);
            readyState("No executive selected", Color.RED);
            return;
        }
        else if (app == AppType.EXTERNAL_APP) {
            JOptionPane.showMessageDialog(this,
                                          "Can't execute " + plan.getName() + ".\nAn external app has been selected for this plan.",
                                          "Settings error",
                                          JOptionPane.ERROR_MESSAGE);
            readyState("External executive selected");
            return;
        }

        reset();
        if (!ExecutionHandler.instance().runExec(plan, PlanView.this)) { // performs own sanity checks
            JOptionPane.showMessageDialog(this,
                                          "Execution of " + plan.getName() + "failed.\nSee console window for details.",
                                          "Execution failure",
                                          JOptionPane.ERROR_MESSAGE);
            readyState("Execution failed", Color.RED);
        }
        else if (Settings.instance().blocksExec())
            stepState();
        else
            executionState();
    }

    public void stepPlan() {
        if (!ExecutionHandler.instance().isExecuting())
            return; // no-op
        stepState();
    }

    public void pauseOrResume() {
        if (!ExecutionHandler.instance().isExecuting())
            return; // no-op
        if (executionPaused) {
            executionState();
            ExecutionHandler.instance().resume();
        }
        else if (!Settings.instance().blocksExec()) {
            showMessage("Can't pause plan because breaks not enabled", Color.RED);
        }
        else {
            ExecutionHandler.instance().pause();
            showMessage("Pause requested", Color.YELLOW);
        }
    }


    //
    // Callbacks from ExecutionHandler
    //

    // Could be called on breakpoint or single-step
    public void blocked() {
        pausedExecutionState();
    }

    public void executionComplete() {
        readyState("Execution complete");
    }

    public void executionFailed() {
        readyState("Execution failed. See console window for details.", Color.RED);
    }

    public void executionTerminated() {
        readyState("Execution terminated", Color.RED);
    }

    //
    // States
    //

    public void readyState(String message) {
        readyState(message, Settings.instance().getColor(PROP_WIN_FCLR));
    }

    public void readyState(String message, Color c) {
        executionPaused = false;
        if (message != null)
            showMessage(message, c);
        showStatus("Ready");
        pauseExecutionItem.setText("Resume execution");
        pauseExecutionItem.setEnabled(false);
        stepExecutionItem.setEnabled(false);
        execItem.setEnabled(true);
        allowBreaksItem.setText(Settings.instance().blocksExec() ? "Disable Breaks" : "Enable Breaks");
        refresh();
        menuBar.requestFocusInWindow();
    }

    // preExecutionState() resets current plan, disables execItem, pauseExecutionItem

    public void executionState() {
        executionPaused = false;
        showMessage("");
        showStatus("Executing");
        pauseExecutionItem.setText("Pause execution");
        pauseExecutionItem.setEnabled(true);
        stepExecutionItem.setEnabled(false);
        execItem.setEnabled(false);
        menuBar.requestFocusInWindow();
    }

    // Like executionState(), but only one step to be taken
    public void stepState() {
        executionPaused = false;
        showMessage("");
        showStatus("Single Step");
        pauseExecutionItem.setText("Pause execution");
        pauseExecutionItem.setEnabled(false);
        stepExecutionItem.setEnabled(false);
        execItem.setEnabled(false);
        ExecutionHandler.instance().step();
        menuBar.requestFocusInWindow();
    }

    public void pausedExecutionState() {
        executionPaused = true;
        // TODO: color
        showMessage("Plan execution is paused. "
                    + ((LuvAction) pauseExecutionItem.getAction()).getAcceleratorDescription()
                    + " to resume, or "
                    + ((LuvAction) stepExecutionItem.getAction()).getAcceleratorDescription()
                    +" to step.");
        showStatus("Paused");
        pauseExecutionItem.setText("Resume execution");
        pauseExecutionItem.setEnabled(true);
        stepExecutionItem.setEnabled(true);
        refresh();
        menuBar.requestFocusInWindow();
    }

    // stopExecutionState() ???

    // finishedExecutionState() ???

    // reloadPlanState() stops execution, resets current plan, goes to readyState()
    
    //
    // Breakpoint handling
    //

    // FIXME? Not sure these need to be public

    private void newBreakPoint(LuvBreakPoint bp) {
        if (breakPointSet.isEmpty())
            removeBPsItem.setEnabled(true);
        breakPointSet.add(bp);
        repaint();
    }

    private void deleteBreakPoint(LuvBreakPoint bp) {
        breakPointSet.remove(bp);
        if (breakPointSet.isEmpty())
            removeBPsItem.setEnabled(false);
        repaint();
    }

    private void removeAllBreakPoints() {
        for (LuvBreakPoint bp : breakPointSet)
            bp.getNode().deleteBreakPoint(bp);
        breakPointSet.clear();
        removeBPsItem.setEnabled(false);
        repaint();
    }

    //
    // Inner Classes
    //

    private interface PlanEvent {
        // Must be called in event thread.
        public void display();
    }

    private class ResetEvent
        implements PlanEvent {
        public void display() {
            resetView();
        }
    }

    private class NewPlanEvent
        implements PlanEvent {
        public Plan plan;

        public NewPlanEvent(Plan p) {
            plan = p;
        }

        public void display() {
            // TODO
        }
    }

    private class NodeEvent {
        public Node node;

        public NodeEvent(Node n) {
            node = n;
        }

        // backward compatibility
        public Node getNode() {
            return node;
        }
    }

    private class NodeStateEvent
        extends NodeEvent
        implements PlanEvent {
        public final NodeState state;
        public final NodeOutcome outcome;
        public final NodeFailureType failureType;
        public final Map<Condition, String> conditions;

        public NodeStateEvent(Node n,
                              NodeState s,
                              NodeOutcome o,
                              NodeFailureType t,
                              Map<Condition, String> c) {
            super(n);
            state = s;
            outcome = o;
            failureType = t;
            conditions = new EnumMap<Condition, String>(c);
        }

        public void display() {
            node.stateTransition(state, outcome, failureType, conditions);
        }
    }

    private class AssignmentEvent
        extends NodeEvent
        implements PlanEvent {
        public final String variable;
        public final String value;

        public AssignmentEvent(Node n,
                               String var,
                               String val) {
            super(n);
            variable = var;
            value = val;
        }

        public void display() {
            node.setVariable(variable, value);
        }
    }
    
    /** The PlanRenderDataProvider class controls how an individual Node is displayed. */
    private class PlanRenderDataProvider
        implements RenderDataProvider {

        // TODO: implement visual cue that node has active/inactive breakpoint

        public Color getBackground(Object o) {
            if (o instanceof Node && ((Node) o).getHighlight())
                return Color.PINK; // *** FIXME ***
            return Settings.instance().getColor(PROP_WIN_BCLR);
        }

        public Color getForeground(Object o) {
            return null; // FIXME?
        }

        public String getDisplayName(Object o) {
            if (o instanceof Node)
                return ((Node) o).getNodeName();
            else
                return null;
        }

        public Icon getIcon(Object o) {
            // FIXME: elaborate depending on Core/Extended mode
            if (o instanceof Node)
                return Constants.getIcon(((Node) o).getType());
            else
                return null;
        }

        public String getTooltipText(Object o) {
            if (o == null || ! (o instanceof Node))
                return null;

            // *** FIXME *** Simplify?
            Node node = (Node) o;
            StringBuilder toolTip  = new StringBuilder();
            toolTip.append("<html><b>NAME</b> ");
            toolTip.append(node.getNodeName());
            toolTip.append("<br><b>TYPE</b>  ");
            toolTip.append(node.getProperty(NODETYPE_ATTR));
            toolTip.append("<br><hr><b>Double-Click</b> on node to view condition information");    
            toolTip.append("<br><b>Right-Click</b> on node to set breakpoints");
          
            return toolTip.toString();
        }

        public boolean isHtmlDisplayName(Object o) {
            return false;
        }
    }

    //
    // PlanRowModel
    //

    // Column names
    protected static final String STATE_COL_NAME        = "State";
    protected static final String OUTCOME_COL_NAME      = "Outcome";
    protected static final String FAILURE_TYPE_COL_NAME = "Failure Type";      

    protected static final String[] cNames = 
    {
        STATE_COL_NAME,
        OUTCOME_COL_NAME,
        FAILURE_TYPE_COL_NAME,
    };

    /** The PlanRowModel class describes the columns of the outline to the right of the tree column. */

    private class PlanRowModel
        implements RowModel {

        public PlanRowModel() {
        }

        public int getColumnCount() {
            return cNames.length;
        }

        public String getColumnName(int col) {
            return cNames[col];
        }

        public Class getColumnClass(int col) {
            switch (col) {
            case STATE_COL_NUM:
                return NodeState.class;

            case OUTCOME_COL_NUM:
                return NodeOutcome.class;

            case FAILURE_TYPE_COL_NUM:
                return NodeFailureType.class;

            default:
                return null;
            }
        }

        public Object getValueFor(Object node, int column) {
            switch (column) {
            case STATE_COL_NUM:
                // State should never be null
                return ((Node) node).getNodeState();

            case OUTCOME_COL_NUM:
                return ((Node) node).getNodeOutcome();

            case FAILURE_TYPE_COL_NUM:
                return ((Node) node).getNodeFailureType();

            default:
                return null;
            }
        }

        public void setValueFor(Object node, int column, Object value) {
            // do nothing
        }

        public boolean isCellEditable(Object node, int column) {
            return false;
        }
        
    }

    //
    // NodeStateRenderer
    //

    // TODO: Add highlighting (borders?) for breakpoint trigger value

    private final static Map<NodeState, Color> nodeStateColorMap =
        new EnumMap<NodeState, Color>(NodeState.INACTIVE.getDeclaringClass()) {
            {
                put(NodeState.INACTIVE,        Color.LIGHT_GRAY);
                put(NodeState.WAITING,         Color.RED); 
                put(NodeState.EXECUTING,       Color.GREEN.darker());
                put(NodeState.ITERATION_ENDED, Color.BLUE.darker());
                put(NodeState.FINISHED,        Color.GRAY);
                put(NodeState.FAILING,         new Color(255, 128, 128));
                put(NodeState.FINISHING,       new Color(128, 128, 255));
            }
        };

    //* NodeState cell renderer
    private class NodeStateRenderer
        extends DefaultTableCellRenderer {

        public Component getTableCellRendererComponent(JTable table,
                                                       Object value,
                                                       boolean isSelected,
                                                       boolean hasFocus,
                                                       int row,
                                                       int column) {
            // TODO set background color appropriately when selected
            setBackground(Color.WHITE); // FIXME
            if (value == null || !(value instanceof NodeState)) {
                setForeground(Color.LIGHT_GRAY);
                setValue(null);
                return this;
            }
            NodeState state = (NodeState) value;
            setForeground(nodeStateColorMap.get(state));
            setValue(state.toString());
            return this;
        }

    }

    //
    // NodeOutcomeRenderer
    //

    // TODO: Add highlighting (borders?) for breakpoint trigger value
    
    private final static Map<NodeOutcome, Color> nodeOutcomeColorMap =
        new EnumMap<NodeOutcome, Color>(NodeOutcome.SUCCESS.getDeclaringClass()) {
            {
                put(NodeOutcome.SUCCESS,         Color.GREEN.darker());
                put(NodeOutcome.FAILURE,         Color.RED);
                put(NodeOutcome.SKIPPED,         Color.BLUE.darker());
                put(NodeOutcome.INTERRUPTED,     Color.RED.darker());
            }
        };

    //* NodeOutcome cell renderer
    private class NodeOutcomeRenderer
        extends DefaultTableCellRenderer {

        public Component getTableCellRendererComponent(JTable table,
                                                       Object value,
                                                       boolean isSelected,
                                                       boolean hasFocus,
                                                       int row,
                                                       int column) {
           // TODO set background color appropriately when selected
            setBackground(Color.WHITE); // FIXME
            if (value == null || !(value instanceof NodeOutcome)) {
                setForeground(Color.LIGHT_GRAY);
                setValue(null);
                return this;
            }
            NodeOutcome outcome = (NodeOutcome) value;
            setForeground(nodeOutcomeColorMap.get(outcome));
            setValue(outcome.toString());
            return this;
        }

    }

    //
    // NodeFailureType cell renderer
    //

    // TODO: Add highlighting (borders?) for breakpoint trigger value

    private class NodeFailureTypeRenderer
        extends DefaultTableCellRenderer {

        public Component getTableCellRendererComponent(JTable table,
                                                       Object value,
                                                       boolean isSelected,
                                                       boolean hasFocus,
                                                       int row,
                                                       int column) {
            // TODO set background color appropriately when selected
            setBackground(Color.WHITE); // FIXME
            setForeground(Color.RED.darker());
            if (value == null || !(value instanceof NodeFailureType)) {
                setValue(null);
                return this;
            }
            NodeFailureType failure = (NodeFailureType) value;
            setValue(failure.toString());
            return this;
        }
    }

}

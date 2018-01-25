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
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.util.EnumMap;
import java.util.Map;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;

import static gov.nasa.luv.Constants.*;
import static gov.nasa.luv.PlexilSchema.*;

/** 
 * The NodeInfoWindow class holds the ConditionsTab, VariablesTab and ActionTab 
 * Plexil Node might contain. 
 */

public class NodeInfoWindow extends JFrame
{
    public static Dimension paneDimensions = new Dimension(900, 300); // shared with VariablesTab in separate file

    private JTabbedPane tabbedPane;

    private ActionTab actionTab;
    private ConditionsTab conditionsTab;
    private VariablesTab variablesTab;
    
    /** 
     * Constructs a NodeInfoWindow with the specified Plexil Node. 
     *
     * @param node the Plexil Node on which the NodeInfoWindow displays information
     */
    
    protected NodeInfoWindow(Node node) 
    {
        super(node.getNodeName() + " Information Window");
        setSize(Settings.instance().getDimension(PROP_NODEINFOWIN_SIZE));
        setLocation(Settings.instance().getPoint(PROP_NODEINFOWIN_LOC));
        
        tabbedPane = new JTabbedPane();
        tabbedPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);

        if (node.hasConditions()) {
            conditionsTab = new ConditionsTab(node);
            tabbedPane.addTab("Conditions", null , conditionsTab, "Displays node conditions");
        }
        else
            conditionsTab = null;

        if (node.hasVariables()) {
            variablesTab = new VariablesTab(node);
            tabbedPane.addTab("Variables", null , variablesTab, "Displays node local variables");

        }
        else
            variablesTab = null;

        if (node.hasAction()) {
            actionTab = new ActionTab(node);
            tabbedPane.addTab(node.getProperty(NODETYPE_ATTR), null , actionTab, "Displays action node expression");
        }
        else
            actionTab = null;
        
        setContentPane(tabbedPane);
        pack();
    }
    
    /** 
     * Creates an instance of an NodeInfoWindow for the specified Plexil Node. 
     *
     * @param node the node on which to create a NodeInfoWindow
     */
    
    public static void open(Node node) 
    {
        NodeInfoWindow iw = node.getInfoWindow();
        if (iw == null) {
            iw = new NodeInfoWindow(node);
            node.setInfoWindow(iw);
        }
        iw.setVisible(true);
        iw.toFront();
    }

    /** 
     * The ActionTab class provides methods for displaying Plexil Node Actions, 
     * such as, Assignment, Command, FunctionCall, LibraryCall or Update. 
     */

    public class ActionTab extends JPanel 
    {    
        /** 
         * Constructs an ActionTab with the specified Plexil Node.
         *
         * @param node Plexil Node on which the ActionTab represents
         */
       
        public ActionTab(Node node) 
        {       
            super(new GridLayout(1,0));
            setPreferredSize(paneDimensions);
        
            // for now there is only one column since the UE does not transmit a
            // resulting value to LUV yet.
            String[] columnNames = {"Expression",};       
            String[][] info = new String[1000][1];  
        
            Vector<String> actionList = node.getActionList();     
            int row = 0;
            for (String action : actionList)
                {
                    if (action != null)
                        {
                            info[row][0] = action; 
                            ++row;
                        }
                }
        
            JTable table = new JTable(info, columnNames);
            table.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
            table.getColumnModel().getColumn(0).setPreferredWidth(900);
            table.setPreferredScrollableViewportSize(paneDimensions);
            table.setShowGrid(false);
            table.setGridColor(Color.GRAY);
            JScrollPane scrollPane = new JScrollPane(table);

            add(scrollPane);
            setOpaque(true);
        }
    }
    
    /** 
     * The ConditionsTab class provides methods for displaying Plexil Node condition 
     * information.
     */

    public class ConditionsTab extends JPanel 
    {
        private Node node;    
        private int rows;    
        private String info[][];    
        private JTable table;     
        private Map<Condition, Integer> rowMap;
    
        /** 
         * Constructs a ConditionsTab with the specified Plexil Node.
         *
         * @param node Plexil Node on which the ConditionsTab represents
         */

        public ConditionsTab(Node node) 
        {       
            super(new GridLayout(1,0));
            setPreferredSize(paneDimensions);

            rowMap = new EnumMap<Condition, Integer>(Condition.EndCondition.getDeclaringClass());
            this.node = node;

            final Map<Condition, Vector<String> > nodeConditions = node.getConditionExprs();
            String[] columnNames = {"Conditions",
                                    "Value",
                                    "Expression"};             
                
            // Compute size of table
            rows = 0;
            for (Vector<String> al : nodeConditions.values())
                rows += al.size();

            // Account for internal conditions
            switch (node.getNodeType()) {
            case Empty:
                rows += 3;
                break;

            case Update:
                rows += 4;
                break;
                
            default:
                rows += 5;
                break;
            }

            info = new String[rows][3];

            int row = 0;
            for (Map.Entry<Condition, Vector<String> > entry : nodeConditions.entrySet()) {
                final Condition c = entry.getKey();
                final int thisRow = row;
                rowMap.put(c, row);

                info[row][0] = c.toString();
                info[row][1] = formatConditionValue(node.getConditionValue(c));
                for (String s : entry.getValue())
                    info[row++][2] = s;
            }
            
            // Add internal conditions
            rowMap.put(Condition.AncestorEndCondition, row);
            info[row][0] = Condition.AncestorEndCondition.toString();
            info[row++][1] = formatConditionValue(node.getConditionValue(Condition.AncestorEndCondition));

            rowMap.put(Condition.AncestorInvariantCondition, row);
            info[row][0] = Condition.AncestorInvariantCondition.toString();
            info[row++][1] = formatConditionValue(node.getConditionValue(Condition.AncestorInvariantCondition));

            rowMap.put(Condition.AncestorExitCondition, row);
            info[row][0] = Condition.AncestorExitCondition.toString();
            info[row++][1] = formatConditionValue(node.getConditionValue(Condition.AncestorExitCondition));

            switch (node.getNodeType()) {
            default:
                rowMap.put(Condition.AbortCompleteCondition, row);
                info[row][0] = Condition.AbortCompleteCondition.toString();
                info[row++][1] = formatConditionValue(node.getConditionValue(Condition.AbortCompleteCondition));

            case Update:
                rowMap.put(Condition.ActionCompleteCondition, row);
                info[row][0] = Condition.ActionCompleteCondition.toString();
                info[row++][1] = formatConditionValue(node.getConditionValue(Condition.ActionCompleteCondition));

            case Empty:
                break;
            }

            this.node.addChangeListener(new Node.ChangeAdapter() {
                    @Override 
                    public void stateTransition(Node node,
                                                NodeState state,
                                                NodeOutcome outcome,
                                                NodeFailureType failure,
                                                Map<Condition, String> conditions) {

                        for (Map.Entry<Condition, String> entry : conditions.entrySet()) {
                            Condition c = entry.getKey();
                            String val = formatConditionValue(entry.getValue());
                            Integer row = rowMap.get(c);
                            if (row == null)
                                continue; // shouldn't happen, but don't blow up if it does
                            info[row][1] = val; // redundant?
                            table.setValueAt(val, row, 1);
                        }
                        repaint();
                    }
                });

            table = new JTable(info, columnNames);
            table.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
            table.getColumnModel().getColumn(0).setPreferredWidth(200);
            table.getColumnModel().getColumn(1).setPreferredWidth(100);
            table.getColumnModel().getColumn(2).setPreferredWidth(600);        
            table.setPreferredScrollableViewportSize(paneDimensions);
            table.setShowGrid(false);
            table.setGridColor(Color.GRAY);
            JScrollPane scrollPane = new JScrollPane(table);

            add(scrollPane);
            setOpaque(true);
        }
    
        /** 
         * Returns the value of the specific condition expression.   
         *
         * @param value the confition value
         * @return the value the condition expression evaluates to (TRUE, FALSE or inf)
         */

        private String formatConditionValue(String value) {
            if (value == null || UNKNOWN.equals(value))
                return UNKNOWN;
            else if (value.equals("0"))
                return "FALSE";
            else if (value.equals("1"))
                return "TRUE";
            else
                return value;
        }
    }


}



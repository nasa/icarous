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

//
// *** FIXME: Add ability to type port number ***
//

package gov.nasa.luv;

import java.awt.*;
import java.awt.event.*;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import java.util.Vector;

public class LuvPortGUI
    extends JPanel
    implements ActionListener {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JFrame frame;
	private DefaultComboBoxModel<Integer> comboBoxModel;
    private JComboBox<Integer> portComboBox;
	
	public LuvPortGUI()
    {
        comboBoxModel = new DefaultComboBoxModel<Integer>();
		portComboBox = new JComboBox<Integer>(); // create empty
		portComboBox.addActionListener(this);
		
		JPanel patternPanel = new JPanel();
		patternPanel.setLayout(new BoxLayout(patternPanel, BoxLayout.PAGE_AXIS));
		patternPanel.add(new JLabel("Select a port:"));
		patternPanel.add(Box.createRigidArea(new Dimension(0,10)));
		portComboBox.setAlignmentX(Component.LEFT_ALIGNMENT);
		patternPanel.add(portComboBox);
		patternPanel.add(Box.createRigidArea(new Dimension(0,10)));

		JButton cancelButton = new JButton("Cancel");
		cancelButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    frame.setVisible(false);
                }
            }
            );
		patternPanel.add(cancelButton);
		patternPanel.setAlignmentX(Component.LEFT_ALIGNMENT);
		add(patternPanel);		
		setBorder(BorderFactory.createEmptyBorder(20, 40, 20, 40));
        setOpaque(true);
		frame = new JFrame("Server Port");
        frame.setContentPane(this);
	}
	
    // FIXME: spin off in separate object?
	public void actionPerformed(ActionEvent e) {
        int newSelection = getSelectedPort();
        int curr = Settings.instance().getPort();
        if (newSelection == curr) {
            frame.setVisible(false);
        } else if (LuvSocketServer.portFree(newSelection)) {
            frame.setVisible(false);
            Luv.getLuv().changePort(newSelection);
        } else {
        	StatusMessageHandler.instance().displayWarningMessage(this,
                                                                         "Port " + newSelection + " in use, please pick another",
                                                                         "Port " + newSelection + " in use");
        	StatusMessageHandler.instance().showChangeOnPort("Still on port " + curr, Color.BLUE);
        	refresh();
        }
    }	
	
	public int getSelectedPort() {
        if (comboBoxModel == null)
            return 0;
        Integer sel = (Integer) comboBoxModel.getSelectedItem();
        return sel.intValue();
	}
	
    private void selectCurrentPort() {
        int deflt = Settings.instance().getPort();
		if (Constants.PORT_MIN <= deflt && deflt <= Constants.PORT_MAX)
            comboBoxModel.setSelectedItem(new Integer(deflt));
    }

    public void refresh() {
        comboBoxModel = new DefaultComboBoxModel<Integer>(LuvSocketServer.getPortList());
        portComboBox.setModel(comboBoxModel);
        selectCurrentPort();
    }

	public void activate()
	{
        StatusMessageHandler.instance().showStatus("Finding open ports");
        refresh();
        frame.pack();
        frame.setVisible(true);
        StatusMessageHandler.instance().showStatus("Select a port");
	}

}

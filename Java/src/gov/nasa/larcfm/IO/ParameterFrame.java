/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

/**
 * A generic frame to hold one or more ParameterPanels.
 * 
 * Either use one of the predefined constructors, or have a subclass create its own.
 * 
 * Subclass constructor can follow this rough workflow:
 * 1. Set up any necessary frames and Pair them with their associatedParameterData objects.
 * 2. Create a list pList of the <ParameterPanel,ParameterData> Pairs.
 * 3. Set up any necessary JButtons or other JComponents and add them to a list bList.
 * 4. Call init(pList,bList). 
 * 
 * If not making a subclass of this, One pattern to follow is:
 * 1. Set up all components in the constructor. (see init below)
 * 2. Create an update() method that: (see update, below)
 *    - updates any needed ParameterData objects (from main program), possibly re-setting them in the ParameterData objects
 *    - calls setup() on any ParameterPanel that might have changed size (number or names of parameters)
 *    - calls update() on all ParameterPanels
 *    - repacks or revalidates its GUI components, if necessary
 */
public class ParameterFrame extends JFrame {
	private static final long serialVersionUID = 1L;

	JScrollPane scroll;
	List<ParameterPanel> ppanels;

	/**
	 * Build an empty ParameterFrame (for subclass constructors)
	 */
	public ParameterFrame () {
		scroll = new JScrollPane();
		ppanels = new ArrayList<ParameterPanel>();
	}
	
	
	/**
	 * Build a ParameterFrame.
	 * This will contain the various ParameterPanels, vertically, followed by any swingItems (such as buttons) beneath them
	 */
	public ParameterFrame(List<Pair<ParameterPanel,ParameterData>> plist, List<JComponent> swingItems) {
		init(plist,swingItems);
	}
	
	/**
	 * Single panel with no other items
	 */
	public ParameterFrame(ParameterPanel pp, ParameterData params) {
		Pair<ParameterPanel,ParameterData> ppp = Pair.make(pp, params);
		@SuppressWarnings("unchecked")
		List<Pair<ParameterPanel,ParameterData>> l1 = Arrays.asList(ppp);
		init(l1, new ArrayList<JComponent>());
	}

	/**
	 * Single panel and one other item (such as a button)
	 */
	public ParameterFrame(ParameterPanel pp, ParameterData params, JComponent sitem) {
		Pair<ParameterPanel,ParameterData> ppp = Pair.make(pp, params);
		@SuppressWarnings("unchecked")
		List<Pair<ParameterPanel,ParameterData>> l1 = Arrays.asList(ppp);
		List<JComponent> l2 = Arrays.asList(sitem);
		init(l1, l2);
	}

	/**
	 * Single panel and multiple items (such as buttons)
	 */
	public ParameterFrame(ParameterPanel pp, ParameterData params, List<JComponent> sitems) {
		Pair<ParameterPanel,ParameterData> ppp = Pair.make(pp, params);
		@SuppressWarnings("unchecked")
		List<Pair<ParameterPanel,ParameterData>> l1 = Arrays.asList(ppp);
		init(l1, sitems);
	}

	/**
	 * Multiple panels that all reference the same ParameterData, multiple other items (such as buttons)
	 */
	public ParameterFrame(List<ParameterPanel> pps, ParameterData params, List<JComponent> sitems) {
		List<Pair<ParameterPanel,ParameterData>> l1 = new ArrayList<Pair<ParameterPanel,ParameterData>>();
		for (ParameterPanel p : pps) {
			l1.add(Pair.make(p,params));
		}
		init(l1,sitems);
	}

	/**
	 * Initialize one panel
	 */
	protected void initPanel(ParameterPanel pp, ParameterData params) {
		pp.setData(params);
		pp.setup();
		pp.update();
		pp.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
	}


	/**
	 * Initialize a frame with one panel over an optional set of swingItems.  SwingItems may be null, indicating none exist.
	 * This is a simplified version of the more general init call.
	 */
	protected void init(ParameterPanel pp, ParameterData pd, List<JComponent> swingItems) {
		List<Pair<ParameterPanel,ParameterData>> plist = new ArrayList<Pair<ParameterPanel,ParameterData>>();
		plist.add(Pair.make(pp, pd));
		init(plist,swingItems);
	}
	
	/**
	 * Initialize a basic frame.  This frame will have a set of ParameterPanels over a set of swingItems.
	 * Either list may be null, indicating no such items exist.
	 * A panel can be modified later (by calling initPanel, then possibly repacking and revalidating this frame), 
	 * but panels cannot be added or removed with this interface.
	 * All subclasses should either call this, overwrite it, or do something equivalent.
	 */
	protected void init(List<Pair<ParameterPanel,ParameterData>> plist, List<JComponent> swingItems) {
		setTitle("Parameters");
		setSize(800,500);
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		
		ppanels = new ArrayList<ParameterPanel>();
		
		JPanel contentPane = new JPanel();
		scroll = new JScrollPane(contentPane);
		add(scroll);
		contentPane.setLayout(new GridBagLayout() );
		GridBagConstraints c = new GridBagConstraints();

		int ROW = 0;
		int COL = 0;
		
		if (plist != null) {
			for (Pair<ParameterPanel,ParameterData> ppp : plist) {
				ppanels.add(ppp.first);
				initPanel(ppp.first,ppp.second);
				c.gridx = COL;
				c.gridy = ROW;
				c.gridwidth = GridBagConstraints.REMAINDER;
				c.fill = GridBagConstraints.HORIZONTAL;
				c.weightx = 1.0;
				contentPane.add(ppp.first,c);
				ROW++;
			}
		}

		c.gridwidth = 1;
		c.fill = GridBagConstraints.NONE;

		if (swingItems != null) {
			for (JComponent b : swingItems) {
				c.gridx = COL;
				c.gridy = ROW;
				contentPane.add(b,c);
				COL++;
			}
		}

		pack();
		scroll.revalidate();
	}
	
	
	
	/**
	 * Update all panels with the same ParameterData object (if you have multiple panels, make sure each has its own key list)
	 * @param params
	 */
	public void update(ParameterData params) {
		for (ParameterPanel ppanel : ppanels) {
			ppanel.setData(params);
			ppanel.setup();
			ppanel.update();
		}
		scroll.revalidate();
	}
	
	/**
	 * Update the panel(s) with name matching panelName with the given ParameterData
	 */
	public void update(String panelName, ParameterData params) {
		for (ParameterPanel ppanel : ppanels) {
			if (ppanel.getName().equals(panelName)) {
				ppanel.setData(params);
				ppanel.setup();
				ppanel.update();				
			}
		}
		scroll.revalidate();
	}
	
	/**
	 * Update each panel with a distinct ParameterData object.
	 * The input list should be in the  same order as the constructor list used
	 */
	public void update(List<ParameterData> params) {
		if (params.size() == ppanels.size()) {
			for (int i = 0; i < ppanels.size(); i++) {
				ppanels.get(i).setData(params.get(i));
				ppanels.get(i).setup();
				ppanels.get(i).update();
				
			}
		}
		scroll.revalidate();
	}

	/**
	 * Return the ParameterPanel associated with the given name, or null if there is no such panel.
	 */
	public ParameterPanel getParameterPanel(String panelName) {
		for (ParameterPanel ppanel : ppanels) {
			if (ppanel.getName().equals(panelName)) {
				return ppanel;
			}
		}
		return null;
	}
	
}


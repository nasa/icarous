/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

//import gov.nasa.larcfm.Util.f;

import java.awt.Component;
//import java.awt.Font;
import java.awt.Graphics;
import java.awt.Toolkit;
//import java.util.ArrayList;

import javax.swing.Action;
//import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
//import javax.swing.UIManager;
//import javax.swing.event.ChangeListener;

/**
 * This extension to JMenu will determine (guess) a maximum number of items allowed based on screen size and create one (or more)
 * "More..." menu items that open subsequent menus that contain the remainder of the items.
 * 
 * Adding JMenuItems and Separators to the base menu will properly create the cascade effect.  
 * Other functions are as per JMenu and may not properly propagate through the submenus.
 * @author ghagen
 *
 */
public class CascadingMenu extends JMenu {

	private static final long serialVersionUID = -6011694877509840352L;
	public static int MAXNUM = -1;
	private int num = 0;
	private CascadingMenu subMenu = null;
//	private ArrayList<ChangeListener> changeListenerList = new ArrayList<ChangeListener>();
	private boolean lastIsSep = false;
		
	public CascadingMenu() {
		super();
		init();
	}

	public CascadingMenu(Action a) {
		super(a);
		init();
	}

	public CascadingMenu(String s) {
		super(s);
		init();
	}
	
	public CascadingMenu(String s, boolean b) {
		super(s, b);
		init();
	}
	
	// guess at the menu height
	private void init() {
		if (MAXNUM < 1) {
			int height = (int)(Toolkit.getDefaultToolkit().getScreenSize().height*0.90);
			MAXNUM = Math.max(6, height/20 - 2); // reserve 2 spaces for separator and More...
		}
	}
	
	/**
	 * If called before any items are added to he menu
	 * @param g
	 */
	public static void setMenuHeight(Graphics g) {
		if (g != null) {
			int height = (int)(Toolkit.getDefaultToolkit().getScreenSize().height*0.90);
			int entryHeight = g.getFontMetrics().getHeight()+4; // there appears to be approximately a 2-pixel buffer to the labels?
			MAXNUM = Math.max(6, height/entryHeight - 2); // reserve 2 spaces for separator and More...
		}
	}

	public JMenuItem add(JMenuItem item) {
		num++;
		if (num >= MAXNUM) {
			if (subMenu == null) {
				if (!lastIsSep) {
					super.addSeparator();
				}
				subMenu = new CascadingMenu("More...");
				super.add(subMenu);
//				for (ChangeListener l : changeListenerList) {
//					subMenu.addChangeListener(l);
//				}
			}
			return subMenu.add(item);
		} else {
			lastIsSep = false;
			return super.add(item);
		}
	}

	public Component add(Component item) {
		num++;
		if (num >= MAXNUM) {
			if (subMenu == null) {
				if (!lastIsSep) {
					super.addSeparator();
				}
				subMenu = new CascadingMenu("More...");
				super.add(subMenu);
//				for (ChangeListener l : changeListenerList) {
//					subMenu.addChangeListener(l);
//				}
			}
			return subMenu.add(item);
		} else {
			lastIsSep = false;
			return super.add(item);
		}
	}

	
	public void addSeparator() {
		num++;
		if (num >= MAXNUM) {
			if (subMenu == null) {
				if (!lastIsSep) {
					super.addSeparator();
				}
				subMenu = new CascadingMenu("More...");
				super.add(subMenu);
//				for (ChangeListener l : changeListenerList) {
//					subMenu.addChangeListener(l);
//				}
			} else {
				subMenu.addSeparator();
			}
		} else {
			lastIsSep = true;
			super.addSeparator();
		}
	}

    public void removeAll() {
    	if (subMenu != null) {
    		subMenu.removeAll();
    	}
    	super.removeAll();
    	lastIsSep = false;
    	num = 0;
    	subMenu = null;
    }

//	public void addChangeListener(ChangeListener l) {
//		if (l != null) changeListenerList.add(l);
//		if (subMenu != null) {
//			subMenu.addChangeListener(l);
//		}
//		super.addChangeListener(l);
//	}
}

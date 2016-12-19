/*     ZoomFrame - a java JFrame that zooms in and out
 *     
 *     Contact:  George Hagen    NASA Langley Research Center
 *               Jeff Maddalon   NASA Langley Research Center
 * 
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Vect2;

import java.awt.Color;
import java.awt.Container;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import javax.swing.JFrame;
import javax.swing.JPopupMenu;

import java.lang.Math;


/**
 * This class defines a general frame to display a ViewGraph2D object.  The class takes care of mouse movement events such as:<p>
 * <ul>
 * <li>Dragging the mouse moves the center of the display
 * <li>Mouse left clicks select a point on the plan
 * <li>Mouse right clicks brings up (the supplied) context menu
 * <li>Mouse wheel scroll, changes the scaling of the ViewPlan;
 * </ul>
 * In the future we may add <p>
 * <ul>
 * <li>Double click
 * <li>Mouse continuous movement
 * </ul>
 */
public class ZoomGraphFrame extends JFrame implements MouseListener, MouseWheelListener, MouseMotionListener, ComponentListener {
	private static final long serialVersionUID = 1L;
	
	// these are set on a mouse click and used in the mouse actions
	private int xpos;
	private int ypos;
	private Vect2 pos;
	
	private long dragDelay = 250; // delay before allowing dragging, in milliseconds
	private int minDragOffset = 1; // minimum number of pixels to have moved to be considered an immediate drag start

	private long lastTimeActivated = 0;
	
	private JPopupMenu contextMenuPoint;
	private JPopupMenu contextMenuSpace;

	private static final int no_button = MouseEvent.NOBUTTON;
	private static final int left_button = MouseEvent.BUTTON1;
	private static final int middle_button = MouseEvent.BUTTON2;
	private static final int right_button = MouseEvent.BUTTON3;
	
	private long timeButtonPressed = 0;
	private int buttonPressed = no_button; // This stores the button that was pressed (may need during drag events, for example)
	
	protected ViewGraph2D panel;
	
	public ZoomGraphFrame(ViewGraph2D myPanel, JPopupMenu pointMenu, JPopupMenu spaceMenu) {  
		panel = myPanel; 
		setSize(panel.getWidth(), panel.getHeight()); 
		setTitle(panel.getPanelName()+" Visualization");

		Container contentPane = getContentPane();
		panel.setBackground(Color.white);
		contentPane.add(panel);
		panel.addMouseListener(this);
		panel.addMouseWheelListener(this);
		panel.addMouseMotionListener(this);
		panel.addComponentListener(this);
		
		contextMenuPoint = pointMenu;
		contextMenuSpace = spaceMenu;
		
		pos = Vect2.ZERO;
	}


	public void componentHidden(ComponentEvent e) {}
	public void componentMoved(ComponentEvent e) {}
	public void componentResized(ComponentEvent e) {}
	public void componentShown(ComponentEvent e) {}

	public void setPointMenu(JPopupMenu pointMenu) {
		contextMenuPoint = pointMenu;
	}

	public void setSpaceMenu(JPopupMenu spaceMenu) {
		contextMenuSpace = spaceMenu;
	}

	public Vect2 getClickPosition() {
		return pos;
	}
	
	public void addData(String s, double x, double y) {
		panel.addData(s, x, y);
	}

	public void markPoint(String s, double x, int m) {
		panel.markPoint(s, x, m);
	}
	
	public void setXUnit(String s) {
		panel.setXUnit(s);
	}

	public void setYUnit(String s) {
		panel.setYUnit(s);
	}
	
	public void clear() {
		panel.clear();
		panel.clearMarks();
		panel.clearSeriesColors();
	}
	
	public void clear(String s) {
		panel.clear(s);
		panel.clearMarks(s);
	}
	
	public void setSeriesColor(String s, Color c) {
		panel.setSeriesColor(s, c);
	}
	
	public void clearSeriesColors() {
		panel.clearSeriesColors();
	}
	
	public ViewGraph2D.GraphType getGraphType() {
		return panel.getGraphType();
	}
	
	public void setGraphType(ViewGraph2D.GraphType m) {
		panel.setGraphType(m);
	}
	
	public boolean getLabelPoints() {
		return panel.getLabelPoints();
	}

	public void setLabelPoints(boolean b) {
		panel.setLabelPoints(b);
	}

	public boolean getLabelSlopes() {
		return panel.getLabelSlopes();
	}

	public void setLabelSlopes(boolean b) {
		panel.setLabelSlopes(b);
	}
	
	public void zoomIn() {
		panel.zoomIn();
	}

	public void zoomOut() {
		panel.zoomOut();
	}

	public void initScale() {
		panel.initScale();
	}
	
	public void setPickPoint(String series, double x) {
		panel.setPickPoint(series, x);
	}
	
	public String getPickSeries() {
		return panel.getPickSeries();
	}

	public double getPickX() {
		return panel.getPickX();
	}

	public double getPickY() {
		return panel.getPickY();
	}
	
	// A mouse press and release 
	public void mouseClicked (MouseEvent me) {
		int button = me.getButton();
		
		if (button == left_button) { 
			panel.pickPoint(xpos,ypos);
		} else if (button == right_button) { 
			if (panel.pickPoint(xpos,ypos) > 0) {
				if (contextMenuPoint != null)contextMenuPoint.show(this,xpos,ypos);
			} else {
				if (contextMenuSpace != null)contextMenuSpace.show(this,xpos,ypos);
			}
		} 
//     Stuff for double clicks:
//
//		else if (me.getClickCount() > 1) {
//			// double click = center on pointer
//			// if on a point, select it and open edit window
//			if (panel.pickPoint(xpos,ypos)) {
//				if (clickAction != null) clickAction.doAction(xpos, ypos);
//			}
//			//panel.centerOnXY(s);
//			//updateDisplays();		
//		} 
		lastTimeActivated = System.currentTimeMillis();
		panel.repaint();
	} 

	// Whenever the mouse is pressed, for a drag operation, this is only called once 
	public void mousePressed(MouseEvent e) {
		xpos = e.getX();
		ypos = e.getY();
		pos = panel.inversePosition2(xpos,ypos);
		buttonPressed = e.getButton();
		lastTimeActivated = timeButtonPressed = System.currentTimeMillis();
	}

	// Whenever the mouse is release, for a drag operation, this is only called once
	public void mouseReleased(MouseEvent e) {
		xpos = e.getX();
		ypos = e.getY();
		pos = panel.inversePosition2(xpos,ypos);
		buttonPressed = no_button;
		lastTimeActivated = System.currentTimeMillis();
	}

	private boolean startDrag(int xoff, int yoff) {
        //return System.currentTimeMillis() > timeButtonPressed + dragDelay || (xoff > minDragOffset || yoff > minDragOffset);
		lastTimeActivated = System.currentTimeMillis();
        return (xoff > minDragOffset || yoff > minDragOffset);
	}
	
	// Invoked every time the mouse is dragged. 
	public void mouseDragged(MouseEvent e) {
		int nx = e.getX();
		int ny = e.getY();
		int xoff = xpos - nx;
		int yoff = ypos - ny;
		xpos = nx;
		ypos = ny;
		// Deliberately not updating the "pos" variable on drag events
		if (startDrag(Math.abs(xoff), Math.abs(yoff))) {
			panel.setCenter(panel.getCenterX()+xoff/panel.getScaleX(), panel.getCenterY()-yoff/panel.getScaleY());
			panel.repaint();			
		}
		lastTimeActivated = System.currentTimeMillis();
	}

	// Mouse entered the frame
	public void mouseEntered(MouseEvent e) {
		lastTimeActivated = System.currentTimeMillis();
	}

	// Mouse exited the frame
	public void mouseExited(MouseEvent e) {
		//		if (statusBar.isVisible()) {
		//			statusBar.clear();
		//		}
		buttonPressed = no_button;
		lastTimeActivated = System.currentTimeMillis();
	}

	public void mouseMoved(MouseEvent e) {
		//updateStatusBar(e, true);
		lastTimeActivated = System.currentTimeMillis();
	}

	// roll the mousewheel to zoom in/out
	public void mouseWheelMoved(MouseWheelEvent mwe) {
		// negative rotation is "up" or zoom in
		int clicks = mwe.getWheelRotation();
		if (clicks < 0) {
			panel.zoomIn();
		} else if (clicks > 0) {
			panel.zoomOut();
		}	
		lastTimeActivated = System.currentTimeMillis();
		panel.repaint();
	}

	/**
	 * Return the last time (in ms since Unix epoch) a mouse button was pressed
	 */
	public long getTimeButtonPressed() {
		return timeButtonPressed;
	}
	
	/**
	 * Return the last time (in ms since Unix epoch) the user interacted with this window
	 */
	public long getLastTimeActivated() {
		return lastTimeActivated;
	}
	
}// ZoomFrame

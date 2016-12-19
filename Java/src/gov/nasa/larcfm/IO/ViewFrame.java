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

import gov.nasa.larcfm.Util.Position;

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


/**
 * This class defines a general frame to display a ViewPlan object.  The class takes care of mouse movement events such as:<p>
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
public class ViewFrame extends JFrame implements MouseListener, MouseWheelListener, MouseMotionListener { 
	private static final long serialVersionUID = 1L;

	// These are just a convenience to map standard Swing constants.
	protected static final int no_button = MouseEvent.NOBUTTON;
	protected static final int left_button = MouseEvent.BUTTON1;
	protected static final int middle_button = MouseEvent.BUTTON2;
	protected static final int right_button = MouseEvent.BUTTON3;
	
	// these are set on a mouse click and used in the mouse actions
	protected int xpos;       // position in screen coordinates
	protected int ypos;       // position in screen coordinates
	private Position pos;     // position in engineering coordinates
	protected int xposStart;       // starting position of pointer in screen coordinates (start of a drag or right-click context menu)
	protected int yposStart;       // starting position of pointer in screen coordinates (start of a drag or right-click context menu)
	private Position posStart;     // starting position of pointer in engineering coordinates (start of a drag or right-click context menu)
	
	private int minDragOffset = 1; // minimum number of pixels to have moved to be considered an immediate drag start
	protected int buttonPressed = no_button; // This stores the button that was pressed (may need during drag events, for example)
	protected long lastTimeActivated = 0;
	
	protected JPopupMenu contextMenuPoint;
	protected JPopupMenu contextMenuSpace;

	protected ViewPort panel;
	
	//Don't allow outsiders to construct one of these.
	protected ViewFrame(ViewPort myPanel, JPopupMenu pointMenu, JPopupMenu spaceMenu) {
		panel = myPanel;
		
		setSize(panel.getWidth(), panel.getHeight()); 
		setTitle("Visualization");
		
		panel.setBackground(Color.white);
		panel.addMouseListener(this);
		panel.addMouseWheelListener(this);
		panel.addMouseMotionListener(this);
		
		contextMenuPoint = pointMenu;
		contextMenuSpace = spaceMenu;
		
		pos = Position.makeLatLonAlt(0,0,0);
	}

	@Override
	// A mouse press and release 
	public void mouseClicked (MouseEvent me) {
		int button = me.getButton();
		
		if (button == left_button) { 
			panel.pickPoint(xpos,ypos);
		} else if (button == right_button) { 
			if (panel.checkPoint(xpos,ypos) > 0) {
				if (contextMenuPoint != null) {
					contextMenuPoint.show(this,xpos,ypos);
				}
			} else {
				if (contextMenuSpace != null) {
					contextMenuSpace.show(this,xpos,ypos);
				}
			}
		} 
	}
	
	
	@Override
	// Whenever the mouse is pressed, for a drag operation, this is only called once 
	public void mousePressed(MouseEvent e) {
		xpos = xposStart = e.getX();
		ypos = yposStart = e.getY();
		pos = panel.inversePosition(xpos,ypos);
		posStart = panel.inversePosition(xposStart,yposStart);
		buttonPressed = e.getButton();
		lastTimeActivated = System.currentTimeMillis();
	}

	@Override
	// Whenever the mouse is release, for a drag operation, this is only called once
	public void mouseReleased(MouseEvent e) {
		xpos = e.getX();
		ypos = e.getY();
		pos = panel.inversePosition(xpos,ypos);
		buttonPressed = no_button;
		lastTimeActivated = System.currentTimeMillis();
	}

	private boolean startDrag(int xoff, int yoff) {
        //return System.currentTimeMillis() > timeButtonPressed + dragDelay || (xoff > minDragOffset || yoff > minDragOffset);
		lastTimeActivated = System.currentTimeMillis();
        return (xoff > minDragOffset || yoff > minDragOffset);
	}
	
	@Override
	// Invoked every time the mouse is dragged. 
	public void mouseDragged(MouseEvent e) {
		// Deliberately not updating the "pos" variable on drag events
		int nx = e.getX();
		int ny = e.getY();
		int xoff = xpos - nx;
		int yoff = ypos - ny;
		xpos = nx;
		ypos = ny;
		if (startDrag(Math.abs(xoff), Math.abs(yoff))) {
			panel.setCenter(panel.getCenterX()+xoff/panel.getScaleX(), panel.getCenterY()-yoff/panel.getScaleY());
			panel.repaint();			
		}
		lastTimeActivated = System.currentTimeMillis();
	}

	@Override
	// Mouse entered the frame
	public void mouseEntered(MouseEvent e) {
		lastTimeActivated = System.currentTimeMillis();
	}

	@Override
	// Mouse exited the frame
	public void mouseExited(MouseEvent e) {
		//		if (statusBar.isVisible()) {
		//			statusBar.clear();
		//		}
		buttonPressed = no_button;
		lastTimeActivated = System.currentTimeMillis();
	}

	@Override
	public void mouseMoved(MouseEvent e) {
		//updateStatusBar(e, true);
		lastTimeActivated = System.currentTimeMillis();
	}

	// roll the mousewheel to zoom in/out
	@Override
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
	 * Return the last time (in ms since Unix epoch) the user interacted with this window
	 */
	public long getLastTimeActivated() {
		return lastTimeActivated;
	}
	
	public int getX() {
		return xpos;
	}
	
	public int getY() {
		return ypos;
	}
	
	public int getXStart() {
		return xpos;
	}
	
	public int getYStart() {
		return ypos;
	}
	
	public Position getClickPosition() {
		return pos;
	}
	
	public Position getStartClickPosition() {
		return posStart;
	}
	
}// ViewFrame

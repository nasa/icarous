/*------------------------------------------------------------------------------
 *     Horizontal View 
 *     
 *     Contact:  George Hagen    NASA Langley Research Center
 *               Rick Butler     NASA Langley Research Center
 *               Jeff Maddalon   NASA Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *-----------------------------------------------------------------------------
 */

package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.BoundingRectangle;
import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Util;

import java.lang.Math;
import java.util.ArrayList;
import java.awt.*;

/**
 * Note.  It is vitally important to call setPlans(p) to load plans when using this class 
 * 
 * Note.  The user of this class has the responsibility of initializing the 2D-arraylist conflicts;
 */
public abstract class ViewPortHoriz extends ViewPort {  

	private static final long serialVersionUID = 1L;

//	public enum ViewType {Main, Vert, Polar, Projection};
//
//	// These need to be _set_ (not re-defined!) in the subclasses!
//	/** Type of the panel.  This should be assigned a value in the subclass constructor. */
//	protected ViewType vType;

	// Graphics Default values

	public static final Color Turn_Point_Color = new Color(250,200,100);
	public static final Color VSC_Point_Color = new Color(250,100,100);
	public static final Color GSC_Point_Color = new Color(250,200,200);
	public static final Color Current_Step_On_Point_Color = black;
	public static final Color Current_Step_Off_Point_Color = makeColorAlpha(gray,150);
	public static final Color Conflict_Color = red; 
	public static final Color Ownship_Current_Step_Color = new Color(250,100,0);


	// minimums and maximums for scaling
	protected double min_xval;
	protected double mid_xval;
	protected double max_xval;
	protected double min_yval;
	protected double mid_yval;
	protected double max_yval;
	protected double min_zval;
	protected double mid_zval;
	protected double max_zval;
	protected double min_seg_length;
	protected double max_seg_length;
	protected double avg_seg_length;
	//protected double min_tval;
	//protected double max_tval;

	private double default_alt;

	private boolean isLatLong;
	private boolean isGridLabels;
	protected String scaleLabel;
	protected int scaleLength;

	private int displayInterval = 1;  
	protected double zoomFactor = 1.2;

	protected boolean defaultScaleToContinentalUS;

	public ViewPortHoriz(boolean stretch) {
		super(stretch);
		//vType = ViewType.Main;

		defaultScaleToContinentalUS = true;

		scaleLabel = "";
		scaleLength = 1;

		isLatLong = true;
		isGridLabels = true;

		default_alt = Units.from("ft", 12000);

		//setAxis(3, true, "deg", 15, true, "deg");
	}



	//
	// Main Paint Method
	//


	public void paintComponent(Graphics g){
		double inc;

		super.paintComponent(g);
		String label;

		g.setColor(black);

		// draw the X axis
		int text_height = g.getFontMetrics().getAscent();
		int text_width = g.getFontMetrics().stringWidth("XX.XX\u00B0N-");

		if (isGridLabels) {
			double xl = getCenterX() - getRangeX()/2.0;
			double xu = getCenterX() + getRangeX()/2.0;
			inc = (xu - xl) / 11.0;
			if (inc < Integer.MAX_VALUE/10) { // avoid overflow issues
				label = "";
				for (double ii=xl+inc; ii < xu; ii = ii + inc) {
					if (isLatLon()) {
						label = f.Fm2(Math.abs(Units.to("deg",ii))) + ((ii < 0.0) ? "\u00B0W" : "\u00B0E");
					} else {
						label = f.Fm2(Units.to("NM",ii));
					}
					drawString(g, "|", screenMapX(ii),  getHeight()-5-text_height, 0, -1);
					drawString(g, label, screenMapX(ii), getHeight()-5-text_height, 0, 1);
				}
			}

			// draw the Y axis
			double yl = getCenterY() - getRangeY()/2.0;
			double yu = getCenterY() + getRangeY()/2.0;
			inc = (yu - yl) / 11.0;
			if (inc < Integer.MAX_VALUE/10) { // avoid overflow issues
				label = "";
				for (double ii=yl+inc; ii < yu; ii = ii + inc) {
					if (isLatLon()) { 
						label = f.Fm2(Math.abs(Units.to("deg",ii))) + ((ii < 0.0) ? "\u00B0S" : "\u00B0N");
					} else {
						label = f.Fm2(Units.to("NM",ii));
					}
					drawString(g, label+"-", text_width+5, screenMapY(ii), 1, 0);
				}
			}
		}


		if (isLatLon()) {
			// draw some lines of latitude and longitude
			g.setColor(gray);
			double startx = Units.from("deg",-180);
			double starty = Units.from("deg",90);
			double endx = Units.from("deg",180);
			double endy = Units.from("deg",-90);
			inc = Units.from("deg",30);

			for (double xp = startx; xp <= endx; xp = xp + inc) {
				for (double yp = endy; yp <= starty; yp = yp + inc) {
					drawLine(g,startx,    yp, endx,      yp);
					drawLine(g,     xp, endy,    xp, starty);
				}
			}			
		}
	}// paintComponent(Graphics g)  


	//
	// Utilities
	//

	public boolean isGridLabels() {
		return isGridLabels;
	}


	public void setGridLabels(boolean isGridLabels) {
		this.isGridLabels = isGridLabels;
	}


	/** Name for this panel */
	public String getPanelName() {
		if (isLatLon()) {
			return "Lat/Lon";
		} else {
			return "XY";
		}
	}

	/**
	 * This method scales (or rescales) the display and sets the center to the middle.  This
	 * is usually invoked after new plan information has been provided.  
	 */
	public void setScaleAndCenterOnMiddle() {
		double cx = (max_xval-min_xval)/2.0+min_xval;
		double cy = (max_yval-min_yval)/2.0+min_yval;

		double sscalx = 0.8*getWidth()/(max_xval - min_xval+0.0001);
		double sscaly = 0.8*getHeight()/(max_yval - min_yval+0.0001);
		double sscal = Util.min(sscalx,sscaly);
		sscal = Util.max(sscal, 0.000001); // want to guard against sscal ever becoming 0.0
		//f.pln("$$$ scale: width,height ="+getWidth()+",  "+getHeight());
		//f.pln("$$$ scale: "+sscal);
		setScaleAndCenter(sscal,sscal,cx,cy);
		computeMapScale();
	}

	public void setCenter(Position ss) {
		setCenter(ss.x(),ss.y());
	}

	/** Take a position and convert it to a screen coordinate in the X (horizontal) direction */
	public int screenMapX(Position p) {
		return screenMapX(p.x());
	}
	/** Take a position and convert it to a screen coordinate in the Y (vertical) direction */
	public int screenMapY(Position p) {
		return screenMapY(p.y());
	}
	/** Take a position and put it into a Vect2 (but retain the user coordinates) */
	public Vect2 position2Vect(Position p) {
		return p.vect2();
	}
	/** Return the components of a velocity that are in the screen's plane.	 */
	public Vect2 vel2Vect(Velocity v) {
		return v.vect2();
	}


	/**
	 * Return true if the given point in user space is within buffer pixels of the visible drawing area
	 */
	public boolean onUserScreen(Position p, int buffer) {
		return onUserScreen(p.x(), p.y(), buffer);
	}

	public boolean onUserScreen(Position p1, Position p2, int buffer) {
		return onUserScreen(p1.x(), p1.y(), p2.x(), p2.y(), buffer);
	}

	public boolean onUserScreen(BoundingRectangle br, int buffer) {
		BoundingRectangle scr = new BoundingRectangle();
		scr.add(inversePosition(-buffer,-buffer));
		scr.add(inversePosition(getWidth()+buffer,getHeight()+buffer));
		return br.intersects(scr);
	}

	/** 
	 * Given x and y in screen coordinates create a position object.  To compute
	 * a Position, some estimate must be taken (fundamentally, moving from 2-space to
	 * 3-space involves some estimation.
	 * @param x horizontal screen coordinate
	 * @param y vertical screen coordinate
	 * @return an estimate of the position on the screen
	 */
	public Position inversePosition(int x, int y) {
		double sx = userMapX(x);
		double sy = userMapY(y);
		if (isLatLon()) {
			return Position.mkLatLonAlt(sy, sx, default_alt);
		} else {
			return Position.mkXYZ(sx, sy, default_alt);	
		}			
	}

	/** Are screen coordinates sx and sy near Position ss. 
	 * 
	 * @param sx screen coordinate x
	 * @param sy screen coordinate y
	 * @param ss position
	 * @return True if the screen coordinates are within the "Default_Point_Radius" 
	 */
	public boolean near(int sx, int sy, Position ss) {
		return Math.abs(screenMapX(ss) - sx) < Default_Point_Radius && Math.abs(screenMapY(ss) - sy) < Default_Point_Radius;
	}

	public int getDisplayInterval() {
		return displayInterval;
	}

	public void setDisplayInterval(int currentStep) {
		//f.pln(" $$$>>> ViewPlansPolysStates: SET displayInterval = "+displayInterval);
		this.displayInterval = currentStep;
	}

	public boolean isLatLon() {
		return isLatLong;
	}	

	/**
	 * TODO fix this documentation
	 * This method finds the minimum and maximum values of all the plans, and also rescales and recenters the display
	 */
	public void setLimits(ViewPortHoriz vph) {		
		min_seg_length = vph.min_seg_length;
		max_seg_length = vph.max_seg_length;
		min_xval = vph.min_xval;
		max_xval = vph.max_xval;
		min_yval = vph.min_yval;
		max_yval = vph.max_yval;
		min_zval = vph.min_zval;
		max_zval = vph.max_zval;

		mid_xval = (max_xval - min_xval) / 2.0 + min_xval;
		mid_yval = (max_yval - min_yval) / 2.0 + min_yval;
		mid_zval = (max_zval - min_zval) / 2.0 + min_zval;

		setScaleAndCenterOnMiddle();
		computeMapScale();
		//f.pln(" $$$$  latlon "+isLatLon());
		//f.pln(" $$$$  ViewPlansPolyStates.findPlanMinMax: min_xval = "+min_xval+" 	max_xval = "+max_xval);
		//f.pln(" $$$$  ViewPlansPolyStates.findPlanMinMax: min_yval = "+min_yval+" 	max_yval = "+max_yval);
		//f.pln(" $$$$  ViewPlansPolyStates.findPlanMinMax: min_zval = "+min_zval+" 	max_zval = "+max_zval);
	}

	public void setScaleContinentalUS() {
		if (isLatLon()) {
			min_xval = Units.from("deg", -135);
			min_yval = Units.from("deg", 15);
			max_xval = Units.from("deg", -60);
			max_yval = Units.from("deg", 60);
		} else { // approximately the same area:
			min_xval = 0;
			min_yval = 0;
			max_xval = Units.from("NM", 4500);
			max_yval = Units.from("NM", 2700);

		}
		min_zval = 0.0;
		max_zval = Units.from("ft", 30000.0);
		mid_xval = (max_xval - min_xval) / 2.0 + min_xval;
		mid_yval = (max_yval - min_yval) / 2.0 + min_yval;
		mid_zval = (max_zval - min_zval) / 2.0 + min_zval;
	}

	public void setDefaultScaleToContinentalUS(boolean b) {
		defaultScaleToContinentalUS = b;
	}

	/**
	 * Does the default (plan-less) scaling default to approximately the continental US (or similar area if Euclidean)?
	 */
	public boolean isDefaultScaleToContinentalUS() {
		return defaultScaleToContinentalUS;
	}

	/**
	 * Compute the information necessary to display the map scale (typically in the lower right corner of the display).  
	 * This information is typically only used on horizontal displays.
	 */
	protected void computeMapScale() { 
		double xScale = getScaleX();   //xScale in screen / user
		double user_dist = GreatCircle.distance_from_angle(100.0/xScale,0); // how long is a 100 pixel line with the current scale
		double cat = 1.0;

		if (user_dist < Units.from(Units.NM, 1.5)) {
			// do nothing
		} else if (user_dist < Units.from(Units.NM, 3.5)) {
			cat = 2.0;
		} else if (user_dist < Units.from(Units.NM, 7.5)) {
			cat = 5.0;
		} else if (user_dist < Units.from(Units.NM, 15.0)) {
			cat = 10.0;
		} else if (user_dist < Units.from(Units.NM, 35.0)) {
			cat = 20.0;
		} else if (user_dist < Units.from(Units.NM, 75.0)) {
			cat = 50.0;
		} else if (user_dist < Units.from(Units.NM, 150.0)) {
			cat = 100.0;
		} else if (user_dist < Units.from(Units.NM, 350.0)) {
			cat = 200.0;
		} else {
			cat = 500.0;
		}

		scaleLabel = f.Fm0(cat)+" NM";
		scaleLength = (int)Math.floor(GreatCircle.angle_from_distance(Units.from(Units.NM,cat),0.0)*xScale);
	}

//	public ViewType getViewType() {
//		return vType;
//	}

	/** Calculate the horizontal distance from position (x1,y1) to position (x2,y2).
    This returns a xy distance. If convertLL is true, first do a geodesic to xy conversion,
    otherwise treat the inputs as xy coordinates.  */
	static double calcD(Vect2 v1, Vect2 vv2, boolean convertLL) {
		if (convertLL) {
			// x is lon, y is lat
			// special case: go over pole?
			double v2_x = vv2.x;
			double v2_y = vv2.y;
			while (v2_x-v1.x > Units.from("deg",180)) {
				v2_x = v2_x-Units.from("deg",360);
			}
			while (v2_x-v1.x < Units.from("deg",-180)) {
				v2_x = v2_x+Units.from("deg",360);
			}
			return GreatCircle.distance(v1.y, v1.x, v2_y, v2_x);
		} else {
			return vv2.Sub(v1).norm();
		}
	}

	public boolean segmentOnScreen(Position p1, Position p2, int buffer) {
		int x1 = screenMapX(p1);
		int x2 = screenMapX(p2);
		int y1 = screenMapY(p1);
		int y2 = screenMapY(p2);
		return (x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) || (x1 > getWidth() && x2 > getWidth()) || (y1 > getHeight() && y2 > getHeight()); 
	}



	public void zoomIn() {
		java.awt.Point p = getCurrentMouseLocation();
		double ubx = userMapX(p.x);
		double uby = userMapY(p.y);

		double sscal = getScaleX();
		sscal*=zoomFactor;
		sscal = Util.min(sscal, 1000000.0);
		double centerX = ubx+((getWidth()/2-p.x) / sscal); 
		double centerY = uby+((p.y-getHeight()/2) / sscal);
		setScaleAndCenter(sscal, sscal, centerX, centerY);	
		computeMapScale();
	}

	public void zoomOut() {
		java.awt.Point p = getCurrentMouseLocation();
		double ubx = userMapX(p.x);
		double uby = userMapY(p.y);

		double sscal = getScaleX();
		if (getRangeX() > 8.0*(max_xval-min_xval) || getRangeY() > 8.0*(max_yval-min_yval)) return;
		sscal/=zoomFactor;
		sscal = Util.max(sscal, 0.01);
		double centerX = ubx+((getWidth()/2-p.x) / sscal); 
		double centerY = uby+((p.y-getHeight()/2) / sscal);

		setScaleAndCenter(sscal, sscal, centerX, centerY);
		computeMapScale();
	}


	public Color npColor(NavPoint np, Color ptColor) {
		Color rtn = ptColor;
		if (np.isTrkTCP()) rtn = Turn_Point_Color;
		if (np.isVsTCP()) rtn = VSC_Point_Color;
		if (np.isGsTCP()) rtn = GSC_Point_Color;
		return rtn;
	}




	//
	// Drawing Primitives
	//

	/**
	 * Draw a circle that can be warped if drawn near the poles of the earth.
	 * @param g
	 * @param v
	 * @param radius
	 */
	protected void drawProjectedCircle(Graphics g, Vect2 v, double radius) {
		if (isLatLon()) {
			double lat = v.y;
			double lon = v.x;
			Vect2 xy = simpleConvToVect2(lat,lon);
			double xcenter = 0.0;
			double ycenter = xy.y();
			int xoffset = screenMapX(simpleConvToLatLon(xy.x(),xy.y()).second) - screenMapX(0.0);

			int height = screenMapY(simpleConvToLatLon(0.0, ycenter-radius).first) - screenMapY(simpleConvToLatLon(0.0, ycenter+radius).first);
			int width = screenMapX(simpleConvToLatLon(radius, ycenter).second) - screenMapX(simpleConvToLatLon(-radius, ycenter).second);

			int x = screenMapX(simpleConvToLatLon(xcenter-radius, ycenter+radius).second)+xoffset;
			int y = screenMapY(simpleConvToLatLon(xcenter-radius, ycenter+radius).first);

			g.drawOval(x,y,width,height);
		} else {
			drawCircle(g, v, radius);
		}
	}

	/**
	 * Draw a plan with lines or arrows
	 * @param g graphics context
	 * @param plan plan to draw
	 * @param arrows true if draw arrowheads
	 * @param dots true if draw points
	 * @param curves true if draw latlon and turn curves
	 * @param names true if names are shown, names with a `$` are never shown
	 * @param ptColor the color of the points, lines, and arrows, colors of TCP points are set by the default values of the class 
	 * @param pointRadius size of the points to plot
	 */
	protected void drawPlan(Graphics g, Plan plan, boolean arrows, boolean dots, boolean curves, boolean names, Color ptColor, int pointRadius) {
		Color tmp = g.getColor();
		for (int i = 0; i < plan.size()-1; i++) {
			g.setColor(ptColor);
			if (i == 0 && dots) drawPt(g,plan.point(0).position());
			NavPoint np0 = plan.point(i);
			NavPoint np = plan.point(i+1);
			if (onUserScreen(np0.position(), np.position(), 10)) {
				if (np.isTrkTCP()) g.setColor(Turn_Point_Color);
				//if (iFP.point(j).isTurnMid()) ptColor = TCPTurnPointColor;
				if (np.isVsTCP()) g.setColor(VSC_Point_Color);
				if (np.isGsTCP()) g.setColor(GSC_Point_Color);
				if (dots) drawPt(g,plan.point(i+1).position(), pointRadius);
				if (names) {
					if ( ! np.label().contains("$")) {
						drawString(g, "     "+np.label(), np.position(),0,0);
					}
				}
				g.setColor(ptColor);
				drawSegArrow(g,plan,plan.getTime(i),plan.getTime(i+1),arrows,curves);
			}
		}
		g.setColor(tmp);
	}

	/**
	 * Draw a possibly curved arrow between points in a plan
	 * @param g graphics
	 * @param plan plan to draw
	 * @param t1 start time
	 * @param t2 end time
	 * @param head true if arrowhead should be drawn
	 * @param showInterpolations set to true to draw latlon and turn curves
	 */
	protected void drawSegArrow(Graphics g, Plan plan, double t1, double t2, boolean head, boolean showInterpolations) {
		Position p1 = plan.position(t1);
		Position p2 = plan.position(t2);
		boolean kin = (!plan.isLinear() && plan.inTrkChange(t1));
		if (showInterpolations && (plan.isLatLon() || kin)) {
			int dx = pixelDistanceX(p1,p2);
			int dy = pixelDistanceY(p1,p2);
			if (dx <= 1 && dy <= 1) {
				// same on screen, draw nothing
				return; 
			} else if ((dx < 20 && dy < 20) || p1.almostEquals(p2) || Math.abs(t1-t2) < Constants.get_time_accuracy()) {
				// close to each other on screen, linear is probably good enough
				if (head) {
					drawArrow(g,position2Vect(p1),position2Vect(p2));
				} else {
					drawLine(g,position2Vect(p1),position2Vect(p2));
				}
			} else {
				// break in half for curves
				double mid = (t1+t2)/2.0;
				drawSegArrow(g,plan, t1,mid,false,showInterpolations);
				drawSegArrow(g,plan, mid,t2,head,showInterpolations);
			}
		} else {
			// linear only
			if (head) {
				drawArrow(g,position2Vect(p1),position2Vect(p2));
			} else {
				drawLine(g,position2Vect(p1),position2Vect(p2));
			}
		}
	}


	/**
	 * Draw a line from p1 to p2, optionally including an arrowhead, optionally following great circles
	 * @param g graphics
	 * @param p1 start position
	 * @param p2 end position
	 * @param head if true, include arrowhead
	 * @param showInterpolations if true, follow great circle
	 */
	protected void drawLine(Graphics g, Position p1, Position p2, boolean head, boolean showInterpolations) {
		if (showInterpolations && p1.isLatLon()) { // follow great circle
			int dx = pixelDistanceX(p1,p2);
			int dy = pixelDistanceY(p1,p2);
			if (dx <= 1 && dy <= 1) {
				// same on screen, draw nothing
				return; 
			} else if ((dx < 20 && dy < 20) || p1.distanceH(p2) < 10000) {
				// close to each other on screen, or within 10km, linear is probably good enough
				if (head) {
					drawArrow(g,position2Vect(p1),position2Vect(p2));
				} else {
					drawLine(g,position2Vect(p1),position2Vect(p2));
				}
			} else {
				// break in half for curves
				Position mid = p1.midPoint(p2);
				drawLine(g,p1, mid,false,showInterpolations);
				drawLine(g,mid, p2,head,showInterpolations);
			}
		} else {
			// linear only
			if (head) {
				drawArrow(g,position2Vect(p1),position2Vect(p2));
			} else {
				drawLine(g,position2Vect(p1),position2Vect(p2));
			}
		}
	}

	/**
	 * Draw a line from p1 to p2, following the great circle, if appropriate
	 */
	protected void drawLine(Graphics g, Position p1, Position p2) {
		drawLine(g,p1,p2,false,true);
	}


	/**
	 * Draw an arrow from p1 to p2, following the great circle, if appropriate
	 */
	protected void drawArrow(Graphics g, Position p1, Position p2) {
		drawLine(g,p1,p2,true,true);
	}

	protected void drawPolygon(Graphics g, SimplePoly p, boolean filled) {
		ArrayList<Vect2> pts = new ArrayList<Vect2>();
		for (int i = 0; i < p.size(); i++) {
			pts.add(p.getVertex(i).vect2());
		}
		super.drawPolygon(g, pts, filled);
	}

	/**
	 * Draw a line representing a polypath's centroid travel.  
	 * If the polypath is continuing, draw an arrow indicating the direction of continuing movement.  
	 * This does not draw the "current time" polygon.
	 * @param g graphics
	 * @param pp path
	 * @param arrows if true, draw arrows between points
	 * @param dots if true, draw dots between segments
	 * @param keyPolys if true, draw key polygons at each segment end (as dashed outlines)
	 */
	protected void drawPolyPath(Graphics g, PolyPath pp, boolean arrows, boolean dots, boolean keyPolys) {
		Stroke baseStroke = ((Graphics2D)g).getStroke();	
		for (int i = 0; i < pp.size(); i++) {
			SimplePoly sp = pp.getPolyRef(i);
			Velocity v = pp.initialVelocity(i);
			if (i == pp.size()-1 && pp.isContinuing() && !v.isZero() && !v.isInvalid()) {
				// draw open arrow
				Position p = sp.centroid().linear(v, 600); // 10 mins
				drawArrow(g,sp.centroid(),p);
			}
			if (i > 0) {
				Position prev = pp.getPolyRef(i-1).centroid();
				drawLine(g,prev,sp.centroid(),true,arrows);
			}
			if (dots) {
				drawPt(g,sp.centroid());
			}
			if (keyPolys) {
				setLineStyleDashed(g);
				drawPolygon(g,sp,false);
				((Graphics2D)g).setStroke(baseStroke);
			}
		}
	}


}



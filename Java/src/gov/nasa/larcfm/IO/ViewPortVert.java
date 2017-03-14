/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.IO;


import gov.nasa.larcfm.Util.Constants;
import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.TcpData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Util;

import java.lang.Math;
import java.util.ArrayList;
import java.awt.*;
import java.awt.image.BufferedImage;

public abstract class ViewPortVert extends ViewPort {   

	private static final long serialVersionUID = 1L;

//	public enum ViewType {Main, Vert, Polar, Projection};
//
//	// These need to be _set_ (not re-defined!) in the subclasses!
//	/** Type of the panel.  This should be assigned a value in the subclass constructor. */
//	protected ViewType vType;

	// These are common variables shared among all ViewPortVerts:

	public static final Color Turn_Point_Color = ViewPortHoriz.Turn_Point_Color;
	public static final Color VSC_Point_Color = ViewPortHoriz.VSC_Point_Color;
	public static final Color GSC_Point_Color = ViewPortHoriz.GSC_Point_Color;
	public static final Color Current_Step_On_Point_Color = ViewPortHoriz.Current_Step_On_Point_Color;
	public static final Color Current_Step_Off_Point_Color = ViewPortHoriz.Current_Step_Off_Point_Color;
	public static final Color Conflict_Color = ViewPortHoriz.Conflict_Color; 
	public static final Color Ownship_Current_Step_Color = ViewPortHoriz.Ownship_Current_Step_Color;

	public static final double Default_object_size = ViewPortHoriz.Default_Object_Size;
	public static final int Default_Point_Radius = ViewPortHoriz.Default_Point_Radius;
	public static final int default_triangle_size = ViewPortHoriz.Default_Triangle_Size;

	// Other parameters
	protected double default_alt;

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

	private int displayInterval = 1;  
	private boolean isLatLong;

	private Vect2 viewVec;
	private Vect2 viewVecPerp;
	private String unitsVDist = "ft";
	private double projected_max_x;
	private double projected_min_x;
	private int text_height;
	private int text_width;

	protected boolean view_WE;
	protected boolean view_EW;
	protected boolean view_NS;
	protected boolean view_SN;

	protected ArrayList<Integer> proximityAc;


	// for vertical stretch is true
	public ViewPortVert(boolean stretch) {
		super(stretch);
		//vType = ViewType.Vert;

		isLatLong = true;

		default_alt = Units.from("ft", 12000);
		setViewDirection(Vect2.mkTrkGs(0.0,1.0));  // look from the south

		BufferedImage bi = new BufferedImage(5, 5, BufferedImage.TYPE_INT_RGB);
		FontMetrics fm = bi.getGraphics().getFontMetrics();
		text_width = fm.stringWidth("XXXXX ["+unitsVDist+"]-");  
		text_height = fm.getHeight();

		proximityAc = new ArrayList<Integer>(10);
	}


	//
	// Main Paint Method
	//

	public void paintComponent(Graphics g) {
		super.paintComponent(g);

		// Draw ground level.
		g.setColor(lightGray);
		drawLine(g,getScreen().getMinX(), 0.0, getScreen().getMaxX(), 0.0);

		// draw the X axis
		g.setColor(black);
		String label;
		text_height = g.getFontMetrics().getAscent();
		double xl = getCenterX() - getRangeX()/2.0;
		double xu = getCenterX() + getRangeX()/2.0;
		double inc = (xu - xl) / 11.0;
		if (inc < Integer.MAX_VALUE/10) { // avoid overflow issues
			label = "";
			char left_char = (view_WE || view_EW)?(view_WE?'W':'E'):(view_NS?'N':'S');
			char right_char = (view_WE || view_EW)?(view_WE?'E':'W'):(view_NS?'S':'N');
			for (double ii=xl+inc; ii < xu; ii = ii + inc) {
				if (isLatLon()) {
					label = f.Fm0(Math.abs(Units.to("deg",ii)))+"\u00B0" + ((ii < 0.0) ? left_char : right_char);
				} else {
					label = f.Fm2(Units.to("NM",ii));
				}
				drawString(g, "|", screenMapX(ii),  getHeight()-5-text_height, 0, -1);
				drawString(g, label, screenMapX(ii), getHeight()-5-text_height, 0, 1);
			}
		}

		// draw the Y axis
		text_width = g.getFontMetrics().stringWidth("XXXXX ["+unitsVDist+"]-");  
		double yl = getCenterY() - getRangeY()/2.0;
		double yu = getCenterY() + getRangeY()/2.0;
		inc = (yu - yl) / 11.0;
		if (inc < Integer.MAX_VALUE/10) { // avoid overflow issues
			label = "";
			for (double ii=yl+inc; ii < yu; ii = ii + inc) {
				label = Units.str(unitsVDist,ii,0)+"-";
				drawString(g, label, text_width+5, screenMapY(ii), 1, 0);
			}
		}

	}// paintComponent(Graphics g)  



	//
	// Utilities
	//

	/** Name for this panel */
	public String getPanelName() {
		if (isLatLon()) {
			return ((view_WE || view_EW)?"Lon":"Lat")+"/Alt";
		} else {
			return ((view_WE || view_EW)?"X":"Y")+"/Z";
		}
	}

	/**
	 * This method scales (or rescales) the display and sets the center to the middle.  This
	 * is usually invoked after new plan information has been provided.  
	 */
	public void setScaleAndCenterOnMiddle() {
		findMinMaxX();
		setUserArea(projected_min_x,min_zval,projected_max_x,max_zval,40,10,text_height+text_height+5,text_width+10);
	}



	public void setCenter(Position ss) {
		setCenter(ss.vect2().dot(viewVecPerp),ss.z());
	}

	/** Take a position and convert it to a screen coordinate in the X (horizontal) direction */
	public int screenMapX(Position p) {
		return screenMapX(p.vect2().dot(viewVecPerp));
	}
	/** Take a position and convert it to a screen coordinate in the Y (vertical) direction */
	public int screenMapY(Position p) {
		return screenMapY(p.z());
	}
	/** Take a position and put it into a Vect2 (but retain the user coordinates) */
	public Vect2 position2Vect(Position p) {
		//return new Vect2(xzView?p.x():p.y(),p.z());
		return new Vect2(p.vect2().dot(viewVecPerp),p.z());
	}
	/** Return the components of a velocity that are in the screen's plane.	 */
	public Vect2 vel2Vect(Velocity v) {
		return new Vect2(v.vect2().dot(viewVecPerp), v.z());
	}

	/**
	 * Return true if the given point in user space is within buffer pixels of the visible drawing area
	 */
	public boolean onUserScreen(Position p, int buffer) {
		return onUserScreen(p.vect2().dot(viewVecPerp), p.z(), buffer);
	}

	public boolean onUserScreen(Position p1, Position p2, int buffer) {
		return onUserScreen(p1.vect2().dot(viewVecPerp), p1.z(), p2.vect2().dot(viewVecPerp), p2.z(), buffer);
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
		Vect2 c = new Vect2(mid_xval, mid_yval);
		Vect2 mapped = viewVecPerp.Scal(userMapX(x)).Add(viewVec.Scal(viewVec.dot(c)));
		//Vect2 mapped = viewVecPerp.Scal(userMapX(x));
		double sz = userMapY(y); 
		Position pos;
		if (isLatLon()) {
			pos = Position.mkLatLonAlt(mapped.y(), mapped.x(), sz);
		} else {
			pos = Position.mkXYZ(mapped.x(), mapped.y(), sz);
		}
		return pos;
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


	public void setLimits(ViewPortVert vph) {		
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
		//f.pln(" $$$$  latlon "+isLatLon());
		//f.pln(" $$$$  ViewPlansPolyStates.findPlanMinMax: min_xval = "+min_xval+" 	max_xval = "+max_xval);
		//f.pln(" $$$$  ViewPlansPolyStates.findPlanMinMax: min_yval = "+min_yval+" 	max_yval = "+max_yval);
		//f.pln(" $$$$  ViewPlansPolyStates.findPlanMinMax: min_zval = "+min_zval+" 	max_zval = "+max_zval);
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

	public boolean segmentOffScreen(Position p1, Position p2) {
		int x1 = screenMapX(p1);
		int x2 = screenMapX(p2);
		int y1 = screenMapY(p1);
		int y2 = screenMapY(p2);
		return (x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) || (x1 > getWidth() && x2 > getWidth()) || (y1 > getHeight() && y2 > getHeight()); 
	}

	public void zoomIn() {
		double scale_x = getScaleX() * 1.2;
		double scale_y = getScaleY() * 1.2;
		scale_x = Util.min(scale_x, 1000000.0); // keep scale from being infinite
		scale_y = Util.min(scale_y, 1000000.0); // keep scale from being infinite
		//f.pln(" $$$$$ ViewPlansPolysStatesVert.zoomIn: scale_x = "+scale_x+" scale_y = "+scale_y);
		setScale(scale_x,scale_y);
	}

	public void zoomOut() {
		findMinMaxX();
		double scale_x = getScaleX() / 1.2;
		double scale_y = getScaleY() / 1.2;
		if (getRangeX() > 4.0*(projected_max_x-projected_min_x)) scale_x = getScaleX();
		if (getRangeY() > 4.0*Util.max(max_zval-min_zval, Units.from("ft",1.0))) scale_y = getScaleY();
		scale_x = Util.max(scale_x, 0.001); // keep scale from being zero
		scale_y = Util.max(scale_y, 0.001); // keep scale from being zero
		//f.pln(" $$$$$ ViewPlansPolysStatesVert.zoomOut: scale_x = "+scale_x+" scale_y = "+scale_y);
		setScale(scale_x,scale_y);
	}	


	public void zoomYonlyIn() {
		double scale_y = getScaleY() * 1.2;
		scale_y = Util.min(scale_y, 1000000.0); // keep scale from being infinite
		//f.pln(" $$$$$ ViewPlansPolysStatesVert.zoomYonlyIn: scale_y = "+scale_y);
		setScale(getScaleX(),scale_y);
	}

	public void zoomYonlyOut() {
		findMinMaxX();
		double scale_y = getScaleY() / 1.2;
		if (getRangeY() > 4.0*Util.max(max_zval-min_zval, Units.from("ft",1.0))) scale_y = getScaleY();
		scale_y = Util.max(scale_y, 0.001); // keep scale from being zero
		//f.pln(" $$$$$ ViewPlansPolysStatesVert.zoomYonlyOut: scale_y = "+scale_y);
		setScale(getScaleX(),scale_y);
	}	


	double screenDistanceV(int x1, int y1, int x2, int y2) {
		Position p1 = inversePosition(x1,y1);
		Position p2 = inversePosition(x2,y2);
		return p1.distanceV(p2);
	}


	public void setViewDirection(Vect2 v) {
		viewVec = v.Hat();
		viewVecPerp = viewVec.PerpR();
		double dirAngle = viewVec.trk();
		view_WE = Math.abs(dirAngle) < Math.PI/4;
		view_EW = Math.abs(dirAngle) > 3*Math.PI/4;
		view_NS = !view_WE && !view_EW && dirAngle >= 0;
		view_SN = !view_WE && !view_EW && dirAngle < 0;

		//scale();
		repaint();
	}

	public Vect2 getViewDirection() {
		return viewVec;
	}

	private void findMinMaxX() {
		double t1 = new Vect2(max_xval,max_yval).dot(viewVecPerp);
		double t2 = new Vect2(max_xval,min_yval).dot(viewVecPerp);
		double t3 = new Vect2(min_xval,max_yval).dot(viewVecPerp);
		double t4 = new Vect2(min_xval,min_yval).dot(viewVecPerp);
		projected_max_x = Util.max(Util.max(t1,t2),Util.max(t3,t4));
		projected_min_x = Util.min(Util.min(t1,t2),Util.min(t3,t4));	
		//f.pln(" $$ ViewPlansPolysStates: findMinMaxX min_x = "+min_x+" max_x = "+max_x);		
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
			Pair<Double,Double> p = simpleConvToLatLon(xy.x(),xy.y());
			int xoffset = screenMapX(p.second) - screenMapX(0.0);

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
			TcpData tcp = plan.getTcpData(i+1);
			if (onUserScreen(np0.position(), np.position(), 10)) {
				if (plan.isTrkTCP(i+1)) g.setColor(Turn_Point_Color);
				//if (iFP.point(j).isTurnMid()) ptColor = TCPTurnPointColor;
				if (plan.isVsTCP(i+1)) g.setColor(VSC_Point_Color);
				if (plan.isGsTCP(i+1)) g.setColor(GSC_Point_Color);
				if (dots) drawPt(g,plan.point(i+1).position(), pointRadius);
				if (names) {
					if ( ! np.label().contains("$")) {
						drawString(g, "     "+np.label(), np.position(),0,0);
					}
				}
				g.setColor(ptColor);
				drawSegArrow(g,plan,plan.time(i),plan.time(i+1),arrows,curves);
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
		boolean kin = (!plan.isLinear() && plan.inVsChange(t1));
		if (showInterpolations && kin) {
			//drawPt(g, p1);
			int dx = pixelDistanceX(p1,p2);
			int dy = pixelDistanceY(p1,p2);
			if (dx <= 1 && dy <= 1) {
				return; // same on screen, draw nothing
			} else if ((dx < 20 && dy < 20) || p1.almostEquals(p2) || Math.abs(t1-t2) < Constants.get_time_accuracy()) {
				//				Vect2 p1v2 = new Vect2(getXzView() ? p1.x() : p1.y(),p1.z());
				//				Vect2 p2v2 = new Vect2(getXzView() ? p2.x() : p2.y(),p2.z());
				Vect2 p1v2 = new Vect2(p1.vect2().dot(viewVecPerp),p1.z());
				Vect2 p2v2 = new Vect2(p2.vect2().dot(viewVecPerp),p2.z());
				if (head) {
					drawArrow(g, p1v2, p2v2);
				} else {
					drawLine(g, p1v2, p2v2);
				}
			} else {
				double mid = (t1+t2)/2.0;
				drawSegArrow(g,plan, t1,mid,false,showInterpolations);
				drawSegArrow(g,plan, mid,t2,head,showInterpolations);
			}
		} else {
			//			Vect2 p1v2 = new Vect2(getXzView() ? p1.x() : p1.y(),p1.z());
			//			Vect2 p2v2 = new Vect2(getXzView() ? p2.x() : p2.y(),p2.z());
			Vect2 p1v2 = new Vect2(p1.vect2().dot(viewVecPerp),p1.z());
			Vect2 p2v2 = new Vect2(p2.vect2().dot(viewVecPerp),p2.z());
			if (head) {
				drawArrow(g, p1v2, p2v2);
			} else {
				drawLine(g, p1v2, p2v2);
			}
		}
	}

	/**
	 * Draw a line from p1 to p2, optionally including an arrowhead
	 * @param g
	 * @param p1
	 * @param p2
	 * @param head if true, include arrowhead
	 */
	protected void drawLine(Graphics g, Position p1, Position p2, boolean head) {
		Vect2 p1v2 = new Vect2(p1.vect2().dot(viewVecPerp),p1.z());
		Vect2 p2v2 = new Vect2(p2.vect2().dot(viewVecPerp),p2.z());
		if (head) {
			drawArrow(g,p1v2,p2v2);
		} else {
			drawLine(g,p1v2,p2v2);
		}
	}

	protected void drawLine(Graphics g, Position p1, Position p2) {
		drawLine(g,p1,p2,false);
	}

	protected void drawArrow(Graphics g, Position p1, Position p2) {
		drawLine(g,p1,p2,true);
	}

	protected void drawPolygon(Graphics g, SimplePoly p, boolean filled) {
		ArrayList<Vect2> pts = new ArrayList<Vect2>();
		double minX = Double.MAX_VALUE;
		double maxX = -Double.MAX_VALUE;
		int minScreenX = Integer.MAX_VALUE;
		int maxScreenX = Integer.MIN_VALUE;
		for (int i = 0; i < p.size(); i++) {
			Position pp = p.getVertex(i);
			int xi = screenMapX(pp);
			if (xi > maxScreenX) {
				maxScreenX = xi;
				maxX = pp.vect2().dot(viewVecPerp);
			}
			if (xi < minScreenX) {
				minScreenX = xi;
				minX = pp.vect2().dot(viewVecPerp);
			}
		}
		pts.add(new Vect2(minX,p.getTop()));
		pts.add(new Vect2(maxX,p.getTop()));
		pts.add(new Vect2(maxX,p.getBottom()));
		pts.add(new Vect2(minX,p.getBottom()));
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
				drawLine(g,prev,sp.centroid(),arrows);
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

}//class ViewPortVert



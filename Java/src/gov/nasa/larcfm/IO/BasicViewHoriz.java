/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;



import gov.nasa.larcfm.Util.BoundingRectangle;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.PolyPath;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Triple;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.util.ArrayList;
import java.util.List;

public class BasicViewHoriz extends ViewPortHoriz {
	private static final long serialVersionUID = 1L;
	/** The plans */
	private volatile ArrayList<Plan> aclist;
	/** The polys */
	private volatile ArrayList<PolyPath> pplist;
	/** The states */
	private volatile ArrayList<Triple<String,Position,Velocity>> states;

	private double simTime;

	public BasicViewHoriz() {
		super(false);
		aclist = new ArrayList<Plan>(0);
		pplist = new ArrayList<PolyPath>(0);
		states = new ArrayList<Triple<String,Position,Velocity>>(0);
		simTime = 0;
	}

	public void addPlan(Plan p) {
		if (p != null) aclist.add(p);
	}

	public void addPlans(List<Plan> p) {
		if (p != null) aclist.addAll(p);
	}

	public void addPath(PolyPath p) {
		if (p != null) pplist.add(p);
	}

	public void addPaths(List<PolyPath> p) {
		if (p != null) pplist.addAll(p);
	}

	public void addState(Triple<String,Position,Velocity> p) {
		if (p != null) states.add(p);
	}

	public void addStates(List<Triple<String,Position,Velocity>> p) {
		if (p != null) states.addAll(p);
	}

	public void setSimTime(double t) {
		simTime = t;
	}


	public void paintComponent(Graphics g) {
		super.paintComponent(g);	
		//TODO take these font objects out of this method and make them public static finals.
		Font f1 = new Font(g.getFont().getFontName(),g.getFont().getStyle(),14);
		Font f2 = new Font(g.getFont().getFontName(),g.getFont().getStyle(),11);		


		// draw scale
		g.setColor(darkBlue);
		int scale_x = getWidth()-20;
		int scale_y = getHeight()-20;
		g.drawLine(scale_x-scaleLength,scale_y,scale_x,scale_y);
		g.drawLine(scale_x-scaleLength,scale_y-5,scale_x-scaleLength,scale_y);
		g.drawLine(scale_x,scale_y-5,scale_x,scale_y);
		drawString(g, scaleLabel, scale_x - scaleLength/2, scale_y - 2, 0, -1);

		int aclistBaseColor = 0;
		int statesBaseColor = aclist.size(); 
		int pplistBaseColor = statesBaseColor + pplist.size();

		
		
		// ------------------ draw polys -------------------------------------------------------------------------------
		for (int i = 0; i < pplist.size(); i++) {
			PolyPath pp = pplist.get(i);
			Color plColor = getColor(i+pplistBaseColor);
			drawPaths(g, plColor, pp, true, simTime);
		}
		// ------------------ draw plans -------------------------------------------------------------------------------
		for (int i = 0; i < aclist.size(); i++) {
			Plan ac = aclist.get(i);
			Color plColor = getColor(i+aclistBaseColor);
			drawPlan(g, plColor, ac);
			if (ac.isTimeInPlan(simTime)) {
				Position pos = ac.position(simTime);
				Velocity vel = ac.velocity(simTime);	        	       
				drawAircraft(g, ac.getName(), pos, vel,simTime, false, plColor);
			}
		}
		// ------------------ draw states -------------------------------------------------------------------------------
		for (int i = 0; i < states.size(); i++) {
			Color plColor = getColor(i+statesBaseColor);
			String name = states.get(i).first;
			Position pos = states.get(i).second;
			Velocity vel = states.get(i).third;	        	       
			drawAircraft(g, name, pos, vel,simTime, true, plColor);			
		}

	}// paintComponent(Graphics g)



	/**
	 * This method finds the minimum and maximum values of all the plans, and also rescales and recenters the display
	 */
	public void initScale() {		
		min_seg_length = 1E15;
		max_seg_length = -1E15;
		int num_segs  = 0;
		double total_seg_length = 0.0;		
		min_xval = 1E15;
		max_xval = -1E15;
		min_yval = 1E15;
		max_yval = -1E15;
		min_zval = 1E15;
		max_zval = -1E15;
		//min_tval = 1E15;
		//max_tval = -1E15;
		//f.pln(" $$$$ ViewPlansPolyStates.findPlanMinMax: plans.size() = "+plans.size());
		for (Plan plan: aclist) {
			if (plan == null) {
				f.pln(" $$$$$$$$$ findPlanMinMax ERROR: null plan in list ????????????");
				continue;
			}
			BoundingRectangle b = plan.getBound();
			if (b.getMinX() < min_xval) min_xval = b.getMinX();
			if (b.getMinY() < min_yval) min_yval = b.getMinY();
			if (b.getMinZ() < min_zval) min_zval = b.getMinZ();
			if (b.getMaxX() > max_xval) max_xval = b.getMaxX();
			if (b.getMaxY() > max_yval) max_yval = b.getMaxY();
			if (b.getMaxZ() > max_zval) max_zval = b.getMaxZ();

			//if (st < min_tval) min_tval = st;
			//if (st > max_tval) max_tval = st;			
			for (int i = 0; i < plan.size()-2; i++) {
				num_segs++;
				double dist = plan.pathDistance(i); 
				if (dist > max_seg_length) {
					max_seg_length = dist;
				}
				if (dist < min_seg_length) {
					min_seg_length = dist;
				}
				total_seg_length += dist;
			}			
		}//for

		for (PolyPath path: pplist) {
			BoundingRectangle b = new BoundingRectangle();
			Position p1 = path.getPolyRef(0).centroid();
			Position p2 = path.getPolyRef(path.size()-1).centroid();
			b.add(p1);
			b.add(p2);
			if (b.getMinX() < min_xval) min_xval = b.getMinX();
			if (b.getMinY() < min_yval) min_yval = b.getMinY();
			if (b.getMinZ() < min_zval) min_zval = b.getMinZ();
			if (b.getMaxX() > max_xval) max_xval = b.getMaxX();
			if (b.getMaxY() > max_yval) max_yval = b.getMaxY();
			if (b.getMaxZ() > max_zval) max_zval = b.getMaxZ();
		}//for

		for (Triple<String,Position,Velocity> ac: states) {
			double x = ac.second.lon();
			double y = ac.second.lat();
			double z = ac.second.alt();
			if (x < min_xval) min_xval = x;
			if (y < min_yval) min_yval = y;
			if (z < min_zval) min_zval = z;
			if (x > max_xval) max_xval = x;
			if (y > max_yval) max_yval = y;
			if (z > max_zval) max_zval = z;
		}//for

		mid_xval = (max_xval - min_xval) / 2.0 + min_xval;
		mid_yval = (max_yval - min_yval) / 2.0 + min_yval;
		mid_zval = (max_zval - min_zval) / 2.0 + min_zval;

		avg_seg_length = total_seg_length / num_segs;

		if (aclist.size() == 0 && pplist.size() == 0 && defaultScaleToContinentalUS) {
			setScaleContinentalUS();
		}

		setLimits(this);
	}

	private void drawPlan(Graphics g, Color ptColor, Plan p) {
		//f.pln(" ############ drawPlans "+p.getName()+" p.size() = "+p.size());
		g.setColor(ptColor);
		int pointRadius = 3;
		drawPlan(g, p, false, true, true, true, ptColor, pointRadius);
	}

	private void drawAircraft(Graphics g, String id, Position pos, Velocity v, double simTime, boolean isState, Color color) {
		drawPt(g,pos);
		int size_scale = 9;
		int font_size = 12;
		drawChevron(g, pos, v, color, size_scale);
		// draw label
		String space = "     ";
		String arrow = "";
		String hdg = "000";
		String fl = "000";
		double heading = v.track("deg");
		double flightLevel = Units.to("ft", pos.alt())/100.0;
		g.setColor(color);
		if (v.verticalSpeed("fpm")>100.0) {
			arrow = "\u2191"; 	// up arrow (climbing)
		} else if (v.verticalSpeed("fpm")<-100.0) {
			arrow = "\u2193"; 	// down arrow (descending)
		}
		if (heading<0) heading += 360;
		hdg = hdg + f.Fm0(heading);			// this ensures I can capture a 3-character heading representation
		fl = fl + f.Fm0(flightLevel);		// this ensures I can capture a 3-character FL representation
		g.setFont(new java.awt.Font("Custom", 0, font_size));
		String text_block = space + id +"\n"
				+space+"FL"+fl.substring(fl.length()-3, fl.length())+arrow+"\n"
				+space+f.Fm0(Units.to("kts", v.gs()))+"kts"+" "+hdg.substring(hdg.length()-3, hdg.length());
		drawString(g, text_block,  pos, -1, -1);
		//		f.pln(simTime+" $$$ drawAircraft: "+id+" isState = "+isState);
	}


	private void drawPaths(Graphics g, Color c, PolyPath pp, boolean fill, double simTime) {
		SimplePoly sp = pp.interpolate(simTime);
		if (sp != null) {
			g.setColor(c);
			this.setLineStyleExtraBold(g);
			drawPolygon(g,sp,false);
			this.clearLineStyle(g);
			if (fill) {
				Color transparent = makeColorAlpha(c, 150);
				g.setColor(transparent);
				drawPolygon(g,sp,true);
			}
			g.setColor(Color.ORANGE.darker().darker());
			drawString(g, pp.getName(), sp.centroid(), 0, 0);
		}
	}


}

/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.IO;

import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;
import gov.nasa.larcfm.Util.Util;

import java.awt.Color;
import java.awt.Graphics;
import java.util.TreeMap;

/**
 * A 2-D functional graph implemented over a viewport.
 * Values within a series are mapped as f(x) -> y, so all x values must be unique (repeated x values will replace earlier pairs). 
 * 
 */
public class ViewGraph2D extends ViewPort {

	private static final long serialVersionUID = 1L;

	public static enum GraphType {SCATTER, LINE, BAR};

	private GraphType mode = GraphType.LINE;
	
	private boolean showMarked = true;

	private String xunit = "unitless";
	private String yunit = "unitless";
	private boolean labelPoints = false;
	private boolean labelSlopes = false;

	private String title = "Graph View";

	private double minx = Double.MAX_VALUE;
	private double miny = Double.MAX_VALUE;
	private double maxx = -Double.MAX_VALUE;
	private double maxy = -Double.MAX_VALUE;

	private String pickSeries = "";
	private double pickX = 0.0;
	private double pickY = 0.0;

	public ViewGraph2D() {
		super(true);
	}

	// this is a nested map of series name -> x coord -> y coord.
	private TreeMap<String,TreeMap<Double,Double>> data = new TreeMap<String,TreeMap<Double,Double>>();
	
	// this is a nested map of series -> xcoord -> marking level (user defined)
	private TreeMap<String,TreeMap<Double, Integer>> marked = new TreeMap<String,TreeMap<Double, Integer>>();

	private TreeMap<String,Color> colors = new TreeMap<String,Color>();
	
	/**
	 * Add data to be displayed, ignoring infinite or NaN entries
	 * @param series name of series
	 * @param xval x value
	 * @param yval y value
	 */
	public void addData(String series, double xval, double yval) {
		if (Double.isFinite(xval) && Double.isFinite(yval)) {
			TreeMap<Double, Double> innerMap = data.get(series);
			if (innerMap == null) {
				innerMap = new TreeMap<Double,Double>();
				data.put(series, innerMap);
			}
			innerMap.put(xval,yval);
		}
//f.pln("ViewGraph2D addData "+series+" "+xval+" "+yval);		
	}

	/**
	 * Add a set of data values
	 * @param series series name of this data
	 * @param xs x values for this series
	 * @param ys y values for this series
	 * Note: if xs and ys are of different length, any unmatched values will be ignored.
	 */
	public void addSeriesData(String series, double[] xs, double[] ys) {
		int min = Util.min(xs.length,  ys.length);
		for (int i = 0; i < min; i++) {
			addData(series, xs[i], ys[i]);
		}
	}
	
	/**
	 * Mark a given point in a series with a (non-zero) value
	 * @param series name of series
	 * @param xval x value
	 * @param mark mark value to be set, 0 is "clear"
	 * Note: there are currently 6 different mark symbols defined, indicated by values 1-6 (all other values default to the #1 mark):
	 * 1-2 circles (empty, filled), 3-4 triangles (up,down), 5-6 squares (empty,filled)
	 * These are drawn over the normal point.
	 */
	public void markPoint(String series, double xval, int mark) {
		TreeMap<Double, Integer> innerMap = marked.get(series);
		if (innerMap == null) {
			innerMap = new TreeMap<Double,Integer>();
			marked.put(series, innerMap);
		}
		innerMap.put(xval,mark);
	}
	
	/**
	 * Return the mark value of an item
	 * @param series series name
	 * @param xval x-value of point
	 * @return mark value of point, 0 = not set
	 */
	public int getMark(String series, double xval) {
		TreeMap<Double, Integer> innerMap = marked.get(series);
		if (innerMap != null) {
			Integer ret = innerMap.get(xval);
			if (ret != null) {
				return ret;
			}
		}
		return 0;
	}
	
	/**
	 * Clear all marks
	 */
	public void clearMarks() {
		marked.clear();
	}

	/**
	 * Clear a single series of marks
	 * @param s series
	 */
	public void clearMarks(String s) {
		TreeMap<Double,Integer> innerMap = marked.get(s);
		if (innerMap != null) {
			innerMap.clear();
		}
	}

	/**
	 * Assign a given color to a series
	 * @param series series name
	 * @param c color value
	 */
	public void setSeriesColor(String series, Color c) {
		colors.put(series,c);
	}
	
	/**
	 * Return series colors colors to default values
	 */
	public void clearSeriesColors() {
		colors.clear();
	}
	
	/**
	 * Switch x and y for given series
	 * @param series
	 */
	public void transpose(String series) {
		TreeMap<Double,Double> innerMap = data.get(series);
		if (innerMap != null) {
			TreeMap<Double,Double> newInnerMap = new TreeMap<Double,Double>();
			for (double x : innerMap.keySet()) {
				newInnerMap.put(innerMap.get(x), x);
			}
			data.put(series, newInnerMap);
		}
	}

	public void setXUnit(String s) {
		xunit = s;
	}

	public void setYUnit(String s) {
		yunit = s;
	}

	public void setPanelName(String s) {
		title = s;
	}

	public String getPanelName() {
		return title;
	}

	public void setGraphType(GraphType g) {
		mode = g;
	}

	public GraphType getGraphType() {
		return mode;
	}
	
	public boolean getLabelPoints() {
		return labelPoints;
	}

	public void setLabelPoints(boolean b) {
		labelPoints = b;
	}

	public boolean getLabelSlopes() {
		return labelSlopes;
	}

	public void setLabelSlopes(boolean b) {
		labelSlopes = b;
	}

	/**
	 * Clear a single series of data and marks
	 */
	public void clear(String series) {
		TreeMap<Double,Double> innerMap = data.get(series);
		if (innerMap != null) {
			innerMap.clear();
		}
		clearMarks(series);
	}

	/**
	 * Clear all data and marks
	 */
	public void clear() {
		data.clear();
		marked.clear();
	}

	public void zoomIn() {
		double xscal = getScaleX();
		double yscal = getScaleY();
		xscal*=2.0;
		yscal*=2.0;
		xscal = Util.min(xscal, 1000000.0);
		yscal = Util.min(yscal, 1000000.0);
		setScale(xscal,yscal);
	}
	public void zoomOut() {
		double xscal = getScaleX();
		double yscal = getScaleY();
		if (getRangeX() > 8.0*(maxx-minx) || getRangeY() > 8.0*(maxy-miny)) return;
		xscal/=2.0;
		yscal/=2.0;
		xscal = Util.max(xscal, 0.01);
		yscal = Util.max(yscal, 0.01);
		setScale(xscal,yscal);
	}


	public void initScale() {
		minx = Double.MAX_VALUE;
		miny = Double.MAX_VALUE;
		maxx = -Double.MAX_VALUE;
		maxy = -Double.MAX_VALUE;
		for(String series : data.keySet()) {
			for (double x : data.get(series).keySet()) {
				double y = data.get(series).get(x);
				minx = Util.min(minx, x);
				miny = Util.min(miny, y);
				maxx = Util.max(maxx, x);
				maxy = Util.max(maxy, y);
			}
		}	
		double dx = maxx-minx;
		double dy = maxy-miny;


		//		f.pln(" $$$$ minx = "+minx+"  miny = "+miny+" maxx = "+maxx+" maxy = "+maxy);
		super.setUserArea(minx-0.1*dx,miny-0.1*dy,maxx+0.1*dx,maxy+0.1*dy);
	}

	public Vect2 inversePosition2(int xpos, int ypos) {
		double xx = super.userMapX(xpos);
		double yy = super.userMapY(ypos);
		return new Vect2(xx,yy);
	}
	
	public Position inversePosition(int xpos, int ypos) {
		double xx = super.userMapX(xpos);
		double yy = super.userMapY(ypos);
		return Position.makeXYZ(xx,yy,0.0);
	}
	
	public void setPickPoint(String series, double x) {
		pickSeries = series;
		pickX = x;
	}
	
	public String getPickSeries() {
		return pickSeries;
	}

	public double getPickX() {
		return pickX;
	}

	public double getPickY() {
		return pickY;
	}

	public int pickPoint(int xpos, int ypos) {
		double xx = super.userMapX(xpos);
		double yy = super.userMapY(ypos);

		double mindx = 10*(super.userMapX(1)-super.userMapX(0));
		double mindy = 10*(super.userMapY(0)-super.userMapY(1));

		boolean ret = false;
		
		pickSeries = "";
		pickX = 0.0;
		pickY = 0.0;

		for(String series : data.keySet()) {
			for (double x : data.get(series).keySet()) {
				double y = data.get(series).get(x);
				double dx = Math.abs(x-xx);
				double dy = Math.abs(y-yy);
				if (dx+dy < mindx+mindy) {
					mindx = dx;
					mindy = dy;
					pickSeries = series;
					pickX = x;
					pickY = y;
					ret = true;
				}
			}
		}	
		return ret ? 1 : 0;
	}

	public void paintComponent(Graphics g) {
		super.setAxis(10, true, xunit, 10, true, yunit);
//		int precisionX = (int)Math.ceil(Math.log10(1/Units.to(xunit, Math.abs(screenMapX(getWidth()) - screenMapX(0)))/10));
//		xGridPrecision = 0;		
//		if (precisionX > 0)	xGridPrecision = Util.min(precisionX, 6);
//		int precisionY = (int)Math.ceil(Math.log10(1/Units.to(yunit, Math.abs(screenMapY(getHeight()) - screenMapY(0)))/10));
//		yGridPrecision = 0;
//		if (precisionY > 0)	yGridPrecision = Util.min(precisionY, 6);
		super.paintComponent(g);

		int colorIndex = 0;

		//f.pln(" $$$$ scaleX = "+super.getScaleX()+" scaleY = "+super.getScaleY());

		for(String series : data.keySet()) {
			boolean matchSeries = series.equals(pickSeries);
			Vect2 prevPt = null;
			Vect2 prevPt2 = null;
			for (double x : data.get(series).keySet()) {
				double y = data.get(series).get(x);

				boolean match = matchSeries && x == pickX;
				
				Color c = colors.get(series);
				if (c == null) {
						c = getColor(colorIndex);
				}
				g.setColor(c);
				Vect2 pt = new Vect2(x,y);
				// scatter graph 
				drawCircle(g, pt, 2, false);
				if (showMarked && getMark(series,x) != 0) {
					g.setColor(c.darker());
					if (getMark(series,x) == 2) {
						drawCircle(g,pt,4,true);
					} else if (getMark(series,x) == 3) {
						drawTriangle(g, pt, 8, 1);
					} else if (getMark(series,x) == 4) {
						drawTriangle(g, pt, 8, -1);
					} else if (getMark(series,x) == 5) {
						drawSquare(g, pt, 8, false);
					} else if (getMark(series,x) == 6) {
						drawSquare(g, pt, 8, true);
					} else {
						drawCircle(g,pt,4,false); //1 or other
					}
					g.setColor(c);
				}
				
				if (match) {
					g.setColor(Color.RED);
					drawCircle(g,pt,10,false);
					g.setColor(c);
				}
				if (labelPoints || match) {
					String s = series+" ("+Units.str(xunit, x)+","+Units.str(yunit, y)+")";
					int lr = 1;
					int tb = 1;
					if (prevPt != null) {
						if (prevPt.y > pt.y) lr = -1;
						if (prevPt.y < pt.y) tb = -1;
					}
					super.drawString(g, s, x, y, lr, tb);
				}
				// line graph
				if (mode == GraphType.LINE && prevPt != null) {
					super.drawLine(g, prevPt, pt);

					if (labelSlopes) {
						double slope = Units.to(yunit, y-prevPt.y)/Units.to(xunit, x-prevPt.x);
						Vect2 mid = pt.Add(prevPt).Scal(0.5);
						super.drawString(g, series+" "+f.Fm2(slope)+yunit+"/"+xunit, mid, 1, 1);
					}
				}
				if (mode == GraphType.BAR && prevPt != null) {
					double dright = (x-prevPt.x)/2.0;
					double dleft = dright;
					if (prevPt2 != null) {
						dleft = (prevPt.x - prevPt2.x)/2.0;
					}
					Vect2 lowerleft;
					Vect2 upperright;
					if (prevPt.y >= 0) {
						lowerleft = new Vect2(prevPt.x-dleft, 0);
						upperright = new Vect2(prevPt.x+dright, prevPt.y);
					} else {
						lowerleft = new Vect2(prevPt.x-dleft, prevPt.y);
						upperright = new Vect2(prevPt.x+dright, 0);
					}
					g.setColor(makeColorAlpha(c,100));
					super.drawRect(g, lowerleft, upperright, true);
				}

				prevPt2 = prevPt;
				prevPt = pt;
			}
			
			// draw last shapes
			if (mode == GraphType.BAR && prevPt2 != null) {
				double dleft = (prevPt.x - prevPt2.x)/2.0;
				double dright = dleft;
				Vect2 lowerleft;
				Vect2 upperright;
				if (prevPt.y >= 0) {
					lowerleft = new Vect2(prevPt.x-dleft, 0);
					upperright = new Vect2(prevPt.x+dright, prevPt.y);
				} else {
					lowerleft = new Vect2(prevPt.x-dleft, prevPt.y);
					upperright = new Vect2(prevPt.x+dright, 0);
				}
				super.drawRect(g, lowerleft, upperright, true);
			}

			
			colorIndex++;
		}
	}

	@Override
	public void setCenter(Position ss) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public int screenMapX(Position p) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int screenMapY(Position p) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Vect2 position2Vect(Position p) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Vect2 vel2Vect(Velocity v) {
		// TODO Auto-generated method stub
		return null;
	}

}

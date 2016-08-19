/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.Hashtable;
import java.util.List;

public class DensityGridTimed extends DensityGrid {

	protected double lookaheadEndTime;
	protected double gs;
//	// search range:
//	double timeBefore = -1;
//	double timeAfter = -1;
//	double interval = -1;
	
	
	public DensityGridTimed(BoundingRectangle b, NavPoint start, Position end, double startT, double groundSpeed, int buffer, double sqSz, boolean ll) {
		super(b, start, end, buffer, sqSz, ll);
		startTime = startT;
		lookaheadEndTime = -1.0;
		gs = groundSpeed;
	}

	// grid coordinates refer to the bottom left (SW) corner of the square
	// buffer is number of extra squares in each direction from the base rectangle
	public DensityGridTimed(Plan p, int buffer, double squareSize) {
	 super(p, buffer, squareSize);
	 startTime = p.getFirstTime();
	 gs = p.averageGroundSpeed();
	}

//	/**
//	 * Set a time range around a plan for searching.  If any of these are negative, do not have any extra range.
//	 * @param before positive (relative) time before the final plan to be clear
//	 * @param after positive (relative) time after the final plan to be clear
//	 * @param interv time interval for checks between before and after
//	 */
//	public void setTimeRange(double before, double after, double interv) {
//		timeBefore = before;
//		timeAfter = after;
//		interval = interv;
//	}
	
	public double getLookaheadEndTime() {
		return lookaheadEndTime;
	}

	public void setLookaheadEndTime(double t) {
		lookaheadEndTime = t;
	}
	
	//TODO: this implements the lookahead time by shortcuttting weight lookup
	public double getWeight(int x, int y, double t) {
		if (lookaheadEndTime > 0 && t > lookaheadEndTime) return 0.0;
		if (!weights.containsKey(Pair.make(x,y))) return Double.POSITIVE_INFINITY;
		return weights.get(Pair.make(x,y));
	}

//	// weight from t to endT
//	public double getWeight(int x, int y, double t, double endT) {
//		if (endT < t) return getWeight(x,y,t);
//		
//		double cost = -1.0;
//		for (double tt = t; Double.isFinite(cost) && tt <= endT; tt += 1.0) {
//			cost = Math.max(cost, getWeight(x,y,tt));
//		}
//		return cost;
//	}

	
	public double getWeight(Pair<Integer,Integer> xy, double t) {
		return getWeight(xy.first,xy.second,t);
	}

	public double getWeight(Triple<Integer,Integer,Integer> pii) {
		return getWeight(pii.first,pii.second, pii.third);
	}
	
	
	
	public double startTime() {
		return startTime;
	}

	public List<Pair<Integer,Integer>> optimalPath() {
		return optimalPath(this.gs);
	}

//	// too inefficient
//	public List<Pair<Integer,Integer>> optimalPathDuration(double endT) {
//		DensityGridTimedSearch dgs = new DensityGridAStarSearch();
//		List<Pair<Integer,Integer>> r = dgs.search(this, this.startPoint, this.endPoint, this.startTime, this.gs, endT);
//		return r; 
//	}

	public List<Pair<Integer,Integer>> optimalPath(double gs) {
		DensityGridTimedSearch dgs = new DensityGridAStarSearch();
		return dgs.search(this, this.startPoint, this.endPoint, this.startTime, gs);
	}


}

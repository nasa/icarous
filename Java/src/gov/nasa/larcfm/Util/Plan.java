/*
 * Plan -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * Authors: George Hagen,  Jeff Maddalon,  Rick Butler
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

/**
 * Manages a flight Plan as a sequence of 4D vectors (three dimensions of space
 * and time).
 * <p>
 *
 * This class assumes that (1) the points are added in increasing time, (2) all
 * point times are 0 or positive, (3) there are no duplicate point times.
 * <p>
 * 
 * Points that "overlap" with existing points are deemed as redundant and will
 * not be included in the Plan. Two points overlap if they are within both a
 * minimum distance and time of each other (as defined by minimumDtTime).
 * <p>
 * 
 * Furthermore this version of the Plan assumes that any points between a
 * beginning and paired end TCP will have constant acceleration (or turn
 * radius). These values will be derived from the beginning and end points. It
 * is not recommended that these Plans be directly modified. Rather the (linear)
 * Plans they are based on should be modified and new kinematic Plans be
 * generated from those.
 * <p>
 * 
 * Currently (as of v 2.0.0), it is assumed that kinematic versions of Plans
 * will be constructed algorithmically. One potential way to do this is using
 * the Util.TrajGen.makeKinematicPlan() method to construct a kinematic Plan
 * core from a linear one (and use Util.TrajGen.makeLinearPlan to do the
 * inverse).
 * <p>
 *
 * Acceleration points are designated as Beginning Of Turn, Middle Of Turn, End
 * of Turn, Ground Speed Change Begin, Ground Speed Change End, Vertical Speed
 * Change Begin, and Vertical Speed Change End. In general, the end points are
 * not considered to be part of the maneuver. All horizontal acceleration
 * regions must be distinct and not overlap (there must be a non-accelerating
 * segment of some length between them). Similarly all vertical acceleration
 * regions must not overlap (there must be a non-accelerating segment of some
 * length between them). Vertical and horizontal regions, however, may overlap.
 * Acceleration start TCP points will contain meta data including acceleration
 * and initial velocity. For other points, velocities are inferred from
 * surrounding points.
 * <p>
 * 
 * There can also be points marked with a "minor velocity change" flag. These
 * points can indicate the known existence of a smaller velocity change than can
 * be expressed by non-overlapping TCP pairs, and are primarily used for
 * consistency checks.
 * <p>
 * 
 * More so than linear Plan cores, Plans accommodate the concept of an "existing
 * velocity" at the first point. If there is a valid existing velocity value,
 * then that is assumed to be the initial velocity at the first point -- this is
 * used, for example, when building a Plan that starts from the "current " that
 * starts from a velocity of zero (or some other non-inferred value), allowing
 * an acceleration on the first Plan segment.
 * <p>
 * 
 * Typical Usage:
 * <ol>
 * <li>create a Plan from a linear Plan using TrajectoryGen.makePlan( ... )
 * <li>compute position as a function of time using Plan.position(t)
 * </ol>
 */

public class Plan implements ErrorReporter, Cloneable {
	protected String name;
	protected ArrayList<NavPoint> points;
	protected ErrorLog error;
	protected int errorLocation;
	private BoundingRectangle bound; // TODO: this bound only applies to added
										// points, when points are deleted, this
										// is not accommodated, or if points are
										// updated, this is not accommodated
	private static boolean debug = false;
	protected String note = "";
	public static final double minDt = GreatCircle.minDt; // points that are closer together in time than this are treated as
														  // the same point for velocity calculations, mergeClosePoints merges points
	                                                      // closer together in time than this parameter.
	public static final String specPre = "$";             // special prefix for labels
	// static final double revertGsTurnConnectionTime = 5; // if a turn segment and GS segment are this close in time, then assume they are related

	public static enum WayType {
		Orig, Virtual, AltPreserve
	};

	public static enum TrkTcpType {
		NONE, BOT, EOT, EOTBOT
	};
	public static enum GsTcpType  {
		NONE, BGS, EGS, EGSBGS
	};
	public static enum VsTcpType  {
		NONE, BVS, EVS, EVSBVS
	};

	public static final int TCP_OUTPUT_COLUMNS = 20; // total number of output columns for full TCP not including aircraft name)
	public static final int MIN_OUTPUT_COLUMNS = 5; // total number of output
													// columns for linear (not
													// including aircraft name)

	// TCP META DATA:
	private class TcpData {
		WayType    ty;              // type of point
		TrkTcpType tcpTrk;	        // Trk TCP type
		GsTcpType  tcpGs;		    // Gs TCP type
		VsTcpType  tcpVs;		    // Vs TCP type
		double     radiusSigned;	// signed turn radius, positive values indicate right turn, negative values indicate left turn, zero indicates no specified radius
		double     accelGs;         // signed gs-acceleration value
		double     accelVs;         // signed vs-acceleration value
		Velocity   velocityInit;	// the initial velocity at the start of an acceleration zone.  
		Position   sourcePosition;  // source position
		double     sourceTime;		// source time (if < 0, no valid source)
		int        linearIndex;	    // in a kinematic plan provides index to original linear plan
		String     information;
		
		TcpData(NavPoint p) {
			ty = WayType.Orig;
			tcpTrk = TrkTcpType.NONE;
			tcpGs = GsTcpType.NONE;
			tcpVs = VsTcpType.NONE;
			radiusSigned = 0.0;	
			accelGs = 0.0;       
			accelVs = 0.0;       
			velocityInit = Velocity.INVALID;	  
			sourcePosition = p.position(); 
			sourceTime = p.time();		
			linearIndex = -1;	
			information = "";			
		}
	}

	protected ArrayList<TcpData> data;

	/** Create an empty Plan */
	public Plan() {
		name = "";
		note = "";
		init();
	}

	/** Create an empty Plan with the given id */
	public Plan(String name) {
		this.name = name;
		note = "";
		init();
	}

	/** Create an empty Plan with the given id */
	public Plan(String name, String note) {
		this.name = name;
		this.note = note;
		init();
	}

	private void init() {
		points = new ArrayList<NavPoint>(100);
		data = new ArrayList<TcpData>(100);
		error = new ErrorLog("Plan");
		error.setConsoleOutput(debug); // debug ON!
		errorLocation = -1;
		// deletedPoints = new ArrayList<NavPoint>();
		bound = new BoundingRectangle();
	}

	/** Construct a new object that is a deep copy of the supplied object */
	public Plan(Plan fp) {
		points = new ArrayList<NavPoint>(100);
		data = new ArrayList<TcpData>(100);
		// deletedPoints = new ArrayList<NavPoint>(fp.deletedPoints);
		for (int j = 0; j < fp.points.size(); j++) {
			points.add(fp.points.get(j));
			data.add(fp.data.get(j));
		}
		name = fp.name;
		note = fp.note;
		error = new ErrorLog(fp.error);
		errorLocation = fp.errorLocation;
		bound = new BoundingRectangle(fp.bound);
	}

	/** search plan to see if there are any TCPs
	 * 
	 * @return  true if the plan does not contain any TCPs 
	 */
	public boolean isLinear() {
		for (int j = 0; j < points.size(); j++) {
			boolean isTcp = points.get(j).isTCP();
			// f.pln(j+" $$$$$$$$$$$$$$$ points.get(j).isTCP() = "+isTcp);
			if (isTcp)
				return false;
		}
		return true;
	}

	public Plan copy() {
		return new Plan(this);
	}

	public Plan copyWithIndex() {
		// deletedPoints = new ArrayList<NavPoint>(fp.deletedPoints);
		Plan lpc = new Plan(name, note);
		for (int j = 0; j < size(); j++) {
			lpc.add(point(j).makeLinearIndex(j));
		}
		return lpc;
	}

	/** merge together all points closer together in time than "minDt"
	 * @param minDt     the smallest delta time allowed between points.  See also wellFormed()
	 */
	public void mergeClosePoints(double minDt) {
		// f.pln(" $$$$$$ mergeClosePoints "+getName()+" minDt = "+minDt);
		for (int i = size() - 2; i >= 0; i--) {
			if (minDt > 0) {
				NavPoint npi = point(i);
				NavPoint npip1 = point(i + 1);
				double ti = npi.time();
				double tip1 = npip1.time();
				if (tip1 - ti < minDt) {
					int ixDelete = i + 1;
					if (i == size() - 1)
						ixDelete = i;
					else if (i == 0)
						ixDelete = 1;
					else if (!npi.isBeginTCP() && npip1.isBeginTCP())
						ixDelete = i;
					// save attributes of "ixDelete"
					NavPoint npDelete = point(ixDelete);
					remove(ixDelete);
					// the index of the remaining point is "i"
					NavPoint newNpi = point(i).mergeTCPInfo(npDelete);
					set(i, newNpi);
					// f.pln(" $$$$$ mergeClosePoints: DELETE point ixDelete =
					// "+ixDelete);
				}
			}
			// NavPoint npi = point(i);
			// set(i,npi.makeLinearIndex(i));
		}
	}

	/**
	 * Find the index in the kinematic plan that corresponds to the n-th
	 * "instance" of linear index "ix"
	 * 
	 * @param ix        linear index
	 * @param instance  n-th copy of the linear index (this parameter should always be at least 1)
	 * @return kinematic index
	 */
	public ArrayList<Integer> findLinearIndex(int ix) {
		ArrayList<Integer> al = new ArrayList<Integer>();
		for (int i = 0; i < size(); i++) {
			int linearIx = point(i).linearIndex();
			if (linearIx == ix)
				al.add(i);
		}
		return al;
	}

	public Plan clone() {
		return new Plan(this);
	}

	/**
	 * Create new plan from existing using points from index "firstIx" to index "lastIx"
	 * 
	 * @param firstIx  first index
	 * @param lastIx   last index
	 * @return
	 */
	public Plan cut(int firstIx, int lastIx) {
		Plan lpc = new Plan(getName(), getNote());
		for (int i = firstIx; i <= lastIx; i++) {
			NavPoint np = point(i);
			lpc.add(np);
		}
		return lpc;
	}

	/**
	 * append plan "pEnd" to this plan
	 * 
	 */
	public Plan append(Plan pEnd) {
		Plan rtn = copy();
		if (pEnd.getFirstTime() < rtn.getLastTime()) {
			rtn.addError(" ERROR: append:  pEnd does not occur after this plan");
			return rtn;
		}
		for (int j = 0; j < pEnd.size(); j++) {
			rtn.add(pEnd.point(j));
		}
		return rtn;
	}

	/**
	 * Create a (simple) new Plan by projecting state information.
	 * 
	 * @param id         Name of aircraft
	 * @param pos        Initial position of aircraft
	 * @param v          Initial velocity of aircraft (if pos in in lat/lon, then this assumes a great circle initial heading)
	 * @param startTime  Time of initial state
	 * @param endTime    Final time when projection ends
	 * @return new Plan, with a Fixed starting point. If endTime <= startTime, returns an empty Plan.
	 */
	static public Plan planFromState(String id, Position pos, Velocity v, double startTime, double endTime) {
		Plan p = new Plan(id);
		if (endTime <= startTime) {
			return p;
		}
		NavPoint np = new NavPoint(pos, startTime);
		p.add(np);
		p.add(np.linear(v, endTime - startTime));
		return p;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((bound == null) ? 0 : bound.hashCode());
		result = prime * result + ((error == null) ? 0 : error.hashCode());
		result = prime * result + errorLocation;
		result = prime * result + ((name == null) ? 0 : name.hashCode());
		result = prime * result + ((points == null) ? 0 : points.hashCode());
		result = prime * result + ((data == null) ? 0 : data.hashCode());
		return result;
	}

	/**
	 * Tests if one plan object has the same contents as another. This test also
	 * compares the concrete classes of the two Plans.
	 */
	public boolean equals(Plan fp) {
		// make sure that two Plans that are satisfy the "equals" method return
		// same hashcode (see hashCode above)
		if (!this.getClass().equals(fp.getClass()))
			return false;
		if (points.size() != fp.points.size()) {
			return false;
		}
		boolean r = true;
		for (int j = 0; j < points.size(); j++) {
			r = r && points.get(j).equals(fp.points.get(j)) && data.get(j).equals(fp.data.get(j));
		}
		return r && name.equalsIgnoreCase(fp.name); // && type == fp.type; // &&
													// numTCPs == fp.numTCPs;
	}

	/** determines if plans are almost equals (not exactly equal because of floating point calculations
	 * 
	 * @param p    plan of interest
	 * @return     true if almost equal in location and time
	 */
	public boolean almostEquals(Plan p) {
		boolean rtn = true;
		for (int i = 0; i < size(); i++) { // Unchanged
			if (!point(i).almostEqualsPosition(p.point(i))) {
				rtn = false;
				f.pln("almostEquals: point i = " + i + " does not match: " + point(i) + "  !=   " + p.point(i));
			}
			if (Math.abs(getTime(i) - p.getTime(i)) > 0.0000001) {
				rtn = false;
				f.pln("almostEquals: time i = " + i + " does not match: " + getTime(i) + "  !=   " + p.getTime(i));
			}
		}
		return rtn;
	}

	/**
	 * Check that positions and times are virtually identical
	 * 
	 * Note: it does not compare source info, or TCP attributes
	 * 
	 * @param p              Plan to compare with
	 * @param epsilon_horiz  allowable horizontal deviation [m]
	 * @param epsilon_vert   allowable vertical deviation [m]
	 * @return
	 */
	public boolean almostEquals(Plan p, double epsilon_horiz, double epsilon_vert) {
		boolean rtn = true;
		for (int i = 0; i < size(); i++) { // Unchanged
			if (!point(i).almostEqualsPosition(p.point(i), epsilon_horiz, epsilon_vert)) {
				rtn = false;
				f.pln("almostEquals: point i = " + i + " does not match: " + point(i) + "  !=   " + p.point(i)
						+ " epsilon_horiz = " + epsilon_horiz);
			}
			if (!point(i).label().equals(p.point(i).label())) {
				f.pln("almostEquals: point i = " + i + " labels do not match.");
				return false;
			}
		}
		return rtn;
	}

	/**
	 * Given an index i, returns the corresponding point's time in seconds. If
	 * the index is outside the range of points, then an error is set.
	 * @param i   the segment number
	 * @return    the time at point i

	 */
	public double getTime(int i) {
		if (i < 0 || i >= size()) {
			addError("getTime: invalid point index of " + i + " sz=" + size(), i);
			// f.pln(" $$!! invalid point index of "+i+" in getTime()");
			// Debug.halt();
			return 0.0;
		}
		return points.get(i).time();
	}

	/**
	 * Return the NavPoint at the given index (segment number)
	 * 
	 * @param i   the segment number
	 * @return    the NavPoint of this index
	 */
	public NavPoint point(int i) {
		if (i < 0 || i >= points.size()) {
			addError("Plan.point: invalid index " + i, i);
			// f.pln("$$$ Plan.point: invalid index "+i);
			// Debug.halt();
			if (isLatLon()) {
				return NavPoint.ZERO_LL;
			} else {
				return NavPoint.ZERO_XYZ;
			}
		}
		return points.get(i);
	}

	public void setDebug(boolean d) {
		debug = d;
		error.setConsoleOutput(debug);
	}

	public static void setStaticDebug(boolean d) {
		debug = d;
	}

	/** Clear a plan of its contents. */
	public void clear() {
		points.clear();
		data.clear();
		getMessage();
		bound.clear();
	}

	/**
	 * Clear all virtual points in a plan.
	 */
	public void clearVirtuals() {
		int i = 0;
		while (i < size()) {
			if (points.get(i).isVirtual()) {
				points.remove(i);
				data.remove(i);
			} else {
				i++;
			}
		}
	}

	/** If size() == 0 then this is a "null" plan. */
	public int size() {
		return points.size();
	}

	/** Get an approximation of the bounding rectangle around this plan */
	public BoundingRectangle getBound() {
		return bound;
	}

	/** Are points specified in Latitude and Longitude */
	public boolean isLatLon() {
		if (points.size() > 0) {
			return points.get(0).isLatLon();
		} else {
			return false;
		}
	}

	/**
	 * Is the geometry of this Plan the same as the given parameter. Note if
	 * this Plan has no points, then it always agrees with the parameter
	 */
	public boolean isLatLon(boolean latlon) {
		if (points.size() > 0) {
			return isLatLon() == latlon;
		} else {
			return true;
		}
	}

	/**
	 * Return the name of this plan (probably the aircraft name).
	 */
	public String getName() {
		return name;
	}

	/**
	 * Set the name of this plan (probably the aircraft name).
	 */
	public void setName(String s) {
		name = s;
	}

	/**
	 * Return the note of this plan (probably the aircraft name).
	 */
	public String getNote() {
		return note;
	}

	/**
	 * Set the note of this plan (probably the aircraft name).
	 */
	public void setNote(String s) {
		note = s;
	}

	/**
	 * Return the time of the first point in the plan.
	 */
	public double getFirstTime() {
		if (size() == 0) {
			addError("getFirstTime: Empty plan", 0);
			return 0.0;
		}
		return points.get(0).time();
	}

	/**
	 * Return the time of the last point in the plan.
	 */
	public double getLastTime() {
		if (size() == 0) {
			addError("getLastTime: Empty plan", 0);
			return 0.0;
		}
		return points.get(size() - 1).time();
	}

	public boolean timeInPlan(double t) {
		return getFirstTime() <= t && t <= getLastTime();
	}

	/**
	 * Return the last point in the plan.
	 */
	public NavPoint getLastPoint() {
		if (size() == 0) {
			addError("getLastTime: Empty plan", 0);
			return NavPoint.INVALID;
		}
		return points.get(size() - 1);
	}

	/**
	 * This returns the time for the first non-virtual point in the plan.
	 * Usually this is the same as getFirstTime(), and this should only be
	 * called if there is a known special handling of virtual points, as it is
	 * less efficient. If there are 0 non-virtual points in the plan, this logs
	 * an error and returns 0.0
	 */
	public double getFirstRealTime() {
		int i = 0;
		while (i < size()) {
			if (!points.get(i).isVirtual())
				return points.get(i).time();
			i++;
		}
		addError("getFirstRealTime: all points are virtual or plan empty", 0);
		return 0.0;
	}

	/**
	 * Returns the index of the next point which is <b>after</b> startWp. If the
	 * startWp is greater or equal to the last wp, then the index of the last
	 * point is returned. Note: if the plan is empty, this returns a -1
	 * <p>
	 *
	 */
	public int nextPtOrEnd(int startWp) {
		int rtn = points.size() - 1;
		if (startWp < points.size() - 1)
			rtn = startWp + 1;
		return rtn;
	}

	/**
	 * Return the index of the point that matches the provided time. If the
	 * result is negative, then the given time corresponds to no point. A
	 * negative result gives information about where the given time does enter
	 * the list. If the (negative) result is i, then the given time corresponds
	 * to a time between indexes -i-2 and -i-1. For example, if times are {0.0,
	 * 1.0, 2.0, 3.0, 4.0}:
	 * <ul>
	 * <li>getIndex(-3.0) == -1 -- before index 0
	 * <li>getIndex(1.0) == 1 -- at index 1
	 * <li>getIndex(1.5) == -3 -- between index 1 (-i-2) and 2 (-i-1)
	 * <li>getIndex(3.4) == -5 -- between index 3 (-i-2) and 4 (-i-1)
	 * <li>getIndex(16.0) == -6 -- after index 4 (-i-2)
	 * </ul>
	 * 
	 * @param tm   a time
	 * @return     the index of the time, or negative if not found.
	 */
	public int getIndex(double tm) {
		if (points.size() == 0) {
			return -1;
		}
		return indexSearch(tm, 0, points.size() - 1);
	}

	
	/**
	 * Return the first point that has a name containing the given string at or
	 * after time t, or -1 if there are no matches.
	 * 
	 * @param str   String of interest
	 * @param t     start search at this time
	 * @return      index of point containing string, or -1 if not found
	 */
	public int getIndexContaining(String str, double t) {
		for (int i = 0; i < points.size() - 1; i++) {
			NavPoint np = points.get(i);
			if (np.time() >= t && np.fullLabel().contains(str))
				return i;
		}
		return -1;
	}
	
	/**
	 * Return the first point that has a name containing the given string, or -1
	 * if there are no matches.
	 */
	public int getIndexContaining(String n) {
		return getIndexContaining(n, points.get(0).time());
	}

	/**
	 * Return the index of first point that has a label equal to the given
	 * string -1 if there are no matches.
	 * 
	 * @param startIx   start with this index
	 * @param label     String to match
	 * @param withWrap  if true, go through whole list
	 */
	public int findLabel(String label, int startIx, boolean withWrap) {
		if (startIx >= points.size()) {
			if (withWrap)
				startIx = 0;
			else
				return -1;
		}
		for (int i = startIx; i < points.size(); i++) {
			NavPoint np = points.get(i);
			if (np.label().equals(label))
				return i;
		}
		if (withWrap) {
			for (int i = 0; i < startIx; i++) {
				NavPoint np = points.get(i);
				if (np.label().equals(label))
					return i;
			}
		}
		return -1;
	}

	/**
	 * Return the index of first point that has a label equal to the given
	 * string, return -1 if there are no matches.
	 * 
	 * @param label    String to match
	 */

	public int findLabel(String label) {
		for (int i = 0; i < points.size(); i++) {
			NavPoint np = points.get(i);
			if (np.label().equals(label))
				return i;
		}
		return -1;
	}


	// This performs a binary search instead of a linear one, for improved
	// performance
	// It explicitly checks the the low (i1) and high (i2) bounds, as well as
	// the midpoint between them
	// then searches a subset between the midpoint and appropriate outer bound.
	// It should never check a given index more than once.
	// This assumes the initial call will have i1 = 0 and i2 = size()-1
	// Return values:
	// i >= 0 : t = point(i).time
	// i = -1 : t < point(0).time
	// i = -size()-1 : t > point(size()-1).time
	// -size()-1 < i < -1 : point(-i-1).time < t < point(-i-2).time
	// Other base cases:
	// i1 = i2: covered under case 2, 4, or 5 (no recursion)
	// i1+1 = i2: mid (if it gets there) will be either i1 or i2. and the
	// recursive call will fall into case 1
	private int indexSearch(double tm, int i1, int i2) {
		// if i1 > i2, then tm must have been between i2 and i1 on the previous
		// call
		if (i1 > i2)
			return -i2 - 2; // case 1
		double t1 = points.get(i1).time();
		double t2 = points.get(i2).time();
		// check the end points for a match
		if (tm == t1)
			return i1; // case 2
		if (tm == t2)
			return i2; // case 3
		// if tm is out of bounds, it must be either "right before" i1 or "right
		// after" i2
		if (tm < t1)
			return -i1 - 1; // case 4
		if (tm > t2)
			return -i2 - 2; // case 5
		// note: if i1==i2, then one of the above cases 2-5 must hold
		// if tm is between i1 and i2, retry, looking at either the low or high
		// half of values
		int mid = (i2 - i1) / 2 + i1;
		// check the midpoint for a match
		if (tm == points.get(mid).time())
			return mid; // case 6
		// skip over all points that have been explicitly checked before
		// if i1+1 == i2 then the new values will effectively swap i1 and i2,
		// falling into case 1, above
		// if i1+2 == i2 then the new values will be i1+1==i1+1, falling into
		// one of the cases 2-5, above
		if (tm < points.get(mid).time()) {
			return indexSearch(tm, i1 + 1, mid - 1); // case 7
		} else {
			return indexSearch(tm, mid + 1, i2 - 1); // case 8
		}
	}

	/**
	 * Return the segment number that contains 'time' in [s]. If the time is not
	 * contained in the flight plan then -1 is returned. For example, if the
	 * time for point 0 is 10.0 and the time for point 1 is 20.0, then
	 * getSegment(10.0) will produce 0, getSegment(15.0) will produce 0, and
	 * getSegment(20.0) will produce 1.
	 */
	public int getSegment(double time) {
		int i = getIndex(time);
		if (i == -1)
			return -1; // before plan
		if (i >= 0)
			return i; // hit point exactly
		if (-i - 2 == size() - 1)
			return -1; // after plan
		return -i - 2; // between point i and i+1
	}

	/**
	 * Return segment that is distance "d" from the beggining of plan
	 * 
	 * @param d    distance of interest
	 * @return     segment number
	 */
	public int getSegmentByDistance(double d) {
		if (d < 0)
			return -1;
		double tdist = 0;
		int i = 0;
		while (tdist <= d && i < size()) {
			tdist += pathDistance(i);
			i++;
		}
		if (tdist > d && i <= size())
			return i - 1; // on segment i-1
		if (Util.within_epsilon(d, tdist, 0.01) && i == size())
			return size() - 1;
		return -1; // not found
	}

	/** Find location that is path distance "d" from the start.  Return index of the segment that this location is in.
	 * 
	 * @param d    distance to traverse from start of plan
	 * @return     index of segment this distance would end up in.
	 */
	public int getIndexByDistance(double d) {
		if (d < 0)
			return -1;
		double tdist = 0;
		int i = 0;
		while (tdist <= d && i < size()) {
			if (tdist == d)
				return i;
			tdist += pathDistance(i);
			i++;
		}
		if (tdist > d && i <= size())
			return -i - 1; // on segment i-1
		if (Util.within_epsilon(d, tdist, 0.01) && i == size())
			return -size() - 1;
		return -1; // not found

	}

	/**
	 * Return the index of the point nearest to the provided time. in the event
	 * two points are equidistant from the time, the earlier one is returned. If
	 * the plan is empty, -1 will be returned.
	 */
	public int getNearestIndex(double tm) {
		int p = getIndex(tm);
		if (p < 0) {
			if (p <= -points.size() - 1) {
				p = points.size() - 1;
			} else if (p == -1) {
				p = 0;
			} else {
				double dt1 = Math.abs(tm - points.get(-p - 1).time());
				double dt2 = Math.abs(tm - points.get(-p - 2).time());
				if (dt2 < dt1)
					p = -p - 2;
				else
					p = -p - 1;
			}
		}
		return p;
	}

	//
	//
	// TCP MetaData Methods
	//
	//
	//

	public String getInfo(int i) {
		if (i < 0 || i >= size()) {
			addError("getInfo: invalid point index of " + i + " size=" + size());
			return "";
		}
		return data.get(i).information;
	}

	public void setInfo(int i, String info) {
		if (i < 0 || i >= size()) {
			addError("setInfo: invalid point index of " + i + " size=" + size());
		}
		TcpData d = data.get(i);
		d.information = info;
		data.set(i, d);
	}

	/** Is this point Virtual? */
	public boolean isVirtual(int i) {
		if (i < 0 || i >= size()) {
			addError("isVirtual: invalid point index of "+i+" size="+size());
			return false;
		}	
		return data.get(i).ty == WayType.Virtual;
	}
	
	public void setVirtual(int i) {
		if (i < 0 || i >= size()) {
			addError("setVirtual: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.ty = WayType.Virtual;
		data.set(i, d);
	}
	

	/** Returns true if the point at index i is an unmodified original point, 
	   false otherwise.  0 <= i < size() */
	public boolean isOriginal(int i) {
		if (i < 0 || i >= size()) {
			addError("isOriginal: invalid point index of "+i+" size="+size());
			return false;
		}	
		return data.get(i).ty == WayType.Orig;
	}

	public void setOriginal(int i) {
		if (i < 0 || i >= size()) {
			addError("setOriginal: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.ty = WayType.Orig;
		data.set(i, d);
	}
	
	/** AltPreserve is used by TrajGen.generateVsTCPs to create a vertical profile that is a function
	 *  of locations (rather than time which is very fluid).  The TrajGen method "markVsChanges" marks points
	 *  with a significant vertical speed change as "AltPreserve".
	 * 
	 * @param i    index of point to be tested
	 * @return
	 */
	public boolean isAltPreserve(int i) {
		if (i < 0 || i >= size()) {
			addError("isAltPreserve: invalid point index of "+i+" size="+size());
			return false;
		}	
		return data.get(i).ty == WayType.AltPreserve;
	}
	
	/** AltPreserve is used by TrajGen.generateVsTCPs to create a vertical profile that is a function
	 *  of locations (rather than time which is very fluid).  The TrajGen method "markVsChanges" marks points
	 *  with a significant vertical speed change as "AltPreserve".
	 * 
	 * @param i    index of point set as "AltPReserve"
	 * @return
	 */
	public void setAltPreserve(int i) {
		if (i < 0 || i >= size()) {
			addError("setAltPreserve: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.ty = WayType.AltPreserve;
		data.set(i, d);
	}
	
	/**
	 * This returns the radius of the current turn. If this is not a turn point,
	 * it returns the stores radius value. If the associated acceleration is
	 * 0.0, this returns a radius of zero.
	 */
	public double turnRadius(int i) {
		if (i < 0 || i >= size()) {
			addError("turnRadius: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		return Math.abs(data.get(i).radiusSigned);
	}

	/**
	 * This returns the radius of the current turn. If this is not a turn point,
	 * it returns the stores radius value. If the associated acceleration is
	 * 0.0, this returns a radius of zero.
	 */
	public double signedRadius(int i) {
		if (i < 0 || i >= size()) {
			addError("signedRadius: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		return data.get(i).radiusSigned;
	}

	/** Sets the radius of point i
	 *  
	 * @param i          index of point
	 * @param radius     value to be set
	 */
	public void setRadius(int i, double radius) {
		if (i < 0 || i >= size()) {
			addError("setRadius: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.radiusSigned = radius;
		data.set(i, d);
	}
	
	/**
	 * This method returns a center of turn position with the same altitude as
	 * the current point. If the current point is not a turn point, and has a
	 * zero stored radius, this returns an invalid position.
	 */
	public Position turnCenter(int i) {
		if (i < 0 || i >= size()) {
			addError("turnCenter: invalid point index of " + i + " size=" + size());
			return Position.INVALID;
		}
		double R = signedRadius(i);
		// if (isTrkTCP()) {
		// if (R > 0.0) {
		// return
		// p.linear(velocityIn.mkAddTrk(Util.sign(R)*Math.PI/2).Hat(),R).mkZ(p.z());
		// }
		// } else
		if (R != 0) {
			Velocity vHat = data.get(i).velocityInit.mkAddTrk(Util.sign(R) * Math.PI / 2).Hat2D();
			return point(i).position().linear(vHat, Math.abs(R)).mkZ(point(i).position().z());
		}
		return Position.INVALID;
	}

	/** Source time of point this was based on for any type except BVS. */
	// @Deprecated
	public double sourceTime(int i) {
		if (i < 0 || i >= size()) {
			addError("sourceTime: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		return data.get(i).sourceTime;
	}
	
	// @Deprecated
	public void setSourceTime(int i, double time) {
		if (i < 0 || i >= size()) {
			addError("setSourceTime: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.sourceTime = time;
		data.set(i, d);
	}
	


	/** Source time of point this was based on for any type except BVS. */
	// @Deprecated
	public Position sourcePosition(int i) {
		if (i < 0 || i >= size()) {
			addError("sourcePosition: invalid point index of " + i + " size=" + size());
			return Position.INVALID;
		}
		return data.get(i).sourcePosition;
	}
	
	// @Deprecated
	public void setSourcePosition(int i, Position pos) {
		if (i < 0 || i >= size()) {
			addError("setSourcePosition: invalid point index of "+i+" size="+size());
			return;
		}
		TcpData d = data.get(i);
		d.sourcePosition = pos;
		data.set(i, d);
	}

//	public NavPoint sourceNavPoint(int i) {
//		if (i < 0 || i >= size()) {
//			addError("sourceNavPoint: invalid point index of " + i + " size=" + size());
//			return NavPoint.INVALID;
//		}
//		return new NavPoint(data.get(i).sourcePosition, data.get(i).sourceTime);
//	}

//	public boolean hasSource(int i) {
//		if (i < 0 || i >= size()) {
//			addError("hasSource: invalid point index of " + i + " size=" + size());
//			return false;
//		}
//		return data.get(i).sourceTime >= 0;
//	}


	/** Often a kinematic plan is generated from a linear plan.  See TrajGen.makeKinematicPlan.  This function
	 *  maps the index of a point back to the linear plan from which it was generated.
	 * 
	 * @return   index of original linear plan that point "i" was derived from
	 */
	public int linearIndex(int i) {
		if (i < 0 || i >= size()) {
			addError("linearIndex: invalid point index of "+i+" size="+size());
			return 0;
		}	
		return data.get(i).linearIndex;
	}

	public void setLinearIndex(int i, int index) {
		if (i < 0 || i >= size()) {
			addError("setLinearIndex: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.linearIndex = index;
		data.set(i, d);
	}
	



	/**
	 * If this is a TCP, return the initial velocity at point "i". Otherwise return an
	 * invalid velocity. Not that this value is only well-defined for "begin" TCPs: (BGSC, BVSC, BOT)
	 * 
	 * Note:  If there is a speed discontinuity at point "i", velocityInit correspond to (TrkOut(i), gsOut(i), vsOut(i))
	 */
	public Velocity velocityInit(int i) {
		if (i < 0 || i >= size()) {
			addError("velocityInit: invalid point index of " + i + " size=" + size());
			return Velocity.INVALID;
		}
		return data.get(i).velocityInit;
	}

	/** Set the "velocityInit" field to "vel" fro point "i"
	 * 
	 * @param i
	 * @param vel
	 */
	public void setVelocityInit(int i, Velocity vel) {
		if (i < 0 || i >= size()) {
			addError("setVelocityInit: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.velocityInit = vel;
		data.set(i, d);
	}
	
	/**
	 * Return the (signed) turn rate (i.e., position acceleration in the "track"
	 * dimension) associated with this point. Turn rate is in rad/sec (positive
	 * is clockwise/right, negative is counterclockwise/left), otherwise return
	 * 0.0;
	 */
	public double trkAccel(int i) {
		if (i < 0 || i >= size()) {
			addError("trkAccel: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		// return accel_trk;
		double rtn = 0.0;
		if (Math.abs(data.get(i).radiusSigned) > 0) {
			rtn = data.get(i).velocityInit.gs() / data.get(i).radiusSigned;
		}
		// f.pln(" $$$ trkAccel: radiusSigned = "+Units.str("NM",radiusSigned)+"
		// rtn = "+Units.str("deg/s",rtn));
		return rtn;
	}

	/**
	 * Return the associated (signed) ground speed rate of change (i.e, horizontal acceleration).
	 */
	public double gsAccel(int i) {
		if (i < 0 || i >= size()) {
			addError("gsAccel: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		return data.get(i).accelGs;
	}
	
	public void setGsAccel(int i, double accel) {
		if (i < 0 || i >= size()) {
			addError("setGsAccel: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.accelGs = accel;
		data.set(i, d);
	}


	/**
	 * Return the associated (signed) vertical acceleration.
	 */
	public double vsAccel(int i) {
		if (i < 0 || i >= size()) {
			addError("vsAccel: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		return data.get(i).accelVs;
	}
	
	public void setVsAccel(int i, double accel) {
		if (i < 0 || i >= size()) {
			addError("setVsAccel: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.accelVs = accel;
		data.set(i, d);
	}

	public void setBOT(int i) {
		if (i < 0 || i >= size()) {
			addError("setBOT: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpTrk = TrkTcpType.BOT;
		data.set(i, d);
	}

	public void setEOT(int i) {
		if (i < 0 || i >= size()) {
			addError("setEOT: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpTrk = TrkTcpType.EOT;
		data.set(i, d);
	}

	public void setEOTBOT(int i) {
		if (i < 0 || i >= size()) {
			addError("setEOTBOT: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpTrk = TrkTcpType.EOTBOT;
		data.set(i, d);
	}

	public void setBGS(int i) {
		if (i < 0 || i >= size()) {
			addError("setBGS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpGs = GsTcpType.BGS;
		data.set(i, d);
	}

	public void setEGS(int i) {
		if (i < 0 || i >= size()) {
			addError("setEGS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpGs = GsTcpType.EGS;
		data.set(i, d);
	}

	public void setEGSBGS(int i) {
		if (i < 0 || i >= size()) {
			addError("setEGSBGS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpGs = GsTcpType.EGSBGS;
		data.set(i, d);
	}

	public void setBVS(int i) {
		if (i < 0 || i >= size()) {
			addError("setBVS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpVs = VsTcpType.BVS;
		data.set(i, d);
	}

	public void setEVS(int i) {
		if (i < 0 || i >= size()) {
			addError("setEVS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpVs = VsTcpType.EVS;
		data.set(i, d);
	}

	public void setEVSBVS(int i) {
		if (i < 0 || i >= size()) {
			addError("setEVSBVS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.tcpVs = VsTcpType.EVSBVS;
		data.set(i, d);
	}

	/** true if this point is a TCP */
	public boolean isTCP(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpTrk != TrkTcpType.NONE || d.tcpGs != GsTcpType.NONE || d.tcpVs != VsTcpType.NONE;
	}

	/** true if this point is part of a turn */
	public boolean isTrkTCP(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpTrk != TrkTcpType.NONE;
	}

	/** true if this point is the start of a turn */
	public boolean isBOT(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpTrk == TrkTcpType.BOT || d.tcpTrk == TrkTcpType.EOTBOT;
	}

	/** true if this point is the end of a turn */
	public boolean isEOT(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpTrk == TrkTcpType.EOT || d.tcpTrk == TrkTcpType.EOTBOT;
	}

	public boolean isGsTCP(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpGs != GsTcpType.NONE;
	}

	public boolean isBGS(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpGs == GsTcpType.BGS || d.tcpGs == GsTcpType.EGSBGS;
	}

	public boolean isEGS(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpGs == GsTcpType.EGS || d.tcpGs == GsTcpType.EGSBGS;
	}

	/** true if this point is part of a vs change */
	public boolean isVsTCP(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpVs != VsTcpType.NONE;
	}

	/** true if this point is the start of a vs change */
	public boolean isBVS(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpVs == VsTcpType.BVS || d.tcpVs == VsTcpType.EVSBVS;
	}

	/** true if this point is the end of a vs change */
	public boolean isEVS(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpVs == VsTcpType.EVS || d.tcpVs == VsTcpType.EVSBVS;
	}

	public boolean isBeginTCP(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpTrk == TrkTcpType.BOT || d.tcpGs == GsTcpType.BGS || d.tcpVs == VsTcpType.BVS ||
				d.tcpTrk == TrkTcpType.EOTBOT || d.tcpGs == GsTcpType.EGSBGS || d.tcpVs == VsTcpType.EVSBVS;
	}

	public boolean isEndTCP(int i) {
		if (i < 0 || i >= size()) {
			return false;
		}
		TcpData d = data.get(i);
		return d.tcpTrk == TrkTcpType.EOT || d.tcpGs == GsTcpType.EGS || d.tcpVs == VsTcpType.EVS ||
				d.tcpTrk == TrkTcpType.EOTBOT || d.tcpGs == GsTcpType.EGSBGS || d.tcpVs == VsTcpType.EVSBVS;
	}

	/**
	 * This returns the index of the Ground Speed Change End point greater than the given index, or -1 if there is no such point. 
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	public int nextEGS(int current) { 
		if (current < 0 || current > size() - 1) {
			addWarning("nextEGS invalid starting index " + current);
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			if (points.get(j).isEGS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Ground Speed Change Begin point greater than the given index, or -1 if there is no such point. 
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	public int nextBGS(int current) { // fixed
		if (current < 0 || current > size() - 1) {
			addWarning("nextBGS invalid starting index " + current);
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			if (points.get(j).isBGS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the ground speed end point <= the given index,
	 * or -1 if there is no such point. This is generally intended to be used to
	 * find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of EGS
	 */
	public int prevEGS(int current) {// fixed
		// return
		// prevVsTCP(current,NavPoint.Vs_TCPType.BVS,NavPoint.Vs_TCPType.EVSBVS);
		if (current < 0 || current > size()) {
			addWarning("prevEGS invalid starting index " + current);
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			// if (points.get(j).tcp_vs==NavPoint.Vs_TCPType.BVS ||
			// points.get(j).tcp_vs==NavPoint.Vs_TCPType.EVSBVS) {
			if (points.get(j).isEGS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Vertical Speed Change End point that is
	 * greater than the given index, or -1 if there is no such point. This is
	 * generally intended to be used to find the end of an acceleration zone.
	 */
	public int nextEVS(int current) { // fixed
		if (current < 0 || current > size() - 1) {
			addWarning("nextVsTCP invalid starting index " + current + " with TCP types EVS, EVSBVS");
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			if (points.get(j).isEVS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the vertical speed end point <= the given
	 * index, or -1 if there is no such point. This is generally intended to be
	 * used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of EVS
	 */
	public int prevEVS(int current) {// fixed
		// return
		// prevVsTCP(current,NavPoint.Vs_TCPType.BVS,NavPoint.Vs_TCPType.EVSBVS);
		if (current < 0 || current > size()) {
			addWarning("prevEVS invalid starting index " + current);
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			// if (points.get(j).tcp_vs==NavPoint.Vs_TCPType.BVS ||
			// points.get(j).tcp_vs==NavPoint.Vs_TCPType.EVSBVS) {
			if (points.get(j).isEVS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the next Vertical Speed Change begin point that
	 * is greater than the given index, or -1 if there is no such point. This is
	 * generally intended to be used to find the end of an acceleration zone.
	 */
	public int nextBVS(int current) { // fixed
		if (current < 0 || current > size() - 1) {
			addWarning("nextBVS invalid starting index " + current + " with TCP types BVS, EVSBVS");
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			if (points.get(j).isBVS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Beginning of Turn (BOT) point that is less
	 * than the given index, or -1 if there is no such point. This is generally
	 * intended to be used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of BOT or EOT
	 */
	public int prevBOT(int current) {// fixed
		if (current < 0 || current > size()) {
			addWarning("prevBOT invalid starting index " + current);
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			// if (points.get(j).tcp_trk==Trk_TCPType.BOT ||
			// points.get(j).tcp_trk==Trk_TCPType.EOTBOT ) {
			if (points.get(j).isBOT()) {
				return j;
			}
		}
		return -1;
	}

	/** This returns the index of the next Beginning of Turn (BOT) point that occurs after "current"
	 * 
	 * @param current  start search after this index  
	 * @return         index of next BOT, or -1 if there is no next BOT
	 */
	public int nextBOT(int current) {
		if (current < 0 || current > size() - 1) {
			addWarning("nextBOT invalid starting index " + current);
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			// if (points.get(j).tcp_trk==NavPoint.Trk_TCPType.EOT ||
			// points.get(j).tcp_trk==NavPoint.Trk_TCPType.EOTBOT) {
			if (points.get(j).isBOT()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the End of Turn point > the given index, or -1
	 * if there is no such point. This is generally intended to be used to find
	 * the end of an acceleration zone.
	 */
	public int nextEOT(int current) {
		if (current < 0 || current > size() - 1) {
			addWarning("nextTrkTCP invalid starting index " + current + " with TCP types EOT, EOTBOT");
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			// if (points.get(j).tcp_trk==NavPoint.Trk_TCPType.EOT ||
			// points.get(j).tcp_trk==NavPoint.Trk_TCPType.EOTBOT) {
			if (points.get(j).isEOT()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the turn end point < the given index, or -1 if
	 * there is no such point. This is generally intended to be used to find the
	 * beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of EOT
	 */
	public int prevEOT(int current) {
		if (current < 0 || current > size()) {
			addWarning("prevEOT invalid starting index " + current);
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			// if (points.get(j).tcp_vs==NavPoint.Vs_TCPType.BVS ||
			// points.get(j).tcp_vs==NavPoint.Vs_TCPType.EVSBVS) {
			if (points.get(j).isEOT()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Vertical Speed Change Begin point < the
	 * given index, or -1 if there is no such point. This is generally intended
	 * to be used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of BVS
	 */
	public int prevBVS(int current) {// fixed
		// return
		// prevVsTCP(current,NavPoint.Vs_TCPType.BVS,NavPoint.Vs_TCPType.EVSBVS);
		if (current < 0 || current > size()) {
			addWarning("prevVsTCP invalid starting index " + current + " with TCP types BVS, EVSBVS");
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			// if (points.get(j).tcp_vs==NavPoint.Vs_TCPType.BVS ||
			// points.get(j).tcp_vs==NavPoint.Vs_TCPType.EVSBVS) {
			if (points.get(j).isBVS()) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Ground Speed Change Begin point < the given
	 * index, or -1 if there is no such point. This is generally intended to be
	 * used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of BGS
	 */
	public int prevBGS(int current) {// fixed
		if (current < 0 || current > size()) {
			addWarning("prevBGS invalid starting index " + current + " with TCP types BGS, EGSBGS");
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			if (points.get(j).isBGS()) {
				// f.pln(" $$>> prevBGS: return j = "+j);
				return j;
			}
		}
		return -1;
	}

	/**
	 * This finds the last previous index where the TCP is of type tcp1 or tcp2
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type
	 */
	public int prevTCP(int current) {// fixed
		if (current < 0 || current > size()) {
			addWarning("prevTCP invalid starting index " + current);
			return -1;
		}
		for (int j = current - 1; j >= 0; j--) {
			// f.pln(" $$$ prevTCP: points.get("+j+") = "+points.get(j).toStringFull());
			if (points.get(j).isTrkTCP() || points.get(j).isGsTCP() || points.get(j).isVsTCP()) {
				return j;
			}
		}
		return -1;
	}

	//
	// Point addition and set methods
	//

	/**
	 * Adds a point to the plan. If the new point has negative time or the same
	 * time as an existing non-Virtual point, it will generate an error.
	 * <p>
	 * 
	 * @param p  the point to add
	 * @return  the (non-negative) index of the point if the point is added without problems, otherwise it will return a negative value.
	 */
	public int add(NavPoint p) {
		return add(p, new TcpData(NavPoint.INVALID)); // TODO do something with the tcp metadata
	}
	
	public int addFull(NavPoint p, double radiusSigned, double accel_gs, double accel_vs, 
			Velocity velocityInit, NavPoint source, int linearIndex,	
			String information) {
		
		TcpData d = new TcpData(source);
		//d.ty = WayType  ty;             
		//d.tcp_trk = Trk_TCPType tcp_trk;	 
		//d.tcp_gs = Gs_TCPType  tcp_gs;		 
		//d.tcp_vs = Vs_TCPType  tcp_vs;		
		d.radiusSigned = radiusSigned;
		d.accelGs = accel_gs;
		d.accelVs = accel_vs;
		d.velocityInit = velocityInit;
		d.linearIndex = linearIndex;	
		d.information = information;
		
		return add(p,d);
	}
	

	private int add(NavPoint p, TcpData d) {
		if (p == null) {
			addError("add: Attempt to add null NavPoint", 0);
			return -1;
		}
		if (p.isInvalid()) {
			addError("add: Attempt to add invalid NavPoint", 0);
			f.pln(" Plan.add(" + name + "): error invalid NavPoint p = " + p.toStringFull());
			Debug.halt();
			return -1;
		}
		if (Double.isInfinite(p.time())) {
			addError("add: Attempt to add NavPoint with infinite time", 0);
			f.pln(" Plan.add(" + name + "): error infinite NavPoint p = " + p.toStringFull());
			Debug.halt();
			return -1;
		}
		if (p.time() < 0) {
			addError("add: Attempt to add a point at negative time, time = " + f.Fm4(p.time()), 0);
			f.pln(" $$$$$ plan.add(" + name + "): Attempt to add a point at negative time, time = " + f.Fm4(p.time()));
			Debug.halt();
			return -points.size() - 1;
		}
		if (!isLatLon(p.isLatLon())) {
			addError("add: Attempted to add NavPoint with wrong geometry", 0);
			return -1;
		}
		int i = getIndex(p.time());
		if (i >= 0) { // exact match in time
			//f.pln(" $$$ Plan.add: point(i).mergeable(p) = "+point(i).mergeable(p));
			if (!point(i).isVirtual()) {
				if (point(i).mergeable(p)) {
					NavPoint np = point(i).mergeTCPInfo(p);
					//f.pln(" $$$ Plan.add: np = "+np);
					points.set(i, np);
					data.set(i, d);
				} else {
					addWarning("Attempt to merge a point at time " + f.Fm4(p.time())
							+ " that already has an incompatible point, no point added.");
					// f.pln(" $$$$$ plan.add Attempt to add a point at time
					// "+f.Fm4(p.time())+" that already has an incompatible
					// point. ");
					// Debug.halt();
					return -points.size() - 1;
				}
			} else { // just replace the virtual
				// f.pln(" $$ NavPoint.add: set at i = "+i+" p = "+p);
				points.set(i, p);
				data.set(i, d);
			}
		} else {
			// insert
			i = -i - 1; // where the point should be inserted
			insert(i, p, d);
		}
		bound.add(p.position());

		return i;
	}

	/**
	 * Adds a point at index i. This may not preserve time ordering! This may
	 * result in inconsistencies between implied and stored ground and vertical
	 * speeds for TCP points.
	 * 
	 * @param i
	 * @param v
	 */
	private void insert(int i, NavPoint v, TcpData d) {
		if (i < 0 || i > points.size()) {
			addError("insert: Invalid index " + i, i);
			return;
		}
		points.add(i, v);
		data.add(i, d);
	}

	// insert a new point in plan at time t. Calculate all TCP data correctly
	public int insert(double t) {
		if (t < getFirstTime() || t > getLastTime()) {
			f.pln(" ERROR t is not within plan!!!!!!");
			return -1;
		}
		Position mp = position(t);
		NavPoint np = new NavPoint(mp, t, "insertedPt");
		// f.pln(" $$ Plan.insert: add np = "+np.toStringFull());
		int rtn = add(np);
		return rtn;
	}

	/**
	 * Remove the i-th point in this plan. (note: This does not store the fact a
	 * point was deleted.)
	 */
	public void remove(int i) {
		if (i < 0 || i >= points.size()) {
			addWarning("remove1: invalid index " + i);
			return;
		}
		points.remove(i);
		data.remove(i);
	}

	/**
	 * Remove a range of points i to j, inclusive, from a plan. (note: This does
	 * not store the fact a point was deleted.)
	 */
	public void remove(int i, int j) {
		if (i < 0 || j < i || j >= points.size()) {
			addWarning("remove2: invalid index(s) " + i + " " + j);
			return;
		}
		for (int k = j; k >= i; k--) {
			remove(k);
		}
	}

	/**
	 * Remove the i-th point in this plan. (note: This does not store the fact a
	 * point was deleted.)
	 */
	public void removeLast() {
		points.remove(size() - 1);
		data.remove(size() - 1);
	}

	/**
	 * Remove the first point with label "name" that occurs after index
	 * "startIx".) if there is no match, do not alter plan
	 */
	public void remove(String name, int startIx) {
		boolean withWrap = false;
		int ix = findLabel(name, startIx, withWrap);
		if (ix >= 0) {
			remove(ix);
		}
	}

	/**
	 * Attempt to replace the i-th point with the given NavPoint. If successful,
	 * this removes the point currently at index i and adds the new point. This
	 * returns an error if the given index is out of bounds or a warning if the
	 * new point overlaps with (and replaces) a different existing point.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new point to replace it with
	 * @return the actual index of the new point
	 */
	public int set(int i, NavPoint v) {
		if (i < 0 || i >= points.size()) {
			addError("set: invalid index " + i, i);
			return -1;
		}
		// f.pln(" $$ Plan.set: i = "+i+" v = "+v+" toString = "+toString());
		remove(i);
		return add(v);
	}

	// note: if you change the time of a point its index may change
	public void setTime(int i, double t) {
		if (t < 0) {
			addError("setTime: invalid time " + f.Fm4(t), i);
			return;
		}
		NavPoint tempv = points.get(i).makeTime(t);
		set(i, tempv);
	}

	/** Set altitude of point "i" to "alt"
	 * 
	 */
	public void setAlt(int i, double alt) {
		NavPoint tempv = points.get(i).mkAlt(alt);
		set(i, tempv);
	}

    // currently only used in "TurnPlanTest"
	public void updateWithDefaultRadius(double radius) {
		for (int i = 0; i < points.size(); i++) {
			NavPoint np = points.get(i);
			if ((np.isBOT() || !np.isTCP()) && np.turnRadius() == 0.0) {
				np = np.makeRadius(radius);
				points.set(i, np);
			}
		}
	}

	/**
	 * Creates a copy with all points in the plan (starting at index start) time
	 * shifted by the provided amount "st". This will drop any points that become
	 * negative or that become out of order. Note this also shifts the start
	 * point. This version will also shift the "source times" associated with
	 * the points
	 */
	public Plan copyAndTimeShift(int start, double st) {
		Plan newKPC = new Plan(this.name);
		if (start >= this.size() || st == 0.0)
			return this.copy();
		for (int j = 0; j < start; j++) {
			newKPC.add(this.point(j));
			// f.pln("0 add newKPC.point("+j+") = "+newKPC.point(j));

		}
		double ft = 0.0; // time of point before start
		if (start > 0) {
			ft = this.getTime(start - 1);
		}
		for (int i = start; i < this.size(); i++) {
			double t = this.getTime(i) + st; // adjusted time for this point
			// f.pln(">>>> timeshiftPlan: t = "+ t+" ft = "+ft);
			if (t > ft && t >= 0.0) {
				// double newSourceTime = this.point(i).sourceTime() + st;
				newKPC.add(this.point(i).makeTime(t));
				// f.pln(" add newKPC.point("+i+") = "+newKPC.point(i));
			} else {
				// f.pln(">>>> copyTimeShift: do not add i = "+i);
			}
		}
		return newKPC;
	}

	/**
	 * Shift all points in the plan (starting at index start) by time.  The shift amount is dt,
	 * which can be postive or negative.  This will drop any points that become negative or
	 * that become out of order. Note this also shifts the start point. 
	 * 
	 * @param start_ix   starting index to begin shifting
	 * @param dt         amount to shift time (can be positive or negative
	 * @return           true, if successful
	 */
	public boolean timeshiftPlan(int start_ix, double dt) {
		if (Double.isNaN(dt) || start_ix < 0) {
			if (debug) Debug.halt();
			return false;
		}
		if (start_ix >= size() || dt == 0.0)
			return true;
		if (dt < 0) {
			double ft = 0.0; // time of point before start
			if (start_ix > 0) {
				ft = getTime(start_ix - 1);
			}
			for (int i = start_ix; i < size(); i++) {
				double t = getTime(i) + dt; // adjusted time for this point
				// f.pln(">>>> timeshiftPlan: t = "+t+" getTime(i) =
				// "+getTime(i)+" getTime(i-1) = "+getTime(i-1));
				if (t > ft && t >= 0.0) {
					// f.pln(">>>> timeshiftPlan: setTime(i,t)! i = "+i+" t =
					// "+t);
					setTime(i, t);
				} else {
					// f.pln(">>>> timeshiftPlan: remove i = "+i);
					remove(i);
					i--;
				}
			}
		} else {
			for (int i = size() - 1; i >= start_ix; i--) {
				// f.pln(">>>> timeshiftPlan: setTime(i,getTime(i)+st! i = "+i+"
				// st = "+st);
				setTime(i, getTime(i) + dt);
			}
		}
		return true;
	}

	/**
	 * This returns true if the given time is >= a BOT but before an EOT point
	 */
	public boolean inTrkChange(double t) { // fixed
		int i = getSegment(t);
		int j1 = prevBOT(i + 1);
		int j2 = prevEOT(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	/**
	 * This returns true if the given time is >= a GSCBegin but before a GSCEnd
	 * point
	 */
	public boolean inGsChange(double t) {// fixed
		int i = getSegment(t);
		int j1 = prevBGS(i + 1);
		int j2 = prevEGS(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	/**
	 * This returns true if the given time is >= a VSCBegin but before a VSCEnd
	 * point
	 */
	public boolean inVsChange(double t) {
		int i = getSegment(t);
		int j1 = prevBVS(i + 1);
		int j2 = prevEVS(i + 1);
		// f.pln(toString());
		// f.pln("i="+i+" j1="+j1+" j2="+j2);
		return j1 >= 0 && j1 >= j2;
	}

	public boolean inTrkChangeByDistance(double d) {
		int i = getSegmentByDistance(d);
		int j1 = prevBOT(i + 1);
		int j2 = prevEOT(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	public boolean inGsChangeByDistance(double d) {
		int i = getSegmentByDistance(d);
		int j1 = prevBGS(i + 1);
		int j2 = prevEGS(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	public boolean inVsChangeByDistance(double d) {
		int i = getSegmentByDistance(d);
		int j1 = prevBVS(i + 1);
		int j2 = prevEVS(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	/**
	 * Is the aircraft accelerating (either horizontally or vertically) at this
	 * time?
	 * 
	 * @param t  time to check for acceleration
	 * @return   true if accelerating
	 */
	public boolean inAccel(double t) {
		return inTrkChange(t) || inGsChange(t) || inVsChange(t);
	}

	/**
	 * true if this point is in a closed interval [BEGIN_TCP , END_TCP]
	 * 
	 * NOTE: inAccel(t) returns false if the point at time "t" is an end TCP.
	 * This method return true!
	 * 
	 */
	public boolean inAccelZone(int ix) {
		return point(ix).isTCP() || inAccel(getTime(ix));
	}

	/**
	 * If time t is in a turn, this returns the radius, otherwise returns a
	 * negative value.
	 */
	public double turnRadiusAtTime(double t) {
		if (inTrkChange(t)) {
			NavPoint bot = points.get(prevBOT(getSegment(t) + 1));
			// return bot.position().distanceH(bot.turnCenter());
			return bot.turnRadius();
		} else {
			return 0.0;
		}
	}

	/**
	 * Return the turn rate (i.e., position acceleration in the "track"
	 * dimension) associated with the point at time t.
	 */
	public double trkAccelAtTime(double t) {
		if (inTrkChange(t)) {
			int b = prevBOT(getSegment(t) + 1);
			return point(b).trkAccel();
		} else {
			return 0.0;
		}
	}

	/**
	 * Return the ground speed rate of change (i.e., position acceleration in
	 * the "ground speed" dimension) associated with the point at time t.
	 */
	public double gsAccelAtTime(double t) {
		if (inGsChange(t)) {
			int b = prevBGS(getSegment(t) + 1);// fixed
			return point(b).gsAccel();
		} else {
			return 0.0;
		}
	}

	/**
	 * Return the vertical speed rate of change (i.e., acceleration in the
	 * vertical dimension) associated with the point at time t.
	 */
	public double vsAccelAtTime(double t) {
		if (inVsChange(t)) {
			int b = prevBVS(getSegment(t) + 1);
			return point(b).vsAccel();
		} else {
			return 0.0;
		}
	}

	//
	public Position position(double t, boolean linear) {
		return positionVelocity(t, linear).first;
	}

	/**
	 * Return a linear interpolation of the position at the given time. If the
	 * time is beyond the end of the plan and getExtend() is true, then the
	 * position is extrapolated after the end of the plan. An error is set if
	 * the time is before the beginning of the plan.
	 *
	 * @param tt
	 *            time
	 * @return linear interpolated position at time tt
	 */
	public Position position(double t) {
		return positionVelocity(t, false).first;
	}

	/**
	 * Estimate the initial velocity at the given time for this aircraft. A time
	 * before the beginning of the plan returns a zero velocity.
	 */
	public Velocity velocity(double tm, boolean linear) {
		return positionVelocity(tm, linear).second;
	}

	/**
	 * Estimate the initial velocity at the given time for this aircraft. A time
	 * before the beginning of the plan returns a zero velocity.
	 */
	public Velocity velocity(double tm) {
		return positionVelocity(tm, false).second;
	}

	/**
	 * time required to cover distance "dist" if initial speed is "vo" and acceleration is "gsAccel"
	 *
	 * @param gsAccel  ground speed acceleration
	 * @param vo       initial velocity
	 * @param dist     distance
	 * @return time required to cover distance
	 */
	static double timeFromGs(double vo, double gsAccel, double dist) {
		double t1 = Util.root(0.5 * gsAccel, vo, -dist, 1);
		double t2 = Util.root(0.5 * gsAccel, vo, -dist, -1);
		double dt = Double.isNaN(t1) || t1 < 0 ? t2 : (Double.isNaN(t2) || t2 < 0 ? t1 : Math.min(t1, t2));
		return dt;
	}

	/** Return the time that corresponds to the point that is path distance "rdist" from the start of segment "seg"
	 * 
	 * @param seg    segment of interest
	 * @param rdist  distance from "seg"
	 * @return       time of point that is "rdist" distance from start of segment "seg"
	 * 
	 * Note: if there is a gs0=0 segment, return the time of the start of the segment return -1 on out of bounds input
	 */
	public double timeFromDistance(int seg, double rdist) {
		if (seg < 0 || seg > size() - 1 || rdist < 0 || rdist > pathDistance(seg))
			return -1;
		double gs0 = initialVelocity(seg).gs();
		if (Util.almost_equals(gs0, 0.0))
			return 0;
		if (inGsChange(getTime(seg))) {
			double a = point(prevBGS(seg + 1)).gsAccel();
			return timeFromGs(gs0, a, rdist);
		} else {
			return rdist / gs0;
		}
	}

	/** Return the time that corresponds to the point that is path distance "rdist" from the start of plan
	 * 
	 * @param rdist  distance from start of plan
	 * @return       time of point that is "rdist" distance from start of plan
	 * 
	 */
	public double timeFromDistance(double dist) {
		int seg = getSegmentByDistance(dist);
		if (seg < 0 || seg > size() - 1)
			return -1;
		double dd = dist - pathDistance(0, seg);
		return timeFromDistance(seg, dd) + getTime(seg);
	}

	/** Return the velocity of the point that is path distance "rdist" from the start of plan
	 * 
	 * @param rdist  distance from start of plan
	 * @return       velocity of point that is "rdist" distance from start of plan
	 * 
	 */
	public Velocity velocityByDistance(double d) {
		return velocity(timeFromDistance(d));
	}

	/**
	 * Calculate track angle out of point "i"
	 * 
	 * @param i        The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         track angle out of point "i"
	 */
	public double trkOut(int seg, boolean linear) {
		// f.pln(" $$$ trkOut: ENTER seg = "+seg+" linear = "+linear);
		if (seg < 0 || seg > size() - 1) {
			addError("trkOut: invalid index " + seg, 0);
			return -1;
		}
		if (seg == size() - 1) {
			return trkFinal(seg - 1, linear);
		}
		Velocity vNew;
		if (inTrkChange(point(seg).time()) && !linear) {
			int ixBOT = prevBOT(seg + 1);
			double distFrom = pathDistance(ixBOT, seg);
			Position center = point(ixBOT).turnCenter();
			double signedRadius = point(ixBOT).signedRadius();
			int dir = Util.sign(signedRadius);
		    //f.pln(" $$$ trkOut (inTurn): distFrom = "+Units.str("ft",distFrom)+" signedRadius = "+Units.str("ft",signedRadius)+" ixBOT = "+ixBOT);
			double gsAt_d = 1000.0; // not used here -- don't use 0.0 because
									// will lose track info
			Position so = point(ixBOT).position();
			vNew = KinematicsPosition.turnByDist(so, center, dir, distFrom, gsAt_d).second;
		   //f.pln(" $$$ trkOut: vNew = "+vNew);
			return vNew.trk();
		} else {
			double distH = point(seg).distanceH(point(seg+1));
			//f.pln(" $$$ NavPoint.initialVelocity: distH = "+Units.str("ft",distH,14));
			vNew = point(seg).initialVelocity(point(seg+1));
		    //f.pln(" $$$ trkOut (linear): seg = "+seg+" vNew = "+vNew);
			return vNew.trk();
		}
	}

	public double trkOut(int seg) {
		return trkOut(seg, false);
	}

	/**
	 * Calculate track angle at the end of segment "i"
	 * 
	 * @param i        The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         track angle at the end of segment "i"
	 */
	public double trkFinal(int seg, boolean linear) {
		// f.pln("$$$ trkFinal: ENTER: seg = "+seg+" linear = "+linear);
		if (seg < 0 || seg >= size() - 1) {
			addError("trkFinal: invalid index " + seg, 0);
			return -1;
		}
		if (inTrkChange(point(seg).time()) && !linear) {
			int ixBOT = prevBOT(seg + 1);
			double d = pathDistance(ixBOT, seg + 1);
			double signedRadius = point(ixBOT).signedRadius();
			int dir = Util.sign(signedRadius);
			Position center = point(ixBOT).turnCenter();
			// f.pln("$$$ trkFinal AA: d = "+Units.str("NM",d)+" signedRadius =
			// "+Units.str("NM",signedRadius)+" ixBOT = "+ixBOT);
			double gsAt_d = 1000.0; // not used here -- don't use 0.0 because
									// will lose track info
			Position so = point(ixBOT).position();
			Velocity vFinal = KinematicsPosition.turnByDist(so, center, dir, d, gsAt_d).second;
			// f.pln("$$$ trkFinal AA: ixBOT = "+ixBOT+" seg = "+seg+" vFinal =
			// "+vFinal);
			return vFinal.trk();
		} else {
			if (isLatLon()) {
				// double d = pathDistance(seg,seg+1);
				double trk = GreatCircle.final_course(point(seg).lla(), point(seg + 1).lla());
				// f.pln("$$$ trkFinal BB: seg = "+seg+" trk =
				// "+Units.str("deg",trk));
				return trk;
			} else {
				Velocity vo = point(seg).finalVelocity(point(seg + 1));
				// f.pln("$$$ trkFinal CC: seg = "+seg+" vo = "+vo);
				return vo.trk();
			}
		}
	}

	/**
	 * ground speed out of point "i"
	 * 
	 * @param i       The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return
	 */
	public double gsOut(int i, boolean linear) {
		if (i < 0 || i > size() - 1) {
			addError("gsOut: invalid index " + i, 0);
			return -1;
		}
		if (i == size() - 1)
			return gsFinal(i - 1);
		int j = i + 1;
		// while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // skip
		// next point(s) if very close
		// j++;
		// }
		double dist = pathDistance(i, j, linear);
		double dt = getTime(j) - getTime(i);
		double a = 0.0;
		if (inGsChange(getTime(j-1)) && !linear) { // use getTime(j-1) rather  than getTime(i) in case j-1 point is an EGS
			int ixBGS = prevBGS(i + 1);
			a = point(ixBGS).gsAccel();
		}
		double rtn = dist / dt - 0.5 * a * dt;
		// f.pln("gsOut seg="+seg+" a="+a+" dist="+dist+" dt="+dt+" gs="+rtn);
		// f.pln("$$>>>>>> gsOut: rtn = "+Units.str("kn",rtn,8)+" a = "+a+" i = "+i+" j = "+j+" dt = "+f.Fm4(dt)+" dist = "+Units.str("NM",dist)+"size = "+size());
		if (rtn <= 0) {
			// f.pln(" ### WARNING gsOut: has encountered an ill-structured plan resulting in a negative ground speed!!");
			rtn = 0.000001; // do not set to 0 because it will lose track info, this can occur if dist = 0.0 or if "a" is larger
							// than it should be
			// Debug.halt();
		}
		return rtn;
	}

	/**
	 * ground speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as gsOut
	 * 
	 * @param i        The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return
	 */
	public double gsFinal(int i, boolean linear) {
		if (i < 0 || i > size() - 1) {
			addError("gsFinal: invalid index " + i, 0);
			return -1;
		}
		int j = i + 1;
		// while (i > 0 && getTime(j) - getTime(i) < minDt) { // collapse next
		// point(s) if very close
		// i--;
		// }
		double dist = pathDistance(i, j, linear);
		double dt = getTime(j) - getTime(i);
		double a = 0.0;
		if (inGsChange(point(i).time()) && !linear) {
			int ixBGS = prevBGS(i + 1);
			a = point(ixBGS).gsAccel();
		}
		return dist / dt + 0.5 * a * dt;
	}

	/**
	 * ground speed into point "seg"
	 * 
	 * @param seg
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double gsIn(int seg, boolean linear) {
		return gsFinal(seg - 1, linear);
	}

	public double gsOut(int seg) {
		return gsOut(seg, false);
	}

	public double gsFinal(int seg) {
		return gsFinal(seg, false);
	}

	public double gsIn(int seg) {
		return gsIn(seg, false);
	}

	/**
	 * ground speed at time t (which must be in segment "seg")
	 * 
	 * @param seg
	 *            segment where time "t" is located
	 * @param gsAtSeg
	 *            ground speed out of segment "seg"
	 * @param t
	 *            time of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	private double gsAtTime(int seg, double gsAtSeg, double t, boolean linear) {
		// f.pln(" $$ gsAtTime: seg = "+seg+" gsAtSeg =
		// "+Units.str("kn",gsAtSeg,8));
		double gs;
		if (!linear && inGsChange(t)) {
			double dt = t - getTime(seg);
			int ixBGS = prevBGS(seg + 1);
			double gsAccel = point(ixBGS).gsAccel();
			gs = gsAtSeg + dt * gsAccel;
			// f.pln(" $$ gsAtTime A: gsAccel = "+gsAccel+" dt = "+f.Fm4(dt)+"
			// seg = "+seg+" gs = "+Units.str("kn",gs,8));
		} else {
			gs = gsAtSeg;
			// f.pln(" $$ gsAtTime B: seg = "+seg+" gs =
			// "+Units.str("kn",gs,8));
		}
		return gs;
	}

	/**
	 * ground speed at time t
	 * 
	 * @param t
	 *            time of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double gsAtTime(double t, boolean linear) {
		double gs;
		int seg = getSegment(t);
		if (seg < 0) {
			gs = -1;
		} else {
			double gsSeg = gsOut(seg, linear);
			// f.pln(" $$ gsAtTime: seg = "+seg+" gsAt =
			// "+Units.str("kn",gsAt,8));
			gs = gsAtTime(seg, gsSeg, t, linear);
		}
		return gs;
	}

	public double gsAtTime(double t) {
		boolean linear = false;
		return gsAtTime(t, linear);
	}

	/**
	 * vertical speed out of point "i"
	 * 
	 * @param i
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double vsOut(int i, boolean linear) {
		if (i < 0 || i > size() - 1) {
			addError("vsOut: invalid index " + i, 0);
			return -1;
		}
		if (i == size() - 1)
			return vsFinal(i - 1);
		int j = i + 1;
		// while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // skip
		// next point(s) if very close
		// j++;
		// }
		// f.pln(" $$>>>>>> vsOut: i = "+i+" j = "+j);
		double dist = point(j).alt() - point(i).alt();
		double dt = getTime(j) - getTime(i);
		double a = 0.0;
		if (inVsChange(getTime(j - 1)) && !linear) { // use getTime(j-1) rather
														// than getTime(i) in
														// case j-1 point is an
														// EGS
			int ixBVS = prevBVS(i + 1);
			a = point(ixBVS).vsAccel();
		}
		double rtn = dist / dt - 0.5 * a * dt;
		// f.pln(" $$>>>>>> vsOut: rtn = "+Units.str("fpm",rtn,8)+" a = "+a+" i
		// = "+i+" dt = "+f.Fm4(dt)+" dist = "+Units.str("ft",dist));
		return rtn;
	}

	/**
	 * vertical speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as vsOut
	 * 
	 * @param i
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double vsFinal(int i, boolean linear) {
		// f.pln("\n $$>>>>>> vsFinal: seg = "+seg+" linear = "+linear);
		if (i < 0 || i > size() - 1) {
			addError("vsFinal: invalid index " + i, 0);
			return -1;
		}
		// double dist = point(i+1).alt() - point(i).alt();
		// double dt = getTime(i+1) - getTime(i);
		int j = i + 1;
		// while (i > 0 && getTime(j) - getTime(i) < minDt) { // collapse next
		// point(s) if very close
		// i--;
		// }
		double dist = point(j).alt() - point(i).alt();
		double dt = getTime(j) - getTime(i);
		double a = 0.0;
		if (inVsChange(point(i).time()) && !linear) {
			int ixBvs = prevBVS(i + 1);
			a = point(ixBvs).vsAccel();
			// f.pln(" $$>>>>>> vsFinal: IN VS ACCEL: a = "+a);
		}
		// f.pln(" $$>>>>>> vsFinal: a = "+a+" seg = "+seg+" dt = "+f.Fm4(dt)+"
		// dist = "+Units.str("ft",dist));
		double rtn = dist / dt + 0.5 * a * dt;
		// f.pln(" $$>>>>>> vsFinal: rtn = "+Units.str("fpm",rtn));
		return rtn;
	}

	/**
	 * vertical speed into point "seg"
	 * 
	 * @param seg
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double vsIn(int seg, boolean linear) {
		return vsFinal(seg - 1, linear);
	}

	public double vsOut(int seg) {
		return vsOut(seg, false);
	}

	public double vsFinal(int seg) {
		return vsFinal(seg, false);
	}

	public double vsIn(int seg) {
		return vsIn(seg, false);
	}

	/**
	 * vertical speed at time t (which must be in segment "seg")
	 * 
	 * @param seg
	 *            segment where time "t" is located
	 * @param vsAtSeg
	 *            vertical speed out of segment "seg"
	 * @param t
	 *            time of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	private double vsAtTime(int seg, double vsAtSeg, double t, boolean linear) {
		// f.pln(" $$ vsAtTime: seg = "+seg+" vsAt = "+Units.str("kn",vsAt,8));
		double vs;
		if (!linear && inVsChange(t)) {
			double dt = t - getTime(seg);
			int ixBvs = prevBVS(seg + 1);
			double vsAccel = point(ixBvs).vsAccel();
			vs = vsAtSeg + dt * vsAccel;
			// f.pln(" $$ vsAtTime A: vsAccel = "+vsAccel+" dt = "+f.Fm4(dt)+"
			// seg = "+seg+" vs = "+Units.str("kn",vs,8));
		} else {
			vs = vsAtSeg;
			// f.pln(" $$ vsAtTime B: seg = "+seg+" vs =
			// "+Units.str("kn",vs,8));
		}
		return vs;
	}

	/**
	 * vertical speed at time t
	 * 
	 * @param t
	 *            time of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double vsAtTime(double t, boolean linear) {
		double vs;
		int seg = getSegment(t);
		if (seg < 0) {
			vs = -1;
		} else {
			double vsSeg = vsOut(seg, linear);
			// f.pln(" $$ vsAtTime: seg = "+seg+" vsSeg =
			// "+Units.str("fpm",vsSeg,8));
			vs = vsAtTime(seg, vsSeg, t, linear);
		}
		return vs;
	}

	/**
	 * Calculate the distance from the Navpoint at "seq" to plan position at
	 * time "t"
	 * 
	 * @param seg
	 *            starting position
	 * @param t
	 *            time of stopping position
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return
	 */
	public double distFromPointToTime(int seg, double t, boolean linear) {
		NavPoint np1 = point(seg);
		double distFromSo = 0;
		double gs0 = gsOut(seg, linear);
		double dt = t - np1.time();
		if (inGsChange(t) && !linear) {
			double gsAccel = point(prevBGS(seg + 1)).gsAccel(); 
			distFromSo = gs0 * dt + 0.5 * gsAccel * dt * dt;
			// f.pln(" $$$ positionVelocity(inGsChange A): dt = "+f.Fm2(dt)+"
			// vo.gs() = "+Units.str("kn",gs0)+" distFromSo =
			// "+Units.str("ft",distFromSo));
		} else {
			distFromSo = gs0 * dt;
			// f.pln(" $$$ positionVelocity(! inGsChange B): dt = "+f.Fm4(dt)+"
			// gs0 = "+Units.str("kn",gs0)+" distFromSo =
			// "+Units.str("ft",distFromSo));
		}
		return distFromSo;
	}

	/**
	 * Compute position and velocity at time t
	 * 
	 * Note that the calculation proceeds in steps. First, the 2D path is determined. This gives a final position and final 
	 * track. Then ground speed is computed. Finally vertical speed is computed.
	 * 
	 * @param t         time of interest
	 * @param linear    If true, then interpret plan in a linear manner
	 * @return          position and velocity at time t
	 */
	public Pair<Position, Velocity> positionVelocity(double t, boolean linear) {
		if (t < getFirstTime() || Double.isNaN(t)) {
			return new Pair<Position, Velocity>(Position.INVALID, Velocity.ZERO);
		}
		int seg = getSegment(t);
		// f.pln("\n $$$$$ positionVelocity: ENTER t = "+t+" seg = "+seg);
		Position sNew;
		Velocity vNew;
		NavPoint np1 = point(seg);
		if (seg + 1 > size() - 1) { // at Last Point
			Velocity v = finalVelocity(seg - 1);
			// f.pln("\n -----size = "+size()+" seg = "+seg+"\n $$$ accelZone:
			// np1 = "+np1+" v = "+v);
			return new Pair<Position, Velocity>(np1.position(), v);
		}
		NavPoint np2 = point(seg + 1);
		// f.pln("\n ----------- seg = "+seg+" $$$ positionVelocity: np1 =
		// "+np1+" np2 = "+np2);
		double gs0 = gsOut(seg, linear);
		double gsAt_d = gsAtTime(seg, gs0, t, linear);
		Position so = np1.position();
		// f.pln(t+" $$$ positionVelocity: seg = "+seg+" t = "+f.Fm2(t)+"
		// positionVelocity: so = "+so+" gs0 = "+Units.str("kn",gs0));

		if (inTrkChange(t) & !linear) {
			int ixPrevBOT = prevBOT(seg + 1);
			Position center = point(ixPrevBOT).turnCenter();
			double signedRadius = point(ixPrevBOT).signedRadius();
			int dir = Util.sign(signedRadius);
			double distFromSo;
			boolean method1 = true; // starting position could be either the
									// current segment or the previous BOT
			if (method1) {
				distFromSo = distFromPointToTime(seg, t, linear); // starting position is the current segment
			} else {
				so = point(ixPrevBOT).position(); // // starting position is the
													// previous BOT
				distFromSo = distFromPointToTime(ixPrevBOT, t, linear);
			}
			Pair<Position, Velocity> tAtd = KinematicsPosition.turnByDist(so, center, dir, distFromSo, gsAt_d);
			sNew = tAtd.first;
			vNew = tAtd.second;
			// f.pln(" $$ %%%% positionVelocity A: vNew("+f.Fm2(t)+") = "+vNew);
			// f.pln(" $$ %%%% positionVelocity A: sNew("+f.Fm2(t)+") = "+sNew);
		} else {
			Velocity vo = np1.initialVelocity(np2);
			double distFromSo = distFromPointToTime(seg, t, linear);
			// f.pln(" $$ %%%% positionVelocity B1: t = "+t+" seg = "+seg+"
			// distFromSo = "+Units.str("ft",distFromSo)+" np2 = "+np2);
			// f.pln(" $$ %%%% positionVelocity B0: t = "+t+" seg = "+seg+" vo =
			// "+vo);
			Pair<Position, Velocity> pv = so.linearDist(vo, distFromSo);
			// f.pln(" $$ %%%% positionVelocity B1:pv.second = "+pv.second+"
			// gsAt_d = "+gsAt_d);
			sNew = pv.first;
			vNew = pv.second.mkGs(gsAt_d);
			// f.pln(" $$ %%%% positionVelocity B2: seg = "+seg+"
			// sNew("+f.Fm2(t)+") = "+sNew);
			// f.pln(" $$ %%%% positionVelocity B3: vNew("+f.Fm2(t)+") =
			// "+vNew);
		}
		if (inVsChange(t) & !linear) {
			NavPoint n1 = points.get(prevBVS(seg + 1));
			Position soP = n1.position();
			// double voPvs = n1.velocityInit().vs();
			double voPvs = vsAtTime(n1.time(), linear);
			// Pair<Position,Velocity> pv = KinematicsPosition.vsAccel(soP, voP,
			// t-n1.time(), n1.vsAccel());
			Pair<Double, Double> pv = KinematicsPosition.vsAccelZonly(soP, voPvs, t - n1.time(), n1.vsAccel());
			sNew = sNew.mkAlt(pv.first);
			vNew = vNew.mkVs(pv.second); // merge Vertical VS with horizontal
											// components
			// f.pln(t+" $$$ positionVelocity(inVsChange) C: vNew = "+vNew);
		} else {
			if (seg < size() - 1) { // otherwise np2 is not a valid future point
				double dt = t - np1.time();
				double vZ = (np2.z() - np1.z()) / (np2.time() - np1.time());
				double sZ = np1.z() + vZ * dt;
				// f.pln(" $$$$$$$$ seg = "+seg+" dt = "+f.Fm2(dt)+" vZ =
				// "+Units.str("fpm",vZ)+" sZ = "+Units.str("ft",sZ));
				sNew = sNew.mkAlt(sZ);
				vNew = vNew.mkVs(vZ);
			}
		}
		// f.pln(" $$ %%%% positionVelocity RETURN: sNew("+f.Fm2(t)+") = "+sNew);
		// f.pln(" $$ %%%% positionVelocity RETURN: vNew("+f.Fm2(t)+") = "+vNew);
		// }
		return new Pair<Position, Velocity>(sNew, vNew);
	}

	public Pair<Position, Velocity> positionVelocity(double t) {
		return positionVelocity(t, false);
	}

	// public Velocity initialVelocityLastPointLinear(){
	// //int i = size()-1;
	// //Velocity rtn = getDtVelocity(i);
	// Position p = position(getLastTime()-10.0*minDt);
	// //f.pln(" $$$$$$$>>>>>>>>>>>>>>>>>>>>>>>. getDtVelocity SPECIAL CASE p =
	// "+p);
	// // Thread.dumpStack();
	// Velocity rtn = p.initialVelocity(points.get(size()-1).position(),
	// 10.0*minDt);
	// return rtn;
	// }

	// estimate the velocity from point i to point i+1 (at point i).
	public Velocity initialVelocity(int i) {
		return initialVelocity(i, false);
	}

	@Deprecated
	public Velocity initialVelocityOLD(int i, boolean linear) {
		// f.pln("\n $$$$ Plan.initialVelocity: ENTER with i = "+i);
		Velocity rtn = Velocity.ZERO;
		if (i > size() - 1) { // there is no velocity after the last point
			addWarning("initialVelocity(int): Attempt to get an initial velocity after the end of the Plan: " + i);
			// Debug.halt();
			return Velocity.ZERO;
		}
		if (i < 0) {
			addWarning("initialVelocity(int): Attempt to get an initial velocity before beginning of the Plan: " + i);
			return Velocity.ZERO;
		}
		if (size() == 1) {
			return Velocity.ZERO;
		}
		NavPoint np = points.get(i);
		double t = np.time();
		if (linear) {
			rtn = linearVelocityOut(i);
			// f.pln(f.Fm0(i)+" $$$$$ initialVelocity0: *******************
			// linear, rtn = "+rtn.toString());
		} else {
			// if (np.isBOT()) || np.isBGS() || np.isBVS()) { 
			if (np.isBOT()) { // || np.isBGS() || np.isBVS()) { 
				rtn = np.velocityInit();
				// f.pln(" $$ initialVelocity A: velocityIn for i = "+i+" rtn =
				// "+rtn);
				if (rtn.isInvalid()) {
					Debug.halt("\n !!!!!! Invalid velocityIn for " + name + " " + i + " this = " + toString());
				}
			} else if (inTrkChange(t) || inGsChange(t) || inVsChange(t) || (np.isTCP() && i == points.size() - 1)) { 
				rtn = velocity(t);
				// f.pln(f.Fm0(i)+" $$$ initialVelocity B: rtn =
				// velocity("+f.Fm2(t)+") = "+ rtn.toString());
			} else {
				rtn = linearVelocityOut(i);
				// f.pln(" $$$ initialVelocity C: i = "+f.Fm0(i)+" rtn =
				// getDtVelocity = "+ rtn.toString());
			}
		}
		// f.pln("---------------------------------------------\n $$$ RETURN
		// initialVelocity("+i+") rtn = "+rtn);

		// ********************** TEMPORARY TEST STUFF ***********************
		// double trkOut = trkOut(i,linear);
		// double deltaTrk = Util.turnDelta(rtn.trk(),trkOut);
		// if (deltaTrk > 0.00001) {
		// f.pln("\n initialVelocity: TEST: i = "+i+" rtn.trk() =
		// "+Units.str("deg",rtn.trk()));
		// f.pln(" initialVelocity: TEST: trkOut = "+Units.str("deg",trkOut));
		// f.pln(" initialVelocity: TEST: deltaTrk =
		// "+Units.str("deg",deltaTrk));
		// //DebugSupport.dumpPlan(this, "initialVelocityTrk");
		// //f.pln(" $$$ this = "+this.toStringTrk());
		// //Debug.halt();
		// }
		// double gsOut = gsOut(i,linear);
		// double deltaGs = Math.abs(rtn.gs()-gsOut);
		// if (deltaGs > 1.0) {
		// f.pln("\n initialVelocity: TEST: i = "+i+" rtn.gs() =
		// "+Units.str("kn",rtn.gs()));
		// f.pln(" initialVelocity: TEST: gsOut = "+Units.str("kn",gsOut));
		// f.pln(" initialVelocity: TEST: deltags = "+Units.str("kn",deltaGs));
		// //DebugSupport.dumpPlan(this, "initialVelocityGs");
		// //f.pln(" $$$ this = "+this.toStringGs());
		// //Debug.halt();
		// }
		// double vsOut = vsOut(i,linear);
		// double deltaVs = Math.abs(rtn.vs()-vsOut);
		// if (deltaVs > 1.0) {
		// f.pln("\n initialVelocity: TEST: i = "+i+" rtn.vs() =
		// "+Units.str("fpm",rtn.vs()));
		// f.pln(" initialVelocity: TEST: vsOut = "+Units.str("fpm",vsOut));
		// f.pln(" initialVelocity: TEST: deltavs = "+Units.str("fpm",deltaVs));
		// //DebugSupport.dumpPlan(this, "initialVelocityVs");
		// //f.pln(" $$$ this = "+this.toStringVs());
		// //Debug.halt();
		// }
		// ******************************************************************

		return rtn;
	}

	public Velocity initialVelocity(int i, boolean linear) {
		return Velocity.mkTrkGsVs(trkOut(i, linear), gsOut(i, linear), vsOut(i, linear));
	}

	/**
	 * This function computes the velocity out at point i in a strictly linear
	 * manner. If i is an inner point it uses the next point to construct the
	 * tangent. if the next point is less than dt ahead in time, then it finds
	 * the next point that is at least minDt ahead in time.
	 * 
	 * If it is called with the last point in the plan it looks backward for a
	 * point. In this case it uses final velocity on the previous leg.
	 * 
	 * @param i   index of point
	 * @return    initial velocity at point i
	 */
	@Deprecated
	protected Velocity linearVelocityOut(int i) {
		Velocity rtn;
		int j = i + 1;
		while (j < points.size() && points.get(j).time() - points.get(i).time() < minDt) { // collapse  next point(s) if very close
			j++;
		}
		if (j >= points.size()) { // LAST POINT: special case, back up a bit
			NavPoint lastPt = points.get(size() - 1);
			while (i > 0 && lastPt.time() - points.get(i).time() < minDt) {
				i--;
			}
			NavPoint npi = points.get(i);
			// Position npi = position(lastPt.time()-10.0*minDt);
			// double dt = 10.0*minDt;
			rtn = npi.finalVelocity(lastPt);
			// f.pln(" $$ linearVelocityOut: SPECIAL CASE rtn = "+rtn);
		} else {
			rtn = points.get(i).initialVelocity(points.get(j));
			// f.pln(" $$ linearVelocityOut: i = "+i+" j = "+j+" rtn = "+rtn);
		}
		return rtn;
	}

	// private Velocity finalLinearVelocity(int i) {
	// Velocity v;
	// int j = i+1; //goal point for velocity in
	// while(i > 0 && points.get(j).time()-points.get(i).time() < minDt) {
	// i--;
	// }
	// if (i == 0 && points.get(j).time()-points.get(i).time() < minDt) {
	// while (j < size()-1 && points.get(j).time()-points.get(i).time() < minDt)
	// {
	// j++;
	// }
	// v = points.get(i).initialVelocity(points.get(j));
	// }
	// NavPoint np = points.get(i);
	// if (isLatLon()) {
	// LatLonAlt p1 = np.lla();
	// LatLonAlt p2 = point(j).lla();
	// double dt = points.get(j).time()-np.time();
	// v = GreatCircle.velocity_final(p1,p2,dt);
	// //f.pln(" $$$ finalLinearVelocity: p1 = "+p1+" p2 = "+p2+" dt = "+dt+" v
	// = "+v);
	// } else {
	// v = np.initialVelocity(points.get(j));
	// }
	// //f.pln(" $$ finalLinearVelocity: dt =
	// "+(points.get(i+1).time()-points.get(i).time()));
	// return v;
	// }

	// This is not defined for the last point (as there is no next point)
	public Velocity dtFinalVelocity(int i) {
		return dtFinalVelocity(i, false);
	}

	// This is not defined for the last point (as there is no next point)
	public Velocity finalVelocity(int i) {
		return finalVelocity(i, false);
	}

	public Velocity finalVelocity(int i, boolean linear) {
		if (i >= size()) {
			addWarning("finalVelocity(int): Attempt to get a final velocity after the end of the Plan: " + i);
			// Debug.halt();
			return Velocity.INVALID;
		}
		if (i == size() - 1) {
			addWarning("finalVelocity(int): Attempt to get a final velocity at end of the Plan: " + i);
			// DebugSupport.halt();
			return Velocity.ZERO;
		}
		// Velocity dtVel = dtFinalVelocity(i);
		// f.pln(" $$$ finalVelocity: i = "+i+" dtVel = "+dtVel);
		Velocity rtn = Velocity.mkTrkGsVs(trkFinal(i, linear), gsFinal(i, linear), vsFinal(i, linear));
		// f.pln(" $$$ finalVelocity: i = "+i+" rtn = "+rtn);
		return rtn;
	}

	public Velocity dtFinalVelocity(int i, boolean linear) {
		// f.pln(" $$ finalVelocity "+i+" "+getName());
		if (i >= size()) { // there is no "final" velocity after the last point (in general. it happens to work for Euclidean, but not lla)
			addWarning("finalVelocity(int): Attempt to get a final velocity after the end of the Plan: " + i);
			// Debug.halt();
			return Velocity.INVALID;
		}
		if (i == size() - 1) {// || points.get(i).time() > getLastTime()-minDt)
								// {
			addWarning("finalVelocity(int): Attempt to get a final velocity at end of the Plan: " + i);
			// DebugSupport.halt();
			return Velocity.ZERO;
		}
		if (i < 0) {
			addWarning("finalVelocity(int): Attempt to get a final velocity before beginning of the Plan: " + i);
			// Debug.halt();
			return Velocity.ZERO;
		}
		// double t1 = points.get(i).time();
		double t2 = points.get(i + 1).time();
		return positionVelocity(t2 - 2.0 * minDt).second;
	}

	/**
	 * Calculate vertical speed from point i to point i+1 (at point i).
	 * 
	 * @param i
	 *            index of the point
	 * @return vertical speed
	 */
	public double verticalSpeed(int i) {
		if (i < 0 || i >= size() - 1) { // there is no velocity after the last
										// point
			addWarning("verticalSpeed: Attempt to get a vertical speed outside of the Plan: " + i);
			return 0;
		}
		return points.get(i).verticalSpeed(points.get(i + 1));
	}

	/**
	 * Estimate the velocity between point i to point i+1 for this aircraft.
	 * This is not defined for the last point of the plan.
	 */
	public Velocity averageVelocity(int i) {
		if (i >= size() - 1) { // there is no velocity after the last point
			addWarning("averageVelocity(int): Attempt to get an averge velocity after the end of the Plan: " + i);
			return Velocity.ZERO;
		}
		if (i < 0) {
			addWarning("averageVelocity(int): Attempt to get an average velocity before beginning of the Plan: " + i);
			return Velocity.ZERO;
		}
		return points.get(i).averageVelocity(points.get(i + 1));
		// return velocity((t2-t1)/2.0+t1);
	}

	/**
	 * calculate delta time for point i to make ground speed into it = gs
	 * 
	 * @param i
	 * @param gs
	 * @return
	 */
	public double calcDtGsin(int i, double gs) {
		if (i < 1 || i >= size()) {
			addError("calcTimeGSin: invalid index " + i, 0); // must have a prev
																// wPt
			return -1;
		}
		if (gs <= 0) {
			addError("calcTimeGSIn: invalid gs=" + f.Fm2(gs), i);
			return -1;
		}
		if (inGsChange(points.get(i).time()) && !points.get(i).isBGS()) {
			// f.pln("#### points.get(i) = "+points.get(i));
			double dist = pathDistance(i - 1, i);
			double initGs = initialVelocity(i - 1).gs();
			int ixBGS = prevBGS(i);
			double gsAccel = point(ixBGS).gsAccel();
			double deltaGs = gs - initGs;
			double dt = deltaGs / gsAccel;
			double acceldist = dt * (gs + initGs) / 2;
			if (acceldist > dist) {
				// f.pln("#### calcTimeGSin: insufficient distance to achieve
				// new ground speed");
				addError("calcTimeGSin " + f.Fm0(i) + " insufficient distance to achieve new ground speed", i);
				return -1;
			}
			dt = dt + (dist - acceldist) / gs;
			// f.pln("#### calcTimeGSin: dist = "+Units.str("nm",dist)+" deltaGs
			// = "+Units.str("kn",deltaGs)+" dt = "+dt);
			return dt;
		} else {
			double dist = pathDistance(i - 1, i);
			double dt = dist / gs;
			// f.pln("#### calcTimeGSin: i = "+i+" dist =
			// "+Units.str("nm",dist)+" dt = "+f.Fm4(dt)+"
			// points.get(i-1).time() = "+points.get(i-1).time());
			return dt;
		}
	}

	/**
	 * calculate time at a waypoint such that the ground speed into that
	 * waypoint is "gs". If i or gs is invalid, this returns -1. If i is in a
	 * turn, this returns the current point time.
	 * 
	 * Note: parameter maxGsAccel is not used on a linear segment
	 */
	public double calcTimeGSin(int i, double gs) {
		return points.get(i - 1).time() + calcDtGsin(i, gs);
	}

	/**
	 * change the ground speed into ix to be gs -- all other ground speeds
	 * remain the same
	 * 
	 * @param p   Plan of interest
	 * @param ix  index
	 * @param gs  new ground speed
	 * @return    revised plan
	 */
	public void mkGsInto(int ix, double gs, boolean updateTCP) {
		if (ix > size() - 1)
			return;
		double tmIx = calcTimeGSin(ix, gs);
		timeshiftPlan(ix, tmIx - point(ix).time());
		if (updateTCP) {
			NavPoint np = point(ix);
			if (np.isBeginTCP()) {
				Velocity vin = np.velocityInit();
				NavPoint npNew = np.makeVelocityInit(vin.mkGs(gs));
				set(ix, npNew);
			}
		}
	}

	/**
	 * Change the ground speed at ix to be gs -- all other ground speeds remain
	 * the same NOTE: This assumes that there are no BVS - EVS segments in the
	 * area
	 *
	 * Note: If point is a begin TCP, we need to update the velocityIn
	 * 
	 * @param p
	 *            Plan of interest
	 * @param ix
	 *            index
	 * @param gs
	 *            new ground speed
	 * @return revised plan
	 */
	public void mkGsOut(int ix, double gs) {
		if (ix >= size() - 1)
			return;
		double dt = calcTimeGSin(ix + 1, gs);
		// f.pln("\n $$ makeGsOut: ix = "+ix+" dt = "+f.Fm4(dt)+" gs =
		// "+Units.str("kn",gs));
		// f.pln(" $$$$ ix = "+ix+" BEFORE vsOut =
		// "+Units.str("fpm",vsOut(ix)));
		timeshiftPlan(ix + 1, dt - getTime(ix + 1));
		NavPoint np = point(ix);
		if (np.isBeginTCP()) {
			Velocity vin = np.velocityInit();
			// vin = vin.mkGs(gs);
			double vsOut = vsOut(ix);
			vin = Velocity.mkTrkGsVs(vin.trk(), gs, vsOut);
			NavPoint npNew = np.makeVelocityInit(vin);
			// f.pln(" $$ makeGsOut: vin = "+vin+" npNew =
			// "+npNew.toStringFull());
			set(ix, npNew);
			// f.pln(" $$$$ ix = "+ix+" AFTER vsOut =
			// "+Units.str("fpm",vsOut(ix)));
		}
		// f.pln(" $$$$ makeGsOut: point(ix) = "+point(ix).toStringFull());
		// f.pln(" $$$$ makeGsOut: EXIT gsOut = "+Units.str("kn",gsOut(ix),6));
		// f.pln(" $$$$ p = "+toStringGs());
	}

	// return time needed at waypoint i in order for ground speed in to be gs
	public double linearCalcTimeGSin(int i, double gs) {
		if (i > 0 && i < size()) {
			double dist = point(i - 1).distanceH(point(i));
			double dt = dist / gs;
			// f.pln(" $$$$ calcTimeGSin: d = "+Units.str("nm",d));
			return point(i - 1).time() + dt;
		} else {
			addError("linearCalcTimeGSin(2): index " + i + " out of bounds");
			return -1;
		}
	}

	/**
	 * set the time at a waypoint such that the ground speed into that waypoint
	 * is "gs", given the ground speed accelerations (possibly ignored);
	 * 
	 * Note: This does not leave speeds after this point unchanged
	 */
	public void setTimeGSin(int i, double gs) {
		double newT = calcTimeGSin(i, gs);
		// f.pln("Plan.setTimeGSin newT="+newT);
		setTime(i, newT);
	}

	/**
	 * set the time at a waypoint such that the ground speed into that waypoint
	 * is "gs", given the ground speed accelerations (possibly ignored)
	 */
	public void setAltVSin(int i, double vs, boolean preserve) {
		if (i <= 0)
			return;
		double dt = point(i).time() - point(i - 1).time();
		double newAlt = point(i - 1).alt() + dt * vs;
		// f.pln(" $$$$$ setAltVSin: dt = "+dt+" newAlt =
		// "+Units.str("ft",newAlt));
		// setAlt(i,newAlt,preserve);
		NavPoint tempv = points.get(i).mkAlt(newAlt);
		if (preserve)
			tempv = tempv.makeAltPreserve();
		set(i, tempv);

	}

	public boolean isVelocityContinuous() {
		for (int i = 0; i < size(); i++) {
			if (i > 0) {
				if (point(i).isTCP()) {
					if (!finalVelocity(i - 1).compare(initialVelocity(i), Units.from("deg", 10.0), Units.from("kn", 20),
							Units.from("fpm", 100))) { // see testAces3, testRandom for worst cases
						// f.pln(" $$$ isVelocityContinuous: FAIL! continuity: finalVelocity("+(i-1)+") = "+finalVelocity(i-1).toString4NP()+" != initialVelocity("+i+") = "+initialVelocity(i).toString4NP());
						return false;
					}
				}
			}
		}
		return true;
	}

	/**
	 * Find the horizontal (curved) distance between points i and i+1 [meters].
	 * 
	 * @param i
	 *            index of starting point
	 * @return path distance (horizontal only)
	 */
	public double pathDistance(int i) {
		return pathDistance(i, false);
	}

	/**
	 * Find the horizontal distance between points i and i+1 [meters].
	 * 
	 * @param i
	 *            index of starting point
	 * @param linear
	 *            if true, measure the straight distance, if false measure the
	 *            curved distance
	 * @return path distance (horizontal only)
	 */
	public double pathDistance(int i, boolean linear) {
		if (i < 0 || i + 1 >= size()) {
			return 0.0;
		}
		NavPoint p1 = points.get(i);
		double tt = p1.time();
		if (!linear && inTrkChange(tt)) {
			// if in a turn, figure the arc distance
			NavPoint p2 = points.get(i + 1);
			NavPoint bot = points.get(prevBOT(i + 1));
			Position center = bot.turnCenter();
			double R = bot.turnRadius();
			double theta = PositionUtil.angle_between(p1.position(), center, p2.position());
			// double theta =
			// GreatCircle.side_side_angle(GreatCircle.angular_distance(p1.position().lla(),center.lla()),GreatCircle.angular_distance(p2.position().lla(),center.lla()),Math.PI/2,false).second;
			// double theta =
			// GreatCircle.angle_temp(p1.position().lla(),center.lla(),p2.position().lla());
			// f.pln(" $$ pathDistance: R = "+Units.str("NM",R)+ "
			// theta="+Units.to("deg",theta)+" bot="+bot+"
			// same="+p1.equals(bot));
			return Math.abs(theta * R); // TODO is this right for spherical coordinates???
		} else {
			// otherwise just use linear distance
			return points.get(i).position().distanceH(points.get(i + 1).position());
		}
	}


	/**
	 * Find the cumulative horizontal (curved) path distance for whole plan.
	 */
	public double pathDistance() {
		return pathDistance(0, size(), false);
	}

	/**
	 * Find the cumulative horizontal (curved) path distance between points i
	 * and j [meters].
	 */
	public double pathDistance(int i, int j) {
		return pathDistance(i, j, false);
	}

	/**
	 * Find the cumulative horizontal path distance between points i and j
	 * [meters].
	 * 
	 * @param i
	 *            beginning index
	 * @param j
	 *            ending index
	 * @param linear
	 *            if true, then TCP turns are ignored. Otherwise, the length of
	 *            the circular turns are calculated.
	 * @return cumulative path distance (horizontal only)
	 */
	public double pathDistance(int i, int j, boolean linear) {
		// f.pln(" $$ pathDistance: i = "+i+" j = "+j+" size = "+size());
		if (i < 0) {
			i = 0;
		}
		if (j >= size()) { // >= is correct, pathDistance(jj, linear) measures
							// from jj to jj+1
			j = size() - 1;
		}
		double total = 0.0;
		for (int jj = i; jj < j; jj++) {
			total = total + pathDistance(jj, linear);
			// f.pln(" $$ pathDistance: i = "+i+" jj = "+jj+" dist =
			// "+Units.str("NM",pathDistance(jj, linear)));
		}
		// f.pln(" $$ Plan.pathDistance: i = "+i+" j = "+j+" total =
		// "+Units.str("NM",total));
		return total;
	}

	/**
	 * return the path distance from the location at time t until the next waypoint
	 * 
	 * @param t      current time of interest
	 * @param linear if "linear" then ignore BOTs
	 * @return       path distance
	 */
	public double partialPathDistance(double t, boolean linear) {
		if (t < getFirstTime() || t >= getLastTime()) {
			return 0.0;
		}
		int seg = getSegment(t);
		Position currentPosition = position(t);
		// f.pln("\n $$$ partialPathDistance: seg = "+seg);
		// if in a turn, figure the arc distance
		if (inTrkChange(t) && !linear) {
			NavPoint bot = points.get(prevBOT(getSegment(t) + 1));
			double R = bot.turnRadius();
			Position center = bot.turnCenter();
			// double distAB =
			// points.get(i).position().distanceH(points.get(i+1).position());
			// double alpha = 2*(Math.asin(distAB/(2*R)));
			// double dt = points.get(seg+1).time() - t;
			// double alpha = points.get(seg).trkAccel()*dt;
			double alpha = PositionUtil.angle_between(currentPosition, center, points.get(seg + 1).position());
			// f.pln(" $$$$ alpha = "+Units.str("deg",alpha));
			double rtn = Math.abs(alpha * R);
			// f.pln(" $$$$+++++ partialPathDistance: rtn =
			// "+Units.str("nm",rtn));
			return rtn;
		} else {
			// otherwise just use linear distance
			double rtn = position(t).distanceH(points.get(seg + 1).position());
			// f.pln(" $$$$.... partialPathDistance: points.get(seg+1) =
			// "+points.get(seg+1));
			// f.pln(" $$$$.... partialPathDistance: rtn =
			// "+Units.str("nm",rtn));
			return rtn;
		}
	}

	/**
	 * return the path distance from the location at time t until the next waypoint
    */
	public double partialPathDistance(double t) {
		boolean linear = false;
		return partialPathDistance(t, linear);
	}

	/**
	 * calculate path distance from the current position at time t to point j
	 * 
	 * @param t   current time
	 * @param j   next point
	 * @return path distance
	 */
	public double pathDistanceFromTime(double t, int j) {
		int i = getSegment(t);
		double dist_i_j = pathDistance(i + 1, j);
		double dist_part = partialPathDistance(t);
		if (j >= i) {
			return dist_i_j + dist_part;
		} else {
			return dist_i_j - dist_part;
		}
	}

	/**
	 * calculates vertical distance from point i to point i+1
	 * 
	 * @param i  point of interest
	 * @return   vertical distance
	 */
	public double vertDistance(int i) {
		// f.pln(" $$$$$$ pathDistance: i = "+i+" points = "+points);
		if (i < 0 || i + 1 > size()) {
			return 0;
		}
		return points.get(i + 1).position().z() - points.get(i).position().z();
	}

	/**
	 * Find the cumulative vertical distance between points i and j [meters].
	 */
	public double vertDistance(int i, int j) {
		if (i < 0) {
			i = 0;
		}
		if (j > size()) {
			j = size();
		}
		double total = 0.0;
		for (int jj = i; jj < j; jj++) {
			// System.out.println("pathDistance "+jj+" "+legDist);
			total = total + vertDistance(jj);
			// System.out.println("jj="+jj+" total="+total);
		}
		return total;
	}

	/**
	 * calculate average ground speed over entire plan
	 * 
	 * @return average ground speed
	 */
	public double averageGroundSpeed() {
		if (size() < 2) return 0.0;
		return pathDistance() / (getLastTime() - getFirstTime());
	}


	/**
	 * returns the first linear segment greater than or equal to ix
	 * 
	 * @param ix
	 *            current segment or size() if there is no future segment that
	 *            is linear
	 * @return
	 */
	public int nextLinearSegment(int ix) {
		int ixFirst = size();
		for (ixFirst = ix; ixFirst < size(); ixFirst++) {
			NavPoint np = point(ixFirst);
			if (!inAccel(np.time())) {
				break; // i is a linear segment
			}
		}
		return ixFirst;
	}

	/**
	 * This returns true if the entire plan is "well formed", i.e. all
	 * acceleration zones have a matching beginning and end point.
	 */
	public boolean isWellFormed() {				
		return indexWellFormed() < 0;
	}

	/**
	 * This returns -1 if the entire plan is "well formed", i.e. all
	 * acceleration zones have a matching beginning and end point. Returns a
	 * nonnegative value to indicate the problem point
	 */
	public int indexWellFormed() {
		double lastTm = -1;
		for (int i = 0; i < size(); i++) {
			NavPoint np = point(i);
			if (np.isBOT()) {
				int j1 = nextBOT(i);
				int j2 = nextEOT(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return i;
			}
			if (np.isEOT()) {
				int j1 = prevBOT(i);
				int j2 = prevEOT(i);
				if (!(j1 >= 0 && j1 >= j2))
					return i;
			}
			if (np.isBGS()) {
				int j1 = nextBGS(i);
				int j2 = nextEGS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return i;
			}
			if (np.isEGS()) {
				int j1 = prevBGS(i);
				int j2 = prevEGS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return i;
			}
			if (np.isBVS()) {
				int j1 = nextBVS(i);
				int j2 = nextEVS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return i;
			}
			if (np.isEVS()) {
				int j1 = prevBVS(i);
				int j2 = prevEVS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return i;
			}
			double tm_i = getTime(i);
			if (i > 0) {
				double dt = Math.abs(tm_i-lastTm);
				if (dt < minDt) {
					f.pln("$$ isWellFormed: Delta time into i = "+i+" is less than minDt = "+minDt);
					return i;
				}			
				lastTm = tm_i;
			}
		}
		return -1;
	}

	/**
	 * This returns a string representing which part of the plan is not "well
	 * formed", i.e. all acceleration zones have a matching beginning and end
	 * point.  See isWellFormed().
	 */
	public String strWellFormed() {
		// f.pln(" isWellFormed: size() = "+size());
		String rtn = "";
		double lastTm = -1;
		for (int i = 0; i < size(); i++) {
			NavPoint np = point(i);
			// not well formed if GSC overlaps with other accel zones
			// if ((np.isTurn() || np.isVSC()) &&
			// inGroundSpeedChange(np.time())) rtn = false;
			// if (np.isGSC() && (inTurn(np.time()) ||
			// inVerticalSpeedChange(np.time()))) rtn = false;
			if (np.isBOT()) {
				int j1 = nextBOT(i);
				int j2 = nextEOT(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return "BOT at i " + i + " NOT FOLLOWED BY EOT!";
			}
			if (np.isEOT()) {
				int j1 = prevBOT(i);
				int j2 = prevEOT(i);
				if (!(j1 >= 0 && j1 >= j2))
					return "EOT at i " + i + " NOT PRECEEDED BY BOT!";
			}
			if (np.isBGS()) {
				int j1 = nextBGS(i);
				int j2 = nextEGS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return "BGS at i " + i + " NOT FOLLOWED BY EGS!";
			}
			if (np.isEGS()) {
				int j1 = prevBGS(i);
				int j2 = prevEGS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return "EGS at i " + i + " NOT PRECEEDED BY BGS!";
			}
			if (np.isBVS()) {
				int j1 = nextBVS(i);
				int j2 = nextEVS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return "BVS at i " + i + " NOT FOLLOWED BY EVS!";
			}
			if (np.isEVS()) {
				int j1 = prevBVS(i);
				int j2 = prevEVS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return "EVS at i " + i + " NOT PRECEEDED BY BVS!";
			}
			if (this.inGsChange(np.time()) && this.inTrkChange(np.time())) {
				rtn = rtn + "  Overlap FAIL at i = " + i;
			}
			// f.pln(" isWellFormed: i = "+i+" OK");
			double tm_i = getTime(i);
			if (i > 0) {
				double dt = Math.abs(tm_i-lastTm);
				if (dt < minDt) {
					return "Delta time into i = "+i+" is less than minDt = "+minDt;
				}			
				lastTm = tm_i;
			}
		}
		return rtn;
	}

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instanteous "jumps," it is not consistent.
	 */
	public boolean isConsistent() {
		boolean silent = false;
		boolean useProjection = false;
		return isConsistent(silent, useProjection);
	}

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instanteous "jumps," it is not consistent.
	 */
	public boolean isConsistent(boolean silent, boolean useProjection) {
		boolean rtn = true;
		if (!isWellFormed()) {
			if (!silent) {
				f.pln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			}
			error.addError("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			return false;
		}
		for (int i = 0; i < size(); i++) {
			if (point(i).isBGS()) {
				if (!PlanUtil.gsConsistent(this, i, 0.00005, 0.07, silent)) {
					// error.addWarning("isConsistent fail: "+i+" Not gs
					// consistent!");
					rtn = false;
				}
			}
			if (point(i).isBVS()) {
				if (!PlanUtil.vsConsistent(this, i, 0.00001, 0.00001, silent)) {
					// error.addWarning("isConsistent fail: "+i+" Not vs
					// consistent!");
					rtn = false;
				}
			}
			if (point(i).isBOT()) {
				if (!PlanUtil.turnConsistent(this, i, 0.02, 0.01, 1.2, silent, useProjection)) {
					// error.addWarning("isConsistent fail: "+i+" Not turn
					// consistent!");
					rtn = false;
				}
			}
			if (i > 0) {
				if (point(i).isTCP()) {
					if (!PlanUtil.isVelocityContinuous(this, i, 2.6, silent)) {
						// error.addWarning("isConsistent fail: "+i+" Not
						// continuous!");
						rtn = false;
					}
				}
			}
		}
		// f.pln("KinPlan.consistency: "+toOutput());
		// f.pln(" $$$ isConsistent: rtn = "+rtn );
		return rtn;
	}

	public boolean isTurnConsistent(boolean silent) {
		boolean rtn = true;
		if (!isWellFormed()) {
			if (!silent) {
				f.pln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			}
			error.addError("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			return false;
		}
		for (int i = 0; i < size(); i++) {
			if (point(i).isBOT()) {
				if (!PlanUtil.turnConsistent(this, i, 0.02, 0.01, 1.2, silent, false)) {
					// error.addWarning("isConsistent fail: "+i+" Not turn
					// consistent!");
					rtn = false;
				}
			}
		}
		return rtn;
	}

	public boolean isVsConsistent(boolean silent) {
		boolean rtn = true;
		if (!isWellFormed()) {
			if (!silent) {
				f.pln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			}
			error.addError("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			return false;
		}
		for (int i = 0; i < size(); i++) {
			if (point(i).isBVS()) {
				if (!PlanUtil.vsConsistent(this, i, 0.00001, 0.00001, silent)) {
					// error.addWarning("isConsistent fail: "+i+" Not vs
					// consistent!");
					rtn = false;
				}
			}
		}
		return rtn;
	}

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instanteous "jumps," it is not consistent.
	 */
	public boolean isWeakConsistent(boolean silent, boolean useProjection) {
		// f.pln(" >>> isConsistent silent = "+silent+" useProjection =
		// "+useProjection);
		boolean rtn = true;
		if (!isWellFormed()) {
			error.addError("isWeakConsistent: not well formed");
			if (!silent) {
				f.pln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			}
			return false;
		}
		for (int i = 0; i < size(); i++) {
			if (point(i).isBGS()) {
				if (!PlanUtil.gsConsistent(this, i, 0.2, 0.1, silent)) {
					error.addWarning("isWeakConsistent: GS " + i + " not consistent");
					rtn = false;
				}
			}
			if (point(i).isBVS()) {
				if (!PlanUtil.vsConsistent(this, i, 0.001, 0.05, silent)) {
					error.addWarning("isWeakConsistent: VS " + i + " not consistent");
					rtn = false;
				}
			}
			if (point(i).isBOT()) {
				if (!PlanUtil.turnConsistent(this, i, 0.5, 1.1, 1.2, silent, useProjection)) {
					error.addWarning("isWeakConsistent: turn " + i + " not consistent");
					rtn = false;
				}
			}
			if (i > 0) {
				if (point(i).isTCP()) {
					if (!PlanUtil.isVelocityContinuous(this, i, 5.0, silent)) {
						error.addWarning("isWeakConsistent: velocity at point " + i + " is not consistent");
						rtn = false;
					}
				}
			}
			// rtn = rtn & PlanUtil.gsConsistent(this, i, 0.2, 0.1, silent);
			// rtn = rtn & PlanUtil.vsConsistent(this, i, 0.001, 0.05, silent);
			// rtn = rtn & PlanUtil.turnConsistent(this, i, 0.1, 0.5, 1.2,
			// silent, useProjection);
			// if (i > 0) {
			// if (point(i).isTCP()) {
			// if (! PlanUtil.isVelocityContinuous(this, i, 5.00, silent)) rtn =
			// false;
			// }
			// }
		}
		// f.pln("KinPlan.consistency: "+toOutput());
		// f.pln(" $$$ isConsistent: rtn = "+rtn );
		return rtn;
	}

	public boolean isWeakConsistent() {
		boolean silent = false;
		boolean useProjection = false;
		return isWeakConsistent(silent, useProjection);
	}

	/**
	 * This removes the acceleration tags on points that appear "unbalanced."
	 * This is not particularly intelligent and may result in bizarre (but
	 * legal) plans.
	 */
	public void fix() {
		// if (!isWellFormed()) {
		f.pln(" Plan.fix has not been ported yet -- used only in Watch!");
		// for (int i = 0; i < size(); i++) {
		// NavPoint np = point(i);
		// if ((np.isGSCBegin() && nextGSCEnd(i) < 0) ||
		// (np.isGSCEnd() && prevGSCBegin(np.time()) < 0) ||
		// (np.isTurnBegin() && nextEOT(i) < 0) ||
		// (np.isTurnEnd() && prevBOT(np.time()) < 0) ||
		// (np.isVSCBegin() && nextVSCEnd(i) < 0) ||
		// (np.isVSCEnd() && prevVSCBegin(np.time()) < 0)
		// )
		// {
		// set(i, np.makeTCPClear());
		// }
		// }
		// }
	}

	/**
	 * experimental -- only used in Watch find the closest point on the given
	 * segment of the current plan to position p, only considering horizontal
	 * distance.
	 */
	public NavPoint closestPointHoriz(int seg, Position p) {
		// f.pln("\nPlan.closestPointHoriz seg="+seg+" p="+p);
		if (seg < 0 || seg >= size() - 1) {
			addError("closestPositionHoriz: invalid index");
			return NavPoint.INVALID;
		}
		double d = 0;
		double t1 = getTime(seg);
		double dt = getTime(seg + 1) - t1;
		NavPoint np = points.get(seg);
		NavPoint np2 = points.get(seg + 1);
		if (pathDistance(seg) <= 0.0) {
			// f.pln("Plan.closestPositionHoriz: "+name+" patDistance for seg
			// "+seg+" is zero");
			// f.pln(toString());
			return np;
		}
		NavPoint ret;
		// vertical case is special
		if (Util.almost_equals(initialVelocity(seg).gs(), 0.0)
				&& Util.almost_equals(initialVelocity(seg + 1).gs(), 0.0)) {
			if ((p.alt() <= np.alt() && np.alt() <= np2.alt()) || (p.alt() >= np.alt() && np.alt() >= np2.alt())) {
				ret = np;
			} else if ((p.alt() <= np2.alt() && np2.alt() <= np.alt())
					|| (p.alt() >= np2.alt() && np2.alt() >= np.alt())) {
				ret = np2;
			} else if (inVsChange(t1)) {
				double vs1 = initialVelocity(seg).vs();
				double a = point(prevBVS(seg + 1)).vsAccel();
				double tm = KinematicsDist.gsTimeConstantAccelFromDist(vs1, a, p.alt() - np.alt());
				ret = new NavPoint(position(tm), tm);
			} else {
				double vtot = Math.abs(np.alt() - np2.alt());
				double frac = Math.abs(np.alt() - p.alt()) / vtot;
				double tm = dt * frac - t1;
				ret = new NavPoint(position(tm), tm);
			}
		} else if (inTrkChange(t1)) {
			NavPoint bot = points.get(prevBOT(seg + 1));
			Position center = bot.turnCenter();
			double endD = pathDistance(seg);
			double d2 = KinematicsPosition.closestDistOnTurn(np.position(), initialVelocity(seg), bot.turnRadius(),
					Util.sign(bot.signedRadius()), center, p, endD);
			if (Util.almost_equals(d2, 0.0)) {
				ret = np;
			} else if (Util.almost_equals(d2, endD)) {
				ret = np2;
			} else {
				double segDt = timeFromDistance(seg, d2);
				ret = new NavPoint(position(t1 + segDt), t1 + segDt);
			}
		} else if (isLatLon()) {
			LatLonAlt lla = GreatCircle.closest_point_segment(points.get(seg).lla(), points.get(seg + 1).lla(),
					p.lla());
			d = GreatCircle.distance(points.get(seg).lla(), lla);
			double segDt = timeFromDistance(seg, d);
			ret = new NavPoint(position(t1 + segDt), t1 + segDt);
		} else {
			Vect3 cp = VectFuns.closestPointOnSegment(points.get(seg).point(), points.get(seg + 1).point(), p.point());
			d = points.get(seg).point().distanceH(cp);
			double segDt = timeFromDistance(seg, d);
			ret = new NavPoint(position(t1 + segDt), t1 + segDt);
		}
		return ret;
	}

	// /** experimental
	// * find the closest point on the current plan to position p between two
	// times.
	// */
	// public NavPoint closestPoint(double start, double end, Position p) {
	// start = Math.min(Math.max(getFirstTime(), start), getLastTime());
	// end = Math.max(Math.min(getLastTime(), end), getFirstTime());
	// int s = getSegment(start);
	// int e = getSegment(end)+1;
	// if (end == getLastTime()) e = size()-1;
	// if (s < 0 || e < 0) {
	// addError("closestPoint invalid start or end time");
	// return NavPoint.INVALID;
	// }
	// NavPoint sp = NavPoint.INVALID;
	// NavPoint ep = NavPoint.INVALID;
	// NavPoint tmp = new NavPoint(position(start),start);
	// if (start == end) return tmp; // out of bounds
	// int i = overlaps(tmp,true);
	// if (i < 0) {
	// sp = tmp.makeVirtual();
	// s = add(sp);
	// e++;
	// }
	// tmp = new NavPoint(position(end),end);
	// int j = overlaps(tmp,true);
	// if (j < 0) {
	// ep = tmp.makeVirtual();
	// e = add(ep);
	// }
	// NavPoint ret = closestPoint(s,e,p);
	// // clean up the virtual point(s), if added
	// if (j >= 0 && points.get(e).isVirtual()) {
	// remove(e);
	// }
	// if (i >= 0 && i < j && points.get(s).isVirtual()) {
	// remove(s);
	// }
	// if (getFirstTime() > ret.time()) {
	// f.pln("closestPoint(ffp)");
	// }
	//
	// return ret;
	// }

	/**
	 * Experimental This returns a NavPoint on the plan that is closest to the
	 * given position. If more than one point are closest horizontally, the
	 * closer vertically is returned. If more than one have the same horizontal
	 * and vertical distances, the first is returned.
	 * 
	 * @param p
	 * @return
	 */
	// experimental
	public NavPoint closestPoint(Position p) {
		return closestPoint(0, points.size() - 1, p, false, 0.0);
	}

	public NavPoint closestPointHoriz(Position p) {
		return closestPoint(0, points.size() - 1, p, true, Units.from("ft", 100));
	}

	/**
	 * Experimental This returns a NavPoint on the plan within the given segment
	 * range that is closest to the given position. If more than one point are
	 * closest horizontally, the closer vertically is returned. If more than one
	 * have the same horizontal and vertical distances, the first is returned.
	 * If start >= end, this returns an INVALID point
	 * 
	 * @param start  start point
	 * @param end    end point
	 * @param p      position to check against
	 * @param horizOnly
	 *            if true, only consider horizontal distances, if false, also
	 *            compare vertical distances if the closest points on 2 segments
	 *            are within maxHdist of each other
	 * @param maxHdist
	 *            only used if horizOnly is false: compare vertical distances if
	 *            candidate points are within this range of each other
	 * @return
	 */
	public NavPoint closestPoint(int start, int end, Position p, boolean horizOnly, double maxHdist) {
		double minhdist = Double.MAX_VALUE;
		double minvdist = Double.MAX_VALUE;
		NavPoint closest = NavPoint.INVALID;
		for (int i = start; i < end; i++) {
			NavPoint np = closestPointHoriz(i, p);
			double dh = np.position().distanceH(p);
			double dv = np.position().distanceV(p);
			if (dh < minhdist || (!horizOnly && Util.within_epsilon(dh, minhdist, maxHdist) && dv < minvdist)) {
				minhdist = dh;
				minvdist = dv;
				closest = np;
			}
		}
		if (getFirstTime() > closest.time()) {
			f.pln("closestPoint(iip)");
		}
		return closest;
	}

	public void diff(Plan p) {
		if (this.size() != p.size()) {
			f.pln(" different sizes: " + this.size() + " " + p.size());
			f.pln(this.toOutput(0, 4, false, true));
			f.pln(p.toOutput(0, 4, false, true));
		}
		for (int i = 0; i < p.size(); i++) {
			this.point(i).diff(i, p.point(i));
		}
	}

	/**
	 * Structurally revert TCP at ix: (does not depend upon source time or
	 * source position!!) This private method assumes ix > 0 AND ix <
	 * pln.size(). If ix is not a BOT, then nothing is done
	 * 
	 * @param pln   plan
	 * @param ix    index of point to be reverted
	 * @param addBackMidPoints
	 *            if addBackMidPoints = true, then if there are extra points
	 *            between the BOT and EOT, make sure they are moved to the
	 *            correct place in the new linear sections. Do this by distance
	 *            not time.
	 * @param killNextGsTCPs
	 *            if true, then if there is a BGS-EGS pair after the turn remove
	 *            both of these
	 * @param zVertex
	 *            if non-negative, then assigned reverted vertex this altitude
	 */
	public void structRevertTurnTCP(int ix, boolean addBackMidPoints, boolean killNextGsTCPs, double zVertex) {
		// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT =
		// "+pln.point(ix).isBOT());
		// f.pln(" $$$$$ structRevertTurnTCP: pln = "+pln);
		if (point(ix).isBOT()) {
			// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix);
			NavPoint BOT = point(ix);
			int BOTlinIndex = BOT.linearIndex();
			String label = BOT.label();
			double tBOT = BOT.time();
			int nextEOTix = nextEOT(ix);
			NavPoint EOT = point(nextEOTix);
			double tEOT = EOT.time();
			ArrayList<NavPoint> betweenPoints = new ArrayList<NavPoint>(4);
			ArrayList<Double> betweenPointDists = new ArrayList<Double>(4);
			if (addBackMidPoints) { // add back mid points that are not TCPs
				for (int j = ix + 1; j < nextEOTix; j++) {
					NavPoint np = point(j);
					if (!np.isTCP()) {
						// f.pln(" >>>>> structRevertTurnTCP: SAVE MID
						// point("+j+") = "+point(j).toStringFull());
						betweenPoints.add(np);
						double distance_j = pathDistance(ix, j);
						betweenPointDists.add(distance_j);
					}
				}
			}
			Velocity vin;
			if (ix == 0)
				vin = point(ix).velocityInit(); // not sure if we should allow
												// TCP as current point ??
			else
				vin = finalVelocity(ix - 1);
			double gsin = vin.gs();
			// f.pln(" $$$$ structRevertTurnTCP: gsin =
			// "+Units.str("kn",gsin,8));
			Velocity vout = initialVelocity(nextEOTix);
			Pair<Position, Double> interSec = Position.intersection(BOT.position(), vin, EOT.position(), vout);
			double distToIntersec = interSec.first.distanceH(BOT.position());
			double tmInterSec = tBOT + distToIntersec / gsin;
			NavPoint vertex;
			if (tmInterSec >= tEOT) { // use BOT position, if vertex angle is
										// too small
				double tMid = (tBOT + tEOT) / 2.0;
				Position posMid = position(tMid);
				// f.pln(" $$$$ structRevertTurnTCP: use BOT position, if vertex
				// angle is too small posMid = "+posMid);
				vertex = new NavPoint(posMid, tMid);
			} else {
				vertex = new NavPoint(interSec.first, tmInterSec);
				// f.pln(" $$$$ structRevertTurnTCP: interSec.first =
				// "+interSec.first);
			}
			if (vertex.isInvalid()) {
				addError(" structRevertTurnTCP: reversion of point " + ix + " failed!");
				return;
			}
			if (zVertex >= 0) {
				vertex = vertex.mkAlt(zVertex); // a better value for altitude
												// obtained from previous
												// internal BVS-EVS pair
			}
			// ======================== No Changes To Plan Before This Point
			// ================================
			double gsInNext = vout.gs();
			// TODO this may be wrong if there are combined EOT+BGS points
			if (killNextGsTCPs & nextEOTix + 1 < size()) {
				NavPoint npAfter = point(nextEOTix + 1);
				// double dt = getTime(nextEOTix+1) - getTime(nextEOTix);
				// f.pln(" $$$$ structRevertTurnTCP: npAfter =
				// "+npAfter.toStringFull()+" dt = "+dt);
				// if (npAfter.isBGS() && dt < Plan.revertGsTurnConnectionTime)
				// { // note that makeKinematicPlan always makes it 1 sec after
				// the EOT
				if (npAfter.isBGS() && (npAfter.linearIndex() == BOTlinIndex)) {
					int ixEGS = nextEGS(nextEOTix);
					vout = initialVelocity(ixEGS);
					gsInNext = vout.gs();
					// f.pln(" $$$$$ let's KILL TWO GS points AT
					// "+(nextEOTix+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
					// f.pln(" $$$$$ KILL "+point(ixEGS).toStringFull());
					// f.pln(" $$$$$ KILL "+point(nextEOTix+1).toStringFull());
					remove(ixEGS);
					remove(nextEOTix + 1);
				}
			}
			// Kill all points between ix and nextEOTix
			// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" nextEOTix =
			// "+nextEOTix);
			for (int k = nextEOTix; k >= ix; k--) {
				// f.pln(" $$$$ structRevertTurnTCP: remove point k = "+k+"
				// "+point(k).toStringFull());
				remove(k);
			}
			// f.pln(" $$$$ structRevertTurnTCP: ADD vertex = "+vertex);
			int ixAdd = add(vertex.makeLabel(label));
			int ixNextPt = ixAdd + 1;
			// add back all removed points with revised position and time
			if (addBackMidPoints) {
				for (int i = 0; i < betweenPointDists.size(); i++) {
					double newTime = BOT.time() + betweenPointDists.get(i) / vin.gs();
					Position newPosition = position(newTime);
					NavPoint savePt = betweenPoints.get(i);
					NavPoint np = savePt.makePosition(newPosition).makeTime(newTime).mkAlt(savePt.alt());
					add(np);
					// f.pln(" $$$$ structRevertTurnTCP: ADD BACK np = "+np);
					ixNextPt++;
				}
			}
			// fix ground speed after
			// f.pln(" $$$$ structRevertTurnTCP: ixNextPt = "+ixNextPt+"
			// gsInNext = "+Units.str("kn", gsInNext));
			double tmNextSeg = getTime(ixNextPt);
			if (tmNextSeg > 0) { // if reverted last point, no need to timeshift
									// points after dSeg
				int newNextSeg = getSegment(tmNextSeg);
				double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
				double dt2 = newNextSegTm - tmNextSeg;
				// f.pln(" $$$$$$$$ structRevertTurnTCP: dt2 = "+dt2);
				timeshiftPlan(newNextSeg, dt2);
			}
			// f.pln(" $$$$ structRevertTurnTCP: initialVelocity("+ixNextPt+") =
			// "+initialVelocity(ixNextPt));
			removeRedundantPoints(getIndex(tBOT), getIndex(tEOT));
		}
	}

	/**
	 * Structurally revert BGS-EGS pair at index "ix"
	 * 
	 * @param pln  plan
	 * @param ix   index
	 * @return index of reverted BGS
	 * 
	 *         Note: it assumes that BGS-EGS pairs will be removed in ascending
	 *         order
	 */
	public void structRevertGsTCP(int ix) {
		if (point(ix).isBGS()) {
			// f.pln("\n\n $$$$>>>>>>>>>>>>>>>>>>>>>> structRevertGsTCP:
			// point("+ix+") = "+point(ix).toStringFull());
			int nextEGSix = nextEGS(ix);
			NavPoint BGS = point(ix);
			NavPoint EGS = point(nextEGSix);
			if (nextEGSix < 0) {
				addError(" structRevertGsTCP: Ill-formed BGS-EGS structure: no EGS found!");
				return;
			}
			double gsOutEGS = gsOut(nextEGSix);
			NavPoint newPoint = BGS.makeNewPoint();
			// f.pln(" $$$$ ix = "+ix+" nextEGSix = "+nextEGSix+" gsOutEGS =
			// "+Units.str("kn",gsOutEGS));
			if (EGS.isBGS()) { // i.e. it is a EGSBGS
				// f.pln(" $$$$---------------------- structRevertGsTCP AA: pln
				// = "+toStringGs());
				String label_EGSBGS = EGS.label();
				NavPoint newBGS = BGS
						.makeBGS(EGS.position(), EGS.time(), EGS.gsAccel(), EGS.velocityInit(), EGS.linearIndex())
						.makeLabel(label_EGSBGS);
				remove(nextEGSix); // remove EGSBGS
				int ixNewBGS = add(newBGS); // make EGSBGS just a BGS
				// f.pln(" $$$$ VIRTUAL: ixNewBGS = "+ixNewBGS+" gsOutEGS =
				// "+Units.str("kn",gsOutEGS));
				if (ixNewBGS >= 0) {
					mkGsOut(ixNewBGS, gsOutEGS);
				}
			} else {
				// f.pln(" $$$$---------------------- structRevertGsTCP BB: pln
				// = "+toStringGs());
				remove(nextEGSix);
			}
			// make the BGS a normal point
			remove(ix);
			int iNew = add(newPoint);
			// f.pln(" $$$ structRevertGsTCP: add newPoint = "+newPoint+" iNew =
			// "+iNew);
			if (iNew >= 0) { // && nextEGSix < size()-1) {
				mkGsOut(iNew, gsOutEGS);
			}
		}
		// f.pln(" $$$ structRevertGsTCP EE: pln = "+toStringGs());
		return;
	}

	/**
	 * Revert all BGS-EGS pairs
	 * 
	 * @param pln  plan file
	 * @return     reverted plan containing no ground speed TCPS
	 */

	public void revertGsTCPs() {
		revertGsTCPs(0);
	}

	/**
	 * Revert all BGS-EGS pairs in range "start" to "end"
	 * 
	 * @param pln    plan file
	 * @param start  starting index
	 * @param end    ending index
	 * @return reverted plan containing no ground speed TCPS in specified range
	 */
	public void revertGsTCPs(int start) {
		// f.pln(" $$$ revertGsTCPs start = "+start+" end = "+end);
		if (start < 0)
			start = 0;
		for (int j = start; j < size(); j++) {
			// f.pln(" $$$ REVERT GS AT j = "+j+" np_ix = "+point(j));
			// boolean revertPreviousTurn = false;
			structRevertGsTCP(j); // ,revertPreviousTurn);
		}
	}

	/**
	 * Revert BVS at ix
	 * 
	 * @param pln   plan file
	 * @param ix    index of a BVS point
	 * @return reverted plan containing no vertical TCPS
	 */
	protected double structRevertVsTCP(int ix) {
		if (point(ix).isBVS()) {
			// f.pln(" $$$$>>>>>>>>> structRevertVsTCP: point("+ix+") =
			// "+point(ix).toStringFull());
			NavPoint BVS = point(ix);
			int nextEVSix = nextEVS(ix); 
			// if (nextEVSix == size()-1) {
			// f.pln(" $$$$ structRevertVerticalTCP: ERROR EVS cannot be last
			// point! ");
			// }
			NavPoint EVS = point(nextEVSix);
			NavPoint pp = point(ix - 1);
			double vsin = (BVS.z() - pp.z()) / (BVS.time() - pp.time());
			double dt = EVS.time() - BVS.time();
			double tVertex = BVS.time() + dt / 2.0;
			double zVertex = BVS.z() + vsin * dt / 2.0;
			// f.pln(" $$$$ structRevertVsTCP: zVertex =
			// "+Units.str("ft",zVertex));
			Position pVertex = position(tVertex);
			NavPoint vertex = new NavPoint(pVertex.mkAlt(zVertex), tVertex);
			// Note: if the vertical TCP is in a turn then the horizontal
			// position will be different than source
			// if (BVS.hasSource()) {
			// Position sourcePos = BVS.sourcePosition();
			// double distV = sourcePos.distanceV(vertex.position());
			// f.pln(" $$$$ structRevertVsTCP: distV =
			// "+Units.str("ft",distV,8));
			// f.pln(" $$$$ structRevertVsTCP: sourcePos = "+sourcePos+" vertex
			// = "+vertex);
			// }
			remove(nextEVSix);
			remove(ix);
			add(vertex);
			return zVertex;
		}
		return -1;
	}

	/**
	 * Revert all BVS-EVS pairs
	 * 
	 * @param pln  plan file
	 * @return     reverted plan containing no vertical TCPS
	 */
	public void revertVsTCPs() {
		revertVsTCPs(0, size() - 1);
	}

	/**
	 * Revert all BVS-EVS pairs in range "start" to "end"
	 * 
	 * @param pln    plan file
	 * @param start  starting index
	 * @param end    ending index
	 * @return reverted plan containing no vertical TCPS in specified range
	 */
	public void revertVsTCPs(int start, int end) {
		// f.pln(" $$## revertVsTCPs: start = "+start+" end = "+end);
		if (start < 0)
			start = 0;
		if (end > size() - 1)
			end = size() - 1;
		for (int j = end; j >= start; j--) {
			// f.pln(" $$$ REVERT j = "+j+" np_ix = "+point(j));
			structRevertVsTCP(j);
		}
	}

	// will not remove first or last point
	public void removeRedundantPoints(int from, int to) {
		double velEpsilon = 1.0;
		int ixLast = Math.min(size() - 2, to);
		int ixFirst = Math.max(1, from);
		for (int i = ixLast; i >= ixFirst; i--) {
			NavPoint p = point(i);
			Velocity vin = finalVelocity(i - 1);
			Velocity vout = initialVelocity(i);
			if (!p.isTCP() && vin.within_epsilon(vout, velEpsilon)) { // 2.6))  see testAces3, testRandom for worst cases
				// f.pln(" $$$$$ removeRedundantPoints: REMOVE i = "+i);
				remove(i);
			}
		}
	}

	public void removeRedundantPoints() {
		removeRedundantPoints(0, Integer.MAX_VALUE);
	}

	private static final String nl = System.getProperty("line.separator");

	/** String representation of the entire plan */
	public String toString() {
		StringBuffer sb = new StringBuffer(100);
		// sb.append(planTypeName()+" Plan for aircraft: ");
		sb.append(" Plan for aircraft: ");
		sb.append(name);
		if (error.hasMessage()) {
			sb.append(" error.message = " + error.getMessageNoClear());
		}
		sb.append(nl);
		if (note.length() > 0) {
			sb.append("Note=" + note + nl);
		}
		if (size() == 0) {
			sb.append("<empty>");
		} else {
			for (int j = 0; j < size(); j++) {
				sb.append("Waypoint " + j + ": " + point(j).toStringFull());
				sb.append(nl);
			}
		}
		return sb.toString();
	}

	/** String representation of the entire plan */
	private String toString(int velField) {
		StringBuffer sb = new StringBuffer(100);
		sb.append(" Plan for aircraft: ");
		sb.append(name);
		if (error.hasMessage()) {
			sb.append(" error.message = " + error.getMessageNoClear());
		}
		sb.append(nl);
		if (note.length() > 0) {
			sb.append("Note=" + note + nl);
		}
		if (size() == 0) {
			sb.append("<empty>");
		} else {
			for (int j = 0; j < size(); j++) {
				sb.append("Waypoint " + j + ": " + point(j).toString());
				sb.append(" linearIndex = " + point(j).linearIndex());
				if (j < size()) {
					if (velField == 0)
						sb.append("  TRK: " + Units.str("deg", trkOut(j), 4));
					if (velField == 1) {
						if (j > 0)
							sb.append(",  GSin: " + Units.str("kn", gsFinal(j - 1), 4));
						else
							sb.append(",  GSin: -------------");
						if (j < size() - 1)
							sb.append(",  GSout: " + Units.str("kn", gsOut(j), 4));
						else
							sb.append(",  GSout: -------------");
						// sb.append(", GSaccel:
						// "+Units.str("m/s^2",point(j).gsAccel(),4));
					}
					if (velField == 2) {
						if (point(j).isAltPreserve())
							sb.append(", *AltPreserve*");
						sb.append(",  vsOut: " + Units.str("fpm", vsOut(j), 4));
					}
				}
				sb.append(nl);
			}
		}
		return sb.toString();
	}

	/** String representation of the entire plan */
	public String toStringTrk() {
		return toString(0);
	}

	/** String representation of the entire plan */
	public String toStringGs() {
		return toString(1);
	}

	/** String representation of the entire plan */
	public String toStringVs() {
		return toString(2);
	}

	/** String representation of the entire plan */
	public String toStringShort(int precision) {
		StringBuffer sb = new StringBuffer(100);
		// sb.append(planTypeName()+" Plan for aircraft: ");
		sb.append(" Plan for aircraft: ");
		sb.append(name);
		if (error.hasMessage()) {
			sb.append(" error.message = " + error.getMessageNoClear());
		}
		sb.append(nl);
		if (size() == 0) {
			sb.append("<empty>");
		} else {
			for (int j = 0; j < size(); j++) {
				sb.append("Waypoint " + j + ": " + point(j).toString(precision));
				sb.append(" " + point(j).tcpTypeString());
				// if (point(j).isBOT() || point(j).isBGS() || point(j).isBVS())
				// sb.append(" vin = "+point(j).velocityIn());
				if (point(j).isBOT())
					sb.append(" radius = " + Units.str("nm", point(j).turnRadius()));
				if (point(j).isBGS())
					sb.append(" gsAccel = " + Units.str("m/s^2", point(j).gsAccel()));
				if (point(j).isBVS())
					sb.append(" vsAccel = " + Units.str("m/s^2", point(j).vsAccel()));
				sb.append(nl);
			}
		}
		return sb.toString();
	}

	public String listLabels() {
		String rtn = "";
		for (int i = 0; i < size(); i++) {
			String label = point(i).label();
			if (!label.equals("") && !label.contains(specPre)) {
				if (rtn.length() > 0) {
					rtn += ".";
				}
				rtn += label;
			}
		}
		// f.pln(" $$ listLabels: rtn = "+rtn);
		return rtn;
	}

	/**
	 * Returns string that of header information that is compatible with the
	 * file format with header and consistent with a call to toOutput(bool, int,
	 * int). This does not include a terminating newline.
	 */
	public String getOutputHeader(boolean tcpcolumns) {
		String s = "";
		s += "Name, ";
		if (isLatLon()) {
			s += "Lat, Lon, Alt";
		} else {
			s += "SX, SY, SZ";
		}
		s += ", Time, ";
		if (tcpcolumns) {
			s += "type, trk, gs, vs, tcp_trk, accel_trk, tcp_gs, accel_gs, tcp_vs, accel_vs, radius, ";
			if (isLatLon()) {
				s += "src_lat, src_lon, src_alt, ";
			} else {
				s += "src_x, src_y, src_z, ";
			}
			s += "src_time, ";
		}
		s += "Label";
		return s;
	}

	/**
	 * Return a minimal (6 field) string compatable with the reader format. This
	 * works well for linear plans. Values use the default precision. Point
	 * metadata, if present, is collapsed into the label field. Virtual points
	 * are not included
	 * 
	 * @return
	 */
	public String toOutputMin() {
		return toOutput(0, Constants.get_output_precision(), false, false);
	}

	/**
	 * Return a nominal (20-odd field) string compatable with the reader format.
	 * Values use the default precision. Point metadata is printed in distinct
	 * columns. Virtual points are not included
	 * 
	 * @return
	 */
	public String toOutput() {
		return toOutput(0, Constants.get_output_precision(), true, false);
	}

	/**
	 * Output plan data in a manner consistent with the PlanReader input files
	 * 
	 * @param extraspace
	 *            append this number of extra blank columns to the data (used
	 *            for polygons)
	 * @param precision
	 *            precision for numeric data
	 * @param tcpColumns
	 *            if true, include metadata as distinct columns, if false,
	 *            collapse relevant metadata into the label column
	 * @param includeVirtuals
	 *            if true, include points marked as "Virtual" (these are
	 *            generally temporary or redundant points and intended to be
	 *            discarded)
	 * @return
	 */
	public String toOutput(int extraspace, int precision, boolean tcpColumns, boolean includeVirtuals) {
		StringBuffer sb = new StringBuffer(100);
		for (int i = 0; i < size(); i++) {
			if (includeVirtuals || !point(i).isVirtual()) {
				if (name.length() == 0) {
					sb.append("Aircraft");
				} else {
					sb.append(name);
				}
				sb.append(", ");
				sb.append(point(i).toOutput(precision, tcpColumns));
				for (int j = 0; j < extraspace; j++) {
					sb.append(", -");
				}
				sb.append(nl);
			}
		}
		return sb.toString();
	}

	public List<String> toStringList(int i, int precision, boolean tcp) {
		List<String> ret = new ArrayList<String>(NavPoint.TCP_OUTPUT_COLUMNS + 1);
		ret.add(name);
		ret.addAll(point(i).toStringList(precision, tcp));
		return ret;
	}

	/**
	 * This fills in the specified plan with the details provided in the string
	 * as lat/lon points. This is the inverse of toOutput() Note that this is
	 * not sufficient to completely re-create a kinematic plan (it's missing
	 * certain TCP information)
	 */
	public static void parseLL(String s, Plan base) {
		base.clear();
		String[] lines = s.split(nl);
		for (int i = 0; i < lines.length; i++) {
			String[] fields = lines[i].split(Constants.wsPatternParens);
			base.setName(fields[0]);
			String np = fields[1];
			for (int j = 2; j < fields.length; j++) {
				np = np + " " + fields[j];
			}
			base.add(NavPoint.parseLL(np));
		}
	}

	/**
	 * This fills in the specified plan with the details provided in the string
	 * as lat/lon points. This is the inverse of toOutput() Note that this is
	 * not sufficient to completely re-create a kinematic plan (it's missing
	 * certain TCP information)
	 */
	public static void parseXYZ(String s, Plan base) {
		base.clear();
		String[] lines = s.split(nl);
		for (int i = 0; i < lines.length; i++) {
			String[] fields = lines[i].split(Constants.wsPatternParens);
			base.setName(fields[0]);
			base.add(NavPoint.parseXYZ(s));
		}
	}

	//
	// Error logging methods
	//

	public void addWarning(String s) {
		error.addWarning("(" + name + ") " + s);
	}

	public void addError(String s) {
		addError(s, 0);
	}

	public void addError(String s, int loc) {
		error.addError("(" + name + ") " + s);
		if (errorLocation < 0 || loc < errorLocation)
			errorLocation = loc; // save the most recent error location
	}

	public int getErrorLocation() {
		return errorLocation;
	}

	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError();
	}

	public boolean hasMessage() {
		return error.hasMessage();
	}

	public String getMessage() {
		errorLocation = -1;
		return error.getMessage();
	}

	public String getMessageNoClear() {
		return error.getMessageNoClear();
	}
}

// /** Revert all TCPS back to its original linear point which have the same
// sourceTime as the point at index dSeg.
// * If the point is a not a TCP do nothing. Note that this function will
// timeshift the points to
// * regain the original ground speed into the first reverted point of the
// group and all points after it.
// * If checkSource is true, this function checks to make sure that the
// source position is reasonably close to the
// * current position, and if not, it reverts to the current position of
// "dSeg".
// *
// * Note. This method restores the sourcePosition, but essentially ignores
// the sourceTimes
// *
// * @param dSeg The index of one of the TCPs created together that should
// be reverted
// * @return index of the point that replaces all the other points
// */
// //@Deprecated
// public int revertGroupOfTCPs(int dSeg, boolean checkSource) {
// //f.pln(" $$$ revertGroupOfTCPs: BEFORE dSeg = "+dSeg);
// //PlanUtil.savePlan(this, "1dump_revertGroupOfTCPs");
// final double maxDistH = Units.from("NM", 15.0);
// final double maxDistV = Units.from("ft", 5000.0);
// if (dSeg < 0 || dSeg >= size()) {
// addError(".. revertGroupOfTCPs: invalid index "+dSeg, 0);
// return -1;
// }
// NavPoint origDsegPt = point(dSeg);
// if (!origDsegPt.isTCP()) {
// //f.pln(" $$ revertGroupOfTCPs: point "+dSeg+" is not a TCP, do
// nothing!");
// return dSeg;
// }
// double sourceTm = origDsegPt.sourceTime();
// //int dSeg = getSegment(sourceTm);
// //f.pln("\n $$$ revertGroupOfTCPs: point(dSeg).time =
// "+point(dSeg).time() +" sourceTm = "+sourceTm);
// int firstInGroup = -1; // index of first TCP in the group
// int lastInGroup = size()-1; // index of the last TCP in the group
// for (int j = 0; j < size(); j++) {
// if (Constants.almost_equals_time(point(j).sourceTime(),sourceTm)) {
// if (firstInGroup == -1) firstInGroup = j;
// lastInGroup = j;
// }
// }
// // save speed into the first point of the group to be reverted
// double gsInFirst = finalVelocity(firstInGroup-1).gs();
// int nextSeg = lastInGroup+1;
// double tmNextSeg = getTime(nextSeg);
// double gsInNext = finalVelocity(nextSeg-1).gs();
// //f.pln(" $$$$ revertGroupOfTCPs: point(dSeg) =
// "+point(dSeg).toStringFull()+" "+point(dSeg).hasSource());
// NavPoint revertedLinearPt = new
// NavPoint(point(dSeg).sourcePosition(),point(dSeg).sourceTime());
// // remove kinematic points from lastInGroup to firstInGroup in reverse
// order
// int lastii = -1;
// for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
// if (Constants.almost_equals_time(point(ii).sourceTime(),sourceTm)) {
// //f.pln(" $$$ remove point ii = "+ii+" point(i).time() =
// "+point(ii).time()+" point(i).tcpSourceTime() =
// "+point(ii).tcpSourceTime());
// if (point(ii).hasSource()) {
// revertedLinearPt = new
// NavPoint(point(ii).sourcePosition(),point(ii).sourceTime());
// //f.pln(" $$$$ ii = "+ii+" revertGroupOfTCPs: revertedLinearPt =
// "+revertedLinearPt.toStringFull());
// }
// remove(ii);
// lastii = ii;
// }
// }
// // safety check in case there is a invalid source position
// double distH = origDsegPt.distanceH(revertedLinearPt);
// double distV = origDsegPt.distanceV(revertedLinearPt);
// if (checkSource && (distH > maxDistH || distV > maxDistV)) {
// //f.pln(" $$$$ revertGroupOfTCPs: for dSeg = "+dSeg+" distH =
// "+Units.str("nm", distH));
// //f.pln(" $$$$ revertGroupOfTCPs: for dSeg = "+dSeg+" distV =
// "+Units.str("ft", distV));
// revertedLinearPt = origDsegPt.makeNewPoint();
// //f.pln(" $$$$ revertGroupOfTCPs: origDsegPt =
// "+origDsegPt.toStringFull());
// }
// //f.pln(" $$$$ revertGroupOfTCPs: revertedLinearPt =
// "+revertedLinearPt.toStringFull());
// // ------- add the reverted point ------------------------
// add(revertedLinearPt);
// // timeshift points to restore time into nextSeg
// if (tmNextSeg > 0) { // if reverted last point, no need to timeshift
// points after dSeg
// int newNextSeg = getSegment(tmNextSeg);
// double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
// double dt2 = newNextSegTm - tmNextSeg;
// timeshiftPlan(newNextSeg, dt2);
// }
// // timeshift plan to regain original ground speed into the first reverted
// point of the group
// if (firstInGroup < lastInGroup) {
// int segNewLinearPt = getSegment(revertedLinearPt.time());
// double newTm = linearCalcTimeGSin(segNewLinearPt, gsInFirst);
// double dt = newTm - revertedLinearPt.time();
// //f.pln(" $$$ revertGroupOfTCPs: TIMESHIFT dt = "+dt);
// timeshiftPlan(segNewLinearPt, dt);
// }
// //f.pln(" $$$ revertGroupOfTCPs: lastii = "+lastii);
// return lastii;
// }


/*
 * Plan -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * Authors: George Hagen,  Jeff Maddalon,  Rick Butler
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;




//import gov.nasa.larcfm.IO.DebugSupport;


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
	private BoundingRectangle bound; // TODO: this bound only applies to added points, when points are deleted, this
	// is not accommodated, or if points are updated, this is not accommodated
	private static boolean debug = false;
	protected String note = "";
	public static final double minDt = GreatCircle.minDt; // points that are closer together in time than this are treated as
	public static final double nearlyZeroGs = 1E-10;;
	// the same point for velocity calculations, mergeClosePoints merges points
	// closer together in time than this parameter.
	public static final String specPre = "$";             // special prefix for labels

	public static final int TCP_OUTPUT_COLUMNS = 20; // total number of output columns for full TCP not including aircraft name)
	public static final int MIN_OUTPUT_COLUMNS = 5; // total number of output columns for linear (not including aircraft name)

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

	/** initialize class variables */
	private void init() {
		points = new ArrayList<NavPoint>(100);
		data = new ArrayList<TcpData>(100);
		error = new ErrorLog("Plan");
		error.setConsoleOutput(debug); // debug ON!
		errorLocation = -1;
		// deletedPoints = new ArrayList<NavPoint>();
		bound = new BoundingRectangle();
	}

	/** Construct a new object that is a deep copy of the supplied object 
	 * 
	 * @param fp plan to copy
	 */
	public Plan(Plan fp) {
		points = new ArrayList<NavPoint>(100);
		data = new ArrayList<TcpData>(100);
		// deletedPoints = new ArrayList<NavPoint>(fp.deletedPoints);
		for (int j = 0; j < fp.points.size(); j++) {
			points.add(fp.points.get(j));
			data.add(fp.getTcpData(j)); // making a copy of TcpData
		}
		name = fp.name;
		note = fp.note;
		error = new ErrorLog(fp.error);
		errorLocation = fp.errorLocation;
		bound = new BoundingRectangle(fp.bound);
	}

	boolean validIndex(int i) {
		if (i >= 0 && i < size()) return true;
		else return false;
	}

	/** search plan to see if there are any TCPs
	 * 
	 * @return  true if the plan does not contain any TCPs 
	 */
	public boolean isLinear() {
		for (int j = 0; j < points.size(); j++) {
			boolean isTcp = isTCP(j);
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
			//f.pln(" $$$copyWithIndex: point(j) = "+point(j));
			lpc.add(point(j), getTcpData(j).setSource(point(j)).setLinearIndex(j));  
		}
		//f.pln(" $$$copyWithIndex: EXIT, lpc = "+lpc.toStringFull());
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
					else if (!isBeginTCP(i) && isBeginTCP(i+1))
						ixDelete = i;
					// save attributes of "ixDelete"
					NavPoint npDelete2 = point(ixDelete);
					TcpData npDelete = getTcpData(ixDelete);
					remove(ixDelete);

					// the index of the remaining point is "i"
					//NavPoint newNpi = point(i).mergeTCPInfo(npDelete);
					TcpData newData = getTcpData(i).mergeTCPInfo(npDelete).appendInformation(npDelete.getInformation());
					NavPoint newNpi = point(i).appendLabel(npDelete2.label());
					set(i, newNpi, newData);
				}
			}
		}
	}
	
	public void mergeClosePoints() {
		mergeClosePoints(minDt); 
	}

	/**
	 * Find the index in the kinematic plan that corresponds to the n-th
	 * "instance" of linear index "ix"
	 * 
	 * @param ix        linear index
	 * @return kinematic index
	 */
	public ArrayList<Integer> findLinearIndex(int ix) {
		ArrayList<Integer> al = new ArrayList<Integer>();
		for (int i = 0; i < size(); i++) {
			int linearIx = getTcpData(i).getLinearIndex();
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
	 * @return new cut down plan
	 */
	public Plan cut(int firstIx, int lastIx) {
		Plan lpc = new Plan(getName(), getNote());
		for (int i = firstIx; i <= lastIx; i++) {
			Pair<NavPoint,TcpData> np = get(i);			
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
			rtn.add(pEnd.get(j));
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
	 * @return new Plan, with a Fixed starting point. If endTime &lt;= startTime, returns an empty Plan.
	 */
	static public Plan planFromState(String id, Position pos, Velocity v, double startTime, double endTime) {
		Plan p = new Plan(id);
		if (endTime <= startTime) {
			return p;
		}
		NavPoint np = new NavPoint(pos, startTime);
		p.addNavPoint(np);
		p.addNavPoint(np.linear(v, endTime - startTime));
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
				//f.pln("almostEquals: point i = " + i + " does not match: " + point(i) + "  !=   " + p.point(i));
			}
			if (Math.abs(time(i) - p.time(i)) > 0.0000001) {
				rtn = false;
				//f.pln("almostEquals: time i = " + i + " does not match: " + time(i) + "  !=   " + p.time(i));
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
	 * @return true if almost equal in location and time
	 */
	public boolean almostEquals(Plan p, double epsilon_horiz, double epsilon_vert) {
		boolean rtn = true;
		for (int i = 0; i < size(); i++) { // Unchanged
			if (!point(i).almostEqualsPosition(p.point(i), epsilon_horiz, epsilon_vert)) {
				rtn = false;
				//f.pln("almostEquals: point i = " + i + " does not match: " + point(i) + "  !=   " + p.point(i)
				//+ " epsilon_horiz = " + epsilon_horiz);
			}
			if (!point(i).label().equals(p.point(i).label())) {
				//f.pln("almostEquals: point i = " + i + " labels do not match.");
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
	public double time(int i) {
		if (i < 0 || i >= size()) {
			addError("time: invalid point index of " + i + " sz=" + size(), i);
			// f.pln(" $$!! invalid point index of "+i+" in getTime()");
			//Debug.halt();
			return 0.0;
		}
		return points.get(i).time();
	}

	public Position getPos(int i) {
		return point(i).position();
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
			//f.pln("$$$ Plan.point: invalid index "+i);
			//Debug.halt();
			if (isLatLon()) {
				return NavPoint.ZERO_LL;
			} else {
				return NavPoint.ZERO_XYZ;
			}
		}
		return points.get(i);
	}

	private TcpData getTcpDataRef(int i) {
		if (i < 0 || i >= data.size()) {
			addError("getTcpDataRef.point: invalid index " + i, i);
			f.pln(" $$ Plan.getTcpDataRef.point: invalid index " + i);
			//Debug.halt();
			return TcpData.makeInvalid();
		}
		return data.get(i);
	}

	public TcpData getTcpData(int i) {
		return new TcpData(getTcpDataRef(i));
	}

	public Pair<NavPoint,TcpData> get(int i) {
		return new Pair<NavPoint,TcpData>(point(i) ,getTcpData(i));
	}


	/** Returns true if the point at index i is an unmodified original point, 
	   false otherwise.  0 &lt;= i &lt; size() */

	public boolean  isOriginal(int i) { return getTcpData(i).isOriginal();}	
	public boolean  isAltPreserve(int i) { return getTcpData(i).isAltPreserve();}	

	public double  signedRadius(int i) {return getTcpData(i).signedRadius(); }
	public double  turnRadius(int i) { return getTcpData(i).turnRadius();}	
	public boolean isVirtual(int i)  { return getTcpData(i).isVirtual();}
	public boolean hasSource(int i)  { return getTcpData(i).hasSource();}


	public double  trkAccel(int i) {
		//return accel_trk;
		double rtn = 0.0;
		if (Math.abs(signedRadius(i)) > 0) {
			rtn = gsOut(i)/signedRadius(i);
		} 
		//f.pln(" $$$ trkAccel: radiusSigned = "+Units.str("NM",radiusSigned)+" rtn = "+Units.str("deg/s",rtn));
		return rtn;
	}

	public double  gsAccel(int i) {return getTcpData(i).gsAccel(); }
	public double  vsAccel(int i) {return getTcpData(i).vsAccel(); }

	//	public Velocity  velocityInit(int i) {return getTcpData(i).velocityInit(); }


	/** true if this point is the start of a turn */
	public boolean isTrkTCP(int i) {return getTcpData(i).isTrkTCP(); }	
	/** true if this point is the start of a turn */
	public boolean isBOT(int i)   {return getTcpData(i).isBOT(); }
	public boolean isEOT(int i)   {return getTcpData(i).isEOT(); }
	public boolean isGsTCP(int i) {return getTcpData(i).isGsTCP(); }
	public boolean isBGS(int i)   {return getTcpData(i).isBGS(); }
	public boolean isEGS(int i)   {return getTcpData(i).isEGS(); }
	public boolean isVsTCP(int i) {return getTcpData(i).isVsTCP(); }
	public boolean isBVS(int i)   {return getTcpData(i).isBVS(); }
	public boolean isEVS(int i)   {return getTcpData(i).isEVS(); }
	public boolean isBeginTCP(int i) {return getTcpData(i).isBeginTCP(); }
	public boolean isEndTCP(int i)   {return getTcpData(i).isEndTCP(); }

	public boolean isTCP(int i)  { return getTcpData(i).isTCP();}

	public NavPoint sourceNavPoint(int i) {
		return new NavPoint(getTcpData(i).getSourcePosition(), getTcpData(i).getSourceTime());
	}

	//	public static Pair<NavPoint,TcpData> makeTrkAccel(NavPoint src, TcpData srcTCP, double omega) {
	//		double radius = srcTCP.velocityInit().gs()/omega;	
	//		//f.pln(" $$$ makeTrkAccel: velocityIn = "+this.velocityIn+" radius = "+radius);
	//		//this.radiusSigned, this.accel_gs, a, velocityIn,  this.sourcePosition, this.sourceTime, linearIndex);
	//        TcpData tcp = srcTCP.setRadiusSigned(radius);
	//        return  new Pair<NavPoint,TcpData>(src,tcp);
	//	}

	/** Make a new NavPoint from the current one with the acceleration/turn rate metadata changed */
	public static Pair<NavPoint,TcpData> makeGsAccel(NavPoint src, TcpData srcTCP, double ga) {
		TcpData tcp = srcTCP.setGsAccel(ga);
		return  new Pair<NavPoint,TcpData>(src,tcp);

	}

	/** Make a new NavPoint from the current one with the acceleration/turn rate metadata changed */
	public static Pair<NavPoint,TcpData> makeVsAccel(NavPoint src, TcpData srcTCP, double va) {
		TcpData tcp = srcTCP.setVsAccel(va);
		return  new Pair<NavPoint,TcpData>(src,tcp);
	}

	/** Make a new NavPoint from the current that is "Original" */
	public static Pair<NavPoint,TcpData> makeOriginal(Pair<NavPoint,TcpData> src) {
		TcpData tcp = src.second.copy().setOriginal();    // copy is necessary 
		return new Pair<NavPoint,TcpData>(src.first,tcp);		
	}

	/** Make a new NavPoint from the current that is "AltPreserve" */
	public static Pair<NavPoint,TcpData> makeAltPreserve(Pair<NavPoint,TcpData> src) {
		TcpData tcp = src.second.copy().setAltPreserve();    // copy is necessary 
		return new Pair<NavPoint,TcpData>(src.first,tcp);		
	}


	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeBOT(NavPoint src, Position p, double t,  double signedRadius, Position center, int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBOT t = "+t+"   center = "+center);  
		NavPoint np = new NavPoint(p,t).makeLabel(src.label()); 
		TcpData tcp = TcpData.makeSource(src).setBOT( signedRadius, center, linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}

	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeEOT(NavPoint src, Position p, double t,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeEOT t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t);
		TcpData tcp = TcpData.makeSource(src).setEOT(linearIndex);
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}

	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeEOTBOT(NavPoint src, Position p, double t,   double signedRadius, Position center, int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBOT t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t);
		TcpData tcp = TcpData.makeSource(src).setEOTBOT( signedRadius, center, linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}


	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeBGS(NavPoint src, Position p, double t, double a,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBGS t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t).makeLabel(src.label()); 
		TcpData tcp = TcpData.makeSource(src).setBGS(a,linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}


	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeEGS(NavPoint src, Position p, double t,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeEGS t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t); 
		TcpData tcp = TcpData.makeSource(src).setEGS(linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}

	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeEGSBGS(NavPoint src, Position p, double t, double a,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBGS t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t); 
		TcpData tcp = TcpData.makeSource(src).setEGSBGS(a,linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}


	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeBVS(NavPoint src, Position p, double t, double a,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t).makeLabel(src.label()); 
		TcpData tcp = TcpData.makeSource(src).setBVS(a,linearIndex);
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}

	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeEVS(NavPoint src, Position p, double t,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeEVS t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t);
		TcpData tcp = TcpData.makeSource(src).setEVS(linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}

	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public static Pair<NavPoint,TcpData> makeEVSBVS(NavPoint src, Position p, double t, double a,  int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);  
		NavPoint np = new NavPoint(p,t); 
		TcpData tcp = TcpData.makeSource(src).setEVSBVS(a,linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}

	/** Makes a new NavPoint that is an intermediate "mid" added point */
	public static Pair<NavPoint,TcpData> makeMidpoint(NavPoint src, TcpData tcp, Position p, double t, int linearIndex) {
		NavPoint np = new NavPoint(p,t); 
		tcp = tcp.copy().setSource(src).setLinearIndex(linearIndex); 
		return new Pair<NavPoint,TcpData>(np,tcp);		
	}


	//	/** Make a new NavPoint from the current one with the source time metadata changed */
	//	public NavPoint makeSource(Position sp, double st) {
	//		//		if (st == this.sourceTime && sp.equals(this.sourcePosition)) return this;
	//		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
	//				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, sp, st, this.linearIndex);
	//	}


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
			if (isVirtual(i)) {
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

	public boolean isTimeInPlan(double t) {
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
			if (!isVirtual(i))
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
			if (np.time() >= t && TcpData.fullLabel(np,getTcpDataRef(i)).contains(str))
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
	public int findLabel(String name, int startIx, boolean withWrap) {
		if (startIx >= points.size()) {
			if (withWrap)
				startIx = 0;
			else
				return -1;
		}
		for (int i = startIx; i < points.size(); i++) {
			NavPoint np = points.get(i);
			if (np.label().equals(name))
				return i;
		}
		if (withWrap) {
			for (int i = 0; i < startIx; i++) {
				NavPoint np = points.get(i);
				if (np.label().equals(name))
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
	
	public void clearLabel(int ix) { //TODO rename clearPointName
		NavPoint np = point(ix);
		TcpData  tcp = getTcpDataRef(ix);
		set(ix,np.makeLabel(""),tcp);
	}
	
	public String label(int ix) { //TODO rename to pointName (confused with plan name)
		return point(ix).label();
	}

	public int findInfo(String info, int startIx, boolean withWrap) {
		if (startIx >= points.size()) {
			if (withWrap)
				startIx = 0;
			else
				return -1;
		}
		for (int i = startIx; i < points.size(); i++) {
			TcpData tcp = getTcpDataRef(i);
			if (tcp.getInformation().equals(info))
				return i;
		}
		if (withWrap) {
			for (int i = 0; i < startIx; i++) {
				TcpData tcp = getTcpDataRef(i);
				if (tcp.getInformation().equals(info))
					return i;
			}
		}
		return -1;
	}


	public int findInfo(String info) {
		for (int i = 0; i < points.size(); i++) {
			TcpData tcp = getTcpDataRef(i);
			if (tcp.getInformation().equals(info))
				return i;
		}
		return -1;
	}
	
	public void clearInfo(int ix) {
		NavPoint np = point(ix);
		TcpData  tcp = getTcpDataRef(ix);
		set(ix,np,tcp.setInformation(""));
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
	 * Return segment that is distance "d" from segment startIx
	 * 
	 * @param d    distance of interest
	 * @return     segment number
	 */
	public int getSegmentByDistance(int startIx, double d) {
		if (d < 0)
			return -1;
		double tdist = 0;
		int i = startIx;
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
	
	/**
	 * Return segment that is distance "d" from beginning of plan
	 * 
	 * @param d    distance of interest
	 * @return     segment number
	 */

	public int getSegmentByDistance(double d) {
		return getSegmentByDistance(0,d);
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
		return data.get(i).getInformation();
	}

	public void setInfo(int i, String info) {
		if (i < 0 || i >= size()) {
			addError("setInfo: invalid point index of " + i + " size=" + size());
		}
		TcpData d = data.get(i);
		d.setInformation(info);
		data.set(i, d);
	}

	public void setVirtual(int i) {
		if (i < 0 || i >= size()) {
			addError("setVirtual: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setVirtual();  //setType(TcpData.WayType.Virtual);
		data.set(i, d);
	}


	/** Returns true if the point at index i is an unmodified original point,
	   false otherwise.  0 &lt;= i &lt; size() */
	public void setOriginal(int i) {
		if (i < 0 || i >= size()) {
			addError("setOriginal: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setOriginal(); //setType(TcpData.WayType.Orig);
		data.set(i, d);
	}

	/** AltPreserve is used by TrajGen.generateVsTCPs to create a vertical profile that is a function
	 *  of locations (rather than time which is very fluid).  The TrajGen method "markVsChanges" marks points
	 *  with a significant vertical speed change as "AltPreserve".
	 * 
	 * @param i    index of point set as "AltPReserve"
	 */
	public void setAltPreserve(int i) {
		if (i < 0 || i >= size()) {
			addError("setAltPreserve: invalid point index of "+i+" size="+size());
			f.pln("$$ setAltPreserve: invalid point index of i = "+i);
		}	
		TcpData d = data.get(i);
		d.setAltPreserve(); //setType(TcpData.WayType.AltPreserve);
		data.set(i, d);
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
		d.setRadiusSigned(radius);
		data.set(i, d);
	}

	//	/**
	//	 * This method returns a center of turn position with the same altitude as
	//	 * the current point. If the current point is not a turn point, and has a
	//	 * zero stored radius, this returns an invalid position.
	//	 */
	//	public Position turnCenter(int i) {
	//		if (i < 0 || i >= size()) {
	//			addError("turnCenter: invalid point index of " + i + " size=" + size());
	//			return Position.INVALID;
	//		}
	//		double R = signedRadius(i);
	//		if (R != 0) {
	//			Velocity vHat = data.get(i).getVelocityInit().mkAddTrk(Util.sign(R) * Math.PI / 2).Hat2D();
	//			Position pos = point(i).position();
	//			return pos.linear(vHat, Math.abs(R)).mkZ(pos.z());
	//		}
	//		return Position.INVALID;
	//	}


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
		Position center = getTcpData(i).turnCenter();
		//f.pln(" $$ turnCenter: center = "+center);
		return center;
	}

	/** Source time of point this was based on for any type except BVS. */
	// @Deprecated
	public double sourceTime(int i) {
		if (i < 0 || i >= size()) {
			addError("sourceTime: invalid point index of " + i + " size=" + size());
			return 0.0;
		}
		return data.get(i).getSourceTime();
	}

	// @Deprecated
	public void setSourceTime(int i, double time) {
		if (i < 0 || i >= size()) {
			addError("setSourceTime: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setSourceTime(time);
		data.set(i, d);
	}



	/** Source time of point this was based on for any type except BVS. */
	// @Deprecated
	public Position sourcePosition(int i) {
		if (i < 0 || i >= size()) {
			addError("sourcePosition: invalid point index of " + i + " size=" + size());
			return Position.INVALID;
		}
		return data.get(i).getSourcePosition();
	}

	// @Deprecated
	public void setSourcePosition(int i, Position pos) {
		if (i < 0 || i >= size()) {
			addError("setSourcePosition: invalid point index of "+i+" size="+size());
			return;
		}
		TcpData d = data.get(i);
		d.setSourcePosition(pos);
		//f.pln(" $$$$ setSourcePosition: d = "+d);
		data.set(i, d);
	}

	public void resetSource(int i) {
		setSource(i,point(i));
	}

	public void setSource(int i, NavPoint npi) {
		setSourceTime(i,npi.time());
		setSourcePosition(i,npi.position());
	}

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
		return data.get(i).getLinearIndex();
	}

	public void setLinearIndex(int i, int index) {
		if (i < 0 || i >= size()) {
			addError("setLinearIndex: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setLinearIndex(index);
		//data.set(i, d);
	}




	//	/** Set the "velocityInit" field to "vel" fro point "i"
	//	 * 
	//	 * @param i
	//	 * @param vel
	//	 */
	//	public void setVelocityInit(int i, Velocity vel) {
	//		if (i < 0 || i >= size()) {
	//			addError("setVelocityInit: invalid point index of "+i+" size="+size());
	//		}	
	//		TcpData d = data.get(i);
	//		//d.setVelocityInit(vel);
	//		data.set(i, d);
	//	}

	/** Set the "velocityInit" field to "vel" fro point "i"
	 *
	 * @param i
	 * @param vel
	 */
	public void setGsAccel(int i, double accel) {
		if (i < 0 || i >= size()) {
			addError("setGsAccel: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setGsAccel(accel);
		data.set(i, d);
	}


	public void setVsAccel(int i, double accel) {
		if (i < 0 || i >= size()) {
			addError("setVsAccel: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setVsAccel(accel);
		//data.set(i, d);
	}

	public void setBOT(int i, double signedRadius, Position center, int linearIndex) {
		if (i < 0 || i >= size()) {
			addError("setBOT: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setBOT(signedRadius, center, linearIndex);
		//data.set(i, d);
	}

	public void setEOT(int i, int idx) {
		if (i < 0 || i >= size()) {
			addError("setEOT: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setEOT(idx);
		//data.set(i, d);
	}

	public void setEOTBOT(int i, double signedRadius, Position center, int linearIndex) {
		if (i < 0 || i >= size()) {
			addError("setEOTBOT: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setEOTBOT(signedRadius, center, linearIndex);
		//data.set(i, d);
	}

	public void setBGS(int i, double acc, Velocity v, int idx) {
		if (i < 0 || i >= size()) {
			addError("setBGS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setBGS(acc, idx);
		//data.set(i, d);
	}

	public void setEGS(int i, Velocity v, int idx) {
		if (i < 0 || i >= size()) {
			addError("setEGS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setEGS(idx);
		//data.set(i, d);
	}

	public void setEGSBGS(int i, double acc, Velocity v, int idx) {
		if (i < 0 || i >= size()) {
			addError("setEGSBGS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setEGSBGS(acc, idx);
		//data.set(i, d);
	}

	public void setBVS(int i, double acc, Velocity v, int idx) {
		if (i < 0 || i >= size()) {
			addError("setBVS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setBVS(acc, idx);
		//data.set(i, d);
	}

	public void setEVS(int i, Velocity v, int idx) {
		if (i < 0 || i >= size()) {
			addError("setEVS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setEVS(i);
		//data.set(i, d);
	}

	public void setEVSBVS(int i, double acc, Velocity v, int idx) {
		if (i < 0 || i >= size()) {
			addError("setEVSBVS: invalid point index of "+i+" size="+size());
		}	
		TcpData d = data.get(i);
		d.setEVSBVS(acc, idx);
		//data.set(i, d);
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
			if (isEGS(j)) {
				return j;
			}
		}
		return -1;
	}

	public int nextBeginTCP(int current) { // fixed
		if (current < 0 || current > size() - 1) {
			addWarning("nextBGS invalid starting index " + current);
			return -1;
		}
		for (int j = current + 1; j < points.size(); j++) {
			if (isBeginTCP(j)) {
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
			if (isBGS(j)) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the ground speed end point less than or equal to the given index,
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
			if (isEGS(j)) {
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
			if (isEVS(j)) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the vertical speed end point less than or equal to the given
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
			if (isEVS(j)) {
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
			if (isBVS(j)) {
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
			if (isBOT(j)) {
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
			if (isBOT(j)) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the End of Turn point greater than the given index, or -1
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
			if (isEOT(j)) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the turn end point less than the given index, or -1 if
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
			if (isEOT(j)) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Vertical Speed Change Begin point less than the
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
			if (isBVS(j)) {
				return j;
			}
		}
		return -1;
	}

	/**
	 * This returns the index of the Ground Speed Change Begin point less than the given
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
			if (isBGS(j)) {
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
			if (isTrkTCP(j) || isGsTCP(j) || isVsTCP(j)) {
				return j;
			}
		}
		return -1;
	}

	//
	// Point addition and set methods
	//

	/** Add a Navpoint to the plan that does not have any associated TCP information.  Once this point is added to the plan a set of default
	 * TCP information will be used for this point.
	 * 
	 * @param p the point to add
	 * @return the (non-negative) index of the point if the point is added without problems, otherwise it will return a negative value.
	 */
	public int addNavPoint(NavPoint p) {
		TcpData d = new TcpData();
		d.setSource(p); // this better matches the semantics of the original NavPoint (with internal TcpData)
		return add(p, d);
	}

	public int add(Position p, double time) {
		NavPoint np = new NavPoint(p,time);
		return addNavPoint(np);
	}
	
	public int add(Position p, double time, String nm) {
		NavPoint np = new NavPoint(p,time,nm);
		return addNavPoint(np);
	}



	public int add(Pair<NavPoint,TcpData> p) {
		return add(p.first, p.second);
	}

	/**
	 * 
	 * @param p
	 * @param tcpdata
	 * @return
	 */
	public int add(NavPoint p, TcpData tcpdata) {
		TcpData d;
		if (tcpdata == null) {
			d = new TcpData(); // use default values
		} else {
			d = new TcpData(tcpdata);
		}

		if (p == null) {
			addError("add: Attempt to add null NavPoint", 0);
			return -1;
		}
		if (p.isInvalid()) {
			addError("add: Attempt to add invalid NavPoint", 0);
			f.pln(" Plan.add(" + name + "): error invalid NavPoint p = " + p.toString());
			Debug.halt();
			return -1;
		}
		if (Double.isInfinite(p.time())) {
			addError("add: Attempt to add NavPoint with infinite time", 0);
			f.pln(" Plan.add(" + name + "): error infinite NavPoint p = " + p.toString());
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
			//f.pln(" $$$ Plan.add: getTcpData(i).mergeable(d) = "+getTcpData(i).mergeable(d));
			if (!isVirtual(i)) {
				if (getTcpData(i).mergeable(d)) {
					NavPoint np2 = point(i).appendLabel(p.label());
					TcpData np = getTcpData(i).mergeTCPInfo(d).appendInformation(tcpdata.getInformation());
					//f.pln(" $$$ Plan.add: np = "+np);
					points.set(i, np2);
					data.set(i, np);
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
				//f.pln(" $$ NavPoint.add: set at i = "+i+" p = "+p);
				points.set(i, p);
				data.set(i, d);
			}
		} else {
			// insert
			i = -i - 1; // where the point should be inserted
			//f.pln(" $$$ Plan.add: insert i = "+i+" p = "+p+" d = "+d.toString());
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
	 * @param i the index to place the new point
	 * @param v the new NavPoint
	 * @param d the new TcpData associated with this NavPoint
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
		int rtn = addNavPoint(np);
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
	public void removeByName(String name, int startIx) {
		boolean withWrap = false;
		int ix = findLabel(name, startIx, withWrap);
		if (ix >= 0) {
			remove(ix);
		}
	}

	public void removeByInfo(String info, int startIx) {
		boolean withWrap = false;
		int ix = findInfo(info, startIx, withWrap);
		if (ix >= 0) {
			remove(ix);
		}
	}

	/**
	 * Attempt to replace the i-th point with the given NavPoint. If successful,
	 * this returns the index of the new point (which may change because of
	 * a new time relative to the old point). This method
	 * returns an error if the given index is out of bounds or a warning if the
	 * new point overlaps with (and replaces) a different existing point.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new point to replace it with
	 * @param d the new TcpData to replace
	 * @return the actual index of the new point
	 */
	public int set(int i, NavPoint v, TcpData d) {
		if (i < 0 || i >= points.size()) {
			addError("set: invalid index " + i, i);
			return -1;
		}
		// f.pln(" $$ Plan.set: i = "+i+" v = "+v+" toString = "+toString());
		remove(i);
		return add(v, d);
	}

	/**
	 * Attempt to replace the i-th point with the given NavPoint/TcpData. If successful,
	 * this returns the index of the new point (which may change because of
	 * a new time relative to the old point). This method
	 * returns an error if the given index is out of bounds or a warning if the
	 * new point overlaps with (and replaces) a different existing point.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param npt  the new NavPoint and TcpData to replace it with
	 * @return the actual index of the new point
	 */
	public int set(int i, Pair<NavPoint,TcpData> npt) {
		return set(i, npt.first, npt.second);
	}

	/**
	 * Attempt to replace the i-th point with the given NavPoint. The TcpData from the 
	 * old point is retained.  If successful,
	 * this returns the index of the new point (which may change because of
	 * a new time relative to the old point). This method
	 * returns -1 and sets an error if the given index is out of bounds or a warning if the
	 * new point overlaps with (and replaces) a different existing point.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new NavPoint to replace it with
	 * @return the actual index of the new point
	 */
	public int setNavPoint(int i, NavPoint v) {
		if (i < 0 || i >= points.size()) {
			addError("setNavPoint: invalid index " + i, i);
			return -1;
		}
		TcpData tcp = getTcpData(i);
		remove(i);
		return add(v,tcp);
	}

	/**
	 * Attempt to replace the i-th point's tcp data  with the given data. The navpoint from the 
	 * old point is retained.  If successful,
	 * this returns the index of the new point. This method
	 * returns -1 and sets an error if the given index is out of bounds.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new TcpData to replace it with
	 * @return the actual index of the new point
	 */
	public int setTcpData(int i, TcpData v) {
		if (i < 0 || i >= points.size()) {
			addError("setTcpData: invalid index " + i, i);
			return -1;
		}
		NavPoint np = point(i);
		remove(i);
		return add(np, v);
	}

	/**
	 * Set the time of the given point to the given value.  Note
	 * the index of the point may change due to a new time.
	 * 
	 * @param i the index of the point to change
	 * @param t the new time
	 */
	public void setTime(int i, double t) {
		if (t < 0) {
			addError("setTime: invalid time " + f.Fm4(t), i);
			return;
		}
		NavPoint tempv = points.get(i).makeTime(t);
		TcpData  tcp = getTcpData(i);
		set(i, tempv, tcp);
	}

	/** Set altitude of point "i" to "alt"
	 * 
	 */
	public void setAlt(int i, double alt) {
		NavPoint tempv = points.get(i).mkAlt(alt);
		setNavPoint(i, tempv);
	}
	

	// currently only used in "TurnPlanTest"
	public void updateWithDefaultRadius(double radius) {
		for (int i = 0; i < points.size(); i++) {
			TcpData d = getTcpDataRef(i);
			if ((d.isBOT() || ! d.isTCP()) && d.turnRadius() == 0.0) {
				NavPoint np = point(i);
				//f.pln("update radius");
				TcpData tcp = TcpData.makeSource(np).setRadiusSigned(radius);
				set(i, np, tcp);
				d.setRadiusSigned(radius);				
				//data.set(i, d);
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
			newKPC.add(get(j));
			// f.pln("0 add newKPC.point("+j+") = "+newKPC.point(j));

		}
		double ft = 0.0; // time of point before start
		if (start > 0) {
			ft = this.time(start - 1);
		}
		for (int i = start; i < this.size(); i++) {
			double t = this.time(i) + st; // adjusted time for this point
			// f.pln(">>>> timeshiftPlan: t = "+ t+" ft = "+ft);
			if (t > ft && t >= 0.0) {
				// double newSourceTime = this.point(i).sourceTime() + st;
				Pair<NavPoint, TcpData> pair = get(i);
				newKPC.add(pair.first.makeTime(t),pair.second);
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
	public boolean timeShiftPlan(int start_ix, double dt) {
		if (Double.isNaN(dt) || start_ix < 0) {
			if (debug) Debug.halt();
			return false;
		}
		if (start_ix >= size() || dt == 0.0)
			return true;
		if (dt < 0) {
			double ft = 0.0; // time of point before start
			if (start_ix > 0) {
				ft = time(start_ix - 1);
			}
			for (int i = start_ix; i < size(); i++) {
				double t = time(i) + dt; // adjusted time for this point
				// f.pln(">>>> timeshiftPlan: t = "+t+" getTime(i) =
				// "+getTime(i)+" getTime(i-1) = "+getTime(i-1));
				if (t > ft && t >= 0.0) {
					// f.pln(">>>> timeshiftPlan: setTime(i,t)! i = "+i+" t =  "+t);
					setTime(i, t);
				} else {
					//f.pln(">>>> timeshiftPlan: remove i = "+i);
					remove(i);
					i--;
				}
			}
		} else {
			for (int i = size() - 1; i >= start_ix; i--) {
				// f.pln(">>>> timeshiftPlan: setTime(i,getTime(i)+st! i = "+i+"  st = "+st);
				setTime(i, time(i) + dt);
			}
		}
		return true;
	}

	/**
	 * This returns true if the given time is greater than or equal to a BOT but before an EOT point
	 */
	public boolean inTrkChange(double t) { // fixed
		int i = getSegment(t);
		int j1 = prevBOT(i + 1);
		int j2 = prevEOT(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	/**
	 * This returns true if the given time is greater than or equal to a GSCBegin but before a GSCEnd
	 * point
	 */
	public boolean inGsChange(double t) {// fixed
		int i = getSegment(t);
		int j1 = prevBGS(i + 1);
		int j2 = prevEGS(i + 1);
		return j1 >= 0 && j1 >= j2;
	}

	/**
	 * This returns true if the given time is greater than or equal to a VSCBegin but before a VSCEnd
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
		return isTCP(ix) || inAccel(time(ix));
	}

	/**
	 * If time t is in a turn, this returns the radius, otherwise returns a
	 * negative value.
	 */
	public double turnRadiusAtTime(double t) {
		if (inTrkChange(t)) {
			int ixBOT = prevBOT(getSegment(t) + 1);
			//NavPoint bot = points.get(ixBOT);
			// return bot.position().distanceH(bot.turnCenter());
			return getTcpData(ixBOT).turnRadius();
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
			return gsAccel(b);
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
			return vsAccel(b);
		} else {
			return 0.0;
		}
	}

	

	/**
	 * Return the position at the given time. 
	 *
	 * @param t       time
	 * @param linear  if true, ignore accelerations
	 * @return    Interpolated position at time t
	 */	
	public Position position(double t, boolean linear) {
		return positionVelocity(t, linear).first;
	}

	/**
	 * Return a linear interpolation of the position at the given time. If the
	 * time is beyond the end of the plan and getExtend() is true, then the
	 * position is extrapolated after the end of the plan. An error is set if
	 * the time is before the beginning of the plan.
	 *
	 * @param t
	 *            time
	 * @return linear interpolated position at time t
	 */
	public Position position(double t) {
		return positionVelocity(t, false).first;
	}

	/** 
	 * Estimate the initial velocity at the given time for this aircraft.   
	 * A time before the beginning of the plan returns a zero velocity.
	 */
	public Velocity velocity(double tm, boolean linear) {
		return positionVelocity(tm, linear).second;
	}	public Velocity velocity(double tm) {
		return positionVelocity(tm, false).second;
	}

	/**
	 * The time required to cover distance "dist" if initial speed is "gs" and acceleration is "gsAccel"
	 *
	 * @param gs       initial ground speed
	 * @param gsAccel  ground speed acceleration
	 * @param dist     distance
	 * @return time required to cover distance
	 */
	static double timeFromDistance(double gs, double gsAccel, double dist) {
		double t1 = Util.root(0.5 * gsAccel, gs, -dist, 1);
		double t2 = Util.root(0.5 * gsAccel, gs, -dist, -1);
		double dt = Double.isNaN(t1) || t1 < 0 ? t2 : (Double.isNaN(t2) || t2 < 0 ? t1 : Util.min(t1, t2));
		return dt;
	}

	/** Return the delta time that corresponds to the point that is path distance "rdist" from the start of segment "seg"
	 * 
	 * @param seg    segment of interest
	 * @param rdist  distance from "seg"
	 * @return       time of point that is "rdist" distance from start of segment "seg"
	 * 
	 * Note: if there is a gs0=0 segment, return the time of the start of the segment return -1 on out of bounds input
	 */
	public double timeFromDistanceWithinSeg(int seg, double rdist) {
		if (seg < 0 || seg > size() - 1 || rdist < 0 || rdist > pathDistance(seg))
			return -1;
		double gs0 = initialVelocity(seg).gs();
		if (Util.almost_equals(gs0, 0.0))
			return 0;
		if (inGsChange(time(seg))) {
			double a = gsAccel(prevBGS(seg + 1));
			return timeFromDistance(gs0, a, rdist);
		} else {
			return rdist / gs0;
		}
	}

	/** Return the time that corresponds to the point that is path distance "rdist" from the start of plan
	 * 
	 * @param dist  distance from start of plan
	 * @return      time of point that is "rdist" distance from start of plan
	 * 
	 */
	public double timeFromDistance(double dist) {
		int seg = getSegmentByDistance(dist);
		if (seg < 0 || seg > size() - 1)
			return -1;
		double dd = dist - pathDistance(0, seg);
		return timeFromDistanceWithinSeg(seg, dd) + time(seg);
	}
	
	/** Return the absolute time that corresponds to the point that is path distance "dist" from startSeg
	 * 
	 * @param dist  distance from start of plan
	 * @return      time of point that is "rdist" distance from start of plan
	 * 
	 */
	public double timeFromDistance(int startSeg, double dist) {	
		//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ startSeg = "+startSeg+"\n this = "+this);
		int seg = getSegmentByDistance(startSeg,dist);
		//f.pln(" $$$$$$$ timeFromDistance: seg = "+seg);
		if (seg < 0 || seg > size() - 1) return -1;
		double pd = pathDistance(startSeg,seg);
		double distWithinLastSeg = dist - pd;
		//f.pln(" $$$$$$$ timeFromDistance: distWithinLastSeg = "+Units.str("ft",distWithinLastSeg));
		double tmWithin = timeFromDistanceWithinSeg(seg, distWithinLastSeg);
		//f.pln(" $$$$$$$ timeFromDistance: tmWithin = "+tmWithin+" time(seg) = "+time(seg));
		return tmWithin + time(seg);
	}


	/** Return the velocity of the point that is path distance "rdist" from the start of plan
	 * 
	 * @param d     distance from start of plan
	 * @return      velocity of point that is "rdist" distance from start of plan
	 * 
	 */
	public Velocity velocityByDistance(double d) {
		return velocity(timeFromDistance(d));
	}

	/**
	 * Calculate track angle out of point "i"
	 * 
	 * @param seg      The index of the point of interest
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
			Position center = turnCenter(ixBOT);
			double signedRadius = signedRadius(ixBOT);
			int dir = Util.sign(signedRadius);
			//f.pln(" $$$ trkOut (inTurn): distFrom = "+Units.str("ft",distFrom)+" signedRadius = "+Units.str("ft",signedRadius)+" ixBOT = "+ixBOT);
			double gsAt_d = 1000.0; // not used here -- don't use 0.0 because
			// will lose track info
			Position so = point(ixBOT).position();
			vNew = KinematicsPosition.turnByDist2D(so, center, dir, distFrom, gsAt_d).second;
			//f.pln(" $$$ trkOut: vNew = "+vNew);
			return vNew.trk();
		} else {
			if (isLatLon()) {
				return GreatCircle.initial_course(point(seg).lla(), point(seg+1).lla());
			} else {
				return point(seg).initialVelocity(point(seg+1)).trk();	
			}
		}
	}

	public double trkOut(int seg) {
		return trkOut(seg, false);
	}

	public double trkIn(int seg) {
		return trkFinal(seg - 1, false);
	}

	/**
	 * Calculate track angle at the end of segment "i"
	 * 
	 * @param seg      The index of the point of interest
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
			double signedRadius = signedRadius(ixBOT);
			int dir = Util.sign(signedRadius);
			Position center = turnCenter(ixBOT);
			// f.pln("$$$ trkFinal AA: d = "+Units.str("NM",d)+" signedRadius =
			// "+Units.str("NM",signedRadius)+" ixBOT = "+ixBOT);
			double gsAt_d = 1000.0; // not used here -- don't use 0.0 because will lose track info
			Position so = point(ixBOT).position();
			Velocity vFinal = KinematicsPosition.turnByDist2D(so, center, dir, d, gsAt_d).second;
			// f.pln("$$$ trkFinal AA: ixBOT = "+ixBOT+" seg = "+seg+" vFinal =
			// "+vFinal);
			return vFinal.trk();
		} else {
			//f.pln("$$$ trkFinal: point(seg) = "+point(seg)+" point(seg + 1) = "+point(seg + 1));
			if (isLatLon()) {
				// double d = pathDistance(seg,seg+1);
				double trk = GreatCircle.final_course(point(seg).lla(), point(seg + 1).lla());
				//f.pln("$$$ trkFinal BB: seg = "+seg+" trk = "+Units.str("deg",trk));
				return trk;
			} else {
				Velocity vo = NavPoint.finalVelocity(point(seg), point(seg + 1));
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
	 * @return ground speed
	 */
	public double gsOut(int i, boolean linear) {
		if (i < 0 || i > size() - 1) {
			addError("gsOut: invalid index " + i, 0);
			return -1;
		}
		if (i == size() - 1)
			return gsFinal(i - 1);
		int j = i + 1;
		// while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // skip next point(s) if very close 
		// j++;
		// }
		double dist = pathDistance(i, j, linear);
		double dt = time(j) - time(i);
		double a = 0.0;
		if (inGsChange(time(j-1)) && !linear) { // use getTime(j-1) rather  than getTime(i) in case j-1 point is an EGS
			int ixBGS = prevBGS(i + 1);
			a =gsAccel(ixBGS);
		}
		double rtn = dist / dt - 0.5 * a * dt;
		//f.pln("gsOut i="+i+" a="+a+" dist="+dist+" dt="+dt+" gs="+rtn);
		//f.pln("$$>>>>>> gsOut: rtn = "+Units.str("kn",rtn,8)+" a = "+a+" i = "+i+" j = "+j+" dt = "+f.Fm4(dt)+" dist = "+Units.str("NM",dist)+"size = "+size());
		if (rtn <= 0) {
			// f.pln(" ### WARNING gsOut: has encountered an ill-structured plan resulting in a negative ground speed!!");
			rtn = nearlyZeroGs; // do not set to 0 because it will lose track info, this can occur if dist = 0.0 or if "a" is larger
			// than it should be
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
	 * @return ground speed
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
		double dt = time(j) - time(i);
		double a = 0.0;
		//f.pln(" $$ gsFinal: i = "+i+" dist = "+Units.str("nm",dist));
		if (inGsChange(point(i).time()) && !linear) {
			int ixBGS = prevBGS(i + 1);
			a = gsAccel(ixBGS);
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
	 * @return ground speed
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
	 * @param seg      segment where time "t" is located
	 * @param gsAtSeg  ground speed out of segment "seg"
	 * @param t        time of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         ground speed at time t (which must be in segment "seg")
	 */
	private double gsAtTime(int seg, double gsAtSeg, double t, boolean linear) {
		// f.pln(" $$ gsAtTime: seg = "+seg+" gsAtSeg = "+Units.str("kn",gsAtSeg,8));
		double gs;
		if (!linear && inGsChange(t)) {
			double dt = t - time(seg);
			int ixBGS = prevBGS(seg + 1);
			double gsAccel =gsAccel(ixBGS);
			gs = gsAtSeg + dt * gsAccel;
			// f.pln(" $$ gsAtTime A: gsAccel = "+gsAccel+" dt = "+f.Fm4(dt)+" seg = "+seg+" gs = "+Units.str("kn",gs,8));
		} else {
			gs = gsAtSeg;
			// f.pln(" $$ gsAtTime B: seg = "+seg+" gs = "+Units.str("kn",gs,8));
		}
		return gs;
	}

	/**
	 * ground speed at time t
	 * 
	 * @param t      time of interest
	 * @param linear If true, then interpret plan in a linear manner
	 * @return       ground speed at time t
	 */
	public double gsAtTime(double t, boolean linear) {
		double gs;
		int seg = getSegment(t);
		if (seg < 0) {
			gs = -1;
		} else {
			double gsSeg = gsOut(seg, linear);
			// f.pln(" $$ gsAtTime: seg = "+seg+" gsAt = "+Units.str("kn",gsAt,8));
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
	 * @return vertical speed
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
		double dt = time(j) - time(i);
		//f.pln(" $$$ vsOut("+i+","+j+"): point(i) = "+point(i)+" point(j) "+point(j));
		double a = 0.0;
		//f.pln(" $$$ vsOut: getTime(j - 1) = "+getTime(j - 1));
		if (inVsChange(time(j - 1)) && !linear) { // use getTime(j-1) rather than getTime(i) in case j-1 point is an EGS
			int ixBVS = prevBVS(i + 1);
			a = vsAccel(ixBVS);
		}
		//f.pln(" $$$ vsOut: a = "+a);
		double rtn = dist / dt - 0.5 * a * dt;
		// f.pln(" $$>>>>>> vsOut: rtn = "+Units.str("fpm",rtn,8)+" a = "+a+" i = "+i+" dt = "+f.Fm4(dt)+" dist = "+Units.str("ft",dist));
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
	 * @return vertical speed
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
		double dt = time(j) - time(i);
		double a = 0.0;
		if (inVsChange(point(i).time()) && !linear) {
			int ixBvs = prevBVS(i + 1);
			a = vsAccel(ixBvs);
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
	 * @return vertical speed
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
	 * @return vertical speed
	 */
	private double vsAtTime(int seg, double vsAtSeg, double t, boolean linear) {
		// f.pln(" $$ vsAtTime: seg = "+seg+" vsAt = "+Units.str("kn",vsAt,8));
		double vs;
		if (!linear && inVsChange(t)) {
			double dt = t - time(seg);
			int ixBvs = prevBVS(seg + 1);
			double vsAccel = vsAccel(ixBvs);
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
	 * @return vertical speed
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

	/** distanceFromTime()
	 * Calculate the distance from the Navpoint at "seq" to plan position at absolute time "t"
	 * 
	 * @param seg    starting position
	 * @param t      time of stopping position (must be in segment "seg")
	 * @param linear If true, then interpret plan in a linear manner
	 * @return distance from NavPoint
	 */
	public double distFromPointToTime(int seg, double t, boolean linear) {
		double distFromSo = 0;
		double gs0 = gsOut(seg, linear);
		double dt = t - time(seg);
		if (inGsChange(t) && !linear) {
			double a = gsAccel(prevBGS(seg + 1)); 
			distFromSo = gs0 * dt + 0.5 * a * dt * dt;
			// f.pln(" $$$ distFromPointToTime(inGsChange A): dt = "+f.Fm2(dt)+" vo.gs() = "+Units.str("kn",gs0)+" distFromSo = "+Units.str("ft",distFromSo));
		} else {
			distFromSo = gs0 * dt;
			// f.pln(" $$$ distFromPointToTime(! inGsChange B): dt = "+f.Fm4(dt)+" gs0 = "+Units.str("kn",gs0)+" distFromSo = "+Units.str("ft",distFromSo));
		}
		return distFromSo;
	}

	/** Advance forward in a plan by distance (does not compute altitude)
	 * 
	 * @param seg           starting point index
	 * @param distFromSeg   distance to advance from starting point
	 * @param linear        if linear, use straight segments only
	 * @param gsAt_d        ground speed at destination
	 * @return
	 */
	public Pair<Position, Velocity> advanceDistanceWithinSeg2D(int seg, double distFromSeg, boolean linear, double gsAt_d) {
		NavPoint np1 = point(seg);
        Position so = np1.position();
        Pair<Position,Velocity> pv;
        double tSeg = point(seg).time();
		if ( ! linear && inTrkChange(tSeg)) {
			int ixPrevBOT = prevBOT(seg + 1);
			Position center = turnCenter(ixPrevBOT);
			//f.pln(" $$$ advanceDistanceWithinSeg: center = "+center);
			double signedRadius = signedRadius(ixPrevBOT);
			int dir = Util.sign(signedRadius);
			pv = KinematicsPosition.turnByDist2D(so, center, dir, distFromSeg, gsAt_d);
			// f.pln(" $$ %%%% advanceDistanceWithinSeg A: vNew("+f.Fm2(t)+") = "+vNew);
			// f.pln(" $$ %%%% advanceDistanceWithinSeg A: sNew("+f.Fm2(t)+") = "+sNew);
		} else {
			NavPoint np2 = point(seg+1);
			Velocity vo = np1.initialVelocity(np2);
			pv = so.linearDist2D(vo.trk(), distFromSeg, gsAt_d);
		}
		return pv;
	}

	
	/** Advance forward in a plan by distance (within a segment only) (does not compute altitude)
	 * 
	 * @param seg           starting point index
	 * @param distFromSeg   distance to advance from starting point
	 * @param linear        if linear, use straight segments only
	 * @return
	 */
	public Position advanceDistanceWithinSeg2D(int seg, double distFromSeg, boolean linear) {
		NavPoint np1 = point(seg);
        Position so = np1.position();
        Position pv;
        double tSeg = point(seg).time();
		if ( ! linear && inTrkChange(tSeg)) {
			int ixPrevBOT = prevBOT(seg + 1);
			Position center = turnCenter(ixPrevBOT);
			int dir = Util.sign(signedRadius(ixPrevBOT));
			pv = KinematicsPosition.turnByDist2D(so, center, dir, distFromSeg);
		} else {
			Velocity vo = np1.initialVelocity(point(seg+1));
			pv = so.linearDist2D(vo.trk(), distFromSeg);
		}
		return pv;
	}

	/** starting with point at seg advance "distanceFromSeg" in Plan
	 * NOTE do not use non-positive value for gsAt_d
	 * 
	 * @param seg             starting segment
	 * @param distFromSeg     distance to advance from start of segment "seg"
	 * @param linear          if linear, then use straight lines only
	 * @return
	 */
	public Pair<Position,Integer> advanceDistance2D(int seg, double distFromSeg, boolean linear) {
		double remainingDist = distFromSeg;
		for (int i = seg; i < size(); i++) {
			double pathDist_i = pathDistance(i);
			if (remainingDist < pathDist_i) {
				 Position newPos = advanceDistanceWithinSeg2D(i, remainingDist, linear);
				 return new Pair<Position,Integer>(newPos,i);
			} else {
				remainingDist = remainingDist - pathDist_i;
			}
		}
		addWarning("advanceDistance: distance exceeded length of plan!");
		int ixLast = size() - 1;
		return  new Pair<Position, Integer>(points.get(ixLast).position(), ixLast);
	}
	
//	public Pair<Position,Integer> advanceDistance(int seg, double distFromSeg, boolean linear) {
//		Pair<Position,Integer> posSeg = advanceDistance2D(seg, distFromSeg, linear);
//		double t = timeFromDistance(seg, distFromSeg);
//		double alt = position(t).alt();
//		//double deltaAlt = posSeg.first.alt() - alt;
//		//f.pln(" $$$$ advanceDistance: deltaAlt = "+deltaAlt);
//		Position altPos = posSeg.first.mkAlt(alt);
//		return new Pair<Position,Integer>(altPos,posSeg.second);
//	}
	
	
	public Pair<Position,Integer> advanceDistance(int seg, double distFromSeg, boolean linear) {
		double remainingDist = distFromSeg;
		for (int i = seg; i < size(); i++) {
			double pathDist_i = pathDistance(i);
			if (remainingDist < pathDist_i) {
				 Position newPos = advanceDistanceWithinSeg2D(i, remainingDist, linear);
				 double t = timeFromDistance(i, remainingDist);
				 double alt = position(t).alt();
				 Position altPos = newPos.mkAlt(alt);
				 return new Pair<Position,Integer>(altPos,i);
			} else {
				remainingDist = remainingDist - pathDist_i;
			}
		}
		addWarning("advanceDistance: distance exceeded length of plan!");
		int ixLast = size() - 1;
		return  new Pair<Position, Integer>(points.get(ixLast).position(), ixLast);
	}

	
	/** Assumes seg = getSegment(t)
	 * 
	 * @param t         time of interest
	 * @param linear    If true, then interpret plan in a linear manner
	 * @param gsAt_d    ground speed at time t
	 * @return          position and velocity at time t
	 */
	public Pair<Position, Velocity> posVelWithinSeg2D(int seg, double t, boolean linear, double gsAt_d) {
		NavPoint np1 = point(seg);
        Position so = np1.position();
        Pair<Position,Velocity> pv;
		if ( ! linear && inTrkChange(t)) {
			int ixPrevBOT = prevBOT(seg + 1);
			Position center = turnCenter(ixPrevBOT);
			double signedRadius = signedRadius(ixPrevBOT);
			int dir = Util.sign(signedRadius);
			double distFromSo;
			boolean method1 = true; // starting position could be either the current segment or the previous BOT
			if (method1) {
				distFromSo = distFromPointToTime(seg, t, linear); // starting position is the current segment
			} else {
				so = point(ixPrevBOT).position(); // // starting position is the previous BOT
				distFromSo = distFromPointToTime(ixPrevBOT, t, linear);
			}
			pv = KinematicsPosition.turnByDist2D(so, center, dir, distFromSo, gsAt_d);
			// f.pln(" $$ %%%% posVelWithinSeg A: vNew("+f.Fm2(t)+") = "+vNew);
			// f.pln(" $$ %%%% posVelWithinSeg A: sNew("+f.Fm2(t)+") = "+sNew);
		} else {
			NavPoint np2 = point(seg+1);
			Velocity vo = np1.initialVelocity(np2);
			double distFromSo = distFromPointToTime(seg, t, linear);
//			Pair<Position, Velocity> pv = so.linearDist2D(vo, distFromSo);
//			vNew = pv.second.mkGs(gsAt_d);
			pv = so.linearDist2D(vo.trk(), distFromSo, gsAt_d);
		}
		return pv;
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
		NavPoint np1 = point(seg);
		if (seg + 1 > size() - 1) { // at Last Point
			Velocity v = finalVelocity(seg - 1);
			// f.pln("\n -----size = "+size()+" seg = "+seg+"\n $$$ accelZone: np1 = "+np1+" v = "+v);
			return new Pair<Position, Velocity>(np1.position(), v);
		}
		// f.pln("\n ----------- seg = "+seg+" $$$ positionVelocity: np1 = "+np1+" np2 = "+np2);
		double gs0 = gsOut(seg, linear);
		double gsAt_d = gsAtTime(seg, gs0, t, linear);
		// f.pln(t+" $$$ positionVelocity: seg = "+seg+" t = "+f.Fm2(t)+" positionVelocity: so = "+so+" gs0 = "+Units.str("kn",gs0));
		Pair<Position, Velocity> adv = posVelWithinSeg2D(seg, t, linear, gsAt_d);
		Position sNew = adv.first;
		Velocity vNew = adv.second;
		Pair<Double,Double> altPair = interpolateAltVs(seg, t-time(seg), linear);
		sNew = sNew.mkAlt(altPair.first);
		vNew = vNew.mkVs(altPair.second); 
		// f.pln(" $$ %%%% positionVelocity RETURN: sNew("+f.Fm2(t)+") = "+sNew);
		// f.pln(" $$ %%%% positionVelocity RETURN: vNew("+f.Fm2(t)+") = "+vNew);
		return new Pair<Position, Velocity>(sNew, vNew);
	}

	public Pair<Position, Velocity> positionVelocity(double t) {
		return positionVelocity(t, false);
	}

	/** estimate the velocity from point i to point i+1 (at point i).
	 * 
	 * @param i index of point
	 * @return velocity at point i
	 */
	public Velocity initialVelocity(int i) {
		return initialVelocity(i, false);
	}

	public Velocity initialVelocity(int i, boolean linear) {
		return Velocity.mkTrkGsVs(trkOut(i, linear), gsOut(i, linear), vsOut(i, linear));
	}

	/** 
	 *  Return interpolated altitude in this segment at absolute time t
	 * 
	 * @param seg     getSegment(t)
	 * @param dt      relative time from start of seg
	 * @param linear  linear flag
	 * @return        altitude and velocity at time dt greater than the time of segment "seg
	 */
	public Pair<Double,Double> interpolateAltVs(int seg, double dt, boolean linear) {
		//f.pln("\n $$$................. interpolateAltVs: seg = "+seg+" dt = "+dt);
		double vsAccel = 0.0;
		double tSeg = time(seg);
		//double t = tSeg + dt;
		if ( ! linear && inVsChange(tSeg)) {
			int ixBVS = prevBVS(seg+1);
			vsAccel = vsAccel(ixBVS);
		}
		//f.pln("\n $$$................. interpolateAltVs: seg = "+seg+" vsAccel = "+vsAccel);
		double alt1 = point(seg).alt();
		double vsInit = vsOut(seg,linear);	
		double newAlt = alt1 + vsInit*dt + 0.5 * vsAccel*dt*dt;; // interpolateAlt(vsInit, vsAccel,  alt1,  t1,  t);
		//f.pln(" $$$ interpolateAltVs: dt = "+dt+" vsInit = "+Units.str("fpm",vsInit,4)+" alt1 = "+Units.str("ft",alt1,4));
		double newVs = vsInit + vsAccel*dt;
		//f.pln(" $$$ interpolateAltVs: dt = "+dt+" newVs = "+Units.str("fpm",newVs,4)+" newAlt = "+Units.str("ft",newAlt,4));
		return new Pair<Double,Double>(newAlt,newVs);
	}

//	//	public Pair<Double,Double> interpolateAltVs(double t, boolean linear) {
//	//		int seg = getSegment(t);
//	//		return interpolateAltVs(seg,t,linear);
//	//	}
//
//	private double interpolateAlt(double vsInit, double vsAccel, double alt1, double t1, double t) {		 
//		double rtn;
//		if (t < t1) { // vsAccel is assumed to be defined for t > t1
//			rtn = alt1;
//		} else {
//			double dt = t-t1;
//			rtn = alt1 + vsInit*dt + 0.5 * vsAccel*dt*dt;
//		}
//		return rtn;
//	}
	
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
			rtn = NavPoint.finalVelocity(npi, lastPt);
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
		return NavPoint.averageVelocity(points.get(i),points.get(i + 1));
		// return velocity((t2-t1)/2.0+t1);
	}
	
	// return absolute time needed at waypoint i in order for ground speed in to be gs
	// assumes that path from (i-1) to (i) is not in a turn 
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
	 * calculate delta time for point i to make ground speed into it = gs
	 * 
	 * @param i         index of interest
	 * @param gs        target ground speed
	 * @return          delta time needed  
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
		if (inGsChange(points.get(i-1).time()) && !isBGS(i)) { // TODO:  does this work for an EGS?
			// f.pln("#### points.get(i) = "+points.get(i));
//			double dist = pathDistance(i - 1, i);
//			double initGs = gsOut(i - 1);
//			int ixBGS = prevBGS(i);
//			double gsAccel = gsAccel(ixBGS);
//			double deltaGs = gs - initGs;
//			double dt = deltaGs / gsAccel;
//			double acceldist = dt * (gs + initGs) / 2;
//			if (acceldist > dist) {
//				// f.pln("#### calcTimeGSin: insufficient distance to achieve new ground speed");
//				addError("calcTimeGSin " + f.Fm0(i) + " insufficient distance to achieve new ground speed", i);
//				return -1;
//			}
//			dt = dt + (dist - acceldist) / gs;
//			f.pln("####>>> calcTimeGSin: dist = "+Units.str("nm",dist)+" deltaGs = "+Units.str("kn",deltaGs)+" dt = "+dt);
//			Debug.halt();
			addError(" calcDtGsin:  attempt to change point "+i+"'s time which is inside a ground speed acceleration!");
			return 0;
		} else {
			double dist = pathDistance(i - 1, i);
			if (dist < 1E-12) {
				f.pln("+++++++++++++++++++++++++++++++++++++++++++ ### calcDtGsin WARNING: dist = "+dist);
			}
			double dt = dist / gs;
			//f.pln("#### calcTimeGSin: i = "+i+" dist = "+Units.str("ft",dist,10)+" dt = "+f.Fm4(dt)+" points.get(i-1).time() = "+points.get(i-1).time());
			return dt;
		}
	}



	/**
	 * change the ground speed into ix to be gs -- all other ground speeds remain the same
	 * 
	 * 
	 * @param ix  index
	 * @param gs  new ground speed
	 * @param updateTCP
	 */
	public void mkGsIn(int ix, double gs) {
		if (ix > size() - 1)
			return;
		double tmIx = calcTimeGSin(ix, gs);
		double dt = tmIx - point(ix).time();
		//double delta = (point(ix).time()-point(ix-1).time());
		//if (dt < 0 && -dt > delta) f.pln(" $$ mkGsIn: tmIx = "+tmIx+" dt = "+dt+" delta = "+delta);
		timeShiftPlan(ix, dt);
	}

	/**
	 * Change the ground speed at ix to be gs -- all other ground speeds remain
	 * the same NOTE: This assumes that there are no BVS - EVS segments in the
	 * area
	 *
	 * Note: If point is a begin TCP, we need to update the velocityIn
	 * 
	 * @param ix
	 *            index
	 * @param gs
	 *            new ground speed
	 */
	public void mkGsOut(int ix, double gs) {
		if (ix >= size() - 1)
			return;
		double newT = calcTimeGSin(ix + 1, gs);
		//f.pln("\n $$ makeGsOut: ix = "+ix+" newT = "+f.Fm4(newT)+" gs = "+Units.str("kn",gs));
		double dt = newT - time(ix + 1);
		timeShiftPlan(ix + 1, dt);
		NavPoint np = point(ix);
		if (isBeginTCP(ix)) {
			//Velocity vin = velocityInit(ix);
			Velocity vin = initialVelocity(ix);
			double vsOut = vsOut(ix);
			vin = Velocity.mkTrkGsVs(vin.trk(), gs, vsOut);
			TcpData np_tcp = getTcpData(ix);
			//f.pln(" $$ makeGsOut: vin = "+vin+" npNew = "+npNew.toStringFull());
			set(ix,np, np_tcp);
			// f.pln(" $$$$ ix = "+ix+" AFTER vsOut = "+Units.str("fpm",vsOut(ix)));
		}
		//f.pln(" $$$$ makeGsOut: EXIT gsOut = "+Units.str("kn",gsOut(ix),6));
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
		//		NavPt tempv = NavPt.mkAlt(get(i),newAlt);
		NavPoint tempv = point(i).mkAlt(newAlt);
		TcpData tcp = getTcpData(i);
		if (preserve)
			tcp = tcp.setAltPreserve();
		setNavPoint(i, tempv);
	}

//	public boolean isVelocityContinuous() {
//		for (int i = 0; i < size(); i++) {
//			if (i > 0) {
//				if (isTCP(i)) {
//					if (!finalVelocity(i - 1).compare(initialVelocity(i), Units.from("deg", 10.0), Units.from("kn", 20),
//							Units.from("fpm", 100))) { // see testAces3, testRandom for worst cases
//						// f.pln(" $$$ isVelocityContinuous: FAIL! continuity: finalVelocity("+(i-1)+") = "+finalVelocity(i-1).toString4NP()+" != initialVelocity("+i+") = "+initialVelocity(i).toString4NP());
//						return false;
//					}
//				}
//			}
//		}
//		return true;
//	}

	
//	public boolean isVelocityContinuous(boolean silent) {		
//		boolean rtn = true;
//		for (int i = 0; i < size(); i++) {
//			if (i > 0) {
//				if (isTCP(i)) {
//					if (!PlanUtil.isVelocityContinuous(this, i, 0.5, silent)) {
//						rtn = false;
//					}
//				}
//			}
//		}
//		return rtn;
//	}
	
	public boolean isVelocityContinuous() { return isVelocityContinuous(true); }
	
	public boolean isVelocityContinuous(boolean silent) {
		for (int i = 0; i < size(); i++) {
			if (i > 0) {
				if (isTCP(i)) {
					if (!PlanUtil.isTrkContinuous(this, i, Units.from("deg", 5.0), silent)) return false;
					if (!PlanUtil.isGsContinuous(this, i, Units.from("kn", 10), silent)) return false;
					if (!PlanUtil.isVsContinuous(this, i, Units.from("fpm", 100), silent)) return false;
				}
			}
		}
		return true;
	}
	
	public boolean isWeakVelocityContinuous(boolean silent) {
		for (int i = 0; i < size(); i++) {
			if (i > 0) {
				if (isTCP(i)) {
					if (!PlanUtil.isTrkContinuous(this, i, Units.from("deg", 10.0), silent)) return false;
					if (!PlanUtil.isGsContinuous(this, i, Units.from("kn", 20), silent)) return false;
					if (!PlanUtil.isVsContinuous(this, i, Units.from("fpm", 300), silent)) return false;
				}
			}
		}
		return true;
	}


	
	public boolean isGsContinuous(boolean silent) {
		boolean rtn = true;
		for (int i = 0; i < size(); i++) {
			if (i > 0) {
				if (isTCP(i)) {
					double maxGsDelta = Units.from("kn",5.05399568);
					if (!PlanUtil.isGsContinuous(this, i, maxGsDelta, silent)) {
						rtn = false;
					}
				}
			}
		}
		return rtn;
	}

	/**
	 * Find the horizontal (curved) distance between points i and i+1 [meters].
	 * 
	 * @param i  index of starting point
	 * @return   path distance (horizontal only)
	 */
	public double pathDistance(int i) {
		return pathDistance(i, false);
	}

	/**
	 * Find the horizontal distance between points i and i+1 [meters].
	 * 
	 * @param i        index of starting point
	 * @param linear   if true, measure the straight distance, if false measure the curved distance
	 * @return         path distance (horizontal only)
	 */
	public double pathDistance(int i, boolean linear) {
		if (i < 0 || i + 1 >= size()) {
			return 0.0;
		}
		Position p1 = points.get(i).position();
		Position p2 = points.get(i+1).position();
		boolean inTurn = inTrkChange(time(i));
		//f.pln(" $$ pathDistance: i = "+i+" inTurn = "+inTurn+" linear = "+linear);
		if (!linear && inTurn) {
			// if in a turn, figure the arc distance			
			int ixBOT = prevBOT(i + 1);
			Position center = turnCenter(ixBOT);
			//f.pln(" $$ pathDistance: i = "+i+" center = "+center);
			double R = getTcpData(ixBOT).turnRadius();
			double theta = PositionUtil.angle_between(p1, center, p2);
			double rtn = Math.abs(theta * R); // TODO is this right for spherical coordinates???
			//f.pln(" $$ Plan.pathDistance("+i+"): R = "+Units.str("NM",R,5)+" rtn = "+Units.str("NM",rtn,5));
			return rtn;
		} else { // otherwise just use linear distance
			double rtn = p1.distanceH(p2);
			//f.pln(" $$ Plan.pathDistance("+i+"): LINEAR SEGMENT: rtn = "+Units.str("NM",rtn,5));
			return rtn;
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
	 * @param i      beginning index
	 * @param j      ending index
	 * @param linear   if true, then TCP turns are ignored. Otherwise, the length of the circular turns are calculated.
	 * @return cumulative path distance (horizontal only)
	 */
	public double pathDistance(int i, int j, boolean linear) {
		// f.pln(" $$ pathDistance: i = "+i+" j = "+j+" size = "+size());
		if (i < 0) {
			i = 0;
		}
		if (j >= size()) { // >= is correct, pathDistance(jj, linear) measures from jj to jj+1
			j = size() - 1;
		}
		double total = 0.0;
		for (int jj = i; jj < j; jj++) {
			total = total + pathDistance(jj, linear);
			// f.pln(" $$ pathDistance: i = "+i+" jj = "+jj+" dist = "+Units.str("NM",pathDistance(jj, linear)));
		}
		//f.pln(" $$ Plan.pathDistance: i = "+i+" j = "+j+" total = "+Units.str("NM",total));
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
		if (inTrkChange(t) && !linear) {
			int ixBOT = prevBOT(getSegment(t) + 1);
			//NavPoint bot = points.get(ixBOT);
			double R = turnRadius(ixBOT);
			Position center = turnCenter(ixBOT);
			double alpha = PositionUtil.angle_between(currentPosition, center, points.get(seg + 1).position());
			// f.pln(" $$$$ alpha = "+Units.str("deg",alpha));
			double rtn = Math.abs(alpha * R);
			// f.pln(" $$$$+++++ partialPathDistance: rtn = "+Units.str("nm",rtn));
			return rtn;
		} else {
			// otherwise just use linear distance
			double rtn = position(t).distanceH(points.get(seg + 1).position());
			// f.pln(" $$$$.... partialPathDistance: points.get(seg+1) = "+points.get(seg+1));
			// f.pln(" $$$$.... partialPathDistance: rtn = "+Units.str("nm",rtn));
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
	 * @param ix  current segment or size() if there is no future segment that is linear
	 * @return    first linear segment greater than or equal to ix
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
	 * This returns true if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and 
	 * end points.  Also requires that there are no points closer together than Plan.minDt.
	 */
	public boolean isWellFormed() {				
		int rtn = indexWellFormed(); 
		//f.pln(" $$$$ isWellFormed: rtn = "+rtn);
		return rtn < 0;
	}

	/**
	 * This returns -1 if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and 
	 * end point. Returns a nonnegative value to indicate the problem point. Also requires that there are no points closer 
	 * together than Plan.minDt.
	 */
	public int indexWellFormed() {
		double lastTm = -1;
		for (int i = 0; i < size(); i++) {
			if (isBOT(i)) {
				int j1 = nextBOT(i);
				int j2 = nextEOT(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return i;
			}
			if (isEOT(i)) {
				int j1 = prevBOT(i);
				int j2 = prevEOT(i);
				if (!(j1 >= 0 && j1 >= j2))
					return i;
			}
			if (isBGS(i)) {
				int j1 = nextBGS(i);
				int j2 = nextEGS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return i;
			}
			if (isEGS(i)) {
				int j1 = prevBGS(i);
				int j2 = prevEGS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return i;
			}
			if (isBVS(i)) {
				int j1 = nextBVS(i);
				int j2 = nextEVS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return i;
			}
			if (isEVS(i)) {
				int j1 = prevBVS(i);
				int j2 = prevEVS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return i;
			}
			double tm_i = time(i);
			if (i > 0) {
				double dt = Math.abs(tm_i-lastTm);
				if (dt < minDt) {
					f.pln("$$ Plan.indexWellFormed: Delta time into i = "+i+" is "+dt+" which is less than minDt = "+minDt);
					return i;
				}			
				lastTm = tm_i;
			}
		}
		return -1;
	}

	/**
	 * This returns a string representing which part of the plan is not "well formed", i.e. all acceleration zones 
	 * have a matching beginning and end point.  See isWellFormed().
	 */
	public String strWellFormed() {
		// f.pln(" isWellFormed: size() = "+size());
		String rtn = "";
		double lastTm = -1;
		for (int i = 0; i < size(); i++) {
			NavPoint np = point(i);
			// not well formed if GSC overlaps with other accel zones
			if (isBOT(i)) {
				int j1 = nextBOT(i);
				int j2 = nextEOT(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return "BOT at i " + i + " NOT FOLLOWED BY EOT!";
			}
			if (isEOT(i)) {
				int j1 = prevBOT(i);
				int j2 = prevEOT(i);
				if (!(j1 >= 0 && j1 >= j2))
					return "EOT at i " + i + " NOT PRECEEDED BY BOT!";
			}
			if (isBGS(i)) {
				int j1 = nextBGS(i);
				int j2 = nextEGS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return "BGS at i " + i + " NOT FOLLOWED BY EGS!";
			}
			if (isEGS(i)) {
				int j1 = prevBGS(i);
				int j2 = prevEGS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return "EGS at i " + i + " NOT PRECEEDED BY BGS!";
			}
			if (isBVS(i)) {
				int j1 = nextBVS(i);
				int j2 = nextEVS(i);
				if (j2 < 0 || (j1 > 0 && j1 < j2))
					return "BVS at i " + i + " NOT FOLLOWED BY EVS!";
			}
			if (isEVS(i)) {
				int j1 = prevBVS(i);
				int j2 = prevEVS(i);
				if (!(j1 >= 0 && j1 >= j2))
					return "EVS at i " + i + " NOT PRECEEDED BY BVS!";
			}
			if (this.inGsChange(np.time()) && this.inTrkChange(np.time())) {
				rtn = rtn + "  Overlap FAIL at i = " + i;
			}
			// f.pln(" isWellFormed: i = "+i+" OK");
			double tm_i = time(i);
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

	public boolean isConsistent() {
		return isConsistent(true);
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
			if (isBOT(i)) {
				if (!PlanUtil.turnConsistent(this, i, 0.01, silent)) {
					// error.addWarning("isConsistent fail: "+i+" Not turn  consistent!");
					rtn = false;
				}
			}
		}
		return rtn;
	}
	
	public boolean isTurnConsistent() { return isTurnConsistent(false);}


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
			if (isBVS(i)) {
				if (!PlanUtil.vsConsistent(this, i, 0.00001, silent)) {
					// error.addWarning("isConsistent fail: "+i+" Not vs
					// consistent!");
					rtn = false;
				}
			}
		}
		return rtn;
	}

	public boolean isVsConsistent() { return isVsConsistent(false);}

	
	public boolean isConsistent(double maxTrkDist, double maxGsDist, double maxVsDist, boolean silent) {
		boolean rtn = true;
		if (!isWellFormed()) {
			if (!silent) {
				f.pln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			}
			error.addError("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
			return false;
		}
		for (int i = 0; i < size(); i++) {
			if (isBOT(i)) {
				if (!PlanUtil.turnConsistent(this, i, maxTrkDist, silent)) {
					//error.addWarning("isConsistent: turn " + i + " not consistent");
					rtn = false;
				}
			}
			if (isBGS(i)) {
				if (!PlanUtil.gsConsistent(this, i, maxGsDist, silent)) {
					//error.addWarning("isConsistent: GS " + i + " not consistent");
					rtn = false;
				}
			}
			if (isBVS(i)) {
				if (!PlanUtil.vsConsistent(this, i, maxVsDist, silent)) {
					//error.addWarning("isConsistent: VS " + i + " not consistent");
					rtn = false;
				}
			}
		}
		return rtn;
	}
	
	public boolean isConsistent(boolean silent) {
		return isConsistent(0.01, 0.007, 0.00001,silent);
	}

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instanteous "jumps," it is not consistent.
	 */
	public boolean isWeakConsistent(boolean silent) {
		return isConsistent(0.05,0.05,0.01,silent);
	}


	public boolean isFlyable(boolean silent) {
		return isConsistent(silent) && isVelocityContinuous(silent);
	}
	
	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instanteous "jumps," it is not consistent.
	 */
	public boolean isFlyable() {
		boolean silent = false;
		return isFlyable(silent);
	}
	
	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instanteous "jumps," it is not consistent.
	 */
	public boolean isWeakFlyable(boolean silent) {
		return isWeakConsistent(silent) && isWeakVelocityContinuous(silent);
	}

	public boolean isWeakFlyable() {
		boolean silent = false;
		return isWeakFlyable(silent);
	}


	/**
	 * This removes the acceleration tags on points that appear "unbalanced."
	 * This is not particularly intelligent and may result in bizarre (but
	 * legal) plans.
	 */
	public void fix() {
		// if (!isWellFormed()) {
		f.pln(" Plan.fix has not been ported yet -- used only in Watch!");
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
		double t1 = time(seg);
		double dt = time(seg + 1) - t1;
		NavPoint np = points.get(seg);
		NavPoint np2 = points.get(seg + 1);
		if (pathDistance(seg) <= 0.0) {
			// f.pln("Plan.closestPositionHoriz: "+name+" patDistance for seg "+seg+" is zero");
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
				double a = vsAccel(prevBVS(seg + 1));
				double tm = KinematicsDist.gsTimeConstantAccelFromDist(vs1, a, p.alt() - np.alt());
				ret = new NavPoint(position(tm), tm);
			} else {
				double vtot = Math.abs(np.alt() - np2.alt());
				double frac = Math.abs(np.alt() - p.alt()) / vtot;
				double tm = dt * frac - t1;
				ret = new NavPoint(position(tm), tm);
			}
		} else if (inTrkChange(t1)) {
			int ixBOT = prevBOT(seg + 1);
			//NavPoint bot = points.get(ixBOT);			
			Position center = turnCenter(ixBOT);
			double endD = pathDistance(seg);
			double d2 = KinematicsPosition.closestDistOnTurn(np.position(), initialVelocity(seg), getTcpData(ixBOT).turnRadius(),
					Util.sign(signedRadius(ixBOT)), center, p, endD);
			if (Util.almost_equals(d2, 0.0)) {
				ret = np;
			} else if (Util.almost_equals(d2, endD)) {
				ret = np2;
			} else {
				double segDt = timeFromDistanceWithinSeg(seg, d2);
				ret = new NavPoint(position(t1 + segDt), t1 + segDt);
			}
		} else if (isLatLon()) {
			LatLonAlt lla = GreatCircle.closest_point_segment(points.get(seg).lla(), points.get(seg + 1).lla(),
					p.lla());
			d = GreatCircle.distance(points.get(seg).lla(), lla);
			double segDt = timeFromDistanceWithinSeg(seg, d);
			ret = new NavPoint(position(t1 + segDt), t1 + segDt);
		} else {
			Vect3 cp = VectFuns.closestPointOnSegment(points.get(seg).point(), points.get(seg + 1).point(), p.point());
			d = points.get(seg).point().distanceH(cp);
			double segDt = timeFromDistanceWithinSeg(seg, d);
			ret = new NavPoint(position(t1 + segDt), t1 + segDt);
		}
		return ret;
	}



	/**
	 * Experimental This returns a NavPoint on the plan that is closest to the
	 * given position. If more than one point are closest horizontally, the
	 * closer vertically is returned. If more than one have the same horizontal
	 * and vertical distances, the first is returned.
	 * 
	 * @param p
	 * @return closest point
	 */
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
	 * If start &gt;= end, this returns an INVALID point
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
	 * @return closest point
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
			NavPoint np1 = point(i);
			TcpData tcp1 = getTcpData(i);
			NavPoint np2 = p.point(i);
			TcpData tcp2 = p.getTcpData(i);

			if ( ! np1.almostEquals(np2)) {
				f.pln(" points at index i = "+i+" have different positions!");
			}
			if ( ! Util.almost_equals(np1.time(),np2.time())) {
				f.pln(" points at index i = "+i+" have different times! "+np1.time()+" != "+np2.time());			
			}
			//			if ( ! tcp1.velocityInit().almostEquals(tcp2.velocityInit())) {
			//				f.pln(" points at index i = "+i+" have different velocityIn fields "+tcp1.velocityInit()+" != "+tcp2.velocityInit());
			//			}
			if ( ! tcp1.getTrkTypeString().equals(tcp2.getTrkTypeString()) || ! tcp1.getGsTypeString().equals(tcp2.getGsTypeString()) || ! tcp1.getVsTypeString().equals(tcp2.getVsTypeString())) {
				f.pln(" points at index i = "+i+" have different tcp_* fields");
			}
			if ( tcp1.isBOT() && tcp1.getRadiusSigned() != tcp2.getRadiusSigned()) {
				f.pln(" points at index i = "+i+" have different accel_trk fields "+tcp1.getRadiusSigned()+" != "+tcp2.getRadiusSigned());
			}
			if ( tcp1.isBGS() && tcp1.gsAccel() != tcp2.gsAccel()) {
				f.pln(" points at index i = "+i+" have different accel_gs fields "+tcp1.gsAccel()+" != "+tcp2.gsAccel());
			}
			if ( tcp1.isBVS() && tcp1.vsAccel() != tcp2.vsAccel()) {
				f.pln(" points at index i = "+i+" have different accel_vs fields "+tcp1.vsAccel()+" != "+tcp2.vsAccel());
			}
			if ( ! tcp1.getSourcePosition().almostEquals(tcp2.getSourcePosition())) {
				f.pln(" points at index i = "+i+" have different source positions!");
			}
			if (!Util.almost_equals(tcp1.getSourceTime(), tcp2.getSourceTime())) {
				f.pln(" points at index i = "+i+" have different source times!");						
			}

		}
	}

//	Position vertexFromTurnTcps(int ixBOT, int ixEOT) {
//		return vertexFromTurnTcps(ixBOT, ixEOT, -1);
//	}

	/** Structurally calculate vertex of turn from BOT and EOT.  If altMid >=0 THEN
	 *  use it for the altitude.  Otherwise search for a middle point to get altitude.
	 * 
	 * @param ixBOT
	 * @param ixEOT
	 * @param altMid
	 * @return
	 */
	public Position vertexFromTurnTcps(int ixBOT, int ixEOT, double altMid) {
		//f.pln(" $$ vertexFromTurnTcps: ixBOT = "+ixBOT+" ixEOT = "+ixEOT);
		if (!validIndex(ixBOT) || !validIndex(ixEOT)) return Position.INVALID;
		double radius = turnRadius(ixBOT);
		Position center = turnCenter(ixBOT);
		//f.pln(" $$ vertexFromTurnTcps: center = "+center.toString());
		Position botPos = getPos(ixBOT);
		Position eotPos = getPos(ixEOT);
		double trkIn_d = trkOut(ixBOT);
		double trkOut_d = trkOut(ixEOT);
		int dir = Util.turnDir(trkIn_d,trkOut_d);	
		//f.pln(" $$ vertexFromTurnTcps: trkIn = "+Units.str("deg",trkIn)+" trkOut = "+Units.str("deg",trkOut)+" dir = "+dir);
		// ---------- recover altitude ---------
		if (altMid < 0) {
		   int ixMOT = findMOT(ixBOT,ixEOT);
		   if (ixMOT < 0) ixMOT = ixEOT-1;
		   altMid = point(ixMOT).alt();
		}
		//f.pln(" $$ vertexFromTurnTcps: botPos = "+botPos+" center = "+center+" eotPos = "+eotPos);
		return vertexFromTurnTcps(botPos, eotPos, radius, dir, center, altMid);
	}

	NavPoint vertexPointTurnTcps(int ixBOT, int ixEOT, double altMid) {
		Position vertex = vertexFromTurnTcps(ixBOT, ixEOT, altMid);
		double gsIn = gsIn(ixBOT);
		Position botPos = getPos(ixBOT);
		double distToVertex = botPos.distanceH(vertex);
		double gsAccel = 0.0;
		double tmBOT = time(ixBOT);
		if (inGsChange(tmBOT)) {
			int ixBGS = prevBGS(ixBOT+1);
			gsAccel = gsAccel(ixBGS);
		}
		double dt = timeFromDistance(gsIn, gsAccel, distToVertex);
		return new NavPoint(vertex,tmBOT+dt);
	}
	
	/**
	 * 
	 * @param botPos   position of turn beginning
	 * @param eotPos   position of end of turn
	 * @param radius   radius of turn 
	 * @param dir      direction +1 = right, -1 = left
	 * @param center   center position
	 * @param altMid   altitude at middle of turn
	 * @return         vertex of turn
	 */
	public static Position vertexFromTurnTcps(Position botPos, Position eotPos, double radius, int dir, Position center, double altMid) {
		double theta = PositionUtil.angle_between(botPos, center, eotPos);
		double cos_theta2 = Math.cos(theta/2);
		double distance = 100.0;
		if (cos_theta2 != 0.0) {
			distance = radius/cos_theta2;
		}
		Position vertex;		
		//f.pln(" $$ vertexFromTurnTcps: theta = "+Units.str("deg",theta)+" distance = "+Units.str("NM",distance));
		if (botPos.isLatLon()) {
			double centerToBOT_trk = GreatCircle.initial_course(center.lla(),botPos.lla());			
			double cLineTrk = centerToBOT_trk + dir*theta/2;	
			//f.pln(" $$ vertexFromTurnTcps: centerToBOT_trk = "+Units.str("deg",centerToBOT_trk));
			vertex = new Position(GreatCircle.linear_initial(center.lla(), cLineTrk, distance));
		} else {
			double centerToBOT_trk = Velocity.mkVel(center.point(), botPos.point(), 100.0).trk();
			//double centerToEOT_trk = Velocity.mkVel(center.point(), eotPos.point(), 100.0).trk();
			double cLineTrk = centerToBOT_trk + dir*theta/2;
			Vect3 sn = center.point().linearByDist2D(cLineTrk, distance);
			//f.pln(" $$ vertexFromTurnTcps: centerToBOT_trk = "+Units.str("deg",centerToBOT_trk)+" cLineTrk = "+Units.str("deg",cLineTrk));
			vertex = new Position(sn);
		}
		vertex = vertex.mkAlt(altMid);  
		return vertex;		
	}
	
	/**
	 * 
	 * @param botPos   position of turn beginning
	 * @param eotPos   position of end of turn
	 * @param signedRadius   signed radius of turn: +1 = right, -1 = left
	 * @param trkInit  initial track at botPos
	 * @param altMid   altitude at middle of turn
	 * @return         vertex of turn
	 */
	public static Position vertexFromTurnTcps(Position botPos, Position eotPos, double signedRadius, double trkInit, double altMid) {
		int dir = Util.sign(signedRadius);
		double radius = Math.abs(signedRadius);
		Position center = KinematicsPosition.centerFromRadius(botPos, signedRadius, trkInit);
		return vertexFromTurnTcps(botPos, eotPos, radius, dir, center, altMid);
	}

    int findMOT(int ixBOT, int ixEOT) {
    	if (ixEOT == ixBOT +2) {
    		//f.pln(" $$ findMOT ("+ixBOT+","+ixEOT+"): AA return (ixBOT+1) = "+(ixBOT+1));
    		return ixBOT+1;
    	} else {
    		int linIx = linearIndex(ixBOT);
    		if (linIx >= 0) {
    			for (int ixMOT = ixBOT; ixMOT < ixEOT; ixMOT++) {
    				if (linearIndex(ixMOT) == linIx) {
    					//f.pln(" $$ findMOT ("+ixBOT+","+ixEOT+"): BB return via linearIndex = "+(ixMOT));
    					return ixMOT;
    				}
    			}
    		}
     	}
     	double pathDistBOT2EOT = pathDistance(ixBOT,ixEOT);
    	double target = 0.49*pathDistBOT2EOT;
    	for (int ixMOT = ixBOT; ixMOT < ixEOT; ixMOT++) {
    		if (pathDistance(ixBOT,ixMOT) >= target) {
    			//f.pln(" $$ findMOT("+ixBOT+","+ixEOT+"): CC return via 1/2 pathDistance = "+(ixMOT));
    			return ixMOT;
    		}
    	}
    	//f.pln(" $$ findMOT("+ixBOT+","+ixEOT+"): DD return -1");
    	//DebugSupport.dumpPlan(this,"LAST_RESORT");
    	//Debug.halt();
    	return -1;
    }
    
    int findNextMOT(int i) {
    	int ixBOT = nextBOT(i);
    	int ixEOT = nextEOT(ixBOT);
    	return findMOT(ixBOT,ixEOT);
    }
    
    // Quad(distanceTo, altAt, gsIn, label)
    private ArrayList<Tuple5<Double,Double,Double,String,String>> buildDistList(int ixBOT, int ixEOT, double ratio) {
       	ArrayList<Tuple5<Double,Double,Double,String,String>> distList = new ArrayList<Tuple5<Double,Double,Double,String,String>>();
       	for (int i = ixBOT; i < ixEOT; i++) {
       		double d_i = ratio*pathDistance(ixBOT,i);
       		double alt_i = point(i).alt();
       		double gsIn_i = gsOut(i);
       		String label_i = point(i).label();
       		String info_i = getInfo(i);
       		Tuple5<Double,Double,Double,String,String> trip_i = new Tuple5<Double,Double,Double,String,String>(d_i,alt_i,gsIn_i,label_i,info_i);
       		distList.add(trip_i);
       	}
    	return distList;
    }
    
    /** build a list of (distanceTo, altAt, gsIn, label) from ixBOT to ixEOT
     *  Vertex is used to calculate the distance compression ratio
     * 
     * @param ixBOT
     * @param ixEOT
     * @param vertex
     * @return
     */
    private ArrayList<Tuple5<Double,Double,Double,String,String>> buildDistList(int ixBOT, int ixEOT, Position vertex) {
     	double turnDist = pathDistance(ixBOT,ixEOT);
     	double vertexDist = point(ixBOT).position().distanceH(vertex) + point(ixEOT).position().distanceH(vertex);
     	double ratio = vertexDist/turnDist;
     	return buildDistList(ixBOT, ixEOT, ratio);
    }
    
	/**
	 * Structurally revert TCP at ix: (does not depend upon source time or
	 * source position!!) This private method assumes ix &gt; 0 AND ix &lt;
	 * pln.size(). If ix is not a BOT, then nothing is done
	 * 
	 * @param ixBOT    index of point to be reverted
	 * @param addBackMidPoints
	 *            if addBackMidPoints = true, then if there are extra points
	 *            between the BOT and EOT, make sure they are moved to the
	 *            correct place in the new linear sections. Do this by distance
	 *            not time.
	 * @param killNextGsTCPs
	 *            if true, then if there is a BGS-EGS pair after the turn (i.e. creat by TrajGen), then 
	 *            remove these points
	 * @param zVertex
	 *            if non-negative, then assigned reverted vertex this altitude
	 */
	public void structRevertTurnTCP(int ixBOT, boolean addBackMidPoints, boolean killNextGsTCPs) {
		// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT = "+pln.point(ix).isBOT());
		if (!isBOT(ixBOT)) return;
		NavPoint BOT = point(ixBOT);
		int BOTlinIndex = getTcpData(ixBOT).getLinearIndex();
		String name = BOT.label();
		int ixEOT = nextEOT(ixBOT);
		double dist2Mid = pathDistance(ixBOT,ixEOT)/2.0;
		//f.pln(" $$$ structRevertTurnTCP: dist2Mid = "+Units.str("ft",dist2Mid));
		boolean linear = false;
		double tMid = timeFromDistance(ixBOT, dist2Mid);
		//f.pln(" $$$ structRevertTurnTCP: timeFromDistance =  "+timeFromDistance(ixBOT, dist2Mid));
		Position posMid = position(tMid);
		double altMid = posMid.alt();
		//f.pln(" $$$ structRevertTurnTCP: tMid = "+tMid+" altMid = "+Units.str("ft",altMid));
		double gsOutMid = velocity(tMid).gs();
		//f.pln(" $$$ structRevertTurnTCP: gsOutMid = "+Units.str("kn",gsOutMid));
		NavPoint vertex =  vertexPointTurnTcps(ixBOT,ixEOT,altMid);
		//f.pln(" $$$ structRevertTurnTCP: ixMOT = "+ixMOT+" vertex = "+vertex);
		//f.pln(" $$$$$$$ ixBOT = "+ixBOT+" ixEOT = "+ixEOT);
		ArrayList<Tuple5<Double,Double,Double,String,String>> distList = buildDistList(ixBOT, ixEOT, vertex.position());
		// ======================== No Changes To Plan Before This Point ================================
		if (killNextGsTCPs & ixEOT + 1 < size()) {  // remove BGS-EGS pair 1 sec after EOT (created by TrajGen)
			TcpData tcpAfter = getTcpData(ixEOT + 1);
			if (tcpAfter.isBGS() && (tcpAfter.getLinearIndex() == BOTlinIndex)) {
				int ixEGS = nextEGS(ixEOT);
				// f.pln(" $$$$$ let's KILL TWO GS points AT  "+(ixEOT+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
				remove(ixEGS);
				remove(ixEOT + 1);
			}
		}
		double gsInEOT = gsIn(ixEOT);
		double gsOutEOT = gsOut(ixEOT);
		//double gsOutMOT = gsOut(ixMOT);
		double gsOutBOT = gsOut(ixBOT);
		// ========================= Kill all points between ix and ixEOT =============================
		for (int k = ixEOT-1; k > ixBOT; k--) {
			// f.pln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+point(k).toStringFull());
			remove(k);
		}
		addNavPoint(vertex.makeLabel(name)); //TODO: INFO?
		double tmBOT = point(ixBOT).time();
		GsPlan gsp = new GsPlan(tmBOT);
		/*
		 *  ********************************************************************************
		 *  NOTE: this code currently strips out the info field when converting to a gsplan! 
		 *  ********************************************************************************
		 * 
		 */
		if (addBackMidPoints) {		
			boolean vertexAdded = false;
			for (int i = 0; i < distList.size(); i++) {  // reverse order to preserve indexes
				Tuple5<Double,Double,Double,String,String> trip_i = distList.get(i);
				double d_i = trip_i.first;
				double alt_i = trip_i.second;
				double gsIn_i = trip_i.third;
				String label_i = trip_i.fourth;
				String info_i = trip_i.fifth;
				double deltaToMid = Math.abs(d_i - dist2Mid);
				if (deltaToMid < 20) {  // close to where vertex will be (probably MOT)	
					//f.pln(" $$$$$$  GSP SKIP VERTEX i = "+i);
				} else if (!vertexAdded && d_i > deltaToMid) {
					gsp.add(vertex.position(),vertex.label(),"",gsOutMid); //TODO: GEH does this have info?
					//f.pln("  GSP ADD VERTEX i = "+i);
					vertexAdded = true;
				} else {
					linear = true;
					Pair<Position, Integer> adv = advanceDistance2D(ixBOT, d_i, linear);
					Position pos_i = adv.first.mkAlt(alt_i);
					gsp.add(pos_i,label_i,info_i,gsIn_i);
				}
				//f.pln(" $$ structRevertTurnTCP: gsp ADD i = "+i+"  gsIn_i_i = "+Units.str("kn",gsIn_i));
			}
			if (!vertexAdded) {
				//f.pln("  GSP ADD VERTEX i AT END");
				gsp.add(vertex.position(),vertex.label(),"",gsOutMid); //TODO: GEH does this have info?
			}

			//gsp.set(ixMOT-ixBOT,vertex.position(),vertex.label(),gsOutMid); // replace MOT with vertex
		} else {
			gsp.add(point(ixBOT).position(),"","",gsOutBOT); //TODO: GEH does this have info?
			gsp.add(vertex.position(),vertex.label(),"",gsOutMid); //TODO: GEH does this have info?
		}
		//f.pln(" $$$ structRevertTurnTCP: AFTER gsp = "+gsp);
		ixEOT = nextEOT(ixBOT);   // EOT may now have new index
		Plan pMids = gsp.linearPlan();
		getTcpDataRef(ixBOT).clearBOT();
		clearLabel(ixBOT);
		getTcpDataRef(ixEOT).clearEOT();
		// ======================== fix ground speeds ============================
		remove(ixBOT+1);   // remove vertex because it is now in the gsPlan
		for (int j = 1; j < pMids.size(); j++) {  // note we start with 1, i.e. do not add ixBOT
			addNavPoint(pMids.point(j));
		}
		mkGsIn(ixEOT, gsInEOT);
		mkGsOut(ixEOT, gsOutEOT);
		removeIfRedundant(ixEOT);    // remove this one first to preserve indexes
		removeIfRedundant(ixBOT);
		mergeClosePoints();
	}

	/**
	 * Structurally revert TCP at ix: (does not depend upon source time or
	 * source position!!) This private method assumes ix &gt; 0 AND ix &lt;
	 * pln.size(). If ix is not a BOT, then nothing is done
	 * 
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
	public void structRevertTurnTCP_OLD(int ix, boolean addBackMidPoints, boolean killNextGsTCPs) {
		// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT = "+pln.point(ix).isBOT());
		// f.pln(" $$$$$ structRevertTurnTCP: pln = "+pln);
		if (isBOT(ix)) {
			// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix);
			NavPoint BOT = point(ix);
			int BOTlinIndex = getTcpData(ix).getLinearIndex();
			String label = BOT.label();
			double tBOT = BOT.time();
			int ixEOT = nextEOT(ix);
			NavPoint EOT = point(ixEOT);
			double tEOT = EOT.time();
			ArrayList<Pair<NavPoint,TcpData>> betweenPoints = new ArrayList<Pair<NavPoint,TcpData>>(4);
			ArrayList<Double> betweenPointDists = new ArrayList<Double>(4);
			if (addBackMidPoints) { // save add back mid points that are not TCPs
				for (int j = ix + 1; j < ixEOT; j++) {
					Pair<NavPoint,TcpData> np = get(j);
					if ( ! isTCP(j)) {
						// f.pln(" >>>>> structRevertTurnTCP: SAVE MID point("+j+") = "+point(j).toStringFull());
						betweenPoints.add(np);
						double distance_j = pathDistance(ix, j);
						betweenPointDists.add(distance_j);
					}
				}
			}
			Velocity vin;
			if (ix == 0)
				vin = initialVelocity(ix);
			else
				vin = finalVelocity(ix - 1);
			Velocity vout = initialVelocity(ixEOT);			
			NavPoint vertex =  vertexPointTurnTcps(ix,ixEOT,-1);			
			// ======================== No Changes To Plan Before This Point ================================
			double gsInNext = vout.gs();
			if (killNextGsTCPs & ixEOT + 1 < size()) {  // remove BGS-EGS pair 1 sec after EOT (created by TrajGen)
				TcpData tcpAfter = getTcpData(ixEOT + 1);
				if (tcpAfter.isBGS() && (tcpAfter.getLinearIndex() == BOTlinIndex)) {
					int ixEGS = nextEGS(ixEOT);
					vout = initialVelocity(ixEGS);
					gsInNext = vout.gs();
					// f.pln(" $$$$$ let's KILL TWO GS points AT  "+(ixEOT+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
					remove(ixEGS);
					remove(ixEOT + 1);
				}
			}
			// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" ixEOT = "+ixEOT);
			// ------------ Kill all points between ix and ixEOT ----------------
			for (int k = ixEOT; k >= ix; k--) {
				// f.pln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+point(k).toStringFull());
				remove(k);
			}
			// f.pln(" $$$$ structRevertTurnTCP: ADD vertex = "+vertex);
			int ixAdd = addNavPoint(vertex.makeLabel(label)); //TODO: INFO?
			int ixNextPt = ixAdd + 1;
			// ---------- add back all removed points with revised position and time ------
			if (addBackMidPoints) {
				for (int i = 0; i < betweenPointDists.size(); i++) {
					double newTime = BOT.time() + betweenPointDists.get(i) / vin.gs();
					Position newPosition = position(newTime);
					Pair<NavPoint,TcpData> savePt = betweenPoints.get(i);
					NavPoint np = savePt.first.makePosition(newPosition).makeTime(newTime).mkAlt(savePt.first.alt());
					add(np,savePt.second);
					// f.pln(" $$$$ structRevertTurnTCP: ADD BACK np = "+np);
					ixNextPt++;
				}
			}
			// fix ground speed after
			// f.pln(" $$$$ structRevertTurnTCP: ixNextPt = "+ixNextPt+"
			// gsInNext = "+Units.str("kn", gsInNext));
			double tmNextSeg = time(ixNextPt);
			if (tmNextSeg > 0) { // if reverted last point, no need to timeshift
				// points after dSeg
				int newNextSeg = getSegment(tmNextSeg);
				double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
				double dt2 = newNextSegTm - tmNextSeg;
				// f.pln(" $$$$$$$$ structRevertTurnTCP: dt2 = "+dt2);
				timeShiftPlan(newNextSeg, dt2);
			}
			// f.pln(" $$$$ structRevertTurnTCP: initialVelocity("+ixNextPt+") =
			// "+initialVelocity(ixNextPt));
			removeRedundantPoints(getIndex(tBOT), getIndex(tEOT));
		}
	}


	/**
	 * Structurally revert BGS-EGS pair at index "ix"
	 *         Note: it assumes that BGS-EGS pairs will be removed in ascending
	 *         order
	 * 
	 * @param ixBGS   index
	 * @return value of revert gs acceleration
	 */
	public void structRevertGsTCP(int ixBGS, boolean saveAccel) {
		if (isBGS(ixBGS)) {
			//f.pln("\n\n $$$$>>>>>>>>>>>>>>>>>>>>>> structRevertGsTCP:  point("+ix+") = "+point(ix).toString());
			int ixEGS = nextEGS(ixBGS);
			if (ixEGS < 0) {
				//f.pln(" $$$$---------------------- structRevertGsTCP : ERROR nextEGSix = "+nextEGSix);
				addError(" structRevertGsTCP: Ill-formed BGS-EGS structure: no EGS found!");
				return;
			}
			double gsOutEGS = gsOut(ixEGS);
			double gsOutBGS = gsOut(ixBGS);
			//f.pln(" $$$$ structRevertGsTCP: ixBGS = "+ixBGS+" gsOutBGS = "+Units.str("kn",gsOutBGS)+" gsOutEGS = "+Units.str("kn",gsOutEGS));
			TcpData tcpEGS= getTcpDataRef(ixEGS);
			tcpEGS.clearEGS();	
			TcpData tcpBGS = getTcpDataRef(ixBGS);
			tcpBGS.clearBGS();
			if (!saveAccel) tcpBGS.setGsAccel(0.0); 
			mkGsOut(ixBGS, gsOutBGS);       // to make removeIfVsConstant test legitimate
			//f.pln(" $$$$ structRevertGsTCP: AT ixBGS = "+ixBGS+" make gsOutBGS = "+Units.str("kn",gsOutBGS));
			//f.pln("\n $$$$**************** structRevertGsTCP: this = "+this.toStringGs());
			removeIfVsConstant(ixEGS);
			//f.pln(" $$$ structRevertGsTCP: add newPoint = "+newPoint+" iNew =  "+iNew);
			//f.pln(" $$$$ structRevertGsTCP: AT ixBGS = "+ixBGS+" make gsOut = gsOutEGS = "+Units.str("kn",gsOutEGS));
			mkGsOut(ixBGS, gsOutEGS);
		}
	}

	/**
	 * Revert all BGS-EGS pairs
	 * 
	 * @return     reverted plan containing no ground speed TCPS
	 */

	public void revertGsTCPs() {
		int start = 0;
		boolean storeAccel = true;
		revertGsTCPs(start,storeAccel);
	}

	/**
	 * Revert all BGS-EGS pairs in range "start" to "end"
	 * 
	 * @param start  starting index
	 */
	public void revertGsTCPs(int start, boolean storeAccel) {
		//f.pln(" $$$ revertGsTCPs start = "+start+" size = "+size());
		if (start < 0) start = 0;
		for (int j = start; j < size(); j++) {
			//f.pln(" $$$ REVERT GS AT j = "+j+" np_ix = "+point(j));
			structRevertGsTCP(j, storeAccel); 
		}
	}

	
	
	// assumes ix > 0 AND ix < size()
	/**
	 * Revert BVS at ix
	 * 
	 * @param ixBVS    index of a BVS point
	 * @return 
	 */
	double structRevertVsTCP(int ixBVS) {
		if (isBVS(ixBVS)) {
			NavPoint BVS = point(ixBVS);
			int nextEVSix = nextEVS(ixBVS);//fixed
			NavPoint EVS = point(nextEVSix);
			NavPoint pp = point(ixBVS - 1);
			double vsin = (BVS.z() - pp.z()) / (BVS.time() - pp.time());
			double dt = EVS.time() - BVS.time();
			double tVertex = BVS.time() + dt/2.0;
			double zVertex = BVS.z() + vsin*dt/2.0;
			Position pVertex = position(tVertex);
			String label = point(ixBVS).label();
			NavPoint vertex = new NavPoint(pVertex.mkAlt(zVertex), tVertex).makeLabel(label); //TODO: INFO?
			// f.pln(" $$$$ structRevertVsTCP: sourcePos = "+sourcePos+" vertex = "+vertex);
			for (int j = ixBVS+1; j < nextEVSix; j++) {
				double t = time(j);
				double newAlt;
				if (t < tVertex) {
					newAlt = interpolateAlts(t, BVS.time(), BVS.alt(), tVertex, zVertex); 
				} else {  // if (t > tVertex) {
					newAlt = interpolateAlts(t, tVertex, zVertex, EVS.time(), EVS.alt()); 
				}
				NavPoint np_j = point(j).mkAlt(newAlt);
				TcpData  tcp_j = getTcpData(j);
				set(j,np_j,tcp_j);
			}	            
			getTcpDataRef(ixBVS).clearBVS();
			clearLabel(ixBVS);
			getTcpDataRef(nextEVSix).clearEVS();
			removeIfRedundant(nextEVSix);
			removeIfRedundant(ixBVS);				
			int ixVertex = addNavPoint(vertex);
			setAltPreserve(ixVertex);
			mergeClosePoints();
			return zVertex;
		}
		return -1;
	}

	/** Given two vertical points (t1,alt1) and (t2,alt2)  find altitude by linear interpolation at t 
	 * where t1 < t < t2
	 * 
	 * @param t    time point of interest
	 * @param t1
	 * @param alt1
	 * @param t2
	 * @param alt2
	 * @return    interpolated altitude
	 */
	double interpolateAlts(double t, double t1, double alt1, double t2, double alt2) {
		if (t2 <= t1) return -1;
		if (t > t2) t = t2;
		if (t < t1) t = t1;
		double vs = (alt2-alt1)/(t2-t1);
		double dt = t-t1;
		double newAlt = alt1 + dt*vs;
		return newAlt;
	}
	
	// assumes ix > 0 AND ix < size()
	double structRevertVsTCP_OLD(int ix) {
 		if (isBVS(ix)) {
			NavPoint BVS = point(ix);
			int nextEVSix = nextEVS(ix);//fixed
			NavPoint EVS = point(nextEVSix);
	        NavPoint pp = point(ix-1);
			//NavPoint qq = point(nextEVSix+1);
			double vsin = (BVS.z() - pp.z())/(BVS.time() - pp.time());
			double dt = EVS.time() - BVS.time();
			double tVertex = BVS.time() + dt/2.0;
			double zVertex = BVS.z() + vsin*dt/2.0;
			Position pVertex = position(tVertex);
			NavPoint vertex = new NavPoint(pVertex.mkAlt(zVertex),tVertex);
			remove(nextEVSix);
			remove(ix);
			addNavPoint(vertex);
			return zVertex;
		}
		return -1;
	}

	/**
	 * Revert all BVS-EVS pairs
	 * 
	 */
	public void revertVsTCPs() {
		revertVsTCPs(0, size() - 1);
	}

	/**
	 * Revert all BVS-EVS pairs in range "start" to "end"
	 * 
	 * @param start  starting index
	 * @param end    ending index
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
	
	
	public void revertTCP(int ix) {
		if (ix < 0) return;
		if (isBOT(ix)) {
			boolean addBackMidPoints = true;
			boolean killNextGsTCPs = true;
			structRevertTurnTCP(ix, addBackMidPoints, killNextGsTCPs);
		} else if (isBGS(ix)) {
			boolean storeAccel = true;
			structRevertGsTCP(ix,storeAccel);
		} else if (isBVS(ix)) {
			structRevertVsTCP(ix);
		}
	} 
	
	public void revertAllTcps() {
		int nx = nextBeginTCP(0);
		while (nx >= 0) {
			revertTCP(nx);
			nx = nextBeginTCP(0);
			revertTCP(nx);
		}	
	}

	/** revert all TCPS back to its original linear point which have the same sourceTime as the point at index dSeg
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points to
	 *  regain the original ground speed into the first reverted point of the group and all points after it.
	 *  If checkSource is true, this function checks to make sure that the source position is reasonably close to the 
	 *  current position,  and if not, it reverts to the current position of "dSeg".
	 *  
	 *  Note.  This method restores the sourcePosition, but essentially ignores the sourceTimes
	 * 
	 * @param dSeg  The index of one of the TCPs created together that should be reverted
	 * @return index of the point that replaces all the other points
	 */
	public void removeRedundantPoints(int from, int to) {
		int ixLast = Util.min(size() - 2, to);
		int ixFirst = Util.max(1, from);
		for (int i = ixLast; i >= ixFirst; i--) {
			removeIfRedundant(i);
		}
	}
	
	// return ix if removed, -1 otherwise
	public int removeIfRedundant(int ix, boolean trkF, boolean gsF, boolean vsF) {
		//f.pln(" $$$$$ removeIfRedundant: ENTER ix = "+ix);
		if (ix < 0 || ix >= size()-1) return -1;   // should not remove last point 
		Velocity vin = finalVelocity(ix - 1);
		Velocity vout = initialVelocity(ix);
		//f.pln(" $$$$$ removeIfRedundant: ix = "+ix+" vin = "+vin+" vout = "+vout);
		double deltaTrk = Util.turnDelta(vin.trk(),vout.trk());
		double deltaGs = Math.abs(vin.gs()-vout.gs());
		double deltaVs = Math.abs(vin.vs()-vout.vs());
		//f.pln(" $$$$$ removeIfRedundant: ix = "+ix+" deltaTrk = "+ Units.str("deg",deltaTrk)+" deltaGs = "+Units.str("kn",deltaGs));
 		if ( ! isTCP(ix)                                   && 
			 ( ! trkF || deltaTrk < Units.from("deg",1.0)) && 
			 ( ! gsF  || deltaGs < Units.from("kn",5.0))   &&
			 ( ! vsF  || deltaVs < Units.from("fpm",100.0))     ) {
			//f.pln(" $$$$$ removeIfRedundant: REMOVE i = "+ix);
			remove(ix);
			return ix;
		}
 		return -1;
	}
	
	public int removeIfRedundant(int ix) {
		boolean trkF = true;
		boolean gsF = true; 
		boolean vsF = false;
		return removeIfRedundant(ix,trkF, gsF, vsF);
	}
	
	public int removeIfVsConstant(int ix) {
		boolean trkF = true;
		boolean gsF = false; 
		boolean vsF = true;
		return removeIfRedundant(ix,trkF, gsF, vsF);
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
				sb.append("Waypoint " + j + ": " + toStringFull(j));
				sb.append(nl);
			}
		}
		return sb.toString();
	}


	public String toStringFull() {
		return toStringFull(false);		
	}

	/** String representation of the entire plan */
	public String toStringFull(boolean showSource) {
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
				sb.append("Waypoint " + j + ": " + toStringFull(j,showSource));	
				sb.append(nl);

			}
		}
		return sb.toString();
	}


	/** String representation of the entire plan */
	private String toStringV(int velField) {
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
				sb.append(" TCP:"+getTcpData(j).tcpTypeString());
				sb.append(" linearIndex = " + getTcpData(j).getLinearIndex());
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
						if (isAltPreserve(j))
							sb.append(", *AltPreserve*");
						sb.append(",  vsOut: " + Units.str("fpm", vsOut(j), 4));
					}
					if (velField == 3) {
					   sb.append(",  pathDist " + Units.str("NM", pathDistance(0,j), 4));
					}

				}
				sb.append(nl);
			}
		}
		return sb.toString();
	}

	/** String representation of the entire plan */
	public String toStringTrk() {
		return toStringV(0);
	}

	/** String representation of the entire plan */
	public String toStringGs() {
		return toStringV(1);
	}

	/** String representation of the entire plan */
	public String toStringVs() {
		return toStringV(2);
	}
	
	/** String representation of the entire plan */
	public String toStringPd() {
		return toStringV(3);
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
				sb.append(" " + getTcpData(j).tcpTypeString());
				// if (point(j).isBOT() || point(j).isBGS() || point(j).isBVS())
				// sb.append(" vin = "+point(j).velocityIn());
				if (isBOT(j))
					sb.append(" radius = " + Units.str("nm", getTcpData(j).turnRadius()));
				if (isBGS(j))
					sb.append(" gsAccel = " + Units.str("m/s^2", gsAccel(j)));
				if (isBVS(j))
					sb.append(" vsAccel = " + Units.str("m/s^2", vsAccel(j)));
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
			s += "type, tcp_trk, tcp_gs, tcp_vs, radius, accel_gs, accel_vs,  ";
			if (isLatLon()) {
				s += "src_lat, src_lon, src_alt, ";
			} else {
				s += "src_x, src_y, src_z, ";
			}
			s += "src_time, ";
			if (isLatLon()) {
                s += "center_lat, center_lon, center_alt, ";
			} else {
				s += "center_x, center_y, center_z";
			}
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
			if (includeVirtuals || !isVirtual(i)) {
				if (name.length() == 0) {
					sb.append("Aircraft");
				} else {
					sb.append(name);
				}
				sb.append(", ");
				sb.append(toOutput(i,precision, tcpColumns));
				for (int j = 0; j < extraspace; j++) {
					sb.append(", -");
				}
				sb.append(nl);
			}
		}
		return sb.toString();
	}






	public String toString(int i, int precision) {
		NavPoint p = point(i);
		TcpData d = getTcpDataRef(i);
		return p.position().toStringNP(precision) + ", " + f.FmPrecision(p.time(),precision) +" TCP:"+d.tcpTypeString()+" "+f.padRight(p.label(),4);
	}

	public String toStringFull(int i) {
		return toStringFull(i,false);
	}

	public String toStringFull(int i, boolean showSource) {
		NavPoint p = point(i);
		TcpData d = getTcpDataRef(i);
		return toStringFull(p,d,showSource);
	}


	/** Return a string representation that includes additional information */
	public static String toStringFull(NavPoint p, TcpData d) {
		boolean showSource = false;
		return toStringFull(p,d,showSource);
	}

	/** Return a string representation that includes additional information */
	public static String toStringFull(NavPoint p, TcpData d, boolean showSource) {	
		StringBuffer sb = new StringBuffer(100);
		sb.append("[(");
		if (p.isLatLon()) sb.append("LL: ");
		sb.append(p.toStringShort(4));
		sb.append("), ");
		sb.append(d.getTypeString());
		if (d.isTrkTCP()) {
			sb.append(", "+d.getTrkTypeString());
			//		if (isBOT()) {
			//			sb.append(" signedRadius = "+f.Fm4(Units.to("NM", radius)));
			//		}
			//			if (d.isBOT()) {
			//				sb.append(" accTrk = "+f.Fm4(Units.to("deg/s", d.trkAccel())));
			//			}
		}
		if (d.isGsTCP()) {
			sb.append(", "+d.getGsTypeString());
			if (d.isBGS()) {
				sb.append(" accGs = "+f.Fm4(Units.to("m/s^2", d.getGsAccel())));
			}
		}
		if (d.isVsTCP()) {
			sb.append(", "+d.getVsTypeString());
			if (d.isBVS()) {
				sb.append(" accVs = "+f.Fm4(Units.to("m/s^2", d.getVsAccel())));
			}
		}
		//f.pln(" $$ toStringFull: d = "+d);
		//		if ( ! d.getVelocityInit().isInvalid()) sb.append(" vin = "+d.getVelocityInit().toStringUnits());
		if (!Util.almost_equals(d.getRadiusSigned(),0.0)) {
			sb.append(" sgnRadius = "+f.Fm4(Units.to("NM",d.getRadiusSigned())));
			Position center = d.turnCenter();
			if (!center.isInvalid()) sb.append(" center = "+center.toString2D(4));
		}
		if (showSource) {
			if (d.getSourceTime() >= 0) {
				sb.append(" srcTime = "+f.Fm2(d.getSourceTime()));
				if (!d.getSourcePosition().isInvalid()) {
					sb.append(" srcPos = "+d.getSourcePosition().toStringUnits()); 		
				}
			}
		}
		//if (d.isAltPreserve()) sb.append(" <*AltPre*> ");
		sb.append("<"+d.getLinearIndex()+">");
		sb.append("]");
		sb.append(" "+p.label()+" "+d.getInformation());
		return sb.toString();
	}

	/**
	 * Return a list of string representing each field (position, time, etc.).  (tcpinfo + label) 
	 * This DOES include the the Plan name as the first field 
	 * @param i
	 * @param precision
	 * @param tcp
	 * @return
	 */
	public List<String> toStringList(int i, int precision, boolean tcp) {
		List<String> ret = new ArrayList<String>(TcpData.TCP_OUTPUT_COLUMNS + 1);
		ret.add(name);
		ret.addAll(toStringList(point(i), getTcpDataRef(i), precision, tcp));
		return ret;
	}

	/** 
	 * Return a list of string representing each field (position, time, etc.).  (tcpinfo + label) 
	 * This does NOT include the Plan name as the first field.
	 * */
	public static List<String> toStringList(NavPoint p, TcpData d, int precision, boolean tcp) {
		//NOTE:  update TCP_OUTPUT_COLUMNS if this changes!!!

		ArrayList<String> ret = new ArrayList<String>(10);  // name is (0) -- not counted in TCP_OUTPUT_COLUMNS
		ret.addAll(p.position().toStringList(precision)); // position (1-3)
		ret.add(f.FmPrecision(p.time(),precision)); // time (4)
		if (tcp) {
			ret.add(d.getTypeString().toString()); // type (string) (5)
			ret.add(d.getTrkTypeString()); // tcp trk (string) (6)
			ret.add(d.getGsTypeString()); // tcp gs (string) (7)
			ret.add(d.getVsTypeString()); // tcp vs (string) (8)
			ret.add(f.FmPrecision(Units.to("NM", d.getRadiusSigned()), precision)); // radius (9)
			ret.add(f.FmPrecision(Units.to("m/s^2", d.getGsAccel()),precision)); // gs accel (10)
			ret.add(f.FmPrecision(Units.to("m/s^2", d.getVsAccel()),precision)); // vs accel (11)
			ret.addAll(d.getSourcePosition().toStringList(precision)); // source position (12-14)
			ret.add(f.FmPrecision(d.getSourceTime(),precision)); // source time (15)
			ret.addAll(d.turnCenter().toStringList(precision)); // turn center (16-18)
			if (d.getInformation().length() > 0) {
				ret.add(d.getInformation()); // info (string) (19)
			} else {
				ret.add("-");
			}
			if (p.label().length() > 0) {
				ret.add(p.label()); // label (string) (20)
			} else {
				ret.add("-");
			}
		} else {
			String fl = TcpData.fullLabel(p,d); 
			if (fl.length() > 0) {
				ret.add(fl); // label (string) (5)
			} else {
				ret.add("-");
			}
		}
		return ret;
	}


	/**
	 * This collapses any metadata into the label field
	 */
	public String toOutput(int i) {
		return toOutput(i, Constants.get_output_precision(),false);
	}

	/** Return a string representation that is consistent with the PathReader input format. 
	 *  if tcp is true, explicily include all metadata as distinct columns.
	 *  if tcp is false, collapse metadata, if any, into the "label" field 
	 */
	public String toOutput(int i, int precision, boolean tcp) {
		NavPoint p = point(i);
		TcpData d = getTcpDataRef(i);

		return toOutput(p,d,precision, tcp);
	}


	public static String toOutput(NavPoint p, TcpData d, int precision, boolean tcp) {
		return f.list2str(toStringList(p,d,precision,tcp),", ");
	}

	/** Return a string including both the point name (if set) and any tcp metadata (if any).  If neither exists, return the empty string. */
	public String fullLabel(int i) {
		NavPoint p = point(i);
		TcpData d = getTcpDataRef(i);
		return TcpData.fullLabel(p,d);
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
			base.add(TcpData.parseLL(np));
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
			base.add(TcpData.parseXYZ(s));
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

/*
 * NavPoint 
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

//import gov.nasa.larcfm.IO.DebugSupport;


/** A position at a time.  This class is immutable.
 * Possible types of points.  
 * "Orig" are from the original plan with no change
 * "Virtual" are temporary points, generally interpolated points (long legs are broken into segments).  Virtual points will be silently over-written, and never register as overlapping with other points.  Virtual points may be deleted unexpectedly.  Not for general use.
 * "Added" are new points added by strategies.
 * "Modified" are original points that have been moved.
 * "AltPreserve" are marked points used in trajectory generation
 * 	Virtual points are internal to Stratway and not
 * expected to be returned to the pilot or plane's automation.
 */
public class NavPoint {
	public static enum WayType     {Orig, Virtual, AltPreserve};
	public static enum Trk_TCPType {NONE, BOT, EOT, EOTBOT};
	public static enum Gs_TCPType  {NONE, BGS, EGS, EGSBGS};
	public static enum Vs_TCPType  {NONE, BVS, EVS, EVSBVS}
	static boolean     debug = false;  // TODO: is this necessary? should we just get rid of it?

	public static final int TCP_OUTPUT_COLUMNS = 20; // total number of output columns for full TCP (not including aircraft name)
	public static final int MIN_OUTPUT_COLUMNS = 5; // total number of output columns for linear (not including aircraft name)

	private final Position p;                   // 3D position (XYZ or LatLonAlt)
	private final double   t;                   // time of point
	private final WayType  ty;                  // type of point
	private final String   label;               // label

	// TCP META DATA: 
	private final Trk_TCPType tcp_trk;	        // 
	private final Gs_TCPType  tcp_gs;		    // 
	private final Vs_TCPType  tcp_vs;		    // 
	private final double   radiusSigned;		    // signed turn radius, positive values indicate right turn, negative values indicate left turn, zero indicates no specified radius
	private final double   accel_gs;            // signed gs-acceleration value
	private final double   accel_vs;            // signed vs-acceleration value
	private final Velocity velocityInit;		    // velocity in to the acceleration zone.  
	private final Position sourcePosition;      // source position
	private final double   sourceTime;		    // source time (if < 0, no valid source)
	private final int      linearIndex;	        // in a kinematic plan provides index to original linear plan
	
	/** A zero in Latitude/Longitude */
	public static final NavPoint ZERO_LL  = new NavPoint(Position.ZERO_LL, 0.0, "ZERO_LL");
	/** A zero in Euclidean space */
	public static final NavPoint ZERO_XYZ = new NavPoint(Position.ZERO_XYZ, 0.0, "ZERO_XYZ");  
	/** An invalid NavPoint.  Note that this is not necessarily equal to other invalid NavPoints -- use the isInvalid() test instead. */
	public static final NavPoint INVALID  = new NavPoint(Position.INVALID, Double.NaN, "INVALID");

	private NavPoint(Position p, double t, WayType ty, String label,
			Trk_TCPType tcp_trk, Gs_TCPType tcp_gs, Vs_TCPType tcp_vs,
			double signedRadius, double accel_gs, double accel_vs,
			Velocity velocityIn, Position sourcePosition, double sourceTime, int linearIndex) {
		super();
		this.p = p;
		this.t = t;
		this.ty = ty;
		this.label = label;
		this.tcp_trk = tcp_trk;
		this.tcp_gs = tcp_gs;
		this.tcp_vs = tcp_vs;
		//this.accel_trk = accel_trk;
		this.radiusSigned = signedRadius;
		this.accel_gs = accel_gs;
		this.accel_vs = accel_vs;
		this.velocityInit = velocityIn;
		this.sourcePosition = sourcePosition;
		this.sourceTime = sourceTime;
		this.linearIndex = linearIndex;
		//this.minorV = minorV;
	}



	/**
	 * Construction a non-TCP NavPoint piecemeal
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 */
	public NavPoint(Position p, double t, String label) {
		super();
		this.p = p;
		this.t = t;
		this.ty = WayType.Orig;
		this.label = label;
		this.tcp_trk = Trk_TCPType.NONE;
		this.tcp_gs = Gs_TCPType.NONE;
		this.tcp_vs = Vs_TCPType.NONE;
		//this.accel_trk = 0.0;
		this.accel_gs =  0.0;
		this.accel_vs =  0.0;
		this.velocityInit = Velocity.INVALID;
		this.sourceTime = t;
		this.sourcePosition = p;
		this.radiusSigned = 0;
		this.linearIndex = -1;
		//this.minorV = 0;
	}

	/**
	 * Construct a new NavPoint from a position and a time.
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 */
	public NavPoint(Position p, double t) {
		super();
		this.p = p;
		this.t = t;
		this.ty = WayType.Orig;
		this.label = "";
		this.tcp_trk = Trk_TCPType.NONE;
		this.tcp_gs = Gs_TCPType.NONE;
		this.tcp_vs = Vs_TCPType.NONE;
//		this.accel_trk = 0.0;
		this.accel_gs =  0.0;
		this.accel_vs =  0.0;
		this.velocityInit = Velocity.INVALID;
		this.sourceTime = t;
		this.sourcePosition = p;
		this.radiusSigned = 0;
		this.linearIndex = -1;
		//this.minorV = 0;
	}


	public static NavPoint makeFull(Position p, double t, WayType ty, String label,
			Trk_TCPType tcp_trk, Gs_TCPType tcp_gs, Vs_TCPType tcp_vs,
			double sgnRadius, double accel_gs, double accel_vs,
			Velocity velocityIn, Position sourcePosition, double sourceTime) {
		return new NavPoint(p, t, ty, label, tcp_trk, tcp_gs, tcp_vs, sgnRadius,  accel_gs,  accel_vs,  velocityIn, sourcePosition,  sourceTime, -1);
	}



	/**
	 * Creates a new lat/lon NavPoint with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude (radians)
	 * @param lon longitude (radians)
	 * @param alt altitude [m]
	 * @param t   time
	 */
	public static NavPoint mkLatLonAlt(double lat, double lon, double alt, double t) {
		return new NavPoint(Position.mkLatLonAlt(lat,lon,alt), t);
	}
	
	
	/**
	 * Creates a new lat/lon NavPoint with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude [deg north latitude]
	 * @param lon longitude [deg east longitude]
	 * @param alt altitude [ft]
	 * @param t   time
	 */
	public static NavPoint makeLatLonAlt(double lat, double lon, double alt, double t) {
		return new NavPoint(Position.makeLatLonAlt(lat,lon,alt), t);
	}


	/**
	 * Creates a new Euclidean NavPoint with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
	 * 
	 * @param x coordinate [nmi]
	 * @param y coordinate [nmi]
	 * @param z altitude [ft]
	 * @param t time
	 */
	public static NavPoint makeXYZ(double x, double y, double z, double t) {
		return new NavPoint(Position.makeXYZ(x, y, z), t);
	}

	public static NavPoint mkXYZ(double x, double y, double z, double t) {
		return new NavPoint(Position.mkXYZ(x, y, z), t);
	}



	//	//TODO: Is this the right implementation? If two NavPoints have the same position but different labels, they are the same.  Hmmm.
	//	@Override
	//	public boolean equals(Object obj) {
	//		if (this == obj)
	//			return true;
	//		if (obj == null)
	//			return false;
	//		if (getClass() != obj.getClass())
	//			return false;
	//		NavPoint other = (NavPoint) obj;
	//		if (p == null) {
	//			if (other.p != null)
	//				return false;
	//		} else if (!p.equals(other.p))
	//			return false;
	//		if (Double.doubleToLongBits(t) != Double.doubleToLongBits(other.t))
	//			return false;
	//		if (ty != other.ty)
	//			return false;
	//		return true;
	//	}



	/**
	 * Returns true if any components of this NavPoint are marked as Invalid
	 */
	public boolean isInvalid() {
		return p.isInvalid() || Double.isNaN(t);
	}



	/**
	 * Can this point be merged with the given point p?  Merged points only apply to TCP points.
	 * @param point the other point
	 * @return true, if the points can be merged.
	 */
	public boolean mergeable(NavPoint point) {
		//f.pln(" $$ mergeable this = "+this.toStringFull()+" p = "+p.toStringFull());
		boolean r1 = (this.tcp_trk == Trk_TCPType.NONE || point.tcp_trk == Trk_TCPType.NONE) 
				|| (this.tcp_trk == Trk_TCPType.BOT && point.tcp_trk == Trk_TCPType.EOT) 
				|| (this.tcp_trk == Trk_TCPType.EOT && point.tcp_trk == Trk_TCPType.BOT);
		boolean r2 = r1 && ((this.tcp_gs == Gs_TCPType.NONE || point.tcp_gs == Gs_TCPType.NONE) 
				|| (this.tcp_gs == Gs_TCPType.BGS && point.tcp_gs == Gs_TCPType.EGS) 
				|| (this.tcp_gs == Gs_TCPType.EGS && point.tcp_gs == Gs_TCPType.BGS));
		boolean r3 = r2 && ((this.tcp_vs == Vs_TCPType.NONE || point.tcp_vs == Vs_TCPType.NONE) 
				|| (this.tcp_vs == Vs_TCPType.BVS && point.tcp_vs == Vs_TCPType.EVS) 
				|| (this.tcp_vs == Vs_TCPType.EVS && point.tcp_vs == Vs_TCPType.BVS));
		return r3 && this.t == point.t && this.p.equals(point.p) && (this.velocityInit.isInvalid() || point.velocityInit.isInvalid() ||  this.velocityInit.equals(point.velocityInit));
	}


	/**
	 * Creates a new point that is the merger of the this point and the given 
	 * point.  Assumes that mergeable() on the two points would return true, if 
	 * not, then approximately the original point is returned. <p>
	 * 
	 * Merging rules:
	 * <ul>
	 * <li> mergeTCPInfo() is commutative
	 * <li> If both points are linear points, then the points are merged.
	 * <li> If one point is a linear point (aka, not a TCP), then the TCP info (velocity and acceleration) 
	 * from the TCP point is used.
	 * <li> If both points are TCP (aka acceleration points) of different types (Trk, Gs, Vs), then the points
	 * should merge without an issue, note: they are required to have the same "velocity in" 
	 * <li> If both points are TCP (aka acceleration points) of the same type (Trk, Gs, or Vs), then the 
	 * resulting point should be a combined point (e.g., EOTBOT), and the TCP information should be the information 
	 * from the beginning point.
	 * <li> If both points are TCP "begin" points, then a message is provided if they have different source positions
	 * </ul> 
	 * 
	 * @param point the other point
	 * @return a new point that is the merger of the two points.
	 */
	public NavPoint mergeTCPInfo(NavPoint point) {
		// position & time -- keep either (should be the same)

		WayType ty = (this.ty == WayType.AltPreserve || point.ty == WayType.AltPreserve) ? WayType.AltPreserve : 
			((this.ty == WayType.Orig || point.ty == WayType.Orig) ? WayType.Orig : WayType.Virtual);
		String label = this.label+point.label;
		Trk_TCPType tcp_trk = ((this.tcp_trk == Trk_TCPType.BOT && point.tcp_trk == Trk_TCPType.EOT) 
				|| (this.tcp_trk == Trk_TCPType.EOT && point.tcp_trk == Trk_TCPType.BOT)) ? Trk_TCPType.EOTBOT : 
					((this.tcp_trk == Trk_TCPType.BOT || point.tcp_trk == Trk_TCPType.BOT) ? Trk_TCPType.BOT : 
						((this.tcp_trk == Trk_TCPType.EOT || point.tcp_trk == Trk_TCPType.EOT) ? Trk_TCPType.EOT : 
							((this.tcp_trk == Trk_TCPType.EOTBOT || point.tcp_trk == Trk_TCPType.EOTBOT) ? Trk_TCPType.EOTBOT : Trk_TCPType.NONE)));
		Gs_TCPType tcp_gs = ((this.tcp_gs == Gs_TCPType.BGS && point.tcp_gs == Gs_TCPType.EGS) 
				|| (this.tcp_gs == Gs_TCPType.EGS && point.tcp_gs == Gs_TCPType.BGS)) ? Gs_TCPType.EGSBGS : 
					((this.tcp_gs == Gs_TCPType.BGS || point.tcp_gs == Gs_TCPType.BGS) ? Gs_TCPType.BGS : 
						((this.tcp_gs == Gs_TCPType.EGS || point.tcp_gs == Gs_TCPType.EGS) ? Gs_TCPType.EGS : 
							((this.tcp_gs == Gs_TCPType.EGSBGS || point.tcp_gs == Gs_TCPType.EGSBGS) ? Gs_TCPType.EGSBGS : Gs_TCPType.NONE)));
		Vs_TCPType tcp_vs = ((this.tcp_vs == Vs_TCPType.BVS && point.tcp_vs == Vs_TCPType.EVS) 
				|| (this.tcp_vs == Vs_TCPType.EVS && point.tcp_vs == Vs_TCPType.BVS)) ? Vs_TCPType.EVSBVS : 
					((this.tcp_vs == Vs_TCPType.BVS || point.tcp_vs == Vs_TCPType.BVS) ? Vs_TCPType.BVS : 
						((this.tcp_vs == Vs_TCPType.EVS || point.tcp_vs == Vs_TCPType.EVS) ? Vs_TCPType.EVS : 
							((this.tcp_vs == Vs_TCPType.EVSBVS || point.tcp_vs == Vs_TCPType.EVSBVS) ? Vs_TCPType.EVSBVS : Vs_TCPType.NONE)));
//		double accel_trk = (this.tcp_trk == Trk_TCPType.BOT || this.tcp_trk == Trk_TCPType.EOT || this.tcp_trk == Trk_TCPType.EOTBOT) ? this.accel_trk : point.accel_trk; 
		double radius = 0.0; // (this.tcp_trk != Trk_TCPType.NONE || point.tcp_trk != Trk_TCPType.NONE) ? 0 : this.sgnRadius;
		if (this.tcp_trk == Trk_TCPType.BOT || this.tcp_trk == Trk_TCPType.EOTBOT) radius = this.radiusSigned;
		else radius = point.radiusSigned;
		
		
		double accel_gs = (this.tcp_gs == Gs_TCPType.BGS || this.tcp_gs == Gs_TCPType.EGSBGS) ? this.accel_gs : point.accel_gs; 
		double accel_vs = (this.tcp_vs == Vs_TCPType.BVS || this.tcp_vs == Vs_TCPType.EVSBVS) ? this.accel_vs : point.accel_vs; 
		Velocity velocityIn = (this.isBeginTCP()) ? this.velocityInit : point.velocityInit; 
		Position sourcePosition = (this.isBeginTCP()) ? this.sourcePosition : point.sourcePosition; 

		double sourceTime = (this.isBeginTCP()) ? this.sourceTime : point.sourceTime; 
		int linearIndex = (this.isBeginTCP()) ? this.linearIndex : point.linearIndex; 

		return new NavPoint(this.p, this.t, ty, label, tcp_trk, tcp_gs, tcp_vs,
				radius, accel_gs, accel_vs, velocityIn, sourcePosition, sourceTime, linearIndex);

		//		
		//		
		//		// original code
		//		if (this.tcp_trk == Trk_TCPType.BOT && point.tcp_trk == Trk_TCPType.EOT) { // add end
		//			np = np.makeTrkTCP(Trk_TCPType.EOTBOT);
		//		} else if (this.tcp_trk == Trk_TCPType.EOT && point.tcp_trk == Trk_TCPType.BOT) { // start
		//			np = np.makeTrkTCP(Trk_TCPType.EOTBOT).makeTrkAccel(point.accel_trk).makeSource(point.sourcePosition,point.sourceTime);
		//		} else if (point.tcp_trk != Trk_TCPType.NONE) { // general case
		//			np = np.makeTrkTCP(point.tcp_trk).makeTrkAccel(point.accel_trk).makeSource(point.sourcePosition,point.sourceTime);
		//		}
		//		
		//		if (this.tcp_gs == Gs_TCPType.BGS && point.tcp_gs == Gs_TCPType.EGS) { // add end
		//			np = np.makeGsTCP(Gs_TCPType.EGSBGS);
		//		} else if (this.tcp_gs == Gs_TCPType.EGS && point.tcp_gs == Gs_TCPType.BGS) { // start
		//			np = np.makeGsTCP(Gs_TCPType.EGSBGS).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition,point.sourceTime);
		//		} else if (point.tcp_gs != Gs_TCPType.NONE) { // general case
		//			np = np.makeGsTCP(point.tcp_gs).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition,point.sourceTime);
		//		}
		//
		//		if (this.tcp_vs == Vs_TCPType.BVS && point.tcp_vs == Vs_TCPType.EVS) { // add end
		//			np = np.makeVsTCP(Vs_TCPType.EVSBVS);
		//		} else if (this.tcp_vs == Vs_TCPType.EVS && point.tcp_vs == Vs_TCPType.BVS) {  // start
		//			np = np.makeVsTCP(Vs_TCPType.EVSBVS).makeGsAccel(point.accel_gs).makeSource(point.sourcePosition,point.sourceTime);
		//		} else if (point.tcp_vs != Vs_TCPType.NONE) { // general case
		//			np = np.makeVsTCP(point.tcp_vs).makeVsAccel(point.accel_vs).makeSource(point.sourcePosition,point.sourceTime);
		//		}
		//		
		//		if (this.velocityIn.isInvalid()) {
		//			np = np.makeVelocityIn(point.velocityIn);
		//		}
		//		
		//		if (this.tcp_trk == Trk_TCPType.BOT && point.tcp_vs == Vs_TCPType.BVS ||  // add warning
		//			this.tcp_vs == Vs_TCPType.BVS && point.tcp_trk ==  Trk_TCPType.BOT) {
		//			f.pln(" $$$$!!!!!!!!!!!!! Warning: this point:"+np+" may not properly revert via removeTCPS");
		//		}
		//		
		//		np = np.appendLabel(point.label);
		//        return np;		
	}


	/**
	 * Checks if two NavPoint are almost the same.
	 * 
	 * @param v NavPoint for comparison
	 * 
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 */
	public boolean almostEquals(NavPoint v) {
		return Constants.almost_equals_time(t,v.t) 
				&& p.almostEquals(v.p);
	}

	/**
	 * Checks if two NavPoint are almost the same in position.
	 * 
	 * @param v NavPoint for comparison
	 * 
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 */
	public boolean almostEqualsPosition(NavPoint v, double epsilon_horiz, double epsilon_vert) {
		return p.almostEquals(v.p, epsilon_horiz, epsilon_vert);
	}

	/**
	 * Checks if two NavPoint are almost the same in position.
	 * 
	 * @param v NavPoint for comparison
	 * 
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 */
	public boolean almostEqualsPosition(NavPoint v) {
		return p.almostEquals(v.p);
	}


	/** Return the horizontal position vector */
	public Vect2 vect2() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in vect2()");
		}
		return p.vect2();
	}

	/** Return the three dimensional position vector */
	public Point point() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in point()");
		}
		return p.point();
	}

	/** Return the LatLonAlt object */
	public LatLonAlt lla() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in lla()");
		}
		return p.lla();
	}

	/** Return the Position object */
	public Position position() {
		return p;
	}

	/** Return the time [s] */
	public double time() {
		return t;
	}


	/** Is this point specified in lat/lon? */
	public boolean isLatLon() {
		return p.isLatLon();
	}

	/** Is this point Virtual? */
	public boolean isVirtual() {
		return ty == WayType.Virtual;
	}

	/** Returns true if the point at index i is an unmodified original point, 
	   false otherwise.  0 <= i < size() */
	public boolean isOriginal() {
		return ty == WayType.Orig;
	}

	/** Returns true if the point at index i is a modified point, 
	   false otherwise.  0 <= i < size() */
	public boolean isAltPreserve() {
		return ty == WayType.AltPreserve;
	}

	/**
	 * Is this an "added" point?
	 */
	public boolean isAdded() {
		return sourceTime < 0.0;
	}

	//	private static final DecimalFormat frm2 = new DecimalFormat("+00;-00");
	//	private static final DecimalFormat frm3 = new DecimalFormat("000");

	/** Return the label of this point, if set.  If no name has been set, then return the empty string. */
	public String label() {
		String name = "";
		if (label != null) {
			name = label;
		}
		return name;
	}

	/** Return a string including both the point name (if set) and any tcp metadata (if any).  If neither exists, return the empty string. */
	public String fullLabel() {
		return label()+metaDataLabel(4);
	}

	/** Returns true if the "label" label of this NavPoint has been set. */
	public boolean isLabelSet() {
		if (label == null) {
			return false;
		}
		return !label.equals("");
	}

	/** Return the latitude */
	public double lat() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in lat()");
		}
		return p.lat();
	}
	/** Return the longitude */
	public double lon() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in lon()");
		}
		return p.lon();
	}

	/** Return the altitude */
	public double alt() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in alt()");
		}
		return p.alt();
	}
	/** Return the x coordinate */
	public double x() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in x()");
		}
		return p.x();
	}

	/** Return the y coordinate */
	public double y() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in y()");
		}
		return p.y();
	}

	/** Return the z coordinate */
	public double z() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in z()");
		}
		return p.z();
	}

//	/**
//	 * 
//	 * @return the radius field, if 0 => not applicable.
//	 */
//	public double getRadius() {
//		return sgnRadius;
//	}
	
	/**
	 * This returns the radius of the current turn.  If this is not a turn point, it returns the stores radius value.  
	 * If the associated acceleration is 0.0, this returns a radius of zero.
	 */
	public double turnRadius() {
		return Math.abs(radiusSigned);
//		if (tcp_trk == Trk_TCPType.NONE) {
//			return radius;
//		} else if (isTrkTCP() && accel_trk != 0.0) {
//			return Math.abs(velocityIn.gs()/accel_trk);
//		}
//		return 0.0;
	}

	
	/**
	 * This returns the radius of the current turn.  If this is not a turn point, it returns the stores radius value.  
	 * If the associated acceleration is 0.0, this returns a radius of zero.
	 */
	public double signedRadius() {
		return radiusSigned;
	}

	/**
	 * This method returns a center of turn position with the same altitude as the current point.  If the current point is not a turn point, and has a zero stored radius,
	 * this returns an invalid position. 
	 */
	public Position turnCenter() {
		double R = signedRadius();
//		if (isTrkTCP()) {
//			if (R > 0.0) {
//				return p.linear(velocityIn.mkAddTrk(Util.sign(R)*Math.PI/2).Hat(),R).mkZ(p.z());
//			}
//		} else 
		if (R != 0) {
			Velocity vHat = velocityInit.mkAddTrk(Util.sign(R)*Math.PI/2).Hat2D();
			return p.linear(vHat, Math.abs(R)).mkZ(p.z());			
		}
		return Position.INVALID;
	}

	/** Source time of point this was based on for any type except BVS. */
	//@Deprecated
	public double sourceTime() {
		return sourceTime;
	}

	/** Source time of point this was based on for any type except BVS. */
	//@Deprecated
	public Position sourcePosition() {
		return sourcePosition;
	}
	
	/** linear index
	 * 
	 * @return
	 */
	public int linearIndex() {
		return linearIndex;
	}


	public NavPoint sourceNavPoint() {
		return new NavPoint(sourcePosition,sourceTime);
	}

	public boolean hasSource() {
		return sourceTime >= 0;
	}

	/**
	 * If this is a TCP, return the velocity in metadata.  Otherwise return an invalid velocity.
	 * Not that this value is only well-defined for two case: the beginning of acceleration points (BGSC, BVSC, BOT) and points with a fixed gs mutability (where trk and vs may still not be well-defined)
	 */
	public Velocity velocityInit() {
		return velocityInit;
	}

	/**
	 * Return the  (signed) turn rate (i.e., position 
	 * acceleration in the "track" dimension) associated with this point. Turn rate is in rad/sec (positive 
	 * is clockwise/right, negative is counterclockwise/left), otherwise return 0.0; 
	 */
	public double trkAccel() {
		//return accel_trk;
		double rtn = 0.0;
		if (Math.abs(radiusSigned) > 0) {
		    rtn = velocityInit.gs()/radiusSigned;
		} 
		//f.pln(" $$$ trkAccel: radiusSigned = "+Units.str("NM",radiusSigned)+" rtn = "+Units.str("deg/s",rtn));
		return rtn;
	}

	/**
	 * Return the associated (signed) ground speed rate of change (i.e, horizontal acceleration).
	 */
	public double gsAccel() {
		return accel_gs;
	}

	/**
	 * Return the associated (signed) vertical acceleration.
	 */
	public double vsAccel() {
		return accel_vs;
	}

	/** true if this point is a TCP */
	public boolean isTCP() {
		return tcp_trk != Trk_TCPType.NONE || tcp_gs != Gs_TCPType.NONE || tcp_vs != Vs_TCPType.NONE;
	}

	/** true if this point is part of a turn */
	public boolean isTrkTCP() {
		return tcp_trk != Trk_TCPType.NONE;
	}


	/** true if this point is the start of a turn */
	public boolean isBOT() {
		return tcp_trk == Trk_TCPType.BOT || tcp_trk == Trk_TCPType.EOTBOT;
	}

	/** true if this point is the end of a turn */
	public boolean isEOT() {
		return tcp_trk == Trk_TCPType.EOT || tcp_trk == Trk_TCPType.EOTBOT;
	}

	public boolean isGsTCP() {
		return tcp_gs != Gs_TCPType.NONE;
	}

	public boolean isBGS() {
		return tcp_gs == Gs_TCPType.BGS || tcp_gs == Gs_TCPType.EGSBGS;
	}

	public boolean isEGS() {
		return tcp_gs == Gs_TCPType.EGS || tcp_gs == Gs_TCPType.EGSBGS;
	}

	/** true if this point is part of a vs change */
	public boolean isVsTCP() {
		return tcp_vs != Vs_TCPType.NONE;
	}

	/** true if this point is the start of a vs change */
	public boolean isBVS() {
		return tcp_vs == Vs_TCPType.BVS || tcp_vs == Vs_TCPType.EVSBVS;
	}

	/** true if this point is the end of a vs change */
	public boolean isEVS() {
		return tcp_vs == Vs_TCPType.EVS || tcp_vs == Vs_TCPType.EVSBVS;
	}

	public boolean isBeginTCP() {
		return tcp_trk == Trk_TCPType.BOT || tcp_gs == Gs_TCPType.BGS || tcp_vs == Vs_TCPType.BVS ||
				tcp_trk == Trk_TCPType.EOTBOT || tcp_gs == Gs_TCPType.EGSBGS || tcp_vs == Vs_TCPType.EVSBVS;
	}

	public boolean isEndTCP() {
		return tcp_trk == Trk_TCPType.EOT || tcp_gs == Gs_TCPType.EGS || tcp_vs == Vs_TCPType.EVS ||
				tcp_trk == Trk_TCPType.EOTBOT || tcp_gs == Gs_TCPType.EGSBGS || tcp_vs == Vs_TCPType.EVSBVS;
	}

	//
	// Make new points from old ones
	//

	private NavPoint copy(Position p) {
		//		if (p.equals(this.p)) return this;
		return new NavPoint(p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit,  this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	private NavPoint copy(WayType ty) {
		//		if (ty == this.ty) return this;
		return new NavPoint(this.p, this.t, ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);

	}

	/** Make a new NavPoint from the current one with the latitude changed */
	public NavPoint mkLat(double lat) {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeLat()");
		}
		return copy(p.mkLat(lat));
	}

	/** Make a new NavPoint from the current one with the X coordinate changed */
	public NavPoint mkX(double x) {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeX()");
		}
		return copy(p.mkX(x));
	}

	/** Make a new NavPoint from the current one with the longitude changed */
	public NavPoint mkLon(double lon) {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeLon()");
		}
		return copy(p.mkLon(lon));
	}

	/** Make a new NavPoint from the current one with the Y coordinate changed */
	public NavPoint mkY(double y) {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeY()");
		}
		return copy(p.mkY(y));
	}

	/** Make a new NavPoint from the current one with the altitude changed */
	public NavPoint mkAlt(double alt) {
		return copy(p.mkAlt(alt));
	}

	/** Make a new NavPoint from the current one with the Z coordinate changed */
	public NavPoint mkZ(double z) {
		return copy(p.mkZ(z));
	}

	/** Make a new NavPoint from the current one with the time changed, source time is not changed */
	public NavPoint makeTime(double time) {
		//		if (time == this.t) return this;
		return new NavPoint(this.p, time, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the time changed, source time is the new time */
	public NavPoint makeTimeNew(double time) {
		//		if (time == this.t) return this;
		return new NavPoint(this.p, time, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, time, this.linearIndex);
	}


	/** Make a new NavPoint from the current one with the TCP label metadata changed */
	public NavPoint makeTrkTCP(Trk_TCPType trktype) {
		//		if (trktype == this.tcp_trk) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, trktype,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the TCP label metadata changed */
	public NavPoint makeGsTCP(Gs_TCPType gstype) {
		//		if (gstype == this.tcp_gs) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  gstype, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the TCP label metadata changed */
	public NavPoint makeVsTCP(Vs_TCPType vstype) {
		//		if (vstype == this.tcp_vs) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, vstype,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the source time metadata changed */
	public NavPoint makeSource(Position sp, double st) {
		//		if (st == this.sourceTime && sp.equals(this.sourcePosition)) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, sp, st, this.linearIndex);
	}

	public NavPoint makeSourcePosition(Position sp) {
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, sp, this.sourceTime, this.linearIndex);
	}

	public NavPoint makeSourceTime(double st) {
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, st, this.linearIndex);
	}

	public NavPoint makeRadius(double r) {
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				r, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);	  
	}
	
	public NavPoint makeLinearIndex(int ix) {
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, ix);	  
	}

	/**
	 * Make an "added" point that does not include valid source info
	 */
	public NavPoint makeAdded() {
		//		if (this.sourceTime < 0 && this.sourcePosition.isInvalid()) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, Position.INVALID, -1.0, this.linearIndex);		
	}

	/** Make a new NavPoint from the current one with the acceleration/turn rate metadata changed 
	 *  VelocityIn must be defined before using this method.
	 * 
	 * */
	public NavPoint makeTrkAccel(double omega) {
		//		if (ta == this.accel_trk) return this;
		double radius = velocityInit.gs()/omega;	
		//f.pln(" $$$ makeTrkAccel: velocityIn = "+this.velocityIn+" radius = "+radius);
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				radius, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the acceleration/turn rate metadata changed */
	public NavPoint makeGsAccel(double ga) {
		//		if (ga == this.accel_gs) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, ga, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the acceleration/turn rate metadata changed */
	public NavPoint makeVsAccel(double va) {
		//		if (va == this.accel_vs) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, va, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current one with the velocity in metadata changed.
	 *  This must be the correct value for points defining the entry into acceleration zones (BOT, BGSC, BVSC).
	 */
	public NavPoint makeVelocityInit(Velocity v) {
		//		if (v.equals(this.velocityIn)) return this;
		return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, v, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	/** Make a new NavPoint from the current that is "Virtual" */
	public NavPoint makeVirtual() {
		return copy(WayType.Virtual);
	}


	/** Make a new NavPoint from the current that is "Original" */
	public NavPoint makeOriginal() {
		return copy(WayType.Orig);
	}

	/** Make a new NavPoint from the current that is "AltPreserve" */
	public NavPoint makeAltPreserve() {
		return copy(WayType.AltPreserve);
	}

	/** Makes a new NavPoint that is devoid of any "turn" or "ground speed" tags, but retains source position and time */
	public NavPoint makeStandardRetainSource() {
		return new NavPoint(this.p, this.t, WayType.Orig, "", Trk_TCPType.NONE, Gs_TCPType.NONE, Vs_TCPType.NONE,
				0.0, 0.0, 0.0, Velocity.INVALID, this.sourcePosition, this.sourceTime, this.linearIndex);			
	}

	public NavPoint makeNewPoint() {
		return new NavPoint(this.p, this.t, WayType.Orig, this.label, Trk_TCPType.NONE, Gs_TCPType.NONE, Vs_TCPType.NONE,
				0.0, 0.0, 0.0, Velocity.INVALID, this.p, this.t, this.linearIndex);			
	}

	/** Make a new NavPoint from the current with the given name */
	public NavPoint makeLabel(String label) {
		if (label.equals(this.label)) return this;
		return new NavPoint(this.p, this.t, this.ty,  label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

	public boolean isNamedWayPoint(String specPre) {
		if (label.equals("")) return false;
		if (label.contains(specPre)) return false;
		return true;
	}

	public NavPoint appendLabel(String label) {
		if (label.equals("")) return this; // do nothing
		return new NavPoint(this.p, this.t, this.ty,  this.label+label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}
	
	public NavPoint appendLabelNoDuplication(String label) {
		if (this.label.contains(label)) return this;		// do nothing if this string is already in the label
		return appendLabel(label);
	}

	/** Make a new "beginning of turn" NavPoint at the given position and time where the source of the current NavPoint is the "source"
	 *  velocityIn must be properly calculated for this point. 
	 * 
	 * @param p
	 * @param t
	 * @param velocityIn
	 * @param signedRadius right turns have a positive radius, left turns have a negative radius
	 * @return a new BOT NavPoint
	 */
	public NavPoint makeBOT(Position p, double t, Velocity velocityIn, double signedRadius, int linearIndex) {
		//f.pln("  $$$$$$$$$$$ NavPoint.makeBOT t = "+t+" velocityIn = "+velocityIn+" signedRadius = "+signedRadius);
		return new NavPoint(p, t, this.ty,  this.label, Trk_TCPType.BOT,  this.tcp_gs, this.tcp_vs,
				signedRadius, this.accel_gs, this.accel_vs, velocityIn,  this.sourcePosition, this.sourceTime, linearIndex);
	}
	
	/** Make a new "end of turn" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn is not well defined for this point, but should generally be the velocityIn for the corresponding BOT point or the velocity at this point 
	 */
	public NavPoint makeEOT(Position p, double t, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, Trk_TCPType.EOT,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, velocityIn, this.sourcePosition, this.sourceTime, linearIndex);
	}

	/**
	 * 
	 * @param p
	 * @param t
	 * @param velocityIn
	 * @param signedRadius
	 * @return
	 */
	public NavPoint makeEOTBOT(Position p, double t, Velocity velocityIn, double signedRadius, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, Trk_TCPType.EOTBOT,  this.tcp_gs, this.tcp_vs,
				signedRadius, this.accel_gs, this.accel_vs, velocityIn, this.sourcePosition, this.sourceTime, linearIndex);
	}


	/** Make a new "beginning of gs change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public NavPoint makeBGS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,  Gs_TCPType.BGS, this.tcp_vs,
				this.radiusSigned, a, this.accel_vs, velocityIn,  this.sourcePosition, this.sourceTime, linearIndex);
	}

	/** Make a new "end of gs change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn is not well defined for this point, but should generally be the velocityIn for the corresponding BGCS point or the velocity at this point 
	 */
	public NavPoint makeEGS(Position p, double t, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,  Gs_TCPType.EGS, this.tcp_vs,
				 this.radiusSigned, this.accel_gs, this.accel_vs, velocityIn, this.sourcePosition, this.sourceTime, linearIndex);
	}

	public NavPoint makeEGSBGS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,  Gs_TCPType.EGSBGS, this.tcp_vs,
				 this.radiusSigned, a, this.accel_vs, velocityIn, this.sourcePosition, this.sourceTime, linearIndex);
	}


	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn must be properly calculated for this point. 
	 */
	public NavPoint makeBVS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
		//f.pln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);  
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs,  Vs_TCPType.BVS,
				this.radiusSigned, this.accel_gs, a, velocityIn,  this.sourcePosition, this.sourceTime, linearIndex);
	}


	/** Make a new "end of vs change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
	 *  velocityIn is not well defined for this point, but should generally be the velocityIn for the corresponding BGCS point or the velocity at this point 
	 */
	public NavPoint makeEVS(Position p, double t, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, Vs_TCPType.EVS,
				this.radiusSigned, this.accel_gs, this.accel_vs, velocityIn,  this.sourcePosition, this.sourceTime, linearIndex);
	}

	public NavPoint makeEVSBVS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs,  Vs_TCPType.EVSBVS,
				this.radiusSigned, this.accel_gs, a, velocityIn,  this.sourcePosition, this.sourceTime, linearIndex);
	}

	/** Makes a new NavPoint that is an intermediate "mid" added point */
	public NavPoint makeMidpoint(Position p, double t, int linearIndex) {
		return new NavPoint(p, t, this.ty,  this.label, this.tcp_trk,   this.tcp_gs, this.tcp_vs, 
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit,  this.sourcePosition, this.sourceTime, linearIndex);
	}


	/** Makes a new NavPoint that is devoid of any "turn" tags. */
	public NavPoint makeTrkTCPClear() {
		if (isTrkTCP()) {
			return new NavPoint(this.p, this.t, this.ty,  this.label, Trk_TCPType.NONE, this.tcp_gs, this.tcp_vs,
					0.0, this.accel_gs, this.accel_vs, velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);			
		} else {
			return this;
		}
	}

	/** Makes a new NavPoint that is devoid of any "ground speed"  tags. */
	public NavPoint makeGsTCPClear() {
		if (isGsTCP()) {
			return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk, Gs_TCPType.NONE, this.tcp_vs,
					 this.radiusSigned, 0.0, this.accel_vs, velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);			
		} else {
			return this;
		}
	}

	/** Makes a new NavPoint that is devoid of any "vertical speed" tags. */
	public NavPoint makeVsTCPClear() {
		if (isVsTCP()) {
			return new NavPoint(this.p, this.t, this.ty,  this.label, this.tcp_trk, this.tcp_gs, Vs_TCPType.NONE,
					this.radiusSigned, this.accel_gs, 0.0, velocityInit,  this.sourcePosition, this.sourceTime, this.linearIndex);			
		} else {
			return this;
		}
	}


	/** Return a new NavPoint that shares all attributes with the specified NavPoint except Position and Time */
	public NavPoint makeMovedFrom(NavPoint o) {
		return new NavPoint(this.p, this.t, o.ty,  o.label, o.tcp_trk,  o.tcp_gs, o.tcp_vs,
				o.radiusSigned, o.accel_gs, o.accel_vs, o.velocityInit, o.sourcePosition, o.sourceTime, o.linearIndex);

	}

	/** Return a new NavPoint that shares all attributes with this one, except position */
	public NavPoint makePosition(Position p) {
		if (p.equals(this.p)) return this;
		return new NavPoint(p, this.t, this.ty,  this.label, this.tcp_trk,  this.tcp_gs, this.tcp_vs,
				this.radiusSigned, this.accel_gs, this.accel_vs, this.velocityInit, this.sourcePosition, this.sourceTime, this.linearIndex);
	}

//	// ----- Static Constructors With No Source Info --------------------------------------------------------
//
//	/** Make a new "beginning of turn" NavPoint at the given position and time where the source of the current NavPoint is the "source"
//	 *  velocityIn must be properly calculated for this point. 
//	 */
//	public static NavPoint mkBOT(Position p, double t, double omega, Velocity velocityIn, int linearIndex) {
//		//f.pln("  $$$$$$$$$$$ NavPoint.makeBOT t = "+t+" velocityIn = "+velocityIn);
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.BOT,  Gs_TCPType.NONE, Vs_TCPType.NONE,
//				0.0, 0.0, 0.0, velocityIn,  Position.INVALID, -1.0, linearIndex);
//	}
//
//	/** Make a new "end of turn" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
//	 *  velocityIn is not well defined for this point, but should generally be the velocityIn for the corresponding BOT point or the velocity at this point 
//	 */
//	public static NavPoint mkEOT(Position p, double t, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.EOT,  Gs_TCPType.NONE, Vs_TCPType.NONE,
//				0.0, 0.0, 0.0, velocityIn,  Position.INVALID, -1.0, linearIndex );
//	}
//
//	public static NavPoint mkEOTBOT(Position p, double t, double omega, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.EOTBOT,  Gs_TCPType.NONE, Vs_TCPType.NONE,
//				omega,0.0, 0.0, velocityIn,  Position.INVALID, -1.0, linearIndex);
//	}
//
//
//	/** Make a new "beginning of gs change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
//	 *  velocityIn must be properly calculated for this point. 
//	 */
//	public static NavPoint mkBGS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,  Gs_TCPType.BGS, Vs_TCPType.NONE,
//				0.0, a, 0.0, velocityIn, Position.INVALID, -1.0, linearIndex);
//	}
//
//	/** Make a new "end of gs change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
//	 *  velocityIn is not well defined for this point, but should generally be the velocityIn for the corresponding BGCS point or the velocity at this point 
//	 */
	public static  NavPoint mkEGS(Position p, double t, Velocity velocityIn, int linearIndex) {
		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,  Gs_TCPType.EGS, Vs_TCPType.NONE,
				0.0,0.0, 0.0, velocityIn, Position.INVALID, -1.0, linearIndex);
	}
//
//	public static NavPoint mkEGSBGS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,  Gs_TCPType.EGSBGS, Vs_TCPType.NONE,
//				0.0, a, 0.0, velocityIn, Position.INVALID, -1.0, linearIndex);
//	}
//
//
//	/** Make a new "beginning of vertical speed change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
//	 *  velocityIn must be properly calculated for this point. 
//	 */
//	public static NavPoint mkBVS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
//		//f.pln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);  
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,  Gs_TCPType.NONE,  Vs_TCPType.BVS,
//				0.0,0.0, a, velocityIn, Position.INVALID, -1.0, linearIndex);
//	}
//
//	/** Make a new "end of vs change" NavPoint at the given position and time where the source of the current NavPoint is the "source" 
//	 *  velocityIn is not well defined for this point, but should generally be the velocityIn for the corresponding BGCS point or the velocity at this point 
//	 */
//	public static NavPoint mkEVS(Position p, double t, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,  Gs_TCPType.NONE, Vs_TCPType.EVS,
//				0.0,0.0,0.0, velocityIn, Position.INVALID, -1.0, linearIndex);
//	}
//
//	public static NavPoint mkEVSBVS(Position p, double t, double a, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,  Gs_TCPType.NONE,  Vs_TCPType.EVSBVS,
//				0.0, 0.0, a, velocityIn, Position.INVALID, -1.0, linearIndex);
//	}
//
//	/** Makes a new NavPoint that is an intermediate "mid" added point */
//	public static NavPoint mkMidpoint(Position p, double t, Velocity velocityIn, int linearIndex) {
//		return new NavPoint(p, t, WayType.Orig,  "", Trk_TCPType.NONE,   Gs_TCPType.NONE, Vs_TCPType.NONE, 
//				0.0,0.0, 0.0, velocityIn, Position.INVALID, -1.0, linearIndex);
//	}


	/** 
	 * Calculate and return the initial velocity between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s2 the given NavPoint
	 */ 
	public static Velocity initialVelocity(NavPoint s1, NavPoint s2) {
		if (debug && s1.isLatLon() != s2.isLatLon()) {
			throw new RuntimeException("Incompatible geometries in velocity()");
		}
		double dt = s2.time() - s1.time();
		//f.pln(" $$$$ NavPoint.initialVelocity: dt = "+dt+" s2 = "+s2);
		if (dt==0) {
			return Velocity.ZERO;
		} else if (dt > 0) {			
			if (s2.isLatLon()) {
				return GreatCircle.velocity_initial(s1.p.lla(), s2.p.lla(), dt);
			} else {
				Velocity vel = Velocity.make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
				return vel;
			}
		} else {
			f.pln("WARNING: NavPoint INITIAL VELOCITY negative time! dt = "+dt+" this="+s1+" s="+s2);
			//Debug.halt();
			if (s2.isLatLon()) {
				return GreatCircle.velocity_initial(s2.p.lla(), s1.p.lla(), -dt);
			} else {
				return Velocity.make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
			}			
		}
	}
	
	// ------------------------------------------------------------------------------------------------------
	/** 
	 * Calculate and return the initial velocity between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 */ 
	public Velocity initialVelocity(NavPoint s) {
		return initialVelocity(this, s);
	}
	
	// ------------------------------------------------------------------------------------------------------
	/** 
	 * Calculate and return the final velocity between the current point and the given point
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 *
	 * @param s the given NavPoint
	 */ 
	public Velocity finalVelocity(NavPoint s) {
		//TODO: revisit this as static 2 parameter function
		if (debug && p.isLatLon() != s.isLatLon()) {
			throw new RuntimeException("Incompatible geometries in velocity()");
		}
		double dt = s.time() - t;
		//f.pln(" $$$$ NavPoint.initialVelocity: dt = "+dt);
		if (dt==0) {
			return Velocity.ZERO;
		} else if (dt > 0) {			
			if (s.isLatLon()) {
				return GreatCircle.velocity_final(p.lla(), s.p.lla(), dt);
			} else {
				return Velocity.make((s.p.point().Sub(p.point())).Scal(1.0/dt));
			}
		} else {
			f.pln("WARNING: NavPoint INITIAL VELOCITY negative time!  Check the code here!");
			f.pln("NavPoint.initialVelocity this="+this+" s="+s);
			//DebugSupport.halt();
			if (s.isLatLon()) {
				return GreatCircle.velocity_final(s.p.lla(), p.lla(), -dt);
			} else {
				return Velocity.make((p.point().Sub(s.p.point())).Scal(1.0/-dt));
			}			
		}
	}

	

	/** 
	 * Calculate and return the vertical speed between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 */ 
	public double  verticalSpeed(NavPoint s) {
		return (s.alt() - this.alt())/((s.time() - this.time()));
	}

	/** 
	 * Calculate and return the REPRESENTATIVE LINEAR velocity between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 */ 
	public Velocity averageVelocity(NavPoint s) {
		//TODO: revisit this as static 2 parameter function
		if (debug && p.isLatLon() != s.isLatLon()) {
			throw new RuntimeException("Incompatible geometries in velocity()");
		}
		double dt = s.time() - t;
		if (dt==0) {
			return Velocity.ZERO;
		} else if (dt > 0) {

			if (s.isLatLon()) {
				return GreatCircle.velocity_average(p.lla(), s.p.lla(), dt);
			} else {
				return Velocity.make((s.p.point().Sub(p.point())).Scal(1.0/dt));
			}
		} else {
			if (debug) {
				f.pln("WARNING: NavPoint AVERAGE VELOCITY negative time!  Check the code here!");
				f.pln("NavPoint.averageVelocity this="+this+" s="+s);
				Thread.dumpStack();
			}
			if (s.isLatLon()) {
				return GreatCircle.velocity_average(s.p.lla(), p.lla(), -dt);
			} else {
				return Velocity.make((p.point().Sub(s.p.point())).Scal(1.0/-dt));
			}
		}
	}


	// Project the point in time, given an initial velocity
	/** Perform a linear projection of the current NavPoint with given velocity and time.  If the 
	 * current NavPoint is in LatLonAlt, then a constant track angle linear projection is used.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 * 	 * @param time  relative time 
	 */
	public NavPoint linear(Velocity v, double time) {
		Position newPos = p.linear(v,time);
		return new NavPoint(newPos,t+time);
	}

	/** Compute a new lat/lon that is obtained by moving with velocity vo for tm secs
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 *
	 * @param vo   velocity away from original position
	 * @param tm   time of relocation
	 * @return new lat/lon position in direction v0
	 */
	public NavPoint linearEst(Velocity vo, double tm) {
		//double tm = dist/vel.gs();
		return new NavPoint(p.linearEst(vo,tm),t+tm);
	}

	/**
	 * return a new NavPoint between this NavPoint and np that corresponds to time by interpolation
	 * This function is commutative with respect to the points: direction between points is always determined by the time ordering of the two points.
	 * Negative time results in a velocity in the opposite direction (along the same great circle, if in latlon)
	 * 
	 * @param np    next NavPoint
	 * @param time  time 
	 * @return      interpolated NavPoint
	 */
	public NavPoint interpolate(NavPoint np, double time) {
		Velocity v = initialVelocity(np);
		Position newPos = p.linear(v,time-t);
		//f.pln(" $$$$$$$ interpolate: t = "+t+" p = "+p+" np = "+np+" v = "+v+" newPos = "+newPos);
		return new NavPoint(newPos,time);
	}

	/**
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * @param np   next NavPoint
	 * @return     ground speed between this NavPoint and np 
	 */
	public double groundSpeed(NavPoint np) {
		return initialVelocity(np).gs();
	}



	/** Horizontal distance */
	public double distanceH(NavPoint np2) {
		return np2.position().distanceH(this.position());
	}

	//	public static NavPoint intersection(NavPoint s1, NavPoint s2, NavPoint p1, NavPoint p2) {
	//		Plan lpc = new Plan();
	//		lpc.add(s1);
	//		lpc.add(s2);
	//		lpc.add(p1);
	//		lpc.add(p2);
	//	  	DebugSupport.dumpAsUnitTest(lpc);
	//		double dto  = s2.time() - s1.time(); 
	//		Pair<Position,Double> interSec = Position.intersection(s1.position(), s2.position(), dto, p1.position(), p2.position());
	//		//f.pln(" "+s1.time()+" "+s2.time()+" "+p1.time()+" "+p2.time()+"  interSec.second = "+interSec.second);
	//		NavPoint rtn = new NavPoint(interSec.first,s1.time()+interSec.second,"intersectionPt");
	//        return rtn;
	//	}


	/** Vertical distance */
	public double distanceV(NavPoint np2) {
		return np2.position().distanceV(this.position());
	}

	public void diff(int i, NavPoint p) {
		if (!this.almostEquals(p)) {
			f.pln(" points at index i = "+i+" have different positions!");
		}
		if (!Util.almost_equals(t,p.t)) {
			f.pln(" points at index i = "+i+" have different times! "+t+" != "+p.t);			
		}
		if (!velocityInit().almostEquals(p.velocityInit())) {
			f.pln(" points at index i = "+i+" have different velocityIn fields "+velocityInit+" != "+p.velocityInit);
		}
		if (tcp_trk != p.tcp_trk || tcp_gs != p.tcp_gs || tcp_vs != p.tcp_vs) {
			f.pln(" points at index i = "+i+" have different tcp_* fields");
		}
		if (tcp_trk == Trk_TCPType.BOT && radiusSigned != p.radiusSigned) {
			f.pln(" points at index i = "+i+" have different accel_trk fields "+radiusSigned+" != "+p.radiusSigned);
		}
		if (tcp_gs == Gs_TCPType.BGS && accel_gs != p.accel_gs) {
			f.pln(" points at index i = "+i+" have different accel_gs fields "+accel_gs+" != "+p.accel_gs);
		}
		if (tcp_vs == Vs_TCPType.BVS && accel_vs != p.accel_vs) {
			f.pln(" points at index i = "+i+" have different accel_vs fields "+accel_vs+" != "+p.accel_vs);
		}
		if (!sourcePosition.almostEquals(p.sourcePosition)) {
			f.pln(" points at index i = "+i+" have different source positions!");
		}
		if (!Util.almost_equals(sourceTime, p.sourceTime)) {
			f.pln(" points at index i = "+i+" have different source times!");						
		}
		if (radiusSigned != p.radiusSigned) {
			f.pln(" points at index i = "+i+" have different stored radii!");
		}
	}

	/**
	 * Return a string representation of TCP meta data (or the empty string if none)
	 */
	public String metaDataLabel(int precision) {
		String tlabel = "";
		if (isVirtual()) {
			tlabel = tlabel + "VIRT:";
		} 
		if (isTrkTCP()) {
			tlabel = tlabel + tcp_trk.toString() +":";
			if (isBOT()) {
				tlabel = tlabel + "ATRK:"+f.FmPrecision(radiusSigned,precision)+":";
			}
		}
		if (isGsTCP()) {
			tlabel = tlabel + tcp_gs.toString() +":";
			if (isBGS()) {
				tlabel = tlabel + "AGS:"+f.FmPrecision(accel_gs,precision)+":";
			}
		}
		if (isVsTCP()) {
			tlabel = tlabel + tcp_vs.toString() +":";
			if (isBVS()) {
				tlabel = tlabel + "AVS:"+f.FmPrecision(accel_vs,precision)+":";
			}
		}
		if (velocityInit != null && !velocityInit.isInvalid()) {
			String v = velocityInit.toStringNP(precision).replaceAll("[, ]+", "_");
			tlabel = tlabel + "VEL:"+v+":";
		}
		if (sourceTime < 0) { // MOT or other added points
			tlabel = tlabel + "ADDED:";
		} else if (isTCP()){ // generated TCP points
			tlabel = tlabel + "SRC:"+sourcePosition.toStringNP(precision).replaceAll("[, ]+", "_")+":";
			tlabel = tlabel + "STM:"+f.FmPrecision(sourceTime,precision)+":";
		} else if (sourceTime != t) { // linear timeshifted point
			tlabel = tlabel + "STM:"+f.FmPrecision(sourceTime,precision)+":";
		}
		// add starting colon, if there is any data;
		if (tlabel.length() > 0) {
			tlabel = ":"+tlabel;
		}
		return tlabel;
	}

	/**
	 * If it is detected that this string uses the old metatdata format, this returns an invalid NavPoint.
	 */
	public NavPoint parseMetaDataLabel() {
		return parseMetaDataLabel(label);
	}



	/**
	 * This parses the label generated for TCP types to reconstruct the meta-data of the point.  
	 * It returns either the same point (if no significant data is in the label) or a fully reconstructed TCP type.
	 * The reconstructed TCP will have a label field that does not contain any TCP metadata -- you will need to call name()
	 * or tcpLabel() to retrieve the full string.
	 * If it is detected that this string uses the old metatdata format, this returns an invalid NavPoint.
	 */
	public NavPoint parseMetaDataLabel(String tlabel) {
		//f.pln(" $$$$$ parseMetaDataLabel: tlabel = "+tlabel);
		NavPoint point = this;
		int i = tlabel.indexOf(":ACC:");
		if (i >= 0) {
			return NavPoint.INVALID;
		}
		int lowIndex = tlabel.length(); // then current end of the user label
		int j;
		i = tlabel.indexOf(":VIRT:");
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			point = point.makeVirtual();
		}

		i = tlabel.indexOf(":ADDED:");
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			point = point.makeSource(Position.INVALID,-1.0);
		} else {
			Position sp = p;
			double st = t;
			i = tlabel.indexOf(":SRC:");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				j = tlabel.indexOf(":",i+5);
				// first try general parse, then coordinate-specific if that fails
				sp = Position.parse(tlabel.substring(i+5, j).replaceAll("_", " "));
				if (sp.isInvalid()) {
					if (isLatLon()) {
						sp = Position.parseLL(tlabel.substring(i+5, j).replaceAll("_", " "));
					} else {
						sp = Position.parseXYZ(tlabel.substring(i+5, j).replaceAll("_", " "));
					}
				}
			}
			i = tlabel.indexOf(":STM:");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				j = tlabel.indexOf(":",i+5);
				st = Double.parseDouble(tlabel.substring(i+5,j));
			}
			//f.pln(" $$$$$ parseMetaDataLabel: i = "+i+" st = "+st);
			point = point.makeSource(sp,st);			
		}

		i = tlabel.indexOf(":VEL:"); // velocity in
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = tlabel.indexOf(":",i+5);
			Velocity v = Velocity.parse(tlabel.substring(i+5, j).replaceAll("_", " "));
			point = point.makeVelocityInit(v);
		}
		i = tlabel.indexOf(":ATRK:"); // acceleration
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = tlabel.indexOf(":",i+6);
			double a = Double.parseDouble(tlabel.substring(i+6,j));
			point = point.makeTrkAccel(a);
		}
		i = tlabel.indexOf(":AGS:"); // acceleration
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = tlabel.indexOf(":",i+5);
			double a = Double.parseDouble(tlabel.substring(i+5,j));
			point = point.makeGsAccel(a);
		}
		i = tlabel.indexOf(":AVS:"); // acceleration
		if (i >= 0) {
			lowIndex = Math.min(i, lowIndex);
			j = tlabel.indexOf(":",i+5);
			double a = Double.parseDouble(tlabel.substring(i+5,j));
			point = point.makeVsAccel(a);
		}
		for (Trk_TCPType t : Trk_TCPType.values()) {
			i = tlabel.indexOf(":"+t+":");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				point = point.makeTrkTCP(t);
			}
		}
		for (Gs_TCPType t : Gs_TCPType.values()) {
			i = tlabel.indexOf(":"+t+":");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				point = point.makeGsTCP(t);
			}
		}
		for (Vs_TCPType t : Vs_TCPType.values()) {
			i = tlabel.indexOf(":"+t+":");
			if (i >= 0) {
				lowIndex = Math.min(i, lowIndex);
				point = point.makeVsTCP(t);
			}
		}
		String name = tlabel;
		if (lowIndex >= 0 && lowIndex < tlabel.length()) {
			name = tlabel.substring(0,lowIndex);
			point = point.makeLabel(name);
		}
		return point;
	}


	public String tcpTypeString() {
		String s = "";
		if (tcp_trk != Trk_TCPType.NONE) s += tcp_trk;
		if (tcp_gs != Gs_TCPType.NONE) {
			if (!s.equals("")) s+= ",";
			s += tcp_gs;
		}
		if (tcp_vs != Vs_TCPType.NONE) {
			if (!s.equals("")) s+= ",";
			s += tcp_vs;
		}
		if (s.equals("")) s += "NONE";
		return s;
	}


	/** String representation, using the default output precision (see Contants.get_output_precision()) */
	public String toStringShort() {
		return toString(Constants.get_output_precision());
	}

	/** String representation, using the give precision */
	public String toStringShort(int precision) {
		return p.toStringNP(precision) + ", " + f.FmPrecision(t,precision); 
	}
	
	/** String representation, using the default output precision (see Contants.get_output_precision()) */
	public String toString() {
		return toString(Constants.get_output_precision());
	}

	/** String representation, using the give precision */
	public String toString(int precision) {
		return p.toStringNP(precision) + ", " + f.FmPrecision(t,precision) +" TCP:"+tcpTypeString()+" "+f.padRight(label,4);
	}


	/** Return a string representation that includes additional information */
	public String toStringFull() {
		StringBuffer sb = new StringBuffer(100);
		sb.append("[(");
		if (isLatLon()) sb.append("LL: ");
		sb.append(toStringShort(4));
		sb.append("), ");
		sb.append(ty);
		if (isTrkTCP()) {
			sb.append(", "+tcp_trk.toString());
//			if (isBOT()) {
//				sb.append(" signedRadius = "+f.Fm4(Units.to("NM", radius)));
//			}
			if (isBOT()) {
				sb.append(" accTrk = "+f.Fm4(Units.to("deg/s", trkAccel())));
			}
		}
		if (isGsTCP()) {
			sb.append(", "+tcp_gs.toString());
			if (isBGS()) {
				sb.append(" accGs = "+f.Fm4(Units.to("m/s^2", accel_gs)));
			}
		}
		if (isVsTCP()) {
			sb.append(", "+tcp_vs.toString());
			if (isBVS()) {
				sb.append(" accVs = "+f.Fm4(Units.to("m/s^2", accel_vs)));
			}
		}
		if (!velocityInit.isInvalid()) sb.append(" vin = "+velocityInit.toStringUnits());
		if (!Util.almost_equals(radiusSigned,0.0)) {
			sb.append(" sgnRadius = "+f.Fm4(Units.to("NM",radiusSigned)));
		}
//		if (sourceTime >= 0) {
//			sb.append(" srcTime = "+f.Fm2(sourceTime));
//			if (!sourcePosition.isInvalid()) {
//				sb.append(" srcPos = "+sourcePosition.toStringUnits()); 		
//			}
//		} else {
//			sb.append(" ADDED");
//		}
		sb.append("<"+linearIndex+">");
		sb.append("]");
		sb.append(" "+label());
		return sb.toString();
	}

	//TODO: update TCP_OUTPUT_COLUMNS if this changes!!!
	/** Return a list of string representing each field (position, time, etc.).  (tcpinfo + label) */
	public List<String> toStringList(int precision, boolean tcp) {
		ArrayList<String> ret = new ArrayList<String>(10);  // name is (0)
		ret.addAll(p.toStringList(precision)); // position (1-3)
		ret.add(f.FmPrecision(t,precision)); // time (4)
		if (tcp) {
			ret.add(ty.toString()); // type (string) (5)
			ret.addAll(velocityInit.toStringList(precision)); // vin (6-8) DO NOT CHANGE THIS -- POLYGONS EXPECT VEL TO BE HERE
			ret.add(tcp_trk.toString()); // tcp trk (string) (9)			
			ret.add(f.FmPrecision(Units.to("deg/s", trkAccel()),precision)); // trk accel (10)
			ret.add(tcp_gs.toString()); // tcp gs (string) (11)
			ret.add(f.FmPrecision(Units.to("m/s^2", accel_gs),precision)); // gs accel (12)
			ret.add(tcp_vs.toString()); // tcp vs (string) (13)
			ret.add(f.FmPrecision(Units.to("m/s^2", accel_vs),precision)); // vs accel (14)
			ret.add(f.FmPrecision(Units.to("nmi", radiusSigned), precision)); // radius (15)
			ret.addAll(sourcePosition.toStringList(precision)); // source position (16-18)
			ret.add(f.FmPrecision(sourceTime,precision)); // source time (19)
			if (label.length() > 0) {
				ret.add(label); // label (string) (20)
			} else {
				ret.add("-");
			}
		} else {
			String fl = fullLabel(); 
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
	public String toOutput() {
		return toOutput(Constants.get_output_precision(),false);
	}

	/** Return a string representation that is consistent with the PathReader input format. 
	 *  if tcp is true, explicily include all metadata as distinct columns.
	 *  if tcp is false, collapse metadata, if any, into the "label" field 
	 */
	public String toOutput(int precision, boolean tcp) {
		return f.list2str(toStringList(precision,tcp),", ");
	}

	public static NavPoint parseLL(String s) {
		String[] fields = s.split(Constants.wsPatternParens);
		Vect3 v = Vect3.parse(fields[0]+" "+fields[1]+" "+fields[2]);
		double time = Util.parse_double(fields[3]);
		Position pos = Position.makeLatLonAlt(v.x, v.y, v.z);
		if (fields.length == MIN_OUTPUT_COLUMNS+1) {
			String extra = fields[MIN_OUTPUT_COLUMNS];
			return (new NavPoint(pos, time, extra)).parseMetaDataLabel(extra);
		} else if (fields.length == TCP_OUTPUT_COLUMNS+1) {
			WayType wt = NavPoint.WayType.valueOf(fields[4]);
			Velocity vv = Velocity.parse(fields[5]+" "+fields[6]+" "+fields[7]);
			Trk_TCPType trkty = Trk_TCPType.valueOf(fields[8]);
			//double trkacc = Units.from("NM", Util.parse_double(fields[9]));
			Gs_TCPType gsty = Gs_TCPType.valueOf(fields[10]);
			double gsacc = Units.from("m/s^2", Util.parse_double(fields[11]));
			Vs_TCPType vsty = Vs_TCPType.valueOf(fields[12]);
			double vsacc = Units.from("m/s^2", Util.parse_double(fields[13]));
			double radius = Util.parse_double(fields[14]);
			LatLonAlt slla = LatLonAlt.parse(fields[15]+" "+fields[16]+" "+fields[17]);
			Position sp = new Position(slla);
			double st = Util.parse_double(fields[18]);
			String lab = fields[19];
			return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
		} else {
			return new NavPoint(pos, time);
		}
	}

	public static NavPoint parseXYZ(String s) {
		String[] fields = s.split(Constants.wsPatternParens);
		//Pair<PointMutability,String> extra = parseExtra(fields);
		Vect3 v = Vect3.parse(fields[0]+" "+fields[1]+" "+fields[2]);
		double time = Util.parse_double(fields[3]);
		Position pos = Position.makeXYZ(v.x, v.y, v.z);
		if (fields.length == MIN_OUTPUT_COLUMNS+1) {
			String extra = fields[MIN_OUTPUT_COLUMNS];
			return (new NavPoint(pos, time, extra)).parseMetaDataLabel(extra);
		} else if (fields.length == TCP_OUTPUT_COLUMNS+1) {
			WayType wt = NavPoint.WayType.valueOf(fields[4]);
			Velocity vv = Velocity.parse(fields[5]+" "+fields[6]+" "+fields[7]);
			Trk_TCPType trkty = Trk_TCPType.valueOf(fields[8]);
			//double accel_trk = Units.from("deg/s", Util.parse_double(fields[9]));
			Gs_TCPType gsty = Gs_TCPType.valueOf(fields[10]);
			double gsacc = Units.from("m/s^2", Util.parse_double(fields[11]));
			Vs_TCPType vsty = Vs_TCPType.valueOf(fields[12]);
			double vsacc = Units.from("m/s^2", Util.parse_double(fields[13]));
			double radius = Util.parse_double(fields[14]);
			Vect3 sv = Vect3.parse(fields[15]+" "+fields[16]+" "+fields[17]);
			Position sp = Position.makeXYZ(sv.x, sv.y, sv.z);
			double st = Util.parse_double(fields[18]);
			String lab = fields[19];
			return new NavPoint(pos, time, wt, lab,	trkty, gsty, vsty, radius, gsacc, vsacc, vv, sp, st, -1);
		} else {
			return new NavPoint(pos, time);
		}
	}



	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		long temp;
		temp = Double.doubleToLongBits(accel_gs);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(radiusSigned);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(accel_vs);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		temp = Double.doubleToLongBits(sourceTime);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result + ((label == null) ? 0 : label.hashCode());
		result = prime * result + ((p == null) ? 0 : p.hashCode());
		temp = Double.doubleToLongBits(t);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result + ((tcp_gs == null) ? 0 : tcp_gs.hashCode());
		result = prime * result + ((tcp_trk == null) ? 0 : tcp_trk.hashCode());
		result = prime * result + ((tcp_vs == null) ? 0 : tcp_vs.hashCode());
		result = prime * result + ((ty == null) ? 0 : ty.hashCode());
		temp = Double.doubleToLongBits(sourceTime);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		result = prime * result
				+ ((velocityInit == null) ? 0 : velocityInit.hashCode());
		return result;
	}



	@Override
	public boolean equals(Object obj) {
		//f.pln(" $$ NavPoint: equals obj = "+obj);
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		NavPoint other = (NavPoint) obj;
		if (Double.doubleToLongBits(accel_gs) != Double
				.doubleToLongBits(other.accel_gs))
			return false;
		if (Double.doubleToLongBits(radiusSigned) != Double
				.doubleToLongBits(other.radiusSigned))
			return false;
		if (Double.doubleToLongBits(accel_vs) != Double
				.doubleToLongBits(other.accel_vs))
			return false;
//		if (Double.doubleToLongBits(sourceTime) != Double
//				.doubleToLongBits(other.sourceTime))
//			return false;
		if (label == null) {
			if (other.label != null)
				return false;
		} else if (!label.equals(other.label))
			return false;
		if (p == null) {
			if (other.p != null)
				return false;
		} else if (!p.equals(other.p))
			return false;
		if (Double.doubleToLongBits(t) != Double.doubleToLongBits(other.t))
			return false;
		if (tcp_gs != other.tcp_gs)
			return false;
		if (tcp_trk != other.tcp_trk)
			return false;
		if (tcp_vs != other.tcp_vs)
			return false;
		if (ty != other.ty)
			return false;
//		if (Double.doubleToLongBits(sourceTime) != Double
//				.doubleToLongBits(other.sourceTime))
//			return false;
		if (velocityInit == null) {
			if (other.velocityInit != null)
				return false;
		} else if (!velocityInit.equals(other.velocityInit))
			return false;
		return true;
	}




}

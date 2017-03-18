/*
 * NavPoint 
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/** 
 * An immutable class representing a position at a time. In addition a label (or name) can be attached 
 * to one of these NavPoints.
 */
public class NavPoint {

	
	private static boolean  debug = false; 

	private final Position p;                   // 3D position (XYZ or LatLonAlt)
	private final double   t;                   // time of point
	private final String   label;               // label

	/** A zero in Latitude/Longitude */
	public static final NavPoint ZERO_LL  = new NavPoint(Position.ZERO_LL, 0.0, "ZERO_LL");
	/** A zero in Euclidean space */
	public static final NavPoint ZERO_XYZ = new NavPoint(Position.ZERO_XYZ, 0.0, "ZERO_XYZ");  
	/** An invalid NavPoint.  Note that this is not necessarily equal to other invalid NavPoints -- use the isInvalid() test instead. */
	public static final NavPoint INVALID  = new NavPoint(Position.INVALID, Double.NaN, "INVALID");

	/**
	 * Construction a non-TCP NavPoint piecemeal
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 * 
	 * @param p position of new NavPoint
	 * @param t time of new NavPoint
	 * @param label string label of new NavPoint
	 */
	public NavPoint(Position p, double t, String label) {
		super();
		this.p = p;
		this.t = t;
		this.label = label;
	}

	/**
	 * Construct a new NavPoint from a position and a time.
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 * 
	 * @param p position of new NavPoint
	 * @param t time of new NavPoint
	 */
	public NavPoint(Position p, double t) {
		super();
		this.p = p;
		this.t = t;
		this.label = "";
	}


	/**
	 * Creates a new lat/lon NavPoint with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude (radians)
	 * @param lon longitude (radians)
	 * @param alt altitude [m]
	 * @param t   time
	 * @return a new NavPoint
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
	 * @return a new NavPoint
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
	 * @param t time [s]
	 * @return a new NavPoint
	 */
	public static NavPoint makeXYZ(double x, double y, double z, double t) {
		return new NavPoint(Position.makeXYZ(x, y, z), t);
	}

	/**
	 * Creates a new Euclidean NavPoint with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
	 * 
	 * @param x coordinate [m]
	 * @param y coordinate [m]
	 * @param z altitude [m]
	 * @param t time [s]
	 * @return a new NavPoint
	 */
	public static NavPoint mkXYZ(double x, double y, double z, double t) {
		return new NavPoint(Position.mkXYZ(x, y, z), t);
	}


	/**
	 * Returns true if any components of this NavPoint are marked as Invalid
	 * @return true, if invalid
	 */
	public boolean isInvalid() {
		return p.isInvalid() || Double.isNaN(t);
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
	 * @param epsilon_horiz maximum allowed horizontal difference
	 * @param epsilon_vert maximum allowed vertical difference
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 * 
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


	/** Return the horizontal position vector 
	 * @return a Vect2
	 * */
	public Vect2 vect2() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in vect2()");
		}
		return p.vect2();
	}

	/** Return the three dimensional position vector 
	 * @return a Point object (essentially a Vect3 with more natural units)
	 * */
	public Point point() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in point()");
		}
		return p.point();
	}

	/** Return the LatLonAlt object 
	 * @return a LatLonAlt object
	 * */
	public LatLonAlt lla() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in lla()");
		}
		return p.lla();
	}

	/** Return the Position object 
	 * @return a Position object
	 * */
	public Position position() {
		return p;
	}

	/** Return the time [s] 
	 * @return time in [s]
	 * */
	public double time() {
		return t;
	}


	/** Is this point specified in lat/lon? 
	 * @return true, if the point is Lat/Lon (not Euclidean)
	 * */
	public boolean isLatLon() {
		return p.isLatLon();
	}


	/** Return the label of this point, if set.  If no name has been set, then 
	 * return the empty string. 
	 * 
	 * @return a string label
	 * */
	public String label() {
		String name = "";
		if (label != null) {
			name = label;
		}
		return name;
	}

	/** Returns true if the "label" label of this NavPoint has been set. 
	 * 
	 * @return true, if label is set
	 * */
	public boolean isLabelSet() {
		if (label == null) {
			return false;
		}
		return !label.equals("");
	}

	/** Return the latitude 
	 * 
	 * @return latitude [radian]
	 * */
	public double lat() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in lat()");
		}
		return p.lat();
	}
	
	/** Return the longitude
	 * 
	 * @return longitude [radian]
	 *  */
	public double lon() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in lon()");
		}
		return p.lon();
	}

	/** Return the altitude 
	 * 
	 * @return altitude [m] */
	public double alt() {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in alt()");
		}
		return p.alt();
	}
	
	/** Return the x coordinate 
	 * @return x coordinate
	 * */
	public double x() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in x()");
		}
		return p.x();
	}

	/** Return the y coordinate 
	 * @return y coordinate
	 * */
	public double y() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in y()");
		}
		return p.y();
	}

	/** Return the z coordinate 
	 * @return z coordinate
	 * */
	public double z() {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in z()");
		}
		return p.z();
	}


	//
	// Make new points from old ones
	//

	private NavPoint copy(Position p) {
		//		if (p.equals(this.p)) return this;
		return new NavPoint(p, this.t, this.label); 
	}

	/** Make a new NavPoint from the current one with the latitude changed
	 * @param lat new latitude value [radian]
	 * @return a new NavPoint
	 *  */
	public NavPoint mkLat(double lat) {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeLat()");
		}
		return copy(p.mkLat(lat));
	}

	/** Make a new NavPoint from the current one with the X coordinate changed 
	 * @param x new x value [m]
	 * @return a new NavPoint
	 * */
	public NavPoint mkX(double x) {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeX()");
		}
		return copy(p.mkX(x));
	}

	/** Make a new NavPoint from the current one with the longitude changed 
	 * @param lon new longitude value [radian]
	 * @return a new NavPoint
	 * */
	public NavPoint mkLon(double lon) {
		if (debug && ! p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeLon()");
		}
		return copy(p.mkLon(lon));
	}

	/** Make a new NavPoint from the current one with the Y coordinate changed 
	 * @param y new y value [m]
	 * @return a new NavPoint
	 * */
	public NavPoint mkY(double y) {
		if (debug && p.isLatLon()) {
			throw new RuntimeException("Incorrect geometry in makeY()");
		}
		return copy(p.mkY(y));
	}

	/** Make a new NavPoint from the current one with the altitude changed 
	 * @param alt a new altitude value [m]
	 * @return a new NavPoint
	 * */
	public NavPoint mkAlt(double alt) {
		return copy(p.mkAlt(alt));
	}

	/** Make a new NavPoint from the current one with the Z coordinate changed 
	 * @param z new z value [m]
	 * @return a new NavPoint
	 * */
	public NavPoint mkZ(double z) {
		return copy(p.mkZ(z));
	}

	/** Make a new NavPoint from the current one with the time changed, source time is not changed 
	 * @param time time in seconds [s]
	 * @return a new NavPoint
	 * */
	public NavPoint makeTime(double time) {
		//		if (time == this.t) return this;
		return new NavPoint(this.p, time, this.label); 
	}

	/** Make a new NavPoint from the current with the given name 
	 * @param label new string label
	 * @return a new NavPoint
	 * */
	public NavPoint makeLabel(String label) {
		if (label.equals(this.label)) return this;
		return new NavPoint(this.p, this.t, label); 
	}

//	public boolean isNamedWayPoint(String specPre) {
//		if (label.equals("")) return false;
//		if (label.contains(specPre)) return false;
//		return true;
//	}

	public boolean isNamedWayPoint() {
		if (label.equals("")) return false;
		return true;
	}

	public NavPoint appendLabel(String label) {
		if (label.equals("")) return this; // do nothing
		return new NavPoint(this.p, this.t, this.label+label);
	}
	
	public NavPoint appendLabelNoDuplication(String label) {	
		if (this.label.contains(label) ||		// do nothing if this string is already in the label
				label.contains("$virtual") || // do not add virtual labels
				(!this.label.startsWith("$") && this.label.length()==4)) return this;		// do nothing if this is a 4-character airport label
		return appendLabel(label);
	}


	/** Return a new NavPoint that shares all attributes with this 
	 * one, except position 
	 * 
	 * @param p new Position object
	 * @return a new NavPoint object
	 * */
	public NavPoint makePosition(Position p) {
		if (p.equals(this.p)) return this;
		return new NavPoint(p, this.t, this.label); 	}


	/** 
	 * Calculate and return the initial velocity between the two NavPoint. 
	 * This function is commutative: direction between points is always 
	 * determined by the time ordering of the two points.
	 * 
	 * @param s1 one NavPoint
	 * @param s2 another NavPoint
	 * @return velocity between two points
	 */ 
	public static Velocity initialVelocity(NavPoint s1, NavPoint s2) {
		if (debug && s1.isLatLon() != s2.isLatLon()) {
			throw new RuntimeException("Incompatible geometries in velocity()");
		}
		double dt = s2.time() - s1.time();
		//f.pln(" $$$$ NavPoint.initialVelocity: dt = "+dt+" s1 = "+s1+"  s2 = "+s2);
		if (dt==0) {
			return Velocity.ZERO;
		} else if (dt > 0) {			
			if (s2.isLatLon()) {
				//double distH = s1.lla().distanceH(s2.lla());
				//f.pln(" $$$ NavPoint.initialVelocity: distH = "+Units.str("ft",distH,14));
				return GreatCircle.velocity_initial(s1.p.lla(), s2.p.lla(), dt);
			} else {
				Velocity vel = Velocity.make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
				return vel;
			}
		} else {
			//f.pln("WARNING: NavPoint INITIAL VELOCITY negative time! dt = "+dt+" this="+s1+" s="+s2);
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
	 * @return velocity
	 */ 
	public Velocity initialVelocity(NavPoint s) {
		return initialVelocity(this, s);
	}

	
	
	/** 
	 * Calculate and return the final velocity between two NavPoint
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 *
	 * @param s1 a NavPoint
	 * @param s2 another NavPoint
	 * @return velocity
	 */ 
	public static Velocity finalVelocity(NavPoint s1, NavPoint s2) {
		if (debug && s1.p.isLatLon() != s2.isLatLon()) {
			throw new RuntimeException("Incompatible geometries in velocity()");
		}
		double dt = s2.time() - s1.time();
		//f.pln(" $$$$ NavPoint.initialVelocity: dt = "+dt);
		if (dt==0) {
			return Velocity.ZERO;
		} else if (dt > 0) {			
			if (s2.isLatLon()) {
				return GreatCircle.velocity_final(s1.p.lla(), s2.p.lla(), dt);
			} else {
				return Velocity.make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
			}
		} else {
			f.pln("WARNING: NavPoint INITIAL VELOCITY negative time!  Check the code here!");
			f.pln("NavPoint.initialVelocity this="+s1+" s="+s2);
			//DebugSupport.halt();
			if (s2.isLatLon()) {
				return GreatCircle.velocity_final(s2.p.lla(), s1.p.lla(), -dt);
			} else {
				return Velocity.make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
			}			
		}
	}
	

	/** 
	 * Calculate and return the vertical speed between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 * @return vertical speed
	 */ 
	public double  verticalSpeed(NavPoint s) {
		return (s.alt() - this.alt())/((s.time() - this.time()));
	}

	/** 
	 * Calculate and return the REPRESENTATIVE LINEAR velocity between two NavPoints. 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s1 a NavPoint
	 * @param s2 another NavPoint
	 * @return velocity
	 */ 
	public static Velocity averageVelocity(NavPoint s1, NavPoint s2) {
		if (debug && s1.isLatLon() != s2.isLatLon()) {
			throw new RuntimeException("Incompatible geometries in velocity()");
		}
		double dt = s2.time() - s1.time();
		if (dt==0) {
			return Velocity.ZERO;
		} else if (dt > 0) {

			if (s2.isLatLon()) {
				return GreatCircle.velocity_average(s1.p.lla(), s2.p.lla(), dt);
			} else {
				return Velocity.make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
			}
		} else {
			if (debug) {
				f.pln("WARNING: NavPoint AVERAGE VELOCITY negative time!  Check the code here!");
				f.pln("NavPoint.averageVelocity this="+s1+" s="+s2);
				Thread.dumpStack();
			}
			if (s2.isLatLon()) {
				return GreatCircle.velocity_average(s2.p.lla(), s1.p.lla(), -dt);
			} else {
				return Velocity.make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
			}
		}
	}


	/** Perform a linear projection of the current NavPoint with given velocity and time.  If the 
	 * current NavPoint is in LatLonAlt, then a constant track angle linear projection is used.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 * 
	 * @param v
	 * @param time  relative time 
	 * @return a linear extrapolation NavPoint
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

	/** Horizontal distance between this point and the other given point
	 * 
	 * @param np2 another point
	 * @return horizontal distance
	 * */
	public double distanceH(NavPoint np2) {
		return np2.position().distanceH(this.position());
	}

	/** Vertical distance between this point and the other given point
	 * 
	 * @param np2 another point
	 * @return vertical distance
	 * */
	public double distanceV(NavPoint np2) {
		return np2.position().distanceV(this.position());
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((label == null) ? 0 : label.hashCode());
		result = prime * result + ((p == null) ? 0 : p.hashCode());
		long temp;
		temp = Double.doubleToLongBits(t);
		result = prime * result + (int) (temp ^ (temp >>> 32));
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		NavPoint other = (NavPoint) obj;
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
		return true;
	}


	/** String representation, using the give precision
	 * 
	 *  @param precision number of digits of precision
	 *  @return a string representation
	 *  */
	public String toStringShort(int precision) {
		return p.toStringNP(precision) + ", " + f.FmPrecision(t,precision); 
	}
	
	/** String representation, using the default output precision (see Contants.get_output_precision()) 
	 * 
	 * @return a string representation
	 * */
	public String toString() {
		return toString(Constants.get_output_precision());
	}

	/** String representation, using the give precision 
	 * 
	 * @param precision number of digits of precision
	 * @return a string representation*/
	public String toString(int precision) {
		return p.toStringNP(precision) + ", " + f.FmPrecision(t,precision) +" "+f.padRight(label,4);
	}

}

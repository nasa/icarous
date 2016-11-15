/* SimpleProjection
 * 
 * Contact: George Hagen (george.hagen@nasa.gov)
 * 
 * project spherical coordinates (lat/lon) into a Euclidean frame and the inverse.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;


/**
 * This class creates a local Euclidean projection around a given point.  This projection may be used to
 * transform geodesic coordinates (LatLonAlt objects) into this Euclidean frame, using the project() method.  Also points
 * within this frame, may be found in geodesic coordinates with the inverse() method.   As long as the points are
 * close to the projection point, the errors will be very small.
 * 
 * This projection has the property that the Euclidean distance in the XY
 * frame is equal to the great circle distance in the lat/lon frame. In
 * addition, the course from the origin to the returned x,y point is the
 * same as the course at the mid point on the great circle arc.
 *
 * This projection has special code for when positions are near the poles.
 *  
 * Note: projection objects should never be made directly, and instead should be retrieved via Projection.getProjection()
 * 
 */
public final class SimpleProjection implements EuclideanProjection {

	//    private double projAlt;

	private final double projLat;
	private final double projLon;
	private final double projAlt;
	private final boolean projNorth;


	public static final double tranLat = Units.from("deg", 85.0);   

    /** Create a projection around the given reference point. */
	public SimpleProjection(LatLonAlt lla) {
		projLat = lla.lat();
		projLon = lla.lon();
		projAlt = lla.alt();
		projNorth = projLat >= 0.0;
	}

    /** Create a projection around the given reference point. */
	public SimpleProjection(double lat, double lon, double alt) {
		projLat = lat;
		projLon = lon;
		projAlt = alt;
		projNorth = projLat >= 0.0;
	}

    /** Return a new projection with the given reference point */
	public EuclideanProjection makeNew(LatLonAlt lla) {
		return new SimpleProjection(lla);
	}

    /** Return a new projection with the given reference point */
	public EuclideanProjection makeNew(double lat, double lon, double alt) {
		return new SimpleProjection(lat, lon, alt);
	}



	public double conflictRange(double lat, double accuracy) {
    	double degs = Math.abs(Units.to("deg",lat));
    	if (accuracy < Units.from("nm", 0.1)) { //0.01 -- 35 nm - 3 nm
    		if (degs < 30) {
    			return Units.from("nm",10);
    		} else {
    			return Units.from("nm",5);
    		}
    	} else if (accuracy < Units.from("nm", 0.5)) {  // 0.1 nm -- 185 nm - 4 nm
    		if (degs < 20) {
    			return Units.from("nm", 40);
    		} else if (degs < 50) {
    			return Units.from("nm", 25);
    		} else if (degs < 70) {
    			return Units.from("nm", 15);
    		} else if (degs < 80) {
    			return Units.from("nm", 10);
    		} else {
    			return Units.from("nm", 5);
    		}    		
    	} else { // 0.5 nm -- 330 nm - 13 nm
    		if (degs < 20) {
    			return Units.from("nm", 95);
    		} else if (degs < 50) {
    			return Units.from("nm", 50);
    		} else if (degs < 70) {
    			return Units.from("nm", 20);
    		} else if (degs < 80) {
    			return Units.from("nm", 10);
    		} else {
    			return Units.from("nm", 5);
    		}
    	}
	}

	/**
	 *  What is the maximum effective horizontal range of this projection? [m] 
	 */
	public double maxRange() {
		return Double.MAX_VALUE;
	}

	/** Get the projection point for this projection */
    public LatLonAlt getProjectionPoint() {
        return LatLonAlt.mk(projLat, projLon, projAlt);
    }

    /** Is the reference point near a pole? */
	public boolean isPolar() {
		return (Math.abs(projLat) >  tranLat);
	}

	//    public void setAltitude(double alt) {
	//    	projAlt = alt;
	//    }

    /** Return a projection with the pole as a reference point. */
	public static Vect2 polar_xy(LatLonAlt lla, boolean north) {
		int sgn = 1;
		if (! north) sgn = -1;
		double a = sgn*Units.from("deg",90.0) - lla.lat();
		double r = Math.abs(GreatCircle.distance_from_angle(a, 0.0));
		//f.pln("^^ polarXY: a = "+Units.to("deg",a)+" r = "+Units.to("nm",r));

		return new Vect2(r*Math.sin(lla.lon()),r*Math.cos(lla.lon()));
	}

    /** Invert a projection, using the pole as a reference point. */
	public static LatLonAlt polar_inverse(Vect2 v, double alt, boolean north) {
		double lon = Util.to_pi(v.compassAngle());
		double d = v.norm();
		double a = GreatCircle.angle_from_distance(d, 0.0);
		//f.pln("^^ polarLL: a = "+Units.to("deg",a)+" d = "+Units.to("nm",d));
		int sgn = 1;
		if (! north) sgn = -1;
		double lat = sgn*(Units.from("deg",90.0) - a);
		//f.pln("^^ polarLL: lat = "+Units.to("deg",lat)+" lon = "+Units.to("deg",lon));
		return LatLonAlt.mk(lat,lon,alt);
	}

    /** Return a projection of a lat/lon(/alt) point in Euclidean 2-space */
	public Vect2 project2(LatLonAlt lla) {
		if (Math.abs(projLat) >  tranLat) {
			return polar_xy(lla, projNorth);
		} else {
			return SimpleProjection.projectXY(projLat,projLon,lla.lat(),lla.lon());
		}
	}

    /** Return a projection of a lat/lon(/alt) point in Euclidean 3-space */
	public Vect3 project(LatLonAlt lla) {
		return new Vect3(project2(lla),lla.alt() - projAlt);
	}


	// does a projection if lat/lon else identity
    /** Return a projection of a Position in Euclidean 3-space (if already in Euclidian coordinate, this is the identity function) */
	public Vect3 project(Position sip) {
		Vect3 si;
		if (sip.isLatLon()) {
			si = project(sip.lla());
		} else {
			si = sip.point();
		}
		return si;
	}

    /** Return a projection of a Position in Euclidean 3-space (if already in Euclidian coordinate, this is the identity function) */
	public Point projectPoint(Position sip) {
		Vect3 si;
		if (sip.isLatLon()) {
			si = project(sip.lla());
		} else {
			si = sip.point();
		}
		return Point.mk(si);
	}


    /** Return a LatLonAlt value corresponding to the given Euclidean position */
	public LatLonAlt inverse(Vect3 xyz) {
		return inverse(xyz.vect2(), xyz.z);
	}


    /** Return a LatLonAlt value corresponding to the given Euclidean position */
	public LatLonAlt inverse(Vect2 xy, double alt) {
		if (Math.abs(projLat) >  tranLat)
			return polar_inverse(xy, alt + projAlt, projNorth);
		else {
			double a = xy.compassAngle();
			double d = xy.norm();
			//f.pln(" a = "+Units.to("deg",a)+" d = "+Units.to("nm",d));
			LatLonAlt origin = LatLonAlt.mk(projLat, projLon, alt + projAlt);
			//           return GreatCircle.linear_rhumb(origin,a,d);
			return GreatCircle.linear_initial(origin,a,d);
		}
	}



	//    public Vect2 project(double lat, double lon) {
	//    	LatLonAlt lla = LatLonAlt.make(lat,lon,0.0); 
	//    	return project(lla);
	//    }

	private static final double TIME_STEP = 10.0;

	// note: this handles the tranLat check at the position level
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
	public Velocity projectVelocity(LatLonAlt lla, Velocity v) {
		LatLonAlt ll2 = GreatCircle.linear_initial(lla,v,TIME_STEP);
		Vect3 se = project(lla);
		Vect3 s2 = project(ll2);
		Vect3 vn = s2.Sub(se).Scal(1/TIME_STEP);
		return Velocity.make(vn);
	}

    /** Given a velocity from a point, return a projection of this velocity in Euclidean 3-space  (if already in Euclidian coordinate, this is the identity function) */
	public Velocity projectVelocity(Position ss, Velocity v) {
		Velocity vn;
		if (ss.isLatLon()) {
			vn = projectVelocity(ss.lla(),v);
		} else {
			vn = v;
		}
		return vn;
	}

    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity.  If toLatLon is true, the velocity is projected into the geodetic coordinate space */
	public Velocity inverseVelocity(Vect3 s, Velocity v, boolean toLatLon){
		if (toLatLon) {
			Vect3 s2 = s.linear(v,TIME_STEP);
			LatLonAlt lla1 = inverse(s);
			LatLonAlt lla2 = inverse(s2);
			Velocity nv = GreatCircle.velocity_initial(lla1,lla2,TIME_STEP);
			return nv;
		} else {
			return v;
		}
	}


	/**
	 * Project both a position and a velocity at that position into a Euclidean reference frame 
	 */
	public Pair<Vect3,Velocity> project(Position p, Velocity v) {
		return new Pair<Vect3,Velocity>(project(p),projectVelocity(p,v));
	}

	/**
	 * Find the lat/lon/alt position and velocity from a given Euclidean reference frame.  
	 */
	public Pair<Position,Velocity> inverse(Vect3 p, Velocity v, boolean toLatLon) {
		if (toLatLon) {
			return new Pair<Position,Velocity>(new Position(inverse(p)),inverseVelocity(p,v,true));
		} else {
			return new Pair<Position,Velocity>(new Position(p),v);
		}

	}



	/**
	 * This method performs a particular projection from a spherical Earth
	 * latitude/longitude coordinate system to Euclidean (XY) one. Lat/long #0
	 * are placed at the origin of the XY coordinate system. The returned value
	 * is an XY position of point #1 relative to point #0.
	 * <p>
	 * 
	 * This projection has the property that the Euclidean distance in the XY
	 * frame is equal to the great circle distance in the lat/lon frame. In
	 * addition, the course from the origin to the returned x,y point is the
	 * same as the course at the mid point on the great circle arc.
	 * <p>
	 * 
	 * This transform has a symmetric correspondence, that is, it doesn't matter
	 * which point is the origin: <tt>projectXY(lat0,lon0,lat1,lon1) =
	 * -projectXY(lat1,lon1,lat0,lon0)</tt>
	 */
	public static Vect2 projectXY(double lat0, double lon0, double lat1, double lon1) {
		double ad = GreatCircle.angular_distance(lat0, lon0, lat1, lon1);
		if (Constants.almost_equals_radian(ad) ) {
			// If the two points are about 1 meter apart, then count them as the
			// same.
			return new Vect2(0.0, 0.0);
		} 

		LatLonAlt p0 = LatLonAlt.mk(lat0, lon0, 0.0);
		LatLonAlt p1 = LatLonAlt.mk(lat1, lon1, 0.0);

		double a;
		if (Constants.almost_equals_radian(ad / 2.0)) {
			// The two points are about 2 meter apart, so skip the midpoint
			a = GreatCircle.initial_course(p0, p1);
		} else {
			LatLonAlt mid = GreatCircle.interpolate(p0, p1, 0.5);
			a = GreatCircle.initial_course(mid, p1);
		}
		double d = GreatCircle.distance_from_angle(ad, 0.0);
		return new Vect2(d * Math.sin(a), d * Math.cos(a));
	}

	/**
	 * This method performs a particular projection from a spherical Earth
	 * latitude/longitude coordinate system to Euclidean (XYZ) one. Lat/long #0
	 * are placed at the origin of the Euclidean coordinate system. The returned value
	 * is the position of point #1 relative to point #0.
	 * <p>
	 * 
	 * This projection has the property that the Euclidean distance in the XY
	 * frame is equal to the great circle distance in the lat/lon frame. In
	 * addition, the course from the origin to the returned x,y point is the
	 * same as the course at the mid point on the great circle arc.
	 * <p>
	 * 
	 * This transform has a symmetric correspondence, that is, it doesn't matter
	 * which point is the origin: <tt>projectXY(lat0,lon0,lat1,lon1) =
	 * -projectXY(lat1,lon1,lat0,lon0)</tt>
	 */
	public static Vect3 projectXYZ(LatLonAlt p0, LatLonAlt p1) {
		Vect2 r = projectXY(p0.lat(), p0.lon(), p1.lat(), p1.lon());
		return new Vect3(r.x, r.y, p1.alt() - p0.alt());
	}


    /** String representation */
	public String toString() {
		return "SimpleProjection "+projLat+" "+projLon+" "+projAlt;
	}


}

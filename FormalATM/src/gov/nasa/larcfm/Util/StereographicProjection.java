/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;


public class StereographicProjection implements EuclideanProjection {

	//    private double projAlt;

	private final double projLat;
	private final double projLon;
	private final double projAlt;

    //private static final double earthRadius = GreatCircle.spherical_earth_radius;
	
    
    public static final double EARTH_RADIUS = Units.from("nmi",3444.046647);    // nmi

    /** Create a projection around the given reference point. */
	public StereographicProjection(LatLonAlt lla) {
		projLat = lla.lat();
		projLon = lla.lon();
		projAlt = lla.alt();
	}

    /** Create a projection around the given reference point. */
	public StereographicProjection(double lat, double lon, double alt) {
		projLat = lat;
		projLon = lon;
		projAlt = alt;
	}

    /** Return a new projection with the given reference point */
	public EuclideanProjection makeNew(LatLonAlt lla) {
		return new StereographicProjection(lla);
	}

    /** Return a new projection with the given reference point */
	public EuclideanProjection makeNew(double lat, double lon, double alt) {
		return new StereographicProjection(lat, lon, alt);
	}



	public double conflictRange(double lat, double accuracy) {
		f.pln(" $$$$$ NOT YET IMPLEMENTED!");
		return Units.from("NM",50);
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

    /** Return a stereographic projection of a lat/lon(/alt) point in Euclidean 2-space */
    public Vect2 project2(LatLonAlt lla) {
    	double lat = lla.lat(); 
    	double lon = lla.lon();
    	double cons = (2.0/(1.0 + Math.sin(projLat)*Math.sin(lat) + Math.cos(projLat)*Math.cos(lat) *Math.cos(lon - projLon)));
    	double xp = (double) (EARTH_RADIUS*cons*Math.cos(lat)*Math.sin(lon - projLon));
    	double yp = (double) (EARTH_RADIUS*cons*(Math.cos(projLat) *Math.sin(lat) -    		        	
    			               Math.sin(projLat) *Math.cos(lat) *Math.cos(lon - projLon)));
    	return new Vect2(xp, yp);
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
		double x = xy.x(); 
		double y = xy.y();
		double p = Math.sqrt((Math.pow(x, 2) + Math.pow(y, 2)));
		if (Util.almost_equals(p,0.0)) return LatLonAlt.mk(projLat,projLon,alt+projAlt);
		double c = 2.0*Math.atan(p/(2.0*EARTH_RADIUS));
		//f.pln(" $$$ Stereographic.inverse: p = "+p+" c = "+c);
		double lon = projLon + Math.atan((x*Math.sin(c))/(p*Math.cos(projLat)*Math.cos(c) - y*Math.sin(projLat)*Math.sin(c)));
		double lat = Math.asin(Math.cos(c)*Math.sin(projLon) + (y*Math.sin(c)*Math.cos(projLon))/p);
		//f.pln(" $$$ Stereographic.inverse: lon = "+lon+" lat = "+lat);
		return LatLonAlt.mk(lat, lon, alt);
	}

	// note: this handles the tranLat check at the position level
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
	public Velocity projectVelocity(LatLonAlt lla, Velocity v) {
		return v;
	}

    /** Given a velocity from a point, return a projection of this velocity in Euclidean 3-space  (if already in Euclidian coordinate, this is the identity function) */
	public Velocity projectVelocity(Position ss, Velocity v) {
		return v;
	}

    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity.  If toLatLon is true, the velocity is projected into the geodetic coordinate space */
	public Velocity inverseVelocity(Vect3 s, Velocity v, boolean toLatLon){
		return v;
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


    /** String representation */
	public String toString() {
		return "StereographicProjection "+projLat+" "+projLon+" "+projAlt;
	}


}

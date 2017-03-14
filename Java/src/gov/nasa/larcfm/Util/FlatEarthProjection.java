/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;



/**
 *   FlatEarth Projection
 *   This projection surface is tangent at some point (projLat, projLon).
 *
 *   Uses a simple arclength routine that computes dy along a meridian, and dx along a latitude circle.  
 *   
 *   See John Snyder, Map Projections used by the USGS, Bulletin 1532, 2nd edition (1983), p 145
 *   Note: code adapted from Unidata software
 */

public final class FlatEarthProjection implements EuclideanProjection {

	//    private double projAlt;

	private final double projLat;
	private final double projLon;
	private final double projAlt;

    //private static final double earthRadius = GreatCircle.spherical_earth_radius;
    private static final double earthRadius = 6371229.;  // canonical radius of the spherical earth
    private double radius = earthRadius; 
	
    /** Create a projection around the given reference point. 
     * 
     * @param lla reference point
     */
	public FlatEarthProjection(LatLonAlt lla) {
		projLat = lla.lat();
		projLon = lla.lon();
		projAlt = lla.alt();
	}

    /** Create a projection around the given reference point.
	 * 
	 * @param lat latitude of reference point
	 * @param lon longitude of reference point
	 * @param alt altitude of reference point
	 */
	public FlatEarthProjection(double lat, double lon, double alt) {
		projLat = lat;
		projLon = lon;
		projAlt = alt;
	}

    /** Return a new projection with the given reference point */
	public EuclideanProjection makeNew(LatLonAlt lla) {
		return new FlatEarthProjection(lla);
	}

    /** Return a new projection with the given reference point */
	public EuclideanProjection makeNew(double lat, double lon, double alt) {
		return new FlatEarthProjection(lat, lon, alt);
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

    /** Return a projection of a lat/lon(/alt) point in Euclidean 2-space */
    public Vect2 project2(LatLonAlt lla) {
    	double fromLat = lla.lat(); 
    	double fromLon = lla.lon();
    	double dy = radius * (fromLat - projLat);
    	double dx = radius * Math.cos(fromLat) * (fromLon - projLon);
     	return new Vect2(dx, dy);
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
        double toLon;
        double TOLERANCE = 1.0e-06;
        double xp = xy.x(); 
        double yp = xy.y();
        double toLat = projLat + yp / radius;
        double cosl = Math.cos(toLat);
        if (Math.abs(cosl) < TOLERANCE) {
            toLon = projLon;
        } else {
            toLon = projLon + xp / cosl / radius;
        }
        return LatLonAlt.mk(toLat, toLon, alt);

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
		return "FlatEarthProjection "+projLat+" "+projLon+" "+projAlt;
	}


}

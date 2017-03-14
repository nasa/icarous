/* SimpleProjection
 * 
 * Contact: George Hagen (george.hagen@nasa.gov)
 * 
 * project spherical coordinates (lat/lon) into a Euclidean frame and the inverse.
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
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
 * This is mimics the original projection function, and does not try to compensate for being near the poles. 
 *  
 * Note: projection objects should never be made directly, and instead should be retrieved via Projection.getProjection()
 * 
 */
public final class SimpleNoPolarProjection implements EuclideanProjection {

//    private double projAlt;
	
    private final double projLat;
    private final double projLon;
    private final double projAlt;


    /** Create a projection around the given reference point.
     * 
     * @param lla reference point
     */
    public SimpleNoPolarProjection(LatLonAlt lla) {
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
    public SimpleNoPolarProjection(double lat, double lon, double alt) {
        projLat = lat;
        projLon = lon;
        projAlt = alt;
    }
     
    /** Return a new projection with the given reference point */
    public EuclideanProjection makeNew(LatLonAlt lla) {
       return new SimpleNoPolarProjection(lla);
    }

    /** Return a new projection with the given reference point */
    public EuclideanProjection makeNew(double lat, double lon, double alt) {
    	return new SimpleNoPolarProjection(lat, lon, alt);
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
    
    /** Return a projection of a lat/lon(/alt) point in Euclidean 2-space */
    public Vect2 project2(LatLonAlt lla) {
          return SimpleProjection.projectXY(projLat,projLon,lla.lat(),lla.lon());
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
           double a = xy.compassAngle();
           double d = xy.norm();
           //f.pln(" a = "+Units.to("deg",a)+" d = "+Units.to("nm",d));
    	   LatLonAlt origin = LatLonAlt.mk(projLat, projLon, alt + projAlt);
           return GreatCircle.linear_initial(origin,a,d);
    }

    

    //private static final double TIME_STEP = 10.0;
      
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
    public Velocity projectVelocity(LatLonAlt lla, Velocity v) {
//   	    LatLonAlt ll2 = GreatCircle.linear_initial(lla,v,TIME_STEP);
//	    Vect3 se = project(lla);
//	    Vect3 s2 = project(ll2);
//	    Vect3 vn = s2.Sub(se).Scal(1/TIME_STEP);
//	    return Velocity.make(vn);
   	return v;
    }
    
    /** Given a velocity from a point, return a projection of this velocity in Euclidean 3-space  (if already in Euclidian coordinate, this is the identity function) */
    public Velocity projectVelocity(Position ss, Velocity v) {
//    	Velocity vn;
//		if (ss.isLatLon()) {
//           vn = projectVelocity(ss.lla(),v);
//		} else {
//			vn = v;
//		}
//		return vn;
    	return v;
    }
    
    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity.  If toLatLon is true, the velocity is projected into the geodetic coordinate space */
    public Velocity inverseVelocity(Vect3 s, Velocity v, boolean toLatLon){
		return v;
//
//    	if (toLatLon) {
//    	  Vect3 s2 = s.linear(v,TIME_STEP);
//    	  LatLonAlt lla1 = inverse(s);
//    	  LatLonAlt lla2 = inverse(s2);
//    	  return GreatCircle.velocity_initial(lla1,lla2,TIME_STEP);
//    	} else {
//    	  return v;
//    	}
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
    	return "SimpleNoPolarProjection: projLat = "+Units.str("deg",projLat)+"  projLon = "+Units.str("deg",projLon)+"  projAlt = "+Units.str("ft",projAlt);
    }

    
}

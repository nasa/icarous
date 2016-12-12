/* OrthographicProjection
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
 * transform geodetic coordinates (LatLonAlt objects) into this Euclidean frame, using the project() method.  Also points
 * within this frame, may be found in geodetic coordinates with the inverse() method.   As long as the points are
 * close to the projection point, the errors will be very small.
 * 
 * This is a standard orthographic projection, and is functionally similar to the ENU projection (it uses different calculations).  
 * It distorts distances from  the tangent point, compressing them more as the distance increases (similar to a picture of a 
 * planet from distant space).  
 * Note that this  projection should be considered to have a hard limit of GreatCircle.spherical_earth_radius as the maximum allowed 
 * distance from the tangent reference point, and significant distortion may occur at much smaller distances.
 * 
 * These formulas are taken from https://en.wikipedia.org/wiki/Orthographic_projection_in_cartography
 * 
 * Note: projection objects should never be made directly, and instead should be retrieved via Projection.getProjection() 
 * 
 */
public final class OrthographicProjection implements EuclideanProjection {

	
    private final double projAlt;
 	private final LatLonAlt llaRef;


    public static final double tranLat = Units.from("deg", 85.0);   
    
    /** Create a projection around the given reference point. */
    public OrthographicProjection(LatLonAlt lla) {
        projAlt = lla.alt();
        llaRef = lla;
    }
 
    /** Create a projection around the given reference point. */
    public OrthographicProjection(double lat, double lon, double alt) {
        projAlt = alt;
        llaRef = LatLonAlt.mk(lat, lon, alt);
    }
    
    
    /** Return a new projection with the given reference point */
    public EuclideanProjection makeNew(LatLonAlt lla) {
        return new OrthographicProjection(lla);
    }
 
    /** Return a new projection with the given reference point */
    public EuclideanProjection makeNew(double lat, double lon, double alt) {
        return new OrthographicProjection(lat, lon, alt);
    }    
    
    public double conflictRange(double lat, double accuracy) {
    	// the following is based on a fitting of 1000000 random scenarios at each distance 1..500 NMI, with error at intersection
    	// (ownship is projection center)
    	return Units.from("NM", Math.floor(243.0*Math.pow(Units.to("NM",Math.ceil(accuracy)),1.0/3.0))); // was 243.5, but this is slightly too high in some cases

    	// curve fit for distance-to-accuracy function (from NM to NM) is 6.8e-8*x^3 + 8.5e-07*x^2 - 0.00011*x + 0.0028
    }
    
    

    
    // this seems to break horribly at distances greater than 3400 nmi
	/**
	 *  What is the maximum effective horizontal range of this projection? [m] 
	 */
    public double maxRange() {
    	return Units.from("nm", 3400);
    }
    
	/** Get the projection point for this projection */
    public LatLonAlt getProjectionPoint() {
      return llaRef;
    }
    
    /** Return a projection of a lat/lon(/alt) point in Euclidean 2-space */
    public Vect2 project2(LatLonAlt lla) {
		return spherical2xy(lla.lat(),lla.lon());
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
		return xy2spherical(xy.x(), xy.y(), alt+projAlt);
    }

	
    private static final double TIME_STEP = 10.0;
      
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
    public Velocity projectVelocity(LatLonAlt lla, Velocity v) {
   	    LatLonAlt ll2 = GreatCircle.linear_initial(lla,v,TIME_STEP);
	    Vect3 se = project(lla);
	    Vect3 s2 = project(ll2);
//System.out.println("lla="+lla+" v="+v+" ll2="+ll2+" se="+se+" s2="+s2);	    
	    Vect3 vn = s2.Sub(se).Scal(1/TIME_STEP);
	    return Velocity.make(vn);
    }
    
    /** Given a velocity from a point, return a projection of this velocity in Euclidean 3-space  (if already in Euclidian coordinate, this is the identity function) */
    public Velocity projectVelocity(Position ss, Velocity v) {
    	if (ss.isLatLon()) {
          return projectVelocity(ss.lla(),v);
    	} else {
    		return v;
    	}
    }
    
    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity.  If toLatLon is true, the velocity is projected into the geodetic coordinate space */
    public Velocity inverseVelocity(Vect3 s, Velocity v, boolean toLatLon){
    	if (toLatLon) {
    	  Vect3 s2 = s.linear(v,TIME_STEP);
    	  return GreatCircle.velocity_initial(inverse(s),inverse(s2),TIME_STEP);
    	} else {
    		return v;
    	}
    }
    
    /**
     * Transforms a lat/lon position to a point on in R3 (on a sphere)
     * From Wikipedia https://en.wikipedia.org/wiki/Orthographic_projection_in_cartography
     * @param lat Latitude
     * @param lon Longitude
     * @return point in R2
     */
    private Vect2 spherical2xy(double lat, double lon) {
    	// remove invalid points
    	double c = Math.sin(llaRef.lat())*Math.sin(lat)+Math.cos(llaRef.lat())*Math.cos(lat)*Math.cos(lon-llaRef.lon());
    	if (c < 0) return Vect2.INVALID;
    	
    	double r = GreatCircle.spherical_earth_radius;
    	double x = r*Math.cos(lat)*Math.sin(lon-llaRef.lon());
    	double y = r*(Math.cos(llaRef.lat())*Math.sin(lat)-Math.sin(llaRef.lat())*Math.cos(lat)*Math.cos(lon-llaRef.lon()));
    	return new Vect2(x,y);
    }

    private LatLonAlt xy2spherical(double x, double y, double alt) {
    	double r = GreatCircle.spherical_earth_radius;
    	double p = Util.sqrt_safe(Util.sq(x)+Util.sq(y));
    	if (Util.almost_equals(p, 0.0)) return llaRef.mkAlt(alt);
    	double c = Util.asin_safe(p/r);
    	double lat = Util.asin_safe(Math.cos(c)*Math.sin(llaRef.lat()) + y*Math.sin(c)*Math.cos(llaRef.lat())/p);
    	double lon = llaRef.lon()+Util.atan2_safe(x*Math.sin(c), p*Math.cos(c)*Math.cos(llaRef.lat())-y*Math.sin(c)*Math.sin(llaRef.lat()));
    	return LatLonAlt.mk(lat, lon, alt);
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
    	return "ENUProjection "+projAlt+" "+llaRef;
    }

    
}

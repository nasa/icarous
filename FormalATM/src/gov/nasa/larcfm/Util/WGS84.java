/* 
 * WGS84
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/** 
 * This class contains values and formulas used for a WGS-84 reference ellipsoid.<p>
 * This is based on "Department of Defense World Geodetic System 1984", NIMA 
 * TR8350.2, Third Edition, 4 July 1997.<p>
 *
 * Notes:
 * <ul>
 * <li>The Earth does not rotate.  This may change.
 * <li>Positive latitude is north and positive longitude is east. 
 * </ul>
 */
public final class WGS84 {
    
    /** WGS-84 earth flattening parameter.  minor = major*(1 - flattening) */
    public static final double flattening = 1 / 298.257223563; // unitless
    /** The semi-major axis from the WGS 84 reference ellipsoid.  This corresponds to the equatorial radius, +/- 2 meters. */
    public static final double major = Units.from(Units.m, 6378137.0000);   // (+ or -) 2m
    /** Computation of the WGS 84 semi-minor axis: major*(1 - flattening) */
    public static final double minor = major*(1 - flattening);  // 6.35675e+006 m = 3432.37 nmi
    /** Angular velocity of the Earth */
    public static final double omega = Units.from(Units.radian_per_second,7292115.0e-11);
    /** Earth's gravitational constant, including atmosphere */
    public static final double GM = Units.from(Units.m*Units.m*Units.m/(Units.s*Units.s),3986004.418e8);
    /** Earth's mass */
    public static final double Me = Units.from(Units.kg, 5.9733328e24);
    
    
    /** First eccentricity squared */
    private static final double e2 = flattening*(2-flattening);    
    ///** First eccentricity  */
    //private static final double e = Math.sqrt(e2);    
    /** Theoretical (normal) gravity at the equator (on the ellipsoid) */
    private static final double gamma_e = Units.from(Units.meter_per_second2, 9.7803253359);      
    /** Theoretical (normal) gravity at the pole (on the ellipsoid) */
    private static final double gamma_p = Units.from(Units.meter_per_second2, 9.8321849378);      
    /** Theoretical (normal) gravity formula constant, page 4-1 */
    private static final double k = minor*gamma_p / (major * gamma_e) - 1;
    /** omega^2*major^2*minor/GM, page 3-7 */
    private static final double m = omega*omega*major*major*minor/GM;


    private WGS84() { // never construct one of these
    }

    /**
     * gravitational acceleration at the surface as a function of latitude, page 4-1.
     */
	public static double gravity(double lat) {
		double lat_sine_sq = Util.sq(Math.sin(lat));
    	return gamma_e*((1+k*lat_sine_sq)/Math.sqrt(1-e2*lat_sine_sq));
    }
    	
    /**
     * gravitational acceleration as a function of latitude and height above the 
     * ellipsoid, page 4-1.  This equation should be valid for aircraft heights, orbital 
     * mechanics may need a better estimate.
     */
	public static double gravity(double lat, double height) {
		double g_surface = gravity(lat);
		double lat_sine_sq = Util.sq(Math.sin(lat));
		//double m = 0.0;
		return g_surface*(1-(2/major)*(1+flattening+m-2*flattening*lat_sine_sq)*height + (3/(major*major))*height*height);
    }
    	
    /**
     * gravitational acceleration as a function of the height above the 
     * ellipsoid at 45 degrees latitude.  This equation should be valid for aircraft 
     * heights, orbital mechanics may need a better estimate.
     */
	public static double gravity_alt(double height) {
		double g_surface = Units.from(Units.meter_per_second2, 9.806197769343779); //gravity(PI/4);
		double lat_sine_sq = 0.5; //Util.sq(Math.sin(PI/4));
		return g_surface*(1-(2/major)*(1+flattening+m-2*flattening*lat_sine_sq)*height + (3/(major*major))*height*height);
    }
    	
    /** 
     * This method performs a particular projection from a WGS-84 reference elipsoid
     * Earth latitude/longitude coordinate system to Euclidean (XY)
     * one.  Lat/long #0 are placed at the origin of the XY coordinate
     * system.  The returned value is an XY position of point #1
     * relative to point #0.  This projection is based on the 
     * Aviation Formulary by Ed Williams v1.44<p>
     *
     * Note: this projection does not have a symmetric correspondence, that is, 
     * <tt>project_xy(lat0,lon0,lat1,lon1) /=
     * -project_xy(lat1,lon1,lat0,lon0)</tt>
     */
    public static Vect2 project_xy(double lat0, double lon0, double lat1, double lon1) {
      double dlat = lat1 - lat0;
      double dlon = lon1 - lon0;
      double r32 = 1-e2*Util.sq(Math.sin(lat0));
      double R1 = major*(1-e2)/Util.sqrt_safe(r32*r32*r32);
      double R2 = major/Util.sqrt_safe(1-e2*Util.sq(Math.sin(lat0)));
      return new Vect2(R2*Math.cos(lat0)*dlon,
		       R1*dlat);
    } 

}

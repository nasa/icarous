/* 
 * GreatCircle
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by 
 * the National Aeronautics and Space Administration.  No copyright 
 * is claimed in the United States under Title 17, U.S.Code. All Other 
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/**
 * This class contains common formulas used for Great Circle calculations. All
 * of these calculations assume a spherical earth. Much of this is based on the
 * Aviation Formulary (v1.44) by Ed Williams.
 * <p>
 * 
 * Notes:
 * <ul>
 * <li>The Earth does not rotate. This may change.
 * <li>Positive latitude is north and positive longitude is east.
 * <li>All course angles (i.e., desired track angles) are in radians clockwise
 * from true north.
 * <li>When the returned value is in lat/long and it is a Vect2, latitude is in
 * the "x" position and longitude is in the "y" position.
 * <li>Great circles cannot be defined by antipodal points
 * </ul>
 */
public final class GreatCircle {

	private static final double pi = Math.PI;

	private static final double EPS = 1.0e-15; // small number, about machine
	// precision
	public static final double minDt = 1E-5;

	private GreatCircle() { // never construct one of these
	}

	/**
	 * Convert an angle in degrees/minutes/seconds into internal units
	 * (radians). The flag indicates if this angle is north (latitude) or east
	 * (longitude).  If the angle does not represent a latitude/longitude (it
	 * is only an angle), then set the north_east flag tbo true.<p>
	 * 
	 * If the degrees is negative (representing 
	 * south or west), then the flag is ignored.
	 */
	public static double decimal_angle(double degrees, double minutes,
			double seconds, boolean north_east) {
		if (degrees < 0.0) {
			degrees = Math.abs(degrees);
			north_east = false;
		}

		return ((north_east) ? 1.0 : -1.0)
				* Units.from(Units.degree,
						(degrees + minutes / 60.0 + seconds / 3600.0));
	}

	/** Convert a string in the form "dd:mm:ss" into a decimal angle, where dd represent degrees, 
	 * mm represents minutes, and ss represents seconds.  Examples include
	 * "46:55:00"  and "-111:57:00".
	 * 
	 * @param degMinSec  Lat/Lon string 
	 * @return numbers of degrees in decimal form
	 */
	public static double parse_degrees(String degMinSec) {
		String [] dms = degMinSec.split(":");
		double degrees = 0.0;
		double minutes = 0.0;
		double seconds = 0.0;
		int sgn = 1;

		if (dms.length >= 1) {
			degrees = Util.parse_double(dms[0]);
			sgn = Util.sign(degrees);
			if (dms.length >= 2) {				
				minutes = Util.parse_double(dms[1]);
				if (dms.length >= 3) {
					seconds = Util.parse_double(dms[2]);
				}
			}
		}
		return decimal_angle(degrees, minutes, seconds, sgn > 0.0);
	}

	 /**
	  * Parse strings like "40-51-35.490N" or "115-37-17.070W" into a double
	  * 
	  * @param degMinSec string representing the angle
	  * @return a floating point representation of the angle.
	  */
	public static double parse_degrees2(String degMinSec) {
		degMinSec = degMinSec.trim();
		boolean nOrE = degMinSec.endsWith("N") || degMinSec.endsWith("E");
		String[] components = degMinSec.substring(0,degMinSec.length()-1).split("-");
		double decimal = 0.0;
		if (components.length == 3) {
			decimal = GreatCircle.decimal_angle(Util.parse_double(components[0]),Util.parse_double(components[1]),Util.parse_double(components[2]), nOrE);
		}
		return decimal;
	 }
	
	/** Convert a "structured" double in the form "[-][d]ddmmss.ss" into a standard decimal angle, where [d]dd represent degrees, 
	 * mm represents minutes, and ss.ss represents seconds.  Examples include
	 * "0465500.00"  and "-115701.15".  
	 * 
	 * @param degMinSec  Lat/Lon "structured" double
	 * @return numbers of degrees in decimal form
	 */
	public static double parse_degrees(double degMinSec) {		
		boolean north_east = true;
		if (degMinSec < 0.0) {
			degMinSec = Math.abs(degMinSec);
			north_east = false;
		}

		double degrees = ((long)(degMinSec / 10000.0)); 
		double minutes = (double)((long)(degMinSec / 100.0)) - 100*degrees;
		double seconds = degMinSec - 10000.0*degrees - 100.0*minutes;

		return decimal_angle(degrees, minutes, seconds, north_east);
	}


	/**
	 * Convert the distance (in internal length units) across the surface of the
	 * (spherical) Earth into an angle.
	 */
	private static double angle_from_distance(double distance) {
		return Units.to(Units.NM, distance) * pi / (180.0 * 60.0);
	}

	/**
	 * The radius of a spherical "Earth" assuming 1 nautical mile is 1852 meters
	 * (as defined by various international committees) and that one nautical
	 * mile is equal to one minute of arc (traditional definition of a nautical
	 * mile) on the Earth's surface. This value lies between the major and minor 
	 * axis as defined by the "reference ellipsoid" in WGS84.
	 */
	public static final double spherical_earth_radius = Units.from(Units.m,
			1.0 / angle_from_distance(1.0));
	//public static final double spherical_earth_radius = Units.from(Units.NM,3440.065);

	/**
	 * Convert the distance (in internal length units) across a sphere at a
	 * height of h (in internal length units) above surface of the (spherical)
	 * Earth into an angle.
	 */
	public static double angle_from_distance(double distance, double h) {
		return angle_from_distance(distance * spherical_earth_radius
				/ (spherical_earth_radius + h));
	}

	/**
	 * Convert the given angle into a distance across a (spherical) Earth at
	 * height above the surface of h.
	 */
	public static double distance_from_angle(double angle, double h) {
		return (spherical_earth_radius + h) * angle;
	}

	/**
	 * Determines if two points are close to each other, see
	 * Constants.get_horizontal_accuracy().
	 */
	public static boolean almost_equals(double lat1, double lon1, double lat2,
			double lon2) {
		return Constants.almost_equals_radian(angular_distance(lat1, lon1,
				lat2, lon2));
	}

	/**
	 * Determines if two points are close to each other, where 'close'
	 * is defined by the distance parameter given in meters.
	 */
	public static boolean almost_equals(double lat1, double lon1, double lat2,
			double lon2, double epsilon) {
		return Util.within_epsilon(distance(lat1, lon1, lat2, lon2), epsilon);
	}

	/**
	 * Compute the great circle distance in radians between the two points. The
	 * calculation applies to any sphere, not just a spherical Earth. The
	 * current implementation uses the haversine formula.
	 */
	public static double angular_distance(double lat1, double lon1,
			double lat2, double lon2) {
		/*
		 * This function uses the haversine formula. Haversines were developed
		 * hundreds of years ago for navigators. A navigator with a haversine table 
		 * and its inverse (and standard trig tables) could compute the 
		 * distance between two lat/lon points without squaring or square roots.
		 * 
		 * Clearly with modern computers, we do not need haversines for this
		 * reason. Instead, the standard distance formula in spherical
		 * trigonometry is the "law of cosines." This formula has some quirks
		 * for Earth navigation. Specifically, if the two points are close to
		 * each other (within a nautical mile), then this formula involves
		 * taking an arccosine of a number very close to one, which will lead to
		 * inaccurate results. Haversines, since they use sines, do not suffer
		 * from this problem.
		 */

		return Util.asin_safe(Util.sqrt_safe(Util.sq(Math.sin((lat1 - lat2) / 2))
				+ Math.cos(lat1)
				* Math.cos(lat2)
				* Util.sq(Math.sin((lon1-lon2) / 2)))) * 2.0;
	}

	/**
	 * Compute the great circle distance in radians between the two points. The
	 * calculation applies to any sphere, not just a spherical Earth. This implementation
	 * is based on the law of cosines for spherical trigonometry. This version should be both
	 * less accurate when finding small distances and it should be faster, than
	 * the version in angular_distance().  It is less
	 * accurate for small distances; however, it is not faster, in fact, 
	 * it takes about twice the time.  It seems that sin's are 
	 * faster than cos's?  Investigations are continuing.
	 */
	public static double angular_distance_alt(double lat1, double lon1,
			double lat2, double lon2) {
        return Util.acos_safe(Math.cos(lat1 - lat2) +
                (Math.cos(lon1 - lon2) - 1.0) *
                Math.cos(lat2) * Math.cos(lat1));
    }
    
	/**
	 * Compute the great circle distance in radians between the two points. The
	 * calculation applies to any sphere, not just a spherical Earth. The
	 * current implementation uses the haversine formula.
	 */
	public static double angular_distance(LatLonAlt p1, LatLonAlt p2) {
		return angular_distance(p1.lat(), p1.lon(), p2.lat(), p2.lon());
	}

	/**
	 * Compute the great circle distance between the two given points. The
	 * calculation assumes the Earth is a sphere
	 */
	public static double distance(double lat1, double lon1, double lat2,
			double lon2) {
		return distance_from_angle(angular_distance(lat1, lon1, lat2, lon2),
				0.0);
	}

	/**
	 * Compute the great circle distance between the two given points. The
	 * calculation assumes the Earth is a sphere. This ignores the altitudes.
	 */
	public static double distance(LatLonAlt p1, LatLonAlt p2) {
		// return distance_from_angle(angular_distance(p1, p2),(p1.alt() + p2.alt())/2.0);
		return distance_from_angle(angular_distance(p1.lat(), p1.lon(), p2.lat(), p2.lon()), 0.0);
	}



	// parameter d is the angular distance between lat/lon #1 and lat/lon #2
	private static double initial_course_impl(LatLonAlt p1, LatLonAlt p2, double d) {
		double lat1 = p1.lat();
		double lon1 = p1.lon();
		double lat2 = p2.lat();
		double lon2 = p2.lon();
		
		if (Math.cos(lat1) < EPS) { 
			// EPS a small positive number, about machine precision
			if (lat1 > 0) {
				return pi; // starting from North pole, all directions are south
			} else {
				return 2.0 * pi; // starting from South pole, all directions are
				// north. JMM: why not 0?
			}
		}
		if (Constants.almost_equals_radian(d)) {
			return 0.0;
			// if the two points are almost the same, then any course is valid
			// returning 0.0 here avoids a 0/0 division (NaN) in the
			// calculations below.
			//
			// This check is used to guard if sin(d) is ever 0.0 in the 
			// division below.  sin(d) for d=PI is never zero given a floating
			// point representation of PI.  Therefore, we only need
			// to check if d == 0.0.  Thank you PVS!
		}

		double tc1;
		double acos1 = (Math.sin(lat2) - Math.sin(lat1) * Math.cos(d))
				/ (Math.sin(d) * Math.cos(lat1));
		if (Util.almost_equals(lon2, lon1, Util.PRECISION13)) {
			tc1 = Util.acos_safe(acos1);
		} else {
			if (Math.sin(lon2 - lon1) > 0) {
				// slightly different than aviation
				// formulary because of +East convention that we use
				tc1 = Util.acos_safe(acos1);
			} else {
				tc1 = 2 * pi - Util.acos_safe(acos1);
			}
		}
		return tc1;
	}

	/**
	 * The initial true course (course relative to true north) at lat/long #1 on
	 * the great circle route from lat/long #1 to lat/long #2. The value is in
	 * internal units of angles (radians), and is a compass angle [0..2*Pi]:
	 * clockwise from true north.
	 * <p>
	 * 
	 * Usage Note: If lat/long #1 and #2 are close to each other, then the
	 * initial course may become unstable. In the extreme case when lat/long #1
	 * equals lat/long #2, then the initial course is undefined.
	 */
	public static double initial_course(double lat1, double lon1, double lat2, double lon2) {
		LatLonAlt p1 = LatLonAlt.mk(lat1, lon1, 0.0);
		LatLonAlt p2 = LatLonAlt.mk(lat2, lon2, 0.0);
		return initial_course(p1, p2);
	}

	/**
	 * The initial true course (course relative to true north) at point #1 on
	 * the great circle route from point #1 to point #2. The value is in
	 * internal units of angles (radians), and is a compass angle [0..2*Pi]:
	 * clockwise from true north.
	 * <p>
	 * 
	 * Usage Note: If point #1 and #2 are close to each other, then the initial
	 * course may become unstable. In the extreme case when point #1 equals
	 * point #2, then the initial course is undefined.
	 */
	public static double initial_course(LatLonAlt p1, LatLonAlt p2) {
		double d = angular_distance(p1.lat(), p1.lon(), p2.lat(), p2.lon());
		return initial_course_impl(p1, p2, d);
	}
	
	public static double final_course(LatLonAlt p1, LatLonAlt p2) {
		return initial_course(p2, p1)+Math.PI;
	}

	/**
	 * A representative course (course relative to true north) for the entire
	 * arc on the great circle route from lat/long #1 to lat/long #2. The value
	 * is in internal units of angles (radians), and is a compass angle
	 * [0..2*Pi]: clockwise from true north. This is currently calculated as the
	 * initial course from the midpoint of the arc to its endpoint.
	 */
	public static double representative_course(double lat1, double lon1, double lat2, double lon2) {
		LatLonAlt p1 = LatLonAlt.mk(lat1, lon1, 0.0);
		LatLonAlt p2 = LatLonAlt.mk(lat2, lon2, 0.0);
		double d = angular_distance(lat1, lon1, lat2, lon2);
		LatLonAlt midPt = interpolate_impl(p1, p2, d, 0.5, 0.0);
		return initial_course_impl(midPt, p2, d / 2.0);
	}

	public static double representative_course(LatLonAlt p1, LatLonAlt p2) {
		return representative_course(p1.lat(), p1.lon(), p2.lat(), p2.lon());
	}

	// parameter d is the angular distance between lat/long #1 and #2
	// parameter f is a fraction between 0.0 and 1.0
	private static LatLonAlt interpolate_impl(LatLonAlt p1, LatLonAlt p2, double d, double f, double alt) {
		if (Constants.almost_equals_radian(d)) {
			return p1.mkAlt(alt);
			// if the two points are almost the same, then consider the two
			// points the same and arbitrarily return one of them (in this case
			// p1) with the altitude that was provided
		}
		double lat1 = p1.lat();
		double lon1 = p1.lon();
		double lat2 = p2.lat();
		double lon2 = p2.lon();
		double a = Math.sin((1 - f) * d) / Math.sin(d);
		double b = Math.sin(f * d) / Math.sin(d);
		double x = a * Math.cos(lat1) * Math.cos(lon1) + b * Math.cos(lat2)	* Math.cos(lon2);
		double y = a * Math.cos(lat1) * Math.sin(lon1) + b * Math.cos(lat2)	* Math.sin(lon2);
		double z = a * Math.sin(lat1) + b * Math.sin(lat2);
		return LatLonAlt.mk(Util.atan2_safe(z, Math.sqrt(x * x + y * y)), // lat
				Util.atan2_safe(y, x), // longitude
				alt); // alt
	}

	/**
	 * Find the position (latitude, longitude, and altitude) of a point on the
	 * great circle from point #1 to point #2 as a fraction of the distance
	 * between the two points. If the fraction is 0.0 then point #1 is returned,
	 * if the fraction is 1.0 then point #2 is returned. If a fraction less than
	 * zero or greater than one is used, then this function will extrapolate
	 * along the great circle.
	 * <p>
	 * 
	 * Usage Notes:
	 * <ul>
	 * <li>The return value r has r.x as latitude and r.y as longitude. This is
	 * different than in the Vect4 class.
	 * <li>Behavior of this function is undefined if the two points are
	 * antipodal (i.e. lat1+lat2=0 and abs(lon1-lon2)=pi) because a unique great
	 * circle line is undefined (there are infinitely many of them).
	 * <li>if lat/long #1 is almost the same as #2, then #1 is returned
	 * </ul>
	 */
	public static LatLonAlt interpolate(LatLonAlt p1, LatLonAlt p2, double f) {
		double d = angular_distance(p1, p2);
		return interpolate_impl(p1, p2, d, f, (p2.alt() - p1.alt()) * f + p1.alt());
	}

	/**
	 * This is a fast but crude way of interpolating between relatively close geodesic points
	 * 
	 * @param p1
	 * @param p2
	 * @param f
	 * @return
	 */
	public static LatLonAlt interpolateEst(LatLonAlt p1, LatLonAlt p2, double f) {
		return LatLonAlt.mk((p2.lat() - p1.lat()) * f + p1.lat(),
				(p2.lon() - p1.lon()) * f + p1.lon(),
				(p2.alt() - p1.alt()) * f + p1.alt());
	}

	/**
	 * Return the initial velocity at a point on the great circle that is between p1 and p2.
	 * @param p1 starting point
	 * @param p2 ending point
	 * @param time total time between p1 and p2
	 * @param f fraction of total time to place midpoint
	 * @return velocity at midpoint 
	 */
	public static Velocity interpolate_velocity(LatLonAlt p1, LatLonAlt p2, double time, double f) {
		LatLonAlt mid = interpolate(p1,p2,f);
		return velocity_initial(mid,p2,(1-f)*time);
	}



	/**
	 * @deprecated
	 * @Deprecated
	 * Solve the spherical triangle when one has a side (in angular distance), another side, and an angle between sides.
	 * The angle is <b>not</b> between the sides.  The sides are labeled a, b, and c.  The angles are labelled A, B, and
	 * C.  Side a is opposite angle A, and so forth.<p>
	 * 
	 * Given these constraints, in some cases two solutions are possible.  To
	 * get one solution set the parameter firstSolution to true, to get the other set firstSolution to false.  
	 * A firstSolution == true
	 * will return a smaller angle, B, than firstSolution == false.
	 * 
	 * @param b one side (in angular distance)
	 * @param a another side (in angular distance)
	 * @param A the angle opposite the side a 
	 * @param firstSolution select which solution to use
	 * @return a Triple of angles B and C, and the side c.
	 */
	public static Triple<Double,Double,Double> side_side_angle(double b, double a, double A, boolean firstSolution) {
		// This function follows the convention of "Spherical Trigonometry" by Todhunter, Macmillan, 1886
		//   Note, angles are labelled counter-clockwise a, b, c
		
		// Law of sines
		double B = Util.asin_safe(Math.sin(b)*Math.sin(A)/Math.sin(a));  // asin returns [-pi/2,pi/2]
		if ( ! firstSolution) {
			B = Math.PI - B; 
		}
		
		// one of Napier's analogies
		double c = 2 * Util.atan2_safe(Math.sin(0.5*(a+b))*Math.cos(0.5*(A+B)),Math.cos(0.5*(a+b))*Math.cos(0.5*(A-B)));
		
		// Law of cosines
		double C = Util.acos_safe(-Math.cos(A)*Math.cos(B)+Math.sin(A)*Math.sin(B)*Math.cos(c));
		
		if ( gauss_check(a,b,c,A,B,C)) {
			return new Triple<Double,Double,Double>(Util.to_pi(B),C,Util.to_2pi(c));
		} else {
			return new Triple<Double,Double,Double>(0.0,0.0,0.0);
		}
	}

	
	//TODO: consider if this should be deprecated
	/**
	 * 
	 * Solve the spherical triangle when one has a side (in angular distance), and two angles.
	 * The side is <b>not</b> between the angles.  The sides are labeled a, b, and c.  The angles are labelled A, B, and
	 * C.  Side a is opposite angle A, and so forth.<p>
	 * 
	 * Given these constraints, in some cases two solutions are possible.  To
	 * get one solution set the parameter firstSolution to true, to get the other set firstSolution to false.  A firstSolution == true
	 * will return a smaller side, b, than firstSolution == false.
	 * 
	 * @param a one side (in angular distance)
	 * @param A the angle opposite the side a 
	 * @param B another angle
	 * @param firstSolution select which solution to use
	 * @return a Triple of side b, angle C, and the side c.
	 */
	public static Triple<Double,Double,Double> side_angle_angle(double a, double A, double B, boolean firstSolution) {
		// This function follows the convention of "Spherical Trigonometry" by Todhunter, Macmillan, 1886
		//   Note, angles are labelled counter-clockwise a, b, c
		
		// Law of sines
		double b = Util.asin_safe(Math.sin(a)*Math.sin(B)/Math.sin(A));  // asin returns [-pi/2,pi/2]
		if ( ! firstSolution) {
			b = Math.PI - b;  
		}
		
		// one of Napier's analogies
		double c = 2 * Util.atan2_safe(Math.sin(0.5*(a+b))*Math.cos(0.5*(A+B)),Math.cos(0.5*(a+b))*Math.cos(0.5*(A-B)));
		
		// Law of cosines
		double C = Util.acos_safe(-Math.cos(A)*Math.cos(B)+Math.sin(A)*Math.sin(B)*Math.cos(c));
		
		if ( gauss_check(a,b,c,A,B,C)) {
			return new Triple<Double,Double,Double>(Util.to_2pi(b),Util.to_2pi(C),Util.to_2pi(c));
		} else {
			return new Triple<Double,Double,Double>(0.0,0.0,0.0);
		}
	}

	//TODO consider if this should be deprecated
	/**
	 * This implements the spherical cosine rule to complete a triangle on the unit sphere
	 * @param a side a (angular distance)
	 * @param C angle between sides a and b
	 * @param b side b (angular distance)
	 * @return triple of A,B,c (angle opposite a, angle opposite b, side opposite C)
	 */
	public static Triple<Double,Double,Double>side_angle_side(double a, double C, double b) {
		double c = Util.acos_safe(Math.cos(a)*Math.cos(b)+Math.sin(a)*Math.sin(b)*Math.cos(C));
		double cRatio = Math.sin(C)/Math.sin(c);
		double A = Util.asin_safe(Math.sin(a)*cRatio);
		double B = Util.asin_safe(Math.sin(b)*cRatio);
		return new Triple<Double,Double,Double>(A,B,c);
	}
	
	/**
	 * @deprecated
	 * @Deprecated
	 * This implements the supplemental (polar triangle) spherical cosine rule to complete a triangle on the unit sphere
	 * @param A angle A
	 * @param c side between A and B (angular distance
	 * @param B angle B
	 * @return triple of a,b,C (side opposite A, side opposite B, angle opposite c)
	 */
	public static Triple<Double,Double,Double>angle_side_angle(double A, double c, double B) {
		double C = Util.acos_safe(-Math.cos(A)*Math.cos(B)+Math.sin(A)*Math.sin(B)*Math.cos(c));
		double cRatio = Math.sin(c)/Math.sin(C);
		double a = Util.asin_safe(Math.sin(A)*cRatio);
		double b = Util.asin_safe(Math.sin(B)*cRatio);
		return new Triple<Double,Double,Double>(a,b,C);
	}
	
	private static boolean gauss_check(double a, double b, double c, double A, double B, double C) {
		// This function follows the convention of "Spherical Trigonometry" by Todhunter, Macmillan, 1886
		//   Note, angles are labelled counter-clockwise a, b, c
		A = Util.to_pi(A);
		B = Util.to_pi(B);
		C = Util.to_pi(C);
		a = Util.to_2pi(a);
		b = Util.to_2pi(b);
		c = Util.to_2pi(c);
		if (A==0.0 || A==Math.PI || B==0.0 || B==Math.PI || C==0.0 || C==Math.PI) return false;
		if (a==0.0 || b==0.0 || c==0.0) return false;
//		f.pln("gauss "+Math.cos(0.5*(A+B))*Math.cos(0.5*c)+" "+Math.cos(0.5*(a+b))*Math.sin(0.5*C));
		return Util.almost_equals(Math.cos(0.5*(A+B))*Math.cos(0.5*c),Math.cos(0.5*(a+b))*Math.sin(0.5*C),Util.PRECISION13);
	}

	
	/**
	 * Find a point on the great circle route from point #1 to point #2,
	 * traveling at the given velocity (only ground speed and vertical speed,
	 * not track angle) for the given amount of time. If points #1 and #2 are
	 * essentially the same, then the direction between these two points is
	 * undefined, so the first point is returned.
	 * <p>
	 * 
	 * This calculation ignores altitude. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 */
	public static LatLonAlt linear_gcgs(LatLonAlt p1, LatLonAlt p2, Velocity v,
			double t) {
		double d = angular_distance(p1, p2);
		if (Constants.almost_equals_radian(d)) {
			// If the two points are about 1 meter apart, then count them as the
			// same.
			return p1;
		}
		double f = angle_from_distance(v.gs() * t) / d;
		return interpolate_impl(p1, p2, d, f, p1.alt() + v.z * t);
	}

	/**
	 * Return a new location on the great circle path from p1 to p2 that is
	 * distance d from p1
	 * 
	 * @param p1   the first point to define the great circle
	 * @param p2   the second point to define the great circle
	 * @param d    distance from point #1 [m]
	 * @return a new position that is distance d from point #1
	 */
	public static LatLonAlt linear_gc(LatLonAlt p1, LatLonAlt p2, double d) {
		// return GreatCircle.linear_initial(p1, initial_course(p1,p2), d);
		double dist = angular_distance(p1, p2);
		double f = angle_from_distance(d) / dist;
		return interpolate_impl(p1, p2, dist, f,
				(p2.alt() - p1.alt()) * f + p1.alt());
	}

	/**
	 * Find a point from the given lat/lon when traveling at the given velocity
	 * for the given amount of time. This calculation follows the rhumb line
	 * (loxodrome or line of constant track).
	 * <p>
	 * 
	 * Modern aircraft (and most ships) usually travel great circles not rhumb
	 * lines, therefore linear_initial() is usually the preferred over this
	 * function.
	 * <p>
	 * 
	 * At "normal" latitudes, rhumb lines are usually within a few percent of
	 * the great circle route. However, near the poles the behavior of rhumb
	 * lines is not intuitive: if the destination is a point near the pole, then
	 * the rhumb line may spiral around the pole to get to the destination. In
	 * fact, if you maintain a constant track angle along a rhumb line for a
	 * long enough distance, gradually the line will spiral in towards one of
	 * the poles.
	 * <p>
	 * 
	 * Rhumb lines are not defined at the exact north and south poles, therefore
	 * if the origin or destination is precisely at a pole, this function will
	 * choose a point near the pole.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 */
	public static LatLonAlt linear_rhumb(LatLonAlt s, Velocity v, double t) {
		return linear_rhumb_impl(s, v.trk(), angle_from_distance(v.gs() * t),
				v.z * t);
	}

	/**
	 * Find a point from the given lat/lon at an angle of 'track' at a distance
	 * of 'dist'. This calculation follows the rhumb line (loxodrome or line of
	 * constant track).
	 * <p>
	 * 
	 * Modern aircraft (and most ships) usually travel great circles not rhumb
	 * lines, therefore linear_initial() is usually preferred over this
	 * function.
	 * <p>
	 * 
	 * At "normal" latitudes, rhumb lines are usually within a few percent of
	 * the great circle route. However, near the poles the behavior of rhumb
	 * lines is not intuitive: if the destination is a point near the pole, then
	 * the rhumb line may spiral around the pole to get to the destination. In
	 * fact, if you maintain a constant track angle along a rhumb line for a
	 * long enough distance, gradually the line will spiral in towards one of
	 * the poles.
	 * <p>
	 * 
	 * Rhumb lines are not defined at the exact north and south poles, therefore
	 * if the origin or destination is precisely at a pole, this function will
	 * choose a point near the pole.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 */
	public static LatLonAlt linear_rhumb(LatLonAlt s, double track, double dist) {
		return linear_rhumb_impl(s, track, angle_from_distance(dist), 0.0);
	}

	/**
	 * Find the final point from the given lat/lon when traveling along the great circle
	 * with the given <b>final</b> velocity for the given amount of time.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 */
	public static LatLonAlt linear_final(LatLonAlt s, Velocity v, double t, boolean firstSolution) {
		double c = GreatCircle.angle_from_distance(v.gs() * t);  // angular distance between initial and final point
		
		double B = v.trk() - Math.PI;
		
		double b;
		if (s.lat() > 0) {
			b = Math.PI/2 - s.lat();
		} else { 
			b = Math.PI/2 + s.lat();
		}

		// Solution #1
		Triple<Double,Double,Double> triple = side_side_angle(c,b,B,true);
		double C = triple.first;
		double A = triple.second;
		double a = triple.third;
		boolean one_valid = (C != 0.0) || (A != 0.0) || (a != 0.0); //gauss_check(a,b,c,A,B,C);
		//f.pln("1: a="+Units.str("deg",a,10)+" A="+Units.str("deg",A,10)+" b="+Units.str("deg",b,10)+" B="+Units.str("deg",B,10)+" c="+Units.str("deg",c,10)+" C="+Units.str("deg",C,10)+" s.lon="+Units.str("deg",s.lon(),10));	
		double lat2;
		if (s.lat() > 0) {
			lat2 = Util.to_pi2_cont(Math.PI/2 - a);
		} else {
			lat2 = Util.to_pi2_cont(a - Math.PI/2);			
		}
		double lon2 = Util.to_pi(s.lon() - C);
		LatLonAlt p1 = LatLonAlt.mk(lat2,lon2,s.alt()+v.vs()*t);
		
		// Solution #2
		triple = side_side_angle(c,b,B,false);
		C = triple.first;
		A = triple.second;
		a = triple.third;
		boolean two_valid = (C != 0.0) || (A != 0.0) || (a != 0.0); //gauss_check(a,b,c,A,B,C);
		//f.pln("2: a="+Units.str("deg",a,10)+" A="+Units.str("deg",A,10)+" b="+Units.str("deg",b,10)+" B="+Units.str("deg",B,10)+" c="+Units.str("deg",c,10)+" C="+Units.str("deg",C,10)+" s.lon="+Units.str("deg",s.lon(),10));
		if (s.lat() > 0) {
			lat2 = Util.to_pi2_cont(Math.PI/2 - a);
		} else {
			lat2 = Util.to_pi2_cont(a - Math.PI/2);
		}
		lon2 = Util.to_pi(s.lon() - C);
		LatLonAlt p2 = LatLonAlt.mk(lat2,lon2,s.alt()+v.vs()*t);

		if (one_valid && two_valid) {
			if (firstSolution) {
				return p1;
			} else {
				return p2;
			}
		} else if (one_valid) {
			return p1;
		} else if (two_valid) {
			return p2;
		}
		return null; // no valid solution
	}

	/**
	 * Find a point from the given lat/lon when traveling along the great circle
	 * with the given initial velocity for the given amount of time.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 */
	public static LatLonAlt linear_initial(LatLonAlt s, Velocity v, double t) {
		return linear_initial_impl(s, v.trk(), angle_from_distance(v.gs() * t),
				v.z * t);
	}

	/**
	 * Find a point from the given lat/lon with an initial angle of 'track' at a distance
	 * of 'dist'. This calculation follows the great circle.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.<p>
	 * 
	 * @param s     a position
	 * @param track the second point to define the great circle
	 * @param dist  distance from point #1 [m]
	 * @return a new position that is distance d from point #1
	 * 
	 * Note: this method does not compute an accurate altitude
	 * 
	 */
	public static LatLonAlt linear_initial(LatLonAlt s, double track, double dist) {
		return linear_initial_impl(s, track, angle_from_distance(dist), 0.0);
	}

	private static LatLonAlt linear_initial_impl(LatLonAlt s, double track,	double d, double vertical) {
		double cosd = Math.cos(d);
		double sind = Math.sin(d);
		double sinslat = Math.sin(s.lat());
		double cosslat = Math.cos(s.lat());
		double lat = Util.asin_safe(sinslat * cosd + cosslat * sind * Math.cos(track));
		double dlon = Util.atan2_safe(Math.sin(track) * sind * cosslat, cosd - sinslat * Math.sin(lat));
		// slightly different from aviation formulary because we use
		// "east positive" convention
		double lon = Util.to_pi(s.lon() + dlon);
		return LatLonAlt.mk(lat, lon, s.alt() + vertical);
	}

	private static LatLonAlt linear_rhumb_impl(LatLonAlt s, double track,
			double d, double vertical) {
		// -- Based on the calculation in the "Rhumb line" section of the
		// Aviation Formulary v1.44
		// -- Weird things happen to rhumb lines that go to the poles, therefore
		// force any polar latitudes to be "near" the pole

		final double eps = 1e-15;
		double s_lat = Math.max(Math.min(s.lat(), pi / 2 - eps), -pi / 2 + eps);
		double lat = s_lat + d * Math.cos(track);
		lat = Math.max(Math.min(lat, pi / 2 - eps), -pi / 2 + eps);

		double q;
		if (Constants.almost_equals_radian(lat, s_lat)) {
			// (Math.abs(lat - lat1) < EPS) {
			q = Math.cos(s_lat);
		} else {
			double dphi = Math.log(Math.tan(lat / 2 + pi / 4)
					/ Math.tan(s_lat / 2 + pi / 4));
			q = (lat - s_lat) / dphi;
		}
		double dlon = -d * Math.sin(track) / q;

		// slightly different from aviation formulary because I use
		// "east positive" convention
		double lon = Util.to_pi(s.lon() - dlon);
		return LatLonAlt.mk(lat, lon, s.alt() + vertical);
	}

	/**
	 * This function forms a great circle from p1 to p2, then computes 
	 * the shortest distance of another point (offCircle) to the great circle.  This is the 
	 * cross track distance. A positive 
	 * value means offCircle is to the right of the path from p1 to p2.  A 
	 * negative value means offCircle is to the left of the path from p1 to p2.<p>
	 *  
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param offCircle the point to measure the cross track distance
	 * @return the cross track distance [m]
	 */
	public static double cross_track_distance(LatLonAlt p1, LatLonAlt p2, LatLonAlt offCircle) {
		double dist_p1oc = angular_distance(p1,offCircle);
		double trk_p1oc = initial_course_impl(p1,offCircle,dist_p1oc);
		double trk_p1p2 = initial_course(p1,p2);
		// This is a direct application of the "spherical law of sines"
		return distance_from_angle(Util.asin_safe(Math.sin(dist_p1oc)*Math.sin(trk_p1oc-trk_p1p2)), (p1.alt()+p2.alt()+offCircle.alt())/3.0);
	}


	/**
	 * Determines if the three points are on the same great circle.
	 * @param p1 One point
	 * @param p2 Second point
	 * @param p3 Third point
	 * @return true, if the three points are collinear
	 */
	public static boolean collinear(LatLonAlt p1, LatLonAlt p2, LatLonAlt p3) {
		double epsilon = 1E-7;
		return Util.within_epsilon(cross_track_distance(p1,p2,p3),epsilon);
	}

	
	/**
	 * This returns the point on the great circle running through p1 and p2 that is closest to point x.
	 * The altitude of the output is the same as x.<p>
	 * If p1 and p2 are the same point, then every great circle runs through them, thus x is on one of these great circles.  In this case, x will be returned.  
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param x point to determine closest segment point to.
	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
	 */
	public static LatLonAlt closest_point_circle(LatLonAlt p1, LatLonAlt p2, LatLonAlt x) {
		// almost same point or antipode:
		if ((Util.almost_equals(p1.lat(),p2.lat()) && Util.almost_equals(p1.lon(),p2.lon())) || 
			(Util.almost_equals(p1.lat(),-p2.lat()) && Util.almost_equals(p1.lon(),Util.to_pi(p2.lon()+Math.PI)))) return x; 
		Vect3 a = spherical2xyz(p1.lat(), p1.lon());
		Vect3 b = spherical2xyz(p2.lat(), p2.lon());
		Vect3 c = a.cross(b);
		Vect3 p = spherical2xyz(x.lat(), x.lon());
		Vect3 g = p.Sub(c.Scal(p.dot(c)/c.sqv()));
		double v = spherical_earth_radius/g.norm();
		return xyz2spherical(g.Scal(v)).mkAlt(x.alt()); // return to x's altitude
	}
	
	/**
	 * This returns the point on the great circle segment running through p1 and p2 that is closest to point x.
	 * This will return either p1 or p2 if the actual closest point is outside the segment.
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param x point to determine closest segment point to.
	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
	 */
	public static LatLonAlt closest_point_segment(LatLonAlt p1, LatLonAlt p2, LatLonAlt x) {
		LatLonAlt c = closest_point_circle(p1,p2,x);
		double d12 = distance(p1,p2);
		double d1c = distance(p1,c);
		double d2c = distance(p2,c);
		if (d1c < d12 && d2c < d12) {
			return c;
		}
		if (d1c < d2c) {
			return p1;
		} else {
			return p2;
		}
	}
	
//	//TODO Fix for obtuse angle cases
//	/**
//	 * @deprecated
//	 * WARNING!!!! THIS FUNCTION MAY HAVE ERRORS IN THE GENERAL CASE!  It may not work if the triangle created contains obtuse angles.
//	 * 
//	 * This returns the point on the great circle that running through p1 and p2 that is closest to point x.
//	 * This uses Napier's rules for right spherical triangles for the non-collinear case. 
//	 * The altitude of the output is the same as x.<p>
//	 * If p1 and p2 are the same point, then every great circle runs through them, thus x is on one of these great circles.  In this case, x will be returned.  
//	 * This assumes any 2 points will be within 90 degrees of each other (angular distance).
//	 * @param p1 the starting point of the great circle
//	 * @param p2 another point on the great circle
//	 * @param x point to determine closest segment point to.
//	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
//	 */
//	public static LatLonAlt closest_point_circle_old(LatLonAlt p1, LatLonAlt p2, LatLonAlt x) {
////		f.pln("GreatCircle.closest_point_circle p1="+p1+" p2="+p2+" x="+x);
////		f.pln("GreatCircle.closest_point_circle dist a="+distance(x,p2)+" b="+distance(p1,p2)+" c="+distance(p1,x));
////		f.pln("GreatCircle.closest_point_circle trk A="+Util.turnDelta(initial_course(p1,p2), initial_course(p1,x))+" trk B="+Util.turnDelta(initial_course(x,p1), initial_course(x,p2))+" trk C="+Util.turnDelta(initial_course(p2,x), initial_course(p2,p1)));
//		double a = angular_distance(x,p2);
//		double b = angular_distance(p1,p2);
//		double c = angular_distance(p1,x);
//		double A = angle_between(p2,p1,x);
//		double B = angle_between(p1,x,p2);
//		double C = angle_between(x,p2,p1);
//		return closest_point_circle(p1,p2,x,a,b,c,A,B,C);
//	}
	

//	// angular distances a,b,c, angles A,B,C
//	private static LatLonAlt closest_point_circle(LatLonAlt p1, LatLonAlt p2, LatLonAlt x, double a, double b, double c, double A, double B, double C) {
//		//       x (B)
//		//      / \
//		// (A) p1--p2 (C)
////		double a = angular_distance(x,p2);
////		double b = angular_distance(p1,p2);
////		double c = angular_distance(p1,x);
////		double A = angle_between(p2,p1,x);
////		double B = angle_between(p1,x,p2);
////		double C = angle_between(x,p2,p1);
//		
////		if (Util.almost_equals(A, 0.0) || Util.almost_equals(C, 0.0) || Util.almost_equals(A, Math.PI) || Util.almost_equals(C, Math.PI)) {			// collinear
//		// collinear check
//		if (Util.within_epsilon(A, 0.000001) || Util.within_epsilon(C, 0.000001) || Util.within_epsilon(Math.PI-A, 0.000001) || Util.within_epsilon(Math.PI-C, 0.000001) || collinear(p1,p2,x)) {
//			return x;
//		} 
//		if (Util.almost_equals(b,0.0)) {
//			return x;   // if p1==p2, every great circle runs through them, thus x is on one of these great circles  
//		}
//		// invalid triangles
//		if (A+B+C < Math.PI || A+B+C >= Math.PI*3) {
//			// if the triangle is relatively small, it is probably collinear
//			if (a < Math.PI/2 && b < Math.PI/2 && c < Math.PI/2) { 
//				return x;
//			}
//			//f.pln(" $$ GreatCircle.closestPoint ERROR: not a triangle p1="+p1+"p2="+p2+"x="+x+" A+B+C="+(A+B+C)+" = "+Units.to("deg", A+B+C)+" deg");
//			return LatLonAlt.INVALID;
//		}
//		if (p1.almostEquals(x) || Util.almost_equals(A, Math.PI/2)) {
//			return p1.mkAlt(x.alt());
//		}
//		if (p2.almostEquals(x) || Util.almost_equals(C, Math.PI/2)) {
//			return p2.mkAlt(x.alt());
//		}
//
//		// general case p1 @ A, x @ B, p2 @ C
////		double d1 = 0;
////f.pln("GreatCircle.closest_point_circle DEG A="+Units.to("deg", A)+" B="+Units.to("deg", B)+" C="+Units.to("deg", C)+" / a="+Units.to("deg", a)+" b="+Units.to("deg", b)+" c="+Units.to("deg", c));		
////f.pln("GreatCircle.closest_point_circle RAD A="+A+" B="+B+" C="+C+" / a="+a+" b="+b+" c="+c);		
////f.pln("GreatCircle.closest_point_circle A/a="+(Math.sin(A)/Math.sin(a))+" B/b="+(Math.sin(B)/Math.sin(b))+" C/c="+(Math.sin(C)/Math.sin(c)));
//		if (A <= Math.PI/2 && C <= Math.PI/2) {
//			//   B       C1
//			//  / \     / |
//			// A---C   B1-A1
//			if (A < C) {
//				double a1 = c;
//				double A1 = Math.PI/2;
//				double B1 = A;
//				Triple<Double, Double, Double> b1C1c1 = side_angle_angle(a1,A1,B1, true);
//				double c1 = b1C1c1.third;
//				double ff = (c1/b);
////f.pln("GreatCircle.closest_point_circle a1) ff="+ff);				
//				return interpolate(p1,p2,ff);
//			} else {
//				//   B     C1
//				//  / \    | \
//				// A---C   A1-B1
//				double a1 = a;
//				double A1 = Math.PI/2;
//				double B1 = C;
//				Triple<Double, Double, Double> b1C1c1 = side_angle_angle(a1,A1,B1, true);
//				double c1 = b1C1c1.third;
//				double ff = (c1/b);
////f.pln("GreatCircle.closest_point_circle a2) ff="+ff);				
//				return interpolate(p2,p1,ff);
//			}
//			
////			d1 = side_angle_angle(a,Math.PI/2,C,true).third;
////f.pln("GreatCircle.closest_point_circle #1 d1="+d1+" "+Units.to("deg", d1)+Units.degreeStr);			
////			double ff = 1-(d1/b);
////f.pln("GreatCircle.closest_point_circle p1="+p1+" p2="+p2+" ff="+ff);			
////			return interpolate(p1, p2, ff);
//		} else if (A <= Math.PI/2 && C > Math.PI/2) {
//			//    -- B    C1
//			//  /   /    / |
//			// A---C    B1-A1
//			double a1 = a;
//			double A1 = Math.PI/2;
//			double B1 = Math.PI-C;
//			Triple<Double, Double, Double> b1C1c1 = side_angle_angle(a1,A1,B1, true);
//			double c1 = b1C1c1.third;
//			double ff = 1+(c1/b);
////f.pln("GreatCircle.closest_point_circle b) ff="+ff);				
//			return interpolate(p1,p2,ff);
//			
////			d1 = side_angle_angle(a,Math.PI/2,Math.PI-C,true).third;
////f.pln("GreatCircle.closest_point_circle #2 d="+d1+" "+Units.to("deg", d1)+Units.degreeStr);			
////			return linear_initial(p1,initial_course(p1,p2),distance_from_angle(b+d1,0)).mkAlt(x.alt());
//		} else if (A > Math.PI/2 && C <= Math.PI/2) {
//			// B--		  C1
//			//  \   \	  | \
//			//   A---C	  A1-B1
//			double a1 = c;
//			double A1 = Math.PI/2;
//			double B1 = Math.PI-A;
//			Triple<Double, Double, Double> b1C1c1 = side_angle_angle(a1,A1,B1, true);
//			double c1 = b1C1c1.third;
//			double ff = 1+(c1/b);
////f.pln("GreatCircle.closest_point_circle c) ff="+ff);				
//			return interpolate(p2,p1,ff);
////			d1 = side_angle_angle(a,Math.PI/2,Math.PI-A,true).third;
////f.pln("GreatCircle.closest_point_circle #3 d="+d1+" "+Units.to("deg", d1)+Units.degreeStr);			
////			return linear_initial(p2,initial_course(p2,p1),distance_from_angle(b+d1,0)).mkAlt(x.alt());
//		}
//f.pln("GreatCircle.closest_point_circle INVALID: weird triangle");			
//		return LatLonAlt.INVALID; // weird triangle
//	}

//	/**
//	 * @deprecated
//	 * This returns the point on the great circle segment running through p1 and p2 that is closest to point x.
//	 * This will return either p1 or p2 if the actual closest point is outside the segment.
//	 * This uses Napier's rules for right spherical triangles for the non-collinear case.
//	 * This assumes any 2 points will be within 90 degrees of each other (angular distance).
//	 * @param p1 the starting point of the great circle
//	 * @param p2 another point on the great circle
//	 * @param x point to determine closest segment point to.
//	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
//	 */
//	public static LatLonAlt closest_point_segment_old(LatLonAlt p1, LatLonAlt p2, LatLonAlt x) {
//		double a = angular_distance(x,p2);
//		double b = angular_distance(p1,p2);
//		double c = angular_distance(p1,x);
//		double A = angle_between(p2,p1,x);
//		double B = angle_between(p1,x,p2);
//		double C = angle_between(x,p2,p1);
//
//		// collinear
//		if (Util.within_epsilon(A, 0.000001) || Util.within_epsilon(C, 0.000001) || Util.within_epsilon(Math.PI-A, 0.000001) || Util.within_epsilon(Math.PI-C, 0.000001)) {
////		if (Util.almost_equals(A, 0.0) || Util.almost_equals(C, 0.0) || Util.almost_equals(A, Math.PI) || Util.almost_equals(C, Math.PI)) {
//			if (b >= a && b >= c) {
//				return x;
//			} else if (a >= b && a >= c) {
//				return p1;
//			} else {
//				return p2;
//			}
//		}
//		
//		if (A <= Math.PI/2 && C <= Math.PI/2) {
//			//   B 
//			//  / \
//			// A---C
//			return closest_point_circle(p1,p2,x,a,b,c,A,B,C);
//		} else if (A <= Math.PI/2 && C > Math.PI/2) {
//			//    -- B 
//			//  /   / 
//			// A---C
//			return p2;
//		} else {
//			// B-- 
//			//  \   \
//			//   A---C
//			return p1;
//		}
//		
//	}

	/**
	 * Given two great circles defined by a1,a2 and b1,b2, return the intersection point that is closest a1.  Use LatLonAlt.antipode() to get the other value.
	 * This assumes that the arc distance between a1,a2 &lt; 90 and b1,b2 &lt; 90
	 * The altitude of the return value is equal to a1.alt()
	 * This returns an INVALID value if both segments are collinear
	 * 
	 * @param a1 point #1 to form great circle #1
	 * @param a2 point #2 to form great circle #1
	 * @param b1 point #1 to form great circle #2
	 * @param b2 point #2 to form great circle #2
	 * @return the point that intersects the two great circles
	 */
	public static LatLonAlt intersection(LatLonAlt a1, LatLonAlt a2, LatLonAlt b1, LatLonAlt b2) {
		// define 2 planes based on the GCs
		Vect3 va = spherical2xyz(a1.lat(), a1.lon()).cross(spherical2xyz(a2.lat(), a2.lon()));
		Vect3 vb = spherical2xyz(b1.lat(), b1.lon()).cross(spherical2xyz(b2.lat(), b2.lon()));
		double r = GreatCircle.spherical_earth_radius;
		Vect3 vavb = va.cross(vb);
		if (vavb.almostEquals(Vect3.ZERO)) {
			return LatLonAlt.INVALID;
		}
		// find the line of the intersection
		Vect3 v1 = vavb.Scal(r / vavb.norm());
		Vect3 v2 = vavb.Scal(-r / vavb.norm());
		LatLonAlt x1 = xyz2spherical(v1).mkAlt(a1.alt());
		LatLonAlt x2 = xyz2spherical(v2).mkAlt(a1.alt());
		// return the closest point to a1
		if (distance(a1,x1) < distance(a1,x2)) {
			return x1;
		} else {
			return x2;
		}
	}


	/**
	 * Given two great circles defined by so, so2 and si, si2 return the intersection point that is closest to so.
	 * (Note. because on a sphere there are two intersection points)
	 * Calculate altitude of intersection using the average of the altitudes of the two closests points to the
	 * intersection.
	 * 
	 * @param so     first point of line o 
	 * @param so2    second point of line o
	 * @param dto    the delta time between point so and point so2.
	 * @param si     first point of line i
	 * @param si2    second point of line i 
	 * @return a pair: intersection point and the delta time from point "so" to the intersection, can be negative if intersect
	 *                 point is in the past. If intersection point is invalid then the returned delta time is -1
	 */
	public static Pair<LatLonAlt,Double> intersectionAvgAlt(LatLonAlt so, LatLonAlt so2, double dto, LatLonAlt si, LatLonAlt si2) {
		LatLonAlt interSec = GreatCircle.intersection(so, so2, si, si2);
		//f.pln(" %%% GreatCircle.intersection: lgc = "+lgc.toString(15));       
		if (interSec.isInvalid()) return new Pair<LatLonAlt,Double>(interSec,-1.0);
		double gso = distance(so,so2)/dto;
		double intTm = distance(so,interSec)/gso;  // relative to so 
		//f.pln(" ## gso = "+Units.str("kn", gso)+" distance(so,lgc) = "+Units.str("NM",distance(so,lgc)));
		boolean behind = GreatCircle.behind(interSec, so, GreatCircle.velocity_average(so, so2, 1.0)); //TODO: initial?
//		f.pln("behind="+behind+" interSec="+interSec+" so="+so+" vo="+GreatCircle.velocity_average(so, so2, 1.0));
		if (behind) intTm = -intTm;			
		// compute a better altitude using average of near points        
		double do1 = distance(so,interSec);
		double do2 = distance(so2,interSec);
		double alt_o = so.alt();
		if (do2 < do1) alt_o = so2.alt();
		double di1 = distance(si,interSec);
		double di2 = distance(si2,interSec);
		double alt_i = si.alt();
		if (di2 < di1) alt_i = si2.alt();
		double nAlt = (alt_o + alt_i)/2.0;       
		//    	f.pln(" $$ LatLonAlt.intersection: so.alt() = "+Units.str("ft",so.alt())+" so2.alt() = "+Units.str("ft",so2.alt())+
		//    			" si.alt() = "+Units.str("ft",si.alt())+" si2.alt() = "+Units.str("ft",si2.alt())+
		//    			" nAlt() = "+Units.str("ft",nAlt));
		//f.pln(" $$ LatLonAlt.intersection: intTm = "+intTm+" vs = "+Units.str("fpm",vs)+" nAlt = "+Units.str("ft",nAlt)+" "+behind);			 
		LatLonAlt pgc = LatLonAlt.mk(interSec.lat(),interSec.lon(),nAlt);
		return new Pair<LatLonAlt,Double>(pgc,intTm);
	}

	/**
	 * Given two great circles defined by so, so2 and si, si2 return the intersection point that is closest to so.
	 * (Note. because on a sphere there are two intersection points)
	 *  Calculate altitude of intersection using linear extrapolation from line (so,so2)
	 * 
	 * @param so     first point of line o 
	 * @param so2    second point of line o
	 * @param dto    the delta time between point so and point so2.
	 * @param si     first point of line i
	 * @param si2    second point of line i 
	 * @return a pair: intersection point and the delta time from point "so" to the intersection, can be negative if intersect
	 *                 point is in the past. If intersection point is invalid then the returned delta time is -1
	 */
	public static Pair<LatLonAlt,Double> intersectionExtrapAlt(LatLonAlt so, LatLonAlt so2, double dto, LatLonAlt si, LatLonAlt si2) {
		LatLonAlt lgc = GreatCircle.intersection(so, so2, si, si2);
		if (lgc.isInvalid()) return new Pair<LatLonAlt,Double>(lgc,-1.0);
		double gso = distance(so,so2)/dto;
		double intTm = distance(so,lgc)/gso;  // relative to so
		boolean behind = GreatCircle.behind(lgc, so, GreatCircle.velocity_average(so, so2, 1.0));
		if (behind) intTm = -intTm;
		// compute a better altitude
		double vs = (so2.alt() - so.alt())/dto;
		double nAlt = so.alt() + vs*(intTm);
		LatLonAlt pgc = LatLonAlt.mk(lgc.lat(),lgc.lon(),nAlt);
		return new Pair<LatLonAlt,Double>(pgc,intTm);
	}
		
	/** Given two great circles defined by so, vo and si, vi return the intersection point that is closest to so.
	 *  
	 * @param so           first point of line o 
	 * @param vo           velocity from point so
	 * @param si           first point of line i
	 * @param vi           velocity from point si
	 * @param checkBehind  if true, returns a negative time if no intersection
	 * @return Position and time of intersection. The returned altitude is so.alt().
	 */
	public static Pair<LatLonAlt,Double> intersection(LatLonAlt so, Velocity vo, LatLonAlt si, Velocity vi, boolean checkBehind) {
		LatLonAlt so2 = linear_initial(so, vo, 1000);
		LatLonAlt si2 = linear_initial(si, vi, 1000);
		LatLonAlt i = intersection(so, so2, si, si2);
		//f.pln(" %%% GreatCircle.intersection: i = "+i.toString(15));
		if (checkBehind) {
			//f.pln(" %%% GreatCircle.intersection: "+i.isInvalid() +" "+ behind(i, so,vo) +" "+ behind(i, si,vi));
			if (i.isInvalid() || behind(i, so,vo) || behind(i, si,vi)) return new Pair<LatLonAlt,Double>(LatLonAlt.INVALID,-1.0); // collinear or (nearly) same position or cross in the past
		}
		double dt = distance(so,i)/vo.gs();
		//f.pln(" ## GreatCircle.intersection: dt = "+dt+" vo.gs() = "+Units.str("kn",vo.gs())+" distance(so,i) = "+Units.str("nm",distance(so,i)));
		if (behind(i, so,vo)) dt = -dt;   // ??? RWB ???
		return new Pair<LatLonAlt,Double>(i,dt);
	}


	/**
	 * Given two great circles defined by so, so2 and si, si2 return the acute angle between them at the 
	 * intersection point.  This is the same as the dihedral angle, or angle between the two GC planes. 
	 * Note this may not be the same angle as the one projected into the Euclidean (unless the projection point is the intersection point), 
	 * and will generally not be the same as the (non-projected) track angle difference between them.
	 */
	public static double angle_between(LatLonAlt a1, LatLonAlt a2, LatLonAlt b1, LatLonAlt b2) {
		Vect3 va = spherical2xyz(a1.lat(), a1.lon()).cross(spherical2xyz(a2.lat(), a2.lon())).Hat(); // normal 1
		Vect3 vb = spherical2xyz(b1.lat(), b1.lon()).cross(spherical2xyz(b2.lat(), b2.lon())).Hat(); // normal 2
		double cosx = va.dot(vb);
		return Math.acos(cosx);
	}

	/**
	 * Return angle between great circles
	 * @param a point on gc1
	 * @param b intersection of gc1 and gc2
	 * @param c point on gc2
	 * @return angle between two great circles
	 */
	public static double angle_between(LatLonAlt a, LatLonAlt b, LatLonAlt c) {
		// Based on the spherical law of cosines
		double a1 = angular_distance(c,b);
		double b1 = angular_distance(a,c);
		double c1 = angular_distance(b,a);
		//f.pln("$$$ "+a1+" "+b1+" "+c1+" "+a+" "+b+" "+c);
		double d = Math.sin(c1)*Math.sin(a1);
		if (d == 0.0) {
			return Math.PI;
		}
		//f.pln(" values ="+(Math.cos(b1)-Math.cos(c1)*Math.cos(a1))+"  "+d);
		return Util.acos_safe( (Math.cos(b1)-Math.cos(c1)*Math.cos(a1)) / d );
	}


	/**
	 * Return true if x is "behind" ll, considering its current direction of travel, v.
	 * "Behind" here refers to the hemisphere aft of ll.
	 * That is, x is within the region behind the perpendicular line to v through ll.
	 * @param ll aircraft position
	 * @param v aircraft velocity
	 * @param x intruder positino
	 * @return
	 */
	public static boolean behind(LatLonAlt x, LatLonAlt ll, Velocity v) {
		Velocity v2 = velocity_initial(ll, x, 100);
		return Util.turnDelta(v.trk(), v2.trk()) > Math.PI/2.0;
	}

	/**
	 * Returns values describing if the ownship state will pass in front of or behind the intruder (from a horizontal perspective)
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si intruder position
	 * @param vi intruder velocity
	 * @return 1 if ownship will pass in front (or collide, from a horizontal sense), -1 if ownship will pass behind, 0 if collinear or parallel or closest intersection is behind you
	 */
	public static int passingDirection(LatLonAlt so, Velocity vo, LatLonAlt si, Velocity vi) {
		Pair<LatLonAlt,Double> p = intersection(so,vo,si,vi,true);
		if (p.second < 0) return 0;
		LatLonAlt si3 = linear_initial(si,vi,p.second); // intruder position at time of intersection
		if (behind(p.first, si3, vi)) return -1;
		return 1;
	}



	public static int dirForBehind(LatLonAlt so, Velocity vo, LatLonAlt si, Velocity vi) {
		LatLonAlt so2 = linear_initial(so, vo, 1000);
		LatLonAlt si2 = linear_initial(si, vi, 1000);
		LatLonAlt i = intersection(so, so2, si, si2);
		if (i.isInvalid() || behind(i,so,vo) || behind(i,si,vi)) return 0; // collinear or (nearly) same position or cross in the past
		int onRight = Util.sign(cross_track_distance(si,si2,so));
		return -onRight;
	}

	/**
	 * Compute the initial velocity on the great circle from lat/lon #1 to
	 * lat/lon #2 with the given amount of time. If points #1 and #2 are
	 * essentially the same (about 1 meter apart), then a zero vector is
	 * returned. Also if the absolute value of time is less than 1 [ms], then a
	 * zero vector is returned.
	 * <p>
	 * 
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.
	 * <p>
	 * 
	 * This calculation ignores altitude when calculating great circle distance.
	 * Small errors (typically less than 0.5%) will be introduced at typical
	 * aircraft altitudes.
	 */
	public static Velocity velocity_initial(LatLonAlt p1, LatLonAlt p2, double t) {
		// p1 is the source position, p2 is another point to form a great circle
		// positive time is moving from p1 toward p2
		// negative time is moving from p1 away from p2
		if (Math.abs(t) < minDt
				|| Util.almost_equals(Math.abs(t) + minDt, minDt,
						Util.PRECISION7)) {
			// time is negative or very small (less than 1 ms)
			//f.pln("GC case 1");
			return Velocity.ZERO;
		}
		double d = angular_distance(p1, p2);
		if (Constants.almost_equals_radian(d)) {
			if (Constants.almost_equals_alt(p1.alt(), p2.alt())) {
				// If the two points are about 1 meter apart, then count them as
				// the same.
				//f.pln("GC case 2");
				return Velocity.ZERO;
			} else {
				//f.pln("GC case 3");
				return Velocity.ZERO.mkVs((p2.alt() - p1.alt()) / t);
			}
		}
		double gs = distance_from_angle(d, 0.0) / t;
		double crs = initial_course_impl(p1, p2, d);
		Velocity v = Velocity.mkTrkGsVs(crs, gs, (p2.alt() - p1.alt()) / t);
		//f.pln(" $$ velocity_initial: crs = "+crs+" v = "+v);
		return v;
	}

	/**
	 * Estimate the velocity on the great circle from lat/lon #1 to lat/lon #2
	 * with the given amount of time. Essentially, the velocity at the mid point
	 * between lat/lon #1 and lat/lon #2. If points #1 and #2 are essentially
	 * the same (about 1 meter apart), then a zero vector is returned. Also if
	 * the absolute value of time is less than 1 [ms], then a zero vector is
	 * returned.
	 * <p>
	 * 
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.<p>
	 * 
	 * This is an estimate of the velocity. This calculation ignores altitude
	 * when calculating great circle distance. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 */
	public static Velocity velocity_average(LatLonAlt p1, LatLonAlt p2, double t) {
		// p1 is the source position, p2 is another point on that circle
		// positive time is moving from p1 toward p2
		// negative time is moving from p1 away from p2
		if (t >= 0.0) {
			return velocity_initial(interpolate(p1, p2, 0.5), p2, t / 2.0);
		} else {
			return velocity_average(p1, interpolate(p1, p2, -1.0), -t);
		}
	}

	/**
	 * Estimate the velocity on the great circle from lat/lon #1 to lat/lon #2
	 * with the given speed. 
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.<p>
	 * 
	 * This is an estimate of the velocity. This calculation ignores altitude
	 * when calculating great circle distance. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 */
	public static Velocity velocity_average_speed(LatLonAlt s1, LatLonAlt s2, double speed) {
		double dist = GreatCircle.distance(s1, s2);
		double dt = dist/speed;
		return GreatCircle.velocity_average(s1, s2, dt);
	}




	/**
	 * Estimate the final velocity on the great circle from lat/lon #1 to
	 * lat/lon #2 with the given amount of time. The track angle of the velocity
	 * is the course from point #1 to #2 roughly at point #2. If points #1 and
	 * #2 are essentially the same (about 1 meter apart), then a zero vector is
	 * returned. Also if the absolute value of time is less than 1 [ms], then a
	 * zero vector is returned.
	 * <p>
	 *  
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.
	 * <p>
	 * 
	 * This is an estimate of the velocity. This calculation ignores altitude
	 * when calculating great circle distance. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 */
	public static Velocity velocity_final(LatLonAlt p1, LatLonAlt p2, double t) {
		// p1 is the source position, p2 is another point on that circle
		// positive time is moving from p1 toward p2
		// negative time is moving from p1 away from p2 (final velocity is the
		// velocity at that time)
		if (t >= 0.0) {
			return velocity_initial(p2, p1, -t);
		} else {
			//f.pln("WARNING: GreatCircle.velocity_final neg time");
			//Thread.dumpStack();
			return velocity_initial(interpolate(p1, p2, -1.0), p1, t);
		}
	}


	/**
	 * Transforms a lat/lon position to a point in R3 (on a sphere)
	 * This is an Earth-Centered, Earth-Fixed translation (assuming earth-surface altitude).
	 * From Wikipedia http://en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
	 * We take a standard radius of the earth as defined in GreatCircle, and treat altitude as 0. 
	 * 
	 * The x-axis intersects the sphere of the earth at 0 latitude (the equator) and 0 longitude (Greenwich). 
	 * 
	 * 
	 * @param lat Latitude
	 * @param lon Longitude
	 * @return point in R3 on surface of the earth
	 */
	public static Vect3 spherical2xyz(double lat, double lon) {
		double r = GreatCircle.spherical_earth_radius;
		// convert latitude to 0-PI
		double theta = Math.PI/2 - lat;
		double phi = lon; //Math.PI + lon;
		double x = r*Math.sin(theta)*Math.cos(phi);
		double y = r*Math.sin(theta)*Math.sin(phi);
		double z = r*Math.cos(theta);
		return new Vect3(x,y,z);
	}

	public static Vect3 spherical2xyz(LatLonAlt lla) {
		return spherical2xyz(lla.lat(), lla.lon());
	}

	/**
	 * Transforms a R3 position on the earth surface into lat/lon coordinates
	 * This is an Earth-Centered, Earth-Fixed translation (assuming earth-surface altitude).
	 * From Wikipedia http://en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
	 * We take a standard radius of the earth as defined in GreatCircle, and treat altitude as 0. 
	 * @param v position in R3, with ECEF origin
	 * @return LatLonAlt point on surface of the earth (zero altitude)
	 */
	public static LatLonAlt xyz2spherical(Vect3 v) {
		double r = GreatCircle.spherical_earth_radius;
		double theta = Util.acos_safe(v.z/r);
		double phi = Util.atan2_safe(v.y, v.x);
		double lat = Math.PI/2 - theta;
		double lon = Util.to_pi(phi); //Math.PI + phi);
		return LatLonAlt.mk(lat, lon, 0.0);
	}


	/**
	 * Return if point p is to the right or left of the line from A to B
	 * @param a point A
	 * @param b point B
	 * @param p point P
	 * @return 1 if to the right or collinear, -1 if to the left.
	 */	
	public static int rightOfLinePoints(LatLonAlt a, LatLonAlt b, LatLonAlt p) {
		Vect3 v1 = spherical2xyz(a);
		Vect3 v2 = spherical2xyz(b);
		Vect3 v3 = spherical2xyz(p);
		return -Util.sign(v3.dot(v1.cross(v2)));
	}
	
	/**
	 * Return the straight-line chord distance (through a spherical earth) from 
	 * two points on the surface of the earth. 
	 * 
	 * @param lat1 latitude of first point
	 * @param lon1 longitude of first point
	 * @param lat2 latitude of second point
	 * @param lon2 longitude of second point
	 * @return the chord distance
	 */
	public static double chord_distance(double lat1, double lon1, double lat2, double lon2) {
		Vect3 v1 = spherical2xyz(lat1,lon1);
		Vect3 v2 = spherical2xyz(lat2,lon2);
		return v1.Sub(v2).norm();
	}

	/**
	 * Return the chord distance (through the earth) corresponding to a given surface distance (at the nominal earth radius)
	 * @param surface_dist
	 * @return
	 */
	public static double chord_distance(double surface_dist) {
		double theta = angle_from_distance(surface_dist,0.0);
		return 2.0*Math.sin(theta/2.0)*GreatCircle.spherical_earth_radius;
	}
	
	/**
	 * Return the surface distance (at the nominal earth radius) corresponding to a given chord distance (through the earth) 
	 * @param chord_distance
	 * @return
	 */
	public static double surface_distance(double chord_distance) {
		double theta = 2.0*Util.asin_safe(chord_distance*0.5 / GreatCircle.spherical_earth_radius);
		return distance_from_angle(theta,0.0);
	}
	


	/**
	 * Return the tangent point to a circle based on a given track heading.
	 * Note that if the circle overlaps a pole, not all track headings may be well-defined.
	 * This uses the spherical sine rules and Napier's analogies for the half-angle/half-side formulas.
	 * @param center center point of circle
	 * @param R radius of circle (great-circle distance)
	 * @param track track at point of tangency
	 * @param right ture if clockwise, false if counterclockwise
	 * @return tangent point on circle, or INVALID if a well-defined tangent does not exist (may happen if a pole is within the circle)
	 */
	public static LatLonAlt tangent(LatLonAlt center, double R, double track, boolean right) {
		double PI = Math.PI;
		double D = GreatCircle.distance_from_angle(PI/2 - center.lat(), 0.0);
		//  shortcut failure cases
		if (Util.almost_equals(D, 0.0)) {
			return LatLonAlt.INVALID;
		}
		double trk = Util.to_2pi(track);
		double alpha;
		if (!right) { // if counterclockwise, work with the opposite track.
			trk = Util.to_2pi(trk+PI);
		}
		if (trk >= 0 && trk < PI/2) {
			alpha = trk+PI/2;
		} else if (trk >= PI/2 && trk < 3*PI/2) {
			alpha = 3*PI/2-trk;
		} else {
			alpha = -(3*PI/2-trk);
		}

		// angle from center to tangent point
		double theta = Math.asin(Math.sin(Math.PI-alpha)*Math.sin(R/spherical_earth_radius)/Math.sin(D/spherical_earth_radius));
		// angular dist from pole to tangent point
		double dist = 2*spherical_earth_radius*Math.atan(Math.cos(0.5*(alpha+theta))*Math.tan(0.5*(D+R)/spherical_earth_radius)/Math.cos(0.5*(alpha-theta)));
		// special case when we are directly north or south
		if (Util.almost_equals(trk, PI/2)) {
			dist = D-R;
		} else if (Util.almost_equals(trk,3*PI/2)) {
			dist = D+R;
		}
		double lon;
		if (right == (track >= PI/2 && track <= 3*PI/2)) {
			lon = Util.to_pi(center.lon()+theta);
		} else {
			lon = Util.to_pi(center.lon()-theta);
		}
		return linear_gc(LatLonAlt.mk(PI/2.0, lon, 0),LatLonAlt.mk(0, lon, 0), dist);
	}


	
	/**
	 * EXPERIMENTAL
	 * Determine the point on the great circle a,b that has the given track.
	 * Direction of travel is assumed to be from a to b.
	 * This will return the point closest to b if two such points exist.
	 * 
	 * @param lla1
	 * @param lla2
	 * @param track
	 * @return tangent point on great circle, or INVALID if no such unique point exists (e.g. on a longitude line)
	 */
	public static LatLonAlt tangent(LatLonAlt lla1, LatLonAlt lla2, double track) {
		// infinite points
		if (Util.almost_equals(lla1.lon(),lla2.lon())) {
			return LatLonAlt.INVALID;
		}
		double PI = Math.PI;
		double trk = Util.to_2pi(track);
		LatLonAlt np = LatLonAlt.mk(PI/2,0,0);
//		if (Util.almost_equals(trk, PI/2) || Util.almost_equals(trk,3*PI/2)) {
//			return closest_point_circle(lla1,lla2,np);
//		}
		double A = angle_between(lla1, lla2, np); 
		double b = PI/2 - lla2.lat(); // dist from pole to b
		double B = trk;
		double a = Math.sin(b)/(Math.sin(A)*Math.sin(B));
		double abdist = angular_distance(lla1,lla2);
		// unit sphere distance from b to tangent point:
		double c = 2*Math.atan(Math.tan(0.5*(a+b)*Math.cos(0.5*(A+B))/Math.cos(0.5*(A-B))));
		return interpolate(lla2,lla1,c/abdist);
	}
	
//	// same as above, just calls the established subfunctions instead of doing it piecemeal 
//	public static LatLonAlt tangent2(LatLonAlt lla1, LatLonAlt lla2, double track) {
//		double PI = Math.PI;
//		boolean firstSoln = true;
//		double trk = Util.to_2pi(track);
//		if (trk > PI) {
//			trk = 2*PI-trk;
//			firstSoln = false;
//		}
//		LatLonAlt np = LatLonAlt.mk(PI/2,0,0);
//		double A = angle_between(lla1, lla2, np); 
//		double b = PI/2 - lla2.lat(); // dist from pole to b
//		double B = trk;
//		Triple<Double,Double,Double> tr = side_angle_angle(b,B,A,firstSoln);
//		double abdist = angular_distance(lla1,lla2);
//		double c = tr.third;
//		return interpolate(lla2,lla1,c/abdist);
//	}

	
	/**
	 * Haversine function
	 */
	public static double haversin(double x) {
		return (1.0-Math.cos(x))/2.0;
	}
	
	/**
	 * EXPERIMENTAL
	 * Given a small circle, rotate a point
	 * @param so point on circle
	 * @param center center of circle
	 * @param angle angle of rotation around center (positive is clockwise)
	 * @return another position on the circle
	 */
	public static LatLonAlt small_circle_rotation(LatLonAlt so, LatLonAlt center, double angle) {
		if (Util.almost_equals(angle, 0)) return so;
//f.pln("R1="+distance(so,center));		
		double R = angular_distance(so, center);
		Triple<Double,Double,Double>ABc = side_angle_side(R, angle, R);
		double A = ABc.first;
		double c = distance_from_angle(ABc.third, 0.0);
		double crs = initial_course(so, center);
		if (crs > Math.PI) crs = crs-2*Math.PI;
		double trk = Util.to_2pi(crs - A);
		LatLonAlt ret = linear_initial(so, trk, c);
//f.pln("angle="+(Util.to_pi(angle))+"  delta="+Math.abs(distance(so,center)-distance(ret,center)));		
		return ret;
	}

	/**
	 * Accurately calculate the linear distance of an arc on a small circle (turn) on the sphere.
	 * @param radius along-surface radius of small circle
	 * @param arcAngle angular (radian) length of the arc.  This is the angle between two great circles that intersect at the small circle's center.
	 * @return linear distance of the small circle arc
	 * Note: A 100 km radius turn over 60 degrees produces about 4.3 m error.
	 */
	public static double small_circle_arc_length(double radius, double arcAngle) {
		// use the chord of the diameter to determine the radius in the ECEF Euclidean frame
		double r2 = chord_distance(radius*2)/2;
		// because this is a circle in a Euclidean frame, use the normal calculations
		return arcAngle*r2;
	}

	/**
	 * Accurately calculate the angular distance of an arc on a small circle (turn) on the sphere.
	 * @param radius along-surface radius of small circle
	 * @param arcLength linear (m) length of the arc.  This is the along-line length of the arc.
	 * @return Angular distance of the arc around the small circle (from 0 o 2pi)
	 * Note: A 100 km radius turn over 100 km of turn produces about 0.0024 degrees of error.
	 */
	public static double small_circle_arc_angle(double radius, double arcLength) {
		// use the chord of the diameter to determine the radius in the ECEF Euclidean frame
		double r2 = chord_distance(radius*2)/2;
		if (r2 == 0.0) return 0.0;
		// because this is a circle in a Euclidean frame, use the normal calculations
		return arcLength/r2;
	}

	
}

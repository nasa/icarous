/* Constants
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * General Constants
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 */

package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.Units;
import java.util.jar.JarFile;

public class Constants {
	/**
	 * String indicating the FormalATM version
	 */
	public static final String version = "v2.6.2";
	
	public static final boolean allowDebugHalt = true; //TODO: set to false for distribution
	
	  /**
	   * Returns a String indicating the build date.  This information
	   * is assumed to be stored in the FormalATM.jar manifest.  If this is not
	   * available (for example the code was not compiled to a jar or the jar has been
	   * renamed), then the empty string is returned.
	   * 
	   * @return date string
	   */
	public static String build_date() {
		JarFile formalATMJar = null;
		try {
		  formalATMJar = new JarFile("FormalATM.jar");
		  String s = formalATMJar.getManifest().getMainAttributes().getValue("FormalATM-Build-Date");
		  formalATMJar.close();
		  if (s != null) return s;
		  return "";
		} catch (Exception e) {
			return "";
		} 
	}

	  /**
	   * Returns a String indicating the build time.  This information
	   * is assumed to be stored in the FormalATM.jar manifest.  If this is not
	   * available (for example the code was not compiled to a jar or the jar has been
	   * renamed), then the empty string is returned.
	   * 
	   * @return time string
	   */
	public static String build_time() {
		try {
		  JarFile formalATMJar = new JarFile("FormalATM.jar");
		  String s = formalATMJar.getManifest().getMainAttributes().getValue("FormalATM-Build-Time");
		  formalATMJar.close();
		  if (s != null) return s;
		  return "";
		} catch (Exception e) {
			return "";
		}
	}

	  /**
	   * Returns a String indicating the distribution type.  This information
	   * is assumed to be stored in the FormalATM.jar manifest.  If this is not
	   * available (for example the code was not compiled to a jar or the jar has been
	   * renamed), then the empty string is returned.
	   * 
	   * @return type string
	   */
	public static String distribution_type() {
		try {
		  JarFile formalATMJar = new JarFile("FormalATM.jar");
		  String s = formalATMJar.getManifest().getMainAttributes().getValue("FormalATM-Build-Type");
		  formalATMJar.close();
		  if (s != null) return s;
		  return "";
		} catch (Exception e) {
			return "";
		}
	}

	/**
	 * Default pattern string representing the separators (commas and semicolons) in input files. 
	 */
	public static final String separatorPattern = "[,;]";
	/**
	 * Default pattern string representing whitespace in input files.  This does not include parentheses or braces.
	 */
	public static final String wsPatternBase = "[,; \\t]+";
	/**
	 * Default pattern string representing whitespace in input files.  This includes parentheses.
	 */
	public static final String wsPatternParens = "[(),; \\t]+";
	/**
	 * Default pattern string representing whitespace in input files.  This includes square braces.
	 */
	public static final String wsPatternBraces = "[,; \\t\\[\\]]+";

	public static final String[] LAT_OR_LON_HEADINGS = {"lat", "lon", "long", "latitude"};
	public static final String[] NAME_HEADINGS = {"name", "aircraft", "id"};
	public static final String[] LATITUDE_HEADINGS = {"sx", "lat", "latitude"};
	public static final String[] LONGITUDE_HEADINGS = {"sy", "lon", "long", "longitude"};
	public static final String[] ALTITUDE_HEADINGS = {"sz", "alt", "altitude"};
	public static final String[] TIME_HEADINGS = {"time", "tm", "st"};


	/**  WAAS specification requirements (25 ft) for horizontal
	 *   accuracy, 95% of the time.  In practice, the actual accuracy is
	 *   much better. */
	public static final double GPS_LIMIT_HORIZONTAL = Units.from(Units.ft, 25.0);
	/**  WAAS specification requirements (25 ft) for vertical
	 *   accuracy, 95% of the time. */
	public static final double GPS_LIMIT_VERTICAL   = Units.from(Units.ft, 25.0);
	/**  Most GPS units update with a frequency of at least 1 HZ.  */
	public static final double TIME_LIMIT_EPSILON   = Units.from(Units.s, 1.0);

	
	public static double HORIZONTAL_ACCURACY = 1E-7;  // GPS_LIMIT_HORIZONTAL;
	public static double VERTICAL_ACCURACY   = 1E-7;  // GPS_LIMIT_VERTICAL;
	private static double HORIZONTAL_ACCURACY_RAD = Units.to("nmi",  HORIZONTAL_ACCURACY) * Math.PI / (180.0 * 60.0);
	public static double TIME_ACCURACY       = 1E-7;  // TIME_LIMIT_EPSILON;

	private static int OUTPUT_PRECISION = 6;
	
	public static double NO_TIME = -1;
	/**
	 * Set the time accuracy value.  This value means any two times that
	 * are within this value of each other are considered the same [s].
	 * 
	 * @param acc accuracy
	 */
	public static void set_time_accuracy(double acc) {
		if (acc > 0.0) {
			TIME_ACCURACY = acc;
		}
	}

	/**
	 * Set the horizontal accuracy value.  This value means any two positions that
	 * are within this value of each other are considered the same [m].
	 * @param acc accuracy
	 */
	public static void set_horizontal_accuracy(double acc) {
		if (acc > 0.0) {
			HORIZONTAL_ACCURACY = acc;
			HORIZONTAL_ACCURACY_RAD = Units.to("nmi", acc) * Math.PI / (180.0 * 60.0); // = GreatCircle.angle_from_distance(acc);
		}
	}

	/**
	 * Set the vertical accuracy value.  This value means any two positions that
	 * are within this value of each other are considered the same [m].
	 * @param acc accuracy
	 */
	public static void set_vertical_accuracy(double acc) {
		if ( acc > 0.0) {
			VERTICAL_ACCURACY = acc;
		}
	}

	/** Return the time accuracy value (in seconds) 
	 * @return time accuracy
	 * */
	public static double get_time_accuracy() {
		return TIME_ACCURACY;
	}

	/** Return the horizontal accuracy value (in meters) 
	 * @return horizontal accuracy
	 * */
	public static double get_horizontal_accuracy() {
		return HORIZONTAL_ACCURACY;
	}
	
	/** Return the vertical accuracy value (in meters) 
	 * @return vertical accuracy
	 * */
	public static double get_vertical_accuracy() {
		return VERTICAL_ACCURACY;
	}

	/** Return the horizontal accuracy value (in radians) 
	 * @return horizontal accuracy
	 * */
	public static double get_latlon_accuracy() {
		return HORIZONTAL_ACCURACY_RAD;
	}

	/** set default precision for output values (0-16) 
	 * @param i number of digits of precision
	 * */
	public static void set_output_precision(int i) {
		if (i >= 0 && i <= 16) {
			OUTPUT_PRECISION = i;
		}
	}
	
	/** return default precision for output values 
	 * @return number of digits of precision
	 * */
	public static int get_output_precision() {
		return OUTPUT_PRECISION;
	}

	/** Return true, if these two times are within the time accuracy of each other 
	 * 
	 * @param t1 a time
	 * @param t2 another time
	 * @return true, if almost equals
	 */
	public static boolean almost_equals_time(double t1, double t2) {
		return Util.within_epsilon(t1,t2,TIME_ACCURACY);
	}

	/** Return true, if these two positions [m] within the horizontal accuracy of each other 
	 * 
	 * @param x1 x coordinate for first value
	 * @param y1 y coordinate for first value
	 * @param x2 x coordinate for second value
	 * @param y2 y coordinate for second value
	 * @return true, if xy values are almost equals
	 */
	public static boolean almost_equals_xy(double x1, double y1, double x2, double y2) {
		return Util.within_epsilon(Util.sq(x1-x2)+Util.sq(y1-y2),Util.sq(HORIZONTAL_ACCURACY));
	}

	/** Return true, if this distance [m] is within the horizontal accuracy of zero 
	 * 
	 * @param d
	 * @return true, if almost equals
	 */
	public static boolean almost_equals_distance(double d) {
		return Util.within_epsilon(d,HORIZONTAL_ACCURACY);
	}

	/** Return true, if these two angles [rad] are within the horizontal accuracy of each other 
	 * 
	 * @param d1 a distance
	 * @param d2 another distance
	 * @return true, if almost equals
	 */
	public static boolean almost_equals_radian(double d1, double d2) {
		return Util.within_epsilon(d1, d2, HORIZONTAL_ACCURACY_RAD);
	}

	/** Return true, if this angle [rad] is within the horizontal accuracy of zero 
	 * 
	 * @param d distance
	 * @return true, if almost equals
	 */
	public static boolean almost_equals_radian(double d) {
		return Util.within_epsilon(d, HORIZONTAL_ACCURACY_RAD);
	}

	/** Return true, if these two altitudes are within the vertical accuracy of each other 
	 * 
	 * @param a1 an altitude
	 * @param a2 another altitude
	 * @return true, if almost equals
	 */
	public static boolean almost_equals_alt(double a1, double a2) {
		return Util.within_epsilon(a1, a2, VERTICAL_ACCURACY);
	}
}

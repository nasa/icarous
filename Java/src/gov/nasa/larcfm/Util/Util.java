/*
 * Util.java 
 * 
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 *
 * Utility functions.
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.List;
import java.text.SimpleDateFormat;  // Next three for strDate
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;

/** A broad collection of utilities */
public final class Util {


  /**
   * The maxUlps (see almostEqual() method) for (approximately) 13 digits of
   * decimal precision.
   * 
   * Please note this value is used for internal floating point comparisons and is not itself a floating point value.
   * It should only be used as a parameter for the various almost_* functions in this file.
   * 
   */
  public static final long PRECISION13 = 16348;

  /**
   * The maxUlps (see almostEqual() method) for (approximately) 5 digits of
   * decimal precision.
   * 
   * Please note this value is a bit pattern used for internal floating point comparisons and is not itself a floating point value.
   * It should only be used as a parameter for the various almost_* functions in this file.
   */
  public static final long PRECISION5 = 1l << 40;

  /**
   * The maxUlps (see almostEqual() method) for (approximately) 7 digits of
   * decimal precision.
   * 
   * Please note this value is a bit pattern used for internal floating point comparisons and is not itself a floating point value.
   * It should only be used as a parameter for the various almost_* functions in this file.
   */
  public static final long PRECISION7 = 1l << 34;

  /**
   * The maxUlps (see almostEqual() method) for (approximately) 9 digits of
   * decimal precision.
   * 
   * Please note this value is a bit pattern used for internal floating point comparisons and is not itself a floating point value.
   * It should only be used as a parameter for the various almost_* functions in this file.
   */
  public static final long PRECISION9 = 1l << 27;

  /** A default precision for the almostEquals method 
   * 
   * Please note this value is a bit pattern used for internal floating point comparisons and is not itself a floating point value.
   * It should only be used as a parameter for the various almost_* functions in this file.
   */
  public static final long PRECISION_DEFAULT = PRECISION13;


  // Do not allow one of these to be created
  private Util() {
  }

  /**
   * Determines if a &lt; b, without being almost equal, according to the
   * definition of the almostEquals() method..
   * 
   */
  public static boolean almost_less(double a, double b) {
    if (almost_equals(a, b)) {
      return false;
    }
    return a < b;
  }

  /**
   * Determines if a &lt; b, without being almost equal, according to the
   * definition of the almostEquals() method..
   * 
   */
  public static boolean almost_less(double a, double b, long maxUlps) {
    if (almost_equals(a, b, maxUlps)) {
      return false;
    }
    return a < b;
  }

  /**
   * Determines if a &gt; b, without being almost equal, according to the
   * definition of the almostEquals() method.
   * 
   */
  public static boolean almost_greater(double a, double b) {
    if (almost_equals(a,b)) {
      return false;
    }
    return a > b;
  }

  /**
   * Determines if a &gt; b, without being almost equal, according to the
   * definition of the almostEquals() method.
   * 
   */
  public static boolean almost_greater(double a, double b, long maxUlps) {
    if (almost_equals(a, b, maxUlps)) {
      return false;
    }
    return a > b;
  }

  /**
   * Determines if a is greater than or almost equal to b, according to the
   * definition of the almostEquals() method.
   *
   */
  public static boolean almost_geq(double a, double b, long maxUlps) {
    return a >= b || almost_equals(a, b, maxUlps);
  }

  /**
   * Determines if a is greater than or almost equal to b, according to the
   * definition of the almostEquals() method.
   *
   */
  public static boolean almost_geq(double a, double b) {
    return almost_geq(a, b, PRECISION_DEFAULT);
  }

  /**
   * Determines if a is less than or almost equal to b, according to the
   * definition of the almostEquals() method.
   *
   */
  public static boolean almost_leq(double a, double b, long maxUlps) {
    return a <= b || almost_equals(a, b, maxUlps);
  }

  /**
   * Determines if a is less than or almost equal to b, according to the
   * definition of the almostEquals() method.
   *
   */
  public static boolean almost_leq(double a, double b) {
    return almost_leq(a, b, PRECISION_DEFAULT);
  }

  /** Are these two numbers almost equal, given the PRECISION_DEFAULT */
  public static boolean almost_equals(double a, double b) {
    return almost_equals(a, b, PRECISION_DEFAULT);
  }

  /**
   * Determines if these two doubles, relative to each other, are almost
   * equal. The "nearness" metric is captured in maxUlps.
   * Mathematically, a == b is the same as a - b == 0.  Due to quirks
   * in floating point, generally almostEquals(a, b) is not the same
   * as almostEquals(a - b, 0).  The form without the subtraction is
   * preferred.  <p>
   * 
   * Consistent with the IEEE-754 floating point specification, "not a number"
   * (NaN) won't compare as equal to anything (including itself or another
   * NaN).
   * <p>
   * 
   * If two doubles are almost_equals() with a maxUlps parameter of 16348, then
   * this means there can be at most 16347 floating point
   * numbers between them. A value of 16348 for "units of least
   * precision" (ulps) corresponds to a and b agreeing to about 13
   * decimal places.  Said another way, the two numbers have an
   * absolute difference of (approximately) 1e-13 if the two floating
   * point numbers are near 1.  <p>
   * 
   * The maxUlps parameter must be positive and smaller than 2^50
   * <p>
   * 
   * The implementation is based on the discussion (but not the code) in
   * (google: comparing floats cygnus)
   */
  public static boolean almost_equals(double a, double b, long maxUlps) {
    if (a == b) { // if two numbers are equal, then the are almost_equal
      return true;
    }

    // special case of comparing to zero.
    if (a == 0.0 || b == 0.0) {
      double comp = 1.0e-13;  // should correspond to PRECISION_DEFAULT;
      if (maxUlps == PRECISION5) comp = 1.0e-5;
      if (maxUlps == PRECISION7) comp = 1.0e-7;
      if (maxUlps == PRECISION9) comp = 1.0e-9;
      if (maxUlps == PRECISION13) comp = 1.0e-13;
      if (Math.abs(a) < comp && Math.abs(b) < comp) {
        return true;
      }
    }

    //if (Double.isNaN(a) || Double.isNaN(b)) {   // this operation is slooooooow
    //    return false;
    //  }
    if (!(a < b || b < a)) { // idiom to filter out NaN's
      return false;
    }
    if (Double.isInfinite(a) || Double.isInfinite(b)) {
      return false;
    }


    long aInt = Double.doubleToLongBits(a);

    // Make aInt lexicographically ordered as a twos-complement long
    if (aInt < 0)
      aInt = 0x8000000000000000l - aInt;

    // Make bInt lexicographically ordered as a twos-complement long
    long bInt = Double.doubleToLongBits(b);

    if (bInt < 0)
      bInt = 0x8000000000000000l - bInt;

    long intDiff = Math.abs(aInt - bInt); // This is valid because IEEE-754
    // doubles are required to be
    // lexically ordered

    if (intDiff <= maxUlps) {
      return true;
    }

    return false;
  }

  /** 
   * Comparison of two values to determine if their absolute difference is within a value 
   * epsilon.  If the epsilon value is too small relative to the 
   * a and b values in question, then this is essentially the same as ==.  Epsilon must be positive.
   */
  public static boolean within_epsilon(double a, double b, double epsilon) {
	  //f.pln(" $$$$$$ within_epsilon: "+ Math.abs(a-b)+" < "+epsilon);
      return Math.abs(a-b) < epsilon;
  }

  /**
   * Returns true if the magnitude of a is less than epsilon. Epsilon must be positive.
   */
  public static boolean within_epsilon(double a, double epsilon) {
    return Math.abs(a) < epsilon;
  }

  /**
   * Discretize the value of nvoz in the direction from voz in units of discreteUnits
   * @param voz    The value nvoz was derived from
   * @param nvoz   The value to be discretized
   * @param discreteUnits     the size of discretization, e.g. 0.1, 1.0, 10.0, 100.0  etc
   * @return       nvoz discretized to units if discreteUnits
   */
  public static double  discretizeDir(double voz, double nvoz, double discreteUnits) {
    int sgn = -1;
    if (nvoz >= 0) sgn = 1;
    double rtn;
    if (sgn*nvoz >= sgn*voz) 
      rtn =  sgn*Math.ceil(Math.abs((nvoz)/discreteUnits))*discreteUnits;
    else
      rtn =  sgn*Math.floor(Math.abs((nvoz)/discreteUnits))*discreteUnits;
    //f.pln(" #### vsDiscretize: voz = "+Units.str("fpm",voz)+" nvoz = "+Units.str("fpm",nvoz)+" rtn = "+Units.str("fpm",rtn));
    return rtn;
  }


  /** Square */
  public static double sq(double x) {
    return x * x;
  }

  /** a safe (won't return NaN or throw exceptions) version of square root */
  public static double sqrt_safe(double x) {
    return Math.sqrt(Util.max(x, 0));
  }

  /** a safe (won't return NaN or throw exceptions) version of arc-tangent */
  public static double atan2_safe(double y, double x) {
    if (y == 0 && x == 0)
      return 0;
    return Math.atan2(y, x);
  }

  /** a safe (won't return NaN or throw exceptions) version of arc-sine */
  public static double asin_safe(double x) {
    return Math.asin(Util.max(-1.0,Util.min(x,1.0)));

    //	double z = Util.max(-1.0,Util.min(x,1.0));
    //	//return Math.atan2(z,Math.sqrt(1-z*z));
    //	  
    //	return z+(z*z*z)/6.0+(3.0*z*z*z*z*z)/40.0+(15.0*z*z*z*z*z*z*z)/336.0;
  }

  /** a safe (won't return NaN or throw exceptions) version of arc-cosine
   * 
   * @param x
   * @return the arc-cosine of x, between [0,pi)
   */
  public static double acos_safe(double x) {
    return Math.acos(Util.max(-1.0,Util.min(x,1.0)));
  }

  /** Discriminant of a quadratic */
  public static double discr(double a, double b, double c) {
    return sq(b) - 4 * a * c;
  }

  /** Quadratic equation, eps = -1 or +1 */
  public static double root(double a, double b, double c, int eps) {
    if (a == 0 && b == 0)
      return Double.NaN;
    else if (a == 0)
      return -c / b;
    else {
      double sqb = sq(b);
      double ac  = 4*a*c;
      if (almost_equals(sqb,ac) || sqb > ac) 
        return (-b + eps * sqrt_safe(sqb-ac)) / (2 * a);
      return Double.NaN; 
    }
  }

  /** root2b(a,b,c,eps) = root(a,2*b,c,eps) , eps = -1 or +1*/
  public static double root2b(double a, double b, double c,int eps) {
    if (a == 0 && b == 0) 
      return Double.NaN; 
    else if (a == 0) 
      return -c/(2*b);
    else {
      double sqb = sq(b);
      double ac  = a*c;
      if (almost_equals(sqb,ac) || sqb > ac) 
        return (-b + eps*sqrt_safe(sqb-ac))/a;
      return Double.NaN; 
    }     
  }

  /** 
   * Returns +1 if the argument is positive or 0, -1 otherwise.  Note:
   * This is not the classic signum function from mathematics that 
   * returns 0 when a 0 is supplied. */
  public static int sign(double x) {
	 // A true signum could be implemented in C++ as below.
	 // template <typename T> int sgn(T val) {
	 //   return (T(0) < val) - (val < T(0));
     //}
    if (x >= 0)
      return 1;
    return -1;
  }

  /** Returns +1 if the argument is true, -1 otherwise */
  public static int sign(boolean b) {
    if (b)
      return 1;
    return -1;
  }

  /** Returns +1 if the argument is positive or 0, -1 otherwise */
  public static boolean bsign(double x) {
    if (x >= 0)
      return true;
    return false;
  }
  
  public static double min(double x, double y) {
	  return Math.min(x, y);
  }

  public static float min(float x, float y) {
	  return Math.min(x, y);
  }

  public static int min(int x, int y) {
	  return Math.min(x, y);
  }

  public static long min(long x, long y) {
	  return Math.min(x, y);
  }

  public static double max(double x, double y) {
	  return Math.max(x, y);
  }

  public static float max(float x, float y) {
	  return Math.max(x, y);
  }

  public static int max(int x, int y) {
	  return Math.max(x, y);
  }

  public static long max(long x, long y) {
	  return Math.max(x, y);
  }

  
  private static final double twopi = 2 * Math.PI;

  /**
   * Converts <code>rad</code> radians to the range
   * (-<code>pi</code>, <code>pi</code>].
   * <p>Note: this should not be used for argument reduction for trigonometric functions (Math.sin(to_pi(x))
   *
   * @param rad Radians
   *
   * @return <code>rad</code> in the range
   * (-<code>pi</code>, <code>pi</code>].
   */
  public static double to_pi(double rad) {
    double r = to_2pi(rad);
    if (r > Math.PI) 
      return r-twopi;
    else {
      return r;
    }
  }

  /**
   * Computes the modulo of val and mod. The returned value is in the range [0,mod)
   */
  public static double modulo(double val, double mod) {
      double n = Math.floor(val / mod);
      double r = val - n * mod;
      return Util.almost_equals(r,mod) ? 0.0 : r;
  }

  /**
   * Converts <code>deg</code> degrees to the range 
   * [<code>0</code>, <code>360</code>).
   * 
   * @param deg Degrees
   * 
   * @return <code>deg</code> in the range [<code>0</code>, <code>360</code>).
   */
  public static double to_360(double deg) {
    return modulo(deg,360);
  }

  /**
   * Converts <code>rad</code> radians to the range
   * [<code>0</code>, <code>2*pi</code>]. 
   * <p>Note: this should not be used for argument reduction for trigonometric functions (Math.sin(to_2pi(x))
   *
   * @param rad Radians
   *
   * @return <code>rad</code> in the range
   * [<code>0</code>, <code>2*pi</code>).
   */
  public static double to_2pi(double rad) {
    return modulo(rad,twopi);
  }

  /**
   * Converts <code>rad</code> radians to the range 
   * [<code>-Math.PI/2</code>, <code>Math.PI/2</code>).
   * This function is continuous, so to_pi2_cont(PI/2+eps) equals PI/2-eps.
   * 
   * @param rad Radians
   * 
   * @return <code>rad</code> in the range [<code>-Math.PI/2</code>, <code>Math.PI/2</code>).
   */
  public static double to_pi2_cont(double rad) {
    double r = to_pi(rad);
    if (r < -Math.PI / 2) {
      return -Math.PI - r;
    } else if (r < Math.PI / 2) {
      return r;
    } else {
      return Math.PI - r;
    }
  }


  /**
   * Converts <code>deg</code> degrees to the range 
   * (<code>-180</code>, <code>180</code>].
   * 
   * @param deg Degrees
   * 
   * @return <code>deg</code> in the range (<code>-180</code>, <code>180</code>].
   */
  public static double to_180(double deg) {
    double d = to_360(deg);
    if (d > 180) {
      return d-360.0;
    } else {
      return d;
    }
  }


  /**
   * Returns true if a turn from track angle alpha to track angle beta is 
   * clockwise (by the shortest path).  If the two angles are equal, then 
   * this function returns true.
   */ 
  public static boolean clockwise(double alpha, double beta) {
    double a = Util.to_2pi(alpha);
    double b = Util.to_2pi(beta);
    if (Math.abs(a-b) <= Math.PI) {
      return b >= a;
    }
    return a > b;
  }


  /**
   * Returns 1 if the minimal turn to goalTrack (i.e. less than pi) is to the right, else -1
   * @param initTrack   initial track [rad]
   * @param goalTrack   target track [rad]
   **/
  public static int turnDir(double initTrack, double goalTrack) {
    if (Util.clockwise(initTrack,goalTrack)) return 1;
    else return -1;
  }


  /**
   * Returns the smallest angle between two track angles [0,PI].
   */ 
  public static double turnDelta(double alpha, double beta) {
    double a = Util.to_2pi(alpha);
    double b = Util.to_2pi(beta);
    double delta = Math.abs(a-b);
    if (delta <= Math.PI) {
      return delta;
    }
    return 2.0*Math.PI - delta;
  }

  public static double trackDelta(Velocity before, Velocity after) {
    return turnDelta(before.trk(), after.trk());  
  }

  public static double gsDelta(Velocity before, Velocity after) {
    return before.gs() - after.gs();  
  }

  public static double vsDelta(Velocity before, Velocity after) {
    return before.vs() - after.vs();  
  }

  /**
   * Returns the smallest angle between two track angles [-PI,PI]. The sign indicates the direction of 
   * the turn, positive is clockwise, negative counterclockwise.
   */ 
  public static double signedTurnDelta(double alpha, double beta) {
    return turnDir(alpha,beta)*turnDelta(alpha,beta);
  }


  /**
   * Returns the angle between two tracks when turning in direction indicated by turnRight flag [0,2PI]
   * Note: this function can return an angle larger than PI!
   */ 
  public static double turnDelta(double alpha, double beta, boolean turnRight) {
    if (Constants.almost_equals_radian(alpha,beta)) return 0.0;      // do not want 2 PI returned
    boolean clkWise = clockwise(alpha,beta);
    double rtn = turnDelta(alpha,beta);
    if (turnRight != clkWise)   // go the long way around
      rtn = 2.0*Math.PI - rtn;
    return rtn;
  }


  /**
   * Returns the angle between two tracks when turning in direction indicated by turnRight flag [0,2PI]
   * Note: this function can return an angle larger than PI!
   * 
   * @param dir = +/- 1 + right, - left
   */ 
  public static double turnDelta(double alpha, double beta, int dir) {
    return turnDelta(alpha, beta, dir > 0);
  }

  /**
   * Convert a track angle (zero is up, clockwise is increasing) to a math angle (zero is to the right, counter clockwise is increasing).
   * @param alpha the track angle
   * @return the math angle
   */ 
  public static double track2math(double alpha) {
    return Math.PI/2 - alpha;
  }


  /**
   * Returns a double value which is a representation of the given string.  If the string does 
   * not represent a number, false is returned.  If one wants to know
   * the fact that the string is not a number, then use java.lang.Double.parseDouble method.
   */ 
  public static boolean is_double(String s) {
    try {
      double v = Double.parseDouble(s);
      return v <= v;  // strange, but this filters out NaN's
    } catch (NumberFormatException e) {
      return false;
    }
  }

  /**
   * Returns a double value which is a representation of the given string.  If the string does 
   * not represent a number, an arbitrary value is returned. 
   * In many cases, but not all, if the string is not a number then 0.0 is returned.  However,
   * on some platforms, "1abc" will return 1.  If one wants to know
   * the fact that the string is not a number, then use Util.is_double() method.
   */ 
  public static double parse_double(String s) {
    try {
      return Double.parseDouble(s);
    } catch (NumberFormatException e) {
      return 0.0;
    }
  }


  /**
   * Returns a double value which is a representation of the given string.  If the string does 
   * not represent a number, a zero is returned.  If one wants to know
   * the fact that the string is not a number, then use java.lang.Double.parseDouble method.
   */ 
  public static boolean is_int(String s) {
    try {
      Integer.parseInt(s);
      return true;
    } catch (NumberFormatException e) {
      return false;
    }
  }

  /**
   * Returns a double value which is a representation of the given string.  If the string does 
   * not represent a number, a zero is returned.  If one wants to know
   * the fact that the string is not a number, then use Util.isDouble() method.
   */ 
  public static int parse_int(String s) {
    try {
      return Integer.parseInt(s);
    } catch (NumberFormatException e) {
      return 0;
    }
  }

  /**
   * Returns true if the stored value for key is likely a boolean
   * @param s name
   * @return true if string value is true/false/t/f, false otherwise
   */
  public static boolean is_boolean(String s) {
	  return (s.equalsIgnoreCase("true") || s.equalsIgnoreCase("T") || s.equalsIgnoreCase("false") || s.equalsIgnoreCase("F"));
  }

  /** Converts a string into a Boolean value.  This is more permissive than 
   * Boolean.parseBoolean, it accepts "true" and "T" (ignoring case).
   * @param value
   * @return
   */
  public static boolean parse_boolean(String value) {
	  return value.equalsIgnoreCase("true") || value.equalsIgnoreCase("T");
  }

  public static double[] arrayList2Array(ArrayList<Double> array) {
    if (array == null) {
      return null;
    } 
    final double[] result = new double[array.size()];
    for (int i = 0; i < result.length; i++) {
      result[i] = array.get(i);
    }
    return result;
  }


  /** Reads in a clock string and converts it to seconds.  
   * Accepts hh:mm:ss, mm:ss, and ss.
   */
  public static double parse_time(String s) {
    double tm = -1;
    String patternStr = "[:]";
    String [] fields2 = s.split(patternStr);
    if (fields2.length >= 3) {
      tm = parse_double(fields2[2]) + 60 * parse_double(fields2[1]) + 3600 * parse_double(fields2[0]); // hrs:min:sec
    } else if (fields2.length == 2) {
      tm = parse_double(fields2[1]) + 60 * parse_double(fields2[0]); // min:sec
    } else if (fields2.length == 1){
      tm = parse_double(fields2[0]); //getColumn(_sec, head[TM_CLK]);
    }
    return tm;
  }

  /**
   * Convert the decimal time (in seconds) into a 00:00:00 string.
   * @param t time in seconds
   * @return String of hours:mins:secs
   */
  public static String time_str(double t) {
    int hours = (int) t/3600;
    int rem = (int) t - hours*3600;
    int mins = rem / 60;
    int secs = rem - mins*60;
    return String.format("%02d:%02d:%02d", hours, mins, secs);
    //return hoursMinutesSeconds(t);
  }

  /**
   * Convert the decimal time (in seconds) into a 0:00:00 string.
   * @param t time in seconds
   * @return String of hours:mins:secs
   */
  public static String hoursMinutesSeconds(double t) {
    int hours = (int) t/3600;
    int rem = (int) t - hours*3600;
    int mins = rem / 60;
    int secs = rem - mins*60;
    //return String.format("%2d:%02d:%02d", hours, mins, secs);
    return String.format("%d:%02d:%02d", hours, mins, secs);
  }

  /** Return a string representing this list */
  public static String list2str(List<? extends Object> l) {
    String rtn =  "{";
    for (Object o : l) {
      rtn = rtn + o.toString() + ", ";
    }
    rtn = rtn + "}";
    return rtn;
  }

  /** Returns true if string s1 is less than or equal to string s2. */
  public static boolean less_or_equal(String s1, String s2) {
    if (s1.compareTo(s2) >= 0) return true;
    //f.pln(" Util::lessThan: s1 = "+s1+" s2 = "+s2+" rtn = "+rtn);
    return false;
  }

  public static String strDate() {
    Calendar today = new GregorianCalendar();
    SimpleDateFormat df = new SimpleDateFormat();
    df.applyPattern("dd/MM/yyyy");
    return df.format(today.getTime());

  }

//  private static int cnt = 0;
//  public static int nextCount() { // a generic system-wide counter
//    return cnt++;
//  }

  /**
   * The behavior of the x%y operator is different between Java and C++ if either x or y is negative.  Use this to always return a value between 0 and y. 
   * @param x value
   * @param y range
   * @return x mod y, having the same sign as y (Java behavior)
   */
  public static int mod(int x, int y) {
    return x % y;
  }

  /**
   * Return the closest flight level value for a given altitude
   * @param alt
   * @return Flight level in feet
   */
  public static int flightLevel(double alt) {
    //Note: foot conversion is hard-coded to avoid reference to Units class.
    return (int)Math.round(alt/0.3048/500.0)*5; 
  }

} // Util.java

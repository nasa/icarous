/*
 * Constants.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * General Constants
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "Units.h"
#include "Util.h"

namespace larcfm {


// these will all eventually be moved into the object initialization
static const std::string _FormalATM_version = "v2.6.1";
static const double _FormalATM_GPS_LIMIT_HORIZONTAL = Units::from("ft", 25.0); // in order to keep both versions tied to a single entry, this needs to be defined here.
static const double _FormalATM_GPS_LIMIT_VERTICAL = Units::from("ft", 25.0); // in order to keep both versions tied to a single entry, this needs to be defined here.
static const double _FormalATM_TIME_LIMIT_EPSILON = 1.0;// in order to keep both versions tied to a single entry, this needs to be defined here.
static const double _FormalATM_NO_TIME_LIMIT_EPSILON = -1;
static const double _FormalATM_NO_TIME = -1;

  class Constants {
  private:
	  static double HORIZONTAL_ACCURACY;
	  static double VERTICAL_ACCURACY;
	  static double HORIZONTAL_ACCURACY_RAD;
	  static double TIME_ACCURACY;
	  static int OUTPUT_PRECISION;

 	  public:

	  	static const std::string separatorPattern;

	  	/**
	  	 * Default pattern string representing whitespace in input files.  This does not include parentheses or braces.
	  	 */
	  	static const std::string wsPatternBase;

	  	/**
	  	 * Default pattern string representing whitespace in input files.  This does not include parentheses or braces.
	  	 */
	  	static const std::string wsPatternBaseNoRegex;
	  	/**
	  	 * Default pattern string representing whitespace in input files.  This includes parentheses.
	  	 */
	  	static const std::string wsPatternParens;
	  	/**
	  	 * Default pattern string representing whitespace in input files.  This includes square braces.
	  	 */
	  	static const std::string wsPatternBraces;


	  	  /**
	  	   * String indicating the version
	  	   */
	  	  static const std::string version;

	  	  /**
	  	   * String indicating the build date.  Note this only changes if
	  	   * Constants is recompiled (e.g. during a full build).
	  	   * Partial compilations may not update this value.
	  	   */
	  	  static const std::string build_date;

	  	  /**
	  	   * String indicating the build time.  Note this only changes if
	  	   * Constants is recompiled (e.g. during a full build).
	  	   * Partial compilations may not update this value.
	  	   */
	  	  static const std::string build_time;


	  	  /**  WAAS specification requirements (25 ft) for horizontal
	  	   *   accuracy, 95% of the time.  In practice, the actual accuracy is
	  	   *   much better. */
	  	  static const double GPS_LIMIT_HORIZONTAL;
	  	  /**  WAAS specification requirements (25 ft) for vertical
	  	   *   accuracy, 95% of the time. */
	  	  static const double GPS_LIMIT_VERTICAL;
	  	  /**  Most GPS units update with a frequency of at least 1 HZ.  */
	  	  static const double TIME_LIMIT_EPSILON;

	  	  static const double NO_TIME_LIMIT_EPSILON;

	  	  static const double NO_TIME;
	/**
	 * Set the time accuracy value.  This value means any two times that
	 * are within this value of each other are considered the same [s].
	 * 
	 * @param acc accuracy
	 */
	  	static void set_time_accuracy(double acc);
	/**
	 * Set the horizontal accuracy value.  This value means any two positions that
	 * are within this value of each other are considered the same [m].
	 * @param acc accuracy
	 */
	  	static void set_horizontal_accuracy(double acc);
	/**
	 * Set the vertical accuracy value.  This value means any two positions that
	 * are within this value of each other are considered the same [m].
	 * @param acc accuracy
	 */
	  	static void set_vertical_accuracy(double acc);
	/** Return the time accuracy value (in seconds) 
	 * @return time accuracy
	 * */
	  	static double get_time_accuracy();
	/** Return the horizontal accuracy value (in meters) 
	 * @return horizontal accuracy
	 * */
	  	static double get_horizontal_accuracy();
	/** Return the vertical accuracy value (in meters) 
	 * @return vertical accuracy
	 * */
	  	static double get_vertical_accuracy();

	/** Return the horizontal accuracy value (in radians) 
	 * @return horizontal accuracy
	 * */
	  	static double get_latlon_accuracy();

	/** return default precision for output values 
	 * @return number of digits of precision
	 * */
	  	static int get_output_precision();

	/** set default precision for output values (0-16) 
	 * @param i number of digits of precision
	 * */
	  	static void set_output_precision(int i);

	/** Return true, if these two times are within the time accuracy of each other 
	 * 
	 * @param t1 a time
	 * @param t2 another time
	 * @return true, if almost equals
	 */
	  	static bool almost_equals_time(double t1, double t2);
	/** Return true, if these two positions [m] within the horizontal accuracy of each other 
	 * 
	 * @param x1 x coordinate for first value
	 * @param y1 y coordinate for first value
	 * @param x2 x coordinate for second value
	 * @param y2 y coordinate for second value
	 * @return true, if xy values are almost equals
	 */
	  	static bool almost_equals_xy(double x1, double y1, double x2, double y2);
	/** Return true, if this distance [m] is within the horizontal accuracy of zero 
	 * 
	 * @param d
	 * @return true, if almost equals
	 */
	  	static bool almost_equals_distance(double d);
	/** Return true, if these two angles [rad] are within the horizontal accuracy of each other 
	 * 
	 * @param d1 a distance
	 * @param d2 another distance
	 * @return true, if almost equals
	 */
	  	static bool almost_equals_radian(double d1, double d2);
	/** Return true, if this angle [rad] is within the horizontal accuracy of zero 
	 * 
	 * @param d distance
	 * @return true, if almost equals
	 */
	  	static bool almost_equals_radian(double d);
	/** Return true, if these two altitudes are within the vertical accuracy of each other 
	 * 
	 * @param a1 an altitude
	 * @param a2 another altitude
	 * @return true, if almost equals
	 */
	  	static bool almost_equals_alt(double a1, double a2);
  };

  /**
   *   \deprecated {Use Constants:: version}
   *   WAAS specification requirements (25 ft) for horizontal
   *   accuracy, 95% of the time.  In practice, the actual accuracy is
   *   much better. */
  static const double GPS_LIMIT_HORIZONTAL = _FormalATM_GPS_LIMIT_HORIZONTAL;// in order to keep both versions tied to a single entry, this needs to be defined here.
  /**
   *   \deprecated {Use Constants:: version}
   *
   *   WAAS specification requirements (25 ft) for vertical
   *   accuracy, 95% of the time. */
  static const double GPS_LIMIT_VERTICAL = _FormalATM_GPS_LIMIT_VERTICAL;// in order to keep both versions tied to a single entry, this needs to be defined here.
  /**
   * \deprecated {Use Constants:: version}
   * Most GPS units update with a frequency of at least 1 HZ.  */
  static const double TIME_LIMIT_EPSILON = _FormalATM_TIME_LIMIT_EPSILON;// in order to keep both versions tied to a single entry, this needs to be defined here.
  
  /**
   * \deprecated {Use Constants:: version}
   */
  static const double NO_TIME_LIMIT_EPSILON = _FormalATM_NO_TIME_LIMIT_EPSILON;

  /**
   * \deprecated {Use Constants:: version}
   */
  static const double NO_TIME = _FormalATM_NO_TIME;
  /**
   * \deprecated {Use Constants:: version}
   * Set the time accuracy value.  This value means any two times that
   * are within this value of each other are considered the same [s].
   */
  void set_time_accuracy(double acc);
  /**
   * \deprecated {Use Constants:: version}
   * Set the horizontal accuracy value.  This value means any two positions that
   * are within this value of each other are considered the same [m].
   */
  void set_horizontal_accuracy(double acc);
  /**
   * \deprecated {Use Constants:: version}
   * Set the vertical accuracy value.  This value means any two positions that
   * are within this value of each other are considered the same [m].
   */
  void set_vertical_accuracy(double acc);
  /**
   * \deprecated {Use Constants:: version}
   * Return the time accuracy value */
  double get_time_accuracy();
  /**
   * \deprecated {Use Constants:: version}
   * Return the horizontal accuracy value */
  double get_horizontal_accuracy();
  /**
   * \deprecated {Use Constants:: version}
   * Return the vertical accuracy value */
  double get_vertical_accuracy();
  /**
   * \deprecated {Use Constants:: version}
   * Return true, if these two times are within the time accuracy of each other */
  bool almost_equals_time(double t1, double t2);
  /**
   * \deprecated {Use Constants:: version}
   * Return true, if these two positions [m] within the horizontal accuracy of each other */
  bool almost_equals_xy(double x1, double y1, double x2, double y2);
  /**
   * \deprecated {Use Constants:: version}
   * Return true, if this distance [m] is within the horizontal accuracy of zero */
  bool almost_equals_distance(double d);
  /**
   * \deprecated {Use Constants:: version}
   * Return true, if these two angles [rad] are within the horizontal accuracy of each other */
  bool almost_equals_radian(double d1, double d2);
  /**
   * \deprecated {Use Constants:: version}
   * Return true, if this angle [rad] is within the horizontal accuracy of zero */
  bool almost_equals_radian(double d);
  /**
   * \deprecated {Use Constants:: version}
   * Return true, if these two altitudes are within the vertical accuracy of each other */
  bool almost_equals_alt(double a1, double a2);

}

#endif /* CONSTANTS_H_ */

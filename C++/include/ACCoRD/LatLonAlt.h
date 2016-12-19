/*
 * LatLonAlt.h - container to hold a geodesic position 
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef LATLONALT_H_
#define LATLONALT_H_

#include "Vect2.h"
#include "Velocity.h"
#include <string>

namespace larcfm {

  /**
   * Container to hold a latitude/longitude/altitude position 
   */
  class LatLonAlt {
  private:

    double lati;
    double longi;
    double alti;

	/**
	 * Creates a new position 
	 */
    LatLonAlt(double x, double y, double z);


  public:
    /** Make a LatLonAlt at (0,0,0) */
    LatLonAlt();

    /** Are these two LatLonAlt objects equal? */
    bool operator == (const LatLonAlt& v) const;
    /** Are these two LatLonAlt objects unequal? */
    bool operator != (const LatLonAlt& v) const;

    LatLonAlt& operator= (const LatLonAlt& lla);
    
    /** Are these two LatLonAlt objects equal? */
    bool equals(const LatLonAlt& v) const;
    bool almostEquals(const LatLonAlt& v) const;
	/** Are these two LatLonAlt almost equal, where 'almost' is defined by the given distances [m] */
    bool almostEquals(const LatLonAlt& a, double horizdist, double vertdist) const;
    bool almostEqualsHoriz (const LatLonAlt& v) const;

    /** Return latitude in degrees north */
    double latitude() const;

    /** Return longitude in degrees east */
    double longitude() const;

    /** Return altitude in [ft] */
    double altitude() const;

	/** Return latitude in internal units */
    double lat() const;
	/** Return longitude in internal units */
    double lon() const;
	/** Return altitude in internal units */
    double alt() const;

    double distanceH(const LatLonAlt& lla2) const;
	
    /** String representation with units of [deg,deg,ft] */
    std::string toString() const;
	/** String representation with units of [deg,deg,ft] */
    std::string toString(int precision) const;

	/** Return a string representation consistent with StateReader or PlanReader with the global default precision */
    std::string toStringNP() const;
	/** Return a string representation consistent with StateReader or PlanReader with user-specified precision */
    std::string toStringNP(int p) const;
    std::string toStringNP(const std::string& latunit, const std::string& lonunit, const std::string& zunit, int p) const;

  /**
   * Creates a zero position
   */
  static const LatLonAlt make();

  /**
   * Creates a new position that is a copy of <code>v</code>.
   * 
   * @param v position object
   */
  static const LatLonAlt mk(const LatLonAlt& v);

  /**
   * Creates a new position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
   * 
   * @param lat latitude [deg north latitude]
   * @param lon longitude [deg east longitude]
   * @param alt altitude [ft]
   */
  static const LatLonAlt make(double lat, double lon, double alt);

	/**
	 * Creates a new position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 *
	 * @param lat latitude [lat_unit north latitude]
	 * @param lat_unit units of latitude
	 * @param lon longitude [lon_unit east longitude]
	 * @param lon_unit units of longitude
	 * @param alt altitude [alt_unit]
	 * @param alt_unit units of altitude
	 */
	static const LatLonAlt make(double lat, std::string lat_unit, double lon, std::string lon_unit, double alt, std::string alt_unit);


  /**
   * Creates a new position with given values
   * 
   * @param lat latitude [internal]
   * @param lon longitude [internal]
   * @param alt altitude [internal]
   */
  static const LatLonAlt mk(double lat, double lon, double alt);

    /**
     * Creates a new LatLonAlt with only altitude changed
     * 
     * @param alt altitude [internal]
     */
    const LatLonAlt mkAlt(double alt) const;
	
    /**
     * Creates a new LatLonAlt with only altitude changed
     * 
     * @param alt altitude [feet]
     */
    const LatLonAlt makeAlt(double alt) const;

  /**
   * Return a copy of the current LatLonAlt with a zero altitude.  This is useful for creating projections that need to preserve altitude
   */
  const LatLonAlt zeroAlt() const;

  /**
    * Returns true if the current LatLonAlt has an "invalid" value
    */
   bool isInvalid() const;


   /** Compute a new lat/lon that is offset by dn meters north and de meters east.
    * This is a computationally fast estimate, and only should be used for relatively short distances.
    *
    * @param so  original lat/lon position
    * @param dn  offset in north direction (m)
    * @param de  offset in east direction  (m)
    * @return
    */
   const LatLonAlt linearEst(double dn, double de) const;

   /** Compute a new lat/lon that is obtained by moving with velocity vo for tm secs
    * This is a computationally fast estimate, and only should be used for relatively short distances.
    *
    * @param so   original lat/lon position
    * @param vo   velocity away from original position
    * @param tm   time of relocation
    * @return new lat/lon position in direction v0
    */
   const LatLonAlt linearEst(const Velocity& vo, double tm) const;


    /** A default value representing zero latitude, longitude, and altitude */
  static const LatLonAlt& ZERO();

  /** An invalid value, used for error reporting */
  static const LatLonAlt& INVALID();

	  /** 
	   * This parses a space or comma-separated string as a LatLonAlt (an inverse 
	   * to the toString method).  If three bare values are present, then it is interpreted as deg/deg/ft.
	   * If there are 3 value/unit pairs then each values is interpreted with regard 
	   * to the appropriate unit.  If the string cannot be parsed, an INVALID value is
	   * returned. 
	   * */
  static const LatLonAlt parse(const std::string& str);


  	  /**
	   * Normalizes the given latitude and longitude values to conventional spherical angles.  Thus
	   * values over the pole (95 degrees of latitude) convert to 85 degrees and the longitude 180 degrees different.
	   * 
	   * @param lat latitude
	   * @param lon longitude
	   * @param alt altitude
	   * @return normalized LatLonAlt value
	   */
  static LatLonAlt normalize(double lat, double lon, double alt);

	  /**
	   * Normalizes the given latitude and longitude values to conventional spherical angles.  Thus
	   * values over the pole (95 degrees of latitude) convert to 85 degrees and the longitude 180 degrees different.
	   * The altitude is assumed to be zero.
	   * 
	   * @param lat latitude
	   * @param lon longitude
	   * @return normalized LatLonAlt value
	   */
  static LatLonAlt normalize(double lat, double lon);

	  /**
	   * Creates a new LatLonAlt object from the current LatLonAlt object so that latitude and longitude values are 
	   * conventional spherical angles.  Thus
	   * values over the pole (95 degrees of latitude) convert to 85 degrees and the longitude 180 degrees different.
	   * 
	   * @return normalized LatLonAlt value
	   */
  LatLonAlt normalize() const;
  

	/**
	 * Return true if this point is (locally) west of the given point.
	 * @param a reference point
	 * @return true if this point is to the west of the reference point within a hemisphere they share.
	 */
	bool isWest(const LatLonAlt& a) const;

  };


}
#endif

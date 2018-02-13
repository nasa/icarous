/*
 * LatLonAlt.h - container to hold a geodesic position 
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2017 United States Government as represented by
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
	/** Are these two LatLonAlt almost equal, where 'almost' is defined by the given distances [m]
	 * 
	 * @param a LatLonAlt object
	 * @param horizEps allowed difference in horizontal dimension
	 * @param vertEps allowed difference in vertical dimension
	 * @return true if the two are almost equals
	 */
	bool almostEquals(const LatLonAlt& a, double horizEps, double vertEps) const;
	bool almostEquals2D (const LatLonAlt& v, double horizEps) const;

	/** Return latitude in degrees north 
	 * @return latitude value in degrees
	 * */
	double latitude() const;

	/** Return longitude in degrees east 
	 * @return longitude in degrees
	 * */
	double longitude() const;

	/** Return altitude in [ft] 
	 * @return altitude in feet
	 * */
	double altitude() const;

	/** Return latitude in internal units 
	 * @return latitude value
	 * */
	double lat() const;
	/** Return longitude in internal units 
	 * @return longitude value
	 * */
	double lon() const;
	/** Return altitude in internal units 
	 * @return altitude value
	 * */
	double alt() const;

	double distanceH(const LatLonAlt& lla2) const;

	/** String representation with units of [deg,deg,ft] */
	std::string toString() const;
	/** String representation with units of [deg,deg,ft] 
	 * @param precision digits of precision for the output values
	 * @return string representation
	 * */
	std::string toString(int precision) const;

	/** Return a string representation consistent with StateReader or 
	 * PlanReader with the global default precision 
	 * 
	 * @return string representation
	 * */
	std::string toStringNP() const;
	/** Return a string representation consistent with StateReader or 
	 * PlanReader with user-specified precision 
	 * @param p number of digits of precision for output
	 * @return a string representation
	 * */
	std::string toStringNP(int p) const;

	/** Return a string representation consistent with StateReader or PlanReader
	 * with user-specified units and precision
	 *
	 * @param latunit units for latitude
	 * @param lonunit units for longitude
	 * @param zunit units for altitude
	 * @param precision number of digits of precision for output
	 * @return string representation
	 */
	std::string toStringNP(const std::string& latunit, const std::string& lonunit, const std::string& zunit, int p) const;

	/** Return a string representation consistent with StateReader or PlanReader
	 * with user-specified units and precision
	 *
	 * @param zunit units for altitude
	 * @param precision number of digits of precision for output
	 * @return string representation
	 */
	std::string toStringNP(const std::string& zunit, int p) const;

	/**
	 * Creates a zero position
	 */
	static const LatLonAlt make();

	/**
	 * Creates a new position that is a copy of <code>v</code>.
	 * 
	 * @param v position object
	 * @return new LatLonAlt object
	 */
	static const LatLonAlt mk(const LatLonAlt& v);

	/**
	 * Creates a new position with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude [deg north latitude]
	 * @param lon longitude [deg east longitude]
	 * @param alt altitude [ft]
	 * @return new LatLonAlt object
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
	 * @return new LatLonAlt object
	 */
	static const LatLonAlt make(double lat, std::string lat_unit, double lon, std::string lon_unit, double alt, std::string alt_unit);


	/**
	 * Creates a new position with given values
	 * 
	 * @param lat latitude [internal]
	 * @param lon longitude [internal]
	 * @param alt altitude [internal]
	 * @return new LatLonAlt object
	 */
	static const LatLonAlt mk(double lat, double lon, double alt);

	/**
	 * Creates a new LatLonAlt with only altitude changed
	 * 
	 * @param alt altitude [internal]
	 * @return new LatLonAlt object with modified altitude
	 */
	const LatLonAlt mkAlt(double alt) const;

	/**
	 * Creates a new LatLonAlt with only altitude changed
	 * 
	 * @param alt altitude [feet]
	 * @return new LatLonAlt object with modified altitude
	 */
	const LatLonAlt makeAlt(double alt) const;

	/**
	 * Return a copy of the current LatLonAlt with a zero altitude.  This is useful for 
	 * creating projections that need to preserve altitude 
	 * 
	 * @return a LatLonAlt copy with a zero altitude
	 */
	const LatLonAlt zeroAlt() const;

	/**
	 * Returns true if the current LatLonAlt has an "invalid" value
	 * @return true if invalid
	 */
	bool isInvalid() const;


	/** Compute a new lat/lon that is offset by dn meters north and de meters east.
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * 
	 * @param dn  offset in north direction (m)
	 * @param de  offset in east direction  (m)
	 * @return new position
	 */
	const LatLonAlt linearEst(double dn, double de) const;

	/** Compute a new lat/lon that is obtained by moving with velocity vo for tm secs
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * 
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
	 * 
	 * @param str string representing a latitude and longitude
	 *  @return a LatLonAlt that corresponds to the given string
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

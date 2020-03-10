/* Defines a 4D Waypoint
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef NAVPOINT_H_
#define NAVPOINT_H_

#include "Position.h"
#include "Point.h"
#include "Velocity.h"

namespace larcfm {
  
/** 
 * An immutable class representing a position at a time. In addition a label (or name) can be attached 
 * to one of these NavPoints.
 */
class NavPoint {

private:
	Position p;
	double t;
	std::string name_s;


public:
    /** Construct a zero NavPoint */
    NavPoint();

	/**
	 * Construct a new NavPoint from a position and a time.
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 * 
	 * @param p position of new NavPoint
	 * @param t time of new NavPoint
	 */
    NavPoint(const Position& p, double t);

	/**
	 * Construction a non-TCP NavPoint piecemeal
	 * If a NavPoint is derived from an existing NavPoint, use one or more of the "make" factory functions instead.
	 * 
	 * @param p position of new NavPoint
	 * @param t time of new NavPoint
	 * @param label string label of new NavPoint
	 */
    NavPoint(const Position& p, double t, const std::string& label);



	/**
	 * Creates a new lat/lon NavPoint with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
	 * 
	 * @param lat latitude [deg north latitude]
	 * @param lon longitude [deg east longitude]
	 * @param alt altitude [ft]
	 * @param t   time
	 * @return a new NavPoint
	 */
    static NavPoint makeLatLonAlt(double lat, double lon, double alt, double t);

    /**
     * Creates a new lat/lon NavPoint with coordinates (<code>lat</code>,<code>lon</code>,<code>alt</code>).
     *
     * @param lat latitude (radians)
     * @param lon longitude (radians)
     * @param alt altitude [m]
     * @param t   time
     * @return a new NavPoint
     */
    static NavPoint mkLatLonAlt(double lat, double lon, double alt, double t);

	/**
	 * Creates a new Euclidean NavPoint with coordinates (<code>x</code>,<code>y</code>,<code>z</code>).
	 * 
	 * @param x coordinate [nmi]
	 * @param y coordinate [nmi]
	 * @param z altitude [ft]
	 * @param t time [s]
	 * @return a new NavPoint
	 */
    static NavPoint makeXYZ(double x, double y, double z, double t);

	/**
	 * Returns true if any components of this NavPoint are marked as Invalid
	 * @return true, if invalid
	 */
    bool isInvalid() const;


    
    
    /** A zero NavPoint, in lat/lon coordinates */
    static const NavPoint& ZERO_LL();
    /** A zero NavPoint, in xyz coordinates */
    static const NavPoint& ZERO_XYZ();

    static const NavPoint& INVALID();

    /**
     * Checks if two NavPoint are almost the same.
     * 
     * @param v NavPoint for comparison
     * 
     * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
     * to <code>v</code>.
     */
    bool almostEquals(const NavPoint& v) const;
	/**
	 * Checks if two NavPoint are almost the same in position.
	 * 
	 * @param v NavPoint for comparison
	 * @param epsilon_horiz maximum allowed horizontal difference
	 * @param epsilon_vert maximum allowed vertical difference
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 * 
	 */
    bool almostEqualsPosition(const NavPoint& v, double epsilon_horiz, double epsilon_vert) const;
    
	/**
	 * Checks if two NavPoint are almost the same in position.
	 * 
	 * @param v NavPoint for comparison
	 * 
	 * @return <code>true</code>, if <code>this</code> NavPoint is almost equal 
	 * to <code>v</code>.
	 */
    bool almostEqualsPosition(const NavPoint& v) const;

    /** Equality */
    bool operator == (const NavPoint& v) const;
    
    /** Equality */
    bool equals(const NavPoint& v) const;

    /** Inequality */
    bool operator != (const NavPoint& v) const;
    
	/** Return the horizontal position vector 
	 * @return a Vect2
	 * */
    Vect2  vect2() const;
    
	/** Return the three dimensional position vector 
	 * @return a Point object (essentially a Vect3 with more natural units)
	 * */
    Point  vect3() const;
    
    /** Return the LatLonAlt object */
    const LatLonAlt& lla() const;
    
    /** Return the Position object */
    const Position& position() const;
    
	/** Return the x coordinate 
	 * @return x coordinate
	 * */
    double x() const;
	/** Return the y coordinate 
	 * @return y coordinate
	 * */
    double y() const;
	/** Return the z coordinate 
	 * @return z coordinate
	 * */
    double z() const;
	/** Return the latitude 
	 * 
	 * @return latitude [radian]
	 * */
    double lat() const;
	/** Return the longitude
	 * 
	 * @return longitude [radian]
	 *  */
    double lon() const;
	/** Return the altitude 
	 * 
	 * @return altitude [m] */
    double alt() const;
    /** Return the latitude in degrees north */
    double latitude() const;
    /** Return the longitude in degrees east */
    double longitude() const;
    /** Return the altitude in [ft] */
    double altitude() const;
    /** Return the x coordinate in [nmi] */
    double xCoordinate() const;
    /** Return the y coordinate in [nmi] */
    double yCoordinate() const;
    /** Return the z coordinate in [ft] */
    double zCoordinate() const;
	/** Return the time [s] 
	 * @return time in [s]
	 * */
    double time() const;

    /** Return the label (or name) for this point */
    const std::string& name() const;
    
	/** Returns true if the "label" label of this NavPoint has been set. 
	 * 
	 * @return true, if label is set
	 * */
    bool isNameSet() const;

	/** Is this point specified in lat/lon? 
	 * @return true, if the point is Lat/Lon (not Euclidean)
	 * */
    bool isLatLon() const;

	/** Make a new NavPoint from the current one with the latitude changed
	 * @param lat new latitude value [radian]
	 * @return a new NavPoint
	 *  */
    const NavPoint mkLat(const double lat) const;
	/** Make a new NavPoint from the current one with the X coordinate changed 
	 * @param x new x value [m]
	 * @return a new NavPoint
	 * */
    const NavPoint mkX(const double x) const;
	/** Make a new NavPoint from the current one with the longitude changed 
	 * @param lon new longitude value [radian]
	 * @return a new NavPoint
	 * */
    const NavPoint mkLon(const double lon) const;
	/** Make a new NavPoint from the current one with the Y coordinate changed 
	 * @param y new y value [m]
	 * @return a new NavPoint
	 * */
    const NavPoint mkY(const double y) const;
	/** Make a new NavPoint from the current one with the altitude changed 
	 * @param alt a new altitude value [m]
	 * @return a new NavPoint
	 * */
    const NavPoint mkAlt(const double alt) const;
	/** Make a new NavPoint from the current one with the Z coordinate changed 
	 * @param z new z value [m]
	 * @return a new NavPoint
	 * */
    const NavPoint mkZ(const double z) const;
	/** Make a new NavPoint from the current one with the time changed, source time is not changed 
	 * @param time time in seconds [s]
	 * @return a new NavPoint
	 * */
    const NavPoint makeTime(const double time) const;

	/** Make a new NavPoint from the current with the given name 
	 * @param label new string label
	 * @return a new NavPoint
	 * */
    const NavPoint makeName(const std::string& label) const;
    const NavPoint appendName(const std::string& label) const;
    const NavPoint appendNameNoDuplication(const std::string& label) const;

	/**
	 * Make an "added" point that does not include valid source info
	 */
    const NavPoint makeAdded() const;

	/** Return a new NavPoint that shares all attributes with the specified NavPoint except Position and Time */
    const NavPoint makeMovedFrom(const NavPoint& o) const;

	/** Return a new NavPoint that shares all attributes with this 
	 * one, except position 
	 * 
	 * @param p new Position object
	 * @return a new NavPoint object
	 * */
    const NavPoint makePosition(const Position& p) const;

	/** 
	 * Calculate and return the initial velocity between the two NavPoint. 
	 * This function is commutative: direction between points is always 
	 * determined by the time ordering of the two points.
	 * 
	 * @param s1 one NavPoint
	 * @param s2 another NavPoint
	 * @return velocity between two points
	 */
    static Velocity initialVelocity(const NavPoint& s1, const NavPoint& s2);

	/** 
	 * Calculate and return the initial velocity between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 * @return velocity
	 */
    Velocity initialVelocity(const NavPoint& s) const;

	/** 
	 * Calculate and return the final velocity between two NavPoint
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 *
	 * @param s1 a NavPoint
	 * @param s2 another NavPoint
	 * @return velocity
	 */
    static Velocity finalVelocity(const NavPoint& s1, const NavPoint& s2);

	/** 
	 * Calculate and return the vertical speed between the current point and the given point 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s the given NavPoint
	 * @return vertical speed
	 */
    double verticalSpeed(const NavPoint& s) const;

	/** 
	 * Calculate and return the REPRESENTATIVE LINEAR velocity between two NavPoints. 
	 * This function is commutative: direction between points is always determined by the time ordering of the two points.
	 * 
	 * @param s1 a NavPoint
	 * @param s2 another NavPoint
	 * @return velocity
	 */
    static Velocity averageVelocity(const NavPoint& s1, const NavPoint& s2);

	/** Perform a linear projection of the current NavPoint with given velocity and time.  If the 
	 * current NavPoint is in LatLonAlt, then a constant track angle linear projection is used.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 * 
	 * @param v velocity
	 * @param time  relative time 
	 * @return a linear extrapolation NavPoint
	 */
    const NavPoint linear(const Velocity& v, const double time) const;

	/** Compute a new lat/lon that is obtained by moving with velocity vo for tm secs
	 * This is a computationally fast estimate, and only should be used for relatively short distances.
	 * Negative time is a projection in the opposite direction (along the same great circle, if in latlon) and into the past
	 *
	 * @param vo   velocity away from original position
	 * @param tm   time of relocation
	 * @return new lat/lon position in direction v0
	 */
    const NavPoint linearEst(const Velocity& vo, double tm) const;


	/**
	 * return a new NavPoint between this NavPoint and np that corresponds to time by interpolation
	 * This function is commutative with respect to the points: direction between points is always determined by the time ordering of the two points.
	 * Negative time results in a velocity in the opposite direction (along the same great circle, if in latlon)
	 *
	 * @param np    next NavPoint
	 * @param time  time
	 * @return      interpolated NavPoint
	 */
    const NavPoint interpolate(const NavPoint& np, const double time) const;


	/** Horizontal distance between this point and the other given point
	 * 
	 * @param np2 another point
	 * @return horizontal distance
	 * */
    double distanceH(const NavPoint& np2) const;

	/** Vertical distance between this point and the other given point
	 * 
	 * @param np2 another point
	 * @return vertical distance
	 * */
	double distanceV(const NavPoint& np2) const;


	/** String representation, using the default output precision (see Contants.get_output_precision()) */
    std::string toStringShort() const;
	/** String representation, using the give precision, NOT including name field
	 * 
	 *  @param precision number of digits of precision
	 *  @return a string representation
	 *  */
    std::string toStringShort(int precision) const;

	/** String representation, using the default output precision (see Contants.get_output_precision()) INCLUDING name field 
	 * 
	 * @return a string representation
	 * */
    std::string toString() const;
	/** String representation, using the give precision INCLUDING "name" field
	 * 
	 * @param precision number of digits of precision
	 * @return a string representation*/
    std::string toString(int precision) const;

    std::vector<std::string> toStringList(int precision, bool tcp) const;

private:
	const NavPoint copy(const Position& p) const;



  };
  
  
}

#endif /* NAVPOINT_H_ */

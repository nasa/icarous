/*
 * Velocity.h
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://research.nianet.org/fm-at-nia/ACCoRD
 *
 * NOTES: 
 * Track is True North/clockwise
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef VELOCITY_H_
#define VELOCITY_H_

#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include <string>

namespace larcfm {

/**
 * This class represents a 3-dimensional velocity vector.  The 
 * track angle is defined by the "true north, clockwise convention."
 *
 */
class Velocity: public Vect3 {

private:

	/**
	 * Instantiates a new velocity in internal units.
	 *
	 * @param vx the vx
	 * @param vy the vy
	 * @param vz the vz
	 */
	Velocity(const double vx, const double vy, const double vz);


public:
	/**
	 * Instantiates a zero velocity.
	 */
	Velocity();

	Velocity(const Vect3& v3);

  /**
   * Angle in explicit units in corresponding range [-<code>Math.PI</code>, <code>Math.PI</code>].
   * Convention is counter-clockwise with respect to east.
   * 
   * @param uangle the explicit units of track angle
   * 
   * @return the track angle [rad]
   */
	double angle(const std::string& uangle) const;

  /**
   * Angle in radians in the range [-<code>Math.PI</code>, <code>Math.PI</code>].
   * Convention is counter-clockwise with respect to east.
   * 
   * @return the track angle [rad]
   */
	double angle() const;

	/**
	 * Track angle in radians in the range [-<code>Math.PI</code>, <code>Math.PI</code>].
	 * Convention is clockwise with respect to north.
	 *
	 * @return the track angle [rad]
	 */
	double trk() const;

	/**
	 * Track angle in explicit units in the corresponding range [-<code>Math.PI</code>, <code>Math.PI</code>].
	 * Convention is clockwise with respect to north.
	 *
	 * @param utrk the explicit units of track angle
	 *
	 * @return the track angle [utrk]
	 */
	double track(const std::string& utrk) const;

	/**
	 * Compass angle in radians in the range [<code>0</code>, <code>2*Math.PI</code>).
	 * Convention is clockwise with respect to north.
	 * 
	 * @return the compass angle [rad]
	 */
	double compassAngle() const;

	/**
	 * Compass angle in explicit units in corresponding range [<code>0</code>, <code>2*Math.PI</code>).
	 * Convention is clockwise with respect to north.
	 *
	 *  @param u the explicit units of compass angle
	 *
	 *  @return the compass angle [u]
	 */
	double compassAngle(const std::string& u) const;

	/**
	 * Ground speed in internal units.
	 * 
	 * @return the ground speed
	 */
	double gs() const;

	/**
	 * Ground speed in explicit units.
	 *
	 * @param ugs the explicit units of ground speed
	 *
	 * @return the ground speed [ugs]
	 */
	double groundSpeed(const std::string& ugs) const;

	/**
	 * Vertical speed in internal units.
	 * 
	 * @return the vertical speed
	 */
	double vs() const;

	/**
	 * Vertical speed in explicit units.
	 *
	 * @param uvs the explicit units of vertical speed
	 *
	 * @return the vertical speed [uvs]
	 */
	double verticalSpeed(const std::string& uvs) const;

	/** compare Velocities: return true iff delta is within specified limits
	 *
	 */
	bool compare(const Velocity& v, double maxTrk, double maxGs, double maxVs);

	/**
	 * Compare two velocities based on horizontal and vertical components.  This could be used against a set of nacV ADS-B limits, for example.
	 * @param v other Velocity
	 * @param horizDelta horizontal tolerance (absolute value)
	 * @param vertDelta vertical tolerance (absolute value)
	 * @return true if the velocities are within both horizontal and vertical tolerances of each other.
	 */
	bool compare(const Velocity& v, double horizDelta, double vertDelta);

	/** String representation of the velocity in polar coordinates (compass angle and groundspeed) */
	std::string toString() const;

	/** String representation of the velocity in polar coordinates (compass angle and groundspeed) in [deg, knot, fpm].  This
	 * method does not output units.
	 * @param prec precision (0-15)
	 */
	std::string toString(int prec) const;

	/** String representation of the velocity in polar coordinates (compass angle and groundspeed) */
	std::string toStringUnits() const;

	/** String representation (trk,gs,vs) with the given units */
	std::string toStringUnits(const std::string& trkUnits, const std::string& gsUnits, const std::string& vsUnits) const;

  /**
   * Euclidean vector representation to arbitrary precision, in [knot,knot,fpm]
   */
	std::string toStringXYZ() const;

	std::string toStringXYZ(int prec) const;

	/**
	 * Return an array of string representing each value of the velocity in the units deg, knot, fpm.
	 * @return array of strings
	 */
	std::vector<std::string> toStringList() const;

	/**
	 * Return an array of string representing each value of the velocity in the units deg, knot, fpm.
	 * @param precision the number of digits to display
	 * @return array of strings
	 */
	std::vector<std::string> toStringList(int precision) const;

	/**
	 * Return an array of string representing each value of the velocity in terms of its Cartesian dimensions in units knot, knot, fpm.
	 * @return array of strings
	 */
	std::vector<std::string> toStringXYZList() const;

	/**
	 * Return an array of string representing each value of the velocity in terms of its Cartesian dimensions in units knot, knot, fpm.
	 *
	 * @param precision the number of digits to display
	 * @return array of strings
	 */
	std::vector<std::string> toStringXYZList(int precision) const;

	/** String representation, default number of decimal places, without parentheses */
	std::string toStringNP() const;

	/**
	 * String representation, with user-specified precision
	 * @param precision number of decimal places (0-15)
	 * @return
	 */
	std::string toStringNP(int precision) const;

	/**
	 * New velocity from Vect3 in internal units.
	 *
	 * @param v the 3-D velocity vector [mps,mps,mps]
	 *
	 * @return the velocity
	 */
	static Velocity make(const Vect3& v);

	/**
	 * New velocity from Vect2 (setting the vertical speed to 0.0).
	 *
	 * @param v the 2-D velocity vector (in internal units).
	 *
	 * @return the velocity
	 */
	static Velocity make(const Vect2& v);

	/**
	 * New velocity from Euclidean coordinates in internal units.
	 * 
	 * @param vx the x-velocity [internal]
	 * @param vy the y-velocity [internal]
	 * @param vz the z-velocity [internal]
	 * 
	 * @return the velocity
	 * 
	 */
	static Velocity mkVxyz(const double vx, const double vy, const double vz);


  /**
   * New velocity from Euclidean coordinates in "conventional" units.
   *
   * @param vx the x-velocity [knot]
   * @param vy the y-velocity [knot]
   * @param vz the z-velocity [fpm]
   *
   * @return the velocity
   */
	static Velocity makeVxyz(const double vx, const double vy, const double vz);


	/**
	 * New velocity from Euclidean coordinates in explicit units.
	 *
	 * @param vx the x-velocity [uvxy]
	 * @param vy the y-velocity [uvzy]
	 * @param uvxy the units of vx and vy
	 * @param vz the z-velocity [uvz]
	 * @param uvz the units of vz
	 *
	 * @return the velocity
	 */
	static Velocity makeVxyz(const double vx, const double vy, const std::string& uvxy,
			const double vz, const std::string& uvz);

	/**
	 * New velocity from Track, Ground Speed, and Vertical speed in internal units.
	 * Note that this uses trigonometric functions, and may introduce numeric instability.
	 * 
	 * @param trk the track angle [internal]
	 * @param gs the ground speed [internal]
	 * @param vs the vs [internal]
	 * 
	 * @return the velocity
	 */
	static Velocity mkTrkGsVs(const double trk, const double gs, const double vs);


  /**
   * New velocity from Track, Ground Speed, and Vertical speed in explicit units.
   * Note that this uses trigonometric functions, and may introduce numeric instability.
   * 
   * @param trk the track angle [deg]
   * @param gs the ground speed [knot]
   * @param vs the vertical speed [fpm]
   * 
   * @return the velocity
   */
	static Velocity makeTrkGsVs(const double trk, const double gs, const double vs);


	/**
	 * New velocity from Track, Ground Speed, and Vertical speed in explicit units.
	 * Note that this uses trigonometric functions, and may introduce numeric instability.
	 * 
	 * @param trk the track angle [utrk]
	 * @param utrk the units of trk
	 * @param gs the ground speed [ugs]
	 * @param ugs the units of gs
	 * @param vs the vertical speed [uvs]
	 * @param uvs the units of vs
	 * 
	 * @return the velocity
	 */
	static Velocity makeTrkGsVs(const double trk, const std::string& utrk,
			const double gs, const std::string& ugs,
			const double vs, const std::string& uvs);

	static Velocity mkVel(const Vect3& p1,const Vect3& p2, double speed);

  /**
   * Return the velocity if moving from p1 to p2 over the given time
   * @param p1 first point
   * @param p2 second point
   * @param dt time 
   * @return the velocity
   */
	static Velocity genVel(const Vect3& p1, const Vect3& p2, double dt) ;


  /**
   * New velocity from existing velocity by adding the given track angle to this 
   * vector's track angle.  Essentially, this rotates the vector, a positive
   * angle means a clockwise rotation.
   * @param trk track angle [rad]
   * @return new velocity
   */
	Velocity mkAddTrk(double trk) const;


	/** Return a zero velocity vector */
	static const Velocity& ZEROV();

	/** An invalid velocity, used for error reporting */
	static const Velocity& INVALIDV();


	/** Return the x component of velocity given the track and ground
	 * speed.  The track angle is assumed to use the radians from true
	 * North-clockwise convention.
	 */
	static double trkgs2vx(double trk, double gs);

	/** Return the y component of velocity given the track and ground
	 *	speed.  The track angle is assumed to use the radians from
	 *	true North-clockwise convention.
	 */
	static double trkgs2vy(double trk, double gs);

	/** Return the 2-dimensional Euclidean vector for velocity given the track and ground
	 *	speed.  The track angle is assumed to use the radians from
	 *	true North-clockwise convention.
	 */
	static Vect2 trkgs2v(double trk, double gs);

	/**
	 * New velocity from existing velocity, changing only the track
	 * @param trk track angle [rad]
	 * @return new velocity
	 */
	Velocity mkTrk(double trk) const;

	/**
	 * New velocity from existing velocity, changing only the track
	 * @param trk track angle [u]
	 * @param u units
	 * @return new velocity
	 */
	Velocity mkTrk(double trk, std::string u) const;

	/**
	 * New velocity from existing velocity, changing only the ground speed
	 * @param gs ground speed [m/s]
	 * @return new velocity
	 */
	Velocity mkGs(double gs) const;

	/**
	 * New velocity from existing velocity, changing only the ground speed
	 * @param gs ground speed [u]
	 * @param u units
	 * @return new velocity
	 */
	Velocity mkGs(double gs, std::string u) const;

	/**
	 * New velocity from existing velocity, changing only the vertical speed
	 * @param vs vertical speed [m/s]
	 * @return new velocity
	 */
	Velocity mkVs(double vs) const;

	/**
	 * New velocity from existing velocity, changing only the vertical speed
	 * @param vs vertical speed [u]
	 * @param u units
	 * @return new velocity
	 */
	Velocity mkVs(double vs, std::string u) const;

	/**
	 * Returns a unit velocity vector in the direction of the original velocity
	 */
	Velocity Hat() const;

  /**
   * Returns a unit velocity vector in the direction of the original velocity in the XY plane
   */
	Velocity Hat2D() const;

	Velocity Neg() const;

	/**
	 * If the z component of the velocity vector is smaller than the threshold, return a new vector with this component set to 0.
	 * Return the original vector if the vertical speed is greater than the threshold.
	 * @param threshold   level of vertical speed below which the vector is altered
	 * @return the new velocity
	 */

	Velocity zeroSmallVs(double threshold) const ;

	/** This parses a space or comma-separated string as a XYZ Velocity (an inverse to the toStringXYZ method).  If three bare values are present, then it is interpreted as internal units.
	 * If there are 3 value/unit pairs then each values is interpreted wrt the appropriate unit.  If the string cannot be parsed, an INVALID value is
	 * returned. */
	static Velocity parseXYZ(const std::string& str);

	/** This parses a space or comma-separated string as a Trk/Gs/Vs Velocity (an inverse to the toString method).  If three bare values are
	 * present, then it is interpreted as degrees/knots/fpm. If there are 3 value/unit pairs then each values is
	 * interpreted wrt the appropriate unit.  If the string cannot be parsed, an INVALID value is
	 * returned. */
	static Velocity parse(const std::string& str);

};

/**
 * \deprecated {Use Velocity:: version.}
 * Return the x component of velocity given the track and ground
 * speed.  The track angle is assumed to use the radians from true
 * North-clockwise convention.
 */
double trkgs2vx(double trk, double gs);

/**
 * \deprecated {Use Velocity:: version.}
 * Return the y component of velocity given the track and ground
 *	speed.  The track angle is assumed to use the radians from
 *	true North-clockwise convention.
 */
double trkgs2vy(double trk, double gs);

/**
 * \deprecated {Use Velocity:: version.}
 * Return the 2-dimensional Euclidean vector for velocity given the track and ground
 *	speed.  The track angle is assumed to use the radians from
 *	true North-clockwise convention.
 */
Vect2 trkgs2v(double trk, double gs);

}

#endif /* VELOCITY_H_ */

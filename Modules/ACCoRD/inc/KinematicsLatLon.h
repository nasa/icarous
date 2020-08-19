/*
 * KinematicsLatLon.h
 * 
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef KINEMATICS_LATLON_H_
#define KINEMATICS_LATLON_H_

#include "Vect3.h"
#include "Velocity.h"
#include "Quad.h"
#include "Tuple5.h"
#include "StateVector.h"
#include "Triple.h"
#include "LatLonAlt.h"
#include "Kinematics.h"


namespace larcfm {

/**
 * <p>A library of functions to aid the computation of the kinematics of an aircraft.  This
 * library is currently under development and is far from complete.  The majority of the functions
 * handle constant velocity turns and movement with constant ground speed acceleration.</p>
 * 
 * <p>Unless otherwise noted, all kinematics function parameters are in internal units -- angles are in radians,
 * linear speeds are in m/s, distances are in meters, time is in seconds.</p>
 * 
 */
class KinematicsLatLon {
public:

    static bool chordalSemantics;

	/**
	 * Linearly project the given position and velocity to a new position and velocity
	 * @param sv0  initial position and velocity
	 * @param t   time
	 * @return linear projection of sv0 to time t
	 */

	static std::pair<LatLonAlt,Velocity> linear(const LatLonAlt& so, const Velocity& vo, double t);

	/**
	 * Linearly project the given position and velocity to a new position and velocity
	 * @param sv0  initial position and velocity
	 * @param t   time
	 * @return linear projection of sv0 to time t
	 */
	static std::pair<LatLonAlt,Velocity> linear(std::pair<LatLonAlt,Velocity> sv0, double t);

	/**
	 * Determine the earth-surface radius of a turn given the ground speed and omega (angular velocity).
	 * @param speed
	 * @param omega
	 * @return turn radius
	 */
	static double turnRadiusByRate(double speed, double omega);

	
	/**
	 * Position/Velocity after turning t time units according to track rate omega
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time into turn
	 * @param omega       rate of change of track, sign indicates direction
	 * @return Position/Velocity after t time
	 */
	static std::pair<LatLonAlt,Velocity> turnOmega(const LatLonAlt& so, const Velocity& vo, double t, double omega);

	/**
	 * Position/Velocity after turning t time units according to track rate omega
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time of turn
	 * @param omega       rate of change of track, sign indicates direction
	 * @return Position/Velocity after t time
	 */
	static std::pair<LatLonAlt,Velocity> turnOmega(std::pair<LatLonAlt,Velocity> sv0, double t, double omega) ;

//	static std::pair<LatLonAlt,Velocity> turnRadius(const LatLonAlt& so, const Velocity& vo, double t, double signedRadius);

	
	/**
	 * Position/Velocity after turning (does not compute altitude!!)
	 *
	 * Note: will be used in a context where altitude is computing subsequently
	 * @param so          starting position
	 * @param center      center of turn
	 * @param R           radius
	 * @param d           distance into turn (sign indicates direction)
	 * @return Position/Velocity after turning distance d
	 */
	static std::pair<LatLonAlt,Velocity> turnByDist2D(const LatLonAlt& so, const LatLonAlt& center, int dir, double d, double gsAtd);

	/** 
	 * Position/Velocity after turning (does not compute altitude!!)
	 * 
	 * Note: will be used in a context where altitude is computing subsequently
	 * 
	 * @param so          starting position
	 * @param center      center of turn
	 * @param dir         direction of turn
	 * @param d           distance into turn (non-negative)
	 * @return            Position/Velocity after turning distance d
	 */
	static LatLonAlt turnByDist2D(const LatLonAlt& so, const LatLonAlt& center, int dir, double d);

	/** 
	 * Position/Velocity after turning (does not compute altitude!!)
	 * 
	 * Note: will be used in a context where altitude is computing subsequently
	 * 
	 * @param so          starting position
	 * @param center      center of turn
	 * @param alpha       angle
	 * @return            Position/Velocity after turning distance d
	 */
	static LatLonAlt turnByAngle2D(const LatLonAlt& so, const LatLonAlt& center, double alpha);

	/**
	 * Position/Velocity after turning t time units right or left with radius R in the direction turnRight
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time of turn
	 * @param R           turn radius (positive)
	 * @param turnRight   true iff only turn direction is to the right
	 * @return Position/Velocity after t time
	 */
	static std::pair<LatLonAlt,Velocity> turn(LatLonAlt s0, Velocity v0, double t, double R, bool turnRight);

	/**
	 * Position/Velocity after turning t time units right or left with with radius R in the direction turnRight
	 * @param sv0         Pair (initial position, initial velocity)
	 * @param t           time point of interest
	 * @param R           turn radius (positive)
	 * @param turnRight   true iff only turn direction is to the right
	 * @return Position/Velocity pair after t time
	 */
	static std::pair<LatLonAlt,Velocity> turn(std::pair<LatLonAlt,Velocity> sv0, double t, double R, bool turnRight);

	/**
	 *  Position/Velocity after t time units turning at the rate of "omega," after that
	 *  continue in a straight line.  This function can make a turn greater than 180 deg
	 * @param so         initial position and velocity
	 * @param t          time point of interest
	 * @param turnTime   total time of turn [secs]
	 * @param omega 	turn rate
	 * @return Position/Velocity after time t
	 */
	static std::pair<LatLonAlt,Velocity> turnUntilTimeOmega(std::pair<LatLonAlt,Velocity> svo, double t, double turnTime, double omega) ;

	/**
	 *  Position/Velocity after t time units turning at the rate of "omega," after that
	 *  continue in a straight line.  This function can make a turn greater than 180 deg
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param t          time point of interest
	 * @param turnTime   total time of turn [secs]
	 * @param omega 	turn rate
	 * @return Position/Velocity after time t
	 */
	static std::pair<LatLonAlt,Velocity> turnUntilTimeOmega(const LatLonAlt& so, const Velocity& vo, double t, double turnTime, double omega) ;

	/**
	 *  Position/Velocity after t time units turning in <b>minimal</b> direction until goalTrack is reached, after that
	 *  continue in a straight line.  The the time t is not long enough to complete the turn, then a position/velocity towards the goal track is returned.
	 *
	 * @param svo         starting position and velocity
	 * @param t          maneuver time [s]
	 * @param goalTrack  the track angle where the turn stops
	 * @param maxBank    the maximum bank angle of the aircraft, must be in (0,pi/2)
	 * @return Position and Velocity after time t
	 */
	static std::pair<LatLonAlt,Velocity> turnUntil(std::pair<LatLonAlt,Velocity> svo, double t, double goalTrack, double maxBank);


	/**
	 *  Position/Velocity after t time units turning in *minimal* direction until goalTrack is reached, after that
	 *  continue in a straight line
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param t          maneuver time [s]
	 * @param goalTrack  the track angle where the turn stops
	 * @param maxBank    the bank angle of the aircraft making the turn (positive)
	 * @return Position/Velocity after time t
	 */
	static std::pair<LatLonAlt,Velocity> turnUntil(const LatLonAlt& so, const Velocity& vo, double t, double goalTrack, double maxBank);

	/**
	 *  Position/Velocity after t time units turning in direction "turnRight" for a total of turnTime, after that
	 *  continue in a straight line.  This function can make a turn greater than 180 deg
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param t          time point of interest
	 * @param turnTime   total time of turn [secs]
	 * @param R          turn radius (positive)
	 * @param turnRight  true iff only turn direction is to the right
	 * @return Position/Velocity after time t
	 */
	static std::pair<LatLonAlt,Velocity> turnUntilTimeRadius(const std::pair<LatLonAlt,Velocity>& svo, double t, double turnTime, double R, bool turnRight);

	/** 
	 * return center of turn from position on turn (and it track) and radius
	 * @param s0 point on turn
	 * @param trk track at point
	 * @param radius radius of turn
	 * @param dir +1 clockwise, -1 counterclockwise
	 * @return turn center
	 */
	static LatLonAlt center(const LatLonAlt& s0, double trk, double radius, int dir);
	  /**
	   * center of turn
	   * @param s0 point on turn
	   * @param v0 velocity at point
	   * @param omega turn rate
	   * @return turn center
	   */
	static LatLonAlt center(const LatLonAlt& s0, const Velocity& v0, double omega);
	/**
	 * EXPERMENTAL
	 * Calculate when during the turn we will be closest to the given point.
	 * @param s0 turn start position
	 * @param v0 turn start velocity
	 * @param omega rate of turn (+ = right, - = left)
	 * @param x point of interest
	 * @param endTime time at which turn finishes.  If &le; 0, assume a full turn is allowed.
	 * @return time on turn when we are closest to the given point x (in seconds), or -1 if we are precisely at the turn's center
	 * This will be bounded by [0,endTime]
	 */
	static double closestTimeOnTurn(const LatLonAlt& s0, const Velocity& v0, double omega, const LatLonAlt& x, double endTime);
	/**
	 * EXPERMENTAL
	 * Calculate when during the turn we will be closest to the given point.
	 * @param s0 turn start position
	 * @param v0 turn start velocity
	 * @param R radius
	 * @param dir direction of turn
	 * @param x point of interest
	 * @param maxDist time at which turn finishes.  If &le; 0, assume a full turn is allowed.
	 * @return dist on turn when we are closest to the given point x, or -1 if we are precisely at the turn's center
	 * This will be bounded by [0,maxDist]
	 */
	static double closestDistOnTurn(const LatLonAlt& s0, const Velocity& v0, double R, int dir, const LatLonAlt& x, double maxDist);


	// ****************************** Ground Speed KINEMATIC CALCULATIONS *******************************

	/**
	 * Final 3D position after a constant GS acceleration for t seconds
	 *
	 * @param so3        current position
	 * @param vo3        current velocity
	 * @param a          acceleration,  i.e. a positive  or negative acceleration
	 * @param t          amount of time accelerating
	 * @return           final position
	 */
	static std::pair<LatLonAlt,Velocity> gsAccel(const LatLonAlt& so, const Velocity& vo,  double t, double a) ;


	/**
	 *  Position after t time units where there is first an acceleration or deceleration to the target
	 *  ground speed goalGs and then continuing at that speed for the remainder of the time, if any.
	 *
	 * @param so         current position
	 * @param vo         current velocity
	 * @param goalGs     the ground speed where the acceleration stops
	 * @param gsAccel    the ground speed acceleration (a positive value)
	 * @param t          time point of interest
	 * @return           Position-Velocity pair after time t
	 */
	static std::pair<LatLonAlt,Velocity> gsAccelUntil(const LatLonAlt& so, const Velocity& vo, double t, double goalGS, double gsAccel_d);

	/**
	 *  Position after t time units where there is first an acceleration or deceleration to the target
	 *  ground speed goalGs and then continuing at that speed for the remainder of the time, if any.
	 *
	 * @param svo        initial position and velocity
	 * @param goalGs     the ground speed where the acceleration stops
	 * @param gsAccel    the ground speed acceleration (a positive value)
	 * @param t          time point of interest
	 * @return           Position-Velocity pair after time t
	 */
	static std::pair<LatLonAlt,Velocity> gsAccelUntil(std::pair<LatLonAlt,Velocity> sv0, double t, double goalGs, double gsAccel);

	// ****************************** Vertical Speed KINEMATIC CALCULATIONS *******************************

	/**
	 * Position/Velocity after a constant vertical speed acceleration for t seconds
	 *
	 * @param so         current position
	 * @param vo         current velocity
	 * @param t          amount of time accelerating
	 * @param a          acceleration,  i.e. a positive  or negative acceleration
	 * @return           position/velocity at time t
	 */
	static std::pair<LatLonAlt,Velocity> vsAccel(const LatLonAlt& so, const Velocity& vo,  double t, double a) ;

	static std::pair<LatLonAlt,Velocity> vsAccel(std::pair<LatLonAlt,Velocity> svo,  double t, double a);
	/**
	 *  Position/Velocity after t time units where there is first an acceleration or deceleration to the target
	 *  vertical speed goalVs and then continuing at that speed for the remainder of the time, if any.
	 *
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param goalVs     vertical speed where the acceleration stops
	 * @param vsAccel    vertical speed acceleration (a positive value)
	 * @param t          time of the point of interest
	 * @return           Position after time t
	 */
	static std::pair<LatLonAlt,Velocity> vsAccelUntil(const LatLonAlt& so, const Velocity& vo, double t, double goalVs, double vsAccel);

	static std::pair<LatLonAlt,Velocity> vsAccelUntil(std::pair<LatLonAlt,Velocity> sv0, double t, double goalVs, double vsAccel);


	/** returns Pair that contains position and velocity at time t due to level out maneuver
	 *
	 * @param sv0        			current position and velocity vectors
	 * @param t          			time point of interest
	 * @param climbRate  			climb rate
	 * @param targetAlt  			target altitude
	 * @param accelUp         		first acceleration
	 * @param accelDown    			second acceleration
	 * @param allowClimbRateChange allows climbRate to change to initial velocity if it can help.
	 * @return
	 */
	static std::pair<LatLonAlt, Velocity> vsLevelOut(std::pair<LatLonAlt, Velocity> sv0, double t, double climbRate,
			double targetAlt, double accelUp, double accelDown, bool allowClimbRateChange);

	static std::pair<LatLonAlt, Velocity> vsLevelOut(std::pair<LatLonAlt, Velocity> sv0, double t, double climbRate,
			double targetAlt, double a, bool allowClimbRateChange) ;

	static std::pair<LatLonAlt, Velocity> vsLevelOut(std::pair<LatLonAlt, Velocity> sv0, double t, double climbRate,
			double targetAlt, double a) ;


private:

	/** returns Pair that contains position and velocity at time t due to level out maneuver based on vsLevelOutTimesAD
	 *
	 * @param sv0        			current position and velocity vectors
	 * @param t          			time point of interest
	 * @param climbRate  			climb rate
	 * @param targetAlt  			target altitude
	 * @param a1         			first acceleration
	 * @param a2         			second acceleration
	 * @param allowClimbRateChange allows climbRate to change to initial velocity if it can help.
	 * @return
	 */
	static std::pair<LatLonAlt, Velocity> vsLevelOutCalculation(std::pair<LatLonAlt,Velocity> sv0,
			                                   double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t);


};

}

#endif //KINEMATICS_LATLON_H_

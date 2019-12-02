/*
 * KinematicsPosition.h
 * 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef KINEMATICS_POSITION_H_
#define KINEMATICS_POSITION_H_

#include "Vect3.h"
#include "Velocity.h"
#include "Quad.h"
#include "Tuple5.h"
#include "KinematicsLatLon.h"
#include "Position.h"
#include "Triple.h"


namespace larcfm {

/**
 * This class contains versions of the Kinematics functions that have been lifted to deal with Position objects instead of Vect3 objects.
 *
 */
class KinematicsPosition {
public:

	static std::pair<Position,Velocity> linear(std::pair<Position,Velocity> p, double t);

	static std::pair<Position,Velocity> linear(const Position& so, const Velocity& vo, double t);


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turn(const Position& so, const Velocity& vo, double t, double R,  bool turnRight);


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turnOmega(const Position& so, const Velocity& vo, double t, double omega) ;

	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turnOmega(std::pair<Position,Velocity> pp, double t, double omega);


//		/**
//	 *  Position and velocity after t time units turning with rate omega
//	 * @param so  starting position
//	 * @param vo  initial velocity
//	 * @param t   time of turn [secs]
//	 * @param omega
//	 * @return Position and Velocity after t time
//	 */
//	static std::pair<Position,Velocity> turnOmegaAlt(const Position& so, const Velocity& vo, double t, double omega);
	
	/** Advance in turn starting at "s0" by distance "d"
	 *
	 * @param so        starting position
	 * @param center    center of turn
	 * @param dir       direction of turn
	 * @param d         distance to turn
	 * @return          position and velocity after turning distance "d"
	 */
	static std::pair<Position,Velocity> turnByDist2D(const Position& so, const Position& center, int dir, double d, double gsAt_d);

	/** 
	 * Advance in turn starting at <code>so</code> by distance <code>d</code> 
	 * 
	 * @param so        starting position
	 * @param center    center of turn
	 * @param dir       direction of turn
	 * @param d         distance to turn       (can be negative)
	 * @return          position after turning distance <code>d</code>
	 */
	static Position turnByDist2D(const Position& so, const Position& center, int dir, double d);

	/** 
	 * Position/Velocity after turning (does not compute altitude!!)
	 * 
	 * Note: will be used in a context where altitude is computing subsequently
	 * 
	 * @param so          starting position
	 * @param center      center of turn
	 * @param alpha       turn angle (signed: positive: turn right, negative: turn left);
	 * @return            Position after turning distance d
	 */
	static Position turnByAngle2D(const Position& so, const Position& center, double alpha);

	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param goalTrack the target track angle
	 * @param bankAngle the aircraft's bank angle
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turnUntil(const Position& so, const Velocity& vo, double t, double goalTrack, double bankAngle);

	static std::pair<Position,Velocity> turnUntil(std::pair<Position,Velocity> sv, double t, double goalTrack, double bankAngle);


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param goalTrack the target track angle
	 * @param bankAngle the aircraft's bank angle
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turnUntilTimeOmega(const Position& so, const Velocity& vo, double t, double turnTime, double omega);

	static std::pair<Position,Velocity> turnUntilTimeOmega(std::pair<Position,Velocity> sv, double t, double turnTime, double omega) ;

	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param goalTrack the target track angle
	 * @param bankAngle the aircraft's bank angle
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turnUntilTime(const Position& so, const Velocity& vo, double t, double turnTime, double R, bool turnRight) ;

	/**
	 *  Position and velocity after t time units accelerating vertically.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> gsAccel(const Position& so, const Velocity& vo, double t, double a);


	/**
	 *  Position and velocity after t time units accelerating horizontally.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   ground speed acceleration (or deceleration) (positive)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> gsAccelUntil(const Position& so, const Velocity& vo, double t, double goalGs, double a) ;


	/**
	 *  Position and velocity after t time units accelerating vertically.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> vsAccel(const Position& so, const Velocity& vo, double t, double a) ;

	/**
	 *  altitude and vertical speed after t time units accelerating vertically.
	 * @param so  starting position
	 * @param vos  initial vertical speed
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return    altitude and vs after time t
	 */
	static std::pair<double,double> vsAccelZonly(const Position& so, double voz, double t, double a) ;


	/**
	 *  Position and velocity after t time units accelerating vertically.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (a positive value)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> vsAccelUntil(const Position& so, const Velocity& vo, double t, double goalVs, double a) ;

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
	static std::pair<Position, Velocity> vsLevelOut(std::pair<Position, Velocity> sv0, double t, double climbRate,
			double targetAlt, double accelUp, double accelDown, bool allowClimbRateChange) ;

	static std::pair<Position, Velocity> vsLevelOut(std::pair<Position, Velocity> sv0, double t, double climbRate,
			double targetAlt, double a, bool allowClimbRateChange) ;

	static std::pair<Position, Velocity> vsLevelOut(std::pair<Position, Velocity> sv0, double t, double climbRate,
			double targetAlt, double a);


	   static Position centerFromRadius(const Position& bot, double signedRadius, double trkIn);


private:


};

}

#endif //KINEMATICS_POSITION_H_

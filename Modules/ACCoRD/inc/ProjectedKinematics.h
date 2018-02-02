/*
 * Kinematics.h
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PROJECTEDKINEMATICS_H_
#define PROJECTEDKINEMATICS_H_

#include "Position.h"
#include "Velocity.h"
#include "Kinematics.h"
#include "EuclideanProjection.h"
#include "Triple.h"
#include "Quad.h"


namespace larcfm {

/**
 * This class contains versions of the Kinematics functions that have been lifted to deal with Position objects instead of Vect3 objects.
 *
 */
class ProjectedKinematics {
public:
	/**
	 * Calculate the angle of a constant-radius turn from two points and the radius
	 * 
	 * @param s1
	 * @param s2
	 * @param R
	 * @return the turn angle
	 */
	static double turnAngle(Position s1, Position s2, double R);

  /**
   * Horizontal distance covered in a turn
   * 
   * @param s1
   * @param s2
   * @param R
   * @return the turn distance
   */
	static double turnDistance(Position s1, Position s2, double R);

  /**
   * Given two points on a turn and the velocity (direction) at the first point, determine the direction for the shortest turn going through the second point,
   * returning true if that relative direction is to the right
   * 
   * @param s1
   * @param v1
   * @param s2
   * @return true if clockwise turn
   */
	 static bool clockwise(Position s1, Velocity v1, Position s2);

//	  /**
//	   * Turn velocity at point s1, given addition position s2 on turn at relative time t
//	   * @param s1 position to take tangent
//	   * @param s2 additional position on turn
//	   * @param C center of circle of rotation
//	   * @param t time to get from s1 to s2 (negative if s1 after s2)
//	   * @return velocity at point s1
//	   */
	//static Velocity turnVelocity(Position s1, Position s2, Position C, double t);

		static double closestTimeOnTurn(const Position& so, const Velocity& v1, double omega, const Position& s2, const Position& x, double endTime);
		static double closestDistOnTurn(const Position& turnstart, const Velocity& v1, double R, int dir, const Position& center, const Position& x, double endDist);

	static std::pair<Position,Velocity> turn(const Position& so, const Velocity& vo, double t, double R, bool turnRight);
	static std::pair<Position,Velocity> gsAccel(const Position& so, const Velocity& vo, double t, double accel);

	static std::pair<Position,Velocity> gsAccelUntil(const Position& so, const Velocity& vo, double t, double goalGs, double a);
	static std::pair<Position,Velocity> vsAccel(const Position& so, const Velocity& vo, double t, double accel);

	static std::pair<Position,Velocity> vsAccelUntil(const Position& so, const Velocity& vo, double t, double goalVs,  double accel);

	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
	 *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	static std::pair<Position,Velocity> turnOmega(const Position& so, const Velocity& vo, double t, double omega);

	static std::pair<Position,Velocity> turnOmega(const Position& so, const Velocity& vo, double t, double omega, const EuclideanProjection& proj);


	/**
	  *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnUntil
	  *  and uses the projection defined in the static Projection class.
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


	static std::pair<Position,Velocity> linear(std::pair<Position,Velocity> p, double t);

	static std::pair<Position,Velocity> linear(const Position& so, const Velocity& p, double t);

	static  std::pair<Position,double> intersection(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi);


	static double timeOfintersection(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi);


	static Quad<Position,Velocity,double,int> directToPoint(const Position& so, const Velocity& vo, const Position& wp, double R);


	static Triple<Position,double,double> genDirectToVertex(const Position& sop, const Velocity& vo, const Position& wpp, double bankAngle, double timeBeforeTurn);

	static Triple<Position,Velocity,double> vsLevelOutFinal(const Position& so, const Velocity& vo, double climbRate, double targetAlt, double a);

	static double vsLevelOutTime(const Position& so, const Velocity& vo, double climbRate, double targetAlt, double a);

	static std::pair<Position,Velocity> vsLevelOut(const Position& so, const Velocity& vo, double t, double climbRate, double targetAlt, double a);


};

}

#endif //PROJECTEDKINEMATICS_H_

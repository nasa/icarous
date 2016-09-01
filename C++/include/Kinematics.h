/*
 * Kinematics.h
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef KINEMATICS_H_
#define KINEMATICS_H_

#include "Vect3.h"
//#include "Vect4.h"
#include "Velocity.h"
#include "Quad.h"
#include "Tuple5.h"
#include "StateVector.h"
#include "Triple.h"


namespace larcfm {

/**
 * A library of functions to aid the computation of the kinematics of an aircraft.  This
 * library is currently under development and is far from complete.  The majority of the functions
 * handle constant velocity turns and movement with constant ground speed acceleration.<p>
 * 
 * Unless otherwise noted, all kinematics function parameters are in internal units -- angles are in radians,
 * linear speeds are in m/s, distances are in meters, time is in seconds.
 * 
 */
class Kinematics {
public:


  /**
   * Calculates turn radius from ground speed and bank angle.  
   * @param speed  ground speed 
   * @param bank   bank angle (positive is clockwise looking out the nose of the aircraft), must be in (-pi/2,pi/2).
   * @param g      local gravitational acceleration (must be positive)
   * @return radius (always non-negative)
   */
	static double turnRadius(double speed, double bank, double g);

  /**
   * Calculates turn radius from ground speed and bank angle.  The function
   * assumes standard sea-level gravity, see Units.gn.
   * @param speed  ground speed 
   * @param bank   bank angle (positive is clockwise looking out the nose of the aircraft), must be in (-pi/2,pi/2).
   * @return radius (always non-negative)
   */
	static double turnRadius(double speed, double bank);

	static double turnRadiusByRate(double speed, double omega);

  /**
   * Calculates ground speed of the vehicle from radius R and the bank angle. Assumes sea-level gravity.  
   * @param R      radius, must be non-negative 
   * @param bank   bank angle, must be in (-pi/2,pi/2)
   * @return speed
   */

	static double speedOfTurn(double R, double bank);


  /**
   * Calculates turn rate (or track-rate) from ground speed and bank angle. Assumes 
   * sea-level gravity.  
   * @param speed  ground speed
   * @param bankAngle   bank angle, must be in (-pi/2,pi/2)
   * @return turn rate (ie. omega or track rate), positive turn rate is right hand turn. 
   */
	static double turnRate(double speed, double bankAngle);

  /** 
   * Calculates the bank angle used for a given turn radius and ground speed.  
   * Because this method does not have enough information, it always returns 
   * a positive bank angle (indicating a right hand turn).  Assumes 
   * sea-level gravity. If R <= 0.0, this returns 0. 
   * @param speed  ground speed
   * @param  R     radius
   * @return       bank angle (positive)
   */
	static double bankAngleRadius(double R, double speed);

	  /**
	   * Calculates the bank angle used for a given turn radius and ground speed.   Assumes
	   * sea-level gravity.
	   * @param speed ground speed (speed >= 0.0)
	   * @param  R     radius (R > 0.0)
	   * @return       bank angle (positive = turn right, negative = turn left)
	   */
	  static double bankAngleRadius(double speed, double R, bool turnRight);

  /**
   * Returns the calculated bank angle for a turn that has specified turnRate. Assumes 
   * sea-level gravity.
   * @param speed ground speed (speed >= 0.0)
   * @param turnRate (positive is a right turn)
   * @return bank angle (positive = turn right, negative = turn left)
   */
	static double bankAngle(double speed, double turnRate);

	  /**
	   * Find the <b>minimum</b> turn for the to reach the goal and returns the maxBank angle, with the correct sign to achieve that goal.
	   * Assumes sea-level gravity.
	   * @param track the current track
	   * @param goalTrack the goal track angle
	   * @param maxBank the maximum bank angle, must be in (0,pi/2)
	   * @return bank angle (positive = turn right, negative = turn left)
	   */
	  static double bankAngleGoal(double track, double goalTrack, double maxBank);

	  /**
	   * Calculates turn rate (or track-rate) for the <b>minimum</b> turn to the goal track angle. Assumes
	   * sea-level gravity.
	   * @param vo  the initial velocity
	   * @param goalTrack the goal track angle
	   * @param maxBank   the maximum bank angle, must be in (0,pi/2)
	   * @return turn rate (ie. omega or track rate), positive turn rate is right hand turn.
	   */
	  static double turnRateGoal(const Velocity& vo, double goalTrack, double maxBank);


  /**
   * Has the turn completed?  Specifically, is the currentTrack at least the targetTrack given that 
   * currentTrack is roughly moving in the direction indicated by the parameter turnRight.
   *  
   * @param currentTrack    initial track angle (radians clockwise from true north)
   * @param targetTrack     target  track angle
   * @param turnRight true iff only turn direction is to the right
   * @return true iff turn has passed Target
   */

	static bool turnDone(double currentTrack, double targetTrack, bool turnRight);

  /**
   * Returns the time it takes to achieve the goal track angle 
   * @param v0          initial velocity vector
   * @param goalTrack   target velocity track [rad]
   * @param maxBank     maximum bank angle, must be in (0,pi/2) [rad]
   * @param turnRight   true iff only turn direction is to the right
   * @return time to achieve turn
   */
	static double turnTime(const Velocity& v0, double goalTrack, double maxBank, bool turnRight);

  /**
   * Returns the time it takes to achieve the goal track when making the <b>minimum</b> turn
   * @param v0          initial velocity vector
   * @param goalTrack   target velocity track [rad]
   * @param maxBank     maximum bank angle, must be in (0,pi/2) [rad]
   * @return time to achieve turn
   */
	static double turnTime(const Velocity& v0, double goalTrack, double maxBank);

  /**
   * Returns the time it takes to turn the given angle (deltaTrack).  Depending on the signs of deltaTrack and bankAngle, 
   * this turn can be more than 180 degrees. 
   * @param groundSpeed ground speed of aircraft
   * @param deltaTrack  given angle of turn [rad]
   * @param bankAngle     bank angle (-pi/2,pi/2) [rad]
   * @return time to achieve turn
   */
	static double turnTime(double groundSpeed, double deltaTrack, double bankAngle);

	static double turnTime(double deltaTrack, double trackRate);

	/**
	 * Returns true if the minimal (i.e. less than 180 deg) turn to goalTrack is tp the right
	 * @param v0          initial velocity vector
	 * @param goalTrack   target velocity track [rad]
	 **/
	static bool turnRight(const Velocity& v0, double goalTrack);

//	/**
//	 * Position after t time units turning right or left with radius R
//	 * @param s0  starting position
//	 * @param v0  initial velocity
//	 * @param R   turn radius
//	 * @param t   time of turn
//	 * @param turnRight true iff only turn direction is to the right
//	 * @return Position after t time
//	 */
//	static Vect3 turnPos(const Vect3& s0, const Velocity& v0, double t, double R, bool turnRight);
//
//
//	/**
//	 * Velocity after t time units in direction "turnRight" with radius R
//	 * @param v0  initial velocity
//	 * @param R   turn radius
//	 * @param t   time of turn
//	 * @param turnRight true iff only turn direction is to the right
//	 * @return Velocity after t
//	 */
//	static Velocity turnVel(const Velocity& v0, double t, double R, bool turnRight);
//


	/**
	 * @param sv0 Pair  (initial position, initial velocity)
	 * @param t         time of travel
	 * @return          new position after traveling straight for t units of time
	 */
	static std::pair<Vect3,Velocity> linear(const std::pair<Vect3,Velocity>& sv0, double t);


	static std::pair<Vect3,Velocity> linear(Vect3 so, Velocity vo, double t);

	/**
	 * Position/Velocity after turning t time units according to track rate omega
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time of turn
	 * @param omega       rate of change of track, sign indicates direction
	 * @return Position/Velocity after t time
	 */
	static std::pair<Vect3,Velocity> turnOmega(const Vect3& s0, const Velocity& v0, double t, double omega);


	static Vect2 center(const Vect3& s0, const Velocity& v0, double omega);

	/**
	 * Position/Velocity after turning t time units according to track rate omega
	 * @param sv0         initial position and velocity
	 * @param t           time of turn
	 * @param omega       rate of change of track, sign indicates direction
	 * @return Position/Velocity after t time
	 */
	static std::pair<Vect3,Velocity> turnOmega(const std::pair<Vect3,Velocity>& sv0, double t, double omega);

	/**
	 * Position/Velocity after turning t time units right or left with bank angle bank
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time of turn
	 * @param R           turn radius
	 * @param turnRight   true iff only turn direction is to the right
	 * @return Position/Velocity after t time
	 */
	static std::pair<Vect3,Velocity> turn(const Vect3& s0, const Velocity& v0, double t, double R,  bool turnRight);

	/**
	 * Position/Velocity after turning t time units right or left with bank angle bank
	 * @param sv0         Pair (initial position, initial velocity)
	 * @param t           time of turn
	 * @param R           turn radius
	 * @param turnRight   true iff only turn direction is to the right
	 * @return Position/Velocity pair after t time
	 */
	static std::pair<Vect3,Velocity> turn(const std::pair<Vect3,Velocity>& sv0, double t, double R,  bool turnRight);

	/**
	 * Position/Velocity after turning t time with bank angle bank, direction of turn determined by sign of bank
	 * @param sv0         Pair (initial position, initial velocity)
	 * @param t           time of turn
	 * @param bank        bank angle  (-pi,pi)   (positive = right turn,  negative = left turn)
	 * @return Position/Velocity after t time
	 */
	static std::pair<Vect3,Velocity> turn(const Vect3& s0, const Velocity& v0, double t, double bank);

//
//	/**
//	 * 2D Position after t time units turning right or left with radius R
//	 * @param s0  starting position
//	 * @param v0  initial velocity
//	 * @param R   turn radius
//	 * @param t   time of turn
//	 * @param turnRight true iff only turn direction is to the right
//	 * @return 2D Position after t time
//	 */
//	static Vect2 turnPosition2D(const Vect2& s0, const Vect2& v0, double R, double t, bool turnRight);


//
//	/**
//	 *  Position after t time units turning in direction "turnRight"  until goalTrack is reached, after that
//	 *  continue in a straight line
//	 * @param so         starting position
//	 * @param vo         initial velocity
//	 * @param goalTrack  the track angle where the turn stops
//	 * @param bankAngle    the bank angle of the aircraft making the turn
//	 * @param t          time of turn [secs]
//	 * @param turnRight  true iff only turn direction is to the right
//	 * @return Position after time t
//	 */
//	static Vect3 turnUntilPosition(const Vect3& so, const Velocity& vo, double goalTrack, double bankAngle, double t, bool turnRight);
//
//
//
//	/**
//	 *  Position after t time units turning in direction "turnRight"  until goalTrack is reached, after that
//	 *  continue in a straight line
//	 * @param vo         initial velocity
//	 * @param goalTrack  the track angle where the turn stops
//	 * @param bankAngle    the bank angle of the aircraft making the turn
//	 * @param t          time of turn [secs]
//	 * @param turnRight  true iff only turn direction is to the right
//	 * @return Position after time t
//	 */
//	static Velocity turnUntilVelocity(const Velocity& vo, double goalTrack, double bankAngle, double t, bool turnRight);


	/**
	 *  Position/Velocity after t time units turning in *minimal* direction  until goalTrack is reached, after that
	 *  continue in a straight line
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param goalTrack  the track angle where the turn stops
	 * @param bankAngle    the bank angle of the aircraft making the turn
	 * @param t          time of turn [secs]
	 * @param turnRight  true iff only turn direction is to the right
	 * @return Position/Velocity after time t
	 */
	static std::pair<Vect3,Velocity> turnUntil(const Vect3& so, const Velocity& vo, double t, double goalTrack, double maxBank);

	/**
	 *  Position/Velocity after t time units turning in *minimal* direction  until goalTrack is reached, after that
	 *  continue in a straight line
	 * @param sv0        initial position and velocity
	 * @param t          time of turn [s]
	 * @param goalTrack  the track angle where the turn stops
	 * @param maxBank    the maximum bank angle
	 * @return Position/Velocity after time t
	 */
	static std::pair<Vect3,Velocity> turnUntil(const std::pair<Vect3,Velocity>& sv0, double t, double goalTrack, double maxBank);

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
	  static std::pair<Vect3,Velocity> turnUntilTimeRadius(std::pair<Vect3,Velocity> svo, double t, double turnTime, double R, bool turnRight);

	/**
	 *  Position/Velocity after t time units turning in direction "turnRight" for a total of turnTime secs, after that
	 *  continue in a straight line.  This function can make a turn greater than 180 deg
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param t          time point of interest
	 * @param turnTime   total time of turn [secs]
	 * @param R          turn radius (positive)
	 * @param turnRight  true iff only turn direction is to the right
	 * @return Position/Velocity after time t
	 */
	static std::pair<Vect3,Velocity> turnUntilTimeOmega(const Vect3& so, const Velocity& vo, double t, double turnTime, double omega);


	/**
	 *  Position/Velocity after t time units turning at the rate of "omega," after that
	 *  continue in a straight line.  This function can make a turn greater than 180 deg
	 * @param so         initial position and velocity
	 * @param t          time point of interest
	 * @param turnTime   total time of turn [secs]
	 * @param omega 	turn rate
	 * @return Position/Velocity after time t
	 */
	static std::pair<Vect3,Velocity> turnUntilTimeOmega(const std::pair<Vect3,Velocity>& svo, double t, double turnTime, double omega);




	/**
	 *  Position after turning to track goalTrack, assumes less than 180 degree turn
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param goalTrack  the track angle where the turn stops
	 * @param bankAngle    the bank angle of the aircraft making the turn
	 * @return Position after time t
	 */
	static Vect3 positionAfterTurn(const Vect3& so, const Velocity& vo, double goalTrack, double bankAngle);

	/**
	 * Calculate when during the turn we will be closest to the given point.
	 * @param s0 turn start position
	 * @param v0 turn start velocity
	 * @param omega rate of turn (+ = right, - = left)
	 * @param x point of interest
	 * @return time on turn when we are closest to the given point x (in seconds)
	 */
	static double closestTimeOnTurn(const Vect3& s0, const Velocity& v0, double omega, const Vect3& x);


	/**
	 * Given an initial position, initial velocity, and goal position (outside of the turn radius) calculate the end of turn and center of turn
	 * @param bot initial position (i.e. beginning of turn)
	 * @param v0 initial velocity
	 * @param goal goal position
	 * @param R turn radius
	 * @return end of turn, center of turn
	 */
	static std::pair<Vect2,Vect2> directTo(const Vect2& bot, const Vect2& v0, const Vect2& goal, double R);

	static Quad<Vect3,Velocity,double,int> directToPoint(const Vect3& so, const Velocity& vo, const Vect3& wp, double R);


	/*  Finds end of turn where aircraft is heading directly towards a specified waypoint wp
	 *  @param so  current position
	 *  @param vo  current velocity
	 *  @param wp  the aircraft is turning to point to this point
	 *  @param R   turn radius
	 *
	 *  returns a triple: (end of turn, velocity at end of turn, time to reach end of turn)
	 *  If no result is possible (for example the point lies within the given turn radius), this will return a negative time.
	 */
	static Triple<Vect3,double,double> genDirectToVertex(const Vect3& so, const Velocity& vo, const Vect3& wp, double bankAngle, double timeBeforeTurn) ;


	/**
	 * find center of turn determined by line (so,vo) with radius R and direction dir
	 * @param so position
	 * @param vo velocity
	 * @param R radius of turn
	 * @param dir direction: 1 = right, -1 = left
	 * @return two dimensional position of turn 
	 */
	static Vect2 centerOfTurn(const Vect2& so, const Vect2& vo, double R, int dir);

	/**
	 * find center of turn determined by line (so,vo) with bankAngle and direction (turnRight)
	 * @param so position
	 * @param vo velocity
	 * @param bankAngle bank angle
	 * @param turnRight right turn (left otherwise)
	 * @return two dimensional position of turn 
	 */
	static Vect2 centerOfTurn(const Vect2& so, const Vect2& vo, double bankAngle, bool turnRight);


	/** Test for LoS(D,H) between two aircraft when only ownship turns, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param bankAngleOwn       the bank angle of the ownship
	 * @param turnRightOwn     the turn direction of ownship
	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static bool testLoSTrk(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
			double bankAngleOwn, bool turnRightOwn, double stopTime, double D, double H);


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
	static Vect3 gsAccelPos(const Vect3& so3, const Velocity& vo3, double t, double a);

	/**
	 * Position/Velocity after a constant GS acceleration for t seconds
	 *
	 * @param so3        current position
	 * @param vo3        current velocity
	 * @param t          amount of time accelerating
	 * @param a          acceleration,  i.e. a positive  or negative acceleration
	 * @return           position/velocity at time t
	 */
	static std::pair<Vect3,Velocity> gsAccel(const Vect3& so3, const Velocity& vo3,  double t, double a);

	/**
	 * returns time required to accelerate to target ground speed GoalGs
	 *
	 * @param vo         current velocity
	 * @param goalGs     ground speed where the acceleration stops
	 * @param gsAccel    ground speed acceleration (a positive value)
	 * @return           acceleration time
	 */
	static double gsAccelTime(const Velocity& vo,double goalGs, double gsAccel);


	static Triple<Vect3,Velocity,double> gsAccelGoal(const Vect3& so, const Velocity& vo, double goalGs, double gsAccel);


	/**
	 *  Position after t time units where there is first an acceleration or deceleration to the target
	 *  ground speed goalGS and then continuing at that speed for the remainder of the time, if any.
	 *
	 * @param so         starting position
	 * @param vo         initial velocity
	 * @param goalGS     the ground speed where the acceleration stops
	 * @param gsAccel    the ground speed acceleration (a positive value)
	 * @param t          time of acceleration [secs]
	 * @return           Position after time t
	 */
        static std::pair<Vect3,Velocity> gsAccelUntilRWB(const Vect3& so3, const Velocity& vo3, double t, double goalGS, double gsAccel);
	static std::pair<Vect3,Velocity> gsAccelUntil(const Vect3& so3, const Velocity& vo3, double t, double goalGS, double gsAccel);
	static std::pair<Vect3,Velocity> gsAccelUntil(const std::pair<Vect3,Velocity>& sv0, double t, double goalGS, double gsAccel);


	/**
	 * Compute the goal ground speed and time needed to accelerate in order to reach a point at a given required time of arrival.
	 * @param gsIn Current ground speed (m/s)
	 * @param dist Current horizontal distance to goal point (m)
	 * @param rta (relative) required time of arrival (s)
	 * @param gsAccel maximum ground speed acceleration or deceleration (positive, m/s^2)
	 * @return the goal ground speed and acceleration duration needed in order to cover the given distance in the given time.  The time will be negative if the rta is not attainable.
	 */
	static std::pair<double,double> gsAccelToRTA(double gsIn, double dist, double rta, double gsAccel);


	static std::pair<double,double> gsAccelToDist(double gsIn, double dist, double gsAccel);


	/** Test for LoS(D,H) between two aircraft when only ownship gs accelerates, compute trajectories up to time stopTime
	 *
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param gsAccelOwn    ground speed acceleration of the ownship
	 * @param stopTime         the duration of the turns
	 * @return                 minimum distance data packed in a Vect4
	 */
	static bool testLoSGs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
			double gsAccelOwn, double stopTime, double D, double H);




	// ****************************** Vertical Speed KINEMATIC CALCULATIONS *******************************

	/**
	 * Return the elevation angle (alternatively the negative glide-slope angle) for a climb (descent)
	 * @return elevation angle [radians]
	 */
	static double elevationAngle(Velocity v);


	/**
	 * Final 3D position after a constant VS acceleration for t seconds
	 *
	 * @param so3      current position
	 * @param vo3      current velocity
	 * @param a        acceleration,  i.e. a positive  or negative acceleration
	 * @param t        amount of time accelerating
	 * @return         final position
	 */
	static Vect3 vsAccelPos(const Vect3& so3, const Velocity& vo3, double t, double a);


	/**
	 * Position/Velocity after a constant vertical speed acceleration for t seconds
	 *
	 * @param so3        current position
	 * @param vo3        current velocity
	 * @param t          amount of time accelerating
	 * @param a          acceleration,  i.e. a positive  or negative acceleration
	 * @return           position/velocity at time t
	 */
	static std::pair<Vect3,Velocity> vsAccel(const Vect3& so3, const Velocity& vo3,  double t, double a);

	static std::pair<Vect3,Velocity> vsAccel(const std::pair<Vect3,Velocity>& sv0,  double t, double a);

	/**
	 * returns time required to vertically accelerate to target GoalVS
	 *
	 * @param vs        current vertical speed
	 * @param goalVs     vertical speed where the acceleration stops
	 * @param vsAccel    vertical speed acceleration (a positive value)
	 * @return           acceleration time
	 */
	static double vsAccelTime(double vs, double goalVs, double vsAccel);

	/**
	 * returns time required to vertically accelerate to target GoalVS
	 *
	 * @param vo        current velocity
	 * @param goalVs     vertical speed where the acceleration stops
	 * @param vsAccel    vertical speed acceleration (a positive value)
	 * @return           acceleration time
	 */
	static double vsAccelTime(const Velocity& vo, double goalVs, double vsAccel);


	static Triple<Vect3,Velocity,double> vsAccelGoal(const Vect3& so, const Velocity& vo, double goalVs, double vsAccel);



//	/**
//	 *  Position after t time units where there is first an acceleration or deceleration to the target
//	 *  vertical speed goalVs and then continuing at that speed for the remainder of the time, if any.
//	 *
//	 * @param so         starting position
//	 * @param vo         initial velocity
//	 * @param goalVs     vertical speed where the acceleration stops
//	 * @param vsAccel    vertical speed acceleration (a positive value)
//	 * @param t          time of acceleration [secs]
//	 * @return           Position after time t
//	 */
//     //	static Vect3 vsAccelUntilPos(const Vect3& so, const Velocity& vo, double t, double goalVs, double vsAccel);


	/**
	 *  Position after t time units where there is first an acceleration or deceleration to the target
	 *  vertical speed goalVs and then continuing at that speed for the remainder of the time, if any.
	 *
	 * @param vo         initial velocity
	 * @param goalVs     the vertical speed where the acceleration stops
	 * @param vsAccel    the vertical speed acceleration (a positive value)
	 * @param t          time of acceleration [secs]
	 * @return           Position after time t
	 */
	//static Velocity vsAccelUntilVel(const Velocity& vo, double t, double goalVs, double vsAccel);

	static std::pair<Vect3,Velocity> vsAccelUntil(const Vect3& so, const Velocity& vo, double t, double goalVs, double vsAccel);
	static std::pair<Vect3,Velocity> vsAccelUntil(const std::pair<Vect3,Velocity>& sv0, double t, double goalVs, double vsAccel);

	/** Test for LoS(D,H) between two aircraft when only ownship gs accelerates, compute trajectories up to time stopTime
	 * 
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param vsAccelOwn    ground speed acceleration of the ownship
	 * @param stopTime         the duration of the turns
	 * @param D	 horizontal distance
	 * @param H  vertical distance
	 * @return                 minimum distance data packed in a Vect4
	 */
	static bool testLoSVs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
			double vsAccelOwn, double stopTime, double D, double H);

//	static double vsLevelOutTimeRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateReduction);
//
//	/** Computes time for the vsLevelOut method,
//	 *  Note: if T2 < T1, there is no constant vertical speed phase, If T1 < 0, target altitude is not achieveable
//	 *
//	 * @param sv0          initial position and velocity
//	 * @param t            time point
//	 * @param climbRate    desired vertical speed for the climb/descent (positive), sign calculated in code
//	 * @param targetAlt    target altitude
//	 * @param a            maximum acceleration (positive), sign calculated in code
//	 * @param allowClimbRateReduction   if true, the climbRate can be reduced, otherwise T1 might be set to -1, which
//	 *                                  indicates failure
//	 * @return <T1 = end of first accel ,T2 = end of constant vertical speed phase, T3 = end of deceleration, climbRate'>
//	 */
//	static Quad<double,double,double,double> vsLevelOutTimesRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt,
//			double a1, double a2, bool allowClimbRateIncrease, bool allowClimbRateDecrease);
//
//	static Quad<double,double,double,double> vsLevelOutTimesRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt,
//			       double a, bool allowClimbRateChange);
//
//	static Quad<double,double,double,double> vsLevelOutTimesRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt,
//			       double a);
//
//	static std::pair<Vect3, Velocity> vsLevelOutCalculationRWB(const std::pair<Vect3,Velocity>& sv0, double t,  double T1, double T2,
//			double T3, double climbRate, double targetAlt, double a1, double a2);
//
//	/** Generates trajectory as a function of time t, for a climb and level out.   The specified climb rate may not be
//	 *  achievable if the level-out altitude is not much greter than the current altitude.
//	 *
//	 * @param sv0          initial position and velocity
//	 * @param t            time point
//	 * @param climbRate    desired vertical speed for the climb/descent (positive), sign calculated in code
//	 * @param targetAlt    target altitude
//	 * @param a            maximum acceleration (positive), sign calculated in code
//	 * @return             position, velocity at time t
//	 */
//	static std::pair<Vect3,Velocity> vsLevelOutRWB(const std::pair<Vect3,Velocity>& sv0, double t, double climbRate, double targetAlt, double a, bool allowClimbRateReduction);
//
//
//	static StateVector vsLevelOutFinalRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateReduction);
//
//	static StateVector vsLevelOutFinalRWB(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a);
//
//
//	static double vsLevelOutClimbRateRWB(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
//			double a1, double a2, bool allowClimbRateChange);


	/** Returns a statevector that holds position, velocity and relative time at final level out position
	 *
	 * @param sv0
	 * @param climbRate
	 * @param targetAlt
	 * @param a
	 * @param allowClimbRateChange
	 * @return
	 */
	static StateVector vsLevelOutFinal(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateChange);

    static StateVector vsLevelOutFinal(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a);

	static bool overShoot(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt, double accelup,
			                       double acceldown, bool allowClimbRateChange);


//	/**
//	 * Returns true if time t is within the constant velocity segment of the climb
//	 * All values are in internal units
//	 */
//	static bool inConstantClimb(double sz, double vz, double t, double climbRate, double targetAlt, double a);


	static Tuple5<double,double,double,double,double> vsLevelOutTimes(double s0z, double v0z, double climbRate, double targetAlt,
		     double accelup, double acceldown, bool allowClimbRateChange);


	static Tuple5<double,double,double,double,double> vsLevelOutTimes(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
		     double accelup, double acceldown, bool allowClimbRateChange);

	static Tuple5<double,double,double,double,double> vsLevelOutTimes(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
		     double a, bool allowClimbRateChange);

	static Tuple5<double,double,double,double,double> vsLevelOutTimes(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt, double a);


	static double vsLevelOutClimbRate(const std::pair<Vect3, Velocity>& svo, double climbRate, double targetAlt,
			double accelup, double acceldown, bool allowClimbRateChange);

	static double vsLevelOutTime(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a, bool allowClimbRateChange) ;


	static double vsLevelOutTime(const std::pair<Vect3,Velocity>& sv0, double climbRate, double targetAlt, double a);

	static std::pair<double, double> vsLevelOutCalc(double soz, double voz, double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t);

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
	static std::pair<Vect3, Velocity> vsLevelOutCalculation(const std::pair<Vect3,Velocity>& sv0,
			                              double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t);

	static std::pair<Vect3, Velocity> vsLevelOut(const std::pair<Vect3, Velocity>& sv0, double t, double climbRate,
			                            double targetAlt, double accelUp, double accelDown, bool allowClimbRateChange);

	static std::pair<Vect3, Velocity> vsLevelOut(const std::pair<Vect3, Velocity>& sv0, double t, double climbRate,
            double targetAlt, double a, bool allowClimbRateChange);

	static std::pair<Vect3, Velocity> vsLevelOut(const std::pair<Vect3, Velocity>& sv0, double t, double climbRate,
            double targetAlt, double a);



	// ******************************* Other **************************

	/** time of closest approach, if parallel return Double.MAX_VALUE
	 * @param s    relative position of aircraft
	 * @param vo   velocity of ownship
	 * @param vi   velocity of traffic
	 * @return     time of closest approach  (can be negative)
	 */
	static double tau(const Vect3& s, const Vect3& vo, const Vect3& vi);

	/**
	 * distance at time of closest approach
	 * @param s    relative position of aircraft
	 * @param vo   velocity of ownship
	 * @param vi   velocity of traffic
	 * @param futureOnly  if true then in divergent cases use distance now
	 * @return     distance at time of closest approach
	 */

	static double distAtTau(const Vect3& s, const Vect3& vo, const Vect3& vi, bool futureOnly);

private:

 static double dirSign(bool turnRight);

//
//	static double antiDer1(double voz, double t, double a);
//	static double antiDer2(double climbRate, double t, double a);
//	static double antiDer3(double climbRate, double t, double a);

	static std::pair<Vect3, Velocity> vsLevelOutCalculation(const std::pair<Vect3,Velocity>& sv0, double t,
         double T1, double T2, double T3, double climbRate, double targetAlt, double a1, double a2);



//static double V1(double voz, double a1, double t);
//
//static double S1(double voz, double a1, double t);
//
//static double T3(double voz, double a1) ;
//
//static double S3(double voz, double a1);

/** Helper function for vsLevelOutTimesAD.
 *  Note: This could be integrated into the function vsLevelOutTimesAD as a recursive call if desired.
 *
 * @param s0z          initial vertical position
 * @param v0z		   initial vertical velocity
 * @param climbRate    desired vertical speed for the climb/descent (positive), sign calculated in code
 * @param targetAlt    target altitude
 * @param accelup      maximum positive acceleration
 * @param acceldown    maximum negative acceleration
 * @param allowClimbRateChange	if true, if the current velocity is of greater magnitude than the specified climb rate,
 * 										then continue at the current velocity (prioritize achieving the desired altitude).
 * 										If false, first achieve the goal climb rate (prioritize achieving the indicated vs)
 *
 *
 * @return <T1 = end of first accel ,T2 = end of constant vertical speed phase, T3 = end of deceleration, a1 = acceleration for phase 1, a2 =acceleration for phase 2>
 */
static Tuple5<double,double,double,double,double> vsLevelOutTimesBase(double s0z, double v0z, double climbRate, double targetAlt,
		     double accelup, double acceldown, bool allowClimbRateChange) ;




};

}

#endif //KINEMATICS_H_

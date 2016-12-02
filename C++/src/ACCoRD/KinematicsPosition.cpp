/* KinematicsPosition
 *
 * Authors:  Ricky Butler              NASA Langley Research Center
 *           George Hagen              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
  *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicsPosition.h"
#include "Quad.h"
#include "Tuple5.h"
#include "format.h"
#include <cmath>
#include <float.h>


namespace larcfm {

	std::pair<Position,Velocity> KinematicsPosition::linear(std::pair<Position,Velocity> p, double t) {
		return linear(p.first, p.second, t);
	}

	std::pair<Position,Velocity> KinematicsPosition::linear(const Position& so ,const Velocity& vo, double t) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::linear(so.lla(),vo,t);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::linear(so.point(),vo,t);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}       
	}


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.  
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::turn(const Position& so, const Velocity& vo, double t, double R,  bool turnRight) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turn(so.lla(),vo,t,R,turnRight);
			return std::pair<Position,Velocity>(Position(resp.first),resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::turn(so.point(),vo,t,R,turnRight);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}
	}


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::turnOmega(const Position& so, const Velocity& vo, double t, double omega) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turnOmega(so.lla(),vo,t,omega);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::turnOmega(so.point(),vo,t,omega);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}
	}

  std::pair<Position,Velocity> KinematicsPosition::turnOmegaAlt(const Position& so, const Velocity& vo, double t, double omega) {
		if (so.isLatLon()) {
		  std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turnOmegaAlt(so.lla(),vo,t,omega);
		  return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
		  std::pair<Vect3,Velocity> resp = Kinematics::turnOmega(so.point(),vo,t,omega);
		  return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}
	}
  
	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::turnOmega(std::pair<Position,Velocity> pp, double t, double omega) {
		Position so = pp.first;
		Velocity vo = pp.second;
		return turnOmega(so,vo,t,omega);
	}


	/**
	 *  Position and velocity after turning a distance d
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::turnByDist(const Position& so, const Position& center, int dir, double d, double gsAt_d) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turnByDist(so.lla(), center.lla(), dir, d, gsAt_d);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::turnByDist(so.point(), center.point(), dir, d, gsAt_d);
			return  std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}
	}



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
	std::pair<Position,Velocity> KinematicsPosition::turnUntil(const Position& so, const Velocity& vo, double t, double goalTrack, double bankAngle) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turnUntil(so.lla(),vo,t,goalTrack,bankAngle);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::turnUntil(so.point(),vo,t,goalTrack,bankAngle);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}
	}

	std::pair<Position,Velocity> KinematicsPosition::turnUntil(std::pair<Position,Velocity> sv, double t, double goalTrack, double bankAngle) {
		return turnUntil(sv.first, sv.second,t, goalTrack, bankAngle);
	}


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
	std::pair<Position,Velocity> KinematicsPosition::turnUntilTimeOmega(const Position& so, const Velocity& vo, double t, double turnTime, double omega) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turnUntilTimeOmega(so.lla(),vo,t,turnTime, omega);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::turnUntilTimeOmega(so.point(),vo,t,turnTime,omega);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}       
	}

	std::pair<Position,Velocity> KinematicsPosition::turnUntilTimeOmega(std::pair<Position,Velocity> sv, double t, double turnTime, double omega) {
		return turnUntilTimeOmega(sv.first, sv.second,t, turnTime, omega);
	}


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
	std::pair<Position,Velocity> KinematicsPosition::turnUntilTime(const Position& so, const Velocity& vo, double t, double turnTime, double R, bool turnRight) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::turnUntilTimeRadius(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,turnTime,R,turnRight);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::turnUntilTimeRadius(std::pair<Vect3,Velocity>(so.point(), vo),t,turnTime,R,turnRight);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}       
	}

	/**
	 *  Position and velocity after t time units accelerating vertically. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::gsAccel(const Position& so, const Velocity& vo, double t, double a) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::gsAccel(so.lla(),vo,t,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::gsAccel(so.point(),vo,t,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}


	/**
	 *  Position and velocity after t time units accelerating horizontally. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   ground speed acceleration (or deceleration) (positive)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::gsAccelUntil(const Position& so, const Velocity& vo, double t, double goalGs, double a) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::gsAccelUntil(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,goalGs,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::gsAccelUntil(std::pair<Vect3,Velocity>(so.point(), vo),t,goalGs,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}


	/**
	 *  Position and velocity after t time units accelerating vertically. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::vsAccel(const Position& so, const Velocity& vo, double t, double a) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::vsAccel(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::vsAccel(std::pair<Vect3,Velocity>(so.point(), vo),t,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}


	/**
	 *  Position and velocity after t time units accelerating vertically.
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	std::pair<double,double> KinematicsPosition::vsAccelZonly(const Position& so, double voz, double t, double a) {
		double nsz = so.alt() + voz*t + 0.5*a*t*t;
		double nvz = voz + a*t;
		return std::pair<double,double>(nsz,nvz);

	}


	/**
	 *  Position and velocity after t time units accelerating vertically. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (a positive value)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	std::pair<Position,Velocity> KinematicsPosition::vsAccelUntil(const Position& so, const Velocity& vo, double t, double goalVs, double a) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::vsAccelUntil(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,goalVs,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::vsAccelUntil(std::pair<Vect3,Velocity>(so.point(), vo),t,goalVs,a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}

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
	std::pair<Position, Velocity> KinematicsPosition::vsLevelOut(std::pair<Position, Velocity> sv0, double t, double climbRate,
			double targetAlt, double accelUp, double accelDown, bool allowClimbRateChange) {
		Position so = sv0.first;
		Velocity vo = sv0.second;
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::vsLevelOut(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,
					climbRate, targetAlt, accelUp, accelDown,  allowClimbRateChange);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::vsLevelOut(std::pair<Vect3,Velocity>(so.point(), vo),t,
					climbRate, targetAlt, accelUp, accelDown,  allowClimbRateChange);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}

	std::pair<Position, Velocity> KinematicsPosition::vsLevelOut(std::pair<Position, Velocity> sv0, double t, double climbRate,
			double targetAlt, double a, bool allowClimbRateChange) {
		Position so = sv0.first;
		Velocity vo = sv0.second;
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::vsLevelOut(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,
					climbRate, targetAlt, a,  allowClimbRateChange);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::vsLevelOut(std::pair<Vect3,Velocity>(so.point(), vo),t,
					climbRate, targetAlt, a,  allowClimbRateChange);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}

	std::pair<Position, Velocity> KinematicsPosition::vsLevelOut(std::pair<Position, Velocity> sv0, double t, double climbRate,
			double targetAlt, double a) {
		Position so = sv0.first;
		Velocity vo = sv0.second;
		if (so.isLatLon()) {
			std::pair<LatLonAlt,Velocity> resp = KinematicsLatLon::vsLevelOut(std::pair<LatLonAlt,Velocity>(so.lla(),vo),t,
					climbRate, targetAlt, a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		} else {
			std::pair<Vect3,Velocity> resp = Kinematics::vsLevelOut(std::pair<Vect3,Velocity>(so.point(), vo),t,
					climbRate, targetAlt, a);
			return std::pair<Position,Velocity>(Position(resp.first), resp.second);
		}          
	}




}

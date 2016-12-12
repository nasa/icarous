/*
 * KinematicsPosition.java 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;


import java.util.ArrayList;

/**
 * This class contains versions of the Kinematics functions that have been lifted to deal with Position objects instead of Vect3 objects.
 *
 */
public final class KinematicsPosition {

	public static Pair<Position,Velocity> linear(Pair<Position,Velocity> p, double t) {
		return linear(p.first, p.second, t);
	}

	public static Pair<Position,Velocity> linear(Position so ,Velocity vo, double t) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.linear(so.lla(),vo,t);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.linear(so.point(),vo,t);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}       
	}

	 public static Position center(Position s0, double trk, double radius, int dir) {
		  if (s0.isLatLon()) {
			  return new Position(KinematicsLatLon.center(s0.lla(), trk, radius, dir));
		  } else {
			  return new Position(new Vect3(Kinematics.centerOfTurn(s0.vect2(), Vect2.mkTrkGs(trk, 100), radius, dir), s0.z()));
		  }
	  }  

	  public static Position center(Position s0, Velocity v0, double omega) {
		  if (s0.isLatLon()) {
			  return new Position(KinematicsLatLon.center(s0.lla(), v0, omega));
		  } else {
			  return new Position(new Vect3(Kinematics.center(s0.point(), v0, omega), s0.z()));
		  }
	  } 	
	
//
//	LatLonAlt KinematicsLatLon::center(const LatLonAlt& s0, const Velocity& v0, double omega) {
//		  double v = v0.gs();
//	    double R = v/omega;
//	    return center(s0, v0.trk(), R, Util::sign(omega));
//	}
	

	/**
	 * Position and velocity after t time units turning in direction "dir" with radius R. 
	 *  
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param R   turn radius
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> turn(Position so, Velocity vo, double t, double R,  boolean turnRight) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turn(so.lla(),vo,t,R,turnRight);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turn(so.point(),vo,t,R,turnRight);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);  
		}
	}


	/**

	 * Position and velocity after t time units turning with rate omega 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param t   time of turn [secs]
	 * @param omega
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> turnOmega(Position so, Velocity vo, double t, double omega) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnOmega(so.lla(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnOmega(so.point(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}
	}

	//	public static Pair<Position,Velocity> turnRadius(Position so, Velocity vo,  double t, double signedRadius) {
	//		if (so.isLatLon()) {
	//			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnRadius(so.lla(),vo, t, signedRadius);
	//			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
	//		} else {
	//			Pair<Vect3,Velocity> resp = Kinematics.turnRadius(so.point(),vo, t, signedRadius);
	//			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
	//		}
	//	}



	/**
	 *  Position and velocity after t time units turning with rate omega
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param t   time of turn [secs]
	 * @param omega
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> turnOmegaAlt(Position so, Velocity vo, double t, double omega) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnOmegaAlt(so.lla(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnOmega(so.point(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}
	}

	//	/**  *** EXPERIMENTAL ***
	//	 *  Position and velocity after turing a distance d
	//	 * @param so  starting position
	//	 * @param vo  initial velocity
	//	 * @param R   turn radius
	//	 * @param t   time of turn [secs]
	//	 * @param turnRight true iff only turn direction is to the right
	//	 * @return Position and Velocity after t time
	//	 */
	//	public static Pair<Position,Velocity> turnByDist(Position so, Velocity vo, double R, double d) {
	//		if (so.isLatLon()) {
	//			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnByDist(so.lla(), vo, R, d);
	//			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
	//		} else {
	//			Pair<Vect3,Velocity> resp = Kinematics.turnByDist(so.point(), vo, R, d);
	//			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
	//		}
	//	}

	
	/** Position and velocity after turning a distance d (does not compute altitude)
	 *  
	 * @param so        starting position
	 * @param center    center of turn 
	 * @param dir       direction of turn +/- 1
	 * @param d         path distance from so
	 * @param gsAt_d    ground speed at end of turn, used to update velocity vector at end (done for efficiency)
	 * @return          Position and velocity after turning a distance d
	 */
	public static Pair<Position,Velocity> turnByDist(Position so, Position center, int dir, double d, double gsAt_d) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnByDist(so.lla(), center.lla(), dir, d, gsAt_d);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnByDist(so.point(), center.point(), dir, d, gsAt_d);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}
	}

	/**
	 *  Position and velocity after turning at rate omega for t secs. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param t   time of turn [secs]
	 * @param omega
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> turnOmega(Pair<Position,Velocity> pp, double t, double omega) {
		Position so = pp.first;
		Velocity vo = pp.second;
		return turnOmega(so,vo,t,omega);
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
	public static Pair<Position,Velocity> turnUntil(Position so, Velocity vo, double t, double goalTrack, double bankAngle) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnUntil(so.lla(),vo,t,goalTrack,bankAngle);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnUntil(so.point(),vo,t,goalTrack,bankAngle);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}
	}

	public static Pair<Position,Velocity> turnUntil(Pair<Position,Velocity> sv, double t, double goalTrack, double bankAngle) {
		return turnUntil(sv.first, sv.second,t, goalTrack, bankAngle);
	}


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.  
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> turnUntilTimeOmega(Position so, Velocity vo, double t, double turnTime, double omega) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnUntilTimeOmega(so.lla(),vo,t,turnTime, omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnUntilTimeOmega(so.point(),vo,t,turnTime,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}       
	}

	public static Pair<Position,Velocity> turnUntilTimeOmega(Pair<Position,Velocity> sv, double t, double turnTime, double omega) {
		return turnUntilTimeOmega(sv.first, sv.second,t, turnTime, omega);
	}


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param t   time of turn [secs]
	 * @param turnTime
	 * @param R
	 * @param turnRight true iff only turn direction is to the right
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> turnUntilTime(Position so, Velocity vo, double t, double turnTime, double R, boolean turnRight) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnUntilTimeRadius(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,turnTime,R,turnRight);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnUntilTimeRadius(new Pair<Vect3,Velocity>(so.point(), vo),t,turnTime,R,turnRight);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}       
	}


	/**
	 *  Return time along turn where x is closest to.
	 *  This returns -1 if x is at the turn's center
	 */
	public static double closestTimeOnTurn(Position turnstart, Velocity v1, double omega, Position center, Position x, double endTime) {
		Vect3 s3 = turnstart.point();
		Vect3 x3 = x.point();
		if (turnstart.isLatLon()) {
			//		      EuclideanProjection proj = Projection.createProjection(center.lla().zeroAlt());
			//		      s3 = proj.project(turnstart); 
			//		      x3 = proj.project(x); 
			//		      v1 = proj.projectVelocity(turnstart, v1);
			return KinematicsLatLon.closestTimeOnTurn(turnstart.lla(), v1, omega, x.lla(), endTime);
		} 
		return Kinematics.closestTimeOnTurn(s3, v1, omega, x3, endTime);
	}


	public static double closestDistOnTurn(Position turnstart, Velocity v1, double R, int dir, Position center, Position x, double endDist) {
		Vect3 s3 = turnstart.point();
		Vect3 x3 = x.point();
		if (turnstart.isLatLon()) {
			return KinematicsLatLon.closestDistOnTurn(turnstart.lla(), v1, R, dir, x.lla(), endDist);
		} else {
			return Kinematics.closestDistOnTurn(s3, v1, R, dir, x3, endDist);
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
	public static Pair<Position,Velocity> gsAccel(Position so, Velocity vo, double t, double a) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.gsAccel(so.lla(),vo,t,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.gsAccel(so.point(),vo,t,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}          
	}

	//	public static Pair<Position,Velocity> gsAccelDist(Position os, Velocity vo, double d, double a) {
	//		
	//	}


	/**
	 *  Position and velocity after t time units accelerating horizontally. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   ground speed acceleration (or deceleration) (positive)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> gsAccelUntil(Position so, Velocity vo, double t, double goalGs, double a) {    
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.gsAccelUntil(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,goalGs,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.gsAccelUntil(new Pair<Vect3,Velocity>(so.point(), vo),t,goalGs,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
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
	public static Pair<Position,Velocity> vsAccel(Position so, Velocity vo, double t, double a) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.vsAccel(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.vsAccel(new Pair<Vect3,Velocity>(so.point(), vo),t,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}          
	}


	/**
	 * Altitude and vertical speed after t time units accelerating vertically.
	 * @param so  starting position
	 * @param voz  initial vertical speed
	 * @param a   vertical speed acceleration (or deceleration) (signed)
	 * @param t   time of turn [secs]
	 * @return alt and vs after t secs
	 */
	public static Pair<Double,Double> vsAccelZonly(Position so, double voz, double t, double a) {
		double nsz = so.alt() + voz*t + 0.5*a*t*t;
		double nvz = voz + a*t;
		return new Pair<Double,Double>(nsz,nvz);
	}



	/**
	 *  Position and velocity after t time units accelerating vertically. 
	 * @param so  starting position
	 * @param vo  initial velocity
	 * @param a   vertical speed acceleration (a positive value)
	 * @param t   time of turn [secs]
	 * @return Position and Velocity after t time
	 */
	public static Pair<Position,Velocity> vsAccelUntil(Position so, Velocity vo, double t, double goalVs, double a) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.vsAccelUntil(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,goalVs,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.vsAccelUntil(new Pair<Vect3,Velocity>(so.point(), vo),t,goalVs,a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
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
	public static Pair<Position, Velocity> vsLevelOut(Pair<Position, Velocity> sv0, double t, double climbRate, 
			double targetAlt, double accelUp, double accelDown, boolean allowClimbRateChange) {
		Position so = sv0.first;
		Velocity vo = sv0.second;
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.vsLevelOut(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,
					climbRate, targetAlt, accelUp, accelDown,  allowClimbRateChange);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.vsLevelOut(new Pair<Vect3,Velocity>(so.point(), vo),t,
					climbRate, targetAlt, accelUp, accelDown,  allowClimbRateChange);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}          
	}

	public static Pair<Position, Velocity> vsLevelOut(Pair<Position, Velocity> sv0, double t, double climbRate, 
			double targetAlt, double a, boolean allowClimbRateChange) {
		Position so = sv0.first;
		Velocity vo = sv0.second;
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.vsLevelOut(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,
					climbRate, targetAlt, a,  allowClimbRateChange);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.vsLevelOut(new Pair<Vect3,Velocity>(so.point(), vo),t,
					climbRate, targetAlt, a,  allowClimbRateChange);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}          
	}

	public static Pair<Position, Velocity> vsLevelOut(Pair<Position, Velocity> sv0, double t, double climbRate, 
			double targetAlt, double a) {
		Position so = sv0.first;
		Velocity vo = sv0.second;
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.vsLevelOut(new Pair<LatLonAlt,Velocity>(so.lla(),vo),t,
					climbRate, targetAlt, a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.vsLevelOut(new Pair<Vect3,Velocity>(so.point(), vo),t,
					climbRate, targetAlt, a);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		}          
	}


}

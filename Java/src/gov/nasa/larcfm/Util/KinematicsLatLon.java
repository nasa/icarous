/*
 * KinematicsLatLon.java 
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
package gov.nasa.larcfm.Util;

/**
 * A library of functions to aid the computation of the kinematics of an aircraft.  This
 * library is currently under development and is far from complete.  The majority of the functions
 * handle constant velocity turns and movement with constant ground speed acceleration.<p>
 * 
 * Unless otherwise noted, all kinematics function parameters are in internal units -- angles are in radians,
 * linear speeds are in m/s, distances are in meters, time is in seconds.
 * 
 */
public class KinematicsLatLon {

	/**
	 * Linearly project the given position and velocity to a new position and velocity
	 * @param so  initial position 
	 * @param vo  initial velocity
	 * @param t   time
	 * @return linear projection of sv0 to time t
	 */

	public static Pair<LatLonAlt,Velocity> linear(LatLonAlt so, Velocity vo, double t) {
		LatLonAlt sn = GreatCircle.linear_initial(so, vo, t);
		return new Pair<LatLonAlt,Velocity>(sn,vo);
	}

	/**
	 * Linearly project the given position and velocity to a new position and velocity
	 * @param sv0  initial position and velocity
	 * @param t   time
	 * @return linear projection of sv0 to time t
	 */
	public static Pair<LatLonAlt,Velocity> linear(Pair<LatLonAlt,Velocity> sv0, double t) {
		LatLonAlt s0 = sv0.first;
		Velocity v0 = sv0.second;
		//f.pln(" $$$$$$$ in linear at time: "+t);
		return new Pair<LatLonAlt,Velocity>(linear(s0,v0,t).first,v0);
	}


	/**
	 * EXPERIMENTAL
	 * Return the beginning and end points and radius for a turn given 3 coordinates and a track in and track out
	 * @param a point on gc before turn
	 * @param b intersection point of two great circles
	 * @param c point on gc after turn
	 * @param trackIn
	 * @param trackOut
	 * @return (beginning of turn point, end of turn point) or (INVALID,INVALID) if the great circles are the same (0 or 180 degree "turns")
	 */
	public static Pair<LatLonAlt, LatLonAlt> turnEndPoints(LatLonAlt a, LatLonAlt b, LatLonAlt c, double trackIn, double trackOut) {
		if (GreatCircle.collinear(a, b, c)) return Pair.make(LatLonAlt.INVALID, LatLonAlt.INVALID);
		LatLonAlt bot = GreatCircle.tangent(a, b, trackIn);
		LatLonAlt eot = GreatCircle.tangent(b, c, trackOut);
		if (bot.isInvalid() && !eot.isInvalid()) {
			double dist1 = GreatCircle.distance(bot, b);
			double dist2 = GreatCircle.distance(b, c);
			eot = GreatCircle.interpolate(b, c, dist1/dist2);
		} else if (eot.isInvalid() && ! bot.isInvalid()) {
			double dist1 = GreatCircle.distance(b, eot);
			double dist2 = GreatCircle.distance(a, b);
			bot = GreatCircle.interpolate(b, a, dist1/dist2);
		} else if (bot.isInvalid() && eot.isInvalid()) {
			return Pair.make(LatLonAlt.INVALID, LatLonAlt.INVALID); 
		}
		return Pair.make(bot, eot);
	}

	/** 
	 * center of turn
	 * @param s0 point on turn
	 * @param trk track at point
	 * @param radius radius of turn
	 * @param dir +1 clockwise, -1 counterclockwise
	 * @return turn center
	 */
	  public static LatLonAlt center(LatLonAlt s0, double trk, double radius, int dir) {
		  return GreatCircle.linear_initial(s0, trk+dir*Math.PI/2, radius);
	  }  

	  /**
	   * center of turn
	   * @param s0 point on turn
	   * @param v0 velocity at point
	   * @param omega turn rate
	   * @return turn center
	   */
	  public static LatLonAlt center(LatLonAlt s0, Velocity v0, double omega) {
		  double v = v0.gs();
	      double R = v/omega;
	      return center(s0, v0.trk(), R, Util.sign(omega)); 		  
	  }  

	
	public static Pair<LatLonAlt, LatLonAlt> turnEndPoints(LatLonAlt a, LatLonAlt b, double trackIn, double trackOut, double R, boolean right) {
		LatLonAlt bot;
		// special case where we are heading due north or south: approximate the BOT
		if (Util.almost_equals(trackIn, Math.PI/2) || Util.almost_equals(trackIn, 3*Math.PI/2)) {
			LatLonAlt b1 = GreatCircle.tangent(a, b, trackIn+0.01);
			LatLonAlt b2 = GreatCircle.tangent(a, b, trackIn-0.01);
			bot = GreatCircle.interpolate(b1, b2, 0.5);
		} else {
			bot = GreatCircle.tangent(a, b, trackIn);
		}
		LatLonAlt center;
		if (right) {
			center = GreatCircle.linear_initial(bot, trackIn+Math.PI/2, R);
		} else {
			center = GreatCircle.linear_initial(bot, trackIn-Math.PI/2, R);
		}
		LatLonAlt eot = GreatCircle.tangent(center, R, trackOut, right);
		return Pair.make(bot, eot);
	}

	/**
	 * EXPERIMENTAL
	 * Find the center of a circle given bot, eot, and intercept points
	 * @param a bot
	 * @param b intercept point
	 * @param c eot
	 * @return center of turn or INVALID
	 */
	public static LatLonAlt center(LatLonAlt a, LatLonAlt b, LatLonAlt c) {
		double PI = Math.PI;
		double alpha = GreatCircle.angle_between(a, b, c);
		Velocity v1 = GreatCircle.velocity_initial(a, b, 100);
		Velocity v2 = GreatCircle.velocity_initial(b, c, 100);
		Velocity v3 = GreatCircle.velocity_final(a, b, 100); // use the final velocity to avoid weirdness near 180 degrees
		int dir = Util.turnDir(v3.trk(), v2.trk());
		Velocity perp = v1.mkAddTrk(PI/2);
		Velocity half = v2.mkAddTrk(alpha/2);
		if (dir < 0) {
			perp = v1.mkAddTrk(-PI/2);
			half = v2.mkAddTrk(-alpha/2);
		}
		LatLonAlt a2 = GreatCircle.linear_initial(a, perp, 100);
		LatLonAlt b2 = GreatCircle.linear_initial(b, half, 100);
		return GreatCircle.intersection(a, a2, b, b2);
	}

	/**
	 * Determine the proper omega (angular velocity) given am earth-surface speed and radius.
	 * @param speed
	 * @param radius
	 * @return
	 */
	public static double turnRateRadius(double speed, double radius) {
		double R = GreatCircle.chord_distance(radius*2.0)/2.0;	//convert surface distance to distance in the small circle's plane
		return Kinematics.turnRateRadius(speed,R);
	}

	/**
	 * Determine the earth-surface radius of a turn given the ground speed and omega (angular velocity).
	 * @param speed
	 * @param omega
	 * @return
	 */
	public static double turnRadiusByRate(double speed, double omega) {
		double R = Kinematics.turnRadiusByRate(speed, omega);	// this is in the small circle's plane
		return GreatCircle.surface_distance(R*2)/2.0;			// convert to surface of the earth distance
	}

	/**
	 * Position/Velocity after turning t time units according to track rate omega
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time into turn 
	 * @param omega       rate of change of turn angle, sign indicates direction
	 * @return Position/Velocity after t time
	 */
	public static Pair<LatLonAlt,Velocity> turnOmega(LatLonAlt so, Velocity vo, double t, double omega) {
		double currentTrk = vo.trk();
		double perpTrk;
		if (omega > 0) {
			perpTrk = currentTrk+Math.PI/2;
		} else {
			perpTrk = currentTrk-Math.PI/2;
		}
		double radius = Kinematics.turnRadiusByRate(vo.gs(), omega);
		LatLonAlt center = GreatCircle.linear_initial(so, perpTrk, radius);
		double theta = omega*t;
		double vFinalTrk = GreatCircle.initial_course(center,so);
		double nTrk = vFinalTrk + theta;
		LatLonAlt sn = GreatCircle.linear_initial(center, nTrk, radius);
		sn = sn.mkAlt(so.alt() + vo.z*t);
		//double finalTrk = currentTrk+theta;                        
		double final_course = GreatCircle.final_course(center,sn);   // TODO: THIS IS PROBABLY BETTER
		double finalTrk = final_course + Util.sign(omega)*Math.PI/2;				
		Velocity vn = vo.mkTrk(finalTrk);  
		return new Pair<LatLonAlt,Velocity>(sn,vn);
	}

	
	
	public static Pair<LatLonAlt,Velocity> turnRadius(LatLonAlt so, Velocity vo, double t, double signedRadius) {
		double currentTrk = vo.trk();
		double dir = Util.sign(signedRadius);
		double perpTrk = currentTrk+dir*Math.PI/2;
		double radius = Math.abs(signedRadius);
		LatLonAlt center = GreatCircle.linear_initial(so, perpTrk, radius);		
	    double pathDist = vo.gs()*t;	
		double theta = pathDist/radius; //TODO: change to GreatCircle.small_circle_arc_angle(radius,pathDist);
		//TODO: theta = pathDist/radius, assumes radius is a chord radius, when it is actually a great circle radius.
		//      for small distances the difference is not that big, but still...
		//Note: The other problem is that this assumes a constant speed and constant ground speed through the turn.
		//      this may or may not be true.
		double vFinalTrk = GreatCircle.initial_course(center,so);
		double nTrk = vFinalTrk + dir*theta;
		LatLonAlt sn = GreatCircle.linear_initial(center, nTrk, radius);
		sn = sn.mkAlt(so.alt() + vo.z*t);
		double final_course = GreatCircle.final_course(center,sn);   
		double finalTrk = final_course + dir*Math.PI/2;				
		Velocity vn = vo.mkTrk(finalTrk);  
		return new Pair<LatLonAlt,Velocity>(sn,vn);
	}


	/**
	 * EXPERIMENTAL
	 * Spherical turnOmega
	 * @param so initial position
	 * @param vo initial velocity
	 * @param t time of turn
	 * @param omega angular rate change (circular turn time/2PI)
	 * @return position and velocity at time i
	 */
	public static Pair<LatLonAlt,Velocity> turnOmegaAlt(LatLonAlt so, Velocity vo, double t, double omega) {
		double currentTrk = vo.trk();
		double perpTrk;
		if (omega > 0.0) {
			perpTrk = currentTrk+Math.PI/2;
		} else {
			perpTrk = currentTrk-Math.PI/2;
		}
		double radius = turnRadiusByRate(vo.gs(), omega);
		LatLonAlt center = GreatCircle.linear_initial(so, perpTrk, radius);
		//f.pln("center="+center);
		LatLonAlt sn = GreatCircle.small_circle_rotation(so,center,omega*t).mkAlt(so.alt()+vo.z*t);
		double finalPerpTrk = GreatCircle.initial_course(sn,center);
		double nTrk = finalPerpTrk - Math.PI/2 * Util.sign(omega);
		Velocity vn = vo.mkTrk(nTrk);  
		return new Pair<LatLonAlt,Velocity>(sn,vn);		
	}


	/**
	 * Position/Velocity after turning t time units according to track rate omega
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time of turn 
	 * @param omega       rate of change of track, sign indicates direction
	 * @return Position/Velocity after t time
	 */
	public static Pair<LatLonAlt,Velocity> turnOmega(Pair<LatLonAlt,Velocity> sv0, double t, double omega) {
		if (Util.almost_equals(omega,0))
			return linear(sv0,t);
		LatLonAlt s0 = sv0.first;
		Velocity v0 = sv0.second;
		return turnOmega(s0,v0,t,omega);
	}

//	/**
//	 * Position/Velocity after turning 
//	 * @param s0          starting position
//	 * @param v0          initial velocity
//	 * @param signedRadius           radius (sign indicates direction)
//	 * @param d           distance into turn  
//	 * @return Position/Velocity after turning distance d
//	 */
//	@Deprecated
//	public static Pair<LatLonAlt,Velocity> turnByDist(LatLonAlt so, Velocity vo, double signedRadius, double d) {
//		double currentTrk = vo.trk();
//		double perpTrk;
//		if (signedRadius > 0) perpTrk = currentTrk+Math.PI/2;
//		else perpTrk = currentTrk-Math.PI/2;
//		double radius = Math.abs(signedRadius);
//		LatLonAlt center = GreatCircle.linear_initial(so, perpTrk, radius);
//		double alpha = d/signedRadius;
//		double vFinalTrk = GreatCircle.initial_course(center,so);
//		double nTrk = vFinalTrk + alpha;
//		LatLonAlt sn = GreatCircle.linear_initial(center, nTrk, radius);
//		double t = d/vo.gs();
//		sn = sn.mkAlt(so.alt() + vo.z*t);	
//		double final_course = GreatCircle.final_course(center,sn);
//		//double finalTrk = currentTrk+alpha;
//		double finalTrk = final_course + Util.sign(d)*Math.PI/2;
//		Velocity vn = vo.mkTrk(finalTrk);  
//		return new Pair<LatLonAlt,Velocity>(sn,vn);
//	}


	/** 
	 * Position/Velocity after turning (does not compute altitude!!)
	 * 
	 * Note: will be used in a context where altitude is computing subsequently
	 * 
	 * @param so          starting position
	 * @param center      center of turn
	 * @param dir         direction of turn
	 * @param d           distance into turn (non-negative)
	 * @param gsAtd       ground speed at end of turn, used to update velocity vector at end (done for efficiency)
	 * @return            Position/Velocity after turning distance d
	 */
	public static Pair<LatLonAlt,Velocity> turnByDist(LatLonAlt so, LatLonAlt center, int dir, double d, double gsAtd) {
        double R = GreatCircle.distance(so, center);
		double alpha = dir*d/R;	//TODO: change to dir * GreatCircle.small_circle_arc_angle(R, d);
		double trkFromCenter = GreatCircle.initial_course(center,so);
		double nTrk = trkFromCenter + alpha;
		LatLonAlt sn = GreatCircle.linear_initial(center, nTrk, R);
		sn = sn.mkAlt(0.0);
		double final_course = GreatCircle.final_course(center,sn);
		//f.pln(" $$ d = "+d+" final_course = "+final_course+" nTrk = "+nTrk);
		double finalTrk = final_course + dir*Math.PI/2;
        Velocity vn = Velocity.mkTrkGsVs(finalTrk,gsAtd,0.0);
		//double finalTrk = vo.trk()+alpha;
		//double finalTrk = final_course + Util.sign(d)*Math.PI/2;		
		return new Pair<LatLonAlt,Velocity>(sn,vn);
	}

	
	/**
	 * Position/Velocity after turning t time units right or left with radius R in the direction turnRight
	 * @param s0          starting position
	 * @param v0          initial velocity
	 * @param t           time of turn 
	 * @param R           turn radius (positive)
	 * @param turnRight   true iff only turn direction is to the right
	 * @return Position/Velocity after t time
	 */
	public static Pair<LatLonAlt,Velocity> turn(LatLonAlt s0, Velocity v0, double t, double R, boolean turnRight) {
		if (Util.almost_equals(R,0))
			return new Pair<LatLonAlt,Velocity>(s0,v0);   
		int dir = -1;  
		if (turnRight) dir = 1;
		double omega = dir*v0.gs()/R; //TODO: change to dir*GreatCircle.small_circle_arc_angle(radius,v0.gs());
		return turnOmega(s0,v0,t,omega);
	}

	/**
	 * Position/Velocity after turning t time units right or left with with radius R in the direction turnRight
	 * @param sv0         Pair (initial position, initial velocity)
	 * @param t           time point of interest 
	 * @param R           turn radius (positive)
	 * @param turnRight   true iff only turn direction is to the right
	 * @return Position/Velocity pair after t time
	 */
	public static Pair<LatLonAlt,Velocity> turn(Pair<LatLonAlt,Velocity> sv0, double t, double R, boolean turnRight) {
		return turn(sv0.first,sv0.second,t,R,turnRight);
	}


	/**
	 *  Position/Velocity after t time units turning at the rate of "omega," after that 
	 *  continue in a straight line.  This function can make a turn greater than 180 deg
	 * @param so         initial position and velocity
	 * @param t          time point of interest
	 * @param turnTime   total time of turn [secs]
	 * @param omega 	turn rate
	 * @return Position/Velocity after time t
	 */
	public static Pair<LatLonAlt,Velocity> turnUntilTimeOmega(Pair<LatLonAlt,Velocity> svo, double t, double turnTime, double omega) {
		Pair<LatLonAlt,Velocity> tPair;
		if (t <= turnTime) {
			tPair = turnOmega(svo, t, omega);
		} else {
			tPair = turnOmega(svo, turnTime, omega);
			tPair = linear(tPair,t-turnTime);
		}
		return tPair;
	}

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
	public static Pair<LatLonAlt,Velocity> turnUntilTimeOmega(LatLonAlt so, Velocity vo, double t, double turnTime, double omega) {
		return turnUntilTimeOmega(new Pair<LatLonAlt,Velocity>(so,vo), t, turnTime, omega);
	}


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
	public static Pair<LatLonAlt,Velocity> turnUntil(Pair<LatLonAlt,Velocity> svo, double t, double goalTrack, double maxBank) {
		double omega = Kinematics.turnRateGoal(svo.second, goalTrack, maxBank);
		double turnTime = Kinematics.turnTime(svo.second, goalTrack, maxBank);		
		return turnUntilTimeOmega(svo,t,turnTime,omega);
	}


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
	public static Pair<LatLonAlt,Velocity> turnUntil(LatLonAlt so, Velocity vo, double t, double goalTrack, double maxBank) {
		return turnUntil(new Pair<LatLonAlt,Velocity>(so,vo), t, goalTrack, maxBank);
	}

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
	public static Pair<LatLonAlt,Velocity> turnUntilTimeRadius(Pair<LatLonAlt,Velocity> svo, double t, double turnTime, double R, boolean turnRight) {
		Pair<LatLonAlt,Velocity> tPair;
		if (t <= turnTime) {
			tPair = turn(svo, t, R, turnRight);
		} else {
			tPair = turn(svo, turnTime, R, turnRight);
			tPair = linear(tPair,t-turnTime);
		}
		return tPair;
	}

	
	/**
	 * EXPERMENTAL
	 * Calculate when during the turn we will be closest to the given point.
	 * @param s0 turn start position
	 * @param v0 turn start velocity
	 * @param omega rate of turn (+ = right, - = left)
	 * @param x point of interest
	 * @param endTime time at which turn finishes.  If <= 0, assume a full turn is allowed.
	 * @return time on turn when we are closest to the given point x (in seconds), or -1 if we are precisely at the turn's center
	 * This will be bounded by [0,endTime]
	 */
	public static double closestTimeOnTurn(LatLonAlt s0, Velocity v0, double omega, LatLonAlt x, double endTime) {
		LatLonAlt center = center(s0,v0,omega);
		if (x.mkAlt(0).almostEquals(center.mkAlt(0))) return -1.0;
		double ang1 = GreatCircle.initial_course(center,s0);
		double ang2 = GreatCircle.initial_course(center,x);		
		double delta = Util.turnDelta(ang1, ang2, Util.sign(omega));
		double t = Math.abs(delta/omega);
		if (endTime > 0 && (t < 0 || t > endTime)) {
			double maxTime = 2*Math.PI/Math.abs(omega);
			if (t > (maxTime + endTime) / 2) {
				return 0.0;
			} else {
				return endTime;
			}
		}
		return t;
	}

	/**
	 * EXPERMENTAL
	 * Calculate when during the turn we will be closest to the given point.
	 * @param s0 turn start position
	 * @param v0 turn start velocity
	 * @param R radius
	 * @param dir direction of turn
	 * @param x point of interest
	 * @param maxDist time at which turn finishes.  If <= 0, assume a full turn is allowed.
	 * @return dist on turn when we are closest to the given point x, or -1 if we are precisely at the turn's center
	 * This will be bounded by [0,maxDist]
	 */
	public static double closestDistOnTurn(LatLonAlt s0, Velocity v0, double R, int dir, LatLonAlt x, double maxDist) {
		LatLonAlt center = center(s0, v0.trk(), R, dir);
		if (x.mkAlt(0).almostEquals(center.mkAlt(0))) return -1.0;
		double ang1 = GreatCircle.initial_course(center,s0);
		double ang2 = GreatCircle.initial_course(center,x);		
		double delta = Util.turnDelta(ang1, ang2, dir);
		double t = GreatCircle.small_circle_arc_length(R, delta);
		if (maxDist > 0 && (t < 0 || t > maxDist)) {
			double maxD = 2*Math.PI*R;
			if (t > (maxD + maxDist) / 2) {
				return 0.0;
			} else {
				return maxDist;
			}
		}
		return t;
	}

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
	public static Pair<LatLonAlt,Velocity> gsAccel(LatLonAlt so, Velocity vo,  double t, double a) {
		double dist = vo.gs()*t + 0.5*a*t*t;
		double currentTrk = vo.trk();
		LatLonAlt sn = GreatCircle.linear_initial(so, currentTrk, dist);
		sn = sn.mkAlt(so.alt() + vo.z*t);
		double vnGs = vo.gs() + a*t;
		Velocity vn = vo.mkGs(vnGs);
		//f.pln(" $$$$$ gsAccel: sn = "+sn+" vn = "+vn);
		return new Pair<LatLonAlt,Velocity>(sn,vn);
	}


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
	public static Pair<LatLonAlt,Velocity> gsAccelUntil(LatLonAlt so, Velocity vo, double t, double goalGS, double gsAccel) {
		if (gsAccel < 0 ) {
			System.out.println("Kinematics.gsAccelUntil: user supplied negative gsAccel!!");
			gsAccel = -gsAccel;                              // make sure user supplies positive value
		}
		double accelTime = Kinematics.gsAccelTime(vo,goalGS,gsAccel);
		int sgn = 1;
		if (goalGS < vo.gs()) sgn = -1;
		double a = sgn*gsAccel;
		Pair<LatLonAlt, Velocity> nsv = gsAccel(so, vo, accelTime, a);
		if (t<=accelTime) return gsAccel(so, vo, t, a);
		else  return gsAccel(nsv.first, nsv.second, t-accelTime, 0);	
	}

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
	public static Pair<LatLonAlt,Velocity> gsAccelUntil(Pair<LatLonAlt,Velocity> sv0, double t, double goalGs, double gsAccel) {
		return gsAccelUntil(sv0.first, sv0.second, t, goalGs, gsAccel);
	}

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
	public static Pair<LatLonAlt,Velocity> vsAccel(LatLonAlt so, Velocity vo,  double t, double a) {
		double dist = vo.gs()*t;
		double currentTrk = vo.trk();
		LatLonAlt sn = GreatCircle.linear_initial(so, currentTrk, dist);
		double nsz = so.alt() + vo.z*t + 0.5*a*t*t;
		sn = sn.mkAlt(nsz);
		Velocity  vn = vo.mkVs(vo.z + a*t);
		return new Pair<LatLonAlt,Velocity>(sn,vn);
	}

	public static Pair<LatLonAlt,Velocity> vsAccel(Pair<LatLonAlt,Velocity> svo,  double t, double a) {
		return vsAccel(svo.first, svo.second,t,a);
	}

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
	public static Pair<LatLonAlt,Velocity> vsAccelUntil(LatLonAlt so, Velocity vo, double t, double goalVs, double vsAccel) {
		if (vsAccel < 0 ) {
			System.out.println("Kinematics.vsAccelUntil: user supplied negative vsAccel!!");
			vsAccel = -vsAccel;                              // make sure user supplies positive value
		}
		double accelTime = Kinematics.vsAccelTime(vo,goalVs, vsAccel);
		int sgn = 1;
		if (goalVs < vo.vs()) sgn = -1;
		//LatLonAlt ns = LatLonAlt.ZERO;
		if (t <= accelTime)
			return vsAccel(so, vo, t, sgn*vsAccel);
		else {
			LatLonAlt posEnd = vsAccel(so,vo,accelTime,sgn*vsAccel).first;
			Velocity nvo = Velocity.mkVxyz(vo.x,vo.y, goalVs);
			return linear(posEnd,nvo,t-accelTime);
		}
	}

	public static Pair<LatLonAlt,Velocity> vsAccelUntil(Pair<LatLonAlt,Velocity> sv0, double t, double goalVs, double vsAccel) {
		return vsAccelUntil(sv0.first, sv0.second,t,goalVs, vsAccel);
	}

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
	private static Pair<LatLonAlt, Velocity> vsLevelOutCalculation(Pair<LatLonAlt,Velocity> sv0,  
			double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t) {
		LatLonAlt s0 = sv0.first;
		Velocity v0 = sv0.second;
		double soz = s0.alt();
		double voz = v0.z();
		Pair<Double, Double> vsL = Kinematics.vsLevelOutCalc(soz,voz, targetAlt, a1, a2, t1, t2, t3, t);
		double nz = vsL.first;
		double nvs = vsL.second;
		Velocity nv = v0.mkVs(nvs);
		LatLonAlt ns = linear(s0,v0,t).first.mkAlt(nz);
		return new Pair<LatLonAlt, Velocity>(ns,nv);
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
	public static Pair<LatLonAlt, Velocity> vsLevelOut(Pair<LatLonAlt, Velocity> sv0, double t, double climbRate, 
			double targetAlt, double accelUp, double accelDown, boolean allowClimbRateChange) {
		Tuple5<Double,Double,Double,Double,Double> LevelParams = Kinematics.vsLevelOutTimes(sv0.first.alt(), sv0.second.vs(), climbRate, targetAlt, accelUp, accelDown, allowClimbRateChange);
		return vsLevelOutCalculation(sv0, targetAlt, LevelParams.fourth, LevelParams.fifth, LevelParams.first, LevelParams.second, LevelParams.third, t);
	}

	public static Pair<LatLonAlt, Velocity> vsLevelOut(Pair<LatLonAlt, Velocity> sv0, double t, double climbRate, 
			double targetAlt, double a, boolean allowClimbRateChange) {
		return vsLevelOut(sv0, t, climbRate, targetAlt, a, -a, allowClimbRateChange);		
	}

	public static Pair<LatLonAlt, Velocity> vsLevelOut(Pair<LatLonAlt, Velocity> sv0, double t, double climbRate, 
			double targetAlt, double a) {
		return vsLevelOut(sv0, t, climbRate, targetAlt, a, -a, true);		
	}


}

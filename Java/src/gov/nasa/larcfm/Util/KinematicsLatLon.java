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

public class KinematicsLatLon {

	/**
	 * Linearly project the given position and velocity to a new position and velocity
	 * @param sv0  initial position and velocity
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
	 * @Param c point on gc after turn
	 * @Param trackIn
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
		double R = GreatCircle.chord_distance(radius*2.0)/2.0;
		return Kinematics.turnRateRadius(speed,R);
	}

	/**
	 * Determine the earth-surface radius of a turn given the ground speed and omega (angular velocity).
	 * @param speed
	 * @param omega
	 * @return
	 */
	public static double turnRadiusByRate(double speed, double omega) {
		double R = Kinematics.turnRadiusByRate(speed, omega);
		return GreatCircle.surface_distance(R*2)/2.0;
	}

	//	  public static double fromEuclideanOmega(double speed, double omega) {
	//		  
	//	  }

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
		if (omega > 0) perpTrk = currentTrk+Math.PI/2;
		else perpTrk = currentTrk-Math.PI/2;
		double radius = Kinematics.turnRadiusByRate(vo.gs(), omega);
		//TODO: the above uses a chord radius.  It should use a great circle radius:
		//		double r = Kinematics.turnRadiusByRate(vo.gs(), omega);
		//		double radius = GreatCircle.distance_from_angle(Math.asin(r/GreatCircle.spherical_earth_radius), 0.0);
		//TODO: The other problem is that this assumes a constant speed and constant turn rate (omega) with 
		//      respect to the center of the turn. This is different from having a constant track change rate
		//      with respect to the ownship.  Effectively the track change rate will be variable throughout the 
		//      turn, with this being more pronounced as one approaches the poles.
		LatLonAlt center = GreatCircle.linear_initial(so, perpTrk, radius);
		double alpha = omega*t;
		double vFinalTrk = GreatCircle.initial_course(center,so);
		double nTrk = vFinalTrk + alpha;
		LatLonAlt sn = GreatCircle.linear_initial(center, nTrk, radius);
		sn = sn.mkAlt(so.alt() + vo.z*t);
		Velocity vn = vo.mkTrk(currentTrk+alpha);  
		return new Pair<LatLonAlt,Velocity>(sn,vn);
	}


	/**
	 * Spherical turnOmega
	 * @param so initial posiiton
	 * @param vo intiial velocity
	 * @param t time of turn
	 * @param omega angular rate change (circular turn time/2PI)
	 * @return position and velocity at time i
	 */
	public static Pair<LatLonAlt,Velocity> turnOmegaAlt(LatLonAlt so, Velocity vo, double t, double omega) {
		double currentTrk = vo.trk();
		double perpTrk;
		if (omega > 0) perpTrk = currentTrk+Math.PI/2;
		else perpTrk = currentTrk-Math.PI/2;
		double radius = turnRadiusByRate(vo.gs(), omega);
		LatLonAlt center = GreatCircle.linear_initial(so, perpTrk, radius);
		LatLonAlt sn = GreatCircle.small_circle_rotation(so,center,omega*t).mkAlt(so.alt()+vo.z*t);
		double finalPerpTrk = GreatCircle.initial_course(sn,center);
		double nTrk = finalPerpTrk - Math.PI/2 * Math.signum(omega);
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
		double omega = dir*v0.gs()/R;
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

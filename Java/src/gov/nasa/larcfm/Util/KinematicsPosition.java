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


	/**
	 *  Position and velocity after t time units turning in direction "dir" with radius R.  
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
			return new Pair<Position,Velocity>(new Position(resp.first),resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turn(so.point(),vo,t,R,turnRight);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);  
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
	public static Pair<Position,Velocity> turnOmega(Position so, Velocity vo, double t, double omega) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnOmega(so.lla(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnOmega(so.point(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
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
	public static Pair<Position,Velocity> turnOmegaAlt(Position so, Velocity vo, double t, double omega) {
		if (so.isLatLon()) {
			Pair<LatLonAlt,Velocity> resp = KinematicsLatLon.turnOmegaAlt(so.lla(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
		} else {
			Pair<Vect3,Velocity> resp = Kinematics.turnOmega(so.point(),vo,t,omega);
			return new Pair<Position,Velocity>(new Position(resp.first), resp.second);
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
	 * @param goalTrack the target track angle
	 * @param bankAngle the aircraft's bank angle
	 * @param t   time of turn [secs]
	 * @param turnRight true iff only turn direction is to the right
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
	 * @param goalTrack the target track angle
	 * @param bankAngle the aircraft's bank angle
	 * @param t   time of turn [secs]
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



	//  /**
	//   *  Position and velocity after t time units accelerating vertically. 
	//   * @param so  starting position
	//   * @param vo  initial velocity
	//   * @param a   vertical speed acceleration (a positive value)
	//   * @param t   time of turn [secs]
	//   * @return Position and Velocity after t time
	//   */
	//  public static Pair<Position,Velocity> vsAccelUntilWithRampUp(Position so, Velocity vo, double t, double goalVs, double a,double tRamp) {
	//    Vect3 s3 = so.point();
	//    if (so.isLatLon()) {
	//      s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
	//    }
	//    Vect3 pres = Kinematics.vsAccelUntilWithRampUp(s3,vo,t,goalVs,a,tRamp).first;
	//    Velocity vres = Kinematics.vsAccelUntilWithRampUp(s3,vo,t,goalVs,a,tRamp).second;
	//    if (so.isLatLon()) {
	//      return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	//    } else {
	//      return new Pair<Position,Velocity>(new Position(pres), vres);  
	//    }
	//  }

	//  public static double vsLevelOutTime(Position so, Velocity vo, double climbRate, double targetAlt, double a) {
	//    Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
	//    // we don't need horizontal components, so don't need to project
	//    return Kinematics.vsLevelOutTime(sv, climbRate, targetAlt, a); 
	//  }
	//
	//  /**
	//   *  Position and velocity after t time units accelerating vertically.  This is a wrapper around posAccelVS 
	//   *  for Position objects,and uses the projection defined in the static Projection class.
	//   * @param so  starting position
	//   * @param vo  initial velocity
	//   * @param a   vertical speed acceleration (a positive value)
	//   * @param t   time of turn [secs]
	//   * @return Position and Velocity after t time
	//   */
	//  public static Pair<Position,Velocity> vsLevelOut(Position so, Velocity vo, double t, double climbRate, double targetAlt, double a, boolean allowClimbRateReduction) {
	//    Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
	//    if (so.isLatLon()) {
	//      sv = Projection.createProjection(so.lla().zeroAlt()).project(so, vo);
	//    }
	//    Pair<Vect3,Velocity> vat = Kinematics.vsLevelOut(sv, t, climbRate, targetAlt, a, allowClimbRateReduction); 
	//    if (so.isLatLon()) {
	//      return Projection.createProjection(so.lla().zeroAlt()).inverse(vat.first,vat.second,true);
	//    } else {
	//      return new Pair<Position,Velocity>(new Position(vat.first), vat.second);  
	//    }
	//  }
	//
	//  public static Pair<Position,Velocity> vsLevelOut(Position so, Velocity vo, double t, double climbRate, double targetAlt, double a) {
	//    Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
	//    if (so.isLatLon()) {
	//      sv = Projection.createProjection(so.lla().zeroAlt()).project(so, vo);
	//    }
	//    Pair<Vect3,Velocity> vat = Kinematics.vsLevelOut(sv, t, climbRate, targetAlt, a); 
	//    if (so.isLatLon()) {
	//      return Projection.createProjection(so.lla().zeroAlt()).inverse(vat.first,vat.second,true);
	//    } else {
	//      return new Pair<Position,Velocity>(new Position(vat.first), vat.second);  
	//    }
	//  }
	//
	//  public static Triple<Position,Velocity,Double> vsLevelOutFinal(Position so, Velocity vo, double climbRate, double targetAlt, double a) {
	//    if (climbRate == 0) {
	//      return new Triple<Position,Velocity,Double>(so.mkZ(targetAlt),vo.mkVs(0),0.0);
	//    } else {
	//      Pair<Vect3, Velocity> sv = new Pair<Vect3, Velocity>(so.point(),vo);
	//      if (so.isLatLon()) {
	//        sv = Projection.createProjection(so.lla().zeroAlt()).project(so, vo);
	//      }
	//      StateVector vat = Kinematics.vsLevelOutFinal(sv, climbRate, targetAlt, a);
	//      if (vat.t < 0) return new Triple<Position,Velocity,Double>(Position.INVALID, Velocity.INVALID, vat.t);
	//      if (so.isLatLon()) {
	//        Pair<Position,Velocity>p = Projection.createProjection(so.lla().zeroAlt()).inverse(vat.s,vat.v,true);
	//        return new Triple<Position,Velocity,Double>(p.first, p.second, vat.t);
	//      } else {
	//        return new Triple<Position,Velocity,Double>(new Position(vat.s), vat.v, vat.t);
	//      }
	//    }
	//  }
	//  

	//  /**
	//   * Perform a turn delta calculation between trk1 and trk2, compensating for geodetic coordinates
	//   * @param so projection point
	//   * @param trk1 first track of interest
	//   * @param trk2 second track of interest
	//   * @return difference between tracks, in [-PI,+PI]
	//   */
	//  public static double turnDelta(Position so, double trk1, double trk2) {
	//	  double alpha = trk1;
	//	  double beta = trk2;
	//	  if (so.isLatLon()) {
	//		  Velocity v1 = Velocity.mkTrkGsVs(trk1, 100.0, 0.0);
	//		  Velocity v2 = Velocity.mkTrkGsVs(trk2, 100.0, 0.0);
	//		  EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt());
	//		  alpha = proj.projectVelocity(so.lla(), v1).trk();
	//		  beta = proj.projectVelocity(so.lla(), v2).trk();
	//	  }
	//	  return Util.turnDelta(alpha, beta);
	//  }


	//
	//public static double closestTimeOnTurn(Position so, Velocity v1, double omega, Position s2) {
	//  Vect3 s3 = so.point();
	//  Vect3 x3 = s2.point();
	//  if (so.isLatLon()) {
	//    EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt()); 
	//    s3 = proj.project(so); 
	//    x3 = proj.project(s2); 
	//  } 
	//  return Kinematics.closestTimeOnTurn(s3, v1, omega, x3);
	//}


	///**
	// *  Position and velocity after t time units turning in direction "dir" with radius R.  This is a wrapper around turnPosition and
	// *  turnVelocity for Position objects,and uses the projection defined in the static Projection class.
	// * @param so  starting position
	// * @param vo  initial velocity
	// * @param goalTrack the target track angle
	// * @param bankAngle the aircraft's bank angle
	// * @param t   time of turn [secs]
	// * @param turnRight true iff only turn direction is to the right
	// * @return Position and Velocity after t time
	// */
	//public static Pair<Position,Velocity> turnUntilWithRoll(Position so, Velocity vo, double t, double goalTrack, double bankAngle, 
	//    double rollTime) {
	//  //f.pln("Kin.turnProjection so = "+so+" vo = "+vo+" t = "+t+" goalTrack=  "+Units.str("deg",goalTrack)
	//  //	+" bankAngle=  "+Units.str("deg",bankAngle)+" rollRate = "+rollRate);	  
	//  Vect3 s3 = so.point();
	//  if (so.isLatLon()) {
	//    s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
	//  }
	//  Pair<Vect3,Velocity> svres = Kinematics.turnUntilWithRoll(s3,vo,t, goalTrack, bankAngle,rollTime);
	//  Vect3 pres = svres.first;
	//  Velocity vres = svres.second;
	//  //f.pln("Kin.turnProjection so = "+so+" pres = "+pres+" vo = "+vo+" vres=  "+vres);	  
	//  if (so.isLatLon()) {
	//    return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	//  } else {
	//    return new Pair<Position,Velocity>(new Position(pres), vres);  
	//  }
	//}



	//
	//// if this fails, it returns a NaN time
	//public static Pair<Position,Double> intersection(Position so, Velocity vo, Position si, Velocity vi) {
	//  Vect3 so3 = so.point();
	//  Vect3 si3 = si.point();
	//  EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt()); 
	//  if (so.isLatLon()) {
	//    so3 = proj.project(so); 
	//    si3 = proj.project(si); 
	//  }
	//  Pair<Vect3,Double> intersect = VectFuns.intersection(so3, vo, si3, vi);
	//  if (so.isLatLon()) {
	//    return new Pair<Position,Double>(new Position(proj.inverse(intersect.first)),intersect.second);
	//  } else {
	//    return new Pair<Position,Double>(new Position(intersect.first),intersect.second);  
	//  }
	//}
	//
	//
	//public static double timeOfintersection(Position so, Velocity vo, Position si, Velocity vi) {
	//  Vect3 so3 = so.point();
	//  Vect3 si3 = si.point();
	//  EuclideanProjection proj = Projection.createProjection(so.lla().zeroAlt()); 
	//  if (so.isLatLon()) {
	//    so3 = proj.project(so); 
	//    si3 = proj.project(si); 
	//  }
	//  double  intersectTime = VectFuns.timeOfIntersection(so3, vo, si3, vi);
	//  return intersectTime;
	//}



	//
	///** Wrapper around Kinematics.directToPoint()
	// * Returns a quad: end of turn point, velocity at that point, time at that point, direction of turn
	// * If no result is possible (for example the point lies within the given turn radius), this will return a negative time.
	// */
	//public static Quad<Position,Velocity,Double,Integer> directToPoint(Position so, Velocity vo, Position wp, double R) {
	//  Vect3 s3 = so.point();
	//  Vect3 g3 = wp.point();
	//  EuclideanProjection proj = null;
	//  if (so.isLatLon()) {
	//    proj = Projection.createProjection(so.lla().zeroAlt());
	//    s3 = proj.project(so);
	//    g3 = proj.project(wp);
	//  }
	//  Quad<Vect3,Velocity,Double,Integer> dtp = Kinematics.directToPoint(s3,vo,g3,R);
	//  Pair<Position,Velocity> pv;
	//  if (so.isLatLon()) {
	//    pv = proj.inverse(dtp.first,dtp.second,true);
	//  } else {
	//    pv = new Pair<Position,Velocity>(new Position(dtp.first), dtp.second);  
	//  }
	//  return new Quad<Position,Velocity,Double,Integer> (pv.first, pv.second, dtp.third,dtp.fourth);
	//}

	///** Wrapper around Kinematic.genDirectToVertex
	// *  Returns the vertex point (in a linear plan sense) between current point and directTo point.
	// * 
	// * @param so     current position
	// * @param vo     current velocity
	// * @param wp     first point (in a flight plan) that you are trying to connect to
	// * @param bankAngle  turn bank angle
	// * @param timeBeforeTurn   time to continue in current direction before beginning turn
	// * @return (so,t0,t1) vertex point and delta time to reach the vertex point and delta time (from so) to reach end of turn
	// *  If no result is possible this will return an invalid position and negative times.
	// */
	//public static Triple<Position,Double,Double> genDirectToVertex(Position sop, Velocity vo, Position wpp, double bankAngle, double timeBeforeTurn) {
	//  Vect3 s3 = sop.point();
	//  Vect3 g3 = wpp.point();
	//  EuclideanProjection proj = null;
	//  if (sop.isLatLon()) {
	//    proj = Projection.createProjection(sop.lla().zeroAlt());
	//    s3 = proj.project(sop);
	//    g3 = proj.project(wpp);
	//  }
	//  Triple<Vect3,Double,Double> vertTriple = Kinematics.genDirectToVertex(s3,vo,g3,bankAngle,timeBeforeTurn);
	//  if (vertTriple.third < 0) {
	//    return new Triple<Position,Double,Double>(Position.INVALID, -1.0, -1.0);
	//  }
	//  //f.pln(" $$$ genDirectToVertex: vertPair.second = "+vertTriple.second);
	//  Vect3 vertex = vertTriple.first;
	//  //Vect3 eot = vertTriple.third;
	//  Position pp;
	//  if (sop.isLatLon()) {
	//    pp = new Position(proj.inverse(vertex));
	//    //eotp = new Position(proj.inverse(eot));
	//  } else {
	//    pp = new Position(vertex);  
	//    //eotp = new Position(eot); 
	//  }
	//  //  f.pln("KinematicsPosition.genDirectToVertex ipPair="+vertTriple);
	//  //  if (sop.isLatLon()) f.pln("name,lat,lon,alt,trk,gs,vs");
	//  //  else f.pln("name,SX,SY,SZ,TRK,GS,VS");
	//  //  f.pln("a1,"+sop.toStringNP(8)+","+vo.toString8NP());
	//  //  f.pln("b2,"+pp.toStringNP(8)+","+vo.toString8NP());
	//  //  f.pln("c3,"+wpp.toStringNP(8)+","+vo.toString8NP());
	//  return new Triple<Position,Double,Double>(pp,vertTriple.second,vertTriple.third);
	//}

	//static ArrayList<Pair<Position,Double>> genDirectToVertexList(Position so, Velocity vo, Position wp, double bankAngle, double timeBeforeTurn, double timeBetweenPieces) {
	//  Vect3 s3 = so.point();
	//  Vect3 g3 = wp.point();
	//  EuclideanProjection proj = null;
	//  if (so.isLatLon()) {
	//    proj = Projection.createProjection(so.lla().zeroAlt());
	//    s3 = proj.project(so);
	//    g3 = proj.project(wp);
	//  }
	//  ArrayList<Pair<Vect3, Double>> vertTriple = Kinematics.genDirectToVertexList(s3,vo,g3,bankAngle,timeBeforeTurn, timeBetweenPieces);
	//
	//  ArrayList<Pair<Position,Double>> ptriple = new ArrayList<Pair<Position,Double>>();
	//  for (int i = 0; i < vertTriple.size(); i++) {
	//    if (so.isLatLon()) {
	//      Position pp = new Position(proj.inverse(vertTriple.get(i).first));
	//      ptriple.add(new Pair<Position,Double>(pp, vertTriple.get(i).second));  
	//    } else {
	//      ptriple.add(new Pair<Position,Double>(new Position(vertTriple.get(i).first), vertTriple.get(i).second));  
	//    }
	//
	//  }
	//  return ptriple;
	//}


	///**
	// *  Position and velocity after t time units accelerating horizontally.  This is a wrapper around posAccelGS 
	// *  for Position objects,and uses the projection defined in the static Projection class.
	// * @param so  starting position
	// * @param vo  initial velocity
	// * @param t   time of turn [secs]
	// * @param a   ground speed acceleration (or deceleration) (signed)
	// * @return Position and Velocity after t time
	// */
	//public static Pair<Position,Velocity> gsAccel(Position so, Velocity vo, double t, double a) {
	//  Vect3 s3 = so.point();
	//  if (so.isLatLon()) {
	//    s3 = Projection.createProjection(so.lla().zeroAlt()).project(so); 
	//  }
	//  Vect3 pres = Kinematics.gsAccelPos(s3,vo,t, a);
	//  Velocity vres = Velocity.mkTrkGsVs(vo.trk(),vo.gs()+a*t,vo.vs());
	//  if (so.isLatLon()) {
	//    return Projection.createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	//  } else {
	//    return new Pair<Position,Velocity>(new Position(pres), vres);  
	//  }
	//}
	//



}

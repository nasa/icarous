/* KinematicsLatLon
 *
 * Authors:  Ricky Butler              NASA Langley Research Center
 *           George Hagen              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
  *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h"
#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Triple.h"
#include "Velocity.h"
#include "Kinematics.h"
#include "StateVector.h"
#include "Constants.h"
#include "VectFuns.h"
#include "KinematicsLatLon.h"
#include "GreatCircle.h"
#include "Quad.h"
#include "Tuple5.h"
#include "format.h"
#include <cmath>
#include <float.h>


namespace larcfm {


   bool KinematicsLatLon::chordalSemantics = false;

std::pair<LatLonAlt,Velocity> KinematicsLatLon::linear(const LatLonAlt& so, const Velocity& vo, double t) {
	LatLonAlt sn = GreatCircle::linear_initial(so, vo, t);
	return std::pair<LatLonAlt,Velocity>(sn,vo);
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::linear(std::pair<LatLonAlt,Velocity> sv0, double t) {
	LatLonAlt s0 = sv0.first;
	Velocity v0 = sv0.second;
	//fpln(" $$$$$$$ in linear at time: "+t);
	return std::pair<LatLonAlt,Velocity>(linear(s0,v0,t).first,v0);
}

  double KinematicsLatLon::turnRadiusByRate(double speed, double omega) {
    double R = Kinematics::turnRadiusByRate(speed, omega);
    return GreatCircle::surface_distance(R*2)/2.0;
  }
  
std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnOmega(const LatLonAlt& so, const Velocity& vo, double t, double omega) {
	double currentTrk = vo.trk();
	double perpTrk;
	if (omega > 0) perpTrk = currentTrk+M_PI/2;
	else perpTrk = currentTrk-M_PI/2;
	double radius = Kinematics::turnRadiusByRate(vo.gs(), omega);
	LatLonAlt center = GreatCircle::linear_initial(so, perpTrk, radius);
	double alpha = omega*t;
	double vFinalTrk = GreatCircle::initial_course(center,so);
	double nTrk = vFinalTrk + alpha;
	LatLonAlt sn = GreatCircle::linear_initial(center, nTrk, radius);
	sn = sn.mkAlt(so.alt() + vo.z*t);
	//double finalTrk = currentTrk+theta;
	double final_course = GreatCircle::final_course(center,sn);   // TODO: THIS IS PROBABLY BETTER
	double finalTrk = final_course + Util::sign(omega)*M_PI/2;
	Velocity vn = vo.mkTrk(finalTrk);
	return std::pair<LatLonAlt,Velocity>(sn,vn);
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnOmega(std::pair<LatLonAlt,Velocity> sv0, double t, double omega) {
	if (Util::almost_equals(omega,0))
		return linear(sv0,t);
	LatLonAlt s0 = sv0.first;
	Velocity v0 = sv0.second;
	return turnOmega(s0,v0,t,omega);
}

//  std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnRadius(const LatLonAlt& so, const Velocity& vo, double t, double signedRadius) {
//    double currentTrk = vo.trk();
//    double dir = Util::sign(signedRadius);
//    double perpTrk = currentTrk+dir*M_PI/2;
//    double radius = std::abs(signedRadius);
//    LatLonAlt center = GreatCircle::linear_initial(so, perpTrk, radius);
//    double pathDist = vo.gs()*t;
//    double theta = pathDist/radius;
//    //TODO: theta = pathDist/radius, assumes radius is a chord radius, when it is actually a great circle radius.
//    //      for small distances the difference is not that big, but still...
//    //Note: The other problem is that this assumes a constant speed and constant ground speed through the turn.
//    //      this may or may not be true.
//    double vFinalTrk = GreatCircle::initial_course(center,so);
//    double nTrk = vFinalTrk + dir*theta;
//    LatLonAlt sn = GreatCircle::linear_initial(center, nTrk, radius);
//    sn = sn.mkAlt(so.alt() + vo.z*t);
//    double final_course = GreatCircle::final_course(center,sn);
//    double finalTrk = final_course + dir*M_PI/2;
//    Velocity vn = vo.mkTrk(finalTrk);
//    return std::pair<LatLonAlt,Velocity>(sn,vn);
//  }
//

  
std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnByDist2D(const LatLonAlt& so, const LatLonAlt& center, int dir, double d, double gsAtd) {
	LatLonAlt sn = turnByDist2D(so, center, dir, d);
	double final_course = GreatCircle::final_course(center,sn);
	double finalTrk = final_course + dir*M_PI/2;
    Velocity vn = Velocity::mkTrkGsVs(finalTrk,gsAtd,0.0);
	return std::pair<LatLonAlt,Velocity>(sn,vn);
}

LatLonAlt KinematicsLatLon::turnByDist2D(const LatLonAlt& so, const LatLonAlt& center, int dir, double d) {
    double R = GreatCircle::distance(so, center);
    double theta;
    if (chordalSemantics) {
    	double chordalRadius = GreatCircle::to_chordal_radius(R);
    	theta = dir*d/chordalRadius;
    } else {
    	theta = dir*d/R;
    }
	double vFinalTrk = GreatCircle::initial_course(center,so);
	double nTrk = vFinalTrk + theta;
	LatLonAlt sn = GreatCircle::linear_initial(center, nTrk, R);
	sn = sn.mkAlt(0.0);
	return sn;
}

LatLonAlt KinematicsLatLon::turnByAngle2D(const LatLonAlt& so, const LatLonAlt& center, double alpha) {
    double R = GreatCircle::distance(so, center);
	double trkFromCenter = GreatCircle::initial_course(center,so);
	double nTrk = trkFromCenter + alpha;
	LatLonAlt sn = GreatCircle::linear_initial(center, nTrk, R);
	sn = sn.mkAlt(0.0);
	return sn;
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::turn(LatLonAlt s0, Velocity v0, double t, double R, bool turnRight) {
	if (Util::almost_equals(R,0))
		return std::pair<LatLonAlt,Velocity>(s0,v0);
	int dir = -1;
	if (turnRight) dir = 1;
	double omega = dir*v0.gs()/R;
	return turnOmega(s0,v0,t,omega);
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::turn(std::pair<LatLonAlt,Velocity> sv0, double t, double R, bool turnRight) {
	return turn(sv0.first,sv0.second,t,R,turnRight);
}


std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnUntilTimeOmega(std::pair<LatLonAlt,Velocity> svo, double t, double turnTime, double omega) {
	std::pair<LatLonAlt,Velocity> tPair;
	if (t <= turnTime) {
		tPair = turnOmega(svo, t, omega);
	} else {
		tPair = turnOmega(svo, turnTime, omega);
		tPair = linear(tPair,t-turnTime);
	}
	return tPair;
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnUntilTimeOmega(const LatLonAlt& so, const Velocity& vo, double t, double turnTime, double omega) {
	return turnUntilTimeOmega(std::pair<LatLonAlt,Velocity>(so,vo), t, turnTime, omega);
}


std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnUntil(std::pair<LatLonAlt,Velocity> svo, double t, double goalTrack, double maxBank) {
	double omega = Kinematics::turnRateGoal(svo.second, goalTrack, maxBank);
	double turnTime = Kinematics::turnTime(svo.second, goalTrack, maxBank);
	return turnUntilTimeOmega(svo,t,turnTime,omega);
}


std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnUntil(const LatLonAlt& so, const Velocity& vo, double t, double goalTrack, double maxBank) {
	return turnUntil(std::pair<LatLonAlt,Velocity>(so,vo), t, goalTrack, maxBank);
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::turnUntilTimeRadius(const std::pair<LatLonAlt,Velocity>& svo, double t, double turnTime, double R, bool turnRight) {
	std::pair<LatLonAlt,Velocity> tPair;
	if (t <= turnTime) {
		tPair = turn(svo, t, R, turnRight);
	} else {
		tPair = turn(svo, turnTime, R, turnRight);
		tPair = linear(tPair,t-turnTime);
	}
	return tPair;
}


LatLonAlt KinematicsLatLon::center(const LatLonAlt& s0, double trk, double radius, int dir) {
	  return GreatCircle::linear_initial(s0, trk+dir*M_PI/2, radius);
}


LatLonAlt KinematicsLatLon::center(const LatLonAlt& s0, const Velocity& v0, double omega) {
	  double v = v0.gs();
    double R = v/omega;
    return center(s0, v0.trk(), R, Util::sign(omega));
}

double KinematicsLatLon::closestTimeOnTurn(const LatLonAlt& s0, const Velocity& v0, double omega, const LatLonAlt& x, double endTime) {
	LatLonAlt cent = center(s0,v0,omega);
	if (GreatCircle::almostEquals(x.mkAlt(0), cent.mkAlt(0))) return -1.0;
	double ang1 = GreatCircle::initial_course(cent,s0);
	double ang2 = GreatCircle::initial_course(cent,x);
	double delta = Util::turnDelta(ang1, ang2, Util::sign(omega));
	double t = std::abs(delta/omega);
	if (endTime > 0 && (t < 0 || t > endTime)) {
		double maxTime = 2*M_PI/std::abs(omega);
		if (t > (maxTime + endTime) / 2) {
			return 0.0;
		} else {
			return endTime;
		}
	}
	return t;

}


double KinematicsLatLon::closestDistOnTurn(const LatLonAlt& s0, const Velocity& v0, double R, int dir, const LatLonAlt& x, double maxDist) {
	LatLonAlt cent = center(s0, v0.trk(), R, dir);
	if (GreatCircle::almostEquals(x.mkAlt(0),cent.mkAlt(0))) return -1.0;
	double ang1 = GreatCircle::initial_course(cent,s0);
	double ang2 = GreatCircle::initial_course(cent,x);
	double delta = Util::turnDelta(ang1, ang2, dir);
	double t = GreatCircle::small_circle_arc_length(R, delta);
	if (maxDist > 0 && (t < 0 || t > maxDist)) {
		double maxD = 2*M_PI*R;
		if (t > (maxD + maxDist) / 2) {
			return 0.0;
		} else {
			return maxDist;
		}
	}
	return t;
}


std::pair<LatLonAlt,Velocity> KinematicsLatLon::gsAccel(const LatLonAlt& so, const Velocity& vo,  double t, double a) {
	double dist = vo.gs()*t + 0.5*a*t*t;
	double currentTrk = vo.trk();
	LatLonAlt sn = GreatCircle::linear_initial(so, currentTrk, dist);
	sn = sn.mkAlt(so.alt() + vo.z*t);
	double vnGs = vo.gs() + a*t;
	Velocity vn = vo.mkGs(vnGs);
	//fpln(" $$$$$ gsAccel: sn = "+sn+" vn = "+vn);
	return std::pair<LatLonAlt,Velocity>(sn,vn);
}


std::pair<LatLonAlt,Velocity> KinematicsLatLon::gsAccelUntil(const LatLonAlt& so, const Velocity& vo, double t, double goalGS, double gsAccel_d) {
	if (gsAccel_d < 0 ) {
		fpln("Kinematics::gsAccelUntil: user supplied negative gsAccel!!");
		gsAccel_d = -gsAccel_d;                              // make sure user supplies positive value
	}
	double accelTime = Kinematics::gsAccelTime(vo,goalGS,gsAccel_d);
	int sgn = 1;
	if (goalGS < vo.gs()) sgn = -1;
	double a = sgn*gsAccel_d;
	std::pair<LatLonAlt, Velocity> nsv = gsAccel(so, vo, accelTime, a);
	if (t<=accelTime) return gsAccel(so, vo, t, a);
	else  return gsAccel(nsv.first, nsv.second, t-accelTime, 0);
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::gsAccelUntil(std::pair<LatLonAlt,Velocity> sv0, double t, double goalGs, double gsAccel) {
	return gsAccelUntil(sv0.first, sv0.second, t, goalGs, gsAccel);
}

// ****************************** Vertical Speed KINEMATIC CALCULATIONS *******************************

std::pair<LatLonAlt,Velocity> KinematicsLatLon::vsAccel(const LatLonAlt& so, const Velocity& vo,  double t, double a) {
	double dist = vo.gs()*t;
	double currentTrk = vo.trk();
	LatLonAlt sn = GreatCircle::linear_initial(so, currentTrk, dist);
	double nsz = so.alt() + vo.z*t + 0.5*a*t*t;
	sn = sn.mkAlt(nsz);
	Velocity  vn = vo.mkVs(vo.z + a*t);
	return std::pair<LatLonAlt,Velocity>(sn,vn);
}

std::pair<LatLonAlt,Velocity>  KinematicsLatLon::vsAccel(std::pair<LatLonAlt,Velocity> svo,  double t, double a) {
	return vsAccel(svo.first, svo.second, t, a);
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::vsAccelUntil(const LatLonAlt& so, const Velocity& vo, double t, double goalVs, double vsAccel_d) {
	if (vsAccel_d < 0 ) {
		fpln("Kinematics::vsAccelUntil: user supplied negative vsAccel!!");
		vsAccel_d = -vsAccel_d;                              // make sure user supplies positive value
	}
	double accelTime = Kinematics::vsAccelTime(vo,goalVs, vsAccel_d);
	int sgn = 1;
	if (goalVs < vo.vs()) sgn = -1;
	//LatLonAlt ns = LatLonAlt.ZERO;
	if (t <= accelTime)
		return vsAccel(so, vo, t, sgn*vsAccel_d);
	else {
		LatLonAlt posEnd = vsAccel(so,vo,accelTime,sgn*vsAccel_d).first;
		Velocity nvo = Velocity::mkVxyz(vo.x,vo.y, goalVs);
		return linear(posEnd,nvo,t-accelTime);
	}
}

std::pair<LatLonAlt,Velocity> KinematicsLatLon::vsAccelUntil(std::pair<LatLonAlt,Velocity> sv0, double t, double goalVs, double vsAccel) {
	return vsAccelUntil(sv0.first, sv0.second,t,goalVs, vsAccel);
}

std::pair<LatLonAlt, Velocity> KinematicsLatLon::vsLevelOutCalculation(std::pair<LatLonAlt,Velocity> sv0,
		double targetAlt, double a1, double a2, double t1, double t2, double t3,  double t) {
	LatLonAlt s0 = sv0.first;
	Velocity v0 = sv0.second;
	double soz = s0.alt();
	double voz = v0.z;
	std::pair<double, double> vsL = Kinematics::vsLevelOutCalc(soz,voz, targetAlt, a1, a2, t1, t2, t3, t);
	double nz = vsL.first;
	double nvs = vsL.second;
	Velocity nv = v0.mkVs(nvs);
	LatLonAlt ns = linear(s0,v0,t).first.mkAlt(nz);
	return std::pair<LatLonAlt, Velocity>(ns,nv);
}

std::pair<LatLonAlt, Velocity> KinematicsLatLon::vsLevelOut(std::pair<LatLonAlt, Velocity> sv0, double t, double climbRate,
		double targetAlt, double accelUp, double accelDown, bool allowClimbRateChange) {
	Tuple5<double,double,double,double,double> LevelParams = Kinematics::vsLevelOutTimes(sv0.first.alt(), sv0.second.vs(), climbRate, targetAlt, accelUp, accelDown, allowClimbRateChange);
	return vsLevelOutCalculation(sv0, targetAlt, LevelParams.fourth, LevelParams.fifth, LevelParams.first, LevelParams.second, LevelParams.third, t);
}

std::pair<LatLonAlt, Velocity> KinematicsLatLon::vsLevelOut(std::pair<LatLonAlt, Velocity> sv0, double t, double climbRate,
		double targetAlt, double a, bool allowClimbRateChange) {
	return vsLevelOut(sv0, t, climbRate, targetAlt, a, -a, allowClimbRateChange);
}

std::pair<LatLonAlt, Velocity> KinematicsLatLon::vsLevelOut(std::pair<LatLonAlt, Velocity> sv0, double t, double climbRate,
		double targetAlt, double a) {
	return vsLevelOut(sv0, t, climbRate, targetAlt, a, -a, true);
}


}

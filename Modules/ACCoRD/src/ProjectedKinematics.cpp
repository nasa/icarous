/*
 * Kinematics.cpp
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h"
#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "Projection.h"
#include "ProjectedKinematics.h"
#include "KinematicsLatLon.h"
#include "StateVector.h"
#include "GreatCircle.h"
#include "Kinematics.h"
#include "Constants.h"
#include "VectFuns.h"
#include "Triple.h"
#include <cmath>


namespace larcfm {

std::pair<Position,Velocity> ProjectedKinematics::linear(std::pair<Position,Velocity> p, double t) {
	return linear(p.first, p.second, t);
}

std::pair<Position,Velocity> ProjectedKinematics::linear(const Position& so, const Velocity& vo, double t) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	Vect3 ns = s3.linear(vo,t);
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(ns,vo,true);
	} else {
		return std::pair<Position,Velocity>(Position(ns),vo);
	}
}



/**
 * Calculate the angle of a constant-radius turn from two points and the radius
 */
double ProjectedKinematics::turnAngle(Position s1, Position s2, double R) {
	double distAB = s1.distanceH(s2);
	return 2*(std::asin(distAB/(2*R)));
}

/**
 * Horizontal distance covered in a turn
 */
double ProjectedKinematics::turnDistance(Position s1, Position s2, double R) {
	return turnAngle(s1,s2,R)*R;
}


/**
 * Given two points on a turn and the velocity (direction) at the first point, determine the direction for the shortest turn going through the second point,
 * returning true if that relative direction is to the right
 */
bool ProjectedKinematics::clockwise(Position s1, Velocity v1, Position s2) {
	double trk1 = v1.trk();
	double trk2;
	if (s1.isLatLon()) {
		trk2 = GreatCircle::velocity_initial(s1.lla(), s2.lla(), 1).trk();
	} else {
		trk2 = s2.point().Sub(s1.point()).vect2().trk();
	}
	return Util::clockwise(trk1, trk2);
}


///**
// * Turn velocity at point s1, given addition position s2 on turn at relative time t
// * @param s1 position to take tangent
// * @param s2 additional position on turn
// * @param R radius
// * @param t time to get from s1 to s2 (negative if s1 after s2)
// * @return velocity at point s1
// */
//Velocity ProjectedKinematics::turnVelocity(Position s1, Position s2, Position C, double t) {
//	  double R = s1.distanceH(C);
//	  double gs = turnDistance(s1,s2,R)/t;
//	  double vs = (s2.z() - s1.z())/t;
//	  double trk = s1.track(C); // work from this due to potential GC issues
//	  double trk1 = C.track(s1);
//	  double trk2 = C.track(s2);
//	  bool clockwise = Util::clockwise(trk1, trk2);
//	  if (t < 0) {
//		  clockwise = !clockwise;
//	  }
//	  if (clockwise) {
//		  trk = Util::to_2pi(trk - Pi/2);
//	  } else if (clockwise) {
//		  trk = Util::to_2pi(trk + Pi/2);
//	  } else if (t < 0) {
//
//	  }
//	  return Velocity::mkTrkGsVs(trk, gs, vs);
//}

double ProjectedKinematics::closestTimeOnTurn(const Position& turnstart, const Velocity& v1, double omega, const Position& center, const Position& x, double endTime) {
    Vect3 s3 = turnstart.point();
    Vect3 x3 = x.point();
    if (turnstart.isLatLon()) {
//	      EuclideanProjection proj = Projection::createProjection(center.lla().zeroAlt());
//	      s3 = proj.project(turnstart);
//	      x3 = proj.project(x);
//	      v1 = proj.projectVelocity(turnstart, v1);
    	return KinematicsLatLon::closestTimeOnTurn(turnstart.lla(), v1, omega, x.lla(), endTime);
    }
    return Kinematics::closestTimeOnTurn(s3, v1, omega, x3, endTime);
}

double ProjectedKinematics::closestDistOnTurn(const Position& turnstart, const Velocity& v1, double R, int dir, const Position& center, const Position& x, double endDist) {
    Vect3 s3 = turnstart.point();
    Vect3 x3 = x.point();
    if (turnstart.isLatLon()) {
    	return KinematicsLatLon::closestDistOnTurn(turnstart.lla(), v1, R, dir, x.lla(), endDist);
    }
    return Kinematics::closestDistOnTurn(s3, v1, R, dir, x3, endDist);
}

std::pair<Position,Velocity> ProjectedKinematics::turn(const Position& so, const Velocity& vo, double t, double R, bool turnRight) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	std::pair<Vect3,Velocity> resp = Kinematics::turn(std::pair<Vect3,Velocity>(s3,vo),t,R,turnRight);
	Vect3 pres = resp.first;
	Velocity vres = resp.second;
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}


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
std::pair<Position,Velocity> ProjectedKinematics::turnOmega(const Position& so, const Velocity& vo, double t, double omega) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	std::pair<Vect3,Velocity> resp = Kinematics::turnOmega(s3,vo,t,omega);
	Vect3 pres = resp.first;
	Velocity vres = resp.second;
	//f.pln("Kin.turnOmega t = "+t+" so = "+so.toString4()+"  vo = "+vo+" omega = "+omega);
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}

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
std::pair<Position,Velocity> ProjectedKinematics::turnOmega(const Position& so, const Velocity& vo, double t, double omega, const EuclideanProjection& proj) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = proj.project(so);
	}
	std::pair<Vect3,Velocity> resp = Kinematics::turnOmega(s3,vo,t,omega);
	Vect3 pres = resp.first;
	Velocity vres = resp.second;
	//f.pln("Kin.turnOmega t = "+t+" so = "+so.toString4()+"  vo = "+vo+" omega = "+omega);
	if (so.isLatLon()) {
		return proj.inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}

std::pair<Position,Velocity> ProjectedKinematics::turnUntil(const Position& so, const Velocity& vo, double t, double goalTrack, double bankAngle) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	//Vect3 pres = Kinematics.turnUntilPosition(s3,vo,goalTrack, bankAngle,t,turnRight);
	//Velocity vres = Kinematics.turnUntilVelocity(vo,goalTrack, bankAngle,t,turnRight);
	std::pair<Vect3,Velocity> svres = Kinematics::turnUntil(s3, vo, t, goalTrack, bankAngle);
	Vect3 pres = svres.first;
	Velocity vres = svres.second;
	//f.pln("Kin.turnProjection so = "+so+" pres = "+pres+" vo = "+vo+" vres=  "+vres);
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}

std::pair<Position,Velocity> ProjectedKinematics::turnUntil(std::pair<Position,Velocity> sv, double t, double goalTrack, double bankAngle) {
	return turnUntil(sv.first, sv.second,t, goalTrack, bankAngle);
}



std::pair<Position,Velocity> ProjectedKinematics::gsAccel(const Position& so, const Velocity& vo, double t, double a) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	Vect3 pres = Kinematics::gsAccelPos(s3,vo,t,a);
	Velocity vres = Velocity::mkTrkGsVs(vo.trk(),vo.gs()+a*t,vo.vs());
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}

std::pair<Position,Velocity> ProjectedKinematics::gsAccelUntil(const Position& so, const Velocity& vo, double t, double goalGs, double a) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	std::pair<Vect3,Velocity> gsuPair = Kinematics::gsAccelUntil(s3,vo,t,goalGs,a);
	Vect3 pres =  gsuPair.first;
	Velocity vres = gsuPair.second;
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}


std::pair<Position,Velocity> ProjectedKinematics::vsAccel(const Position& so, const Velocity& vo, double t, double a) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	Vect3 pres = Kinematics::vsAccelPos(s3,vo,t, a);
	Velocity vres = Velocity::mkVxyz(vo.x, vo.y, vo.z+a*t);
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}

std::pair<Position,Velocity> ProjectedKinematics::vsAccelUntil(const Position& so, const Velocity& vo, double t, double goalVs, double a) {
	Vect3 s3 = so.point();
	if (so.isLatLon()) {
		s3 = Projection::createProjection(so.lla().zeroAlt()).project(so);
	}
	std::pair<Vect3,Velocity> vsuPair = Kinematics::vsAccelUntil(s3,vo,t,goalVs,a);
	Vect3 pres = vsuPair.first;
	Velocity vres = vsuPair.second;
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(pres,vres,true);
	} else {
		return std::pair<Position,Velocity>(Position(pres), vres);
	}
}



// if this fails, it returns a NaN time
std::pair<Position,double> ProjectedKinematics::intersection(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi) {
	Vect3 so3 = so.point();
	Vect3 si3 = si.point();
	EuclideanProjection proj = Projection::createProjection(so.lla().zeroAlt());
	if (so.isLatLon()) {
		so3 = proj.project(so);
		si3 = proj.project(si);
	}
	std::pair<Vect3,double> intersect = VectFuns::intersection(so3, vo, si3, vi);
	if (so.isLatLon()) {
		return std::pair<Position,double>(Position(proj.inverse(intersect.first)),intersect.second);
	} else {
		return std::pair<Position,double>(Position(intersect.first),intersect.second);
	}
}


double ProjectedKinematics::timeOfintersection(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi) {
	Vect3 so3 = so.point();
	Vect3 si3 = si.point();
	EuclideanProjection proj = Projection::createProjection(so.lla().zeroAlt());
	if (so.isLatLon()) {
		so3 = proj.project(so);
		si3 = proj.project(si);
	}
	double  intersectTime = VectFuns::timeOfIntersection(so3, vo, si3, vi);
	return intersectTime;
}










/** Wrapper around Kinematic.turnTimeDirecTo()
 * Returns a triple: end of turn point, velocity at that point, time at that point
 */
Quad<Position,Velocity,double,int> ProjectedKinematics::directToPoint(const Position& so, const Velocity& vo, const Position& wp, double R) {
	Vect3 s3 = so.point();
	Vect3 g3 = wp.point();
	EuclideanProjection proj = EuclideanProjection();
	if (so.isLatLon()) {
		proj = Projection::createProjection(so.lla().zeroAlt());
		s3 = proj.project(so);
		g3 = proj.project(wp);
	}
	Quad<Vect3,Velocity,double,int> dtp = Kinematics::directToPoint(s3,vo,g3,R);
	std::pair<Position,Velocity> pv;
	if (so.isLatLon()) {
		pv = proj.inverse(dtp.first,dtp.second,true);
	} else {
		pv = std::pair<Position,Velocity>(Position(dtp.first), dtp.second);
	}
	return Quad<Position,Velocity,double,int> (pv.first, pv.second,dtp.third,dtp.fourth);
}



/** Wrapper around Kinematic.genDirectToVertex
 *  Returns the vertex point (in a linear plan sense) between current point and directTo point.
 *
 * @param so     current position
 * @param vo     current velocity
 * @param wp     first point (in a flight plan) that you are trying to connect to
 * @param bankAngle  turn bank angle
 * @param timeBeforeTurn   time to continue in current direction before beginning turn
 * @return (so,t0,t1) vertex point and delta time to reach the vertex point and delta time (from so) to reach end of turn
 *  If no result is possible this will return an invalid position and negative times.
 */
Triple<Position,double,double> ProjectedKinematics::genDirectToVertex(const Position& sop, const Velocity& vo, const Position& wpp, double bankAngle, double timeBeforeTurn) {
	Vect3 s3 = sop.point();
	Vect3 g3 = wpp.point();
	EuclideanProjection proj = EuclideanProjection();
	if (sop.isLatLon()) {
		proj = Projection::createProjection(sop.lla().zeroAlt());
		s3 = proj.project(sop);
		g3 = proj.project(wpp);
	}
	Triple<Vect3,double,double> vertTriple = Kinematics::genDirectToVertex(s3,vo,g3,bankAngle,timeBeforeTurn);
	if (vertTriple.third < 0) return Triple<Position,double,double>(Position::INVALID(), -1.0, -1.0);
	//f.pln(" $$$ genDirectToVertex: vertPair.second = "+vertTriple.second);
	Vect3 vertex = vertTriple.first;
	//Vect3 eot = vertTriple.third;
	Position pp;
	if (sop.isLatLon()) {
		pp = Position(proj.inverse(vertex));
		//eotp = Position(proj.inverse(eot));
	} else {
		pp = Position(vertex);
		//eotp = Position(eot);
	}
	return Triple<Position,double,double>(pp,vertTriple.second,vertTriple.third);
}

Triple<Position,Velocity,double> ProjectedKinematics::vsLevelOutFinal(const Position& so, const Velocity& vo, double climbRate, double targetAlt, double a) {
	if (climbRate == 0) {
		return Triple<Position,Velocity,double>(so.mkZ(targetAlt),vo.mkVs(0),0.0);
	} else {
		std::pair<Vect3, Velocity> sv = std::pair<Vect3, Velocity>(so.point(),vo);
		if (so.isLatLon()) {
			sv = Projection::createProjection(so.lla().zeroAlt()).project(so, vo);
		}
		StateVector vat = Kinematics::vsLevelOutFinal(sv, climbRate, targetAlt, a);
		if (vat.t() < 0) return Triple<Position,Velocity,double>(Position::INVALID(), Velocity::INVALIDV(), vat.t());
		if (so.isLatLon()) {
			std::pair<Position,Velocity>p = Projection::createProjection(so.lla().zeroAlt()).inverse(vat.s(),vat.v(),true);
			return Triple<Position,Velocity,double>(p.first, p.second, vat.t());
		} else {
			return Triple<Position,Velocity,double>(Position(vat.s()), vat.v(), vat.t());
		}
	}
}

double ProjectedKinematics::vsLevelOutTime(const Position& so, const Velocity& vo, double climbRate, double targetAlt, double a) {
	std::pair<Vect3, Velocity> sv(so.point(),vo);
	// we don't need horizontal components, so don't need to project
	return Kinematics::vsLevelOutTime(sv, climbRate, targetAlt, a);
}

std::pair<Position,Velocity> ProjectedKinematics::vsLevelOut(const Position& so, const Velocity& vo, double t, double climbRate, double targetAlt, double a) {
	std::pair<Vect3, Velocity> sv = std::pair<Vect3, Velocity>(so.point(),vo);
	if (so.isLatLon()) {
		sv = Projection::createProjection(so.lla().zeroAlt()).project(so, vo);
	}
	std::pair<Vect3,Velocity> vat = Kinematics::vsLevelOut(sv, t, climbRate, targetAlt, a);
	if (so.isLatLon()) {
		return Projection::createProjection(so.lla().zeroAlt()).inverse(vat.first,vat.second,true);
	} else {
		return std::pair<Position,Velocity>(Position(vat.first), vat.second);
	}
}


}

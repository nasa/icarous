/*
 * KinematicsDist.cpp
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h"
#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Kinematics.h"
#include "KinematicsDist.h"
#include "Constants.h"
#include "format.h"
#include <cmath>
#include <float.h>


namespace larcfm {


Vect4 KinematicsDist::minDistBetweenTrk(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi, const Velocity& nvi,
		double bankAngOwn, double stopTime) {
	double minDist =  MAXDOUBLE;
	double minDistH =  MAXDOUBLE;
	double minDistV =  MAXDOUBLE;
	double minT = -1;
	double step = 1.0;
	for (double t = 0; t < stopTime; t = t + step) {
		//Vect3 soAtTm = turnUntilPosition(so, vo, nvo.track(), bankAngOwn, t, turnRightOwn);
		//Vect3 siAtTm = turnUntilPosition(si, vi, nvi.track(), bankAngTraf, t, turnRightTraf);
		std::pair<Vect3,Velocity> psv = Kinematics::turnUntil(so, vo, t, nvo.trk(), bankAngOwn);
		Vect3 soAtTm = psv.first;
		Velocity vown = psv.second;
		std::pair<Vect3,Velocity> psvi = Kinematics::turnUntil(si, vi, t, nvi.trk(), bankAngOwn);
		Vect3 siAtTm = psvi.first;
		Velocity vtraf = psvi.second;
		//fpln(" $$$$ minDistBetweenTrk: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
		double dist = soAtTm.Sub(siAtTm).norm();
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		//fpln(" $$$$ minDistBetweenTrk: t = "+t+"  dist = "+Units.str("nm",dist));
		if (dist < minDist) {               // compute distances at TCA in 3D
			minDist = dist;
			minDistH = distH;
			minDistV = distV;
			minT = t;
		}
		Vect3 s = soAtTm.Sub(siAtTm);
        bool divg = s.dot(vown.Sub(vtraf)) > 0;
       if (divg) break;

	}
	return Vect4(minDistH,minDist,minDistV,minT);
}


/** Minimum distance between two aircraft when BOTH aircraft gs accelerate, compute trajectories up to time stopTime
 *
 * @param so    initial position of ownship
 * @param vo    initial velocity of ownship
 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
 * @param si    initial position of traffic
 * @param vi    initial velocity of traffic
 * @param nvi           target velocity of traffic (i.e. after acceleration maneuver complete)
 * @param gsAccelOwn    ground speed acceleration of the ownship
 * @param gsAccelTraf   ground speed acceleration of the traffic
 * @param stopTime         the duration of the turns
 * @return                 minimum distance data packed in a Vect4
 */
Vect4 KinematicsDist::minDistBetweenGs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,  const Velocity& nvi,
		double gsAccelOwn, double gsAccelTraf, double stopTime) {
	double minDist = MAXDOUBLE;
	double minDistH = MAXDOUBLE;
	double minDistV = MAXDOUBLE;
	//fpln(" $$$$ minDistBetween: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
	double step = 1.0;
	double minT = -1;
	//fpln(" $$$$$$$$$$$$$$$$$$$$ minDistBetweenTrk: step = "+step);
	for (double t = 0; t < stopTime; t = t + step) {
		Vect3 soAtTm = Kinematics::gsAccelUntil(so, vo, t, nvo.gs(), gsAccelOwn).first;
		Vect3 siAtTm = Kinematics::gsAccelUntil(si, vi, t, nvi.gs(), gsAccelTraf).first;
		//fpln(" $$$$ minDistBetweenTrk: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
		double dist = soAtTm.Sub(siAtTm).norm();
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		//fpln(" $$$$ minDistBetweenTrk: dist = "+Units.str("nm",dist));
		if (dist < minDist) {               // compute distances at TCA in 3D
			minDist = dist;
			minDistH = distH;
			minDistV = distV;
			minT = t;
		}
	}
	return Vect4(minDistH,minDist,minDistV,minT);
}

Vect4 KinematicsDist::minDistBetweenGs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
		double gsAccelOwn, double stopTime) {
	double minDist = MAXDOUBLE;
	double minDistH = MAXDOUBLE;
	double minDistV = MAXDOUBLE;
	//fpln(" $$$$ minDistBetween: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
	double step = 1.0;
	double minT = -1;
	//fpln(" $$$$$$$$$$$$$$$$$$$$ minDistBetweenTrk: step = "+step);
	for (double t = 0; t < stopTime; t = t + step) {
		Vect3 soAtTm = Kinematics::gsAccelUntil(so, vo, t, nvo.gs(), gsAccelOwn).first;
		Vect3 siAtTm = si.linear(vi,t);
		//fpln(" $$$$ minDistBetweenTrk: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
		double dist = soAtTm.Sub(siAtTm).norm();
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		//fpln(" $$$$ minDistBetweenTrk: dist = "+Units.str("nm",dist));
		if (dist < minDist) {               // compute distances at TCA in 3D
			minDist = dist;
			minDistH = distH;
			minDistV = distV;
			minT = t;
		}
	}
	return Vect4(minDistH,minDist,minDistV,minT);
}


Vect4 KinematicsDist::minDistBetweenVs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,  const Velocity& nvi,
		double vsAccelOwn, double vsAccelTraf, double stopTime) {
	double minDist = MAXDOUBLE;
	double minDistH = MAXDOUBLE;
	double minDistV = MAXDOUBLE;
	double minT = -1;
	//fpln(" $$$$ minDistBetweenVs: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
	double step = 1.0;
	for (double t = 0; t < stopTime; t = t + step) {
		Vect3 soAtTm = Kinematics::vsAccelUntil(so, vo, t, nvo.vs(), vsAccelOwn).first;
		Vect3 siAtTm = Kinematics::vsAccelUntil(si, vi, t, nvi.vs(), vsAccelTraf).first;
		//fpln(" $$$$ minDistBetweenVs: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
		double dist = soAtTm.Sub(siAtTm).norm();
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		//fpln(" $$$$ minDistBetweenVs: dist = "+Units.str("nm",dist));
		if (dist < minDist) {               // compute distances at TCA in 3D
			minDist = dist;
			minDistH = distH;
			minDistV = distV;
			minT = t;
		}
	}
	return Vect4(minDistH,minDist,minDistV,minT);
}
Vect4 KinematicsDist::minDistBetweenVs(const Vect3& so, const Velocity& vo, const Velocity& nvo, const Vect3& si, const Velocity& vi,
		double vsAccelOwn, double stopTime) {
	double minDist = MAXDOUBLE;
	double minDistH = MAXDOUBLE;
	double minDistV = MAXDOUBLE;
	//fpln(" $$$$ minDistBetween: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
	double step = 1.0;
	double minT = -1;
	for (double t = 0; t < stopTime; t = t + step) {
		Vect3 soAtTm = Kinematics::vsAccelUntil(so, vo, t, nvo.vs(), vsAccelOwn).first;
		Vect3 siAtTm = si.linear(vi,t);
		//fpln(" $$$$ minDistBetweenVs: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
		double dist = soAtTm.Sub(siAtTm).norm();
		double distH = soAtTm.Sub(siAtTm).vect2().norm();
		double distV = std::abs(soAtTm.Sub(siAtTm).z);
		//fpln(" $$$$ minDistBetweenVs: distV = "+Units.str("ft",dist));
		if (dist < minDist) {               // compute distances at TCA in 3D
			minDist = dist;
			minDistH = distH;
			minDistV = distV;
			//fpln(" $$$$ minDistBetweenVs: minDistV = "+Units.str("ft",minDistV));
			minT = t;
		}
	}
	return Vect4(minDistH,minDist,minDistV,minT);
}

double KinematicsDist::gsTimeConstantAccelFromDist(double gs1, double a, double dist) {
	double t1 = Util::root(0.5*a,  gs1,  -dist, 1);
	double t2 = Util::root(0.5*a,  gs1,  -dist, -1);
	return t1 < 0 ? t2 : t1;
}


}

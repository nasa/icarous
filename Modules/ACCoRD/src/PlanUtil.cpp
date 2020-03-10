
/*
 * PlanUtil.cpp - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "PlanUtil.h"
#include "Plan.h"
#include "PlanWriter.h"
#include "Projection.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "KinematicsPosition.h"
#include "Kinematics.h"
#include "TrajGen.h"
#include "Velocity.h"
#include "Vect3.h"
#include "format.h"
#include "Util.h"
#include "Constants.h"
#include "string_util.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <float.h>
#include "TcpData.h"

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;

const double PlanUtil::maxTurnDeltaStraight = Units::from("deg",1.0);

const double PlanUtil::maximumInterpolationNumber = 900;	// maximum number of legs that a plan should have after interpolation
const double PlanUtil::minimumInterpolationLegSize = 1000.0;  // do not allow interpolations to create legs smaller than this (can override maximumInterpolationNumber)


int PlanUtil::prevTrackChange(const Plan& fp, int iNow) {
	//fpln(" $$ prevTrackChange: iNow = "+Fm0(iNow));
	for (int i = iNow-1; i > 0; i--) {
		if (fp.isEOT(i)) return i;
		double gsIn_i = fp.gsIn(i);
		//fpln(" $$ prevTrackChange: i = "+i+" gsIn_i = "+Units::str("kn",gsIn_i));
        if (Util::almost_equals(gsIn_i,0.0,PRECISION5)) continue;
		double trkIn_i = fp.trkIn(i);
		double trkOut_i = fp.defTrkOut(i);
		double turnDelta = Util::turnDelta(trkIn_i,trkOut_i);
		//f.pln(" $$ prevTrackChange: i = "+i+" turnDelta = "+Units::str("deg",turnDelta));
		//f.pln(" $$ prevTrackChange::  i = "+i+" trkIn_i = "+Units::str("deg",trkIn_i)+" trkOut_i = "+Units::str("deg",trkOut_i));
		if (turnDelta > maxTurnDeltaStraight) return i;
	}
	return 0;
}

int PlanUtil::nextTrackChange(const Plan& fp, int iNow) {
	for (int i = iNow+1; i < fp.size(); i++) {
		if (i == fp.size()-1) return fp.size()-1;
		if (fp.isBOT(i)) return i;
		double gsOut_i = fp.gsOut(i);
        if (Util::almost_equals(gsOut_i,0.0,PRECISION5)) continue;
		double trkIn_i = fp.defTrkIn(i);
		double trkOut_i = fp.trkOut(i);
		//f.pln("nextTrackChange:  i = "+i+" trkIn_i = "+Units::str("deg",trkIn_i)+" trkOut_i = "+Units::str("deg",trkOut_i));
		double turnDelta = Util::turnDelta(trkIn_i,trkOut_i);
		//f.pln(" $$ nextTrackChange: iNow = "+iNow+" turnDelta = "+Units::str("deg",turnDelta));
		if (turnDelta > maxTurnDeltaStraight) return i;
	}
	return fp.size()-1;
}

int PlanUtil::prevVsChange(const Plan& p, int iNow) {
	//f.pln("$$$ prevVsChange: iNow = "+iNow);
	//double vsStart = p.vsOut(iNow-1);
	for (int i = iNow-1; i > 0; i--) {
		if (p.isEVS(i)) return i;
		double vs_im1 = p.vsOut(i-1);
		double vs_i = p.vsOut(i);
		//f.pln("$$$ prevVsChange:  i = "+i+" vs_im1 = "+Units.str("fpm",vs_im1)+" vs_i = "+Units.str("fpm",vs_i));
		double vsDelta = std::abs(vs_i-vs_im1);
		//f.pln(" $$ prevVsChange: i = "+i+" vsDelta = "+Units.str("fpm",vsDelta));
		if (vsDelta > Plan::MIN_VS_DELTA_GEN) return i;
	}
	return 0;

}


int PlanUtil::nextVsChange(const Plan& p, int iNow) {
	//double vsStart = p.vsOut(iNow);
	for (int i = iNow+1; i < p.size(); i++) {
		if (i == p.size()-1) return p.size()-1;
		if (p.isBVS(i)) return i;
		double vsStart = p.vsOut(i-1);
		double vsOut_i = p.vsOut(i);
		//f.pln("nextVsChange:  i = "+i+" trkIn_i = "+Units.str("deg",trkIn_i)+" trkOut_i = "+Units.str("deg",trkOut_i));
		double vsDelta = std::abs(vsStart-vsOut_i);
		//f.pln(" $$ nextVsChange: iNow = "+iNow+" vsDelta = "+Units.str("fpm",vsDelta));
		if (vsDelta > Plan::MIN_VS_DELTA_GEN) return i;
	}
	return p.size()-1;
}


Position PlanUtil::positionFromDistanceInSeg(const Plan& p, double curTm, double advDistance, bool linear) {
	//fpln(" $$ %%%% advanceDistanceInSeg: ENTER: curTm = "+f.Fm4(curTm)+"  advDistance = "+Units::str("NM",advDistance));
	if (curTm < p.getFirstTime() || curTm > p.getLastTime()) {
		fpln(" $$@@ advanceDistanceInSeg:  ERROR: currentTime = "+Fm2(curTm)+" outside of plan!");
	}
	int seg = p.getSegment(curTm);
	//fpln(" $$$ advanceDistanceInSeg: currentTime = "+currentTime+" seg = "+seg);
	double distLeftInSeg0 = p.partialPathDistance(curTm,linear);
	if (Util::almost_equals(advDistance, distLeftInSeg0)) {
		return p.point(seg+1).position();
	}
	if (advDistance > distLeftInSeg0) {
		//fpln(" $$$$ currentTime = "+currentTime+" distFromSo = "+Units::str("NM",advDistance,8)+" distLeftInSeg0 = "+Units::str("NM",distLeftInSeg0,12));
		//fpln(" $$$$ ERROR: advanceDistanceInSeg assumes the advance by distance will not leave current segment!");
		//Debug.halt();
		return Position::INVALID();
	}
	std::pair<Position, Velocity> positionVelocity = p.positionVelocity(curTm,linear);
	Position so = positionVelocity.first;
	//Velocity vo = positionVelocity.second;
	//fpln(" $$ %%%% advanceDistanceInSeg: currentTime = "+f.Fm4(currentTime)+" vo = "+vo);
	Position sNew;
	if (p.inTrkChange(curTm) & !linear) {
		int ixPrevBOT = p.prevBOT(seg+1);//fixed
		Position center = p.turnCenter(ixPrevBOT);
		int dir = p.turnDir(ixPrevBOT);
		double gsAt_d = -1.0;             // THIS IS ONLY USED IN THE VELOCITY CALCULATION WHICH WE ARE NOT USING
		std::pair<Position,Velocity> tAtd = KinematicsPosition::turnByDist2D(so, center, dir, advDistance, gsAt_d);
		sNew = tAtd.first;
		//fpln(" $$ %%%% advanceDistanceInSeg A: sNew("+f.Fm2(currentTime)+") = "+sNew);
	} else {
		//fpln("\n\n $$ %%%% advanceDistanceInSeg B1: currentTime = "+currentTime+" seg = "+seg+"  distFromSo = "+Units::str("NM",distFromSo));
		//double track = vo.trk();  // look into getting track another way
		double track = p.trkOut(seg);
		sNew = so.linearDist2D(track, advDistance);  // does not compute altitude !!
		//fpln(" $$ %%%% advanceDistanceInSeg B2: seg = "+seg+" sNew("+f.Fm2(currentTime)+") = "+sNew);
	}
	double segDistance = p.pathDistance(seg);
	double deltaAlt = p.point(seg+1).alt() - p.point(seg).alt();
	double sZ = so.alt() + advDistance/segDistance*deltaAlt;
	//fpln(" $$ %%%% advanceDistanceInSeg B1: curTm = "+curTm+" sZ = "+Units::str("ft",sZ)+" deltaAlt = "+Units::str("fpm",deltaAlt));
	sNew = sNew.mkAlt(sZ);
	return sNew;
}



std::pair<Position,int> PlanUtil::positionFromDistance(const Plan& p, double currentTime, double advanceDist, bool linear) {
	Position sNew;
	int initSeg = p.getSegment(currentTime);
	//fpln(" $$::::::::::: advanceDistance AA: initSeg = "+initSeg+" p.size = "+p.size()+"  advanceDist = "+Units::str("NM",advanceDist));
	double distLeftInSeg0 = p.partialPathDistance(currentTime,linear);
	//fpln("distLeftInSeg0="+distLeftInSeg0);
	int finalSeg = initSeg;
	//fpln(" $$::::::::::: advanceDistance BB: initSeg = "+initSeg+"  distLeftInSeg0 = "+Units::str("NM",distLeftInSeg0));
	if (advanceDist < distLeftInSeg0) {  //  new position remains in segment "seg"
		//fpln(" $$::::::::::: advanceDistance 00000000: initSeg = "+initSeg+"  advanceDist = "+Units::str("NM",advanceDist)+" distLeftInSeg0 = "+Units::str("NM",distLeftInSeg0));
		sNew = positionFromDistanceInSeg(p, currentTime, advanceDist, linear);
		//fpln(" $$ advanceDistance A: sNew = "+sNew);
	} else {
		double distSofar = distLeftInSeg0;
		for (finalSeg = initSeg+1; finalSeg < p.size(); finalSeg++) {
			double nextDistSoFar = distSofar + p.pathDistance(finalSeg,finalSeg+1,linear);
			//fpln("advanceDist="+advanceDist+" nextDistSoFar="+nextDistSoFar+" totalDist="+p.pathDistance());
			if (nextDistSoFar > advanceDist) {
				break;
			}
			distSofar = nextDistSoFar;
		}
		double remainingDist = advanceDist - distSofar;
		//fpln(" $$ advanceDistance B1: remainingDist = "+Units::str("NM",remainingDist));
		// need to go remainingDist in segment j
		if (finalSeg >= p.size()-1) {
			return std::pair<Position,int>(p.getLastPoint().position(),p.size()-1);
		} else {
			double t0 = p.time(finalSeg);
			//fpln(" $$ advanceDistance B2: j = "+j+" t0 = "+t0+" remainingDist = "+Units::str("NM",remainingDist));
			sNew = positionFromDistanceInSeg(p, t0, remainingDist, linear);
			//fpln(j+" $$ advanceDistance B3: sNew = "+sNew);
		}
	}
	return std::pair<Position,int>(sNew,finalSeg);
}



/** time required to cover distance "dist" if initial speed is "gsInit" and acceleration is "gsAccel"
 *
 * @param gsAccel   ground speed acceleration
 * @param gsInit    initial ground speed
 * @param dist      distance travelled
 * @return
 */
double PlanUtil::timeFromGs(double gsInit, double gsAccel, double dist) {
	double t1 = Util::root(0.5*gsAccel, gsInit, -dist, 1);
	double t2 = Util::root(0.5*gsAccel, gsInit, -dist, -1);
	double dt = ISNAN(t1) || t1 < 0 ? t2 : (ISNAN(t2) || t2 < 0 ? t1 : Util::min(t1, t2));
	return dt;
}




Plan PlanUtil::applyWindField(const Plan& pin, const Velocity& v) {
	Plan p = pin;
	NavPoint np0 = p.point(0);
	Velocity v0 = p.initialVelocity(0);
	for (int j = 1; j < p.size(); j++) {
		Vect3 vsub = v0.Sub(v);
		Velocity v1 = Velocity::make(vsub);
		NavPoint np1 = p.point(j);
		double dt = np1.time()-np0.time();
		NavPoint np2 = np1.makePosition(np0.linear(v1, dt).position());
		np0 = np1;
		p.setNavPoint(j, np2);
	}
	return p;
}

bool PlanUtil::isCurrentPositionUnchanged(const Plan& solution, double currentTime, const Position& currentPos) {
	bool ok = true;
	if (!solution.isWellFormed()) {
		std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
		std::cout << " ............... ERROR: solution is not wellFormed currentTime = "<<currentTime<<" ...." << std::endl;
		std::cout << " ..............."<<solution.strWellFormed() << std::endl;
		std::cout << " mySolution = "<<solution.toString() << std::endl;
		std::cout << "----------------------------------------------------------------------------------\n" << std::endl;
		ok = false;
	}
	if (!solution.position(currentTime).almostEquals(currentPos)) {
		std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
		std::cout << " ............... ERROR: moved location of current position! currentTime = "<<currentTime<<" ......." << std::endl;
		std::cout << " ............... from " << currentPos.toString() <<" to "<<solution.position(currentTime).toString()<<std::endl;
		std::cout << "----------------------------------------------------------------------------------\n" << std::endl;
		ok = false;
	}
	return ok;
}

bool PlanUtil::checkMySolution(const Plan& solution, double currentTime, const Position& currentPos, const Velocity& currentVel) {
	bool ok = isCurrentPositionUnchanged(solution, currentTime, currentPos);
	if (!solution.velocity(currentTime).within_epsilon(currentVel,0.10)) {
		std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
		std::cout << " ............... ERROR: changed **velocity** of current position! currentTime = "<<currentTime<<" ...." << std::endl;
		std::cout << " ............... from " << currentVel.toString() << " to " << solution.velocity(currentTime).toString()<<std::endl;
		std::cout << "----------------------------------------------------------------------------------\n" << std::endl;
		ok = false;
	}
	return ok;
}


double PlanUtil::distanceBetween(const Plan& A,const Plan& B) {
	double rtn = 0;
	rtn = std::abs(A.getFirstTime() - B.getFirstTime());
	rtn = rtn + std::abs(A.getLastTime() - B.getLastTime());
	double maxStart = std::max(A.getFirstTime(), B.getFirstTime());
	double minEnd   = std::min(A.getLastTime(), B.getLastTime());
	double step = (minEnd - maxStart)/20;
	for (double t = maxStart; t <= minEnd; t = t + step) {
		double errH = A.position(t).distanceH(B.position(t));
		double errV = A.position(t).distanceV(B.position(t));
		//f.pln(" $$$$$ distanceBetween t = "+t+" errH = "+f.Fm2(errH)+" errV = "+f.Fm2(errV));
		rtn = rtn + errH + errV;
	}
	return rtn;
}



// from Aviation Formulary
// longitude sign is reversed from the formulary!
double PlanUtil::lonCross(const Plan& ac, int i, double lat3) {
	double lat1 = ac.point(i).lat();
	double lon1 = ac.point(i).lon();
	double lat2 = ac.point(i+1).lat();
	double lon2 = ac.point(i+1).lon();
	double lon = GreatCircle::lonCross(lat1, lon1, lat2, lon2, lat3);
	return lon;
}


// from Aviation Formulary
double PlanUtil::latMax(const Plan& ac, int i) {
	double tc = ac.initialVelocity(i).compassAngle();
	double lat = ac.point(i).lat();
	double ret = Util::acos_safe(std::abs(std::sin(tc)*std::cos(lat)));
	if (tc > Pi/2 && tc < 3*Pi/2) ret = -ret;
	return ret;
}

int PlanUtil::addLocalMaxLat(Plan& ac, int i) {
	bool t1 = std::abs(ac.initialVelocity(i).trk()) > Pi/2;
	bool t2 = std::abs(ac.finalVelocity(i).trk()) > Pi/2;
	if (t1 == t2) {
		return i+1;
	} else {
		double lat1 = std::abs(ac.point(i).lat());
		double lon1 = std::abs(ac.point(i).lon());
		double lat2 = latMax(ac,i);
		double lon2 = lonCross(ac,i,lat2);
		double dist = GreatCircle::distance(lat1,lon1,lat2,lon2);
		double gs = ac.initialVelocity(i).gs();
		double t = ac.time(i) + dist/gs;
		if (t < ac.time(i+1) && t > ac.time(i)) {
			//    	  error.addError("addLocalMaxLat time out of bounds");
			insertVirtual(ac,t);
			ac.setNavPoint(i+1, ac.point(i+1).makeName("maxlat")
			);
			return i+2;
		} else return i+1;
	}
}

void PlanUtil::insertLocalMax(Plan& ac) {
	int i = 0;
	while (i < ac.size()-1) {
		i = addLocalMaxLat(ac,i);
	}
}


int PlanUtil::insertVirtual(Plan& ac, double time) {
	if (time >= ac.getFirstTime() && (time <= ac.getLastTime()) && ac.getIndex(time) < 0) {
		TcpData tcp = ac.getTcpData(ac.getSegment(time));
		NavPoint np(ac.position(time), time); //src.makeStandardRetainSource().makePosition(ac.position(time)).makeTime(time);
		tcp.reset();
		tcp.setVirtual();
		return ac.add(np, tcp);
	}
	else return -1;
}



double PlanUtil::getLegDist(const Plan& ac, int i, double accuracy, double mindist) {
	double lat = Util::max(std::abs(ac.point(i).lat()),
			std::abs(ac.point(i+1).lat()));
	double maxdist = Projection::projectionConflictRange(lat, accuracy);
	// necessary due to hard limits on number of wp
	if (maxdist < mindist) maxdist = mindist;
	return maxdist;
}



Plan PlanUtil::revertAllTCPs(const Plan& pln) {
	bool markIndices = false;
	return revertAllTCPs(pln,markIndices);
}

Plan PlanUtil::revertAllTCPs(const Plan& pln, bool markIndices) {
    //fpln(" $$$$$$ revertAllTCPs: ENTER revertVsTCPs, pln = "+pln.toStringGs());
	Plan rtn = Plan(pln);
	if (markIndices) {
		for (int i = 0; i < rtn.size(); i++) {
			rtn.appendInfo(i, ":ksrc-"+Fmi(i)+":");
		}
	}
    //fpln(" $$$$$$ revertAllTCPs: AFTER markIndices, rtn = "+rtn.toStringGs());
	//double vsAccel = findVsAccel(rtn);
	rtn.revertVsTCPs();
    //fpln(" $$$$$$ revertAllTCPs: AFTER revertVsTCPs, rtn = "+rtn.toStringGs());
 	rtn = setAltPreserveByDelta(rtn);
    //fpln(" $$$$$$ revertAllTCPs: AFTER setAltPreserveByDelta, rtn = "+rtn.toStringGs());
	rtn.revertGsTCPs();
    //fpln(" $$$$$$ revertAllTCPs: AFTER revertGsTCPs, rtn = "+rtn.toStringGs());
	rtn.revertTurnTCPs();
    //fpln(" $$$$$$ revertAllTCPs: AFTER revertTurnTCPs, rtn = "+rtn.toStringGs());
	rtn = TrajGen::makeMarkedVsConstant(rtn);
	rtn.cleanPlan();
	return rtn;
}

Plan PlanUtil::setAltPreserveByDelta(const Plan& lpc) {
	//fpln(" $$$ setAltPreserveByDelta: ENTER, lpc = "+lpc.toStringTrk());
	Plan kpc = Plan(lpc.getID(),lpc.getNote());
	kpc.add(lpc.get(0)); // first point need not be marked
	for (int i = 1; i < lpc.size()-1; i++) {
		NavPoint np_i = lpc.point(i);
		TcpData tcp_i = lpc.getTcpData(i);
		//fpln(" $$>> setAltPreserveByDelta: i = "+Fm0(i)+ " tcp_i = "+tcp_i.toString());
		double vs1 = lpc.vsOut(i-1);
		double vs2 = lpc.vsOut(i);
		//fpln(" $$>> setAltPreserveByDelta: vs1 = "+Units::str("fpm",vs1)+" vs2 = "+Units::str("fpm",vs2));
		double deltaVs = vs1 - vs2;
		if (std::abs(deltaVs) > 0.1*Plan::MIN_VS_DELTA_GEN) { // || deltaTm >= TrajGen.MIN_MARK_LEG_TIME) {
            //fpln(" $$$$ setAltPreserveByAccel: SET i = "+i+" AltPreserve!!!!");
			tcp_i.setAltPreserve();
		}
		kpc.add(np_i,tcp_i);
	}
	kpc.add(lpc.get(lpc.size()-1)); // last point need not be marked
	return kpc;
}

 double PlanUtil::findVsAccel(const Plan& kpc) {
	double vsAccel_d = 2.0;
	int ix = kpc.nextBVS(0);
	if (ix >= 0) {
		vsAccel_d = kpc.vsAccel(ix);
	}
    return vsAccel_d;
}



// this adds for the leg starting at or before startTm, and ending before or at endTm
void PlanUtil::interpolateVirtuals(Plan& ac, double accuracy, double startTm, double endTm) {
	if(ac.isLatLon()) {
		int origSz = ac.size();
		if (startTm < ac.getFirstTime()) startTm = ac.getFirstTime();
		if (endTm > ac.getLastTime()) endTm = ac.getLastTime();
		insertLocalMax(ac);
		int i = ac.getSegment(startTm);
		double mindist = std::max(minimumInterpolationLegSize, ac.pathDistance() / maximumInterpolationNumber); // lower bound on distance between virtuals
		while (i < ac.size()-1 && ac.time(i) <= endTm) {
			double dist = ac.pathDistance(i);
			double legDist = getLegDist(ac,i,accuracy,mindist);
			//Special case for kinematic plans in acceleration zones -- ensure there are at least 2 mid-points
			if (dist > legDist) {
				double gs = ac.averageVelocity(i).gs();
				int j = i;
				double tmIncr = legDist/gs;
				if (std::abs(ac.point(i).lat()) >= std::abs(ac.point(i+1).lat())) {
					double nextT = ac.time(i+1);
					double t = ac.time(i);
					while (t + tmIncr + Constants::TIME_LIMIT_EPSILON < nextT) {
						//	    fpln("$$## add point for ac "+ac+" at time "+t);
						t += tmIncr;
						insertVirtual(ac,t);
						legDist = getLegDist(ac,j,accuracy,mindist);
						j++;
						tmIncr = legDist/gs;
					}
				} else {
					double thisT = ac.time(i);
					double t = ac.time(i+1);
					while (t - tmIncr - Constants::TIME_LIMIT_EPSILON > thisT) {
						//	    fpln("$$## add point for ac "+ac+" at time "+t);
						t -= tmIncr;
						insertVirtual(ac,t);
						legDist = getLegDist(ac,i,accuracy,mindist);
						j++;
						tmIncr = legDist/gs;
					}
				}
				i = Util::max(i,j-1); // possibly take back last increment
			}
			i++;
		}
		//		modified = true;
		if (ac.size() > 50 && (ac.size() > origSz*4 || ac.size() >= maximumInterpolationNumber)) ac.addWarning("PlanUtil.interpolateVirtuals has increased plan size from "+Fm0(origSz)+" to "+Fm0(ac.size()));
	}
}

void PlanUtil::interpolateVirtuals(Plan& ac, double accuracy) {
	interpolateVirtuals(ac, accuracy, ac.getFirstTime(), ac.getLastTime());
}




// this removes all virtuals AFTER time startTm and BEFORE endTm.
// this will NOT remove any Fixed virtuals!
// returns TRUE if all virtuals removed, otherwise FALSE
bool PlanUtil::removeVirtualsRange(Plan& ac, double startTm, double endTm, bool all) {
	int i = ac.getSegment(startTm)+1;
	bool rtn = true;
	while (i < ac.size() && ac.time(i) < endTm) {
		if (ac.isVirtual(i) || all) {
			ac.remove(i);
		} else {
			i++;
		}
	}
	return rtn;
}


bool PlanUtil::removeVirtuals(Plan& ac) {
	return removeVirtualsRange(ac, ac.getFirstTime(), ac.getLastTime(), false);
}

/** Structurally revert all TCPS that create acceleration zones containing ix
 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain
 *  original ground speed into the point after ix.
 *
 *  NOTE This method does not depend upon source time!!
 *
 * @param ix  The index of one of the TCPs created together that should be reverted
 * @return index of the reverted point
 */
int PlanUtil::revertGroupOfTCPs(Plan& pln, int ix) {
	if (ix < 0 || ix >= pln.size()) {
		pln.addError(".. structRevertGroupOfTCPs: invalid index "+Fm0(ix), 0);
		return -1;
	}
	NavPoint origPt = pln.point(ix);
	if ( ! pln.isTCP(ix)) return ix;  // nothing to do
	//fpln("$$$ structRevertGroupOfTCPs: point("+ix+") = "+pln.point(ix).toStringFull());
	int firstInGroup = ix;                  // index of first TCP in the group
	int lastInGroup = ix; // pln.size()-1;         // index of the last TCP in the group
	if (firstInGroup == 0) {
		pln.addError(" .. structRevertGroupOfTCPs: cannot remove first point",0);
		return -1;
	}
	if (pln.isVsTCP(ix)) {
		if (pln.isEVS(ix)) {
			int ixBVS = pln.prevBVS(ix);//fixed
			if (ixBVS < firstInGroup) firstInGroup = ixBVS;
			lastInGroup = ix;
		} else {
			int ixEVS = pln.nextEVS(ix);//fixed
			if (ixEVS > lastInGroup) lastInGroup = ixEVS;
			firstInGroup = ix;
		}
		//fpln(" $$$$ structRevertGroupOfTCPs(VS): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
	} else {
		if (pln.isGsTCP(ix)) {
			if (pln.isEGS(ix)) {
				int ixBGS = pln.prevBGS(ix);//fixed
				if (ixBGS >= 0 && ixBGS < firstInGroup) firstInGroup = ixBGS;
				lastInGroup = ix;
			} else {
				int ixEGS = pln.nextEGS(ix);//fixed
				if (ixEGS >= 0 && ixEGS > lastInGroup) lastInGroup = ixEGS;
				firstInGroup = ix;
			}
			//fpln(" $$$$ structRevertGroupOfTCPs(GS): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
		} else { // track
			if (pln.isEOT(ix)) {
				int ixBOT = pln.prevBOT(ix);//fixed
				if (ixBOT >= 0 && ixBOT < firstInGroup) firstInGroup = ixBOT;
				lastInGroup = ix;
			} else {
				int ixEOT = pln.nextEOT(ix);//fixed
				if (ixEOT >= 0 && ixEOT > lastInGroup) lastInGroup = ixEOT;
				firstInGroup = ix;
			}
			//fpln(" $$$$ structRevertGroupOfTCPs(TRK): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
		}
	}
	//fpln(" $$$$ structRevertGroupOfTCPs(FINAL): sz = "+pln.size()+" firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
	// revert vertical TCPs
	double zVertex = -1;
	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
		if (pln.isVsTCP(ii)) {
			zVertex = pln.revertVsTCP(ii);
			if (zVertex >= 0) {
				lastInGroup--;
				//if (ii == firstInGroup) firstInGroup++;
			}
		}
	}
	// get rid of all ground speed TCPs if this is fundamentally a turn
	if (pln.isTrkTCP(firstInGroup)) {
		//bool killNextGsTCPs = true;
		pln.revertTurnTCP(firstInGroup); // , killNextGsTCPs);
	}
	if (pln.isGsTCP(firstInGroup)) {  // does not revert previous turn
		//fpln(" $$$$ structRevertGroupOfTCPs: GS section: firstInGroup = "+firstInGroup);
		bool revertPreviousTurn = true;
		firstInGroup = revertGsTCP(pln,firstInGroup,revertPreviousTurn);
	}
	return firstInGroup;
}



// assumes ix > 0 AND ix < pln.size()
int PlanUtil::revertGsTCP(Plan& pln, int ix, bool revertPreviousTurn) {
	//fpln(" $$$$>>>>>>>>> structRevertGsTCP: pln.point("+ix+") = "+pln.point(ix).toStringFull());
	if (! pln.validIndex(ix)) return -1;
	if (pln.isBGS(ix)) {
		if (revertPreviousTurn) { // && ixPrev >= 0 && pln.isEOT(ixPrev) && pln.getTcpData(ix).getLinearIndex() == prevLinIndex) {
			pln.revertGsTCP(ix);
			//bool killNextGsTCPs = true;
			int ixPrevBOT = pln.prevBOT(ix);
			//fpln(" $$$$$$$$$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
			if (ixPrevBOT >= 0) {
		    	pln.revertTurnTCP(ixPrevBOT); // , killNextGsTCPs);
				return ixPrevBOT;
			}
		} else {
			pln.revertGsTCP(ix);
		}
	}
	return ix;
}


Plan PlanUtil::cutDownLinear(const Plan& plan, double startTime, double endTime) {
	Plan rtn(plan.getID(),plan.getNote());
	int ix = plan.getIndex(startTime);
	NavPoint start;
	TcpData start_tcp;
	if (ix >= 0) {
		start = plan.point(ix);
		start_tcp = plan.getTcpData(ix);
	} else {
		Position startPos = plan.position(startTime);
		start = NavPoint(startPos, startTime);
	}
	rtn.add(start, start_tcp);
	for (int i = 0; i < plan.size(); i++) {
		std::pair<NavPoint,TcpData> pi = plan.get(i);
		if (startTime < pi.first.time() && pi.first.time() < endTime ) {
			rtn.add(pi);
		}
	}
	ix = plan.getIndex(endTime);
	std::pair<NavPoint,TcpData> end;
	if (ix >= 0) {
		end = plan.get(ix);
	} else {
		Position endPos = plan.position(endTime);
		NavPoint endPt(endPos, endTime);
		end = std::pair<NavPoint,TcpData>(endPt,TcpData());
	}
	rtn.add(end);
	return rtn;
}

/**
 *  Cut down a plan to the region between t1 and t2.
 *  The plan is continued linearly to time tExtend.   A new point is created at t1 and t2 if necessary.
 *  The  t1, t2 and tExtend times are absolute.
 *
 * @param plan     Plan file to be cut
 * @param t1       Location to start cut out. If before first point or negative, first point is used
 * @param t2       The absolute lookahead time -- all plan information after this time is not copied
 * @param tExtend  After the intentThreshold, the plan is extended linearly to this time (absolute time)
 * @return         plan cut down to times [t1,t2].
 */
Plan PlanUtil::cutDownTo(const Plan& plan, double t1, double t2, double tExtend) {
	if (plan.size() == 0) return plan;
	Plan nPlan(plan.getID(),plan.getNote());
	if (t2 < t1) {
		return nPlan;
	}
	double lastTime = plan.getLastTime();
	if (t1 < plan.point(0).time()) t1 = plan.point(0).time();
	if (t2 > lastTime) t2 = lastTime;
	int ixInit = plan.getSegment(t1);
	Position startPos = plan.position(t1);
	NavPoint firstPoint(startPos, t1);
	nPlan.addNavPoint(firstPoint);
	TcpData firstPoint_tcp;
	//f.pln(" $$ cutDownTo:  at t1 = "+t1+" "+plan.inTrkChange(t1)+" "+plan.inGsChange(t1)+" "+plan.inVsChange(t1));
	if (plan.inTrkChange(t1)) {
       	int ixBOT = plan.prevBOT(ixInit+1);
       	double signedRadius = plan.signedRadius(ixBOT);
       	Position center = plan.turnCenter(ixBOT);
    	firstPoint_tcp.setBOT(signedRadius,center);
	}
    if (plan.inGsChange(t1)) {
    	int ixBGS = plan.prevBGS(ixInit+1);
    	firstPoint_tcp.setBGS(plan.gsAccel(ixBGS));
    }
     if (plan.inVsChange(t1)) {
    	int ixBVS = plan.prevBVS(ixInit+1);
    	firstPoint_tcp.setBVS(plan.vsAccel(ixBVS));
    }
	nPlan.add(firstPoint,firstPoint_tcp);
	//f.pln(" $$$$0 cutDownTo: ADD INITIAL point("+ixInit+") = "+firstPoint.toString());
	int ix = ixInit + 1;
	while (ix < plan.size() && plan.point(ix).time() < t2) {
		std::pair<NavPoint,TcpData> p = plan.get(ix);
		nPlan.add(p);
		//fpln(" $$$$$$ cutDownTo ADD p = "+p);
		ix++;
	}
	if (ix >= plan.size()) { // plan ran out before intentThreshold
		double dt = t2 - plan.getLastTime();
		Velocity vout = plan.initialVelocity(plan.size()-1);
		NavPoint lastP = plan.point(plan.size()-1);
		NavPoint np = lastP.linear(vout, dt); // .makeName("CutDownTo_0");
		nPlan.addNavPoint(np);
		//fpln(" $$$$$$ cutDownTo ADD np = "+np);
	} else {                // intentThreshold is within plan
		NavPoint newLastPt = NavPoint(plan.position(t2),t2); // .makeName("CutDownTo_newLastPt");
		//Velocity vout = plan.velocity(t2);
		TcpData newLastTcpDatad;
		if (! plan.inAccel(t2)) {
			//fpln(" $$$$$$ cutDownTo ADD newLastPtp = "+newLastPt+" vout = "+vout);
			nPlan.addNavPoint(newLastPt);
		} else {
			//bool inTurn = plan.inTrkChange(t2);
			//bool inGsAccel = plan.inGsChange(t2);
			//bool inVsAccel = plan.inVsChange(t2);
			bool inTurn = nPlan.inTrkAccel(nPlan.size()-1);
			bool inGsAccel = nPlan.inGsAccel(nPlan.size()-1);
			bool inVsAccel = nPlan.inVsAccel(nPlan.size()-1);
			NavPoint lastP(newLastPt.position(), newLastPt.time()); // , "CutDownTo_lastP");
			int ixNP = nPlan.addNavPoint(lastP);
			if (inTurn) nPlan.setEOT(ixNP);
			if (inGsAccel) nPlan.setEGS(ixNP);
			if (inVsAccel) nPlan.setEVS(ixNP);
			if (nPlan.isEOT(ixNP)) {
				int ixPrevBOT = nPlan.prevBOT(ixNP);
				if (ixPrevBOT >= 0) {
					PlanUtil::createAndAddMOT(nPlan,ixPrevBOT,ixNP);
				}
			}
			//fpln(" $$$$$$ cutDownTo ADD lastP = "+lastP+" vout = "+vout);
		}
	}
	NavPoint lastPt = nPlan.getLastPoint();
	if (tExtend > t2 && tExtend > lastPt.time()) {
		Velocity vout = nPlan.initialVelocity(nPlan.size()-1);  // final velocity of next to last point
		double dt = tExtend - lastPt.time();
		NavPoint extendPt = lastPt.linear(vout, dt); // .makeName("CutDownTo_extendPt");
		TcpData extendTcpData = TcpData();
		extendTcpData.setInformation("CutDownTo_extendPt");
		nPlan.add(extendPt,extendTcpData);

	}
	nPlan.repairPlan();
	nPlan.cleanPlan();
	if (nPlan.isBOT(0)) nPlan.repairMOT(0);
	int lastIx = nPlan.size()-1;
    if (nPlan.isEOT(lastIx)) nPlan.repairMOT(lastIx);

	//fpln(" $$$$ cutDownTo: nPlan = "+nPlan);
	return nPlan;
}

Plan PlanUtil::cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold) {
	return cutDownTo(plan, timeOfCurrentPosition, intentThreshold, intentThreshold);
}


std::pair<bool,double> PlanUtil::enoughDistanceForAccel(const Plan& p, int ix, double maxAccel,	double M) {
	if (ix == 0 || ix == p.size()-1) return std::pair<bool,double>(true,0.0);
	double gsIn = p.finalVelocity(ix-1).gs();
	double gsOut = p.initialVelocity(ix).gs();
	//fpln(" $$ enoughDistanceForAccel: ix = "+ix+" gsIn = "+Units::str("kn",gsIn)+" gsOut = "+Units::str("kn",gsOut));
	double deltaGs = gsOut - gsIn;
	double a = Util::sign(deltaGs)*maxAccel;
	//fpln(" $$ enoughDistanceForAccel: deltaGs = "+Units::str("kn",deltaGs)+" actual accel = "+deltaGs/dt);
	double dtNeeded = deltaGs/a + 2*M; // plus 2*M because BGS starts M second into segment and EGS ends M sec before
	//fpln(" $$ enoughDistanceForAccel: dtNeeded = "+dtNeeded);
	double distanceNeeded = gsIn*dtNeeded + 0.5*a*dtNeeded*dtNeeded;
	double distanceBetween = p.pathDistance(ix,ix+1);
	//fpln(" $$ enoughDistanceForAccel: distanceBetween = "+distanceBetween+" distanceNeeded = "+distanceNeeded);
	bool rtn = (distanceNeeded <= distanceBetween);
	if (rtn) { // no repair needed
		return std::pair<bool,double>(true,0.0);
	} else {
		//double b = gsIn;
		//double c = -distanceBetween;
		double b = 2*(gsIn + a*M);
		double c = 4*gsIn*M - 2.0*distanceBetween;
		double dtp = (-b+sqrt(b*b - 4*a*c))/(2.0*a);
		double dtCurrent = p.time(ix+1) - p.time(ix) - 2*M;
		double correction = (dtp-dtCurrent);
		return std::pair<bool,double>(false, correction);
	}
}


// Fix Plan p at ix if there is not enough distance for the current speed and specified acceleration (maxAccel)
// It makes the new ground speed as close to the original as possible (that is achievable over the distance)
// return -1 if no change was necessary,
// otherwise return the new time at ix+1
void PlanUtil::fixGsAccelAt(Plan& p, int ix, double maxAccel, bool checkTCP, double M) {
	if (ix < 0 || ix >= p.size() - 1) return;
	double dtNow = p.time(ix+1) - p.time(ix);
	if (2*M >= dtNow) {
		//double gsIn = p.finalVelocity(ix-1).gs();
		double gsIn = p.gsIn(ix);
		p.mkGsIn(ix+1,gsIn);
	} else {
		std::pair<bool,double> pEnoughDist = enoughDistanceForAccel(p, ix, maxAccel, M);
		//NavPoint np_ix = p.point(ix+1);   // we will be altering point ix+1
		if (checkTCP && (p.isEOT(ix+1) || p.isEVS(ix+1))) return;
		if ( ! pEnoughDist.first) {
			double correction = pEnoughDist.second;
			//fpln(" $$$$>>>>>>>>>>>>>>>>>>>>>.. fixGsAccelAt: ix = "+ix+" dtCurrent = "+dtCurrent+" dtNew = "+dtNew+" correction = "+correction);
			p.timeShiftPlan(ix+1,correction);	  // 2.2 about right
		}
	}
	return ;
}


int PlanUtil::hasPointsTooClose(const Plan& plan) {
	for (int i = 0; i < plan.size()-1; i++) {
		NavPoint pi = plan.point(i);
		NavPoint pn = plan.point(i+1);
		if (pi.almostEquals(pn)) {
			return i;
		}
	}
	return -1;
}


double PlanUtil::diffMetric(const Plan& lpc, const Plan& kpc) {
	double sumSqDist = 0.0;
	int nPoints = 0;
	for (int i = 0; i < lpc.size(); i++) {
		Position lpcPos = lpc.point(i).position();
		Position kpcPos = kpc.point(i).position();
		double distH = lpcPos.distanceH(kpcPos);
		double distV = lpcPos.distanceV(kpcPos);
		double dist = std::sqrt(distH*distH+distV*distV);
		sumSqDist = sumSqDist + dist*dist;
		//fpln(tm+" dist = "+Units::str("nm",dist));
		nPoints++;
	}
	//fpln(" $$ computeMetric: "+sumSqDist+" "+nPoints);
	return std::sqrt(sumSqDist)/nPoints;
}


// will not remove segments that are longer than maxLegSize
Plan PlanUtil::unZigZag(const Plan& pp) {
	return unZigZag(pp, MAXDOUBLE);
}

// will not remove segments that are longer than maxLegSize
Plan PlanUtil::unZigZag(const Plan& pp, double maxLegSize) {
	Plan p = pp;
	double lastEvenTrack = 0.0;
	double lastOddTrack = 0.0;
	//bool prevTheSame[p.size()]; //  = bool[p.size()];
	std::vector<bool> prevTheSame = std::vector<bool>(p.size());
	for (int i = 0; i < p.size(); i++) {
		Velocity v_i = p.initialVelocity(i);
		//fpln(" $$ reRouteWx: v_"+i+" = "+v_i);
		double currentTrk = v_i.trk();
		if (i % 2 == 0) {  // even
			prevTheSame[i] = (Util::turnDelta(lastEvenTrack,currentTrk) < Units::from("deg",5));
			lastEvenTrack = currentTrk;
		} else {
			prevTheSame[i] = Util::turnDelta(lastOddTrack,currentTrk) < Units::from("deg",5);
			lastOddTrack = currentTrk;
		}
	}
	for (int i = 0; i < p.size(); i++) {
		//fp(" unZigZag >>>>>>>>>. "+Fm0(i));
		if (i % 2 != 0) fp("      ");
		//fpln(" "+bool2str(prevTheSame[i]));
	}
	int cntZigZag = 0;
	bool lastTheSame = false;
	for (int j = p.size()-1; j >= 0; j--) {
		if (prevTheSame[j] || lastTheSame) cntZigZag++;
		else cntZigZag = 0;
		lastTheSame = prevTheSame[j];
		if (cntZigZag > 1 && cntZigZag % 2 == 0) {
			if (j > 1 && p.pathDistance(j) <= maxLegSize && p.pathDistance(j-1) <= maxLegSize) {        // do not remove point 1, it preserves current velocity (lead in)
				//fpln(" $$$$$$$$$ unZigZag: REMOVE j = "+j);
				p.remove(j);
			}
		}
		if (!lastTheSame) cntZigZag = 0;
	}
	p = mkGsConstant(p);
	//fpln(" $$$$ END: unZigZag: p = "+p);
	return p;
}

bool PlanUtil::aboutTheSameTrk(const Velocity& v1, const Velocity& v2, double sameTrackBound) {
	return std::abs(v1.trk()- v2.trk()) < sameTrackBound;
}

Plan PlanUtil::removeCollinearTrk(const Plan& pp, double sameTrackBound){
	if (pp.size() < 1) return Plan(pp.getID());
	//fpln(" $$ removeCollinearTrk: pp = "+pp);
	Plan p(pp.getID(),pp.getNote());
	Velocity lastVel = pp.initialVelocity(0);
	p.add(pp.get(0));
	for (int j = 1; j < pp.size()-1; j++) {
		Velocity vel = pp.initialVelocity(j);
		bool same = aboutTheSameTrk(lastVel,vel,sameTrackBound);
		//fpln(j+" $$ removeCollinear: lastVel = "+lastVel+" vel = "+vel+" same = "+same);
		if (!same) {
			p.add(pp.get(j));
		}
		lastVel = vel;
	}
	p.add(pp.get(pp.size()-1));
	//fpln(" $$ removeCollinearTrk: pp.size() = "+pp.size()+" p.size() = "+p.size());
	return p;
}


Plan PlanUtil::mkGsConstant(const Plan& p, int i, int j, double gs) {
	Plan rtnPln(p);
    rtnPln.mkGsConstant(i,j,gs);
    return rtnPln;
}

Plan PlanUtil::mkGsConstant(const Plan& p, double gs) {
	return mkGsConstant(p,0,p.size()-1,gs);
}

Plan PlanUtil::mkGsConstant(const Plan& p) {
	double dtot = p.pathDistance(0,p.size()-1);
	double ttot = p.time(p.size()-1) - p.time(0);
	double gs = dtot/ttot;
	return mkGsConstant(p, 0,p.size()-1,gs);
}


Plan PlanUtil::mkGsConstant(const Plan& p, int wp1, int wp2) {
	//int rtn = -1;
	//fpln("%%##  makeGSConstant: wp1 = "+wp1+" wp2 = "+wp2);
	if (wp1 < 0) return p;
	if (wp2 >= p.size()) wp2 = p.size()-1;
	//int start = wp1;
	//int end = wp2;
	if (wp1 >= wp2) return p;
	double dtot = p.pathDistance(wp1,wp2);
	double ttot = p.time(wp2) - p.time(wp1);
	double gs = dtot/ttot;
	//fpln(" makeGSConstant: TARGET gs = "+Units::str("kn",gs));
	return mkGsConstant(p, wp1,wp2,gs);
}



// change vertical profile: adjust altitudes
void PlanUtil::mkVsConstant(Plan& p, int wp1, int wp2, double vs) {
	//fpln("%%## makeVsConstant:  wp1 = "+wp1+" wp2 = "+wp2+ " vs = "+Units::str("fpm",vs));
	if (wp1 < 0) return;
	if (wp2 >= p.size()) wp2 = p.size()-1;
	if (wp1 >= wp2) return;
	for (int i = wp1; i < wp2; i++) {
		double dt = p.point(i+1).time() - p.point(i).time();
		double newAlt = p.point(i).alt()+dt*vs;
		NavPoint np = p.point(i+1).mkAlt(newAlt);
		TcpData  tcp = p.getTcpData(i+1);
		//fpln(" $$$$$>> makeVsConstant ADD  "+np);
		p.remove(i+1);
		p.add(np, tcp);
	}
}

// change vertical profile: adjust altitudes
void PlanUtil::mkVsConstant(Plan& p, int start, int end) {
	if (start < 0) return;
	if (end >= p.size()) end = p.size()-1;
	if (start >= end) return;
	double zStart = p.point(start).z();
	double zEnd = p.point(end).z();
	double dtot = zEnd - zStart;
	double ttot = p.time(end) - p.time(start);
	double vs = dtot/ttot;
	mkVsConstant(p,start,end,vs);
}


// change vertical profile: adjust altitudes
void PlanUtil::mkVsConstant(Plan& p) {
	mkVsConstant(p,0,p.size()-1);
}

void PlanUtil::mkVsConstant(Plan& p, double vs) {
	mkVsConstant(p,0,p.size()-1,vs);
}


int PlanUtil::mkVsShortLegsContinuous(Plan& p, double vsAccel, bool inhibitFixGs0, double aggressiveFactor, bool inhibitFixVs0) {
	return mkVsShortLegsContinuous(p,0,p.size()-1,vsAccel, inhibitFixGs0, aggressiveFactor,inhibitFixVs0);
}

/** Examine each vertical segment [i,i+1] over the range.  Calculate needed time for the vertical
 *  accelerations at each end.  If the sum of half of each of these accels is longer
 *  in time than the segment dt, then smooth away this segment vertically.  Either
 *  side could be potentially smooth.  This method smooths the side with the smallest
 *  delta vs.
 *
 * @param p          Plan to be modified
 * @param start      starting index
 * @param end        ending index
 * @param vsAccel    vertical speed acceleration
 *
 * @return number of segments that were smoothed
 */
int PlanUtil::mkVsShortLegsContinuous(Plan& p, int start, int end, double vsAccel, bool inhibitFixGs0,
		                              double aggressiveFactor, bool inhibitFixVs0) {
	int numSmoothed = 0;
	if (end >= p.size()) end = p.size()-1;
	if (start >= end) return 0;
	if (start <= 0) start = 1;
	for (int i = start; i < end-1; i++) {
		int nextIx = i+1; // nextVsChange(p,i);
		//double dt_avail = p.time(nextIx) - p.time(i);
		double dt_avail = aggressiveFactor*(p.time(nextIx) - p.time(i));
		double vs_im1 = p.vsOut(i-1);
		double vs_i = p.vsOut(i);
		double vs_ip1 = p.vsOut(i+1);
		double delta_i = std::abs(vs_i - vs_im1);
		if (std::abs(delta_i) < Plan::MIN_VS_DELTA_GEN) delta_i = 0;   // will not create a BVS-EVS at i
		double delta_ip1 = std::abs(vs_ip1 - vs_i);
		//double deltaVs = Math.max(delta_i, delta_ip1);
		if (std::abs(delta_ip1) < Plan::MIN_VS_DELTA_GEN) delta_ip1 = 0;  // will not create a BVS-EVS at i+1
		double neededDt = (delta_i + delta_ip1)/(2*vsAccel);
		bool isNextToCruise = std::abs(vs_i) <  Units::from("fpm", 10);
		//f.pln("\n $$$ mkVsShortLegsContinuous: i = "+i+" nextIx = "+nextIx+" aggressiveFactor = "+aggressiveFactor+" info = "+p.getInfo(i));
		//f.pln("\n $$$ mkVsShortLegsContinuous: i = "+i+" p = "+p.toStringProfile());
		//f.pln(" $$$ mkVsShortLegsContinuous: nextIx = "+nextIx+" vs_i = "+Units.str("fpm",vs_i)+" vs("+nextIx+") = "+Units.str("fpm",vs_ip1));
		//f.pln(" $$$ mkVsShortLegsContinuous: i = "+i+" delta_i = "+Units.str("fpm",delta_i)+" delta_ip1 = "+Units.str("fpm",delta_ip1));
		//f.pln(" $$$  mkVsShortLegsContinuous: i = "+i+" dt_avail = "+f.Fm1(dt_avail)+" neededDt = "+f.Fm2(neededDt));
		//f.pln(" $$$  mkVsShortLegsContinuous: isNextToCruise = "+isNextToCruise+" inhibitVs0 = "+inhibitVs0);
		if (neededDt >= dt_avail && (!inhibitFixVs0 || !isNextToCruise)) {
			int ixSmooth = i;
			if (delta_ip1 < delta_i) ixSmooth = i+1;
			if (std::abs(p.gsOut(ixSmooth)) > Units::from("kn",1.0) || ! inhibitFixGs0) {
				int remIx = mkVsContinuousAt(p,ixSmooth);
				numSmoothed++;
				if (remIx >= 0) end = end - 1;
			}
		}
	}
	return numSmoothed;
}

int PlanUtil::mkVsContinuousAt(Plan& p, int i) {
	double alt_im1 = p.alt(i-1);
	double newVs = (p.alt(i+1) - alt_im1)/(p.time(i+1) - p.time(i-1));
	double newAlt = alt_im1 + newVs*(p.time(i) - p.time(i-1));
	p.mkAlt(i,newAlt);
	int remIx = p.removeIfRedundant(i);
	return remIx;
}


Plan PlanUtil::repairShortVsLegs(const Plan& traj, double vsAccel_d) {
	Plan lpc = Plan(traj); //fp.copy();
	for (int i = 1; i < traj.size()-1; i++) {
		double vs1 = traj.vsOut(i-1);
		double vs2 = traj.vsOut(i);
		int sign = 1;
		if (vs1 > vs2) sign = -1;
		double a = vsAccel_d*sign;
		double deltaVs = vs2 - vs1;
		double accelTime = std::abs(deltaVs/a);
		//if (accelTime > MIN_ACCEL_TIME) {
		bool vsAccelNeeded = std::abs(deltaVs) > Plan::MIN_VS_DELTA_GEN;
		if (vsAccelNeeded) {
			//fpln(" $$$ linearRepairShortVsLegs: i = "+Fm0(i)+" vsAccel_d = "+Fm1(vsAccel_d)+" accelTime = "+Fm1(accelTime));
			double tbegin = traj.point(i).time() - accelTime/2.0;
			double tend = tbegin + accelTime;
			double prevEndTime = traj.getFirstTime();
			int ixEVS =  traj.prevEVS(i);
			if (ixEVS >= 0) prevEndTime = Util::max(prevEndTime,traj.time(ixEVS));
			int nextVsChangeIx = PlanUtil::nextVsChange(traj, i);
			double nextVsChangeTm = traj.time(nextVsChangeIx);
			if (tbegin < prevEndTime) {
				PlanUtil::mkVsConstant(lpc, i-1,i+1);
			} else if (tend > nextVsChangeTm)
				PlanUtil::mkVsConstant(lpc, i,i+2);
		}
	}
	//fpln(" $$ linearRepairShortVsLegs: AFTER CALL, RETURN lpc = "+lpc.toString());
	return lpc;
}


int PlanUtil::shareSegment(const Plan& p1, const Plan& p2) {
	for (int i = p1.size()-2; i >=0; i--) {
		Position pos1a = p1.point(i).position();
		Position pos1b = p1.point(i+1).position();
		for (int j = 0; j < p2.size()-1; j++) {
			Position pos2a = p2.point(j).position();
			Position pos2b = p2.point(j+1).position();
			if (pos1a.almostEquals(pos2a) && pos1b.almostEquals(pos2b)) return i;
		}
	}
	return -1;
}



std::pair<NavPoint,TcpData> PlanUtil::makeMidpoint( TcpData& tcp, const Position& p, double t) {
	NavPoint np = NavPoint(p,t);
	//tcp = tcp.copy().setLinearIndex();
	return std::pair<NavPoint,TcpData>(np,tcp);
}


Plan PlanUtil::projectPlan(const Plan& p, const EuclideanProjection& proj) {
	Plan p3d = Plan(p.getID());
	for (int i = 0; i < p.size(); i++) {
		Vect3 v = proj.project(p.point(i).position());
		NavPoint n = NavPoint(Position(v), p.time(i));
		TcpData tcp = p.getTcpData(i);
		TcpData tcp3d = TcpData(tcp);
		// this version just projects the source and center points.
		// to be more correct it might be necessary to completely re-generate them
		Position center = Position(proj.project(tcp.turnCenter()));
		tcp3d.setTurnCenter(center);
		p3d.add(n, tcp3d);
	}
	return p3d;
}

bool PlanUtil::basicCheck(const Plan& lpc, const Plan& kpc, double maxlastDt) {
	if ( ! lpc.isLatLon()) return true; // TODO: need to set new limits if we don't have a lat/lon Plan
	if (std::abs(lpc.point(0).time() - kpc.point(0).time()) > 1E-10 ||
			std::abs(lpc.point(0).lat() - kpc.point(0).lat()) > 1E-10 ||
			std::abs(lpc.point(0).lon() - kpc.point(0).lon()) > 1E-10 ||
			std::abs(lpc.point(0).alt() - kpc.point(0).alt()) > 1E-10)
	{
		//fpln(" $$$$ basicCheck: lpc.point(0) = "+lpc.point(0).toString()+" kpc.point(0) = "+kpc.point(0).toString());
		return false;
	}
	if (std::abs(lpc.point(lpc.size()-1).time() - kpc.point(kpc.size()-1).time()) > maxlastDt ||
			std::abs(lpc.point(lpc.size()-1).lat() - kpc.point(kpc.size()-1).lat()) > 1E-10 ||
			std::abs(lpc.point(lpc.size()-1).lon() - kpc.point(kpc.size()-1).lon()) > 1E-10 ||
			std::abs(lpc.point(lpc.size()-1).alt() - kpc.point(kpc.size()-1).alt()) > 1E-10)
	{
		//fpln(" $$$$ basicCheck: lpc.point(lpc.size()-1) = "+lpc.point(lpc.size()-1).toString()+" kpc.point(kpc.size()-1) = "+kpc.point(kpc.size()-1).toString());
		return false;
	}
   return true;
}


/**  Repair turns for semi-linear plan.  Removes vertex point if turn is infeasible.
 *
 * @param fp          Plan that needs repair
 * @param bankAngle   default bank angle
 * @param repair      if true, attempt repair.  If false, only determine the number of bad turns remaining.
 * @return            number of repaired or infeasible vertices
 *
 * Note: This assumes that there are no GS or VS TCPs in lpc and no existing turns in the repair range
 */
int PlanUtil::infeasibleTurns(Plan& lpc, int startIx, int endIx, double default_bank_angle, bool repair) {
	//f.pln(" $$>> fixInfeasibleTurns: ENTER default_bank_angle = "+Units::str("deg",default_bank_angle));
	std::string name = lpc.getID();
	if (repair) {
		lpc.mergeClosePoints();
		bool trkF = true;
		bool gsF = true;
		bool vsF = true;      // TODO -- this could be set to false
		double minTrk = Units::from("deg",1.0);
		double minGs = Units::from("kn",5.0);
		double minVs = Units::from("fpm",100.0);
		int cntRemoved = 0;
		for (int i = endIx; i >= startIx; i--) {
			int ixRem = lpc.removeIfRedundant(i,  trkF,  gsF,  vsF, minTrk,  minGs,  minVs, repair);
			if (ixRem >= 0) {
				cntRemoved++;
				//f.pln(" $$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> fixInfeasibleTurns: removeIfRedundant removed ix = "+ixRem);
			}
		}
		//if (cntRemoved > 0) f.pln(" $$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> fixInfeasibleTurns("+name+"): removeIfRedundant removed "+cntRemoved+" points");
	}
	double bank = std::abs(default_bank_angle);
	if (bank == 0) {
		lpc.addError("ERROR in TrajGen.generateTurnTCPs: specified bank angle is 0");
		bank = Units::from("deg", 0.001); // prevent divisions by 0.0
	}
	if (endIx > lpc.size()-2) endIx = lpc.size()-2;
	if  (startIx < 1) startIx = 1;
	int numBadVerts = 0;
	int ixNextVertex = PlanUtil::nextTrackChange(lpc,endIx);
	double lastTurnDistUsed = 0;
	for (int i = endIx; i >= startIx; i--) {
		double gsIn = lpc.gsIn(i);
		if (Util::almost_equals(gsIn,0.0)) continue;
		double turnDelta = Util::turnDelta(lpc.trkIn(i), lpc.trkOut(i));
		double R = lpc.vertexRadius(i);        // R stored in a linear plan
		if (Util::almost_equals(R, 0.0)) {      // no specified radius
			R = Kinematics::turnRadius(gsIn, bank);
		}
		//f.pln(" $$>> fixInfeasibleTurns: i = "+i+" R = "+Units::str("NM",R)+" turnDelta = "+Units::str("deg",turnDelta)+" >=? "+Units::str("deg",Plan.MIN_TRK_DELTA_GEN));
		double gsOut_i = lpc.gsOut(i);
		//f.pln(" $$$$$ fixInfeasibleTurns: CHECK i = "+i+" gsOut_i = "+gsOut_i+" "+lpc.getInfo(i)+" "+lpc.getPointName(i));
		if (turnDelta >= Plan::MIN_TRK_DELTA_GEN && gsOut_i > 1E-10) {  // do not generate when gsOut is near 0
			double needLegDist = TrajGen::neededTurnLegDistance(lpc, i, R);
			double distAfter_i = lpc.pathDistance(i,ixNextVertex)- lastTurnDistUsed;
			int ixPrev = PlanUtil::prevTrackChange(lpc,i);
			double distBefore_i = lpc.pathDistance(ixPrev,i);
			//f.pln(" $$$ fixInfeasibleTurns:  i = "+i+" needLegDist = "+Units::str("NM",needLegDist)+" lastTurnDistUsed = "+Units::str("NM",lastTurnDistUsed)+" R = "+Units::str("NM",R));
			//f.pln(" $$$ fixInfeasibleTurns: i = "+i+" distBefore_i = "+Units::str("NM",distBefore_i)+" distAfter_i = "+Units::str("NM",distAfter_i));
			//f.pln(" $$$ fixInfeasibleTurns: ixNextVertex = "+ixNextVertex+" ixPrev = "+ixPrev);
			if (needLegDist > distAfter_i || needLegDist > distBefore_i) {
				//f.pln("\n $$$$ fixInfeasibleTurns: LOOK AT i = "+i);
				numBadVerts++;
				if (repair) {
					bool allowRepairByIntersection = true;
				    bool removedAPoint = fixInfeasibleTurnAt(lpc,i,allowRepairByIntersection);
				    if (removedAPoint) i++;
				} else {
					//f.pln(" >> fixInfeasibleTurns("+name+"): DETECTION ONLY: turn at i = "+i+" INFEASIBLE!! "+lpc.getInfo(i)+" "+lpc.getPointName(i));
					//f.pln(" >> fixInfeasibleTurns("+name+"): DETECTION ONLY: i = "+i+" needLegDist = "+Units::str("NM",needLegDist)+" lastTurnDistUsed = "+Units::str("NM",lastTurnDistUsed)+" R = "+Units::str("NM",R));
					//f.pln(" >> fixInfeasibleTurns("+name+"): DETECTION ONLY: i = "+i+" distBefore_i = "+Units::str("NM",distBefore_i)+" distAfter_i = "+Units::str("NM",distAfter_i));
					//f.pln(" >> fixInfeasibleTurns("+name+"): DETECTION ONLY: isRedundant("+i+") = "+lpc.isRedundant(i,true,true,true));
					//f.pln(" >> fixInfeasibleTurns("+name+"): DETECTION ONLY: isTCP ="+lpc.isTCP(i)+" isAltPreserve ="+lpc.isAltPreserve(i));
				}
			} else {
				// f.pln(" $$$$$ fixInfeasibleTurns: TURN i = "+i+" is OK!");
			}
			lastTurnDistUsed = needLegDist;
			ixNextVertex = i;
		}
	}//for
	return numBadVerts;
}

int PlanUtil::fixBadTurns(Plan& lpc, int startIx, int endIx, double default_bank_angle) {
	return infeasibleTurns(lpc, startIx, endIx, default_bank_angle,true);
}

int PlanUtil::fixBadTurns(Plan& lpc, double default_bank_angle) {
	return infeasibleTurns(lpc, 1, lpc.size()-2, default_bank_angle, true);
}

int PlanUtil::countBadTurns(Plan& lpc, double default_bank_angle) {
	return infeasibleTurns(lpc, 0, lpc.size()-1, default_bank_angle,false);
}


/**  Repairs turn problem at index ix using several different strategies:
 *      -- create a new vertex point (that makes one turn our of two)
 *      -- removing the vertex point
 *      -- moving the vertex point
 *
 * @param lpc    semi-linear plan  (no GS or VS TCPs)
 * @param ix     index
 *
 * @return true if point removed
 */
bool PlanUtil::fixInfeasibleTurnAt(Plan& lpc, int ix, bool allowRepairByIntersection) {
	// if turns are in same direction can fix with an intersection point
	//int iPrev = PlanUtil::prevTrackChange(lpc,ix);
	std::string pName = lpc.getID();
	std::string info_i = lpc.getInfo(ix);
	std::string name_i = lpc.getName(ix);
	bool altPres_i = lpc.isAltPreserve(ix);
	//f.pln(" $$$$ fixInfeasibleTurnAt: info_i = "+info_i+" name_i = "+name_i+" altPres_i = "+altPres_i);
	if (equals(info_i,"") && equals(name_i,"") && !altPres_i) {
		lpc.remove(ix);
		//f.pln(" $$$$ fixInfeasibleTurnAt: FIXED "+pName+" BY DELETING i = "+ix);
		return true;
	} else if (allowRepairByIntersection && ! equals(name_i,"")) {  // probably should not move named waypoints
		// which leg is shorter ?
		double distBefore = lpc.pathDistance(ix-1,ix);
		double distAfter = lpc.pathDistance(ix,ix+1);
		if (distBefore < distAfter) {
			fixInfeasibleTurnAtViaIntersection(lpc, ix);   // TODO; after should we ix--; ??
			//f.pln(" $$$$>>>>>>>>>> fixInfeasibleTurnAt(AAAA): ix = "+ix+" lpc = "+lpc.toStringGs());
			//Debug.halt();
		} else {
			fixInfeasibleTurnAtViaIntersection(lpc, ix+1);   // TODO; after should we ix--; ??
   			//f.pln(" $$$$>>>>>>>>>> fixInfeasibleTurnAt(BBBB): ix+1 = "+(ix+1)+" lpc = "+lpc.toStringGs());
   		    //Debug.halt();
		}
		return false;
	} else { // Need to Relocate point ix
		//f.pln(" $$$$ fixInfeasibleTurnAt: "+info_i+" "+name_i+" altPres_i = "+altPres_i);
		double d0 = lpc.pathDistance(ix-1,ix);
		double gs0 = lpc.gsOut(ix-1);
		//int connectionIndex = PlanUtil::nextTrackChange(lpc,ix);  // TODO: move all points up to this point
		double d1 = lpc.pathDistance(ix,ix+1);
		TcpData tcpData_i = lpc.getTcpData(ix);
		NavPoint np_i = lpc.point(ix);
		double gsIx = lpc.gsOut(ix);
		lpc.remove(ix);
		double dNew = lpc.pathDistance(ix-1,ix);
		double newDist = d0*dNew/(d0+d1);
		std::pair<Position, int> advPair = lpc.advanceDistance(ix-1,newDist,false);
		Position newPos = advPair.first.mkAlt(np_i.alt());
		double dt = newDist/gs0;
		double t0 = lpc.time(ix-1);
		NavPoint newNp = NavPoint(newPos,t0+dt);
		newNp = newNp.makeName(np_i.name());
		int ixNew = lpc.add(newNp,tcpData_i);
		//lpc.appendInfo(ixNew,"<MOVED>");
		lpc.mkGsOut(ixNew,gsIx);     // repair GS at move location
		//f.pln(" $$$$ fixInfeasibleTurnAt: FIXED "+pName+" BY MOVING  i = "+ix);
		//f.pln(" $$$$ fixInfeasibleTurnAt: lpc = "+lpc.toStringGs());
		//DebugSupport.dumpPlan(lpc,"fixInfeasibleTurnAt_MOVE_"+ix);
		return false;
	}
	//		}
}

/** Create an intersection point between ix-1 and ix
 *
 * @param lpc     Plan to be repaired
 * @param ix      index of problem turn
 * @return        true if plan was successfully repaired
 */
bool PlanUtil::fixInfeasibleTurnAtViaIntersection(Plan& lpc, int ix) {
	bool rtn = false;
	std::string name = lpc.getID();
	//f.pln(" $$$ fixInfeasibleTurnAtViaIntersection: ix = "+ix+" "+turnDir(lpc,ix-1)+" =?"+ turnDir(lpc,ix));
	if (turnDir(lpc,ix-1) == turnDir(lpc,ix)) { // && iPrev == ix-1) {
		NavPoint inters = intersection(lpc, ix);
		double gsIn_i = lpc.gsIn(ix);
		int ixInt = lpc.addNavPoint(inters);
		if (ixInt >= 0) {
			if (ixInt != ix) {
				lpc.addError(" fixInfeasibleTurnAtViaIntersection: REPAIR of "+name+" at ix ="+Fm0(ix)+" failed, intersection point placed in wrong place!");
				//fpln(" $$$ fixInfeasibleTurnAtViaIntersection:  ========================================================");
				fpln(" $$$ fixInfeasibleTurnAtViaIntersection: REPAIR of "+name+" at ix ="+Fm0(ix)+" failed, intersection point placed in wrong place! inters = "+inters.toString());
				//fpln(" $$$ fixInfeasibleTurnAtViaIntersection:  ========================================================");
				lpc.remove(ixInt); // clean-up after failure
			} else {
				//f.pln(" $$$$ fixInfeasibleTurnAtViaIntersection:  FIXED "+name+" at i = "+ix+" by adding intersection point at "+ixInt);
				lpc.mkGsIn(ix+1,gsIn_i);
				//lpc.setInfo(ixInt,"<TurnRepair>");
				ix = ixInt - 1;
				rtn = true;
			}
		}
	}
	return rtn;
}

int PlanUtil::turnDir(Plan lpc, int i) {
	return Util::turnDir(lpc.trkIn(i),lpc.trkOut(i));
}

/** Create intersection point between (i-1) and i
 *
 * @param lpc
 * @param i
 * @return
 */
NavPoint PlanUtil::intersection(Plan lpc, int i) {
	Position int2D = Position::intersection2D(lpc.getPos(i-2), lpc.getPos(i-1), lpc.getPos(i),lpc.getPos(i+1));
	double gs0 = lpc.gsOut(i-1);
	//f.pln(" $$$$ PlanUtil.intersection: gs0 = "+Units.str("kn",gs0));
	double intTm;
	if (Util::almost_equals(gs0,0.0)) {
		intTm = (lpc.time(i-1) + lpc.time(i))/2.0;
	} else {
		double distToInt = lpc.getPos(i-1).distanceH(int2D);
		double dt = distToInt/gs0;
		intTm = lpc.time(i-1) + dt;
	}
	double intAlt = (lpc.getPos(i-1).alt() +lpc.getPos(i).alt())/2.0;
	NavPoint newNp = NavPoint(int2D,intTm).mkAlt(intAlt);
	return newNp;
}



bool PlanUtil::checkReversion(const Plan& kpc, const Plan& lpc) {
	 bool verbose = false;
	 return checkReversion(kpc, lpc, verbose);
}


bool PlanUtil::checkReversion(const Plan& kpc, const Plan& lpc, bool verbose) {
	double maxlastDt = 100;
	if (! basicCheck(lpc,kpc,maxlastDt)) {
		if (verbose) fpln(" ## checkReversion: FAILED basicCheck!");
		return false;
	}
	if (kpc.size() != lpc.size()) {
		if (verbose) fpln(" ## checkReversion: FAILED size check!");
		return false;
	}
	for (int j = 0; j < lpc.size(); j++) {
		double maxDt = 1;
		if (j == lpc.size()-1) maxDt = maxlastDt;
		if (std::abs(lpc.point(j).time() - kpc.point(j).time()) > maxDt) {
			if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" TIMES: "+Fm4(lpc.point(j).time())+" "+Fm4(kpc.point(j).time()));
			return false;
		}
		if (lpc.isLatLon()) {
			if (std::abs(lpc.point(j).lat() - kpc.point(j).lat()) > 10) {
				if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" LATS: "+Fm4(lpc.point(j).lat())+" "+Fm4(kpc.point(j).lat()));
				return false;
			}
			if (std::abs(lpc.point(j).lon() - kpc.point(j).lon()) > 10) {
				if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" LONS: "+Fm4(lpc.point(j).lon())+" "+Fm4(kpc.point(j).lon()));
				return false;
			}
		} else {
			if (std::abs(lpc.point(j).x() - kpc.point(j).x()) > 10) {
				if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" Xs: "+Fm4(lpc.point(j).lat())+" "+Fm4(kpc.point(j).lat()));
				return false;
			}
			if (std::abs(lpc.point(j).y() - kpc.point(j).y()) > 10) {
				if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" Ys: "+Fm4(lpc.point(j).lon())+" "+Fm4(kpc.point(j).lon()));
				return false;
			}
		}
		if (std::abs(lpc.point(j).alt() - kpc.point(j).alt()) > 10){
			if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" ALTS: "+Fm4(lpc.point(j).alt())+" "+Fm4(kpc.point(j).alt()));
			return false;
		}
		std::string nm1 = lpc.getName(j);
		if (! larcfm::equals(nm1, kpc.getName(j))) {
			if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" NAMES MISMATCH: "+lpc.getName(j)+" != "+kpc.getName(j));
			return false;
		}
		if (! larcfm::equals(lpc.getInfo(j),kpc.getInfo(j))) {
			if (verbose) fpln(" ## checkReversion: FAILED j = "+Fm0(j)+" INFO MISMATCH: "+lpc.getInfo(j)+" != "+kpc.getInfo(j));
			return false;
		}

	}
	if (checkNamesInfoRetained(kpc,lpc,true)) return true;
	return false;
}


/**  Test to make sure that all name and info in plan lpc is also in kpc
 *
 * @param kpc
 * @param lpc
 * @param verbose
 * @return
 */
bool PlanUtil::checkNamesInfoRetained(const Plan& kpc, const Plan& lpc, bool verbose) {
	bool rtn = true;
	for (int j = 0; j < lpc.size(); j++) {
		std::string name_j =  lpc.getName(j);
		if (! equals(name_j,"")) {
			int ixN = kpc.findName(name_j);
			if (ixN < 0) {
				if (verbose) fpln(" >>> checkNamesInfoRetained: at j = "+Fm0(j)+" name "+name_j+" was lost!");
				rtn = false;
			}
		}
		std::string info_j =  lpc.getInfo(j);
		if (TcpData::motFlagInInfo) info_j = larcfm::replace(info_j,Plan::manualRadius,"");
		else info_j = larcfm::replace(larcfm::replace(info_j,TcpData::MOTflag,""),Plan::manualRadius,"");
		if (! equals(info_j,"")) {
			int ixI = kpc.findInfo(info_j);
			if (ixI < 0) {
				if (verbose) fpln(" >>> checkNamesInfoRetained: at j = "+Fm0(j)+" info "+info_j+" was lost!");
				rtn = false;
			}
		}
	}
	return rtn;
}



void PlanUtil::createAndAddMOT(Plan& kpc, int ixBOT, int ixEOT) {
	double midD = kpc.pathDistance(ixBOT,ixEOT)/2.0;
	Position midPos = kpc.advanceDistance(ixBOT,midD,false).first;
	double midTm = kpc.timeFromDistance(ixBOT, midD);
    NavPoint MOT = NavPoint(midPos,midTm);
	int ixMOT = kpc.addNavPoint(MOT);
	//kpc.setInfo(ixMOT,Plan::MOTflag);
	kpc.setMOT(ixMOT);
}



/** Only tests velocity continuity at TCPs.
 *
 * Note: If given a linear plan as input, Stratway will only generate a kinematic
 *       subplan over the conflict region.   The output can still be linear elsewhere.
 *       This test is appropriate for Stratway output.
 *
 * @param p            Plan to be tested
 * @param silent       if true, output is suppressed
 * @return
 */
bool PlanUtil::isVelocityContAtTcps(const Plan& p, bool silent) {
	for (int i = 1; i < p.size(); i++) {
		if ( ! p.isTCP(i)) continue;
		if ( ! p.isTrkContinuous(i, silent)) return false;
		if ( ! p.isGsContinuous(i, silent)) return false;
		if ( ! p.isVsContinuous(i, silent)) return false;
	}
	return true;
}

Plan PlanUtil::repairSmallNegativeGS(const Plan& p) {
	Plan pp = Plan(p);
	double gsMinNegativeToFix = -1E-4;	// do not repair negative values less than this (bad plan data assumed)
	for (int ix = 0; ix < pp.size(); ix++) {
		if (pp.isBGS(ix)) {
			double gsOutCalc = pp.gsOutCalc(ix, false);
			if (gsOutCalc < 0 && gsOutCalc > gsMinNegativeToFix) {
				// repair small negative GS by a small adjustment of the time at the next point
				double a = pp.getGsAccel(ix);
				double dist = pp.pathDistance(ix, ix+1, false);
				double newT = pp.time(ix) + std::sqrt(2.0*dist/a);
				pp.setTime(ix+1,newT);
			} else if (gsOutCalc <= gsMinNegativeToFix) {
				pp.addError("PlanUtil.repairSmallNegativeGS failed on BGS "+Fm0(ix));
			}
		} else if (pp.isEGS(ix)) {
			double gsInCalc = pp.gsInCalc(ix);
			if (gsInCalc < 0 && gsInCalc > gsMinNegativeToFix) {
				// repair small negative GS by a small adjustment of the time at the previous point
				double a = -pp.getGsAccel(pp.prevBGS(ix));
				double dist = pp.pathDistance(ix-1, ix, false);
				double newT = pp.time(ix-1) + std::sqrt(2.0*dist/a);
				pp.setTime(ix,newT);
			} else if (gsInCalc <= gsMinNegativeToFix) {
				pp.addError("PlanUtil.repairSmallNegativeGS failed on EGS "+Fm0(ix));
			}
		} else {
			double gsOutCalc = pp.gsOutCalc(ix, false);
			if (gsOutCalc < 0 && gsOutCalc > gsMinNegativeToFix) {
				pp.addError("PlanUtil.repairSmallNegativeGS failed on internal point "+Fm0(ix));
			}
		}
	}
	return pp;
}

Plan PlanUtil::fixAccelConsistency(const Plan& p) {
	// use the following to store "current" relative times
	Plan ret = p.copy();

	for (int i = 0; i < ret.size(); i++) {
		if (ret.isBGS(i)) {
			int egs = ret.nextEGS(i);
			double t0 = ret.time(i);
			double t1 = ret.time(egs);
			double dt = t1-t0;
			double d = ret.pathDistance(i, egs);
			double a = ret.gsAccel(i);
			double gs0 = d/dt-0.5*a*dt;
			double gs1 = d/dt+0.5*a*dt;
			if (gs0 < 0) {
				a = gs1/dt;
				ret.setGsAccel(i, a);
				gs0 = 0.0;
			} else if (gs1 < 0) {
				a = -gs0/dt;
				ret.setGsAccel(i, a);
				gs1 = 0.0;
			}

			for (int j = i+1; j < egs; j++) {
				// distances in accel zone parts, before/after j
				double dd = ret.pathDistance(i,j);
				double dd2 = ret.pathDistance(j,egs);

				// calculated relative times from i to j and from j to egs
				double tt1 = Util::root(0.5*a, gs0, -dd, 1); // tmie from i to j
//					double tt2 = Util.root(0.5*a, gs0, -dd, -1);
				// time from end
				double tt3 = Util::root(-0.5*a, gs1, -dd2, 1); // time from j to end
//					double tt4 = Util.root(-0.5*a, gs1, -dd2, -1);
				// assume the first time
				double tj = t0+tt1;
//f.pln("dt="+dt+"  tt1="+tt1+"  tt2="+tt2+"  tt3="+tt3+"  tt4="+tt4);


				if (!Util::almost_equals(tt1+tt3, dt)) {
					// if the times are not quite consistent, check the calculated ground speeds at either end of the accel segment for both
					// if tt1 calculations result in a less than zero gs, use the time that results in the larger gs (it may still be < 0)
					// these are the calculations for using tt1
					double dt2 = dt-tt1; // time from j to end
					double gsA = dd/tt1-0.5*a*tt1; //start
					double gsB = dd2/dt2+0.5*a*dt2; //end
					// these are the calculations for using tt3
					double dt3 = dt-tt3; // time from i to j
					double gsC = dd/dt3-0.5*a*dt3; //start ALT
					double gsD = dd2/tt3+0.5*a*tt3; //end ALT
					if ((gsA < 0 && gsA < gsC) || (gsB < 0 && gsB < gsD)) {
						fpln(">>>>>>>>>>> PlanUtil.fixAccelConsistency Choosing second time because first is more negative");
						tj = t1-tt3;
					}
					if ((gsA < 0 && gsC < 0) || (gsB < 0 && gsD < 0)) {
						fpln(">>>>>>>>>>> PlanUtil.fixAccelConsistency ERROR!!!  j="+Fm0(j)+" gsA="+Fm3(gsA)+" gsB="+Fm3(gsB)+" gsC="+Fm3(gsC)+" gsD="+Fm3(gsD));
					}
				}

				if (!ISNAN(tj)) {
					ret.setTimeInPlace(j,tj);
				} else {
					fpln("PlanUtil.fixAccelConsistency ERROR NaN time! j="+Fm0(j));
				}

				if (!Util::within_epsilon(dd+dd2, d, 0.001)) { // check distances are consistent.  if this is the case, something weird is going on...
					fpln("PlanUtil.fixAccelConsistency ERROR i= "+Fm0(i)+" j="+Fm0(j)+" egs="+Fm0(egs));
					fpln("total path dist  = "+Fm3(d));
					fpln("sum of distances = "+Fm3(dd+dd2));
//						Debug.halt();
				}
			}

//				f.pln("fixAccelConsistency i="+i+" dt="+dt+" d="+d+" a="+a+" gs0="+gs0+" gs1="+gs1);
//				f.pln("fixAccelConsistency gsOut BGS = "+ret.gsOut(i));
//				f.pln("fixAccelConsistency  gsIn EGS = "+ret.gsFinalCalc(egs-1, false));

		}
	}
	return ret;
}

NavPoint PlanUtil::closestPoint3D(const Plan& plan, const Position& p, double maxLegLength) {
	if (plan.isLatLon()) {
		Plan pp = Plan(plan);
		int i = 0;
		// cut any leg of size > maxlength in half
		while (i < pp.size()-1) {
			if (pp.pathDistance(i) > maxLegLength) {
				double t = (pp.time(i)+pp.time(i+1))/2;
				Position pos = pp.position(t);
				NavPoint np = NavPoint(pos, t);
				pp.addNavPoint(np);
			} else {
				i++;
			}
		}
		return pp.closestPoint3D(p);
	} else {
		return plan.closestPoint3D(p);
	}
}


}


//Plan PlanUtil::revertTCPs(const Plan& fp) {
//	Plan lpc(fp.getName(),fp.getNote());
//	//fpln(" $$$$$$$$$$$$ removeTCPs: lpc = "+lpc);
//	//if (fp.getType() == Plan::PlanType.LINEAR)  return lpc;
//	//		if (fp.existingVelocityDefined()) lpc.setExistingVelocity(fp.getExistingVelocity());
//	//	std::vector<NavPoint> deleted = fp.getDeletedPoints();
//	//	if (deleted.size() > 0) {
//	//		printError("removeTCPS Warning: source plan "+fp.getName()+" has been modified (points deleted)");
//	//	}
//	for (int i = 0; i < fp.size(); i++) {
//		NavPoint p = fp.point(i);
//		TcpData tcp = fp.getTcpData(i);
//		// case: point it virtual or temp: delete point
//		// case: point is not original or first or last: keep point.
//		// case: point is part of a turn: keep only the vertex point, delete others, if all three are present, otherwise keep remaining
//		// case: point is part of GSC: keep only first point, delete second if both present, otherwise keep remaining
//		// case: point is part of VSC: keep only vertext point, delete others if both present, otherwise keep remaining
//		// don't deal with deleted points quite yet.  this gets really messy.
//		if ((!fp.isVirtual(i) ) ) {
//			if (fp.hasSource(i)){
//				lpc.addNavPoint(NavPoint(tcp.getSourcePosition(),tcp.getSourceTime())); // ,tcp);
//			}
//		}
//	}
//	lpc.getMessage();
//	//lpc.setPlanType(Plan::LINEAR);
//	//fpln(" $$$$$$$$$$$$ removeTCPs: lpc = "+lpc);
//	return lpc;
//}



//void PlanUtil::structRevertTCP(Plan& pln, int ix) {
//	if (ix < 0 || ix >= pln.size()) {
//		pln.addError(" structRevertTCP: index out of range");
//	}
//	if (pln.isBGS(ix)) {
//		bool revertPreviousTurn = true;
//		structRevertGsTCP(pln,ix,revertPreviousTurn);
//	} else if (pln.isBVS(ix)) {
//		pln.structRevertVsTCP(ix);
//	} if (pln.isBOT(ix)) {
//		// store distance from BOT to all points between BOT and EOT
//		bool addBackMidPoints = true;
//		bool killNextGsTCPs = false;
//		//double zVertex = -1;
//		pln.structRevertTurnTCP(ix,addBackMidPoints,killNextGsTCPs);
//
//	} else {
//		pln.addError(" structRevertTCP: index must be a begin TCP!!!" );
//	}
//	// return new Plan();
//}

//void PlanUtil::structRevertTCPs(Plan& pln, bool removeRedPoints ) {
//	//fpln(" ----------------------- structRevertVsTCP pass ------------------------");
//	//double zVertex = -1;
//	for (int i = pln.size()-2; i > 0; i--) {
//		pln.structRevertVsTCP(i);
//	}
//	//fpln(" ----------------------- structRevertTurnTCP pass ------------------------");
//	for (int i = pln.size()-2; i > 0; i--) {
//		bool addBackMidPoints = true;
//		bool killNextGsTCPs = true;
//		pln.structRevertTurnTCP(i,addBackMidPoints,killNextGsTCPs);
//	}
//	//fpln(" ----------------------- structRevertGsTCP pass ------------------------");
//	//for (int i = pln.size()-2; i > 0; i--) {
//	for (int i = 0; i < pln.size(); i++) {
//		bool revertPreviousTurn = true;
//		structRevertGsTCP(pln,i,revertPreviousTurn);
//	}
//	//fpln(" ----------------------- removeRedundantPoints pass ------------------------");
//	if (removeRedPoints) pln.removeRedundantPoints(0,pln.size()-1);
//}


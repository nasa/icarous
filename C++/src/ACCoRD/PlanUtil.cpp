
/*
 * PlanUtil.cpp - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2017 United States Government as represented by
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


//const double PlanUtil::revertGsTurnConnectionTime = 5.0;


//void PlanUtil::savePlan(const Plan& plan, std::string fileName) {
//	if (plan.getName() == "") {
//		//plan.setName("noname");    // removed because const nature of plan
//	}
//	PlanWriter pw;
//	pw.setPrecision(12);
//	pw.open(fileName);
//	pw.writePlan(plan,true);
//	pw.close();
//	if (pw.hasMessage()) {
//		fdln(pw.getMessage());
//	}
//}


bool PlanUtil::gsConsistent(const Plan& p, int ixBGS, double distEpsilon, bool silent) {
	if ( ! p.isBGS(ixBGS)) return true;
	//fpln("$$$ gsConsistent (0): accelEpsilon = "+Fm8(accelEpsilon));
    //bool rtn = true;
	NavPoint BGS = p.point(ixBGS);
	int ixEGS = p.nextEGS(ixBGS); //fixed
	NavPoint EGS = p.point(ixEGS);
	double gsOutBGS = p.gsOut(ixBGS);
    bool rtn = true;
	double dt = EGS.time() - BGS.time();
	double aBGS = p.gsAccel(ixBGS);
	double ds = gsOutBGS*dt + 0.5*aBGS*dt*dt;
	double distH = 	p.pathDistance(ixBGS,ixEGS);
	double absDiff = std::abs(ds-distH);
	//fpln(" ds = "+Units::str("nm",ds)+ " distH = "+Units::str("nm",distH)+" absDiff = "+Units::str15("m",absDiff));
	if (absDiff > distEpsilon) { // See testGsSimple for worst case
		if (!silent)
			fpln(" >>> isConsistent GS FAIL! at i = "+Fm0(ixBGS)+" GSC section fails Test! absDiff = "+Units::str("nm",absDiff));
		rtn = false;
	}
	//fpln(" $$$ gsConsistent: EXIT rtn = "+bool2str(rtn));
	return rtn;
}




bool PlanUtil::vsConsistent(const Plan& p, int ixBVS, double distEpsilon, double a,  bool silent) {
	if ( ! p.isBVS(ixBVS)) {
		return true;
	}
	// must be a BVS
	NavPoint VSCBegin = p.point(ixBVS);
	int ixEVS = p.nextEVS(ixBVS);//fixed
	NavPoint VSCEnd = p.point(ixEVS);//fixed
	double dt = VSCEnd.time() - VSCBegin.time();
	double ds = p.vsOut(ixBVS)*dt + 0.5*a*dt*dt;
	//double distV = vertDistance(i,nextVSCEnd(i));
	double deltaAlts = VSCEnd.alt() - VSCBegin.alt();
	double absDiff = std::abs(ds-deltaAlts);
	//fpln(" $$$ vsConsistent i = "+Fm0(i)+" dt = "+dt+" ds = "+Units::str("ft",ds,8)+ " distV = "+Units::str("ft",distV,8)+" absDiff = "+Units::str("ft",absDiff,8));
	if (absDiff > distEpsilon) {
		if (!silent) fpln(" >>> isConsistent VS FAIL!  at i = "+Fm0(ixBVS)+" VSC Section fails Test! absDiff = "+Units::str("m",absDiff,8));
		fpln(" >>> isConsistent ixBVS = "+Fm0(ixBVS)+" absDiff = "+Units::str("ft",absDiff,8));
		return false;
	}
	return true;
}




bool PlanUtil::turnConsistent(const Plan& p, int i, double distH_Epsilon, bool silent) {
	bool rtn = turnCenterConsistent(p,i,distH_Epsilon,silent);
	if ( ! p.isBOT(i)) return true;
	NavPoint BOT = p.point(i);
	int ixEOT = p.nextEOT(i);//fixed
	NavPoint EOT = p.point(ixEOT);
	//Velocity vin = p.point(i).velocityInit();
	//Velocity vout = p.initialVelocity(ixEOT);
	double pathDist = p.pathDistance(i,ixEOT);
	double signedRadius = p.signedRadius(i);
	Position center = p.turnCenter(i);
	int dir = Util::sign(signedRadius);
	Position tAtd = KinematicsPosition::turnByDist2D(BOT.position(), center, dir, pathDist);
	Position EOTcalc = tAtd.mkAlt(EOT.alt());
	if (!EOT.position().almostEquals2D(EOTcalc,distH_Epsilon)) { //       (p,0.005,1.2)) {
		if ( ! silent) {
			fpln(" >>> turnConsistent: TURN FAIL! i = "+Fm0(i)+" calculated pos = "+EOTcalc.toString(8)
					+ "\n                                             plan EOT = "+EOT.position().toString(8));
			double distanceH = EOT.position().distanceH(EOTcalc);
			//double distanceV = EOT.position().distanceV(EOTcalc);
			fpln("            .... distanceH = "+Units::str("m",distanceH,8));
		}
		rtn = false;
	}
	return rtn;
}

bool PlanUtil::isTrkContinuous(const Plan& p, int i, double trkEpsilon, bool silent) {
	bool rtn = true;
	double trkIn = p.trkIn(i);
	double trkOut = p.trkOut(i);
	double trkDelta = Util::turnDelta(trkIn,trkOut);
	if ( std::abs(trkDelta) > trkEpsilon) {
		if (!silent) fpln(" $$ isTrkContinuous: FAIL trkDelta ("+Fm0(i)+") = "+Units::str("deg",trkDelta));
		rtn = false;
	}
    return rtn;
}

bool PlanUtil::isGsContinuous(const Plan& p, int i, double gsEpsilon, bool silent) {
	bool rtn = true;
	double gsIn =   p.gsIn(i);
	double gsOut = p.gsOut(i);
	double gsDelta = gsOut - gsIn;
	if (std::abs(gsDelta) > gsEpsilon) {
		if (!silent) {
			fpln(" $$ isGsContinuous: FAIL gsIn ("+Fm0(i)+") = "+Units::str("kn",gsIn)+"  gsOut ("+Fm0(i)+") = "+Units::str("kn",gsOut)+" gsDelta = "+Units::str("kn",gsDelta));
		}
		rtn = false;
	}
    return rtn;
}

bool PlanUtil::isVsContinuous(const Plan& p, int i, double velEpsilon, bool silent) {
	bool rtn = true;
	double vsIn  = p.vsIn(i);
	double vsOut = p.vsOut(i);
	double vsDelta = vsIn - vsOut;
	if (std::abs(vsDelta) > velEpsilon) {
		if (!silent) {
			fpln(" $$ isVsContinuous: FAIL vsIn ("+Fm0(i)+") = "+Units::str("fpm",vsIn)+" vsOut ("+Fm0(i)+") = "+Units::str("fpm",vsOut)+" vsDelta  = "+Units::str("fpm",vsDelta));
		}
		rtn = false;
	}
    return rtn;
}




//bool PlanUtil::isVelocityContinuous(const Plan& p, int i, double velEpsilon, bool silent) {
//	bool rtn = isTrkContinuous(p,i,velEpsilon,silent)
//               && isGsContinuous(p,i,velEpsilon,silent)
//			   && isVsContinuous(p,i,velEpsilon,silent);
//	return rtn;
//}

bool PlanUtil::turnCenterConsistent(const Plan& p, int i, double distH_Epsilon, bool silent) {
	bool rtn = true;
	if (p.inTrkChange(p.point(i).time())) {
		int ixBOT = p.prevBOT(i+1);
		Position center = p.turnCenter(ixBOT);
		double distanceFromCenter = p.point(i).position().distanceH(center);
		double turnRadius = p.getTcpData(ixBOT).turnRadius();
		double deltaRadius = distanceFromCenter - turnRadius;
		if (std::abs(distanceFromCenter - turnRadius) > distH_Epsilon) {
			if ( ! silent) {
				fpln(" >>> checkWithinTurn: "+p.getName()+" POINT OFF CIRCLE at i = "+Fm0(i)+" deltaRadius = "+Units::str("NM",deltaRadius));
			}
			rtn = false;
		}
	}
	return rtn;

}


/**  timeShift points in plan by "dt" starting at index "start"
 *   Note: This will drop any points that become negative or that become out of order using a negative dt
 *   See also Plan.timeShiftPlan for equivalent method
 *
 * @param p       plan
 * @param start   starting index
 * @param dt      delta time
 * @return        time-shifted plan
 */
Plan PlanUtil::timeShift(const Plan& p, int start, double dt) {
	Plan np = Plan(p.getName());
	double lastTime = -1; // ensure overlapping points are not included; also fixes start=0 and dt=0 problem
	for (int i = 0; i < p.size(); i++) {
		if (i < start) {
			np.add(p.get(i));
			lastTime = p.point(i).time();
		} else {
			double t = p.time(i) + dt;
			NavPoint nav = p.point(i).makeTime(t);
			if (t >= 0 && t > lastTime) {
				np.add(nav,p.getTcpData(i));
				lastTime = t;
			}
		}
	}
	return np;
}

Plan PlanUtil::makeSourceNew(const Plan& lpc) {
	Plan npc = Plan(lpc.getName(),lpc.getNote());
	for (int i = 0; i < lpc.size(); i++) {
		NavPoint np = lpc.point(i);
		TcpData tcp = lpc.getTcpData(i);
		tcp.reset();
		npc.add(np, tcp);                  // TODO: is this tight?
	}
	return npc;
}


Position PlanUtil::advanceDistanceInSeg(const Plan& p, double curTm, double advDistance, bool linear) {
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
 	Velocity vo = positionVelocity.second;
 	//fpln(" $$ %%%% advanceDistanceInSeg: currentTime = "+f.Fm4(currentTime)+" vo = "+vo);
 	Position sNew;
 	if (p.inTrkChange(curTm) & !linear) {
 		int ixPrevBOT = p.prevBOT(seg+1);//fixed
 		Position center = p.turnCenter(ixPrevBOT);
 		double signedRadius = p.signedRadius(ixPrevBOT);
 		int dir = Util::sign(signedRadius);
 		double gsAt_d = -1.0;             // THIS IS ONLY USED IN THE VELOCITY CALCULATION WHICH WE ARE NOT USING
 		std::pair<Position,Velocity> tAtd = KinematicsPosition::turnByDist2D(so, center, dir, advDistance, gsAt_d);
 		sNew = tAtd.first;
  		//fpln(" $$ %%%% advanceDistanceInSeg A: sNew("+f.Fm2(currentTime)+") = "+sNew);
 	} else {
 		//fpln("\n\n $$ %%%% advanceDistanceInSeg B1: currentTime = "+currentTime+" seg = "+seg+"  distFromSo = "+Units::str("NM",distFromSo));
 		double track = vo.trk();  // TODO:  look into getting track another way
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



 std::pair<Position,int> PlanUtil::advanceDistance(const Plan& p, double currentTime, double advanceDist, bool linear) {
 	Position sNew;
 	int initSeg = p.getSegment(currentTime);
 	//fpln(" $$::::::::::: advanceDistance AA: initSeg = "+initSeg+" p.size = "+p.size()+"  advanceDist = "+Units::str("NM",advanceDist));
 	double distLeftInSeg0 = p.partialPathDistance(currentTime,linear);
     //fpln("distLeftInSeg0="+distLeftInSeg0);
 	int finalSeg = initSeg;
 	//fpln(" $$::::::::::: advanceDistance BB: initSeg = "+initSeg+"  distLeftInSeg0 = "+Units::str("NM",distLeftInSeg0));
 	if (advanceDist < distLeftInSeg0) {  //  new position remains in segment "seg"
 		//fpln(" $$::::::::::: advanceDistance 00000000: initSeg = "+initSeg+"  advanceDist = "+Units::str("NM",advanceDist)+" distLeftInSeg0 = "+Units::str("NM",distLeftInSeg0));
 		sNew = advanceDistanceInSeg(p, currentTime, advanceDist, linear);
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
 		//fpln("\n $$ +++++++++++++ advanceDistance 11111111: finalSeg = "+finalSeg+" remainingDist = "+Units::str("NM",remainingDist));
 		//fpln(" $$ advanceDistance B1: remainingDist = "+Units::str("NM",remainingDist));
 		// need to go remainingDist in segment j
 		if (finalSeg >= p.size()-1) {
 			return std::pair<Position,int>(p.getLastPoint().position(),p.size()-1);
 		} else {
 			double t0 = p.time(finalSeg);
 			//fpln(" $$ advanceDistance B2: j = "+j+" t0 = "+t0+" remainingDist = "+Units::str("NM",remainingDist));
 			sNew = advanceDistanceInSeg(p, t0, remainingDist, linear);
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
	//double vo = initialVelocity(seg).gs();
	//double a = point(prevBGS(seg)).gsAccel();
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

bool PlanUtil::checkMySolution(const Plan& solution, double currentTime, const Position& currentPos, const Velocity& currentVel) {
	bool ok = true;
	if (!solution.position(currentTime).almostEquals(currentPos)) {
		std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
		std::cout << " ............... ERROR: moved location of current position! currentTime = "<<currentTime<<" ......." << std::endl;
		std::cout << " ............... from " << currentPos.toString() <<" to "<<solution.position(currentTime).toString()<<std::endl;
		std::cout << "----------------------------------------------------------------------------------\n" << std::endl;
		ok = false;
	}
	if (!solution.velocity(currentTime).within_epsilon(currentVel,0.10)) {
		std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
		std::cout << " ............... ERROR: changed **velocity** of current position! currentTime = "<<currentTime<<" ...." << std::endl;
		std::cout << " ............... from " << currentVel.toString() << " to " << solution.velocity(currentTime).toString()<<std::endl;
		std::cout << "----------------------------------------------------------------------------------\n" << std::endl;
		ok = false;
	}
	if (!solution.isWellFormed()) {
		std::cout << "\n---------------------------------------------------------------------------------" << std::endl;
		std::cout << " ............... ERROR: solution is not wellFormed currentTime = "<<currentTime<<" ...." << std::endl;
		std::cout << " ..............."<<solution.strWellFormed() << std::endl;
		std::cout << " mySolution = "<<solution.toString() << std::endl;
		std::cout << "----------------------------------------------------------------------------------\n" << std::endl;
		ok = false;
	}
	return ok;
}


// from Aviation Formulary
// longitude sign is reversed from the formulary!
double PlanUtil::lonCross(const Plan& ac, int i, double lat3) {
	double lat1 = ac.point(i).lat();
	double lon1 = ac.point(i).lon();
	double lat2 = ac.point(i+1).lat();
	double lon2 = ac.point(i+1).lon();
	double tc = ac.initialVelocity(i).compassAngle();
	bool NW = (tc > Pi/2 && tc <= Pi) || tc >= 3*Pi/2;
	double l12;

	if (NW) l12 = lon1-lon2;
	else l12 = lon2-lon1;

	double A = std::sin(lat1)*std::cos(lat2)*std::cos(lat3)*std::sin(l12);
	double B = std::sin(lat1)*std::cos(lat2)*std::cos(lat3)*std::cos(l12) - std::cos(lat1)*std::sin(lat2)*std::cos(lat3);
	double lon;

	if (NW) lon = lon1 + std::atan2(B,A) + Pi;
	else lon = lon1 - std::atan2(B,A) - Pi;

	if (lon >= 2*Pi) lon = lon-2*Pi;

	if (NW) {
		lon = lon-Pi;
	} else {
		lon = Pi+lon;
	}

	if (lon < -Pi) lon = 2*Pi+lon;
	if (lon > Pi) lon = -2*Pi+lon;

	return lon;
}


// from Aviation Formulary
double PlanUtil::latMax(const Plan& ac, int i) {
	double tc = ac.initialVelocity(i).compassAngle();
	double lat = ac.point(i).lat();
	double ret = std::acos(std::abs(std::sin(tc)*std::cos(lat)));
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
			//cout << "NEWTIME " << ac << " " << i << " " << t << endl;
			ac.setNavPoint(i+1, ac.point(i+1).makeLabel("maxlat")
			);
			//  	  modified = true;
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
		//NavPoint src = ac.point(ac.getSegment(time));
		//NavPoint np = src.makeStandardRetainSource().makePosition(ac.position(time)).makeTime(time);
		//TcpData tcp = TcpData::makeOrig().makeVirtual();
		//return ac.add(np,tcp);

		//NavPoint src = ac.point(ac.getSegment(time));
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




// this adds for the leg starting at or before startTm, and ending before or at endTm
void PlanUtil::interpolateVirtuals(Plan& ac, double accuracy, double startTm, double endTm) {
	if(ac.isLatLon()) {
		if (startTm < ac.getFirstTime()) startTm = ac.getFirstTime();
		if (endTm > ac.getLastTime()) endTm = ac.getLastTime();
		insertLocalMax(ac);
		int i = ac.getSegment(startTm);
		double mindist = ac.pathDistance() / 900;
		while (i < ac.size()-1 && ac.time(i) <= endTm) {
			double dist = ac.pathDistance(i);
			double legDist = getLegDist(ac,i,accuracy,mindist);
			//Special case for kinematic plans in acceleration zones -- ensure there are at least 2 mid-points
			// TODO: Possibly get rid of this
			//				if (!ac.isLinear()) {
			//					Plan kpc = ac;
			//					if (kpc.point(i).isTurnBegin() || kpc.point(i).isTurnMid()) {
			//						legDist = Util::min(legDist, dist/3.0);
			//					}
			//				}
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
//			if (ac.point(i).isFixed()) {
//				rtn = false;
//				i++;
//			} else {
				ac.remove(i);
//			}
		} else {
			i++;
		}
	}
//	noLongLegs[ac] = false;
//	interpolated[ac] = -1.0;
//	modified = true;
	return rtn;
}


bool PlanUtil::removeVirtuals(Plan& ac) {
	return removeVirtualsRange(ac, ac.getFirstTime(), ac.getLastTime(), false);
}


Plan PlanUtil::revertTCPs(const Plan& fp) {
	Plan lpc(fp.getName(),fp.getNote());
	//fpln(" $$$$$$$$$$$$ removeTCPs: lpc = "+lpc);
	//if (fp.getType() == Plan.PlanType.LINEAR)  return lpc;
	//		if (fp.existingVelocityDefined()) lpc.setExistingVelocity(fp.getExistingVelocity());
	//	std::vector<NavPoint> deleted = fp.getDeletedPoints();
	//	if (deleted.size() > 0) {
	//		printError("removeTCPS Warning: source plan "+fp.getName()+" has been modified (points deleted)");
	//	}
	for (int i = 0; i < fp.size(); i++) {
		NavPoint p = fp.point(i);
		TcpData tcp = fp.getTcpData(i);
		// case: point it virtual or temp: delete point
		// case: point is not original or first or last: keep point.
		// case: point is part of a turn: keep only the vertex point, delete others, if all three are present, otherwise keep remaining
		// case: point is part of GSC: keep only first point, delete second if both present, otherwise keep remaining
		// case: point is part of VSC: keep only vertext point, delete others if both present, otherwise keep remaining
		// don't deal with deleted points quite yet.  this gets really messy.
		if ((!fp.isVirtual(i) ) ) {
			if (fp.hasSource(i)){
				lpc.addNavPoint(NavPoint(tcp.getSourcePosition(),tcp.getSourceTime())); // ,tcp);
			}
		}
	}
	lpc.getMessage();
	//lpc.setPlanType(Plan::LINEAR);
	//fpln(" $$$$$$$$$$$$ removeTCPs: lpc = "+lpc);
	return lpc;
}



/** *********** UNDER DEVELOPMENT *************
 * Revert the TCP pair in a plan structurally.  Properly relocate all points between TCP pair.
 *
 * Note.  No check is made to insure that start or upto is not in the middle of a TCP.
 */
void PlanUtil::structRevertTCP(Plan& pln, int ix) {
	if (ix < 0 || ix >= pln.size()) {
		pln.addError(" structRevertTCP: index out of range");
	}
	if (pln.isBGS(ix)) {
		bool revertPreviousTurn = true;
		structRevertGsTCP(pln,ix,revertPreviousTurn);
	} else if (pln.isBVS(ix)) {
		pln.structRevertVsTCP(ix);
	} if (pln.isBOT(ix)) {
		// store distance from BOT to all points between BOT and EOT
		bool addBackMidPoints = true;
		bool killNextGsTCPs = false;
		//double zVertex = -1;
		pln.structRevertTurnTCP(ix,addBackMidPoints,killNextGsTCPs);

	} else {
		pln.addError(" structRevertTCP: index must be a begin TCP!!!" );
	}
	// return new Plan();
}

void PlanUtil::structRevertTCPs(Plan& pln, bool removeRedPoints ) {
	//fpln(" ----------------------- structRevertVsTCP pass ------------------------");
	//double zVertex = -1;
	for (int i = pln.size()-2; i > 0; i--) {
		pln.structRevertVsTCP(i);
	}
	//fpln(" ----------------------- structRevertTurnTCP pass ------------------------");
	for (int i = pln.size()-2; i > 0; i--) {
		bool addBackMidPoints = true;
		bool killNextGsTCPs = true;
		pln.structRevertTurnTCP(i,addBackMidPoints,killNextGsTCPs);
	}
	//fpln(" ----------------------- structRevertGsTCP pass ------------------------");
	//for (int i = pln.size()-2; i > 0; i--) {
    for (int i = 0; i < pln.size(); i++) {
		bool revertPreviousTurn = true;
		structRevertGsTCP(pln,i,revertPreviousTurn);
	}
	//fpln(" ----------------------- removeRedundantPoints pass ------------------------");
	if (removeRedPoints) pln.removeRedundantPoints(0,pln.size()-1);
}

/** structurally revert all TCPS that create acceleration zones containing ix
 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain
 *  original ground speed into the point after ix.
 *
 *  NOTE This method does not depend upon source time!!
 *
 * @param ix  The index of one of the TCPs created together that should be reverted
 * @return index of the reverted point
 */
int PlanUtil::structRevertGroupOfTCPs(Plan& pln, int ix, bool killAllOthersInside) {
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
    	fpln(" !! structRevertGroupOfTCPs: ERROR cannot remove first point 1");
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
        	zVertex = pln.structRevertVsTCP(ii);
        	if (zVertex >= 0) {
        		lastInGroup--;
        		//if (ii == firstInGroup) firstInGroup++;
        	}
        }
	}
 	// get rid of all ground speed TCPs if this is fundamentally a turn
	if (pln.isTrkTCP(firstInGroup)) {
    	bool killNextGsTCPs = true;
    	bool addBackMidPoints = false;
    	pln.structRevertTurnTCP(firstInGroup, addBackMidPoints, killNextGsTCPs);
	}
	if (pln.isGsTCP(firstInGroup)) {  // does not revert previous turn
		//fpln(" $$$$ structRevertGroupOfTCPs: GS section: firstInGroup = "+firstInGroup);
		bool revertPreviousTurn = true;
		firstInGroup = structRevertGsTCP(pln,firstInGroup,revertPreviousTurn);
	}
 	//fpln(" $$$$$$$$$$$$$ firstInGroup = "+firstInGroup);
    return firstInGroup;
}



// assumes ix > 0 AND ix < pln.size()
int PlanUtil::structRevertGsTCP(Plan& pln, int ix, bool revertPreviousTurn) {
	//fpln(" $$$$>>>>>>>>> structRevertGsTCP: pln.point("+ix+") = "+pln.point(ix).toStringFull());
	if (! pln.validIndex(ix)) return -1;
	if (pln.isBGS(ix)) {
		int ixPrev = pln.prevTCP(ix);
		int prevLinIndex = -1;
		if (ixPrev >= 0) prevLinIndex = pln.getTcpData(ixPrev).getLinearIndex();
		if (revertPreviousTurn && ixPrev >= 0 && pln.isEOT(ixPrev) && pln.getTcpData(ix).getLinearIndex() == prevLinIndex) {
			bool killNextGsTCPs = true;
			int ixPrevBOT = pln.prevBOT(ixPrev);//fixed
			//fpln(" $$$$$$$$$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
			bool addBackMidPoints = false;
			pln.structRevertTurnTCP(ixPrevBOT, addBackMidPoints, killNextGsTCPs);
			//fpln(" $$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
			return ixPrevBOT;
		} else {
			bool saveAccel = false;
			pln.structRevertGsTCP(ix, saveAccel);
		}
	}
	return ix;
}


/**
 * change ground speed to newGs, starting at startIx
 * @param p
 * @param newGs
 * @param startIx
 * @return
 */
Plan PlanUtil::makeGsConstant(const Plan& p, double newGs, int startIx) {
	//fpln(" makeGSConstant: newGs = "+Units::str("kn",newGs,4)+"  startIx = "+startIx);
	int ix;
	if (p.isBVS(startIx)) {
		ix = startIx;
	} else {
		ix = p.nextBVS(startIx); // fixed
	}
	double vsAccel = 1.0;
	if (ix >= 0) {
		vsAccel = p.vsAccel(ix);
	}
	Plan kpc = p;
	kpc.revertVsTCPs(startIx,kpc.size()-1);
	bool saveAccel = false;
	kpc.revertGsTCPs(startIx,saveAccel);

	if (Util::almost_equals(newGs,0.0)) {
		kpc.addError(" 	makeGSConstant_No_Verts: newGs cannot be zero1");
		return kpc;
	}
	for (int i = startIx; i < kpc.size(); i++) {
		//fpln(" $$$$ makeGSConstant: i = "+i+" kpc.point(i) = "+kpc.point(i).toStringFull());
        kpc.mkGsOut(i,newGs);
	}
	Plan vpc = kpc;
	if (ix >= 0) {
		bool continueGen = false;
		vpc = TrajGen::generateVsTCPs(kpc,vsAccel,continueGen);
	}
	return vpc;
}




Plan PlanUtil::cutDown(const Plan& plan, double startTime, double endTime) {
	Plan rtn(plan.getName(),plan.getNote());
	Position startPos = plan.position(startTime);
	NavPoint start(startPos, startTime);
	rtn.addNavPoint(start);
	for (int i = 0; i < plan.size(); i++) {
		std::pair<NavPoint,TcpData> pi = plan.get(i);
		if (startTime < pi.first.time() && pi.first.time() < endTime ) {
			rtn.add(pi);
		}
	}
	Position endPos = plan.position(endTime);
	NavPoint end(endPos, endTime);
    rtn.addNavPoint(end);
	return rtn;
}


/** This method cuts a Plan so that the acceleration information after intentThreshold is discarded.  The plan
 *  is continued linearly to time tExtend.  The first time point of the new plan is the
 *  NavPoint before timeOfCurrentPosition in the plan.  The  intentThreshold and tExtend times are absolute.
 *
 * @param plan                      Plan file to be cut
 * @param timeOfCurrentPosition     Current location of aircraft in the plan file
 * @param intentThreshold           the absolute lookahead time -- all acceleration information after this time is not copied
 * @param tExtend                   After the intentThreshold, the plan is extended linearly to this time (absolute time)
 * @return
 */
Plan PlanUtil::cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold, double tExtend) {
	Plan nPlan(plan.getName(),plan.getNote());
	if (intentThreshold < 0) {
		fpln(" $$$ ERROR:  negative intentThreshold = "+Fm1(intentThreshold));
		return nPlan;
	}
	if (plan.size() == 0) return plan;
    //fpln(" $$$$$$$$$$$$$$$$ plan = "+plan);
	int ixInit = 0;
	bool inAccel = false;
	NavPoint firstPoint = plan.point(0);
	if (timeOfCurrentPosition >= firstPoint.time()) {
		inAccel = plan.inAccel(timeOfCurrentPosition);
		//if (inAccel) ix = prevNonTCP(ix);
		ixInit = plan.getSegment(timeOfCurrentPosition);
		//fpln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> timeOfCurrentPosition = "+timeOfCurrentPosition+" ixInit = "+ixInit);
		if (ixInit < 0) {
			return Plan(""); // TODO -- what do we really want to do here ??
		}
		//fpln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> inAccel = "+inAccel);
		if (inAccel) ixInit = plan.prevTCP(ixInit+1);            // NOTE THE ixInit+1
	}
	//fpln("\n $$$$ cutDownTo: ixInit = "+ixInit+" timeOfCurrentPosition = "+timeOfCurrentPosition+" intentThreshold = "+intentThreshold);
	// ADD FIRST POINT
	//firstPoint = plan.point(ixInit);
	//if (plan.isEndTCP(ixInit)) firstPoint = firstPoint.makeNewPoint();
	//nPlan.add(firstPoint);

	std::pair<NavPoint,TcpData> firstPair = plan.get(ixInit);
	firstPoint = firstPair.first;
	TcpData firstPoint_tcp = firstPair.second;
	if (plan.isEndTCP(ixInit)) {
		firstPoint_tcp.reset();
	}
	nPlan.add(firstPoint,firstPoint_tcp);
	//fpln(" $$$$0 cutDownTo: ADD INITIAL point("+ixInit+") = "+firstPoint.toString()+" "+firstPoint_tcp);
	//fpln(" $$$$0 cutDownTo: ADD INITIAL point("+ixInit+") = "+firstPoint.toStringFull());
	int ix = ixInit + 1;
	while (ix < plan.size() && plan.point(ix).time() < intentThreshold) {
		std::pair<NavPoint,TcpData> p = plan.get(ix);
		nPlan.add(p);
		//fpln(" $$$$$$ cutDownTo ADD p = "+p);
		ix++;
	}
	if (ix >= plan.size()) { // plan ran out before intentThreshold
		double dt = intentThreshold - plan.getLastTime();
		Velocity vout = plan.initialVelocity(plan.size()-1);
		NavPoint lastP = plan.point(plan.size()-1);
		NavPoint np = lastP.linear(vout, dt).makeLabel("CutDownTo_0");
		nPlan.addNavPoint(np);
		//fpln(" $$$$$$ cutDownTo ADD np = "+np);
	} else {                // intentThreshold is within plan
		NavPoint newLastPt = NavPoint(plan.position(intentThreshold),intentThreshold).makeLabel("CutDownTo_newLastPt");
		Velocity vout = plan.velocity(intentThreshold);
		if (! plan.inAccel(intentThreshold)) {
			//fpln(" $$$$$$ cutDownTo ADD newLastPtp = "+newLastPt+" vout = "+vout);
			nPlan.addNavPoint(newLastPt);
		} else {
			bool inTurn = plan.inTrkChange(intentThreshold);
			bool inGsAccel = plan.inGsChange(intentThreshold);
			bool inVsAccel = plan.inVsChange(intentThreshold);
			std::string trkType = "NONE";
			std::string gsType = "NONE";
			std::string vsType = "NONE";
            if (inTurn) trkType = "EOT";
            if (inGsAccel) gsType = "EGS";
            if (inVsAccel) vsType = "EVS";
            NavPoint lastP(newLastPt.position(), newLastPt.time(), "CutDownTo_lastP");
            TcpData lastTcp = TcpData::makeFull("Orig",  trkType,  gsType, vsType,
						0.0, Position::ZERO_LL(), 0.0, 0.0, vout, newLastPt.position(), newLastPt.time(), -1);
            nPlan.add(lastP,lastTcp);
            //fpln(" $$$$$$ cutDownTo ADD lastP = "+lastP+" vout = "+vout);
			}
	}
	NavPoint lastPt = nPlan.getLastPoint();
	if (tExtend > intentThreshold && tExtend > lastPt.time()) {
		Velocity vout = nPlan.initialVelocity(nPlan.size()-1);  // final velocity of next to last point
		double dt = tExtend - lastPt.time();
		NavPoint extendPt = lastPt.linear(vout, dt).makeLabel("CutDownTo_extendPt");
		nPlan.addNavPoint(extendPt);
	}
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
		double gsIn = p.finalVelocity(ix-1).gs();
		double calcTimeGSin = p.calcTimeGSin(ix+1,gsIn);
		double correction = calcTimeGSin - p.time(ix+1);
		p.timeShiftPlan(ix+1,correction);
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
	p = linearMakeGSConstant(p);
	//fpln(" $$$$ END: unZigZag: p = "+p);
	return p;
}

bool PlanUtil::aboutTheSameTrk(const Velocity& v1, const Velocity& v2, double sameTrackBound) {
	return std::abs(v1.trk()- v2.trk()) < sameTrackBound;
}

Plan PlanUtil::removeCollinearTrk(const Plan& pp, double sameTrackBound){
	if (pp.size() < 1) return Plan(pp.getName());
	//fpln(" $$ removeCollinearTrk: pp = "+pp);
	Plan p(pp.getName(),pp.getNote());
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





///**
// *
// * Returns a new Plan that sets all points in a range to have a constant GS.
// * THIS ASSUMES NO VERTICAL TCPS.
// *
// * NOTE.  First remove Vertical TCPS then re-generate Vertical TCPs
// * */
//Plan PlanUtil::makeGSConstant_NO_Verts(const Plan& p, double newGs) {
//	//fpln(" $$ makeGSConstant: newGs = "+Units::str4("kn",newGs)+" "+Fm0(p.getType()));
//	Plan kpc(p.getName());
//	//kpc.setPlanType(p.getType());
//	//double time0 = point(0).time();
//	double lastTime = p.point(0).time();
//	kpc.add(p.point(0));
//	int j = 0;   // last point included in new file (note GS TCPs are removed)
//	for (int i = 1; i < p.size(); i++) {
//		NavPoint np = p.point(i);
//		TcpData tcp = p.getTcpData(i);
//		//fpln(" $$$$ makeGSConstant: i = "+Fm0(i)+" np = "+np.toStringFull());
//		// skip any GSC points (i.e. remove them)
//		if (! tcp.isGsTCP()) {
//			double d = p.pathDistance(j,i);
//			double dt = d/newGs;
//			double nt = lastTime + dt;
//			//fpln(" $$$$ makeGSConstant: d = "+Units::str("nm",d)+" dt = "+Fm4(dt)+" nt = "+Fm4(nt));
//			lastTime = nt;
//			if (tcp.isTCP()) {        // assume only turn TCPs allowed
//				if (!tcp.isTrkTCP()) {
//					//fpln("makeGSConstant: start point within acceleration zone");
//					p.addError("TrajGen.genGSConstant: start point within acceleration zone");
//				}
//				//fpln(" $$$ makeGSConstant(TCP) for i = "+Fm0(i)+" BEFORE np.turnRadius = "+Fm4(np.turnRadius()));
//				double oldGs = p.velocityInit(i).gs();
//				double newVsIn = p.vertDistance(j,i)/dt;
//				np = np.makeVelocityInit(p.velocityInit(i).mkGs(newGs).mkVs(newVsIn));
//				//fpln(" makeGSConstant(TCP): make point i = "+Fm0(i)+" have velocityIn = "+Units::str4("kn",newGs));
//				// calculate new acceleration: newAccel = oldAccel * oldTime/newTime
//				double k = newGs/oldGs;
//				double newAccel; // = np.accel() * k;
//				if (tcp.isBOT()) {                                 // ***RWB*** KCHANGE
//					newAccel = p.trkAccel(i) * k;
//					//fpln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE Trkaccel from "+np.trkAccel()+" to "+newAccel);
//			    	np = np.makeTrkAccel(newAccel); // modify turn omega to match new gs in
//				} else if (tcp.isBGS()) {
//					newAccel = p.gsAccel(i) * k;
//					//fpln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE gsAccel from "+np.gsAccel()+" to "+newAccel);
//			    	np = np.makeGsAccel(newAccel); // modify gsAccel
//				} else if (tcp.isBVS()) {
//					newAccel = p.vsAccel(i) * k;
//					//fpln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE vsAccel from "+np.vsAccel()+" to "+newAccel);
//			    	np = np.makeVsAccel(newAccel); // modify turn omega to match new gs in
//				}
//				//fpln(" $$$ makeGSConstant(TCP) for i = "+Fm0(i)+" CHANGE accel from "+Fm4(np.accel())+" to "+Fm4(newAccel));
//				//np = np.makeAccel(newAccel);
//				//fpln(" $$$ makeGSConstant(TCP) for i = "+Fm0(i)+" AFTER np.turnRadius = "+Fm4(np.turnRadius()));
//				// NOTE: we need to recalculate times of MOT and BOT, we are preserving turn radius
//			}
//			NavPoint newNp = np.makeTime(nt);
//			//fpln(" $$$ makeGSConstant for i = "+Fm0(i)+" CHANGE time from "+Fm4(p.point(i).time())+" to "+Fm4(newNp.time()));
//			//fpln(" $$$ makeGSConstant add "+newNp.toString());
//			kpc.add(newNp);
//			j = i;
//		}
//	}
//	return kpc;
//}
//
//Plan PlanUtil::makeGSConstant_No_Verts(const Plan& p) {
//	double dt = p.getLastTime() - p.getFirstTime();
//	double newGS = p.pathDistance()/dt;
//	return makeGSConstant_NO_Verts(p, newGS);
//}
//

// This methods assumes plan is linear
Plan PlanUtil::linearMakeGSConstant(const Plan& p, int wp1, int wp2, double gs) {
	//fpln("%%## makeGSConstant:  wp1 = "+wp1+" wp2 = "+wp2+ " gs = "+Units::str("kn",gs));
	//fpln(" ENTER linearMakeGSConstant p = "+p);
	if (gs == 0.0) {
		p.addError("PlanUtil::linearMakeGSConstant: cannot accept gs = 0");
		return p;
	}
	//DebugSupport.dumpPlan(p,"linearMakeGSConstant");
	if (wp1 < 0) return p;
	if (wp2 >= p.size()) wp2 = p.size()-1;
	//int start = wp1;
	//int end = wp2;
	if (wp1 >= wp2) return p;
	Plan rtnPln(p.getName(),p.getNote());
	for (int i = 0; i < wp1+1; i++) {
		//fpln(" $$$$$ makeGSConstant  ADD  "+p.point(i));
		rtnPln.add(p.get(i));
	}
	double lastTime = p.time(wp1);
	for (int i = wp1; i < wp2; i++) {
		double dt = p.pathDistance(i,i+1)/gs;
		double newTime = lastTime+dt;
		NavPoint np = p.point(i+1).makeTime(newTime);
		//fpln(" $$$$$>>> makeGSConstant ADD  "+np);
		rtnPln.add(np, p.getTcpData(i+1));
		lastTime = newTime;
	}
	for (int i = wp2+1; i < p.size(); i++) {
		//fpln(" $$$$$... makeGSConstant ADD  "+p.point(i));
		double originalGs = p.initialVelocity(i-1).gs();
		double d = p.point(i-1).distanceH(p.point(i));
		//fpln(" $$$$ calcTimeGSin: d = "+Units::str("nm",d));
		double newTime = rtnPln.point(i-1).time() + d/originalGs;
		//fpln(i+" $$$ originalGs = "+Units::str("kn", originalGs)+" newTime = "+newTime);
		NavPoint np = p.point(i).makeTime(newTime);
		//fpln(" $$$$$AFTER linearMakeGSConstant ADD Back  i = "+i+": "+np);
		rtnPln.add(np,p.getTcpData(i));
		//rtnPln.add(p.point(i));
	}
	//fpln(" linearMakeGSConstant a = "+a);
	return rtnPln;
}


Plan PlanUtil::linearMakeGSConstant(const Plan& p, double gs) {
	return linearMakeGSConstant(p,0,p.size()-1,gs);
}

Plan PlanUtil::linearMakeGSConstant(const Plan& p) {
	double dtot = p.pathDistance(0,p.size()-1);
	double ttot = p.time(p.size()-1) - p.time(0);
	double gs = dtot/ttot;
	return linearMakeGSConstant(p, 0,p.size()-1,gs);
}


Plan PlanUtil::linearMakeGSConstant(const Plan& p, int wp1, int wp2) {
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
	return linearMakeGSConstant(p, wp1,wp2,gs);
}


/** change the ground speed into ix to be gs -- all other ground speeds remain the same
 *
 * @param p    Plan of interest
 * @param ix   index
 * @param gs   new ground speed
 * @return     revised plan
 */
void PlanUtil::linearMakeGsInto(Plan& p, int ix, double gs) {
	if (ix > p.size() - 1) return;
	double tmIx = p.linearCalcTimeGSin(ix,gs);
	p.timeShiftPlan(ix,tmIx - p.point(ix).time());
}



// change vertical profile: adjust altitudes
void PlanUtil::linearMakeVsConstant(Plan& p, int wp1, int wp2, double vs) {
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
void PlanUtil::linearMakeVsConstant(Plan& p, int start, int end) {
	if (start < 0) return;
	if (end >= p.size()) end = p.size()-1;
	if (start >= end) return;
	double zStart = p.point(start).z();
	double zEnd = p.point(end).z();
	double dtot = zEnd - zStart;
	double ttot = p.time(end) - p.time(start);
	double vs = dtot/ttot;
	linearMakeVsConstant(p,start,end,vs);
}


// change vertical profile: adjust altitudes
void PlanUtil::linearMakeVsConstant(Plan& p) {
	linearMakeVsConstant(p,0,p.size()-1);
}

void PlanUtil::linearMakeVsConstant(Plan& p, double vs) {
	linearMakeVsConstant(p,0,p.size()-1,vs);
}



}

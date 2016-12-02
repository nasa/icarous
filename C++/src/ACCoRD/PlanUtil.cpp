
/*
 * PlanUtil.cpp - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "PlanUtil.h"
#include "Plan.h"
#include "PlanWriter.h"
//#include "UnitSymbols.h"
#include "Projection.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "KinematicsPosition.h"
#include "Kinematics.h"
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


bool PlanUtil::gsConsistent(const Plan& p, int ixBGS, double accelEpsilon, double distEpsilon,	 bool silent) {
	if ( ! p.point(ixBGS).isBGS()) return true;
	//fpln("$$$ gsConsistent (0): accelEpsilon = "+Fm8(accelEpsilon));
    //bool rtn = true;
	NavPoint BGS = p.point(ixBGS);
	int ixEGS = p.nextEGS(ixBGS); //fixed
	NavPoint EGS = p.point(ixEGS);
	//Velocity vin  = p.point(i).velocityInit();
	//Velocity vout = p.initialVelocity(p.nextEGS(i));
	//Velocity vEGSout = p.initialVelocity(ixEGS);
	double gsOutBGS = p.gsOut(ixBGS);
	double gsOutEGS = p.gsOut(ixEGS);
	//fpln("$$$ gsConsistent: at i = "+Fm0(i)+" gsIn = "+Units::str("kn",gsIn)+" gsOut = "+Units::str("kn",gsOutEGS));

    bool rtn = true;
	double dt = EGS.time() - BGS.time();
	double aBGS = BGS.gsAccel();
	//double acalc = calcGsAccel(i);
	double acalc = (gsOutEGS - gsOutBGS)/(dt);
	//fpln("$$$ gsConsistent(1): accelEpsilon = "+Fm8(accelEpsilon));
	//fpln("$$$ gsConsistent: at i = "+Fm0(i)+" gsInBGS = "+Units::str("kn",gsInBGS)+" gsOutEGS = "+Units::str("kn",gsOutEGS)+" dt = "+Fm1(dt)+" a_BGS = "+a_BGS+" acalc = "+acalc);
	if (!Util::within_epsilon(aBGS, acalc, accelEpsilon)) {
		if ( !silent ) {
			fpln(" >>> isConsistent GS FAIL! at i = "+Fm0(ixBGS)+" GSC section fails ACCEL testtest at t = "+Fm8(p.getTime(ixBGS))+": aBGS = "+Fm8(aBGS)+" acalc = "+Fm8(acalc));
		}
		rtn = false;
	}
	double ds = gsOutBGS*dt + 0.5*aBGS*dt*dt;
	double distH = 	p.pathDistance(ixBGS,ixEGS);
	double absDiff = std::abs(ds-distH);
	//fpln(" ds = "+Units::str("nm",ds)+ " distH = "+Units::str("nm",distH)+" absDiff = "+Units::str15("m",absDiff));
	if (!Util::within_epsilon(absDiff,distEpsilon)) { // See testGsSimple for worst case
		if (!silent) fpln(" >>> isConsistent GS FAIL! at i = "+Fm0(ixBGS)+" GSC section fails Test! absDiff = "+Units::str("nm",absDiff));
		rtn = false;
	}
	return rtn;



	//return PlanUtil::gsConsistent(i, BGS, EGS, accelEpsilon, distEpsilon, gsOutBGS, gsOutEGS, silent);

}



bool PlanUtil::vsConsistent(int i, const NavPoint& VSCBegin, const NavPoint& VSCEnd, double accelEpsilon, double distEpsilon, const Velocity& vin, const Velocity& vout,
		  bool silent) {
    bool rtn = true;
	double dt = VSCEnd.time() - VSCBegin.time();
	double a = VSCBegin.vsAccel();
	// Tests that that stored acceleration matches acceleration calculated from surrounding velocities
	//double acalc = (initialVelocity(i+1).vs()-vo.vs())/t;
	//double acalc = calcVertAccel(i);
    double acalc = (vout.vs() - vin.vs())/dt;
	//fpln("\n $$$ vsConsistent: vin = "+vin+" vout = "+vout+" dt = "+dt+" a = "+a+" acalc = "+Fm4(acalc));
	if (!Util::within_epsilon(a, acalc, accelEpsilon )) {
		if (!silent) fpln(" >>> isConsistent VS FAIL! i = "+Fm0(i)+" VSC section fails ACCEL test! a = "+Fm4(a)+" acalc = "+Fm4(acalc));
		rtn = false;
	}
	//fpln(" >>> isConsistent i = "+Fm0(i)+" a = "+a+"  vo = "+vo);
	// check that the altitude at VSCEnd is proper
	double ds = vin.vs()*dt + 0.5*a*dt*dt;
	//double distV = vertDistance(i,nextVSCEnd(i));
	double distV = 	VSCEnd.position().signedDistanceV(VSCBegin.position());
	double absDiff = std::abs(ds-distV);
	//fpln(" $$$ vsConsistent i = "+Fm0(i)+" dt = "+dt+" ds = "+Units::str("ft",ds,8)+ " distV = "+Units::str("ft",distV,8)+" absDiff = "+Units::str("ft",absDiff,8));
	if (!Util::within_epsilon(absDiff,distEpsilon)) {
	  if (!silent) fpln(" >>> isConsistent VS FAIL!  at i = "+Fm0(i)+" VSC Section fails Test! absDiff = "+Units::str("m",absDiff,8));
		//fpln(" >>> isConsistent i = "+Fm0(i)+" ds = "+Units::str("ft",ds,8)+ " distV = "+Units::str("ft",distV,8));
		rtn = false;
	}
	return rtn;
}

bool PlanUtil::vsConsistent(const Plan& p, int i,  double accelEpsilon, double distEpsilon, bool silent) {
	if ( ! p.point(i).isBVS()) {
		return true;
	}
	// must be a BVS
	NavPoint VSCBegin = p.point(i);
	NavPoint VSCEnd = p.point(p.nextEVS(i));//fixed
	Velocity vin = p.point(i).velocityInit();
//	if (i == 0) vin = p.point(i).velocityIn();     // not sure if we should allow TCP as current point ??
//	else vin = p.finalVelocity(i-1);
	Velocity vout = p.initialVelocity(p.nextEVS(i));
	return PlanUtil::vsConsistent(i, VSCBegin, VSCEnd, accelEpsilon, distEpsilon, vin, vout, silent);
}



//bool PlanUtil::turnConsistent(int i, const NavPoint& BOT, const NavPoint& EOT, const Velocity& vin, double finalTrack, bool silent) {
//	bool rtn = true;
//	double dt = EOT.time() - BOT.time();
//	double omega = BOT.trkAccel();
//	Position so = BOT.position();
//	//bool turnRight = Util::clockwise(vo.trk(), vEOT.trk());
//	//Position pos = BOT.position().linear(vin,dt);                  // for vertical
//	Position pos = (ProjectedKinematics::turnOmega(so, vin, dt, omega).first).mkAlt(EOT.alt()); // need to treat altitude separately
//	//fpln(" $$$ isConsistent: so = "+so+" vo = "+vo+" p = "+p);
//	if (!EOT.position().almostEquals(pos,0.005,1.2)) {
//		if (!silent) {
//			fpln(" >>> isConsistent: Turn FAIL! at i = "+Fm0(i)+" Turn section fails POSITION test! p = "+pos.toString(4)+ " EOT = "+EOT.toString(8));
//			fpln("      .... distanceH = "+Units::str("nm",EOT.position().distanceH(pos),8));
//		}
//		rtn = false;
//	}
//	//fpln(i+" $$$$ isConsistent: nextEOT(i) = "+nextEOT(i)+" finalTrack = "+Units::str("deg",finalTrack,8)+" vo.trk() = "+Units::str("deg",vo.compassAngle(8),8));
//	double deltaTrack = Util::turnDelta(vin.trk(),finalTrack);
//	//fpln(i+" $$$$ isConsistent: deltaTrack = "+Units::str("deg",deltaTrack,8)+" turnRate = "+Units::str("deg/s",turnRate,8));
//	double turnTime = Kinematics::turnTime(deltaTrack,omega);
//	if (!Util::within_epsilon(dt,turnTime,0.012)) {  // See testVsWithTurnHard3Vert for worst case
//		if (!silent) fpln(" >>> isConsistent: Turn FAIL! at i = "+Fm0(i)+" Turn section fails TIME test!  dt = "+Fm4(dt)+" != turnTime = "+Fm4(turnTime));
//		rtn = false;
//	}
//	//fpln(" $$$ turnConsistent: vin = "+vin+" dt = "+dt+" 	getVelocityIn() = "+BOT.getVelocityIn()+" rtn = "+rtn);
//	return rtn;
//}


//bool PlanUtil::turnConsistent(int i, const NavPoint& BOT, const NavPoint& EOT, double timeEpsilon, double distH_Epsilon, double distV_Epsilon,
//		const Velocity& vin, const Velocity& vout, bool silent, bool useProjection) {
//	bool rtn = true;
//	double finalTrack = vout.trk();
//	double dt = EOT.time() - BOT.time();
//	double omega = BOT.trkAccel();
//	Position so = BOT.position();
//	//fpln(" >>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: vin = "+vin+" dt = "+dt+" omega = "+Units::str4("deg/s",omega));
//	//fpln(" >>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: i = "+i+"   vout = "+vout);
//	//fpln(" >>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: vin.trk() = "+Units::str("deg", vin.compassAngle())+" finalTrack = "+Units::str("deg", vout.compassAngle())+" omega = "+Units::str4("deg/s",omega));
//	Position pos;
//	if (useProjection) {
//		Position vertex = BOT.sourcePosition();
//		//f.pln(" $$$$$ turnConsistent: vertex = "+vertex+" BOT = "+BOT);
//		EuclideanProjection proj = Projection::createProjection(vertex.lla().zeroAlt());
//		pos = (ProjectedKinematics::turnOmega(so, vin, dt, omega, proj).first).mkAlt(EOT.alt()); // need to treat altitude separately
//	} else {
//		pos = (KinematicsPosition::turnOmega(so, vin, dt, omega).first).mkAlt(EOT.alt());
//
//	}
//	if (!EOT.position().almostEquals(pos,distH_Epsilon,distV_Epsilon)) { //       (p,0.005,1.2)) {
//		double distanceH = EOT.position().distanceH(pos);
//		if (!silent) {
//			fpln(" >>> turnConsistent: TURN FAIL! i = "+Fm0(i)+" POSITION test! pos = "+pos.toString(4)+ " EOT = "+EOT.toString(8));
//			fpln("      .... distanceH = "+Units::str("m",distanceH,8));
//			//fpln("      .... distanceV = "+Units::str("m",EOT.position().distanceV(p)),8);
//		}
//		if ( ! silent && distanceH > 10*distH_Epsilon) fp(" turnConsistent: ************************************************** (turn) distanceH = "+Units::str("m",distanceH,8));
//		rtn = false;
//	}
////	double deltaTrack = Util::turnDelta(vin.trk(),finalTrack);
//////		double deltaTrack = ProjectedKinematics.turnDelta(so,vin.trk(),finalTrack);
////    double turnTime = Kinematics::turnTime(deltaTrack,omega);
////    //fpln(" >>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: deltaTrack = "+Units::str("deg",deltaTrack)+" turnTime = "+Fm2(turnTime));
////	if (!Util::within_epsilon(dt,turnTime,timeEpsilon)) { // 0.02)) {  // See testVsWithTurnHard3Vert for worst case  ***KCHANGE*** from 0.012 to 0.02
////		if (!silent) fpln(" >>> turnConsistent: TURN FAIL! i = "+Fm0(i)+" Turn section fails TIME test!  dt = "+Fm2(dt)+" != turnTime = "+Fm2(turnTime));
////		rtn = false;
////	}
//	//fpln(" $$$ turnConsistent: vin = "+vin+" dt = "+dt+" 	getVelocityIn() = "+BOT.getVelocityIn()+" rtn = "+rtn);
//	return rtn;
//}





bool PlanUtil::turnConsistent(const Plan& p, int i, double timeEpsilon, double distH_Epsilon, double distV_Epsilon, bool silent, bool useProjection) {
	//bool rtn = true;
	if ( ! p.point(i).isBOT()) return true;
	NavPoint BOT = p.point(i);
	int ixEOT = p.nextEOT(i);//fixed
	NavPoint EOT = p.point(ixEOT);
	//Velocity vin = p.point(i).velocityInit();
	//Velocity vout = p.initialVelocity(ixEOT);
	double pathDist = p.pathDistance(i,ixEOT);
	//f.pln(" $$$$>>>> turnConsistent  ixBOT = "+ixEOT+" BOT = "+BOT);
	//f.pln(" $$$$>>>> turnConsistent  ixEOT = "+ixEOT+" EOT = "+EOT+" pathDist = "+Units.str("ft",pathDist));
	//DebugSupport.dumpPlan(p,"_BUGBUG");
	bool rtn = true;
	//f.pln(" $$>>>> turnConsistent: i = "+i+" vinit.trk() = "+Units.str("deg", vInit.compassAngle())+" vout.trk() = "+Units.str("deg", vout.compassAngle()));
	double signedRadius = BOT.signedRadius();
	Position center = BOT.turnCenter();
	//f.pln(" $$$ turnConsistent: signedRadius = "+Units.str("ft",signedRadius,4)+" center = "+center);
	//double R = BOT.position().distanceH(center);
	//f.pln(" $$$ turnConsistent: R = "+Units.str("ft",R,4));
	double gsAt_d = 100;
	int dir = Util::sign(signedRadius);
	std::pair<Position,Velocity> tAtd = KinematicsPosition::turnByDist(BOT.position(), center, dir, pathDist, gsAt_d);
	Position EOTcalc = (tAtd.first).mkAlt(EOT.alt());// TODO: should we test altitude?
	//Position EOTcalc = new Position(KinematicsLatLon.turnByDist(BOT.lla(), vin, signedRadius, turnDist).first.mkAlt(EOT.alt()));
	//fpln(" $$$$$ EOTcalc = "+EOTcalc); // +" EOTcalc2 = "+EOTcalc2);
	if (!EOT.position().almostEquals(EOTcalc,distH_Epsilon,distV_Epsilon)) { //       (p,0.005,1.2)) {
		if ( ! silent) {
			fpln(" >>> turnConsistent: TURN FAIL! i = "+Fm0(i)+" calculated pos = "+EOTcalc.toString(8)
					+ "\n                                             plan EOT = "+EOT.position().toString(8));
			double distanceH = EOT.position().distanceH(EOTcalc);
			//double distanceV = EOT.position().distanceV(EOTcalc);
			fpln("            .... distanceH = "+Units::str("m",distanceH,8));
			//f.pln("            .... distanceV = "+Units.str("m",distanceV,8));
			//if (distanceH > distH_Epsilon) f.pln(" turnConsistent:  (turn) distanceH = "+Units.str("m",distanceH,8));
		}
		rtn = false;
	}
	return rtn;
}


bool PlanUtil::velocityContinuous(const Plan& p, int i, double velEpsilon, bool silent) {
	bool rtn = true;
	std::string label = p.point(i).label();
////	bool checkTurn = (label.find("$minorTrkChange:") != std::string::npos);
////	//bool checkTurn = ! label.contains("$minorTrkChange:");
////	if (checkTurn) {
////		double turnDelta = Util::turnDelta(p.finalVelocity(i-1).trk(),p.initialVelocity(i).trk());
////		if (turnDelta > velEpsilon) {
////			fpln(" $$ FAIL: turnDelta = "+Units::str("deg",turnDelta));
////			rtn = false;
////		}
////	}
//	double gsDelta = p.finalVelocity(i-1).gs() - p.initialVelocity(i).gs();
//	if (std::abs(gsDelta) > velEpsilon) {
//		fpln(" $$ FAIL gsDelta = "+Units::str("kn",gsDelta));
//		rtn = false;
//	}
//	double vsDelta = p.finalVelocity(i-1).vs() - p.initialVelocity(i).vs();
//	if (std::abs(vsDelta) > velEpsilon) {
//		fpln(" $$ FAIL vsDelta = "+Units::str("fpm",vsDelta));
//		rtn = false;
//	}
//	if (! rtn) { // 2.6)) { // see testAces3, testRandom for worst cases
//		if (!silent) {
//			fpln("\n ----------------------------------------------");
//			fpln(" $$$ isConsistent: FAIL! continuity: finalVelocity("+Fm0(i-1)+") = "+p.finalVelocity(i-1).toStringNP(4)
//						+" != initialVelocity("+Fm0(i)+") = "+p.initialVelocity(i).toStringNP(4));
//			Velocity DeltaV = p.finalVelocity(i-1).Sub(p.initialVelocity(i));
//			if (DeltaV.norm() > 10*velEpsilon) {fp(" turnConsistent: ********************************");
//			fpln("           ....  DeltaV = "+DeltaV.toStringNP(4)+" DeltaV.norm() = "+Fm2(DeltaV.norm()));
//			}
//		}
//	}
	double gsIn =  p.gsIn(i); //  p.finalVelocity(i-1).gs();
	double gsOut = p.gsOut(i); // ) p.initialVelocity(i).gs();
	double gsDelta = gsIn - gsOut;
	//f.pln(" $$ isVelocityContinuous: at i = "+i+" p.finalVelocity(i-1).gs() = "+Units::str("kn",p.finalVelocity(i-1).gs())+" p.initialVelocity(i).gs() = "+Units::str("kn",p.initialVelocity(i).gs()));
	//f.pln(" $$ isVelocityContinuous: at i = "+i+" finalGs = "+Units::str("kn",finalGs)+" initialGs = "+Units::str("kn",initialGs));
	if (std::abs(gsDelta) > velEpsilon) {
		if (!silent) fpln(" $$ isVelocityContinuous: FAIL gsDelta = "+Units::str("kn",gsDelta));
		rtn = false;
	}
	double vsIn  = p.vsIn(i); //  p.finalVelocity(i-1).gs();
	double vsOut = p.vsOut(i); // ) p.initialVelocity(i).gs();
	double vsDelta = vsIn - vsOut;
	//f.pln(" $$ isVelocityContinuous: at i = "+i+" p.finalVelocity(i-1).vs() = "+Units::str("fpm",p.finalVelocity(i-1).vs())+" p.initialVelocity(i).vs() = "+Units::str("fpm",p.initialVelocity(i).vs()));
	//f.pln(" $$ isVelocityContinuous: at i = "+i+" finalVs = "+Units::str("fpm",finalVs)+" initialVs = "+Units::str("fpm",initialVs));
	if ( std::abs(vsDelta) > velEpsilon) {
		if (!silent) fpln(" $$ isVelocityContinuous: FAIL vsDelta = "+Units::str("fpm",vsDelta));
		rtn = false;
	}
	double finalTrk = p.finalVelocity(i-1).compassAngle();
	double initialTrk = p.initialVelocity(i).compassAngle();
	double trkDelta = Util::turnDelta(finalTrk,initialTrk);
	if ( std::abs(trkDelta) > velEpsilon) {
		if (!silent) fpln(" $$ isVelocityContinuous: FAIL trkDelta = "+Units::str("deg",trkDelta));
		rtn = false;
	}



	return rtn;

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
		p.set(j, np2);
	}
	return p;
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
		double t = ac.getTime(i) + dist/gs;
		if (t < ac.getTime(i+1) && t > ac.getTime(i)) {
			//    	  error.addError("addLocalMaxLat time out of bounds");
			insertVirtual(ac,t);
			//cout << "NEWTIME " << ac << " " << i << " " << t << endl;
			ac.set(i+1, ac.point(i+1).makeLabel("maxlat")
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
		NavPoint src = ac.point(ac.getSegment(time));
//		NavPoint np = src.makePosition(ac.position(time)).makeTime(time); // .makeMutability(false,false,false);
		NavPoint np = src.makeStandardRetainSource().makePosition(ac.position(time)).makeTime(time);
		np = np.makeVirtual();
		return ac.add(np);
	}
	else return -1;
}

double PlanUtil::getLegDist(const Plan& ac, int i, double accuracy, double mindist) {
	double lat = std::max(std::abs(ac.point(i).lat()),
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
		while (i < ac.size()-1 && ac.getTime(i) <= endTm) {
			double dist = ac.pathDistance(i);
			double legDist = getLegDist(ac,i,accuracy,mindist);
			//Special case for kinematic plans in acceleration zones -- ensure there are at least 2 mid-points
			// TODO: Possibly get rid of this
			//				if (!ac.isLinear()) {
			//					Plan kpc = ac;
			//					if (kpc.point(i).isTurnBegin() || kpc.point(i).isTurnMid()) {
			//						legDist = std::min(legDist, dist/3.0);
			//					}
			//				}
			if (dist > legDist) {
				double gs = ac.averageVelocity(i).gs();
				int j = i;
				double tmIncr = legDist/gs;
				if (std::abs(ac.point(i).lat()) >= std::abs(ac.point(i+1).lat())) {
					double nextT = ac.getTime(i+1);
					double t = ac.getTime(i);
					while (t + tmIncr + Constants::TIME_LIMIT_EPSILON < nextT) {
						//	    fpln("$$## add point for ac "+ac+" at time "+t);
						t += tmIncr;
						insertVirtual(ac,t);
						legDist = getLegDist(ac,j,accuracy,mindist);
						j++;
						tmIncr = legDist/gs;
					}
				} else {
					double thisT = ac.getTime(i);
					double t = ac.getTime(i+1);
					while (t - tmIncr - Constants::TIME_LIMIT_EPSILON > thisT) {
						//	    fpln("$$## add point for ac "+ac+" at time "+t);
						t -= tmIncr;
						insertVirtual(ac,t);
						legDist = getLegDist(ac,i,accuracy,mindist);
						j++;
						tmIncr = legDist/gs;
					}
				}
				i = std::max(i,j-1); // possibly take back last increment
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
	while (i < ac.size() && ac.getTime(i) < endTm) {
		if (ac.point(i).isVirtual() || all) {
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
		// case: point it virtual or temp: delete point
		// case: point is not original or first or last: keep point.
		// case: point is part of a turn: keep only the vertex point, delete others, if all three are present, otherwise keep remaining
		// case: point is part of GSC: keep only first point, delete second if both present, otherwise keep remaining
		// case: point is part of VSC: keep only vertext point, delete others if both present, otherwise keep remaining
		// don't deal with deleted points quite yet.  this gets really messy.
		if ((!p.isVirtual() ) ) {
			if (p.hasSource()){
				lpc.add(NavPoint(p.sourcePosition(),p.sourceTime()));
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
	if (pln.point(ix).isBGS()) {
		bool revertPreviousTurn = true;
		structRevertGsTCP(pln,ix,revertPreviousTurn);
	} else if (pln.point(ix).isBVS()) {
		pln.structRevertVsTCP(ix);
	} if (pln.point(ix).isBOT()) {
		// store distance from BOT to all points between BOT and EOT
		bool addBackMidPoints = true;
		bool killNextGsTCPs = false;
		double zVertex = -1;
		pln.structRevertTurnTCP(ix,addBackMidPoints,killNextGsTCPs,zVertex);

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
		pln.structRevertTurnTCP(i,addBackMidPoints,killNextGsTCPs, -1.0);
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
	if (! origPt.isTCP()) return ix;  // nothing to do
	//fpln("$$$ structRevertGroupOfTCPs: point("+ix+") = "+pln.point(ix).toStringFull());
    int firstInGroup = ix;                  // index of first TCP in the group
    int lastInGroup = ix; // pln.size()-1;         // index of the last TCP in the group
    if (firstInGroup == 0) {
    	fpln(" !! structRevertGroupOfTCPs: ERROR cannot remove first point 1");
    	return -1;
    }
    if (origPt.isVsTCP()) {
    	if (origPt.isEVS()) {
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
	    if (origPt.isGsTCP()) {
	    	if (origPt.isEGS()) {
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
	    	if (origPt.isEOT()) {
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
        if (pln.point(ii).isVsTCP()) {
        	zVertex = pln.structRevertVsTCP(ii);
        	if (zVertex >= 0) {
        		lastInGroup--;
        		//if (ii == firstInGroup) firstInGroup++;
        	}
        }
	}
 	// get rid of all ground speed TCPs if this is fundamentally a turn
	if (pln.point(firstInGroup).isTrkTCP()) {
    	bool killNextGsTCPs = true;
    	bool addBackMidPoints = false;
    	pln.structRevertTurnTCP(firstInGroup, addBackMidPoints, killNextGsTCPs,zVertex);
	}
	if (pln.point(firstInGroup).isGsTCP()) {  // does not revert previous turn
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
	if (pln.point(ix).isBGS()) {
    	NavPoint ixP = pln.point(ix);
   		//if (ixP.isEGS()) ix = pln.prevBGS(ix);//fixed
   		int ixPrev = pln.prevTCP(ix);
   		NavPoint npPrev = pln.point(ixPrev);
		int prevLinIndex = npPrev.linearIndex();
   		//double dt = ixP.time() - npPrev.time();
   		//fpln(" $$$$$$$$$$$ structRevertGsTCP: npPrev = "+npPrev.toStringFull()+" dt = "+dt);
   		if (revertPreviousTurn && npPrev.isEOT() && ixP.linearIndex() == prevLinIndex) {
   			bool killNextGsTCPs = true;
   			int ixPrevBOT = pln.prevBOT(ixPrev);//fixed
   			//fpln(" $$$$$$$$$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
   			bool addBackMidPoints = false;
   			pln.structRevertTurnTCP(ixPrevBOT, addBackMidPoints, killNextGsTCPs,-1.0);
   			//fpln(" $$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
   			return ixPrevBOT;
   		} else {
   			pln.structRevertGsTCP(ix);
   		}
	}
	return ix;
}






Plan PlanUtil::cutDown(const Plan& plan, double startTime, double endTime) {
	Plan rtn(plan.getName(),plan.getNote());
	Position startPos = plan.position(startTime);
	NavPoint start(startPos, startTime);
	rtn.add(start);
	for (int i = 0; i < plan.size(); i++) {
		NavPoint pi = plan.point(i);
		if (startTime < pi.time() && pi.time() < endTime ) {
			rtn.add(pi);
		}
	}
	Position endPos = plan.position(endTime);
	NavPoint end(endPos, endTime);
    rtn.add(end);
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
	firstPoint = plan.point(ixInit);
	if (firstPoint.isEndTCP()) firstPoint = firstPoint.makeNewPoint();
	nPlan.add(firstPoint);
	//fpln(" $$$$0 cutDownTo: ADD INITIAL point("+ixInit+") = "+firstPoint.toStringFull());
	int ix = ixInit + 1;
	while (plan.point(ix).time() < intentThreshold && ix < plan.size()) {
		NavPoint p = plan.point(ix);
		nPlan.add(p);
		//fpln(" $$$$$$ cutDownTo ADD p = "+p);
		ix++;
	}
	if (ix >= plan.size()) { // plan ran out before intentThreshold
		double dt = intentThreshold - plan.getLastTime();
		Velocity vout = plan.initialVelocity(plan.size()-1);
		NavPoint lastP = plan.point(plan.size()-1);
		NavPoint np = lastP.linear(vout, dt).makeLabel("CutDownTo_0");
		nPlan.add(np);
		//fpln(" $$$$$$ cutDownTo ADD np = "+np);
	} else {                // intentThreshold is within plan
		NavPoint newLastPt = NavPoint(plan.position(intentThreshold),intentThreshold).makeLabel("CutDownTo_newLastPt");
		Velocity vout = plan.velocity(intentThreshold);
		if (! plan.inAccel(intentThreshold)) {
			//fpln(" $$$$$$ cutDownTo ADD newLastPtp = "+newLastPt+" vout = "+vout);
			nPlan.add(newLastPt);
		} else {
			bool inTurn = plan.inTrkChange(intentThreshold);
			bool inGsAccel = plan.inGsChange(intentThreshold);
			bool inVsAccel = plan.inVsChange(intentThreshold);
			NavPoint::Trk_TCPType trkType = NavPoint::NONE;
			NavPoint::Gs_TCPType gsType = NavPoint::NONEg;
			NavPoint::Vs_TCPType vsType = NavPoint::NONEv;
            if (inTurn) trkType = NavPoint::EOT;
            if (inGsAccel) gsType = NavPoint::EGS;
            if (inVsAccel) vsType = NavPoint::EVS;
            NavPoint lastP = NavPoint::makeFull(newLastPt.position(), newLastPt.time(), NavPoint::Orig,  "CutDownTo_lastP", trkType,  gsType, vsType,
						0.0, 0.0, 0.0, vout, newLastPt.position(), newLastPt.time());
            nPlan.add(lastP);
            //fpln(" $$$$$$ cutDownTo ADD lastP = "+lastP+" vout = "+vout);
			}
	}
	NavPoint lastPt = nPlan.getLastPoint();
	if (tExtend > intentThreshold && tExtend > lastPt.time()) {
		//Velocity vout = nPlan.finalVelocity(nPlan.size()-2);  // final velocity of next to last point
		Velocity vout = nPlan.initialVelocity(nPlan.size()-1);  // final velocity of next to last point
		double dt = tExtend - lastPt.time();
		//if (timeOfCurrentPosition == 3900)
		//    	fpln(" $$$$.............. cutDownTo: dt = "+dt+" vout = "+vout);
		NavPoint extendPt = lastPt.linear(vout, dt).makeLabel("CutDownTo_extendPt");
		nPlan.add (extendPt);
	}
	//fpln(" $$$$ cutDownTo: nPlan = "+nPlan);
	return nPlan;
}

Plan PlanUtil::cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold) {
   return cutDownTo(plan, timeOfCurrentPosition, intentThreshold, intentThreshold);
}



//	/**
//	 * Cut a Plan down to contain "numTCPs" future TCPS past the current time (i.e. timeOfCurrentPosition). If tExtend
//	 * is greater than 0, create an extra leg after the last TCP with a duration of "tExtend".  This is intended
//	 * to mimic having only state information after the last TCP.  Note that numTCPs should be interpreted as number of
//	 * acceleration zones.  That is  [BOT,EOT] counts as one TCP.  Similarly [BGS, EGS] is one TCP.
//	 *
//	 * This method eliminates waypoints earlier than timeOfCurrentPosition as much as possible.  If the aircraft
//	 * is in an acceleration zone at timeOfCurrentPosition, then it retains the plan back to the last begin TCP.
//	 *
//	 * @param numTCPs  maximum number of TCPs to allow in the future, see note above.
//	 * @param timeOfCurrentPosition  indicates current location of aircraft, if negative, then aircraft is at point 0.
//	 * @param tExtend  amount of additional time to extend the plan after last TCP end point.
//	 *
//	 * NOTE: THIS CODE WILL NOT WORK WITH OVERLAPPING HORIZONTAL/VERTICAL Accel Zones
//	 *
//	 * @return
//	 */
Plan PlanUtil::cutDownToByCount(const Plan& plan, int numTCPs, double timeOfCurrentPosition, double tExtend) {
    int ix = plan.getSegment(timeOfCurrentPosition);
    int cnt = 0;
    if (plan.inAccel(timeOfCurrentPosition)) cnt = 1;
  	while (cnt < 2*numTCPs && ix < plan.size()) {
		//fpln(" $$$$ cutDownTo point ix = "+ix+" point(ix) = "+point(ix).toStringFull());
  		NavPoint p = plan.point(ix);
		if (p.isTCP()) cnt++;
	    ix++;
  	}
  	double lastTm = plan.point(ix).time() + 5.0;
  	return cutDownTo(plan,timeOfCurrentPosition,lastTm, lastTm+tExtend);
}



/** Determines if there is enough distance for the current speed and specified acceleration (maxAccel)
 *
 * @param p         plan
 * @param ix        index of ground speed change
 * @param maxAccel
 *
 * Note: calculates delta time at ix+1 that is achievable if not enough distance
 * @return  Return the needed correction at ix+1:  achieveable delta time - current delta time (see fixGsAccelAt);
 */
std::pair<bool,double> PlanUtil::enoughDistanceForAccel(const Plan& p, int ix, double maxAccel,	double M) {
	if (ix == 0 || ix == p.size()-1) return std::pair<bool,double>(true,0.0);
	double gsIn = p.finalVelocity(ix-1).gs();
	double gsOut = p.initialVelocity(ix).gs();
	//f.pln(" $$ enoughDistanceForAccel: ix = "+ix+" gsIn = "+Units::str("kn",gsIn)+" gsOut = "+Units::str("kn",gsOut));
	double deltaGs = gsOut - gsIn;
	double a = Util::sign(deltaGs)*maxAccel;
	//f.pln(" $$ enoughDistanceForAccel: deltaGs = "+Units::str("kn",deltaGs)+" actual accel = "+deltaGs/dt);
	double dtNeeded = deltaGs/a + 2*M; // plus 2*M because BGS starts M second into segment and EGS ends M sec before
	//f.pln(" $$ enoughDistanceForAccel: dtNeeded = "+dtNeeded);
	double distanceNeeded = gsIn*dtNeeded + 0.5*a*dtNeeded*dtNeeded;
	double distanceBetween = p.pathDistance(ix,ix+1);
	//f.pln(" $$ enoughDistanceForAccel: distanceBetween = "+distanceBetween+" distanceNeeded = "+distanceNeeded);
	bool rtn = (distanceNeeded <= distanceBetween);
	if (rtn) { // no repair needed
		return std::pair<bool,double>(true,0.0);
	} else {
		//double b = gsIn;
        //double c = -distanceBetween;
		double b = 2*(gsIn + a*M);
        double c = 4*gsIn*M - 2.0*distanceBetween;
        double dtp = (-b+sqrt(b*b - 4*a*c))/(2.0*a);
 		double dtCurrent = p.getTime(ix+1) - p.getTime(ix) - 2*M;
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
	double dtNow = p.getTime(ix+1) - p.getTime(ix);
	if (2*M >= dtNow) {
		double gsIn = p.finalVelocity(ix-1).gs();
		double calcTimeGSin = p.calcTimeGSin(ix+1,gsIn);
		double correction = calcTimeGSin - p.getTime(ix+1);
		p.timeshiftPlan(ix+1,correction);
	} else {
		std::pair<bool,double> pEnoughDist = enoughDistanceForAccel(p, ix, maxAccel, M);
		NavPoint np_ix = p.point(ix+1);   // we will be altering point ix+1
		if (checkTCP && (np_ix.isEOT() || np_ix.isEVS())) return;
		if ( ! pEnoughDist.first) {
			double correction = pEnoughDist.second;
			//f.pln(" $$$$>>>>>>>>>>>>>>>>>>>>>.. fixGsAccelAt: ix = "+ix+" dtCurrent = "+dtCurrent+" dtNew = "+dtNew+" correction = "+correction);
			p.timeshiftPlan(ix+1,correction);	  // 2.2 about right
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
	p.add(pp.point(0));
	for (int j = 1; j < pp.size()-1; j++) {
		Velocity vel = pp.initialVelocity(j);
		bool same = aboutTheSameTrk(lastVel,vel,sameTrackBound);
        //fpln(j+" $$ removeCollinear: lastVel = "+lastVel+" vel = "+vel+" same = "+same);
		if (!same) {
			p.add(pp.point(j));
		}
		lastVel = vel;
	}
	p.add(pp.getLastPoint());
	//fpln(" $$ removeCollinearTrk: pp.size() = "+pp.size()+" p.size() = "+p.size());
	return p;
}





/**
 *
 * Returns a new Plan that sets all points in a range to have a constant GS.
 * THIS ASSUMES NO VERTICAL TCPS.
 *
 * NOTE.  First remove Vertical TCPS then re-generate Vertical TCPs
 * */
Plan PlanUtil::makeGSConstant_NO_Verts(const Plan& p, double newGs) {
	//fpln(" $$ makeGSConstant: newGs = "+Units::str4("kn",newGs)+" "+Fm0(p.getType()));
	Plan kpc(p.getName());
	//kpc.setPlanType(p.getType());
	//double time0 = point(0).time();
	double lastTime = p.point(0).time();
	kpc.add(p.point(0));
	int j = 0;   // last point included in new file (note GS TCPs are removed)
	for (int i = 1; i < p.size(); i++) {
		NavPoint np = p.point(i);
		//fpln(" $$$$ makeGSConstant: i = "+Fm0(i)+" np = "+np.toStringFull());
		// skip any GSC points (i.e. remove them)
		if (!np.isGsTCP()) {
			double d = p.pathDistance(j,i);
			double dt = d/newGs;
			double nt = lastTime + dt;
			//fpln(" $$$$ makeGSConstant: d = "+Units::str("nm",d)+" dt = "+Fm4(dt)+" nt = "+Fm4(nt));
			lastTime = nt;
			if (np.isTCP()) {        // assume only turn TCPs allowed
				if (!np.isTrkTCP()) {
					//fpln("makeGSConstant: start point within acceleration zone");
					p.addError("TrajGen.genGSConstant: start point within acceleration zone");
				}
				//fpln(" $$$ makeGSConstant(TCP) for i = "+Fm0(i)+" BEFORE np.turnRadius = "+Fm4(np.turnRadius()));
				double oldGs = np.velocityInit().gs();
				double newVsIn = p.vertDistance(j,i)/dt;
				np = np.makeVelocityInit(np.velocityInit().mkGs(newGs).mkVs(newVsIn));
				//fpln(" makeGSConstant(TCP): make point i = "+Fm0(i)+" have velocityIn = "+Units::str4("kn",newGs));
				// calculate new acceleration: newAccel = oldAccel * oldTime/newTime
				double k = newGs/oldGs;
				double newAccel; // = np.accel() * k;
				if (np.isBOT()) {                                 // ***RWB*** KCHANGE
					newAccel = np.trkAccel() * k;
					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE Trkaccel from "+np.trkAccel()+" to "+newAccel);
			    	np = np.makeTrkAccel(newAccel); // modify turn omega to match new gs in
				} else if (np.isBGS()) {
					newAccel = np.gsAccel() * k;
					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE gsAccel from "+np.gsAccel()+" to "+newAccel);
			    	np = np.makeGsAccel(newAccel); // modify gsAccel
				} else if (np.isBVS()) {
					newAccel = np.vsAccel() * k;
					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE vsAccel from "+np.vsAccel()+" to "+newAccel);
			    	np = np.makeVsAccel(newAccel); // modify turn omega to match new gs in
				}
				//fpln(" $$$ makeGSConstant(TCP) for i = "+Fm0(i)+" CHANGE accel from "+Fm4(np.accel())+" to "+Fm4(newAccel));
				//np = np.makeAccel(newAccel);
				//fpln(" $$$ makeGSConstant(TCP) for i = "+Fm0(i)+" AFTER np.turnRadius = "+Fm4(np.turnRadius()));
				// NOTE: we need to recalculate times of MOT and BOT, we are preserving turn radius
			}
			NavPoint newNp = np.makeTime(nt);
			//fpln(" $$$ makeGSConstant for i = "+Fm0(i)+" CHANGE time from "+Fm4(p.point(i).time())+" to "+Fm4(newNp.time()));
			//fpln(" $$$ makeGSConstant add "+newNp.toString());
			kpc.add(newNp);
			j = i;
		}
	}
	return kpc;
}

Plan PlanUtil::makeGSConstant_No_Verts(const Plan& p) {
	double dt = p.getLastTime() - p.getFirstTime();
	double newGS = p.pathDistance()/dt;
	return makeGSConstant_NO_Verts(p, newGS);
}


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
		rtnPln.add(p.point(i));
	}
	double lastTime = p.getTime(wp1);
	for (int i = wp1; i < wp2; i++) {
		double dt = p.pathDistance(i,i+1)/gs;
		double newTime = lastTime+dt;
		NavPoint np = p.point(i+1).makeTime(newTime);
		//fpln(" $$$$$>>> makeGSConstant ADD  "+np);
		rtnPln.add(np);
		lastTime = newTime;
	}
	for (int i = wp2+1; i < p.size(); i++) {
		//fpln(" $$$$$... makeGSConstant ADD  "+p.point(i));
		double originalGs = p.initialVelocity(i-1).gs();
		double d = p.point(i-1).distanceH(p.point(i));
		//f.pln(" $$$$ calcTimeGSin: d = "+Units::str("nm",d));
		double newTime = rtnPln.point(i-1).time() + d/originalGs;
		//f.pln(i+" $$$ originalGs = "+Units::str("kn", originalGs)+" newTime = "+newTime);
		NavPoint np = p.point(i).makeTime(newTime);
		//f.pln(" $$$$$AFTER linearMakeGSConstant ADD Back  i = "+i+": "+np);
		rtnPln.add(np);
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
	double ttot = p.getTime(p.size()-1) - p.getTime(0);
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
	double ttot = p.getTime(wp2) - p.getTime(wp1);
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
	p.timeshiftPlan(ix,tmIx - p.point(ix).time());
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
		//fpln(" $$$$$>> makeVsConstant ADD  "+np);
		p.remove(i+1);
		p.add(np);
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
	double ttot = p.getTime(end) - p.getTime(start);
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

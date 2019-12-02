/* Trajectory Generation
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Plan.h"
#include "PlanUtil.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "KinematicsPosition.h"
#include "TurnGeneration.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "Velocity.h"
#include "format.h"
#include "Util.h"
#include "Constants.h"
#include "string_util.h"
#include "TrajGen.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;

bool TrajGen::vertexNameInBOT = false;
bool TrajGen::verbose = false;


//double TrajGen::gsOffsetTime = 0.0;
const double TrajGen::MIN_MARK_LEG_TIME = Units::from("s",50);
const double TrajGen::minorVfactor = 0.01; // used to differentiate "minor" vel change vs no vel change
const double TrajGen::maxVs = Units::from("fpm",10000);
const double TrajGen::maxAlt = Units::from("ft",60000);

const std::string TrajGen::turnFail = "<TrajGen.turnGenError>";
const std::string TrajGen::gsFail = "<TrajGen.gsGenError>";
const std::string TrajGen::vsFail = "<TrajGen.vsGenError>";


void TrajGen::printError(const std::string& s) {
	if (verbose) fpln(s);
}

Plan TrajGen::repairPlan(const Plan& fp, bool repairTurn, bool repairVs,
		double bankAngle, double vsAccel) {
	Plan lpc = fp;
	if (repairTurn) {
		bool addMiddle = true;
		lpc = TrajGen::linearRepairShortTurnLegs(lpc, bankAngle, addMiddle);
	}
	if (repairVs) {
		// lpc = PlanUtil::repairShortVsLegs(lpc,vsAccel);
		bool inhibitFixGs0 = true;
		bool inhibitFixVs0 = true;
		double aggressiveFactor = 1.0;
		PlanUtil::mkVsShortLegsContinuous(lpc, vsAccel, inhibitFixGs0, aggressiveFactor, inhibitFixVs0);
		//if (lpc.hasError()) {
			//fpln(" $$$2 generateTCPs: repair failed! "+lpc.getMessageNoClear());
		//}
	}
return lpc;
}

 Triple<double,double,double> TrajGen::getStoredParameters(const Plan& p) {
	ParameterData pd = p.extractParameters().extractPrefix("TRAJGEN_");
	double bank = -1.0;
	double gs = -1.0;
	double vs = -1.0;
	if (pd.contains("bankAngle")) bank = pd.getValue("bankAngle");
	if (pd.contains("gsAccel")) bank = pd.getValue("gsAccel");
	if (pd.contains("vsAccel")) bank = pd.getValue("vsAccel");
	return Triple<double,double,double>(bank, gs, vs);
}

/**
 * Store accel values in a plan's note field
 * @param p plan to modify
 * @param bankAngle bank angle in radians, negative for none
 * @param gsAccel gs accel in m/s, negative for none
 * @param vsAccel vs accel in m/s, negative for none
 */
void TrajGen::addParams(Plan& p, double bankAngle, double gsAccel, double vsAccel) {
	ParameterData base = p.extractParameters();
	ParameterData pd = ParameterData();
	if (bankAngle >= 0)	pd.setInternal("bankAngle",bankAngle,"rad");
	if (gsAccel >= 0) pd.setInternal("gsAccel",gsAccel,"m/s");
	if (vsAccel >= 0) pd.setInternal("vsAccel",vsAccel,"m/s");
	base.copy(pd.copyWithPrefix("TRAJGEN_"), true);
	p.includeParameters(pd);
}


/** TODO
 *	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points.
 *  Also all TCPs should reference points in a feasible plan.
 *  If the trajectory is modified, it will have added, modified, or deleted points.
 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
 *	@param fp input plan (is linearized if not already so)
 *  @param bankAngle maximum allowed (and default) bank angle for turns
 *  @param gsAccel    maximum allowed (and default) ground speed acceleration (m/s^2)
 *  @param vsAccel    maximum allowed (and default) vertical speed acceleration (m/s^2)
 *  @param minVsChangeRecognized minimum vs change that will register as "non-constant" (m/s)
 *  @param repairTurn attempt to repair infeasible turns as a preprocessing step
 *  @param repairGs attempt to repair infeasible gs accelerations as a preprocessing step
 *  @param repairVs attempt to repair infeasible vs accelerations as a preprocessing step
 *  @param constantGS if true, produces a constant ground speed kinematic plan with gs being average of linear plan
 *
 *
 *  */
Plan TrajGen::makeKinematicPlan(const Plan& fp, double bankAngleIn, double gsAccelIn, double vsAccelIn,
		bool repairTurn, bool repairGs, bool repairVs) {
	//fpln(" generateTCPs: ENTER ----------------------------------- fp = "+fp.toString());
	double bankAngle = bankAngleIn;
	double gsAccel = gsAccelIn;
	double vsAccel = vsAccelIn;
	Plan lpc = fp;
	lpc.mergeClosePoints(Plan::minDt);  // TODO: should we add THIS HERE?
	Plan & ret = lpc;
	if (lpc.size() < 2) {
		ret = lpc;
	} else {
		lpc = repairPlan(lpc, repairTurn, repairVs, bankAngle, vsAccel);
		//fpln(" generateTCPs: after repair: ----------------------------------- lpc = "+lpc.toString());
		Plan kpc = markVsChanges(lpc,vsAccel);
		Plan kpc2 = generateTurnTCPs(kpc, bankAngle);
		//DebugSupport.dumpPlan(kpc2, "generateTCPs_turns");
		if (kpc2.hasError()) {
			ret = kpc2;
		} else {
			//DebugSupport.dumpPlan(kpc2, "generateTCPs_fixgs");
			//fpln(" generateTCPs: generateGsTCPs ----------------------------------- "+kpc2.isWellFormed());
			//bool useOffset = true;
			Plan kpc3 = generateGsTCPs(kpc2, gsAccel, repairGs);
			//fpln(" $$>> makeKinematicPlan: kpc3 = "+kpc3.toStringGs());
			//DebugSupport.dumpPlan(kpc3, "generateTCPs_gsTCPs");
			if (kpc3.hasError()) {
				ret = kpc3;
			} else {
				//fpln(" generateTCPs: makeMarkedVsConstant ----------------------------------- "+kpc3.isWellFormed());
				// *******************
				Plan kpc4 = makeMarkedVsConstant(kpc3);
				//DebugSupport.dumpPlan(kpc4, "generateTCPs_vsconstant");
				if (kpc4.hasError()) {
					ret = kpc4;
				} else {
					//fpln(" generateVsTCPs: generateVsTCPs ----------------------------------- "+kpc4.isWellFormed());
					bool continueGen = false;
					Plan kpc5 = generateVsTCPs(kpc4, vsAccel, continueGen);
					//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
					kpc5.cleanPlan();
					//fpln(" generateVsTCPs: DONE ----------------------------------- "+kpc5.isWellFormed());
					ret = kpc5;
				}
			}
		}
	}
	//fpln(" $$$$ makeKinematicPlan: "+fp.getNote());
	ret.setNote(fp.getNote());
	addParams(ret, bankAngle, gsAccel, vsAccel);
	return ret;
}

Plan TrajGen::makeKinematicPlan(const Plan& fp, double bankAngleIn, double gsAccelIn, double vsAccelIn) {
		bool repairTurn = false;
		bool repairGs = true;
		bool repairVs = false;
		return TrajGen::makeKinematicPlan(fp, bankAngleIn, gsAccelIn, vsAccelIn, repairTurn, repairGs, repairVs);
}

bool TrajGen::turnCanBeInscribed(const Plan& lpc, int i, const Position& BOT, const Position& EOT) {
	Position np2 = lpc.point(i).position();
	double d1new = BOT.distanceH(np2);
	double d2new = np2.distanceH(EOT);
	int ipTC = i-1;
	int inTC = i+1;
	ipTC = PlanUtil::prevTrackChange(lpc,i);
	inTC = PlanUtil::nextTrackChange(lpc,i);
	double d1old = lpc.point(ipTC).position().distanceH(np2);
	double d2old = np2.distanceH(lpc.point(inTC).position());
	bool rtn = true;
	//fpln(" $$$$ generateTurnTCPs: "+(BOT.time()-getMinTimeStep())+" "+fp.getTime(i-1)+" "+EOT.time()+getMinTimeStep()+" "+fp.getTime(i+1));
	if (d1new >= d1old  || d2new >= d2old) {
		if (verbose) {
			fpln("####### i = "+Fm0(i)+" ipTC = "+Fm0(ipTC)+" inTC = "+Fm0(inTC)+" np2 =  "+np2.toString());
			fpln("####### d1new = "+Fm2(d1new)+" >? d1old = "+Fm2(d1old));
			fpln("####### d2new = "+Fm2(d2new)+" >? d2old = "+Fm2(d2old));
		}
		rtn = false;
	}
	return rtn;
}


/**
 * This takes a plancore lpc and returns a kinematic plan core that has all points with vertical changes as "AltPreserve" points, with all other points
 * being "Original" points.  Beginning and end points are always marked.
 * @param lpc source plan
 * @return kinematic plan with marked points
 */
Plan TrajGen::markVsChanges(const Plan& lpc, double vsAccel) {
	//fpln(" markVsChanges: lpc = "+lpc);
	std::string name = lpc.getName();
	Plan kpc(name);
	if (lpc.size() < 2) {
		kpc = Plan(lpc);
		kpc.addError("markVsChanges"+vsFail+": Source plan "+name+" is too small for kinematic conversion");
		return kpc;
	}
	kpc.add(lpc.get(0)); // first point need not be marked
	for (int i = 1; i < lpc.size(); i++) {
		NavPoint np = lpc.point(i);
		TcpData tcp = lpc.getTcpData(i);
		double vs1 = lpc.vsOut(i-1);
		double vs2 = lpc.vsOut(i);
		if (std::abs(vs1) > maxVs) {
			kpc.addWarning("Input File has vertical speed exceeding "+Units::str("fpm",maxVs)+" at "+Fm0(i-1));
		}
		double deltaTm = 0;
		if (i+1 < lpc.size()) deltaTm = lpc.time(i+1) - lpc.time(i);
		double deltaVs = std::abs(vs1-vs2);
		if (std::abs(deltaVs) > 0.1*Plan::MIN_VS_DELTA_GEN || deltaTm >= MIN_MARK_LEG_TIME) {
			kpc.add(np, tcp.setAltPreserve());
			//fpln(" $$>>>>>>>>>>>>>> markVsChanges: i= "+i+" marked ALT = "+Units::str("ft",kpc.point(i).alt())+" deltaTm = "+deltaTm);
		} else {
			kpc.add(np, tcp);
		}
	}
	//		kpc.add(lpc.point(lpc.size()-1)); // last point need not be marked
	return kpc;
}

Plan TrajGen::generateTurnTCPs(const Plan& lpc, double bankAngle) {
	bool continueGen = false;
	return generateTurnTCPs(lpc,bankAngle,continueGen);
}

Plan TrajGen::generateTurnTCPs(const Plan& lpc, double bankAngle, bool continueGen) {
	//fpln("$$$ generateTurnTCPs: bankAngle = "+Units::str("deg",bankAngle));
	Plan traj(lpc); // the current trajectory based on working
	double bank = std::abs(bankAngle);
	for (int i = lpc.size()-2; i > 0; i--) {
		if (lpc.inAccelZone(i)) continue;     // skip over regions already generated (for now only BOT-EOT pairs)
		double gsIn = lpc.gsIn(i);
		if (Util::almost_equals(gsIn,0.0)) {
			//fpln(" ###### WARNING: Segment from point "+Fm0(i-1)+" to point "+Fm0(i)+" has zero ground speed!");
            continue;
		}
    	generateTurnTcpAt(traj, i, bank, continueGen);
		if (traj.hasError() && ! continueGen) return traj;
	}
	//fpln(" $$>>>>>>>>>  generateTurnTCPsRadius: EXIT traj = "+traj.toStringFull());
	return traj;
}




void TrajGen::generateTurnTcpAt(Plan& traj, int ixNp2, double default_bankAngle, bool continueGen) {
	double gsIn = traj.gsIn(ixNp2);
	double turnDelta = Util::turnDelta(traj.trkIn(ixNp2),traj.trkOut(ixNp2));
	//double turnTime;
	TcpData tcp_i = traj.getTcpData(ixNp2);
	double R = traj.vertexRadius(ixNp2);
    bool bankUsed = true;
	if (Util::almost_equals(R, 0.0)) {
		R = Kinematics::turnRadius(gsIn, default_bankAngle);
	} else {
		bankUsed = false;
	}
    double gsOut_i = traj.gsOut(ixNp2);
	if (turnDelta >= Plan::MIN_TRK_DELTA_GEN && gsOut_i > 1E-10) {  // do not generate when gsOut is near 0
		generateTurnTcpAt(traj, ixNp2, R, bankUsed, continueGen);
	}
}


void TrajGen::generateTurnTcpAt(Plan& traj, int ixNp2, double R, bool bankUsed,  bool continueGen) {
	//fpln(" $$ generateTurnTcpsAt: ENTER traj = "+traj.toString(12));
    //fpln(" $$ generateTurnTcpsAt: R = "+Units::str("NM",R)+" ixNp2 = "+Fm0(ixNp2));
	NavPoint np1 = traj.point(ixNp2-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
	NavPoint np2 = traj.point(ixNp2);
	NavPoint np3 = traj.point(ixNp2+1);
	if (np3.time() - np2.time() < 0.1 && ixNp2+2 < traj.size()) {
		np3 = traj.point(ixNp2 + 2);
	}
	Tuple6<Position,Position,Position,int,double,Position> turn = TurnGeneration::turnGenerator(np1.position(), np2.position(), np3.position(), R);
	Position botPos = turn.first;
	//fpln(" $$ generateTurnTcpsAt: botPos = "+botPos.toString());
	Position motPos = turn.second;
	Position eotPos = turn.third;
	int dir = turn.fourth;
	Position center = turn.sixth;
	double distBotToVertex = botPos.distanceH(np2.position());
	double distVertexToEot = eotPos.distanceH(np2.position());
	double ttEOT = traj.timeFromDistance(ixNp2,distVertexToEot);
	// ----------- recalculate radius from center (may be slightly different) -----------------------
	double radius = center.distanceH(botPos);
	double chordalRadius;
	if (KinematicsLatLon::chordalSemantics) {
		chordalRadius = dir*GreatCircle::to_chordal_radius(radius);
	} else {
		chordalRadius = dir*radius;
	}
	//------------ store the chordal radius in the BOT TCP -------------------------------------------
	std::string label = "";
	if ( np2.name() != "") label = " ("+np2.name()+") ";
	if (botPos.isInvalid() || motPos.isInvalid() || eotPos.isInvalid()) {
		if (!continueGen) {
			traj.addError("generateTurnTCPs"+turnFail+": turn points at "+Fm0(ixNp2)+label+" invalid.",ixNp2);
			printError("generateTurnTCPs"+turnFail+": turn points at "+Fm0(ixNp2)+label+" invalid.");
		}
	} else if ( ! turnCanBeInscribed(traj, ixNp2, botPos, eotPos)) {
		if (!continueGen) {
			traj.addError("generateTurnTCPs"+turnFail+": cannot achieve turn at point "+Fm0(ixNp2)+label
					+" from surrounding points at time "+Fm2(traj.point(ixNp2).time()),ixNp2);
			printError("generateTurnTCPs"+turnFail+": cannot achieve turn "+Fm0(ixNp2)+label+" from surrounding points at "+Fm0(ixNp2));
		}
	} else { // if (turnTime > 2.0*getMinAccelTime() ||deltaTrack > Units::from("deg",3.5) ) { // ignore small changes  is the 2.0 factor right?
		if (traj.inTrkChange(ttEOT)) {
			if (!continueGen) {
				traj.addError("generateTurnTCPs"+turnFail+": EOT in new turn overlaps existing turn in time at "+Fm0(ixNp2)+label,ixNp2);
				printError("generateTurnTCPs"+turnFail+": EOT in new turn overlaps existing turn int time at "+Fm0(ixNp2)+label);
			}
		} else {
			//fpln(" $$$$ generateTurnTCPs: BOT.time() = "+f.Fm2(BOT.time())+" np times = "+f.Fm2(np1.time())+", "+f.Fm2(np2.time())+", "+f.Fm2(np3.time()));
			int ixBOT, ixEOT;
			double targetGS =  traj.gsOut(ixNp2);
			// **************** traj is not modified above here (other than errors) ************************
			ixBOT = traj.insertByDistance(ixNp2,-distBotToVertex);
			double botTime = traj.time(ixBOT);
			double gsIn = traj.gsAtTime(botTime);
			if (TrajGen::vertexNameInBOT) {
		    	traj.setNavPoint(ixBOT, traj.point(ixBOT).makeName(np2.name()));
			}
			std::string srcinfo = traj.getInfo(ixNp2);
			ixNp2 = ixNp2 + 1;  // just added point before np2
			ixEOT = traj.insertByDistance(ixNp2,distBotToVertex);
			TcpData tcpEOT = TcpData().setEOT();
			TcpData tcpBOT = TcpData().setBOT(chordalRadius, center);
	 		if (! bankUsed) tcpBOT.appendInformation(Plan::manualRadius);
			if (TrajGen::vertexNameInBOT) {
    			tcpBOT.setInformation(srcinfo);
			}
			traj.setTcpData(ixBOT,tcpBOT);
			traj.setTcpData(ixEOT,tcpEOT);
			//fpln(" $$$$$$ ixEOT = "+Fm0(ixEOT)+" traj = "+traj.toString(12));
			ixEOT = traj.mergeClosePointsByDist(ixEOT,1.1E-3); // EOT may be very very close to existing point
			//fpln(" $$$$$$ ixEOT = "+Fm0(ixEOT)+" traj = "+traj.toString(12));
			// finally, move all points in turn proportionately to new distance divided by old distance
			std::pair<int,double> mPair = movePointsWithinTurn(traj,ixBOT,ixNp2,ixEOT,gsIn);
			ixEOT = mPair.first;
			// ********** cannot survive the following errors: already added BOT, EOT ************
			if (ixBOT < 0 || ixEOT < 0 ) {
				//fpln("$$$$$$ TrajGen.generateTurnTCPs: BOT/EOT overlaps point at ixBOT = "+ixBOT);
				traj.addError("generateTurnTCPs"+turnFail+": BOT/EOT overlaps point ");
				return;
			}
			//fpln(" $$>> generateTurnTCPs: at ixBOT = "+ixBOT+"  ixEOT = "+ixEOT+" gsIn = "+Units::str("kn",gsIn,4));
			traj.mkGsIn(ixBOT, gsIn);
			targetGS = mPair.second;
			traj.mkGsOut(ixEOT, targetGS);
			//fpln(" $$>> generateTurnTCPs: END traj = "+traj.toStringGs());
		}
	}
	//fpln(" $$$ generateTurnTcpAt:: traj = "+traj.toStringGs());


}

std::pair<int,double> TrajGen::movePointsWithinTurn(Plan& traj, int ixBOT, int ixNp2, int ixEOT,  double gsIn) {
	//fpln(" $$$ movePointsWithinTurn: ------- ixBOT, ixNp2, ixEOT = "+Fm0(ixBOT)+", "+Fm0(ixNp2)+", "+Fm0(ixEOT));
	//fpln(" $$$ movePointsWithinTurn: ENTER traj = "+traj.toStringGs());
	NavPoint vertex = traj.point(ixNp2);
	//traj.appendInfo(ixNp2,Plan::MOTflag);
	traj.setMOT(ixNp2);
	double longDistTOEOT = traj.point(ixBOT).distanceH(vertex) + vertex.distanceH(traj.point(ixEOT));
	// **********************************************************
	// create a copy of interior points, then remove from traj
	Plan tempPlan("temp");   // needs to be a plan because it must save GS and VS TCP DATA
	for (int j = ixEOT; j > ixBOT; j--) {
		int ix = tempPlan.add(traj.get(j));
		if (! TcpData::motFlagInInfo && j == ixNp2) {
			tempPlan.setMOT(ix);
		}
		if (j < ixEOT) {               // EOT stays in trajectory
//			if (traj.isTrkTCP(j) && ! traj.isMOT(j)) { // The isMOT test is included in case an MOT is made a TCP
//				traj.addError("generateTurnTCPs"+turnFail+": TURN overlap !!! at j = "+Fm0(j));
//			}
			//fpln(" $$ movePointsWithinTurn:: REMOVE j = "+Fm0(j));
			traj.remove(j);
		}
	}
	//fpln("$$ movePointsWithinTurn: traj = "+traj.toString(12));
	double targetGs = tempPlan.gsIn(tempPlan.size()-1);
	tempPlan.addNavPoint(traj.point(ixBOT));  // add BOT as non-TCP (i.e. want linear distance
	//fpln("$$ movePointsWithinTurn: >>>>>>>> tempPlan = "+tempPlan.toStringGs());
	double pathDist = traj.pathDistance(ixBOT,ixBOT+1);  // all inner points now removed
	double ratio = pathDist/longDistTOEOT;
	double tmBOT = traj.time(ixBOT);
	traj.timeShiftPlan(ixBOT+1,1000);				// temporarily move EOT out of the fray, see test_UF150
	for (int i = tempPlan.size()-2; i > 0; i--) {       // EOT is last in tempPlan, skip it here
		NavPoint orig_i = tempPlan.point(i);
		TcpData tcp_i = tempPlan.getTcpData(i);
		double dist_to_i = tempPlan.pathDistance(0,i);  // linear distance
		double d_i = ratio*dist_to_i;
		//fpln(" $$ movePointsWithinTurn: d_i = "+Units::str("NM",d_i,5));
		bool linear = false;
		std::pair<Position, int> adv = traj.advanceDistance2D(ixBOT, d_i, linear);
		double alt_i = orig_i.alt();
		Position pos_i = adv.first.mkAlt(alt_i);
		//fpln(" $$$  movePointsWithinTurn: i = "+i+" pos_i = "+pos_i);
		double dt = d_i/gsIn;
		double t_i = tmBOT + dt;
		std::string label_i =  orig_i.name();
		if (TrajGen::vertexNameInBOT) {
			double deltaToMOT = std::abs(dist_to_i - longDistTOEOT/2.0);
			if (deltaToMOT < 1.0) {
				label_i = "";             // MOT label is moved to BOT
				tcp_i.setInformation(""); // MOT info is moved to BOT, so remove here
			}
		}
		NavPoint np_i = NavPoint(pos_i, t_i,label_i);
		traj.add(np_i,tcp_i);  // add back interior point
	}
	for (int i = 0; i < tempPlan.size()-1; i++) {
		double gs = tempPlan.gsOut(i);
		traj.mkGsOut(ixBOT+i,gs);
		//fpln(" $$ movePointsWithinTurn: mkGsOut at "+(ixBOT+i)+" = "+Units::str("kn",gs));
	}
	//fpln(" $$ movePointsWithinTurn: traj = "+traj.toStringGs());
	ixEOT = traj.nextEOT(ixBOT);  // some of the points that were after EOT may have been moved in front
	return std::pair<int,double>(ixEOT,targetGs);
}

Plan TrajGen::generateTurnTCPsRadius(const Plan& lpc, bool continueGen, bool delayGsChangeUntilEOT) { //, double default_radius) {
	Plan traj = Plan(lpc); // the current trajectory based on working
	//fpln(" $$>> generateTurnTCPsRadius: lpc = "+lpc.toString());
	for (int i = lpc.size()-2; i > 0; i--) {   // Perform in reverse order so indexes match between lpc and traj
		double gsIn = lpc.gsIn(i);
		if (Util::almost_equals(gsIn,0.0)) {
			//printError("TrajGen.generateTurnTCPs ###### WARNING: Segment from point "+Fm0(i-1)+" to point "+Fm0(i)+" has zero ground speed!");
			continue;
		}
		double turnDelta = Util::turnDelta(lpc.trkIn(i),lpc.trkOut(i));
		TcpData tcp_i = lpc.getTcpData(i);
		double R = lpc.vertexRadius(i);
		//fpln(" $$>> generateTurnTCPsRadius(lpc): i = "+i+" R = "+Units::str("NM",R));
		//double turnTime = turnDelta*R/gsIn;    // if R = 0 ==> do not process this point
		double gsOut_i = lpc.gsOut(i);
		//fpln(" $$$$$ generateTurnTCPs: i = "+i+" gsOut_i = "+gsOut_i);
		//if (turnTime >= getMinAccelTime() && gsOut_i > 1E-10) {  // do not generate when gsOut is near 0
		if (R > 0 && turnDelta >= Plan::MIN_TRK_DELTA_GEN && gsOut_i > 1E-10) {  // do not generate when gsOut is near 0
            bool bankUsed = false;
			generateTurnTcpAt(traj, i, R, bankUsed, continueGen);
			if (!continueGen && traj.hasError()) return traj;
		}
	}//for
	return traj;
}


Plan TrajGen::generateTurnTCPsRadius(const Plan& lpc, bool continueGen) {
	bool delayGsChangeUntilEOT = true;
    return generateTurnTCPsRadius(lpc, continueGen, delayGsChangeUntilEOT);
}


Plan TrajGen::generateTurnTCPsRadius(const Plan& lpc) {
	bool continueGen = false;
	bool delayGsChangeUntilEOT = true;
	return generateTurnTCPsRadius(lpc, continueGen, delayGsChangeUntilEOT);
}


/**
* @param p        the Plan (i.e. trajectory)
* @param ix       vertex index
* @param radius   radius of turn
* @return         minimum leg distance, return 0 if no turn would be generated here
*/
double TrajGen::neededTurnLegDistance(const Plan& p, int ix, double radius) {
	if (ix < 0 || ix > p.size()-1) return -1;
	double trkIn =  p.trkIn(ix); //  GreatCircle.final_course(p1, p2);
	double trkOut = p.trkOut(ix); // GreatCircle.initial_course(p2, p3);
	double deltaTrack = Util::turnDelta(trkIn,trkOut);
	if (deltaTrack < PlanUtil::maxTurnDeltaStraight) return 0;
	double theta = deltaTrack/2.0;
	double distance = radius*tan(theta);
	return distance;
}

double TrajGen::neededLegLengthBankAngle(const Plan& p, int ix, double bankAngle) {
	double gsIn = p.gsIn(ix);
	double radius = Kinematics::turnRadius(gsIn, bankAngle);
	return neededTurnLegDistance(p, ix, radius);
}



Plan TrajGen::generateGsTCPs(const Plan& fp, double maxGsAccel, bool repairGs) {
	Plan traj(fp); // the current trajectory based on working
	for (int i = 1; i < traj.size(); i++) {
		if (traj.isGsTCP(i)) continue;
		double a_gs = maxGsAccel;
		double gsAccel = std::abs(traj.gsAccel(i));  // see if gsAccel is stored at point ix
		//f.pln(" $$ generateGsTCPs: i = "+i+" gsAccel = "+gsAccel);
		if (gsAccel > 0) {
			a_gs = gsAccel;
			traj.appendInfo(i,Plan::manualGsAccel);
		}
		//f.pln(" $$$$$$ generateGsTCPs: i = "+i+" traj.gsIn(i+1) = "+Units.str("kn",traj.gsIn(i+1))+"  traj.gsOut(i+1) = "+Units.str("kn",traj.gsOut(i+1)));
		double targetGs = traj.gsOut(i);
		//f.pln(" $$$ generateGsTCPs: i = "+i+" targetGs = "+Units.str("kn",targetGs));
		double gsIn = traj.gsIn(i);
		if (std::abs(targetGs - gsIn) > Plan::MIN_GS_DELTA_GEN) {
			if (Util::almost_equals(targetGs,0.0,PRECISION5)) {
				//f.pln("$$$$## generateGsTCPs: targetGs is 0 at index "+i);
				int ixBGS = generateGsToZero(traj,i,a_gs,targetGs);
				if (ixBGS >= 0) {
					int ixEGS = traj.nextEGS(ixBGS);
					if (ixEGS >= 0) i = ixEGS;
				}
			} else {
				generateGsTCPsAt(traj, i, a_gs, repairGs);
				int ixEGS = traj.nextEGS(i);
				if (ixEGS >= 0) i = ixEGS;
			}
		}
	}
	//f.pln(" generateGsTCPs: EXIT traj = "+traj.toStringGs());
	return traj;

}

void TrajGen::generateGsTCPsAt(Plan& traj, int ixBGS, double gsAccel, bool repairGs) {
	double gsIn = traj.gsIn(ixBGS);
	double targetGs = traj.gsOut(ixBGS);
	int sign = 1;
	if (gsIn > targetGs) sign = -1;
	double a = std::abs(gsAccel)*sign;
	double neededDistance = 0.0;
	double accelTime = 0.0;
	int j = ixBGS+1;
	int topIndex = traj.size();
	if (! repairGs) topIndex = j+1;
	for (j = ixBGS+1; j < topIndex; j++) {  // skip over segments that do not have enough distance to meet targetGs
		double distAvailable = traj.pathDistance(ixBGS,j);
		targetGs = traj.gsOut(j-1);
		accelTime = (targetGs - gsIn)/a;
		neededDistance = gsIn*accelTime + 0.5*a*accelTime*accelTime;
		//f.pln(" $$$ findTargetGs: IN LOOP i = "+i+" j = "+j+" targetGs = "+Units.str("kn",targetGs)+" neededDistance = "+Units.str("NM",neededDistance)+" distAvailable = "+Units.str("NM",distAvailable));
		if (neededDistance <= distAvailable) break;
	}
	//fpln(" ##$$ generateGsTCPs: at i = "+Fm0(i)+" Accelerate FROM gsIn = "+Units::str("kn",gsIn)+" TO targetGs = "+Units::str("kn",targetGs));
	double distanceToEndOfPlan = traj.pathDistance(ixBGS,traj.size()-1);
	if (neededDistance > distanceToEndOfPlan) {
		traj.addError("generateGsTCPs"+gsFail+": Cannot complete acceleration at i = "+Fm0(ixBGS)+" before end of Plan");
		return;
	}
	if (neededDistance >= Units::from("ft",50)) {
		//std::string label = np1.name();
		double t0 = traj.time(ixBGS);
		std::pair<Position,int> pv = traj.advanceDistance(ixBGS,neededDistance,false);
		NavPoint EGS = NavPoint(pv.first, accelTime+t0); // .makeAdded();//.appendName(setName);
		TcpData tcp1 = traj.getTcpData(ixBGS);
		TcpData EGS_tcp = makeEGS(tcp1);
		TcpData BGS_tcp = makeBGS(tcp1, a, tcp1.isAltPreserve());
		//int ixBGS = traj.add(BGS, BGS_tcp);
		traj.setTcpData(ixBGS,BGS_tcp);
		bool ok = adjustGsInsideAccel(traj, ixBGS, a, neededDistance);
		if (ok) {
			int ixEGS = traj.add(EGS, EGS_tcp);
			//fpln(" $$##### generateGsTCPs:  i = "+i+" ixBGS = "+ixBGS+" ixEGS = "+ixEGS);
			traj.mkGsIn(ixEGS+1,targetGs);  // did not overlap fix gs between EGS and next point
		}
	}
}

/** remove all points that will fall within distance to new EGS (distToEGS)
 *
 * @param traj       the plan file of interest
 * @param ixBGS      index of BGS
 * @param aBGS       acceleration value
 * @param distToEGS  distance from BGS to new EGS point
 */
bool TrajGen::adjustGsInsideAccel(Plan& traj, int ixBGS, double aBGS, double distToEGS) {
	//fpln(" $$$ adjustGsInsideAccel: distToEGS = "+Units::str("nm",distToEGS,8));
	double gsBGS = traj.gsIn(ixBGS);
	int lastIxInGsAccel = ixBGS;
	bool rtn = true;
	for (int ii = ixBGS+1; ii < traj.size(); ii++) {
		double dist_to_ii =  traj.pathDistance(ixBGS,ii);
		//fpln(" $$$$$ adjustGsInsideAccel: ii = "+ii+" dist_to_ii = "+Units::str("nm",dist_to_ii,8));
		if (dist_to_ii < distToEGS) lastIxInGsAccel = ii;
		else break;
		if (traj.isBGS(ii)) {
			//fpln("$$$$$ adjustGsInsideAccel: OVERLAP error:  BGS at ii = "+ii);
			traj.addError("generateGsTCPsAt"+gsFail+": OVERLAP error:  BGS at ii = "+Fm0(ii));
            traj.clearBGS(ixBGS);
            return false;
		}
	}
	//fpln("\n $$$$$>>> adjustGsInsideAccel:  lastIxInGsAccel = "+lastIxInGsAccel);
	double tBGS = traj.time(ixBGS);
	traj.timeShiftPlan(ixBGS+1,1000);
	for (int j = ixBGS+1; j <= lastIxInGsAccel; j++) {
		double d_j = traj.pathDistance(ixBGS,j);
		double t_j = tBGS + Plan::timeFromDistance(gsBGS, aBGS, d_j);
		if (ISNAN(t_j)) {
            traj.addError("adjustGsInsideAccel"+gsFail+": timeFromDistance generated NaN");
			//DebugSupport::dumpPlan(traj,"adjustGsBug");
            rtn = false;
		} else {
		   traj.setTime(j,t_j);
		}
		//fpln(" $$$$$>>> adjustGsInsideAccel: set time of j = "+j+" to "+t_j);
	}
	return rtn;
}



/** generate an BGS-EGS pair with the EGS at point i
 *
 * @param traj        trajectory
 * @param i           point with GSOut = 0
 * @param a           absolute acceleration
 * @param targetGs    should be zero are almost zero
 * @return            index where BGS is located
 */
int TrajGen::generateGsToZero(Plan& traj, int i, double gsAccel, double targetGs) {
	if (gsAccel < 0) gsAccel = -gsAccel;
	double gsIn = traj.gsIn(i);
	//double t_i = traj.time(i);
	//fpln("\n $$$$ generateGSToZero: ENTER i = "+Fm0(i)+" gsIn = "+Units::str("kn",gsIn)+" targetGs = "+Units::str("kn",targetGs));
	double accelTime = (gsIn - targetGs )/gsAccel;
	//fpln(" $$$$$ generateGSToZero: i = "+Fm0(i)+" accelTime = "+Fm1(accelTime));
//
//	if (std::abs(targetGs - gsIn) <= Plan::MIN_GS_DELTA_GEN) {
//		//fpln(" $$$$$ generateGSToZero no GS TCPS needed at i = "+i+" accelTime = "+accelTime);
//		return -1; // no change
//	}
	//fpln("\n $$$$ generateGSToZero: ENTER i = "+i+" traj = "+traj.toStringGs());
	double neededDist = accelTime*(gsIn+targetGs)/2.0;
	//fpln(" $$$$$ generateGSToZero: distBackToBGS = "+distBackToBGS);
	double distBack_bSeg =  0.0;
	int bSeg = -1;
	for (int j = i-1; j >= 0; j--) {
		double dist_j = traj.pathDistance(j);
		distBack_bSeg = distBack_bSeg + dist_j;
		//fpln(" $$$$$ generateGSToZero: j = "+j+" dist_j =  "+dist_j+" distBack = "+distBack);
		if (distBack_bSeg > neededDist) {
			bSeg = j;
			break;
		}
		// recompute needed distance with new gsIn (because previous segment will have a different gs)
		if (j>0) gsIn = traj.gsOut(j-1);
		accelTime = (gsIn - targetGs)/gsAccel;
		neededDist = accelTime*(gsIn+targetGs)/2.0;
		//fpln(" $$$$$ generateGSToZero: REVISED j = "+j+"  gsIn = "+Units::str("kn",gsIn));
	}
	//fpln(" $$$$$ generateGsToZero: bSeg = "+bSeg);
	if (bSeg < 0) {
		traj.addError("generateGsToZero"+gsFail+": Plan too short to decelerate to zero speed!");
		return -1;
	}
	double deltaDist = distBack_bSeg - neededDist;
    double bgsTime = traj.timeFromDistance(bSeg, deltaDist);
	Position bgsPos = traj.advanceDistance(bSeg,deltaDist,false).first;
	//NavPoint BGS = new NavPoint(bgsPos,bgsTime);
	traj.setEGS(i);
	//fpln(" $$$$$ >>>>>>>>>>>>>>>> generateGsToZero: setEGS = i ="+i);
	std::string name = traj.getPointName(i);
	std::pair<NavPoint,TcpData> BGS;
	if (traj.inGsChange(bgsTime)) {
	    BGS = Plan::makeEGSBGS(name, bgsPos, bgsTime, -gsAccel);
	} else {
    	BGS = Plan::makeBGS(name, bgsPos, bgsTime, -gsAccel);
	}	//fpln(" $$$$$ generateGsToZero: gsAtTime("+bgsTime+") =  "+Units::str("kn",traj.gsAtTime(bgsTime)));
	int ixBGS = traj.add(BGS);
    int ixEGS = i+1;   // because we added a new BGS
	int ixDelete = traj.mergeClosePoints(ixBGS-1,Plan::minDt);
	if (ixDelete >= 0) {
		ixBGS--;
		ixEGS--;
	}
    if (ixBGS > 0) {
    	//fpln(" $$$$$ generateGsToZero: ixBGS = "+ixBGS+" gsIn = "+Units::str("kn",gsIn));
    	traj.mkGsIn(ixBGS, gsIn);
     }
	adjustGsInsideAccelToEGS(traj, ixBGS, -gsAccel, ixEGS);
	//fpln("\n $$$$ generateGSToZero: EXIT traj = "+traj.toStringGs());
	return ixBGS;
}

/** Adjust ground speed of all points that will fall within distance to new EGS (distToEGS)
 *
 * @param traj       the plan file of interest
 * @param ixBGS      index of BGS
 * @param aBGS       signed acceleration value
 * @param distToEGS  distance from BGS to new EGS point
 *
 * Note: this function used where EGS has already been added.
 */
void TrajGen::adjustGsInsideAccelToEGS(Plan& traj, int ixBGS, double gsAccel, int ixEGS) {
	//fpln(" $$$$$ adjustGsInsideAccelToEGS: ENTER ixBGS = "+ixBGS+" ixEGS = "+ixEGS+" traj = "+traj.toStringGs());
    double dtAtIxEGS = 0;
	if (ixEGS < traj.size()-1) {
		dtAtIxEGS = traj.time(ixEGS+1) - traj.time(ixEGS);
	}
	if (ixEGS >= traj.size()) {
		return;
		//Debug.halt();
	}
	double gsBGS = traj.gsIn(ixBGS);
	for (int ii = ixBGS+1; ii < ixEGS; ii++) {
		if (traj.isBGS(ii)) traj.clearBGS(ii);
		if (traj.isEGS(ii)) traj.clearEGS(ii);
	}
	double tBGS = traj.time(ixBGS);
	traj.timeShiftPlan(ixBGS+1,1000);
	//fpln(" $$$ adjustGsInsideAccelToEGS: ixBGS = "+ixBGS+" traj = "+traj);
	for (int j = ixBGS+1; j < ixEGS; j++) {
		double d_j = traj.pathDistance(ixBGS,j);
		d_j = d_j - 1E-8;     // to keep timeFromDistance from return NAN
		double t_j = tBGS + Plan::timeFromDistance(gsBGS, gsAccel, d_j);
		if (ISNAN(t_j)) {
            traj.addError("adjustGsInsideAccel"+gsFail+": timeFromDistance generated NaN");
			//fpln(" $$$$$$$$$$$$$$$$$$$$ TrajGen.adjustGsInsideAccel: j = "+j+" gsBGS = "+gsBGS+" gsAccel = "+gsAccel+" d_j = "+d_j);
		} else {
		   //fpln(" $$$ adjustGsInsideAccelToEGS: set time of j = "+j+" to "+t_j+" sz = "+traj.size());
		   traj.setTime(j,t_j);
		}
	}
	// setting EGS time
	double tEGS = tBGS - gsBGS/gsAccel;
	traj.setTime(ixEGS,tEGS);
	// ground speed should be zero at ixEGS, times cannot be based on ground speed
	if (ixEGS < traj.size()) {
       double dtNow = traj.time(ixEGS+1) - traj.time(ixEGS);
       double timeCorrection = dtAtIxEGS - dtNow;
       //fpln(" $$$ adjustGsInsideAccelToEGS: timeCorrection = "+timeCorrection);
       traj.timeShiftPlan(ixEGS+1,timeCorrection);
	}
}

bool TrajGen::attemptRepair2(Plan& kpc, int ixBGS, int ixEGS, double maxGsAccel) {
	//f.pln("\n $$ attemptRepair2: ############## ENTER attemptRepair ixBGS = "+ixBGS+" ixEGS = "+ixEGS+" kpc = "+kpc.toStringGs());
    if (kpc.inGsAccel(ixBGS-1)) {               // ixBGS falls within a previously generated BGS+EGS pair
    	    int prevBGS = kpc.prevBGS(ixBGS);
    	    kpc.revertGsTCP(prevBGS);
    }
    for (int jj = ixEGS-1; jj > ixBGS; jj--) {
		if (kpc.isBGS(jj)) {
			kpc.crudeRevertGsTCP(jj);
			//f.pln("$$ attemptRepair2: REVERT at jj = "+jj+"  kpc = "+kpc.toStringGs());
		}
	}
	//f.pln(" $$$$ attemptRepair2: kpc = "+kpc.toStringGs());
	int ixNewEGS = kpc.nextEGS(ixBGS);
	PlanUtil::fixGs_continuity(kpc, ixBGS, ixNewEGS, maxGsAccel);
	bool ok = kpc.isWellFormed();
	return ok;
}


TcpData TrajGen::makeBGS(const TcpData& tcp1, double a, bool isAltPreserve) {
	TcpData BGS_tcp;
	BGS_tcp = tcp1.copy();
	if (isAltPreserve) BGS_tcp.setAltPreserve();
	BGS_tcp.setBGS(a);
	return BGS_tcp;
}

TcpData TrajGen::makeEGS(const TcpData& tcp1) {
	TcpData EGS_tcp = TcpData();
	EGS_tcp.setEGS();
	return EGS_tcp;
}


/** Generate Vertical acceleration TCPs
 *  It assumes that all horizontal passes have been completed.
 *
 * @param kpc      kinematic plan with final horizontal path
 * @param maxVsAccel  vertical speed acceleration (positive)
 * @return
 */
Plan TrajGen::generateVsTCPs(const Plan& kpc, double maxVsAccel, bool continueGen) {
	if (maxVsAccel < 0) maxVsAccel = -maxVsAccel;
	Plan traj(kpc);
	for (int i = 1; i < traj.size()-1; i++) {
		double a_vs = maxVsAccel;
		double vsAccel = std::abs(traj.vsAccel(i));  // see if vsAccel is stored at point i
		if (vsAccel > 0) {
			a_vs = vsAccel;
			//traj.appendInfo(i,Plan::manualVsAccel);
		}
		//fpln("$$>> generateVsTCPs: i = "+Fm0(i)+" vsAccel "+Fm2(vsAccel));
		i = generateOneVsTCPs(traj, i, a_vs, continueGen);
	} // for loop
	return traj;
} // generateVsTCPs


/** Generate a BVS EVS pair at index i if deltaVs is large enough
 *
 * @param traj            trajectory
 * @param i               index
 * @param vsAccel         vertical acceleration
 * @param continueGen     if true continue generation even if errors are encountered, results in a partial kinematic plan
 * @return                revised traj
 */
int TrajGen::generateOneVsTCPs(Plan& traj, int i, double vsAccel_d, bool continueGen) {
	if (vsAccel_d == 0) {
		traj.addError("generateVsTCPs"+vsFail+": vsAccel = "+Fm1(vsAccel_d));
		return i;
	}
	NavPoint np2 = traj.point(i);
	double vs1 = traj.vsOut(i-1);
	double vs2 = traj.vsOut(i);
	int sign = 1;
	if (vs1 > vs2) sign = -1;
	double a = vsAccel_d*sign;
	double deltaVs = vs2 - vs1;
	double accelTime = std::abs(deltaVs/a);
    bool vsAccelNeeded = std::abs(deltaVs) > Plan::MIN_VS_DELTA_GEN;
	if (vsAccelNeeded) {
		//fpln(" $$$ generateOneVsTCPs: i = "+Fm0(i)+" vsAccel_d = "+Fm1(vsAccel_d)+" accelTime = "+Fm1(accelTime));
		double tbegin = np2.time() - accelTime/2.0;
		double tend = tbegin + accelTime;
		double prevEndTime = traj.getFirstTime();
		int ixEVS =  traj.prevEVS(i);
		if (ixEVS >= 0) prevEndTime = Util::max(prevEndTime,traj.time(ixEVS));
		//double nextVsChangeTm = TrajGen::nextVsChangeTime(traj, i, vsAccel_d);
		int nextVsChangeIx = PlanUtil::nextVsChange(traj, i);
		double nextVsChangeTm = traj.time(nextVsChangeIx);
		if (tbegin < prevEndTime || tend > nextVsChangeTm) {
			if (! continueGen) {
				traj.addError("generateVsTCPs"+vsFail+": Insufficient time i = "+Fm0(i)+" for vertical accel! tbegin = "+Fm1(tbegin)+" prevEndTime = "+Fm1(prevEndTime));
				printError("generateVsTCPs"+vsFail+": Insufficient time at i = "+Fm0(i)+" for vertical accel! tbegin = "+Fm1(tbegin)+" prevEndTime = "+Fm1(prevEndTime));
			}
			return i;
		} else { // SUCCESS
			bool newTCPneeded = (tbegin >= 0 && tend >= 0);
			//fpln("$$$$ generateVsTCPs: i = "+i+" tbegin = "+f.Fm2(tbegin)+" tend = "+f.Fm2(tend)+" np1 = "+np1.time()+" np2 = "+np2.time() );
			if (newTCPneeded) {
				if (traj.inVsChange(tbegin)) {
					traj.addError("generateOneVsTCPs"+vsFail+": VsAccel Overlap!");
				} else {
					//fpln(i+" $$ generateVsTCPs: $$ generateVsTCPs: tbegin = "+f.Fm2(tbegin)+" tend = "+f.Fm2(tend)+" np1 = "+np1.time()+" np2 = "+np2.time() );
					std::string label = traj.point(i).name();
					std::string info_i = traj.getInfo(i);
					std::pair<NavPoint,TcpData> bPair = Plan::makeBVS("", traj.position(tbegin), tbegin, a);
					std::pair<NavPoint,TcpData> ePair = Plan::makeEVS(traj.position(tend), tend);
					bool removedVsTransitionPt = false;
					bool manualVsAccel = std::abs(traj.vsAccel(i)) > 0;
					if ( ! traj.isTCP(i) && ! traj.isMOT(i)) {
						traj.remove(i);  // remove the original vs-transition point
						removedVsTransitionPt = true;
					}
					int bindex = traj.add(bPair);
					int eindex = traj.add(ePair);
					if (bindex < 0) {
						bindex = -bindex;   // There was an overlap
					}
					if (eindex < 0) {
						eindex = -eindex;   // There was an overlap
					}
					if (removedVsTransitionPt) {
						traj.setPointName(bindex, label);
						traj.setInfo(bindex, info_i);
					} else {
						if (manualVsAccel) {  // move manualVsFlag to BVS
							traj.appendInfo(bindex, Plan::manualVsAccel);
							int iVertex = i+1;   // added BVS
							traj.setInfo(iVertex, replace(traj.getInfo(iVertex),Plan::manualVsAccel,""));
						}
					}
					i = eindex;
					// fix altitude for all remaining points between b and e: altitudes should be between b.alt and e.alt
					bool altok = fixAlts(traj, bindex, eindex, vs1, a);
					if (!altok) {  // only create one error message
						traj.addError("generateVsTCPs"+vsFail+": generated altitude is out of bounds");
					}
				}
			}

		}
	}
	return i;
}



bool TrajGen::fixAlts(Plan& traj, int bindex, int eindex, double vs1, double accel) {
	bool altok = true;
	double balt = traj.point(bindex).alt();
	double tbegin = traj.time(bindex);
	for (int k = bindex+1; k <= eindex; k++) {
		double dt2 = traj.time(k) - tbegin;
		double newAlt = balt + vs1*dt2 + 0.5*accel*dt2*dt2;
		//fpln(" $$$$ SET: k = "+k+"  dt2 = "+dt2+"  newAlt = "+Units::str("ft",newAlt));
		NavPoint newNp = traj.point(k).mkAlt(newAlt);
		TcpData newNp_tcp = traj.getTcpData(k);
		//fpln(" $$ generateVsTCPs: traj.finalVelocity(k-1) = "+traj.finalVelocity(k-1));
		if (newAlt >= 0 && newAlt <= maxAlt) {
			traj.set(k, newNp, newNp_tcp);
		} else {
			altok = false;
		}
	}//for
	return altok;
}



/** calculate radius at vertex ix,  If a radius has been set use it, otherwise use bank angle and GS to
 *  calculate value
 *
 * @param p          Plan
 * @param ix         index of vertex point
 * @param bank       bank angle
 * @return
 */
double TrajGen::calcRadius(const Plan& p, int ix, double bank) {
	double gsIn = p.gsIn(ix);
	double R = p.vertexRadius(ix); 	   // if R is 0 then turnTime will be zero and point will not be processed
	if (Util::almost_equals(R, 0.0)) {      // no specified radius
		R = Kinematics::turnRadius(gsIn, bank);
	}
	return R;
}


Plan TrajGen::linearRepairShortTurnLegs(const Plan& fp, double bank, bool addMiddle) {
	Plan npc = Plan(fp);  //fp.copy();
	for (int j = 0; j+3 < fp.size(); j++) {
		if (Util::almost_equals(fp.gsIn(j+1),0.0)) continue;   // first vertex
		if (Util::almost_equals(fp.gsIn(j+2),0.0)) continue;   // second vertex
		NavPoint p0 = fp.point(j);
		NavPoint p1 = fp.point(j+1);
		NavPoint p2 = fp.point(j+2);
		NavPoint p3 = fp.point(j+3);
		double deltaTrack1 = std::abs(fp.trkIn(j+1) - fp.trkOut(j+1));   // vertex at j+1
		double deltaTrack2 = std::abs(fp.trkIn(j+2) - fp.trkOut(j+2));   // vertex at j+2
		//fpln(j+"#### removeShortLegsBetween: deltaTrack1 = "+Units::str("deg",deltaTrack1)+" deltaTrack2 = "+Units::str("deg",deltaTrack2));
		if (deltaTrack1> Plan::MIN_TRK_DELTA_GEN  && deltaTrack2 > Plan::MIN_TRK_DELTA_GEN ) {
			double R1 = calcRadius(fp, j+1, bank);
			double R2 = calcRadius(fp, j+2, bank);
			Tuple6<Position,Position,Position,int,double,Position> turn1 = TurnGeneration::turnGenerator(p0.position(), p1.position(), p2.position(), R1);
			Position EOT_1 = turn1.third;
			Tuple6<Position,Position,Position,int,double,Position> turn2 = TurnGeneration::turnGenerator(p1.position(), p2.position(), p3.position(), R2);
			Position BOT_2 = turn2.first;
			double distP1EOT = p1.position().distanceH(EOT_1);
			double distP1BOT = p1.position().distanceH(BOT_2);
			//f.pln(j+" ######### removeShortLegsBetween: distP1BOT = "+Units.str("nm",distP1BOT)+" distP1EOT = "+Units.str("nm",distP1EOT));
			if (distP1EOT > distP1BOT ) {
				//fpln("............removeShortLegsBetween REMOVE npc.point(j+2) = "+npc.point(j+2)+ " npc.point(j+1) = "+npc.point(j+1));
                 std::string info_1 = npc.getInfo(j+1);
                 std::string info_2 = npc.getInfo(j+2);
                 std::string name_1 = npc.getPointName(j+1);
                 std::string name_2 = npc.getPointName(j+2);
				npc.remove(j+2);
				npc.remove(j+1);
				double gsOut = fp.gsOut(j);
				if (addMiddle) {
					Position mid = p1.position().midPoint(p2.position());
					double tmid =npc.time(j)+  mid.distanceH(p0.position())/gsOut;
					NavPoint midNP = p1.makePosition(mid).makeTime(tmid); // preserve source info
					// fpln(" $$ ADD removeShortLegsBetween: midNP = "+midNP);
					int ixMid = npc.addNavPoint(midNP);
					npc.appendInfo(ixMid,info_1+info_2);
					npc.setPointName(ixMid,name_1+name_2);
					npc =  PlanUtil::mkGsConstant(npc, j,j+2,gsOut);
				} else {
					double dist = p3.distanceH(p0);
					double dt = dist/gsOut;
					double t0 = p0.time();
					//fpln(" $$## t0 = "+t0+" dt = "+dt+" j+1 = "+(j+1)+" gs1 = "+Units::str("kn",gs1)+" dist = "+Units::str("nm",dist));
					npc.setTime(j+1,t0+dt);
				}
				j = j+2;
				//				}
			}
		}
	}
	//fpln(" removeShortLegs: npc = "+npc);
	return npc;
}


// Make vertical speeds constant by adjusting altitudes between wp1 and wp2
// Assume there are no vertical speed accelerations
// do not alter wp1 and wp2
Plan TrajGen::makeMarkedVsConstant(const Plan& kpc) {
	Plan traj(kpc);
	int prevIndex = 0;
	for (int i = 1; i < traj.size(); i++) {
		NavPoint currFixedAlt = traj.point(i);
		TcpData currFixedAltTcp = traj.getTcpData(i);
		if (traj.isAltPreserve(i) || i == traj.size()-1) {
			NavPoint prevFixedAlt = traj.point(prevIndex);
			//fpln("makeMarkedVsConstant: Altitude at point i = "+i+" IS FIXED at "+Units::str4("ft",prevFixedAlt.alt()));
			double constantVs = (currFixedAlt.alt() - prevFixedAlt.alt())/(currFixedAlt.time() - prevFixedAlt.time());
			//fpln("makeMarkedVsConstant: end of segment: "+i+" vs="+Units::to("fpm",constantVs));
			// fix all points between the two fixed altitude in points
			for (int j = prevIndex+1; j < i; j++) {
				NavPoint np = traj.point(j);
				double dt = np.time() - prevFixedAlt.time();
				double newAlt = prevFixedAlt.alt() + constantVs*dt;
				np = np.mkAlt(newAlt);
				TcpData tcp = traj.getTcpData(j);
				//				if (traj.isTCP(j)) {
				//					tcp = tcp.setVelocityInit(traj.initialVelocity(j).mkVs(constantVs));
				//				}
				traj.set(j,np, tcp);
			}
			prevIndex = i;
		}
	}
	return traj;
}


/*
	The generateTCPs function translates a PlanCore object into a PlanCore object using 
	multiple passes.  It first processes turns, then vertical speed changes, and the ground speed 
	changes. The instantaneous transitions of the linear plan are converted into constant acceleration 
	segments that are defined by TCPs.  The process is incredibly ambiguous.  We are seeking to discover 
	reasonable interpretation that is both simple and useful.

	In the first pass instantaneous turns are replaced by turn TCPS which introduce a circular path.
	This circular path is shorter than the path in the linear plan because it turns before the vertex
	point.  This causes the first issue.  

 */


double TrajGen::directToContinueTime(const Plan& lpc, const Position& so, const Velocity& vo, double to, double bankAngle) {
	const double extraProjTime = 1.0;
	NavPoint np0 = lpc.point(0);
	double gs0 = vo.gs();
	double dist1 = so.distanceH(np0.position());
	double nominalTime = dist1/gs0;
	double deltaTrack = Util::turnDelta(lpc.initialVelocity(0).trk(),vo.trk());
	double estTurnTime = Kinematics::turnTime(gs0,deltaTrack,bankAngle);
	fpln(" #### genDirectTo nominalTime = "+Fm1(nominalTime)+" deltaTrack = "+Units::str("deg",deltaTrack)+" estTurnTime  = "+Fm1(estTurnTime));
	//double deltaGs = std::abs(vin.gs() - gs2);
	double dt = np0.time() - to;
	double continueTm = Util::min(estTurnTime+extraProjTime,dt/2);
	return continueTm;
}

/**
 *
 * @param lpc    a linear plan that needs to be connected to, preferable at point 0
 * @param so     current position
 * @param vo     current velocity
 * @param to     current time
 * @param bankAngle  turn bank angle
 * @return new plan with (so,vo,to) added as a first point and a leadIn point added
 */
Plan TrajGen::genDirect2(const Plan& pc, const Position& so, const Velocity& vo, double to, double bankAngle) {
	Plan lpc = pc;
	if (lpc.size() == 0) {
		fpln("genDirectTo: ERROR empty plan provides no merge point!");
		return lpc;
	}
	double continueTm = directToContinueTime(lpc,so,vo,to,bankAngle);
	if (continueTm >  (lpc.point(0).time() - to) && lpc.size() > 1) {
		fpln(" remove first point from plan -- not enough time");
		lpc.remove(0);
		continueTm = directToContinueTime(lpc,so,vo,to,bankAngle);
	}
	NavPoint npNew(so.linear(vo,continueTm), to+continueTm);
	NavPoint npNew0(so, to);
	int idx = lpc.addNavPoint(npNew0);
	if (idx < 0)
		fpln(" genDirectTo: ERROR -- could not add direct to point!");
	//fpln(" #### genDirectTo0: lpc = "+lpc+" idx = "+idx);
	idx = lpc.addNavPoint(npNew);
	//fpln(" #### genDirectTo1: lpc = "+lpc+" idx = "+idx);
	//lpc = lpc.makeSpeedConstant(0,2);
	return lpc;
}

/**
 *
 * @param lpc    a linear plan that needs to be connected to, preferable at point 0
 * @param so     current position
 * @param vo     current velocity
 * @param to     current time
 * @param bankAngle  turn bank angle
 * @return new plan with (so,vo,to) added as a first point and a leadIn point added
 */
Plan TrajGen::genDirectToLinear(const Plan& fp, const Position& so, const Velocity& vo, double to, double bankAngle, double timeBeforeTurn) {
	Plan lpc = Plan(fp);
	Position wpp = lpc.point(0).position();
	NavPoint new0(so,to);
	Triple<Position,double,double> vertTriple = ProjectedKinematics::genDirectToVertex(so, vo, wpp, bankAngle, timeBeforeTurn);
	double newTime = to+vertTriple.second;
	double initialTime = lpc.point(0).time();
	lpc.addNavPoint(new0);
	if (ISNAN(vertTriple.second)) { // turn is not achievable
		lpc.addError("TrajGen.genDirectTo: the required turn radius is larger than can reach target point!",1);
	} else if (newTime > initialTime){
		//fpln(" $$$$ genDirectTo: newTime = "+newTime+" initialTime = "+initialTime);
		lpc.addError("TrajGen.genDirectTo: the time of the vertex point exceeds the first point of lpc",1);
	} else if (vertTriple.third < 0 || newTime < 0) {
		lpc.addError("TrajGen.genDirectTo: could not generate direct to",1);
	} else {
		//fpln(" $$$$$>>>>>>>>>.. vertTriple.second = "+vertTriple.second);
		NavPoint vertexNP(vertTriple.first,newTime);
		//fpln(" $$$ genDirectTo: ADD vertexNP = "+vertexNP+" vertTriple.third = "+vertTriple.third);
		lpc.addNavPoint(vertexNP);
		//lpc.add(eotNP);
	}
	return lpc;
}

//Plan TrajGen::genDirectTo(const Plan& fp, const Position& so, const Velocity& vo, double to,
//		double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn) {
//	//double minVsChangeRecognized = vsAccel*getMinTimeStep();
//	return genDirectTo(fp, so, vo, to, bankAngle, gsAccel, vsAccel, MIN_ACCEL_TIME, timeBeforeTurn);
//}

//Plan TrajGen::genDirectTo(const Plan& fp, const Position& so, const Velocity& vo, double to,
//		double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn) {
//	Plan lpc = fp;
//	if (!fp.isLinear()) {
//		lpc = PlanUtil::revertTCPs(fp);
//	} else {
//		lpc = (Plan) fp;
//	}
//	Plan lpc2 = TrajGen::genDirectToLinear(lpc,so,vo,to,bankAngle,timeBeforeTurn);
//	//fpln(" $$$ genDirectTo: lpc2.initialVelocity(1) = "+lpc2.initialVelocity(1));
//	//fpln("$$$ +kpc.initialVelocity(3)  = "+kpc.initialVelocity(3));
//	Plan kpc = TrajGen::makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, false, true, true);
//	//bool twoTurnOverlap = std::abs(lpc2.initialVelocity(1).compassAngle() -  kpc.initialVelocity(3).compassAngle()) > Units::from("deg",10.0);
//	bool twoTurnOverlap = Util::turnDelta(lpc2.trkOut(1),kpc.trkOut(3)) > Units::from("deg",10.0);
//
//	if (twoTurnOverlap) {
//		//printError(" $$$ genDirectTo: Turns Overlap");
//		kpc.addError("TrajGen.genDirectTo: Turns Overlap",1);
//	}
//	if (lpc2.hasError()) {
//		//fpln(" $$$$$$$$$ genDirectTo: GENERATION ERROR: "+lpc2.getMessage());
//		kpc.addError(lpc2.getMessage(),0);
//	}
//	return kpc;
//}

//Plan TrajGen::genDirectToRetry(const Plan& p, const Position& so, const Velocity& vo, double to,
//		double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn, double timeIntervalNextTry) {
//	double minVsChangeRecognized = vsAccel*getMinTimeStep();
//	return genDirectToRetry(p, so, vo, to, bankAngle, gsAccel, vsAccel, timeBeforeTurn,timeIntervalNextTry);
//}

//Plan TrajGen::genDirectToRetry(const Plan& p, const Position& so, const Velocity& vo, double to,
//		double bankAngle, double gsAccel, double vsAccel,  double timeBeforeTurn, double timeIntervalNextTry) {
//	Plan fp = p;
//	bool done = false;
//	Plan kpc;
//	do {
//		kpc = TrajGen::genDirectTo(fp,so,vo,to,bankAngle,gsAccel,vsAccel,  timeBeforeTurn);
//		//fpln(" $$$$ generate GenDirectTo !!!! kpc.size() = "+kpc.size()+" kpc.hasError() = "+kpc.hasError());
//		if (kpc.hasError() && fp.size() > 1) {
//			double tm0 = fp.point(0).time();
//			double tm1 = fp.point(1).time();
//			if (tm0 + timeIntervalNextTry + 20 < tm1) { // add connection point
//				Position connectPt = fp.position(tm0+timeIntervalNextTry);
//				//fpln(" $$$$ Add connectPt = "+connectPt+" at time "+(tm0+timeIntervalNextTry));
//				fp.addNavPoint(NavPoint(connectPt,tm0+timeIntervalNextTry));
//				fp.remove(0);
//			} else {
//				fp.remove(0);
//			}
//		} else {
//			done = true;
//		}
//	} while (!done);
//
//	// 	     PlanCore lpc2 = TrajGen::genDirectToLinear((PlanCore) fp,so,vo,to,bankAngle,timeBeforeTurn);
//	//   		 kpc = TrajGen::makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, true);
//
//	return kpc;
//}

/**
 * Construct a (hopefully) feasible linear plan that describes a path from s to the goal point.
 * If this fails, it reurns a simple direct plan.
 */
Plan TrajGen::buildDirectTo(std::string id, const NavPoint& s, const Velocity& v, const Plan& base, double bankAngle) {
	double minTimeStep = 10.0;
	bool done = false;
	double tt = base.getFirstTime();
	double range = base.getLastTime()-s.time();
	double stepSize = Util::max(minTimeStep, range/20.0);
	double minGsDiff = Units::from("kts", 10.0);
	//double bestTime = base.getLastTime();
	double bestdgs = MAXDOUBLE;
	Quad<Position,Velocity,double,int> bestdtp;
	bool bestdtpDef = false;
	// find the "best" point at which to reconnect
	while (!done && tt < base.getLastTime()) {
		if (tt > s.time()) { // first filter -- need to connect in the future
			double R = Kinematics::turnRadius(v.gs(), bankAngle);
			Quad<Position,Velocity,double,int> dtp = ProjectedKinematics::directToPoint(s.position(), v, base.position(tt), R);
			double t2 = s.time()+dtp.third;
			if (t2 < tt) {  // second filter -- need to finish directTo turn before connect point
				// bool turnRight = Kinematics::turnRight(v, dtp.second.trk());
				double dgs = std::abs(dtp.second.gs() - dtp.first.distanceH(s.position())/(tt-t2));
				if (dgs+minGsDiff < bestdgs) {
					bestdgs = dgs;
					//bestTime = tt;
					bestdtp = dtp;
					bestdtpDef = true;
				}
			}
		}
		tt += stepSize;
	}
	Plan lpc(base.getName());
	lpc.addNavPoint(s);
	if (!bestdtpDef) {
		//NavPoint np = s.linear(v, minTimeStep + bestdtp.third/2.0); // halfwat through turn plus some slop
		for (int i = 0; i < base.size(); i++) {
			if (base.time(i) < tt) lpc.addNavPoint(base.point(i));
		}
		return lpc;
	}
	// fail
	lpc.addNavPoint(base.point(base.size()-1));
	return lpc;
}


/**
 * Returns a PlanCore version of the given plan.  If it was a kinematic plan, this will attempt to regress the TCPs to their original
 * source points (if the proper metadata is available).  If this is already a PlanCore, return a copy.
 */
Plan TrajGen::makeLinearPlan(const Plan& fp) {
	if (fp.isLinear()) {
		return Plan(fp);
	}
	//return PlanUtil::structRevertTCPs(fp,false);
	//Plan np = Plan(fp);
	//PlanUtil::structRevertTCPs(np,false);
	Plan np = PlanUtil::revertAllTCPs(fp,false);
	return np;
}


void TrajGen::setVerbose(bool b) {
	verbose = b;
}

}

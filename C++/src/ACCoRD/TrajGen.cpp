/* Trajectory Generation
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Plan.h"
#include "PlanUtil.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"   //TODO -- get rid of this
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


bool TrajGen::verbose = false;
const std::string TrajGen::minorTrkChangeLabel = "::minorTrkChange:";  // segmentation fault
//const std::string TrajGen::minorTrkChangeLabel = "$minorTrkChange:";
const double TrajGen::MIN_ACCEL_TIME = 1.0;
const double TrajGen::MIN_TURN_BUFFER = Plan::minDt;
const double TrajGen::MIN_VS_CHANGE = Units::from("fpm",50);
const double TrajGen::MIN_VS_TIME = Units::from("s",30);

bool TrajGen::method2 = true;

//const double TrajGen::minVsChangeRecognizedDefault = 2.0; // Units::from("fpm", 50);

const double TrajGen::minorVfactor = 0.01; // used to differentiate "minor" vel change vs no vel change
const double TrajGen::maxVs = Units::from("fpm",10000);
const double TrajGen::maxAlt = Units::from("ft",60000);

/**
 * Trajectory generation is discretized by a certain time unit (meaning no two points on the trajectory should be closer in time than this value)
 */
double TrajGen::getMinTimeStep() {
    // return Constants::get_time_accuracy();
	return MIN_ACCEL_TIME;
}


void TrajGen::printError(const std::string& s) {
	if (verbose) fpln(s);
}

Plan TrajGen::repairPlan(const Plan& fp, bool repairTurn, bool repairGs, bool repairVs,
		bool flyOver, bool addMiddle, double bankAngle, double gsAccel, double vsAccel) {
	Plan lpc = fp;
	if (repairTurn) {
		//fpln(" generateTCPs: removeShortLegs -----------------------------------");
		bool addMiddle = true;
		lpc = TrajGen::linearRepairShortTurnLegs(lpc, bankAngle, addMiddle);
	}
	//fpln("repairPlan turn :"+lpc.toString());
	if (lpc.hasError()) {
		//fpln(" $$$0 generateTCPs: repair failed! "+lpc.getMessageNoClear());
		//DebugSupport.halt();
	} else {
		if (lpc.hasError())  {
			//fpln(" $$$1 generateTCPs: repair failed! "+lpc.getMessageNoClear());
		} else {
			if (repairVs) {
				lpc = TrajGen::linearRepairShortVsLegs(lpc,vsAccel);
				if (lpc.hasError()) {
					//fpln(" $$$2 generateTCPs: repair failed! "+lpc.getMessageNoClear());
				} else {
					if (repairTurn) {
						bool strict = false;
						lpc = removeInfeasibleTurns(lpc,bankAngle, strict);
					}
//					if (lpc.hasError()) {
//						//fpln(" $$$3 generateTCPs: repair failed! "+lpc.getMessageNoClear());
//					}
					//DebugSupport.dumpPlan(lpc, "generateTCPs_after_repair", 0.0);
				}
			}
		}
	}
	return lpc;
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
Plan TrajGen::makeKinematicPlan(const Plan& fp, double bankAngle, double gsAccel, double vsAccel,
		bool repairTurn, bool repairGs, bool repairVs) {
	//Plan lpc = makeLinearPlan(fp);
	Plan lpc = fp.copyWithIndex();
	Plan & ret = lpc;
	if (lpc.size() < 2) {
		ret = lpc;
	} else {
		//		fpln(" generateTCPs: ENTER ----------------------------------- lpc = "+lpc.toString());
		bool addMiddle = true;
		bool flyOver = false;
		lpc = repairPlan(lpc, repairTurn, repairGs, repairVs, flyOver, addMiddle, bankAngle, gsAccel, vsAccel);
		//fpln(" generateTCPs: generateTurnTCPs ---------------------------------------------------------------------");
		Plan kpc = markVsChanges(lpc);
		Plan kpc2 = generateTurnTCPs(kpc, bankAngle);
		//DebugSupport.dumpPlan(kpc2, "generateTCPs_turns");
		if (kpc2.hasError()) {
			ret = kpc2;
		} else {
			//DebugSupport.dumpPlan(kpc2, "generateTCPs_fixgs");
			//fpln(" generateTCPs: generateGsTCPs ----------------------------------- "+kpc2.isWellFormed());
			bool useOffset = true;
			Plan kpc3 = generateGsTCPs(kpc2, gsAccel, repairGs, useOffset);
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
					cleanPlan(kpc5);
					//fpln(" generateVsTCPs: DONE ----------------------------------- "+kpc5.isWellFormed());
					ret = kpc5;
				}
			}
		}
	}
	//fpln(" $$$$ makeKinematicPlan: "+fp.getNote());
	ret.setNote(fp.getNote());
	return ret;
}



Plan TrajGen::makeKinematicPlan(const Plan& fp, double bankAngle, double gsAccel, double vsAccel, bool repair) {
	Plan traj = makeKinematicPlan(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair);
	return traj;
}

/**
 * Remove records of deleted points and make all remaining points "original"
 * @param fp
 */
void TrajGen::cleanPlan(Plan& fp) {
	fp.mergeClosePoints(Plan::minDt);
	for (int i = 0; i < fp.size(); i++) {
        TcpData tcp = fp.getTcpData(i).setOriginal();
		fp.set(i,fp.point(i), tcp);
	}
}



int TrajGen::prevTrackChange(const Plan& fp, int iNow) {
	for (int i = iNow-1; i > 0; i--) {
		if (i==1) return 0;
		//NavPoint npi = fp.point(i);
		//NavPoint npPrev = fp.point(i-1);
		Velocity vf0 = fp.finalVelocity(i-1);
		Velocity vi1 = fp.initialVelocity(i);
		if (std::abs(vf0.trk() - vi1.trk()) > Units::from("deg",1.0)) return i;
	}
	return 0;
}

int TrajGen::nextTrackChange(const Plan& fp, int iNow) {
	for (int i = iNow+1; i+1 < fp.size(); i++) {
		if (i>=fp.size()-1) return fp.size()-1;
		//NavPoint npi = fp.point(i);
		//NavPoint npNext = fp.point(i+1);
		Velocity vf0 = fp.finalVelocity(i);
		Velocity vi1 = fp.initialVelocity(i+1);
		//fpln("nextTrackChange:  i = "+i+" vf0 = "+vf0+" vi1 = "+vi1);
		if (std::abs(vf0.trk() - vi1.trk()) > Units::from("deg",1.0)) return i+1;
	}
	return fp.size()-1;
}

bool TrajGen::turnIsFeas(const Plan& lpc, int i, const NavPoint& BOT, const NavPoint& EOT) {
	NavPoint np2 = lpc.point(i);
	double d1new = BOT.distanceH(np2);
	double d2new = np2.distanceH(EOT);
	int ipTC = i-1;
	int inTC = i+1;
	ipTC = prevTrackChange(lpc,i);
	inTC = nextTrackChange(lpc,i);
	double d1old = lpc.point(ipTC).distanceH(np2);
	double d2old = np2.distanceH(lpc.point(inTC));
	bool rtn = true;
	//fpln(" $$$$ generateTurnTCPs: "+(BOT.time()-getMinTimeStep())+" "+fp.getTime(i-1)+" "+EOT.time()+getMinTimeStep()+" "+fp.getTime(i+1));
	if (BOT.time()-MIN_TURN_BUFFER <= lpc.time(ipTC) || EOT.time()+MIN_TURN_BUFFER >= lpc.time(inTC)
			|| d1new+Constants::get_horizontal_accuracy() >= d1old || d2new+Constants::get_horizontal_accuracy() >= d2old) {
		//trajCore(lpc); // revert to initial plan
		//traj.addError("ERROR in TrajGen::generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.",i);
		//printError("ERROR in TrajGen::generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.");
		if (verbose) {
			fpln("####### i = "+Fm0(i)+" ipTC = "+Fm0(ipTC)+" inTC = "+Fm0(inTC)+" np2 =  "+Plan::toStringFull(np2, lpc.getTcpData(i)));
			fpln("####### BOT.time() = "+Fm2(BOT.time())+" <? fp.getTime(ipTC) = "+Fm2(lpc.time(ipTC)));
			fpln("####### EOT.time() = "+Fm2(EOT.time())+" >? fp.getTime(inTC) = "+Fm2(lpc.time(inTC)));
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
Plan TrajGen::markVsChanges(const Plan& lpc) {
	//fpln(" markVsChanges: lpc = "+lpc);
	std::string name = lpc.getName();
	Plan kpc(name);
	if (lpc.size() < 2) {
		kpc = Plan(lpc);
		kpc.addError("TrajGen.markVsChanges: Source plan "+name+" is too small for kinematic conversion");
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
//		if (Util::within_epsilon(vs1, vs2, MIN_VS_CHANGE) && (deltaTm < MIN_VS_TIME)) {
//			kpc.add(np,tcp);
//		} else {
//			//f.pln(" $$>> markVsChanges "+i+" PUT mark at i = "+i+" "+np2.getTcpData().toString());
//			kpc.add(np, tcp.setAltPreserve());
//		}
        bool nextTheSame = true;
		if (i+1 < lpc.size()) {
			double vs3 = lpc.vsOut(i);
			nextTheSame = std::abs(vs2-vs3) < MIN_VS_CHANGE;
        }
		double deltaVs = std::abs(vs1-vs2);
		//f.pln("$$$$$ markVsChanges: i =  "+i+" deltaVs = "+Units.str("fpm",deltaVs)+" deltaTm = "+deltaTm+" nextTheSame = "+nextTheSame);
		if (deltaVs >= MIN_VS_CHANGE && nextTheSame) {
			kpc.add(np, tcp.setAltPreserve());
		} else if (deltaTm >= MIN_VS_TIME) {
			kpc.add(np, tcp.setAltPreserve());
			//f.pln(" $$>>>>>>>>>>>>>> markVsChanges: i= "+i+" marked ALT = "+Units.str("ft",kpc.point(i).alt())+" deltaTm = "+deltaTm);
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
	// for (int i = 1; i+1 < lpc.size(); i++) {
	for (int i = lpc.size()-2; i > 0; i--) {
		//NavPoint np2 = lpc.point(i);
		if (lpc.inAccelZone(i)) continue;     // skip over regions already generated (for now only BOT-EOT pairs)
		Velocity vf0 = lpc.finalVelocity(i-1);
		Velocity vi1 = lpc.initialVelocity(i);
		double gsIn = vf0.gs();
		if (Util::almost_equals(gsIn,0.0)) {
			printError(" ###### WARNING: Segment from point "+Fm0(i-1)+" to point "+Fm0(i)+" has zero ground speed!");
		}
		double turnDelta = Util::turnDelta(vf0.trk(), vi1.trk());
		double turnTime;
		TcpData tcp_i = lpc.getTcpData(i);
		double R = tcp_i.turnRadius();
		if (Util::almost_equals(R, 0.0)) {
			if (bank == 0) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: specified bank angle is 0");
				bank = Units::from("deg",0.001); // prevent divisions by 0.0
			}
		   turnTime = Kinematics::turnTime(gsIn,turnDelta,bank);
		   R = Kinematics::turnRadius(gsIn, bank);
		} else {
		   turnTime =  turnDelta*R/gsIn;
		}
		if (turnTime >= getMinTimeStep()) {
			insertTurnTcpsInTraj(traj, lpc, i, R, continueGen);
			if (traj.hasError() && ! continueGen) return traj;
		}
	}
	//fpln(" $$>>>>>>>>>  generateTurnTCPsRadius: EXIT traj = "+traj.toStringFull());
	return traj;
}





Plan TrajGen::generateTurnTCPsRadius(const Plan& lpc, bool continueGen) { //, double default_radius) {
	Plan traj = Plan(lpc); // the current trajectory based on working
	//f.pln(" $$>> generateTurnTCPs: lpc = "+lpc.toStringFull());
	for (int i = lpc.size()-2; i > 0; i--) {   // Perform in reverse order so indexes match between lpc and traj
		Velocity vf0 = lpc.finalVelocity(i-1);
		Velocity vi1 = lpc.initialVelocity(i);
		double gsIn = vf0.gs();
		if (Util::almost_equals(gsIn,0.0)) {
			printError("TrajGen.generateTurnTCPs ###### WARNING: Segment from point "+Fm0(i-1)+" to point "+Fm0(i)+" has zero ground speed!");
			continue;
		}
		double turnDelta = Util::turnDelta(vf0.trk(), vi1.trk());
		TcpData tcp_i = lpc.getTcpData(i);
		double R = tcp_i.turnRadius();
		//f.pln(" $$>> generateTurnTCPsRadius(lpc): i = "+i+" R = "+Units.str("NM",R));
		// if R is 0 then turnTime will be zero and point will not be processed
		double turnTime = turnDelta*R/gsIn;    // if R = 0 ==> do not process this point
		if (turnTime >= getMinTimeStep()) {
			insertTurnTcpsInTraj(traj, lpc, i, R, continueGen);
			if (!continueGen && traj.hasError()) return traj;
		}
	}//for
	//f.pln(" $$>>>>>>>>>  generateTurnTCPsRadius: EXIT traj = "+traj.toStringFull());
	return traj;
}

bool TrajGen::trackContinuousAt(const Plan& traj, int ix, double maxDelta) {
	double courseIn = traj.finalVelocity(ix-1).compassAngle();
	double courseOut = traj.initialVelocity(ix).compassAngle();
	//f.pln(" $$>> generateTurnTCPs: courseIn = "+Units.str("deg",courseInB)+" courseOut = "+Units.str("deg",courseOut));
	double turnDelta = Util::turnDelta(courseIn,courseOut);
	//f.pln(" $$>> generateTurnTCPs: turnDeltaBOT = "+Units.str("deg",turnDeltaBOT));
	return (turnDelta < maxDelta);
}



Plan TrajGen::generateTurnTCPsRadius(const Plan& lpc) {
	bool continueGen = false;
	return generateTurnTCPsRadius(lpc, continueGen);
}

/**
 *
 * @param traj          trajectory under construction
 * @param lpc           original lpc
 * @param ixNp2             index of np2 in plan
 * @param np1
 * @param np2
 * @param tcp2
 * @param linearIndex
 * @param np3
 * @param R             unsigned radius
 * @param strict if strict do not allow any interior waypoints in the turn
 */
void TrajGen::insertTurnTcpsInTraj(Plan& traj, const Plan& lpc, int ixNp2, double R, bool continueGen) {
	NavPoint np1 = lpc.point(ixNp2-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
	NavPoint np2 = lpc.point(ixNp2);
	NavPoint np3 = lpc.point(ixNp2+1);
	if (np3.time() - np2.time() < 0.1 && ixNp2+2 < lpc.size()) {
		np3 = lpc.point(ixNp2 + 2);
	}
	int linearIndex = lpc.getTcpData(ixNp2).getLinearIndex();
	//f.pln(" $$$ insertTurnTcpsInTraj: lpc = "+lpc.toStringFull(true));
	Velocity vf0 = lpc.finalVelocity(ixNp2-1);
	Velocity vi1 = lpc.initialVelocity(ixNp2);
	double gsIn = vf0.gs();
	double targetGS = vi1.gs();
	Tuple5<NavPoint,NavPoint,NavPoint,int,Position> tg = TurnGeneration::turnGenerator(np1,np2,linearIndex,np3,R);
	NavPoint BOT = tg.getFirst();
	NavPoint MOT = tg.getSecond();
	NavPoint EOT = tg.getThird();
	int dir = tg.getFourth();
	Velocity vin = NavPoint::initialVelocity(np1,np2);
	Velocity vout = NavPoint::initialVelocity(np2,np3); // used to set gs and vs
	Velocity vin2 = BOT.position().initialVelocity(np2.position(),100);
	Velocity vout2 = np2.position().finalVelocity(EOT.position(),100);
	vin = vin.mkTrk(vin2.trk());
	vout = vout.mkTrk(vout2.trk());
	NavPoint np2_src = lpc.sourceNavPoint(ixNp2).makeLabel(np2.label());
	std::string srcinfo = lpc.getInfo(ixNp2);
	//f.pln(" $$$ insertTurnTcpsInTraj: i = "+i+" np2_src = "+np2_src);
	double signedRadius = dir*R;
	Position center;
	if (method2) {
		center = tg.fifth;
	} else {
	    center = KinematicsPosition::centerFromRadius(BOT.position(), signedRadius, vin.trk());
	}
	std::pair<NavPoint,TcpData> npBOT = Plan::makeBOT(np2_src, BOT.position(), BOT.time(),  signedRadius, center, linearIndex);
	std::pair<NavPoint,TcpData> npEOT =	Plan::makeEOT(np2_src, EOT.position(), EOT.time(), linearIndex);
	npBOT.second.setInformation(srcinfo);
	//std::pair<NavPoint,TcpData> npMOT = Plan.makeMidpoint(np2_src, tcp2, MOT.position(),MOT.time(),linearIndex);
	// Calculate altitudes based on lpc, NOTE THAT these times may be out of range of lpc and INVALIDs will be produced
	if (ixNp2 > 1 && BOT.time() < np1.time()) { //only okay if ground speeds did not change, see test testJBU173
		gsIn = traj.gsIn(ixNp2-1);   //ignore ground speed change in middle of turn
	}
	double botAlt;
	if (BOT.time() < lpc.getFirstTime()) {  // is this right?
		botAlt = lpc.point(0).alt();
	} else {
		//f.pln(" $$$ BOT.time() = "+BOT.time());
		botAlt = lpc.position(BOT.time()).alt();
	}
	BOT = BOT.mkAlt(botAlt);
	//f.pln(" $$$$ generateTurnTCPs: BOT.time() = "+f.Fm2(BOT.time())+" np times = "+f.Fm2(np1.time())+", "+f.Fm2(np2.time())+", "+f.Fm2(np3.time()));
	//f.pln(" $$$$ BOT.time() = "+BOT.time()+" botAlt = "+Units::str("ft", botAlt)+" lpc.getFirstTime() = "+lpc.getFirstTime());
	MOT = MOT.mkAlt(lpc.position(MOT.time()).alt());
	//f.pln(" $$>> generateTurnTCPs: MOT = "+MOT.toStringFull());
	double EotAlt;
	if (EOT.time() > lpc.getLastTime()) { // is this right?
		EotAlt = lpc.getLastPoint().alt();
	} else {
		EotAlt = lpc.position(EOT.time()).alt();
	}
	//f.pln(" $$: insertTurnTcpsInTraj: EotAlt =  "+Units::str("ft", EotAlt));
	EOT = EOT.mkAlt(EotAlt);
	std::string label = "";
	if ( np2.label() != "") label = " ("+np2.label()+") ";
	if (BOT.isInvalid() || MOT.isInvalid() || EOT.isInvalid()) {
		if (!continueGen) {
			traj.addError("ERROR in TrajGen.generateTurnTCPs: turn points at "+Fm0(ixNp2)+label+" invalid.",ixNp2);
			printError("ERROR in TrajGen.generateTurnTCPs: turn points at "+Fm0(ixNp2)+label+" invalid.");
		}
	} else if ( ! turnIsFeas(lpc, ixNp2, BOT, EOT)) {
		if (!continueGen) {
			traj.addError("ERROR in TrajGen.generateTurnTCPs: cannot achieve turn at point "+Fm0(ixNp2)+label
					+" from surrounding points at time "+Fm2(lpc.point(ixNp2).time()),ixNp2);
			printError("ERROR in TrajGen.generateTurnTCPs: cannot achieve turn "+Fm0(ixNp2)+label+" from surrounding points at "+Fm0(ixNp2));
		}
	} else if (EOT.time()-BOT.time() > 2.0*getMinTimeStep()) { // ignore small changes  is the 2.0 factor right?
		if (traj.inTrkChange(BOT.time())) {
			if (!continueGen) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn in time at "+Fm0(ixNp2)+label,ixNp2);
				printError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn int time at "+Fm0(ixNp2)+label);
			}
		} else if (traj.inTrkChange(EOT.time())) {
			if (!continueGen) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn in time at "+Fm0(ixNp2)+label,ixNp2);
				printError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn int time at "+Fm0(ixNp2)+label);
			}
		} else {
			int ixBOT = traj.add(npBOT);
			ixNp2 = ixNp2 + 1;                      // just added point before np2
			int ixEOT = traj.add(npEOT);
			//f.pln(" $$$$.......................................................... insertTurnTcpsInTraj: traj = "+traj);
			ixEOT = movePointsWithinTurn(traj,ixBOT,ixNp2,ixEOT,gsIn);
			// ********** cannot survive the following errors: already added BOT, EOT ************
			if (ixBOT < 0 || ixEOT < 0 ) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT/EOT overlaps point ");
				return;
			}
			if (!trackContinuousAt(traj, ixBOT, M_PI/10)) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into BOT not equal to track out of BOT at "+Fm0(ixBOT)+label);
				//f.pln("$$$$$$ TrajGen.generateTurnTCPs: track into BOT not equal to track out of BOT at "+ixBOT+label);
				return;
			}
			if (!trackContinuousAt(traj, ixEOT, M_PI/10)) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into EOT not equal to track out of EOT at "+Fm0(ixEOT)+label);
				//f.pln("TrajGen.generateTurnTCPs:  track into EOT not equal to track out of EOT at "+ixEOT+label);
				return;// traj;
			}
			//f.pln(" $$>> generateTurnTCPs: at ixBOT = "+ixBOT+"  ixEOT = "+ixEOT+" gsIn = "+Units::str("kn",gsIn,4));
			traj.mkGsIn(ixBOT, gsIn);  // ****
			//traj.mkGsOut(ixBOT, gsIn);
			traj.mkGsIn(ixEOT, gsIn);       // can this be removed?
			traj.mkGsOut(ixEOT, targetGS);
		}
	}
	//f.pln(" $$$ generateTurnTCPs:: traj = "+traj.toStringGs());
}



int TrajGen::movePointsWithinTurn(Plan& traj, int ixBOT, int ixNp2, int ixEOT,  double gsIn) {
	//f.pln(" $$$ movePointsWithinTurn: ------- ixBOT, ixNp2, ixEOT = "+ixBOT+", "+ixNp2+", "+ixEOT);
	NavPoint vertex = traj.point(ixNp2);
	NavPoint BOT = traj.point(ixBOT);
	NavPoint EOT = traj.point(ixEOT);
	double distVertexToEOT =  vertex.distanceH(EOT);
	double distVertexToBOT =  vertex.distanceH(BOT);
	double longDistTOEOT = traj.point(ixBOT).distanceH(vertex) + vertex.distanceH(EOT);
	//f.pln(" $$ movePointsWithinTurn_New: distVertexToEOT = "+Units::str("NM",distVertexToEOT,7));
	//f.pln(" $$ movePointsWithinTurn_New: distVertexToBOT = "+Units::str("NM",distVertexToBOT,7));
	//f.pln(" $$ movePointsWithinTurn_New: pathDist = "+Units::str("NM",pathDist,7));
	//f.pln(" $$ movePointsWithinTurn_New: longDistTOEOT = "+Units::str("NM",longDistTOEOT,7));
	int lastIxInTurn = -1;
	// ************ EOT can be out of order so let's remove it before we move inner points
	for (int ii = ixNp2; ii < traj.size(); ii++) {
		double dist_to_ii =  vertex.distanceH(traj.point(ii));
		//f.pln(" %%%%% generateTurnTCPs: ii = "+ii+" dist_to_ii = "+Units::str("NM",dist_to_ii,7));
		if (ii == ixEOT) continue;
		else if (dist_to_ii < distVertexToEOT) lastIxInTurn = ii;
		else break;
	}
	int firstIxInTurn = lastIxInTurn;
	for (int ii = ixNp2; ii > ixBOT; ii--) {
		double dist_from_ii =  vertex.distanceH(traj.point(ii));
		//f.pln(" %%%%% generateTurnTCPs: ii = "+ii+" dist_from_ii = "+Units::str("NM",dist_from_ii,5));
		if (dist_from_ii <= distVertexToBOT) firstIxInTurn = ii;
		else break;
	}
	Plan tempPlan = Plan();
	if (lastIxInTurn >= 0) {
		for (int j = lastIxInTurn; j >= firstIxInTurn; j--) {
			if (j != ixEOT) {
			   tempPlan.add(traj.get(j));
			   traj.remove(j);
			}
		}
	}
	double pathDist = traj.pathDistance(ixBOT,ixBOT+1);  // all inner points now removed
	double ratio = pathDist/longDistTOEOT;
	tempPlan.addNavPoint(traj.point(ixBOT));  // add as non-TCP
	double tmBOT = traj.time(ixBOT);
	for (int i =  tempPlan.size()-1; i > 0; i--) {
		NavPoint orig_i = tempPlan.point(i);
		TcpData tcp_i = tempPlan.getTcpData(i);
		double dist_to_i = tempPlan.pathDistance(0,i);  // linear distance
		double d_i = ratio*dist_to_i;
		bool linear = false;
		std::pair<Position, int> adv = traj.advanceDistance2D(ixBOT, d_i, linear);
		double alt_i = orig_i.alt();
		Position pos_i = adv.first.mkAlt(alt_i);
		double dt = d_i/gsIn;
		double t_i = tmBOT + dt;
		std::string label_i =  orig_i.label();
		double deltaToMOT = std::abs(dist_to_i - longDistTOEOT/2.0);
		if (deltaToMOT < 1.0) label_i = ""; // MOT label is moved to BOT
		NavPoint np_i = NavPoint(pos_i, t_i,label_i);
		if (tcp_i.isTrkTCP()) {
			traj.addError("ERROR in TrajGen.generateTurnTCPs: TURN overlap !!! at i = "+Fm0(i));
		}
		tcp_i.clearTrk();  // this could allow small turn in big turn
        tcp_i.setRadiusSigned(0.0); // inner points shuld not have radius data
		//f.pln(" $$ movePointsWithinTurn: ADD BACK d_i= "+Units::str("NM",d_i,5)+" np_i = "+np_i);
		traj.add(np_i,tcp_i);
	}
	ixEOT = traj.nextEOT(ixBOT);  // some of the points that were after EOT may have been moved in front
	return ixEOT;
}



// create GSC TCPS between np1 and np2 using vin from previous leg
/**
 * @param np1 start point of the acceleration segment/where the speed change occurs
 * @param np2 end point of the acceleration segment
 * @param targetGs target gs
 * @param vin velocity in
 * @param s  signed gs acceleration
 * @return third component: distance to EGS from BGS
 * NOTE!!! This has been changed so the accel time is always returned, but if it is less than getMinTimeStep(), there is no region and this has to be handled by the calling program!!!
 */
Triple<NavPoint,NavPoint,double> TrajGen::gsAccelGenerator(const Plan& traj, int ixNp1, double gsIn, double targetGs, double a, double timeOffset,  bool allowOverlap) {
	NavPoint np1 = traj.point(ixNp1);
	NavPoint np2 = traj.point(ixNp1+1);
	std::string label = np1.label();
	NavPoint np1b = np1.makeLabel(label);
	double accelTime = (targetGs - gsIn)/a;
	if (accelTime < getMinTimeStep()) {
		//fpln(" $$$$$ gsAccelGenerator no GS TCPS needed, at time +"+np1b.time()+" accelTime = "+accelTime);
		return Triple<NavPoint,NavPoint,double>(np1b,np2,0.0); // no change
	}
	double t0 = np1.time();
	//fpln(" $$## gsAccelGenerator: at t0 = "+Fm2(t0)+" Accelerate FROM gsIn = "+Units::str("kn",gsIn)+" TO targetGs = "+Units::str("kn",targetGs));
	double distToBGS = gsIn*timeOffset;
	//Position bPos = traj.advanceDistanceWithinSeg2D(ixNp1,distToBGS,false); // TODO: why does this work?
	Position bPos = traj.advanceDistance(ixNp1,distToBGS,false).first;
	double bTm = t0+timeOffset;
	NavPoint b = NavPoint (bPos,bTm).makeLabel(label); // .makeAdded();//.appendName(setName);
	double d = b.distanceH(np2) - targetGs*timeOffset;
	double remainingDist = d - accelTime * (gsIn+targetGs)/2;
	if ( ! allowOverlap &&  remainingDist <= 0) {
		//fpln(" ##### gsAccelGenerator: accelTime = "+accelTime+" remainingDist = "+Units::str("nm",remainingDist));
		traj.addError("TrajGen.generateGsTCPs ERROR:  we don't have enough distance to the next point = "+Fm0(ixNp1+1));
		return Triple<NavPoint,NavPoint,double>(np1b,np2,-1.0); // no change
	}
	//std::pair<Position,Velocity>  pv = KinematicsPosition::gsAccel(b.position(), vin, accelTime, a);
	double distToEGS = gsIn*accelTime + 0.5*a*accelTime*accelTime;
	std::pair<Position,int> pv = traj.advanceDistance(ixNp1,distToBGS+distToEGS,false);
	NavPoint e = NavPoint(pv.first, accelTime+t0+timeOffset); // .makeAdded();//.appendName(setName);
	return Triple<NavPoint,NavPoint,double>(b,e,distToEGS);
}

Plan TrajGen::generateGsTCPs(const Plan& fp, double gsAccel, bool repairGs, bool useOffset) {
	Plan traj(fp); // the current trajectory based on working
	for (int i = traj.size() - 2; i > 0; i--) {
		if (repairGs) {
			bool checkTCP = true;
			PlanUtil::fixGsAccelAt(traj, i, gsAccel, checkTCP, getMinTimeStep());
		}
		double timeOffset = 0.0;
		TcpData tcp1 = traj.getTcpData(i);
		if (useOffset && tcp1.isEOT() ) timeOffset = getMinTimeStep();
		double targetGs = traj.gsOut(i+1);
	    generateGsTCPsAt(traj, i, gsAccel, targetGs, timeOffset);
	}
	return traj;
}


void TrajGen::generateGsTCPsAt(Plan& traj, int i, double gsAccel, double targetGs, double timeOffset) {
	bool allowOverlap = true;
	generateGsTCPsAt(traj, i, gsAccel,  targetGs,   timeOffset, allowOverlap);
}

void TrajGen::generateGsTCPsAt(Plan& traj, int i, double gsAccel, double targetGs, double timeOffset, bool allowOverlap) {
	double gsIn = traj.gsIn(i);
	if (Util::almost_equals(gsIn,0.0) || Util::almost_equals(targetGs,0.0)) {
		traj.addWarning("TrajGen.generateGsTCPs: zero ground speed at index "+Fm0(i));
		gsIn = 1E-10;
	}
	//fpln(" ##$$ generateGsTCPs: at i = "+Fm0(i)+" Accelerate FROM gsIn = "+Units::str("kn",gsIn)+" TO targetGs = "+Units::str("kn",targetGs));
	int sign = 1;
	if (gsIn > targetGs) sign = -1;
	double a = std::abs(gsAccel)*sign;
	Triple<NavPoint,NavPoint,double> tcpTriple =  gsAccelGenerator(traj, i, gsIn, targetGs, a, timeOffset, allowOverlap);
	double distanceToEGS = tcpTriple.third;
	double distanceToEndOfPlan = traj.pathDistance(i,traj.size()-1);
	if (distanceToEGS > distanceToEndOfPlan) {
		traj.addError("TrajGen.generateGsTCPs ERROR: Cannot complete acceleration at i = "+Fm0(i)+" before end of Plan");
		return;
	}
	if (distanceToEGS >= Units::from("ft",50)) {
		NavPoint BGS = tcpTriple.first;
		NavPoint EGS = tcpTriple.second;
		TcpData tcp1 = traj.getTcpData(i);
		TcpData EGS_tcp = makeEGS(tcp1);
		if ( ! tcp1.isTCP()) {
			//fpln(" #### generateGsTCPs remove point at time "+np1.time()+" index="+traj.getIndex(np1.time()));
			traj.remove(i);  // assumes times have not changed from lpc to lpc
		}
		int ixEGS;
		int ixBGS;
		TcpData BGS_tcp = makeBGS(tcp1, a, timeOffset, tcp1.isAltPreserve());
		if (allowOverlap) {
	        bool isAltPreserve = tcp1.isAltPreserve();
			ixBGS = traj.add(BGS, BGS_tcp);
			if (ixBGS < 0) {
				//fpln(" $$##### generateGsTCPs: ixBGS = "+ixBGS+" BGS.t = "+BGS.time());
				traj.addError("TrajGen.generateGsTCPsAt ERROR: Overlap Problem at i = "+Fm0(i));
				return;
			}
			adjustGsInsideAccel(traj, ixBGS, a, distanceToEGS, timeOffset, isAltPreserve);
		} else {
			ixBGS = traj.add(BGS, BGS_tcp);
			traj.timeShiftPlan(ixBGS+1,1000);
		}
		ixEGS = traj.add(EGS, EGS_tcp);
		//f.pln(" $$##### generateGsTCPs:  i = "+i+" ixBGS = "+ixBGS+" ixEGS = "+ixEGS);
		traj.mkGsIn(ixEGS+1,targetGs);  // did not overlap fix gs between EGS and next point
	}
}

/** remove all points that will fall within distance to new EGS (distToEGS)
 *
 * @param traj       the plan file of interest
 * @param ixBGS      index of BGS
 * @param aBGS       acceleration value
 * @param distToEGS  distance from BGS to new EGS point
 */
void TrajGen::adjustGsInsideAccel(Plan& traj, int ixBGS, double aBGS, double distToEGS, double timeOffset, bool isAltPreserve) {
	//f.pln(" $$$$$ generateGsTCPsAt(1): aBGS = "+aBGS+" traj = "+traj.toStringGs());
	//f.pln(" $$$ adjustGsInsideAccel: distToEGS = "+Units.str("nm",distToEGS,8));
	double gsBGS = traj.gsIn(ixBGS);
	int lastIxInGsAccel = ixBGS;
	traj.setGsAccel(ixBGS,0.0);                         // *** THIS IS IMPORTANT --> isGsContinuous test needs this
	for (int ii = ixBGS+1; ii < traj.size(); ii++) {
		double dist_to_ii =  traj.pathDistance(ixBGS,ii);
		//f.pln(" $$$$$ adjustGsInsideAccel: ii = "+ii+" dist_to_ii = "+Units.str("nm",dist_to_ii,8));
		if (dist_to_ii < distToEGS) lastIxInGsAccel = ii;
		else break;
		if (!PlanUtil::isGsContinuous(traj, ii, Units::from("kn",5.0), true)) { // *** ACCELERATION NEEDS TO BE OFF HERE ****
			//f.pln("$$$$$ TrajGen.generateGsTCPsAt: passed over a ground speed discontinuity at "+ii);
			traj.addError("TrajGen.generateGsTCPsAt: passed over a ground speed discontinuity at "+Fm0(ii));
		}
	}
	//f.pln("\n $$$$$>>> adjustGsInsideAccel:  lastIxInGsAccel = "+lastIxInGsAccel);
	double tBGS = traj.time(ixBGS);
	traj.timeShiftPlan(ixBGS+1,1000);
	traj.setGsAccel(ixBGS,aBGS);               // **************** TURN ON ACCELERATION NOW *******************
	for (int j = ixBGS+1; j <= lastIxInGsAccel; j++) {
		double d_j = traj.pathDistance(ixBGS,j);
		double t_j = tBGS + Plan::timeFromDistance(gsBGS, aBGS, d_j);
		traj.setTime(j,t_j);
		//f.pln(" $$$$$>>> adjustGsInsideAccel: set time of j = "+j+" to "+t_j);
	}
}

TcpData TrajGen::makeBGS(const TcpData& tcp1, double a, double timeOffset, bool isAltPreserve) {
	TcpData BGS_tcp;
	//f.pln(" $$$$$ generateGsTCPsAt: BGS_tcp = "+BGS_tcp);
	if (timeOffset > 0)
		BGS_tcp = TcpData();
	else
		BGS_tcp = tcp1.copy();
	if (isAltPreserve) BGS_tcp.setAltPreserve();
	BGS_tcp.setSource(tcp1.getSourcePosition(),tcp1.getSourceTime());
	int linearIndex = tcp1.getLinearIndex();
	BGS_tcp.setBGS(a, linearIndex);
	return BGS_tcp;
}

TcpData TrajGen::makeEGS(const TcpData& tcp1) {
	TcpData EGS_tcp = TcpData();
	EGS_tcp.setSource(tcp1.getSourcePosition(),tcp1.getSourceTime());
	int linearIndex = tcp1.getLinearIndex();
	EGS_tcp.setEGS(linearIndex);
	return EGS_tcp;
}






Triple<double,double,double> TrajGen::vsAccelGenerator(double t1, double t2, double tLast, double vs1, double vs2, double a) {
	double deltaVs = vs1 - vs2;
	double accelTime = std::abs(deltaVs/a);
	double tbegin = 0;
	double tend = 0;
	if (accelTime > 0) {
		tbegin = t2 - accelTime/2.0;
		tend = tbegin + accelTime;
		if (tbegin < t1 || tend > tLast) {
			accelTime = -1;
		}
	}
	return Triple<double,double,double>(tbegin,tend,accelTime);
}


Triple<double,double,double> TrajGen::calcVsTimes(int i, Plan& traj, double vsAccel_d) {
	NavPoint np1 = traj.point(i-1);
	NavPoint np2 = traj.point(i);
	NavPoint np3 = traj.point(i+1);
	// find next point where vertical speed changes
	double nextVsChangeTm = traj.getLastTime();
	double targetVs = traj.vsOut(i);
	for (int j = i+1; j < traj.size(); j++) {
		//NavPoint np_j = traj.point(j);
		nextVsChangeTm = traj.time(j);
		double vs_j = traj.vsOut(j);
		double dt = Kinematics::vsAccelTime(vs_j, targetVs, vsAccel_d);
		if (dt > getMinTimeStep()) break;
		//if (std::abs(vs_j - targetVs) > Units::from("fpm",10)) break;
	}
	double vs1 = np1.verticalSpeed(np2);
	double vs2 = np2.verticalSpeed(np3);
	int sign = 1;
	if (vs1 > vs2) sign = -1;
	double a = vsAccel_d*sign;
	double prevEndTime = traj.getFirstTime();
	int ixEVS =  traj.prevEVS(i);
	if (ixEVS >= 0) prevEndTime = Util::max(prevEndTime,traj.time(ixEVS));
	Triple<double,double,double> vsTriple = vsAccelGenerator(prevEndTime, np2.time(), nextVsChangeTm, vs1, vs2, a);
	double accelTime = vsTriple.third;
	if (accelTime < 0) {
		// we have a calculation error, possibly too short a time for the needed accleration
		traj.addError("TrajGen::generateVsTCPs ERROR: Insufficient room i = "+Fm0(i)+" for vertical accel! tbegin = "+Fm1(vsTriple.first)+" prevEndTime = "+Fm1(prevEndTime));
		printError("TrajGen::generateVsTCPs ERROR: Insufficient room at i = "+Fm0(i)+" for vertical accel! tbegin = "+Fm1(vsTriple.first)+" prevEndTime = "+Fm1(prevEndTime));
		return Triple<double,double,double>(-1.0,-1.0,-1.0);
	} else if (accelTime > MIN_ACCEL_TIME) {
		return Triple<double,double,double>(vsTriple.first,vsTriple.second,a);
	} else { //if (vsTriple.third > getMinTimeStep()*minorVfactor) {
		// we have an accel time too small for TCP generation
		return Triple<double,double,double>(-1.0,-1.0,-1.0);
	}
}

Plan TrajGen::generateVsTCPs(const Plan& kpc, double vsAccel_d, bool continueGen) {
	Plan traj(kpc);
	for (int i = 1; i < traj.size()-1; i++) {
		NavPoint np1 = traj.point(i-1);
		NavPoint np2 = traj.point(i);
		double vs1 = np1.verticalSpeed(np2);
		Triple<double,double,double> cboTrip = TrajGen::calcVsTimes(i, traj, vsAccel_d);
		double tbegin = cboTrip.first;
		double tend = cboTrip.second;
		double accel = cboTrip.third;
		//fpln(" $$$$ generateVsTCPs: for i = "+Fm0(i)+" tbegin = "+Fm2(tbegin)+" tend = "+Fm2(tend));
		bool newTCPneeded = (tbegin >= 0 && tend >= 0 && !traj.inVsChange(tbegin));
		if (traj.inVsChange(tbegin) && tbegin >=0 ) {     // TODO: probably should fail generation in this case !!!
			//fpln(i+" $$!!!!!!!!!!!!! "+kpc.getName()+" generateVsTCPs:  traj.inVsChange("+f.Fm2(tbegin)+") = "+traj.inVsChange(tbegin)+"   traj.inVsChange(tend) = "+traj.inVsChange(tend));
			if (continueGen) {
				continue;
			} else {
				traj.addError("Vertical Speed regions overlap at time "+Fm2(tbegin));
				return traj;
			}
		}
		if (tbegin > 0 && np1.time() > tbegin && traj.isEVS(i-1)) {   // see test "test_AWE918"
			if (continueGen) {
				continue;
			} else {
				traj.addError("Vertical Speed regions overlap at time "+Fm2(tbegin));
				return traj;
			}
		}
		if (newTCPneeded) {
			std::string label = np2.label();
			NavPoint np2Source = traj.sourceNavPoint(i);
			int linearIndex = traj.getTcpData(i).getLinearIndex();
			Velocity vin = traj.velocity(tbegin);
			std::pair<NavPoint,TcpData> bPair = Plan::makeBVS(np2Source,traj.position(tbegin), tbegin, accel, linearIndex);
			NavPoint b = bPair.first.makeLabel(label); // .makeAdded();//.appendName(setName);
			//fpln(" $$$$$$$$$$$$ generateVsTCPs: i = "+Fm0(i)+" make BVSC   point="+b.toString()+" vin = "+vin.toString());
			std::pair<NavPoint,TcpData> ePair = Plan::makeEVS(np2Source,traj.position(tend), tend, linearIndex);
			NavPoint e = ePair.first;
			if ( ! traj.isTCP(i)) {
				traj.remove(i);
			}
			int bindex = traj.add(b,bPair.second);
			int eindex = traj.add(e,ePair.second);
			if (bindex < 0) {
				bindex = -bindex;
			}
			if (eindex < 0) {
				eindex = -eindex;
			}
			i = eindex;
			// fix altitude for all remaining points between b and e
			bool altok = true;
			for (int k = bindex+1; k < eindex; k++) {
				double dt2 = traj.time(k) - tbegin;
				std::pair<double,double> npair = vsAccel(b.alt(), vs1, dt2, accel);
				double newAlt = npair.first;
				//f.pln(" $$$$ k = "+k+"  dt2 = "+dt2+"  newAlt = "+Units.str("ft",newAlt));
				NavPoint newNp = traj.point(k).mkAlt(newAlt);
				TcpData newNp_tcp = traj.getTcpData(k);
				//fpln(" $$ generateVsTCPs: traj.point(k) = "+traj.point(k).toStringFull());
				if (newAlt >= 0 && newAlt <= maxAlt) {
					traj.set(k, newNp, newNp_tcp);
				} else {
					altok = false;
				}
			}//for
			if (!altok) traj.addError("TrajGen.generateVsTCPs: generated altitude is out of bounds");
		}
	} // for loop
	//fpln(" generateVsTCPs: END traj.hasError() = "+Fmb(traj.hasError()));
	return traj;
} // generateVsTCPs



std::pair<double,double> TrajGen::vsAccel(double sz, double vz,  double t, double a) {
	double nvz = vz + a*t;
	double nsz = sz + vz*t + 0.5*a*t*t;
	return std::pair<double,double>(nsz,nvz);
}



Plan TrajGen::removeShortFirstLeg(Plan& fp, double bank)  {
	//fpln(" #### removeShortFirstLeg: this = "+this);
	if (fp.size() < 3) return fp;
	int j = 0;
	NavPoint p0 = fp.point(j);
	NavPoint p1 = fp.point(j+1);
	int linearIndex = fp.getTcpData(j+1).getLinearIndex();
	NavPoint p2 = fp.point(j+2);
	Velocity vf0 = fp.finalVelocity(j);
	Velocity vi1 = fp.initialVelocity(j+1);
	double deltaTrack1 = std::abs(vf0.trk() - vi1.trk());
	if (deltaTrack1> Units::from("deg",1.0)) {
		double gs1 = fp.initialVelocity(j).gs();
		double R1 = Kinematics::turnRadius(gs1,bank);
		Tuple5<NavPoint,NavPoint,NavPoint,int,Position> turn1 = TurnGeneration::turnGenerator(p0, p1, linearIndex, p2, R1);
		NavPoint BOT = turn1.first;
		NavPoint EOT = turn1.third;
		//fpln(" #### removeShortFirstLeg: BOT = "+BOT+" EOT = "+EOT);
		if (BOT.time() < p0.time()) {
			fp.remove(1);
			//fpln(" ##### removeShortFirstLeg: REMOVED SECOND POINT");
		}
	}
	return fp;
}


Plan TrajGen::removeShortLastLeg(Plan& fp, double bank)  {
	//fpln(" #### removeShortLastLeg: this = "+this);
	if (fp.size() < 3) return fp;
	int j = fp.size()-3;
	NavPoint p0 = fp.point(j);
	NavPoint p1 = fp.point(j+1);
	int linearIndex = fp.getTcpData(j+1).getLinearIndex();
	NavPoint p2 = fp.point(j+2);
	Velocity vf0 = fp.finalVelocity(j);
	Velocity vi1 = fp.initialVelocity(j+1);
	//fpln("#### removeShortLastLeg:  j = "+j+" vf0 = "+vf0+" vi1 = "+vi1);
	double deltaTrack1 = std::abs(vf0.trk() - vi1.trk());
	//fpln(" #### removeShortLastLeg: deltaTrack1 = "+deltaTrack1);
	if (deltaTrack1 > Units::from("deg",1.0)) {
		double gs1 = fp.initialVelocity(j).gs();
		double R1 = Kinematics::turnRadius(gs1,bank);
		Tuple5<NavPoint,NavPoint,NavPoint,int,Position> turn1 = TurnGeneration::turnGenerator(p0, p1, linearIndex, p2, R1);
		NavPoint BOT = turn1.first;
		NavPoint EOT = turn1.third;
		if (p1.distanceH(p2) < p1.distanceH(EOT)) {
			fp.remove(j+1);
			//fpln(" ##### removeShortLastLeg: REMOVED LAST POINT");
		}
	}
	return fp;
}

Plan TrajGen::linearRepairShortTurnLegs(const Plan& fp, double bank, bool addMiddle) {
	Plan npc = Plan(fp);  //fp.copy();
	for (int j = 0; j+3 < fp.size(); j++) {
		NavPoint p0 = fp.point(j);
		NavPoint p1 = fp.point(j+1);
		NavPoint p2 = fp.point(j+2);
		NavPoint p3 = fp.point(j+3);
		Velocity vf0 = fp.finalVelocity(j);
		Velocity vi1 = fp.initialVelocity(j+1);
		Velocity vf1 = fp.finalVelocity(j+1);
		Velocity vi2 = fp.initialVelocity(j+2);
		double deltaTrack1 = std::abs(vf0.trk() - vi1.trk());
		double deltaTrack2 = std::abs(vf1.trk() - vi2.trk());
		//fpln(j+"#### removeShortLegsBetween: deltaTrack1 = "+Units::str("deg",deltaTrack1)+" deltaTrack2 = "+Units::str("deg",deltaTrack2));
		if (deltaTrack1> Units::from("deg",1.0) && deltaTrack2 > Units::from("deg",1.0)) {
			double gs1 = fp.initialVelocity(j).gs();
			double R1 = Kinematics::turnRadius(gs1,bank);
			double gs2 = fp.initialVelocity(j+1).gs();
			double R2 = Kinematics::turnRadius(gs2,bank);
			int linearIndex1 = fp.getTcpData(j+1).getLinearIndex();
			int linearIndex2 = fp.getTcpData(j+2).getLinearIndex();
			Tuple5<NavPoint,NavPoint,NavPoint,int,Position> turn1 = TurnGeneration::turnGenerator(p0, p1, linearIndex1, p2, R1);
			Tuple5<NavPoint,NavPoint,NavPoint,int,Position> turn2 = TurnGeneration::turnGenerator(p1, p2, linearIndex2, p3, R2);  // should this use R1 ~ gs1 ??
			NavPoint A = turn1.third;
			NavPoint B = turn2.first;
			double distP1EOT = p1.distanceH(A);
			double distP1BOT = p1.distanceH(B);
			//fpln(j+" ######### removeShortLegsBetween: distP1BOT = "+Units::str("nm",distP1BOT)+" distP1EOT = "+Units::str("nm",distP1EOT));
			if (A.time() > B.time() || distP1EOT > distP1BOT ) {
				//fpln("............removeShortLegsBetween REMOVE npc.point(j+2) = "+npc.point(j+2)+ " npc.point(j+1) = "+npc.point(j+1));
				npc.remove(j+2);
				npc.remove(j+1);
				if (addMiddle) {
					Position mid = p1.position().midPoint(p2.position());
					double tmid =npc.time(j)+  mid.distanceH(p0.position())/gs1;
					NavPoint midNP = p1.makePosition(mid).makeTime(tmid); // preserve source info
					// fpln(" $$ ADD removeShortLegsBetween: midNP = "+midNP);
					npc.addNavPoint(midNP);
					npc =  PlanUtil::linearMakeGSConstant(npc, j,j+2,gs1);
				} else {
					double dist = p3.distanceH(p0);
					double dt = dist/gs1;
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



Plan TrajGen::removeInfeasibleTurns(const Plan& fp, double bankAngle, bool strict) {
	Plan traj = Plan(fp); //fp.copy(); // the current trajectory based on working
	//int j = 1; // traj index corresponding to i in fp
	for (int i = fp.size() - 2; i > 0; i--) {
		//double tm = fp.getTime(i);
		Velocity vf0 = fp.finalVelocity(i-1);
		Velocity vi1 = fp.initialVelocity(i);
		if (Util::almost_equals(vi1.gs(),0.0)) {
			fpln(" ###### WARNING: Segment from point "+Fm0(i)+" to point "+Fm0(i+1)+" has zero ground speed!");
		}
		if (std::abs(vf0.trk() - vi1.trk()) > Units::from("deg",1.0) && bankAngle >= 0.0) {
			NavPoint np1 = fp.point(i-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
			NavPoint np2 = fp.point(i);
			NavPoint np3 = fp.point(i + 1);
			double gs = vf0.gs();
			double R = Kinematics::turnRadius(gs, bankAngle);
			//fpln("$$$ generateTurnTCPs: t="+np2.time()+"   gs="+Units::to("knot", gs)+"   R = "+Units::str8("nm",R));
			int linearIndex = fp.getTcpData(i).getLinearIndex();
			Tuple5<NavPoint,NavPoint,NavPoint,int,Position> tg = TurnGeneration::turnGenerator(np1,np2,linearIndex,np3,R);
			NavPoint BOT = tg.getFirst();
			NavPoint MOT = tg.getSecond();
			NavPoint EOT = tg.getThird();
			// Calculate Altitudes based on fp
			Position lBOT = fp.position(BOT.time());
			Position lMOT = fp.position(MOT.time());
			double tEOT = EOT.time();
			if (tEOT < fp.getLastTime()) {
				Position lEOT = fp.position(tEOT);
				BOT = BOT.mkAlt(lBOT.alt());
				MOT = MOT.mkAlt(lMOT.alt());
				EOT = EOT.mkAlt(lEOT.alt());
				if (!turnIsFeas(fp, i, BOT, EOT)) {
					traj.remove(i);
				}
			} else {
				traj.remove(i);
			}
		}
	}//for
	return traj;
}

/**
 * if there is a segment with a ground speed change at point j that cannot be achieved with the gsAccel value, then it
 * makes the ground speed before and after j the same (i.e. averages over two segments)
 *
 * @param fp
 * @param gsAccel
 * @return
 */
Plan TrajGen::linearRepairShortGsLegs(const Plan& fp, double gsAccel) {
	Plan lpc = Plan(fp);  //fp.copy();
	//fpln(" $$ smoothShortGsLegs: BEFORE npc = "+lpc.toString());
	for (int j = 1; j < fp.size()-1; j++) {
		Velocity vin = lpc.finalVelocity(j-1);
		double	targetGs = lpc.initialVelocity(j).gs();
		int sign = 1;
		double gs1 = vin.gs();
		//double gs2 = np1.initialVelocity(np2).gs(); // target Gs
		if (gs1 > targetGs) sign = -1;
		double a = std::abs(gsAccel)*sign;
		double accelTime = (targetGs - gs1)/a;
		//f.p(j+" $$$$$$ smoothShortGsLegs: accelTime = "+accelTime+"  dt = "+f.Fm2(dt));
		//fpln(" gs1 = "+Units::str("kn",gs1)+"  targetGs = "+Units::str("kn",targetGs));
		double d = lpc.pathDistance(j-1,j) - targetGs*getMinTimeStep();
		double remainingDist = d - accelTime * (gs1+targetGs)/2;
		//fpln("$$$ smoothShortGsLegs: remainingDist = "+Units::str("nm",remainingDist));
		if (accelTime > getMinTimeStep() && remainingDist <= 0) {
			printError(Fm0(j)+" smoothShortGsLegs: REPAIRED GS ACCEL PROBLEM AT "+Fm2(lpc.point(j).time()));
			lpc = PlanUtil::linearMakeGSConstant(lpc, j-1,j+1);
		}
	}
	//fpln(" $$ smoothShortGsLegs: AFTER npc = "+lpc.toString());
	return lpc;
}


Plan TrajGen::linearRepairShortVsLegs(const Plan& fp, double vsAccel) {
	if (!fp.isLinear()) {
		fp.addError("TrajGen.smoothShortVsLegs should only be called on linear plans");
		return fp;
	}
	Plan lpc = Plan(fp); //fp.copy();
	for (int i = 1; i < fp.size()-1; i++) {
		Triple<double,double,double> cboTrip = calcVsTimes(i, lpc, vsAccel);
		double tbegin = cboTrip.first;
		double tend = cboTrip.second;
		double accel = cboTrip.third;
		bool repairNeeded = false;
		if ((tbegin < 0.0 || tend < 0.0) && accel != 0.0) repairNeeded = true;
        if (repairNeeded || (tbegin >= 0.0 && tbegin <= lpc.getFirstTime()) || tend >= lpc.getLastTime() ) {
    		//fpln(" $$ linearRepairShortVsLegs: BEFORE  call to linearMakeVsConstant, lpc = "+lpc.toString());
			//fpln(" ###### tbegin = "+tbegin+" tend = "+tend+" CALL linearMakeVsConstant AT i =  "+i);
        	PlanUtil::linearMakeVsConstant(lpc, i-1,i+1);
        }
	}
	//fpln(" $$ linearRepairShortVsLegs: AFTER CALL, RETURN lpc = "+lpc.toString());
	return lpc;
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
//			if (currFixedAltTcp.isTCP()) {
//				//TcpData currFixedAlt_tcp = traj.getTcpData(i);
//				currFixedAltTcp = currFixedAltTcp.setVelocityInit(traj.initialVelocity(i).mkVs(constantVs));  // TODO: does this do anything??
//			}
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

Plan TrajGen::genDirectTo(const Plan& fp, const Position& so, const Velocity& vo, double to,
		double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn) {
	Plan lpc = fp;
	if (!fp.isLinear()) {
		lpc = PlanUtil::revertTCPs(fp);
	} else {
		lpc = (Plan) fp;
	}
	Plan lpc2 = TrajGen::genDirectToLinear(lpc,so,vo,to,bankAngle,timeBeforeTurn);
	//fpln(" $$$ genDirectTo: lpc2.initialVelocity(1) = "+lpc2.initialVelocity(1));
	//fpln("$$$ +kpc.initialVelocity(3)  = "+kpc.initialVelocity(3));
	Plan kpc = TrajGen::makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, false, true, true);
	//bool twoTurnOverlap = std::abs(lpc2.initialVelocity(1).compassAngle() -  kpc.initialVelocity(3).compassAngle()) > Units::from("deg",10.0);
	bool twoTurnOverlap = Util::turnDelta(lpc2.trkOut(1),kpc.trkOut(3)) > Units::from("deg",10.0);

	if (twoTurnOverlap) {
		//printError(" $$$ genDirectTo: Turns Overlap");
		kpc.addError("TrajGen.genDirectTo: Turns Overlap",1);
	}
	if (lpc2.hasError()) {
		//fpln(" $$$$$$$$$ genDirectTo: GENERATION ERROR: "+lpc2.getMessage());
		kpc.addError(lpc2.getMessage(),0);
	}
	return kpc;
}

//Plan TrajGen::genDirectToRetry(const Plan& p, const Position& so, const Velocity& vo, double to,
//		double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn, double timeIntervalNextTry) {
//	double minVsChangeRecognized = vsAccel*getMinTimeStep();
//	return genDirectToRetry(p, so, vo, to, bankAngle, gsAccel, vsAccel, timeBeforeTurn,timeIntervalNextTry);
//}

Plan TrajGen::genDirectToRetry(const Plan& p, const Position& so, const Velocity& vo, double to,
		double bankAngle, double gsAccel, double vsAccel,  double timeBeforeTurn, double timeIntervalNextTry) {
	Plan fp = p;
	bool done = false;
	Plan kpc;
	do {
		kpc = TrajGen::genDirectTo(fp,so,vo,to,bankAngle,gsAccel,vsAccel,  timeBeforeTurn);
		//fpln(" $$$$ generate GenDirectTo !!!! kpc.size() = "+kpc.size()+" kpc.hasError() = "+kpc.hasError());
		if (kpc.hasError() && fp.size() > 1) {
			double tm0 = fp.point(0).time();
			double tm1 = fp.point(1).time();
			if (tm0 + timeIntervalNextTry + 20 < tm1) { // add connection point
				Position connectPt = fp.position(tm0+timeIntervalNextTry);
				//fpln(" $$$$ Add connectPt = "+connectPt+" at time "+(tm0+timeIntervalNextTry));
				fp.addNavPoint(NavPoint(connectPt,tm0+timeIntervalNextTry));
				fp.remove(0);
			} else {
				fp.remove(0);
			}
		} else {
			done = true;
		}
	} while (!done);

	// 	     PlanCore lpc2 = TrajGen::genDirectToLinear((PlanCore) fp,so,vo,to,bankAngle,timeBeforeTurn);
	//   		 kpc = TrajGen::makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, true);

	return kpc;
}

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
	double bestTime = base.getLastTime();
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
					bestTime = tt;
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
	return PlanUtil::revertTCPs(fp);
}

//// only add a point it it will not overwrite an existing one!
//void TrajGen::safeAddPoint(Plan& p, const NavPoint& n) {
//	if (p.overlaps(n) < 0) p.add(n);
//}



///**
// *
// * Returns a new Plan that sets all points in a range to have a constant GS.
// * */
//Plan TrajGen::makeGSConstant(const Plan& p, double newGs,  double bankAngle, double gsAccel, double vsAccel,
//		bool repair) {
//	Plan kpc = revertTCPs(p);
//	kpc = linearMakeGSConstant(kpc,newGs);
//	if (!p.isLinear()) {
//		kpc = makeKinematicPlan(kpc,  bankAngle, gsAccel, vsAccel, repair);
//	}
//	return kpc;
//}
//
///**
// *
// * Returns a new Plan that sets all points in a range to have a constant GS.
// * */
//Plan TrajGen::makeGSConstant(const Plan& p, double bankAngle, double gsAccel, double vsAccel,
//		bool repair) {
//	Plan kpc = revertTCPs(p);
//	kpc = linearMakeGSConstant(kpc);
//	if (!p.isLinear()) {
//		kpc = makeKinematicPlan(kpc,  bankAngle, gsAccel, vsAccel, repair);
//	}
//	return kpc;
//}


void TrajGen::setVerbose(bool b) {
	verbose = b;
}




}

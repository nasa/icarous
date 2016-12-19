/* Trajectory Generation
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Plan.h"
#include "PlanUtil.h"
//#include "UnitSymbols.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
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
const double TrajGen::MIN_VS_CHANGE = Units::from("fpm",100);
const double TrajGen::MIN_VS_TIME = Units::from("s",30);


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
			Plan kpc3 = generateGsTCPs(kpc2, gsAccel, repairGs);
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
					//fpln(" generateTCPs: generateVsTCPs ----------------------------------- "+kpc4.isWellFormed());
					Plan kpc5 = generateVsTCPs(kpc4, vsAccel);
					//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
					cleanPlan(kpc5);
					//fpln(" generateTCPs: DONE ----------------------------------- "+kpc5.isWellFormed());
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
		fp.set(i,fp.point(i).makeOriginal());
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

bool TrajGen::turnIsFeas(const Plan& lpc, int i, const NavPoint& BOT, const NavPoint& EOT, bool strict) {
	NavPoint np2 = lpc.point(i);
	double d1new = BOT.distanceH(np2);
	double d2new = np2.distanceH(EOT);
	int ipTC = i-1;
	int inTC = i+1;
	if (! strict) {
	    ipTC = prevTrackChange(lpc,i);
	    inTC = nextTrackChange(lpc,i);
	}
	double d1old = lpc.point(ipTC).distanceH(np2);
	double d2old = np2.distanceH(lpc.point(inTC));
	bool rtn = true;
	//fpln(" $$$$ generateTurnTCPs: "+(BOT.time()-getMinTimeStep())+" "+fp.getTime(i-1)+" "+EOT.time()+getMinTimeStep()+" "+fp.getTime(i+1));
	if (BOT.time()-getMinTimeStep() <= lpc.getTime(ipTC) || EOT.time()+getMinTimeStep() >= lpc.getTime(inTC)
			|| d1new+Constants::get_horizontal_accuracy() >= d1old || d2new+Constants::get_horizontal_accuracy() >= d2old) {
		//trajCore(lpc); // revert to initial plan
		//traj.addError("ERROR in TrajGen::generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.",i);
		//printError("ERROR in TrajGen::generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.");
		if (verbose) {
			fpln("####### i = "+Fm0(i)+" ipTC = "+Fm0(ipTC)+" inTC = "+Fm0(inTC)+" np2 =  "+np2.toStringFull());
			fpln("####### BOT.time() = "+Fm2(BOT.time())+" <? fp.getTime(ipTC) = "+Fm2(lpc.getTime(ipTC)));
			fpln("####### EOT.time() = "+Fm2(EOT.time())+" >? fp.getTime(inTC) = "+Fm2(lpc.getTime(inTC)));
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
	kpc.add(lpc.point(0)); // first point need not be marked
	for (int i = 1; i < lpc.size(); i++) {
		NavPoint np = lpc.point(i);
		double vs1 = lpc.vsOut(i-1);
		double vs2 = lpc.vsOut(i);
		if (std::abs(vs1) > maxVs) {
			kpc.addWarning("Input File has vertical speed exceeding "+Units::str("fpm",maxVs)+" at "+Fm0(i-1));
		}
		double deltaTm = 0;
		if (i+1 < lpc.size()) deltaTm = lpc.getTime(i+1) - lpc.getTime(i);
		if (! Util::within_epsilon(vs1, vs2, MIN_VS_CHANGE) || (deltaTm >= MIN_VS_TIME)) {
			//fpln("markVsChanges "+i+" marked ALT PRESERVE");
			kpc.add(np.makeAltPreserve());
		} else {
			kpc.add(np);
		}
	}
	//		kpc.add(lpc.point(lpc.size()-1)); // last point need not be marked
	return kpc;
}

Plan TrajGen::generateTurnTCPs(const Plan& lpc, double bankAngle, bool continueGen, bool strict) {
	//fpln("$$$ generateTurnTCPs: bankAngle = "+Units::str("deg",bankAngle));
	Plan traj(lpc); // the current trajectory based on working
	double bank = std::abs(bankAngle);
	// for (int i = 1; i+1 < lpc.size(); i++) {
	for (int i = lpc.size()-2; i > 0; i--) {
		NavPoint np2 = lpc.point(i);
		if (lpc.inAccelZone(i)) continue;     // skip over regions already generated (for now only BOT-EOT pairs)
		Velocity vf0 = lpc.finalVelocity(i-1);
		Velocity vi1 = lpc.initialVelocity(i);
		if (Util::almost_equals(vi1.gs(),0.0)) {
			printError(" ###### WARNING: Segment from point "+Fm0(i)+" to point "+Fm0(i+1)+" has zero ground speed!");
		}
		double turnDelta = Util::turnDelta(vf0.trk(), vi1.trk());
		double gsIn = vf0.gs();
		double turnTime;
		double R = np2.turnRadius();
		if (Util::almost_equals(R, 0.0)) {
			if (bank == 0) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: specified bank angle is 0");
				bank = Units::from("deg",0.001); // prevent divisions by 0.0
			}
		   R = Kinematics::turnRadius(gsIn, bank);
		   turnTime = Kinematics::turnTime(vf0.gs(),turnDelta,bank);
		} else {
		   turnTime =  turnDelta*R/gsIn;
		}
		if (turnTime >= getMinTimeStep()) {
			NavPoint np1 = lpc.point(i-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
			NavPoint np3 = lpc.point(i + 1);
			if (np3.time() - np2.time() < 0.1 && i+2 < lpc.size()) {
				np3 = lpc.point(i + 2);
			}
			Triple<NavPoint,NavPoint,NavPoint> tg = TurnGeneration::turnGenerator(np1,np2,np3,R);		 // $$$$$$$$$$$$$$$$ RWB NEW
			insertTurnTcpsInTraj(traj, lpc, i, np1, np2, np3, R, strict);
			if (traj.hasError() && ! continueGen) return traj;
		}
	}
	return traj;
}

Plan TrajGen::generateTurnTCPs(const Plan& lpc, double bankAngle) {
	bool continueGen = false;
	bool strict = false;
	return generateTurnTCPs(lpc,bankAngle,continueGen,strict);
}


void TrajGen::insertTurnTcpsInTraj(Plan& traj, const Plan& lpc, int i, const NavPoint& np1, const NavPoint& np2, const NavPoint& np3, double R, bool strict) {
	Velocity vf0 = lpc.finalVelocity(i-1);
	Velocity vi1 = lpc.initialVelocity(i);
	double gsIn = vf0.gs();
	//double targetGS = vi1.gs();
	Triple<NavPoint,NavPoint,NavPoint> tg = TurnGeneration::turnGenerator(np1,np2,np3,R);		 // $$$$$$$$$$$$$$$$ RWB NEW
    //fpln(" $$ generateTurnTCPs: tg.1="+tg.first.toString()+" tg.2="+tg.second.toString()+" tg.3="+tg.third.toString());
	NavPoint BOT = tg.getFirst(); //.appendName(setName);
	NavPoint MOT = tg.getSecond();//.appendName(setName);
	NavPoint EOT = tg.getThird(); //.appendName(setName);
	if (i > 1 && BOT.time() < np1.time()) { //only okay if ground speeds did not change, see test testJBU173
		double gsInAtNp1 = traj.gsFinal(i-2);
		double gsOutAtNp1 = traj.gsOut(i-1);
		if (std::abs(gsOutAtNp1 - gsInAtNp1) > Units::from("kn",10.0)) {
			//f.pln(" $$$ generateTurnTCPs: gsInAtNp1 = "+Units.str("kn",gsInAtNp1)+" gsOutAtNp1 = "+Units.str("kn",gsOutAtNp1));
			traj.addError("ERROR in TrajGen.generateTurnTCPs: ground speed change in turn "+Fm0(i)+" at time "+Fm1(np1.time()),i);
			return; // traj;

		}
	}
	// Calculate Al
	double botAlt;
	if (BOT.time() < lpc.getFirstTime()) {  // is this right?
		botAlt = lpc.point(0).alt();
	} else {
		botAlt = lpc.position(BOT.time()).alt();
	}
	BOT = BOT.mkAlt(botAlt);
	MOT = MOT.mkAlt(lpc.position(MOT.time()).alt());
	double EotAlt;
	if (EOT.time() > lpc.getLastTime()) {  // TODO: is this right?
		EotAlt = lpc.getLastPoint().alt();
	} else {
		EotAlt = lpc.position(EOT.time()).alt();
	}
	EOT = EOT.mkAlt(EotAlt);
	//bank = bank - Units::from("deg",0.5);
	if (BOT.isInvalid() || MOT.isInvalid() || EOT.isInvalid()) {
		traj.addError("ERROR in TrajGen.generateTurnTCPs: turn points at "+Fm0(i)+" invalid.",i);
		printError("ERROR in TrajGen.generateTurnTCPs: turn points at "+Fm0(i)+" invalid. "+Fm0(i));
		//fpln(" #### BOT = "+BOT);
		//f.pln(" #### MOT = "+MOT);
		//f.pln(" #### EOT = "+EOT);
		return; // traj;
	}
	if (!turnIsFeas(lpc, i, BOT, EOT, strict)) {
		//trajCore(lpc); // revert to initial plan
		traj.addError("ERROR in TrajGen::generateTurnTCPs: cannot achieve turn "+Fm0(i)+" from surrounding points.",i);
		printError("ERROR in TrajGen::generateTurnTCPs: cannot achieve turn "+Fm0(i)+" from surrounding points. "+Fm0(i));
		//fpln(" traj = "+traj);
		//DebugSupport.dumpPlan(traj, "generateTurnTCPs_ERROR "+i);
		return; // traj;
	} else 	if (EOT.time() - BOT.time() > 2*getMinTimeStep()) { // ignore small changes
		std::string label = "";
		if ( np2.label() != "" ) label = " (" +np2.label()+ ") ";
		if (traj.inTrkChange(BOT.time())) {
			traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn in time at "+Fm0(i),i);
			printError("ERROR in TrajGen::generateTurnTCPs: BOT in new turn overlaps existing turn int time "+Fm0(i));
			return; // traj;
		}	else if (traj.inTrkChange(EOT.time())) {
			traj.addError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn in time at "+Fm0(i),i);
			printError("ERROR in TrajGen::generateTurnTCPs: EOT in new turn overlaps existing turn int time "+Fm0(i));
			return; // traj;
		}
		//fpln("Creating turn at "+np2.time());
		//fpln(" $$$ generateTurnTCPs: traj.point("+i+").toOutput() =  "+traj.point(i).toOutput());
		int jj_MOT = i; // traj.getIndex(lpc.point(i).time());   // depends upon reverse order for loop
		if (traj.point(jj_MOT).isAltPreserve()) {
			MOT = MOT.makeAltPreserve();
		}
		if (!lpc.point(i).isTCP()) {
			//fpln(" $$$ generateTurnTCPs: lpc.point("+i+").time() = "+lpc.point(i).time()+" jj = "+jj);

			traj.remove(jj_MOT);
		}
		int ixBOT = traj.add(BOT);
		int ixMOT = traj.add(MOT);
		int ixEOT = traj.add(EOT);
		if (ixBOT < 0 || ixMOT < 0 || ixEOT < 0 ) {
			//f.pln("\n\n $$$$$$ TrajGen.generateTurnTCPs: NEGATIVE! ixBOT = "+ixBOT+" ixMOT = "+ixMOT+" ixEOT = "+ixEOT);
			traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT/EOT overlaps point ");
			return; // traj;
		}
		//fpln(" $$$ generateTurnTCPs: BOT = "+BOT.toString(8)+" MOT = "+MOT.toString(8)+" EOT = "+EOT.toString(8));
		movePointsWithinTurn(traj,ixMOT);
		//traj.timeshiftPlan(j+3, timeShift);

		double courseInBOT = traj.finalVelocity(ixBOT-1).compassAngle();
		double courseOutBOT = traj.initialVelocity(ixBOT).compassAngle();
		if (Util::turnDelta(courseInBOT,courseOutBOT) > M_PI/10.0) {
			traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into BOT not equal to track out of BOT at "+Fm0(ixBOT));
			return; // traj;
		}
		double courseInEOT = traj.finalVelocity(ixEOT-1).compassAngle();
		double courseOutEOT = traj.initialVelocity(ixEOT).compassAngle();
		//f.pln(" $$>> generateTurnTCPs: courseIn = "+Units.str("deg",courseIn)+" courseOut = "+Units.str("deg",courseOut));
        double turnDeltaEOT = Util::turnDelta(courseInEOT,courseOutEOT);
		if (turnDeltaEOT >  M_PI/10.0) {  // See test case "test_SWA2013" and T002, T026
	        //fpln(" $$>> generateTurnTCPs: turnDeltaEOT = "+Units.str("deg",turnDeltaEOT));
			traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into EOT not equal to track out of EOT at "+Fm0(ixEOT)+label);
			return; // traj;
		}
		//f.pln("  $$$ generateTurnTCPs: gsIn = "+Units.str("kn",gsIn));
		traj.mkGsInto(ixBOT, gsIn);
		//f.pln(" $$>> traj = "+traj.toStringGs());
		double targetGS = vi1.gs();
		traj.mkGsInto(ixEOT+1, targetGS);
	}
}


void TrajGen::movePointsWithinTurn(Plan& traj, int ixMOT) {
	//fpln(" ## movePointsWithinTurn: ------------------------------------ ixMOT = "+ixMOT+" traj = "+traj);
	int ipTC = traj.prevBOT(ixMOT);//fixed
	int inTC = traj.nextEOT(ixMOT);//fixed
	//fpln(" ## movePointsWithinTurn: ixMOT = "+ixMOT+" ipTC =  "+ipTC+" inTC = "+inTC);
	//for (int i = inTC-1; i > ipTC; i--) {            // delete in reverse order to preserve index
	for (int i = ipTC+1; i < inTC; i++) {            // delete in reverse order to preserve index
		if (i == ixMOT) continue;
		NavPoint npi = traj.point(i);
		//fpln(" ## movePointsWithinTurn: npi("+i+") = "+npi.toStringFull());
		traj.remove(i);
		double iTm = npi.time();
		Position iPos = traj.position(iTm).mkAlt(npi.alt());
		//fpln(" movePointsWithinTurn: move point at i = "+i+" iTm = "+iTm+" to iPos = "+iPos);
		npi = NavPoint(iPos,iTm).makeMovedFrom(npi);
		//fpln(" movePointsWithinTurn: npi("+Fm0(i)+") = "+npi.toStringFull());
		traj.add(npi);
		if (npi.isBVS()) {
			NavPoint np = npi.makeVelocityInit(traj.finalVelocity(i-1));
			traj.set(i,np);
		}
	}
}

// create GSC TCPS between np1 and np2 using vin from previous leg
/**
 * @param np1 start point of the acceleration segment/where the speed change occurs
 * @param np2 end point of the acceleration segment
 * @param targetGs target gs
 * @param vin velocity in
 * @param gsAccel non negative (horizontal) acceleration
 * @return third component: accel time, or negative if not feasible
 * NOTE!!! This has been changed so the accel time is always returned, but if it is less than getMinTimeStep(), there is no region and this has to be handled by the calling program!!!
 */
Triple<NavPoint,NavPoint,double> TrajGen::gsAccelGenerator(const NavPoint& np1, const NavPoint& np2, double targetGs, const Velocity& vin, double gsAccel) {
	int sign = 1;
	double gs1 = vin.gs();
	//double gs2 = np1.initialVelocity(np2).gs(); // target Gs
	if (gs1 > targetGs) sign = -1;
	double a = std::abs(gsAccel)*sign;
	//fpln(" ##### gsAccelGenerator: np1 = "+np1+" np2 = "+np2+"   gs1 = "+Units::str("kn",gs1)+" gs2 = "+Units::str("kn",targetGs));
	//		String setName = "TCP_"+setNum++;
	double t0 = np1.time();
	//fpln(" ##### gsAccelGenerator: Accelerate FROM gs1 = "+Units::str("kn",gs1)+" TO targetGs = "+Units::str("kn",targetGs)+" -------------");
	// if np1 is a TCP, we shift the begin point forward
	NavPoint b;
	NavPoint e;
	double accelTime = -1;
	double timeOffset = getMinTimeStep();
	if (!np1.isTCP()) {
		timeOffset = 0.0;
	}
	std::string label = np1.label();
	//fpln(" gsAccelGenerator: make BGSC from TCP!! np1.tcpSourceTime = "+np1.tcpSourceTime());
	NavPoint np1b = np1.makeStandardRetainSource();
	int ix = np1.linearIndex();
	b = np1b.makeBGS(np1.linear(vin, timeOffset).position(), t0+timeOffset, a, vin, ix).makeLabel(label); // .makeAdded();//.appendName(setName);
//	if (gsm == PRESERVE_TIMES) { // || (gsm == PRESERVE_RTAS && np2.isFixedTime())) {
//		double d = b.distanceH(np2);
//		double t = np2.time()-b.time();
//		std::pair<double,double> p2 = Kinematics::gsAccelToRTA(gs1, d, t, gsAccel);
//		//f.pln("gsAccelGenerator b="+b+" np2="+np2+" gs1="+gs1+" d="+b.distanceH(np2)+" gs2="+p2.first+" aTime="+p2.second);
//		accelTime = p2.second;
//		if (accelTime < 0) {
//			//f.pln("current gs = "+gs1+"  avg new gs ="+(d/t)+" over t="+t+"  a="+gsAccel);
//			return Triple<NavPoint,NavPoint,double>(np1b,np2,-1.0); // cannot complete
//		} else if (accelTime < getMinTimeStep()) {
//			return Triple<NavPoint,NavPoint,double>(np1b,np2,accelTime); // minor vs change
//		}
//		std::pair<Position,Velocity> pv = ProjectedKinematics::gsAccel(b.position(), vin, accelTime, a);
//		e = np1b.makeEGS(pv.first, accelTime+t0+timeOffset, vin, ix); // .makeAdded();//.appendName(setName);
//	} else {
		double d = b.distanceH(np2) - targetGs*getMinTimeStep(); 
		accelTime = (targetGs - gs1)/a;
		double deltaTime = (np2.time() - np1.time());
		//f.pln(" $$ gsAccelGenerator: accelTime = "+accelTime+" deltaTime = "+deltaTime);
		if (accelTime >= deltaTime) {
			return Triple<NavPoint,NavPoint,double>(np1b,np2,-accelTime);
		}
		double remainingDist = d - accelTime * (gs1+targetGs)/2;
		//fpln("$$$$$$$$$$$$ gsAccelGenerator: remainingDist = "+Units::str("nm",remainingDist));
		if (accelTime < getMinTimeStep()) {
			//fpln(" $$$$$ gsAccelGenerator no GS TCPS needed, at time +"+np1b.time()+" accelTime = "+accelTime);
			return Triple<NavPoint,NavPoint,double>(np1b,np2,accelTime); // no change
		}
		if (remainingDist <= 0) {
			//fpln(" ##### gsAccelGenerator: accelTime = "+accelTime+" remainingDist = "+Units::str("nm",remainingDist));
			return Triple<NavPoint,NavPoint,double>(np1b,np2,-1.0); // no change
		}
		// start moving in the current direction at the previous speed
		std::pair<Position,Velocity> pv = ProjectedKinematics::gsAccel(b.position(), vin, accelTime, a);
		e = np1b.makeEGS(pv.first, accelTime+t0+timeOffset, vin, ix); // .makeAdded();//.appendName(setName);
		//fpln(" gsAccelGenerator: make EGSC from TCP!! tcpSourceTime = "+np1b.tcpSourceTime()+" point="+e+" srctm="+e.tcpSourceTime());
//	}
	//fpln(" $$$$$$$$$$$$$$$$$$ gsAccelGenerator: b = "+b.toStringFull()+"\n e = "+e.toStringFull());
	return Triple<NavPoint,NavPoint,double>(b,e,accelTime);
}

// EXPERIMENTAL
Plan TrajGen::linearRepairShortGsLegsNew(const Plan& fp, double gsAccel) {
	Plan lpc = fp;
	//f.pln(" $$ smoothShortGsLegs: BEFORE npc = "+lpc.toString());
	for (int j = 1; j < lpc.size()-1; j++) {
		NavPoint np1 = fp.point(j);
		NavPoint np2 = fp.point(j+1);
		if (fp.inTrkChange(np1.time())) { // this is new!!! 7/8
			continue;
		}
		Velocity vin = fp.finalVelocity(j-1);
		//			f.pln(i+" REGULAR: +++++++++++++++++++++++++++++  vin = "+vin);
		double targetGS = fp.gsOut(j);   // get target gs out of lpc!
		if (Util::almost_equals(vin.gs(), 0.0) || Util::almost_equals(targetGS,0.0)) {
			fp.addWarning("TrajGen.generateGsTCPs: zero ground speed at index "+Fm0(j));
			continue;
		}
		Triple<NavPoint,NavPoint,double> tcpTriple =  gsAccelGenerator(np1, np2, targetGS, vin, gsAccel);
		//f.pln(" linearRepairShortGsLegsNew: for j = "+j+" tcpTriple = "+tcpTriple.toString());
        if (tcpTriple.third < 0) {
        	lpc = PlanUtil::linearMakeGSConstant(lpc, j-1,j+1);
        	//f.pln(" $$^^ REPAIRED GS ACCEL PROBLEM AT  j = "+j+" time = "+lpc.point(j).time());
        }
	}
    return lpc;
}



Plan TrajGen::generateGsTCPs(const Plan& fp, double gsAccel, bool repairGs) {
	Plan traj(fp); // the current trajectory based on working
	for (int i = traj.size() - 2; i > 0; i--) {
		if (repairGs) {
			bool checkTCP = true;
			PlanUtil::fixGsAccelAt(traj, i, gsAccel, checkTCP, getMinTimeStep());
		}
		double targetGS;
		NavPoint np1 = traj.point(i);
		NavPoint np2 = traj.point(i+1);
		if (traj.inTrkChange(np1.time())) {
			continue;
		}
		double gsIn = traj.gsFinal(i-1);    // ********* NEW $$RWB$$ ***********
		targetGS = traj.gsOut(i+1);   // RWB CHANGED FROM i on 8/25/2016
		if (Util::almost_equals(gsIn,0.0) || Util::almost_equals(targetGS,0.0)) {
			traj.addWarning("TrajGen.generateGsTCPs: zero ground speed at index "+Fm0(i));
			gsIn = 1E-10;
			//continue;
		}
		Velocity vin = traj.initialVelocity(i).mkGs(gsIn);
		//fpln(i+" ##### generateGsTCPs: Accelerate FROM gs1 = "+Units::str("kn",vin.gs())+" TO targetGs = "+Units::str("kn",targetGS)+" -------------");
		Triple<NavPoint,NavPoint,double> tcpTriple =  gsAccelGenerator(np1, np2, targetGS, vin, gsAccel);
		if (tcpTriple.third >= getMinTimeStep()) {
			//fpln(i+"#### generateGsTCPs: vin = "+vin+" targetGS = "+Units::str("kn",targetGS));
			//int offset = 0;
			//if (traj.inVerticalSpeedChange(np1.time())) fpln(" >>>>>>>>>>>> CAN'T HANDLE SIMULTANEOUS GS/VS ACCEL!");
			int j = i+2;
			NavPoint GSCBegin = tcpTriple.first;
			NavPoint GSCEnd = tcpTriple.second;
			if (traj.point(i).isAltPreserve()) {
				GSCBegin = GSCBegin.makeAltPreserve();
			}
			if (!np1.isTCP()) {
				//fpln(" #### generateGsTCPs remove point at time "+np1.time()+" index="+traj.getIndex(np1.time()));
				traj.remove(i);  // assumes times have not changed from lpc to lpc
				j = j - 1;
			}
			traj.add(GSCBegin);
			traj.add(GSCEnd);
			// GET THE GROUND SPEEDS BACK IN ORDER
			double nt = traj.linearCalcTimeGSin(j+1, targetGS);
			double timeShift = nt - traj.point(j+1).time();
			//fpln(" #######>>>>> for j+1 = "+(j+1)+" targetGS = "+Units::str("kn",targetGS)+" timeShift = "+timeShift+" nt = "+nt);
			//if (gsm != PRESERVE_TIMES) {
				traj.timeshiftPlan(j+1, timeShift); // , gsm == PRESERVE_RTAS);
			//}
		} else if (tcpTriple.third < 0) {
			printError("TrajGen.generateGsTCPs ERROR:  we don't have time to accelerate to the next point: "+np2.toString());
			traj.addError("TrajGen.generateGsTCPs ERROR:  we don't have time to accelerate to the next point at time "+Fm1(np2.time()),i);
			//}
		}
	 	//else if (tcpTriple.third > getMinTimeStep()*minorVfactor) {
			// mark known GS shift points
			//fpln(" $$>> traj = "+traj.toString());
		//}
	}
	//fpln(" generateGsTCPs: END traj = "+traj);
	return traj;
}



//Plan TrajGen::fixGS(const Plan& lpc, const Plan& fp, double gsAccel) {
//	Plan traj(fp);
//	//fpln(" fixGS: ---------------------------  lpc = "+lpc.toOutput(false,0,15));
//	//fpln(" fixGS: ---------------------------  traj = "+traj.toOutput(false,0,15));
//	for (int i = traj.size()-1; i > 0; i--) {
//		if ( traj.point(i).isBOT() ||
//				(!traj.point(i).isTCP()  && !traj.inTrkChange(traj.point(i).time()))     // don't separately shift MOTs
//				//  || traj.point(i).isGSCBegin()    // NOTE GSC removed because comes later
//				// || (traj.point(i).isVSCBegin() && !traj.inTurn(traj.point(i).time()))  //***RWB*** KCHANGE
//				) {
//			int lpcSegment = lpc.getSegment(traj.getTime(i));
//			if (!traj.point(i).isBOT() && !traj.point(i).isBVS()) lpcSegment = lpcSegment -1;
//			//fpln(" $$ fixGS: lpcSegment = "+lpcSegment+" traj.getTime(i) = "+traj.getTime(i)+" traj.point(i) = "+traj.point(i));
//			double targetGS = lpc.initialVelocity(lpcSegment).gs();
//			if (targetGS == 0.0) {
//				traj.addError("TrajGen.fixGS:  attempt to make ground speed 0!");
//				return traj;
//			}
//			double nt = traj.linearCalcTimeGSin(i, targetGS);
//			double timeShift = nt - traj.point(i).time();
//			//fpln(" #######>>>>> fixGS: for i = "+i+" targetGS = "+Units::str("kn",targetGS)+" timeShift = "+timeShift+" nt = "+nt);
//			traj.timeshiftPlan(i, timeShift);
//		}
//
//	}
//	//fpln(" fixGS: ---------------------------  traj = "+traj);
//	return traj;
//}




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
		nextVsChangeTm = traj.getTime(j);
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
	if (ixEVS >= 0) prevEndTime = Util::max(prevEndTime,traj.getTime(ixEVS));
	Triple<double,double,double> vsTriple = vsAccelGenerator(prevEndTime, np2.time(), nextVsChangeTm, vs1, vs2, a);
	double dt = vsTriple.third ;
	// we have a long enough accel time and it is calculated properly
	if (dt < 0) {
		// we have a calculation error, possibly too short a time for the needed accleration
		traj.addError("TrajGen::generateVsTCPs ERROR: Insufficient room i = "+Fm0(i)+" for vertical accel! tbegin = "+Fm1(vsTriple.first)+" prevEndTime = "+Fm1(prevEndTime));
		printError("TrajGen::generateVsTCPs ERROR: Insufficient room at i = "+Fm0(i)+" for vertical accel! tbegin = "+Fm1(vsTriple.first)+" prevEndTime = "+Fm1(prevEndTime));
		return Triple<double,double,double>(-1.0,-1.0,-1.0);
	} else if (dt > MIN_ACCEL_TIME) {
		double tbegin = vsTriple.first;
		double tend = vsTriple.second;
		return Triple<double,double,double>(tbegin,tend,a);
	} else { //if (vsTriple.third > getMinTimeStep()*minorVfactor) {
		// we have an accel time too small for TCP generation
		return Triple<double,double,double>(-1.0,-1.0,-1.0);
	}
}

Plan TrajGen::generateVsTCPs(const Plan& kpc, double vsAccel_d) {
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
            traj.addError("Vertical Speed regions overlap at time "+Fm2(tbegin));
            return traj;
		}
		if (tbegin > 0 && np1.time() > tbegin && np1.isEVS()) {   // see test "test_AWE918"
			 //fpln(" $$$$$$ ERROR: i= "+i+" np1 = "+np1.toStringFull());
		     //fpln(" $$$$$$ ERROR: tbegin = "+f.Fm2(tbegin)+" np1.time() = "+f.Fm2(np1.time()));
		     traj.addError("Vertical Speed regions overlap at time "+Fm2(tbegin));
		     return traj;
		}

		if (newTCPneeded) {
			std::string label = np2.label();
			np2 = np2.makeStandardRetainSource(); // ***RWB*** KCHANGE
			//fpln(" ### generateVsTCPs: traj.position("+tend+") = "+traj.position(tend));
			int linearIndex = np2.linearIndex();
			Velocity vin = traj.velocity(tbegin);
			NavPoint b = np2.makeBVS(traj.position(tbegin), tbegin, accel, vin, linearIndex).makeLabel(label); //.makeAdded();//.appendName(setName);
           //fpln(" $$ tbegin = "+Fm12(tbegin)+" tend = "+Fm12(tend));
			//fpln(" $$$$$$$$$$$$ generateVsTCPs: i = "+Fm0(i)+" make BVSC   point="+b.toStringFull()+" vin = "+vin.toString());
			NavPoint e = np2.makeEVS(traj.position(tend), tend, traj.velocity(tend), linearIndex); //.makeAdded();//.appendName(setName);
			//fpln(" $$$$$$$$$$$$ generateVsTCPs: make EVSC  point="+e.toStringFull());
			// turns can use traj because ground speeds are already correct there -- they may not be in other sections!
			NavPoint np0 = traj.point(i); // normally we delete this, but if this is a (horizontal) TCP point, we do not want to delete it
			if (!np0.isTCP()) {
				traj.remove(i);
			}
			int bindex = traj.add(b);
			int eindex = traj.add(e);
//fpln(" generateVsTCPs: bindex = "+Fm0(bindex)+"  eindex = "+Fm0(eindex));
//fpln(" $$$$$ generateVsTCPs: vin = "+vin.toString()+"   vend="+traj.velocity(tend).toString());
//fpln(" $$$$$ generateVsTCPs: b = "+b.toStringFull()+"\n            e = "+e.toStringFull());
//fpln(" $$$$$ generateVsTCPs: traj = "+traj.toString());
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
				double dt2 = traj.getTime(k) - tbegin;
				std::pair<double,double> npair = vsAccel(b.alt(), vs1, dt2, accel);
				double newAlt = npair.first;
 				//f.pln(" $$$$ k = "+k+"  dt2 = "+dt2+"  newAlt = "+Units.str("ft",newAlt));
				NavPoint newNp = traj.point(k).mkAlt(newAlt);
				//fpln(" ^^^^^^^^^^!! generateVsTCPs: traj.point(k) = "+traj.point(k).toStringFull());
				if (newNp.isTCP()) {
					//fpln(" ^^^^^^^^^^ generateVsTCPs: dt2 = "+dt2+" accel = "+accel);
					newNp = newNp.makeVelocityInit(newNp.velocityInit().mkVs(npair.second));
				}
				if (newAlt >= 0 && newAlt <= maxAlt) {
 				   traj.set(k, newNp);
                } else {
                	altok = false;
    				//f.pln(" $$$$ generateVsTCPs: b = "+Units.str("ft",b.alt())+" e = "+Units.str("ft",e.alt()));
    				//f.pln(" $$$$ generateVsTCPs: newAlt = "+Units.str("ft",newAlt));
                	//f.pln(" $$$$ generateVsTCPs: altok test failed!");
                	//DebugSupport.halt();
                }
			}
	    	if (!altok) traj.addError("TrajGen.generateVsTCPs: generated altitude is out of bounds");
		} else {
			//fpln(" $$ generateVsTCPs:   FAILED TO GENERATE Vertical TCPS at i = "+i);
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
	NavPoint p2 = fp.point(j+2);
	Velocity vf0 = fp.finalVelocity(j);
	Velocity vi1 = fp.initialVelocity(j+1);
	double deltaTrack1 = std::abs(vf0.trk() - vi1.trk());
	if (deltaTrack1> Units::from("deg",1.0)) {
		double gs1 = fp.initialVelocity(j).gs();
		double R1 = Kinematics::turnRadius(gs1,bank);
		Triple<NavPoint,NavPoint,NavPoint> turn1 = TurnGeneration::turnGenerator(p0, p1, p2, R1);
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
	NavPoint p2 = fp.point(j+2);
	Velocity vf0 = fp.finalVelocity(j);
	Velocity vi1 = fp.initialVelocity(j+1);
	//fpln("#### removeShortLastLeg:  j = "+j+" vf0 = "+vf0+" vi1 = "+vi1);
	double deltaTrack1 = std::abs(vf0.trk() - vi1.trk());
	//fpln(" #### removeShortLastLeg: deltaTrack1 = "+deltaTrack1);
	if (deltaTrack1 > Units::from("deg",1.0)) {
		double gs1 = fp.initialVelocity(j).gs();
		double R1 = Kinematics::turnRadius(gs1,bank);
		Triple<NavPoint,NavPoint,NavPoint> turn1 = TurnGeneration::turnGenerator(p0, p1, p2, R1);
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
			Triple<NavPoint,NavPoint,NavPoint> turn1 = TurnGeneration::turnGenerator(p0, p1, p2, R1);
			Triple<NavPoint,NavPoint,NavPoint> turn2 = TurnGeneration::turnGenerator(p1, p2, p3, R2);  // should this use R1 ~ gs1 ??
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
					double tmid =npc.getTime(j)+  mid.distanceH(p0.position())/gs1;
					NavPoint midNP = p1.makePosition(mid).makeTime(tmid); // preserve source info
					// fpln(" $$ ADD removeShortLegsBetween: midNP = "+midNP);
					npc.add(midNP);
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
			Triple<NavPoint,NavPoint,NavPoint> tg = TurnGeneration::turnGenerator(np1,np2,np3,R);
			NavPoint BOT = tg.getFirst(); //.appendName(setName);
			NavPoint MOT = tg.getSecond();//.appendName(setName);
			NavPoint EOT = tg.getThird(); //.appendName(setName);
			// Calculate Altitudes based on fp
			Position lBOT = fp.position(BOT.time());
			Position lMOT = fp.position(MOT.time());
			Position lEOT = fp.position(EOT.time());
			BOT = BOT.mkAlt(lBOT.alt());
			MOT = MOT.mkAlt(lMOT.alt());
			EOT = EOT.mkAlt(lEOT.alt());
			if (!turnIsFeas(fp, i, BOT, EOT, strict)) {
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
		if (currFixedAlt.isAltPreserve() || i == traj.size()-1) {
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
				if (np.isTCP()) {
					np = np.makeVelocityInit(np.velocityInit().mkVs(constantVs));
				}
				traj.set(j,np);
			}
			prevIndex = i;
			if (currFixedAlt.isTCP()) {
				currFixedAlt = currFixedAlt.makeVelocityInit(currFixedAlt.velocityInit().mkVs(constantVs));
			}
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
	int idx = lpc.add(npNew0);
	if (idx < 0)
		fpln(" genDirectTo: ERROR -- could not add direct to point!");
	//fpln(" #### genDirectTo0: lpc = "+lpc+" idx = "+idx);
	idx = lpc.add(npNew);
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
	lpc.add(new0);
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
		lpc.add(vertexNP);
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
				fp.add(NavPoint(connectPt,tm0+timeIntervalNextTry));
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
	lpc.add(s);
	if (!bestdtpDef) {
		//NavPoint np = s.linear(v, minTimeStep + bestdtp.third/2.0); // halfwat through turn plus some slop
		for (int i = 0; i < base.size(); i++) {
			if (base.getTime(i) < tt) lpc.add(base.point(i));
		}
		return lpc;
	}
	// fail
	lpc.add(base.point(base.size()-1));
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

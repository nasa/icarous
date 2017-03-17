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

package gov.nasa.larcfm.Util;

import java.util.ArrayList;

//import gov.nasa.larcfm.IO.DebugSupport;


/**
 * Trajectory generation functionality.  This class translates between Linear and Kinematic plans.  Note that the translations are 
 * not currently robust.
 * 
 * Note: there are several global variables that are settable by the user and modify low-level aspects of the transformation:
 * trajKinematicsTrack: allow track transformations (default true, if false, no turn TCPs will be generated)
 * trajKinematicsGS: allow gs transformations (default true, if false, no ground speed TCPs will be generated)
 * trajKinematicsVS: allow vs transformations (default true, if false, no vertical speed TCPs will be generated)
 * trajPreserveGS: prioritize preserving grounds speeds (default FALSE. If true, times may change, if false point times should be preserved)
 * trajAccelerationReductionAllowed: if true, if there are vs end points that nearly overlap with existing points, allow the acceleration to be adjusted so that they actually do overlap or are sufficiently distinct that there will not be problems inferring the velocities between them. (default true)
 * 
 * These values may be set through setter methods.
 * 
 */
public class TrajGen {

	static boolean method2 = true;

	
	static private boolean verbose = false;
	private static double MIN_ACCEL_TIME = 1.0;  
	private static double MIN_TURN_BUFFER = Plan.minDt;  
	static double MIN_VS_CHANGE = Units.from("fpm",50);     // was 100
	private static double MIN_VS_TIME = Units.from("s",30);          // was 30
	final static String minorTrkChangeLabel = "::minorTrkChange:";
//	private static double minVsChangeRecognizedDefault = 2.0; // Units.from("fpm", 50);
	private static final double minorVfactor = 0.01; // used to differentiate "minor" vel change vs no vel change
	private static final double maxVs = Units.from("fpm",10000);   // only used for warning
	private static final double maxAlt = Units.from("ft",60000);    // only used for error
	/**
	 * PRESERVE_GS: where ever possible, the gs for the final plan should match the gs of the initial plan.  Times are thrown out. (this is the "new" default)
	 * PRESERVE_TIMES: where ever possible, the times for points in the original plan should match the times for corresponding points in the final plan.  Ground speeds are thrown out.
	 * PRESERVE_RTAS: keep ground speeds the same as in the original plan except leading into time-fixed points, which keep the same times.
	 * CONSTANT_GS: ground speed is always equal to the initial ground speed in the original plan.  Times are thrown out.
	 */ 
	//static public enum GsMode {PRESERVE_GS, PRESERVE_TIMES};

	static public enum ErrType {NONE, UNKNOWN, TURN_INFEAS, TURN_OVERLAPS_B, TURN_OVERLAPS_E, GSACCEL_DIST, GS_ZERO, VSACCEL_DIST, REMOVE_FIXED, GSACCEL_OVERLAP};


	static class TransformationFailureException extends Exception {
		static final long serialVersionUID = 0;
		public double t;
		public TransformationFailureException() { super(); t = 0.0;}
		public TransformationFailureException(double j) { super(); t = j;}
		public TransformationFailureException(String s, double j) { super(s); t = j;}
	} 

	/**
	 * Trajectory generation is discretized by a certain time unit (meaning no two points on the trajectory should be closer in time than this value) 
	 */
	public static double getMinTimeStep() {
		return MIN_ACCEL_TIME;
	}

	/**
	 * Set the minimum time between two points on a plan.  Note that this should not be less than Constants.TIME_LIMIT_EPSILON, which is a
	 * "stronger" limit on the minimum.
	 */
	public static void setMinTimeStep(double t) {
		//Constants.set_time_accuracy(t);
		MIN_ACCEL_TIME = t;
	}


	public void setVerbose(boolean b) {
		verbose = b;	
	}

	private static void printError(String s) {
		if (verbose) System.out.println(s);
	}


	/** TODO
	 * 
	 * The resulting Plan will be "clean" in that it will have all original points, with no history of deleted points. 
	 *  Also all TCPs should reference points in a feasible plan. 
	 *  If the trajectory is modified, it will have added, modified, or deleted points.
	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
	 *  Note: This method seeks to preserve ground speeds of legs.  The time at a waypoint is assumed to be not important
	 *        compared to the original ground speed.
	 *  Note. If a turn vertex NavPoint has a non-zero "radius", then that value is used rather than "bankAngle"
	 *	@param fp input plan (is linearized if not already so)
	 *  @param bankAngle  maximum allowed (and default) bank angle for turns
	 *  @param gsAccel    maximum allowed (and default) ground speed acceleration (m/s^2)
	 *  @param vsAccel    maximum allowed (and default) vertical speed acceleration (m/s^2)
	 *  @param repairTurn attempt to repair infeasible turns as a preprocessing step
	 *  @param repairGs attempt to repair infeasible ground speed accelerations
	 *  @param repairVs attempt to repair infeasible vertical speed accelerations as a preprocessing step
	 *  @return the resulting kinematic plan
	 */
	public static Plan makeKinematicPlan(Plan fp, double bankAngle, double gsAccel, double vsAccel,
			boolean repairTurn, boolean repairGs, boolean repairVs) {
        //f.pln(" $$$ makeKinematicPlan: ENTER fp = "+fp.toStringFull(true));
        //f.pln(" $$$$ bankAngle = "+Units.str("deg",bankAngle)+" gsAccel = "+gsAccel+" vsAccel = "+vsAccel+"+"+ " repairTurn = "+repairTurn+" repairGs = "+repairGs+" repairVs = "+repairVs+" gsm = "+gsm);						
		Plan ret;
		Plan lpc = fp.copyWithIndex();  // set linearIndex for all points in linear plan
		if (lpc.size() < 2) {
			ret = lpc;
		} else {
			//f.pln(" $$$ makeKinematicPlan: ENTER ----------------------------------- lpc = "+lpc.toStringFull()+" "+repairTurn);
            boolean addMiddle = true;
            boolean flyOver = false;
            //DebugSupport.dumpPlan(lpc, "makeKinematicPlan_lpc");
            lpc = repairPlan(lpc, repairTurn, repairGs, repairVs, flyOver, addMiddle, bankAngle, gsAccel, vsAccel);
  			//f.pln(" makeKinematicPlan: generateTurnTCPs ---------------------------------------------------------------------");
			Plan kpc = markVsChanges(lpc);
			//f.pln(" makeKinematicPlan: AFTER MARK kpc = "+kpc);
			//DebugSupport.dumpPlan(kpc, "makeKinematicPlan_Mark");
			Plan kpc2 = generateTurnTCPs(kpc, bankAngle);
			//f.pln(" $$>> makeKinematicPlan: kpc2 = "+kpc2.toStringGs()+" ****** "+kpc2.isWellFormed());					
			//DebugSupport.dumpPlan(kpc2, "makeKinematicPlan_Turn");
			if (kpc2.hasError()) {
				ret = kpc2;
			} else {
				//f.pln(" generateTCPs: generateGsTCPs ----------------------------------- "+kpc2.isWellFormed());
				boolean useOffset = true;
				Plan kpc3 = generateGsTCPs(kpc2, gsAccel, repairGs, useOffset);
				//f.pln(" $$>> makeKinematicPlan: kpc3 = "+kpc3.toStringGs()+" ******"+kpc3.isWellFormed());		
				//DebugSupport.dumpPlan(kpc3, "generateTCPs_gsTCPs");
				if (kpc3.hasError()) {
					ret = kpc3;
				} else {
					//f.pln(" $$>> makeKinematicPlan.makeMarkedVsConstant ----------------------------------- "+kpc3.isWellFormed());
					Plan kpc4 = makeMarkedVsConstant(kpc3);
					//DebugSupport.dumpPlan(kpc4, "generateTCPs_vsconstant");
					//f.pln(" $$>> makeKinematicPlan: kpc4 = "+kpc4.toStringFull()+" ******"+kpc4.isWellFormed());	
					if (kpc4.hasError()) {
						ret = kpc4;
					} else {
						//f.pln(" makeKinematicPlan: generateVsTCPs ----------------------------------- "+kpc4.isWellFormed());
                        boolean continueGen = false;
						Plan kpc5 = generateVsTCPs(kpc4, vsAccel, continueGen);
						//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
						cleanPlan(kpc5);
						//f.pln(" $$>> makeKinematicPlan: kpc5 = "+kpc5.toStringGs());	
						//f.pln(" generateTCPs: DONE ----------------------------------- wellFormed = "+kpc5.isWellFormed());
						ret = kpc5;
					}
				}
			}
		}
		ret.setNote(fp.getNote());
		//f.pln(" $$$$ makeKinematicPlan: END ret.size() = "+ret.size());
		return new Plan(ret);
	}

	public static Plan repairPlan(Plan lpc, boolean repairTurn, boolean repairGs, boolean repairVs,
			boolean flyOver, boolean addMiddle, double bankAngle, double gsAccel, double vsAccel) {
		//f.pln(" repairPlan: ENTER, lpc = "+lpc.toStringFull());
		if (repairTurn) {	
			lpc = linearRepairShortTurnLegs(lpc, bankAngle, addMiddle);
		}
        //f.pln("repairPlan turn :"+lpc.toString());		
		if (lpc.hasError()) {
			//f.pln(" $$$0 repairPlan: repair failed! "+lpc.getMessageNoClear());
		} else {
			if (lpc.hasError())  {
				//f.pln(" $$$1 repairPlan: repair failed! "+lpc.getMessageNoClear());
			} else {
				if (repairVs) {
					lpc = linearRepairShortVsLegs(lpc,vsAccel);
					if (lpc.hasError()) {
						//f.pln(" $$$2 generateTCPs: repair failed! "+lpc.getMessageNoClear());
					} else {
						if (repairTurn && flyOver) {
							lpc = removeInfeasibleTurnsOver(lpc,bankAngle);						
						} else if(repairTurn) {
							//f.pln(" generateTCPs: repairPlan -----------------------------------");
							boolean strict = false;
							lpc = removeInfeasibleTurns(lpc,bankAngle,strict);
						}
						//DebugSupport.dumpPlan(lpc, "repairPlan");
					}
				}
			}
		}
		//f.pln(" repairPlan: EXIT, lpc = "+lpc.toStringFull());
		return lpc;
	}
	
	
	public static Plan repairShortLegs(Plan lpc,
			boolean flyOver, boolean addMiddle, double bankAngle, double gsAccel, double vsAccel) {
		lpc = linearRepairShortTurnLegs(lpc, bankAngle, addMiddle);
		if (!lpc.hasError()) {
//			if (gsm != GsMode.PRESERVE_TIMES)  {
//				lpc = linearRepairShortGsLegs(lpc,gsAccel);
//			}
			if (!lpc.hasError())  {
				lpc = linearRepairShortVsLegs(lpc,vsAccel);
			}
		}
		return lpc;
	}




	/** 
	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points. 
	 *  Also all TCPs should reference points in a feasible plan. 
	 *  If the trajectory is modified, it will have added, modified, or deleted points.
	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
	 *	@param fp input plan (is linearized if not already so)
	 *  @param bankAngle maximum allowed (and default) bank angle for turns
	 *  @param gsAccel    maximum allowed (and default) ground speed acceleration (m/s^2)
	 *  @param vsAccel    maximum allowed (and default) vertical speed acceleration (m/s^2)
	 *  @param repair attempt to repair infeasible turns, gs accels, and vs accelerations as a preprocessing step
	 *  @param constantGS if true, produces a constant ground speed kinematic plan with gs being average of linear plan
	 */
	public static Plan makeKinematicPlan(Plan fp, double bankAngle, double gsAccel, double vsAccel, 
			boolean repair, boolean constantGS) {
		Plan traj = makeKinematicPlan(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair);
		return traj;
	}


	/** 
	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points. 
	 *  Also all TCPs should reference points in a feasible plan. 
	 *  If the trajectory is modified, it will have added, modified, or deleted points.
	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
	 *	@param fp input plan (is linearized if not already so)
	 *  @param bankAngle maximum allowed (and default) bank angle for turns
	 *  @param gsAccel   maximum allowed (and default) ground speed acceleration (m/s^2)
	 *  @param vsAccel   maximum allowed (and default) vertical speed acceleration (m/s^2)
	 *  @param repair    attempt to repair infeasible turns, gs accels, and vs accelerations as a preprocessing step
	 */
	public static Plan makeKinematicPlan(Plan fp, double bankAngle, double gsAccel, double vsAccel, 
			boolean repair) {
		Plan traj = makeKinematicPlan(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair);
		return traj;
	}


	/**
	 * Remove records of deleted points and make all remaining points "original"
	 * Used to clean up left over generation data
	 * @param fp
	 */
	public static void cleanPlan(Plan fp) {
		fp.mergeClosePoints(Plan.minDt);
		for (int i = 0; i < fp.size(); i++) {
			//fp.set(i,NavPt.makeOriginal(fp.get(i)));
	        TcpData tcp = fp.getTcpData(i).setOriginal();
			fp.set(i, fp.point(i),tcp);
		}
	}




	/**
	 * Returns an index at or before iNow where there is a significant enough track change to bother with.
	 * This is used to "backtrack" through collinear points in order to get a reasonable sized leg to work with. 
	 */
	private static int prevTrackChange(Plan fp, int iNow) {
		for (int i = iNow-1; i > 0; i--) {
			if (i==1) return 0;
			//NavPoint npi = fp.point(i);
			//NavPoint npPrev = fp.point(i-1);
			Velocity vf0 = fp.finalVelocity(i-1);
			Velocity vi1 = fp.initialVelocity(i);
			if (Math.abs(vf0.trk() - vi1.trk()) > Units.from("deg",1.0)) return i;
		}
		return 0;
	}

	/**
	 * Returns an index at or after iNow where there is a significant enough track change to bother with
	 * This is used to move forward through collinear points in order to get a reasonable sized leg to work with. 
	 */
	private static int nextTrackChange(Plan fp, int iNow) {
		for (int i = iNow+1; i+1 < fp.size(); i++) {
			if (i>=fp.size()) return fp.size()-1;
			//NavPoint npi = fp.point(i);
			//NavPoint npNext = fp.point(i+1);
			Velocity vf0 = fp.finalVelocity(i);
			Velocity vi1 = fp.initialVelocity(i+1);
			//f.pln("nextTrackChange:  i = "+i+" vf0 = "+vf0+" vi1 = "+vi1);
			if (Math.abs(vf0.trk() - vi1.trk()) > Units.from("deg",1.0)) return i+1;
		}
		return fp.size()-1;
	}

	/** Returns true if turn at i can be inscribed in the available leg space.
	 * 
	 * @param lpc     source plan
	 * @param i       vertex to be tested
	 * @param BOT     Beginning of turn
	 * @param EOT     End of turn
	 * @return        true iff the BOT - EOT pair can be placed within vertex
	 */
	private static boolean turnIsFeas(Plan lpc, int i, NavPoint BOT, NavPoint EOT) {
		NavPoint np2 = lpc.point(i);
		double d1new = BOT.distanceH(np2);
		double d2new = np2.distanceH(EOT);
		int ipTC = i-1;
		int inTC = i+1;
		ipTC = prevTrackChange(lpc,i);
		inTC = nextTrackChange(lpc,i);
		double d1old = lpc.point(ipTC).distanceH(np2);
		double d2old = np2.distanceH(lpc.point(inTC));	
		boolean rtn = true;
		//f.pln(" $$$$ generateTurnTCPs: "+(BOT.time()-getMinTimeStep())+" "+fp.getTime(i-1)+" "+EOT.time()+getMinTimeStep()+" "+fp.getTime(i+1));
		if (BOT.time()-MIN_TURN_BUFFER <= lpc.time(ipTC) || EOT.time()+MIN_TURN_BUFFER >= lpc.time(inTC) 
				|| d1new+Constants.get_horizontal_accuracy() >= d1old || d2new+Constants.get_horizontal_accuracy() >= d2old) {
			//traj = new PlanCore(lpc); // revert to initial plan
			//traj.addError("ERROR in TrajGen.generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.",i);
			//printError("ERROR in TrajGen.generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.");
			if (verbose) {
				f.pln("####### i = "+i+" ipTC = "+ipTC+" inTC = "+inTC+" np2 =  "+Plan.toStringFull(np2,lpc.getTcpData(i)));
				f.pln("####### BOT.time() = "+BOT.time()+" <? fp.getTime(ipTC) = "+lpc.time(ipTC));
				f.pln("####### EOT.time() = "+EOT.time()+" >? fp.getTime(inTC) = "+lpc.time(inTC));
				f.pln("####### d1new = "+d1new+" >? d1old = "+d1old);
				f.pln("####### d2new = "+d2new+" >? d2old = "+d2old);
				f.pln("####### turnIsFeas: FAIL on lpc = "+lpc);
			} 
			rtn = false;
		}
		return rtn;
	}



	/**
	 * This takes a Plan lpc and returns a plan that has all points with vertical changes as "AltPreserve" points, with all other points
	 * being "Original" points.  Beginning and end points are always marked.
	 * @param lpc source plan
	 * @return kinematic plan with marked points
	 */
	public static Plan markVsChanges(Plan lpc) {
		//f.pln(" $$$ markVsChanges: ENTER, lpc = "+lpc.toStringFull());
		String name = lpc.getName();
		Plan kpc = new Plan(name);
		if (lpc.size() < 2) {
			kpc = new Plan(lpc);
			kpc.addError("TrajGen.markVsChanges: Source plan "+name+" is too small for kinematic conversion");
			return kpc;
		}
		kpc.add(lpc.get(0)); // first point need not be marked
		for (int i = 1; i < lpc.size(); i++) {
			NavPoint np = lpc.point(i);
			TcpData  tcp = lpc.getTcpData(i);
			//f.pln("$$$$$ markVsChanges: i= "+i+" np = "+np+" tcp = "+tcp);
			double vs1 = lpc.vsOut(i-1);
			double vs2 = lpc.vsOut(i);			
			if (Math.abs(vs1) > maxVs) {
				kpc.addWarning("Input File has vertical speed exceeding "+Units.str("fpm",maxVs)+" at "+(i-1));
			}
			double deltaTm = 0;
			if (i+1 < lpc.size()) deltaTm = lpc.time(i+1) - lpc.time(i);
            boolean nextTheSame = true;
			if (i+1 < lpc.size()) {
				double vs3 = lpc.vsOut(i);
				nextTheSame = Math.abs(vs2-vs3) < MIN_VS_CHANGE;
            }
			double deltaVs = Math.abs(vs1-vs2);
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
		//f.pln("\n\n $$************ markVsChanges: EXIT, kpc = "+kpc.toStringFull());
		return kpc;
	}

//	/**
//	 * This takes a Plan lpc and returns a plan that has all points with vertical changes as "AltPreserve" points, with all other points
//	 * being "Original" points.  Beginning and end points are always marked.
//	 * @param lpc source plan
//	 * @return kinematic plan with marked points
//	 */
//	public static Plan markVsChanges_NEW(Plan lpc, boolean kinematic) {
//		//f.pln(" $$$ markVsChanges: ENTER, lpc = "+lpc.toStringFull());
//		String name = lpc.getName();
//		Plan kpc = new Plan(name);
//		if (lpc.size() < 2) {
//			kpc = new Plan(lpc);
//			kpc.addError("TrajGen.markVsChanges: Source plan "+name+" is too small for kinematic conversion");
//			return kpc;
//		}
//		kpc.add(lpc.get(0)); // first point need not be marked
//		for (int i = 1; i < lpc.size(); i++) {
//			NavPoint np = lpc.point(i);
//			TcpData  tcp = lpc.getTcpData(i);
//			//f.pln("$$$$$ markVsChanges: i= "+i+" np = "+np+" tcp = "+tcp);
//			double vs1 = lpc.vsOut(i-1);
//			double vs2 = lpc.vsOut(i);
//			if (Math.abs(vs1) > maxVs) {
//				kpc.addWarning("Input File has vertical speed exceeding "+Units.str("fpm",maxVs)+" at "+(i-1));
//			}
//			double deltaTm = 0;
//			if (i+1 < lpc.size()) deltaTm = lpc.time(i+1) - lpc.time(i);
//			int ixMOT = -1;
//			//f.pln("$$$$$ markVsChanges: i =  "+i+" original vs1 = "+Units.str("fpm",vs1)+" vs2 = "+Units.str("fpm",vs2)+" deltaTm = "+deltaTm);	
//			if (kinematic) {
//				//f.pln(" $$$$$$$$$$$$$$$ markVsChanges_NEW: inTrkChange at i = "+i+" lpc.time(i) = "+lpc.time(i));
//				if (lpc.inTrkChange(lpc.time(i))) {
//					//f.pln(" $$$$$$$$$$$$$$$ markVsChanges_NEW: inTrkChange at i = "+i);
//					if (lpc.isBOT(i))       {
//						ixMOT = lpc.findMOT(i,lpc.nextEOT(i));
//						//f.pln(" $$$$$$$$$$$$$$$ markVsChanges_NEW: i = "+i+" ixMOT = "+ixMOT);
//					}
//					if (i == ixMOT)         tcp.setAltPreserve();
//				} else if ( ! Util.within_epsilon(vs1, vs2, MIN_VS_CHANGE/10.0) || (deltaTm >= MIN_VS_TIME)) {
//					tcp.setAltPreserve();
//				}
//				if (lpc.isEOT(i))  ixMOT = -1;			
//			} else {
//				if (Util.within_epsilon(vs1, vs2, MIN_VS_CHANGE/10.0) && (deltaTm < MIN_VS_TIME)) {
//					// NOT MARKED
//				} else {
//					tcp.setAltPreserve();
//				}
//			}
//			//f.pln(" $$>>>>>>>>>>>>>> markVsChanges: i= "+i+" marked ALT = "+Units.str("ft",kpc.point(i).alt())+" PRESERVE tcp = "+tcp);	
//			kpc.add(np, tcp); 
//		}
//		//f.pln("\n\n $$************ markVsChanges: EXIT, kpc = "+kpc.toStringFull());
//		return kpc;
//	}


	public static Plan generateTurnTCPs(Plan lpc, double bankAngle) {
		boolean continueGen = false;
		return generateTurnTCPs(lpc,bankAngle,continueGen);
	}

	/** 
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 * bank angle must be nonnegative!!!!!
	 * 
	 * @param lpc        linear plan
	 * @param default_bank_angle  the default bank angle, if a radius is not present in the plan. 
	 * @return           a turn plan with BOTs and EOTs
	 */
	public static Plan generateTurnTCPs(Plan lpc, double default_bank_angle, boolean continueGen) {
		//f.pln(" $$>> generateTurnTCPs: lpc = "+lpc.toStringFull());
		//f.pln(" $$>> generateTurnTCPs: default_bank_angle = "+Units.str("deg",default_bank_angle)+" "+continueGen+" "+strict);
		//DebugSupport.dumpAsUnitTest(lpc);		
		Plan traj = new Plan(lpc); // the current trajectory based on working
		double bank = Math.abs(default_bank_angle);
		for (int i = lpc.size()-2; i > 0; i--) {   // Perform in reverse order so indexes match between lpc and traj
			if (lpc.inAccelZone(i)) continue;     // skip over regions already generated (for now only BOT-EOT pairs)
			Velocity vf0 = lpc.finalVelocity(i-1);
			Velocity vi1 = lpc.initialVelocity(i);
			double gsIn = vf0.gs(); 
			//f.pln(" $$>> generateTurnTCPs:  i = "+i+" vf0 = "+vf0+" vi1 = "+vi1);
			if (Util.almost_equals(gsIn,0.0)) {
				printError("TrajGen.generateTurnTCPs ###### WARNING: Segment from point "+(i-1)+" to point "+(i)+" has zero ground speed!");
				continue;
			}
			double turnDelta = Util.turnDelta(vf0.trk(), vi1.trk());
			//f.pln(" $$$ generateTurnTCPs: gsIn = "+Units.str("kn",gsIn,4));
			double turnTime;
			TcpData tcp_i = lpc.getTcpData(i);
			double R = tcp_i.turnRadius();
			//f.pln(" $$>> generateTurnTCPs: i = "+i+" np2.signedRadius() = "+Units.str("NM",np2.signedRadius(),8));
			if (Util.almost_equals(R, 0.0)) {
				if (bank == 0) {
					//f.pln(" $$$ ERROR in TrajGen.generateTurnTCPs: specified bank angle is 0");
					traj.addError("ERROR in TrajGen.generateTurnTCPs: specified bank angle is 0");
					bank = Units.from("deg", 0.001); // prevent divisions by 0.0
				}
			    turnTime = Kinematics.turnTime(gsIn,turnDelta, bank);
			    R = Kinematics.turnRadius(gsIn, bank);
			} else {
 			    turnTime = turnDelta*R/gsIn;
			}
            //f.pln(" $$>> generateTurnTCPs: i = "+i+" R = "+Units.str("NM",R)+" turnTime = "+f.Fm2(turnTime)+" >=? "+getMinTimeStep());
			if (turnTime >= getMinTimeStep()) {
				insertTurnTcpsInTraj(traj, lpc, i, R, continueGen);
				if (traj.hasError() && ! continueGen) return traj;
			} 
			//f.pln(" $$>>>>>>>>> generateTurnTCPs: AFTER i = "+i+" traj = "+traj.toStringGs());
		}//for
		//f.pln(" $$>>>>>>>>> generateTurnTCPs: traj = "+traj.toStringGs());
		return traj;
	}


	

	/** 
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 * 
	 * @param lpc        linear plan
	 * @param continueGen continue generation, even with problems
	 * @return           a plan with BOTs and EOTs
	 */
	public static Plan generateTurnTCPsRadius(Plan lpc, boolean continueGen) { 
		Plan traj = new Plan(lpc); // the current trajectory based on working
		//f.pln(" $$>> generateTurnTCPsRadius: lpc = "+lpc.toStringFull());
		//DebugSupport.dumpPlan(lpc,"generateTurnTCPsRadius_lpc");
		for (int i = lpc.size()-2; i > 0; i--) {   // Perform in reverse order so indexes match between lpc and traj
			NavPoint np2 = lpc.point(i);
			Velocity vf0 = lpc.finalVelocity(i-1);
			Velocity vi1 = lpc.initialVelocity(i);
			double gsIn = vf0.gs(); 
			if (Util.almost_equals(gsIn,0.0)) {
				printError("TrajGen.generateTurnTCPs ###### WARNING: Segment from point "+(i-1)+" to point "+(i)+" has zero ground speed!");
				continue;
			}
			double turnDelta = Util.turnDelta(vf0.trk(), vi1.trk());
			TcpData tcp_i = lpc.getTcpData(i);
			double R = tcp_i.turnRadius(); 	// if R is 0 then turnTime will be zero and point will not be processed
			double turnTime = turnDelta*R/gsIn;    // if R = 0 ==> do not process this point
			//f.pln(" $$>> generateTurnTCPsRadius(lpc): i = "+i+" R = "+Units.str("NM",R)+" turnTime = "+turnTime);
			if (turnTime >= getMinTimeStep()) {				
				insertTurnTcpsInTraj(traj, lpc, i, R, continueGen);
				//f.pln(" $$>>>>>>>>>  generateTurnTCPsRadius: EXIT traj = "+traj.toStringFull());
			    if (!continueGen && traj.hasError()) return traj;
			}
		}//for
		//f.pln(" $$>>>>>>>>>  generateTurnTCPsRadius: EXIT traj = "+traj.toStringFull());
		return traj;
	}
	
	/** 
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 * 
	 * @param lpc        linear plan
	 * @param strict     if strict do not allow any interior waypoints in the turn
	 * @return           a plan with BOTs and EOTs
	 */
	public static Plan generateTurnTCPsRadius(Plan lpc) {
		//boolean strict = false;
		boolean continueGen = false;
		return generateTurnTCPsRadius(lpc, continueGen);
	}


	
	/**
	 * 
	 * @param traj          trajectory under construction
	 * @param lpc           original lpc
	 * @param ixNp2         index of np2 in plan
	 * @param R             unsigned radius
	 * @param continueGen   continue generation, even with problems
	 */
	private static void insertTurnTcpsInTraj(Plan traj, Plan lpc, int ixNp2, double R, boolean continueGen) {
		NavPoint np1 = lpc.point(ixNp2-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
		NavPoint np2 = lpc.point(ixNp2);
		NavPoint np3 = lpc.point(ixNp2+1);
		if (np3.time() - np2.time() < 0.1 && ixNp2+2 < lpc.size()) { //TODO: fixed time?
			np3 = lpc.point(ixNp2 + 2);
		}
		int linearIndex = lpc.getTcpData(ixNp2).getLinearIndex();
		//TcpData tcp2 = traj.getTcpData(ixNp2);
		//f.pln(" $$$ generateTurnTCPs: i ="+i+" linearIndex = "+linearIndex);
		//f.pln(" $$$ insertTurnTcpsInTraj: lpc = "+lpc.toStringFull(true));
		Velocity vf0 = lpc.finalVelocity(ixNp2-1);
		Velocity vi1 = lpc.initialVelocity(ixNp2);
		double gsIn = vf0.gs(); 
		double targetGS = vi1.gs();
		Tuple5<NavPoint,NavPoint,NavPoint,Integer,Position> tg = TurnGeneration.turnGenerator(np1,np2,linearIndex,np3,R);		
		NavPoint BOT = tg.getFirst(); 
		NavPoint MOT = tg.getSecond();
		NavPoint EOT = tg.getThird(); 
		int dir = tg.getFourth();
		Velocity vin = NavPoint.initialVelocity(np1,np2);
		Velocity vout = NavPoint.initialVelocity(np2,np3); // used to set gs and vs
		Velocity vin2 = BOT.position().initialVelocity(np2.position(),100);
		Velocity vout2 = np2.position().finalVelocity(EOT.position(),100);
		vin = vin.mkTrk(vin2.trk());
		vout = vout.mkTrk(vout2.trk());
		NavPoint np2_src = lpc.sourceNavPoint(ixNp2).makeLabel(np2.label());
		String srcinfo = lpc.getInfo(ixNp2);
		//f.pln(" $$$ insertTurnTcpsInTraj: i = "+i+" np2_src = "+np2_src);	
		double signedRadius = dir*R;
		Position center;
		if (method2) {
			center = tg.fifth;
		} else {
		    center = KinematicsPosition.centerFromRadius(BOT.position(), signedRadius, vin.trk());
		}
		//f.pln("$$ insertTurnTcpsInTraj: center = "+center);
		Pair<NavPoint,TcpData> npBOT = Plan.makeBOT(np2_src, BOT.position(), BOT.time(),  signedRadius, center, linearIndex);
		Pair<NavPoint,TcpData> npEOT =	Plan.makeEOT(np2_src, EOT.position(), EOT.time(), linearIndex);	
		npBOT.second.setInformation(srcinfo);
        // ------- BOT time is before np1 ---------
		if (ixNp2 > 1 && BOT.time() < np1.time()) {
			gsIn = traj.gsIn(ixNp2-1);   //ignore ground speed change in middle of turn
		}
		double botAlt; 
		if (BOT.time() < lpc.getFirstTime()) {  // is this right?
			// should be an error?
			botAlt = lpc.point(0).alt();
		} else {
			//f.pln(" $$$ BOT.time() = "+BOT.time());
			botAlt = lpc.position(BOT.time()).alt();
		}								
		BOT = BOT.mkAlt(botAlt);
		//f.pln(" $$$$ generateTurnTCPs: BOT.time() = "+f.Fm2(BOT.time())+" np times = "+f.Fm2(np1.time())+", "+f.Fm2(np2.time())+", "+f.Fm2(np3.time()));
		//f.pln(" $$$$ BOT.time() = "+BOT.time()+" botAlt = "+Units.str("ft", botAlt)+" lpc.getFirstTime() = "+lpc.getFirstTime());
		MOT = MOT.mkAlt(lpc.position(MOT.time()).alt());
		//f.pln(" $$>> generateTurnTCPs: MOT = "+MOT.toStringFull());
		double EotAlt;
		if (EOT.time() > lpc.getLastTime()) { // is this right?
			// should be an error?
			EotAlt = lpc.getLastPoint().alt();
		} else {					
			EotAlt = lpc.position(EOT.time()).alt();
		}
		//f.pln(" $$: insertTurnTcpsInTraj: EotAlt =  "+Units.str("ft", EotAlt));
		EOT = EOT.mkAlt(EotAlt);
		String label = "";
		if ( ! np2.label().equals("")) label = " ("+np2.label()+") ";
		if ((BOT.isInvalid() || MOT.isInvalid() || EOT.isInvalid())) {
			if (!continueGen) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: turn points at "+ixNp2+label+" invalid.",ixNp2);
			    printError("ERROR in TrajGen.generateTurnTCPs: turn points at "+ixNp2+label+" invalid.");
			}
		} else if ( ! turnIsFeas(lpc, ixNp2, BOT, EOT)) {
			if (!continueGen) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: cannot achieve turn at point "+ixNp2+label
						+" from surrounding points at time "+f.Fm2(lpc.point(ixNp2).time()),ixNp2);
				printError("ERROR in TrajGen.generateTurnTCPs: cannot achieve turn "+ixNp2+label+" from surrounding points at "+ixNp2);
			}
		} else if (EOT.time()-BOT.time() > 2.0*getMinTimeStep()) { // ignore small changes  is the 2.0 factor right?
			if (traj.inTrkChange(BOT.time())) {
				if (!continueGen) {
					traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn in time at "+ixNp2+label,ixNp2);
					printError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn int time at "+ixNp2+label);
				}
			} else if (traj.inTrkChange(EOT.time())) {
				if (!continueGen) {
					traj.addError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn in time at "+ixNp2+label,ixNp2);
					printError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn int time at "+ixNp2+label);
				}
			} else {		
				// **************** traj is not modified above here (other than errors) ************************
				int ixBOT = traj.add(npBOT);
				ixNp2 = ixNp2 + 1;                      // just added point before np2
				int ixEOT = traj.add(npEOT);
				//f.pln(" $$$$..................... insertTurnTcpsInTraj: ixBOT = "+ixBOT+" ixEOT = "+ixEOT); 
				ixEOT = movePointsWithinTurn(traj,ixBOT,ixNp2,ixEOT,gsIn);	
				// ********** cannot survive the following errors: already added BOT, EOT ************
				if (ixBOT < 0 || ixEOT < 0 ) {
					//f.pln("$$$$$$ TrajGen.generateTurnTCPs: BOT/EOT overlaps point at ixBOT = "+ixBOT);
					traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT/EOT overlaps point ");
					return;  		
				}			
				if (!trackContinuousAt(traj, ixBOT, Math.PI/10)) {
					//f.pln("$$$$$$ TrajGen.generateTurnTCPs: track into BOT not equal to track out of BOT at "+ixBOT+label);
					traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into BOT not equal to track out of BOT at "+ixBOT+label);
					return;

				}
				if (!trackContinuousAt(traj, ixEOT, Math.PI/10)) {
					//f.pln("TrajGen.generateTurnTCPs:  track into EOT not equal to track out of EOT at "+ixEOT+label);
					traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into EOT not equal to track out of EOT at "+ixEOT+label);
					return;
				}			
				//f.pln(" $$>> generateTurnTCPs: at ixBOT = "+ixBOT+"  ixEOT = "+ixEOT+" gsIn = "+Units.str("kn",gsIn,4));
				traj.mkGsIn(ixBOT, gsIn);  
				//traj.mkGsOut(ixBOT, gsIn); // should we have this too?
				traj.mkGsIn(ixEOT, gsIn);    // can this be removed?
				traj.mkGsOut(ixEOT, targetGS);	
				//f.pln(" $$>> generateTurnTCPs: END traj = "+traj.toStringGs());
			}
		} 
		//f.pln(" $$$ insertTurnTcpsInTraj:: traj = "+traj.toStringGs());
	}

	
	
	/**
	 * Re-aligns points that are within a newly generated turn at j
	 */
	private static int movePointsWithinTurn(Plan traj, int ixBOT, int ixNp2, int ixEOT,  double gsIn) {
		//f.pln(" $$$ movePointsWithinTurn: ------- ixBOT, ixNp2, ixEOT = "+ixBOT+", "+ixNp2+", "+ixEOT);
		//DebugSupport.dumpPlan(traj,"movePointsWithinTurn_traj");
		NavPoint vertex = traj.point(ixNp2);
		NavPoint BOT = traj.point(ixBOT);
		NavPoint EOT = traj.point(ixEOT);		
		double distVertexToEOT =  vertex.distanceH(EOT);
		double distVertexToBOT =  vertex.distanceH(BOT);	
		double longDistTOEOT = traj.point(ixBOT).distanceH(vertex) + vertex.distanceH(EOT);
		//f.pln(" $$ movePointsWithinTurn_New: distVertexToEOT = "+Units.str("NM",distVertexToEOT,7));
		//f.pln(" $$ movePointsWithinTurn_New: distVertexToBOT = "+Units.str("NM",distVertexToBOT,7));
		//f.pln(" $$ movePointsWithinTurn_New: pathDist = "+Units.str("NM",pathDist,7));
		//f.pln(" $$ movePointsWithinTurn_New: longDistTOEOT = "+Units.str("NM",longDistTOEOT,7));
		int lastIxInTurn = -1;
		// ************ EOT can be out of order so let's remove it before we move inner points
		//Pair<NavPoint, TcpData> pairEOT = traj.get(ixEOT);
		//raj.remove(ixEOT);
		for (int ii = ixNp2; ii < traj.size(); ii++) {
			double dist_to_ii =  vertex.distanceH(traj.point(ii));
			//f.pln(" %%%%% generateTurnTCPs: ii = "+ii+" dist_to_ii = "+Units.str("NM",dist_to_ii,7));
			if (ii == ixEOT) continue;
			else if (dist_to_ii < distVertexToEOT) lastIxInTurn = ii;
			else break;
		}
		int firstIxInTurn = lastIxInTurn;
		for (int ii = ixNp2; ii > ixBOT; ii--) {
			double dist_from_ii =  vertex.distanceH(traj.point(ii));
			//f.pln(" %%%%% generateTurnTCPs: ii = "+ii+" dist_from_ii = "+Units.str("NM",dist_from_ii,5));
			if (dist_from_ii <= distVertexToBOT) firstIxInTurn = ii;
			else break;
		}
		//f.pln(" $$$$$>>>>>>>>>>>>>>>>>>>>>>>>>> firstIxInTurn = "+firstIxInTurn+" lastIxInTurn = "+lastIxInTurn);
		Plan tempPlan = new Plan();
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
		tempPlan.addNavPoint(traj.point(ixBOT));  // add BOT as non-TCP (i.e. want linear distance
		double tmBOT = traj.time(ixBOT);
		for (int i =  tempPlan.size()-1; i > 0; i--) {  
			NavPoint orig_i = tempPlan.point(i);
			TcpData tcp_i = tempPlan.getTcpData(i);
			double dist_to_i = tempPlan.pathDistance(0,i);  // linear distance
			double d_i = ratio*dist_to_i;
			//f.pln(" $$ movePointsWithinTurn_New: d_i = "+Units.str("NM",d_i,5));
			boolean linear = false;
			Pair<Position, Integer> adv = traj.advanceDistance2D(ixBOT, d_i, linear);
			double alt_i = orig_i.alt();
			Position pos_i = adv.first.mkAlt(alt_i);			
			double dt = d_i/gsIn;
			double t_i = tmBOT + dt;
			String label_i =  orig_i.label();
			double deltaToMOT = Math.abs(dist_to_i - longDistTOEOT/2.0);
			if (deltaToMOT < 1.0) label_i = ""; // MOT label is moved to BOT
			NavPoint np_i = new NavPoint(pos_i, t_i,label_i);
			if (tcp_i.isTrkTCP()) {
				traj.addError("ERROR in TrajGen.generateTurnTCPs: TURN overlap !!! at i = "+i);
			}			
			tcp_i.clearTrk();  // TODO:  this could allow small turn in big turn (but would have to make sure both TCPS are inside
            tcp_i.setRadiusSigned(0.0); // inner points should not have radius data
			//f.pln(" $$ movePointsWithinTurn: ADD BACK d_i= "+Units.str("NM",d_i,5)+" np_i = "+np_i);
			traj.add(np_i,tcp_i);
		}
		// ************** Add the EOT back now
		ixEOT = traj.nextEOT(ixBOT);  // some of the points that were after EOT may have been moved in front
		return ixEOT;
	}

	static boolean trackContinuousAt(Plan traj, int ix, double maxDelta) {
		double courseIn = traj.finalVelocity(ix-1).compassAngle();
		double courseOut = traj.initialVelocity(ix).compassAngle();
		//f.pln(" $$>> generateTurnTCPs: courseIn = "+Units.str("deg",courseInB)+" courseOut = "+Units.str("deg",courseOut));
		double turnDelta = Util.turnDelta(courseIn,courseOut);
		//f.pln(" $$>> generateTurnTCPs: turnDeltaBOT = "+Units.str("deg",turnDeltaBOT));
		return (turnDelta < maxDelta);
	}
	
	/** Generates ground speed TCPs
	 * 
	 * @param fp        plan to be processed
	 * @param gsAccel   ground speed acceleration
	 * @param repairGs  if true, attempt repair by delaying some of speed change to subsequent segments
	 * @return          plan with BGS-EGS pairs added
	 */
	public static Plan generateGsTCPs(Plan fp, double gsAccel, boolean repairGs, boolean useOffset) {
		Plan traj = new Plan(fp); // the current trajectory based on working
		//f.pln(" generateGsTCPs: ENTER ------------------------ fp =  "+fp.toStringFull());
		for (int i = traj.size() - 2; i > 0; i--) {
			if (repairGs) {
				boolean checkTCP = true;			
				PlanUtil.fixGsAccelAt(traj, i, gsAccel, checkTCP, getMinTimeStep());				
			}
    		double timeOffset = 0.0;	
			TcpData tcp1 = traj.getTcpData(i);
    		if (useOffset && tcp1.isEOT()) timeOffset = getMinTimeStep();
    		double targetGs = traj.gsOut(i+1);   
 		    generateGsTCPsAt(traj, i, gsAccel, targetGs, timeOffset);	
		}
		//f.pln(" generateGsTCPs: EXIT traj = "+traj);				
		return traj;
	}

	/**
	 * Generate gs acceleration zone between np1 and np2 
	 * 
	 * @param traj
	 * @param ixNp1
	 * @param gsIn
	 * @param targetGs
	 * @param a
	 * @param timeOffset    if non-zero then make sure BGS and EGS are this far from np1 and np2
	 * @return BGS, EGS and distance to EGS,  
	 */
	private static Triple<NavPoint,NavPoint,Double> gsAccelGenerator(Plan traj, int ixNp1, double gsIn, double targetGs, double a, 
			              double timeOffset, boolean allowOverlap) {
		NavPoint np1 = traj.point(ixNp1);
		NavPoint np2 = traj.point(ixNp1+1);
		String label = np1.label();
		NavPoint np1b = np1.makeLabel(label);  
		double accelTime = (targetGs - gsIn)/a;			
		if (accelTime < getMinTimeStep()) {
			//f.pln(" $$$$$ gsAccelGenerator no GS TCPS needed, at time +"+np1b.time()+" accelTime = "+accelTime);
			return new Triple<NavPoint,NavPoint,Double>(np1b,np2,0.0); // no change
		}
		double t0 = np1.time();
		//f.pln(" $$## gsAccelGenerator: at t0 = "+f.Fm2(t0)+" Accelerate FROM gsIn = "+Units.str("kn",gsIn)+" TO targetGs = "+Units.str("kn",targetGs));
		double distToBGS = gsIn*timeOffset;
		//Position bPos = traj.advanceDistanceWithinSeg2D(ixNp1,distToBGS,false); // TODO why is the alt ok?
		Position bPos = traj.advanceDistance(ixNp1,distToBGS,false).first;
		double bTm = t0+timeOffset;
		NavPoint b = new NavPoint(bPos, bTm).makeLabel(label); 
		double d = b.distanceH(np2) - targetGs*timeOffset;   // want at least timeOffset after EGS			
		double remainingDist = d - accelTime * (gsIn+targetGs)/2;
		//f.pln(" $$$$ gsAccelGenerator: accelTime = "+accelTime+" remainingDist = "+Units.str("nm",remainingDist));
		if (!allowOverlap && remainingDist <= 0) {
			traj.addError("TrajGen.generateGsTCPs ERROR:  we don't have enough distance to the next point = "+(ixNp1+1));
			return new Triple<NavPoint,NavPoint,Double>(np1b,np2,-1.0); // no change
		}
		//f.pln(" $$$$ gsAccelGenerator: remainingDist = "+Units.str("nm",remainingDist));
		double distToEGS = gsIn*accelTime + 0.5*a*accelTime*accelTime;
		NavPoint e;
		Pair<Position,Integer> pv = traj.advanceDistance(ixNp1,distToBGS+distToEGS,false);
		e = new NavPoint(pv.first, accelTime+t0+timeOffset);
		return new Triple<NavPoint,NavPoint,Double>(b,e,distToEGS);
	}
	
	
	
	public static void generateGsTCPsAt(Plan traj, int i, double gsAccel, double targetGs, double  timeOffset) {
		boolean allowOverlap = true;
		generateGsTCPsAt(traj, i, gsAccel,  targetGs,   timeOffset, allowOverlap);
	}
	
	
	public static void generateGsTCPsAt(Plan traj, int i, double gsAccel, double targetGs, double  timeOffset, boolean allowOverlap) {
		double gsIn = traj.gsIn(i);  		
		//f.pln(" $$$$$ generateGsTCPsAt:  gsIn = "+Units.str("kn",gsIn)+"  targetGs = "+Units.str("kn",targetGs));
		if (Util.almost_equals(gsIn, 0.0) || Util.almost_equals(targetGs,0.0)) {
			traj.addWarning("TrajGen.generateGsTCPsAt: zero ground speed at index "+i);
			gsIn = 1E-10;
		}	
		//f.pln(" $$%% generateGsTCPs: at i = "+i+" Accelerate FROM gsIn = "+Units.str("kn",gsIn)+" TO targetGs = "+Units.str("kn",targetGs));
		int sign = 1;
		if (gsIn > targetGs) sign = -1;
		double a = Math.abs(gsAccel)*sign; 
		Triple<NavPoint,NavPoint,Double> tcpTriple = gsAccelGenerator(traj, i, gsIn, targetGs, a, timeOffset, allowOverlap);			
		double distanceToEGS = tcpTriple.third;
		double distanceToEndOfPlan = traj.pathDistance(i,traj.size()-1);
		if (distanceToEGS > distanceToEndOfPlan) {
			//f.pln(" $$##### generateGsTCPs  ERROR: Cannot complete acceleration at i = "+i+" before end of Plan");
			traj.addError("TrajGen.generateGsTCPsAt ERROR: Cannot complete acceleration at i = "+i+" before end of Plan");
			return;
		}
		//f.pln(" $$$$$ generateGsTCPsAt: distanceToEGS = "+Units.str("nm",distanceToEGS,8));
		if (distanceToEGS >= Units.from("ft",50)) {  // do not generate tiny acceleration regions
			NavPoint BGS = tcpTriple.first;
			NavPoint EGS = tcpTriple.second;
			TcpData tcp1 = traj.getTcpData(i);
			TcpData EGS_tcp = makeEGS(tcp1);			
			if (! tcp1.isTCP()) {  // for example it is an EOT
				//f.pln(" #### generateGsTCPsAt: remove point at time "+traj.time(i)+" index="+traj.getIndex(traj.time(i)));
				traj.remove(i);  // assumes times have not changed from lpc to lpc			
			}		
			int ixEGS;
			int ixBGS;
			TcpData BGS_tcp = makeBGS(tcp1, a, timeOffset, tcp1.isAltPreserve());
			//f.pln(" $$%% generateGsTCPsAt: BGS_tcp = "+BGS_tcp);
			if (allowOverlap) {	
		        boolean isAltPreserve = tcp1.isAltPreserve();
				ixBGS = traj.add(BGS, BGS_tcp);   
				if (ixBGS < 0) {
					//f.pln(" $$##### generateGsTCPs: ixBGS = "+ixBGS+" BGS.t = "+BGS.time());
					traj.addError("TrajGen.generateGsTCPsAt ERROR: Overlap Problem at i = "+i+" ");
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
	private static void adjustGsInsideAccel(Plan traj, int ixBGS, double aBGS, double distToEGS, double timeOffset, boolean isAltPreserve) {
		//f.pln(" $$$$$ generateGsTCPsAt(1): ixBGS = "+ixBGS+" traj = "+traj.toStringGs());
		//f.pln(" $$$ adjustGsInsideAccel: distToEGS = "+Units.str("nm",distToEGS,8));
		double gsBGS = traj.gsIn(ixBGS);
		int lastIxInGsAccel = ixBGS;
		traj.setGsAccel(ixBGS,0.0);                         // *** THIS IS IMPORTANT --> isGsContinuous test needs this
		for (int ii = ixBGS+1; ii < traj.size(); ii++) {
			double dist_to_ii =  traj.pathDistance(ixBGS,ii);
			//f.pln(" $$$$$ adjustGsInsideAccel: ii = "+ii+" dist_to_ii = "+Units.str("nm",dist_to_ii,8));
			if (dist_to_ii < distToEGS) lastIxInGsAccel = ii;
			else break;
			if (!PlanUtil.isGsContinuous(traj, ii, Units.from("kn",5.0), true)) { // *** ACCELERATION NEEDS TO BE OFF HERE ****
				//f.pln("$$$$$ TrajGen.generateGsTCPsAt: passed over a ground speed discontinuity at "+ii);
				traj.addError("TrajGen.generateGsTCPsAt: passed over a ground speed discontinuity at "+ii);
			}
		}
		//f.pln("\n $$$$$>>> adjustGsInsideAccel:  lastIxInGsAccel = "+lastIxInGsAccel);
		double tBGS = traj.time(ixBGS);	
		traj.timeShiftPlan(ixBGS+1,1000); 
		traj.setGsAccel(ixBGS,aBGS);               // **************** TURN ON ACCELERATION NOW *******************
		for (int j = ixBGS+1; j <= lastIxInGsAccel; j++) {
			double d_j = traj.pathDistance(ixBGS,j);
			double t_j = tBGS + Plan.timeFromDistance(gsBGS, aBGS, d_j);
			traj.setTime(j,t_j);
			//f.pln(" $$$$$>>> adjustGsInsideAccel: set time of j = "+j+" to "+t_j);
		}
	}

	private static TcpData makeBGS(TcpData tcp1, double a, double timeOffset, boolean isAltPreserve) {
		TcpData BGS_tcp; 
		if (timeOffset > 0) 
			BGS_tcp = new TcpData();
		else 
			BGS_tcp = tcp1.copy();
		if (isAltPreserve) BGS_tcp.setAltPreserve();
		BGS_tcp.setSource(tcp1.getSourcePosition(),tcp1.getSourceTime());
		int linearIndex = tcp1.getLinearIndex();
		BGS_tcp.setBGS(a, linearIndex);	
		return BGS_tcp;
	}
	
	private static TcpData makeEGS(TcpData tcp1) {
		TcpData EGS_tcp = new TcpData();
		EGS_tcp.setSource(tcp1.getSourcePosition(),tcp1.getSourceTime());
		int linearIndex = tcp1.getLinearIndex();
		EGS_tcp.setEGS(linearIndex);	
		return EGS_tcp;
	}


	

	/**       Generate vertical speed TCPs centered around np2.  Compute the absolute times of BVS and EVS
	 * @param t1 = previous point's time (will not place tbegin before this point)
	 * @param t2 end time of the linear segment where the vertical speed change occurred
	 * @param tLast = time of the end of the plan 
	 * @param vs1 vertical speed into point at time t2
	 * @param vs2 vertical speed out of point at time t2
	 * @param vsAccelDef non negative (horizontal) acceleration
	 * 
	 *                             vs1                              vs2
	 *             t1  -------------------------------- t2 ------------------------------ tLast
	 *                                            ^           ^
	 *                                            |           |
	 *                                          tbegin       tend
	 *                                            
	 * @return First two components are tbegin,  tend, the beginning and ending times of the acceleration zone.
	 *         The third component: the acceleration time.  If -1 => not feasible, >= 0 the acel time needed, 0 = no accel needed
	 * NOTE!!!: This has been changed so that the accel time is always returned (or at least an estimate), 
	 *          but no zone is made if it is < getMinTimeStep() and this needs to be handled in the calling function!!!!
	 */
	static Triple<Double,Double,Double> vsAccelGenerator(double t1, double t2, double tLast, double vs1, double vs2, double a) {
		// we want the velocities around n2, so v1 is in the opposite direction, then reversed
		//f.pln("\n------------------\n$$# vsAccelGenerator:  t1  = "+t1+" t2 = "+t2+" tLast = "+tLast+" a = "+a);
		double deltaVs = vs1 - vs2;
		double accelTime = Math.abs(deltaVs/a);
		double tbegin = 0;
		double tend = 0;
		//f.pln("### vsAccelGenerator: vs1 = "+Units.str("fpm",vs1)+" vs2 = "+Units.str("fpm",vs2));
		if (accelTime > 0) {
			tbegin = t2 - accelTime/2.0;
			tend = tbegin + accelTime;	
			//f.pln("### vsAccelGenerator: dt = "+dt+" tbegin = "+tbegin);
			if (tbegin < t1 || tend > tLast) {
				accelTime = -1;
			}
		}
		return new Triple<Double,Double,Double>(tbegin,tend,accelTime);
	}

	
	/** calculate tbegin, tend for BVS and EVS
	 * 
	 * @param i             index of point where BVS-EVS pair should be generated
	 * @param traj          plan file
	 * @param vsAccel       vertical acceleration

	 * @return  tbegin, tend, signed acceleration ( -1 values indicate no new tcp is needed)
	 */
	private static Triple<Double,Double,Double> calcVsTimes(int i, Plan traj, double vsAccel, boolean continueGen) {
		if (vsAccel == 0) {
			traj.addError("TrajGen.generateVsTCPs ERROR: vsAccel = "+vsAccel);
			return new Triple<Double,Double,Double>(-1.0,-1.0,-1.0);
		}
		NavPoint np1 = traj.point(i-1);
		NavPoint np2 = traj.point(i);
		NavPoint np3 = traj.point(i+1);
		// find next point where vertical speed changes
		double nextVsChangeTm = traj.getLastTime();
		double targetVs = traj.vsOut(i);
		for (int j = i+1; j < traj.size(); j++) {
			nextVsChangeTm = traj.time(j);
			double vs_j = traj.vsOut(j);
			double dt = Kinematics.vsAccelTime(vs_j, targetVs, vsAccel);
			if (dt > getMinTimeStep()) break;
		}		
		//f.pln(" $$>> calcVsTimes: np2 = "+np2+" np3 = "+np3+" dt23 = "+(np3.time()-np2.time()));
		double vs1 = np1.verticalSpeed(np2);
		double vs2 = np2.verticalSpeed(np3);
		//f.pln(" $$>> calcVsTimes: vs1 = "+Units.str("fpm",vs1)+" vs2 = "+Units.str("fpm",vs2));
		int sign = 1;
		if (vs1 > vs2) sign = -1;
		double a = vsAccel*sign;
		double prevEndTime = traj.getFirstTime();
		Triple<Double,Double,Double> vsTriple = vsAccelGenerator(prevEndTime, np2.time(), nextVsChangeTm, vs1, vs2, a);	
		double accelTime = vsTriple.third;
		if (accelTime  < 0) { // 
			// we have a calculation error, possibly too short a time for the needed acceleration
			if (!continueGen) {
				traj.addError("TrajGen.generateVsTCPs ERROR: Insufficient room i = "+i+" for vertical accel! tbegin = "+f.Fm1(vsTriple.first)+" prevEndTime = "+f.Fm1(prevEndTime));
				printError("TrajGen.generateVsTCPs ERROR: Insufficient room at i = "+i+" for vertical accel! tbegin = "+f.Fm1(vsTriple.first)+" prevEndTime = "+f.Fm1(prevEndTime));
			}
			return new Triple<Double,Double,Double>(-1.0,-1.0,-1.0);
		} else if (accelTime > MIN_ACCEL_TIME) {  // SUCCESS!
			return new Triple<Double,Double,Double>(vsTriple.first, vsTriple.second,a);
		} else { // NO TCPs needed
			return new Triple<Double,Double,Double>(-1.0,-1.0,-1.0);  
		}		
}
	

	/** Generate Vertical acceleration TCPs
	 *  It assumes that all horizontal passes have been completed.
	 *  
	 * @param kpc      kinematic plan with final horizontal path
	 * @param vsAccel  vertical speed acceleration
	 * @return
	 */
	public static Plan generateVsTCPs(Plan kpc, double vsAccel, boolean continueGen) {
		Plan traj = new Plan(kpc);
		//f.pln("$$>> generateVsTCPs: ENTER  traj = "+traj);	
		for (int i = 1; i < traj.size()-1; i++) {
			NavPoint np1 = traj.point(i-1);
			NavPoint np2 = traj.point(i);
			TcpData tcp2 = traj.getTcpData(i);
			//f.pln("$$>> generateVsTCPs: i = "+i+" tm = "+np2.time());
			double vs1 = np1.verticalSpeed(np2);
			// we want the velocities around n2, so v1 is in the opposite direction, then reversed
			Triple<Double,Double,Double> cboTrip = calcVsTimes(i, traj, vsAccel, continueGen);
			double tbegin = cboTrip.first;
			double tend = cboTrip.second;										
			double accel = cboTrip.third;
			boolean newTCPneeded = (tbegin >= 0 && tend >= 0 && !traj.inVsChange(tbegin)); 
			//f.pln("$$$$ generateVsTCPs: i = "+i+" tbegin = "+f.Fm2(tbegin)+" tend = "+f.Fm2(tend)+" np1 = "+np1.time()+" np2 = "+np2.time() );
			if (tbegin >= 0 && traj.inVsChange(tbegin)) {    // See testGen2
				//f.pln(i+" $$!!!!!!!!!!!!! "+kpc.getName()+" generateVsTCPs:  traj.inVsChange("+f.Fm2(tbegin)+") = "+traj.inVsChange(tbegin)+"   traj.inVsChange("+f.Fm2(tend)+") = "+traj.inVsChange(tend));
				if (continueGen) {
					continue;
				} else {
					traj.addError("Vertical Speed region overlaps at time "+f.Fm1(tbegin));
					return traj;
				}
			}
			if (tbegin > 0 && np1.time() > tbegin && traj.isEVS(i-1)) {   // see test "test_AWE918"
				if (continueGen) {
					continue;
				} else {
					//f.pln(" $$$ generateVsTCPs: ERROR: i = "+i+" tbegin = "+f.Fm2(tbegin)+" np1.time() = "+f.Fm2(np1.time()));
					traj.addError("Vertical Speed regions overlap at time "+f.Fm1(tbegin));
					return traj;
				}
			}
			if (newTCPneeded) {	
				//f.pln(i+" $$ generateVsTCPs: $$ generateVsTCPs: tbegin = "+f.Fm2(tbegin)+" tend = "+f.Fm2(tend)+" np1 = "+np1.time()+" np2 = "+np2.time() );
				String label = np2.label();
				NavPoint np2Source = traj.sourceNavPoint(i);
               int linearIndex = tcp2.getLinearIndex();
                Pair<NavPoint,TcpData> bPair = Plan.makeBVS(np2Source, traj.position(tbegin), tbegin, accel, linearIndex);
				NavPoint b = bPair.first.makeLabel(label); // .makeAdded();//.appendName(setName);
				bPair.second.setInformation(traj.getInfo(i));
				Pair<NavPoint,TcpData> ePair = Plan.makeEVS(np2Source, traj.position(tend), tend, linearIndex);
				NavPoint e = ePair.first;
				//f.pln(" $$ generateVsTCPs: i = "+i+"  b ="+b.toString()+" TCP: "+bPair.second);
				//f.pln(" $$ generateVsTCPs: i = "+i+"  e ="+e.toString()+" TCP: "+ePair.second);
				if ( ! traj.isTCP(i)) {
					traj.remove(i);  // remove the original vs-transition point
					//f.pln(" $$ ^^^^^^^ altPreserve: "+np0.isAltPreserve()+" generateVsTCPs: REMOVE i = "+i+"  "+np0.toStringFull());
				}
				int bindex = traj.add(b, bPair.second);
				int eindex = traj.add(e, ePair.second);
				//DebugSupport.dumpPlan(traj,"AFTER");
				if (bindex < 0) {
					bindex = -bindex;   // There was an overlap
				}
				if (eindex < 0) {
					eindex = -eindex;   // There was an overlap
				}
				i = eindex;
				// fix altitude for all remaining points between b and e: altitudes should be between b.alt and e.alt
				boolean altok = true;
				for (int k = bindex+1; k < eindex; k++) {
					double dt2 = traj.time(k) - tbegin;
					Pair<Double,Double> npair = vsAccel(b.alt(), vs1, dt2, accel);
					double newAlt = npair.first;	
					//f.pln(" $$$$ SET: k = "+k+"  dt2 = "+dt2+"  newAlt = "+Units.str("ft",newAlt));
					NavPoint newNp = traj.point(k).mkAlt(newAlt);
					TcpData newNp_tcp = traj.getTcpData(k);
					//f.pln(" $$ generateVsTCPs: traj.finalVelocity(k-1) = "+traj.finalVelocity(k-1));
					if (newAlt >= 0 && newAlt <= maxAlt) {
						traj.set(k, newNp, newNp_tcp);
					} else {
						altok = false;
					}
				}//for
				if (!altok) {  // only create one error message
					traj.addError("TrajGen.generateVsTCPs: generated altitude is out of bounds");
				}
			} else {
				//f.pln(" $$ generateVsTCPs:   FAILED TO GENERATE Vertical TCPS at i = "+i);	
			}
		} // for loop
		//f.pln(" $$ generateVsTCPs: EXIT traj = "+traj);
		return traj;
	} // generateVsTCPs


	private static Pair<Double,Double> vsAccel(double sz, double vz,  double t, double a) {
		double nvz = vz + a*t;
		double nsz = sz + vz*t + 0.5*a*t*t;
		return new Pair<Double,Double>(nsz,nvz);
	}


	/**
	 * Repair function for lpc.  This can eliminate short legs in the lpc.  This may alter the initial velocity of the plan
	 * @param fp
	 * @param bank
	 * @param addMiddle if true, it adds a middle point when it deletes two points
	 * @return
	 */
	public static Plan linearRepairShortTurnLegs(Plan fp, double bank, boolean addMiddle) {
		Plan npc = fp.copy();
		for (int j = 0; j+3 < fp.size(); j++) {
			NavPoint p0 = fp.point(j);
			NavPoint p1 = fp.point(j+1);
			NavPoint p2 = fp.point(j+2);
			NavPoint p3 = fp.point(j+3);
			Velocity vf0 = fp.finalVelocity(j);
			Velocity vi1 = fp.initialVelocity(j+1);
			Velocity vf1 = fp.finalVelocity(j+1);
			Velocity vi2 = fp.initialVelocity(j+2);
			double deltaTrack1 = Math.abs(vf0.trk() - vi1.trk());
			double deltaTrack2 = Math.abs(vf1.trk() - vi2.trk());
			//f.pln(j+"#### removeShortLegsBetween: deltaTrack1 = "+Units.str("deg",deltaTrack1)+" deltaTrack2 = "+Units.str("deg",deltaTrack2));
			if (deltaTrack1> Units.from("deg",1.0) && deltaTrack2 > Units.from("deg",1.0)) {
				double gs1 = fp.initialVelocity(j).gs();
				double R1 = Kinematics.turnRadius(gs1,bank);
				double gs2 = fp.initialVelocity(j+1).gs();
				double R2 = Kinematics.turnRadius(gs2,bank);
				int linearIndex1 = fp.getTcpData(j+1).getLinearIndex();
				int linearIndex2 = fp.getTcpData(j+2).getLinearIndex();
				Tuple5<NavPoint,NavPoint,NavPoint,Integer,Position> turn1 = TurnGeneration.turnGenerator(p0, p1, linearIndex1, p2, R1);
				Tuple5<NavPoint,NavPoint,NavPoint,Integer,Position> turn2 = TurnGeneration.turnGenerator(p1, p2, linearIndex2, p3, R2);  // should this use R1 ~ gs1 ??
				NavPoint A = turn1.third;
				NavPoint B = turn2.first;	
				//f.pln(" $$$#####linearRepairShortTurnLegs: A.time = "+A.time()+" B.time() = "+B.time());
				double distP1EOT = p1.distanceH(A);
				double distP1BOT = p1.distanceH(B);
				//f.pln(j+" ######### removeShortLegsBetween: distP1BOT = "+Units.str("nm",distP1BOT)+" distP1EOT = "+Units.str("nm",distP1EOT));
				if (A.time() > B.time() || distP1EOT > distP1BOT ) {
					//f.pln(" $$.. linearRepairShortTurnLegs REMOVE npc.point(j+2) = "+npc.point(j+2)+ " npc.point(j+1) = "+npc.point(j+1));
					npc.remove(j+2);	
					npc.remove(j+1);	
					if (addMiddle) {
						Position mid = p1.position().midPoint(p2.position());
						double tmid =npc.time(j)+  mid.distanceH(p0.position())/gs1;
						//f.pln(" tmid = "+tmid);
						NavPoint midNP = p1.makePosition(mid).makeTime(tmid); // preserve source info
						//f.pln("............ ADD removeShortLegsBetween: midNP = "+midNP.toStringFull());
						npc.addNavPoint(midNP);
						npc =  PlanUtil.linearMakeGSConstant(npc, j,j+2,gs1);
					} else {
						double dist = p3.distanceH(p0);
						double dt = dist/gs1;
						double t0 = p0.time();
						//f.pln(" ######### t0 = "+t0+" dt = "+dt+" j+1 = "+(j+1)+" gs1 = "+Units.str("kn",gs1)+" dist = "+Units.str("nm",dist));
						npc.setTime(j+1,t0+dt);
					}
					j = j+2;
				}
			}
		}
		//f.pln(" removeShortLegs: npc = "+npc);
		return npc;
	}


	/**
	 * Repair function for lpc.  Removes vertex point if turn is infeasible.
	 * @param fp
	 * @param bankAngle
	 * @return
	 */
	private static Plan removeInfeasibleTurns(Plan fp, double bankAngle, boolean strict) {  
		Plan traj = fp.copy(); // the current trajectory based on working
		//int j = 1; // traj index corresponding to i in fp
		for (int i = fp.size() - 2; i > 0; i--) {
			//double tm = fp.getTime(i);
			Velocity vf0 = fp.finalVelocity(i-1);
			Velocity vi1 = fp.initialVelocity(i);
			if (Util.almost_equals(vi1.gs(),0.0)) {
				f.pln(" ###### WARNING: Segment from point "+(i)+" to point "+(i+1)+" has zero ground speed!");
			}
			if (Math.abs(vf0.trk() - vi1.trk()) > Units.from("deg",1.0) && bankAngle >= 0.0) {
				NavPoint np1 = fp.point(i-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
				NavPoint np2 = fp.point(i);
				NavPoint np3 = fp.point(i + 1);
				double gs = vf0.gs(); 
				double R = Kinematics.turnRadius(gs, bankAngle);
				//f.pln(" $$>> generateTurnTCPs: t="+np2.time()+"   gs="+Units.to("knot", gs)+"   R = "+Units.str8("nm",R));
				int linearIndex = fp.getTcpData(i).getLinearIndex();
				Tuple5<NavPoint,NavPoint,NavPoint,Integer,Position> tg = TurnGeneration.turnGenerator(np1,np2,linearIndex,np3,R);	  
				NavPoint BOT = tg.getFirst(); //.appendName(setName);
				NavPoint MOT = tg.getSecond();//.appendName(setName);
				NavPoint EOT = tg.getThird(); //.appendName(setName);
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
						//f.pln(" $$$$ removeInfeasibleTurns: remove point i = "+i);
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
	 * Repair function for lpc.  Removes vertex point if turn is infeasible.
	 * @param kpc
	 * @param bankAngle
	 * @return plan without infeasible turns
	 */
	public static Plan removeInfeasibleTurnsOver(Plan kpc, double bankAngle) {
		Plan traj = kpc.copy(); // the current trajectory based on working
		//int j = 1; // traj index corresponding to i in fp
		for (int i = 1; i+1 < kpc.size(); i++) {
			//double tm = kpc.getTime(i);
			Velocity vin =  traj.finalVelocity(i-1);          //         Velocity vin = kpc.finalVelocity(i-1);	
			Velocity vi1 = traj.initialVelocity(i);
			if (Util.almost_equals(vi1.gs(),0.0)) {
				printError(" ###### WARNING: Segment from point "+(i)+" to point "+(i+1)+" has zero ground speed!");
			}
			double turnTime = Kinematics.turnTime(vin.gs(),Util.turnDelta(vin.trk(), vi1.trk()),bankAngle);
			//f.pln(" $$>> removeInfeasibleTurnsOver: for i = "+i+" turnTime = "+turnTime);
			if (turnTime >= getMinTimeStep()) {
				NavPoint np = traj.point(i);
				//f.pln("\n removeInfeasibleTurnsOver:  i = "+i+" np = "+np.toStringFull()+" vin = "+vin+" vi1 = "+vi1);
				Position so = np.position();
				double   to = traj.point(i).time();    // lpc(i) time no longert valid
				//f.pln(" $$$ removeInfeasibleTurnsOver: to = "+to+" traj.point(i).time() = "+ traj.point(i).time());
				NavPoint wp = traj.point(i+1);
				Position wpp = wp.position();               
				double R = Kinematics.turnRadius(vin.gs(), bankAngle);                
				Quad<Position,Velocity,Double,Integer> dtp = ProjectedKinematics.directToPoint(so,vin,wpp,R);
				//f.pln(" $$$ removeInfeasibleTurnsOver: so="+so+ " vin="+vin+" wpp="+wpp);
				//f.pln(" $$$ removeInfeasibleTurnsOver: dtp="+dtp);	    
				if (dtp.third < 0) {
					f.pln(" removeInfeasibleTurnsOver: REMOVE !!!!!!!!!! traj.point("+(i+1)+") = "+traj.point(i+1));
					traj.remove(i+1);
					//i--;                  // potentially remove more than one point 
				} else {
					double turnTm = dtp.third;
					//f.pln(" $$$ removeInfeasibleTurnsOver: turnTm = "+turnTm); 
					NavPoint vertexNP = new NavPoint(dtp.first,to+turnTm);
					//f.pln(" $$$ removeInfeasibleTurnsOver: ADD vertexNP = "+vertexNP+" dtp.second = "+dtp.second+" dtp.third = "+dtp.third);
					if (wp.time() < vertexNP.time()) {
						//f.pln(" $$$$$$ removeInfeasibleTurnsOver:  not enough time !!!!!!  "+wp.time()+"  < +"+vertexNP.time());      			 
						f.pln(" removeInfeasibleTurnsOver: REMOVE >>>>>>>>>> traj.point("+(i+1)+"( = "+traj.point(i+1));
						traj.remove(i+1);
						//i--;               // potentially remove more than one point 
					}
				}	
			}
		}//for
		return traj;
	}



	/**
	 * Ff there is a segment with a ground speed change at point j that cannot be achieved with the gsAccel value, then it
	 * makes the ground speed before and after j the same (i.e. averages over two segments)
	 * 
	 * @param fp         plan to be repaired
	 * @param gsAccel
	 * @return
	 */
	private static Plan linearRepairShortGsLegs(Plan fp, double gsAccel) {
		Plan lpc = fp.copy();
		//f.pln(" $$ smoothShortGsLegs: BEFORE npc = "+lpc.toString());
		for (int j = 1; j < lpc.size()-1; j++) {
			Velocity vin = lpc.finalVelocity(j-1);
			double	targetGs = lpc.gsOut(j);
			int sign = 1;
			double gs1 = vin.gs();
			if (gs1 > targetGs) sign = -1;
			double a = Math.abs(gsAccel)*sign;
			double accelTime = (targetGs - gs1)/a;
			//f.p(j+" $$$$$$ smoothShortGsLegs: accelTime = "+accelTime+"  dt = "+f.Fm2(dt));
			//f.pln(" gs1 = "+Units.str("kn",gs1)+"  targetGs = "+Units.str("kn",targetGs));
			double d = lpc.pathDistance(j-1,j) - targetGs*getMinTimeStep(); 
			double remainingDist = d - accelTime * (gs1+targetGs)/2;
			//f.pln(" $$>> smoothShortGsLegs: remainingDist = "+Units.str("nm",remainingDist));
			if (accelTime > getMinTimeStep() && remainingDist <= 0) {
	             f.pln(" REPAIRED GS ACCEL PROBLEM AT j = "+j+" time = "+lpc.point(j).time());
				lpc = PlanUtil.linearMakeGSConstant(lpc, j-1,j+1);
			}
		}
		//f.pln(" $$>> smoothShortGsLegs: AFTER npc = "+lpc.toString());
		return lpc;
	}


	/**
	 * Attempts to repair plan with infeasible vertical points by averaging vertical speeds over 2 segments.
	 * Assumes linear plan input.
	 * @param fp
	 * @param vsAccel
	 * @param minVsChangeRequired
	 * @return
	 */
	private static Plan linearRepairShortVsLegs(Plan fp, double vsAccel) {
		if (!fp.isLinear()) {
			fp.addError("TrajGen.smoothShortVsLegs should only be called on linear plans");
			return fp;
		}
		Plan lpc = fp.copy();
		for (int i = 1; i < fp.size()-1; i++) {
			boolean continueGen = false;
			Triple<Double,Double,Double> cboTrip = calcVsTimes(i, lpc, vsAccel, continueGen);
			double tbegin = cboTrip.first;
			double tend = cboTrip.second;
			double accel = cboTrip.third;
			boolean repairNeeded = false;
			if ((tbegin < 0.0 || tend < 0.0) && accel != 0.0) repairNeeded = true;	
            if (repairNeeded || (tbegin >= 0.0 && tbegin <= lpc.getFirstTime()) || tend >= lpc.getLastTime() ) {
    			//f.pln(" $$@@ linearRepairShortVsLegs: i = "+i+" tbegin = "+tbegin+" tend = "+tend+" accel="+accel+" "+repairNeeded);
            	PlanUtil.linearMakeVsConstant(lpc, i-1,i+1);
 	           	//f.pln(" $$^^ REPAIRED VS ACCEL PROBLEM AT  i = "+i+" time = "+lpc.point(i).time());	           
            }		
		}
		//f.pln(" $$ linearRepairShortVsLegs: AFTER CALL, RETURN lpc = "+lpc.toString());
		return lpc;
	}


	// Make vertical speeds constant by adjusting altitudes between wp1 and wp2
	// Assume there are no vertical speed accelerations
	// do not alter wp1 and wp2
	/**
	 * Penultimate Kinematic generation pass.  Repairs altitudes so that only VSC points have vertical speed changes.
	 * Averages vertical speeds between alt preserved points by modifying altitudes on points between them.
	 * @param kpc
	 * @return
	 */
	static public Plan makeMarkedVsConstant(Plan kpc) {
		Plan traj = new Plan(kpc);
		int prevIndex = 0;
		//f.pln(" makeMarkedVsConstant: kpc = "+kpc);
		for (int i = 1; i < traj.size(); i++) {
			NavPoint currFixedAlt = traj.point(i);
			TcpData currFixedAltTcp = traj.getTcpData(i);
			//f.pln(" makeMarkedVsConstant: for i = "+i+" AltPreserve = "+currFixedAlt.isAltPreserve());
			if (traj.isAltPreserve(i) || i == traj.size()-1) {
				NavPoint prevFixedAlt = traj.point(prevIndex);
				//f.pln("makeMarkedVsConstant: Altitude at point i = "+i+" IS FIXED at "+Units.str("ft",prevFixedAlt.alt(),4));
				double constantVs = (currFixedAlt.alt() - prevFixedAlt.alt())/(currFixedAlt.time() - prevFixedAlt.time());
				//f.pln("makeMarkedVsConstant: end of segment: "+i+" vs="+Units.to("fpm",constantVs));
				// fix all points between the two fixed altitude in points
				for (int j = prevIndex+1; j < i; j++) {
					NavPoint np = traj.point(j);
                    //f.pln("TrajGen.makeMarkedVsConstant inner before futzing j="+j+" np="+np.toStringFull());					
					double dt = np.time() - prevFixedAlt.time();
					double newAlt = prevFixedAlt.alt() + constantVs*dt;
					np = np.mkAlt(newAlt);
					TcpData tcp = traj.getTcpData(j);
					if (traj.isTCP(j)) {
						//f.pln(" $$>> makeMarkedVsConstant: UPDATE vin: "+np.velocityIn().mkVs(constantVs));
						//tcp = tcp.setVelocityInit(traj.initialVelocity(j).mkVs(constantVs));
					}
					//f.pln(" $$ makeMarkedVsConstant: np("+j+") = "+np.toString());
					traj.set(j,np,tcp);
				}
				// fix the final, fixed altitude point
				prevIndex = i;
//				if (currFixedAltTcp.isTCP()) {
//					currFixedAltTcp = currFixedAltTcp.setVelocityInit(traj.initialVelocity(i).mkVs(constantVs));
//				}
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



	/**

	 * Attempts to compute (heuristic) the amount of time to continue current velocity before starting direct to turn.  (in 2-D)
	 * @param lpc Plan trying to connect to at point zero.
	 * @param so current position
	 * @param vo current velocity
	 * @param to current time
	 * @param bankAngle max bank angle
	 * @return
	 */
	static private double directToContinueTime(Plan lpc, Position so, Velocity vo, double to, double bankAngle) {
		final double extraProjTime = 1.0;
		NavPoint np0 = lpc.point(0);
		double gs0 = vo.gs();
		double dist1 = so.distanceH(np0.position());
		double nominalTime = dist1/gs0;
		double deltaTrack = Util.turnDelta(lpc.initialVelocity(0).trk(),vo.trk());
		double estTurnTime = Kinematics.turnTime(gs0,deltaTrack,bankAngle);
		f.pln(" #### genDirectTo nominalTime = "+f.Fm1(nominalTime)+" deltaTrack = "+Units.str("deg",deltaTrack)+" estTurnTime  = "+f.Fm1(estTurnTime));
		//double deltaGs = Math.abs(vin.gs() - gs2);
		double dt = np0.time() - to;
		double continueTm = Util.min(estTurnTime+extraProjTime,dt/2);
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
	static Plan genDirect2(Plan fp, Position so, Velocity vo, double to, double bankAngle) {
		Plan lpc = fp.copy();
		if (lpc.size() == 0) {
			f.pln(" genDirectTo: ERROR empty plan provides no merge point!");
			return lpc;	
		}
		double continueTm = directToContinueTime(lpc,so,vo,to,bankAngle);
		if (continueTm >  (lpc.point(0).time() - to) && lpc.size() > 1) {
			f.pln(" remove first point from plan -- not enough time");
			lpc.remove(0);
			continueTm = directToContinueTime(lpc,so,vo,to,bankAngle);
		}
		NavPoint npNew = new NavPoint(so.linear(vo,continueTm), to+continueTm);
		NavPoint npNew0= new NavPoint(so, to);
		int idx = lpc.addNavPoint(npNew0);
		if (idx < 0)
			f.pln(" genDirectTo: ERROR -- could not add direct to point!");
		//f.pln(" #### genDirectTo0: lpc = "+lpc+" idx = "+idx);
		idx = lpc.addNavPoint(npNew);
		//f.pln(" #### genDirectTo1: lpc = "+lpc+" idx = "+idx);
		//lpc = lpc.makeSpeedConstant(0,2);
		return lpc;
	}


	public static Plan genDirectToPoint(String id, Position so, Velocity vo, double to, Position goal, double bankAngle, double timeBeforeTurn) {
		Plan lpc = new Plan(id);
		NavPoint np0 = new NavPoint(so,to);
		lpc.addNavPoint(np0);
		Triple<Position,Double,Double> trip = ProjectedKinematics.genDirectToVertex(so, vo, goal, bankAngle, timeBeforeTurn);
		if (trip.second < 0 || trip.third < 0) {
			lpc.addError("TrajGen.genDirectToPoint: could not generate maneuver");
			return lpc;
		}
		NavPoint vertex = new NavPoint(trip.first, to+trip.second);
		double vet = trip.first.distanceH(goal)/vo.gs();
		if (trip.second + vet < trip.third) {
			lpc.addError("TrajGen.genDirectToPoint: turn takes too long "+trip.second);
			return lpc;
		}

		NavPoint end = new NavPoint(goal,vertex.time()+vet);
		lpc.addNavPoint(vertex);
		lpc.addNavPoint(end);
		return lpc;
	}



	/**
	 * Constructs a new linear plan that connects current state to existing linear plan lpc (in 2-D).
	 * @param fp     a linear plan that needs to be connected to at point 0
	 * @param so     current position
	 * @param vo     current velocity
	 * @param to     current time
	 * @param bankAngle  turn bank angle
	 * @param timeBeforeTurn delay time before continuing turn.  (can call directToContinueTime for an estimate)
	 * @return new linear plan with (so,vo,to) added as a first point and a leadIn point added
	 * If we cannot connect to point zero of lpc this returns a plan with its error status set.
	 * The resulting plan may be infeasible from a kinematic view.
	 */
	public static Plan genDirectToLinear(Plan fp, Position so, Velocity vo, double to, double bankAngle, double timeBeforeTurn) {
		Plan lpc = fp.copy();
		Position wpp = lpc.point(0).position();
		NavPoint new0 = new NavPoint(so,to);
		//Plan tmp = lpc.copy();
		//tmp.add(new0);
		//DebugSupport.dumpPlan(tmp, "bleh");
		//f.pln("genDirectToLinear: so="+so.toString4()+" vo="+vo.toString()+" to="+to+" timebefore="+timeBeforeTurn);
		Triple<Position,Double,Double> vertTriple = ProjectedKinematics.genDirectToVertex(so, vo, wpp, bankAngle, timeBeforeTurn);
		double newTime = to+vertTriple.second;
		//f.pln(" $$$$$$$$$$$ genDirectToLinear: to = "+to+" newTime = "+newTime);
		double initialTime = lpc.point(0).time();
		lpc.addNavPoint(new0);
		//f.pln("genDirectToLinear lpc1="+lpc);
		if (Double.isNaN(vertTriple.second) || vertTriple.first.isInvalid()) { // turn is not achievable                               // THIS TEST MAY NOT BE NECESSARY
			lpc.addError("TrajGen.genDirectToLinear: the required turn radius is larger than can reach target point!",1);
		} else if (newTime > initialTime){
			//f.pln(" $$$$ genDirectTo: newTime = "+newTime+" initialTime = "+initialTime);
			lpc.addError("TrajGen.genDirectToLinear: the time of the vertex point exceeds the first point of lpc",1);
		} else if (vertTriple.third < 0 || newTime < 0) {
			lpc.addError("TrajGen.genDirectToLinear: could not generate direct to",1);
		} else {
			//f.pln(" $$$$$>>>>>>>>>.. vertTriple.second = "+vertTriple.second);
			NavPoint vertexNP = new NavPoint(vertTriple.first,newTime);
			//f.pln(" $$$ genDirectTo: ADD verstexNP = "+vertexNP+" vertTriple.second = "+vertTriple.second+" vertTriple.third = "+vertTriple.third);
			lpc.addNavPoint(vertexNP); 
			//lpc.add(eotNP); 
		}
		//f.pln("genDirectToLinear lpc2="+lpc);
		return lpc;
	}


	/**
	 * Creates a kinematic plan that connects a given state to an existing plan fp
	 * @param fp    a plan that needs to be connected to at point 0
	 * @param so     current position
	 * @param vo     current velocity
	 * @param to     current time
	 * @param bankAngle  turn bank angle
	 * @param gsAccel gs accel
	 * @param vsAccel vs accel
	 * @param timeBeforeTurn delay time before continuing turn.  (can call directToContinueTime for an estimate)
	 * @return new kinematic plan with (so,vo,to) added as a first point
	 * If we cannot connect to point zero of fp or the kinematic generation fails this returns a plan with its error status set.
	 * The resulting plan may be infeasible from a kinematic view.
	 */
	static Plan genDirectTo(Plan fp, Position so, Velocity vo, double to, 
			double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn) {
		double minVsChangeRecognized = vsAccel*getMinTimeStep(); // this is the minimum change all
		return genDirectTo( fp,  so,  vo,  to, bankAngle,  gsAccel,  vsAccel,  minVsChangeRecognized,  timeBeforeTurn);
	}

	static Plan genDirectTo(Plan fp, Position so, Velocity vo, double to, 
			double bankAngle, double gsAccel, double vsAccel, double minVsChangeRecognized, double timeBeforeTurn) {
		Plan lpc = fp.copy();
		if (!fp.isLinear()) {
			lpc = PlanUtil.revertTCPs(fp);
		} 
		Plan lpc2 = genDirectToLinear(lpc,so,vo,to,bankAngle,timeBeforeTurn);
		Plan kpc = makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, false, true, true);
		//f.pln("genDirectTo fp="+fp.getFirstTime()+" lpc="+lpc.getFirstTime()+" lpc2="+lpc2.getFirstTime()+" kpc="+kpc.getFirstTime());
		// you need to guard initialVelocity() calls!
		if (lpc2.size() < 2) {
			kpc.addError("TrajGen.genDirectTo: lpc2 size < 2");
			return kpc;
		} else if (kpc.size() < 4) {
			kpc.addError("TrajGen.genDirectTo: kpc size < 4");
			return kpc;
		}
		boolean twoTurnOverlap = Util.turnDelta(lpc2.trkOut(1),kpc.trkOut(3)) > Units.from("deg",10.0);
		if (twoTurnOverlap) {
			//   			printError(" $$$ genDirectTo: Turns Overlap");
			kpc.addError("TrajGen.genDirectTo: Turns Overlap",1);
		}
		if (lpc2.hasError()) {
			//f.pln(" $$$$$$$$$ genDirectTo: GENERATION ERROR: "+lpc2.getMessage());
			kpc.addError(lpc2.getMessage(),0);
		} 
		return kpc;
	}

	/**
	 * Creates a kinematic plan that connects a given state to an existing plan fp
	 * @param fp    a plan that needs to be connected at first position in plan it can feasibly reach
	 * @param so     current position
	 * @param vo     current velocity
	 * @param to     current time
	 * @param bankAngle  turn bank angle
	 * @param gsAccel gs accel
	 * @param vsAccel vs accel
	 * @param timeBeforeTurn delay time before continuing turn.  (can call directToContinueTime for an estimate)
	 * @param timeIntervalNextTry how far into the plan the next attempt should be (by time)
	 * @return new kinematic plan with (so,vo,to) added as a first point
	 * If we cannot connect to point zero of fp or the kinematic generation fails this returns a plan with its error status set.
	 * The resulting plan may be infeasible from a kinematic view.
	 */
	public static Plan genDirectToRetry(Plan fp, Position so, Velocity vo, double to, 
			double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn, double timeIntervalNextTry) {
		double minVsChangeRecognized = vsAccel*getMinTimeStep(); // this is the minimum change all
		return genDirectToRetry(fp, so, vo, to, bankAngle, gsAccel, vsAccel, minVsChangeRecognized, timeBeforeTurn, timeIntervalNextTry);
	}

	public static Plan genDirectToRetry(Plan p, Position so, Velocity vo, double to, 
			double bankAngle, double gsAccel, double vsAccel, double minVsChangeRecognized, double timeBeforeTurn, double timeIntervalNextTry) {
		boolean done = false;
		Plan kpc;
		Plan fp = new Plan(p);
		do {
			kpc = genDirectTo(fp,so,vo,to,bankAngle,gsAccel,vsAccel, minVsChangeRecognized, timeBeforeTurn);
			//if (kpc.getFirstTime() < p.getFirstTime()) f.pln("genDirectToRetry to="+to+" p1="+p.getFirstTime()+" fp1="+fp.getFirstTime()+" kpc="+kpc.getFirstTime());
			//f.pln(" $$$$ generate GenDirectTo !!!! kpc.size() = "+kpc.size()+" kpc.hasError() = "+kpc.hasError());
			if (kpc.hasError() && fp.size() > 1) {
				double tm0 = fp.point(0).time();
				double tm1 = fp.point(1).time();
				if (tm0 + timeIntervalNextTry + 20 < tm1) { // add connection point
					Position connectPt = fp.position(tm0+timeIntervalNextTry);
					//f.pln(" $$$$ Add connectPt = "+connectPt+" at time "+(tm0+timeIntervalNextTry));
					fp.addNavPoint(new NavPoint(connectPt,tm0+timeIntervalNextTry));
					fp.remove(0);
				} else {
					fp.remove(0);
				}
			} else {
				done = true;
			}
		} while (!done);

		// 	     PlanCore lpc2 = genDirectToLinear((PlanCore) fp,so,vo,to,bankAngle,timeBeforeTurn);
		//   		 kpc = makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, true);

		return kpc;
	}

	/**
	 * Construct a (hopefully) feasible linear plan that describes a path from s to the goal point.
	 * If this fails, it reurns a simple direct plan.
	 */
	public static Plan buildDirectTo(String id, NavPoint s, Velocity v, Plan base, double bankAngle) {
		double minTimeStep = 10.0;
		boolean done = false;
		double tt = base.getFirstTime();
		double range = base.getLastTime()-s.time();
		double stepSize = Util.max(minTimeStep, range/20.0);
		double minGsDiff = Units.from("kts", 10.0);
		double bestTime = base.getLastTime();
		double bestdgs = Double.MAX_VALUE;
		Quad<Position,Velocity,Double,Integer> bestdtp = null;
		// find the "best" point at which to reconnect
		while (!done && tt < base.getLastTime()) {
			if (tt > s.time()) { // first filter -- need to connect in the future
				double R = Kinematics.turnRadius(v.gs(), bankAngle);
				Quad<Position,Velocity,Double,Integer> dtp = ProjectedKinematics.directToPoint(s.position(), v, base.position(tt), R);
				double t2 = s.time()+dtp.third;
				if (t2 < tt) {  // second filter -- need to finish directTo turn before connect point
					// boolean turnRight = Kinematics.turnRight(v, dtp.second.trk());
					double dgs = Math.abs(dtp.second.gs() - dtp.first.distanceH(s.position())/(tt-t2));
					if (dgs+minGsDiff < bestdgs) {
						bestdgs = dgs;
						bestTime = tt;
						bestdtp = dtp;
					}
				}
			}
			tt += stepSize;
		}
		Plan lpc = new Plan(base.getName());
		lpc.addNavPoint(s);
		if (bestdtp != null) {
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
	 * Attempt to connect to the given point. Returns a kinematic plan that moves from the current state to that position.
	 * @param s
	 * @param v
	 * @param goal
	 * @return
	 */
	public static Plan genDirectTo(String id, Position s, Velocity v, double t, Position goal, double bankAngle, double gsAccel, double vsAccel, double delay) {
		Plan lpc = new Plan(id);
		NavPoint np1 = new NavPoint(s,t);
		lpc.addNavPoint(np1);
		if (v.gs() <= 0.0 || bankAngle == 0.0) {
			return lpc;  
		}
		double R = Kinematics.turnRadius(v.gs(), bankAngle);
		if (delay <= 1.0) {
			delay = 1.0;
		}
		double dt = delay;
		Position p2 = s.linear(v, delay);
		Quad<Position,Velocity,Double,Integer> tri = ProjectedKinematics.directToPoint(p2, v, goal, R);
		while (tri.third < 0.0) {
			dt += 1.0;
			p2 = s.linear(v, delay);
			tri = ProjectedKinematics.directToPoint(p2, v, goal, R);
		}
		NavPoint turnPt = new NavPoint(tri.first, t+dt+tri.third);
		NavPoint endPt = new NavPoint(goal, turnPt.time() + goal.distanceH(tri.first)/v.vs());
		lpc.addNavPoint(turnPt);
		lpc.addNavPoint(endPt);
		Plan kpc = makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false); 
		return kpc;
	}


	/**
	 * Returns a PlanCore version of the given plan.  If it was a kinematic plan, this will attempt to regress the TCPs to their original
	 * source points (if the proper metadata is available).  If this is already a PlanCore, return a copy.
	 */
	public static Plan makeLinearPlan(Plan fp) {
		//f.pln(" $$$$ makeLinearPlan: fp.isLinear() = "+fp.isLinear());
		if (fp.isLinear()) {
			return new Plan(fp);
		}
		return PlanUtil.revertTCPs(fp);
	}

//	// only add a point it it will not overwrite an existing one!
//	private static void safeAddPoint(Plan p, NavPoint n) {
//		if (p.overlaps(n) < 0) p.add(n);
//	}

	// remove TCPs from plan but make sure that does not leave a begin TCP without its corresponding ned TCP or vice-versa
//	public static Plan removeTCPsSafe(Plan fp, int start, int upto) {	
//		ArrayList<Integer> inPts = insideAccelZone(fp);
//		f.pln(" $$ removeTCPsSafe: "+start+" "+fp.point(start).isTCP()+" "+fp.point(start).isBeginTCP());
//		while ( start > 0 && (inPts.contains(start) || (fp.point(start).isTCP() && !fp.point(start).isBeginTCP()))) {
//			f.pln(" $$ removeTCPsSafe: "+start+" "+fp.point(start).isTCP()+" "+fp.point(start).isBeginTCP());
//			f.pln(" $$ decrement start = "+start+" inPts = "+inPts);
//		    start--;
//		}
// 		while ( upto < fp.size()-2 && (inPts.contains(upto) || (fp.point(upto).isTCP() && !fp.point(upto).isEndTCP()))) {
//		    upto++;
//		}	
//		f.pln(" ## removeTCPsSafe: start = "+start+" upto = "+upto);
//	    return removeTCPs(fp,start,upto);
//	}
	
    // This code assumes that vertical zones can overlap with turn or gs zones, but that turns cannot overlap with turns or gs zones.
	public static ArrayList<Integer> insideAccelZone(Plan fp) {
		ArrayList<Integer> inPts = new ArrayList<Integer>(10);
		boolean insideTurn = false;
		boolean insideGsAccel = false;
		boolean insideVsAccel = false;
		for (int i = 0; i < fp.size(); i++) {
			if (fp.isBOT(i)) insideTurn = true;
			if (fp.isEOT(i)) insideTurn = false;
			if (fp.isBGS(i)) insideGsAccel = true;
			if (fp.isEGS(i)) insideGsAccel = false;
			if (fp.isBVS(i)) insideVsAccel = true;
			if (fp.isEVS(i)) insideVsAccel = false;
		    if (insideTurn && (fp.isBVS(i) || fp.isEVS(i))) inPts.add(i);
		    if (insideGsAccel && (fp.isBVS(i) || fp.isEVS(i))) inPts.add(i);
            if (insideVsAccel &&  (fp.isBOT(i) || fp.isEOT(i))) inPts.add(i);
            if (insideVsAccel &&  (fp.isBGS(i) ||  fp.isEGS(i))) inPts.add(i);
		}
		return inPts;
	}




	/**
	 * Returns a new Plan that sets all points in a range to have a constant GS.
	 * The return plan type will be the same as the input plan type.
	 * This re-generates a kinematic plan, if necessary (if this fails, error status will be set)
	 * The new gs is specified by the user.
	 */
	public static Plan makeGSConstant(Plan p, double newGs,  double bankAngle, double gsAccel, double vsAccel, 	boolean repair) {
		Plan kpc = PlanUtil.revertTCPs(p);
		kpc = PlanUtil.linearMakeGSConstant(kpc,newGs);
		if (!p.isLinear()) {
			kpc = makeKinematicPlan(kpc,  bankAngle, gsAccel, vsAccel, repair);
		}
		return kpc;
	}

	/**
	 * Returns a new Plan that sets all points in a range to have a constant GS.
	 * The return plan type will be the same as the input plan type.
	 * This re-generates a kinematic plan, if necessary (if this fails, error status will be set)
	 * The new gs is the average of the linear version of the plan.
	 */
	public static Plan makeGSConstant(Plan p, double bankAngle, double gsAccel, double vsAccel, boolean repair) {
		//f.pln("  $$$$$$ TrajGen.makeGSConstant: p = "+p);
		Plan kpc = PlanUtil.revertTCPs(p);
		kpc = PlanUtil.linearMakeGSConstant(kpc);
		if (!p.isLinear()) {
			kpc = makeKinematicPlan(kpc,  bankAngle, gsAccel, vsAccel, repair);
		}
		return kpc;
	}




	///TODO

	// This generates a mini kinimatic plan that consists of a track (and possibly VS) change going from state direct to (through) goal at constant gs.
	// This only needs a goal position, not a goal plan, unlike directTo, above
	public static Plan maneuverToPosition(String name, Position so, Velocity vo, double to, Position goal, double bankAngle, double gsAccel, double vsAccel, double delay) {
		NavPoint end = new NavPoint(goal, to + 100000);
		Plan pc = new Plan(name);
		pc.addNavPoint(end);
		boolean done = false;
		Plan kpc = pc.copy();
		double d = delay;
		while (!done && d < 50) {
			done = true;
			Plan lpc = genDirectToLinear(pc,so,vo,to,bankAngle,d);
//f.pln("maneuvertoposition = "+d+" = "+lpc);

			lpc = PlanUtil.linearMakeGSConstant(lpc,vo.gs());

			if (lpc.hasError()) {
				return lpc;
			}
			
			kpc = makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false);
//f.pln("xxx "+d+"  "+kpc);
			if (kpc.hasError()) {
				done = false;
				d = d + 1.0;
			}
		}
		return kpc;
	}

	// This generates a mini kinematic plan that involves a symmetric two-turn maneuver that passes through both the start and goal points and has the specified velocities at both points.
	// Turns will tend to be close to the "ends" of the maneuver, possible leaving a long straight segment in the middle.
	// Both GS and VS may change to match the final velocity.  There is no assigned time to the goal point.
	// This can be seen as a more stringent "track to fix" maneuver (as it also deals with VS)
	// This will be more fragile than the simple maneuverToPosition
	public static Plan maneuverToPositionVelocity(String name, Position so, Velocity vo, double to, Position goalp, Velocity goalv, double bankAngle, double gsAccel, double vsAccel, double delay) {
		Position pMid = so.midPoint(goalp);

		NavPoint end = new NavPoint(pMid, to + 100000);
		Plan pc = new Plan(name);
		//f.pln("so="+so+" goalp="+goalp+" pMid="+pMid+" end="+end);		
		pc.addNavPoint(end);
		Plan lpc1 = genDirectToLinear(pc,so,vo,to,bankAngle,delay);
		if (lpc1.hasError()) {
			lpc1.addError("TrajGen.maneuverToPositionVelocity failed on initial turn");
		}
		lpc1 = PlanUtil.linearMakeGSConstant(lpc1,vo.gs());
		//		f.pln(""+lpc1);

		Plan lpc2r = genDirectToLinear(pc,goalp,goalv.Neg(),to,bankAngle,delay); // go in the reverse direction
		if (lpc2r.hasError()) {
			lpc1.addError("TrajGen.maneuverToPositionVelocity failed on secondary turn"); // write error to lpc
		}
		lpc2r = PlanUtil.linearMakeGSConstant(lpc2r,goalv.gs());
		//		f.pln(""+lpc2r);

		Plan lpc = lpc1.copy();
		double t = lpc.getLastTime();
		if (!lpc1.position(lpc1.getLastTime()).almostEquals(lpc2r.position(lpc2r.getLastTime()))) {
			lpc.addError("TrajGen.maneuverToPositionVelocity: sub-plans do not have a common point!");
		}
		for (int i = lpc2r.size()-2; i >=0; i--) { // add points in reverse order
			t = t + lpc2r.time(i+1)-lpc2r.time(i);
			NavPoint np = lpc2r.point(i).makeTime(t);
			lpc.addNavPoint(np);
		}
		Plan kpc = makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false);
		return kpc;
	}


//	// This generates a mini kinimatic plan that consists of a ground speed change (while moving straight ahead)
//	public static Plan maneuverToGs(String name, Position so, Velocity vo, double to, double targetGS, double gsAccel, double delayBefore, double delayAfter) {
//		Plan kpc = new Plan(name);
//		//kpc.setPlanType(Plan.PlanType.KINEMATIC);
//		NavPoint np1 = new NavPoint (so,to);
//		if (delayBefore > 0) {
//			kpc.add(np1); // if there is a delay, add the initial point
//		}
//		np1 = np1.linear(vo, delayBefore);
//		NavPoint np2 = np1.linear(vo, 3600); // somewhere far away		
//		Triple<NavPoint,NavPoint,Double> tcpTriple =  gsAccelGenerator(np1, np2, targetGS, vo, gsAccel, GsMode.PRESERVE_GS);
//		if (tcpTriple.third >= getMinTimeStep()) {
//			np1 = tcpTriple.first;
//			np2 = tcpTriple.second;
//			kpc.add(np1);				
//			kpc.add(np2);
//		} else if (tcpTriple.third < 0) {
//			kpc.add(np1);
//			kpc.addError("TrajGen.maneuverToGs: insufficient time");
//		} else if (tcpTriple.third > getMinTimeStep()*minorVfactor) {
//			//np1 = np1.makeLabel("minorGsChange"); // makeMinorGsChange();
//			kpc.add(np1);
//		}
//		if (delayAfter > 0) {
//			NavPoint np3 = kpc.point(kpc.size()-1).linear(vo.mkGs(targetGS), delayAfter);
//			kpc.add(np3);
//		}
//		return kpc;
//	}

//	// This generates a mini kinematic plan that involves reaching a given point (along my current path) as an RTA.
//	// This could follow a maneuverToPoint call, for example
//	public static Plan maneuverToTime(String name, Position so, Velocity vo, double to, Position goal, double rta, double gsAccel, double delay) {
//		Plan kpc = new Plan(name);
//		//kpc.setPlanType(Plan.PlanType.KINEMATIC);
//		if (rta < to) {
//			kpc.addError("TrajGen.maneuverToTime: RTA is before current time! "+rta+" < "+to);
//			return kpc;
//		}
//		NavPoint np1 = new NavPoint (so,to);
//		if (delay > 0) {
//			kpc.add(np1); // if there is a delay, add the initial point
//		}
//		np1 = np1.linear(vo, delay);
//		NavPoint np3 = new NavPoint(goal, rta);
//		kpc.add(np3);
//		Velocity v2 = np1.initialVelocity(np3).mkGs(vo.gs());
//		Pair<Double,Double> gst = Kinematics.gsAccelToRTA(vo.gs(), np1.distanceH(np3), rta-(to+delay), gsAccel);
//		if (gst.second < 0) { // not enough space to accelerate
//			kpc.add(np1);
//			kpc.addError("TrajGen.maneuverToTime: insufficient distance to acceleration to new gs");
//		} else if (gst.second < getMinTimeStep()) { 
//			//np1 = np1.makeLabel("makeMinorGsChange"); // makeMinorGsChange();
//			kpc.add(np1);
//		} else {
//			double a = gsAccel;
//			if (gst.first < vo.gs()) a = -gsAccel;
//			np1 = np1.makeBGS(np1.position(), np1.time(), a, v2,-1);
//			NavPoint np2 = np1.makeEGS(ProjectedKinematics.gsAccel(so, v2, gst.second, a).first, gst.second+np1.time(), v2,-1);
//			kpc.add(np1);
//			kpc.add(np2);
//		}
//		return kpc;
//	}

//	// This generates a mini kinematic plan that involves reaching a given point (along my current path) as an RTA, with the given ground speed.
//	// This could follow a maneuverToPoint call, for example
//	public static Plan maneuverToTimeVelocity(String name, Position so, Velocity vo, double to, Position goal, double gsGoal, double rta, double gsAccel, double delay) {
//		Plan kpc = new Plan(name);
//		//kpc.setPlanType(Plan.PlanType.KINEMATIC);
//		if (rta < to) {
//			kpc.addError("maneuverToTimeVelocity: RTA is before current time! "+rta+" < "+to);
//			return kpc;
//		}
//		NavPoint np1 = new NavPoint(so,to);
//		NavPoint np2 = new NavPoint(goal,rta);
//		kpc.add(np1);
//		kpc.add(np2);
//		double d = so.distanceH(goal);
//		double t = rta-to;
//		if (Util.almost_equals(vo.gs(), gsGoal) && Util.almost_equals(gsGoal, d/t)) {
//			// nothing needs to be done
//			return kpc;
//		}
//		delay = Util.max(delay,MIN_ACCEL_TIME);
//		Position p1 = so.linear(vo, delay);
//		Position p2 = goal.linear(vo.mkGs(gsGoal), -delay);
//		t = rta - to - delay*2;
//		d = p1.distanceH(p2);
//		if (t < 0) {
//			kpc.addError("TrajGen.maneuverToTimeVelocity: insufficient time before goal point");
//			return kpc;
//		}
//		Pair<Triple<Double,Double,Double>,Triple<Double,Double,Double>> r = Kinematics.gsAccelToRTAV(vo.gs(), d, t, gsGoal, gsAccel);
//		double t1 = r.first.first;
//		double t2 = r.first.second;
//		double t3 = r.first.third;
//		double a1 = r.second.first;
//		double gs2 = r.second.second;
//		double a2 = r.second.third;
//		if (t1 > 0.0 && t2 > 0 && t3 > 0) {
//			NavPoint np3 = np1.makeBGS(p1, to+delay, a1, vo,-1);
//			NavPoint np4 = np1.makeEGS(ProjectedKinematics.gsAccel(so, vo, t1, a1).first, to+delay+t1,  vo,-1);
//			kpc.add(np3);
//			kpc.add(np4);
//			Velocity v2 = vo.mkGs(gs2);
//			Position p5 = np4.position().linear(v2,t2);
//			NavPoint np5 = np1.makeBGS(p5, to+delay+t1+t2, a2, v2,-1);
//			NavPoint np6 = np1.makeEGS(ProjectedKinematics.gsAccel(p5, v2, t3, a2).first, to+delay+t1+t2+t3, v2,-1);
//			kpc.add(np5);
//			kpc.add(np6);
//		} else if (Util.almost_equals(t1, 0)) {
//			Velocity v2 = vo.mkGs(gs2);
//			NavPoint np5 = np1.makeBGS(p1, to+delay+t1+t2, a2, v2,-1);
//			NavPoint np6 = np1.makeEGS(ProjectedKinematics.gsAccel(p1, v2, t3, a2).first, to+delay+t1+t2+t3,  v2,-1);
//			kpc.add(np5);
//			kpc.add(np6);
//		} else if (Util.almost_equals(t3, 0)) {
//			NavPoint np3 = np1.makeBGS(p1, to+delay, a1, vo,-1);
//			NavPoint np4 = np1.makeEGS(ProjectedKinematics.gsAccel(so, vo, t1, a1).first, to+delay+t1,  vo,-1);
//			kpc.add(np3);
//			kpc.add(np4);
//		} else if (Util.almost_equals(t2, 0)) {
//			kpc.addError("TrajGen.maneuverToTimeVelocity: case 2");			
//		} else {
//			// fail
//			kpc.addError("TrajGen.maneuverToTimeVelocity: cannot match RTA point");
//		}
//		return kpc;
//	}
	

	public static Plan removeRedundantPoints(Plan pc) {
		Plan p = pc.copy();
		int i = 0;
		while (i < p.size()-2) {
			NavPoint np1 = p.point(i);
			NavPoint np2 = p.point(i+1);
			NavPoint np3 = p.point(i+2);
			if ( ! p.isTCP(i+2) && PositionUtil.collinear(np1.position(),np2.position(), np3.position()) && pc.finalVelocity(i).almostEquals(pc.initialVelocity(i+1))) {
				p.remove(i+1);
			} else {
				i++;
			}
		}
		return p;
	}


//	/** This reconstructs complete navpoints for a turn, including the source point (timing may be early) */
//	static Triple<NavPoint,NavPoint,NavPoint> turnGenerator3(NavPoint so, Velocity vo, double omega, double dt) {
//		Position p1 = so.position();
//		double t = so.time();
//		Pair<Position,Velocity> prMid = ProjectedKinematics.turnOmega(p1, vo, dt/2, omega);
//		Pair<Position,Velocity> prEnd = ProjectedKinematics.turnOmega(p1, vo, dt, omega);
//		Pair<Position,Double> prSrc = ProjectedKinematics.intersection(p1, vo, prEnd.first, prEnd.second);
//		if (prSrc.second < 0) {
//			f.pln("Error in TrajGen.turnGenerator3: negative intersection time");
//		}
//		NavPoint src = new NavPoint(prSrc.first, t + prSrc.second);
//		double radius = vo.gs()/omega;
////		int ix = src.linearIndex();
//		int ix = -1;
//		NavPoint np1 = src.makeBOT(p1, t, vo, radius,ix);
//		//NavPoint np2 = src.makeTurnMid(prMid.first, t+dt/2.0, omega, prMid.second);
//		NavPoint np2 = src.makePosition(prMid.first).makeTime(t+dt/2.0);
//		TcpData np2_tcp = np2.getTcpData();
//		np2_tcp.makeVelocityInit(prMid.second);		
//		NavPoint np3 = src.makeEOT(prEnd.first, t+dt, prEnd.second,ix);
//		return new Triple<NavPoint,NavPoint,NavPoint>(np1,np2,np3);
//	}


//	/**
//	 * Produce a standard holding pattern loop.  given a start position so and velocity vo, maintain the same altitude: immediately turn to 90 degrees from vo
//	 * at the given rate and direction, travel legDistA (generally close to zero), and then turn and travel legDistB (usually 1-2nmi), and repeat until returning to so.
//	 * 
//	 * @param so starting position, at beginning of first turn
//	 * @param vo starting velocity, just before first turn
//	 * @param t starting time
//	 * @param turnOmega turn rate and direction: standard right turn is Units.from("deg/s",3.0) (or 0.05236), standard left turn in -Units.from("deg/sec",3.0)
//	 * @param legDistA the distance between turn end and begin on the "short" (perpendicular to track) side
//	 * @param legDistB the distance between turn end and begin on the "long" (parallel to track) side
//	 * @return Kinematic plan completing one loop, beginning and ending at so.  If reverted to a linear Plan, this should consist of 4 turn points plus the end point.
//	 */
//	public static Plan standardHoldingPattern(Position so, Velocity vo, double t, double turnOmega, double legDistA, double legDistB) {
//		Plan plan = new Plan();
//		if (vo.gs() == 0.0) {
//			plan.addError("TrajGen: standardHoldingPattern error: zero ground speed");
//			return plan;
//		}
//		Velocity vo0 = vo.mkVs(0); // level flight
//		double dt = Math.abs((Math.PI/2.0)/turnOmega); 
//		double tA = Util.max(MIN_ACCEL_TIME, legDistA/vo.gs());
//		double tB = Util.max(MIN_ACCEL_TIME, legDistB/vo.gs());
//
//		NavPoint np0 = new NavPoint(so,t);
//		Triple<NavPoint,NavPoint,NavPoint> turn1 = turnGenerator3(np0,vo0,turnOmega,dt);
//		Velocity vo1 = turn1.third.velocityInit();
//		NavPoint np1 = turn1.third.linear(vo1, tA);
//		Triple<NavPoint,NavPoint,NavPoint> turn2 = turnGenerator3(np1,vo1,turnOmega,dt);
//		Velocity vo2 = turn2.third.velocityInit();
//		NavPoint np2 = turn2.third.linear(vo2, tB);
//		Triple<NavPoint,NavPoint,NavPoint> turn3 = turnGenerator3(np2,vo2,turnOmega,dt);
//		Velocity vo3 = turn3.third.velocityInit();
//		NavPoint np3 = turn3.third.linear(vo3, tA);
//		Triple<NavPoint,NavPoint,NavPoint> turn4 = turnGenerator3(np3,vo3,turnOmega,dt);
//		Velocity vo4 = turn4.third.velocityInit();
//		NavPoint np4 = turn4.third.linear(vo4, tB);
//
//		plan.add(turn1.first);
//		plan.add(turn1.second);
//		plan.add(turn1.third);
//		plan.add(turn2.first);
//		plan.add(turn2.second);
//		plan.add(turn2.third);
//		plan.add(turn3.first);
//		plan.add(turn3.second);
//		plan.add(turn3.third);
//		plan.add(turn4.first);
//		plan.add(turn4.second);
//		plan.add(turn4.third);
//		plan.add(np4);
//		return plan;
//	}

	
	/**
	 * 
	 * @param totalDist
	 * @param dt
	 * @param gs0
	 * @param gsAccel
	 * @return
	 */
	private static double accelTimeToRTA(double totalDist, double dt, double gs0, double gsAccel) {
		double a = -0.5*gsAccel;
		double b = gsAccel*dt;
		double c = gs0*dt - totalDist;
		double root1 = Util.root(a,b,c,1);
		double root2 = Util.root(a,b,c,-1);
		//f.pln(" $$$ accelTimeToRTA: roots = "+root1+", "+root2);
		if (Double.isNaN(root1) || root1 < 0) return root2;
		if (Double.isNaN(root2) || root2 < 0) return root1;
		return root1;
	}

	private static double accelDistance(double dt, double gs0, double gsAccel) {
		return gs0*dt + 0.5*gsAccel*dt*dt;
	}
		
	// ***** EXPERIMENTAL ****
	/**
	 * 
	 * @param fp
	 * @param tBGS       create BGS EGS pair starting at this time point in time
	 * @param ixRTA      location of RTA
	 * @param RTA        required time of arrival
	 * @param minGs         
	 * @param maxGs
	 * @param gsAccel
	 * @param maxOmega
	 * @param makeKin
	 * @param vsAccel
	 * @return
	 */
	public static Plan addRTA(Plan fp, double tBGS, int ixRTA, double RTA, double minGs, double maxGs, 
			double gsAccel, double maxOmega, boolean makeKin, double vsAccel) {		
		int currentSeg = fp.getSegment(tBGS);
		double pathDist = fp.pathDistanceFromTime(tBGS,ixRTA);
		//f.pln(" $$$ pathDist = "+Units.str("nmi",pathDist));
		double dtAtRTA = RTA - tBGS;
		Velocity vin = fp.initialVelocity(currentSeg);
		double gs0 = vin.gs();
		double accelTime = TrajGen.accelTimeToRTA(pathDist, dtAtRTA, gs0, gsAccel);
		//f.pln(" $$$ accelTime = "+accelTime);
		double deltaGs = Units.from("kn",200);
 		Plan kpcGS = PlanUtil.add_BGS_EGS_pair(fp, tBGS , deltaGs, gsAccel);
		
		return kpcGS;
	}
	
//	/**  ++++++ EXPERIMENTAL ++++
//	 * Creates a new plan which is identical to "p" upto currentTime, but has constant gs after currentTime
//	 *  if "makeKin" is true, then the final section is regenerated as a kinematic plan.
//	 *  Otherwise, The end section is returned as linear plan.   
//	 *  
//	 *  Note: The radius of turns is preserved for the generation of turns with the same radius as before
//
//	 * 
//	 * @param p                 plan to be processed
//	 * @param currentTime       current time in plan
//	 * @param gs                target ground speed
//	 * @return                  new Plan with ground speeds equal to gs after currentTime
//	 */
//	public static Plan makeGsConstant(Plan p, double currentTime, double gs, boolean makeKin, double vsAccel) {
//		int wp1 = p.getSegment(currentTime)+1;
//		Plan priorPlan = PlanUtil.cutDown(p, p.getFirstTime(), currentTime);		
//		Position currentPos = p.position(currentTime);
//		Route rt = PlanUtil.toRoute(p,wp1);
//		rt.add(0,currentPos,Plan.specPre+"currentPosition",0.0);	
//		f.pln("\n $$$$$+++++++++++++++++++  makeGsConstant: rt = "+rt.toString());
//		Plan linPlan = rt.linearPlan(currentTime,gs); 
//		Plan rtn;
//		if (makeKin) {
//			double bankAngle = Units.from("deg", 0.00000000001);   // should not be needed !!! radius in NAvPoint
//			double gsAccel = 0.00000000000001;                     // There should not be any BGS - EGS zones needed
//			boolean repairTurn = false;
//			boolean repairGs = true;
//			boolean repairVs = false;
//			GsMode gsm = GsMode.PRESERVE_GS;
//			//f.pln(" $$$$$$$$$$$$$@@@@@@@ makeGsConstant: linPlan = "+linPlan.toStringGs());
////			DebugSupport.dumpPlan(linPlan, "testGsSeq_linPlan");
//			Plan kpc = TrajGen.makeKinematicPlan(linPlan, bankAngle, gsAccel, vsAccel, repairTurn, repairGs, repairVs, gsm);	
//			rtn = priorPlan.append(kpc);
//			//f.pln(" $$$$$$$$$$$$$@@@@@@@ append kpc = "+kpc.toStringGs());
//		} else {
//		    rtn = priorPlan.append(linPlan);
//		}
//		return rtn;
//	}


	
	/**
	 * Construct a feasible kinematic plan that describes a path from s to the goal point.
	 * If this fails, it reurns a simple direct plan.
	 */
	public static Plan buildDirectToOLD(String id, NavPoint s, Velocity v, Position goal, double bankAngle, double gsAccel, double vsAccel) {
		Plan fp = new Plan(id);		
		//boolean turnRight = ProjectedKinematics.turnRightDirectTo(s.position(), v, goal);
		double R = Kinematics.turnRadius(v.gs(), bankAngle);
		//directToPoint(Position so, Velocity vo, Position wp, double R)
		Quad<Position,Velocity,Double,Integer> dtp = ProjectedKinematics.directToPoint(s.position(), v, goal, R);
		double dt = dtp.third;
		boolean turnRight = dtp.fourth > 0;				
		double finalTime = s.position().distanceH(goal)/v.gs()+s.time();
		NavPoint BOT = s.linear(v, 2.0);
		//double dt = ProjectedKinematics.turnTimeDirectTo(BOT.position(), v, goal, bankAngle);
		if (dt < 0) { // we are too close to be able to make the turn.  go straight for at least enough time to add 2R to our distance
			double t =  R*2/v.gs();
			BOT = s.linear(v,t);
			//dt = ProjectedKinematics.turnTimeDirectTo(BOT.position(), v, goal, bankAngle);
			dt = ProjectedKinematics.directToPoint(BOT.position(), v, goal, R).third;

		}		
		if (dt > 2.0) {
			NavPoint s2 = BOT.linear(v, dt/2);
			Position center = BOT.linear(Velocity.make(v.PerpL().Hat2D()), R).position();
			if (turnRight) center = BOT.linear(Velocity.make(v.PerpR().Hat2D()), R).position();
			double rot = v.gs()/R*Util.turnDir(v.trk(), dtp.second.trk());
			double radius = R*Util.turnDir(v.trk(), dtp.second.trk());
//			int ix = s2.linearIndex();
			int ix = -1;
			BOT = new NavPoint(BOT.position(), BOT.time());
			TcpData BOT_tcp = TcpData.makeSource(s).setBOT(radius, center, ix);
			//			NavPoint MOT = new NavPoint(ProjectedKinematics.turn(BOT.position(), v, dt*0.5, R, turnRight).first,BOT.time()+dt*0.5);
			//			MOT = s2.makeTurnMid(MOT.position(), MOT.time(), rot, v);
			Pair<Position,Velocity> pr = ProjectedKinematics.turn(BOT.position(), v, dt, R, turnRight);
			NavPoint EOT = new NavPoint(pr.first,BOT.time()+dt);
			TcpData  EOT_tcp = TcpData.makeSource(s).setBOT(R, center, ix);
			double d = EOT.position().distanceH(goal);
			finalTime = d/v.gs()+EOT.time();
			fp.add(BOT, BOT_tcp);
			//			fp.add(MOT);
			fp.add(EOT, EOT_tcp);
		}
		NavPoint np = new NavPoint(goal,finalTime);
		fp.addNavPoint(s);
		fp.addNavPoint(np);
		return fp;
	}
	
	
	/**
	 * Given a starting plan, this returns a navpoint to get from point 0 to point 1 (assuming an existing velocity that is not compensated for
	 * @param working plan
	 * @param v existing velocity
	 * @param delay delay to be factored in before turn
	 * @param maxBank maximum bank angle
	 * @return new navpoint.  If the point's time is less than 0 then it is redundant
	 */
	//TODO: used in watch.  substitute with something newer there
	public static NavPoint turnToPointOLD(Plan working, Velocity v, double delay, double maxBank) {
		double R = Kinematics.turnRadius(v.gs(),maxBank);
		//double dt = ProjectedKinematics.turnTimeDirectTo(working.point(0).position(), v, working.point(1).position(), maxBank);
		double dt = ProjectedKinematics.directToPoint(working.point(0).position(), v, working.point(1).position(), R).third;
		if (working.size() < 2 || !working.initialVelocity(0).almostEquals(v) && dt > 1.0) {		
			return working.point(0).linear(v, dt/2 + 1.0 + delay);
		}
		return new NavPoint(Position.ZERO_LL,-1.0);
	}


//	// attempt to gracefully reconnect to the provided plan.
	public static Plan reconnectToPlan(Plan p, Position so, Velocity vo, double to, 
			double bankAngle, double gsAccel, double vsAccel, double minVsChangeRecognized, double timeBeforeTurn, 
			double timeIntervalNextTry) {
		Plan kpc = p.copy(); 
		f.pln(" !!!!!!!!!!!!!!!!!!! NOT YET PORTED TO NEW NavPoint Data Structure !!!!!!!!!!!!!!!!!!!!!!!");
//		if (kpc.isLinear()) {
//			kpc = makeKinematicPlan(p, bankAngle, gsAccel, vsAccel, minVsChangeRecognized, false, false, false, gsm);
//		}
//
//		NavPoint current = new NavPoint(so,to);
//		NavPoint closest = kpc.closestPoint(to-100, to+100, so);
//		
//
//		//if (closest.time() < kpc.getFirstTime()) {
//		//	f.pln("reconnectToPlan: "+closest);
//		//	f.pln(""+kpc);
//		//}
//
//		double t = Util.max(to,closest.time()) + timeBeforeTurn*2;
//
//		if (t >= p.getLastTime()) {
//
//		}
//
//		while (!done && t < kpc.getLastTime() && t >= kpc.getFirstTime()) {
////f.pln("TrajGen.reconnectToPlan t="+t+" first="+kpc.getFirstTime()+" last="+kpc.getLastTime());			 
//			double gs0 = current.initialVelocity(new NavPoint(kpc.position(t),t)).gs();
//			Position so2 = so;
//			Velocity vo2 = vo;
//			double to2 = to;
//			Plan initialSegment = new Plan(p.getName());
//			if (Math.abs(gs0-vo.gs()) > vo.gs()*0.20) {
//				initialSegment = TrajGen.maneuverToGs(p.getName(), so, vo, to, gs0, gsAccel, timeBeforeTurn, timeBeforeTurn);
//				to2 = initialSegment.getLastTime();
//				so2 = initialSegment.position(to2);
//				vo2 = initialSegment.velocity(to2);
//			}
////f.pln("TRAJGEN "+t);			
////f.pln("init="+initialSegment.toOutput()+initialSegment.getMessageNoClear());		
//			Plan toPositionSegment = maneuverToPosition(p.getName(), so2, vo2, to2, kpc.position(t), bankAngle, gsAccel, vsAccel, timeBeforeTurn);
////f.pln("pos="+toPositionSegment.toOutput()+toPositionSegment.getMessageNoClear());		
//			// we can construct a path to the goal point.
//			if (!toPositionSegment.hasError()) {
//				double toPosEnd = toPositionSegment.getLastTime();
//				// if we don't have time to reach it, just get there.
//				if (toPosEnd >= kpc.getLastTime()) {
//					NavPoint goal = kpc.point(kpc.size()-1);
//					return maneuverToPosition(p.getName(), so, vo, to, goal.position(), bankAngle, gsAccel, vsAccel, timeBeforeTurn);
//				}
//				// otherwie construct a path and time fix to get there
//				Plan kpcRemainder = kpc.slice(kpc.getSegment(toPosEnd)+1).second;
//				// remove first point (since the timing is probably off)
//				kpcRemainder.remove(0);
//				Plan toTimeSegment = maneuverToTimeVelocity(p.getName(), kpc.position(t), kpc.velocity(t), toPositionSegment.getLastTime(), kpcRemainder.point(0).position(), kpc.velocity(t).gs(), kpcRemainder.getFirstTime(), gsAccel, timeBeforeTurn);
////f.pln("time="+toTimeSegment.toOutput()+toTimeSegment.getMessageNoClear());
////f.pln("rem="+kpcRemainder.toOutput()+kpcRemainder.getMessageNoClear());
//				toPositionSegment = initialSegment.join(toPositionSegment);
////f.pln("pos2="+toPositionSegment.toOutput()+toPositionSegment.getMessageNoClear());
//				toPositionSegment = toPositionSegment.join(toTimeSegment); //add gs match to end
////f.pln("pos3="+toPositionSegment.toOutput()+toPositionSegment.getMessageNoClear());		
//				kpcRemainder = kpcRemainder.join(toPositionSegment);
////f.pln("fin="+kpcRemainder.toOutput()+kpcRemainder.getMessageNoClear());
////Plan tmo = kpcRemainder.copy();
////tmo.setName(p.getName()+"::"+plist.size()+"::"+t);
////plist.add(tmo);
////System.exit(1);
//				// if all this worked without errors, return the new plan
//				if (!kpcRemainder.hasError() && kpcRemainder.isConsistent(true)) {
//					done = true;
//					kpc = kpcRemainder;
//				}
//			}
//			t += timeIntervalNextTry;
//		}
//
//		if (kpc.hasError()) {
//			String s = kpc.getMessage();
//			kpc = p.copy();
//			kpc.addError(s);
//		}
//
////f.pln(kpc.getOutputHeader());
////for (int i = 0; i < plist.size(); i++) {
////	f.pln(""+plist.get(i).toOutput());
////}
//		
		return kpc;
	}
	
	
	
//	/**
//	 * 
//	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points. 
//	 *  Also all TCPs should reference points in a feasible plan. 
//	 *  If the trajectory is modified, it will have added, modified, or deleted points.
//	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
//	 *	@param fp input plan (is linearized if not already so)
//	 *  @param bankAngle maximum allowed (and default) bank angle for turns
//	 *  @param gsAccel    maximum allowed (and default) ground speed acceleration (m/s^2)
//	 *  @param vsAccel    maximum allowed (and default) vertical speed acceleration (m/s^2)
//	 *  @param minVsChangeRecognized minimum vs change that will register as "non-constant" (m/s)
//	 *  @param repairTurn attempt to repair infeasible turns as a preprocessing step
//	 *  @param repairGs attempt to repair infeasible gs accelerations as a preprocessing step
//	 *  @param repairVs attempt to repair infeasible vs accelerations as a preprocessing step
//	 *  @param constantGS if true, produces a constant ground speed kinematic plan with gs being average of linear plan
//	 */
//	public static Plan makeKinPlanFlyOver(Plan lpc, double bankAngle, double gsAccel, double vsAccel,
//			boolean repairTurn, boolean repairGs, boolean repairVs) {
//		//Plan lpc = makeLinearPlan(fp);
//		if (lpc.size() < 2) return lpc;
//		//f.pln(" generateTCPs: ENTER ----------------------------------- lpc = "+lpc.toString()+" "+repairTurn);
//		final boolean flyOver = true;
//		final boolean addMiddle = true;
//		lpc = repairPlan( lpc, repairTurn, repairGs, repairVs, flyOver, addMiddle, bankAngle, gsAccel, vsAccel);
//		//f.pln(" generateTCPs: generateTurnTCPs ---------------------------------------------------------------------");
//		//DebugSupport.dumpPlan(lpc, "generateTCPs_lpc__repaired");
//		Plan kpc0 = markVsChanges(lpc);
//		//f.pln("##########FF kpc0 = "+kpc0);	
//		Plan kpc = generateTurnTCPsOver(lpc, kpc0, bankAngle);
//		//DebugSupport.dumpPlan(kpc, "generateTCPs_turns");
//		//f.pln("##########FF kpc = "+kpc);
//		if (kpc.hasError()) {
//			return kpc;
//		} else {
//			//Plan kpc2 = fixGS(lpc, kpc, gsAccel);
//			Plan kpc2 = kpc;
//			//f.pln("######## kpc2="+kpc2.toString());		
//			//DebugSupport.dumpPlan(kpc2, "generateTCPs_fixgs");
//			// ********************
//			Plan kpc3 = generateGsTCPs(kpc2, gsAccel, repairGs);
//			//DebugSupport.dumpPlan(kpc3, "generateTCPs_gsTCPs");
//			//f.pln("########## kpc3 = "+kpc3);
//			if (kpc3.hasError()) {
//				return kpc3;
//			} else {
//				// *******************
//				Plan kpc4 = makeMarkedVsConstant(kpc3);
//				//DebugSupport.dumpPlan(kpc4, "generateTCPs_vsconstant");
//				//f.pln("##########FF kpc4 = "+kpc4);
//				if (kpc4.hasError()) {
//					return kpc4;
//				} else {
//					Plan kpc5 = generateVsTCPs(kpc4, vsAccel);
//					//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
//					//f.pln("##########FF kpc5 = "+kpc5.toString());
//					cleanPlan(kpc5);
//					//kpc5.setPlanType(Plan.PlanType.KINEMATIC);
////					if (!kpc5.isConsistent()) {
////						f.pln("-------------------------------------"+lpc.getName());
////					}
//					return kpc5;
//				}
//			}
//		}
//	}

//
//
//	public static Plan makeKinPlanFlyOver(Plan fp, double bankAngle, double gsAccel, double vsAccel,boolean repair) {
//		//double minVsChangeRecognized = vsAccel*getMinTimeStep(); // this is the minimum change all
//		Plan traj = makeKinPlanFlyOver(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair);
//		return traj;
//	}
//

		// this marks all points where there is a significant vs change.  This may be overly aggressive for some applications
		// Also run smootheMarkedVsChanges to remove some extraneous marks.
		public static Plan markVsChangesNew(Plan lpc) {
			//f.pln(" markVsChanges: ENTER, lpc = "+lpc.toStringFull());
			String name = lpc.getName();
			Plan kpc = new Plan(name);
			if (lpc.size() < 2) {
				kpc = new Plan(lpc);
				kpc.addError("TrajGen.markVsChanges: Source plan "+name+" is too small for kinematic conversion");
				return kpc;
			}
			
			kpc.add(lpc.get(0)); // first point need not be marked
			for (int i = 1; i < lpc.size()-1; i++) {
				NavPoint np = lpc.point(i);
				TcpData  tcp = lpc.getTcpData(i);
				double vs1 = lpc.vsOut(i-1);
				double vs2 = lpc.vsOut(i);
				if (Math.abs(vs1) > maxVs) {
					kpc.addWarning("Input File has vertical speed exceeding "+Units.str("fpm",maxVs)+" at "+(i-1));
				}
				if ( ! Util.within_epsilon(vs1, vs2, MIN_VS_CHANGE)) {
					kpc.add(np, tcp.setAltPreserve());
				} else {
					kpc.add(np, tcp);	
				}
			}
			kpc.add(lpc.get(lpc.size()-1)); // last point need not be marked
			
			// we may need to deal with constant accel regions
			// make a copy only containing the current altitude preserve points.
			Plan tmp = new Plan(); 
			for (int i = 0; i < kpc.size(); i++) {
				if (i == 0 || i == kpc.size()-1 || kpc.getTcpData(i).isAltPreserve()) {
					tmp.addNavPoint(kpc.point(i));
				}
			}
			// check if the original vs is within the range of the marked vs profile
			for (int i = 1; i < kpc.size()-1; i++) {
				double vs1 = kpc.initialVelocity(i).vs();
				double vs2 = tmp.velocity(kpc.time(i)).vs();
				if (!Util.within_epsilon(vs1, vs2, 2*MIN_VS_CHANGE)) {
					kpc.setAltPreserve(i);
					tmp.addNavPoint(kpc.point(i)); // re-add for reference
				}
			}
			
			//f.pln(" $$>> markVsChanges: EXIT, kpc = "+kpc.toStringFull());
			return kpc;
		}
	
		// return true if this plan has a point marked with the given linear index
		private static boolean includesLindex(Plan p, int index) {
			for (int i = 0; i < p.size(); i++) {
				if (p.getTcpData(i).getLinearIndex() == index) {
					return true;
				}
			}
			return false;
		}
		
		// This assumes you have a plan that has altitude preserve points marked
		// It may remove some of those marks, resulting in a plan with a simpler vertical profile.
		// noiseWindow -- in seconds, how wide a spike can be and still be eliminated
		// levelRange -- in meters, how much variation is allowed in altitude to be considered level
		public static Plan smootheMarkedVsChanges(Plan lpc, double noiseWindow, double levelRange) {
			// make a copy only containing the current altitude preserve points.
			Plan tmp = new Plan();
			for (int i = 0; i < lpc.size(); i++) {
				if (i == 0 || i == lpc.size()-1 || lpc.getTcpData(i).isAltPreserve()) {
					TcpData tcp = new TcpData();
					tcp.setLinearIndex(i);
					tmp.add(lpc.point(i), tcp);
				}
			}
			
			// remove redundant vs points (vs delta is too small)
			for (int i = 1; i < tmp.size()-1; i++) {
				double vs1 = tmp.initialVelocity(i-1).vs();
				double vs2 = tmp.initialVelocity(i).vs();
				if (Util.within_epsilon(vs1, vs2, MIN_VS_CHANGE)) {
					tmp.remove(i);
					i--; // fix index				
				}
			}
			
			// remove short spikes (speed directions are opposite and they are temporally close together
			for (int i = 1; i < tmp.size()-1; i++) {
				double vs1 = tmp.initialVelocity(i-1).vs();
				double vs2 = tmp.initialVelocity(i).vs();
				double t1 = tmp.time(i)-tmp.time(i-1);
				double t2 = tmp.time(i+1)-tmp.time(i);
				if (Util.sign(vs1) != 0 && Util.sign(vs1) == -Util.sign(vs2) && Util.within_epsilon(t1+t2, noiseWindow)) {
					tmp.remove(i);
					i--; // fix index				
				}
			}
			
			// detect level flight (altitude delta is too small)
			for (int i = 1; i < tmp.size()-1; i++) {
				double alt1 = tmp.point(i-1).alt();
				double alt2 = tmp.point(i).alt();
				double alt3 = tmp.point(i+1).alt();
				if (Util.within_epsilon(alt1, alt2, levelRange) && Util.within_epsilon(alt2, alt3, levelRange) && Util.within_epsilon(alt1, alt3, levelRange)) {
					tmp.remove(i);
					i--; // fix index
				}
			}
		
			// re-build original plan with appropriate altitude preserve marks
			Plan kpc = new Plan(lpc.getName());
			for (int i = 0; i < lpc.size(); i++) {
				NavPoint np = lpc.point(i);
				TcpData tcp = lpc.getTcpData(i);
				if (includesLindex(tmp,i)) {
					kpc.add(np,tcp.setAltPreserve());
				} else {
					kpc.add(np,tcp.setOriginal());				
				}
			}
			
			return kpc;
		}

	

}
	


///**
// * @deprecated
// * Kinematic generations pass that adds turn TCPs.  This defers ground speed changes until after the turn.
// * It only assumes legs are long enough to support the turns.
// * kpc will have marked vs changes (if any).
// * 
// * bank angle must be nonnegative!!!!!
// * 
// */
//private static Plan generateTurnTCPsOver(Plan lpc, Plan kpc, double bankAngle) { //  THIS NEEDS MASSIVE WORK
//	//f.pln(" $$>> generateTurnTCPsOver: bankAngle = "+Units.str("deg",bankAngle));
//	//f.pln(" $$>> generateTurnTCPsOver: THIS IS BROKEN -- DON'T USE !!!");
//	System.exit(0);
//	Plan traj = new Plan(kpc); // the current trajectory based on working
//	int j = 1;
//	for (int i = 1; i+1 < kpc.size(); i++) {
//		//double tm = kpc.getTime(i);
//		Velocity vin =  traj.finalVelocity(j-1);          //         Velocity vin = kpc.finalVelocity(i-1);	
//		Velocity vi1 = traj.initialVelocity(j);
//		if (Util.almost_equals(vi1.gs(),0.0)) {
//			printError(" ###### WARNING: Segment from point "+(i)+" to point "+(i+1)+" has zero ground speed!");
//		}
//		double turnTime = Kinematics.turnTime(vin.gs(),Util.turnDelta(vin.trk(), vi1.trk()),bankAngle);
//		//f.pln(" $$>> generateTurnTCPsOver: for i = "+i+" j = "+j+" turnTime = "+turnTime);
//		if (turnTime >= getMinTimeStep()) {
//			NavPoint np = kpc.point(i);
//			//f.pln("\n generateTurnTCPsOver:  i = "+i+" j = "+j+"  np = "+np.toStringFull()+" vin = "+vin+" vi1 = "+vi1);
//			Position so = np.position();
//			//Velocity vo = kpc.finalVelocity(i-1);
//			double   to = traj.point(j).time();    // lpc(i) time no longert valid
//			//f.pln(" $$$ generateTurnTCPsOver: to = "+to+" traj.point(j).time() = "+ traj.point(j).time());
//			NavPoint wp = kpc.point(i+1);
//			Position wpp = wp.position();               
//			double R = Kinematics.turnRadius(vin.gs(), bankAngle);                
//			Quad<Position,Velocity,Double,Integer> dtp = ProjectedKinematics.directToPoint(so,vin,wpp,R);
//			int turnDir = dtp.fourth;
//			//f.pln(" $$$ generateTurnTCPsOver: so="+so+ " vin="+vin+" wpp="+wpp);
//			//f.pln(" $$$ generateTurnTCPsOver: dtp="+dtp);	    
//			if (dtp.third < 0) {
//				//f.pln(" $$$$$$$$ generateTurnTCPsOver: not enough time !!! at point "+i+" R  = "+Units.str("nm",R));
//				traj.addError("TrajGen.generateTurnTCPsOver: not enought time to complete turn before next point "+i,1); 
//				//AugmentedPlan atraj = new AugmentedPlan(traj);
//				//atraj.etype = ErrType.TURN_OVERLAPS_E;
//				//atraj.lpcErrorPt = i;
//				//return atraj;
//				return traj;
//			} else {
//				double turnTm = dtp.third;
//				//f.pln(" $$$ generateTurnTCPsOver: turnTm = "+turnTm); 
//				NavPoint eot_NP = new NavPoint(dtp.first,to+turnTm);
//				//f.pln(" $$$ generateTurnTCPsOver: ADD eot_NP = "+eot_NP+" dtp.second = "+dtp.second+" dtp.third = "+dtp.third);
//				//f.pln(" $$$ generateTurnTCPsOver: traj.point(j+1) = "+traj.point(j+1));
//				if (wp.time() < eot_NP.time()) {
//					//f.pln(" $$$$$$  generateTurnTCPsOver: not enough time !!!!!! at point "+i+"  "+wp.time()+"  < +"+eot_NP.time());
//					traj.addError("TrajGen.generateTurnTCPsOver: not enought time to reach direct to point  before next point "+i,1); 
//					//AugmentedPlan atraj = new AugmentedPlan(traj);
//					//atraj.etype = ErrType.TURN_OVERLAPS_B;
//					//atraj.lpcErrorPt = i;
//					//return atraj;
//					return traj;
//				}
//				Velocity v2 = eot_NP.initialVelocity(wp);
//				//f.pln(" $$$ generateTurnTCPsOver: v2 = "+v2);        			
//				//Velocity vx = traj.point(j).initialVelocity(eot_NP);
//				//f.pln(" $$$ generateTurnTCPsOver: vx = "+vx);        			      			
//				double omega = turnDir*vin.gs()/R;    // turnRate
//				//f.pln("  $$$ generateTurnTCPsOver: omega = "+omega);
//				NavPoint npBOT = np.makeBOT(np.position(),to, vin, turnDir*R,i);
//				NavPoint npEOT = np.makeEOT(eot_NP.position(),eot_NP.time(), v2,i);	
//
//				//f.pln(" $$$ generateTurnTCPsOver: remove j = "+j+" traj.point(j) = "+traj.point(i));
//				traj.remove(j);
//				traj.add(npBOT);
//				traj.add(npEOT);
//				double motTime = (npBOT.time()+npEOT.time())/2.0;
//				Position motPos = traj.position(motTime);
//				//f.pln("### generateTurnTCPsOver: motPos = "+motPos);
//				//NavPoint npMOT = np.makeTurnMid(motPos,motTime, omega, v2);	
//				NavPoint npMOT = np.makePosition(motPos).makeTime(motTime);
//				TcpData tcp = npMOT.getTcpData().makeVelocityInit(v2);		
//				traj.add(npMOT,tcp);
//				//f.pln("### generateTurnTCPsOver: npBOT = "+npBOT.toStringFull());
//				//f.pln("### generateTurnTCPsOver: npEOT = "+npEOT.toStringFull());
//				//f.pln("### generateTurnTCPsOver: npMOT = "+npMOT.toStringFull());
//				Velocity v1 = np.initialVelocity(kpc.point(i+1));          		 
//				double targetGs = v1.gs();
//				//kpc.add(npEOT);        			
//				//i++;     			        			
//				j = j + 3;
//				double nt = traj.linearCalcTimeGSin(j, targetGs);
//				double timeShift = nt - traj.point(j).time();
//				//f.pln(" $$$$$$$$$####### fixGS: for (j) = "+(j)+" targetGs = "+Units.str("kn",targetGs)+" timeShift = "+timeShift+" nt = "+nt);    	            
//				traj.timeshiftPlan(j, timeShift);
//				//f.pln("  ### generateTurnTCPsOver: SET j = "+j);
//			}
//		} else {
//			j = j+1;
//		}
//	}
//	//f.pln("");
//	//f.pln(" ### generateTurnTCPsOver: traj = "+traj.toString());
//	return traj;
//}



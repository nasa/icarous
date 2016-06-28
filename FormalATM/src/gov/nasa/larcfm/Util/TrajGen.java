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

package gov.nasa.larcfm.Util;



import java.util.ArrayList;
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

	static private boolean verbose = false;
	private static double MIN_ACCEL_TIME = 1.0;   
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
	static public enum GsMode {PRESERVE_GS, PRESERVE_TIMES, PRESERVE_RTAS, CONSTANT_GS};

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

	/**
	 * 
	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points. 
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
	 */
	public static Plan makeKinPlanFlyOver(Plan lpc, double bankAngle, double gsAccel, double vsAccel,
			boolean repairTurn, boolean repairGs, boolean repairVs, GsMode gsm) {
		//Plan lpc = makeLinearPlan(fp);
		if (lpc.size() < 2) return lpc;
		//f.pln(" generateTCPs: ENTER ----------------------------------- lpc = "+lpc.toString()+" "+repairTurn);
		final boolean flyOver = true;
		final boolean addMiddle = true;
		lpc = repairPlan( lpc, repairTurn, repairGs, repairVs, gsm, flyOver, addMiddle, bankAngle, gsAccel, vsAccel);
		//f.pln(" generateTCPs: generateTurnTCPs ---------------------------------------------------------------------");
		//DebugSupport.dumpPlan(lpc, "generateTCPs_lpc__repaired");
		Plan kpc0 = markVsChanges(lpc);
		//f.pln("##########FF kpc0 = "+kpc0);	
		Plan kpc = generateTurnTCPsOver(lpc, kpc0, bankAngle);
		//DebugSupport.dumpPlan(kpc, "generateTCPs_turns");
		//f.pln("##########FF kpc = "+kpc);
		if (kpc.hasError()) {
			return kpc;
		} else {
			//Plan kpc2 = fixGS(lpc, kpc, gsAccel);
			Plan kpc2 = kpc;
			//f.pln("######## kpc2="+kpc2.toString());		
			//DebugSupport.dumpPlan(kpc2, "generateTCPs_fixgs");
			// ********************
			Plan kpc3 = generateGsTCPs(kpc2, gsAccel, gsm);
			//DebugSupport.dumpPlan(kpc3, "generateTCPs_gsTCPs");
			//f.pln("##########FF kpc3 = "+kpc3);
			if (kpc3.hasError()) {
				return kpc3;
			} else {
				// *******************
				Plan kpc4 = makeMarkedVsConstant(kpc3);
				//DebugSupport.dumpPlan(kpc4, "generateTCPs_vsconstant");
				//f.pln("##########FF kpc4 = "+kpc4);
				if (kpc4.hasError()) {
					return kpc4;
				} else {
					Plan kpc5 = generateVsTCPs(kpc4, vsAccel);
					//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
					//f.pln("##########FF kpc5 = "+kpc5.toString());
					cleanPlan(kpc5);
					//kpc5.setPlanType(Plan.PlanType.KINEMATIC);
					return kpc5;
				}
			}
		}
	}


	// here's an alternate:
	protected static Triple<NavPoint,NavPoint,NavPoint> turnGenerator2(NavPoint np1, NavPoint np2, NavPoint np3, double bankAngle) {
		Velocity v1 = np1.initialVelocity(np2);
		Velocity v2 = np2.initialVelocity(np3);
		double trk1 = v1.trk();
		double trk2 = v2.trk();
		double gs1 = v1.gs();
		double theta = Util.turnDelta(trk1,  trk2); // turn angle
		double R = Kinematics.turnRadius(gs1, bankAngle);
		double distback = R*Math.sin(theta); // distance from corner on first leg
		double botTime = np2.time() - distback/gs1; // note this could be before np1.time(), which indicates the initial leg was too short  
		double turnTime = Kinematics.turnTime(gs1, theta, bankAngle);
		Position BOT = np2.position().linear(v1.Neg().Hat(), distback);
		Position EOT = np2.position().linear(v2.Hat(), distback);
		boolean turnRight = Util.turnDir(trk1,  trk2) > 0;
		Pair<Position,Velocity> MOT = ProjectedKinematics.turn(BOT, v1, turnTime/2, R, turnRight);
		double omega = Kinematics.turnRateGoal(v1, trk2, bankAngle);
		NavPoint npBOT = np2.makeBOT(BOT, botTime, omega, v1);
		//NavPoint npMOT = np2.makeTurnMid(MOT.first, botTime+turnTime/2, omega, v1.mkTrk(MOT.second.trk()));
        double tMOT = botTime+turnTime/2;
		NavPoint npMOT = np2.makePosition(MOT.first).makeTime(tMOT).makeVelocityIn(v1.mkTrk(MOT.second.trk()));		
		NavPoint npEOT = np2.makeEOT(EOT, botTime+turnTime, v1.mkTrk(trk2));
		// error states:
		if (npBOT.time() < np1.time()) {
			npBOT = NavPoint.INVALID; // error: initial leg was too short
		}
		if (distback > np2.distanceH(np3)) {
			npEOT = NavPoint.INVALID; // error: second leg was too short
		}
		return new Triple<NavPoint,NavPoint, NavPoint>(npBOT,npMOT,npEOT);
	}

	public static Plan makeKinPlanFlyOver(Plan fp, double bankAngle, double gsAccel, double vsAccel,boolean repair) {
		//double minVsChangeRecognized = vsAccel*getMinTimeStep(); // this is the minimum change all
		Plan traj = makeKinPlanFlyOver(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair, GsMode.PRESERVE_GS);
		return traj;
	}


	public static Plan makeKinPlanFlyOver(Plan fp, double bankAngle, double gsAccel, double vsAccel, 
			boolean repair, boolean constantGS) {
		//double minVsChangeRecognized = vsAccel*getMinTimeStep(); // this is the minimum change all
		GsMode gsm = GsMode.PRESERVE_GS;
		if (constantGS) gsm = GsMode.CONSTANT_GS;
		Plan traj = makeKinPlanFlyOver(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair, gsm);
		return traj;
	}
	

	/** TODO
	 * 
	 * The resulting Plan will be "clean" in that it will have all original points, with no history of deleted points. 
	 *  Also all TCPs should reference points in a feasible plan. 
	 *  If the trajectory is modified, it will have added, modified, or deleted points.
	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
	 *  Note: This method seeks to preserve ground speeds of legs.  The time at a waypoint is assumed to be not important
	 *        compared to the original ground speed.
	 *	@param fp input plan (is linearized if not already so)
	 *  @param bankAngle  maximum allowed (and default) bank angle for turns
	 *  @param gsAccel    maximum allowed (and default) ground speed acceleration (m/s^2)
	 *  @param vsAccel    maximum allowed (and default) vertical speed acceleration (m/s^2)
	 *  @param minVsChangeRecognized minimum vertical speed change that will register as "non-constant" (m/s)
	 *  @param repairTurn attempt to repair infeasible turns as a preprocessing step
	 *  @param repairGs attempt to repair infeasible ground speed accelerations as a preprocessing step
	 *  @param repairVs attempt to repair infeasible vertical speed accelerations as a preprocessing step
	 *  @param gsm      What to do with ground speed, for example, maintain a constant ground speed.
	 *  @return the resulting kinematic plan
	 */
	public static Plan makeKinematicPlan(Plan fp, double bankAngle, double gsAccel, double vsAccel,
			boolean repairTurn, boolean repairGs, boolean repairVs, GsMode gsm) {
//            f.pln(" $$$$ bankAngle = "+Units.str("deg",bankAngle)+" gsAccel = "+gsAccel+" vsAccel = "+vsAccel+"+"
//        		+ " minVsChangeRecognized = "+Units.str("fpm",minVsChangeRecognized)
//        				+ " repairTurn = "+repairTurn+" repairGs = "+repairGs+" repairVs = "+repairVs+" gsm = "+gsm);						
		Plan ret;
		Plan lpc = fp;
 		if (lpc.size() < 2) {
			ret = lpc;
		} else {
			//f.pln(" generateTCPs: ENTER ----------------------------------- lpc = "+lpc.toString()+" "+repairTurn);	
            boolean addMiddle = true;
            boolean flyOver = false;
            //DebugSupport.dumpPlan(lpc, "makeKinematicPlan_lpc");
            lpc = repairPlan(lpc, repairTurn, repairGs, repairVs, gsm,flyOver, addMiddle, bankAngle, gsAccel, vsAccel);
  			//f.pln(" generateTCPs: generateTurnTCPs ---------------------------------------------------------------------");
			Plan kpc0 = markVsChanges(lpc);
			Plan kpc = generateTurnTCPs(kpc0, bankAngle);
			//f.pln("$$ makeKinematicPlan: kpc="+kpc.toStringGs());					//DebugSupport.dumpPlan(kpc3, "generateTCPs_gsTCPs");
			//DebugSupport.dumpPlan(kpc, "makeKinematicPlan_Turn");
			if (kpc.hasError()) {
				ret = kpc;
			} else {
				//f.pln(" generateTCPs: fixGS ----------------------------------- "+kpc.isWellFormed());
				Plan kpc2 = kpc;
				if (gsm != GsMode.PRESERVE_TIMES) {
					kpc2 = fixGS(lpc, kpc, gsAccel, gsm == GsMode.PRESERVE_RTAS);
				}
				//DebugSupport.dumpPlan(kpc2, "generateTCPs_fixgs");
				//f.pln(" generateTCPs: generateGsTCPs ----------------------------------- "+kpc2.isWellFormed());
				// ********************
				Plan kpc3 = generateGsTCPs(kpc2, gsAccel, gsm);
				//f.pln("$$ makeKinematicPlan: kpc3="+kpc3.toStringGs());					//DebugSupport.dumpPlan(kpc3, "generateTCPs_gsTCPs");
				if (kpc3.hasError()) {
					ret = kpc3;
				} else {
					//f.pln(" generateTCPs: makeMarkedVsConstant ----------------------------------- "+kpc3.isWellFormed());
					// *******************
					Plan kpc4 = makeMarkedVsConstant(kpc3);
					//DebugSupport.dumpPlan(kpc4, "generateTCPs_vsconstant");
					if (kpc4.hasError()) {
						ret = kpc4;
					} else {
						//f.pln(" generateTCPs: generateVsTCPs ----------------------------------- "+kpc4.isWellFormed());
						Plan kpc5 = generateVsTCPs(kpc4, vsAccel);
						//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
						//f.pln("$$ makeKinematicPlan: kpc5="+kpc5.toStringGs());	
						cleanPlan(kpc5);
						//kpc5.setPlanType(Plan.PlanType.KINEMATIC);
						//f.pln(" generateTCPs: DONE ----------------------------------- "+kpc5.isWellFormed());
						ret = kpc5;
					}
				}
			}
		}
		ret.setNote(fp.getNote());
		return new Plan(ret);
	}


	public static Plan repairPlan(Plan lpc, boolean repairTurn, boolean repairGs, boolean repairVs, GsMode gsm,
			boolean flyOver, boolean addMiddle, double bankAngle, double gsAccel, double vsAccel) {
		if (repairTurn) {	
			//f.pln(" generateTCPs: linearRepairShortTurnLegs -----------------------------------");
			lpc = linearRepairShortTurnLegs(lpc, bankAngle, addMiddle);
		}
        //f.pln("repairPlan turn :"+lpc.toString());		
		if (lpc.hasError()) {
			//f.pln(" $$$0 generateTCPs: repair failed! "+lpc.getMessageNoClear());
		} else {
			if (repairGs && gsm != GsMode.PRESERVE_TIMES)  {
				//lpc = (Plan) linearRepairShortGsLegs(lpc,gsAccel);
				//f.pln(" generateTCPs: linearRepairShortGsLegsNew -----------------------------------");
				lpc = (Plan) linearRepairShortGsLegsNew(lpc,gsAccel,gsm);
			}
            //f.pln("repairPlan gs :"+lpc.toString());		
			if (lpc.hasError())  {
				//f.pln(" $$$1 generateTCPs: repair failed! "+lpc.getMessageNoClear());
			} else {
				if (repairVs) {
					//f.pln(" generateTCPs: linearRepairShortVsLegs -----------------------------------");
					lpc = linearRepairShortVsLegs(lpc,vsAccel);
					if (lpc.hasError()) {
						//f.pln(" $$$2 generateTCPs: repair failed! "+lpc.getMessageNoClear());
					} else {
						if (repairTurn && flyOver) {
							lpc = removeInfeasibleTurnsOver(lpc,bankAngle);						
						} else if(repairTurn) {
							//f.pln(" generateTCPs: removeInfeasibleTurns -----------------------------------");
							lpc = removeInfeasibleTurns(lpc,bankAngle);
						}
						if (lpc.hasError()) {
							//f.pln(" $$$3 generateTCPs: repair failed! "+lpc.getMessageNoClear());
						} else {
							if (gsm == GsMode.CONSTANT_GS) { 
								lpc = PlanUtil.makeGSConstant_No_Verts(lpc);
							}
							if (lpc.hasError()) {
								//f.pln(" $$$4 generateTCPs: repair failed! "+lpc.getMessageNoClear());
							}
						}
						//DebugSupport.dumpPlan(lpc, "generateTCPs_after_repair", 0.0);
					}
				}
			}
		}
		return lpc;
	}
	
	
	public static Plan repairShortLegs(Plan lpc, GsMode gsm,
			boolean flyOver, boolean addMiddle, double bankAngle, double gsAccel, double vsAccel) {
		lpc = linearRepairShortTurnLegs(lpc, bankAngle, addMiddle);
		if (!lpc.hasError()) {
			if (gsm != GsMode.PRESERVE_TIMES)  {
				lpc = (Plan) linearRepairShortGsLegs(lpc,gsAccel);
			}
			if (!lpc.hasError())  {
				lpc = linearRepairShortVsLegs(lpc,vsAccel);
			}
		}
		return lpc;
	}


	/**
	 * 
	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points. 
	 *  Also all TCPs should reference points in a feasible plan. 
	 *  If the trajectory is modified, it will have added, modified, or deleted points.
	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
	 *	@param fp input plan (is linearized if not already so)
	 *  @param bankAngle maximum allowed (and default) bank angle for turns
	 *  @param gsAccel    maximum allowed (and default) ground speed acceleration (m/s^2)
	 *  @param vsAccel    maximum allowed (and default) vertical speed acceleration (m/s^2)
	 *  @param repairTurn attempt to repair infeasible turns as a preprocessing step
	 *  @param repairGs attempt to repair infeasible gs accelerations as a preprocessing step
	 *  @param repairVs attempt to repair infeasible vs accelerations as a preprocessing step
	 *  @param constantGS if true, produces a constant ground speed kinematic plan with gs being average of linear plan
	 */
	public static Plan makeKinematicPlan(Plan fp, double bankAngle, double gsAccel, double vsAccel, 
			boolean repairTurn, boolean repairGs, boolean repairVs, boolean constantGS) {
		//double minVsChangeRecognized = vsAccel*getMinTimeStep(); // this is the minimum change all
		//f.pln(" $$$ minVsChangeRecognized = "+minVsChangeRecognized);
		//double minVsChangeRecognized =  minVsChangeRecognizedDefault;
		GsMode gsm = GsMode.PRESERVE_GS;
		if (constantGS) gsm = GsMode.CONSTANT_GS;
		return makeKinematicPlan(fp,bankAngle, gsAccel, vsAccel, repairTurn, repairGs, repairVs, gsm);
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
		Plan traj = makeKinematicPlan(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair, constantGS);
		return traj;
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
	 */
	public static Plan makeKinematicPlan(Plan fp, double bankAngle, double gsAccel, double vsAccel, 
			boolean repair) {
		Plan traj = makeKinematicPlan(fp,bankAngle, gsAccel, vsAccel, repair, repair, repair, false);
		return traj;
	}


	/**
	 * Remove records of deleted points and make all remaining points "original"
	 * Used to clean up left over generation data
	 * @param fp
	 */
	public static void cleanPlan(Plan fp) {
		//fp.clearDeletedPoints();
		for (int i = 0; i < fp.size(); i++) {
			fp.set(i,fp.point(i).makeOriginal());
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

	/**
	 * Returns true if turn at i can be inscribed in the available leg space. 
	 */
	public static boolean turnIsFeas(Plan lpc, int i, NavPoint BOT, NavPoint EOT) {
		NavPoint np2 = lpc.point(i);
		double d1new = BOT.distanceH(np2);
		double d2new = np2.distanceH(EOT);				
		int ipTC = prevTrackChange(lpc,i);
		int inTC = nextTrackChange(lpc,i);
		double d1old = lpc.point(ipTC).distanceH(np2);
		double d2old = np2.distanceH(lpc.point(inTC));	
		boolean rtn = true;
		//f.pln(" $$$$ generateTurnTCPs: "+(BOT.time()-getMinTimeStep())+" "+fp.getTime(i-1)+" "+EOT.time()+getMinTimeStep()+" "+fp.getTime(i+1));
		if (BOT.time()-getMinTimeStep() <= lpc.getTime(ipTC) || EOT.time()+getMinTimeStep() >= lpc.getTime(inTC) 
				|| d1new+Constants.get_horizontal_accuracy() >= d1old || d2new+Constants.get_horizontal_accuracy() >= d2old) {
			//traj = new PlanCore(lpc); // revert to initial plan
			//traj.addError("ERROR in TrajGen.generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.",i);
			//printError("ERROR in TrajGen.generateHorizTCPs: cannot achieve turn "+i+" from surrounding points.");
			if (verbose) {
				f.pln("####### i = "+i+" ipTC = "+ipTC+" inTC = "+inTC+" np2 =  "+np2.toStringFull());
				f.pln("####### BOT.time() = "+BOT.time()+" <? fp.getTime(ipTC) = "+lpc.getTime(ipTC));
				f.pln("####### EOT.time() = "+EOT.time()+" >? fp.getTime(inTC) = "+lpc.getTime(inTC));
				f.pln("####### d1new = "+d1new+" >? d1old = "+d1old);
				f.pln("####### d2new = "+d2new+" >? d2old = "+d2old);
				f.pln("####### turnIsFeas: FAIL on lpc = "+lpc);
			} 
			rtn = false;
		}
		return rtn;
	}



	/**
	 * This takes a plancore lpc and returns a kinematic plan core that has all points with vertical changes as "AltPreserve" points, with all other points
	 * being "Original" points.  Beginning and end points are always marked.
	 * @param lpc source plan
	 * @param minVsChangeRecognized minimum vs change recognized (m/s)
	 * @return kinematic plan with marked points
	 */
	private static Plan markVsChanges(Plan lpc) {
		//f.pln(" markVsChanges: lpc = "+lpc);
		String name = lpc.getName();
		Plan kpc = new Plan(name);
		if (lpc.size() < 2) {
			kpc = new Plan(lpc);
			kpc.addError("TrajGen.markVsChanges: Source plan "+name+" is too small for kinematic conversion");
			//AugmentedPlan apc = new AugmentedPlan(kpc);
			//apc.etype = ErrType.UNKNOWN;
			//return apc;
			return kpc;
		}
		kpc.add(lpc.point(0)); // first point need not be marked
		//		double minVsChangeRecognized = maxVsAccel*getMinTimeStep(); // this is the minimum change all
		for (int i = 1; i < lpc.size(); i++) {
			NavPoint np = lpc.point(i);
			double vs1 = lpc.initialVelocity(i-1).vs();
			double vs2 = lpc.initialVelocity(i).vs();
			if (Math.abs(vs1) > maxVs) {
				kpc.addWarning("Input File has vertical speed exceeding "+Units.str("fpm",maxVs)+" at "+(i-1));
			}
			//f.pln("markVsChanges "+i+" original vs1="+vs1+" vs2="+vs2+" minvs="+minVsChangeRecognized);				
			if (Util.within_epsilon(vs1, vs2, MIN_ACCEL_TIME)) {
				//				if (np.isOriginal()) {
				kpc.add(np);					
				//				} else {
				//					kpc.add(np.makeOriginal());
				//				}
			} else {
				//f.pln("$$%% markVsChanges "+i+" marked ALT PRESERVE np = "+np.makeAltPreserve());				
				kpc.add(np.makeAltPreserve());
			}
		}
		//f.pln(" markVsChanges kpc = "+kpc.toString());
		//		kpc.add(lpc.point(lpc.size()-1)); // last point need not be marked
		return kpc;
	}


	/**
	 * Kinematic generations pass that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It only assumes legs are long enough to support the turns.
	 * kpc will have marked vs changes (if any).
	 */
	// bank angle must be nonnegative!!!!!
	private static Plan generateTurnTCPsOver(Plan lpc, Plan kpc, double bankAngle) {
		//f.pln("$$$ generateTurnTCPsOver: bankAngle = "+Units.str("deg",bankAngle));
		//f.pln(" $$$ generateTurnTCPsOver: kpc = "+kpc);
		Plan traj = new Plan(kpc); // the current trajectory based on working
		//traj.setPlanType(Plan.PlanType.KINEMATIC);
		int j = 1;
		for (int i = 1; i+1 < kpc.size(); i++) {
			//double tm = kpc.getTime(i);
			Velocity vin =  traj.finalVelocity(j-1);          //         Velocity vin = kpc.finalVelocity(i-1);	
			Velocity vi1 = traj.initialVelocity(j);
			if (Util.almost_equals(vi1.gs(),0.0)) {
				printError(" ###### WARNING: Segment from point "+(i)+" to point "+(i+1)+" has zero ground speed!");
			}
			double turnTime = Kinematics.turnTime(vin.gs(),Util.turnDelta(vin.trk(), vi1.trk()),bankAngle);
			//f.pln("$$ generateTurnTCPsOver: for i = "+i+" j = "+j+" turnTime = "+turnTime);
			if (turnTime >= getMinTimeStep()) {
				NavPoint np = kpc.point(i);
				//f.pln("\n generateTurnTCPsOver:  i = "+i+" j = "+j+"  np = "+np.toStringFull()+" vin = "+vin+" vi1 = "+vi1);
				Position so = np.position();
				//Velocity vo = kpc.finalVelocity(i-1);
				double   to = traj.point(j).time();    // lpc(i) time no longert valid
				//f.pln(" $$$ generateTurnTCPsOver: to = "+to+" traj.point(j).time() = "+ traj.point(j).time());
				NavPoint wp = kpc.point(i+1);
				Position wpp = wp.position();               
				double R = Kinematics.turnRadius(vin.gs(), bankAngle);                
				Quad<Position,Velocity,Double,Integer> dtp = ProjectedKinematics.directToPoint(so,vin,wpp,R);
				int turnDir = dtp.fourth;
				//f.pln(" $$$ generateTurnTCPsOver: so="+so+ " vin="+vin+" wpp="+wpp);
				//f.pln(" $$$ generateTurnTCPsOver: dtp="+dtp);	    
				if (dtp.third < 0) {
					//f.pln(" $$$$$$$$ generateTurnTCPsOver: not enough time !!! at point "+i+" R  = "+Units.str("nm",R));
					traj.addError("TrajGen.generateTurnTCPsOver: not enought time to complete turn before next point "+i,1); 
					//AugmentedPlan atraj = new AugmentedPlan(traj);
					//atraj.etype = ErrType.TURN_OVERLAPS_E;
					//atraj.lpcErrorPt = i;
					//return atraj;
					return traj;
				} else {
					double turnTm = dtp.third;
					//f.pln(" $$$ generateTurnTCPsOver: turnTm = "+turnTm); 
					NavPoint eot_NP = new NavPoint(dtp.first,to+turnTm);
					//f.pln(" $$$ generateTurnTCPsOver: ADD eot_NP = "+eot_NP+" dtp.second = "+dtp.second+" dtp.third = "+dtp.third);
					//f.pln(" $$$ generateTurnTCPsOver: traj.point(j+1) = "+traj.point(j+1));
					if (wp.time() < eot_NP.time()) {
						//f.pln(" $$$$$$  generateTurnTCPsOver: not enough time !!!!!! at point "+i+"  "+wp.time()+"  < +"+eot_NP.time());
						traj.addError("TrajGen.generateTurnTCPsOver: not enought time to reach direct to point  before next point "+i,1); 
						//AugmentedPlan atraj = new AugmentedPlan(traj);
						//atraj.etype = ErrType.TURN_OVERLAPS_B;
						//atraj.lpcErrorPt = i;
						//return atraj;
						return traj;
					}
					Velocity v2 = eot_NP.initialVelocity(wp);
					//f.pln(" $$$ generateTurnTCPsOver: v2 = "+v2);        			
					//Velocity vx = traj.point(j).initialVelocity(eot_NP);
					//f.pln(" $$$ generateTurnTCPsOver: vx = "+vx);        			      			
					double omega = turnDir*vin.gs()/R;    // turnRate
					//f.pln("  $$$ generateTurnTCPsOver: omega = "+omega);
					NavPoint npBOT = np.makeBOT(np.position(),to, omega, vin);
					NavPoint npEOT = np.makeEOT(eot_NP.position(),eot_NP.time(), v2);	

					//f.pln(" $$$ generateTurnTCPsOver: remove j = "+j+" traj.point(j) = "+traj.point(i));
					traj.remove(j);
					traj.add(npBOT);
					traj.add(npEOT);
					double motTime = (npBOT.time()+npEOT.time())/2.0;
					Position motPos = traj.position(motTime);
					//f.pln("### generateTurnTCPsOver: motPos = "+motPos);
					//NavPoint npMOT = np.makeTurnMid(motPos,motTime, omega, v2);	
					NavPoint npMOT = np.makePosition(motPos).makeTime(motTime).makeVelocityIn(v2);		
					traj.add(npMOT);
					//f.pln("### generateTurnTCPsOver: npBOT = "+npBOT.toStringFull());
					//f.pln("### generateTurnTCPsOver: npEOT = "+npEOT.toStringFull());
					//f.pln("### generateTurnTCPsOver: npMOT = "+npMOT.toStringFull());
					Velocity v1 = np.initialVelocity(kpc.point(i+1));          		 
					double targetGs = v1.gs();
					//kpc.add(npEOT);        			
					//i++;     			        			
					j = j + 3;
					double nt = traj.linearCalcTimeGSin(j, targetGs);
					double timeShift = nt - traj.point(j).time();
					//f.pln(" $$$$$$$$$####### fixGS: for (j) = "+(j)+" targetGs = "+Units.str("kn",targetGs)+" timeShift = "+timeShift+" nt = "+nt);    	            
					traj.timeshiftPlan(j, timeShift);
					//f.pln("  ### generateTurnTCPsOver: SET j = "+j);
				}
			} else {
				j = j+1;
			}
		}
		//f.pln("");
		//f.pln(" ### generateTurnTCPsOver: traj = "+traj.toString());
		return traj;
	}

	
	/**
	 * Return the BOT, MOT, and EOT TCPs for a turn. 
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param R Radius of the turn
	 * @return BOT,MOT,EOT triple.   
	 * NOTE.   MOT.alt = np.alt()
	 * velocity in metadata is set correctly for BOT, with track values modified to match the MOT and EOT points
	 */
	protected static Triple<NavPoint,NavPoint,NavPoint> turnGenerator(NavPoint np1, NavPoint np2, NavPoint np3, double R) {
		Vect2 pt1;
		Vect2 pt2;
		Vect2 pt3;
		Position p1 = np1.position();
		//f.pln("\n >>>>>>>>>>>>>>>>>.. TurnGenerator: np1 = "+np1+" np2 = "+np2+" np3 = "+np3);
		Position p2 = np2.position();
		Position p3 = np3.position();
		// convert the points to Euclidean vectors (if necessary)
		EuclideanProjection proj = Projection.createProjection(p2.lla().lat(), p2.lla().lon(), 0); // preserves altitude
		if (p2.isLatLon()) {  
			double chkLen = Units.from("nm",10.0);
			LatLonAlt p1lla = p1.lla();
			if (p2.distanceH(p1) > chkLen) {
				p1lla = GreatCircle.linear_gc(p2.lla(), p1.lla(), chkLen);
				//f.pln(" $$ TurnGenerator is using p1lla "+p1lla+" instead of "+p1.lla());
			}
			LatLonAlt p3lla = p3.lla();
			if (p2.distanceH(p3) > chkLen) {
				p3lla = GreatCircle.linear_gc(p2.lla(), p3.lla(), chkLen);
				//f.pln(" $$ TurnGenerator is using p3lla "+p3lla+" instead of "+p3.lla());
			}
			pt1 = proj.project2(p1lla);
			pt2 = proj.project2(p2.lla());
			pt3 = proj.project2(p3lla);
		} else {
			pt1 = p1.vect2();
			pt2 = p2.vect2();
			pt3 = p3.vect2();
		}
		Vect2 ao = pt3.Sub(pt2);
		Vect2 bo = pt1.Sub(pt2);
		Vect2 ahat = ao.Hat();
		Vect2 bhat = bo.Hat();
		Vect2 v = ahat.Add(bhat);
		double k = R/Math.sqrt((v.sqv() - Util.sq(v.dot(ahat))));
		Vect2 w = v.Scal(k);
		Vect2 wdota = ahat.Scal(w.dot(ahat));
		Vect2 wdotb = bhat.Scal(w.dot(bhat));
		Vect2 Center = pt2.Add(w);
		Vect2 BOT = pt2.Add(wdotb);
		Vect2 EOT = pt2.Add(wdota);
		double gs1 = np1.groundSpeed(np2);
		double distAB = BOT.Sub(EOT).norm();
		double alpha = 2*(Math.asin(distAB/(2*R))); 
		double d2 = alpha*R;	//arc length
		Velocity v1 = np1.initialVelocity(np2);
		Velocity v2 = np2.initialVelocity(np3);
		// make the time of the mot = the time of the middle vertex
		double tMOT = np2.time();
		double turnTime = d2/gs1;
		//f.pln(" $$$$$ TurnGenerator: turnTime = "+turnTime+" alpha = "+Units.str("deg",alpha)+" gs1 = "+Units.str("kn",gs1));
		double tBOT = tMOT - turnTime/2;
		double tEOT = tMOT + turnTime/2;
		double vs1 = np1.initialVelocity(np2).vs();
		double vs2 = np2.initialVelocity(np3).vs();
		double altMOT = np2.alt();
		double altBOT = altMOT - vs1*turnTime/2;
		double altEOT = altMOT + vs2*turnTime/2;
		// calculate MOT position
		Vect2 vhat = pt2.Sub(Center).Hat();
		Vect2 mot = Center.Add(vhat.Scal(R));
		Vect3 v3BOT = new Vect3(BOT.x,BOT.y,altBOT);
		Vect3 v3MOT = new Vect3(mot.x,mot.y,altMOT);
		Vect3 v3EOT = new Vect3(EOT.x,EOT.y,altEOT);
		Position motPos;
		Position botPos;
		Position eotPos;
		if (p1.isLatLon()) {  
			motPos = new Position(proj.inverse(v3MOT));
			botPos = new Position(proj.inverse(v3BOT));
			eotPos = new Position(proj.inverse(v3EOT));                         
		} else {
			motPos = new Position(v3MOT);
			botPos = new Position(v3BOT);
			eotPos = new Position(v3EOT);
		}
		//double  vinTrk2 = new NavPoint(botPos,tBOT).initialVelocity(np2).trk();
		double vinTrk = pt2.Sub(BOT).trk();
		//f.pln(" $$$ vinTrk = "+Units.str8("deg",vinTrk)+" vinTrk2 = "+Units.str8("deg",vinTrk2));
		//		double  voutTrk = new NavPoint(eotPos,tEOT).initialVelocity(np3).trk();
		Velocity vin = v1.mkTrk(vinTrk);
		double omega = vin.gs()/R*Util.turnDir(vin.trk(), v2.trk());    // turnRate
//		if (tBOT < 0) {
//		    f.pln(" $$$ turnGenerator tBOT = "+tBOT+" vin = "+vin);
//		}
		double trk2 = vin.trk() + omega * (tMOT-tBOT); // make vin track appropriate for each point
		double trk3 = vin.trk() + omega * (tEOT-tBOT);
		NavPoint npBOT = np2.makeOriginal().makeBOT(botPos,tBOT, omega, vin).makeLabel(np2.label());   // only BOT has label from np2
        //f.pln(" $$$ turnGenerator: npBOT = "+npBOT.toStringFull()+"  alpha = "+alpha+" turnTime = "+turnTime);
		NavPoint npMOT = np2.makeMidpoint(motPos,tMOT,vin.mkTrk(trk2)).makeLabel("");   
		//f.pln(" $$$ turnGenerator: npMOT = "+npMOT.toStringFull());
		NavPoint npEOT = np2.makeOriginal().makeEOT(eotPos,tEOT, vin.mkTrk(trk3)).makeLabel("");	
        //f.pln(" $$$ turnGenerator: npEOT = "+npEOT.toStringFull()+"  alpha = "+alpha+" turnTime = "+turnTime);
		return new Triple<NavPoint,NavPoint, NavPoint>(npBOT,npMOT,npEOT);
	}

	/**
	 * Kinematic generations pass that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It only assumes legs are long enough to support the turns.
	 * kpc will have marked vs changes (if any).
	 */
	// bank angle must be nonnegative!!!!!
	protected static Plan generateTurnTCPs(Plan kpc, double bankAngle) {
		//f.pln("$$$ generateTurnTCPs: bankAngle = "+Units.str("deg",bankAngle));
		//f.pln(" $$$ generateTurnTCPs: kpc = "+kpc);
		Plan traj = new Plan(kpc); // the current trajectory based on working
//f.pln("generateTurnTCPs: start="+kpc.toString());
		//int j = 1;
		for (int i = 1; i+1 < kpc.size(); i++) {
			//double tm = kpc.getTime(i);
			Velocity vf0 = kpc.finalVelocity(i-1);
			Velocity vi1 = kpc.initialVelocity(i);
//f.pln("generateTurnTCPs:  i = "+i+" vf0 = "+vf0+" vi1 = "+vi1);
			//if (trajKinematicsTrack && fp.isaTurn(i) && bankAngle >= 0.0) {
			if (Util.almost_equals(vi1.gs(),0.0)) {
				printError("TrajGen.generateTurnTCPs ###### WARNING: Segment from point "+(i)+" to point "+(i+1)+" has zero ground speed!");
				continue;
			}
			//			if (trajKinematicsTrack && Math.abs(vf0.trk() - vi1.trk()) > Units.from("deg",1.0) && bankAngle >= 0.0) {
			double turnTime = Kinematics.turnTime(vf0.gs(),Util.turnDelta(vf0.trk(), vi1.trk()), bankAngle);
            //f.pln(" $$ generateTurnTCPs: "+turnTime+" >=? "+getMinTimeStep());
			if (turnTime >= getMinTimeStep()) {
				NavPoint np1 = kpc.point(i-1); // get the point in the traj that corresponds to the point BEFORE fp(i)!
				NavPoint np2 = kpc.point(i);
				NavPoint np3 = kpc.point(i + 1);
				if (np3.time() - np2.time() < 0.1 && i+2 < kpc.size())  np3 = kpc.point(i + 2);
				//f.pln("$$$ generateTurnTCPs: t="+np2.time()+"   gs="+Units.to("knot", gs)+"   R = "+Units.str8("nm",R)+" omega = "+Units.str("deg/s",gs/R));
				//f.pln("generateTurnTCPs: np1 = "+np1+" np2 = "+np2+" np3 = "+np3);
				double gsIn = vf0.gs(); 
				//f.pln(" $$$ generateTurnTCPs: gsIn = "+Units.str8("kn",gsIn));
				NavPoint BOT; 
				NavPoint MOT; 
				NavPoint EOT; 
				double bank = Math.abs(bankAngle);
				if (bank == 0) {
					traj.addError("ERROR in TrajGen.generateTurnTCPs: specified bank angle is 0");
					bank = 0.001; // prevent divisions by 0.0
				}
				double R = np2.getRadius();
				if (R == 0.0) {
				   R = Kinematics.turnRadius(gsIn, bank);
				}
				Triple<NavPoint,NavPoint,NavPoint> tg = TrajGen.turnGenerator(np1,np2,np3,R);	 
				//Triple<NavPoint,NavPoint,NavPoint> tg = TurnGeneration.turnGenerator(np1,np2,np3,R);		 // $$$$$$$$$$$$$$$$ RWB NEW
                //f.pln(" $$ generateTurnTCPs: tg.1="+tg.first.toString()+" tg.2="+tg.second.toString()+" tg.3="+tg.third.toString());
				BOT = tg.getFirst(); //.appendName(setName);
				MOT = tg.getSecond();//.appendName(setName);
				EOT = tg.getThird(); //.appendName(setName);
				// Calculate altitudes based on kpc, NOTE THAT these times may be out of range of kpc and INVALIDs will be produced
				double botAlt; 
				if (BOT.time() < kpc.getFirstTime()) {  // is this right?
					botAlt = kpc.point(0).alt();
				} else {
					botAlt = kpc.position(BOT.time()).alt();
				}								
				BOT = BOT.mkAlt(botAlt);
				//f.pln(" $$$$ BOT.time() = "+BOT.time()+" botAlt = "+Units.str("ft", botAlt)+" kpc.getFirstTime() = "+kpc.getFirstTime());
				MOT = MOT.mkAlt(kpc.position(MOT.time()).alt());
				//f.pln("$$$ generateTurnTCPs: MOT = "+MOT.toStringFull());
				double EotAlt;
				if (EOT.time() > kpc.getLastTime()) {  // is this right?
					EotAlt = kpc.getLastPoint().alt();
				} else {
					EotAlt = kpc.position(EOT.time()).alt();
				}
				EOT = EOT.mkAlt(EotAlt);
				//f.pln(" $$ generateTurnTCPs: BOT = "+BOT+"\n                      EOT = "+EOT);
				if (BOT.isInvalid() || MOT.isInvalid() || EOT.isInvalid()) {
					String label = "";
					if ( ! np2.label().equals("")) label = " ("+np2.label()+") ";
					traj.addError("ERROR in TrajGen.generateTurnTCPs: turn points at "+i+label+" invalid.",i);
					printError("ERROR in TrajGen.generateTurnTCPs: turn points at "+i+label+" invalid.");
					//f.pln(" #### BOT = "+BOT+" kpc.getFirstTime() = "+kpc.getFirstTime());
					//f.pln(" $$$ generateTurnTCPs: inValid TCPS: BOT = "+BOT+" EOT = "+EOT);
					return traj;
				}
				if (!turnIsFeas(kpc, i, BOT, EOT)) {
					//traj = new PlanCore(lpc); // revert to initial plan
					String label = "";
					if ( ! np2.label().equals("")) label = " ("+np2.label()+") ";
					traj.addError("ERROR in TrajGen.generateTurnTCPs: cannot achieve turn at point "+i+label+" from surrounding points",i);
					printError("ERROR in TrajGen.generateTurnTCPs: cannot achieve turn "+i+label+" from surrounding points at "+i);
					//f.pln(" $$ generateTurnTCPs: cannot achieve turn i = "+i);
					return traj;
				} else 	if (EOT.time() - BOT.time() > 2*getMinTimeStep()) { // ignore small changes
					String label = "";
					if ( ! np2.label().equals("")) label = " ("+np2.label()+") ";
					if (traj.inTrkChange(BOT.time())) {
						traj.addError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn in time at "+i+label,i);
						printError("ERROR in TrajGen.generateTurnTCPs: BOT in new turn overlaps existing turn int time at "+i+label);
						return traj;

					} else if (traj.inTrkChange(EOT.time())) {
						traj.addError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn in time at "+i+label,i);
						printError("ERROR in TrajGen.generateTurnTCPs: EOT in new turn overlaps existing turn int time at "+i+label);
						return traj;
					} 					
					//f.pln("Creating turn at "+np2.time());
					int jj_MOT = traj.getIndex(kpc.point(i).time());
					if (traj.point(jj_MOT).isAltPreserve()) {
						MOT = MOT.makeAltPreserve();
					}
					//f.pln("$$$ generateTurnTCPs(2): MOT = "+MOT.toStringFull());
					if (!kpc.point(i).isTCP()) {
						//f.pln(" $$$ generateTurnTCPs: kpc.point("+i+").time() = "+kpc.point(i).time()+" jj = "+jj);
						traj.remove(jj_MOT); 
					}
					int ixBOT = traj.add(BOT);
					int ixMOT = traj.add(MOT);
					traj.add(EOT);
					//f.pln("$$ generateTurnTCPs:  ixMOT = "+ixMOT);
					movePointsWithinTurn(traj,ixMOT);	
					
					//f.pln(" at ixBOT = "+ixBOT+" traj.initialVelocity(ixBOT) "+traj.initialVelocity(ixBOT));
					//f.pln(" at ixBOT = "+ixBOT+" traj.finalVelocity(ixBOT-1) "+traj.finalVelocity(ixBOT-1));

					double courseIn = traj.finalVelocity(ixBOT-1).trk();
					double courseOut = traj.initialVelocity(ixBOT).trk();
					if (Util.turnDelta(courseIn,courseOut) > Units.from("deg", 10)) {
						traj.addError("ERROR in TrajGen.generateTurnTCPs:  track into BOT not equal to track out of BOT at "+ixBOT+label);
						return traj;
					}					
				} else if (turnTime > getMinTimeStep()*minorVfactor && turnTime < getMinTimeStep()) {
					// mark points which have a detectable but minor turn
					//traj.set(i, traj.point(i).makeMinorTrkChange());
					traj.set(i, traj.point(i)); // .makeLabel("minorTrkChange"));
					//f.pln("$$ generateTurnTCPs: minor turn at point "+i);					
				} else {
					//f.pln("$$ generateTurnTCPs: No turn at point "+i);					
				}
			}
		}//for
		//f.pln(" $$>>>>>>>>> generateTurnTCPs: traj = "+traj.toStringGs());
		return traj;
	}
	
	

//	static void printGs(Plan kpc) {
//		for (int i = 0; i < kpc.size(); i++) {
//			f.pln(" gs at i = "+i+" = "+Units.str("kn", kpc.initialVelocity(i).gs(),8));
//		}		
//	}
	
	
	/**
	 * Alters point times in fp so that gs in return plan matches the gs in lpc
	 * @param lpc
	 * @param fp
	 * @param gsAccel
	 * @return
	 */
	private static Plan fixGS(Plan lpc, Plan fp, double gsAccel, boolean preserveRTAs) {
		Plan traj = new Plan(fp);
		//f.pln(" fixGS: ---------------------------  lpc = "+lpc.toOutput(false,0,15));
		//f.pln(" fixGS: ---------------------------  traj = "+traj.toOutput(false,0,15));
		for (int i = traj.size()-1; i > 0; i--) {
			if ( traj.point(i).isBOT() || (!traj.point(i).isTCP() && !traj.inTrkChange(traj.point(i).time())) ) {
				int lpcSegment = lpc.getSegment(traj.getTime(i));
				if (!traj.point(i).isBOT() && !traj.point(i).isBVS()) lpcSegment = lpcSegment -1;
				//f.pln(" $$ fixGS: lpcSegment = "+lpcSegment+" traj.getTime(i) = "+traj.getTime(i)+" traj.point(i) = "+traj.point(i));
				double targetGS = lpc.initialVelocity(lpcSegment).gs();
				if (targetGS == 0.0) {
					traj.addError("TrajGen.fixGS:  attempt to make ground speed 0 at i = "+i);
					//f.pln(" TrajGen.fixGS: lpcSegment = "+lpcSegment);
					//f.pln(" TrajGen.fixGS: lpc = "+lpc);
					//f.pln(" TrajGen.fixGS: traj = "+traj);
					return traj;
				}
				//f.pln(" $$$$$$$$$$$$$$ fixGS: timeShift point: traj.point(i) = "+traj.point(i).toStringFull());
				double nt = traj.linearCalcTimeGSin(i, targetGS);
				double timeShift = nt - traj.point(i).time();
				//f.pln(" #######>>>>> fixGS: for i = "+i+" targetGS = "+Units.str("kn",targetGS)+" timeShift = "+timeShift+" nt = "+nt);
				traj.timeshiftPlan(i, timeShift, preserveRTAs);
			}

		}
		//f.pln(" fixGS: ---------------------------  traj = "+traj);
		//printGs(traj);
		return traj;
	}


	/**
	 * Re-aligns points that are within a newly generated turn at j
	 */
	private static void movePointsWithinTurn(Plan traj, int j) {
		//f.pln(" ## movePointsWithinTurn: ------------------------------------ j = "+j+" traj = "+traj);
		int ipTC = traj.prevBOT(j);
		int inTC = traj.nextEOT(j);
		//f.pln(" ## movePointsWithinTurn: j = "+j+" ipTC =  "+ipTC+" inTC = "+inTC);
		for (int i = inTC-1; i > ipTC; i--) {            // delete in reverse order to preserve index          
			NavPoint npi = traj.point(i);
			//f.pln(" ## movePointsWithinTurn: npi("+i+") = "+npi.toStringFull());
			traj.remove(i);
			double iTm = npi.time();
			Position iPos = traj.position(iTm).mkAlt(npi.alt());
			//f.pln(" movePointsWithinTurn: move point at i = "+i+" iTm = "+iTm+" to iPos = "+iPos);
			npi = new NavPoint(iPos,iTm).makeMovedFrom(npi);
			//f.pln(" movePointsWithinTurn: npi("+i+") = "+npi.toStringFull());
			traj.add(npi);
			if (npi.isBVS()) {
				NavPoint np = npi.makeVelocityIn(traj.finalVelocity(i-1));
				traj.set(i,np);				
			}
		}
	}

	// create GSC TCPS between np1 and np2 using vin from previous leg
	/**
	 * Generate gs acceleration zone between np1 and np2 
	 * @param np1 start point of the acceleration segment/where the speed change occurs
	 * @param np2 end point of the acceleration segment
	 * @param targetGs target gs
	 * @param vin velocity in
	 * @param gsAccel non negative (horizontal) acceleration
	 * @param gsm ground speed mode - preserve time and RTA may have a different result
	 * @return third component: accel time, or negative if not feasible
	 * NOTE!!! This has been changed so the accel time is always returned, but if it is less than getMinTimeStep(), there is no region and this has to be handled by the calling program!!!
	 */
	private static Triple<NavPoint,NavPoint,Double> gsAccelGenerator(NavPoint np1, NavPoint np2, double targetGs, Velocity vin, double gsAccel, GsMode gsm) {
		int sign = 1;
		double gs1 = vin.gs();
		//double gs2 = np1.initialVelocity(np2).gs(); // target Gs
		if (gs1 > targetGs) sign = -1;
		double a = Math.abs(gsAccel)*sign;
		//f.pln(" ##### gsAccelGenerator: np1 = "+np1+" np2 = "+np2+"   gs1 = "+Units.str("kn",gs1)+" gs2 = "+Units.str("kn",targetGs));
		double t0 = np1.time();
		//f.pln(" ##### gsAccelGenerator: Accelerate FROM gs1 = "+Units.str("kn",gs1)+" TO targetGs = "+Units.str("kn",targetGs)+" -------------");
		// if np1 is a TCP, we shift the begin point forward
		NavPoint b;
		NavPoint e;
		double accelTime = -1;
		double timeOffset = getMinTimeStep();
		if (!np1.isTCP()) {
			timeOffset = 0.0;	
		}
		String label = np1.label();
		//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>BEFORE gsAccelGenerator: np1 = "+np1.toStringFull());
		NavPoint np1b = np1.makeStandardRetainSource();    // ***RWB*** KCHANGE
		//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>AFTER gsAccelGenerator: np1b = "+np1b.toStringFull());
		//f.pln(" gsAccelGenerator: make BGSC from TCP!! np1b.tcpSourceTime = "+np1b.tcpSourceTime());
		b = np1b.makeBGS(np1b.linear(vin, timeOffset).position(), t0+timeOffset, a, vin).makeLabel(label); // .makeAdded();//.appendName(setName);
		//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> gsAccelGenerator: b = "+b.toStringFull());
		if (gsm == GsMode.PRESERVE_TIMES) { // || (gsm == GsMode.PRESERVE_RTAS) && np2.isFixedTime())) {
			double d = b.distanceH(np2);
			double t = np2.time()-b.time();
			Pair<Double,Double> p2 = Kinematics.gsAccelToRTA(gs1, d, t, gsAccel);
			//			double gs2 = p2.first;
			//f.pln("gsAccelGenerator b="+b+" np2="+np2+" gs1="+gs1+" d="+b.distanceH(np2)+" gs2="+p2.first+" aTime="+p2.second);
			accelTime = p2.second;
			if (accelTime < 0) {
				//f.pln("current gs = "+gs1+"  avg new gs ="+(d/t)+" over t="+t+"  a="+gsAccel);				
				return new Triple<NavPoint,NavPoint,Double>(np1b,np2,-1.0); // cannot complete
			} else if (accelTime < getMinTimeStep()) {
				return new Triple<NavPoint,NavPoint,Double>(np1b,np2,accelTime); // minor vs change
			}
			Pair<Position,Velocity> pv = ProjectedKinematics.gsAccel(b.position(), vin, accelTime, a);
			e = np1b.makeEGS(pv.first, accelTime+t0+timeOffset, vin); // .makeAdded();//.appendName(setName);
		} else {
			double d = b.distanceH(np2) - targetGs*getMinTimeStep(); 
			accelTime = (targetGs - gs1)/a;
			double remainingDist = d - accelTime * (gs1+targetGs)/2;
			//f.pln("$$$$$$$$$$$$ gsAccelGenerator: remainingDist = "+Units.str("nm",remainingDist));
			if (accelTime < getMinTimeStep()) {
				//f.pln(" $$$$$ gsAccelGenerator no GS TCPS needed, at time +"+np1b.time()+" accelTime = "+accelTime);
				return new Triple<NavPoint,NavPoint,Double>(np1b,np2,accelTime); // no change
			}
			if (remainingDist <= 0) {
				//f.pln(" ##### gsAccelGenerator: accelTime = "+accelTime+" remainingDist = "+Units.str("nm",remainingDist));
				return new Triple<NavPoint,NavPoint,Double>(np1b,np2,-1.0); // no change
			}
			//f.pln("gsAccelGenerator: GENERATE GS TCPS, acceltime = "+accelTime+" at time +"+np1b.time());
			// start moving in the current direction at the previous speed
			Pair<Position,Velocity> pv = ProjectedKinematics.gsAccel(b.position(), vin, accelTime, a);
			e = np1b.makeEGS(pv.first, accelTime+t0+timeOffset, vin); // .makeAdded();//.appendName(setName);
			//f.pln(" gsAccelGenerator: make EGSC from TCP!! tcpSourceTime = "+np1b.tcpSourceTime()+" point="+e+" srctm="+e.tcpSourceTime());
		}
		//f.pln(" $$$$$$$$$$$$$$$$$$ gsAccelGenerator: b = "+b.toStringFull()+"\n e = "+e.toStringFull());
		return new Triple<NavPoint,NavPoint,Double>(b,e,accelTime);
	}

	
	// EXPERIMENTAL
	private static Plan linearRepairShortGsLegsNew(Plan fp, double gsAccel, GsMode gsm) {
		Plan lpc = fp.copy();
		//f.pln(" $$ smoothShortGsLegs: BEFORE npc = "+lpc.toString());
		for (int j = 1; j < lpc.size()-1; j++) {
			NavPoint np1 = fp.point(j);
			NavPoint np2 = fp.point(j+1);
			if (fp.inTrkChange(np1.time())) { // this is new!!! 7/8
				continue;
			}
			Velocity vin = fp.finalVelocity(j-1);
			//			f.pln(i+" REGULAR: +++++++++++++++++++++++++++++  vin = "+vin);
			double targetGS = fp.initialVelocity(j).gs();   // get target gs out of lpc!
			if (Util.almost_equals(vin.gs(), 0.0) || Util.almost_equals(targetGS,0.0)) {
				fp.addWarning("TrajGen.generateGsTCPs: zero ground speed at index "+j);
				continue; 
			}
			Triple<NavPoint,NavPoint,Double> tcpTriple =  gsAccelGenerator(np1, np2, targetGS, vin, gsAccel, gsm); 
			//f.pln(" linearRepairShortGsLegsNew: for j = "+j+" tcpTriple = "+tcpTriple.toString());
            if (tcpTriple.third < 0) {
            	lpc = PlanUtil.linearMakeGSConstant(lpc, j-1,j+1);
            	//f.pln(" $$^^ REPAIRED GS ACCEL PROBLEM AT  j = "+j+" time = "+lpc.point(j).time());
            }
		}
        return lpc;
	}



	/**
	 * A pass in kinematic plan generation.  Assumes vertical change points have been marked and all turns have been generated and all legs have
	 * proper "end" gs (gsFix has been called).
	 * @param fp
	 * @param gsAccel
	 * @param repair
	 * @return
	 */
	private static Plan generateGsTCPs(Plan fp, double gsAccel, GsMode gsm) {
		Plan traj = new Plan(fp); // the current trajectory based on working
		//f.pln(" generateGsTCPs: ENTER ------------------------ ");
		for (int i = traj.size() - 2; i > 0; i--) {
			//f.pln(" ######>>>>>>>>>>> generateGsTCPs: i = "+i);
			NavPoint np1;
			NavPoint np2;
			Velocity vin;
			//Velocity vin2;
			double targetGS;
			np1 = traj.point(i);
			np2 = traj.point(i+1);
			if (traj.inTrkChange(np1.time())) { // this is new!!! 7/8
				continue;
			}
			vin = traj.finalVelocity(i-1);
			targetGS = traj.initialVelocity(i).gs();   // get target gs out of lpc!			
			if (Util.almost_equals(vin.gs(), 0.0) || Util.almost_equals(targetGS,0.0)) {
				traj.addWarning("TrajGen.generateGsTCPs: zero ground speed at index "+i);
				continue; 
			}			
			//f.pln(i+" ##### generateGsTCPs: Accelerate FROM gs1 = "+Units.str("kn",vin.gs())+" TO targetGs = "+Units.str("kn",targetGS)+" -------------"+np2);
			Triple<NavPoint,NavPoint,Double> tcpTriple =  gsAccelGenerator(np1, np2, targetGS, vin, gsAccel, gsm);
			//f.pln(" generateGsTCPs: for i = "+i+" tcpTriple = "+tcpTriple);
			if (tcpTriple.third >= getMinTimeStep()) {
				//f.pln(i+" #### generateGsTCPs: "+tcpTriple.third+" >= "+getMinTimeStep());
				//f.pln(i+"#### generateGsTCPs: vin = "+vin+" targetGS = "+Units.str("kn",targetGS));
				int j = i+2;               
				NavPoint GSCBegin = tcpTriple.first;
				NavPoint GSCEnd = tcpTriple.second;
				if (traj.point(i).isAltPreserve()) {
					GSCBegin = GSCBegin.makeAltPreserve();
					//f.pln(" $$$$$$ generateGsTCPs: make GSCBegin = "+GSCBegin+" AltPreserve!!");
				}
				if (!np1.isTCP()) {
					//f.pln(" #### generateGsTCPs remove point at time "+np1.time()+" index="+traj.getIndex(np1.time()));
					//f.pln("111"+traj);					
					traj.remove(i);  // assumes times have not changed from lpc to kpc
					j = j - 1;
				}
				traj.add(GSCBegin);				
				traj.add(GSCEnd);
				//f.pln(" #### generateGsTCPs: ADD GSCBegin = "+GSCBegin.toStringFull()+"\n                GSCEnd = "+GSCEnd.toStringFull());
				// GET THE GROUND SPEEDS BACK IN ORDER
				double nt = traj.linearCalcTimeGSin(j+1, targetGS);
				double timeShift = nt - traj.point(j+1).time();
                //f.pln(" #######>>>>> TrajGen.generateGsTCPs for j+1 = "+(j+1)+" targetGS = "+Units.str("kn",targetGS)+" timeShift = "+timeShift+" nt = "+nt);
				if (gsm != GsMode.PRESERVE_TIMES) {
					traj.timeshiftPlan(j+1, timeShift, gsm == GsMode.PRESERVE_RTAS);
				}
			} else if (tcpTriple.third < 0) {	
				//DebugSupport.dumpPlan(traj, "traj_genGsTCPs_repair", 0.0);
				printError(i+"TrajGen.generateGsTCPs ERROR:  we don't have time to accelerate to the next point i = "+i+": "+np2);
				traj.addError("TrajGen.generateGsTCPs ERROR:  we don't have time to accelerate to the next point i = "+i+" at time "+f.Fm1(np2.time()),i);
                //f.pln("  $$$$$$$$$ previous point (np1) = "+np1.toStringFull());
                //f.pln("  $$$$$$$$$ previous point (np2) = "+np2.toStringFull());
			} else if (tcpTriple.third > getMinTimeStep()*minorVfactor) {
				// mark known GS shift points
				// f.pln(" $$>> traj.point(i) = "+traj.point(i));
				//DebugSupport.dumpAsUnitTest(traj,true);
				//traj.set(i, traj.point(i).makeLabel("minorGsChange"));   //***** GETTING RID OF makeLabel CAUSES C++ INCONSISTENCY
				//f.pln("### generateGsTCPs: NO GSC NEEDED: at point i = "+i);
			}
		}
		//f.pln(" generateGsTCPs: END traj = "+traj);
		return traj;
	}

	
	/**       Generate vertical speed tcps centered around np2.  Compute the absolute times of BVS and EVS
	 * @param t1 = previous point's time (will not place tbegin before this point)
	 * @param t2 end time of the linear segment where the vertical speed change occurred
	 * @param tLast = time of the end of the plan 
	 * @param vs1 vertical speed into point at time t2
	 * @param vs2 vertical speed out of point at time t2
	 * @param vsAccel non negative (horizontal) acceleration
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
		double rtn = Math.abs((vs2 - vs1)/a);
		double tbegin = 0;
		double tend = 0;
		//f.pln("### vsAccelGenerator: vs1 = "+Units.str("fpm",vs1)+" vs2 = "+Units.str("fpm",vs2));
		if (rtn > 0) {
			//double t2 = np2.time();
			double dt = Kinematics.vsAccelTime(vs1, vs2, Math.abs(a));
			//f.pln("### vsAccelGenerator: tbegin = "+f.Fm2(tbegin)+" tend = "+f.Fm2(tend)+" t1 = "+t1+" t2 = "+t2+" dt = "+f.Fm2(dt));
			tbegin = t2 - dt/2.0;
			tend = tbegin + dt;			
			if (tbegin < t1 || tend > tLast) {
				rtn = -1;
			} else {
				rtn = dt;
			}
			//f.pln("### vsAccelGenerator: tbegin = "+tbegin+" t1  = "+t1+" t2 = "+t2+" dt = "+dt+" rtn = "+rtn);
		}
		return new Triple<Double,Double,Double>(tbegin,tend,rtn);
	}

	
	// return tbegin, tend, accel, -1 values or accel = 0.0 indicate no new tcp needed, may label point as minor vs as side effect
	private static Triple<Double,Double,Double> calcVsTimes(int i, Plan kpc, Plan traj, double vsAccel) {
		NavPoint np1 = traj.point(i-1);
		NavPoint np2 = traj.point(i);
		NavPoint np3 = traj.point(i+1);
		//f.pln("$$### calcVsTimes: np2 = "+np2+" np3 = "+np3+" dt23 = "+(np3.time()-np2.time()));
		double vs1 = np1.verticalSpeed(np2);
		double vs2 = np2.verticalSpeed(np3);
		//f.pln("$$### calcVsTimes: vs1 = "+Units.str("fpm",vs1)+" vs2 = "+Units.str("fpm",vs2));
		int sign = 1;
		if (vs1 > vs2) sign = -1;
		double a = vsAccel*sign;
		//double tbegin = -1;
		//double tend = -1;
		double prevEndTime = traj.getFirstTime();
		Triple<Double,Double,Double> vsTriple = vsAccelGenerator(prevEndTime, np2.time(), kpc.getLastTime(), vs1, vs2, a);	
		// we have a long enough accel time and it is calculated properly
		if (vsTriple.third > MIN_ACCEL_TIME) {
			double tbegin = vsTriple.first;
			double tend = vsTriple.second;
			return new Triple<Double,Double,Double>(tbegin,tend,a);
		} else if (vsTriple.third < 0) {
			// we have a calculation error, possibly too short a time for the needed accleration
			traj.addError("TrajGen.generateVsTCPs ERROR: Insufficient room i = "+i+" for vertical accel! tbegin = "+f.Fm1(vsTriple.first)+" prevEndTime = "+f.Fm1(prevEndTime));
			printError("TrajGen.generateVsTCPs ERROR: Insufficient room at i = "+i+" for vertical accel! tbegin = "+f.Fm1(vsTriple.first)+" prevEndTime = "+f.Fm1(prevEndTime));
			return new Triple<Double,Double,Double>(-1.0,-1.0,-1.0);
		} else if (vsTriple.third > getMinTimeStep()*minorVfactor) {
			// we have a long enough accel time to recognize as a minor change, but not long enough for a full TCP
			// mark a known minor vs shift here.  Note we still exit the loop and skip over the acceleration zone generation code below!
			//traj.set(i, traj.point(i).makeLabel("minorVsChange")); //***** GETTING RID OF MakeLabel CAUSES C++ INCONSISTENCY
			return new Triple<Double,Double,Double>(-1.0,-1.0,-1.0);
		} else {
			// fallthrough -- no significant acceleration, so do nothing, counts as failure
			return new Triple<Double,Double,Double>(-1.0,-1.0,0.0);
		}		
	}
	

	/**
	 * next to last kinematic generation pass. adds vsc tcps.  assumes all horizontal passes have been completed.
	 * @param kpc
	 * @param vsAccel
	 * @return
	 */
	private static Plan generateVsTCPs(Plan kpc, double vsAccel) {
		Plan traj = new Plan(kpc);
		//f.pln(" generateVsTCPs: BEGIN traj = "+traj);
		for (int i = 1; i < traj.size()-1; i++) {
			NavPoint np1 = traj.point(i-1);
			NavPoint np2 = traj.point(i);
			double vs1 = np1.verticalSpeed(np2);
			// we want the velocities around n2, so v1 is in the opposite direction, then reversed
			Triple<Double,Double,Double> cboTrip = calcVsTimes(i, kpc, traj, vsAccel);
			double tbegin = cboTrip.first;
			double tend = cboTrip.second;						
			double accel = cboTrip.third;
			//f.pln(" $$............................ generateVsTCPs: tbegin = "+tbegin+" tend = "+tend);
			boolean newTCPneeded = false;
			if (tbegin >=0 && tend >= 0 && accel != 0.0 && !traj.inVsChange(tbegin) && !traj.inVsChange(tend)) newTCPneeded = true;						
			if (newTCPneeded) {	
				//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>BEFORE generateVsTCPs: np2 = "+np2.toStringFull());
				String label = np2.label();
				np2 = np2.makeStandardRetainSource(); // ***RWB*** KCHANGE
				//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>AFTER generateVsTCPs: np2 = "+np2.toStringFull());
                Velocity vin = traj.velocity(tbegin);				
				NavPoint b = np2.makeBVS(traj.position(tbegin), tbegin, accel, vin).makeLabel(label); // .makeAdded();//.appendName(setName);
				//f.pln(" $$$$$$$$$$$$ generateVsTCPs: i = "+i+" make BVS   point="+b.toStringFull()+" vin = "+vin);
				NavPoint e = np2.makeEVS(traj.position(tend), tend, traj.velocity(tend)); //.makeAdded();//.appendName(setName);
				//f.pln(" $$$$$$$$$$$$ generateVsTCPs: i = "+i+" make EVS  point="+e.toStringFull());
				// turns can use traj because ground speeds are already correct there -- they may not be in other sections!
				NavPoint np0 = traj.point(i); // normally we delete this, but if this is a (horizontal) TCP point, we do not want to delete it
				if (!np0.isTCP()) {
					traj.remove(i);
					//f.pln(" ^^^^^^^^^^ generateVsTCPs: REMOVE i = "+i+"  "+np0.toStringFull());
				}
				int bindex = traj.add(b);
				int eindex = traj.add(e);
				//f.pln(" $$$$$ generateVsTCPs: bindex = "+bindex+"  eindex = "+eindex);
				//f.pln(" $$$$$ generateVsTCPs: b = "+b.toStringFull()+"\n            e = "+e.toStringFull());
				if (bindex < 0) {
					bindex = -bindex;   // There was an overlap
				}
				if (eindex < 0) {
					eindex = -eindex;   // There was an overlap
				}
				i = eindex;
				// fix altitude for all remaining points between b and e
				// altitudes should be between b.alt and e.alt
				//f.pln("\n$$$$$$$$$$$$$ accel = "+accel+" tbegin = "+tbegin+" vs1 = "+Units.str("fpm",vs1));
				boolean altok = true;
				for (int k = bindex+1; k < eindex; k++) {
					double dt2 = traj.getTime(k) - tbegin;
					Pair<Double,Double> npair = vsAccel(b.alt(), vs1, dt2, accel);
					double newAlt = npair.first;	
 					//f.pln(" $$$$ k = "+k+"  dt2 = "+dt2+"  newAlt = "+Units.str("ft",newAlt));
					NavPoint newNp = traj.point(k).mkAlt(newAlt);
					//f.pln(" $$^^^^^^^^^^ generateVsTCPs: traj.point("+k+") = "+traj.point(k).toStringFull());
					if (newNp.isTCP()) {
						//f.pln(" ^^^^^^^^^^ generateVsTCPs: dt2 = "+dt2+" accel = "+accel);
						newNp = newNp.makeVelocityIn(newNp.velocityIn().mkVs(npair.second));
					}
					//f.pln(" ^^^^^^^^^^^^^ generateVsTCPs: traj.finalVelocity(k-1) = "+traj.finalVelocity(k-1));
					//f.pln(" $$^^^^^^^^^ generateVsTCPs: SET newNp = "+newNp.toStringFull());
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
				//f.pln(" $$ generateVsTCPs:   FAILED TO GENERATE Vertical TCPS at i = "+i);	
			}
		} // for loop
		//f.pln(" $$$ generateVsTCPs: END traj = "+traj);
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
		//npc = removeShortFirstLeg(npc, bank);
		//npc = removeShortLastLeg(npc,bank);
		for (int j = 0; j+3 < fp.size(); j++) {
			NavPoint p0 = fp.point(j);
			NavPoint p1 = fp.point(j+1);
			NavPoint p2 = fp.point(j+2);
			NavPoint p3 = fp.point(j+3);

			Velocity vf0 = fp.finalVelocity(j);
			Velocity vi1 = fp.initialVelocity(j+1);
			Velocity vf1 = fp.finalVelocity(j+1);
			Velocity vi2 = fp.initialVelocity(j+2);
			//f.pln("#### removeShortLegsBetween:  j = "+j+" vf0 = "+vf0+" vi1 = "+vi1);
			//f.pln("#### removeShortLegsBetween:  j = "+j+" vf1 = "+vf1+" vi2 = "+vi2);
			double deltaTrack1 = Math.abs(vf0.trk() - vi1.trk());
			double deltaTrack2 = Math.abs(vf1.trk() - vi2.trk());
			//f.pln(j+"#### removeShortLegsBetween: deltaTrack1 = "+Units.str("deg",deltaTrack1)+" deltaTrack2 = "+Units.str("deg",deltaTrack2));
			if (deltaTrack1> Units.from("deg",1.0) && deltaTrack2 > Units.from("deg",1.0)) {
				double gs1 = fp.initialVelocity(j).gs();
				double R1 = Kinematics.turnRadius(gs1,bank);
				double gs2 = fp.initialVelocity(j).gs();
				double R2 = Kinematics.turnRadius(gs2,bank);
				Triple<NavPoint,NavPoint,NavPoint> turn1 = turnGenerator(p0, p1, p2, R1);
				Triple<NavPoint,NavPoint,NavPoint> turn2 = turnGenerator(p1, p2, p3, R2);  // should this use R1 ~ gs1 ??
				NavPoint A = turn1.third;
				NavPoint B = turn2.first;	
				//f.pln(" $#########$$ A = "+A);
				//f.pln(" $#########$$ B = "+B);
				double distP1EOT = p1.distanceH(A);
				double distP1BOT = p1.distanceH(B);
				//f.pln(j+" ######### removeShortLegsBetween: distP1BOT = "+Units.str("nm",distP1BOT)+" distP1EOT = "+Units.str("nm",distP1EOT));
				if (A.time() > B.time() || distP1EOT > distP1BOT ) {
					if (A.time() > B.time() ) {
						//f.pln("............removeShortLegsBetween:  TIME TEST: REMOVE LEG ................."+f.Fm2(A.time())+"  > "+f.Fm2(B.time())); 
					} else {
						//f.pln("............removeShortLegsBetween:  DISTTEST: REMOVE LEG ................."); 	
					}
					//f.pln("removeShortLegs: npc.point(j+2)+ "+npc.point(j+2).toStringFull()+" npc.point(j+1) = "+npc.point(j+1).toStringFull());
					//f.pln("............linearRepairShortTurnLegs REMOVE npc.point(j+2) = "+npc.point(j+2)+ " npc.point(j+1) = "+npc.point(j+1));
					npc.remove(j+2);	
					npc.remove(j+1);	
					if (addMiddle) {
						Position mid = p1.position().midPoint(p2.position());
						double tmid =npc.getTime(j)+  mid.distanceH(p0.position())/gs1;
						//f.pln(" tmid = "+tmid);
						NavPoint midNP = p1.makePosition(mid).makeTime(tmid); // preserve source info
						//f.pln("............ ADD removeShortLegsBetween: midNP = "+midNP.toStringFull());
						npc.add(midNP);
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
	private static Plan removeInfeasibleTurns(Plan fp, double bankAngle) {  // ***RWB*** KCHANGE
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
				//f.pln("$$$ generateTurnTCPs: t="+np2.time()+"   gs="+Units.to("knot", gs)+"   R = "+Units.str8("nm",R));
				Triple<NavPoint,NavPoint,NavPoint> tg = turnGenerator(np1,np2,np3,R);	  
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
				if (!turnIsFeas(fp, i, BOT, EOT)) {
					//f.pln(" $$$$ removeInfeasibleTurns: remove point i = "+i);
					traj.remove(i);
				}
			}
		}//for
		return traj;
	}


	/**
	 * Repair function for lpc.  Removes vertex point if turn is infeasible.
	 * @param fp
	 * @param bankAngle
	 * @return
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
			//f.pln("$$ removeInfeasibleTurnsOver: for i = "+i+" turnTime = "+turnTime);
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
			double	targetGs = lpc.initialVelocity(j).gs();
			int sign = 1;
			double gs1 = vin.gs();
			//double gs2 = np1.initialVelocity(np2).gs(); // target Gs
			if (gs1 > targetGs) sign = -1;
			double a = Math.abs(gsAccel)*sign;
			double accelTime = (targetGs - gs1)/a;
			//f.p(j+" $$$$$$ smoothShortGsLegs: accelTime = "+accelTime+"  dt = "+f.Fm2(dt));
			//f.pln(" gs1 = "+Units.str("kn",gs1)+"  targetGs = "+Units.str("kn",targetGs));
			double d = lpc.pathDistance(j-1,j) - targetGs*getMinTimeStep(); 
			double remainingDist = d - accelTime * (gs1+targetGs)/2;
			//f.pln("$$$ smoothShortGsLegs: remainingDist = "+Units.str("nm",remainingDist));
			if (accelTime > getMinTimeStep() && remainingDist <= 0) {
	             f.pln(" REPAIRED GS ACCEL PROBLEM AT j = "+j+" time = "+lpc.point(j).time());
				lpc = PlanUtil.linearMakeGSConstant(lpc, j-1,j+1);
			}
		}
		//f.pln(" $$ smoothShortGsLegs: AFTER npc = "+lpc.toString());
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
			Triple<Double,Double,Double> cboTrip = calcVsTimes(i, fp, lpc, vsAccel);
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
	static protected Plan makeMarkedVsConstant(Plan kpc) {
		Plan traj = new Plan(kpc);
		int prevIndex = 0;
		//f.pln(" makeMarkedVsConstant: kpc = "+kpc);
		for (int i = 1; i < traj.size(); i++) {
			NavPoint currFixedAlt = traj.point(i);
			//f.pln(" makeMarkedVsConstant: for i = "+i+" AltPreserve = "+currFixedAlt.isAltPreserve());
			if (currFixedAlt.isAltPreserve() || i == traj.size()-1) {
				NavPoint prevFixedAlt = traj.point(prevIndex);
				//f.pln("makeMarkedVsConstant: Altitude at point i = "+i+" IS FIXED at "+Units.str4("ft",prevFixedAlt.alt()));
				double constantVs = (currFixedAlt.alt() - prevFixedAlt.alt())/(currFixedAlt.time() - prevFixedAlt.time());
				//f.pln("makeMarkedVsConstant: end of segment: "+i+" vs="+Units.to("fpm",constantVs));
				// fix all points between the two fixed altitude in points
				for (int j = prevIndex+1; j < i; j++) {
					NavPoint np = traj.point(j);
                    //f.pln("TrajGen.makeMarkedVsConstant inner before futzing j="+j+" np="+np.toStringFull());					
					double dt = np.time() - prevFixedAlt.time();
					double newAlt = prevFixedAlt.alt() + constantVs*dt;
					np = np.mkAlt(newAlt);
					//f.pln("$$$ makeMarkedVsConstant: np = "+np);
					if (np.isTCP()) {
						//f.pln("$$$ makeMarkedVsConstant: UPDATE vin: "+np.velocityIn().mkVs(constantVs));
						np = np.makeVelocityIn(np.velocityIn().mkVs(constantVs));
					}
					traj.set(j,np);
				}
				// fix the final, fixed altitude point
				//				currFixedAlt = currFixedAlt.makeOriginal(); // clear out the alt preserve flag
				prevIndex = i;
				if (currFixedAlt.isTCP()) {
					currFixedAlt = currFixedAlt.makeVelocityIn(currFixedAlt.velocityIn().mkVs(constantVs));
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
		double continueTm = Math.min(estTurnTime+extraProjTime,dt/2);
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
		int idx = lpc.add(npNew0);
		if (idx < 0)
			f.pln(" genDirectTo: ERROR -- could not add direct to point!");
		//f.pln(" #### genDirectTo0: lpc = "+lpc+" idx = "+idx);
		idx = lpc.add(npNew);
		//f.pln(" #### genDirectTo1: lpc = "+lpc+" idx = "+idx);
		//lpc = lpc.makeSpeedConstant(0,2);
		return lpc;
	}


	public static Plan genDirectToPoint(String id, Position so, Velocity vo, double to, Position goal, double bankAngle, double timeBeforeTurn) {
		Plan lpc = new Plan(id);
		NavPoint np0 = new NavPoint(so,to);
		lpc.add(np0);
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
		lpc.add(vertex);
		lpc.add(end);
		return lpc;
	}



	/**
	 * Constructs a new linear plan that connects current state to existing linear plan lpc (in 2-D).
	 * @param lpc    a linear plan that needs to be connected to at point 0
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
		lpc.add(new0);
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
			lpc.add(vertexNP); 
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
		Plan kpc = makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, false, true, true, GsMode.PRESERVE_GS);
		//f.pln("genDirectTo fp="+fp.getFirstTime()+" lpc="+lpc.getFirstTime()+" lpc2="+lpc2.getFirstTime()+" kpc="+kpc.getFirstTime());
		// you need to guard initialVelocity() calls!
		if (lpc2.size() < 2) {
			kpc.addError("TrajGen.genDirectTo: lpc2 size < 2");
			return kpc;
		} else if (kpc.size() < 4) {
			kpc.addError("TrajGen.genDirectTo: kpc size < 4");
			return kpc;
		}
		boolean twoTurnOverlap = Math.abs(lpc2.initialVelocity(1).compassAngle() -  kpc.initialVelocity(3).compassAngle()) > Units.from("deg",10.0);
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
					fp.add(new NavPoint(connectPt,tm0+timeIntervalNextTry));
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
		double stepSize = Math.max(minTimeStep, range/20.0);
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
		lpc.add(s);
		if (bestdtp != null) {
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
	 * Attempt to connect to the given point. Returns a kinematic plan that moves from the current state to that position.
	 * @param s
	 * @param v
	 * @param goal
	 * @return
	 */
	public static Plan genDirectTo(String id, Position s, Velocity v, double t, Position goal, double bankAngle, double gsAccel, double vsAccel, double delay) {
		Plan lpc = new Plan(id);
		NavPoint np1 = new NavPoint(s,t);
		lpc.add(np1);
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
		lpc.add(turnPt);
		lpc.add(endPt);
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
			if (fp.point(i).isBOT()) insideTurn = true;
			if (fp.point(i).isEOT()) insideTurn = false;
			if (fp.point(i).isBGS()) insideGsAccel = true;
			if (fp.point(i).isEGS()) insideGsAccel = false;
			if (fp.point(i).isBVS()) insideVsAccel = true;
			if (fp.point(i).isEVS()) insideVsAccel = false;
		    if (insideTurn && (fp.point(i).isBVS() || fp.point(i).isEVS())) inPts.add(i);
		    if (insideGsAccel && (fp.point(i).isBVS() || fp.point(i).isEVS())) inPts.add(i);
            if (insideVsAccel &&  (fp.point(i).isBOT() || fp.point(i).isEOT())) inPts.add(i);
            if (insideVsAccel &&  (fp.point(i).isBGS() ||  fp.point(i).isEGS())) inPts.add(i);
		}
		return inPts;
	}

	
//	/**
//	 * Return true if TCP source points appear vaguely reasonable.  If this returns false, removeTCPs is likely to return garbage.
//	 * Add warnings to the plan if problems are discovered
//	 */
//	public static boolean testTCPSources(Plan fp) {
//		boolean ret = true;
//		for (int i = 0; i < fp.size(); i++) {
//			NavPoint npi = fp.point(i);
//			if (npi.isTurnBegin()) {
//				int j = fp.nextEOT(i);
//				Position src = npi.tcpSourcePosition();
//				NavPoint srcnp = new NavPoint(src, (npi.time()+fp.getTime(j))/2.0);
//				if (!Util.almost_equals(npi.initialVelocity(srcnp).trk(), fp.initialVelocity(i).trk())) {
//					ret = false;
//					fp.addWarning("TrajGen.testTCPSource src position does not match initial velocity track "+i);
//				}
//				if (!Util.almost_equals(srcnp.initialVelocity(fp.point(j)).trk(), fp.finalVelocity(j-1).trk())) {
//					ret = false;
//					fp.addWarning("TrajGen.testTCPSource src position does not match final velocity track "+j);
//				}
//				double tm = npi.tcpSourceTime();
//				for (int k = i+1; k <= j; k++) {
//					NavPoint npk = fp.point(k);
//					if (npk.isTurn() && !npk.tcpSourcePosition().almostEquals(src)) {
//						ret = false;
//						fp.addWarning("TrajGen.testTCPSource position mismatch "+i+" vs "+k);
//					};
//					if (npk.isTurn() && !Util.almost_equals(npk.tcpSourceTime(),tm)) {
//						ret = false;
//						fp.addWarning("TrajGen.testTCPSource time mismatch "+i+" vs "+k);
//					};
//				}
//			}
//			if (npi.isGSCBegin()) {
//				int j = fp.nextGSCEnd(i);
//				Position src = npi.tcpSourcePosition();
//				if (!src.almostEquals(npi.position())) {
//					ret = false;
//					fp.addWarning("TrajGen.testTCPSource src position does not does not match GSC begin point "+i);
//				}
//				double tm = npi.tcpSourceTime();
//				for (int k = i+1; k <= j; k++) {
//					NavPoint npk = fp.point(k);
//					if (npk.isGSC() && !npk.tcpSourcePosition().almostEquals(src)) {
//						ret = false;
//						fp.addWarning("TrajGen.testTCPSource position mismatch "+i+" vs "+k);
//					};
//					if (npk.isGSC() && !Util.almost_equals(npk.tcpSourceTime(),tm)) {
//						ret = false;
//						fp.addWarning("TrajGen.testTCPSource time mismatch "+i+" vs "+k);
//					};
//				}
//			}
//
//			if (npi.isVSCBegin()) {
//				int j = fp.nextGSCEnd(i);
//				Position src = npi.tcpSourcePosition();
//				NavPoint srcnp = new NavPoint(src, (npi.time()+fp.getTime(j))/2.0);
//				if (!Util.almost_equals(npi.initialVelocity(srcnp).vs(), fp.initialVelocity(i).vs())) {
//					ret = false;
//					fp.addWarning("TrajGen.testTCPSource src position does not match initial velocity vs "+i);
//				}
//				if (!Util.almost_equals(srcnp.initialVelocity(fp.point(j)).vs(), fp.finalVelocity(j-1).vs())) {
//					ret = false;
//					fp.addWarning("TrajGen.testTCPSource src position does not match final velocity vs "+j);
//				}
//				double tm = npi.tcpSourceTime();
//				for (int k = i+1; k <= j; k++) {
//					NavPoint npk = fp.point(k);
//					if (npk.isVSC() && !npk.tcpSourcePosition().almostEquals(src)) {
//						ret = false;
//						fp.addWarning("TrajGen.testTCPSource position mismatch "+i+" vs "+k);
//					};
//					if (npk.isVSC() && !Util.almost_equals(npk.tcpSourceTime(),tm)) {
//						ret = false;
//						fp.addWarning("TrajGen.testTCPSource time mismatch "+i+" vs "+k);
//					};
//				}
//			}
//		
//		}
//		return ret;
//	}











	//	/**
	//	 * Returns a new Plan that sets all points in a range to have a constant GS.
	//	 * The return plan type will be the same as the input plan type.
	//	 * This re-generates a kinematic plan, if necessary (if this fails, error status will be set)
	//	 * The new gs is specified by the user.
	//	 */
	//	public static Plan makeGSConstant(Plan p, double newGs,  double bankAngle, double gsAccel, double vsAccel, 
	//			boolean repair) {
	//		Plan kpc = removeTCPs(p);
	//        kpc = linearMakeGSConstant(kpc,newGs);
	//        if (!p.isLinear()) {
	//        	kpc = makeKinematicPlan(kpc,  bankAngle, gsAccel, vsAccel, repair);
	//        }
	//		return kpc;
	//	}
	//	// Make vertical speeds constant by adjusting altitudes between wp1 and wp2
	//	// Assume there are no vertical speed accelerations
	//	// do not alter wp1 and wp2
	//	static public Plan vsConstantBetweenMarked0(Plan kpc) {
	//		//f.pln(" $$$$ makeVsConstant: ENTER");
	//		Plan traj = new Plan(kpc);
	////		if (start < 0) return kpc;
	////		if (end >= kpc.size()) end = kpc.size()-1;
	//		int beginIndex = 0;
	//		boolean inConstant = true; // start in a constant area
	//	
	//		for (int i = 0; i < traj.size(); i++) {
	//			NavPoint npi = kpc.point(i);
	////			if (inConstant && (npi.isVSCBegin() || i == traj.size()-1)) {
	//			if (inConstant && npi.isVSCBegin()) {
	//				double deltaAlt = traj.point(i).alt() - traj.point(beginIndex).alt();
	//				double deltaTime = traj.point(i).time() - traj.point(beginIndex).time() ;
	//				double constantVs = deltaAlt/deltaTime;
	//				inConstant = false;
	//				for (int j = beginIndex+1; j < i; j++) {
	//					if (!Util.within_epsilon(constantVs,kpc.initialVelocity(j).vs(),Units.from("fpm",1.0))) {
	//						double dt = traj.getTime(j) - traj.getTime(j-1);
	//						double newAlt = traj.point(j-1).alt()+constantVs*dt;
	//						NavPoint np = traj.point(j).mkAlt(newAlt);
	//                        //f.pln(j+" $$$$ makeVsConstant CHANGE ALT FROM "+Units.str4("ft",traj.point(j).alt()) +" to "+ Units.str4("ft",np.alt())+ " np = "+np);						
	//						traj.set(j,np);
	//					}
	//				}
	////			} else if (!inConstant && (npi.isVSCEnd()  || i == traj.size()-1)) {
	//			} else if (!inConstant && npi.isVSCEnd()) {
	//				beginIndex = i;
	//				inConstant = true;
	//				//recalculate vs acceleration and store it in the TCPs
	//				int vscbindex = kpc.prevVSCBegin(npi.time());
	//				NavPoint VSCB = kpc.point(vscbindex);
	//				double dt = npi.time()-VSCB.time();
	//				double dvs = kpc.initialVelocity(i).vs() - kpc.finalVelocity(vscbindex-1).vs();
	//				double vsacc = dvs/dt;
	////f.pln(" >>>>>>> %%%%%% new accel = "+vsacc+" old was = "+npi.accel());				
	//			}
	////			if (i >= start && i < end) {
	////				if (npi.isVSCBegin()) f.pln(" ###### ERROR !!!!!!!!!!");
	////				double dt = npi.time() - startTime;
	////				double newAlt = startAlt + constantVs*dt;
	////				NavPoint np = kpc.point(i).makeAlt(newAlt);
	////				f.pln("### newAlt = "+Units.str("ft",newAlt)+" npi = "+npi+"  np = "+np);
	////				traj.add(np);
	////			} else {
	////				traj.add(npi);
	////			}
	//		}
	//		
	//		return traj;
	//	}


	//	// Make vertical speeds constant by adjusting altitudes between wp1 and wp2
	//	// Assume there are no vertical speed accelerations
	//	// do not alter wp1 and wp2
	//	static public Plan vsConstantBetweenMarked(Plan kpc) {
	//		//f.pln(" $$$$ makeVsConstant: ENTER");
	//		Plan traj = new Plan(kpc);
	//
	//
	//		// fix all altitudes OUTSIDE of acceleration zones
	//		int beginIndex = 0;
	//		boolean inConstant = true; // start in a constant area
	//		for (int i = 0; i < traj.size(); i++) {
	//			NavPoint npi = traj.point(i);
	//			if (inConstant && (npi.isVSCBegin() || i == traj.size()-1)) {
	//				double deltaAlt = traj.point(i).alt() - traj.point(beginIndex).alt();
	//				double deltaTime = traj.point(i).time() - traj.point(beginIndex).time() ;
	//				double constantVs = deltaAlt/deltaTime;
	//				//f.pln("$$ makeVsConstant: i = "+i+" constantVs = "+Units.str4("fpm",constantVs));
	//				inConstant = false;
	//				double alt0 = traj.point(beginIndex).alt();
	//				double t0 = traj.getTime(beginIndex);
	//				for (int j = beginIndex+1; j < i; j++) {
	//					double dt = traj.getTime(j) - t0;
	//					double newAlt = alt0 +constantVs*dt;
	//					NavPoint np = traj.point(j).mkAlt(newAlt);
	//					//f.pln(j+" $$$$ makeVsConstant CHANGE ALT FROM "+traj.point(j).alt() +" to "+ np.alt()+ " np = "+np);						
	//					traj.set(j,np);
	//				}
	//			} else if (!inConstant && npi.isVSCEnd()) {
	//				beginIndex = i;
	//				inConstant = true;
	//			}
	//		}	
	//		// fix velocityIn.vs() outside of vertical speed zone
	//		for (int i = 1; i < traj.size(); i++) {
	//			NavPoint npi = traj.point(i);
	//            //double vFvs = traj.finalVelocity(i-1).vs();
	//            if (npi.isTCP() && !traj.inVerticalSpeedChange(traj.getTime(i))) {
	//            	double vFvs = traj.point(i-1).verticalSpeed(npi);
	//            	npi = npi.makeVelocityIn(npi.velocityIn().mkVs(vFvs)); 
	//            	//f.pln("## makeVsConstant:  set velocityIn["+i+"].vs() to "+Units.str4("fpm",vFvs));
	//            	traj.set(i, npi);
	//            }
	//		}
	//
	////		
	////		// fix all VSC accel metadata
	////		for (int i = 1; i < traj.size(); i++) {
	////			NavPoint npi = traj.point(i);
	////			if (npi.isVSCBegin()) {
	////				double accel = traj.calcVertAccel(i);
	////				NavPoint newNp = npi.makeAccel(accel);
	////				f.pln("## makeVsConstant: set meta accel at "+i+" to "+accel);
	////				traj.set(i, newNp);
	////			}
	////		}
	//		
	////      // fix velocityIn.vs() in vertical speed zone 
	////		for (int i = 1; i < traj.size(); i++) {
	////			NavPoint npi = traj.point(i);
	////			if (traj.inVerticalSpeedChange(npi.time())) { //note: vsc end point is not considered inside accel zone
	////				NavPoint tbegin = traj.point(traj.prevVSCBegin(i));
	////				double dt = npi.time()-tbegin.time();
	////				Pair<Position,Velocity> pv = ProjectedKinematics.vsAccel(tbegin.position(), tbegin.velocityIn(), dt, tbegin.accel());
	////				double newalt = pv.first.alt();
	////				double newvs = pv.second.vs();
	////				NavPoint np = npi.makeAlt(newalt);
	////				if (npi.isTurnBegin()) {
	////					np = np.makeVelocityIn(np.velocityIn().mkVs(newvs));
	////				}
	////				traj.set(i,np);
	////	           	f.pln(">> makeVsConstant:  set velocityIn["+i+"].vs() to "+Units.str4("fpm",newvs));
	////
	////			}
	////		}
	////
	//
	//		
	//		return traj;
	//	}

	//	

	//	/**
	//	 * Returns a new Plan that sets all points in a range to have a constant GS.
	//	 * The return plan type will be the same as the input plan type.
	//	 * This re-generates a kinematic plan, if necessary (if this fails, error status will be set)
	//	 * The new gs is the average of the linear version of the plan.
	//	 */
	//	public static Plan makeGSConstant(Plan p, double bankAngle, double gsAccel, double vsAccel, 
	//			boolean repair) {
	//		Plan kpc = removeTCPs(p);
	//        kpc = linearMakeGSConstant(kpc);
	//        if (!p.isLinear()) {
	//        	kpc = makeKinematicPlan(kpc,  bankAngle, gsAccel, vsAccel, repair);
	//        }
	//		return kpc;
	//	}
	//
	//	
	//	/**
	//	 * Returns a new Plan that sets all points in a range to have a constant GS.
	//	 * THIS ASSUMES NO VERTICAL TCPS, but allows turns (turn omega metatdata may be altered -- maximum omega values are not tested for).
	//	 * The return plan type will be the same as the input plan type.
	//	 * The new gs is specified by the user.
	//	 * This will set an error status in the return plan if there are any vertical TCPs.
	//	 */
	//	public static AugmentedPlan makeGSConstant_NO_Verts(Plan p, double newGs) {
	//		//f.pln(" makeGSConstant: newGs = "+Units.str4("kn",newGs)+"  type = "+p.getType());
	//		Plan kpc = new Plan(p.name);
	//		kpc.setPlanType(p.getType());
	//		//double time0 = point(0).time();
	//		double lastTime = p.point(0).time();
	//		kpc.add(p.point(0));
	//		int j = 0;   // last point included in new file (note GS TCPs are removed)
	//		for (int i = 1; i < p.size(); i++) {
	//			NavPoint np = p.point(i);
	//			//f.pln(" $$$$ makeGSConstant: i = "+i+" np = "+np.toStringFull());
	//			// skip any GSC points (i.e. remove them)
	//			if (!np.isGSC()) {
	//				double d = p.pathDistance(j,i);
	//				double dt = d/newGs;
	//				double nt = lastTime + dt;
	//				//f.pln(" $$$$ makeGSConstant: d = "+Units.str("nm",d)+" dt = "+dt+" nt = "+nt);
	//				lastTime = nt;
	//				if (np.isTCP()) {        // assume only turn TCPs allowed
	//					if (!np.isTurn()) {
	//						//f.pln("makeGSConstant(TCP): start point within acceleration zone");
	//						p.addError("TrajGen.genGSConstant: start point within acceleration zone");
	//						return new AugmentedPlan(kpc, -1, i, ErrType.GSACCEL_OVERLAP);
	//					}
	//				    //f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" BEFORE np.turnRadius = "+np.turnRadius());
	//                    double oldGs = np.velocityIn().gs();
	//                    double newVsIn = p.vertDistance(j,i)/dt;
	//					np = np.makeVelocityIn(np.velocityIn().mkGs(newGs).mkVs(newVsIn));  // modify gs,vs in for turns
	//					//f.pln(" makeGSConstant(TCP): make point i = "+i+" have velocityIn = "+Units.str4("kn",newGs));
	//					// calculate new acceleration: newAccel = oldAccel * oldTime/newTime
	//					double k = newGs/oldGs;
	//					double newAccel = np.accel() * k;
	//					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE accel from "+np.accel()+" to "+newAccel);
	//					np = np.makeAccel(newAccel); // modify turn omega to match new gs in
	//					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" AFTER np.turnRadius = "+np.turnRadius());
	//					// NOTE: we need to recalculate times of MOT and BOT, we are preserving turn radius
	//				}
	//				NavPoint newNp = np.makeTime(nt);
	//				//f.pln(" $$$ makeGSConstant for i = "+i+" CHANGE time from "+p.point(i).time()+" to "+newNp.time());
	//				//f.pln(" $$$ makeGSConstant add "+newNp);
	//				kpc.add(newNp);
	//				j = i;
	//			}
	//		}
	//		return new AugmentedPlan(kpc);
	//	}
	//
	//
	//	
	//	/**
	//	 * Returns a new Plan that sets all points in a range to have a constant GS.
	//	 * THIS ASSUMES NO VERTICAL TCPS, but allows turns (turn omega metatdata may be altered -- maximum omega values are not tested for).
	//	 * The return plan type will be the same as the input plan type.
	//	 * The new gs is an average over the whole plan.  End time should be preserved.
	//	 * This will set an error status in the return plan if there are any vertical TCPs.
	//	 */
	//	public static AugmentedPlan makeGSConstant_No_Verts(Plan p) {
	//		double dt = p.getLastTime() - p.getFirstTime();
	//		double newGS = p.pathDistance()/dt;
	//		return makeGSConstant_NO_Verts(p, newGS);
	//	}

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
		pc.add(end);
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
		pc.add(end);
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
			t = t + lpc2r.getTime(i+1)-lpc2r.getTime(i);
			NavPoint np = lpc2r.point(i).makeTime(t);
			lpc.add(np);
		}
		Plan kpc = makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false);
		return kpc;
	}


	// This generates a mini kinimatic plan that consists of a ground speed change (while moving straight ahead)
	public static Plan maneuverToGs(String name, Position so, Velocity vo, double to, double targetGS, double gsAccel, double delayBefore, double delayAfter) {
		Plan kpc = new Plan(name);
		//kpc.setPlanType(Plan.PlanType.KINEMATIC);
		NavPoint np1 = new NavPoint (so,to);
		if (delayBefore > 0) {
			kpc.add(np1); // if there is a delay, add the initial point
		}
		np1 = np1.linear(vo, delayBefore);
		NavPoint np2 = np1.linear(vo, 3600); // somewhere far away		
		Triple<NavPoint,NavPoint,Double> tcpTriple =  gsAccelGenerator(np1, np2, targetGS, vo, gsAccel, GsMode.PRESERVE_GS);
		if (tcpTriple.third >= getMinTimeStep()) {
			np1 = tcpTriple.first;
			np2 = tcpTriple.second;
			kpc.add(np1);				
			kpc.add(np2);
		} else if (tcpTriple.third < 0) {
			kpc.add(np1);
			kpc.addError("TrajGen.maneuverToGs: insufficient time");
		} else if (tcpTriple.third > getMinTimeStep()*minorVfactor) {
			//np1 = np1.makeLabel("minorGsChange"); // makeMinorGsChange();
			kpc.add(np1);
		}
		if (delayAfter > 0) {
			NavPoint np3 = kpc.point(kpc.size()-1).linear(vo.mkGs(targetGS), delayAfter);
			kpc.add(np3);
		}
		return kpc;
	}

	// This generates a mini kinematic plan that involves reaching a given point (along my current path) as an RTA.
	// This could follow a maneuverToPoint call, for example
	public static Plan maneuverToTime(String name, Position so, Velocity vo, double to, Position goal, double rta, double gsAccel, double delay) {
		Plan kpc = new Plan(name);
		//kpc.setPlanType(Plan.PlanType.KINEMATIC);
		if (rta < to) {
			kpc.addError("TrajGen.maneuverToTime: RTA is before current time! "+rta+" < "+to);
			return kpc;
		}
		NavPoint np1 = new NavPoint (so,to);
		if (delay > 0) {
			kpc.add(np1); // if there is a delay, add the initial point
		}
		np1 = np1.linear(vo, delay);
		NavPoint np3 = new NavPoint(goal, rta);
		kpc.add(np3);
		Velocity v2 = np1.initialVelocity(np3).mkGs(vo.gs());
		Pair<Double,Double> gst = Kinematics.gsAccelToRTA(vo.gs(), np1.distanceH(np3), rta-(to+delay), gsAccel);
		if (gst.second < 0) { // not enough space to accelerate
			kpc.add(np1);
			kpc.addError("TrajGen.maneuverToTime: insufficient distance to acceleration to new gs");
		} else if (gst.second < getMinTimeStep()) { 
			//np1 = np1.makeLabel("makeMinorGsChange"); // makeMinorGsChange();
			kpc.add(np1);
		} else {
			double a = gsAccel;
			if (gst.first < vo.gs()) a = -gsAccel;
			np1 = np1.makeBGS(np1.position(), np1.time(), a, v2);
			NavPoint np2 = np1.makeEGS(ProjectedKinematics.gsAccel(so, v2, gst.second, a).first, gst.second+np1.time(), v2);
			kpc.add(np1);
			kpc.add(np2);
		}
		return kpc;
	}

	// This generates a mini kinematic plan that involves reaching a given point (along my current path) as an RTA, with the given ground speed.
	// This could follow a maneuverToPoint call, for example
	public static Plan maneuverToTimeVelocity(String name, Position so, Velocity vo, double to, Position goal, double gsGoal, double rta, double gsAccel, double delay) {
		Plan kpc = new Plan(name);
		//kpc.setPlanType(Plan.PlanType.KINEMATIC);
		if (rta < to) {
			kpc.addError("maneuverToTimeVelocity: RTA is before current time! "+rta+" < "+to);
			return kpc;
		}
		NavPoint np1 = new NavPoint(so,to);
		NavPoint np2 = new NavPoint(goal,rta);
		kpc.add(np1);
		kpc.add(np2);
		double d = so.distanceH(goal);
		double t = rta-to;
		if (Util.almost_equals(vo.gs(), gsGoal) && Util.almost_equals(gsGoal, d/t)) {
			// nothing needs to be done
			return kpc;
		}
		delay = Math.max(delay,MIN_ACCEL_TIME);
		Position p1 = so.linear(vo, delay);
		Position p2 = goal.linear(vo.mkGs(gsGoal), -delay);
		t = rta - to - delay*2;
		d = p1.distanceH(p2);
		if (t < 0) {
			kpc.addError("TrajGen.maneuverToTimeVelocity: insufficient time before goal point");
			return kpc;
		}
		Pair<Triple<Double,Double,Double>,Triple<Double,Double,Double>> r = Kinematics.gsAccelToRTAV(vo.gs(), d, t, gsGoal, gsAccel);
		double t1 = r.first.first;
		double t2 = r.first.second;
		double t3 = r.first.third;
		double a1 = r.second.first;
		double gs2 = r.second.second;
		double a2 = r.second.third;
		if (t1 > 0.0 && t2 > 0 && t3 > 0) {
			NavPoint np3 = np1.makeBGS(p1, to+delay, a1, vo);
			NavPoint np4 = np1.makeEGS(ProjectedKinematics.gsAccel(so, vo, t1, a1).first, to+delay+t1,  vo);
			kpc.add(np3);
			kpc.add(np4);
			Velocity v2 = vo.mkGs(gs2);
			Position p5 = np4.position().linear(v2,t2);
			NavPoint np5 = np1.makeBGS(p5, to+delay+t1+t2, a2, v2);
			NavPoint np6 = np1.makeEGS(ProjectedKinematics.gsAccel(p5, v2, t3, a2).first, to+delay+t1+t2+t3, v2);
			kpc.add(np5);
			kpc.add(np6);
		} else if (Util.almost_equals(t1, 0)) {
			Velocity v2 = vo.mkGs(gs2);
			NavPoint np5 = np1.makeBGS(p1, to+delay+t1+t2, a2, v2);
			NavPoint np6 = np1.makeEGS(ProjectedKinematics.gsAccel(p1, v2, t3, a2).first, to+delay+t1+t2+t3,  v2);
			kpc.add(np5);
			kpc.add(np6);
		} else if (Util.almost_equals(t3, 0)) {
			NavPoint np3 = np1.makeBGS(p1, to+delay, a1, vo);
			NavPoint np4 = np1.makeEGS(ProjectedKinematics.gsAccel(so, vo, t1, a1).first, to+delay+t1,  vo);
			kpc.add(np3);
			kpc.add(np4);
		} else if (Util.almost_equals(t2, 0)) {
			kpc.addError("TrajGen.maneuverToTimeVelocity: case 2");			
		} else {
			// fail
			kpc.addError("TrajGen.maneuverToTimeVelocity: cannot match RTA point");
		}
		return kpc;
	}

//	// attempt to gracefully reconned to the provided plan.
	public static Plan reconnectToPlan(Plan p, Position so, Velocity vo, double to, 
			double bankAngle, double gsAccel, double vsAccel, double minVsChangeRecognized, double timeBeforeTurn, 
			double timeIntervalNextTry, GsMode gsm) {
//		
////f.pln("RECONNECTTOPLAN p="+p+" so="+so.toStringNP(6)+" vo="+vo+" to="+to+" bankAngle="+bankAngle+" gsAccel="+gsAccel+"vsAccel="+vsAccel+" minVsChange="+minVsChangeRecognized+" timeBefore="+timeBeforeTurn+" timeInterval="+timeIntervalNextTry+" gsm="+gsm);		
//		
////		f.pln("TrajGen.reconnectToPlan so=("+so+"( vo=("+vo+") to="+to+" p="+p);
//		boolean done = false;
//
////ArrayList<Plan> plist = new ArrayList<Plan>();
////plist.add(p.copy());
//				
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
//		double t = Math.max(to,closest.time()) + timeBeforeTurn*2;
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


//	//	static public enum ErrType {UNKNOWN, TURN_INFEAS, TURN_OVERLAPS_B, TURN_OVERLAPS_E, GSACCEL_DIST, GS_ZERO, VSACCEL_DIST, REMOVE_FIXED, GSACCEL_OVERLAP};
//
//	// attempt to fix the error in plan p by returning a new linear plan (and true if fix worked, false if known to fail).  Include a copy of the original lpc. bool is false if fix failed
//	public static Pair<Plan,Boolean> postFix(AugmentedPlan p, Plan lpc, int iteration) {
//		int i = p.lpcErrorPt;
//		int j = p.kpcErrorPt;
//		switch (p.etype) {
//		case TURN_INFEAS: {
//			// reduce the turn radius by moving the point inward
//			Position c = p.point(j).turnCenter();
//			Position tpos = lpc.point(i).position();
//			Velocity v = tpos.initialVelocity(c, 100);
//			Position npos = tpos.linear(v, 10*iteration); // move 10% closer to center of turn
//			if (tpos.almostEquals(c) || iteration > 9) {
//				return new Pair<Plan,Boolean>(lpc,false); // can't fix
//			} else {
//				Plan npc = lpc.copy();
//				npc.set(i, lpc.point(i).makePosition(npos));
//				return new Pair<Plan,Boolean>(npc, true);
//			}
//		}
//		case TURN_OVERLAPS_B: {
//			if (p.lpcErrorPt <= 1) {
//				return new Pair<Plan,Boolean>(lpc,false); // can't fix					
//			} else { // average the two points
//				Plan npc = lpc.copy();
//				NavPoint np1 = lpc.point(i-1);
//				NavPoint np2 = lpc.point(i);
//				Velocity v = np1.initialVelocity(np2);
//				NavPoint mid = np1.linear(v, (np2.time()-np1.time())/2);
//				npc.remove(i);
//				npc.remove(i);
//				npc.add(mid);
//				return new Pair<Plan,Boolean>(npc, true);
//			}
//		}
//		case GSACCEL_OVERLAP: // these both work from kpc
//		case GSACCEL_DIST:
//			i = lpc.getSegment(lpc.closestPoint(p.point(j).position()).time()); // use this value in the following:
//		case TURN_OVERLAPS_E: {
//			if (i >= lpc.size()-2) {
//				return new Pair<Plan,Boolean>(lpc,false); // can't fix					
//			} else { // average the two points
//				Plan npc = lpc.copy();
//				NavPoint np1 = lpc.point(i);
//				NavPoint np2 = lpc.point(i+1);
//				Velocity v = np1.initialVelocity(np2);
//				NavPoint mid = np1.linear(v, (np2.time()-np1.time())/2);
//				npc.remove(i);
//				npc.remove(i);
//				npc.add(mid);
//				return new Pair<Plan,Boolean>(npc, true);
//			}
//		}
//		case VSACCEL_DIST: {
//			if (i <= 1 || i >= lpc.size()-1 || iteration > 9) {
//				return new Pair<Plan,Boolean>(lpc,false); // can't fix					
//			} else { // average the two points
//				Plan npc = lpc.copy();
//				NavPoint np1 = lpc.point(i-1);
//				NavPoint np2 = lpc.point(i);
//				double nalt = np2.alt() - iteration*(np2.alt()-np1.alt())/10.0;
//				npc.remove(i);
//				npc.add(np2.mkAlt(nalt));
//				return new Pair<Plan,Boolean>(npc, true);
//			}
//		}
//		case GS_ZERO:
//		case REMOVE_FIXED:
//			return new Pair<Plan,Boolean>(lpc,false); // can't fix					
//		default: return new Pair<Plan,Boolean>(lpc,true);
//		}
//	}
//
//	public static Plan makeKinematicPlanDynamicRepair(Plan fp, double bankAngle, double gsAccel, double vsAccel, double minVsChangeRecognized, GsMode gsm) {
//		Pair<Plan,Boolean> pb = new Pair<Plan,Boolean>(fp,true);
//		Plan base = fp;
//		AugmentedPlan apPrev = new AugmentedPlan(fp);
//		int depth = 1;
//		do {
//			AugmentedPlan ap = makeKinematicPlan(pb.first, bankAngle, gsAccel, vsAccel, minVsChangeRecognized, false, false, false, gsm);
//			pb = postFix(ap, base, depth);
//			if (ap.etype == apPrev.etype && ap.lpcErrorPt == apPrev.lpcErrorPt && ap.kpcErrorPt == apPrev.kpcErrorPt) {
//				depth++;
//			} else {
//				depth = 1;
//				base = pb.first;
//			}
//			apPrev = ap;
//		} while (!pb.second && depth < 9);
//
//		return pb.first;
//	}

	public static Plan removeRedundantPoints(Plan pc) {
		Plan p = pc.copy();
		int i = 0;
		while (i < p.size()-2) {
			NavPoint np1 = p.point(i);
			NavPoint np2 = p.point(i+1);
			NavPoint np3 = p.point(i+2);
			if (!np2.isTCP() && np1.position().collinear(np2.position(), np3.position()) && pc.finalVelocity(i).almostEquals(pc.initialVelocity(i+1))) {
				p.remove(i+1);
			} else {
				i++;
			}
		}
		return p;
	}


	/** This reconstructs complete navpoints for a turn, including the source point (timing may be early) */
	static Triple<NavPoint,NavPoint,NavPoint> turnGenerator3(NavPoint so, Velocity vo, double omega, double dt) {
		Position p1 = so.position();
		double t = so.time();
		Pair<Position,Velocity> prMid = ProjectedKinematics.turnOmega(p1, vo, dt/2, omega);
		Pair<Position,Velocity> prEnd = ProjectedKinematics.turnOmega(p1, vo, dt, omega);
		Pair<Position,Double> prSrc = ProjectedKinematics.intersection(p1, vo, prEnd.first, prEnd.second);
		if (prSrc.second < 0) {
			f.pln("Error in TrajGen.turnGenerator3: negative intersection time");
		}
		NavPoint src = new NavPoint(prSrc.first, t + prSrc.second);
		NavPoint np1 = src.makeBOT(p1, t, omega, vo);
		//NavPoint np2 = src.makeTurnMid(prMid.first, t+dt/2.0, omega, prMid.second);
		NavPoint np2 = src.makePosition(prMid.first).makeTime(t+dt/2.0).makeVelocityIn(prMid.second);		

		NavPoint np3 = src.makeEOT(prEnd.first, t+dt, prEnd.second);
		return new Triple<NavPoint,NavPoint,NavPoint>(np1,np2,np3);
	}


	/**
	 * Produce a standard holding pattern loop.  given a start position so and velocity vo, maintain the same altitude: immediately turn to 90 degrees from vo
	 * at the given rate and direction, travel legDistA (generally close to zero), and then turn and travel legDistB (usually 1-2nmi), and repeat until returning to so.
	 * 
	 * @param so starting position, at beginning of first turn
	 * @param vo starting velocity, just before first turn
	 * @param t starting time
	 * @param turnOmega turn rate and direction: standard right turn is Units.from("deg/s",3.0) (or 0.05236), standard left turn in -Units.from("deg/sec",3.0)
	 * @param legDistA the distance between turn end and begin on the "short" (perpendicular to track) side
	 * @param legDistB the distance between turn end and begin on the "long" (parallel to track) side
	 * @return Kinematic plan completing one loop, beginning and ending at so.  If reverted to a linear Plan, this should consist of 4 turn points plus the end point.
	 */
	public static Plan standardHoldingPattern(Position so, Velocity vo, double t, double turnOmega, double legDistA, double legDistB) {
		Plan plan = new Plan();
		if (vo.gs() == 0.0) {
			plan.addError("TrajGen: standardHoldingPattern error: zero ground speed");
			return plan;
		}
		Velocity vo0 = vo.mkVs(0); // level flight
		double dt = Math.abs((Math.PI/2.0)/turnOmega); 
		double tA = Math.max(MIN_ACCEL_TIME, legDistA/vo.gs());
		double tB = Math.max(MIN_ACCEL_TIME, legDistB/vo.gs());

		NavPoint np0 = new NavPoint(so,t);
		Triple<NavPoint,NavPoint,NavPoint> turn1 = turnGenerator3(np0,vo0,turnOmega,dt);
		Velocity vo1 = turn1.third.velocityIn();
		NavPoint np1 = turn1.third.linear(vo1, tA);
		Triple<NavPoint,NavPoint,NavPoint> turn2 = turnGenerator3(np1,vo1,turnOmega,dt);
		Velocity vo2 = turn2.third.velocityIn();
		NavPoint np2 = turn2.third.linear(vo2, tB);
		Triple<NavPoint,NavPoint,NavPoint> turn3 = turnGenerator3(np2,vo2,turnOmega,dt);
		Velocity vo3 = turn3.third.velocityIn();
		NavPoint np3 = turn3.third.linear(vo3, tA);
		Triple<NavPoint,NavPoint,NavPoint> turn4 = turnGenerator3(np3,vo3,turnOmega,dt);
		Velocity vo4 = turn4.third.velocityIn();
		NavPoint np4 = turn4.third.linear(vo4, tB);

		plan.add(turn1.first);
		plan.add(turn1.second);
		plan.add(turn1.third);
		plan.add(turn2.first);
		plan.add(turn2.second);
		plan.add(turn2.third);
		plan.add(turn3.first);
		plan.add(turn3.second);
		plan.add(turn3.third);
		plan.add(turn4.first);
		plan.add(turn4.second);
		plan.add(turn4.third);
		plan.add(np4);
		return plan;
	}

	
	
	
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
			Position center = BOT.linear(Velocity.make(v.PerpL().Hat()), R).position();
			if (turnRight) center = BOT.linear(Velocity.make(v.PerpR().Hat()), R).position();
			double rot = v.gs()/R*Util.turnDir(v.trk(), dtp.second.trk());
			BOT = s2.makeBOT(BOT.position(), BOT.time(), rot, v);
//			NavPoint MOT = new NavPoint(ProjectedKinematics.turn(BOT.position(), v, dt*0.5, R, turnRight).first,BOT.time()+dt*0.5);
//			MOT = s2.makeTurnMid(MOT.position(), MOT.time(), rot, v);
			Pair<Position,Velocity> pr = ProjectedKinematics.turn(BOT.position(), v, dt, R, turnRight);
			NavPoint EOT = new NavPoint(pr.first,BOT.time()+dt);
			EOT = s2.makeEOT(EOT.position(), EOT.time(), pr.second);
			double d = EOT.position().distanceH(goal);
			finalTime = d/v.gs()+EOT.time();
			fp.add(BOT);
//			fp.add(MOT);
			fp.add(EOT);

			if (dt > 15) {
//				NavPoint MOT2 = new NavPoint(ProjectedKinematics.turn(BOT.position(), v, dt*0.2, R, turnRight).first,BOT.time()+dt*0.2);
//				MOT2 = s2.makeTurnMid(MOT2.position(), MOT2.time(), rot, v);
//				NavPoint MOT3 = new NavPoint(ProjectedKinematics.turn(BOT.position(), v, dt*0.4, R, turnRight).first,BOT.time()+dt*0.4);
//				MOT3 = s2.makeTurnMid(MOT3.position(), MOT3.time(), rot, v);
//				NavPoint MOT4 = new NavPoint(ProjectedKinematics.turn(BOT.position(), v, dt*0.6, R, turnRight).first,BOT.time()+dt*0.6);
//				MOT4 = s2.makeTurnMid(MOT4.position(), MOT4.time(), rot, v);
//				NavPoint MOT5 = new NavPoint(ProjectedKinematics.turn(BOT.position(), v, dt*0.8, R, turnRight).first,BOT.time()+dt*0.8);
//				MOT5 = s2.makeTurnMid(MOT5.position(), MOT5.time(), rot, v);
//				fp.add(MOT2);
//				fp.add(MOT3);
//				fp.add(MOT4);
//				fp.add(MOT5);
			}

		}
		NavPoint np = new NavPoint(goal,finalTime);

//		fp.setExistingVelocity(v);
		fp.add(s);
		fp.add(np);
//f.pln("buildDirectTo 1"+fp);
//		try {
//			return generateVSCs(fp, fp.getFirstTime(), fp.getLastTime(), bankAngle, gsAccel, vsAccel);
//		} catch (Exception e) {
//f.pln(""+e);			
			return fp;
//		}

	}
	
	/**
	 * Given a starting plan, this returns a navpoint to get from point 0 to point 1 (assuming an existing velocity that is not compensated for
	 * @param working plan
	 * @param v existing velocity
	 * @param delay delay to be factored in before turn
	 * @param maxBank maximum bank angle
	 * @return new navpoint.  If the point's time is < 0 then it is redundant
	 */
	public static NavPoint turnToPointOLD(Plan working, Velocity v, double delay, double maxBank) {
		double R = Kinematics.turnRadius(v.gs(),maxBank);
		//double dt = ProjectedKinematics.turnTimeDirectTo(working.point(0).position(), v, working.point(1).position(), maxBank);
		double dt = ProjectedKinematics.directToPoint(working.point(0).position(), v, working.point(1).position(), R).third;
		if (working.size() < 2 || !working.initialVelocity(0).almostEquals(v) && dt > 1.0) {		
			return working.point(0).linear(v, dt/2 + 1.0 + delay);
		}
		return new NavPoint(Position.ZERO_LL,-1.0);
	}
	
}







//    static public class AugmentedPlan extends Plan {
//
//   	public int lpcErrorPt;
//    	public int kpcErrorPt;
//    	public ErrType etype;
//
//
//    	AugmentedPlan(AugmentedPlan p) {
//    		super(p);
//    		lpcErrorPt = p.lpcErrorPt;
//    		kpcErrorPt = p.kpcErrorPt;
//    		etype = p.etype;
//    	}
//    	AugmentedPlan(Plan p) {
//    		super(p);
//    		lpcErrorPt = p.getErrorLocation();
//    		kpcErrorPt = p.getErrorLocation();
//    		if (p.hasError()) {
//    			etype = ErrType.UNKNOWN;
//    		} else {
//    			etype = ErrType.NONE;
//    		}
//    	}
//
//    	AugmentedPlan(Plan p, int lloc, int kloc, ErrType t) {
//    		super(p);
//    		lpcErrorPt = lloc;
//    		kpcErrorPt = kloc;
//    		etype = t;
//    	}
//
//    	public AugmentedPlan copy() {
//    		AugmentedPlan p = new AugmentedPlan(super.copy());
//    		p.lpcErrorPt = lpcErrorPt;
//    		p.kpcErrorPt = kpcErrorPt;
//    		p.etype = etype;
//    		return p;
//    	}
//
//    	public boolean hasTypedError() {
//    		return etype != ErrType.NONE;
//    	}
//
//    }










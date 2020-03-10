/* Trajectory Generation
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TrajGen_H
#define TrajGen_H

#include "Plan.h"
//#include "UnitSymbols.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "Velocity.h"
#include "format.h"
#include "Util.h"
#include "Constants.h"
#include "string_util.h"
#include "Triple.h"
#include "Tuple5.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>



namespace larcfm {



/**
 * <p>Trajectory generation functionality.  This class translates Linear plans into Kinematic plans.  The translation back to a 
 * Linear plan from a Kinematic plan is accomplished using reversion methods defined in PlanUtil (See PlanUtil.revertAllTCPS).</p>
 * 
 * <p><b>Note:</b>  It is very easy to create a linear plan that is infeasible, that is, it contains a segment where there is insufficient 
 *        time or distance to achieve the acceleration implicit in it.  If TrajGen.makeKinematicPlan fails to generate a Kinematic 
 *        plan, an error message is added to the plan.  There are "repair" methods available that modify the linear plan that 
 *        often turn it into a feasible plan (See PlanUtil.repairShortVsLegs, PlanUtil.fixBadTurns)</p>
 *        
 * 
 */
class TrajGen {

public:


private:
	static bool verbose; // = false;


public:
//	static bool vertexNameInBOT;
//	static double gsOffsetTime;

    static const double MIN_MARK_LEG_TIME;
    static const double minorVfactor; // = 0.01; // used to differentiate "minor" vel change vs no vel change
    static const double maxVs;         // only used for warning
	static const double maxAlt;         // only used for error

	static const std::string turnFail;
	static const std::string gsFail;
	static const std::string vsFail;


	static Triple<double,double,double> getStoredParameters(const Plan& p);
	/**
	 * Store accel magnitude values used to generate this kinematic plan in a plan's note field.  Negative values will be ignored.
	 * @param p plan to modify
	 * @param bankAngle bank angle in radians, negative for none
	 * @param gsAccel gs accel in m/s, negative for none
	 * @param vsAccel vs accel in m/s, negative for none
	 */
	static void addParams(Plan& p, double bankAngle, double gsAccel, double vsAccel);


	/** Returns true if turn at i can be inscribed in the available leg space.   Note that it will check
	 *  for collinear points using prevTrackChange and nextTrackChange
	 * 
	 * @param lpc     source plan
	 * @param i       vertex to be tested
	 * @param BOT     Beginning of turn
	 * @param EOT     End of turn
	 * @return        true iff the BOT - EOT pair can be placed within vertex
	 */
	static bool turnCanBeInscribed(const Plan& lpc, int i, const Position& BOT, const Position& EOT) ;

	
	/**
	 * This takes a Plan lpc and returns a plan that has all points with vertical changes as "AltPreserve" points, with all other points
	 * being "Original" points.  Beginning and end points are always marked.
	 * @param lpc source plan
	 * @param vsAccel vertical acceleration
	 * @return kinematic plan with marked points
	 */
	static Plan markVsChanges(const Plan& lpc);

	
	static Plan generateTurnTCPs(const Plan& kpc, double bankAngle) ;

	/** 
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 * bank angle must be nonnegative!!!!!
	 * 
	 * @param lpc                 linear plan
	 * @param default_bank_angle  the default bank angle, if a radius is not present in the plan. 
	 * @param continueGen         if true, do not terminate generation if an error message is produced
	 * @return           a turn plan with BOTs and EOTs
	 */
	static Plan generateTurnTCPs(const Plan& lpc, double default_bank_angle, bool continueGen);


	/** Generate turn at vertex "ixNP2".  If there is a stored radius use that value, otherwise
	 *  compute radius from default_bankAngle and speed
	 *
	 * @param traj                traj
	 * @param ixNp2               turn vertex -- should not be in a turn already
	 * @param dafault_bankAngle   default banks angle used to compute radius
	 * @param continueGen         if true continue turn generation in the presence of errors
	 */
	static void generateTurnTcpAt(Plan& traj, int ixNp2, double default_bankAngle,  bool continueGen);

	/** Generate turn at vertex "ixNP2" with radius R
	 * 
	 * @param traj          trajectory under construction
	 * @param ixNp2         index of np2 in plan
	 * @param R             unsigned radius
	 * @param bankUsed      true if default bank angle was used to compute R
	 * @param continueGen   continue generation, even with problems
	 */
	static void generateTurnTcp(Plan& traj, int ixNp2, double R,  bool bankUsed, bool continueGen);

	/**  Calculates the minimum leg distance that can handle a specified radius
	 * 
	 * @param p        the Plan (i.e. trajectory)
	 * @param ix       vertex index
	 * @param radius   radius of turn
	 * @return         minimum leg distance, return 0 if no turn would be generated here
	 */
	static double neededTurnLegDistance(const Plan& p, int ix, double radius);

	static double neededLegLengthBankAngle(const Plan& p, int ix, double bankAngle);

	/**
	 * Re-aligns points that are within a newly generated turn at j
	 */
	static std::pair<int,double> movePointsWithinTurn(Plan& traj,int ixBOT, int ixNp2, int ixEOT,  double gsIn);

	/**
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 *
	 * @param lpc        linear plan
	 * @return           a plan with BOTs and EOTs
	 */
	static Plan generateTurnTCPsRadius(const Plan& lpc);

	/**
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 *
	 * @param lpc        linear plan
	 * @param continueGen continue generation, even with problems
	 * @return           a plan with BOTs and EOTs
	 */
	static Plan generateTurnTCPsRadius(const Plan& lpc, bool continueGen);

	
	/**  Create a BGS-EGS pair starting at index "i".  The speed into and out of EGS will be "targetGs"
	 *  Note: targetGS is often chosen to be gsOut(i+1) so that there is continuity at "i+1" after this method is called at "i"
	 * 
	 * @param traj            trajectory
	 * @param i               index where BGS is created 
	 * @param gsAccel         unsigned acceleration value
	 * @param targetGs        target ground speed (usually gsOut(i+1) -- calculated earlier)
	 * @param timeOffset      create BGS this much time after time(i), it can be zero
	 * 
	 * Note:  This method will skip over points until the targetGs is obtained.
	 */
	static void generateGsTCPsAt(Plan& traj, int i, double gsAccel, bool repairGs);

	/** Adjust ground speed of all points that will fall within distance to new EGS (distToEGS)
	 *  This is necessary because points between a newly added BGS -- EGS region will have
	 *  time based on the previous non-accelerated section. 
	 * 
	 * @param traj       the plan file of interest
	 * @param ixBGS      index of BGS (that has just been added, but EGS has not yet been added!)
	 * @param aBGS       signed acceleration value
	 * @param distToEGS  distance from BGS to new EGS point
	 * 
	 * Note: It is necessary to adjust by distance and not time.  The times will change, so the EGS should added
	 * after this function is called.
	 */
	static bool adjustGsInsideAccel(Plan& traj, int ixBGS, double aBGS, double distToEGS);


	/** generate an BGS-EGS pair with the EGS at point i
	 *
	 * @param traj        trajectory
	 * @param i           point with GSOut = 0
	 * @param a           absolute acceleration
	 * @param targetGs    should be zero are almost zero
	 * @return            index where BGS is located
	 */

	static int generateGsToZero(Plan& traj, int i, double a, double targetGs);
	/** Adjust ground speed of all points that will fall within distance to new EGS (distToEGS)
	 *
	 * @param traj       the plan file of interest
	 * @param ixBGS      index of BGS
	 * @param aBGS       signed acceleration value
	 * @param distToEGS  distance from BGS to new EGS point
	 *
	 * Note: this function used where EGS has already been added.
	 */
	static void adjustGsInsideAccelToEGS(Plan& traj, int ixBGS, double aBGS, int ixEGS);

	static bool attemptRepair2(Plan& kpc, int ixBGS, int ixEGS, double maxGsAccel);

	/** Create a BGS point from a previous point (that is using its TCP info in tcp1)
	 * 
	 * @param tcp1
	 * @param a
	 * @param timeOffset
	 * @param isAltPreserve
	 * @return
	 */
	static TcpData makeBGS(const TcpData& tcp1, double a, bool isAltPreserve);

	static TcpData makeEGS();

	/** 
	 * 
	 * Generates ground speed TCPs  (in forward direction)
	 * 
	 * @param fp         plan to be processed
	 * @param maxGsAccel ground speed acceleration (non-negative)
	 * @param repairGs        if segment is too short, repair by skipping over it (i.e. achieve max possible accel)
	 * @return           plan with BGS-EGS pairs added
	 */
	static Plan generateGsTCPs(const Plan& fp, double maxGsAccel, bool repairGs);

	static void generateGsTCP(Plan& traj, int i, double maxGsAccel, bool repairGs);

	/** Generate Vertical acceleration TCPs
	 *  It assumes that all horizontal passes have been completed.
	 *  
	 * @param kpc      kinematic plan with final horizontal path
	 * @param maxVsAccel  vertical speed acceleration (positive)
	 * @param continueGen true means continue generation even with problems
	 * @return plan
	 */
	static Plan generateVsTCPs(const Plan& kpc, double maxVsAccel, bool continueGen, bool repairVs_zeroGs);

	static Plan generateVsTCPs(const Plan& kpc, double maxVsAccel);


	/** Generate a BVS EVS pair at index i if deltaVs is large enough
	 * 
	 * @param traj            trajectory
	 * @param i               index
	 * @param vsAccel         vertical acceleration
	 * @param continueGen     if true continue generation even if errors are encountered, results in a partial kinematic plan
	 * @return                the index of the EVS if successful, otherwise just i
	 * 
	 * t1 = previous point's time (previous EVS)
	 * t2 = np2.time() = end time of the linear segment where the vertical speed change occurred
	 * nextVsChangeTm = time of the next vertical change (i.e. where the next TCP will appear)
	 * vs1 vertical speed into point at time t2
	 * vs2 vertical speed out of point at time t2
	 * 
	 *                             vs1                              vs2
	 *             t1  -------------------------------- t2 ------------------------------ nextVsChangeTm
	 *                                            ^     "i"   ^
	 *                                            |           |
	 *                                          tbegin       tend
	 * 
	 */
	static int generateVsTCP(Plan& traj, int i, double vsAccel, bool continueGen,  bool repairVs_zeroGs);


	/** This method makes all of the vertical speed out of bindex to be vs1.  It also
	 *  smoothes the velocity profile according to the "accel" parameter.
	 * 
	 * Note: this is essential to making the BVS-EVS pair consistent
	 * 
	 * @param traj     trajectory of interest
	 * @param bindex   start of smoothing region
	 * @param eindex   end of smoothing region
	 * @param vs1      velocity to use for smoothing
	 * @param accel    acceleration in this BGS EGS region
	 * @return true, if successful
	 */
	static bool fixAlts(Plan& traj, int bindex, int eindex, double vs1, double accel);

	/** calculate radius at vertex ix,  If a radius has been set use it, otherwise use bank angle and GS to
	 *  calculate value
	 * 
	 * @param p          Plan 
	 * @param ix         index of vertex point
	 * @param bank       bank angle
	 * @return
	 */
	static double calcRadius(const Plan& p, int ix, double bank);

	/** 
	 * Repair function for a linear plan  This can eliminate short legs.   This function look for two sequential turns
	 * with a very short leg segment between them. It collapses the two turns into one or no turns
	 * @param fp        linear plan to be repaired
	 * @param bank      bank angle
	 * @param addMiddle if true, it adds a middle point when it deletes two points (one turn left)
	 * @return          repaired Plan
	 * 
	 * Note: if addMiddle is false, point info and named waypoint may be lost
	 * 
	 */
	static Plan linearRepairShortTurnLegs(const Plan& fp, double bank, bool addMiddle) ;

	
	// Make vertical speeds constant by adjusting altitudes between wp1 and wp2
	// Assume there are no vertical speed accelerations
	// do not alter wp1 and wp2
	/**
	 * Penultimate Kinematic generation pass.  Repairs altitudes so that only VSC points have vertical speed changes.
	 * Averages vertical speeds between alt preserved points by modifying altitudes on points between them.
	 * @param kpc kinematic plan
	 * @return plan
	 */
	static Plan makeMarkedVsConstant(const Plan& kpc) ;
	
	
	/** Method to repair an infeasible linear plan where there are segments that are too short to
	 *  achieve specified acceleration.
	 * 
	 * @param lpc             linear plan
	 * @param repairTurn      true, to attempt repair of infeasible turns
	 * @param repairVs        true, to attempt repair of infeasible vertical speed segments
	 * @param bankAngle       bank angle
	 * @param vsAccel         vertical speed acceleration
	 * @return                repaired plan
	 */
	static Plan repairPlan(const Plan& lpc, bool repairTurn, bool repairVs,
			double bankAngle, double vsAccel);
	

	/** TODO (helps me find this method)
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
	static Plan makeKinematicPlan(const Plan& fp, double bankAngle, double gsAccel, double vsAccel, bool repairTurn, bool repairVs, bool repairGs);

	/** 
	 * The resulting PlanCore will be "clean" in that it will have all original points, with no history of deleted points. 
	 *  Also all TCPs should reference points in a feasible plan. 
	 *  If the trajectory is modified, it will have added, modified, or deleted points.
	 *  If the conversion fails, the resulting plan will have one or more error messages (and may have a point labeled as "TCP_generation_failure_point").
	 *	@param fp input plan (is linearized if not already so)
	 *  @param bankAngle maximum allowed (and default) bank angle for turns
	 *  @param gsAccel   maximum allowed (and default) ground speed acceleration (m/s^2)
	 *  @param vsAccel   maximum allowed (and default) vertical speed acceleration (m/s^2)
	 * @return plan
	 */
	static Plan makeKinematicPlan(const Plan& fp, double bankAngle, double gsAccel, double vsAccel);

	/**
	 * This assumes a plan that has altitude preserve points marked.  Generate the vertical profile (only) for VsTCPs in this plan.
	 * @param p         input plan (need not be linear)
	 * @param vsAccel   vertical acceleration
	 * @return plan with vertical TCPs generated
	 */
	static Plan makeVsKinematicPlan(const Plan& p, double vsAccel);
	
	/**
	 * Attempts to compute (heuristic) the amount of time to continue current velocity before starting direct to turn.  (in 2-D)
	 * @param lpc Plan trying to connect to at point zero.
	 * @param so current position
	 * @param vo current velocity
	 * @param to current time
	 * @param bankAngle max bank angle
	 * @return
	 */
private: static  double directToContinueTime(const Plan& lpc, const Position& so, const Velocity& vo, double to, double bankAngle) ;
		
public:
	/**  *** UNUSED ***
	 * 
	 * @param lpc    a linear plan that needs to be connected to, preferable at point 0
	 * @param so     current position
	 * @param vo     current velocity
	 * @param to     current time
	 * @param bankAngle  turn bank angle
	 * @return new plan with (so,vo,to) added as a first point and a leadIn point added
	 */
	static Plan genDirect2(const Plan& lpc, const Position& so, const Velocity& vo, double to, double bankAngle);

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
	static Plan genDirectToLinear(const Plan& fp, const Position& so, const Velocity& vo, double to, double bankAngle, double timeBeforeTurn) ;
	

	/**
	 * Construct a (hopefully) feasible linear plan that describes a path from s to the goal point.
	 * If this fails, it reurns a simple direct plan.
	 */
	static Plan buildDirectTo(const NavPoint& s, const Velocity& v, const Plan& base, double bankAngle);

	/**
	 * Returns a PlanCore version of the given plan.  If it was a kinematic plan, this will attempt to regress the TCPs to their original
	 * source points (if the proper metadata is available).  If this is already a PlanCore, return a copy.
	 * @param fp plan
	 * @return plan
	 */
	static Plan makeLinearPlan(const Plan& fp);


public:



	static void setVerbose(bool b);

	static void printError(const std::string& s);

};

}
#endif // Plan_H


/*
u * Copyright (c) 2011-2017 United States Government as represented by
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
class TrajGen {

public:

	//enum GsMode {PRESERVE_GS};





//	static class TransformationFailureException extends Exception;

private:
	static bool verbose; // = false;
	static bool method2; // = false;
public:

    static const double MIN_ACCEL_TIME;
    static const double MIN_TURN_BUFFER;

    static const double MIN_VS_CHANGE;
    static const double MIN_VS_TIME;
    static const double minorVfactor; // = 0.01; // used to differentiate "minor" vel change vs no vel change
    static const double maxVs;         // only used for warning
	static const double maxAlt;         // only used for error
	static const std::string minorTrkChangeLabel;

    //static const double minVsChangeRecognizedDefault;
	/**
	 * Trajectory generation is discretized by a certain time unit (meaning no two points on the trajectory should be closer in time than this value) 
	 */
	static double getMinTimeStep();
//	/**
//	 * Set the minimum time between two points on a plan.  Note that this should not be less than Constants.TIME_LIMIT_EPSILON, which is a
//	 * "stronger" limit on the minimum.
//	 */
//	static void setMinTimeStep(double t);


	/**
	 * Returns an index at or before iNow where there is a significant enough track change to bother with.
	 * This is used to "backtrack" through collinear points in order to get a reasonable sized leg to work with. 
	 */
	static int prevTrackChange(const Plan&  fp, int iNow);
	
	/**
	 * Returns an index at or after iNow where there is a significant enough track change to bother with
	 * This is used to move forward through collinear points in order to get a reasonable sized leg to work with. 
	 */
	static int nextTrackChange(const Plan& fp, int iNow);

	/** Returns true if turn at i can be inscribed in the available leg space.
	 * 
	 * @param lpc     source plan
	 * @param i       vertex to be tested
	 * @param BOT     Beginning of turn
	 * @param EOT     End of turn
	 * @return        true iff the BOT - EOT pair can be placed within vertex
	 */
	static bool turnIsFeas(const Plan& lpc, int i, const NavPoint& BOT, const NavPoint& EOT) ;

	
	/**
	 * This takes a Plan lpc and returns a plan that has all points with vertical changes as "AltPreserve" points, with all other points
	 * being "Original" points.  Beginning and end points are always marked.
	 * @param lpc source plan
	 * @return kinematic plan with marked points
	 */
	static Plan markVsChanges(const Plan& lpc);

	
	
	// takes a linear plancore
	// bank angle must be nonnegative!!!!!
	/**
	 * Kinematic generations pass that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It only assumes legs are long enough to support the turns.
	 * kpc will have marked vs changes (if any).
	 */
	static Plan generateTurnTCPs(const Plan& kpc, double bankAngle, bool continueGen) ;

	static Plan generateTurnTCPs(const Plan& kpc, double bankAngle) ;

	/** 
	 * Kinematic generator that adds turn TCPs.  This defers ground speed changes until after the turn.
	 * It assumes legs are long enough to support the turns.
	 * 
	 * @param lpc        linear plan
	 * @param strict     if strict do not allow any interior waypoints in the turn
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

	static bool trackContinuousAt(const Plan& traj, int ix, double maxDelta);
	/**
	 * 
	 * @param traj          trajectory under construction
	 * @param lpc           original lpc
	 * @param ixNp2         index of np2 in plan
	 * @param R             unsigned radius
	 * @param continueGen   continue generation, even with problems
	 */
	static void insertTurnTcpsInTraj(Plan& traj, const Plan& lpc, int ixNp2, double R, bool continueGen);



	/**
	 * Re-aligns points that are within a newly generated turn at j
	 */
	static int movePointsWithinTurn(Plan& traj,int ixBOT, int ixNp2, int ixEOT,  double gsIn);
 
	// create GSC TCPS between np1 and np2 using vin from previous leg
	/**
	 * @param np1 start point of the acceleration segment/where the speed change occurs
	 * @param np2 end point of the acceleration segment
	 * @param targetGs target gs
	 * @param vin velocity in
	 * @param a    signed (horizontal) acceleration
	 * @return third component: accel time, or negative if not feasible
	 * NOTE!!! This has been changed so the accel time is always returned, but if it is less than getMinTimeStep(), there is no region and this has to be handled by the calling program!!!
	 */
	static Triple<NavPoint,NavPoint,double> gsAccelGenerator(const Plan& traj, int i, double gsIn, double targetGs, double a, double timeOffset, bool allowOverlap) ;
	
	static void generateGsTCPsAt(Plan& traj, int i, double gsAccel,  double targetGs, double timeOffset);

	static void generateGsTCPsAt(Plan& traj, int i, double gsAccel,  double targetGs, double timeOffset,  bool allowOverlap);

	/** remove all points that will fall within distance to new EGS (distToEGS)
	 * 
	 * @param traj       the plan file of interest
	 * @param ixBGS      index of BGS 
	 * @param aBGS       acceleration value
	 * @param distToEGS  distance from BGS to new EGS point
	 */
	static void adjustGsInsideAccel(Plan& traj, int ixBGS, double aBGS, double distToEGS, double timeOffset, bool isAltPreserve);

	static TcpData makeBGS(const TcpData& tcp1, double a, double timeOffset, bool isAltPreserve);

	static TcpData makeEGS(const TcpData& tcp1);

	/** Generates ground speed TCPs
	 * 
	 * @param fp        plan to be processed
	 * @param gsAccel   ground speed acceleration
	 * @param repairGs  if true, attempt repair by delaying some of speed change to subsequent segments
	 * @return          plan with BGS-EGS pairs added
	 */
	static Plan generateGsTCPs(const Plan& fp, double gsAccel, bool repairGs, bool useOffset);


	static Triple<double,double,double> calcVsTimes(int i, Plan& traj, double vsAccel);

	/** Generate Vertical acceleration TCPs
	 *  It assumes that all horizontal passes have been completed.
	 *  
	 * @param kpc      kinematic plan with final horizontal path
	 * @param vsAccel  vertical speed acceleration
	 * @return
	 */
	static Plan generateVsTCPs(const Plan& kpc, double vsAccel, bool continueGen) ;

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
	static Triple<double,double,double> vsAccelGenerator(double t1, double t2, double tLast, double vs1, double vs2, double a);

	
	// based on the PlanCore
//	static Plan generateVsTCPsSimple(const Plan& lpc, double vsAccel);


	static std::pair<double,double> vsAccel(double sz, double vz,  double t, double a) ;

	/**
	 * Repair function for lpc. This can eliminate a short first leg in the lpc.  This may alter the initial velocity of the plan
	 * @param fp
	 * @param bank
	 * @return
	 */
	static Plan removeShortFirstLeg(Plan& fp, double bank) ;

	/**
	 * Repair function for lpc. This can eliminate a short last leg in the lpc.  This may alter the final velocity of the plan
	 * @param fp
	 * @param bank
	 * @return
	 */
	static Plan removeShortLastLeg(Plan& fp, double bank);
	
	/**
	 * Repair function for lpc.  This can eliminate short legs in the lpc.  This may alter the initial velocity of the plan
	 * @param fp
	 * @param bank
	 * @param addMiddle if true, it adds a middle point when it deletes two points
	 * @return
	 */
	static Plan linearRepairShortTurnLegs(const Plan& fp, double bank, bool addMiddle) ;
	
	
	/**
	 * Repair function for lpc.  Removes vertex point if turn is infeasible.
	 * @param fp
	 * @param bankAngle
	 * @return
	 */
	static Plan removeInfeasibleTurns(const Plan& fp, double bankAngle, bool strict);

	/**
	 * Ff there is a segment with a ground speed change at point j that cannot be achieved with the gsAccel value, then it
	 * makes the ground speed before and after j the same (i.e. averages over two segments)
	 * 
	 * @param fp         plan to be repaired
	 * @param gsAccel
	 * @return
	 */
	static Plan linearRepairShortGsLegs(const Plan& fp, double gsAccel) ;



	/**
	 * Attempts to repair plan with infeasible vertical points by averaging vertical speeds over 2 segments.
	 * Assumes linear plan input.
	 * @param fp
	 * @param vsAccel
	 * @param minVsChangeRequired
	 * @return
	 */
    private: static Plan linearRepairShortVsLegs(const Plan& fp, double vsAccel);
	
public:

//	static Plan removeExtraAlt0Points(Plan& fp) ;
	
	// Make vertical speeds constant by adjusting altitudes between wp1 and wp2
	// Assume there are no vertical speed accelerations
	// do not alter wp1 and wp2
	/**
	 * Penultimate Kinematic generation pass.  Repairs altitudes so that only VSC points have vertical speed changes.
	 * Averages vertical speeds between alt preserved points by modifying altitudes on points between them.
	 * @param kpc
	 * @return
	 */
	static Plan makeMarkedVsConstant(const Plan& kpc) ;
	
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
	
	static Plan repairPlan(const Plan& lpc, bool repairTurn, bool repairGs, bool repairVs,
			bool flyOver, bool addMiddle, double bankAngle, double gsAccel, double vsAccel);
	

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
	static Plan makeKinematicPlan(const Plan& fp, double bankAngle, double gsAccel, double vsAccel,
			                                          bool repairTurn, bool repairGs, bool repairVs);

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
	static Plan makeKinematicPlan(const Plan& fp, double bankAngle, double gsAccel, double vsAccel,
			bool repair);



	/**
	 * Remove records of deleted points and make all remaining points "original"
	 * Used to clean up left over generation data
	 * @param fp
	 */
	static void cleanPlan( Plan& fp);
	
	
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
	/**
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
	static Plan genDirectTo(const Plan& fp, const Position& so, const Velocity& vo, double to,
			                             double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn) ;

//	static Plan genDirectTo(const Plan& fp, const Position& so, const Velocity& vo, double to,
//			                             double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn) ;

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
	static Plan genDirectToRetry(const Plan& fp, const Position& so, const Velocity& vo, double to,
            double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn, double timeIntervalNextTry);

//	static Plan genDirectToRetry(const Plan& fp, const Position& so, const Velocity& vo, double to,
//            double bankAngle, double gsAccel, double vsAccel, double timeBeforeTurn, double timeIntervalNextTry);

	/**
	 * Construct a (hopefully) feasible linear plan that describes a path from s to the goal point.
	 * If this fails, it reurns a simple direct plan.
	 */
	static Plan buildDirectTo(std::string id, const NavPoint& s, const Velocity& v, const Plan& base, double bankAngle);

	/**
	 * Returns a PlanCore version of the given plan.  If it was a kinematic plan, this will attempt to regress the TCPs to their original
	 * source points (if the proper metadata is available).  If this is already a PlanCore, return a copy.
	 */
	static Plan makeLinearPlan(const Plan& fp);

//	// only add a point it it will not overwrite an existing one!
//    private: static void safeAddPoint(Plan& p, const NavPoint& n);

public:


	
//	/**
//	 * Returns a new Plan that sets all points in a range to have a constant GS.
//	 * The return plan type will be the same as the input plan type.
//	 * This re-generates a kinematic plan, if necessary (if this fails, error status will be set)
//	 * The new gs is specified by the user.
//	 */
//	static Plan makeGSConstant(const Plan& p, double newGs,  double bankAngle, double gsAccel, double vsAccel,
//			bool repair) ;
//
//	/**
//	 * Returns a new Plan that sets all points in a range to have a constant GS.
//	 * The return plan type will be the same as the input plan type.
//	 * This re-generates a kinematic plan, if necessary (if this fails, error status will be set)
//	 * The new gs is the average of the linear version of the plan.
//	 */
//	static Plan makeGSConstant(const Plan& p, double bankAngle, double gsAccel, double vsAccel,
//			bool repair);

	

	static void setVerbose(bool b);

	static void printError(const std::string& s);

};

}
#endif // Plan_H


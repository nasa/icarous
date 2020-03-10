/*
 * PlanUtil - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PlanUtil_H
#define PlanUtil_H

#include "Units.h"
#include "Util.h"
#include "NavPoint.h"
#include "Position.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "EuclideanProjection.h"
#include "Plan.h"
#include "TcpData.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace larcfm {
/**
 * Utilities to operate on Plans
 * 
 */
class PlanUtil {

public:

    static const double maxTurnDeltaStraight;
	static const double maximumInterpolationNumber;	// maximum number of legs that a plan should have after interpolation
	static const double minimumInterpolationLegSize;  // do not allow interpolations to create legs smaller than this (can override maximumInterpolationNumber)


	/**
	 * Returns an index before iNow where there is a significant enough track change to bother with.
	 * This is used to "backtrack" through collinear points in order to get a reasonable sized leg to work with. 
	 * If there is not previous track change it returns 0
     *
	 * Note: must use both trkIn and TrkOut because of large great circles
	 * @param fp plan
	 * @param iNow index
	 * @return index of track change
	 * 
	 */
	static int prevTrackChange(const Plan&  fp, int iNow);

	/**
	 * Returns an index after iNow where there is a significant enough track change to bother with
	 * This is used to move forward through collinear points in order to get a reasonable sized leg to work with. 
	 * 
	 * If there is not next track change it returns fp.size()-1
	 * @param fp plan
	 * @param iNow index
	 * @return index of next track change
	 */
	static int nextTrackChange(const Plan& fp, int iNow);

	/**
	 * Returns an index after iNow where there is a significant enough vertical speed change to bother with
	 * This is used to move forward through collinear points in order to get a reasonable sized leg to work with. 
	 * 
	 * If there is not a next vs change it returns fp.size()-1
	 * @param p plan
	 * @param iNow index
	 * @return index of vertical speed change
	 */
	static int prevVsChange(const Plan& p, int iNow);

	/**
	 * Returns an index after iNow where there is a significant enough vertical speed change to bother with
	 * This is used to move forward through collinear points in order to get a reasonable sized leg to work with. 
	 * 
	 * If there is not a next vs change it returns fp.size()-1
	 * @param p plan
	 * @param iNow index 
	 * @return index of next vertical speed change
	 */
	static int nextVsChange(const Plan& p, int iNow);


private:
    /** Advance forward in plan "p"  starting at time "curTm" a distance of "advDistance" within a single segment
     * 
     * Note : assumes the advance by distance will not leave current segment
     * Note : this can be used before there is a ground speed profile -- it does not depend upon correct velocities
     * 
     * @param p            plan of interest
     * @param curTm        currentTime  of so
     * @param advDistance  distance to advance
     * @param linear       if true, treat plan as a linear plan (i.e. path is not curved)
     * @return             Position "advDistance" ahead of "curTm"
     */
	static Position positionFromDistanceInSeg(const Plan& p, double curTm, double advDistance, bool linear);

public:
	static std::pair<Position,int> positionFromDistance(const Plan& p, double currentTime, double advanceDist, bool linear);


	/** time required to cover distance "dist" if initial speed is "gsInit" and acceleration is "gsAccel"
	 * 
	 * @param gsAccel   ground speed acceleration
	 * @param gsInit    initial ground speed
	 * @param dist      distance travelled
	 * @return time
	 */
	static double timeFromGs(double gsInit, double gsAccel, double dist);

	static Plan applyWindField(const Plan& pin, const Velocity& v);

	/**  Simple check that current position (currentPos) is where it should be according to plan "solution"
	 * 
	 * @param solution      the trajectory 
	 * @param currentTime   current time
	 * @param currentPos    current position
	 * @return              true if position is almost equal to currentPos.position(currentTime)
	 */
	static bool isCurrentPositionUnchanged(const Plan& solution, double currentTime, const Position& currentPos);

	/** Used in Unit tests to make sure that a resolution does not change current position or velocity of vehicle
	 * 
	 * @param solution plan
	 * @param currentTime time
	 * @param currentPos position
	 * @param currentVel velocity
	 * @return true if correct
	 */
	static bool checkMySolution(const Plan& solution, double currentTime, const Position& currentPos, const Velocity& currentVel);

	/** Calculate a distance difference between two plans
	 * 
	 * @param A    plan A
	 * @param B    plan B
	 * @return distance
	 */
	static double distanceBetween(const Plan& A,const Plan& B);

	// from Aviation Formulary
	// longitude sign is reversed from the formulary!
	static double lonCross(const Plan& ac, int i, double lat3);

	// from Aviation Formulary
	static double latMax(const Plan& ac, int i);

	static int addLocalMaxLat(Plan& ac, int i);

	static void insertLocalMax(Plan& ac);

	static int insertVirtual(Plan& ac, double time);


	static double getLegDist(const Plan& ac, int i, double accuracy, double mindist);

	// this adds for the leg starting at or before startTm, and ending before or at endTm
	/** 
	 * this adds virtual points int the plan starting at or before startTm, and ending before or at endTm
	 * @param ac           plan
	 * @param accuracy	   accuracy of projections
	 * @param startTm      starting segment
	 * @param endTm        ending segment
	 */
	static void interpolateVirtuals(Plan& ac, double accuracy, double startTm, double endTm);

	static void interpolateVirtuals(Plan& ac, double accuracy);

	static Plan revertAllTCPs(const Plan& pln);

	/** 
	 * Revert all TCPs in the plan, i.e. remove all acceleration zones and replace with instantaneous
	 * velocity changes.   This undoes TrajGen.makeKinematicPlans.
	 * 
	 * Note:  This retains the acceleration values in the resulting linear plan.
	 * 
	 * @param pln         kinematic plan to be reverted to a linear plan
	 * @param markIndices if true, add :ksrc-#: to each point's info (original behavior is "false")
	 * This may include multiple flags in a single point.
	 * @return plan
	 */
	static Plan revertAllTCPs(const Plan& pln, bool markIndices);

private:
	/** Called after revertVsTCPs to set AltPreserve.  Assumes that vsAccels have been saved in reverted Plan
	 * 
	 * Note: this is very similar to TrajGen.markVsChanges
	 * 
	 * @param lpc
	 * @param vsAccel
	 * @return
	 */
	static Plan setAltPreserveByDelta(const Plan& lpc);
	static double findVsAccel(const Plan& kpc);

public:

	// this removes all virtuals AFTER time startTm and BEFORE endTm.
	// this will NOT remove any Fixed virtuals!
	// returns TRUE if all virtuals removed, otherwise FALSE
	static bool removeVirtualsRange(Plan& ac, double startTm, double endTm, bool all);


	static bool removeVirtuals(Plan& ac);

	/** 
	 * 
	 *  Used in STRATWAY to structurally revert all TCPS "near" ix.
	 *  Note that this function will timeshift the points after ix to regain 
	 *  original ground speed into the point after ix.  
	 *  
	 *  NOTE: This method does not depend upon source time or source position
	 * 
	 * @param pln plan
	 * @param ix  The index of one of the TCPs created together that should be reverted
	 * @return index of the reverted point
	 */
	static int revertGroupOfTCPs(Plan& pln, int ix);

	/** if "ix" is a BGS, then it reverts the BGS-EGS pair back to a single linear point
	 *  Note: It (does not depend upon source time or source position!)
	 *  
	 * @param pln                  plan
	 * @param ix                   index of BGS
	 * @param revertPreviousTurn   if true then if this GS segment is right after a turn then revert the turn as well  
	 * @return                     index of reverted point
	 */
	static int revertGsTCP(Plan& pln, int ix, bool revertPreviousTurn);

	static void makeWellFormedEnds(Plan& lpc);


	/**  make a new plan that is identical to plan from startTime to endTime 
	 *   It assumes that  startTime and endTime are in linear segments.  See also Plan.cut
	 * 
	 * @param plan         source plan
	 * @param startTime    absolute time of start time
	 * @param endTime      absolute time of end time
     *
	 * @return   truncated plan
	 */
	static Plan cutDownLinear(const Plan& plan, double startTime, double endTime);


	/** 
	 * Cut down a plan so that it only contains points between timeOfCurrentPosition and intentThreshold.
	 * This method cuts a Plan so that the acceleration information after intentThreshold is discarded.  The plan
	 *  is continued linearly to time tExtend.  The first time point of the new plan is the 
	 *  NavPoint before timeOfCurrentPosition in the plan.  The  intentThreshold and tExtend times are absolute.
	 * 
	 * @param plan                      Plan file to be cut
	 * @param timeOfCurrentPosition     Current location of aircraft in the plan file
	 * @param intentThreshold           the absolute lookahead time -- all acceleration information after this time is not copied
	 * @param tExtend                   After the intentThreshold, the plan is extended linearly to this time (absolute time)
	 * @return plan
	 */
	static Plan cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold, double tExtend);

	/** Create new Plan from time "timeOfCurrentPosition" to time "intentThreshold"
	 * 
	 *  NOTE: currently only being used in Unit Tests
	 * 
	 * @param plan plan
	 * @param timeOfCurrentPosition time
	 * @param intentThreshold time
	 * @return plan
	 */
	static Plan cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold);

	static std::pair<bool,double> enoughDistanceForAccel(const Plan& p, int ix, double maxAccel, double M);

	/** Fix Plan p at ix if there is not enough distance for the current speed and specified acceleration (maxAccel)
	 *  It makes the new ground speed as close to the original as possible (that is achievable over the distance)
	 * 	
	 * @param p         plan 
	 * @param ix        index of ground speed change 
	 * @param maxAccel  maximum ground speed acceleration (non-negative)
	 * @param checkTCP  if true, do not alter time if point and it is an EOT or EVS
	 * @param M max time
	 */
	static void fixGsAccelAt(Plan& p, int ix, double maxAccel, bool checkTCP, double M);

	static int hasPointsTooClose(const Plan& plan) ;

	/** This calculates a heuristic measure of the difference between two plans.   Currently only
	 * used in KinematicsPosition test
	 * 
	 * @param lpc   plan 1
	 * @param kpc   plan 2
	 * @return metric
	 */
	static double diffMetric(const Plan& lpc, const Plan& kpc);

	// will not remove segments that are longer than maxLegSize
	static Plan unZigZag(const Plan& pp);

	// will not remove segments that are longer than maxLegSize
	static Plan unZigZag(const Plan& pp, double maxLegSize) ;

	static bool aboutTheSameTrk(const Velocity& v1, const Velocity& v2, double sameTrackBound);

	static Plan removeCollinearTrk(const Plan& pp, double sameTrackBound);

	// This methods assumes plan is linear
	/**
	 * Make a new plan with constant ground speed from wp1 to wp2. 
	 * Assumes input plan is linear.
	 * 
	 * <pre>
	 * 
	 *       200     200     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    mkGSConstant(p, 1, 3, 500)
	 *    
	 *    	 200     500     500     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    Note that if wp1 == wp2 no change is made.
	 * </pre>
	 * 
	 * @param p plan
	 * @param i index of waypoint 1
	 * @param j index of waypoint 2
	 * @param gs ground speed
	 * @return a new plan
	 */
	static Plan mkGsConstant(const Plan& p, int i, int j, double gs);

	/**
	 * Make ground speed constant gs for entire plan.
	 * 
	 * @param p plan
	 * @param gs ground speed
	 * @return a new plan
	 */
	static Plan mkGsConstant(const Plan& p, double gs);

	/**
	 * Make ground speed constant gs between wp1 and wp2.
	 * 
	 * @param p a plan
	 * @return a new plan with constant ground speed
	 */
	static Plan mkGsConstant(const Plan& p);


	/**
	 * Make ground speed constant between wp1 and wp2 as an average of the total distance and time travelled.
	 * 
	 * @param p       trajectory
	 * @param wp1     starting waypoint
	 * @param wp2     starting waypoint
	 * @return        trajectory revised so that ground speed is constant on [wp1,wp2]
	 */
	static Plan mkGsConstant(const Plan& p, int wp1, int wp2) ;

	/**
	 * Make vertical speed constant vs between wp1 and wp2.
	 * Assumes plan does not contain any vertical speed accel regions
	 *
	 */
	static void mkVsConstant(Plan& p, int wp1, int wp2, double vs) ;

	/**
	 * Make vertical speed constant vs between wp1 and wp2, with vs being the average speed.
	 * @param p plan
	 * @param start starting index
	 * @param end ending index
	 */
	static void mkVsConstant(Plan& p, int start, int end) ;


	// change vertical profile: adjust altitudes
	/**
	 * Make vertical speed constant vs over plan with vs being the average speed.
	 * @param p plan
	 */
	static void mkVsConstant(Plan& p) ;

	/**
	 * Make vertical speed constant vs for full plan.
	 * @param p plan
	 * @param vs vertical speed
	 */
	static void mkVsConstant(Plan& p, double vs);

	/** Examine each vertical segment [i,i+1] over the specified range.  Calculate needed time for
	 *  the vertical accelerations at each end.  If the sum of half of each of these accels is longer
	 *  in time than the segment dt, then smooth away this segment vertically.  Either
	 *  side could be potentially smooth.  This method smooths the side with the smallest
	 *  delta vs.
	 *
	 * @param p            Plan to be modified
	 * @param start        starting index
	 * @param end          ending index
	 * @param vsAccel      vertical speed acceleration
	 * @param inhibitFixGs0   if true will NOT attempt to repair on segments with gsOut = 0
	 * @param inhibitFixVs0   if true will NOT attempt to repair level segments (i.e. vertical speed = 0)
	 * @param aggressiveFactor    a factor in the interval (0,1] that determines how aggressively the repair is done
	 *
	 * @return number of segments that were smoothed
	 */
	static int mkVsShortLegsContinuous(Plan& p, int start, int end, double vsAccel, bool allowFixGs0, double aggressiveFactor, bool inhibitFixVs0);

	static int mkVsShortLegsContinuous(Plan& p, double vsAccel, bool allowFixGs0, double aggressiveFactor, bool inhibitFixGs0);

	/** Repair Method: make the vertical speed continuous at index i (i.e. vsIn(i) = vsOut(i).  Accomplish this
	 *  by changing the altitude at index i.  If the new altitude makes the point redundant, then remove it.
	 * 
	 * @param p     trajectory
	 * @param i     index
	 * @return      revised trajectory
	 */
	static int mkVsContinuousAt(Plan& p, int i);

	/**
	 * Attempts to repair plan with infeasible vertical points by averaging vertical speeds over 2 segments.
	 * Assumes linear plan input.
	 * @param fp
	 * @param vsAccel
	 * @param minVsChangeRequired
	 * @return
	 */
	static Plan repairShortVsLegs(const Plan& fp, double vsAccel);

	/**
	 * Return the index of p1 that shares a segment (has same start and end positions) as p2, or -1 if there are none
	 * This returns the last such shared segment
	 * @param p1 point 1
	 * @param p2 point 2
	 * @return segment number
	 */
	static int shareSegment(const Plan& p1, const Plan& p2);

	static std::pair<NavPoint,TcpData> makeMidpoint(TcpData& tcp, const Position& p, double t);

	/**
	 * Project plan from latlonalt to Euclidean.
	 * Note: some tcp turn information may no longer be valid.
	 * @param p plan
	 * @param proj projection
	 * @return plan
	 */
	static Plan projectPlan(const Plan& p, const EuclideanProjection& proj);


	/**  Check that first and last points are almost the same.  Same means, same time,
	 *  same latitude, longitude, and altitude.
	 * 
	 * @param lpc a plan
	 * @param kpc another plan
	 * @param maxlastDt Maximum distance allowed at the last point
	 * @return True, if the first and last points are almost the same.
	 */
	static bool basicCheck(const Plan& lpc, const Plan& kpc, double maxlastDt);



	/**  Repair turns for semi-linear plan.  Removes vertex point if turn is infeasible.
	 * 
	 * @param lpc       Plan that needs repair
	 * @param startIx 	Starting index
	 * @param endIx 	Ending index
	 * @param default_bank_angle Default bank angle 
	 * @param repair    If true, attempt repair.  If false, only determine the number of bad turns remaining.
	 * @return          Number of repaired or infeasible vertices
	 * 
	 * Note: This assumes that there are no GS or VS TCPs in lpc and no existing turns in the repair range
	 */
	static int infeasibleTurns(Plan& lpc, int startIx, int endIx, double default_bank_angle, bool repair) ;

	static int fixBadTurns(Plan& lpc, int startIx, int endIx, double default_bank_angle);

	static int fixBadTurns(Plan& lpc, double default_bank_angle) ;

	static int countBadTurns(Plan& lpc, double default_bank_angle) ;


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
	static bool fixInfeasibleTurnAt(Plan& lpc, int ix, bool allowRepairByIntersection) ;

	/** Create an intersection point between ix-1 and ix
	 *
	 * @param lpc     Plan to be repaired
	 * @param ix      index of problem turn
	 * @return        true if plan was successfully repaired
	 */
	static bool fixInfeasibleTurnAtViaIntersection(Plan& lpc, int ix) ;

private:
	static int turnDir(Plan lpc, int i);

	/** Create intersection point between (i-1) and i
	 * 
	 * @param lpc linear plan
	 * @param i index
	 * @return point
	 */
	static NavPoint intersection(Plan lpc, int i);
public:

	static bool checkReversion(const Plan& kpc, const Plan& lpc, bool verbose);

	static bool checkReversion(const Plan& kpc, const Plan& lpc);

	/**  Test to make sure that all name and info in plan lpc is also in kpc
	 * 
	 * @param kpc kinematic plan
	 * @param lpc linear plan
	 * @param verbose true for verbose mode
	 * @return true if correct
	 */
	static bool checkNamesInfoRetained(const Plan& kpc, const Plan& lpc, bool verbose);

	static void createAndAddMOT(Plan& kpc, int ixBOT, int ixEOT);

	static bool isVelocityContAtTcps(const Plan& p, bool silent);

	static Plan repairSmallNegativeGS(const Plan& p);

	static Plan fixAccelConsistency(const Plan& p);

	/**
	 * Experimental
	 * Return the closest geometric point on a plan to a reference point, as measured as a Euclidean 3D norm from each linear segment.
	 * For latlon plans, perform linear interpolation on any legs longer then the given amount.
	 * (Kinematic turns are somewhat accounted for in this distance calculation, via linear interpolation.)
	 *
	 * Choosing a shorter maxLegLength can increase the accuracy of the closest point calculation, but it can significantly increase the
	 * computational expense of the operation.
	 *
	 * @param plan base plan
	 * @param p reference point
	 * @param maxLegLength linearly interpolate any latlon legs longer than this amount
	 * @return point
	 */
	static NavPoint closestPoint3D(const Plan& plan, const Position& p, double maxLegLength);


};

}
#endif

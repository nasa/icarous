/*
 * PlanUtil - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2017 United States Government as represented by
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

	static bool gsConsistent(const Plan& p, int ixBGS, double distEpsilon,	 bool silent);

	static bool vsConsistent(const Plan& p, int ixBVS,  double distEpsilon, double a, bool silent);

	static bool turnConsistent(int i, const NavPoint& BOT, const NavPoint& EOT, const Velocity& vin, double finalTrack, bool silent) ;

	static bool turnConsistent(const Plan& p, int i, double distH_Epsilon, bool silent);


	static bool isTrkContinuous(const Plan& p, int i, double trkEpsilon, bool silent) ;

	static bool isGsContinuous(const Plan& p, int i, double gsEpsilon, bool silent) ;

	static bool isVsContinuous(const Plan& p, int i, double velEpsilon, bool silent);

//	static bool isVelocityContinuous(const Plan& p, int i, double velEpsilon, bool silent);

	/** Checks to make sure that turnCenter, radius and location of BOT is consistent
	 * 
	 * @param p                plan
	 * @param i                index point
	 * @param distH_Epsilon    error bound
	 * @param silent
	 * @return
	 */
	static bool turnCenterConsistent(const Plan& p, int i, double distH_Epsilon, bool silent);


	/**  timeShift points in plan by "dt" starting at index "start"
	 *   Note: This will drop any points that become negative or that become out of order using a negative dt
	 *   See also Plan.timeShiftPlan for equivalent method
	 *
	 * @param p       plan
	 * @param start   starting index
	 * @param dt      delta time
	 * @return        time-shifted plan
	 */
	static Plan timeShift(const Plan& p, int start, double dt);

	static Plan makeSourceNew(const Plan& lpc);

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
	static Position advanceDistanceInSeg(const Plan& p, double curTm, double advDistance, bool linear);
public:
	static std::pair<Position,int> advanceDistance(const Plan& p, double currentTime, double advanceDist, bool linear);


	/** time required to cover distance "dist" if initial speed is "gsInit" and acceleration is "gsAccel"
	 * 
	 * @param gsAccel   ground speed acceleration
	 * @param gsInit    initial ground speed
	 * @param dist      distance travelled
	 * @return
	 */
	static double timeFromGs(double gsInit, double gsAccel, double dist);

	static Plan applyWindField(const Plan& pin, const Velocity& v);

	static bool checkMySolution(const Plan& solution, double currentTime, const Position& currentPos, const Velocity& currentVel);

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
	static void interpolateVirtuals(Plan& ac, double accuracy, double startTm, double endTm);

	static void interpolateVirtuals(Plan& ac, double accuracy);

	// this removes all virtuals AFTER time startTm and BEFORE endTm.
	// this will NOT remove any Fixed virtuals!
	// returns TRUE if all virtuals removed, otherwise FALSE
	static bool removeVirtualsRange(Plan& ac, double startTm, double endTm, bool all);


	static bool removeVirtuals(Plan& ac);


	/**
	 * Returns a Plan with TCPs removed between start and upto.  If it was a kinematic plan, this will attempt to regress the TCPs to 
	 * their original source points and times (if the proper metadata is available).  
	 * 
	 * Note.  No check is made to insure that start or upto is not in the middle of a TCP.
	 * Note.  See also Plan.revertGroupOfTCPs which uses sourcePosition but not sourceTimes.  It seeks to retain ground speeds.
	 * 
	 * Warning:  The reversion of points later in a plan without reverting earlier points can lead to strange ground speeds, because
	 *           the source times correspond to longer paths (without turns).
	 * 
	 */
	static Plan revertTCPs(const Plan& fp) ;


	/** 
	 * Revert the TCP pair in a plan structurally.  Properly relocate all points between TCP pair.
	 *
	 * Note.  No check is made to insure that start or upto is not in the middle of a TCP.
	 * 
	 * @param pln plan
	 * @param ix  index
	 */
	static void structRevertTCP(Plan& pln, int ix);

	static void structRevertTCPs(Plan& pln, bool removeRedPoints);

	/** Structurally revert all TCPS that create acceleration zones containing ix
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain 
	 *  original ground speed into the point after ix.  
	 *  
	 *  NOTE This method does not depend upon source time or source position
	 * 
	 * @param pln plan
	 * @param ix  The index of one of the TCPs created together that should be reverted
	 * @param killAllOthersInside
	 * @return index of the reverted point
	 */
	static int structRevertGroupOfTCPs(Plan& pln, int ix, bool killAllOthersInside);

	/** if "ix" is a BGS, then it reverts the BGS-EGS pair back to a single linear point
	 *  Note: It (does not depend upon source time or source position!)
	 *  
	 * @param pln                  plan
	 * @param ix                   index of BGS
	 * @param revertPreviousTurn   if true then if this GS segment is right after a turn then revert the turn as well  
	 * @return                     index of reverted point
	 */
	static int structRevertGsTCP(Plan& pln, int ix, bool revertPreviousTurn);

	/**
	 * change ground speed to newGs, starting at startIx 
	 * @param p
	 * @param newGs
	 * @param startIx
	 * @return a new Plan
	 */
	static Plan makeGsConstant(const Plan& p, double newGs, int startIx);




	/**  make a new plan that is identical to plan from startTime to endTime 
	 *   It assumes that  startTime and endTime are in linear segments
	 * 
	 * @param plan         source plan
	 * @param startTime    absolute time of start time
	 * @param endTime      absolute time of end time

	 * @return   truncated plan
	 */
	static Plan cutDown(const Plan& plan, double startTime, double endTime);


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
	 * @return
	 */
	static Plan cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold, double tExtend);

	static Plan cutDownTo(const Plan& plan, double timeOfCurrentPosition, double intentThreshold);


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
	/**
	 * Cut a Plan down to contain "numTCPs" future TCPS past the current time (i.e. timeOfCurrentPosition). If tExtend
	 * is greater than 0, create an extra leg after the last TCP with a duration of "tExtend".  This is intended
	 * to mimic having only state information after the last TCP.  Note that numTCPs should be interpreted as number of
	 * acceleration zones.  That is  [BOT,EOT] counts as one TCP.  Similarly [BGS, EGS] is one TCP.
	 * 
	 * This method eliminates waypoints earlier than timeOfCurrentPosition as much as possible.  If the aircraft
	 * is in an acceleration zone at timeOfCurrentPosition, then it retains the plan back to the last begin TCP.
	 * 
	 * @param numTCPs  maximum number of TCPs to allow in the future, see note above.
	 * @param timeOfCurrentPosition  indicates current location of aircraft, if negative, then aircraft is at point 0.
	 * @param tExtend  amount of additional time to extend the plan after last TCP end point.
	 * 
	 * NOTE: THIS CODE WILL NOT WORK WITH OVERLAPPING HORIZONTAL/VERTICAL Accel Zones
	 * 
	 * @return
	 */
//	static Plan cutDownToByCount(const Plan& plan, int numTCPs, double timeOfCurrentPosition, double tExtend);

	static std::pair<bool,double> enoughDistanceForAccel(const Plan& p, int ix, double maxAccel, double M);

	/** Fix Plan p at ix if there is not enough distance for the current speed and specified acceleration (maxAccel)
	 *  It makes the new ground speed as close to the original as possible (that is achievable over the distance)
	 * 	
	 * @param p         plan 
	 * @param ix        index of ground speed change 
	 * @param maxAccel  maximum ground speed acceleration
	 * @param checkTCP  if true, do not alter time if point is an EOT or EVS
	 * @return -1 if no change was necessary, otherwise return the new time at ix+1
	 */
	static void fixGsAccelAt(Plan& p, int ix, double maxAccel, bool checkTCP, double M);

	static int hasPointsTooClose(const Plan& plan) ;

	static double diffMetric(const Plan& lpc, const Plan& kpc);

	// will not remove segments that are longer than maxLegSize
	static Plan unZigZag(const Plan& pp);

	// will not remove segments that are longer than maxLegSize
	static Plan unZigZag(const Plan& pp, double maxLegSize) ;

	static bool aboutTheSameTrk(const Velocity& v1, const Velocity& v2, double sameTrackBound);

	static Plan removeCollinearTrk(const Plan& pp, double sameTrackBound);

//	/**
//	 *
//	 * Returns a new Plan that sets all points in a range to have a constant GS.
//	 * THIS ASSUMES NO VERTICAL TCPS.
//	 *
//	 * NOTE.  First remove Vertical TCPS then re-generate Vertical TCPs
//	 * */
//	static Plan makeGSConstant_NO_Verts(const Plan& p, double newGs);
//
//	/**
//	 * Returns a new Plan that sets all points in a range to have a constant GS.
//	 * THIS ASSUMES NO VERTICAL TCPS, but allows turns (turn omega metatdata may be altered -- maximum omega values are not tested for).
//	 * The return plan type will be the same as the input plan type.
//	 * The new gs is an average over the whole plan.  End time should be preserved.
//	 * This will set an error status in the return plan if there are any vertical TCPs.
//	 */
//	static Plan makeGSConstant_No_Verts(const Plan& p) ;

	// This methods assumes plan is linear
	/**
	 * Make a new plan with constant ground speed from wp1 to wp2. 
	 * Assumes input plan is linear.
	 * 
	 *       200     200     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    linearMakeGSConstant(p, 1, 3, 500)
	 *    
	 *    	 200     500     500     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    Note that if wp1 == wp2 no change is made.
	 *    
	 * 
	 * @param p
	 * @param wp1
	 * @param wp2
	 * @param gs
	 * @return a new plan
	 */
	static Plan linearMakeGSConstant(const Plan& p, int wp1, int wp2, double gs);

	/**
	 * Make ground speed constant gs for entire plan.
	 * Assumes input plan is linear.
	 * 
	 * @param p plan
	 * @param gs ground speed
	 * @return a new plan
	 */
	static Plan linearMakeGSConstant(const Plan& p, double gs);

	/**
	 * Make ground speed constant gs between wp1 and wp2.
	 * Assumes input plan is linear.
	 * 
	 * @param p a plan
	 * @return a new plan with constant ground speed
	 */
	static Plan linearMakeGSConstant(const Plan& p);


	/**
	 * Make ground speed constant between wp1 and wp2 as an average of the total distance and time travelled.
	 * Assumes input plan is linear.
	 * 
	 * @param p   
	 * @param wp1
	 * @param wp2
	 * @return
	 */
	static Plan linearMakeGSConstant(const Plan& p, int wp1, int wp2) ;


	// change vertical profile: adjust altitudes
	/**
	 * Make vertical speed constant vs between wp1 and wp2.
	 * Assumes input plan is linear.
	 */
	static void linearMakeVsConstant(Plan& p, int wp1, int wp2, double vs) ;

	// change vertical profile: adjust altitudes
	/**
	 * Make vertical speed constant vs between wp1 and wp2, with vs being the average speed.
	 * Assumes input plan is linear.
	 */
	static void linearMakeVsConstant(Plan& p, int start, int end) ;


	// change vertical profile: adjust altitudes
	/**
	 * Make vertical speed constant vs over plan with vs being the average speed.
	 * Assumes input plan is linear.
	 */
	static void linearMakeVsConstant(Plan& p) ;

	/**
	 * Make vertical speed constant vs for full plan.
	 * Assumes input plan is linear.
	 */
	static void linearMakeVsConstant(Plan& p, double vs);

	static void linearMakeGsInto(Plan& p, int ix, double gs);




};

}
#endif

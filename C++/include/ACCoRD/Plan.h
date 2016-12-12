
/*
 * Plan - the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Authors: George Hagen,  Jeff Maddalon,  Rick Butler
 *
 */

#ifndef Plan_H
#define Plan_H

#include "Units.h"
#include "ErrorLog.h"
#include "NavPoint.h"
#include "Position.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "BoundingRectangle.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace larcfm {

const int maxWpts = 1000;              //  Maximum number of points

/**
 * Manages a flight Plan as a sequence of 4D vectors (three dimensions
 * of space and time). <p>
 *
 * This class assumes that (1) the points are added in increasing
 * time, (2) all point times are 0 or positive, (3) there are no
 * duplicate point times.<p>
 * 
 * Points that "overlap" with existing points are deemed as redundant
 * and will not be included in the Plan.  Two points overlap if they
 * are within both a minimum distance and time of each other (as
 * defined by minimumDtTime). <p>
 * 
 * Furthermore this version of the Plan assumes that any points
 * between a beginning and paired end TCP will have constant
 * acceleration (or turn radius).  These values will be derived from
 * the beginning and end points.  It is not recommended that these
 * Plans be directly modified.  Rather the (linear) Plans they are
 * based on should be modified and new kinematic Plans be generated
 * from those. <p>
 * 
 * Currently (as of v 2.0.0), it is assumed that kinematic versions of
 * Plans will be constructed algorithmically.  One potential way to do
 * this is using the Util.TrajGen.makeKinematicPlan() method to
 * construct a kinematic Plan core from a linear one (and use
 * Util.TrajGen.makeLinearPlan to do the inverse). <p>
 *
 * Acceleration points are designated as Beginning Of Turn, Middle Of
 * Turn, End of Turn, Ground Speed Change Begin, Ground Speed Change
 * End, Vertical Speed Change Begin, and Vertical Speed Change End.
 * In general, the end points are not considered to be part of the
 * maneuver.  All horizontal acceleration regions must be distinct and
 * not overlap (there must be a non-accelerating segment of some
 * length between them).  Similarly all vertical acceleration regions
 * must not overlap (there must be a non-accelerating segment of some
 * length between them).  Vertical and horizontal regions, however,
 * may overlap. Acceleration start TCP points will contain meta data
 * including acceleration and initial velocity.  For other points,
 * velocities are inferred from surrounding points. <p>
 * 
 * There can also be points marked with a "minor velocity change"
 * flag.  These points can indicate the known existence of a smaller
 * velocity change than can be expressed by non-overlapping TCP pairs,
 * and are primarily used for consistency checks. <p>
 * 
 * More so than linear Plan cores, Plans accommodate the concept of an
 * "existing velocity" at the first point.  If there is a valid
 * existing velocity value, then that is assumed to be the initial
 * velocity at the first point -- this is used, for example, when
 * building a Plan that starts from the "current " that starts from a
 * velocity of zero (or some other non-inferred value), allowing an
 * acceleration on the first Plan segment. <p>
 * 
 * Typical Usage: <ol>
 *     <li> create a Plan from a linear Plan using TrajectoryGen.makePlan( ... )
 *     <li> compute position as a function of time using Plan.position(t)
 *</ol>
 */
class Plan : public ErrorReporter {

protected:
	typedef std::vector<NavPoint> navPointVector;
	typedef navPointVector::iterator navPointIterator;

	friend class PlanCollection;        // Plans needs to access some of these
	std::string  name;
	navPointVector points;
	mutable ErrorLog error;
	mutable int errorLocation;
	std::string  note;
	static bool debug;
	BoundingRectangle bound;



public:
	static const double minDt;  // points that are closer together in time than this are treated as the same point

	//static const double revertGsTurnConnectionTime;

	/** Create an empty Plan */
	Plan();

	/** Create an empty Plan with the given id */
	Plan(const std::string& name);

	/** Create an empty Plan with the given id */
	Plan(const std::string& name, const std::string& note);


	/** Construct a new object that is a deep copy of the supplied object */
	Plan(const Plan& fp);

	~Plan();

	static std::string specPre();

protected:
	void init();


public:

	bool isLinear() const;

	/**
	 * Tests if one plan object has the same contents as another.
	 * This test also compares the concrete classes of the two Plans.
	 */
	bool equals(const Plan& fp);

	bool almostEquals(const Plan& p) const;


	/** Tests if one plan object has the same contents as another. */
	bool operator == (const Plan& o) const;
	/** Inequality */
	bool operator != (const Plan& o) const;

	void setDebug(bool d);

	void static setStaticDebug(bool d);

	/** Clear a plan of its contents.  */
	void clear();


	/**
	 * Clear all virtual points in a plan.
	 */
	void clearVirtuals();

	BoundingRectangle getBound() const;

	/** If size() == 0 then this is a "null" plan.  */
	int size() const;

	/** Are points specified in Latitude and Longitude */
	bool isLatLon() const;

	/** Is the geometry of this Plan the same as the given parameter.  Note if
	 * this Plan has no points, then it always agrees with the parameter */
	bool isLatLon(bool latlon) const;

	/**
	 * Return the name of this plan (probably the aircraft name).
	 */
	const std::string& getName() const;

	/**
	 * Set the name of this plan (probably the aircraft name).
	 */
	void setName(const std::string& s);

	/**
	 * Return the note of this plan.
	 */
	const std::string& getNote() const;

	/**
	 * Set the note of this plan (probably the aircraft name).
	 */
	void setNote(const std::string& s);


	/**
	 * Return the time of the first point in the plan.
	 */
	double getFirstTime() const;

	/**
	 * This returns the time for the first non-virtual point in the plan.
	 * Usually this is the same as getFirstTime(), and this should only be called
	 * if there is a known special handling of virtual points, as it is less efficient.
	 * If there are 0 non-virtual points in the plan, this logs an error and returns 0.0
	 */
	double getFirstRealTime() const;

	/**
	 * Return the time of the last point in the plan.
	 */
	double getLastTime() const;

	/**
	 * Return the last point in the plan.
	 */

	NavPoint getLastPoint() const;

	/**
	 * Returns the index of the next point which is
	 * <b>after</b> startWp.  If the startWp is greater or equal to
	 * the last wp, then the index of the last point is returned.
	 * Note: if the plan is empty, this
	 * returns a -1<p>
	 *
	 */
	int nextPtOrEnd(int startWp) const;

	/**
	 * Return the index of the point that matches the provided
	 * time.  If the result is negative, then the given time
	 * corresponds to no point.  A negative result gives
	 * information about where the given time does enter the list.  If
	 * the (negative) result is i, then the given time corresponds to
	 * a time between indexes -i-2 and -i-1.
	 * For example, if times are {0.0, 1.0, 2.0, 3.0, 4.0}:
	 * <ul>
	 * <li> getIndex(-3.0) == -1 -- before index 0
	 * <li> getIndex(1.0) == 1  -- at index 1
	 * <li> getIndex(1.5) == -3 -- between index 1 (-i-2) and 2 (-i-1)
	 * <li> getIndex(3.4) == -5 -- between index 3 (-i-2) and 4 (-i-1)
	 * <li> getIndex(16.0) == -6 -- after index 4 (-i-2) 
	 * </ul>
	 * 
	 * @param tm a time
	 * @return the index of the time, or negative if not found.
	 */
	int getIndex(double tm) const;

	/**
	 * Return the segment number that contains 'time' in [s].  If the
	 * time is not contained in the flight plan then -1 is returned.  If
	 * the time for point 0 is 10.0 and the time for point 1 is 20.0, then
	 * getSegment(10.0) will produce 0, getSegment(15.0) will produce 0,
	 * and getSegment(20.0) will produce 1.
	 */
	int getSegment(double tm) const;

	int getSegmentByDistance(double d) const;

	/**
	 * Return the index of the point nearest to the provided time.
	 * in the event two points are equidistant from the time, the earlier one
	 * is returned.  If the plan is empty, -1 will be returned.
	 */
	int getNearestIndex(double tm) const;

	/**
	 * Given an index i, returns the corresponding point's time in seconds.
	 * If the index is outside the range of points, then an error is set.
	 */
	double getTime(int i) const;

	/**
	 * Return the NavPoint at the given index (segment number)
	 * @param i the segment number
	 * @return the NavPoint of this index
	 */
	const NavPoint point(int i) const;

	/** 
	 * Adds a point to the plan.  
	 * If the new point has negative time or the same time as an existing 
	 * non-Virtual point, it will generate an error.<p>
	 *   
	 * @param p the point to add
	 * @return the (non-negative) index of the point if the point
	 * is added without problems, otherwise it will return a negative value.
	 */
	int add(const NavPoint& p);

	/** Remove the i-th point in this plan. (note: This does not store the fact a point was deleted.) */
	void remove(int i);


private:

	// Insert a point at position i
	// Warning: i must be a valid index: 0..numPts-1
	/**
	 * Adds a point at index i.  This may not preserve time ordering!
	 * This may result in inconsistencies between implied and stored ground and vertical speeds for TCP points.
	 * @param i
	 * @param v
	 */
	void insert(int i, const NavPoint& v);

	int indexSearch(double tm, int i1, int i2) const;

//	std::pair<Position,Velocity> accelZone(const Velocity& v, double t1, double t2) const;

	/** This function computes the velocity out at point i in a strictly linear manner.  If i is an
	 *  inner point it uses the next point to construct the tangent.  if the next point is less than
	 *  dt ahead in time, then it finds the next point that is at least minDt ahead in time.
	 *  
	 *  If it is called with the last point in the plan it looks backward for a point.   In this case
	 *  it uses final velocity on the previous leg.
	 * 
	 * @param i
	 * @return initial velocity at point i
	 */
	Velocity linearVelocityOut(int i) const;

//	Velocity finalLinearVelocity(int i) const;

public:

	/**  **UNUSED**
	 * Inserts point at index, then timeshifts itself and all subsequent points, if necessary.
	 * This may result in inconsistencies between implied and stored velocities for TCP points to either side of the inserted point (as well as itself).
	 * If the inserted point in is an acceleration zone and not on the current plan, this may result in other inconsistencies.
	 * @param i
	 * @param v
	 */
	void insertWithTimeshift(int i, const NavPoint& v);

	void setAlt(int i, double alt);

	void setTime(int i, double t);

	/**
	 * Creates a copy with all points in the plan (starting at index start) time shifted by the provided amount st.
	 * This will drop any points that become negative or that become out of order.
	 * Note this also shifts the start point.
	 * This version will also shift the "source times" associated with the points
	 */
	Plan copyAndTimeShift(int start, double st);

	/**
	 * Temporally shift all points in the plan (starting at index start) by the provided amount st.
	 * This will drop any points that become negative or that become out of order. 
	 * Note this also shifts the start point as well as any points marked as time-fixed.
	 */
//	bool timeshiftPlan(int start, double st, bool preserveRTAs);
	bool timeshiftPlan(int start, double st);

	/**
	 * This finds the last index where the TCP type is BOT or EOT. If BOT or EOT
	 * are never found, then return -1.
	 * 
	 * @param current the index of the point to begin the search
	 * @return index before <i>current</i> which has a TCP type of BOT or EOT
	 */
	int prevTrkTCP(int current) const;
	/**
	 * This finds the last previous index where the TCP is a speed change type
	 */
	int prevGsTCP(int current) const;
	/**
	 * This finds the last previous index where the TCP is of type tcp1 or tcp2
	 */
	int prevVsTCP(int current) const;

	/**
	 * This finds the last index where the TCP type is BOT or EOT. If BOT or EOT
	 * are never found, then return -1.
	 * 
	 * @param current the index of the point to begin the search
	 * @return index after <i>current</i> which has a TCP type of BOT or EOT
	 */
	int nextTrkTCP(int current) const;
	/**
	 * This finds the next Gs TCP
	 */
	int nextGsTCP(int current) const;
	/**
	 * This finds the last previous index where the TCP is of type tcp1 or tcp2
	 */
	int nextVsTCP(int current) const;


	/**
	 * This returns the index of the Beginning of Turn (BOT) point that is less than or equal to the given 
	 * index, or -1 if there is no such point.
	 * This is generally intended to be used to find the beginning of an acceleration zone.
	 */
	int prevBOT(int current) const;
	int prevEOT(int current) const;

	/**
	 * This returns the index of the End of Turn point >= the given index, or -1 if there is no such point.
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	int nextEOT(int current) const;
	int nextBOT(int current) const;

//	/**
//	 * This returns the index of the Ground Speed Change Begin point <= the given index, or -1 if there is no such point.
//	 * This is generally intended to be used to find the beginning of an acceleration zone.
//	 */
//	int prev_BGS(double t) const ;

	int prevBGS(int current) const;
	int prevEGS(int current) const;

	/**
	 * This returns the index of the Ground Speed Change End point greater than or equal to the given index, or -1 if there is no such point.
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	int nextEGS(int current) const;
	int nextBGS(int current) const;

	/**
	 * This returns the index of the Vertical Speed Change Begin point <= the given index, or -1 if there is no such point.
	 * This is generally intended to be used to find the beginning of an acceleration zone.
	 */
	int prevBVS(int current) const;
	int prevEVS(int current) const;

	/**
	 * This returns the index of the Vertical Speed Change End point that is greater than or equal the given 
	 * index, or -1 if there is no such point.
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	int nextEVS(int current) const;
	int nextBVS(int current) const;

	/**
	 * This finds the last previous index where the TCP is of type tcp1 or tcp2
	 */
	int prevTCP(int current) const;

	/** This returns true if the given time is >= a BOT but before an EOT point */
	bool inTrkChange(double t) const;
//	/** This returns true if the given time is >= a BOT but before an EOT point */
	//bool inTrkChange(int i) const;

	/** This returns true if the given time is >= a GSCBegin but before a GSCEnd point */
	bool inGsChange(double t) const;
	//bool inGsChange(int i) const;


	/** This returns true if the given time is >= a VSCBegin but before a VSCEnd point */
	bool inVsChange(double t) const;
	//bool inVsChange(int i) const;

	bool inTrkChangeByDistance(double d) const;
	bool inGsChangeByDistance(double d) const;
	bool inVsChangeByDistance(double d) const;



	/**
	 * If time t is in a turn, this returns the radius, otherwise returns a negative value.
	 */
	double turnRadiusAtTime(double t) const;

	/**
	 * Return the turn rate (i.e., position acceleration in the "track" dimension) 
	 * associated with the point at time t.
	 */
	double trkAccelAtTime(double t) const;

	/**
	 * Return the ground speed rate of change (i.e., position acceleration in the 
	 * "ground speed" dimension) associated with the point at time t.
	 */
	double gsAccelAtTime(double t) const;

	/**
	 * Return the vertical speed rate of change (i.e., acceleration in the vertical dimension)
	 * associated with the point at time t.
	 */
	double vsAccelAtTime(double t) const;



	/**
	 * Return a linear interpolation of the position at the given time.  If the
	 * time is beyond the end of the plan and getExtend() is true, then the
	 * position is extrapolated after the end of the plan.  An error is
	 * set if the time is before the beginning of the plan.
	 *
	 * @param tt time
	 * @return linear interpolated position at time tt
	 */
	Position position(double t) const;

	Position position(double t, bool linear) const;


//	Position positionOLD(double t) const;
//
//	Position positionOLD(double t, bool linear) const;


	/** 
	 * Estimate the initial velocity at the given time for this aircraft.   
	 * A time before the beginning of the plan returns a zero velocity.
	 */
	Velocity velocity(double tm) const;

	/** 
	 * Estimate the initial velocity at the given time for this aircraft.   
	 * A time before the beginning of the plan returns a zero velocity.
	 */
	Velocity velocity(double tm, bool linear) const;

//	Velocity velocityOLD(double tm) const;
//
//	Velocity velocityOLD(double tm, bool linear) const;


	/** EXPERIMENTAL
	 * time required to cover distance "dist" if initial speed is "vo" and acceleration is "gsAccel"
	 *
	 * @param gsAccel
	 * @param vo
	 * @param dist
	 * @return
	 */
	static double timeFromGs(double vo, double gsAccel, double dist);

	// experimental - map path distance (in this segment) to relative time (in this segment)
	// if there is a gs0=0 segment, return the time of the start of the segment
	// return -1 on out of bounds input
	double timeFromDistance(int seg, double rdist) const;

	// experimental -- map total path distance to absolute time
	// return -1 on out of bounds input
	double timeFromDistance(double dist) const;


	/**
	 * Estimate the velocity between point i to point
	 * i+1 for this aircraft.   This is not defined for the last point of the plan.
	 */
	Velocity averageVelocity(int i) const;

	Velocity initialVelocity(int i) const;

	Velocity initialVelocity(int i, bool linear) const;

	//    Velocity averageVelocity(double tm) const;

	// estimate the velocity from point i to point i+1 (at point i+1).  This is not defined for the last point of the plan.
	Velocity finalVelocity(int i) const;

	Velocity finalVelocity(int i, bool linear) const;

	Velocity dtFinalVelocity(int i, bool linear) const;


	double trkOut(int seg, bool linear) const;

	double trkOut(int seg) const;

	double trkFinal(int seg, bool linear) const;

	/**  ground speed out of point "seg"
	 * 
	 * @param seg         The index of the point of interest
	 * @param linear      If true, then interpret plan in a linear manner
	 * @return
	 */
	double gsOut(int i, bool linear) const;

	/**  ground speed at the end of segment "i"
	 * 
	 *   Note.  if there is no acceleration, this will be the same as gsOut
	 * 
	 * @param i           The index of the point of interest
	 * @param linear      If true, then interpret plan in a linear manner
	 * @return
	 */
	double gsFinal(int i, bool linear) const;

	/**  ground speed into point "seg"
	 * 
	 * @param i           The index of the point of interest
	 * @param linear      If true, then interpret plan in a linear manner
	 * @return
	 */
	double gsIn(int seg, bool linear) const;


	double gsOut(int seg) const;

	double gsFinal(int seg) const;

	double gsIn(int seg) const;


//	double gsAtSeg(int seg, bool linear) const;

	/**  ground speed at time t (which must be in segment "seg")
	 *  
	 * @param seg          segment where time "t" is located
	 * @param gsAtSeg      ground speed out of segment "seg"
	 * @param t            time of interest
	 * @param linear       If true, then interpret plan in a linear manner
	 * @return
	 */
	double gsAtTime(int seg, double gsAtSeg, double t, bool linear) const;

	/**  ground speed at time t
	 *  
	 * @param t            time of interest
	 * @param linear       If true, then interpret plan in a linear manner
	 * @return
	 */
	double gsAtTime(double t, bool linear) const;


	/**  vertical speed out of point "i"
	 * 
	 * @param i         The index of the point of interest
	 * @param linear      If true, then interpret plan in a linear manner
	 * @return
	 */
	double vsOut(int i, bool linear) const;

	/**  vertical speed at the end of segment "i"
	 * 
	 *   Note.  if there is no acceleration, this will be the same as vsOut
	 * 
	 * @param i          The index of the point of interest
	 * @param linear      If true, then interpret plan in a linear manner
	 * @return
	 */
	double vsFinal(int i, bool linear) const;

	/** vertical speed into point "seg"
	 * 
	 * @param seg         The index of the point of interest
	 * @param linear      If true, then interpret plan in a linear manner
	 * @return
	 */
	double vsIn(int seg, bool linear) const;


	double vsOut(int seg) const;

	double vsFinal(int seg) const;

	double vsIn(int seg) const;


//	double vsAtSeg(int seg, bool linear) const;

	/** vertical speed at time t (which must be in segment "seg")
	 *  
	 * @param seg          segment where time "t" is located
	 * @param vsAtSeg      vertical speed out of segment "seg"
	 * @param t            time of interest
	 * @param linear       If true, then interpret plan in a linear manner
	 * @return
	 */
	double vsAtTime(int seg, double vsAtSeg, double t, bool linear) const;

	/** vertical speed at time t
	 *  
	 * @param t            time of interest
	 * @param linear       If true, then interpret plan in a linear manner
	 * @return
	 */
	double vsAtTime(double t, bool linear) const;

	double vsAtTime(double t) const;

	/** Calculate the distance from the Navpoint at "seq" to plan position at time "t"
	 * 
	 * @param seg     starting position
	 * @param t       time of stopping position
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return
	 */
	double distFromPointToTime(int seg, double t, bool linear) const;

	std::pair<Position,Velocity> positionVelocity(double t, bool linear) const;

	std::pair<Position,Velocity> positionVelocity(double t) const;


	double calcVertAccel(int i) ;

	double calcGsAccel(int i);

	/** calculate time at a waypoint such that the ground speed into that waypoint is "gs".  
	 * If i or gs is invalid, this returns -1. If i is in a turn, this returns the current point time. 
	 * 
	 * Note: parameter maxGsAccel is not used on a linear segment
	 */
	double calcTimeGSin(int i, double gs) const;

	double linearCalcTimeGSin(int i, double gs) const;

	/** set the time at a waypoint such that the ground speed into that waypoint is "gs", 
	 *  given the ground speed accelerations (possibly ignored);
	 *  
	 *  Note:  This does not leave speeds after this point unchanged
	 */
	void setTimeGSin(int i, double gs);

	void mkGsInto(int ix, double gs);

	void mkGsOut(int ix, double gs);


	bool isVelocityContinuous() const;
	/**
	 * Find the cumulative horizontal (curved) path distance for whole plan.
	 */
	double pathDistance() const;

	/** 
	 * Find the horizontal (curved) distance between points i and i+1 [meters]. 
	 * 
	 * @param i index of starting point
	 * @return path distance (horizontal only)
	 */
	double pathDistance(int i) const;

	/** 
	 * Find the horizontal distance between points i and i+1 [meters]. 
	 * 
	 * @param i index of starting point
	 * @param linear if true, measure the straight distance, if false measure the curved distance 
	 * @return path distance (horizontal only)
	 */
	double pathDistance(int i, bool linear) const;

	/**
	 * Find the cumulative horizontal (curved) path distance between points i and j [meters].
	 */
	double pathDistance(int i, int j) const;

	/** 
	 * Find the cumulative horizontal path distance between points i and j [meters].   
	 * 
	 * @param i beginning index
	 * @param j ending index
	 * @param linear if true, then TCP turns are ignored. Otherwise, the length of the circular turns are calculated.
	 * @return cumulative path distance (horizontal only)
	 */
	double pathDistance(int i, int j, bool linear) const;


	/** return the path distance from the location at time t until the next waypoint
	 * 
	 * @param t  current time of interest   
	 * @return   path distance
	 */
	double partialPathDistance(double t, bool linear) const;


	/** calculate path distance from the current position at time t to point j
	 *
	 * @param t     current time
	 * @param j     next point
	 * @return      path distance
	 */
	double pathDistanceFromTime(double t, int j) const;

	/** calculates vertical distance from point i to point i+1
	 * 
	 * @param i   point of interest
	 * @return    vertical distance
	 */
	double vertDistance(int i) const;

	/** 
	 * Find the cumulative vertical distance between points i and j [meters].
	 */
	double vertDistance(int i, int j) const;

	double averageGroundSpeed() const;

	// calculate vertical speed from point i to point i+1 (at point i).
	/** 
	 * Calculate vertical speed from point i to point i+1 (at point i).
	 *   
	 * @param i index of the point
	 * @return vertical speed
	 */
	double verticalSpeed(int i) const ;

	// assumes ix > 0 AND ix < pln.size()
	/** Structurally revert TCP at ix: (does not depend upon source time!!)
	 *  This private method assumes ix > 0 AND ix < pln.size().  If ix is not a BOT, then nothing is done
	 *
	 * @param pln
	 * @param ix
	 * @param addBackMidPoints if addBackMidPoints = true, then if there are extra points between the BOT and EOT, make sure they are moved to
	                            the correct place in the new linear sections.  Do this by distance not time.
	 */
	void structRevertTurnTCP(int ix, bool addBackMidPoints, bool killNextGsTCPs, double zVertex);


	void structRevertGsTCP(int ix);

	double structRevertVsTCP(int ix);

	/** revert all TCPS back to its original linear point which have the same sourceTime as the point at index dSeg
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points to
	 *  regain the original ground speed into the first reverted point of the group and all points after it.
	 *  If checkSource is true, this function checks to make sure that the source position is reasonably close to the 
	 *  current position,  and if not, it reverts to the current position of "dSeg".
	 *  
	 *  Note.  This method restores the sourcePosition, but essentially ignores the sourceTimes
	 * 
	 * @param dSeg  The index of one of the TCPs created together that should be reverted
	 * @return index of the point that replaces all the other points
	 */
	int revertGroupOfTCPs(int dSeg, bool checkSource);

	void removeRedundantPoints(int from, int to);

	void removeRedundantPoints() ;


	/**
	 * This returns true if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and end point.
	 */
	bool isWellFormed() const  ;


	int indexWellFormed() const;

	/**
	 * This returns a string representing which part of the plan is not 
	 * "well formed", i.e. all acceleration zones have a matching beginning and end point.
	 */
	std::string strWellFormed() const;

	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instanteous "jumps," it is not consistent.
	 */
	bool isConsistent() const ;

	bool isConsistent(bool silent) const ;

	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instanteous "jumps," it is not consistent.
	 */
	bool isWeakConsistent(bool silent, bool useProjection) const ;

	bool isWeakConsistent() const ;
	/**
	 * This removes the acceleration tags on points that appear "unbalanced."
	 * This is not particularly intelligent and may result in bizarre (but legal) plans.
	 */
	void fix();

	/** experimental -- only used in Watch
	 * find the closest point on the given segment of the  current plan to position p, only considering horizontal distance. 
	 */
	NavPoint closestPointHoriz(int seg, const Position& p);

	/**
	 * Experimental
	 * This returns a NavPoint on the plan that is closest to the given position.
	 * If more than one point are closest horizontally, the closer vertically is returned.
	 * If more than one have the same horizontal and vertical distances, the first is returned.
	 * @param p
	 * @return
	 */
	NavPoint closestPoint(const Position& p);

	NavPoint closestPointHoriz(const Position& p);

	/**
	 * Experimental
	 * This returns a NavPoint on the plan within the given segment range that is closest to the given position.
	 * If more than one point are closest horizontally, the closer vertically is returned.
	 * If more than one have the same horizontal and vertical distances, the first is returned.
	 * If start >= end, this returns an INVALID point
	 * @param start start point
	 * @param end end point
	 * @param p position to check against
	 * @param horizOnly if true, only consider horizontal distances, if false, also compare vertical distances if the closest points on 2 segments are within maxHdist of each other 
	 * @param maxHdist only used if horizOnly is false: compare vertical distances if candidate points are within this range of each other
	 * @return
	 */
	NavPoint closestPoint(int start, int end, const Position& p, bool horizOnly, double maxHdist);

	/** Is the aircraft accelerating (either horizontally or vertically) at this time? 
	 * @param t time to check for acceleration
	 * @return true if accelerating
	 */
	bool inAccel(double t) const;

	/** true if this point is in a closed interval [BEGIN_TCP , END_TCP]
	 *
	 *  NOTE:  inAccel(t) returns false if the point at time "t" is an end TCP.  This method return true!
	 *
	 * */
	bool inAccelZone(int ix) const;

	/** Remove a range of points i to j, inclusive, from a plan.  (note: This does not store the fact a point was deleted.) */
	void remove(int i, int k);

	/**
	 * Attempt to replace the i-th point with the given NavPoint.
	 * If successful, this removes the point currently at index i and adds the new point.
	 * This returns an error if the given index is out of bounds or a warning if the new point overlaps
	 * with (and replaces) a different existing point.
	 *
	 * @param i the index of the point to be replaced
	 * @param v the new point to replace it with
	 * @return the actual index of the new point
	 */
	int set(int i, const NavPoint& v);


public:


	/**
	 * Create a (simple) new Plan by projecting state information.
	 * @param id Name of aircraft
	 * @param pos Initial position of aircraft
	 * @param v Initial velocity of aircraft (if pos in in lat/lon, then this assumes a great circle initial heading)
	 * @param startTime Time of initial state
	 * @param endTime Final time when projection ends
	 * @return new Plan, with a Fixed starting point.
	 * If endTime <= startTime, returns an empty Plan.
	 */
	static Plan planFromState(const std::string& id, const Position& pos, const Velocity& v, double startTime, double endTime);

	Plan copyWithIndex() const;

	/** Create new plan from existing using points from index "firstIx" to index "lastIx"
	 * 
	 * @param firstIx    first index
	 * @param lastIx     last index
	 * @return
	 */
	Plan cut(int firstIx, int lastIx) const;

	void mergeClosePoints(double minDt);

	/** String representation of the entire plan */
	std::string toString() const;

//	/** String representation of a single point */
//	std::string toString(int j) const;


	/** Returns string that of header information that is compatible with the file format with header and consistent with a call to toOutput(bool, int, int).
	 * This does not include a terminating newline.
	 */
	std::string getOutputHeader(bool tcpcolumns) const;

	/**
	 * Return a minimal (6 field) string compatable with the reader format.  This works well for linear plans.
	 * Values use the default precision.
	 * Point metadata, if present, is collapsed into the label field.
	 * Virtual points are not included
	 * @return
	 */
	std::string toOutputMin() const;

	/**
	 * Return a nominal (20-odd field) string compatable with the reader format.
	 * Values use the default precision.
	 * Point metadata is printed in distinct columns.
	 * Virtual points are not included
	 * @return
	 */
	std::string toOutput() const;

	/**
	 * Output plan data in a manner consistent with the PlanReader input files
	 * @param extraspace append this number of extra blank columns to the data (used for polygons)
	 * @param precision precision for numeric data 
	 * @param tcpColumns if true, include metadata as distinct columns, if false, collapse relevant metadata into the label column
	 * @param includeVirtuals if true, include points marked as "Virtual" (these are generally temporary or redundant points and intended to be discarded)
	 * @return
	 */
	std::string toOutput(int extraspace, int precision, bool tcpColumns, bool includeVirtuals) const;


	//std::string toStringPlanType(Plan::PlanType ty) const;


	// ErrorReporter Interface Methods

	void addWarning(std::string s) const {
		error.addWarning("("+name+") "+s);
	}

	void addError(std::string s) const {
		addError(s,0);
	}

	void addError(std::string s, int loc) const {
		error.addError("("+name+") "+s);
		if (errorLocation < 0 || loc < errorLocation) errorLocation = loc; // save the most recent error location
	}

	int getErrorLocation() const {
		return errorLocation;
	}


	bool hasError() const {
		return error.hasError();
	}
	bool hasMessage() const {
		return error.hasMessage();
	}
	std::string getMessage() {
		errorLocation = -1;
		return error.getMessage();
	}
	std::string getMessageNoClear() const {
		return error.getMessageNoClear();
	}

};


/** Stream output for plans */
std::ostream& operator << (std::ostream& os, const Plan &f);

}

#endif // Plan_H

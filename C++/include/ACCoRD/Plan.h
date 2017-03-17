
/*
 * Plan - the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2017 United States Government as represented by
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
#include "Quad.h"
#include "BoundingRectangle.h"
#include "TcpData.h"
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
	typedef std::vector<TcpData> tcpDataVector;
	typedef tcpDataVector::iterator tcpDataIterator;

	friend class PlanCollection;        // Plans needs to access some of these
	std::string  name;
	navPointVector points;
	tcpDataVector data;
	mutable ErrorLog error;
	mutable int errorLocation;
	std::string  note;
	static bool debug;
	BoundingRectangle bound;
    static double gsIn_0;


public:
	static const double minDt;  // points that are closer together in time than this are treated as the same point
	static const double nearlyZeroGs;

	/** Create an empty Plan */
	Plan();

	/** Create an empty Plan with the given id */
	Plan(const std::string& name);

	/** Create an empty Plan with the given id */
	Plan(const std::string& name, const std::string& note);


	/** Construct a new object that is a deep copy of the supplied object 
	 * 
	 * @param fp plan to copy
	 */
	Plan(const Plan& fp);

	~Plan();

	static std::string specPre();

protected:
	/** initialize class variables */
	void init();


public:

	/** search plan to see if there are any TCPs
	 * 
	 * @return  true if the plan does not contain any TCPs 
	 */
	bool isLinear() const;

	bool validIndex(int i) const;

	/**
	 * Tests if one plan object has the same contents as another.
	 * This test also compares the concrete classes of the two Plans.
	 */
	bool equals(const Plan& fp);

	/** determines if plans are almost equals (not exactly equal because of floating point calculations
	 * 
	 * @param p    plan of interest
	 * @return     true if almost equal in location and time
	 */
	bool almostEquals(const Plan& p) const;

	/**
	 * Check that positions and times are virtually identical
	 * 
	 * Note: it does not compare source info, or TCP attributes
	 * 
	 * @param p              Plan to compare with
	 * @param epsilon_horiz  allowable horizontal deviation [m]
	 * @param epsilon_vert   allowable vertical deviation [m]
	 * @return true if almost equal in location and time
	 */
	bool almostEquals(const Plan& p, double epsilon_horiz, double epsilon_vert) const;

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

	/** Get an approximation of the bounding rectangle around this plan */
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

	bool isTimeInPlan(double t) const;

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

	void clearLabel(int ix);

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

	/**
	 * Return segment that is distance "d" from segment startIx
	 * 
	 * @param d    distance of interest
	 * @return     segment number
	 */
	int getSegmentByDistance(int startIx, double d) const;

	/**
	 * Return segment that is distance "d" from beginning of plan
	 * 
	 * @param d    distance of interest
	 * @return     segment number
	 */
	int getSegmentByDistance(double d) const;


	/**
	 * Return the index of the point nearest to the provided time.
	 * in the event two points are equidistant from the time, the earlier one
	 * is returned.  If the plan is empty, -1 will be returned.
	 */
	int getNearestIndex(double tm) const;

	/**
	 * Given an index i, returns the corresponding point's time in seconds. If
	 * the index is outside the range of points, then an error is set.
	 * @param i   the segment number
	 * @return    the time at point i

	 */
	double time(int i) const;

	const Position getPos(int i) const;
	/**
	 * Return the NavPoint at the given index (segment number)
	 * @param i the segment number
	 * @return the NavPoint of this index
	 */

	const NavPoint point(int i) const;

	const std::pair<NavPoint,TcpData> get(int i) const;

private:
	TcpData& getTcpDataRef(int i);

	/** Do not rely on this like an invalid value in Vect3::INVALID */
	static TcpData invalid_value;

public:
	TcpData getTcpData(int i) const;

	/** Returns true if the point at index i is an unmodified original point, 
	   false otherwise.  0 &lt;= i &lt; size() */
	bool    isOriginal(int i) const;
	bool    isAltPreserve(int i) const;
	double  signedRadius(int i) const;
	double  turnRadius(int i) const;
	bool    isVirtual(int i) const;
	bool    hasSource(int i) const;

	double  trkAccel(int i) const;
	double  gsAccel(int i) const;
	double  vsAccel(int i) const;

//	Velocity velocityInit(int i) const;

	/** true if this point is the start of a turn */
	bool isTrkTCP(int i)      const;
	/** true if this point is the start of a turn */
	bool isBOT(int i)      const;
	bool isEOT(int i)      const;
	bool isGsTCP(int i)    const;
	bool isBGS(int i)      const;
	bool isEGS(int i)      const;
	bool isVsTCP(int i)    const;
	bool isBVS(int i)      const;
	bool isEVS(int i)      const;
	bool isBeginTCP(int i) const;
	bool isEndTCP(int i)   const;

	bool isTCP(int i) const;

	NavPoint sourceNavPoint(int i) const;

	static std::pair<NavPoint,TcpData> makeBOT(const NavPoint& src, Position p, double t,  double signedRadius, const Position& center, int linearIndex);
	static std::pair<NavPoint,TcpData> makeEOT(const NavPoint& src, Position p, double t, int linearIndex);
	static std::pair<NavPoint,TcpData> makeEOTBOT(const NavPoint& src, Position p, double t,  double signedRadius, const Position& center, int linearIndex);


	static std::pair<NavPoint,TcpData> makeBGS(const NavPoint& src, Position p, double t, double a, int linearIndex);
	static std::pair<NavPoint,TcpData> makeEGS(const NavPoint& src, Position p, double t, int linearIndex);
	static std::pair<NavPoint,TcpData> makeEGSBGS(const NavPoint& src, Position p, double t, double a, int linearIndex);

	static std::pair<NavPoint,TcpData> makeBVS(const NavPoint& src, Position p, double t, double a, int linearIndex);
	static std::pair<NavPoint,TcpData> makeEVS(const NavPoint& src, Position p, double t, int linearIndex);
	static std::pair<NavPoint,TcpData> makeEVSBVS(const NavPoint& src, Position p, double t, double a, int linearIndex);

	static std::pair<NavPoint,TcpData> makeMidpoint(const NavPoint& src, TcpData& tcp, const Position& p, double t, int linearIndex);

	//
	//
	// TCP MetaData Methods
	//
	//
	//

	std::string getInfo(int i) const;
	void setInfo(int i, const std::string& info);

	void setVirtual(int i);

	/** Returns true if the point at index i is an unmodified original point,
	   false otherwise.  0 &lt;= i &lt; size() */
//	bool isOriginal(int i) const;
	void setOriginal(int i);

//	/** AltPreserve is used by TrajGen.generateVsTCPs to create a vertical profile that is a function
//	 *  of locations (rather than time which is very fluid).  The TrajGen method "markVsChanges" marks points
//	 *  with a significant vertical speed change as "AltPreserve".
//	 *
//	 * @param i    index of point to be tested
//	 * @return
//	 */
//	bool isAltPreserve(int i) const;

	/** AltPreserve is used by TrajGen.generateVsTCPs to create a vertical profile that is a function
	 *  of locations (rather than time which is very fluid).  The TrajGen method "markVsChanges" marks points
	 *  with a significant vertical speed change as "AltPreserve".
	 * 
	 * @param i    index of point set as "AltPReserve"
	 */
	void setAltPreserve(int i);

//	/**
//	 * This returns the radius of the current turn. If this is not a turn point,
//	 * it returns the stores radius value. If the associated acceleration is
//	 * 0.0, this returns a radius of zero.
//	 */
//	double turnRadius(int i) const;

//	/**
//	 * This returns the radius of the current turn. If this is not a turn point,
//	 * it returns the stores radius value. If the associated acceleration is
//	 * 0.0, this returns a radius of zero.
//	 */
//	double signedRadius(int i) const;

	/** Sets the radius of point i
	 *
	 * @param i          index of point
	 * @param radius     value to be set
	 */
	void setRadius(int i, double radius);

	double getGsIn_0() const;

	void setGsIn_0(double gsIn_0);

	/**
	 * This method returns a center of turn position with the same altitude as
	 * the current point. If the current point is not a turn point, and has a
	 * zero stored radius, this returns an invalid position.
	 */
	Position turnCenter(int i) const;

	/** Source time of point this was based on for any type except BVS. */
	// @Deprecated
	double sourceTime(int i) const;

	// @Deprecated
	void setSourceTime(int i, double time);

	/** Source time of point this was based on for any type except BVS. */
	// @Deprecated
	Position sourcePosition(int i) const;

	// @Deprecated
	void setSourcePosition(int i, const Position& pos);

	//	NavPoint sourceNavPoint(int i) {
	//		if (i < 0 || i >= size()) {
	//			addError("sourceNavPoint: invalid point index of " + i + " size=" + size());
	//			return NavPoint.INVALID;
	//		}
	//		return new NavPoint(data[i].sourcePosition, data[i].sourceTime);
	//	}

	//	bool hasSource(int i) {
	//		if (i < 0 || i >= size()) {
	//			addError("hasSource: invalid point index of " + i + " size=" + size());
	//			return false;
	//		}
	//		return data[i].sourceTime >= 0;
	//	}

	void resetSource(int i);

	void setSource(int i, const NavPoint& npi);

	/** Often a kinematic plan is generated from a linear plan.  See TrajGen.makeKinematicPlan.  This function
	 *  maps the index of a point back to the linear plan from which it was generated.
	 *
	 * @return   index of original linear plan that point "i" was derived from
	 */
	int linearIndex(int i) const;

	void setLinearIndex(int i, int index);

//	/**
//	 * If this is a TCP, return the initial velocity at point "i". Otherwise return an
//	 * invalid velocity. Not that this value is only well-defined for "begin" TCPs: (BGSC, BVSC, BOT)
//	 *
//	 * Note:  If there is a speed discontinuity at point "i", velocityInit correspond to (TrkOut(i), gsOut(i), vsOut(i))
//	 */
//	Velocity velocityInit(int i) const;

	/** Set the "velocityInit" field to "vel" fro point "i"
	 *
	 * @param i
	 * @param vel
	 */
//	void setVelocityInit(int i, const Velocity& vel);

//	/**
//	 * Return the (signed) turn rate (i.e., position acceleration in the "track"
//	 * dimension) associated with this point. Turn rate is in rad/sec (positive
//	 * is clockwise/right, negative is counterclockwise/left), otherwise return
//	 * 0.0;
//	 */
//	double trkAccel(int i) const;
//
//	/**
//	 * Return the associated (signed) ground speed rate of change (i.e, horizontal acceleration).
//	 */
//	double gsAccel(int i) const;

	void setGsAccel(int i, double accel);

//	/**
//	 * Return the associated (signed) vertical acceleration.
//	 */
//	double vsAccel(int i) const;

	void setVsAccel(int i, double accel);
	void setBOT(int i, double signedRadius, Position center, int linearIndex);
	void setEOT(int i);
	void setEOTBOT(int i, double signedRadius, Position center, int linearIndex);
	void setBGS(int i);
	void setEGS(int i);
	void setEGSBGS(int i);
	void setBVS(int i);
	void setEVS(int i);
	void setEVSBVS(int i);

//	/** true if this point is a TCP */
//	bool isTCP(int i) const;

//	/** true if this point is part of a turn */
//	bool isTrkTCP(int i) const;
//	/** true if this point is the start of a turn */
//	bool isBOT(int i) const;
//
//	/** true if this point is the end of a turn */
//	bool isEOT(int i) const;
//	bool isGsTCP(int i) const;
//
//	bool isBGS(int i) const;
//	bool isEGS(int i) const;
//
//	/** true if this point is part of a vs change */
//	bool isVsTCP(int i) const;
//
//	/** true if this point is the start of a vs change */
//	bool isBVS(int i) const;
//
//	/** true if this point is the end of a vs change */
//	bool isEVS(int i) const;
//	bool isBeginTCP(int i) const;
//	bool isEndTCP(int i) const;



	/** Add a Navpoint to the plan that does not have any associated TCP information.  Once this point is added to the plan a set of default
	 * TCP information will be used for this point.
	 * 
	 * @param p the point to add
	 * @return the (non-negative) index of the point if the point is added without problems, otherwise it will return a negative value.
	 */
	int addNavPoint(const NavPoint& p);

	int add(const Position& p, double time);

//	int addFull(const NavPoint& p, double radiusSigned, double accel_gs, double accel_vs,
//			const NavPoint& source, int linearIndex,
//			const std::string& information);

	int add(const std::pair<NavPoint,TcpData>& p);

	int add(const NavPoint& p2, const TcpData& d);

	/** Remove the i-th point in this plan. (note: This does not store the fact a point was deleted.) */
	void remove(int i);


private:

	/**
	 * Adds a point at index i. This may not preserve time ordering! This may
	 * result in inconsistencies between implied and stored ground and vertical
	 * speeds for TCP points.
	 * 
	 * @param i the index to place the new point
	 * @param v the new NavPoint
	 * @param d the new TcpData associated with this NavPoint
	 */
	void insert(int i, const NavPoint& v, const TcpData& d);

	int indexSearch(double tm, int i1, int i2) const;


	/**
	 * This function computes the velocity out at point i in a strictly linear
	 * manner. If i is an inner point it uses the next point to construct the
	 * tangent. if the next point is less than dt ahead in time, then it finds
	 * the next point that is at least minDt ahead in time.
	 * 
	 * If it is called with the last point in the plan it looks backward for a
	 * point. In this case it uses final velocity on the previous leg.
	 * 
	 * @param i   index of point
	 * @return    initial velocity at point i
	 */
	Velocity linearVelocityOut(int i) const;

//	Velocity finalLinearVelocity(int i) const;

	std::vector<Quad<double,double,double,std::string> > buildDistList(int ixBOT, int ixEOT, double ratio);

	std::vector<Quad<double,double,double,std::string> > buildDistList(int ixBOT, int ixEOT, const Position& vertex);


public:

//	/**  **UNUSED**
//	 * Inserts point at index, then timeshifts itself and all subsequent points, if necessary.
//	 * This may result in inconsistencies between implied and stored velocities for TCP points to either side of the inserted point (as well as itself).
//	 * If the inserted point in is an acceleration zone and not on the current plan, this may result in other inconsistencies.
//	 * @param i
//	 * @param v
//	 */
//	void insertWithTimeshift(int i, const NavPoint& v);

	/** Set altitude of point "i" to "alt"
	 * 
	 */
	void setAlt(int i, double alt);

	/**
	 * Set the time of the given point to the given value.  Note
	 * the index of the point may change due to a new time.
	 * 
	 * @param i the index of the point to change
	 * @param t the new time
	 */
	void setTime(int i, double t);

	/**
	 * Creates a copy with all points in the plan (starting at index start) time
	 * shifted by the provided amount "st". This will drop any points that become
	 * negative or that become out of order. Note this also shifts the start
	 * point. This version will also shift the "source times" associated with
	 * the points
	 */
	Plan copyAndTimeShift(int start, double st);

	/**
	 * Temporally shift all points in the plan (starting at index start) by the provided amount st.
	 * This will drop any points that become negative or that become out of order. 
	 * Note this also shifts the start point as well as any points marked as time-fixed.
	 */
//	bool timeshiftPlan(int start, double st, bool preserveRTAs);
	bool timeShiftPlan(int start, double st);

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
	 * This returns the index of the Beginning of Turn (BOT) point that is less
	 * than the given index, or -1 if there is no such point. This is generally
	 * intended to be used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of BOT or EOT
	 */
	int prevBOT(int current) const;
	/**
	 * This returns the index of the turn end point less than the given index, or -1 if
	 * there is no such point. This is generally intended to be used to find the
	 * beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of EOT
	 */
	int prevEOT(int current) const;

	/**
	 * This returns the index of the End of Turn point greater than the given index, or -1
	 * if there is no such point. This is generally intended to be used to find
	 * the end of an acceleration zone.
	 */
	int nextEOT(int current) const;
	/** This returns the index of the next Beginning of Turn (BOT) point that occurs after "current"
	 * 
	 * @param current  start search after this index  
	 * @return         index of next BOT, or -1 if there is no next BOT
	 */
	int nextBOT(int current) const;

//	/**
//	 * This returns the index of the Ground Speed Change Begin point <= the given index, or -1 if there is no such point.
//	 * This is generally intended to be used to find the beginning of an acceleration zone.
//	 */
//	int prev_BGS(double t) const ;

	/**
	 * This returns the index of the Ground Speed Change Begin point less than the given
	 * index, or -1 if there is no such point. This is generally intended to be
	 * used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of BGS
	 */
	int prevBGS(int current) const;
	/**
	 * This returns the index of the ground speed end point less than or equal to the given index,
	 * or -1 if there is no such point. This is generally intended to be used to
	 * find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of EGS
	 */
	int prevEGS(int current) const;

	/**
	 * This returns the index of the Ground Speed Change End point greater than the given index, or -1 if there is no such point. 
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	int nextEGS(int current) const;
	/**
	 * This returns the index of the Ground Speed Change Begin point greater than the given index, or -1 if there is no such point. 
	 * This is generally intended to be used to find the end of an acceleration zone.
	 */
	int nextBGS(int current) const;

	/**
	 * This returns the index of the Vertical Speed Change Begin point less than the
	 * given index, or -1 if there is no such point. This is generally intended
	 * to be used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of BVS
	 */
	int prevBVS(int current) const;
	/**
	 * This returns the index of the vertical speed end point less than or equal to the given
	 * index, or -1 if there is no such point. This is generally intended to be
	 * used to find the beginning of an acceleration zone.
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type of EVS
	 */
	int prevEVS(int current) const;

	/**
	 * This returns the index of the Vertical Speed Change End point that is
	 * greater than the given index, or -1 if there is no such point. This is
	 * generally intended to be used to find the end of an acceleration zone.
	 */
	int nextEVS(int current) const;
	/**
	 * This returns the index of the next Vertical Speed Change begin point that
	 * is greater than the given index, or -1 if there is no such point. This is
	 * generally intended to be used to find the end of an acceleration zone.
	 */
	int nextBVS(int current) const;

	/**
	 * This finds the last previous index where the TCP is of type tcp1 or tcp2
	 * 
	 * @param current
	 *            the index of the point to begin the search (Note: the index
	 *            may be up to size instead of size-1. This allows the current
	 *            point to be checked by this method.)
	 * @return index before <i>current</i> which has a TCP type
	 */
	int prevTCP(int current) const;

	/**
	 * This returns true if the given time is greater than or equal to a BOT but before an EOT point
	 */
	bool inTrkChange(double t) const;
//	/** This returns true if the given time is >= a BOT but before an EOT point */
	//bool inTrkChange(int i) const;

	/**
	 * This returns true if the given time is greater than or equal to a GSCBegin but before a GSCEnd
	 * point
	 */
	bool inGsChange(double t) const;
	//bool inGsChange(int i) const;


	/**
	 * This returns true if the given time is greater than or equal to a VSCBegin but before a VSCEnd
	 * point
	 */
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
	 * Return a linear interpolation of the position at the given time. If the
	 * time is beyond the end of the plan and getExtend() is true, then the
	 * position is extrapolated after the end of the plan. An error is set if
	 * the time is before the beginning of the plan.
	 *
	 * @param t
	 *            time
	 * @return linear interpolated position at time t
	 */
	Position position(double t) const;

	/**
	 * Return the position at the given time. 
	 *
	 * @param t       time
	 * @param linear  if true, ignore accelerations
	 * @return    Interpolated position at time t
	 */
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


	/**
	 * time required to cover distance "dist" if initial speed is "vo" and acceleration is "gsAccel"
	 *
	 * @param vo       initial velocity
	 * @param gsAccel  ground speed acceleration
	 * @param dist     distance
	 * @return time required to cover distance
	 */
	static double timeFromDistance(double vo, double gsAccel, double dist);

	// experimental - map path distance (in this segment) to relative time (in this segment)
	// if there is a gs0=0 segment, return the time of the start of the segment
	// return -1 on out of bounds input
	/** Return the delta time that corresponds to the point that is path distance "rdist" from the start of segment "seg"
	 * 
	 * @param seg    segment of interest
	 * @param rdist  distance from "seg"
	 * @return       time of point that is "rdist" distance from start of segment "seg"
	 * 
	 * Note: if there is a gs0=0 segment, return the time of the start of the segment return -1 on out of bounds input
	 */
	double timeFromDistanceWithinSeg(int seg, double rdist) const;

	// experimental -- map total path distance to absolute time
	// return -1 on out of bounds input
	/** Return the time that corresponds to the point that is path distance "rdist" from the start of plan
	 * 
	 * @param dist  distance from start of plan
	 * @return      time of point that is "rdist" distance from start of plan
	 * 
	 */
	double timeFromDistance(double dist) const;

	/** Return the absolute time that corresponds to the point that is path distance "dist" from startSeg
	 * 
	 * @param dist  distance from start of plan
	 * @return      time of point that is "rdist" distance from start of plan
	 * 
	 */
	double timeFromDistance(int startSeg, double dist) const;

	/**
	 * Estimate the velocity between point i to point
	 * i+1 for this aircraft.   This is not defined for the last point of the plan.
	 */
	Velocity averageVelocity(int i) const;

	/** estimate the velocity from point i to point i+1 (at point i).
	 * 
	 * @param i index of point
	 * @return velocity at point i
	 */
	Velocity initialVelocity(int i) const;

	Velocity initialVelocity(int i, bool linear) const;

	//    Velocity averageVelocity(double tm) const;

	// estimate the velocity from point i to point i+1 (at point i+1).  This is not defined for the last point of the plan.
	Velocity finalVelocity(int i) const;

	Velocity finalVelocity(int i, bool linear) const;

//	Velocity dtFinalVelocity(int i, bool linear) const;


	/** Return the velocity of the point that is path distance "rdist" from the start of plan
	 * 
	 * @param d     distance from start of plan
	 * @return      velocity of point that is "rdist" distance from start of plan
	 * 
	 */
	Velocity velocityByDistance(double d) const;

	/**
	 * Calculate track angle out of point "i"
	 * 
	 * @param seg      The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         track angle out of point "i"
	 */
	double trkOut(int seg, bool linear) const;

	double trkOut(int seg) const;
	double trkIn (int seg) const;
	/**
	 * Calculate track angle at the end of segment "i"
	 * 
	 * @param seg      The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         track angle at the end of segment "i"
	 */
	double trkFinal(int seg, bool linear) const;

	/**
	 * ground speed out of point "i"
	 * 
	 * @param i       The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return ground speed
	 */
	double gsOut(int i, bool linear) const;

	/**
	 * ground speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as gsOut
	 * 
	 * @param i        The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return ground speed
	 */
	double gsFinal(int i, bool linear) const;

	/**
	 * ground speed into point "seg"
	 * 
	 * @param seg
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return ground speed
	 */
	double gsIn(int seg, bool linear) const;


	double gsOut(int seg) const;

	double gsFinal(int seg) const;

	double gsIn(int seg) const;


//	double gsAtSeg(int seg, bool linear) const;

	/**
	 * ground speed at time t (which must be in segment "seg")
	 * 
	 * @param seg      segment where time "t" is located
	 * @param gsAtSeg  ground speed out of segment "seg"
	 * @param t        time of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         ground speed at time t (which must be in segment "seg")
	 */
	double gsAtTime(int seg, double gsAtSeg, double t, bool linear) const;

	/**
	 * ground speed at time t
	 * 
	 * @param t      time of interest
	 * @param linear If true, then interpret plan in a linear manner
	 * @return       ground speed at time t
	 */
	double gsAtTime(double t, bool linear) const;


	/**
	 * vertical speed out of point "i"
	 * 
	 * @param i
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return vertical speed
	 */
	double vsOut(int i, bool linear) const;

	/**
	 * vertical speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as vsOut
	 * 
	 * @param i
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return vertical speed
	 */
	double vsFinal(int i, bool linear) const;

	/**
	 * vertical speed into point "seg"
	 * 
	 * @param seg
	 *            The index of the point of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return vertical speed
	 */
	double vsIn(int seg, bool linear) const;


	double vsOut(int seg) const;

	double vsFinal(int seg) const;

	double vsIn(int seg) const;


//	double vsAtSeg(int seg, bool linear) const;

	/**
	 * vertical speed at time t (which must be in segment "seg")
	 * 
	 * @param seg
	 *            segment where time "t" is located
	 * @param vsAtSeg
	 *            vertical speed out of segment "seg"
	 * @param t
	 *            time of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return vertical speed
	 */
	double vsAtTime(int seg, double vsAtSeg, double t, bool linear) const;

	/**
	 * vertical speed at time t
	 * 
	 * @param t
	 *            time of interest
	 * @param linear
	 *            If true, then interpret plan in a linear manner
	 * @return vertical speed
	 */
	double vsAtTime(double t, bool linear) const;

	double vsAtTime(double t) const;

	/** distanceFromTime()
	 * Calculate the distance from the Navpoint at "seq" to plan position at absolute time "t"
	 * 
	 * @param seg    starting position
	 * @param t      time of stopping position (must be in segment "seg")
	 * @param linear If true, then interpret plan in a linear manner
	 * @return distance from NavPoint
	 */
	double distFromPointToTime(int seg, double t, bool linear) const;

	std::pair<Position, Velocity> advanceWithinSeg(int seg, double t, bool linear, double gsAt_d) const ;

	/** Advance forward in a plan by distance (within a segment only)
	 *
	 * @param seg           starting point index
	 * @param distFromSeg   distance to advance from starting point
	 * @param linear        if linear, use straight segments only
	 * @return
	 */
	std::pair<Position, Velocity> advanceDistanceWithinSeg2D(int seg, double distFromSeg, bool linear, double gsAt_d) const;

	/** Advance forward in a plan by distance (within a segment only) (does not compute altitude)
	 * 
	 * @param seg           starting point index
	 * @param distFromSeg   distance to advance from starting point
	 * @param linear        if linear, use straight segments only
	 * @return
	 */
	Position advanceDistanceWithinSeg2D(int seg, double distFromSeg, bool linear) const;

	/** starting with point at seg advance "distanceFromSeg" in Plan
	 * NOTE do not use non-positive value for gsAt_d
	 *
	 * @param seg
	 * @param distFromSeg
	 * @param linear
	 * @param gsAt_d
	 * @return
	 */
	std::pair<Position,int> advanceDistance2D(int seg, double distFromSeg, bool linear) const;

	std::pair<Position,int> advanceDistance(int seg, double distFromSeg, bool linear) const;

	/** Assumes seg = getSegment(t)
	 *
	 * @param t         time
	 * @param linear    If true, then interpret plan in a linear manner
	 * @return          position and velocity at time t
	 * @param gsAt_d    ground speed at time t
	 * @return
	 */
	std::pair<Position,Velocity> posVelWithinSeg(int seg, double t, bool linear, double gsAt_d) const;

	std::pair<Position,Velocity> positionVelocity(double t, bool linear) const;

	std::pair<Position,Velocity> positionVelocity(double t) const;

	/** Return interpolated altitude in this segment at absolute time t
	 *
	 * @param seg     getSegment(t)
	 * @param t       time of interest
	 * @param linear  linear flag
	 * @return
	 */
	std::pair<double,double> interpolateAltVs(int seg, double t, bool linear) const;

	double calcVertAccel(int i) ;

	double calcGsAccel(int i);

	/**
	 * calculate delta time for point i to make ground speed into it = gs
	 * 
	 * @param i         index of interest
	 * @param gs        target ground speed
	 * @return          delta time needed  
	 */
	double calcDtGsin(int i, double gs) const;

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

	/**
	 * set the time at a waypoint such that the ground speed into that waypoint
	 * is "gs", given the ground speed accelerations (possibly ignored)
	 */
	void setAltVSin(int i, double vs, bool preserve);

	/**
	 * change the ground speed into ix to be gs -- all other ground speeds
	 * remain the same
	 * 
	 * @param ix  index
	 * @param gs  new ground speed
	 * @param updateTCP
	 */
	void mkGsIn(int ix, double gs);

	/**
	 * Change the ground speed at ix to be gs -- all other ground speeds remain
	 * the same NOTE: This assumes that there are no BVS - EVS segments in the
	 * area
	 *
	 * Note: If point is a begin TCP, we need to update the velocityIn
	 * 
	 * @param ix
	 *            index
	 * @param gs
	 *            new ground speed
	 */
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


	/**
	 * return the path distance from the location at time t until the next waypoint
	 * 
	 * @param t      current time of interest
	 * @param linear if "linear" then ignore BOTs
	 * @return       path distance
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

	/**
	 * calculate average ground speed over entire plan
	 * 
	 * @return average ground speed
	 */
	double averageGroundSpeed() const;

	// calculate vertical speed from point i to point i+1 (at point i).
	/** 
	 * Calculate vertical speed from point i to point i+1 (at point i).
	 *   
	 * @param i index of the point
	 * @return vertical speed
	 */
	double verticalSpeed(int i) const ;


	/** Structurally calculate vertex of turn from BOT and EOT.  If altMid >=0 THEN
	 *  use it for the altitude.  Otherwise search for a middle point to get altitude.
	 * 
	 * @param ixBOT   index of Beginning of Turn (BOT)
	 * @param ixEOT   index of End of Turn (BOT)
	 * @param altMid  altitude at middle point of turn
	 * @return        vertex of the turn
	 */
	Position vertexFromTurnTcps(int ixBOT, int ixEOT, double altMid) const;

	NavPoint vertexPointTurnTcps(int ixBOT, int ixEOT, double altMid) const;

	/**
	 * 
	 * @param botPos   position of turn beginning
	 * @param eotPos   position of end of turn
	 * @param radius   radius of turn 
	 * @param dir      direction +1 = right, -1 = left
	 * @param center   center position
	 * @param altMid   altitude at middle of turn
	 * @return         vertex of turn
	 */
	static Position vertexFromTurnTcps(const Position& botPos, const Position& eotPos, double radius, int dir, const Position& center, double altMid) ;

	/**
	 * 
	 * @param botPos   position of turn beginning
	 * @param eotPos   position of end of turn
	 * @param signedRadius   signed radius of turn: +1 = right, -1 = left
	 * @param trkInit  initial track at botPos
	 * @param altMid   altitude at middle of turn
	 * @return         vertex of turn
	 */
	static Position vertexFromTurnTcps(const Position& botPos, const Position& eotPos, double signedRadius, double trkInit, double altMid) ;

	int findMOT(int ixBOT, int ixEOT) const;
	/**
	 * Structurally revert TCP at ix: (does not depend upon source time or
	 * source position!!) This private method assumes ix &gt; 0 AND ix &lt;
	 * pln.size(). If ix is not a BOT, then nothing is done
	 * 
	 * @param ix    index of point to be reverted
	 * @param addBackMidPoints
	 *            if addBackMidPoints = true, then if there are extra points
	 *            between the BOT and EOT, make sure they are moved to the
	 *            correct place in the new linear sections. Do this by distance
	 *            not time.
	 * @param killNextGsTCPs
	 *            if true, then if there is a BGS-EGS pair after the turn remove
	 *            both of these
	 * @param zVertex
	 *            if non-negative, then assigned reverted vertex this altitude
	 */
	void structRevertTurnTCP(int ix, bool addBackMidPoints, bool killNextGsTCPs);

//	void structRevertTurnTCP_OLD(int ix, bool addBackMidPoints, bool killNextGsTCPs);


	/**
	 * Structurally revert BGS-EGS pair at index "ix"
	 *         Note: it assumes that BGS-EGS pairs will be removed in ascending
	 *         order
	 * 
	 * @param ix   index
	 * @return index of reverted BGS
	 */
	void structRevertGsTCP(int ix, bool saveAccel);


	/**
	 * Revert all BGS-EGS pairs
	 *
	 * @return     reverted plan containing no ground speed TCPS
	 */

	void revertGsTCPs();

	/**
	 * Revert all BGS-EGS pairs in range "start" to "end"
	 * 
	 * @param start        starting index
	 * @param saveAccel   if true store acceleration in reverted point        
	 */
	void revertGsTCPs(int start, bool saveAccel);


	/**
	 * Revert BVS at ix
	 * 
	 * @param ix    index of a BVS point
	 * @return 
	 */
	double structRevertVsTCP(int ix);

	static double interpolateAlts(double vsInit, double vsAccel, double alt1, double t1, double t);


	/**
	 * Revert all BVS-EVS pairs
	 * 
	 */
	void revertVsTCPs();

	/**
	 * Revert all BVS-EVS pairs in range "start" to "end"
	 *
	 * @param start  starting index
	 * @param end    ending index
	 */
	void revertVsTCPs(int start, int end);

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
//	int revertGroupOfTCPs(int dSeg, bool checkSource);

	void removeRedundantPoints(int from, int to);

	void removeRedundantPoints() ;

	/** Remove point from plan if it passes all specified tests (indicated by flags
	 * 
	 * @param ix    index of interest
	 * @param trkF  track flag: perform track continuity test
	 * @param gsF   ground speed flag: perform ground speed continuity test
	 * @param vsF   vertical speed flag: perform vertical speed continuity test
	 * @return      ix if point is removed, -1 otherwise
	 */
	void removeIfRedundant(int ix, bool trkF, bool gsF, bool vsF);

	void removeIfRedundant(int ix);

	void removeIfVsConstant(int ix);

	/**
	 * This returns true if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and 
	 * end points.  Also requires that there are no points closer together than Plan.minDt.
	 */
	bool isWellFormed() const  ;


	/**
	 * This returns -1 if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and 
	 * end point. Returns a nonnegative value to indicate the problem point. Also requires that there are no points closer 
	 * together than Plan.minDt.
	 */
	int indexWellFormed() const;

	/**
	 * This returns a string representing which part of the plan is not "well formed", i.e. all acceleration zones 
	 * have a matching beginning and end point.  See isWellFormed().
	 */
	std::string strWellFormed() const;

	bool isConsistent(double maxTrkDist, double maxGsDist, double maxVsDist, bool silent) const;

	bool isConsistent() const ;

	bool isConsistent(bool silent) const ;

	bool isWeakConsistent() const ;

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instanteous "jumps," it is not consistent.
	 */
	bool isWeakConsistent(bool silent) const ;


	bool isVelocityContinuous(bool silent) const;

	bool isWeakVelocityContinuous(bool silent) const;

	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instanteous "jumps," it is not consistent.
	 */
	bool isFlyable() const ;

	bool isFlyable(bool silent) const ;

	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instanteous "jumps," it is not consistent.
	 */
	bool isWeakFlyable(bool silent) const ;

	bool isWeakFlyable() const ;
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
	 * Experimental This returns a NavPoint on the plan that is closest to the
	 * given position. If more than one point are closest horizontally, the
	 * closer vertically is returned. If more than one have the same horizontal
	 * and vertical distances, the first is returned.
	 * 
	 * @param p
	 * @return closest point
	 */
	NavPoint closestPoint(const Position& p);

	NavPoint closestPointHoriz(const Position& p);

	/**
	 * Experimental This returns a NavPoint on the plan within the given segment
	 * range that is closest to the given position. If more than one point are
	 * closest horizontally, the closer vertically is returned. If more than one
	 * have the same horizontal and vertical distances, the first is returned.
	 * If start &gt;= end, this returns an INVALID point
	 * 
	 * @param start  start point
	 * @param end    end point
	 * @param p      position to check against
	 * @param horizOnly
	 *            if true, only consider horizontal distances, if false, also
	 *            compare vertical distances if the closest points on 2 segments
	 *            are within maxHdist of each other
	 * @param maxHdist
	 *            only used if horizOnly is false: compare vertical distances if
	 *            candidate points are within this range of each other
	 * @return closest point
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
	 * Attempt to replace the i-th point with the given NavPoint. If successful,
	 * this returns the index of the new point (which may change because of
	 * a new time relative to the old point). This method
	 * returns an error if the given index is out of bounds or a warning if the
	 * new point overlaps with (and replaces) a different existing point.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new point to replace it with
	 * @param d the new TcpData to replace
	 * @return the actual index of the new point
	 */
	int set(int i, const NavPoint& v, const TcpData& d);

	/**
	 * Attempt to replace the i-th point with the given NavPoint. The TcpData from the 
	 * old point is retained.  If successful,
	 * this returns the index of the new point (which may change because of
	 * a new time relative to the old point). This method
	 * returns -1 and sets an error if the given index is out of bounds or a warning if the
	 * new point overlaps with (and replaces) a different existing point.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new NavPoint to replace it with
	 * @return the actual index of the new point
	 */
	int setNavPoint(int i, const NavPoint& v);

public:


	/**
	 * Create a (simple) new Plan by projecting state information.
	 * 
	 * @param id         Name of aircraft
	 * @param pos        Initial position of aircraft
	 * @param v          Initial velocity of aircraft (if pos in in lat/lon, then this assumes a great circle initial heading)
	 * @param startTime  Time of initial state
	 * @param endTime    Final time when projection ends
	 * @return new Plan, with a Fixed starting point. If endTime &lt;= startTime, returns an empty Plan.
	 */
	static Plan planFromState(const std::string& id, const Position& pos, const Velocity& v, double startTime, double endTime);

	Plan copyWithIndex() const;

	/**
	 * Create new plan from existing using points from index "firstIx" to index "lastIx"
	 * 
	 * @param firstIx  first index
	 * @param lastIx   last index
	 * @return new cut down plan
	 */
	Plan cut(int firstIx, int lastIx) const;

	/** merge together all points closer together in time than "minDt"
	 * @param minDt     the smallest delta time allowed between points.  See also wellFormed()
	 */
	void mergeClosePoints(double minDt);
	void mergeClosePoints();


	std::vector<int> findLinearIndex(int ix) const;

	/** String representation of the entire plan */
	std::string toString() const;
	/** String representation of the entire plan */
	std::string toStringFull(bool showSource) const;
	std::string toStringFull(int i) const;
	std::string toStringFull() const;
	std::string toStringFull(int i, bool showSource) const;
	/** Return a string representation that includes additional information */
	static std::string toStringFull(const NavPoint& p, const TcpData& d);
	/** Return a string representation that includes additional information */
	static std::string toStringFull(const NavPoint& p, const TcpData& d, bool showSource);
	std::vector<std::string> toStringList(int i, int precision, bool tcp) const;
	static std::vector<std::string> toStringList(const NavPoint& p, const TcpData& d, int precision, bool tcp);
	/** String representation of the entire plan */
	std::string toStringGs() const;
private:
	/** String representation of the entire plan */
	std::string toStringV(int velField) const;
public:
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

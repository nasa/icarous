/*
 * Plan -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 *           See An Efficient Universal Trajectory Language, NASA/TM-2017-219669, Sept 2017.
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef Plan_H
#define Plan_H

#include "Units.h"
#include "ErrorLog.h"
#include "NavPoint.h"
#include "Position.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "Tuple5.h"
#include "BoundingBox.h"
#include "ParameterData.h"
#include "TcpData.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>


namespace larcfm {



/**
 * Manages a flight plan or kinematic trajectory as a sequence of 4D vectors (three dimensions of space
 * and time).  Acceleration zones in a kinematic plan are defined by appending acceleration values to the
 * beginning point of that zone.
 * <p>
 *
 * This class assumes that (1) the points are added in increasing time, (2) all
 * point times are 0 or positive, (3) there are no duplicate point times.
 * <p>
 * 
 * Points that "overlap" with existing points are deemed as redundant and will
 * not be included in the Plan. Two points overlap if they are within both a
 * minimum distance and time of each other (as defined by minDt).
 * <p>
 * 
 * Kinematic plans can be constructed algorithmically. One way to do this is using
 * the Util.TrajGen.makeKinematicPlan() method to construct a kinematic Plan
 * core from a linear one.
 * <p>
 * 
 * Furthermore this version of the Plan assumes that any points between a
 * beginning and paired end TCP will have constant acceleration (or turn
 * radius). These values will be derived from the beginning and end points. It
 * is not recommended that these Plans be directly modified. Rather the (linear)
 * Plans they are based on should be modified and new kinematic Plans be
 * generated from those.
 * <p>
 *
 * Acceleration regions are delineated by BOT, EOT, BGS, EGS, BVS, EVS trajectory change
 * points (TCPs). In general, the end TCPS points are not considered to be part of the maneuver.
 * All acceleration regions of the same type must be distinct and not overlap.
 * Acceleration begin TCP points contain meta data including acceleration
 * and initial velocity. For other points, velocities are inferred from
 * surrounding points.
 * <p>
 * 
 * See An Efficient Universal Trajectory Language, NASA/TM-2017-219669, Sept 2017.
 * 
 * Typical Usage:
 * <ol>
 * <li>create a Plan from a linear Plan using TrajectoryGen.makePlan( ... )
 * <li>compute position as a function of time using Plan.position(t)
 * </ol>
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
	std::string  note;                  // Note for whole plan?

	static bool debug;
//    static double gsIn_0;
	BoundingBox bound; // NOTE: This bound is only computed when points are added, when points are
	                         // deleted/updated (i.e. same time), the bounding box is not recalculated.
	                         // This can lead to an overly conservative bounding box.


public:
    static double MIN_TRK_DELTA_GEN;        // minimum track delta that will result in a BOT-EOT generation
    static double MIN_GS_DELTA_GEN;         // minimum GS delta that will result in a BGS-EGS generation
    static double MIN_VS_DELTA_GEN;         // minimum VS delta that will result in a BVS-EVS generation
     static std::string manualRadius;        // indicates whether a manual radius was specified at vertex
    static std::string manualGsAccel;       // indicates whether a manual gs accel value was specified
    static std::string manualVsAccel;       // indicates whether a manual vs accel value was specified
	static const double minDt;              // points that are closer together in time than this are treated as the same point
	static const double nearlyZeroGs;
    static const std::string noteParamStart;
    static const std::string noteParamEnd;

	/** Create an empty Plan */
	Plan();

	/** Create an empty Plan with the given id 
	 * @param name name of this plan, typically the aircraft id
	 * */
	explicit Plan(const std::string& name);

	/** Create an empty Plan with the given id 
	 * @param name name of this plan, typically the aircraft id
	 * @param note general information about this Plan. 
	 */
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

	/** Checks if index is in range of the plan
	 * 
	 * @param i  index to be checked
	 * @return   true if valid
	 */
	bool validIndex(int i) const;

	/**
	 * Tests if one plan object has the same contents as another. This test also
	 * compares the concrete classes of the two Plans.
	 * @param fp plan of interest
	 * @return true, if equal
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

	/** Get an approximation of the bounding rectangle around this plan 
	 * @return bounding box*/
	BoundingBox getBoundBox() const;

	/** If size() == 0 then this is a "null" plan. 
	 * @return size*/
	int size() const;

	/** Are points specified in Latitude and Longitude 
	 * @return true if lat/lon*/
	bool isLatLon() const;

	/**
	 * Is the geometry of this Plan the same as the given parameter. Note if
	 * this Plan has no points, then it always agrees with the parameter.
	 * @param latlon true if geometry is expected to be lat/lon
	 * @return true if geometry matches given geometry
	 */
	bool isLatLon(bool latlon) const;

	/**
	 * Return the name of this plan (probably the aircraft name).
	 */
	const std::string& getName() const;

	/**
	 * Set the name of this plan (probably the aircraft name).
	 * @param s name
	 */
	void setName(const std::string& s);

	/**
	 * Return the name of the point i.
	 * @param i index
	 * @return name of point
	 */
	const std::string getPointName(int i) const;

	void setPointName(int i, const std::string& s);

	/**
	 * Return the note of this plan.
	 */
	const std::string& getNote() const;

	/**
	 * Set the note of this plan (probably the aircraft name).
	 * @param s note
	 */
	void setNote(const std::string& s);


	void appendNote(const std::string& s);

	/**
	 * Include a parameterData object into the note field of this plan.  This replaces any such information already in the note field. 
	 * @param pd parameters to include.  If empty, do not include parameter information in the note field (and delete any such previously in it)
	 */
	void includeParameters(ParameterData pd);

	/**
	 * Retrieve parameters stored in the plan's note field, if any.
	 * @return ParameterData containing stored parameters, possibly empty.
	 */
	ParameterData extractParameters() const;


	/**
	 * Return the time of the first point in the plan.
	 * @return first time
	 */
	double getFirstTime() const;

	/**
	 * This returns the time for the first non-virtual point in the plan.
	 * Usually this is the same as getFirstTime(), and this should only be
	 * called if there is a known special handling of virtual points, as it is
	 * less efficient. If there are 0 non-virtual points in the plan, this logs
	 * an error and returns 0.0
	 * @return first time
	 */
	double getFirstRealTime() const;

	/**
	 * Return the time of the last point in the plan.
	 * @return last time
	 */
	double getLastTime() const;

	bool isTimeInPlan(double t) const;

	/**
	 * Return the last point in the plan.
	 * @return last point
	 */

	NavPoint getLastPoint() const;

	/**
	 * Returns the index of the next point which is <b>after</b> startWp. If the
	 * startWp is greater or equal to the last wp, then the index of the last
	 * point is returned. Note: if the plan is empty, this returns a -1
	 * 
	 * @param startWp  index
	 * @return index
	 *
	 */
	int nextPtOrEnd(int startWp) const;


	/**
	 * Return the index of first point that has a name equal to the given
	 * string -1 if there are no matches.
	 * 
	 * @param name      nane
	 * @param startIx   start with this index
	 * @param withWrap  if true, go through whole list
	 * @return index
	 */
	int findName(const std::string& name, int startIx, bool withWrap);

	/**
	 * Return the index of first point that has a name equal to the given
	 * string, return -1 if there are no matches.
	 * 
	 * @param label    String to match
	 * @return index
	 */
	int findName(const std::string& label) const;

	void clearName(int ix);


	/**  
	 * Return information field
	 * @param i   plan index
	 * @return    information stored at index i
	 */
	std::string getInfo(int i) const;

	int findInfo(const std::string& info, int startIx, bool withWrap) const;
	/**
	 * Return the first index that has a tcp field that matches the given search term.
	 * @param info the term to be searched for.
	 * @param exact if true, the term must match the stored information exactly.  If false, the term must be a substring of the stored information; this is generally more robust in most circumstances, especially if the term is delimited.
	 * @return Index of the first occurrence of the term, or -1 if not found
	 */
	int findInfo(const std::string& info, bool exact) const;
	/**
	 * Return the first index that has a tcp field that contains the given search term, or -1 if not found.
	 * Same as findInfo(term, false).
	 * @param info information
	 * @return index
	 */
	int findInfo(const std::string& info) const;

	/**
	 * Set information field
	 * @param i index
	 * @param info value
	 */
	void setInfo(int i, const std::string& info);
	/** Append info at index i
	 * 
	 * @param i       plan index
	 * @param info    information to be appended
	 */
	void appendInfo(int i, const std::string& info);
	void clearInfo(int ix);

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
	 * @param startIx starting index
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
	 * Return the index of the point nearest to the provided time. in the event
	 * two points are equidistant from the time, the earlier one is returned. If
	 * the plan is empty, -1 will be returned.
	 * @param tm time
	 * @return index
	 */
	int getNearestIndex(double tm) const;

	/**
	 * Given an index i, returns the corresponding point's time in seconds. If
	 * the index is outside the range of points, then an error is set.
	 * @param i   the segment number
	 * @return    the time at point i

	 */
	double time(int i) const;

	/** Position at index "i"
	 * 
	 * @param i     index
	 * @return      position at index "i"
	 */
	const Position getPos(int i) const;

	/** Altitude at index i
	 * 
	 * @param i    plan index
	 * @return     Altitude at index i
	 */
	double alt(int i) const;

	/**
	 * Return the NavPoint at the given index (segment number)
	 * @param i the segment number
	 * @return the NavPoint of this index
	 */

	const NavPoint point(int i) const;

	const std::pair<NavPoint,TcpData> get(int i) const;

private:

	/** Do not rely on this like an invalid value in Vect3::INVALID */
	static TcpData invalid_value;

public:
	/**
	 * @param i    plan index
	 * @return     TCP data at index i
	 */
	TcpData getTcpData(int i) const;
	TcpData& getTcpDataRef(int i);

	/** Returns true if the point at index i is an unmodified original point, 
	 * false otherwise.  0 &lt;= i &lt; size() 
	 * @param i index
	 * @return true/false 
	 * */
	bool    isOriginal(int i) const;
	/** Returns true if the point at index i is an altitude perserve point, 
	 *  false otherwise.  0 &lt;= i &lt; size() 
	 * @param i index
	 * @return true/false 
	 * */
	bool    isAltPreserve(int i) const;
	/** Returns true if the point at index i is a virtual point, 
	 *  false otherwise.  0 &lt;= i &lt; size() 
	 * @param i index
	 * @return true/false 
	 * */
	bool    isVirtual(int i) const;
	/**
	 * Return the (signed) radius for the given index.  This is the radius defined at the vertex.
	 * See method {@link setVertexRadius}.
	 * @param i index of point
	 * @return the radius, negative means to the left, positive to the right.
	 */
	double  signedRadius(int i) const;

	/** 
	 * Calculates a radius (great circle distance from center to BOT, i.e. surface radius). 
	 * It uses center and BOT position to calculate the radius.<p>  
	 *  
	 * Note it is usually preferable to get the radius from the {@link signedRadius} method
	 * 
	 * @param i index
	 * @return the calculated radius 
	 */
	double  calcRadius(int i) const;
	/** An unsigned radius value that is stored at a vertex. See method {@link setVertexRadius}.
	 * 
	 * @param i  vertex index
	 * @return   specified radius; a return value of 0 indicates no specified radius
	 */
	double  vertexRadius(int i) const;
	/**  turn direction at index i 
	 * 
	 * @param i   plan index (of BOT containing turn info)
	 * @return    +1 if turn right, -1 if turn left
	 */
	int     turnDir(int i) const;

	double  gsAccel(int i) const;
	double  vsAccel(int i) const;

	bool isTrkTCP(int i)      const;
	bool isBOT(int i)      const;
	bool isMOT(int i)      const;
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

	//
	//
	// TCP MetaData Methods
	//
	//
	//


	/**  makes point a virtual point 
	 * 
	 *   Note: virtual points are temporarily added during conflict detection and resolution
	 * 
	 * @param i  plan index
	 */
	void setVirtual(int i);

	/** Returns true if the point at index i is an original point
	 * 
	 * @param i   plan index
	 */
	void setOriginal(int i);

	/** AltPreserve is used by TrajGen.generateVsTCPs to create a vertical profile that is a function
	 *  of locations (rather than time which is very fluid).  The TrajGen method "markVsChanges" marks points
	 *  with a significant vertical speed change as "AltPreserve".
	 * 
	 * @param i    index of point set as "AltPReserve"
	 */
	void setAltPreserve(int i);


	/** Sets the radius of point i  (in a linear plan)
	 *  
	 *  Note:  This is used by TrajGen.makeKinematicPlan to create a turn with a user-specified radius
	 *         rather than one generated from a bank angle and ground speed.
	 *  
	 * @param i          index of point
	 * @param radius     value to be set
	 */
	void setVertexRadius(int i, double radius);

//	double getGsIn_0() const;
//
//	void setGsIn_0(double gsIn_0);

	/**
	 * This method returns a center of turn position with the same altitude as
	 * the current point. If the current point is not a turn point, and has a
	 * zero stored radius, this returns an invalid position.
	 * 
	 * @param  i index of BOT where turn info is stored
	 * @return position of center
	 */
	Position turnCenter(int i) const;

	/**
	 * Ground speed acceleration at a point
	 * @param i   index of BGS
	 * @return    ground speed acceleration stored in the BGS
	 */
	double getGsAccel(int i);
	/**
	 * Vertical speed acceleration at a point
	 * @param i   index of BVS
	 * @return    vertical speed acceleration stored in the BVS
	 */
	double getVsAccel(int i);

	/** Sets the desired ground speed acceleration of point i  (in a linear plan)
	 *  
	 *  Note:  This is used by TrajGen.makeKinematicPlan to create a BGS-EGS with a user-specified gs acceleration
	 *         rather than one generated using the parameter.  This enables different accelerations at different
	 *         points.
	 *  
	 * @param i          index of point
	 * @param accel      ground speed acceleration to be set
	 */
	void setGsAccel(int i, double accel);
	/** Sets the desired vertical speed acceleration of point i  (in a linear plan)
	 *  
	 *  Note:  This is used by TrajGen.makeKinematicPlan to create a BVS-EVS with a user-specified vs acceleration
	 *         rather than one generated using the parameter.  This enables different accelerations at different
	 *         points.
	 *  
	 * @param i          index of point
	 * @param accel      vertical speed acceleration to be set
	 */
	void setVsAccel(int i, double accel);
	/**  makes the point at index i to be a BOT
	 * 
	 * @param i                 plan index
	 * @param signedRadius      signed radius, positive indicates turn right, negative indicated turn left
	 * @param center            center of turn
	 */
	void setBOT(int i, double signedRadius, Position center);
	/**  makes the point at index i to be an EOT
	 * 
	 * @param i                 plan index
	 */
	void setEOT(int i);
	/**  makes the point at index i to be an EOTBOT
	 * 
	 * @param i                 plan index
	 * @param signedRadius      signed radius, positive indicates turn right, negative indicated turn left
	 * @param center            center of turn
	 */
	void setEOTBOT(int i, double signedRadius, Position center);
	/**  makes the point at index i to be a BGS
	 * 
	 * @param i        plan index
	 * @param acc      ground speed acceleration
	 */

	void setMOT(int i);

	void clearMOT(int i);


	void setBGS(int i, double acc);
	void setEGS(int i);
	void clearEGS(int ix);
	void setEGSBGS(int i, double acc);
	void clearBGS(int ix);
	/**  makes the point at index i to be a BVS
	 * 
	 * @param i        plan index
	 * @param acc      vertical speed acceleration
	 */
	void setBVS(int i, double acc);
	void setEVS(int i);
	void setEVSBVS(int i, double acc);



	/** Add a Navpoint to the plan that does not have any associated TCP information.  Once this point is added to the plan a set of default
	 * TCP information will be used for this point.
	 * 
	 * @param p the point to add
	 * @return the (non-negative) index of the point if the point is added without problems, otherwise it will return a negative value.
	 */
	int addNavPoint(const NavPoint& p);

	/** Add a NavPoint to the plan constructed from a Position and a time
	 * 
	 * @param p         Position
	 * @param time      Time
	 * @return          updated plan
	 */
	int add(const Position& p, double time);

	int add(const std::pair<NavPoint,TcpData>& p);

	int add(const NavPoint& p2, const TcpData& d);

	/**
	 * Remove the i-th point in this plan. (note: This does not store the fact a
	 * point was deleted.)
	 * @param i index
	 */
	void remove(int i);

	/**
	 * Insert a navpoint at the indicated distance from plan start. 
	 * @param d distance (cannot be negative)
	 * @return index of new point, or -1 if there is an error.
	 */
	int insertByDistance(double d);

	/**
	 * Insert a navpoint at the indicated distance from the given index point.  Negative distances are before the given point. 
	 * @param i index
	 * @param d distance (may be positive or negative)
	 * @return index of new point, or -1 if there is an error.
	 */
	int insertByDistance(int i, double d);


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
	 * ground speed out of point "i"
	 * 
	 * @param i       The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return        ground speed at the beginning of segment "i"
	 */
	double gsOutCalc(int i, bool linear) const;
	/**
	 * ground speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as gsOut
	 * 
	 * @param i        The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         ground speed at the end of segment "i"
	 */
	double gsFinalCalc(int i, bool linear) const;


//	std::vector<Tuple5<double,double,double,std::string,std::string> > buildDistList(int ixBOT, int ixEOT, double ratio);
//
//	std::vector<Tuple5<double,double,double,std::string,std::string> > buildDistList(int ixBOT, int ixEOT, const Position& vertex);


public:

	/** Set altitude of point "i" to "alt"
	 * @param i index
	 * @param alt altitude
	 * 
	 */
	void setAlt(int i, double alt);

	/**
	 * Attempt to replace the i-th point's tcp data  with the given data. The navpoint from the 
	 * old point is retained.  If successful,
	 * this returns the index of the new point. This method
	 * returns -1 and sets an error if the given index is out of bounds.
	 * 
	 * @param i  the index of the point to be replaced
	 * @param v  the new TcpData to replace it with
	 * @return the actual index of the new point
	 */
	int setTcpData(int i, TcpData v);

	/**
	 * Set the time of the given point to the given value.  Note
	 * the index of the point may change due to a new time.
	 * 
	 * @param i the index of the point to change
	 * @param t the new time
	 */
	void setTime(int i, double t);

	/**
	 * Temporally shift all points in the plan (starting at index start) by the provided amount st.
	 * This will drop any points that become negative or that become out of order. 
	 * Note this also shifts the start point as well as any points marked as time-fixed.
	 */
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
	 * @param current current index
	 * @return index
	 */
	int nextEOT(int current) const;

	/** This returns the index of the next Beginning of Turn (BOT) point that occurs after "current"
	 * 
	 * @param current  start search after this index  
	 * @return         index of next BOT, or -1 if there is no next BOT
	 */
	int nextBOT(int current) const;


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
	 * This returns the index of the Ground Speed Change End point greater than the given index, or -1 if there is 
	 * no such point. 
	 * @param current starting index
	 * @return index
	 */
	int nextEGS(int current) const;

	/**
	 * This returns the index of the Ground Speed Change Begin point greater than the given index, 
	 * or -1 if there is no such point. 
	 * @param current current index
	 * @return index
	 */
	int nextBGS(int current) const;


	/** Find the last BOT or EOT before "current" point.  Will not return "current"
	 *
	 * @param  current index point
	 * @return the last BOT or EOT before "current" point. If there is none return -1
	 */
	int prevTRK(int current) const;

	/** Find the last BGS or EGS before "current" point.  Will not return "current"
	 *
	 * @param  current index point
	 * @return the last BGS or EGS before "current" point. If there is none return -1
	 */

	int prevGS(int current) const;


	/** Find the last BVS or EVS before "current" point.  Will not return "current"
	 *
	 * @param  current index point
	 * @return the last BVS or EVS before "current" point. If there is none return -1
	 */
	int prevVS(int current) const;


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
	 * @param current current index
	 * @return index
	 */
	int nextEVS(int current) const;

	/**
	 * This returns the index of the next Vertical Speed Change begin point that
	 * is greater than the given index, or -1 if there is no such point. This is
	 * generally intended to be used to find the end of an acceleration zone.
	 * @param current current index
	 * @return index
	 */
	int nextBVS(int current) const;

	/**
	 * Return the highest indexed TCP point that is less than current, or -1 if none
	 * @param current start point for search
	 * @return greatest tcp index that is less than current, or -1 if none
	 */
	int prevTCP(int current) const;

	/**
	 * Return the lowest indexed TCP point that is greater than current, or -1 if none
	 * @param current start point for search
	 * @return greatest tcp index that is greater than current, or -1 if none
	 */
	int nextTCP(int current) const;

	/**
	 * This returns true if the given time is greater than or equal to a BGS but before a EGS
	 * point
	 * @param t time
	 * @return true if in track change
	 */
	bool inTrkChange(double t) const;

	/**  In track acceleration at index ix ?
	 *
	 * @param ix   point index
	 * @return Returns true if point ix is in a track acceleration zone
	 */
	bool inTrkAccel(int ix) const;


	/**  In ground speed acceleration at index ix ?
	 * 
	 * @param ix   point index
	 * @return Returns true if point ix is in a ground speed acceleration zone
	 */
	bool inGsAccel(int ix) const;


	/**  In vertical speed acceleration at index ix ?
	 * 
	 * @param ix   point index
	 * @return Returns true if point ix is in a vertical speed acceleration zone
	 */
	bool inVsAccel(int ix) const;





	/**
	 * This returns true if the given time is greater than or equal to a BGS but before a EGS
	 * point
	 * @param t time
	 * @return true if in gs change
	 */
	bool inGsChange(double t) const;

	/**
	 * This returns true if the given time is greater than or equal to a BVS but before a EVS
	 * point
	 * @param t time
	 * @return true if in vs change
	 */
	bool inVsChange(double t) const;
//	bool inTrkChangeByDistance(double d) const;
//	bool inGsChangeByDistance(double d) const;
//	bool inVsChangeByDistance(double d) const;

	/**
	 * If time t is in a turn, this returns the radius, otherwise returns a
	 * negative value.
	 * @param t time
	 * @return radius
	 */
	double turnRadiusAtTime(double t) const;


	/**
	 * Return the ground speed rate of change (i.e., position acceleration in
	 * the "ground speed" dimension) associated with the point at time t.
	 * @param t time
	 * @return gs accel
	 */
	double gsAccelAtTime(double t) const;

	/**
	 * Return the vertical speed rate of change (i.e., acceleration in the
	 * vertical dimension) associated with the point at time t.
	 * @param t time
	 * @return vs accel
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


	/** 
	 * Estimate the initial velocity at the given time for this aircraft.   
	 * A time before the beginning of the plan returns a zero velocity.
	 * 
	 * @param tm      time
	 * @return velocity
	 */
	Velocity velocity(double tm) const;

	/** 
	 * Estimate the initial velocity at the given time for this aircraft.   
	 * A time before the beginning of the plan returns a zero velocity.
	 * 
	 * @param tm      time
	 * @param linear  true, if linear plan is to be used
	 * @return velocity
	 */
	Velocity velocity(double tm, bool linear) const;

	/**
	 * time required to cover distance "dist" if initial speed is "vo" and acceleration is "gsAccel"
	 *
	 * @param vo       initial velocity
	 * @param gsAccel  ground speed acceleration
	 * @param dist     distance
	 * @return time required to cover distance
	 */
	static double timeFromDistance(double vo, double gsAccel, double dist);

	/** Return the delta time that corresponds to the point that is path distance "rdist" from the start of segment "seg"
	 * 
	 * @param seg    segment of interest
	 * @param rdist  non-negative distance from "seg"
	 * @return       time of point that is "rdist" distance from start of segment "seg"
	 * 
	 * Note: if there is a gs0=0 segment, return the time of the start of the segment return -1 on out of bounds input
	 */
	double timeFromDistanceWithinSeg(int seg, double rdist) const;

	/** Return the time that corresponds to the point that is path distance "rdist" from the start of plan
	 * 
	 * @param dist  non-negative distance from start of plan
	 * @return      time of point that is "rdist" distance from start of plan
	 * 
	 */
	double timeFromDistance(double dist) const;

	/** Return the absolute time that corresponds to the point that is path distance "dist" from startSeg
	 * 
	 * @param startSeg starting segment
	 * @param dist  non-negative distance from start of segment
	 * @return      time of point that is "rdist" horizontal distance from start of plan
	 * 
	 */
	double timeFromDistance(int startSeg, double dist) const;

	/**
	 * Estimate the velocity between point i to point i+1 for this aircraft.
	 * This is not defined for the last point of the plan.
	 * @param i index
	 * @return velocity
	 */
	Velocity averageVelocity(int i) const;

	/** estimate the velocity from point i to point i+1 (at point i).
	 * 
	 * @param i index of point
	 * @return velocity at point i
	 */
	Velocity initialVelocity(int i) const;

	/** calculate the velocity out of point i.
	 * 
	 * @param    i index of point
	 * @param linear    if true, measure the straight distance, if false measure the curved distance
	 * @return   velocity out of point i
	 */
	Velocity initialVelocity(int i, bool linear) const;


	// estimate the velocity from point i to point i+1 (at point i+1).  This is not defined for the last point of the plan.
	Velocity finalVelocity(int i) const;

	/** Return the velocity at the end of segment "i". This is not defined for the last point (because there is no next point)
	 * 
	 * @param i         segment of interest
	 * @param linear    if true, measure the straight distance, if false measure the curved distance
	 * @return          velocity at end of segment "i"
	 */
	Velocity finalVelocity(int i, bool linear) const;


	/** Return the velocity of the point that is path distance "rdist" from the start of plan
	 * 
	 * @param d     non-negative distance from start of plan
	 * @return      velocity of point that is "rdist" distance from start of plan
	 * 
	 */
	Velocity velocityFromDistance(double d) const;

	/**
	 * Calculate track angle out of point "seg"
	 * 
	 * @param seg      The index of the point of interest
	 * @param linear   If linear is true, then interpret plan in a linear manner
	 * @return         track angle out of point "seg"
	 */
	double trkOut(int seg, bool linear) const;

	/**  return track angle at point ix within a turn starting at ixBOT
	 * 
	 * @param ix        an index between an ixBOT and ixEOT
	 * @param ixBOT     index of the beginning of turn
	 * @return track angle
	 */
	double trkInTurn(int ix, int ixBOT) const;

	double trkOut(int seg) const;
	double trkIn (int seg) const;
	double trkDelta(int i) const;



	/**  This function generalizes trkOut to handle segments with zero ground speed.
	 *   If gsOut(ix) &gt; 0 returns trkOut(ix)
	 * 
	 * @param seg  The index of the point of interest
	 * @return     defined track out
	 */
	double defTrkOut(int seg) const;

	/**  This function generalizes trkIn to handle segments with zero ground speed.
	 *
	 * @param seg  The index of the point of interest
	 * @return     defined track in
	 */
     double defTrkIn (int seg) const;


	/**
	 * Calculate track angle at the end of segment "seg"
	 * 
	 * @param seg      The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         track angle at the end of segment "seg"
	 */
	double trkFinal(int seg, bool linear) const;

	/**
	 * ground speed out of point "i"
	 * 
	 * @param i       The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return        ground speed at the beginning of segment "i"
	 */
	double gsOut(int i, bool linear) const;

	/**
	 * ground speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as gsOut
	 * 
	 * @param i        The index of the point of interest
	 * @param linear   If true, then interpret plan in a linear manner
	 * @return         ground speed at the end of segment "i"
	 */
	double gsFinal(int i, bool linear) const;

	/**
	 * ground speed into point "seg"
	 * 
	 * @param seg  The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return ground speed, or NaN for segment 0.
	 */
	double gsIn(int seg, bool linear) const;
	/**
	 * @param seg       The index of the point of interest
	 * @return          ground speed out of point "seg"
	 */
	double gsOut(int seg) const;
	double gsFinal(int seg) const;
	/**
	 * @param seg       The index of the point of interest
	 * @return          ground speed into point "seg"
	 */
	double gsIn(int seg) const;

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
	 * @param t   time
	 * @return    ground speed at time t
	 */
	double gsAtTime(double t) const;

	/**
	 * vertical speed out of point "i"
	 * 
	 * @param i       The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return        vertical speed at the beginning of segment "i"
	 */
	double vsOut(int i, bool linear) const;

	/**
	 * vertical speed at the end of segment "i"
	 * 
	 * Note. if there is no acceleration, this will be the same as vsOut
	 * 
	 * @param i       The index of the point of interest
	 * @param linear  If true, then interpret plan in a linear manner
	 * @return vertical speed at the end of segment "i"
	 */
	double vsFinal(int i, bool linear) const;

	/** vertical speed into point "seg"
	 * 
	 * @param seg       The index of the point of interest
	 * @param linear    If true, then interpret plan in a linear manner
	 * @return          vertical speed into point "seg"
	 */
	double vsIn(int seg, bool linear) const;
	/**
	 * @param seg       The index of the point of interest
	 * @return          vertical speed out of point "seg"
	 */
	double vsOut(int seg) const;
	double vsFinal(int seg) const;
	/**
	 * @param seg       The index of the point of interest
	 * @return          vertical speed into point "seg"
	 */
	double vsIn(int seg) const;


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
	 * Calculate the distance from the Navpoint at "seq" to plan position at absolute time "t" (within the segment)
	 * 
	 * @param seg    starting position
	 * @param t      time of stopping position (must be in segment "seg")
	 * @param linear If true, then interpret plan in a linear manner
	 * @return distance from NavPoint
	 * Always returns 0 if the time is less than minDt
	 */
	double distFromPointToTime(int seg, double t, bool linear) const;

	std::pair<Position, Velocity> posVelWithinSeg2D(int seg, double t, bool linear, double gsAt_d) const ;

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
	 * @return position
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
	std::pair<double,double> interpolateAltVs(int seg, double dt, bool linear) const;

	// This is equivalent to position(t,linear).alt() -- but faster
	std::pair<double,double> altitudeVs(double t, bool linear) const;

	double calcVertAccel(int i) ;

	double calcGsAccel(int i);

	/**
	 * calculate delta time (from point i-1) into point i to make ground speed into it = gs
	 * 
	 * @param i         index of interest
	 * @param gs        target ground speed
	 * @return          delta time needed  
	 */
	double calcDtGsIn(int i, double gs) const;

	/**
	 * calculate time at a point such that the ground speed into that
	 * point is "gs". If i or gs is invalid, this returns -1. If i is in a
	 * turn, this returns the current point time.
	 * 
	 * Note: parameter maxGsAccel is not used on a linear segment
	 * @param i index
	 * @param gs ground speed
	 * @return time
	 */
	double calcTimeGsIn(int i, double gs) const;

	//double linearCalcTimeGsIn(int i, double gs) const;

	/**
	 * Set the time at a point such that the ground speed into that point
	 * is "gs", given the ground speed accelerations (possibly ignored);
	 * 
	 * Note: This does not leave speeds after this point unchanged as "mkGsIn" does
	 * @param i index
	 * @param gs ground speed
	 */
	void setTimeGsIn(int i, double gs);

	/** Set the altitude at a point such that the vertical speed into that point
	 * is "vs", given the vertical speed accelerations (possibly ignored)
	 * @param i index
	 * @param vs vertical speed
	 * @param preserve preserve flag
	 */
	void setAltVSin(int i, double vs, bool preserve);

	/**  Change the altitude at point ix to "newAlt"
	 * 
	 * @param ix          plan index
	 * @param newAlt      new altitude
	 */
	void mkAlt(int ix, double newAlt);

	/** Remove all AltPreserve Flags from plan
	 * 
	 *  Note:  AltPreserve is used by TrajGen to define the vertical profile and keep it unaltered as
	 *         multiple passes over the plan are executed.
	 * 
	 */
	void removeAltPreserves();

	/**
	 * Change the ground speed into ix to be gs -- all other ground speeds remain the same in the linear case
	 * Note:  If ix is in an acceleration zone, this will also affect gsOut(ix-1)
	 * Note:  If the segment has zero length, or parameter gs = 0, special considerations are present
	 * 
	 * @param ix  index
	 * @param gs  new ground speed
	 * @return true if successful
	 */
	bool mkGsIn(int ix, double gs);

	/**
	 * Change the ground speed at ix to be gs -- all other ground speeds remain the same if linear
	 * Note:  If ix is in an acceleration zone, this will also affect gsIn(ix+1)
	 * Note:  If the segment has zero length, or parameter gs = 0, special considerations are present
	 * 
	 * @param ix        index
	 * @param gs        new ground speed
	 * @return true if successful
	 */
	bool mkGsOut(int ix, double gs);

	/**
	 * Make a new plan with constant ground speed from wp1 to wp2. 
	 * 
	 *       200     200     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    mkGsConstant(p, 1, 3, 500)
	 *    
	 *    	 200     500     500     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    Note that if wp1 == wp2 no change is made.
	 *    
	 * Note: If the initial plan has zero ground speed segments, then the final plan may
	 *       have points removed.
	 *       
	 * Note. This method can handle kinematic models, but it may result in loss of velocity continuity
	 * 
	 * @param wp1     starting index
	 * @param wp2     ending index
	 * @param gs      new ground speed 
	 * 
	 */
	void mkGsConstant(int wp1, int wp2, double gs);

	void mkGsConstant(double gs);

	/** Determines if the track component of velocity is continuous at index i
	 * 
	 * @param i             index i
	 * @param trkEpsilon    tolerance of the test
	 * @param silent        if false, info about a discontinuity is printed
	 * @return              true if track component of velocity is continuous at index i
	 */
	bool isTrkContinuous(int i, double trkEpsilon, bool silent) const ;

	bool isTrkContinuous(int i, bool silent) const;

	bool isTrkContinuous(bool silent) const;

	/** Determines if the ground speed component of velocity is continuous at index i
	 * 
	 * @param i             index i
	 * @param gsEpsilon     tolerance of the test
	 * @param silent        if false, info about a discontinuity is printed
	 * @return              true if ground speed component of velocity is continuous at index i
	 */
	bool isGsContinuous(int i, double gsEpsilon, bool silent) const ;

	bool isGsContinuous(int i, bool silent) const;

	bool isGsContinuous(bool silent) const;

	/** Determines if the vertical speed component of velocity is continuous at index i
	 * 
	 * @param i             index i
	 * @param velEpsilon    tolerance of the test
	 * @param silent        if false, info about a discontinuity is printed
	 * @return              true if vertical speed component of velocity is continuous at index i
	 */
	bool isVsContinuous(int i, double velEpsilon, bool silent) const;

	bool isVsContinuous(int i, bool silent) const;

	bool isVsContinuous(bool silent) const;

	/**
	 * Checks if each point in the plan has approximately the same velocity in and velocity out.
	 * @return true if velocity continuous
	 */
	bool isVelocityContinuous() const;
	/**
	 * Find the cumulative horizontal (curved) path distance for whole plan.
	 * @return path distance
	 */
	double pathDistance() const;

	/** 
	 * Find the horizontal (curved) distance between points i and i+1 [meters]. 
	 * 
	 * @param i index of starting point
	 * @return path distance (horizontal only)
	 */
	double pathDistance(int i) const;

	/** Calculate the horizontal distance between points i and i+1 [meters].
	 * 
	 * @param i        index of starting point
	 * @param linear   if true, measure the straight distance, if false measure the curved distance
	 * @return         path distance (horizontal only)
	 */
	double pathDistance(int i, bool linear) const;

	/**
	 * Find the cumulative horizontal (curved) path distance between points i
	 * and j [meters].
	 * @param i first index
	 * @param j second index
	 * @return path distance
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
	 * return the path distance from the location at time t until the next point
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
	double pathDistanceToPoint(double t, int j) const;

	double pathDistanceFromTime(double t) const;

	/** calculates vertical distance from point i to point i+1
	 * 
	 * @param i   point of interest
	 * @return    vertical distance
	 */
	double vertDistance(int i) const;

	/**
	 * Find the cumulative vertical distance between points i and j [meters].
	 * @param i first point
	 * @param j second point
	 * @return vertical distance
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


	/** Structurally calculate vertex of turn from BOT and EOT.  If altMid &ge; 0 THEN
	 *  use it for the altitude.  Otherwise search for a middle point to get altitude.
	 * 
	 * @param ixBOT   index of Beginning of Turn (BOT)
	 * @param ixEOT   index of End of Turn (BOT)
	 * @param altMid  altitude at middle point of turn
	 * @return        vertex of the turn
	 */
	Position vertexFromTurnTcps(int ixBOT, int ixEOT, double altMid) const;

	/**  Calculate the vertex point from which a turn was generated using BOT and EOT
	 * 
	 * @param ixBOT        index of BOT
	 * @param ixEOT        index of EOT
	 * @param altMid       altitude to assign to vertex
	 * @return point
	 */
	NavPoint vertexPointTurnTcps(int ixBOT, int ixEOT, double altMid) const;

	/** Calculate the vertex point from which a turn was generated using botPos, eotPos, radius, center , etc
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

	/** Find middle of turn
	 * 
	 * @param ixBOT   index of a BOT point
	 * @return index of MOT
	 */
	int findMOT(int ixBOT) const;
	int findMOT(int ixBOT, int ixEOT) const;


	/**
	 * Revert all BGS-EGS pairs
	 * 
	 */
	void revertGsTCPs();
	/**
	 * Revert all BVS-EVS pairs
	 * 
	 */
	void revertVsTCPs();
	/**
	 * Revert all BOT-EOT pairs in range "start" to "end"
	 */
	void revertTurnTCPs();

	/**
	 * Clean up mismatched begin or end tcp markers.
	 */
	void repairPlan();

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
	void revertTurnTCP(int ix, bool killNextGsTCPs);

	/**
	 * Structurally revert BGS-EGS pair at index "ix"
	 *         Note: it assumes that BGS-EGS pairs will be removed in ascending
	 *         order
	 * 
	 * @param ix   index
	 * @return index of reverted BGS
	 */
	void revertGsTCP(int ix);

	void crudeRevertGsTCP(int ix);



	/**
	 * Revert BVS at ix
	 * 
	 * @param ix    index of a BVS point
	 * @return 
	 */
	double revertVsTCP(int ix);

	static double interpolateAlts(double vsInit, double vsAccel, double alt1, double t1, double t);


	/** Remove all redundant points in Plan in the index range [from,to] using "removeIfRedundant"
	 * 
	 * Note: A point is NOT redundant if it is a TCP, is AltPreserve, has information or a name
	 *       or if there is significant change in velocity.  
	 * 
	 * @param from     starting location
	 * @param to       ending location
	 */
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
	int removeIfRedundant(int ix, bool trkF, bool gsF, bool vsF);

	/** <p>Remove point from plan if it passes all specified tests (indicated by flags)</p>
	 * 
	 * Note: This method can remove a "named" point, but will not remove a TCP or AltPreserve
	 * Note: A point is not redundant if it is a TCP, is AltPreserve, has information or a name
	 *       or if there is a significant change in velocity as determined by parameters minTrk, minGs, and minVs,
	 * 
	 * @param ix    index of interest
	 * @param trkF  track flag: perform track continuity test
	 * @param gsF   ground speed flag: perform ground speed continuity test
	 * @param vsF   vertical speed flag: perform vertical speed continuity test
	 * @param minTrk minimum track
	 * @param minGs  minimum ground speed
	 * @param minVs minimum vertical speed
	 * @param repair true if method should repair points
	 * @return index if point is removed, -1 otherwise
	 * 
	 */
	int removeIfRedundant(int ix,  bool trkF, bool gsF, bool vsF, double minTrk, double minGs, double minVs, bool repair);

	/** Remove point from plan if it passes trk and gs tests
	 * 
	 * @param ix    index of interest
	 * @return      ix if point is removed, -1 otherwise
	 */
	int removeIfRedundant(int ix);

	int removeIfVsConstant(int ix);

	/**
	 * This returns true if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and 
	 * end points.  Also requires that there are no points closer together than Plan.minDt.
	 * @return true if well formed
	 */
	bool isWellFormed() const  ;


	/**
	 * This returns -1 if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and 
	 * end point. Returns a nonnegative value to indicate the problem point. Also requires that there are no points closer 
	 * together than Plan.minDt.
	 * @return index of well formed point
	 */
	int indexWellFormed() const;

	/**
	 * This returns a string representing which part of the plan is not "well formed", i.e. all acceleration zones 
	 * have a matching beginning and end point.  See isWellFormed().
	 * @return string representation
	 */
	std::string strWellFormed() const;

private:
	/**
	 * Finds the index of the BOT associated with the given index.  If
	 * the given index does not represent a point within a turn, then
	 * return -1.
	 *
	 * @param p plan
	 * @param i index of some point within a turn
	 * @return index of BOT, this index is less or equal to i.
	 */
	int findBot(int i) const;

	void fixBGS_EGS(int wp1, int wp2, double gs);

public:

	bool isGsConsistent(int ixBGS, double distEpsilon, bool silent, double nearZeroGsValue) const;

	bool isVsConsistent(int ixBVS, double distEpsilon, bool silent) const;

	/**
	 * Check the consistency of all of the points in the turn indicated by <tt>i</tt>.  This assumes that the plan is well-formed.
	 * 
	 * @param p plan
	 * @param i index of a point in a turn (typically the BOT)
	 * @param distH_Epsilon horizontal epsilon
	 * @param silent if true, then do not display anything to the console
	 * @return true, if the EOT is consistent with the BOT.
	 */
	bool isTurnConsistent(int i, double distH_Epsilon, bool silent) const;

	/**  Check the Turn consistency of all of the points.
	 * 
	 * @param   silent if true, then do not display anything to the console
	 * @return true if turns are consistent
	 */
	bool isTurnConsistent(bool silent) const;

	/**  Check the Vertical Speed consistency of all of the points.
	 * 
	 * @param   silent if true, then do not display anything to the console
	 * @return is vertical speed consistent
	 */
	bool isVsConsistent(bool silent) const;

	bool isConsistent(double maxTrkDist, double maxGsDist, double maxVsDist, bool silent, double nearZeroGsValue) const;

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instantaneous "jumps," it is not consistent.
	 * @return true if consistent
	 */
	bool isConsistent() const ;

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instantaneous "jumps," it is not consistent.
	 * @param silent use true to ensure limited console messages
	 * @return true if consistent
	 */
	bool isConsistent(bool silent) const ;

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instantaneous "jumps," it is not consistent.
	 * @return true if weakly consistent
	 */
	bool isWeakConsistent() const ;

	/**
	 * This returns true if the entire plan produces reasonable accelerations.
	 * If the plan has instantaneous "jumps," it is not consistent.
	 * @param silent use true to ensure limited console messages
	 * @return true if weakly consistent
	 */
	bool isWeakConsistent(bool silent) const ;


	/**
	 * Checks if each point in the plan has approximately the same velocity in and velocity out.
	 * @param silent true if limited console messages
	 * @return true if velocity continuous
	 */
	bool isVelocityContinuous(bool silent) const;

	/**
	 * Checks if each point in the plan has approximately the same velocity in and velocity out.
	 * @param silent true if limited console messages
	 * @return true if velocity continuous
	 */
	bool isWeakVelocityContinuous(bool silent) const;

	/**
	 * Checks if the entire plan contains reasonable accelerations. If
	 * the plan has instantaneous <i>jumps,</i> it is not flyable. 
	 * A flyable plan is both consistent and continuous.
	 * 
	 * @return True, if the entire plan contains reasonable accelerations.
	 */
	bool isFlyable() const ;

	/**
	 * Checks if the entire plan produces reasonable accelerations. If
	 * the plan has instantaneous <i>jumps,</i> it is not flyable. 
	 * A flyable plan is both consistent and continuous.
	 * 
	 * @param silent Use true to suppress messages to console.
	 * @return True, if the entire plan produces reasonable accelerations.
	 */
	bool isFlyable(bool silent) const ;

	/**
	 * A flyable plan is both consistent and continuous.
	 * @param silent Use true to suppress messages to console.
	 * @return true if flyable
	 */
	bool isWeakFlyable(bool silent) const ;

	bool isWeakFlyable();

	/**
	 * A weak flyable plan is both weakly consistent and weakly continuous.
	 * @return true if flyable
	 */
	bool isWeakFlyable() const ;

	static std::pair<NavPoint,TcpData> makeBOT(const std::string& name, Position p, double t,  double signedRadius, const Position& center);
	static std::pair<NavPoint,TcpData> makeEOT(Position p, double t);
	static std::pair<NavPoint,TcpData> makeEOTBOT(Position p, double t,  double signedRadius, const Position& center);

	static std::pair<NavPoint,TcpData> makeBGS(const std::string& name, Position p, double t, double a);
	static std::pair<NavPoint,TcpData> makeEGS(Position p, double t);
	static std::pair<NavPoint,TcpData> makeEGSBGS(const std::string& name, Position p, double t, double a);

	static std::pair<NavPoint,TcpData> makeBVS(const std::string& name, Position p, double t, double a);
	static std::pair<NavPoint,TcpData> makeEVS(Position p, double t);
	static std::pair<NavPoint,TcpData> makeEVSBVS(Position p, double t, double a);

	/**
	 * This removes the acceleration tags on points that appear "unbalanced."
	 * This is not particularly intelligent and may result in bizarre (but legal) plans.
	 */
	void fix();

	/**
	 * Experimental 
	 * Return the "closest" point on a segment to position p.  This will only use horizontal (2d) distance calculations unless the 
	 * segment's ground speed is (nearly) zero, in which case it will use altitude to determine the closest point.  
	 * This will attempt to allow for curved turns (if kinematic).  If multiple points are equally closest, it will return the first such point.
	 * @param seg segment to check
	 * @param p reference position
	 * @return closest point on the segment
	 */
	NavPoint closestPointHoriz(int seg, const Position& p) const;

	/**
	 * Experimental
	 * This returns a NavPoint on the plan that is closest to the
	 * given position. If more than one point are closest horizontally, the
	 * closer vertically is returned. If more than one have the same horizontal
	 * and vertical distances, the first is returned.
	 * 
	 * @param p position
	 * @return closest point
	 */
	NavPoint closestPoint(const Position& p) const;

	/**
	 * Experimental
	 * Returns the (first) point on the plan that has the smallest horizontal distance from the reference point.
	 * If the closest point horizontally happens to occur on a 0 gs segment, it will return the point on the segment with the closest altitude.
	 * This will attempt to allow for curved turns in a kinematic plan.
	 * @param p position
	 * @return point
	 */
	NavPoint closestPointHoriz(const Position& p) const;

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
	NavPoint closestPoint(int start, int end, const Position& p, bool horizOnly, double maxHdist) const;

	/**
	 * Return the closest geometric point on a segment to a reference point, as measured as a Euclidean 3D norm from a linear segment.
	 * (Kinematic turns are not accounted for in this distance calculation.) 
	 * @param seg
	 * @param p reference point
	 * @return 
	 */
	Vect3 closestPoint3D(int seg, const Vect3& v0) const;
	/**
	 * Experimental
	 * Return the closest geometric point on a plan to a reference point, as measured as a Euclidean 3D norm from each linear segment.
	 * (Kinematic turns are not accounted for in this distance calculation.)
	 * This may produce unexpected results if a kinematic segment includes a turn &ge; 180 degrees or a vertical segment that crosses over itself. 
	 * @param p reference point
	 * @return point
	 */
	NavPoint closestPoint3D(const Position& p) const;


	/** Is the aircraft accelerating (either horizontally or vertically) at this time? 
	 * @param t time to check for acceleration
	 * @return true if accelerating
	 */
	bool inAccel(double t) const;

	/**
	 * true if this point is in a closed interval [BEGIN_TCP , END_TCP]
	 * 
	 * NOTE: inAccel(t) returns false if the point at time "t" is an end TCP.
	 * This method return true!
	 * @param ix index
	 * @return true if in accel zone
	 * 
	 */
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

	Plan copy() const;

	/**
	 * Create new plan from existing using points from index "firstIx" to index "lastIx".  This
	 * is a "raw" cut, no attention is given to TCPs.  For that see PlanUtil.cutDown
	 * 
	 * @param firstIx  first index
	 * @param lastIx   last index
	 * @return new cut down plan
	 * 
	 * Note.  It is often a good idea to follow this method with a call to "PlanUtil.cleanPlan" and "repairPlan"
	 * Note.  See also PlanUtil.cutDown
	 */
	Plan cut(int firstIx, int lastIx) const;

	/** merge together all points closer together in time than "minDt".
	 * 
	 * @param minDt     the smallest delta time allowed between points.  See also wellFormed()
	 * 
	 */
	int mergeClosePoints(int i, double minDt);
	void mergeClosePoints(double minDt);
	void mergeClosePoints();

	/** merge points j-1 and j if they are closer together (in distance2D) than minDist
	 *
	 * @param j         index to be merged
	 * @param minDist   minimum horizontal distance
	 * @return          index of merged point if merged otherwise return j
	 *
	 * Note:  it retains position of "j"  (i.e. removes j-1)
	 *
	 */
	int  mergeClosePointsByDist(int j, double minDist);

	void cleanPlan();

	double getMIN_GS_DELTA_GEN() const;
	static void setMIN_GS_DELTA_GEN(double minGsDeltaGen);
	double getMIN_TRK_DELTA_GEN() const ;
	static void setMIN_TRK_DELTA_GEN(double minTrkDeltaGen);
	double getMIN_VS_DELTA_GEN() const ;
	static void setMIN_VS_DELTA_GEN(double minVsDeltaGen);

	/** Restores TrajGen generation parameters back to default values
	 * 
	 */
	static void setMinDeltaGen_BackToDefaults();

	std::string toUnitTest() ;
	/** used to create a unit test from the plan.   It creates a Java version of the Plan
	 * 
	 * @param prefix     create variable names using this prefix (prevents duplicate variable defs)
	 * @param asserts    if true, create some assert statements
	 * @param si         if true, create Java code to use SI units
	 * @return string
	 */
	std::string toUnitTest(std::string prefix, bool asserts, bool si);

	/** String representation of the entire plan */
	std::string toString() const;
 private:
	/** Return a string representation that includes NavPoint and TcpData from index i
    */
	std::string toStringPoint(int i, int precision) const;
 public:
	std::vector<std::string> toStringList(int i, int precision, bool tcp) const;
	static std::vector<std::string> toStringList(const NavPoint& p, const TcpData& d, int precision, bool tcp);
	/** String representation of the entire plan */
	std::string toStringVelocity(int velField) const;
	/** String representation of the entire plan 
	 * @return string */
	std::string toStringTrk() const;
	/** String representation of the entire plan 
	 * @return string */
	std::string toStringGs() const;
	/** String representation of the entire plan 
	 * @return string */
	std::string toStringVs() const;
	/** String representation of the entire plan in a tabular form 
	 * 
	 * @return a string
	 */
	std::string toStringProfile() const;



public:

	/**
	 * @deprecated
	 * Returns string that of header information that is compatible with the
	 * file format with header and consistent with a call to toOutput(bool, int,
	 * int). This does not include a terminating newline.
	 * Using the PlanWriter class to print formatted plans instead is preferred.
	 */
	std::string getOutputHeader(bool tcpcolumns) const;


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

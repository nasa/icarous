/*
 * Copyright (c) 2017-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef GSPLAN_H
#define GSPLAN_H

#include <vector>
#include <string>
#include "Route.h"
#include "Position.h"
#include "Plan.h"

namespace larcfm {

/** GsPlan  -- an alternate yet convenient way to store a linear plan.  There are no times
 *             stored. Instead the ground speed at each point is specified.
 * 
 *  Since there are no times in a GsPlan, only the order of the points is maintained.
 *  This often eliminates the need to continually recalculate times.  After an appropriate
 *  GsPlan is constructed it can be converted to a linear plan using the "linearPlan" method
 *  
  */
class GsPlan {

private:
	 Route rt;
	 std::vector<double>  gsOuts;                   // ground speeds out
	 std::string id;                                    // name of the GsPlan
	 double starttime;                            // start time

public:

	GsPlan();
	
	GsPlan(double startTime);

	GsPlan(const std::string& s);

	/** Converts a section of a linear plan into a GsPlan.
	 *  The ground speeds correspond to the speeds in the "lcp" linear plan.
	 *  However, no route radii values are set using this constructor
	 * 
	 * @param lpc     linear plan
	 * @param start   starting index of the section to be used 
	 * @param end     ending index of the section to be used 
	 * 
	 */
	GsPlan(const Plan& lpc, int start, int end);

	/** Converts a full linear plan into a GsPlan.
	 *  The ground speeds correspond to the speeds in the "lcp" linear plan.
	 *  However, no route radii values are set using this constructor.
	 *  @param lpc     linear plan
    */
	GsPlan(const Plan& lpc);

	/** Creates a copy of a GsPlan
	 * 
	 * @param gsp ground speed plan
	 */
	GsPlan(const GsPlan& gsp);

	GsPlan(const Route& r, const std::string& name, double start, double gsAll);

	/** Create a new GsPlan from "gsp" with every segment given a ground speed of "gsNew"
	 * 
	 * @param gsp      The source GsPlan
	 * @param gsNew    the ground speed used on every segment
	 * @return a new GsPlan
	 */
	static GsPlan makeGsPlanConstant(const GsPlan& gsp, double gsNew);

	/** Create a route from a linear plan and calculate radii using "bankAngle"
	 * 
	 * @param lpc         linear plan
	 * @param bankAngle   bank angle used for turn generation
	 * @return a new GsPlan
	 */
	static GsPlan mkGsPlanBankAngle(const Plan& lpc, double bankAngle);

	int size() const;

	std::string getName() const;

	/**
	 * set aircraft id
	 * @param s string name
	 */
	void setName(const std::string& s);

	void setInfo(int i, const std::string& s);

	/**
	 * Get groundspeed
	 * @param i index
	 * @return ground speed
	 */
	double gs(int i) const;

	/** Provide a copy of the route in this GsPlan
	 * 
	 * @return route
	 */
	Route route() const;

	void add(const Position& pos, const std::string& label, const std::string& info, double gsOut, double rad);


	/**
	 * Add a position 
	 * @param pos position
	 * @param label label for point -- if this equals GsPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param info  information for a point
	 * @param gsOut ground speed out 
	 */
	void add(const Position& pos, const std::string& label, const std::string& info, double gsOut);

	/** This method is primarily added to prevent accidental use of lower level Route method
	 * 
	 *  It makes the ground speed the same as the last one, if there is a previous point, otherwise -1.0
	 * @param pos position
	 * @param label label for this point
	 * @param info info string for this point
	 * 
	 */
	void add(const Position& pos, const std::string& label, const std::string& info);

	/** add point "ix" from GsPlan "p"
	 * 
	 * @param p ground speed plan
	 * @param ix index
	 */
	void add(const GsPlan& p, int ix);


	/** Add all of the points of GsPlan "p" to this plan
	 * 
	 * @param p ground speed plan
	 */
	void addAll(const GsPlan& p);

	/**
	 * Sets a point
	 * @param ix  index of the point
	 * @param pos position
	 * @param label label for point -- if this equals GsPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param info  information for a point
	 * @param gsOut ground speed out of "ix"
	 */
	void set(int ix, const Position& pos, const std::string& label, const std::string& info, double gsOut);


	Position position(int i) const;

	Position last() const;

	std::string name(int i) const;

	std::string info(int i) const;

	double radius(int i) const;

	/** Set the radius at point "i" to be "rad"
	 *
	 * @param i          index
	 * @param rad        radius to be stored
	 */
	void setRadius(int i, double rad);

	std::vector<double> getGsInits() const;

	/** Create a new GsPlan that is a copy of this one, then add all the elements from p2 to this new GsPlan 
	 * 
	 * @param p2 point
	 * @return groundspeed
	 */
	GsPlan append(const GsPlan& p2);

	void remove(int i);

	void setGs(int i, double gsin);

	double startTime() const;

	void setStartTime(double startTime);

	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * @param nm name 
	 * @return index of name
	 */
	int findName(const std::string& nm) const;

	double pathDistance(int i, int j, bool linear) const;

	/**
	 * Calculate ETA for this GsPlan
	 * 
	 * @param linear false to include kinematic plans
	 * @return estimated final time for this plan
	 */
	double ETA(bool linear) const;

	/**
	 * Make linear plan from GsPlan.
	 * Note: in order to not generate invalid Plans, this ensures each segment has a positive groundspeed (currently &ge; 0.001 m/s)
	 * @return plan
	 */
	Plan linearPlan() const;
	
	/**
	 * Return the segment containing the point closest to the given position.
	 * @param pos position
	 * @return  segment number
	 */
	int closestSegment(const Position& pos) const;

	/** 
	 * Position at distance
	 * @param dist distance to query
	 * @param defaultBank default bank angle for turns (overridden by radius)
	 * @param linear flag to generate turns
	 * @return position at horizontal distance (does not incorporate vertical or ground speed accelerations)
	 */
	Position positionFromDistance(double dist, double defaultBank, bool linear) const;

	/** 
	 * Velocity at distance
	 * @param dist distance to query
	 * @param defaultBank default bank angle for turns (overridden by radius)
	 * @param linear flse to generate turns
	 * @return velocity at horizontal distance (does not incorporate vertical or ground speed accelerations)
	 */
	Velocity velocityFromDistance(double dist, double defaultBank, bool linear) const;
	
	/** test equality of GsPlans
	 */
	bool equals(const GsPlan& fp) const;
	
	bool almostEquals(const GsPlan& p) const;
	
	std::string toString() const;
};


}

#endif

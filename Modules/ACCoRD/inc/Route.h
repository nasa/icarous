/*
 * Copyright (c) 2017-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <string>
#include "Position.h"
#include "Plan.h"

namespace larcfm {

/** 
 * A class (and tools) for creating a Route, that is, a sequence of 3D positions.
 * Routes can be converted into Plans and vice versa.
 * 
 */
class Route {

private:
	std::vector<std::string>   names;
	std::vector<std::string>   infos;
	std::vector<Position> positions;
	std::vector<double> radius_v;                               // optional information about turn

public:
	static const std::string virtualName;

	Route();
	
	Route(const Route& gsp);

	/** Creates a route from a linear plan, all points have radius 0.0
	 * 
	 * @param lpc     linear plan
	 * @param start   starting index
	 * @param end     ending index
	 */
	Route(const Plan& lpc, int start, int end);
	
	/** Create a route from a linear plan, all points have radius 0.0
	 * 
	 * @param lpc      linear plan
	 * */
	Route(const Plan& lpc);

	/** Create a route from a linear plan and calculate radii using "radius"
	 * 
	 * @param fp       linear plan
	 * @param start    starting index
	 * @param end      ending index
	 * @param radius   this radius value is inserted at all vertex points (used in path distance calculations)
	 * @return         route generated from linear plan
	 */
	static Route mkRoute(const Plan& fp, int start, int end, double radius);

	/** Create a route from a plan fp using only the named points.   All other points are discarded
	 * 
	 * @param fp    source plan
	 * @return      route constructed from named points in plan "fp"
	 */
	static Route mkRouteNamedOnly(const Plan& fp);
	
	/** Create a route from a linear plan and make all radii have the value "radius"
	 * 
	 * @param fp       source plan
	 * @param radius   radius to be used at every vertex (for path distance calculations)
	 * @return new route
	 */
	static Route mkRoute(const Plan& fp, double radius);

	
	/** Create a route from a linear plan and calculate radii using "bankAngle"
	 * NOTE: If a radius is already stored in the source plan, this will not overwrite it
	 * 
	 * @param fp          source plan
	 * @param bankAngle   bank angle used to calculate radii values (used in path distance calculations)
	 * @return            Route generated from a linear plan fp, radii added to vertexs
	 *                    
	 */
	static Route mkRouteBankAngle(const Plan& fp, double bankAngle);

	/** Create a route which is a subset of the given route (from the starting index, to the ending index)
	 * 
	 * @param fp       source route
	 * @param start    starting index
	 * @param end      ending index
	 * @return         new route
	 */
	static Route mkRouteCut(const Route& fp, int start, int end);

	
	int size() const;
	
	Position position(int i) const;
	
	Position positionFromDistance(double dist, bool linear) const;

	Position positionFromDistance(int i, double dist, bool linear) const;
	
	std::string name(int i) const;
	
	std::string info(int i) const;

	double radius(int i) const;

	
	/**
	 * Add a position
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 * @param data  data field for a point
	 */
	void add(const Position& pos, const std::string& label, const std::string& data);
	


	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 */
	void add(const Position& pos,  const std::string& label, const std::string& data, double rad);

	/**
	 * Add a position 
	 * 
	 * @param ix    index
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 * @param data  data field for a point
	 * @param rad     radius
	 */
	void add(int ix, const Position& pos, const std::string& label, const std::string& data, double rad);
	
	void set(int ix, const Position& pos, const std::string& label, const std::string& data, double rad);

		
	void remove(int i);
	
	void removeFirst();

	
	void removeLast();

	
	void add(const Route& p, int ix);
	
	void addAll(const Route& p) ;
	
	Route append(const Route& p2);


	void updateWithDefaultRadius(double default_radius);

	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param nm      String to match
	 *  @param startIx index to begin searching
	 *  @return index of matching label
	 */
	int findName(const std::string& nm, int startIx) const;
	
	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param nm      String to match
	 *  @return index of matching label
	 */
	int findName(const std::string& nm) const;

	
	void setName(int i, const std::string& name);
	
	int findInfo(const std::string&  nm, int startIx) const;

	int findInfo(const std::string&  nm) const;

	void setInfo(int i, const std::string& data);


	void setPosition(int i, const Position& pos);
	
	void setRadius(int i, double rad);

	Route copy() const;
	
	/**
	 * Find the path distance between the given starting and ending indexes
	 * @param i    starting index
	 * @param j    ending index
	 * @param linear if true, use linear distance
	 * @return distance from i to j; returns -1 if the Route is ill-formed
	 * 
	 * Note:  the distance is from MOT to MOT if linear = false and indices are vertices
	 */
	double pathDistance(int i, int j, bool linear) const;
	
	
	double pathDistance(bool linear) const;
	
	
	double pathDistance() const;

	/**
	 * Position along the route
	 * @param dist distance to query
	 * @param gs ground speed (must be greater than zero, but otherwise only used for turn generation)
	 * @param defaultBank default bank angle for turns (overridden by radius info)
	 * @param linear true to remain linear, false to generate turns.
	 * @return Position at distance  (does not incorporate vertical or ground speed accelerations)
	 */
	Position positionFromDistance(double dist, double gs, double defaultBank, bool linear) const;

	/**
	 * Velocity along the route (primarily track)
	 * @param dist distance to query
	 * @param gs ground speed (must be greater than zero, but otherwise only used for turn generation)
	 * @param defaultBank default bank angle for turns (overridden by radius info)
	 * @param linear true to remain linear, false to generate turns.
	 * @return Velocity at distance (does not incorporate vertical or ground speed accelerations)
	 */
	Velocity velocityFromDistance(double dist, double gs, double defaultBank, bool linear) const;


	/** Create a linear plan from this route using a constant ground speed "gs".  It will copy the radius information
	 *  into the plan.
	 * 
	 * @param startTime   The time that will be assigned to point 0
	 * @param gs          ground speed to use
	 * @return            linear plan with a constant ground speed
	 */
	Plan linearPlan(double startTime, double gs) const;
	
	
	/** test equality of GsPlans
	 * @param fp another route
	 * @return true, if equal
	 */
	bool equals(const Route& fp) const;


	
	std::string toString() const;
	
	std::string toString(double startTime, double gs) const;
	
};


}

#endif

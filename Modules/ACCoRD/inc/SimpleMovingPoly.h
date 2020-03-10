/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef SIMPLEMOVINGPOLY_H_
#define SIMPLEMOVINGPOLY_H_

#include <vector>
#include <string>
#include "Position.h"
#include "EuclideanProjection.h"
#include "BoundingRectangle.h"
#include "MovingPolygon3D.h"
#include "SimplePoly.h"
#include "Velocity.h"

namespace larcfm {

/**
 * A "stateful" version of a SimplePoly that includes velocity.
 */
class SimpleMovingPoly {
  public:
	SimplePoly poly;
	std::vector<Velocity> vlist;
	bool morphingPoly;
  public:

	SimpleMovingPoly(const SimplePoly& start, const std::vector<Velocity>& vs);

	SimpleMovingPoly(const SimplePoly& start, const Velocity& v);

	SimpleMovingPoly();

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top altitude
	 */
	SimpleMovingPoly(double b, double t);

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top altitude
	 * @param units units of altitude
	 */
	SimpleMovingPoly(double b, double t, const std::string& units);

	/**
	 * Create a deep copy of a SimplePoly
	 * 
	 * @param p Source poly.
	 */
	SimpleMovingPoly(const SimpleMovingPoly& p);

	SimpleMovingPoly copy() const;

	bool addVertex(const Position& p, const Velocity& v);

	void removeVertex(int i);

	void setTop(double top);

	void setBottom(double bot);

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates.
	 * 
	 * @param p3 polygon
	 * @param proj projection
	 * @return polygon
	 */
	static SimpleMovingPoly make(const MovingPolygon3D& p3, const EuclideanProjection& proj);

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
	 * 
	 * @param p3 polygon
	 * @return polygon
	 */
	static SimpleMovingPoly make(const MovingPolygon3D& p3);

	bool isLatLon() const;

	/**
	 * Return the polygon projected to be at time dt (dt = 0 returns a copy of the base polygon)
	 * 
	 * @param dt time increment
	 * @return polygon
	 */
	SimplePoly position(double dt) const;

	/**
	 * Return the average Velocity (at time 0).
	 * @return velocity
	 */
	Velocity averageVelocity() const;

	SimpleMovingPoly linear(double dt) const;

	  /**
	   * This will return a moving polygon that starts at a time
	   * @param time start time
	   * @param proj relevant projection
	   * @return moving polygon
	   */
	MovingPolygon3D getMovingPolygon(double time, const EuclideanProjection& proj) ;

	MovingPolygon3D getMovingPolygon(const EuclideanProjection& proj) ;

	int size() const;

	/**
	 * Return true if point p is within the polygon at time dt from now.
	 * 
	 * @param p position
	 * @param dt time increment
	 * @return true, if point is in polygon
	 */
	bool contains(const Position& p, double dt) const;

//	std::vector<std::string> toStringList(int vertex, bool trkgsvs, int precision) const;

	std::string toString() const;

	std::vector<std::string> toStringList(int vertex, bool trkgsvs, int precision) const;

}; //class

}// namespace

#endif


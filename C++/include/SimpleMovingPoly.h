/*
 * Copyright (c) 2011-2016 United States Government as represented by
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
	 * @param t Top Altitude
	 */
	SimpleMovingPoly(double b, double t);

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top Altitude
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
	 */
	static SimpleMovingPoly make(const MovingPolygon3D& p3, const EuclideanProjection& proj);

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
	 */
	static SimpleMovingPoly make(const MovingPolygon3D& p3);

	bool isLatLon() const;

	/**
	 * Return the polygon projected to be at time dt (dt = 0 returns a copy of the base polygon)
	 */
	SimplePoly position(double dt) const;

	/**
	 * Return the average Velocity (at time 0).
	 */
	Velocity averageVelocity() const;

	SimpleMovingPoly linear(double dt) const;

	  /**
	   * This will return a moving polygon that starts at point i and ends at point i+1
	   * @param i
	   * @param proj
	   * @return
	   */
	MovingPolygon3D getMovingPolygon(double time, const EuclideanProjection& proj) const;

	MovingPolygon3D getMovingPolygon(const EuclideanProjection& proj) const;

	int size() const;

	/**
	 * Return true if point p is within the polygon at time dt from now.
	 */
	bool contains(const Position& p, double dt) const;

//	std::vector<std::string> toStringList(int vertex, bool trkgsvs, int precision) const;

	std::string toString() const;

}; //class

}// namespace

#endif


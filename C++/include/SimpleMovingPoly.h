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
 * A basic polygon that describes a volume.  This volume has a flat bottom and top
 * (specified as altitude values).  Points describe the cross-section area vertices in
 * a consistently clockwise (or consistently counter-clockwise) manner.  The cross-section
 * need not be convex, but an "out of order" listing of the vertices will result in edges
 * that cross, and will cause several calculations to fail (with no warning).
 *
 * A SimplePoly sets the altitude for all its points to be the _bottom_ altitude,
 * while the top is stored elsewhere as a single value.  The exact position for "top"
 * vertices is computed on demand.
 *
 * Point indices are based on the order they are added.
 *
 * Note: polygon support is experimental and the interface is subject to change!
 *
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

	SimpleMovingPoly(double b, double t);

	SimpleMovingPoly(double b, double t, const std::string& units);

	SimpleMovingPoly(const SimpleMovingPoly& p);

	SimpleMovingPoly copy() const;

	bool addVertex(const Position& p, const Velocity& v);

	void removeVertex(int i);

	void setTop(double top);

	void setBottom(double bot);

	static SimpleMovingPoly make(const MovingPolygon3D& p3, const EuclideanProjection& proj);

	static SimpleMovingPoly make(const MovingPolygon3D& p3);

	bool isLatLon() const;

	SimplePoly position(double dt) const;

	Velocity averageVelocity() const;

	SimpleMovingPoly linear(double dt) const;

	MovingPolygon3D getMovingPolygon(double time, const EuclideanProjection& proj) const;

	MovingPolygon3D getMovingPolygon(const EuclideanProjection& proj) const;

	int size() const;

	bool contains(const Position& p, double dt) const;

//	std::vector<std::string> toStringList(int vertex, bool trkgsvs, int precision) const;

	std::string toString() const;

}; //class

}// namespace

#endif


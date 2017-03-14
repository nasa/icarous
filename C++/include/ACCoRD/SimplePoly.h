/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef SIMPLEPOLY_H_
#define SIMPLEPOLY_H_

#include <vector>
#include <string>
#include "Position.h"
#include "EuclideanProjection.h"
#include "BoundingRectangle.h"
#include "ErrorLog.h"
#include "Poly3D.h"

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
 * The cross-section must be a simple polygon, that is it allows for non-convex areas, but
 * vertices and edges may not overlap or cross.  Vertices may be ordered in either a clockwise
 * or counterclockwise manner.
 * 
 * (A vertex-complete polygon allows for vertices and edges to overlap but not cross, while
 * a general polygon allows for edges to cross.)
 * 
 * Point indices are based on the order they are added.
 * 
 * Note: polygon support is experimental and the interface is subject to change!
 *
 */
class SimplePoly {
  private:
	std::vector<Position> points;
	bool bottomTopSet;
	double top;
	double bottom;
	
	mutable bool boundingCircleDefined;
	mutable bool centroidDefined;
	mutable bool averagePointDefined;
	
	mutable Position cPos;
	mutable Position bPos;
	mutable Position aPos;
    
	mutable double maxRad;
	mutable double bRad;

	mutable double clockwiseSum;

    void init();
    
    void calcCentroid() const;
//    void calcBoundingCircleCenter() const;
    Position maxDistPair(const Position& p) const;

	/**
	 * Return the area (in m^2 or rad^2) of this SimplePoly.
	 */
	double signedArea(double dx, double dy) const;
	
  public:

	/**
	 * Constructs an empty SimplePoly.
	 */
	SimplePoly();

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top Altitude
	 */
	SimplePoly(double b, double t);

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 *
	 * @param b Bottom altitude
	 * @param t Top Altitude
	 */
	SimplePoly(double b, double t, const std::string& units);

	/**
	 * Create a deep copy of a SimplePoly
	 * 
	 * @param p Source poly.
	 */
	SimplePoly(const SimplePoly& p);


	bool equals(const SimplePoly& p) const;

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates .
	 */
    static SimplePoly make(const Poly3D& p3);

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates .
	 */
    static SimplePoly make(const Poly3D& p3, const EuclideanProjection& proj);

    bool isClockwise() const;

	/**
	 *  
	 * @return True if this SimplePoly contains any geodetic points.
	 */
	bool isLatLon() const;

	/**
	 * 
	 * @return Number of points in this SimplePoly
	 */
	int size() const;

	
		
	/**
	 * Return this centroid of this volume.
	 * Note: if sides are small (less than about 10^-5 in internal units), there may be errors in the centroid calculations
	 * @return The centroid position of this volume.
	 */
	Position centroid() const;
	
  private:

	Position avgPos(const std::vector<Position>& points, const std::vector<double>& wgts) const;

  public:

	/**
	 * Return the average of all vertices.  Note this is not the same as the centroid!  This will, however, have the nice property of having
	 * a constant velocity when dealing with a morphing polygon. 
	 */
	Position averagePoint() const;

    double apBoundingRadius();


	/**
	 * Returns the center of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
	 */
	Position boundingCircleCenter() const;
	
	/** Returns true if this polygon is convex */
	  bool isConvex();

	/**
	 * Returns the radius of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
	 */
	double boundingCircleRadius() const;
	
	/** 
	 * Returns the max horizontal distance between any vertex and the centroid
	 */
	double maxRadius() const;

	/** 
	 * Add a new point to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner. 
	 * This currently does NOT set the Z component of the point (unless it is the first point)
	 * Returns false if an error is detected (duplicate or invalid vertex), true otherwise. 
	 */
	bool addVertex(const Position& p);


	/**
	 * Remove a point from this SimplePoly.
	 * @param n Index (in order added) of the point to be removed.
	 */
	void remove(int n);

	/**
	 * Change the position of a point already added to the SimplePoly, indicated by its index. 
	 * This currently does NOT set the Z component of the point.
	 * Returns false if an invalid vertex is detected, true otherwise 
	 */
	bool setVertex(int n, Position p);

	/**
	 * Sets the top altitude of this SimplePoly.
	 * @param t New top altitude.
	 */
	void setTop(double t);

	/**
	 * Return the top altitude of this SimplePoly.
	 */
	double getTop() const;

	/**
	 * Sets the bottom altitude of this SimplePoly.
	 * @param b New bottom altitude.
	 */
	void setBottom(double b);

	/**
	 * Return the bottom altitude of this SimplePoly.
	 */
	double getBottom() const;

	/**
	 * Returns the position of the (bottom) point with index n.
	 * If n is not a valid index, this returns the centroid position.
	 */
	Position getVertex(int n) const;

	std::vector<Position> getVertices() const;

	/**
	 * Returns the position of the top point with index n.
	 * If n is not a valid index, this returns the centroid position.
	 */
	Position getTopPoint(int n) const;

	/**
	 * Returns a deep copy of this SimplPoly.
	 */
	SimplePoly copy() const;

	/** return a aPolygon3D version of this.  proj is ignored if this is Euclidean */
	Poly3D poly3D(const EuclideanProjection& proj) const;

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	bool contains(const Position& p) const;

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	bool contains2D(const Position& p) const;

	/**
	 * This moves the SimplePoly by the amount determined by the given (Euclidean) offset.
	 * @param off offset
	 */
	void translate(const Vect3& off);

	SimplePoly linear(const Velocity& v, double t) const;

	bool validate();

	bool validate(ErrorLog* error);

	BoundingRectangle getBoundingRectangle() const;

	int maxInRange(const Position& p, double a1, double a2) const;

	/**
	 * Return the angle that is perpendicular to the middle of the edge from vertex i to i+1, facing outward.
	 * Return NaN if i is out of bounds or vertex i overlaps vertex i+1. 
	 */
	double perpSide(int i) const;

  private:
	bool vertexConvex(const Position& p0, const Position& p1, const Position& p2) const;

  public:
	/**
	 * Return the internal angle of vertex i.
	 * Return NaN if i is out of bounds or vertex i overlaps with an adjacent vertex. 
	 */
	double vertexAngle(int i) const;

	/**
	 * String representation of this SimplePoly.
	 */
	std::string toString() const;

	std::vector<std::string> toStringList(int vertex, int precision) const;


}; //class

}// namespace

#endif

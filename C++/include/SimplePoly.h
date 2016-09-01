/*
 * Copyright (c) 2011-2016 United States Government as represented by
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

    void init();
    
    void calcCentroid() const;
//    void calcBoundingCircleCenter() const;
    Position maxDistPair(const Position& p) const;

	/**
	 * Return the area (in m^2) of this SimplePoly.
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

    static SimplePoly make(const Poly3D& p3, const EuclideanProjection& proj);


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
	 * @return The centroid position of this volume.
	 */
	Position centroid() const;
	
	Position averagePoint() const;

    double apBoundingRadius();


	/**
	 * Returns the center of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
	 */
	Position boundingCircleCenter() const;
	
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

	/**
	 * Returns the position of the top point with index n.
	 * If n is not a valid index, this returns the centroid position.
	 */
	Position getTopPoint(int n) const;

	/**
	 * Returns a deep copy of this SimplPoly.
	 */
	SimplePoly copy() const;

	/** return a aPolygon3D version of this */
	Poly3D poly3D(const EuclideanProjection& proj) const;

	/**
	 * This moves the SimplePoly by the amount determined by the given (Euclidean) offset.
	 * @param off offset
	 */
	void translate(const Vect3& off);

	SimplePoly linear(const Velocity& v, double t) const;

	bool validate();

	BoundingRectangle getBoundingRectangle() const;

	/**
	 * String representation of this SimplePoly.
	 */
	std::string toString() const;

}; //class

}// namespace

#endif

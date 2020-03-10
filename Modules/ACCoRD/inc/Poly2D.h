/*
 * Poly2D.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef POLY2D_H_
#define POLY2D_H_

//#include "BoundingRectangle.h"
#include <vector>
#include "Vect2.h"
//#include "GeneralPath.h"

namespace larcfm {

/**
 * Encapsulates a geometric polygon. The polygon is defined in terms of its vertex coordinates. 
 * This implementation assumes a simply connected polygon. The Polygon is otherwise quite general 
 * allowing multiply connected regions. The class provides a containment test for points and uses 
 * bounding rectangles to speed up computations.  
 * 
 * Note: It does not use the "BoundingRectangle" class to avoid importing Lat/Lon classes such as GreatCircle.
 */
class Poly2D {

private:
    std::vector<Vect2> vertices;
//	BoundingRectangle boundingRect;
//	bool boundingRectangleDefined;
	static const double vertexPerturb;
	static const double perturbAmount;
	double minX;
	double minY;
	double maxX;
	double maxY;

public:

	Poly2D();

	explicit Poly2D(const std::vector<Vect2>& verts);

	/**
	 * Constructor for creating a copy of a Polygon
	 *
	 * @param polygon The polygon to be copied
	 */
	Poly2D(const Poly2D& polygon);

//	static Poly2D copy(const Poly2D& polygon);


	int size() const;

	Poly2D linear(const Vect2& v, double t) const;

	Poly2D linear(const std::vector<Vect2>& v, double t) const;

	void add(double x, double y);

	void add(const Vect2& v);

	void insert(int i, const Vect2& v);

	/**
	 * Return vertex, or INVALID if out of bounds.
	 * @param i index
	 * @return vertex
	 */
	Vect2 get(int i) const;


	/**
	 * Determines if the supplied point lies within this polygon. Uses the "crossing number" algorithm
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * 
	 * @param i index
	 * @param v vector
	 */

	void set(int i, const Vect2& v);

	/**
	 * Remove a point from this SimplePolyNew.
	 * @param n Index (in order added) of the point to be removed.
	 */
	void remove(int n);

	/**
	 * Determines if the supplied point lies within this polygon. Uses the "crossing number" algorithm
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD 
	 * detection algorithms.
	 * 
	 * @param a - x coordinate of the point
	 * @param b - y coordinate of the point
	 * @return true if the Polygon contains the point (a, b), otherwise false
	 */
	bool contains(double a, double b) const;

	bool contains(const Vect2& v) const;


	std::vector<Vect2> getVerticesRef() const;

//	BoundingRectangle getBoundingRectangle() const;
//
//	void setBoundingRectangle(BoundingRectangle boundingRectangle);


	/**
	 * Planar geometric area of a <b>simple</b> polygon. It is positive if the
	 * vertices are ordered counterclockwise and negative if clockwise.
	 *
	 * @return the signed area of the polygon
	 */
	double signedArea() const ;

	/**
	 * Return the horizontal area (in m^2) of this Poly3D.
	 * @return area
	 */
	double area() const;   // not const !!

	/**
	 * @return the geometric centroid.
	 */
	Vect2 centroid() const;

	/**
	 * Return the average of all vertices.  Note this is not the same as the centroid, and will be weighted 
	 * towards concentrations of vertices instead of concentrations of area/mass.  This will, however, have the nice property of having
	 * a constant linear velocity between two polygons, even if they morph shape. 
	 * 
	 * @return point
	 */
	Vect2 averagePoint() const;

	// area and centroid courtesy of Paul Bourke (1988) http://paulbourke.net/geometry/polyarea/
	// these are for non self-intersecting polygons
	// this calculation assumes that point 0 = point n (or the start point is counted twice)
	/**
	 * Return the horizontal area (in m^2) of this Poly3D.
	 */
	//double area() const;

	double innerDiameter() const;

	double outerDiameter() const;

	bool isClockwise() const;

	/**
	 * Reverse order of vertices
	 * @return polygon
	 */
	Poly2D reverseOrder() const;

	/**
	 * Distance from averagePoint to farthest vertex
	 * @return radius
	 */
	double apBoundingRadius() const;

	void recalcBoundingRectangle();

	/**
	 * Returns a GeneralPath representation for testing purposes.
	 *
	 * @return this polygon as a GeneralPath
	 */
	//GeneralPath getGeneralPath() ;

	std::string strVectArray(const std::vector<Vect2>& verts) const ;

	std::string toString() const;

};


}

#endif /* POLY2D_H_ */

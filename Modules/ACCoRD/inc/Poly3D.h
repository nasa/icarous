/*
 * Poly3D.h
 * 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef POLY3D_H_
#define POLY3D_H_
#include <vector>

#include "Poly2D.h"
#include "Vect2.h"
#include "Vect3.h"
#include "BoundingRectangle.h"

namespace larcfm {

/**
 * Euclidean space representation of polygon with height info.
 * This extends Vect3 (temporarily) in order to take advantage to polymorphism of detection algorithms.
 * If treated as a normal Vect3 point, this ignores the polygon information.
 * Only functions that are aware of Poly3Ds will have access to its full definition.
 */
class Poly3D {

private:

	Poly2D p2d;
	double top;
	double bottom; ;

public:
	void calcCentroid() ;

	Poly3D();

	Poly3D(double b, double t);


	Poly3D(const Poly2D& v, double b, double t);

//	Poly3D(const Vect3& v);

	Poly3D(const Poly3D& p);

	Poly2D poly2D() const;

	bool equals(const Poly3D& p) const;

	void add(const Vect2& v);

	void insert(int i, const Vect2& v);

	Vect2 get2D(int i) const ;

	void set(int i,const Vect2& v);

	/**
	 * Remove a point from this SimplePolyNew.
	 * @param n Index (in order added) of the point to be removed.
	 */
	void remove(int n);

	int size() const;

	double getTop() const ;

	void setTop(double t);

	double getBottom() const ;

	void setBottom(double b) ;

	const std::vector<Vect2> getVerticesRef() const;

	Vect3 centroid() const ;

	/**
	 * Return the average of all vertices.  Note this is not the same as the centroid, and will be weighted 
	 * towards concentrations of vertices instead of concentrations of area/mass.  This will, however, have the nice 
	 * property of having a constant linear velocity between two polygons, even if they morph shape. 
	 * 
	 * @return point
	 */
	Vect3 averagePoint() const ;

	/** Returns true if this polygon is convex 
	 * @return true, if convex
	 */
	bool isConvex() const;

	/**
	 * Reverses the order of the vertices
	 * @return polygon
	 */
	Poly3D reverseOrder() const;

	std::string toString() ;

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * @param v vector
	 * @return true if contains
	 */
	bool contains(const Vect3& v) const;

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * @param v vector
	 * @return true if contains
	 */
	bool contains2D(const Vect2& v) const;

    double area() const;

	/** Creates a bounding rectangle from the vertices of the polygon.
	 * 
	 * @return distance
	 */
	BoundingRectangle getBoundingRectangle() const;

};


}

#endif /* Poly3D_H_ */

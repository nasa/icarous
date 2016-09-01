/*
 * Poly2D.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
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

	Poly2D(const std::vector<Vect2>& verts);

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

	void addVertex(double x, double y);

	void addVertex(const Vect2& v);


	/**
	 * Determines if the supplied point lies within this polygon. Uses the "crossing number" algorithm
	 *
	 * @param a - x coordinate of the point
	 * @param b - y coordinate of the point
	 * @return true if the Polygon contains the point (a, b), otherwise false
	 */
	bool contains(double a, double b);

	bool contains(const Vect2& v);

	void setVertex(int i, const Vect2& v);

	Vect2 getVertex(int i) const;

	std::vector<Vect2> getVertices() const;

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

	double area();   // not const !!

	/**
	 * @return the geometric centroid.
	 */
	Vect2 centroid() const;

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

	double apBoundingRadius() const;


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

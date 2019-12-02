/*
 * Poly3DLL.h
 * 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef Poly3DLL_H_
#define Poly3DLL_H_
#include <vector>

#include "Poly2DLLCore.h"
#include "Vect2.h"
#include "Vect3.h"
#include "BoundingRectangle.h"

namespace larcfm {

/**
 * Euclidean space representation of polygon with height info.
 * This extends Vect3 (temporarily) in order to take advantage to polymorphism of detection algorithms.
 * If treated as a normal Vect3 point, this ignores the polygon information.
 * Only functions that are aware of Poly3DLLs will have access to its full definition.
 */
class Poly3DLL {

private:

	std::vector<LatLonAlt> vertices;         // The altitude is superfluous -- should be ignored
	BoundingRectangle boundingRect;
	bool boundingRectangleDefined;

	double top;
	double bottom;
	Vect3 cpos;
	LatLonAlt lastAdd;

public:

	Poly3DLL();
	Poly3DLL(double b, double t);


	Poly3DLL(const std::vector<LatLonAlt>& verts, double b, double t);

//	Poly3DLL(Vect3 v) {
//		//super(v.x,v.y,v.z);
//		p2d = new Poly2DLL();
//	}

	Poly3DLL(const Poly3DLL& p) ;


	void add(const LatLonAlt& lla);

	void add(double lat, double lon, double alt);


	void insert(int i, const LatLonAlt& lla);

	/**
	 * Change the position of a point already added to the SimplePolyNew, indicated by its index.
	 * This currently does NOT set the Z component of the point.
	 *
	 * @param n index
	 * @param p position
	 * @return false, if an invalid vertex is detected, true otherwise
	 */
	bool set(int n, const LatLonAlt& p);



	LatLonAlt get(int i) const;

	/**
	 * Remove a point from this SimplePolyNew.
	 * @param n Index (in order added) of the point to be removed.
	 */
	void remove(int n) ;

	int size() const;

	double getTop() const;

	void setTop(double t);

	double getBottom() const;

	void setBottom(double b) ;

	bool isClockwise();

	/**
	 * Note that the calculations used here are Euclidean, and only return the approximate centroid for polygons on a sphere.
	 * @return the geometric centroid.
	 */
	LatLonAlt centroid();

	LatLonAlt averagePoint();

	void recalcBoundingRectangle();

	std::string toString() const ;


	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * @param lla point
	 * @return true if contains
	 */
	bool contains(const LatLonAlt& lla);


	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * @param lla point
	 * @return true if contains
	 */
	bool contains2D(const LatLonAlt& lla);



    bool equals(const Poly3DLL& p) const;

	/** Returns true if this polygon is convex
	 * @return true, if convex
	 */
	bool isConvex() const;


	Poly3DLL reverseOrder();

	double sumAngles(int dir) const;

	/**
	 * This returns the approximate area of the polyon in square meters.
	 * Calculation for area is a standard formula extending Girard's Theorem for the area of a triangle on a sphere (A = (E-PI)*R^2)
	 * Considering this is based on the angular excess of the triangles in question, and polygons are generally very small compared 
	 * to the radius of the Earth, this is very sensitive to minor inaccuracies in the interior angle calculations.
	 * @return area
	 */
	double area();

	/**
	 * Planar geometric area of a <b>simple</b> polygon. It is positive if the
	 * vertices are ordered counterclockwise and negative if clockwise.
	 * This returns the area in square radians (used for centroid calculation)
	 * Note that the area calculations used here are Euclidean, and do not return the actual area occupied by the polygon.
	 * 
	 * @return the signed area of the polygon
	 */
	double signedArea();

	BoundingRectangle getBoundingRectangle();

	const std::vector<LatLonAlt>& getVerticesRef() const;

};


}

#endif /* Poly3DLL_H_ */

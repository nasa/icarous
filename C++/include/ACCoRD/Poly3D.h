/*
 * Poly3D.h
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
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

	Poly3D(const Poly2D& v, double b, double t);

//	Poly3D(const Vect3& v);

	Poly3D(const Poly3D& p);

	Poly2D poly2D() const;

	void addVertex(const Vect2& v);

	Vect2 getVertex(int i) const ;

	int size() const;

	double getTop() const ;

	void setTop(double t);

	double getBottom()const ;

	void setBottom(double b) ;

	Vect3 centroid() const ;

	Vect3 averagePoint() const ;

	std::string toString() ;

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	bool contains(const Vect3& v) const;


};


}

#endif /* Poly3D_H_ */

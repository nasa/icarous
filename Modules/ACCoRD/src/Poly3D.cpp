/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Poly3D.h"

#include <vector>
#include "format.h"

namespace larcfm {

	
	Poly3D::Poly3D() {
		//super(x, y, z);
		p2d = Poly2D();
		top = 0;
		bottom = 0;
	}

	Poly3D::Poly3D(const Poly2D& v, double b, double t) {
		//super(v.x, v.y, (t+b)/2.0);
		p2d = Poly2D(v);
		top = t;
		bottom = b;
	}

//	Poly3D::Poly3D(const Vect3& v) {
//		//super(v.x,v.y,v.z);
//		p2d = Poly2D();
//		top = 0;
//		bottom = 0;
//	}
	
	Poly3D::Poly3D(const Poly3D& p) {
		//super(p.x,p.y,p.z);
		p2d = Poly2D(p.p2d);
		top = p.top;
		bottom = p.bottom;
	}

	Poly2D Poly3D::poly2D() const {
		return p2d;
	}

	void Poly3D::addVertex(const Vect2& v) {
		p2d.addVertex(v);		
	}
	
	Vect2 Poly3D::getVertex(int i) const {
		return p2d.getVertex(i);
	}

	int Poly3D::size() const {
		return p2d.size();
	}

	double Poly3D::getTop() const {
		return top;
	}

	void Poly3D::setTop(double t) {
		top = t;
	}

	double Poly3D::getBottom()const  {
		return bottom;
	}

	void Poly3D::setBottom(double b) {
		bottom = b;
	}

	Vect3 Poly3D::centroid() const {
		return Vect3(p2d.centroid(), (top-bottom)/2);
	}

	Vect3 Poly3D::averagePoint() const {
		return Vect3(p2d.averagePoint(), (top-bottom)/2);
	}

//	// Overwriting all Vect3 functions that make Vect3s...
//
//	static final aPolygon3D ZERO = aPolygon3D(0,0,0);
//
//	/** An invalid Vect3.  Note that this is not necessarily equal to other invalid Vect3s -- use the isInvalid() test instead. */
//	static final aPolygon3D INVALID = aPolygon3D(Double.NaN,Double.NaN,Double.NaN);
//
//
//	static aPolygon3D makeXYZ(double x, double y, double z) {
//		return aPolygon3D(Units::from("NM",x),Units::from("NM",y),Units::from("ft",z));
//	}
//
//	static aPolygon3D mkXYZ(double x, double y, double z) {
//		return aPolygon3D(x,y,z);
//	}


	std::string Poly3D::toString() {
		return /*super.toString()+" "+*/ p2d.toString()+" bot="+Fm2(bottom)+" top="+Fm2(top);
	}
	
	bool Poly3D::contains(const Vect3& v) const {
		if (v.z > top || v.z < bottom) return false;
		return p2d.contains(v.vect2());
	}

	
}

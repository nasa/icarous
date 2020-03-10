/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Poly3D.h"

#include <vector>
#include "format.h"
#include "EuclideanProjection.h"

namespace larcfm {

	
	Poly3D::Poly3D() {
		//super(x, y, z);
		p2d = Poly2D();
		top = 0;
		bottom = 0;
	}

	Poly3D::Poly3D(double b, double t) {
		//super(v.x, v.y, (t+b)/2.0);
		p2d = Poly2D();
		top = t;
		bottom = b;
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

    bool Poly3D::equals(const Poly3D& p) const {
		bool ret = (bottom == p.bottom) && (top == p.top);
		if (size() != p.size()) return false;
		for (int i = 0; i < size(); i++) {
			if ( ! (get2D(i) == p.get2D(i))) return false;
		}
		return ret;
	}


	void Poly3D::add(const Vect2& v) {
		p2d.add(v);		
	}
	
	void Poly3D::insert(int i, const Vect2& v) {
		p2d.insert(i,v);
	}


	Vect2 Poly3D::get2D(int i) const {
		return p2d.get(i);
	}

	void Poly3D::set(int i,const Vect2& v) {
		p2d.set(i,v);
	}


	/**
	 * Remove a point from this SimplePolyNew.
	 * @param n Index (in order added) of the point to be removed.
	 */
	void Poly3D::remove(int n) {
		p2d.remove(n);
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

	const std::vector<Vect2> Poly3D::getVerticesRef() const {
		//fpln(" $$$$$$$$$$$$$$$$ (Poly3D) : vertices = "+Fm0( p2d.getVertices().size()));
		return p2d.getVerticesRef();
	}


	Vect3 Poly3D::centroid() const {
		return Vect3(p2d.centroid(), (top+bottom)/2);
	}

	Vect3 Poly3D::averagePoint() const {
		return Vect3(p2d.averagePoint(), (top+bottom)/2);


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


	/** Returns true if this polygon is convex
	 * @return true, if convex
	 */
	bool Poly3D::isConvex() const {
		bool ret = true;
		std::vector<Vect2> points = p2d.getVerticesRef();
		if (points.size() > 2) {
			double a1 = points[points.size()-1].compassAngle(points[0]);
			double a2 = points[0].compassAngle(points[1]);
			int pdir = Util::turnDir(a1, a2);
			for (int i = 0; i < (int) points.size()-2; i++) {
				a1 = a2;
				a2 = points[i+1].compassAngle(points[i+2]);
				int dir = Util::turnDir(a1, a2);;
				ret = ret && (pdir == 0 || dir == pdir);
				if (dir != 0) pdir = dir;
			}
		}
		return ret;
	}

	Poly3D Poly3D::reverseOrder() const {
		return Poly3D(p2d.reverseOrder(), bottom, top);
	}


	double Poly3D::area() const {
		return p2d.area();
	}

	std::string Poly3D::toString() {
		return /*super.toString()+" "+*/ p2d.toString()+" bot="+Fm2(bottom)+" top="+Fm2(top);
	}
	
	bool Poly3D::contains(const Vect3& v) const {
		if (v.z > top || v.z < bottom) return false;
		return p2d.contains(v.vect2());
	}

	bool Poly3D::contains2D(const Vect2& v) const {
		return p2d.contains(v);
	}


	BoundingRectangle Poly3D::getBoundingRectangle() const {
		return BoundingRectangle(p2d.getVerticesRef());
	}

}

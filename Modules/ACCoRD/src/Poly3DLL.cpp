/*
 * Copyright (c) 2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Poly3DLL.h"
#include "LatLonAlt.h"
#include "Pair.h"
#include "Vect3.h"
#include "GreatCircle.h"

namespace larcfm {

//	double Poly3DLL::top = 0;
//	double Poly3DLL::bottom = 0;
//	Vect3  Poly3DLL::cpos = Vect3::ZERO(); // hide the final version!
	
	 Poly3DLL::Poly3DLL() {
		//super(x, y, z);
		//p2d = new Poly2DLL();
		vertices.resize(0); // = new std::vector<LatLonAlt>(0);
		top = 0;
        bottom = 0;
        cpos = Vect3::ZERO();
        lastAdd = LatLonAlt::INVALID();
        boundingRectangleDefined = false;
	}
	
	 Poly3DLL::Poly3DLL(double b, double t) {
		//super(x, y, z);
		vertices.resize(0); // = new std::vector<LatLonAlt>(0);
		bottom = b;
		top = t;
		cpos = Vect3::ZERO();
		lastAdd = LatLonAlt::INVALID();
        boundingRectangleDefined = false;
	}


	 Poly3DLL::Poly3DLL(const std::vector<LatLonAlt>& verts, double b, double t) {
		//super(v.x, v.y, (t+b)/2.0);
		//p2d = new Poly2DLL(v);
		vertices.resize(0); // = new std::vector<LatLonAlt>(verts.size());
		for (int i = 0 ; i < (int) verts.size() ; i++) {
			//vertices.set(i,new LatLonAlt(polygon.vertices[i].lat(),polygon.vertices[i].lon()));
			vertices.push_back(LatLonAlt::mk(verts[i]));
		}
        boundingRectangleDefined = false;
		top = t;
		bottom = b;
		cpos = Vect3::ZERO();
		lastAdd = LatLonAlt::INVALID();
	}

//	Poly3DLL(Vect3 v) {
//		//super(v.x,v.y,v.z);
//		p2d = new Poly2DLL();
//	}
	
	 Poly3DLL::Poly3DLL(const Poly3DLL& p) {
		vertices.resize(0);
		for (int i = 0; i < (int) p.vertices.size(); i++) {  // copy
			//vertices.set(i,new LatLonAlt(verts[i].lat(),verts[i].lon()));
			vertices.push_back(LatLonAlt::mk(p.vertices[i]));
		}
        boundingRectangleDefined = false;
		top = p.top;
		bottom = p.bottom;
		cpos = p.cpos;
		lastAdd = p.lastAdd;
	}

//	Poly2DLL Poly2DLL() {
//		return p2d;
//	}

	void Poly3DLL::add(const LatLonAlt& lla) {
		vertices.push_back(lla);
		if (lastAdd.isInvalid()) {
			lastAdd = lla;
		}
        boundingRectangleDefined = false;
	}
	
	void Poly3DLL::add(double lat, double lon, double alt) {
		LatLonAlt lla = LatLonAlt::mk(lat,lon,alt);
		add(lla);
	}


	void Poly3DLL::insert(int i, const LatLonAlt& lla) {
		if (i >= 0 && i < size()) {
			vertices.insert(vertices.begin()+i, lla);
			if (i > 0) boundingRect.add(vertices[i-1],lla);
			if (i < size()-1) boundingRect.add(lla,vertices[i+1]);
	        boundingRectangleDefined = false;
		}
	}


//	void set(int i, Vect2 v) {
//		centroidDefined = false;
//		p2d.setVertex(i,v);
//	}
//
	
	/**
	 * Change the position of a point already added to the SimplePolyNew, indicated by its index. 
	 * This currently does NOT set the Z component of the point.
	 * 
	 * @param n index
	 * @param p position
	 * @return false, if an invalid vertex is detected, true otherwise 
	 */
	bool Poly3DLL::set(int n, const LatLonAlt& p) {
		if (p.isInvalid() || contains2D(p)) {
			// error -- no duplicate points allowed
			return false;
		}
		if (n >= 0 && n < (int) vertices.size()) {
			vertices[n] = p;
		} 
		recalcBoundingRectangle();
		return true;	
	}

	
	
	
	LatLonAlt Poly3DLL::get(int i) const {
//		return p2d.get(i);
		if (i < 0 || i >= (int) vertices.size()) {
//			fpln("Poly2DLL.getVertex index out of bounds: "+i);
			return LatLonAlt::INVALID();
		} else {
			return vertices[i];
		}
	}
	
	/**
	 * Remove a point from this SimplePolyNew.
	 * @param n Index (in order added) of the point to be removed.
	 */
	void Poly3DLL::remove(int n) {
		if (n >= 0 && n < (int) vertices.size()) {
			//vertices.remove(n);
			vertices.erase(vertices.begin()+n);
		} 
		recalcBoundingRectangle();
	}

	int Poly3DLL::size() const {
		return vertices.size();
	}

	double Poly3DLL::getTop() const{
		return top;
	}

	void Poly3DLL::setTop(double t) {
//		centroidDefined = false;
		top = t;
	}

	double Poly3DLL::getBottom() const{
		return bottom;
	}

	void Poly3DLL::setBottom(double b) {
//		centroidDefined = false;
		bottom = b;
	}

	bool Poly3DLL::isClockwise() {
		if (size() < 3) return false;
		double sum = 0.0;
		double x0 = vertices[size()-1].lon();
		double y0 = vertices[size()-1].lat();
		BoundingRectangle br = getBoundingRectangle();
		for (int i = 0; i < size(); i++) {
			double x1 = br.fixLonWrap(vertices[i]).lon(); // in case of wraparound
			double y1 = vertices[i].lat();
			sum +=  (x1-x0)*(y1+y0);
			x0 = x1;
			y0 = y1;
		}
		return sum > 0;
	}


	/**
	 * @return the geometric centroid.
	 */
	LatLonAlt Poly3DLL::centroid() {
		double altAvg = (top+bottom)/2;
		if (size() == 1) {
			return vertices[0];
		} else if (size() == 2) {
			LatLonAlt v0 = vertices[0];
			LatLonAlt v1 = vertices[1];
			return GreatCircle::interpolate(v0,v1,0.5).mkAlt(altAvg);
		}
		getBoundingRectangle();
		double minLat = boundingRect.getMinLat();
		double maxLat = boundingRect.getMaxLat();
		double minLon = boundingRect.getMinLon();
		double maxLon = boundingRect.getMaxLon();
		double dLat = (minLat+maxLat)/2.0;
		double dLon = (minLon+maxLon)/2.0;
		double temp = 0;
		double tempX = 0;
		double tempY = 0;
		int size = vertices.size();
		if (size < 3) return LatLonAlt::INVALID();
		if (vertices[0].lat() == vertices[size - 1].lat() && vertices[0].lon() == vertices[size - 1].lon()) {
			size--;
		}
		for (int i = 0 ; i < size - 1 ; ++i) {
			double x0 = vertices[i].lat() - dLat;
			double y0 = vertices[i].lon() - dLon;
			double x1 = vertices[i+1].lat() - dLat;
			double y1 = vertices[i+1].lon() - dLon;
			temp = x0 * y1 - x1 * y0;
			tempX += (x0 + x1) * temp;
			tempY += (y0 + y1) * temp;
		}
		double x0 = vertices[0].lat() - dLat;
		double y0 = vertices[0].lon() - dLon;
		double x1 = vertices[size - 1].lat() - dLat;
		double y1 = vertices[size - 1].lon() - dLon;

		temp = x1 * y0 - x0 * y1;
		tempX += (x1 + x0) * temp;
		tempY += (y1 + y0) * temp;
		double area6 = 6 * signedArea();
		tempX /= area6;
		tempY /= area6;
		return LatLonAlt::mk(tempX+dLat, tempY+dLon, altAvg);
	}


	LatLonAlt Poly3DLL::averagePoint(){
		getBoundingRectangle();
		double avgAlt = (top+bottom)/2;
		double minLat = boundingRect.getMinLat();
		double maxLat = boundingRect.getMaxLat();
		double minLon = boundingRect.getMinLon();
		double maxLon = boundingRect.getMaxLon();
		double dLat = (minLat+maxLat)/2.0;
		double dLon = (minLon+maxLon)/2.0;
		double x = 0;
		double y = 0;
		for (int i = 0; i < (int) vertices.size(); i++) {
			x += vertices[i].lat()-dLat;
			y += vertices[i].lon()-dLon;
		}
		return LatLonAlt::mk(dLat+x/vertices.size(), dLon+y/vertices.size(), avgAlt);
	}


	void Poly3DLL::recalcBoundingRectangle() {
        boundingRectangleDefined = false;
	}

	
	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	bool Poly3DLL::contains(const LatLonAlt& lla) {
		if (lla.alt() > top || lla.alt() < bottom) return false;
		//return p2d.contains(v.vect2());
		bool isInside = contains2D(lla);
		return isInside;
	}
	
	
	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	bool Poly3DLL::contains2D(const LatLonAlt& lla) {
		//return p2d.contains(v.vect2());
		getBoundingRectangle();
		if (!boundingRect.contains(lla)) {
			return false;
		}
		double BUFF = 1E-12;
		bool isInside = Poly2DLLCore::spherical_inside(vertices, lla, BUFF);
		//fpln(" $$$$ contains: isInside = "+isInside);
		return isInside;
	}


//	/**
//	 * Return true if p is a subset of this polygon.
//	 * This uses standard raycasting checks for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
//	 */
//	bool contains(const Poly3DLL& p) {
//		if (p.top > top || p.bottom < bottom) return false;
//		return p2d.contains(p.Poly2DLL());
//	}
	
//	
//	double distanceFromEdge(Vect3 v3) {
//			Vect3 cl = VectFuns::closestPointOnSegment(Vect3(get(size()-1),bottom), Vect3(get(0), bottom), v3);
//			double dist = v3.distanceH(cl);
//			for (int i = 0; i < size()-1; i++) {
//				cl = VectFuns::closestPointOnSegment(Vect3(get(i),bottom), Vect3(get(i+1),bottom), v3);
//				dist = Util::min(v3.distanceH(cl), dist);				
//			}
//			return dist;
//	}
	
    bool Poly3DLL::equals(const Poly3DLL& p) const {
		bool ret = (bottom == p.bottom) && (top == p.top);
		if (size() != p.size()) return false;
		for (int i = 0; i < size(); i++) {
			if (!get(i).equals(p.get(i))) return false; 
		}
		return ret;
	}

	/** Returns true if this polygon is convex 
	 * @return true, if convex
	 */
	bool Poly3DLL::isConvex() const {
		bool ret = true;
		if (vertices.size() > 2) {
			double a1 = vertices[vertices.size()-1].track(vertices[0]);
			double a2 = vertices[0].track(vertices[1]);
			int pdir = Util::turnDir(a1, a2); 
			for (int i = 0; i < (int) vertices.size()-2; i++) {
				a1 = a2;
				a2 = vertices[i+1].track(vertices[i+2]);
				int dir = Util::turnDir(a1, a2);;
				ret = ret && (pdir == 0 || dir == pdir);
				if (dir != 0) pdir = dir;
			}
		}
		return ret;
	}

	
	Poly3DLL Poly3DLL::reverseOrder() {
		std::vector<LatLonAlt> np; // std::vector<LatLonAlt>();
		for (int i = size()-1; i >= 0; i--) {
			np.push_back(vertices[i]);
		}
		return Poly3DLL(np, bottom, top);
	}
	
//	std::string toString(std::string xunit, std::string yunit, std::string zunit) {
//		return /*super.toStringUnits(xunit,yunit,zunit)+" "+*/ p2d.toString(xunit,yunit)+" bot="+Units::to(zunit, bottom)+" top="+Units::to(zunit, top);
//	}

//	/** 
//	 * This parses a space or comma-separated string as a Vect3 (an inverse to the tostd::string method).  If three 
//	 * bare values are present, then it is interpreted as internal units.
//	 * If there are 3 value/unit pairs then each values is interpreted wrt the appropriate unit.  If the string 
//	 * cannot be parsed, an INVALID value is returned. 
//	 * */
//	static aPolygon3D parse(std::string str) {
//		String[] fields = str.split(Constants::wsPatternParens);
//		if (fields[0].equals("")) {
//			fields = Arrays.copyOfRange(fields,1,fields.length);
//		}
//		try {
//			if (fields.length == 3) {
//				return new aPolygon3D(Double.parseDouble(fields[0]),Double.parseDouble(fields[1]),Double.parseDouble(fields[2]));
//			} else if (fields.length == 6) {
//				return new aPolygon3D(Units::from(Units.clean(fields[1]),Double.parseDouble(fields[0])),
//						Units::from(Units.clean(fields[3]),Double.parseDouble(fields[2])),
//						Units::from(Units.clean(fields[5]),Double.parseDouble(fields[4])));
//			}
//		} catch (Exception e) {}
//		return aPolygon3D.INVALID;
//	}
//

	double Poly3DLL::sumAngles(int dir) const {
		double theta = 0;
		for (int i = 0; i < (int) vertices.size(); i++) {
			int j0 = i-1;
			int j1 = i;
			int j2 = i+1;
			if (j0 < 0) {
				j0 = vertices.size()-1;
			} else if (j2 > (int) vertices.size()-1) {
				j2 = 0;
			}
			double dt = GreatCircle::angle_between(vertices[j0], vertices[j1], vertices[j2], dir);
			theta += dt; //TODO: test for redundant vertices?
		}
		return theta;
	}

	double Poly3DLL::area(){
		if (vertices.size() > 2) {
			int dir = -1;
			if (isClockwise()) {
				dir = 1;
			}
			double area = (sumAngles(dir) - (size()-2)*Pi)*GreatCircle::spherical_earth_radius*GreatCircle::spherical_earth_radius;
			return area;
		}
		return 0.0;
	}

	
	/**
	 * Planar geometric area of a <b>simple</b> polygon. It is positive if the
	 * vertices are ordered counterclockwise and negative if clockwise.
	 * 
	 * @return the signed area of the polygon
	 */
	double Poly3DLL::signedArea()  {
		getBoundingRectangle();
		double minLat = boundingRect.getMinLat();
		double maxLat = boundingRect.getMaxLat();
		double minLon = boundingRect.getMinLon();
		double maxLon = boundingRect.getMaxLon();
		double dLat = (minLat+maxLat)/2.0;
		double dLon = (minLon+maxLon)/2.0;
		double temp = 0;
		int size = vertices.size();
		if (vertices[0].lat() == vertices[size - 1].lat() && vertices[0].lon() == vertices[size - 1].lon()) {
			size--;
		}
		for (int i = 1 ; i < size - 1 ; ++i) {
			temp += (vertices[i].lat()-dLat) * ((vertices[i + 1].lon()-dLon) - (vertices[i - 1].lon()-dLon));
		}
		double area2 = (vertices[0].lat()-dLat) * ((vertices[1].lon()-dLon) - (vertices[size - 1].lon()-dLon))
		                 	+ temp + (vertices[size - 1].lat()-dLat) * ((vertices[0].lon()-dLon) - (vertices[size - 2].lon()-dLon));
		return area2 / 2;
	}

	BoundingRectangle Poly3DLL::getBoundingRectangle() {
		if (!boundingRectangleDefined) {
			boundingRect = BoundingRectangle::makePoly(vertices);
			boundingRectangleDefined = true;
		}
		return boundingRect;
	 }

	const std::vector<LatLonAlt>& Poly3DLL::getVerticesRef() const {
		return vertices;
	}


	std::string Poly3DLL::toString() const {
		return "Poly2DLL"; // + //[vertices=vertices.toString()+
//				", boundingRect="+ boundingRect.toString()
//				+"] bot = "+Fm2(bottom)+" top = "+Fm2(top);
	}

}

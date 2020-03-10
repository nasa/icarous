/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "SimplePoly.h"
#include "Position.h"
#include "Vect2.h"
#include "VectFuns.h"
#include "GreatCircle.h"
#include "NavPoint.h"
#include "EuclideanProjection.h"
#include "Projection.h"
#include "ErrorLog.h"
#include "Util.h"
#include "VectFuns.h"
#include "format.h"
#include <string>
#include <vector>
//#include "Debug.h"

/**
 * A basic polygon that describes a volume.  This volume has a flat bottom and top 
 * (specified as altitude values).  Points describe the cross-section area vertices in
 * a consistently clockwise (or consistently counter-clockwise) manner.  The cross-section
 * need not be convex, but an "out of order" listing of the vertices will result in edges 
 * that cross, and will cause several calculations to fail (with no warning).<p>
 * 
 * A SimplePoly sets the altitude for all its points to be the _bottom_ altitude,
 * while the top is stored elsewhere as a single value.  The exact position for "top"
 * vertices is computed on demand.<p>
 * 
 * The cross-section must be a simple polygon, that is it allows for non-convex areas, but
 * vertices and edges may not overlap or cross.  Vertices may be ordered in either a clockwise
 * or counterclockwise manner.<p>
 * 
 * (A vertex-complete polygon allows for vertices and edges to overlap but not cross, while
 * a general polygon allows for edges to cross.)<p>
 * 
 * Point indices are based on the order they are added.<p>
 * 
 * Note: polygon support is experimental and the interface is subject to change!
 *
 */
namespace larcfm {

using std::vector;
using std::string;
using std::endl;

const bool SimplePoly::move_constant_shape = false;

/**
 * Constructs an empty SimplePoly.
 */
SimplePoly::SimplePoly() {
	isLatLon_b = false;
	p3d = Poly3D();
	p3dll = Poly3DLL();
	init();
}

void SimplePoly::init() {
	//boundingCircleDefined = false;
	centroidDefined = false;
	averagePointDefined = false;
	aPos = Position::ZERO_LL();
//	bPos = Position::ZERO_LL();
	cPos = Position::ZERO_LL();
	clockwiseSum = 0;
}



/**
 * Constructor for a SimplePoly with predefined top and bottom altitudes.
 *
 * @param b Bottom altitude
 * @param t Top altitude
 */
SimplePoly::SimplePoly(double b, double t) {
	isLatLon_b = false;
	p3d = Poly3D(b,t);
	p3dll = Poly3DLL(b,t);
	init();
}

/**
 * Constructor for a SimplePoly with predefined top and bottom altitudes.
 *
 * @param b Bottom altitude
 * @param t Top altitude
 * @param units units to interpret both altitudes
 */
SimplePoly::SimplePoly(double b, double t, const std::string& units) {
	isLatLon_b = false;
	double top = Units::from(units,t);
	double bottom = Units::from(units,b);
	p3d = Poly3D(bottom,top);
	p3dll = Poly3DLL(bottom,top);
	init();
}


/**
 * Create a deep copy of a SimplePoly
 *
 * @param p Source poly.
 */
SimplePoly::SimplePoly(const SimplePoly& p) {
	isLatLon_b = p.isLatLon_b;
	p3d = Poly3D(p.p3d);
	p3dll = Poly3DLL(p.p3dll);
	init();
}

/**
 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
 * @param p3 3D polygon
 * @return a new SimplePoly
 */
SimplePoly SimplePoly::make(const Poly3D& p3) {
	SimplePoly sp; // = SimplePoly();
	sp.setBottom(p3.getBottom());
	sp.setTop(p3.getTop());
	for(int i = 0; i < p3.size(); i++) {
		Position v(Vect3(p3.get2D(i),p3.getBottom()));
		sp.add(v);
	}
	return sp;
}

/**
 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
 * @param p3 3D polygon
 * @return a new SimplePoly
 */
SimplePoly SimplePoly::make(const Poly3DLL& p3) {
	SimplePoly sp; //  = SimplePoly();
	sp.setBottom(p3.getBottom());
	sp.setTop(p3.getTop());
	for(int i = 0; i < p3.size(); i++) {
		Position v(p3.get(i));
		sp.add(v);
	}
	return sp;
}


/** return a aPolygon3D version of this */
Poly3D SimplePoly::poly3D(const EuclideanProjection& proj) const {
	std::vector<Position> points = getVertices();
	Poly3D p3;
	if (isLatLon()) {
		//		p3 = Poly3D();
		for (int i = 0; i < (int) points.size(); i++) {
			Position p = points[i];
			p3.add(proj.project(p).vect2());
		}
	} else {
		//		p3 = Poly3D();
		for (int i = 0; i < (int) points.size(); i++) {
			Position p = points[i];
			p3.add(p.vect3().vect2());
		}
	}
	p3.setBottom(getBottom());
	p3.setTop(getTop());
	return p3;
}


//	/** **********DEBUG*******************
//	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
//	 * @param p3 3D polygon
//	 * @return a new SimplePoly
//	 */
//	SimplePoly make(Poly2DLL p2) {
//		SimplePoly sp = new SimplePoly();
//		sp.setBottom(0.0);
//		sp.setTop(1E10);
//		for(int i = 0; i < p2.size(); i++) {
//			Position v(p2.get(i));
//			sp.add(v);
//		}
//		return sp;
//	}
//


/**
 * Create a SimplePoly from a std::vector<LatLonAlt>.
 * @return a new SimplePoly
 */
SimplePoly SimplePoly::make(std::vector<LatLonAlt> pList, double b, double t) {
	SimplePoly sp(b,t);
	for (int j = 0; j < (int) pList.size(); j++) {
		sp.add(pList[j]);
	}
	return sp;
}

/**
 * Create a SimplePoly from a std::vector<Position>.
 * @return a new SimplePoly
 */
SimplePoly SimplePoly::mk(std::vector<Position> pList, double b, double t) {
	SimplePoly sp(b,t);
	for (int j = 0; j < (int) pList.size(); j++) {
		sp.add(pList[j]);
	}
	return sp;
}


bool SimplePoly::equals(const SimplePoly& p) const {
	bool rtn = (isLatLon() == p.isLatLon());
	//fpln(" $$$$$$$$$$$$ equals rtn = "+rtn+" "+isLatLon()+" "+p.isLatLon());
	if (rtn) {
		if (isLatLon()) {
			rtn = p3dll.equals(p.p3dll);
		} else {
			rtn = p3d.equals(p.p3d);
		}
	}
	return rtn;
}


/**
 * Returns the position of the (bottom) point with index n.
 * If n is not a valid index, this returns the centroid position.  // TODO: Huh?
 *
 * NOTE: The altitude component of the nth position is ambiguous.  This method returns
 * the bottom altitude of the 3D object.
 *
 * @param n index
 * @return position
 */
Position SimplePoly::getVertex(int n) const {
	if (n >= 0 && n < size()) {
		if (isLatLon_b) {
			return Position(p3dll.get(n).mkAlt(getBottom()));
		} else {
			return Position(Vect3(p3d.get2D(n),getBottom()));
		}
	}
	return Position::INVALID();
}

std::vector<Position> genList_l(const std::vector<LatLonAlt>& lList) {
	std::vector<Position> p; // = std::vector<Position>();
	for (int i = 0; i < (int) lList.size(); i++) {
		p.push_back(Position(lList[i]));
	}
	return p;
}

std::vector<Position> genList_v(const std::vector<Vect2>& vList) {
	std::vector<Position> p; // = new std::vector<Position>();
	for (int i = 0; i < (int) vList.size(); i++) {
		p.push_back(Position(Vect3(vList[i],0.0)));
	}
	return p;
}


std::vector<Position> SimplePoly::getVertices() const {
	if (isLatLon_b) {
		return genList_l(p3dll.getVerticesRef());   // TODO: RWB is this ok?
	} else {
		return genList_v(p3d.getVerticesRef());
	}
}


/**
 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates .
 * @param p3 3D polygon
 * @param proj projection to use to interpret the points in the 3D polygon
 * @return a new SimplePoly
 */
//@Deprecated
SimplePoly SimplePoly::make(const Poly3D& p3, const EuclideanProjection& proj) {
	//SimplePoly sp;
	//sp.setBottom(p3.getBottom());
	//sp.setTop(p3.getTop());
	SimplePoly sp(p3.getBottom(),p3.getTop());
	for(int i = 0; i < p3.size(); i++) {
		Position v(proj.inverse(p3.get2D(i), p3.getBottom()));
		sp.add(v);
	}
	return sp;
}


/**
 * Determine if the polygon contains geodetic points.
 * @return True if this SimplePoly contains any geodetic points. If size is 0, returns false.
 */
bool SimplePoly::isLatLon() const{
	return isLatLon_b;
}

/**
 * Number of points
 * @return Number of points in this SimplePoly
 */
int SimplePoly::size() const {
	if (isLatLon_b) return p3dll.size();
	else return p3d.size();
}

/**
 * Add vertices to a polygon until it has a specified number.  This will not remove vertices if there are already more than the desired number.  Each new vertex will be added the to current longest edge.
 * @param sp1 Polygon
 * @param sz desired number of vertices
 */
void SimplePoly::increaseVertices(int sz) {
	while (size() < sz) {
		int ii = 0;
		double d = getVertex(0).distanceH(getVertex(size()-1));
		Position p = getVertex(0).midPoint(getVertex(size()-1));
		for (int i = 0; i < size()-1; i++) {
			double d_i = getVertex(i).distanceH(getVertex(i+1));
			if (d_i > d) {
				p = getVertex(i).midPoint(getVertex(i+1));   // midpoint of shortest leg
				d = d_i;
				ii = i+1; // where to insert
			}
		}
		insert(ii,p);
	}
}

/**
 * Return this centroid of this volume.
 * Note: if sides are small (less than about 10^-5 in internal units), there may be errors in the centroid calculations
 * @return The centroid position of this volume.
 */
Position SimplePoly::centroid(){
	if (!centroidDefined) {
		if (isLatLon_b) {
			cPos = Position(p3dll.centroid());
		} else {
			cPos = Position(p3d.centroid());
		}
		centroidDefined = true;
	}
	return cPos;
}

/**
 * Area in square meters.
 */
double SimplePoly::area(){
	if (isLatLon_b) {
		return p3dll.area();
	} else {
		return p3d.area();
	}
}



Position SimplePoly::avgPos(const std::vector<Position>& points, const std::vector<double>& wgts) {
	// there can be numeric problems with these calculations if sides are small (multiplying small numbers loses precision)
	// shift points to around the origin (and then back) in an attempt to alleviate these.
	Position bbcent = getBoundingRectangle().centerPos();
	double dx = bbcent.x();
	double dy = bbcent.y();
	double x = 0;
	double y = 0;
	double xorig = points[0].x()-dx;
	double tot = 0;
	for (int i = 0; i < (int) points.size(); i++) {
		double x0 = points[i].x()-dx;
		double y0 = points[i].y()-dy;
		// correct for date line wrap around
		if (isLatLon()) {
			if (x0-xorig > Pi) {
				x0 = x0 - 2*Pi;
			} else if (x0-xorig < -Pi) {
				x0 = x0 + 2*Pi;
			}
		}
		double w = 1.0;
		if (wgts.size() == points.size()) {
			w = wgts[i];
		}
		x += x0 * w;
		y += y0 * w;
		tot += w;
	}
	tot = tot == 0.0 ? 1.0 : tot;
	if (isLatLon()) {
		x = Util::to_pi(x);
	}
	Position myaPos = points[0].mkX(dx+x/tot).mkY(dy+y/tot);
	return myaPos;
}



// note: this may fail over poles!
/**
 * Return the average of all vertices.  Note this is not the same as the centroid!  This will, however, have the nice property of having
 * a constant velocity when dealing with a morphing polygon.
 * @return average of all vertices
 */
Position SimplePoly::averagePoint()  {
	if (!averagePointDefined) {
		//aPos = avgPos(points, null).mkZ((top+bottom)/2);
		if (isLatLon_b) {
			aPos = Position(p3dll.averagePoint());
		} else {
			aPos = Position(p3d.averagePoint());
		}
		//aPos = aPos.mkZ((top+bottom)/2;
		averagePointDefined = true;
	}
	return aPos;
}


// returns the center of a bounding circle
// calculates it and radius, if necessary
/**
 * Returns the center of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
 * @return center of polygon
 */
Position SimplePoly::boundingCircleCenter()  {
	return centroid();
}


/** Returns true if this polygon is convex
 * @return true, if convex
 */
bool SimplePoly::isConvex() const {
	bool rtn;
	if (isLatLon_b) {
		rtn = p3dll.isConvex();
	} else {
		rtn = p3d.isConvex();
	}
	return rtn;
}

/**
 * Returns the radius of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
 * @return radius of bounding circle
 */
double SimplePoly::boundingCircleRadius() {
	return maxRadius();
}



Position SimplePoly::maxDistPair(const Position& p) const {
	double maxD = 0.0;
	Position p2 = p;
	std::vector<Position> points;
	if (isLatLon_b) {
		points = genList_l(p3dll.getVerticesRef());
	} else {
		points = genList_v(p3d.getVerticesRef());
	}
	for (int i = 0; i < (int) points.size(); i++) {
		double d = points[i].distanceH(p);
		if (d > maxD) {
			p2 = points[i];
			maxD = d;
		}
	}
	return p2;
}


/**
 * Returns the max horizontal distance between any vertex and the centroid
 * @return radius
 */
double SimplePoly::maxRadius() {
	Position c = centroid();
	Position p = maxDistPair(c);
	//fpln("SimplePoly,maxRadius "+this);
	//fpln("SimplePoly,maxRadius c="+c+" p="+p+" dist="+Units::str("nmi", c.distanceH(p)));
	return c.distanceH(p);
}

double SimplePoly::apBoundingRadius() {
	Position c = averagePoint();
	Position p = maxDistPair(c);
	return c.distanceH(p);
}


/**
 * Add a new vertex to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner.
 * This currently does NOT set the Z component of the point
 * @param p position to add
 * @return false, if an error is detected (duplicate or invalid vertex), true otherwise.
 */
bool SimplePoly::add(const Position& p){
	//fpln(" $$$ SimplePoly.add p = "+p);
	if (p.isInvalid()){ //|| points.contains(p)) {
		// error -- no duplicate points allowed
		//fpln("SimplePoly.add error: bad point "+p);
		//			Debug.halt();
		return false;
	}
	init();
	isLatLon_b = p.isLatLon();
	if (isLatLon_b) {
		p3dll.add(p.lla());
	} else {
		p3d.add(p.vect2());
	}
	return true;
}


bool SimplePoly::insert(int i, const Position& p) {
	if (p.isInvalid()){ //|| points.contains(p)) {
		return false;
	}
	init();
	isLatLon_b = p.isLatLon();
	if (isLatLon_b) {
		p3dll.insert(i, p.lla());
	} else {
		p3d.insert(i, p.vect2());
	}
	return true;

}


/**
 * Add a new point to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner.
 * This currently does NOT set the Z component of the point (unless it is the first point)
 * @param p position to add
 * @return false, if an error is detected (duplicate or invalid vertex), true otherwise.
 */
bool SimplePoly::add(const LatLonAlt& lla){
	if (lla.isInvalid()){ //|| points.contains(p)) {
		return false;
	}
	init();
	isLatLon_b = true;
	p3dll.add(lla);
	return true;
}

/**
 * Add a new point to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner.
 * This currently does NOT set the Z component of the point (unless it is the first point)
 * @param p position to add
 * @return false, if an error is detected (duplicate or invalid vertex), true otherwise.
 */
bool SimplePoly::add(const Vect2& v){
	if (v.isInvalid()){ //|| points.contains(p)) {
		return false;
	}
	init();
	isLatLon_b = false;
	p3d.add(v);
	return true;
}




/**
 * Remove a point from this SimplePoly.
 * @param n Index (in order added) of the point to be removed.
 */
void SimplePoly::remove(int n) {
	init();
	if (isLatLon_b) {
		p3dll.remove(n);
	} else {
		p3d.remove(n);
	}
}

/**
 * Change the position of a point already added to the SimplePoly, indicated by its index.
 * This currently does NOT set the Z component of the point.
 *
 * @param n index
 * @param p position
 * @return false, if an invalid vertex is detected, true otherwise
 */
bool SimplePoly::set(int n, Position p) {
	if (p.isInvalid()) { //  || points.contains2D(p.mkAlt(bottom))) {
		// error -- no duplicate points allowed
		return false;
	}
	init();
	if (isLatLon_b) {
		//if (p3dll.vertices.contains(p.lla().mkAlt(nullAlt))) return false; // no duplicate points allowed
		for (int i = 0; i < (int) p3dll.size(); i++) {
			if (p3dll.get(i).equals2D(p.lla())) return false;
		}
		p3dll.set(n,p.lla());
	} else {
		//if (p3d.p2d.vertices.contains(p.vect2())) return false; // no duplicate points allowed
		for (int i = 0; i < (int) p3d.size(); i++) {
			if (p3d.get2D(i) == p.vect2()) return false;
		}
		p3d.set(n,p.vect2());
	}
	return true;
}



/**
 * Sets the top altitude of this SimplePoly.
 * @param t New top altitude.
 */
void SimplePoly::setTop(double t) {
	centroidDefined = false;
	averagePointDefined = false;
	if (isLatLon_b) {
		p3dll.setTop(t);
	} else {
		p3d.setTop(t);
	}
}

/**
 * Return the top altitude of this SimplePoly.
 * @return top altitude
 */
double SimplePoly::getTop() const {
	if (isLatLon_b) {
		return p3dll.getTop();
	} else {
		return p3d.getTop();
	}
}

/**
 * Sets the bottom altitude of this SimplePoly.
 * @param b New bottom altitude.
 */
void SimplePoly::setBottom(double b) {
	centroidDefined = false;
	averagePointDefined = false;
	if (isLatLon_b) {
		p3dll.setBottom(b);
	} else {
		p3d.setBottom(b);
	}


}

/**
 * Return the bottom altitude of this SimplePoly.
 * @return bottom altitude
 */
double SimplePoly::getBottom()const  {
	if (isLatLon_b) {
		return p3dll.getBottom();
	} else {
		return p3d.getBottom();
	}
}


/**
 * Returns the position of the top point with index n.
 * If n is not a valid index, this returns INVALID // the centroid position.
 *
 * @param n index
 * @return position
 */
Position SimplePoly::getTopPoint(int n) const {
	//		if (n >= 0 && n < points.size()) {
	//			return points.get(n).mkAlt(top);
	//		}
	if (n >= 0 && n < size()) {
		if (isLatLon_b) {
			return Position(p3dll.get(n).mkAlt(p3dll.getTop()));
		} else {
			return Position(Vect3(p3d.get2D(n),p3d.getTop()));
		}
	}
	return Position::INVALID();
}

/**
 * Returns a deep copy of this SimplPoly.
 *
 * @return copy
 */
SimplePoly SimplePoly::copy() const {
	SimplePoly r(getBottom(),getTop());
	for (int i = 0; i < size(); i++) {
		//fpln("copy "+i+" src point="+get(i));
		//r.add(get(i));
		if (isLatLon_b) {
			r.add(Position(p3dll.get(i)));
		} else {
			r.add(Position(Vect3(p3d.get2D(i),getBottom())));   // TODO: LOOK AT THIS get(i) ??
		}
	}
	return r;
}

/** return a Poly3D version of this.  proj is ignored if this is Euclidean
 * @param proj projection
 * @return a new 3D polygon
 */
//@Deprecated
Poly3D SimplePoly::poly3D(const EuclideanProjection& proj) {
	if (isLatLon_b) {
		std::vector<Position> points = getVertices();
		Poly3D p3 = Poly3D();
		for (int i = 0; i < (int) points.size(); i++) {
			Position p = points[i];
			p3.add(proj.project(p).vect2());
		}
		p3.setBottom(getBottom());
		p3.setTop(getTop());
		return p3;
	} else {
		Poly3D rtn = p3d; // TODO: RWB IS THIS COPY NECESSARY?
		return rtn;
	}
}


bool SimplePoly::isClockwise() const {
	std::vector<Position> points = getVertices();
	if (Util::almost_equals(clockwiseSum, 0.0)) {
		for (int i = 0; i < (int) points.size(); i++) {
			int h = i-1;
			int j = i+1;
			if (h < 0) h = (int) points.size()-1;
			if (j == (int) points.size()) j = 0;
			double trk1 = points[h].finalVelocity(points[i], 100).trk();
			double trk2 = points[i].initialVelocity(points[j], 100).trk();
			double angle = Util::to_pi(Util::turnDelta(trk1, trk2, true));
			clockwiseSum += angle;
		}
	}
	return clockwiseSum > 0.0;
}

void SimplePoly::reverseOrder() {
	if (isLatLon_b) {
		p3dll = p3dll.reverseOrder();
	} else {
		p3d = p3d.reverseOrder();
	}
}


/**
 * closest horizontal distance from p to an edge.
 * @param p
 * @return distance from edge
 */
double SimplePoly::distanceFromEdge(const Position& p) const {
	if (p.isLatLon()) {
		LatLonAlt ll = p.lla();
		LatLonAlt cl = GreatCircle::closest_point_segment(getVertex(size()-1).lla(), getVertex(0).lla(), ll);
		double dist = GreatCircle::distance(ll, cl);
		for (int i = 0; i < size()-1; i++) {
			cl = GreatCircle::closest_point_segment(getVertex(i).lla(), getVertex(i+1).lla(), ll);
			dist = Util::min(GreatCircle::distance(ll, cl), dist);
		}
		return dist;
	} else {
		Vect3 v3 = p.vect3();
		Vect3 cl = VectFuns::closestPointOnSegment(getVertex(size()-1).vect3(), getVertex(0).vect3(), v3);
		double dist = v3.distanceH(cl);
		for (int i = 0; i < size()-1; i++) {
			cl = VectFuns::closestPointOnSegment(getVertex(i).vect3(), getVertex(i+1).vect3(), v3);
			dist = Util::min(v3.distanceH(cl), dist);
		}
		return dist;
	}
}

/**
 * return vertical distance from the polygon.  This will be zero if the point is between the polygon's top and bottom.
 * @param p
 * @return
 */
double SimplePoly::distanceV(const Position& p) const {
	if (p.isLatLon()) {
		if (p.alt() >= getBottom() && p.alt() <= getTop()) return 0.0;
		return Util::min(std::abs(p.alt()-getTop()), std::abs(p.alt()-getBottom()));
	} else {
		if (p.z() >= getBottom() && p.z() <= getTop()) return 0.0;
		return Util::min(std::abs(p.z()-getTop()), std::abs(p.z()-getBottom()));
	}

}

/**
 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
 * @param p position
 * @return true, if position is in the polygon
 */
bool SimplePoly::contains(const Position& p) {
	if (size() < 2) return false;
	if (p.isLatLon()) {
		return p3dll.contains(p.lla());
	} else {
		return p3d.contains(p.vect3());
	}

}

/**
 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
 *
 * @param p position
 * @return true, if position is in the polygon
 */
bool SimplePoly::contains2D(const Position& p) {
	if (size() < 2) return false;
	if (p.isLatLon()) {
		return p3dll.contains2D(p.lla());
	} else {
		return p3d.contains2D(p.vect2());
	}
}



//	final std::string nl = System.getProperty("line.separator");


/**
 * Return the angle that is perpendicular to the middle of the edge from vertex i to i+1, facing outward.
 * Return NaN if i is out of bounds or vertex i overlaps vertex i+1.
 *
 * @param i index
 * @return angle
 */
double SimplePoly::perpSide(int i) const {
	std::vector<Position> points = getVertices();
	if (i < 0 || i >= (int) points.size()) return NaN;
	Position p1 = points[i];
	Position p2;
	if (i == (int) points.size()-1) {
		p2 = points[0];
	} else {
		p2 = points[i+1];
	}
	if (p1.almostEquals(p2)) return NaN;
	double trk = p1.avgVelocity(p2, 100).trk() - M_PI/2.0;
	if (isClockwise()) {
		return Util::to_2pi(trk);
	} else {
		return Util::to_2pi(trk+M_PI);
	}
}

bool SimplePoly::vertexConvex(const Position& p0, const Position& p1, const Position& p2) {
	std::vector<Position> pts;
	std::vector<double> wgts;
	pts.push_back(p0);
	pts.push_back(p1);
	pts.push_back(p2);
	wgts.push_back(1.0);
	wgts.push_back(100.0);
	wgts.push_back(1.0);
	Position avg = avgPos(pts, wgts);
	return contains2D(avg);
}

/**
 * Return the internal angle of vertex i.
 * Return NaN if i is out of bounds or vertex i overlaps with an adjacent vertex.
 *
 * @param i index
 * @return angle
 */
double SimplePoly::vertexAngle(int i) {
	std::vector<Position> points = getVertices();
	if (i < 0 || i >= (int) points.size()) return NaN;
	Position p1 = points[i];
	Position p2;
	Position p0;
	if (i == (int) points.size()-1) {
		p2 = points[0];
	} else {
		p2 = points[i+1];
	}
	if (i == 0) {
		p0 = points[points.size()-1];
	} else {
		p0 = points[i-1];
	}
	if (p1.almostEquals(p2) || p1.almostEquals(p0) || p0.almostEquals(p2)) return NaN;
	double ang;
	if (p1.isLatLon()) {
		ang = GreatCircle::angle_between(p0.lla(), p1.lla(), p2.lla());
	} else {
		ang = Util::to_2pi(VectFuns::angle_between(p0.vect2(), p1.vect2(), p2.vect2()));
	}
	if (vertexConvex(p0,p1,p2)) {
		return ang;
	} else {
		return 2*Pi - ang;
	}
}


/**
 * This moves the SimplePoly by the amount determined by the given (Euclidean) offset.
 * @param off offset
 */
void SimplePoly::translate(const Vect3& off) {
	centroidDefined = false;
	averagePointDefined = false;
	std::vector<Position> points = getVertices();
	Position c = centroid();
	if (isLatLon()) {
		EuclideanProjection proj = Projection::createProjection(c.lla());
		for (int i = 0; i < size(); i++) {
			set(i, Position(proj.project(points[i])));
		}
	}
	// translate
	for (int i = 0; i < size(); i++) {
		Position p = points[i];
		set(i, p.mkX(p.x()+off.x).mkY(p.y()+off.y));
	}
	// shift back to latlon, if necessary
	if (isLatLon()) {
		EuclideanProjection proj = Projection::createProjection(c.lla());
		for (int i = 0; i < size(); i++) {
			set(i, Position(proj.inverse(points[i].vect3())));
		}
	}
	setTop(getTop()+off.z);
	setBottom(getBottom()+off.z);
}


//	SimplePoly SimplePoly::linear(const Velocity& v, double t) const {
//		SimplePoly newPoly;
//		int sz = size();
//		for (int j = 0; j < sz; j++) {
//			Position p = get(j).linear(v,t);
//			Position pt = getTopPoint(j).linear(v,t);
//			newPoly.add(p);
//			newPoly.setBottom(p.z());
//			newPoly.setTop(pt.z());
//		}
//		return newPoly;
//	}


SimplePoly SimplePoly::linearAllVerts(const Velocity& v, double t) const {
	SimplePoly newPoly;
	int sz = size();
	if (isLatLon_b) {
		double ptz = p3dll.getTop() + v.z*t;
		double pbz = p3dll.getBottom() + v.z*t;
		for (int j = 0; j < sz; j++) {
			LatLonAlt p = GreatCircle::linear_initial(p3dll.get(j),v,t);
			//f.pln("linear vj="+getVertex(j)+" v="+v+" t="+t+" p="+p);
			//Position pt = getTopPoint(j).linear(v,t);
			newPoly.add(p);
			newPoly.setBottom(pbz);
			newPoly.setTop(ptz);
		}

	} else {
		double ptz = p3d.getTop() + v.z*t;
		double pbz = p3d.getBottom() + v.z*t;
		for (int j = 0; j < sz; j++) {
			//Vect3 p0 = Vect3(p3d.get(j),getBottom());
			Vect2 p = p3d.get2D(j).linear(v.vect2(),t);
			//f.pln("linear vj="+getVertex(j)+" v="+v+" t="+t+" p="+p);
			//Position pt = getTopPoint(j).linear(v,t);
			newPoly.add(p);
			newPoly.setBottom(pbz);
			newPoly.setTop(ptz);
		}
	}
	return newPoly;
}


SimplePoly SimplePoly::linear(const Velocity& v, double t) const {
	if (move_constant_shape) {
		return linearFixedShape(v,t);
	} else {
		return linearAllVerts(v,t);
	}
}


/** Move a polygon by velocity using vertex 0 and reconstructing geometrically
 *
 * @param v
 * @param t
 * @return
 */
SimplePoly SimplePoly::linearFixedShape(const Velocity& v, double t) const {
	SimplePoly newPoly;
	//Position center = centroid();
	//Position center = averagePoint();
	Position center = getVertex(0);
	Position newCenter = center.linear(v,t);
	newPoly.add(newCenter);
	for (int i = 1; i < size(); i++) {
		Position pos_i = getVertex(i);
		double dist_i = center.distanceH(pos_i);
		double trk_i = center.track(pos_i);
		Position newPos_i = newCenter.linearDist2D(trk_i,dist_i);
		newPoly.add(newPos_i);
	}
	return newPoly;
}



bool SimplePoly::validate() const {
	return validate(NULL);
}

bool SimplePoly::validate(ErrorLog* error) const {
	std::vector<Position> points = getVertices();
	// not a polygon
	if (size() < 3)	{
		if (error != NULL) error->addError("polygon has fewer than 3 points!");
		return false;
	}
	for (int i = 0; i < (int) points.size(); i++) {
		// invalid points
		if (points[i].isInvalid()) {
			if (error != NULL) error->addError("point "+Fm0(i)+" is invalid!");
			return false;
		}
		for (int j = 0; j < i; j++) {
			// duplicated points

			double distH = points[i].distanceH(points[j]);
			if (distH < Constants::get_horizontal_accuracy()) {
				if (error != NULL) error->addError("polygon has duplicated points at "+Fm0(i)+" distH = "+Fm1(distH));
				return false;
			}
			if (j < i-2) {
				// check for intersections
				if (isLatLon()) {
					LatLonAlt a = points[j].lla();
					LatLonAlt b = points[j+1].lla();
					LatLonAlt c = points[i-1].lla();
					LatLonAlt d = points[i].lla();
					if (!GreatCircle::intersectSegments(a,b,c,d).isInvalid()) {
						if (error != NULL) error->addError("polygon has intersecting edges at "+Fm0(i-1)+"-"+Fm0(i)+" and "+Fm0(j)+"-"+Fm0(j+1));
						return false;
					}
//					double t = GreatCircle::intersection(a, GreatCircle::velocity_initial(a, b, 100), c, GreatCircle::velocity_initial(c,d,100)).second;
//					if (t > 0 && t < 100) {
//						if (error != NULL) error->addError("polygon has intersecting edges at "+Fm0(i-1)+"-"+Fm0(i)+" and "+Fm0(j)+"-"+Fm0(j+1)+" t="+Fm2(t));
//						return false;
//					}
				} else {
					Vect2 a = points[j].vect2();
					Vect2 b = points[j+1].vect2();
					Vect2 c = points[i-1].vect2();
					Vect2 d = points[i].vect2();
					if (!VectFuns::intersectSegments(a, b, c, d).first.isInvalid()) {
						if (error != NULL) error->addError("polygon has intersecting edges at "+Fm0(i-1)+"-"+Fm0(i)+" and "+Fm0(j)+"-"+Fm0(j+1));
						return false;
					}
//					double t = VectFuns::timeOfIntersection(a, Velocity::make(b.Sub(a).Hat().Scal(0.01)), c, Velocity::make(d.Sub(c).Hat().Scal(0.01)));
//					if (t > 0 && t < 100) {
//						if (error != NULL) error->addError("polygon has intersecting edges at "+Fm0(i-1)+"-"+Fm0(i)+" and "+Fm0(j)+"-"+Fm0(j+1));
//						return false;
//					}
				}
			}
		}
		if (i > 1) {
			// redundant (consecutive collinear) points
			if (isLatLon()) {
				if (GreatCircle::collinear(points[i-2].lla(), points[i-1].lla(), points[i].lla())) {
					if (error != NULL) error->addWarning("polygon has redundant collinear points at "+Fm0(i));
					//Debug::halt();
					//fpln(" $$$ SimplePoly::validate: polygon has redundant collinear points at "+Fm0(i));
					//fpln(" $$$ SimplePoly::validate: points[i-2,i-1,1] = "+points[i-2].lla().toString()+", "+points[i-1].lla().toString()+", "+points[i].lla().toString());
					//return false;
				}
			} else {
				if (VectFuns::collinear(points[i-2].vect2(), points[i-1].vect2(), points[i].vect2())) {
					if (error != NULL) error->addWarning("polygon has redundant collinear points at "+Fm0(i));
					//Debug::halt();
					//return false;
				}
			}
		}
	}
	return true;
}



int SimplePoly::maxInRange(const Position& p, double a1, double a2) const {
	std::vector<Position> points = getVertices();
	double maxD = 0.0;
	int idx = -1;
	for (int i = 0; i < (int) points.size(); i++) {
		double d = points[i].distanceH(p);
		double a = p.initialVelocity(points[i], 100).compassAngle();
		if (d > maxD && a1 <= a && a < a2) {
			idx = i;
			maxD = d;
		}
	}
	return idx;
}

std::vector<LatLonAlt> SimplePoly::rip(const SimplePoly& sPoly) {
	std::vector<LatLonAlt> polyList;
	if (sPoly.isLatLon()) {
		for (int j = 0; j < sPoly.size(); j++) {
			polyList.push_back(sPoly.getVertex(j).lla());
		}
	}
	return polyList;
}




BoundingRectangle SimplePoly::getBoundingRectangle() {
	if (isLatLon_b) {
		return p3dll.getBoundingRectangle();
	} else {
		return p3d.getBoundingRectangle();
	}
}


std::string SimplePoly::toOutput(const std::string& name, int precision, int numberTcpColumns) const {
	//std::string sb;
	std::ostringstream sb;
	for (int j = 0; j < size(); j++) {
		std::vector<std::string> ret; //  = new ArrayList<std::string>(numberTcpColumns+2);
		//ret.push_back(name);  // name is (0)
		sb << name << " ";
		std::vector<std::string> sl = getVertex(j).toStringList(precision,0,false);
		sb << list2str(sl,",") << ", ";
		//ret.push_back(FmPrecision(0.0)); // time 4
		if (numberTcpColumns > 0) {
			ret.push_back("-"); // type
			int start = 0;
			for (int k = start; k < numberTcpColumns; k++) {
				ret.push_back("-");
			}
		} else {
			ret.push_back("-"); // label
		}
		ret.push_back(FmPrecision(Units::to("ft", getTop()),precision));
		sb << list2str(ret,", ") << endl;
	}
	return sb.str();
}




string SimplePoly::toString() const {
	string s = "SimplePoly:  bottom = "+Fm1(getBottom())+" top = "+Fm1(getTop())+" size = "+Fm0(size())+"\n";
	for (int i = 0; i < size(); i++) {
		s = s + getVertex(i).toString()+"\n";
	}
	return s;
}

std::vector<std::string> SimplePoly::toStringList(int vertex, int precision) const {
	std::vector<std::string> ret;
	Position p = getVertex(vertex);
	std::vector<std::string> plist = p.toStringList(precision);
	ret.insert(ret.end(), plist.begin(), plist.end());
	if (p.isInvalid()) {
		ret.push_back("-");
	} else {
		ret.push_back(FmPrecision(Units::to("ft",getTop()),precision));
	}
	return ret;
}


}

/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Poly2D.h"
#include "Vect2.h"
#include "format.h"
#include "Util.h"
//#include "GeneralPath.h"

/**
 * Encapsulates a geometric polygon. The polygon is defined in terms of its vertex coordinates. 
 * This implementation assumes a simply connected polygon. The Polygon is otherwise quite general allowing multiply
 * connected regions. The class provides a containment test for points and uses bounding rectangles
 * to speed up computations.
 */

namespace larcfm {

const double Poly2D::vertexPerturb = 1E-10;
const double Poly2D::perturbAmount = 2E-10;


int Poly2D::size() const {
	return vertices.size();
}

Poly2D::Poly2D() {
	vertices.resize(0);
//	boundingRectangleDefined = false;
	minX = minY = MAXDOUBLE;
	maxX = maxY = -MAXDOUBLE;
}


Poly2D::Poly2D(const std::vector<Vect2>& verts) {
//	boundingRectangleDefined = false;
	minX = minY = MAXDOUBLE;
	maxX = maxY = -MAXDOUBLE;
	vertices.resize(verts.size());
	for (int i = 0; i < (int) verts.size(); i++) {  // copy
		vertices[i] = Vect2(verts[i].x,verts[i].y);
		maxX = Util::max(verts[i].x, maxX);
		maxY = Util::max(verts[i].y, maxY);
		minX = Util::min(verts[i].x, minX);
		minY = Util::min(verts[i].y, minY);
	}
//	if (vertices.size() > 0) {
//		boundingRect = BoundingRectangle(vertices);
//		boundingRectangleDefined = true;
//	}
}

Poly2D::Poly2D(const Poly2D& polygon) {
	vertices.resize(polygon.size());   //vertices[polygon.size()];
	for (int i = 0 ; i < (int) polygon.size() ; i++) {
		vertices[i] = Vect2(polygon.vertices[i].x,polygon.vertices[i].y);
	}
//	boundingRect = BoundingRectangle(polygon.boundingRect);
//	boundingRectangleDefined = polygon.boundingRectangleDefined;
	maxX = polygon.maxX;
	maxY = polygon.maxY;
	minX = polygon.minX;
	minY = polygon.minY;
}

//	Poly2D Poly2D::copy(const Poly2D& polygon) {
//		Poly2D apoly = Poly2D();
//		apoly.vertices.resize(polygon.size());   //vertices[polygon.size()];
//		for (int i = 0 ; i < (int) polygon.size() ; i++) {
//			apoly.vertices[i] = Vect2(polygon.vertices[i].x,polygon.vertices[i].y);
//		}
//		apoly.boundingRect = BoundingRectangle(polygon.boundingRect);
//		apoly.boundingRectangleDefined = polygon.boundingRectangleDefined;
//		return apoly;
//	}


Poly2D Poly2D::linear(const Vect2& v, double t) const  {
	Poly2D rtn = Poly2D();
	//for (Vect2 vt: vertices) {
	for (int i = 0 ; i < (int) vertices.size() ; i++) {
		Vect2 vt = vertices[i];
		rtn.addVertex(vt.linear(v,t));
	}
	return rtn;
}

Poly2D Poly2D::linear(const std::vector<Vect2>& v, double t) const  {
	Poly2D rtn = Poly2D();
	for (int i = 0 ; i < (int) vertices.size() ; i++) {
		Vect2 vt = vertices[i];
		rtn.addVertex(vt.linear(v[i],t));
	}
	return rtn;
}


void Poly2D::addVertex(double x, double y) {
	vertices.push_back(Vect2(x,y));
//	boundingRectangleDefined = false;
	minX = minY = MAXDOUBLE;
	maxX = maxY = -MAXDOUBLE;
}


void Poly2D::addVertex(const Vect2& v) {
	//f.pln("Poly2D.addVertex "+v);
	vertices.push_back(v);
	//	boundingRectangleDefined = false;
	minX = minY = MAXDOUBLE;
	maxX = maxY = -MAXDOUBLE;
}


bool Poly2D::contains(double a, double b) const {
//	if (!boundingRectangleDefined) {
//		boundingRect = BoundingRectangle(vertices);
//		boundingRectangleDefined = true;
//	}
//	if (!boundingRect.contains(a, b)) {
//		return false;
//	}
	if (minX == MAXDOUBLE) {
		for (int i = 0; i < (int) vertices.size(); i++) {  // copy
			maxX = Util::max(vertices[i].x, maxX);
			maxY = Util::max(vertices[i].y, maxY);
			minX = Util::min(vertices[i].x, minX);
			minY = Util::min(vertices[i].y, minY);
		}
	}
	if (a > maxX || a < minX || b > maxY || b < minY) {
		return false;
	}


	// Adjust the x coordinate if it is very close to that of the point in question to avoid ambiguity caused by loss of
	// significance in floating point calculations. This assures that all polygon x coordinates will be at least vertexPerturb
	// to the left or right of the x coordinate of the point in question.
	Poly2D poly2 = Poly2D(*this);
	//f.pln(" ^^$$>> contains:  poly2 = "+poly2);
	for (int i = 0 ; i < (int) vertices.size() ; i++) {
		if (std::abs(a - vertices[i].x) <= vertexPerturb) {
			double yi = poly2.vertices[i].y;
			poly2.vertices[i] = Vect2(vertices[i].x + perturbAmount, yi);
		}
	}
	// Determine, for each edge of the polygon, whether the ray parallel to the y-axis
	// which proceeds in the positive direction from the point in question intersects the edge.
	// Count the number of edges intersected. If the count is odd, the point lies within the polygon
	int crossingCount = 0;
	int n = poly2.vertices.size();
	for (int i = 0 ; i < n ; i++) {
		// Note: the purpose of the modulo n on the index is to make the last
		// line segment
		// the one that connects the last point back to the first.
		if ((poly2.vertices[i].x < a && poly2.vertices[(i+1)%n].x < a)|| (poly2.vertices[i].x > a && poly2.vertices[(i+1)%n].x > a)) {
			// Line is entirely to the left or right of the y-axis ray
			continue;
		}
		if (poly2.vertices[i].y < b && poly2.vertices[(i+1)%n].y < b) {
			// Line is entirely below the y-axis ray
			continue;
		}
		if (poly2.vertices[i].y > b && poly2.vertices[(i+1)%n].y > b) {
			// both y coordinates are above the point, and the x coordinates straddle it; The ray must cross the line;
			crossingCount++;
			continue;
		}
		// because of the perturbation, we cannot have any candidate vertical edges (those would be either all-left or all-right)
		// Calculate the inverse (x/y) slope of the line
		double slope = (poly2.vertices[(i+1)%n].x - poly2.vertices[i].x) / (poly2.vertices[(i+1)%n].y - poly2.vertices[i].y);
		// Now the x intercept at the y value of the ray
		double yRayIntercept = (a - poly2.vertices[i].x)/slope + poly2.vertices[i].y;
		if (yRayIntercept >= b) {
			crossingCount++;
			continue;
		}
	}
	//f.pln(" $$>> Poly2D.contains crossingCount = "+crossingCount);
	if (crossingCount % 2 > 0){
		return true;
	}
	return false;
}


bool Poly2D::contains(const Vect2& v) const {
	return contains(v.x,v.y);
}

void Poly2D::setVertex(int i,const Vect2& v) {
	if (i < 0 || i >= (int) vertices.size()) {
		fpln("Poly2D.setVertex index out of bounds: "+Fm0(i));
	} else {
		vertices[i] = v;
	}
//	boundingRectangleDefined = false;
	minX = minY = MAXDOUBLE;
	maxX = maxY = -MAXDOUBLE;

}

Vect2 Poly2D::getVertex(int i) const {
	if (i < 0 || i >= (int) vertices.size()) {
//		fpln("Poly2D.getVertex index out of bounds: "+Fm0(i));
		return Vect2::INVALID();
	} else {
		return vertices[i];
	}
}


std::vector<Vect2> Poly2D::getVertices() const {
	return vertices;
}

//BoundingRectangle Poly2D::getBoundingRectangle() const {
//	return boundingRect;
//}
//
//void Poly2D::setBoundingRectangle(BoundingRectangle boundingRectangle) {
//	boundingRect = boundingRectangle;
//}


/**
 * Planar geometric area of a <b>simple</b> polygon. It is positive if the
 * vertices are ordered counterclockwise and negative if clockwise.
 *
 * @return the signed area of the polygon
 */
double Poly2D::signedArea() const {
	double dx = (minX+maxX)/2.0;
	double dy = (minY+maxY)/2.0;
	double temp = 0;
	int size = vertices.size();
	if (vertices[0].x == vertices[size - 1].x && vertices[0].y == vertices[size - 1].y) {
		size--;
	}
	for (int i = 1 ; i < size - 1 ; ++i) {
		temp += (vertices[i].x-dx) * ((vertices[i + 1].y-dy) - (vertices[i - 1].y-dy));
	}
	double area2 =
			(vertices[0].x-dx) * ((vertices[1].y-dy) - (vertices[size - 1].y-dy)) + temp + (vertices[size - 1].x-dx) * ((vertices[0].y-dy) - (vertices[size - 2].y-dy));

	return area2 / 2;
}

// area and centroid courtesy of Paul Bourke (1988) http://paulbourke.net/geometry/polyarea/
// these are for non self-intersecting polygons
// this calculation assumes that point 0 = point n (or the start point is counted twice)
/**
 * Return the horizontal area (in m^2) of this Poly3D.
 */
double Poly2D::area() {
	return std::abs(signedArea());
	//fpln(" Poly2D.area -- not yet implemented!!"); return -1;
	//		double a = 0;
	//		vertices.push_back(vertices[0]);
	//		for (int i = 0; i < (int) vertices.size()-1; i++) {
	//			a = a + (vertices[i].x()*vertices[i+1].y() - vertices[i+1].x()*vertices[i].y());
	//		}
	//		//vertices.remove(vertices.size()-1);
	//
	//		map<string,int>::iterator it = vertices.find(vertices.size()-1);
	//	                aircraftHList.erase(it);
	//
	//
	//		return 0.5*a;
}


/**
 * @return the geometric centroid.
 */
Vect2 Poly2D::centroid()	const {
	double dx = (minX+maxX)/2.0;
	double dy = (minY+maxY)/2.0;
	double temp = 0;
	double tempX = 0;
	double tempY = 0;
	int size = vertices.size();
	if (vertices[0].x == vertices[size - 1].x && vertices[0].y == vertices[size - 1].y) {
		size--;
	}
	for (int i = 0 ; i < size - 1 ; ++i) {
		temp = (vertices[i].x-dx) * (vertices[i + 1].y-dy) - (vertices[i + 1].x-dx) * (vertices[i].y-dy);
		tempX += (vertices[i].x - dx + vertices[i + 1].x - dx) * temp;
		tempY += (vertices[i].y - dy + vertices[i + 1].y - dy) * temp;
	}
	temp = (vertices[size - 1].x-dx) * (vertices[0].y-dy) - (vertices[0].x-dx) * (vertices[size - 1].y-dy);
	tempX += (vertices[size - 1].x - dx + vertices[0].x - dx) * temp;
	tempY += (vertices[size - 1].y - dy + vertices[0].y - dy) * temp;
	double area6 = 6 * signedArea();
	tempX /= area6;
	tempY /= area6;
	return Vect2(tempX+dx, tempY+dy);
}

Vect2 Poly2D::averagePoint() const {
	double dx = (minX+maxX)/2.0;
	double dy = (minY+maxY)/2.0;
	double x = 0;
	double y = 0;
	for (int i = 0; i < (int) vertices.size(); i++) {
		x += vertices[i].x-dx;
		y += vertices[i].y-dy;
	}
	return Vect2(dx+x/vertices.size(), dy+y/vertices.size());
}

//	// area and centroid courtesy of Paul Bourke (1988) http://paulbourke.net/geometry/polyarea/
//	// these are for non self-intersecting polygons
//	// this calculation assumes that point 0 = point n (or the start point is counted twice)
//	/**
//	 * Return the horizontal area (in m^2) of this Poly3D.
//	 */
//	double Poly2D::area() const {
//		double a = 0;
//		vertices.push_back(vertices[0]);
//		for (int i = 0; i < vertices.size()-1; i++) {
//			a = a + (vertices[i].x()*vertices[i+1].y() - vertices[i+1].x()*vertices[i].y());
//		}
//		vertices.remove(vertices.size()-1);
//		return 0.5*a;
//	}


double Poly2D::innerDiameter() const {
	Vect2 cpos = centroid();
	double minDist = MAXDOUBLE;
	for (int i = 0; i < (int) vertices.size(); i++) {
		Vect2 v =  vertices[i];
		double dist = cpos.distance(v);
		if (dist < minDist) minDist = dist;
	}
	return minDist;
}


double Poly2D::outerDiameter() const {
	Vect2 cpos = centroid();
	double maxDist = 0.0;
	for (int i = 0; i < (int) vertices.size(); i++) {
		Vect2 v =  vertices[i];
		double dist = cpos.distance(v);
		if (dist > maxDist) maxDist = dist;
	}
	return maxDist;
}

double Poly2D::apBoundingRadius() const {
	Vect2 cpos = averagePoint();
	double maxDist = 0.0;
	for (int i = 0; i < (int) vertices.size(); i++) {
		Vect2 v =  vertices[i];
		double dist = cpos.distance(v);
		if (dist > maxDist) maxDist = dist;
	}
	return maxDist;
}


//	/**
//	 * Returns a GeneralPath representation for testing purposes.
//	 *
//	 * @return this polygon as a GeneralPath
//	 */
//	GeneralPath Poly2D::getGeneralPath() {
//		final double epsilon = 1e-6;
//		GeneralPath gp = null;
//		//if (gp == null) {
//		gp = GeneralPath();
//		gp.moveTo((float) vertices[0].x, (float) vertices[0].y);
//		int length = vertices.size();
//		if (std::abs(vertices[length - 1].x - vertices[0].x) < epsilon && std::abs(vertices[length-1].y - vertices[0].y) < epsilon) --length;
//		for (int i = 1 ; i <= length - 1 ; i++) {
//			gp.lineTo((float) vertices[i].x, (float) vertices[i].y);
//		}
//		gp.closePath();
//		//}
//		return gp;
//	}

std::string Poly2D::strVectArray(const std::vector<Vect2>& verts) const {
	std::string rtn = "";
	for (int i = 0; i < (int) verts.size(); i++) {
		rtn = rtn +" "+verts[i].toString();
	}
	return rtn;
}

std::string Poly2D::toString() const {
	return "Poly2D [vertices=" + strVectArray(vertices)
	  //+ ", boundingRect=" + boundingRect.toString()
	  + "]";
}



}

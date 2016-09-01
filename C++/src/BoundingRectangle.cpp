/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include "BoundingRectangle.h"
#include "format.h"


namespace larcfm {



	/** Create a bounding rectangle with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
  BoundingRectangle::BoundingRectangle() {
		clear();
	}	
	
	/** Create a bounding rectangle with the list of given points.
	 */
	BoundingRectangle::BoundingRectangle(const std::vector<Vect2>& vertices) {
		clear();
		for (int i = 0; i < (int) vertices.size(); i++) {
			add(vertices[i].x,vertices[i].y);
		}
		
		// Vect2 v0 = vertices[0];
		// double xmin = v0.x;
		// double ymin = v0.y;
		// double xmax = v0.x;
		// double ymax = v0.y;
		// //fpln(" %%%%%%%%% BoundingRectangle: vertices = "+strVectArray(vertices));
		// for (int i = 1; i < (int) vertices.size(); i++) {
		// 	Vect2 vi = vertices[i];
		// 	if (vi.x < xmin) xmin = vi.x;
		// 	if (vi.y < ymin) ymin = vi.y;
		// 	if (vi.x > xmax) xmax = vi.x;
		// 	if (vi.y > ymax) ymax = vi.y;
		// }
		// xMin = xmin;
		// xMax = xmax;
		// yMin = ymin;
		// yMax = ymax;
	}	

	/** Copy a bounding rectangle from an existing bounding rectangle
	 */
	BoundingRectangle::BoundingRectangle(const BoundingRectangle& br) {  //copy
	  //if (br != null) {
			xMin = br.xMin;
			xMax = br.xMax;
			yMin = br.yMin;
			yMax = br.yMax;
			zMin = br.zMin;
			zMax = br.zMax;
			xCenter = br.xCenter;
			//} else {
			//clear();
			//}
	}

	/** Reset this bounding rectangle to a structure with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
	void BoundingRectangle::clear() {
		xMin = MAXDOUBLE;
		xMax = -MAXDOUBLE;
		yMin = MAXDOUBLE;
		yMax = -MAXDOUBLE;
		zMin = MAXDOUBLE;
		zMax = -MAXDOUBLE;
		
		xCenter = MAXDOUBLE; // initially I used NaN, but this was slow.
	}

	/**
	 * Add a point to this bounding rectangle.
	 * @param x
	 * @param y
	 */
	void BoundingRectangle::add(double x, double y) {
		//fpln("min "+x+" "+xMin);
		xMin = std::min(x,xMin);
		yMin = std::min(y,yMin);
		xMax = std::max(x,xMax);
		yMax = std::max(y,yMax);
	}

	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param v
	 */
	void BoundingRectangle::add(const Vect2& v) {
	  //if (v == null) return;
		add(v.x,v.y);
	}
	
	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param x
	 * @param y
	 * @param z
	 */
	void BoundingRectangle::add(double x, double y, double z) {
		add(x,y);
		zMin = std::min(z,zMin);
		zMax = std::max(z,zMax);
	}

	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param v
	 */
	void BoundingRectangle::add(const Vect3& v) {
	  //if (v == null) return;
		add(v.x,v.y,v.z);
	}
	
	/**
	 * Add another bounding rectangle to this bounding rectangle.
	 * 
	 * @param br the other bounding rectangle (this rest is not changed).
	 */
	void BoundingRectangle::add(const BoundingRectangle& br) {
	  //if (br == null) return;
		
		if (xCenter == MAXDOUBLE) {
			LatLonAlt ur = LatLonAlt::mk(br.getMaxY(), br.getMaxX(), br.getMaxZ());
			LatLonAlt ll = LatLonAlt::mk(br.getMinY(), br.getMinX(), br.getMinZ());
			add(ur);
			add(ll);
		} else {
			add(br.getMaxX(),br.getMaxY(),br.getMaxZ());
			add(br.getMinX(),br.getMinY(),br.getMinZ());
		}
	}

	double BoundingRectangle::fix_lon(double lon) const {
		if (xCenter == MAXDOUBLE) {
			xCenter = lon;
		}
		
		if (std::abs(lon - xCenter) > M_PI) {
			if ((lon - xCenter > M_PI)) {
				return -M_PI + (lon - M_PI);
			} else { 
				return M_PI + (lon + M_PI);
			}
		}
		return lon;
	}

	
	/**
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param lla
	 */
	void BoundingRectangle::add(const LatLonAlt& lla2) {
	  //if (lla == null) return;
		LatLonAlt lla = lla2.normalize();
		
		if (xCenter == MAXDOUBLE) {
			xCenter = lla.lon();
		}
		
		LatLonAlt nlla = denormalize(lla);
		add(nlla.lon(),nlla.lat(),nlla.alt());
	}
	
	/**
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param p
	 */
	void BoundingRectangle::add(const Position& p) {
	  //if (p == null) return;
		if (p.isLatLon()) {
			add(p.lla()); //p.lon(),Util::to_pi2_cont(p.lat()),p.alt());
		} else {
			add(p.x(),p.y(),p.z());
		}
	}
	
	/**
	 * Return a LatLonAlt object that is consistent with this bounding rectangle.  This LatLonAlt object
	 * may have a longitude greater than 180 degrees or less than -180 degrees.
	 * @param lla a LatLonAlt object
	 * @return a LatLonAlt object with (possibly) non-standard longitude.
	 */
	LatLonAlt BoundingRectangle::denormalize(const LatLonAlt& lla2) const {
		LatLonAlt lla = lla2.normalize();
		return LatLonAlt::mk(lla.lat(),fix_lon(lla.lon()),lla.alt());
	}

	/**
	 * Return a Position object that is consistent with this bounding rectangle.  If this bounding
	 * rectangle contains latitude/longitude points, then the returned Position object
	 * may have a longitude greater than 180 degrees or less than -180 degrees.
	 * 
	 * @param p a Position object
	 * @return a LatLonAlt object with (possibly) non-standard longitude.
	 */
	Position BoundingRectangle::denormalize(const Position& p) const {
		if (p.isLatLon()) {
			return Position(denormalize(p.lla()));
		} else {
			return p;
		}
	}

	/**
	 * Determines if the given point is within the bounding rectangle. A point on
	 * the edges of this rectangle is also within the rectangle.
	 * 
	 * @param x the x coordinate of the point in question
	 * @param y the y coordinate of the point in question
	 * @return true if the point is within the bounding rectangle or on any of its
	 *         edges.
	 */
	bool BoundingRectangle::contains(double x,  double y) const {
		//fpln("x>=xMin: "+(x>=xMin));
		//fpln("x<=xMax: "+(x<=xMax));
		//fpln("y>=yMin: "+(y>=yMin));
		//fpln("y<=yMax: "+(y<=yMax));
		return (x >= xMin && x <= xMax && y >= yMin && y <= yMax);
	}

	bool BoundingRectangle::contains(const Vect2& v) const {
		return contains(v.x,v.y);
	}
	
	/** Is this Vect3 in the contained area?  (using only the X and Y coordinates)
	 * 
	 * @param v Vect3
	 * @return true if the point is contained
	 */
	bool BoundingRectangle::contains(const Vect3& v) const {
		return contains(v.x,v.y);
	}
	
	/** Is this position in the contained area?  (using only the X and Y coordinates)
	 * 
	 * @param p a position
	 * @return true if the point is contained
	 */
	bool BoundingRectangle::contains(const Position& p) const {
		if (p.isLatLon()) {
			return contains(p.lla());
		} else {
			return contains(p.x(), p.y());
		}
	}
	
	/** Is this LatLonAlt in the contained area?  (using only the latitude and longitude coordinates)
	 * 
	 * @param lla a position
	 * @return true if the point is contained
	 */
	bool BoundingRectangle::contains(const LatLonAlt& lla) const {
		return contains(fix_lon(lla.lon()), lla.lat());
	}
	
	/**
	 * Does the given rectangle share any points with this rectangle?  Only two 
	 * dimensions are used.
	 * 
	 * @param rect the BoundingRectangle to be tested for intersection
	 * @return true, if the supplied BoundingRectangle intersects this one
	 */
	bool BoundingRectangle::intersects(const BoundingRectangle& rect) const {
		return intersects(rect, 0.0);
	}

	/**
	 * Return true if any point in rect is within buffer of any point in this bounding rectangle. Only two dimensions are used
	 * @return true, if the supplied BoundingRectangle intersects this one
	 */
	bool BoundingRectangle::intersects(const BoundingRectangle& rect, double buffer) const {
		return ! (xMax+buffer < rect.getMinX() || xMin-buffer > rect.getMaxX() || yMax+buffer < rect.getMinY() || yMin-buffer > rect.getMaxY());
//		if (xMax+buffer < rect.getMinX() || xMin-buffer > rect.getMaxX() || yMax+buffer < rect.getMinY() || yMin-buffer > rect.getMaxY()) {
//		return false;
//	}
//	return true;
	}

	/**
	 * @return left bound
	 */
	double BoundingRectangle::getMinX() const {
		return xMin;
	}

	/**
	 * @return right bound
	 */
	double BoundingRectangle::getMaxX() const {
		return xMax;
	}

	/**
	 * @return bottom bound
	 */
	double BoundingRectangle::getMinY() const {
		return yMin;
	}

	/**
	 * @return top bound
	 */
	double BoundingRectangle::getMaxY() const {
		return yMax;
	}

	/**
	 * @return min Z bound
	 */
	double BoundingRectangle::getMinZ() const {
		return zMin;
	}

	/**
	 * @return max Z bound
	 */
	double BoundingRectangle::getMaxZ() const {
		return zMax;
	}
	
	Vect3 BoundingRectangle::centerVect() const {
		return Vect3((xMax+xMin)/2.0,(yMax+yMin)/2.0,(zMax+zMin)/2.0);
	}

	Position BoundingRectangle::centerPos() const {
		Vect3 v = centerVect();
		if (xCenter == MAXDOUBLE) {
			return Position(v);
		} else {
			return Position(LatLonAlt::mk(v.y, v.x, v.z).normalize());
		}
	}


	std::string BoundingRectangle::toString() const {
	  std::string rtn = "<" +Fm8(xMin)+" "+Fm8(xMax)+" "+Fm8(yMin)+" "+Fm8(yMax)+">";
	  return rtn;			
	}

}

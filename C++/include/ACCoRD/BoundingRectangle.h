/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BOUNDINGRECTANGLE_H_
#define BOUNDINGRECTANGLE_H_

#include "Vect2.h"
#include "Vect2.h"
#include "format.h"
#include "LatLonAlt.h"
#include "Position.h"
#include <string>
#include <vector>


namespace larcfm {
  
/**
 * A bounding rectangle for a 3-dimensional polygon. The bounding rectangle is the smallest rectangle that encloses a set 
 * of points. A point is contained in the rectangle if it lies within its boundary or on one of its edges. 
 * Thus, the bounding rectangle includes its boundary.<p>
 * 
 * Special processing is conducted for LatLonAlt objects (including those emmbedded in Position objects.
 * The special processing includes (1) altitude information is captured as a bound, but not used
 * in the "contains" operation, and (2) proper handling when the longitude "wraps around" at -180/180.
 * This second special processing means that a bounding rectangle for LatLonAlt is limited to 180 degrees
 * (half the earth).  Very erratic results will come from a bounding rectangle that has both LatLonAlt points
 * and normal euclidean points.<p>
 * 
 * This probably should have been called BoundingBox, because it is three dimensional.
 */
class BoundingRectangle { 

 private:
	double xMin;
	double xMax;
	double yMin;
	double yMax;
	double zMin;
	double zMax;
	mutable double xCenter;  // only used for LatLonAlt objects

 public:
	/** Create a bounding rectangle with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
	BoundingRectangle();
	
	/** Create a bounding rectangle with the list of given points.
	 * 
	 * @param vertices list of vertices
	 */
	BoundingRectangle(const std::vector<Vect2>& vertices);
 
	/** Copy a bounding rectangle from an existing bounding rectangle
	 * 
	 * @param br rectangle to copy
	 */
	BoundingRectangle(const BoundingRectangle& br);

	/** Reset this bounding rectangle to a structure with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
	void clear();

	/**
	 * Add a point to this bounding rectangle.
	 * @param x x coordinate
	 * @param y y coordinate
	 */
	void add(double x, double y);

	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param v
	 */
	void add(const Vect2& v);
	
	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param x x coordinate
	 * @param y y coordinate
	 * @param z z coordinate
	 */
	void add(double x, double y, double z);

	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param v vector
	 */
	void add(const Vect3& v);
	
	/**
	 * Add another bounding rectangle to this bounding rectangle.
	 * 
	 * @param br the other bounding rectangle (this rest is not changed).
	 */
	void add(const BoundingRectangle& br);

 private:
	double fix_lon(double lon) const;

 public:
	
	/**
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param lla point
	 */
	void add(const LatLonAlt& lla);
	
	/**
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param p point
	 */
	void add(const Position& p);
	
	/**
	 * Return a LatLonAlt object that is consistent with this bounding rectangle.  This LatLonAlt object
	 * may have a longitude greater than 180 degrees or less than -180 degrees.
	 * @param lla a LatLonAlt object
	 * @return a LatLonAlt object with (possibly) non-standard longitude.
	 */
	LatLonAlt denormalize(const LatLonAlt& lla) const;

	/**
	 * Return a Position object that is consistent with this bounding rectangle.  If this bounding
	 * rectangle contains latitude/longitude points, then the returned Position object
	 * may have a longitude greater than 180 degrees or less than -180 degrees.
	 * 
	 * @param p a Position object
	 * @return a LatLonAlt object with (possibly) non-standard longitude.
	 */
	Position denormalize(const Position& p) const;

	/**
	 * Determines if the given point is within the bounding rectangle. A point on
	 * the edges of this rectangle is also within the rectangle.
	 * 
	 * @param x the x coordinate of the point in question
	 * @param y the y coordinate of the point in question
	 * @return true if the point is within the bounding rectangle or on any of its
	 *         edges.
	 */
	bool contains(double x,  double y) const;

	bool contains(const Vect2& v) const;
	
	/** Is this Vect3 in the contained area?  (using only the X and Y coordinates)
	 * 
	 * @param v Vect3
	 * @return true if the point is contained
	 */
	bool contains(const Vect3& v) const;
	
	/** Is this position in the contained area?  (using only the X and Y coordinates)
	 * 
	 * @param p a position
	 * @return true if the point is contained
	 */
	bool contains(const Position& p) const;
	
	/** Is this LatLonAlt in the contained area?  (using only the latitude and longitude coordinates)
	 * 
	 * @param lla a position
	 * @return true if the point is contained
	 */
	bool contains(const LatLonAlt& lla) const;
	
	/**
	 * Does the given rectangle share any points with this rectangle?  Only two 
	 * dimensions are used.
	 * 
	 * @param rect the BoundingRectangle to be tested for intersection
	 * @return true, if the supplied BoundingRectangle intersects this one
	 */
	bool intersects(const BoundingRectangle& rect) const;

	/**
	 * Return true if any point in rect is within buffer of any point in this bounding rectangle. Only two dimensions are used
	 * 
	 * @param rect    another rectangle
	 * @param buffer  maximum distance and still considered an overlap
	 * @return true, if the supplied BoundingRectangle intersects this one
	 */
	bool intersects(const BoundingRectangle& rect, double buffer) const;

	/**
	 * @return left bound
	 */
	double getMinX() const;

	/**
	 * @return right bound
	 */
	double getMaxX() const;

	/**
	 * @return bottom bound
	 */
	double getMinY() const;

	/**
	 * @return top bound
	 */
	double getMaxY() const;

	/**
	 * @return min Z bound
	 */
	double getMinZ() const;

	/**
	 * @return max Z bound
	 */
	double getMaxZ() const;
	
	/**
	 * Return center vector value for this BoundingRectangle
	 * @return center
	 */
	Vect3 centerVect() const;

	/**
	 * Return center Position for this BoundingRectangle.
	 * If any LatLonAlt values were input to the BoundingRectangle, this will be a normalized (within [-pi,+pi]) LatLonAlt position.
	 * @return center
	 */
	Position centerPos()  const;

	std::string toString() const;
};
}


#endif /* BOUNDINGRECTANGLE_H_ */

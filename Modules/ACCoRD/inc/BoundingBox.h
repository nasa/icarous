/*
 * Copyright (c) 2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "BoundingRectangle.h"
#include "Vect2.h"
#include "format.h"
#include "LatLonAlt.h"
#include "Position.h"
#include <string>
#include <vector>

namespace larcfm {

/**
 * A BoundingRetangle that has been extended to 3 dimensions, so it includes height information.
 */
class BoundingBox {

private:
	BoundingRectangle br;
	double zMin;
	double zMax;

public:

/** Create a bounding rectangle with invalid values.  When the
 * first point is added, valid values are obtained.
 */
BoundingBox();

BoundingBox(double zMin, double zMax);

	/** Copy a bounding rectangle from an existing bounding box
	 * @param bb box to copy
	 */
BoundingBox(const BoundingBox& bb);

	/**
	 * Extend an existing BoundingRectangle with invalid height information.
	 * Height information will be assigned as additional points are added.
	 * @param br rectangle to copy
	 */
	 BoundingBox(const BoundingRectangle& br);

	/**
	 * Extend an existing BoundingRectangle to include explicit height information.
	 * @param br rectangle to copy
	 * @param zMin minimal height
	 * @param zMax maximal height
	 */
BoundingBox(const BoundingRectangle& br, double zMin, double zMax);



	/** Reset this bounding rectangle to a structure with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
void clear();
	
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
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param p point
	 */
void add(const Position& p);
	

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

double getMinLon() const;
double getMaxLon() const;
double getMinLat() const;
double getMaxLat() const;

	
Position upperRightTop();
	
Position lowerLeftBottom();
	
double getMinAlt();
double getMaxAlt();
	

BoundingRectangle getBoundRect() const;

bool intersects(const BoundingBox& bb);
	
BoundingBox linear(const Velocity& v, double dt);

std::string toString();
	
};

}

#endif /* BOUNDINGBOX_H_ */

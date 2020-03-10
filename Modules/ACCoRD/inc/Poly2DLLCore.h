/*
 * Poly2DLL.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef Poly2DLLCore_H_
#define Poly2DLLCore_H_

//#include "BoundingRectangle.h"
#include <vector>
#include "Vect2.h"
#include "LatLonAlt.h"
//#include "GeneralPath.h"

namespace larcfm {

/**
 * Encapsulates a geometric polygon. The polygon is defined in terms of its vertex coordinates. 
 * This implementation assumes a simply connected polygon. The Polygon is otherwise quite general allowing multiply
 * connected regions. The class provides a containment test for points and uses bounding rectangles
 * to speed up computations.
 */
class Poly2DLLCore {

//private:
//	static const double BUFF;

public:

	static std::pair<bool,bool> upshot_crosses_edge(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, int i);
	static bool edges_crossed_upto(const std::vector<LatLonAlt>& p, const LatLonAlt& s, int i, int j);
	static std::pair<int,bool> number_upshot_crosses_upto(const std::vector<LatLonAlt>& p, const LatLonAlt& ll) ;
	static std::vector<LatLonAlt> fix_polygon(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF);
	static bool spherical_inside_prelim(const std::vector<LatLonAlt>& p, const LatLonAlt& ll) ;
	/**
	 * Main function
	 * @param p list of points (polygon)
	 * @param ll point
	 * @param BUFF buffer
	 * @return true if inside
	 */
	static bool spherical_inside(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF);
	static bool spherical_outside_prelim(const std::vector<LatLonAlt>& p, const LatLonAlt& ll);
	/**
	 * Main function
	 * @param p list of points (polygon)
	 * @param ll point
	 * @param BUFF buffer
	 * @return true if inside
	 */
	static bool spherical_outside(const std::vector<LatLonAlt>& p, const LatLonAlt& ll, double BUFF);
};


}

#endif /* Poly2DLLCore_H_ */

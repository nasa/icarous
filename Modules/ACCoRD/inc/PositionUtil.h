/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef POSITIONUTIL_H_
#define POSITIONUTIL_H_

#include "Velocity.h"
#include "Position.h"

namespace larcfm {


/**
 * GreatCircle and VectFuns functions lifted to Positions
 */
class PositionUtil {

public:
	static bool collinear(const Position & p0, const Position & p1, const Position & p2);

	static Position interpolate(const Position & v1, const Position & v2, double f);

	/**
	 * Return angle between P1-P2 and P2-P3
	 * @param p1 point 1
	 * @param p2 point 2 (intersection of two lines)
	 * @param p3 point 3
	 * @return angle between two lines
	 */
	static double angle_between(const Position & p1, const Position & p2, const Position & p3);

	static std::pair<Position,double> intersection(const Position & so, const Velocity & vo, const Position & si, const Velocity & vi);

	static std::pair<Position,double> intersection(const Position & so1, const Position & so2, double dto, const Position & si1, const Position & si2);

	static Position closestPoint(const Position & a, const Position & b, const Position & x);

	static Position closestPointOnSegment(const Position & a, const Position & b, const Position & x);

	static Position behind(const Position & a, const Position & b, const Position & x);

	static bool behind(const Position & p1, const Position & p2, const Velocity & vo);

	static int passingDirection(const Position & so, const Velocity & vo, const Position & si, const Velocity & vi);

	static int dirForBehind(const Position & so, const Velocity & vo, const Position & si, const Velocity & vi);
};

}

#endif /* POSITIONUTIL_H_ */


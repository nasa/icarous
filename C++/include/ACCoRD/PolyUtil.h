/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef POLYUTIL_H_
#define POLYUTIL_H_

#include "SimplePoly.h"
#include "PolyPath.h"
#include "Velocity.h"
#include "Position.h"
#include "Plan.h"
#include "Triple.h"
#include <vector>
#include <string>

namespace larcfm {

class PolyUtil {

private:
class SIComparator {
public:
    inline bool operator ()(const Triple<Position,int,double>& lhs, const Triple<Position,int,double>& rhs) const
    {
        return lhs.third < rhs.third;
    }
};



public:

	static SimplePoly convexHull(const std::vector<Position>& plist, double bottom, double top);

	static SimplePoly convexHull(const SimplePoly& p);

	static SimplePoly convexHull(const std::vector<SimplePoly>& p);

	static SimplePoly convexHull(const SimplePoly& p, double buffer);

	static Position pushOut(const SimplePoly& poly, const Position& p, double buffer);

	static Position pushOut(const SimplePoly& poly, int i, double buffer);

	static SimplePoly simplify2(const SimplePoly& p, double buffer);

	static SimplePoly simplify(const SimplePoly& p, double buffer);

	static SimplePoly simplify(const SimplePoly& p);

	static SimplePoly simplifyToSize(const SimplePoly& p, int num);

	static SimplePoly stretchOverTime(const SimplePoly& sp, const Velocity& v, double timeBefore, double timeAfter);

	static PolyPath stretchOverTime(const PolyPath& pbase, double timeBefore, double timeAfter);

	static SimplePoly bufferedConvexHull(const SimplePoly& p, double hbuff, double vbuff);

	static PolyPath bufferedConvexHull(const PolyPath& pbase, double hbuff, double vbuff);

	static bool intersectsPolygon2D(const Position& so, const Velocity& vo, const SimplePoly& sp, const Velocity& vp, double T, double incr);

	static bool intersectsPolygon(const Position& so, const Velocity& vo, const SimplePoly& sp, const Velocity& vp, double T, double incr);

	static std::pair<double,std::string> intersectsPolygon2D(const Plan& p, const PolyPath& pp, double B, double T, double incr);

	static Plan reducePlanAgainstPolys(const Plan& p, const std::vector<PolyPath>& paths, double incr);

	static Plan reducePlanAgainstPolys(const Plan& plan, double gs, const std::vector<PolyPath>& paths, double incr);

	static std::pair<double,std::string> isPlanInConflictWx(const Plan& plan, const std::vector<PolyPath>& paths, double start, double end, double incr);

	static std::pair<double,std::string>  isPlanInConflictWx(const Plan& plan, const std::vector<PolyPath>& paths, double incr);

	static std::pair<double,std::string> isPlanInConflictWx(const Plan& plan, const std::vector<PolyPath>& paths, double incr, double fromTime);

	static double calculateWxExitTime(const Plan& plan, const std::vector<PolyPath>& paths, double incr, double entryTime);

}; //class

}// namespace

#endif

/*
 * Copyright (c) 2011-2019 United States Government as represented by
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
#include "Util.h"
#include <vector>
#include <string>

namespace larcfm {

class PolyUtil {

private:
	class SIComparator {
	public:
		inline bool operator ()(const Triple<Position,double,double>& o1, const Triple<Position,double,double>& o2) const
		{
			if (Util::almost_equals(o1.second, o2.second)) {
					return o1.third <  o2.third;
			}
			return o1.second < o2.second;
		}
};



public:

/**
 * Return a polygon that is the convex hull of the listed points.  This uses the Graham scan algorithm.
 * For purposes of this algorithm, we only consider x, y values of positions (since all calculations are relative and no points are moved).
 * This uses the NE-most point as the origin, to allow for our track computations (0 is north) and proceeds in a clockwise fashion
 * This assumes that the resulting hull will not include the north or south poles (for lat lon positions)
 * @param plist list of positions
 * @param bottom lower altitude
 * @param top top altitude
 * @return SimplePoly
 */
static SimplePoly convexHull(const std::vector<Position>& plist, double bottom, double top);

static SimplePoly convexHull(const SimplePoly& p);

	/**
	 * Returns the convex hull of a set of polygons.
	 * This assumes that the resulting hull will not include the north or south poles.
	 * @param p list of polygons
	 * @return convex hull (top and bottom from max min values
	 */
static SimplePoly convexHull(const std::vector<SimplePoly>& p);

/**
 * Returns the convex hull of a polygon that has been expanded by (approximately) the given buffer size.
 * This is done by adding additional points around each polygon point and taking the convex hull of them all.
 * This assumes that the resulting hull will not include the north or south poles.
 * @param p polygon
 * @param buffer buffer
 * @return polygon
 */
static SimplePoly convexHull(const SimplePoly& p, double buffer);

/**
 * Return a position that is buffer distance further away from the poly's centroid
 * @param poly polygon
 * @param p position
 * @param buffer buffer
 * @return position
 */
static Position pushOut(SimplePoly& poly, const Position& p, double buffer);

/**
 * Return a position that is buffer distance further away from the poly's centroid at vertex i
 *
 * @param poly polygon
 * @param i    index
 * @param buffer buffer
 * @return position
 */
static Position pushOut(SimplePoly& poly, int i, double buffer);

static SimplePoly simplify2(SimplePoly& p, double buffer);

/**
 * Attempt to (over) approximate the given polygon with one with fewer edges.
 * @param p original polygon
 * @param buffer approximate increase in size in any dimension.
 * @return new larger polygon, or original polygon on failure
 */
static SimplePoly simplify(SimplePoly& p, double buffer);

static SimplePoly simplify(SimplePoly& p);

	/**
	 * Attempt to over-approximate a polygons with one with the specified number of vertices.
	 * This will automatically increase the buffer by up to 30% of the original's max radius value,
	 * and either stop there or when we have no more than the desired number of vertices. 
	 * @param p original polygon
	 * @param num desired number of vertices
	 * @return simple polygon
	 */
static SimplePoly simplifyToSize(SimplePoly& p, int num);

	/**
	 * Stretch a polygon so that it covers (at least) what the original would as it moves over a given time range.
	 * The uses a convex hull, so will be an over-approximation.  There may be inaccuracies for very long periods of 
	 * time if using geodesic coordinates.<p>
	 * 
	 * Example: Given a polygon W and an aircraft A with a timeAfter of 100 seconds, a conflict detection for 
	 * A against the stretched polygon W' is approximately equivalent to the disjunction of conflict detections 
	 * (any positive is a positive) of the set A' against the original W, where A' is the set A plus all of "echos" 
	 * timeshifted up to 100 seconds into the past, modulo distortion from geodetic projections and convex hull expansions.  Effectively 
	 * this is saying that if the conflict detection of A vs. W' is clear, then A will be clear of W and any aircraft that 
	 * precisely follow A will also be clear of W for up to 100 seconds.  (If it were not clear for 100 seconds, then A 
	 * would have impacted the extended W', which "arrives" 100 seconds earlier than W would.)<p>
	 * 
	 * Similarly for timeBefore.<p>
	 * 
	 * If A is clear of W', then it is also clear of the original W, which is a subset of W'.
	 * 
	 * Note this technique does not necessarily work for morphing polygons.
	 * 
	 * @param sp base polygon
	 * @param v average velocity of polygon
	 * @param timeBefore time before stated position to cover (in sec)
	 * @param timeAfter time after stated position to cover (in sec)
	 * @return  enlarged polygon
	 * 
	 */
static SimplePoly stretchOverTime(const SimplePoly& sp, const Velocity& v, double timeBefore, double timeAfter);

/**
 * Given a polypath, expand the polygons on it so they at least cover the areas that they would when over a longer time.
 * Note: this does not work for MORPHING paths, and will convert them instead to USER_VEL_FINITE paths.
 * Polygons are expanded to a new convex hull, meaning this will be an over-approximation.
 * This may also not be accurate for paths with very long legs in geodetic coordinates.
 * This may also overestimate the polygons at the start and end of each leg in the path.
 * @param pbase starting path
 * @param timeBefore time before the base path time to cover (relative, in seconds)
 * @param timeAfter time after the base path time to cover (relative, in seconds)
 * @return new path with expanded polygons.
 */
static PolyPath stretchOverTime(const PolyPath& pbase, double timeBefore, double timeAfter);

/**
 * Return a convex hull that has been expanded by hbuff and vbuff.
 * This extends the polygons around points, both perpendicular to the segments and directly away from vertices, approximating a round join.
 * There may be points outside the resulting hull (near vertices) that are slightly closer to the original than the requested buffer.
 * @param p base polygon
 * @param hbuff size of horizontal buffer (approx)
 * @param vbuff size of vertical buffer
 * @return convex hull
 */
static SimplePoly bufferedConvexHull(SimplePoly& p, double hbuff, double vbuff);

/**
 * Return a path where polygons are replaced by convex hulls that have been expanded by hbuff and vbuff.
 * @param pbase base polygon path
 * @param hbuff size of horizontal buffer (approx)
 * @param vbuff size of vertical buffer
 * @return path of convex hulls
 */
static PolyPath bufferedConvexHull(const PolyPath& pbase, double hbuff, double vbuff);

//	/**
//	 * This is a SLOW, APPROXIMATE test for 2D intersection between a moving polygon and a moving point.  For more accurate
//	 * (and verified) tests for this property, see polygon functions in the ACCoRD framework, specifically classes that implement
//	 * the DetectionPolygon interface and CDSSPolygon.
//	 * @param so point-mass starting position
//	 * @param vo point-mass velocity
//	 * @param sp polygon starting position
//	 * @param vp polygon average velocity
//	 * @param T end time for test (relative)
//	 * @param incr time increment for search (&gt; 0)
//	 * @return true if the point mass will intersect with the polygon at or before time T
//	 */
//	static bool intersectsPolygon2D(const Position& so, const Velocity& vo, SimplePoly& sp, const Velocity& vp, double T, double incr);
//
//	/**
//	 * This is a SLOW, APPROXIMATE test for 3D intersection between a moving polygon and a moving point.  For more accurate
//	 * (and verified) tests for this property, see polygon functions in the ACCoRD framework, specifically classes that implement
//	 * the DetectionPolygon interface and CDSSPolygon.
//	 * @param so point-mass starting position
//	 * @param vo point-mass velocity
//	 * @param sp polygon starting position
//	 * @param vp polygon average velocity
//	 * @param T end time for test (relative)
//	 * @param incr time increment for search (&gt; 0)
//	 * @return true if the point mass will intersect with the polygon at or before time T
//	 */
//	static bool intersectsPolygon(const Position& so, const Velocity& vo, SimplePoly& sp, const Velocity& vp, double T, double incr);

	/**
	 * This is a SLOW, APPROXIMATE test for 2D intersection between a moving polygon and a moving point.  For more accurate 
	 * (and verified) tests for this property, see polygon functions in the ACCoRD framework, specifically classes that implement 
	 * the DetectionPolygon interface and CDIIPolygon.
	 * @param p plan describing point-mass trajectory
	 * @param pp path describing polygon movement
	 * @param B start time to check (absolute)
	 * @param T end time to check (absolute)
	 * @param incr time increment for search (&gt; 0)
	 * @return time of loss of separation if aircraft will intersect with the polygon between times B and T
	 *         and polygon name that plan is in conflict with
	 * 
	 * Note: the return time will be at an interior point, and the name of the polygon 
	 */
static double intersectsPolygon2D(const Plan& p, PolyPath& pp, double B, double T, double incr);

/**
 * Attempt to minimize a given plan (by removing points) such that new segments do not intersect with any polygons.
 * This uses the intersectsPolygon2D() check, and so has the limitations associated with it.  Use ACCoRD
 * calls instead for better performance and/or more accuracy.  Returns a new plan that is hopefully smaller than
 * the original plan.
 *
 * @param p      plan
 * @param paths  paths
 * @param incr   increment
 * @return plan
 */
//static Plan reducePlanAgainstPolys(const Plan& p, const std::vector<PolyPath>& paths, double incr);

static Plan reducePlanAgainstPolys(const Plan& plan, double gs, std::vector<PolyPath>& paths, double incr,
		bool leadInsPresent, const std::vector<PolyPath>& containment);

static Plan reducePlanAgainstPolys(const Plan& pln, double gs, std::vector<PolyPath>& paths, double incr,
		bool leadInsPresent);

static std::pair<double,std::string> isPlanInConflictWx(const Plan& plan, std::vector<PolyPath>& paths, double start, double end, double incr);

static std::pair<double,std::string>  isPlanInConflictWx(const Plan& plan, std::vector<PolyPath>& paths, double incr);

static std::pair<double,std::string> isPlanInConflictWx(const Plan& plan, std::vector<PolyPath>& paths, double incr, double fromTime);

	/**
	 * Returns time of intersection if the plan is NOT free of polygons from time start to time end.  This may miss intrusions of up to incr sec
	 * so it may not detect 
	 * @param plan   plan to test
	 * @param paths  set of polygons
	 * @param B  start time of search
	 * @param T    end time of search
	 * @param incr   search increment
	 * @return  time of intersection with a polygon if it occurs when search time interval, otherwise -1
	 *          and polygon name that plan is in conflict with
	 */
static bool isPlanContained(const Plan& plan, const std::vector<PolyPath>& paths, double B, double T, double incr);

	/** Given that plan is in loss with paths at time entryTime, then this function returns the exit time
	 * 
	 * @param plan plan
	 * @param paths polygon paths
	 * @param incr increment
	 * @param entryTime time
	 * @return exit time
	 */
static double calculateWxExitTime(const Plan& plan, std::vector<PolyPath>& paths, double incr, double entryTime);

}; //class

}// namespace

#endif

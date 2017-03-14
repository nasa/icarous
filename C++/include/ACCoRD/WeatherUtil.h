/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef WEATHERUTIL_H_
#define WEATHERUTIL_H_

#include "Plan.h"
#include "DensityGrid.h"
#include "PolyPath.h"
#include <vector>
#include <string>

namespace larcfm {

class WeatherUtil {
public:
	/**
	 * Produce a plan that travels between two end points and is approximately conflict free of any polygons.
	 *
	 * Note:
	 * - "Approximately conflict free" means that there should be no intrusions that are greater than gridSize.  This is intended to be a first step
	 * in a more thorough algorithm to avoid polygons.
	 * - This methods can produce solutions with a small intrusion into one or more polygons (less than 1 gridSize).
	 * These usually happen at acute corners of polygons.  Reducing the grid size will mitigate this (to the reduced grid size).
	 * - If reduceGridPath is true, this is more likely.
	 * - FastPolygonReroute uses an overestimation of polygons.
	 *
	 * @param own   plan for ownship
	 * @param paths polypath collection of polygons to be routed around
	 * @param gridSize approximate size of grid square (in meters)
	 * @param buffer approximate size of extended grid area (in meters)
	 * @param factor weighting for closeness to path (1.0 = approximate distance in grid squares)
	 * @param T_p lookahead time, relative to timeOfCurrentPosition.  Conflicts beyond this time will be ignored, which can cause unexpected behavior.  It is recommended that this be turned off (set to -1) for most cases.
	 * @param fastPolygonReroute,
	 * @param reduceGridPath,		perform a low-level smoothing operation on the resulting path.  this may result in a solution that is not completely conflict free.
	 * @param timeOfCurrentPosition  location of aircraft in its plan
	 * @param reRouteLeadIn          leadin time from current position (wait this amount of time before maneuvar)
	 *
	 * @return new ownship plan (empty and/or error set on fail) and densitygrid used (for visualization)
	 * Note that even a successful return does not guarantee a completely conflict-free return path, only one that does not intrude into any polygons by more than gridSize.
	 */
	static std::pair<Plan,DensityGrid> reRouteWx(const Plan& own, const std::vector<PolyPath>& paths, double gridSize, double buffer,
			double factor, double T_p, const std::vector<PolyPath>& containment, bool fastPolygonReroute, bool reduceGridPath,
			double timeOfCurrentPosition, double reRouteLeadIn);

	/**
	 * As reRouteWx(), but this will internally expand the polygons so that they cover a larger area, either representing their total coverage over
	 * a period of time of duration timeBefore+timeAfter, and/or over-approximating them to mitigate solutions that cut corners of polygons.
	 * Note that setting reduceGridPath to true may still result in a solutions that still contains conflicts.
	 *
	 * @param own ownship plan
	 * @param paths list of polygons to avoid
	 * @param gridSize desired grid size (m)
	 * @param buffer buffer to expand the grid to allow go-around solutions (m)
	 * @param factor weight indicating desire to retain original path (0 = abandon original path, higher numbers = favor original path; transition point in the 3-10 range)
	 * @param T_p lookahead time for search, relative to timeOfCurrentPosition.  Conflicts beyond this time will be ignored, which can cause unexpected behavior.  It is recommended that this be turned off (set to -1) in most cases.
	 * @param containment list of polygons to remain within
	 * @param fastPolygonReroute if set to true, use a less exact (greater over-approximation) but much faster search
	 * @param reduceGridPath if set to true, attempt to smooth resulting path (may re-introduce minor conflicts)
	 * @param timeOfCurrentPosition absolute time ownship start
	 * @param reRouteLeadIn time (in s) to allow for initial and final velocity matching of aircraft (should probably be enough to make a 180 degree turn)
	 * @param expandPolygons if set to true, internally over-approximate the polygons to mitigate missed conflicts
	 * @param timeBefore if greater than zero, ensure the final path is continuously clear for this amount of time before the given solution
	 * @param timeAfter if greater than zero, ensure the final path is continuously clear for this amount of time after the given solution
	 * @param reRouteReduction if true, perform more aggressive simplification of resulting path.
	 * @return Solution path and the density grid used for the search (the later is intended for further display or analysis)
	 * If no solution is found, return a null or empty path.
	 *
	 * Note that even a successful return does not guarantee a completely conflict-free return path, only one that does not intrude into any polygons by more
	 * than gridSize.  Setting expandPolygons to true should mitigate this, and setting reduceGridPath may re-introduce intrusions.
	 */
	static std::pair<Plan,DensityGrid> reRouteWx(const Plan& own, const std::vector<PolyPath>& paths, double gridSize, double buffer,
			double factor, double T_p, const std::vector<PolyPath>& containment, bool fastPolygonReroute, bool reduceGridPath,
			double timeOfCurrentPosition, double reRouteLeadIn, bool expandPolygons, double timeBefore, double timeAfter,
			bool reRouteReduction);

private:
	static std::pair<Plan,DensityGrid> reRouteWithAstar(const std::vector<PolyPath>& paths, const Plan& ownship, double gridSize, double buffer,
			double factor, double gs, const std::vector<PolyPath>& containment, double endT,
			bool fastPolygonReroute, bool reduceGridPath);

    static Plan setAltitudes(const Plan& pp, double firstLegVs);

};
}
#endif

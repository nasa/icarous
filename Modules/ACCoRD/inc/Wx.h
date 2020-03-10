/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef WX_H_
#define WX_H_

#include "Plan.h"
#include "WeatherUtil.h"
#include "PolyPath.h"
#include <vector>
#include <string>

namespace larcfm {

class Wx {
public:
	static Plan reRoute(const Plan& own, const PolyPath& pp) ;

	static Plan reRoute(const Plan& own, const std::vector<PolyPath>& paths, double cellSize,
			      bool adhere, bool solutionSmoothing) ;


	static Plan reRoute(const Plan& own, const std::vector<PolyPath>& paths, bool adhere, bool solutionSmoothing);

	static Plan reRoute(const Plan& own, const std::vector<PolyPath>& paths, bool adhere,
			      bool solutionSmoothing, double timeOfCurrentPosition, double reRouteLeadIn) ;


	static Plan reRoute(const Plan& own, const std::vector<PolyPath>& paths, bool adhere);

	/**
	 * Produce a plan that travels between two end points and is approximately conflict free of any polygons.
	 *
	 * (Note this code is experimental and may not always produce correct results.)
	 *
	 * Note:
	 * - "Approximately conflict free" means that there should be no intrusions that are greater than cellSize.  This
	 * is intended to be a first step in a more thorough algorithm to avoid polygons.
	 * - This methods can produce solutions with a small intrusion into one or more polygons (less than 1 cellSize).
	 * These usually happen at acute corners of polygons.  Reducing the grid size will mitigate this (to the reduced grid size).
	 * - If reduceGridPath is true, this is more likely.
	 * - FastPolygonReroute uses an overestimation of polygons.
	 *
	 * @param startPos
	 * @param startTime
	 * @param endPos
	 * @param gs
	 * @param paths a list of polypaths (i.e. a collection of polygons) to be routed around
	 * @param cellSize approximate size of grid square (in meters)
	 * @param buffer approximate size of extended grid area (in meters)
	 * @param factor weighting for closeness to path (1.0 = approximate distance in grid squares)
	 * @param T_p lookahead time, relative to timeOfCurrentPosition::  Conflicts beyond this time will be ignored, which can cause unexpected behavior.  It is recommended that this be turned off (set to -1) for most cases.
     * @param containment
	 * @param fastPolygonReroute,
	 * @param reduceGridPath,		perform a low-level smoothing operation on the resulting path.  this may result in a solution that is not completely conflict free.
	 * @param timeOfCurrentPosition  location of aircraft in its plan (if -1 then, the first point of plan is used)
	 * @param reRouteLeadIn          leadin time from current position (wait this amount of time before maneuvar)
	 *
	 * @return new ownship plan (empty and/or error set on fail) and a Densitygrid used (for visualization)
	 * Note that even a successful return does not guarantee a completely conflict-free return path, only one that does not intrude into any polygons by more than cellSize.
	 */
	static std::pair<Plan,DensityGrid> reRoute(Position startPos, double startTime,
			Position endPos, double gs,
			const std::vector<PolyPath>& paths, double cellSize, double buffer,
			double factor, double T_p, std::vector<PolyPath> containment,
			double reRouteLeadIn) ;



private:

};
}
#endif

/*
 * Copyright (c) 2016-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DensityGridMovingPolys.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <limits>
#include "Plan.h"
#include "Position.h"
#include "PolyPath.h"
#include "BoundingRectangle.h"
#include "DensityGrid.h"
#include "Triple.h"
#include "NavPoint.h"

namespace larcfm {


DensityGridMovingPolys::DensityGridMovingPolys(const Plan& p, int buffer, double squareSize, double gs_, const std::vector<PolyPath>& ps, const std::vector<PolyPath>& cs) : DensityGridTimed(p, buffer, squareSize) {
	paths = ps;
	contains = cs;
	startTime_ = p.getFirstTime();
//	double endT = -1.0;
	gs = gs_;
}

double DensityGridMovingPolys::getWeightT(int x, int y, double t) const {
	if (lookaheadEndTime > 0 && t > lookaheadEndTime) {
		return 0.0;
	}
	std::pair<int,int> xy = std::pair<int,int>(x,y);
	if (weights.find(xy) == weights.end()) {
		return std::numeric_limits<double>::infinity();
	}
	double w = weights.find(xy)->second;
	double cost = 0;
	Position cent = center(x,y);
	for (int i = 0; i < (int) paths.size(); i++) {
		if (paths[i].contains2D(cent, t)) {
			cost = std::numeric_limits<double>::infinity();
			break;
		}
	}
	if (contains.size() > 0) {
		bool within = (contains.size() == 0); // no containment is vacuously fulfilled
		for (int i = 0; i < (int) contains.size(); i++) {
			if (contains[i].contains2D(cent, t)) {
				within = true;
				break;
			}
		}
		if (!within) {
			cost = std::numeric_limits<double>::infinity();
		}
	}
	return w + cost;
}

double DensityGridMovingPolys::getWeightT(const std::pair<int,int>& xy, double t) const {
	return getWeightT(xy.first,xy.second,t);
}

double DensityGridMovingPolys::getWeightT(const Triple<int,int,int>& pii) const {
	return getWeightT(pii.first,pii.second, pii.third);
}


}


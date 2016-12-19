/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DensityGridMovingPolysEst.h"
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
#include <limits>
#include <vector>

namespace larcfm {


DensityGridMovingPolysEst::DensityGridMovingPolysEst(const Plan& p, int buffer, double squareSize, double gs_, const std::vector<PolyPath>& ps, const std::vector<PolyPath>& containment) :
		DensityGridTimed(p, buffer, squareSize) {
	for (int i = 0; i < ps.size(); i++) {
		Triple<Plan,double,double> pp = ps[i].buildPlan();
		plans.push_back(pp);
	}
	if (containment.size() > 0) {
		for (int i = 0; i < containment.size(); i++) {
			// static plans have already been handled in StratwayCore with a call to setWeightsWithin()
			if (!containment[i].isStatic()) {
				Triple<Plan,double,double> cc = containment[i].buildPlan();
				contains.push_back(cc);
			}
		}
	}
	startTime_ = p.getFirstTime();
	gs = gs_;

}


double DensityGridMovingPolysEst::getWeightT(int x, int y, double t) const {
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
	// disallow anything within one of the weather cells
	for (int i = 0; i < plans.size(); i++) {
		if (t >= plans[i].first.getFirstTime() && t <= plans[i].first.getLastTime()) {
			Position p = plans[i].first.position(t);
			double D = plans[i].second;
			double centdist = cent.distanceH(p);
			if (centdist <= D) {
				cost = std::numeric_limits<double>::infinity();
				break;
			}
		}
	}

	// disallow anything outside of one of the contains
	bool within = (contains.size() == 0);
	for (int i = 0; i < contains.size(); i++) {
		if (t >= contains[i].first.getFirstTime() && t <= contains[i].first.getLastTime()) {
			Position p = contains[i].first.position(t);
			double D = contains[i].second;
			double centdist = cent.distanceH(p);
			if (centdist <= D) {
				within = true;
				break;
			}
		}
	}
	if (!within) {
		cost = std::numeric_limits<double>::infinity();
	}

	return w + cost;
}


double DensityGridMovingPolysEst::getWeightT(const std::pair<int,int>& xy, double t) const {
	return getWeightT(xy.first,xy.second,t);
}

double DensityGridMovingPolysEst::getWeightT(const Triple<int,int,int>& pii) const {
	return getWeightT(pii.first,pii.second, pii.third);
}


}


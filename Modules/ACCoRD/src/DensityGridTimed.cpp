/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DensityGridTimed.h"
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

DensityGridTimed::~DensityGridTimed() {}

DensityGridTimed::DensityGridTimed(const BoundingRectangle& b, const NavPoint& start, const Position& end, double startT, double groundSpeed, int buffer, double sqSz, bool ll) : DensityGrid(b, start, end, buffer, sqSz, ll) {
	startTime_ = startT;
	lookaheadEndTime = -1.0;
	gs = groundSpeed;
}

DensityGridTimed::DensityGridTimed(const Plan& p, int buffer, double squareSize) : DensityGrid(p, buffer, squareSize) {
	startTime_ = p.getFirstTime();
	gs = p.averageGroundSpeed();
	lookaheadEndTime = -1.0;
}


double DensityGridTimed::getLookaheadEndTime() const {
	return lookaheadEndTime;
}

void DensityGridTimed::setLookaheadEndTime(double t) {
	lookaheadEndTime = t;
}

double DensityGridTimed::getWeightT(int x, int y, double t) const {
	if (lookaheadEndTime > 0 && t > lookaheadEndTime) return 0.0;
	std::pair<int,int> xy = std::pair<int,int>(x,y);
	if (weights.find(xy) == weights.end()) {
		return std::numeric_limits<double>::infinity();
	} else {
		return weights.find(xy)->second;
	}
}


double DensityGridTimed::getWeightT(const std::pair<int,int>& xy, double t) const {
	return getWeightT(xy.first,xy.second,t);
}

double DensityGridTimed::getWeightT(const Triple<int,int,int>& pii) const {
	return getWeightT(pii.first,pii.second, pii.third);
}

double DensityGridTimed::getGroundSpeed() const {
	return gs;
}

}


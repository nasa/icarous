/*
 * Copyright (c) 2016-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DENSITYGRIDTIMEDSEARCH_H_
#define DENSITYGRIDTIMEDSEARCH_H_

#include <vector>
#include "Position.h"
#include "DensityGrid.h"

namespace larcfm {

class DensityGridTimedSearch{

	public:
	virtual std::vector<std::pair<int,int> > searchT(DensityGridTimed& dg, const Position& startPos, const Position& endPos, double startTime, double gs) const = 0;

	virtual std::vector<std::pair<int,int> > optimalPathT(DensityGridTimed& dg) const = 0;

};
}
#endif /* FORMAT_H_ */


/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DENSITYGRIDSEARCH_H_
#define DENSITYGRIDSEARCH_H_

#include <vector>
#include "Position.h"
#include "DensityGrid.h"

namespace larcfm {

class DensityGridSearch {

	public:
	virtual std::vector<std::pair<int,int> > search(DensityGrid& dg, const Position& startPos, const Position& endPos) const = 0;

	virtual std::vector<std::pair<int,int> > optimalPath(DensityGrid& dg) const = 0;

};
}
#endif /* FORMAT_H_ */

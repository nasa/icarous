/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DENSITYGRIDMOVINGPOLYSEST_H_
#define DENSITYGRIDMOVINGPOLYSEST_H_

#include "DensityGridTimed.h"
#include "Plan.h"
#include "Position.h"
#include "PolyPath.h"
#include "BoundingRectangle.h"
#include "DensityGrid.h"
#include "Triple.h"
#include "NavPoint.h"
#include <vector>

namespace larcfm {

class DensityGridMovingPolysEst : public DensityGridTimed {

protected:
	std::vector<Triple<Plan,double,double> > plans; // plan, width, height
	std::vector<Triple<Plan,double,double> > contains;


	public:
	DensityGridMovingPolysEst(const Plan& p, int buffer, double squareSize, double gs, const std::vector<PolyPath>& ps, const std::vector<PolyPath>& containment);

	virtual double getWeightT(int x, int y, double t) const;

	virtual double getWeightT(const std::pair<int,int>& xy, double t) const;

	virtual double getWeightT(const Triple<int,int,int>& pii) const;

};
}
#endif /* FORMAT_H_ */


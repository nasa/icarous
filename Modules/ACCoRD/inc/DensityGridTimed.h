/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DENSITYGRIDTIMED_H_
#define DENSITYGRIDTIMED_H_

#include "Plan.h"
#include "Position.h"
#include "PolyPath.h"
#include "BoundingRectangle.h"
#include "DensityGrid.h"
#include "Triple.h"
#include "NavPoint.h"

namespace larcfm {

class DensityGridTimed : public DensityGrid {


	protected:
	double lookaheadEndTime;
	double gs;


	public:
	DensityGridTimed(const BoundingRectangle& b, const NavPoint& start, const Position& end, double startT, double groundSpeed, int buffer, double sqSz, bool ll);

	DensityGridTimed(const Plan& p, int buffer, double squareSize);

	virtual ~DensityGridTimed();

	double getLookaheadEndTime() const;

	void setLookaheadEndTime(double t);

	virtual double getWeightT(int x, int y, double t) const;

	virtual double getWeightT(const std::pair<int,int>& xy, double t) const;

	virtual double getWeightT(const Triple<int,int,int>& pii) const;

	double getGroundSpeed() const;

};
}
#endif /* FORMAT_H_ */


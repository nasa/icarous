/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DENSITYGRIDASTARSEARCH_H_
#define DENSITYGRIDASTARSEARCH_H_

#include <string>
#include <map>
#include <set>
#include <vector>
#include "format.h"
#include "Plan.h"
#include "Position.h"
#include "PolyPath.h"
#include "Triple.h"
#include "BoundingRectangle.h"
#include "DensityGrid.h"
#include "DensityGridSearch.h"
#include "DensityGridTimed.h"
#include "DensityGridTimedSearch.h"

namespace larcfm {

class DensityGridAStarSearch : public DensityGridSearch, public DensityGridTimedSearch {

public:
	double dirWeight; //1.0
	double distWeight; //1.0

	DensityGridAStarSearch();

	class FringeEntry {
	public:
		int x;
		int y;
		double t;
		double cost;
		std::vector<std::pair<int,int> > path;

		FringeEntry(std::pair<int,int> xy, double cc) {
			t = 0;
			x = xy.first;
			y = xy.second;
			cost = cc;
			path = std::vector<std::pair<int,int> >();
			path.push_back(xy);
		}

		FringeEntry(std::pair<int,int> xy, double cc, FringeEntry f) {
			x = xy.first;
			y = xy.second;
			cost = f.cost + cc;
			path = std::vector<std::pair<int,int> >();
			path.insert( path.end(), f.path.begin(), f.path.end() );
			path.push_back(xy);
		}

		FringeEntry(std::pair<int,int> xy, double t_d, double cc) {
			x = xy.first;
			y = xy.second;
			t = t_d;
			cost = cc;
			path = std::vector<std::pair<int,int> >();
			path.push_back(xy);
		}

		FringeEntry(std::pair<int,int> xy, double t_d, double cc, FringeEntry f) {
			x = xy.first;
			y = xy.second;
			t = t_d;
			cost = f.cost + cc;
			path = std::vector<std::pair<int,int> >();
			path.insert( path.end(), f.path.begin(), f.path.end() );
			path.push_back(xy);
		}

		std::pair<int,int> getCell() {
			return std::pair<int,int>(x, y);
		}

		Triple<int,int,int> getCell3() {
			return Triple<int,int,int>(x, y, (int)t);
		}

		double getCost() {
			return cost;
		}

		std::vector<std::pair<int,int> > getPath() {
			return path;
		}

		std::string toString() {
			return "("+Fm0(x)+","+Fm0(y)+","+Fm0(t)+") = "+Fm3(cost);
		}

	    bool operator < (const FringeEntry& f2) const
	    {
	        return (cost < f2.cost);
	    }


	};

private:
	bool contains(const std::vector<std::pair<int,int> >& gPath, const std::pair<int,int>& pii) const;
	bool contains(const std::vector<Triple<int,int,int> >& gPath, const Triple<int,int,int>& pii) const;

public:


	bool sameDirection(FringeEntry c, std::pair<int,int> cell2) const;

	double directionCost(FringeEntry c, std::pair<int,int> cell2) const;

	double distanceCost(std::pair<int,int> cell2, int endx, int endy) const;

	std::vector<std::pair<int,int> > astar(DensityGrid& dg, int endx, int endy, std::vector<FringeEntry>& fringe, std::vector<std::pair<int,int> >& searched) const;

	std::vector<std::pair<int,int> > astarT(DensityGridTimed& dg, int endx, int endy, double gs, std::vector<FringeEntry>& fringe, std::vector<Triple<int,int,int> >& searched) const;

	virtual std::vector<std::pair<int,int> > search(DensityGrid& dg, const Position& startPos, const Position& endPos) const;

	virtual std::vector<std::pair<int,int> > searchT(DensityGridTimed& dg, const Position& startPos, const Position& endPos, double startTime, double gs) const;

	virtual std::vector<std::pair<int,int> > optimalPath(DensityGrid& dg) const;

	virtual std::vector<std::pair<int,int> > optimalPathT(DensityGridTimed& dg) const;

};
}
#endif /* FORMAT_H_ */


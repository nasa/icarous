/*
 * Copyright (c) 2016-2019 United States Government as represented by
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
	static double dirWeight; //0.5
	static double distWeight; //1.0
	static double predDistWeight; //2.0
	static bool fourway;
	static bool oldHeuristics;
	static bool gridDistances;
	static const double diagonalCost;


	DensityGridAStarSearch();

	class FringeEntry {
	public:
		int x;
		int y;
		double t;
		double actualCost;
		double predictedCost;
		std::vector<std::pair<int,int> > path;

		FringeEntry(std::pair<int,int> xy, double cc, double pc) {
			t = 0;
			x = xy.first;
			y = xy.second;
			actualCost = cc;
			predictedCost = pc;
			path = std::vector<std::pair<int,int> >();
			path.push_back(xy);
		}

		FringeEntry(std::pair<int,int> xy, double ac, double pc, FringeEntry f) {
			x = xy.first;
			y = xy.second;
			actualCost = f.actualCost + ac;
			predictedCost = pc;
			path = std::vector<std::pair<int,int> >();
			path.insert( path.end(), f.path.begin(), f.path.end() );
			path.push_back(xy);
		}

		FringeEntry(double t_d, std::pair<int,int> xy, double ac, double pc) {
			x = xy.first;
			y = xy.second;
			t = t_d;
			actualCost = ac;
			predictedCost = pc;
			path = std::vector<std::pair<int,int> >();
			path.push_back(xy);
		}

		FringeEntry(double t_d, std::pair<int,int> xy, double ac, double pc, FringeEntry f) {
			x = xy.first;
			y = xy.second;
			t = t_d;
			actualCost = f.actualCost + ac;
			predictedCost = pc;
			path = std::vector<std::pair<int,int> >();
			path.insert( path.end(), f.path.begin(), f.path.end() );
			path.push_back(xy);
		}

		std::pair<int,int> getCell() const {
			return std::pair<int,int>(x, y);
		}

		Triple<int,int,int> getCell3() const {
			return Triple<int,int,int>(x, y, (int)t);
		}

		double getActualCost() const {
			return actualCost;
		}

		double getPredictedCost() const {
			return predictedCost;
		}

		double getTotalCost() const {
			return actualCost+predictedCost;
		}

		std::vector<std::pair<int,int> > getPath() const {
//fpln("DensiyGridAstarSearch getpath:"+Fobj(path));
			return path;
		}

		std::string toString() const {
			return "("+Fm0(x)+","+Fm0(y)+","+Fm4(t)+") = "+Fm6(actualCost)+"+"+Fm6(predictedCost);
		}

	    bool operator < (const FringeEntry& f2) const
	    {
	        return (getTotalCost() < f2.getTotalCost());
	    }


	};

private:
	bool contains(const std::vector<std::pair<int,int> >& gPath, const std::pair<int,int>& pii) const;
	bool contains(const std::vector<Triple<int,int,int> >& gPath, const Triple<int,int,int>& pii) const;

public:

	static double getDirectionWeight() {
		return dirWeight;
	}

	static void setDirectionWeight(double dw) {
		dirWeight = dw;
	}

	static double getDistanceWeight() {
		return distWeight;
	}

	static void setDistanceWeight(double dw) {
		distWeight = dw;
	}

	static bool isFourway() {
		return fourway;
	}

	static void setFourway(bool fw) {
		fourway = fw;
	}

	static bool isOldHeuristics() {
		return oldHeuristics;
	}

	static void setOldHeuristics(bool oh) {
		oldHeuristics = oh;
	}

	static double getPredictedDistanceWeight() {
		return predDistWeight;
	}

	static void setPredictedDistanceWeight(double pdw) {
		predDistWeight = pdw;
	}


//	bool sameDirection(FringeEntry c, std::pair<int,int> cell2) const;

	double directionCost(FringeEntry c, int x2, int y2, double directionWeight) const;

	double predictedDistanceCost(std::pair<int,int> cell2, int endx, int endy, double distanceWeight) const;

	std::vector<std::pair<int,int> > astar(DensityGrid& dg, int endx, int endy, std::vector<FringeEntry>& fringe, std::vector<std::pair<int,int> >& searched, bool fourway, double directionWeight, double distanceWeight, double predictedDistanceWeight) const;

	std::vector<std::pair<int,int> > astarT(DensityGridTimed& dg, int endx, int endy, double gs, std::vector<FringeEntry>& fringe, std::vector<Triple<int,int,int> >& searched, bool fourway, double directionWeight, double distanceWeight, double predictedDistanceWeight) const;

	virtual std::vector<std::pair<int,int> > search(DensityGrid& dg, const Position& startPos, const Position& endPos) const;

	virtual std::vector<std::pair<int,int> > searchT(DensityGridTimed& dg, const Position& startPos, const Position& endPos, double startTime, double gs) const;

	virtual std::vector<std::pair<int,int> > optimalPath(DensityGrid& dg) const;

	virtual std::vector<std::pair<int,int> > optimalPathT(DensityGridTimed& dg) const;
};
}
#endif /* FORMAT_H_ */


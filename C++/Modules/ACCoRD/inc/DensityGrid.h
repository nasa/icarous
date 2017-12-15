/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DENSITYGRID_H_
#define DENSITYGRID_H_

#include <string>
#include <map>
#include <set>
#include <vector>
#include "Plan.h"
#include "Position.h"
#include "PolyPath.h"
#include "BoundingRectangle.h"

namespace larcfm {

class DensityGrid{
protected:
	Position startPoint_;
	double startTime_;
	Position endPoint_;
	std::map<std::pair<int,int>,double> weights;
	std::map<std::pair<int,int>,double>::iterator weightspos;
	std::map<std::pair<int,int>,std::pair<double,double> > corners;
	std::map<std::pair<int,int>,std::pair<double,double> >::iterator cornerspos;
//	std::map<std::pair<int,int>,double> searchedWeights;
//	std::map<std::pair<int,int>,double>::iterator searchedweightspos;
//	std::set<std::pair<int,int> > marked;
//	std::set<std::pair<int,int> >::iterator markedpos;
	std::vector<std::pair<int,int> >::iterator gPathpos;
	bool latLon;
	double squareSize;
	double squareDist;
	int sz_x;
	int sz_y;
	BoundingRectangle bounds; // overall bounds
	double minSearchedWeightValue;
	double maxSearchedWeightValue;


public:
	DensityGrid();

	DensityGrid(const BoundingRectangle& b, const NavPoint& start, const Position& end, int buffer, double sqSz, bool ll);

	DensityGrid(const Plan& p, int buffer, double squareSize);

	DensityGrid(const Plan& p, double startT, int buffer, double squareSize);

protected:
	void init(const BoundingRectangle& b, const NavPoint& start, const Position& end, int buffer, double sqSz, bool ll);


private:
	static double linearEstY(double lati, double dn);

	static double distEstLatLon(double lat1, double lat2);

public:
	/**
	 * Approximate size of square, in either meters (if Euclidean) or radians (if latlon)
	 * @return size
	 */
	double getNativeSquareDist() const;

	/**
	 * Approximate size of square, in meters
	 * @return size
	 */
	double getSquareDist() const;


	/**
	 * Adjust all grid corner coordinates so that the start point it in the center of its grid.
	 */
	void snapToStart();

	std::pair<int,int> gridPosition(Position p) const;

	bool containsCell(const std::pair<int,int>& xy) const;

	/**
	 * Note: the grid size should be 1 larger than expected (to allow for the first point to be in the middle of the square)
	 * The SW corner of the plan bounding box should be in square (buffer,buffer)
	 * @return size
	 */
	int sizeX() const;

	/**
	 * Note: the grid size should be 1 larger than expected (to allow for the first point to be in the middle of the square)
	 * @return size
	 */
	int sizeY() const;

	Position getPosition(int x, int y) const;

	Position getPosition(const std::pair<int,int>& pii) const;

	Position center(int x, int y) const;

	Position center(const std::pair<int,int>& pii) const;

	double getWeight(int x, int y) const;

	double getWeight(const std::pair<int,int>& pii) const;

	void setWeight(int x, int y, double d);

	void clearWeight(int x, int y);

//	double getSearchedWeight(int x, int y);
//
//	void setSearchedWeight(int x, int y, double d);
//
//	void clearSearchedWeights();
//
//	double getMinSearchedWeightValue();
//
//	double getMaxSearchedWeightValue();

//	bool getMark(const std::pair<int,int>& pii);
//
//	bool getMark(int x, int y);
//
//	void setMark(const std::pair<int,int>& pii, bool b);
//
//	void setMark(int x, int y, bool b);
//
//	void clearMarks();

	/**
	 * Set all weights to d in grid cells minX to maxX and minY to maxY, inclusive
	 * 
	 * @param minX minimum x
	 * @param minY minimum y
	 * @param maxX maximum x
	 * @param maxY maximum y
	 * @param d weight
	 */
	void setWeights(int minX, int minY, int maxX, int maxY, double d);

	/**
	 * Set all weights to d
	 * 
	 * @param d weight
	 */
	void setWeights(double d);

	/**
	 * Clear all weights of grid squares whose center is outside the given (static) polygon.
	 * @param poly
	 */
	void clearWeightsOutside(SimplePoly poly);

	/**
	 * set all weights of grid squares whose center is inside the given (static) polygon.
	 * @param poly polygon
	 * @param d weight
	 */
	void setWeightsInside(SimplePoly poly, double d);


	void clearWeights();

	Position startPoint() const;

	Position endPoint() const;

	double startTime() const;

private:

	static bool validPair(const std::pair<int,int>&  p);

public:

	std::vector<std::pair<int,int> > gridPath(const Plan& p) const;

	void printGridPath(const std::vector<std::pair<int,int> >& gPath);

	 enum Direction {undef, N, NE, E, SE, S, SW, W, NW};

private:

	Direction direction(const std::pair<int,int>& p1, const std::pair<int,int>& p2);

public:

	std::vector<std::pair<int,int> > thin(const std::vector<std::pair<int,int> >& gPath);

	std::vector<std::pair<int,int> > reduceGridPath(const std::vector<std::pair<int,int> >& gp);

	Plan gridPathToPlan(const std::vector<std::pair<int,int> >& gPath, double gs, double vs, bool reduce);

private:

	// this is NOT a general function!
	bool contains(const std::vector<std::pair<int,int> >& gPath, const std::pair<int,int>& pii) const;

public:

	void setProximityWeights(const std::vector<std::pair<int,int> >& gPath, double factor, bool applyToUndefined);

	/**
	 * Weight against plan points.
	 * @param p  plan
	 * @param factor factor
	 * @param applyToUndefined true, if should be applied to undefined points
	 */
	void setProximityWeights(const Plan& p, double factor, bool applyToUndefined);

	void setPolyWeights(double time, const std::vector<PolyPath>& path);

	std::string toString() const;

	void printWeights() const;

//	void printSearchedWeights() const;

	void printCorners() const;
};
}
#endif /* FORMAT_H_ */


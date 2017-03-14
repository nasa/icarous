/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "WeatherUtil.h"
#include "DensityGrid.h"
#include "DensityGridSearch.h"
#include "DensityGridAStarSearch.h"
#include "DensityGridMovingPolys.h"
#include "DensityGridMovingPolysEst.h"
#include "Plan.h"
#include "PolyPath.h"
#include "PlanUtil.h"
#include "PolyUtil.h"
#include "format.h"
#include <vector>
#include <float.h>

namespace larcfm {
std::pair<Plan,DensityGrid> WeatherUtil::reRouteWx(const Plan& own, const std::vector<PolyPath>& paths, double gridSize, double buffer,
		double factor, double T_p, const std::vector<PolyPath>& containment,	bool fastPolygonReroute, bool reduceGridPath,
		double timeOfCurrentPosition, double reRouteLeadIn) {
	//f.pln("WeatherUtil.reRouteWx 1");
	//f.pln("WeatherUtil.reRouteWx own="+own+" paths="+paths+" gridsize="+gridSize+" buffer="+buffer+" factor="+factor+" Tp="+T_p+" containment="+containment.size()+" fast="+fastPolygonReroute+" red="+reduceGridPath+" timeOfCurrentPosition="+timeOfCurrentPosition+" reRouteLeadIn="+reRouteLeadIn);
	//Plan p = plans.getPlan(0);
	Plan solution;
	if (own.size() < 2) {
		//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$ reRoute: FAIL !! reRoute needs at least 2 points !!");
		own.addError("reRouteWx failed (plan too short).");
		return std::pair<Plan,DensityGrid>(own,DensityGrid());
	}
	double tmCurrentPos = timeOfCurrentPosition;
	NavPoint currentPos = own.point(0);
	NavPoint finalPos = own.point(own.size()-1).makeTime(DBL_MAX); // ensure it is at end of plan, will correct later
	if (tmCurrentPos < 0) {
		tmCurrentPos = own.getFirstTime();
	}
	if (tmCurrentPos < own.getFirstTime() || tmCurrentPos > own.getLastTime()) {
		own.addError("reRouteWx: specified time of current position ("+Fm3(tmCurrentPos)+") is not within plan");
		return std::pair<Plan,DensityGrid>(own,DensityGrid());
	}
	int currentIdx = own.getIndex(tmCurrentPos);
	if (currentIdx >= 0) {
		currentPos = own.point(currentIdx);
	} else {
		currentPos = NavPoint(own.position(tmCurrentPos), tmCurrentPos);
	}
	//f.pln(" $$$$ reRouteWx: tmCurrentPos = "+tmCurrentPos+" currentPos = "+currentPos);
	Velocity currentVel = own.velocity(tmCurrentPos);
	Plan nPlan = own;
	double startTime = tmCurrentPos + reRouteLeadIn;
	double endTime = own.getLastTime() - reRouteLeadIn;
	Position endLeadinPos = own.position(endTime);
	//f.pln(" $$ reRouteWx: tmCurrentPos = "+tmCurrentPos+"  startTime = "+startTime+" endTime = "+endTime);
	//		solution = null;
	DensityGrid dg = DensityGrid(); // this used for visualization only
	// cut down working plan to startTime
	if (paths.size() == 0) {
		solution = own;
	} else if (own.isTimeInPlan(startTime) && own.isTimeInPlan(endTime)) {
		nPlan = PlanUtil::cutDown(own, startTime, endTime);
		double gs = nPlan.initialVelocity(0).gs();
		if (gs <= 0) {
			fpln("reRouteWx: ERROR Warning!!! initial groundspeed is zero!");
		}
		double endT = -1.0;
		if (timeOfCurrentPosition > 0 && T_p > 0) {
			endT = timeOfCurrentPosition + T_p;
		}
		std::pair<Plan,DensityGrid> rr = reRouteWithAstar(paths, nPlan, gridSize, buffer, factor, gs, containment, endT,
				fastPolygonReroute, reduceGridPath);
		Plan rrPlan = rr.first;
		dg = rr.second;
		//f.pln(" $$$$ reRouteWx: rr.first = "+rr.first);
		//f.pln(" $$$$ reRouteWx: rr.second = "+rr.second);
		if (rrPlan.size() > 0) {
			rrPlan = setAltitudes(rrPlan,currentVel.vs());
			if (rrPlan.size() > 0) {
				rrPlan.addNavPoint(currentPos);
				rrPlan.addNavPoint(finalPos);
				rrPlan.setTimeGSin(rrPlan.size()-1, nPlan.initialVelocity(0).gs());
				//f.pln(" $$$$ reRoute: rrPlan = "+rrPlan+" "+localParams.unZigReroute);
				solution = rrPlan;
			} else {
				own.addError("reRouteWx failed to find solution"); // +speedString+".");
				//					solution = null;
			}
		} else {
			//f.pln(" $$ reRouteWx did not find a solution"); // +speedString+"!!");
			//				solution = null;
		}
		//			} // for
		//f.pln(" reRouteWx: rrPlan = "+rrPlan);
	} else {
		//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$ reRoute: FAIL !! $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		own.addError("reRouteWx failed to find solution (insuffcient plan length).");
		//			solution = null;
	}
	if (solution.size() > 0) {
		PlanUtil::checkMySolution(solution, tmCurrentPos, currentPos.position(), currentVel);
	}
	return std::pair<Plan,DensityGrid>(solution,dg);
}// reRouteWx




std::pair<Plan,DensityGrid> WeatherUtil::reRouteWx(const Plan& own, const std::vector<PolyPath>& paths, double gridSize, double buffer,
		double factor, double T_p, const std::vector<PolyPath>& containment, bool fastPolygonReroute, bool reduceGridPath,
		double timeOfCurrentPosition, double reRouteLeadIn, bool expandPolygons, double timeBefore, double timeAfter,
		bool reRouteReduction) {
	std::vector<PolyPath> npaths = std::vector<PolyPath>();
	for (int i = 0; i < (int) paths.size(); i++) {
		PolyPath exp = PolyUtil::stretchOverTime(paths[i], timeBefore, timeAfter);
		if (expandPolygons) {
			// expand by 1/2 diagonal of grid size to ensure no missed sections.
			exp = PolyUtil::bufferedConvexHull(exp, gridSize*0.71, 0.0);
		}
		npaths.push_back(exp);
	}
	std::pair<Plan,DensityGrid> pr = reRouteWx(own, npaths, gridSize, buffer, factor, T_p, containment, fastPolygonReroute, reduceGridPath, timeOfCurrentPosition, reRouteLeadIn);
	Plan ret = pr.first;
	if (reRouteReduction && ret.size() == 0) {
		double incr = 10.0;
		double gs = own.initialVelocity(0).gs();
		Plan ret2 = PolyUtil::reducePlanAgainstPolys(ret, gs, npaths, incr);
		if (ret2.size() > 0) {
			ret = ret2;
		}
		//f.pln("reduced plan="+ret);
	}
	return std::pair<Plan,DensityGrid>(ret, pr.second);
}


std::pair<Plan,DensityGrid> WeatherUtil::reRouteWithAstar(const std::vector<PolyPath>& paths, const Plan& ownship, double gridSize, double buffer,
		double factor, double gs, const std::vector<PolyPath>& containment, double endT,
		bool fastPolygonReroute, bool reduceGridPath) {
	//f.pln("WeatherUtil.reRouteWithAstar");
//	bool markPath = true; // mark all cells in the grid that are in the original optimal path
	int b = (int)std::ceil(buffer/gridSize);
	DensityGridTimed *dg;
	if (fastPolygonReroute) {
		dg = new DensityGridMovingPolysEst(ownship, b, gridSize, gs, paths, containment);
	} else {
		dg = new DensityGridMovingPolys(ownship, b, gridSize, gs, paths, containment);
	}
	dg->setLookaheadEndTime(endT);
	dg->snapToStart();
	std::vector<std::pair<int,int> > origpath = dg->gridPath(ownship);
	bool noContainment = true;
	if (containment.size() > 0) {
		for (int i = 0; i < (int) containment.size(); i++) {
			if (containment[i].isStatic()) {
				dg->setWeightsInside(containment[i].getPoly(0), 1.0);
				noContainment = false;
			}
		}
	}
	if (noContainment) {
		dg->setWeights(1.0);
	}
	if (factor > 0.0) {
		dg->setProximityWeights(origpath, factor, false);
	}
	DensityGridAStarSearch dgs;
	std::vector<std::pair<int,int> > gPath = dgs.optimalPathT(*dg);
	if (gPath.size() == 0) {
		DensityGrid dg2 = *dg;
		delete dg;
		return std::pair<Plan,DensityGrid>(Plan(),dg2);
	}
	Plan plan2 = dg->gridPathToPlan(gPath,gs,0.0,reduceGridPath);
	DensityGrid dg2 = *dg;
	delete dg;
	if (plan2.size() == 0) {
		return std::pair<Plan,DensityGrid>(Plan(),dg2);
	}
	return std::pair<Plan,DensityGrid>(plan2,dg2);
}



Plan WeatherUtil::setAltitudes(const Plan& pp, double firstLegVs) {
	Plan p = Plan(pp);
	//f.pln(" $$ setAltitudes: startAlt = "+p.point(0).alt());
	//f.pln(" $$ setAltitudes: endAlt = "+p.point(p.size()-1).alt());
	if (p.size() < 2) return p;
	bool altPreserve = true;
	p.setAltVSin(1, firstLegVs, altPreserve);
	if (p.size() < 3) return p;
	double vs = (p.point(p.size()-1).alt() - p.point(1).alt())/(p.point(p.size()-1).time() - p.point(1).time());
	//f.pln(" $$ setAltitudes: vs = "+Units.str("fpm",vs));
	for (int j = 2; j < p.size()-1; j++) {
		p.setAltVSin(j, vs, altPreserve);
	}
	return p;
}

}

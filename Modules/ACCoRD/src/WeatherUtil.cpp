/*
 * Copyright (c) 2016-2018 United States Government as represented by
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
//#include "DebugSupport.h"
#include "format.h"
#include <vector>
#include <float.h>

namespace larcfm {

std::pair<Plan,DensityGrid> WeatherUtil::reRouteWx(const Plan& own, std::vector<PolyPath>& paths, double cellSize, double gridExtension,
		double adherenceFactor, double T_p, const std::vector<PolyPath>& containmentPolys,	bool fastPolygonReroute,
		double timeOfCurrentPosition, double reRouteLeadIn, bool solutionSmoothing) {
//	fpln("\n-------------------------------------------------------------------------------");
//		for (int j = 0; j < (int) paths.size(); j++) {
//			fpln("WeatherUtil.reRouteWx paths["+Fm0(j)+"] = "+paths[j].toStringShort()+" \n");
//	}
//	fpln("WeatherUtil.reRouteWx own="+own.toString()+" paths[0]="+paths[0].toString()   );
//			+" cellSize="+Units::str("NM",cellSize)+" gridExtension="+Units::str("NM",gridExtension)
//	       +" adherenceFactor="+Fm1(adherenceFactor)+"\n fastPolygonReroute ="+bool2str(fastPolygonReroute)
//			+" timeOfCurrentPosition="+Fm1(timeOfCurrentPosition)+" reRouteLeadIn="+Fm1(reRouteLeadIn)+" solutionSmoothing ="+bool2str(solutionSmoothing));
//	DebugSupport::dumpPlanAndPolyPaths(own,paths,"reRouteWx_input");
	Plan solution;
	if (own.size() < 2) {
		own.addError("reRouteWx failed (plan too short).");
		return std::pair<Plan,DensityGrid>(own,DensityGrid());
	}
	double tmCurrentPos = timeOfCurrentPosition;
	NavPoint currentPos = own.point(0);
	NavPoint finalPos = own.point(own.size()-1).makeTime(1E30); // ensure it is at end of plan, will correct later
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
	//fpln(" $$$$ reRouteWx: tmCurrentPos = "+Fm1(tmCurrentPos)+" currentPos = "+currentPos.toString());
	Velocity currentVel = own.velocity(tmCurrentPos);
	Plan nPlan = own;
	double startTime = tmCurrentPos + reRouteLeadIn;
	double endTime = own.getLastTime() - reRouteLeadIn;
	//Position endLeadinPos = own.position(endTime);
	//f.pln(" $$ reRouteWx: tmCurrentPos = "+tmCurrentPos+"  startTime = "+startTime+" endTime = "+endTime);
	//		solution = null;
	DensityGrid dg = DensityGrid(); // this used for visualization only
	if (own.isTimeInPlan(startTime) && own.isTimeInPlan(endTime)) {
		nPlan = PlanUtil::cutDown(own, startTime, endTime);
		double gs = nPlan.initialVelocity(0).gs();
		if (gs <= 0) {
			fpln("reRouteWx: ERROR Warning!!! initial groundspeed is zero!");
		}
		double endT = -1.0;
		if (timeOfCurrentPosition > 0 && T_p > 0) {
			endT = timeOfCurrentPosition + T_p;
		}
		//fpln(" $$$$ reRouteWx: nPlan = "+nPlan.toString());
//		for (int j = 0; j < (int) paths.size(); j++) {
//		   fpln(" $$$$$ WeatherUtil::reRouteWx: paths["+Fm0(j)+"] = "+paths[j].toStringShort());
//		}
		//DebugSupport::dumpPlanAndPolyPaths(own,paths,"reRouteWx_input");
		std::pair<Plan,DensityGrid> rr = reRouteWithAstar(paths, nPlan, cellSize, gridExtension, adherenceFactor, gs, containmentPolys, endT,
				fastPolygonReroute);
		Plan rrPlan = rr.first;
		dg = rr.second;
		//fpln(" $$$$ reRouteWx: ASTAR.rrPlan = "+rrPlan.toString());
		// TODO: REMOVE NEXT LINE
		//DebugSupport::dumpPlan(rrPlan,"ASTAR.rrPlan");
		if (rrPlan.size() > 0) {
			rrPlan = setAltitudes(rrPlan,currentVel.vs());
			if (rrPlan.size() > 0) {
				rrPlan.addNavPoint(currentPos);
				rrPlan.addNavPoint(finalPos);
				double gs0 = nPlan.initialVelocity(0).gs();
				rrPlan.setTimeGsIn(rrPlan.size()-1, gs0);
				//fpln(" $$$$ reRoute: rrPlan = "+rrPlan.toString());
				solution = rrPlan;
			} else {
				//fpln(" $$$$ reRoute failed to find solution");
				own.addError("reRouteWx failed to find solution"); // +speedString+".");
	            //solution = null;
			}
		} else {
			//fpln(" $$ reRouteWx did not find a solution"); // +speedString+"!!");
			//				solution = null;
		}
		//fpln(" $$$reRouteWx>>>>>> AFTER: rrPlan = "+rrPlan.toString());
	} else {
		//fpln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$ reRoute: FAIL !! $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		own.addError("reRouteWx failed to find solution (insufficient plan length).");
		//			solution = null;
	}
	if (solution.size() > 0) {
		if (solutionSmoothing) {
			double incr = 10.0;
			double gs = own.initialVelocity(0).gs();
			bool leadInsPresent = (reRouteLeadIn > 0);
			solution = PolyUtil::reducePlanAgainstPolys(solution, gs, paths, incr, leadInsPresent, containmentPolys);
			//fpln("$$$$ reRouteWx: reduced plan="+solution.toString());
		}
		PlanUtil::isCurrentPositionUnchanged(solution, tmCurrentPos, currentPos.position());
	}
	//fpln(" $$$$ reRouteWx: EXIT. solution.size = "+Fm0(solution.size()));
	return std::pair<Plan,DensityGrid>(solution,dg);
}// reRouteWx




std::pair<Plan,DensityGrid> WeatherUtil::reRouteWxExpand(const Plan& own, const std::vector<PolyPath>& paths, double cellSize, double gridExtension,
		double adherenceFactor, double T_p, const std::vector<PolyPath>& containment, bool fastPolygonReroute,
		double timeOfCurrentPosition, double reRouteLeadIn, bool expandPolygons, double timeBefore, double timeAfter,
		bool solutionSmoothing) {
	//fpln(" $$$ reRouteWxExpand:  expandPolygons = "+bool2str(expandPolygons)+" timeBefore = "+Fm1(timeBefore)+ " timeAfter = "+Fm1(timeAfter));
	//fpln(" $$$$ reRouteWxExpand: paths.size() ="+Fm0(paths.size())); //+" paths[0].getPathMode() = "+paths[0].getPathMode());
	std::vector<PolyPath> npaths = ReRouteExpandIt(paths, cellSize, expandPolygons, timeBefore, timeAfter);
//fpln("=====");
//for (int i = 0; i < paths.size(); i++) {
//fpln(npaths[i].toString());
//}
	std::pair<Plan,DensityGrid> pr = reRouteWx(own, npaths, cellSize, gridExtension, adherenceFactor, T_p, containment,
			fastPolygonReroute, timeOfCurrentPosition, reRouteLeadIn, solutionSmoothing);
	Plan ret = pr.first;
	//fpln(" $$$ reRouteWxExpand: EXIT: ret = "+ret.toString());
	return std::pair<Plan,DensityGrid>(ret, pr.second);
}

std::vector<PolyPath> WeatherUtil::ReRouteExpandIt(const std::vector<PolyPath>& paths, double cellSize,
		bool expandPolygons, double timeBefore, double timeAfter) {
	std::vector<PolyPath> npaths; //  = std::vector<PolyPath>();
	//fpln(" $$$$$$ expandIt: timeAfter = "+Fm3(timeAfter));
	for (int i = 0; i < (int) paths.size(); i++) {
		PolyPath exp;
		if (timeBefore > 0 || timeAfter > 0) {
			exp = PolyUtil::stretchOverTime(paths[i], timeBefore, timeAfter);
		} else {
			exp = paths[i];
		}
		//fpln(" $$$$$$ expandIt: i = "+Fm0(i)+", exp = "+exp.toString());
		if (expandPolygons) {
			// expand by 1/2 diagonal of grid size to ensure no missed sections.
			exp = PolyUtil::bufferedConvexHull(exp, cellSize*0.71, 0.0);
		}
		npaths.push_back(exp);
	}
	//fpln(" $$$$$$ expandIt: RETURN npaths.size() = "+npaths.size());
	return npaths;
}



std::pair<Plan,DensityGrid> WeatherUtil::reRouteWithAstar(const std::vector<PolyPath>& paths, const Plan& ownship, double gridSize, double buffer,
		double factor, double gs, const std::vector<PolyPath>& containment, double endT,
		bool fastPolygonReroute) {
	//fpln(" $$$$$$$ reRouteWithAstar: ownship = "+ownship.toString());
	//fpln("WeatherUtil.reRouteWithAstar: ENTER: fastPolygonReroute = "+bool2str(fastPolygonReroute));
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

	// shortcut if no search
	if (paths.size() == 0 && containment.size() == 0) {
		DensityGrid dg2 = *dg;
		return std::pair<Plan,DensityGrid>(ownship,dg2);
	}

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
//fpln(Fobj(gPath));
	Plan plan2 = dg->gridPathToPlan(gPath,gs,0.0);
	DensityGrid dg2 = *dg;
	delete dg;
	if (plan2.size() == 0) {
		return std::pair<Plan,DensityGrid>(Plan(),dg2);
	}
	//fpln("WeatherUtil.reRouteWithAstar: EXIT plan2 = "+plan2.toString());
	return std::pair<Plan,DensityGrid>(plan2,dg2);
}



Plan WeatherUtil::setAltitudes(const Plan& pp, double firstLegVs) {
	Plan p = Plan(pp);
	//f.pln(" $$ setAltitudes: startAlt = "+p.point(0).alt());
	//f.pln(" $$ setAltitudes: endAlt = "+p.point(p.size()-1).alt());
	if (p.size() < 2) return p;
	bool altPreserve = false;
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

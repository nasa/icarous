/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Wx.h"
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
	 Plan Wx::reRoute(const Plan& own, const PolyPath& pp) {		  // ONLY USED FOR EXAMPLE 1 of PAPER
		//fpln(" Wx::reRoute: HERE I AM 0");
		double  cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		double  gridExtension = cellSize * 3.0;       // Stratway = 3*cellSize
		double  factor = 1E8;                         // Stratway = 0.0
		double  T_p = MAXDOUBLE;
		std::vector<PolyPath> containment;
		double  reRouteLeadIn = 45.0;	
//		bool fastPolygonReroute = false;     // Stratway = true
		int fastPolygonReroute = 0;     // Stratway = true
		double timeOfCurrentPosition = -1;
		bool expandPolygons = false;          // should be tied to fastPolygonReroute
		double timeBefore = 0;  
		double timeAfter = 0;
		bool reRouteReduction = true;        // Stratway false
		std::vector<PolyPath> paths; // = std::vector<PolyPath>();
		paths.push_back(pp);
		return WeatherUtil::reRouteWxExpand(own, paths,  cellSize,  gridExtension, factor,  T_p, containment,  fastPolygonReroute,  
				timeOfCurrentPosition,  reRouteLeadIn,  expandPolygons,  timeBefore,  timeAfter,
				reRouteReduction).first;
	}

	 Plan Wx::reRoute(const Plan& own, const std::vector<PolyPath>& paths, double cellSize,
			      bool adhere, bool solutionSmoothing) {		
		double  gridExtension = cellSize * 3.0;       // Stratway = 3*cellSize
		double  T_p = MAXDOUBLE;
		std::vector<PolyPath> containment;
		double  reRouteLeadIn = 0.0;	
//		bool fastPolygonReroute = false;           // Stratway = true
		int fastPolygonReroute = 0;           // Stratway = true
		double timeOfCurrentPosition = -1;
		bool expandPolygons = false;               // should be tied to fastPolygonReroute
		double timeBefore = 0;  
		double timeAfter = 0;
		double factor = 0.0;
		if (adhere) factor = 1E8;
		return WeatherUtil::reRouteWxExpand(own, paths,  cellSize,  gridExtension, factor,  T_p, containment,  fastPolygonReroute,
				 timeOfCurrentPosition,  reRouteLeadIn,  expandPolygons,  timeBefore,  timeAfter,
				 solutionSmoothing).first;
	}

	
	 Plan Wx::reRoute(const Plan& own, const std::vector<PolyPath>& paths, bool adhere, bool solutionSmoothing) {
		double  cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		return Wx::reRoute(own, paths, cellSize, adhere, solutionSmoothing);
	}
	
	 Plan Wx::reRoute(const Plan& own, const std::vector<PolyPath>& paths, bool adhere,
			      bool solutionSmoothing, double timeOfCurrentPosition, double reRouteLeadIn) {		
		double  cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		double  gridExtension = cellSize * 3.0;       // Stratway = 3*cellSize
		double  T_p = MAXDOUBLE;
		std::vector<PolyPath> containment;
//		bool fastPolygonReroute = false;           // Stratway = true
		int fastPolygonReroute = 0;           // Stratway = true
		bool expandPolygons = false;                // should be tied to fastPolygonReroute
		double timeBefore = 0;  
		double timeAfter = 0;
		double factor = 0.0;
		if (adhere) factor = 100000000;
		return WeatherUtil::reRouteWxExpand(own, paths,  cellSize,  gridExtension, factor,  T_p, containment,  fastPolygonReroute,
				 timeOfCurrentPosition,  reRouteLeadIn,  expandPolygons,  timeBefore,  timeAfter,
				 solutionSmoothing).first;
	}
	
	
	 Plan Wx::reRoute(const Plan& own, const std::vector<PolyPath>& paths, bool adhere) {
		 bool solutionSmoothing = false;
         return reRoute(own, paths, adhere, solutionSmoothing);
	}

	/**
	 * Produce a plan that travels between two end points and is approximately conflict free of any polygons.
	 *
	 * (Note this code is experimental and may not always produce correct results.)
	 * 
	 * Note: 
	 * - "Approximately conflict free" means that there should be no intrusions that are greater than cellSize.  This 
	 * is intended to be a first step in a more thorough algorithm to avoid polygons.
	 * - This methods can produce solutions with a small intrusion into one or more polygons (less than 1 cellSize).
	 * These usually happen at acute corners of polygons.  Reducing the grid size will mitigate this (to the reduced grid size).
	 * - If reduceGridPath is true, this is more likely.
	 * - FastPolygonReroute uses an overestimation of polygons.
	 * 
	 * @param startPos
	 * @param startTime
	 * @param endPos
	 * @param gs
	 * @param paths a list of polypaths (i.e. a collection of polygons) to be routed around
	 * @param cellSize approximate size of grid square (in meters)
	 * @param buffer approximate size of extended grid area (in meters)
	 * @param factor weighting for closeness to path (1.0 = approximate distance in grid squares)
	 * @param T_p lookahead time, relative to timeOfCurrentPosition::  Conflicts beyond this time will be ignored, which can cause unexpected behavior.  It is recommended that this be turned off (set to -1) for most cases.
     * @param containment   
	 * @param fastPolygonReroute,
	 * @param reduceGridPath,		perform a low-level smoothing operation on the resulting path.  this may result in a solution that is not completely conflict free.
	 * @param timeOfCurrentPosition  location of aircraft in its plan (if -1 then, the first point of plan is used)
	 * @param reRouteLeadIn          leadin time from current position (wait this amount of time before maneuvar)
	 *
	 * @return new ownship plan (empty and/or error set on fail) and a Densitygrid used (for visualization)
	 * Note that even a successful return does not guarantee a completely conflict-free return path, only one that does not intrude into any polygons by more than cellSize.
	 */
	 std::pair<Plan,DensityGrid> Wx::reRoute(Position startPos, double startTime,
			Position endPos, double gs,
			const std::vector<PolyPath>& paths, double cellSize, double buffer,
			double factor, double T_p, std::vector<PolyPath> containment,
			double reRouteLeadIn) {				
		//fpln("WeatherUtil::reRouteWx own="+own+" paths="+paths+" cellSize="+cellSize+" buffer="+buffer+" factor="+factor+" Tp="+T_p+" containment="+containment.size()+" fast="+fastPolygonReroute+" red="+reduceGridPath+" timeOfCurrentPosition="+timeOfCurrentPosition+" reRouteLeadIn="+reRouteLeadIn);
		Plan solution;
		DensityGrid dg = DensityGrid(); // this used for visualization only
		if (gs <= 0) {
			fpln(" specified ground speed must be > 0!");
			//solution = null;
		} else {
			Plan own("temp");
			NavPoint startNP(startPos,startTime);
			double dist = startPos.distanceH(endPos);
			double endTime = startTime + dist/gs;
			//fpln(" $$$$ reRouteWeather: endTime = "+endTime);
			NavPoint endNP(endPos,endTime);
			own.addNavPoint(startNP);
			own.addNavPoint(endNP);
			//fpln(" $$$$ reRouteWeather: own = "+own);
			Velocity currentVel = own.velocity(startTime);
			double startTimeLead = startTime + reRouteLeadIn;
			double endTimeLead = endTime - reRouteLeadIn;
			//solution = null;
			// cut down working plan to startTimeLead
			if (paths.size() == 0) {
				solution = own;
			} else if (own.isTimeInPlan(startTimeLead) && own.isTimeInPlan(endTimeLead)) {
				Plan nPlan = PlanUtil::cutDownLinear(own, startTimeLead, endTimeLead);
				//fpln(" $$$$ reRouteWeather: nPlan = "+nPlan);
				double endT = startTime + T_p;
//				bool fastPolygonReroute = false;
				int fastPolygonReroute = 0;  //TODO EXPERIMENTAL
				std::pair<Plan,DensityGrid> rr = WeatherUtil::reRouteWithAstar(paths, nPlan, cellSize, buffer, factor, gs, 
						containment, endT, fastPolygonReroute);
				Plan rrPlan = rr.first; 	
				dg = rr.second;
				//fpln(" $$$$ reRouteWx: rr.first = "+rr.first);
				//fpln(" $$$$ reRouteWx: rr.second = "+rr.second);
				if (rrPlan.size() > 0) {
					rrPlan = WeatherUtil::setAltitudes(rrPlan,currentVel.vs());
					if (rrPlan.size() > 0) {
						NavPoint currentPos = own.point(0);
						NavPoint finalPos = own.point(own.size()-1).makeTime(MAXDOUBLE); 
						rrPlan.addNavPoint(currentPos);
						rrPlan.addNavPoint(finalPos);
						rrPlan.setTimeGsIn(rrPlan.size()-1, nPlan.initialVelocity(0).gs());
						//fpln(" $$$$ reRoute: rrPlan = "+rrPlan+" "+localParams.unZigReroute);
						solution = rrPlan;
					} else {
						own.addError("reRouteWx failed to find solution"); // +speedString+".");				
					}
				} else {
					//fpln(" $$ reRouteWx did not find a solution"); // +speedString+"!!");
				}
				//fpln(" reRouteWx: rrPlan = "+rrPlan);			
			} else {
				own.addError("reRouteWx failed to find solution (insuffcient plan length).");
			}			
			if (solution.size() == 0) {
				PlanUtil::isCurrentPositionUnchanged(solution, startTime, own.point(0).position());
			}
		}
		return std::pair<Plan,DensityGrid>(solution,dg);
	}// reRouteWx


	

	
}

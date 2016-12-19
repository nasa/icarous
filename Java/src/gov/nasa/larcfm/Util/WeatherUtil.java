/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;


public class WeatherUtil {

	/**
	 * Produce a plan that travels between two end points and is approximately conflict free of any polygons.
	 * 
	 * Note: 
	 * - "Approximately conflict free" means that there should be no intrusions that are greater than gridSize.  This is intended to be a first step
	 * in a more thorough algorithm to avoid polygons.
	 * - This methods can produce solutions with a small intrusion into one or more polygons (less than 1 gridSize).
	 * These usually happen at acute corners of polygons.  Reducing the grid size will mitigate this (to the reduced grid size).
	 * - If reduceGridPath is true, this is more likely.
	 * - FastPolygonReroute uses an overestimation of polygons.
	 * 
	 * @param own   plan for ownship
	 * @param paths polypath collection of polygons to be routed around
	 * @param gridSize approximate size of grid square (in meters)
	 * @param buffer approximate size of extended grid area (in meters)
	 * @param factor weighting for closeness to path (1.0 = approximate distance in grid squares)
	 * @param T_p lookahead time, relative to timeOfCurrentPosition.  Conflicts beyond this time will be ignored, which can cause unexpected behavior.  It is recommended that this be turned off (set to -1) for most cases.
	 * @param fastPolygonReroute,
	 * @param reduceGridPath,		perform a low-level smoothing operation on the resulting path.  this may result in a solution that is not completely conflict free.
	 * @param timeOfCurrentPosition  location of aircraft in its plan
	 * @param reRouteLeadIn          leadin time from current position (wait this amount of time before maneuvar)
	 *
	 * @return new ownship plan (empty and/or error set on fail) and densitygrid used (for visualization)
	 * Note that even a successful return does not guarantee a completely conflict-free return path, only one that does not intrude into any polygons by more than gridSize.
	 */
	static public Pair<Plan,DensityGrid> reRouteWx(Plan own, ArrayList<PolyPath> paths, double gridSize, double buffer, 
			double factor, double T_p, ArrayList<PolyPath> containment,	boolean fastPolygonReroute, boolean reduceGridPath, 
			double timeOfCurrentPosition, double reRouteLeadIn) {				
//f.pln("WeatherUtil.reRouteWx 1");
//f.pln("WeatherUtil.reRouteWx own="+own+" paths="+paths+" gridsize="+gridSize+" buffer="+buffer+" factor="+factor+" Tp="+T_p+" containment="+containment.size()+" fast="+fastPolygonReroute+" red="+reduceGridPath+" timeOfCurrentPosition="+timeOfCurrentPosition+" reRouteLeadIn="+reRouteLeadIn);
		//Plan p = plans.getPlan(0);	
		Plan solution;
		if (own.size() < 2) {
			//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$ reRoute: FAIL !! reRoute needs at least 2 points !!");
			own.addError("reRouteWx failed (plan too short).");
			return Pair.make(own,null);
		}	
		double tmCurrentPos = timeOfCurrentPosition;
		NavPoint currentPos = own.point(0);
		NavPoint finalPos = own.point(own.size()-1).makeTime(Double.MAX_VALUE); // ensure it is at end of plan, will correct later
		if (tmCurrentPos < 0) {
			tmCurrentPos = own.getFirstTime();
		}
		if (tmCurrentPos < own.getFirstTime() || tmCurrentPos > own.getLastTime()) {
			own.addError("reRouteWx: specified time of current position ("+tmCurrentPos+") is not within plan");
			return Pair.make(own,null);
		}
		int currentIdx = own.getIndex(tmCurrentPos);
		if (currentIdx >= 0) {
			currentPos = own.point(currentIdx);
		} else {
			currentPos = new NavPoint(own.position(tmCurrentPos), tmCurrentPos);
		}
		//f.pln(" $$$$ reRouteWx: tmCurrentPos = "+tmCurrentPos+" currentPos = "+currentPos);
		Velocity currentVel = own.velocity(tmCurrentPos);
		Plan nPlan = own;
		double startTime = tmCurrentPos + reRouteLeadIn;
		double endTime = own.getLastTime() - reRouteLeadIn;
		Position endLeadinPos = own.position(endTime);
		//f.pln(" $$ reRouteWx: tmCurrentPos = "+tmCurrentPos+"  startTime = "+startTime+" endTime = "+endTime);
		solution = null;
		DensityGrid dg = new DensityGrid(); // this used for visualization only
		// cut down working plan to startTime
		if (paths.size() == 0) {
			solution = own;
		} else if (own.timeInPlan(startTime) && own.timeInPlan(endTime)) {
			nPlan = PlanUtil.cutDown(own, startTime, endTime);
			double gs = nPlan.initialVelocity(0).gs();
			if (gs <= 0) {
				f.pln("reRouteWx: ERROR Warning!!! initial groundspeed is zero!");
			}
			double endT = -1.0;
			if (timeOfCurrentPosition > 0 && T_p > 0) {
				endT = timeOfCurrentPosition + T_p;
			}
			Pair<Plan,DensityGrid> rr = reRouteWithAstar(paths, nPlan, gridSize, buffer, factor, gs, containment, endT,
					fastPolygonReroute, reduceGridPath);
			Plan rrPlan = rr.first; 	
			dg = rr.second;
//f.pln(" $$$$ reRouteWx: rr.first = "+rr.first);
//f.pln(" $$$$ reRouteWx: rr.second = "+rr.second);
			if (rrPlan.size() > 0) {
				rrPlan = setAltitudes(rrPlan,currentVel.vs());
				if (rrPlan.size() > 0) {
					rrPlan.add(currentPos);
					rrPlan.add(finalPos);
					rrPlan.setTimeGSin(rrPlan.size()-1, nPlan.initialVelocity(0).gs());
					//f.pln(" $$$$ reRoute: rrPlan = "+rrPlan+" "+localParams.unZigReroute);
					solution = rrPlan;
				} else {
					own.addError("reRouteWx failed to find solution"); // +speedString+".");				
					solution = null;
				}
			} else {
				//f.pln(" $$ reRouteWx did not find a solution"); // +speedString+"!!");
				solution = null;
			}
			//			} // for
//f.pln(" reRouteWx: rrPlan = "+rrPlan);			
		} else {
			//f.pln(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$ reRoute: FAIL !! $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
			own.addError("reRouteWx failed to find solution (insuffcient plan length).");
			solution = null;
		}			
		if (solution != null) {
			PlanUtil.checkMySolution(solution, tmCurrentPos, currentPos.position(), currentVel);
		}
		return Pair.make(solution,dg);
	}// reRouteWx

	/**
	 * As reRouteWx(), but this will internally expand the polygons so that they cover a larger area, either representing their total coverage over 
	 * a period of time of duration timeBefore+timeAfter, and/or over-approximating them to mitigate solutions that cut corners of polygons.  
	 * Note that setting reduceGridPath to true may still result in a solutions that still contains conflicts. 
	 * 
	 * @param own ownship plan
	 * @param paths list of polygons to avoid
	 * @param gridSize desired grid size (m)
	 * @param buffer buffer to expand the grid to allow go-around solutions (m)
	 * @param factor weight indicating desire to retain original path (0 = abandon original path, higher numbers = favor original path; transition point in the 3-10 range)
	 * @param T_p lookahead time for search, relative to timeOfCurrentPosition.  Conflicts beyond this time will be ignored, which can cause unexpected behavior.  It is recommended that this be turned off (set to -1) in most cases.
	 * @param containment list of polygons to remain within
	 * @param fastPolygonReroute if set to true, use a less exact (greater over-approximation) but much faster search
	 * @param reduceGridPath if set to true, attempt to smooth resulting path (may re-introduce minor conflicts)
	 * @param timeOfCurrentPosition absolute time ownship start
	 * @param reRouteLeadIn time (in s) to allow for initial and final velocity matching of aircraft (should probably be enough to make a 180 degree turn) 
	 * @param expandPolygons if set to true, internally over-approximate the polygons to mitigate missed conflicts
	 * @param timeBefore if greater than zero, ensure the final path is continuously clear for this amount of time before the given solution 
	 * @param timeAfter if greater than zero, ensure the final path is continuously clear for this amount of time after the given solution
	 * @param reRouteReduction if true, perform more aggressive simplification of resulting path.
	 * @return Solution path and the density grid used for the search (the later is intended for further display or analysis)
	 * If no solution is found, return a null or empty path.
	 * 
	 * Note that even a successful return does not guarantee a completely conflict-free return path, only one that does not intrude into any polygons by more 
	 * than gridSize.  Setting expandPolygons to true should mitigate this, and setting reduceGridPath may re-introduce intrusions.
	 */
	static public Pair<Plan,DensityGrid> reRouteWx(Plan own, ArrayList<PolyPath> paths, double gridSize, double buffer, 
			double factor, double T_p, ArrayList<PolyPath> containment, boolean fastPolygonReroute, boolean reduceGridPath,
			double timeOfCurrentPosition, double reRouteLeadIn, boolean expandPolygons, double timeBefore, double timeAfter,
			boolean reRouteReduction) {
		ArrayList<PolyPath> npaths = new ArrayList<PolyPath>();
		for (int i = 0; i < paths.size(); i++) {
			PolyPath exp = PolyUtil.stretchOverTime(paths.get(i), timeBefore, timeAfter);
			if (expandPolygons) {
				// expand by 1/2 diagonal of grid size to ensure no missed sections.
				exp = PolyUtil.bufferedConvexHull(exp, gridSize*0.71, 0.0);
			}
			npaths.add(exp);
		}
		Pair<Plan,DensityGrid> pr = reRouteWx(own, npaths, gridSize, buffer, factor, T_p, containment, fastPolygonReroute, reduceGridPath, timeOfCurrentPosition, reRouteLeadIn);
		Plan ret = pr.first;
//f.pln("reRouteWx ret1="+ret.toString());		
		if (reRouteReduction && ret != null) {
			double incr = 10.0;
			double gs = own.initialVelocity(0).gs();
			Plan ret2 = PolyUtil.reducePlanAgainstPolys(ret, gs, npaths, incr);
			if (ret2 != null) {
				ret = ret2;
			}
            //f.pln("reduced plan="+ret);		
		}
//f.pln("reRouteWx ret2="+ret.toString());		
		return Pair.make(ret, pr.second);
	}

	
	static private Pair<Plan,DensityGrid> reRouteWithAstar(ArrayList<PolyPath> paths, Plan ownship, double gridSize, double buffer, 
			double factor, double gs, ArrayList<PolyPath> containment, double endT,
			boolean fastPolygonReroute, boolean reduceGridPath) {
//f.pln("WeatherUtil.reRouteWithAstar");
		boolean markPath = true; // mark all cells in the grid that are in the original optimal path
		int b = (int)Math.ceil(buffer/gridSize);
		DensityGridTimed dg;
		if (fastPolygonReroute) {
			dg = new DensityGridMovingPolysEst(ownship, b, gridSize, gs, paths, containment);
		} else {
			dg = new DensityGridMovingPolys(ownship, b, gridSize, gs, paths, containment);
		}
		dg.setLookaheadEndTime(endT);
		dg.snapToStart();
		// only set the search time range if it makes sense
		//		dg.resetWeights(1.0);
		List<Pair<Integer,Integer>> origpath = dg.gridPath(ownship);
		boolean noContainment = true;
		if (containment != null && containment.size() > 0) {
			for (int i = 0; i < containment.size(); i++) {
				if (containment.get(i).isStatic()) {
					dg.setWeightsInside(containment.get(i).getPoly(0), 1.0);
					noContainment = false;
				}
			}
		}
		if (noContainment) {
			dg.setWeights(1.0);
		}
		if (factor > 0.0) {
			dg.setProximityWeights(origpath, factor, false);
			//	dg.setProximityWeights(ownship, factor, false);
		} 
		//dg.setProximityWeights(ownship, factor);
		DensityGridAStarSearch dgs = new DensityGridAStarSearch();
		List<Pair<Integer,Integer>> gPath = dgs.optimalPathT(dg);
		//dg.printSearchedWeights();		
		if (gPath == null) {
			//f.pln(" $$$$$ reRouteWithAstar: optimal path not found !!!");
			return Pair.make(new Plan(),(DensityGrid)dg);
		}
		if (markPath) {
			for (int i = 0; i < gPath.size(); i++) {
				dg.setMark(gPath.get(i),true);
			}
		}
//f.pln("reRouteWithAstar  ---------------- optimal path --------- "+gPath.size());
//dg.printGridPath(gPath);
		Plan plan2 = dg.gridPathToPlan(gPath,gs,0.0,reduceGridPath);
		if (plan2 == null) {
			//f.pln(" $$$$$$$ reRouteWithAstar: FAIL !!!! plan2 = null");
			return Pair.make(new Plan(),(DensityGrid)dg);
		}
//f.pln("reRouteWithAstar plan2:"+plan2);		
		//f.pln(" $$$$$$$ reRouteWithAstar: plan2 = "+plan2);
		return Pair.make(plan2,(DensityGrid)dg);
	}


	
    static public Plan setAltitudes(Plan pp, double firstLegVs) {
    	Plan p = pp.copy();
    	//f.pln(" $$ setAltitudes: startAlt = "+p.point(0).alt());
    	//f.pln(" $$ setAltitudes: endAlt = "+p.point(p.size()-1).alt());
    	if (p.size() < 2) return p;
    	boolean altPreserve = true;
    	p.setAltVSin(1, firstLegVs, altPreserve);
    	if (p.size() < 3) return p;
    	double vs = (p.point(p.size()-1).alt() - p.point(1).alt())/(p.point(p.size()-1).time() - p.point(1).time());
    	//f.pln(" $$ setAltitudes: vs = "+Units.str("fpm",vs));
    	for (int j = 2; j < p.size()-1; j++) {
    	    p.setAltVSin(j, vs, altPreserve);
    	}
    	return p;
    }


//	static public List<Pair<Integer,Integer>> optimalPath(DensityGrid dg, DensityGridSearch dgs) {
//		return dgs.search(dg, dg.startPoint, dg.endPoint);
//	}

	
}

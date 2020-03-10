/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


// Uses the Google unit test framework.

#include "WeatherUtil.h"
#include "Plan.h"
#include "Position.h"
#include "SimplePoly.h"
#include "DensityGrid.h"
#include "PolyPath.h"
#include "Wx.h"
#include "NavPoint.h"
#include "PlanUtil.h"
#include "PolyUtil.h"
#include "Units.h"
#include "DensityGridAStarSearch.h"
#include <gtest/gtest.h>

#include <vector>
#include <cmath>
#include <float.h>

#include "DebugSupport.h"

using namespace larcfm;

class WeatherUtilTest : public ::testing::Test {

public:

protected:
  virtual void SetUp() {
  }
};


	
	TEST_F(WeatherUtilTest, test_reRouteWx) {
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		//fpln(" ----------------------------- test_reRouteWx --------------------------");
		Plan own;
		Position p0  = Position::makeLatLonAlt(-1.240071, -0.141961, 5000.0);
		Position p1  = Position::makeLatLonAlt(-1.290175, 5.440447, 5000.0);
		NavPoint np0(p0,1000.0);        	 own.addNavPoint(np0);
		NavPoint np1(p1,3991.956034);    	 own.addNavPoint(np1);
		PolyPath pp = PolyPath("pp");
		SimplePoly sPoly0 = SimplePoly(0.0,3048.0);
		//fpln(" $$ test_reRouteWx 1");
		Position pp0  = Position::makeLatLonAlt(-0.870000, 1.720000, 0); sPoly0.add(pp0);
		Position pp1  = Position::makeLatLonAlt(-1.730000, 1.720000, 0); sPoly0.add(pp1);
		Position pp2  = Position::makeLatLonAlt(-1.730000, 3.340000, 0); sPoly0.add(pp2);
		Position pp3  = Position::makeLatLonAlt(-0.810000, 3.320000, 0); sPoly0.add(pp3);
		Velocity v0  = Velocity::makeTrkGsVs(30.00, 100.00, 0.00);
		pp.addPolygon(sPoly0,v0,1000.0);
		pp.setPathMode(PolyPath::USER_VEL_EVER);
		std::vector<PolyPath> paths = std::vector<PolyPath>();
		paths.push_back(pp);
		//fpln(" $$ test_reRouteWx 2");
		//DebugSupport.dumpPlanAndPolyPaths(own,paths,"test_reRouteWx_pp");
		double cellSize = Units::from("nmi", 5.0);
		double buffer = cellSize * 3.5;
		double factor = 0.0;
		double T_p = MAXDOUBLE;
		std::vector<PolyPath> containment = std::vector<PolyPath>();
//		bool fastPolygonReroute = false;
		int fastPolygonReroute = 0;  //TODO EXPERIMENTAL
		bool expandPolys = false;
		double timeOfCurrentPosition = 1010.0;
		double reRouteLeadIn = 45.0;
		bool solutionSmoothing = false;
		//DebugSupport.dumpPlan(rrw.first,"test_reRouteWx_sln");
		double tmConflict = PolyUtil::isPlanInConflictWx(own, paths, 1.0).first;
		EXPECT_NEAR(2140,tmConflict,0.00001);
		double timeBefore = 400;
		double timeAfter  = 500;
		//fpln(" $$ test_reRouteWx 3C: paths = "+paths[0].toString());
		std::pair<Plan,DensityGrid> rrPair = WeatherUtil::reRouteWxExpand(own, paths, cellSize, buffer, factor, T_p, containment,
				fastPolygonReroute, timeOfCurrentPosition, reRouteLeadIn, expandPolys, timeBefore, timeAfter,solutionSmoothing);
		Plan ret = rrPair.first;
		if (ret.hasError()) {
			std::string errmsg = ret.getMessage();
		}
		Plan rrShift = Plan(ret);
		rrShift.timeShiftPlan(0,-300);
		//DebugSupport.dumpPlan(rrShift,"reRouteWx_ret_Shift");
		//fpln(" $$$t est_reRouteWx rrPair.first = "+rrPair.first.toString());
		//fpln("$$$test_reRouteWx------------------------------------------------------------------");
		for (double t = rrShift.time(0); t < 2000; t = t + 100.0) {
			SimplePoly ppAt_t = pp.position(t);
			//fpln(" $$$ test_reRouteWx: t = "+t+" ppAt_t = "+ppAt_t);
			Position acAt_t = rrShift.position(t);
			EXPECT_FALSE(ppAt_t.contains(acAt_t));
		}
		rrShift = Plan(rrPair.first);
		rrShift.timeShiftPlan(0,-600);
		SimplePoly ppAt_t = pp.position(1520);
		Position acAt_t = rrShift.position(1520);
		EXPECT_TRUE(ppAt_t.contains(acAt_t));

		//DebugSupport.dumpAsUnitTest(ret);
		Plan retChk("noname");
		Position rp0  = Position::makeLatLonAlt(-1.24025834, -0.12330332, 5000.0);
		Position rp1  = Position::makeLatLonAlt(-1.24109976, -0.03934371, 5000.0);
		Position rp2  = Position::makeLatLonAlt(-1.24109976, 1.71065629, 5000.0);
		Position rp3  = Position::makeLatLonAlt(-1.49109982, 1.96065642, 5000.0);
		Position rp4  = Position::makeLatLonAlt(-1.49109982, 5.12732309, 5000.0);
		Position rp5  = Position::makeLatLonAlt(-1.32443311, 5.29398967, 5000.0);
		Position rp6  = Position::makeLatLonAlt(-1.28951099, 5.35648421, 5000.0);
		Position rp7  = Position::makeLatLonAlt(-1.29017500, 5.44044700, 5000.0);
		NavPoint rnp0(rp0,1010.0);    	 retChk.addNavPoint(rnp0);
		NavPoint rnp1(rp1,1055.000000);    	 retChk.addNavPoint(rnp1);
		NavPoint rnp2(rp2,1992.903709);    	 retChk.addNavPoint(rnp2);
		NavPoint rnp3(rp3,2182.406406);    	 retChk.addNavPoint(rnp3);
		NavPoint rnp4(rp4,3879.388798);    	 retChk.addNavPoint(rnp4);
		NavPoint rnp5(rp5,4005.722779);    	 retChk.addNavPoint(rnp5);
		NavPoint rnp6(rp6,4044.092454);    	 retChk.addNavPoint(rnp6);
		NavPoint rnp7(rp7,4089.092454);    	 retChk.addNavPoint(rnp7);

		for (int j = 0; j < ret.size(); j++) {
			EXPECT_EQ(retChk.point(j), retChk.point(j));
		}
	}



	TEST_F(WeatherUtilTest, test_reRouteWx2) {
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		//fpln(" ----------------------------- test_reRouteWx2 --------------------------");
		Plan lpc("calculateDynamicReRoute");
		Position pp0  = Position::makeLatLonAlt(41.51693111, -81.16318417, 0.0);
		Position pp1  = Position::makeLatLonAlt(41.44236031, -75.63489517, 0.0);
		NavPoint np0(pp0,16503.000000);
		np0 = np0.makeName("CXR");    	 lpc.addNavPoint(np0);
		NavPoint np1(pp1,18459.740225);
		np1 = np1.makeName("LVZ");    	 lpc.addNavPoint(np1);

		PolyPath pp = PolyPath("pp");
		SimplePoly sPoly0 = SimplePoly(0.0,18288.0);
		Position p0  = Position::makeLatLonAlt(42.28300000, -78.53300000, 0.0); sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(42.08300000, -78.53300000, 0.0); sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(41.55000000, -78.95000000, 0.0); sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(41.38300000, -79.08300000, 0.0); sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(41.20000000, -79.55000000, 0.0); sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(41.20000000, -79.75000000, 0.0); sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(41.40000000, -79.75000000, 0.0); sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(41.66600000, -79.58300000, 0.0); sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(42.06600000, -79.15000000, 0.0); sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(42.28300000, -78.73300000, 0.0); sPoly0.add(p9);
		Velocity v0  = Velocity::mkTrkGsVs(1.221730, 26.751111, 0.000000);
		pp.addPolygon(sPoly0,v0,14000.0);
		pp.setPathMode(PolyPath::USER_VEL);

		std::vector<PolyPath> paths = std::vector<PolyPath>();
		paths.push_back(pp);
		double tmConf = PolyUtil::isPlanInConflictWx(lpc, paths, 1.0).first;
		EXPECT_TRUE(tmConf > 0);
		//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"reRouteWx2_pp");
		bool solutionSmoothing = false;
		double timeOfCurrentPosition =  -1; // 775;
		double reRouteLeadIn = 45;
		double cellSize = Units::from("nmi", 10.0);
		double gridExtension = cellSize * 10.0;
		double T_p = MAXDOUBLE;
//		bool fastPolygonReroute = false;
		int fastPolygonReroute = 0;
//		bool expandPolygons = false;
//		double timeBefore = 0;
//		double timeAfter = 0;
		double adhereFactor = 1E8;
		std::vector<PolyPath> containment;
		std::pair<Plan, DensityGrid> rrPair = WeatherUtil::reRouteWx(lpc, paths,  cellSize,  gridExtension, adhereFactor,  T_p,
				                         containment,  fastPolygonReroute, timeOfCurrentPosition,  reRouteLeadIn,
				                         solutionSmoothing);
		Plan reRoute = rrPair.first;
		if (reRoute.size() == 0) {
			fpln(" ### reRouteWx2: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute9: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"reRouteWx2_sln");
		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		EXPECT_TRUE(tmConflict < 0);
	}

	
	

	
	TEST_F(WeatherUtilTest, test_Example1) {     // SEE PAPER Ex 1
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		//fpln(" ----------------------------- test_Example1 --------------------------");
		PolyPath pp = PolyPath("Example1");
		SimplePoly poly = SimplePoly(0.0,10668.0);
		Velocity v0  = Velocity::makeTrkGsVs(60.0,40,0.0); 
		Position p0  = Position::makeLatLonAlt(42.6576,  -99.9165, 0.0); poly.add(p0);
		Position p1  = Position::makeLatLonAlt(42.8313, -100.0093, 0.0); poly.add(p1);
		Position p2  = Position::makeLatLonAlt(42.8751, -100.3355, 0.0); poly.add(p2);
		Position p3  = Position::makeLatLonAlt(42.6603, -100.5250, 0.0); poly.add(p3);
		Position p4  = Position::makeLatLonAlt(42.5569, -100.6152, 0.0); poly.add(p4);
		Position p5  = Position::makeLatLonAlt(42.4323, -100.7186, 0.0); poly.add(p5);
		Position p6  = Position::makeLatLonAlt(42.3633, -100.5158, 0.0); poly.add(p6);
		Position p7  = Position::makeLatLonAlt(42.4840, -100.3209, 0.0); poly.add(p7);
		Position p8  = Position::makeLatLonAlt(42.5504, -100.1933, 0.0); poly.add(p8);
		Position p9  = Position::makeLatLonAlt(42.5594, -100.1558, 0.0); poly.add(p9);
		Position p10 = Position::makeLatLonAlt(42.5234, -100.1182, 0.0); poly.add(p10);
		Position p11 = Position::makeLatLonAlt(42.4110, -100.1366, 0.0); poly.add(p11);
		Position p12 = Position::makeLatLonAlt(42.3858,  -99.8051, 0.0); poly.add(p12);
		Position p13 = Position::makeLatLonAlt(42.2161,  -99.6818, 0.0); poly.add(p13);
		Position p14 = Position::makeLatLonAlt(42.5861,  -99.5293, 0.0); poly.add(p14);
		Position p15 = Position::makeLatLonAlt(42.5622,  -99.7561, 0.0); poly.add(p15);

		pp.addPolygon(poly,v0,600.0); 
		pp.setPathMode(PolyPath::USER_VEL);
		//fpln(" $$$$ pp = "+pp.toString());
		Plan own("Aircraft_0");
		Position pA  = Position::makeLatLonAlt(42.89261, -101.01528, 5000.00);
		Position pB  = Position::makeLatLonAlt(42.90706,  -98.85868, 5000.00);
		double startTime = 600;
		NavPoint npA(pA,startTime);   own.addNavPoint(npA);
		NavPoint npB(pB,2000);    	 own.addNavPoint(npB);
		Velocity currentVel = own.velocity(startTime);

		//DebugSupport.dumpPlanAndPolyPath(own,pp,"Example1_pp");
		Plan reRoute = Wx::reRoute(own,pp);
		if (reRoute.size() == 0) {
			fpln(" ### test_Example1: REROUTE FAILED!");
		} else {
			//fpln(" $$$ test_Example1: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"Example1_reRoute");
		}
		bool ok = PlanUtil::checkMySolution(reRoute, startTime, own.point(0).position(), currentVel);
		EXPECT_TRUE(ok);
		EXPECT_EQ(5,reRoute.size());
		//EXPECT_EQ(Position::makeLatLonAlt(42.88778509, -100.37390000, 5000.0),reRoute.point(0).position());
		//        EXPECT_TRUE(Position::makeLatLonAlt(42.88849715, -100.34348807, 5000.0).distanceH(reRoute.point(1).position()) < .001);
		//        EXPECT_TRUE(Position::makeLatLonAlt(43.05516384, -100.17682239, 5000.0).distanceH(reRoute.point(2).position()) < .001);
		//        EXPECT_TRUE(Position::makeLatLonAlt(42.90569670,  -99.45788948, 5000.0).distanceH(reRoute.point(3).position()) < .001);
		//        EXPECT_EQ(Position::makeLatLonAlt(42.90616624,  -99.42746000, 5000.0),reRoute.point(4).position());
		EXPECT_FALSE(reRoute.size() == 0);

		std::vector<PolyPath> paths = std::vector<PolyPath>(1);
		paths.push_back(pp);
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 10.0).first;
		EXPECT_TRUE(tmConflict < 0);
		EXPECT_EQ(5,reRoute.size());
		// do not change start and end point !
		EXPECT_NEAR(reRoute.point(0).time(),600.0000,0.001);
		EXPECT_NEAR(reRoute.point(0).lat(),0.7485,0.001);
		EXPECT_NEAR(reRoute.point(0).lon(),-1.763,0.001);
		EXPECT_NEAR(reRoute.point(0).alt(),1524.0000,0.001);
		EXPECT_NEAR(reRoute.point(4).time(),2031.0994,0.001);
		EXPECT_NEAR(reRoute.point(4).lat(),0.7489,0.001);
		EXPECT_NEAR(reRoute.point(4).lon(),-1.7254,0.001);
		EXPECT_NEAR(reRoute.point(4).alt(),1524.0000,0.001);

		Plan lpc("IntermediateUnReduced");
		p0  = Position::makeLatLonAlt(42.88778509, -100.37390000, 5000.0);
		p1  = Position::makeLatLonAlt(42.88849715, -100.34348807, 5000.0);
		p2  = Position::makeLatLonAlt(42.88849715, -100.26015470, 5000.0);
		p3  = Position::makeLatLonAlt(43.05516382, -100.09348819, 5000.0);
		p4  = Position::makeLatLonAlt(43.05516382, -99.67682153, 5000.0);
		p5  = Position::makeLatLonAlt(42.97188040, -99.59339488, 5000.0);
		p6  = Position::makeLatLonAlt(42.90569670, -99.45788948, 5000.0);
		p7  = Position::makeLatLonAlt(42.90616624, -99.42746000, 5000.0);
		NavPoint np0(p0,600.0);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,645.000000);    	 lpc.addNavPoint(np1);
		NavPoint np2(p2,768.243304);    	 lpc.addNavPoint(np2);
		NavPoint np3(p3,1185.098327);    	 lpc.addNavPoint(np3);
		NavPoint np4(p4,1799.646309);    	 lpc.addNavPoint(np4);
		NavPoint np5(p5,2008.024727);    	 lpc.addNavPoint(np5);
		NavPoint np6(p6,2457.213356);    	 lpc.addNavPoint(np6);
		NavPoint np7(p7,2502.213356);    	 lpc.addNavPoint(np7);

		double gs = own.gsOut(0);
		double incr = 10.0;
		//DebugSupport.dumpPlan(lpc,"Example1_interm");
		bool leadInsPresent = true;
		Plan reduced = PolyUtil::reducePlanAgainstPolys(lpc, gs, paths, incr, leadInsPresent);
		//DebugSupport.dumpPlan(reduced,"Example1_reduced");
		EXPECT_EQ(4,reduced.size());
	}


	
	TEST_F(WeatherUtilTest, test_Example_2) {  
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		//fpln(" ----------------------------- test_Example_2 --------------------------");
		PolyPath pp = PolyPath("reRoute2_A");
		SimplePoly poly1 = SimplePoly(0.0,12000.0);
		Position p0  = Position::makeLatLonAlt(43.47086750, -99.93538019, 0.0);     poly1.add(p0);
		Position p1  = Position::makeLatLonAlt(43.18252517, -100.94818263, 0.0);     poly1.add(p1);
		Position p2  = Position::makeLatLonAlt(43.02033260, -100.51206486, 0.0);     poly1.add(p2);
		Position p3  = Position::makeLatLonAlt(43.06178181, -100.24354606, 0.0);     poly1.add(p3);
		Position p4  = Position::makeLatLonAlt(43.13406686, -100.07088861, 0.0);     poly1.add(p4);
		Position p5  = Position::makeLatLonAlt(43.11404386, -99.92276522, 0.0);     poly1.add(p5);
		Position p6  = Position::makeLatLonAlt(43.03114544, -99.88311815, 0.0);     poly1.add(p6);
		Position p7  = Position::makeLatLonAlt(42.88156786, -99.81824112, 0.0);     poly1.add(p7);
		Position p8  = Position::makeLatLonAlt(42.95725772, -99.10279171, 0.0);     poly1.add(p8);
		Position p9  = Position::makeLatLonAlt(43.08520963, -99.52269023, 0.0);     poly1.add(p9);
		Velocity v1  = Velocity::makeTrkGsVs(298.0,15,0.0); 		
		pp.addPolygon(poly1,v1,600.0); 
		pp.setPathMode(PolyPath::USER_VEL);
		//fpln(pp.toString());		
		PolyPath pp2 = PolyPath("reRoute2-B");
		SimplePoly poly2 = SimplePoly(0.0,12000.0);
		p0  = Position::makeLatLonAlt(42.96222522, -100.98959486, 0.0);     poly2.add(p0);
		p1  = Position::makeLatLonAlt(42.88143990, -101.33742056, 0.0);     poly2.add(p1);
		p2  = Position::makeLatLonAlt(42.67723255, -101.48777102, 0.0);     poly2.add(p2);
		p3  = Position::makeLatLonAlt(42.52912613, -101.75929947, 0.0);     poly2.add(p3);
		p4  = Position::makeLatLonAlt(42.16783621, -101.72339488, 0.0);     poly2.add(p4);
		p5  = Position::makeLatLonAlt(42.12968758, -101.36210496, 0.0);     poly2.add(p5);
		p6  = Position::makeLatLonAlt(42.07807474, -101.01427926, 0.0);     poly2.add(p6);
		p7  = Position::makeLatLonAlt(42.32043071, -101.05242789, 0.0);     poly2.add(p7);
		p8  = Position::makeLatLonAlt(42.37653163, -100.74275081, 0.0);     poly2.add(p8);
		p9  = Position::makeLatLonAlt(42.76699402, -100.43307374, 0.0);     poly2.add(p9);
		Velocity v2  = Velocity::makeTrkGsVs(75.0,30,0.0); 		
		pp2.addPolygon(poly2,v2,600.0); 
		pp2.setPathMode(PolyPath::USER_VEL);

		std::vector<PolyPath> paths = std::vector<PolyPath>(1);
		paths.push_back(pp);
		paths.push_back(pp2);
		//fpln(" $$$ test_Wx_reRoute2: paths = "+paths);

		Plan lpc("Aircraft_0");
		p0  = Position::makeLatLonAlt(43.05184519, -101.78726328, 5000.0);
		p1  = Position::makeLatLonAlt(42.57797542, -98.59870772, 5000.0);
		NavPoint np0(p0,600.0);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,2000.0);    	 lpc.addNavPoint(np1);

		//DebugSupport.dumpPlan(lpc,"test_Wx_reRoute2_plan");
		//DebugSupport.dumpPlanAndPolyPaths(lpc,paths,"reRoute2_pp");

		bool adhere = false;
		bool solutionSmoothing = false; 
		double timeOfCurrentPosition =  775;
		double reRouteLeadIn = 45;
		Plan reRoute = Wx::reRoute(lpc,paths,adhere,solutionSmoothing,timeOfCurrentPosition,reRouteLeadIn);
		EXPECT_FALSE(reRoute.size() == 0);
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_reRoute2: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute2: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"test_Wx_reRoute2_reRoute");
			//DebugSupport.dumpAsUnitTest(reRoute);
		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		EXPECT_TRUE(tmConflict < 0);

	}

	
	TEST_F(WeatherUtilTest, test_example3_AND_descretizationError) {  
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		//fpln(" ----------------------------- test_example3_AND_descretizationError --------------------------");
		PolyPath pp = PolyPath("discreteError");
		SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
		Position vt0  = Position::makeLatLonAlt(42.85694265, -97.61814701, 0.0); sPoly0.add(vt0);
		Position vt1  = Position::makeLatLonAlt(42.89369765, -97.80426401, 0.0); sPoly0.add(vt1);
		Position vt2  = Position::makeLatLonAlt(43.07754055, -97.87108357, 0.0); sPoly0.add(vt2);
		Position vt3  = Position::makeLatLonAlt(43.03844029, -98.26208619, 0.0); sPoly0.add(vt3);
		Position vt4  = Position::makeLatLonAlt(42.87421919, -98.42005125, 0.0); sPoly0.add(vt4);
		Position vt5  = Position::makeLatLonAlt(42.75691840, -98.71877725, 0.0); sPoly0.add(vt5);
		Position vt6  = Position::makeLatLonAlt(42.78679765, -98.22924901, 0.0); sPoly0.add(vt6);
		Position vt7  = Position::makeLatLonAlt(42.65682173, -97.97430826, 0.0); sPoly0.add(vt7);
		Position vt8  = Position::makeLatLonAlt(42.22577078, -97.29396371, 0.0); sPoly0.add(vt8);  // ****
		Position vt9  = Position::makeLatLonAlt(42.63805360, -96.57451889, 0.0); sPoly0.add(vt9);
		Velocity v0  = Velocity::makeTrkGsVs(55.0,30,0.0); 		
		pp.addPolygon(sPoly0,v0,600.0); 
		pp.setPathMode(PolyPath::USER_VEL);
		//fpln(pp.toString());		

		std::vector<PolyPath> paths = std::vector<PolyPath>(1);
		paths.push_back(pp);
		//fpln(" $$$ test_Wx_discreteError: paths = "+paths);

		Plan lpc("Aircraft_0");
		Position p0  = Position::makeLatLonAlt(43.78669800, -101.39917900, 5000.0);
		Position p1  = Position::makeLatLonAlt(43.96759200, -99.97042100, 5000.0);
		Position p2  = Position::makeLatLonAlt(43.08151700, -98.63670900, 5000.0);
		Position p3  = Position::makeLatLonAlt(42.59708800, -95.92329600, 5000.0);
		Position p4  = Position::makeLatLonAlt(43.14283700, -94.20326800, 5000.0);
		NavPoint np0(p0,600.0);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,1432.243205);    	 lpc.addNavPoint(np1);
		NavPoint np2(p2,2476.146025);    	 lpc.addNavPoint(np2);
		NavPoint np3(p3,4105.920308);    	 lpc.addNavPoint(np3);
		NavPoint np4(p4,5199.221462);    	 lpc.addNavPoint(np4);

		double tmConflict_lpc = PolyUtil::isPlanInConflictWx(lpc, paths, 10.0).first;
		//fpln(" $$$ tmConflict_lpc = "+tmConflict_lpc);
		EXPECT_NEAR(2750,tmConflict_lpc,0.01);
		//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"reRoute3_pp");
		bool adhere = true;
		bool solutionSmoothing = true;
		Plan reRoute = Wx::reRoute(lpc, paths, adhere, solutionSmoothing);
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_discreteError: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_discreteError: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"reRoute3_reRoute");
			//DebugSupport.dumpAsUnitTest(reRoute);
		}
		EXPECT_FALSE(reRoute.size() == 0);
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 10.0).first;
		//fpln(" $$$ tmConflict = "+tmConflict);
		EXPECT_TRUE(tmConflict < 0);
		//  The FOLLOWING Conflict is due to discretization
		tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		//fpln(" $$$ tmConflict = "+tmConflict);
		EXPECT_TRUE(tmConflict > 0);
	}


	
	TEST_F(WeatherUtilTest, test_Wx_reRoute4) {   //square
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		//fpln(" ----------------------------- test_Wx_reRoute4 --------------------------");
		PolyPath pp = PolyPath(".");
		SimplePoly sPoly0 = SimplePoly(0.0,40000);
		Position p0  = Position::mkXYZ(1000,  25000, 0.0); sPoly0.add(p0);
		Position p1  = Position::mkXYZ(1000, 75000, 0.0); sPoly0.add(p1);
		Position p2  = Position::mkXYZ(40000,75000, 0.0); sPoly0.add(p2);
		Position p3  = Position::mkXYZ(40000, 25000, 0.0); sPoly0.add(p3);
		Velocity v0  = Velocity::ZEROV();
		pp.addPolygon(sPoly0,v0,0.0); 
	    //fpln(" $$$$ WeatherUtilTest: pp = "+pp.toString());
		EXPECT_EQ(4,sPoly0.size());
		pp.setPathMode(PolyPath::USER_VEL);
		EXPECT_EQ(1,pp.size());
		EXPECT_EQ(4,pp.getPoly(0).size());
		//fpln(" $$$$ sPoly0 = "+sPoly0.toOutput("FOO",4,0));
		Plan own("Aircraft_0");
		Position pA  = Position::mkXYZ(-35000, 15000, 25000.0);
		Position pB  = Position::mkXYZ(70000, 95000, 35000.0);  
		double startTime = 600;
		NavPoint npA(pA,startTime);     own.addNavPoint(npA);
		NavPoint npB(pB,2000);    	   own.addNavPoint(npB);
		//DebugSupport::dumpPlanAndPoly(own,sPoly0,"reRoute4_pp");
		//DebugSupport::dumpPlan(own,"reRoute4_own");
		std::vector<PolyPath> paths; // = std::vector<PolyPath>(1);
		paths.push_back(pp);
		EXPECT_EQ(1,pp.size());

		bool adhere = true;
		bool solutionSmoothing = true;
		//double  cellSize = 5010;
		double timeOfCurrentPosition =  775;
		double reRouteLeadIn = 45;
		EXPECT_EQ((unsigned long)1,paths.size());
		EXPECT_EQ(4,pp.getPoly(1).size());
		//fpln(" $$$ test_Wx_reRoute4: paths[0] = "+paths[0].toString());
		Plan reRoute = Wx::reRoute(own,paths,adhere,solutionSmoothing,timeOfCurrentPosition,reRouteLeadIn);
		//fpln("\n -----------------------------------------------------");
		//DebugSupport::dumpPlan(own,"reRoute4_own");
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		//fpln(" $$$ test_Wx_reRoute4: tmConflict = "+Fm1(tmConflict));
		EXPECT_TRUE(tmConflict < 0);
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_reRoute4: REROUTE FAILED!");
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute4: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"reRoute4_reRoute");
		}
		EXPECT_FALSE(reRoute.size() == 0);
		double gs0 = own.gsOut(0);
		for (int j = 0; j < reRoute.size(); j++) {
			EXPECT_NEAR(gs0,reRoute.gsOut(j),0.001);
			//fpln(" ^^^^^^^^^^^ reRoute.vsOut(j) = "+reRoute.vsOut(j));
		}			
	}


	TEST_F(WeatherUtilTest, test_Wx_morph) {
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

	      //fpln(" ----------------------------- test_Wx_morph --------------------------");
		PolyPath pp = PolyPath("pp");
		SimplePoly sPoly0 = SimplePoly(0.0,22000.0);
		Position p0  = Position::makeLatLonAlt(41.527722, -96.628498, 0.0); sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(40.816744, -97.641012, 0.0); sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(40.970000, -96.300000, 0.0); sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(41.302729, -96.152969, 0.0); sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(41.446725, -96.152969, 0.0); sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(41.455724, -96.0904, 0.0);   sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(41.455724, -96.065372, 0.0); sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(41.446725, -95.927719, 0.0); sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(41.671718, -96.115428, 0.0); sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(42.603708, -95.665706, 0.0); sPoly0.add(p9);
		Position p10  = Position::makeLatLonAlt(42.031707, -96.390734, 0.0);sPoly0.add(p10);
		SimplePoly sPoly2 = SimplePoly(0.0,22000.0);
		Position p200  = Position::makeLatLonAlt(41.023523, -96.576657, 0.0);     sPoly2.add(p200);
		Position p201  = Position::makeLatLonAlt(41.203516, -95.725078, 0.0);     sPoly2.add(p201);
		Position p202  = Position::makeLatLonAlt(41.134370, -93.842577, 0.0);     sPoly2.add(p202);
		Position p203  = Position::makeLatLonAlt(41.303354, -93.440822, 0.0);     sPoly2.add(p203);
		Position p204  = Position::makeLatLonAlt(41.585355, -93.403345, 0.0);     sPoly2.add(p204);
		Position p205  = Position::makeLatLonAlt(41.621354, -93.390702, 0.0);     sPoly2.add(p205);
		Position p206  = Position::makeLatLonAlt(41.639353, -93.390638, 0.0);     sPoly2.add(p206);
		Position p207  = Position::makeLatLonAlt(41.702350, -93.615662, 0.0);     sPoly2.add(p207);
		Position p208  = Position::makeLatLonAlt(41.914485, -95.947771, 0.0);     sPoly2.add(p208);
		Position p209  = Position::makeLatLonAlt(41.923485, -95.947738, 0.0);     sPoly2.add(p209);
		Position p210  = Position::makeLatLonAlt(41.689717, -96.165483, 0.0);     sPoly2.add(p210);
		Velocity v0  = Velocity::mkTrkGsVs(0.674265, 8.698876, 0.0);
		pp.addPolygon(sPoly0,v0,500.0);
		Velocity v2  = Velocity::ZEROV();
		pp.addPolygon(sPoly2,v2,7674.2781);
		pp.setPathMode(PolyPath::MORPHING);
		//pp.setPathMode(PolyPath::USER_VEL);     // THIS WORKS
		std::vector<PolyPath> paths = std::vector<PolyPath>(1);
		paths.push_back(pp);

		Plan lpc("Flight288");
		p0  = Position::makeLatLonAlt(42.02229600, -102.50575500, 30000.0);
		p1  = Position::makeLatLonAlt(41.16000000, -98.17000000, 30000.0);
		p2  = Position::makeLatLonAlt(41.59275900, -92.63952800, 30000.0);
		p3  = Position::makeLatLonAlt(41.79722200, -87.01268000, 30000.0);
		NavPoint np0(p0,500.0);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,2527.836300);    	 lpc.addNavPoint(np1);
		NavPoint np2(p2,5050.503800);    	 lpc.addNavPoint(np2);
		NavPoint np3(p3,7192.486300);    	 lpc.addNavPoint(np3);

		//fpln(" sizes:  "+sPoly0.size()+"  "+sPoly1.size()+" "+sPoly2.size());
		//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"morph_pp");
		bool solutionSmoothing = true;
		double timeOfCurrentPosition =  -1; // 775;
		double reRouteLeadIn = 45;
		double cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		double gridExtension = cellSize * 3.0;       // Stratway = 3*cellSize
		double T_p = MAXDOUBLE;
//		bool fastPolygonReroute = false;           // Stratway = true
		int fastPolygonReroute = 0;           // Stratway = true
		double adhereFactor = 1E8;
		std::vector<PolyPath> containment;

		//fpln(" $$$ test_Wx_morph:  paths(0) = "+paths.get(0).toStringShort());
		std::pair<Plan, DensityGrid> rrPair = WeatherUtil::reRouteWx(lpc, paths,  cellSize,  gridExtension, adhereFactor,  T_p,
				                         containment,  fastPolygonReroute, timeOfCurrentPosition,  reRouteLeadIn,
				                         solutionSmoothing);
		Plan reRoute = rrPair.first;
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_morph: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute9: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"morph_reRoute");
		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 10.0).first;
		EXPECT_TRUE(tmConflict < 0);
		EXPECT_EQ(5,reRoute.size());
		EXPECT_NEAR(reRoute.point(0).time(),500.0000,0.001);
		EXPECT_NEAR(reRoute.point(0).lat(),0.7334,0.001);
		EXPECT_NEAR(reRoute.point(0).lon(),-1.7891,0.001);
		EXPECT_NEAR(reRoute.point(2).time(),3280.6726,0.001);
		EXPECT_NEAR(reRoute.point(2).lat(),0.7128,0.001);
		EXPECT_NEAR(reRoute.point(2).lon(),-1.6856,0.001);
		EXPECT_NEAR(reRoute.point(2).alt(),9144.0000,0.001);
		EXPECT_NEAR(reRoute.point(4).lat(),0.7295,0.001);
		EXPECT_NEAR(reRoute.point(4).lon(),-1.5187,0.001);
	}



	TEST_F(WeatherUtilTest, test_reRoute5_After) {    // Example 5 in paper
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		PolyPath pp = PolyPath("pp");
		SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
		Position p0  = Position::makeLatLonAlt(42.87804, -99.68634, 0.0); sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(42.98844, -99.87381, 0.0); sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(42.98844, -100.01457, 0.0); sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(43.01069, -100.12236, 0.0); sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(42.96135, -100.25924, 0.0); sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(42.93435, -100.34674, 0.0); sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(42.80154, -100.34379, 0.0); sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(42.60998, -100.12927, 0.0); sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(42.22608, -99.460000, 0.0); sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(42.96235, -99.29644, 0.0); sPoly0.add(p9);
		Velocity v0  = Velocity::makeTrkGsVs(320,40, 0.000000);
		pp.addPolygon(sPoly0,v0,521.0);
		pp.setPathMode(PolyPath::USER_VEL_EVER);

		std::vector<PolyPath> paths;
		paths.push_back(pp);


		bool solutionSmoothing = true;
		double timeOfCurrentPosition =  -1; // 775;
		double reRouteLeadIn = 45;
		double cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		double gridExtension = cellSize * 5.0;       // Stratway = 3*cellSize
		double T_p = MAXDOUBLE;
		int fastPolygonReroute = 0;          // Stratway = true
		bool expandPolygons = false;              // should be tied to fastPolygonReroute
		double timeBefore = 0;
		double timeAfter =  2000;    // 2000
		double adhereFactor = 1E8;
	    EXPECT_EQ((unsigned long)1,paths.size());


		std::vector<PolyPath> npaths = WeatherUtil::ReRouteExpandIt(paths, cellSize, expandPolygons, timeBefore, timeAfter);
		EXPECT_EQ((unsigned long)1,npaths.size());
	    PolyPath npath0 = npaths[0];
	    EXPECT_EQ(1,npath0.size());

	    SimplePoly stretchedPoly = npath0.getPoly(0);
	    EXPECT_EQ(8,stretchedPoly.size());
	    EXPECT_NEAR(stretchedPoly.getVertex(7).lat(), 0.7556, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(7).lon(), -1.7532, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(7).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(6).lat(), 0.7548, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(6).lon(), -1.7388, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(6).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(5).lat(), 0.7498, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(5).lon(), -1.7330, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(5).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(4).lat(), 0.7370, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(4).lon(), -1.7359, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(4).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(3).lat(), 0.7437, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(3).lon(), -1.7476, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(3).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(2).lat(), 0.7486, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(2).lon(), -1.7533, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(2).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(1).lat(), 0.7520, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(1).lon(), -1.7570, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(1).alt(), 0.0000, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(0).lat(), 0.7543, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(0).lon(), -1.7571, 0.0001);
	    EXPECT_NEAR(stretchedPoly.getVertex(0).alt(), 0.0000, 0.0001);




		Plan lpc("Aircraft_0");
		p1  = Position::makeLatLonAlt(42.67300367, -102.03693631, 5000.0);
		p2  = Position::makeLatLonAlt(42.54875358, -100.92788920, 5000.0);
		p3  = Position::makeLatLonAlt(43.11017992, -99.23900833, 5000.0);
		p4  = Position::makeLatLonAlt(42.95831870, -98.34624841, 5000.0);
		p5  = Position::makeLatLonAlt(42.94451313, -97.31083098, 5000.0);
		NavPoint np1(p1,521.331298);    	 lpc.addNavPoint(np1);
		NavPoint np2(p2,1115.777934);    	 lpc.addNavPoint(np2);
		NavPoint np3(p3,2094.875880);    	 lpc.addNavPoint(np3);
		NavPoint np4(p4,2577.270735);    	 lpc.addNavPoint(np4);
		NavPoint np5(p5,3123.013328);    	 lpc.addNavPoint(np5);

		//fpln(" sizes:  "+sPoly0.size()+"  "+sPoly1.size()+" "+sPoly2.size());
		//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"reRoute5_After_pp");
		std::vector<PolyPath> empty;
		std::pair<Plan, DensityGrid> rrPair = WeatherUtil::reRouteWxExpand(lpc, paths,  cellSize,  gridExtension, adhereFactor,  T_p,
				empty,  fastPolygonReroute,  timeOfCurrentPosition,  reRouteLeadIn,
				expandPolygons,timeBefore, timeAfter, solutionSmoothing);
		Plan reRoute = rrPair.first;
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_reRoute5_After: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute9: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"reRoute5_After_reRoute");
			EXPECT_EQ(5,reRoute.size());
		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		EXPECT_TRUE(tmConflict < 0);
	}



	TEST_F(WeatherUtilTest, test_reRoute5_Before) {
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		PolyPath pp = PolyPath("pp");
		SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
		Position p0  = Position::makeLatLonAlt(42.72536068, -99.82259398, 0.0); sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(42.92309100, -99.63527052, 0.0); sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(43.00121530, -100.05160160, 0.0); sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(43.02346530, -100.15939160, 0.0); sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(42.97412530, -100.29627160, 0.0); sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(42.94712530, -100.38377160, 0.0); sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(42.81431530, -100.38082160, 0.0); sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(42.46120634, -100.13139525, 0.0); sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(42.34551031, -99.66649109, 0.0); sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(42.40795147, -98.88077322, 0.0); sPoly0.add(p9);
		Velocity v0  = Velocity::makeTrkGsVs(320,60, 0.000000);
		pp.addPolygon(sPoly0,v0,600.0);
		pp.setPathMode(PolyPath::USER_VEL_EVER);

		std::vector<PolyPath> paths = std::vector<PolyPath>(1);
		paths.push_back(pp);

		Plan lpc("Aircraft_1");
		Position pp0  = Position::makeLatLonAlt(42.15396215, -102.16977933, 5000.0);
		Position pp1  = Position::makeLatLonAlt(42.2615765, -96.97207928, 5000.0);
		NavPoint np0(pp0,521.331298);    	 lpc.addNavPoint(np0);
		NavPoint np1(pp1,2577.270735);    	 lpc.addNavPoint(np1);

		//fpln(" sizes:  "+sPoly0.size()+"  "+sPoly1.size()+" "+sPoly2.size());
		//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"reRoute5_Before_pp");
		bool solutionSmoothing = true;
		double timeOfCurrentPosition =  -1; // 775;
		double reRouteLeadIn = 45;
		double cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		double gridExtension = cellSize * 5.0;       // Stratway = 3*cellSize
		double T_p = MAXDOUBLE;
//		bool fastPolygonReroute = false;          // Stratway = true
		int fastPolygonReroute = 0;          // Stratway = true
		bool expandPolygons = false;              // should be tied to fastPolygonReroute
		double timeBefore = 3600;
		double timeAfter =  0;
		double adhereFactor = 1E8;
		std::vector<PolyPath> empty;
		std::pair<Plan, DensityGrid> rrPair = WeatherUtil::reRouteWxExpand(lpc, paths,  cellSize,  gridExtension, adhereFactor,  T_p,
				empty,  fastPolygonReroute,  timeOfCurrentPosition,  reRouteLeadIn,
				expandPolygons,timeBefore, timeAfter, solutionSmoothing);
		Plan reRoute = rrPair.first;
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_reRoute5-Before: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute9: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"reRoute5_Before_reRoute");
		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		EXPECT_TRUE(tmConflict < 0);
	}



	TEST_F(WeatherUtilTest, test_Wx_reRoute8) {     // Example 4 in the paper
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		PolyPath pp = PolyPath("pp");
		SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
		Position p0  = Position::makeLatLonAlt(41.527722, -96.628498, 0.0); sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(40.416744, -96.941012, 0.0); sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(40.960739, -96.303136, 0.0); sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(41.302729, -96.152969, 0.0); sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(41.446725, -96.152969, 0.0); sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(41.455724, -96.0904, 0.0);   sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(41.455724, -96.065372, 0.0); sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(41.446725, -95.927719, 0.0); sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(41.671718, -96.115428, 0.0); sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(42.013708, -96.365706, 0.0); sPoly0.add(p9);
		Position p10  = Position::makeLatLonAlt(42.031707, -96.390734, 0.0);sPoly0.add(p10);
		SimplePoly sPoly1 = SimplePoly(0.0,10668.0);
		Position p200  = Position::makeLatLonAlt(41.023523, -96.576657, 0.0);     sPoly1.add(p200);
		Position p201  = Position::makeLatLonAlt(41.203516, -95.725078, 0.0);     sPoly1.add(p201);
		Position p202  = Position::makeLatLonAlt(41.234370, -93.842577, 0.0);     sPoly1.add(p202);
		Position p203  = Position::makeLatLonAlt(41.603354, -93.440822, 0.0);     sPoly1.add(p203);
		Position p204  = Position::makeLatLonAlt(41.585355, -93.403345, 0.0);     sPoly1.add(p204);
		Position p205  = Position::makeLatLonAlt(41.621354, -93.390702, 0.0);     sPoly1.add(p205);
		Position p206  = Position::makeLatLonAlt(41.639353, -93.390638, 0.0);     sPoly1.add(p206);
		Position p207  = Position::makeLatLonAlt(41.702350, -93.615662, 0.0);     sPoly1.add(p207);
		Position p208  = Position::makeLatLonAlt(41.914485, -95.947771, 0.0);     sPoly1.add(p208);
		Position p209  = Position::makeLatLonAlt(41.923485, -95.947738, 0.0);     sPoly1.add(p209);
		Position p210  = Position::makeLatLonAlt(41.689717, -96.165483, 0.0);     sPoly1.add(p210);
		Velocity v0  = Velocity::mkTrkGsVs(0.674265, 8.698876, 0.0);
		pp.addPolygon(sPoly0,v0,500.0);
		Velocity v1  = Velocity::ZEROV();
		pp.addPolygon(sPoly1,v1,7674.2781);
		pp.setPathMode(PolyPath::MORPHING);

		std::vector<PolyPath> paths = std::vector<PolyPath>(1);
		paths.push_back(pp);

		PolyPath ppc = PolyPath("Container");
		SimplePoly sPolyC = SimplePoly(0.0,3048.0);
		p0  = Position::makeLatLonAlt(38.3906, -103.0538, 0.0);     sPolyC.add(p0);
		p1  = Position::makeLatLonAlt(38.6451, -96.3074, 0.0);     sPolyC.add(p1);
		p2  = Position::makeLatLonAlt(38.8743, -83.3366, 0.0);     sPolyC.add(p2);
		p3  = Position::makeLatLonAlt(44.9801, -85.9404, 0.0);     sPolyC.add(p3);
		p4  = Position::makeLatLonAlt(43.7853, -90.4068, 0.0);     sPolyC.add(p4);
		p5  = Position::makeLatLonAlt(40.2108, -98.3186, 0.0);     sPolyC.add(p5);
		p6  = Position::makeLatLonAlt(45.1878, -101.0426, 0.0);     sPolyC.add(p6);
		p7  = Position::makeLatLonAlt(45.0351, -102.8756, 0.0);     sPolyC.add(p7);
		Velocity vC  = Velocity::ZEROV();
		ppc.addPolygon(sPolyC,vC,0.0);
		ppc.setPathMode(PolyPath::USER_VEL_FINITE);
		std::vector<PolyPath> containment = std::vector<PolyPath>(1);
		containment.push_back(ppc);

		Plan lpc("Flight288");
		p0  = Position::makeLatLonAlt(42.02229600, -102.50575500, 30000.0);
		p1  = Position::makeLatLonAlt(41.16477300, -98.17540100, 30000.0);
		p2  = Position::makeLatLonAlt(41.59275900, -92.63952800, 30000.0);
		p3  = Position::makeLatLonAlt(41.79722200, -87.01268000, 30000.0);
		NavPoint np0(p0,500.0);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,2527.836300);    	 lpc.addNavPoint(np1);
		NavPoint np2(p2,5050.503800);    	 lpc.addNavPoint(np2);
		NavPoint np3(p3,7192.486300);    	 lpc.addNavPoint(np3);

		bool solutionSmoothing = false;
		double timeOfCurrentPosition =  -1; // 775;
		double reRouteLeadIn = 45;
		double cellSize = Units::from("nmi", 10.0);   // Stratway = 10 NM
		double gridExtension = cellSize * 8.0;       // Stratway = 3*cellSize
		double T_p = MAXDOUBLE;
		int fastPolygonReroute = 0;           // Stratway = true
		double adhereFactor = 1E8;
		std::pair<Plan, DensityGrid> rrPair = WeatherUtil::reRouteWx(lpc, paths,  cellSize,  gridExtension, adhereFactor,  T_p,
				                         containment,  fastPolygonReroute, timeOfCurrentPosition,  reRouteLeadIn,
				                         solutionSmoothing);
		Plan reRoute = rrPair.first;
		if (reRoute.size() == 0) {
			fpln(" ### test_Wx_reRoute8: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		EXPECT_TRUE(tmConflict < 0);
	}




	TEST_F(WeatherUtilTest, test_Wx_reRoute9) {
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		PolyPath pp = PolyPath("reRoute9_A");
		SimplePoly poly1 = SimplePoly(0.0,10668.0);

		Position p0  = Position::makeLatLonAlt(42.854669, -100.092760, 0.0);     poly1.add(p0);
		Position p1  = Position::makeLatLonAlt(42.857783, -100.176398, 0.0);     poly1.add(p1);
		Position p2  = Position::makeLatLonAlt(42.830783, -100.176398, 0.0);     poly1.add(p2);
		Position p3  = Position::makeLatLonAlt(42.803783, -100.151398, 0.0);     poly1.add(p3);
		Position p4  = Position::makeLatLonAlt(42.776783, -100.151398, 0.0);     poly1.add(p4);
		Position p5  = Position::makeLatLonAlt(42.749783, -100.138898, 0.0);     poly1.add(p5);
		Position p6  = Position::makeLatLonAlt(42.722783, -100.126398, 0.0);     poly1.add(p6);
		Position p7  = Position::makeLatLonAlt(42.731783, -100.076298, 0.0);     poly1.add(p7);
		Position p8  = Position::makeLatLonAlt(42.698943,  -99.991896, 0.0);     poly1.add(p8);
		Position p9  = Position::makeLatLonAlt(42.756760,  -99.788820, 0.0);     poly1.add(p9);
		Velocity v0  = Velocity::makeTrkGsVs(55.0,30,0.0);
		pp.addPolygon(poly1,v0,600.0);
		pp.setPathMode(PolyPath::USER_VEL);
		//fpln(pp.toString());
		PolyPath pp2 = PolyPath("reRoute9-B");
		SimplePoly poly2 = SimplePoly(0.0,10668.0);

		p0  = Position::makeLatLonAlt(42.981353, -99.618865, 0.0);     poly2.add(p0);
		p1  = Position::makeLatLonAlt(43.018108, -99.804982, 0.0);     poly2.add(p1);
		p2  = Position::makeLatLonAlt(43.018108, -99.945743, 0.0);     poly2.add(p2);
		p3  = Position::makeLatLonAlt(42.985264, -100.027854, 0.0);     poly2.add(p3);
		p4  = Position::makeLatLonAlt(42.991011, -100.190414, 0.0);     poly2.add(p4);
		p5  = Position::makeLatLonAlt(42.964011, -100.277914, 0.0);     poly2.add(p5);
		//		p6  = Position::makeLatLonAlt(42.831208, -100.274967, 0.0);     poly2.addVertex(p6);  // *** PROBLEM ***
		p6  = Position::makeLatLonAlt(42.911208, -100.229967, 0.0);     poly2.add(p6);  // *** PROBLEM ***
		p7  = Position::makeLatLonAlt(42.946011, -100.215314, 0.0);     poly2.add(p7);
		p8  = Position::makeLatLonAlt(42.855451, -99.679080, 0.0);     poly2.add(p8);
		p9  = Position::makeLatLonAlt(42.992011, -99.227614, 0.0);     poly2.add(p9);
		Velocity v2  = Velocity::makeTrkGsVs(276.0,15,0.0);
		pp2.addPolygon(poly2,v2,600.0);
		pp2.setPathMode(PolyPath::USER_VEL);

		std::vector<PolyPath> paths = std::vector<PolyPath>();
		paths.push_back(pp);
		paths.push_back(pp2);
		//fpln(" $$$ test_Wx_reRoute9: paths = "+paths);
		Plan own("Aircraft_0");
		Position pA(LatLonAlt::make(42.88778509, -100.48790, 5000.0));
		Position pB(LatLonAlt::make(42.80616624, -99.127460, 5000.0));
		NavPoint npA(pA,600);    	 own.addNavPoint(npA);
		NavPoint npB(pB,2000);    	 own.addNavPoint(npB);

		//DebugSupport.dumpPlanAndPolyPath(own,pp,"reRoute9_pp");

		bool adhere = false;
		bool solutionSmoothing = false;
		double timeOfCurrentPosition =  775;
		double reRouteLeadIn = 45;
		Plan reRoute = Wx::reRoute(own,paths,adhere,solutionSmoothing,timeOfCurrentPosition,reRouteLeadIn);
		EXPECT_FALSE(reRoute.size() == 0);
		if (reRoute.size() == 0) {
			//fpln(" ### test_Wx_reRoute9: REROUTE FAILED!");
			EXPECT_TRUE(false);
		} else {
			//fpln(" $$$ REROUTE SUCCESS !!! -----------------------------------------------");
			//fpln(" $$$ test_Wx_reRoute9: reRoute = "+reRoute);
			//DebugSupport.dumpPlan(reRoute,"reRoute9_reRoute");
			//DebugSupport.dumpAsUnitTest(reRoute);
			EXPECT_EQ(6,reRoute.size());
			EXPECT_NEAR(reRoute.point(0).time(), 775.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(0).lat(), 0.7484, 0.0001);
			EXPECT_NEAR(reRoute.point(0).lon(), -1.7509, 0.0001);
			EXPECT_NEAR(reRoute.point(0).alt(), 1524.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(1).time(), 820.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(1).lat(), 0.7483, 0.0001);
			EXPECT_NEAR(reRoute.point(1).lon(), -1.7501, 0.0001);
			EXPECT_NEAR(reRoute.point(1).alt(), 1524.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(2).time(), 1053.1562, 0.0001);
			EXPECT_NEAR(reRoute.point(2).lat(), 0.7454, 0.0001);
			EXPECT_NEAR(reRoute.point(2).lon(), -1.7501, 0.0001);
			EXPECT_NEAR(reRoute.point(2).alt(), 1524.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(3).time(), 2081.0938, 0.0001);
			EXPECT_NEAR(reRoute.point(3).lat(), 0.7454, 0.0001);
			EXPECT_NEAR(reRoute.point(3).lon(), -1.7327, 0.0001);
			EXPECT_NEAR(reRoute.point(3).alt(), 1524.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(4).time(), 2426.4856, 0.0001);
			EXPECT_NEAR(reRoute.point(4).lat(), 0.7472, 0.0001);
			EXPECT_NEAR(reRoute.point(4).lon(), -1.7309, 0.0001);
			EXPECT_NEAR(reRoute.point(4).alt(), 1524.0000, 0.0001);
			EXPECT_NEAR(reRoute.point(5).time(), 2471.4856, 0.0001);
			EXPECT_NEAR(reRoute.point(5).lat(), 0.7471, 0.0001);
			EXPECT_NEAR(reRoute.point(5).lon(), -1.7301, 0.0001);
			EXPECT_NEAR(reRoute.point(5).alt(), 1524.0000, 0.0001);

		}
		double tmConflict = PolyUtil::isPlanInConflictWx(reRoute, paths, 1.0).first;
		EXPECT_TRUE(tmConflict < 0);
	}

	TEST_F(WeatherUtilTest, test_Null_Paths) {   //square
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		double startTime = 600;
		Plan own("Aircraft_0");
		Position pA  = Position::mkXYZ(-55000, 15000, 25000.0);
		Position pB  = Position::mkXYZ(70000, 95000, 35000.0);
		NavPoint npA(pA,startTime);     own.addNavPoint(npA);
		NavPoint npB(pB,2000);    	   own.addNavPoint(npB);
		//DebugSupport.dumpPlan(own,"test_Null_Paths_own");

		bool solutionSmoothing = true;
		double  cellSize = 5010;
		double  gridExtension = 15030;
		double timeOfCurrentPosition =  -1;
		double reRouteLeadIn = 45;
		int fastPolygonReroute = 0;
		double adherenceFactor = 0.0;
		double T_p = 1E20;
		std::vector<PolyPath> empty;
		std::pair<Plan,DensityGrid> rrPair = WeatherUtil::reRouteWx(own, empty, cellSize, gridExtension, adherenceFactor, T_p,
				empty, fastPolygonReroute,
				timeOfCurrentPosition, reRouteLeadIn,solutionSmoothing);
		Plan reRoute = rrPair.first;
		//DebugSupport.dumpPlan(reRoute,"test_Null_Paths_rtn");

		EXPECT_EQ(own.point(0),reRoute.point(0));
		EXPECT_TRUE(own.getLastPoint().almostEquals(reRoute.getLastPoint()));
		double gs0 = own.gsOut(0);
		for (int j = 0; j < reRoute.size(); j++) {
			EXPECT_NEAR(gs0,reRoute.gsOut(j),0.001);
			//fpln(" ^^^^^^^^^^^ reRoute.vsOut(j) = "+reRoute.vsOut(j));
		}

		PolyPath pp = PolyPath(".");
		SimplePoly sPoly0 = SimplePoly(0.0,40000);
		Position p0  = Position::mkXYZ(1000,  25000, 0.0); sPoly0.add(p0);
		Position p1  = Position::mkXYZ(1000, 75000, 0.0); sPoly0.add(p1);
		Position p2  = Position::mkXYZ(40000,75000, 0.0); sPoly0.add(p2);
		Position p3  = Position::mkXYZ(40000, 25000, 0.0); sPoly0.add(p3);
		Velocity v0  = Velocity::ZEROV();
		pp.addPolygon(sPoly0,v0,0.0);
		pp.setPathMode(PolyPath::USER_VEL);
		//fpln(" $$$$ sPoly0 = "+sPoly0.toString());
		//DebugSupport.dumpPlanAndPolyPath(own,pp,"InitialContainment_pp");
		std::vector<PolyPath> container = std::vector<PolyPath>(1);
		container.push_back(pp);
		rrPair = WeatherUtil::reRouteWx(own, empty, cellSize, gridExtension, adherenceFactor, T_p,
				container, fastPolygonReroute,
				timeOfCurrentPosition, reRouteLeadIn,solutionSmoothing);

		reRoute = rrPair.first;
		//fpln("\n -----------------------------------------------------");
		EXPECT_TRUE(reRoute.size() == 0);  // initial point outside of container!
	}


	TEST_F(WeatherUtilTest, test_Null_Paths2) {   //square
		  // use old heuristics

	      DensityGridAStarSearch::setFourway(true);
	      DensityGridAStarSearch::setDirectionWeight(1.0);
	      DensityGridAStarSearch::setDistanceWeight(1.0);
	      DensityGridAStarSearch::setPredictedDistanceWeight(1.0);

		double startTime = 600;
		PolyPath pp = PolyPath(".");
		SimplePoly sPoly0 = SimplePoly(0.0,40000);
		Position p0  = Position::mkXYZ(1000,  25000, 0.0); sPoly0.add(p0);
		Position p1  = Position::mkXYZ(1000, 75000, 0.0); sPoly0.add(p1);
		Position p2  = Position::mkXYZ(40000,75000, 0.0); sPoly0.add(p2);
		Position p3  = Position::mkXYZ(40000, 25000, 0.0); sPoly0.add(p3);
		Velocity v0  = Velocity::makeVxyz(60.0,0.0,0.0);
		pp.addPolygon(sPoly0,v0,startTime);
		pp.setPathMode(PolyPath::USER_VEL);
		//fpln(" $$$$ sPoly0 = "+sPoly0.toString());
		Plan own("Aircraft_0");
		Position pA  = Position::mkXYZ(27500, 38600, 25000.0);
		Position pB  = Position::mkXYZ(73100, 38000, 35000.0);
		NavPoint npA(pA,startTime);     own.addNavPoint(npA);
		NavPoint npB(pB,1500);    	   own.addNavPoint(npB);
		//DebugSupport.dumpPlanAndPolyPath(own,pp,"null_paths2_pp");
		std::vector<PolyPath> container = std::vector<PolyPath>(1);
		container.push_back(pp);

		bool solutionSmoothing = true;
		double  cellSize = 5010;
		double  gridExtension = 15030;
		double timeOfCurrentPosition =  775;
		double reRouteLeadIn = 45;
		int fastPolygonReroute = 0;
		double adherenceFactor = 0.0;
		double T_p = 1E20;
		std::vector<PolyPath> empty;
		std::pair<Plan,DensityGrid> rrPair = WeatherUtil::reRouteWx(own, empty, cellSize, gridExtension, adherenceFactor, T_p,
				container, fastPolygonReroute,
				timeOfCurrentPosition, reRouteLeadIn,solutionSmoothing);

		Plan reRoute = rrPair.first;
		if (reRoute.size() == 0) {
			fpln(" ### test_Example1: REROUTE FAILED!");
		} else {
		    //DebugSupport.dumpPlan(reRoute,"test_null_naths2_rtn");
		}
		//fpln("\n -----------------------------------------------------");
		EXPECT_EQ(6,reRoute.size());
		double gs0 = own.gsOut(0);
		for (int j = 0; j < reRoute.size(); j++) {
			EXPECT_NEAR(gs0,reRoute.gsOut(j),0.001);
			//fpln(" ^^^^^^^^^^^ reRoute.vsOut(j) = "+reRoute.vsOut(j));
		}
	}



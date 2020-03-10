/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include <cmath>
#include <gtest/gtest.h>
#include "SimplePoly.h"
#include "Position.h"
#include "PolyUtil.h"
#include "PlanUtil.h"
#include "format.h"

using namespace larcfm;

class PolyUtilTest : public ::testing::Test {

public:

protected:
	virtual void SetUp() {
	}
};


TEST_F(PolyUtilTest, test_isPlanInConflictWx) {
	Plan lpc;
	Position p0(LatLonAlt::make(41.771518, -83.559022, 37000.000000));
	Position p1(LatLonAlt::make(41.673686, -81.058892, 0.000000));
	Position p2(LatLonAlt::make(42.056072, -78.395069, 0.000000));
	Position p3(LatLonAlt::make(41.272802, -75.689468, 0.000000));
	NavPoint np0(p0,0.000000);
	np0 = np0.makeName("IDEAS");    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1345.343635);
	np1 = np1.makeName("ZITER");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,2799.909519);
	np2 = np2.makeName("WUDMO");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,4360.550103);
	np3 = np3.makeName("LVZ");        	 lpc.addNavPoint(np3);

	PolyPath pp = PolyPath("pp");
	SimplePoly sPoly0 = SimplePoly(0.0,18288.0);
	Position pp0(LatLonAlt::make(39.066000, -81.866000, 0.000000));     sPoly0.add(pp0);
	Position pp1(LatLonAlt::make(38.900000, -81.766000, 0.000000));     sPoly0.add(pp1);
	Position pp2(LatLonAlt::make(38.700000, -81.766000, 0.000000));     sPoly0.add(pp2);
	Position pp3(LatLonAlt::make(38.516000, -81.800000, 0.000000));     sPoly0.add(pp3);
	Position pp4(LatLonAlt::make(38.300000, -81.933000, 0.000000));     sPoly0.add(pp4);
	Position pp5(LatLonAlt::make(38.083000, -82.283000, 0.000000));     sPoly0.add(pp5);
	Position pp6(LatLonAlt::make(38.083000, -82.500000, 0.000000));     sPoly0.add(pp6);
	Position pp7(LatLonAlt::make(38.116000, -82.516000, 0.000000));     sPoly0.add(pp7);
	Position pp8(LatLonAlt::make(38.333000, -82.516000, 0.000000));     sPoly0.add(pp8);
	Position pp9(LatLonAlt::make(39.050000, -82.083000, 0.000000));     sPoly0.add(pp9);
	Position pp10(LatLonAlt::make(39.066000, -82.066000, 0.000000));     sPoly0.add(pp10);
	Velocity v0  = Velocity::mkTrkGsVs(1.221730, 26.751111, 0.000000);
	pp.addPolygon(sPoly0,v0,18000.0);
	pp.setPathMode(PolyPath::USER_VEL);
	std::vector<PolyPath> paths;
	paths.push_back(pp);

	double gs = Units::from("kn",450);
	double startTime = 18300.0;
	Plan tsplan = Plan(lpc);
	tsplan.timeShiftPlan(0,startTime);
	tsplan = PlanUtil::mkGsConstant(tsplan,gs);
	//fpln(" $$$ testPlanClear2D:  tsplan  = "+tsplan);
	//fpln(" $$$ testPlanClear2D:  paths  = "+paths);
	double tmInter = PolyUtil::isPlanInConflictWx(lpc, paths, 10.0).first;
	EXPECT_TRUE(tmInter < 0);
}


TEST_F(PolyUtilTest, test_convexHull) {
	std::vector<Position> poly;
	Position p0  = Position::makeLatLonAlt(42.6576,  -99.9165, 10000.0); poly.push_back(p0);
	Position p2  = Position::makeLatLonAlt(42.8751, -100.3355, 10000.0); poly.push_back(p2);
	Position p4  = Position::makeLatLonAlt(42.5569, -100.6152, 10000.0); poly.push_back(p4);
	Position p6  = Position::makeLatLonAlt(42.3633, -100.5158, 10000.0); poly.push_back(p6);
	Position p8  = Position::makeLatLonAlt(42.5504, -100.1933, 10000.0); poly.push_back(p8);
	Position p10 = Position::makeLatLonAlt(42.5234, -100.1182, 10000.0); poly.push_back(p10);
	Position p12 = Position::makeLatLonAlt(42.3858,  -99.8051, 10000.0); poly.push_back(p12);
	Position p14 = Position::makeLatLonAlt(42.5861,  -99.5293, 10000.0); poly.push_back(p14);
	double bottom = 4000;
	double top =  20000;
	SimplePoly sPoly = SimplePoly::mk(poly,  bottom,  top);
    //fpln(" $$$ sPoly =  "+sPoly.toString());
	EXPECT_NEAR(0.74451, sPoly.getVertex(0).lat(),  0.0001);
	EXPECT_NEAR(-1.7438,sPoly.getVertex(0).lon(), 0.0001);
	for (int j = 0; j < (int) poly.size(); j++) {
		//Position pos = poly.get(j);
		Position pos = PolyUtil::pushOut(sPoly, j, -0.0001);
		bool cont = sPoly.contains2D(pos);
		//fpln(" $$$ j = "+Fm0(j)+" pos = "+pos.toString()+" cont = "+bool2str(cont));
		EXPECT_TRUE(cont);
	}
	//DebugSupport.dumpPoly(sPoly,"sPoly");
	SimplePoly spHull = PolyUtil::convexHull(poly,  bottom,  top);
	//f.pln(" $$$$ spHull = "+spHull);
	//DebugSupport.dumpPoly(spHull,"spHull");
	//Plan dpc = new Plan("debug");
	EXPECT_NEAR(0.74276, spHull.getVertex(0).lat(),  0.0001);
	EXPECT_NEAR(-1.75607,spHull.getVertex(0).lon(), 0.0001);

	Position px0  = Position::makeLatLonAlt(42.39868605, -100.16342618, 7000.00000000);
	Position px1  = Position::makeLatLonAlt(42.71528835, -99.91838467, 7000.00000000);

	EXPECT_TRUE(spHull.contains2D(px0));
	EXPECT_TRUE(spHull.contains2D(px1));
	EXPECT_FALSE(spHull.contains(px0));
	EXPECT_FALSE(spHull.contains(px1));
	Plan dpc("Aircraft_0");
	NavPoint np0x(px0,0.000000);    dpc.addNavPoint(np0x);
	NavPoint np1x(px1,200);         dpc.addNavPoint(np1x);
    for (double t = 0; t <= 200; t = t+10) {
    	EXPECT_TRUE(spHull.contains2D(dpc.position(t)));
    }
	for (int j = 0; j < sPoly.size(); j++) {
		//Position pos = poly.get(j);
		//Position pos = moveInsideSlightly(spHull,j,0.00001);
		Position pos = PolyUtil::pushOut(sPoly, j, -0.00001);
		bool cont = spHull.contains2D(pos);
		//dpc.addNavPoint(new NavPoint(pos,j*100.0));
		//f.pln(" $$$ j = "+j+" pos = "+pos+" cont = "+cont);
		//DebugSupport.dumpPlan(dpc,"convexHull_dpc");
		//bool isInterior = j == 0 || j == 4 || j == 5;
		EXPECT_TRUE(cont);
	}
	double buffer = 0.0001;
	Plan lpc("Aircraft_0");
	Position po1 = PolyUtil::pushOut(spHull, 1, buffer);
	Position po2 = PolyUtil::pushOut(spHull, 2, buffer);
	Position po3 = PolyUtil::pushOut(spHull, 3, buffer);
	Position po4 = PolyUtil::pushOut(spHull, 4, buffer);
    EXPECT_NEAR(buffer,po1.distanceH(spHull.getVertex(1)),0.000001);
    EXPECT_NEAR(buffer,po2.distanceH(spHull.getVertex(2)),0.000001);
    EXPECT_NEAR(buffer,po3.distanceH(spHull.getVertex(3)),0.000001);
    EXPECT_NEAR(buffer,po4.distanceH(spHull.getVertex(4)),0.000001);

	NavPoint np1(po1,0.000);    	 lpc.addNavPoint(np1);
	NavPoint np2(po2,300.000);     lpc.addNavPoint(np2);
	NavPoint np3(po3,600.000);     lpc.addNavPoint(np3);
	NavPoint np4(po4,900.000);     lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc,"lpc");
	double step = 2.0;
	for (double t = 0.0; t < 900.0; t = t + step) {
		Position pos = lpc.position(t);
		bool sPolyContains = spHull.contains(pos);
		EXPECT_FALSE(sPolyContains);
	}
}



TEST_F(PolyUtilTest, testStretchOverTime) {
	//PolyPath pp = PolyPath("pp");
	SimplePoly sPoly0 = SimplePoly(0.0,20000.0);
	Position p0  = Position::makeXYZ(-2.0, 65.0, 0.000000);     sPoly0.add(p0);
	Position p1  = Position::makeXYZ(-2.0, 10.0, 0.000000);     sPoly0.add(p1);
	Position p2  = Position::makeXYZ(85.0, 10.0, 0.000000);     sPoly0.add(p2);
	Position p3  = Position::makeXYZ(85.0, 65.0, 0.000000);     sPoly0.add(p3);
	Velocity v0  = Velocity::makeTrkGsVs(30.000000, 200.000000, 0.000000);
	//pp.addPolygon(sPoly0,v0,0.0);
	//pp.setPathMode(PolyPath::PathMode::MORPHING);
	//DebugSupport::dumpPoly(sPoly0,"testStretchOverTime_0");
	double timeBefore = 0;
	double timeAfter = 100;
	SimplePoly ssp1 = PolyUtil::stretchOverTime(sPoly0, v0, timeBefore, timeAfter);
	//DebugSupport::dumpPoly(ssp1,"testStretchOverTime_1");
}


TEST_F(PolyUtilTest, testStretchOverTimeLL) {
	//PolyPath pp = PolyPath("pp");
	SimplePoly sPoly0 = SimplePoly(0.0,3048.0);
	Position p0(LatLonAlt::make(-0.87, 1.72, 0.000000));     sPoly0.add(p0);
	Position p1(LatLonAlt::make(-1.73, 1.72, 0.000000));     sPoly0.add(p1);
	Position p2(LatLonAlt::make(-1.73, 3.34, 0.000000));     sPoly0.add(p2);
	Position p3(LatLonAlt::make(-0.81, 3.32, 0.000000));     sPoly0.add(p3);
	//Velocity v0  = Velocity::mkTrkGsVs(0.000000, 0.000000, 0.000000);
	Velocity v0  = Velocity::makeTrkGsVs(230.000000, 800.000000, 0.000000);
	//pp.addPolygon(sPoly0,v0,0.0);
	//pp.setPathMode(PolyPath::PathMode::MORPHING);
	//DebugSupport::dumpPoly(sPoly0,"testStretchOverTimeLL_0");
	double timeBefore = 0;
	double timeAfter = 100;
	SimplePoly ssp1 = PolyUtil::stretchOverTime(sPoly0, v0, timeBefore, timeAfter);
	//DebugSupport::dumpPoly(ssp1,"testStretchOverTimeLl_1");
	//fpln(" $$$ ssp1 = "+ssp1);
	EXPECT_EQ(6,ssp1.size());
	//fpln(" 0 = "+ssp1.getVertex(0));
	EXPECT_NEAR(-0.0141,ssp1.getVertex(5).lat(),0.0001);
	EXPECT_NEAR(0.0579,ssp1.getVertex(5).lon(),0.0001);
	EXPECT_NEAR(0.0,ssp1.getVertex(5).alt(),0.0001);
	EXPECT_NEAR(-0.0343,ssp1.getVertex(2).lat(),0.0001);
	EXPECT_NEAR(0.0250,ssp1.getVertex(2).lon(),0.0001);
	EXPECT_NEAR(0.0,ssp1.getVertex(2).alt(),0.0001);
	EXPECT_NEAR(-0.0151,ssp1.getVertex(0).lat(),0.0001);
	EXPECT_NEAR(0.030,ssp1.getVertex(0).lon(),0.0001);
	EXPECT_NEAR(0.0,ssp1.getVertex(0).alt(),0.0001);
	timeBefore = 200;
	timeAfter = 100;
	SimplePoly ssp2 = PolyUtil::stretchOverTime(sPoly0, v0, timeBefore, timeAfter);
	//DebugSupport::dumpPoly(ssp2,"testStretchOverTimeLl_2");
}


TEST_F(PolyUtilTest, test_stretch1) {
	PolyPath pp = PolyPath("Weather");
	SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
	Position p0  = Position::makeLatLonAlt(42.879804, -100.168626, 0);  sPoly0.add(p0);
	Position p2  = Position::makeLatLonAlt(42.861805, -100.206167, 0);  sPoly0.add(p2);
	Position p4  = Position::makeLatLonAlt(42.834806, -100.206167, 0);  sPoly0.add(p4);
	Position p6  = Position::makeLatLonAlt(42.807807, -100.181140, 0);  sPoly0.add(p6);
	Position p8  = Position::makeLatLonAlt(42.780807, -100.181140, 0);  sPoly0.add(p8);
	Position p10  = Position::makeLatLonAlt(42.753808, -100.168626, 0); sPoly0.add(p10);
	Position p20  = Position::makeLatLonAlt(42.735809, -100.018459, 0); sPoly0.add(p20);
	Position p30  = Position::makeLatLonAlt(42.861805, -100.131084, 0); sPoly0.add(p30);
	SimplePoly sPoly1 = SimplePoly(0.0,10668.0);
	Position p100  = Position::makeLatLonAlt(42.927553, -99.947657, 0);     sPoly1.add(p100);
	Position p102  = Position::makeLatLonAlt(42.909554, -99.985268, 0);     sPoly1.add(p102);
	Position p104  = Position::makeLatLonAlt(42.882555, -99.985371, 0);     sPoly1.add(p104);
	Position p106  = Position::makeLatLonAlt(42.855557, -99.960447, 0);     sPoly1.add(p106);
	Position p108  = Position::makeLatLonAlt(42.828558, -99.960550, 0);     sPoly1.add(p108);
	Position p110  = Position::makeLatLonAlt(42.801559, -99.948139, 0);     sPoly1.add(p110);
	Position p120  = Position::makeLatLonAlt(42.783560, -99.798041, 0);     sPoly1.add(p120);
	Position p130  = Position::makeLatLonAlt(42.882555, -99.710065, 0);     sPoly1.add(p130);
	Velocity v0  = Velocity::makeTrkGsVs(45.0,400,0.0);
	pp.addPolygon(sPoly0,v0,600.0);
	Velocity v1  = Velocity::ZEROV();
	pp.addPolygon(sPoly1,v1,1500.0);
	pp.setPathMode(PolyPath::MORPHING);

	double timeBefore = 3;
	double timeAfter  = 100;
	//fpln(pp.toString());
	Plan lpc("Aircraft_0");
	Position pA  = Position::makeLatLonAlt(42.78805005, -100.44929625, 5000.0);
	Position pB  = Position::makeLatLonAlt(42.83636953, -99.24591111, 5000.0);
	NavPoint npA(pA,600);    	 lpc.addNavPoint(npA);
	NavPoint npB(pB,2000);    	 lpc.addNavPoint(npB);

	//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"stretch1_orig");
	PolyPath pStretch = PolyUtil::stretchOverTime(pp, timeBefore, timeAfter);
	//DebugSupport.dumpPolyPath(pStretch,"stretch1_STRETCH");
	EXPECT_EQ(PolyPath::USER_VEL_FINITE,pStretch.getPathMode());

	SimplePoly poly1 = pp.getPoly(0);
	//double tm = pp.getTime(0);
	SimplePoly polyStretch = pStretch.getPoly(0);
	for (int i = 0; i < poly1.size(); i++) {
		Position vertex = PolyUtil::pushOut(polyStretch, i, -0.0001);
		//fpln(" $$$ i = "+i);
		EXPECT_TRUE(polyStretch.contains(vertex));
	}
	//EXPECT_EQ(poly1.getVertex(0),polyStretch.getVertex(0));
}



TEST_F(PolyUtilTest, test_stretchOverTime) {
	PolyPath pp = PolyPath("Weather");
	SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
	Position p0  = Position::makeLatLonAlt(42.879804, -100.168626, 0); sPoly0.add(p0);
	Position p2  = Position::makeLatLonAlt(42.861805, -100.206167, 0); sPoly0.add(p2);
	Position p4  = Position::makeLatLonAlt(42.834806, -100.206167, 0); sPoly0.add(p4);
	Position p6  = Position::makeLatLonAlt(42.807807, -100.181140, 0); sPoly0.add(p6);
	Position p8  = Position::makeLatLonAlt(42.780807, -100.181140, 0); sPoly0.add(p8);
	Position p10  = Position::makeLatLonAlt(42.753808, -100.168626, 0); sPoly0.add(p10);
	Position p12  = Position::makeLatLonAlt(42.726809, -100.156112, 0); sPoly0.add(p12);
	Position p14  = Position::makeLatLonAlt(42.735809, -100.106056, 0); sPoly0.add(p14);
	Position p16  = Position::makeLatLonAlt(42.762808, -100.093542, 0); sPoly0.add(p16);
	Position p18  = Position::makeLatLonAlt(42.771808, -100.056000, 0); sPoly0.add(p18);
	Position p20  = Position::makeLatLonAlt(42.735809, -100.018459, 0); sPoly0.add(p20);
	Position p22  = Position::makeLatLonAlt(42.744808, -99.968403, 0); sPoly0.add(p22);
	Position p24  = Position::makeLatLonAlt(42.681810, -99.918347, 0); sPoly0.add(p24);
	Position p26  = Position::makeLatLonAlt(42.785887, -99.913565, 0); sPoly0.add(p26);
	Position p28  = Position::makeLatLonAlt(42.825806, -100.018459, 0); sPoly0.add(p28);
	Position p30  = Position::makeLatLonAlt(42.861805, -100.131084, 0); sPoly0.add(p30);
	SimplePoly sPoly1 = SimplePoly(0.0,10668.0);
	Position p100  = Position::makeLatLonAlt(42.927553, -99.947657, 0);     sPoly1.add(p100);
	Position p102  = Position::makeLatLonAlt(42.909554, -99.985268, 0);     sPoly1.add(p102);
	Position p104  = Position::makeLatLonAlt(42.882555, -99.985371, 0);     sPoly1.add(p104);
	Position p106  = Position::makeLatLonAlt(42.855557, -99.960447, 0);     sPoly1.add(p106);
	Position p108  = Position::makeLatLonAlt(42.828558, -99.960550, 0);     sPoly1.add(p108);
	Position p110  = Position::makeLatLonAlt(42.801559, -99.948139, 0);     sPoly1.add(p110);
	Position p112  = Position::makeLatLonAlt(42.774560, -99.935728, 0);     sPoly1.add(p112);
	Position p114  = Position::makeLatLonAlt(42.783560, -99.885638, 0);     sPoly1.add(p114);
	Position p116  = Position::makeLatLonAlt(42.810559, -99.873021, 0);     sPoly1.add(p116);
	Position p118  = Position::makeLatLonAlt(42.819558, -99.835445, 0);     sPoly1.add(p118);
	Position p120  = Position::makeLatLonAlt(42.783560, -99.798041, 0);     sPoly1.add(p120);
	Position p122  = Position::makeLatLonAlt(42.792560, -99.747951, 0);     sPoly1.add(p122);
	Position p124  = Position::makeLatLonAlt(42.810559, -99.697827, 0);     sPoly1.add(p124);
	Position p126  = Position::makeLatLonAlt(42.770559, -99.657827, 0);     sPoly1.add(p126);
	Position p128  = Position::makeLatLonAlt(42.747562, -99.635497, 0);     sPoly1.add(p128);
	Position p130  = Position::makeLatLonAlt(42.882555, -99.710065, 0);     sPoly1.add(p130);
	Velocity v0  = Velocity::makeTrkGsVs(0.0,400,0.0);
	pp.addPolygon(sPoly0,v0,600.0);
	Velocity v1  = Velocity::ZEROV();
	pp.addPolygon(sPoly1,v1,2500.0);
	pp.setPathMode(PolyPath::USER_VEL_FINITE);
	double timeBefore = 3;
	double timeAfter  = 100;
	//fpln(pp.toString());
	Plan lpc("Aircraft_0");
	Position pA  = Position::makeLatLonAlt(42.78805005, -100.44929625, 5000.0);
	Position pB  = Position::makeLatLonAlt(42.83636953, -99.24591111, 5000.0);
	NavPoint npA(pA,600);    	 lpc.addNavPoint(npA);
	NavPoint npB(pB,2000);    	 lpc.addNavPoint(npB);

	//DebugSupport.dumpPlanAndPolyPath(lpc,pp,"stretchOverTime_pp");
	PolyPath pStretch = PolyUtil::stretchOverTime(pp, timeBefore, timeAfter);
	//DebugSupport.dumpPolyPath(pStretch,"stretchOverTime_stretch");

	SimplePoly poly1 = pp.getPoly(0);
	//double tm = pp.getTime(0);
	SimplePoly polyStretch = pStretch.getPoly(0);
	polyStretch.reverseOrder();
	for (int i = 0; i < poly1.size(); i++) {
		Position vertex = poly1.getVertex(i);
		bool cont = polyStretch.contains(vertex);
		//fpln(" $$$ i = "+Fm0(i)+" cont = "+bool2str(cont));
		if (i != 13)
	    	EXPECT_TRUE(cont);
	}
	//EXPECT_EQ(poly1.getVertex(0),polyStretch.getVertex(0));
}


TEST_F(PolyUtilTest, test_bufferedConvexHull) {
	PolyPath pp = PolyPath("pp");
	SimplePoly sPoly0 = SimplePoly(0.0,3048.0);
	Position pp0  = Position::makeLatLonAlt(-0.870000, 1.720000, 0); sPoly0.add(pp0);
	Position pp1  = Position::makeLatLonAlt(-2.730000, 1.820000, 0); sPoly0.add(pp1);
	Position pp2  = Position::makeLatLonAlt(-1.770000, 3.640000, 0); sPoly0.add(pp2);
	Position pp3  = Position::makeLatLonAlt(-0.300000, 3.980000, 0); sPoly0.add(pp3);
	Position pp4  = Position::makeLatLonAlt(-0.750000, 3.760000, 0); sPoly0.add(pp4);
	Position pp5  = Position::makeLatLonAlt(-0.810000, 3.221000, 0); sPoly0.add(pp5);

	Velocity v0  = Velocity::makeTrkGsVs(30.00, 100.00, 0.00);
	pp.addPolygon(sPoly0,v0,1000.0);
	pp.setPathMode(PolyPath::USER_VEL);
	double cellSize = Units::from("nmi", 15.0);

	Plan own("myOwn");
	Position p0  = Position::makeLatLonAlt(-1.240071, -0.141961, 5000.0);
	Position p1  = Position::makeLatLonAlt(-1.290175, 5.440447, 5000.0);
	NavPoint np0(p0,1000.0);    	 own.addNavPoint(np0);
	NavPoint np1(p1,3991.956034);    	 own.addNavPoint(np1);
	PolyPath exp = PolyUtil::bufferedConvexHull(pp, cellSize*0.71, 200.0);
	//DebugSupport.dumpPlanAndPolyPath(own,pp,"bufferedConvexHull_orig");
	//DebugSupport.dumpPolyPath(exp,"bufferedConvexHull_convexHull");

	SimplePoly poly1 = pp.getPoly(0);
	SimplePoly polyStretch = exp.getPoly(0);
	for (int i = 0; i < poly1.size(); i++) {
		Position vertex = poly1.getVertex(i);
		//fpln(" $$$ i = "+i);
		if (i != 0)
			EXPECT_TRUE(polyStretch.contains(vertex));
	}
}




TEST_F(PolyUtilTest, test_bufferedConvexHull2) {
	std::vector<Position> poly = std::vector<Position>();
	Position p0  = Position::makeLatLonAlt(-22.55,  99.91, 10000.0); poly.push_back(p0);
	Position p2  = Position::makeLatLonAlt(-22.67, 100.33, 10000.0); poly.push_back(p2);
	Position p4  = Position::makeLatLonAlt(-22.55, 100.61, 10000.0); poly.push_back(p4);
	Position p6  = Position::makeLatLonAlt(-22.36, 100.51, 10000.0); poly.push_back(p6);
	Position p8  = Position::makeLatLonAlt(-22.55, 100.19, 10000.0); poly.push_back(p8);
	Position p10 = Position::makeLatLonAlt(-22.52, 100.11, 10000.0); poly.push_back(p10);
	Position p12 = Position::makeLatLonAlt(-22.38,  99.80, 10000.0); poly.push_back(p12);
	Position p14 = Position::makeLatLonAlt(-22.58,  99.52, 10000.0); poly.push_back(p14);
	double bottom = 4000;
	double top =  20000;
	SimplePoly sp0 = SimplePoly::mk(poly,bottom,top);
	//fpln(" $$$$ sp0 = "+sp0);
	//DebugSupport::dumpPoly(sp0,"sp0");
	double hbuff = 100;
	double vbuff = 200;
	SimplePoly spHull = PolyUtil::bufferedConvexHull(sp0,  hbuff,  vbuff);
	//DebugSupport::dumpPoly(spHull,"spHull");
	//fpln("  $$ spHull = "+spHull);
	for (int j = 0; j < (int) poly.size(); j++) {
		Position pos = poly[j];
		bool cont2D = spHull.contains2D(pos);
		//bool cont3D = spHull.contains(pos);
		//fpln(" $$$ j = "+j+" pos = "+pos+" cont2D = "+cont2D+" cont3D = "+cont3D);
		EXPECT_TRUE(cont2D);
		//EXPECT_TRUE(cont3D);  // TODO ?????
	}
}


TEST_F(PolyUtilTest, test_bufferedConvexHull3) {
	SimplePoly sPoly0(0.0,9448.800000000001);
	Position p0  = Position::makeLatLonAlt(43.000992, -96.260713, 0.000000);     sPoly0.add(p0);
	Position p1  =Position::makeLatLonAlt(42.991993, -96.273227, 0.000000);     sPoly0.add(p1);
	Position p2  = Position::makeLatLonAlt(42.991993, -96.285741, 0.000000);     sPoly0.add(p2);
	Position p3  = Position::makeLatLonAlt(42.982993, -96.310769, 0.000000);     sPoly0.add(p3);
	Position p4  = Position::makeLatLonAlt(42.973993, -96.323283, 0.000000);     sPoly0.add(p4);
	Position p5  = Position::makeLatLonAlt(42.964993, -96.323283, 0.000000);     sPoly0.add(p5);
	Position p6  = Position::makeLatLonAlt(42.955994, -96.323283, 0.000000);     sPoly0.add(p6);
	Position p7  = Position::makeLatLonAlt(42.946994, -96.323283, 0.000000);     sPoly0.add(p7);
	Position p8  = Position::makeLatLonAlt(42.928995, -96.323283, 0.000000);     sPoly0.add(p8);
	Position p9  = Position::makeLatLonAlt(42.928995, -96.335797, 0.000000);     sPoly0.add(p9);
	Position p10  = Position::makeLatLonAlt(42.928995, -96.348311, 0.000000);     sPoly0.add(p10);
	Position p11  = Position::makeLatLonAlt(42.928995, -96.360825, 0.000000);     sPoly0.add(p11);
	Position p12  = Position::makeLatLonAlt(42.928995, -96.373339, 0.000000);     sPoly0.add(p12);
	Position p13  = Position::makeLatLonAlt(42.919995, -96.398366, 0.000000);     sPoly0.add(p13);
	Position p14  = Position::makeLatLonAlt(42.919995, -96.410880, 0.000000);     sPoly0.add(p14);
	Position p15  = Position::makeLatLonAlt(42.910995, -96.410880, 0.000000);     sPoly0.add(p15);
	Position p16  = Position::makeLatLonAlt(42.901995, -96.410880, 0.000000);     sPoly0.add(p16);
	Position p17  = Position::makeLatLonAlt(42.892996, -96.423394, 0.000000);     sPoly0.add(p17);
	Position p18  = Position::makeLatLonAlt(42.874996, -96.410880, 0.000000);     sPoly0.add(p18);
	Position p19  = Position::makeLatLonAlt(42.865996, -96.410880, 0.000000);     sPoly0.add(p19);
	Position p20  = Position::makeLatLonAlt(42.874996, -96.398366, 0.000000);     sPoly0.add(p20);
	Position p21  = Position::makeLatLonAlt(42.865996, -96.385852, 0.000000);     sPoly0.add(p21);
	Position p22  = Position::makeLatLonAlt(42.856997, -96.385852, 0.000000);     sPoly0.add(p22);
	Position p23  = Position::makeLatLonAlt(42.856997, -96.360825, 0.000000);     sPoly0.add(p23);
	Position p24  = Position::makeLatLonAlt(42.865996, -96.348311, 0.000000);     sPoly0.add(p24);
	Position p25  = Position::makeLatLonAlt(42.856997, -96.335797, 0.000000);     sPoly0.add(p25);
	Position p26  = Position::makeLatLonAlt(42.847997, -96.335797, 0.000000);     sPoly0.add(p26);
	Position p27  = Position::makeLatLonAlt(42.838997, -96.323283, 0.000000);     sPoly0.add(p27);
	Position p28  = Position::makeLatLonAlt(42.820998, -96.335797, 0.000000);     sPoly0.add(p28);
	Position p29  = Position::makeLatLonAlt(42.811998, -96.348311, 0.000000);     sPoly0.add(p29);
	Position p30  = Position::makeLatLonAlt(42.811998, -96.335797, 0.000000);     sPoly0.add(p30);
	Position p31  = Position::makeLatLonAlt(42.811998, -96.323283, 0.000000);     sPoly0.add(p31);
	Position p32  = Position::makeLatLonAlt(42.802998, -96.310769, 0.000000);     sPoly0.add(p32);
	Position p33  = Position::makeLatLonAlt(42.820998, -96.298255, 0.000000);     sPoly0.add(p33);
	Position p34  = Position::makeLatLonAlt(42.829997, -96.285741, 0.000000);     sPoly0.add(p34);
	Position p35  = Position::makeLatLonAlt(42.820998, -96.273227, 0.000000);     sPoly0.add(p35);
	Position p36  = Position::makeLatLonAlt(42.811998, -96.273227, 0.000000);     sPoly0.add(p36);
	Position p37  = Position::makeLatLonAlt(42.829997, -96.260713, 0.000000);     sPoly0.add(p37);
	Position p38  = Position::makeLatLonAlt(42.838997, -96.248199, 0.000000);     sPoly0.add(p38);
	Position p39  = Position::makeLatLonAlt(42.829997, -96.235686, 0.000000);     sPoly0.add(p39);
	Position p40  = Position::makeLatLonAlt(42.829997, -96.223172, 0.000000);     //sPoly0.add(p40);
	Position p41  = Position::makeLatLonAlt(42.820998, -96.210658, 0.000000);     sPoly0.add(p41);
	Position p42  = Position::makeLatLonAlt(42.802998, -96.223172, 0.000000);     sPoly0.add(p42);
	Position p43  = Position::makeLatLonAlt(42.811998, -96.223172, 0.000000);     //sPoly0.add(p43);
	Position p44  = Position::makeLatLonAlt(42.774794, -96.203816, 0.000000);     sPoly0.add(p44);
	Position p45  = Position::makeLatLonAlt(42.829997, -96.198144, 0.000000);     sPoly0.add(p45);
	Position p46  = Position::makeLatLonAlt(42.838997, -96.185630, 0.000000);     sPoly0.add(p46);
	Position p47  = Position::makeLatLonAlt(42.856997, -96.160602, 0.000000);     sPoly0.add(p47);
	Position p48  = Position::makeLatLonAlt(42.865996, -96.173116, 0.000000);     sPoly0.add(p48);
	Position p49  = Position::makeLatLonAlt(42.874996, -96.173116, 0.000000);     sPoly0.add(p49);
	Position p50  = Position::makeLatLonAlt(42.883996, -96.185630, 0.000000);     sPoly0.add(p50);
	Position p51  = Position::makeLatLonAlt(42.892996, -96.198144, 0.000000);     sPoly0.add(p51);
	Position p52  = Position::makeLatLonAlt(42.892996, -96.223172, 0.000000);     sPoly0.add(p52);
	Position p53  = Position::makeLatLonAlt(42.901995, -96.235686, 0.000000);     sPoly0.add(p53);
	Position p54  = Position::makeLatLonAlt(42.910995, -96.235686, 0.000000);     sPoly0.add(p54);
	Position p55  = Position::makeLatLonAlt(42.919995, -96.248199, 0.000000);     sPoly0.add(p55);
	Position p56  = Position::makeLatLonAlt(42.928995, -96.235686, 0.000000);     sPoly0.add(p56);
	Position p57  = Position::makeLatLonAlt(42.946994, -96.223172, 0.000000);     sPoly0.add(p57);
	Position p58  = Position::makeLatLonAlt(42.955994, -96.210658, 0.000000);     sPoly0.add(p58);
	Position p59  = Position::makeLatLonAlt(42.964993, -96.210658, 0.000000);     sPoly0.add(p59);
	Position p60  = Position::makeLatLonAlt(42.973993, -96.210658, 0.000000);     sPoly0.add(p60);
	Position p61  = Position::makeLatLonAlt(42.982993, -96.210658, 0.000000);     sPoly0.add(p61);
	Position p62  = Position::makeLatLonAlt(43.000992, -96.235686, 0.000000);     sPoly0.add(p62);
	Position p63  = Position::makeLatLonAlt(42.991993, -96.248199, 0.000000);     sPoly0.add(p63);

	sPoly0.validate();
	double gridSize = Units::from("nmi", 10.0);
	//DebugSupport::dumpPoly(sPoly0,"badPoly_sPoly0");
	SimplePoly bch0 = PolyUtil::bufferedConvexHull(sPoly0, gridSize*0.72, 0.0);
	//DebugSupport::dumpPoly(bch0,"badPoly_bch0");
	fpln(" badPoly: sPoly0.size() = "+Fm0(sPoly0.size())+"  bch0.size() = "+Fm0(bch0.size()));
	EXPECT_EQ(21.0,bch0.size());
}

TEST_F(PolyUtilTest, test_intersectsPolygon2D) {
	SimplePoly sPoly0 = SimplePoly(0.0,10231.0);
	Position pp0(LatLonAlt::make(39.066000, -11.866000, 0.000));     sPoly0.add(pp0);
	//Position pp1(LatLonAlt::make(38.900, -11.766000, 0.000));     sPoly0.addVertex(pp1);
	Position pp2(LatLonAlt::make(38.700, -11.766000, 0.000));     sPoly0.add(pp2);
	//Position pp3(LatLonAlt::make(38.516000, -11.800, 0.000));     sPoly0.addVertex(pp3);
	Position pp4(LatLonAlt::make(38.300, -11.933000, 0.000));     sPoly0.add(pp4);
	//Position pp5(LatLonAlt::make(38.083000, -82.283000, 0.000));     sPoly0.addVertex(pp5);
	Position pp6(LatLonAlt::make(38.083000, -12.500, 0.000));     sPoly0.add(pp6);
	Position pp7(LatLonAlt::make(38.116000, -12.516000, 0.000));     sPoly0.add(pp7);
	Position pp8(LatLonAlt::make(38.333000, -12.516000, 0.000));     sPoly0.add(pp8);
	Position pp9(LatLonAlt::make(39.050, -12.083000, 0.000));     sPoly0.add(pp9);
	Position pp10(LatLonAlt::make(39.066000, -12.066000, 0.000));    sPoly0.add(pp10);

	PolyPath pp = PolyPath("WWW");
	Velocity v = Velocity::makeTrkGsVs(220,50,0.0);
	pp.addPolygon(sPoly0,v,0.0);
	//DebugSupport::dumpPolyPath(pp,"intersectsPolygon2D_poly");
	//fpln(" $$$ test_intersectsPolygon2D: pp = "+pp);

	Plan lpc("0");
	Position p0  = Position::makeLatLonAlt(36.78408, -21.91776, 5000.0);
	Position p1  = Position::makeLatLonAlt(36.73703, -12.77417, 5000.0);
	NavPoint np0(p0,0.000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,9000.000);    	 lpc.addNavPoint(np1);
	//DebugSupport::dumpPlan(lpc,"intersectsPolygon2D_lpc");

	double B = 12;
	double T = 100000;
	double incr = 1.0;
	double intersectionTime = PolyUtil::intersectsPolygon2D(lpc, pp, B, T, incr);
	//fpln(" $$$ intersectionTime = "+intersectionTime);
	EXPECT_NEAR(7823,intersectionTime,0.1);

	lpc = Plan("0");
	p0  = Position::makeLatLonAlt(36.78408, -21.91776, 5000.0);
	p1  = Position::makeLatLonAlt(36.617, -14.37, 5000.0);
	np0 = NavPoint(p0,0.000);    	 lpc.addNavPoint(np0);
	np1 = NavPoint(p1,9000.000);    lpc.addNavPoint(np1);
	//DebugSupport::dumpPlan(lpc,"intersectsPolygon2D_lpc");

	intersectionTime = PolyUtil::intersectsPolygon2D(lpc, pp, B, T, incr);  // bounding boxes do not intersect
	//fpln(" $$$ intersectionTime = "+intersectionTime);
	EXPECT_NEAR(-1,intersectionTime,0.1);

//	std::vector<Position> interList = PolyUtil::intersectsSimplePoly2D(p0,p1,sPoly0);
//	fpln(" $$$ interList = "+interList);
}





TEST_F(PolyUtilTest, test_reducePlanAgainstPolys) {
	Plan lpc("noname");
	Position p0  = Position::makeLatLonAlt(43.78669800, -101.39917900, 5000.0);
	Position p1  = Position::makeLatLonAlt(43.79693346, -101.32214969, 5000.0);
	Position p2  = Position::makeLatLonAlt(43.96360013, -100.15548349, 5000.0);
	Position p3  = Position::makeLatLonAlt(43.96360013, -99.82215016, 5000.0);
	Position p4  = Position::makeLatLonAlt(43.79693345, -99.82214948, 5000.0);
	Position p5  = Position::makeLatLonAlt(43.79693345, -99.48881615, 5000.0);
	Position p6  = Position::makeLatLonAlt(43.63026678, -99.48881547, 5000.0);
	Position p7  = Position::makeLatLonAlt(43.63026678, -99.32214881, 5000.0);
	Position p8  = Position::makeLatLonAlt(43.46360010, -99.32214814, 5000.0);
	Position p9  = Position::makeLatLonAlt(43.46360010, -98.98881480, 5000.0);
	Position p10  = Position::makeLatLonAlt(43.29693343, -98.98881414, 5000.0);
	Position p11  = Position::makeLatLonAlt(43.29693343, -98.82214747, 5000.0);
	Position p12  = Position::makeLatLonAlt(43.13026675, -98.82214681, 5000.0);
	Position p13  = Position::makeLatLonAlt(43.13026675, -98.48881347, 5000.0);
	Position p14  = Position::makeLatLonAlt(42.95860007, -98.50981281, 5000.0);
	Position p15  = Position::makeLatLonAlt(42.96360007, -97.65547948, 5000.0);
	Position p16  = Position::makeLatLonAlt(42.79693339, -97.65547882, 5000.0);
	Position p17  = Position::makeLatLonAlt(42.79693339, -97.32214549, 5000.0);
	Position p18  = Position::makeLatLonAlt(42.63026671, -97.32214484, 5000.0);
	Position p19  = Position::makeLatLonAlt(42.63026671, -96.98881150, 5000.0);
	Position p20  = Position::makeLatLonAlt(42.46360003, -96.98881085, 5000.0);
	Position p21  = Position::makeLatLonAlt(42.46360003, -96.48881085, 5000.0);
	Position p22  = Position::makeLatLonAlt(42.63026671, -96.48881150, 5000.0);
	Position p23  = Position::makeLatLonAlt(42.63026671, -95.32214484, 5000.0);
	Position p24  = Position::makeLatLonAlt(42.79693339, -95.32214549, 5000.0);
	Position p25  = Position::makeLatLonAlt(42.79693339, -94.82214549, 5000.0);
	Position p26  = Position::makeLatLonAlt(42.96360007, -94.82214615, 5000.0);
	Position p27  = Position::makeLatLonAlt(42.96360007, -94.32214615, 5000.0);
	Position p28  = Position::makeLatLonAlt(43.12088503, -94.27466317, 5000.0);
	Position p29  = Position::makeLatLonAlt(43.14283700, -94.20326800, 5000.0);
	NavPoint np0(p0,600.0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,645.000000);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1327.316647);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1518.278636);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,1650.931578);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,1842.428504);    	 lpc.addNavPoint(np5);
	NavPoint np6(p6,1975.081448);    	 lpc.addNavPoint(np6);
	NavPoint np7(p7,2071.096617);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,2203.749561);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,2396.311496);    	 lpc.addNavPoint(np9);
	NavPoint np10(p10,2528.964441);    	 lpc.addNavPoint(np10);
	NavPoint np11(p11,2625.510487);    	 lpc.addNavPoint(np11);
	NavPoint np12(p12,2758.163433);    	 lpc.addNavPoint(np12);
	NavPoint np13(p13,2951.783859);    	 lpc.addNavPoint(np13);
	NavPoint np14(p14,3084.436805);    	 lpc.addNavPoint(np14);
	NavPoint np15(p15,3569.803176);    	 lpc.addNavPoint(np15);
	NavPoint np16(p16,3702.456123);    	 lpc.addNavPoint(np16);
	NavPoint np17(p17,3897.128487);    	 lpc.addNavPoint(np17);
	NavPoint np18(p18,4029.781435);    	 lpc.addNavPoint(np18);
	NavPoint np19(p19,4224.977299);    	 lpc.addNavPoint(np19);
	NavPoint np20(p20,4357.630248);    	 lpc.addNavPoint(np20);
	NavPoint np21(p21,4651.206580);    	 lpc.addNavPoint(np21);
	NavPoint np22(p22,4783.859529);    	 lpc.addNavPoint(np22);
	NavPoint np23(p23,5467.040081);    	 lpc.addNavPoint(np23);
	NavPoint np24(p24,5599.693029);    	 lpc.addNavPoint(np24);
	NavPoint np25(p25,5891.701336);    	 lpc.addNavPoint(np25);
	NavPoint np26(p26,6024.354284);    	 lpc.addNavPoint(np26);
	NavPoint np27(p27,6315.574869);    	 lpc.addNavPoint(np27);
	NavPoint np28(p28,6443.771604);    	 lpc.addNavPoint(np28);
	NavPoint np29(p29,6488.771604);    	 lpc.addNavPoint(np29);
	PolyPath pp = PolyPath("pp");
	SimplePoly sPoly0 = SimplePoly(0.0,10668.0);
	p0  = Position::makeLatLonAlt(42.85694300, -97.61814700, 0.0); sPoly0.add(p0);
	p1  = Position::makeLatLonAlt(42.89369800, -97.80426400, 0.0); sPoly0.add(p1);
	p2  = Position::makeLatLonAlt(43.07754100, -97.87108400, 0.0); sPoly0.add(p2);
	p3  = Position::makeLatLonAlt(43.03844000, -98.26208600, 0.0); sPoly0.add(p3);
	p4  = Position::makeLatLonAlt(42.87421900, -98.42005100, 0.0); sPoly0.add(p4);
	p5  = Position::makeLatLonAlt(42.75691800, -98.71877700, 0.0); sPoly0.add(p5);
	p6  = Position::makeLatLonAlt(42.78679800, -98.22924900, 0.0); sPoly0.add(p6);
	p7  = Position::makeLatLonAlt(42.65682200, -97.97430800, 0.0); sPoly0.add(p7);
	p8  = Position::makeLatLonAlt(42.21577100, -97.29396400, 0.0); sPoly0.add(p8);
	p9  = Position::makeLatLonAlt(42.63805400, -96.57451900, 0.0); sPoly0.add(p9);
	Velocity v0  = Velocity::mkTrkGsVs(0.959931, 15.433333, 0.0);
	pp.addPolygon(sPoly0,v0,600.0);
	pp.setPathMode(PolyPath::USER_VEL);

	double incr = 5.0;                             // NOTE at 10.0 error because conflict last 7.6 secs
	double gs = lpc.initialVelocity(0).gs();
	bool leadInsPresent = true;
	std::vector<PolyPath> paths = std::vector<PolyPath>(1);
	paths.push_back(pp);
	double tmConflict = PolyUtil::isPlanInConflictWx(lpc, paths, incr).first;
	EXPECT_TRUE(tmConflict < 0);

	//DebugSupport::dumpPlan(lpc,"reducePlanAgainstPolys_lpc");
	//DebugSupport::dumpPlanAndPolyPath(lpc,paths.get(0),"reducePlanAgainstPolys_pp");

	Plan redPlan = PolyUtil::reducePlanAgainstPolys(lpc, gs, paths, incr, leadInsPresent);
	//DebugSupport::dumpPlan(redPlan,"reducePlanAgainstPolys_reduce");
	tmConflict = PolyUtil::isPlanInConflictWx(redPlan, paths, incr).first;
	EXPECT_TRUE(tmConflict < 0);

	PolyPath pp1 = PolyPath("pp");
	SimplePoly sPoly2 = SimplePoly(0.0,3048.0);
	p0  = Position::makeLatLonAlt(43.98110984, -101.50669809, 0.00000000);     sPoly2.add(p0);
	p1  = Position::makeLatLonAlt(43.62623267, -101.53035657, 0.00000000);     sPoly2.add(p1);
	p2  = Position::makeLatLonAlt(43.25952627, -99.18225267, 0.00000000);     sPoly2.add(p2);
	p3  = Position::makeLatLonAlt(43.31867246, -99.00481409, 0.00000000);     sPoly2.add(p3);
	p4  = Position::makeLatLonAlt(42.89873449, -98.99298485, 0.00000000);     sPoly2.add(p4);
	p5  = Position::makeLatLonAlt(42.60891814, -98.51981530, 0.00000000);     sPoly2.add(p5);
	p6  = Position::makeLatLonAlt(42.50836961, -97.90469488, 0.00000000);     sPoly2.add(p6);
	p7  = Position::makeLatLonAlt(42.44922341, -97.37237914, 0.00000000);     sPoly2.add(p7);
	p8  = Position::makeLatLonAlt(42.29544331, -96.55616166, 0.00000000);     sPoly2.add(p8);
	p9  = Position::makeLatLonAlt(42.42556494, -96.02384591, 0.00000000);     sPoly2.add(p9);
	p10  = Position::makeLatLonAlt(42.70946667, -95.31409158, 0.00000000);     sPoly2.add(p10);
	p11  = Position::makeLatLonAlt(42.82775905, -94.53927644, 0.00000000);     sPoly2.add(p11);
	p12  = Position::makeLatLonAlt(43.18855084, -93.85318059, 0.00000000);     sPoly2.add(p12);
	p13  = Position::makeLatLonAlt(43.04068535, -95.16031148, 0.00000000);     sPoly2.add(p13);
	p14  = Position::makeLatLonAlt(42.60300352, -96.56207628, 0.00000000);     sPoly2.add(p14);
	p15  = Position::makeLatLonAlt(43.09983155, -98.18268199, 0.00000000);     sPoly2.add(p15);
	p16  = Position::makeLatLonAlt(43.33050170, -98.59670535, 0.00000000);     sPoly2.add(p16);
	p17  = Position::makeLatLonAlt(43.53159876, -98.87469246, 0.00000000);     sPoly2.add(p17);
	Velocity v2 = Velocity::ZEROV();
	pp1.addPolygon(sPoly2,v2,0.0);
	pp1.setPathMode(PolyPath::AVG_VEL);

	std::vector<PolyPath> container1 = std::vector<PolyPath>();
	container1.push_back(pp1);
	//DebugSupport::dumpPolyPath(container1.get(0),"reducePlanAgainstPolys_container1");
	double B = 0;
	double T = 1E20;
	bool pc = PolyUtil::isPlanContained(redPlan, container1,  B,  T,  incr);
	EXPECT_TRUE(pc);
	int lastVert = sPoly2.size()-1;
	sPoly2.remove(lastVert);
	//sPoly2.remove(lastVert-1);
	sPoly2.remove(lastVert-2);
	//sPoly2.remove(lastVert-3);
	//sPoly2.remove(lastVert-4);
	//sPoly2.remove(lastVert-5);
	std::vector<PolyPath> container2 = std::vector<PolyPath>();
	PolyPath pp2 = PolyPath("pp");
	pp2.addPolygon(sPoly2,Velocity::ZEROV(),0.0);
	pp2.setPathMode(PolyPath::AVG_VEL);
	container2.push_back(pp2);
	//DebugSupport::dumpPolyPath(container2.get(0),"reducePlanAgainstPolys_container2");
	pc = PolyUtil::isPlanContained(redPlan, container2,  B,  T,  incr);
	EXPECT_FALSE(pc);
}





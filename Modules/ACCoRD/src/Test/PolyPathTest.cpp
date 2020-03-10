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
#include "PolyPath.h"
#include "Projection.h"
#include "format.h"

using namespace larcfm;

class PolyPathTest : public ::testing::Test {

public:

protected:
	virtual void SetUp() {
	}
};


TEST_F(PolyPathTest, testPolyPath) {
	SimplePoly sPoly1 = SimplePoly(Units::from("ft",0),Units::from("ft",10000));
	Position p0(LatLonAlt::make(0.25,  4.7, 5000));
	Position p1(LatLonAlt::make(0.28,  4.7, 5000));
	Position p2(LatLonAlt::make(0.28,  4.9, 5000));
	Position p3(LatLonAlt::make(0.25,  4.9, 5000));
	sPoly1.add(p0);
	sPoly1.add(p1);
	sPoly1.add(p2);
	sPoly1.add(p3);
	EXPECT_TRUE(sPoly1.isConvex());
	EXPECT_EQ(4,sPoly1.size());
	Position pos = sPoly1. averagePoint();
	EXPECT_TRUE(sPoly1.validate());
	//		fpln(" %% pos = "+pos.toString12NP());
	Position center(LatLonAlt::make(0.265,  4.8, 5000));
	EXPECT_NEAR(pos.lat(),center.lat(),0.00001);
	EXPECT_NEAR(pos.lon(),center.lon(),0.00001);
	EXPECT_NEAR(pos.alt(),center.alt(),0.00001);
	EXPECT_NEAR(11236.2036,sPoly1.boundingCircleRadius(),0.0001);
	EXPECT_NEAR(11236.2036,sPoly1.maxRadius(),0.0001);
	EXPECT_NEAR(11236.2036,sPoly1.apBoundingRadius(),0.0001);
	// connect with Poly3D
	EuclideanProjection proj = Projection::createProjection(center.lla());
	Poly3D p3D = sPoly1.poly3D(proj);
	//fpln(" ## p3D = "+p3D.toString()+" proj.project(p0) = "+proj.project(p0).toString());
	EXPECT_NEAR(p3D.get2D(0).x,proj.project(p0).x,0.0001);
	EXPECT_NEAR(p3D.get2D(0).y,proj.project(p0).y,0.0001);
	EXPECT_NEAR(p3D.get2D(1).x,proj.project(p1).x,0.0001);
	EXPECT_NEAR(p3D.get2D(1).y,proj.project(p1).y,0.0001);
	EXPECT_NEAR(p3D.get2D(3).x,proj.project(p3).x,0.0001);
	EXPECT_NEAR(p3D.get2D(3).y,proj.project(p3).y,0.0001);
	EXPECT_NEAR(Units::from("ft",10000),p3D.getTop(),0.0001);
	EXPECT_NEAR(Units::from("ft",0.0),p3D.getBottom(),0.0001);
	EXPECT_NEAR(0.0,proj.project(sPoly1.averagePoint()).x,0.0001);
	EXPECT_NEAR(0.0,proj.project(sPoly1.averagePoint()).y,0.0001);

	// one moving polygon
	PolyPath pp = PolyPath("Weather");
	Velocity v = Velocity::makeTrkGsVs(220,5,0.0);
	EXPECT_EQ(4,sPoly1.size());
	pp.addPolygon(sPoly1,v,0.0);
	EXPECT_EQ(1,pp.size());
	EXPECT_EQ(4,pp.getPoly(0).size());

	EXPECT_FALSE(pp.isStatic());
	EXPECT_TRUE(pp.isContinuing());
	EXPECT_TRUE(pp.getPathMode() == PolyPath::USER_VEL);
	EXPECT_NEAR(0.0,pp.getFirstTime(),0.001);
	//fpln(" $$$$$ testPolyPath: pp.getLastTime() = "+Fm2(pp.getLastTime()));
	EXPECT_TRUE(std::numeric_limits<double>::infinity() == pp.getLastTime());

	Plan own("ownShip");
	Position pA(LatLonAlt::make(0.24, 4.6, 5000.00));
	Position pB(LatLonAlt::make(0.29, 4.95, 5000.00));
	NavPoint npA(pA ,0.0);          own.addNavPoint(npA);
	NavPoint npB(pB,4180.8032);     own.addNavPoint(npB);
	SimplePoly p12 = pp.interpolate(1200.5);
	Vect3 off = Vect3(1.0,1.0,1.0);
	EXPECT_EQ(4,p12.size());
	p12.translate(off);
	EXPECT_EQ(4,p12.size());
	//fpln(" $$$$ testPolyPath:  p12 = "+p12.toString());
	//		fpln(" %% AFTER translate p12 = "+p12);
	SimplePoly sPoly2 = sPoly1.linear(v,100.0);

	EXPECT_NEAR(0.00485,sPoly2.getVertex(1).lat(),0.00001);
	EXPECT_NEAR(0.0820,sPoly2.getVertex(1).lon(),0.00001);
	EXPECT_NEAR(0.0,sPoly2.getVertex(1).alt(),0.00001);
	EXPECT_TRUE(sPoly2.validate());


	pp.setPathMode(PolyPath::MORPHING);
	EXPECT_TRUE(pp.getPathMode() == PolyPath::MORPHING);
	pp.setPathMode(PolyPath::USER_VEL);
	SimplePoly sPoly3 = SimplePoly(sPoly1);
	sPoly1.remove(0);
	EXPECT_EQ(p1.mkAlt(0.0),sPoly1.getVertex(0));
	EXPECT_EQ(3,sPoly1.size());
	sPoly1.setTop(Units::from("ft",100));
	EXPECT_NEAR(Units::from("ft",100),sPoly1.getTop(),0.00001);
	sPoly1.setBottom(Units::from("ft",200));                   // This should probably be an error
	EXPECT_NEAR(Units::from("ft",200),sPoly1.getBottom(),0.00001);
	EXPECT_EQ(p1.mkAlt(Units::from("ft",200)),sPoly1.getVertex(0));
	//		fpln(" ## det2 = "+det2);
	// other PolyPath Constructors
	PolyPath pp2 = PolyPath("Weather",sPoly1, 0.0);
	EXPECT_EQ(1,pp2.size());
	pp.setPathMode(PolyPath::USER_VEL);
	EXPECT_EQ(Velocity::ZEROV(),pp2.initialVelocity(0));
	//fpln(" $$$$$$$$$$ testPolyPath: sPoly3 = "+sPoly3.toString());
	PolyPath pp3 = PolyPath("Weather",sPoly3, v, 0.0);
	EXPECT_TRUE(pp3.getPathMode() == PolyPath::USER_VEL);
	//fpln(" $$$$$$$$$$ testPolyPath: pp3.size() = "+Fm0(pp3.size())+" v ="+v.toString()+" "+pp3.initialVelocity(0).toString());
	EXPECT_EQ(v,pp3.initialVelocity(0));
	pp3.setPathMode(PolyPath::MORPHING);
	EXPECT_EQ(Velocity::ZEROV(),pp3.initialVelocity(0));
	pp3.addPolygon(p12, 1000);
	//fpln(" $$$$$$$$$$ testPolyPath: AFTER pp3.size() = "+Fm0(pp3.size()));
	EXPECT_EQ(2,pp3.size());
	SimplePoly sPoly4 = pp3.position(1500);
	//EXPECT_EQ(sPoly4, null);
	pp3.setPathMode(PolyPath::USER_VEL);
	EXPECT_TRUE(pp3.getLastTime() == std::numeric_limits<double>::infinity() );
	sPoly4 = pp3.position(1500);
	EXPECT_TRUE(pp3.initialVelocity(1).isZero());
	EXPECT_EQ(pp3.getSegment(1500.0),1);
	EXPECT_TRUE(pp3.getPolyRef(1).equals(p12));
	//		fpln("spoly4="+sPoly4+"\n p12="+p12);
	EXPECT_TRUE(sPoly4.equals(p12));
	pp3.setVelocity(1, v);
	SimplePoly sPoly5 = p12.linear(v, 500);
	sPoly4 = pp3.position(1500);
	EXPECT_TRUE(sPoly4.equals(sPoly5));
}


TEST_F(PolyPathTest, testCopy) {

	SimplePoly sPoly0(0.0, 9448.800000000001);
	Position p0 = Position(LatLonAlt::make(43.000992, -96.260713, 0.0));     sPoly0.add(p0);
	Position p1 = Position(LatLonAlt::make(42.991993, -96.273227, 0.0));     sPoly0.add(p1);
	Position p2 = Position(LatLonAlt::make(42.991993, -96.285741, 0.0));     sPoly0.add(p2);
	Position p3 = Position(LatLonAlt::make(42.982993, -96.310769, 0.0));     sPoly0.add(p3);
	Position p4 = Position(LatLonAlt::make(42.973993, -96.323283, 0.0));     sPoly0.add(p4);
	Position p5 = Position(LatLonAlt::make(42.964993, -96.323283, 0.0));     sPoly0.add(p5);
	Position p6 = Position(LatLonAlt::make(42.955994, -96.323283, 0.0));     sPoly0.add(p6);
	Position p7 = Position(LatLonAlt::make(42.811998, -96.273227, 0.0));     sPoly0.add(p7);
	Position p8 = Position(LatLonAlt::make(42.901995, -96.235686, 0.0));     sPoly0.add(p8);

	SimplePoly sPoly1(0.0,9448.800000000001);
	Position p100 = Position(LatLonAlt::make(43.036475, -95.982256, 0.0));     sPoly1.add(p100);
	Position p101 = Position(LatLonAlt::make(43.027475, -95.994811, 0.0));     sPoly1.add(p101);
	Position p102 = Position(LatLonAlt::make(43.027475, -96.007325, 0.0));     sPoly1.add(p102);
	Position p103 = Position(LatLonAlt::make(43.018476, -96.032394, 0.0));     sPoly1.add(p103);
	Position p104 = Position(LatLonAlt::make(43.009476, -96.044949, 0.0));     sPoly1.add(p104);
	Position p105 = Position(LatLonAlt::make(43.000477, -96.044990, 0.0));     sPoly1.add(p105);
	Position p106 = Position(LatLonAlt::make(42.991477, -96.045031, 0.0));     sPoly1.add(p106);
	Position p107 = Position(LatLonAlt::make(42.982478, -96.045072, 0.0));     sPoly1.add(p107);
	Position p108 = Position(LatLonAlt::make(42.964479, -96.045154, 0.0));     sPoly1.add(p108);
	Position p109 = Position(LatLonAlt::make(42.964479, -96.057668, 0.0));     sPoly1.add(p109);
	Position p110 = Position(LatLonAlt::make(42.964479, -96.070182, 0.0));     sPoly1.add(p110);
	Position p111 = Position(LatLonAlt::make(42.964479, -96.082696, 0.0));     sPoly1.add(p111);
	Position p123 = Position(LatLonAlt::make(42.892482, -96.083024, 0.0));     sPoly1.add(p123);
	Position p124 = Position(LatLonAlt::make(42.901482, -96.070469, 0.0));     sPoly1.add(p124);
	Position p125 = Position(LatLonAlt::make(42.892482, -96.057996, 0.0));     sPoly1.add(p125);
	Position p137 = Position(LatLonAlt::make(42.865483, -95.983035, 0.0));     sPoly1.add(p137);
	Position p138 = Position(LatLonAlt::make(42.874483, -95.970480, 0.0));     sPoly1.add(p138);
	Position p139 = Position(LatLonAlt::make(42.865483, -95.958007, 0.0));     sPoly1.add(p139);
	Position p140 = Position(LatLonAlt::make(42.865483, -95.945493, 0.0));     sPoly1.add(p140);
	Position p159 = Position(LatLonAlt::make(43.000477, -95.932365, 0.0));     sPoly1.add(p159);
	Position p160 = Position(LatLonAlt::make(43.009476, -95.932324, 0.0));     sPoly1.add(p160);
	Position p161 = Position(LatLonAlt::make(43.018476, -95.932283, 0.0));     sPoly1.add(p161);

	SimplePoly sPoly2 = SimplePoly(0.0,9448.800000000001);
	Position p200 = Position(LatLonAlt::make(43.164090, -95.725685, 0.0));     sPoly2.add(p200);
	Position p201 = Position(LatLonAlt::make(43.155091, -95.738240, 0.0));     sPoly2.add(p201);
	Position p202 = Position(LatLonAlt::make(43.155091, -95.750753, 0.0));     sPoly2.add(p202);
	Position p209 = Position(LatLonAlt::make(43.092094, -95.801097, 0.0));     sPoly2.add(p209);
	Position p210 = Position(LatLonAlt::make(43.092094, -95.813611, 0.0));     sPoly2.add(p210);
	Position p211 = Position(LatLonAlt::make(43.092094, -95.826125, 0.0));     sPoly2.add(p211);
	Position p218 = Position(LatLonAlt::make(43.038096, -95.876426, 0.0));     sPoly2.add(p218);
	Position p219 = Position(LatLonAlt::make(43.029097, -95.876467, 0.0));     sPoly2.add(p219);
	Position p220 = Position(LatLonAlt::make(43.038096, -95.863912, 0.0));     sPoly2.add(p220);
	Position p221 = Position(LatLonAlt::make(43.029097, -95.851439, 0.0));     sPoly2.add(p221);
	Position p222 = Position(LatLonAlt::make(43.020097, -95.851480, 0.0));     sPoly2.add(p222);
	Position p230 = Position(LatLonAlt::make(42.975099, -95.801628, 0.0));     sPoly2.add(p230);
	Position p231 = Position(LatLonAlt::make(42.975099, -95.789114, 0.0));     sPoly2.add(p231);
	Position p232 = Position(LatLonAlt::make(42.966100, -95.776641, 0.0));     sPoly2.add(p232);
	Position p233 = Position(LatLonAlt::make(42.984099, -95.764046, 0.0));     sPoly2.add(p233);
	Position p234 = Position(LatLonAlt::make(42.993098, -95.751491, 0.0));     sPoly2.add(p234);
	Position p235 = Position(LatLonAlt::make(42.984099, -95.739018, 0.0));     sPoly2.add(p235);
	Position p236 = Position(LatLonAlt::make(42.975099, -95.739059, 0.0));     sPoly2.add(p236);
	Position p252 = Position(LatLonAlt::make(43.056095, -95.688635, 0.0));     sPoly2.add(p252);
	Position p253 = Position(LatLonAlt::make(43.065095, -95.701108, 0.0));     sPoly2.add(p253);
	Position p254 = Position(LatLonAlt::make(43.074095, -95.701067, 0.0));     sPoly2.add(p254);
	Position p260 = Position(LatLonAlt::make(43.137091, -95.675752, 0.0));     sPoly2.add(p260);
	Position p261 = Position(LatLonAlt::make(43.146091, -95.675711, 0.0));     sPoly2.add(p261);
	Position p262 = Position(LatLonAlt::make(43.164090, -95.700657, 0.0));     sPoly2.add(p262);
	Position p263 = Position(LatLonAlt::make(43.155091, -95.713212, 0.0));     sPoly2.add(p263);
	PolyPath pp1 = PolyPath("PP1");
	Velocity v0  = Velocity::mkTrkGsVs(0.96, 15.27, 0.0);
	Velocity v1  = Velocity::mkTrkGsVs(0.96, 15.27, 0.0);
	Velocity v2  = Velocity::mkTrkGsVs(0.0, 0.0, 0.0);
	pp1.addPolygon(sPoly0,v0,600.0);
	pp1.addPolygon(sPoly1,v1,1500.0);
	pp1.addPolygon(sPoly2,v2,3127.9049);
	//pp1.setPathMode(PolyPath::MORPHING);

	PolyPath ppNew = pp1.copy();
	//DebugSupport::dumpPolyPath(pp,"testReRouteWx2_pp3");
	EXPECT_EQ(ppNew.size(),pp1.size());          // was 5
	//DebugSupport::dumpPlan(nPlan, "testReRouteWx2");
	for (int j = 0; j < pp1.size(); j++) {
		EXPECT_EQ(pp1.getPoly(j).size(), ppNew.getPoly(j).size());
		for (int k = 0; k < pp1.size(); k++) {
			EXPECT_NEAR(pp1.getPoly(j).getVertex(k).lat(),ppNew.getPoly(j).getVertex(k).lat(),0.000000001);
			EXPECT_NEAR(pp1.getPoly(j).getVertex(k).lon(),ppNew.getPoly(j).getVertex(k).lon(),0.000000001);
			EXPECT_NEAR(pp1.getPoly(j).getVertex(k).alt(),ppNew.getPoly(j).getVertex(k).alt(),0.000000001);
		}
	}
}




	TEST_F(PolyPathTest, test_getSegment) {
		SimplePoly sPoly1 = SimplePoly(Units::from("ft",0),Units::from("ft",10000));
		Position p0  = Position(LatLonAlt::make(0.25,  4.7, 5000));
		Position p1  = Position(LatLonAlt::make(0.28,  4.7, 5000));
		Position p2  = Position(LatLonAlt::make(0.28,  4.9, 5000));
		Position p3  = Position(LatLonAlt::make(0.25,  4.9, 5000));
		sPoly1.add(p0);
		sPoly1.add(p1);
		sPoly1.add(p2);
		sPoly1.add(p3);

		SimplePoly sPoly2 =  SimplePoly(Units::from("ft",0),Units::from("ft",10000));
		Position p4  =  Position(LatLonAlt::make(0.35,  4.7, 5000));
		Position p5  =  Position(LatLonAlt::make(0.38,  4.7, 5000));
		Position p6  =  Position(LatLonAlt::make(0.38,  4.9, 5000));
		Position p7  =  Position(LatLonAlt::make(0.35,  4.9, 5000));
		sPoly1.add(p4);
		sPoly1.add(p5);
		sPoly1.add(p6);
		sPoly1.add(p7);

		PolyPath pp = PolyPath("test");
		pp.addPolygon(sPoly1,  100);
		pp.setPathMode(PolyPath::AVG_VEL);

		EXPECT_EQ(0, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(1000.0));
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_TRUE(pp.isStatic());

		pp.setPathMode(PolyPath::MORPHING);
		EXPECT_EQ(0, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(1000.0));
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_TRUE(pp.isStatic());

		pp.setPathMode(PolyPath::USER_VEL);
		EXPECT_EQ(-1, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(1000.0));
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());

		pp.setPathMode(PolyPath::USER_VEL_FINITE);
		EXPECT_EQ(-1, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(1000.0));
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());

		pp.setPathMode(PolyPath::USER_VEL_EVER);
		EXPECT_EQ(0, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(1000.0));
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());

		pp.setPathMode(PolyPath::AVG_VEL);
		pp.addPolygon(sPoly2,  200);

		EXPECT_EQ(PolyPath::AVG_VEL, pp.getPathMode());
		EXPECT_FALSE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());
		EXPECT_EQ(-1, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(150.0));
		EXPECT_EQ(1, pp.getSegment(200.0));
		EXPECT_EQ(-1, pp.getSegment(1000.0));

		pp.setPathMode(PolyPath::MORPHING);
		EXPECT_FALSE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());
		EXPECT_EQ(-1, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(150.0));
		EXPECT_EQ(1, pp.getSegment(200.0));
		EXPECT_EQ(-1, pp.getSegment(1000.0));

		pp.setPathMode(PolyPath::USER_VEL);
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());
		EXPECT_EQ(-1, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(150.0));
		EXPECT_EQ(1, pp.getSegment(200.0));
		EXPECT_EQ(1, pp.getSegment(1000.0));

		pp.setPathMode(PolyPath::USER_VEL_FINITE);
		EXPECT_FALSE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());
		EXPECT_EQ(-1, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(150.0));
		EXPECT_EQ(1, pp.getSegment(200.0));
		EXPECT_EQ(-1, pp.getSegment(1000.0));

		pp.setPathMode(PolyPath::USER_VEL_EVER);
		EXPECT_TRUE(pp.isContinuing());
		EXPECT_FALSE(pp.isStatic());
		EXPECT_EQ(0, pp.getSegment(0.0));
		EXPECT_EQ(0, pp.getSegment(150.0));
		EXPECT_EQ(1, pp.getSegment(200.0));
		EXPECT_EQ(1, pp.getSegment(1000.0));
	}




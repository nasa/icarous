/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Vect4.h"
#include "Units.h"
#include "Route.h"
#include "PlanUtil.h"
#include "TrajGen.h"
#include <cmath>
#include <gtest/gtest.h>
#include "format.h"

using namespace larcfm;

class RouteTest : public ::testing::Test {

public:

protected:
	virtual void SetUp() {
		//lpc.clear();
	}
};



TEST_F(RouteTest, rt_test0) {
	Plan lpc("Test0");
	Position p1(LatLonAlt::make(-0.851365, 1.413133, 12800.000000));
	Position p2(LatLonAlt::make(-1.025561, 1.707843, 12800.000000));
	Position p3(LatLonAlt::make(-1.562750, 1.842057, 12800.000000));
	double startTime = 1304;
	double gs = Units::from("kn",577);
	NavPoint np1(p1,startTime);
	np1 = np1.makeName("P1");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1677);
	np2 = np2.makeName("P2");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1937);
	np3 = np3.makeName("P3");    	 lpc.addNavPoint(np3);
	lpc = PlanUtil::mkGsConstant(lpc,gs);
	//lpc = PlanUtil::makeSourceNew(lpc);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	Route pp = Route(lpc);
	EXPECT_EQ(lpc.size(), pp.size());
	Plan rpc = pp.linearPlan(startTime,gs);
	//fpln(" $$ rpc = "+rpc.toStringGs());
	EXPECT_TRUE(lpc.almostEquals(rpc,0.000000000001,0.000000000001));
	for (int i = 0; i < rpc.size(); i++) {
		Velocity vel = lpc.initialVelocity(i);
		EXPECT_NEAR(gs,vel.gs(),0.000001);
	}
	int ix = pp.findName("P1");
	EXPECT_EQ(0,ix);
	ix = pp.findName("P2");
	EXPECT_EQ(1,ix);
	ix = pp.findName("P0");
	EXPECT_EQ(-1,ix);
	pp.setName(1,"FOO");
	pp.setName(2,"FOO");
	ix = pp.findName("FOO");
	EXPECT_EQ(1,ix);
	Position pNew(LatLonAlt::make(-0.9, 1.7, 34000.0));
	pp.setPosition(1,pNew);
	EXPECT_EQ(pNew,pp.position(1));
	pp.setName(2,"NEW2");
	EXPECT_EQ("NEW2",pp.name(2));
	//fpln(" $$ pp = "+pp);
}


TEST_F(RouteTest, rt_test01) {
	Route pp = Route();
	Position p0(LatLonAlt::make(-0.80, 1.40, 22800.000000));
	Position p1(LatLonAlt::make(-1.00, 1.70, 22800.000000));
	Position p2(LatLonAlt::make(-1.50, 1.80, 22800.000000));
	pp.add(p0,"Point0","");
	pp.add(p1,"Point1","");
	pp.add(p2,"Point2","");
	EXPECT_EQ(3,pp.size());
	//fpln(" $$ pp = "+pp.toString());
	EXPECT_EQ(p0,pp.position(0));
	EXPECT_EQ(p1,pp.position(1));
	EXPECT_EQ(p2,pp.position(2));
	EXPECT_EQ("Point0",pp.name(0));
	EXPECT_EQ("Point1",pp.name(1));
	EXPECT_EQ("Point2",pp.name(2));
	EXPECT_EQ("<INVALID>",pp.name(3));
	double startTime = 1000;
	double gs = Units::from("kn",501);
	Plan rpc = pp.linearPlan(startTime,gs);
	EXPECT_EQ(p0,rpc.point(0).position());
	EXPECT_EQ(p1,rpc.point(1).position());
	EXPECT_EQ(p2,rpc.point(2).position());
	EXPECT_EQ("Point0",rpc.point(0).name());
	EXPECT_EQ("Point1",rpc.point(1).name());
	EXPECT_EQ("Point2",rpc.point(2).name());
	for (int i = 0; i < rpc.size(); i++) {
		//assertEquals(np.time(), pp.   ,0.000000001);
		EXPECT_EQ(rpc.point(i).position(),pp.position(i));
	}
	//fpln(" $$ rpc = "+rpc.toStringGs());
}


TEST_F(RouteTest, rt_test2) {
	Plan lpc("Test2");
	Position p1(LatLonAlt::make(-0.85, 1.41, 33400.000000));
	Position p2(LatLonAlt::make(-1.02, 1.70, 33500.000000));
	Position p3(LatLonAlt::make(-1.56, 1.84, 33700.000000));
	Position p4(LatLonAlt::make(-1.96, 1.84, 33700.000000));
	double startTime = 1304;
	NavPoint np1(p1,startTime);
	np1 = np1.makeName("P1");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1677);
	np2 = np2.makeName("P2");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1937);
	np3 = np3.makeName("P3");    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,2115);
	np4 = np4.makeName("P4");    	 lpc.addNavPoint(np4);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	Route pp = Route(lpc);
	//fpln(" $$ pp = "+pp.toString());
	for (int i = 0; i < lpc.size(); i++) {
		EXPECT_EQ(lpc.point(i).name(), pp.name(i));
	}
	Route ppNew = Route();
	ppNew.add(pp,0);
	EXPECT_EQ(1,ppNew.size());
	EXPECT_EQ(lpc.point(0).position(),ppNew.position(0));
	EXPECT_EQ(lpc.point(0).name(),ppNew.name(0));
	//fpln(" $$ pp= "+ppNew.toString());
	ppNew.add(pp,2);
	EXPECT_EQ(2,ppNew.size());
	EXPECT_EQ(lpc.point(2).position(),ppNew.position(1));
	EXPECT_EQ(lpc.point(2).name(),ppNew.name(1));
	//fpln(" $$ pp= "+ppNew.toString());
	Route ppFront = Route(lpc,0,1);
	//fpln(" $$ ppFront = "+ppFront.toString());
	EXPECT_EQ(2,ppFront.size());
	Route ppEnd  = Route(lpc,2,lpc.size()-1);
	//fpln(" $$ ppEnd = "+ppEnd.toString());
	EXPECT_EQ(2,ppEnd.size());
	Route ppMerge = ppFront.append(ppEnd);
	//fpln(" $$ ppMerge = "+ppMerge.toString());
	for (int i = 0; i < ppMerge.size(); i++) {                // Unchanged
		EXPECT_EQ(pp.position(i),ppMerge.position(i));
		EXPECT_EQ(pp.name(i),ppMerge.name(i));
		EXPECT_NEAR(pp.radius(i),ppMerge.radius(i),0.0001);
	}
	EXPECT_TRUE(ppMerge.equals(pp));
}

TEST_F(RouteTest, rt_mkRoute) {
	Plan lpc("Test2");
	Position p1(LatLonAlt::make(-0.85, 1.41, 33400.000000));
	Position p2(LatLonAlt::make(-1.02, 1.70, 33500.000000));
	Position p3(LatLonAlt::make(-1.56, 1.84, 33700.000000));
	Position p4(LatLonAlt::make(-1.96, 1.84, 33700.000000));
	double startTime = 100;
	NavPoint np1(p1,startTime);
	np1 = np1.makeName("P1");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1677);
	np2 = np2.makeName("P2");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1937);
	np3 = np3.makeName("P3");    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,2115);
	np4 = np4.makeName("P4");    	 lpc.addNavPoint(np4);
	double radius = Units::from("NM",1.111);
	Route rt = Route::mkRoute(lpc,radius);
	//fpln(" $$ rt = "+rt);
	for (int j = 0; j < rt.size(); j++) {
		EXPECT_EQ(rt.position(j), lpc.point(j).position());
		EXPECT_NEAR(radius,rt.radius(j),0.00000000000001);
	}
	double bankAngle = Units::from("deg",22);
	Route rt2 = Route::mkRouteBankAngle(lpc,bankAngle);
	//fpln(" $$ rt2 = "+rt2);
	for (int j = 0; j < rt2.size(); j++) {
		EXPECT_EQ(rt2.position(j), lpc.point(j).position());
		if (j != 0 && j != rt2.size()-1) {
			EXPECT_TRUE(rt2.radius(j) > 100);
		}
	}
}


TEST_F(RouteTest, testPositionFromDistance) {
	Route r = Route();
	Position p0(LatLonAlt::make(0, -10.0, 22800.000000));
	Position p1(LatLonAlt::make(0, 0, 22800.000000));
	Position p2(LatLonAlt::make(10, 0, 22800.000000));
	r.add(p0,"Point0","");
	double radius = Units::from("NM",60.0);
	r.add(p1,"Point1","",radius);
	r.add(p2,"Point2","");

	//
	// Linear
	//

	Position p;
	p = r.positionFromDistance(0,true);
	EXPECT_NEAR(p0.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p0.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p0.alt(),p.alt(),0.00000001);

	double dist;
	dist = r.pathDistance(0,2,true);
	p = r.positionFromDistance(dist,true);
	EXPECT_NEAR(p2.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p2.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p2.alt(),p.alt(),0.00000001);

	dist = r.pathDistance(0,1,true);
	p = r.positionFromDistance(dist,true);
	EXPECT_NEAR(p1.latitude(),p.latitude(),0.00000001);
	EXPECT_NEAR(p1.longitude(),p.longitude(),0.00000001);
	EXPECT_NEAR(p1.alt(),p.alt(),0.00000001);

	dist = r.pathDistance(1,2,true);
	p = r.positionFromDistance(1,dist,true);
	EXPECT_NEAR(p2.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p2.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p2.alt(),p.alt(),0.00000001);

	//
	// Curved (linear = false)
	//

	dist = r.pathDistance(0,2,false);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(p2.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p2.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p2.alt(),p.alt(),0.00000001);

	//dist = r.pathDistance(0,1,false);
	//p = r.positionFromDistance(dist,false);
	//EXPECT_NEAR(p1.latitude(),p.latitude(),0.001);
	//EXPECT_NEAR(p1.longitude(),p.longitude(),0.001);
	//EXPECT_NEAR(p1.altitude(),p.altitude(),0.001);

	dist = r.pathDistance(1,2,false);
	p = r.positionFromDistance(1,dist,false);
	EXPECT_NEAR(p2.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p2.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p2.alt(),p.alt(),0.00000001);

	//
	// Curved Distances
	//

	dist = Units::from("NM", 0.0);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(p0.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p0.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p0.alt(),p.alt(),0.00000001);

	dist = Units::from("NM", 60.0);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(0.0,p.latitude(),0.00000001);
	EXPECT_NEAR(-9.0,p.longitude(),0.00000001);
	EXPECT_NEAR(22800.0,p.altitude(),0.00000001);

	dist = Units::from("NM", 2*60.0);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(0.0,p.latitude(),0.00000001);
	EXPECT_NEAR(-8.0,p.longitude(),0.00000001);
	EXPECT_NEAR(22800.0,p.altitude(),0.00000001);

	dist = Units::from("NM", 9*60.0 + M_PI/2*60.0);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(1.0,p.latitude(),0.001);
	EXPECT_NEAR(0.0,p.longitude(),0.001);
	EXPECT_NEAR(22800.0,p.altitude(),0.00000001);

	dist = Units::from("NM", 9*60.0 + M_PI/2*60.0 + 60.0);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(2.0,p.latitude(),0.001);
	EXPECT_NEAR(0.0,p.longitude(),0.001);
	EXPECT_NEAR(22800.0,p.altitude(),0.00000001);

	dist = Units::from("NM", 10*60.0);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(0.4597,p.latitude(),0.0001);
	EXPECT_NEAR(-0.1585,p.longitude(),0.0001);
	EXPECT_NEAR(22800.0,p.altitude(),0.00000001);

}


TEST_F(RouteTest, testpositionFromDistance2) {
	Route r = Route();
	Position p0(LatLonAlt::make(0, -10.0, 22800.000000));
	Position p1(LatLonAlt::make(0, 0, 22800.000000));
	Position p2(LatLonAlt::make(10, 0, 22800.000000));
	Position p3(LatLonAlt::make(10, 10, 22800.000000));
	r.add(p0,"Point0","");
	double radius = Units::from("NM",60.0);
	r.add(p1,"Point1","",radius);
	r.add(p2,"Point2","");
	r.add(p3,"Point3","");

	//
	// Curved (linear = false)
	//

	Position p;
	double dist;
	dist = r.pathDistance(0,3,false);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(p3.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p3.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p3.alt(),p.alt(),0.00000001);

	dist = r.pathDistance(0,3,false);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(p3.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p3.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p3.alt(),p.alt(),0.00000001);

	dist = r.pathDistance(0,3,false);
	p = r.positionFromDistance(dist,false);
	EXPECT_NEAR(p3.lat(),p.lat(),0.00000001);
	EXPECT_NEAR(p3.lon(),p.lon(),0.00000001);
	EXPECT_NEAR(p3.alt(),p.alt(),0.00000001);
}



TEST_F(RouteTest, testPathDistance) {
	Route pp = Route();
	Position p0(LatLonAlt::make(0, -10.0, 22800.000000));
	Position p1(LatLonAlt::make(0, 0, 22800.000000));
	Position p2(LatLonAlt::make(10, 0, 22800.000000));
	pp.add(p0,"Point0","");
	double radius = Units::from("NM",60.0);
	pp.add(p1,"Point1","",radius);
	pp.add(p2,"Point2","");
	//Plan linPlan = pp.linearPlan(0.0, 200);
	EXPECT_EQ(3,pp.size());
	bool linear = true;
	EXPECT_NEAR(Units::from("NM",1200),pp.pathDistance(0,pp.size()-1,linear),0.00001);
	linear = false;
	EXPECT_NEAR(18*60+M_PI/2*60,Units::to("NM",pp.pathDistance(0,pp.size()-1,linear)),0.01);
	EXPECT_NEAR(9*60+M_PI/4*60,Units::to("NM",pp.pathDistance(0,1,linear)),0.01);
	EXPECT_NEAR(9*60+M_PI/4*60,Units::to("NM",pp.pathDistance(1,2,linear)),0.01);
	EXPECT_NEAR(18*60+M_PI/2*60,Units::to("NM",pp.pathDistance(0,2,linear)),0.01);
	pp = Route();
	p0 = Position(LatLonAlt::make(0, -10.0, 22800.000000));
	Position p0b(LatLonAlt::make(0, -9.0, 22800.000000));
	Position p0c(LatLonAlt::make(0, -0.5, 22800.000000));
	p1 = Position(LatLonAlt::make(0, 0, 22800.000000));
	p2 = Position(LatLonAlt::make(10, 0, 22800.000000));
	pp.add(p0,"Point0","");
	pp.add(p0b,"Point0b","");
	pp.add(p0c,"Point0c","");
	radius = Units::from("NM",60.0);
	pp.add(p1,"Point1","",radius);
	pp.add(p2,"Point2","");
	Plan linPlan = pp.linearPlan(0.0, 200);
	//fpln(" ## testPathDistance: linPlan = "+linPlan.toString());
	//DebugSupport.dumpPlan(linPlan,"rt_testPathDistance");
	EXPECT_EQ(5,pp.size());
	linear = true;
	EXPECT_NEAR(Units::from("NM",1200),pp.pathDistance(0,pp.size()-1,linear),0.00001);
	linear = false;
	//fpln("00 ------------------------------------------------------------------------");
	EXPECT_NEAR(18*60+M_PI/2*60,Units::to("NM",pp.pathDistance(0,pp.size()-1,linear)),0.01);    // TODO:
	//fpln("11 ------------------------------------------------------------------------");
	EXPECT_NEAR(60,Units::to("NM", pp.pathDistance(0,1,linear)),0.01);
	EXPECT_NEAR(9*60+M_PI/4*60,Units::to("NM",pp.pathDistance(0,3,linear)),0.01);
	//fpln(" --------------------------------------");
	double dist = pp.pathDistance(2,3,linear);
	EXPECT_NEAR(23.564,Units::to("NM",dist),0.01);
	double lDist = linPlan.pathDistance(0,2);
	EXPECT_TRUE(lDist < pp.pathDistance(0,3,linear));
	EXPECT_NEAR(1087349, pp.pathDistance(0,3,linear),10);  // big tolerance is the curved path from BOT to point 2
	EXPECT_NEAR(9*60,Units::to("NM",pp.pathDistance(0,2,linear)),25);  // big tolerance is the curved path from BOT to point 2
}



TEST_F(RouteTest, testPathDist2) {
	Plan lpc;
	Position p0(LatLonAlt::make(2.152, 5.247, 7916.425));
	Position p1(LatLonAlt::make(0.957, 6.174, 7916.425));
	Position p2(LatLonAlt::make(-1.846, 7.193, 342.343));
	Position p3(LatLonAlt::make(-2.966, 9.240, 16144.55));
	NavPoint np0(p0,0.000000);
	np0 = np0.makeName("AA");    	    lpc.addNavPoint(np0);
	NavPoint np1(p1,695.943700);
	np1 = np1.makeName("BB");    	     lpc.addNavPoint(np1);
	NavPoint np2(p2,2378.534100);
	np2 = np2.makeName("CC");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3525.191700);
	np3 = np3.makeName("DD");    	 lpc.addNavPoint(np3);
	double bankAngle = Units::from("deg", 20);
	//double gsAccel = 4;
	//double vsAccel = 1;
	//fpln(" $$$$ lpc = "+lpc.toStringGs());
	//Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false);
	Plan kpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
	//fpln(" TEST: kpc = "+kpc);
	//fpln(" 0 --------------------------------------------------------------------------- ");
	//DebugSupport.dumpPlan(kpc, "test5_kpc");
	if (kpc.hasError()) {
		//fpln(" $$$$ test5. msg ="+kpc.getMessageNoClear());
	}
	Route rt = Route::mkRouteBankAngle(lpc,bankAngle);
	//fpln(" 1 --------------------------------------------------------------------------- ");
	//fpln(" $$$ rt = "+rt.toString());
	EXPECT_EQ(rt.position(0), lpc.point(0).position());
	EXPECT_EQ(rt.position(1), lpc.point(1).position());
	EXPECT_EQ(rt.position(2), lpc.point(2).position());
	EXPECT_EQ(rt.position(3), lpc.point(3).position());
	bool linear = false;
	double pathDist_rt = rt.pathDistance(0,rt.size()-1,linear);
	//fpln(" 2 --------------------------------------------------------------------------- ");
	//fpln(" $$$$ pathDist_rt = "+Units::str("NM",pathDist_rt));
	double pathDist_kpc_orig = kpc.pathDistance();
	//fpln(" 3 --------------------------------------------------------------------------- ");
	EXPECT_NEAR(pathDist_rt,pathDist_kpc_orig,0.0001);
	//fpln(" 4 --------------------------------------------------------------------------- ");
	EXPECT_FALSE(kpc.hasError());
	//DebugSupport.dumpPlan(lpc, "pathDist2_lpc");
	Route rt0 = Route(lpc);
	//fpln(" $$$ BEFORE rt0 = "+rt0.toString());
	double pathDist_rt0 = rt0.pathDistance(0,rt0.size()-1,linear);
	double linDistance = lpc.pathDistance(0, lpc.size(), true);
	//fpln("\n $$$$################### pathDist_rt0 = "+Units::str("NM",pathDist_rt0,6));
	//fpln(" $$$$ linDistance = "+Units::str("NM",linDistance));
	EXPECT_NEAR(linDistance,pathDist_rt0,0.0001);
	rt0.setRadius(1,Units::from("NM",100));
	//fpln(" $$$ AFTER rt0 = "+rt0.toString());
	double pathDist_rt1 = rt0.pathDistance(0,rt0.size()-1,linear);
	//fpln(" $$$$#################### pathDist_rt1 = "+Units::str("NM",pathDist_rt1,6));
	rt0.setRadius(1,Units::from("NM",115));
	double pathDist_rt2 = rt0.pathDistance(0,rt0.size()-1,linear);
	//fpln(" $$$$#################### pathDist_rt2 = "+Units::str("NM",pathDist_rt2,6));
	EXPECT_NEAR(Units::from("NM",409.582165),pathDist_rt0,0.01);
	EXPECT_NEAR(Units::from("NM",409.334206),pathDist_rt1,0.5);
	EXPECT_NEAR(Units::from("NM",409.298690),pathDist_rt2,1.0);
}


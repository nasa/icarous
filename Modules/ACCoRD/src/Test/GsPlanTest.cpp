/*
 * Copyright (c) 2017-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Vect4.h"
#include "Units.h"
#include "GsPlan.h"
#include "PlanUtil.h"
#include "TrajGen.h"
#include <gtest/gtest.h>
#include "format.h"
#include <cmath>

using namespace larcfm;

class GsPlanTest : public ::testing::Test {

public:

protected:
	virtual void SetUp() {
		//lpc.clear();
	}
};



TEST_F(GsPlanTest, test0) {
	Plan lpc("Test0");
	GsPlan pp0 = GsPlan(lpc);
	Position p0(LatLonAlt::make(-0.8, 1.4, 42200.0));
	pp0.add(p0,"P0","");  // no ground speed
	EXPECT_EQ(1,pp0.size());
	EXPECT_EQ(p0,pp0.position(0));
	EXPECT_EQ("P0",pp0.name(0));
	EXPECT_NEAR(-1,pp0.gs(0),0.0000001);
	pp0.setGs(0,400.0);
	EXPECT_NEAR(400.0,pp0.gs(0),0.0000001);
	pp0.remove(0);
	EXPECT_EQ(0,pp0.size());
	Plan emp = pp0.linearPlan();
	EXPECT_EQ(0,emp.size());
	Position p1(LatLonAlt::make(-0.851365, 1.413133, 12800.000000));
	Position p2(LatLonAlt::make(-1.025561, 1.707843, 12800.000000));
	Position p3(LatLonAlt::make(-1.562750, 1.842057, 12800.000000));
	double startTime = 1304;
	double gs = Units::from("kn",577);
	NavPoint np1(p1,startTime, "P1");   	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1677, "P2");    		 lpc.addNavPoint(np2);
	NavPoint np3(p3,1937, "P3");    		 lpc.addNavPoint(np3);
	lpc = PlanUtil::mkGsConstant(lpc,gs);
	//lpc = PlanUtil::makeSourceNew(lpc);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	GsPlan pp = GsPlan(lpc);
	EXPECT_EQ(lpc.size(), pp.size());
	Plan rpc = pp.linearPlan();
	//fpln(" $$ rpc = "+rpc.toStringGs());
	EXPECT_TRUE(lpc.almostEquals(rpc,0.000000000001,0.000000000001));
	for (int i = 0; i < rpc.size(); i++) {
		Velocity vel = lpc.initialVelocity(i);
		EXPECT_NEAR(gs,vel.gs(),0.000001);
	}
	EXPECT_TRUE(pp.equals(pp));
}



TEST_F(GsPlanTest, test1) {
	double startTime = 1000;
	GsPlan pp = GsPlan(1000);
	Position p0(LatLonAlt::make(-0.80, 1.40, 22800.000000));
	Position p1(LatLonAlt::make(-1.00, 1.70, 22800.000000));
	Position p2(LatLonAlt::make(-1.50, 1.80, 22800.000000));
	double gs0 = Units::from("kn",400);
	double gs1 = Units::from("kn",450);
	pp.add(p0,"Point0","",gs0);
	pp.add(p1,"Point1","",gs1);
	pp.add(p2,"Point2","",gs1);
	EXPECT_EQ(3,pp.size());
	//fpln(" $$ pp = "+pp.toString());
	EXPECT_EQ(p0,pp.position(0));
	EXPECT_EQ(p1,pp.position(1));
	EXPECT_EQ(p2,pp.position(2));
	EXPECT_EQ("Point0",pp.name(0));
	EXPECT_EQ("Point1",pp.name(1));
	EXPECT_EQ("Point2",pp.name(2));
	EXPECT_EQ("<INVALID>",pp.name(3));
	EXPECT_NEAR(gs0,pp.gs(0),0.000001);
	EXPECT_NEAR(gs1,pp.gs(1),0.000001);
	EXPECT_NEAR(gs1,pp.gs(2),0.000001);
	Plan rpc = pp.linearPlan();
	//fpln(" $$ rpc = "+rpc.toStringGs());
	EXPECT_EQ(p0,rpc.point(0).position());
	EXPECT_EQ(p1,rpc.point(1).position());
	EXPECT_EQ(p2,rpc.point(2).position());
	EXPECT_EQ("Point0",rpc.point(0).name());
	EXPECT_EQ("Point1",rpc.point(1).name());
	EXPECT_EQ("Point2",rpc.point(2).name());
	EXPECT_NEAR(gs0,rpc.initialVelocity(0).gs(),0.000001);
	EXPECT_NEAR(gs1,rpc.initialVelocity(1).gs(),0.000001);
	EXPECT_NEAR(gs1,rpc.initialVelocity(2).gs(),0.000001);
	for (int i = 0; i < rpc.size(); i++) {
		NavPoint np = rpc.point(i);
		//EXPECT_NEAR(np.time(),np.sourceTime(),0.000000001);
		EXPECT_EQ(np.position(),pp.position(i));
	}
	double gs = Units::from("kn",501);
	Plan rpc2 = pp.route().linearPlan(startTime,gs);
	//fpln(" $$ rpc2 = "+rpc2.toStringGs());
	for (int i = 0; i < rpc2.size(); i++) {
		Velocity vel = rpc2.initialVelocity(i);
		EXPECT_NEAR(gs,vel.gs(),0.000001);
	}
}


TEST_F(GsPlanTest, test2) {
	Plan lpc("Test0");
	Position p1(LatLonAlt::make(-0.85, 1.41, 33400.000000));
	Position p2(LatLonAlt::make(-1.02, 1.70, 33500.000000));
	Position p3(LatLonAlt::make(-1.56, 1.84, 33700.000000));
	Position p4(LatLonAlt::make(-1.96, 1.84, 33700.000000));
	double startTime = 1304;
	NavPoint np1(p1,startTime, "P1");     lpc.addNavPoint(np1);
	NavPoint np2(p2,1677, "P2");    	 	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1937, "P3");	    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,2115, "P4"); 	   	 lpc.addNavPoint(np4);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	GsPlan pp = GsPlan(lpc);
	//fpln(" $$ pp = "+pp.toString());
	for (int i = 0; i < lpc.size(); i++) {
		Velocity vel = lpc.initialVelocity(i);
		EXPECT_NEAR(vel.gs(),pp.gs(i),0.000001);
	}
	GsPlan ppNew = GsPlan(lpc.getFirstTime());
	ppNew.add(pp,0);
	EXPECT_EQ(1,ppNew.size());
	EXPECT_EQ(lpc.point(0).position(),ppNew.position(0));
	EXPECT_EQ(lpc.point(0).name(),ppNew.name(0));
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),ppNew.gs(0),0.0000001);
	//fpln(" $$ pp= "+ppNew.toString());
	ppNew.add(pp,2);
	EXPECT_EQ(2,ppNew.size());
	EXPECT_EQ(lpc.point(2).position(),ppNew.position(1));
	EXPECT_EQ(lpc.point(2).name(),ppNew.name(1));
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),ppNew.gs(1),0.0000001);
	//fpln(" $$ pp= "+ppNew.toString());
	GsPlan ppFront = GsPlan(lpc,0,1);
	EXPECT_EQ(2,ppFront.size());
	GsPlan ppEnd  = GsPlan(lpc,2,lpc.size()-1);
	EXPECT_EQ(2,ppEnd.size());
	GsPlan ppMerge = ppFront.append(ppEnd);
	EXPECT_NEAR(pp.startTime(),ppMerge.startTime(),0.000000001);
	for (int i = 0; i < ppMerge.size(); i++) {                // Unchanged
		EXPECT_EQ(pp.position(i),ppMerge.position(i));
		EXPECT_EQ(pp.name(i),ppMerge.name(i));
		EXPECT_NEAR(pp.gs(i),ppMerge.gs(i),0.000000001);
	}
	EXPECT_TRUE(ppMerge.equals(pp));
}



TEST_F(GsPlanTest, test_pathDistance) {
	Plan lpc;
	Position p0(LatLonAlt::make(2.152, 5.247, 7916.425));
	Position p1(LatLonAlt::make(0.957, 6.174, 7916.425));
	Position p2(LatLonAlt::make(-1.846, 7.193, 342.343));
	Position p3(LatLonAlt::make(-2.966, 9.240, 16144.55));
	NavPoint np0(p0,0.000000, "AA");    	  lpc.addNavPoint(np0);
	NavPoint np1(p1,695.943700, "BB");        lpc.addNavPoint(np1);
	NavPoint np2(p2,2378.534100, "CC");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3525.191700, "DD");    	 lpc.addNavPoint(np3);
	double bankAngle = Units::from("deg", 25);
	//double gsAccel = 4;
	//double vsAccel = 1;
	//fpln(" $$$$ lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "test5_lpc");
	//Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false);
	Plan kpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
	//fpln(" TEST: kpc = "+kpc);
	//fpln(" 0 --------------------------------------------------------------------------- ");
	//DebugSupport.dumpPlan(kpc, "test5_kpc");
	if (kpc.hasError()) {
		//fpln(" $$$$ test5. msg ="+kpc.getMessageNoClear());
	}
	GsPlan gsp = GsPlan::mkGsPlanBankAngle(lpc,bankAngle);
	//fpln(" 1 --------------------------------------------------------------------------- ");
	//fpln(" $$$ rt = "+rt.toString());
	EXPECT_EQ(gsp.position(0), lpc.point(0).position());
	EXPECT_EQ(gsp.position(1), lpc.point(1).position());
	EXPECT_EQ(gsp.position(2), lpc.point(2).position());
	EXPECT_EQ(gsp.position(3), lpc.point(3).position());
	bool linear = false;
	double pathDist_gsp = gsp.pathDistance(0,gsp.size()-1,linear);
	//fpln(" 2 --------------------------------------------------------------------------- ");
	//fpln(" $$$$ pathDist_rt = "+Units::str("NM",pathDist_rt));
	double pathDist_kpc_orig = kpc.pathDistance();
	//fpln(" 3 --------------------------------------------------------------------------- ");
	EXPECT_NEAR(pathDist_gsp,pathDist_kpc_orig,0.00001);
	//fpln(" 4 --------------------------------------------------------------------------- ");
	EXPECT_FALSE(kpc.hasError());
}



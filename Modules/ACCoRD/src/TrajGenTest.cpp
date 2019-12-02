/*
 * Copyright (c) 2014-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "TrajTemplates.h"
#include "format.h"
#include <gtest/gtest.h>
#include "VectFuns.h"
#include "PlanUtil.h"
#include "TrajGen.h"
#include "GsPlan.h"
#include "TurnGeneration.h"
#include "DebugSupport.h"
#include "PositionUtil.h"
#include "KinematicsPosition.h"

using namespace larcfm;


class TrajGenTest : public ::testing::Test {

public:
	double gsAccel;
	double vsAccel;
	double bankAngle;
	double gs;
	double vs;
	Plan lpc;
	LatLonAlt KMSP;
	LatLonAlt KDFW;
	LatLonAlt mid;


protected:
	virtual void SetUp() {
		lpc.clear();

		gsAccel = 2;
		vsAccel = 1;
		bankAngle = Units::from("deg", 20);
		gs = Units::from("kn",  530);
		vs = Units::from("fpm", 2000);

		KMSP = LatLonAlt::make(44.878101, -93.21869, 10000.0);
		KDFW = LatLonAlt::make(32.898700, -97.04039, 10000.0);
		mid = GreatCircle::interpolate( KMSP,KDFW,0.5);

	}
};

//
//bool plans_almost_equal(const Plan& lpc, Plan kpc) {
//	bool rtn = true;
//	for (int i = 0; i < lpc.size(); i++) {                // Unchanged
//		if (!lpc.point(i).almostEqualsPosition(kpc.point(i))) rtn = false;
//	}
//	return rtn;
//}

//bool lastPointTheSame(const Plan& orig, const Plan& npc) {
//	NavPoint origLastPoint = orig.getLastPoint();
//	NavPoint finalLastPoint = npc.getLastPoint();
//	if (origLastPoint.position().almostEquals(finalLastPoint.position())) return true;
//	//fp(" $$$$ lastPointTheSame: distance = "+Units::str("ft",origLastPoint.distanceH(finalLastPoint)));
//	return false;
//}


//
//	bool plans_equal_position(PlanCore lpc, PlanCore kpc) {
//		bool rtn = true;
//		for (int i = 0; i < lpc.size(); i++) {                // Unchanged
//		    if (!lpc.point(i).position().equals(kpc.point(i).position())) rtn = false;
//		}
//        return rtn;
//	}
//
//bool NavPoint_equal_position(const NavPoint &lpc, const NavPoint& kpc) {
//	return lpc.position() == kpc.position();
//}


bool Plans_equal_gs(const Plan& lpc, const Plan& kpc) {
	bool rtn = true;
	for (int i = 0; i < lpc.size(); i++) {                // Unchanged
		double tm = lpc.point(i).time();
		int j = kpc.getSegment(tm);
		if (j >= 0) {
			//fpln("!!!! Plans_equal_gs: i,j = "+i+","+j+" tm = "+tm);
			if (std::abs(lpc.initialVelocity(i).gs() - kpc.initialVelocity(j).gs()) > Units::from("kn",1.0)) {
				//fpln("!!!! Plans_equal_gs: i,j = "+i+","+j+" "+Units::from("kn",lpc.initialVelocity(i).gs())+" != "+Units::from("kn",kpc.initialVelocity(j).gs()));
				rtn = false;
			}
		}
	}
	return rtn;
}


bool isVsConstant(const Plan& lpc) {
	int end = lpc.size()-1;
	double zStart = lpc.point(0).z();
	double zEnd = lpc.point(end).z();
	double dtot = zEnd - zStart;
	double ttot = lpc.time(end) - lpc.time(0);
	double avgVs = dtot/ttot;
	bool rtn = true;
	for (int i = 0; i < end; i++) {
		double vs_i = lpc.initialVelocity(i).vs();
		if (std::abs(vs_i - avgVs) > Units::from("fpm",1.0)) {
			rtn = false;
			fpln("..... vs_i = "+Fm2(Units::from("fpm",vs_i))+" avgVs  = "+Fm2(Units::from("fpm",avgVs)));
			break;
		}
	}
	return rtn;
}


TEST_F(TrajGenTest, testTurnGenerator) {
	Vect3 p0(0,0,0);
	Vect3 p1(10,0,0);
	Vect3 p2(30,0,0);
	EXPECT_TRUE(VectFuns::collinear(p0,p1,p2));
	EXPECT_TRUE(VectFuns::collinear(p1,p0,p2));
	EXPECT_TRUE(VectFuns::collinear(p2,p1,p0));
	p2 = Vect3(10,10,0);
	NavPoint np0(Position(p0),0);
	NavPoint np1(Position(p1),100);
	NavPoint np2(Position(p2),200);
	double R = 6;
	Tuple5<NavPoint,NavPoint,NavPoint,int, Position> tG = TurnGeneration::turnGenerator(np0, np1, np2, R);
	//fpln(" $$ tG.first = "+tG.first);
	//fpln(" $$ tG.second = "+tG.second);
	//fpln(" $$ tG.third = "+tG.third);
	EXPECT_NEAR(4.0,tG.first.x(),0.001);
	EXPECT_NEAR(0.0,tG.first.y(),0.001);
	EXPECT_NEAR(0.0,tG.first.z(),0.001);
	EXPECT_NEAR(0.0,tG.first.y(),0.001);
	EXPECT_NEAR(52.8761,tG.first.time(),0.001);
	EXPECT_NEAR(8.243,tG.second.x(),0.001);
	EXPECT_NEAR(1.757,tG.second.y(),0.001);
	EXPECT_NEAR(0.0,tG.second.z(),0.001);
	EXPECT_NEAR(100,tG.second.time(),0.001);
	EXPECT_NEAR(10.0,tG.third.x(),0.001);
	EXPECT_NEAR(6.0,tG.third.y(),0.001);
	EXPECT_NEAR(0.0,tG.third.z(),0.001);
	EXPECT_NEAR(147.123,tG.third.time(),0.001);
}

TEST_F(TrajGenTest, testGsSimple) {
	Position pKMSP(KMSP);
	Position pKDFW(KDFW);
	Position pMid(mid);
	//		NavPoint np1(pKMSP,0.0, NavPoint.WayType.Orig, NavPoint.PointMutability.MUTABLE,
	//				"", NavPoint.HorizontalTCPType.BGSC, NavPoint.VerticalTCPType.NONE, pKMSP, pKMSP);
	//		NavPoint np2(pKDFW,100.0, NavPoint.WayType.Orig, NavPoint.PointMutability.MUTABLE,
	//				"", NavPoint.HorizontalTCPType.EGSC, NavPoint.VerticalTCPType.NONE, pKDFW, pKDFW, 100.0);
	NavPoint np1(pKMSP,0.0);
	NavPoint np2(pKDFW,4000.0);
	NavPoint nMid(pMid,1600.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(nMid);
	double gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testGsSimple: gs0 = "+Units::str8("kn",gs0));
	//fpln("testGsSimple: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(gs0,Units::from("kn",832.902),0.001);
	EXPECT_NEAR(gs1,Units::from("kn",555.268),0.001);
	bool repair = true;
	double firstTime = lpc.getFirstTime();
	//fpln("testGsSimple: lpc = "+lpc.toString());
	//DebugSupport::dumpPlan(lpc, "testGsSimple_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport::dumpPlan(kpc, "testGsSimple_kpc");
	//fpln("$$$$$$$$$$$$$$$$ testGsSimple: kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.isTCP(2));
	EXPECT_TRUE(kpc.isFlyable(false));
	EXPECT_FALSE(kpc.hasMessage());
    EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));

	EXPECT_NEAR(firstTime,kpc.getFirstTime(),0.001);
	EXPECT_EQ(4,kpc.size());
	EXPECT_TRUE(kpc.inGsChange(1650.0));
	EXPECT_FALSE(kpc.inVsChange(1650.0));
	EXPECT_NEAR(428.48,kpc.initialVelocity(0).gs(),0.01);
	//EXPECT_NEAR(0.0,kpc.initialVelocity(4000).gs(),0.01);     // ??
	//EXPECT_NEAR(0.0,kpc.initialVelocity(3900).gs(),0.01);     // ??
	EXPECT_TRUE(!kpc.hasMessage());
	//pln("testGsSimple: kpc.getMessage() = "+kpc.getMessage());
	double t = 400;
	EXPECT_NEAR(0.757,kpc.position(t).lat(),0.001);
	EXPECT_NEAR(-1.636,kpc.position(t).lon(),0.001);
	EXPECT_NEAR(428.48,kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(0.676,kpc.position(1650).lat(),0.001);
	EXPECT_NEAR(-1.664,kpc.position(1650).lon(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);            // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.0001);          // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.0001);        // INVARIANT
	//fpln(" $$$$$$$$$>>> lpc.finalVelocity(2) = "+lpc.finalVelocity(2).toString()+"  lpc.finalVelocity(4) = "+lpc.finalVelocity(4).toString());
	t = 500;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	t = 1623;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	EXPECT_TRUE(kpc.inGsChange(t));
	EXPECT_EQ(1,kpc.prevBGS(kpc.getSegment(t)+1));
	EXPECT_EQ(2,kpc.nextEGS(0));
	t = 2500;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	EXPECT_EQ(1,kpc.prevBGS(kpc.getSegment(t)+1));
	EXPECT_EQ(2,kpc.nextEGS(0));
	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isEGS(2));
	//fpln("$$$ ================================= kpc.point(1).time() = "+kpc.point(1).time());
	EXPECT_TRUE(kpc.inGsChange(kpc.point(1).time()));
	EXPECT_NEAR(gsAccel,std::abs(kpc.gsAccelAtTime(kpc.point(1).time())),0.0001);
	EXPECT_NEAR(0.0,std::abs(kpc.gsAccelAtTime(kpc.point(2).time())),0.0001);           // ??
	EXPECT_NEAR(gsAccel,std::abs(kpc.gsAccelAtTime((kpc.point(1).time()+kpc.point(2).time())/2.0)),0.0001);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.finalVelocity(1).gs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).gs(),kpc.finalVelocity(2).gs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).gs(),kpc.initialVelocity(2).gs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).gs(),kpc.finalVelocity(2).gs(),0.01);
	//EXPECT_NEAR(lpc.finalVelocity(2).gs(),kpc.finalVelocity(3).gs(),0.01);
	EXPECT_TRUE(kpc.finalVelocity(4).isInvalid());      // INVARIANT
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
}



TEST_F(TrajGenTest, testGsAccel) {
	Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p2 = Position::makeXYZ(0.0, 10.0, 10000);
	Position p3 = Position::makeXYZ(0.0, 22.0, 10000);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,400.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	double gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testGsAccel: gs0 = "+Units::str8("kn",gs0));
	//fpln("testGsAccel: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(gs0,Units::from("kn",180.0),0.001);
	EXPECT_NEAR(gs1,Units::from("kn",216),0.001);
	//fpln("testGsAccel: lpc = "+lpc);
	bool repair = true;
	double firstTime = lpc.getFirstTime();
	EXPECT_NEAR(firstTime,0.0,0.0001);
	//fpln("testGsSimple: lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testGsAccel_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc, "testGsAccel_kpc");
	EXPECT_TRUE(kpc.isTCP(2));
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_FALSE(kpc.hasMessage());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//fpln("testGsAccel: kpc = "+kpc);
	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isEGS(2));
	EXPECT_NEAR(0.0,kpc.point(1).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",10.0),kpc.point(1).y(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(1).z(),0.0001);
	EXPECT_NEAR(Units::from("NM",0.0),kpc.point(2).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",10.5093),kpc.point(2).y(),0.1);                   // ERROR ??  10.5228 ??
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(2).z(),0.0001);
	EXPECT_NEAR(Units::from("NM",22.0),kpc.point(3).y(),0.0001);
}



TEST_F(TrajGenTest, testGsShortLeg) {
	//fpln(" ------------------------------ testGsShortLeg ---------------------------------");
	Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p2 = Position::makeXYZ(0.0, 10.0, 10000);
	Position p3 = Position::makeXYZ(0.0, 20.0, 10000);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,220.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	double gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testGsShortLeg: gs0 = "+Units::str8("kn",gs0));
	//fpln("testGsShortLeg: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(gs0,Units::from("kn",180.0),0.001);
	EXPECT_NEAR(gs1,Units::from("kn",1800),0.001);
	//fpln("testGsShortLeg: lpc = "+lpc);
	bool repair = false;
	double firstTime = lpc.getFirstTime();
	EXPECT_NEAR(firstTime,0.0,0.0001);
	//fpln("testGsSimple: lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testGsShortLeg_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc, "testGsShortLeg_kpc");repair
	EXPECT_TRUE(kpc.hasError());
	kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, 12*gsAccel, vsAccel, repair, repair, repair);

	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(kpc.point(0) == lpc.point(0));    // kpc = lpc
	//fpln("testGsShortLeg: "+ kpc.point(0).toOutput(true,3)+" "+lpc.point(0).toOutput(true,3));
	EXPECT_TRUE(kpc.point(1).almostEqualsPosition(lpc.point(1)));
	//EXPECT_TRUE(kpc.point(2).almostEqualsPosition(lpc.point(2)));
	// EXPECT_TRUE(kpc.hasMessage());                    // ERROR MESSAGE SHOULD BE SEEN BUT IS NOT
	//fpln("testGsShortLeg: kpc = "+kpc);
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
}




TEST_F(TrajGenTest, testTurnAround) {
	//fpln(" ------------------------------ testTurnAround ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	double legLength = Units::from("nmi",1.4);
	Position p3(Vect3(Units::from("nmi",10.0), legLength, Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,220.0);
	NavPoint np4(p4,489.07248);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	double gs0 = lpc.initialVelocity(0).gs();
	//fpln("testTurnAround: gs0 = "+Units::str8("kn",gs0));
	//fpln("testTurnAround: gs1 = "+Units::str8("kn",gs1));
	//fpln("testTurnAround: gs2 = "+Units::str8("kn",gs2));
	//fpln("testTurnAround: BEFORE lpc = "+lpc);
	lpc = (Plan) PlanUtil::mkGsConstant(lpc,Units::from("kn",180.0));    // Smooth It to Match 'testNormalLegs"
	//DebugSupport.dumpPlan(lpc, "testTurnAround_lpc");
	//fpln("testTurnAround: AFTER lpc = "+lpc);
	gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
	EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
	bool repair = true;
	//lpc.setExtend(true);
	//DebugSupport.dumpPlan(lpc, "testTurnAround_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln("testTurnAround: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testTurnAround_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,80));
	EXPECT_EQ(5,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_FALSE(kpc.isTCP(4));
	EXPECT_NEAR(Units::from("NM",7.0188),kpc.point(1).x(),0.1);
	EXPECT_NEAR(Units::from("NM",0.4913),kpc.point(1).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(1).z(),0.01);
	EXPECT_NEAR(Units::from("NM",8.1513),kpc.point(2).x(),0.1);
	EXPECT_NEAR(Units::from("NM",1.3532),kpc.point(2).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(2).z(),0.01);
	EXPECT_NEAR(Units::from("NM",7.8116),kpc.point(3).x(),0.1);
	EXPECT_NEAR(Units::from("NM",2.7352),kpc.point(3).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(3).z(),0.01);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
}





TEST_F(TrajGenTest, testGsErrors) {
	//fpln(" ------------------------------ testGsErrors ---------------------------------");
	Position p1 = Position::makeXYZ(0.0, 1.0, 10000);
	Position p2 = Position::makeXYZ(10.0, 2.0, 10000);
	Position p3 = Position::makeXYZ(20.0, 3.0, 10000);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,100.0);
	NavPoint np3(p3,200.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	double gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testGs: gs0 = "+Units::str8("kn",gs0));
	//fpln("testGs: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(gs0,Units::from("kn",361.7955),0.001);
	EXPECT_NEAR(gs1,Units::from("kn",361.7955),0.001);
	bool repair = true;
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//fpln(" $$$ kpc.getMessage() = "+kpc.getMessage());   // Why No message?
	//EXPECT_TRUE(plans_equal(lpc,kpc));
	// +++++++++++++ alter plan +++++++++++++
	lpc.remove(2);
	np3 = np3.makeTime(150);
	lpc.addNavPoint(np3);
	gs0 = lpc.initialVelocity(0).gs();
	gs1 = lpc.initialVelocity(1).gs();
	EXPECT_NEAR(gs0,Units::from("kn",361.7955),0.001);
	EXPECT_NEAR(gs1,Units::from("kn",723.591),0.001);
	//DebugSupport.dumpPlan(lpc, "testGsErrors_lpc");
	kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	EXPECT_TRUE(kpc.hasError());
	//DebugSupport.dumpPlan(kpc, "testGsErrors_kpc");
	// +++++++++++++ alter plan +++++++++++++
	lpc.remove(2);
	np3 = np3.makeTime(190);
	lpc.addNavPoint(np3);
	Position p4(Vect3(Units::from("nmi",30.0), Units::from("nmi",4.0), Units::from("ft",10000)));
	NavPoint np4(p4,300.0);
	lpc.addNavPoint(np4);
	gs0 = lpc.initialVelocity(0).gs();
	gs1 = lpc.initialVelocity(1).gs();
	lpc =  (Plan) PlanUtil::mkGsConstant(lpc,2,3, gs1);
	//fpln(" >>>>>>>>>> lpc = "+lpc);
	double gs2 = lpc.initialVelocity(2).gs();
	//fpln("testGs: gs0 = "+Units::str8("kn",gs0));
	//fpln("testGs: gs1 = "+Units::str8("kn",gs1));
	//fpln("testGs: gs2 = "+Units::str8("kn",gs2));
	EXPECT_NEAR(gs0,Units::from("kn",361.7955),0.001);
	EXPECT_NEAR(gs1,Units::from("kn",401.995),0.001);
	//DebugSupport.dumpPlan(lpc, "testGsErrors_lpc");
	kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	EXPECT_TRUE(kpc.isFlyable());
	//DebugSupport.dumpPlan(kpc, "testGsErrors_kpc");
	//fpln(" >>>>>>>>>>> kpc = "+kpc.toString());
	EXPECT_FALSE(lpc.almostEquals(kpc));
	EXPECT_NEAR(190.517,kpc.point(3).time(),0.0001);
	EXPECT_NEAR(Units::from("NM",20.0),kpc.point(3).x(),0.01);
	EXPECT_NEAR(Units::from("NM",3.0),kpc.point(3).y(),0.1);                   // ERROR ??
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(3).z(),0.0001);
	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isEGS(2));
	EXPECT_NEAR(gs2,kpc.initialVelocity(3).gs(),0.001);
}



TEST_F(TrajGenTest, testTurnAndLevelOut) {
	//fpln(" ------------------------------ testTurnAndLevelOut ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",5000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	double legLength = Units::from("nmi",2.4);
	Position p4(Vect3(Units::from("nmi",10.0), 3.5*legLength, Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,217.39);
	np2 = np2.makeName("V123");
	NavPoint np4(p4,400);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np4);
	lpc.setVsAccel(1,2.22222);
	//DebugSupport.dumpPlan(lpc, "testTurnAndLevelOut_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("testTurnAndLevelOut: kpc = "+kpc.toString());
	//DebugSupport.dumpPlan(kpc, "testTurnAndLevelOut_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_TRUE(Plans_equal_gs(lpc,kpc));
	EXPECT_NEAR(lpc.finalVelocity(0).trk(),kpc.finalVelocity(0).trk(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).trk(),kpc.initialVelocity(5).trk(),0.01);
	// make sure intermediate position and velocity are correct vertically
	EXPECT_NEAR(lpc.point(2).alt(),kpc.position(220.0).alt(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.velocity(220.0).vs(),0.01);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
	//DebugSupport.dumpPlan(noTCPS, "testTurnAndLevelOut_noTCPS");
	//EXPECT_TRUE(lpc.almostEquals(noTCPS));
	EXPECT_NEAR(-2.222,noTCPS.getVsAccel(1),0.001);
}



TEST_F(TrajGenTest, testTurnAndLevelHard) {
	//fpln(" ------------------------------ testTurnAndLevelHard ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",5000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	double legLength = Units::from("nmi",2.4);
	Position p4(Vect3(Units::from("nmi",10.0), 3.5*legLength, Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,217.39);
	NavPoint np4(p4,450);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc, "testTurnAndLevelHard_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("testTurnAndLevelOut: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testTurnAndLevelHard_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_EQ(8,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	int ixMOT = kpc.findMOT(1);
	EXPECT_TRUE(kpc.isMOT(ixMOT));
	EXPECT_TRUE(kpc.isBVS(2));
	EXPECT_NEAR(kpc.point(4).alt(),kpc.point(5).alt(),0.01);
	EXPECT_NEAR(kpc.point(5).alt(),kpc.point(6).alt(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(6).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(7).gs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(8).gs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(0).trk(),kpc.finalVelocity(0).trk(),0.01);
	// make sure intermediate position and velocity are correct vertically
	EXPECT_NEAR(lpc.point(2).alt(),kpc.position(220.0).alt(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.velocity(220.0).vs(),0.01);
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
	//DebugSupport.dumpPlan(noTCPS, "testTurnAndLevelOut_noTCPS");
	//EXPECT_TRUE(lpc.almostEquals(noTCPS));
}



TEST_F(TrajGenTest, testNormalLegsGS) {
	//fpln(" ------------------------------ testNormalLegs ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",-10.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,350.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	//double firstTime = lpc.getFirstTime();
	double gs0 = lpc.initialVelocity(0).gs();
	//double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testNormalLegs: gs0 = "+Units::str8("kn",gs0));
	//fpln("testNormalLegs: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
	//EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
	bool repair = true;
	//fpln("testNormalLegs: lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testNormalLegsGS_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln("testNormalLegs: kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//DebugSupport.dumpPlan(kpc, "testNormalLegsGS_kpc");
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	//fpln(" $$$ testNormalLegs: kpc.point(1) = "+kpc.point(1).toString(8));
	EXPECT_NEAR(0.0,kpc.point(1).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",8.70283731),kpc.point(1).y(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(1).z(),0.0001);
	EXPECT_NEAR(Units::from("NM",-0.37993015),kpc.point(2).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",9.6200698),kpc.point(2).y(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(2).z(),0.0001);
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_NEAR(lpc.getFirstTime(),kpc.getFirstTime(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.0001);             // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.0001);           // INVARIANT
	//Plan noTCPS = Plan(kpc);
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	//fpln(" ## testNormalLegsGS: lpc = "+lpc.toStringGs());
	//fpln(" ## testNormalLegsGS: kpc = "+kpc.toStringGs());
	//fpln(" ## testNormalLegsGS: noTCPS = "+noTCPS.toStringGs());
	bool trkF = true;
	bool gsF = false;
	bool vsF = true;
	noTCPS.removeIfRedundant(2,trkF, gsF, vsF);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
}



TEST_F(TrajGenTest, testRemoveShortLegsBetween) {
	//fpln(" ------------------------------ testRemoveShortLegsBetween ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	double legLength = Units::from("nmi",2.0);
	Position p3(Vect3(Units::from("nmi",10.0), legLength, Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",20.0), legLength, Units::from("ft",10000)));
	Position p5(Vect3(Units::from("nmi",25.0), 2*legLength, Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,220.0);
	NavPoint np4(p4,489);
	NavPoint np5(p5,589);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc, "removeShortLegs_BEFORE");
	//fpln("removeShortLegs: BEFORE lpc = "+lpc);
	lpc =  (Plan) PlanUtil::mkGsConstant(lpc,Units::from("kn",180.0));                                // Smooth It to Match 'testNormalLegs"
	double bankAngle = Units::from("deg",20);
	lpc = TrajGen::linearRepairShortTurnLegs(lpc,bankAngle,false);
	//fpln("removeShortLegs: AFTER: lpc = "+lpc);
	double gs0_After = lpc.initialVelocity(0).gs();
	//fpln("removeShortLegs: gs0_After = "+Units::str8("kn",gs0_After));
	EXPECT_EQ(3,lpc.size());
	EXPECT_NEAR(Units::from("kn",180.0),gs0_After,0.001);
}



TEST_F(TrajGenTest, testRemoveShortLegs2) {
	//fpln(" ------------------------------ testRemoveShortLegs2 ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",10.0), Units::from("nmi",3.0), Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",0.0), Units::from("nmi",3.0), Units::from("ft",10000)));
	Position p5(Vect3(Units::from("nmi",0.0), Units::from("nmi",7.0), Units::from("ft",10000)));
	Position p6(Vect3(Units::from("nmi",10.0), Units::from("nmi",7.0), Units::from("ft",10000)));

	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,220.0);
	NavPoint np4(p4,489);
	NavPoint np5(p5,589);
	NavPoint np6(p6,689);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	lpc.addNavPoint(np6);
	double bankAngle = Units::from("deg",20);
	lpc = TrajGen::linearRepairShortTurnLegs(lpc,bankAngle,true);
	//DebugSupport.dumpPlan(lpc, "testRemoveShortLegs2_lpc");
	double gs0_After = lpc.initialVelocity(0).gs();
	//fpln("removeShortLegs: gs0_After = "+Units::str8("kn",gs0_After));
	EXPECT_EQ(5,lpc.size());
	EXPECT_NEAR(Units::from("kn",180.0),gs0_After,0.001);
	lpc =  (Plan) PlanUtil::mkGsConstant(lpc,Units::from("kn",180));                                  // run with and without
	//DebugSupport.dumpPlan(lpc, "removeShortLegs2_gsConst");
	//lpc.setExtend(true);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,210));
}



TEST_F(TrajGenTest, testTurnClimb) {
	//fpln(" ------------------------------ testTurnClimb ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",12666.6667)));
	double legLength = Units::from("nmi",2.4);
	Position p3(Vect3(Units::from("nmi",10.0), legLength, Units::from("ft",13303.6667)));
	Position p4(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",16656.0679)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,248.0);
	NavPoint np4(p4,499.2051);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	double gs0 = lpc.initialVelocity(0).gs();
	gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
	EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
	bool repair = true;
	//DebugSupport.dumpPlan(lpc, "testTurnClimb_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln("testTurnClimb: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testTurnClimb_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_TRUE(Plans_equal_gs(lpc,kpc));
	EXPECT_EQ(8,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	int ixMOT = kpc.findMOT(1);
	EXPECT_TRUE(kpc.isMOT(ixMOT));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBOT(4));
	int ixMOT4 = kpc.findMOT(4);
	EXPECT_TRUE(kpc.isMOT(ixMOT4));
	EXPECT_TRUE(kpc.isEOT(6));
	EXPECT_FALSE(kpc.isTCP(7));
	EXPECT_NEAR(Units::from("NM",8.7028),kpc.point(1).x(),0.1);
	EXPECT_NEAR(Units::from("NM",0.0),kpc.point(1).y(),0.1);
	//EXPECT_NEAR(Units::from("ft",12320.7566),kpc.point(1).z(),0.01);
	EXPECT_NEAR(174.0567,kpc.point(1).time(),0.001);
	EXPECT_NEAR(17816.369,kpc.point(2).x(),0.1);
	EXPECT_NEAR(703.630,kpc.point(2).y(),0.1);
	//EXPECT_NEAR(Units::from("ft",12592.4337),kpc.point(2).z(),0.01);
	EXPECT_NEAR(194.4325,kpc.point(2).time(),0.001);
	EXPECT_NEAR(Units::from("NM",10.0),kpc.point(3).x(),0.1);
	EXPECT_NEAR(Units::from("NM",1.2972),kpc.point(3).y(),0.1);
	//EXPECT_NEAR(Units::from("ft",12864.1109),kpc.point(3).z(),0.01);
	EXPECT_NEAR(214.8083,kpc.point(3).time(),0.001);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
}




TEST_F(TrajGenTest, testBadAcuteTurn2) {
	//fpln(" ------------------------------ testBadAcuteTurn2 ---------------------------------");
	Position p0  = Position::makeXYZ(0.0000, 0.0000, 10000.0000);
	Position p1  = Position::makeXYZ(1.6749, -3.5917, 10000.0000);
	Position p2  = Position::makeXYZ(0.0000, 10.0000, 10000.0000);
	Position p3  = Position::makeXYZ(10.0000, 20.0000, 10000.0000);
	Position p4  = Position::makeXYZ(17.0000, 22.0000, 10000.0000);
	NavPoint np0(p0,0.0000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,92.0449);     lpc.addNavPoint(np1);
	NavPoint np2(p2,200.0000);     lpc.addNavPoint(np2);
	NavPoint np3(p3,380.0000);     lpc.addNavPoint(np3);
	NavPoint np4(p4,626.0000);     lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc, "testBadAcuteTurn2_lpc");
	// NO REPAIR !!
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc, "testBadAcuteTurn2_kpc");
	if (kpc.hasError()) {
		//fpln(" #### testBadAcuteTurn2: msg = "+kpc.getMessageNoClear());
	} else {
		EXPECT_TRUE(kpc.isWellFormed());
		EXPECT_TRUE(kpc.isFlyable());   // GROUND SPEED MESSED UP
		EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	}
}


TEST_F(TrajGenTest, testGenTurnsOverlap) {
	//fpln(" ------------------------------ testGenTurnsOverlap ---------------------------------");
	//Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",10.0), Units::from("nmi",20.0), Units::from("ft",10000)));
	//Position p3(Vect3(Units::from("nmi",-2.955), Units::from("nmi",20.0), Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",17.0), Units::from("nmi",22.0), Units::from("ft",10000)));
	//NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,380.0);
	NavPoint np4(p4,626.0);
	//lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc, "testGenTurnsOverlap_lpc");
	Position so(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	//Velocity vo = Velocity::makeTrkGsVs(80.0,195.0,0.0);
	//Velocity vo = Velocity::makeTrkGsVs(138.0,253.0,0.0);  // ********** VARY ************
	Velocity vo = Velocity::makeTrkGsVs(158.0,177.0,0.0);  // ********** VARY ************
	double to = 0;
	double timeBeforeTurn = 5.0;
	//double timeIntervalNextTry = 2.0;
	Plan lpc2 = TrajGen::genDirectToLinear(lpc,so,vo,to,bankAngle,timeBeforeTurn);
}


TEST_F(TrajGenTest, testGenDirectToRetry) {
	//fpln(" ------------------------------ testGenDirectToRetry ---------------------------------");
	//Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",10.0), Units::from("nmi",20.0), Units::from("ft",10000)));
	//Position p3(Vect3(Units::from("nmi",-2.955), Units::from("nmi",20.0), Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",17.0), Units::from("nmi",22.0), Units::from("ft",10000)));
	//NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,380.0);
	NavPoint np4(p4,626.0);
	//lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc, "testBadAcuteTurn_lpc");
	Position so(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	//Velocity vo = Velocity::makeTrkGsVs(80.0,195.0,0.0);
	Velocity vo = Velocity::makeTrkGsVs(250.0,380.0,0.0);  // ********** VARY ************
	double to = 10;
	double timeBeforeTurn = 2.0;
	Plan lpc2 = TrajGen::genDirectToLinear(lpc,so,vo,to,bankAngle,timeBeforeTurn);
	//PlanCore kpc = TrajGen::makeKinematicPlan(lpc2, bankAngle, gsAccel, vsAccel, false);
	//		Plan kpc = TrajGen::genDirectTo(lpc,so,vo,to,bankAngle,gsAccel, vsAccel,timeBeforeTurn);
	//		EXPECT_TRUE(kpc.hasError());
	//		//if (kpc.hasError())
	//		//fpln(" $$$$$$$$$ GENERATION ERROR: "+kpc.getMessage());
	//		//DebugSupport.dumpPlan(lpc2, "testBadAcuteTurn_lpc2");
	//		//DebugSupport.dumpPlan(kpc, "testBadAcuteTurn_kpc");
	//		EXPECT_TRUE(kpc.hasError());
	//		fpln(" testGenDirectToRetry: msg = "+kpc.getMessage());
	//		EXPECT_TRUE(kpc.isWellFormed());
	//		EXPECT_TRUE(kpc.isFlyable());
	//		EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//		kpc = TrajGen::genDirectToRetry(lpc,so,vo,to,bankAngle,gsAccel, vsAccel,timeBeforeTurn,20.0);
	//		EXPECT_TRUE(kpc.hasError());
	//		EXPECT_TRUE(kpc.isWellFormed());
	//		EXPECT_TRUE(kpc.isFlyable());
}



TEST_F(TrajGenTest, testTurnLatLon) {
	//fpln(" ------------------------------ testTurnLatLon ---------------------------------");
	LatLonAlt mid = GreatCircle::interpolate(KMSP,KDFW,0.5);
	double trk_mid = GreatCircle::initial_course(KMSP,KDFW);
	LatLonAlt midPerp = GreatCircle::linear_initial(mid, trk_mid+M_PI/2,Units::from("NM",20.0));
	double dist = GreatCircle::distance(KMSP,midPerp)+GreatCircle::distance(midPerp,KDFW);
	double gs = Units::from("kn",  480);
	double flightTime = dist/gs;
	NavPoint npMid(Position(midPerp),flightTime/2.0);
	NavPoint np1(Position(KMSP),0.0);
	NavPoint np2(Position(KDFW), flightTime);
	NavPoint npX(Position::makeLatLonAlt(39.1501, -95.6437,100000),2705.2745);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(npMid);
	//lpc.addNavPoint(npX);
	double firstTime = lpc.getFirstTime();
	//bool repair = true;
	//fpln("testTurnLatLon: lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testTurnLatLon_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc, "testTurnLatLon_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isWeakFlyable(true));
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_NEAR(firstTime,kpc.getFirstTime(),0.001);
	EXPECT_FALSE(kpc.hasMessage());
	//fpln("testTurnLatLon: kpc = "+kpc.toString());
	//fpln(" testTurnLatLon 0: kpc.getMessage() = "+kpc.getMessage());
	EXPECT_EQ(5,kpc.size());
	EXPECT_NEAR(246.611,kpc.initialVelocity(0).gs(),0.01);
	double t = 400;
	EXPECT_NEAR(0.768,kpc.position(t).lat(),0.001);
	EXPECT_NEAR(-1.634,kpc.position(t).lon(),0.001);
	EXPECT_NEAR(3048.0,kpc.position(t).alt(),0.001);
	EXPECT_NEAR(246.611,kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(0.722,kpc.position(1650).lat(),0.001);
	EXPECT_NEAR(-1.653,kpc.position(1650).lon(),0.001);
	EXPECT_NEAR(3048.0,kpc.position(t).alt(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);                 // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.0001);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.0001);             // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(4).gs(),0.01);                 // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(2).vs(),kpc.initialVelocity(4).vs(),0.00001);        // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(2).trk(),kpc.initialVelocity(4).trk(),0.0001);      // INVARIANT
	t = 500;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	t = 2777;
	EXPECT_FALSE(kpc.inVsChange(t));
	EXPECT_FALSE(kpc.inGsChange(t));
	EXPECT_TRUE(kpc.inTrkChange(t));

	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	EXPECT_TRUE(kpc.inTrkChange(t));
	EXPECT_EQ(1,kpc.prevBOT(kpc.getSegment(t)+1));
	EXPECT_EQ(3,kpc.nextEOT(0));
	EXPECT_NEAR(0.0,kpc.getTcpData(0).getRadiusSigned(),0.001);
	double gsIn1 = kpc.initialVelocity(0).gs();
	double R = Kinematics::turnRadius(gsIn1, bankAngle);
	//fpln(" $$$$ gsIn1 = "+Units::str8("kn",gsIn1)+" R = "+Units::str8("NM",R));
	//fpln(kpc.point(1).toOutput()+" "+kpc.point(1).turnRadius());
	//fpln(kpc.point(2).toOutput()+" "+kpc.point(2).turnRadius());
	//fpln(kpc.point(3).toOutput()+" "+kpc.point(3).turnRadius());
	EXPECT_NEAR(-17038.856,kpc.getTcpData(1).getRadiusSigned(),0.001);
	EXPECT_NEAR(-R,kpc.getTcpData(1).getRadiusSigned(),100);                  // off due to curvature of earth  ERROR ??
	//EXPECT_NEAR(R,kpc.point(2).turnRadius(),0.001);        // why different ??  MID no longer calculates radius
	//EXPECT_NEAR(17038.856,kpc.point(3).turnRadius(),0.001);
	EXPECT_NEAR(0.0,kpc.getTcpData(4).getRadiusSigned(),0.001);
	EXPECT_EQ(1,kpc.prevBOT(kpc.getSegment(t)+1));
	EXPECT_EQ(3,kpc.nextEOT(0));
	EXPECT_TRUE(kpc.isBOT(1));
	//EXPECT_TRUE(kpc.point(2).isTurnMid());
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.inTrkChange(kpc.point(1).time()));
	EXPECT_TRUE(kpc.inTrkChange(kpc.point(2).time()));
	EXPECT_NEAR(R,std::abs(kpc.turnRadiusAtTime(kpc.point(1).time())),50.0);                         // NOT ACCURATE: ERROR??
	EXPECT_NEAR(0.0,std::abs(kpc.gsAccelAtTime(kpc.point(2).time())),0.0001);           // ??
	EXPECT_NEAR(R,std::abs(kpc.turnRadiusAtTime((kpc.point(1).time()+kpc.point(2).time())/2.0)),50.0); // NOT ACCURATE: ERROR??
	t = 1640;
	EXPECT_NEAR(0.722,kpc.position(t).lat(),0.001);
	EXPECT_NEAR(-1.653,kpc.position(t).lon(),0.001);
}



TEST_F(TrajGenTest, testTurnNormalEucl) {
	//fpln(" ------------------------------ testTurnNormalEucl ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",-10.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,400.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	//double firstTime = lpc.getFirstTime();
	double gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testTurnNormalEucl: gs0 = "+Units::str8("kn",gs0));
	//fpln("testTurnNormalEucl: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
	EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
	bool repair = true;
	//fpln("testTurnNormalEucl: lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testTurnNormalEucl_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	EXPECT_TRUE(kpc.isFlyable());
	//DebugSupport.dumpPlan(kpc, "testTurnNormalEucl_kpc");
	//fpln("testTurnNormalEucl: kpc = "+kpc);
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isTCP(1));
	EXPECT_TRUE(!kpc.isTCP(2));
	EXPECT_TRUE(kpc.isTCP(3));
	EXPECT_TRUE(!kpc.isTCP(4));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	//fpln(" $$$ testTurnNormalEucl: kpc.point(1) = "+kpc.point(1).toString(8));
	EXPECT_NEAR(0.0,kpc.point(1).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",8.70283731),kpc.point(1).y(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(1).z(),0.0001);
	//fpln(" $$$  testTurnNormalEucl: kpc.point(2) = "+kpc.point(2).toString(8));
	EXPECT_NEAR(Units::from("NM",-0.37993015),kpc.point(2).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",9.6200698),kpc.point(2).y(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(2).z(),0.0001);
	//fpln(" $$$  testTurnNormalEucl: kpc.point(3) = "+kpc.point(3).toString(8));
	EXPECT_NEAR(Units::from("NM",-1.29716269),kpc.point(3).x(),0.0001);
	EXPECT_NEAR(Units::from("NM",10.00000000),kpc.point(3).y(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(3).z(),0.0001);
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_NEAR(lpc.getFirstTime(),kpc.getFirstTime(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.0001);             // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.0001);           // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(2).gs(),kpc.finalVelocity(4).gs(),0.01);                   // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(2).vs(),kpc.finalVelocity(4).vs(),0.00001);                // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(2).trk(),kpc.finalVelocity(4).trk(),0.00001);              // INVARIANT
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
}





//TEST_F(TrajGenTest, testShortLegs) {
//	//fpln(" ------------------------------ testShortLegs ---------------------------------");
//	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
//	Position p2(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
//	Position p3(Vect3(Units::from("nmi",-1.0), Units::from("nmi",10.0), Units::from("ft",10000)));
//	NavPoint np1(p1,0.0);
//	NavPoint np2(p2,200.0);
//	NavPoint np3(p3,220.0);
//	lpc.addNavPoint(np1);
//	lpc.addNavPoint(np2);
//	lpc.addNavPoint(np3);
//	//fpln("testShortLegs: BEFORE lpc = "+lpc);
//	lpc =  (Plan) PlanUtil::mkGsConstant(lpc,Units::from("kn",180.0));                                // Smooth It to Match 'testTurnNormalEucl"
//	//fpln("testShortLegs: AFTER lpc = "+lpc);
//	double gs0 = lpc.initialVelocity(0).gs();
//	double gs1 = lpc.initialVelocity(1).gs();
//	//fpln(" gs0 = "+Units::str8("kn",gs0));
//	//fpln(" gs1 = "+Units::str8("kn",gs1));
//	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
//	EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
//	bool repair = true;
//	//fpln("testShortLegs: lpc = "+lpc);
//	//lpc.setExtend(true);
//	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
//	EXPECT_TRUE(kpc.isFlyable());
//	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
//	//fpln("testShortLegs: kpc = "+kpc);
//	//DebugSupport.dumpPlan(kpc, "ShortLeg");
//	std::string msg = kpc.getMessage();
//	EXPECT_EQ(lpc.size()-1,kpc.size());               // removed point
//	//fpln("testShortLegs: kpc.getMessage() = "+msg);
//	//EXPECT_TRUE(plans_equal_position(lpc,kpc));
//	// +++++++++++++ alter plan +++++++++++++
//	lpc.remove(2);
//	np3 = np3.makeTime(230);
//	lpc.addNavPoint(np3);
//	//fpln(" lpc = "+lpc);
//	gs0 = lpc.initialVelocity(0).gs();
//	gs1 = lpc.initialVelocity(1).gs();
//	//fpln("testGs: gs0 = "+Units::str8("kn",gs0));
//	//fpln("testGs: gs1 = "+Units::str8("kn",gs1));
//	EXPECT_NEAR(gs0,Units::from("kn",180.0),0.001);
//	EXPECT_NEAR(gs1,Units::from("kn",120),0.001);
//	kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
//	EXPECT_TRUE(kpc.isFlyable());
//	//fpln("testShortLegs: kpc = "+kpc);
//	Plan lpcNew = TrajGen::linearRepairShortTurnLegs(lpc,bankAngle,false);
//	//fpln("testShortLegs: lpcNew = "+lpcNew);
//	EXPECT_EQ(3,lpcNew.size());
//}


TEST_F(TrajGenTest, testShortLegs2) {
	//fpln(" ------------------------------ testShortLegs2 ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",9.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",0.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",-10.0), Units::from("nmi",10.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,20.0);
	NavPoint np3(p3,220.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	double gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	//fpln("testShortLegs2: gs0 = "+Units::str8("kn",gs0));
	//fpln("testShortLegs2: gs1 = "+Units::str8("kn",gs1));
	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
	EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
	bool repair = true;
	//DebugSupport.dumpPlan(lpc, "testShortLegs2_lpc");

	np1 = NavPoint(p3,0.0);
	np2 = NavPoint(p2,200.0);
	np3 = NavPoint(p1,220.0);
	lpc.clear();
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	//DebugSupport.dumpPlan(lpc, "testShortLegs2_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//DebugSupport.dumpPlan(kpc, "testShortLegs2_kpc");

}



TEST_F(TrajGenTest, testMakeSpeedConstant) {
	//fpln(" ------------------------------ testMakeSpeedConstant ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",8.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",8.0), Units::from("nmi",5.0), Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",0.0), Units::from("nmi",12.0), Units::from("ft",10000)));
	Position p5(Vect3(Units::from("nmi",5.0), Units::from("nmi",18.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,179);
	NavPoint np3(p3,270);
	NavPoint np4(p4,390);
	NavPoint np5(p5,543);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	double gs0 = lpc.initialVelocity(0).gs();
	Plan nlpc =  (Plan) PlanUtil::mkGsConstant(lpc,gs0);
	for (int j = 0; j < nlpc.size()-1; j++) {
		//fpln(" nlpc.initialVelocity(j).gs = "+Units::str("kn",nlpc.initialVelocity(j).gs()));
		EXPECT_NEAR(gs0,nlpc.initialVelocity(j).gs(),0.001);
	}
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	EXPECT_TRUE(kpc.isFlyable());
}


//
//
//	TEST_F(TrajGenTest, testMakeVsConstant) {
//		Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
//		Position p2(Vect3(Units::from("nmi",7.222), Units::from("nmi",7.222), Units::from("ft",10000)));
//		Position p3(Vect3(Units::from("nmi",10.0), Units::from("nmi",10.0), Units::from("ft",13000)));
//		Position p4(Vect3(Units::from("nmi",13.140), Units::from("nmi",10.6), Units::from("ft",15000)));
//		Position p5(Vect3(Units::from("nmi",20.0), Units::from("nmi",12.0), Units::from("ft",15000)));
//		NavPoint np1(p1,0.0);
//		NavPoint np2(p2,134.28);
//		NavPoint np3(p3,185.93);
//		NavPoint np4(p4,228.0);
//		NavPoint np5(p5,320.0);
//		lpc.addNavPoint(np1);   // Add out of order for check
//		lpc.addNavPoint(np3);
//		lpc.addNavPoint(np5);
//		lpc.addNavPoint(np2);
//		lpc.addNavPoint(np4);
//		PlanCore kpcCore(lpc);
//		//EXPECT_TRUE(isVsConstant(kpc));
//	}
//




TEST_F(TrajGenTest, testVsSimple) {
	EXPECT_FALSE(lpc.hasMessage());
	Position pKMSP(KMSP);
	Position pKDFW(KDFW);
	LatLonAlt top = GreatCircle::interpolate( KMSP,KDFW,0.66).makeAlt(15000);
	Position pTop(top);
	NavPoint np1(pKMSP,0.0);
	NavPoint np2(pKDFW,4000.0);
	NavPoint nTop(pTop,1600.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(nTop);
	//fpln("testVsSimple: BEFORE lpc = "+lpc);
	lpc =  PlanUtil::mkGsConstant(lpc);                                // Smooth It to Match 'testTurnNormalEucl"
	//fpln("testVsSimple: AFTER lpc = "+lpc);
	double firstTime = lpc.getFirstTime();
	bool repair = true;
	//fpln("testVsSimple: lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testVsSimple_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc, "testVsSimple_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_FALSE(kpc.hasMessage());
	//pln("testVsSimple kpc = "+kpc);
	EXPECT_EQ(4,kpc.size());
	EXPECT_NEAR(firstTime,kpc.getFirstTime(),0.001);
	double t = 2640;
	EXPECT_TRUE(kpc.inVsChange(t));
	EXPECT_FALSE(kpc.inGsChange(t));
	EXPECT_NEAR(342.785,kpc.initialVelocity(0).gs(),0.01);
	t = 400;
	EXPECT_NEAR(0.762,kpc.position(t).lat(),0.001);
	EXPECT_NEAR(-1.634,kpc.position(t).lon(),0.001);
	EXPECT_NEAR(3278.909,kpc.position(t).alt(),0.001);
	EXPECT_NEAR(342.785,kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(0.697,kpc.position(1650).lat(),0.001);
	EXPECT_NEAR(-1.657,kpc.position(1650).lon(),0.001);
	EXPECT_NEAR(3278.909,kpc.position(t).alt(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.0001);             // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.0001);           // INVARIANT
	EXPECT_TRUE(kpc.finalVelocity(4).isInvalid());         // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(2).gs(),kpc.finalVelocity(3).gs(),0.01);         // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(2).vs(),kpc.finalVelocity(3).vs(),0.00001);      // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(2).trk(),kpc.finalVelocity(3).trk(),0.00001);    // INVARIANT
	t = 500;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	t = 2639.2;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.5);   // ??
	EXPECT_TRUE(kpc.inVsChange(t));
	EXPECT_EQ(1,kpc.prevBVS(kpc.getSegment(t)+1));
	EXPECT_EQ(2,kpc.nextEVS(0));
	t = 2900;
	EXPECT_NEAR(lpc.position(t).lat(),kpc.position(t).lat(),0.001);
	EXPECT_NEAR(lpc.position(t).lon(),kpc.position(t).lon(),0.001);
	EXPECT_NEAR(lpc.position(t).alt(),kpc.position(t).alt(),0.001);
	EXPECT_EQ(1,kpc.prevBVS(kpc.getSegment(t)+1));
	EXPECT_EQ(2,kpc.nextEVS(0));
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isEVS(2));
	EXPECT_TRUE(kpc.inVsChange(kpc.point(1).time()));
	// EXPECT_NEAR(vsAccel,std::abs(kpc.vsAccel(kpc.point(1).time())),0.0001);    // ERROR ??
	EXPECT_NEAR(0.0,std::abs(kpc.vsAccelAtTime(kpc.point(2).time())),0.0001);           // ??
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
	//DebugSupport.dumpPlan(noTCPS, "testVsSimple_noTCPS");
	//EXPECT_TRUE(almostEquals(lpc,noTCPS));
}



TEST_F(TrajGenTest, testTurnTurn) {
	//fpln(" ------------------------------ testTurnTurn ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	double legLength = Units::from("nmi",2.4);
	Position p3(Vect3(Units::from("nmi",10.0), legLength, Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",10.0), 3.5*legLength, Units::from("ft",10000)));
	Position p5(Vect3(Units::from("nmi",20.0), 4.5*legLength, Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,248.0);
	NavPoint np4(p4,340);
	NavPoint np5(p5,497.54);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	double gs0 = lpc.initialVelocity(0).gs();
	//fpln("testTurnAround: gs0 = "+Units::str8("kn",gs0));
	//fpln("testTurnAround: gs1 = "+Units::str8("kn",gs1));
	//fpln("testTurnAround: gs2 = "+Units::str8("kn",gs2));
	//fpln("testTurnAround: BEFORE lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testTurnTurn_lpc");
	//fpln("testTurnAround: AFTER lpc = "+lpc);
	gs0 = lpc.initialVelocity(0).gs();
	double gs1 = lpc.initialVelocity(1).gs();
	EXPECT_NEAR(Units::from("kn",180.0),gs0,0.001);
	EXPECT_NEAR(Units::from("kn",180.0),gs1,0.001);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("testTurnTurn: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testTurnTurn_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_EQ(10,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBGS(4));
	EXPECT_TRUE(kpc.isEGS(5));
	EXPECT_TRUE(kpc.isBOT(6));
	EXPECT_TRUE(kpc.isEOT(8));
	EXPECT_FALSE(kpc.isTCP(9));
	//fpln(" kpc.point(7).velocityIn().gs() = "+Units::str("kn",kpc.point(7).velocityIn().gs()));
	EXPECT_NEAR(Units::from("kn",180),kpc.initialVelocity(1).gs(),0.001);
	EXPECT_NEAR(Units::from("kn",180),kpc.initialVelocity(3).gs(),0.001);
	EXPECT_NEAR(Units::from("kn",180),kpc.initialVelocity(4).gs(),0.001);
	//EXPECT_NEAR(Units::from("kn",180),kpc.initialVelocity(5).gs(),0.001);
	EXPECT_NEAR(Units::from("kn",234.78),kpc.initialVelocity(6).gs(),0.01);
	//EXPECT_NEAR(Units::from("kn",234.78),kpc.point(7).velocityIn().gs(),0.01);
	EXPECT_NEAR(Units::from("kn",235.0),kpc.initialVelocity(8).gs(),0.01);
	EXPECT_NEAR(Units::from("NM",8.7028),kpc.point(1).x(),0.1);
	EXPECT_NEAR(Units::from("NM",0.0),kpc.point(1).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(1).z(),0.01);
	EXPECT_NEAR(174.0567,kpc.point(1).time(),0.001);
	EXPECT_NEAR(Units::from("NM",9.6201),kpc.point(2).x(),0.1);
	EXPECT_NEAR(Units::from("NM",0.3799),kpc.point(2).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(2).z(),0.01);
	EXPECT_NEAR(194.4325,kpc.point(2).time(),0.001);
	EXPECT_NEAR(Units::from("NM",10.0),kpc.point(3).x(),0.1);
	EXPECT_NEAR(Units::from("NM",1.2972),kpc.point(3).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(3).z(),0.01);
	EXPECT_NEAR(214.8083,kpc.point(3).time(),0.001);
	EXPECT_NEAR(236.865,kpc.point(4).time(),0.001);
	EXPECT_NEAR(250.956,kpc.point(5).time(),0.001);
	EXPECT_NEAR(303.830,kpc.point(6).time(),0.001);
	EXPECT_NEAR(326.422,kpc.point(7).time(),0.001);
	EXPECT_NEAR(Units::from("kn",180),kpc.finalVelocity(1).gs(),0.001);
	EXPECT_NEAR(Units::from("kn",180),kpc.finalVelocity(2).gs(),0.001);
	EXPECT_NEAR(Units::from("kn",180),kpc.finalVelocity(3).gs(),0.001);
	EXPECT_NEAR(Units::from("kn",234.78),kpc.finalVelocity(4).gs(),0.01);
	EXPECT_NEAR(Units::from("kn",234.78),kpc.finalVelocity(5).gs(),0.01);
	EXPECT_NEAR(Units::from("kn",234.78),kpc.finalVelocity(6).gs(),0.01);
	EXPECT_NEAR(Units::from("kn",234.78),kpc.finalVelocity(7).gs(),0.2);
	EXPECT_NEAR(Units::from("kn",235.0),kpc.finalVelocity(8).gs(),0.01);
	//EXPECT_TRUE(Plans_equal_gs(lpc,kpc));
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
}




TEST_F(TrajGenTest, testVsWithTurn) {
	//fpln(" ------------------------------ testVsWithTurn ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",10.0), Units::from("nmi",10.0), Units::from("ft",13000)));
	Position p5(Vect3(Units::from("nmi",20.0), Units::from("nmi",12.0), Units::from("ft",15018.5)));
	Position m1(Vect3(Units::from("nmi",7.222), Units::from("nmi",7.222), Units::from("ft",10000)));
	Position m2(Vect3(Units::from("nmi",12.592), Units::from("nmi",10.518), Units::from("ft",15018.5)));
	NavPoint np1(p1,0.0);
	NavPoint np3(p3,185.93);
	NavPoint np5(p5,320.0);
	NavPoint np2(m1,134.28);
	NavPoint np4(m2,220.68);
	lpc.addNavPoint(np1);   // Add out of order for check
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np5);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np4);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	EXPECT_TRUE(kpc.isFlyable());
	std::string msg = kpc.getMessage();
	EXPECT_EQ(9,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isEVS(2));
	EXPECT_TRUE(kpc.isBOT(3));
	EXPECT_TRUE(kpc.isEOT(5));
	EXPECT_TRUE(kpc.isBVS(6));
	EXPECT_TRUE(kpc.isEVS(7));
	EXPECT_FALSE(kpc.isTCP(8));
	EXPECT_NEAR(12486.290,kpc.point(1).x(),0.1);
	EXPECT_NEAR(12486.290,kpc.point(1).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10000),kpc.point(1).z(),0.01);
	EXPECT_NEAR(3207.26,kpc.point(2).z(),0.015);
	EXPECT_TRUE(lpc.point(0).almostEqualsPosition((kpc.point(0))));
	EXPECT_TRUE(lpc.point(4).almostEqualsPosition((kpc.point(8))));
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//XPECT_TRUE(lpc.almostEquals(noTCPS));
}




TEST_F(TrajGenTest, testVsLevelOutWithGS) {
	//fpln(" ------------------------------ testVsLevelOutWithGS ---------------------------------");
	Position p1(LatLonAlt::make(45.01, -93.31, 10000.0));
	NavPoint np1(p1,80);
	//NavPoint np4(p4,49.21);
	Velocity v1 = Velocity::makeTrkGsVs(100,470,2000);
	Velocity v2 = Velocity::makeTrkGsVs(100,450,2000);
	Velocity v3 = Velocity::makeTrkGsVs(100,410,0.0);
	//fpln(" v = "+v);
	NavPoint np2 = np1.linear(v1,30.0);
	NavPoint np3 = np2.linear(v2,20.0); // .makeTime(132);
	NavPoint np4 = np3.linear(v3,20.0); // makeTime(155);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("testVsLevelOutWithGS: kpc = "+kpc.toString());
	//DebugSupport.dumpPlan(kpc, "testVsLevelOutWithGS_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_EQ(8,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isEGS(2));
	EXPECT_TRUE(kpc.isBVS(3));
	EXPECT_TRUE(kpc.isBGS(4));
	EXPECT_TRUE(kpc.isEVS(5));
	EXPECT_TRUE(kpc.isEGS(6));
	EXPECT_FALSE(kpc.isTCP(7));
	EXPECT_NEAR(3556,kpc.point(5).alt(),0.01);
	EXPECT_NEAR(kpc.point(5).alt(),kpc.point(6).alt(),0.01);
	EXPECT_NEAR(kpc.point(6).alt(),kpc.point(7).alt(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(0).trk(),kpc.finalVelocity(0).trk(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).trk(),kpc.initialVelocity(5).trk(),0.01);
	EXPECT_NEAR(kpc.initialVelocity(6).gs(),lpc.initialVelocity(2).gs(),0001);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
}





TEST_F(TrajGenTest, testSimpleTurn) {
	//fpln(" ------------------------------ testSimpleTurn ---------------------------------");
	Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p2 = Position::makeXYZ(10.0, 5.0, 10000);
	Position p3 = Position::makeXYZ(20.0, 15.0, 10000);
	Velocity v4 = Velocity::makeTrkGsVs(45,450,500);
	Position p4 = p3.linear(v4,200);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,201.25);
	NavPoint np3(p3,455.8);
	NavPoint np4(p4,905.8);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	//lpc = lpc.makeSpeedConstant(Units::from("kn",180.0));
	//DebugSupport.dumpPlan(lpc, "testSimpleTurn_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("testTurnAround: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testSimpleTurn_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_EQ(7,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBVS(4));
	EXPECT_TRUE(kpc.isEVS(5));
	EXPECT_FALSE(kpc.isTCP(6));
	for (int i = 0; i < 5; i++) {
		EXPECT_NEAR(lpc.point(0).alt(),kpc.point(i).alt(),0.01);
	}
	EXPECT_NEAR(lpc.initialVelocity(0).compassAngle(),kpc.initialVelocity(0).compassAngle(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).compassAngle(),kpc.initialVelocity(5).compassAngle(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(0).gs(),kpc.finalVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).gs(),kpc.finalVelocity(3).gs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(2).gs(),kpc.finalVelocity(5).gs(),0.01);   // after gs accel
	EXPECT_NEAR(lpc.finalVelocity(0).vs(),kpc.finalVelocity(0).vs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).vs(),kpc.finalVelocity(3).vs(),0.01);
	for (int i = 0; i < 4; i++) {
		EXPECT_NEAR(lpc.point(0).alt(),kpc.point(i).alt(),0.01);
	}
	EXPECT_NEAR(lpc.point(0).x(),kpc.point(0).x(),0.01);
	EXPECT_NEAR(lpc.point(0).y(),kpc.point(0).y(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).x(),kpc.point(kpc.size()-1).x(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).y(),kpc.point(kpc.size()-1).y(),0.01);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
	// THE FOLLOWING WILL FAIL BECAUSE OF VERTICAL TCPS
	//double gs0 = lpc.initialVelocity(0).gs();
	//PlanCore kpc2 = kpc.genGSConstant(10.2*gs0);
	//DebugSupport.dumpPlan(kpc2, "testSimpleTurn_kpc2");
	//EXPECT_TRUE(kpc2.isConsistent());
}





TEST_F(TrajGenTest, testTwoTurns) {
	//fpln(" ------------------------------ testTwoTurns ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",10.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",10.0), Units::from("nmi",5.0), Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",20.0), Units::from("nmi",5.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,200.0);
	NavPoint np3(p3,300.0);
	NavPoint np4(p4,420.0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	bool repair = true;
	//fpln("testTwoTurns: lpc = "+lpc);
	//TrajGen tG = new TrajGen();
	//tG.setTrajPreserveGS(true);
	//lpc.setExtend(true);
	//DebugSupport.dumpPlan(lpc, "testTwoTurns_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc, "testTwoTurns_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_EQ(9,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBOT(4));
	EXPECT_TRUE(kpc.isEOT(6));
	EXPECT_FALSE(kpc.isTCP(8));
	EXPECT_NEAR(lpc.finalVelocity(0).trk(),kpc.finalVelocity(0).trk(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).trk(),kpc.finalVelocity(3).trk(),0.01);
	for (int i = 6; i < kpc.size()-1; i++) {
		//fpln("i="+i+""+lpc.finalVelocity(2)+" "+kpc.finalVelocity(i));
		EXPECT_NEAR(lpc.finalVelocity(2).trk(),kpc.finalVelocity(i).trk(),0.01);
		EXPECT_NEAR(lpc.finalVelocity(2).vs(),kpc.finalVelocity(i).vs(),0.01);
	}
	EXPECT_NEAR(lpc.finalVelocity(0).vs(),kpc.finalVelocity(0).vs(),0.01);
	EXPECT_NEAR(lpc.finalVelocity(1).vs(),kpc.finalVelocity(3).vs(),0.01);
	for (int i = 0; i < kpc.size(); i++) {
		EXPECT_NEAR(lpc.point(2).alt(),kpc.point(i).alt(),0.01);
	}
	EXPECT_NEAR(lpc.point(0).x(),kpc.point(0).x(),0.01);
	EXPECT_NEAR(lpc.point(0).y(),kpc.point(0).y(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).x(),kpc.point(kpc.size()-1).x(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).y(),kpc.point(kpc.size()-1).y(),0.01);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
}



TEST_F(TrajGenTest, testVsWithGs) {
	//fpln(" ------------------------------ testVsWithGs ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",1.0), Units::from("nmi",1.0), Units::from("ft",11000)));
	Position p5(Vect3(Units::from("nmi",2.0), Units::from("nmi",2.0), Units::from("ft",11000)));
	NavPoint np1(p1,0.0);
	NavPoint np3(p3,27.81);
	NavPoint np5(p5,50.0);
	lpc.addNavPoint(np1);   // Add out of order for check
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc, "testVsWithGs_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln(" kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testVsWithGs_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_FALSE(kpc.hasMessage());
	EXPECT_EQ(6,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isBGS(2));
	EXPECT_TRUE(kpc.isEVS(3));
	EXPECT_TRUE(kpc.isEGS(4));
	EXPECT_FALSE(kpc.isTCP(5));
	EXPECT_NEAR(kpc.point(5).x(),lpc.point(2).x(),0.01);
	EXPECT_NEAR(kpc.point(5).y(),lpc.point(2).y(),0.01);
	EXPECT_NEAR(kpc.point(5).z(),lpc.point(2).z(),0.01);
	EXPECT_NEAR(3337.78,kpc.point(2).z(),0.01);
	EXPECT_NEAR(Units::from("ft",11000),kpc.point(3).z(),0.01);
	EXPECT_NEAR(Units::from("ft",11000),kpc.point(4).z(),0.01);
	EXPECT_NEAR(Units::from("ft",11000),kpc.point(5).z(),0.01);
	// invariants
	EXPECT_TRUE(lpc.point(0).almostEquals(kpc.point(0)));
	//fpln(">>>>>>>>>>.. "+lpc.point(4)+" "+kpc.point(8));
	//EXPECT_TRUE(NavPoint_equal_position(lpc.point(4),(kpc.point(8))));
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
}




TEST_F(TrajGenTest, testTurnVsWithAtMid) {
	fpln(" ------------------------------ testTurnVsWithAtMid ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",1.49), Units::from("nmi",0.765), Units::from("ft",10200)));
	Position p5(Vect3(Units::from("nmi",2.0), Units::from("nmi",2.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np3(p3,27.81);
	NavPoint np5(p5,50.0);
	lpc.addNavPoint(np1);   // Add out of order for check
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc, "testTurnVsWithAtMid_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testTurnVsWithAtMid_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//EXPECT_FALSE(kpc.hasMessage());
	EXPECT_EQ(7,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isBVS(2));
	EXPECT_TRUE(kpc.isEVS(4));
	EXPECT_TRUE(kpc.isEOT(5));
	EXPECT_FALSE(kpc.isTCP(6));
	//fpln("testTurnVsWithAtMid: "+ Units::str("ft",kpc.point(3).alt()) +"<"+ Units::str("ft",lpc.point(1).alt()));
	EXPECT_TRUE(kpc.point(3).alt() < lpc.point(1).alt());
	// invariants
	EXPECT_TRUE(lpc.point(0).almostEquals(kpc.point(0)));
	//fpln(">>>>>>>>>>.. "+lpc.point(4)+" "+kpc.point(8));
	//EXPECT_TRUE(NavPoint_equal_position(lpc.point(4),(kpc.point(8))));
	EXPECT_TRUE(Plans_equal_gs(lpc,kpc));
	//fpln(" kpc = "+kpc);
	EXPECT_NEAR(Units::from("NM",0.8742),kpc.point(1).x(),0.1);
	EXPECT_NEAR(Units::from("NM",0.4489),kpc.point(1).y(),0.1);
	EXPECT_NEAR(Units::from("ft",10119.740),kpc.point(1).z(),0.15);
	//fpln(" $$$  testTurnNormalEucl: kpc.point(2) = "+kpc.point(2).toString(8));
	EXPECT_NEAR(2393.102,kpc.point(2).x(),0.1);
	EXPECT_NEAR(1372.513,kpc.point(2).y(),0.1);
	EXPECT_NEAR(3103.34,kpc.point(2).z(),0.01);
	//fpln(" $$$  testTurnNormalEucl: kpc.point(3) = "+kpc.point(3).toString(8));
	//EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.0001);             // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.0001);           // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(2).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(3).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(5).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(5).gs(),0.01);
	//	    EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(9).gs(),0.01);
	//	    EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(8).gs(),0.01);   // after gs accel
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(1).vs(),0.1);
	EXPECT_NEAR(lpc.point(0).x(),kpc.point(0).x(),0.01);
	EXPECT_NEAR(lpc.point(0).y(),kpc.point(0).y(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).x(),kpc.point(kpc.size()-1).x(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).y(),kpc.point(kpc.size()-1).y(),0.01);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
    PlanUtil::checkReversion(noTCPS,lpc);
	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
}




TEST_F(TrajGenTest, testTwoGSTwoVs) {
	fpln(" ------------------------------ testTwoGSTwoVs ---------------------------------");
	Position p1(LatLonAlt::make(45.01, -93.31, 10000.0));
	NavPoint np1(p1,80);
	//NavPoint np4(p4,49.21);
	Velocity v1 = Velocity::makeTrkGsVs(10,470,2000);
	Velocity v2 = Velocity::makeTrkGsVs(10,450,1000);
	Velocity v3 = Velocity::makeTrkGsVs(10,410,0.0);
	//fpln(" v = "+v);
	NavPoint np2 = np1.linear(v1,30.0);
	NavPoint np3 = np2.linear(v2,20.0); // .makeTime(132);
	NavPoint np4 = np3.linear(v3,20.0); // makeTime(155);
	NavPoint np5 = np4.linear(v3,20.0); // makeTime(155);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc, "testTwoGSTwoVs_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testTwoGSTwoVs_kpc");
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_EQ(11,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isBGS(2));
	EXPECT_TRUE(kpc.isEVS(3));
	EXPECT_TRUE(kpc.isEGS(4));
	EXPECT_TRUE(kpc.isBVS(5));
	EXPECT_TRUE(kpc.isBGS(6));
	EXPECT_TRUE(kpc.isEVS(7));
	EXPECT_TRUE(kpc.isEGS(8));
	EXPECT_FALSE(kpc.isTCP(9));                  // unnecessary point
	EXPECT_FALSE(kpc.isTCP(10));
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).trk(),kpc.initialVelocity(3).trk(),0.01);
	for (int i = 0; i < kpc.size()-1; i++) {
		//fpln(" i = 0 "+i+" kpc.initialVelocity(i) =  "+kpc.initialVelocity(i));
		EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(i).trk(),0.01);
	}
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(4).gs(),0.01);   // ** HERE **
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(5).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(8).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(9).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(8).gs(),0.01);   // after gs accel
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.initialVelocity(3).vs(),0.1);
	for (int i = 7; i < kpc.size(); i++) {
		EXPECT_NEAR(lpc.point(4).alt(),kpc.point(i).alt(),0.01);
	}
	EXPECT_NEAR(lpc.point(0).x(),kpc.point(0).x(),0.01);
	EXPECT_NEAR(lpc.point(0).y(),kpc.point(0).y(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).x(),kpc.point(kpc.size()-1).x(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).y(),kpc.point(kpc.size()-1).y(),0.01);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
}




TEST_F(TrajGenTest, testTurnVsHard) {
	fpln(" ------------------------------ testTurnVsHard ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",20000)));
	Position p2(Vect3(Units::from("nmi",-0.5), Units::from("nmi",1.5), Units::from("ft",20000)));
	Position p3(Vect3(Units::from("nmi",0.0), Units::from("nmi",3.0), Units::from("ft",20000)));
	//Position p4(Vect3(Units::from("nmi",0.267), Units::from("nmi",3.787), Units::from("ft",19692.590)));
	Position p5(Vect3(Units::from("nmi",0.8), Units::from("nmi",5.356), Units::from("ft",19000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,20.1476);
	NavPoint np3(p3,40.2952);
	//NavPoint np4(p4,49.21);
	NavPoint np5(p5,72.00);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	//lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	//lpc = lpc.makeSpeedConstant();
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testTurnVsHard_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testTurnVsHard_kpc");
	//fpln("testTurnVsHard: kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	std::string msg = kpc.getMessage();
	//fpln(" msg = "+msg);
	EXPECT_FALSE(kpc.hasMessage());
	EXPECT_EQ(7,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBVS(4));
	EXPECT_TRUE(kpc.isEVS(5));
	EXPECT_FALSE(kpc.isTCP(6));
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(0).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(1).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(2).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(3).z(),0.001);
	double vs1 = kpc.initialVelocity(4).vs();
	double vs2 = kpc.finalVelocity(4).vs();
	//fpln(" $$$ vs1 = "+vs1+" vs2 = "+vs2);
	double dt = kpc.time(5)-kpc.time(4);
	double a = kpc.vsAccel(4);
	double alt = kpc.point(3).alt()+0.5*a*dt*dt;
	EXPECT_NEAR(a,(vs2-vs1)/dt,0.001);
	EXPECT_NEAR(alt,kpc.point(5).z(),0.001);
	//EXPECT_NEAR(Units::from("ft",19393.550),kpc.point(5).z(),0.001);
	EXPECT_NEAR(Units::from("ft",19000),kpc.point(6).z(),0.001);
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(1).trk(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(1).trk(),kpc.initialVelocity(4).trk(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(1).trk(),kpc.initialVelocity(5).trk(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(2).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(3).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(4).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(5).gs(),0.01);               // INVARIANT
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
	double newVsAccel = 0.55;
    noTCPS.setVsAccel(2,newVsAccel);
	Plan kpc2 = TrajGen::makeKinematicPlan(noTCPS, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("$$ testTurnVsHard: kpc2 = "+kpc2.toString());
	int ixBVS = kpc2.nextBVS(0);
	EXPECT_NEAR(newVsAccel,std::abs(kpc2.vsAccel(ixBVS)),0.001);
	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
}






TEST_F(TrajGenTest, testRandom) {
	fpln(" ------------------------------ testRandom ---------------------------------");
	Position p1(Vect3(Units::from("nmi",14.6439), Units::from("nmi",13.4648), Units::from("ft",7059.9329)));
	Position p2(Vect3(Units::from("nmi",18.9721), Units::from("nmi",22.7693), Units::from("ft",7900.3569)));
	Position p3(Vect3(Units::from("nmi",32.7232), Units::from("nmi",31.7079), Units::from("ft",9230.3704)));
	Position p4(Vect3(Units::from("nmi",42.1049), Units::from("nmi",34.4970), Units::from("ft",7727.2198)));
	Position p5(Vect3(Units::from("nmi",42.4013), Units::from("nmi",43.5021), Units::from("ft",6390.0410)));
	Position p6(Vect3(Units::from("nmi",43.2642), Units::from("nmi",60.6884), Units::from("ft",3914.6675)));
	Position p7(Vect3(Units::from("nmi",48.4222), Units::from("nmi",64.3791), Units::from("ft",4726.1268)));
	Position p8(Vect3(Units::from("nmi",63.9031), Units::from("nmi",59.4313), Units::from("ft",7367.6314)));
	Position p9(Vect3(Units::from("nmi",87.4562), Units::from("nmi",65.9610), Units::from("ft",9062.0401)));
	Position p10(Vect3(Units::from("nmi",96.4651), Units::from("nmi",68.4790), Units::from("ft",9598.5122)));
	Position p11(Vect3(Units::from("nmi",93.8363), Units::from("nmi",78.8310), Units::from("ft",7988.6856)));
	NavPoint np1(p1,243.6765);
	NavPoint np2(p2,379.6857);
	NavPoint np3(p3,594.9424);
	NavPoint np4(p4,733.1321);
	NavPoint np5(p5,856.1062);
	NavPoint np6(p6,1083.7563);
	NavPoint np7(p7,1165.6394);
	NavPoint np8(p8,1373.5636);
	NavPoint np9(p9,1657.8353);
	NavPoint np10(p10,1763.8560);
	NavPoint np11(p11,1880.6028);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	lpc.addNavPoint(np6);
	lpc.addNavPoint(np7);
	lpc.addNavPoint(np8);
	lpc.addNavPoint(np9);
	lpc.addNavPoint(np10);
	lpc.addNavPoint(np11);
	bool repair = true;
	//fpln("testTwoTurns: lpc = "+lpc);
	//tG.setTrajPreserveGS(true);
	//DebugSupport.dumpPlan(lpc, "testRandom_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc, "testRandom_kpc");
	//fpln("testTwoTurns: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "TwoTurns");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,60));
	if (!kpc.isFlyable()) fpln(" ......... testRandom needs work on isConsistent");
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
	//EXPECT_TRUE(plans_almost_equal(lpc,noTCPS));
}



TEST_F(TrajGenTest, testTurnVertGs) {
	fpln(" ------------------------------ testTurnVertGs ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",20000)));
	Position p2(Vect3(Units::from("nmi",-0.5), Units::from("nmi",1.5), Units::from("ft",20000)));
	Position p3(Vect3(Units::from("nmi",0.0), Units::from("nmi",3.0), Units::from("ft",20000)));
	Position p4(Vect3(Units::from("nmi",0.8), Units::from("nmi",5.356), Units::from("ft",19008.890)));
	//Position p5(Vect3(Units::from("nmi",0.267), Units::from("nmi",3.787), Units::from("ft",19692.590)));
	//NavPoint np5(p5,49.21);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,19.4815);
	NavPoint np3(p3,38.9630);
	NavPoint np4(p4,72.00);
	//NavPoint np5 = np3.linear(np3.initialVelocity(np4),10.26).mkAlt(Units::from("ft",19800));
	//np5 = np5.makeSourcePosition(np5.position());
	NavPoint np3L = np3.linear(np3.initialVelocity(np4),10.26);
	NavPoint np5 = np3L.mkAlt(Units::from("ft",19800));
	//f.pln(" ## np5 = "+np5.toStringFull());
	//np5 = np5.makeSource(np5.position(),np5.time());

	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	//lpc.resetSource(ix5);

	//lpc = lpc.makeSpeedConstant();
	//DebugSupport.dumpPlan(lpc, "testTurnVertGs_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, false);
	//DebugSupport.dumpPlan(kpc, "testTurnVertGs_kpc");
	//fpln(" kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.isFlyable());
	std::string msg = kpc.getMessage();
	//fpln(" msg = "+msg);
	EXPECT_FALSE(kpc.hasMessage());
	EXPECT_EQ(11,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBVS(4));
	EXPECT_TRUE(kpc.isBGS(5));
	EXPECT_TRUE(kpc.isEVS(6));
	EXPECT_TRUE(kpc.isEGS(7));
	EXPECT_TRUE(kpc.isBVS(8));
	EXPECT_TRUE(kpc.isEVS(9));
	EXPECT_FALSE(kpc.isTCP(10));
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(0).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(1).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(2).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(3).z(),0.001);
	EXPECT_NEAR(Units::from("ft",20000),kpc.point(4).z(),0.001);
	EXPECT_NEAR(6091.40,kpc.point(5).z(),0.001);
	EXPECT_NEAR(6077.60,kpc.point(6).z(),0.01);
	EXPECT_NEAR(6063.15,kpc.point(7).z(),0.01);
	EXPECT_NEAR(6048.75,kpc.point(8).z(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(2).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(3).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(4).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(5).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(7).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(3).gs(),kpc.initialVelocity(9).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(3).gs(),kpc.finalVelocity(6).gs(),0.01);               // INVARIANT
	EXPECT_NEAR(lpc.finalVelocity(3).gs(),kpc.finalVelocity(7).gs(),0.01);               // INVARIANT
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//DebugSupport.dumpPlan(noTCPS, "f_noTCPS");
	//EXPECT_TRUE(plans_almost_equal(lpc,noTCPS));
}



TEST_F(TrajGenTest, testVsWithTurnHard3Vert) {
	fpln(" ------------------------------ testVsWithTurnHard3Vert ---------------------------------");
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	//Position p2(Vect3(Units::from("nmi",1.2), Units::from("nmi",0.62), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",1.49), Units::from("nmi",0.765), Units::from("ft",10080)));
	//Position p4(Vect3(Units::from("nmi",1.588), Units::from("nmi",1.0), Units::from("ft",10080)));
	Position p5(Vect3(Units::from("nmi",2.0), Units::from("nmi",2.0), Units::from("ft",10150)));
	NavPoint np1(p1,0.0);
	//		NavPoint np2(p2,22.43);
	NavPoint np3(p3,27.81);
	//		NavPoint np4(p4,32.04);
	NavPoint np5(p5,50.0);
	NavPoint np2 = np1.linear(np1.initialVelocity(np3),21).mkAlt(Units::from("ft",10000));
	//np2 = np2.makeSourcePosition(np2.position());
	NavPoint np4 = np3.linear(np3.initialVelocity(np5),6).mkAlt(Units::from("ft",10080));
	//np4 = np4.makeSourcePosition(np4.position());
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	//lpc.resetSource(ix2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	//lpc.resetSource(ix5);
	//DebugSupport.dumpPlan(lpc, "testVsWithTurnHard3Vert_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testVsWithTurnHard3Vert_kpc");
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_FALSE(kpc.hasError());
	//fpln(" #### testVsWithTurnHard3Vert: msg = "+kpc.getMessage());
	EXPECT_TRUE(kpc.isFlyable());   // i = 1  Turn section fails TIME test!  dt = 22.26583582139898 turnTime = 21.919693419559156
	if (!kpc.isFlyable()) fpln(" ......... testVsWithTurnHard3Vert needs work on isConsistent");
	//fpln(" kpc.getMessage() = "+kpc.getMessage());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_FALSE(kpc.hasMessage());
	// invariants
	EXPECT_NEAR(lpc.initialVelocity(0).trk(),kpc.initialVelocity(0).trk(),0.01);           // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(3).trk(),kpc.initialVelocity(9).trk(),0.01);           // INVARIANT
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(2).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(3).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(5).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(8).gs(),0.2);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(9).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(8).gs(),0.2);
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(3).vs(),kpc.initialVelocity(9).vs(),0.01);
	EXPECT_NEAR(lpc.point(0).x(),kpc.point(0).x(),0.01);
	EXPECT_NEAR(lpc.point(0).y(),kpc.point(0).y(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).x(),kpc.point(kpc.size()-1).x(),0.01);
	EXPECT_NEAR(lpc.point(lpc.size()-1).y(),kpc.point(kpc.size()-1).y(),0.01);
	for (int i = 0; i < 3; i++) {
		EXPECT_NEAR(lpc.point(0).alt(),kpc.point(i).alt(),0.01);
	}
	for (int i = 6; i < 8; i++) {
		EXPECT_NEAR(lpc.point(2).alt(),kpc.point(i).alt(),0.01);
	}
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);
	//EXPECT_TRUE(plans_almost_equal(lpc,noTCPS));
}






TEST_F(TrajGenTest, testGSC_During_climb) {
	fpln(" ------------------------------ testGSC_During_climb ---------------------------------");
	Position p1(LatLonAlt::make(45.01, -93.31, 10000.0));
	NavPoint np1(p1,80);
	//NavPoint np4(p4,49.21);
	lpc.addNavPoint(np1);
	Velocity v1 = Velocity::makeTrkGsVs(300,450,2000);
	Velocity v2 = Velocity::makeTrkGsVs(300,430,2000);
	Velocity v3 = Velocity::makeTrkGsVs(300,422,2000);
	//fpln(" v = "+v);
	NavPoint np4 = np1.linear(v1,30.0);
	NavPoint np5 = np4.linear(v2,20.0); // .makeTime(132);
	NavPoint np6 = np5.linear(v3,20.0); // makeTime(155);
	//fpln(" np4 = "+np4);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	lpc.addNavPoint(np6);
	//lpc = lpc.makeSpeedConstant(4,6);
	//DebugSupport.dumpPlan(lpc, "testGSC_During_climb_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//		for (int j = 0; j < kpc.size(); j++) {
	//			fpln(" kpc.initialVelocity("+j+").vs() = "+Units::str4("fpm",kpc.initialVelocity(j).vs()));
	//		}
	//DebugSupport.dumpPlan(kpc, "testGSC_During_climb_kpc");
	//fpln("testGSC_During_climb: kpc = "+kpc.toString());
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	std::string msg = kpc.getMessage();
	//fpln("testGSC_During_climb: msg = "+msg);
	EXPECT_FALSE(kpc.hasMessage());
	EXPECT_EQ(5,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isEGS(2));
	for (int ii = 0; ii < 5; ii++) {
		//fpln("  ii = "+ii+" "+lpc.initialVelocity(3));
		//fpln("  ii = "+ii+" "+Units::str("deg",lpc.initialVelocity(1).compassAngle())+" "+Units::str("deg",kpc.initialVelocity(ii).compassAngle()));
		EXPECT_NEAR(lpc.initialVelocity(1).compassAngle(),kpc.initialVelocity(ii).compassAngle(),0.01);
	}
}



TEST_F(TrajGenTest, testDescentOnly) {
	fpln(" ------------------------------ testDescentOnly ---------------------------------");
	Position p7(LatLonAlt::make(34.1626, -118.9911, 5697.2238));
	Position p8(LatLonAlt::make(34.1967, -119.0753, 0.0000));
	Position p9(LatLonAlt::make(34.1987, -119.0802, 0.0000));
	Position p10(LatLonAlt::make(34.2000, -119.0833, 0.0000));
	NavPoint np7(p7,12340.4870);     lpc.addNavPoint(np7);
	NavPoint np8(p8,12435.6536);     lpc.addNavPoint(np8);
	NavPoint np9(p9,12441.5811);     lpc.addNavPoint(np9);
	NavPoint np10(p10,12445.6500);     lpc.addNavPoint(np10);
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testDescentOnly_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, 1.5*gsAccel, 2.0*vsAccel, true, true, true);
	//fpln(" kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testDescentOnly_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isBGS(2));
	EXPECT_TRUE(kpc.isEGS(3));
	EXPECT_TRUE(kpc.isEVS(4));
	// Check Speeds against lpc
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(0).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(1).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.initialVelocity(4).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.initialVelocity(5).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.initialVelocity(6).vs(),0.01);
	//for (int ii = 0; ii < 5; ii++) {
	//fpln("  ii = "+ii+" "+lpc.initialVelocity(3));
	//fpln("  ii = "+ii+" "+Units::str("kn",lpc.initialVelocity(ii).gs())+" "+Units::str("kn",kpc.initialVelocity(ii).gs()));
	//}
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(3).gs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(4).gs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(5).gs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(6).gs(),0.01);
	//EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(7).gs(),0.01);
	//std::string msg = kpc.getMessage();
	//fpln(" #### testDescentOnly: msg = "+msg);
}



TEST_F(TrajGenTest, testAces1) {
	fpln(" ------------------------------ testAces1 ---------------------------------");
	Position p6(LatLonAlt::make(34.0805, -116.6608, 33000.0000));
	//Position p7(LatLonAlt::make(33.7979, -113.5422, 33000.0000));
	Position p8(LatLonAlt::make(33.7968, -113.5312, 32779.4874));
	Position p9(LatLonAlt::make(33.6218, -111.9440, 392.8133));
	Position p10(LatLonAlt::make(33.6174, -111.9056, 0.0000));
	NavPoint np6(p6,12056.9042);     lpc.addNavPoint(np6);
	//NavPoint np7(p7,13258.5000);     lpc.addNavPoint(np7);
	NavPoint np8(p8,13262.8434);     lpc.addNavPoint(np8);
	NavPoint np9(p9,13900.7628);     lpc.addNavPoint(np9);
	NavPoint np10(p10,13916.2372);   lpc.addNavPoint(np10);
	double bank = Units::from("deg", 30);
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testAces1_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, 1.5*gsAccel, 2.0*vsAccel, true, true, true);
	//fpln(" kpc = "+kpc);
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isBGS(2));
	EXPECT_TRUE(kpc.isEGS(3));
	EXPECT_TRUE(kpc.isEVS(4));
	EXPECT_TRUE(kpc.isBVS(5));
	EXPECT_TRUE(kpc.isEVS(6));
	EXPECT_FALSE(kpc.isTCP(7));
	//DebugSupport.dumpPlan(kpc, "testAces1_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//fpln(" lpc2 = "+lpc2);
	//DebugSupport.dumpPlan(lpc2, "testAces1_lpc2");
	//		EXPECT_EQ(4,lpc.size());
	//		EXPECT_EQ(4,lpc2.size());
	//		EXPECT_TRUE(plans_almost_equal(lpc,lpc2));
	//		//std::string msg = kpc.getMessage();
	//		//fpln(" #### testDescentOnly: msg = "+msg);
	//		EXPECT_FALSE(kpc.isTCP(0));
}


TEST_F(TrajGenTest, testAces2) {
	fpln(" ------------------------------ testAces2 ---------------------------------");
	Position p0(LatLonAlt::make(33.9667, -117.6333, 0.0000));
	Position p1(LatLonAlt::make(33.9724, -117.6399, 0.0000));
	Position p2(LatLonAlt::make(34.0357, -117.7120, 6000.0000));
	Position p3(LatLonAlt::make(34.0509, -117.7582, 6000.0000));
	Position p4(LatLonAlt::make(34.0521, -117.7595, 6000.0000));
	Position p5(LatLonAlt::make(34.0774, -117.7882, 8300));
	Position p6(LatLonAlt::make(34.1299, -117.8478, 8300));
	Position p7(LatLonAlt::make(37.5839, -122.0305, 6000.0000));
	Position p8(LatLonAlt::make(37.6445, -122.1095, 0.0000));
	Position p9(LatLonAlt::make(37.6500, -122.1167, 0.0000));
	//		NavPoint np0(p0,11220.0000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,11230.0000);     lpc.addNavPoint(np1);
	NavPoint np2(p2,11340.0000);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11401.0542);     lpc.addNavPoint(np3);
	NavPoint np4(p4,11403.0000);     lpc.addNavPoint(np4);
	NavPoint np5(p5,11446.8730);     lpc.addNavPoint(np5);
	NavPoint np6(p6,11537.5605);     lpc.addNavPoint(np6);
	//		  NavPoint np7(p7,17635.8700);     lpc.addNavPoint(np7);
	//		  NavPoint np8(p8,17745.8700);     lpc.addNavPoint(np8);
	//		  NavPoint np9(p9,17755.8700);     lpc.addNavPoint(np9);
	//fpln(" lpc = "+lpc);
	//		lpc = (Plan) TrajGen::removeExtraAlt0Points(lpc);
	lpc = PlanUtil::mkGsConstant(lpc,0,3);
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testAces2_lpc");
	double bank = Units::from("deg", 30);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testAces2_kpc");
	//fpln(" kpc = "+kpc.toOutput());
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	//fpln(" #### testAces2: msg = "+kpc.getMessage());
	EXPECT_FALSE(kpc.hasError());
	//EXPECT_TRUE(kpc.isConsistent());
	if (!kpc.isFlyable()) fpln(" ......... testAces2 needs work on isConsistent");
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//EXPECT_EQ(16,kpc.size());
	// Check Speeds against lpc
	EXPECT_NEAR(lpc.point(0).alt(),kpc.point(0).alt(),0.01);
	EXPECT_NEAR(lpc.point(1).alt(),kpc.point(5).alt(),0.01);
	EXPECT_NEAR(lpc.point(1).alt(),kpc.point(6).alt(),0.01);
	//EXPECT_NEAR(lpc.point(2).alt(),kpc.point(4).alt(),0.01);
	EXPECT_NEAR(lpc.point(2).alt(),kpc.point(5).alt(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).vs(),kpc.initialVelocity(1).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.initialVelocity(5).vs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).vs(),kpc.initialVelocity(5).vs(),0.01);
}


TEST_F(TrajGenTest, testAces3) {
	fpln(" ------------------------------ testAces3 ---------------------------------");
	lpc.clear();
	Position p0(LatLonAlt::make(33.1510, -96.5918, 0.0000));
	Position p1(LatLonAlt::make(32.9389, -96.7063, 2250));
	Position p2(LatLonAlt::make(32.8608, -96.6529, 3100));
	Position p3(LatLonAlt::make(32.8319, -96.6692, 3400));
	Position p4(LatLonAlt::make(32.0337, -97.1159, 10300));
	Position p5(LatLonAlt::make(30.2895, -98.0876, 24000.0000));
	Position p6(LatLonAlt::make(29.5491, -98.4588, 0.0000));
	NavPoint np0(p0,10990.0000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,11125.5756);     lpc.addNavPoint(np1);
	NavPoint np2(p2,11179.0000);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11197.4445);     lpc.addNavPoint(np3);
	NavPoint np4(p4,11706.0808);     lpc.addNavPoint(np4);
	NavPoint np5(p5,12813.2200);     lpc.addNavPoint(np5);
	NavPoint np6(p6,13283.2200);     lpc.addNavPoint(np6);
	//		  fpln(" lpc = "+lpc);
	//		  PlanCore lpc2 = (PlanCore) TrajGen::removeExtraAlt0Points(lpc);
	//		  fpln(" lpc2 = "+lpc2);
	lpc =  (Plan) PlanUtil::mkGsConstant(lpc);
	//DebugSupport.dumpPlan(lpc, "testAces3_lpc");
	double bank = Units::from("deg", 30);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testAces3_kpc");
	//fpln(" testAces3: kpc = "+kpc.toString());
	fpln(kpc.getMessageNoClear());
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isConsistent());
	EXPECT_TRUE(kpc.isGsContinuous(true));
	EXPECT_TRUE(kpc.isTrkContinuous(true));
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//EXPECT_EQ(12,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBOT(4));
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(2).gs(),0.1);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(3).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(4).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(5).gs(),0.08);
}



TEST_F(TrajGenTest, testAcuteTurn) {
	Position p0  = Position::makeXYZ(0.0000, 0.0000, 10000.0000);
	Position p1  = Position::makeXYZ(9.3979, -3.4205, 10000.0000);
	Position p2  = Position::makeXYZ(0.0000, 10.0000, 10000.0000);
	Position p3  = Position::makeXYZ(10.0000, 20.0000, 10000.0000);
	Position p4  = Position::makeXYZ(17.0000, 22.0000, 10000.0000);
	NavPoint np0(p0,10.0000);      lpc.addNavPoint(np0);
	NavPoint np1(p1,112.8673);     lpc.addNavPoint(np1);
	NavPoint np2(p2,200.0000);     lpc.addNavPoint(np2);
	NavPoint np3(p3,482.0000);     lpc.addNavPoint(np3);
	NavPoint np4(p4,626.0000);     lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc, "testAcuteTurn_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testAcuteTurn_kpc");
	//fpln(" kpc = "+kpc);
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,300));
	EXPECT_EQ(9,kpc.size());
	Plan kpc2 = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc2, "testAcuteTurn_kpc2");
	EXPECT_TRUE(kpc2.hasError());
	EXPECT_TRUE(kpc2.isWellFormed());
	EXPECT_EQ(9,kpc.size());
}


TEST_F(TrajGenTest, testTurnWithRedundant) {
	// For some unknown reason points p2 and p4 are removed
	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",10000)));
	Position p2(Vect3(Units::from("nmi",1.2), Units::from("nmi",0.618), Units::from("ft",10000)));
	Position p3(Vect3(Units::from("nmi",1.49), Units::from("nmi",0.765), Units::from("ft",10000)));
	Position p4(Vect3(Units::from("nmi",1.589), Units::from("nmi",1.0), Units::from("ft",10000)));
	Position p5(Vect3(Units::from("nmi",2.0), Units::from("nmi",2.0), Units::from("ft",10000)));
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,22.45);
	NavPoint np3(p3,27.84);
	NavPoint np4(p4,32.07);
	NavPoint np5(p5,50.0);
	lpc.addNavPoint(np1);   // Add out of order for check
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np5);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np4);
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testTurnWithRedundant_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln(" kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testTurnWithRedundant_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_FALSE(kpc.hasMessage());
	//std::string msg = kpc.getMessage();
	//fpln(" #### testTurnWithRedundant: msg = "+msg);
	EXPECT_EQ(7,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_FALSE(kpc.isTCP(2));
	EXPECT_FALSE(kpc.isTCP(4));
	EXPECT_TRUE(kpc.isEOT(5));
	EXPECT_FALSE(kpc.isTCP(6));
	// invariants
	//EXPECT_NEAR(lpc.point(3).time()-lpc.point(1).time(),kpc.point(4).time()-kpc.point(2).time(),0.01);
	EXPECT_TRUE(lpc.point(0).almostEquals(kpc.point(0)));
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc,false);

	//DebugSupport.dumpPlan(noTCPS, "testTurnWithRedundant_noTCPS");
	//EXPECT_TRUE(plans_almost_equal(lpc,noTCPS));
}




TEST_F(TrajGenTest, testEotAfterBot) {
	Position p0(LatLonAlt::make(6.844333, 3.018931, 5000.000000));
	Position p1(LatLonAlt::make(6.847494, 3.019121, 5000.000000));
	Position p2(LatLonAlt::make(6.867892, 3.014905, 5000.000000));
	Position p3(LatLonAlt::make(6.860902, 3.143400, 5000.000000));
	//Position p4(LatLonAlt::make(6.858433, 3.147735, 5000.000000));
	NavPoint np0(p0,1470.682034);     lpc.addNavPoint(np0);
	NavPoint np1(p1,1474.085862);     lpc.addNavPoint(np1);
	NavPoint np2(p2,1496.468231);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1633.800111);     lpc.addNavPoint(np3);
	//NavPoint np4(p4,1639.132668);     lpc.addNavPoint(np4);
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testEotAfterBot_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	EXPECT_TRUE(kpc.hasError());
	//fpln(" #### test1: msg = "+kpc.getMessage());
	//DebugSupport.dumpPlan(kpc, "testEotAfterBot_kpc");
	EXPECT_TRUE(isVsConstant(kpc));
	//fpln(" kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.hasError());
	//fpln(" #### testEotAfterBot: ERROR IS EXPECTED msg = "+kpc.getMessage());
	EXPECT_TRUE(kpc.isWellFormed());
	//fpln(" lpc.initialVelocity(1) = "+lpc.initialVelocity(1)+" kpc.initialVelocity(3) = "+kpc.initialVelocity(3));
	//		  for (int j = 0; j <= kpc.size()-1; j++) {
	//			  fpln(j+" kpc.finalVelocity(j).trk() = "+Units::str("deg",kpc.finalVelocity(j).trk())+" kpc.initialVelocity(j+1).trk() = "+Units::str("deg",kpc.initialVelocity(j+1).trk()));
	//              fpln(" deltaInOut = "+Units::str("deg",Util::turnDelta(kpc.finalVelocity(j).trk(),kpc.initialVelocity(j+1).trk())));
	//		  }
	//EXPECT_NEAR(-0.202, kpc.finalVelocity(2).trk(),0.001);
	//EXPECT_NEAR(-0.202, kpc.initialVelocity(3).trk(),0.001);
	//EXPECT_EQ(-2,kpc.isSmooth());
	//EXPECT_TRUE(kpc.isConsistent());
}


TEST_F(TrajGenTest, testBug2) {

	lpc.clear();
	Position p0(LatLonAlt::make(3.197509, -0.249489, 20000));
	Position p1(LatLonAlt::make(3.240384, -0.341111, 20000));
	Position p2(LatLonAlt::make(3.338113, -0.349487, 20000));
	Position p3(LatLonAlt::make(3.382948, -0.441520, 20000));
	Position p4(LatLonAlt::make(3.344226, -0.551378, 20000));
	Position p5(LatLonAlt::make(3.377102, -0.618295, 20000));
	NavPoint np0(p0,4411.736037);     lpc.addNavPoint(np0);
	NavPoint np1(p1,4446.073759);     lpc.addNavPoint(np1);
	NavPoint np2(p2,4479.411823);     lpc.addNavPoint(np2);
	NavPoint np3(p3,4514.158581);     lpc.addNavPoint(np3);
	NavPoint np4(p4,4553.688549);     lpc.addNavPoint(np4);
	NavPoint np5(p5,4578.994387);     lpc.addNavPoint(np5);
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testBug2_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	EXPECT_TRUE(kpc.hasError());
	//fpln(" #### testBug2: msg = "+kpc.getMessage());
}





TEST_F(TrajGenTest, testBotOverlapError) {
	Position p0  = Position::makeXYZ(47.0709, 9.7543, 5000.0000);
	Position p1  = Position::makeXYZ(48.4866, 9.1688, 5000.0000);
	Position p2  = Position::makeXYZ(49.8068, 9.7166, 5000.0000);
	Position p3  = Position::makeXYZ(58.7256, 6.0283, 5000.0000);
	Position p4  = Position::makeXYZ(64.5761, 2.5152, 5000.0000);
	Position p5  = Position::makeXYZ(64.9085, 2.3778, 5000.0000);
	NavPoint np0(p0,1098.5391);     lpc.addNavPoint(np0);
	NavPoint np1(p1,1113.4551);     lpc.addNavPoint(np1);
	NavPoint np2(p2,1127.3711);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1221.3384);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1287.7808);     lpc.addNavPoint(np4);
	NavPoint np5(p5,1291.2824);     lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc, "testBotOverlapError_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc, "testBotOverlapError_kpc");
	//fpln(" kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_TRUE(kpc.hasError());
	EXPECT_TRUE(isVsConstant(kpc));
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));

	//fpln(" #### test1: msg = "+kpc.getMessage());
}




TEST_F(TrajGenTest, testNewTurnOverlapsError) {
	fpln(" ------------------------------ test1 ---------------------------------");

	Position p0(LatLonAlt::make(32.8459, -96.8378, 0.0000));
	Position p1(LatLonAlt::make(32.9515, -96.7355, 4257.5342));
	Position p2(LatLonAlt::make(32.9963, -96.7266, 5580.3649));
	Position p3(LatLonAlt::make(33.0061, -96.6825, 6462.2520));
	Position p4(LatLonAlt::make(33.7607, -95.9406, 37000.0000));
	NavPoint np0(p0,10990.0000);     lpc.addNavPoint(np0);
	NavPoint np1 = NavPoint(p1,11074.0000); // .makeMutability(true,true,true);     lpc.addNavPoint(np1);
	NavPoint np2(p2,11100.0991);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11123.6712);     lpc.addNavPoint(np3);
	NavPoint np4(p4,11725.3638);     lpc.addNavPoint(np4);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	EXPECT_FALSE(kpc.hasError());
	//fpln(" #### testNewTurnOverlapsError: msg = "+kpc.getMessage());
	EXPECT_TRUE(kpc.isWellFormed());
	//EXPECT_TRUE(kpc.isConsistent());
	EXPECT_FALSE(kpc.isTCP(0));
	//Plan lpc2 = kpc;
	Plan lpc2 = PlanUtil::revertAllTCPs(kpc,false);
	EXPECT_EQ(4,lpc.size());
	//EXPECT_EQ(3,lpc2.size());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,20));
	//EXPECT_TRUE(plans_almost_equal(lpc,lpc2));

}



TEST_F(TrajGenTest, testLinearMakeGSConstant_0) {
	fpln(" ------------------------------ testLinearMakeGSConstant ---------------------------------");
	lpc.clear();
	Position p0(LatLonAlt::make(-0.407600, 0.934600, 15324.889000));
	Position p1(LatLonAlt::make(-0.407600, 0.934600, 15324.889000));
	Position p2(LatLonAlt::make(-0.407600, 0.934600, 15583.932054));
	Position p3(LatLonAlt::make(-0.407600, 0.934600, 15324.889000));
	Position p4(LatLonAlt::make(-0.407600, 0.934600, 15324.889000));
	NavPoint np0(p0,6212.620000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,6214.530000);     lpc.addNavPoint(np1);
	NavPoint np2(p2,6244.340000);     lpc.addNavPoint(np2);
	NavPoint np3(p3,6511.120000);     lpc.addNavPoint(np3);
	NavPoint np4(p4,6517.120000);     lpc.addNavPoint(np4);
	lpc = PlanUtil::mkGsConstant(lpc,0.0);
	EXPECT_TRUE(lpc.hasError());
	lpc.remove(4);
	lpc.remove(3);
	lpc.remove(1);
	lpc.remove(0);
	lpc = PlanUtil::mkGsConstant(lpc,0.0);
	EXPECT_TRUE(lpc.hasError());
	//		if (lpc.hasError())
	//			fpln(" msg2 = "+lpc.getMessageNoClear());
}



TEST_F(TrajGenTest, testTrkStr) {
	fpln(" ------------------------------ testTrkStr ---------------------------------");
	lpc.clear();
	Position p0(LatLonAlt::make(-0.025242, 1.234852, 6238.486217));
	Position p1(LatLonAlt::make(-0.038966, 1.310627, 6267.692675));
	Position p2(LatLonAlt::make(-0.597800, 1.356914, 6280.250463));
	Position p3(LatLonAlt::make(-0.682634, 1.909756, 6272.784915));
	Position p4(LatLonAlt::make(-0.189723, 2.286265, 6224.421048));
	Position p5(LatLonAlt::make(-0.195201, 2.342467, 6223.555120));
	NavPoint np0(p0,1486.264975);     lpc.addNavPoint(np0);
	NavPoint np1(p1,1520.822114);     lpc.addNavPoint(np1);
	NavPoint np2(p2,1772.457658);     lpc.addNavPoint(np2);
	NavPoint np3(p3,2023.434414);     lpc.addNavPoint(np3);
	NavPoint np4(p4,2301.772078);     lpc.addNavPoint(np4);
	NavPoint np5(p5,2327.112179);     lpc.addNavPoint(np5);
	double bank = Units::from("deg", 30);
	double gsAccel = 2;				// acceptable ground speed acceleration
	double vsAccel = 2; 				// acceptable vertical speed acceleration
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testTrkStr_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, false, false, false);
	//fpln(" testTrkStr: kpc = "+kpc.toString());
	//DebugSupport.dumpPlan(kpc, "testTrkStr_kpc");
	EXPECT_TRUE(kpc.isWellFormed());
	//		if (kpc.hasError())
	//			fpln(" msg = "+kpc.getMessageNoClear());
	EXPECT_TRUE(kpc.isWeakConsistent());
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,50));
	EXPECT_TRUE(kpc.isVelocityContinuous());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBOT(4));
	EXPECT_TRUE(kpc.isEOT(6));
	EXPECT_TRUE(kpc.isBOT(7));
	EXPECT_TRUE(kpc.isEOT(9));
	EXPECT_TRUE(kpc.isBOT(10));
	EXPECT_TRUE(kpc.isEOT(12));
	EXPECT_FALSE(kpc.isTCP(13));
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(0).gs(),kpc.initialVelocity(1).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(3).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(4).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(1).gs(),kpc.initialVelocity(5).gs(),0.01);
	EXPECT_NEAR(lpc.initialVelocity(2).gs(),kpc.initialVelocity(6).gs(),0.01);
}


TEST_F(TrajGenTest, testWild) {   // Non-continuous output
	fpln(" ------------------------------ testWild ---------------------------------");
	lpc.clear();
	Position p0(LatLonAlt::make(29.951867, -95.482999, 4894.444480));
	Position p1(LatLonAlt::make(29.954930, -95.485087, 4725.109608));
	Position p2(LatLonAlt::make(29.971207, -95.496183, 3825.386723));
	Position p3(LatLonAlt::make(30.016293, -95.462726, 1485.731836));
	Position p4(LatLonAlt::make(30.049439, -95.485330, -349.684844));
	Position p5(LatLonAlt::make(30.006854, -95.520499, 1854.732379));
	Position p6(LatLonAlt::make(30.040400, -95.543400, 0.000000));
	NavPoint np0(p0,11250.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,11253.175000);     lpc.addNavPoint(np1);
	NavPoint np2(p2,11270.044651);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11317.874091);     lpc.addNavPoint(np3);
	NavPoint np4(p4,11352.224685);     lpc.addNavPoint(np4);
	NavPoint np5(p5,11398.943515);     lpc.addNavPoint(np5);
	NavPoint np6(p6,11433.719433);     lpc.addNavPoint(np6);
	double bank = Units::from("deg", 30);
	double gsAccel = 2;				// acceptable ground speed acceleration
	double vsAccel = 2; 				// acceptable vertical speed acceleration
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testWild_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, false, false, false);
	//DebugSupport.dumpPlan(kpc, "testWild_kpc");
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(!kpc.isVelocityContinuous());
	//		if (kpc.hasError())
	//			fpln(" msg = "+kpc.getMessageNoClear());
	EXPECT_FALSE(kpc.isFlyable(true)); // ????????????????? NEED TO FIX
}





TEST_F(TrajGenTest, testZeroGs) {
	fpln(" ------------------------------ testZeroGs ---------------------------------");
	lpc.clear();

	Position p0(LatLonAlt::make(0.149510, 0.647977, 4984.040621));
	Position p1(LatLonAlt::make(0.149510, 0.647977, 4984.040621));
	Position p2(LatLonAlt::make(0.066300, 1.712500, 5000.000000));
	NavPoint np0(p0,1041.051169);     lpc.addNavPoint(np0);
	NavPoint np1(p1,1051.051169);     lpc.addNavPoint(np1);
	NavPoint np2(p2,2228.835123);     lpc.addNavPoint(np2);
	double bank = Units::from("deg", 30);
	//DebugSupport.dumpPlan(lpc, "testZeroGs_lpc");
	//fpln(" lpc = "+lpc);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, false, false, false);
	//DebugSupport::dumpPlan(kpc, "testZeroGs_kpc");
	//fpln(" kpc = "+kpc)
	EXPECT_FALSE(kpc.hasError());
	EXPECT_EQ(4,kpc.size());
	//fpln(kpc.getMessageNoClear());
}



TEST_F(TrajGenTest, testFastTm) {
	fpln(" ------------------------------ testFastTm ---------------------------------");
	lpc.clear();
	Position p0(LatLonAlt::make(32.945975, -96.764681, 3553.852644));
	Position p1(LatLonAlt::make(32.909926, -96.645135, 10684.195597));
	Position p2(LatLonAlt::make(32.901018, -96.519529, 17680.872991));
	Position p3(LatLonAlt::make(32.880624, -96.451931, 17584.319449));
	Position p4(LatLonAlt::make(32.171945, -94.299383, 10.419721));
	Position p5(LatLonAlt::make(32.169600, -94.292200, 0.000000));
	NavPoint np0(p0,10996.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,11128.420489);     lpc.addNavPoint(np1);
	NavPoint np2(p2,11259.860327);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11334.770414);     lpc.addNavPoint(np3);
	NavPoint np4(p4,13754.539398);     lpc.addNavPoint(np4);
	NavPoint np5(p5,13762.633136);     lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc, "testFastTm_lpc");
	double bank = Units::from("deg", 30);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, true, true, true);
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isWeakFlyable(true));
	//fpln(" kpc = "+kpc);
	fpln(kpc.getMessageNoClear());
	EXPECT_NEAR(1083.2142858912,lpc.point(0).alt(), 0.001);
	EXPECT_NEAR(3256.5428179656,lpc.point(1).alt(), 0.001);
	EXPECT_NEAR(5389.1300876568,lpc.point(2).alt(), 0.001);
	EXPECT_NEAR(5359.7005680552,lpc.point(3).alt(), 0.001);
	EXPECT_NEAR(3.1759309607999997,lpc.point(4).alt(), 0.001);
	EXPECT_NEAR(0.0,lpc.point(5).alt(), 0.001);
	EXPECT_NEAR(lpc.verticalSpeed(0),kpc.verticalSpeed(0),0.1);
}



TEST_F(TrajGenTest,testMakeKin) {


	Position p0  = Position(LatLonAlt::make(37.019590, -76.591670, 682.549363));
	Position p1  = Position(LatLonAlt::make(37.019768, -76.591651, 681.608421));
	Position p2  = Position(LatLonAlt::make(37.021518, -76.590990, 671.932671));
	Position p3  = Position(LatLonAlt::make(37.023769, -76.590755, 659.980640));
	Position p4  = Position(LatLonAlt::make(37.024100, -76.591200, 658.645300));
	Position p5  = Position(LatLonAlt::make(37.025350, -76.589456, 648.762921));
	NavPoint np0 = NavPoint(p0,2.094000);     lpc.addNavPoint(np0);
	NavPoint np1 = NavPoint(p1,3.374700);     lpc.addNavPoint(np1);
	NavPoint np2 = NavPoint(p2,16.521000);     lpc.addNavPoint(np2);
	NavPoint np3 = NavPoint(p3,32.759000);     lpc.addNavPoint(np3);
	NavPoint np4 = NavPoint(p4,36.248300);     lpc.addNavPoint(np4);
	NavPoint np5 = NavPoint(p5,49.698600);     lpc.addNavPoint(np5);

	double bankAngle = Units::from("deg", 39.99245410013146);
	double gsAccel = 14.71;
	double vsAccel = 4.903325;
	//f.pln(" lpc = "+lpc);
	//DebugSupport::dumpPlan(lpc, "testMakeKin_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	if (kpc.hasError()) fpln(" kpc.getMessage() = "+kpc.getMessageNoClear());
	EXPECT_FALSE(kpc.hasError());
	//f.pln(" kpc = "+kpc);
	//DebugSupport::dumpPlan(kpc, "testMakeKin_kpc");
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable(false));
	//EXPECT_NEAR(10,kpc.size());
}

TEST_F(TrajGenTest, testPathDistance) {
	Position p0 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p1 = Position::makeXYZ(0.0, 10.0, 10000);
	Position p2 = Position::makeXYZ(10.0, 10.0, 10000);
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,200.0);
	NavPoint np2(p2,482.0);
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	double d = lpc.pathDistance(0);
	EXPECT_NEAR(Units::from("nmi",10.0),d,0.001);
	double gs = lpc.initialVelocity(0).gs();
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 20);
	double R = Kinematics::turnRadius(gs, bankAngle);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	double pathD = kpc.pathDistance(1,4);
	EXPECT_NEAR(R*M_PI/2.0,pathD,0.0001);
	// problem 2
	p2 = Position::makeXYZ(10.0, 20.0, 10000);
	np2 = NavPoint(p2,482.0);
	lpc.remove(2);
	lpc.addNavPoint(np2);
	kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	double cd2 = kpc.pathDistance(1,3);
	//fpln(" R = "+Units::str8("NM",R)+" R*pi/2 = "+Units::str8("NM",R*M_PI/2.0)+"  cd2 = "+Units::str8("NM",cd2));
	EXPECT_NEAR(R*M_PI/4.0,cd2,0.0001);
}



TEST_F(TrajGenTest, testRemoveTCPs) {
	double gs = Units::from("kn",520);
	double vs = Units::from("fpm",3000);
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 10);
	LatLonAlt wp1 = LatLonAlt::make(42.00, -94, 500.0);
	LatLonAlt wp2 = LatLonAlt::make(35.00, -95.0, 200.0);
	double cruiseAlt = Units::from("ft",10000);
	//KinematicPlanCore kpc1 = TrajTemplates.makeKPC_Turn(wp1, wp2, gs, vs, cruiseAlt);
	Plan lpc = TrajTemplates::makeLPC_Turn(wp1,wp2,gs,vs,cruiseAlt);
	//fpln(" $$$ lpc = "+lpc.toString());
	//DebugSupport.dumpPlan(lpc, "testRemoveTCPs_lpc", 0.0);
	bool repair = true;
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
}

TEST_F(TrajGenTest, test_gsAnomaly) {  // NEW BUG 12/24/2015
	Plan lpc = Plan("");
	Position p0(LatLonAlt::make(29.742078, -95.280103, 0.000000));
	Position p1(LatLonAlt::make(29.786879, -95.368259, 8000.000018));
	Position p2(LatLonAlt::make(29.786898, -95.368297, 8000.00000));
	Position p3(LatLonAlt::make(29.894799, -95.441600, 8000.000000));
	Position p4(LatLonAlt::make(29.894804, -95.441602, 7974.137188));
	Position p5(LatLonAlt::make(30.048425, -95.524605, 1.000000));
	NavPoint np0(p0,10935.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,11079.93);         lpc.addNavPoint(np1);
	NavPoint np2(p2,11079.994941);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11191.766878);     lpc.addNavPoint(np3);
	NavPoint np4(p4,11191.771138);     lpc.addNavPoint(np4);
	NavPoint np5(p5,11349.967409);     lpc.addNavPoint(np5);
	bool repair = true;
	double gsAccel = 2;
	double vsAccel = 2;
	double bankAngle = Units::from("deg", 30);
	//DebugSupport::dumpPlan(lpc, "test_gsAnomaly_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
}

TEST_F(TrajGenTest, test_gsOut) {
	Plan lpc = Plan("");
	lpc.clear();
	Position p0  = Position(LatLonAlt::make(8.234, 6.752, 5000.000000));
	Position p1  = Position(LatLonAlt::make(8.614, 8.318, 6000.000000));
	Position p2  = Position(LatLonAlt::make(7.981, 8.998, 7000.000000));
	Position p3  = Position(LatLonAlt::make(8.525, 10.161, 4000.000000));
	NavPoint np0 = NavPoint(p0,0.0).makeName("*P0*");    lpc.addNavPoint(np0);
	NavPoint np1 = NavPoint(p1,900.1).makeName("*P1*");    	 lpc.addNavPoint(np1);
	NavPoint np2 = NavPoint(p2,1400.1).makeName("*P2*");    	 lpc.addNavPoint(np2);
	NavPoint np3 = NavPoint(p3,1999.8).makeName("*P3*");    	 lpc.addNavPoint(np3);
	//fpln(" $$$$ lpc = "+lpc.toString());
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg",25);
	bool repair = false;
	//bool repairGs = true;
	//DebugSupport::.dumpPlan(lpc,"test_gsAt_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln(" $$%% kpc.isConsistent() = "+bool2str(kpc.isConsistent()));
	//DebugSupport::dumpPlan(kpc, "test_gsAt_kpc");
		EXPECT_TRUE(kpc.isFlyable());
		EXPECT_NEAR(Units::from("kn",382.764),kpc.gsOut(0),0.001);
		EXPECT_NEAR(Units::from("kn",382.764),kpc.gsOut(1),0.001);
		for (int i = 3; i < 7; i++) {
			//double t = kpc.point(i).time();
			//Velocity v = PlanOld.velocity(kpc,t);
			double gs = kpc.gsOut(i,false);
			//f.pln(i+" ##%% t = "+t+" v = "+v+" gs = "+Units::str("kn",gs,8));
			//assertEquals(v.gs(),gs,0.001);
			EXPECT_NEAR(Units::from("kn",399.0927),gs,0.001);
		}
//	}
}


TEST_F(TrajGenTest, test_2turn_bad) {   // Non-continuous output
	//fpln(" ------------------------------ test_2turn_bad ---------------------------------");
	lpc.clear();
	Position p2(LatLonAlt::make(29.971207, -95.496183, 4000.0));
	Position p3(LatLonAlt::make(30.016293, -95.462726, 4000.0));
	Position p4(LatLonAlt::make(30.049439, -95.485330, 4000.0));
	Position p5(LatLonAlt::make(30.006854, -95.520499, 4000.0));
	NavPoint np2(p2,11270.044651);     lpc.addNavPoint(np2);
	NavPoint np3(p3,11317.874091);     lpc.addNavPoint(np3);
	NavPoint np4(p4,11352.224685);     lpc.addNavPoint(np4);
	NavPoint np5(p5,11398.943515);     lpc.addNavPoint(np5);
	//NavPoint np6(p6,11433.719433);     lpc.addNoTcp(np6);
	double bank = Units::from("deg", 30);
	double gsAccel = 2;				// acceptable ground speed acceleration
	double vsAccel = 2; 				// acceptable vertical speed acceleration
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "test_2turn_bad_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bank, gsAccel, vsAccel, false, false, false);
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,40));
	EXPECT_TRUE(kpc.hasError());
	if (kpc.hasError()) {
		//fpln(" ############ ERROR.msg = "+kpc.getMessageNoClear());
		//DebugSupport.dumpPlan(kpc, "test_2turn_bad_kpc");
		//fpln(" kpc = "+kpc.toStringTrk());
	} else {
		EXPECT_TRUE(kpc.isWellFormed());
	}
	//EXPECT_TRUE(kpc.isConsistent());
}



TEST_F(TrajGenTest, test_pathDistAlternatives) {
	Plan lpc("");
	//fpln(" ------------------------------ test_turnDist ---------------------------------");
	for (double lon2 = -115.8; lon2 < -113.5; lon2 = lon2 + 0.01) {
		lpc.clear();
		Position p1(LatLonAlt::make(26.190, -114.900, 50340.0));
		Position p2(LatLonAlt::make(26.769, lon2, 50340.0));
		Position p3(LatLonAlt::make(27.110, -112.294, 50340.0));
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,481.28);
		NavPoint np3(p3,891.25);
		lpc.addNavPoint(np1);
		lpc.addNavPoint(np2);
		lpc.addNavPoint(np3);
		double bankAngle = Units::from("deg", 20);
		Plan tpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
		NavPoint BOT = tpc.point(1);
		NavPoint EOT = tpc.point(2);
		double R = tpc.signedRadius(1);
		Position center = tpc.turnCenter(1);
		//fpln("2 -------------------------------------------------------------------");
		double pathDist_1_2 = tpc.pathDistance(1,2);       // computed as angleBetween * R from prevBOT
		double angleBetw = PositionUtil::angle_between(BOT.position(), center, EOT.position());
		double arcLength = GreatCircle::small_circle_arc_length(R, angleBetw);
		//fpln(" $$$ arcLength = "+Units::str("NM",arcLength,12));
		//fpln(" $$$ lon2 = "+lon2+" angleBetw = "+Units::str("deg",(angleBetw),12));
		EXPECT_NEAR(pathDist_1_2,arcLength,0.02);
		EXPECT_NEAR(angleBetw*R, arcLength,0.02);
		EXPECT_NEAR(pathDist_1_2, angleBetw*R,0.01);
	}
}



TEST_F(TrajGenTest, test_turnDist) {
	Plan lpc("");
	//bool method2 = false;
	//fpln(" ------------------------------ test_turnDist ---------------------------------");
	int cnt = 0;
	double avgDistH = 0.0;
	for (double lon2 = -115.8; lon2 < -113.5; lon2 = lon2 + 0.01) {
		lpc.clear();
		Position p1(LatLonAlt::make(36.190, -114.900, 50340.0));
		Position p2(LatLonAlt::make(36.769, lon2, 50340.0));
		Position p3(LatLonAlt::make(37.110, -112.294, 50340.0));
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,481.28);
		NavPoint np3(p3,891.25);
		lpc.addNavPoint(np1);
		lpc.addNavPoint(np2);
		lpc.addNavPoint(np3);
		double bankAngle = Units::from("deg", 20);
		Plan tpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
		//if (tpc.hasError()) //fpln(" test_turnDist: lon2 = "+lon2+" tpc = "+tpc.getMessageNoClear());
		//fpln(" $$$$ lon2 = "+lon2+" tpc = "+tpc);
		//DebugSupport.dumpPlan(lpc,"test_turnDist_lpc");
		//DebugSupport.dumpPlan(tpc,"test_turnDist_tpc");
		NavPoint BOT = tpc.point(1);
		NavPoint EOT = tpc.point(2);
		Position center = tpc.turnCenter(1);
		double signedRadius = tpc.signedRadius(1);
		//fpln(" $$ signedRadius = "+Units::str("NM",signedRadius,12)+" delta radius = "+Units::str("NM",BOT.position().distanceH(center)-signedRadius,12));
		double theta = PositionUtil::angle_between(BOT.position(), center, EOT.position());
		//fpln(" $$ pathDistance: R = "+Units::str("NM",R,12)+ "  theta="+Units::str("deg",theta,12)+" bot="+bot+" center="+center);
		double pathDist = theta * tpc.signedRadius(1);
		double altRadius = BOT.position().distanceH(center);
		pathDist = theta * altRadius;
		double gsAt_d = 100;
		int dir = Util::sign(signedRadius);
		std::pair<Position,Velocity> tAtd = KinematicsPosition::turnByDist2D(BOT.position(), center, dir, pathDist, gsAt_d);
		Position EOTcalc = (tAtd.first).mkAlt(EOT.alt());// TODO: should we test altitude?
		//fpln(" >>> turnPrint i = "+f.padLeft(1+"", 2)+" calculated pos = "+EOTcalc.toString(12));
		//+ "\n                            plan EOT = "+EOT.position().toString(12));
		//double distanceH = EOT.position().distanceH(EOTcalc);
		//double distanceV = EOT.position().distanceV(EOTcalc);
		//fpln("            .... distanceH = "+Units::str("m",distanceH,12));
		//fpln("            .... distanceV = "+Units::str("m",distanceV,12));
		//			double centerAcc = PlanUtil::centerAccuracy(BOT.position(),EOT.position(),center);
		//			EXPECT_TRUE(distanceH < 0.05);  // 0.08  without altRadius
		//			EXPECT_TRUE(centerAcc < 0.05);
		//			EXPECT_TRUE(distanceV < 0.000001);
		//			avgDistH = avgDistH + distanceH;
		cnt++;
	}
	avgDistH = avgDistH/cnt;
	//fpln(" $$test_turnDist: "+cnt+" tests completed: avgDistH = "+avgDistH);
}



TEST_F(TrajGenTest, test_turnDist2) {
	//fpln(" ------------------------------ test_turnDist2 ---------------------------------");
	Plan lpc("");
	int cnt = 0;
	double avgDistH = 0.0;

	//double lat3 = 36.1; {
	for (double lat3 = 36.1; lat3 < 37.2; lat3 = lat3 + 0.01) {
		lpc.clear();
		Position p1(LatLonAlt::make(36.200, -114.900, 50340.0));
		Position p2(LatLonAlt::make(36.770, -113.500, 50340.0));
		Position p3(LatLonAlt::make(lat3, -112.200, 50340.0));
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,481.28);
		NavPoint np3(p3,891.25);
		lpc.addNavPoint(np1);
		lpc.addNavPoint(np2);
		lpc.addNavPoint(np3);
		double bankAngle = Units::from("deg", 20);
		//fpln("0 -------------------------------------------------------------------");
		Plan tpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
		//fpln(" $$$$ lat3 = "+lat3+" tpc = "+tpc);
		//DebugSupport.dumpPlan(tpc,"test_turnDist2");
		//fpln("1 -------------------------------------------------------------------");
		tpc.remove(2);  // MOT
		//PlanUtil::turnPrint(tpc,1);
		NavPoint BOT = tpc.point(1);
		NavPoint EOT = tpc.point(2);
		Position center = tpc.turnCenter(1);
		//fpln(" $$ test_turnDist2: center = "+center);
		double signedRadius = tpc.signedRadius(1);
		//fpln(" $$ signedRadius = "+Units::str("NM",signedRadius,12)+" delta radius = "+Units::str("m",BOT.position().distanceH(center)-signedRadius,12));
		double theta = PositionUtil::angle_between(BOT.position(), center, EOT.position());
		//fpln(" $$ pathDistance: R = "+Units::str("NM",R,12)+ "  theta="+Units::str("deg",theta,12)+" bot="+bot+" center="+center);
		//fpln(" $$$ lat3 = "+lat3+" theta = "+Units::str("deg",(theta),12));
		double pathDist = theta * tpc.signedRadius(1);

		if (KinematicsLatLon::chordalSemantics) {
		} else {
			signedRadius = BOT.position().distanceH(center);
		}
		pathDist = theta * signedRadius;
		double gsAt_d = 100;
		int dir = Util::sign(signedRadius);
		std::pair<Position,Velocity> tAtd = KinematicsPosition::turnByDist2D(BOT.position(), center, dir, pathDist, gsAt_d);
		Position EOTcalc = (tAtd.first).mkAlt(EOT.alt());// TODO: should we test altitude?
		//fpln(" >>> turnPrint i = "+f.padLeft(1+"", 2)+" calculated pos = "+EOTcalc.toString(12)
		//+ "\n                            plan EOT = "+EOT.position().toString(12));
		double distanceH = EOT.position().distanceH(EOTcalc);
		//double distanceV = EOT.position().distanceV(EOTcalc);
		//fpln("            .... distanceH = "+Units::str("m",distanceH,12));
		//fpln("            .... distanceV = "+Units::str("m",distanceV,12));
		//			double centerAcc = PlanUtil::centerAccuracy(BOT.position(),EOT.position(),center);
		//			EXPECT_NEAR(0.0,distanceH,1E-5);
		//			EXPECT_TRUE(centerAcc < 0.0000001);
		//			EXPECT_TRUE(distanceV < 0.000001);
		cnt++;
		avgDistH = avgDistH + distanceH;
	}
	avgDistH = avgDistH/cnt;
	//fpln(" $$test_turnDist2: "+cnt+" tests completed:  avgDistH = "+avgDistH);
}



TEST_F(TrajGenTest, testGen1) {
	Position p0(LatLonAlt::make(-1.184546, -0.431315, 31146.578405));
	Position p1(LatLonAlt::make(-1.086379, -0.383909, 31207.867663));
	Position p2(LatLonAlt::make(0.629428, 0.257121, 32233.286500));
	Position p3(LatLonAlt::make(3.618800, 1.674700, 31889.132200));
	NavPoint np0(p0,3691.301500);     lpc.addNavPoint(np0);
	NavPoint np1(p1,3736.467000);     lpc.addNavPoint(np1);
	NavPoint np2(p2,4495.351000);     lpc.addNavPoint(np2);
	NavPoint np3(p3,5865.912500);     lpc.addNavPoint(np3);
	//DebugSupport.dumpPlan(lpc, "testMakeKin_lpc.txt");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//if (kpc.hasError()) //fpln(" kpc.getMessage() = "+kpc.getMessageNoClear());
	EXPECT_FALSE(kpc.hasError());
	//fpln(" testGen1: kpc = "+kpc);
	//DebugSupport.dumpPlan(kpc, "testMakeKin_kpc.txt");
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_EQ(8,kpc.size());
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
}


TEST_F(TrajGenTest, testGen2) {
	Position p0(LatLonAlt::make(0.928270, -9.323164, 19820.35));
	Position p1(LatLonAlt::make(0.925604, -9.315217, 19820.35));
	Position p2(LatLonAlt::make(0.923658, -9.309115, 19805));
	Position p3(LatLonAlt::make(0.921771, -9.303790, 19820.35));
	Position p4(LatLonAlt::make(0.191862, -7.105696, 14388.31));

	NavPoint np0(p0,28.859388);     lpc.addNavPoint(np0);
	NavPoint np1(p1,32.034388);     lpc.addNavPoint(np1);
	NavPoint np2(p2,34.460614); np2=np2.makeName("P2");     lpc.addNavPoint(np2);
	NavPoint np3(p3,36.600287); np3=np3.makeName("P3******");     lpc.addNavPoint(np3);
	NavPoint np4(p4,913.952093);     lpc.addNavPoint(np4);
	double gsAccel = 2;
	double vsAccel = 2;
	double bankAngle = Units::from("deg", 25);
	//DebugSupport.dumpPlan(lpc, "testGen2_lpc.txt");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//if (kpc.hasError()) //fpln(" kpc.getMessage() = "+kpc.getMessageNoClear());
	//EXPECT_TRUE(kpc.hasError());
	//fpln(" testGen2: kpc = "+kpc.toStringTrk());
	//DebugSupport.dumpPlan(kpc, "testGen2_kpc.txt");
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	//EXPECT_EQ(8,kpc.size());
	//Plan noTCPS = kpc;
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));
}




TEST_F(TrajGenTest, testReRouteCase4) {
	Position p0(LatLonAlt::make(-42.819600, -76.932900, 31396.659200));
	Position p1(LatLonAlt::make(-42.846284, -76.993029, 31387.138937));
	Position p2(LatLonAlt::make(-42.679618, -77.159697, 31387.138937));
	Position p3(LatLonAlt::make(-42.679618, -77.326363, 31387.138937));
	Position p4(LatLonAlt::make(-42.512951, -77.493031, 31387.138937));
	NavPoint np0(p0,842.108300);     lpc.addNavPoint(np0);
	NavPoint np1(p1,872.108300);     lpc.addNavPoint(np1);
	NavPoint np2(p2,992.457401);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1063.775799);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1184.238131);     lpc.addNavPoint(np4);
	bool repair = false;  // otherwise might eliminate the initial point
	bankAngle = Units::from("deg", 25);
	gsAccel = 2;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "testReRouteCase4_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc, "testReRouteCase4_kpc");
	//fpln(" testReRouteCase4: kpc = "+kpc);
}





TEST_F(TrajGenTest, test4) {
	//fpln(" ------------------------------ test4 ---------------------------------");
	Position p1(LatLonAlt::make(41.33, -92.32, 11000));
	Position p2(LatLonAlt::make(42.76, -92.04, 11000));
	Position p3(LatLonAlt::make(43.319, -90.214,   11000));
	NavPoint np1(p1,0);     lpc.addNavPoint(np1);
	NavPoint np2(p2,700);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1400);     lpc.addNavPoint(np3);
	//fpln(" lpc = "+lpc);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	EXPECT_TRUE(kpc.isWellFormed());
	// EXPECT_TRUE(kpc.isConsistent());                       // TODO $$$$$$$$$$$$$$$$$$$$$ UNCOMMENT THIS $$$$$$$$$$$$$$$$$$$$$$$$$$
	EXPECT_FALSE(kpc.isTCP(0));
	//fpln(" lpc2 = "+lpc2);
	EXPECT_EQ(3,lpc.size());
	double curTime = 1300.0;
	Position curPos = lpc.position(curTime);
	NavPoint curNp = NavPoint(curPos,curTime);
	curNp = curNp.makeName("curNp");
	NavPoint closestPoint = kpc.closestPoint(curPos).makeName("closestPoint");
	lpc.addNavPoint(curNp);
	Plan kpc2 = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//int ix_kpc_CurNp = kpc2.findName("curNp");
	//double tm_kpc_CurNp = kpc2.point(ix_kpc_CurNp).time();
	kpc.addNavPoint(closestPoint);
}





TEST_F(TrajGenTest, test_notSmallDeltaTrk) {
	lpc.clear();
	Position p0(LatLonAlt::make(57.878008, -0.375143, 5000.000000));
	Position p1(LatLonAlt::make(57.896765, 1.931988, 5000.000000));
	Position p2(LatLonAlt::make(56.952234, 3.887413, 5000.000000));
	NavPoint np0(p0,0.000000);  	 lpc.addNavPoint(np0);
	NavPoint np1(p1,15163);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,32680);    	 lpc.addNavPoint(np2);
	//fpln(" ## test_notSmall lpc = "+lpc.toStringGs());
	bankAngle = Units::from("deg", 25);
	gsAccel = 2;
	vsAccel = 1;
	double trkIn = lpc.finalVelocity(0).trk();
	double trkOut = lpc.initialVelocity(1).trk();
	double deltaTrack = Util::turnDelta(trkIn,trkOut);
	//fpln(" ##$ test_notSmall.deltaTrack = "+Units::str("deg",deltaTrack));
	EXPECT_NEAR(Units::from("deg",40.97),deltaTrack,0.001);
	double turnTime = Kinematics::turnTime(trkIn, trkOut, bankAngle);
	EXPECT_NEAR(0.786,turnTime,0.001);
	//DebugSupport.dumpPlan(lpc, "test_notSmall_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc, "test_notSmall_kpc");
	//fpln(" $$ test_notSmallDeltaTrk: kpc = "+kpc);
	int ix = kpc.nextEOT(0);
	EXPECT_EQ(3,ix);   // no EOT
}



TEST_F(TrajGenTest, test_AA) {
	double startTime = 1000;
	GsPlan pp =  GsPlan(startTime);
	Position p0(LatLonAlt::make(57.878008, -0.375143, 5000.000000));
	Position p1(LatLonAlt::make(57.896765, 1.931988, 6000.000000));
	Position p2(LatLonAlt::make(56.952234, 3.887413, 6000.000000));
	double gs = Units::from("kn",400);
	pp.add(p0,"Point0","",gs);
	pp.add(p1,"Point1","",gs);
	pp.add(p2,"Point2","",gs);
	lpc = pp.linearPlan();
	//fpln(" ## test_notSmall lpc = "+lpc.toStringGs());
	NavPoint vp1(lpc.position(1500),1500); vp1=vp1.mkAlt(Units::from("ft",6000)).makeName("VP1");
	NavPoint vp2(lpc.position(1760),1760); vp2=vp2.makeName("VP2");
	//fpln(" ##$ vp1 = "+vp1+" vp2 = "+vp2);
	lpc.addNavPoint(vp1);
	lpc.addNavPoint(vp2);
	//fpln(" ## test_notSmall lpc = "+lpc.toStringGs());
	bankAngle = Units::from("deg", 25);
	gsAccel = 2;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "test_AA_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc, "test_AA_kpc");
	if (kpc.hasError()) {
		//EXPECT_TRUE(" kpc.getMessageNoClear = "+kpc.getMessageNoClear());
	} else {
		EXPECT_TRUE(kpc.isWeakFlyable());           // TODO:  why not fully consistent?
		for (double t = 1500; t < kpc.getLastTime(); t = t + 100) {
			//fpln(" ##$ test_AA: kpc.position(t).alt() = "+Units::str("ft",kpc.position(t).alt()));
			EXPECT_TRUE(kpc.position(t).alt() > Units::from("ft",5900));
		}
	}
}




TEST_F(TrajGenTest, testBug2016) {  // NEWlY DISCOVERED TrajGen ERROR -- NOT YET FIXED
	//fpln(" ------------------------------ testBug2016 ---------------------------------");
	Position p0  = Position::makeXYZ(0.000000, 0.000000, 10000.000000);
	Position p1  = Position::makeXYZ(10.000000, 1.500000, 10000.000000);
	Position p2  = Position::makeXYZ(0.000000, 3.000000, 10000.000000);
	Position p3  = Position::makeXYZ(0.000000, 7.000000, 10000.000000);
	Position p4  = Position::makeXYZ(10.000000, 7.000000, 10000.000000);
	NavPoint np0(p0,0.000000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,202.237484);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,404.474968);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,504.474968);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,604.474968);    	 lpc.addNavPoint(np4);
	lpc = PlanUtil::mkGsConstant(lpc,Units::from("kn", 220));;            // below 208 kn the bug disappears
	//DebugSupport.dumpPlan(lpc, "testBug2016_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln(" ##$$ kpc.getMessageNoClear() = "+kpc.getMessageNoClear());
	//DebugSupport.dumpPlan(kpc, "testBug2016_kpc");
	if (! kpc.hasError()) {
		EXPECT_TRUE(kpc.isFlyable());
	}
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,300));
}



TEST_F(TrajGenTest, test_ex50) {    // used in EUTL paper
	Plan lpc("_");
	Position p2(LatLonAlt::make(41.690007, -72.573280, 5000.0));
	Position p3(LatLonAlt::make(41.641105, -72.547418, 6000.0));
	Position p4(LatLonAlt::make(41.678012, -72.441028, 6000.0));
	NavPoint np2(p2,36130.0);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,36176.3);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,36245.4);    	 lpc.addNavPoint(np4);
	double gsAccel = 4;
	double vsAccel = 2;
	double bankAngle = Units::from("deg",25);
	bool repair = false;
	//DebugSupport.dumpPlan(lpc,"test_ex50_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc,"test_ex50_kpc");
	//fpln(" ## testUCA5008_example: kpc = "+kpc);
	for (int i = 0; i < kpc.size(); i++) {
		//fpln(" ##$$ testUCA5008_altCrazy: time="+Units::str("s",kpc.point(i).time(),0)+" kpc.point(i).alt() = "+ Units::str("ft",kpc.point(i).alt(),2));
		EXPECT_TRUE(kpc.point(i).alt() < Units::from("ft",7000));
	}
	EXPECT_FALSE(kpc.hasMessage());
	EXPECT_TRUE(kpc.isFlyable());

	// show me some data for MATLAB plotting
	// linear plan
	//fpln("\n% Linear Plan Data:");
	//fpln("% time(sec),lat(deg),lon(deg),alt(ft),gs(kts)");
	//fpln("lpc = [");
	for (int i=0; i<lpc.size(); i++) {
		Position p = lpc.getPos(i);
		Velocity v = lpc.averageVelocity(i);
		//fpln(lpc.time(i)+","+p.latitude()+","+p.longitude()+","+p.altitude()+","+v.groundSpeed("kts"));
	}
	//fpln("];");
}


TEST_F(TrajGenTest, testmakeKinematicPlan_withRadius) {
	double firstRadius = Units::from("NM",3.1);
	double secondRadius = Units::from("NM",4.2);
	Position p0(LatLonAlt::make(39.99, 100.593, 20000.000000));
	Position p1(LatLonAlt::make(40.02074, 100.613338, 20000.000000));
	Position p2(LatLonAlt::make(40.03072, 100.701749, 18957.666667));
	Position p3(LatLonAlt::make(40.04647, 101.327707, 18957.666667));
	NavPoint np0(p0,331.18);
	np0 = np0.makeName("isolationStart");    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,363.56);
	TcpData tcp1 = TcpData().setRadiusSigned(firstRadius);
	lpc.add(np1,tcp1);
	NavPoint np2(p2,427.85);
	TcpData tcp2 = TcpData().setRadiusSigned(secondRadius);
	lpc.add(np2,tcp2);
	NavPoint np3(p3,876.27);    	 lpc.addNavPoint(np3);
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg",25);
	bool repair = false;
	//DebugSupport.dumpPlan(lpc,"withRadius_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//DebugSupport.dumpPlan(kpc,"withRadius_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
}


TEST_F(TrajGenTest, test_cont_small_turn) {
	Plan lpc("foo");
	//	Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899.410000));
	//	Position p1(LatLonAlt::make(-1.015100, -1.100000, 7899.410000));
	Position p2(LatLonAlt::make(-1.188000, -0.370300, 7899.410000));
	Position p3(LatLonAlt::make(-1.549400, 0.351100, 3983.877000));
	Position p4(LatLonAlt::make(-1.845700, 0.899500, 3983.877000));
	//	NavPoint np0(p0,1.000000);       lpc.addNoTcp(np0);
	//	NavPoint np1(p1,403.897800);     lpc.addNoTcp(np1);
	NavPoint np2(p2,769.147300);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1133.991300);    lpc.addNavPoint(np3);
	NavPoint np4(p4,1461.597000);    lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc,"test_cont_lpc");
	double  gsAccel = 4;                                 // ground speed acceleration
	double  vsAccel = 2;		                         // vertical speed acceleration
	double  bankAngle = Units::from("deg", 25.0);         // bank angle of aircraft in turn
	bool repairTurn = false;
	//bool repairGs = false;
	bool repairGs = false;
	bool repairVs = false;
	//	GsMode gsm = GsMode.PRESERVE_GS;
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, repairGs, repairVs);
	//DebugSupport.dumpPlan(kpc,"test_cont_kpc");gsAt
	EXPECT_TRUE(kpc.isFlyable());
}



TEST_F(TrajGenTest, testAAL1851) {
	lpc.clear();
	Position p8(LatLonAlt::make(40.985330, -72.578243, 14000.000000));
	Position p9(LatLonAlt::make(40.929618, -72.798859, 12416.939813));
	Position p10(LatLonAlt::make(40.900369, -72.826977, 12014.000000));
	Position p11(LatLonAlt::make(40.685464, -73.032611, 8192.695136));
	NavPoint np8(p8,5668);  np8=np8.makeName("$FPA");    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,5768.12); np9=np9.makeName("CCC");    	 lpc.addNavPoint(np9);
	NavPoint np10(p10,5792.29); np10=np10.makeName("$descentSpeedChangePoint");    	 lpc.addNavPoint(np10);
	NavPoint np11(p11,6021.57); np11=np11.makeName("ROBER");    	 lpc.addNavPoint(np11);
	//fpln(" ##$$ lpc = "+lpc.toStringGs());
	bool repairTurn = false;
	//bool repairGs = true;
	bool repairVs = false;
	bankAngle = Units::from("deg", 25);
	gsAccel = 4;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "testAAL1851_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//DebugSupport.dumpPlan(kpc, "testAAL1851_kpc");
	//fpln(" ##$$ kpc = "+kpc.toString());
	if (kpc.hasError()) {  // overlap problem
		//fpln(" ##$$ testAAL1851. msg ="+kpc.getMessageNoClear());
	}
	EXPECT_TRUE(kpc.isFlyable());
}




TEST_F(TrajGenTest, testJBU173) {
	lpc.clear();
	Position p0(LatLonAlt::make(38.695444, -121.590778, 27.000000));
	Position p1(LatLonAlt::make(38.475557, -121.498233, 10027.000000));
	Position p2(LatLonAlt::make(38.445225, -121.485514, 10999.611236));
	Position p3(LatLonAlt::make(38.517547, -121.198058, 16561.014416));
	NavPoint np0(p0,96360.0); np0=np0.makeName("KSMF");    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,96560.0); np1=np1.makeName("$ascentSpeedChangePoint");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,96579.4); np2=np2.makeName("LOGYE");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,96690.6); np3=np3.makeName("SHELD");    	 lpc.addNavPoint(np3);
	//fpln(" ##$$ lpc = "+lpc.toStringGs());
	bool repairTurn = false;
//	bool repairGs = true;
	bool repairVs = false;
	bankAngle = Units::from("deg", 25);
	gsAccel = 4;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "testJBU173_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//DebugSupport.dumpPlan(kpc, "testJBU173_kpc");
	//fpln(" ##$$ kpc = "+kpc.toString());
	EXPECT_FALSE(kpc.hasError());          // before bug fix this was false
	if (!kpc.hasError() && ! kpc.isWeakFlyable()) {
		//fpln(" #### testJBU173: "+kpc.getName()+" is not consistent!");
	}
	EXPECT_EQ(8,kpc.size());
	lpc.setNavPoint(0,np0.makeTime(96420));
	//fpln(" ##$$ lpc = "+lpc.toStringGs());
	Plan kpc2 = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//DebugSupport.dumpPlan(kpc2, "testJBU173_kpc2");
	if (kpc2.hasError()) {
		//fpln(" ##$$ testJBU173. msg ="+kpc2.getMessageNoClear());
	}
	//EXPECT_TRUE(kpc2.isWeakConsistent());
}



TEST_F(TrajGenTest, test_SWA2013) {
	lpc.clear();
	Position p13(LatLonAlt::make(33.054156, -113.368064, 27000.000000));
	Position p14(LatLonAlt::make(33.274306, -113.069672, 27000.000000));
	Position p15(LatLonAlt::make(33.275346, -113.029335, 27000.000000));
	Position p16(LatLonAlt::make(33.280497, -112.821597, 22801.069351));
	NavPoint np13(p13,76051.067618); np13=np13.makeName("RKDAM");    	 lpc.addNavPoint(np13);
	NavPoint np14(p14,76217.544776); np14=np14.makeName("HYDRR");    	 lpc.addNavPoint(np14);
	NavPoint np15(p15,76234.414834); np15=np15.makeName("$endCruisePoint");    	 lpc.addNavPoint(np15);
	NavPoint np16(p16,76321.289261); np16=np16.makeName("GEELA");    	 lpc.addNavPoint(np16);
	//fpln(" ##$$ lpc = "+lpc.toStringGs());
	bool repairTurn = false;
//	bool repairGs = true;
	bool repairVs = false;
	//GsMode gsm = GsMode.PRESERVE_GS;
	bankAngle = Units::from("deg", 25);
	gsAccel = 4;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "test_SWA2013_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//DebugSupport.dumpPlan(kpc, "test_SWA2013_kpc");
	//fpln(" ##$$ kpc = "+kpc.toString());
	if (kpc.hasError()) {
		//fpln(" $$$$ test_SWA2013. msg ="+kpc.getMessageNoClear());
	}
	if (!kpc.hasError() && ! kpc.isWeakFlyable()) {
		//fpln(" $$## test_SWA2013: "+kpc.getName()+" is not consistent!");
	}
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isFlyable());
	//EXPECT_EQ(6,kpc.size());
}




TEST_F(TrajGenTest, test_AWE918) {
	lpc.clear();
	Position p36(LatLonAlt::make(40.665661, -75.148794, 17418.642867));
	Position p37(LatLonAlt::make(40.569197, -75.011478, 14152.792625));
	Position p38(LatLonAlt::make(40.567577, -75.006031, 14021.000000));
	Position p39(LatLonAlt::make(40.496001, -74.766590, 12021.000000));
	NavPoint np36(p36,80024.04); np36=np36.makeName("LIZZI");    	 lpc.addNavPoint(np36);
	NavPoint np37(p37,80091.61); np37=np37.makeName("BEUTY");    	 lpc.addNavPoint(np37);
	NavPoint np38(p38,80093.73); np38=np38.makeName("$FPA");    	 lpc.addNavPoint(np38);
	NavPoint np39(p39,80186);    np39=np39.makeName("$desc");    	 lpc.addNavPoint(np39);
	//fpln(" $$$$ lpc = "+lpc.toStringVs());
	bool repairTurn = false;
	bool repairVs = false;
	bankAngle = Units::from("deg", 25);
	gsAccel = 4;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "test_AWE918_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//fpln(" $$$$ kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.hasError());
	if (kpc.hasError()) {
//		fpln(" $$$$ test_AWE918. msg ="+kpc.getMessageNoClear());
	}
	//EXPECT_TRUE(kpc.hasError());          // before bug fix this was false
	if (!kpc.hasError() && ! kpc.isWeakFlyable()) {
		//fpln(" $$## test_AWE918: "+kpc.getName()+" is not consistent!");
	}
	EXPECT_EQ(7,kpc.size());
	bool continueGen = true;
	//fpln("\n ----------------------------------------");
	kpc = TrajGen::generateVsTCPs(lpc,vsAccel,continueGen);
	//DebugSupport.dumpPlan(kpc, "test_AWE918_kpc");
	//fpln(" $$$$ kpc = "+kpc.toString());
	EXPECT_FALSE(kpc.hasError());
}


TEST_F(TrajGenTest, test_T094) {
	lpc.clear();
	Position p0(LatLonAlt::make(2.152600, 5.247200, 7916.425100));
	Position p1(LatLonAlt::make(0.957900, 6.174000, 7916.425100));
	Position p2(LatLonAlt::make(-1.846300, 7.193700, 342.343300));
	Position p3(LatLonAlt::make(-4.053300, 8.029800, 16144.550500));
	Position p4(LatLonAlt::make(-5.828700, 9.833100, 22688.465900));
	//		Position p5(LatLonAlt::make(-6.294900, 11.746500, 22688.465900));
	//		Position p6(LatLonAlt::make(-6.325700, 13.821700, 23636.183700));
	NavPoint np0(p0,0.000000);    	    lpc.addNavPoint(np0);
	NavPoint np1(p1,695.943700);    	     lpc.addNavPoint(np1);
	NavPoint np2(p2,2378.534100);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3525.191700);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,4744.077500);    	 lpc.addNavPoint(np4);
	//		NavPoint np5(p5,5693.497100);    	 lpc.addNavPoint(np5);
	//		NavPoint np6(p6,6760.521500);    	 lpc.addNavPoint(np6);

	//fpln(" $$$$ lpc = "+lpc.toStringGs());
	bool repairTurn = false;
	bool repairVs = false;
	//GsMode gsm = GsMode.PRESERVE_GS;
	bankAngle = Units::from("deg", 25);
	gsAccel = 4;
	vsAccel = 1;
	//DebugSupport.dumpPlan(lpc, "test_T094_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//DebugSupport.dumpPlan(kpc, "test_T094_kpc");
	//fpln(" $$$$ test_T094: kpc = "+kpc.toStringGs());
	if (kpc.hasError()) {
		//fpln(" $$$$ test_T094. msg ="+kpc.getMessageNoClear());
	}
	EXPECT_FALSE(kpc.hasError());          // before bug fix this was false
	EXPECT_TRUE(kpc.isFlyable(false));
}



TEST_F(TrajGenTest, testmakeKinematicPlan_shortGs) {
	lpc.clear();
	Position p7(LatLonAlt::make(35.187833, -85.729333, 24000.000000));
	Position p8(LatLonAlt::make(35.552589, -85.910511, 12262.292063));
	Position p9(LatLonAlt::make(35.568993, -85.932425, 12000.000000));
	NavPoint np7(p7,11741.221453); np7=np7.makeName("N351127W0854376");    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,11946.051308); np8=np8.makeName("SWFFT");    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,11961.788832); np9=np9.makeName("DescentSpeedChangePoint");    	 lpc.addNavPoint(np9);
	//fpln(" $$$$ lpc = "+lpc.toStringGs());
	double gsAccel = 4;
	double vsAccel = 1;
	double bankAngle = Units::from("deg",25);
	bool repair = false;
	//DebugSupport.dumpPlan(lpc,"shortGs_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln(" $$$$ kpc.getMessageNoClear() = "+kpc.getMessageNoClear());
	//DebugSupport.dumpPlan(kpc,"shortGs_kpc");
}





TEST_F(TrajGenTest, test_T004) {	// **NEW**
	bankAngle = Units::from("deg", 25);
	gsAccel = 2;
	vsAccel = 1;

	lpc.clear();
	Position p0  = Position::makeXYZ(44.836702, 10.678188, 5000.000000);
	Position p1  = Position::makeXYZ(45.879466, 10.246969, 5000.000000);
	Position p2  = Position::makeXYZ(46.901958, 10.328036, 5000.000000);
	Position p3  = Position::makeXYZ(57.667782, 5.875989, 5000.000000);
	Position p4  = Position::makeXYZ(85.555406, -9.512914, 5000.000000);
	Position p5  = Position::makeXYZ(87.900000, -7.130000, 5000.000000);
	NavPoint np0(p0,1075.000000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1085.986447);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1095.972893);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1209.400393);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,1519.516731);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,1552.064563);    	 lpc.addNavPoint(np5);
	//fpln(" #### lpc = "+lpc.toStringGs());
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg",25);
	bool repair = false;
	//DebugSupport.dumpPlan(lpc,"test_gsAt_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln(" ## test_T004: kpc = "+kpc);
	EXPECT_TRUE(kpc.hasError());
	//EXPECT_TRUE(kpc.isFlyable());
	//DebugSupport.dumpPlan(kpc, "test_gsAt_kpc");
	//		lpc = TrajGen::removeInfeasibleTurns(lpc, bankAngle);
	//		//fpln(" #### lpc = "+lpc.toStringGs());
	//		kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//		//fpln(" ## test_T004: kpc = "+kpc);
	//		EXPECT_TRUE(kpc.isFlyable());
}



TEST_F(TrajGenTest, testAltPreserveFun) {   // NEW -- needs new MarkVs
	Plan lpc;
	Position p0(LatLonAlt::make(2.040, 5.780, 20000));
	Position p1(LatLonAlt::make(0.957, 6.174, 21000));
	Position p2(LatLonAlt::make(-1.846, 7.193,21000));
	Position p3(LatLonAlt::make(-3.407, 7.756, 20000));
	NavPoint np0(p0,0.000000);    lpc.addNavPoint(np0);
	NavPoint np1(p1,700);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,2500);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3500);    	 lpc.addNavPoint(np3);
	NavPoint np4(lpc.position(690), 690);         lpc.addNavPoint(np4);
	NavPoint np5(lpc.position(725), 725);         lpc.addNavPoint(np5);
	NavPoint np6(lpc.position(750), 750);         lpc.addNavPoint(np6);
	NavPoint np7(lpc.position(775), 775);         lpc.addNavPoint(np7);
	NavPoint np8(lpc.position(800), 800);         lpc.addNavPoint(np8);
	NavPoint np9(lpc.position(825), 825);         lpc.addNavPoint(np9);
	NavPoint np10(lpc.position(850), 850);         lpc.addNavPoint(np10);
	NavPoint np11(lpc.position(875), 875);         lpc.addNavPoint(np11);
	NavPoint np12(lpc.position(900), 900);         lpc.addNavPoint(np12);
	NavPoint np13(lpc.position(925), 925);         lpc.addNavPoint(np13);
	double bankAngle = Units::from("deg", 10);  // should not be used by makeKinematicPlan
	double gsAccel = 2.85;
	double vsAccel = 1.77;
	//fpln(" #### lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "testAltPreserveFun_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, false, true, false);
	//fpln(" #### kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "testAltPreserveFun_kpc");
	double distBetween = PlanUtil::distanceBetween(lpc,kpc);
	//fpln(" ## testAltPreserveFun: distBetween = "+distBetween);
	EXPECT_TRUE(distBetween < 1.0);      // TODO:  THIS manifests altitude profile problem
}



TEST_F(TrajGenTest, testTurnOverLap) {
	Plan lpc;
	Position p1 = Position::makeXYZ(0.0,0.0,10000);
	Position p2 = Position::makeXYZ(20.0,0.0,10000);
	Position p3 = Position::makeXYZ(20.0,12.0,10000);
	Position p4 = Position::makeXYZ(0.0,20.0,10000);
	NavPoint np1(p1,0.0);          lpc.addNavPoint(np1);
	NavPoint np2(p2,400.0);        lpc.addNavPoint(np2);
	NavPoint np3(p3,600.0);        lpc.addNavPoint(np3);
	NavPoint np4(p4,1047.21);      lpc.addNavPoint(np4);
	double radius1 = Units::from("NM",8.0);
	double radius2 = Units::from("NM",6.0);
	lpc.setVertexRadius(1,radius1);
	lpc.setVertexRadius(2,radius2);
	bool repairTurn = false;
	bool repairGs = false;
	bool repairVs = false;
	double bankAngle = Units::from("deg", 0.00001);  // should not be used by makeKinematicPlan
	double gsAccel = 2.1;
	double vsAccel = 0.4;
	//fpln(" #### lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "test7_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repairTurn, repairGs, repairVs);
	//fpln(" #### kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "test7_kpc");
	//		if (kpc.hasError()) {
	//			//fpln(" #### test7. msg ="+kpc.getMessageNoClear());
	//		}
	EXPECT_TRUE(kpc.hasError());
}


TEST_F(TrajGenTest, test_generateTurnTCPsRadius) {
	Plan lpc;
	Position p0(LatLonAlt::make(3.152, 5.247, 10000));
	Position p1(LatLonAlt::make(1.857, 6.174, 10000));
	Position p2(LatLonAlt::make(1.746, 7.193, 10000));
	Position p3(LatLonAlt::make(2.966, 9.240, 10000));
	NavPoint np0(p0,0.000000); np0 = np0.makeName("AA");    	    lpc.addNavPoint(np0);
	NavPoint np1(p1,695.943700); np1=np1.makeName("BB");    	     lpc.addNavPoint(np1);
	NavPoint np2(p2,2378.534100); np2=np2.makeName("CC");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3525.191700); np3=np3.makeName("DD");    	 lpc.addNavPoint(np3);
	lpc = PlanUtil::mkGsConstant(lpc,Units::from("kn",600));
	bool continueGen = false;
	//DebugSupport.dumpPlan(lpc,"generateTurnTCPsRadius_lpc");
	double radius1 = Units::from("NM",42.0);
	double radius2 = Units::from("NM",45.0);
	lpc.setVertexRadius(1,radius1);
	lpc.setVertexRadius(2,radius2);
	Plan kpc = TrajGen::generateTurnTCPsRadius(lpc, continueGen); //,Units::from("NM",1.5));
	//fpln(" $$$ test_generateTurnTCPsRadius: kpc = "+kpc);
	int ixBOT1 = kpc.nextBOT(0);
	int ixBOT2 = kpc.nextBOT(ixBOT1);
	Position center1 = kpc.turnCenter(ixBOT1);
	double calcRadius1 = kpc.point(ixBOT1).position().distanceH(center1);
	double deltaRadius1 = std::abs(radius1 - calcRadius1);
	EXPECT_TRUE(deltaRadius1 < 0.4);
	//fpln(" $$$ test_generateTurnTCPsRadius: deltaRadius1 = "+deltaRadius1);
	Position center2 = kpc.turnCenter(ixBOT2);
	double calcRadius2 = kpc.point(ixBOT2).position().distanceH(center2);
	double deltaRadius2 = std::abs(radius2 - calcRadius2);
	//fpln(" $$$ test_generateTurnTCPsRadius: deltaRadius2 = "+deltaRadius2);
	EXPECT_TRUE(deltaRadius2 < 0.3);
	//DebugSupport.dumpPlan(kpc,"generateTurnTCPsRadius_kpc");
	EXPECT_TRUE(kpc.isFlyable(true));
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	//fpln(" $$$ test_generateTurnTCPsRadius: noTCPS = "+noTCPS);
	EXPECT_TRUE(PlanUtil::checkReversion(noTCPS,lpc));

}



TEST_F(TrajGenTest, testPointInTurn) {  // NEW
	Plan lpc("testPointInTurn");
	Position p0(LatLonAlt::make(2.152, 5.247, 20000));
	Position p1(LatLonAlt::make(0.957, 6.174, 21000));
	Position p2(LatLonAlt::make(-1.846, 7.193,21000));
	NavPoint np0(p0,0.000000);
	np0 = np0.makeName("AA");    lpc.addNavPoint(np0);
	NavPoint np1(p1,700);
	np1 = np1.makeName("VERT");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,2500);
	np2 = np2.makeName("CC");    	 lpc.addNavPoint(np2);
	//NavPoint np3(p3,3500).makeLabel("DD");    	 lpc.addNavPoint(np3);
	//NavPoint np4(lpc.position(690), 690);         lpc.addNavPoint(np4);
	NavPoint np5(lpc.position(725), 725);
	np5 = np5.makeName("XX");   lpc.addNavPoint(np5);
	double bankAngle = Units::from("deg", 10);  // should not be used by makeKinematicPlan
	//fpln(" #### testPointInTurn: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "testPointInTurn_lpc");
	Plan kpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
	//fpln(" #### testPointInTurn: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "testPointInTurn_kpc");
	EXPECT_EQ("AA",kpc.point(0).name());
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ("VERT",kpc.point(1).name());
		EXPECT_EQ("",kpc.point(2).name());
	} else {
		EXPECT_EQ("",kpc.point(1).name());
		EXPECT_EQ("VERT",kpc.point(2).name());
	}
	EXPECT_EQ("XX",kpc.point(3).name());   // This shows problem in turn generation
	EXPECT_EQ("",kpc.point(4).name());
	EXPECT_EQ("CC",kpc.point(5).name());
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(4));
	int ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(33323.93709911,kpc.calcRadius(ixBOT),0.0001);
	EXPECT_NEAR(Units::from("kn",466.6158),kpc.gsOut(ixBOT),0.0001);
	int ixEOT = kpc.nextEOT(0);
	EXPECT_NEAR(Units::from("kn",357.8918),kpc.gsOut(ixEOT),0.0001);
	EXPECT_TRUE(!kpc.hasError());
	//fpln(" $$$$$$$$$$$$$$ kpc.point(ixEOT).position() = "+kpc.point(ixEOT).position().toString(10));
	Position EOT = Position::makeLatLonAlt(0.9128526124, 6.1900494134, 21000.000000);
	EXPECT_TRUE(EOT.almostEquals(kpc.point(ixEOT).position(),0.0001, 0.0001));
	//EXPECT_TRUE(kpc.isTurnConsistent(false));
	int ixMOT = kpc.findMOT(ixBOT,ixEOT);
	EXPECT_NEAR(lpc.gsOut(1),kpc.gsOut(ixMOT),0.0001);
	bool continueGen = false;
	//bool delayGsChangeUntilEOT = false;
	kpc = TrajGen::generateTurnTCPs(lpc, bankAngle, continueGen);
	fpln(" $$ testPointInTurn: msg0 = "+kpc.getMessageNoClear());
	EXPECT_FALSE(kpc.hasError());

	//fpln(" $$$ kpc = "+kpc);
	EXPECT_EQ("AA",kpc.point(0).name());
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ("VERT",kpc.point(1).name());
		EXPECT_EQ("",kpc.point(2).name());
	} else {
		EXPECT_EQ("",kpc.point(1).name());
		EXPECT_EQ("VERT",kpc.point(2).name());
	}
	EXPECT_EQ("XX",kpc.point(3).name());   // This shows problem in turn generation
	EXPECT_EQ("",kpc.point(4).name());
	EXPECT_EQ("CC",kpc.point(5).name());
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(4));
	ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(33323.93709911,kpc.calcRadius(ixBOT),0.0001);
	EXPECT_NEAR(Units::from("kn",466.6158),kpc.gsOut(ixBOT),0.0001);
	ixEOT = kpc.nextEOT(0);
	EXPECT_NEAR(Units::from("kn",357.8918),kpc.gsOut(ixEOT),0.0001);
	//fpln(" $$$$$$$$$$$$$$ kpc.point(ixEOT).position() = "+kpc.point(ixEOT).position().toString(10));
	EOT = Position::makeLatLonAlt(0.9128526124, 6.1900494134, 21000.000000);
	EXPECT_TRUE(EOT.almostEquals(kpc.point(ixEOT).position(),0.0001, 0.0001));
	//EXPECT_TRUE(kpc.isTurnConsistent(false));
	ixMOT = kpc.findMOT(ixBOT,ixEOT);
	EXPECT_NEAR(Units::from("kn",357.8918),kpc.gsOut(ixMOT),0.0001);
}


TEST_F(TrajGenTest, testTwoPointsInTurn) {  // NEW
	Plan lpc("testTwoPointsInTurn");
	Position p0(LatLonAlt::make(2.152, 5.247, 20000));
	Position p1(LatLonAlt::make(0.957, 6.174, 21000));
	Position p2(LatLonAlt::make(-1.846, 7.193,21000));
	NavPoint np0(p0,0.000000); 	np0 = np0.makeName("AA");    lpc.addNavPoint(np0);
	NavPoint np1(p1,700); 		np1 = np1.makeName("VERT");    	    lpc.addNavPoint(np1);
	NavPoint np2(p2,2500); 		np2 = np2.makeName("CC");    	 lpc.addNavPoint(np2);
	NavPoint np5(lpc.position(725), 725); np5 = np5.makeName("XX");   lpc.addNavPoint(np5);
	NavPoint np6(lpc.position(690), 690); np6 = np6.makeName("YY");   lpc.addNavPoint(np6);
	double bankAngle = Units::from("deg", 10);  // should not be used by makeKinematicPlan
	double gsOut2 = Units::from("kn",400.00);
	lpc.mkGsOut(2,gsOut2);
	//fpln(" #### testTwoPointsInTurn: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "testTwoPointsInTurn_lpc");
	Plan kpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
	//fpln(" #### testTwoPointsInTurn: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "testTwoPointsInTurn_kpc");
	EXPECT_EQ("AA",kpc.point(0).name());
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ("VERT",kpc.point(1).name());
		EXPECT_EQ("",kpc.point(3).name());
	} else {
		EXPECT_EQ("",kpc.point(1).name());
		EXPECT_EQ("VERT",kpc.point(3).name());
	}
	EXPECT_EQ("YY",kpc.point(2).name());
	EXPECT_EQ("XX",kpc.point(4).name());
	EXPECT_EQ("",kpc.point(5).name());
	EXPECT_EQ("CC",kpc.point(6).name());
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(5));
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	int ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(Units::from("kn",466.6158),kpc.gsOut(ixBOT),0.0001);
	int ixEOT = kpc.nextEOT(0);
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	int ixMOT = kpc.findMOT(ixBOT,ixEOT);
//	EXPECT_NEAR(Units::from("kn",466.6158),kpc.gsOut(ixMOT),0.0001);
//	EXPECT_NEAR(gsOut2,kpc.gsOut(ixEOT),0.0001);
	bool continueGen = false;
	//bool delayGsChangeUntilEOT = false;
	kpc = TrajGen::generateTurnTCPs(lpc, bankAngle, continueGen);
	//fpln(" #### testTwoPointsInTurn: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "testTwoPointsInTurn_kpc");
	EXPECT_EQ("AA",kpc.point(0).name());
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ("VERT",kpc.point(1).name());
		EXPECT_EQ("",kpc.point(3).name());
	} else {
		EXPECT_EQ("",kpc.point(1).name());
		EXPECT_EQ("VERT",kpc.point(3).name());
	}
	EXPECT_EQ("YY",kpc.point(2).name());
	EXPECT_EQ("XX",kpc.point(4).name());
	EXPECT_EQ("",kpc.point(5).name());
	EXPECT_EQ("CC",kpc.point(6).name());
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(5));
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(Units::from("kn",466.6158),kpc.gsOut(ixBOT),0.0001);
	ixEOT = kpc.nextEOT(0);
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	ixMOT = kpc.findMOT(ixBOT,ixEOT);
	EXPECT_NEAR(gsOut2,kpc.gsOut(ixMOT),0.0001);
	EXPECT_NEAR(lpc.gsOut(3),kpc.gsOut(ixEOT),0.0001);
}


TEST_F(TrajGenTest, testThreePointsInTurn) {  // NEW
	Plan lpc("testThreePointsInTurn");
	Position p0(LatLonAlt::make(2.152, 5.247, 20000));
	Position p1(LatLonAlt::make(0.957, 6.174, 21000));
	Position p2(LatLonAlt::make(-1.846, 7.193,21000));
	NavPoint np0(p0,0.00); np0=np0.makeName("AA");    lpc.addNavPoint(np0);
	NavPoint np1(p1,700); np1=np1.makeName("VERT");    	    lpc.addNavPoint(np1);
	NavPoint np2(p2,2500); np2=np2.makeName("CC");    	 lpc.addNavPoint(np2);
	NavPoint np5(lpc.position(725), 725); np5=np5.makeName("I3");   lpc.addNavPoint(np5);
	NavPoint np6(lpc.position(690), 690); np6=np6.makeName("I1");   lpc.addNavPoint(np6);
	NavPoint np7(lpc.position(720), 720); np7=np7.makeName("I2");   lpc.addNavPoint(np7);
	double bankAngle = Units::from("deg", 10);  // should not be used by makeKinematicPlan
	//double gsOut0 = Units::from("kn",466.6158);
	double gsOut0 = Units::from("kn",470.00);
	lpc.mkGsOut(0,gsOut0);
	double gsOut2 = Units::from("kn",400.00);
	lpc.mkGsOut(2,gsOut2);
	double gsOut3 = Units::from("kn",390.00);
	lpc.mkGsOut(3,gsOut3);
	//fpln(" #### testThreePointsInTurn: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "testThreePointsInTurn_lpc");
	Plan kpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
	//fpln(" #### testThreePointsInTurn: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "testThreePointsInTurn_kpc");
	EXPECT_EQ("AA",kpc.point(0).name());
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ("VERT",kpc.point(1).name());
		EXPECT_EQ("I1",kpc.point(2).name());
		EXPECT_EQ("",kpc.point(3).name());
	} else {
		EXPECT_EQ("",kpc.point(1).name());
		EXPECT_EQ("I1",kpc.point(2).name());
		EXPECT_EQ("VERT",kpc.point(3).name());
	}
	EXPECT_EQ("I1",kpc.point(2).name());
	EXPECT_EQ("I2",kpc.point(4).name());
	EXPECT_EQ("I3",kpc.point(5).name());
	EXPECT_EQ("",kpc.point(6).name());
	EXPECT_EQ("CC",kpc.point(7).name());
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(6));
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	int ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(gsOut0,kpc.gsOut(ixBOT),0.0001);
	int ixEOT = kpc.nextEOT(0);
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	int ixMOT = kpc.findMOT(ixBOT,ixEOT);
	bool continueGen = false;
	//bool delayGsChangeUntilEOT = false;
	kpc = TrajGen::generateTurnTCPs(lpc, bankAngle, continueGen);
	//fpln(" #### testThreePointsInTurn: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "testThreePointsInTurn_kpc");
	EXPECT_EQ("AA",kpc.point(0).name());
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ("VERT",kpc.point(1).name());
		EXPECT_EQ("",kpc.point(3).name());

	} else {
		EXPECT_EQ("",kpc.point(1).name());
		EXPECT_EQ("VERT",kpc.point(3).name());
	}
	EXPECT_EQ("I1",kpc.point(2).name());
	EXPECT_EQ("I2",kpc.point(4).name());
	EXPECT_EQ("I3",kpc.point(5).name());
	EXPECT_EQ("",kpc.point(6).name());
	EXPECT_EQ("CC",kpc.point(7).name());
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_TRUE(kpc.isEOT(6));
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(gsOut0,kpc.gsOut(ixBOT),0.0001);
	ixEOT = kpc.nextEOT(0);
	EXPECT_TRUE(!kpc.hasError());
	EXPECT_TRUE(kpc.isTurnConsistent(false));
	ixMOT = kpc.findMOT(ixBOT,ixEOT);
	EXPECT_NEAR(gsOut2,kpc.gsOut(ixMOT),0.0001);
	EXPECT_NEAR(lpc.gsOut(4),kpc.gsOut(ixEOT),0.0001);
}




TEST_F(TrajGenTest, test_T007) {
	Plan lpc;
	Position p0(LatLonAlt::make(0.000000, 0.000000, 9526.607100));
	Position p1(LatLonAlt::make(0.015307, 0.068002, 9439.839506));
	Position p2(LatLonAlt::make(0.169414, 1.274985, 7931.935100));
	Position p3(LatLonAlt::make(0.169600, 1.802900, 12094.990400));
	Position p4(LatLonAlt::make(-0.519600, 2.490600, 28757.277900));
	NavPoint np0(p0,0.000000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,30.334531);   np1=np1.makeName("P1");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,559.867776);  np2=np2.makeName("P2");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,789.611198);  np3=np3.makeName("P3");    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,1213.319185); np4=np4.makeName("P4");    	 lpc.addNavPoint(np4);
	std::string info1 = "info###########";
	lpc.setInfo(1,info1);
	double default_bank_angle = Units::from("deg", 25);
	bool continueGen = false;
	Plan vpc = TrajGen::markVsChanges(lpc,vsAccel);
	//fpln(" ## test_T007: lpc = "+lpc);
	Plan kpc = TrajGen::generateTurnTCPs(vpc, default_bank_angle, continueGen);
	//fpln(" ## test_T007: kpc = "+kpc);
	EXPECT_EQ(11,kpc.size());
	EXPECT_NEAR(kpc.point(0).time(),0.0000,0.001);
	EXPECT_NEAR(kpc.point(0).lat(),0.0000,0.001);
	EXPECT_NEAR(kpc.point(0).lon(),0.0000,0.001);
	EXPECT_NEAR(kpc.point(0).alt(),2903.7098,0.001);
	EXPECT_NEAR(kpc.point(1).time(),27.6971,0.001);
	EXPECT_NEAR(kpc.point(1).lat(),0.0002,0.001);
	EXPECT_NEAR(kpc.point(1).lon(),0.0011,0.001);
	EXPECT_NEAR(kpc.point(1).alt(),2879.5608,0.01);
	EXPECT_TRUE(kpc.isBOT(1));
	if (TrajGen::vertexNameInBOT) {
		EXPECT_EQ(info1,kpc.getInfo(1));
		EXPECT_EQ("",kpc.getInfo(2));                               // MOT

	} else {
		EXPECT_EQ("",kpc.getInfo(1));
		//EXPECT_EQ(info1+Plan::MOTflag,kpc.getInfo(2));                               // MOT
		EXPECT_TRUE(kpc.isMOT(2));
	}
	EXPECT_NEAR(kpc.point(2).time(),30.3326,0.001);
	EXPECT_NEAR(kpc.point(2).lat(),0.0003,0.001);
	EXPECT_NEAR(kpc.point(2).lon(),0.0012,0.001);
	EXPECT_NEAR(kpc.point(2).alt(),2877.2631,0.001);
	EXPECT_NEAR(kpc.point(3).time(),32.9681,0.001);
	EXPECT_NEAR(kpc.point(3).lat(),0.0003,0.001);
	EXPECT_NEAR(kpc.point(3).lon(),0.0013,0.001);
	EXPECT_NEAR(kpc.point(3).alt(),2874.9756,0.01);
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_NEAR(kpc.point(4).time(),556.3246,0.001);
	EXPECT_NEAR(kpc.point(4).lat(),0.0029,0.001);
	EXPECT_NEAR(kpc.point(4).lon(),0.0221,0.001);
	EXPECT_NEAR(kpc.point(4).alt(),2420.7217,0.01);
	EXPECT_TRUE(kpc.isBOT(4));
	EXPECT_NEAR(kpc.point(5).time(),559.8591,0.001);
	EXPECT_NEAR(kpc.point(5).lat(),0.0030,0.001);
	EXPECT_NEAR(kpc.point(5).lon(),0.0223,0.001);
	EXPECT_NEAR(kpc.point(5).alt(),2417.6538,0.001);
	EXPECT_NEAR(kpc.point(6).time(),563.3937,0.001);
	EXPECT_NEAR(kpc.point(6).lat(),0.0030,0.001);
	EXPECT_NEAR(kpc.point(6).lon(),0.0224,0.001);
	EXPECT_NEAR(kpc.point(6).alt(),2437.1757,0.05);
	EXPECT_TRUE(kpc.isEOT(6));
	EXPECT_NEAR(kpc.point(7).time(),766.4225,0.001);
	EXPECT_NEAR(kpc.point(7).lat(),0.0030,0.001);
	EXPECT_NEAR(kpc.point(7).lon(),0.0305,0.001);
	//EXPECT_NEAR(kpc.point(7).alt(),3565.2260,0.05);
	EXPECT_TRUE(kpc.isBOT(7));
	EXPECT_NEAR(kpc.point(8).time(),788.3897,0.0001);
	EXPECT_NEAR(kpc.point(8).lat(),0.0028,0.0001);
	EXPECT_NEAR(kpc.point(8).lon(),0.0314,0.0001);
	EXPECT_NEAR(kpc.point(8).alt(),3686.5531,0.0001);
	EXPECT_NEAR(kpc.point(9).time(),810.3568,0.0001);
	EXPECT_NEAR(kpc.point(9).lat(),0.0023,0.0001);
	EXPECT_NEAR(kpc.point(9).lon(),0.0321,0.0001);
	//EXPECT_NEAR(kpc.point(9).alt(),3949.8563,0.001);
	EXPECT_TRUE(kpc.isEOT(9));
	EXPECT_NEAR(kpc.point(10).time(),1210.8894,0.001);
	EXPECT_NEAR(kpc.point(10).lat(),-0.0091,0.001);
	EXPECT_NEAR(kpc.point(10).lon(),0.0435,0.001);
	EXPECT_NEAR(kpc.point(10).alt(),8765.2183,0.001);
}


TEST_F(TrajGenTest, test_T059) {	// **NEW**
	Plan lpc;
	Position p0(LatLonAlt::make(-1.533774, 0.870610, 18479.686188));
	Position p1(LatLonAlt::make(-1.575800, 0.857200, 18541.789300));
	Position p2(LatLonAlt::make(-1.581347, 0.855430, 18549.986757));
	Position p3(LatLonAlt::make(-1.586654, 0.855961, 18590.853975));
	Position p4(LatLonAlt::make(-2.464098, 0.876259, 24455.979205));
	Position p5(LatLonAlt::make(-2.466000, 0.876300, 24469.854100));
	NavPoint np0(p0,1229.518420);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1253.571900);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1256.746900);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1259.654732);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,1738.238968);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,1739.276321);    	 lpc.addNavPoint(np5);
	bankAngle = Units::from("deg", 25);
	//DebugSupport.dumpPlan(lpc, "test_T059_lpc");
	Plan kpc = TrajGen::generateTurnTCPs(lpc, bankAngle);
	//DebugSupport.dumpPlan(kpc, "test_T059_kpc");
	//fpln(" $$## test_T059: kpc ="+kpc);
	EXPECT_TRUE(kpc.hasError());
	//EXPECT_TRUE(kpc.isWellFormed());
	//EXPECT_TRUE(kpc.isTurnConsistent(true));
}


TEST_F(TrajGenTest, test_EasyGs) {
	Position p0(LatLonAlt::make(34.1626, -118.9911, 5000));
	Position p1(LatLonAlt::make(34.1967, -119.0753, 5000));
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,100.0);
	Plan lpc;
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	Velocity v0 = lpc.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0);
	NavPoint np2 = np1.linear(v0.mkGs(200),40).makeName("CC");
	NavPoint np3 = np2.linear(v0.mkGs(200),40).makeName("CC");
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.mkGsOut(2,Units::from("kn",301));
	lpc.mkGsOut(1,Units::from("kn",300));
	lpc.mkGsOut(0,Units::from("kn",200));
	//fpln(" $$$ test_EasyGs: lpc = "+lpc.toStringGs());
	double gsAccel = 1.2345;
	//bool useOffset = false;
	Plan kpc = TrajGen::generateGsTCPs(lpc,gsAccel,true);
	//fpln(" $$$ test_EasyGs: kpc = "+kpc.toStringGs());
	EXPECT_TRUE(kpc.isFlyable());
	kpc.revertGsTCPs();
	//f.pln(" $$$ test_EasyGs: AFTER kpc = "+kpc.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(kpc,lpc));
    double newGsAccel1 = 2.2;
    kpc.setGsAccel(1,newGsAccel1);
	Plan kpc2 = TrajGen::generateGsTCPs(kpc,gsAccel,true);
	//f.pln(" $$$ test_EasyGs: kpc2 = "+kpc2.toStringGs());
	int ixBGS = kpc2.nextBGS(0);
	EXPECT_NEAR(newGsAccel1,kpc.gsAccel(ixBGS),0.0001);
}




TEST_F(TrajGenTest, test_gsGen) {
	Position p0(LatLonAlt::make(35.1626, -110.9911, 5000));
	Position p1(LatLonAlt::make(35.1967, -111.0753, 5000));
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,100.0);
	Plan lpc;
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	Velocity v0 = lpc.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0);
	double kn100 = Units::from("kn",100);
	NavPoint np2 = np1.linear(v0.mkGs(kn100),40).makeName("CC");
	lpc.addNavPoint(np2);
	NavPoint np3 = np2.linear(v0.mkGs(kn100),60).makeName("DD");
	lpc.addNavPoint(np3);
	std::string name = lpc.getName();
	std::string note = lpc.getNote();
	EXPECT_EQ(name,lpc.getName());
	EXPECT_EQ(note,lpc.getNote());
	lpc.mkGsOut(0,kn100);
	//fpln(" lpc = "+lpc.toStringGs());
	lpc.mkGsOut(1,2*kn100);
	lpc.mkGsOut(2,3*kn100);
	//fpln(" lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "test_GsGen_lpc");
	double t12 = lpc.time(2) - lpc.time(1);
	double a = 1.2*kn100/t12;
	//fpln(" $$$ a = "+a+" d12 = "+Units::str("NM",d12)+" t12  = "+t12+" d12calc = "+Units::str("NM",d12calc));
	//fpln(" $$$ test_gsGen: a = "+a);
	lpc.setGsAccel(1,a);
	Plan kpc = lpc;
	//double targetGs = 2*kn100;
	//bool allowOverlap = true;
	TrajGen::generateGsTCPsAt(kpc, 1, a,true);
	//fpln(" test_gsGen: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "test_gsGen");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isGsContinuous(1, Units::from("kn", 20), false));
	//fpln(" msg = "+kpc.getMessageNoClear());
	kpc.revertGsTCPs();
	//f.pln(" $$$ test_gsGen: kpc = "+kpc.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(kpc,lpc));
	lpc.mkGsOut(2,2.0*kn100);
	NavPoint in1 = NavPoint(lpc.position(170.0).mkAlt(2000),170.0);
	lpc.addNavPoint(in1);
	NavPoint in2 = NavPoint(lpc.position(180.0).mkAlt(2000),180.0);
	lpc.addNavPoint(in2);
	//f.pln(" $$$ test_gsGen: lpc = "+lpc.toStringGs());
	//bool useOffset = true;
	//bool repairGs = false;
    Plan kpc3 = TrajGen::generateGsTCPs(lpc, gsAccel,true);
	//f.pln(" $$$ test_gsGen: kpc3 = "+kpc3.toStringGs());
	kpc3.revertGsTCPs();
	//f.pln(" $$$ test_gsGen: AFTER kpc3 = "+kpc3.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(kpc3,lpc));
}



TEST_F(TrajGenTest, test_gsGen2) {
	Position p0(LatLonAlt::make(35.1626, -110.9911, 5000));
	Position p1(LatLonAlt::make(35.1967, -111.0753, 5000));
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,100.0);
	Plan lpc;
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	Velocity v0 = lpc.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0);
	double kn100 = Units::from("kn",100);
	NavPoint np2 = np1.linear(v0.mkGs(kn100),40).makeName("CC");
	NavPoint np3 = np2.linear(v0.mkGs(kn100),60).mkAlt(Units::from("ft",4000)).makeName("DD");
	NavPoint np4 = np3.linear(v0.mkGs(kn100),50).mkAlt(Units::from("ft",4000)).makeName("EE");
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	std::string name = lpc.getName();
	std::string note = lpc.getNote();
	EXPECT_EQ(name,lpc.getName());
	EXPECT_EQ(note,lpc.getNote());
	// ********* create ground speed profile *************
	lpc.mkGsOut(0,kn100);
	lpc.mkGsOut(1,2*kn100);
	lpc.mkGsOut(2,2*kn100);
	lpc.mkGsOut(3,2*kn100);
	//fpln(" lpc = "+lpc.toStringGs());
	double t12 = lpc.time(2) - lpc.time(1);
	double a = 0.6*kn100/t12;
	//lpc.setGsAccel(1,a);
	//double targetGs = 2*kn100;
	//DebugSupport.dumpPlan(lpc, "test_gsGen2_lpc");
	//fpln(" ## test_gsGen2: lpc = "+lpc.toStringGs());
	Plan kpc = lpc; // .copy();
	//bool allowOverlap = true;
	TrajGen::generateGsTCPsAt(kpc, 1, a,true);
	//fpln(" test_gsGen2: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "test_gsGen2_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWeakConsistent(true));
	EXPECT_TRUE(kpc.isGsContinuous(true));
	int ixEGS = kpc.nextEGS(1);
	EXPECT_TRUE(ixEGS > 1);
	//fpln(" test_gsGen2: kpc = "+kpc.toStringGs());
	EXPECT_NEAR(2*kn100, kpc.gsOut(ixEGS),0.00001);
	EXPECT_NEAR(2*kn100, kpc.gsIn(ixEGS),0.00001);
	for (int j = 1; j < ixEGS; j++) {
		EXPECT_TRUE(2*kn100 > kpc.gsOut(j));
		EXPECT_TRUE(2*kn100 > kpc.gsIn(j));
	}
}


TEST_F(TrajGenTest, test_gsGen3) {
	Position p0(LatLonAlt::make(38.1626, -110.8011, 5000));
	Position p1(LatLonAlt::make(38.1967, -111.0753, 5000));
	NavPoint np0(p0,0.0); np0 = np0.makeName("AA");
	NavPoint np1(p1,100.0); np1=np1.makeName("BB");
	Plan lpc;
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	Velocity v0 = lpc.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0);
	double kn100 = Units::from("kn",100);
	NavPoint np2 = np1.linear(v0.mkGs(kn100),40); np2=np2.makeName("CC");
	NavPoint np3 = np2.linear(v0.mkGs(kn100),40).mkAlt(Units::from("ft",4000)); np3=np3.makeName("DD");
	NavPoint np4 = np3.linear(v0.mkGs(kn100),40).mkAlt(Units::from("ft",4000)); np4=np4.makeName("EE");
	Position p5(LatLonAlt::make(38.200, -111.1753, 5000));
	NavPoint np5(p5,np4.time()+20); np5=np5.makeName("BB");
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	lpc.addNavPoint(np5);
	std::string name = lpc.getName();
	std::string note = lpc.getNote();
	EXPECT_EQ(name,lpc.getName());
	EXPECT_EQ(note,lpc.getNote());
	// ********* create ground speed profile *************
	lpc.mkGsOut(0,3*kn100);
	lpc.mkGsOut(1,2*kn100);
	lpc.mkGsOut(2,2*kn100);
	lpc.mkGsOut(3,2*kn100);
	lpc.mkGsOut(4,2*kn100);
	//fpln(" lpc = "+lpc.toStringGs());
	double t12 = lpc.time(2) - lpc.time(1);
	double a = 0.3*kn100/t12;
	//lpc.setGsAccel(1,a);
	//double targetGs = 2*kn100;
	//DebugSupport.dumpPlan(lpc, "test_gsGen3_lpc");
	Plan kpc = TrajGen::generateTurnTCPs(lpc,Units::from("deg",15));
	//DebugSupport.dumpPlan(kpc, "test_gsGen3_turn");
	TrajGen::generateGsTCPsAt(kpc, 1, a,true);
	//fpln(" ## test_gsGen3: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc, "test_gsGen3_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWeakConsistent(true));
	EXPECT_TRUE(kpc.isGsContinuous(true));
	int ixEGS = kpc.nextEGS(1);
	EXPECT_TRUE(ixEGS > 1);
	//fpln(" ## test_gsGen3: kpc = "+kpc.toStringGs());
	EXPECT_NEAR(2*kn100, kpc.gsOut(ixEGS),0.00001);
	EXPECT_NEAR(2*kn100, kpc.gsIn(ixEGS),0.00001);
	for (int j = 1; j < ixEGS; j++) {
		EXPECT_TRUE(2*kn100 < kpc.gsOut(j));
		EXPECT_TRUE(2*kn100 < kpc.gsIn(j));
	}
	//fpln(" msg = "+kpc.getMessageNoClear());

	//fpln(" $$$$$$$$$$$$ "+Units::str("fpm",2.6,8));
}


TEST_F(TrajGenTest, test_gsGen0) {
	Position p0(LatLonAlt::make(-42.810000, -76.900000, 31390.000000));
	Position p1(LatLonAlt::make(-42.875005, -76.934963, 31385.000000));
	Position p2(LatLonAlt::make(-42.940000, -76.970000, 31380.000000));
	NavPoint np0(p0,0.000000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,50.000000); np1=np1.makeName("$AA");    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,97.167435); np2=np2.makeName("$BB");    	 lpc.addNavPoint(np2);
	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	double  vsAccel = Units::from("m/s", 2.0);		       // vertical speed acceleration
	double  bankAngle = Units::from("deg", 25.0);         // bank angle of aircraft in turn
	bool repairTurn = false;
	bool repairVs = false;
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repairTurn, true, repairVs);
	//fpln(" ## test_gsGen0: kpc = "+kpc.toString());
	EXPECT_EQ(lpc.point(1).name(), kpc.point(1).name());
	EXPECT_EQ(lpc.point(2).name(), kpc.point(3).name());
	EXPECT_EQ(lpc.getInfo(1), kpc.getInfo(1));
	EXPECT_EQ(lpc.getInfo(2), kpc.getInfo(3));
}



TEST_F(TrajGenTest, test_T445) {
	Plan lpc("ac_10930_KHOU_64");
	Position p1(LatLonAlt::make(29.785060, -95.382675, 8000));
	Position p2(LatLonAlt::make(29.892961, -95.455975, 8000));
	Position p3(LatLonAlt::make(29.895042, -95.457097, 7800));
	Position p4(LatLonAlt::make(30.046586, -95.538980, 6000));
	NavPoint np1(p1,72);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,193);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,195);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,351);    	 lpc.addNavPoint(np4);
	lpc.mkGsConstant(2,3,lpc.gsOut(1));
	double gsAcc = 3.0;
	lpc.setGsAccel(1,gsAcc);
	//fpln("$$ test_T445: lpc = "+lpc.toStringGs());
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, false, true);
	//fpln("$$ test_T445: kpc = "+kpc.toStringGs());
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isFlyable(false));
	int ixBGS = kpc.nextBGS(0);
	EXPECT_TRUE(kpc.isBGS(ixBGS));
	EXPECT_TRUE(contains(kpc.getInfo(ixBGS),Plan::manualGsAccel));
	//EXPECT_TRUE(contains(kpc.getInfo(ixBGS),Plan::MOTflag));
	EXPECT_TRUE(kpc.isMOT(ixBGS));
	EXPECT_NEAR(gsAcc,kpc.gsAccel(ixBGS),0.0001);
	Plan noTCPS = PlanUtil::revertAllTCPs(kpc);
	//fpln("$$ test_T445: noTCPS = "+noTCPS.toStringGs());
	EXPECT_NEAR(gsAcc,noTCPS.gsAccel(1),0.0001);
}


TEST_F(TrajGenTest, test_Dp) {
	Plan lpc("NA001");
	Position p0(LatLonAlt::make(37.44987228, -78.94022347, 37078.79716795));
	Position p1(LatLonAlt::make(37.45009720, -78.94019292, 37078.79716795));
	Position p2(LatLonAlt::make(37.59396632, -78.92061241, 34291.28083814));
	Position p3(LatLonAlt::make(37.73770794, -78.90097306, 31740.22125506));
	Position p4(LatLonAlt::make(37.84890416, -78.88572767, 29751.88225417));
	Position p5(LatLonAlt::make(37.96005448, -78.87044229, 28000.00000000));
	std::pair<NavPoint,TcpData> np0 = Plan::makeBGS("", p0,4400.000000, 0.251014);
	lpc.add(np0);
	std::pair<NavPoint,TcpData> np1 = Plan::makeEGSBGS("", p1,4400.138900, 0.034741);
	lpc.add(np1);
	std::pair<NavPoint,TcpData> np2 = Plan::makeEGSBGS("", p2,4488.232520, 0.039766);
	lpc.add(np2);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEGSBGS("", p3,4574.710440, -0.092163);
	lpc.add(np3);
	std::pair<NavPoint,TcpData> np4 = Plan::makeEGSBGS("", p4,4642.112794, -0.084268);
	lpc.add(np4);
	std::pair<NavPoint,TcpData> np5 = Plan::makeEGS(p5, 4711.770634);
	lpc.add(np5);
	double vsAccel = 1;
	bool continueGen = true;
	//DebugSupport.dumpPlan(lpc,"test_Dp_lpc");
	Plan kpc = TrajGen::generateVsTCPs(lpc,vsAccel,continueGen);
}


TEST_F(TrajGenTest, test_bank_angle) {
	Plan lpc("traff");
	// this should be flying at approx 25 kts
	Position p0(LatLonAlt::make(36.409284,-76.021027,700.000000));
	Position p1(LatLonAlt::make(36.409355,-76.022829,700.000000));
	Position p2(LatLonAlt::make(36.409523,-76.024311,700.000000));
	Position p3(LatLonAlt::make(36.408375,-76.024482,700.000000));
	Position p4(LatLonAlt::make(36.407982,-76.022560,700.000000));
	NavPoint np0(p0, 9.000000);
	NavPoint np1(p1, 21.543236);
	NavPoint np2(p2, 31.946260);
	NavPoint np3(p3, 41.934475);
	NavPoint np4(p4, 55.722276);
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	double bank1 = Units::from("deg", 20);
	Plan kpc1 = TrajGen::makeKinematicPlan(lpc, bank1, 2.0, 1.0, false, false, false);
	EXPECT_TRUE(kpc1.isConsistent());
	EXPECT_TRUE(kpc1.isVelocityContinuous());
}



TEST_F(TrajGenTest, test_SlowToZero) {
	Plan lpc("Boots");
	Position p5  = Position::makeLatLonAlt(32.81891292, -96.81118919, 4000.00000000);
	Position p7  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1169.00000000);
	Position p8  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1119.00000000);
	Position p9  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1169.00000000);
	NavPoint np5(p5,1378.438782);    	 lpc.addNavPoint(np5);
	NavPoint np7(p7,1548.298782);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,1551.298782);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,1601.298782);    	 lpc.addNavPoint(np9);
	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	//bool repairGs = false;
	//bool useOffset = false;
	Plan kpc = TrajGen::generateGsTCPs(lpc,gsAccel,true);
	//fpln(" $$$ test_SlowToZero: kpc = "+kpc.toStringVs());
	EXPECT_TRUE(kpc.isConsistent(false));
//	if (TrajGen::generateGsToZero) {
		EXPECT_TRUE(kpc.isBGS(1));
		EXPECT_TRUE(kpc.isEGS(2));
		EXPECT_TRUE(kpc.isGsContinuous(false));
//	}
	//DebugSupport.dumpPlan(kpc, "slowToZero_kpc");
	EXPECT_TRUE(kpc.isTrkContinuous(false));
	int ixBGS = kpc.nextBGS(0);
	EXPECT_EQ(1,ixBGS);
	kpc.revertGsTCP(ixBGS);
	//fpln(" $$$ test_SlowToZero: kpc = "+kpc.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(kpc,lpc));
}


TEST_F(TrajGenTest, test_SlowToZero_2) {
	Plan lpc("Boots");
	Position p5  = Position::makeLatLonAlt(32.81891292, -96.81118919, 4000.00000000);
	Position p6  = Position::makeLatLonAlt(32.89100626, -96.87850213, 1251.28941685);
	Position p7  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1169.00000000);
	Position p8  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1119.00000000);
	Position p9  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1000.00000000);
	NavPoint np5(p5,1390.4);    	 lpc.addNavPoint(np5);
	NavPoint np6(p6,1543.361417);    	 lpc.addNavPoint(np6);
	NavPoint np7(p7,1548.298782);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,1551.298782);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,1601.298782);    	 lpc.addNavPoint(np9);
	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	//bool repairGs = false;
	//bool useOffset = false;
	Plan kpc = TrajGen::generateGsTCPs(lpc,gsAccel,true);
	//fpln(" $$$ test_SlowToZero: kpc = "+kpc.toStringGs());
	EXPECT_TRUE(kpc.isConsistent(false));
//	if (TrajGen::generateGsToZero) {
		EXPECT_TRUE(kpc.isBGS(1));
		EXPECT_TRUE(kpc.isEGS(3));
		EXPECT_TRUE(kpc.isGsContinuous(false));
//	}
	//DebugSupport.dumpPlan(kpc, "slowToZero_kpc");
	EXPECT_TRUE(kpc.isTrkContinuous(false));
	int ixBGS = kpc.nextBGS(0);
	EXPECT_EQ(1,ixBGS);
	kpc.revertGsTCP(ixBGS);
	//fpln(" $$$ test_SlowToZero: kpc = "+kpc.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(kpc,lpc));
}


TEST_F(TrajGenTest, test_generateGsToZero) {
	Plan lpc("generateGsToZero");
	Position p4  = Position::makeLatLonAlt(32.67606860, -96.67823726, 4000.00000000);
	Position p5  = Position::makeLatLonAlt(32.81891292, -96.81118919, 4000.00000000);
	Position p6  = Position::makeLatLonAlt(32.89100626, -96.87850213, 1251.28941685);
	Position p7  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1169.00000000);
	Position p8  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1119.00000000);
	Position p9  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1119.00000000);
	NavPoint np4(p4,1182.519519);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,1378.438782);    	 lpc.addNavPoint(np5);
	NavPoint np6(p6,1543.361417);    	 lpc.addNavPoint(np6);
	NavPoint np7(p7,1548.298782);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,1551.298782);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,1601.298782);    	 lpc.addNavPoint(np9);
	//fpln(" $$ test_generateGsToZero: lpc = "+lpc.toStringGs());
	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	//bool repairGs = false;
	//bool useOffset = false;
	Plan kTraj = TrajGen::generateGsTCPs(lpc,gsAccel,true);
	if (kTraj.hasError()) {
		fpln(" ***************************************************************************************");
		fpln(" $$$$ makeSimpleTrajectory.makeKinematicPlan ERROR = "+kTraj.getMessageNoClear());
		fpln(" ***************************************************************************************");
		//DebugSupport.dumpPlan(lpcPlus,"UAMTraj_KIN_GEN_ERROR_lpc_"+lpcPlus.getName());
	}
	//DebugSupport.dumpPlan(kTraj,"test_U9_kTraj");
	//fpln(" $$ test_generateGsToZero: kTraj = "+kTraj.toStringGs());
	EXPECT_TRUE(kTraj.isConsistent());
	EXPECT_TRUE(kTraj.isGsContinuous(false));
	EXPECT_TRUE(lpc.point(0).almostEquals(kTraj.point(0)));
	EXPECT_NEAR(lpc.gsOut(0),kTraj.gsOut(0),0.001);
	EXPECT_NEAR(0.0,kTraj.vsOut(0),0.001);
	EXPECT_NEAR(0.0,kTraj.gsIn(kTraj.size()-1),0.001);
	EXPECT_NEAR(0.0,kTraj.vsIn(kTraj.size()-1),0.001);
	EXPECT_NEAR(lpc.point(lpc.size()-1).lat(),kTraj.point(kTraj.size()-1).lat(),0.0001);
	EXPECT_NEAR(lpc.point(lpc.size()-1).lon(),kTraj.point(kTraj.size()-1).lon(),0.0001);
	EXPECT_NEAR(lpc.point(lpc.size()-1).alt(),kTraj.point(kTraj.size()-1).alt(),0.0001);
	kTraj.revertGsTCPs();
	//f.pln(" $$$ test_SlowToZero_2: kpc = "+kTraj.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(kTraj,lpc));
}


TEST_F(TrajGenTest, test_U9) {
	Plan lpc("U9");
	Position p0  = Position::makeLatLonAlt(32.60704000, -96.61418800, 1322.00000000);
	Position p1  = Position::makeLatLonAlt(32.60704000, -96.61418800, 1322.00000000);
	Position p2  = Position::makeLatLonAlt(32.60704000, -96.61418800, 1372.00000000);
	Position p3  = Position::makeLatLonAlt(32.60920196, -96.61619205, 1454.28941685);
	Position p4  = Position::makeLatLonAlt(32.67606860, -96.67823726, 4000.00000000);
	Position p5  = Position::makeLatLonAlt(32.81891292, -96.81118919, 4000.00000000);
	Position p6  = Position::makeLatLonAlt(32.89100626, -96.87850213, 1251.28941685);
	Position p7  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1169.00000000);
	Position p8  = Position::makeLatLonAlt(32.89316400, -96.88051900, 1119.00000000);
	NavPoint np0(p0,971.839519);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1021.839519);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1024.839519);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1029.776884);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,1182.519519);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,1378.438782);    	 lpc.addNavPoint(np5);
	NavPoint np6(p6,1543.361417);    	 lpc.addNavPoint(np6);
	NavPoint np7(p7,1548.298782);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,1551.298782);    	 lpc.addNavPoint(np8);
	//fpln(" $$ test_U9: lpc = "+lpc.toStringGs());
	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	double  vsAccel = Units::from("m/s", 2.0);		       // vertical speed acceleration
	double  bankAngle = Units::from("deg", 25.0);         // bank angle of aircraft in turn
	bool repair = false;
	//DebugSupport::dumpPlan(lpc,"test_U9_lpc");
	Plan kTraj = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	if (kTraj.hasError()) {
		fpln(" ***************************************************************************************");
		fpln(" $$$$ makeSimpleTrajectory.makeKinematicPlan ERROR = "+kTraj.getMessageNoClear());
		fpln(" ***************************************************************************************");
	}
	//DebugSupport.dumpPlan(kTraj,"test_U9_kTraj");
	//fpln(" $$ test_U9: kTraj = "+kTraj.toStringGs());
	//double startTm = lpc.getFirstTime();
	//fpln(" $$ test_U9: kTraj = "+kTraj.toStringGs());
	EXPECT_TRUE(kTraj.isConsistent());
	//fpln(" $$ test_U9: kTraj = "+kTraj.toStringTrk());
    EXPECT_TRUE(kTraj.isGsContinuous(false));
	EXPECT_TRUE(lpc.point(0).almostEquals(kTraj.point(0)));
	EXPECT_NEAR(0.0,kTraj.gsOut(0),0.001);
	EXPECT_NEAR(0.0,kTraj.vsOut(0),0.001);
	EXPECT_NEAR(0.0,kTraj.gsIn(kTraj.size()-1),0.001);
	//EXPECT_NEAR(0.0,kTraj.vsIn(kTraj.size()-1),0.001);
	EXPECT_NEAR(lpc.point(lpc.size()-1).lat(),kTraj.point(kTraj.size()-1).lat(),0.0001);
	EXPECT_NEAR(lpc.point(lpc.size()-1).lon(),kTraj.point(kTraj.size()-1).lon(),0.0001);
	EXPECT_NEAR(lpc.point(lpc.size()-1).alt(),kTraj.point(kTraj.size()-1).alt(),0.0001);
	EXPECT_NEAR(2.0,kTraj.vsAccel(1),0.001);
	//EXPECT_NEAR(2.0,kTraj.vsAccel(kTraj.size()-3),0.001);  // TODO: Should this be -2?
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kTraj,100));
}


TEST_F(TrajGenTest, test_gs0_UF18) {
	Plan lpc("UF18");
	Position p0  = Position::makeLatLonAlt(32.63610800, -97.05703000, 1447.00000000);
	Position p1  = Position::makeLatLonAlt(32.63610800, -97.05703000, 1497.00000000);
	Position p2  = Position::makeLatLonAlt(32.63667404, -97.05384284, 1590.51070096);
	Position p3  = Position::makeLatLonAlt(32.63915184, -97.03988558, 2000.00000000);
	Position p4  = Position::makeLatLonAlt(32.65333848, -96.95978997, 2000.00000000);
	Position p5  = Position::makeLatLonAlt(32.65604680, -96.94446358, 1550.51070096);
	Position p6  = Position::makeLatLonAlt(32.65661000, -96.94127500, 1457.00000000);
	Position p7  = Position::makeLatLonAlt(32.65661000, -96.94127500, 1407.00000000);
	NavPoint np0(p0,43.500000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,46.500000);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,52.110642);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,76.680000);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,161.260985);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,188.230343);    	 lpc.addNavPoint(np5);
	NavPoint np6(p6,193.840985);    	 lpc.addNavPoint(np6);
	NavPoint np7(p7,196.840985);    	 lpc.addNavPoint(np7);

	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	double  vsAccel = Units::from("m/s", 2.0);		       // vertical speed acceleration
	double  bankAngle = Units::from("deg", 25.0);         // bank angle of aircraft in turn
	bool repair = false;
	//fpln(" ## test_gs0_UF18: lpc = "+lpc.toStringGs());

	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//TrajGen::generateGsToZero_b = false;
	if (kpc.hasError()) {
		fpln(" ***************************************************************************************");
		fpln(" $$$$ ERROR = "+kpc.getMessageNoClear());
		fpln(" ***************************************************************************************");
		//DebugSupport.dumpPlan(lpcPlus,"UAMTraj_KIN_GEN_ERROR_lpc_"+lpcPlus.getName());
	}
	//DebugSupport.dumpPlan(kpc,"test_gs0_UF18_kpc");
	//fpln(" $$ test_gs0_UF18: kTraj = "+kpc.toStringGs());
	//fpln(" $$ test_gs0_UF18: kTraj = "+kpc.toStringVs());
	EXPECT_TRUE(kpc.isConsistent());
	//fpln(" $$ test_gs0_UF18: kTraj = "+kpc.toStringTrk());
	EXPECT_TRUE(kpc.isVelocityContinuous(false));
	EXPECT_TRUE(lpc.point(0).almostEquals(kpc.point(0)));

	//fpln(" ## test_gs0_UF18: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"test_gs0_UF18");
	EXPECT_EQ(lpc.point(1).name(), kpc.point(1).name());
	EXPECT_EQ(lpc.point(2).name(), kpc.point(3).name());
	EXPECT_EQ(lpc.getInfo(1), kpc.getInfo(1));
	EXPECT_EQ(lpc.getInfo(2), kpc.getInfo(3));
	//EXPECT_EQ(15,kpc.size());
	EXPECT_TRUE(kpc.isBGS(1));
	EXPECT_TRUE(kpc.isEGS(3));
	EXPECT_TRUE(kpc.isBVS(4));
	EXPECT_TRUE(kpc.isBGS(5));
	EXPECT_TRUE(kpc.isEVS(6));
	EXPECT_TRUE(kpc.isEGS(7));
	EXPECT_TRUE(kpc.isBVS(8));
	//EXPECT_TRUE(kpc.isEVS(9));
	//EXPECT_TRUE(kpc.isBGS(10));
	//EXPECT_TRUE(kpc.isEGS(12));
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc,100));
}




TEST_F(TrajGenTest, test_liftoff) {
	Plan lpc("UF18");
	Position p0  = Position::makeLatLonAlt(32.6361, -97.0570, 0.00);
	Position p1  = Position::makeLatLonAlt(32.6361, -97.0570, 100.00);
	Position p2  = Position::makeLatLonAlt(32.6361, -97.0570, 2000.00);

	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,10.0);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,100);    	     lpc.addNavPoint(np2);

	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	double  vsAccel = Units::from("m/s", 2.0);		       // vertical speed acceleration
	double  bankAngle = Units::from("deg", 25.0);         // bank angle of aircraft in turn
	bool repair = false;
	//fpln(" ## test_liftoff: lpc = "+lpc.toStringVs());
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln(" ## test_liftoff: kpc = "+kpc.toStringVs());
	//DebugSupport.dumpPlan(kpc,"test_liftoff");
	EXPECT_EQ(4,kpc.size());
	EXPECT_NEAR(Units::from("fpm",600),kpc.vsOut(0),0.001);
	EXPECT_NEAR(Units::from("fpm",600),kpc.vsOut(1),0.001);
	EXPECT_NEAR(Units::from("fpm",1266.666),kpc.vsOut(2),0.001);
	EXPECT_NEAR(Units::from("fpm",1266.666),kpc.vsOut(3),0.001);

	EXPECT_NEAR(kpc.point(0).time(),0.0,0.001);
	EXPECT_NEAR(kpc.point(0).lat(),0.5696,0.001);
	EXPECT_NEAR(kpc.point(0).lon(),-1.6940,0.001);
	EXPECT_NEAR(kpc.point(0).alt(),0.0,0.001);
	EXPECT_NEAR(kpc.point(1).time(),9.1533,0.001);
	EXPECT_NEAR(kpc.point(1).lat(),0.5696,0.001);
	EXPECT_NEAR(kpc.point(1).lon(),-1.6940,0.001);
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isEVS(2));
	EXPECT_NEAR(kpc.point(2).time(),10.846,0.001);
	EXPECT_NEAR(kpc.point(3).time(),100.0,0.001);
}


TEST_F(TrajGenTest, test_liftoff2) {
	Plan lpc("liftoff2");
	Position p0  = Position::makeLatLonAlt(32.6361, -97.0570, 0.00);
	Position p1  = Position::makeLatLonAlt(32.6361, -97.0570, 0.00);
	Position p2  = Position::makeLatLonAlt(32.6361, -97.0570, 500.00);
	Position p3  = Position::makeLatLonAlt(32.6361, -97.0570, 4000.00);

	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,0.775);         	 lpc.addNavPoint(np1);
	NavPoint np2(p2,50);    	     lpc.addNavPoint(np2);
	NavPoint np3(p3,170);    	     lpc.addNavPoint(np3);

	double  gsAccel = Units::from("m/s", 4.0);            // ground speed acceleration
	double  vsAccel = Units::from("m/s", 2.0);		       // vertical speed acceleration
	double  bankAngle = Units::from("deg", 25.0);         // bank angle of aircraft in turn
	bool repair = false;
	//fpln(" ## test_liftoff2: lpc = "+lpc.toStringVs());
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	//fpln(" ## test_liftoff2: kpc = "+kpc.toStringVs());
	//DebugSupport.dumpPlan(kpc,"test_liftoff2");
	EXPECT_EQ(6,kpc.size());
	EXPECT_TRUE(kpc.isBVS(1));
	EXPECT_TRUE(kpc.isEVS(2));
	EXPECT_NEAR(Units::from("fpm",0),kpc.vsOut(0),0.001);
	EXPECT_NEAR(Units::from("fpm",0),kpc.vsOut(1),0.1);
	EXPECT_NEAR(Units::from("fpm",609.4),kpc.vsOut(2),0.1);   // BVS
	EXPECT_NEAR(Units::from("fpm",609.4),kpc.vsOut(3),0.1);   // EVS
	EXPECT_TRUE(kpc.isBVS(3));
	EXPECT_TRUE(kpc.isEVS(4));
}





TEST_F(TrajGenTest, test_BasicGs) {
	Plan lpc("U9");
	Position p0(LatLonAlt::make(34.1626, -118.9911, 5000));
	NavPoint np0(p0,1000.0);    	 lpc.addNavPoint(np0);
	double alt = Units::from("ft", 5000);
	Position p1 = p0.linearDist2D(M_PI/2,50000).mkAlt(alt);
	NavPoint np1(p1,2000.0);    	 lpc.addNavPoint(np1);
	Position p2 = p1.linearDist2D(M_PI/2,100000).mkAlt(2*alt);
	NavPoint np2(p2,3000.0);    	 lpc.addNavPoint(np2);
	Position p3 = p2.linearDist2D(M_PI/2,150000).mkAlt(3*alt);
	NavPoint np3(p3,4000.0);    	 lpc.addNavPoint(np3);
	NavPoint np23(lpc.position(3500), 3500); lpc.addNavPoint(np23);
	//fpln(" $$ test_BasicGs: lpc = "+lpc.toStringGs());
	double gsAccel = 1.2345;
	//bool repairGs = false;
	//bool useOffset = false;
	Plan kpc = TrajGen::generateGsTCPs(lpc,gsAccel,true);
	//fpln(" $$$ test_BasicGs: kpc = "+kpc.toStringGs());
	EXPECT_TRUE(kpc.isConsistent(false));
	EXPECT_TRUE(kpc.isGsContinuous(false));
	NavPoint np4(p3,5000.0);    	 lpc.addNavPoint(np4);
	//TrajGen::generateGsToZero = true;
	Plan kpc2 = TrajGen::generateGsTCPs(lpc,gsAccel,true);
	//fpln(" $$$ test_BasicGs: kpc2 = "+kpc2.toStringGs());
	EXPECT_TRUE(kpc2.isConsistent(false));
	EXPECT_TRUE(kpc2.isGsContinuous(false));
	EXPECT_TRUE(PlanUtil::basicCheck(lpc,kpc2,1000));
}


TEST_F(TrajGenTest, test_UF37) {
	double bankAngle = 0.34906585;
	double gsAccel = 1.471;
	double vsAccel = 1.471;
	Plan lpc("lpc");
	Position p0  = Position::makeLatLonAlt(32.705784000000, -97.4651320000000000, 1383.000000000000);
	Position p1  = Position::makeLatLonAlt(32.705784000000, -97.4651320000000000, 1383.000000000000);
	Position p2  = Position::makeLatLonAlt(32.705784000000, -97.4651320000000000, 1433.000000000000);
	Position p3  = Position::makeLatLonAlt(32.704747761064, -97.4621137800098900, 1547.290856735958);
	Position p4  = Position::makeLatLonAlt(32.690145338877, -97.4201981014435700, 3000.000000000000);
	NavPoint np0(p0,564.408);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,614.408);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,617.408);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,624.265);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,720.25);    	 lpc.addNavPoint(np4);  // 720.25    // 711.428
	//fpln(" $$ test_UF37: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//DebugSupport.dumpPlan(kpc,"kpc");
	EXPECT_FALSE(kpc.isLinear());
}




TEST_F(TrajGenTest, test_revertAllTCPs_info) {
	Plan VRD418 = Plan("VRD418");
	Position p0  = Position::makeLatLonAlt(33.942536111111, -118.4080744444444500, 126.000000000000);
	Position p1  = Position::makeLatLonAlt(34.962456944444, -116.5781663888888900, 31882.040346438740);
	Position p2  = Position::makeLatLonAlt(35.102350000000, -116.4038500000000200, 33055.574754773000);
	Position p3  = Position::makeLatLonAlt(35.675680555556, -115.6797527777777600, 35000.000000000000);
	NavPoint np0 = NavPoint(p0,1200.0000000000000000);
	np0 = np0.makeName("KLAX");    	 VRD418.addNavPoint(np0);
	VRD418.setInfo(0,"$bottomOfClimb");
	NavPoint np1 = NavPoint(p1,2096.0996071154477000);
	np1 = np1.makeName("DAG");    	 VRD418.addNavPoint(np1);
	NavPoint np2 = NavPoint(p2,2194.4353457539030000);
	np2 = np2.makeName("MISEN");    	 VRD418.addNavPoint(np2);
	NavPoint np3 = NavPoint(p3,2599.3255724257465000);
	np3 = np3.makeName("CLARR");    	 VRD418.addNavPoint(np3);
	VRD418.setInfo(1,"INFO--1");
	VRD418.setInfo(2,"INFO--2");
	VRD418.setInfo(3,"INFO--3");
	//f.pln(" $$$ test_revertAllTCPs_info: VRD418 = "+VRD418.toStringGs());
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 20);
	Plan kpc = TrajGen::makeKinematicPlan(VRD418, bankAngle, gsAccel, vsAccel, false, true, false);
	//f.pln(" $$$$$$ revertToLinearPlan: kpc = "+kpc.toStringGs());
	//Plan rev = Plan(kpc);
	Plan rev = PlanUtil::revertAllTCPs(kpc,false);
	//f.pln(" $$$ test_revertAllTCPs_info: rev = "+rev.toStringGs());
	EXPECT_TRUE(PlanUtil::checkReversion(rev,VRD418));
}


TEST_F(TrajGenTest, test_UF_3783) {  // point within turn ends up extremely close to added EOT
	Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);  // setMinTrkDeltaGen
	Plan::setMIN_GS_DELTA_GEN(0.5144444444444445);
	//Plan.setMIN_GS_DELTA_GEN(Units::from("kn",2.0));
	Plan::setMIN_VS_DELTA_GEN(0.254);
	Plan lpc("UF3783");
	Position p4  = Position::makeLatLonAlt(37.913099765912, -122.4937383941538100, 500.000000000000);
	Position p5  = Position::makeLatLonAlt(37.912521309669, -122.4932800518161700, 500.000000000000);
	Position p6  = Position::makeLatLonAlt(37.909352641728, -122.4907694967093900, 500.000000000000);
	Position p7  = Position::makeLatLonAlt(37.909352632484, -122.4907694893857100, 500.000000000000);
	Position p8  = Position::makeLatLonAlt(37.909147008080, -122.4906065809101100, 500.000000000000);
	Position p9  = Position::makeLatLonAlt(37.908946004546, -122.4904346333919400, 500.000000000000);
	Position p11  = Position::makeLatLonAlt(37.908474972137, -122.4900316941610500, 500.000000000000);
	NavPoint np4 = NavPoint(p4,25848.0000000000000000).makeName("AA");   lpc.addNavPoint(np4);
	NavPoint np5 = NavPoint(p5,25849.0232724072380000).makeName("BB");   lpc.addNavPoint(np5);
	NavPoint np6 = NavPoint(p6,25854.6284).makeName("CC");   // NEAR BOT
	int ix6 = lpc.addNavPoint(np6); lpc.setInfo(ix6,"info6");
	NavPoint np7 = NavPoint(p7,25854.6286).makeName("DD");   // NEAR CC/ BOT
	int ix7 = lpc.addNavPoint(np7); lpc.setInfo(ix7,"info7");
	NavPoint np8 = NavPoint(p8,25854.9922519038260000).makeName("Vert4");
	int ix8 = lpc.addNavPoint(np8); lpc.setInfo(ix8,"info8");
	NavPoint np9 = NavPoint(p9,25855.3560049723560000).makeName("FF");
	int ix9 = lpc.addNavPoint(np9); lpc.setInfo(ix9,"infoFF");
	NavPoint np11 = NavPoint(p11,25856.2084243260540000).makeName("GG"); lpc.addNavPoint(np11);
	EXPECT_TRUE(lpc.isConsistent(false));
	//DebugSupport.dumpPlan(lpc,"test_UF_3783_lpc");
    //int ixRemove = lpc.removeIfRedundant(10);
    //fpln(" $$  lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"lpc");
    EXPECT_EQ(7,lpc.size());
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
	//fpln(" $$ test_UF_3783: GENERATED: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"test_UF_3783_kpc");
	EXPECT_FALSE(kpc.isLinear());
	EXPECT_TRUE(kpc.isConsistent(false));
	int ixBOT = kpc.nextBOT(0);
	EXPECT_NEAR(Units::from("deg",37.909352642387),kpc.point(ixBOT).lat()  ,0.00000001);
	EXPECT_NEAR(Units::from("deg",-122.490769497472),kpc.point(ixBOT).lon(),0.00000001);
     // DOES NOT MERGE CC AND BOT -- Java does so
    EXPECT_EQ("CC",kpc.getPointName(3));
    EXPECT_EQ("DD",kpc.getPointName(4));
    EXPECT_EQ("Vert4",kpc.getPointName(5));   // THE MOT
    EXPECT_EQ("FF",kpc.getPointName(6));
    EXPECT_EQ("GG",kpc.getPointName(8));
    EXPECT_EQ("info6",kpc.getInfo(3));
    EXPECT_EQ("info7",kpc.getInfo(4));
    //EXPECT_EQ("info8"+Plan::MOTflag,kpc.getInfo(5));
    EXPECT_TRUE(kpc.isMOT(5));
    EXPECT_EQ("infoFF",kpc.getInfo(6));
    // ----- test cut -----
	Plan kpc2 = TrajGen::generateTurnTCPs(lpc, bankAngle);
    //fpln(" $$  kpc2 = "+kpc2.toStringGs());
    Plan cut = kpc2.cut(2,6);
		//fpln(" $$  cut = "+cut.toStringGs());
		for (int jj = 2; jj <= 6; jj++) {
			EXPECT_NEAR(kpc2.gsOut(jj),cut.gsOut(jj-2),0.00001);
		}
    // ----- test removeRedundantPoints ----
	lpc.removeRedundantPoints();
	//fpln(" $$ AFTER removeIfRedundant lpc = "+lpc.toStringGs());
	Plan::setMinDeltaGen_BackToDefaults();
}


TEST_F(TrajGenTest, test_UF74_wind) {
	double bankAngle = 0.3490658503988659;
	double gsAccel = 1.2; // 0.980665;  does not work            // 1.1 works
	double vsAccel = 0.4903325;
	Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);
	Plan::setMIN_GS_DELTA_GEN(0.5144444444444445);
	Plan::setMIN_VS_DELTA_GEN(0.254);
	Plan lpc("lpc");
	Position p21  = Position::makeLatLonAlt(32.789714559119, -96.8421797486203100, 1752.609071274549);
	Position p22  = Position::makeLatLonAlt(32.797623523318, -96.8190779942969900, 1315.761339093187);
	Position p23  = Position::makeLatLonAlt(32.800258902162, -96.8113764963756600, 1170.145428366067);
	Position p24  = Position::makeLatLonAlt(32.801293000000, -96.8083540000000000, 1113.000000000000);
	Position p25  = Position::makeLatLonAlt(32.801293000000, -96.8083540000000000, 1063.000000000000);
	Position p26  = Position::makeLatLonAlt(32.801293000000, -96.8083540000000000, 1063.000000000000);
	NavPoint np21(p21,1415.2717528620240000);    	 lpc.addNavPoint(np21);
	NavPoint np22(p22,1466.7312450545865000);    	 lpc.addNavPoint(np22);
	NavPoint np23(p23,1483.4980603568183000);    	 lpc.addNavPoint(np23);
	NavPoint np24(p24,1490.0409444155887000);    	 lpc.addNavPoint(np24);
	NavPoint np25(p25,1496.0409444155887000);    	 lpc.addNavPoint(np25);
	NavPoint np26(p26,1546.0409444155887000);    	 lpc.addNavPoint(np26);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"UF74_lpc");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, true);
	//fpln(" $$ kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"UF74_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_FALSE(kpc.isLinear());
	EXPECT_TRUE(kpc.isConsistent(false));
	Plan::setMinDeltaGen_BackToDefaults();
}


TEST_F(TrajGenTest, test_fixInfeasibleTurns) {
	double bankAngle = 0.34906585;
	double gsAccel = 2.0;
	double vsAccel = 0.5;
	Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);
	Plan::setMIN_GS_DELTA_GEN(5.144444444444445);
	Plan::setMIN_VS_DELTA_GEN(1.016);
	Plan lpc("lpc");
	Position p9  = Position::makeLatLonAlt(37.255323720132, -121.9388950527045700, 500.000000000000);
	Position p10  = Position::makeLatLonAlt(37.276969036962, -121.9352029068581200, 500.000000000000);
	Position p11  = Position::makeLatLonAlt(37.278396864437, -121.9346735276199000, 500.000000000000);
	Position p12  = Position::makeLatLonAlt(37.281355437901, -121.9317323616837400, 500.000000000000);
	Position p13  = Position::makeLatLonAlt(37.289938607242, -121.9231979753745900, 500.000000000000);
	NavPoint np9(p9,22375.3354127886340000);    	 lpc.addNavPoint(np9);
	NavPoint np10(p10,22408.1011779953150000);    	 lpc.addNavPoint(np10);
	NavPoint np11(p11,22410.3341776463400000);    	 int ix = lpc.addNavPoint(np11);
	lpc.setInfo(ix,"!!!!!!!!!!");
	NavPoint np12(p12,22415.9925596473100000);    	 int es = lpc.addNavPoint(np12);
	lpc.setInfo(es,"<EARLY_SLOWDOWN>");
	NavPoint np13(p13,22445.5178335319580000);    	 int todIx = lpc.addNavPoint(np13);
	lpc.setInfo(todIx,"<TOD>");
	//fpln(" $$ test_UF273: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"UF273_lpc");
	//bool repair = true;
    //int sz = lpc.size();
	PlanUtil::fixBadTurns(lpc, bankAngle);
	//fpln(" $$ test_UF273: AFTER REPAIR: lpc = "+lpc.toStringGs());
    //EXPECT_EQ(sz+1,lpc.size());   // Intersection point added
	//DebugSupport.dumpPlan(lpc,"UF273_lpc_repair");
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, true);
	//fpln(" $$ test_UF273_generation: kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"UF273_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_FALSE(kpc.isLinear());
	EXPECT_TRUE(kpc.isConsistent(false));
	Plan::setMinDeltaGen_BackToDefaults();
}



TEST_F(TrajGenTest, test_fixInfeasibleTurns2) {
	Plan lpc("lpc");
	Position p0  = Position::makeLatLonAlt(37.255323720132, -121.9388950527049900, 500.000000000000);
	Position p1  = Position::makeLatLonAlt(37.276969036962, -121.9352029068580000, 500.000000000000);
	Position p2  = Position::makeLatLonAlt(37.278396864437, -121.9346735276200000, 500.000000000000);
	Position p3  = Position::makeLatLonAlt(37.281822792790, -121.9352505209254600, 500.000000000000);
	Position p4  = Position::makeLatLonAlt(37.292721346711, -121.9371329984209500, 500.000000000000);
	NavPoint np0(p0,22375.3354127886340000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,22408.1011779953150000);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,22410.3341776463400000);    	 lpc.addNavPoint(np2);
	lpc.setInfo(2,"!!!!!!!!!!");
	NavPoint np3(p3,22415.9925596473100000);    	 lpc.addNavPoint(np3);
	lpc.setInfo(3,"<EARLY_SLOWDOWN>");
	NavPoint np4(p4,22445.5178335319580000);    	 lpc.addNavPoint(np4);
	lpc.setInfo(4,"<TOD>");
	//DebugSupport.dumpPlan(lpc,"test_fixInFeas3_lpc");
	//bool repair = true;
//	    int sz =lpc.size();
//		fpln(" $$ test_fixInFeas2: BEFORE: lpc = "+lpc.toStringGs());
	PlanUtil::fixBadTurns(lpc, bankAngle);
	//fpln(" $$ test_fixInFeas2: AFTER REPAIR: lpc = "+lpc.toStringGs());
	int numBadVerts = PlanUtil::countBadTurns(lpc, bankAngle);
	EXPECT_EQ(0,numBadVerts);
}


TEST_F(TrajGenTest, test_fixInfeasibleTurns3) {
	Plan lpc("lpc");
	Position p0  = Position::makeLatLonAlt(37.255323720132, -121.9388950527049900, 500.000000000000);
	Position p1  = Position::makeLatLonAlt(37.276969036962, -121.9352029068580000, 500.000000000000);
	Position p2  = Position::makeLatLonAlt(37.278396864437, -121.9346735276200000, 500.000000000000);
	Position p3  = Position::makeLatLonAlt(37.281822792790, -121.9352505209254600, 500.000000000000);
	Position p4  = Position::makeLatLonAlt(37.292721346711, -121.9371329984209500, 500.000000000000);
	NavPoint np0(p0,22375);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,22408);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,22410);    	 lpc.addNavPoint(np2);
	lpc.setInfo(2,"!!!!!!!!!!");
	NavPoint np3(p3,22415);    	 lpc.addNavPoint(np3);
	lpc.setInfo(3,"<EARLY_SLOWDOWN>");
	NavPoint np4(p4,22445);    	 lpc.addNavPoint(np4);
	lpc.setInfo(4,"<TOD>");
	Position pos1B = lpc.position(22409);
	NavPoint np1B(pos1B,22409); np1B = np1B.makeName("EXTRA");
	lpc.addNavPoint(np1B);
	Position pos2B = lpc.position(22412);
	//NavPoint np2B(pos2B,22412); // .makeName("EXTRA_AFTER");
	//int ix2B = lpc.addNavPoint(np2B);
	//lpc.setInfo(ix2B,"EXTRA_AFTER");
	//DebugSupport.dumpPlan(lpc,"test_fixInFeas3_lpc");
	//bool repair = true;
    //int sz =lpc.size();
	int numBadVerts = PlanUtil::countBadTurns(lpc, bankAngle);
	EXPECT_EQ(2,numBadVerts);
	//fpln(" $$ test_fixInFeas3: BEFORE: lpc = "+lpc.toStringGs());
	//fpln("\n===========================================");
	PlanUtil::fixBadTurns(lpc, bankAngle);
	//fpln("===========================================");
	//DebugSupport.dumpPlan(lpc,"test_fixInFeas3_AFTER_FIX_ lpc");
	//fpln(" $$ test_fixInFeas3: AFTER REPAIR: lpc = "+lpc.toStringGs());
	numBadVerts = PlanUtil::countBadTurns(lpc, bankAngle);
	EXPECT_EQ(0,numBadVerts);
}


TEST_F(TrajGenTest, test_UF358_infeasibleTurns) {

	double bankAngle = 0.3490658503988659;
	Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);
	Plan::setMIN_GS_DELTA_GEN(0.5144444444444445);
	Plan::setMIN_VS_DELTA_GEN(0.254);
	Plan lpc("lpc");
	Position p7  = Position::makeLatLonAlt(37.457345755058, -122.1558672343925300, 500.000000000000);
	Position p8  = Position::makeLatLonAlt(37.458352783892, -122.1575898287455700, 500.000000000000);
	Position p9  = Position::makeLatLonAlt(37.497462329161, -122.2296263160580600, 500.000000000000);
	Position p10  = Position::makeLatLonAlt(37.511036107119, -122.2283915971140300, 500.000000000000);
	Position p11  = Position::makeLatLonAlt(37.515011797515, -122.2337549262338100, 500.000000000000);
	Position p12  = Position::makeLatLonAlt(37.515013592212, -122.2337573475376800, 500.000000000000);
	Position p13  = Position::makeLatLonAlt(37.515062994388, -122.2338400795897800, 500.000000000000);
	Position p14  = Position::makeLatLonAlt(37.521566859122, -122.2448067294901400, 500.000000000000);
	Position p15  = Position::makeLatLonAlt(37.527603247628, -122.2549883744947400, 273.337103855783);
	Position p16  = Position::makeLatLonAlt(37.530328471891, -122.2545954577705200, 211.668551928128);
	Position p17  = Position::makeLatLonAlt(37.531700000000, -122.2516000000000000, 150.000000000000);
	Position p18  = Position::makeLatLonAlt(37.531700000000, -122.2516000000000000, 100.000000000000);
	Position p19  = Position::makeLatLonAlt(37.531700000000, -122.2516000000000000, 100.000000000000);
	NavPoint np7(p7,22447.1551689628000000);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,22449.7024683387500000);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,22553.6001323829200000);    	 lpc.addNavPoint(np9);
	NavPoint np10(p10,22574.0137432701200000);    	 lpc.addNavPoint(np10);
	NavPoint np11(p11,22582.7479369941760000);    	 int ix11 = lpc.addNavPoint(np11);
	lpc.setInfo(ix11,"<EARLY_SLOWDOWN>");
	NavPoint np12(p12,22582.7550286153560000);    	 lpc.addNavPoint(np12);
	NavPoint np13(p13,22582.9766308441250000);    	 lpc.addNavPoint(np13);
	NavPoint np14(p14,22612.2781880700680000);    	 int ix14 = lpc.addNavPoint(np14);
	lpc.setInfo(ix14,"<TOD>");
	NavPoint np15(p15,22639.4777356073760000);    	 int ix15 = lpc.addNavPoint(np15);
	lpc.setVertexRadius(ix15,236.40019567689424);
	lpc.setInfo(ix15,"<arrFixVirtual>");
	NavPoint np16(p16,22646.8779618386900000);    	 int ix16 = lpc.addNavPoint(np16);
	lpc.setVertexRadius(ix16,269.5443080805979);
	lpc.setInfo(ix16,"<arrFix>");
	NavPoint np17(p17,22654.2781880700680000);    	 int ix17 = lpc.addNavPoint(np17);
	lpc.setInfo(ix17,"<BOD>");
	NavPoint np18(p18,22660.2781880700680000);    	 lpc.addNavPoint(np18);
	NavPoint np19(p19,22710.2781880700680000);    	 int ix19 = lpc.addNavPoint(np19);
	lpc.setInfo(ix19,"<virtualBOOT>");
	//fpln(" $$ test_UF358: BEFORE REPAIR: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"test_UF358_BEFORE");
	int numFixedVerts = PlanUtil::infeasibleTurns(lpc,1, lpc.size()-2, bankAngle, true);
	EXPECT_EQ(2,numFixedVerts);   // Intersection point added
	//fpln("\n--------------");
	//fpln(" $$ test_UF358: AFTER REPAIR: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"test_UF358_AFTER");
	int numBadVerts = PlanUtil::infeasibleTurns(lpc,1, lpc.size()-2, bankAngle, false);
	EXPECT_EQ(0,numBadVerts);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, true);
	//fpln(" $$ kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_FALSE(kpc.isLinear());
	EXPECT_TRUE(kpc.isConsistent(false));
	Plan::setMinDeltaGen_BackToDefaults();
}





TEST_F(TrajGenTest, test_UF3873_huh) {    // ***** TEMP ****
	double bankAngle = 0.3490658503988659;
	double gsAccel = 0.980665;
	double vsAccel = 0.4903325;
	Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);
	Plan::setMIN_GS_DELTA_GEN(0.5144444444444445);
	Plan::setMIN_VS_DELTA_GEN(0.254);
	Plan lpc("lpc");
	Position p7  = Position::makeLatLonAlt(37.518668296474, -122.2077404958450600, 1500.000000000000);
	Position p8  = Position::makeLatLonAlt(37.541905387064, -122.2246202542356000, 1500.000000000000);
	Position p9  = Position::makeLatLonAlt(37.571303757908, -122.2525085881628500, 1500.000000000000);
	Position p10  = Position::makeLatLonAlt(37.696485217941, -122.3372226504242200, 1500.000000000000);
	Position p11  = Position::makeLatLonAlt(37.697209773622, -122.3377517354516800, 1500.000000000000);
	Position p12  = Position::makeLatLonAlt(37.691723873046, -122.3377843084300000, 1500.000000000000);
	Position p13  = Position::makeLatLonAlt(37.733431704407, -122.3665062832843700, 1500.000000000000);
	Position p14  = Position::makeLatLonAlt(37.787916188294, -122.4040828988886900, 207.145428365600);
	Position p15  = Position::makeLatLonAlt(37.790300000000, -122.4058000000000000, 150.000000000000);
	Position p16  = Position::makeLatLonAlt(37.790300000000, -122.4058000000000000, 100.000000000000);
	Position p17  = Position::makeLatLonAlt(37.790300000000, -122.4058000000000000, 100.000000000000);
	NavPoint np10(p10,26401.1687492855160000);    	 lpc.addNavPoint(np10);
	NavPoint np11(p11,26402.4239565379470000);    	 lpc.addNavPoint(np11);
	NavPoint np12(p12,26410.6528982226200000);    int ix12 =	lpc.addNavPoint(np12);
	lpc.setInfo(ix12,"<descentOffset>");
	NavPoint np13(p13,26481.8960504787360000);    	 lpc.addNavPoint(np13);
	NavPoint np14(p14,26637.0385990748620000);    	 lpc.addNavPoint(np14);
	NavPoint np15(p15,26643.8960504787360000);    	 lpc.addNavPoint(np15);
	NavPoint np16(p16,26649.8960504787360000);    	 lpc.addNavPoint(np16);
	NavPoint np17(p17,26699.8960504787360000);    	 lpc.addNavPoint(np17);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	bool repair = false;
	int numBadVerts = PlanUtil::infeasibleTurns(lpc,1, lpc.size()-2, bankAngle, repair);
	EXPECT_EQ(2,numBadVerts);
	//DebugSupport.dumpPlan(lpc, "test_UF3873_huh");
	int numFixed = PlanUtil::infeasibleTurns(lpc, 1, lpc.size()-2, bankAngle, true);
	EXPECT_EQ(2,numFixed);     // number repaired
	//fpln(" $$ AFTER fix: lpc = "+lpc.toStringGs());
	numBadVerts = PlanUtil::infeasibleTurns(lpc, 1, lpc.size()-2, bankAngle, false);
	EXPECT_EQ(0,numBadVerts);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, true);
	//fpln(" $$ kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"test_UF3873_kpc");
	EXPECT_FALSE(kpc.hasError());
	EXPECT_FALSE(kpc.isLinear());
	EXPECT_TRUE(kpc.isConsistent(false));
	Plan::setMinDeltaGen_BackToDefaults();
}



TEST_F(TrajGenTest, test_fixInfeasibleTurns4) {    // ***** TEMP ****
	Plan lpc("UF358");
	Position p9  = Position::makeLatLonAlt(37.515011797515, -122.2337549262340000, 500.000000000000);
	//Position p10  = Position::makeLatLonAlt(37.515013592212, -122.2337573475380200, 500.000000000000);
	//Position p11  = Position::makeLatLonAlt(37.515062994388, -122.2338400795900000, 500.000000000000);
	Position p12  = Position::makeLatLonAlt(37.521566859122, -122.2448067294899800, 500.000000000000);
	Position p13  = Position::makeLatLonAlt(37.527603247628, -122.2549883744950000, 273.337103855783);
	Position p14  = Position::makeLatLonAlt(37.530328471891, -122.2545954577710000, 211.668551928128);
	Position p15  = Position::makeLatLonAlt(37.533700000000, -122.2516000000000000, 150.000000000000);
	Position p16  = Position::makeLatLonAlt(37.533700000000, -122.2516000000000000, 100.000000000000);
	Position p17  = Position::makeLatLonAlt(37.533700000000, -122.2516000000000000, 100.000000000000);
	NavPoint np9(p9,22582.7479364305620000);    	 int ix9 = lpc.addNavPoint(np9);
	lpc.setInfo(ix9,"<EARLY_SLOWDOWN>");
	//NavPoint np10(p10,22582.7550280517300000);    	 lpc.addNavPoint(np10);
	//NavPoint np11(p11,22582.9766302805000000);    	 lpc.addNavPoint(np11);
	NavPoint np12(p12,22612.2781875064430000);    	 lpc.addNavPoint(np12);
	NavPoint np13(p13,22639.4777350437500000); np13 = np13.makeName("UGLY");
	lpc.addNavPoint(np13);
	NavPoint np14(p14,22646.8779612750660000);    	 lpc.addNavPoint(np14);
	NavPoint np15(p15,22654.2781875064430000);    	 lpc.addNavPoint(np15);
	NavPoint np16(p16,22660.2781875064430000); np16 = np16.makeName("K441");    	 lpc.addNavPoint(np16);
	NavPoint np17(p17,22710.2781875064430000);    	 lpc.addNavPoint(np17);
	//fpln(" $$ BEFORE fix: lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "test_fixInfeasibleTurns4_BEFORE");
	int numBadVerts = PlanUtil::countBadTurns(lpc, bankAngle);
	EXPECT_EQ(1,numBadVerts);
	int numFixed = PlanUtil::infeasibleTurns(lpc, 1, lpc.size()-2, bankAngle, true);
	//DebugSupport.dumpPlan(lpc, "test_fixInfeasibleTurns4_AFTER");
	EXPECT_EQ(1,numFixed);     // number repaired
	//fpln("test_UF3873_sw: ===============================================================");
	//fpln(" $$ AFTER fix: lpc = "+lpc.toStringGs());
	numBadVerts = PlanUtil::infeasibleTurns(lpc, 0, lpc.size()-2, bankAngle, false);
	EXPECT_EQ(0,numBadVerts);
}



TEST_F(TrajGenTest, test_fixInfeasibleTurns5) {    // ***** TEMP ****
	Plan lpc("lpc");
	Position p0  = Position::makeLatLonAlt(37.524827617569, -122.2330882155913200, 500.000000000000);
	Position p1  = Position::makeLatLonAlt(37.523958123914, -122.2520671633027700, 500.000000000000);
	Position p2  = Position::makeLatLonAlt(37.524467827091, -122.2537461855331600, 273.337103855783);
	Position p3  = Position::makeLatLonAlt(37.530328471891, -122.2545954577710000, 211.668551928128);
	Position p4  = Position::makeLatLonAlt(37.53700000000, -122.2536000000000000, 150.000000000000);
	NavPoint np0(p0,22582.7479364305620000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,22612.2781875064430000);    int ix1 = lpc.addNavPoint(np1);
    lpc.setInfo(ix1,"INFO_1");
	NavPoint np2(p2,22639.4777350437500000);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,22646.8779612750660000);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,22654.2781875064430000);    	 lpc.addNavPoint(np4);
	//DebugSupport.dumpPlan(lpc, "test_fixInfeasibleTurns5_BEFORE");
	int numBadVerts = PlanUtil::countBadTurns(lpc, bankAngle);
	EXPECT_EQ(1,numBadVerts);
	int numFixed = PlanUtil::infeasibleTurns(lpc, 1, lpc.size()-2, bankAngle, true);
	//DebugSupport.dumpPlan(lpc, "test_fixInfeasibleTurns5_AFTER");
	EXPECT_EQ(1,numFixed);     // number repaired
	//fpln("test_UF3873_sw: ===============================================================");
	//fpln(" $$ AFTER fix: lpc = "+lpc.toStringGs());
	numBadVerts = PlanUtil::countBadTurns(lpc, bankAngle);
	EXPECT_EQ(0,numBadVerts);
}


TEST_F(TrajGenTest, test_fixInfeasibleTurns6) {    // ***** TEMP ****
	double bankAngle = 0.3490658503988659;
	Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);
	Plan lpc("UF358");
	Position p8  = Position::makeLatLonAlt(37.497462329466, -122.2296263157830000, 500.000000000000);
	Position p9  = Position::makeLatLonAlt(37.515011797515, -122.2337549262340000, 500.000000000000);
	Position p10  = Position::makeLatLonAlt(37.515013592212, -122.2337573475380200, 500.000000000000);
	Position p11  = Position::makeLatLonAlt(37.515062994388, -122.2338400795900000, 500.000000000000);
	Position p12  = Position::makeLatLonAlt(37.521566859122, -122.2448067294899800, 500.000000000000);
	NavPoint np8(p8,22553.6001323681600000);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,22582.7479364305620000);    	 lpc.addNavPoint(np9);
	NavPoint np10(p10,22582.7550280517300000);    	 lpc.addNavPoint(np10);
	NavPoint np11(p11,22582.9766302805000000);    	 lpc.addNavPoint(np11);
	NavPoint np12(p12,22612.2781875064430000);    	 lpc.addNavPoint(np12);
	int numBadVerts = PlanUtil::infeasibleTurns(lpc,0, lpc.size()-2, bankAngle, false);
	//fpln(" $$ lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"test_fixInfeasibleTurns5");
	EXPECT_EQ(2,numBadVerts);
	//fpln(" $$ ================================= ");
	int numFixed = PlanUtil::infeasibleTurns(lpc, 1, lpc.size()-2, bankAngle, true);
	EXPECT_EQ(1,numFixed);     // number repaired
	numBadVerts = PlanUtil::infeasibleTurns(lpc,0, lpc.size()-2, bankAngle, false);
	EXPECT_EQ(0,numBadVerts);     // number repaired
	//fpln(" $$ lpc = "+lpc.toStringGs());
	Plan::setMinDeltaGen_BackToDefaults();
}




TEST_F(TrajGenTest, test_revertFromKinFileRed2) {
	Plan p("A1331");
	Position p0  = Position::makeLatLonAlt(42.632524000000, -71.1786380000000000, 10056.394589000000);
	Position p1  = Position::makeLatLonAlt(42.639886000000, -71.1834220000000000, 10225.578358000000);
	Position p2  = Position::makeLatLonAlt(42.731772000000, -71.2432520000000000, 12979.841459000000);
	Position p3  = Position::makeLatLonAlt(42.732459000000, -71.2437000000000000, 12999.549639000000);
	Position p4  = Position::makeLatLonAlt(42.733145000000, -71.2441480000000000, 13018.357097000000);
	Position p5  = Position::makeLatLonAlt(42.758527000000, -71.2607100000000000, 13693.597708000000);
	Position p6  = Position::makeLatLonAlt(42.776635000000, -71.2800420000000000, 14244.776607000000);
	Position p7  = Position::makeLatLonAlt(42.787065000000, -71.3079830000000000, 14791.408571000000);
	Position p8  = Position::makeLatLonAlt(42.799832000000, -71.3754850000000000, 15981.642903000002);
	Position p9  = Position::makeLatLonAlt(43.064155000000, -72.8303900000000000, 31007.548433000000);
	Position p10  = Position::makeLatLonAlt(43.256037000000, -73.9806420000000000, 36000.000000000000);
	std::pair<NavPoint,TcpData> np0 = Plan::makeBGS("", p0,249.753541000000, 0.089983000000);    	 int ix0 = p.add(np0);
	p.setInfo(ix0,"$targetCASReached");
	NavPoint np1(p1,255.6168290000000000);    	 p.addNavPoint(np1);
	std::pair<NavPoint,TcpData> np2 = Plan::makeBVS("", p2,327.206306000000, -1.000000000000);    	 p.add(np2);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEGSBGS("", p3,327.730272000000, 0.070483084263);    	 p.add(np3);
	std::pair<NavPoint,TcpData> np4 = Plan::makeEVS( p4,328.253480578877);    	 p.add(np4);
	Position turnCenter5 = Position::makeLatLonAlt(42.735482000000, -71.3261150000000000, 19.000000000000);
	std::pair<NavPoint,TcpData> np5 = Plan::makeBOT("", p5,347.525181909701, -5919.666128000000, turnCenter5); p.add(np5);
	NavPoint np6(p6,363.2834533318464000);    	 int ix6 = p.addNavPoint(np6);
	p.setVertexRadius(ix6,5919.666128);
	//p.setInfo(ix6,Plan::MOTflag);
	p.setMOT(ix6);
	std::pair<NavPoint,TcpData> np7 = Plan::makeEOT( p7,378.936042393645);    	 p.add(np7);
	std::pair<NavPoint,TcpData> np8 = Plan::makeBVS("", p8,413.098308103629, -1.000000000000);    	 p.add(np8);
	std::pair<NavPoint,TcpData> np9 = Plan::makeEVS( p9,1054.343127460486);    	 p.add(np9);
	std::pair<NavPoint,TcpData> np10 = Plan::makeEGS( p10,1474.568943378636);    p.add(np10);
	//fpln(" $$$$ test_revertFromKinFile: p = "+p.toStringGs());
	PlanUtil::fixGs_continuity(p, 0, 3, 5);
	//fpln(" $$$$ test_revertFromKinFile: AFTER fixGs_continuity: p = "+p.toStringGs());
	EXPECT_TRUE(p.isWeakConsistent(false));
	//DebugSupport.dumpPlan(p,"test_revertFromKinFile_p");
	Plan noTCPs = PlanUtil::revertAllTCPs(p);
	//DebugSupport.dumpPlan(noTCPs,"test_revertFromKinFile_noTCPS");
	//fpln(" $$$$ test_revertFromKinFile: noTCPs = "+noTCPs.toStringGs());
	//bool chkRev =
			PlanUtil::checkReversion(noTCPs,p,false);
	//double distBetw =
			PlanUtil::distanceBetween(noTCPs,p);
	//fpln(" ## test_revertFromKinFile:  distBetw = "+distBetw);
	//fpln(" $$$$ test_revertFromKinFile: chkRev = "+chkRev);
	//			p = PlanUtil::fixConsistent(p, 0.05, 0.05);
	//fpln("p="+p);
	//EXPECT_TRUE(p.isWeakConsistent(false));  // TODO: -- add back
}


TEST_F(TrajGenTest, test_revertFromKinFileRed3) {
	Plan p("A1331");
	Position p0  = Position::makeLatLonAlt(42.632524000000, -71.1786380000000000, 10056.394589000000);
	Position p1  = Position::makeLatLonAlt(42.639886000000, -71.1834220000000000, 10225.578358000000);
	Position p2  = Position::makeLatLonAlt(42.732458502126, -71.2436996753271600, 12999.985396908374);
	Position p3  = Position::makeLatLonAlt(42.732459000000, -71.2437000000000000, 12999.998718692103);
	Position p4  = Position::makeLatLonAlt(42.758527000000, -71.2607100000000000, 13693.597708000000);
	Position p5  = Position::makeLatLonAlt(42.776635000000, -71.2800420000000000, 14244.776607000000);
	Position p6  = Position::makeLatLonAlt(42.787065000000, -71.3079830000000000, 14791.408571000000);
	Position p7  = Position::makeLatLonAlt(42.926436124301, -72.0581621289065600, 360760.768856983000);
//		Position p8  = Position::makeLatLonAlt(43.256037000000, -73.9806420000000000, 36000.000000000000);
	NavPoint np0(p0,249.7535410000000000);    	 int ix0 = p.addNavPoint(np0);
	p.setInfo(ix0,"$targetCASReached");
	NavPoint np1(p1,255.3655637817034300);    	 p.addNavPoint(np1);
	NavPoint np2(p2,325.9492431462810500);    	 p.addNavPoint(np2);
	NavPoint np3(p3,325.9496228133033400);    	 p.addNavPoint(np3);
	Position turnCenter4 = Position::makeLatLonAlt(42.735482000000, -71.3261150000000000, 19.000000000000);
	std::pair<NavPoint,TcpData> np4 = Plan::makeBOT("", p4,339.200629714639, -5919.666128000000, turnCenter4);    	 int ix4 = p.add(np4);
    p.setInfo(ix4,"iBOT");
	NavPoint np5(p5,349.8309031853596600);    	 int ix5 = p.addNavPoint(np5);
    //p.setInfo(ix5,"P5"+Plan::MOTflag);
	p.setMOT(ix5);
	p.setVertexRadius(ix5,5919.666128);
	std::pair<NavPoint,TcpData> np6 = Plan::makeEOT( p6,360.461364719547);    	 int ix6 = p.add(np6);
    p.setInfo(ix6,"iEOT");
	NavPoint np7(p7,620.5206596856427000);    	 p.addNavPoint(np7);
//		NavPoint np8(p8,1281.5740146160524000);    	 int ix8 = p.addNavPoint(np8);
	//fpln(" $$$$ test_revertFromKinFile: p = "+p.toStringGs());
    EXPECT_TRUE(p.isWeakConsistent(false));
	//DebugSupport.dumpPlan(p,"test_revertFromKinFile_p");
	Plan noTCPs = PlanUtil::revertAllTCPs(p);
	//DebugSupport.dumpPlan(noTCPs,"test_revertFromKinFile_noTCPS");
	//fpln(" $$$$ test_revertFromKinFile: noTCPs = "+noTCPs.toStringGs());
	//bool chkRev =
			PlanUtil::checkReversion(noTCPs,p,false);
	//double distBetw =
			PlanUtil::distanceBetween(noTCPs,p);
	//fpln(" ## test_revertFromKinFile:  distBetw = "+distBetw);
	//fpln(" $$$$ test_revertFromKinFile: chkRev = "+chkRev);
}


TEST_F(TrajGenTest, test_revTOC) {
	Plan lpc("agh");
	Position p2  = Position::makeLatLonAlt(37.999500000000, -122.5463000000000000, 134.793072049321);
	Position p3  = Position::makeLatLonAlt(37.997489882732, -122.5449403583873500, 266.543990288480);
	Position p4  = Position::makeLatLonAlt(37.997093604842, -122.5446362767961100, 279.323563872963);
	Position p5  = Position::makeLatLonAlt(37.996725555244, -122.5442796154388700, 291.061500329137);
	Position p6  = Position::makeLatLonAlt(37.988034327240, -122.5348713851046800, 485.428870186460);
	Position p7  = Position::makeLatLonAlt(37.987646437104, -122.5344977995149700, 493.051637050157);
	Position p8  = Position::makeLatLonAlt(37.987265758700, -122.5342080067752000, 499.999999999988);
	Position p9  = Position::makeLatLonAlt(37.987227591210, -122.5341825828742700, 499.999999999988);
	Position p10  = Position::makeLatLonAlt(37.971822198792, -122.5240485822427600, 500.000000000000);
	NavPoint np2(p2,32082.8023362311360000);    	 int ix2 = lpc.addNavPoint(np2);
	lpc.setInfo(ix2,"<BOC>");
	Position turnCenter3 = Position::makeLatLonAlt(37.999501646776, -122.5401504583876400, 150.000000000000);
	std::pair<NavPoint,TcpData> np3 = Plan::makeBOT("", p3,32088.947662752562, -475.284212041871, turnCenter3);    lpc.add(np3);
	NavPoint np4(p4,32090.1976503051000000);    	 int ix4 = lpc.addNavPoint(np4);
	lpc.setInfo(ix4,"<depFix>.<MOT>.");
	std::pair<NavPoint,TcpData> np5 = Plan::makeEOT( p5,32091.447637858960);    	 lpc.add(np5);
	Position turnCenter6 = Position::makeLatLonAlt(37.985258420504, -122.5390000967131100, 211.668551927725);
	std::pair<NavPoint,TcpData> np6 = Plan::makeBOT("", p6,32122.268821795602, 475.284212044305, turnCenter6);    lpc.add(np6);
	NavPoint np7(p7,32123.5833536402570000);    	 int ix7 = lpc.addNavPoint(np7);
	lpc.setInfo(ix7,".<MOT>.");
	NavPoint np8(p8,32124.7815857952100000);    	 int ix8 = lpc.addNavPoint(np8);
	lpc.setInfo(ix8,"<TOC>");
	std::pair<NavPoint,TcpData> np9 = Plan::makeEOT( p9,32124.846536619105);    	 lpc.add(np9);
	NavPoint np10(p10,32150.9926813249080000);    	 lpc.addNavPoint(np10);
	//fpln(" $$$$$$$ lpc = "+lpc);
	Plan noTCPS = PlanUtil::revertAllTCPs(lpc);
	//fpln(" $$$$$$$ refPlanLinear = "+noTCPS);
	// make sure we find the indices for TOC and TOD
	int ixTOC = noTCPS.findInfo("TOC");	// TOC index
	EXPECT_TRUE(ixTOC>=0);
}




	TEST_F(TrajGenTest, test_JavaCpp_unsync) {
		Position p0(LatLonAlt::make(1.516640, -0.473195, 19292.623870));
		Position p1(LatLonAlt::make(1.521673, -0.471533, 19289.164984));
		Position p2(LatLonAlt::make(1.542203, -0.464751, 19275.054672));
		Position p3(LatLonAlt::make(1.554821, -0.469124, 19245.184162));
		Position p4(LatLonAlt::make(2.831986, -0.236309, 12703.529226));
		Position p5(LatLonAlt::make(2.833700, -0.236000, 12695.143200));
		NavPoint np0(p0,222.326640);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,225.501640);    	 lpc.addNavPoint(np1);
		NavPoint np2(p2,238.453857);    	 lpc.addNavPoint(np2);
		NavPoint np3(p3,246.453650);    	 lpc.addNavPoint(np3);
		NavPoint np4(p4,1024.161981);    	 lpc.addNavPoint(np4);
		NavPoint np5(p5,1025.205022);    	 lpc.addNavPoint(np5);
		bankAngle = Units::from("deg", 25);
		gsAccel = 2;
		vsAccel = 1;
		//DebugSupport::dumpPlan(lpc, "test_JavaCpp_unsync_lpc");
		Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, false);
		//DebugSupport::dumpPlan(kpc, "test_JavaCpp_unsync_kpc");
		EXPECT_TRUE(kpc.hasMessage());
		//fpln(" $$$$$$$$$$ test_JavaCpp_unsync: msg = "+kpc.getMessageNoClear());
	}

	TEST_F(TrajGenTest, test_UF150) {
		double bankAngle = 0.43633231;
		double gsAccel = 4.0;
		double vsAccel = 2.0;
		Plan::setMIN_TRK_DELTA_GEN(0.017453292519943295);
		Plan::setMIN_GS_DELTA_GEN(5.144444444444445);
		Plan::setMIN_VS_DELTA_GEN(1.016);
		Plan lpc = Plan("lpc");
		Position p0  = Position::makeLatLonAlt(32.000000000000, -96.0000000000000000, 1200.000000000000);
		Position p1  = Position::makeLatLonAlt(32.000000000000, -96.0000000000000000, 1200.000000000000);
		Position p2  = Position::makeLatLonAlt(32.000000000000, -96.0000000000000000, 1250.000000000000);
		Position p3  = Position::makeLatLonAlt(31.997257019438, -95.9999999999995700, 1720.010964309356);
		Position p4  = Position::makeLatLonAlt(31.997257004880, -95.9980732636498900, 2000.000000000000);
		Position p5  = Position::makeLatLonAlt(31.997256978415, -95.9967656333139700, 2000.000000000000);
		Position p6  = Position::makeLatLonAlt(32.003523969866, -95.9968475879886000, 2000.000000000000);
		Position p7  = Position::makeLatLonAlt(32.003384251434, -95.9975645954790900, 2000.000000000000);
		Position p8  = Position::makeLatLonAlt(32.002963515555, -95.9997235910196000, 2000.000000000000);
		Position p9  = Position::makeLatLonAlt(32.002909647228, -96.0000000000004400, 1979.846532160011);
		Position p10  = Position::makeLatLonAlt(32.000166666667, -96.0000000000000300, 1750.000000000000);
		Position p11  = Position::makeLatLonAlt(32.000166666667, -96.0000000000000300, 1700.000000000000);
		Position p12  = Position::makeLatLonAlt(32.000166666667, -96.0000000000000300, 1700.000000000000);
		//		NavPoint np0 = NavPoint(p0,173.0000000000000000);    	 int ix0 = lpc.addNavPoint(np0);
		//		lpc.setInfo(ix0,"<virtualBOOT>");
		//		NavPoint np1 = NavPoint(p1,223.0000000000000000).makeName("VP01");    	 lpc.addNavPoint(np1);
		NavPoint np2 = NavPoint(p2,229.0000000000000000);    	 int ix2 = lpc.addNavPoint(np2);
		lpc.setInfo(ix2,"<BOC>");
		NavPoint np3 = NavPoint(p3,246.0871131877572000);    	 int ix3 = lpc.addNavPoint(np3);
		lpc.setInfo(ix3,"<depFix>");
		NavPoint np4 = NavPoint(p4,256.2660339097600400);    	 int ix4 = lpc.addNavPoint(np4);
		lpc.setInfo(ix4,"<TOC>");
		NavPoint np5 = NavPoint(p5,258.3823286296727700);    	 int ix5 = lpc.addNavPoint(np5);
		lpc.setVertexRadius(ix5,134.2868845420679);
		lpc.setInfo(ix5,"<depFixVirtual>");
		NavPoint np6 = NavPoint(p6,270.3426677437370000);    	 int ix6 = lpc.addNavPoint(np6);
		lpc.setVertexRadius(ix6,109.36099854754514);
		lpc.setInfo(ix6,"<arrFixVirtual>");
		NavPoint np7 = NavPoint(p7,271.5332485776299000);    	 int ix7 = lpc.addNavPoint(np7);
		lpc.setInfo(ix7,"<EARLY_SLOWDOWN>");
		NavPoint np8 = NavPoint(p8,279.0156280069243000);    	 int ix8 = lpc.addNavPoint(np8);
		lpc.setInfo(ix8,"<TOD>");
		NavPoint np9 = NavPoint(p9,279.9735769882740000);    	 int ix9 = lpc.addNavPoint(np9);
		lpc.setInfo(ix9,"<arrFix>");
		NavPoint np10 = NavPoint(p10,290.8988059909243000);    	 int ix10 = lpc.addNavPoint(np10);
		lpc.setInfo(ix10,"<BOD>");
		NavPoint np11 = NavPoint(p11,296.8988059909243000).makeName("VP02");    	 lpc.addNavPoint(np11);
		NavPoint np12 = NavPoint(p12,346.8988059909243000);    	 int ix12 = lpc.addNavPoint(np12);
		lpc.setInfo(ix12,"<virtualBOOT>");
		//f.pln(" $$ test_UF150: lpc = "+lpc.toStringProfile());
		//DebugSupport.dumpPlan(lpc,"UF150_lpc");
		Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, true, true);
		//f.pln(" $$ test_UF150: kpc = "+kpc.toStringGs());
		//DebugSupport.dumpPlan(kpc,"UF150_kpc");
		EXPECT_FALSE(kpc.hasError());
		EXPECT_FALSE(kpc.isLinear());
		EXPECT_TRUE(kpc.isConsistent(false));
		Plan::setMinDeltaGen_BackToDefaults();
	}



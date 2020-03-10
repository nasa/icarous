/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect4.h"
#include "Units.h"
#include "Plan.h"
#include "PlanUtil.h"
#include <cmath>
#include <gtest/gtest.h>
#include "format.h"
#include "Kinematics.h"
#include "KinematicsPosition.h"
#include "Projection.h"
#include "KinematicsPosition.h"
#include "TrajTemplates.h"
#include "NavPoint.h"
#include "TrajGen.h"

using namespace larcfm;

class PlanTest : public ::testing::Test {

public:
	Plan fp;
	Plan lpc;

protected:
	virtual void SetUp() {
		fp.clear();
		//NavPoint::PointMutability un(NavPoint::Mutable);
		fp.addNavPoint(NavPoint(Position(LatLonAlt::make(33.3440, -97.2440, 20000.0000)),   1.0000, ""));
		fp.addNavPoint(NavPoint(Position(LatLonAlt::make(33.0110, -97.2440, 20000.0000)), 239.6590, ""));
		fp.addNavPoint(NavPoint(Position(LatLonAlt::make(33.0110, -96.0060, 20000.0000)), 986.9550, ""));
		//lpc.clear();
	}
};

TEST_F(PlanTest, testSetup) {
	EXPECT_EQ(3, fp.size());
	EXPECT_TRUE( ! fp.hasError());
	EXPECT_TRUE(fp.isLatLon());

	Plan fp2;
	EXPECT_TRUE(fp2.isLatLon(true));
	EXPECT_TRUE(fp2.isLatLon(false));
	fp2.addNavPoint(NavPoint(Position(Vect3(0,0,0)), 0.0, ""));
	EXPECT_TRUE(fp2.isLatLon(false));
	EXPECT_FALSE(fp2.isLatLon(true));
}

TEST_F(PlanTest, testPositionTime) {
	LatLonAlt s;
	s = fp.position(1.0).lla();
	EXPECT_TRUE( ! fp.hasError());
	EXPECT_NEAR(33.3440, s.latitude(), 0.0001);
	EXPECT_NEAR(-97.2440, s.longitude(), 0.0001);
	EXPECT_NEAR(20000.0, s.altitude(), 0.0001);

	s = fp.position(239.6590).lla();
	EXPECT_TRUE( ! fp.hasError());
	EXPECT_NEAR(33.0110, s.latitude(), 0.0001);
	EXPECT_NEAR(-97.2440, s.longitude(), 0.0001);
	EXPECT_NEAR(20000.0, s.altitude(), 0.0001);
}

TEST_F(PlanTest, testVelocitySeg) {
	Vect3 v = fp.averageVelocity(0);
	EXPECT_TRUE( ! fp.hasError());
	EXPECT_NEAR(0, v.x, 0.0000001);
	//EXPECT_NEAR(2.85775160985e-5, v.x, 0.0000001);
	EXPECT_NEAR(-155.0, v.y, 0.5);
	EXPECT_NEAR(0.0, v.z, 0.0);
}

TEST_F(PlanTest, testPlanEquals) {
	Plan lpc("GREAT_PLAN");
	Position p0  = Position::makeXYZ(0.0, 0.0, 10000.0);
	Position p1  = Position::makeXYZ(2.0, 5.0, 10000.0);
	Position p2  = Position::makeXYZ(-2.0, 8.0, 10000.0);
	NavPoint np0(p0,   10.0);          lpc.addNavPoint(np0);
	NavPoint np1(p1, 20.0, "PT1");     lpc.addNavPoint(np1);
	NavPoint np2(p2,   30.0);          lpc.addNavPoint(np2);
	Plan cp = Plan(lpc);
	EXPECT_TRUE(cp == lpc);
	EXPECT_FALSE(lpc. isTrkContinuous(1, true) );
	EXPECT_FALSE(lpc. isGsContinuous(1, true) );
	EXPECT_TRUE(lpc. isVsContinuous(1, true) );

}

TEST_F(PlanTest, testVelocityTime) {
	Vect3 v;
	v = fp.velocity(0.0);
	EXPECT_TRUE(! fp.hasError());
	//  EXPECT_NEAR(0.0, v.x, 0.00000005);
	//  EXPECT_NEAR(0.0, v.y, 0.5);
	//  EXPECT_NEAR(0.0, v.z, 0.0);
	//std::cout << "TEST2" << std::endl;
	v = fp.velocity(1.01);
	EXPECT_TRUE(!fp.hasError());
	EXPECT_NEAR(1.89876e-14, v.x, 0.0000001);
	EXPECT_NEAR(-155.0, v.y, 0.5);
	EXPECT_NEAR(0.0, v.z, 0.0);

//	std::cout << "testVelocityTime TEST3 is expected to fail!" << std::endl;
	v = fp.velocity((239.6590 - 1.0) / 2.0 + 1.0);
	EXPECT_TRUE( !fp.hasError());
	EXPECT_NEAR(2.130004224e-5, v.x, 0.0001);  // this is the Java answer, C++ is different on OSX
	EXPECT_NEAR(-155.0, v.y, 0.5);
	EXPECT_NEAR(0.0, v.z, 0.0);
}

TEST_F(PlanTest, testClear) {
	fp.clear();
	EXPECT_EQ(0,fp.size());
	fp.addNavPoint(NavPoint(Position(LatLonAlt::make(35, -95, 30000.0000)), 1000,  ""));
	fp.addNavPoint(NavPoint(Position(LatLonAlt::make(45, -95, 30000.0000)), 1010,  ""));
	fp.addNavPoint(NavPoint(Position(LatLonAlt::make(55, -95, 30000.0000)), 1020,  ""));
	EXPECT_EQ(3,fp.size());
	EXPECT_TRUE(fp.isWellFormed());
	EXPECT_TRUE(fp.isConsistent());
	EXPECT_TRUE(fp. isTrkContinuous(1, true) );
	EXPECT_TRUE(fp. isGsContinuous(1, true) );
	EXPECT_TRUE(fp. isVsContinuous(1, true) );

}


TEST_F(PlanTest, testAdvance) {
	Plan lpc("EEE");
	Position p0(LatLonAlt::make(40.000, -100.000, 30000.0));
	Position p1(LatLonAlt::make(39.977, -96.377,  40000.0));
	NavPoint np0(p0,0.000000);    lpc.addNavPoint(np0);
	NavPoint np1(p1,1500);        lpc.addNavPoint(np1);
	double pathD = lpc.pathDistance();
	bool linear = false;
    Position posMid = lpc.advanceDistanceWithinSeg2D(0, pathD/2.0, linear );
	NavPoint npMid(posMid,500);        lpc.addNavPoint(npMid);
    EXPECT_NEAR(lpc.point(0).lat(),0.6981,0.001);
    EXPECT_NEAR(lpc.point(0).lon(),-1.7453,0.001);
    EXPECT_NEAR(lpc.point(0).alt(),9144.0000,0.001);
    EXPECT_NEAR(lpc.point(1).time(),500.0000,0.001);
    EXPECT_NEAR(lpc.point(1).lat(),0.6982,0.001);
    EXPECT_NEAR(lpc.point(1).lon(),-1.7137,0.001);
    EXPECT_NEAR(lpc.point(1).alt(),9144.0000,0.001);
    EXPECT_NEAR(lpc.point(2).time(),1500.0000,0.001);
    EXPECT_NEAR(lpc.point(2).lat(),0.6977,0.001);
    EXPECT_NEAR(lpc.point(2).lon(),-1.6821,0.001);
    Position above = posMid.mkAlt(p1.alt());
    NavPoint npAbove(above,700);        lpc.addNavPoint(npAbove);
    Position posMidAlt = lpc.advanceDistanceWithinSeg2D(1, 10, linear ).mkAlt((p1.alt()+p0.alt())/2.0);
    NavPoint npMidAlt(posMidAlt,600);        lpc.addNavPoint(npMidAlt);
//        DebugSupport.dumpPlan(lpc,"testAdvance_lpc");
    //DebugSupport.dumpAsUnitTest(lpc,true);
    EXPECT_EQ(5,lpc.size());
    EXPECT_NEAR(lpc.point(0).time(),0.0000,0.001);
    EXPECT_NEAR(lpc.point(0).lat(),0.6981,0.001);
    EXPECT_NEAR(lpc.point(0).lon(),-1.7453,0.001);
    EXPECT_NEAR(lpc.point(0).alt(),9144.0000,0.001);
    EXPECT_NEAR(lpc.point(1).time(),500.0000,0.001);
    EXPECT_NEAR(lpc.point(1).lat(),0.6982,0.001);
    EXPECT_NEAR(lpc.point(1).lon(),-1.7137,0.001);
    EXPECT_NEAR(lpc.point(1).alt(),9144.0000,0.001);
    EXPECT_NEAR(lpc.point(2).time(),600.0000,0.001);
    EXPECT_NEAR(lpc.point(2).lat(),0.6982,0.001);
    EXPECT_NEAR(lpc.point(2).lon(),-1.7137,0.001);
    EXPECT_NEAR(lpc.point(2).alt(),10668.0000,0.001);
    EXPECT_NEAR(lpc.point(3).time(),700.0000,0.001);
    EXPECT_NEAR(lpc.point(3).lat(),0.6982,0.001);
    EXPECT_NEAR(lpc.point(3).lon(),-1.7137,0.001);
    EXPECT_NEAR(lpc.point(3).alt(),12192.0000,0.001);
    EXPECT_NEAR(lpc.point(4).time(),1500.0000,0.001);
    EXPECT_NEAR(lpc.point(4).lat(),0.6977,0.001);
    EXPECT_NEAR(lpc.point(4).lon(),-1.6821,0.001);
    EXPECT_NEAR(lpc.point(4).alt(),12192.0000,0.001);
}


TEST_F(PlanTest, testAdvance2) {
	Plan lpc("noname");
	Position p0  = Position::makeLatLonAlt(34.04000000, -117.53000000, 12654.00000000);
	Position p1  = Position::makeLatLonAlt(34.16018267, -117.62432186, 12565.83333333);
	Position p2  = Position::makeLatLonAlt(34.35532533, -117.77816932, 12422.56222884);
	Position p3  = Position::makeLatLonAlt(34.76000000, -118.10000000, 12125.00000000);
	NavPoint np0(p0,400.000000);   	 lpc.addNavPoint(np0);
	NavPoint src1 =  NavPoint::makeLatLonAlt(34.16018267, -117.62432186, 12565.83333333, 500.000000).makeName("");
	std::pair<NavPoint,TcpData> np1 = Plan::makeBGS("", p1,500.000000, 1.111000);
	lpc.add(np1);
	NavPoint src2 =  NavPoint::makeLatLonAlt(34.35532533, -117.77816932, 12422.56222884, 615.761576).makeName("");
	std::pair<NavPoint,TcpData> np2 = Plan::makeEGSBGS("",p2,615.761576, 3.333000);  // should be EGS
	lpc.add(np2);
	NavPoint np3(p3,802.483407);    	 lpc.addNavPoint(np3);
	EXPECT_EQ(4,lpc.size());
	EXPECT_NEAR(lpc.point(0).time(),400.0000,0.001);
	//fpln(" $$ testAdvance2: lpc = "+lpc.toStringGs());
//		DebugSupport.dumpPlan(lpc,"testAdvance2_lpc");
	EXPECT_FALSE(lpc.isConsistent(false));
	EXPECT_NEAR(0.0,lpc.gsOut(2),1E-12);  // because segment 2 has negative calc gs
}

TEST_F(PlanTest, test_advanceDistanceWithinSeg2D) {
	Plan lpc("Adv2D");
	Position so  = Position::makeLatLonAlt(32.60704, -96.61418, 12000.0);
	double t0 = 0;
	double track = Units::from("deg",80);
	double R = Units::from("NM",10);
	Position center = so.linearDist2D(track,R);
	double turnDist = Units::from("NM",20);
	Position eotPos = KinematicsPosition::turnByDist2D(so, center, +1, turnDist);
    std::pair<NavPoint, TcpData> BOT = Plan::makeBOT(so, t0, R, center);
    lpc.add(BOT);
    double gs = Units::from("kn",300);
    double t1 = turnDist/gs;
    std::pair<NavPoint, TcpData> EOT = Plan::makeEOT(eotPos, t1);
    lpc.add(EOT);
    bool linear = false;
    Position posMot = lpc.advanceDistanceWithinSeg2D(0, turnDist/2.0, linear);
    NavPoint MOT(posMot,t1/2);
    int ix = lpc.addNavPoint(MOT);
    lpc.setMOT(ix);
    EXPECT_TRUE(lpc.isConsistent());
    double distMOTcenter = center.distanceH(posMot);
    EXPECT_NEAR(R,distMOTcenter,0.0001);

    Position p2pos = lpc.advanceDistanceWithinSeg2D(0, turnDist/4.0, linear);
    double distCenterP2 = center.distanceH(p2pos);
    NavPoint P2(p2pos,t1/4);
    lpc.addNavPoint(P2);
    EXPECT_NEAR(R,distCenterP2,0.0001);

    linear = true;
    Position p4pos = lpc.advanceDistanceWithinSeg2D(0, turnDist/10.0, linear);
    NavPoint P4(p4pos,t1+100);
    lpc.addNavPoint(P4);
    //EXPECT_NEAR(R,distCenterP2,0.0001);
    double d01 = lpc.point(0).distanceH(lpc.point(1));
    double checkDist = lpc.point(0).distanceH(lpc.point(4)) + lpc.point(4).distanceH(lpc.point(1));
    EXPECT_NEAR(d01,checkDist,0.0001);  // collinear check
    //DebugSupport.dumpPlan(lpc,"test_advanceDistanceWithinSeg2D_lpc");
    EXPECT_TRUE(lpc.isConsistent());
}




TEST_F(PlanTest, testVelocityNearEndPoint) {
	LatLonAlt KMSP = LatLonAlt::make(44.878101, -93.21869, 10000.0);
	LatLonAlt KDFW = LatLonAlt::make(32.898700, -97.04039, 10000.0);
	NavPoint np1(Position(KMSP), 0);
	NavPoint np2(Position(KDFW), 2000);
	Plan kpc("testVelocityNearEndPoint");
	kpc.addNavPoint(np1);
	kpc.addNavPoint(np2);
	Velocity v = kpc.velocity(np2.time()-0.0001);
	Velocity vf = kpc.finalVelocity(0);
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	v = kpc.velocity(np2.time()-0.00001);
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	v = kpc.velocity(np2.time()-0.000001);
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	v = kpc.velocity(np2.time()-1E-13);
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);


}

TEST_F(PlanTest, testFinalPointInitialVelocity) {
	Position p0  = Position(LatLonAlt::make(40.000000, -100.000000, 30000.000000));
	Position p1  = Position(LatLonAlt::make(39.977439, -96.377050, 30000.000000));
	NavPoint np0 = NavPoint(p0,0.000000);     lpc.addNavPoint(np0);
	double t1 = 1500;
	NavPoint np1 = NavPoint(p1,t1);     lpc.addNavPoint(np1);
	//fpln(" $$ lpc.initialVelocity(1) = "+lpc.initialVelocity(1));
	//fpln(" $$ lpc.finalVelocity(0) = "+lpc.finalVelocity(0));
	//  lpc.initialVelocity(1) = lpc.finalVelocity(0);
	Velocity v0 = lpc.initialVelocity(0);
	Velocity actual = Velocity::mkTrkGsVs(1.5992, 205.6238, 0.0);
	EXPECT_NEAR(actual.trk(), lpc.finalVelocity(0).trk(), 0.0001);
	EXPECT_NEAR(actual.gs(), lpc.finalVelocity(0).gs(), 0.0001);
	EXPECT_NEAR(actual.vs(), lpc.finalVelocity(0).vs(), 0.0001);
	EXPECT_NEAR(lpc.finalVelocity(0).trk(), lpc.initialVelocity(1).trk(), 0.0001);
	EXPECT_NEAR(lpc.finalVelocity(0).gs(), lpc.initialVelocity(1).gs(), 0.0001);
	EXPECT_NEAR(lpc.finalVelocity(0).vs(), lpc.initialVelocity(1).vs(), 0.0001);
	double t2 = t1 - 0.001;                           // this is related to Constants.HORIZONTAL_ACCURACY_RAD
	Position p2 = lpc.position(t2);
	NavPoint np2 = NavPoint(p2,t2);
	//fpln(" $$$$ np1 = "+np1.toString(15));
	//fpln(" $$$$ np2 = "+np2.toString(15));
	//fpln(" dist = "+Units::str("ft", p1.distanceH(p2))+" v="+p1.initialVelocity(p2, 0.001));
	lpc.addNavPoint(np2);
	//fpln(" $$ lpc.initialVelocity(2) = "+lpc.initialVelocity(2));
	//fpln(" $$ lpc.finalVelocity(1) = "+lpc.finalVelocity(1));
	EXPECT_NEAR(actual.trk(), lpc.initialVelocity(1).trk(), 0.0001);
	EXPECT_NEAR(actual.gs(), lpc.initialVelocity(1).gs(), 0.0001);
	EXPECT_NEAR(actual.vs(), lpc.initialVelocity(1).vs(), 0.0001);
	EXPECT_NEAR(actual.trk(), lpc.initialVelocity(2).trk(), 0.0001);
	EXPECT_NEAR(actual.gs(), lpc.initialVelocity(2).gs(), 0.0001);
	EXPECT_NEAR(actual.vs(), lpc.initialVelocity(2).vs(), 0.0001);
	EXPECT_NEAR(actual.trk(), lpc.finalVelocity(1).trk(), 0.0001);
	EXPECT_NEAR(actual.gs(), lpc.finalVelocity(1).gs(), 0.0001);
	EXPECT_NEAR(actual.vs(), lpc.finalVelocity(1).vs(), 0.0001);
	EXPECT_NEAR(v0.trk(), lpc.initialVelocity(0).trk(), 0.0001);
	EXPECT_NEAR(v0.gs(), lpc.initialVelocity(0).gs(), 0.0001);
	EXPECT_NEAR(v0.vs(), lpc.initialVelocity(0).vs(), 0.0001);

	Position pAfter = lpc.position(t1+1);
	EXPECT_TRUE(pAfter.isInvalid());
	bool inLoS = p0.LoS(pAfter,5000,5000);
	EXPECT_FALSE(inLoS);

}


TEST_F(PlanTest, testTimeShift) {
	//fpln("BEFORE  fp = "+fp);
	EXPECT_EQ(3,fp.size());
	fp.timeShiftPlan(0,10);
	//fpln("AFTER  fp = "+fp);
	EXPECT_EQ(3,fp.size());
	//fpln(" fp.position(11) = "+fp.position(11));
	EXPECT_NEAR(Units::from("deg", 33.3440),fp.position(11).lat(),0.0001);
	EXPECT_NEAR(Units::from("deg", -97.2440),fp.position(11).lon(),0.0001);
	EXPECT_NEAR(Units::from("ft", 20000),fp.position(11).alt(),0.0001);
	fp.clear();
	//NavPoint::PointMutability un(NavPoint::Mutable);
	fp.addNavPoint( NavPoint( Position(LatLonAlt::make(4.8781, -93.2187, 0.0000)),   0.0,  ""));
	fp.addNavPoint( NavPoint( Position(LatLonAlt::make(43.8238, -93.6149, 14830.6667)),   444.9200,  ""));
	fp.addNavPoint( NavPoint( Position(LatLonAlt::make(43.7997, -93.6238, 15000.0000)),   455.0800,  ""));
	fp.addNavPoint( NavPoint( Position(LatLonAlt::make(33.9472, -96.7539, 15000.0000)),   4590.5644,  ""));
	fp.addNavPoint( NavPoint( Position(LatLonAlt::make(33.9229, -96.7606, 14830.6667)),    4600.7244,  ""));
	fp.addNavPoint( NavPoint( Position(LatLonAlt::make(32.8987, -97.0404, 560.0000)),   5028.8444,  ""));
	EXPECT_EQ(6,fp.size());
	//fpln("BEFORE  fp = "+fp);
	fp.timeShiftPlan(0,0);
	//fpln("AFTER  fp = "+fp);
	EXPECT_EQ(6,fp.size());
	EXPECT_NEAR(Units::from("deg", 33.9472),fp.position(4590.5644).lat(),0.0001);
	EXPECT_NEAR(Units::from("deg", -96.7539),fp.position(4590.5644).lon(),0.0001);
	EXPECT_NEAR(Units::from("ft", 15000.0),fp.position(4590.5644).alt(),0.0001);
	Plan fp_cp = fp;
	fp_cp.timeShiftPlan(0, 0);
	EXPECT_TRUE(fp_cp == fp);
	fp.timeShiftPlan(2,-15);
	EXPECT_NEAR(Units::from("deg", 33.9229),fp.point(3).lat(),0.0001);
	EXPECT_NEAR(Units::from("deg", -93.2187),fp.point(0).lon(),0.0001);
	EXPECT_NEAR(Units::from("ft", 560),fp.point(4).alt(),0.0001);
	//fpln("AFTER AFTER fp = "+fp);
}

TEST_F(PlanTest, testPlanFromState) {
	Position so = Position::makeXYZ(-1.9000, 3.1000, 100.0000);
	Velocity vo = Velocity::makeTrkGsVs(120.0000,   500.0000,  450.0000);
	Plan np = Plan::planFromState("agh",so,vo,10.0,60.0);
	//fpln(" np = "+np);
	EXPECT_EQ("agh",np.getID());
	EXPECT_NEAR(10.0,np.getFirstTime(),0.0001);
	EXPECT_NEAR(60.0,np.getLastTime(),0.0001);
	NavPoint p0 = np.point(0);
	NavPoint p1 = np.point(1);
	EXPECT_NEAR(Units::from("nm",-1.9),p0.x(),0.0001);
	EXPECT_NEAR(Units::from("nm",3.1),p0.y(),0.0001);
	EXPECT_NEAR(Units::from("ft",100),p0.z(),0.0001);
	EXPECT_NEAR(10,p0.time(),0.0001);
	EXPECT_NEAR(Units::from("nm",4.114065304),p1.x(),0.0001);
	EXPECT_NEAR(Units::from("nm",-0.372222222),p1.y(),0.0001);
	EXPECT_NEAR(Units::from("ft",475.0),p1.z(),0.001);
	EXPECT_NEAR(60,p1.time(),0.0001);
}

//TEST_F(PlanTest, testCopyTimeShift) {
//	Plan lpc;
//	Position p1(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",20000)));
//	Position p2(Vect3(Units::from("nmi",-0.5), Units::from("nmi",1.5), Units::from("ft",20000)));
//	Position p3(Vect3(Units::from("nmi",0.0), Units::from("nmi",3.0), Units::from("ft",20000)));
//	Position p4(Vect3(Units::from("nmi",0.8), Units::from("nmi",5.356), Units::from("ft",19008.890)));
//	Position p5(Vect3(Units::from("nmi",0.267), Units::from("nmi",3.787), Units::from("ft",19692.590)));
//	NavPoint np1(p1,0.0);
//	NavPoint np2(p2,19.4815);
//	NavPoint np3(p3,38.9630);
//	NavPoint np4(p4,72.00);
//	NavPoint np5(p5,49.21);
//	lpc.addNavPoint(np1);
//	lpc.addNavPoint(np2);
//	lpc.addNavPoint(np3);
//	lpc.addNavPoint(np4);
//	lpc.addNavPoint(np5);
//	double tmShift = 23.5;
//	Plan npc = lpc.copyAndTimeShift(0,tmShift);
//	for (int i = 0; i < lpc.size(); i++) {                // Unchanged
//		EXPECT_TRUE(lpc.point(i).position() == npc.point(i).position());
//		//fpln(i+" $$$$ "+lpc.point(i).time()+tmShift+" "+npc.point(i).time());
//		EXPECT_NEAR(lpc.point(i).time()+tmShift, npc.point(i).time(),0.001);
//	}
//	//fpln(" $$ lpc = "+lpc.toString());
//	Plan ipc = lpc.copyWithIndex();
//	EXPECT_TRUE(ipc.almostEquals(lpc));
//	//fpln(" $$ ipc = "+ipc.toString());
//	for (int i = 0; i < ipc.size(); i++) {
//		EXPECT_EQ(i,ipc.getTcpData(i).getLinearIndex());
//	}
//
//}

TEST_F(PlanTest, testMethods) {
	Plan lpc;
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
	//		lpc.setProtectionDistance(Units::from("nm",5));
	//		lpc.setProtectionHeight(Units::from("ft",940));
	//		EXPECT_NEAR(Units::from("nm",5),lpc.getProtectionDistance(),0.00001);
	//		EXPECT_NEAR(Units::from("ft",940),lpc.getProtectionHeight(),0.00001);
	//		for (int k = 0; k < lpc.size(); k++) {
	//			EXPECT_NEAR(0.0,lpc.getSegmentDistanceBuffer(k),0.00001);
	//			EXPECT_NEAR(0.0,lpc.getSegmentHeightBuffer(k),0.000001);
	//		}
	EXPECT_TRUE(lpc.isLatLon());
	EXPECT_EQ("",lpc.getID());
	lpc.setID("HotDog");
	EXPECT_EQ("HotDog",lpc.getID());
	EXPECT_NEAR(10996, lpc.getFirstTime() ,0.00001);
	EXPECT_NEAR(13762.633136, lpc.getLastTime() ,0.00001);
	EXPECT_NEAR(10996, lpc.getFirstRealTime() ,0.00001);
	EXPECT_NEAR(1.0, lpc.nextPtOrEnd(0) ,0.00001);
	EXPECT_NEAR(5.0, lpc.nextPtOrEnd(4) ,0.00001);
	EXPECT_NEAR(-1.0, lpc.getIndex(5000) ,0.00001);
	EXPECT_NEAR(-2.0, lpc.getIndex(11000) ,0.00001);
	EXPECT_NEAR(-7.0, lpc.getIndex(17680.872991) ,0.00001);
	EXPECT_NEAR(-1, lpc.getSegment(15467) ,0.00001);
	EXPECT_NEAR(0, lpc.getSegment(10996) ,0.00001);
	EXPECT_NEAR(0.0, lpc.getNearestIndex(8000) ,0.00001);
	EXPECT_NEAR(13754.539398, lpc.time(4) ,0.00001);
	EXPECT_NEAR(0.57438, lpc.point(1).lat() ,0.0001);
	EXPECT_NEAR(-1.68677, lpc.point(1).lon() ,0.0001);
	EXPECT_NEAR(3256.5428, lpc.point(1).alt() ,0.0001);
	EXPECT_NEAR(11128.4204, lpc.point(1).time() ,0.0001);
	//		EXPECT_TRUE(lpc.overlaps(np5) > 0);
	NavPoint np6(p5,200000);
	EXPECT_EQ(6,lpc.size());
	lpc.remove(3);
	EXPECT_EQ(5,lpc.size());
	lpc.addNavPoint(NavPoint(p3,11400));
	lpc.remove(3,5);
	EXPECT_EQ(3,lpc.size());
	lpc.addNavPoint(NavPoint(p3,11400));
	EXPECT_EQ(4,lpc.size());
	EXPECT_EQ(3,lpc.setNavPoint(3,NavPoint(p3,11600)));
	EXPECT_NEAR(11600, lpc.time(3) ,0.00001);
	lpc.setTime(1,10001);            // note that 10001 is less than the current first point
	EXPECT_FALSE(lpc.hasError());
	EXPECT_NEAR(10001, lpc.time(0) ,0.00001);
	lpc.setAlt(2,12000);
	EXPECT_NEAR(12000, lpc.point(2).position().alt(),0.001);
	Position p7(LatLonAlt::make(32.7, -96.7, 3000));
	NavPoint np7(p7,6564);
	Velocity vin = lpc.velocity(11000.0);
	//fpln(" %%%%%%%% vin = "+vin.toString());

	double omega = 0.1;
	double radius = vin.gs()/omega;
	Position turnCenter = KinematicsPosition::centerFromRadius(p7, radius, vin.trk());
	std::pair<NavPoint,TcpData> np8 = Plan::makeBOT( p7, 11100.0, radius, turnCenter);
	lpc.add(np8);
	//lpc.setPlanType(Plan::KINEMATIC);
	//fpln(" $$ lpc = "+lpc.toString()+" np8 = "+np8.toStringFull());
	EXPECT_TRUE(lpc.isTCP(2));
	EXPECT_EQ(-1,lpc.prevBOT(1));
	EXPECT_EQ(2,lpc.prevBOT(4));
	EXPECT_EQ(-1,lpc.nextEGS(1));
	EXPECT_EQ(-1,lpc.nextEVS(1));
	EXPECT_NEAR(887.044,lpc.turnRadiusAtTime(11101),0.01);
	EXPECT_NEAR(0.0,lpc.gsAccelAtTime(11101),0.001);
	EXPECT_NEAR(0.0,lpc.vsAccelAtTime(11101),0.001);
	Position p9(LatLonAlt::make(32.9, -95.7, 14000));
	NavPoint np9(p9,6564);
	Velocity vin2 = lpc.velocity(11000.0);
	std::pair<NavPoint,TcpData> np10 = Plan::makeBGS("", p7, 11300.0, 2.1);
	lpc.add(np10);
	//f.pln(" .............. lpc = "+lpc);
	EXPECT_TRUE(lpc.isTCP(2));
	EXPECT_EQ(-1,lpc.prevBGS(1));
	EXPECT_EQ(4,lpc.prevBGS(4+1));
	EXPECT_EQ(-1,lpc.nextEGS(1));
	EXPECT_EQ(-1,lpc.prevBGS(1));
	EXPECT_NEAR(887.044,lpc.turnRadiusAtTime(11301),0.001);
	EXPECT_NEAR(2.1,lpc.gsAccelAtTime(11301),0.001);
	EXPECT_NEAR(0.0,lpc.vsAccelAtTime(11301),0.001);

	EXPECT_NEAR(0.57071, lpc.position(11345).lat() ,0.001);
	EXPECT_NEAR(-1.687835, lpc.position(11345).lon() ,0.001);
	EXPECT_NEAR(1581.19508, lpc.position(11345).alt() ,0.001);
	EXPECT_FALSE(lpc.isWellFormed());
	// --------- the plan is not well formed ------------
	EXPECT_NEAR(0.6465, lpc.averageVelocity(2).trk() ,0.0001);
	EXPECT_NEAR(175.05125, lpc.averageVelocity(2).gs() ,0.0001);
	EXPECT_NEAR(69.34553, lpc.averageVelocity(2).vs() ,0.0001);
	//         EXPECT_NEAR(-1.0788, lpc.initialVelocity(3).trk() ,0.0001);
	//         EXPECT_NEAR(9.254, lpc.initialVelocity(3).gs() ,0.001);
	//         EXPECT_NEAR(-276.17564, lpc.initialVelocity(3).vs() ,0.0001);
	//         EXPECT_NEAR(-1.71745, lpc.calcVertAccel(2) ,0.00001);
	//EXPECT_NEAR(-1.768045, lpc.calcVertAccel(3) ,0.00001);
	//         EXPECT_NEAR(0.96997, lpc.calcVertAccel(4) ,0.00001);
	//         EXPECT_NEAR(-1.6257, lpc.calcGsAccel(2) ,0.001);
	//        EXPECT_NEAR(4.49556, lpc.calcGsAccel(3) ,0.00001);
	//         EXPECT_NEAR(2.96625, lpc.calcGsAccel(4) ,0.00001);
	EXPECT_NEAR(10079.98, lpc.calcTimeGsIn(1,150) , 0.01);
	EXPECT_NEAR(10048.39, lpc.calcTimeGsIn(1,250)  , 0.01);
	EXPECT_NEAR(11075.98, lpc.calcTimeGsIn(2,350) , 0.01);
	EXPECT_NEAR(10079.98, lpc.calcTimeGsIn(1,150) , 0.01);
	EXPECT_NEAR(10048.39, lpc.calcTimeGsIn(1,250)  , 0.01);
	EXPECT_NEAR(11075.98, lpc.calcTimeGsIn(2,350) , 0.01);
	EXPECT_FALSE(lpc.isWellFormed());
	lpc.remove(4);
	lpc.remove(2);
	EXPECT_TRUE(lpc.isWellFormed());
	EXPECT_TRUE(lpc.isConsistent());
	//EXPECT_TRUE(lpc.isVelocityContinuous());
	EXPECT_NEAR(41955.47, lpc.pathDistance() , 0.01);
	EXPECT_NEAR(23405.59, lpc.pathDistance(1) , 0.01);
	EXPECT_NEAR(6702.20, lpc.pathDistance(2) , 0.01);
	EXPECT_NEAR(0, lpc.partialPathDistance(3,false) , 0.01);
	EXPECT_NEAR(41955.47, lpc.pathDistance(0,3) , 0.01);
	EXPECT_NEAR(-6640.29, lpc.vertDistance(2) , 0.01);
	EXPECT_NEAR(4276.48, lpc.vertDistance(1,3) , 0.01);
	//EXPECT_EQ("KINEMATIC",lpc.planTypeName());
	lpc.mkGsIn(3, 1000);
	EXPECT_NEAR(11266.56, lpc.point(3).time() , 0.01);
	//EXPECT_EQ(lpc,lpc.removeVerticalTCPs());
	for (double t =  lpc.getFirstTime(); t <=  lpc.getLastTime(); t = t + 99.3) {
		EXPECT_FALSE(lpc.inTrkChange(t));
		EXPECT_FALSE(lpc.inVsChange(t));
		EXPECT_FALSE(lpc.inGsChange(t));
	}
	EXPECT_FALSE(lpc. isTrkContinuous(1, true) );
	EXPECT_FALSE(lpc. isGsContinuous(1, true) );
	EXPECT_FALSE(lpc. isVsContinuous(1, true) );

}


TEST_F(PlanTest, test_mkGsIn) {
	Plan lpc("");
	Position p0(LatLonAlt::make(8.25, 106.75, 15000.000000));
	Position p1(LatLonAlt::make(8.61, 108.31, 16000.000000));
	Position p2(LatLonAlt::make(7.98, 108.99, 16000.000000));
	Position p3(LatLonAlt::make(8.52, 110.16, 15000.000000));
	NavPoint np0(p0,0.0);    np0 = np0.makeName("*P0*");         lpc.addNavPoint(np0);
	NavPoint np1(p1,1000.0);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1500.0); np2 = np2.makeName("*P2*");    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,2000.0);    	 lpc.addNavPoint(np3);
    //fpln(" $$ test_mkGsIn: lpc = "+lpc.toStringGs());
    Velocity v0 = lpc.initialVelocity(0);
    //Velocity v1 = lpc.initialVelocity(1);
    Velocity v2 = lpc.initialVelocity(2);
    double newGs = Units::from("kn",499);
	lpc.mkGsIn(2, newGs);
	EXPECT_TRUE(v0.almostEquals(lpc.initialVelocity(0)));
	EXPECT_NEAR(v0.gs(),lpc.initialVelocity(0).gs(),0.0001);
	EXPECT_NEAR(newGs,lpc.initialVelocity(1).gs(),0.0001);
	EXPECT_NEAR(v2.gs(),lpc.initialVelocity(2).gs(),0.0001);
	lpc.mkGsIn(3, newGs);
	//fpln("$$ test_mkGsIn: lpc = "+lpc.toStringGs());
	EXPECT_NEAR(newGs,lpc.initialVelocity(2).gs(),0.0001);
	lpc.mkGsIn(1, newGs);
	EXPECT_NEAR(newGs,lpc.initialVelocity(0).gs(),0.0001);
	//fpln(" lpc = "+lpc.toStringGs());
	double gsAccel = 3.0;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 25);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, false, false);
//	if (kpc.hasError()) {
//		EXPECT_TRUE(" !! getMessage(1) = "+kpc.getMessageNoClear());
//	}
	//fpln(" BEFORE kpc = "+kpc.toStringGs());
	newGs = Units::from("kn",177);
	kpc.mkGsIn(2, newGs);
	//fpln(" AFTER kpc = "+kpc.toStringGs());
	//EXPECT_NEAR(newGs,kpc.initialVelocity(1).gs(),0.0001);  //TODO
}


TEST_F(PlanTest, test_mkGsIn2) {
	Position p0  = Position::makeXYZ(100,200,10000);
	Position p1  = p0.linearDist2D(M_PI/6,1000);
	Position p2  = p1.linearDist2D(M_PI/6,5000);
	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,10);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,65);    	 lpc.addNavPoint(np2);
	double maxGsAccel = 0.2;
	//bool repairGs = false;
	//bool useOffset = false;
	//fpln(" $$$ test_mkGsIn2: lpc = "+lpc.toStringGs());
	Plan kpc = TrajGen::generateGsTCPs(lpc, maxGsAccel,true);
	EXPECT_TRUE(kpc.isConsistent());
	EXPECT_TRUE(kpc.isVelocityContinuous());
	Position pos50 = kpc.position(50);
	NavPoint np50(pos50,50);    	 kpc.addNavPoint(np50);
	double newGs = Units::from("kn",150);
	//fpln(" $$$ test_mkGsIn2: kpc = "+kpc.toStringGs());
	kpc.mkGsOut(2,newGs);
    EXPECT_NEAR(newGs,kpc.gsOut(2),0.0001);
	//fpln(" $$$ test_mkGsIn2: kpc = "+kpc.toStringGs());
    kpc.mkGsIn(2,newGs);
	//fpln(" $$$ test_mkGsIn2: AFTER kpc = "+kpc.toStringGs());
    EXPECT_NEAR(newGs,kpc.gsIn(2),0.0001);
    EXPECT_NEAR(Units::from("kn",168.2374),kpc.gsOut(1),0.0001);  // NOTE THAT IT Changes gsOut(2) !!
	//kpc.mkGsOut(3,newGs);
    kpc.mkGsIn(3,newGs);
	//fpln(" $$$ test_mkGsIn2: AFTER kpc = "+kpc.toStringGs());
}


TEST_F(PlanTest, test_calcDtGsin) {
	Plan lpc("GS");
	Position p0  = Position::makeXYZ(100,200,10000);
	Position p1  = p0.linearDist2D(M_PI/6,1000);
	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,10.1);    	 lpc.addNavPoint(np1);
	double gs200 = Units::from("kn",200);
	double t1 = lpc.calcDtGsIn(1, gs200);
	//f.pln(" $$$ t1 = "+t1+" lpc = "+lpc.toStringGs());
	lpc.mkGsIn(1,gs200);
	EXPECT_NEAR(t1,lpc.time(1),0.00000001);
	//double a_gs = 21.18; {
	for (double a_gs = 0.1; a_gs < 100.0; a_gs++) {
		lpc.clear();
		std::pair<NavPoint, TcpData> bgs = Plan::makeBGS("",p0,0.0,a_gs);
		std::pair<NavPoint, TcpData> egs = Plan::makeEGS(p1,t1);
		lpc.add(bgs);
		lpc.add(egs);
		//f.pln(" $$$$ a_gs = "+a_gs);
		if (a_gs < 21.18) {
			EXPECT_TRUE(lpc.isConsistent(false));
		}
	}
	double a_gs = -5.0;
	lpc.clear();
	std::pair<NavPoint, TcpData> bgs = Plan::makeBGS("",p0,0.0,a_gs);
	std::pair<NavPoint, TcpData> egs = Plan::makeEGS(p1,t1);
	lpc.add(bgs);
	lpc.add(egs);
	//f.pln(" $$$ AFTER lpc = "+lpc.toStringGs());
	for (double gsIn = 0.1; gsIn < 1000.0; gsIn++) {
		double t_1 = lpc.calcTimeGsIn(1, gsIn);
        bool ok = lpc.mkGsIn(1,gsIn);
		//f.pln(" $$$ IN gsIn Loop:  gsIn= "+gsIn);
        EXPECT_TRUE(lpc.isConsistent(false));
        EXPECT_FALSE(lpc.hasError());
		EXPECT_TRUE(ok);
        EXPECT_NEAR(gsIn,lpc.gsIn(1),0.0001);
        EXPECT_NEAR(t_1,lpc.time(1),0.00001);
	}

	a_gs = 6.5;
	//f.pln(" $$$ AFTER lpc = "+lpc.toStringGs());
	for (double gsIn = 1; gsIn < 1000.0; gsIn++) {
		double t_1 = lpc.calcTimeGsIn(1, gsIn);
        bool ok = lpc.mkGsIn(1,gsIn);
        EXPECT_TRUE(lpc.isConsistent(false));
        EXPECT_FALSE(lpc.hasError());
		EXPECT_TRUE(ok);
        EXPECT_NEAR(gsIn,lpc.gsIn(1),0.0001);
        EXPECT_NEAR(t_1,lpc.time(1),0.00001);
	}

}

TEST_F(PlanTest, test_mkGsConstant) {
	Position p0  = Position::makeXYZ(100,200,10000);
	Position p1  = p0.linearDist2D(M_PI/6,1000);
	Position p2  = p1.linearDist2D(M_PI/6,5000);
	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,10);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,65);    	 lpc.addNavPoint(np2);
	double maxGsAccel = 0.2;
	//bool repairGs = false;
	//bool useOffset = false;
	//fpln(" $$$ test_mkGsIn3: lpc = "+lpc.toStringGs());
	Plan kpc = TrajGen::generateGsTCPs(lpc, maxGsAccel,true);
	EXPECT_TRUE(kpc.isConsistent());
	EXPECT_TRUE(kpc.isVelocityContinuous());
	Position pos32 = kpc.position(32);
	NavPoint np32(pos32,32);    	 kpc.addNavPoint(np32);
	Position pos40 = kpc.position(40);
	NavPoint np40(pos40,40);    	 kpc.addNavPoint(np40);
	Position pos50 = kpc.position(50);
	NavPoint np50(pos50,50);    	 kpc.addNavPoint(np50);
	Plan orig(kpc);
	double newGs = Units::from("kn",150);
	//int wp1 = 1; {
    //int wp2 = 2; {
	for (int wp1 = 0; wp1 <= 6; wp1++) {
		for (int wp2 = 0; wp2 <= 6; wp2++) {
			kpc = Plan(orig);
			//fpln(" $$$ test_mkGsIn3: \n ---------------------------- wp1 = "+wp1+" wp2 = "+wp2);
			//fpln(" $$$ test_mkGsIn3: BEFORE orig = "+orig.toStringGs());
			kpc.mkGsConstant(wp1,wp2,newGs);
			//fpln(" $$$ test_mkGsIn3: AFTER kpc = "+kpc.toStringGs());
			EXPECT_TRUE(kpc.isConsistent());
			for (int j = wp1; j < wp2; j++) {
				EXPECT_NEAR(newGs,kpc.gsOut(j),0.0001);
			}
			for (int j = 0; j < wp1; j++) {
				//fpln(" LOOP2 j = "+j+" OUT: "+orig.gsOut(j)+" "+kpc.gsOut(j));
				EXPECT_NEAR(orig.gsOut(j),kpc.gsOut(j),0.0001);
				if (j>0) EXPECT_NEAR(orig.gsIn(j), kpc.gsIn(j), 0.0001);

			}
			for (int j = wp2; j < orig.size(); j++) {
				//fpln(" LOOP3 j = "+j+" OUT: "+orig.gsOut(j)+" "+kpc.gsOut(j));
				if (j < orig.size()-1) EXPECT_NEAR(orig.gsOut(j),kpc.gsOut(j),0.0001);
				//fpln(" LOOP3 j = "+j+" IN: "+orig.gsIn(j)+" "+kpc.gsIn(j));
				if (j>wp2) EXPECT_NEAR(orig.gsIn(j),kpc.gsIn(j),0.0001);
			}
		}
	}
}



TEST_F(PlanTest, test_mkGsConstant2) {
	Position p0(LatLonAlt::make(34.320, -117.631, 11000));
	Position p1  = p0.linearDist2D(M_PI/6,1000);
	Position p2  = p1.linearDist2D(M_PI/6,5000);
	Position p3  = p1.linearDist2D(M_PI/6,7000);

	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,15);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,70);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,91);    	 lpc.addNavPoint(np3);

	double maxGsAccel = 0.5;
	//bool repairGs = false;
	//bool useOffset = false;
	lpc.setGsAccel(1,0.6);
	lpc.setGsAccel(2,0.777);
	//fpln(" $$$ test_mkGsIn3: lpc = "+lpc.toStringGs());
	Plan kpc = TrajGen::generateGsTCPs(lpc, maxGsAccel,true);
	EXPECT_TRUE(kpc.isConsistent());
	EXPECT_TRUE(kpc.isVelocityContinuous());
	Position pos32 = kpc.position(32);
	NavPoint np32(pos32,32);    	 kpc.addNavPoint(np32);
	Plan orig(kpc);
	double newGs = Units::from("kn",150);
	//int wp1 = 1; {
    //int wp2 = 4; {
	for (int wp1 = 0; wp1 <= 4; wp1++) {
		for (int wp2 = 0; wp2 <= 4; wp2++) {
			kpc = Plan(orig);
			//fpln(" $$$ test_mkGsIn3: \n ---------------------------- wp1 = "+wp1+" wp2 = "+wp2);
			//fpln(" $$$ test_mkGsIn3: BEFORE orig = "+orig.toStringGs());
			kpc.mkGsConstant(wp1,wp2,newGs);
			//fpln(" $$$ test_mkGsIn3: AFTER kpc = "+kpc.toStringGs());
			EXPECT_TRUE(kpc.isConsistent());
			for (int j = wp1; j < wp2; j++) {
				EXPECT_NEAR(newGs,kpc.gsOut(j),0.0001);
			}
			for (int j = 0; j < wp1; j++) {
				//fpln(" LOOP2 j = "+j+" OUT: "+orig.gsOut(j)+" "+kpc.gsOut(j));
				EXPECT_NEAR(orig.gsOut(j),kpc.gsOut(j),0.0001);
				if (j>0) EXPECT_NEAR(orig.gsIn(j),kpc.gsIn(j),0.0001);

			}
			for (int j = wp2; j < orig.size(); j++) {
				//fpln(" LOOP3 j = "+j+" OUT: "+orig.gsOut(j)+" "+kpc.gsOut(j));
				if (j < orig.size()-1) EXPECT_NEAR(orig.gsOut(j),kpc.gsOut(j),0.0001);
				//fpln(" LOOP3 j = "+j+" IN: "+orig.gsIn(j)+" "+kpc.gsIn(j));
				if (j>wp2) EXPECT_NEAR(orig.gsIn(j),kpc.gsIn(j),0.0001);
			}
		}
	}
}


TEST_F(PlanTest, testPathDistance) {
	Plan lpc;
	Position p0(LatLonAlt::make(0.0, -10.0, 0.0));
	Position p1(LatLonAlt::make(0.0, -5.0, 0.0));
	Position p2(LatLonAlt::make(5.0, 0.0, 0.0));
	Position p3(LatLonAlt::make(10.0, 0.0, 0.0));
	Velocity v1 = Velocity::makeTrkGsVs(Units::from("deg", 90.0), p0.distanceH(p1) / 100.0 ,0.0);
	Velocity v2 = Velocity::makeTrkGsVs(Units::from("deg",  0.0), p2.distanceH(p3) / 100.0 ,0.0);
	NavPoint np0(p0,100.00);                       lpc.addNavPoint(np0);
	double signedRadius = p0.distanceH(p1);
	Position center = KinematicsPosition::centerFromRadius(p1, signedRadius, v1.trk());

	std::pair<NavPoint,TcpData> np1 = Plan::makeBOT( p1, 200.00, signedRadius, center); lpc.add(np1);
	std::pair<NavPoint,TcpData> np2 = Plan::makeEOT(p2, 300.00);                   lpc.add(np2);
	NavPoint np3(p3,400.00);                       lpc.addNavPoint(np3);
	EXPECT_TRUE(lpc.isLatLon());
	EXPECT_EQ("",lpc.getID());
	lpc.setID("HotDog");
	EXPECT_EQ("HotDog",lpc.getID());
	EXPECT_NEAR(100.00, lpc.getFirstTime() ,0.00001);
	EXPECT_NEAR(400.00, lpc.getLastTime() ,0.00001);
	EXPECT_NEAR(100.00, lpc.getFirstRealTime() ,0.00001);
	EXPECT_NEAR(lpc.pathDistance(0,false),lpc.pathDistance(0,true),0.0); // linear segment
	EXPECT_NEAR(lpc.pathDistance(2,false),lpc.pathDistance(2,true),0.0); // linear segment
	EXPECT_NEAR(lpc.pathDistance(0,false),lpc.pathDistance(2,false),0.0); // just an consequence of how the segments were defined
	EXPECT_NEAR(p0.distanceH(p1),lpc.pathDistance(0,false),0.0);
	// Test curves
	EXPECT_NEAR(M_PI / 2 * p0.distanceH(p1), lpc.pathDistance(1,false), 20000.0); // rough estimate, quarter turn of the radius
	EXPECT_NEAR(857761.97606,lpc.pathDistance(1,false),0.00001);
	// Test full distance
	EXPECT_NEAR(p0.distanceH(p1) + p2.distanceH(p3) + 857761.97606, lpc.pathDistance(), 0.00001);
}





//	TEST_F(PlanTest, testCutDownTo) {
//		fpln(" ------------------------------ testCutDownTo ---------------------------------");
//		Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
//		Position p2 = Position::makeXYZ(10.0, 5.0, 10000);
//		Position p3 = Position::makeXYZ(20.0, 15.0, 10000);
//        Velocity v4 = Velocity::makeTrkGsVs(45,450,500);
//        Position p4 = p3.linear(v4,200);
//		NavPoint np1(p1,0.0);
//		NavPoint np2(p2,201.25);
//		NavPoint np3(p3,455.8);
//		NavPoint np4(p4,905.8);
//		Plan lpc("");
//		lpc.addNavPoint(np1);
//		lpc.addNavPoint(np2);
//		lpc.addNavPoint(np3);
//		lpc.addNavPoint(np4);
//		//lpc = lpc.makeSpeedConstant(Units::from("kn",180.0));
//		double gsAccel = 2;
//		double vsAccel = 1;
//		double bankAngle = Units::from("deg", 20);
//		Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true);
//        //DebugSupport.dumpPlan(kpc, "testCutDownTo_kpc");
//	    //fpln("testCutDownTo: kpc = "+kpc);
//	    EXPECT_TRUE(kpc.isConsistent());
//	    EXPECT_EQ(7,kpc.size());
//	    EXPECT_FALSE(kpc.point(0).isTCP());
//	    EXPECT_TRUE(kpc.point(1).isBOT());
//	    EXPECT_FALSE(kpc.point(2).isTCP());
//	    EXPECT_TRUE(kpc.point(3).isEOT());
//	    EXPECT_TRUE(kpc.point(4).isBVS());
//	    EXPECT_TRUE(kpc.point(5).isEVS());
//	    EXPECT_FALSE(kpc.point(6).isTCP());
//	    //fpln("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
//	    Plan kpc2 = PlanUtil::cutDownTo(kpc,0,200);
//	    //fpln(" $$ testCutDownTo: kpc2 = "+kpc2);
//	    //DebugSupport.dumpPlan(kpc2, "testCutDownTo_kpc2");
//	    EXPECT_TRUE(kpc2.isConsistent());
//	    EXPECT_EQ(3,kpc2.size());
//	    EXPECT_FALSE(kpc2.point(0).isTCP());
//	    EXPECT_TRUE(kpc2.point(1).isBOT());
//	    EXPECT_TRUE(kpc2.point(2).isTCP());
//	    EXPECT_TRUE(kpc2.point(2).isEOT());
//	    kpc2 = PlanUtil::cutDownTo(kpc,0,500);
//	    EXPECT_TRUE(kpc2.isConsistent());
//	    EXPECT_EQ(7,kpc2.size());
//	    EXPECT_FALSE(kpc2.point(0).isTCP());
//	    EXPECT_TRUE(kpc2.point(1).isBOT());
//	    EXPECT_TRUE(kpc2.point(3).isEOT());
//	    EXPECT_TRUE(kpc2.point(4).isBVS());
//	    EXPECT_TRUE(kpc2.point(5).isEVS());
//	    kpc2 = PlanUtil::cutDownTo(kpc,0,456);
//	    EXPECT_TRUE(kpc2.isConsistent());
//	    EXPECT_EQ(6,kpc2.size());
//	    EXPECT_FALSE(kpc2.point(0).isTCP());
//	    EXPECT_TRUE(kpc2.point(1).isBOT());
//	    EXPECT_TRUE(kpc2.point(3).isEOT());
//	    EXPECT_TRUE(kpc2.point(4).isBVS());
//	    EXPECT_TRUE(kpc2.point(5).isEVS());
//	    kpc2 = PlanUtil::cutDownTo(kpc,300,456);
//	    EXPECT_TRUE(kpc2.isConsistent());
//	    EXPECT_EQ(3,kpc2.size());
//	    EXPECT_FALSE(kpc2.point(0).isTCP());
//	    EXPECT_TRUE(kpc2.point(1).isBVS());
//	    EXPECT_TRUE(kpc2.point(2).isEVS());
//	    kpc2 = PlanUtil::cutDownTo(kpc,200,250);                        // in the middle of a turn so go back
//	    EXPECT_TRUE(kpc2.isConsistent());
//	    EXPECT_EQ(4,kpc2.size());
//	    EXPECT_TRUE(kpc2.point(0).isTCP());
//	    EXPECT_TRUE(kpc2.point(0).isBOT());
//	    EXPECT_TRUE(kpc2.point(2).isEOT());
//	    EXPECT_FALSE(kpc2.point(3).isTCP());
//	    kpc2 = PlanUtil::cutDownTo(kpc,100,1000);
//	    EXPECT_TRUE(kpc2.isConsistent());
//	    EXPECT_EQ(8,kpc2.size());
//	    EXPECT_FALSE(kpc2.point(0).isTCP());
//	    EXPECT_TRUE(kpc2.point(1).isBOT());
//	    EXPECT_FALSE(kpc2.point(2).isTCP());
//	    EXPECT_TRUE(kpc2.point(3).isEOT());
//	    EXPECT_TRUE(kpc2.point(4).isBVS());
//	    EXPECT_TRUE(kpc2.point(5).isEVS());
//	    EXPECT_FALSE(kpc2.point(6).isTCP());
//	    EXPECT_FALSE(kpc2.point(7).isTCP());
//	}


TEST_F(PlanTest, testInitialVelocityLastPointIsEOT) {
	Position p0  = Position::makeXYZ(0.000000, 0.000000, 10000.000000);
	Position p1  = Position::makeXYZ(9.767568, 4.883784, 10000.000000);
	double t1 = 196.572301;
	double t2 = 200.0;
	NavPoint np0(p0,0.000000);
	lpc.addNavPoint(np0);
	Velocity vin = p0.finalVelocity(p1, t1);
	double omega = Units::from("deg/s",-1.9877);
	std::pair<Vect3,Velocity> tp = Kinematics::turnOmega(p1.vect3(),vin,t2-t1,omega);;
	Position eotPos(tp.first);
	Velocity vout = tp.second;
	//fpln(" $$$ eotPos = "+eotPos+" vin = "+vin+"  vout = "+vout);
	double radius = vin.gs()/omega;
	NavPoint np1(p1,t1, "BOT");
	Position turnCenter = KinematicsPosition::centerFromRadius(p1, radius, vin.trk());
	TcpData tcp1 = TcpData::makeFull("Orig", "BOT", "NONE", "NONE",	radius, turnCenter, 0.0, 0.0);
	lpc.add(np1,tcp1);
	//Velocity vout = Velocity::makeTrkGsVs(56.62, 200.0, 0.00);
	NavPoint np2(eotPos,t2, "EOT");
	Position center = Position::ZERO_LL();
	TcpData tcp2 = TcpData::makeFull("Orig", "EOT", "NONE", "NONE",	radius, center, 0.0, 0.0);
	lpc.add(np2,tcp2);
	//fpln(" $$ lpc = "+lpc);
	//DebugSupport::dumpPlan(lpc, "testInitialVelocityLastPointIsEOT_lpc");
	//fpln(" $$ lpc.finalVelocity(1).trk() = "+Units::str("deg", lpc.finalVelocity(1).trk()));
	//fpln(" $$ lpc.initialVelocity(2).trk() = "+Units::str("deg", lpc.initialVelocity(2).trk()));
	EXPECT_NEAR(vout.trk(),lpc.finalVelocity(1).trk(),0.001);
	EXPECT_NEAR(vout.trk(),lpc.initialVelocity(2).trk(),0.001);   // Should be = vout.trk()
	//		for (double t = t1; t <= t2; t = t + 0.1) {
	//			fpln(" $$ for t = "+Fm2(t)+" lpc.velocity(t).trk() = "+Units::str("deg",lpc.velocity(t).trk()));
	//		}
	Position p3 = eotPos.linear(vout, 20.0);
	NavPoint np3(p3,t2+20);
	lpc.addNavPoint(np3);
	// fpln(" $$ lpc.initialVelocity(2).trk() = "+Units::str("deg", lpc.initialVelocity(2).trk()));
}


TEST_F(PlanTest, testInitialVelocityLastPointIsEndTCP) {
	Position p0(LatLonAlt::make(32.945975, -96.764681, 3553.852644));
	Position p1(LatLonAlt::make(32.909926, -96.645135, 10684.195597));
	double t0 = 0.0;
	double t1 = t0 + p0.distanceH(p1)/Units::from("kn",450);
	NavPoint np0(p0,t0);
	lpc.addNavPoint(np0);
	Velocity vin = p0.finalVelocity(p1, t1);
	double omega = Units::from("deg/s",4.1);
	double accelTime = 7;
	std::pair<Position,Velocity> tp = ProjectedKinematics::turnOmega(p1,vin,accelTime,omega);;
	Position eotPos = tp.first;
	Velocity vout = tp.second;
	double t2 = t1 + accelTime;
	double sgnRadius = vin.gs()/omega;
	NavPoint np1(p1,t1, "BOT");
	Position turnCenter = KinematicsPosition::centerFromRadius(p1, sgnRadius, vin.trk());


	TcpData tcp1 = TcpData::makeFull("Orig", "BOT", "NONE", "NONE",	sgnRadius, turnCenter, 0.0, 0.0);
	lpc.add(np1,tcp1);
	NavPoint np2(eotPos,t2,"EOT");
	TcpData tcp2 = TcpData::makeFull("Orig", "EOT", "NONE", "NONE", 0.0, Position::ZERO_LL(), 0.0, 0.0);
	lpc.add(np2,tcp2);
	//fpln(" $$ lpc = "+lpc);
	EXPECT_NEAR(vout.gs(),lpc.initialVelocity(0).gs(),0.001);
	EXPECT_NEAR(vout.gs(),lpc.finalVelocity(0).gs(),0.001);
	// **************** remove first point ****************************
	lpc.remove(0);
	EXPECT_NEAR(vin.trk(),lpc.initialVelocity(0).trk(),0.001);
	//f.pln("\n $$##@@ lpc.finalVelocity(0).gs() = "+Units::str("kn",lpc.finalVelocity(0).gs()));
	//fpln(" $$##@@ lpc.gsOut(0) = "+Units::str("kn",lpc.gsOut(0)));
	//f.pln(" $$##@@ lpc.vout.gs()(0) = "+Units::str("kn",vout.gs()));
	EXPECT_NEAR(lpc.finalVelocity(0).gs(),lpc.gsOut(0), 0.001);
	EXPECT_NEAR(vout.trk(),lpc.initialVelocity(1).trk(),0.02);


	// GROUND SPEED ACCEL
	lpc.clear();
	lpc.addNavPoint(np0);
	accelTime = 7;
	double gsAccel = 2.35;
	tp = ProjectedKinematics::gsAccel(p1,vin,accelTime,gsAccel);;
	Position egsPos = tp.first;
	vout = tp.second;
	//fpln(" $$$$$$$$$$$$$ egsPos = "+egsPos+" vin = "+vin+"  vout = "+vout);
	t2 = t1 + accelTime;
	//double radius = vin.gs()/omega;
	np1 = NavPoint(p1,t1,"BGS");
	tcp1 = TcpData::makeFull("Orig", "NONE", "BGS", "NONE",	0.0, Position::ZERO_LL(), gsAccel, 0.0);
	lpc.add(np1,tcp1);
	np2 = NavPoint(egsPos,t2, "EGS");
	tcp2 = TcpData::makeFull("Orig", "NONE", "EGS", "NONE", 0.0, Position::ZERO_LL(), 0.0, 0.0);
	lpc.add(np2,tcp2);
	//fpln(" $$ lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testInitialVelocityLastPointIsEndTCP_lpc");
	//fpln(" $$ lpc.finalVelocity(1).gs() = "+Units::str("kn", lpc.finalVelocity(1).gs()));
	//fpln(" $$ lpc.initialVelocity(2).gs() = "+Units::str("kn", lpc.initialVelocity(2).gs()));
	//fpln(" $$>>>>>>. lpc.finalVelocity(1).gs() = "+Units::str("kn", lpc.finalVelocity(1).gs()));
	//fpln(" $$>>>>>>> lpc.initialVelocity(2).gs() = "+Units::str("kn", lpc.initialVelocity(2).gs()));
	//fpln(" $$>>>>>>> vout.gs() = "+Units::str("kn", vout.gs()));
	EXPECT_NEAR(vout.gs(),lpc.finalVelocity(1).gs(),0.001);
	EXPECT_NEAR(vout.gs(),lpc.initialVelocity(2).gs(),0.001);
	lpc.remove(0);
	EXPECT_NEAR(vin.gs(),lpc.initialVelocity(0).gs(),0.001);
	//EXPECT_NEAR(vout.gs(),lpc.finalVelocity(0).gs(),0.001);
	EXPECT_NEAR(vout.gs(),lpc.initialVelocity(1).gs(),0.001);

	// VERTICAL SPEED ACCEL
	lpc.clear();
	lpc.addNavPoint(np0);
	accelTime = 9;
	double vsAccel = 1.77;
	tp = ProjectedKinematics::vsAccel(p1,vin,accelTime,vsAccel);;
	Position evsPos = tp.first;
	vout = tp.second;
	//fpln(" $$$$$$$$$$$$$ evsPos = "+evsPos+" vin = "+vin+"  vout = "+vout);
	t2 = t1 + accelTime;
	np1 = NavPoint(p1,t1, "BVS");
	tcp1 = TcpData::makeFull("Orig", "NONE", "NONE", "BVS", 0.0, Position::ZERO_LL(), 0.0, vsAccel);
	lpc.add(np1,tcp1);
	np2 = NavPoint(evsPos,t2,"EVS");
	tcp2 = TcpData::makeFull("Orig", "NONE", "NONE", "EVS", 0.0, Position::ZERO_LL(), 0.0, 0.0);
	lpc.add(np2,tcp2);
	//fpln(" $$ lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "testInitialVelocityLastPointIsEndTCP_lpc");
	//fpln(" $$ lpc.finalVelocity(1).vs() = "+Units::str("kn", lpc.finalVelocity(1).vs()));
	//fpln(" $$ lpc.initialVelocity(2).vs() = "+Units::str("kn", lpc.initialVelocity(2).vs()));
	EXPECT_NEAR(vout.vs(),lpc.finalVelocity(1).vs(),0.001);
	EXPECT_NEAR(vout.vs(),lpc.initialVelocity(2).vs(),0.001);
	lpc.remove(0);
	EXPECT_NEAR(vin.vs(),lpc.initialVelocity(0).vs(),0.001);
	EXPECT_NEAR(vout.vs(),lpc.finalVelocity(0).vs(),0.001);
	EXPECT_NEAR(vout.vs(),lpc.initialVelocity(1).vs(),0.001);
}



TEST_F(PlanTest, testTooClose) {

	Plan hPlan("");
	Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899));
	Position p1(LatLonAlt::make(-0.354700001, -1.320799999, 7899.00001));
	Position p2(LatLonAlt::make(-0.354699998, -1.320800002, 7898.99999));
	Position p3(LatLonAlt::make(-1.549400, 0.351100, 3983));
	Position p4(LatLonAlt::make(-1.845700, 0.899500, 3993));
	Position p5(LatLonAlt::make(-2.001500, 1.187700, 59177));
	NavPoint np0(p0, 97.0);
	NavPoint np1(p1, 97.0+1E-8);
	NavPoint np2(p2, 97.0+2E-8);
	NavPoint np3(p3,197.0);
	NavPoint np4(p4,197.0+1E-8);
	NavPoint np5(p5,297.0);
	hPlan.addNavPoint(np0);
	hPlan.addNavPoint(np1);
	hPlan.addNavPoint(np2);
	hPlan.addNavPoint(np3);
	hPlan.addNavPoint(np4);
	hPlan.addNavPoint(np5);
	Velocity v0 = hPlan.initialVelocity(0);
	Velocity v1 = hPlan.initialVelocity(1);
	Velocity v2 = hPlan.initialVelocity(2);
	Velocity v3 = hPlan.initialVelocity(3);
	Velocity v4 = hPlan.initialVelocity(4);
	//fpln(""+v0.toString12NP());
	//fpln(""+v1.toString12NP());
	//fpln(""+v2.toString12NP());
	//fpln(""+v3.toString12NP());
	//fpln(""+v4.toString12NP());
	EXPECT_TRUE(hPlan.size() == 6);
	EXPECT_TRUE(!v0.isInvalid());
	EXPECT_TRUE(!v1.isInvalid());
	EXPECT_TRUE(!v2.isInvalid());
	EXPECT_TRUE(!v3.isInvalid());
	EXPECT_TRUE(!v4.isInvalid());
	EXPECT_TRUE(!v0.isZero());
	EXPECT_TRUE(!v1.isZero());
	EXPECT_TRUE(!v2.isZero());
	EXPECT_TRUE(!v3.isZero());
	EXPECT_TRUE(!v4.isZero());
	EXPECT_FALSE(hPlan.hasError());
	EXPECT_FALSE(hPlan.isWellFormed());
	Velocity vf0 = hPlan.finalVelocity(0);
	//fpln("vf0= "+vf0.toString12NP());
	//        EXPECT_TRUE(vf0.within_epsilon(v1, 1.0));
	//        EXPECT_TRUE(hPlan.finalVelocity(1).within_epsilon(v2, 0.1));
	//        EXPECT_TRUE(hPlan.finalVelocity(2).within_epsilon(v3, 0.1));



	// Euclidean
	Position e0(Vect3(Units::from("nmi",0.0), Units::from("nmi",0.0), Units::from("ft",20000)));
	Position e1(Vect3(Units::from("nmi",1.0), Units::from("nmi",1.0), Units::from("ft",20000)));
	Position e2(Vect3(Units::from("nmi",1.0+1E-13), Units::from("nmi",1.0), Units::from("ft",20000)));
	Position e3(Vect3(Units::from("nmi",1.0+1E-13), Units::from("nmi",1.0+1E-13), Units::from("ft",20000)));
	NavPoint n0(e0, 1.000000000);
	NavPoint n1(e1, 2.000000000);
	NavPoint n2(e2, 2.000000000+1E-13);
	NavPoint n3(e3, 2.000000000+2E-13);
	Plan ePlan("");
	ePlan.addNavPoint(n0);
	ePlan.addNavPoint(n1);
	ePlan.addNavPoint(n2);
	ePlan.addNavPoint(n3);
	Velocity fv0 = ePlan.finalVelocity(0);
	Velocity fv1 = ePlan.finalVelocity(1);
	Velocity fv2 = ePlan.finalVelocity(2);
	Velocity fv3 = ePlan.finalVelocity(3);                // at end of plan should return 0;
	//fpln("fv0 = "+fv0.toString12NP());
	//fpln("fv1 = "+fv1.toString12NP());
	//fpln("fv2 = "+fv2.toString12NP());
	//fpln("fv3 = "+fv3.toString12NP());
	//fpln(" "+fv0.x+" "+fv0.y);
	//fpln(" "+fv1.x+" "+fv1.y);
	EXPECT_NEAR(1852,fv0.x,0.000001);
	EXPECT_NEAR(1852,fv0.y,0.000001);
	double xcomp = 1852.302222;


	bool oldFinalVelocity = false;
	if (oldFinalVelocity) {
		EXPECT_NEAR(1852.0,fv1.x,0.000001);
		EXPECT_NEAR(1852.0,fv1.y,0.000001);
		EXPECT_NEAR(1852.0,fv2.x,0.000001);
		EXPECT_NEAR(1852.0,fv2.y,0.000001);
		EXPECT_NEAR(M_PI/4.0,fv1.trk(),0.0001);
		EXPECT_NEAR(M_PI/4.0,fv2.trk(),0.0001);
		EXPECT_NEAR(0.0,fv3.trk(),0.0001);
		EXPECT_NEAR(2619.1235,fv1.gs(),0.0001);
		EXPECT_NEAR(2619.1235,fv2.gs(),0.0001);
	} else {
		EXPECT_NEAR(Units::from("deg",90.0),fv1.trk(),0.000001);
		EXPECT_NEAR(1852.302,fv1.gs(),0.001);
		EXPECT_NEAR(0.0,fv2.trk(),0.000001);
		EXPECT_NEAR(1852.302,fv2.gs(),0.001);
	}
	EXPECT_NEAR(0.0,fv3.y,0.000001);
	EXPECT_NEAR(0.0,fv3.x,0.000001);
	EXPECT_NEAR(0.0,fv3.gs(),0.0001);
	EXPECT_NEAR(2619.1235,fv0.gs(),0.0001);
	EXPECT_NEAR(M_PI/4.0,fv0.trk(),0.0001);
	Velocity iv0 = ePlan.initialVelocity(0);
	Velocity iv1 = ePlan.initialVelocity(1);
	Velocity iv2 = ePlan.initialVelocity(2);
	Velocity iv3 = ePlan.initialVelocity(3);                // at end of plan should return 0;
	//fpln(""+iv0.toString12NP());
	//fpln(""+iv1.toString12NP());
	//fpln(""+iv2.toString12NP());
	//fpln(""+iv3.toString12NP());
	//fpln(" "+iv0.x+" "+iv0.y);
	//fpln(" "+iv1.x+" "+iv1.y);
	xcomp = 1851.999999;
	EXPECT_NEAR(1852,iv0.x,0.000001);
	EXPECT_NEAR(1852,iv0.y,0.000001);

	bool oldInitialVelocity = false;
	if (oldInitialVelocity) {
		EXPECT_NEAR(xcomp,iv1.x,0.000001);
		EXPECT_NEAR(xcomp,iv1.y,0.000001);
		EXPECT_NEAR(xcomp,iv2.x,0.000001);
		EXPECT_NEAR(xcomp,iv2.y,0.000001);
		EXPECT_NEAR(xcomp,iv3.x,0.000001);
		EXPECT_NEAR(xcomp,iv3.y,0.000001);
		EXPECT_NEAR(2619.1235,iv1.gs(),0.0001);
		EXPECT_NEAR(2619.1235,iv2.gs(),0.0001);              // compare with fv2
		EXPECT_NEAR(M_PI/4,iv1.trk(),0.0001);
		EXPECT_NEAR(M_PI/4,iv2.trk(),0.0001);
		EXPECT_NEAR(M_PI/4,iv3.trk(),0.0001);
		EXPECT_NEAR(2619.1235,iv0.gs(),0.0001);
		EXPECT_NEAR(2619.1235,iv3.gs(),0.0001);
	} else {
		EXPECT_NEAR(M_PI/2,iv1.trk(),0.000001);
		EXPECT_NEAR(1852.302, iv1.gs(),0.001);
		EXPECT_NEAR(0.0,iv2.trk(),0.000001);
		EXPECT_NEAR(1852.302,iv2.gs(),0.001);
		EXPECT_NEAR(0.0,iv3.trk(),0.001);
		EXPECT_NEAR(1852.302,iv3.gs(),0.001);
		EXPECT_NEAR(0.0,iv3.trk(),0.0001);
		EXPECT_NEAR(2619.1235,iv0.gs(),0.0001);
		EXPECT_NEAR(1852.3022,iv3.gs(),0.0001);
	}
	EXPECT_NEAR(M_PI/4,iv0.trk(),0.0001);

	Position e4(Vect3(Units::from("nmi",1.5), Units::from("nmi",1.0), Units::from("ft",20000)));

	NavPoint n4(e4, 2.5);
	ePlan.addNavPoint(n4);
	fv3 = ePlan.finalVelocity(3);
	EXPECT_NEAR(1852,fv3.x,0.000001);
	EXPECT_NEAR(0,fv3.y,0.000001);
	EXPECT_NEAR(1852,fv3.gs(),0.0001);
	EXPECT_NEAR(M_PI/2.0,fv3.trk(),0.0001);
	fv2 = ePlan.finalVelocity(2);
	xcomp = 1852;
	if (oldInitialVelocity) {
		EXPECT_NEAR(xcomp,fv2.x,0.000001);
		EXPECT_NEAR(1852,fv2.y,0.000001);
		EXPECT_NEAR(M_PI/4,fv2.trk(),0.0001);
	} else {
		EXPECT_NEAR(1852.3022,fv2.gs(),0.0001);
	}
}

TEST_F(PlanTest, testOverlap) {
	Plan hPlan("");
	Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899));
	NavPoint np0(p0,97.0);
	NavPoint np0EOT = np0.makeName("EOT");
	TcpData np0EOTTcp;
	np0EOTTcp.setEOT();

	NavPoint np0BVS = np0.makeName("BVS");
	TcpData np0BVSTcp;
	np0BVSTcp.setBVS(1.0);

	//
	// Check same time, same position
	//
	hPlan.add(np0EOT, np0EOTTcp);
	hPlan.add(np0BVS, np0BVSTcp);
	EXPECT_FALSE(hPlan.hasMessage());
	EXPECT_EQ(1,hPlan.size());
	EXPECT_TRUE(hPlan.isEOT(0));
	EXPECT_TRUE(hPlan.isBVS(0));
	EXPECT_TRUE(hPlan.point(0).name() == "EOTBVS");
	//
	// non-overlap
	//
	Position p1(LatLonAlt::make(-1.015100, -1.100000, 7899));
	NavPoint np0tm1(p0,97.0);
	NavPoint np0tm2(p0,97.0000001);
	NavPoint np1tm1(p1,97.0);

	NavPoint np0tm1EOT = np0tm1.makeName("EOT");
	TcpData np0tm1EOTTcp;
	np0tm1EOTTcp.setEOT();
	NavPoint np0tm2BVS = np0tm2.makeName("BVS");
	TcpData np0tm2BVSTcp;
	np0tm2BVSTcp.setBVS(1.0);
	NavPoint np1tm1BVS = np1tm1.makeName("BVS");
	TcpData np1tm1BVSTcp;
	np1tm1BVSTcp.setBVS(1.0);

	// different time, otherwise compatible points
	hPlan = Plan("");
	hPlan.add(np0tm1EOT,np0tm1EOTTcp);
	hPlan.add(np0tm2BVS,np0tm2BVSTcp);
	EXPECT_FALSE(hPlan.hasMessage());
	EXPECT_EQ(2, hPlan.size());
	EXPECT_TRUE(hPlan.isEOT(0));
	EXPECT_TRUE(hPlan.isBVS(1));
	// same time, different position, otherwise compatible points
	hPlan = Plan("");
	hPlan.add(np0tm1EOT,np0tm1EOTTcp);
	hPlan.add(np1tm1BVS,np1tm1BVSTcp);
	EXPECT_FALSE(hPlan.hasError());
	EXPECT_FALSE(hPlan.hasMessage());
}


//TEST_F(PlanTest, testCopyTimeShift2) {
//	LatLonAlt wp5 = LatLonAlt::make(40.00, -93, 800.0);
//	LatLonAlt wp6 = LatLonAlt::make(37.00, -95.8, 600.0);
//	double gs = Units::from("kn",  530);
//	double vs = Units::from("fpm", 2000);
//	double cruiseAlt2 = Units::from("ft",9000);
//
//	Plan kpc3 = TrajTemplates::makeKPC_Turn(wp5, wp6, gs, vs, cruiseAlt2);
//	Plan kpc3Tm = (Plan) kpc3.copyAndTimeShift(0, 220);
//	//fpln(" kpc3Tm = "+kpc3Tm);
//	for (int i = 0; i < lpc.size(); i++) {                // Unchanged
//		EXPECT_TRUE(kpc3.point(i).position() == kpc3Tm.point(i).position());
//		EXPECT_NEAR(kpc3.point(i).time()+220, kpc3Tm.point(i).time(),0.001);
//	}
//}


TEST_F(PlanTest, testPlanUpdateShouldBeNOOP) {
	Plan traj("");
	Position p0  = Position(LatLonAlt::make(-0.835368, 0.424420, 23330.054754));
	Position p1  = Position(LatLonAlt::make(-0.837288, 0.425092, 23330.156655));
	Position p2  = Position(LatLonAlt::make(-0.843043, 0.427304, 23330.464340));
	Position p3  = Position(LatLonAlt::make(-0.848652, 0.429864, 23330.771441));
	Position p4  = Position(LatLonAlt::make(-0.848652, 0.429864, 23330.771822));
	Position p5  = Position(LatLonAlt::make(-2.445261, 1.266038, 23327.273806));
	Position p6  = Position(LatLonAlt::make(-2.447100, 1.267000, 23327.270700));
	NavPoint np0 = NavPoint(p0,867.574451);    	 traj.addNavPoint(np0);
	Velocity vin1 = Velocity::mkTrkGsVs(2.805172,226.482091,0.030998);
	NavPoint src1 =  NavPoint::makeLatLonAlt(-0.84311626, 0.42713043, 23330.46433971, 871.601860);
	double radius = -vin1.gs()/0.020191;
	Position turnCenter = KinematicsPosition::centerFromRadius(p1, radius, vin1.trk());
	std::pair<NavPoint,TcpData> np1 = Plan::makeBOT( p1, 868.572669, radius, turnCenter);    	 traj.add(np1);
	NavPoint np2 = NavPoint(p2,871.598091);    	 traj.addNavPoint(np2);
	Velocity vin3 = Velocity::mkTrkGsVs(2.682999,226.482091,0.030998);
	NavPoint src3 =  NavPoint::makeLatLonAlt(-0.84311626, 0.42713043, 23330.46433971, 871.601860);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEOT(p3, 874.623514);    	 traj.add(np3);
	NavPoint np4 = NavPoint(p4,874.623524);    	 traj.addNavPoint(np4);
	NavPoint np5 = NavPoint(p5,1758.818036);    	 traj.addNavPoint(np5);
	NavPoint np6 = NavPoint(p6,1759.836259);    	 traj.addNavPoint(np6);
	EXPECT_EQ(7,traj.size());
	// The following should be a NOOP, but it is not!
	// interesting -- the point is apparently returning the reference to the vector storage location, so when we remove one, it "shifts" to the next indexed value...
	traj.setNavPoint(4, traj.point(4)); // .makeName("LABEL**");    //***** GETTING RID OF makeLabel CAUSES C++ INCONSISTENCY
	EXPECT_EQ(7,traj.size());            // This is 6, but it should be 7!!!!!!!!!!
}




TEST_F(PlanTest, testVelocityNearEnd) {
	LatLonAlt KMSP = LatLonAlt::make(44.878101, -93.21869, 10000.0);
	LatLonAlt KDFW = LatLonAlt::make(32.898700, -97.04039, 10000.0);
	NavPoint np1(Position(KMSP), 0);
	NavPoint np2(Position(KDFW), 2000);
	Plan lpc("testVelocityNearEndPoint");
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	//DebugSupport.dumpPlan(lpc, "testVelocityNearEnd_lpc");
	Velocity v = lpc.positionVelocity(np2.time()-0.0001).second;
	Velocity vold = lpc.velocity(np2.time()-0.0001);
	Velocity vf = lpc.finalVelocity(0);
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	v = lpc.positionVelocity(np2.time()-0.00001).second;
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	v = lpc.positionVelocity(np2.time()-0.000001).second;
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	v = lpc.positionVelocity(np2.time()-1E-13).second;
	EXPECT_NEAR(vf.trk(),v.trk(),0.0001);
	EXPECT_NEAR(vf.gs(),v.gs(),0.0001);
	EXPECT_NEAR(vf.vs(),v.vs(),0.0001);
	//fpln(" $$>>!! kpc.finalVelocity(0).trk() = "+kpc.finalVelocity(0).trk()+" kpc.initialVelocity(1).trk() = "+kpc.initialVelocity(1).trk());
	EXPECT_NEAR(lpc.finalVelocity(0).trk(),lpc.initialVelocity(1).trk(),0.0001);
	EXPECT_NEAR(lpc.finalVelocity(0).gs(),lpc.initialVelocity(1).gs(),0.0001);
	EXPECT_NEAR(lpc.finalVelocity(0).vs(),lpc.initialVelocity(1).vs(),0.0001);
}



TEST_F(PlanTest, testVelocityNewTime) {
	Vect3 v;
	//fpln(" $$$ testVelocityNewTime: fp = "+fp.toStringGs());
	//System.out.println("TEST2");
	v = fp.positionVelocity(1.01).second;
	EXPECT_TRUE( ! fp.hasError());
	EXPECT_NEAR(1.89876e-14, v.x, 0.0000001);
	EXPECT_NEAR(-155.0, v.y, 0.5);
	EXPECT_NEAR(0.0, v.z, 0.0);

	//System.out.println("TEST3");
	double t3 = (239.6590 - 1.0) / 2.0 + 1.0;
	//fpln(" $$$ testVelocityNewTime: t3 = "+t3);
	v = fp.positionVelocity(t3).second;
	EXPECT_TRUE( ! fp.hasError());
	EXPECT_NEAR(1.89876e-14, v.x, 0.000001);
	EXPECT_NEAR( -155.0, v.y, 0.5);
	EXPECT_NEAR( 0.0, v.z, 0.0);
}




TEST_F(PlanTest, testLongPath) {
	LatLonAlt KMSP = LatLonAlt::make(44.878101, -93.21869, 10000.0);
	LatLonAlt KDFW = LatLonAlt::make(32.898700, -97.04039, 10000.0);
	NavPoint np1(Position(KMSP), 0);
	NavPoint np2(Position(KDFW), 4000);
	Plan lpc("longPath");
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	for (double t = 0; t <= lpc.getLastTime(); t=t+1.0) {
		std::pair<Position, Velocity> pvp = lpc.positionVelocity(t);
		Position pos = lpc.position(t);
		Velocity vel = lpc.velocity(t);
		double distBetw = pos.distanceH(pvp.first);
		EXPECT_TRUE(distBetw < 10);
		EXPECT_NEAR(pos.lat(),pvp.first.lat(),0.0000001);
		EXPECT_NEAR(pos.lon(),pvp.first.lon(),0.0000001);
		//double deltaAlt = pos.alt() - positionVelocity::first.alt();
		//fpln(t+" $$ positionVelocity3: deltaAlt = "+Units::str("ft",deltaAlt,8));
		//fpln(t+" $$ positionVelocity3: vel = "+vel+" positionVelocity::second = "+positionVelocity::second);
		//fpln(t+" positionVelocity(t).trk = "+Units::str("deg",positionVelocity::second.trk(),4));
		//fpln(t+" lpc.velocity(t).trk = "+Units::str("deg",vel.trk(),4));
		//double deltaTrack = positionVelocity::second.trk() - vel.trk();
		//fpln(t+" deltaTrack = "+Units::str("deg",deltaTrack,8));
		EXPECT_NEAR(pos.alt(),pvp.first.alt(),0.0000000000001);
		EXPECT_NEAR(vel.trk(),pvp.second.trk(),0.000001);
		EXPECT_NEAR(vel.gs(),pvp.second.gs(),0.0001);
		EXPECT_NEAR(vel.vs(),pvp.second.vs(),0.00000000001);
	}
	//DebugSupport.dumpPlan(lpc, "longPath_lpc");
	// convert to BGS EGS
	Velocity vBGS = lpc.initialVelocity(0);
	double gsAccel = 3.4;
	double dist = lpc.pathDistance(0,1);
	double gsEGS = vBGS.gs();
	double dt = PlanUtil::timeFromGs(gsEGS, gsAccel, dist);
	//fpln(" $$$$ dt = "+dt);
	std::pair<NavPoint,TcpData> npBGS = Plan::makeBGS("", np1.position(),np1.time(),gsAccel);
	std::pair<NavPoint,TcpData> npEGS = Plan::makeEGS(np2.position(),np1.time()+dt);
	Plan kpc("longPathGS");
	kpc.add(npBGS);
	kpc.add(npEGS);
	//fpln("  $$$ kpc = "+kpc.toString());
	//fpln(" ---------------------------------------------- ");
	EXPECT_TRUE(kpc.isWeakConsistent());  // TODO: ??
	Plan trace("");
	Plan traceNew("'");
	//DebugSupport.dumpPlan(kpc, "longPath_kpc");
	for (double t = 0; t <= kpc.getLastTime(); t=t+10.0) {
		std::pair<Position, Velocity> pvp = kpc.positionVelocity(t);
		Position pos = kpc.position(t);
		Velocity vel = kpc.velocity(t);
		//fpln(t+" $$ positionVelocity3: pos = "+pos+" pvPair.first = "+positionVelocity::first);
		NavPoint np1 = NavPoint(pos,t);
		trace.addNavPoint(np1);
		NavPoint np2 = NavPoint(pvp.first,t);
		traceNew.addNavPoint(np2);
		double distBetw = pos.distanceH(pvp.first);
		EXPECT_TRUE(distBetw < 10);
		EXPECT_NEAR(pos.lat(),pvp.first.lat(),0.0000001);
		EXPECT_NEAR(pos.lon(),pvp.first.lon(),0.0000001);
		//double deltaAlt = pos.alt() - positionVelocity::first.alt();
		//fpln(t+" $$ positionVelocity3: deltaAlt = "+Units::str("ft",deltaAlt,8));
		//fpln(t+" $$ positionVelocity3: vel = "+vel+" positionVelocity::second = "+positionVelocity::second);
		//fpln(t+" positionVelocity(t).trk = "+Units::str("deg",positionVelocity::second.trk(),4));
		//fpln(t+" kpc.velocity(t).trk = "+Units::str("deg",vel.trk(),4));
		EXPECT_NEAR(pos.alt(),pvp.first.alt(),0.0000000000001);
		EXPECT_NEAR(vel.trk(),pvp.second.trk(),0.0001);
		EXPECT_NEAR(vel.gs(),pvp.second.gs(),0.00000001);
		EXPECT_NEAR(vel.vs(),pvp.second.vs(),0.00000000001);
	}
	//DebugSupport.dumpPlan(trace, "longPath_trace");
	//DebugSupport.dumpPlan(traceNew, "longPath_traceNew");
}





TEST_F(PlanTest, test_structRevertGsTCPs) {
	Position p0(LatLonAlt::make(34.1626, -118.9911, 5000));
	Position p1(LatLonAlt::make(34.1967, -119.0753, 5000));
	NavPoint np0(p0,0.0);
	np0 = np0.makeName("AA");
	NavPoint np1(p1,100.0);
	np1 =np1.makeName("BB");
	Plan lpc("");
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	Velocity v0 = lpc.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0);
	NavPoint np2 = np1.linear(v0.mkGs(200),40).makeName("CC");
	lpc.addNavPoint(np2);
	NavPoint np3 = np2.linear(v0.mkGs(300),60).makeName("DD");
	lpc.addNavPoint(np3);
	std::string name = lpc.getID();
	std::string note = lpc.getNote();
	EXPECT_EQ(name,lpc.getID());
	EXPECT_EQ(note,lpc.getNote());
	//fpln(" lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc, "structRevertGs_lpc");
	//bool repairGs = true;
	double gsAccel = 2.1;
	//bool useOffset = true;
	Plan kpc = TrajGen::generateGsTCPs(lpc, gsAccel,true);
	//Plan kpc = TrajGen::makeKinematicPlan(lpc, Units::from("deg",20), gsAccel, 1.0, repairGs);
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isWeakConsistent());
	//fpln(" msg = "+kpc.getMessageNoClear());
	gsAccel = 3.3;
	kpc = TrajGen::generateGsTCPs(lpc, gsAccel,true);
	EXPECT_FALSE(kpc.hasError());
	//DebugSupport.dumpPlan(kpc, "structRevertGs_kpc");
	kpc.revertGsTCPs();
	//DebugSupport.dumpPlan(kpc, "structRevertGs_kpc_after");
	for (int k = 0; k < lpc.size(); k++) {
		EXPECT_EQ(lpc.point(k).position(),kpc.point(k).position());
		EXPECT_NEAR(lpc.point(k).time(),kpc.point(k).time(),0.0001); // TODO:  why not better?
		EXPECT_EQ(lpc.point(k).name(),kpc.point(k).name());
	}
	//fpln(" kpc = "+kpc.toStringGs());
}



TEST_F(PlanTest, test_revert_EGSBGS) {
	Position p0  = Position(LatLonAlt::make(34.1626, -118.9911, 5000));
	double t0 = 0;
	Position p1  = Position(LatLonAlt::make(34.1967, -119.0753, 5000));
	NavPoint np0 = NavPoint(p0,t0).makeName("AA");
	Plan lpc = Plan();
	lpc.addNavPoint(np0);
	double speed01 = Units::from("kn",300);
	double dist01 = p0.distanceH(p1);
	double dt01 = dist01/speed01;
	double t1 = t0 + dt01;
	NavPoint np1 = NavPoint(p1,t1).makeName("BB");
	//f.pln(" $$$ v0 = "+v0);
	double track = Units::from("deg",296.11);
	double a1 = 3.1;
	//f.pln(" #### a1 = "+Units::str("kn",a1));
	std::pair<NavPoint,TcpData> bgs = Plan::makeBGS(np1.name(), p1, t1, a1);
	double dt12 = 50;
	double dist12 = speed01*dt12 + 0.5*a1*dt12*dt12;
	Position p2 = p1.linearDist2D(track,dist12);
	double t2 = t1 + dt12;
	NavPoint np2 = NavPoint(p2,t2,"CC");
	double a2 = 1.8; // 1.2;
	std::pair<NavPoint,TcpData> egsbgs = Plan::makeEGSBGS("",p2, t2, a2);
	double dt23 = 70;
	double gsIn2 = speed01 + a1*dt12;
	//f.pln(" ### gsIn2 = "+Units::str("kn",gsIn2));
	double dist23 = gsIn2*dt23 + 0.5*a2*dt23*dt23;
	Position p3 = p2.linearDist2D(track,dist23);
	double t3 = t2 + dt23;
	NavPoint np3 = NavPoint(p3,t3,"DD");
	std::pair<NavPoint,TcpData> egs = Plan::makeEGS(p3, t3);
	lpc.add(bgs);
	lpc.add(egsbgs);
	lpc.add(egs);
	double gsOut3 = lpc.gsOut(3);
	double dist34 = Units::from("NM",25);
	double dt34 = dist34/gsOut3;
	Position p4 = p3.linearDist2D(track,dist34).mkAlt(Units::from("ft",6000));
	double t4 = t3 + dt34;
	NavPoint np4 = NavPoint(p4,t4,"END");
	lpc.addNavPoint(np4);
	//f.pln(" ### lpc = "+lpc.toStringGs());
	//f.pln(" ### gsOut(2) = "+Units::str("kn",lpc.gsOut(2)));
	//f.pln(" ### gsFinal(2) = "+Units::str("kn",lpc.gsFinal(2)));
	double gsOut0 = lpc.gsOut(0);
	//double gsOut1 = lpc.gsOut(1);
	double gsOut2 = lpc.gsOut(2);
    double vsOut2 = lpc.vsOut(2);
	gsOut3 = lpc.gsOut(3);
	//DebugSupport.dumpPlan(lpc, "test_revert_EGSBGS");
	EXPECT_TRUE(lpc.isWeakConsistent());
	lpc.setAltPreserve(3);
	lpc.revertGsTCPs();
	//DebugSupport.dumpPlan(lpc, "test_revert_EGSBGS_after");
	EXPECT_NEAR(gsOut0, lpc.gsOut(0),0.0001);
	EXPECT_NEAR(gsOut2, lpc.gsOut(1),0.0001);
	EXPECT_NEAR(gsOut3, lpc.gsOut(2),0.0001);
	EXPECT_NEAR(gsOut3, lpc.gsOut(3),0.0001);
	EXPECT_TRUE(lpc.isWeakConsistent());
	EXPECT_NEAR(vsOut2, lpc.vsOut(2),0.001);
}




TEST_F(PlanTest, test_structRevertVsTCPs) {
	Position p0(LatLonAlt::make(34.1626, -118.9911, 5000));
	Position p1(LatLonAlt::make(34.1967, -119.0753, 10000));
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,100.0);
	Plan lpc("");
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	Velocity v0 = lpc.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0);
	NavPoint np2 = np1.linear(v0.mkVs(30),40);
	lpc.addNavPoint(np2);
	NavPoint np3 = np2.linear(v0.mkVs(40),60);
	lpc.addNavPoint(np3);
	std::string name = lpc.getID();
	std::string note = lpc.getNote();
	EXPECT_EQ(name,lpc.getID());
	EXPECT_EQ(note,lpc.getNote());
	//fpln(" lpc = "+lpc);
	//DebugSupport.dumpPlan(lpc, "structRevertVs_lpc");
	double vsAccel = 3.8;
	//bool continueGen = false;
	Plan kpc = TrajGen::generateVsTCPs(lpc, vsAccel);
	//Plan kpc = TrajGen::makeKinematicPlan(lpc, Units::from("deg",20), gsAccel, 1.0, repairGs);
	//DebugSupport.dumpPlan(kpc, "structRevertVs_kpc");
	kpc.revertVsTCPs();
	//DebugSupport.dumpPlan(kpc, "structRevertVs_kpc_after");
	for (int k = 0; k < lpc.size(); k++) {
		EXPECT_NEAR(lpc.point(k).lat(),kpc.point(k).lat(),0.00000001);
		EXPECT_NEAR(lpc.point(k).lon(),kpc.point(k).lon(),0.00000001);
		EXPECT_NEAR(lpc.point(k).alt(),kpc.point(k).alt(),0.000000000001);
		EXPECT_NEAR(lpc.point(k).time(),kpc.point(k).time(),1E-14);
	}
}

TEST_F(PlanTest, test_revertVsTCPs_gs0) {
	 Plan kpc("UF17");
	 Position p20  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 684.791996587350);
	 Position p21  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 674.000000000000);
	 NavPoint np20(p20,1349.7740120740598000);     TcpData tcp20;
	 tcp20.setBVS(0.980665000000);
	 kpc.add(np20, tcp20);
	 NavPoint np21(p21,1352.3640912550238000);
	 TcpData tcp21;
	 tcp21.setEVS();
	 int ix21 = kpc.add(np21, tcp21);
	 kpc.setName(ix21,"VP00119");

	 EXPECT_TRUE(kpc.isFlyable());
	 //f.pln(" $$$$ test_revertTCPs: kpc = "+kpc.toStringProfile());
	 Plan lpc = Plan(kpc);
	 lpc.revertVsTCPs();
	 //f.pln(" $$$$ test_revertTCPs: lpc = "+lpc.toStringProfile());
	 //DebugSupport.dumpPlan(lpc,"test_revertVsTCPs_gs0_lpc");
	 //DebugSupport.dumpPlan(kpc,"test_revertVsTCPs_gs0_kpc");
	 EXPECT_EQ(lpc.getName(0),kpc.getName(0));
	 EXPECT_EQ(lpc.getName(1),kpc.getName(1));
	 EXPECT_EQ("",lpc.getName(2));
	 EXPECT_EQ(lpc.getID(),kpc.getID());
	 EXPECT_EQ(lpc.getNote(),kpc.getNote());
}


TEST_F(PlanTest, test_closestPointKin) {
	Plan lpc("");
	Position p0(LatLonAlt::make(34.049879, -117.530758, 12654.000000));
	Position p1(LatLonAlt::make(34.320226, -117.631595, 11552.000000));
	Position p2(LatLonAlt::make(34.507122, -117.968878, 12274.000000));
	Position p3(LatLonAlt::make(34.767908, -118.106225, 12125.000000));
	NavPoint np0(p0,430);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,610);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,830);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1000);    	 lpc.addNavPoint(np3);

	Plan kpc = TrajGen::makeKinematicPlan(lpc, Units::from("deg", 15.0), 0.5, 0.5, false, false, false);

	EuclideanProjection proj = Projection::createProjection(p0.mkAlt(0));

	// euclidean version:
	Plan lpc2("");
	Position p0a(proj.project(p0));
	Position p1a(proj.project(p1));
	Position p2a(proj.project(p2));
	Position p3a(proj.project(p3));
	NavPoint np0a(p0a,430);    	 lpc2.addNavPoint(np0a);
	NavPoint np1a(p1a,610);    	 lpc2.addNavPoint(np1a);
	NavPoint np2a(p2a,830);    	 lpc2.addNavPoint(np2a);
	NavPoint np3a(p3a,1000);    	 lpc2.addNavPoint(np3a);

	Plan kpc2 = TrajGen::makeKinematicPlan(lpc2, Units::from("deg", 15.0), 0.5, 0.5, false, false, false);

	// euclidean linear plan
	for (double t = lpc2.getFirstTime(); t < lpc2.getLastTime(); t += 10) {
		Position pos = lpc2.position(t);
		NavPoint closest = lpc2.closestPoint(pos);
		//fpln(" $$ test_closestPointKinXYZ t="+t+" inTurn="+kpc2.inTrkChange(t)+" pos="+pos+" closest="+closest);
		EXPECT_TRUE(pos.almostEquals(closest.position()));
		EXPECT_NEAR(t,closest.time(),0.001);
	}

	// latlon linear plan
	for (double t = lpc.getFirstTime(); t < lpc.getLastTime(); t += 10) {
		Position pos = lpc.position(t);
		NavPoint closest = lpc.closestPoint(pos);
		EXPECT_TRUE(pos.almostEquals(closest.position()));
		//fpln("test_closestPointKinLLA t="+t+" inTurn="+kpc.inTrkChange(t)+" pos="+pos+" closest="+closest);
		EXPECT_NEAR(t,closest.time(),0.001);
	}

	//subproblems of the kinematic plans below
	// kpc2, t=640, seg=5, gsaccel = -0.5
	Vect3 a = Vect3::makeXYZ(-6.906340, "nmi", 17.513092, "nmi", 11630.060613, "ft"); // point 5
	Vect3 b = Vect3::makeXYZ(-7.731070, "nmi", 18.070208, "nmi", 11665.617635, "ft"); // point 6
	double dttot = 10.717875847086361; // from a to b
	double dt = 7.3747530587234; // 640-getTime(5)
	double acc = -0.5; // from point 5
	Vect3 so = Vect3::makeXYZ(-7.476578, "nmi", 17.898295, "nmi", 11654.526679, "ft"); // from position(t=640)
	Velocity v0 = Velocity::makeTrkGsVs(304.039514, 339.506486, 199.052622); // from initialVelocity(5)
	Velocity vv = Velocity::make(b.Sub(a).Scal(1.0/dttot));

	double gs2 = v0.gs()+acc*dt;
	double avggs = (v0.gs()+gs2)/2.0;
	Velocity v1 = v0.mkGs(avggs);
	Vect3 so2 = a.AddScal(dt, v1);
	Vect3 c = VectFuns::closestPoint(a, b, so);
	EXPECT_NEAR(so.x,so2.x,0.001);
	EXPECT_NEAR(so.y,so2.y,0.001);
	EXPECT_NEAR(so.z,so2.z,0.001);
	EXPECT_NEAR(v0.trk(),vv.trk(),0.00001);
	EXPECT_NEAR(v0.vs(),vv.vs(),0.0001);

	EXPECT_NEAR(so.x,c.x,0.001);
	EXPECT_NEAR(so.y,c.y,0.001);
	EXPECT_NEAR(so.z,c.z,0.05);




	// euclidean kin plan
	for (double t = kpc2.getFirstTime(); t < kpc2.getLastTime(); t += 10) {
		Position pos = kpc2.position(t);
		NavPoint closest = kpc2.closestPoint(pos);
		//fpln(" $$ test_closestPointKin t="+Fm3(t)+" inTurn="+Fmb(kpc2.inTrkChange(t))+" pos="+pos.toString()+" closest="+closest.toString());
		EXPECT_NEAR(pos.x(),closest.x(),0.0001);
		EXPECT_NEAR(pos.y(),closest.y(),0.0001);
		EXPECT_NEAR(pos.z(),closest.z(),0.0001);
		//fpln("test_closestPointKinXYZ t="+t+" inTurn="+kpc2.inTrkChange(t)+" pos="+pos+" closest="+closest);
		EXPECT_NEAR(t,closest.time(),0.001);
	}

	// latlon kin plan
	for (double t = kpc.getFirstTime(); t < kpc.getLastTime(); t += 10) {
		Position pos = kpc.position(t);
		NavPoint closest = kpc.closestPoint(pos);
		EXPECT_NEAR(pos.lat(),closest.lat(),0.000001);
		EXPECT_NEAR(pos.lon(),closest.lon(),0.000001);
		EXPECT_NEAR(pos.alt(),closest.alt(),0.0001);
		//fpln("test_closestPointKinLLA t="+t+" inTurn="+kpc.inTrkChange(t)+" pos="+pos+" closest="+closest);
		EXPECT_NEAR(t,closest.time(),0.001);
	}

}


TEST_F(PlanTest, test_timeFromDistance) {
	Plan lpc("test_timeFromDistance");
	Position p1(LatLonAlt::make(-0.60, 1.50, 23000));
	Position p2(LatLonAlt::make(-1.30, 1.70, 38000));
	Position p3(LatLonAlt::make(-1.70, 1.40, 25000));
	Position p4(LatLonAlt::make(-3.50, 0.50, 25000));
	Position p5(LatLonAlt::make(-4.70, 1.10, 20000));
	double startTime = 1450;
	NavPoint np1(p1,startTime);   lpc.addNavPoint(np1);
	NavPoint np2(p2,1706.60);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1975.43);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,2710.95);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,3333.00);    	 lpc.addNavPoint(np5);
	double speed = Units::from("kn",400);
	lpc = PlanUtil::mkGsConstant(lpc,speed);
	//DebugSupport.dumpPlan(lpc,"test_timeFromDistance");
	//fpln(" $$$$ test_timeFromDistance: lpc = "+lpc.toStringGs());
	double pathDist = lpc.pathDistance();
	for (double d = 0.0; d <= pathDist; d = d + Units::from("NM",0.01)) {
		double tm = lpc.timeFromDistance(d);
		//fpln(" $$$$  d = "+Units::str("NM",d)+" tm = "+Fm3(tm));
		EXPECT_NEAR(startTime+d/speed,tm,0.00000001);
	}
	double gsAccel = 0.5;
	double vsAccel = 0.5;
	double bankAngle = Units::from("deg", 10);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln("testVsLevelOutWithGS: kpc = "+kpc);
	EXPECT_TRUE(kpc.isWeakConsistent());
	pathDist = kpc.pathDistance();
	for (double d = 0.0; d <= pathDist; d = d + Units::from("NM",1.0)) {
		double tm = kpc.timeFromDistance(d);
		//fpln(" $$$$  d = "+Units::str("NM",d)+" tm = "+Fm3(tm));
		EXPECT_NEAR(startTime+d/speed,tm,0.001);                 // not as good
		int seg = kpc.getSegmentByDistance(d);
		double rdist = d - kpc.pathDistance(0,seg);
		double tmRel = kpc.timeFromDistanceWithinSeg(seg, rdist);
		//fpln(" $$$$ seg = "+seg+" d = "+Units::str("NM",d)+" tmRel = "+Fm3(tmRel)+" rdist = "+Units::str("NM",rdist));
		EXPECT_NEAR(rdist/speed,tmRel,0.001);
	}
}


TEST_F(PlanTest, test_timeFromDistanceWithinSeg) {
	Plan kpc("UF38");
	Position p0  = Position::makeLatLonAlt(37.435170000000, -122.174600000000, 127.000000000000);
	Position p1  = Position::makeLatLonAlt(37.435170000000, -122.174600000000, 137.791995660000);
	Position p2  = Position::makeLatLonAlt(37.435170000000, -122.174600000000, 177.000000000000);
	Position p3  = Position::makeLatLonAlt(37.433871720000, -122.169295640000, 440.352380060000);
	NavPoint np0(p0,1571943934.5930000000000000);    TcpData tcp0;
	tcp0.setBVS(0.980665000000);
	kpc.add(np0, tcp0);
	NavPoint np1(p1,1571943937.1830790000000000);     TcpData tcp1;
	tcp1.setEVS();        kpc.add(np1, tcp1);
	NavPoint np2(p2,1571943941.8880396000000000);     TcpData tcp2;
	tcp2.setBGS(0.980654812430);
	int ix2 = kpc.add(np2, tcp2);
	kpc.setInfo(ix2,"<BOC>");
	NavPoint np3(p3,1571943973.4922917000000000);     TcpData tcp3;
	tcp3.setEGS();        kpc.add(np3, tcp3);


	EXPECT_TRUE(kpc.isConsistent());
	double t0 = kpc.timeFromDistanceWithinSeg(2,2.0790375);
	EXPECT_NEAR(2.05913,t0,0.001);
	//f.pln(" $$$$$ test_timeFromDistanceWithinSeg: t0 = "+t0);
	double t1 = kpc.timeFromDistanceWithinSeg(0,1.0);
	EXPECT_NEAR(-1,t1,0.0001);
	double t2 = kpc.timeFromDistanceWithinSeg(0,0.0);
	EXPECT_NEAR(0.0,t2,0.0001);
	double t3 = kpc.timeFromDistanceWithinSeg(2,1.1111);
	EXPECT_NEAR(1.50532,t3,0.001);
	double t4 = kpc.timeFromDistanceWithinSeg(3,0.0);
	EXPECT_NEAR(-1,t4,0.0001);

}


TEST_F(PlanTest, test_mergeClosePoints) {

	Plan lpc("test_mergeClosePoints");
	Position p1(LatLonAlt::make(-0.60, 1.50, 23000));
	Position p2(LatLonAlt::make(-1.30, 1.70, 38000));
	Position p3(LatLonAlt::make(-1.70, 1.40, 25000));
	Position p4(LatLonAlt::make(-3.50, 0.50, 25000));
	Position p5(LatLonAlt::make(-3.500000001, 0.50000, 25000));
	double startTime = 1450;
	NavPoint np1(p1,startTime);       lpc.addNavPoint(np1);
	NavPoint np2(p2,1700);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1970);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,2710);    	 lpc.addNavPoint(np4);
	double extra = 1E-7;
	NavPoint np5(p5,2710+extra);    	 lpc.addNavPoint(np5);
	//fpln(" $$ BEFORE lpc = "+lpc.toStringGs());
	lpc.mergeClosePoints(extra*10.0);
	//fpln(" $$ AFTER lpc = "+lpc.toStringGs());
	EXPECT_EQ(4,lpc.size());
	//EXPECT_EQ("P4P5", lpc.point(3).name());
	Position p0(LatLonAlt::make(27.0, -100.0, 50000));
	double t0 = 0.0;
	Velocity v0 = Velocity::makeTrkGsVs(45.0,400,1000);
	double t1 = 10.0;
	p1 = p0.linear(v0,t1);
	double t2 = 70.0;
	v0 = Velocity::makeTrkGsVs(45.0,500,1000);
	p2 = p0.linear(v0,t2);
	Plan pln("");
	pln.addNavPoint(NavPoint(p0,t0));
	pln.addNavPoint(NavPoint(p1,t1));
	np2 = NavPoint(p2,t2);
	pln.addNavPoint(np2);
	//fpln(" $$ pln = "+pln.toStringGs());
	double gsAccel = 2.5;
	//bool repairGs = false;
	//bool useOffset = true;
	Plan kpc = TrajGen::generateGsTCPs(pln, gsAccel,true);
	double t3 = kpc.time(2)+1e-7;
	p3 = kpc.point(2).position().linear(v0,1e-7);
	np3 = NavPoint(p3,t3);
	//int linearIndex = 2;
	std::pair<NavPoint,TcpData> BGS = Plan::makeBGS(np3.name(), p3, t3, gsAccel);
	kpc.add(BGS);
	//fpln(" $$ kpc = "+kpc.toStringGs());
	kpc.mergeClosePoints(1e-6);
	//fpln(" $$ AFTER MERGE: kpc = "+kpc.toStringGs());

}




TEST_F(PlanTest, test_velocity_initialVelocity) {
	 Position p0  = Position(LatLonAlt::make(34.632770, -120.490995, 2181.000000));
	  Position p1  = Position(LatLonAlt::make(36.143467, -119.500660, 12181.000000));
	  Position p2  = Position(LatLonAlt::make(36.722625, -118.141749, 50340.000000));
	  Position p3  = Position(LatLonAlt::make(41.660013, -115.813159, 50340.000000));
	  Position p4  = Position(LatLonAlt::make(36.961200, -108.597124, 50340.000000));
	  Position p5  = Position(LatLonAlt::make(39.016931, -102.157233, 50340.000000));
	  Position p6  = Position(LatLonAlt::make(42.939601, -100.290294, 50340.000000));
	  Position p7  = Position(LatLonAlt::make(41.135592, -95.591481, 50340.000000));
	  NavPoint np0 = NavPoint(p0,0.000000);    	 lpc.addNavPoint(np0);
	  //EXPECT_TRUE(lpc.initialVelocity(0).isInvalid());
	  //EXPECT_TRUE(lpc.velocity(lpc.getFirstTime()).isInvalid());
	  EXPECT_FALSE(lpc.isWellFormed());
	  NavPoint np1 = NavPoint(p1,1233.271274);    	 lpc.addNavPoint(np1);
	  NavPoint np2 = NavPoint(p2,2124.069398);    	 lpc.addNavPoint(np2);
	  NavPoint np3 = NavPoint(p3,5908.607248);    	 lpc.addNavPoint(np3);
	  NavPoint np4 = NavPoint(p4,11159.274154);    	 lpc.addNavPoint(np4);
	  NavPoint np5 = NavPoint(p5,15100.787398);    	 lpc.addNavPoint(np5);
	  NavPoint np6 = NavPoint(p6,18101.668636);    	 lpc.addNavPoint(np6);
	  NavPoint np7 = NavPoint(p7,20929.616819);    	 lpc.addNavPoint(np7);
	  double bankAngle = Units::from("deg", 45);
	  //double distH = lpc.point(5).position().distanceH(lpc.point(5).position());
	  //f.pln(" $$$$$ distH = "+Units::str("ft",distH,15));
	  //DebugSupport.dumpPlan(lpc,"test_trkOut_lpc");
	  Plan kpc = TrajGen::generateTurnTCPs(lpc,bankAngle);
	  //fpln(" $$$ test_velocity_initialVelocity: kpc = "+kpc.toStringTrk());
	  //DebugSupport.dumpPlan(kpc,"test_trkOut_kpc");
	  //EXPECT_TRUE(kpc.isFlyable());
	  EXPECT_TRUE(kpc.isConsistent());
	  //EXPECT_TRUE(kpc.isVelocityContinuous(false));
	  //EXPECT_TRUE(kpc.isTrkContinuous(false));
	  //EXPECT_TRUE(kpc.isGsContinuous(false));
	  EXPECT_TRUE(kpc.isVsContinuous(4, Units::from("fpm", 130), false));  // does not pass stan 100
	  for (int k = 0; k < kpc.size(); k++) {
		  Velocity v_k = kpc.initialVelocity(k);
		  Velocity v_t = kpc.velocity(kpc.time(k));
		  //fpln(" $$$ k = "+k+" v_k = "+v_k+" v_t = "+v_t);
		  EXPECT_TRUE(v_k.almostEquals(v_t,PRECISION7));
	  }
}


TEST_F(PlanTest, test_vertexFromTurnTcps) {
	Plan lpc("");
	Position p0  = Position::mkXYZ(100.0, 100.0, 0.0);
	Position p1  = Position::mkXYZ(100.0, 200.0, 0.0);
	Position p2  = Position::mkXYZ(200.0, 300.0, 0.0);
	Position p3  = Position::mkXYZ(300.0, 300.0, 0.0);
	Position center = Position::mkXYZ(200.0, 200.0, 0.0);
	lpc.addNavPoint(NavPoint(p0,1.0));
	lpc.addNavPoint(NavPoint(p1,2.0));
	lpc.addNavPoint(NavPoint(p2,3.0));
	lpc.addNavPoint(NavPoint(p3,4.0));
	double sRadius = 100.0;
	lpc.setBOT(1,sRadius,center);
	//fpln(" $$ test_vertexFromTurnTcps: lpc = "+lpc.toStringGs());
	Position vertex = lpc.vertexFromTurnTcps(1, 2, 100.0);
	EXPECT_NEAR(100.0,vertex.x(),0.0000001);
	EXPECT_NEAR(300.0,vertex.y(),0.0000001);
	EXPECT_NEAR(100.0 ,vertex.z(),0.0000001);
	//fpln(" $$ test_vertexFromTurnTcps: vertex = "+vertex);
	//fpln("\n -------------------------------------------------------------------");
	lpc.clear();
	p0  = Position::makeLatLonAlt(20.0, 10.0, 0.0);
	p1  = Position::makeLatLonAlt(20.0, 15.0, 0.0);
	p2  = Position::makeLatLonAlt(25.0, 20.0, 0.0);
	p3  = Position::makeLatLonAlt(30.0, 20.0, 0.0);
	center = Position::makeLatLonAlt(25.0, 15.0, 0.0);
	lpc.addNavPoint(NavPoint(p0,10000.0));
	lpc.addNavPoint(NavPoint(p1,20000.0));
	lpc.addNavPoint(NavPoint(p2,30000.0));
	lpc.addNavPoint(NavPoint(p3,40000.0));
	sRadius = - p1.distanceH(center);
	//fpln(" $$ sRadius = "+Units::str("NM",sRadius));
	lpc.setBOT(1,sRadius,center);
	//fpln(" $$ test_vertexFromTurnTcps: lpc = "+lpc.toStringGs());
	vertex = lpc.vertexFromTurnTcps(1, 2, 0.0);
	//fpln(" $$ test_vertexFromTurnTcps: vertex = "+vertex);
	lpc.addNavPoint(NavPoint(center,70000.0));
	lpc.addNavPoint(NavPoint(vertex,80000.0));
	//DebugSupport.dumpPlan(lpc,"test_vertexFromTurnTcps");
	EXPECT_NEAR(19.9047,vertex.latitude(),0.001);
	EXPECT_NEAR(20.4107,vertex.longitude(),0.001);
	EXPECT_NEAR(0.0 ,vertex.alt(),0.0000001);
}


TEST_F(PlanTest, test_vertexFromTurnTcps_2) {
	Plan lpc("");
	Position p0  = Position::makeLatLonAlt(20.0, 10.0, 0.0);
	Position p3  = Position::makeLatLonAlt(30.0, 20.0, 0.0);
	Position vertex = Position::makeLatLonAlt(30.0, 10.0, 0.0);
	//fpln(" $$ test_vertexFromTurnTcps: vertex = "+vertex);
	lpc.addNavPoint(NavPoint(p0,10000.0));
	lpc.addNavPoint(NavPoint(vertex,20000.0));
	lpc.addNavPoint(NavPoint(p3,30000.0));
	//fpln(" lpc = "+lpc.toStringGs());
	double bankAngle = Units::from("deg",5);
	Plan kpc = TrajGen::generateTurnTCPs(lpc,bankAngle);
	//fpln(" $$ test_vertexFromTurnTcps: kpc = "+kpc.toStringFull());
	//DebugSupport.dumpPlan(kpc,"test_vertexFromTurnTcps_kpc");
	Position calcVertex = kpc.vertexFromTurnTcps(1, 3, 0.0);
	//fpln(" $$ test_vertexFromTurnTcps: calcVertex = "+calcVertex);
	kpc.addNavPoint(NavPoint(vertex,80000.0));
	//DebugSupport.dumpPlan(kpc,"test_vertexFromTurnTcps_kpc");
	EXPECT_NEAR(vertex.lat(),calcVertex.lat(),0.00000001);
	EXPECT_NEAR(vertex.lon(),calcVertex.lon(),0.00000001);
}

TEST_F(PlanTest, test_interpolateAlt) {
	Plan lpc("");
	Position p0  = Position::makeLatLonAlt(20.0, 10.0, 0.0);
	Position p3  = Position::makeLatLonAlt(30.0, 20.0, 1000.0);
	double t0 = 11050.0;
	NavPoint np0(p0,t0);    	    lpc.addNavPoint(np0);
	NavPoint np3(p3,t0+100.0);   lpc.addNavPoint(np3);
	bool linear = false;
	int ix = lpc.getSegment(t0+12);
	std::pair<double,double> altPair = lpc.interpolateAltVs(ix, 12 , linear);
	double newAlt = altPair.first;
	double newVs = altPair.second;
	//fpln(" newAlt = "+newAlt+" newVs = "+newVs);
	EXPECT_NEAR(36.576,newAlt,0.0001);
	EXPECT_NEAR(3.048,newVs,0.0001);
	fpln("-----------------------");
	Position p1  = Position::makeLatLonAlt(20.0, 10.0, 500.0);
	NavPoint np1(p1,t0+40.0);    	 lpc.addNavPoint(np1);
	//fpln(" $$$ lpc = "+lpc.toStringGs());
	//DebugSupport::dumpPlan(lpc,"test_interpolateAlt");
	altPair = lpc.interpolateAltVs(0, 12 , linear);
	newAlt = altPair.first;
	newVs = altPair.second;
	//fpln(" newAlt = "+newAlt+" newVs = "+newVs);
	EXPECT_NEAR(45.72,newAlt,0.0001);
	EXPECT_NEAR(3.81,newVs,0.0001);
}


TEST_F(PlanTest, test_interpolateAlt2) {
	double t0 = 110.0;
	Plan lpc("");
	Position p0(LatLonAlt::make(47.830, -122.114, 4000.0));
	NavPoint np0(p0,t0);    	 lpc.addNavPoint(np0);
	double d01 = Units::from("NM",100);
	Position p1 = p0.linearDist2D(M_PI/4,d01).mkAlt(5000);
	NavPoint np1(p1,t0+1000);    	 lpc.addNavPoint(np1);
	double d12 = Units::from("NM",150);
	Position p2 = p1.linearDist2D(M_PI/4,d12).mkAlt(5000);
	NavPoint np2(p2,t0+2000);    	 lpc.addNavPoint(np2);
	double d23 = Units::from("NM",50);
	Position p3 = p2.linearDist2D(M_PI/4,d23).mkAlt(10000);
	NavPoint np3(p3,t0+3000);    	 lpc.addNavPoint(np3);
	double vsAccel = 0.01;
	//fpln(" %%%%%%%%%% lpc = "+lpc.toStringGs());
	//bool continueGen = false;
	Plan kpc = TrajGen::generateVsTCPs(lpc,vsAccel);
	//fpln(" %%%%%%%%%% kpc = "+kpc);

	double tBVS1 = kpc.time(1);
	double tEVS1 = kpc.time(2);
	double tBVS2 = kpc.time(3);
	double tEVS2 = kpc.time(4);

	bool linear = false;
	double tMid = (tBVS1+tEVS1)/2.0;
	int segMid = kpc.getSegment(tMid);
	double dt = tMid-kpc.time(segMid);
	std::pair<double,double> altPair = kpc.interpolateAltVs(segMid, dt, linear);
	double newAlt = altPair.first;
	double newVs = altPair.second;
	EXPECT_NEAR(4821.3193,newAlt,0.0001);
	EXPECT_NEAR(Units::from("fpm",372),newVs,0.01);

	altPair = kpc.interpolateAltVs(segMid, dt, true);
	newAlt = altPair.first;
	newVs = altPair.second;
	EXPECT_NEAR((kpc.point(1).alt()+kpc.point(2).alt())/2.0, newAlt,0.0001);
	EXPECT_NEAR(1.890,newVs,0.01);

	tMid = (tEVS1+tBVS2)/2.0;
	segMid = kpc.getSegment(tMid);
	dt = tMid-kpc.time(segMid);
	altPair = kpc.interpolateAltVs(segMid, dt, linear);
	newAlt = altPair.first;
	newVs = altPair.second;
	EXPECT_NEAR((kpc.point(2).alt()+kpc.point(3).alt())/2.0, newAlt,0.0001);
	EXPECT_NEAR(kpc.vsOut(2),newVs,0.01);

	int ixBVS1 = 1;
	for  (double t1 = tBVS1+10; t1 < tEVS1; t1 = t1 + 30) {
		int seg1 = kpc.getSegment(t1);
		double dt1 = t1 - kpc.time(seg1);
		altPair = kpc.interpolateAltVs(seg1, dt1, linear);
		newAlt = altPair.first;
		newVs = altPair.second;
		std::pair<Position, Velocity> pv2 = kpc.positionVelocity(t1);
		kpc.add(pv2.first, 200000.0);
		//DebugSupport.dumpPlan(kpc,"test_interpolateAlt2_kpc");
		double vs0 = kpc.vsIn(ixBVS1);
		int dir = -1;
		double dt = t1-tBVS1;
		double newVsCalc = vs0 + dir*vsAccel*dt;
		double newAltCalc = kpc.point(ixBVS1).alt() + vs0*dt + 0.5*dir*vsAccel*dt*dt;
		EXPECT_NEAR(newAltCalc,newAlt,0.000001);
		EXPECT_NEAR(newVsCalc,newVs,0.000001);
	}

	int ixBVS2 = 3;
	for  (double t2 = tBVS2+10; t2 < tEVS2; t2 = t2 + 30) {
		int seg2 = kpc.getSegment(t2);
		double dt2 = t2 - kpc.time(seg2);
		altPair = kpc.interpolateAltVs(seg2, dt2, linear);
		newAlt = altPair.first;
		newVs = altPair.second;
		std::pair<Position, Velocity> pv2 = kpc.positionVelocity(t2);
		kpc.add(pv2.first,200000.0);
		//DebugSupport.dumpPlan(kpc,"test_interpolateAlt2_kpc");
		double vs0 = kpc.vsIn(ixBVS2);
		int dir = 1;
		double dt = t2-tBVS2;
		double newVsCalc = vs0 + dir*vsAccel*dt;
		double newAltCalc = kpc.point(ixBVS2).alt() + vs0*dt + 0.5*dir*vsAccel*dt*dt;
		EXPECT_NEAR(newAltCalc,newAlt,0.000001);
		EXPECT_NEAR(newVsCalc,newVs,0.000001);
	}

}


TEST_F(PlanTest, test_revertTurn) {
	Plan kpc;
	Position p0(LatLonAlt::make(33.151000, -96.591800, 0.000000));
	Position p1(LatLonAlt::make(33.140625, -96.597419, 351.814034));
	Position p2(LatLonAlt::make(33.113309, -96.605017, 1223.562800));
	Position p3(LatLonAlt::make(33.085642, -96.599533, 2086.150641));
	Position p4(LatLonAlt::make(33.084034, -96.598808, 2138.658151));
	Position p5(LatLonAlt::make(33.082271, -96.598013, 2195.924488));
	Position p6(LatLonAlt::make(32.864397, -96.500030, 9231.345960));
	Position p7(LatLonAlt::make(32.821948, -96.496678, 10535.366700));
	Position p8(LatLonAlt::make(32.785065, -96.521902, 11771.788899));
	NavPoint np0(p0,10990.000000);    	 kpc.addNavPoint(np0);
	//NavPoint src1 =  NavPoint::makeLatLonAlt(33.14062500, -96.59741900, 351.81403400, 10996.629850).makeName("WP_1");
	Position turnCenter1(LatLonAlt::make(33.110975, -96.519406, 351.814034));
	std::pair<NavPoint,TcpData> np1 = Plan::makeBOT(p1,10996.629850, -7972.522936, turnCenter1);
	kpc.add(np1);
	NavPoint np2(p2,11013.057745);    	 kpc.addNavPoint(np2);
	//NavPoint src3 =  NavPoint::makeLatLonAlt(33.08564200, -96.59953300, 2086.15064100, 11029.485640);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEOT( p3,11029.485640);
	kpc.add(np3);
	//NavPoint src4 =  NavPoint::makeLatLonAlt(33.08403400, -96.59880800, 2138.65815100, 11030.485640);
	std::pair<NavPoint,TcpData> np4 = Plan::makeBGS("", p4,11030.485640, 2.000000);
	kpc.add(np4);
	//NavPoint src5 =  NavPoint::makeLatLonAlt(33.08227100, -96.59801300, 2195.92448800, 11031.576271);
	std::pair<NavPoint,TcpData> np5 = Plan::makeEGS( p5,11031.576271);
	kpc.add(np5);
	//NavPoint src6 =  NavPoint::makeLatLonAlt(32.86439700, -96.50003000, 9231.34596000, 11165.565132).makeName("WP_6");
	Position turnCenter6(LatLonAlt::make(32.838494, -96.581776, 9231.345960));
	std::pair<NavPoint,TcpData> np6 = Plan::makeBOT(p6,11165.565132, 8155.724628, turnCenter6);
	kpc.add(np6);
	NavPoint np7(p7,11190.400070);    	 kpc.addNavPoint(np7);
	//NavPoint src8 =  NavPoint::makeLatLonAlt(32.78506500, -96.52190200, 11840.0, 11215.235008);
	std::pair<NavPoint,TcpData> np8 = Plan::makeEOT(p8,11215.235008);
	kpc.add(np8);
	NavPoint src9 =  NavPoint::makeLatLonAlt(32.774, -96.537, 12345.8, 11224.93);
	kpc.addNavPoint(src9);
	//fpln(" $$$$ BEFORE kpc = "+kpc);
	Plan saveKpc = kpc;
	int ixBOT = 6;
	int ixEOT = 8;
	//NavPoint vert = kpc.vertexPointTurnTcps(ixBOT, ixEOT,-1);
	//fpln(" ## vert = "+vert);
	//bool killNextGsTCPs = false;
	int ixMOT = kpc.findMOT(ixBOT);
	double altMOT = kpc.point(ixMOT).alt();
	//fpln(" $$$$$$$$$$ altMOT = "+Units::str("ft",altMOT));
	double gsInEOT =  kpc.gsIn(ixEOT);
	//double gsOutEOT = kpc.gsOut(ixEOT);
	EXPECT_EQ(7,ixMOT);
	//double zVertex = -1;
	//fpln("\n -----------------------------------");
	bool oldMethod = true;
	if (! oldMethod) {
		//DebugSupport.dumpPlan(kpc,"test_revertTurn_BEFORE");
		kpc.revertTurnTCP(ixBOT); //, killNextGsTCPs);
		//DebugSupport.dumpPlan(kpc,"test_revertTurn_AFTER");
		//fpln(" $$$$ AFTER kpc = "+kpc.toStringGs());
		int revVert = 6;
		NavPoint revertedVertex = kpc.point(revVert);
		EXPECT_NEAR(Units::from("deg",32.818),revertedVertex.lat(),0.0001);
		EXPECT_NEAR(Units::from("deg",-96.479),revertedVertex.lon(),0.0001);
		EXPECT_NEAR(altMOT,revertedVertex.alt(),0.01);
		EXPECT_NEAR(gsInEOT,kpc.gsOut(revVert),0.2);
		//fpln(" vert = "+vert);

		fpln("\n -------------------------------- 1 internal point ------------------------- ");
		kpc = saveKpc;
		double tmBOT = kpc.point(ixBOT).time();
		double tmExtra = tmBOT + 10.0;
		NavPoint npExtra = NavPoint(kpc.position(tmExtra),tmExtra);
		npExtra = npExtra.makeName("EXTRA");
		kpc.addNavPoint(npExtra);
		//DebugSupport.dumpPlan(kpc,"test_revertTurn_BEFORE");
		kpc.revertTurnTCP(ixBOT); // , killNextGsTCPs);
		//DebugSupport.dumpPlan(kpc,"test_revertTurn_AFTER");
		revVert = 7;
		revertedVertex = kpc.point(revVert);
		EXPECT_NEAR(Units::from("deg",32.806),revertedVertex.lat(),0.001);
		EXPECT_NEAR(Units::from("deg",-96.440),revertedVertex.lon(),0.001);
		EXPECT_NEAR(altMOT,revertedVertex.alt(),0.01);
		EXPECT_NEAR(gsInEOT,kpc.gsOut(revVert),0.2);
		int ixExtra = 6;
		NavPoint extraNp = kpc.point(ixExtra);
		EXPECT_NEAR(Units::from("deg",32.848),extraNp.lat(),0.001);
		EXPECT_NEAR(Units::from("deg",-96.495),extraNp.lon(),0.001);
		EXPECT_NEAR(npExtra.alt(),extraNp.alt(),0.000001);

		fpln("\n-------------------------------- 2 internal points -------------------------");
		kpc = saveKpc;
		//tmBOT = kpc.point(ixBOT).time();
		tmExtra = tmBOT + 10.0;
		npExtra = NavPoint(kpc.position(tmExtra),tmExtra).makeName("EXTRA");
		kpc.addNavPoint(npExtra);
		double tmExtra2 = tmBOT + 40.0;
		NavPoint npExtra2 = NavPoint(kpc.position(tmExtra2),tmExtra2).makeName("EXTRA2");
		kpc.addNavPoint(npExtra2);
		//DebugSupport.dumpPlan(kpc,"test_revertTurn_BEFORE");
		kpc.revertTurnTCP(ixBOT); // , killNextGsTCPs);
		//DebugSupport.dumpPlan(kpc,"test_revertTurn_AFTER");
		revVert = 7;
		revertedVertex = kpc.point(revVert);
		//fpln(" ## kpc = "+kpc);
		EXPECT_NEAR(Units::from("deg",32.806),revertedVertex.lat(),0.001);
		EXPECT_NEAR(Units::from("deg",-96.440),revertedVertex.lon(),0.001);
		EXPECT_NEAR(altMOT,revertedVertex.alt(),0.01);
		int ixExtra2 = 8;
		NavPoint extraNp2 = kpc.point(ixExtra2);
		EXPECT_NEAR(Units::from("deg",32.848),extraNp2.lat(),0.001);
		EXPECT_NEAR(Units::from("deg",-96.495),extraNp2.lon(),0.001);
		EXPECT_NEAR(npExtra2.alt(),extraNp2.alt(),0.01);
	}
}



TEST_F(PlanTest, test_revertTurn2) {
	Position p0(LatLonAlt::make(47.324, -122.215, 0.0));
	Position p1(LatLonAlt::make(47.386, -122.202, 4000.0));
	Position p2(LatLonAlt::make(47.434, -122.229, 5000.0));
	NavPoint np0(p0,0.000000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,36.56);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,66.54);    	 lpc.addNavPoint(np2);
	//DebugSupport.dumpPlan(lpc,"test_revertTurn2_lpc");
	Plan kpc = TrajGen::generateTurnTCPs(lpc, Units::from("deg",12));
	//fpln(" ## test_revertTurn2: kpc = "+kpc);
	Plan saveKpc = kpc;
	//DebugSupport.dumpPlan(kpc,"test_revertTurn2_BEFORE");
	int ixBOT = kpc.nextBOT(0);
	//bool killNextGsTCPs = false;
	int ixMOT = 2; // kpc.findNextMOT(0);
	EXPECT_EQ(2,ixMOT);
	double altMOT = kpc.point(ixMOT).alt();
	kpc.revertTurnTCP(ixBOT); // , killNextGsTCPs);
	//DebugSupport.dumpPlan(kpc,"test_revertTurn2_AFTER");
	NavPoint vertex = kpc.point(1);
	EXPECT_NEAR(Units::from("deg",47.386),vertex.lat(),0.001);
	EXPECT_NEAR(Units::from("deg",-122.202),vertex.lon(),0.001);
	EXPECT_NEAR(altMOT,vertex.alt(),0.001);

	kpc = saveKpc;
	double tmBOT = kpc.point(ixBOT).time();
	double tmExtra = tmBOT + 10.0;
	NavPoint npExtra = NavPoint(kpc.position(tmExtra),tmExtra).makeName("EXTRA");
	kpc.addNavPoint(npExtra);
	tmExtra = tmBOT + 25.0;
	npExtra = NavPoint(kpc.position(tmExtra),tmExtra).makeName("EXTRA");
	kpc.addNavPoint(npExtra);
	tmExtra = tmBOT + 29.0;
	npExtra = NavPoint(kpc.position(tmExtra),tmExtra).makeName("EXTRA");
	kpc.addNavPoint(npExtra);
	kpc.mkGsIn(3,Units::from("kn",400));
	kpc.mkGsIn(4,Units::from("kn",420));
	kpc.mkGsIn(5,Units::from("kn",440));
	kpc.mkGsIn(6,Units::from("kn",410));
	//DebugSupport.dumpPlan(kpc,"test_revertTurn2_BEFORE");

	kpc.revertTurnTCP(ixBOT); // , killNextGsTCPs);
	//DebugSupport.dumpPlan(kpc,"test_revertTurn2_AFTER");
	vertex = kpc.point(2);
	EXPECT_NEAR(Units::from("deg",47.386),vertex.lat(),0.001);
	EXPECT_NEAR(Units::from("deg",-122.202),vertex.lon(),0.001);
	EXPECT_NEAR(altMOT,vertex.alt(),0.001);
	EXPECT_NEAR(Units::from("kn",370),kpc.gsIn(1),0.05);
	bool oldMethod = true;
	if (! oldMethod) {
		EXPECT_NEAR(Units::from("kn",400),kpc.gsOut(1),0.05);
		EXPECT_NEAR(Units::from("kn",400),kpc.gsOut(2),0.05);
		EXPECT_NEAR(Units::from("kn",440),kpc.gsOut(3),0.05);
		EXPECT_NEAR(Units::from("kn",410),kpc.gsOut(4),0.05);
		EXPECT_NEAR(Units::from("kn",370),kpc.gsOut(5),0.05);
	}
}


TEST_F(PlanTest, positionVelocityAlt) {
	Plan lpc;
	Position p0(LatLonAlt::make(47.324200, -122.215200, 0.000000));
	Position p1(LatLonAlt::make(47.386000, -122.202900, 4529.633600));
	Position p2(LatLonAlt::make(47.398800, -122.200400, 5000.000000));
	Position p3(LatLonAlt::make(47.817900, -122.116600, 5000.000000));
	Position p4(LatLonAlt::make(47.830700, -122.114000, 4529.633600));
	Position p5(LatLonAlt::make(47.892400, -122.101500, 0.000000));
	NavPoint np0 = NavPoint(p0,11050.000000);
	lpc.addNavPoint(np0);
	//NavPoint src1 =  NavPoint::makeLatLonAlt(47.38600000, -122.20290000, 4529.63360000, 11131.533700);
	std::pair<NavPoint,TcpData> np1 = Plan::makeBVS("", p1,11131.533700, -1.000000);
	lpc.add(np1);
	//NavPoint src2 =  NavPoint::makeLatLonAlt(47.39880000, -122.20040000, 5000.00000000, 11148.467000);
	std::pair<NavPoint,TcpData> np2 = Plan::makeEVS(p2,11148.467000);
	lpc.add(np2);
	//NavPoint src3 =  NavPoint::makeLatLonAlt(47.81790000, -122.11660000, 5000.00000000, 11702.083000);
	std::pair<NavPoint,TcpData> np3 = Plan::makeBVS("", p3,11702.083000, -1.000000);
	lpc.add(np3);
	//NavPoint src4 =  NavPoint::makeLatLonAlt(47.83070000, -122.11400000, 4529.63360000, 11719.016300);
	std::pair<NavPoint,TcpData> np4 = Plan::makeEVS(p4,11719.016300);
	lpc.add(np4);
	NavPoint np5(p5,11800.550000);    	 lpc.addNavPoint(np5);
	//DebugSupport.dumpPlan(lpc,"positionVelocityAlt");
	//fpln(" $$$ lpc = "+lpc.toStringVs());
	double t = 11702.083;
	bool linear = true;
	std::pair<Position, Velocity> pv = lpc.positionVelocity(t,linear);
	EXPECT_NEAR(p3.latitude(),pv.first.latitude(),0.0001);
	EXPECT_NEAR(p3.longitude(),pv.first.longitude(),0.0001);
	EXPECT_NEAR(p3.altitude(),pv.first.altitude(),0.0001);
	EXPECT_NEAR(0.1355,pv.second.trk(),0.0001);
	EXPECT_NEAR(84.7739,pv.second.gs(),0.0001);
	EXPECT_NEAR(-8.466,pv.second.vs(),0.001);
	NavPoint npInter(pv.first,t);
	lpc.addNavPoint(npInter);
	//DebugSupport.dumpPlan(lpc,"positionVelocityAlt_after");
	double pathDist = lpc.pathDistance();
	double step = pathDist/50.0;
	for (double d = 0; d <= pathDist; d = d + step) {
		for (int i = 0; i < 5; i++) {
			//fpln(" $$ positionVelocityAlt: i = "+i+" d = "+d);
			Position posAtD = lpc.advanceDistance(i,d,false).first;
			double tmAtD = lpc.timeFromDistance(i,d);
			double distToI = lpc.pathDistance(0,i);
			if (d+distToI <= pathDist) {
				EXPECT_TRUE(tmAtD >= 0);
				Position posAtD_2 = lpc.position(tmAtD);
				//fpln(" $$ positionVelocityAlt: tmAtD = "+tmAtD+" posAtD = "+posAtD+" posAtD_2 = "+posAtD_2);
				EXPECT_TRUE(posAtD.almostEquals(posAtD_2));
			} else {
				EXPECT_TRUE(tmAtD < 0);
			}
		}
	}
}

TEST_F(PlanTest, test_repairPlan) {
	Plan kpc = Plan("ac_12011_KTMB_501");
	Position p0  = Position::makeLatLonAlt(25.757400000000, -80.3962000000000200, 6999.999999999999);
	Position p1  = Position::makeLatLonAlt(26.709300000000, -80.2375000000000100, 6999.999999999999);
	//NavPoint src0 =  NavPoint::makeLatLonAlt(25.75740000, -80.39620000,  7000.00000000, 12145.102600).makeName("");
	std::pair<NavPoint,TcpData> np0 = Plan::makeEVS(p0,12145.102600);
	kpc.add(np0);
	//NavPoint src1 =  NavPoint::makeLatLonAlt(26.70930000, -80.23750000,  7000.00000000, 13250.937400).makeName("");
	std::pair<NavPoint,TcpData> np1 = Plan::makeBVS("", p1,13250.937400, -2.000000);
	kpc.add(np1);
	//f.pln(" $$ kpc = "+kpc.toStringGs());
	EXPECT_FALSE(kpc.isWellFormed());
	//Plan::makeWellFormedEnds(kpc);
	kpc.repairPlan();
	EXPECT_TRUE(kpc.isConsistent(false));
	//kpc.revertVsTCPs();
}

TEST_F(PlanTest, test_repairPlan_2) {
	Position p0 = Position::makeXYZ(0,0,0);
	Position p1 = Position::makeXYZ(5,0,0);
	NavPoint np0(p0,0.0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1.0);    	 lpc.addNavPoint(np1);
	lpc.setEGS(1);
	EXPECT_FALSE(lpc.isWellFormed());
	//f.pln(" $$ lpc = "+lpc.toStringGs());
	lpc.repairPlan();
	EXPECT_TRUE(lpc.isWellFormed());
	//f.pln(" $$ AFTER lpc = "+lpc.toStringGs());
	lpc.setEVS(1);
	lpc.setEOT(1);
	EXPECT_FALSE(lpc.isWellFormed());
	//f.pln(" $$ lpc = "+lpc.toStringGs());
	lpc.repairPlan();
	EXPECT_TRUE(lpc.isWellFormed());
	//f.pln(" $$ AFTER lpc = "+lpc.toStringGs());
}


TEST_F(PlanTest, testInsertByDistance2) {    // came from TrajGenTest.test_T094
	int ixNp2 = 1;
	double distBotToVertex = Units::from("NM",0.740481);
	Plan lpc("noname");
	Position p0(LatLonAlt::make(36.14346700, -119.50066000, 12181.00000000));
	Position p1(LatLonAlt::make(36.72262500, -118.14174900, 50340.00000000));
	Position p2(LatLonAlt::make(41.66001300, -115.81315900, 50340.00000000));
	NavPoint np0(p0,1233.271274);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,2124.069398);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,5908.607248);    	 lpc.addNavPoint(np2);
	int ixNew = lpc.insertByDistance(ixNp2,-distBotToVertex);
	//fpln(" $$$$ lpc = "+lpc.toStringVs());
	EXPECT_TRUE(lpc.isGsContinuous(true));
	EXPECT_NEAR(0.6408,lpc.point(ixNew).lat(),0.001);
	EXPECT_NEAR(-2.0622,lpc.point(ixNew).lon(),0.001);
	EXPECT_NEAR(15227.613,lpc.point(ixNew).alt(),0.001);   // 15233.1475 ??
	EXPECT_NEAR(2115.183,lpc.point(ixNew).time(),0.001);
}



TEST_F(PlanTest, testHoverProfile) {
	Plan lpc("noname");
	Position p0(LatLonAlt::make(36.722, -118.1417, 12181.0));
	Position p1(LatLonAlt::make(36.722, -118.1417, 50340.0));
	Position p2(LatLonAlt::make(36.722, -118.1417, 50340.0));
	Position p3(LatLonAlt::make(41.660, -115.8131, 50340.0));
	NavPoint np0(p0,1233);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,2124);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,5908);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,6908);    	 lpc.addNavPoint(np3);
	//DebugSupport.dumpPlan(lpc,"testHoverProfile_lpc");
	//fpln(" $$$ lpc = "+lpc.toStringTrk());
	//EXPECT_TRUE(ISNAN(lpc.gsIn(0)));  // undefined
	EXPECT_NEAR(0.0,lpc.gsIn(1),0.00001);
	EXPECT_NEAR(0.0,lpc.gsIn(2),0.00001);
	EXPECT_NEAR(Units::from("kn",1135.485),lpc.gsIn(3),0.001);
	EXPECT_NEAR(0.0,lpc.gsOut(0),0.00001);  // undefined
	EXPECT_NEAR(0.0,lpc.gsOut(1),0.00001);
	EXPECT_NEAR(Units::from("kn",1135.485),lpc.gsOut(2),0.001);
	EXPECT_NEAR(Units::from("kn",1135.485),lpc.gsOut(3),0.001);
	EXPECT_TRUE(lpc.isTrkContinuous(false));                       // because ignore gs 0 regions
	EXPECT_FALSE(lpc.isGsContinuous(false));
	EXPECT_FALSE(lpc.isVsContinuous(false));

	//EXPECT_NEAR(-1,lpc.vsIn(0),0.00001);  // undefined
	EXPECT_NEAR(Units::from("fpm",2569.629),lpc.vsIn(1),0.00001);
	EXPECT_NEAR(0.0,lpc.vsIn(2),0.00001);
	EXPECT_NEAR(0.0,lpc.vsIn(3),0.001);
	EXPECT_NEAR(Units::from("fpm",2569.629),lpc.vsOut(0),0.00001);
	EXPECT_NEAR(0.0,lpc.vsOut(1),0.00001);
	EXPECT_NEAR(0.0,lpc.vsOut(2),0.001);
	EXPECT_NEAR(0.0,lpc.vsOut(3),0.001);

	//EXPECT_NEAR(-1,lpc.trkIn(0),0.00001);  // undefined
	EXPECT_NEAR(M_PI,lpc.trkIn(1),0.00001);    // What should this be?
	EXPECT_NEAR(M_PI,lpc.trkIn(2),0.00001);    // What should this be?
	EXPECT_NEAR(Units::from("deg",20.8214),lpc.trkIn(3),0.001);
	EXPECT_NEAR(0.0,lpc.trkOut(0),0.00001);       // What should this be?
	EXPECT_NEAR(0.0,lpc.trkOut(1),0.00001);   // What should this be?
	EXPECT_NEAR(Units::from("deg",19.3484),lpc.trkOut(2),0.001);
	EXPECT_NEAR(Units::from("deg",20.8214),lpc.trkOut(3),0.001);
}



TEST_F(PlanTest, test_nonCont) {
	Plan kpc("UF18");
	Position p0  = Position::makeXYZ(75.43900000, -6.44600000, 5000.00000000);
	Position p1  = Position::makeXYZ(85.24043128, -6.03017479, 5000.00000000);
	Position p2  = Position::makeXYZ(87.72478351, -6.26869091, 5000.00000000);
	Position p3  = Position::makeXYZ(89.91849607, -5.07851398, 5000.00000000);
	Position p4  = Position::makeXYZ(101.22233910, -4.05844284, 5000.00000000);
	NavPoint np0(p0,1337.120000);    	 kpc.addNavPoint(np0);
	//NavPoint src1 =  NavPoint::makeLatLonAlt(87.90000000, -7.13000000, 5000.00000000, 1528.710021).makeName("");
	Position turnCenter1 = Position::makeXYZ(86.87310201, -2.08209290, 5000.00000000);
	std::pair<NavPoint,TcpData> np1 = Plan::makeBOT( p1,1500.689107, -7912.389978, turnCenter1);
	kpc.add(np1);
	NavPoint np2(p2,1525.348027);    	 kpc.addNavPoint(np2);
	//NavPoint src3 =  NavPoint::makeLatLonAlt(87.90000000, -7.13000000, 5000.00000000, 1528.710021).makeName("");
	std::pair<NavPoint,TcpData> np3 = Plan::makeEOT( p3,1550.006947);
	kpc.add(np3);
	NavPoint np4(p4,1751.317913);    	 kpc.addNavPoint(np4);
	//fpln(" ## test_nonCont: kpc = "+kpc.toStringVs());
	//DebugSupport.dumpPlan(kpc,"test_nonCont");
	EXPECT_TRUE(kpc.isConsistent(true));
	EXPECT_FALSE(kpc.isVelocityContinuous(false));
}



TEST_F(PlanTest, positionWithAdd) {    // came from TrajGenTest.test_T094
	 Position p0(LatLonAlt::make(2.152600, 5.247200, 7916.425100));
	  Position p1(LatLonAlt::make(0.972093, 6.162997, 7916.425100));
	  Position p2(LatLonAlt::make(0.941024, 6.180137, 7879.439502));
	  Position p3(LatLonAlt::make(0.938982, 6.180879, 7874.928951));
	  Position p4(LatLonAlt::make(0.918431, 6.188352, 7824.933539));
	  Position p5(LatLonAlt::make(-1.846300, 7.193700, 342.343300));
	  Position p6(LatLonAlt::make(-1.860463, 7.199056, 451.232164));
	  Position p7(LatLonAlt::make(-4.032389, 8.021859, 15997.146387));
	  Position p8(LatLonAlt::make(-4.053300, 8.029800, 16144.550500));
	  Position p9(LatLonAlt::make(-4.069038, 8.045725, 16201.998730));
	  Position p10(LatLonAlt::make(-5.828700, 9.833100, 22688.465900));
	  NavPoint np0(p0,0.000000);    	 lpc.addNavPoint(np0);
	  //NavPoint src1 =  NavPoint::makeLatLonAlt(0.95790000, 6.17400000, 7916.42510000, 695.943700);
	  Position turnCenter1(LatLonAlt::make(0.901839, 6.072355, 7916.425100));
	  std::pair<NavPoint,TcpData> np1 = Plan::makeBOT( p1,687.677314, 12742.179325, turnCenter1);
	    	 lpc.add(np1);
	  //NavPoint src2 =  NavPoint::makeLatLonAlt(0.95790000, 6.17400000, 7916.42510000, 695.943700);
	  std::pair<NavPoint,TcpData> np2 = Plan::makeEOT(p2,704.076904);
	    	 lpc.add(np2);
	  //NavPoint src3 =  NavPoint::makeLatLonAlt(0.95790000, 6.17400000, 7916.42510000, 695.943700);
	  std::pair<NavPoint,TcpData> np3 = Plan::makeBGS("", p3,705.076904, -4.000000);
	    	 lpc.add(np3);
	  //NavPoint src4 =  NavPoint::makeLatLonAlt(0.95790000, 6.17400000, 7916.42510000, 695.943700);
	  std::pair<NavPoint,TcpData> np4 = Plan::makeEGS(p4,716.161006);
	    	 lpc.add(np4);
	  //NavPoint src5 =  NavPoint::makeLatLonAlt(-1.84630000, 7.19370000, 342.34330000, 2378.534100);
	  std::pair<NavPoint,TcpData> np5 = Plan::makeBGS("", p5,2375.069062, 4.000000);
	    	 lpc.add(np5);
	  //NavPoint src6 =  NavPoint::makeLatLonAlt(-1.84630000, 7.19370000, 342.34330000, 2378.534100);
	  std::pair<NavPoint,TcpData> np6 = Plan::makeEGS(p6,2382.972998);
	    	 lpc.add(np6);
	  //NavPoint src7 =  NavPoint::makeLatLonAlt(-4.05330000, 8.02980000, 16144.55050000, 3525.191700);
	  Position turnCenter7(LatLonAlt::make(-3.995933, 8.118325, 16144.550500));
	  std::pair<NavPoint,TcpData> np7 = Plan::makeBOT( p7,3511.407081, -11434.598837, turnCenter7);
	    	 lpc.add(np7);
	  NavPoint np8(p8,3522.106730);    	 lpc.addNavPoint(np8);
	  //NavPoint src9 =  NavPoint::makeLatLonAlt(-4.05330000, 8.02980000, 16144.55050000, 3525.191700);
	  std::pair<NavPoint,TcpData> np9 = Plan::makeEOT(p9,3532.806379);
	    	 lpc.add(np9);
	  NavPoint np10(p10,4740.901423);    	 lpc.addNavPoint(np10);
	  //fpln(" BEFORE lpc = "+lpc.toStringGs());
	  double tbegin = 695.19;
	  double tend = 696.56;
	  //int i = 2;
	  //NavPoint np2Source = lpc.sourceNavPoint(i);
	  double accel = 1;
	  std::pair<NavPoint,TcpData> bPair = Plan::makeBVS("", lpc.position(tbegin), tbegin, accel);
	  NavPoint b = bPair.first; // .makeAdded();//.appendName(setName);
	  std::pair<NavPoint,TcpData> ePair = Plan::makeEVS(lpc.position(tend), tend);
	  NavPoint e = ePair.first;
}



TEST_F(PlanTest, test_fixGs_continuity) {
	Plan lpc("UF2303");
	Position p0  =Position::makeLatLonAlt(32.763598000000, -97.2653960000000000, 600.000000000000);
	Position p1  =Position::makeLatLonAlt(32.763598000000, -97.2653960000000000, 650.000000000000);
	Position p2  =Position::makeLatLonAlt(32.764508568970, -97.2623190431280000, 696.755350481078);
	Position p3  =Position::makeLatLonAlt(32.789859219570, -97.1765275463450000, 1999.999999999628);
	Position p4  =Position::makeLatLonAlt(32.789859219570, -97.1765275463450000, 2000.000000000000);
	NavPoint np0(p0,46932.7260119239100000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,46938.7260119239100000);    	 lpc.addNavPoint(np1);
	lpc.setInfo(1,"BOC");
	NavPoint np2(p2,46944.3366539816340000);    	 lpc.addNavPoint(np2);
	lpc.setInfo(2,"depFix");
	NavPoint np3(p3,47100.7260119238600000);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,47100.7260119239100000);    	 lpc.addNavPoint(np4);
	//fpln(" $$$ AFTER lpc = "+lpc.toStringGs());
	EXPECT_FALSE(lpc.isConsistent(false));
	lpc.mergeClosePoints();
	EXPECT_TRUE(lpc.isConsistent(false));

	EXPECT_FALSE(lpc.isVelocityContinuous());
	double bankAngle = .2;
	double gsAccel = 5.0;
	double vsAccel = 2.0;
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle,gsAccel,vsAccel);
	//fpln(" $$$ kpc = "+kpc.toStringGs());
	EXPECT_TRUE(lpc.isConsistent(false));
	double vo = Units::from("kn",210);
	double vf = Units::from("kn",190);
	lpc.repairGsContinuity(0, 1,  vo,  vf, 1E10);
}


TEST_F(PlanTest, test_fixGs_continuity_2) {
	Plan lpc("GS");
	Position p0  = Position::makeLatLonAlt(32.763, -47.2653, 6000.00);
	Position p1  = p0.linearDist2D(M_PI/6,10000);
	NavPoint np0(p0,0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,100);    lpc.addNavPoint(np1);
	double gs200 = Units::from("kn",200);
	double t1 = lpc.calcDtGsIn(1, gs200);
	lpc.mkGsIn(1,gs200);
	//fpln(" $$$ lpc = "+lpc.toStringGs());
	double a_gs = 2.18;
	lpc.clear();
	std::pair<NavPoint, TcpData> bgs = Plan::makeBGS("",p0,0.0,a_gs);
	std::pair<NavPoint, TcpData> egs = Plan::makeEGS(p1,t1);
	lpc.add(bgs);
	lpc.add(egs);
	double gs300 = Units::from("kn",300);
	lpc.mkGsOut(0,gs300);
	//fpln(" $$$ t1 = "+t1+" lpc = "+lpc.toStringGs());
	double vo = Units::from("kn",340);
	double vf = Units::from("kn",650);
	lpc.repairGsContinuity(0, 1,  vo,  vf, 1E10);
	//fpln(" $$$ AFTER lpc = "+lpc.toStringGs());
	EXPECT_NEAR(vo,lpc.gsOut(0),0.0001);
	EXPECT_NEAR(vf,lpc.gsIn(1),0.0001);
	// add some middle points
	Position mid = lpc.position(10.0);
	NavPoint npMid(mid,10.0);    	 lpc.addNavPoint(npMid);
	EXPECT_TRUE(lpc.isFlyable());
	lpc.mkGsIn(1,2*gs200);
	//fpln(" $$$ AFTER MID lpc = "+lpc.toStringGs());
	//EXPECT_FALSE(lpc.isConsistent());
	int ixBGS = 0;
	int ixEGS = lpc.nextEGS(ixBGS);
	lpc.repairGsContinuity(ixBGS, ixEGS,  vo,  vf, 1E10);
	//fpln(" $$$ AFTER MID FIX lpc = "+lpc.toStringGs());
	EXPECT_TRUE(lpc.isConsistent());
	EXPECT_TRUE(lpc.isVelocityContinuous(false));
}



TEST_F(PlanTest, test_fixGs_continuity_3) {
	Position p0  = Position::makeLatLonAlt(32.7, -97.2, 6000.0);
	Position p1  = p0.linearDist2D(M_PI/3,3000);
	Position p2  = p1.linearDist2D(M_PI/3,6000);
	Position p3  = p2.linearDist2D(M_PI/3,8000);
	Position p4  = p3.linearDist2D(M_PI/3,7000);
	Position p5  = p4.linearDist2D(M_PI/3,11000);
	NavPoint np0(p0,0);    	lpc.addNavPoint(np0);
	NavPoint np1(p1,100);    lpc.addNavPoint(np1);
	NavPoint np2(p2,200);    lpc.addNavPoint(np2);
	NavPoint np3(p3,300);    lpc.addNavPoint(np3);
	NavPoint np4(p4,400);    lpc.addNavPoint(np4);
	NavPoint np5(p5,400);    lpc.addNavPoint(np5);
	fpln(" $$$ lpc = "+lpc.toStringGs());
	fpln(" $$$ lpc = "+lpc.toStringProfile());
	double bankAngle = Units::from("deg",19);
	double gsAccel = 1.0;
	double vsAccel = 2.0;
	Plan kpc = TrajGen::makeKinematicPlan(lpc,bankAngle,gsAccel,vsAccel);
	fpln(" $$$ kpc = "+kpc.toStringGs());
	fpln(" $$$ kpc = "+kpc.toStringProfile());


}




TEST_F(PlanTest, test_fixGs_Cont) {
	Plan lpc("bleh");
	Position p0(LatLonAlt::make(44.1626, 18.9911, 10000));
	Position p1(LatLonAlt::make(44.2067, 19.0753, 10000));
	Position p2(LatLonAlt::make(44.3007, 19.0853, 10000));
	Position p3(LatLonAlt::make(44.3307, 19.1111, 10000));
	NavPoint np0(p0, 0);
	NavPoint np1(p1, 100);
	NavPoint np2(p2, 200);
	NavPoint np3(p3, 300);
	lpc.addNavPoint(np0);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	NavPoint np4 = lpc.navPt(107);
	lpc.addNavPoint(np4);
	//fpln(" $$$$$ lpc = "+lpc.toStringProfile());
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 20);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true,true,true);
	//fpln(" $$$$$ kpc = "+kpc.toStringProfile());
	EXPECT_TRUE(kpc.isVelocityContinuous(false));
	Plan saveP = kpc.copy();
	for (double nt = 100.0; nt < 108.0; nt = nt + 0.1) {
		Plan p = saveP.copy();
		p.setTime(3,nt);
		//fpln(" $$$$$ AT nt = "+nt+" p = "+p.toStringGs());
		//EXPECT_FALSE(p.isVelocityContinuous(false));
		//fpln(" $$$ nt = "+nt+" consistent = "+p.isGsConsistent(true)+" continuous = "+p.isVelocityContinuous(false));
		p.repairGsContinuity(10.0);
		EXPECT_TRUE(p.isGsConsistent(false));
		EXPECT_TRUE(p.isVelocityContinuous(false));
	}
}



TEST_F(PlanTest, test_fixGs_Cont_2) {
	Plan kpc("cont2");
	Position p0  = Position::makeLatLonAlt(42.6325, -71.0786, 10056.3);
	Position p1  = Position::makeLatLonAlt(42.6898, -71.0834, 10225.5);
	Position p2  = Position::makeLatLonAlt(42.7217, -71.2432, 12979.8);
	Position p3  = Position::makeLatLonAlt(42.7474, -71.3947, 12999.5);
	Position p4  = Position::makeLatLonAlt(42.7531, -71.5861, 13018.3);
	Position p5  = Position::makeLatLonAlt(42.7685, -71.7907, 13693.5);
	NavPoint np0(p0, 0);      kpc.addNavPoint(np0);
	std::pair<NavPoint,TcpData> np1 = Plan::makeBGS("", p1, 100, 1.5);  	    int ix1 = kpc.add(np1);
	std::pair<NavPoint,TcpData> np2 = Plan::makeEGSBGS("", p2, 200, 1.5);  	int ix2 = kpc.add(np2);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEGSBGS("", p3, 300, 1.5);   	int ix3 = kpc.add(np3);
	std::pair<NavPoint,TcpData> np4 = Plan::makeEGS(p4, 400);
	NavPoint np5(p5, 500);    kpc.addNavPoint(np5);
	//fpln(" $$$$$ BEFORE kpc = "+kpc.toStringGs());
	EXPECT_TRUE(kpc.isGsConsistent(false));
	EXPECT_FALSE(kpc.isGsContinuous(false));
	kpc.repairGsContinuity(10.0);
	//fpln(" $$$$$ AFTER kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"test_fixGs_Cont_2");
	EXPECT_TRUE(kpc.isGsConsistent(false));
	EXPECT_TRUE(kpc.isGsContinuous(false));
}


TEST_F(PlanTest, test_fixGs_Cont_3) {
	Plan kpc("cont2");
	//Position p0  = Position::makeLatLonAlt(42.6325, -71.0786, 10056.3);
	Position p1  = Position::makeLatLonAlt(42.6898, -71.0834, 10225.5);
	Position p2  = Position::makeLatLonAlt(42.7217, -71.2432, 12979.8);
	Position p3  = Position::makeLatLonAlt(42.7474, -71.3947, 12999.5);
	Position p4  = Position::makeLatLonAlt(42.7531, -71.5861, 13018.3);
	//Position p5  = Position::makeLatLonAlt(42.7685, -71.7907, 13693.5);
	//NavPoint np0(p0, 0);      kpc.addNavPoint(np0);
	std::pair<NavPoint,TcpData> np1 = Plan::makeBGS("", p1, 100, 1.5);  	    int ix1 = kpc.add(np1);
	std::pair<NavPoint,TcpData> np2 = Plan::makeEGSBGS("", p2, 200, 1.5);  	int ix2 = kpc.add(np2);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEGSBGS("", p3, 300, 1.5);   	int ix3 = kpc.add(np3);
	std::pair<NavPoint,TcpData> np4 = Plan::makeEGS(p4, 400);                 int ix4 = kpc.add(np4);
	//NavPoint np5(p5, 500);    kpc.addNavPoint(np5);
	//fpln(" $$$$$ BEFORE kpc = "+kpc.toStringGs());
	EXPECT_TRUE(kpc.isGsConsistent(false));
	EXPECT_FALSE(kpc.isGsContinuous(false));
	kpc.repairGsContinuity(0, 1, 0.0, 300, 10.0);
	kpc.repairGsContinuity(1, 2, 300, 200, 10.0);
	kpc.repairGsContinuity(2, 3, 200, 0, 10.0);

	//PlanUtil::repairGsContinuity(kpc,10.0);
	fpln(" $$$$$ AFTER kpc = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"test_fixGs_Cont_2");
	EXPECT_TRUE(kpc.isGsConsistent(false));
	EXPECT_TRUE(kpc.isGsContinuous(false));
}




TEST_F(PlanTest, test_calcTimeGsIn_NAN) {
	Plan lpc("noname");
	Position p3  = Position::makeLatLonAlt(32.745405043907, -97.146846742627, 3250.000000000000);
	Position p4  = Position::makeLatLonAlt(32.721311284324, -97.166268450918, 2116.847579085755);
	Position p5  = Position::makeLatLonAlt(32.642073000000, -97.230087000000, 1277.000000000000);
	NavPoint np3(p3,396.1356955962630000);     TcpData tcp3;
	lpc.add(np3, tcp3);
	NavPoint np4(p4,425.7256767336330000);     TcpData tcp4;
	lpc.add(np4, tcp4);
	NavPoint np5(p5,523.0312068129490000);     TcpData tcp5;
	lpc.add(np5, tcp5);
	double gsout = 0.0;
	double lastTm = lpc.calcTimeGsIn(lpc.size()-1, gsout);
	EXPECT_TRUE(ISNAN(lastTm));
}



TEST_F(PlanTest, test_repair_negGs) {
	 Plan kpc("UF3");
	 Position p0  = Position::makeLatLonAlt(32.624787320000, -96.631879970000, 2026.013422060000);
	 Position p1  = Position::makeLatLonAlt(32.609350850000, -96.616491110000, 1607.989890080000);
	 Position p2  = Position::makeLatLonAlt(32.607040000000, -96.614188000000, 1372.000000000000);
	 Position p3  = Position::makeLatLonAlt(32.607040000000, -96.614188000000, 1332.791996590000);
	 Position p4  = Position::makeLatLonAlt(32.607040000000, -96.614188000000, 1322.000000000000);
	 NavPoint np0(p0,368.0039992600000000);     TcpData tcp0;
	 tcp0.setBGS(-0.980665000000);       kpc.add(np0, tcp0);
	 NavPoint np1(p1,414.3237076300000000);     TcpData tcp1;
	 int ix1 = kpc.add(np1, tcp1);
	 kpc.setInfo(ix1,"<arrFix>");
	 NavPoint np2(p2,440.4729122800000000);     TcpData tcp2;
	 tcp2.setEGS();       int ix2 = kpc.add(np2, tcp2);
	 kpc.setInfo(ix2,"<BOD>");
	 NavPoint np3(p3,445.1778726900000000);     TcpData tcp3;
	 tcp3.setBVS(0.980665000000);       kpc.add(np3, tcp3);
	 NavPoint np4(p4,447.7679518700000000);     TcpData tcp4;
	 tcp4.setEVS();       kpc.add(np4, tcp4);
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 //fpln(" $$$$ BEFORE gsIn1 = "+kpc.gsInCalc(1));
	 //fpln(" $$$$ BEFORE gsIn2 = "+kpc.gsInCalc(2));
	 EXPECT_TRUE(kpc.gsInCalc(1) >= 0);
	 EXPECT_FALSE(kpc.gsInCalc(2) >= 0);
	 //EXPECT_FALSE(kpc.isConsistent(false));
	 kpc.repairNegGsIn(2);
	 kpc.getMessage();
	 //fpln(" $$$$ AFTER gsIn1 = "+kpc.gsInCalc(1));
	 //fpln(" $$$$ AFTER gsIn2 = "+kpc.gsInCalc(2));
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 EXPECT_TRUE(kpc.gsInCalc(1) >= 0);
	 EXPECT_TRUE(kpc.gsInCalc(2) >= 0);
	 EXPECT_TRUE(kpc.isConsistent(false));

	 kpc.setTime(2,440.49);
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 EXPECT_FALSE(kpc.gsInCalc(2) >= 0);
	 kpc.repairNegGsIn(2);
	 EXPECT_TRUE(kpc.gsInCalc(2) >= 0);
	 EXPECT_TRUE(kpc.isConsistent(false));

	 kpc.setGsAccel(0,-0.982);
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 EXPECT_TRUE(kpc.gsInCalc(1) >= 0);
	 EXPECT_FALSE(kpc.gsInCalc(2) >= 0);
	 kpc.repairNegGsIn(2);
	 EXPECT_TRUE(kpc.gsInCalc(2) >= 0);
	 //EXPECT_TRUE(kpc.isConsistent(false));
}


















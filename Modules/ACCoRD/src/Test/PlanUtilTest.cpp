/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect4.h"
#include "Units.h"
#include "PlanUtil.h"
#include <cmath>
#include <gtest/gtest.h>
#include "format.h"
#include "Kinematics.h"
#include "TrajTemplates.h"
#include "KinematicsPosition.h"
#include "TrajGen.h"
//#include "DebugSupport.h"

using namespace larcfm;

class PlanUtilTest : public ::testing::Test {

public:
	Plan lpc;

protected:
	virtual void SetUp() {
		lpc.clear();
		lpc.setID("testCutDownTo");
		lpc.setNote("**note**");
	}
};


TEST_F(PlanUtilTest, testCutDownTo) {
	//fpln(" ------------------------------ testCutDownTo ---------------------------------");
	Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p2 = Position::makeXYZ(10.0, 5.0, 10000);
	Position p3 = Position::makeXYZ(20.0, 15.0, 10000);
	Velocity v4 = Velocity::makeTrkGsVs(45,450,500);
	Position p4 = p3.linear(v4,200);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,201.25);
	NavPoint np3(p3,455.8);
	NavPoint np4(p4,905.8);
	Plan lpc("testCutDownTo","**note**");
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	//lpc = lpc.makeSpeedConstant(Units::from("kn",180.0));
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 20);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//DebugSupport.dumpPlan(kpc, "testCutDownTo_kpc");
	//fpln("testCutDownTo: kpc = "+kpc);
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_EQ(7,kpc.size());
	EXPECT_FALSE(kpc.isTCP(0));
	EXPECT_TRUE(kpc.isBOT(1));
	EXPECT_FALSE(kpc.isTCP(2));
	EXPECT_TRUE(kpc.isEOT(3));
	EXPECT_TRUE(kpc.isBVS(4));
	EXPECT_TRUE(kpc.isEVS(5));
	EXPECT_FALSE(kpc.isTCP(6));
	//fpln("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	Plan kpc2 = PlanUtil::cutDownTo(kpc,0,200);
	//fpln(" $$ testCutDownTo: kpc2 = "+kpc2);
	//DebugSupport.dumpPlan(kpc2, "testCutDownTo_kpc2");
	EXPECT_TRUE(kpc2.isFlyable());
	EXPECT_FALSE(kpc2.isTCP(0));
	EXPECT_TRUE(kpc2.isBOT(1));
	EXPECT_TRUE(kpc2.isMOT(2));
	EXPECT_TRUE(kpc2.isEOT(3));
	EXPECT_EQ(kpc2.getID(),kpc.getID());
	EXPECT_EQ(kpc2.getNote(),kpc.getNote());
	kpc2 = PlanUtil::cutDownTo(kpc,0,500);
	EXPECT_TRUE(kpc2.isFlyable());
	EXPECT_EQ(7,kpc2.size());
	EXPECT_FALSE(kpc2.isTCP(0));
	EXPECT_TRUE(kpc2.isBOT(1));
	EXPECT_TRUE(kpc2.isEOT(3));
	EXPECT_TRUE(kpc2.isBVS(4));
	EXPECT_TRUE(kpc2.isEVS(5));
	kpc2 = PlanUtil::cutDownTo(kpc,0,456);
	//fpln(" $$$$$$$$$$$$$$$$$$$$$$ kpc2 = "+kpc2);
	EXPECT_TRUE(kpc2.isWeakConsistent());
	EXPECT_EQ(6,kpc2.size());
	EXPECT_FALSE(kpc2.isTCP(0));
	EXPECT_TRUE(kpc2.isBOT(1));
	EXPECT_TRUE(kpc2.isEOT(3));
	EXPECT_TRUE(kpc2.isBVS(4));
	EXPECT_TRUE(kpc2.isEVS(5));
	kpc2 = PlanUtil::cutDownTo(kpc,300,456);
	EXPECT_TRUE(kpc2.isWeakConsistent());
	EXPECT_EQ(3,kpc2.size());
	EXPECT_FALSE(kpc2.isTCP(0));
	EXPECT_TRUE(kpc2.isBVS(1));
	EXPECT_TRUE(kpc2.isEVS(2));
	kpc2 = PlanUtil::cutDownTo(kpc,200,250);                        // in the middle of a turn so go back
	EXPECT_TRUE(kpc2.isFlyable());
	EXPECT_EQ(4,kpc2.size());
	EXPECT_TRUE(kpc2.isTCP(0));
	EXPECT_TRUE(kpc2.isBOT(0));
	EXPECT_TRUE(kpc2.isEOT(2));
	EXPECT_FALSE(kpc2.isTCP(3));
	kpc2 = PlanUtil::cutDownTo(kpc,100,1000);
	EXPECT_TRUE(kpc2.isFlyable());
	EXPECT_EQ(8,kpc2.size());
	EXPECT_FALSE(kpc2.isTCP(0));
	EXPECT_TRUE(kpc2.isBOT(1));
	EXPECT_FALSE(kpc2.isTCP(2));
	EXPECT_TRUE(kpc2.isEOT(3));
	EXPECT_TRUE(kpc2.isBVS(4));
	EXPECT_TRUE(kpc2.isEVS(5));
	EXPECT_FALSE(kpc2.isTCP(6));
	EXPECT_FALSE(kpc2.isTCP(7));
    //fpln(" $$$$"+kpc2.getName()+" =? "+kpc.getName());
	EXPECT_EQ(kpc2.getID(),kpc.getID());
	EXPECT_EQ(kpc2.getNote(),kpc.getNote());
}

TEST_F(PlanUtilTest, testCutDownTo_2) {
	Plan kpc("UF17");
	Position p6  = Position::makeLatLonAlt(32.767446441849, -96.869025269317, 1997.302343259675);
	Position p7  = Position::makeLatLonAlt(32.767313388374, -96.870006368012, 1999.999999999999);
	Position p8  = Position::makeLatLonAlt(32.762530551013, -96.905234357567, 2000.000000000000);
	Position p9  = Position::makeLatLonAlt(32.753586126783, -96.970910539478, 2000.000000000000);
	Position p10  = Position::makeLatLonAlt(32.753369347450, -96.972499005100, 1996.909602628373);
	Position p11  = Position::makeLatLonAlt(32.753153906062, -96.974077513728, 1987.638410513491);
	Position p12  = Position::makeLatLonAlt(32.748423175068, -97.008700524547, 1659.521886992683);
	Position p13  = Position::makeLatLonAlt(32.745091943041, -97.033037060744, 1370.319700252508);
	Position p14  = Position::makeLatLonAlt(32.741849734340, -97.056688565373, 957.218767985915);
	Position p15  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 724.000000000000);
	Position p16  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 684.791996587350);
	Position p17  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 674.000000000000);
	NavPoint np6(p6,1105.2619924519513000);     TcpData tcp6;
	tcp6.setBGS(0.980665000000);       int ix6 = kpc.add(np6, tcp6);
	kpc.setInfo(ix6,"<TOC>");
	NavPoint np7(p7,1106.5569498622626000);     TcpData tcp7;
	tcp7.setEVS();       kpc.add(np7, tcp7);
	NavPoint np8(p8,1143.4322622861132000);     TcpData tcp8;
	tcp8.setEGS();       kpc.add(np8, tcp8);
	NavPoint np9(p9,1200.7322694094587000);     TcpData tcp9;
	tcp9.setBVS(-0.980665000000);       kpc.add(np9, tcp9);
	NavPoint np10(p10,1202.1182887940117000);     TcpData tcp10;
	tcp10.setBGS(-0.980665000000);       int ix10 = kpc.add(np10, tcp10);
	kpc.setInfo(ix10,"<TOD>");
	NavPoint np11(p11,1203.5043081785652000);     TcpData tcp11;
	tcp11.setEVS();       kpc.add(np11, tcp11);
	NavPoint np12(p12,1240.2937406773926000);     TcpData tcp12;
	tcp12.setEGS();       kpc.add(np12, tcp12);
	NavPoint np13(p13,1272.7199782024777000);     TcpData tcp13;
	tcp13.setBGS(-0.980665000000);       kpc.add(np13, tcp13);
	NavPoint np14(p14,1319.0381236580834000);     TcpData tcp14;
	int ix14 = kpc.add(np14, tcp14);
	kpc.setInfo(ix14,"<arrFix>");
	NavPoint np15(p15,1345.1873283071240000);     TcpData tcp15;
	tcp15.setEGS();       int ix15 = kpc.add(np15, tcp15);
	kpc.setInfo(ix15,"<BOD>");
	NavPoint np16(p16,1349.8922887166423000);     TcpData tcp16;
	tcp16.setBVS(0.980665000000);       kpc.add(np16, tcp16);
	NavPoint np17(p17,1352.4823678976063000);     TcpData tcp17;
	tcp17.setEVS();       kpc.add(np17, tcp17);
	double tmTOD = 1202.1182887940117;
	// %%%%%%%%%%%%%%%%%%%% ownship.getLastTime() = 1352.4823678976063
    //fpln(" $$ testCutDownTo_2: kpc = "+kpc.toStringProfile());
	Plan afterTOD = PlanUtil::cutDownTo(kpc, tmTOD, kpc.getLastTime());
    //fpln(" $$ testCutDownTo_2: afterTOD = "+afterTOD.toStringProfile());
    EXPECT_NEAR(tmTOD,afterTOD.time(0),0.00001);
    EXPECT_NEAR(kpc.getLastTime(),afterTOD.getLastTime(),0.00001);
    afterTOD.repairPlan();
    //fpln(" $$ testCutDownTo_2: afterTOD = "+afterTOD.toStringProfile());
	bool isConsistentAft = afterTOD.isConsistent(false);
	EXPECT_TRUE(isConsistentAft);
	EXPECT_TRUE(afterTOD.isVelocityContinuous(false));
}


TEST_F(PlanUtilTest, testCutDownTo_3) {
	double t1 = 1192.1182887940117;
	double t2 = 1352.4823678976063;
	Plan kpc("UF17");
	Position p0  = Position::makeLatLonAlt(32.774902000000, -96.813955000000, 1037.000000000000);
	Position p1  = Position::makeLatLonAlt(32.774902000000, -96.813955000000, 1047.791996587351);
	Position p2  = Position::makeLatLonAlt(32.774902000000, -96.813955000000, 1087.000000000000);
	Position p3  = Position::makeLatLonAlt(32.774423093328, -96.817498064092, 1304.896210682829);
	Position p4  = Position::makeLatLonAlt(32.771220856312, -96.841169056992, 1690.899443476699);
	Position p5  = Position::makeLatLonAlt(32.767578309590, -96.868052855371, 1989.209373038700);
	Position p6  = Position::makeLatLonAlt(32.767446441849, -96.869025269317, 1997.302343259675);
	Position p7  = Position::makeLatLonAlt(32.767313388374, -96.870006368012, 1999.999999999999);
	Position p8  = Position::makeLatLonAlt(32.762530551013, -96.905234357567, 2000.000000000000);
	Position p9  = Position::makeLatLonAlt(32.753586126783, -96.970910539478, 2000.000000000000);
	Position p10  = Position::makeLatLonAlt(32.753369347450, -96.972499005100, 1996.909602628373);
	Position p11  = Position::makeLatLonAlt(32.753153906062, -96.974077513728, 1987.638410513491);
	Position p12  = Position::makeLatLonAlt(32.748423175068, -97.008700524547, 1659.521886992683);
	Position p13  = Position::makeLatLonAlt(32.745091943041, -97.033037060744, 1370.319700252508);
	Position p14  = Position::makeLatLonAlt(32.741849734340, -97.056688565373, 957.218767985915);
	Position p15  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 724.000000000000);
	Position p16  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 684.791996587350);
	Position p17  = Position::makeLatLonAlt(32.741364000000, -97.060229000000, 674.000000000000);
	NavPoint np0(p0,988.4000000000000000);     TcpData tcp0;
	tcp0.setBVS(0.980665000000);       kpc.add(np0, tcp0);
	NavPoint np1(p1,990.9900791809640000);     TcpData tcp1;
	tcp1.setEVS();       kpc.add(np1, tcp1);
	NavPoint np2(p2,995.6950395904820000);     TcpData tcp2;
	tcp2.setBGS(0.980665000000);       int ix2 = kpc.add(np2, tcp2);
	kpc.setInfo(ix2,"<BOC>");
	NavPoint np3(p3,1021.8442442395273000);     TcpData tcp3;
	int ix3 = kpc.add(np3, tcp3);
	kpc.setInfo(ix3,"<depFix>");
	NavPoint np4(p4,1068.1675717443472000);     TcpData tcp4;
	tcp4.setEGS();       kpc.add(np4, tcp4);
	NavPoint np5(p5,1103.9670350416400000);     TcpData tcp5;
	tcp5.setBVS(-0.980665000000);       kpc.add(np5, tcp5);
	NavPoint np6(p6,1105.2619924519513000);     TcpData tcp6;
	tcp6.setBGS(0.980665000000);       int ix6 = kpc.add(np6, tcp6);
	kpc.setInfo(ix6,"<TOC>");
	NavPoint np7(p7,1106.5569498622626000);     TcpData tcp7;
	tcp7.setEVS();       kpc.add(np7, tcp7);
	NavPoint np8(p8,1143.4322622861132000);     TcpData tcp8;
	tcp8.setEGS();       kpc.add(np8, tcp8);
	NavPoint np9(p9,1200.7322694094587000);     TcpData tcp9;
	tcp9.setBVS(-0.980665000000);       kpc.add(np9, tcp9);
	NavPoint np10(p10,1202.1182887940117000);     TcpData tcp10;
	tcp10.setBGS(-0.980665000000);       int ix10 = kpc.add(np10, tcp10);
	kpc.setInfo(ix10,"<TOD>");
	NavPoint np11(p11,1203.5043081785652000);     TcpData tcp11;
	tcp11.setEVS();       kpc.add(np11, tcp11);
	NavPoint np12(p12,1240.2937406773926000);     TcpData tcp12;
	tcp12.setEGS();       kpc.add(np12, tcp12);
	NavPoint np13(p13,1272.7199782024777000);     TcpData tcp13;
	tcp13.setBGS(-0.980665000000);       kpc.add(np13, tcp13);
	NavPoint np14(p14,1319.0381236580834000);     TcpData tcp14;
	int ix14 = kpc.add(np14, tcp14);
	kpc.setInfo(ix14,"<arrFix>");
	NavPoint np15(p15,1345.1873283071240000);     TcpData tcp15;
	tcp15.setEGS();       int ix15 = kpc.add(np15, tcp15);
	kpc.setInfo(ix15,"<BOD>");
	NavPoint np16(p16,1349.8922887166423000);     TcpData tcp16;
	tcp16.setBVS(0.980665000000);       kpc.add(np16, tcp16);
	NavPoint np17(p17,1352.4823678976063000);     TcpData tcp17;
	tcp17.setEVS();       kpc.add(np17, tcp17);
	//fpln(" $$ testCutDownTo_3: kpc = "+kpc.toStringProfile());
	Plan after = PlanUtil::cutDownTo(kpc, t1,t2);
    EXPECT_NEAR(t1,after.time(0),0.00001);
    EXPECT_NEAR(t2,after.getLastTime(),0.00001);
	bool isConsistentAft = after.isConsistent(false);
	EXPECT_TRUE(isConsistentAft);
	EXPECT_TRUE(after.isVelocityContinuous(false));
	t1 = 1105.26;
	t2 = 1202.12;
	after = PlanUtil::cutDownTo(kpc, t1,t2);
    EXPECT_NEAR(t1,after.time(0),0.00001);
    EXPECT_NEAR(t2,after.getLastTime(),0.00001);
	EXPECT_TRUE(after.isConsistent(false));
	EXPECT_TRUE(after.isVelocityContinuous(false));
}




TEST_F(PlanUtilTest, test_cutDownTo_4) {
	Plan kpc("");
	Position p0_2  = Position::makeLatLonAlt(32.733170000000, -96.710393000000, 1036.000000000000);
	Position p0_3  = Position::makeLatLonAlt(32.733100783173, -96.711170054582, 1137.116313503922);
	Position p0_4  = Position::makeLatLonAlt(32.733100102095, -96.711178790924, 1137.684011892130);
	Position p0_5  = Position::makeLatLonAlt(32.733099613625, -96.711187545467, 1138.251710280339);
	NavPoint np0_2(p0_2,72.7950395904819600);     TcpData tcp0_2;
	tcp0_2.setBGS(0.980665000000);       kpc.add(np0_2, tcp0_2);
	NavPoint np0_3(p0_3,85.0000000000000000);     TcpData tcp0_3;
	Position turnCenter0_3 = Position::makeLatLonAlt(32.733381137647, -96.711205347486, 1137.116313503922);tcp0_3.setBOT(31.327174956166, turnCenter0_3);tcp0_3.setEGS();       int ix0_3 = kpc.add(np0_3, tcp0_3);
	kpc.setVertexRadius(ix0_3,31.3271749561663);
	NavPoint np0_4(p0_4,85.0685224382944200);     TcpData tcp0_4;
	tcp0_4.setMOT(true);kpc.add(np0_4, tcp0_4);
	NavPoint np0_5(p0_5,85.1370448765888600);     TcpData tcp0_5;
	tcp0_5.setEOT();       kpc.add(np0_5, tcp0_5);
	EXPECT_TRUE(kpc.isFlyable(false));
	//fpln(" $$>>> test_cutDownTo_4: newTraj = "+kpc.toStringVs());
    // This range is in a turn, but does not include MOT
	double t1 = 85.10;
	double t2 = 85.14;
	Plan newTraj = PlanUtil::cutDownTo(kpc, t1,t2);
	//fpln(" $$>>> test_cutDownTo_4: newTraj = "+newTraj.toStringVs());
	EXPECT_NEAR(t1,newTraj.time(0),0.00001);
	EXPECT_NEAR(t2,newTraj.getLastTime(),0.00001);
	EXPECT_TRUE(newTraj.isConsistent(false));
    EXPECT_TRUE(newTraj.isVelocityContinuous(false));
}




TEST_F(PlanUtilTest, test_cutDownTo_Plus) {
	double t1 = 85.0;
	Plan kpc("UF25");
	Position p0_0  = Position::makeLatLonAlt(32.733170000000, -96.710393000000, 986.000000000000);
	Position p0_1  = Position::makeLatLonAlt(32.733170000000, -96.710393000000, 996.791996587350);
	Position p0_2  = Position::makeLatLonAlt(32.733170000000, -96.710393000000, 1036.000000000000);
	Position p0_3  = Position::makeLatLonAlt(32.733100783173, -96.711170054582, 1137.116313503922);
	Position p0_4  = Position::makeLatLonAlt(32.733100102095, -96.711178790924, 1137.684011892130);
	Position p0_5  = Position::makeLatLonAlt(32.733099613625, -96.711187545467, 1138.251710280339);
	Position p0_6  = Position::makeLatLonAlt(32.680772131651, -97.778885202595, 70357.647054552280);
	NavPoint np0_0(p0_0,65.5000000000000000);     TcpData tcp0_0;
	tcp0_0.setBVS(0.980665000000);       kpc.add(np0_0, tcp0_0);
	NavPoint np0_1(p0_1,68.0900791809639300);     TcpData tcp0_1;
	tcp0_1.setEVS();       kpc.add(np0_1, tcp0_1);
	NavPoint np0_2(p0_2,72.7950395904819600);     TcpData tcp0_2;
	tcp0_2.setBGS(0.980665000000);       int ix0_2 = kpc.add(np0_2, tcp0_2);
	kpc.setInfo(ix0_2,"<BOC>");
	NavPoint np0_3(p0_3,85.0000000000000000);     TcpData tcp0_3;
	Position turnCenter0_3 = Position::makeLatLonAlt(32.733381137647, -96.711205347486, 1137.116313503922);tcp0_3.setBOT(31.327174956166, turnCenter0_3);tcp0_3.setEGS();       int ix0_3 = kpc.add(np0_3, tcp0_3);
	kpc.setVertexRadius(ix0_3,31.3271749561663);
	kpc.setInfo(ix0_3,"ChorusResolution");
	NavPoint np0_4(p0_4,85.0685224382944200);     TcpData tcp0_4;
	tcp0_4.setMOT(true);kpc.add(np0_4, tcp0_4);
	NavPoint np0_5(p0_5,85.1370448765888600);     TcpData tcp0_5;
	tcp0_5.setEOT();       kpc.add(np0_5, tcp0_5);
	NavPoint np0_6(p0_6,8440.0696019750180000);     TcpData tcp0_6;
	kpc.add(np0_6, tcp0_6);
	EXPECT_TRUE(kpc.isFlyable(false));
    //fpln(" $$ kpc = "+kpc.toStringProfile());
	double tExtend = 1E6;
	Plan newTraj = PlanUtil::cutDownTo(kpc, t1, tExtend);
    //fpln(" $$ newTraj = "+newTraj.toStringProfile());
	EXPECT_TRUE(newTraj.isFlyable(false));
    EXPECT_NEAR(t1,newTraj.time(0),0.00001);
    EXPECT_NEAR(tExtend,newTraj.getLastTime(),0.00001);
	Plan newTraj2 = PlanUtil::cutDownTo(kpc, 65.5,85.0);
    EXPECT_NEAR(65.5,newTraj2.time(0),0.00001);
    EXPECT_NEAR(85.0,newTraj2.getLastTime(),0.00001);
	EXPECT_TRUE(newTraj2.isFlyable(false));
	//fpln(" $$ test_cutDownTo_Plus: kpc = "+kpc.toStringProfile());
	t1 = 84.99999999999889;
	double t2 = 85.13;
	newTraj = PlanUtil::cutDownTo(kpc, t1,t2);
	//fpln(" $$>>> test_cutDownTo_Plus: newTraj = "+newTraj.toStringProfile());
	EXPECT_NEAR(t1,newTraj.time(0),0.00001);
	EXPECT_NEAR(t2,newTraj.getLastTime(),0.00001);
	EXPECT_TRUE(newTraj.isConsistent(false));
    EXPECT_TRUE(newTraj.isVelocityContinuous(false));
	t1 = 65.5;
	t2 = 85.14;
	for (double tt = t1; tt < t2; tt = tt + 0.1) {
		//fpln(" $$.......................... test_cutDownTo_Plus: tt = "+tt+" t2 = "+t2);
		newTraj = PlanUtil::cutDownTo(kpc, tt, t2);
		//fpln(" $$>>> test_cutDownTo_Plus: tt = "+tt+"  newTraj = "+newTraj.toStringProfile());
		EXPECT_NEAR(tt,newTraj.time(0),0.00001);
		EXPECT_NEAR(t2,newTraj.getLastTime(),0.00001);
		EXPECT_TRUE(newTraj.isConsistent(false));
		EXPECT_TRUE(newTraj.isVelocityContinuous(false));
	}
}






TEST_F(PlanUtilTest, testCutDown) {
	//fpln(" ------------------------------ testCutDownTo ---------------------------------");
	Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p2 = Position::makeXYZ(10.0, 5.0, 10000);
	Position p3 = Position::makeXYZ(20.0, 15.0, 10000);
	Velocity v4 = Velocity::makeTrkGsVs(45,450,500);
	Position p4 = p3.linear(v4,200);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,201.25);
	NavPoint np3(p3,455.8);
	NavPoint np4(p4,905.8);
	Plan lpc("testCutDown","***route***");
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	//lpc = lpc.makeSpeedConstant(Units::from("kn",180.0));
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 20);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
    //fpln(" $$$ "+kpc.getName()+" =? "+lpc.getName());
	EXPECT_EQ(kpc.getID(),lpc.getID());
	EXPECT_TRUE(contains(kpc.getNote(),lpc.getNote()));

	//DebugSupport.dumpPlan(kpc, "testCutDown_kpc");
	//fpln("testCutDown: kpc = "+kpc.toString());
	EXPECT_TRUE(kpc.isFlyable());
	EXPECT_EQ(7,kpc.size());
	double leadInTime = 320;
	double startTime = kpc.getFirstTime()+leadInTime;
	double endTime = kpc.getLastTime()-leadInTime;
	//fpln(" $$$$ startTime = "+Fm1(startTime)+" endTime = "+Fm1(endTime));
	Plan nPlan = PlanUtil::cutDownLinear(kpc, startTime, endTime);
	//fpln(" $$$$ nPlan = "+nPlan.toString());
	//DebugSupport.dumpPlan(nPlan, "testCutDown_nPlan");
	EXPECT_NEAR(startTime,nPlan.getFirstTime(),0.0001);
	EXPECT_NEAR(endTime,nPlan.getLastTime(),0.0001);
	int k = 0;
	for (k = 0; k < nPlan.size(); k++) {
		if (kpc.point(k).time() > startTime) break;
	}
	//fpln(" $$$$ k = "+Fm0(k));
	for (int j = 1; j < nPlan.size()-1; j++) {
		EXPECT_TRUE(kpc.point(k+j-1).equals(nPlan.point(j)));
	}
}




TEST_F(PlanUtilTest, test_removeRedundantPoints) {
	Position p0(LatLonAlt::make(34.1626, -118.9911, 5697.2238));
	Position p1(LatLonAlt::make(34.1967, -119.0753, 0.0000));
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,100.0);
	Plan plan("");
	plan.addNavPoint(np0);
	plan.addNavPoint(np1);
	Velocity v0 = plan.initialVelocity(0);
	//fpln(" $$$ v0 = "+v0.toString());
	NavPoint np2 = np0.linear(v0,25);
	plan.addNavPoint(np2);
	NavPoint np3 = np0.linear(v0,37);
	plan.addNavPoint(np3);
	NavPoint np4 = np0.linear(v0,50.5);
	plan.addNavPoint(np4);
	NavPoint np5 = np0.linear(v0,87.2);
	plan.addNavPoint(np5);
	plan.removeRedundantPoints();
	//fpln(" pln = "+plan.toString());
}

TEST_F(PlanUtilTest, test_revertTCPs) {
	Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899.410000));
	Position p1(LatLonAlt::make(-1.015100, -1.100000, 7899.410000));
	Position p2(LatLonAlt::make(-1.188000, -0.370300, 7899.410000));
	Position p3(LatLonAlt::make(-1.549400, 0.351100, 3983.877000));
	Position p4(LatLonAlt::make(-1.845700, 0.899500, 3983.877000));
	NavPoint np0(p0,0.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,403.897800);     lpc.addNavPoint(np1);
	NavPoint np2(p2,769.147300);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1133.991300);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1461.597000);     lpc.addNavPoint(np4);
	bool repair = true;
	double gsAccel = 1;
	double vsAccel = 2;
	double bankAngle = Units::from("deg", 20);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	EXPECT_TRUE(kpc.isWellFormed());
	//fpln(" $$$$$$$$$$$ BEFORE kpc = "+kpc.toString());
	//bool checkSource = false;
	Plan kpcSave(kpc);
	//PlanUtil::savePlan(kpc, "1dump_testStructuralRevertTCPs_BEFORE.txt");
	//kpc = PlanUtil::revertTCPs(kpc);  // BOT
	kpc = PlanUtil::revertAllTCPs(kpc,false);
	//fpln(" $$$$$$$$$$$ AFTER kpc = "+kpc.toString());
	//PlanUtil::savePlan(kpc, "1dump_testStructuralRevertTCPs_AFTER.txt");
    EXPECT_EQ(lpc.getID(),kpc.getID());
    //EXPECT_EQ(lpc.getNote(),kpc.getNote());
	EXPECT_TRUE(contains(kpc.getNote(),lpc.getNote()));

//    for (int i = 0; i < lpc.size(); i++) {
//    	EXPECT_NEAR(lpc.point(i).lat(),kpc.point(i).lat(),0.0001);
//    	EXPECT_NEAR(lpc.point(i).lon(),kpc.point(i).lon(),0.0001);
//    	EXPECT_NEAR(lpc.point(i).alt(),kpc.point(i).alt(),0.0001);
//    }
}



TEST_F(PlanUtilTest, test1_StructRevertTCP) {
	fpln(" ------------------------------ test1_StructRevertTCP ---------------------------------");
	Position p1 = Position::makeXYZ(0.0, 0.0, 10000);
	Position p2 = Position::makeXYZ(10.0, 5.0, 10000);
	Position p3 = Position::makeXYZ(20.0, 15.0, 10000);
	Velocity v4 = Velocity::makeTrkGsVs(45,450,0);
	Position p4 = p3.linear(v4,200);
	NavPoint np1(p1,0.0);
	NavPoint np2(p2,201.25);
	NavPoint np3(p3,455.8);
	NavPoint np4(p4,905.8);
	lpc.addNavPoint(np1);
	lpc.addNavPoint(np2);
	lpc.addNavPoint(np3);
	lpc.addNavPoint(np4);
	//fpln("testSimpleTurn: lpc = "+lpc.toString());
	//fpln(" $$$ lpc: speed into p2 "+Units::str("kn",lpc.finalVelocity(0).gs(),8));
	//PlanUtil::savePlan(lpc, "1dump_test1_StructRevertTCP_lpc.txt");
	double gsAccel = 2;
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 5);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, true, true, true);
	//fpln(" $$$ kpc: speed into p2 "+Units::str("kn",kpc.finalVelocity(0).gs(),8));
	//PlanUtil::savePlan(kpc, "1dump_before_StructRevertTCP_kpc.txt");
	//fpln(" $$$$$ kpc.strWellFormed = "+kpc.strWellFormed());
	EXPECT_TRUE(kpc.isFlyable());
	Plan kpcSave(kpc);
	//fpln(" $$$ test1_StructRevertTCP: BEFORE kpc = "+kpc.toString());
	kpc = PlanUtil::revertAllTCPs(kpc,false);
	EXPECT_NEAR(kpcSave.initialVelocity(0).gs(),kpc.initialVelocity(0).gs(),0.01);
	EXPECT_NEAR(kpcSave.initialVelocity(1).gs(),kpc.initialVelocity(1).gs(),0.01);
    EXPECT_EQ(lpc.getID(),kpc.getID());
	EXPECT_TRUE(contains(kpc.getNote(),lpc.getNote()));
}


TEST_F(PlanUtilTest, test2_StructRevertTCP) {
	Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899.410000));
	Position p1(LatLonAlt::make(-1.015100, -1.100000, 7899.410000));
	Position p2(LatLonAlt::make(-1.188000, -0.370300, 7899.410000));
	Position p3(LatLonAlt::make(-1.549400, 0.351100, 3983.877000));
	Position p4(LatLonAlt::make(-1.845700, 0.899500, 3983.877000));
	NavPoint np0(p0,0.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,403.897800);     lpc.addNavPoint(np1);
	NavPoint np2(p2,769.147300);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1133.991300);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1461.597000);     lpc.addNavPoint(np4);
	bool repair = true;
	double gsAccel = 2;
	double vsAccel = 2;
	double bankAngle = Units::from("deg", 30);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair, repair, repair);
	EXPECT_TRUE(kpc.isWellFormed());
	//fpln(" $$$$$$$$$$$ BEFORE kpc = "+kpc.toString());
	//bool checkSource = false;
	Plan kpcSave(kpc);
	//PlanUtil::savePlan(kpc, "1dump_testStructuralRevertTCPs_BEFORE.txt");
//	PlanUtil::structRevertTCP(kpc,6);  // BOT
//	PlanUtil::structRevertTCP(kpc,4);  // BGS
//	PlanUtil::structRevertTCP(kpc,5);  // BVS
	kpc = PlanUtil::revertAllTCPs(kpc,false);
	//fpln(" $$$$$$$$$$$ AFTER kpc = "+kpc.toString());
	//PlanUtil::savePlan(kpc, "1dump_testStructuralRevertTCPs_AFTER.txt");
    EXPECT_EQ(lpc.getID(),kpc.getID());
    //EXPECT_EQ(lpc.getNote(),kpc.getNote());
	EXPECT_TRUE(contains(kpc.getNote(),lpc.getNote()));

}


//TEST_F(PlanUtilTest, test_structRevertTCPs) {
//		Position p0 = Position::makeXYZ(-45.570000, 24.510000, 5000.000000);
//		Position p1 = Position::makeXYZ(-9.653533, 24.083014, 5000.000000);
//		Position p2 = Position::makeXYZ(-9.401160, 24.068648, 5000.000000);
//		Position p3 = Position::makeXYZ(-9.151097, 24.031671, 5000.000000);
//		Position p4 = Position::makeXYZ(31.472446, 16.143892, 5000.000000);
//		Position p5 = Position::makeXYZ(31.745921, 16.076427, 5000.000000);
//		Position p6 = Position::makeXYZ(32.011278, 15.981952, 5000.000000);
//		Position p7 = Position::makeXYZ(32.088296, 15.950103, 5000.000000);
//		Position p8 = Position::makeXYZ(33.629881, 15.312603, 5000.000000);
//		Position p9 = Position::makeXYZ(87.900000, -7.130000, 5000.000000);
//		NavPoint np0(p0,0.000000);    	 lpc.addNavPoint(np0);
//		Velocity vin1 = Velocity::mkTrkGsVs(1.582684,154.301579,0.000000);
//		NavPoint src1 =  NavPoint::makeLatLonAlt(-9.40000000, 24.08000000, 5000.00000000, 434.160000);
//		double sRadius1 = vin1.gs()/0.029636;
//		Position center = KinematicsPosition::centerFromRadius(p1, sRadius1, vin1.trk());
//		//fpln(" ## test_structRevertTCPs: center = "+center.toString());
//		std::pair<NavPoint,TcpData> np1 = Plan::makeBOT(src1,p1, 431.116760, sRadius1, center,-1);    	 lpc.add(np1);
//		NavPoint np2(p2,434.151789);    	 lpc.addNavPoint(np2);
//		Velocity vin3 = Velocity::mkTrkGsVs(1.762578,154.301579,0.000000);
//		NavPoint src3 =  NavPoint::makeLatLonAlt(-9.40000000, 24.08000000, 5000.00000000, 434.160000);
//		std::pair<NavPoint,TcpData> np3 = Plan::makeEOT(src3,p3, 437.186817);   	 lpc.add(np3);
//		Velocity vin4 = Velocity::mkTrkGsVs(1.762578,154.352444,0.000000);
//		NavPoint src4 =  NavPoint::makeLatLonAlt(31.75000000, 16.09000000, 5000.00000000, 937.120000);
//		double sRadius4 = vin4.gs()/ 0.029636;
//		center = KinematicsPosition::centerFromRadius(p4, sRadius4, vin4.trk());
//		std::pair<NavPoint,TcpData> np4 = Plan::makeBOT(src4,p4, 933.712150, sRadius4, center);   	 lpc.add(np4);
//		NavPoint np5(p5,937.093226);    	 lpc.addNavPoint(np5);
//		Velocity vin6 = Velocity::mkTrkGsVs(1.962916,154.352444,0.000000);
//		NavPoint src6 =  NavPoint::makeLatLonAlt(31.75000000, 16.09000000, 5000.00000000, 937.120000);
//		std::pair<NavPoint,TcpData> np6 = Plan::makeEOT(src6,p6, 940.474302);   	 lpc.add(np6);
//		Velocity vin7 = Velocity::mkTrkGsVs(1.962916,154.352444,0.000000);
//		NavPoint src7 =  NavPoint::makeLatLonAlt(31.75000000, 16.09000000, 5000.00000000, 937.120000);
//		std::pair<NavPoint,TcpData> np7 = Plan::makeBGS(src7,p7, 941.474302, 2.000000);   	 lpc.add(np7);
//		Velocity vin8 = Velocity::mkTrkGsVs(1.962916,154.352444,0.000000);
//		NavPoint src8 =  NavPoint::makeLatLonAlt(31.75000000, 16.09000000, 5000.00000000, 937.120000);
//		std::pair<NavPoint,TcpData> np8 = Plan::makeEGS(src8, p8, 959.406829);   	 lpc.add(np8);
//		NavPoint np9(p9,1531.190622);    	 lpc.addNavPoint(np9);
//		Plan save = Plan(lpc);
//		//		f.pln(" $$ save = "+save);
//		//		for (int j = 0; j < lpc.size(); j++) {
//		//			f.pln(" $$ initialVelocity("+j+") = "+lpc.initialVelocity(j));
//		//		}
//		bool killNextGsTCPs = false;
//		bool addBackMidPoints = false;
//	    std::string name = lpc.getName();
//	    std::string note = lpc.getNote();
//		lpc.structRevertTurnTCP(1, addBackMidPoints, killNextGsTCPs);
//		EXPECT_EQ(name,lpc.getName());
//		EXPECT_EQ(note,lpc.getNote());
//		for (int j = 2; j < lpc.size(); j++) {
//			//f.pln(" $$ initialVelocity("+j+") = "+lpc.initialVelocity(j));
//			EXPECT_TRUE(lpc.initialVelocity(j).compare(save.initialVelocity(j+2), 0.000001, 0.000001));
//		}
//		bool revertPreviousTurn = false;
//		PlanUtil::structRevertGsTCP(lpc, 5, revertPreviousTurn);
//		EXPECT_EQ(7,lpc.size());
//		EXPECT_NEAR(lpc.point(0).time(), 0.0000, 0.001);
//		EXPECT_NEAR(lpc.point(0).lat(), 45392.5200, 0.001);
//		EXPECT_NEAR(lpc.point(0).lon(), -84395.6400, 0.001);
//		EXPECT_NEAR(lpc.point(0).alt(), 1524.0000, 0.001);
//		EXPECT_NEAR(lpc.point(1).time(), 434.1600, 0.001);
//		EXPECT_NEAR(lpc.point(1).lat(), 44596.1599, 0.001);
//		EXPECT_NEAR(lpc.point(1).lon(), -17408.8000, 0.001);
//		EXPECT_NEAR(lpc.point(1).alt(), 1524.0000, 0.001);
//		EXPECT_NEAR(lpc.point(2).time(), 933.7276, 0.001);
//		EXPECT_NEAR(lpc.point(2).lat(), 29898.4880, 0.001);
//		EXPECT_NEAR(lpc.point(2).lon(), 58286.9700, 0.001);
//		EXPECT_NEAR(lpc.point(2).alt(), 1524.0000, 0.001);
//		EXPECT_TRUE(lpc.isBOT(2));
//		EXPECT_NEAR(lpc.point(3).time(), 937.1086, 0.001);
//		EXPECT_NEAR(lpc.point(3).lat(), 29773.5428, 0.001);
//		EXPECT_NEAR(lpc.point(3).lon(), 58793.4457, 0.001);
//		EXPECT_NEAR(lpc.point(3).alt(), 1524.0000, 0.001);
//		EXPECT_NEAR(lpc.point(4).time(), 940.4897, 0.001);
//		EXPECT_NEAR(lpc.point(4).lat(), 29598.5751, 0.001);
//		EXPECT_NEAR(lpc.point(4).lon(), 59284.8869, 0.001);
//		EXPECT_NEAR(lpc.point(4).alt(), 1524.0000, 0.001);
//		EXPECT_TRUE(lpc.isEOT(4));
//		EXPECT_NEAR(lpc.point(5).time(), 941.489, 0.001);
//		EXPECT_NEAR(lpc.point(5).lat(), 29539.59, 0.001);
//		EXPECT_NEAR(lpc.point(5).lon(), 59427.524, 0.001);
//		EXPECT_NEAR(lpc.point(5).alt(), 1524.0000, 0.001);
//		EXPECT_NEAR(lpc.point(6).time(), 1529.515, 0.001);
//		EXPECT_NEAR(lpc.point(6).lat(), -13204.7600, 0.001);
//		EXPECT_NEAR(lpc.point(6).lon(), 162790.8000, 0.001);
//		EXPECT_NEAR(lpc.point(6).alt(), 1524.0000, 0.001);
//		lpc.structRevertTurnTCP(2, addBackMidPoints, killNextGsTCPs);
//		EXPECT_EQ(5,lpc.size());
//		EXPECT_TRUE(lpc.isLinear());
//
//}


TEST_F(PlanUtilTest, test_structRevertGroupOfTCPs) {
	Plan lpc;
	Position p0(LatLonAlt::make(29.742000, -95.280103, 0.000000));
	Position p1(LatLonAlt::make(29.784761, -95.365811, 7610.100428));
	Position p2(LatLonAlt::make(29.785040, -95.366371, 7656.981993));
	Position p3(LatLonAlt::make(29.787661, -95.369423, 7828.219610));
	Position p4(LatLonAlt::make(29.788144, -95.369750, 7832.219402));
	Position p5(LatLonAlt::make(29.788186, -95.369779, 7832.326881));
	Position p6(LatLonAlt::make(29.809409, -95.384169, 7870.291058));
	Position p7(LatLonAlt::make(29.890022, -95.438898, 7981.577932));
	Position p8(LatLonAlt::make(29.892979, -95.440907, 7954.928930));
	Position p9(LatLonAlt::make(29.895074, -95.442179, 7900.707982));
	Position p10(LatLonAlt::make(29.898184, -95.443852, 7770.188756));
	Position p11(LatLonAlt::make(30.048000, -95.524605, 1.000000));
	NavPoint np0(p0,10935.000000);    	 lpc.addNavPoint(np0);
	Velocity vin1 = Velocity::mkTrkGsVs(-1.049534,67.672453,16.461250);
	//NavPoint src1 =  NavPoint::makeLatLonAlt(29.78600000, -95.36829700, 7826.86574400, 11079.994941);
	//NavPoint np1 = src1.makeBVS(p1, 11075.910238, 0.000000, vin1, -1);
	std::pair<NavPoint,TcpData>  np1 = Plan::makeBVS("", p1, 11075.910238, 0.000000);
	lpc.add(np1);
	Velocity vin2 = Velocity::mkTrkGsVs(-1.049556,67.672453,14.622405);
	//NavPoint src2 =  NavPoint::makeLatLonAlt(29.78600000, -95.36829700, 7826.86574400, 11079.994941);
	double sRadius2 = vin2.gs()/ 0.083666;
	Position center = KinematicsPosition::centerFromRadius(p2, sRadius2, vin2.trk());
	std::pair<NavPoint,TcpData> np2 = Plan::makeBOT(p2, 11076.829660,  sRadius2, center);   	 lpc.add(np2);
	Velocity vin3 = Velocity::mkTrkGsVs(-0.531790,67.672453,2.245403);
	//NavPoint src3 =  NavPoint::makeLatLonAlt(29.78600000, -95.36829700, 7826.86574400, 11079.994941);
	std::pair<NavPoint,TcpData> np3 = Plan::makeEOT(p3, 11083.018161);    	 lpc.add(np3);
	Velocity vin4 = Velocity::mkTrkGsVs(-0.531793,67.672453,0.406559);
	//NavPoint src4 =  NavPoint::makeLatLonAlt(29.78600000, -95.36829700, 7826.86574400, 11079.994941);
	std::pair<NavPoint,TcpData> np4 = Plan::makeEVS(p4, 11083.937583);    	 lpc.add(np4);
	Velocity vin5 = Velocity::mkTrkGsVs(-0.531790,67.672453,0.406559);
	//NavPoint src5 =  NavPoint::makeLatLonAlt(29.78600000, -95.36829700, 7826.86574400, 11079.994941);
	std::pair<NavPoint,TcpData> np5 = Plan::makeBGS("", p5, 11084.018161, 2.000000); lpc.add(np5);
	Velocity vin6 = Velocity::mkTrkGsVs(-0.531790,67.672453,0.406559);
	//NavPoint src6 =  NavPoint::makeLatLonAlt(29.78600000, -95.36829700, 7826.86574400, 11079.994941);
	std::pair<NavPoint,TcpData> np6 = Plan::makeEGS(p6, 11112.480156);    	 lpc.add(np6);
	Velocity vin7 = Velocity::mkTrkGsVs(-0.532406,124.596443,0.406559);
	NavPoint src7 =  NavPoint::makeLatLonAlt(29.89400000, -95.44160000, 8000.00000000, 11191.766878);
	//NavPoint np7 = src7.makeBVS(p7, 11195.912662, 0.000000, vin7, -1);
	//std::pair<NavPoint,TcpData> np7 = Plan::makeBVS(src7,p7, 11195.912662, 0.000000, -1);
	lpc.addNavPoint(src7);

	EXPECT_TRUE(lpc.isBVS(1));
	EXPECT_TRUE(lpc.isBOT(2));
	EXPECT_TRUE(lpc.isEOT(3));
	EXPECT_TRUE(lpc.isEVS(4));
	EXPECT_TRUE(lpc.isBGS(5));
	EXPECT_TRUE(lpc.isEGS(6));
	//bool killNextGsTCPs = true;
 	lpc.revertTurnTCP(2); // , killNextGsTCPs);
    //fpln(" $$$$$$$$$$$ AFTER 1 lpc = "+lpc.toStringGs());
 	EXPECT_TRUE(lpc.isWellFormed());
 	EXPECT_TRUE(lpc.isTurnConsistent(true));
	EXPECT_TRUE(lpc.isBVS(1));
	NavPoint kp2 =  lpc.point(2);
	EXPECT_FALSE(lpc.hasError());   // DEBUG
	//fpln(" $$$$$$$$$$$ BEFORE lpc = "+lpc.toString());
    std::string name = lpc.getID();
    std::string note = lpc.getNote();
    //DebugSupport.dumpPlan(lpc, "testStructuralRevertTCPs_BEFORE2.txt");
    PlanUtil::revertGroupOfTCPs(lpc,1);  // BVS
    EXPECT_EQ(name,lpc.getID());
    EXPECT_EQ(note,lpc.getNote());
    //fpln(" $$$$$$$$$$$ AFTER 2 lpc = "+lpc.toStringGs());
    //DebugSupport.dumpPlan(lpc, "test_structRevertGroupOfTCPs_AFTER2.txt");
    EXPECT_TRUE(lpc.isWellFormed());
	EXPECT_TRUE(lpc.isConsistent());

 }

TEST_F(PlanUtilTest, test_revertTurnTCPs) {
	Plan UF2303("UF2303");
	Position p0  = Position::makeLatLonAlt(32.763598000000, -97.2653960000000000, 600.000000000000);
	Position p1  = Position::makeLatLonAlt(32.763598000000, -97.2653960000000000, 650.000000000000);
	Position p2  = Position::makeLatLonAlt(32.764508568970, -97.2623190431280000, 696.755350481078);
	Position p3  = Position::makeLatLonAlt(32.789859219570, -97.1765275463450000, 1999.999999999628);
	Position p4  = Position::makeLatLonAlt(32.789859219570, -97.1765275463450000, 2000.000000000000);
	Position p5  = Position::makeLatLonAlt(32.789964045380, -97.1761722825770000, 2000.000000000000);
	Position p6  = Position::makeLatLonAlt(32.794207780379, -97.1617863375160000, 2000.000000000000);
	Position p7  = Position::makeLatLonAlt(32.812273663375, -97.1067581431540000, 2000.000000000000);
	Position p8  = Position::makeLatLonAlt(32.827246650201, -97.0654326815870000, 2000.000000000000);
	Position p9  = Position::makeLatLonAlt(32.827486101290, -97.0647338991170000, 2000.000000000000);
	Position p10  = Position::makeLatLonAlt(32.827702287256, -97.0640244543780000, 2000.000000000000);
	Position p11  = Position::makeLatLonAlt(32.842867882395, -97.0109801649890000, 2000.000000000000);
	NavPoint np0(p0,46932.7260119239100000);    	 UF2303.addNavPoint(np0);
	NavPoint np1(p1,46938.7260119239100000);    	 UF2303.addNavPoint(np1);
	UF2303.setInfo(1,"BOC");
	NavPoint np2(p2,46944.3366539816340000);    	 UF2303.addNavPoint(np2);
	UF2303.setInfo(2,"depFix");
	NavPoint np3(p3,47100.7260119238600000);    	 UF2303.addNavPoint(np3);
	NavPoint np4(p4,47100.7260119239100000);    	 UF2303.addNavPoint(np4);
	UF2303.setInfo(4,"TOC");
	NavPoint np5(p5,47101.1144671015400000);    	 UF2303.addNavPoint(np5);
	NavPoint np6(p6,47116.8436860364000000);    	 UF2303.addNavPoint(np6);
	NavPoint np7(p7,47177.7853138750700000);    	 UF2303.addNavPoint(np7);
	Position turnCenter8 = Position::makeLatLonAlt(32.812430343152, -97.0578363580940000, 2000.000000000000);
	std::pair<NavPoint,TcpData> np8 = Plan::makeBOT(p8,47224.199907108305, 1792.706013490160, turnCenter8);
	UF2303.add(np8);
	NavPoint np9(p9,47224.9782181440900000);    	 UF2303.addNavPoint(np9);
	std::pair<NavPoint,TcpData> np10 = Plan::makeEOT( p10,47225.756529179830);    	 UF2303.add(np10);
	NavPoint np11(p11,47283.5354074120840000);    	 UF2303.addNavPoint(np11);
	//fpln(" $$$ test_revertTurnTCPs: UF2303 = "+UF2303);
	int ixTOC = UF2303.findInfo("TOC", true);	// TOC index
	EXPECT_EQ(4,ixTOC);

	Plan revPlan(UF2303);
	revPlan.revertTurnTCPs();
	//fpln(" $$$ test_revertTurnTCPs: revPlan = "+revPlan);
	ixTOC = revPlan.findInfo("TOC", true);	// TOC index
	EXPECT_EQ(3,ixTOC);
}

TEST_F(PlanUtilTest, test_fixGsAccel) {
	Plan kpc("test_fixGsAccel");
	Position p6  = Position::makeLatLonAlt(32.676424439024, -96.637885258343, 1401.153339653128);
	Position p7  = Position::makeLatLonAlt(32.711959424678, -96.677592917967, 1440.730009479692);
	Position p8  = Position::makeLatLonAlt(32.737807746296, -96.706505141119, 1469.527036357765);
	Position p9  = Position::makeLatLonAlt(32.739466111766, -96.708563825682, 1471.473241469816);
	Position p10  = Position::makeLatLonAlt(32.740936243943, -96.710815642281, 1469.476154104439);
	Position p11  = Position::makeLatLonAlt(32.759710001276, -96.742888706773, 1424.163578366671);
	Position p12  = Position::makeLatLonAlt(32.774895174720, -96.768851157710, 1385.486553947035);
	NavPoint np6(p6,1070.0000000000000000);     TcpData tcp6;
	kpc.add(np6, tcp6);
	NavPoint np7(p7,1120.0000000000000000);     TcpData tcp7;
	kpc.add(np7, tcp7);
	NavPoint np8(p8,1156.3813163212947000);     TcpData tcp8;
	Position turnCenter8 = Position::makeLatLonAlt(32.721954753813, -96.726522578793, 1372.000000000000);
	tcp8.setBOT(-2569.919064571338, turnCenter8);
	       int ix8 = kpc.add(np8, tcp8);
	kpc.setVertexRadius(ix8,-2569.9190645713384);
	NavPoint np9(p9,1158.8400945871010000);     TcpData tcp9;
	tcp9.setMOT(true); tcp9.setBGS(-4.000000000000);
	       kpc.add(np9, tcp9);
	NavPoint np10(p10,1161.4218435651360000);     TcpData tcp10;
	tcp10.setEOT();        kpc.add(np10, tcp10);
	NavPoint np11(p11,1220.0000000000000000);     TcpData tcp11;
	tcp11.setEGS();        int ix11 = kpc.add(np11, tcp11);
	kpc.setInfo(ix11,"addEGS(e)");
	NavPoint np12(p12,1270.0000000000000000);     TcpData tcp12;
	kpc.add(np12, tcp12);

	int ixBGS = 3;
	EXPECT_FALSE(kpc.isGsConsistent(false));
	//fpln(" $$$ test_fixGsAccel: kpc = "+kpc.toStringGs());
	kpc.repairGsConsistencyAt(ixBGS);
	EXPECT_TRUE(kpc.isGsConsistent(false));

}


TEST_F(PlanUtilTest, test_fixGsAccel_2) {
	Position p0(LatLonAlt::make(44.1926, 18.9991, 10000));
	Position p1(LatLonAlt::make(44.2067, 19.0153, 10000));
	NavPoint np0(p0,0.0);
	NavPoint np1(p1,100.0);
	Plan p("");
	p.addNavPoint(np0);
	p.addNavPoint(np1);
	NavPoint np2 = p.navPt(50.0);
	p.addNavPoint(np2);
    p.setBGS(0,-1.980665000000);
    p.setEGS(2);
	//fpln(" $$$ test_fixGsAccel_2: p = "+p.toStringGs());
    p.setBGS(00,-1.980665000000);
	EXPECT_FALSE(p.isGsConsistent(false));
	//fpln(" $$$ test_fixGsAccel: BEFORE p = "+p.toStringGs());
	//PlanUtil::fixGsAccels(p);
	p.repairGsConsistencyAt(0);
	//fpln(" $$$ test_fixGsAccel: AFTER p = "+p.toStringGs());
	EXPECT_TRUE(p.isGsConsistent(false));
	Plan saveP = p.copy();
	//fpln(" %%%%%%%%%%%%%%%%%%%");
	for (double nt = 1.0; nt < 100.0; nt = nt + 1.0) {
		p = saveP.copy();
		p.setTime(1,nt);
		//fpln(" $$$ nt = "+nt+" consistent = "+p.isGsConsistent(true));
		p.repairGsConsistency();
		EXPECT_TRUE(p.isGsConsistent(false));
	}
    NavPoint navPt = saveP.navPt(49.999);
    saveP.addNavPoint(navPt);
    saveP.setGsAccel(0,-05.0);
    saveP.repairGsConsistency();

	//fpln(" $$$ test_fixGsAccel: AFTER saveP = "+saveP.toStringGs());
	for (double nt = 1.0; nt < 100.0; nt = nt + 1.0) {
		p = saveP.copy();
		p.setTime(2,nt);
		//fpln(" $$$ test_fixGsAccel: AFTER saveP = "+saveP.toStringGs());
		//fpln(" $$$ nt = "+nt+" p.gsInCalc(2) = "+p.gsInCalc(2));
		p.repairNegGsIn(2);
		p.repairNegGsIn(3);
		//fpln(" AFTER               $$$ nt = "+nt+" p.gsInCalc(2) = "+p.gsInCalc(2));
		EXPECT_TRUE(p.isGsConsistent(false));
	}


}





//
//
//TEST_F(PlanUtilTest, test_tcpa) {
//Plan lpc1("1");
//Position p0(LatLonAlt::make(44.114290, -96.621387, 29537.789052));
//Position p1(LatLonAlt::make(43.280957, -97.454717, 22094.206730));
//Position p2(LatLonAlt::make(43.114290, -97.454717, 21488.520711));
//Position p3(LatLonAlt::make(42.780957, -97.788049, 19987.308798));
//Position p4(LatLonAlt::make(42.614290, -97.788048, 19381.622768));
//Position p5(LatLonAlt::make(42.280957, -97.454713, 17876.161400));
//Position p6(LatLonAlt::make(41.947623, -97.454712, 16664.789314));
//Position p7(LatLonAlt::make(41.280956, -98.121376, 13639.824638));
//Position p8(LatLonAlt::make(41.280956, -98.288043, 13184.661714));
//Position p9(LatLonAlt::make(40.447623, -99.121373, 9387.766195));
//Position p10(LatLonAlt::make(40.447623, -99.288040, 8926.839632));
//Position p11(LatLonAlt::make(39.643486, -100.164379, 5121.117843));
//NavPoint np0(p0,145.000000);     lpc1.addNavPoint(np0);
//NavPoint np1(p1,869.693358);     lpc1.addNavPoint(np1);
//NavPoint np2(p2,987.150642);     lpc1.addNavPoint(np2);
//NavPoint np3(p3,1278.272227);     lpc1.addNavPoint(np3);
//NavPoint np4(p4,1395.729512);     lpc1.addNavPoint(np4);
//NavPoint np5(p5,1687.675170);     lpc1.addNavPoint(np5);
//NavPoint np6(p6,1922.589746);     lpc1.addNavPoint(np6);
//NavPoint np7(p7,2509.204136);     lpc1.addNavPoint(np7);
//NavPoint np8(p8,2597.471322);     lpc1.addNavPoint(np8);
//NavPoint np9(p9,3333.781919);     lpc1.addNavPoint(np9);
//NavPoint np10(p10,3423.166815);     lpc1.addNavPoint(np10);
//NavPoint np11(p11,4161.189041);     lpc1.addNavPoint(np11);
//
//Plan lpc2("2");
//   p0(LatLonAlt::make(41.850115, -101.358910, 30000.000000));
//   p1(LatLonAlt::make(42.016782, -100.692245, 30000.000000));
//   p2(LatLonAlt::make(42.183448, -100.525579, 30000.000000));
//   p3(LatLonAlt::make(42.183448, -100.025579, 30000.000000));
//   p4(LatLonAlt::make(42.683449, -99.525581, 30000.000000));
//   p5(LatLonAlt::make(42.683449, -99.358914, 30000.000000));
//   p6(LatLonAlt::make(42.850115, -99.192248, 30000.000000));
//   p7(LatLonAlt::make(42.850115, -98.858915, 30000.000000));
//   p8(LatLonAlt::make(43.016782, -98.692249, 30000.000000));
//   p9(LatLonAlt::make(43.016782, -98.525582, 30000.000000));
//   p10(LatLonAlt::make(42.850115, -98.358915, 30000.000000));
//   p11(LatLonAlt::make(42.850115, -97.525581, 30000.000000));
//  Position p12(LatLonAlt::make(43.183449, -97.192249, 30000.000000));
//  Position p13(LatLonAlt::make(43.183449, -96.692249, 30000.000000));
//  Position p14(LatLonAlt::make(42.850115, -96.358915, 30000.000000));
//  Position p15(LatLonAlt::make(42.850115, -95.192248, 30000.000000));
//  Position p16(LatLonAlt::make(43.016782, -95.025582, 30000.000000));
//  Position p17(LatLonAlt::make(43.016782, -94.192249, 30000.000000));
//  Position p18(LatLonAlt::make(43.183449, -94.025583, 30000.000000));
//  Position p19(LatLonAlt::make(43.183449, -93.858916, 30000.000000));
//  Position p20(LatLonAlt::make(43.516782, -93.525584, 30000.000000));
//  Position p21(LatLonAlt::make(43.516782, -93.358917, 30000.000000));
//  Position p22(LatLonAlt::make(43.683449, -93.192251, 30000.000000));
//  Position p23(LatLonAlt::make(43.683449, -93.025585, 30000.000000));
//  Position p24(LatLonAlt::make(44.016782, -92.692253, 30000.000000));
//  Position p25(LatLonAlt::make(44.016782, -92.525586, 30000.000000));
//  Position p26(LatLonAlt::make(44.350115, -92.192254, 30000.000000));
//  Position p27(LatLonAlt::make(44.350115, -92.025587, 30000.000000));
//  Position p28(LatLonAlt::make(44.516782, -91.858921, 30000.000000));
//  Position p29(LatLonAlt::make(44.516782, -91.692255, 30000.000000));
//  Position p30(LatLonAlt::make(44.850115, -91.358923, 30000.000000));
//  Position p31(LatLonAlt::make(44.850115, -91.192256, 30000.000000));
//  Position p32(LatLonAlt::make(45.016782, -91.025590, 30000.000000));
//  Position p33(LatLonAlt::make(45.016782, -90.858924, 30000.000000));
//  Position p34(LatLonAlt::make(45.350115, -90.525592, 30000.000000));
//  Position p35(LatLonAlt::make(45.350115, -90.358925, 30000.000000));
//  Position p36(LatLonAlt::make(45.516782, -90.192259, 30000.000000));
//  Position p37(LatLonAlt::make(45.516782, -90.025592, 30000.000000));
//  Position p38(LatLonAlt::make(45.683449, -89.858926, 30000.000000));
//  Position p39(LatLonAlt::make(45.683449, -89.692260, 30000.000000));
//  Position p40(LatLonAlt::make(45.850115, -89.525594, 30000.000000));
//  Position p41(LatLonAlt::make(45.850115, -89.358927, 30000.000000));
//  Position p42(LatLonAlt::make(46.016782, -89.192261, 30000.000000));
//  Position p43(LatLonAlt::make(45.999147, -88.928451, 30000.000000));
//   np0(p0,545.000000);     lpc2.addNavPoint(np0);
//   np1(p1,761.277403);     lpc2.addNavPoint(np1);
//   np2(p2,847.066093);     lpc2.addNavPoint(np2);
//   np3(p3,1000.220427);     lpc2.addNavPoint(np3);
//   np4(p4,1257.104394);     lpc2.addNavPoint(np4);
//   np5(p5,1307.750237);     lpc2.addNavPoint(np5);
//   np6(p6,1393.217731);     lpc2.addNavPoint(np6);
//   np7(p7,1494.237205);     lpc2.addNavPoint(np7);
//   np8(p8,1579.624113);     lpc2.addNavPoint(np8);
//   np9(p9,1629.997367);     lpc2.addNavPoint(np9);
//   np10(p10,1715.384510);     lpc2.addNavPoint(np10);
//   np11(p11,1967.932330);     lpc2.addNavPoint(np11);
//  NavPoint np12(p12,2138.625221);     lpc2.addNavPoint(np12);
//  NavPoint np13(p13,2289.333978);     lpc2.addNavPoint(np13);
//  NavPoint np14(p14,2460.027340);     lpc2.addNavPoint(np14);
//  NavPoint np15(p15,2813.592904);     lpc2.addNavPoint(np15);
//  NavPoint np16(p16,2898.979812);     lpc2.addNavPoint(np16);
//  NavPoint np17(p17,3150.845087);     lpc2.addNavPoint(np17);
//  NavPoint np18(p18,3236.151299);     lpc2.addNavPoint(np18);
//  NavPoint np19(p19,3286.387618);     lpc2.addNavPoint(np19);
//  NavPoint np20(p20,3456.757293);     lpc2.addNavPoint(np20);
//  NavPoint np21(p21,3506.718469);     lpc2.addNavPoint(np21);
//  NavPoint np22(p22,3591.781956);     lpc2.addNavPoint(np22);
//  NavPoint np23(p23,3641.604925);     lpc2.addNavPoint(np23);
//  NavPoint np24(p24,3811.488229);     lpc2.addNavPoint(np24);
//  NavPoint np25(p25,3861.033522);     lpc2.addNavPoint(np25);
//  NavPoint np26(p26,4030.591603);     lpc2.addNavPoint(np26);
//  NavPoint np27(p27,4079.857543);     lpc2.addNavPoint(np27);
//  NavPoint np28(p28,4164.514516);     lpc2.addNavPoint(np28);
//  NavPoint np29(p29,4213.640154);     lpc2.addNavPoint(np29);
//  NavPoint np30(p30,4382.709040);     lpc2.addNavPoint(np30);
//  NavPoint np31(p31,4431.552827);     lpc2.addNavPoint(np31);
//  NavPoint np32(p32,4515.964817);     lpc2.addNavPoint(np32);
//  NavPoint np33(p33,4564.667058);     lpc2.addNavPoint(np33);
//  NavPoint np34(p34,4733.245235);     lpc2.addNavPoint(np34);
//  NavPoint np35(p35,4781.663149);     lpc2.addNavPoint(np35);
//  NavPoint np36(p36,4865.829432);     lpc2.addNavPoint(np36);
//  NavPoint np37(p37,4914.104568);     lpc2.addNavPoint(np37);
//  NavPoint np38(p38,4998.188801);     lpc2.addNavPoint(np38);
//  NavPoint np39(p39,5046.320749);     lpc2.addNavPoint(np39);
//  NavPoint np40(p40,5130.322861);     lpc2.addNavPoint(np40);
//  NavPoint np41(p41,5178.311215);     lpc2.addNavPoint(np41);
//  NavPoint np42(p42,5262.231138);     lpc2.addNavPoint(np42);
//  NavPoint np43(p43,5338.324234);     lpc2.addNavPoint(np43);
//
//  double tcpa1 = PlanUtil::timeOfClosestApproachIterative(lpc1, lpc2, 0.0, MAXDOUBLE, 0.1);
//  double tcpa2 = PlanUtil::timeOfClosestApproachLinear(lpc1, lpc2, 0.0, MAXDOUBLE);
//  EXPECT_NEAR(tcpa1,tcpa2,0.1);
//
//   tcpa1 = PlanUtil::timeOfClosestApproachIterative(lpc1, lpc2, 500, 800, 0.1);
//   tcpa2 = PlanUtil::timeOfClosestApproachLinear(lpc1, lpc2, 500, 800);
//  EXPECT_NEAR(tcpa1,tcpa2,0.1);
//
//  p0 = Position::makeLatLonAlt(10.0, 0.0, 0.0);
//  p1 = Position::makeLatLonAlt(10.0, 50.0, 0.0);
//  p2 = Position::makeLatLonAlt(15.0, 0.0, 0.0);
//  p3 = Position::makeLatLonAlt(10.1, 50.0, 0.0);
//
//  // both arrive at nearly the same point
//  np0(p0, 0.0);
//  np1(p1, 3600.0);
//  np2(p2, 0.0);
//  np3(p3, 3600.0);
//
//  lpc1("1");
//  lpc2("2");
//  lpc1.addNavPoint(np0);
//  lpc1.addNavPoint(np1);
//  lpc2.addNavPoint(np2);
//  lpc2.addNavPoint(np3);
//
//	  // the initial point is closer to a great circle intersection, but the actual tcpa is at the end of the plans.
//
//   tcpa1 = PlanUtil::timeOfClosestApproachIterative(lpc1, lpc2, 0, MAXDOUBLE, 1.0);
//   tcpa2 = PlanUtil::timeOfClosestApproachLinear(lpc1, lpc2, 0, MAXDOUBLE);
//  EXPECT_NEAR(tcpa1,tcpa2,1.0);
//
//  p0 = Position::makeLatLonAlt(10.0, 0.0, 0.0);
//  p1 = Position::makeLatLonAlt(10.0, 50.0, 0.0);
//  p2 = Position::makeLatLonAlt(10.1, 0.0, 0.0);
//  p3 = Position::makeLatLonAlt(15.0, 50.0, 0.0);
//
//  // both arrive at nearly the same point
//  np0(p0, 0.0);
//  np1(p1, 3600.0);
//  np2(p2, 0.0);
//  np3(p3, 3600.0);
//
//  lpc1("1");
//  lpc2("2");
//  lpc1.addNavPoint(np0);
//  lpc1.addNavPoint(np1);
//  lpc2.addNavPoint(np2);
//  lpc2.addNavPoint(np3);
//
//  fpln("---");
//	  // the opposite of the above.  The end is closer to the intersection, but not on the segment.
//
//   tcpa1 = PlanUtil::timeOfClosestApproachIterative(lpc1, lpc2, 0, MAXDOUBLE, 1.0);
//   tcpa2 = PlanUtil::timeOfClosestApproachLinear(lpc1, lpc2, 0, MAXDOUBLE);
//  EXPECT_NEAR(tcpa1,tcpa2,1.0);
//}






TEST_F(PlanUtilTest, testLinearMakeGsConstant) {
	Position p0(LatLonAlt::make(-42.859615, -77.023113, 31382.378806));
	Position p1(LatLonAlt::make(-42.526281, -77.356448, 31305.965063));
	Position p2(LatLonAlt::make(-42.526281, -77.689781, 31550.325172));
	Position p3(LatLonAlt::make(-42.359615, -77.856448, 31756.367526));
	Position p4(LatLonAlt::make(-42.359615, -78.523115, 32246.388618));
	NavPoint np0(p0,880);     lpc.addNavPoint(np0);
	NavPoint np1(p1,1000);     lpc.addNavPoint(np1);
	NavPoint np2(p2,1200);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1400);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1700);     lpc.addNavPoint(np4);
	double gs = Units::from("kn",500.0);
    std::string name = lpc.getID();
    std::string note = lpc.getNote();
	Plan rrPlan = PlanUtil::mkGsConstant(lpc, gs);
	for (int j= 0; j < rrPlan.size()-1; j++) {
		NavPoint np = rrPlan.point(j);
		//fpln(" gs = "+lpc.initialVelocity(j).gs());
		EXPECT_EQ(rrPlan.point(j).position(),lpc.point(j).position());
		EXPECT_NEAR(gs,rrPlan.initialVelocity(j).gs(),0.001);
	}
	EXPECT_EQ(name,lpc.getID());
	EXPECT_EQ(note,lpc.getNote());
}



TEST_F(PlanUtilTest, testLinearMakeGSConstant2) {
	Position p0(Position::makeXYZ(0.000000, 0.000000, 10000.000000));
	Position p1(Position::makeXYZ(10.000000, 1.500000, 10010.000000));
	Position p2(Position::makeXYZ(0.000000, 3.000000, 10020.000000));
	Position p3(Position::makeXYZ(0.000000, 7.000000, 10030.000000));
	Position p4(Position::makeXYZ(10.000000, 7.000000, 10040.000000));
	Position p5(Position::makeXYZ(10.000000, 9.000000, 10050.000000));
	NavPoint np0 = NavPoint(p0,0.000000);    	 lpc.addNavPoint(np0);
	NavPoint np1 = NavPoint(p1,200);    	     lpc.addNavPoint(np1);
	NavPoint np2 = NavPoint(p2,404.474968);    	 lpc.addNavPoint(np2);
	NavPoint np3 = NavPoint(p3,444.474968);    	 lpc.addNavPoint(np3);
	NavPoint np4 = NavPoint(p4,564.474968);    	 lpc.addNavPoint(np4);
	NavPoint np5 = NavPoint(p5,604.474968);    	 lpc.addNavPoint(np5);
	lpc =  PlanUtil::mkGsConstant(lpc,0,2);
    //f.pln(" ---------");
	for (int i = 0; i < lpc.size(); i++) {
		//f.pln(" $$$$... "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units.str("kn", lpc.initialVelocity(i).gs()));
		if (i < 2 || i >= 4) {
			EXPECT_NEAR(Units::from("kn", 180),lpc.initialVelocity(i).gs(),0.0001);
		} else if (i == 2) {
			EXPECT_NEAR(Units::from("kn", 360),lpc.initialVelocity(i).gs(),0.0001);
		} else {
			EXPECT_NEAR(Units::from("kn", 300),lpc.initialVelocity(i).gs(),0.0001);

		}
	}
	Plan kpc =  PlanUtil::mkGsConstant(lpc,2,4,Units::from("kn", 99.0));
//		for (int i = 0; i < kpc.size(); i++) {
//			f.pln(" $$$$ "+i+" "+" "+kpc.point(i)+"   >> gs = "+Units.str("kn", kpc.initialVelocity(i).gs()));
//		}
	for (int i = 0; i < lpc.size(); i++) {
		//f.pln(" $$$$... "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units.str("kn", lpc.initialVelocity(i).gs()));
		if (i >= 2 && i < 4) {
			EXPECT_NEAR(Units::from("kn", 99.0),kpc.initialVelocity(i).gs(),0.0001);
		} else {
			EXPECT_NEAR(lpc.initialVelocity(i).gs(),kpc.initialVelocity(i).gs(),0.0001);
		}
	}
}

TEST_F(PlanUtilTest, test_LinearMakeVsConstant) {
	  Position p0(LatLonAlt::make(-42.859615, -77.023113, 31000));
	  Position p1(LatLonAlt::make(-42.526281, -77.356448, 32000));
	  Position p2(LatLonAlt::make(-42.526281, -77.689781, 31));
	  Position p3(LatLonAlt::make(-42.359615, -77.856448, 31));
	  Position p4(LatLonAlt::make(-42.359615, -78.523115, 31));
	  NavPoint np0 = NavPoint(p0,880);      lpc.addNavPoint(np0);
	  NavPoint np1 = NavPoint(p1,1000);     lpc.addNavPoint(np1);
	  NavPoint np2 = NavPoint(p2,1200);     lpc.addNavPoint(np2);
	  NavPoint np3 = NavPoint(p3,1400);     lpc.addNavPoint(np3);
	  NavPoint np4 = NavPoint(p4,1700);     lpc.addNavPoint(np4);
	  double vs0 = Units::from("fpm",2500.0);
	  std::string name = lpc.getID();
	  std::string note = lpc.getNote();
	  PlanUtil::mkVsConstant(lpc, vs0);
	  for (int j= 0; j < lpc.size()-1; j++) {
		  NavPoint np = lpc.point(j);
		  //fpln(" vs = "+lpc.initialVelocity(j).vs());
		  EXPECT_NEAR(vs0,lpc.initialVelocity(j).vs(),0.00001);
	  }
	  //f.pln(" lpc = "+lpc.getName()+" =? "+name);
	  EXPECT_EQ(name,lpc.getID());
	  EXPECT_EQ(note,lpc.getNote());
}



TEST_F(PlanUtilTest, testUnZigZag) {
	Position p0(LatLonAlt::make(-42.859615, -77.023113, 31382.378806));
	Position p1(LatLonAlt::make(-42.526281, -77.356448, 31305.965063));
	Position p2(LatLonAlt::make(-42.526281, -77.689781, 31550.325172));
	Position p3(LatLonAlt::make(-42.359615, -77.856448, 31756.367526));
	Position p4(LatLonAlt::make(-42.359615, -78.523115, 32246.388618));
	Position p5(LatLonAlt::make(-42.526281, -78.689781, 32452.430408));
	Position p6(LatLonAlt::make(-42.526281, -78.856448, 32574.610521));
	Position p7(LatLonAlt::make(-42.692948, -79.023114, 32780.458922));
	Position p8(LatLonAlt::make(-42.692948, -79.189780, 32902.312548));
	Position p9(LatLonAlt::make(-43.026281, -79.523112, 33313.427539));
	Position p10(LatLonAlt::make(-43.526281, -79.523110, 33810.790703));
	Position p11(LatLonAlt::make(-43.859615, -79.856442, 34219.959891));
	Position p12(LatLonAlt::make(-44.157355, -79.758501, 34524.306047));
	NavPoint np0(p0,887.108300);     lpc.addNavPoint(np0);
	NavPoint np1(p1,1127.901259);     lpc.addNavPoint(np1);
	NavPoint np2(p2,1270.889470);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1391.455907);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1678.193541);     lpc.addNavPoint(np4);
	NavPoint np5(p5,1798.759648);     lpc.addNavPoint(np5);
	NavPoint np6(p6,1870.253788);     lpc.addNavPoint(np6);
	NavPoint np7(p7,1990.706733);     lpc.addNavPoint(np7);
	NavPoint np8(p8,2062.009828);     lpc.addNavPoint(np8);
	NavPoint np9(p9,2302.575269);     lpc.addNavPoint(np9);
	NavPoint np10(p10,2593.609143);     lpc.addNavPoint(np10);
	NavPoint np11(p11,2833.035990);     lpc.addNavPoint(np11);
	NavPoint np12(p12,3011.125256);     lpc.addNavPoint(np12);
	//DebugSupport.dumpPlan(lpc, "unZigZag_lpc");
	Plan unPlan = PlanUtil::unZigZag(lpc);
	EXPECT_EQ(lpc.getID(),unPlan.getID());
	EXPECT_EQ(lpc.getNote(),unPlan.getNote());


	//DebugSupport.dumpPlan(unPlan, "unZigZag_kpc");
	EXPECT_EQ(9,unPlan.size());
	EXPECT_EQ(lpc.point(0), unPlan.point(0));
	EXPECT_EQ(lpc.point(lpc.size()-1).position(), unPlan.point(unPlan.size()-1).position());
	EXPECT_EQ(lpc.point(1).position(), unPlan.point(1).position());
	EXPECT_EQ(lpc.point(4).position(), unPlan.point(3).position());
	EXPECT_EQ(lpc.point(8).position(), unPlan.point(5).position());
}


TEST_F(PlanUtilTest, testUnZigZag2) {
	Position p0(LatLonAlt::make(41.850115, -101.358910, 30000.000000));
	Position p1(LatLonAlt::make(42.016782, -100.692245, 30000.000000));
	Position p2(LatLonAlt::make(42.183448, -100.525579, 30000.000000));
	Position p3(LatLonAlt::make(42.183448, -100.025579, 30000.000000));
	Position p4(LatLonAlt::make(42.683449, -99.525581, 30000.000000));
	Position p5(LatLonAlt::make(42.683449, -99.358914, 30000.000000));
	Position p6(LatLonAlt::make(42.850115, -99.192248, 30000.000000));
	Position p7(LatLonAlt::make(42.850115, -98.858915, 30000.000000));
	Position p8(LatLonAlt::make(43.016782, -98.692249, 30000.000000));
	Position p9(LatLonAlt::make(43.016782, -98.525582, 30000.000000));
	Position p10(LatLonAlt::make(42.850115, -98.358915, 30000.000000));
	Position p11(LatLonAlt::make(42.850115, -97.525581, 30000.000000));
	Position p12(LatLonAlt::make(43.183449, -97.192249, 30000.000000));
	Position p13(LatLonAlt::make(43.183449, -96.692249, 30000.000000));
	Position p14(LatLonAlt::make(42.850115, -96.358915, 30000.000000));
	Position p15(LatLonAlt::make(42.850115, -95.192248, 30000.000000));
	Position p16(LatLonAlt::make(43.016782, -95.025582, 30000.000000));
	Position p17(LatLonAlt::make(43.016782, -94.192249, 30000.000000));
	Position p18(LatLonAlt::make(43.183449, -94.025583, 30000.000000));
	Position p19(LatLonAlt::make(43.183449, -93.858916, 30000.000000));
	Position p20(LatLonAlt::make(43.516782, -93.525584, 30000.000000));
	Position p21(LatLonAlt::make(43.516782, -93.358917, 30000.000000));
	Position p22(LatLonAlt::make(43.683449, -93.192251, 30000.000000));
	Position p23(LatLonAlt::make(43.683449, -93.025585, 30000.000000));
	Position p24(LatLonAlt::make(44.016782, -92.692253, 30000.000000));
	Position p25(LatLonAlt::make(44.016782, -92.525586, 30000.000000));
	Position p26(LatLonAlt::make(44.350115, -92.192254, 30000.000000));
	Position p27(LatLonAlt::make(44.350115, -92.025587, 30000.000000));
	Position p28(LatLonAlt::make(44.516782, -91.858921, 30000.000000));
	Position p29(LatLonAlt::make(44.516782, -91.692255, 30000.000000));
	Position p30(LatLonAlt::make(44.850115, -91.358923, 30000.000000));
	Position p31(LatLonAlt::make(44.850115, -91.192256, 30000.000000));
	Position p32(LatLonAlt::make(45.016782, -91.025590, 30000.000000));
	Position p33(LatLonAlt::make(45.016782, -90.858924, 30000.000000));
	Position p34(LatLonAlt::make(45.350115, -90.525592, 30000.000000));
	Position p35(LatLonAlt::make(45.350115, -90.358925, 30000.000000));
	Position p36(LatLonAlt::make(45.516782, -90.192259, 30000.000000));
	Position p37(LatLonAlt::make(45.516782, -90.025592, 30000.000000));
	Position p38(LatLonAlt::make(45.683449, -89.858926, 30000.000000));
	Position p39(LatLonAlt::make(45.683449, -89.692260, 30000.000000));
	Position p40(LatLonAlt::make(45.850115, -89.525594, 30000.000000));
	Position p41(LatLonAlt::make(45.850115, -89.358927, 30000.000000));
	Position p42(LatLonAlt::make(46.016782, -89.192261, 30000.000000));
	Position p43(LatLonAlt::make(45.999147, -88.928451, 30000.000000));
	NavPoint np0(p0,545.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,761.277403);     lpc.addNavPoint(np1);
	NavPoint np2(p2,847.066093);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1000.220427);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1257.104394);     lpc.addNavPoint(np4);
	NavPoint np5(p5,1307.750237);     lpc.addNavPoint(np5);
	NavPoint np6(p6,1393.217731);     lpc.addNavPoint(np6);
	NavPoint np7(p7,1494.237205);     lpc.addNavPoint(np7);
	NavPoint np8(p8,1579.624113);     lpc.addNavPoint(np8);
	NavPoint np9(p9,1629.997367);     lpc.addNavPoint(np9);
	NavPoint np10(p10,1715.384510);     lpc.addNavPoint(np10);
	NavPoint np11(p11,1967.932330);     lpc.addNavPoint(np11);
	NavPoint np12(p12,2138.625221);     lpc.addNavPoint(np12);
	NavPoint np13(p13,2289.333978);     lpc.addNavPoint(np13);
	NavPoint np14(p14,2460.027340);     lpc.addNavPoint(np14);
	NavPoint np15(p15,2813.592904);     lpc.addNavPoint(np15);
	NavPoint np16(p16,2898.979812);     lpc.addNavPoint(np16);
	NavPoint np17(p17,3150.845087);     lpc.addNavPoint(np17);
	NavPoint np18(p18,3236.151299);     lpc.addNavPoint(np18);
	NavPoint np19(p19,3286.387618);     lpc.addNavPoint(np19);
	NavPoint np20(p20,3456.757293);     lpc.addNavPoint(np20);
	NavPoint np21(p21,3506.718469);     lpc.addNavPoint(np21);
	NavPoint np22(p22,3591.781956);     lpc.addNavPoint(np22);
	NavPoint np23(p23,3641.604925);     lpc.addNavPoint(np23);
	NavPoint np24(p24,3811.488229);     lpc.addNavPoint(np24);
	NavPoint np25(p25,3861.033522);     lpc.addNavPoint(np25);
	NavPoint np26(p26,4030.591603);     lpc.addNavPoint(np26);
	NavPoint np27(p27,4079.857543);     lpc.addNavPoint(np27);
	NavPoint np28(p28,4164.514516);     lpc.addNavPoint(np28);
	NavPoint np29(p29,4213.640154);     lpc.addNavPoint(np29);
	NavPoint np30(p30,4382.709040);     lpc.addNavPoint(np30);
	NavPoint np31(p31,4431.552827);     lpc.addNavPoint(np31);
	NavPoint np32(p32,4515.964817);     lpc.addNavPoint(np32);
	NavPoint np33(p33,4564.667058);     lpc.addNavPoint(np33);
	NavPoint np34(p34,4733.245235);     lpc.addNavPoint(np34);
	NavPoint np35(p35,4781.663149);     lpc.addNavPoint(np35);
	NavPoint np36(p36,4865.829432);     lpc.addNavPoint(np36);
	NavPoint np37(p37,4914.104568);     lpc.addNavPoint(np37);
	NavPoint np38(p38,4998.188801);     lpc.addNavPoint(np38);
	NavPoint np39(p39,5046.320749);     lpc.addNavPoint(np39);
	NavPoint np40(p40,5130.322861);     lpc.addNavPoint(np40);
	NavPoint np41(p41,5178.311215);     lpc.addNavPoint(np41);
	NavPoint np42(p42,5262.231138);     lpc.addNavPoint(np42);
	NavPoint np43(p43,5338.324234);     lpc.addNavPoint(np43);
	//DebugSupport.dumpPlan(lpc, "unZigZag2_lpc");
	Plan unPlan = PlanUtil::unZigZag(lpc);
	//DebugSupport.dumpPlan(unPlan, "unZigZag2_kpc");
	//fpln(" $$ unPlan = "+unPlan.toString());
	EXPECT_EQ(25,unPlan.size());
	EXPECT_EQ(lpc.point(0), unPlan.point(0));
	EXPECT_EQ(lpc.point(lpc.size()-1).position(), unPlan.point(unPlan.size()-1).position());
	EXPECT_EQ(lpc.point(1).position(), unPlan.point(1).position());
	EXPECT_EQ(lpc.point(4).position(), unPlan.point(3).position());
	EXPECT_EQ(lpc.point(8).position(), unPlan.point(5).position());
	EXPECT_EQ(lpc.point(12).position(), unPlan.point(7).position());
	//Plan rrPlan = PlanUtil::removeCollinearTrk(unPlan);
	//DebugSupport.dumpPlan(unPlan, "unZigZag2_unPlan");
	EXPECT_EQ(lpc.getID(),unPlan.getID());
	EXPECT_EQ(lpc.getNote(),unPlan.getNote());
}


TEST_F(PlanUtilTest, testUnZigZag3) {
	Position p0(LatLonAlt::make(44.114290, -96.621387, 29537.789052));
	Position p1(LatLonAlt::make(43.280957, -97.454717, 22094.206730));
	Position p2(LatLonAlt::make(43.114290, -97.454717, 21488.520711));
	Position p3(LatLonAlt::make(42.780957, -97.788049, 19987.308798));
	Position p4(LatLonAlt::make(42.614290, -97.788048, 19381.622768));
	Position p5(LatLonAlt::make(42.280957, -97.454713, 17876.161400));
	Position p6(LatLonAlt::make(41.947623, -97.454712, 16664.789314));
	Position p7(LatLonAlt::make(41.280956, -98.121376, 13639.824638));
	Position p8(LatLonAlt::make(41.280956, -98.288043, 13184.661714));
	Position p9(LatLonAlt::make(40.447623, -99.121373, 9387.766195));
	Position p10(LatLonAlt::make(40.447623, -99.288040, 8926.839632));
	Position p11(LatLonAlt::make(39.643486, -100.164379, 5121.117843));
	NavPoint np0(p0,145.000000);     lpc.addNavPoint(np0);
	NavPoint np1(p1,869.693358);     lpc.addNavPoint(np1);
	NavPoint np2(p2,987.150642);     lpc.addNavPoint(np2);
	NavPoint np3(p3,1278.272227);     lpc.addNavPoint(np3);
	NavPoint np4(p4,1395.729512);     lpc.addNavPoint(np4);
	NavPoint np5(p5,1687.675170);     lpc.addNavPoint(np5);
	NavPoint np6(p6,1922.589746);     lpc.addNavPoint(np6);
	NavPoint np7(p7,2509.204136);     lpc.addNavPoint(np7);
	NavPoint np8(p8,2597.471322);     lpc.addNavPoint(np8);
	NavPoint np9(p9,3333.781919);     lpc.addNavPoint(np9);
	NavPoint np10(p10,3423.166815);     lpc.addNavPoint(np10);
	NavPoint np11(p11,4161.189041);     lpc.addNavPoint(np11);
	//DebugSupport.dumpPlan(lpc, "unZigZag3_lpc");
	//fpln(" $$ lpc = "+lpc.getName());
	Plan unPlan = PlanUtil::unZigZag(lpc);
	double gs = Units::from("kn", 400);
	unPlan = PlanUtil::mkGsConstant(unPlan, gs);
	unPlan = PlanUtil::removeCollinearTrk(unPlan, Units::from("deg",2.0));
	//DebugSupport.dumpPlan(unPlan, "unZigZag3_kpc");
	//fpln(" $$ unPlan = "+unPlan.getName());
	EXPECT_EQ(7,unPlan.size());
	EXPECT_EQ(lpc.point(0), unPlan.point(0));
	EXPECT_EQ(lpc.point(lpc.size()-1).position(), unPlan.point(unPlan.size()-1).position());
	for (int j= 0; j < unPlan.size()-1; j++) {
		Velocity v = unPlan.initialVelocity(j);
		EXPECT_NEAR(gs,v.gs(),0.001);
	}
	EXPECT_EQ(lpc.getID(),unPlan.getID());
	EXPECT_EQ(lpc.getNote(),unPlan.getNote());
}



double calcSpline(double c0, double c1, double c2, double dt) {
	return c0 +c1*dt + c2*dt*dt;
}


TEST_F(PlanUtilTest, testfixGsAccelAt) {
	Plan lpc("");
	Position p0(LatLonAlt::make(-42.819, -76.932, 31396.6));
	Position pN(LatLonAlt::make(-42.512, -77.493, 31387.1));
	double last = 7;
	for (int i = 0; i < last; i++) {
		double f = 1.0*i/last;
		Position pi = p0.interpolate(pN,f);
		lpc.addNavPoint(NavPoint(pi,std::sqrt(2000.0*i)));
	}
	double gsAccel = 2;
	for (int i = 0; i < last; i++) {
		//f.p(" $$$$ "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units::str("kn", lpc.initialVelocity(i).gs()));
		//fpln(" $$$$ enoughTime = "+PlanUtil::enoughDistanceForAccel(lpc,i,gsAccel,0.0).first);
	}
	//fpln("--------------------------------------------------------");
	PlanUtil::fixGsAccelAt(lpc,1,gsAccel,true,0.0);
	PlanUtil::fixGsAccelAt(lpc,4,gsAccel,true,0.0);
	//fpln("--------------------------------------------------------");
	for (int i = 0; i < last; i++) {
		//f.p(" $$$$ "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units::str("kn", lpc.initialVelocity(i).gs()));
		//fpln(" $$$$ enoughTime = "+PlanUtil::enoughDistanceForAccel(lpc,i,gsAccel,0.0).first);
	}
}


TEST_F(PlanUtilTest, testfixGsAccelAt2) {
	Plan lpc("");
	Position p0(LatLonAlt::make(-42.819, -76.932, 31396.6));
	Position pN(LatLonAlt::make(-42.512, -77.493, 31387.1));
	double last = 6;
	for (int i = 0; i <= last; i++) {
		double f = 1.0*i/last;
		Position pi = p0.interpolate(pN,f);
		lpc.addNavPoint(NavPoint(pi,150 - std::sqrt(2500.0*i)));
	}
	double gsAccel = 2;
	for (int i = 0; i <= last; i++) {
		//f.p(" $$$$ "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units::str("kn", lpc.initialVelocity(i).gs()));
		//fpln(" $$$$ enoughTime = "+PlanUtil::enoughDistanceForAccel(lpc,i,gsAccel,0.0).first);
	}
	//fpln("--------------------------------------------------------");
	PlanUtil::fixGsAccelAt(lpc,2,gsAccel,true,0.0);
	//PlanUtil::fixGsAccelAt(lpc,3,gsAccel);
	//PlanUtil::fixGsAccelAt(lpc,4,gsAccel);
	//PlanUtil::fixGsAccelAt(lpc,5,gsAccel);
	//fpln("--------------------------------------------------------");
	for (int i = 0; i <= last; i++) {
		//f.p(" $$$$ "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units::str("kn", lpc.initialVelocity(i).gs()));
		//fpln(" $$$$ enoughTime = "+PlanUtil::enoughDistanceForAccel(lpc,i,gsAccel,0.0).first);
	}
}


//TEST_F(PlanUtilTest, testfixGsAccelAt3) {
//	Plan lpc("");
//	Position p13(LatLonAlt::make(34.040114, -117.330992, 12654.170337));
//	Position p14(LatLonAlt::make(34.034425, -117.390881, 12274.777897));
//	Position p15(LatLonAlt::make(34.032768, -117.408944, 12125.000000));
//	Position p16(LatLonAlt::make(34.028381, -117.456617, 11552.587777));
//	Position p17(LatLonAlt::make(33.942496, -118.408068, 125.000000));
//	NavPoint np13(p13,40436.890235); np13 = np13.makeName("HABSO");    	 lpc.addNavPoint(np13);
//	NavPoint np14(p14,40459.653781); np14 = np14.makeName("CIVET");    	 lpc.addNavPoint(np14);
//	NavPoint np15(p15,40468.640455); np15 = np15.makeName("DescentSpeedChangePoint");    	 lpc.addNavPoint(np15);
//	NavPoint np16(p16,40502.985188); np16 = np16.makeName("RIIVR");    	 lpc.addNavPoint(np16);
//	NavPoint np17(p17,41188.640455); np17 = np17.makeName("KLAX");    	 lpc.addNavPoint(np17);
//	double gsAccel = 4.0;
//	for (int i = 0; i < lpc.size(); i++) {
//		//f.p(" $$$$ "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units::str("kn", lpc.initialVelocity(i).gs()));
//		//fpln(" $$$$ enoughTime = "+PlanUtil::enoughDistanceForAccel(lpc,i,gsAccel,0.0).first);
//	}
//	PlanUtil::fixGsAccelAfter(lpc,0,gsAccel,0.0);
//	for (int i = 0; i < lpc.size(); i++) {
//		//f.p(" $$$$ "+i+" "+" "+lpc.point(i)+"   >> gs = "+Units::str("kn", lpc.initialVelocity(i).gs()));
//		//fpln(" $$$$ enoughTime = "+PlanUtil::enoughDistanceForAccel(lpc,i,gsAccel,0.0).first);
//	}
//	PlanUtil::fixGsAccelAfter(lpc,0,gsAccel,0.0);
//	bool repair = false;
//	if (lpc.hasError()) {
//		EXPECT_TRUE(" $$$$$$$$$$$$$ error in speed/vertical profile: error = "+lpc.getMessage());
//	}
//	double vsAccel = 1;
//	double bankAngle = Units::from("deg", 25);
//	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair);
//	if (kpc.hasError()) {
//		EXPECT_TRUE(" !! getMessage = "+kpc.getMessageNoClear());
//	}
//}



TEST_F(PlanUtilTest, testRepairGs) {
	Plan lpc("");
	Position p13(LatLonAlt::make(34.040114, -117.330992, 12654.170337));
	Position p14(LatLonAlt::make(34.034425, -117.390881, 12274.777897));
	Position p15(LatLonAlt::make(34.032768, -117.408944, 12125.000000));
	Position p16(LatLonAlt::make(34.028381, -117.456617, 11552.587777));
	NavPoint np13(p13,40436.890235); np13 = np13.makeName("HABSO");    	 lpc.addNavPoint(np13);
	NavPoint np14(p14,40459.653781); np14 = np14.makeName("CIVET");    	 lpc.addNavPoint(np14);
	NavPoint np15(p15,40468.640455); np15 = np15.makeName("descent");  	 lpc.addNavPoint(np15);
	NavPoint np16(p16,40502.985188); np16 = np16.makeName("RIIVR");    	 lpc.addNavPoint(np16);

	double gsAccel = 4.0;
	//fpln(" $$ lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"testRepairGs_lpc");

	double vsAccel = 1;
	double bankAngle = Units::from("deg", 25);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, false, false);
	if (kpc.hasError()) {
		//fpln(" !! getMessage(1) = "+kpc.getMessageNoClear());
	}
	bool repair = false;
	kpc = TrajGen::makeKinematicPlan(lpc,bankAngle, gsAccel, vsAccel, repair, repair, repair);
	if (kpc.hasError()) {
		//fpln(" !! getMessage(2) = "+kpc.getMessageNoClear());
	}
	//DebugSupport.dumpPlan(kpc,"testRepairGs_kpc");

	//fpln(" $$ kpc = "+kpc.toStringGs());
	//bool removeRedundantPoints = false;
    //PlanUtil::structRevertTCPs(kpc, removeRedundantPoints);
    lpc = PlanUtil::revertAllTCPs(kpc,false);
    EXPECT_TRUE(lpc.isLinear());
    EXPECT_TRUE(lpc.isWellFormed());
	//fpln(" $$ rev = "+kpc.toStringGs());
	//DebugSupport.dumpPlan(kpc,"testRepairGs_rev");
}




TEST_F(PlanUtilTest, test_advanceDistance) {
	Plan lpc("");
	Position p0(LatLonAlt::make(34.049879, -117.530758, 12654.000000));
	Position p1(LatLonAlt::make(34.320226, -117.631595, 11552.000000));
	Position p2(LatLonAlt::make(34.507122, -117.968878, 12274.000000));
	Position p3(LatLonAlt::make(34.767908, -118.106225, 12125.000000));
	NavPoint np0(p0,430);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,610);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,830);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1000);    	 lpc.addNavPoint(np3);
	double gsAccel = 2.0;
	//fpln(" $$ lpc = "+lpc.toStringGs());
	//DebugSupport.dumpPlan(lpc,"advanceDistance_lpc");
	double vsAccel = 1;
	double bankAngle = Units::from("deg", 20);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, false, false, false);
	if (kpc.hasError()) {
		//fpln(" !! getMessage(1) = "+kpc.getMessageNoClear());
	}
	EXPECT_TRUE(kpc.isFlyable());
	//DebugSupport.dumpPlan(kpc,"distanceStuff_kpc");
	double totalDist = kpc.pathDistance();
	//fpln(" $$ advanceDistance: totalDist = "+Units::str("ft",totalDist));
	//double step = totalDist/25;
	//fpln(" $$ kpc = "+kpc.toStringGs());
	double step = Units::from("ft",500);
	bool linear = false;
	Plan trace("trace");
	double t = 0.0;
	double currentTime = kpc.getFirstTime()+2.0;
	Position currentPosition = kpc.position(currentTime);
   	double startDist = kpc.pathDistance(0) - kpc.partialPathDistance(currentTime,linear);    // offset distances by about 350 m
	for (double d = 0; d <= totalDist; d = d + step) {
		//fpln("\n $$ test_advanceDistance: t = "+t+" d = "+Units::str("ft",d));
		std::pair<Position,int> pad = PlanUtil::positionFromDistance(kpc, currentTime , d, linear); // start at time +2 sec, then advance by d dist
		Position sNew = pad.first; // calculated position
		int finalSeg = pad.second; // segment of calculated position
		//fpln(" $$ test_advanceDistance: finalSeg = "+finalSeg+" d = "+Units::str("NM",d)+" sNew = "+sNew);
        if (startDist + d < kpc.pathDistance(0)) { // if on first segment
        	EXPECT_EQ(0,finalSeg);
        	double distSeg0 = currentPosition.distanceH(sNew);
        	EXPECT_NEAR(distSeg0,d,0.0001);
        } else if (startDist + d < kpc.pathDistance(0,2)) { // if on second segment
        	EXPECT_EQ(1,finalSeg);
        } else if (startDist + d <  kpc.pathDistance(0,3)) {
        	EXPECT_EQ(2,finalSeg);
        } else if (startDist + d <  kpc.pathDistance(0,4)) {
        	EXPECT_EQ(3,finalSeg);
        } else if (startDist + d <  kpc.pathDistance(0,5)) {
        	EXPECT_EQ(4,finalSeg);
        	double distSeg4 = kpc.point(4).position().distanceH(sNew);
        	EXPECT_NEAR(distSeg4,d+startDist-kpc.pathDistance(0,4),0.01); // segment 4 is in turn (so straight-line distance is a bad fit)
        } else if (startDist + d <  kpc.pathDistance(0,6)) {
        	EXPECT_EQ(5,finalSeg);
        } else if (startDist + d <  kpc.pathDistance(0,7)) {
        	EXPECT_EQ(6,finalSeg);
        } else if (startDist + d <  kpc.pathDistance(0,8)) {
        	EXPECT_EQ(7,finalSeg);
        } else if (startDist + d <  kpc.pathDistance(0,9)) {
        	EXPECT_EQ(8,finalSeg);
        }
		t = t + 10.0;
		trace.addNavPoint(NavPoint(sNew,t));
	}
	//DebugSupport.dumpPlan(trace,"distanceStuff_trace");
}


TEST_F(PlanUtilTest, test_nextTrackChange) {
	Plan lpc("");
	Position p0  = Position::makeLatLonAlt(-0.783, 0.399, 23330.054753588436);
	Position p1  = Position::makeLatLonAlt(-0.843119711562, 0.4271316347549973, 23330.464521963740);
	Position p2  = Position::makeLatLonAlt(-1.182291040126, 0.5945982365945182, 23330.215011408185);
	Position p3  = Position::makeLatLonAlt(-1.555261153149, 0.871, 23329.518998011074);
	Position p4  = Position::makeLatLonAlt(-2.444841259841, 1.2658179210444862, 23327.274514316636);
	Position p5  = Position::makeLatLonAlt(-2.447100000000, 1.2670000000000000, 23327.270700000000);
	NavPoint np0(p0,867.5744509684803000);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,871.6036521876770000);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,1057.1864985781700000);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,1280.2391550325767000);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,1758.6649164767407000);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,1759.9154745464782000);    	 lpc.addNavPoint(np5);
	int ipTC = PlanUtil::prevTrackChange(lpc,1);
	int inTC = PlanUtil::nextTrackChange(lpc,1);
	//fpln(" $$$$$ turnCanBeInscribed: ipTC = "+ipTC+" inTC = "+inTC);
	EXPECT_EQ(0,ipTC);
	EXPECT_EQ(2,inTC);
	//DebugSupport.dumpPlan(lpc, "test_turnCanBeInscribed");
}




TEST_F(PlanUtilTest, test_nextTrackChange2) {
	Position p0 = Position::makeXYZ(0,0,0);         // trkOut = 90
	Position p1 = Position::makeXYZ(5,0,0);         // trkOut = ??
	Position p2 = Position::makeXYZ(5,0,100);       // trkOut = ??
	Position p3 = Position::makeXYZ(5,0,200);       // trkOut  = 26.6
	Position p4 = Position::makeXYZ(10,10,200);
	NavPoint np0(p0,0.0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1.0);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,2.0);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3.0);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,4.0);    	 lpc.addNavPoint(np4);
	//fpln(" $$ test_nextTrackChange2: lpc = "+lpc.toStringTrk());
	EXPECT_EQ(3,PlanUtil::nextTrackChange(lpc,0));
	EXPECT_EQ(3,PlanUtil::nextTrackChange(lpc,1));
	EXPECT_EQ(3,PlanUtil::nextTrackChange(lpc,2));
	EXPECT_EQ(4,PlanUtil::nextTrackChange(lpc,3));
	EXPECT_EQ(4,PlanUtil::nextTrackChange(lpc,4));

	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,0));
	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,1));
	EXPECT_EQ(1,PlanUtil::prevTrackChange(lpc,2));
	EXPECT_EQ(1,PlanUtil::prevTrackChange(lpc,3));
	EXPECT_EQ(1,PlanUtil::prevTrackChange(lpc,4));
}



TEST_F(PlanUtilTest, test_nextTrackChange3) {
	Position p0 = Position::makeXYZ(0,0,0);           // 0 trkOut = 90
	Position p1 = Position::makeXYZ(5,0,0);           // 1 trkOut = ??
	Position p2 = Position::makeXYZ(5,0,100);         // 2 trkOut = ??
	Position p3 = Position::makeXYZ(5,0,200);         // 3 trkOut = 90
	Position p4 = Position::makeXYZ(10,0,300);        // 4 trkOut = 0 change
	Position p5 = Position::makeXYZ(10,10,300);       // 5 trkOut = 45 change
	Position p6 = Position::makeXYZ(20,20,300);       // 6 trkOut = ??
	Position p7 = Position::makeXYZ(20,20,400);       // 7 trkOut = ??
	Position p8 = Position::makeXYZ(20,20,400);       // 8 trkOut = 5.71 change
	Position p9 = Position::makeXYZ(21,30,400);       // 9 trkOut = 5.71
	Position p10 = Position::makeXYZ(22,40,400);      // 10 n/a


	// roughly:
	//     10 x
	//        |
	//      9 x
	//        |
	//    6-8 x
	//       /
	//    5 x
	//      |
	//	x---x
	// 0-3  4

	NavPoint np0(p0,0.0);    	 lpc.addNavPoint(np0);
	NavPoint np1(p1,1.0);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,2.0);    	 lpc.addNavPoint(np2);
	NavPoint np3(p3,3.0);    	 lpc.addNavPoint(np3);
	NavPoint np4(p4,4.0);    	 lpc.addNavPoint(np4);
	NavPoint np5(p5,5.0);    	 lpc.addNavPoint(np5);
	NavPoint np6(p6,6.0);    	 lpc.addNavPoint(np6);
	NavPoint np7(p7,7.0);    	 lpc.addNavPoint(np7);
	NavPoint np8(p8,8.0);    	 lpc.addNavPoint(np8);
	NavPoint np9(p9,9.0);    	 lpc.addNavPoint(np9);
	NavPoint np10(p10,10.0);    	 lpc.addNavPoint(np10);

	//TODO: the comments say it will return an index "at or after iNow" where there is a change in track, but the behavior is strictly after
	// I'm interpreting that as skipping over collinear points (including gs 0)
	// so the above have changes at index 4, 5, and 8 (and 10)

	//fpln(" $$ test_nextTrackChange3: lpc = "+lpc.toStringTrk());
	EXPECT_EQ(4,PlanUtil::nextTrackChange(lpc,0));
	EXPECT_EQ(4,PlanUtil::nextTrackChange(lpc,1));
	EXPECT_EQ(4,PlanUtil::nextTrackChange(lpc,2));
	EXPECT_EQ(4,PlanUtil::nextTrackChange(lpc,3));
	EXPECT_EQ(5,PlanUtil::nextTrackChange(lpc,4)); // if return > i this should be 4 (currently 5)
	EXPECT_EQ(8,PlanUtil::nextTrackChange(lpc,5)); // if return > i this should be 5 (currently 8)
	EXPECT_EQ(8,PlanUtil::nextTrackChange(lpc,6));
	EXPECT_EQ(8,PlanUtil::nextTrackChange(lpc,7));
	EXPECT_EQ(10,PlanUtil::nextTrackChange(lpc,8));  // if return > i this should be 8 (currently 10)
	EXPECT_EQ(10,PlanUtil::nextTrackChange(lpc,9)); // effectively n/a
	EXPECT_EQ(10,PlanUtil::nextTrackChange(lpc,10)); // effectively n/a

	// in this direction changes are at index 6, 5, and 4 (and 0)
	// again, this says "at or before iNow", but the behavior is strictly before

	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,0));
	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,1));
	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,2));
	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,3));
	EXPECT_EQ(0,PlanUtil::prevTrackChange(lpc,4)); // if return < i this should be 4 (currently 0)
	EXPECT_EQ(4,PlanUtil::prevTrackChange(lpc,5)); // if return < i this should be 5 (currently 4)
	EXPECT_EQ(5,PlanUtil::prevTrackChange(lpc,6)); // if return < i this should be 6 (currently 5)
	EXPECT_EQ(6,PlanUtil::prevTrackChange(lpc,7));
	EXPECT_EQ(6,PlanUtil::prevTrackChange(lpc,8));
	EXPECT_EQ(6,PlanUtil::prevTrackChange(lpc,9));
	EXPECT_EQ(6,PlanUtil::prevTrackChange(lpc,10));
}

TEST_F(PlanUtilTest, test_prevVsChange) {
	Plan::setMIN_VS_DELTA_GEN(Units::from("fpm",10.0));

	Plan lpc("lpc");
	Position p0  = Position::makeLatLonAlt(32.750403000000, -97.3352720000000000, 1300.000000000000);
	Position p1  = Position::makeLatLonAlt(32.750403000000, -97.3352720000000000, 1300.000000000000);
	Position p2  = Position::makeLatLonAlt(32.750403000000, -97.3352720000000000, 1350.000000000000);
	Position p3  = Position::makeLatLonAlt(32.749995239122, -97.3320468116458500, 1493.893287830858);
	Position p4  = Position::makeLatLonAlt(32.749977932261, -97.3319099379717700, 1500.000000000000);
	Position p5  = Position::makeLatLonAlt(32.745210121214, -97.2942497745267300, 1500.000000000000);
	Position p6  = Position::makeLatLonAlt(32.744642925948, -97.2897757712713400, 1368.083670372850);
	NavPoint np0(p0,95.0000000000000000);    	 int ix0 = lpc.addNavPoint(np0);
	lpc.setInfo(ix0,"<virtualBOOT>");
	NavPoint np1(p1,145.0000000000000000);    	 lpc.addNavPoint(np1);
	NavPoint np2(p2,151.0000000000000000);    	 int ix2 = lpc.addNavPoint(np2);
	lpc.setInfo(ix2,"<BOC>");
	NavPoint np3(p3,180.3105800212316600);    	 int ix3 = lpc.addNavPoint(np3);
	lpc.setInfo(ix3,"<depFix>");
	NavPoint np4(p4,181.4738403730942500);    	 int ix4 = lpc.addNavPoint(np4);
	lpc.setInfo(ix4,"<TOC>");
	NavPoint np5(p5,500.6703148086986600);    	 int ix5 = lpc.addNavPoint(np5);
	lpc.setInfo(ix5,"<TOD>");
	NavPoint np6(p6,521.0782505839699000);    	 int ix6 = lpc.addNavPoint(np6);
	lpc.setInfo(ix6,"<arrFix>");
	NavPoint np7(lpc.position(147),147);   lpc.addNavPoint(np7);
	NavPoint np8(lpc.position(240),240);   lpc.addNavPoint(np8);
	NavPoint np9(lpc.position(340),340);   lpc.addNavPoint(np9);
	//fpln(" $$ lpc = "+lpc.toStringProfile());
	EXPECT_EQ(0,PlanUtil::prevVsChange(lpc,1));
	EXPECT_EQ(1,PlanUtil::prevVsChange(lpc,2));
	EXPECT_EQ(1,PlanUtil::prevVsChange(lpc,3));
	EXPECT_EQ(3,PlanUtil::prevVsChange(lpc,4));
	EXPECT_EQ(4,PlanUtil::prevVsChange(lpc,5));
	EXPECT_EQ(5,PlanUtil::prevVsChange(lpc,6));
	EXPECT_EQ(5,PlanUtil::prevVsChange(lpc,7));
	EXPECT_EQ(5,PlanUtil::prevVsChange(lpc,8));
	EXPECT_EQ(8,PlanUtil::prevVsChange(lpc,9));
	EXPECT_EQ(3,PlanUtil::nextVsChange(lpc,1));
	EXPECT_EQ(3,PlanUtil::nextVsChange(lpc,2));
	EXPECT_EQ(4,PlanUtil::nextVsChange(lpc,3));
	EXPECT_EQ(5,PlanUtil::nextVsChange(lpc,4));
	EXPECT_EQ(8,PlanUtil::nextVsChange(lpc,5));
	EXPECT_EQ(8,PlanUtil::nextVsChange(lpc,6));
	EXPECT_EQ(8,PlanUtil::nextVsChange(lpc,7));
	EXPECT_EQ(9,PlanUtil::nextVsChange(lpc,8));
	EXPECT_EQ(9,PlanUtil::nextVsChange(lpc,9));
	Plan::setMinDeltaGen_BackToDefaults();
}


/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicsPosition.h"
#include "ProjectedKinematics.h"
#include "Plan.h"
#include "PlanUtil.h"
#include "GreatCircle.h"
#include "Util.h"
#include "Vect3.h"
#include "format.h"

#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class KinematicsPositionTest : public ::testing::Test {

protected:

	double PI;

	virtual void SetUp() {
		PI = M_PI;
	}
};

TEST_F(KinematicsPositionTest, testLinear) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	for (double t = 1; t < 100; t++) {
		std::pair<Position,Velocity> kp  = KinematicsPosition::linear(sop, vo, t);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::linear(sop, vo, t);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.000001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.001);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.001);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,0.2);
		EXPECT_NEAR(von.y,von2.y    ,0.1);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}

TEST_F(KinematicsPositionTest, testTurnOmega_90) {
	LatLonAlt so = LatLonAlt::make(40.0, 100.0, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",90.0),Units::from("kn",300),0.0);
	double omega = -Units::from("deg/s",1.0);
    double tFinal = 2.0*std::abs(Units::from("deg",90.0)/omega);
    for (double t = 0; t < tFinal; t++) {
    	std::pair<Position,Velocity> kp = KinematicsPosition::turnOmega(sop, vo, t, omega);
     	//Position son = kp.first;
     	Velocity von = kp.second;
     	std::pair<Position,Velocity> kp2 = ProjectedKinematics::turnOmega(sop, vo, t, omega);
     	//Position son2 = kp2.first;
     	Velocity von2 = kp2.second;
     	//f.pln(" $$$ von.trk() = "+von.trk()+"von2.trk() = "+von2.trk());
     	EXPECT_NEAR(von2.trk(),von.trk() ,0.0001);                   // TODO:  NOT VERY CLOSE
     }
}

TEST_F(KinematicsPositionTest, testTurn) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	double R = Units::from("nm", 1.5);
	for (double t = 1; t < 100; t++) {
		bool turnRight = (((int) t) % 2 == 0);
		std::pair<Position,Velocity> kp  = KinematicsPosition::turn(sop, vo, t, R, turnRight);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::turn(sop, vo, t, R, turnRight);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.000001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs(),0.0001);
		EXPECT_NEAR(von.vs(),von2.vs(),0.000001);
		EXPECT_NEAR(von.x,von2.x,0.2);
		EXPECT_NEAR(von.y,von2.y,0.2);
		EXPECT_NEAR(von.z,von2.z,0.000001);
	}
}




TEST_F(KinematicsPositionTest, testTurnOmega) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	//double R = Units::from("nm", 1.5);
	for (double t = 1; t < 100; t++) {
		int dir = -1;
		if (((int) t) % 2 == 0) dir = 1;
		double omega = dir*Units::from("deg/s",2.4);
		//fpln(" dir = "+dir);
		std::pair<Position,Velocity> kp  = KinematicsPosition::turnOmega(sop, vo, t, omega);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::turnOmega(sop, vo, t, omega);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.000001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs(),0.0001);
		EXPECT_NEAR(von.vs(),von2.vs(),0.000001);
		EXPECT_NEAR(von.x,von2.x,0.4);
		EXPECT_NEAR(von.y,von2.y,0.2);
		EXPECT_NEAR(von.z,von2.z,0.000001);
	}
}


TEST_F(KinematicsPositionTest, testTurnUntil) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	double goalTrack = Units::from("deg", 180);
	double bankAngle = Units::from("deg", 25);
	for (double t = 1; t < 100; t++) {
		std::pair<Position,Velocity> kp  = KinematicsPosition::turnUntil(sop, vo, t, goalTrack, bankAngle);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::turnUntil(sop, vo, t, goalTrack, bankAngle);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.000005);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.001);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,0.4);
		EXPECT_NEAR(von.y,von2.y    ,0.3);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}


TEST_F(KinematicsPositionTest, testGsAccel) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",400),0.0);
	for (double t = 1; t < 100; t++) {
		int dir = -1;
		if (((int) t) % 2 == 0) dir = 1;
		double a = dir*1.8;
		std::pair<Position,Velocity> kp  = KinematicsPosition::gsAccel(sop, vo, t, a);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::gsAccel(sop, vo, t, a);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		//fpln(" testGsAccel: t = "+t+" von = "+von+" von2 = "+von2);
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.00001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.01);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,0.8);
		EXPECT_NEAR(von.y,von2.y    ,0.5);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}



TEST_F(KinematicsPositionTest, testGsAccelUntil) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	double a = 2;
	for (double t = 1; t < 200; t++) {
		int dir = -1;
		if (((int) t) % 2 == 0) dir = 1;
		double goalGs = Units::from("kn",400)  + dir*Units::from("kn",300) + t;
		std::pair<Position,Velocity> kp  = KinematicsPosition::gsAccelUntil(sop, vo, t, goalGs, a);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::gsAccelUntil(sop, vo, t, goalGs, a);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		//fpln(" goalGs = "+Units::str("kn",goalGs)+" von = "+von+" von2 = "+von2);
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.00001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.05);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,2.0);
		EXPECT_NEAR(von.y,von2.y    ,0.5);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}



TEST_F(KinematicsPositionTest, testVsAccel) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	for (double t = 1; t < 100; t++) {
		int dir = -1;
		if (((int) t) % 2 == 0) dir = 1;
		double a = dir*2.3;
		std::pair<Position,Velocity> kp  = KinematicsPosition::vsAccel(sop, vo, t, a);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::vsAccel(sop, vo, t, a);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.000001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.00001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.001);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.001);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,0.1);
		EXPECT_NEAR(von.y,von2.y    ,0.02);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}



TEST_F(KinematicsPositionTest, testVsAccelUntil) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	for (double t = 1; t < 1000; t++) {
		int dir = -1;
		if (((int) t) % 2 == 0) dir = 1;
		double a = 1.8;
		double goalVs = dir*Units::from("fpm",3000);
		std::pair<Position,Velocity> kp  = KinematicsPosition::vsAccelUntil(sop, vo, t, goalVs, a);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::vsAccelUntil(sop, vo, t, goalVs, a);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.00001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.00001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.05);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,1.0);
		EXPECT_NEAR(von.y,von2.y    ,0.2);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}



TEST_F(KinematicsPositionTest, testVsLevelOut) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	double climbRate = Units::from("fpm",1700);
	double targetAlt = Units::from("ft",19000);
	for (double t = 1; t < 1000; t++) {
		//int dir = -1;
		//if (((int) t) % 2 == 0) dir = 1;
		double a = 1.8;
		//double goalVs = dir*Units::from("fpm",3000);
		std::pair<Position,Velocity> kp  = KinematicsPosition::vsLevelOut(std::pair<Position,Velocity>(sop, vo), t,  climbRate,  targetAlt,  a);
		Position son = kp.first;
		Velocity von = kp.second;
		std::pair<Position,Velocity> kp2 = ProjectedKinematics::vsLevelOut(sop, vo, t,  climbRate,  targetAlt,  a);
		Position son2 = kp2.first;
		Velocity von2 = kp2.second;
		EXPECT_NEAR(son.lat(),son2.lat(),0.00001);
		EXPECT_NEAR(son.lon(),son2.lon(),0.00001);
		EXPECT_NEAR(son.alt(),son2.alt(),0.000001);
		EXPECT_NEAR(von.trk(),von2.trk(),0.01);
		EXPECT_NEAR(von.gs(),von2.gs()  ,0.05);
		EXPECT_NEAR(von.vs(),von2.vs()  ,0.000001);
		EXPECT_NEAR(von.x,von2.x    ,1.0);
		EXPECT_NEAR(von.y,von2.y    ,0.2);
		EXPECT_NEAR(von.z,von2.z    ,0.000001);
	}
}


TEST_F(KinematicsPositionTest, testTurn_LatLon) {
	LatLonAlt so = LatLonAlt::make(85.0, -83.0, 10000.0);
	//fpln(" $$ s2 = "+s2);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",0.0),Units::from("kn",300),0.0);
	double omega = Units::from("deg/s",0.05);
	//fpln(" $$$ radius = "+Units::str("NM",Kinematics::turnRadiusByRate(vo.gs(), omega)));
	Plan lpc;
	Plan kpc;
	double endT = Units::from("deg",180.0)/omega;
	for (double t = 0; t <= endT; t = t + 10.0 ) {
		LatLonAlt st = KinematicsLatLon::turnOmega(so, vo, t, omega).first;
	    NavPoint nt(Position(st), t);
	    lpc.addNavPoint(nt);
	    Position kp = ProjectedKinematics::turnOmega(sop, vo, t, omega).first;
	    kpc.addNavPoint(NavPoint(kp,t));
        //fpln(" $$ omega*t = "+omega*t+" arclength = "+
    }
    //DebugSupport.dumpPlan(lpc, "turnLatLon_lpc");
    //DebugSupport.dumpPlan(kpc, "turnLatLon_kpc");
    double diffMetric = PlanUtil::diffMetric(lpc, kpc);
    EXPECT_TRUE(diffMetric < 10);
}



TEST_F(KinematicsPositionTest, testTurn_LatLon2) {
	LatLonAlt so = LatLonAlt::make(85.0, -83.0, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",180.0),Units::from("kn",300),0.0);
	double omega = -Units::from("deg/s",0.05);
	double endT = Units::from("deg",180.0)/-omega;
	//fpln(" $$$ radius = "+Units::str("nm",Kinematics::turnRadiusByRate(vo.gs(), omega)));
	Plan lpc;
	Plan kpc;
	for (double t = 0; t <= endT; t = t + 10 ) {
		LatLonAlt st = KinematicsLatLon::turnOmega(so, vo, t, omega).first;
	    NavPoint nt(Position(st), t);
	    lpc.addNavPoint(nt);
	    Position kp = ProjectedKinematics::turnOmega(sop, vo, t, omega).first;
	    kpc.addNavPoint(NavPoint(kp,t));
        //fpln(" $$ omega*t = "+omega*t+" arclength = "+
    }
	//DebugSupport.dumpPlan(lpc, "turnLatLon_lpc2");
	//DebugSupport.dumpPlan(kpc, "turnLatLon_kpc2");
	//fpln("  $$ diffMetric = "+PlanUtil::diffMetric(lpc, kpc));
	double diffMetric = PlanUtil::diffMetric(lpc, kpc);
	EXPECT_TRUE(diffMetric < 10);

}



TEST_F(KinematicsPositionTest, testGsAccelLatLon) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	double gsAccel = Units::from("deg/s",2.0);
	Plan lpc;
	Plan kpc;
	for (double t = 0; t <= 1000.0; t++ ) {
		LatLonAlt st = KinematicsLatLon::gsAccel(so, vo, t, gsAccel).first;
	    NavPoint nt(Position(st), t);
	    lpc.addNavPoint(nt);
	    Position kp = ProjectedKinematics::gsAccel(sop, vo, t, gsAccel).first;
	    kpc.addNavPoint(NavPoint(kp,t));
	}
	//DebugSupport.dumpPlan(lpc, "turnGsAccel_lpc");
	//DebugSupport.dumpPlan(kpc, "turnGsAccel_kpc");
	//fpln("  $$ diffMetric = "+PlanUtil::diffMetric(lpc, kpc));
	double diffMetric = PlanUtil::diffMetric(lpc, kpc);
	EXPECT_TRUE(diffMetric < 10);
}



TEST_F(KinematicsPositionTest, testVsAccelLatLon) {
	LatLonAlt so = LatLonAlt::make(54.878101, -83.21869, 10000.0);
	Position sop(so);
	Velocity vo = Velocity::mkTrkGsVs(Units::from("deg",10.0),Units::from("kn",300),0.0);
	double vsAccel = Units::from("deg/s",2.0);
	Plan lpc;
	Plan kpc;
	for (double t = 0; t <= 1000.0; t++ ) {
		LatLonAlt st = KinematicsLatLon::vsAccel(so, vo, t, vsAccel).first;
	    NavPoint nt(Position(st), t);
	    lpc.addNavPoint(nt);
	    Position kp = ProjectedKinematics::vsAccel(sop, vo, t, vsAccel).first;
	    kpc.addNavPoint(NavPoint(kp,t));
    }
	//DebugSupport.dumpPlan(lpc, "turnVsAccel_lpc");
	//DebugSupport.dumpPlan(kpc, "turnVsAccel_kpc");
	//fpln("  $$ diffMetric = "+PlanUtil::diffMetric(lpc, kpc));
	double diffMetric = PlanUtil::diffMetric(lpc, kpc);
	EXPECT_TRUE(diffMetric < 10);
}




TEST_F(KinematicsPositionTest, testTurnByDistOmega) {
	LatLonAlt so = LatLonAlt::make(60, 60, 0.0);
	Velocity vo = Velocity::makeTrkGsVs(15, 100, 0);
	//fpln(" $$$ so = "+so+" vo = "+vo);
	double R = Units::from("NM",5);
	LatLonAlt center = GreatCircle::linear_initial(so, vo.trk()+M_PI/2, R);
	Plan lpc;
	Plan tpc;
	double step = Units::from("NM",1.0);
	double dt = step/vo.gs();
    for (double  d = 0.0; d < Units::from("NM",10.0); d = d + step) {
    	int dir = 1;
    	std::pair<LatLonAlt,Velocity> tAtd = KinematicsLatLon::turnByDist2D(so, center, dir, d, vo.gs());
    	Velocity vel = tAtd.second;
    	double t = d/vo.gs();
    	NavPoint np(Position(tAtd.first),t);
    	lpc.addNavPoint(np);
    	// check
    	double omega = step/(R*dt);
    	//std::pair<LatLonAlt,Velocity> pr = KinematicsLatLon::turnOmegaAlt(so, vo, t, omega);
    	std::pair<LatLonAlt,Velocity> pr = KinematicsLatLon::turnOmega(so, vo, t, omega);
    	NavPoint tnp(Position(pr.first),t);
    	Velocity vel2 = pr.second;
    	tpc.addNavPoint(tnp);
    	t++;
//	    	fpln(" $$$ d = "+Units::str("NM",d)+" t = "+t+" vel = "+vel+" vel2 = "+vel2);
    	EXPECT_NEAR(tnp.lat(),np.lat(),1E-12);
    	EXPECT_NEAR(tnp.lon(),np.lon(),1E-12);
    	EXPECT_NEAR(vel.trk(),vel2.trk(),1E-12);
    	EXPECT_NEAR(vel.gs(),vel2.gs(),1E-12);
    }
    //DebugSupport.dumpPlan(lpc, "testTurnByDist_lpc");
    //DebugSupport.dumpPlan(tpc, "testTurnByDist_tpc");
}



TEST_F(KinematicsPositionTest, testTurnByDist2) {
	LatLonAlt center = LatLonAlt::make(41.68354084, -72.52474233, 6000.00000000);
	LatLonAlt BOT = LatLonAlt::make(41.67203131, -72.56376857, 5514.24190541);
	LatLonAlt EOT = LatLonAlt::make(41.67203131, -72.56376857, 5514.24190541);
	//double R = 1.880315 [NM]
	//double distCenterToEOT = center.distanceH(EOT);
	//double distCenterToBOT = center.distanceH(BOT);
	//double deltaDist = std::abs(distCenterToEOT - distCenterToBOT);
	double R = center.distanceH(BOT);
	//double RC = GreatCircle::chord_radius_from_surface_radius(R);
	//fpln(" $$ testTurnByDist2: R-RC = "+(R-RC));
	//R = RC;
	//double RC = GreatCircle::chord_distance(center,BOT);
	//fpln(" $$ testTurnByDist2: R = "+Units::str("NM",R,14));
	//fpln(" $$ testTurnByDist2: RC = "+Units::str("NM",RC,14));
	//fpln(" $$ testTurnByDist2: deltaDist = "+Units::str("m",deltaDist,14));
	int dir = 1;
	double gsAtd = 100.0;

	double ang1 = GreatCircle::initial_course(center,BOT);
	double ang2 = GreatCircle::initial_course(center,EOT);
	double deltaAng = Util::turnDelta(ang1,ang2);

	double d = deltaAng*R;
	std::pair<LatLonAlt, Velocity> td = KinematicsLatLon::turnByDist2D(BOT, center, dir, d, gsAtd);
	LatLonAlt ATP = td.first;

	double distATPToEOT = ATP.distanceH(EOT);
	//fpln(" $$ testTurnByDist2: distATPToEOT = "+distATPToEOT);
	EXPECT_TRUE(distATPToEOT < 1.1E-9);
}


TEST_F(KinematicsPositionTest, testTurnByDist3) {
	LatLonAlt center = LatLonAlt::make(43.0000000, 66.666, 10000.00000000);
	double R = Units::from("NM",1.5);
	double track1 = Units::from("deg",30.0);
	LatLonAlt BOT = GreatCircle::linear_initial(center,track1,R);
	double calcR = center.distanceH(BOT);
	EXPECT_NEAR(R,calcR,1E-9);   // 1E-10  does not work
	//double RC = GreatCircle::chord_distance(R);
	//fpln(" $$ testTurnByDist2: R = "+Units::str("NM",R,14));
	//fpln(" $$ testTurnByDist2: RC = "+Units::str("NM",RC,14));
	double maxError = 0;
	for (double track2 = Units::from("deg",40.0); track2 <= Units::from("deg",90.0) ; track2 = track2 +Units::from("deg",0.01)) {
		LatLonAlt EOT = GreatCircle::linear_initial(center,track2,R);
		calcR = center.distanceH(EOT);
		//double calcR_diff = std::abs(calcR-R);
		//fpln("   calcR_diff = "+Units::str("m",calcR_diff,14));
		EXPECT_NEAR(R,calcR,2.9E-9);   // 2.8E-9 does not work at step = 0.01
		//double distCenterToEOT = center.distanceH(EOT);
		//double distCenterToBOT = center.distanceH(BOT);
		//f.p(" $$ testTurnByDist2: deltaDist = "+Units::str("m",deltaDist,14));
		int dir = 1;
		double ang1 = GreatCircle::initial_course(center,BOT);
		double ang2 = GreatCircle::initial_course(center,EOT);
		double deltaAng = Util::turnDelta(ang1,ang2);
		//LatLonAlt ATP = KinematicsLatLon::turnByAngle2D(BOT, center, deltaAng);
		//double distATPToEOT = ATP.distanceH(EOT);
		//fpln("   distATPToEOT = "+Units::str("m",distATPToEOT,14));
		double d = deltaAng*R;
		LatLonAlt ATP2 =  KinematicsLatLon::turnByDist2D(BOT, center, dir, d);
		double distATP2ToEOT = ATP2.distanceH(EOT);
		if (distATP2ToEOT > maxError) maxError = distATP2ToEOT;
		//if (distATP2ToEOT > 0.0001) f.p("********************************");
		//fpln("   distATP2ToEOT = "+distATP2ToEOT);
		//EXPECT_TRUE(distATP2ToEOT < 4.9E-9);                            // USING RC			                          // USING R
		//EXPECT_NEAR(ATP.distanceH(ATP2),0.0,0.0);
	}
	//fpln(" $$ testTurnByDist3:  maxError = "+maxError);
	EXPECT_NEAR(0.0,maxError,2.13E-9); // was 2E-9
}


TEST_F(KinematicsPositionTest, testTurnByDist4) {
	LatLonAlt center = LatLonAlt::make(40.0000000, -88.88888, 10000.00000000);
	double R = Units::from("NM",1.5);
	double track1 = Units::from("deg",90.0);
	LatLonAlt BOT = GreatCircle::linear_initial(center,track1,R);
	//double RC = GreatCircle::chord_distance(R);
	double maxError = 0;
	for (double track2 = Units::from("deg",90.0); track2 <= Units::from("deg",190.0) ; track2 = track2 +Units::from("deg",0.1)) {
		LatLonAlt EOT = GreatCircle::linear_initial(center,track2,R);
		double calcR = center.distanceH(EOT);
		EXPECT_NEAR(R,calcR,2.9E-9);   // 2.8E-9 does not work at step = 0.01
		int dir = 1;
		double ang1 = GreatCircle::initial_course(center,BOT);
		double ang2 = GreatCircle::initial_course(center,EOT);
		double deltaAng = Util::turnDelta(ang1,ang2);
		double d = deltaAng*R;
		LatLonAlt ATP2 =  KinematicsLatLon::turnByDist2D(BOT, center, dir, d);
		double distATP2ToEOT = ATP2.distanceH(EOT);
		if (distATP2ToEOT > maxError) maxError = distATP2ToEOT;
		//if (distATP2ToEOT > 0.0001) f.p("********************************");
		//fpln(" track2 = "+track2+"  distATP2ToEOT = "+distATP2ToEOT);
		//EXPECT_TRUE(distATP2ToEOT < 4.9E-9);                            // USING RC
		//EXPECT_NEAR(ATP.distanceH(ATP2),0.0,0.0);
	}
	//fpln("   maxError = "+maxError);
	//EXPECT_TRUE(maxError > 0.00213);                            // USING R
	EXPECT_NEAR(0.0,maxError,5E-9);
}



TEST_F(KinematicsPositionTest, testTurnByDist5) {
	Plan lpc("Dist5");
	double maxMaxError = 0.0;
	//double worstLat = -1;
	double lat = 59.158999999998; {
	//for (double lat = 49.239; lat < 60.0; lat = lat + 0.01) {
		LatLonAlt center = LatLonAlt::make(lat, -88.88888, 10000.00000000);
		lpc.add(Position(center),0.0);
		double R = Units::from("NM",1.5);
		double track1 = Units::from("deg",90.0);
		LatLonAlt BOT = GreatCircle::linear_initial(center,track1,R);
		lpc.add(Position(BOT),10.0);
		//double RC = GreatCircle::chord_distance(R);
		//R = RC;
		double maxError = 0;
		//double track2 = Units::from("deg",180.0); {
		for (double track2 = Units::from("deg",179.9); track2 <= Units::from("deg",180.9) ; track2 = track2 +Units::from("deg",0.0001)) {
			//for (double track2 = Units::from("deg",90.0); track2 <= Units::from("deg",190.0) ; track2 = track2 +Units::from("deg",0.1)) {
			LatLonAlt EOT = GreatCircle::linear_initial(center,track2,R);
			lpc.add(Position(EOT),10.0+track2);
			double calcR = center.distanceH(EOT);
			EXPECT_NEAR(R,calcR,3.0E-9);   // 2.8E-9 does not work at step = 0.01
			int dir = 1;
			double ang1 = GreatCircle::initial_course(center,BOT);
			double ang2 = GreatCircle::initial_course(center,EOT);
			double deltaAng = Util::turnDelta(ang1,ang2);
			double d = deltaAng*R;
			LatLonAlt ATP2 =  KinematicsLatLon::turnByDist2D(BOT, center, dir, d);
			double distATP2ToEOT = ATP2.distanceH(EOT);
			if (distATP2ToEOT > maxError) maxError = distATP2ToEOT;
			//if (distATP2ToEOT > 0.0001) f.p("********************************");
			//fpln(" track2 = "+track2+"  distATP2ToEOT = "+distATP2ToEOT);
			//EXPECT_TRUE(distATP2ToEOT < 4.9E-9);                            // USING RC
			//EXPECT_NEAR(ATP.distanceH(ATP2),0.0,0.0);
		}
		//DebugSupport.dumpPlan(lpc,"Dist5");
		//fpln(" lat = "+lat+"  maxError = "+maxError);
		//EXPECT_TRUE(maxError < 4.9E-9);
		if (maxError > maxMaxError) {
			maxMaxError = maxError;
			//worstLat = lat;
		}
	}// USING R
	//fpln(" worstLat = "+worstLat+" maxMaxError = "+maxMaxError);
	EXPECT_NEAR(0.0,maxMaxError,3E-9);
}



TEST_F(KinematicsPositionTest, testLinearInitial) {
	double maxError = 0;
	//double maxD = 0.0;
	double lat = 59.158999999998; {
		//for (double lat = 49.239; lat < 60.0; lat = lat + 0.01) {
		LatLonAlt so = LatLonAlt::make(lat, -88.88888, 10000.00000000);
		double track = M_PI;//+0.1;
		for (double d = 10; d < 30000; d = d + 10) {
			//double d = 20.0;
			LatLonAlt sn = GreatCircle::linear_initial(so, track, d);    // surface R
			EXPECT_NEAR(sn.lon(),so.lon(),1.e-15);
			EXPECT_NEAR((so.lat() - d/GreatCircle::spherical_earth_radius), sn.lat(), 1.0e-15);
			//LatLonAlt sn = GreatCircle::linear_initial_impl(center, nTrk, theta, 0.0);
			//double deltaSn = center.distanceH(sn) - R;
			//fpln(" $$$$$>>>>>>>>> deltaSn = "+Units::str("NM",deltaSn,12));
			// ++++++++++++++++++++++++++++
			sn = sn.mkAlt(0.0);
			double checkAngle = GreatCircle::initial_course(so,sn);
			double angError = Util::turnDelta(checkAngle,track);
			//fpln(" $$$$>>>>>>>>> d = "+d+"  testLinearInitial: angError = "+(angError));
			if (angError > maxError) {
				//maxD = d;
				maxError = angError;
			}
		}
	}
	//fpln(" $$$ testLinearInitial: lat = "+lat+"  maxError = "+maxError);
	EXPECT_TRUE(maxError < 1.5E-5);
	//EXPECT_TRUE(maxError > 1.0E-6);
}

// from KinematicsLatLon,,,

TEST_F(KinematicsPositionTest, test_closestTimeOnTurn) {

	LatLonAlt start = LatLonAlt::make(40, 40, 0);
	Velocity v0 = Velocity::mkTrkGsVs(0.0, 1000, 0);
	double omega = 0.1;
	LatLonAlt center = KinematicsLatLon::center(start, v0, omega);

	for (double x = 0; x < 100; x += 1) {
		// given this point we figure the time on the turn
		LatLonAlt vx = LatLonAlt::make(39+x*0.1, 41, 0);
		double t = KinematicsLatLon::closestTimeOnTurn(start, v0, omega, vx, -1);

		// we calc the angle of the point relative to the center
		double ang1 = Util::to_2pi(GreatCircle::initial_course(center, vx));

		// now calc the angle of the turn position relative to the center
		LatLonAlt turnpos = KinematicsLatLon::turnOmega(start, v0, t, omega).first;
		double ang2 = Util::to_2pi(GreatCircle::initial_course(center, turnpos));

		// the angles should be the sames
		EXPECT_NEAR(ang1,ang2,0.0001);
	}

	for (double x = 0; x < 100; x += 1) {
		// given this point we figure the time on the turn
		LatLonAlt vx = LatLonAlt::make(39+x*0.1, 39, 0);
		double t = KinematicsLatLon::closestTimeOnTurn(start, v0, omega, vx, -1);

		// we calc the angle of the point relative to the center
		double ang1 = GreatCircle::initial_course(center, vx);

		// now calc the angle of the turn position relative to the center
		LatLonAlt turnpos = KinematicsLatLon::turnOmega(start, v0, t, omega).first;
		double ang2 = GreatCircle::initial_course(center, turnpos);

		// the angles should be the sames
		EXPECT_NEAR(ang1,ang2,0.0001);
	}

}

// from KinematicsLatLon,,,

//TEST_F(KinematicsPositionTest, test_closestDistOnTurn) {
//
//	Velocity v0 = Velocity::mkTrkGsVs(0.0, 200, 0);
//	double R = Units::from("nmi", 10.0);
//	int dir = 1;
//
//	// euclidean
//	Position center(Vect3::ZERO());
//	Position start(Vect3::makeXYZ(-10, "nmi", 0, "m", 0, "m"));
//
//	// given this point we figure the time on the turn
//	Position vx(Vect3(100, 100, 0));
//	double d = KinematicsPosition::closestDistOnTurn(start, v0, R, dir, center, vx, -1);
//	double dist = R*Units::from("deg", 135);
//	// the angles should be the sames
//	EXPECT_NEAR(d,dist,0.0001);
//
//	//compare with turnbydist
//	Position p1 = KinematicsPosition::turnByDist2D(start, center, dir, d, v0.gs()).first;
//	double trk1 = center.initialVelocity(vx, 100).trk();
//	double trk2 = center.initialVelocity(p1, 100).trk();
//	EXPECT_NEAR(trk1,trk2,0.0001);
//
//
//	//latlon
//	center(LatLonAlt.ZERO);
//	start(LatLonAlt::make(0.0, -0.1, 0.0));
//	R = center.distanceH(start);
//
//	// given this point we figure the time on the turn
//	vx(GreatCircle::linear_initial(center.lla(), Units::from("deg", 45), 1000));
//	d = KinematicsPosition::closestDistOnTurn(start, v0, R, dir, center, vx, -1);
//	dist = GreatCircle::small_circle_arc_length(R, Units::from("deg", 135));
//	// the angles should be the sames
//	EXPECT_NEAR(d,dist,0.0001);
//
//	// not as accurate
//	double dist2 = R * Units::from("deg", 135);
//	EXPECT_NEAR(d,dist2,0.02);
//
//	//compare with turnbydist
//	p1 = KinematicsPosition::turnByDist2D(start, center, dir, d, v0.gs()).first;
//	trk1 = center.initialVelocity(vx, 100).trk();
//	trk2 = center.initialVelocity(p1, 100).trk();
//	EXPECT_NEAR(trk1,trk2,0.0001);
//}





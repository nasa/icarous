/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Kinematics.h"
#include "Plan.h"
#include "Util.h"
#include "Vect3.h"
#include "format.h"

#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class KinematicsTest : public ::testing::Test {

protected:

	double PI;

	virtual void SetUp() {
		PI = M_PI;
	}
};

TEST_F(KinematicsTest, turnRadius) {
	double speed = Units::from("kn",300);
	double bank = Units::from("deg",25);
	EXPECT_NEAR(Units::from("nm",2.81245295),Kinematics::turnRadius(speed,bank),0.001);
	EXPECT_NEAR(Units::from("nm",2.81245295),Kinematics::turnRadius(speed,-bank),0.001);

	speed = Units::from("m/s",100);
	bank = Units::from("deg",45);
	EXPECT_NEAR(Units::from("m",10000), Kinematics::turnRadius(speed, bank, 1.0), 0.001);
	EXPECT_NEAR(Units::from("m",10000), Kinematics::turnRadius(speed, -bank, 1.0), 0.001);

		// erroneous bank angles
		bank = Units::from("deg", 90.0);
		EXPECT_NEAR(0.0, Kinematics::turnRadius(speed,bank), 0.00000001);
		bank = Units::from("deg", 95.0);
		EXPECT_NEAR(0.0, Kinematics::turnRadius(speed,bank), 0.00000001);
		bank = Units::from("deg", 0.0);
		EXPECT_NEAR(MAXDOUBLE, Kinematics::turnRadius(speed,bank), 0.01);
		bank = Units::from("deg", -90.0);
		EXPECT_NEAR(0.0, Kinematics::turnRadius(speed,bank), 0.00000001);
		bank = Units::from("deg", -95.0);
		EXPECT_NEAR(0.0, Kinematics::turnRadius(speed,bank), 0.00000001);
}

TEST_F(KinematicsTest, bankAngleGoal) {
	double trk10 = Units::from("deg", 10.0);
	double trk20 = Units::from("deg", 20.0);
	double trk170 = Units::from("deg", 170.0);
	double trk190 = Units::from("deg", 190.0);
	double trk350 = Units::from("deg", 350.0);
	double maxBank = Units::from("deg", 20.0);

	EXPECT_NEAR( maxBank, Kinematics::bankAngleGoal( trk10,  trk20, maxBank), 0.0000000001);
	EXPECT_NEAR(-maxBank, Kinematics::bankAngleGoal( trk20,  trk10, maxBank), 0.0000000001);
	EXPECT_NEAR(-maxBank, Kinematics::bankAngleGoal( trk10, trk350, maxBank), 0.0000000001);
	EXPECT_NEAR( maxBank, Kinematics::bankAngleGoal(trk350,  trk10, maxBank), 0.0000000001);
	EXPECT_NEAR( maxBank, Kinematics::bankAngleGoal(trk170,  trk190, maxBank), 0.0000000001);
	EXPECT_NEAR(-maxBank, Kinematics::bankAngleGoal(trk190,  trk170, maxBank), 0.0000000001);

}

TEST_F(KinematicsTest, turnRate) {
	double speed300 = Units::from("knot", 300);
	double speedX = 9.80665;   // a convoluted speed
	double bank0 = 1.0e-13;
	double bank45 = Units::from("deg", 45.0);
	double bank20 = Units::from("deg", 20.0);

	EXPECT_NEAR( 0.0, Kinematics::turnRate( speed300, bank0), 0.0000000001);
	EXPECT_NEAR( 1.0, Kinematics::turnRate( speedX, bank45), 0.0000000001);
	EXPECT_NEAR( 0.023127399770194294, Kinematics::turnRate( speed300, bank20), 0.0000000001);

	EXPECT_NEAR( -1.0, Kinematics::turnRate( speedX, -bank45), 0.0000000001);
	EXPECT_NEAR( -0.023127399770194294, Kinematics::turnRate( speed300, -bank20), 0.0000000001);
}

TEST_F(KinematicsTest, turnRateGoal) {
	Velocity vo10 = Velocity::mkTrkGsVs(Units::from("deg",10.0),9.80665,0.0);   // a convoluted speed
	Velocity vo20 = Velocity::mkTrkGsVs(Units::from("deg",20.0),9.80665,0.0);   // a convoluted speed
	Velocity vo170 = Velocity::mkTrkGsVs(Units::from("deg",170.0),9.80665,0.0);   // a convoluted speed
	Velocity vo190 = Velocity::mkTrkGsVs(Units::from("deg",190.0),9.80665,0.0);   // a convoluted speed
	Velocity vo350 = Velocity::mkTrkGsVs(Units::from("deg",350.0),9.80665,0.0);   // a convoluted speed
	double trk10 = Units::from("deg", 10.0);
	double trk20 = Units::from("deg", 20.0);
	double trk350 = Units::from("deg", 350.0);
	double trk170 = Units::from("deg", 170.0);
	double trk190 = Units::from("deg", 190.0);
	double maxBank = Units::from("deg", 45.0);

	EXPECT_NEAR( 1, Kinematics::turnRateGoal( vo10,  trk20, maxBank), 0.0000000001);
	EXPECT_NEAR(-1, Kinematics::turnRateGoal( vo20,  trk10, maxBank), 0.0000000001);
	EXPECT_NEAR(-1, Kinematics::turnRateGoal( vo10, trk350, maxBank), 0.0000000001);
	EXPECT_NEAR( 1, Kinematics::turnRateGoal(vo350,  trk10, maxBank), 0.0000000001);
	EXPECT_NEAR( 1, Kinematics::turnRateGoal(vo170,  trk190, maxBank), 0.0000000001);
	EXPECT_NEAR(-1, Kinematics::turnRateGoal(vo190,  trk170, maxBank), 0.0000000001);
}

TEST_F(KinematicsTest, turnOmega) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(0,1,0);
	std::pair<Vect3,Velocity> p0(so,vo);
	double omega = Units::from("deg", 360) / 360.0;
	std::pair<Vect3,Velocity>p1 = Kinematics::turnOmega(so, vo,  90.0, omega);  // one quarter of a turn
	EXPECT_NEAR(1, p1.second.x, 0.0000000001);
	EXPECT_NEAR(0, p1.second.y, 0.0000000001);
	EXPECT_NEAR(0, p1.second.z, 0.0000000001);

	std::pair<Vect3,Velocity>p2 = Kinematics::turnOmega(so, vo, 180.0, omega);  // one half of a turn
	EXPECT_NEAR( 0, p2.second.x, 0.0000000001);
	EXPECT_NEAR(-1, p2.second.y, 0.0000000001);
	EXPECT_NEAR( 0, p2.second.z, 0.0000000001);

	std::pair<Vect3,Velocity>p3 = Kinematics::turnOmega(so, vo, 270.0, omega);  // three quarters of a turn
	EXPECT_NEAR(-1, p3.second.x, 0.0000000001);
	EXPECT_NEAR( 0, p3.second.y, 0.0000000001);
	EXPECT_NEAR( 0, p3.second.z, 0.0000000001);

	std::pair<Vect3,Velocity>p4 = Kinematics::turnOmega(so, vo, 360.0, omega);  // one full turn
	EXPECT_NEAR(so.x, p4.first.x, 0.0000000001);
	EXPECT_NEAR(so.y, p4.first.y, 0.0000000001);
	EXPECT_NEAR(so.z, p4.first.z, 0.0000000001);
	EXPECT_NEAR(0, p4.second.x, 0.0000000001);
	EXPECT_NEAR(1, p4.second.y, 0.0000000001);
	EXPECT_NEAR(0, p4.second.z, 0.0000000001);

	std::pair<Vect3,Velocity>p4p = Kinematics::turnOmega(p0, 360.0, omega);  // one full turn
	EXPECT_NEAR(p0.first.x, p4p.first.x, 0.0000000001);
	EXPECT_NEAR(p0.first.y, p4p.first.y, 0.0000000001);
	EXPECT_NEAR(p0.first.z, p4p.first.z, 0.0000000001);
	EXPECT_NEAR(0, p4p.second.x, 0.0000000001);
	EXPECT_NEAR(1, p4p.second.y, 0.0000000001);
	EXPECT_NEAR(0, p4p.second.z, 0.0000000001);

}



TEST_F(KinematicsTest, turnByDistEucl2PiTurn) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(0,1,0);
	double R = 10;
	double d = 2*M_PI*R;
	Vect3 center(10.0,0.0, 0.0);
	double gsAt_d = vo.gs();
	//Vect3 s0, Vect3 center, double d, double gsAt_d
	int dir = 1;
	std::pair<Vect3,Velocity> p2 = Kinematics::turnByDist2D(so, center, dir, d, gsAt_d);
	//fpln(" p2 = "+p2);
	EXPECT_TRUE(so.almostEquals(p2.first));
	EXPECT_TRUE(vo.almostEquals(p2.second));
}



TEST_F(KinematicsTest, turnByDistEucl) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(0,1,0);
	double R = 10;
	double d = 2*M_PI*R;
	Vect3 center(10.0,0.0, 0.0);
	double gsAt_d = vo.gs();
	//Vect3 s0, Vect3 center, double d, double gsAt_d
	int dir = 1;
	std::pair<Vect3,Velocity> p2 = Kinematics::turnByDist2D(so, center, dir, d, gsAt_d);
	//fpln(" p2 = "+p2);
	EXPECT_TRUE(so.almostEquals(p2.first));
	EXPECT_TRUE(vo.almostEquals(p2.second));
}


TEST_F(KinematicsTest, testTurnByDist) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(0,300,0);
	double R = Units::from("NM",1.0);
	Vect3 center(R,0.0, 0.0);
	//fpln(" $$$ so = "+so+" vo = "+vo);
	Plan lpc("");
	Plan tpc("");
	double step = Units::from("NM",0.1);
    for (double  d = 0.0; d < Units::from("NM",1.0); d = d + step) {
    //double  d = 0.0; {
    	//fpln(" ## testTurnByDist: d = "+Units::str("NM",1.0));
	    int dir = 1;
    	std::pair<Vect3,Velocity> tAtd = Kinematics::turnByDist2D(so, center, dir, d, vo.gs());
    	Velocity vel = tAtd.second;
    	double t = d/vo.gs();
    	NavPoint np = NavPoint(Position(tAtd.first),t);
    	lpc.addNavPoint(np);
    	// check
    	double omega = vo.gs()/R;
    	//std::pair<LatLonAlt,Velocity> pr = KinematicsLatLon.turnOmegaAlt(so, vo, t, omega);
    	std::pair<Vect3,Velocity> pr = Kinematics::turnOmega(so, vo, t, omega);
    	NavPoint tnp(Position(pr.first),t);
    	Velocity vel2 = pr.second;
    	tpc.addNavPoint(tnp);
    	t++;
    	//fpln(" ## testTurnByDist: np = "+np.toString()+" tnp = "+tnp.toString());
    	//fpln(" $$$ d = "+Units::str("NM",d)+" t = "+Fm4(t)+" vel = "+vel.toString()+" vel2 = "+vel2.toString());
    	EXPECT_NEAR(tnp.x(),np.x(),0.000000001);
    	EXPECT_NEAR(tnp.y(),np.y(),0.000001);
    	EXPECT_NEAR(vel.trk(),vel2.trk(),0.00000001);
    	EXPECT_NEAR(vel.gs(),vel2.gs(),0.0000000001);
    }
    //DebugSupport.dumpPlan(lpc, "testTurnByDist_tDist");
    //DebugSupport.dumpPlan(tpc, "testTurnByDist_tOmega");
}



TEST_F(KinematicsTest, turnGoal) {
	Vect3 so(0,0,0);
	double speed = 9.80665;
	Velocity vo90 = Velocity::mkTrkGsVs(Units::from("deg",90.0),speed,0.0);   // a convoluted speed
	double trk0 = Units::from("deg", 0.0);
	double trk90 = Units::from("deg", 90.0);
	double trk180 = Units::from("deg", 180.0);
	//double trk350 = Units::from("deg", 350.0);
	//double trk170 = Units::from("deg", 170.0);
	//double trk190 = Units::from("deg", 190.0);
	double bank45 = Units::from("deg", 45.0);

	Vect3 s1 = Kinematics::positionAfterTurn(so, vo90, trk90, bank45);
	EXPECT_NEAR(0, s1.x, 0.0000000001);
	EXPECT_NEAR(0, s1.y, 0.0000000001);
	EXPECT_NEAR(0, s1.z, 0.0000000001);

	Vect3 s2 = Kinematics::positionAfterTurn(so, vo90, trk0, bank45);
	EXPECT_NEAR(speed, s2.x, 0.0000000001);
	EXPECT_NEAR(speed, s2.y, 0.0000000001);
	EXPECT_NEAR(0, s2.z, 0.0000000001);

	Vect3 s3 = Kinematics::positionAfterTurn(so, vo90, trk180, bank45);
	EXPECT_NEAR(speed, s3.x, 0.0000000001);
	EXPECT_NEAR(-speed, s3.y, 0.0000000001);
	EXPECT_NEAR(0, s3.z, 0.0000000001);
}


TEST_F(KinematicsTest, turnPosition1) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(0,1,0);
	double R = 1.0;
	double t = PI/2;
	Vect3 tp = Kinematics::turn(so, vo, t, R, true).first;
	double omega = vo.gs()/R;
	EXPECT_NEAR(1.0, omega, 0.001);
	EXPECT_NEAR(1.0, tp.x, 0.001);
	EXPECT_NEAR(1.0, tp.y, 0.001);

	t = PI;
	tp = Kinematics::turn(so, vo, t, R, true).first;
	EXPECT_NEAR(2.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);

	t = 3*PI/2;
	tp = Kinematics::turn(so, vo, t, R, true).first;
	EXPECT_NEAR(1.0, tp.x, 0.001);
	EXPECT_NEAR(-1.0, tp.y, 0.001);

	t = 2*PI;
	tp = Kinematics::turn(so, vo, t, R, true).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);



	t = PI;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(-2.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);

	t = 3*PI/2;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(-1.0, tp.x, 0.001);
	EXPECT_NEAR(-1.0, tp.y, 0.001);

	t = 2*PI;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);

}


TEST_F(KinematicsTest, turnPosition2) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(0,-2,0);
	double R = 2.0;
	double t = PI/2;
	Vect3 tp = Kinematics::turn(so, vo, t, R, true).first;
	double omega = vo.gs()/R;
	EXPECT_NEAR(1.0, omega, 0.001);
	EXPECT_NEAR(-2.0, tp.x, 0.001);
	EXPECT_NEAR(-2.0, tp.y, 0.001);
	t = PI;
	tp = Kinematics::turn(so, vo, t, R, true).first;
	EXPECT_NEAR(-4.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);
	t = 3*PI/2;
	tp = Kinematics::turn(so, vo, t, R, true).first;
	EXPECT_NEAR(-2.0, tp.x, 0.001);
	EXPECT_NEAR(2.0, tp.y, 0.001);
	t = 2*PI;
	tp = Kinematics::turn(so, vo, t, R, true).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);
}



TEST_F(KinematicsTest, turnPosition3) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(1,0,0);
	double R = 1.0;
	double t = PI/2;
	Vect3 tp = Kinematics::turn(so, vo, t, R, false).first;
	double omega = vo.gs()/R;
	EXPECT_NEAR(1.0, omega, 0.001);
	EXPECT_NEAR(1.0, tp.x, 0.001);
	EXPECT_NEAR(1.0, tp.y, 0.001);
	t = PI;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(2.0, tp.y, 0.001);
	t = 3*PI/2;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(-1.0, tp.x, 0.001);
	EXPECT_NEAR(1.0, tp.y, 0.001);
	t = 2*PI;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);
}


TEST_F(KinematicsTest, turnPosition4) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::mkVxyz(-4,0,0);
	double R = 4.0;
	double t = PI/2;
	Vect3 tp = Kinematics::turn(so, vo, t, R, false).first;
	double omega = vo.gs()/R;
	EXPECT_NEAR(1.0, omega, 0.001);
	EXPECT_NEAR(-4.0, tp.x, 0.001);
	EXPECT_NEAR(-4.0, tp.y, 0.001);
	t = PI;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(-8.0, tp.y, 0.001);
	t = 3*PI/2;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(4.0, tp.x, 0.001);
	EXPECT_NEAR(-4.0, tp.y, 0.001);
	t = 2*PI;
	tp = Kinematics::turn(so, vo, t, R, false).first;
	EXPECT_NEAR(0.0, tp.x, 0.001);
	EXPECT_NEAR(0.0, tp.y, 0.001);

	//		Vect2 tp2D = Kinematics::turnPosition2D(so.vect2(), vo.vect2(), R, t, false);
	//		EXPECT_NEAR(tp2D.x,tp.x,0.00001);
	//		EXPECT_NEAR(tp2D.y,tp.y,0.00001);

}

TEST_F(KinematicsTest, turnVelocity1) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(250,600.0,0.0);
	double R = 30.0;
	double t  = 0.0;
	bool turnRight = true;
	Velocity tv = Kinematics::turn(so, vo, t, R, turnRight).second;
	EXPECT_NEAR(vo.trk(),tv.trk(),0.0001);
	EXPECT_NEAR(vo.gs(),tv.gs(),0.0001);
	double omega = vo.gs()/R;
	t = PI/omega;
	tv = Kinematics::turn(so, vo, t, R, turnRight).second;
	EXPECT_NEAR(180+Units::to("deg",vo.trk()),Units::to("deg",tv.trk()),0.0001);
	EXPECT_NEAR(vo.gs(),tv.gs(),0.0001);

	t = PI/(2*omega);
	tv = Kinematics::turn(so, vo, t, R, !turnRight).second;
	EXPECT_NEAR(Util::to_360(Units::to("deg",vo.trk()-PI/2)),Units::to("deg",tv.trk()),0.0001);
	EXPECT_NEAR(vo.gs(),tv.gs(),0.0001);
}

TEST_F(KinematicsTest, turnDone1) {
	Velocity vo = Velocity::makeTrkGsVs(30,100.0,0.0);
	Velocity target = Velocity::makeTrkGsVs(70,100.0,0.0);
	bool turnRight = true;
	bool tD = Kinematics::turnDone(vo.trk(), target.trk(), turnRight);
	EXPECT_FALSE(tD);
	target = Velocity::makeTrkGsVs(29.7,100.0,0.0);
	tD = Kinematics::turnDone(vo.trk(), target.trk(), turnRight);
	EXPECT_TRUE(tD);
	target = Velocity::makeTrkGsVs(30.1,100.0,0.0);
	tD = Kinematics::turnDone(vo.trk(), target.trk(), turnRight);
	EXPECT_FALSE(tD);
	target = Velocity::makeTrkGsVs(30.0,100.0,0.0);
	tD = Kinematics::turnDone(vo.trk(), target.trk(), turnRight);
	EXPECT_TRUE(tD);
	tD = Kinematics::turnDone(Units::from("deg",-22.05),Units::from("deg",266),true);
	EXPECT_TRUE(tD);
	tD = Kinematics::turnDone(Units::from("deg",-22.05),Units::from("deg",266),false);
	EXPECT_FALSE(tD);
}


TEST_F(KinematicsTest, testTurn) {
        Vect3 s0 = Vect3::makeXYZ(0,"nm",0,"nm",10000,"ft");
	Velocity v0 = Velocity::makeTrkGsVs(40.0,400,0);
	double maxBank = Units::from("deg",35);
	bool turnRight = true;
	double R = Kinematics::turnRadius(v0.gs(), maxBank);
	std::pair<Vect3,Velocity> svRO = Kinematics::turn(s0, v0, 5.0, R, turnRight);
	Vect3 ns = svRO.first;
	Velocity nv = svRO.second;
	EXPECT_NEAR(723.893,ns.x,0.001);
	EXPECT_NEAR(729.478,ns.y,0.001);
	EXPECT_NEAR(0.864,nv.trk(),0.001);
	maxBank = Units::from("deg",25);
	turnRight = false;
	R = Kinematics::turnRadius(v0.gs(), maxBank);
	svRO = Kinematics::turn(s0, v0, 31.0, R, turnRight);
	EXPECT_NEAR(2165.997,svRO.first.x,0.001);
	EXPECT_NEAR(5866.634,svRO.first.y,0.001);
	EXPECT_NEAR(0.009,svRO.second.trk(),0.001);
	double omega = Kinematics::turnRate(v0.gs(), maxBank);
	EXPECT_NEAR(0.0222,omega,0.001);
}




TEST_F(KinematicsTest, posAccel1) {
	Vect3 so3(0,0,0);
	Velocity vo3 = Velocity::mkVxyz(1,0,0);
	double a = 10;
	double t = 0;
	Vect3 pA = Kinematics::gsAccelPos(so3, vo3, t, a);
	EXPECT_NEAR(0.0, pA.x, 0.001);
	t = 1;
	pA = Kinematics::gsAccelPos(so3, vo3, t, a);
	EXPECT_NEAR(6.0, pA.x, 0.001);
	t = 10;
	vo3 = Velocity::mkVxyz(3,0,0);
	pA = Kinematics::gsAccelPos(so3, vo3, t, a);
	EXPECT_NEAR(530, pA.x, 0.001);
	t = 1;
	vo3 = Velocity::mkVxyz(0,1,0);
	pA = Kinematics::gsAccelPos(so3, vo3, t, a);
	EXPECT_NEAR(6, pA.y, 0.001);
}



TEST_F(KinematicsTest, turnUntiltrack) {
	Vect3 so = Vect3(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(90,300,0);
	double goalTrack = Units::from("deg",120);
	double bankAng = Units::from("deg",35.0);
	bool turnRight = true;
	double R = Kinematics::turnRadius(vo.gs(), bankAng);
	double omega = Kinematics::turnRate(vo.gs(), bankAng);
	double deltaAng = Util::turnDelta(vo.trk(), goalTrack, turnRight);
	double turnTime = deltaAng/omega;
	EXPECT_NEAR(0.04449259,omega,0.0000001);
	EXPECT_NEAR(Units::from("deg",30),deltaAng,0.00000001);
	EXPECT_NEAR(11.768223317,turnTime,0.000000001);
	Vect3 soEndTurn = Kinematics::turn(so, vo, turnTime, R, turnRight).first;
	Vect3 goalV = Velocity::makeTrkGsVs(Units::to("deg",goalTrack),300,0);
	double totalTime = turnTime + 24.5;
	Vect3 finalPlace = soEndTurn.linear(goalV,totalTime-turnTime);
	Vect3 vp = Kinematics::turnUntil(so, vo, totalTime, goalTrack, bankAng).first;
	EXPECT_NEAR(vp.x,finalPlace.x,0.000000000001);
	EXPECT_NEAR(vp.y,finalPlace.y,0.000000000001);
	EXPECT_NEAR(vp.z,finalPlace.z,0.000000000001);
}


TEST_F(KinematicsTest,centerofturn) {
	Vect3 so = Vect3(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(0,300,0);
	double bankAng = Units::from("deg",35.0);
	bool turnRight = true;
	Vect2 cot = Kinematics::centerOfTurn(so.vect2(), vo.vect2(), bankAng, turnRight);
	EXPECT_NEAR(3468.7421,cot.x,0.0001);
	EXPECT_NEAR(0.0,cot.y,0.0000000000001);
}


TEST_F(KinematicsTest, gsAccel) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(0,300,0);
	double goalGS = Units::from("kn",600.0);
	double gsAccel = Units::gn;
	double t = 10.0;
	Vect3 soAfter = Kinematics::gsAccelUntil(so, vo, t, goalGS, gsAccel).first;
	EXPECT_NEAR(0.0,soAfter.x,0.0001);
	double finalY = vo.gs()*t + 0.5*gsAccel*t*t;
	EXPECT_NEAR(finalY,soAfter.y,0.0001);
	goalGS = 0;
	soAfter = Kinematics::gsAccelUntil(so, vo, t, goalGS, gsAccel).first;
	finalY = vo.gs()*t - 0.5*gsAccel*t*t;
	//f.pln(" finalY = "+Units::str("nm",finalY));
	EXPECT_NEAR(finalY,soAfter.y,0.0001);

	vo = Velocity::makeTrkGsVs(200,300,0);
	Velocity voAfter = Kinematics::gsAccel(so, vo, t, gsAccel).second;
	Vect3 nvo = vo.Hat().Scal(vo.gs() + gsAccel*t);
	EXPECT_NEAR(voAfter.x, nvo.x,0.0001);
	EXPECT_NEAR(voAfter.y, nvo.y,0.0001);
	EXPECT_NEAR(voAfter.z, nvo.z,0.0001);

}

TEST_F(KinematicsTest, vsAccel) {
	Vect3 so(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(0,0,100);
	double vsAccel = 0.0;
	double t = Units::from("min",1.0);
	Vect3 soAfter = Kinematics::vsAccelPos(so, vo, t, vsAccel);
	EXPECT_NEAR(0.0,soAfter.x,0.0001);
	EXPECT_NEAR(0.0,soAfter.y,0.0001);
	EXPECT_NEAR(Units::from("ft",100.0),soAfter.z,0.0001);
	vsAccel = 10.0;
	soAfter = Kinematics::vsAccelPos(so, vo, t, vsAccel);
	EXPECT_NEAR(Units::from("ft",59155.12),soAfter.z,0.001);
	double goalVs = Units::from("fpm",1000.0);
	vsAccel = 10.0;
	soAfter = Kinematics::vsAccelUntil(so, vo, t, goalVs, vsAccel).first;
	Velocity voAfter = Kinematics::vsAccelUntil(so, vo, t, goalVs, vsAccel).second;
	EXPECT_NEAR(0.0,soAfter.x,0.0001);
	EXPECT_NEAR(0.0,soAfter.y,0.0001);
	EXPECT_NEAR(303.7548,soAfter.z,0.0001);
	EXPECT_NEAR(0.0,voAfter.trk(),0.0001);
	EXPECT_NEAR(0.0,voAfter.gs(),0.0001);
	EXPECT_NEAR(5.08,voAfter.vs(),0.0001);
	t = 0;
	vo = Velocity::makeTrkGsVs(0,500,1000);
	goalVs = Units::from("fpm",-2000.0);
	soAfter = Kinematics::vsAccelUntil(so, vo, t, goalVs, vsAccel).first;
	voAfter = Kinematics::vsAccelUntil(so, vo, t, goalVs, vsAccel).second;
	EXPECT_NEAR(0.0,soAfter.x,0.0001);
	EXPECT_NEAR(0.0,soAfter.y,0.0001);
	EXPECT_NEAR(0.0,soAfter.z,0.0001);
	EXPECT_NEAR(0.0,voAfter.trk(),0.0001);
	EXPECT_NEAR(257.2222,voAfter.gs(),0.0001);
	EXPECT_NEAR(5.08,voAfter.vs(),0.0001);
	t = 20;
	soAfter = Kinematics::vsAccelUntil(so, vo, t, goalVs, vsAccel).first;
	voAfter = Kinematics::vsAccelUntil(so, vo, t, goalVs, vsAccel).second;
	EXPECT_NEAR(0.0,soAfter.x,0.0001);
	EXPECT_NEAR(5144.4444,soAfter.y,0.0001);
	EXPECT_NEAR(-191.5871,soAfter.z,0.0001);
	EXPECT_NEAR(0.0,voAfter.trk(),0.0001);
	EXPECT_NEAR(257.2222,voAfter.gs(),0.0001);
	EXPECT_NEAR(-10.16,voAfter.vs(),0.0001);
}

TEST_F(KinematicsTest, turnIncrement) {
        Vect3 s0 = Vect3::makeXYZ(0,"nm",0,"nm",10000,"ft");
	Velocity v0 = Velocity::makeTrkGsVs(0,400,0);
	double maxBank = Units::from("deg",30);
	double timeStep = 1.0;
	bool turnRight = true;
	double rollInStartTm = 60;
	for (timeStep = 1.0; timeStep < 20; timeStep++) {
		Vect3 ns = s0;
		Velocity nv = v0;
		std::pair<Vect3,Velocity> svEnd = Kinematics::turn(ns, nv, rollInStartTm, maxBank,  turnRight);
		ns = svEnd.first;
		nv = svEnd.second;
		std::pair<Vect3,Velocity> svRO = Kinematics::turn(ns, nv, timeStep, maxBank,  turnRight);
		std::pair<Vect3,Velocity> svR2 = Kinematics::turn(s0, v0, rollInStartTm+timeStep, maxBank, turnRight);
		double sError = std::sqrt(Util::sq(svRO.first.x - svR2.first.x) + Util::sq(svRO.first.y - svR2.first.y));
		EXPECT_NEAR(0.000,sError,0.00000000001);
	}
}


TEST_F(KinematicsTest,turnTime1) {
	Velocity vo10 = Velocity::mkTrkGsVs(Units::from("deg",10.0),9.80665,0.0);   // a convoluted speed
	Velocity vo20 = Velocity::mkTrkGsVs(Units::from("deg",20.0),9.80665,0.0);   // a convoluted speed
	Velocity vo170 = Velocity::mkTrkGsVs(Units::from("deg",170.0),9.80665,0.0);   // a convoluted speed
	Velocity vo190 = Velocity::mkTrkGsVs(Units::from("deg",190.0),9.80665,0.0);   // a convoluted speed
	Velocity vo350 = Velocity::mkTrkGsVs(Units::from("deg",350.0),9.80665,0.0);   // a convoluted speed
	double trk10 = Units::from("deg", 10.0);
	double trk20 = Units::from("deg", 20.0);
	double trk350 = Units::from("deg", 350.0);
	double trk170 = Units::from("deg", 170.0);
	double trk190 = Units::from("deg", 190.0);
	double maxBank = Units::from("deg", 45.0);

	double t1a = Kinematics::turnTime(vo10, trk20, maxBank, true);
	double t1b = Kinematics::turnTime(vo20, trk10, maxBank, false);
	double t1c = Kinematics::turnTime(vo20, trk10, maxBank);
	double t1d = Kinematics::turnTime(vo10, trk20, maxBank);
	EXPECT_NEAR(0.17453292519943298, t1a, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298, t1b, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298, t1c, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298, t1d, 0.00000000000001);

	double t2a = Kinematics::turnTime( vo10, trk190, maxBank, true);
	double t2b = Kinematics::turnTime(vo190,  trk10, maxBank, false);
	double t2c = Kinematics::turnTime( vo10, trk190, maxBank, false);
	double t2d = Kinematics::turnTime(vo190,  trk10, maxBank, true);
	double t2e = Kinematics::turnTime( vo10, trk190, maxBank);
	double t2f = Kinematics::turnTime(vo190,  trk10, maxBank);
	EXPECT_NEAR(0.17453292519943298*18, t2a, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*18, t2b, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*18, t2c, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*18, t2d, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*18, t2e, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*18, t2f, 0.00000000000001);

	double t3a = Kinematics::turnTime( vo10, trk170, maxBank, true);
	double t3b = Kinematics::turnTime(vo170,  trk10, maxBank, false);
	double t3c = Kinematics::turnTime( vo10, trk170, maxBank);
	double t3d = Kinematics::turnTime(vo170,  trk10, maxBank);
	EXPECT_NEAR(0.17453292519943298*16, t3a, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*16, t3b, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*16, t3c, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*16, t3d, 0.00000000000001);

	double t4a = Kinematics::turnTime(vo190, trk350, maxBank, true);
	double t4b = Kinematics::turnTime(vo350, trk190, maxBank, false);
	double t4c = Kinematics::turnTime(vo190, trk350, maxBank);
	double t4d = Kinematics::turnTime(vo350, trk190, maxBank);
	EXPECT_NEAR(0.17453292519943298*16, t4a, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*16, t4b, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*16, t4c, 0.00000000000001);
	EXPECT_NEAR(0.17453292519943298*16, t4d, 0.00000000000001);
}


TEST_F(KinematicsTest,turnTime2) {
		//fpln(" ---------- in turnTime -----------");
		for (int trk = 0; trk < 360; trk += 30) {
			Velocity vo = Velocity::makeTrkGsVs(trk,100,0);
			double bankAngle = Units::from("deg",20.0);
			for (int i = 0; i < 360; i += 20) {
				double goal = Units::from("deg", i);
				double t1 = Kinematics::turnTime(vo, goal, bankAngle, false);
				double t2 = Kinematics::turnTime(vo, goal, bankAngle, true);
				if (i != trk) {
					EXPECT_NEAR(t1+t2, 90.559, 0.001);
				}
			}
		}
	}

TEST_F(KinematicsTest, testMore) {
	for (double bank = Units::from("deg",-45); bank < Units::from("deg",45); bank = bank + Units::from("deg", 5)) {
		for (double speed = Units::from("kn",100); speed < Units::from("kn",500); speed = speed + Units::from("kn",100)) {
			double turnRate = Kinematics::turnRate(speed,bank);
			//f.pln(" bank = "+Units::str("deg",bank)+" turnRate = "+Units::str("deg",turnRate));
			EXPECT_NEAR(bank,Kinematics::bankAngle(speed,turnRate),0.0001);
		}
	}
	Vect3 so = Vect3(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(0,300,0);
	double bank = Units::from("deg",25.0);
	double R = Kinematics::turnRadius(vo.gs(),bank);
	//for ( bool turnRight = true; turnRight == true; turnRight = !turnRight) {
	for (int i = 0; i < 2; ++i) {
		bool turnRight = (i == 0);
		//f.pln(" %%%%% turnRight = "+turnRight);
		for (double targetTrack = Units::from("deg",0.0); targetTrack <= Units::from("deg",360.0); targetTrack = targetTrack + Units::from("deg",3.0)) {
			double turnTime = Kinematics::turnTime(vo, targetTrack, bank, turnRight);
			std::pair<Vect3,Velocity> svEnd = Kinematics::turn(so, vo, turnTime, R,  turnRight);
			Vect3 ns = svEnd.first;
			Velocity nv = svEnd.second;
			//f.pln(" && "+Units::str("deg",targetTrack)+" "+Units::str("deg",nv.compassAngle()));
			EXPECT_NEAR(targetTrack, nv.compassAngle(),0.0000001);
			if (std::abs(targetTrack - Units::from("deg",180.0)) < 0.00001 || std::abs(targetTrack - Units::from("deg",360.0)) < 0.00001)
				EXPECT_NEAR(0.0,ns.y,0.0000001);
			if (turnRight || Util::almost_equals(ns.x,0.0))	EXPECT_EQ(1,Util::sign(ns.x));
			else EXPECT_EQ(-1,Util::sign(ns.x));
			if ((turnRight && targetTrack <= Units::from("deg",180.0))
					||  (!turnRight && targetTrack >= Units::from("deg",180.0))
					|| Util::almost_equals(ns.y,0.0))
				EXPECT_EQ(1,Util::sign(ns.y));
			else
				EXPECT_EQ(-1,Util::sign(ns.y));
		}
	}
	std::pair<Vect3,Velocity> svo = std::pair<Vect3,Velocity>(so,vo);
	for (double t = 0.0; t < 10.0; t++) {
		std::pair<Vect3,Velocity> lp = Kinematics::linear(svo,t);
		EXPECT_NEAR(svo.first.linear(svo.second,t).x,lp.first.x,0.00000001);
		EXPECT_NEAR(svo.first.linear(svo.second,t).y,lp.first.y,0.00000001);
		EXPECT_NEAR(svo.first.linear(svo.second,t).z,lp.first.z,0.00000001);
		EXPECT_EQ(vo,lp.second);
		svo = lp;
	}

}




TEST_F(KinematicsTest, testGsAccelUntil) {
	//f.pln(" ---------- in testGsAccelUntil -----------");
	Vect3 so = Vect3(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(23,300,0);
	double goalGS = Units::from("kn",532.0);
	double gsAccel = Units::from("m/s",5.2);
	double t = 100.0;
	std::pair<Vect3,Velocity> pat = Kinematics::gsAccelUntil(so, vo, t, goalGS, gsAccel);
   	EXPECT_NEAR(10158.5,pat.first.x,0.1);
  	EXPECT_NEAR(23931.9,pat.first.y,0.1);
   	EXPECT_NEAR(273.68,pat.second.gs(),0.01);
   	so = Vect3::mkXYZ(263.335569, 289.560000, 13.800832);
	vo = Velocity::makeTrkGsVs(270.00, 132.59,-703.68);
	std::pair<Vect3,Velocity> nsv = std::pair<Vect3,Velocity>(so,vo);
    double segmentTime = 0.5;
	goalGS = Units::from("kn",250);
	double escGsAccel = 5.0;
	std::pair<Vect3,Velocity> psv = Kinematics::gsAccelUntil(nsv, segmentTime, goalGS, escGsAccel);
    //f.pln(f.Fm1(t)+" $$ segmentTime = "+segmentTime+" psv.first = "+f.sStr8(psv.first));
	EXPECT_NEAR(228.605,psv.first.x,0.1);
	EXPECT_NEAR(289.56,psv.first.y,0.1);
	EXPECT_NEAR(12.013,psv.first.z,0.1);
}

TEST_F(KinematicsTest,testVsAccelUntil) {
	Vect3 so = Vect3(0,0,0);
	Velocity vo = Velocity::makeTrkGsVs(23,300,-100.0);
	double goalVS = Units::from("fpm",2300);
	double vsAccel = Units::from("m/s",5.5);
	double t = 100.0;
	std::pair<Vect3,Velocity> pat = Kinematics::vsAccelUntil(so, vo, t, goalVS, vsAccel);
   	EXPECT_NEAR(6030.28,pat.first.x,0.1);
  	EXPECT_NEAR(14206.45,pat.first.y,0.1);
   	EXPECT_NEAR(11.68,pat.second.vs(),0.01);
   	for (double tt = 1; tt < 100.0; tt++) {
		std::pair<Vect3,Velocity> patA = Kinematics::vsAccelUntil(so, vo, t, goalVS, vsAccel);
		std::pair<Vect3,Velocity> patB = Kinematics::vsAccelUntil(std::pair<Vect3,Velocity>(so, vo), t, goalVS, vsAccel);
       	EXPECT_NEAR(patB.first.x,patA.first.x,0.1);
      	EXPECT_NEAR(patB.first.y,patA.first.y,0.1);
       	EXPECT_NEAR(patB.second.vs(),patA.second.vs(),0.01);
   	}

}




TEST_F(KinematicsTest, testVsLevelOut) {
		//Vect3 so = new Vect3(0,0,10000);
		//Velocity vo = Velocity::makeTrkGsVs(23,300,-400.0);
                Vect3 s0 = Vect3::makeXYZ(0,"nm",0,"nm",10000,"ft");
		Velocity v0 = Velocity::makeTrkGsVs(23,300,-400.0);
		std::pair<Vect3,Velocity> svo = std::pair<Vect3,Velocity>(s0,v0);
		double climbRate = Units::from("fpm",2300);
		double targetAlt = Units::from("ft",11200);
		double vsAccel = Units::from("m/s",1.5);
		double tL = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel,true);
		EXPECT_NEAR(40.56,tL,0.01);
		double t = 5.0;
		std::pair<Vect3,Velocity> pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(301.514,pat.first.x,0.1);
		EXPECT_NEAR(710.323,pat.first.y,0.1);
		EXPECT_NEAR(3056.59,pat.first.z,0.1);
		EXPECT_NEAR(5.468,pat.second.vs(),0.01);
		t = 6.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(6.97,pat.second.vs(),0.01);
		EXPECT_NEAR(3062.808,pat.first.z,0.1);
		t = 10.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(11.68,pat.second.vs(),0.01);
		EXPECT_NEAR(3102.13,pat.first.z,0.1);
		t = 15.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(904.542,pat.first.x,0.1);
		EXPECT_NEAR(2130.96,pat.first.y,0.1);
		EXPECT_NEAR(11.68,pat.second.vs(),0.01);
		EXPECT_NEAR(3160.55,pat.first.z,0.1);
		t = 100.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(6030.28,pat.first.x,0.1);
		EXPECT_NEAR(14206.45,pat.first.y,0.1);
		EXPECT_NEAR(0,pat.second.vs(),0.01);
		EXPECT_NEAR(3413.76,pat.first.z,0.1);
		double T3 = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel, true);
		pat = Kinematics::vsLevelOut(svo, T3, climbRate, targetAlt, vsAccel, true);
		StateVector nsv = Kinematics::vsLevelOutFinal(svo, climbRate, targetAlt, vsAccel, true);
		bool pEq = nsv.pair() == pat;
		EXPECT_TRUE(pEq);
		targetAlt = Units::from("ft",11000);
		climbRate = Units::from("fpm", 3000);
		s0 = Vect3::makeXYZ(4.466,"nm",0.011,"nm",10000,"ft");
		v0 = Velocity::makeTrkGsVs(90.00,160.185,800.00);
		vsAccel = Units::from("m/s",2.0);
		t = 1.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(-0.03,pat.second.vs(),0.01);
		EXPECT_NEAR(3046.968,pat.first.z,0.1);
		t = 2.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(1.968,pat.second.vs(),0.01);
		EXPECT_NEAR(3047.94,pat.first.z,0.1);
		t = 3.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
		EXPECT_NEAR(3.968,pat.second.vs(),0.01);
		EXPECT_NEAR(3050.90,pat.first.z,0.1);


		t = 5.0;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
       	EXPECT_NEAR(7.968,pat.second.vs(),0.01);
    	EXPECT_NEAR(3062.84,pat.first.z,0.1);
    	Tuple5<double,double,double,double,double> qV =  Kinematics::vsLevelOutTimes(svo,climbRate,targetAlt, vsAccel,  true);
       	EXPECT_NEAR(8.636,qV.first,0.001);
       	EXPECT_NEAR(21.083,qV.second,0.001);
       	EXPECT_NEAR(28.703,qV.third,0.001);
       	//EXPECT_NEAR(15.24,qV.fourth,0.001);
		t = qV.first;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
       	EXPECT_NEAR(15.24,pat.second.vs(),0.01);
    	EXPECT_NEAR(3105.032,pat.first.z,0.1);
		t = 15;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
       	EXPECT_NEAR(15.24,pat.second.vs(),0.01);
    	EXPECT_NEAR(3202.019,pat.first.z,0.1);
		t = 21.083;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
       	EXPECT_NEAR(15.24,pat.second.vs(),0.01);
    	EXPECT_NEAR(3294.724,pat.first.z,0.1);
		t = 25;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
       	EXPECT_NEAR(7.407,pat.second.vs(),0.01);
    	EXPECT_NEAR(3339.082,pat.first.z,0.1);
		t = 31;
		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
       	EXPECT_NEAR(0.0,pat.second.vs(),0.01);
    	EXPECT_NEAR(3352.8,pat.first.z,0.1);
}



TEST_F(KinematicsTest, testVsLevelOut2) {  // very small climb rate
	//fpln(" ---------------- testVsLevelOut2 ----------------------------------------");
	//Vect3 so = new Vect3(0,0,10000);
	//Velocity vo = Velocity::makeTrkGsVs(23,300,-400.0);
    Vect3 s0 = Vect3::mkXYZ(0.000000, 0.000000, 6096.000000);
    Velocity    v0 = Velocity::makeTrkGsVs(0.00, 100.00, -2773.00);
    double    climbRate = -1.6826846194058422; // Units::from("fpm",2773.0);   //
    double    targetAlt = 3360.395448767623;
    double    vsAccel = 2.0;
	std::pair<Vect3,Velocity> svo = std::pair<Vect3,Velocity>(s0,v0);
	double tL = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel, true);
    EXPECT_NEAR(197.717,tL,0.01);
    Tuple5<double,double,double,double,double> qV = Kinematics::vsLevelOutTimes(svo,climbRate,targetAlt,vsAccel, true);
   	EXPECT_NEAR(0.0,qV.first,0.01);
  	EXPECT_NEAR(190.674,qV.second,0.01);
  	EXPECT_NEAR(197.72,qV.third,0.01);
  	//EXPECT_NEAR(-14.087,qV.fourth,0.01);
	double t = 1.0;
	std::pair<Vect3,Velocity> pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
   	EXPECT_NEAR(0,pat.first.x,0.1);
  	EXPECT_NEAR(51.44,pat.first.y,0.1);
 	EXPECT_NEAR(6081.91,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 5.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(6025.56,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 100.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(4687.32,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 191.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(3405.519,pat.first.z,0.1);
   	EXPECT_NEAR(-13.43,pat.second.vs(),0.01);
	t = 198.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(targetAlt,pat.first.z,0.1);
   	EXPECT_NEAR(0,pat.second.vs(),0.01);
	t = 250.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(targetAlt,pat.first.z,0.1);
   	EXPECT_NEAR(0,pat.second.vs(),0.01);
}


TEST_F(KinematicsTest, testVsLevelOut3) {     // NASTY CASE: overshoot
	//fpln(" ---------------- testVsLevelOut3 ----------------------------------------");
	//Vect3 so = new Vect3(0,0,10000);
	//Velocity vo = Velocity::makeTrkGsVs(23,300,-400.0);
    Vect3 s0 = Vect3::makeXYZ(0.000000,"nm", 0.000000,"nm", 20000.000000,"ft");
    Velocity v0 = Velocity::makeTrkGsVs(0.00, 100.00, -2773.00);
    double climbRate = -2; // Units::from("fpm",2773.0);   //
    double targetAlt = Units::from("ft",19900); // 3360.395448767623;
    double vsAccel = 2.0;
	std::pair<Vect3,Velocity> svo = std::pair<Vect3,Velocity>(s0,v0);
	double tL = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel, true);
	EXPECT_NEAR(17.608,tL,0.01);
    Tuple5<double,double,double,double,double> qV = Kinematics::vsLevelOutTimes(svo,climbRate,targetAlt,vsAccel, true);
    EXPECT_NEAR(8.043,qV.first,0.01);
    EXPECT_NEAR(16.608,qV.second,0.01);
    EXPECT_NEAR(17.608,qV.third,0.01);

  	//EXPECT_NEAR(-14.087,qV.fourth,0.01);
	double t = 1.0;
	std::pair<Vect3,Velocity> pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
   	//EXPECT_NEAR(Double.NaN,pat.first.x,0.1);
  	//EXPECT_NEAR(Double.NaN,pat.first.y,0.1);
 	//EXPECT_NEAR(Double.NaN,pat.first.z,0.1);
   	//EXPECT_NEAR(Double.NaN,pat.second.vs(),0.01);
}

TEST_F(KinematicsTest,testVsLevelOut4) {
	//fpln(" ---------------- testVsLevelOut4 ----------------------------------------");
	//Vect3 so = new Vect3(0,0,10000);
	//Velocity vo = Velocity::makeTrkGsVs(23,300,-400.0);g
    Vect3 s0 = Vect3::makeXYZ(0.000000,"nm", 0.000000,"nm", 20000.000000,"ft");
    Velocity v0 = Velocity::makeTrkGsVs(0.00, 100.00, -2773.00);
    double climbRate = Units::from("fpm",-800);   //
    double targetAlt = Units::from("ft",18000); // 3360.395448767623;
    double vsAccel = 2.0;
	std::pair<Vect3,Velocity> svo = std::pair<Vect3,Velocity>(s0,v0);
	double tL = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel, true);
    EXPECT_NEAR(46.79,tL,0.01);
    Tuple5<double,double,double,double,double> qV = Kinematics::vsLevelOutTimes(svo,climbRate,targetAlt,vsAccel, true);
   	EXPECT_NEAR(0,qV.first,0.01);
  	EXPECT_NEAR(39.75,qV.second,0.01);
  	EXPECT_NEAR(46.79,qV.third,0.01);
  	//EXPECT_NEAR(-14.087,qV.fourth,0.01);
	double t = 1.0;
	std::pair<Vect3,Velocity> pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
   	EXPECT_NEAR(0,pat.first.x,0.1);
  	EXPECT_NEAR(51.44,pat.first.y,0.1);
 	EXPECT_NEAR(6081.91,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 30.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(5673.39,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 50.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(5486.4,pat.first.z,0.1);
   	EXPECT_NEAR(0,pat.second.vs(),0.01);

   	//  NOW WITHOUT allowClimbRateChange = false
	tL = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel, true);
    EXPECT_NEAR(46.79,tL,0.01);

	qV = Kinematics::vsLevelOutTimes(svo,climbRate,targetAlt,vsAccel, false);
	// **** OLD ******
//       	assertTrue(qV.first <= 0);
//      	EXPECT_NEAR(155.16,qV.second,0.01);
//      	EXPECT_NEAR(157.195,qV.third,0.01);
//      	EXPECT_NEAR(-4.064,qV.fourth,0.01);
//		t = 1.0;
//		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
//       	EXPECT_NEAR(0,pat.first.x,0.1);
//      	EXPECT_NEAR(51.44,pat.first.y,0.1);
//     	EXPECT_NEAR(6091.93,pat.first.z,0.1);
//       	EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
//		t = 30.0;
//		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
//     	EXPECT_NEAR(5974.08,pat.first.z,0.1);
//       	EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
//		t = 50.0;
//		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
//     	EXPECT_NEAR(5892.8,pat.first.z,0.1);
//       	EXPECT_NEAR(-4.064,pat.second.vs(),0.01);
//		t = 156.0;
//		pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
//     	EXPECT_NEAR(5462.71,pat.first.z,0.1);
//       	EXPECT_NEAR(-2.391,pat.second.vs(),0.01);
   	// ******** NEW **********
   	EXPECT_TRUE(qV.first > 0);
  	EXPECT_NEAR(142.80,qV.second,0.01);
  	EXPECT_NEAR(144.84,qV.third,0.01);
  	//EXPECT_NEAR(-4.064,qV.fourth,0.01);
	t = 1.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
   	EXPECT_NEAR(0,pat.first.x,0.1);
  	EXPECT_NEAR(51.44,pat.first.y,0.1);
 	EXPECT_NEAR(6082.913,pat.first.z,0.1);
   	EXPECT_NEAR(-12.084,pat.second.vs(),0.01);
	t = 30.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
 	EXPECT_NEAR(5948.96,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 50.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
 	EXPECT_NEAR(5867.68,pat.first.z,0.1);
   	EXPECT_NEAR(-4.064,pat.second.vs(),0.01);
	t = 143.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
 	EXPECT_NEAR(5489.7,pat.first.z,0.1);
   	EXPECT_NEAR(-3.6725,pat.second.vs(),0.01);
	t = 156.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, false);
 	EXPECT_NEAR(5486.4,pat.first.z,0.1);
   	EXPECT_NEAR(0,pat.second.vs(),0.01);

}

TEST_F(KinematicsTest,testVsLevelOut5) {
	//fpln(" ---------------- testVsLevelOut5 ----------------------------------------");
	//Vect3 so = new Vect3(0,0,10000);
	//Velocity vo = Velocity::makeTrkGsVs(23,300,-400.0);
    Vect3 s0 = Vect3::makeXYZ(0.000000,"nm", 0.000000,"nm", 20000.000000,"ft");
    Velocity v0 = Velocity::makeTrkGsVs(20.00, 200.00, 100.00);
    double climbRate = Units::from("fpm",100.0);   //
    double targetAlt = Units::from("ft",20300); // 3360.395448767623;
    double vsAccel = 2.0;
	std::pair<Vect3,Velocity> svo = std::pair<Vect3,Velocity>(s0,v0);
	double tL = Kinematics::vsLevelOutTime(svo, climbRate, targetAlt, vsAccel, true);
    EXPECT_NEAR(180.12,tL,0.01);
    Tuple5<double,double,double,double,double> qV = Kinematics::vsLevelOutTimes(svo,climbRate,targetAlt,vsAccel, true);
   	EXPECT_NEAR(0,qV.first,0.01);
  	EXPECT_NEAR(179.87,qV.second,0.01);
  	EXPECT_NEAR(180.12,qV.third,0.01);
  	//EXPECT_NEAR(0.508,qV.fourth,0.01);
	double t = 1.0;
	std::pair<Vect3,Velocity> pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
   	EXPECT_NEAR(35.19,pat.first.x,0.1);
  	EXPECT_NEAR(96.68,pat.first.y,0.1);
 	EXPECT_NEAR(6096.51,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 30.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(6111.24,pat.first.z,0.1);
   	//EXPECT_NEAR(qV.fourth,pat.second.vs(),0.01);
	t = 250.0;
	pat = Kinematics::vsLevelOut(svo, t, climbRate, targetAlt, vsAccel, true);
 	EXPECT_NEAR(6187.44,pat.first.z,0.1);
   	EXPECT_NEAR(0,pat.second.vs(),0.01);
  	EXPECT_NEAR(8797.51,pat.first.x,0.1);
  	EXPECT_NEAR(24170.98,pat.first.y,0.1);
}


//
//#ifdef _MSC_VER
//int main(int argc, char** argv)
//{
//
//	testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
//
//	system ("pause");
//}
//#endif


TEST_F(KinematicsTest, testDirectToPoint) {
    Vect3 s0 = Vect3::makeXYZ(0.0,"nm", 0.0,"nm", 20000.0,"ft");
    Velocity v0 = Velocity::makeTrkGsVs(0.0, 200.00, 0.0);
    double R = Units::from("nm", 2.0);

    Vect3 wp = Vect3::makeXYZ(4.0,"nm", 2.0,"nm", 20000.0,"ft");
    Quad<Vect3,Velocity,double,int> dtp = Kinematics::directToPoint(s0,v0, wp, R);
    Vect3 EOT = Vect3::makeXYZ(2.0,"nm", 2.0,"nm", 20000.0,"ft");

    EXPECT_NEAR(EOT.x,dtp.first.x,0.0001);
    EXPECT_NEAR(EOT.y,dtp.first.y,0.0001);
    EXPECT_NEAR(EOT.z,dtp.first.z,0.0001);
    EXPECT_NEAR(M_PI/2.0,dtp.second.trk(),0.001);
    EXPECT_NEAR(v0.gs(),dtp.second.gs(),0.001);
    EXPECT_NEAR(v0.vs(),dtp.second.vs(),0.001);
    double tEOT = (M_PI*R/2.0)/v0.gs();
    EXPECT_NEAR(tEOT,dtp.third,0.001);
    //fpln("$$$$$$$$$ tEOT = "+tEOT);
    //double bankAngle = Kinematics::bankAngleRadius(v0.gs(), R);
    //double tDt= Kinematics::turnTimeDirectTo(s0,v0, wp, bankAngle);
    //EXPECT_NEAR(tDt,dtp.third,0.0001);
    wp = Vect3::makeXYZ(1.0,"nm", 1.0,"nm", 20000.0,"ft");    // inside circle
    dtp = Kinematics::directToPoint(s0,v0, wp, R);
    EXPECT_TRUE(dtp.first.isInvalid());        // returns invalid value
    wp = Vect3::makeXYZ(10.0,"nm", 0.0,"nm", 20000.0,"ft");
    //int dir = 1;
    //double omega = dir*v0.gs()/R;
     dtp = Kinematics::directToPoint(s0,v0, wp, R);
    //fpln(" dtp = "+dtp.first.toStringNP()+" "+dtp.second);
    Vect3 dtp1 = Vect3::makeXYZ(2.5,"nm",1.9365,"nm",20000,"ft");
    EXPECT_NEAR(4630,dtp.first.x,0.001);
    EXPECT_NEAR(3586.3825,dtp.first.y,0.001);
    EXPECT_NEAR(6096,dtp.first.z,0.001);
    wp = Vect3::makeXYZ(10.0,"nm", 10.0,"nm", 20000.0,"ft");
    dtp = Kinematics::directToPoint(s0,v0, wp, R);
    //fpln(" dtp = "+dtp.first.toStringNP()+" "+dtp.second);
    double bankAngle = Units::from("deg", 10.0);
    R = Kinematics::turnRadius(v0.gs(), bankAngle);
    //double omega = dir*v0.gs()/R;
    //dtp = Kinematics::directToPoint(s0,v0, wp, R);
    //double tDt= Kinematics::turnTimeDirectTo(s0,v0, wp, bankAngle);
    //EXPECT_NEAR(tDt,dtp.third,0.0001);
    s0 = Vect3::makeXYZ(4.0,"nm", 0.0,"nm", 20000.0,"ft");
    v0 = Velocity::makeTrkGsVs(0.0, 200.00, 0.0);
    R = Units::from("nm", 4.0);
    EOT = Vect3::makeXYZ(4.0*std::cos(M_PI/4.0),"nm", 4.0*std::sin(M_PI/4.0),"nm", 20000.0,"ft");
    Vect3 vv = Velocity::makeTrkGsVs(-45.0, 200.00, 0.0);
    wp = EOT.linear(vv, 1.0);
    dtp = Kinematics::directToPoint(s0,v0, wp, R);
    EXPECT_NEAR(EOT.x,dtp.first.x,0.0001);
    EXPECT_NEAR(EOT.y,dtp.first.y,0.0001);
    EXPECT_NEAR(EOT.z,dtp.first.z,0.0001);
    EXPECT_NEAR(Units::from("deg", -45.0),dtp.second.trk(),0.001);
    EXPECT_NEAR(v0.gs(),dtp.second.gs(),0.001);
    EXPECT_NEAR(v0.vs(),dtp.second.vs(),0.001);
    tEOT = (M_PI*R/4.0)/v0.gs();
    EXPECT_NEAR(tEOT,dtp.third,0.001);
}



TEST_F(KinematicsTest, testDirectToAcuteHughGsOut) {
	Plan lpc;
	Position p0  = Position::makeXYZ(0.000000, 0.000000, 10000.000000);
	Position p1  = Position::makeXYZ(1.827000, -4.521900, 10000.000000);
	Position p2  = Position::makeXYZ(1.111100, 11.111100, 10000.000000);
	NavPoint np0(p0,0.000000);       lpc.addNavPoint(np0);
	NavPoint np1(p1,52.2716);          lpc.addNavPoint(np1);
	NavPoint np2(p2,220.000000);     lpc.addNavPoint(np2);
	Position wpp = np2.position();
	Velocity vin =  lpc.finalVelocity(0);          //         Velocity vin = kpc.finalVelocity(i-1);
	//DebugSupport.dumpPlan(lpc, "testDirectToAcute_initial");
	double bankAngle = Units::from("deg", 20);
	double R = Kinematics::turnRadius(vin.gs(), bankAngle);
	Quad<Position,Velocity,double,int> dtp = Kinematics::directToPoint(p1.vect3(),vin,wpp.vect3(),R);
	double turnTm = dtp.third;
	int turnDir = dtp.fourth;
	EXPECT_NEAR(164.9107,turnTm,0.001);
	EXPECT_NEAR(-1,turnDir,0.00000001);
	NavPoint np = lpc.point(1);
	NavPoint eot_NP(Position(dtp.first), np.time()+turnTm);
	Velocity v2 = eot_NP.initialVelocity(np2);
	int dir = Util::turnDir(vin.trk(), v2.trk());
	EXPECT_FALSE(dir == turnDir);
		//fpln(" $$$  ADD eot_NP = "+eot_NP+" dtp.second = "+dtp.second+" dtp.third = "+dtp.third);
	//fpln(" $$$:  vin = "+vin+" v2 = "+v2+" dir = "+dir);
	double omega = turnDir*vin.gs()/R;    // turnRate
	EXPECT_NEAR(-0.02065,omega,0.001);
	EXPECT_NEAR(17804.8687,eot_NP.x(),0.001);
	EXPECT_NEAR(-185.994,eot_NP.y(),0.001);
	EXPECT_NEAR(3048,eot_NP.z(),0.001);
	NavPoint npEOT(eot_NP.position(),eot_NP.time());
	double trkOutOfEOT = npEOT.initialVelocity(np2).compassAngle();
	double speedOutOfEOT = npEOT.initialVelocity(np2).gs();
	EXPECT_NEAR(Units::from("deg",322.82),trkOutOfEOT,0.1);
	EXPECT_NEAR(Units::from("kn",17977.7),speedOutOfEOT,0.1);        // AGH ... huge ground speed !!!
}









/*
 * Class: UnitsTest
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#include "Units.h"
#include "format.h"
#include <cmath>
#include <gtest/gtest.h>
#include "Projection.h"
#include "VectFuns.h"
#include "Kinematics.h"

using namespace larcfm;
using namespace std;

class VectFunsTest : public ::testing::Test {

public:

protected:
	virtual void SetUp() {
	}
};


TEST_F(VectFunsTest, testLoS) {
	double D = Units::from("nm",5.1);					// Lateral separation distance
	double H = Units::from("ft",950.0);				// Vertical separation distance

	Position soP = Position::makeLatLonAlt(44.878101, -93.21869, 10000.0);
	Velocity  vo = Velocity::makeTrkGsVs(30.0,300.0,0.0);
	EuclideanProjection proj = Projection::createProjection(soP);
	Vect3 so = proj.project(soP);
	for (double t = 0; t < 100; t = t + 0.1000001) {
		Position siP = soP.linear(vo,t);
		Vect3 si = proj.project(siP);
		double distH = soP.distanceH(siP);
		bool losF = VectFuns::LoS(so,si,D,H);
		//fpln(" t = "+t+" losF = "+losF+" distH = "+Units::str("nm",distH));
		if (distH < D) {
			EXPECT_TRUE(losF);
		} else {
			EXPECT_FALSE(losF);
		}
		bool losGeodesic = soP.LoS(siP,D,H);
		EXPECT_EQ(losF,losGeodesic);
	}
}


TEST_F(VectFunsTest, testRightOfLine) {
	Vect2 so1 = Vect2(2.0, 4.0);
	Vect2 vo1 = Vect2(0.0, 1.0);
	Vect2 so2 = Vect2(6.0, 4.0);
	EXPECT_TRUE(VectFuns::rightOfLine(so1, vo1, so2));
	so2 = Vect2(-1.0, 4.0);
	EXPECT_FALSE(VectFuns::rightOfLine(so1, vo1, so2));
	vo1 = Vect2(0.5, -1.0);
	EXPECT_TRUE(VectFuns::rightOfLine(so1, vo1, so2));
}


TEST_F(VectFunsTest, testCollinear) {
	Vect2 so1 = Vect2(2.0, 4.0);
	Vect2 so2 = Vect2(6.0, 4.0);
	Vect2 so3 = Vect2(8.0, 4.0);
	Vect2 so3e(8.0, 4.00000000000001);
	EXPECT_TRUE(VectFuns::collinear(so1, so2, so2));
	EXPECT_TRUE(VectFuns::collinear(so1, so2, so3));
	EXPECT_FALSE(VectFuns::collinear(so1, so2, so3e));
	EXPECT_TRUE(VectFuns::collinear(so1, so1, so1));
	Vect3 si1 = Vect3(2.0, 4.0,0.0);
	Vect3 si2 = Vect3(4.0, 8.0,0.0);
	Vect3 si3 = Vect3(8.0, 16.0,0.0);
	EXPECT_TRUE(VectFuns::collinear(si1, si2, si3));
	EXPECT_TRUE(VectFuns::collinear(si1, si2, si2));
	EXPECT_TRUE(VectFuns::collinear(si1.vect2(), si2.vect2(), si3.vect2()));
}


TEST_F(VectFunsTest, testMidpoint) {
	Vect2 so1= Vect2(2.0, 4.0);
	Vect2 so2= Vect2(6.0, 4.0);
	EXPECT_NEAR(4.0,VectFuns::midPoint(so1, so2).x,0.0001);
	EXPECT_NEAR(4.0,VectFuns::midPoint(so1, so2).y,0.0001);
}


TEST_F(VectFunsTest, testInterpolate) {
	Velocity  v0 = Velocity::makeTrkGsVs(30.0,300.0,0.0);
	Velocity  v1 = Velocity::makeTrkGsVs(50.0,300.0,0.0);
	double alpha = 0.5;
	Velocity vInt = Velocity::make(VectFuns::interpolate(v0,v1,alpha));
	Velocity vAlt = Velocity::make(v0.Scal(alpha).Add(v1.Scal(1-alpha)));
	//fpln(" ## vInt = "+vInt+" vAlt = "+vAlt);
	EXPECT_NEAR(Units::from("deg",40.0),vInt.trk(),0.0001);
	EXPECT_NEAR(Units::from("kn",295.4423),vInt.gs(),0.0001);
	EXPECT_NEAR(0.0,vInt.vs(),0.0001);
	EXPECT_TRUE(vInt == vAlt);
	//--------------------------------
	Velocity vVel = Velocity::make(VectFuns::interpolateVelocity(v0,v1,alpha));
	EXPECT_NEAR(Units::from("deg",40.0),vVel.trk(),0.0001);
	EXPECT_NEAR(Units::from("kn",300.0),vVel.gs(),0.0001);
	EXPECT_NEAR(0.0,vVel.vs(),0.0001);
	for (double ff = 0; ff <= 1.0; ff = ff+0.1) {
		vVel = Velocity::make(VectFuns::interpolateVelocity(v0,v1,ff));
		EXPECT_NEAR(Units::from("kn",300.0),vVel.gs(),0.0001);
		EXPECT_NEAR(0.0,vVel.vs(),0.0001);
		//fpln(" ## vVel = "+vVel);
		EXPECT_TRUE(Units::from("deg",30.0) <= vVel.trk()+1E-16);
		EXPECT_TRUE(vVel.trk() <= Units::from("deg",50.0));
	}

	//--------------------------------
	Vect3 s0 = Vect3::mkXYZ(0,0,0);
	double bank = Units::from("deg",30.0);
	double gs = Units::from("kn",300.0);
	double deltaTrack = Units::from("deg",20.0);
	double turnTm = Kinematics::turnTime(gs,deltaTrack,bank);
	double omega = deltaTrack/turnTm;
	Vect3 sMid = Kinematics::turnOmega(s0,v0,turnTm/2,omega).first;
	Velocity vMid = Kinematics::turnOmega(s0,v0,turnTm/2,omega).second;
	Vect3 sEnd = Kinematics::turnOmega(s0,v0,turnTm,omega).first;
	//fpln(" .. vMid = "+vMid.toString());
	//fpln(" .. vMid.x = "+vMid.x);
	EXPECT_NEAR(99.203,vMid.x,0.001);
	EXPECT_NEAR(118.226,vMid.y,0.001);
	EXPECT_NEAR(0.0,vMid.z,0.001);
	Velocity vCalc = Velocity::make(sEnd.Sub(sMid).Scal(1.0/(turnTm/2.0)));
	fpln(" vCalc = "+vCalc.toString());
	EXPECT_NEAR(108.991,vCalc.x,0.001);
	EXPECT_NEAR(108.991,vCalc.y,0.001);
	EXPECT_NEAR(0.0,vCalc.z,0.001);
}


TEST_F(VectFunsTest, testIntersect) {
	Vect3 so1(2.0, 4.0, 0.0);
	Vect3 so2(6.0, 4.0, 0.0);
	double dto = 1;
	Vect3 si1(8.0, -1.0, 0.0);
	Vect3 si2(8.0, 8.0, 0.0);
	//double dti = 5;
	std::pair<Vect3,double> p1 = VectFuns::intersectionAvgZ(so1, so2, dto, si1, si2);
	//fpln(" p1 = "+p1);
	EXPECT_NEAR(8.0,p1.first.x,0.0001);
	EXPECT_NEAR(4.0,p1.first.y,0.0001);
	EXPECT_NEAR(0.0,p1.first.z,0.0001);
	EXPECT_NEAR(1.5,p1.second,0.0001);
	p1 = VectFuns::intersectionAvgZ(so2, so1, dto, si1, si2);
	//fpln(" p1 = "+p1);
	EXPECT_NEAR(8.0,p1.first.x,0.0001);
	EXPECT_NEAR(4.0,p1.first.y,0.0001);
	EXPECT_NEAR(0.0,p1.first.z,0.0001);
	EXPECT_NEAR(-0.5,p1.second,0.0001);
	p1 = VectFuns::intersectionAvgZ(so1, so2, dto, si2, si1);
	//fpln(" p1 = "+p1);
	EXPECT_NEAR(8.0,p1.first.x,0.0001);
	EXPECT_NEAR(4.0,p1.first.y,0.0001);
	EXPECT_NEAR(0.0,p1.first.z,0.0001);
	EXPECT_NEAR(1.5,p1.second,0.0001);
}


TEST_F(VectFunsTest, testIntersect2) {
	//Plan ac_1();
	Position p0(LatLonAlt::make(6.602521, 9.472367, 5000.000000));
	Position p1(LatLonAlt::make(8.121851, 11.910798, 6000.000000));
	//NavPoint np0(p0,0.000000);     ac_1.add(np0);
	//NavPoint np1(p1,2056.265300);     ac_1.add(np1);
	//Plan ac_2();
	Position p2(LatLonAlt::make(5.833477, 15.080758, 5000.000000));
	Position p3(LatLonAlt::make(8.440723, 15.343358, 5000.000000));
	//NavPoint np2(p2,0.000000);     ac_2.add(np2);
	//NavPoint np3(p3,1886.566400);     ac_2.add(np3);
	double dto = 2056.27;
	std::pair<Position,double> intersec = Position::intersection(p0, p1, dto, p2, p3);
	//fpln(" %% intersec = "+intersec);
	EXPECT_NEAR(0.1803,intersec.first.lat(),0.0001);
	EXPECT_NEAR(0.2711,intersec.first.lon(),0.0001);
//	EXPECT_NEAR(Units::from("ft",5500),intersec.first.alt(),0.01);
	EXPECT_NEAR(5084.178,intersec.second,0.01);
	std::pair<Position,double> intersec2 = Position::intersection(p1, p0, dto, p2, p3);
	//fpln(" %% intersec2 = "+intersec2);
	EXPECT_NEAR(0.1803,intersec2.first.lat(),0.0001);
	EXPECT_NEAR(0.2711,intersec2.first.lon(),0.0001);
//	EXPECT_NEAR(Units::from("ft",5500),intersec2.first.alt(),0.01);
	EXPECT_NEAR(dto-5084.178,intersec2.second,0.01);

	Velocity v0 = p0.initialVelocity(p1,dto);
	Velocity v2 = p2.initialVelocity(p3,dto);
	std::pair<Position,double> intersec3 = Position::intersection(p0,v0,p2,v2);
	//fpln(" ## intersec3 = "+intersec3);
	EXPECT_NEAR(0.1803,intersec3.first.lat(),0.001);
	EXPECT_NEAR(0.2711,intersec3.first.lon(),0.001);
	EXPECT_NEAR(Units::from("ft",5000),intersec3.first.alt(),0.01);  // takes altitude of p0
	EXPECT_NEAR(5084.178,intersec3.second,0.01);
	std::pair<Position,double> intersec4 = Position::intersection(p1,Velocity(v0.Neg()),p2,v2);
	//fpln(" ## intersec4 = "+intersec4);
	EXPECT_NEAR(0.1803,intersec4.first.lat(),0.001);
	EXPECT_NEAR(0.2711,intersec4.first.lon(),0.001);
	EXPECT_NEAR(Units::from("ft",6000),intersec4.first.alt(),0.01);  // // takes altitude of p1
	//EXPECT_NEAR(dto-5084.178,intersec4.second,0.01);
}



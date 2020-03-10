/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


// Uses the Google unit test framework.

#include "Vect2.h"
#include "Vect3.h"
#include "Vect4.h"
#include "Units.h"
#include "Velocity.h"
#include "NavPoint.h"
#include <gtest/gtest.h>

#include <cmath>

using namespace larcfm;

class VelocityTest : public ::testing::Test {

public:
  double trk1;
  double trk2;
  double gs;

protected:	
  virtual void SetUp() {
    trk1 = Units::from("deg", 180.0);
    trk2 = Units::from("deg",  90.0);
    gs = Units::from("knot", 600.0);
  }
};

TEST_F(VelocityTest, testTrkgs2vx) {
  EXPECT_NEAR(Units::from("knot",   0.0), trkgs2vx(trk1, gs), 0.00001);
  EXPECT_NEAR(Units::from("knot", 600.0), trkgs2vx(trk2, gs), 0.00001);
}

TEST_F(VelocityTest, testTrkgs2vy) {
  EXPECT_NEAR(Units::from("knot", -600.0), trkgs2vy(trk1, gs), 0.00001);
  EXPECT_NEAR(Units::from("knot",    0.0), trkgs2vy(trk2, gs), 0.00001);
}

TEST_F(VelocityTest, testTrkgs2v) {
  Vect2 v;
  v = trkgs2v(trk1, gs);
  EXPECT_NEAR(Units::from("knot",    0.0), v.x, 0.00001);
  EXPECT_NEAR(Units::from("knot", -600.0), v.y, 0.00001);
  v = trkgs2v(trk2, gs);
  EXPECT_NEAR(Units::from("knot", 600.0), v.x, 0.00001);
  EXPECT_NEAR(Units::from("knot",   0.0), v.y, 0.00001);
}


TEST_F(VelocityTest, testConstructor) {
  Velocity v;
  EXPECT_NEAR(0.0, v.x, 0.0);
  EXPECT_NEAR(0.0, v.y, 0.0);
  EXPECT_NEAR(0.0, v.z, 0.0);
}

TEST_F(VelocityTest, testMkTrkGsVs) {
	double trk = Units::from("deg",20.1);
	double gs = Units::from("kn",555.5);
	double vs = Units::from("fpm",399.9);
	Velocity v = Velocity::mkTrkGsVs(trk,gs,vs);
	EXPECT_NEAR(trk,v.trk(),0.000001);
	EXPECT_NEAR(gs,v.gs(),0.000001);
	EXPECT_NEAR(vs,v.vs(),0.000001);
	trk = -2E-14;
	v = Velocity::mkTrkGsVs(trk,gs,vs);
	EXPECT_NEAR(0,v.compassAngle(),0.000001);
	EXPECT_NEAR(gs,v.gs(),0.000001);
	EXPECT_NEAR(vs,v.vs(),0.000001);

	EXPECT_TRUE(v.mkVs(55).almostEquals(Velocity::mkTrkGsVs(trk,gs,55)));
	EXPECT_TRUE(v.mkTrk(5).almostEquals(Velocity::mkTrkGsVs(5,gs,vs)));
	EXPECT_TRUE(v.mkGs(55).almostEquals(Velocity::mkTrkGsVs(trk,55,vs)));

	v = Velocity::mkTrkGsVs(trk,0.0,vs);
	EXPECT_NEAR(0.0,v.trk(),0.000001);
	EXPECT_NEAR(0.0,v.y,0.000001);
	EXPECT_NEAR(0.0,v.gs(),0.000001);
	EXPECT_NEAR(gs,v.mkGs(gs).gs(),0.000001);

}

TEST_F(VelocityTest, testInvalidVelocity) {
	Velocity v = Velocity::INVALIDV();
	EXPECT_TRUE(v.isInvalid());
	EXPECT_FALSE(Velocity::ZEROV().isInvalid());
}

TEST_F(VelocityTest, testAccessors) {
		double trk =  20.1; // degrees
		double gs =  555.5; // knots
		double vs =  399.9; // fpm
		Velocity v = Velocity::makeTrkGsVs(trk,gs,vs);
		
		EXPECT_NEAR(trk,v.track("deg"),0.000001);
		EXPECT_NEAR(gs,v.groundSpeed("knot"),0.000001);
		EXPECT_NEAR(vs,v.verticalSpeed("fpm"),0.000001);
		
		EXPECT_NEAR(Units::from("deg",trk),v.trk(),0.000001);
		EXPECT_NEAR(Units::from("knot",gs),v.gs(),0.000001);
		EXPECT_NEAR(Units::from("fpm",vs), v.vs(),0.000001);
	}

TEST_F(VelocityTest, testRotate) {
		double trk =  20.1; // degrees
		double gs =  555.5; // knots
		double vs =  399.9; // fpm
		Velocity v1 = Velocity::makeTrkGsVs(trk,gs,vs);
		
		Velocity v2 = v1.mkAddTrk(Units::from("deg",90.0));
		Velocity v3 = v1.mkAddTrk(Units::from("deg",180.0));
		Velocity v4 = v1.mkAddTrk(Units::from("deg",360.0));
		
		EXPECT_NEAR(v1.gs(), v2.gs(), 0.000001);
		EXPECT_NEAR(v1.gs(), v3.gs(), 0.000001);
		EXPECT_NEAR(v1.gs(), v4.gs(), 0.000001);

		EXPECT_NEAR(v1.vs(), v2.vs(), 0.000001);
		EXPECT_NEAR(v1.vs(), v3.vs(), 0.000001);
		EXPECT_NEAR(v1.vs(), v4.vs(), 0.000001);

		EXPECT_NEAR(trk+90.0, v2.compassAngle("deg"), 0.000001);
		EXPECT_NEAR(trk+180,  v3.compassAngle("deg"), 0.000001);
		EXPECT_NEAR(trk,      v4.compassAngle("deg"), 0.000001);
		
		Velocity v5 = v1.mkAddTrk(Units::from("deg",450.0));
		EXPECT_NEAR(v2.trk(), v5.trk(), 0.000001);
		EXPECT_NEAR(v2.compassAngle(), v5.compassAngle(), 0.000001);
	}


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

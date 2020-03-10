/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.


#include "Vect2.h"
#include "Units.h"
#include "Util.h"
#include <cmath>
#include <gtest/gtest.h>
#include "Velocity.h"
#include "VectFuns.h"

using namespace larcfm;

class VecTest : public ::testing::Test {
};



TEST_F(VecTest, testDistPerp) {
	Vect2 s = Vect2(0,10);
	Vect2 v = Vect2(1,0);
	Vect2 q = Vect2(20,30);
	double dP = Vect2::distPerp(s, v, q);
    EXPECT_NEAR(20.0,dP,0.000001);
    v = Vect2(157,0);
    dP = Vect2::distPerp(s, v, q);
    EXPECT_NEAR(20.0,dP,0.000001);
	s = Vect2(-23,-12);
    dP = Vect2::distPerp(s, v, q);
    EXPECT_NEAR(42.0,dP,0.000001);
	s = Vect2(10,10);
    v = Vect2(4,4);
	q = Vect2(20,10);
    dP = Vect2::distPerp(s, v, q);
    EXPECT_NEAR(10.0/sqrt(2.0),dP,0.000001);
	q = Vect2(20,20);
    dP = Vect2::distPerp(s, v, q);
    EXPECT_NEAR(0.0,dP,0.000001);
}


TEST_F(VecTest, testIntersect_pt) {
	Vect2 s0 = Vect2(0,1);
	Vect2 s1 = Vect2(1,0);
	Vect2 v0 = Vect2(2,0);
	Vect2 v1 = Vect2(0,2);
	Vect2 ip = Vect2::intersect_pt(s0, v0, s1, v1);
	EXPECT_NEAR(1.0,ip.x,0.0001);
	EXPECT_NEAR(1.0,ip.y,0.0001);
	s1 = Vect2(2,0);
	ip = Vect2::intersect_pt(s0, v0, s1, v1);
	EXPECT_NEAR(2.0,ip.x,0.0001);
}

TEST_F(VecTest,testZeroSmallVs) {
	double trk = Units::from("deg",20.1);
	double gs = Units::from("kn",555.5);
	double vs = Units::from("fpm",399.9);
	Velocity v = Velocity::mkTrkGsVs(trk,gs,vs);
	Velocity nv = v.zeroSmallVs(Units::from("fpm",10.0));
	EXPECT_NEAR(v.x,nv.x,0.00000001);
	EXPECT_NEAR(v.y,nv.y,0.00000001);
	EXPECT_NEAR(v.z,nv.z,0.00000001);
	v = Velocity::mkTrkGsVs(trk,gs,Units::from("fpm",9.9));
	nv = v.zeroSmallVs(Units::from("fpm",10.0));
	EXPECT_NEAR(v.x,nv.x,0.00000001);
	EXPECT_NEAR(v.y,nv.y,0.00000001);
	EXPECT_NEAR(0.0,nv.z,0.00000001);
}



TEST_F(VecTest,testRightOfLine) {
	Vect2 so = Vect2(0,0);
	Vect2 vo = Vect2(0,1);
	Vect2 si = Vect2(1,0.2);
	bool rom = VectFuns::rightOfLine(so,vo,si);
	EXPECT_EQ(true,rom);
	si = Vect2(1,-7);
	rom = VectFuns::rightOfLine(so,vo,si);
	EXPECT_EQ(true,rom);
	si = Vect2(-1,0.2);
	rom = VectFuns::rightOfLine(so,vo,si);
	EXPECT_EQ(false,rom);
	si = Vect2(-1,-0.9);
	rom = VectFuns::rightOfLine(so,vo,si);
	EXPECT_EQ(false,rom);
}

TEST_F(VecTest, testDistMore) {
	double hdg = Units::from("deg",90.0);
 	Vect2 dir = Velocity::mkTrkGsVs(hdg,Units::from("kn",160),0.0).vect2();
	Vect2 so = Vect2(0.0, Units::from("nm",0.26));
	Vect2 q = Vect2(Units::from("nm",0.1250), Units::from("nm",0.10));
    double dP = Vect2::distPerp(so, dir, q);
    //fpln(" dP = "+Units::str("ft",dP));
    EXPECT_NEAR(Units::from("nm",0.16),dP,0.001);
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

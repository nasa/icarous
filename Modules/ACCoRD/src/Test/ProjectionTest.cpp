/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

//#include "UnitSymbols.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "format.h"

#include "ENUProjection.h"
#include "OrthographicProjection.h"
#include "VectFuns.h"
#include <stdlib.h>
#include <time.h>

#include <gtest/gtest.h>

using namespace larcfm;

class ProjectionTest : public ::testing::Test {


public:

protected:
  virtual void SetUp() {
  }
};


TEST_F(ProjectionTest, testXyz) {
    LatLonAlt so = LatLonAlt::make(37, -122,  10000);
	EuclideanProjection sp = Projection::createProjection(so.lat(),so.lon(),0.0);
    LatLonAlt s1 = LatLonAlt::make(37.4701, -122.03,  10000);
    //double projDist = GreatCircle::distance(so, s1);
    //fpln(" projDist = "+Units::str("NM",projDist));
    double step = Units::from("deg",5);
	double dStep = Units::from("ft",2000);
	//double track = step; {
	//double dist = dStep; {

	for (double track = 0; track < 2*M_PI; track = track + step) {
		for (double dist = dStep; dist <= 20*dStep; dist = dist + dStep) {
			LatLonAlt s2 = GreatCircle::linear_initial(s1, track, dist);
			double speed = Units::from("kn",480);
			Velocity v = GreatCircle::velocity_average_speed(s1, s2, speed);
   			//fpln(" v = "+v.toString());
 			//EXPECT_NEAR(Util::to_2pi(track),Util::to_2pi(v.trk()),0.01);
			EXPECT_NEAR(0,Util::turnDelta(track,v.trk()),0.001);
			EXPECT_NEAR(speed,v.gs(),0.01);
			EXPECT_NEAR(0.0,v.vs(),0.01);
			//fpln(" v = "+v.toString6NP());
			double rtrack = GreatCircle::representative_course(s1.lat(),s1.lon(),s2.lat(),s2.lon());
			//EXPECT_NEAR(rtrack,track,0.0001);
			EXPECT_NEAR(0,Util::turnDelta(rtrack,track),0.001);
			Vect3 pso = sp.project(s1);
			Vect3 ps2 = sp.project(s2);
			Velocity pv = Velocity::mkVel(pso,ps2,speed);
			//fpln(" pv = "+pv.toString6NP());
			// compare velocities
			EXPECT_NEAR(0,Util::turnDelta(v.trk(),pv.trk()),0.002);
			EXPECT_NEAR(v.gs(),pv.gs(),0.0001);
			EXPECT_NEAR(v.vs(),pv.vs(),0.0001);
			// compare distance
			double actDist = GreatCircle::distance(s1, s2);
			double distH = pso.distanceH(ps2);
			double delta = actDist - distH;
			//fpln(" actDist = "+Units::str("ft",actDist)+" delta = "+Units::str("ft",delta));
			EXPECT_NEAR(0,delta,Units::from("ft",1.6));
		}
	}
}

//
//double nextDouble() {
//	return rand()*1.0/RAND_MAX;
//}
//
//TEST_F(ProjectionTest, test3) {
//	double worstd1 = 0.0;
//	double worstlatd1 = 0.0;
//	double worstang1 = 0.0;
//	double worstlatang1 = 0.0;
//
//	srand (time(NULL));
//
//	for (double lat = -90; lat <= 90; lat += 0.01) {
//		LatLonAlt lla = LatLonAlt::make(lat, 0, 0);
//		OrthographicProjection proj1 = OrthographicProjection(lla);
//
//		ENUProjection proj2 = ENUProjection(lla);
//		for (int i = 0; i < 1000; i++) {
//			LatLonAlt lla1 = LatLonAlt::make(lat+nextDouble(), nextDouble(), 0);
//			LatLonAlt lla2 = LatLonAlt::make(lat+nextDouble(), nextDouble(), 0);
//			LatLonAlt lla3 = LatLonAlt::make(lat-nextDouble(), nextDouble(), 0);
//
//			Velocity v1 = GreatCircle::velocity_initial(lla1, lla2, 100);
//			Velocity v2 = GreatCircle::velocity_initial(lla1, lla3, 100);
//
//			v1 = v1.mkGs(30, "kts");
//			v2 = v2.mkGs(30, "kts");
//
//			LatLonAlt lla4 = GreatCircle::linear_initial(lla1, v1, 3600);
//
//			LatLonAlt lla5 = GreatCircle::linear_initial(lla1, v2, 3600);
//
//			Vect3 v3a1 = proj1.project(lla1);
//			Vect3 v3b1 = proj1.project(lla4);
//			Vect3 v3c1 = proj1.project(lla5);
//
//			Vect3 v3a2 = proj2.project(lla1);
//			Vect3 v3b2 = proj2.project(lla4);
//			Vect3 v3c2 = proj2.project(lla5);
//
//			LatLonAlt llab1 = proj1.inverse(v3b1);
//			LatLonAlt llab2 = proj2.inverse(v3b2);
//
//			double ang1 = VectFuns::angleBetween(v3b1.vect2(), v3a1.vect2(), v3c1.vect2());
//			double ang2 = VectFuns::angleBetween(v3b2.vect2(), v3a2.vect2(), v3c2.vect2());
//
//			double d1 = GreatCircle::distance(llab1, llab2);
//			if (d1 > worstd1) {
//				worstd1 = d1;
//				worstlatd1 = lat;
//			}
//
//			double d2 = std::abs(ang1-ang2);
//			if (d2 > worstang1) {
//				worstang1 = d2;
//				worstlatang1 = lat;
//			}
//		}
//	}
//
//	std::cout << "worst dist m:" << worstd1 << " lat:" << worstlatd1 << std::endl;
//	std::cout << "worst angle deg:" << Units::to("deg", worstang1) << " lat:" << worstlatang1 << std::endl;
//}

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

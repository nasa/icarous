/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


// Uses the Google unit test framework.

#include "Vect2.h"
#include "Vect3.h"
#include "Units.h"
#include "Velocity.h"
#include "Position.h"
#include "GreatCircle.h"
#include "format.h"
#include <gtest/gtest.h>

#include <cmath>

using namespace larcfm;
using namespace std;

class PositionTest : public ::testing::Test {

public:

protected:	
  virtual void SetUp() {
  }
};


TEST_F(PositionTest, testPosition) {
	Vect3 v3_0 = Vect3(0,0,0);
	Vect3  v3_100 = Vect3(100,100,100);
	Position p1 = Position(v3_0);
	Position p2 = Position(v3_100);

	Velocity vel1 = p1.initialVelocity(p2, 100);

	EXPECT_NEAR(Units::from("deg",  45.0), vel1.trk(), 0.0001);
	EXPECT_NEAR(Units::from("knot",   2.749), vel1.gs(), 0.0001);
	EXPECT_NEAR(Units::from("fpm",   196.8504), vel1.z, 0.0001);

	LatLonAlt lla0 = LatLonAlt::make(0,0,0);
	LatLonAlt lla50 = LatLonAlt::make(50, 50, 50);
	LatLonAlt lla5 = LatLonAlt::make(5, 5, 5);
	Position p3 = Position(lla0);
	Position p4 = Position(lla50);
	Position p4a = Position(lla5);

	Velocity vel2 = p3.initialVelocity(p4,100);
	Velocity vel3 = p3.initialVelocity(p4a,100);


	Position p5 = p2;
	EXPECT_TRUE(p2 == p5);

	p5 = Position::makeLatLonAlt(50, 50, 50);
	EXPECT_TRUE(p4 == p5);

	p5 = Position::mkXYZ(100, 100, 100);
	EXPECT_TRUE(p2 == p5);

	p5 = Position::mkLatLonAlt(p4.lat()+Constants::get_latlon_accuracy(), p4.lon()+Constants::get_latlon_accuracy(), p4.alt());
	EXPECT_TRUE(p4.almostEquals(p4));
	EXPECT_FALSE(p4.almostEquals(p5));

	Vect2 v2 = p2.vect2();
	EXPECT_NEAR(v2.x,100,0.0001);
	EXPECT_NEAR(v2.y,100,0.0001);

	Point v3 = p2.vect3();
	EXPECT_NEAR(v3.x,100,0.0001);
	EXPECT_NEAR(v3.y,100,0.0001);
	EXPECT_NEAR(v3.z,100,0.0001);

	LatLonAlt lla = p4.lla();
	EXPECT_NEAR(lla.lat(),lla50.lat(),0.0001);
	EXPECT_NEAR(lla.lon(),lla50.lon(),0.0001);
	EXPECT_NEAR(lla.alt(),lla50.alt(),0.0001);

	EXPECT_FALSE(p2.isLatLon());
	EXPECT_TRUE(p4.isLatLon());

	EXPECT_TRUE(Position::INVALID().isInvalid());
	EXPECT_FALSE(p4.isInvalid());

	p5 = p2.mkX(500);
	EXPECT_NEAR(p5.x(),500,0.0001);
	EXPECT_NEAR(p5.y(),100,0.0001);
	EXPECT_NEAR(p5.z(),100,0.0001);

	p5 = p2.mkY(500);
	EXPECT_NEAR(p5.x(),100,0.0001);
	EXPECT_NEAR(p5.y(),500,0.0001);
	EXPECT_NEAR(p5.z(),100,0.0001);

	p5 = p2.mkZ(500);
	EXPECT_NEAR(p5.x(),100,0.0001);
	EXPECT_NEAR(p5.y(),100,0.0001);
	EXPECT_NEAR(p5.z(),500,0.0001);

	p5 = p3.mkLat(3);
	EXPECT_NEAR(p5.lat(),3,0.0001);
	EXPECT_NEAR(p5.lon(),0,0.0001);
	EXPECT_NEAR(p5.alt(),0,0.0001);

	p5 = p3.mkLon(3);
	EXPECT_NEAR(p5.lat(),0,0.0001);
	EXPECT_NEAR(p5.lon(),3,0.0001);
	EXPECT_NEAR(p5.alt(),0,0.0001);

	p5 = p3.mkAlt(3);
	EXPECT_NEAR(p5.lat(),0,0.0001);
	EXPECT_NEAR(p5.lon(),0,0.0001);
	EXPECT_NEAR(p5.alt(),3,0.0001);

	p5 = p5.zeroAlt();
	EXPECT_TRUE(p5 == p3);

	EXPECT_NEAR(p1.distanceH(p2), 141.42135, 0.0001);
	EXPECT_NEAR(p1.distanceV(p2), 100, 0.0001);
	EXPECT_NEAR(p3.distanceH(p4), Units::from("nmi", 3935.7300215983), 0.5);
	EXPECT_NEAR(p3.distanceV(p4), Units::from("ft",50), 0.0001);

	p5 = p1.linear(vel1,100);
	EXPECT_NEAR(p2.x(),p5.x(),0.0001);
	EXPECT_NEAR(p2.y(),p5.y(),0.0001);
	EXPECT_NEAR(p2.z(),p5.z(),0.0001);

	p5 = p3.linear(vel2,100);
	EXPECT_NEAR(p4.x(),p5.x(),0.0001);
	EXPECT_NEAR(p4.y(),p5.y(),0.0001);
	EXPECT_NEAR(p4.z(),p5.z(),0.0001);

	//TODO linearEst(dn,de)

	p5 = p1.linearEst(vel1,100);
	EXPECT_NEAR(p2.x(),p5.x(),0.0001);
	EXPECT_NEAR(p2.y(),p5.y(),0.0001);
	EXPECT_NEAR(p2.z(),p5.z(),0.0001);

	p5 = p3.linearEst(vel3,100);
	EXPECT_NEAR(p4a.x(),p5.x(),0.001);
	EXPECT_NEAR(p4a.y(),p5.y(),0.001);
	EXPECT_NEAR(p4a.z(),p5.z(),0.001);

	//TODO linearEstPerp()

	p5 = p1.midPoint(p2);
	EXPECT_NEAR(p5.x(),50,0.0001);
	EXPECT_NEAR(p5.y(),50,0.0001);
	EXPECT_NEAR(p5.z(),50,0.0001);


	p5 = p3.midPoint(p4);
	lla = GreatCircle::interpolate(lla0, lla50, 0.50);
	EXPECT_NEAR(p5.lat(),lla.lat(),0.0001);
	EXPECT_NEAR(p5.lon(),lla.lon(),0.0001);
	EXPECT_NEAR(p5.alt(),Units::from("ft",25),0.0001);

	EXPECT_NEAR(p1.track(p2), Units::from("deg", 45), 0.0001);
	EXPECT_NEAR(p3.track(p4), Units::from("deg", 32.7324), 0.0001);

	// midpoint track for entire segment
	EXPECT_NEAR(p1.representativeTrack(p2), Units::from("deg", 45), 0.0001);
	EXPECT_NEAR(p3.representativeTrack(p4), Units::from("deg", 37.4047), 0.0001);
	EXPECT_NEAR(p3.representativeTrack(p4), p3.midPoint(p4).track(p4), 0.0001);

//	// just in and out of protection zone
//	EXPECT_TRUE(p1.LoS(p2, 142, 200));
//	EXPECT_FALSE(p1.LoS(p2, 141, 200));

//	// collinear
//	EXPECT_TRUE(p1.collinear(p1.linear(vel1, 100), p1.linear(vel1, -100)));
//	EXPECT_TRUE(p3.collinear(p3.linear(vel2, 100), p3.linear(vel2, -100)));

//	// not collinear
//	EXPECT_FALSE(p1.collinear(p1.linear(vel1, 100), p1.linear(vel2, -100)));
//	EXPECT_FALSE(p3.collinear(p3.linear(vel2, 100), p3.linear(vel1, -100)));

}


TEST_F(PositionTest, testParsing) {
	Point  v3_100 = Point::make(100,100,100);
	LatLonAlt lla50 = LatLonAlt::make(-10.0, 90.0, 3600);
	Position p2(v3_100);
	Position p4(lla50);
	Position p5;

	// make sure we can parse the output
	p5 = Position::parseXYZ(p2.toString(8));
	EXPECT_NEAR(p2.x(),p5.x(),0.000000001);
	EXPECT_NEAR(p2.y(),p5.y(),0.000000001);
	EXPECT_NEAR(p2.z(),p5.z(),0.000000001);
	p5 = Position::parseLL(p4.toString(8));
	EXPECT_TRUE(p5.almostEquals(p4));
	p5 = Position::parse(p2.toString(8));
	EXPECT_TRUE(p5.almostEquals(p2));
	p5 = Position::parse(p4.toString(8));
	EXPECT_TRUE(p5.almostEquals(p4));

	// not proper format:
	p5 = Position::parseXYZ("5, 5, 5");
	EXPECT_NEAR(9260.0,p5.x(),0.00000001);
	EXPECT_NEAR(9260.0,p5.y(),0.00000001);
	EXPECT_NEAR(1.524,p5.z(),0.00000001);
	EXPECT_TRUE(Position::parse("5 [deg], 5 [NM], 5 [fps]").isInvalid()); // inconsistent units
	EXPECT_TRUE(Position::parse("5, 5, 5").isInvalid()); // missing units

	// check different toString's for compatibility
	Position p6 = Position::parseLL(p4.toStringNP());
	Position p7 = Position::parseLL(p4.toString());
	EXPECT_TRUE(p4.toStringNP(8) == (p6.toStringNP(8)));
	EXPECT_TRUE(p4.toStringNP(8) == (p7.toStringNP(8)));
}


TEST_F(PositionTest, test_linearDist) {
	double track = Units::from("deg",90);
	double gsAt_d = Units::from("kn",400.0);
	Velocity v = Velocity::makeTrkGsVs(90,400,100);
	for (double d = 0.0; d <= Units::from("NM",10.0);  d = d + Units::from("NM",1.0)) {
		//double altAtd = Units::from("ft",23456);
		for (double lat = 0.00; lat < 88; lat = lat + 0.1) {
			Position so = Position::makeLatLonAlt(lat, 15.08, 5000.0);
			std::pair<Position,Velocity> lPair = so.linearDist2D(track, d, gsAt_d);
			//f.pln(" lat = "+lat+" lPair = "+lPair);
			Position sNew = lPair.first;
			double distSep = so.distanceH(sNew);
			EXPECT_NEAR(d,distSep,0.000001);
			EXPECT_NEAR(lat,sNew.latitude(),0.01);
			//EXPECT_NEAR(altAtd,sNew.alt(),0.00000001);
			Velocity vNew = lPair.second;
			//fpln(" $$ test_linearDist: vNew = "+vNew);
			EXPECT_NEAR(gsAt_d,vNew.gs(),0.0000001);
			EXPECT_NEAR(0.0,vNew.vs(),0.0000001);
			EXPECT_TRUE(vNew.trk()+1E-15 >= track);
			EXPECT_TRUE(vNew.trk() <  track +Units::from("deg",5.0) );

		}
	}
}




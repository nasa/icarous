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
#include "GreatCircle.h"
#include <gtest/gtest.h>
#include "Plan.h"
#include "format.h"

#include <cmath>

using namespace larcfm;

class NavPointTest : public ::testing::Test {

public:

protected:	
  virtual void SetUp() {
  }
};


TEST_F(NavPointTest, testXyz) {
  Velocity v;
  NavPoint s1 = NavPoint::ZERO_LL();
  NavPoint s2 = NavPoint(Position(LatLonAlt::make(-10, 0.0, 3600)), 3600.0); 
  v = s1.initialVelocity(s2);
  EXPECT_NEAR(Units::from("knot",    0.0), v.x, 0.1);
  EXPECT_NEAR(Units::from("knot", -600.0), v.y, 0.1);
  EXPECT_NEAR(Units::from("fpm",    60.0), v.z, 0.1);
  s1 = NavPoint::ZERO_XYZ();
  s2 = NavPoint(Position(Vect3(Units::from("nmi", 10), Units::from("nmi", 10), Units::from("ft", 3600))), 3600.0);
  v = s1.initialVelocity(s2);
  EXPECT_NEAR(Units::from("knot",   10.0), v.x, 0.1);
  EXPECT_NEAR(Units::from("knot",   10.0), v.y, 0.1);
  EXPECT_NEAR(Units::from("fpm",    60.0), v.z, 0.1);

  Velocity v2 = s2.initialVelocity(s1); // Operation is commutative: should be the same as v
  EXPECT_NEAR(Units::from("knot",   10.0), v2.x, 0.1);
  EXPECT_NEAR(Units::from("knot",   10.0), v2.y, 0.1);
  EXPECT_NEAR(Units::from("fpm",    60.0), v2.z, 0.1);


  s1 = NavPoint(Position(LatLonAlt::make(
		  -62.61840000000001,
		  -177.48460000000003,
		  20750.2199)),
		  842.1083);
  s2 = NavPoint(Position(LatLonAlt::make(
		  -62.635796013430024,
		  -177.50640811422426,
		  20998.469779546806)),
		  857.7597);
  v = s1.initialVelocity(s2);
  EXPECT_NEAR(Units::from("knot",  -139.0), v.x, 0.5);
  EXPECT_NEAR(Units::from("knot",  -240.0), v.y, 0.1);
  EXPECT_NEAR(Units::from("fpm",    950.0), v.z, 0.1);

  v2 = s2.initialVelocity(s1); 

  // initialVelocity is commutative v = v2
  EXPECT_NEAR(v.x, v2.x, 0.001);
  EXPECT_NEAR(v.y, v2.y, 0.001);
  EXPECT_NEAR(v.z, v2.z, 0.001);
  // s3 is co-linear (in a great circle sense) with s1 and s2.
  NavPoint s3 = s1.linear(v, (s2.time()-s1.time())*2);
  // s1 and s3 equidistant from s2
  EXPECT_NEAR(s1.distanceH(s2), s2.distanceH(s3), 0.001);
  Velocity v3 = s2.initialVelocity(s3);
//  // v3 is the average velocity between s1 and s3
//  Velocity v4 = s1.averageVelocity(s3);
//  EXPECT_NEAR(v4.x, v3.x, 0.001);
//  EXPECT_NEAR(v4.y, v3.y, 0.001);
//  EXPECT_NEAR(v4.z, v3.z, 0.001);

  EXPECT_TRUE(NavPoint::INVALID().isInvalid());

}

TEST_F(NavPointTest, testTrkgsvs) {
  Velocity v;
  NavPoint s1 = NavPoint::ZERO_LL();
  NavPoint s2 = NavPoint(Position(LatLonAlt::make(-10.0, 0.0, 3600)), 3600.0);
  v = s1.initialVelocity(s2);
  EXPECT_NEAR(Units::from("deg",   180.0), v.compassAngle(), 0.1);
  EXPECT_NEAR(Units::from("knot",  600.0), v.gs(), 0.1);
  EXPECT_NEAR(Units::from("fpm",    60.0), v.z, 0.1);
  s1 = NavPoint::ZERO_XYZ();
  s2 = NavPoint(Position(Vect3(Units::from("nmi", 10), Units::from("nmi", 10), Units::from("ft", 3600))), 3600.0);
   v = s1.initialVelocity(s2);
  EXPECT_NEAR(Units::from("deg",   45.0), v.compassAngle(), 0.1);
  EXPECT_NEAR(Units::from("knot",  10.0*sqrt(2.0)), v.gs(), 0.1);
  EXPECT_NEAR(Units::from("fpm",   60.0), v.z, 0.1);
}



//TEST_F(NavPointTest, testParsing) {
//	NavPoint p1 = NavPoint(Position::makeLatLonAlt(-10.0, 90.0, 3600), 5000);
//	NavPoint p2 = NavPoint::parseLL(p1.toOutput(8,true));
//	NavPoint p3 = NavPoint::parseLL(p1.toString());
//	EXPECT_TRUE(p1.toOutput(8,true) == p2.toOutput(8,true));
//	EXPECT_TRUE(p1.toOutput(8,true) == p3.toOutput(8,true));
//
//	p1 = NavPoint(Position::makeXYZ(-10.0, 90.0, 3600), 5000);
//	p2 = NavPoint::parseXYZ(p1.toOutput(8,false));
//	p3 = NavPoint::parseXYZ(p1.toString());
//	EXPECT_TRUE(p1.toOutput(8,false) == p2.toOutput(8,false));
//	EXPECT_TRUE(p1.toOutput(8,false) == p3.toOutput(8,false));
//}



TEST_F(NavPointTest, testFastLinear) {
	LatLonAlt lla0 = LatLonAlt::make(33.0, -95.0, 0.0000);
	double dn = Units::from("nm",1.4);
	double de = Units::from("nm",0.0);
	LatLonAlt nlla = lla0.linearEst(dn,de);
	EXPECT_NEAR(GreatCircle::distance(lla0,nlla),dn,10.0);
	dn = Units::from("nm",0.0);
	de = Units::from("nm",0.2);
	nlla = lla0.linearEst(dn,de);
	EXPECT_NEAR(GreatCircle::distance(lla0,nlla),de,1.0);
	Velocity v = Velocity::makeTrkGsVs(330,360,0.0);
	Position p0  = Position(LatLonAlt::make(32.0, -96.0, 0.0000));
	NavPoint np0 = NavPoint(p0,0.0);
	NavPoint np1 = np0.linearEst(v,10.0);
	EXPECT_NEAR(0.5587568,np1.lat(),0.000001);
	EXPECT_NEAR(-1.675687,np1.lon(),0.000001);
	EXPECT_NEAR(0.0,np1.alt(),0.000001);
	v = Velocity::makeTrkGsVs(25,200,100.0);
	p0  = Position(LatLonAlt::make(32.0, -96.0, 0.0000));
	np0 = NavPoint(p0,0.0);
	NavPoint np2 = np0.linearEst(v,10.0);
	//lpc.add(np0);
	//lpc.add(np2);
	//f.pln(" lpc = "+lpc);
    //f.pln(" np2.lat() = "+np2.lat());
	EXPECT_NEAR(0.558651,np2.lat(),0.000001);
	EXPECT_NEAR(-1.675435,np2.lon(),0.000001);
	EXPECT_NEAR(5.08,np2.alt(),0.001);
}

TEST_F(NavPointTest, testLinearGs) {
	double  si0x = -5.0;
	double  si0y = 33.33333333333333;
	double trki0 = 180.0;
	double trki1 = 180.0;
	double gsi = 350.0;
    double leg1Time = 2.479895384194002;
    double leg2Time = 514.2857085022476;
    Velocity vi0 = Velocity::makeTrkGsVs(trki0, gsi, 0);  // Intruder initial velocity
    Vect3 si0 = Vect3::makeXYZ(si0x,"nm",si0y,"nm",0,"ft");         // Intruder initial position
    Vect3 si1 = si0.linear(vi0,leg1Time);
    Plan lpc("");
    NavPoint si_1(Position(si1),leg1Time);
    lpc.addNavPoint(si_1);
    Velocity vi1 = Velocity::makeTrkGsVs(trki1, gsi, 0);  // Intruder initial velocity
    Vect3 si2 = si1.linear(vi1,leg2Time);
    NavPoint si_2(Position(si2),leg1Time+leg2Time);
    lpc.addNavPoint(si_2);
    //fpln(" lpc.initialVelocity(0).gs() = "+Units::str8("kn",lpc.initialVelocity(0).gs()));
    //fpln(" lpc.initialVelocity(1).gs() = "+Units::str8("kn",lpc.initialVelocity(1).gs()));
    //fpln(" lpc.initialVelocity(2).gs() = "+Units::str("kn",lpc.initialVelocity(2).gs()));
}



TEST_F(NavPointTest, testPosition) {
	Position p1 = Position::makeLatLonAlt(1.8578,-8.1082999,10000.00);
	EXPECT_TRUE(p1.isLatLon());
	EXPECT_NEAR(Units::from("deg",1.8578),p1.lat(),0.0001);
	EXPECT_NEAR(Units::from("deg",-8.1082999),p1.lon(),0.0001);
	EXPECT_NEAR(Units::from("ft",10000),p1.alt(),0.0001);
	EXPECT_NEAR(1.8578,p1.latitude(),0.0001);
	EXPECT_NEAR(-8.1082999,p1.longitude(),0.0001);
	EXPECT_NEAR(10000,p1.altitude(),0.000001);
	EXPECT_NEAR(0,p1.mkX(0.0).lon(),0.000001);
	EXPECT_NEAR(0,p1.mkY(0.0).lat(),0.000001);
	EXPECT_NEAR(0,p1.mkZ(0.0).alt(),0.000001);
	EXPECT_NEAR(-3.5,p1.mkLon(-3.5).lon(),0.000001);
	EXPECT_NEAR(2.76,p1.mkLat(2.76).lat(),0.000001);
	EXPECT_NEAR(23000,p1.mkAlt(23000).alt(),0.000001);
	Position p2 = p1.linearEst(100.0, 0);
	EXPECT_NEAR(p1.distanceH(p2),p2.distanceH(p1),0.000001);
	double distH = p1.distanceH(p2);
	EXPECT_NEAR(100.0,distH,0.1);
	double distV = p1.distanceV(p2);
	EXPECT_NEAR(0.0,distV,0.1);
	p2 = p1.linearEst(0, 100.0);
	distH = p1.distanceH(p2);
	EXPECT_NEAR(100.0,distH,0.1);
	distV = p1.distanceV(p2);
	EXPECT_NEAR(0.0,distV,0.1);
	p2 = p1.linearEst(200.0, 200.0);
	distH = p1.distanceH(p2);
	EXPECT_NEAR(282.8,distH,0.1);
	double trk = p1.representativeTrack(p2);
	EXPECT_NEAR(0.785,trk,0.001);
//	Velocity  vv = p1.averageVelocity(p2,Units::from("kn",400));
//	EXPECT_NEAR(0.785,vv.trk(),0.001);
//	EXPECT_NEAR(205.777,vv.gs(),0.001);
//	EXPECT_NEAR(0.0,vv.vs(),0.001);
	Position p3 = Position::makeXYZ(100.0,-50.0,10000.00);
	EXPECT_FALSE(p3.isLatLon());
	Position p4 = p3.linearEst(100.0, 0);
	distH = p3.distanceH(p4);
	EXPECT_NEAR(100.0,distH,0.1);
	p4 = p3.linearEst(100.0,100.0);
	distH = p3.distanceH(p4);
	EXPECT_NEAR(std::sqrt(10000.0+10000.0),distH,0.1);
	distV = p3.distanceV(p4);
	EXPECT_NEAR(0.0,distV,0.1);
}




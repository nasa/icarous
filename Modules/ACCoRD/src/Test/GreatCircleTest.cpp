/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect2.h"
//#include "UnitSymbols.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "SimpleProjection.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class GreatCircleTest : public ::testing::Test {

public:
double lax_lat_d;
double lax_lat_m;
double lax_lat_s;
double lax_lon_d;
double lax_lon_m;
double lax_lon_s;

double jfk_lat_d;
double jfk_lat_m;
double jfk_lat_s;
double jfk_lon_d;
double jfk_lon_m;
double jfk_lon_s;

double lax_lat; 
double lax_lon; 

double jfk_lat;
double jfk_lon;

double sng_lat;
double sng_lon;

double nrt_lat;
double nrt_lon;

  LatLonAlt lax;
  LatLonAlt jfk;
  LatLonAlt sng;
  LatLonAlt nrt;

protected:
  virtual void SetUp() {
    lax_lat_d =  33.0;
    lax_lat_m =  57.0;
    lax_lat_s =   0.0;
    lax_lon_d = 118.0;
    lax_lon_m =  24.0;
    lax_lon_s =   0.0;
    
    jfk_lat_d =  40.0;
    jfk_lat_m =  38.0;
    jfk_lat_s =   0.0;
    jfk_lon_d =  73.0;
    jfk_lon_m =  47.0;
    jfk_lon_s =   0.0;
    
    lax_lat = LatLonAlt::decimal_angle(lax_lat_d, lax_lat_m, lax_lat_s, true);    // Los Angeles airport, California
    lax_lon = LatLonAlt::decimal_angle(lax_lon_d, lax_lon_m, lax_lon_s, false);
    
    jfk_lat = LatLonAlt::decimal_angle(jfk_lat_d, jfk_lat_m, jfk_lat_s, true);    // JFK airport, New York
    jfk_lon = LatLonAlt::decimal_angle(jfk_lon_d, jfk_lon_m, jfk_lon_s, false);
    
    sng_lat = LatLonAlt::decimal_angle(  1.0, 21.0,  0.0, true);    // Singapore
    sng_lon = LatLonAlt::decimal_angle(103.0, 59.0, 40.0, true);
    
    nrt_lat = LatLonAlt::decimal_angle( 35.0, 45.0, 53.0, true);    // Narita Airport, Tokyo, Japan
    nrt_lon = LatLonAlt::decimal_angle(140.0, 23.0, 11.0, true);		

    lax = LatLonAlt::mk(lax_lat, lax_lon, 0.0);
    jfk = LatLonAlt::mk(jfk_lat, jfk_lon, 10.0);
    sng = LatLonAlt::mk(sng_lat, sng_lon, 0.0);
    nrt = LatLonAlt::mk(nrt_lat, nrt_lon, 0.0);	
  }
};
	


	

	TEST_F(GreatCircleTest, testWithin) {
	  EXPECT_TRUE( GreatCircle::almostEquals(lax,jfk,Units::from("nmi",2144.0),11.0));
		EXPECT_FALSE(GreatCircle::almostEquals(lax,jfk,Units::from("nmi",2143.0),11.0));
		EXPECT_FALSE(GreatCircle::almostEquals(lax,jfk,Units::from("nmi",2144.0),10.0));
		EXPECT_FALSE(GreatCircle::almostEquals(lax,jfk,Units::from("nmi",2143.0),10.0));
		
		EXPECT_TRUE(GreatCircle::almostEquals(lax,lax,Units::from("m",0.1),0.1)); // note: the same points have zero distance, so the d and h must be positive
		
		LatLonAlt p1 = LatLonAlt::make(-62.6184, -177.4846, 20750.0000);
		LatLonAlt p2 = LatLonAlt::make(-62.6000, -177.4000, 21000.0000);
		EXPECT_TRUE( GreatCircle::almostEquals(p1,p2,Units::from("nmi",3.0),Units::from("ft", 300.0)));
		EXPECT_FALSE(GreatCircle::almostEquals(p1,p2,Units::from("nmi",2.0),Units::from("ft", 300.0)));
		EXPECT_FALSE(GreatCircle::almostEquals(p1,p2,Units::from("nmi",3.0),Units::from("ft", 200.0)));
		EXPECT_FALSE(GreatCircle::almostEquals(p1,p2,Units::from("nmi",2.0),Units::from("ft", 200.0)));
	}



TEST_F(GreatCircleTest,testAngleFromDistance) {
  double a;
  a = GreatCircle::angle_from_distance(Units::from("m", 6366000.0),0.0);
  EXPECT_NEAR(1.0, a, 0.001);
  a = GreatCircle::angle_from_distance(Units::from("m", 1.0),0.0);
  EXPECT_NEAR(1.57e-7, a, 1.0e-10); // one meter in radians is very small
  a = GreatCircle::angle_from_distance(Units::from("NM", 1.0), 0.0);
  EXPECT_NEAR(2.9088e-4, a, 1.0e-8); // one nautical mile in radians is small
  EXPECT_NEAR(0.016667, Units::to("deg",a), 0.000001); // 1/60 degree = 1 NM; that is, a NM is an "arc minute"

  double a1 = GreatCircle::angle_from_distance(Units::from("m", 1000.0), 0.0);
  double a2 = GreatCircle::angle_from_distance(Units::from("m", 1000.0), GreatCircle::spherical_earth_radius);
  EXPECT_NEAR(a1, 2.0*a2, 0.0); // test with a non-zero altitude


}

	
TEST_F(GreatCircleTest,testDistanceFromAngle) {
  double d;
  d = GreatCircle::distance_from_angle(1.0,0.0);
  EXPECT_NEAR(Units::from("m", 6366000.0), d, 1000.0);
  EXPECT_NEAR(Units::from("m", 6366707.0194937070000000000), d, 0.0);
  d = GreatCircle::distance_from_angle(Units::from("deg", 1.0),0.0);
  EXPECT_NEAR(Units::from("m", 111120.0), d, 1.0); // one degree is 111,120 meters
  d = GreatCircle::distance_from_angle(Units::from("deg", 1.0/60.0),0.0);
  EXPECT_NEAR(Units::from("m", 1852.0), d, 1.0); // one arc-minute of angle ( ~ 0.016667) is one nautical mile
}

	
TEST_F(GreatCircleTest,testAngularDistance) {
  double a1 = GreatCircle::angular_distance(0.0, 0.0, 0.0, Pi);
  EXPECT_NEAR(Pi, a1, 0.00000001);
		
  double a2 = GreatCircle::angular_distance(lax_lat, lax_lon, jfk_lat, jfk_lon);
  EXPECT_NEAR(0.623585, a2, 0.00001);
}

	
TEST_F(GreatCircleTest,testDistance) {
  double dist1 = GreatCircle::distance(lax_lat, lax_lon, jfk_lat, jfk_lon);
  double dist2 = GreatCircle::distance(jfk_lat, jfk_lon, lax_lat, lax_lon);

  EXPECT_NEAR(2144.0, Units::to("nmi", dist1), 1.0);
  EXPECT_NEAR(2144.0, Units::to("nmi", dist2), 1.0);
		
  double dist3 = GreatCircle::distance(lax_lat, lax_lon, nrt_lat, nrt_lon);
  double dist4 = GreatCircle::distance(nrt_lat, nrt_lon, lax_lat, lax_lon);

  EXPECT_NEAR(4723.0, Units::to("nmi", dist3), 1.0);  // 4737 with elipsoidal Earth
  EXPECT_NEAR(4723.0, Units::to("nmi", dist4), 1.0);
		
  double same = GreatCircle::distance(lax_lat, lax_lon, lax_lat, lax_lon);
  EXPECT_NEAR(0.0, same, 0.00000001);
}

	
TEST_F(GreatCircleTest,testInitialCourse) {
  double itrk1 = GreatCircle::initial_course(lax_lat, lax_lon, jfk_lat, jfk_lon);
  double itrk2 = GreatCircle::initial_course(jfk_lat, jfk_lon, lax_lat, lax_lon);
  LatLonAlt midpoint = GreatCircle::interpolate(lax, jfk, 0.5);
  double itrk3 = GreatCircle::initial_course(midpoint.lat(), midpoint.lon(), jfk_lat, jfk_lon);
		
  EXPECT_NEAR( 66.0, Units::to("deg", itrk1), 0.9);
  EXPECT_NEAR(273.0, Units::to("deg", itrk2), 0.9);
  EXPECT_NEAR( 78.0, Units::to("deg", itrk3), 0.9);
		
  double itrk4 = GreatCircle::initial_course(sng_lat, sng_lon, jfk_lat, jfk_lon);
  EXPECT_NEAR(359.0, Units::to("deg", itrk4), 2.0);  // Singapore to JFK is roughly a due north route.
		
  double itrk5 = GreatCircle::initial_course(lax_lat, lax_lon, lax_lat, lax_lon);
  EXPECT_FALSE(ISNAN(itrk5)); 

  double itrk6 = GreatCircle::initial_course(0.0, lax_lon, 0.0, jfk_lon);
  EXPECT_NEAR(90.0, Units::to("deg",itrk6), 0.1); //due east

  double itrk7 = GreatCircle::initial_course(lax_lat, lax_lon, jfk_lat, lax_lon);
  EXPECT_NEAR(  0.0, Units::to("deg",itrk7), 0.1);  // due north

  double itrk8 = GreatCircle::initial_course(jfk_lat, lax_lon, lax_lat, lax_lon);
  EXPECT_NEAR(180.0, Units::to("deg",itrk8), 0.1);  // due south

  // from the projectxy3 test, below
  double itrk9 = GreatCircle::initial_course(Units::from("deg",39.455751619177427), Units::from("deg", -97.136908163830554), Units::from("deg",33.011000000000003), Units::from("deg",-97.244000000000000));
  EXPECT_NEAR(3.1555556534521747, itrk9, 0.0000000001);  // due south

}

	
TEST_F(GreatCircleTest,testRepresentativeCourse) {
  LatLonAlt midpoint = GreatCircle::interpolate(lax, jfk, 0.5);
  double c1 = GreatCircle::initial_course(midpoint.lat(), midpoint.lon(), jfk_lat, jfk_lon);
  double c2 = GreatCircle::representative_course(lax_lat, lax_lon, jfk_lat, jfk_lon);
  EXPECT_NEAR(c1, c2, 0.00001);

  double c3 = GreatCircle::representative_course(lax_lat, lax_lon, lax_lat, lax_lon+Units::from("deg", 10));
  EXPECT_NEAR( 90.0, Units::to("deg", c3), 0.1);
  double c4 = GreatCircle::representative_course(lax_lat, lax_lon, lax_lat-Units::from("deg", 10), lax_lon);
  EXPECT_NEAR(180.0, Units::to("deg", c4), 0.1);
}

	
TEST_F(GreatCircleTest,testInterpolate) {
  LatLonAlt point = GreatCircle::interpolate(lax, jfk, 0.40);
  EXPECT_NEAR( 0.674909, point.lat(), 0.000001);
  EXPECT_NEAR(-1.77371,  point.lon(), 0.00001);
  EXPECT_NEAR(4.0, point.alt(), 0.00001);


  point = GreatCircle::interpolate(lax, jfk, 0.00);
  EXPECT_NEAR(lax.lat(), point.lat(), 0.000001);
  EXPECT_NEAR(lax.lon(), point.lon(), 0.00001);

  point = GreatCircle::interpolate(lax, jfk, 1.00);
  EXPECT_NEAR(jfk.lat(), point.lat(), 0.000001);
  EXPECT_NEAR(jfk.lon(), point.lon(), 0.00001);


  LatLonAlt temp = LatLonAlt::mk(lax_lat+0.000001, lax_lon+0.000001, 0.0);
  LatLonAlt nott = GreatCircle::interpolate(lax, temp, 0.40);
  EXPECT_TRUE(lax_lat != nott.lat()); // different (aka, not exactly the same)
  EXPECT_TRUE(lax_lon != nott.lon());
		  
  //temp = LatLonAlt::mk(lax_lat+0.0000001, lax_lon+0.0000001, 0.0);
  //LatLonAlt same = GreatCircle::interpolate(lax, temp, 0.40);
  //EXPECT_NEAR(lax_lat, same.lat(), 0.0);                           //***RWB***
  //EXPECT_NEAR(lax_lon, same.lon(), 0.0);                            //***RWB***

  temp = LatLonAlt::mk(lax_lat+0.5, lax_lon, 0.0);
  LatLonAlt above1 = GreatCircle::interpolate(lax, temp, 0.50);
  EXPECT_TRUE(lax_lat < above1.lat());
  EXPECT_NEAR(lax_lon, above1.lon(), 0.0);

  LatLonAlt above2 = GreatCircle::interpolate(temp, lax, 0.50);
  EXPECT_TRUE(lax_lat < above2.lat());
  EXPECT_NEAR(lax_lon, above2.lon(), 0.0);


  LatLonAlt p1 = LatLonAlt::make(-80.0, 20.0, 10.0);
  LatLonAlt p2 = LatLonAlt::make(-86.0, 20.0, 10.0);
  LatLonAlt c = GreatCircle::interpolate(p1,p2,-0.5);
  EXPECT_NEAR( -77.0, c.latitude(),  0.0000001);
  EXPECT_NEAR(  20.0, c.longitude(), 0.0000001);
  EXPECT_NEAR(  10.0, c.altitude(),  0.00000001);
  LatLonAlt cEst = GreatCircle::interpolateEst(p1,p2,-0.5);
  EXPECT_NEAR( -77.0, cEst.latitude(),  0.0000001);
  EXPECT_NEAR(  20.0, cEst.longitude(), 0.0000001);
  EXPECT_NEAR(  10.0, cEst.altitude(),  0.00000001);
  //f.pln(" $$ c = "+c.toString(15)+" cEst = "+cEst.toString(15));
  p2 = LatLonAlt::make(-86.0, 21.0, 10.0);
  c = GreatCircle::interpolate(p1,p2,-0.1);
  EXPECT_NEAR( -79.3999372, c.latitude(),  0.0000001);
  EXPECT_NEAR( 19.9620122, c.longitude(), 0.0000001);
  EXPECT_NEAR(  10.0, c.altitude(),  0.00000001);
  cEst = GreatCircle::interpolateEst(p1,p2,-0.1);
  EXPECT_NEAR( -79.39999999, cEst.latitude(),  0.0000001);
  EXPECT_NEAR(  19.90000, cEst.longitude(), 0.0000001);
  EXPECT_NEAR(  10.0, cEst.altitude(),  0.00000001);
  //f.pln(" $$ c = "+c.toString(15)+" cEst = "+cEst.toString(15));



}

TEST_F(GreatCircleTest,testInterpolateVertical) {
  LatLonAlt lax_high = lax.mkAlt(10000);
  LatLonAlt point = GreatCircle::interpolate(lax, lax_high, 0.5);
  EXPECT_NEAR( lax.lat(), point.lat(), 0.00000001);
  EXPECT_NEAR( lax.lon(), point.lon(), 0.00000001);
  EXPECT_NEAR(    5000.0, point.alt(), 0.00000001);		  
}

TEST_F(GreatCircleTest,testInterpolateExtrapolate) {
  LatLonAlt s1 = LatLonAlt::make(37.4701, -122.03, 10000); 
  LatLonAlt s2 = LatLonAlt::make(47.4701, -152.03, 20000);
  const double t = 3600;
  Velocity v3 = GreatCircle::velocity_initial(s2, s1, t);
  LatLonAlt s3 = GreatCircle::linear_initial(s2, v3, t*2);
  // s1, s2, s3 should all be on the same great circle, with s1 midway between s2 and s3
  
  LatLonAlt s = GreatCircle::interpolate(s3, s2, 0.5);
  EXPECT_NEAR(s1.lat(),  s.lat(), 0.0000001);
  EXPECT_NEAR(s1.lon(),  s.lon(), 0.0000001);
  EXPECT_NEAR(s1.alt(),  s.alt(), 0.0000001);
  
  s = GreatCircle::interpolate(s2, s3, 0.5);
  EXPECT_NEAR(s1.lat(),  s.lat(), 0.0000001);
  EXPECT_NEAR(s1.lon(),  s.lon(), 0.0000001);
  EXPECT_NEAR(s1.alt(),  s.alt(), 0.0000001);
  
  s = GreatCircle::interpolate(s1, s2, -1.0);
  EXPECT_NEAR(s3.lat(),  s.lat(), 0.0000001);
  EXPECT_NEAR(s3.lon(),  s.lon(), 0.0000001);
  EXPECT_NEAR(s3.alt(),  s.alt(), 0.0000001);
  
  s = GreatCircle::interpolate(s3, s1, 2.0);
  EXPECT_NEAR(s2.lat(),  s.lat(), 0.0000001);
  EXPECT_NEAR(s2.lon(),  s.lon(), 0.0000001);
  EXPECT_NEAR(s2.alt(),  s.alt(), 0.0000001);
}


	
TEST_F(GreatCircleTest,testLinearGCGS) {
  //Point on the great circle distance from LAX to JFK, going all the way around the earth
  Velocity unity = Velocity::mkTrkGsVs(0.0, 1.0, 1.0);
  Velocity very_fast = Velocity::mkTrkGsVs(0.0, GreatCircle::spherical_earth_radius*2*Pi, 1.0);
  LatLonAlt pos6 = GreatCircle::linear_gcgs(lax, jfk, very_fast, 1.0); 
  EXPECT_NEAR(0.0, Units::to("nmi",GreatCircle::distance(pos6, lax)), 0.00001);
		
  LatLonAlt temp = LatLonAlt::make();
  temp = LatLonAlt::mk(lax_lat+0.000001, lax_lon+0.000001, 0.0);
  LatLonAlt pos7 = GreatCircle::linear_gcgs(lax, temp, unity, 1.0);
  EXPECT_TRUE(lax_lat != pos7.lat());   // different (aka, not exactly the same)
  EXPECT_TRUE(lax_lon != pos7.lon());

  //temp = LatLonAlt::mk(lax_lat+0.0000001, lax_lon+0.0000001, 0.0);
  //LatLonAlt pos8 = GreatCircle::linear_gcgs(lax, temp, unity, 1.0);
  //EXPECT_NEAR(lax_lat, pos8.lat(), 0.0);                        //***RWB***
  //EXPECT_NEAR(lax_lon, pos8.lon(), 0.0);                        //***RWB***

  LatLonAlt pos9 = GreatCircle::linear_gcgs(lax, lax, unity, 1.0); // used to cause a divide by 0
  EXPECT_NEAR(lax_lat, pos9.lat(), 0.0);
  EXPECT_NEAR(lax_lon, pos9.lon(), 0.0);
}


TEST_F(GreatCircleTest, testLinearDist) {
	// There are two ways to get a new point along a great circle from at a distance, linear_gc and interpolate
	// They should give the same result
	LatLonAlt p1 = LatLonAlt::make(-8.0, 20.0, 10.0);
	LatLonAlt p2 = LatLonAlt::make(-8.0, 30.0, 10.0);
    LatLonAlt p3 = GreatCircle::linear_gc(p1,p2,4000000.0);
	double c = GreatCircle::initial_course(p1,p2);
    LatLonAlt p4 = GreatCircle::linear_initial(p1,c,4000000.0);
    EXPECT_NEAR( p3.latitude(),  p4.latitude(),  0.0);
    EXPECT_NEAR( p3.longitude(), p4.longitude(), 0.000000000001);
    EXPECT_NEAR( p3.altitude(),  p4.altitude(),  0.0);
}



TEST_F(GreatCircleTest,testLinearRhumbVelocity1) {
  double dist1 = GreatCircle::distance(lax_lat, lax_lon, jfk_lat, jfk_lon);
  double itrk1 = GreatCircle::initial_course(lax_lat, lax_lon, jfk_lat, jfk_lon);
  double itrk2 = GreatCircle::initial_course(jfk_lat, jfk_lon, lax_lat, lax_lon);
  Velocity v1 = Velocity::mkTrkGsVs(itrk1, dist1, 0.0);
  Velocity v1h = Velocity::mkTrkGsVs(itrk1, dist1/2.0, 0.0);
  Velocity v2h = Velocity::mkTrkGsVs(itrk2, dist1/2.0, 0.0);

  LatLonAlt pos2 = GreatCircle::linear_rhumb(lax, v1h, 1.0);
  LatLonAlt pos3 = GreatCircle::linear_rhumb(jfk, v2h, 1.0);
  EXPECT_NEAR(37.0, Units::to("nmi",GreatCircle::distance(pos2, pos3)), 1.0);  // compare distance at mid from LA to JFK and JFK to LA
		
  // Point at then end of the great circle distance from LAX to JFK using initial course from LAX
  LatLonAlt pos4 = GreatCircle::linear_rhumb(lax, v1, 1.0); 
  EXPECT_NEAR(475.0, Units::to("nmi",GreatCircle::distance(pos4, jfk)), 10.0);

  // Point at then end of the great circle distance from LAX to JFK using course at the midpoint between LAX-JFK
  LatLonAlt pos5 = GreatCircle::linear_rhumb(lax, Velocity::mkTrkGsVs(Units::from("deg", 78.0), dist1, 0.0), 1.0);
  EXPECT_NEAR(48.0, Units::to("nmi",GreatCircle::distance(pos5, jfk)), 1.0);

  LatLonAlt pos6 = GreatCircle::linear_rhumb(lax, Velocity::mkTrkGsVs(Units::from("deg",66.0), Units::from("knot", 100.0), 0.0), Units::from("hour", 1.0));
  EXPECT_NEAR( 0.604180, pos6.lat(), 0.001);
  EXPECT_NEAR(-2.034206, pos6.lon(), 0.001);

  LatLonAlt pos7 = GreatCircle::linear_rhumb(lax, Velocity::mkTrkGsVs(Units::from("deg",66.0), 0.0, 0.0), 1.0);
  EXPECT_NEAR(lax_lat, pos7.lat(), 0.0);
  EXPECT_NEAR(lax_lon, pos7.lon(), 0.0);
		
  LatLonAlt temp = LatLonAlt::make();
  Velocity v_temp;
  temp = LatLonAlt::mk(0, 0.186169, 0.0);
  v_temp = Velocity::mkTrkGsVs(1.5708, 195.57, 0.0);
  LatLonAlt pos8 = GreatCircle::linear_rhumb(temp, v_temp, -1515.15);
  EXPECT_NEAR(0.0, pos8.lat(), 0.000001);
  EXPECT_NEAR(0.139627, pos8.lon(), 0.00001);
		
  temp = LatLonAlt::mk(0.0, 0.18616903532247916, 0.0);
  v_temp = Velocity::mkTrkGsVs(1.5707963267948966, 195.56999899712983, 0.0);
  LatLonAlt pos9 = GreatCircle::linear_rhumb(temp, v_temp, -1515.1512999999995);
  EXPECT_NEAR(-2.849864603406237E-18, pos9.lat(), 0.0000001);
  EXPECT_NEAR(0.1396272143600527,     pos9.lon(),    0.0000001);

  temp = LatLonAlt::make(-0.0000000000000000000001, 0.908333333333333, 5000.000000000000000);
  v_temp = Velocity::makeTrkGsVs(90.00, "deg", 432.00, "kn", 0.00, "fpm");
  LatLonAlt pos10 = GreatCircle::linear_rhumb(temp, v_temp, 46.72143333333338);
  temp = LatLonAlt::make(-0.000000000000000, 0.908300000000000, 5000.000000000000000);
  v_temp = Velocity::makeTrkGsVs(90.00, "deg", 432.01, "kn", 0.00, "fpm");
  LatLonAlt pos11 = GreatCircle::linear_rhumb(temp, v_temp, 46.72140000000002);

  EXPECT_NEAR(3.47, GreatCircle::distance(pos10, pos11), 0.005);
  EXPECT_NEAR(-0.000000000000000, Units::to("deg", pos10.lat()), 0.0000001);
  EXPECT_NEAR(1.00177619999, Units::to("deg", pos10.lon()), 0.0000001);
  EXPECT_NEAR(5000.00000000000, Units::to("ft", pos10.alt()), 0.0000001);
  EXPECT_NEAR(-0.000000000000000, Units::to("deg", pos11.lat()), 0.0000001);
  EXPECT_NEAR(1.00174496, Units::to("deg", pos11.lon()), 0.0000001);
  EXPECT_NEAR(5000.00000000000, Units::to("ft", pos11.alt()), 0.0000001);
}

	
TEST_F(GreatCircleTest,testLinearRhumbVelocity2) {
  double dist1 = GreatCircle::distance(lax_lat, lax_lon, jfk_lat, jfk_lon);
  double itrk1 = GreatCircle::initial_course(lax_lat, lax_lon, jfk_lat, jfk_lon);
  double itrk2 = GreatCircle::initial_course(jfk_lat, jfk_lon, lax_lat, lax_lon);
		
  Velocity v1 = Velocity::mkTrkGsVs(itrk1, dist1/2, 0.0);
  Velocity v2 = Velocity::mkTrkGsVs(itrk2, dist1/2, 0.0);

  LatLonAlt pos2 = GreatCircle::linear_rhumb(lax, v1, 1.0);
  LatLonAlt pos3 = GreatCircle::linear_rhumb(jfk, v2, 1.0);
  EXPECT_NEAR(37.0, Units::to("nmi",GreatCircle::distance(pos2.lat(),pos2.lon(),
				      pos3.lat(), pos3.lon())), 1.0);  // compare dist at mid from LA to JFK and JFK to LA
		
  // Point at then end of the great circle distance from LAX to JFK using initial course from LAX
  LatLonAlt pos4 = GreatCircle::linear_rhumb(lax, v1, 2.0); 
  EXPECT_NEAR(475.0, Units::to("nmi",GreatCircle::distance(pos4.lat(),pos4.lon(), jfk_lat, jfk_lon)), 10.0);

  //		// Point at then end of the great circle distance from LAX to JFK using course at the midpoint between LAX-JFK
  //		Vect2 pos5 = GreatCircle.linear(lax_lat, lax_lon, from("deg", 78.0), dist1, 1.0); 
  //		EXPECT_NEAR(48.0, to("nmi",distance(pos5.x,pos5.y, jfk_lat, jfk_lon)), 1.0);
  //
  //		Vect2 pos6 = GreatCircle.linear(lax_lat, lax_lon, from("deg",66.0), from("knot", 100.0), from("hour", 1.0));
  //		EXPECT_NEAR( 0.604180, pos6.x, 0.001);
  //		EXPECT_NEAR(-2.034206, pos6.y, 0.001);
  //
  //		Vect2 pos7 = GreatCircle.linear(lax_lat, lax_lon, from("deg",66.0), 0.0, 1.0);
  //		EXPECT_NEAR(lax_lat, pos7.x, 0.0);
  //		EXPECT_NEAR(lax_lon, pos7.y, 0.0);
  //		
  //		Vect2 pos8 = GreatCircle.linear(0, 0.186169, 1.5708, 195.57, -1515.15);
  //		EXPECT_NEAR(0.0, pos8.x, 0.000001);
  //		EXPECT_NEAR(0.139627, pos8.y, 0.00001);
  //		
  //		Vect2 pos9 = GreatCircle.linear(0.0, 0.18616903532247916, 1.5707963267948966, 195.56999899712983, -1515.1512999999995);
  //		EXPECT_NEAR(-2.849864603406237E-18, pos9.x, 0.0000001);
  //		EXPECT_NEAR(0.1396272143600527,     pos9.y,    0.0000001);
}

TEST_F(GreatCircleTest,testLinearRhumbDist) {
	LatLonAlt pos1;
	LatLonAlt pos2;

	//
	// Rhumb line is a constant track
	//

	pos2 = LatLonAlt::mk(jfk);
	for (int i = 0; i < 100; i++) {
	  pos1 = GreatCircle::linear_rhumb(jfk, Units::from("deg",10.0), Units::from("nmi",10.0) * i);
	  EXPECT_NEAR(pos1.latitude(), pos2.latitude(), 0.000001);
	  EXPECT_NEAR(pos1.longitude(), pos2.longitude(), 0.000001);
	  pos2 = GreatCircle::linear_rhumb(pos2, Units::from("deg",10.0), Units::from("nmi",10.0));
	}

	//
	// Go to the pole
	//

	pos1 = GreatCircle::linear_rhumb(jfk, 0.0, Units::from("nmi",100.0));
	EXPECT_NEAR(42.3, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	pos1 = GreatCircle::linear_rhumb(jfk, 0.0, Units::from("nmi",1000.0));
	EXPECT_NEAR(57.3, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	pos1 = GreatCircle::linear_rhumb(jfk, 0.0, Units::from("nmi",1500.0));
	EXPECT_NEAR(65.633333333333333, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	double d = GreatCircle::distance(Units::from("deg",90.0), Units::from("deg",180.0), jfk_lat, jfk_lon); // distance to north pole
	pos1 = GreatCircle::linear_rhumb(jfk, 0.0, d);
	EXPECT_NEAR(90.0, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	pos1 = GreatCircle::linear_rhumb(jfk, 0.0, d+Units::from("nmi",60.0));
	EXPECT_NEAR(90.0, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	//
	// Rhumb line going towards the pole
	//

	pos1 = GreatCircle::linear_rhumb(jfk, Units::from("deg", 1.0), 5*d);
	EXPECT_NEAR(Units::from("deg", 90.0), pos1.lat(), 0.000001);
	EXPECT_NEAR(1.717885, pos1.lon(), 0.0001);

	pos2 = LatLonAlt::mk(jfk);
	for (int i = 0; i < 200; i++) {
	  pos1 = GreatCircle::linear_rhumb(jfk, Units::from("deg",1.0), Units::from("nmi",10.0) * i);
	  EXPECT_NEAR(pos1.latitude(), pos2.latitude(), 0.000001);
	  EXPECT_NEAR(pos1.longitude(), pos2.longitude(), 0.000001);
	  pos2 = GreatCircle::linear_rhumb(pos2, Units::from("deg",1.0), Units::from("nmi",10.0));
	}

	LatLonAlt orig = LatLonAlt::make(89.0, 0.0, 0.0);
	pos2 = LatLonAlt::mk(orig);
	for (int i = 0; i < 400; i++) {
	  pos1 = GreatCircle::linear_rhumb(orig, Units::from("deg",1.0), Units::from("nmi",0.25) * i);
	  EXPECT_NEAR(pos1.latitude(), pos2.latitude(), 0.000001);
	  if (i < 241) {
	    EXPECT_NEAR(pos1.longitude(), pos2.longitude(), 0.000001);
	  }
	  pos2 = GreatCircle::linear_rhumb(pos2, Units::from("deg",1.0), Units::from("nmi",0.25));
	}

	//
	// Leaving the north pole
	//

	LatLonAlt np = LatLonAlt::mk(Units::from("deg", 90.0), jfk_lon,0.0);
	pos1 = GreatCircle::linear_rhumb(np, Units::from("deg", 180.0), d);
	EXPECT_NEAR(jfk_lat, pos1.lat(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	pos1 = GreatCircle::linear_rhumb(np, Units::from("deg", 180.0), d-Units::from("nmi", 100.0));
	EXPECT_NEAR(42.3, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	// going northerly from the north pole
	pos1 = GreatCircle::linear_rhumb(np, Units::from("deg", 45.0), 5485624.0000000000000000000); //d);
	EXPECT_NEAR(Units::from("deg", 90.0), pos1.lat(), 0.000001);
	EXPECT_NEAR(1.375, pos1.lon(), 0.0001);							//^^

	// going southerly from the north pole
	pos1 = GreatCircle::linear_rhumb(np, Units::from("deg", 135.0), d);
	EXPECT_NEAR( 0.96154, pos1.lat(), 0.00001);
	EXPECT_NEAR( 1.3605957, pos1.lon(), 0.00001);

	//
	// Leaving the south pole
	//

	LatLonAlt sp = LatLonAlt::mk(Units::from("deg", -90.0), jfk_lon,0.0);
	d = GreatCircle::distance(sp, jfk);
	pos1 = GreatCircle::linear_rhumb(sp, Units::from("deg", 0.0), d);
	EXPECT_NEAR(jfk_lat, pos1.lat(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	pos1 = GreatCircle::linear_rhumb(sp, Units::from("deg", 0.0), d+Units::from("nmi", 100.0));
	EXPECT_NEAR(42.3, pos1.latitude(), 0.000001);
	EXPECT_NEAR(jfk_lon, pos1.lon(), 0.000001);

	// going northerly from the south pole
	pos1 = GreatCircle::linear_rhumb(sp, Units::from("deg", 45.0), d);
	EXPECT_NEAR(0.0413943, pos1.lat(), 0.000001);
	EXPECT_NEAR(2.4650800, pos1.lon(), 0.0001);

	// going southerly from the south pole
	pos1 = GreatCircle::linear_rhumb(sp, Units::from("deg", 135.0), d);
	EXPECT_NEAR(Units::from("deg", -90.0), pos1.lat(), 0.00001);
	EXPECT_NEAR(0.25, pos1.lon(), 0.00001);						//^^
}

	
// move to SimpleProjectionTest
TEST_F(GreatCircleTest,testProjectXY) {
  Vect2 xy_jfk_lax = SimpleProjection::projectXY(lax_lat, lax_lon, jfk_lat, jfk_lon);
  Vect2 xy_lax_jfk = SimpleProjection::projectXY(jfk_lat, jfk_lon, lax_lat, lax_lon);
		
  EXPECT_NEAR(2144.0, Units::to("nmi", xy_jfk_lax.norm()), 1.0);
  EXPECT_NEAR(78.0, Units::to("deg", xy_jfk_lax.compassAngle()), 0.9);
  EXPECT_TRUE(xy_jfk_lax.x > 0);
  EXPECT_TRUE(xy_jfk_lax.y > 0);
		
  EXPECT_NEAR(2144.0, Units::to("nmi", xy_lax_jfk.norm()), 1.0);
  EXPECT_NEAR(180.0 + 78.0, Units::to("deg", xy_lax_jfk.compassAngle()), 0.9);
  EXPECT_TRUE(xy_lax_jfk.x < 0);
  EXPECT_TRUE(xy_lax_jfk.y < 0);

  EXPECT_NEAR(0.0, xy_jfk_lax.Add(xy_lax_jfk).norm(), 0.01);
}

	
// move to SimpleProjectionTest
TEST_F(GreatCircleTest,testProjectXY2) {
  Vect2 xy_lax = SimpleProjection::projectXY(lax_lat, lax_lon, lax_lat, lax_lon); // same point
  EXPECT_NEAR(0.0, xy_lax.x, 0.0000001);
  EXPECT_NEAR(0.0, xy_lax.y, 0.0000001);
}

	
// move to SimpleProjectionTest
TEST_F(GreatCircleTest,testProjectXY3) {
  Vect2 xy = SimpleProjection::projectXY(0.5819625857849893, -1.6972279778093657, 0.5761506393758482, -1.6972279778093657);
  //EXPECT_NEAR(0.006820, xy.x, 0.000005);
  EXPECT_NEAR(0.0, xy.x, 1.0e-8);
  EXPECT_NEAR(-37003.0, xy.y, 0.5);
}

	
// move to SimpleProjectionTest
TEST_F(GreatCircleTest,testProjectXYZ) {
  Vect2 xy_jfk_lax2 = SimpleProjection::projectXY(lax_lat, lax_lon, jfk_lat, jfk_lon);
  Vect3 xy_jfk_lax3 = SimpleProjection::projectXYZ(lax, jfk);
		
  EXPECT_TRUE(xy_jfk_lax3.x > 0.0);
  EXPECT_TRUE(xy_jfk_lax3.y > 0.0);
  EXPECT_TRUE(xy_jfk_lax3.z > 0.0);
		
  EXPECT_NEAR(xy_jfk_lax3.x, xy_jfk_lax2.x, 0.0);
  EXPECT_NEAR(xy_jfk_lax3.y, xy_jfk_lax2.y, 0.0);
}

	
TEST_F(GreatCircleTest,testVelocity_trkgs1) {
  Velocity vel;
  vel = GreatCircle::velocity_average(lax, jfk, 3600.0);
  EXPECT_NEAR(78.70940263, Units::to("deg", vel.trk()), 0.0000001);
  EXPECT_NEAR(2143.7, Units::to("knot", vel.gs()), 1.000000);
		
  LatLonAlt temp = LatLonAlt::mk(lax_lat+Units::from("deg", 10), lax_lon, 0.0);
  vel = GreatCircle::velocity_average(lax, temp, 3600.0); 
  EXPECT_NEAR(  0.0, Units::to("deg", vel.trk()), 0.0000000001);
  EXPECT_NEAR(600.0, Units::to("knot", vel.gs()), 0.0000001);

  LatLonAlt temp3 = LatLonAlt::mk(lax_lat-Units::from("deg", 10), lax_lon, 0.0);
  vel = GreatCircle::velocity_average(lax, temp3, 3600.0);
  EXPECT_NEAR(180.0, Units::to("deg", vel.trk()), 0.001);
  EXPECT_NEAR(600.0, Units::to("knot", vel.gs()), 0.0000001);
		
  LatLonAlt temp1 = LatLonAlt::mk(0.0, lax_lon, 0.0);
  LatLonAlt temp2 = LatLonAlt::mk(0.0, lax_lon-Units::from("deg", 10), 0.0);
  vel = GreatCircle::velocity_average(temp1, temp2, 3600.0); 
  EXPECT_NEAR(270.0, Units::to("deg", vel.compassAngle()), 0.001);
  EXPECT_NEAR(600.0, Units::to("knot", vel.gs()), 0.0000001);
}

	
TEST_F(GreatCircleTest,testVelocity_trkgs2) {
  Velocity vel;
  vel = GreatCircle::velocity_average(lax, lax, 10.0); // same point
  EXPECT_NEAR(0.0, vel.trk(), 0.0000001);
  EXPECT_NEAR(0.0, vel.gs(), 0.0000001);
  vel = GreatCircle::velocity_average(lax, jfk, 0.0); // no time
  EXPECT_NEAR(0.0, vel.trk(), 0.0000001);
  EXPECT_NEAR(0.0, vel.gs(), 0.0000001);
}

	
TEST_F(GreatCircleTest,testVelocityvxvy1) {
  Velocity vel;
  vel = GreatCircle::velocity_average(lax, jfk, 3600.0);
  EXPECT_NEAR(2102.2, Units::to("knot", vel.x), 0.1);
  EXPECT_NEAR( 419.7, Units::to("knot", vel.y), 1.000000);

  LatLonAlt temp = LatLonAlt::mk(lax_lat-Units::from("deg", 10), lax_lon, 0.0);
  vel = GreatCircle::velocity_average(lax, temp, 3600.0); 
  EXPECT_NEAR(   0.0, Units::to("knot", vel.x), 0.001);
  EXPECT_NEAR(-600.0, Units::to("knot", vel.y), 0.0000001);
		
  LatLonAlt temp1 = LatLonAlt::mk(0.0, lax_lon, 0.0);
  LatLonAlt temp2 = LatLonAlt::mk(0.0, lax_lon-Units::from("deg", 10), 0.0);
  vel = GreatCircle::velocity_average(temp1, temp2, 3600.0); 
  EXPECT_NEAR(-600.0, Units::to("knot", vel.x), 0.001);
  EXPECT_NEAR(   0.0, Units::to("knot", vel.y), 0.0000001);
}

	
TEST_F(GreatCircleTest,testVelocityVxVy2) {
  Velocity vel;
  vel = GreatCircle::velocity_average(lax, lax, 10.0); // same point
  EXPECT_NEAR(0.0, vel.x, 0.0000001);
  EXPECT_NEAR(0.0, vel.y, 0.0000001);
  vel = GreatCircle::velocity_average(lax, jfk, 0.0); // no time
  EXPECT_NEAR(0.0, vel.x, 0.0000001);
  EXPECT_NEAR(0.0, vel.y, 0.0000001);
}

	
TEST_F(GreatCircleTest,testVelocityClose) {
  double d;
  LatLonAlt temp = LatLonAlt::make();
  temp = LatLonAlt::mk(lax_lat, 3.0e-6+lax_lon, 0.0);
  d = GreatCircle::distance(lax, temp);
  EXPECT_NEAR(15.0, d, 4.0);

  Velocity v1 = GreatCircle::velocity_average(lax, temp, 1.0);
  EXPECT_TRUE(0.0 < v1.x);
  EXPECT_NEAR(0.0, v1.y, 1.0e-8);
		
  temp = LatLonAlt::mk(lax_lat+1.0e-7, lax_lon, 0.0);
  d = GreatCircle::distance(lax, temp);
  EXPECT_NEAR(0.0, d, 1.0);
		
  Velocity v2 = GreatCircle::velocity_average(lax, temp, 1.0);
  //EXPECT_NEAR(0.0, v2.x, 0.0);                                    //***RWB***
  //EXPECT_NEAR(0.0, v2.y, 0.0);                                    //***RWB***
}

	
TEST_F(GreatCircleTest,testInitialVelocity_trkgs1) {
  Velocity vel1, vel2;
  LatLonAlt temp1 = LatLonAlt::mk(0.0, lax_lon, 0.0);
  LatLonAlt temp2 = LatLonAlt::mk(0.0, lax_lon-Units::from("deg", 10), 0.0);
  vel1 = GreatCircle::velocity_average(temp1, temp2, 3600.0); 
  EXPECT_NEAR(270.0, Units::to("deg", vel1.compassAngle()), 0.001);
  EXPECT_NEAR(600.0, Units::to("knot", vel1.gs()), 0.0000001);
		
  vel1 = GreatCircle::velocity_initial(lax, jfk, 3600.0);
  vel2 = GreatCircle::velocity_average(lax, jfk, 3600.0);
  EXPECT_FALSE(std::abs(vel1.trk() - vel2.trk()) < 0.1);

  LatLonAlt mid = GreatCircle::interpolate(lax, jfk, 0.5);
  vel1 = GreatCircle::velocity_initial(mid, jfk, 3600.0 / 2);
  vel2 = GreatCircle::velocity_average(lax, jfk, 3600.0);
  EXPECT_NEAR(vel1.x, vel2.x, 0.0000001);
  EXPECT_NEAR(vel1.y, vel2.y, 0.0000001);
  EXPECT_NEAR(vel1.z, vel2.z, 0.0000001);
}

	
TEST_F(GreatCircleTest, testFinalVelocity) {
  Velocity vel1;
  LatLonAlt temp1 = LatLonAlt::mk(0.0, lax_lon, 0.0);
  LatLonAlt temp2 = LatLonAlt::mk(0.0, lax_lon-Units::from("deg", 10), 0.0);
  vel1 = GreatCircle::velocity_final(temp1, temp2, 3600.0); 
  EXPECT_NEAR(270.0, Units::to("deg", vel1.compassAngle()), 0.001);
  EXPECT_NEAR(600.0, Units::to("knot", vel1.gs()), 0.0000001);
}

TEST_F(GreatCircleTest, testCompareInitialAndFinalVelocity) {
  LatLonAlt s1 = LatLonAlt::make(37.4701, -122.03,  10000); 
  LatLonAlt s2 = LatLonAlt::make(47.4701, -152.03,  20000);
  const double t = 3600;
  
  // the first position parameter is the source position, the second position parameter is another point on that circle
  // positive time is moving from p1 toward p2
  // negative time is moving from p1 away from p2 (along the great circle formed by p1 and p2)
  
  // initial velocity is a negated final velocity going the opposite direction (track and vs)
  Velocity v1 = GreatCircle::velocity_initial(s1, s2, t);
  Velocity v2 = GreatCircle::velocity_final(s1, s2, t);
  Velocity v3 = GreatCircle::velocity_initial(s2, s1, t);
  Velocity v4 = GreatCircle::velocity_final(s2, s1, t);

  // v1 == -v4
  EXPECT_NEAR(Util::to_pi(v1.trk()), Util::to_pi(v4.trk()+Pi), 0.001);
  EXPECT_NEAR(v1.gs(), v4.gs(), 0.001);
  EXPECT_NEAR(v1.vs(), -v4.vs(), 0.001);
  // v2 == -v3
  EXPECT_NEAR(Util::to_pi(v2.trk()), Util::to_pi(v3.trk()+Pi), 0.001);
  EXPECT_NEAR(v2.gs(), v3.gs(), 0.001);
  EXPECT_NEAR(v2.vs(), -v3.vs(), 0.001);
}

TEST_F(GreatCircleTest, testVelocityWithNegativeTime) {
  LatLonAlt s1 = LatLonAlt::make(37.4701, -122.03,  10000); 
  LatLonAlt s2 = LatLonAlt::make(47.4701, -152.03,  20000);
  const double t = 3600;
  Velocity v3 = GreatCircle::velocity_initial(s2, s1, t);
  LatLonAlt s3 = GreatCircle::linear_initial(s2, v3, t*2);
  // s1, s2, s3 should all be on the same great circle, with s1 midway between s2 and s3
  
  // Negating time is moving away from the second position, or moving toward a point that is the "reflection" of the second point, along the same great circle.
  // (Here s3 is the "reflection" of s2.)
  // This mean that final and average velocities with negative times do not neatly correspond with either s1 or s2

  Velocity vps1s2i = GreatCircle::velocity_initial(s1, s2, t);
  Velocity vps1s2f = GreatCircle::velocity_final(s1, s2, t);
  Velocity vps1s3i = GreatCircle::velocity_initial(s1, s3, t);
  Velocity vps1s3f = GreatCircle::velocity_final(s1, s3, t);
		
  Velocity vns1s2i = GreatCircle::velocity_initial(s1, s2, -t);
  Velocity vns1s2f = GreatCircle::velocity_final(s1, s2, -t);
  Velocity vns1s3i = GreatCircle::velocity_initial(s1, s3, -t);
  Velocity vns1s3f = GreatCircle::velocity_final(s1, s3, -t);
		
  Velocity vps2s1i = GreatCircle::velocity_initial(s2, s1, t);
  Velocity vps2s1f = GreatCircle::velocity_final(s2, s1, t);
  Velocity vps3s1i = GreatCircle::velocity_initial(s3, s1, t);
  Velocity vps3s1f = GreatCircle::velocity_final(s3, s1, t);
  
  //
  //   <-vns1s2f  <-vns1s2i  vns1s3i->   vns1s3f->
  //   <-vps1s3f  <-vps1s3i  vps1s2i->   vps1s2f->
  // s3--------------------s1---------------------s2
  //   vps3s1i->  vps3s1f->  <-vps2s1f   <-vps2s1i
  //
  
  // Test the initial velocities (and positive finals)
  
  // vps1s2i == vns1s3i
  EXPECT_NEAR(vps1s2i.trk(), vns1s3i.trk(), 0.001);
  EXPECT_NEAR(vps1s2i.gs(),  vns1s3i.gs(),  0.001);
  EXPECT_NEAR(vps1s2i.vs(),  vns1s3i.vs(),  0.001);
  // vps1s2i == vps3s1f
  EXPECT_NEAR(vps1s2i.trk(), vps3s1f.trk(), 0.001);
  EXPECT_NEAR(vps1s2i.gs(),  vps3s1f.gs(), 0.001);
  EXPECT_NEAR(vps1s2i.vs(),  vps3s1f.vs(), 0.001);
  // vps1s3i == vns1s2i
  EXPECT_NEAR(vps1s3i.trk(), vns1s2i.trk(), 0.001);
  EXPECT_NEAR(vps1s3i.gs(),  vns1s2i.gs(), 0.001);
  EXPECT_NEAR(vps1s3i.vs(),  vns1s2i.vs(), 0.001);
  // vps1s3i == vps2s1f
  EXPECT_NEAR(vps1s3i.trk(), vps2s1f.trk(), 0.001);
  EXPECT_NEAR(vps1s3i.gs(),  vps2s1f.gs(), 0.001);
  EXPECT_NEAR(vps1s3i.vs(),  vps2s1f.vs(), 0.001);
  
  
  // Test the final velocities (especially the negative final velocities)
  
  // vps1s2f == vns1s3f
  EXPECT_NEAR(vps1s2f.trk(), vns1s3f.trk(), 0.001);
  EXPECT_NEAR(vps1s2f.gs(),  vns1s3f.gs(), 0.001);
  EXPECT_NEAR(vps1s2f.vs(),  vns1s3f.vs(), 0.001);
  // vps1s3f == vns1s2f
  EXPECT_NEAR(vps1s3f.trk(), vns1s2f.trk(), 0.001);
  EXPECT_NEAR(vps1s3f.gs(), vns1s2f.gs(), 0.001);
  EXPECT_NEAR(vps1s3f.vs(), vns1s2f.vs(), 0.001);
  
  
  // Test some 180 relationships
  
  // vps1s2i == -vps1s3i
  EXPECT_NEAR(vps1s2i.trk(), Util::to_pi(vps1s3i.trk()+Pi), 0.001);
  EXPECT_NEAR(vps1s2i.gs(),  vps1s3i.gs(), 0.001);
  EXPECT_NEAR(vps1s2i.vs(), -vps1s3i.vs(), 0.001);
  // vps3s1f == -vps2s1f
  EXPECT_NEAR(vps3s1f.trk(), Util::to_pi(vps2s1f.trk()+Pi), 0.001);
  EXPECT_NEAR(vps3s1f.gs(),  vps2s1f.gs(), 0.001);
  EXPECT_NEAR(vps3s1f.vs(), -vps2s1f.vs(), 0.001);
  
  // vps1s2f == -vps2s1i
  EXPECT_NEAR(vps1s2f.trk(), Util::to_pi(vps2s1i.trk()+Pi), 0.001);
  EXPECT_NEAR(vps1s2f.gs(),  vps2s1i.gs(), 0.001);
  EXPECT_NEAR(vps1s2f.vs(), -vps2s1i.vs(), 0.001);
  // vps1s3f == -vps3s1i
  EXPECT_NEAR(vps1s3f.trk(), Util::to_pi(vps3s1i.trk()+Pi), 0.001);
  EXPECT_NEAR(vps1s3f.gs(),  vps3s1i.gs(), 0.001);
  EXPECT_NEAR(vps1s3f.vs(), -vps3s1i.vs(), 0.001);
}

TEST_F(GreatCircleTest, testAverageVelocityWithNegativeTime) {
  LatLonAlt s1 = LatLonAlt::make(37.4701, -122.03,  10000); 
  LatLonAlt s2 = LatLonAlt::make(47.4701, -152.03,  20000);
  double t = 3600;
  Velocity v = GreatCircle::velocity_initial(s2, s1, t);
  LatLonAlt s3 = GreatCircle::linear_initial(s2, v, t*2);
  // s1, s2, s3 should all be on the same great circle, with s1 midway between s2 and s3
  
  // averages.  Negating time should result in the average velocity between the start and reflection point.
  
  Velocity v1 = GreatCircle::velocity_average(s1, s2, t);
  Velocity v2 = GreatCircle::velocity_average(s2, s1, t);
  Velocity v3 = GreatCircle::velocity_average(s1, s3, t);
  Velocity v4 = GreatCircle::velocity_average(s3, s1, t);
  Velocity v5 = GreatCircle::velocity_average(s1, s2, -t);
  Velocity v6 = GreatCircle::velocity_average(s1, s3, -t);
  
  //
  //            v4->    <-v2
  //          <-v3        v1->
  //      s3---------s1---------s2
  //          <-v5        v6->
  //
  
  
  // v1 == -v2
  EXPECT_NEAR(v1.trk(), Util::to_pi(v2.trk()+Pi), 0.001);
  EXPECT_NEAR(v1.gs(),  v2.gs(), 0.001);
  EXPECT_NEAR(v1.vs(), -v2.vs(), 0.001);
  // v3 == -v4
  EXPECT_NEAR(v3.trk(), Util::to_pi(v4.trk()+Pi), 0.001);
  EXPECT_NEAR(v3.gs(),  v4.gs(), 0.001);
  EXPECT_NEAR(v3.vs(), -v4.vs(), 0.001);
  // v1 == v6
  EXPECT_NEAR(v1.trk(), v6.trk(), 0.001);
  EXPECT_NEAR(v1.gs(),  v6.gs(), 0.001);
  EXPECT_NEAR(v1.vs(),  v6.vs(), 0.001);
  // v3 == v5
  EXPECT_NEAR(v3.trk(), v5.trk(), 0.001);
  EXPECT_NEAR(v3.gs(),  v5.gs(), 0.001);
  EXPECT_NEAR(v3.vs(),  v5.vs(), 0.001);
}
	

TEST_F(GreatCircleTest, testXTrack) {
	LatLonAlt p1 = LatLonAlt::mk(0.0,                   0.0, 0.0);
	LatLonAlt p2 = LatLonAlt::mk(0.0, Units::from("deg", 20), 0.0);

	LatLonAlt oc = LatLonAlt::mk(-Units::from("deg", 10), 0.0, 0.0);
	double d = GreatCircle::cross_track_distance(p1,p2,oc);
	EXPECT_NEAR( GreatCircle::distance(p1,oc), d, 0.0);

	LatLonAlt oc1 = LatLonAlt::mk(-Units::from("deg", 10), Units::from("deg", 10), 0.0);
	LatLonAlt oc2 = LatLonAlt::mk(+Units::from("deg", 10), Units::from("deg", 10), 0.0);
	double d1 = GreatCircle::cross_track_distance(p1,p2,oc1);
	double d2 = GreatCircle::cross_track_distance(p1,p2,oc2);
	EXPECT_NEAR(    d1, -d2, 0.000000001);
	EXPECT_NEAR( 600.0, Units::to("nmi",d1), 0.00001);
	EXPECT_NEAR(-600.0, Units::to("nmi",d2), 0.00001);

	LatLonAlt oc3 = LatLonAlt::mk(-Units::from("deg", 10), -Units::from("deg", 10), 0.0);
	LatLonAlt oc4 = LatLonAlt::mk(+Units::from("deg", 10), -Units::from("deg", 10), 0.0);
	double d3 = GreatCircle::cross_track_distance(p1,p2,oc3);
	double d4 = GreatCircle::cross_track_distance(p1,p2,oc4);
	EXPECT_NEAR(    d3, -d4, 0.000000001);
	EXPECT_NEAR( 600.0, Units::to("nmi",d3), 0.00001);
	EXPECT_NEAR(-600.0, Units::to("nmi",d4), 0.00001);
}


TEST_F(GreatCircleTest, testRepresentativeWithCourseVelocityAverage) {
	LatLonAlt s1 = LatLonAlt::make(37.4701, -122.03,  10000);
	double track = Units::from("deg",33.0);
	double speed = Units::from("kn",473);
	double dist = Units::from("nm",20.0);
	LatLonAlt s2 = GreatCircle::linear_initial(s1, track, dist);
	Velocity vA = GreatCircle::velocity_average(s1,s2, dist/speed);
	double repTrack = GreatCircle::representative_course(s1.lat(), s1.lon(), s2.lat(), s2.lon());
	//f.pln(" vA = "+vA.toString8NP()+" repTrack = "+Units.str8("deg",repTrack));
	EXPECT_NEAR(repTrack,Util::to_2pi(vA.trk()),0.00001);
}


	
TEST_F(GreatCircleTest, testCollinear) {
  LatLonAlt p1 = LatLonAlt::make(10.0, 0.0, 0.0);
  LatLonAlt p2 = LatLonAlt::make(10.0, 20,  0.0);
  LatLonAlt p3 = GreatCircle::interpolate(p1,p2,0.7);
  EXPECT_TRUE(GreatCircle::collinear(p1,p2,p3));
  EXPECT_TRUE(GreatCircle::collinear(p2,p1,p3));
  EXPECT_TRUE(GreatCircle::collinear(p3,p2,p1));
  p1 = LatLonAlt::make(70.0, 4.0, 0.0);
  p2 = LatLonAlt::make(70.0, 123,  1000.0);
  p3 = GreatCircle::interpolate(p1,p2,0.7);
  EXPECT_TRUE(GreatCircle::collinear(p1,p2,p3));
  EXPECT_TRUE(GreatCircle::collinear(p2,p1,p3));
  EXPECT_TRUE(GreatCircle::collinear(p3,p1,p2));
  p2 = LatLonAlt::make(70.1, 123,  1000.0);
  EXPECT_FALSE(GreatCircle::collinear(p1,p2,p3));
  EXPECT_FALSE(GreatCircle::collinear(p2,p1,p3));
  EXPECT_FALSE(GreatCircle::collinear(p3,p1,p2));
  p2 = LatLonAlt::make(70.0, 123.00000000001,  1000.0);
  EXPECT_FALSE(GreatCircle::collinear(p1,p2,p3));
  EXPECT_FALSE(GreatCircle::collinear(p2,p1,p3));
  EXPECT_FALSE(GreatCircle::collinear(p3,p1,p2));
  p2 = LatLonAlt::make(70.00000000001, 123,  1000.0);
  EXPECT_FALSE(GreatCircle::collinear(p1,p2,p3));
  EXPECT_FALSE(GreatCircle::collinear(p2,p1,p3));
  EXPECT_FALSE(GreatCircle::collinear(p3,p1,p2));
}

	
TEST_F(GreatCircleTest, testClosestPointCircle) {
  LatLonAlt p1 = LatLonAlt::make(0.0,  0.0, 10.0);
  LatLonAlt p2 = LatLonAlt::make(0.0, 10.0, 20.0);
  LatLonAlt p3;
  LatLonAlt x;	        
		        
  // special cases - collinears
		        
  LatLonAlt c = GreatCircle::closest_point_circle(p1,p2,p1);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p1));
		        
  c = GreatCircle::closest_point_circle(p1,p2,p2);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p2));
		        
  c = GreatCircle::closest_point_circle(p1,p1,p2);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p2));
		        
  p3 = LatLonAlt::mk(p1.lat()+1.0e-14,p1.lon(),p2.alt());
  c = GreatCircle::closest_point_circle(p1,p3,p2);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p2));
		        
  p3 = GreatCircle::interpolate(p1,p2,0.7);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p3));
		        
  p3 = LatLonAlt::make(0.0, 180.0, 0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p3));

  // general cases
		        
  p3 = GreatCircle::interpolate(p1,p2,0.5);
  x = LatLonAlt::mk(p3.lat()+0.5, p3.lon(), p3.alt());
  c = GreatCircle::closest_point_circle(p1,p2,x);
  EXPECT_NEAR(p3.latitude(),  c.latitude(),  0.00000001);
  EXPECT_NEAR(p3.longitude(), c.longitude(), 0.00000001);
  EXPECT_NEAR(p3.altitude(),  c.altitude(),  0.00000001);
  EXPECT_NEAR( 0.0, p3.latitude(),  0.00000001);
  EXPECT_NEAR( 5.0, p3.longitude(), 0.00000001);
  EXPECT_NEAR(15.0, p3.altitude(),  0.00000001);

  p3 = GreatCircle::interpolate(p1,p2,-0.5);
  x = LatLonAlt::mk(p3.lat()+0.5, p3.lon(), p3.alt());
  c = GreatCircle::closest_point_circle(p1,p2,x);
  EXPECT_NEAR(p3.latitude(),  c.latitude(),  0.00000001);
  EXPECT_NEAR(p3.longitude(), c.longitude(), 0.00000001);
  EXPECT_NEAR(p3.altitude(),  c.altitude(),  0.00000001);
  EXPECT_NEAR( 0.0, p3.latitude(),  0.00000001);
  EXPECT_NEAR(-5.0, p3.longitude(), 0.00000001);
  EXPECT_NEAR( 5.0, p3.altitude(),  0.00000001);
		        
  p3 = GreatCircle::interpolate(p1,p2,1.7);
  x = LatLonAlt::mk(p3.lat()+0.5, p3.lon(), p3.alt());
  c = GreatCircle::closest_point_circle(p1,p2,x);
  EXPECT_NEAR(p3.latitude(),  c.latitude(),  0.00000001);
  EXPECT_NEAR(p3.longitude(), c.longitude(), 0.00000001);
  EXPECT_NEAR(p3.altitude(),  c.altitude(),  0.00000001);

  p3 = GreatCircle::interpolate(p1,p2,-1.7);
  x = LatLonAlt::mk(p3.lat()+0.5, p3.lon(), p3.alt());
  c = GreatCircle::closest_point_circle(p1,p2,x);
  EXPECT_NEAR(p3.latitude(),  c.latitude(),  0.00000001);
  EXPECT_NEAR(p3.longitude(), c.longitude(), 0.00000001);
  EXPECT_NEAR(p3.altitude(),  c.altitude(),  0.00000001);

		        
  p3 = LatLonAlt::make(-20.0, -90.0, 0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(  0.0, c.latitude(),  0.00000001);
  EXPECT_NEAR(-90.0, c.longitude(), 0.00000001);
  EXPECT_NEAR(0.0, c.altitude(),  0.00000001);

  p1 = LatLonAlt::make(63, 37, 10.0);
  p2 = LatLonAlt::make(-6, 12, 20.0);
  p3 = LatLonAlt::make(53, 47, 30.0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(57.5734, Units::to("deg",c.lat()),   0.001);
  EXPECT_NEAR(32.0931, Units::to("deg",c.lon()),   0.001);
  EXPECT_NEAR(30.0,    Units::to("ft",c.alt()), 0.001);

  // More special cases
		        
  p1 = LatLonAlt::make(63, 37, 10.0);
  p2 = LatLonAlt::make(-6, 12, 20.0);
  p3 = GreatCircle::interpolate(p1,p2,1.5);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(p3.lat(), c.lat(), 0.000000001);
  EXPECT_NEAR(p3.lon(), c.lon(), 0.000000001);
  EXPECT_NEAR(p3.alt(), c.alt(), 0.000000001);
		        
  p1 = LatLonAlt::make(  0.0, 20.0, 10.0);
  p2 = LatLonAlt::make(  0.0, 30.0, 10.0);
  p3 = LatLonAlt::make(-80.0,  0.0, 10.0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(  0.0, Units::to("deg",c.lat()),   0.000001);
  EXPECT_NEAR(  0.0, Units::to("deg",c.lon()),   0.000001);
  EXPECT_NEAR( 10.0, Units::to("ft", c.alt()),   0.000001);    
  EXPECT_TRUE(GreatCircle::collinear(p1, p2, c));
		        
  p3 = LatLonAlt::make(80, 0.0, 0.0);
  LatLonAlt c2 = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(c.lat(), c2.lat(), 0.000001);
  EXPECT_NEAR(c.lon(), c2.lon(), 0.000001);
  EXPECT_TRUE(GreatCircle::collinear(p1, p2, c2));
		        
  p1 = LatLonAlt::make(  0.0, 20.0, 10.0);
  p2 = LatLonAlt::make(  0.0, 30.0, 10.0);
  p3 = LatLonAlt::make(-80.0, 40.0, 10.0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(  0.0, Units::to("deg",c.lat()),   0.000001);
  EXPECT_NEAR( 40.0, Units::to("deg",c.lon()),   0.000001);
  EXPECT_NEAR( 10.0, Units::to("ft", c.alt()),   0.000001);    
  EXPECT_TRUE(GreatCircle::collinear(p1, p2, c));		        
		        
  p3 = LatLonAlt::make(80, 40.0, 0.0);
  c2 = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(c.lat(), c2.lat(), 0.000001);
  EXPECT_NEAR(c.lon(), c2.lon(), 0.000001);
  EXPECT_TRUE(GreatCircle::collinear(p1, p2, c2));

  // try other side of the world
		        
  p1 = LatLonAlt::make(  0.0,  20.0, 10.0);
  p2 = LatLonAlt::make(  0.0,  40.0, 10.0);
  p3 = LatLonAlt::make( 10.0, 250.0, 10.0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR(  0.0, Units::to("deg",c.lat()),   0.000001);
  EXPECT_NEAR(250.0, Units::to("deg",Util::to_2pi(c.lon())),   0.000001);
  EXPECT_NEAR( 10.0, Units::to("ft", c.alt()),   0.000001);    
  EXPECT_TRUE(GreatCircle::collinear(p1, p2, c));		        

  // try 
		        
  p1 = LatLonAlt::make( 85.0,  20.0, 10.0);
  p2 = LatLonAlt::make( 80.0,  20.0, 10.0);
  p3 = LatLonAlt::make( 10.0,  80.0, 10.0);
  c = GreatCircle::closest_point_circle(p1,p2,p3);
  EXPECT_NEAR( 19.4254, Units::to("deg",c.lat()),   0.000001);
  EXPECT_NEAR( 20.0, Units::to("deg",c.lon()),   0.000000001);
  EXPECT_NEAR( 10.0, Units::to("ft", c.alt()),   0.000001);    
  EXPECT_NEAR(  0.0, GreatCircle::cross_track_distance(p1, p2, c), 0.5);		        

		        
}


	
TEST_F(GreatCircleTest, testClosestPoint) {
  LatLonAlt p1 = LatLonAlt::make(0.0, 0.0, 0.0);
  LatLonAlt p2 = LatLonAlt::make(0.0, 10.0, 0.0);
  LatLonAlt p3 = GreatCircle::interpolate(p1,p2,0.7);
  LatLonAlt x = LatLonAlt::make(40.0, 7.0, 0.0);

  LatLonAlt c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p3));

  c = GreatCircle::closest_point_segment(p1,p2,x);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p3));

  p3 = GreatCircle::interpolate(p1,p2,1.7);
  c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_TRUE(c.equals(p2));

  p3 = GreatCircle::interpolate(p1,p2,-1.7);
  c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_TRUE(c.equals(p1));

  p3 = LatLonAlt::make(0.0, 9.99999999, 0);
  c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_TRUE(GreatCircle::almostEquals(c,p3));

  p3 = LatLonAlt::make(0.0, 180.0, 0);
  c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_TRUE(c.equals(p2));

  p3 = LatLonAlt::make(-20.0, -90.0, 0);
  c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_TRUE(c.equals(p1));

  p1 = LatLonAlt::make(63, 37, 0.0);
  p2 = LatLonAlt::make(-6, 12, 0.0);
  p3 = LatLonAlt::make(53, 77, 0.0);
  c = GreatCircle::closest_point_segment(p1,p2,p3);
  EXPECT_NEAR(c.lat(),1.099557,0.000001);
  EXPECT_NEAR(c.lon(),0.645772,0.000001);

  p3 = LatLonAlt::make(43, 14, 0.0);
  c = GreatCircle::closest_point_segment(p1,p2,p3);

  EXPECT_NEAR(c.lat(),0.72314,0.000001);
  EXPECT_NEAR(c.lon(),0.41325,0.000001);
  EXPECT_TRUE(GreatCircle::collinear(p1, p2, c));

}

TEST_F(GreatCircleTest, testAngleBetween) {
	LatLonAlt a = LatLonAlt::make(0.0, 0.0, 0.0);
	LatLonAlt b = LatLonAlt::make(10.0, 0.0, 0.0);
	LatLonAlt c = LatLonAlt::make(0.0, 10.0, 0.0);
	LatLonAlt d = LatLonAlt::make(10.0, 10.0, 0.0);

	double ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(10.0,Units::to("deg", ang),0.001);

	a = LatLonAlt::make(0.0, 0.0, 0.0);
	b = LatLonAlt::make(10.0, 0.0, 0.0);
	c = LatLonAlt::make(0.0, 40.0, 0.0);
	d = LatLonAlt::make(10.0, 40.0, 0.0);

	ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(40.0,Units::to("deg", ang),0.00001);

	a = LatLonAlt::make(0.0, 0.0, 0.0);
	b = LatLonAlt::make(10.0, 0.0, 0.0);
	c = LatLonAlt::make(0.0, 190.0, 0.0);
	d = LatLonAlt::make(10.0, 190.0, 0.0);

	ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(170.0,Units::to("deg", ang),0.00001);


	a = LatLonAlt::make(0.0, 0.0, 0.0);
	b = LatLonAlt::make(10.0, 0.0, 0.0);
	c = LatLonAlt::make(0.0, 40.0, 0.0);
	d = LatLonAlt::make(10.0, 40.0, 0.0);

	ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(40.0,Units::to("deg", ang),0.00001);

	a = LatLonAlt::make(0.0, 0.0, 0.0);
	b = LatLonAlt::make(0.0, 10.0, 0.0);
	c = LatLonAlt::make(10.0, 0.0, 0.0);
	d = LatLonAlt::make(10.0, 1.0, 0.0); // this works out to very slightly more than 10 degrees

	ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(10.0004,Units::to("deg", ang),0.0001);

	a = LatLonAlt::make(0.0, 0.0, 0.0);
	b = LatLonAlt::make(0.0, 10.0, 0.0);
	c = LatLonAlt::make(60.0, 0.0, 0.0);
	d = LatLonAlt::make(60.0, 10.0, 0.0); // this works out to slightly more than 60 degrees

	ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(60.0945,Units::to("deg", ang),0.0001);

	a = LatLonAlt::make(0.0, 0.0, 0.0);
	b = LatLonAlt::make(10.0, 0.0, 0.0);
	c = LatLonAlt::make(0.0, 0.0, 0.0);
	d = LatLonAlt::make(0.0, 10.0, 0.0);

	ang = GreatCircle::angleBetween(a, b, c, d);
	EXPECT_NEAR(90,Units::to("deg", ang),0.00001);

}

TEST_F(GreatCircleTest, testSphericalTransform) {
  LatLonAlt ll1 = LatLonAlt::make( 0.0, -10.0, 0.0);
  LatLonAlt ll2 = LatLonAlt::make( 0.0,   0.0, 0.0);
  LatLonAlt ll3 = LatLonAlt::make(10.0,   0.0, 0.0);
		
  Vect3 v1 = GreatCircle::spherical2xyz(ll1);
  Vect3 v2 = GreatCircle::spherical2xyz(ll2);
  Vect3 v3 = GreatCircle::spherical2xyz(ll3);
		
  LatLonAlt ll1c = GreatCircle::xyz2spherical(v1);
  LatLonAlt ll2c = GreatCircle::xyz2spherical(v2);
  LatLonAlt ll3c = GreatCircle::xyz2spherical(v3);

		// Check if 0 lat and 0 lon is the x axis
  EXPECT_NEAR(GreatCircle::spherical_earth_radius, v2.x, 0.0);
		EXPECT_NEAR(0.0, v2.y, 0.000000001);
		EXPECT_NEAR(0.0, v2.z, 0.000000001);
		
		
		// Check if transforms are inverses
		EXPECT_NEAR(ll1.lat(), ll1c.lat(), 0.0);
		EXPECT_NEAR(ll1.lon(), ll1c.lon(), 0.000000000000001);
		EXPECT_NEAR(ll3.lat(), ll3c.lat(), 0.0000000000000001);
		EXPECT_NEAR(ll3.lon(), ll3c.lon(), 0.0);

		EXPECT_TRUE(GreatCircle::almostEquals(ll1,ll1c));
		EXPECT_TRUE(ll2.equals(ll2c));
		EXPECT_TRUE(GreatCircle::almostEquals(ll3,ll3c));
		
		// check if distance is always radius of earth.
		EXPECT_NEAR(GreatCircle::spherical_earth_radius, v1.norm(), 0.0);
		EXPECT_NEAR(GreatCircle::spherical_earth_radius, v2.norm(), 0.0);
		EXPECT_NEAR(GreatCircle::spherical_earth_radius, v3.norm(), 0.0);
		
		
		//f.pln("v1 "+v1);
		//f.pln("v2 "+v2);
		//f.pln("v3 "+v3);
		
		//int dir = GreatCircle.rightOfLinePoints(ll1,ll2,ll3);
		//EXPECT_NEAR(-1, dir);
		//dir = GreatCircle.rightOfLinePoints(ll3,ll2,ll1);
		//EXPECT_NEAR(1, dir);
		
		
	}


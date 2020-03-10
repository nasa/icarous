/* 
 * LatLonAltTest
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2014-2019 United States Government as represented by 
 * the National Aeronautics and Space Administration.  No copyright 
 * is claimed in the United States under Title 17, U.S.Code. All Other 
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "LatLonAlt.h"
#include "GreatCircle.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class LatLonAltTest : public ::testing::Test {

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
	
	double ba_lat;
	double ba_lon;
	
	double per_lat;
	double per_lon;
	
	LatLonAlt lax;
	LatLonAlt jfk;
	LatLonAlt sng;
	LatLonAlt nrt;
	LatLonAlt ba;
	LatLonAlt per;
	
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
		
		ba_lat = LatLonAlt::decimal_angle( 34.0, 33.0, 33.0, false);    // Buenos Aires, Argentina
		ba_lon = LatLonAlt::decimal_angle( 58.0, 24.0, 56.0, false);
		
		per_lat = LatLonAlt::decimal_angle( 31.0, 56.0, 25.0, false);    // Perth, Australia
		per_lon = LatLonAlt::decimal_angle(115.0, 58.0, 01.0, true);
		
		lax = LatLonAlt::mk(lax_lat, lax_lon, 0.0);
		jfk = LatLonAlt::mk(jfk_lat, jfk_lon, 10.0);
		sng = LatLonAlt::mk(sng_lat, sng_lon, 0.0);
		nrt = LatLonAlt::mk(nrt_lat, nrt_lon, 0.0);	
		ba = LatLonAlt::mk(ba_lat, ba_lon, 0.0);	
		per = LatLonAlt::mk(per_lat, per_lon, 0.0);	
	}
};


TEST_F(LatLonAltTest,testDecimalAngle) {
  EXPECT_NEAR( 0.592539, lax_lat, 0.000001);
  EXPECT_NEAR(-2.066470, lax_lon, 0.000001);
  EXPECT_NEAR( 0.709186, jfk_lat, 0.000001);
  EXPECT_NEAR(-1.287762, jfk_lon, 0.000001);
}


	

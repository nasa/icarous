/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "PlanReader.h"

#include "Units.h"
#include "TrajGen.h"
#include "PlanIO.h"
#include "PlanWriter.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

using namespace larcfm;
using namespace std;

class PlanReaderTest  : public ::testing::Test {

protected:

	  istringstream* srPlan004;
	  istringstream* srPlan005;
	  //istringstream* iss3;
	  //istringstream* iss4;


	  virtual void SetUp() {
	    srPlan004 = new istringstream(
	    		"#originally T028\n"
	    			"plantype = kinematic\n"
	    			"NAME, lat, long, altitude, time, label\n"
	    			"Ownship, 5.5822, 2.9942, 6000.0, 0.0000, \n"
	    				" \" , 6.2009006225325685, 2.976927931017289, 6000.0, 684.2876, :SRC:6.2019_2.9769_6000.0000:STM:772.2479:VEL:358.4085_195.3723_0.0000:BOT:ATRK:0.0563:\n"
	    				" \" , 6.201898930308219, 2.976931259736151, 6000.0, 685.3915, :ADDED:VEL:1.9714_195.3723_0.0000:\n"
	    				" \" , 6.202895102829578, 2.9769969872158253, 5998.603563342071, 686.4954, :SRC:6.2019_2.9769_6000.0000:STM:772.2479:VEL:5.5343_195.3723_-75.9006:EOT:\n"
	    				" \" , 6.903831360797199, 3.0454083475806835, 5013.697220394467, 1465.0718, :SRC:6.9150_3.0465_5000.0000:STM:1556.9591:VEL:5.5423_195.3723_-75.9006:BOT:ATRK:0.0563:\n"
	    				" \" , 6.912706360028755, 3.0492061105540746, 5000.0, 1475.8996, :ADDED:VEL:40.4901_195.3723_-75.9006:\n"
	    				" \" , 6.91782118590052, 3.057440270526422, 5000.0, 1486.7273, :SRC:6.9150_3.0465_5000.0000:STM:1556.9591:VEL:75.4379_195.3723_0.0000:EOT:\n"
	    				" \" , 7.038891880776754, 3.5279859817643535, 5000.0, 2022.4076, \n"
	    				"\n"
	    				"Intruder, 5.642156886095542, 2.3221492067418454, 5000.0, 0.0000, \n"
	    				"\" , 6.106124485615975, 2.0215345302560066, 5000.0, 334.2555, :SRC:6.1174_2.0142_5000.0000:STM:425.3793:VEL:327.1811_356.6999_0.0000:BOT:ATRK:0.0309:\n"
	    				" \" , 6.117886500444492, 2.0158140795838535, 5000.0, 342.1868, :ADDED:VEL:341.2025_356.6999_0.0000:\n"
	    				" \" , 6.130676143512023, 2.0131299384405543, 5000.0, 350.1182, :SRC:6.1174_2.0142_5000.0000:STM:425.3793:VEL:355.2238_356.6999_0.0000:EOT:\n"
	    				" \" , 6.13232179567882, 2.0129916214327124, 5000.0, 351.1182, :SRC:6.1174_2.0142_5000.0000:STM:425.3793:VEL:355.2229_356.6999_0.0000:BGS:AGS:-2.0000:\n"
	    				" \" , 6.134742211335465, 2.012788184655762, 5000.0, 352.6010, :SRC:6.1174_2.0142_5000.0000:STM:425.3793:VEL:355.2229_356.6999_0.0000:EGS:\n"
	    				" \" , 6.7402667274470485, 1.961841662342225, 5000.0, 726.6004, :SRC:6.7632_1.8770_5000.0000:STM:777.5573:VEL:355.2177_350.9354_0.0000:BOT:ATRK:0.0314:\n"
	    				" \" , 6.7625719254697225, 1.9649052089038042, 5000.0, 740.5677, :ADDED:VEL:20.3152_350.9354_0.0000:\n"
	    				" \" , 6.781480620074488, 1.9772073692281282, 5000.0, 754.5350, :SRC:6.7632_1.8770_5000.0000:STM:777.5573:VEL:45.4127_350.9354_0.0000:EOT:\n"
	    				" \" , 6.78262111539111, 1.9783726424241155, 5000.0, 755.5350, :SRC:6.7632_1.8770_5000.0000:STM:777.5573:VEL:45.4145_350.9355_0.0000:BGS:AGS:-2.0000:\n"
	    				" \" , 6.791059828849038, 1.9869949098744073, 5000.0, 763.2652, :SRC:6.7632_1.8770_5000.0000:STM:777.5573:VEL:45.4145_350.9355_0.0000:EGS:\n"
	    				" \" , 7.136987149451118, 2.340852939580245, 5000.0, 1095.1186, :SRC:7.1547_2.3589_5000.0000:STM:1348.1989:VEL:45.4610_320.8828_0.0000:BOT:ATRK:0.0343:\n"
	    				" \" , 7.148075677363948, 2.360646742117084, 5000.0, 1110.4778, :ADDED:VEL:75.6443_320.8828_0.0000:\n"
	    				" \" , 7.147785341752288, 2.3833757409988015, 5000.0, 1125.8370, :SRC:7.1547_2.3589_5000.0000:STM:1348.1989:VEL:105.8276_320.8828_0.0000:EOT:\n"
	    				" \" , 7.1473800285799065, 2.3848161428791417, 5000.0, 1126.8370, :SRC:7.1547_2.3589_5000.0000:STM:1348.1989:VEL:105.8329_320.8829_0.0000:BGS:AGS:2.0000:\n"
	    				" \" , 7.143441306824695, 2.3988127781884137, 5000.0, 1136.0410, :SRC:7.1547_2.3589_5000.0000:STM:1348.1989:VEL:105.8329_320.8829_0.0000:EGS:\n"
	    				" \" , 6.759756886095541, 3.7543492067418454, 5000.0, 1983.4115, "
						, istringstream::in);

	    srPlan005 = new istringstream(
	    "#originally T028\n"
	    "Name, Lat, Lon, Alt, Time, type, trk, gs, vs, tcp_trk, accel_trk, tcp_gs, accel_gs, tcp_vs, accel_vs, src_lat, src_lon, src_alt, src_time, Label\n"
	    "Ownship, 5.5822, 2.9942, 6000.0000, 0.0000, Orig, -, -, -, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, 5.5822, 2.9942, 6000.0000, 0.0000, -\n"
	    "Ownship, 6.2009, 2.9769, 6000.0000, 684.2876, Orig, 358.4085, 195.3723, 0.0000, BOT, 3.2258, NONE, 0.0000, NONE, 0.0000, 6.2019, 2.9769, 6000.0000, 772.2479, -\n"
	    "Ownship, 6.2019, 2.9769, 6000.0000, 685.3915, Orig, 1.9714, 195.3723, 0.0000, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, -, -, -, -1.0000, -\n"
	    "Ownship, 6.2029, 2.9770, 5998.6036, 686.4954, Orig, 5.5343, 195.3723, -75.9006, EOT, 0.0000, NONE, 0.0000, NONE, 0.0000, 6.2019, 2.9769, 6000.0000, 772.2479, -\n"
	    "Ownship, 6.9038, 3.0454, 5013.6972, 1465.0718, Orig, 5.5423, 195.3723, -75.9006, BOT, 3.2258, NONE, 0.0000, NONE, 0.0000, 6.9150, 3.0465, 5000.0000, 1556.9591, -\n"
	    "Ownship, 6.9127, 3.0492, 5000.0000, 1475.8996, Orig, 40.4901, 195.3723, -75.9006, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, -, -, -, -1.0000, -\n"
	    "Ownship, 6.9178, 3.0574, 5000.0000, 1486.7273, Orig, 75.4379, 195.3723, 0.0000, EOT, 0.0000, NONE, 0.0000, NONE, 0.0000, 6.9150, 3.0465, 5000.0000, 1556.9591, -\n"
	    "Ownship, 7.0389, 3.5280, 5000.0000, 2022.4076, Orig, -, -, -, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, 7.0389, 3.5280, 5000.0000, 2022.4076, -\n"
	    "\n"
	    "Intruder, 5.6422, 2.3221, 5000.0000, 0.0000, Orig, -, -, -, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, 5.6422, 2.3221, 5000.0000, 0.0000, -\n"
	    "Intruder, 6.1061, 2.0215, 5000.0000, 334.2555, Orig, 327.1811, 356.6999, 0.0000, BOT, 1.7704, NONE, 0.0000, NONE, 0.0000, 6.1174, 2.0142, 5000.0000, 425.3793, -\n"
	    "Intruder, 6.1179, 2.0158, 5000.0000, 342.1868, Orig, 341.2025, 356.6999, 0.0000, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, -, -, -, -1.0000, -\n"
	    "Intruder, 6.1307, 2.0131, 5000.0000, 350.1182, Orig, 355.2238, 356.6999, 0.0000, EOT, 0.0000, NONE, 0.0000, NONE, 0.0000, 6.1174, 2.0142, 5000.0000, 425.3793, -\n"
	    "Intruder, 6.1323, 2.0130, 5000.0000, 351.1182, Orig, 355.2229, 356.6999, 0.0000, NONE, 0.0000, BGS, -2.0000, NONE, 0.0000, 6.1174, 2.0142, 5000.0000, 425.3793, -\n"
	    "Intruder, 6.1347, 2.0128, 5000.0000, 352.6010, Orig, 355.2229, 356.6999, 0.0000, NONE, 0.0000, EGS, 0.0000, NONE, 0.0000, 6.1174, 2.0142, 5000.0000, 425.3793, -\n"
	    "Intruder, 6.7403, 1.9618, 5000.0000, 726.6004, Orig, 355.2177, 350.9354, 0.0000, BOT, 1.7991, NONE, 0.0000, NONE, 0.0000, 6.7632, 1.8770, 5000.0000, 777.5573, -\n"
	    "Intruder, 6.7626, 1.9649, 5000.0000, 740.5677, Orig, 20.3152, 350.9354, 0.0000, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, -, -, -, -1.0000, -\n"
	    "Intruder, 6.7815, 1.9772, 5000.0000, 754.5350, Orig, 45.4127, 350.9354, 0.0000, EOT, 0.0000, NONE, 0.0000, NONE, 0.0000, 6.7632, 1.8770, 5000.0000, 777.5573, -\n"
	    "Intruder, 6.7826, 1.9784, 5000.0000, 755.5350, Orig, 45.4145, 350.9355, 0.0000, NONE, 0.0000, BGS, -2.0000, NONE, 0.0000, 6.7632, 1.8770, 5000.0000, 777.5573, -\n"
	    "Intruder, 6.7911, 1.9870, 5000.0000, 763.2652, Orig, 45.4145, 350.9355, 0.0000, NONE, 0.0000, EGS, 0.0000, NONE, 0.0000, 6.7632, 1.8770, 5000.0000, 777.5573, -\n"
	    "Intruder, 7.1370, 2.3409, 5000.0000, 1095.1186, Orig, 45.4610, 320.8828, 0.0000, BOT, 1.9652, NONE, 0.0000, NONE, 0.0000, 7.1547, 2.3589, 5000.0000, 1348.1989, -\n"
	    "Intruder, 7.1481, 2.3606, 5000.0000, 1110.4778, Orig, 75.6443, 320.8828, 0.0000, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, -, -, -, -1.0000, -\n"
	    "Intruder, 7.1478, 2.3834, 5000.0000, 1125.8370, Orig, 105.8276, 320.8828, 0.0000, EOT, 0.0000, NONE, 0.0000, NONE, 0.0000, 7.1547, 2.3589, 5000.0000, 1348.1989, -\n"
	    "Intruder, 7.1474, 2.3848, 5000.0000, 1126.8370, Orig, 105.8329, 320.8829, 0.0000, NONE, 0.0000, BGS, 2.0000, NONE, 0.0000, 7.1547, 2.3589, 5000.0000, 1348.1989, -\n"
	    "Intruder, 7.1434, 2.3988, 5000.0000, 1136.0410, Orig, 105.8329, 320.8829, 0.0000, NONE, 0.0000, EGS, 0.0000, NONE, 0.0000, 7.1547, 2.3589, 5000.0000, 1348.1989, -\n"
	    "Intruder, 6.7598, 3.7543, 5000.0000, 1983.4115, Orig, -, -, -, NONE, 0.0000, NONE, 0.0000, NONE, 0.0000, 6.7598, 3.7543, 5000.0000, 1983.4115, -\n"
		, istringstream::in);

	  }
	  virtual void TearDown() {
		  delete srPlan004;
		  delete srPlan005;
	  }
};

TEST_F(PlanReaderTest, testReadLinPlans) {
	PlanReader r = PlanReader();
	r.open("../../tests/Plan001.txt");
	EXPECT_EQ(2, r.size());
	Plan p1 = r.getPlan(0);
	Plan p2 = r.getPlan(1);
	EXPECT_TRUE(p1.isLatLon());
	EXPECT_EQ(5, p1.size());
	EXPECT_NEAR(0.2994, p1.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(-0.1579, p1.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(5000.0, p1.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1800.0, p1.point(3).time(), 0.0001);
	EXPECT_EQ(5, p2.size());
	EXPECT_NEAR(-0.2526, p2.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(-0.1876, p2.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(5000.0, p2.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1303.99, p2.point(3).time(), 0.0001);
	EXPECT_FALSE(p1.isTCP(2));
	EXPECT_FALSE(p2.isTCP(2));

	r = PlanReader();
	r.open("../../tests/Plan002.txt");
	EXPECT_EQ(4, r.size());
	p1 = r.getPlan(0);
	p2 = r.getPlan(1);
	Plan p3 = r.getPlan(2);
	Plan p4 = r.getPlan(3);
	EXPECT_TRUE(p1.isLatLon());

	EXPECT_EQ(4, p1.size());
	EXPECT_NEAR(0.0, p1.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(1.2498, p1.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(12094.9904, p1.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1343.2595, p1.point(3).time(), 0.0001);

	EXPECT_EQ(5, p2.size());
	EXPECT_NEAR(0.4106, p2.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(1.3085, p2.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(3800.7727, p2.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1506.4705, p2.point(3).time(), 0.0001);

	EXPECT_EQ(4, p3.size());
	EXPECT_NEAR(1.1468, p3.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(0.1166, p3.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(16389.7191, p3.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1586.6959, p3.point(3).time(), 0.0001);

	EXPECT_EQ(5, p4.size());
	EXPECT_NEAR(0.1539, p4.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(0.0983, p4.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(37432.6864, p4.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1608.8029, p4.point(3).time(), 0.0001);

	EXPECT_FALSE(p1.isTCP(2));
	EXPECT_FALSE(p2.isTCP(2));
	EXPECT_FALSE(p3.isTCP(2));
	EXPECT_FALSE(p4.isTCP(2));

	r = PlanReader();
	r.open("../../tests/Plan003.txt");
	p1 = r.getPlan(0);
	p2 = r.getPlan(1);
	EXPECT_FALSE(p1.isLatLon());

	EXPECT_EQ(2, p1.size());
	EXPECT_NEAR(31.75, p1.point(0).position().xCoordinate(), 0.0001);
	EXPECT_NEAR(-7.13, p1.point(1).position().yCoordinate(), 0.0001);
	EXPECT_NEAR(5000.0, p1.point(0).position().zCoordinate(), 0.0001);
	EXPECT_NEAR(1528.71, p1.point(1).time(), 0.0001);

	EXPECT_EQ(3, p2.size());
	EXPECT_NEAR(45.57, p2.point(0).position().xCoordinate(), 0.0001);
	EXPECT_NEAR(0.32, p2.point(1).position().yCoordinate(), 0.0001);
	EXPECT_NEAR(5000.0, p2.point(2).position().zCoordinate(), 0.0001);
	EXPECT_NEAR(915.90, p2.point(0).time(), 0.0001);
	if (p1.size() > 2) {
		EXPECT_FALSE(p1.isTCP(2));
		EXPECT_FALSE(p2.isTCP(2));
	}
	r = PlanReader();
	r.open("../../tests/Plan006.txt");
	p1 = r.getPlan(0);
	p2 = r.getPlan(1);
	EXPECT_TRUE(p1.isLatLon());
	EXPECT_EQ(5, p1.size());
	EXPECT_NEAR(0.2994, p1.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(-0.1579, p1.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(5000.0, p1.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1800.0, p1.point(3).time(), 0.0001);
	EXPECT_EQ(5, p2.size());
	EXPECT_NEAR(-0.2526, p2.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(-0.1876, p2.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(5000.0, p2.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(1303.99, p2.point(3).time(), 0.0001);
	EXPECT_FALSE(p1.isTCP(2));
	EXPECT_FALSE(p2.isTCP(2));

}


TEST_F(PlanReaderTest, testReadKinPlans) {
	PlanReader r = PlanReader();
	r.open(srPlan004);
	EXPECT_EQ(2, r.size());
	Plan p1 = r.getPlan(0);
	Plan p2 = r.getPlan(1);
	EXPECT_TRUE(p1.isLatLon());

	EXPECT_EQ(8, p1.size());
	EXPECT_NEAR(5.5822, p1.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(2.9769, p1.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(6000.0, p1.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(686.4954, p1.point(3).time(), 0.0001);
//	EXPECT_EQ("",p1.point(3).name());  //TODO ???

	EXPECT_EQ(17, p2.size());
	EXPECT_NEAR(5.6422, p2.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(2.0215, p2.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(5000.0, p2.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(350.1182, p2.point(3).time(), 0.0001);

	EXPECT_TRUE(p1.isBOT(4));
	//EXPECT_TRUE(p1.point(5).isAdded());
	//EXPECT_NEAR(1556.9591, p1.getTcpData(6).getSourceTime(), 0.0001);
	//EXPECT_NEAR(6.2019, p1.getTcpData(1).getSourcePosition().latitude(), 0.0001);
	//EXPECT_NEAR(2.9769, p1.getTcpData(1).getSourcePosition().longitude(), 0.0001);
	//EXPECT_NEAR(6000.0, p1.getTcpData(1).getSourcePosition().altitude(), 0.0001);
	//EXPECT_NEAR(0.0563, p1.trkAccel(4), 0.0001);
	EXPECT_NEAR(5.5343, p1.initialVelocity(3).track("deg"), 0.0001);
	EXPECT_NEAR(195.3723, p1.initialVelocity(3).groundSpeed("kts"), 0.0001);
	EXPECT_NEAR(-75.9006, p1.initialVelocity(3).verticalSpeed("fpm"), 0.0001);

	EXPECT_TRUE(p2.isBOT(1));
	EXPECT_TRUE(p2.isEOT(3));
	EXPECT_TRUE(p2.isBGS(4));
	EXPECT_TRUE(p2.isEGS(5));

	EXPECT_TRUE(p1.isWellFormed());
	EXPECT_TRUE(p2.isWellFormed());

	r = PlanReader();
	r.open(srPlan005);
	EXPECT_EQ(2, r.size());
	p1 = r.getPlan(0);
	p2 = r.getPlan(1);
	EXPECT_TRUE(p1.isLatLon());

	EXPECT_EQ(8, p1.size());
	EXPECT_NEAR(5.5822, p1.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(2.9769, p1.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(6000.0, p1.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(686.4954, p1.point(3).time(), 0.0001);

	EXPECT_EQ(17, p2.size());
	EXPECT_NEAR(5.6422, p2.point(0).position().latitude(), 0.0001);
	EXPECT_NEAR(2.0215, p2.point(1).position().longitude(), 0.0001);
	EXPECT_NEAR(5000.0, p2.point(2).position().altitude(), 0.0001);
	EXPECT_NEAR(350.1182, p2.point(3).time(), 0.0001);


	EXPECT_TRUE(p1.isBOT(4));
	//EXPECT_TRUE(p1.point(5).isAdded());
	//EXPECT_NEAR(1556.9591, p1.getTcpData(6).getSourceTime(), 0.0001);
	//EXPECT_NEAR(6.2019, p1.getTcpData(1).getSourcePosition().latitude(), 0.0001);
	//EXPECT_NEAR(2.9769, p1.getTcpData(1).getSourcePosition().longitude(), 0.0001);
	//EXPECT_NEAR(6000.0, p1.getTcpData(1).getSourcePosition().altitude(), 0.0001);
	//EXPECT_NEAR(Units::from("deg/s",3.2258), p1.trkAccel(4), 0.0001);
	//EXPECT_NEAR(5.5343, p1.initialVelocity(3).track("deg"), 0.0001);
	//EXPECT_NEAR(195.3723, p1.initialVelocity(3).groundSpeed("kts"), 0.0001);
	//EXPECT_NEAR(-75.9006, p1.initialVelocity(3).verticalSpeed("fpm"), 0.0001);

	EXPECT_TRUE(p2.isBOT(1));
	EXPECT_TRUE(p2.isEOT(3));
	EXPECT_TRUE(p2.isBGS(4));
	EXPECT_TRUE(p2.isEGS(5));

	EXPECT_TRUE(p1.isWellFormed());
	EXPECT_TRUE(p2.isWellFormed());
	}



TEST_F(PlanReaderTest, test_rdr) {
	Plan lpc = Plan("foo");
	Position p0  = Position::makeLatLonAlt(20.0, 10.0, 0.0);
	Position p3  = Position::makeLatLonAlt(30.0, 20.0, 0.0);
	Position vertex = Position::makeLatLonAlt(30.0, 10.0, 0.0);
    lpc.addNavPoint(NavPoint(p0,10000.0));
    lpc.addNavPoint(NavPoint(vertex,20000.0));
    lpc.addNavPoint(NavPoint(p3,30000.0));

    double bankAngle = Units::from("deg",5);
    Plan kpc = TrajGen::generateTurnTCPs(lpc,bankAngle);
    std::string fName = "test_rdr.txt";
    PlanIO::savePlan(kpc, fName);
	PlanReader pr = PlanReader();
//	std::istringstream osr(osw.str());
	pr.open(fName);
    //fpln(" $$$$ pr message: "+pr.getMessageNoClear());
	EXPECT_FALSE(pr.hasMessage());
    Plan p = pr.getPlan(0);
    //fpln(" $$ test_rdr: p = "+p.toStringFull());
    EXPECT_TRUE(p.almostEquals(kpc));
}






/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework...

//#include "UnitSymbols.h"
#include "Velocity.h"
#include "CDII.h"
#include "CDSI.h"
#include "CDCylinder.h"
#include "GreatCircle.h"

#include <cmath>
#include "gtest/gtest.h"

using namespace larcfm;

class CDSITest : public testing::Test {

public: 
  Vect3 so;
  Vect3 vo;
  CDSI cdsi;
  CDCylinder* cd;
  Plan fp;
  
protected:
  virtual void SetUp() {
    vo = Velocity::mkTrkGsVs(Units::from("deg", 90.0), Units::from("knot", 500.0), Units::from("fpm", 0.0));
    cd = new CDCylinder();
    cdsi = CDSI::make(cd);
    cd = (CDCylinder*)cdsi.getCoreDetectionPtr(); // link to actual cd
    //fp = Plan();
  }
};

  
TEST_F(CDSITest, testSetup) {
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(0, cdsi.size());
  EXPECT_NEAR(5.0, cd->getHorizontalSeparation("nm"), 0.00000001);
  EXPECT_NEAR(1000.0, cd->getVerticalSeparation("ft"), 0.00000001);
  EXPECT_EQ(0, fp.size());

    
  cd->setHorizontalSeparation(2.2,"nm");
  cd->setVerticalSeparation(4.4,"ft");
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(0, cdsi.size());
  EXPECT_NEAR(2.2, cd->getHorizontalSeparation("nm"), 0.00000001);
  EXPECT_NEAR(4.4, cd->getVerticalSeparation("ft"), 0.00000001);
  EXPECT_FALSE(cdsi.hasMessage());    
}
  
  
TEST_F(CDSITest, testDetectionXYZ_straight) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0,  50.0, 5000.0, 7200.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3240.0, cdsi.getTimeIn(0),  0.00001);
  EXPECT_NEAR(3960.0, cdsi.getTimeOut(0), 0.00001);
  EXPECT_NEAR(3600.0, cdsi.getCriticalTime(0), 0.0001);
  EXPECT_NEAR(   0.0, cdsi.getDistanceAtCriticalTime(0), 0.0001);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));
}

  
TEST_F(CDSITest, testDetectionXYZ_noConflict) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0, -50.0, 5000.0, 7200.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(0, cdsi.size());
}

  
TEST_F(CDSITest, testDetectionXYZ_extend1) {
  fp.addNavPoint(NavPoint::makeXYZ(  0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ(250.0, -25.0, 5000.0, 1800.0));
  fp.addNavPoint(NavPoint::makeXYZ(555.555555555, 5.5555555555555, 5000.0, 4000.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3240.0, cdsi.getTimeIn(0),  0.00001);
  EXPECT_NEAR(3960.0, cdsi.getTimeOut(0), 0.00001);
  EXPECT_EQ( 1, cdsi.getSegmentIn(0));
  EXPECT_EQ( 1, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_intent1) { 
  fp.addNavPoint(NavPoint::makeXYZ(   0.0,   -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0,     0.0, 5000.0, 3600.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0, -1000.0, 5000.0, 7200.0));
  fp.addNavPoint(NavPoint::makeXYZ(1500.0,     0.0, 5000.0,10800.0));
  fp.addNavPoint(NavPoint::makeXYZ(2000.0,   -50.0, 5000.0,14400.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3240.0, cdsi.getTimeIn(0),  0.00001);
  EXPECT_NEAR(3618.0, cdsi.getTimeOut(0), 0.00001);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 1, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}

  
TEST_F(CDSITest, testDetectionXYZ_intent2) {
  vo = Velocity::makeTrkGsVs(23.20, "deg", 496.68, "kn",  0.00, "fpm");
  fp.addNavPoint(NavPoint::makeXYZ(  0.0,     5.000,   0.0,     2.0));
  fp.addNavPoint(NavPoint::makeXYZ( 15.0,    10.000,   0.0,   115.2));
  fp.addNavPoint(NavPoint::makeXYZ( 30.0,     5.000,   0.0,   230.4));
  cdsi.detectionXYZ(7.0, 0.0, 0.0, Units::to("kn", vo.x), Units::to("kn", vo.y), Units::to("fpm", vo.z), 0.0, 55.2, fp, 0.0, 55.2);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(34.61179, cdsi.getTimeIn(0),  0.00001);
  EXPECT_NEAR(55.2, cdsi.getTimeOut(0), 0.00001);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
  
TEST_F(CDSITest, testDetectionXYZ_multipleConflicts) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0,   0.0, 5000.0, 3600.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0, -50.0, 5000.0, 7200.0));
  fp.addNavPoint(NavPoint::makeXYZ(1500.0,   0.0, 5000.0,10800.0));
  fp.addNavPoint(NavPoint::makeXYZ(2000.0, -50.0, 5000.0,14400.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 14400.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(2, cdsi.size());
  EXPECT_NEAR(3240.0, cdsi.getTimeIn(0),  0.00001);
  EXPECT_NEAR(3960.0, cdsi.getTimeOut(0), 0.00001);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 1, cdsi.getSegmentOut(0));

  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_NEAR(10440.0, cdsi.getTimeIn(1),  0.00001);
  EXPECT_NEAR(11160.0, cdsi.getTimeOut(1), 0.00001);
  EXPECT_EQ( 2, cdsi.getSegmentIn(1));
  EXPECT_EQ( 3, cdsi.getSegmentOut(1));
  EXPECT_FALSE(cdsi.hasMessage());
}

  
TEST_F(CDSITest, testDetectionXYZ_intent3) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0,   0.0, 5000.0, 3600.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0, -50.0, 5000.0, 7200.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3240.0, cdsi.getTimeIn(0),  0.00001);
  EXPECT_NEAR(3960.0, cdsi.getTimeOut(0), 0.00001);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 1, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_intent4) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0,   1.0, 5000.0, 3600.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0,   1.0, 5000.0, 7200.0));
  fp.addNavPoint(NavPoint::makeXYZ(1500.0, -50.0, 5000.0,10800.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3176.5, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(7623.5, cdsi.getTimeOut(0), 0.1);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 2, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_tca) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0,  -2.0, 5000.0, 3600.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0,  -1.0, 5000.0, 7175.0));
  fp.addNavPoint(NavPoint::makeXYZ(1500.0, -50.0, 5000.0,10800.0));
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3375.0, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(7380.5, cdsi.getTimeOut(0), 0.1);
  EXPECT_NEAR(4147.6, cdsi.getCriticalTime(0), 0.1); 
  EXPECT_NEAR(0.14775, cdsi.getDistanceAtCriticalTime(0), 0.0001); 
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 2, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_nonZerot0) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,   10.0));
  fp.addNavPoint(NavPoint::makeXYZ(1000.0,  50.0, 5000.0, 7210.0));
    
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 1.0, 10.0e+300, fp, 0.0, 3600.0);    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3261.4, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(3958.6, cdsi.getTimeOut(0), 0.1);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));

  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 10.0, 10.0e+300, fp, 0.0, 3600.0);    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3250.0, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(3970.0, cdsi.getTimeOut(0), 0.1);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));

  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 20.0, 10.0e+300, fp, 0.0, 3600.0);    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3264.2, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(3955.8, cdsi.getTimeOut(0), 0.1);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_zeroTimeConflict) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0,  -5.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0, 3600.0));
    
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(0, cdsi.size());
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_filter1) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ(   1.0,  -1.0, 5000.0,    1.0));
  fp.addNavPoint(NavPoint::makeXYZ(   2.0, -50.0, 5000.0,    2.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0, -50.0, 5000.0, 3600.0));
    
  cdsi.setFilterTime(10.0);
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(0, cdsi.size());
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionXYZ_filter2) {
  fp.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeXYZ(   1.0,  -1.0, 5000.0,    1.0));
  fp.addNavPoint(NavPoint::makeXYZ(   2.0,  -1.0, 5000.0,    2.1));
  fp.addNavPoint(NavPoint::makeXYZ(   3.0, -50.0, 5000.0,    4.0));
  fp.addNavPoint(NavPoint::makeXYZ( 500.0, -50.0, 5000.0, 3600.0));
    
  cdsi.setFilterTime(1.0);
  cdsi.detectionXYZ(0.0, 0.0, 5000.0, 500.0, 0.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(1.0, cdsi.getTimeIn(0),  0.0001); // note error, conflict really started at ~0.9
  EXPECT_NEAR(2.1, cdsi.getTimeOut(0), 0.0001); // note error, conflict really ended at ~2.2
  EXPECT_EQ( 1, cdsi.getSegmentIn(0));
  EXPECT_EQ( 1, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}
  
  
TEST_F(CDSITest, testDetectionLL_straight) {
  fp.addNavPoint(NavPoint::makeLatLonAlt(-1.0,  0.0, 5000.0,    0.0));
  fp.addNavPoint(NavPoint::makeLatLonAlt( 1.0, GreatCircle::angle_from_distance(Units::from("nmi", 1000.0),0.0)*180/Pi, 5000.0, 7200.0));
  cdsi.detectionLL(0.0, 0.0, 5000.0, 90.0, 500.0, 0.0, 0.0, 10.0e+300, fp, 0.0, 3600.0);
    
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(3323.6, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(3928.0, cdsi.getTimeOut(0), 0.1);
  EXPECT_NEAR(3625.8, cdsi.getCriticalTime(0), 0.1);
  EXPECT_NEAR(   0.0, cdsi.getDistanceAtCriticalTime(0), 0.0001);
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));
  EXPECT_FALSE(cdsi.hasMessage());
}

  
TEST_F(CDSITest, testDetectionLL_intent1) {
  //System.out.println("LLIntent1");
    
  double t0 = 842.1083;
  double t_start = 857.7597;
  fp.addNavPoint(NavPoint::makeLatLonAlt(-62.3980, -177.5512, 23390.8643, t_start));
  fp.addNavPoint(NavPoint::makeLatLonAlt(-63.9044, -178.5724, 35651.1322, 1849.5564));
  fp.addNavPoint(NavPoint::makeLatLonAlt(-64.4493, -179.3629, 39158.9147, 2207.1337));
    
  Velocity vo = Velocity::mkVxyz(-72.347576, -122.830568, 4.834492);

  double time_cpa = 1346.9;
    
  LatLonAlt p0 = LatLonAlt::make(-62.62, -177.48, 20750.0);
  LatLonAlt p1 = GreatCircle::linear_initial(p0, vo, time_cpa - t0);
  LatLonAlt p2 = GreatCircle::linear_initial(fp.position(t_start).lla(), fp.velocity(t_start), time_cpa - t_start);

  cdsi.detectionLL(p0.latitude(), p0.longitude(), p0.altitude(), 
		   vo.track("deg"), vo.groundSpeed("knot"), vo.verticalSpeed("fpm"),
		   t0, 871.6858, fp, 0.0, 871.68580);
  EXPECT_FALSE(cdsi.hasMessage());
  EXPECT_EQ(1, cdsi.size());
  EXPECT_NEAR(  1321.9, cdsi.getTimeIn(0),  0.1);
  EXPECT_NEAR(  1369.6, cdsi.getTimeOut(0), 0.1);
  EXPECT_NEAR(  1345.8, cdsi.getCriticalTime(0), 0.1);
  EXPECT_NEAR(  0.9824, cdsi.getDistanceAtCriticalTime(0), 0.0001);
  EXPECT_TRUE(Units::from("nmi", 5.0) > GreatCircle::distance(p1, p2));
  EXPECT_EQ( 0, cdsi.getSegmentIn(0));
  EXPECT_EQ( 0, cdsi.getSegmentOut(0));
    
  p0 = LatLonAlt::make(-62.6184, -177.4846, 20750.2199);
  p1 = GreatCircle::linear_initial(p0, vo, time_cpa - t0);
  p2 = GreatCircle::linear_initial(fp.position(t_start).lla(), fp.velocity(t_start), time_cpa - t_start);
    
  cdsi.detectionLL(p0.latitude(), p0.longitude(), p0.altitude(), 
		   vo.track("deg"), vo.groundSpeed("knot"), vo.verticalSpeed("fpm"),
		   t0, 871.6858, fp, 0.0, 871.68580);
  EXPECT_EQ(0, cdsi.size());
  EXPECT_TRUE(Units::from("nmi", 5.0) < GreatCircle::distance(p1, p2));
  EXPECT_FALSE(cdsi.hasMessage());
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

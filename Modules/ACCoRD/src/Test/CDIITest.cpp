/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "CDII.h"
#include "Units.h"
#include "PlanUtil.h"
#include "LatLonAlt.h"
#include "Position.h"
#include "NavPoint.h"
#include "format.h"

#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class CDIITest : public ::testing::Test {

public:
  CDII cdii;
  Plan ownship;
  Plan traffic1;

protected:
  virtual void SetUp() {
    cdii = CDII::make(5.0, "nm", 1000.0, "ft");
  }
};

TEST_F(CDIITest, testSetup) {
  EXPECT_EQ(0, ownship.size());
  EXPECT_EQ(0, traffic1.size());
  EXPECT_EQ(0, cdii.size());
  EXPECT_NEAR(5.0, cdii.getDistance("NM"), 0.00000001);
  EXPECT_NEAR(1000.0, cdii.getHeight("ft"), 0.00000001);
  EXPECT_FALSE(cdii.hasMessage());

  cdii.setDistance(2.2,"NM");
  cdii.setHeight(4.4,"ft");
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(0, cdii.size());
  EXPECT_NEAR(2.2, cdii.getDistance("NM"), 0.00000001);
  EXPECT_NEAR(4.4, cdii.getHeight("ft"), 0.00000001);
  EXPECT_FALSE(cdii.hasMessage());    
}

TEST_F(CDIITest, testNoConflict) {
  ownship.addNavPoint(NavPoint::makeXYZ(  0.0,     5.000,   0.0,     2.0));
  ownship.addNavPoint(NavPoint::makeXYZ( 15.0,    10.000,   0.0,   300.2));

  traffic1.addNavPoint(NavPoint::makeXYZ( 5.1,     -5.0,     0.0,     0.0));
  traffic1.addNavPoint(NavPoint::makeXYZ(10.0,    -10.0,     0.0,   300.2));

  cdii.detection(ownship, traffic1, 0.0, 1400.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(0, cdii.size());
}

TEST_F(CDIITest, testDetection1) {
  ownship.addNavPoint(NavPoint::makeXYZ(  0.0,     5.000,   0.0,     2.0));
  ownship.addNavPoint(NavPoint::makeXYZ( 15.0,    10.000,   0.0,   115.2));
  ownship.addNavPoint(NavPoint::makeXYZ( 30.0,     5.000,   0.0,   230.4));

  traffic1.addNavPoint(NavPoint::makeXYZ( 7.0,     0.0,     0.0,     0.0));
  traffic1.addNavPoint(NavPoint::makeXYZ(10.0,     7.0,     0.0,    55.2));
  traffic1.addNavPoint(NavPoint::makeXYZ(17.0,     8.0,     0.0,   106.8));
  traffic1.addNavPoint(NavPoint::makeXYZ(30.0,   -11.0,     0.0,  1386.0));

  cdii.detection(ownship, traffic1, 0.0, 1400.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(34.6106, cdii.getTimeIn(0), 0.0001);
  EXPECT_NEAR(174.0574, cdii.getTimeOut(0), 0.0001);
  EXPECT_NEAR(135.6256, cdii.getCriticalTime(0), 0.0001);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 1, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection2) {
  ownship.addNavPoint(NavPoint::makeXYZ(  0.0,     5.000,   0.0,     2.0));
  ownship.addNavPoint(NavPoint::makeXYZ( 15.0,    10.000,   0.0,   115.2));
  ownship.addNavPoint(NavPoint::makeXYZ( 30.0,     5.000,   0.0,   230.4));

  traffic1.addNavPoint(NavPoint::makeXYZ( 7.0,     0.0,     0.0,     0.0));
  traffic1.addNavPoint(NavPoint::makeXYZ(10.0,     7.0,     0.0,    55.2));
  traffic1.addNavPoint(NavPoint::makeXYZ(17.0,     8.0,     0.0,   106.8));
  traffic1.addNavPoint(NavPoint::makeXYZ(30.0,   -11.0,     0.0,  1386.0));

  cdii.detection(traffic1, ownship, 0.0, 1400.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 34.6106, cdii.getTimeIn(0), 0.0001);
  EXPECT_NEAR(174.0574, cdii.getTimeOut(0), 0.0001);
  EXPECT_NEAR(135.6256, cdii.getCriticalTime(0), 0.0001);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 2, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection3) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.00, -50.00, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ( 500.00, -25.00, 5000.0000, 3600.00));
  ownship.addNavPoint(NavPoint::makeXYZ( 600.00, -10.00, 5000.0000, 3600.00+720.0));
  ownship.addNavPoint(NavPoint::makeXYZ( 750.00,  -1.00, 5000.0000, 5400.00));
  ownship.addNavPoint(NavPoint::makeXYZ(1000.00, -50.00, 5000.0000, 7200.00));

  traffic1.addNavPoint(NavPoint::makeXYZ(   0.00,  50.00, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ( 500.00,  25.00, 5000.0000, 3600.00));
  traffic1.addNavPoint(NavPoint::makeXYZ( 600.00,  10.00, 5000.0000, 3600.00+720.0));
  traffic1.addNavPoint(NavPoint::makeXYZ( 750.00,   1.00, 5000.0000, 5400.00));
  traffic1.addNavPoint(NavPoint::makeXYZ(1000.00,  50.00, 5000.0000, 7200.00));

  cdii.detection(ownship, traffic1, 0.0, 7200.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 5220.0, cdii.getTimeIn(0), 0.1);
  EXPECT_NEAR( 5455.1, cdii.getTimeOut(0), 0.1);
  EXPECT_NEAR( 5400.0, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR( 0.16, cdii.getDistanceAtCriticalTime(0), 0.01);
  EXPECT_EQ( 2, cdii.getSegmentIn(0));
  EXPECT_EQ( 3, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection_OneLeg) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.00,   0.00, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ(1000.00,   0.00, 5000.0000, 7200.00));

  traffic1.addNavPoint(NavPoint::makeXYZ(   0.00,  50.00, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ( 750.00,   1.00, 5000.0000, 5400.00));

  cdii.detection(ownship, traffic1, 0.0, 7200.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 4959.2, cdii.getTimeIn(0), 0.1);
  EXPECT_NEAR( 5400.0, cdii.getTimeOut(0), 0.1);
  EXPECT_NEAR( 5400.0, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR( 0.04, cdii.getDistanceAtCriticalTime(0), 0.01);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 0, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection_straight) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.00, -50.00, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ(1000.00,  50.00, 5000.0000, 7200.00));

  traffic1.addNavPoint(NavPoint::makeXYZ(   0.00,   0.00, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ(1000.00,   0.00, 5000.0000, 7200.00));

  cdii.detection(ownship, traffic1, 0.0, 7200.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 3240.0, cdii.getTimeIn(0), 0.0001);
  EXPECT_NEAR( 3960.0, cdii.getTimeOut(0), 0.0001);
  EXPECT_NEAR( 3600.0, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR( 0.0, cdii.getDistanceAtCriticalTime(0), 0.00001);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 0, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection_straight_cutoff) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.00, -50.00, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ(1000.00,  50.00, 5000.0000, 7200.00));

  traffic1.addNavPoint(NavPoint::makeXYZ(   0.00,   0.00, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ(1000.00,   0.00, 5000.0000, 7200.00));

  cdii.detection(ownship, traffic1, 0.0, 3500.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 3240.0, cdii.getTimeIn(0), 0.0001);
  EXPECT_NEAR( 3960.0, cdii.getTimeOut(0), 0.0001);
  EXPECT_NEAR( 3600.0, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR( 0.0, cdii.getDistanceAtCriticalTime(0), 0.00001);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 0, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection_cutoff) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.0500, -50.7400, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ( 500.7200,   0.3400, 5000.0000, 3550.3900));
  ownship.addNavPoint(NavPoint::makeXYZ( 550.4347,   0.9253, 5000.0000, 4000.2300));
  ownship.addNavPoint(NavPoint::makeXYZ( 600.4133, -50.0134, 5000.0000, 4500.3632));

  traffic1.addNavPoint(NavPoint::makeXYZ(  0.9300, 0.6300, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ( 500.080, 0.8900, 5000.0000, 3550.3300));
  traffic1.addNavPoint(NavPoint::makeXYZ(550.3400, 0.2700, 5000.0000, 4000.5800));
  traffic1.addNavPoint(NavPoint::makeXYZ(600.4600, 0.0000, 5000.0000, 4500.2700));
  traffic1.addNavPoint(NavPoint::makeXYZ(655.2900, 0.7300, 5000.0000, 5000.2300));

  cdii.detection(ownship, traffic1, 0.0, 3600.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 3241.2547, cdii.getTimeIn(0), 0.0001);
  EXPECT_NEAR( 4056.0382, cdii.getTimeOut(0), 0.0001);
  EXPECT_NEAR( 4006.7010, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR(    0.00065, cdii.getDistanceAtCriticalTime(0), 0.00001);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 2, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection_cutoff2) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.0500, -50.7400, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ( 500.7200,   0.3400, 5000.0000, 3550.3900));
  ownship.addNavPoint(NavPoint::makeXYZ( 550.4347,   0.9253, 5000.0000, 4000.2300));
  ownship.addNavPoint(NavPoint::makeXYZ( 600.4133, -50.0134, 5000.0000, 4500.3632));

  traffic1.addNavPoint(NavPoint::makeXYZ(  0.9300, 0.6300, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ( 500.080, 0.8900, 5000.0000, 3550.3300));
  traffic1.addNavPoint(NavPoint::makeXYZ(550.3400, 0.2700, 5000.0000, 4000.5800));
  traffic1.addNavPoint(NavPoint::makeXYZ(600.4600, 0.0000, 5000.0000, 4500.2700));
  traffic1.addNavPoint(NavPoint::makeXYZ(655.2900, 0.7300, 5000.0000, 5000.2300));

  cdii.detection(traffic1, ownship, 0.0, 3600.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR( 3241.2547, cdii.getTimeIn(0), 0.0001);
  EXPECT_NEAR( 4056.0382, cdii.getTimeOut(0), 0.0001);
  EXPECT_NEAR( 4006.7010, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR(    0.00065, cdii.getDistanceAtCriticalTime(0), 0.00001);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 2, cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetection_multiple) {
  ownship.addNavPoint(NavPoint::makeXYZ(   0.00, -50.00, 5000.0000, 0.0000));
  ownship.addNavPoint(NavPoint::makeXYZ( 500.00,  -1.00, 5000.0000, 3600.00));
  ownship.addNavPoint(NavPoint::makeXYZ( 600.00, -10.00, 5000.0000, 3600.00+720.0));
  ownship.addNavPoint(NavPoint::makeXYZ( 750.00,  -1.00, 5000.0000, 5400.00));
  ownship.addNavPoint(NavPoint::makeXYZ(1000.00, -50.00, 5000.0000, 7200.00));

  traffic1.addNavPoint(NavPoint::makeXYZ(   0.00,  50.00, 5000.0000, 0.0000));
  traffic1.addNavPoint(NavPoint::makeXYZ( 500.00,   1.00, 5000.0000, 3600.00));
  traffic1.addNavPoint(NavPoint::makeXYZ( 600.00,  10.00, 5000.0000, 3600.00+720.0));
  traffic1.addNavPoint(NavPoint::makeXYZ( 750.00,   1.00, 5000.0000, 5400.00));
  traffic1.addNavPoint(NavPoint::makeXYZ(1000.00,  50.00, 5000.0000, 7200.00));

  cdii.detection(ownship, traffic1, 0.0, 7200.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(2, cdii.size());
  EXPECT_NEAR( 3489.8, cdii.getTimeIn(0), 0.1);
  EXPECT_NEAR( 3720.0, cdii.getTimeOut(0), 0.1);
  EXPECT_NEAR( 3600.0, cdii.getCriticalTime(0), 0.0001);
  EXPECT_NEAR( 0.16, cdii.getDistanceAtCriticalTime(0), 0.01);
  EXPECT_EQ( 0, cdii.getSegmentIn(0));
  EXPECT_EQ( 1, cdii.getSegmentOut(0));

  EXPECT_NEAR( 5220.0, cdii.getTimeIn(1), 0.1);
  EXPECT_NEAR( 5455.1, cdii.getTimeOut(1), 0.1);
  EXPECT_NEAR( 5400.0, cdii.getCriticalTime(1), 0.0001);
  EXPECT_NEAR( 0.16, cdii.getDistanceAtCriticalTime(1), 0.01);
  EXPECT_EQ( 2, cdii.getSegmentIn(1));
  EXPECT_EQ( 3, cdii.getSegmentOut(1));
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetectionXYZ_start_time1) {
  double end_time = 3960.0;

  ownship.addNavPoint(NavPoint::makeXYZ(   0.0,    0.000, 5000.0,     0.0));
  ownship.addNavPoint(NavPoint::makeXYZ(1000.0,    0.000, 5000.0,  7200.0));

  traffic1.addNavPoint(NavPoint::makeXYZ(   0.0, -50.0, 5000.0,    0.0));
  traffic1.addNavPoint(NavPoint::makeXYZ(1000.0,  50.0, 5000.0, 7200.0));

  cdii.detection(ownship, traffic1, 0.0, 7200.0);

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(3240.0, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  cdii.detection(ownship, traffic1, end_time, 7200.0); 

  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_EQ(0, cdii.size());
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetectionXYZ_start_time2) {
  double start_time = 651.7060392816564;
  double end_time =   656.046674802993;
  bool r;

  ownship.addNavPoint(NavPoint::makeLatLonAlt( -0.8453, -0.0050, 290.8207, 619.4765));
  ownship.addNavPoint(NavPoint::makeLatLonAlt( -1.5769, -0.0606, 290.8207, 968.5701));
  ownship.addNavPoint(NavPoint::makeLatLonAlt( -2.1803, -0.7279, 15853.1723, 1400.2762));

  traffic1.addNavPoint(NavPoint::makeLatLonAlt( -1.2293, 0.4797, 1951.3321, 254.4634));
  traffic1.addNavPoint(NavPoint::makeLatLonAlt( -1.0062, -0.0509, 1137.6576, 631.3467));
  traffic1.addNavPoint(NavPoint::makeLatLonAlt( -0.1481, -0.4877, 4401.6286, 1157.7137));

  r = cdii.detection(ownship, traffic1, 0.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 0.0, 100.0);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());

  r = cdii.detection(ownship, traffic1, 10.0, 100.0);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());

  r = cdii.detection(ownship, traffic1, 100.0, 600.0);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());

  r = cdii.detection(ownship, traffic1, 10.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 100.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 300.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 600.0, end_time);
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 620.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 640.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00002);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 652.0, end_time); 
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(652.0, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);  

  r = cdii.detection(ownship, traffic1, end_time, 900.0); 
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testDetectionXYZ_start_time2_reverse) {
  double start_time = 651.7060693126186;
  double end_time =   656.046674802993;
  bool r;

  traffic1.addNavPoint(NavPoint::makeLatLonAlt( -0.8453, -0.0050, 290.8207, 619.4765));
  traffic1.addNavPoint(NavPoint::makeLatLonAlt( -1.5769, -0.0606, 290.8207, 968.5701));
  traffic1.addNavPoint(NavPoint::makeLatLonAlt( -2.1803, -0.7279, 15853.1723, 1400.2762));

  ownship.addNavPoint(NavPoint::makeLatLonAlt( -1.2293, 0.4797, 1951.3321, 254.4634));
  ownship.addNavPoint(NavPoint::makeLatLonAlt( -1.0062, -0.0509, 1137.6576, 631.3467));
  ownship.addNavPoint(NavPoint::makeLatLonAlt( -0.1481, -0.4877, 4401.6286, 1157.7137));

  r = cdii.detection(ownship, traffic1, 0.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 0.0, 100.0);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());

  r = cdii.detection(ownship, traffic1, 10.0, 100.0);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());

  r = cdii.detection(ownship, traffic1, 100.0, 600.0);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());

  r = cdii.detection(ownship, traffic1, 10.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 100.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 300.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 600.0, end_time);
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 620.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 640.0, end_time);    
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(start_time, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);

  r = cdii.detection(ownship, traffic1, 652.0, end_time); 
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_TRUE(r);
  EXPECT_EQ(1, cdii.size());
  EXPECT_NEAR(652.0, cdii.getTimeIn(0),  0.00001);
  EXPECT_NEAR(end_time, cdii.getTimeOut(0), 0.00001);  

  r = cdii.detection(ownship, traffic1, end_time, 900.0); 
  EXPECT_FALSE(cdii.hasMessage());
  EXPECT_FALSE(r);
  EXPECT_EQ(0, cdii.size());
  EXPECT_FALSE(cdii.hasMessage());
}

TEST_F(CDIITest, testInLoss) {
  Plan traffic;
  Plan traffic2;

  Position p0  = Position(LatLonAlt::make(6.987988, -4.512625, 10000.000000));
  Position p1  = Position(LatLonAlt::make(-5.210948, 4.022839, 10000.000000));
  NavPoint np0 = NavPoint(p0,0.000000);     ownship.addNavPoint(np0);
  NavPoint np1 = NavPoint(p1,8449.806400);     ownship.addNavPoint(np1);
  p0  = Position(LatLonAlt::make(7.138859, -4.269949, 10000.000000));
  p1  = Position(LatLonAlt::make(-13.295224, 5.741917, 10000.000000));
  np0 = NavPoint(p0,0.000000);     traffic.addNavPoint(np0);
  np1 = NavPoint(p1,12740.346900);     traffic.addNavPoint(np1);

  double tm = 0;
  double  Td = Units::from("s",1200);         // the lookahead time (relative)
  PlanUtil::interpolateVirtuals(ownship,Units::from("NM",0.1),tm,tm+Td);
  cdii.detection(ownship, traffic, tm, tm+Td);
  EXPECT_NEAR(712.5005,cdii.getTimeIn(0) ,0.0001);
  //    f.pln(tm+" $$$$$$$ cdii = \n"+cdii);

  tm = 720;
  cdii.detection(ownship, traffic, tm, tm+Td);
  EXPECT_NEAR(tm,cdii.getTimeIn(0) ,0.0001);
  //    f.pln(tm+" $$$$$$$ cdii = \n"+cdii);

  tm = 1260;
  cdii.detection(ownship, traffic, tm, tm+Td);
  //    f.pln(tm+" $$$$$$$ cdii = \n"+cdii);
  EXPECT_NEAR(tm,cdii.getTimeIn(0) ,0.0001);
  cdii.detectionExtended(ownship, traffic, tm, tm+Td);
  //    f.pln(tm+" $$$$$$$ cdii = \n"+cdii);
  EXPECT_NEAR(712.5005,cdii.getTimeIn(0) ,0.0001);

  p0  = Position(LatLonAlt::make(5.814, -3.621, 10000.000000));
  np0 = NavPoint(p0,826);     traffic2.addNavPoint(np0);
  np1 = NavPoint(p1,12740.346900);     traffic2.addNavPoint(np1);

  tm = 720;
  //PlanUtil.interpolateVirtuals(ownship,Units.from("NM",0.1),tm,tm+Td);
  cdii.detection(ownship, traffic2, tm, tm+Td);
  //    f.pln(tm+" $$$$$$$ cdii = \n"+cdii);
  EXPECT_NEAR(826.00,cdii.getTimeIn(0) ,0.0001);

}

TEST_F(CDIITest, testStratT004) {

  Plan own("");
  Position p0  = Position::makeXYZ(44.836702, 10.678188, 5000.000000);
  Position p1  = Position::makeXYZ(44.931616, 10.638938, 5000.000000);
  Position p2  = Position::makeXYZ(45.712447, 10.419209, 5000.000000);
  Position p3  = Position::makeXYZ(46.522999, 10.387833, 5000.000000);
  Position p4  = Position::makeXYZ(54.605042, 11.028613, 5000.000000);
  Position p5  = Position::makeXYZ(55.760937, 10.924473, 5000.000000);
  Position p6  = Position::makeXYZ(56.816926, 10.442991, 5000.000000);
  Position p7  = Position::makeXYZ(83.429138, -7.637322, 5000.000000);
  Position p8  = Position::makeXYZ(85.707185, -8.217058, 5000.000000);
  Position p9  = Position::makeXYZ(87.827965, -7.203213, 5000.000000);
  Position p10  = Position::makeXYZ(87.900000, -7.130000, 5000.000000);
  NavPoint np0(p0,1075.000000);     own.addNavPoint(np0);
  NavPoint np1(p1,1076.000000);     own.addNavPoint(np1);
  NavPoint np2(p2,1083.915900);     own.addNavPoint(np2);
  NavPoint np3(p3,1091.831800);     own.addNavPoint(np3);
  NavPoint np4(p4,1170.767300);     own.addNavPoint(np4);
  NavPoint np5(p5,1182.120900);     own.addNavPoint(np5);
  NavPoint np6(p6,1193.474500);     own.addNavPoint(np6);
  NavPoint np7(p7,1506.718900);     own.addNavPoint(np7);
  NavPoint np8(p8,1530.072800);     own.addNavPoint(np8);
  NavPoint np9(p9,1553.426700);     own.addNavPoint(np9);
  NavPoint np10(p10,1554.426700);     own.addNavPoint(np10);

  Plan traf("");
  p0  = Position::makeXYZ(45.570000, -18.680000, 5000.000000);
  p1  = Position::makeXYZ(52.480000, 0.320000, 5000.000000);
  p2  = Position::makeXYZ(65.870000, 27.320000, 5000.000000);
  np0 = NavPoint(p0,915.900000);     traf.addNavPoint(np0);
  np1 = NavPoint(p1,1115.900000);     traf.addNavPoint(np1);
  np2 = NavPoint(p2,1511.900000);     traf.addNavPoint(np2);
  cdii.setDistance(5.2, "NM");
  cdii.setHeight(1100,"ft");
  cdii.setFilterTime(0);
  cdii.detection(own, traf, 0, 10000);
  //fpln(" $$ cdii = "+cdii);
  EXPECT_NEAR(1192.6367,cdii.getTimeIn(0),0.0001);
  EXPECT_NEAR(1250.5910,cdii.getTimeOut(0),0.0001);
  EXPECT_NEAR(0.42930,cdii.getDistanceAtCriticalTime(0) ,0.0001);
  EXPECT_NEAR(1,cdii.size() ,0.0001);
  EXPECT_TRUE(cdii.getCore().conflict());
  EXPECT_NEAR(0.0,cdii.getFilterTime() ,0.0001);
  EXPECT_TRUE(cdii.conflictBetween(1192,1193));
  EXPECT_EQ(5,cdii.getSegmentIn(0));
  EXPECT_EQ(6,cdii.getSegmentOut(0));
  EXPECT_FALSE(cdii.hasError());
  EXPECT_TRUE(cdii.getCore().violation(own, traf, 1200));

}

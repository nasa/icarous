/*
 * ACCoRDTest.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC; modified for gtest framework by Jeff Maddalon (July 2012)
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#include <iostream>
#include "Vect3.h"
#include "Velocity.h"
#include "Units.h"
#include <gtest/gtest.h>
#include "CDSSCore.h"
#include "Detection3D.h"
#include "CDCylinder.h"
#include "TCAS3D.h"
#include "WCV_TAUMOD.h"
#include "WCV_TCPA.h"
#include "WCV_TEP.h"
#include "format.h"

using namespace larcfm;
using namespace std;

class CDSSCoreTest : public testing::Test {

public:

protected:
  virtual void SetUp() {
  }
};

TEST_F(CDSSCoreTest, testCores1) { // from S003
  Vect3 so = Vect3::makeXYZ(0.00,"nm", 0.00,"nm", 25000.00,"ft");
  Velocity vo = Velocity::makeVxyz(392.33, -325.30, 50.00);
  Vect3 si = Vect3::makeXYZ(23.66,"nm", 6.33,"nm", 25000.00,"ft");
  Velocity vi = Velocity::makeVxyz(-126.66, -472.71, 60.00);
  double T = 300.0;
  Detection3D* cd = new CDCylinder();
  CDSSCore cdss = CDSSCore(cd, 0.0);
  bool det = cdss.detection(so, vo, si, vi, T);
  bool conf = cdss.conflict(so, vo, si, vi, T);
  bool viol = cdss.violation(so,vo,si,vi);
  bool isdet = cdss.conflict();
  double tin = cdss.getTimeIn();
  double tout = cdss.getTimeOut();
  double tca = cdss.getCriticalTime();
  double dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_FALSE(viol);
  EXPECT_NEAR(130.14, tin, 0.01);
  EXPECT_NEAR(196.68, tout, 0.01);
  EXPECT_NEAR(163.41, tca, 0.01);
  EXPECT_NEAR(0.0, dtca, 0.01);

  TCASTable tcas_table;
  cd = new TCAS3D();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_FALSE(viol);
  EXPECT_NEAR(127.10, tin, 0.01);
  EXPECT_NEAR(170.31, tout, 0.01);
  EXPECT_NEAR(163.41, tca, 0.01);
  EXPECT_NEAR(0.12, dtca, 0.01);

  cd = new WCV_TAUMOD();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_FALSE(viol);
  EXPECT_NEAR(128.04, tin, 0.01);
  EXPECT_NEAR(167.02, tout, 0.01);
  EXPECT_NEAR(147.53, tca, 0.01);
  EXPECT_NEAR(13.31, dtca, 0.01);

  cd = new WCV_TCPA();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_FALSE(viol);
  EXPECT_NEAR(128.41, tin, 0.01);
  EXPECT_NEAR(167.02, tout, 0.01);
  EXPECT_NEAR(147.71, tca, 0.01);
  EXPECT_NEAR(13.01, dtca, 0.01);

  cd = new WCV_TEP();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_FALSE(viol);
  EXPECT_NEAR(124.78, tin, 0.01);
  EXPECT_NEAR(167.02, tout, 0.01);
  EXPECT_NEAR(145.91, tca, 0.01);
  EXPECT_NEAR(16.11, dtca, 0.01);
}

TEST_F(CDSSCoreTest, testCores2) { // from S047_los_no_trk
  Vect3 so = Vect3::makeXYZ(51.85,"nm", 5.36,"nm", 25000.00,"ft");
  Velocity vo = Velocity::makeTrkGsVs(152.400803,        479,      0.000000);
  Vect3 si = Vect3::makeXYZ(51.95,"nm", 5.99,"nm", 25200.00,"ft");
  Velocity vi = Velocity::makeTrkGsVs(182.368530,        489,      0.000000);
  double T = 300.0;
  Detection3D* cd = new CDCylinder();
  CDSSCore cdss = CDSSCore(cd, 0.0);
  bool det = cdss.detection(so, vo, si, vi, T);
  bool conf = cdss.conflict(so, vo, si, vi, T);
  bool viol = cdss.violation(so,vo,si,vi);
  bool isdet = cdss.conflict();
  double tin = cdss.getTimeIn();
  double tout = cdss.getTimeOut();
  double tca = cdss.getCriticalTime();
  double dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_TRUE(viol);
  EXPECT_NEAR(0.0, tin, 0.01);
  EXPECT_NEAR(75.08, tout, 0.01);
  EXPECT_NEAR(0.0, tca, 0.01);
  EXPECT_NEAR(0.04, dtca, 0.01);

  cd = new TCAS3D();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_TRUE(viol);
  EXPECT_NEAR(0.0, tin, 0.01);
  EXPECT_NEAR(17.11, tout, 0.01);
  EXPECT_NEAR(3.71, tca, 0.01);
  EXPECT_NEAR(0.28, dtca, 0.01);

  cd = new WCV_TAUMOD();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_TRUE(viol);
  EXPECT_NEAR(0.0, tin, 0.01);
  EXPECT_NEAR(8.14, tout, 0.01);
  EXPECT_NEAR(4.07, tca, 0.01);
  EXPECT_NEAR(0.79, dtca, 0.01);

  cd = new WCV_TCPA();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_TRUE(viol);
  EXPECT_NEAR(0.0, tin, 0.01);
  EXPECT_NEAR(8.14, tout, 0.01);
  EXPECT_NEAR(4.07, tca, 0.01);
  EXPECT_NEAR(0.79, dtca, 0.01);

  cd = new WCV_TEP();
  cdss.setCoreDetectionPtr(cd);
  det = cdss.detection(so, vo, si, vi, T);
  conf = cdss.conflict(so, vo, si, vi, T);
  viol = cdss.violation(so,vo,si,vi);
  isdet = cdss.conflict();
  tin = cdss.getTimeIn();
  tout = cdss.getTimeOut();
  tca = cdss.getCriticalTime();
  dtca = cdss.distanceAtCriticalTime();

  EXPECT_TRUE(conf);
  EXPECT_TRUE(det);
  EXPECT_TRUE(isdet);
  EXPECT_TRUE(viol);
  EXPECT_NEAR(0.0, tin, 0.01);
  EXPECT_NEAR(8.14, tout, 0.01);
  EXPECT_NEAR(4.07, tca, 0.01);
  EXPECT_NEAR(0.79, dtca, 0.01);
}




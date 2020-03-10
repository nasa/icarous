/*
 * Copyright (c) 2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <iostream>
#include "Vect3.h"
#include "Velocity.h"
#include "Units.h"
#include <gtest/gtest.h>
#include "CDSS.h"
#include "CDSSCyl.h"
#include "Detection3D.h"
#include "CDCylinder.h"
#include "TCAS3D.h"
#include "WCV_TAUMOD.h"
#include "WCV_TCPA.h"
#include "WCV_TEP.h"
#include "format.h"

using namespace larcfm;
using namespace std;

class CDSSTest : public testing::Test {

public:

protected:
  virtual void SetUp() {
  }
};


  TEST_F(CDSSTest, test1_CDSS_cyl_xyz) {
    Position so = Position::makeXYZ(37.71817390, 13.62195017, 5106.28982910);
    Velocity vo = Velocity::mkTrkGsVs(1.962916, 190.217500, 0.515222);
    Position si = Position::makeXYZ(49.08995762, -9.37248428, 5420.50000000);
    Velocity vi = Velocity::mkTrkGsVs(0.348813, 187.214232, 1.524000);
    double 	D = Units::from("m", 6000);
    double 	H = Units::from("m", 290);
    CDSS cdss = CDSS::mkCyl("cylinder", D, H, 1.0);
    bool conflictsFound = cdss.detection(so,vo,si,vi,10000,"s");
    EXPECT_TRUE(conflictsFound);
    double tmIn = cdss.getTimeIn("s");
    double tmOut = cdss.getTimeOut("s");
    EXPECT_NEAR(153.09,tmIn,0.1);
    EXPECT_NEAR(192.5,tmOut,0.1);

    CDSS cdss2 = CDSS(10000, "ft", 2000, "ft");
    conflictsFound = cdss2.detection(so,vo,si,vi,10000,"s");
    EXPECT_TRUE(conflictsFound);
    EXPECT_NEAR(164.98,cdss2.getTimeIn("s"),0.1);
    EXPECT_NEAR(183.32,cdss2.getTimeOut("s"),0.1);
    EXPECT_FALSE(cdss2.lossOfSeparation(so,vo,si,vi));
  }



  TEST_F(CDSSTest, test1_CDSS_cyl_ll) {
    Position so(LatLonAlt::make(10, -0.383, 11500));
    Position si(LatLonAlt::make(10,  0,     10000));
    Velocity vo = Velocity::makeTrkGsVs( 93,"deg", 260,"kn", -500,"fpm");
    Velocity vi = Velocity::makeTrkGsVs(287,"deg", 350,"kn",  200,"fpm");
    double 	D_det = Units::from("NM", 5.0);
    double 	H_det = Units::from("ft", 1000);
    CDSS cdss = CDSS::mkCyl("cylinder", D_det, H_det, 1.0);
    bool conflictsFound = cdss.detection(so,vo,si,vi,10000,"s");
    EXPECT_TRUE(conflictsFound);
    double tmIn = cdss.getTimeIn("s");
    double tmOut = cdss.getTimeOut("s");
    EXPECT_NEAR(117.19,tmIn,0.1);
    EXPECT_NEAR(146.89,tmOut,0.1);
    EXPECT_TRUE(cdss.conflict(so,vo,si,vi,0,10000));
    EXPECT_EQ("cylinder",cdss.getCorePtr()->getIdentifier());
  }



  TEST_F(CDSSTest, test1_CDSS) {    //  copied from testCDSSDetection3D 1/9/2015

    Vect3 so = Vect3::ZERO();
    Velocity vo = Velocity::makeTrkGsVs(0.0, 300.0, 0.0);
    double D = Units::from("nmi",5.0);
    double H = Units::from("ft",1000.0);
    double T = 300.0;
    CDSS cdss = CDSS(D,"m",H,"m");
    cdss.setFilterTime(0.0);
    Position sop = Position(so);
    for (double x = Units::from("nmi",-50); x < Units::from("nmi",50); x = x +Units::from("nmi", 5.0)) {
      for (double y = Units::from("nmi",-50); y < Units::from("nmi",50); y = y +Units::from("nmi", 10.0)) {
        for (double z = Units::from("ft",-5000); x < Units::from("ft",5000); x = x +Units::from("ft", 1000.0)) {
          Vect3 si(x,y,z);
          Position sip = Position(si);
         for (double trk = 0.0; trk < Units::from("deg",360.0); trk = trk + Units::from("deg",10.0)) {
            for (double gs = Units::from("kn",100.0); gs < Units::from("kn",600); gs = gs + Units::from("kn",50.0)) {
              for (double vs = Units::from("fpm",-3000.0); vs <= Units::from("fpm",3000.0); vs = vs + Units::from("fpm",500.0)) {
                Velocity vi = Velocity::mkTrkGsVs(trk, gs, vs);
                bool conf1 = CD3D::cd3d(so.Sub(si), vo, vi, D, H, T);
                bool conf2 = cdss.detection(sop, vo, sip, vi, T, "s");
                EXPECT_EQ(conf1,conf2);
                bool los1 = CD3D::LoS(so.Sub(si),D,H);
                bool los2 = cdss.lossOfSeparation(sop, vo, sip, vi);    // should this be violation?
                EXPECT_EQ(los1,los2);
              }
            }
          }
        }
      }
    }

  }


  TEST_F(CDSSTest, test2_CDSSCyl) {
    double D = Units::from("nmi",5.0);
    double H = Units::from("ft",1000.0);
    double T = 300.0;
    Vect3 so = Vect3::makeXYZ(0.000000000000000,"nm", 4.180555555555556,"nm", 0.000000000000000,"ft");
    Velocity vo = Velocity::makeTrkGsVs(0.0000, 350.0000, 0.0000);
    Vect3 si = Vect3::makeXYZ(5.317844039751536,"nm", 8.565001167892936,"nm", 0.000000000000000,"ft");
    Velocity vi = Velocity::makeTrkGsVs(196.9474, 550.0000, 0.0000);
    bool conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
    EXPECT_TRUE(conf);
    CDSS cdss = CDSS(D,"m",H,"m");
    cdss.setFilterTime(0.0);

    Position sop = Position(so);
    Position sip = Position(si);
    bool det = cdss.detection(sop, vo, sip, vi, T, "s" );
    EXPECT_TRUE(det);
    EXPECT_NEAR(12.0217,cdss.getTimeIn("s"),0.01);
    sop = Position::makeXYZ(-0.000,"nm", 5.347,"nm", 0.0,"ft");
    vo = Velocity::makeTrkGsVs(359.858, 350.0000, 0.000);
    sip = Position::makeXYZ(4.713,"nm", 6.8344,"nm", 0.000,"ft");
    vi = Velocity::makeTrkGsVs(199.725, 550.000, 0.000);
    det = cdss.detection(sop, vo, sip, vi, T, "s");
    EXPECT_TRUE(det);
    EXPECT_NEAR(0.0,cdss.getTimeIn("s"),0.01);
    so = Vect3::makeXYZ(5.00,"nm", 0.0,"nm", 0.0,"ft");
    vo = Velocity::makeTrkGsVs(90.0, 350.0000, 0.000);
    si = Vect3::makeXYZ(-5.00,"nm", 0.0,"nm", 0.0,"ft");
    vi = Velocity::makeTrkGsVs(270, 550.000, 0.000);
    conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
    EXPECT_FALSE(conf);
  }



  TEST_F(CDSSTest, test3_CDSSCyl) {
    double D = Units::from("nmi",5.2);
    double H = Units::from("ft",1079.0);
    double T = 360.0;
    Vect3 so = Vect3::makeXYZ(0.0,"nm", 0.0,"nm", 30000,"ft");
    Vect3 si = Vect3::makeXYZ(0.0,"nm", 59.81,"nm", 30000,"ft");
    Velocity vo = Velocity::makeTrkGsVs(0.0,459.67,0.0);
    Velocity vi = Velocity::makeTrkGsVs(180,459.60,0.0);
    bool conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
    EXPECT_TRUE(conf);
    CDSSCyl cdss = CDSSCyl(D,H,0.0);
    //EXPECT_NEAR(Consts.InfiniteTime,cdss.getTimeHorizon(),0.0001);  // i.e. -1
    bool detEver = cdss.detectionEver(so, vo, si, vi);
    EXPECT_TRUE(detEver);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);
    EXPECT_NEAR(254.589,cdss.getTimeOut(),0.001);
    EXPECT_NEAR(234.224,cdss.getCriticalTime(),0.001);
    EXPECT_NEAR(3.564,cdss.getTimeIn("min"),0.001);
    EXPECT_NEAR(4.243,cdss.getTimeOut("min"),0.001);
    EXPECT_NEAR(3.904,cdss.getCriticalTime("min"),0.001);
    //cdss.setTimeHorizon(230.0);
    detEver = cdss.detectionEver(so, vo, si, vi);
    EXPECT_TRUE(detEver);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);
    //EXPECT_NEAR(230,cdss.timeOut(),0.001);                     // truncates to timeHorizon
    //EXPECT_NEAR(230,cdss.timeOfClosestApproach(),0.001);       // truncates to timeHorizon   (is this what we want?
    //cdss.setTimeHorizon(200.0);
    detEver = cdss.detectionEver(so, vo, si, vi);
    //EXPECT_FALSE(detEver);
    //cdss.setTimeHorizon(Consts.InfiniteTime);
    double B = 10.0;
    T = 240.0;
    bool detBetw = cdss.detectionBetween(so, vo, si, vi,B,T);
    EXPECT_TRUE(detBetw);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);
    EXPECT_NEAR(254.589,cdss.getTimeOut(),0.001);                    // does not truncate to T !!
    EXPECT_NEAR(234.224,cdss.getCriticalTime(),0.001);
    EXPECT_NEAR(3.564,cdss.getTimeIn("min"),0.001);
    EXPECT_NEAR(4.243,cdss.getTimeOut("min"),0.001);
    EXPECT_NEAR(3.904,cdss.getCriticalTime("min"),0.001);
    B = 220.0;
    T = 270.0;
    detBetw = cdss.detectionBetween(so, vo, si, vi,B,T);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);                    // timeIn not within [B,T]
    EXPECT_TRUE(detBetw);
    B = 260.0;
    T = 270.0;
    detBetw = cdss.detectionBetween(so, vo, si, vi,B,T);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);
    EXPECT_FALSE(detBetw);                                          // T is after timeOut
    B = 220.0;
    T = 250.0;
    detBetw = cdss.detectionBetween(so, vo, si, vi,B,T);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);
    EXPECT_NEAR(254.589,cdss.getTimeOut(),0.001);                    // does not truncate to T !!
    EXPECT_TRUE(detBetw);                                           // [B,T]  contained in  [tmIn, tmOut] = [213.8, 254.6]
    //cdss.setTimeHorizon(230);
    detBetw = cdss.detectionBetween(so, vo, si, vi,B,T);
    EXPECT_TRUE(detBetw);
    EXPECT_NEAR(213.861,cdss.getTimeIn(),0.001);
    //EXPECT_NEAR(230.0,cdss.timeOut(),0.001);                    //  truncates to timeHorizon
    //EXPECT_NEAR(230.0,cdss.timeOfClosestApproach(),0.001);      //  truncates to timeHorizon
  }


  TEST_F(CDSSTest, test_taumod_ll) {
    Position so = Position::makeLatLonAlt(-0.86735, -0.006675676, 290.82);
    Velocity vo = Velocity::mkTrkGsVs(-3.065767, 233.546859, 0.00);
    Position si = Position::makeLatLonAlt(-1.11893, 0.033499187, 164.04);
    Velocity vi = Velocity::mkTrkGsVs(Units::from("deg",332.0), 169.59, -0.657);
    double DTHR = Units::from("ft", 4000.0);
    double ZTHR = Units::from("ft", 3475.0);
    double TCOA = 0.0;
    double TTHR = 35.0;
    CDSS cdss = CDSS::mkTauMOD("tauMod", DTHR, ZTHR, TTHR, TCOA);
    bool conflictsFound = cdss.detection(so,vo,si,vi,10000,"s");
    //fpln(" $$$ CDSSTest: cdss = "+cdss.toString());
    EXPECT_TRUE(conflictsFound);
    double tmIn = cdss.getTimeIn("s");
    double tmOut = cdss.getTimeOut("s");
    EXPECT_NEAR(37.75,tmIn,0.1);
    EXPECT_NEAR(76.18,tmOut,0.1);
    EXPECT_NEAR(56.97,cdss.getCriticalTime("s"),0.1);
    CDSSCore* cdsscore = cdss.getCorePtr();
    EXPECT_NEAR(26.09,cdsscore->distanceAtCriticalTime(),0.1);
    EXPECT_NEAR(38.42,cdsscore->conflictDuration(),0.1);
    EXPECT_NEAR(0.0,cdsscore->getFilterTime(),0.1);
    Detection3D* coreDetection = cdsscore->getCoreDetectionPtr();
    //EXPECT_TRUE(coreDetection instanceof WCV_TAUMOD);
    EXPECT_EQ("tauMod",coreDetection->getIdentifier());
    //cdss.getCoreRef().setFilterTime(38.5);
    //coreDetection->setFilterTime(38.5);
    cdsscore->setFilterTime(38.5);
    conflictsFound = cdss.detection(so,vo,si,vi,10000,"s");
    EXPECT_FALSE(conflictsFound);
    cdsscore->setFilterTime(38.0);
    //cdss.getCoreRef().setFilterTime(38);
    conflictsFound = cdss.detection(so,vo,si,vi,10000,"s");
    EXPECT_TRUE(conflictsFound);
  }


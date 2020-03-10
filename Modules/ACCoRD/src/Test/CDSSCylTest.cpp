/*
 * CDSSCylTest.cpp
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

#include "CDSSCyl.h"

#include <iostream>
#include "Vect3.h"
#include "Velocity.h"
#include "Units.h"
#include <gtest/gtest.h>
#include "CDSSCore.h"
#include "CDCylinder.h"

using namespace larcfm;
using namespace std;

class CDSSCylTest : public testing::Test {

public:
  double D;
  double H;
  double T;
  CDCylinder* cdcyl;

  Vect3 so, si;
  Velocity vo, vi;
  
protected:
  virtual void SetUp() {
    D = Units::from("nmi",5);    // [nm]
    H = Units::from("ft",1000); // [ft]
    T = Units::from("min",5);    // [min]
    cdcyl = new CDCylinder(D,H);

  }
};

//Vect3 makeXYZnmnmft(double x, double y, double z) {
//  return Vect3::makeXYZ(x,"nm",y,"nm",z,"ft");
//}

TEST_F(CDSSCylTest, test1_CDSSCore) {

  so = Vect3::ZERO();
  vo = Velocity::makeTrkGsVs(0.0, 300.0, 0.0);
  CDSSCore cdsscore(cdcyl,0.0);

  for (double x = Units::from("nmi",-50); x < Units::from("nmi",50); x = x +Units::from("nmi", 5.0)) {
    for (double y = Units::from("nmi",-50); y < Units::from("nmi",50); y = y +Units::from("nmi", 10.0)) {
      for (double z = Units::from("ft",-5000); x < Units::from("ft",5000); x = x +Units::from("ft", 1000.0)) {
        Vect3 si = Vect3(x,y,z);
        for (double trk = 0.0; trk < Units::from("deg",360.0); trk = trk + Units::from("deg",10.0)) {
          for (double gs = Units::from("kn",100.0); gs < Units::from("kn",600); gs = gs + Units::from("kn",50.0)) {
            for (double vs = Units::from("fpm",-3000.0); vs <= Units::from("fpm",3000.0); vs = vs + Units::from("fpm",500.0)) {
              Velocity vi = Velocity::mkTrkGsVs(trk, gs, vs);
              bool conf1 = CD3D::cd3d(so.Sub(si), vo, vi, D, H, T);
              bool conf2 = cdsscore.detection(so, vo, si, vi, T);
              EXPECT_EQ(conf1,conf2);
              bool los1 = CD3D::LoS(so.Sub(si),D,H);
              bool los2 = cdsscore.violation(so, vo, si, vi);
              EXPECT_EQ(los1,los2);
            }
          }
        }
      }
    }
  }
}


TEST_F(CDSSCylTest, test1_CDSS) {

  so = Vect3::ZERO();
  vo = Velocity::makeTrkGsVs(0.0, 300.0, 0.0);
  CDSSCyl cdss(D,H,0.0);

  for (double x = Units::from("nmi",-50); x < Units::from("nmi",50); x = x +Units::from("nmi", 5.0)) {
    for (double y = Units::from("nmi",-50); y < Units::from("nmi",50); y = y +Units::from("nmi", 10.0)) {
      for (double z = Units::from("ft",-5000); x < Units::from("ft",5000); x = x +Units::from("ft", 1000.0)) {
        Vect3 si = Vect3(x,y,z);
        for (double trk = 0.0; trk < Units::from("deg",360.0); trk = trk + Units::from("deg",10.0)) {
          for (double gs = Units::from("kn",100.0); gs < Units::from("kn",600); gs = gs + Units::from("kn",50.0)) {
            for (double vs = Units::from("fpm",-3000.0); vs <= Units::from("fpm",3000.0); vs = vs + Units::from("fpm",500.0)) {
              Velocity vi = Velocity::mkTrkGsVs(trk, gs, vs);
              bool conf1 = CD3D::cd3d(so.Sub(si), vo, vi, D, H, T);
              bool conf2 = cdss.detection(so, vo, si, vi, T);
              EXPECT_EQ(conf1,conf2);
              bool los1 = CD3D::LoS(so.Sub(si),D,H);
              bool los2 = cdss.lossOfSeparation(so, si);
              EXPECT_EQ(los1,los2);
            }
          }
        }
      }
    }
  }
}



TEST_F(CDSSCylTest, test2_CDSSCore) {
  double D = Units::from("nmi",5.0);
  double H = Units::from("ft",1000.0);
  double T = 300.0;
  Vect3 so = Vect3::make(0.000000000000000, 4.180555555555556, 0.000000000000000);
  Velocity vo = Velocity::makeTrkGsVs(0.0000, 350.0000, 0.0000);
  Vect3 si = Vect3::make(5.317844039751536, 8.565001167892936, 0.000000000000000 );
  Velocity vi = Velocity::makeTrkGsVs(196.9474, 550.0000, 0.0000);
  bool conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  EXPECT_TRUE(conf);
  CDSSCore cdsscore = CDSSCore(cdcyl,0.0);
  bool det = cdsscore.detection(so, vo, si, vi, T);
  EXPECT_TRUE(det);
  EXPECT_NEAR(12.0217,cdsscore.getTimeIn(),0.01);
  so = Vect3::make(-0.000, 5.347, 0.0);
  vo = Velocity::makeTrkGsVs(359.858, 350.0000, 0.000);
  si = Vect3::make(4.713, 6.8344, 0.000);
  vi = Velocity::makeTrkGsVs(199.725, 550.000, 0.000);
  conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  det = cdsscore.detection(so, vo, si, vi, T);
  EXPECT_TRUE(det);
  EXPECT_NEAR(0.0,cdsscore.getTimeIn(),0.01);
  so = Vect3::make(5.00, 0.0, 0.0);
  vo = Velocity::makeTrkGsVs(90.0, 350.0000, 0.000);
  si = Vect3::make(-5.00, 0.0, 0.000);
  vi = Velocity::makeTrkGsVs(270, 550.000, 0.000);
  conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  EXPECT_FALSE(conf);
}

TEST_F(CDSSCylTest, test2_CDSS) {
  double D = Units::from("nmi",5.0);
  double H = Units::from("ft",1000.0);
  double T = 300.0;
  Vect3 so = Vect3::make(0.000000000000000, 4.180555555555556, 0.000000000000000);
  Velocity vo = Velocity::makeTrkGsVs(0.0000, 350.0000, 0.0000);
  Vect3 si = Vect3::make(5.317844039751536, 8.565001167892936, 0.000000000000000 );
  Velocity vi = Velocity::makeTrkGsVs(196.9474, 550.0000, 0.0000);
  bool conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  EXPECT_TRUE(conf);
  CDSSCyl cd = CDSSCyl(D,H,0.0);
  bool det = cd.detection(so, vo, si, vi, T);
  EXPECT_TRUE(det);
  EXPECT_NEAR(12.0217,cd.getTimeIn(),0.01);
  so = Vect3::make(-0.000, 5.347, 0.0);
  vo = Velocity::makeTrkGsVs(359.858, 350.0000, 0.000);
  si = Vect3::make(4.713, 6.8344, 0.000);
  vi = Velocity::makeTrkGsVs(199.725, 550.000, 0.000);
  conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  det = cd.detection(so, vo, si, vi, T);
  EXPECT_TRUE(det);
  EXPECT_NEAR(0.0,cd.getTimeIn(),0.01);
  so = Vect3::make(5.00, 0.0, 0.0);
  vo = Velocity::makeTrkGsVs(90.0, 350.0000, 0.000);
  si = Vect3::make(-5.00, 0.0, 0.000);
  vi = Velocity::makeTrkGsVs(270, 550.000, 0.000);
  conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  EXPECT_FALSE(conf);
}


TEST_F(CDSSCylTest, test3_CDSS) {
  double D = Units::from("nmi",5.2);
  double H = Units::from("ft",1079.0);
  double T = 360.0;
  Vect3 so = Vect3::make(0.0, 0.0, 30000);
  Vect3 si = Vect3::make(0.0,59.81,30000);
  Velocity vo = Velocity::makeTrkGsVs(0.0,459.67,0.0);
  Velocity vi = Velocity::makeTrkGsVs(180,459.60,0.0);
  bool conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  EXPECT_TRUE(conf);
  CDSSCyl cdss = CDSSCyl(D,H,0.0);
  //EXPECT_NEAR(InfiniteTime,cdss.getTimeHorizon(),0.0001);  // i.e. -1
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
  //EXPECT_NEAR(230,cdss.getTimeOut(),0.001);                     // truncates to timeHorizon
  //EXPECT_NEAR(230,cdss.timeOfClosestApproach(),0.001);       // truncates to timeHorizon   (is this what we want?
  //cdss.setTimeHorizon(200.0);
  detEver = cdss.detectionEver(so, vo, si, vi);
  EXPECT_TRUE(detEver);
  //cdss.setTimeHorizon(InfiniteTime);
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
  //EXPECT_NEAR(230.0,cdss.getTimeOut(),0.001);                    //  truncates to timeHorizon
  //EXPECT_NEAR(230.0,cdss.timeOfClosestApproach(),0.001);      //  truncates to timeHorizon
}

TEST_F(CDSSCylTest, test3_CDSSCore) {
  double D = Units::from("nmi",5.2);
  double H = Units::from("ft",1079.0);
  double T = 360.0;
  Vect3 so = Vect3::make(0.0, 0.0, 30000);
  Vect3 si = Vect3::make(0.0,59.81,30000);
  Velocity vo = Velocity::makeTrkGsVs(0.0,459.67,0.0);
  Velocity vi = Velocity::makeTrkGsVs(180,459.60,0.0);
  bool conf = CD3D::cd3d(so.Sub(si),vo,vi, D, H, T);
  EXPECT_TRUE(conf);
  CDCylinder cdcyl = CDCylinder::mk(D,H); // it gets copied right away
  CDSSCore cdss = CDSSCore::make(cdcyl,0.0,"s");
  //EXPECT_NEAR(InfiniteTime,cdss.getTimeHorizon(),0.0001);  // i.e. -1
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
  //EXPECT_NEAR(230,cdss.getTimeOut(),0.001);                     // truncates to timeHorizon
  //EXPECT_NEAR(230,cdss.timeOfClosestApproach(),0.001);       // truncates to timeHorizon   (is this what we want?
  //cdss.setTimeHorizon(200.0);
  detEver = cdss.detectionEver(so, vo, si, vi);
  EXPECT_TRUE(detEver);
  //cdss.setTimeHorizon(InfiniteTime);
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
  //EXPECT_NEAR(230.0,cdss.getTimeOut(),0.001);                    //  truncates to timeHorizon
  //EXPECT_NEAR(230.0,cdss.timeOfClosestApproach(),0.001);      //  truncates to timeHorizon
}


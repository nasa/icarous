/*
 * Copyright (c) 2014-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "CRSS.h"
#include "Units.h"
#include "Vect3.h"
#include "Velocity.h"
#include "format.h"

#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class CRSSTest : public ::testing::Test {

public:
  CRSS crss;

protected:
  virtual void SetUp() {
    crss = CRSS::make(5.0, "nmi", 1000.0, "ft");
  }
};

Vect3 makeXYZnmnmft(double x, double y, double z) {
  return Vect3::makeXYZ(x,"nm",y,"nm",z,"ft");
}

TEST_F(CRSSTest, testConstructors) {
  crss = CRSS(Units::from("nmi",5.1), Units::from("ft",950));
  EXPECT_NEAR(5.1,crss.getDistance("NM"),0.00000001);
  EXPECT_NEAR(950,crss.getHeight("ft"),0.00000001);

  crss = CRSS::make(5.2, "nm", 960, "ft");
  EXPECT_NEAR(5.2,crss.getDistance("NM"),0.00000001);
  EXPECT_NEAR(960,crss.getHeight("ft"),0.00000001);

  crss = CRSS::mk(Units::from("nmi",5.3), Units::from("ft",977));
  EXPECT_NEAR(5.3,crss.getDistance("NM"),0.00000001);
  EXPECT_NEAR(977,crss.getHeight("ft"),0.00000001);
}


TEST_F(CRSSTest, testSetters) {
  crss.setDistance(Units::from("NM",5.4));
  EXPECT_NEAR(5.4,crss.getDistance("NM"),0.00000001);
  crss.setHeight(Units::from("ft",1201));
  EXPECT_NEAR(1201,crss.getHeight("ft"),0.00000001);
  crss.setMinGs(400);
  EXPECT_NEAR(400,crss.getMinGs(),0.0000001);
  crss.setMaxGs(500);
  EXPECT_NEAR(500,crss.getMaxGs(),0.0000001);
  crss.setMaxVs(333);
  EXPECT_NEAR(333,crss.getMaxVs(),0.0000001);
  crss.setMinHorizExitSpeed(405);
  EXPECT_NEAR(405,crss.getMinHorizExitSpeed(),0.0000001);
  crss.setMinVertExitSpeed(1001);
  EXPECT_NEAR(1001,crss.getMinVertExitSpeed(),0.0000001);
  crss.setVsDiscretization(333);
  EXPECT_NEAR(333,crss.getVsDiscretization(),0.0000001);
}


TEST_F(CRSSTest, testResolution) {
  Vect3 so(Units::from("nm",14.85), Units::from("nm",5.3), Units::from("ft",25000));
  Vect3 si(Units::from("nm",11.05), Units::from("nm",5.9), Units::from("ft",25500));
  Velocity vo = Velocity::makeVxyz(500, 0.0, "kn", 0.0,"fpm");
  Velocity vi = Velocity::makeVxyz(-205.21, -563.82, "kn", 800.0,"fpm");
  int resolution = crss.resolution(so.Sub(si),vo,vi, "own", "traf");
  EXPECT_EQ(3,resolution);
  EXPECT_TRUE(crss.hasTrkOnly());
  EXPECT_TRUE(crss.hasGsOnly());
  EXPECT_TRUE(crss.hasVsOnly());
  EXPECT_FALSE(crss.hasOptTrkGs());
  //EXPECT_FALSE(crss.hasSpeedOnly());                              // NOT IN C++ VERSION OF CRSS ??????????
  //EXPECT_FALSE(crss.hasConstantAoA());                            // NOT IN C++ VERSION OF CRSS ??????????
  EXPECT_NEAR(92.00,crss.trkOnly("deg"),0.001);
  EXPECT_NEAR(520.000,crss.gsOnly("kn"),0.001);
  EXPECT_NEAR(-700.0,crss.vsOnly("fpm"),0.001);
}



TEST_F(CRSSTest, testResolution_T004) {
  Vect3 so = makeXYZnmnmft(0.0, 0.0, 30000);
  Vect3 si = makeXYZnmnmft(0.0,59.81,30000);
  Velocity vo = Velocity::makeTrkGsVs(0.0,459.67,0.0);
  Velocity vi = Velocity::makeTrkGsVs(180,459.60,0.0);
  int resolution = crss.resolution(so.Sub(si),vo,vi, "own", "traf");
  EXPECT_EQ(1,resolution);   // conflict
  EXPECT_TRUE(crss.hasTrkOnly());
  EXPECT_FALSE(crss.hasGsOnly());
  EXPECT_TRUE(crss.hasVsOnly());
  EXPECT_TRUE(crss.hasOptTrkGs());
  //EXPECT_FALSE(crss.hasSpeedOnly());                              // NOT IN C++ VERSION OF CRSS ??????????
  //EXPECT_FALSE(crss.hasConstantAoA());                           // NOT IN C++ VERSION OF CRSS ??????????
  EXPECT_NEAR(350.409,crss.trkOnly("deg"),0.001);
  EXPECT_NEAR(280.0,crss.vsOnly("fpm"),0.001);
  EXPECT_NEAR(350.409,crss.optTrk("deg"),0.001);
  EXPECT_NEAR(459.669,crss.optGs("kn"),0.001);
}



TEST_F(CRSSTest, testResolution_2) {
  Vect3 so = makeXYZnmnmft(0.0, 0.0, 30000);
  Vect3 si = makeXYZnmnmft(17.46,31.20,30000);
  Velocity vo = Velocity::makeTrkGsVs(0.0,460,0.0);
  Velocity vi = Velocity::makeTrkGsVs(236,460,0.0);
  int resolution = crss.resolution(so.Sub(si),vo,vi, "own", "traf");
  EXPECT_EQ(1,resolution);   // conflict
  EXPECT_TRUE(crss.hasTrkOnly());
  EXPECT_TRUE(crss.hasGsOnly());
  EXPECT_TRUE(crss.hasVsOnly());
  EXPECT_TRUE(crss.hasOptTrkGs());
  //EXPECT_TRUE(crss.hasSpeedOnly());
  //EXPECT_TRUE(crss.hasConstantAoA());
  EXPECT_NEAR(346.386,crss.trkOnly("deg"),0.001);
  EXPECT_NEAR(440,crss.vsOnly("fpm"),0.001);
  EXPECT_NEAR(349.717,crss.optTrk("deg"),0.001);
  EXPECT_NEAR(502.882,crss.optGs("kn"),0.001);
  //EXPECT_NEAR(726.324,crss.speedOnlyGs("kn"),0.001);
  //EXPECT_NEAR(0.0,crss.speedOnlyVs("fpm"),0.001);
}



TEST_F(CRSSTest, testResolution_unnecessary) {
  Vect3 so = makeXYZnmnmft(16.75, 5.3, 23000);
  Vect3 si = makeXYZnmnmft(11.35, 5.9, 23000);
  Velocity vo = Velocity::makeVxyz(500, 0.0, "kn", 0.0,"fpm");
  Velocity vi = Velocity::makeVxyz(-205.21, -563.82, "kn", 800.0,"fpm");
  int resolution = crss.resolution(so.Sub(si),vo,vi, "own", "traf");
  EXPECT_EQ(0,resolution);            // Unnecessary
  EXPECT_TRUE(crss.hasTrkOnly());                                   // ???
  EXPECT_TRUE(crss.hasGsOnly());                                    // ???
  EXPECT_TRUE(crss.hasVsOnly());                                    // ???
  EXPECT_TRUE(crss.hasOptTrkGs());
  //EXPECT_FALSE(crss.hasSpeedOnly());
  //EXPECT_FALSE(crss.hasConstantAoA());
  EXPECT_NEAR(90.00,crss.trkOnly("deg"),0.001);
  EXPECT_NEAR(500.000,crss.gsOnly("kn"),0.001);
  EXPECT_NEAR(0.0,crss.vsOnly("fpm"),0.001);
}

TEST_F(CRSSTest, testChorusT039) {
  Vect3 so = Vect3::makeXYZ(12575.080000, "m", -22224.000000, "m", 7620.000000, "m");  // (16.75,"nm", 5.3,"nm", 23000,"ft");
  Vect3 si = Vect3::makeXYZ(51300.400000, "m", -16871.720000, "m", 7620.000000, "m");   // (11.35,"nm", 5.9,"nm", 23000,"ft");
  Velocity vo = Velocity::makeTrkGsVs(122.300000, "deg", 425.300000, "knot", 50.000000, "fpm") ;  //(500, 0.0, "kn", 0.0,"fpm");
  Velocity vi = Velocity::makeTrkGsVs(218.600000, "deg", 472.710000, "knot", 60.000000, "fpm");    //	(-205.21, -563.82, "kn", 800.0,"fpm");

  Vect3 s = so.Sub(si);

  EXPECT_NEAR(-38725.320000, s.x, 0.0);
  EXPECT_NEAR(-5352.280000, s.y, 0.001);
  EXPECT_NEAR(0.0, s.z, 0.0);

  double Hr=335.28000000000003;
  double FP_FUDGE=1.000000000001;
  double Dr=11941.696;
  double minHorizExitSpeedLoS=51.44444444444445;
  double minVertExitSpeedLoS =7.619999999949201;
  double maxGs=360.11111111111114;
  double minGs=77.16666666666667;
  double maxVs=15.24;

  crss.setDistance(FP_FUDGE*Dr); // floating point errors.
  crss.setHeight(Hr);
  crss.setMinGs(minGs); //f.pln("crc 928: "+f.Fm16(maxGs));
  crss.setMaxGs(maxGs);
  crss.setMaxVs(maxVs);
  crss.setMinHorizExitSpeed(minHorizExitSpeedLoS);
  crss.setMinVertExitSpeed(minVertExitSpeedLoS);

  crss.resolution(s,vo,vi, 1, -1);
  EXPECT_NEAR(-770, crss.vsOnly("fpm"), 0.0);

}


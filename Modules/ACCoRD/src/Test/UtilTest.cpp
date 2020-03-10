/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect2.h"
#include "Units.h"
#include "Util.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class UtilTest : public ::testing::Test {
protected:
    virtual void SetUp() {
     }	 
};
  
  TEST_F(UtilTest, testAlmostLess) {
    EXPECT_TRUE(9999.0 < 10000.0);   
    EXPECT_FALSE(Util::almost_less( 9999.0, 10000.0, PRECISION5));
    EXPECT_FALSE(Util::almost_less(10000.0,  9999.0, PRECISION5));

    EXPECT_TRUE(999.0 < 10000.0);   
    EXPECT_TRUE( Util::almost_less(  999.0, 10000.0, PRECISION5));
    EXPECT_FALSE(Util::almost_less(10000.0,   999.0, PRECISION5));
  }	

  
  TEST_F(UtilTest, testAlmostGreater) {
    EXPECT_TRUE(10000.0 > 9999.0);   
    EXPECT_FALSE(Util::almost_greater( 9999.0, 10000.0, PRECISION5));
    EXPECT_FALSE(Util::almost_greater(10000.0,  9999.0, PRECISION5));

    EXPECT_TRUE(10000.0 > 999.0);   
    EXPECT_FALSE(Util::almost_greater(  999.0, 10000.0, PRECISION5));
    EXPECT_TRUE( Util::almost_greater(10000.0,   999.0, PRECISION5));
  }

  TEST_F(UtilTest, testAlmostLEQ_GEQ) {
		EXPECT_TRUE(10000.0 > 9990.0);   
		EXPECT_TRUE( Util::almost_equals(10000.0, 9999.0, PRECISION5)); 
		EXPECT_FALSE(Util::almost_equals(10000.0, 9990.0, PRECISION5)); 
		
		EXPECT_TRUE( Util::almost_leq( 9990.0, 10000.0, PRECISION5));   
		EXPECT_FALSE(Util::almost_leq(10000.0,  9990.0, PRECISION5));   
		EXPECT_FALSE(Util::almost_geq( 9990.0, 10000.0, PRECISION5));   
		EXPECT_TRUE( Util::almost_geq(10000.0,  9990.0, PRECISION5));   

		EXPECT_TRUE( Util::almost_leq( 9999.0, 10000.0, PRECISION5));   
		EXPECT_TRUE( Util::almost_leq(10000.0,  9999.0, PRECISION5));   
		EXPECT_TRUE( Util::almost_geq( 9999.0, 10000.0, PRECISION5));   
		EXPECT_TRUE( Util::almost_geq(10000.0,  9999.0, PRECISION5));   
	}
  
  TEST_F(UtilTest, testAlmostEqualsNominal) {
    EXPECT_TRUE(Util::almost_equals(10000.0,10000.0));
    EXPECT_FALSE(Util::almost_equals(10000.0,1000.0));
  }

  
  TEST_F(UtilTest, testAlmostEqualsDefault) {
    EXPECT_TRUE( Util::almost_equals(1.0, 0.999999999999999));
    EXPECT_TRUE( Util::almost_equals(1.0, 0.99999999999999));
    EXPECT_TRUE( Util::almost_equals(1.0, 0.9999999999999));
    EXPECT_TRUE( Util::almost_equals(1.0, 0.999999999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.99999999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.9999999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.999999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.99999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.9999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.999999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.99999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.9999));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.999));

    EXPECT_TRUE( Util::almost_equals(1.1e-320, -1.0e-320));
    EXPECT_TRUE( Util::almost_equals(1.0,0.999999999999999));
    EXPECT_TRUE( Util::almost_equals(1.0-0.99999999999999, 0.0));
    EXPECT_TRUE( Util::almost_equals(1.0e20,0.999999999999999e20));
    EXPECT_FALSE(Util::almost_equals(1.0e20-0.99999999999999e20, 0.0));
    EXPECT_TRUE( Util::almost_equals(1.0e-20,0.999999999999999e-20));
    EXPECT_TRUE(Util::almost_equals(1.0e-20-0.999999999999999e-20, 0.0));
  }

  
  TEST_F(UtilTest, testAlmostEqualsAbnormal) {
    EXPECT_TRUE(Util::almost_equals(-0.0,0.0));
    EXPECT_FALSE(Util::almost_equals(NaN,NaN));
    EXPECT_FALSE(Util::almost_equals(PINFINITY, MAXDOUBLE));
    EXPECT_FALSE(Util::almost_equals(NINFINITY,-MAXDOUBLE));
    EXPECT_TRUE(Util::almost_equals( PINFINITY, PINFINITY));
    EXPECT_FALSE(Util::almost_equals(NINFINITY, PINFINITY));
    EXPECT_FALSE(Util::almost_equals(PINFINITY, NINFINITY));
    EXPECT_TRUE(Util::almost_equals( NINFINITY, NINFINITY));
  }

  TEST_F(UtilTest, testAbnormal) {
	  double pinf = PINFINITY;
	  double ninf = NINFINITY;
	  double nan = NaN;
	  EXPECT_FALSE(ISFINITE(pinf));
	  EXPECT_FALSE(ISFINITE(ninf));
	  EXPECT_FALSE(ISFINITE(nan));
	  EXPECT_TRUE(ISPINF(pinf));
	  EXPECT_TRUE(ISNINF(ninf));
	  EXPECT_TRUE(ISINF(pinf*3.2));
	  EXPECT_TRUE(ISPINF(pinf*3.2));
	  EXPECT_TRUE(ISNINF(ninf*3.2));
	  EXPECT_TRUE(ISNINF(pinf*-3.2));
	  EXPECT_TRUE(ISPINF(ninf*-3.2));
	  EXPECT_TRUE(ISNAN(nan*3.2));
	  EXPECT_TRUE(ISPINF(pinf+3.2));
	  EXPECT_TRUE(ISNINF(ninf+3.2));
	  EXPECT_TRUE(ISPINF(pinf-3.2));
	  EXPECT_TRUE(ISNINF(ninf-3.2));
	  EXPECT_TRUE(ISNAN(nan-3.2));
  }

  TEST_F(UtilTest, testAlmostEqualsZero) {
    EXPECT_TRUE(Util::almost_equals(0.0,0.0,PRECISION13));
    EXPECT_TRUE( Util::almost_equals(0.0, -1.0e-14,PRECISION13));
    EXPECT_TRUE( Util::almost_equals(0.0,  1.0e-14,PRECISION13));
    EXPECT_FALSE( Util::almost_equals(0.0,-1.0e-12,PRECISION13));
    EXPECT_FALSE( Util::almost_equals(0.0, 1.0e-12,PRECISION13));
		
    EXPECT_TRUE( Util::almost_equals( -1.0e-14, 0.0, PRECISION13));
    EXPECT_TRUE( Util::almost_equals(  1.0e-14, 0.0, PRECISION13));
    EXPECT_FALSE( Util::almost_equals(-1.0e-12, 0.0, PRECISION13));
    EXPECT_FALSE( Util::almost_equals( 1.0e-12, 0.0, PRECISION13));
    
    EXPECT_TRUE( Util::almost_equals( 0.0,  1.0e-6, PRECISION5));
    EXPECT_FALSE( Util::almost_equals(0.0, -1.0e-4, PRECISION5));
    
    EXPECT_TRUE( Util::almost_equals(  1.0e-6, 0.0, PRECISION5));
    EXPECT_FALSE( Util::almost_equals(-1.0e-4, 0.0, PRECISION5));
  }
  
  TEST_F(UtilTest, testAlmostEquals5) {
    EXPECT_TRUE(Util::almost_equals(1.0, 0.999999, PRECISION5));
    EXPECT_TRUE(Util::almost_equals(1.0, 0.99999, PRECISION5));
    EXPECT_TRUE(Util::almost_equals(1.0, 0.9999, PRECISION5));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.999, PRECISION5));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.99, PRECISION5));
  }

  
  TEST_F(UtilTest, testAlmostEquals7) {
    EXPECT_TRUE(Util::almost_equals(1.0, 0.99999999, PRECISION7));
    EXPECT_TRUE(Util::almost_equals(1.0, 0.9999999, PRECISION7));
    EXPECT_TRUE(Util::almost_equals(1.0, 0.999999, PRECISION7));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.99999, PRECISION7));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.9999, PRECISION7));

    EXPECT_TRUE(Util::almost_equals(0.1, 0.1000001, PRECISION7));
    EXPECT_FALSE(Util::almost_equals(0.1, 0.100001, PRECISION7));
		
    EXPECT_TRUE(Util::almost_equals(0.5, 0.500001, PRECISION7));
    EXPECT_FALSE(Util::almost_equals(0.5, 0.50001, PRECISION7));
    EXPECT_TRUE(Util::almost_equals(0.05, 0.0500001, PRECISION7));
  }

  TEST_F(UtilTest, testAlmostEquals9) {
    EXPECT_TRUE(Util::almost_equals( 1.0, 0.9999999999, PRECISION9));
    EXPECT_TRUE(Util::almost_equals( 1.0, 0.999999999, PRECISION9));
    EXPECT_TRUE(Util::almost_equals(1.0, 0.99999999, PRECISION9));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.9999999, PRECISION9));
    EXPECT_FALSE(Util::almost_equals(1.0, 0.999999, PRECISION9));
  }
    

	TEST_F(UtilTest,testDiscretizeDir) {
		EXPECT_NEAR(220.0,Util::discretizeDir(250.0,221.0,10.0),0.001);
		EXPECT_NEAR(230.0,Util::discretizeDir(20.0,221.0,10.0),0.001);
		EXPECT_NEAR(1300.0,Util::discretizeDir(-20.0,1221.0,100.0),0.001);
		EXPECT_NEAR(-1200.0,Util::discretizeDir(21.0,-1110.0,100.0),0.001);
		EXPECT_NEAR(-1200.0,Util::discretizeDir(-206.0,-1110.0,100.0),0.001);
		EXPECT_NEAR(1200.0,Util::discretizeDir(-206.0,1110.0,100.0),0.001);
		EXPECT_NEAR(1200.0,Util::discretizeDir(1001.0,1110.0,100.0),0.001);
	}


  TEST_F(UtilTest, testSq) {
    EXPECT_NEAR(4.0, sq(2.0), 0.0);
  }

  
  TEST_F(UtilTest, testSqrt_safe) {
    EXPECT_NEAR(2.0, sqrt_safe(4.0), 0.00001);
    EXPECT_NEAR(0.0, sqrt_safe(-4.0), 0.00001);
  }

  
  TEST_F(UtilTest, testAtan2_safe) {
    EXPECT_NEAR(0.0, atan2_safe(0.0, 1.0), 0.00001);
    EXPECT_NEAR(Pi / 2.0, atan2_safe(1.0, 0.0), 0.00001);
    EXPECT_NEAR(0.0, atan2_safe(0.0, 0.0), 0.00001);
  }

  
  TEST_F(UtilTest, testDiscr) {
    EXPECT_NEAR(-3.0, discr(1.0, 1.0, 1.0), 0.000001);
  }

  
  TEST_F(UtilTest, testRoot) {
    EXPECT_NEAR(-1.0, root(1.0, 3.0, 2.0,  1), 0.000001);
    EXPECT_NEAR(-2.0, root(1.0, 3.0, 2.0, -1), 0.000001);
  }

  
  TEST_F(UtilTest, testRoot2b) {
    EXPECT_NEAR(-1.0, root2b(1.0, 1.5, 2.0,  1), 0.000001);
    EXPECT_NEAR(-2.0, root2b(1.0, 1.5, 2.0, -1), 0.000001);
  }

  
  TEST_F(UtilTest, testSign) {
    int r;
    r = sign(-0.5);
    EXPECT_EQ(-1, r);
    r = sign(0.5);
    EXPECT_EQ(1, r);
    r = sign(0.0);
    EXPECT_EQ(1, r);
  }

  TEST_F(UtilTest, testTo_2pi) {
    EXPECT_NEAR(3.141592653589793, to_2pi(Pi), 0.0);
    EXPECT_NEAR(1.0e100, floor(1.0e100), 0.0);
    EXPECT_NEAR(-6, floor(-5.4), 0.0);

	EXPECT_NEAR(0.0, to_2pi(0.0), 0.0);
    EXPECT_NEAR(Pi, to_2pi(Pi), 0.0);
    EXPECT_NEAR(0.0, to_2pi(2*Pi), 0.00001);
    EXPECT_NEAR(Pi, to_2pi(3*Pi), 0.00001);
    EXPECT_NEAR(Pi, to_2pi(-Pi), 0.0);
    EXPECT_NEAR(0.0, to_2pi(-2*Pi), 0.00001);
    EXPECT_NEAR(Pi, to_2pi(-3*Pi), 0.00001);

    EXPECT_NEAR(Pi/2, to_2pi(Pi/2), 0.00001);
    EXPECT_NEAR(3*Pi/2, to_2pi(3*Pi/2), 0.00001);
    EXPECT_NEAR(3*Pi/2, to_2pi(-Pi/2), 0.00001);
    EXPECT_NEAR(Pi/2, to_2pi(-3*Pi/2), 0.00001);

    EXPECT_NEAR(3*Pi/2, to_2pi(11*Pi+Pi/2), 0.00001);
    EXPECT_NEAR(Pi/2, to_2pi(-11*Pi-Pi/2), 0.00001);

    // In Java this gives 2.0625
    EXPECT_NEAR(1.625, to_2pi((5.0000000000001E14)), 0.00001); 			//^^
    EXPECT_NEAR(1.75, to_2pi(6.06024e+13), 0.00001);					//^^
    EXPECT_NEAR(1.1714553645825241E-15, cos(1.5707963267948954), 0.0);
    EXPECT_NEAR(-5.200803293214491E14, -0.8616108740678673*sin(Pi/4) /  1.1714553645825241E-15, 0.0);
    EXPECT_NEAR(5.200803293214478E14, (-1.2877620997631494 - -5.200803293214491E14), 0.0);
    EXPECT_NEAR(1.375, to_2pi(5.200803293214478E14), 0.00001);			//^^

    EXPECT_NEAR(4.625, to_2pi(-5.0000000000001E14), 0.00001);			//^^

  }

  TEST_F(UtilTest, testTo_pi2_cont) {
	  EXPECT_NEAR( 0.0, to_pi2_cont(0.0), 0.0);

	  EXPECT_NEAR( Pi/4, to_pi2_cont(Pi/4), 0.00001);
	  EXPECT_NEAR(-Pi/4, to_pi2_cont(-Pi/4), 0.00001);

	  EXPECT_NEAR( Pi/2, to_pi2_cont(Pi/2), 0.00001);
	  EXPECT_NEAR(-Pi/2, to_pi2_cont(-Pi/2), 0.00001);

	  EXPECT_NEAR( Pi/4, to_pi2_cont(3*Pi/4), 0.00001);
	  EXPECT_NEAR(-Pi/4, to_pi2_cont(-3*Pi/4), 0.00001);

	  EXPECT_NEAR( 0.0, to_pi2_cont(Pi), 0.0);
	  EXPECT_NEAR( 0.0, to_pi2_cont(-Pi), 0.0);

	  EXPECT_NEAR(-Pi/4, to_pi2_cont(5*Pi/4), 0.00001);
	  EXPECT_NEAR( Pi/4, to_pi2_cont(-5*Pi/4), 0.00001);

	  EXPECT_NEAR(-Pi/2, to_pi2_cont(3*Pi/2), 0.00001);
	  EXPECT_NEAR( Pi/2, to_pi2_cont(-3*Pi/2), 0.00001);

	  EXPECT_NEAR(-Pi/4, to_pi2_cont(7*Pi/4), 0.00001);
	  EXPECT_NEAR( Pi/4, to_pi2_cont(-7*Pi/4), 0.00001);

	  EXPECT_NEAR( 0.0, to_pi2_cont(2*Pi), 0.00001);
	  EXPECT_NEAR( 0.0, to_pi2_cont(-2*Pi), 0.00001);


	  EXPECT_NEAR( 0.0, to_pi2_cont(3*Pi), 0.00001);
	  EXPECT_NEAR( 0.0, to_pi2_cont(-3*Pi), 0.00001);
	  EXPECT_NEAR(-Pi/2, to_pi2_cont(11*Pi+Pi/2), 0.00001);
	  EXPECT_NEAR(Pi/2, to_pi2_cont(-11*Pi-Pi/2), 0.00001);
  }

  TEST_F(UtilTest, testTo_pi) {
    EXPECT_NEAR(0.0, to_pi(0.0), 0.0);
    EXPECT_NEAR(Pi, to_pi(Pi), 0.0);
    EXPECT_NEAR(0.0, to_pi(2*Pi), 0.00001);
    EXPECT_NEAR(Pi, to_pi(3*Pi), 0.00001);
    EXPECT_NEAR(Pi, to_pi(-Pi), 0.0);
    EXPECT_NEAR(0.0, to_pi(-2*Pi), 0.00001);
    EXPECT_NEAR(Pi, to_pi(-3*Pi), 0.00001);

    EXPECT_NEAR(Pi/2, to_pi(Pi/2), 0.00001);
    EXPECT_NEAR(-Pi/2, to_pi(3*Pi/2), 0.00001);
    EXPECT_NEAR(-Pi/2, to_pi(-Pi/2), 0.00001);
    EXPECT_NEAR(Pi/2, to_pi(-3*Pi/2), 0.00001);

    EXPECT_NEAR(-Pi/2, to_pi(11*Pi+Pi/2), 0.00001);
    EXPECT_NEAR(Pi/2, to_pi(-11*Pi-Pi/2), 0.00001);
  }

  TEST_F(UtilTest, testTo_360) {
    EXPECT_NEAR(0.0, to_360(0.0), 0.0);
    EXPECT_NEAR(180.0, to_360(180.0), 0.0);
    EXPECT_NEAR(0.0, to_360(2*180.0), 0.00001);
    EXPECT_NEAR(180.0, to_360(3*180.0), 0.00001);
    EXPECT_NEAR(180.0, to_360(-180.0), 0.0);
    EXPECT_NEAR(0.0, to_360(-2*180.0), 0.00001);
    EXPECT_NEAR(180.0, to_360(-3*180.0), 0.00001);

    EXPECT_NEAR(180.0/2, to_360(180.0/2), 0.00001);
    EXPECT_NEAR(3*180.0/2, to_360(3*180.0/2), 0.00001);
    EXPECT_NEAR(3*180.0/2, to_360(-180.0/2), 0.00001);
    EXPECT_NEAR(180.0/2, to_360(-3*180.0/2), 0.00001);

    EXPECT_NEAR(3*180.0/2, to_360(11*180.0+180.0/2), 0.00001);
    EXPECT_NEAR(180.0/2, to_360(-11*180.0-180.0/2), 0.00001);
  }

  TEST_F(UtilTest, testTo_180) {
    EXPECT_NEAR(0.0, to_180(0.0), 0.0);
    EXPECT_NEAR(180.0, to_180(180.0), 0.0);
    EXPECT_NEAR(0.0, to_180(2*180.0), 0.00001);
    EXPECT_NEAR(180.0, to_180(3*180.0), 0.00001);
    EXPECT_NEAR(180.0, to_180(-180.0), 0.0);
    EXPECT_NEAR(0.0, to_180(-2*180.0), 0.00001);
    EXPECT_NEAR(180.0, to_180(-3*180.0), 0.00001);

    EXPECT_NEAR(180.0/2, to_180(180.0/2), 0.00001);
    EXPECT_NEAR(-180.0/2, to_180(3*180.0/2), 0.00001);
    EXPECT_NEAR(-180.0/2, to_180(-180.0/2), 0.00001);
    EXPECT_NEAR(180.0/2, to_180(-3*180.0/2), 0.00001);

    EXPECT_NEAR(-180.0/2, to_180(11*180.0+180.0/2), 0.00001);
    EXPECT_NEAR(180.0/2, to_180(-11*180.0-180.0/2), 0.00001);
  }

TEST_F(UtilTest, clockwise)  {
    double deg0 =   Units::from("deg",   0);
    double deg20 =  Units::from("deg",  20);
    double deg60 =  Units::from("deg",  60);
    double deg179 = Units::from("deg", 179);
    double deg180 = Units::from("deg", 180);
    double deg359 = Units::from("deg", 359);
    double deg360 = Units::from("deg", 360);

    EXPECT_TRUE( Util::clockwise( deg0,  deg0));
    EXPECT_TRUE( Util::clockwise(deg20, deg20));

    EXPECT_TRUE( Util::clockwise( deg0, deg20));
    EXPECT_FALSE(Util::clockwise(deg20,  deg0));

    EXPECT_TRUE( Util::clockwise(deg20, deg60));
    EXPECT_FALSE(Util::clockwise(deg60, deg20));

    EXPECT_TRUE( Util::clockwise(  deg0,deg179));
    EXPECT_FALSE(Util::clockwise(deg179,  deg0));

    EXPECT_TRUE( Util::clockwise(deg179,deg180));
    EXPECT_FALSE(Util::clockwise(deg180,deg179));

    EXPECT_TRUE( Util::clockwise(  deg0,deg180));
    EXPECT_FALSE(Util::clockwise(deg180,  deg0));

    EXPECT_TRUE( Util::clockwise(deg360,deg179));
    EXPECT_FALSE(Util::clockwise(deg179,deg360));

    EXPECT_TRUE( Util::clockwise(deg359,deg360));
    EXPECT_FALSE(Util::clockwise(deg360,deg359));

    EXPECT_TRUE( Util::clockwise(deg359, deg20));
    EXPECT_FALSE(Util::clockwise( deg20, deg359));
 }

TEST_F(UtilTest, turndelta0)  {
    double deg0 =   Units::from("deg",   0);
    double deg20 =  Units::from("deg",  20);
    double deg60 =  Units::from("deg",  60);
    double deg179 = Units::from("deg", 179);
    double deg180 = Units::from("deg", 180);
    double deg359 = Units::from("deg", 359);
    double deg360 = Units::from("deg", 360);

	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg0,deg0)),0.0000001);
	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg20,deg20)),0.0000001);
	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg360,deg360)),0.0000001);

	EXPECT_NEAR(20.0,Units::to("deg",Util::turnDelta(deg0,deg20)),0.0000001);
	EXPECT_NEAR(20.0,Units::to("deg",Util::turnDelta(deg20,deg0)),0.0000001);

	EXPECT_NEAR(40.0,Units::to("deg",Util::turnDelta(deg20,deg60)),0.0000001);
	EXPECT_NEAR(40.0,Units::to("deg",Util::turnDelta(deg60,deg20)),0.0000001);

	EXPECT_NEAR(180.0,Units::to("deg",Util::turnDelta(deg0,deg180)),0.0000001);
	EXPECT_NEAR(180.0,Units::to("deg",Util::turnDelta(deg180,deg0)),0.0000001);

	EXPECT_NEAR(179.0,Units::to("deg",Util::turnDelta(deg0,deg179)),0.0000001);
	EXPECT_NEAR(179.0,Units::to("deg",Util::turnDelta(deg179,deg0)),0.0000001);

	EXPECT_NEAR(21.0,Units::to("deg",Util::turnDelta(deg20,deg359)),0.0000001);
	EXPECT_NEAR(21.0,Units::to("deg",Util::turnDelta(deg359,deg20)),0.0000001);
}

TEST_F(UtilTest, turndelta1)  {
	double deg0 =   Units::from("deg",   0);
	double deg20 =  Units::from("deg",  20);
	double deg60 =  Units::from("deg",  60);
	double deg179 = Units::from("deg", 179);
	double deg180 = Units::from("deg", 180);
	double deg359 = Units::from("deg", 359);
	double deg360 = Units::from("deg", 360);

	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg0,deg0,true)),0.0000001);
	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg20,deg20,true)),0.0000001);
	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg360,deg360,true)),0.0000001);

	EXPECT_NEAR( 20.0,Units::to("deg",Util::turnDelta(deg0,deg20,true)),0.0000001);
	EXPECT_NEAR(340.0,Units::to("deg",Util::turnDelta(deg20,deg0,true)),0.0000001);

	EXPECT_NEAR( 40.0,Units::to("deg",Util::turnDelta(deg20,deg60,true)),0.0000001);
	EXPECT_NEAR(320.0,Units::to("deg",Util::turnDelta(deg60,deg20,true)),0.0000001);

	EXPECT_NEAR(180.0,Units::to("deg",Util::turnDelta(deg0,deg180,true)),0.0000001);
	EXPECT_NEAR(180.0,Units::to("deg",Util::turnDelta(deg180,deg0,true)),0.0000001);

	EXPECT_NEAR(179.0,Units::to("deg",Util::turnDelta(deg0,deg179,true)),0.0000001);
	EXPECT_NEAR(181.0,Units::to("deg",Util::turnDelta(deg179,deg0,true)),0.0000001);

	EXPECT_NEAR(339.0,Units::to("deg",Util::turnDelta(deg20,deg359,true)),0.0000001);
	EXPECT_NEAR( 21.0,Units::to("deg",Util::turnDelta(deg359,deg20,true)),0.0000001);



	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg0,deg0,false)),0.0000001);
	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg20,deg20,false)),0.0000001);
	EXPECT_NEAR(0.0,Units::to("deg",Util::turnDelta(deg360,deg360,false)),0.0000001);

	EXPECT_NEAR(340.0,Units::to("deg",Util::turnDelta(deg0,deg20,false)),0.0000001);
	EXPECT_NEAR( 20.0,Units::to("deg",Util::turnDelta(deg20,deg0,false)),0.0000001);

	EXPECT_NEAR(320.0,Units::to("deg",Util::turnDelta(deg20,deg60,false)),0.0000001);
	EXPECT_NEAR( 40.0,Units::to("deg",Util::turnDelta(deg60,deg20,false)),0.0000001);

	EXPECT_NEAR(180.0,Units::to("deg",Util::turnDelta(deg0,deg180,false)),0.0000001);
	EXPECT_NEAR(180.0,Units::to("deg",Util::turnDelta(deg180,deg0,false)),0.0000001);

	EXPECT_NEAR(181.0,Units::to("deg",Util::turnDelta(deg0,deg179,false)),0.0000001);
	EXPECT_NEAR(179.0,Units::to("deg",Util::turnDelta(deg179,deg0,false)),0.0000001);

	EXPECT_NEAR( 21.0,Units::to("deg",Util::turnDelta(deg20,deg359,false)),0.0000001);
	EXPECT_NEAR(339.0,Units::to("deg",Util::turnDelta(deg359,deg20,false)),0.0000001);
}


TEST_F(UtilTest, testTime)  {
	EXPECT_NEAR(1.0,Util::decimalDegrees("1:0:0"),0.000001);
	EXPECT_NEAR(1.0,Util::decimalDegrees("0:60:0"),0.000001);
	EXPECT_NEAR(1.98,Util::decimalDegrees("0:59:3599"),0.01);

	EXPECT_NEAR(3600.0,Util::parse_time("1:0:0"),0.00001);
	EXPECT_NEAR(60.0,Util::parse_time("0:1:0"),0.00001);
	EXPECT_NEAR(83.0,Util::parse_time("0:1:23"),0.00001);
	EXPECT_NEAR(234.34,Util::parse_time("234.34"),0.00001);
	EXPECT_NEAR(234.34,Util::parse_time("234.34"),0.00001);
	//fpln(" "+Util::hoursMinutesSeconds(9075.0));
	EXPECT_EQ("1:01:40",Util::hoursMinutesSeconds(3700.0));
	EXPECT_EQ("2:31:15",Util::hoursMinutesSeconds(9075.0));

}


/*
 * Class: UnitsTest
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */

#include "Units.h"
#include "format.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;
using namespace std;

class UnitsTest : public ::testing::Test {

protected:
  double PI;

  virtual void SetUp() {
    PI = Pi;
  }

  virtual void checkTight(string header, double x, double y) {
    EXPECT_TRUE(Util::almost_equals(x,y)) << header << " x = " << x << " y = " << y;
  }
	
  virtual void checkLoose(string header, double x, double y) {
    EXPECT_TRUE(Util::almost_equals(x, y, PRECISION5)) << header << " x = " << x << " y = " << y;
  }
	
  virtual void checkUnit(string unit1, string unit2) {
    checkTight("Unit: " + unit1 + " " + unit2, 
	       Units::from(unit1, Units::to(unit2, 100.0)), 
	       100.0);
  }
	
  virtual void checkString(string str1, string str2) {
    EXPECT_TRUE(str1 == str2) << "str1=" + str1 + " str2=" + str2;
  }
};

	
// The test cases

TEST_F(UnitsTest, testNonzero) { // C++ has the "Static Initialization Fiasco", this is a test to check if we have been bitten
  EXPECT_FALSE(0.0 == Units::m);
  EXPECT_FALSE(0.0 == Units::km);
  EXPECT_FALSE(0.0 == Units::nmi);
  EXPECT_FALSE(0.0 == Units::NM );
  EXPECT_FALSE(0.0 == Units::ft );
  EXPECT_FALSE(0.0 == Units::mm );

  EXPECT_FALSE(0.0 == Units::s );
  EXPECT_FALSE(0.0 == Units::min );
  EXPECT_FALSE(0.0 == Units::hour);
  EXPECT_FALSE(0.0 == Units::day);
  EXPECT_FALSE(0.0 == Units::ms);

  EXPECT_FALSE(0.0 == Units::rad);
  EXPECT_FALSE(0.0 == Units::deg);

  EXPECT_FALSE(0.0 == Units::kg);
  EXPECT_FALSE(0.0 == Units::pound_mass);

  EXPECT_FALSE(0.0 == Units::mps);
  EXPECT_FALSE(0.0 == Units::meter_per_second);
  EXPECT_FALSE(0.0 == Units::kph);
  EXPECT_FALSE(0.0 == Units::knot);
  EXPECT_FALSE(0.0 == Units::kn);
  EXPECT_FALSE(0.0 == Units::fpm);

  EXPECT_FALSE(0.0 == Units::meter_per_second2);
  EXPECT_FALSE(0.0 == Units::G);

  EXPECT_FALSE(0.0 == Units::newton);
  EXPECT_FALSE(0.0 == Units::pascal);

  EXPECT_FALSE(0.0 == Units::atm);

  EXPECT_FALSE(0.0 == Units::degreeC);
  EXPECT_FALSE(0.0 == Units::degreeF);
  EXPECT_FALSE(0.0 == Units::K);
  EXPECT_FALSE(0.0 == Units::degreeK);
  EXPECT_FALSE(0.0 == Units::degreeR);
}



TEST_F(UnitsTest, testTemperature) {
  double tempC = Units::from("degreeC", 100.00);
  double tempK = Units::from("degreeK", 373.15);
  double tempR = Units::from("degreeR", 671.67);
  double tempF = Units::from("degreeF", 212.00);
		
  checkTight("Check 'checkTight'", tempC, tempC);
  checkLoose("Check 'checkLoose'", tempC, tempC);

  checkUnit("degreeC", "degreeC");
  checkUnit("degreeK", "K");
  checkUnit("degreeF", "degreeF");
  checkUnit("degreeR", "degreeR");
		
  checkTight("Degree K", Units::to("K", 373.15), tempK);
		
  checkTight("Degree K", tempC, tempK);
  checkTight("Degree R", tempC, tempR);
  checkTight("Degree F", tempC, tempF);

  tempC = Units::from("degreeC", 0.00);
  tempF = Units::from("degreeF", 32.00);
  checkTight("Degree F", tempC, tempF);
}
	
TEST_F(UnitsTest, testDistance) {
		
  double m   = Units::from("m",    500.00000);
  double ft  = Units::from("ft",  1640.41995);
  double in  = Units::from("in", 19685.03937);
  double yd  = Units::from("yard", 546.80665);
  double km  = Units::from("km",     0.50000);
  double mi  = Units::from("mi",     0.31069);
  double nmi = Units::from("nmi",    0.26998);
		
  checkUnit("m",   "meter");
  checkUnit("m",   "metre");
  checkUnit("ft",  "foot");
  checkUnit("in",  "inch");
  checkUnit("mi",  "mile");
  checkUnit("nmi", "nautical_mile");
  checkUnit("NM",  "nmi");
		
  checkLoose("foot", m, ft);
  checkLoose("inch", m, in);
  checkLoose("yard", m, yd);
  checkLoose("mile", m, mi);
  checkLoose("nmi",  m, nmi);
  checkLoose("NM",   m, nmi);
  checkLoose("km",   m, km);
}
	
TEST_F(UnitsTest, testMass) {
  double lbm = Units::from("pound_mass", 32.174042);
  double slug = Units::from("slug",  1.0000);
  double kg   = Units::from("kg",   14.5939);
		
  checkUnit("kilogram", "kg");

  checkLoose("mass1", slug, kg);
  checkLoose("mass2", lbm, kg);
}
	
TEST_F(UnitsTest, testVelocity) {
  double fps  = Units::from("ft/s", 1.00000);
  double mps  = Units::from("m/s",  0.30480);
  double mph  = Units::from("mph",  0.68182);
  double knot = Units::from("knot", 0.59248);
		
  checkTight("mps", fps, mps);
  checkLoose("mph", fps, mph);
  checkLoose("knot", fps, knot);
	
}
	
TEST_F(UnitsTest, testAcceleration) {
  double G    = Units::from("G",  1.00000);
  double fpss = Units::from("ft/s^2", 32.17405);
  double mpss = Units::from("m/s^2",  9.80665);
		
  checkLoose("f/s^2", G, fpss);
  checkLoose("m/s^2", G, mpss);
	
}
    
TEST_F(UnitsTest, testPressure) {
  double atm1 = Units::from(Units::atm,  1.00000);
  double atm2 = Units::from("atm",      1.00000);
  double Pa   = Units::from("Pa",  101325.00);
  // double mmHg = Units::from("mmHg",   759.999820);
//   double inHg = Units::from("inHg",    29.92125);
//   double bar  = Units::from("bar",      1.01325);
//   double mbar = Units::from("mbar",  1013.25);
//   double psi  = Units::from("psi",     14.69595);
//   double psf  = Units::from("psf",   2116.2166);
		
  checkTight("atm",  atm1, atm2);
  checkLoose("Pa",   atm1, Pa);
//   checkLoose("mmHg", atm1, mmHg);
//   checkLoose("inHg", atm1, inHg);
//   checkLoose("bar",  atm1, bar);
//   checkLoose("mbar", atm1, mbar);
//   checkLoose("psi",  atm1, psi);
//   checkLoose("psf",  atm1, psf);
		
}
	
TEST_F(UnitsTest, testAngle) {
  double rad = Units::from("radian",  1.00000);
  double deg = Units::from("degree", 57.29578);
		
  checkLoose("radian", rad, deg);
		
  double degps = Units::from("deg/s", 360.0);
  checkLoose("rad/s", Units::to("rad/s", degps),  2*PI);	
}
    
TEST_F(UnitsTest, testForce) {
  double lbf = Units::from("lbf",  1.00000);
  double N   = Units::from("N",    4.44822);
		
  checkLoose("lbf", lbf, N);
		
}
	
// TEST_F(UnitsTest,  testVolume) {
//   double L   = Units::from("liter",  1.00000);
//   double in3 = Units::from("in^3",  61.023744);
//   double m3  = Units::from("m^3",    0.001);
		
//   checkLoose("in^3", L, in3);
//   checkLoose("m^3", L, m3);
		
//   checkUnit("liter", "litre");
//   checkUnit("liter", "L");
		
// }
	
// TEST_F(UnitsTest,  testElectric) {
//   checkUnit("A", "ampere");
//   checkUnit("W", "watt");
//   checkUnit("ohm", "ohm");  // I know, this doesn't test much
//   checkUnit("F", "farad");
//   checkUnit("C", "coulomb");
		
// }
    
TEST_F(UnitsTest,  testCompatible) {
  EXPECT_TRUE( Units::isCompatible("m", "ft"));		
  EXPECT_FALSE( Units::isCompatible("m", "s"));
  EXPECT_TRUE( Units::isCompatible("m", "in"));
  EXPECT_TRUE( Units::isCompatible("m", "NM"));
  EXPECT_TRUE( Units::isCompatible("m", "yard"));
  EXPECT_TRUE( Units::isCompatible("m", "km"));
  EXPECT_TRUE( Units::isCompatible("m", "nmi"));

  EXPECT_TRUE( Units::isCompatible("kg", "slug"));
  EXPECT_TRUE( Units::isCompatible("kg", "lbm"));

  EXPECT_TRUE( Units::isCompatible("kn", "ft/s"));
  EXPECT_TRUE( Units::isCompatible("kn", "mph"));
  EXPECT_TRUE( Units::isCompatible("kn", "m/s"));

  EXPECT_TRUE( Units::isCompatible("G", "m/s^2"));
  EXPECT_TRUE( Units::isCompatible("G", "ft/s^2"));

  EXPECT_TRUE( Units::isCompatible("atm", "Pa"));

  EXPECT_TRUE( Units::isCompatible("radian", "deg"));
  EXPECT_TRUE( Units::isCompatible("rad/s", "deg/s"));

  EXPECT_TRUE( Units::isCompatible("N", "lbf"));

  EXPECT_TRUE( Units::isCompatible("m", "unspecified"));
  EXPECT_FALSE(Units::isCompatible("m", "quizzle"));
}
	
TEST_F(UnitsTest,  testClean) {
  checkString("feet", Units::clean("feet"));
  checkString("foot", Units::clean("foot"));
  checkString("meter", Units::clean("[meter]"));
  checkString("meter", Units::clean(" [meter]"));
  checkString("meter", Units::clean(" [ meter ] "));
  checkString("unspecified", Units::clean("[nimi]"));
  checkString("unspecified", Units::clean("[][d[f[g][g]fg[d]d"));
  checkString("unspecified", Units::clean("[]"));
  checkString("unspecified", Units::clean("  [    ]       "));
  checkString("unspecified", Units::clean(""));
}


TEST_F(UnitsTest, testDefaultUnits) {
	double m   = Units::from("m",    500.00000);
	double ft  = Units::from("ft",  1640.41995);
	checkLoose("foot", m, ft);


	double ft2  = Units::fromInternal("ft",  "ft", ft);
	double ft3  = Units::fromInternal("ft",  "unspecified", ft);
	double ft4  = Units::fromInternal("unspecified",  "ft", ft);
	double ft5  = Units::fromInternal("unspecified",  "unspecified", ft);
	checkLoose("foot-2", m, ft2);
	checkLoose("foot-3", 152.4, ft3);  // double conversion: 1640/3.28 = 500; 500/3.28 = 152
	checkLoose("foot-4", m, ft4);
	checkLoose("foot-5", m, ft5);
}

TEST_F(UnitsTest, testParse) {
	//
	//  ATTENTION: all of these tests should pass, even the weird ones.  Invariably there are
	//    some regression tests that use EACH of these special cases.
	//

		//
		// Basic parse string
		//
		
		EXPECT_NEAR(10.0, Units::parse("10"), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("   10   "), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10 m"), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10m"), 0.00000001);  
		EXPECT_NEAR( 0.0, Units::parse("kfjdk"), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10 kfjdk"), 0.00000001);
		EXPECT_NEAR( 0.0, Units::parse("10.10.10 [m]"), 0.00000001);
		
		EXPECT_NEAR(1.0, Units::parse("1000 mm kfj"), 0.00000001); 
		EXPECT_NEAR(1.0, Units::parse("1000 mm; kfj"), 0.00000001); 
		EXPECT_NEAR(2.0, Units::parse("2 9"), 0.00000001);
		EXPECT_NEAR(2.0, Units::parse("2 m 9"), 0.00000001);  // warning: this is 2 [unspecified]

		EXPECT_NEAR(1000.0, Units::parse("1000 [m]"), 0.00000001);
		EXPECT_NEAR(1000.0, Units::parse("1 [km]"), 0.00000001);
		EXPECT_NEAR(1000.0, Units::parse("1000000 [mm]"), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10 [m]"), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10[m]"), 0.00000001); 
		EXPECT_NEAR(0.010, Units::parse("10[mm]"), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10[]"), 0.00000001);
		
		EXPECT_NEAR(10.0, Units::parse("10[m/s]"), 0.00000001); 
		EXPECT_NEAR(0.9290304, Units::parse("10[ft^2]"), 0.00000001); 
		EXPECT_NEAR(3.048, Units::parse("10[ft/s]"), 0.00000001); 
		EXPECT_NEAR(3.048, Units::parse("10[feet_per_second]"), 0.00000001); 
		EXPECT_NEAR(Units::from("ft-lbf",10.0), Units::parse("10 ft-lbf"), 0.00000001);
		
		//
		// Parse with a default value
		//
		
		EXPECT_NEAR(10.0, Units::parse("10",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("   10   ",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10 m",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10m",-1.0), 0.00000001);  
		EXPECT_NEAR(-1.0, Units::parse("kfjdk",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("10 kfjdk",-1.0), 0.00000001);
		EXPECT_NEAR(-1.0, Units::parse("10.10.10 [m]",-1.0), 0.00000001);
		EXPECT_NEAR(-1.0, Units::parse("10.10.10 [ft]",-1.0), 0.00000001);

		//
		// Parse with a default unit and a default value
		//
		
		EXPECT_NEAR(10.0, Units::parse("m", "10",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("m", "   10   ",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("ft", "10 m",-1.0), 0.00000001);
		EXPECT_NEAR(10.0, Units::parse("ft", "10m",-1.0), 0.00000001);  
		EXPECT_NEAR(10100.0, Units::parse("ft", "10.10 [km]",-1.0), 0.00000001);

		EXPECT_NEAR(-0.3048, Units::parse("ft", "kfjdk", -1.0), 0.00000001);
		EXPECT_NEAR( 3.048,  Units::parse("ft", "10 kfjdk",-1.0), 0.00000001);
		EXPECT_NEAR(-0.3048, Units::parse("ft", "10.10.10 [m]", -1.0), 0.00000001);
		EXPECT_NEAR(-0.3048, Units::parse("ft", "10.10.10 [km]", -1.0), 0.00000001);
		EXPECT_NEAR(-0.3048, Units::parse("ft", "10.10.10 [ffjkd]",-1.0), 0.00000001);

		EXPECT_NEAR(-1.0, Units::parse("ft", "kfjdk", Units::to("ft",-1.0)), 0.00000001);

		//
		// Test parse with default units
		//
		
		EXPECT_NEAR(1000.0, Units::parse("m", "1000"), 0.00000001);
		EXPECT_NEAR(1.0, Units::parse("mm", "1000"), 0.00000001);
		
		EXPECT_NEAR(1.0, Units::parse("ft", "1000 [mm]"), 0.00000001);

		EXPECT_NEAR(51.4444, Units::parse("knot", "100 [knot]"), 0.0001);
		EXPECT_NEAR(51.4444, Units::parse("knot", "100"), 0.0001);
		EXPECT_NEAR(51.4444, Units::parse("mph", "100 [knot]"), 0.0001);


		
	// EXPECT_NEAR(10.0, Units::parse("10", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("     10", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10     ", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("   10   ", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10 m", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10m", -1.0), 0.00000001);
	// EXPECT_NEAR( -1.0, Units::parse("kfjdk", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10 kfjdk", -1.0), 0.00000001);
	// EXPECT_NEAR(10.1, Units::parse("10.10.10 [m]", -1.0), 0.00000001);  // should be -1.0, C++ regular expression parsing is different than Java, hmmm.

	// EXPECT_NEAR(1.0, Units::parse("1000 mm kfj", -1.0), 0.00000001);
	// EXPECT_NEAR(1.0, Units::parse("1000 mm; kfj", -1.0), 0.00000001);
	// EXPECT_NEAR(2.0, Units::parse("2 9", -1.0), 0.00000001);
	// EXPECT_NEAR(2.0, Units::parse("2 m 9", -1.0), 0.00000001);  // warning: this is 2 [unspecified]

	// EXPECT_NEAR(1000.0, Units::parse("1000 [m]", -1.0), 0.00000001);
	// EXPECT_NEAR(1000.0, Units::parse("1 [km]", -1.0), 0.00000001);
	// EXPECT_NEAR(1000.0, Units::parse("1000000 [mm]", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10 [m]", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10[m]", -1.0), 0.00000001);
	// EXPECT_NEAR(0.010, Units::parse("10[mm]", -1.0), 0.00000001);
	// EXPECT_NEAR(10.0, Units::parse("10[]", -1.0), 0.00000001);

	// EXPECT_NEAR(10.0, Units::parse("10[m/s]", -1.0), 0.00000001);
	// EXPECT_NEAR(0.9290304, Units::parse("10[ft^2]", -1.0), 0.00000001);
	// EXPECT_NEAR(3.048, Units::parse("10[ft/s]", -1.0), 0.00000001);
	// EXPECT_NEAR(3.048, Units::parse("10[feet_per_second]", -1.0), 0.00000001);

	// //EXPECT_NEAR(1000.0, Units::parse("m", "m", "1000"), 0.00000001);
	// //EXPECT_NEAR(1.0, Units::parse("km", "km", "1"), 0.00000001);
	// //EXPECT_NEAR(1000.0, Units::parse("m", "mm", "1"), 0.00000001);
	// //EXPECT_NEAR(1.0, Units::parse("mm", "m", "1000"), 0.00000001);

	// //EXPECT_NEAR(1000.0, Units::parse("m", "mm", "1 [m]"), 0.00000001);
	// //EXPECT_NEAR(1000.0, Units::parse("ft", "mm", "1 [m]"), 0.00000001);
}

TEST_F(UnitsTest, testParseUnits) {
	EXPECT_EQ("unspecified", Units::parseUnits("10"));
	EXPECT_EQ("m", Units::parseUnits("10 m"));
	EXPECT_EQ("m", Units::parseUnits("10m"));
	EXPECT_EQ("m", Units::parseUnits("10 [m]"));
	EXPECT_EQ("unspecified", Units::parseUnits("kfjdk"));
	EXPECT_EQ("unspecified", Units::parseUnits("10 kfjdk"));
	//EXPECT_EQ("mm", Units::parseUnits("1000 mm kfj"));

	EXPECT_EQ("m", Units::parseUnits("1000 [m]"));
	EXPECT_EQ("km", Units::parseUnits("1 [km]"));
	EXPECT_EQ("mm", Units::parseUnits("1000000 [mm]"));
	EXPECT_EQ("mm", Units::parseUnits("1000000 [mm ]"));
	EXPECT_EQ("mm", Units::parseUnits("1000000 [ mm]"));
	EXPECT_EQ("mm", Units::parseUnits("1000000 [ mm ]"));
}













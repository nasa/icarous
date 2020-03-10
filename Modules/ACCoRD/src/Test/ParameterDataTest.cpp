/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


// Uses the Google unit test framework.
#include "Units.h"
#include "ParameterData.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

using namespace larcfm;
using namespace std;

class ParameterDataTest: public ::testing::Test {
public:
	ParameterData pd;

protected:

	void setUp() {
		//pd = new ParameterData();    // not needed in C++
	}
};



TEST_F(ParameterDataTest, testInit) {
	EXPECT_FALSE(pd.contains("x"));
	EXPECT_EQ(0,pd.size());
}



TEST_F(ParameterDataTest, testBasic1) {
	EXPECT_FALSE(pd.contains("x"));

	EXPECT_TRUE(pd.set("x = hello    "));
	EXPECT_TRUE(pd.set("y = 10  "));
	EXPECT_TRUE(pd.set("z = 10   [nmi]"));

	EXPECT_EQ(3,pd.size());

	// check parameters
	EXPECT_TRUE(pd.contains("x"));
	EXPECT_TRUE(pd.contains("y"));
	EXPECT_TRUE(pd.contains("z"));
	EXPECT_FALSE(pd.contains("a"));
	EXPECT_FALSE(pd.contains("Test"));
	EXPECT_EQ("hello",     pd.getString("x"));
	EXPECT_EQ("10",        pd.getString("y"));
	EXPECT_EQ("10   [nmi]", pd.getString("z")); // note, 10__[nmi] is converted to 10_[nmi]
	EXPECT_NEAR(0.0,pd.getValue("x"),0.0);
	EXPECT_NEAR(10.0,pd.getValue("y"),0.0);
	EXPECT_NEAR(10.0,Units::to("nmi", pd.getValue("z")),0.00000001);
	EXPECT_EQ("unspecified", pd.getUnit("x"));
	EXPECT_EQ("unspecified", pd.getUnit("y"));
	EXPECT_EQ("nmi",         pd.getUnit("z"));
	EXPECT_FALSE(pd.getBool("x"));
	EXPECT_FALSE(pd.getBool("y"));
	EXPECT_FALSE(pd.getBool("z"));
}



TEST_F(ParameterDataTest, testBasic2) {
	EXPECT_TRUE(pd.set("x = hello    "));
	EXPECT_TRUE(pd.set("y = 10  "));
	EXPECT_TRUE(pd.set("z = 10   nmi"));


	// check parameters
	EXPECT_TRUE(pd.contains("x"));
	EXPECT_TRUE(pd.contains("y"));
	EXPECT_TRUE(pd.contains("z"));
	EXPECT_FALSE(pd.contains("a"));
	EXPECT_FALSE(pd.contains("Test"));
	EXPECT_EQ("hello",     pd.getString("x"));
	EXPECT_EQ("10",        pd.getString("y"));
	EXPECT_EQ("10   nmi", pd.getString("z")); // note, 10__[nmi] is converted to 10_[nmi]
	EXPECT_NEAR(0.0,pd.getValue("x"),0.0);
	EXPECT_NEAR(10.0,pd.getValue("y"),0.0);
	EXPECT_NEAR(10.0,Units::to("nmi", pd.getValue("z")),0.00000001);
	EXPECT_EQ("unspecified", pd.getUnit("x"));
	EXPECT_EQ("unspecified", pd.getUnit("y"));
	EXPECT_EQ("nmi",         pd.getUnit("z"));
	EXPECT_FALSE(pd.getBool("x"));
	EXPECT_FALSE(pd.getBool("y"));
	EXPECT_FALSE(pd.getBool("z"));
}


TEST_F(ParameterDataTest, testSetAndGet) {
	EXPECT_TRUE(pd.set("n_x = hello      "));
	EXPECT_TRUE(pd.set("y = 10  "));
	EXPECT_TRUE(pd.set("z = 10   nmi"));

	EXPECT_FALSE(pd.contains("Test"));
	EXPECT_FALSE(pd.contains("x"));
	EXPECT_TRUE(pd.contains("n_x"));
	EXPECT_FALSE(pd.contains("x"));
	EXPECT_TRUE(pd.contains("n_x"));
	EXPECT_TRUE(pd.contains("y"));
	EXPECT_TRUE(pd.contains("z"));
	EXPECT_EQ("hello", pd.getString("n_x"));

	EXPECT_NEAR(10.0,pd.getValue("y"),0.0000001);
	EXPECT_EQ("unspecified", pd.getUnit("y"));
	EXPECT_NEAR(18520.0,Units::fromInternal("NM",pd.getUnit("y"),pd.getValue("y")),0.0000001);
	EXPECT_NEAR(18520.0,pd.getValue("z"),0.0000001);
	EXPECT_EQ("nmi",   pd.getUnit("z"));
	EXPECT_NEAR(18520.0,Units::fromInternal("NM",pd.getUnit("z"),pd.getValue("z")),0.0000001);
}


TEST_F(ParameterDataTest, testSetOneString) {
	EXPECT_TRUE(pd.set("z = 10.000000001  [NM]"));
	EXPECT_EQ(1, pd.size());
	EXPECT_TRUE(pd.contains("z"));
	EXPECT_NEAR(Units::from("nmi",10.000000001),pd.getValue("z"),0.00000000001);
	EXPECT_EQ(18520, pd.getInt("z"));
	EXPECT_EQ(18520, pd.getLong("z"));
	EXPECT_EQ("10.000000001  [NM]", pd.getString("z"));
	EXPECT_FALSE(pd.getBool("z"));

	EXPECT_TRUE(pd.set("y=10.000000001  [NM]"));
	EXPECT_EQ(2, pd.size());
	EXPECT_TRUE(pd.contains("y"));
	EXPECT_NEAR(Units::from("nmi",10.000000001),pd.getValue("y"),0.00000000001);
	EXPECT_EQ(18520, pd.getInt("y"));
	EXPECT_EQ(18520, pd.getLong("y"));
	EXPECT_EQ("10.000000001  [NM]", pd.getString("y"));
	EXPECT_FALSE(pd.getBool("y"));

	EXPECT_TRUE(pd.set("z1=10.000000001[NM]"));
	EXPECT_TRUE(pd.contains("z1"));
	EXPECT_NEAR(Units::from("nmi",10.000000001),pd.getValue("z1"),0.00000000001);

	EXPECT_TRUE(pd.set("z2=10.000000001NM"));
	EXPECT_TRUE(pd.contains("z2"));
	EXPECT_NEAR(Units::from("nmi",10.000000001),pd.getValue("z2"),0.00000000001);

	EXPECT_TRUE(pd.set("z3="));
	EXPECT_TRUE(pd.contains("z3"));
	EXPECT_EQ("", pd.getString("z3"));
}


TEST_F(ParameterDataTest, testSetOneStringOverwrite) {
	EXPECT_TRUE(pd.set("d = 2mm"));
	EXPECT_NEAR(0.002,pd.getValue("d"),0.000000001);
	EXPECT_EQ("2mm", pd.getString("d"));

	EXPECT_TRUE(pd.set("d = 4mm"));
	EXPECT_NEAR(0.004,pd.getValue("d"),0.000000001);
	EXPECT_EQ("4mm", pd.getString("d"));

	EXPECT_FALSE(pd.set("d = 6min"));
	pd.setUnitCompatibility(false);

	EXPECT_TRUE(pd.set("d = 6min"));
	EXPECT_NEAR(360.0,pd.getValue("d"),0.000000001);
	EXPECT_EQ("6min", pd.getString("d"));

	EXPECT_TRUE(pd.set("y = 5"));
	EXPECT_TRUE(pd.set("y = 10  nmi"));
	EXPECT_TRUE(pd.set("y = 20"));

	EXPECT_NEAR(20.0,Units::to("NM", pd.getValue("y")),0.0000001);
	EXPECT_EQ("nmi", pd.getUnit("y"));
}


TEST_F(ParameterDataTest, testSetOneStringDefaultUnits) {
	EXPECT_TRUE(pd.set("d = 2mm"));
	EXPECT_TRUE(pd.set("e = 4"));
	EXPECT_TRUE(pd.set("f = 6 [unitless]"));
	EXPECT_NEAR(0.002,pd.getValue("d","m"),0.000000001);
	EXPECT_NEAR(0.002,pd.getValue("d","ft"),0.000000001);
	EXPECT_NEAR(0.002,pd.getValue("d","lbf"),0.000000001);
	EXPECT_NEAR(4.000,pd.getValue("e","m"),0.000000001);
	EXPECT_NEAR(0.004,pd.getValue("e","mm"),0.000000001);
	EXPECT_NEAR(4000.0,pd.getValue("e","km"),0.000000001);
	EXPECT_NEAR(6.000,pd.getValue("f","m"),0.000000001);
	EXPECT_NEAR(6.000,pd.getValue("f","mm"),0.000000001);
	EXPECT_NEAR(6.000,pd.getValue("f","km"),0.000000001);
}


TEST_F(ParameterDataTest, testSetOneStringError) {
	EXPECT_TRUE(pd.set("d = 2  9"));
	EXPECT_NEAR(2.0,pd.getValue("d"),0.000000001);
	EXPECT_EQ("2  9", pd.getString("d"));

	EXPECT_TRUE(pd.set("e = 2  [mn]"));
	EXPECT_EQ("2  [mn]", pd.getString("e"));
	EXPECT_NEAR(2.0,pd.getValue("e"),0.0000001);

	EXPECT_FALSE(pd.set("2  [NM]"));
	EXPECT_FALSE(pd.set("  2"));
	EXPECT_FALSE(pd.set("  =2"));
	EXPECT_FALSE(pd.set(""));
	EXPECT_FALSE(pd.set("  "));
	EXPECT_FALSE(pd.set("a"));
	EXPECT_FALSE(pd.set("a  "));
}


TEST_F(ParameterDataTest, testSetKeyString) {
	EXPECT_TRUE(pd.set("z", "10.000000001  NM"));
	EXPECT_EQ(1, pd.size());
	EXPECT_TRUE(pd.contains("z"));
	EXPECT_NEAR(Units::from("nmi",10.000000001),pd.getValue("z"),0.00000000001);
	EXPECT_EQ(18520, pd.getInt("z"));
	EXPECT_EQ(18520, pd.getLong("z"));
	EXPECT_EQ("10.000000001  NM", pd.getString("z"));
	EXPECT_FALSE(pd.getBool("z"));

	EXPECT_TRUE(pd.set("y","10.000000001  [NM]"));
	EXPECT_EQ(2, pd.size());
	EXPECT_TRUE(pd.contains("y"));
	EXPECT_NEAR(Units::from("nmi",10.000000001),pd.getValue("y"),0.00000000001);
	EXPECT_EQ(18520, pd.getInt("y"));
	EXPECT_EQ(18520, pd.getLong("y"));
	EXPECT_EQ("10.000000001  [NM]", pd.getString("y"));
	EXPECT_FALSE(pd.getBool("y"));

	EXPECT_TRUE(pd.set("d","2  9"));
	EXPECT_EQ("2  9",     pd.getString("d"));
	EXPECT_TRUE(pd.set("e","2  [NM]"));
	EXPECT_EQ("2  [NM]",     pd.getString("e"));
}


TEST_F(ParameterDataTest, testSetKeyStringOverride) {
	EXPECT_TRUE(pd.set("y","5"));
	EXPECT_TRUE(pd.set("y","10  nmi"));
	EXPECT_TRUE(pd.set("y","20"));

	EXPECT_NEAR(20.0,Units::to("NM", pd.getValue("y")),0.0000001);
	EXPECT_EQ("nmi", pd.getUnit("y"));

	EXPECT_FALSE(pd.set("y","10 kg"));
	pd.setUnitCompatibility(false);
	EXPECT_TRUE(pd.set("y","10 kg"));
}


TEST_F(ParameterDataTest, testSetKeyValueCaseSensitive) {
	EXPECT_TRUE(pd.set("d", 2, "mm"));
	EXPECT_NEAR(0.002,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_NEAR(0.002,pd.getValue("d"),0.00000000001);
	EXPECT_NEAR(0.002,pd.getValue("D"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_TRUE(pd.set("d", 3, "m"));
	EXPECT_NEAR(3.00,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("m", pd.getUnit("d"));
	EXPECT_TRUE(pd.setInternal("d", 2, "mm"));
	EXPECT_NEAR(2.000,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_EQ("2000.000000 [mm]", pd.getString("d"));
	EXPECT_TRUE(pd.setInternal("D", 7, "mm"));
	EXPECT_NEAR(7.000,pd.getValue("d"),0.00000000001);
	EXPECT_NEAR(7.000,pd.getValue("D"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));

}



TEST_F(ParameterDataTest, testSetKeyValue) {
	//case insensitive
	EXPECT_TRUE(pd.set("d", 2, "mm"));
	EXPECT_NEAR(0.002,pd.getValue("d"),0.00000000001);
	EXPECT_NEAR(0.002,pd.getValue("D"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_EQ("mm", pd.getUnit("D"));
	EXPECT_TRUE(pd.set("D", 3, "mm"));
	EXPECT_NEAR(0.003,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_TRUE(pd.set("d", 3, "m"));
	EXPECT_NEAR(3.00,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("m", pd.getUnit("d"));
	EXPECT_TRUE(pd.setInternal("d", 2, "mm"));
	EXPECT_NEAR(2.000,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));

}



TEST_F(ParameterDataTest, testSetKeyValuePreserve) {
	EXPECT_TRUE(pd.set("d", 2, "mm"));
	EXPECT_NEAR(0.002,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_TRUE(pd.set("d", 3, "mm"));
	EXPECT_NEAR(0.003,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	EXPECT_TRUE(pd.set("d", 3, "m"));
	EXPECT_NEAR(3.00,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("m", pd.getUnit("d"));
	pd.setPreserveUnits(true);
	EXPECT_TRUE(pd.set("d", 2, "mm"));
	EXPECT_NEAR(0.002,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("m", pd.getUnit("d"));
	pd.setPreserveUnits(false);

	EXPECT_TRUE(pd.setInternal("d", 3, "mm"));
	EXPECT_NEAR(3.000,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	pd.setPreserveUnits(true);
	EXPECT_TRUE(pd.setInternal("d", 5, "km"));
	EXPECT_NEAR(5.0,pd.getValue("d"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("d"));
	pd.setPreserveUnits(false);


	EXPECT_TRUE(pd.set("e", "2"));
	EXPECT_NEAR(2,pd.getValue("e"),0.00000000001);
	EXPECT_EQ("unspecified", pd.getUnit("e"));
	pd.setPreserveUnits(true);
	EXPECT_TRUE(pd.set("e", 3, "mm"));
	EXPECT_NEAR(0.003,pd.getValue("e"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("e"));
	pd.setPreserveUnits(false);

	EXPECT_TRUE(pd.set("f", "2"));
	EXPECT_NEAR(2,pd.getValue("f"),0.00000000001);
	EXPECT_EQ("unspecified", pd.getUnit("f"));
	pd.setPreserveUnits(true);
	EXPECT_TRUE(pd.setInternal("f", 5, "mm"));
	EXPECT_NEAR(5.000,pd.getValue("f"),0.00000000001);
	EXPECT_EQ("mm", pd.getUnit("f"));
	pd.setPreserveUnits(false);
}


TEST_F(ParameterDataTest, testSetKeyValueOverride) {
	EXPECT_TRUE(pd.set("y",5,"unspecified"));
	EXPECT_TRUE(pd.set("y",10,"nmi"));
	EXPECT_TRUE(pd.set("y",20,"unspecified"));

	EXPECT_NEAR(20.0,Units::to("NM", pd.getValue("y")),0.0000001);
	EXPECT_EQ("nmi", pd.getUnit("y"));

	EXPECT_FALSE(pd.set("y",10,"kg"));
	EXPECT_FALSE(pd.setInternal("y",10,"kg"));
	pd.setUnitCompatibility(false);
	EXPECT_TRUE(pd.set("y",10,"kg"));
	EXPECT_TRUE(pd.setInternal("y",12,"kg"));
}

TEST_F(ParameterDataTest, testSetKeyValueOverride2) {
	pd.setPreserveUnits(true);
	EXPECT_TRUE(pd.set("y",5,"unspecified"));
	EXPECT_TRUE(pd.set("y",5,"ft"));
	EXPECT_TRUE(pd.set("y",10,"m"));
	EXPECT_NEAR(10.0, Units::to("m", pd.getValue("y")), 0.0000001);
	EXPECT_EQ("ft", pd.getUnit("y"));
	EXPECT_TRUE(pd.set("y",20,"unspecified"));
	EXPECT_NEAR(20.0, Units::to("ft", pd.getValue("y")), 0.0000001);
	pd.setPreserveUnits(false);
	EXPECT_TRUE(pd.set("y",20,"unspecified"));
	EXPECT_NEAR(20.0, Units::to("ft", pd.getValue("y")), 0.0000001);
	EXPECT_TRUE(pd.set("y",20,"nmi"));
	EXPECT_NEAR(20.0, Units::to("nmi", pd.getValue("y")), 0.0000001);
}

TEST_F(ParameterDataTest, testSetKeyValueOverrideWithString) {
		EXPECT_TRUE(pd.set("y",5,"ft"));
		EXPECT_EQ("ft",pd.getUnit("y"));
		EXPECT_EQ("5.000000 [ft]", pd.getString("y"));
		EXPECT_EQ("ft", pd.getUnit("y"));
		
		// Check if a value with unspecified units uses the existing units in the database
		EXPECT_TRUE(pd.set("y"," 20 "));
		EXPECT_NEAR(20.0, Units::to("ft", pd.getValue("y")), 0.0000001);
		EXPECT_EQ("20", pd.getString("y")); // also check is string is retained

		// Check if another value with unspecified units uses the existing units in the database
		EXPECT_TRUE(pd.set("y",30,"unspecified"));
		EXPECT_NEAR(30.0, Units::to("ft", pd.getValue("y")), 0.0000001);
		EXPECT_EQ("30.000000", pd.getString("y")); // also check if string is retained

		// Now lets try with an complex value, with unspecified units.
		EXPECT_TRUE(pd.set("y","10,20,30"));
		EXPECT_EQ("ft",pd.getUnit("y"));
		EXPECT_NEAR(10.0, Units::to("ft", pd.getValue("y")), 0.0000001);
		EXPECT_EQ("10,20,30", pd.getString("y"));
		
		//OK now lets try complex values from the start
		
		EXPECT_TRUE(pd.set("z","aa,bb,cc "));
		EXPECT_EQ("unspecified",pd.getUnit("z"));
		EXPECT_NEAR(0.0, Units::to("ft", pd.getValue("z")), 0.0000001);
		EXPECT_EQ("aa,bb,cc", pd.getString("z"));
		
		EXPECT_TRUE(pd.set("z","  10 [m],20 [ft] , 30 [mm]"));
		EXPECT_EQ("m",pd.getUnit("z"));
		EXPECT_NEAR(10.0, Units::to("m", pd.getValue("z")), 0.0000001);
		EXPECT_EQ("10 [m],20 [ft] , 30 [mm]", pd.getString("z"));

		// check if double value and units change with a list.  By change, I mean change 
		// from last call where they were set to '10' and 'm'
		EXPECT_TRUE(pd.set("z","  50 [mm],60 [ft] , 70 [mm]  "));
		EXPECT_EQ("mm",pd.getUnit("z"));
		EXPECT_NEAR(0.05, Units::to("m", pd.getValue("z")), 0.0000001);
		EXPECT_EQ("50 [mm],60 [ft] , 70 [mm]", pd.getString("z"));

	}



TEST_F(ParameterDataTest, testSetKeyBool1) {
	//case sensitive
	EXPECT_TRUE(pd.setBool("d",true));
	EXPECT_TRUE(pd.getBool("d"));
	EXPECT_EQ("true", pd.getString("d"));
	EXPECT_TRUE(pd.setBool("D",false));
	EXPECT_FALSE(pd.getBool("d"));
	EXPECT_FALSE(pd.getBool("D"));
	EXPECT_EQ("false", pd.getString("D"));

	EXPECT_TRUE(pd.setBool("e",true));
	EXPECT_TRUE(pd.getBool("e"));
	EXPECT_EQ("true", pd.getString("e"));

	EXPECT_TRUE(pd.setBool("f",false));
	EXPECT_FALSE(pd.getBool("f"));
	EXPECT_EQ("false", pd.getString("f"));
}



TEST_F(ParameterDataTest, testSetKeyBool2) {
	EXPECT_TRUE(pd.setBool("d",true));
	EXPECT_TRUE(pd.getBool("d"));
	EXPECT_EQ("true", pd.getString("d"));
	EXPECT_TRUE(pd.setBool("D",false));
	EXPECT_FALSE(pd.getBool("d"));
	EXPECT_FALSE(pd.getBool("D"));
	EXPECT_EQ("false", pd.getString("D"));

	EXPECT_TRUE(pd.setBool("e",true));
	EXPECT_TRUE(pd.getBool("e"));
	EXPECT_EQ("true", pd.getString("e"));

	EXPECT_TRUE(pd.setBool("f",false));
	EXPECT_FALSE(pd.getBool("f"));
	EXPECT_EQ("false", pd.getString("f"));
}



TEST_F(ParameterDataTest, testSetKeyBoolOverride) {
	EXPECT_TRUE(pd.setBool("d",true));
	EXPECT_FALSE(pd.set("d", 10, "NM"));
}

TEST_F(ParameterDataTest, testList) {
	EXPECT_TRUE(pd.set("d = 2, 4, 1, 5, 6"));
	vector<string> ret = pd.getListString("d");
	EXPECT_EQ((unsigned long)5,ret.size());
	EXPECT_EQ("2", ret[0]);
	EXPECT_EQ("4", ret[1]);
	EXPECT_EQ("1", ret[2]);
	EXPECT_EQ("5", ret[3]);
	EXPECT_EQ("6", ret[4]);

	EXPECT_TRUE(pd.set("e = 2mm, 4 m, 5  ft, 6 [ km]"));
	ret = pd.getListString("e");
	EXPECT_EQ((unsigned long)4,ret.size());
	EXPECT_EQ("2mm", ret[0]);
	EXPECT_EQ("4 m", ret[1]);
	EXPECT_EQ("5  ft", ret[2]);
	EXPECT_EQ("6 [ km]", ret[3]);

	EXPECT_NEAR(Units::from("mm",2), Units::parse(ret[0]), 0.00000001);
	EXPECT_NEAR(Units::from("m",4), Units::parse(ret[1]), 0.00000001);
	EXPECT_NEAR(Units::from("ft",5), Units::parse(ret[2]), 0.00000001);
	EXPECT_NEAR(Units::from("km",6), Units::parse(ret[3]), 0.00000001);

	vector<double> ret2 = pd.getListDouble("d");
	EXPECT_EQ((unsigned long)5,ret2.size());

	EXPECT_NEAR(2, ret2[0], 0.00000001);
	EXPECT_NEAR(4, ret2[1], 0.00000001);
	EXPECT_NEAR(1, ret2[2], 0.00000001);
	EXPECT_NEAR(5, ret2[3], 0.00000001);
	EXPECT_NEAR(6, ret2[4], 0.00000001);

	ret2 = pd.getListDouble("e");
	EXPECT_EQ((unsigned long)4,ret2.size());
	EXPECT_NEAR(Units::from("mm",2), ret2[0], 0.00000001);
	EXPECT_NEAR(Units::from("m",4), ret2[1], 0.00000001);
	EXPECT_NEAR(Units::from("ft",5), ret2[2], 0.00000001);
	EXPECT_NEAR(Units::from("km",6), ret2[3], 0.00000001);


	EXPECT_TRUE(pd.set("f = x, y, t, u, v"));
	vector<bool> ret3 = pd.getListBool("f");
	ret = pd.getListString("f");
	EXPECT_EQ((unsigned long)5,ret3.size());

	EXPECT_FALSE(ret3[0]);
	EXPECT_FALSE(ret3[1]);
	EXPECT_TRUE(ret3[2]);
	EXPECT_FALSE(ret3[3]);
	EXPECT_FALSE(ret3[4]);


	vector<int> ret4 = pd.getListInteger("d");
	EXPECT_EQ((unsigned long)5,ret4.size());

	EXPECT_EQ(2, (int)ret4[0]);
	EXPECT_EQ(4, (int)ret4[1]);
	EXPECT_EQ(1, (int)ret4[2]);
	EXPECT_EQ(5, (int)ret4[3]);
	EXPECT_EQ(6, (int)ret4[4]);


	EXPECT_TRUE(pd.set("g = 2, 4; 1, 5, 6"));
	ret = pd.getListString("g");
	EXPECT_EQ((unsigned long)5,ret.size());
	EXPECT_EQ("2", ret[0]);
	EXPECT_EQ("4", ret[1]);
	EXPECT_EQ("1", ret[2]);
	EXPECT_EQ("5", ret[3]);
	EXPECT_EQ("6", ret[4]);
}

TEST_F(ParameterDataTest, testListOddballs) {
	EXPECT_TRUE(pd.set("d = ,,,"));
	vector<string> ret = pd.getListString("d");
	//fpln(" qqq "+list2str(ret,", "));
	EXPECT_EQ((unsigned long)4,ret.size());
	EXPECT_EQ("", ret[0]);
	EXPECT_EQ("", ret[1]);
	EXPECT_EQ("", ret[2]);
	EXPECT_EQ("", ret[3]);

	EXPECT_TRUE(pd.set("e = , 4, ,,"));
	ret = pd.getListString("e");
	EXPECT_EQ((unsigned long)5,ret.size());
	EXPECT_EQ("", ret[0]);
	EXPECT_EQ("4", ret[1]);
	EXPECT_EQ("", ret[2]);
	EXPECT_EQ("", ret[3]);
	EXPECT_EQ("", ret[4]);

	vector<double> ret2 = pd.getListDouble("d");
	EXPECT_EQ((unsigned long)4,ret2.size());

	EXPECT_NEAR(0.0, ret2[0], 0.0);
	EXPECT_NEAR(0.0, ret2[1], 0.0);
	EXPECT_NEAR(0.0, ret2[2], 0.0);
	EXPECT_NEAR(0.0, ret2[3], 0.0);

	ret2 = pd.getListDouble("e");
	EXPECT_EQ((unsigned long)5,ret2.size());
	EXPECT_NEAR(0.0, ret2[0], 0.0);
	EXPECT_NEAR(4.0, ret2[1], 0.0);
	EXPECT_NEAR(0.0, ret2[2], 0.0);
	EXPECT_NEAR(0.0, ret2[3], 0.0);
	EXPECT_NEAR(0.0, ret2[4], 0.0);
}

//
// Not implemented in C++
//
//TEST_F(ParameterDataTest, testParseArguments) {
//	String[] args = {"-Px=10mm", "-Phello=", "-Pstuff", "test", "-Py=10min", "-Py=15lbf", "-P"};
//	List<String> ret;
//	pd.setUnitCompatibility(false);
//	ret = pd.parseArguments("-P",args);
//	EXPECT_EQ(3, pd.size());
//	EXPECT_TRUE(pd.contains("x"));
//	EXPECT_TRUE(pd.contains("y"));
//	EXPECT_TRUE(pd.contains("hello"));
//	EXPECT_FALSE(pd.contains("stuff"));
//
//	EXPECT_NEAR(Units::from("mm",10.0),pd.getValue("x"),0.00000000001);
//	EXPECT_EQ("mm", pd.getUnit("x"));
//	EXPECT_NEAR(Units::from("lbf",15.0),pd.getValue("y"),0.00000000001);
//	EXPECT_EQ("lbf", pd.getUnit("y"));
//	EXPECT_EQ("",pd.getString("hello"));
//
//	EXPECT_EQ(1, ret.size());
//	EXPECT_EQ("test", ret[0));
//
//}

TEST_F(ParameterDataTest, testEntryOrder) {
	pd.set("key = 35 mm");  // entry 0, alpha 1
	pd.set("moo = 37");     // entry 1, alpha 2
	pd.set("abc = xyz");    // entry 2, alpha 0
	pd.set("xyz = true");   // entry 3, alpha 4
	pd.set("key = 95 nmi"); // entry 0, alpha 1
	pd.set("roo = blue");   // entry 4, alpha 3

	// entry order
	std::vector<std::string> keys = pd.getKeyListEntryOrder();
	EXPECT_EQ((unsigned long)5, keys.size());
	EXPECT_NEAR(Units::from("nmi", 95), pd.getValue(keys[0]), 0.001);
	EXPECT_TRUE(pd.getBool(keys[3]));

	// lexical order (note that this is case insensitive!)
	std::vector<std::string> keys2 = pd.getKeyList();
	EXPECT_NEAR(Units::from("nmi", 95), pd.getValue(keys2[1]), 0.001);
	EXPECT_TRUE(pd.getBool(keys2[4]));

	pd.set("AAA = 8");     // entry 5, alpha 0 (all alpha shift 1)
	keys = pd.getKeyListEntryOrder();
	keys2 = pd.getKeyList();
	EXPECT_NEAR(Units::from("nmi", 95), pd.getValue(keys[0]), 0.001);
	EXPECT_NEAR(Units::from("nmi", 95), pd.getValue(keys2[2]), 0.001);

}




/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect4.h"
#include "Units.h"
#include "SeparatedInput.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

using namespace larcfm;
using namespace std;

class SeparatedInputTest : public ::testing::Test {

public:
  istringstream* iss1;
  istringstream* iss2;
  istringstream* iss3;
  istringstream* iss4;

protected:

  virtual void SetUp() {
    iss1 = new istringstream("\n\n# Test\n\n  #Test = 3\n\nx = hello    \ny = 10\nz    = 10  [nmi]\ncol1 col2, Col3\n nmi ft jfjf\n 1 2 3\n 4 5 6", istringstream::in);
    iss2 = new istringstream("\n\n# Test\n\n  #Test = 3\n\nx = hello    \ny = 10\nz    = 10  [nmi]\ncol1 col2, Col3\n nmi ft jfjf\n 1 2 3\n 4 5 6\n", istringstream::in);
	iss3 = new istringstream("\n\n# Test\n\n  #Test = 3\n\nx = hello    \ny = 10\nz    = 10  [nmi]\ncol1 col2, Col3\n [nmi] [ft] [jfjf]\n 1 2 3\n 4 5 6\n");
	iss4 = new istringstream("\n\n# Test\n\n  #Test = 3\n\nx = hello    \ny = 10\nz    = 10  [nmi]\ncol1 col2, Col3\n 1 2 3\n 4 5 6\n");
  }
};
	

TEST_F(SeparatedInputTest, testComments) {
	SeparatedInput si = SeparatedInput(new istringstream("# Test\n\n  x = 3 [ft] # 5 [ft]\n"));
	EXPECT_FALSE(si.hasError());
	EXPECT_FALSE(si.getParametersRef().contains("x"));

	EXPECT_TRUE(si.readLine());  // true would mean EOF
	EXPECT_FALSE(si.hasError());


	// check parameters
	EXPECT_TRUE(si.getParametersRef().contains("x"));
	EXPECT_EQ("3 [ft]",     si.getParametersRef().getString("x"));
	EXPECT_NEAR(Units::from("ft",3),     si.getParametersRef().getValue("x"), 0.0);
}


TEST_F(SeparatedInputTest, testBasic1) {
  SeparatedInput si(iss1);
  EXPECT_FALSE(si.hasError());
  EXPECT_FALSE(si.getParametersRef().contains("x"));
  EXPECT_FALSE(si.readLine());  // true would mean EOF
  EXPECT_FALSE(si.hasError());
		
  // check parameters
  EXPECT_TRUE(si.getParametersRef().contains("x"));
  EXPECT_TRUE(si.getParametersRef().contains("y"));
  EXPECT_TRUE(si.getParametersRef().contains("z"));
  EXPECT_FALSE(si.getParametersRef().contains("a"));
  EXPECT_FALSE(si.getParametersRef().contains("Test"));
  EXPECT_EQ("hello",     si.getParametersRef().getString("x"));
  EXPECT_EQ("10",        si.getParametersRef().getString("y"));
  EXPECT_EQ("10  [nmi]", si.getParametersRef().getString("z")); // note, 10__[nmi] is converted to 10_[nmi]
  EXPECT_NEAR( 0.0,     si.getParametersRef().getValue("x"), 0.0);
  EXPECT_NEAR(10.0,     si.getParametersRef().getValue("y"), 0.0);
  EXPECT_NEAR(10.0,     Units::to("nmi", si.getParametersRef().getValue("z")), 0.00000001);
  EXPECT_EQ("unspecified", si.getParametersRef().getUnit("x"));
  EXPECT_EQ("unspecified", si.getParametersRef().getUnit("y"));
  EXPECT_EQ("nmi",         si.getParametersRef().getUnit("z"));
  EXPECT_FALSE(si.getParametersRef().getBool("x"));
  EXPECT_FALSE(si.getParametersRef().getBool("y"));
  EXPECT_FALSE(si.getParametersRef().getBool("z"));
  EXPECT_FALSE(si.hasError());
		
  // Check headings
  EXPECT_EQ( 0, si.findHeading("col1"));
  EXPECT_EQ( 1, si.findHeading("col2"));
  EXPECT_EQ( 2, si.findHeading("Col3"));
  EXPECT_EQ(-1, si.findHeading("col3"));
  EXPECT_EQ("nmi", si.getUnit(0));
  EXPECT_EQ("ft", si.getUnit(1));
  EXPECT_EQ("unspecified", si.getUnit(2));
  EXPECT_FALSE(si.hasError());
		
  // Check values for first line
  EXPECT_NEAR(1.0,     Units::to("nmi", si.getColumn(0)), 0.00000001);
  EXPECT_NEAR(2.0,     Units::to("ft",  si.getColumn(1)), 0.00000001);
  EXPECT_NEAR(3.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
  EXPECT_EQ("1", si.getColumnString(0));
  EXPECT_EQ("2", si.getColumnString(1));
  EXPECT_EQ("3", si.getColumnString(2)); 
  EXPECT_FALSE(si.hasError());

  // Check values for second line
  EXPECT_FALSE(si.readLine());  // true would mean EOF
  EXPECT_FALSE(si.hasError());
  EXPECT_NEAR(4.0,     Units::to("nmi", si.getColumn(0)), 0.00000001);
  EXPECT_NEAR(5.0,     Units::to("ft",  si.getColumn(1)), 0.00000001);
  EXPECT_NEAR(6.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
  EXPECT_EQ("4", si.getColumnString(0));
  EXPECT_EQ("5", si.getColumnString(1));
  EXPECT_EQ("6", si.getColumnString(2)); 
  EXPECT_FALSE(si.hasError());

  EXPECT_TRUE(si.readLine());  // indicates EOF
}

TEST_F(SeparatedInputTest, testBasic2) {
  SeparatedInput si(iss2);
  EXPECT_FALSE(si.hasError());
  EXPECT_FALSE(si.getParametersRef().contains("x"));
  EXPECT_FALSE(si.readLine());  // true would mean EOF
  EXPECT_FALSE(si.hasError());
		
  // check parameters
  EXPECT_TRUE(si.getParametersRef().contains("x"));
  EXPECT_TRUE(si.getParametersRef().contains("y"));
  EXPECT_TRUE(si.getParametersRef().contains("z"));
  EXPECT_FALSE(si.getParametersRef().contains("a"));
  EXPECT_FALSE(si.getParametersRef().contains("Test"));
  EXPECT_EQ("hello",     si.getParametersRef().getString("x"));
  EXPECT_EQ("10",        si.getParametersRef().getString("y"));
  EXPECT_EQ("10  [nmi]", si.getParametersRef().getString("z")); // note, 10__[nmi] is converted to 10_[nmi]
  EXPECT_NEAR( 0.0,     si.getParametersRef().getValue("x"), 0.0);
  EXPECT_NEAR(10.0,     si.getParametersRef().getValue("y"), 0.0);
  EXPECT_NEAR(10.0,     Units::to("nmi", si.getParametersRef().getValue("z")), 0.00000001);
  EXPECT_EQ("unspecified", si.getParametersRef().getUnit("x"));
  EXPECT_EQ("unspecified", si.getParametersRef().getUnit("y"));
  EXPECT_EQ("nmi",         si.getParametersRef().getUnit("z"));
  EXPECT_FALSE(si.getParametersRef().getBool("x"));
  EXPECT_FALSE(si.getParametersRef().getBool("y"));
  EXPECT_FALSE(si.getParametersRef().getBool("z"));
  EXPECT_FALSE(si.hasError());
		
  // Check headings
  EXPECT_EQ( 0, si.findHeading("col1"));
  EXPECT_EQ( 1, si.findHeading("col2"));
  EXPECT_EQ( 2, si.findHeading("Col3"));
  EXPECT_EQ(-1, si.findHeading("col3"));
  EXPECT_EQ("nmi", si.getUnit(0));
  EXPECT_EQ("ft", si.getUnit(1));
  EXPECT_EQ("unspecified", si.getUnit(2));
  EXPECT_FALSE(si.hasError());
		
  // Check values for first line
  EXPECT_NEAR(1.0,     Units::to("nmi", si.getColumn(0)), 0.00000001);
  EXPECT_NEAR(2.0,     Units::to("ft",  si.getColumn(1)), 0.00000001);
  EXPECT_NEAR(3.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
  EXPECT_EQ("1", si.getColumnString(0));
  EXPECT_EQ("2", si.getColumnString(1));
  EXPECT_EQ("3", si.getColumnString(2)); 
  EXPECT_FALSE(si.hasError());

  // Check values for second line
  EXPECT_FALSE(si.readLine());  // true would mean EOF
  EXPECT_FALSE(si.hasError());
  EXPECT_NEAR(4.0,     Units::to("nmi", si.getColumn(0)), 0.00000001);
  EXPECT_NEAR(5.0,     Units::to("ft",  si.getColumn(1)), 0.00000001);
  EXPECT_NEAR(6.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
  EXPECT_EQ("4", si.getColumnString(0));
  EXPECT_EQ("5", si.getColumnString(1));
  EXPECT_EQ("6", si.getColumnString(2)); 
  EXPECT_FALSE(si.hasError());

  EXPECT_TRUE(si.readLine());  // indicates EOF
}

TEST_F(SeparatedInputTest, testBasic3) {
	SeparatedInput si(iss3);
	EXPECT_FALSE(si.hasError());
	EXPECT_FALSE(si.getParametersRef().contains("x"));

	EXPECT_FALSE(si.readLine());  // true would mean EOF
	EXPECT_FALSE(si.hasError());

	// check parameters
	EXPECT_TRUE(si.getParametersRef().contains("x"));
	EXPECT_TRUE(si.getParametersRef().contains("y"));
	EXPECT_TRUE(si.getParametersRef().contains("z"));
	EXPECT_FALSE(si.getParametersRef().contains("a"));
	EXPECT_FALSE(si.getParametersRef().contains("Test"));
	EXPECT_EQ("hello",     si.getParametersRef().getString("x"));
	EXPECT_EQ("10",        si.getParametersRef().getString("y"));
	EXPECT_EQ("10  [nmi]", si.getParametersRef().getString("z")); // note, 10__[nmi] is converted to 10_[nmi]
	EXPECT_NEAR( 0.0,     si.getParametersRef().getValue("x"), 0.0);
	EXPECT_NEAR(10.0,     si.getParametersRef().getValue("y"), 0.0);
	EXPECT_NEAR(10.0,     Units::to("nmi", si.getParametersRef().getValue("z")), 0.00000001);
	EXPECT_EQ("unspecified", si.getParametersRef().getUnit("x"));
	EXPECT_EQ("unspecified", si.getParametersRef().getUnit("y"));
	EXPECT_EQ("nmi",         si.getParametersRef().getUnit("z"));
	EXPECT_FALSE(si.getParametersRef().getBool("x"));
	EXPECT_FALSE(si.getParametersRef().getBool("y"));
	EXPECT_FALSE(si.getParametersRef().getBool("z"));
	EXPECT_FALSE(si.hasError());

	// Check headings
	EXPECT_EQ( 0, si.findHeading("col1"));
	EXPECT_EQ( 1, si.findHeading("col2"));
	EXPECT_EQ( 2, si.findHeading("Col3"));
	EXPECT_EQ(-1, si.findHeading("col3"));
	EXPECT_EQ("nmi", si.getUnit(0));
	EXPECT_EQ("ft", si.getUnit(1));
	EXPECT_EQ("unspecified", si.getUnit(2));
	EXPECT_EQ("unspecified", si.getUnit(3));   // note: there is no column number 3
	EXPECT_FALSE(si.hasError());

	// Check values for first line
	EXPECT_NEAR(1.0,     Units::to("nmi", si.getColumn(0)), 0.00000001);
	EXPECT_NEAR(2.0,     Units::to("ft",  si.getColumn(1)), 0.00000001);
	EXPECT_NEAR(3.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
	EXPECT_EQ("1", si.getColumnString(0));
	EXPECT_EQ("2", si.getColumnString(1));
	EXPECT_EQ("3", si.getColumnString(2));
	EXPECT_FALSE(si.hasError());

	// Check values for second line
	EXPECT_FALSE(si.readLine());  // true would mean EOF
	EXPECT_FALSE(si.hasError());
	EXPECT_NEAR(4.0,     Units::to("nmi", si.getColumn(0)), 0.00000001);
	EXPECT_NEAR(5.0,     Units::to("ft",  si.getColumn(1)), 0.00000001);
	EXPECT_NEAR(6.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
	EXPECT_EQ("4", si.getColumnString(0));
	EXPECT_EQ("5", si.getColumnString(1));
	EXPECT_EQ("6", si.getColumnString(2));
	EXPECT_FALSE(si.hasError());

	EXPECT_TRUE(si.readLine());  // indicates EOF
}

TEST_F(SeparatedInputTest, testBasic4) {
	SeparatedInput si(iss4);
	EXPECT_FALSE(si.hasError());
	EXPECT_FALSE(si.getParametersRef().contains("x"));

	EXPECT_FALSE(si.readLine());  // true would mean EOF
	EXPECT_FALSE(si.hasError());

	// check parameters
	EXPECT_TRUE(si.getParametersRef().contains("x"));
	EXPECT_TRUE(si.getParametersRef().contains("y"));
	EXPECT_TRUE(si.getParametersRef().contains("z"));
	EXPECT_FALSE(si.getParametersRef().contains("a"));
	EXPECT_FALSE(si.getParametersRef().contains("Test"));
	EXPECT_EQ("hello",     si.getParametersRef().getString("x"));
	EXPECT_EQ("10",        si.getParametersRef().getString("y"));
	EXPECT_EQ("10  [nmi]", si.getParametersRef().getString("z")); // note, 10__[nmi] is converted to 10_[nmi]
	EXPECT_NEAR( 0.0,     si.getParametersRef().getValue("x"), 0.0);
	EXPECT_NEAR(10.0,     si.getParametersRef().getValue("y"), 0.0);
	EXPECT_NEAR(10.0,     Units::to("nmi", si.getParametersRef().getValue("z")), 0.00000001);
	EXPECT_EQ("unspecified", si.getParametersRef().getUnit("x"));
	EXPECT_EQ("unspecified", si.getParametersRef().getUnit("y"));
	EXPECT_EQ("nmi",         si.getParametersRef().getUnit("z"));
	EXPECT_FALSE(si.getParametersRef().getBool("x"));
	EXPECT_FALSE(si.getParametersRef().getBool("y"));
	EXPECT_FALSE(si.getParametersRef().getBool("z"));
	EXPECT_FALSE(si.hasError());

	// Check headings
	EXPECT_EQ( 0, si.findHeading("col1"));
	EXPECT_EQ( 1, si.findHeading("col2"));
	EXPECT_EQ( 2, si.findHeading("Col3"));
	EXPECT_EQ(-1, si.findHeading("col3"));
	EXPECT_EQ("unspecified", si.getUnit(0));   // Note: no units are defined
	EXPECT_EQ("unspecified", si.getUnit(1));
	EXPECT_EQ("unspecified", si.getUnit(2));
	EXPECT_EQ("unspecified", si.getUnit(3));
	EXPECT_FALSE(si.hasError());

	// Check values for first line
	EXPECT_NEAR(1.0,     Units::to("unspecified", si.getColumn(0)), 0.00000001);
	EXPECT_NEAR(2.0,     Units::to("unspecified", si.getColumn(1)), 0.00000001);
	EXPECT_NEAR(3.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
	EXPECT_EQ("1", si.getColumnString(0));
	EXPECT_EQ("2", si.getColumnString(1));
	EXPECT_EQ("3", si.getColumnString(2));
	EXPECT_FALSE(si.hasError());

	// Check values for second line
	EXPECT_FALSE(si.readLine());  // true would mean EOF
	EXPECT_FALSE(si.hasError());
	EXPECT_NEAR(4.0,     Units::to("unspecified", si.getColumn(0)), 0.00000001);
	EXPECT_NEAR(5.0,     Units::to("unspecified", si.getColumn(1)), 0.00000001);
	EXPECT_NEAR(6.0,     Units::to("unspecified", si.getColumn(2)), 0.00000001);
	EXPECT_EQ("4", si.getColumnString(0));
	EXPECT_EQ("5", si.getColumnString(1));
	EXPECT_EQ("6", si.getColumnString(2));
	EXPECT_FALSE(si.hasError());

	EXPECT_TRUE(si.readLine());  // indicates EOF
}


	TEST_F(SeparatedInputTest, testSingleColumn) {
		istringstream* ii = new istringstream("y \n 3 \n 5");
			SeparatedInput si(ii);
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.readLine());  // there is no "data" in this file
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("x"));
		EXPECT_FALSE(si.getParametersRef().contains("y"));
		EXPECT_FALSE(si.getParametersRef().contains("y2"));
		EXPECT_FALSE(si.getParametersRef().contains("z"));
		
		EXPECT_EQ( 0, si.findHeading("y"));
		EXPECT_EQ(-1, si.findHeading("x"));
		EXPECT_EQ("unspecified", si.getUnit(0));   // Note: no units are defined
		EXPECT_FALSE(si.hasError());
		EXPECT_NEAR(3.0,Units::to("unspecified", si.getColumn(0)),0.00000001);
	}

	
	TEST_F(SeparatedInputTest, testEmptyColumns) {
		istringstream* ii = new istringstream("col1, col2, Col3\n 1, 2, 3\n 4,,6\n");
			SeparatedInput si(ii);
			//si.setColumnDelimiters(",\\s*");   // something like this is needed to read empty columns
		si.setColumnDelimiters(", *");   // something like this is needed to read empty columns
		
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("x"));
		
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		// Check headings
		EXPECT_EQ( 0, si.findHeading("col1"));
		EXPECT_EQ( 1, si.findHeading("col2"));
		EXPECT_EQ( 2, si.findHeading("Col3"));
		EXPECT_EQ(-1, si.findHeading("col3"));
		EXPECT_EQ("unspecified", si.getUnit(0));   // Note: no units are defined
		EXPECT_EQ("unspecified", si.getUnit(1));
		EXPECT_EQ("unspecified", si.getUnit(2));
		EXPECT_EQ("unspecified", si.getUnit(3));   
		EXPECT_FALSE(si.hasError());
		
		// Check values for first line
		EXPECT_NEAR(1.0,Units::to("unspecified", si.getColumn(0)),0.00000001);
		EXPECT_NEAR(2.0,Units::to("unspecified", si.getColumn(1)),0.00000001);
		EXPECT_NEAR(3.0,Units::to("unspecified", si.getColumn(2)),0.00000001);
		EXPECT_EQ("1", si.getColumnString(0));
		EXPECT_EQ("2", si.getColumnString(1));
		EXPECT_EQ("3", si.getColumnString(2)); 
		EXPECT_FALSE(si.hasError());

		// Check values for second line
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_NEAR(4.0,Units::to("unspecified", si.getColumn(0)),0.00000001);
		EXPECT_NEAR(0.0,Units::to("unspecified", si.getColumn(1)),0.00000001);
		EXPECT_NEAR(6.0,Units::to("unspecified", si.getColumn(2)),0.00000001);
		EXPECT_EQ("4", si.getColumnString(0));
		EXPECT_EQ("", si.getColumnString(1));
		EXPECT_EQ("6", si.getColumnString(2)); 
		EXPECT_FALSE(si.hasError());

		EXPECT_TRUE(si.readLine());  // indicates EOF
	}
	

	TEST_F(SeparatedInputTest, testParameter) {
	istringstream* ii = new istringstream("\n\n# Test\n\n  #Test = 3\n\nn_x = hello    \ny = 10\nz    = 10  [nmi]\ncol1 col2, Col3\n nmi ft jfjf\n 1 2 3\n 4 5 6\n");
	SeparatedInput si(ii);
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("Test"));
		EXPECT_FALSE(si.getParametersRef().contains("x"));
		EXPECT_FALSE(si.getParametersRef().contains("n_x"));
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("x"));
		EXPECT_TRUE(si.getParametersRef().contains("n_x"));
		EXPECT_TRUE(si.getParametersRef().contains("y"));
		EXPECT_TRUE(si.getParametersRef().contains("z"));
		EXPECT_EQ("hello", si.getParametersRef().getString("n_x"));
		
		EXPECT_NEAR(10.0,si.getParametersRef().getValue("y"),0.0000001);
		EXPECT_EQ("unspecified", si.getParametersRef().getUnit("y"));
		EXPECT_NEAR(18520.0,Units::fromInternal("NM", si.getParametersRef().getUnit("y"), si.getParametersRef().getValue("y")),0.0000001);
		EXPECT_NEAR(18520.0,si.getParametersRef().getValue("z"),0.0000001);
		EXPECT_EQ("nmi",   si.getParametersRef().getUnit("z"));
		EXPECT_NEAR(18520.0,Units::fromInternal("NM",si.getParametersRef().getUnit("z"),si.getParametersRef().getValue("z")),0.0000001);
	}

	
	TEST_F(SeparatedInputTest, testParameterOnly) {
		istringstream* ii = new istringstream("\n\n# Test\n\n  #Test = 3\n\nn_x = hello    \ny = 10\nz    = 10  [nmi]");
			SeparatedInput si(ii);
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("Test")); // hasn't been read yet
		EXPECT_FALSE(si.getParametersRef().contains("x"));    // hasn't been read yet
		EXPECT_FALSE(si.getParametersRef().contains("n_x"));  // hasn't been read yet
		EXPECT_TRUE(si.readLine());  // returns TRUE meaning end of file, there is no "data" in this file
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("x"));
		EXPECT_TRUE(si.getParametersRef().contains("n_x"));
		EXPECT_TRUE(si.getParametersRef().contains("y"));
		EXPECT_TRUE(si.getParametersRef().contains("z"));
		EXPECT_EQ("hello", si.getParametersRef().getString("n_x"));
		
		EXPECT_NEAR(10.0,si.getParametersRef().getValue("y"),0.0000001);
		EXPECT_EQ("unspecified", si.getParametersRef().getUnit("y"));
		EXPECT_NEAR(18520.0,Units::fromInternal("NM", si.getParametersRef().getUnit("y"), si.getParametersRef().getValue("y")),0.0000001);
		EXPECT_NEAR(18520.0,si.getParametersRef().getValue("z"),0.0000001);
		EXPECT_EQ("nmi",   si.getParametersRef().getUnit("z"));
		EXPECT_NEAR(18520.0,Units::fromInternal("NM",si.getParametersRef().getUnit("z"),si.getParametersRef().getValue("z")),0.0000001);
	}

	
	TEST_F(SeparatedInputTest, testParameterOnlyEmpty) {
		istringstream* ii = new istringstream("n_x = hello    \ny = \ny2 =\nz    = 10  [nmi]");
			SeparatedInput si(ii);
		EXPECT_FALSE(si.hasError());
		EXPECT_TRUE(si.readLine());  // there is no "data" in this file
		EXPECT_FALSE(si.hasError());
		EXPECT_FALSE(si.getParametersRef().contains("x"));
		EXPECT_TRUE(si.getParametersRef().contains("n_x"));
		EXPECT_TRUE(si.getParametersRef().contains("y"));
		EXPECT_TRUE(si.getParametersRef().contains("y2"));
		EXPECT_TRUE(si.getParametersRef().contains("z"));
		
		EXPECT_EQ("hello",    si.getParametersRef().getString("n_x"));
		EXPECT_EQ("",    si.getParametersRef().getString("y"));
		EXPECT_EQ("",    si.getParametersRef().getString("y2"));
		EXPECT_EQ("10  [nmi]",    si.getParametersRef().getString("z"));
	}

	
	TEST_F(SeparatedInputTest, testStratwayFile) {
		istringstream* ii = new istringstream(
				"NAME       sx,     sy,     sz,     st,    mutable, label\n"
				"unspecified       m,     m,     m,     s,    unspecified, unspecified\n"
				"Ownship 455.3677 519.8260 0.4572 0.0000 F\n"
				"  \"      468.4619 519.8984 0.4572 8.4845 FFM  ");
	SeparatedInput si(ii);
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ( 0, si.findHeading("NAME"));
		EXPECT_EQ( 1, si.findHeading("sx"));
		EXPECT_EQ( 2, si.findHeading("sy"));
		EXPECT_EQ( 3, si.findHeading("sz"));
		EXPECT_EQ( 4, si.findHeading("st"));
		EXPECT_EQ( 5, si.findHeading("mutable"));
		EXPECT_EQ( 6, si.findHeading("label"));
		EXPECT_EQ("unspecified", si.getUnit(0));   // Note: no units are defined
		EXPECT_EQ("m", si.getUnit(1));
		EXPECT_EQ("m", si.getUnit(2));
		EXPECT_EQ("m", si.getUnit(3));   
		EXPECT_EQ("s", si.getUnit(4));   
		EXPECT_EQ("unspecified", si.getUnit(5));   // Note: no units are defined
		EXPECT_EQ("unspecified", si.getUnit(6));   // Note: no units are defined
		EXPECT_FALSE(si.hasError());
		
		// Check values for first line
		EXPECT_EQ("Ownship", si.getColumnString(0));
		EXPECT_NEAR(455.3677,Units::to("m", si.getColumn(1)),0.00000001);
		EXPECT_NEAR(519.8260,Units::to("m", si.getColumn(2)),0.00000001);
		EXPECT_FALSE(si.hasError());

		// Check values for second line
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_EQ("\"", si.getColumnString(0));
		EXPECT_NEAR(468.4619,Units::to("m", si.getColumn(1)),0.00000001);
		EXPECT_NEAR(519.8984,Units::to("m", si.getColumn(2)),0.00000001);
		EXPECT_NEAR(0.4572,Units::to("m", si.getColumn(3)),0.00000001);
		EXPECT_NEAR(8.4845,Units::to("s", si.getColumn(4)),0.00000001);
		EXPECT_FALSE(si.hasError());

		EXPECT_TRUE(si.readLine());  // indicates EOF
	}

	
	
	TEST_F(SeparatedInputTest, testFixedColumns) {
		istringstream* ii = new istringstream(
			 	 "AASONFIX    40-51-35.490N 115-37-17.070W\n"                                       
				"AASUNFIX    44-54-27.640N 093-15-18.160W  \n"                                       
				"AAVVAFIX    37-36-26.860N 077-23-47.020W                   \n"                                       
				"ABB  VORTAC 38-35-19.820N 085-38-09.570W  71001E112.40LNABB\n"                    
				"AATREFIX    21-13-11.050N 157-55-18.910W12345678901234567890 \n"                                       
				"ABENYFIX    54-15-06.000N 113-04-34.000W\n");
			SeparatedInput si(ii);
		si.setFixedColumn("5,7,13,15,19","Name,Type,Lat,Long,Other","unitless,unitless,unitless,unitless,unitless");
		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ( 0, si.findHeading("Name"));
		EXPECT_EQ( 1, si.findHeading("Type"));
		EXPECT_EQ( 2, si.findHeading("Lat"));
		EXPECT_EQ( 3, si.findHeading("Long"));
		EXPECT_EQ( 4, si.findHeading("Other"));
		EXPECT_EQ("unitless", si.getUnit(0));   // Note: no units are defined
		EXPECT_EQ("unitless", si.getUnit(1));
		EXPECT_EQ("unitless", si.getUnit(2));
		EXPECT_EQ("unitless", si.getUnit(3));   
		EXPECT_EQ("unitless", si.getUnit(4));   
		EXPECT_FALSE(si.hasError());
		
		// Check values for first line
		EXPECT_EQ("AASON", si.getColumnString(0));
		EXPECT_EQ("FIX    ", si.getColumnString(1));
		EXPECT_EQ("40-51-35.490N", si.getColumnString(2));
		EXPECT_EQ(" 115-37-17.070W", si.getColumnString(3));
		EXPECT_EQ("", si.getColumnString(4));
		EXPECT_FALSE(si.hasError());

		// Check values for second line
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_EQ("AASUN", si.getColumnString(0));
		EXPECT_EQ("FIX    ", si.getColumnString(1));
		EXPECT_EQ("44-54-27.640N", si.getColumnString(2));
		EXPECT_EQ(" 093-15-18.160W", si.getColumnString(3));
		EXPECT_EQ("", si.getColumnString(4));
		EXPECT_FALSE(si.hasError());

		// Third line
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_EQ("AAVVA", si.getColumnString(0));
		EXPECT_EQ("FIX    ", si.getColumnString(1));
		EXPECT_EQ("37-36-26.860N", si.getColumnString(2));
		EXPECT_EQ(" 077-23-47.020W", si.getColumnString(3));
		EXPECT_EQ("", si.getColumnString(4));
		EXPECT_FALSE(si.hasError());

		// Fourth line
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_EQ("ABB  ", si.getColumnString(0));
		EXPECT_EQ("VORTAC ", si.getColumnString(1));
		EXPECT_EQ("38-35-19.820N", si.getColumnString(2));
		EXPECT_EQ(" 085-38-09.570W", si.getColumnString(3));
		EXPECT_EQ("  71001E112.40LNABB", si.getColumnString(4));
		EXPECT_FALSE(si.hasError());

		// Fifth line
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_EQ("AATRE", si.getColumnString(0));
		EXPECT_EQ("FIX    ", si.getColumnString(1));
		EXPECT_EQ("21-13-11.050N", si.getColumnString(2));
		EXPECT_EQ(" 157-55-18.910W", si.getColumnString(3));
		EXPECT_EQ("1234567890123456789", si.getColumnString(4));
		EXPECT_FALSE(si.hasError());

		// Sixth line           
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		EXPECT_EQ("ABENY", si.getColumnString(0));
		EXPECT_EQ("FIX    ", si.getColumnString(1));
		EXPECT_EQ("54-15-06.000N", si.getColumnString(2));
		EXPECT_EQ(" 113-04-34.000W", si.getColumnString(3));
		EXPECT_EQ("", si.getColumnString(4));
		EXPECT_FALSE(si.hasError());

		EXPECT_TRUE(si.readLine());  // indicates EOF
	}
	
	TEST_F(SeparatedInputTest, testTokenizeQuotes) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n\"nosegay garbonzo\"; moose  ;\"2 2  2\";\"3 3 3\"; pie ;\"feelers; peelers\"\n");
		SeparatedInput si(ii);

		si.setQuoteCharacter('"');
		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3 3", si.getColumnString(3));
		EXPECT_EQ("pie", si.getColumnString(4));
		EXPECT_EQ("feelers; peelers", si.getColumnString(5));


	}
	
	TEST_F(SeparatedInputTest, testQuotes) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n\"nosegay garbonzo\"; moose  ;\"2 2  2\";\"3 3 3\"; pie ;\"feelers; peelers\"\n");
		SeparatedInput si(ii);
		si.setQuoteCharacter('"');
		
		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3 3", si.getColumnString(3));
		EXPECT_EQ("pie", si.getColumnString(4));
		EXPECT_EQ("feelers; peelers", si.getColumnString(5));
	}
	
TEST_F(SeparatedInputTest, testErrorQuoteIsDelimiter) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\nx,y,z,a\n");
		SeparatedInput si(ii);
		si.setQuoteCharacter(',');
		
		EXPECT_TRUE(si.hasError());
	}
	

TEST_F(SeparatedInputTest, testDelimiter) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n"
				"  x,y , z,a\n"
				",,,,\n"
				", , ,  ,\n"
				" \"  nos\"\"egay garbonzo\", mo\"ose\"  ,\"2 2\" \"  2\",\"3 3 3\", pie ,\"feelers, peelers\"\n"
				);
		
		SeparatedInput si(ii);
		si.setColumnDelimiters(","); // this is a CSV file without filtering out the extra space around the comma
		si.setQuoteCharacter('"');

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y ", si.getColumnString(1));
		EXPECT_EQ(" z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("", si.getColumnString(0));
		EXPECT_EQ("", si.getColumnString(1));
		EXPECT_EQ("", si.getColumnString(2));
		EXPECT_EQ("", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("", si.getColumnString(0));
		EXPECT_EQ(" ", si.getColumnString(1));
		EXPECT_EQ(" ", si.getColumnString(2));
		EXPECT_EQ("  ", si.getColumnString(3));

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ("  nos\"egay garbonzo", si.getColumnString(0));
		EXPECT_EQ(" moose  ", si.getColumnString(1));
		EXPECT_EQ("2 2   2", si.getColumnString(2));
		EXPECT_EQ("3 3 3", si.getColumnString(3));
		EXPECT_EQ(" pie ", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));	
		
		EXPECT_TRUE(si.readLine());  // true would mean EOF
	}
	
TEST_F(SeparatedInputTest, testCsv) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n"
				"  x,y , z,a\n"
				",,,,\n"
				", , ,    ,\n"
				" \"  nos\"\"egay garbonzo\", mo\"ose\"  ,\"2 2\" \"  2\",\"3 3 3\", pie ,\"feelers, peelers\"\n"
				"x ,\"feelers, peelers\"\n"
				"\"  nosegay garbonzo\", mo\"ose\"  ,\"2 2\"   \"  2\",\"3 3 3\", pi\"\"e ,\"feelers, peelers\"\n"
				"   \"\"\"  nosegay garbonzo\", mo\"ose\"  ,\"2 2\" \"  2\",\"3 3 3\", pi\"\"e ,\"feelers, peelers\"\n");
		SeparatedInput si(ii);
		
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("", si.getColumnString(0));
		EXPECT_EQ("", si.getColumnString(1));
		EXPECT_EQ("", si.getColumnString(2));
		EXPECT_EQ("", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("", si.getColumnString(0));
		EXPECT_EQ("", si.getColumnString(1));
		EXPECT_EQ("", si.getColumnString(2));
		EXPECT_EQ("", si.getColumnString(3));

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ("  nos\"egay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2   2", si.getColumnString(2));
		EXPECT_EQ("3 3 3", si.getColumnString(3));
		EXPECT_EQ("pie", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));
		
		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("feelers, peelers", si.getColumnString(1));

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ("  nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2     2", si.getColumnString(2));
		EXPECT_EQ("3 3 3", si.getColumnString(3));
		EXPECT_EQ("pi\"e", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());
		
		EXPECT_EQ("\"  nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2   2", si.getColumnString(2));
		EXPECT_EQ("3 3 3", si.getColumnString(3));
		EXPECT_EQ("pi\"e", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));		
		
		EXPECT_TRUE(si.readLine());  // true would mean EOF
	}
	
TEST_F(SeparatedInputTest, testCsv2) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n"
				"  x, y \" y\",z,a\n"
				"  x, \" y\" y,z,a\n"
				"  x, \"y \" \" y\",z,a\n"
				"  x, \"y\"\"y\",z,a\n"
				"  x, \"y\" \"y\",z,a\n"
				"  x, y\"\" y y,z,a\n"
				);
		SeparatedInput si(ii);
	
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y  y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ(" y y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y   y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));


		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y\"y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("x", si.getColumnString(0));
		EXPECT_EQ("y\" y y", si.getColumnString(1));
		EXPECT_EQ("z", si.getColumnString(2));
		EXPECT_EQ("a", si.getColumnString(3));

		EXPECT_TRUE(si.readLine());  // true would mean EOF
	}
	

	
	TEST_F(SeparatedInputTest, testQuotesCsvQuoteInQuote) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n\"nosegay garbonzo\", moose  ,\"2 2  2\",\"3 3\"\" 3\", pie ,\"feelers, peelers\"\n");
		SeparatedInput si(ii);
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3\" 3", si.getColumnString(3));
		EXPECT_EQ("pie", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));
	}
	
	TEST_F(SeparatedInputTest, testCsvNewlineInQuote) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n\"nosegay garbonzo\", moose  ,\"2 2  2\",\"3 3\n 3\", pie ,\"feelers, peelers\"\n");
		SeparatedInput si(ii);
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3\n 3", si.getColumnString(3));
		EXPECT_EQ("pie", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));
	}
	
	TEST_F(SeparatedInputTest, testCsvNewlinesInQuote) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n\"nosegay garbonzo\", moose  ,\"2 2  2\",\"3 3\n\n\n 3\", pie ,\"feelers, peelers\"\n");
		SeparatedInput si(ii);
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3\n\n\n 3", si.getColumnString(3));
		EXPECT_EQ("pie", si.getColumnString(4));
		EXPECT_EQ("feelers, peelers", si.getColumnString(5));
	}
	
	TEST_F(SeparatedInputTest, testCsvQuoteUnmatched) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\nnos\"egay garbonzo\", moose  ,\"2 2  2\",\"3 3 3, pie ,\"feelers, peelers\"\n");
		SeparatedInput si(ii);
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3 3, pie ,feelers", si.getColumnString(3));
		EXPECT_EQ("peelers", si.getColumnString(4));
	}
	
	TEST_F(SeparatedInputTest, testCsvQuoteUnmatchedWithNewline) {
		istringstream* ii = new istringstream("H1, H2, H3, H4\n\"nosegay garbonzo\", moose  ,\"2 2  2\",\"3 3\n 3, pie ,\"feelers, peelers\"\n");
		SeparatedInput si(ii);
		si.setCsv();

		EXPECT_FALSE(si.hasError());

		EXPECT_FALSE(si.readLine());  // true would mean EOF
		EXPECT_FALSE(si.hasError());

		EXPECT_EQ("nosegay garbonzo", si.getColumnString(0));
		EXPECT_EQ("moose", si.getColumnString(1));
		EXPECT_EQ("2 2  2", si.getColumnString(2));
		EXPECT_EQ("3 3\n 3, pie ,feelers", si.getColumnString(3));
		EXPECT_EQ("peelers", si.getColumnString(4));
	}

/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect4.h"
#include "Units.h"
#include "StateWriter.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

using namespace larcfm;
using namespace std;

class SeparatedOutputTest : public ::testing::Test {
};
	

	TEST_F(SeparatedOutputTest, testBasic) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		//so.setParameterPrecision(num);
		//so.setParameters(pr);
		//so.setParameters(pr,std::string []);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(2,so.length());
		EXPECT_EQ("name,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
		
		so.writeLine();		
		EXPECT_EQ(2,so.length());
		EXPECT_EQ("name,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());

		so.setColumn(3, 3.1);
		so.setColumn(2, 2.1);
		so.setColumn(1, 1.1);
		so.setColumn(0, "n1");
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("name,x,y,z\nn,1.0,2.0,3.0\nn1,1.1,2.1,3.1\n", osw.str());
		EXPECT_FALSE(so.hasError());
		
		so.addColumn("p");
		so.setColumn(1.0);
		so.writeLine();		
		EXPECT_EQ(4,so.length());
		EXPECT_EQ("name,x,y,z\nn,1.0,2.0,3.0\nn1,1.1,2.1,3.1\np,1.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
		
		so.setColumn(0,"q");
		so.setColumn(3,1.0);
		so.writeLine();		
		EXPECT_EQ(5,so.length());
		EXPECT_EQ("name,x,y,z\nn,1.0,2.0,3.0\nn1,1.1,2.1,3.1\np,1.0\nq,,,1.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
	
	
	TEST_F(SeparatedOutputTest, testPrecision) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setColumnDelimiterComma();
		so.setOutputUnits(true);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(0.22);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("name,x,y,z\nunspecified,m,m,m\nn,1.0,0.2,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}		
	
	
	TEST_F(SeparatedOutputTest, testDelimiterComma) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setColumnDelimiterComma();
		so.setOutputUnits(true);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("name,x,y,z\nunspecified,m,m,m\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}		
	
	
	TEST_F(SeparatedOutputTest, testDelimiterTab) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setColumnDelimiterTab();
		so.setOutputUnits(true);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("name\tx\ty\tz\nunspecified\tm\tm\tm\nn\t1.0\t2.0\t3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
	
	
	TEST_F(SeparatedOutputTest, testDelimiterSpace) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setColumnDelimiterSpace();
		so.setOutputUnits(true);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("name x y z\nunspecified m m m\nn 1.0 2.0 3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}		
	
	
	TEST_F(SeparatedOutputTest, testColumnSpace) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setColumnSpace(3);  // additional space after the delimiter
		so.setEmptyValue("empty"); 
		so.setHeading(0,"name", "unspecified");
		so.setHeading(1,"x",    "m");
		so.setHeading(3,"y",    "m");
		so.setHeading(4,"z",    "m");
		so.setColumn(0,"n");
		so.setColumn(1,1.0);
		so.setColumn(3,2.0);
		so.setColumn(4,3.0);
		so.writeLine();		
		EXPECT_EQ(2,so.length());
		EXPECT_EQ("name,   x,   empty,   y,   z\nn,   1.0,   empty,   2.0,   3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}		

	
	TEST_F(SeparatedOutputTest, testUnitConversions) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setOutputUnits(true);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "km");
		so.addHeading("y",    "m");
		so.addHeading("z",    "mm");
		so.addColumn("n");
		so.setColumn(1000.0); // in internal units, i.e., [m]
		so.setColumn(1000.0); // in internal units, i.e., [m]
		so.setColumn(1000.0); // in internal units, i.e., [m]
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("name,x,y,z\nunspecified,km,m,mm\nn,1.0,1000.0,1000000.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
		
	
	TEST_F(SeparatedOutputTest, testUnitConversions2) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setOutputUnits(false);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "km");
		so.addHeading("y",    "m");
		so.addHeading("z",    "mm");
		so.addColumn("n");
		so.setColumn(1000.0); // in internal units, i.e., [m]
		so.setColumn(1000.0); // in internal units, i.e., [m]
		so.setColumn(1000.0); // in internal units, i.e., [m]
		so.writeLine();		
		EXPECT_EQ(2,so.length());
		EXPECT_EQ("name,x,y,z\nn,1.0,1000.0,1000000.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
		

	
	TEST_F(SeparatedOutputTest, testComment) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.addComment("This is a test");
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ("# This is a test\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
		
		so.addComment("This is another test");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(5,so.length());
		EXPECT_EQ("# This is a test\nname,x,y,z\nn,1.0,2.0,3.0\n# This is another test\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
		
	
	TEST_F(SeparatedOutputTest, testComment2) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.setCommentCharacter(";; ");
		so.addComment("This is a test");
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(3,so.length());
		EXPECT_EQ(";; This is a test\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
		
		so.addComment("This is another test");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(5,so.length());
		EXPECT_EQ(";; This is a test\nname,x,y,z\nn,1.0,2.0,3.0\n;; This is another test\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
		
	
	TEST_F(SeparatedOutputTest, testComment3) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.addComment("This is a test");
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.addComment("This is another test");
		so.writeLine();		
		EXPECT_EQ(4,so.length());
		EXPECT_EQ("# This is a test\n# This is another test\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
		

	
	TEST_F(SeparatedOutputTest, testComment4) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());

		so.addComment("This is a test\nAnother Test");
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.addComment("This is another test");
		so.writeLine();		
		EXPECT_EQ(5,so.length());
		EXPECT_EQ("# This is a test\n# Another Test\n# This is another test\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_FALSE(so.hasError());
	}
		

	
	TEST_F(SeparatedOutputTest, testParameters1) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());
		
		ParameterData sr; // = new ParameterData();
		sr.setInternal("a", Units::from("NM",15.0), "NM");
		sr.setInternal("b", Units::from("kg",5.12346), "kg");
		sr.set("c", "a_string");

		so.setParameters(sr);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(5,so.length());

		// Different than Java, but OK because the order the parameters come out is not guarenteed
		//EXPECT_EQ("b = 5.1235 [kg]\nc = a_string\na = 15.0000 [NM]\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());
		EXPECT_EQ("a = 15.000000 [NM]\nb = 5.123460 [kg]\nc = a_string\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());

		EXPECT_FALSE(so.hasError());
	}
		
	
	TEST_F(SeparatedOutputTest, testParameters2) {
		ostringstream osw;
		SeparatedOutput so(&osw); // = new SeparatedOutput(osw);
		EXPECT_FALSE(so.hasError());
		
		ParameterData sr; // = new ParameterData();  // just for parameters
		sr.setInternal("a", Units::from("NM",15.0), "NM");
		sr.setInternal("b", Units::from("kg",5.12346), "kg");
		//sr.set("c", "a_string");

//		ArrayList<String> list = new ArrayList<String>();
//		list.add("a");
//		list.add("b");
//		so.setParameters(sr,list);
		so.setParameters(sr);
		so.addHeading("name", "unspecified");
		so.addHeading("x",    "m");
		so.addHeading("y",    "m");
		so.addHeading("z",    "m");
		so.addColumn("n");
		so.setColumn(1.0);
		so.setColumn(2.0);
		so.setColumn(3.0);
		so.writeLine();		
		EXPECT_EQ(4,so.length());

		EXPECT_EQ("a = 15.000000 [NM]\nb = 5.123460 [kg]\nname,x,y,z\nn,1.0,2.0,3.0\n", osw.str());

		EXPECT_FALSE(so.hasError());
	}





/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "PlanWriter.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "PlanReader.h"

using namespace larcfm;
using namespace std;

class PlanWriterTest: public ::testing::Test {
public:
	ostringstream osw;
	istringstream osr;

protected:

	void setUp() {
		//pd = new ParameterData();    // not needed in C++
	}
};


	
	TEST_F(PlanWriterTest, testBasic) {
		PlanWriter sw;
		sw.open(&osw);
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());
		
		Plan p("PlanWriterTester");
		NavPoint np(Position::makeXYZ(1.0,2.0,3.0), 0.0);
		p.addNavPoint(np);
		np = NavPoint(Position::makeXYZ(2.0,2.0,3.0), 5.0);
		p.addNavPoint(np);
		
		sw.writePlan(p,false);
		
		EXPECT_EQ("ID,sx,sy,sz,time,name\nunitless,NM,NM,ft,s,unitless\nPlanWriterTester,1.00000,2.00000,3.00000,0.00000,-\nPlanWriterTester,2.00000,2.00000,3.00000,5.00000,-\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.setPrecision(6);
		np = NavPoint(Position::makeXYZ(3.0,2.0,3.0), 10.0);
		p.addNavPoint(np);
		sw.writePlan(p,false);
		EXPECT_EQ("ID,sx,sy,sz,time,name\nunitless,NM,NM,ft,s,unitless\nPlanWriterTester,1.00000,2.00000,3.00000,0.00000,-\nPlanWriterTester,2.00000,2.00000,3.00000,5.00000,-\nPlanWriterTester,1.000000,2.000000,3.000000,0.000000,-\nPlanWriterTester,2.000000,2.000000,3.000000,5.000000,-\nPlanWriterTester,3.000000,2.000000,3.000000,10.000000,-\n", osw.str());
		EXPECT_FALSE(sw.hasError());

		sw.close();
		EXPECT_FALSE(sw.hasError());
	}
	
	
	TEST_F(PlanWriterTest, testComment) {
		PlanWriter sw;
		sw.open(&osw);
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		sw.addComment("This is");
		sw.addComment("A Test.");
		sw.addComment("This is\nanother test");
		
		Plan p("PlanWriterTester");
		NavPoint np;
		np = NavPoint(Position::makeXYZ(1.0,2.0,3.0), 0.0);
		p.addNavPoint(np);
		np = NavPoint(Position::makeXYZ(2.0,2.0,3.0), 5.0);
		p.addNavPoint(np);
		
		sw.writePlan(p,false);
		
		sw.addComment("This is a third test"); // ignored, because after writePlan
		
		EXPECT_EQ("# This is\n# A Test.\n# This is\n# another test\nID,sx,sy,sz,time,name\nunitless,NM,NM,ft,s,unitless\nPlanWriterTester,1.00000,2.00000,3.00000,0.00000,-\nPlanWriterTester,2.00000,2.00000,3.00000,5.00000,-\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		sw.close();
		EXPECT_FALSE(sw.hasError());
	}
	
	
	TEST_F(PlanWriterTest, testLatLonAlt) {
		PlanWriter sw;
		sw.open(&osw);
		sw.setPrecision(12);
		EXPECT_FALSE(sw.hasError());
		
		Plan p("PlanWriterTester");
		NavPoint np;
		np = NavPoint(Position::mkLatLonAlt(1.0,2.0,3.0), 1.0);
		p.addNavPoint(np);
		np = NavPoint(Position::mkLatLonAlt(2.0,2.0,4.0), 5.0);
		p.addNavPoint(np);
		
		sw.writePlan(p,false);
		sw.close();
		
		// Read it.
		
		std::istringstream osr(osw.str());
		PlanReader pr;
		pr.open(&osr);
		EXPECT_FALSE(pr.hasMessage());
		p = pr.getPlan(0);
		
		EXPECT_NEAR(1.0,p.time(0),0.000000001);
		EXPECT_NEAR(1.0,p.point(0).lat(),0.000000001);
		EXPECT_NEAR(2.0,p.point(0).lon(),0.000000001);
		EXPECT_NEAR(3.0,p.point(0).alt(),0.000000001);
		
		EXPECT_NEAR(5.0,p.time(1),0.000000001);
		EXPECT_NEAR(2.0,p.point(1).lat(),0.000000001);
		EXPECT_NEAR(2.0,p.point(1).lon(),0.000000001);
		EXPECT_NEAR(4.0,p.point(1).alt(),0.000000001);
	}

	
	TEST_F(PlanWriterTest, testMultiplePlans) {
		PlanWriter sw;
		sw.open(&osw);
		sw.setPrecision(12);
		EXPECT_FALSE(sw.hasError());
		
		Plan p1("PlanWriterTester1");
		NavPoint np;
		np = NavPoint(Position::mkLatLonAlt(1.0,2.0,3.0), 1.0);
		p1.addNavPoint(np);
		np = NavPoint(Position::mkLatLonAlt(2.0,2.0,4.0), 5.0);
		p1.addNavPoint(np);
		Plan p2("PlanWriterTester2");
		np = NavPoint(Position::mkLatLonAlt(1.1,2.0,3.0), 1.0);
		p2.addNavPoint(np);
		np = NavPoint(Position::mkLatLonAlt(2.1,2.0,4.0), 5.0);
		p2.addNavPoint(np);
		
		sw.writePlan(p1,false);
		sw.writePlan(p2,false);
		sw.close();
		
		// Read it.
		
		istringstream osr(osw.str());
		PlanReader pr;
		pr.open(&osr);
		EXPECT_FALSE(pr.hasMessage());
		p1 = pr.getPlan(0);
		p2 = pr.getPlan(1);
		
		EXPECT_NEAR(1.0,p1.time(0),0.000000001);
		EXPECT_NEAR(1.0,p1.point(0).lat(),0.000000001);
		EXPECT_NEAR(2.0,p1.point(0).lon(),0.000000001);
		EXPECT_NEAR(3.0,p1.point(0).alt(),0.000000001);
		
		EXPECT_NEAR(5.0,p1.time(1),0.000000001);
		EXPECT_NEAR(2.0,p1.point(1).lat(),0.000000001);
		EXPECT_NEAR(2.0,p1.point(1).lon(),0.000000001);
		EXPECT_NEAR(4.0,p1.point(1).alt(),0.000000001);
		
		EXPECT_NEAR(1.0,p2.time(0),0.000000001);
		EXPECT_NEAR(1.1,p2.point(0).lat(),0.000000001);
		EXPECT_NEAR(2.0,p2.point(0).lon(),0.000000001);
		EXPECT_NEAR(3.0,p2.point(0).alt(),0.000000001);
		
		EXPECT_NEAR(5.0,p2.time(1),0.000000001);
		EXPECT_NEAR(2.1,p2.point(1).lat(),0.000000001);
		EXPECT_NEAR(2.0,p2.point(1).lon(),0.000000001);
		EXPECT_NEAR(4.0,p2.point(1).alt(),0.000000001);
	}


	
	TEST_F(PlanWriterTest, testTcpBvs) {
		PlanWriter pw;
		pw.open(&osw);
		pw.setPrecision(12);
		EXPECT_FALSE(pw.hasError());

		Plan p("TestTcpBvsTester");
		// TODO: linearIndex
		NavPoint np = NavPoint(Position::mkLatLonAlt(1.0,2.0,3.0), 1.0, "hello");
		NavPoint src = NavPoint(Position::mkLatLonAlt(0.1,0.2,0.3), 30.0);
		TcpData tcp = TcpData().setBVS(1.12);
//fpln("tcp="+tcp.toString(true));
		p.add(np,tcp);
		np = NavPoint(Position::mkLatLonAlt(2.0,2.0,4.0), 5.0);
		p.addNavPoint(np);

		pw.writePlan(p,true);
		pw.close();


		// Read it.

//fpln(osw.str());
		istringstream osr(osw.str());
		PlanReader pr;
		pr.open(&osr);
		//fpln(pr.getMessageNoClear());
		EXPECT_FALSE(pr.hasMessage());
		p = pr.getPlan(0);
//fpln(" $$ testTCP: READ p = "+p.toStringFull());

		EXPECT_EQ("TestTcpBvsTester", p.getID());
		EXPECT_NEAR(1.0,p.time(0),0.000000001);
		EXPECT_NEAR(1.0,p.point(0).lat(),0.000000001);
		EXPECT_NEAR(2.0,p.point(0).lon(),0.000000001);
		EXPECT_NEAR(3.0,p.point(0).alt(),0.000000001);
		EXPECT_EQ("hello", p.point(0).name());
		//EXPECT_NEAR(30.0,p.getTcpData(0).getSourceTime(),0.000000001);
		//EXPECT_NEAR(0.1,p.getTcpData(0).getSourcePosition().lat(),0.0000001);
		//EXPECT_NEAR(0.2,p.getTcpData(0).getSourcePosition().lon(),0.0000001);
		//EXPECT_NEAR(0.3,p.getTcpData(0).getSourcePosition().alt(),0.0000001);
		EXPECT_TRUE(p.isTCP(0));
		EXPECT_FALSE(p.isBOT(0));
		EXPECT_FALSE(p.isEOT(0));
		EXPECT_FALSE(p.isBGS(0));
		EXPECT_FALSE(p.isEGS(0));
		EXPECT_TRUE(p.isBVS(0));
		EXPECT_FALSE(p.isEVS(0));
		//EXPECT_NEAR(0.4,p.velocityInit(0).trk(),0.0000001);
		//EXPECT_NEAR(4.5,p.velocityInit(0).gs(),0.0000001);
		//EXPECT_NEAR(4.6,p.velocityInit(0).vs(),0.0000001);
		//EXPECT_NEAR(0.0,p.trkAccel(0),0.0000001);
		EXPECT_NEAR(0.0,p.gsAccel(0),0.0000001);
		EXPECT_NEAR(1.12,p.vsAccel(0),0.0000001);

		EXPECT_NEAR(5.0,p.time(1),0.000000001);
		EXPECT_NEAR(2.0,p.point(1).lat(),0.000000001);
		EXPECT_NEAR(2.0,p.point(1).lon(),0.000000001);
		EXPECT_NEAR(4.0,p.point(1).alt(),0.000000001);
	}

	TEST_F(PlanWriterTest, testTcpBot) {
		PlanWriter sw;
		sw.open(&osw);
		sw.setPrecision(12);
		EXPECT_FALSE(sw.hasError());
		
		Plan p("PlanWriterTester");
		// TODO: linearIndex
		NavPoint np = NavPoint(Position::mkLatLonAlt(1.0,2.0,3.0), 1.0, "hello");
		NavPoint src = NavPoint(Position::mkLatLonAlt(0.1,0.2,0.3), 30.0);
		TcpData tcp = TcpData().setBOT(Units::from("NM",-14.0),Position::INVALID());
		p.add(np,tcp);
		np = NavPoint(Position::mkLatLonAlt(2.0,2.0,4.0), 5.0);
		p.addNavPoint(np);
		
		sw.writePlan(p,true);
		sw.close();
		
		// Read it.
		
		istringstream osr(osw.str());
		PlanReader pr;
		pr.open(&osr);
		EXPECT_TRUE(pr.hasMessage()); // this should be true, as np/tcp is not a fullt defined BOT (no center)
		p = pr.getPlan(0);
		
		EXPECT_EQ("PlanWriterTester", p.getID());
		EXPECT_NEAR(1.0,p.time(0),0.000000001);
		EXPECT_NEAR(1.0,p.point(0).lat(),0.000000001);
		EXPECT_NEAR(2.0,p.point(0).lon(),0.000000001);
		EXPECT_NEAR(3.0,p.point(0).alt(),0.000000001);

		EXPECT_EQ("hello", p.point(0).name());
//		EXPECT_NEAR(30.0,p.getTcpData(0).getSourceTime(),0.000000001);
//		EXPECT_NEAR(0.1,p.getTcpData(0).getSourcePosition().lat(),0.0000001);
//		EXPECT_NEAR(0.2,p.getTcpData(0).getSourcePosition().lon(),0.0000001);
//		EXPECT_NEAR(0.3,p.getTcpData(0).getSourcePosition().alt(),0.0000001);
		EXPECT_TRUE(p.isTCP(0));
		EXPECT_TRUE(p.isBOT(0));
		EXPECT_FALSE(p.isEOT(0));
		EXPECT_FALSE(p.isBGS(0));
		EXPECT_FALSE(p.isEGS(0));
		EXPECT_FALSE(p.isBVS(0));
		EXPECT_FALSE(p.isEVS(0));
		//EXPECT_NEAR(0.4,p.velocityInit(0).trk(),0.0000001);
		//EXPECT_NEAR(4.5,p.velocityInit(0).gs(),0.0000001);
		//EXPECT_NEAR(4.6,p.velocityInit(0).vs(),0.0000001);
		EXPECT_NEAR(-14,Units::to("NM", p.signedRadius(0)),0.0000001);
		EXPECT_NEAR(-1,p.turnDir(0),0.0000001);
		EXPECT_NEAR(0.0,p.gsAccel(0),0.0000001);
		EXPECT_NEAR(0.0,p.vsAccel(0),0.0000001);
		
		EXPECT_NEAR(5.0,p.time(1),0.000000001);
		EXPECT_NEAR(2.0,p.point(1).lat(),0.000000001);
		EXPECT_NEAR(2.0,p.point(1).lon(),0.000000001);
		EXPECT_NEAR(4.0,p.point(1).alt(),0.000000001);
	}

	
	TEST_F(PlanWriterTest, testParameters) {
		PlanWriter sw;
		sw.open(&osw);
		EXPECT_FALSE(sw.hasError());

		//sw.setUnit("name","unit");		
		//sw.setParameterPrecision(num);
		//sw.setParameters(pr);
		//sw.setParameters(pr,std::string []);
		
		ParameterData cfg;
		cfg.setInternal("param1",Units::from("NM",1),"NM");
		cfg.setInternal("param2",Units::from("m",2),"mm");
		cfg.setInternal("param3",Units::from("lbm",3),"lbm");
		
		sw.setParameters(cfg);
		sw.addComment("This is a test");
		
		NavPoint np;
		Plan p("PlanWriterTester");
		np = NavPoint(Position::mkLatLonAlt(1.0,2.0,3.0), 1.0);
		p.addNavPoint(np);
		np = NavPoint(Position::mkLatLonAlt(2.0,2.0,4.0), 5.0);
		p.addNavPoint(np);
		
		sw.writePlan(p,true);
		sw.close();

		// Read it.
		
		istringstream osr(osw.str());
		PlanReader pr;
		pr.open(&osr);
		EXPECT_FALSE(pr.hasMessage());

		ParameterData pd = pr.getParametersRef();
		EXPECT_NEAR(Units::from("NM",1),pd.getValue("param1"),0.00000001);
		EXPECT_NEAR(Units::from("m",2),pd.getValue("param2"),0.00000001);
		EXPECT_NEAR(Units::from("lbm",3),pd.getValue("param3"),0.00000001);
		
	}


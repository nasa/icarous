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

class StateWriterTest : public ::testing::Test {
};
	
	TEST_F(StateWriterTest, testBasic) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,time,sx,sy,sz,vx,vy,vz\nunitless,s,NM,NM,ft,knot,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,time,sx,sy,sz,vx,vy,vz\nunitless,s,NM,NM,ft,knot,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,time,sx,sy,sz,vx,vy,vz\nunitless,s,NM,NM,ft,knot,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\nac1,20.0000000,4.0000000,5.0000000,6.0000000,0.0000000,0.0000000,0.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}
	
	
	TEST_F(StateWriterTest, testNoVelocity) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		sw.setVelocity(false);
		sw.setTrkGsVs(false);
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,time,sx,sy,sz\nunitless,s,NM,NM,ft\nac1,10.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,time,sx,sy,sz\nunitless,s,NM,NM,ft\nac1,10.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,time,sx,sy,sz\nunitless,s,NM,NM,ft\nac1,10.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000\nac1,20.0000000,4.0000000,5.0000000,6.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}
	
	
	TEST_F(StateWriterTest, testNoTime) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		// sw.setVelocity(true);      // default, not needed
		sw.setTrkGsVs(false);
		sw.setOutputTime(false);
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,sx,sy,sz,vx,vy,vz\nunitless,NM,NM,ft,knot,knot,fpm\nac1,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,sx,sy,sz,vx,vy,vz\nunitless,NM,NM,ft,knot,knot,fpm\nac1,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,sx,sy,sz,vx,vy,vz\nunitless,NM,NM,ft,knot,knot,fpm\nac1,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\nac1,4.0000000,5.0000000,6.0000000,0.0000000,0.0000000,0.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}
	
	
	TEST_F(StateWriterTest, testNoUnits) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, not needed
		sw.setTrkGsVs(false);
		sw.setOutputUnits(false);
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,time,sx,sy,sz,vx,vy,vz\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,time,sx,sy,sz,vx,vy,vz\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,time,sx,sy,sz,vx,vy,vz\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\nac1,20.0000000,4.0000000,5.0000000,6.0000000,0.0000000,0.0000000,0.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}
	
	
	TEST_F(StateWriterTest, testTrkGsVs) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);   // default, not needed
		//sw.setTrkGsVs(true);    // default, not needed
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeTrkGsVs(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,time,sx,sy,sz,trk,gs,vs\nunitless,s,NM,NM,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeTrkGsVs(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,time,sx,sy,sz,trk,gs,vs\nunitless,s,NM,NM,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,time,sx,sy,sz,trk,gs,vs\nunitless,s,NM,NM,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\nac1,20.0000000,4.0000000,5.0000000,6.0000000,0.0000000,0.0000000,0.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}

	
	TEST_F(StateWriterTest, testLatLonAlt) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, not needed
		//sw.setTrkGsVs(true);   // default, not needed
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		v = Velocity::makeTrkGsVs(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,time,lat,lon,alt,trk,gs,vs\nunitless,s,deg,deg,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeLatLonAlt(3.1,2.1,1.1);
		v = Velocity::makeTrkGsVs(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,time,lat,lon,alt,trk,gs,vs\nunitless,s,deg,deg,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeLatLonAlt(4.0,5.0,6.0);
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,time,lat,lon,alt,trk,gs,vs\nunitless,s,deg,deg,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\nac1,20.0000000,4.0000000,5.0000000,6.0000000,0.0000000,0.0000000,0.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}

	
	TEST_F(StateWriterTest, testLatLonAltMix) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, not needed
		//sw.setTrkGsVs(true);   // default, not needed
		sw.setPrecision(5);
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		v = Velocity::makeTrkGsVs(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name,time,lat,lon,alt,trk,gs,vs\nunitless,s,deg,deg,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);   // this doesn't really make sense (because XYZ and LatLonAlt points are both being added, but this class doesn't care
		v = Velocity::makeTrkGsVs(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name,time,lat,lon,alt,trk,gs,vs\nunitless,s,deg,deg,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);  // this doesn't really make sense (because XYZ and LatLonAlt points are both being added, but this class doesn't care
		sw.setPrecision(7);
		sw.writeState("ac1",t,p);
		EXPECT_EQ("name,time,lat,lon,alt,trk,gs,vs\nunitless,s,deg,deg,ft,deg,knot,fpm\nac1,10.00000,1.00000,2.00000,3.00000,1.00000,2.00000,3.00000\nac2,15.000000,3.100000,2.100000,1.100000,3.200000,2.200000,1.200000\nac1,20.0000000,4.0000000,5.0000000,6.0000000,0.0000000,0.0000000,0.0000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}

	
	TEST_F(StateWriterTest, testTabSeparator) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		sw.setPrecision(5);
		sw.setColumnDelimiterTab();
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		EXPECT_EQ("name\ttime\tsx\tsy\tsz\tvx\tvy\tvz\nunitless\ts\tNM\tNM\tft\tknot\tknot\tfpm\nac1\t10.00000\t1.00000\t2.00000\t3.00000\t1.00000\t2.00000\t3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());

		sw.setColumnDelimiterComma();
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name\ttime\tsx\tsy\tsz\tvx\tvy\tvz\nunitless\ts\tNM\tNM\tft\tknot\tknot\tfpm\nac1\t10.00000\t1.00000\t2.00000\t3.00000\t1.00000\t2.00000\t3.00000\nac2\t15.000000\t3.100000\t2.100000\t1.100000\t3.200000\t2.200000\t1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		sw.close();
		
	}
	
	
	TEST_F(StateWriterTest, testSpaceSeparator) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		sw.setPrecision(5);
		sw.setColumnDelimiterSpace();
		EXPECT_FALSE(sw.hasError());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		
		EXPECT_EQ("name time sx sy sz vx vy vz\nunitless s NM NM ft knot knot fpm\nac1 10.00000 1.00000 2.00000 3.00000 1.00000 2.00000 3.00000\n", osw.str());
		EXPECT_FALSE(sw.hasError());

		sw.setColumnDelimiterComma();
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.setPrecision(6);
		sw.writeState("ac2",t,p,v);
		EXPECT_EQ("name time sx sy sz vx vy vz\nunitless s NM NM ft knot knot fpm\nac1 10.00000 1.00000 2.00000 3.00000 1.00000 2.00000 3.00000\nac2 15.000000 3.100000 2.100000 1.100000 3.200000 2.200000 1.200000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		sw.close();
		
	}

	
	TEST_F(StateWriterTest, testComment) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasError());

		//sw.setUnit("name","unit");		
		//sw.setParameterPrecision(num);
		//sw.setParameters(pr);
		//sw.setParameters(pr,std::string []);
		
		double t;
		Position p;
		Velocity v;		

		sw.addComment("This is");
		sw.addComment("A Test.");
		sw.addComment("This is\nanother test");
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		sw.addComment("This is a third test");
		
		EXPECT_EQ("# This is\n# A Test.\n# This is\n# another test\nname,time,sx,sy,sz,vx,vy,vz\nunitless,s,NM,NM,ft,knot,knot,fpm\nac1,10.000000,1.000000,2.000000,3.000000,1.000000,2.000000,3.000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		sw.close();
		
	}
	
	
	TEST_F(StateWriterTest, testParameters) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasError());

		//sw.setUnit("name","unit");		
		//sw.setParameterPrecision(num);
		//sw.setParameters(pr);
		//sw.setParameters(pr,std::string []);
		
		double t;
		Position p;
		Velocity v;		

		ParameterData cfg; // = new ParameterData();
		cfg.setInternal("param1",Units::from("NM",1),"NM");
		cfg.setInternal("param2",Units::from("m",2),"mm");
		cfg.setInternal("param3",Units::from("lbm",3),"lbm");
		
		sw.setParameters(cfg);
		sw.addComment("This is a test");
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.0,2.0,3.0);
		sw.writeState("ac1",t,p,v);
		
		EXPECT_EQ("# This is a test\nparam1 = 1.000000 [NM]\nparam2 = 2000.000000 [mm]\nparam3 = 3.000000 [lbm]\nname,time,sx,sy,sz,vx,vy,vz\nunitless,s,NM,NM,ft,knot,knot,fpm\nac1,10.000000,1.000000,2.000000,3.000000,1.000000,2.000000,3.000000\n", osw.str());
		EXPECT_FALSE(sw.hasError());
		
		sw.close();
	}

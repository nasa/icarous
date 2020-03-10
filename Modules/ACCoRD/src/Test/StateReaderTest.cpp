/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework.

#include "Vect4.h"
#include "Units.h"
#include "StateReader.h"
#include "StateWriter.h"
#include "SequenceReader.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

using namespace larcfm;
using namespace std;

class StateReaderTest  : public ::testing::Test {

};

	
	// This is a wrap around test, put something in a StateWriter, then see if StateReader can parse it.
	// This doesn't test all functionality of StateReader, but it is a quick test that StateReader and
	// StateWriter are compatible with each other.
	// If this test fails, be sure that the unit test for StateWriter passes, before trying to debug this
	// test or StateReader
	
	TEST_F(StateReaderTest, testWrapAroundXYZ) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasMessage());

		sw.addComment("This is");
		sw.addComment("A Test.");
		sw.addComment("This is\nanother test");

		ParameterData cfg; // = new ParameterData();
		cfg.set("param1",1,"NM");
		cfg.set("param2",2,"m");
		cfg.set("param3",3,"kg");		
		sw.setParameters(cfg);

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeXYZ(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.2,2.2,3.2);
		sw.writeState("ac1",t,p,v);
		
		t = 20.0;
		p = Position::makeXYZ(4.0,5.0,6.0);
		v = Velocity::makeVxyz(4.2,5.2,6.2);
		sw.writeState("ac1",t,p,v);
		EXPECT_FALSE(sw.hasMessage());
		
		t = 15.0;
		p = Position::makeXYZ(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.writeState("ac2",t,p,v);
		
		t = 25.0;
		p = Position::makeXYZ(6.1,5.1,4.1);
		v = Velocity::makeVxyz(6.2,5.2,4.2);
		sw.writeState("ac2",t,p,v);
		
		t = 35.0;
		p = Position::makeXYZ(9.1,8.1,7.1);
		v = Velocity::makeVxyz(9.2,8.2,7.2);
		sw.writeState("ac2",t,p,v);
		
		sw.close();
		EXPECT_FALSE(sw.hasMessage());
		
		StateReader sr; // = new StateReader();
		istringstream iss(osw.str());
		sr.open(&iss);
		EXPECT_NEAR(1,Units::to("NM",sr.getParametersRef().getValue("param1")),0.00000001);
		EXPECT_NEAR(2,Units::to("m", sr.getParametersRef().getValue("param2")),0.00000001);
		EXPECT_NEAR(3,Units::to("kg",sr.getParametersRef().getValue("param3")),0.00000001);

		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(20.0,sr.getTime(0),0.0000001);
		EXPECT_NEAR(35.0,sr.getTime(1),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(2, as.size());
		EXPECT_EQ("ac1",as.name());	
		
		EXPECT_FALSE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("NM",as.position(0).x()),0.00000001);
		EXPECT_NEAR(2,Units::to("NM",as.position(0).y()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).z()),0.00000001);
		EXPECT_NEAR(1.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(3.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(4,Units::to("NM",as.position(1).x()),0.00000001);
		EXPECT_NEAR(5,Units::to("NM",as.position(1).y()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as.position(1).z()),0.00000001);
		EXPECT_NEAR(4.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(6.2,Units::to("fpm",as.velocity(1).z),0.00000001);
		
		as = sr.getAircraftState(1);
		EXPECT_EQ(3, as.size());
		EXPECT_EQ("ac2",as.name());

		EXPECT_FALSE(as.position(0).isLatLon());
		EXPECT_NEAR(3.1,Units::to("NM",as.position(0).x()),0.00000001);
		EXPECT_NEAR(2.1,Units::to("NM",as.position(0).y()),0.00000001);
		EXPECT_NEAR(1.1,Units::to("ft",as.position(0).z()),0.00000001);
		EXPECT_NEAR(3.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(1.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(6.1,Units::to("NM",as.position(1).x()),0.00000001);
		EXPECT_NEAR(5.1,Units::to("NM",as.position(1).y()),0.00000001);
		EXPECT_NEAR(4.1,Units::to("ft",as.position(1).z()),0.00000001);
		EXPECT_NEAR(6.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(4.2,Units::to("fpm",as.velocity(1).z),0.00000001);
				
		EXPECT_NEAR(9.1,Units::to("NM",as.position(2).x()),0.00000001);
		EXPECT_NEAR(8.1,Units::to("NM",as.position(2).y()),0.00000001);
		EXPECT_NEAR(7.1,Units::to("ft",as.position(2).z()),0.00000001);
		EXPECT_NEAR(9.2,Units::to("kn",as.velocity(2).x),0.00000001);
		EXPECT_NEAR(8.2,Units::to("kn",as.velocity(2).y),0.00000001);
		EXPECT_NEAR(7.2,Units::to("fpm",as.velocity(2).z),0.00000001);
	}
	
	
	TEST_F(StateReaderTest, testWrapAroundLLA) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasMessage());

		sw.addComment("This is");
		sw.addComment("A Test.");
		sw.addComment("This is\nanother test");

		ParameterData cfg; // = new ParameterData();
		cfg.set("param1",1,"NM");
		cfg.set("param2",2,"m");
		cfg.set("param3",3,"kg");		
		sw.setParameters(cfg);

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.2,2.2,3.2);
		sw.writeState("ac1",t,p,v);
		
		t = 20.0;
		p = Position::makeLatLonAlt(4.0,5.0,6.0);
		v = Velocity::makeVxyz(4.2,5.2,6.2);
		sw.writeState("ac1",t,p,v);
		EXPECT_FALSE(sw.hasMessage());
		
		t = 15.0;
		p = Position::makeLatLonAlt(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.writeState("ac2",t,p,v);
		
		t = 25.0;
		p = Position::makeLatLonAlt(6.1,5.1,4.1);
		v = Velocity::makeVxyz(6.2,5.2,4.2);
		sw.writeState("ac2",t,p,v);
		
		t = 35.0;
		p = Position::makeLatLonAlt(9.1,8.1,7.1);
		v = Velocity::makeVxyz(9.2,8.2,7.2);
		sw.writeState("ac2",t,p,v);
		
		sw.close();
		EXPECT_FALSE(sw.hasMessage());
		
		StateReader sr; // = new StateReader();
		istringstream iss(osw.str());
		sr.open(&iss);
		EXPECT_NEAR(1,Units::to("NM",sr.getParametersRef().getValue("param1")),0.00000001);
		EXPECT_NEAR(2,Units::to("m", sr.getParametersRef().getValue("param2")),0.00000001);
		EXPECT_NEAR(3,Units::to("kg",sr.getParametersRef().getValue("param3")),0.00000001);

		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(20.0,sr.getTime(0),0.0000001);
		EXPECT_NEAR(35.0,sr.getTime(1),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(2, as.size());
		EXPECT_EQ("ac1",as.name());	
		
		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(1.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(3.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(4,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as.position(1).alt()),0.00000001);
		EXPECT_NEAR(4.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(6.2,Units::to("fpm",as.velocity(1).z),0.00000001);
		
		as = sr.getAircraftState(1);
		EXPECT_EQ(3, as.size());
		EXPECT_EQ("ac2",as.name());

		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(3.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(1.1,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(3.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(1.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(6.1,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5.1,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(4.1,Units::to("ft",as.position(1).alt()),0.00000001);
		EXPECT_NEAR(6.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(4.2,Units::to("fpm",as.velocity(1).z),0.00000001);
				
		EXPECT_NEAR(9.1,Units::to("deg",as.position(2).lat()),0.00000001);
		EXPECT_NEAR(8.1,Units::to("deg",as.position(2).lon()),0.00000001);
		EXPECT_NEAR(7.1,Units::to("ft",as.position(2).alt()),0.00000001);
		EXPECT_NEAR(9.2,Units::to("kn",as.velocity(2).x),0.00000001);
		EXPECT_NEAR(8.2,Units::to("kn",as.velocity(2).y),0.00000001);
		EXPECT_NEAR(7.2,Units::to("fpm",as.velocity(2).z),0.00000001);
	}
	
	
	
	TEST_F(StateReaderTest, testWrapAroundNoVelocity) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		sw.setVelocity(false);  // default, so not needed
		EXPECT_FALSE(sw.hasMessage());

		double t;
		Position p;
		
		t = 10.0;
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		sw.writeState("ac1",t,p);
		
		t = 20.0;
		p = Position::makeLatLonAlt(4.0,5.0,6.0);
		sw.writeState("ac1",t,p);
		EXPECT_FALSE(sw.hasMessage());
		
		t = 15.0;
		p = Position::makeLatLonAlt(3.1,2.1,1.1);
		sw.writeState("ac2",t,p);
		
		t = 25.0;
		p = Position::makeLatLonAlt(6.1,5.1,4.1);
		sw.writeState("ac2",t,p);
		
		t = 35.0;
		p = Position::makeLatLonAlt(9.1,8.1,7.1);
		sw.writeState("ac2",t,p);
		
		sw.close();
		EXPECT_FALSE(sw.hasMessage());
		
		StateReader sr; // = new StateReader();
		istringstream iss(osw.str());
		sr.open(&iss);

		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(20.0,sr.getTime(0),0.0000001);
		EXPECT_NEAR(35.0,sr.getTime(1),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(2, as.size());
		EXPECT_EQ("ac1",as.name());	
		
		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).alt()),0.00000001);
		
		EXPECT_NEAR(4,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as.position(1).alt()),0.00000001);
		
		as = sr.getAircraftState(1);
		EXPECT_EQ(3, as.size());
		EXPECT_EQ("ac2",as.name());

		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(3.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(1.1,Units::to("ft",as.position(0).alt()),0.00000001);
		
		EXPECT_NEAR(6.1,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5.1,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(4.1,Units::to("ft",as.position(1).alt()),0.00000001);
				
		EXPECT_NEAR(9.1,Units::to("deg",as.position(2).lat()),0.00000001);
		EXPECT_NEAR(8.1,Units::to("deg",as.position(2).lon()),0.00000001);
		EXPECT_NEAR(7.1,Units::to("ft",as.position(2).alt()),0.00000001);
	}

	
	TEST_F(StateReaderTest, testWrapAroundNoTime) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setOutputTime(false);
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasMessage());

		Position p;
		Velocity v;		
		
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.2,2.2,3.2);
		sw.writeState("ac1",p,v);
		
		p = Position::makeLatLonAlt(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.writeState("ac2",p,v);
		
		p = Position::makeLatLonAlt(6.1,5.1,4.1);
		v = Velocity::makeVxyz(6.2,5.2,4.2);
		sw.writeState("ac3",p,v);
		
		p = Position::makeLatLonAlt(9.1,8.1,7.1);
		v = Velocity::makeVxyz(9.2,8.2,7.2);
		sw.writeState("ac4",p,v);
		
		sw.close();
		EXPECT_FALSE(sw.hasMessage());
		
		StateReader sr; // = new StateReader();
		istringstream iss(osw.str());
		sr.open(&iss);

		EXPECT_EQ(4, sr.size());
		EXPECT_NEAR(0.0,sr.getTime(0),0.0000001);
		EXPECT_NEAR(0.0,sr.getTime(1),0.0000001);
		EXPECT_NEAR(0.0,sr.getTime(2),0.0000001);
		EXPECT_NEAR(0.0,sr.getTime(3),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(1, as.size());
		EXPECT_EQ("ac1",as.name());	
		
		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(1.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(3.2,Units::to("fpm",as.velocity(0).z),0.00000001);
				
		as = sr.getAircraftState(1);
		EXPECT_EQ(1, as.size());
		EXPECT_EQ("ac2",as.name());

		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(3.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(1.1,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(3.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(1.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		as = sr.getAircraftState(2);
		EXPECT_EQ(1, as.size());
		EXPECT_EQ("ac3",as.name());
		
		EXPECT_NEAR(6.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(5.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(4.1,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(6.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(4.2,Units::to("fpm",as.velocity(0).z),0.00000001);
				
		as = sr.getAircraftState(3);
		EXPECT_EQ(1, as.size());
		EXPECT_EQ("ac4",as.name());
		
		EXPECT_NEAR(9.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(8.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(7.1,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(9.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(8.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(7.2,Units::to("fpm",as.velocity(0).z),0.00000001);
	}
	
	
	TEST_F(StateReaderTest, testWrapAroundNotConsecutiveName) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasMessage());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.2,2.2,3.2);
		sw.writeState("ac1",t,p,v);
		
		t = 20.0;
		p = Position::makeLatLonAlt(4.0,5.0,6.0);
		v = Velocity::makeVxyz(4.2,5.2,6.2);
		sw.writeState("ac1",t,p,v);
		EXPECT_FALSE(sw.hasMessage());
		
		t = 15.0;
		p = Position::makeLatLonAlt(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.writeState("ac2",t,p,v);
		
		t = 25.0;
		p = Position::makeLatLonAlt(6.1,5.1,4.1);
		v = Velocity::makeVxyz(6.2,5.2,4.2);
		sw.writeState("ac2",t,p,v);
		
		t = 35.0;
		p = Position::makeLatLonAlt(9.1,8.1,7.1);
		v = Velocity::makeVxyz(9.2,8.2,7.2);
		sw.writeState("ac1",t,p,v);
		
		sw.close();
		EXPECT_FALSE(sw.hasMessage());
		
		StateReader sr; // = new StateReader();
		istringstream iss(osw.str());
		sr.open(&iss);
		
		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(35.0,sr.getTime(0),0.0000001);
		EXPECT_NEAR(25.0,sr.getTime(1),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(3, as.size());
		EXPECT_EQ("ac1",as.name());	
		
		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(1.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(3.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(4,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as.position(1).alt()),0.00000001);
		EXPECT_NEAR(4.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(6.2,Units::to("fpm",as.velocity(1).z),0.00000001);
		
		EXPECT_NEAR(9.1,Units::to("deg",as.position(2).lat()),0.00000001);
		EXPECT_NEAR(8.1,Units::to("deg",as.position(2).lon()),0.00000001);
		EXPECT_NEAR(7.1,Units::to("ft",as.position(2).alt()),0.00000001);
		EXPECT_NEAR(9.2,Units::to("kn",as.velocity(2).x),0.00000001);
		EXPECT_NEAR(8.2,Units::to("kn",as.velocity(2).y),0.00000001);
		EXPECT_NEAR(7.2,Units::to("fpm",as.velocity(2).z),0.00000001);
		
		as = sr.getAircraftState(1);
		EXPECT_EQ(2, as.size());
		EXPECT_EQ("ac2",as.name());

		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(3.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(1.1,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(3.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(1.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(6.1,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5.1,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(4.1,Units::to("ft",as.position(1).alt()),0.00000001);
		EXPECT_NEAR(6.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(4.2,Units::to("fpm",as.velocity(1).z),0.00000001);
				
	}
	
	
	TEST_F(StateReaderTest, testWrapAroundNotConsecutiveTime) {
		StateWriter sw; // = new StateWriter();
		ostringstream osw;
		sw.open(&osw);
		//sw.setVelocity(true);  // default, so not needed
		sw.setTrkGsVs(false);
		EXPECT_FALSE(sw.hasMessage());

		double t;
		Position p;
		Velocity v;		
		
		t = 10.0;
		p = Position::makeLatLonAlt(1.0,2.0,3.0);
		v = Velocity::makeVxyz(1.2,2.2,3.2);
		sw.writeState("ac1",t,p,v);
		
		t = 20.0;
		p = Position::makeLatLonAlt(4.0,5.0,6.0);
		v = Velocity::makeVxyz(4.2,5.2,6.2);
		sw.writeState("ac1",t,p,v);
		EXPECT_FALSE(sw.hasMessage());
		
		t = 15.0;
		p = Position::makeLatLonAlt(3.1,2.1,1.1);
		v = Velocity::makeVxyz(3.2,2.2,1.2);
		sw.writeState("ac2",t,p,v);
		
		t = 12.0;
		p = Position::makeLatLonAlt(6.1,5.1,4.1);
		v = Velocity::makeVxyz(6.2,5.2,4.2);
		sw.writeState("ac2",t,p,v);
		
		t = 35.0;
		p = Position::makeLatLonAlt(9.1,8.1,7.1);
		v = Velocity::makeVxyz(9.2,8.2,7.2);
		sw.writeState("ac1",t,p,v);
		
		sw.close();
		EXPECT_FALSE(sw.hasMessage());
		
		StateReader sr; // = new StateReader();
		istringstream iss(osw.str());
		sr.open(&iss);
		EXPECT_FALSE(sr.hasError());
		EXPECT_TRUE(sr.hasMessage());
		EXPECT_EQ("Warning in StateReader(): Time not increasing from 15.0000 to 12.0000 for aircraft ac2, skipping non-consecutive data.\n",sr.getMessage());
		
		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(35.0,sr.getTime(0),0.0000001);
		EXPECT_NEAR(15.0,sr.getTime(1),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(3, as.size());
		EXPECT_EQ("ac1",as.name());	
		
		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(1.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(3.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
		EXPECT_NEAR(4,Units::to("deg",as.position(1).lat()),0.00000001);
		EXPECT_NEAR(5,Units::to("deg",as.position(1).lon()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as.position(1).alt()),0.00000001);
		EXPECT_NEAR(4.2,Units::to("kn",as.velocity(1).x),0.00000001);
		EXPECT_NEAR(5.2,Units::to("kn",as.velocity(1).y),0.00000001);
		EXPECT_NEAR(6.2,Units::to("fpm",as.velocity(1).z),0.00000001);
		
		EXPECT_NEAR(9.1,Units::to("deg",as.position(2).lat()),0.00000001);
		EXPECT_NEAR(8.1,Units::to("deg",as.position(2).lon()),0.00000001);
		EXPECT_NEAR(7.1,Units::to("ft",as.position(2).alt()),0.00000001);
		EXPECT_NEAR(9.2,Units::to("kn",as.velocity(2).x),0.00000001);
		EXPECT_NEAR(8.2,Units::to("kn",as.velocity(2).y),0.00000001);
		EXPECT_NEAR(7.2,Units::to("fpm",as.velocity(2).z),0.00000001);
		
		as = sr.getAircraftState(1);
		EXPECT_EQ(1, as.size());
		EXPECT_EQ("ac2",as.name());

		EXPECT_TRUE(as.position(0).isLatLon());
		EXPECT_NEAR(3.1,Units::to("deg",as.position(0).lat()),0.00000001);
		EXPECT_NEAR(2.1,Units::to("deg",as.position(0).lon()),0.00000001);
		EXPECT_NEAR(1.1,Units::to("ft",as.position(0).alt()),0.00000001);
		EXPECT_NEAR(3.2,Units::to("kn",as.velocity(0).x),0.00000001);
		EXPECT_NEAR(2.2,Units::to("kn",as.velocity(0).y),0.00000001);
		EXPECT_NEAR(1.2,Units::to("fpm",as.velocity(0).z),0.00000001);
		
	}
	
	
	TEST_F(StateReaderTest, testEmptyAircraft) {
		StateReader sr; // = new StateReader();
		istringstream iss("\n\n# Test\naircraft time x y z");
		sr.open(&iss);
		EXPECT_EQ(0, sr.size());
		EXPECT_NEAR(0.0,sr.getTime(0),0.0000001); // really, this method can return anything
		EXPECT_NEAR(0.0,sr.getTime(1),0.0000001); // really, this method can return anything
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ("INVALID",as.name());
	}
	
	
	TEST_F(StateReaderTest, testDitto) {		
		StateReader sr; // = new StateReader();
		istringstream iss("\n\n# Test\naircraft time sx sy sz\nac1 1.0 1.0 2 3\n\" 2.0 4 5 6");
		sr.open(&iss);
		
		EXPECT_EQ(1, sr.size());
		EXPECT_NEAR(2.0,sr.getTime(0),0.0000001);
		
		AircraftState as;
		as = sr.getAircraftState(0);
		EXPECT_EQ(2, as.size());
		EXPECT_EQ("ac1",as.name());	
		EXPECT_FALSE(as.isLatLon());
		
		EXPECT_FALSE(as.position(0).isLatLon());
		EXPECT_NEAR(1,Units::to("NM",as.position(0).x()),0.00000001);
		EXPECT_NEAR(2,Units::to("NM",as.position(0).y()),0.00000001);
		EXPECT_NEAR(3,Units::to("ft",as.position(0).z()),0.00000001);
		
		EXPECT_NEAR(4,Units::to("NM",as.position(1).x()),0.00000001);
		EXPECT_NEAR(5,Units::to("NM",as.position(1).y()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as.position(1).z()),0.00000001);		
	}
	
	TEST_F(StateReaderTest, testExtraColumns) {
		StateReader sr;
		istringstream iss("\n\n# Test\naircraft time foo sx sy sz boogie fun cheese\nac1 1.0 10 1.0 2 3 true 77 cheddar\nac2 2.0 10 4 5 6 true - mahon\nac1 2.0 15 2 3 4 no 3.6 -");
		sr.open(&iss);

		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(2.0, sr.getTime(0), 0.0000001);

		AircraftState as1, as2;
		as1 = sr.getAircraftState(0);
		as2 = sr.getAircraftState(1);
		EXPECT_EQ(2, as1.size());
		EXPECT_EQ("ac1",as1.name());
		EXPECT_FALSE(as1.isLatLon());

		EXPECT_FALSE(as1.position(0).isLatLon());
		EXPECT_NEAR(1,  Units::to("NM",as1.position(0).x()),0.00000001);
		EXPECT_NEAR(2,  Units::to("NM",as1.position(0).y()),0.00000001);
		EXPECT_NEAR(3,  Units::to("ft",as1.position(0).z()),0.00000001);

		EXPECT_NEAR(4,Units::to("NM",as2.position(0).x()),0.00000001);
		EXPECT_NEAR(5,Units::to("NM",as2.position(0).y()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as2.position(0).z()),0.00000001);

		EXPECT_FALSE(sr.getExtraColumnBool(0, "boogie")); // bad data, default to false
		EXPECT_TRUE(sr.getExtraColumnBool(1, "boogie"));
		EXPECT_NEAR(3.6, sr.getExtraColumnValue(0,  "fun"), 0.000000);
		EXPECT_TRUE(std::isnan(sr.getExtraColumnValue(1, "fun"))); // no data
		EXPECT_TRUE(std::isnan(sr.getExtraColumnValue(0, "joy"))); // no data
		EXPECT_TRUE(equals("", sr.getExtraColumnString(0, "cheese"))); // overwritten by blank
		EXPECT_TRUE(equals("mahon", sr.getExtraColumnString(1, "cheese")));
		EXPECT_TRUE(equals("", sr.getExtraColumnString(2, "cheese"))); // no data
	}

	TEST_F(StateReaderTest, testExtraColumnsSeq) {
		SequenceReader sr;
		istringstream iss("\n\n# Test\naircraft time foo sx sy sz boogie fun cheese\nac1 1.0 10 1.0 2 3 true 77 cheddar\nac2 2.0 10 4 5 6 true - mahon\nac1 2.0 15 2 3 4 no 3.6 -");
		sr.open(&iss);

		EXPECT_EQ(2, sr.size());
		EXPECT_NEAR(2.0, sr.getTime(0), 0.0000001);

		AircraftState as1, as2;
		as1 = sr.getAircraftState(0);
		as2 = sr.getAircraftState(1);
		EXPECT_EQ(2, as1.size());
		EXPECT_EQ("ac1",as1.name());
		EXPECT_FALSE(as1.isLatLon());

		EXPECT_FALSE(as1.position(0).isLatLon());
		EXPECT_NEAR(1,  Units::to("NM",as1.position(0).x()),0.00000001);
		EXPECT_NEAR(2,  Units::to("NM",as1.position(0).y()),0.00000001);
		EXPECT_NEAR(3,  Units::to("ft",as1.position(0).z()),0.00000001);

		EXPECT_NEAR(4,Units::to("NM",as2.position(0).x()),0.00000001);
		EXPECT_NEAR(5,Units::to("NM",as2.position(0).y()),0.00000001);
		EXPECT_NEAR(6,Units::to("ft",as2.position(0).z()),0.00000001);

		EXPECT_FALSE(sr.getExtraColumnBool(0, "boogie")); // bad data, default to false
		EXPECT_TRUE(sr.getExtraColumnBool(1, "boogie"));
		EXPECT_NEAR(3.6, sr.getExtraColumnValue(0,  "fun"), 0.000000);
		EXPECT_TRUE(std::isnan(sr.getExtraColumnValue(1, "fun"))); // no data
		EXPECT_TRUE(std::isnan(sr.getExtraColumnValue(0, "joy"))); // no data
		EXPECT_TRUE(equals("", sr.getExtraColumnString(0, "cheese"))); // overwritten by blank
		EXPECT_TRUE(equals("mahon", sr.getExtraColumnString(1, "cheese")));
		EXPECT_TRUE(equals("", sr.getExtraColumnString(2, "cheese"))); // no data

		EXPECT_TRUE(sr.hasExtraColumnData(1.0, "ac1", "cheese"));
		EXPECT_FALSE(sr.hasExtraColumnData(2.0, "ac1", "cheese"));
		EXPECT_TRUE(sr.getExtraColumnBool(1.0, "ac1", "boogie"));
		EXPECT_FALSE(sr.getExtraColumnBool(2.0, "ac1", "boogie"));
		EXPECT_EQ("cheddar", sr.getExtraColumnString(1.0, "ac1", "cheese"));
		EXPECT_EQ("", sr.getExtraColumnString(2.0, "ac1", "cheese"));
		EXPECT_EQ("", sr.getExtraColumnString(2.0, "what", "cheese")); // bad index
		EXPECT_NEAR(77, sr.getExtraColumnValue(1.0,  "ac1",  "fun"), 0.00001);
		EXPECT_NEAR(3.6, sr.getExtraColumnValue(2.0,  "ac1",  "fun"), 0.00001);
		EXPECT_TRUE(isnan(sr.getExtraColumnValue(2.0,  "ac2",  "fun")));

	}

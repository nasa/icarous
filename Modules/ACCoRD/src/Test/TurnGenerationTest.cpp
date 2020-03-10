/*
 * TurnGenerationTest.java 
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "TurnGeneration.h"
#include "KinematicsPosition.h"
#include "KinematicsLatLon.h"
#include "GreatCircle.h"
#include "Plan.h"
#include "PlanUtil.h"
#include "format.h"
// #include <chrono>   //TODO cause problems in RedHat
#include <cmath>
#include <gtest/gtest.h>


using namespace larcfm;

class TurnGenerationTest : public ::testing::Test {
  // public:
  // Plan fp;
  // Plan lpc;

protected:
  virtual void SetUp() {
  }
};

	
	
	TEST_F(TurnGenerationTest, testTurnGenerationPositionEucl) {
		Vect3 v0(0,0,0);
		Vect3 v1(10,0,1);
		Vect3 v2(10,10,2);
		Position p0(v0);
		Position p1(v1);
		Position p2(v2);
		double R = 6;
		Tuple6<Position,Position,Position,int,double,Position> turns = TurnGeneration::turnGenerator(p0, p1, p2, R);
		Position BOT = turns.first;
		Position MOT = turns.second;
		Position EOT = turns.third;
		
		EXPECT_NEAR(4.0,BOT.x(),0.001);
        EXPECT_NEAR(0.0,BOT.y(),0.001);
        EXPECT_NEAR(0,BOT.z(),0.001);          // By contract, turnGenerator for Positions returns an altitude for BOT as p1.alt()        
        EXPECT_NEAR(8.243,MOT.x(),0.001);
        EXPECT_NEAR(1.757,MOT.y(),0.001);
        EXPECT_NEAR(0.0,MOT.z(),0.001);        // By contract, turnGenerator for Positions returns an altitude for MOT as p1.alt()
        EXPECT_NEAR(10.0,EOT.x(),0.001);
        EXPECT_NEAR(6.0,EOT.y(),0.001);
        EXPECT_NEAR(0,EOT.z(),0.001);          // By contract, turnGenerator for Positions returns an altitude for EOT as p1.alt()
	}
	
	
	
	TEST_F(TurnGenerationTest, testTurnGenerationPositionLL) {  
		Position p1(LatLonAlt::make( 0.000,   -10.000,   10.0000));
		Position p2(LatLonAlt::make( 0.000,     0.000, 6000.0000));
		Position p3(LatLonAlt::make(10.000,     0.000, 6000.0000));
		double radius = Units::from("NM",60.0);
		Tuple6<Position,Position,Position,int,double,Position> turns = TurnGeneration::turnGenerator(p1, p2, p3, radius);
		Position BOT = turns.first;
		Position MOT = turns.second;
		Position EOT = turns.third;	
		
	    EXPECT_NEAR(Units::from("deg",  0.0),BOT.lat(),0.0001);
	    EXPECT_NEAR(Units::from("deg", -1.0),BOT.lon(),0.0001);
	    EXPECT_NEAR(Units::from("ft",  10.0),BOT.alt(),0.000001);
	    EXPECT_NEAR(Units::from("deg",  0.5),MOT.lat(),0.01);
	    EXPECT_NEAR(Units::from("deg", -0.5),MOT.lon(),0.01);
	    EXPECT_NEAR(Units::from("ft",  10.0),MOT.alt(),0.0001);
	    EXPECT_NEAR(Units::from("deg",  1.0),EOT.lat(),0.0005);
	    EXPECT_NEAR(Units::from("deg",  0.0),EOT.lon(),0.0000000000001);
	    EXPECT_NEAR(Units::from("ft",  10.0),EOT.alt(),0.0000000000001);                   	    	    
	}
	
	TEST_F(TurnGenerationTest, testTurnGenerationNavPointLL) {  
		Position p2(LatLonAlt::make(0.0, -10.0, 6000.0000));
		Position p3(LatLonAlt::make(0.0,   0.0, 6000.0000));
		Position p4(LatLonAlt::make(5.0,   0.0, 6000.0000));
		NavPoint np2(p2,10.0000);    
		NavPoint np3(p3,20.0000);    
		NavPoint np4(p4,30.0000);    
		
		double radius = Units::from("NM",0.5);
		Tuple5<NavPoint,NavPoint,NavPoint,int,Position> pG = TurnGeneration::turnGenerator(np2, np3, np4, radius);
		NavPoint BOT = pG.first;
		NavPoint MOT = pG.second;
		NavPoint EOT = pG.third;
        EXPECT_NEAR(0.0,Units::to("deg",BOT.lat()),0.001);
        EXPECT_NEAR(-0.00833,Units::to("deg",BOT.lon()),0.001);  // note: 1 deg = 60 NM, so 0.5NM = 0.008333 deg
        EXPECT_NEAR(6000.0,Units::to("ft",BOT.alt()),0.1);
        EXPECT_NEAR(19.993455,Units::to("s",BOT.time()),0.01);
        EXPECT_NEAR(0.00833,Units::to("deg",EOT.lat()),0.001);
        EXPECT_NEAR(0.0,Units::to("deg",EOT.lon()),0.001);
        EXPECT_NEAR(6000.0,Units::to("ft",EOT.alt()),0.001);
        EXPECT_NEAR(20.0065,Units::to("s",EOT.time()),0.01);    
		
        Quad<NavPoint,NavPoint,NavPoint,int> pGp = TurnGeneration::turnGeneratorProjected(np2, np3, np4, radius);
		BOT = pGp.first;
		MOT = pGp.second;
		EOT = pGp.third;
        EXPECT_NEAR(0.0,Units::to("deg",BOT.lat()),0.001);
        EXPECT_NEAR(-0.00833,Units::to("deg",BOT.lon()),0.001);  // note: 1 deg = 60 NM, so 0.5NM = 0.008333 deg
        EXPECT_NEAR(6000.0,Units::to("ft",BOT.alt()),0.1);
        EXPECT_NEAR(19.993455,Units::to("s",BOT.time()),0.01);
        EXPECT_NEAR(0.00833,Units::to("deg",EOT.lat()),0.001);
        EXPECT_NEAR(0.0,Units::to("deg",EOT.lon()),0.001);
        EXPECT_NEAR(6000.0,Units::to("ft",EOT.alt()),0.001);
        EXPECT_NEAR(20.0065,Units::to("s",EOT.time()),0.01);
	}
	
	

	
	TEST_F(TurnGenerationTest, testTurnGeneratorLooseCompareLL) {  
		Plan lpc("LinPlan");
		Position p2(LatLonAlt::make(34.0357, -117.7120, 6000.0000));
		Position p3(LatLonAlt::make(34.0509, -117.7582, 6000.0000));
		Position p4(LatLonAlt::make(34.0521, -117.7595, 6000.0000));
		Position p5(LatLonAlt::make(34.0774, -117.7882, 8300));
		Position p6(LatLonAlt::make(34.1299, -117.8478, 8300));
		NavPoint np2 = NavPoint(p2,11340.0000).makeName("np2");     lpc.addNavPoint(np2);
		NavPoint np3 = NavPoint(p3,11401.0542).makeName("np3");     lpc.addNavPoint(np3);
		NavPoint np4 = NavPoint(p4,11403.0000).makeName("np4");     lpc.addNavPoint(np4);
		NavPoint np5 = NavPoint(p5,11446.8730).makeName("np5");     lpc.addNavPoint(np5);
		NavPoint np6 = NavPoint(p6,11537.5605).makeName("np6");     lpc.addNavPoint(np6);
		EXPECT_EQ(5,lpc.size());
		EXPECT_TRUE(lpc.isWellFormed());
		
		double radius = Units::from("NM",0.5);
		Quad<NavPoint,NavPoint,NavPoint,int> tGorig = TurnGeneration::turnGeneratorProjected(np2, np3, np4, radius);
		NavPoint BOTorig = tGorig.first;
		NavPoint MOTorig = tGorig.second;
		NavPoint EOTorig = tGorig.third;

		Tuple5<NavPoint,NavPoint,NavPoint, int, Position> pG = TurnGeneration::turnGenerator(np2, np3, np4, radius);
		NavPoint BOT = pG.first;
		NavPoint MOT = pG.second;
		NavPoint EOT = pG.third;

        EXPECT_NEAR(0.5939,BOT.lat(),0.001);
        EXPECT_NEAR(-2.0544,BOT.lon(),0.001);
        EXPECT_NEAR(1828.8,BOT.alt(),0.1);
        EXPECT_NEAR(11398.203,BOT.time(),0.01);
        EXPECT_NEAR(0.594,EOT.lat(),0.001);
        EXPECT_NEAR(-2.055,EOT.lon(),0.001);
        EXPECT_NEAR(1828.8,EOT.alt(),0.001);
        EXPECT_NEAR(11403.905,EOT.time(),0.01);    
		
		//
		// Compare
		//
	    EXPECT_NEAR(BOTorig.lat(),BOT.lat(),0.001);
	    EXPECT_NEAR(BOTorig.lon(),BOT.lon(),0.001);
	    EXPECT_NEAR(BOTorig.alt(),BOT.alt(),0.0001);
	    EXPECT_NEAR(BOTorig.time(),BOT.time(),0.01);
	    
	    EXPECT_NEAR(MOTorig.lat(),MOT.lat(),0.001);
	    EXPECT_NEAR(MOTorig.lon(),MOT.lon(),0.001);
	    EXPECT_NEAR(MOTorig.alt(),MOT.alt(),0.001);
	    EXPECT_NEAR(MOTorig.time(),MOT.time(),0.001);
	    
	    EXPECT_NEAR(EOTorig.lat(),EOT.lat(),0.001);
	    EXPECT_NEAR(EOTorig.lon(),EOT.lon(),0.001);
	    EXPECT_NEAR(EOTorig.alt(),EOT.alt(),0.001);
	    EXPECT_NEAR(EOTorig.time(),EOT.time(),0.01);

	    //
	    // Check Plans
	    //
	    Plan kpcOrig("KinPlanOrig");
 		kpcOrig.addNavPoint(np2);
 		kpcOrig.addNavPoint(BOTorig);
 		kpcOrig.addNavPoint(MOTorig);
		kpcOrig.addNavPoint(EOTorig);
		EXPECT_EQ(4,kpcOrig.size());
		EXPECT_TRUE(kpcOrig.isWellFormed());
	    EXPECT_TRUE(kpcOrig.isTurnConsistent(3, 0.3, false));

	    Plan kpc("KinPlanOrig");
 		kpc.addNavPoint(np2);
 		kpc.addNavPoint(BOT);
 		kpc.addNavPoint(MOT);
		kpc.addNavPoint(EOT);
		EXPECT_EQ(4,kpc.size());
		EXPECT_TRUE(kpc.isWellFormed());
	    EXPECT_TRUE(kpc.isTurnConsistent(3, 0.3,  false));
	}

	
	
	TEST_F(TurnGenerationTest, testTurnGeneratorLooseCompareLL2) {  
		Plan lpc("LinPlan");
		Position p2(LatLonAlt::make(34.0357, -117.7120, 6000.0000));
		Position p5(LatLonAlt::make(34.0774, -117.7882, 8300));
		Position p6(LatLonAlt::make(34.1299, -117.8478, 9300));
		NavPoint np2 = NavPoint(p2,11340.0000).makeName("np2");     lpc.addNavPoint(np2);
		NavPoint np5 = NavPoint(p5,11446.8730).makeName("np5");     lpc.addNavPoint(np5);
		NavPoint np6 = NavPoint(p6,11537.5605).makeName("np6");     lpc.addNavPoint(np6);

		double radius = Units::from("NM",0.5);
		Quad<NavPoint,NavPoint,NavPoint,int> tGorig = TurnGeneration::turnGeneratorProjected(np2, np5, np6, radius);
		NavPoint BOTorig = tGorig.first;
		NavPoint MOTorig = tGorig.second;
		NavPoint EOTorig = tGorig.third;		
			
		EXPECT_NEAR(0.5947,BOTorig.lat(),0.0001);
        EXPECT_NEAR(-2.0557,BOTorig.lon(),0.0001);
        EXPECT_NEAR(2520.8355,BOTorig.alt(),0.0001);
        EXPECT_NEAR(11445.5002,BOTorig.time(),0.0001);
        EXPECT_NEAR(0.59477,EOTorig.lat(),0.0001);
        EXPECT_NEAR(-2.05580,EOTorig.lon(),0.0001);
        EXPECT_NEAR(2534.4536,EOTorig.alt(),0.0001);
        EXPECT_NEAR(11448.2457,EOTorig.time(),0.0001);

		Tuple5<NavPoint,NavPoint,NavPoint,int,Position> pG = TurnGeneration::turnGenerator(np2, np5, np6, radius);
		NavPoint BOT = pG.first;
		NavPoint MOT = pG.second;
		NavPoint EOT = pG.third;

		EXPECT_NEAR(0.5947,BOT.lat(),0.0001);
        EXPECT_NEAR(-2.0557,BOT.lon(),0.0001);
        EXPECT_NEAR(2520.8355,BOT.alt(),0.00011);            
        EXPECT_NEAR(11445.5002,BOT.time(),0.005);
        EXPECT_NEAR(0.59477,EOT.lat(),0.0001);
        EXPECT_NEAR(-2.05580,EOT.lon(),0.0001);
        EXPECT_NEAR(2534.4536,EOT.alt(),0.0001);
        EXPECT_NEAR(11448.2457,EOT.time(),0.005);
        
        //
        // Compare
        //
	    EXPECT_NEAR(BOTorig.lat(),BOT.lat(),0.001);
	    EXPECT_NEAR(BOTorig.lon(),BOT.lon(),0.001);
	    EXPECT_NEAR(BOTorig.alt(),BOT.alt(),0.001);                 
	    EXPECT_NEAR(BOTorig.time(),BOT.time(),0.01);
	    EXPECT_NEAR(MOTorig.lat(),MOT.lat(),0.001);
	    EXPECT_NEAR(MOTorig.lon(),MOT.lon(),0.001);
	    EXPECT_NEAR(MOTorig.alt(),MOT.alt(),0.001);
	    EXPECT_NEAR(MOTorig.time(),MOT.time(),0.001);
	    EXPECT_NEAR(EOTorig.lat(),EOT.lat(),0.001);
	    EXPECT_NEAR(EOTorig.lon(),EOT.lon(),0.001);
	    EXPECT_NEAR(EOTorig.alt(),EOT.alt(),0.001);
	    EXPECT_NEAR(EOTorig.time(),EOT.time(),0.01);

        //
	    // Check Plans
	    //
	    Plan kpcOrig("KinPlanOrig");
 		kpcOrig.addNavPoint(np2);
 		kpcOrig.addNavPoint(BOTorig);
		kpcOrig.addNavPoint(MOTorig);
		kpcOrig.addNavPoint(EOTorig);
 		kpcOrig.addNavPoint(np6);
		EXPECT_EQ(5,kpcOrig.size());
		EXPECT_TRUE(kpcOrig.isWellFormed());
	    EXPECT_TRUE(kpcOrig.isTurnConsistent(3, 0.3,  true));

	    Plan kpc("KinPlan");
 		kpc.addNavPoint(np2);
 		kpc.addNavPoint(BOTorig);
		kpc.addNavPoint(MOTorig);
		kpc.addNavPoint(EOTorig);
 		kpc.addNavPoint(np6);
		EXPECT_EQ(5,kpc.size());
		EXPECT_TRUE(kpc.isWellFormed());
	    EXPECT_TRUE(kpc.isTurnConsistent(3, 0.3, true));
	}

	
	TEST_F(TurnGenerationTest, testTurnGenerationWithShortSegments) {
		Position p2(Vect3(Units::from("nmi",0.5), Units::from("nmi",0.5), Units::from("ft",10000)));   
		Position p3(Vect3(Units::from("nmi",1.0), Units::from("nmi",0.5), Units::from("ft",10000)));  
		Position p4(Vect3(Units::from("nmi",1.0), Units::from("nmi",1.0), Units::from("ft",10000)));
		double radius = Units::from("NM", 1.0);
		Tuple6<Position, Position, Position, int, double,Position> turnPosition = TurnGeneration::turnGenerator(p2, p3, p4, radius);
		Position BOTp = turnPosition.first;
		Position MOTp = turnPosition.second;
		Position EOTp = turnPosition.third;
		
	    EXPECT_NEAR(Units::from("NM",     0.0),BOTp.x(),0.001);
	    EXPECT_NEAR(Units::from("NM",     0.5),BOTp.y(),0.001);
	    EXPECT_NEAR(Units::from("ft", 10000.0),BOTp.z(),0.001);                 
	    EXPECT_NEAR(Units::from("NM",   std::sin(M_PI/4)),MOTp.x(),0.001);
	    EXPECT_NEAR(Units::from("NM",   1.5-std::cos(M_PI/4)),MOTp.y(),0.001);
	    EXPECT_NEAR(Units::from("ft", 10000.0),MOTp.z(),0.001);
	    EXPECT_NEAR(Units::from("NM",     1.0),EOTp.x(),0.001);
	    EXPECT_NEAR(Units::from("NM",     1.5),EOTp.y(),0.001);
	    EXPECT_NEAR(Units::from("ft", 10000.0),EOTp.z(),0.001);

	    // With NavPoints
		
		NavPoint np2(p2,10.0);
		NavPoint np3(p3,20.0);
		NavPoint np4(p4,30.0);
		Tuple5<NavPoint, NavPoint, NavPoint, int, Position> turnNavPoint = TurnGeneration::turnGenerator(np2, np3, np4, radius);
		NavPoint BOTn = turnNavPoint.first;
		NavPoint MOTn = turnNavPoint.second;
		NavPoint EOTn = turnNavPoint.third;		

	    EXPECT_NEAR(Units::from("NM",     0.0),BOTn.x(),0.001);
	    EXPECT_NEAR(Units::from("NM",     0.5),BOTn.y(),0.001);
	    EXPECT_NEAR(Units::from("ft", 10000.0),BOTn.z(),0.001);                 
	    EXPECT_NEAR(Units::from("NM",   std::sin(M_PI/4)),MOTp.x(),0.001);
	    EXPECT_NEAR(Units::from("NM",   1.5-std::cos(M_PI/4)),MOTp.y(),0.001);
	    EXPECT_NEAR(Units::from("ft", 10000.0),MOTn.z(),0.001);
	    EXPECT_NEAR(Units::from("NM",     1.0),EOTn.x(),0.001);
	    EXPECT_NEAR(Units::from("NM",     1.5),EOTn.y(),0.001);
	    EXPECT_NEAR(Units::from("ft", 10000.0),EOTn.z(),0.001);
	    EXPECT_NEAR(4.2920,Units::to("s", BOTn.time()),0.001);
	    EXPECT_NEAR(20.0,Units::to("s", MOTn.time()),0.001);
	    EXPECT_NEAR(35.70796,Units::to("s", EOTn.time()),0.001);
	}



	
	
	TEST_F(TurnGenerationTest, testTurnGeneratorBasicEucl) {
		Vect3 p0(0,0,0);
		Vect3 p1(10,0,1);
		Vect3 p2(10,10,2);
		NavPoint np0(Position(p0),0);
		NavPoint np1(Position(p1),100);
		NavPoint np2(Position(p2),200);
		double R = 6;
		Tuple5<NavPoint,NavPoint,NavPoint,int,Position> turns = TurnGeneration::turnGenerator(np0, np1, np2, R);
		NavPoint BOT = turns.first;
		NavPoint MOT = turns.second;
		NavPoint EOT = turns.third;

		EXPECT_NEAR(4.0,BOT.x(),0.001);
        EXPECT_NEAR(0.0,BOT.y(),0.001);
        EXPECT_NEAR(0.528,BOT.z(),0.001);                   // NOT 0.4
        EXPECT_NEAR(52.8761,BOT.time(),0.001);
        EXPECT_NEAR(8.243,MOT.x(),0.001);
        EXPECT_NEAR(1.757,MOT.y(),0.001);
        EXPECT_NEAR(1.0,MOT.z(),0.001);
        EXPECT_NEAR(100,MOT.time(),0.001);
        EXPECT_NEAR(52.8761,BOT.time(),0.001);
        EXPECT_NEAR(10.0,EOT.x(),0.001);
        EXPECT_NEAR(6.0,EOT.y(),0.001);
        EXPECT_NEAR(1.4712,EOT.z(),0.001);                   // NOT 1.6
        EXPECT_NEAR(147.123,EOT.time(),0.001);

		Quad<NavPoint,NavPoint,NavPoint,int> turnsOrig = TurnGeneration::turnGeneratorProjected(np0, np1, np2, R);
		NavPoint BOT2 = turnsOrig.first;
		NavPoint MOT2 = turnsOrig.second;
		NavPoint EOT2 = turnsOrig.third;

	    EXPECT_NEAR(BOT2.x(),BOT.x(),0.001);
	    EXPECT_NEAR(BOT2.y(),BOT.y(),0.001);
	    EXPECT_NEAR(BOT2.z(),BOT.z(),0.001);
	    EXPECT_NEAR(BOT2.time(),BOT.time(),0.001);
	    EXPECT_NEAR(MOT2.x(),MOT.x(),0.001);
	    EXPECT_NEAR(MOT2.y(),MOT.y(),0.001);
	    EXPECT_NEAR(MOT2.z(),MOT.z(),0.001);
	    EXPECT_NEAR(MOT2.time(),MOT.time(),0.001);
	    EXPECT_NEAR(EOT2.x(),EOT.x(),0.001);
	    EXPECT_NEAR(EOT2.y(),EOT.y(),0.001);
	    EXPECT_NEAR(EOT2.z(),EOT.z(),0.001);
	    EXPECT_NEAR(EOT2.time(),EOT.time(),0.001);

		Plan kpc("KinEuclidean");
		kpc.addNavPoint(np0);
		kpc.addNavPoint(np1);
		kpc.addNavPoint(np2);
		kpc.addNavPoint(BOT);
		kpc.addNavPoint(EOT);
		EXPECT_TRUE(kpc.isWellFormed());
		EXPECT_TRUE(kpc.isWeakConsistent());

		Velocity v01 = NavPoint::initialVelocity(np0,np1);
		Velocity v0B = NavPoint::initialVelocity(np0,BOT);
		EXPECT_NEAR(v01.trk(),v0B.trk(),0.001);
		//EXPECT_NEAR(v01.gs(),v0B.gs(),0.001);  // gs shouldn't match because path is shorter
		EXPECT_NEAR(v01.vs(),v0B.vs(),0.001);

		Velocity v12 = NavPoint::initialVelocity(np1,np2);
		Velocity vE2 = NavPoint::initialVelocity(EOT,np2);
		EXPECT_NEAR(v12.trk(),vE2.trk(),0.001);
		EXPECT_NEAR(v12.vs(),vE2.vs(),0.001);

	}



	TEST_F(TurnGenerationTest, testTurnGenerationClimbEucl) {
		Position p1(Vect3(Units::from("NM",0.0), Units::from("NM",0.0), Units::from("ft",10000)));
		Position p2(Vect3(Units::from("NM",10.0), Units::from("NM",0.0), Units::from("ft",12666.6667)));
		double legLength = Units::from("NM",2.4);
		Position p3(Vect3(Units::from("NM",10.0), legLength, Units::from("ft",13303.6667)));
		Position p4(Vect3(Units::from("NM",0.0), Units::from("NM",10.0), Units::from("ft",16656.0679)));
		NavPoint np1(p1,0.0);
		NavPoint np2(p2,200.0);
		NavPoint np3(p3,248.0);
		NavPoint np4(p4,499.2051);

		Plan lpc("LinEuclidean");
		lpc.addNavPoint(np1);
		lpc.addNavPoint(np2);
		lpc.addNavPoint(np3);
		lpc.addNavPoint(np4);

		double radius = Units::from("NM",1.0);
		Tuple5<NavPoint,NavPoint,NavPoint,int,Position> turns = TurnGeneration::turnGenerator(np1, np2, np3, radius);
		NavPoint BOT = turns.first;
		NavPoint MOT = turns.second;
		NavPoint EOT = turns.third;
        EXPECT_NEAR(16668.0,BOT.x(),0.001);
        EXPECT_NEAR(0.0,BOT.y(),0.001);
        EXPECT_NEAR(3796.962,BOT.z(),0.001);
        EXPECT_NEAR(184.292,BOT.time(),0.001);
        EXPECT_NEAR(18520.0,EOT.x(),0.001);
        EXPECT_NEAR(1852,EOT.y(),0.001);
        EXPECT_NEAR(3924.337,EOT.z(),0.001);
        EXPECT_NEAR(215.707,EOT.time(),0.001);

		Quad<NavPoint,NavPoint,NavPoint,int> turnsOrig = TurnGeneration::turnGeneratorProjected(np1, np2, np3, radius);
		NavPoint BOT2 = turnsOrig.first;
		NavPoint MOT2 = turnsOrig.second;
		NavPoint EOT2 = turnsOrig.third;
	    EXPECT_NEAR(BOT2.x(),BOT.x(),0.001);
	    EXPECT_NEAR(BOT2.y(),BOT.y(),0.001);
	    EXPECT_NEAR(BOT2.z(),BOT.z(),0.001);
	    EXPECT_NEAR(BOT2.time(),BOT.time(),0.001);
	    EXPECT_NEAR(MOT2.x(),MOT.x(),0.001);
	    EXPECT_NEAR(MOT2.y(),MOT.y(),0.001);
	    EXPECT_NEAR(MOT2.z(),MOT.z(),0.001);
	    EXPECT_NEAR(MOT2.time(),MOT.time(),0.001);
	    EXPECT_NEAR(EOT2.x(),EOT.x(),0.001);
	    EXPECT_NEAR(EOT2.y(),EOT.y(),0.001);
	    EXPECT_NEAR(EOT2.z(),EOT.z(),0.001);
	    EXPECT_NEAR(EOT2.time(),EOT.time(),0.001);
	    //EXPECT_TRUE(lpc.isWeakConsistent());                   //  This is not true because not generating BVS EVS and BGS EGS

		Plan kpc("KinEuclidean");
		kpc.addNavPoint(np1);
		kpc.addNavPoint(BOT);
		kpc.addNavPoint(MOT);
		kpc.addNavPoint(EOT);
		kpc.addNavPoint(np4);
        EXPECT_TRUE(kpc.isWellFormed());
	    EXPECT_TRUE(kpc.isTurnConsistent(3, 0.3, true));

        turns = TurnGeneration::turnGenerator(np2, np3, np4, 1.3*radius);
        BOT = turns.first;
        MOT = turns.second;
        EOT = turns.third;
        EXPECT_NEAR(18520.0,BOT.x(),0.001);
        EXPECT_NEAR(3250.569,BOT.y(),0.001);
        EXPECT_NEAR(4006.531,BOT.z(),0.001);
        EXPECT_NEAR(236.027,BOT.time(),0.001);
        EXPECT_NEAR(17569.198,EOT.x(),0.001);
        EXPECT_NEAR(5167.409,EOT.y(),0.001);
        EXPECT_NEAR(4103.655,EOT.z(),0.001);
        EXPECT_NEAR(259.972,EOT.time(),0.001);

		kpc = Plan("KinEuclidean");
		kpc.addNavPoint(np1);
		kpc.addNavPoint(BOT);
		kpc.addNavPoint(MOT);
		kpc.addNavPoint(EOT);
        kpc.addNavPoint(BOT);
		kpc.addNavPoint(MOT);
        kpc.addNavPoint(EOT);
		kpc.addNavPoint(np4);
        EXPECT_TRUE(kpc.isWellFormed());
	    EXPECT_TRUE(kpc.isTurnConsistent(3, 0.3,  true));
 	}




	TEST_F(TurnGenerationTest, testTurnGeneration1) {
		double t2 = 640;
		Position p1(LatLonAlt::make(34.036, -117.11, 6000.0));
		Position p2(LatLonAlt::make(34.051, -117.74, 6000.0));
		NavPoint np1(p1, 340);
		NavPoint np2(p2, t2);
		double step = Units::from("NM",0.4);
		for (double radius = step; radius <=  20.0*step; radius = radius + step) {
			for (double lon = -117.76; lon < -117.0; lon = lon + 0.01) {
				for (double lat = 33.3; lat < 35.5; lat = lat + 0.05) {
					if (lat > 33.8 && lat < 35) continue;     // ensure EOT is not too close to np3
					Position p3(LatLonAlt::make(lat, lon, 6000.0000));
					double dist23 = p2.distanceH(p3);
					double dt23 = dist23/Units::from("kn",200);
					NavPoint np3(p3,t2+dt23);
					Velocity vin = NavPoint::initialVelocity(np1,np2);
					double trk1 = vin.compassAngle();
					double trk2 = NavPoint::initialVelocity(np2,np3).compassAngle();
					double deltaTrack = Util::turnDelta(trk1,trk2);
					if (deltaTrack < Units::from("deg",150)) {   // 160 and above causes failures
						Tuple5<NavPoint,NavPoint,NavPoint, int, Position> tg = TurnGeneration::turnGenerator(np1, np2, np3, radius);
						NavPoint BOT = tg.getFirst();
						NavPoint EOT = tg.getThird();
						//int dir = tg.getFourth();
						Position center = tg.fifth;
						Velocity vinBOT = BOT.position().initialVelocity(np2.position(),100);
						//vin = vin.mkTrk(vin2.trk());
					    double distCenterToBOT = center.distanceH(BOT.position());
					    double distCenterToEOT = center.distanceH(EOT.position());
					    double distDelta = std::abs(distCenterToBOT - distCenterToEOT);
					    double radiusDelta = std::abs(distCenterToBOT - radius);
					    EXPECT_NEAR(0.0,distDelta,2E-8);  // WAS 1E-8
					    EXPECT_NEAR(0.0,radiusDelta,2E-1);
					    EXPECT_NEAR(0.0,radiusDelta/radius, 2E-5);
					    // continuity checks
//					    double trkOutBOT = Plan::trkInTurn(BOT.position(), center, dir);
//					    double trkDelta2 = Util::turnDelta(vinBOT.trk(),trkOutBOT);
//					    EXPECT_NEAR(0.0,trkDelta2,7E-6);
					}
				}
			}
		}
	}



	TEST_F(TurnGenerationTest, testTurnGen_LLA_0) {
		LatLonAlt p1  = LatLonAlt::make(34.00, -117.10, 6000.0);
		LatLonAlt p2  = LatLonAlt::make(34.02, -117.95, 6000.0);
		double step = Units::from("NM",0.4);
		for (double radius = step; radius <=  20.0*step; radius = radius + step) {
			for (double lon = -117.76; lon < -117.0; lon = lon + 0.01) {
				for (double lat = 33.3; lat < 35.5; lat = lat + 0.05) {
					if (lat > 33.8 && lat < 35) continue;     // ensure EOT is not too close to np3
					LatLonAlt p3  = LatLonAlt::make(lat, lon, 6000.0000);
					double trkIn = GreatCircle::final_course(p1,p2);
					double trkOut = GreatCircle::initial_course(p2,p3);
					double deltaTrack = Util::turnDelta(trkIn,trkOut);
					if (deltaTrack < Units::from("deg",150)) {   // 160 and above causes failures
						Tuple6<LatLonAlt,LatLonAlt,LatLonAlt,int,double,LatLonAlt> tg = TurnGeneration::turnGeneratorLLA_orig(p1, p2, p3, radius);
						LatLonAlt BOT = tg.getFirst();
						LatLonAlt EOT = tg.getThird();
						int dir = tg.getFourth();
						LatLonAlt center = tg.sixth;
						double trkBOTp2 = GreatCircle::initial_course(BOT,p2);
					    double distCenterToBOT = center.distanceH(BOT);
					    double distCenterToEOT = center.distanceH(EOT);
					    double distDelta = std::abs(distCenterToBOT - distCenterToEOT);
					    double radiusDelta = std::abs(distCenterToBOT - radius);
					    EXPECT_NEAR(0.0,distDelta,1E-8);
					    EXPECT_NEAR(0.0,radiusDelta,2E-1);
					    EXPECT_NEAR(0.0,radiusDelta/radius, 2E-5);
					    // continuity checks
					    //double trkOutBOT = Plan.trkOutInTurn(BOT, center, dir);
						double final_course = GreatCircle::final_course(center,BOT);
						double trkOutBOT = final_course + dir*M_PI/2;
					    double trkDelta2 = Util::turnDelta(trkBOTp2,trkOutBOT);
					    EXPECT_NEAR(0.0,trkDelta2,7E-6);
					}
				}
			}
		}
	}


	TEST_F(TurnGenerationTest, testTurnGen_LLA_Alt2) {
		LatLonAlt p1  = LatLonAlt::make(34.00, -117.10, 6000.0);
		LatLonAlt p2  = LatLonAlt::make(34.02, -117.95, 6000.0);
		double step = Units::from("NM",0.4);
		for (double radius = step; radius <=  20.0*step; radius = radius + step) {
			for (double lon = -117.76; lon < -117.0; lon = lon + 0.01) {
				for (double lat = 33.3; lat < 35.5; lat = lat + 0.05) {
					if (lat > 33.8 && lat < 35) continue;     // ensure EOT is not too close to np3
					LatLonAlt p3  = LatLonAlt::make(lat, lon, 6000.0000);
					double trkIn = GreatCircle::final_course(p1,p2);
					double trkOut = GreatCircle::initial_course(p2,p3);
					double deltaTrack = Util::turnDelta(trkIn,trkOut);
					if (deltaTrack < Units::from("deg",150)) {   // 160 and above causes failures
						//Tuple6<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double,LatLonAlt> tg = TurnGeneration::turnGeneratorLLA_Alt2(p2, trkIn, trkOut, radius);
						Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,LatLonAlt> tg =  TurnGeneration::turnGeneratorLLA_Alt2(p1,p2,p3,radius);
						LatLonAlt BOT = tg.getFirst();
						LatLonAlt EOT = tg.getThird();
						int dir = tg.getFourth();
						LatLonAlt center = tg.fifth;
						double trkBOTp2 = GreatCircle::initial_course(BOT,p2);
					    double distCenterToBOT = center.distanceH(BOT);
					    double distCenterToEOT = center.distanceH(EOT);
					    double distDelta = std::abs(distCenterToBOT - distCenterToEOT);
					    double radiusDelta = std::abs(distCenterToBOT - radius);
					    EXPECT_NEAR(0.0,distDelta,1E-6);
					    EXPECT_NEAR(0.0,radiusDelta,5E-7);
					    EXPECT_NEAR(0.0,radiusDelta/radius, 1E-10);
					    // continuity checks
					    //double trkOutBOT = Plan.trkOutInTurn(BOT, center, dir);
						double final_course = GreatCircle::final_course(center,BOT);
						double trkOutBOT = final_course + dir*M_PI/2;
					    double trkDelta2 = Util::turnDelta(trkBOTp2,trkOutBOT);
					    EXPECT_NEAR(0.0,trkDelta2,1E-10);
					}
				}
			}
		}
	}



	TEST_F(TurnGenerationTest, testTurnGen_LLA_Alt3) {
		LatLonAlt p1  = LatLonAlt::make(34.00, -117.10, 6000.0);
		LatLonAlt p2  = LatLonAlt::make(34.02, -117.95, 6000.0);
		double step = Units::from("NM",0.4);
		for (double radius = step; radius <=  20.0*step; radius = radius + step) {
			for (double lon = -117.76; lon < -117.0; lon = lon + 0.01) {
				for (double lat = 33.3; lat < 35.5; lat = lat + 0.05) {
					if (lat > 33.8 && lat < 35) continue;     // ensure EOT is not too close to np3
					LatLonAlt p3  = LatLonAlt::make(lat, lon, 6000.0000);
					double trkIn = GreatCircle::final_course(p1,p2);
					double trkOut = GreatCircle::initial_course(p2,p3);
					double deltaTrack = Util::turnDelta(trkIn,trkOut);
					if (deltaTrack < Units::from("deg",150)) {   // 160 and above causes failures
						//Tuple6<LatLonAlt,LatLonAlt,LatLonAlt,Integer,Double,LatLonAlt> tg = TurnGeneration.turnGeneratorLLA_Alt2(p2, trkIn, trkOut, radius);
						Tuple6<LatLonAlt,LatLonAlt,LatLonAlt,int,double,LatLonAlt> tg =  TurnGeneration::turnGeneratorLLA_Alt3(p1,p2,p3,radius);
						LatLonAlt BOT = tg.getFirst();
						LatLonAlt EOT = tg.getThird();
						int dir = tg.getFourth();
						LatLonAlt center = tg.sixth;
						double trkBOTp2 = GreatCircle::initial_course(BOT,p2);
					    double distCenterToBOT = center.distanceH(BOT);
					    double distCenterToEOT = center.distanceH(EOT);
					    double distDelta = std::abs(distCenterToBOT - distCenterToEOT);
					    double radiusDelta = std::abs(distCenterToBOT - radius);
					    EXPECT_NEAR(0.0,distDelta,1.5E-8);
					    EXPECT_NEAR(0.0,radiusDelta,7E-9);
					    EXPECT_NEAR(0.0,radiusDelta/radius, 1E-9);
					    // continuity checks
					    //double trkOutBOT = Plan.trkOutInTurn(BOT, center, dir);
						double final_course = GreatCircle::final_course(center,BOT);
						double trkOutBOT = final_course + dir*M_PI/2;
					    double trkDelta2 = Util::turnDelta(trkBOTp2,trkOutBOT);
					    EXPECT_NEAR(0.0,trkDelta2,1E-10);
					}
				}
			}
		}
	}




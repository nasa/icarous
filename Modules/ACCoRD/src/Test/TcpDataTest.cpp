/*
 * TcpDataTest
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "NavPoint.h"
#include "TcpData.h"
#include <gtest/gtest.h>
#include "Plan.h"
#include "PlanUtil.h"
#include "format.h"

#include <cmath>

using namespace larcfm;

class TcpDataTest : public ::testing::Test {

public:
	TcpData d;

protected:
  virtual void SetUp() {
  }
};



	TEST_F(TcpDataTest, testBasic) {
		d.setType("Virtual");
		EXPECT_TRUE(d.isVirtual());
	}

	
	TEST_F(TcpDataTest, testBasicError) {
		EXPECT_FALSE(d.isVirtual());
		d.setType("xyz");
		EXPECT_FALSE(d.isVirtual());
	}

	
	
	TEST_F(TcpDataTest, testMergeable) {		
		Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899));
        Position p1(LatLonAlt::make(-1.015100, -1.100000, 7899));
        
		TcpData np0;//p0,97.0);
		TcpData np0alt;//p0,97.0);
		TcpData np0BOT; np0BOT.setBOT(0.0, Position::INVALID());//.makeLabel("BOT");
		TcpData np0EOT; np0EOT.setEOT();//.makeLabel("EOT");
		TcpData np0BVS; np0BVS.setBVS(0.0);//.makeLabel("BVS");
		TcpData np0EVS; np0EVS.setEVS();//.makeLabel("EVS");
		TcpData np0BGS; np0BGS.setBGS(0.0);//.makeLabel("BGS");
		TcpData np0EGS; np0EGS.setEGS();//.makeLabel("EGS");
		TcpData np0EOTBOT; np0EOTBOT.setEOTBOT(0.0, Position::INVALID());//.makeLabel("CombinedT");
		TcpData np0EVSBVS; np0EVSBVS.setEVSBVS(0.0);//.makeLabel("CombinedV");
		TcpData np0EGSBGS; np0EGSBGS.setEGSBGS(0.0);//.makeLabel("CombinedG");
		
		//
		// Check same time, same position, velocities are invalid
		//
		
		EXPECT_TRUE(np0.mergeable(np0alt));

		EXPECT_TRUE(np0EOT.mergeable(np0BVS));
		EXPECT_TRUE(np0BVS.mergeable(np0EOT));
		EXPECT_TRUE(np0EOT.mergeable(np0BOT));
		EXPECT_TRUE(np0BOT.mergeable(np0EOT));
        EXPECT_TRUE(np0EVS.mergeable(np0BOT));
        EXPECT_TRUE(np0EVS.mergeable(np0BVS));
        EXPECT_TRUE(np0EGS.mergeable(np0BVS));
        EXPECT_TRUE(np0EVS.mergeable(np0BGS));
        EXPECT_TRUE(np0EGS.mergeable(np0BOT));
        EXPECT_TRUE(np0EOT.mergeable(np0BGS));
        
        // Combined cases

        EXPECT_TRUE(np0EOTBOT.mergeable(np0BVS));
        EXPECT_TRUE(np0EOTBOT.mergeable(np0EVS));
        EXPECT_TRUE(np0EOTBOT.mergeable(np0BGS));
        EXPECT_TRUE(np0EOTBOT.mergeable(np0EGS));
        EXPECT_TRUE(np0BVS.mergeable(np0EOTBOT));
        EXPECT_TRUE(np0EVS.mergeable(np0EOTBOT));
        EXPECT_TRUE(np0BGS.mergeable(np0EOTBOT));
        EXPECT_TRUE(np0EGS.mergeable(np0EOTBOT));
        
        EXPECT_TRUE(np0EVSBVS.mergeable(np0BOT));
        EXPECT_TRUE(np0EVSBVS.mergeable(np0EOT));
        EXPECT_TRUE(np0EVSBVS.mergeable(np0BGS));
        EXPECT_TRUE(np0EVSBVS.mergeable(np0EGS));
        EXPECT_TRUE(np0BOT.mergeable(np0EVSBVS));
        EXPECT_TRUE(np0EOT.mergeable(np0EVSBVS));
        EXPECT_TRUE(np0BGS.mergeable(np0EVSBVS));
        EXPECT_TRUE(np0EGS.mergeable(np0EVSBVS));

        EXPECT_TRUE(np0EGSBGS.mergeable(np0BOT));
        EXPECT_TRUE(np0EGSBGS.mergeable(np0EOT));
        EXPECT_TRUE(np0EGSBGS.mergeable(np0BVS));
        EXPECT_TRUE(np0EGSBGS.mergeable(np0EVS));
        EXPECT_TRUE(np0BOT.mergeable(np0EGSBGS));
        EXPECT_TRUE(np0EOT.mergeable(np0EGSBGS));
        EXPECT_TRUE(np0BVS.mergeable(np0EGSBGS));
        EXPECT_TRUE(np0EVS.mergeable(np0EGSBGS));

		//
		// Check different velocities
		//

        Velocity v = Velocity::makeTrkGsVs(90,500,1000);
        Velocity v2 = Velocity::makeTrkGsVs(90,501,1000);
		TcpData np0EOTvel; np0EOTvel.setEOT(); //np0EOTvel.setVelocityInit(v);
		TcpData np0BVSvel; np0BVSvel.setBVS(0.0); //np0BVSvel.setVelocityInit(v);
		TcpData np0BVSvel2; np0BVSvel2.setBVS(0.0); //np0BVSvel2.setVelocityInit(v2);
		
		EXPECT_TRUE(np0EOT.mergeable(np0BVSvel));
		EXPECT_TRUE(np0EOTvel.mergeable(np0BVS));
		EXPECT_TRUE(np0EOTvel.mergeable(np0BVSvel));
		
		//
        // non-mergeables
		//

        EXPECT_FALSE(np0BOT.mergeable(np0BOT));
        EXPECT_FALSE(np0EOT.mergeable(np0EOT));
        EXPECT_FALSE(np0BVS.mergeable(np0BVS));
        EXPECT_FALSE(np0EVS.mergeable(np0EVS));
        EXPECT_FALSE(np0BGS.mergeable(np0BGS));
        EXPECT_FALSE(np0EGS.mergeable(np0EGS));

		EXPECT_TRUE(np0EOTvel.mergeable(np0BVSvel2));

        TcpData np0tm1; //(p0,97.0);
        TcpData np0tm2; //(p0,97.0000001);
        TcpData np1tm1; //(p1,97.0);

        TcpData np0tm1EOT; np0tm1EOT.setEOT();//.makeLabel("EOT");
        TcpData np0tm2BVS; np0tm2BVS.setBVS(0.0);//.makeLabel("BVS");
        TcpData np1tm1BVS; np1tm1BVS.setBVS(0.0);//.makeLabel("BVS");

        EXPECT_TRUE(np0tm1EOT.mergeable(np0tm2BVS));      // different time, otherwise compatible points
        EXPECT_TRUE(np0tm1EOT.mergeable(np1tm1BVS));      // same time, different position, otherwise compatible points 
        
        // Combined cases...
        EXPECT_FALSE(np0EOTBOT.mergeable(np0BOT));
        EXPECT_FALSE(np0EOTBOT.mergeable(np0EOT));
        EXPECT_FALSE(np0BOT.mergeable(np0EOTBOT));
        EXPECT_FALSE(np0EOT.mergeable(np0EOTBOT));

        EXPECT_FALSE(np0EVSBVS.mergeable(np0BVS));
        EXPECT_FALSE(np0EVSBVS.mergeable(np0EVS));
        EXPECT_FALSE(np0BVS.mergeable(np0EVSBVS));
        EXPECT_FALSE(np0EVS.mergeable(np0EVSBVS));

        EXPECT_FALSE(np0EGSBGS.mergeable(np0BGS));
        EXPECT_FALSE(np0EGSBGS.mergeable(np0EGS));
        EXPECT_FALSE(np0BGS.mergeable(np0EGSBGS));
        EXPECT_FALSE(np0EGS.mergeable(np0EGSBGS));
	}
	
	
	TEST_F(TcpDataTest, testBOTcenter) {		
		Position p1(LatLonAlt::make(8.604236, 8.277396, 6255.173510));
		Velocity vin1 = Velocity::mkTrkGsVs(1.332154,196.910830,0.436341);
		NavPoint src1 =  NavPoint::makeLatLonAlt(8.61400000, 8.31800000, 6000.00000000, 900.100000);
		double sRadius =  8479.026265; 
		//fpln(" $$$ sRadius = "+Units::str("ft",sRadius,4));
		std::pair<NavPoint,TcpData> BOT = Plan::makeBOT(p1, 876.784246, sRadius, Position::INVALID());
	}

	
	
	
	TEST_F(TcpDataTest, testMergeTCPInfo) {		
		TcpData m;
		Position p0(LatLonAlt::make(-0.354700, -1.320800, 7899));
		Position sp0(LatLonAlt::make(1.354700, 1.320800, 17899));
		Position sp1(LatLonAlt::make(1.354700, 2.320800, 17899));
		Position sp2(LatLonAlt::make(1.354700, 3.320800, 17899));
		Velocity v0 = Velocity::makeTrkGsVs(45.0, 300.0, 1500);
		Velocity v1 = Velocity::makeTrkGsVs(45.0, 400.0, 1500);
		Velocity v2 = Velocity::makeTrkGsVs(45.0, 500.0, 1500);
		double   an =  0.0;
		double   a0 = 10.0;
		double   a1 = 11.0;
		double   a2 = 12.0;
		TcpData np0; //(p0,97.0);
		TcpData np0BOT;
		double sgRadius = 200.0;
	    np0BOT.setBOT(sgRadius, Position::INVALID());

		TcpData np0EOT; np0EOT.setEOT(); np0EOT = np0EOT.setRadiusSigned(sgRadius);//.makeLabel("EOT");
		TcpData np0BVS; np0BVS.setBVS(a2);
		TcpData np0EVS; np0EVS.setEVS(); np0EVS = np0EVS.setVsAccel(a0);//.makeLabel("EVS");
		TcpData np0BGS; np0BGS.setBGS(a1);
		TcpData np0EGS; np0EGS.setEGS(); np0EGS = np0EGS.setGsAccel(a2);//.makeLabel("EGS");
		
		//
		// Check same time, same position
		//

		m = np0EOT.mergeTCPData(np0BVS);
        EXPECT_TRUE(m.isEOT());
        EXPECT_TRUE(m.isBVS());
        //EXPECT_EQ("EOTBVS", m.label());
        //EXPECT_NEAR(0.0,m.trkAccel(),0.00000001);
        EXPECT_NEAR(an,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(a2,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t2,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp2,m.getSourcePosition());
        //EXPECT_EQ(v2,m.velocityInit());
		
 		m = np0BVS.mergeTCPData(np0EOT);
        EXPECT_TRUE(m.isEOT());
        EXPECT_TRUE(m.isBVS());
        //EXPECT_EQ("BVSEOT", m.label());
        EXPECT_NEAR(sgRadius,m.getRadiusSigned(),0.000001);
        EXPECT_NEAR(an,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(a2,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t2,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp2,m.getSourcePosition());
        //EXPECT_EQ(v2,m.velocityInit());
        
		m = np0EOT.mergeTCPData(np0BOT);
        EXPECT_TRUE(m.isEOT());
        EXPECT_TRUE(m.isBOT());
        //EXPECT_EQ("EOTBOT", m.label());
        EXPECT_NEAR(sgRadius,m.getRadiusSigned(),0.00000001);
        EXPECT_NEAR(an,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(an,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t0,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp0,m.getSourcePosition());
        //EXPECT_EQ(v0,m.velocityInit());
		
		m = np0BOT.mergeTCPData(np0EOT);
        EXPECT_TRUE(m.isEOT());
        EXPECT_TRUE(m.isBOT());
        //EXPECT_EQ("BOTEOT", m.label());
        EXPECT_NEAR(sgRadius,m.getRadiusSigned(),0.00000001);
        EXPECT_NEAR(an,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(an,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t0,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp0,m.getSourcePosition());
        //EXPECT_EQ(v0,m.velocityInit());
		
        m = np0EVS.mergeTCPData(np0BOT);
        EXPECT_TRUE(m.isEVS());
        EXPECT_TRUE(m.isBOT());
        //EXPECT_EQ("EVSBOT", m.label());
        EXPECT_NEAR(sgRadius,m.getRadiusSigned(),0.00000001);
        EXPECT_NEAR(an,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(0.0,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t0,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp0,m.getSourcePosition());
        //EXPECT_EQ(v0,m.velocityInit());
		
        m = np0EVS.mergeTCPData(np0BVS);
        EXPECT_TRUE(m.isEVS());
        EXPECT_TRUE(m.isBVS());
        //EXPECT_EQ("EVSBVS", m.label());
        //EXPECT_NEAR(sgRadius,m.getRadiusSigned(),0.00000001);
        EXPECT_NEAR(an,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(a2,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t2,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp2,m.getSourcePosition());
        //EXPECT_EQ(v2,m.velocityInit());
		
        m = np0EGS.mergeTCPData(np0BVS);
        EXPECT_TRUE(m.isEGS());
        EXPECT_TRUE(m.isBVS());
        //EXPECT_EQ("EGSBVS", m.label());
        //EXPECT_NEAR(sgRadius,m.getRadiusSigned(),0.00000001);
        EXPECT_NEAR(0.0,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(a2,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t2,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp2,m.getSourcePosition());
        //EXPECT_EQ(v2,m.velocityInit());
		
        m = np0EVS.mergeTCPData(np0BGS);
        EXPECT_TRUE(m.isEVS());
        EXPECT_TRUE(m.isBGS());
        //EXPECT_EQ("EVSBGS", m.label());
        //EXPECT_NEAR(an,m.trkAccel(),0.00000001);
        EXPECT_NEAR(a1,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(0.0,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t4,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp1,m.getSourcePosition());
        //EXPECT_EQ(v1,m.velocityInit());
		
        m = np0EGS.mergeTCPData(np0BOT);
        EXPECT_TRUE(m.isEGS());
        EXPECT_TRUE(m.isBOT());
        //EXPECT_EQ("EGSBOT", m.label());
        //EXPECT_NEAR(a0,m.trkAccel(),0.00000001);
        EXPECT_NEAR(0.0,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(an,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t0,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp0,m.getSourcePosition());
        //EXPECT_EQ(v0,m.velocityInit());
		
        m = np0EOT.mergeTCPData(np0BGS);
        EXPECT_TRUE(m.isEOT());
        EXPECT_TRUE(m.isBGS());
        //EXPECT_EQ("EOTBGS", m.label());
        //EXPECT_NEAR(0.0,m.trkAccel(),0.00000001);
        EXPECT_NEAR(a1,m.getGsAccel(),0.00000001);
        EXPECT_NEAR(an,m.getVsAccel(),0.00000001);
        //EXPECT_NEAR(t4,m.getSourceTime(),0.00000001);
        //EXPECT_EQ(sp1,m.getSourcePosition());
        //EXPECT_EQ(v1,m.velocityInit());


	}
	
	
	// random from 0 to 1
	double fRand() {
		return (double)rand() / RAND_MAX;
	}

//	TEST_F(TcpDataTest, testRadiusDistance) {
//		srand(123456789);
//		//Random r = new Random();
//		for(int i = 0; i < 1000; i++) {
//			Position p(LatLonAlt::make(fRand()*180-90, fRand()*360-180, 3600));
//			NavPoint np(p, 5000);
//			double R = fRand()*20000;
//			np.makeRadius(R);
//			Velocity v = Velocity::mkVxyz(fRand()*1000-500, fRand()*1000-500, fRand()*1000-500);
//			np.makeVelocityInit(v);
//			//Position cent = np.turnCenter();
//			//double R2 = cent.distanceH(p);
//			//if (std::abs(R-R2) > 0.0000001) {
//			//	fpln("ERROR: p="+p.toString()+" v="+v.toString()+" R="+Fm4(R)+" R2="+Fm4(R2));
//			//}
//		}
//	}
//

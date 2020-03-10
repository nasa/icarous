/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


// Uses the Google unit test framework.

#include "Vect2.h"
#include "Vect3.h"
#include "Vect4.h"
#include "Units.h"
#include "Velocity.h"
#include "NavPoint.h"
#include "TrajGen.h"
#include <gtest/gtest.h>
#include "Plan.h"
#include "format.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "TrajTemplates.h"
#include "format.h"
#include <gtest/gtest.h>
#include "VectFuns.h"
#include "PlanUtil.h"
#include "PlanReader.h"
#include "TrajGen.h"
#include "PolyPath.h"
#include "PolyUtil.h"
#include "Wx.h"
#include "DebugSupport.h"
#include "PlanIO.h"
#include "PlanWriter.h"


#include <cmath>

using namespace larcfm;

class NewTest : public ::testing::Test {

public:
	double gsAccel;
	double vsAccel;
	double bankAngle;
	double gs;
	double vs;
	Plan lpc;
	LatLonAlt KMSP;
	LatLonAlt KDFW;
	LatLonAlt mid;


protected:
	virtual void SetUp() {
		lpc.clear();

		gsAccel = 2;
		vsAccel = 1;
		bankAngle = Units::from("deg", 20);
		gs = Units::from("kn",  530);
		vs = Units::from("fpm", 2000);

		KMSP = LatLonAlt::make(44.878101, -93.21869, 10000.0);
		KDFW = LatLonAlt::make(32.898700, -97.04039, 10000.0);
		mid = GreatCircle::interpolate( KMSP,KDFW,0.5);

	}
};


//TEST_F(NewTest, test_0) {  // point within turn ends up extremely close to added EOT
//}


TEST_F(NewTest, test_repair_negGs) {
	 Plan kpc("UF3");
	 Position p0  = Position::makeLatLonAlt(32.624787320000, -96.631879970000, 2026.013422060000);
	 Position p1  = Position::makeLatLonAlt(32.609350850000, -96.616491110000, 1607.989890080000);
	 Position p2  = Position::makeLatLonAlt(32.607040000000, -96.614188000000, 1372.000000000000);
	 Position p3  = Position::makeLatLonAlt(32.607040000000, -96.614188000000, 1332.791996590000);
	 Position p4  = Position::makeLatLonAlt(32.607040000000, -96.614188000000, 1322.000000000000);
	 NavPoint np0(p0,368.0039992600000000);     TcpData tcp0;
	 tcp0.setBGS(-0.980665000000);       kpc.add(np0, tcp0);
	 NavPoint np1(p1,414.3237076300000000);     TcpData tcp1;
	 int ix1 = kpc.add(np1, tcp1);
	 kpc.setInfo(ix1,"<arrFix>");
	 NavPoint np2(p2,440.4729122800000000);     TcpData tcp2;
	 tcp2.setEGS();       int ix2 = kpc.add(np2, tcp2);
	 kpc.setInfo(ix2,"<BOD>");
	 NavPoint np3(p3,445.1778726900000000);     TcpData tcp3;
	 tcp3.setBVS(0.980665000000);       kpc.add(np3, tcp3);
	 NavPoint np4(p4,447.7679518700000000);     TcpData tcp4;
	 tcp4.setEVS();       kpc.add(np4, tcp4);
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 //fpln(" $$$$ BEFORE gsIn1 = "+kpc.gsInCalc(1));
	 //fpln(" $$$$ BEFORE gsIn2 = "+kpc.gsInCalc(2));
	 EXPECT_TRUE(kpc.gsInCalc(1) >= 0);
	 EXPECT_FALSE(kpc.gsInCalc(2) >= 0);
	 //EXPECT_FALSE(kpc.isConsistent(false));
	 kpc.repairNegGsIn(2);
	 kpc.getMessage();
	 //fpln(" $$$$ AFTER gsIn1 = "+kpc.gsInCalc(1));
	 //fpln(" $$$$ AFTER gsIn2 = "+kpc.gsInCalc(2));
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 EXPECT_TRUE(kpc.gsInCalc(1) >= 0);
	 EXPECT_TRUE(kpc.gsInCalc(2) >= 0);
	 EXPECT_TRUE(kpc.isConsistent(false));

	 kpc.setTime(2,440.49);
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 EXPECT_FALSE(kpc.gsInCalc(2) >= 0);
	 kpc.repairNegGsIn(2);
	 EXPECT_TRUE(kpc.gsInCalc(2) >= 0);
	 EXPECT_TRUE(kpc.isConsistent(false));

	 kpc.setGsAccel(0,-0.982);
	 //fpln(" $$$$ kpc = "+kpc.toStringGs());
	 EXPECT_TRUE(kpc.gsInCalc(1) >= 0);
	 EXPECT_FALSE(kpc.gsInCalc(2) >= 0);
	 kpc.repairNegGsIn(2);
	 EXPECT_TRUE(kpc.gsInCalc(2) >= 0);
	 //EXPECT_TRUE(kpc.isConsistent(false));
}


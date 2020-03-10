/* Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include "Units.h"
#include "Poly2DLLCore.h"
#include "SimplePoly.h"
#include <cmath>
#include <gtest/gtest.h>
#include "format.h"
#include "Plan.h"
#include "GreatCircle.h"


using namespace larcfm;

class Poly2DLLCoreTest : public ::testing::Test {

protected:
    virtual void SetUp() {
     }	 
};
	
	TEST_F(Poly2DLLCoreTest, test0) {
		std::vector<LatLonAlt> p0; //  = new std::vector<LatLonAlt>();
		p0.push_back(LatLonAlt::make(0.0,0.0,0.0));
		p0.push_back(LatLonAlt::make(1.0,0.0,0.0));
		p0.push_back(LatLonAlt::make(1.0,1.0,0.0));
		p0.push_back(LatLonAlt::make(0.0,1.0,0.0));
		EXPECT_EQ((unsigned long)4,p0.size());
		//fpln(" ## test0: p0 = "+p0);
		LatLonAlt lla1 = LatLonAlt::make(0.5,0.5,0.0);
		double BUFF = 1E-8;
		bool isInside1 = Poly2DLLCore::spherical_inside(p0, lla1, BUFF);
		EXPECT_TRUE(isInside1);
	}



	TEST_F(Poly2DLLCoreTest, test1) {
		std::vector<LatLonAlt> p1; // = new std::vector<LatLonAlt>();
		p1.push_back(LatLonAlt::make(30.0,-101.0,5000.0));
		p1.push_back(LatLonAlt::make(30.0,-100.0,5000.0));
		p1.push_back(LatLonAlt::make(30.5,-101.0,5000.0));
		EXPECT_EQ((unsigned long)3,p1.size());
		//fpln(" ## test0: p1 = "+p1);
		LatLonAlt lla1 = LatLonAlt::make(30.1,-100.5,5000.0);
		double BUFF = 1E-8;
		bool isInside1 = Poly2DLLCore::spherical_inside(p1, lla1, BUFF);
		EXPECT_TRUE(isInside1);
		LatLonAlt lla2 = LatLonAlt::make(30.1,-100.5,5000.0);
		bool isOutside2 = Poly2DLLCore::spherical_outside(p1, lla2, BUFF);
		EXPECT_FALSE(isOutside2);

	}



	TEST_F(Poly2DLLCoreTest, test2) {
		std::vector<LatLonAlt> pList; // = new std::vector<LatLonAlt>();
		LatLonAlt pp0  = LatLonAlt::make(39.066000, -81.866000, 0.000);     pList.push_back(pp0);
		LatLonAlt pp1  = LatLonAlt::make(38.900, -81.766000, 0.000);     pList.push_back(pp1);
		LatLonAlt pp2  = LatLonAlt::make(38.700, -81.766000, 0.000);     pList.push_back(pp2);
		LatLonAlt pp3  = LatLonAlt::make(38.516000, -81.800, 0.000);     pList.push_back(pp3);
		LatLonAlt pp4  = LatLonAlt::make(38.300, -81.933000, 0.000);     pList.push_back(pp4);
		LatLonAlt pp5  = LatLonAlt::make(38.083000, -82.283000, 0.000);     pList.push_back(pp5);
		LatLonAlt pp6  = LatLonAlt::make(38.083000, -82.500, 0.000);     pList.push_back(pp6);
		LatLonAlt pp7  = LatLonAlt::make(38.116000, -82.516000, 0.000);     pList.push_back(pp7);
		LatLonAlt pp8  = LatLonAlt::make(38.333000, -82.516000, 0.000);     pList.push_back(pp8);
		LatLonAlt pp9  = LatLonAlt::make(39.050, -82.083000, 0.000);     pList.push_back(pp9);
		LatLonAlt pp10  = LatLonAlt::make(39.066000, -82.066000, 0.000);    pList.push_back(pp10);

		SimplePoly sPoly2 = SimplePoly::make(pList,0,75000);
		//DebugSupport.dumpPoly(sPoly2,"test2_poly");
		Plan lpc("Aircraft_0");
		Position p0  = Position::makeLatLonAlt(38.5087, -83.1403, 5000.00);
		Position p1  = Position::makeLatLonAlt(38.6642, -80.9077, 5000.00);
		NavPoint np0(p0,0.000);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,2000.000);     lpc.addNavPoint(np1);
		//DebugSupport.dumpPlan(lpc,"test2_lpc");

		// ----------- WxV = [685.71, 1216.84] ---------------
		LatLonAlt pos = lpc.position(600.0).lla();
		double BUFF = 1E-8;
		bool isInside1 = Poly2DLLCore::spherical_inside(pList, pos, BUFF);
		EXPECT_FALSE(isInside1);
		pos = lpc.position(685.0).lla();
		isInside1 = Poly2DLLCore::spherical_inside(pList, pos, BUFF);
		EXPECT_FALSE(isInside1);
		EXPECT_TRUE(Poly2DLLCore::spherical_inside(pList, lpc.position(1028.0).lla(), BUFF));
		EXPECT_FALSE(Poly2DLLCore::spherical_inside(pList, lpc.position(1217.0).lla(), BUFF));
		EXPECT_TRUE(Poly2DLLCore::spherical_inside(pList, lpc.position(1026.0).lla(), BUFF));
	}




	TEST_F(Poly2DLLCoreTest, test3) {
		SimplePoly sPoly0 = SimplePoly(0.0,18288.0);
		Position pp0(LatLonAlt::make(39.066000, -81.866000, 0.000));     sPoly0.add(pp0);
		Position pp1(LatLonAlt::make(38.900, -81.766000, 0.000));     sPoly0.add(pp1);
		Position pp2(LatLonAlt::make(38.700, -81.766000, 0.000));     sPoly0.add(pp2);
		Position pp3(LatLonAlt::make(38.516000, -81.800, 0.000));     sPoly0.add(pp3);
		Position pp4(LatLonAlt::make(38.300, -81.933000, 0.000));     sPoly0.add(pp4);
		Position pp5(LatLonAlt::make(38.083000, -82.283000, 0.000));     sPoly0.add(pp5);
		Position pp6(LatLonAlt::make(38.083000, -82.500, 0.000));     sPoly0.add(pp6);
		Position pp7(LatLonAlt::make(38.116000, -82.516000, 0.000));     sPoly0.add(pp7);
		Position pp8(LatLonAlt::make(38.333000, -82.516000, 0.000));     sPoly0.add(pp8);
		Position pp9(LatLonAlt::make(39.050, -82.083000, 0.000));     sPoly0.add(pp9);
		Position pp10(LatLonAlt::make(39.066000, -82.066000, 0.000));    sPoly0.add(pp10);
		//DebugSupport.dumpPoly(sPoly0,"test3_poly");
		Plan lpc("Aircraft_0");
		Position p0  = Position::makeLatLonAlt(38.50870000, -83.14030000, 5000.00000000);
		Position p1  = Position::makeLatLonAlt(38.87296095, -81.54387600, 5000.00000000);

		NavPoint np0(p0,0.000);    	 lpc.addNavPoint(np0);
		NavPoint np1(p1,2000.000);    	 lpc.addNavPoint(np1);
		//DebugSupport.dumpPlan(lpc,"test3_lpc");
		std::vector<LatLonAlt> polyList = SimplePoly::rip(sPoly0);
		EXPECT_EQ((unsigned long)sPoly0.size(),polyList.size());
		//fpln(" $$$ polyList = "+polyList);
		// WxV = [1064.75,1722.81]
		double BUFF = 1E-8;
		double step = 0.5;
		for (double t = 0.0; t < 2000.0; t = t + step) {
			Position pos = lpc.position(t);
			bool sPolyContains = sPoly0.contains(pos);
			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
			//if (sPolyContains != isInside) {
		    //	fpln(" $$$ test3: t = "+Fm1(t)+" sPolyContains = "+bool2str(sPolyContains)+" isInside = "+bool2str(isInside));
			//}
			EXPECT_EQ(sPolyContains,isInside);
		}

//		//long startTimeMs = System.currentTimeMillis();
//        for (double t = 0.0; t < 2000.0; t = t + step) {
//			Position pos = lpc.position(t);
//			bool sPolyContains = sPoly0.contains(pos);
//		}
//		//double taskTimeA = (System.currentTimeMillis() - startTimeMs) / 1000.0;
//		//fpln("## speedComparison: SimplePoly Simulation elapsed time " + taskTimeA);
//
//		//startTimeMs = System.currentTimeMillis();
//        for (double t = 0.0; t < 2000.0; t = t + step) {
//			Position pos = lpc.position(t);
//			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
//		}
//		//double taskTimeB = (System.currentTimeMillis() - startTimeMs) / 1000.0;
//		//fpln("## speedComparison: PolycarpContainmentSpherical elapsed time " + taskTimeB);
//		//fpln("## percentage diff = "+Fm1(100.0*(taskTimeA-taskTimeB)/taskTimeA)+" %");
	}


	TEST_F(Poly2DLLCoreTest, test4) {
		SimplePoly sPoly0 = SimplePoly(0.0,18288.0);
		Position p1  = Position::makeLatLonAlt(10.8, 21.0, 0.0);     sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(10.6, 21.0, 0.0);     sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(10.8, 21.2, 0.0);     sPoly0.add(p3);

		//DebugSupport.dumpPoly(sPoly0,"test4_poly");
		Plan lpc("Aircraft_0");
		Position pA  = Position::makeLatLonAlt(10.6, 21.4, 5000.0);
		Position pB  = Position::makeLatLonAlt(10.7, 20.8, 5000.0);
		NavPoint np0(pA,0.000);    	 lpc.addNavPoint(np0);
		NavPoint np1(pB,2000.000);    	 lpc.addNavPoint(np1);
		//DebugSupport.dumpPlan(lpc,"test4_lpc");
		std::vector<LatLonAlt> polyList = SimplePoly::rip(sPoly0);
		EXPECT_EQ((unsigned long)sPoly0.size(),polyList.size());
		//fpln(" $$$ polyList = "+polyList);
		// WxV = [1064.75,1722.81]
		double step = 1.0;
		Position first = Position::ZERO_LL();
		Position last = Position::ZERO_LL();
		for (double t = 0.0; t < 2000.0; t = t + step) {
			Position pos = lpc.position(t);
			bool sPolyContains = sPoly0.contains(pos);
			double BUFF = 1E-8;
			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
			if (sPolyContains != isInside) {
				//double ctd = GreatCircle::cross_track_distance(p1.lla(),p2.lla(),pos.lla());
				//fpln(" $$$ test4: t = "+t+" pos = "+pos+" sPolyContains = "+sPolyContains+" isInside = "+isInside+" ctd = "+Units::str("m",ctd));
                if (first == Position::ZERO_LL()) first = pos;
                last = pos;
			}
			EXPECT_EQ(sPolyContains,isInside);
		}
        //double distFirstLast = last.distanceH(first);
        //fpln(" $$$$ distFirstLast = "+distFirstLast);
		//long startTimeMs = System.currentTimeMillis();
		for (double t = 0.0; t < 2000.0; t = t + step) {
			Position pos = lpc.position(t);
			//bool sPolyContains =
					sPoly0.contains(pos);
		}
		//double taskTimeA = (System.currentTimeMillis() - startTimeMs) / 1000.0;
		//fpln("## test4: SimplePoly Simulation elapsed time " + taskTimeA);

		//startTimeMs = System.currentTimeMillis();
		double BUFF = 1E-8;
		for (double t = 0.0; t < 2000.0; t = t + step) {
			Position pos = lpc.position(t);
			//bool isInside =
					Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
		}
		//double taskTimeB = (System.currentTimeMillis() - startTimeMs) / 1000.0;
		//fpln("## test4: PolycarpContainmentSpherical elapsed time " + taskTimeB);
		//fpln("## percentage diff = "+Fm1(100.0*(taskTimeA-taskTimeB)/taskTimeA)+" %");
	}


	TEST_F(Poly2DLLCoreTest, test4b) {
		SimplePoly sPoly0 = SimplePoly(0.0,18288.0);
		Position p1  = Position::makeLatLonAlt(10.8, 21.0, 0.0);     sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(10.6, 21.0, 0.0);     sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(10.8, 21.2, 0.0);     sPoly0.add(p3);
		//DebugSupport.dumpPoly(sPoly0,"test4_poly");
		std::vector<LatLonAlt> polyList = SimplePoly::rip(sPoly0);
		EXPECT_EQ((unsigned long)sPoly0.size(),polyList.size());
		//fpln(" $$$ polyList = "+polyList);
		// WxV = [1064.75,1722.81]
		//double step = 1.0;
		Position first = Position::ZERO_LL();
		Position last = Position::ZERO_LL();
		Position pos = Position::makeLatLonAlt(10.6673, 20.9965, 0.0);
		//fpln("p1 lon lat is "+p1.lon()+" and "+p1.lat());
		//fpln("p2 lon lat is "+p2.lon()+" and "+p2.lat());
		//fpln("p3 lon lat is "+p3.lon()+" and "+p3.lat());
		//fpln("pos lon lat is "+pos.lon()+" and "+pos.lat());
		bool sPolyContains = sPoly0.contains(pos);
		double BUFF = 0.000000015695154889990305;
		bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
		//double ctd =
				GreatCircle::cross_track_distance(p1.lla(),p2.lla(),pos.lla());
		//fpln(" $$$ test4b: pos = "+pos+" sPolyContains = "+sPolyContains+" isInside = "+isInside+" ctd = "+Units::str("m",ctd));
		EXPECT_EQ(sPolyContains,isInside);
 	}



	TEST_F(Poly2DLLCoreTest, test5) {
		std::vector<LatLonAlt> poly; // = new std::vector<LatLonAlt>();
		LatLonAlt pcc  = LatLonAlt::make(42.9476,  -99.6765, 10.0); poly.push_back(pcc);
		LatLonAlt pbb  = LatLonAlt::make(42.8751, -100.3355, 10.0); poly.push_back(pbb);
		LatLonAlt paa  = LatLonAlt::make(42.5569, -100.6152, 10.0); poly.push_back(paa);
		LatLonAlt p6  = LatLonAlt::make(42.3633, -100.5158, 10.0); poly.push_back(p6);
		LatLonAlt p8  = LatLonAlt::make(42.5504, -100.1933, 10.0); poly.push_back(p8);
		LatLonAlt p10 = LatLonAlt::make(42.5234, -100.1182, 10.0); poly.push_back(p10);
		LatLonAlt p12 = LatLonAlt::make(42.3858,  -99.8051, 10.0); poly.push_back(p12);
		LatLonAlt p14 = LatLonAlt::make(42.5861,  -99.5293, 10.0); poly.push_back(p14);

		double BUFF = 1E-8;
		// test vertexes
		for (int j = 0; j < (int) poly.size(); j++) {
			LatLonAlt pos = poly[j];
			bool cont = Poly2DLLCore::spherical_inside(poly, pos, BUFF);
			//fpln(" $$$ j = "+j+" pos = "+pos+" cont = "+cont);
			EXPECT_EQ(j==1 || j == 2 || j == 4 || j == 5,cont);
		}
		double bottom = 4000;
		double top =  20;
		SimplePoly sp0 = SimplePoly::make(poly,bottom,top);
		//fpln(" $$$$ sp0 = "+sp0);

		Plan lpc("Aircraft_0");
		double offset = 1E-12;
		Position pA  = Position::makeLatLonAlt(42.5569, -100.6152+offset, 5000.00);
		Position pB  = Position::makeLatLonAlt(42.8751, -100.3355+offset, 5000.00);
		Position pC  = Position::makeLatLonAlt(42.9476-offset, -99.6765, 5000.00);

		NavPoint npA(pA,0.0);      lpc.addNavPoint(npA);
		NavPoint npB(pB,1000);     lpc.addNavPoint(npB);
		NavPoint npC(pC,2000);     lpc.addNavPoint(npC);
		//DebugSupport.dumpPlanAndPoly(lpc,sp0,"test4");
		for (double t = 0.0; t < 2000.0; t++) {
			LatLonAlt pos = lpc.position(t).lla();
			bool cont = Poly2DLLCore::spherical_inside(poly, pos, BUFF);
			//bool cont = sp0.contains2D(Position(pos));
			//if (!cont) fpln(" $$$ test5: t = "+t+" pos = "+pos+" cont = "+cont);
			EXPECT_TRUE(cont);
		}

	}


	void speedComparison() {
		fpln(" -------------------------- speedComparison -------------------------------");
		SimplePoly sPoly0 = SimplePoly(0.0,15163.0);
		Position p0  = Position::makeLatLonAlt(42.03170, -96.41580, 0.00);     sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(42.02270, -96.42830, 0.00);     sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(42.02270, -96.44080, 0.00);     sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(42.01370, -96.46580, 0.00);     sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(41.99570, -96.47830, 0.00);     sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(41.98670, -96.49080, 0.00);     sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(41.97770, -96.50340, 0.00);     sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(41.96870, -96.50340, 0.00);     sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(41.95970, -96.50340, 0.00);     sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(41.94170, -96.49080, 0.00);     sPoly0.add(p9);
		Position p10  = Position::makeLatLonAlt(41.93270, -96.47830, 0.00);     sPoly0.add(p10);
		Position p11  = Position::makeLatLonAlt(41.92370, -96.46580, 0.00);     sPoly0.add(p11);
		Position p12  = Position::makeLatLonAlt(41.91470, -96.45330, 0.00);     sPoly0.add(p12);
		Position p13  = Position::makeLatLonAlt(41.89670, -96.44080, 0.00);     sPoly0.add(p13);
		Position p14  = Position::makeLatLonAlt(41.88770, -96.44080, 0.00);     sPoly0.add(p14);
		Position p15  = Position::makeLatLonAlt(41.87870, -96.44080, 0.00);     sPoly0.add(p15);
		Position p16  = Position::makeLatLonAlt(41.86970, -96.44080, 0.00);     sPoly0.add(p16);
		Position p17  = Position::makeLatLonAlt(41.86970, -96.45330, 0.00);     sPoly0.add(p17);
		Position p18  = Position::makeLatLonAlt(41.86070, -96.46580, 0.00);     sPoly0.add(p18);
		Position p19  = Position::makeLatLonAlt(41.86070, -96.47830, 0.00);     sPoly0.add(p19);
		Position p20  = Position::makeLatLonAlt(41.85170, -96.49080, 0.00);     sPoly0.add(p20);
		Position p21  = Position::makeLatLonAlt(41.85170, -96.50340, 0.00);     sPoly0.add(p21);
		Position p22  = Position::makeLatLonAlt(41.84270, -96.51590, 0.00);     sPoly0.add(p22);
		Position p23  = Position::makeLatLonAlt(41.83370, -96.51590, 0.00);     sPoly0.add(p23);
		Position p24  = Position::makeLatLonAlt(41.82470, -96.51590, 0.00);     sPoly0.add(p24);
		Position p25  = Position::makeLatLonAlt(41.80670, -96.50340, 0.00);     sPoly0.add(p25);
		Position p26  = Position::makeLatLonAlt(41.79770, -96.47830, 0.00);     sPoly0.add(p26);
		Position p27  = Position::makeLatLonAlt(41.78870, -96.46580, 0.00);     sPoly0.add(p27);
		Position p28  = Position::makeLatLonAlt(41.77970, -96.44080, 0.00);     sPoly0.add(p28);
		Position p29  = Position::makeLatLonAlt(41.77970, -96.42830, 0.00);     sPoly0.add(p29);
		Position p30  = Position::makeLatLonAlt(41.69870, -96.45330, 0.00);     sPoly0.add(p30);
		Position p31  = Position::makeLatLonAlt(41.69870, -96.42830, 0.00);     sPoly0.add(p31);
		Position p32  = Position::makeLatLonAlt(41.69870, -96.41580, 0.00);     sPoly0.add(p32);
		Position p33  = Position::makeLatLonAlt(41.73450, -96.39030, 0.00);     sPoly0.add(p33);
		Position p34  = Position::makeLatLonAlt(41.68970, -96.41580, 0.00);     sPoly0.add(p34);
		Position p35  = Position::makeLatLonAlt(41.63570, -96.41580, 0.00);     sPoly0.add(p35);
		Position p36  = Position::makeLatLonAlt(41.62670, -96.41580, 0.00);     sPoly0.add(p36);
		Position p37  = Position::makeLatLonAlt(41.61770, -96.42830, 0.00);     sPoly0.add(p37);
		Position p38  = Position::makeLatLonAlt(41.60870, -96.44080, 0.00);     sPoly0.add(p38);
		Position p39  = Position::makeLatLonAlt(41.59970, -96.44080, 0.00);     sPoly0.add(p39);
		Position p40  = Position::makeLatLonAlt(41.59970, -96.45330, 0.00);     sPoly0.add(p40);
		Position p41  = Position::makeLatLonAlt(41.59970, -96.46580, 0.00);     sPoly0.add(p41);
		Position p42  = Position::makeLatLonAlt(41.59970, -96.47830, 0.00);     sPoly0.add(p42);
		Position p43  = Position::makeLatLonAlt(41.59970, -96.49080, 0.00);     sPoly0.add(p43);
		Position p44  = Position::makeLatLonAlt(41.60870, -96.50340, 0.00);     sPoly0.add(p44);
		Position p45  = Position::makeLatLonAlt(41.60870, -96.51590, 0.00);     sPoly0.add(p45);
		Position p46  = Position::makeLatLonAlt(41.60870, -96.52840, 0.00);     sPoly0.add(p46);
		Position p47  = Position::makeLatLonAlt(41.59970, -96.55340, 0.00);     sPoly0.add(p47);
		Position p48  = Position::makeLatLonAlt(41.59970, -96.56590, 0.00);     sPoly0.add(p48);
		Position p49  = Position::makeLatLonAlt(41.59070, -96.57840, 0.00);     sPoly0.add(p49);
		Position p50  = Position::makeLatLonAlt(41.57270, -96.59100, 0.00);     sPoly0.add(p50);
		Position p51  = Position::makeLatLonAlt(41.56370, -96.60350, 0.00);     sPoly0.add(p51);
		Position p52  = Position::makeLatLonAlt(41.55470, -96.60350, 0.00);     sPoly0.add(p52);
		Position p53  = Position::makeLatLonAlt(41.54570, -96.61600, 0.00);     sPoly0.add(p53);
		Position p54  = Position::makeLatLonAlt(41.53670, -96.62850, 0.00);     sPoly0.add(p54);
		Position p55  = Position::makeLatLonAlt(41.52770, -96.62850, 0.00);     sPoly0.add(p55);
		Position p56  = Position::makeLatLonAlt(41.52770, -96.64100, 0.00);     sPoly0.add(p56);
		Position p57  = Position::makeLatLonAlt(41.52770, -96.65350, 0.00);     sPoly0.add(p57);
		Position p58  = Position::makeLatLonAlt(41.53670, -96.66600, 0.00);     sPoly0.add(p58);
		Position p59  = Position::makeLatLonAlt(41.54570, -96.66600, 0.00);     sPoly0.add(p59);
		Position p60  = Position::makeLatLonAlt(41.55470, -96.67860, 0.00);     sPoly0.add(p60);
		Position p61  = Position::makeLatLonAlt(41.56370, -96.69110, 0.00);     sPoly0.add(p61);
		Position p62  = Position::makeLatLonAlt(41.57270, -96.69110, 0.00);     sPoly0.add(p62);
		Position p63  = Position::makeLatLonAlt(41.58170, -96.70360, 0.00);     sPoly0.add(p63);
		Position p64  = Position::makeLatLonAlt(41.59070, -96.71610, 0.00);     sPoly0.add(p64);
		Position p65  = Position::makeLatLonAlt(41.62650, -96.69070, 0.00);     sPoly0.add(p65);
		Position p66  = Position::makeLatLonAlt(41.59070, -96.72860, 0.00);     sPoly0.add(p66);
		Position p67  = Position::makeLatLonAlt(41.58170, -96.74110, 0.00);     sPoly0.add(p67);
		Position p68  = Position::makeLatLonAlt(41.58170, -96.75360, 0.00);     sPoly0.add(p68);
		Position p69  = Position::makeLatLonAlt(41.56370, -96.75360, 0.00);     sPoly0.add(p69);
		Position p70  = Position::makeLatLonAlt(41.55470, -96.76620, 0.00);     sPoly0.add(p70);
		Position p71  = Position::makeLatLonAlt(41.54570, -96.76620, 0.00);     sPoly0.add(p71);
		Position p72  = Position::makeLatLonAlt(41.53670, -96.75360, 0.00);     sPoly0.add(p72);
		Position p73  = Position::makeLatLonAlt(41.52770, -96.75360, 0.00);     sPoly0.add(p73);
		Position p74  = Position::makeLatLonAlt(41.51870, -96.74110, 0.00);     sPoly0.add(p74);
		Position p75  = Position::makeLatLonAlt(41.51870, -96.72860, 0.00);     sPoly0.add(p75);
		Position p76  = Position::makeLatLonAlt(41.50970, -96.71610, 0.00);     sPoly0.add(p76);
		Position p77  = Position::makeLatLonAlt(41.50070, -96.71610, 0.00);     sPoly0.add(p77);
		Position p78  = Position::makeLatLonAlt(41.48270, -96.70360, 0.00);     sPoly0.add(p78);
		Position p79  = Position::makeLatLonAlt(41.47370, -96.69110, 0.00);     sPoly0.add(p79);
		Position p80  = Position::makeLatLonAlt(41.46470, -96.67860, 0.00);     sPoly0.add(p80);
		Position p81  = Position::makeLatLonAlt(41.46470, -96.66600, 0.00);     sPoly0.add(p81);
		Position p82  = Position::makeLatLonAlt(41.45570, -96.66600, 0.00);     sPoly0.add(p82);
		Position p83  = Position::makeLatLonAlt(41.44670, -96.65350, 0.00);     sPoly0.add(p83);
		Position p84  = Position::makeLatLonAlt(41.43770, -96.65350, 0.00);     sPoly0.add(p84);
		Position p85  = Position::makeLatLonAlt(41.47350, -96.62810, 0.00);     sPoly0.add(p85);
		Position p86  = Position::makeLatLonAlt(41.42870, -96.65350, 0.00);     sPoly0.add(p86);
		Position p87  = Position::makeLatLonAlt(41.41970, -96.66600, 0.00);     sPoly0.add(p87);
		Position p88  = Position::makeLatLonAlt(41.41070, -96.67860, 0.00);     sPoly0.add(p88);
		Position p89  = Position::makeLatLonAlt(41.40170, -96.69110, 0.00);     sPoly0.add(p89);
		Position p90  = Position::makeLatLonAlt(41.39270, -96.69110, 0.00);     sPoly0.add(p90);
		Position p91  = Position::makeLatLonAlt(41.38370, -96.69110, 0.00);     sPoly0.add(p91);
		Position p92  = Position::makeLatLonAlt(41.37470, -96.69110, 0.00);     sPoly0.add(p92);
		Position p93  = Position::makeLatLonAlt(41.41050, -96.66560, 0.00);     sPoly0.add(p93);
		Position p94  = Position::makeLatLonAlt(41.36570, -96.67860, 0.00);     sPoly0.add(p94);
		Position p95  = Position::makeLatLonAlt(41.35670, -96.66600, 0.00);     sPoly0.add(p95);
		Position p96  = Position::makeLatLonAlt(41.33870, -96.65350, 0.00);     sPoly0.add(p96);
		Position p97  = Position::makeLatLonAlt(41.32970, -96.64100, 0.00);     sPoly0.add(p97);
		Position p98  = Position::makeLatLonAlt(41.32070, -96.64100, 0.00);     sPoly0.add(p98);
		Position p99  = Position::makeLatLonAlt(41.31170, -96.64100, 0.00);     sPoly0.add(p99);
		Position p100  = Position::makeLatLonAlt(41.30270, -96.65350, 0.00);     sPoly0.add(p100);
		Position p101  = Position::makeLatLonAlt(41.29370, -96.66600, 0.00);     sPoly0.add(p101);
		Position p102  = Position::makeLatLonAlt(41.28470, -96.69110, 0.00);     sPoly0.add(p102);
		Position p103  = Position::makeLatLonAlt(41.28470, -96.70360, 0.00);     sPoly0.add(p103);
		Position p104  = Position::makeLatLonAlt(41.28470, -96.71610, 0.00);     sPoly0.add(p104);
		Position p105  = Position::makeLatLonAlt(41.28470, -96.74110, 0.00);     sPoly0.add(p105);
		Position p106  = Position::makeLatLonAlt(41.28470, -96.75360, 0.00);     sPoly0.add(p106);
		Position p107  = Position::makeLatLonAlt(41.28470, -96.76620, 0.00);     sPoly0.add(p107);
		Position p108  = Position::makeLatLonAlt(41.27570, -96.77870, 0.00);     sPoly0.add(p108);
		Position p109  = Position::makeLatLonAlt(41.27570, -96.79120, 0.00);     sPoly0.add(p109);
		Position p110  = Position::makeLatLonAlt(41.26670, -96.80370, 0.00);     sPoly0.add(p110);
		Position p111  = Position::makeLatLonAlt(41.25770, -96.81620, 0.00);     sPoly0.add(p111);
		Position p112  = Position::makeLatLonAlt(41.25770, -96.84120, 0.00);     sPoly0.add(p112);
		Position p113  = Position::makeLatLonAlt(41.24870, -96.85370, 0.00);     sPoly0.add(p113);
		Position p114  = Position::makeLatLonAlt(41.23970, -96.87880, 0.00);     sPoly0.add(p114);
		Position p115  = Position::makeLatLonAlt(41.23970, -96.89130, 0.00);     sPoly0.add(p115);
		Position p116  = Position::makeLatLonAlt(41.23970, -96.90380, 0.00);     sPoly0.add(p116);
		Position p117  = Position::makeLatLonAlt(41.23070, -96.91630, 0.00);     sPoly0.add(p117);
		Position p118  = Position::makeLatLonAlt(41.23070, -96.92880, 0.00);     sPoly0.add(p118);
		Position p119  = Position::makeLatLonAlt(41.22170, -96.94130, 0.00);     sPoly0.add(p119);
		Position p120  = Position::makeLatLonAlt(41.21270, -96.94130, 0.00);     sPoly0.add(p120);
		Position p121  = Position::makeLatLonAlt(41.20370, -96.95390, 0.00);     sPoly0.add(p121);
		Position p122  = Position::makeLatLonAlt(41.19470, -96.96640, 0.00);     sPoly0.add(p122);
		Position p123  = Position::makeLatLonAlt(41.17670, -96.96640, 0.00);     sPoly0.add(p123);
		Position p124  = Position::makeLatLonAlt(41.16770, -96.97890, 0.00);     sPoly0.add(p124);
		Position p125  = Position::makeLatLonAlt(41.15870, -96.97890, 0.00);     sPoly0.add(p125);
		Position p126  = Position::makeLatLonAlt(41.14970, -96.97890, 0.00);     sPoly0.add(p126);
		Position p127  = Position::makeLatLonAlt(41.14070, -96.99140, 0.00);     sPoly0.add(p127);
		Position p128  = Position::makeLatLonAlt(41.13170, -96.99140, 0.00);     sPoly0.add(p128);
		Position p129  = Position::makeLatLonAlt(41.11370, -96.97890, 0.00);     sPoly0.add(p129);
		Position p130  = Position::makeLatLonAlt(41.10470, -96.97890, 0.00);     sPoly0.add(p130);
		Position p131  = Position::makeLatLonAlt(41.08670, -96.97890, 0.00);     sPoly0.add(p131);
		Position p132  = Position::makeLatLonAlt(41.07770, -96.96640, 0.00);     sPoly0.add(p132);
		Position p133  = Position::makeLatLonAlt(41.06870, -96.95390, 0.00);     sPoly0.add(p133);
		Position p134  = Position::makeLatLonAlt(41.05970, -96.94130, 0.00);     sPoly0.add(p134);
		Position p135  = Position::makeLatLonAlt(41.05970, -96.92880, 0.00);     sPoly0.add(p135);
		Position p136  = Position::makeLatLonAlt(41.06870, -96.90380, 0.00);     sPoly0.add(p136);
		Position p137  = Position::makeLatLonAlt(41.06870, -96.89130, 0.00);     sPoly0.add(p137);
		Position p138  = Position::makeLatLonAlt(41.06870, -96.87880, 0.00);     sPoly0.add(p138);
		Position p139  = Position::makeLatLonAlt(41.06870, -96.86630, 0.00);     sPoly0.add(p139);
		Position p140  = Position::makeLatLonAlt(41.06870, -96.85370, 0.00);     sPoly0.add(p140);
		Position p141  = Position::makeLatLonAlt(41.05970, -96.85370, 0.00);     sPoly0.add(p141);
		Position p142  = Position::makeLatLonAlt(41.04170, -96.85370, 0.00);     sPoly0.add(p142);
		Position p143  = Position::makeLatLonAlt(41.03270, -96.85370, 0.00);     sPoly0.add(p143);
		Position p144  = Position::makeLatLonAlt(41.01470, -96.84120, 0.00);     sPoly0.add(p144);
		Position p145  = Position::makeLatLonAlt(41.00570, -96.84120, 0.00);     sPoly0.add(p145);
		Position p146  = Position::makeLatLonAlt(40.99670, -96.84120, 0.00);     sPoly0.add(p146);
		Position p147  = Position::makeLatLonAlt(40.99670, -96.82870, 0.00);     sPoly0.add(p147);
		Position p148  = Position::makeLatLonAlt(40.99670, -96.81620, 0.00);     sPoly0.add(p148);
		Position p149  = Position::makeLatLonAlt(40.99670, -96.80370, 0.00);     sPoly0.add(p149);
		Position p150  = Position::makeLatLonAlt(40.99670, -96.77870, 0.00);     sPoly0.add(p150);
		Position p151  = Position::makeLatLonAlt(41.03250, -96.75320, 0.00);     sPoly0.add(p151);
		Position p152  = Position::makeLatLonAlt(40.98770, -96.76620, 0.00);     sPoly0.add(p152);
		Position p153  = Position::makeLatLonAlt(40.97870, -96.76620, 0.00);     sPoly0.add(p153);
		Position p154  = Position::makeLatLonAlt(40.96970, -96.76620, 0.00);     sPoly0.add(p154);
		Position p155  = Position::makeLatLonAlt(40.95170, -96.76620, 0.00);     sPoly0.add(p155);
		Position p156  = Position::makeLatLonAlt(40.94270, -96.76620, 0.00);     sPoly0.add(p156);
		Position p157  = Position::makeLatLonAlt(40.92470, -96.77870, 0.00);     sPoly0.add(p157);
		Position p158  = Position::makeLatLonAlt(40.91570, -96.76620, 0.00);     sPoly0.add(p158);
		Position p159  = Position::makeLatLonAlt(40.89770, -96.76620, 0.00);     sPoly0.add(p159);
		Position p160  = Position::makeLatLonAlt(40.88870, -96.75360, 0.00);     sPoly0.add(p160);
		Position p161  = Position::makeLatLonAlt(40.87970, -96.75360, 0.00);     sPoly0.add(p161);
		Position p162  = Position::makeLatLonAlt(40.87070, -96.74110, 0.00);     sPoly0.add(p162);
		Position p163  = Position::makeLatLonAlt(40.86170, -96.72860, 0.00);     sPoly0.add(p163);
		Position p164  = Position::makeLatLonAlt(40.85270, -96.71610, 0.00);     sPoly0.add(p164);
		Position p165  = Position::makeLatLonAlt(40.83470, -96.70360, 0.00);     sPoly0.add(p165);
		Position p166  = Position::makeLatLonAlt(40.83470, -96.67860, 0.00);     sPoly0.add(p166);
		Position p167  = Position::makeLatLonAlt(40.82570, -96.66600, 0.00);     sPoly0.add(p167);
		Position p168  = Position::makeLatLonAlt(40.81670, -96.65350, 0.00);     sPoly0.add(p168);
		Position p169  = Position::makeLatLonAlt(40.81670, -96.64100, 0.00);     sPoly0.add(p169);
		Position p170  = Position::makeLatLonAlt(40.81670, -96.61600, 0.00);     sPoly0.add(p170);
		Position p171  = Position::makeLatLonAlt(40.82570, -96.59100, 0.00);     sPoly0.add(p171);
		Position p172  = Position::makeLatLonAlt(40.82570, -96.57840, 0.00);     sPoly0.add(p172);
		Position p173  = Position::makeLatLonAlt(40.83470, -96.56590, 0.00);     sPoly0.add(p173);
		Position p174  = Position::makeLatLonAlt(40.84370, -96.54090, 0.00);     sPoly0.add(p174);
		Position p175  = Position::makeLatLonAlt(40.85270, -96.54090, 0.00);     sPoly0.add(p175);
		Position p176  = Position::makeLatLonAlt(40.86170, -96.52840, 0.00);     sPoly0.add(p176);
		Position p177  = Position::makeLatLonAlt(40.87970, -96.51590, 0.00);     sPoly0.add(p177);
		Position p178  = Position::makeLatLonAlt(40.88870, -96.51590, 0.00);     sPoly0.add(p178);
		Position p179  = Position::makeLatLonAlt(40.89770, -96.50340, 0.00);     sPoly0.add(p179);
		Position p180  = Position::makeLatLonAlt(40.90670, -96.49080, 0.00);     sPoly0.add(p180);
		Position p181  = Position::makeLatLonAlt(40.91570, -96.47830, 0.00);     sPoly0.add(p181);
		Position p182  = Position::makeLatLonAlt(40.95150, -96.45290, 0.00);     sPoly0.add(p182);
		Position p183  = Position::makeLatLonAlt(40.91570, -96.46580, 0.00);     sPoly0.add(p183);
		Position p184  = Position::makeLatLonAlt(40.91570, -96.45330, 0.00);     sPoly0.add(p184);
		Position p185  = Position::makeLatLonAlt(40.90670, -96.44080, 0.00);     sPoly0.add(p185);
		Position p186  = Position::makeLatLonAlt(41.03270, -96.21550, 0.00);     sPoly0.add(p186);
		Position p187  = Position::makeLatLonAlt(41.04170, -96.20300, 0.00);     sPoly0.add(p187);
		Position p188  = Position::makeLatLonAlt(41.05070, -96.20300, 0.00);     sPoly0.add(p188);
		Position p189  = Position::makeLatLonAlt(41.05970, -96.20300, 0.00);     sPoly0.add(p189);
		Position p190  = Position::makeLatLonAlt(41.07770, -96.20300, 0.00);     sPoly0.add(p190);
		Position p191  = Position::makeLatLonAlt(41.08670, -96.20300, 0.00);     sPoly0.add(p191);
		Position p192  = Position::makeLatLonAlt(41.09570, -96.20300, 0.00);     sPoly0.add(p192);
		Position p193  = Position::makeLatLonAlt(41.10470, -96.19050, 0.00);     sPoly0.add(p193);
		Position p194  = Position::makeLatLonAlt(41.11370, -96.19050, 0.00);     sPoly0.add(p194);
		Position p195  = Position::makeLatLonAlt(41.12270, -96.17800, 0.00);     sPoly0.add(p195);
		Position p196  = Position::makeLatLonAlt(41.12270, -96.16550, 0.00);     sPoly0.add(p196);
		Position p197  = Position::makeLatLonAlt(41.12270, -96.15300, 0.00);     sPoly0.add(p197);
		Position p198  = Position::makeLatLonAlt(41.13170, -96.14050, 0.00);     sPoly0.add(p198);
		Position p199  = Position::makeLatLonAlt(41.13170, -96.12790, 0.00);     sPoly0.add(p199);
		Position p200  = Position::makeLatLonAlt(41.13170, -96.11540, 0.00);     sPoly0.add(p200);
		Position p201  = Position::makeLatLonAlt(41.13170, -96.09040, 0.00);     sPoly0.add(p201);
		Position p202  = Position::makeLatLonAlt(41.13170, -96.07790, 0.00);     sPoly0.add(p202);
		Position p203  = Position::makeLatLonAlt(41.13170, -96.06540, 0.00);     sPoly0.add(p203);
		Position p204  = Position::makeLatLonAlt(41.13170, -96.04030, 0.00);     sPoly0.add(p204);
		Position p205  = Position::makeLatLonAlt(41.13170, -96.02780, 0.00);     sPoly0.add(p205);
		Position p206  = Position::makeLatLonAlt(41.16750, -96.00240, 0.00);     sPoly0.add(p206);
		Position p207  = Position::makeLatLonAlt(41.13170, -96.01530, 0.00);     sPoly0.add(p207);
		Position p208  = Position::makeLatLonAlt(41.16750, -95.98990, 0.00);     sPoly0.add(p208);
		Position p209  = Position::makeLatLonAlt(41.33970, -95.92600, 0.00);     sPoly0.add(p209);
		Position p210  = Position::makeLatLonAlt(41.14070, -96.09040, 0.00);     sPoly0.add(p210);
		Position p211  = Position::makeLatLonAlt(41.14070, -96.10290, 0.00);     sPoly0.add(p211);
		Position p212  = Position::makeLatLonAlt(41.14070, -96.11540, 0.00);     sPoly0.add(p212);
		Position p213  = Position::makeLatLonAlt(41.14970, -96.12790, 0.00);     sPoly0.add(p213);
		Position p214  = Position::makeLatLonAlt(41.14970, -96.11540, 0.00);     sPoly0.add(p214);
		Position p215  = Position::makeLatLonAlt(41.15870, -96.11540, 0.00);     sPoly0.add(p215);
		Position p216  = Position::makeLatLonAlt(41.16770, -96.09040, 0.00);     sPoly0.add(p216);
		Position p217  = Position::makeLatLonAlt(41.30270, -96.15300, 0.00);     sPoly0.add(p217);
		Position p218  = Position::makeLatLonAlt(41.31170, -96.15300, 0.00);     sPoly0.add(p218);
		Position p219  = Position::makeLatLonAlt(41.32970, -96.15300, 0.00);     sPoly0.add(p219);
		Position p220  = Position::makeLatLonAlt(41.33870, -96.14050, 0.00);     sPoly0.add(p220);
		Position p221  = Position::makeLatLonAlt(41.35670, -96.14050, 0.00);     sPoly0.add(p221);
		Position p222  = Position::makeLatLonAlt(41.36570, -96.14050, 0.00);     sPoly0.add(p222);
		Position p223  = Position::makeLatLonAlt(41.38370, -96.14050, 0.00);     sPoly0.add(p223);
		Position p224  = Position::makeLatLonAlt(41.39270, -96.15300, 0.00);     sPoly0.add(p224);
		Position p225  = Position::makeLatLonAlt(41.40170, -96.15300, 0.00);     sPoly0.add(p225);
		Position p226  = Position::makeLatLonAlt(41.41070, -96.15300, 0.00);     sPoly0.add(p226);
		Position p227  = Position::makeLatLonAlt(41.41970, -96.16550, 0.00);     sPoly0.add(p227);
		Position p228  = Position::makeLatLonAlt(41.43770, -96.16550, 0.00);     sPoly0.add(p228);
		Position p229  = Position::makeLatLonAlt(41.44670, -96.15300, 0.00);     sPoly0.add(p229);
		Position p230  = Position::makeLatLonAlt(41.44670, -96.14050, 0.00);     sPoly0.add(p230);
		Position p231  = Position::makeLatLonAlt(41.45570, -96.14050, 0.00);     sPoly0.add(p231);
		Position p232  = Position::makeLatLonAlt(41.45570, -96.11540, 0.00);     sPoly0.add(p232);
		Position p233  = Position::makeLatLonAlt(41.45570, -96.10290, 0.00);     sPoly0.add(p233);
		Position p234  = Position::makeLatLonAlt(41.45570, -96.09040, 0.00);     sPoly0.add(p234);
		Position p235  = Position::makeLatLonAlt(41.45570, -96.07790, 0.00);     sPoly0.add(p235);
		Position p236  = Position::makeLatLonAlt(41.45570, -96.06540, 0.00);     sPoly0.add(p236);
		Position p237  = Position::makeLatLonAlt(41.45570, -96.05290, 0.00);     sPoly0.add(p237);
		Position p238  = Position::makeLatLonAlt(41.45570, -96.04030, 0.00);     sPoly0.add(p238);
		Position p239  = Position::makeLatLonAlt(41.46470, -96.02780, 0.00);     sPoly0.add(p239);
		Position p240  = Position::makeLatLonAlt(41.46470, -96.01530, 0.00);     sPoly0.add(p240);
		Position p241  = Position::makeLatLonAlt(41.47370, -95.99030, 0.00);     sPoly0.add(p241);
		Position p242  = Position::makeLatLonAlt(41.47370, -95.97780, 0.00);     sPoly0.add(p242);
		Position p243  = Position::makeLatLonAlt(41.47370, -95.96530, 0.00);     sPoly0.add(p243);
		Position p244  = Position::makeLatLonAlt(41.46470, -95.95270, 0.00);     sPoly0.add(p244);
		Position p245  = Position::makeLatLonAlt(41.45570, -95.94020, 0.00);     sPoly0.add(p245);
		Position p246  = Position::makeLatLonAlt(41.44670, -95.92770, 0.00);     sPoly0.add(p246);
		Position p247  = Position::makeLatLonAlt(41.44670, -95.91520, 0.00);     sPoly0.add(p247);
		Position p248  = Position::makeLatLonAlt(41.43770, -95.90270, 0.00);     sPoly0.add(p248);
		Position p249  = Position::makeLatLonAlt(41.47350, -95.87730, 0.00);     sPoly0.add(p249);
		Position p250  = Position::makeLatLonAlt(41.43770, -95.89020, 0.00);     sPoly0.add(p250);
		Position p251  = Position::makeLatLonAlt(41.44670, -95.87770, 0.00);     sPoly0.add(p251);
		Position p252  = Position::makeLatLonAlt(41.45570, -95.85260, 0.00);     sPoly0.add(p252);
		Position p253  = Position::makeLatLonAlt(41.45570, -95.84010, 0.00);     sPoly0.add(p253);
		Position p254  = Position::makeLatLonAlt(41.46470, -95.81510, 0.00);     sPoly0.add(p254);
		Position p255  = Position::makeLatLonAlt(41.45570, -95.80260, 0.00);     sPoly0.add(p255);
		Position p256  = Position::makeLatLonAlt(41.45570, -95.79010, 0.00);     sPoly0.add(p256);
		Position p257  = Position::makeLatLonAlt(41.44670, -95.77760, 0.00);     sPoly0.add(p257);
		Position p258  = Position::makeLatLonAlt(41.43770, -95.76500, 0.00);     sPoly0.add(p258);
		Position p259  = Position::makeLatLonAlt(41.42870, -95.76500, 0.00);     sPoly0.add(p259);
		Position p260  = Position::makeLatLonAlt(41.46450, -95.73960, 0.00);     sPoly0.add(p260);
		Position p261  = Position::makeLatLonAlt(41.63660, -95.66320, 0.00);     sPoly0.add(p261);
		Position p262  = Position::makeLatLonAlt(41.82400, -95.60420, 0.00);     sPoly0.add(p262);
		Position p263  = Position::makeLatLonAlt(41.47350, -95.73960, 0.00);     sPoly0.add(p263);
		Position p264  = Position::makeLatLonAlt(41.44670, -95.76500, 0.00);     sPoly0.add(p264);
		Position p265  = Position::makeLatLonAlt(41.46470, -95.75250, 0.00);     sPoly0.add(p265);
		Position p266  = Position::makeLatLonAlt(41.47370, -95.75250, 0.00);     sPoly0.add(p266);
		Position p267  = Position::makeLatLonAlt(41.48270, -95.75250, 0.00);     sPoly0.add(p267);
		Position p268  = Position::makeLatLonAlt(41.49170, -95.75250, 0.00);     sPoly0.add(p268);
		Position p269  = Position::makeLatLonAlt(41.50070, -95.76500, 0.00);     sPoly0.add(p269);
		Position p270  = Position::makeLatLonAlt(41.50970, -95.76500, 0.00);     sPoly0.add(p270);
		Position p271  = Position::makeLatLonAlt(41.51870, -95.77760, 0.00);     sPoly0.add(p271);
		Position p272  = Position::makeLatLonAlt(41.55450, -95.75210, 0.00);     sPoly0.add(p272);
		Position p273  = Position::makeLatLonAlt(41.52770, -95.79010, 0.00);     sPoly0.add(p273);
		Position p274  = Position::makeLatLonAlt(41.52770, -95.81510, 0.00);     sPoly0.add(p274);
		Position p275  = Position::makeLatLonAlt(41.52770, -95.82760, 0.00);     sPoly0.add(p275);
		Position p276  = Position::makeLatLonAlt(41.52770, -95.84010, 0.00);     sPoly0.add(p276);
		Position p277  = Position::makeLatLonAlt(41.51870, -95.86510, 0.00);     sPoly0.add(p277);
		Position p278  = Position::makeLatLonAlt(41.51870, -95.87770, 0.00);     sPoly0.add(p278);
		Position p279  = Position::makeLatLonAlt(41.51870, -95.90270, 0.00);     sPoly0.add(p279);
		Position p280  = Position::makeLatLonAlt(41.51870, -95.91520, 0.00);     sPoly0.add(p280);
		Position p281  = Position::makeLatLonAlt(41.51870, -95.94020, 0.00);     sPoly0.add(p281);
		Position p282  = Position::makeLatLonAlt(41.51870, -95.95270, 0.00);     sPoly0.add(p282);
		Position p283  = Position::makeLatLonAlt(41.52770, -95.96530, 0.00);     sPoly0.add(p283);
		Position p284  = Position::makeLatLonAlt(41.53670, -95.97780, 0.00);     sPoly0.add(p284);
		Position p285  = Position::makeLatLonAlt(41.54570, -95.97780, 0.00);     sPoly0.add(p285);
		Position p286  = Position::makeLatLonAlt(41.55470, -95.97780, 0.00);     sPoly0.add(p286);
		Position p287  = Position::makeLatLonAlt(41.59050, -95.95230, 0.00);     sPoly0.add(p287);
		Position p288  = Position::makeLatLonAlt(41.56370, -95.97780, 0.00);     sPoly0.add(p288);
		Position p289  = Position::makeLatLonAlt(41.59950, -95.95230, 0.00);     sPoly0.add(p289);
		Position p290  = Position::makeLatLonAlt(41.57270, -95.97780, 0.00);     sPoly0.add(p290);
		Position p291  = Position::makeLatLonAlt(41.58170, -95.97780, 0.00);     sPoly0.add(p291);
		Position p292  = Position::makeLatLonAlt(41.61750, -95.95230, 0.00);     sPoly0.add(p292);
		Position p293  = Position::makeLatLonAlt(41.59070, -95.99030, 0.00);     sPoly0.add(p293);
		Position p294  = Position::makeLatLonAlt(41.59970, -96.00280, 0.00);     sPoly0.add(p294);
		Position p295  = Position::makeLatLonAlt(41.60870, -96.01530, 0.00);     sPoly0.add(p295);
		Position p296  = Position::makeLatLonAlt(41.61770, -96.02780, 0.00);     sPoly0.add(p296);
		Position p297  = Position::makeLatLonAlt(41.61770, -96.05290, 0.00);     sPoly0.add(p297);
		Position p298  = Position::makeLatLonAlt(41.61770, -96.06540, 0.00);     sPoly0.add(p298);
		Position p299  = Position::makeLatLonAlt(41.61770, -96.07790, 0.00);     sPoly0.add(p299);
		Position p300  = Position::makeLatLonAlt(41.61770, -96.10290, 0.00);     sPoly0.add(p300);
		Position p301  = Position::makeLatLonAlt(41.61770, -96.11540, 0.00);     sPoly0.add(p301);
		Position p302  = Position::makeLatLonAlt(41.62670, -96.14050, 0.00);     sPoly0.add(p302);
		Position p303  = Position::makeLatLonAlt(41.63570, -96.15300, 0.00);     sPoly0.add(p303);
		Position p304  = Position::makeLatLonAlt(41.64470, -96.16550, 0.00);     sPoly0.add(p304);
		Position p305  = Position::makeLatLonAlt(41.65370, -96.17800, 0.00);     sPoly0.add(p305);
		Position p306  = Position::makeLatLonAlt(41.66270, -96.17800, 0.00);     sPoly0.add(p306);
		Position p307  = Position::makeLatLonAlt(41.68070, -96.19050, 0.00);     sPoly0.add(p307);
		Position p308  = Position::makeLatLonAlt(41.68970, -96.17800, 0.00);     sPoly0.add(p308);
		Position p309  = Position::makeLatLonAlt(41.72550, -96.15260, 0.00);     sPoly0.add(p309);
		Position p310  = Position::makeLatLonAlt(41.68970, -96.16550, 0.00);     sPoly0.add(p310);
		Position p311  = Position::makeLatLonAlt(41.68970, -96.15300, 0.00);     sPoly0.add(p311);
		Position p312  = Position::makeLatLonAlt(41.68070, -96.12790, 0.00);     sPoly0.add(p312);
		Position p313  = Position::makeLatLonAlt(41.67170, -96.11540, 0.00);     sPoly0.add(p313);
		Position p314  = Position::makeLatLonAlt(41.66270, -96.09040, 0.00);     sPoly0.add(p314);
		Position p315  = Position::makeLatLonAlt(41.66270, -96.07790, 0.00);     sPoly0.add(p315);
		Position p316  = Position::makeLatLonAlt(41.65370, -96.06540, 0.00);     sPoly0.add(p316);
		Position p317  = Position::makeLatLonAlt(41.65370, -96.05290, 0.00);     sPoly0.add(p317);
		Position p318  = Position::makeLatLonAlt(41.64470, -96.02780, 0.00);     sPoly0.add(p318);
		Position p319  = Position::makeLatLonAlt(41.64470, -96.01530, 0.00);     sPoly0.add(p319);
		Position p320  = Position::makeLatLonAlt(41.64470, -96.00280, 0.00);     sPoly0.add(p320);
		Position p321  = Position::makeLatLonAlt(41.64470, -95.99030, 0.00);     sPoly0.add(p321);
		Position p322  = Position::makeLatLonAlt(41.63570, -95.96530, 0.00);     sPoly0.add(p322);
		Position p323  = Position::makeLatLonAlt(41.63570, -95.95270, 0.00);     sPoly0.add(p323);
		Position p324  = Position::makeLatLonAlt(41.62670, -95.92770, 0.00);     sPoly0.add(p324);
		Position p325  = Position::makeLatLonAlt(41.62670, -95.91520, 0.00);     sPoly0.add(p325);
		Position p326  = Position::makeLatLonAlt(41.62670, -95.90270, 0.00);     sPoly0.add(p326);
		Position p327  = Position::makeLatLonAlt(41.62670, -95.89020, 0.00);     sPoly0.add(p327);
		Position p328  = Position::makeLatLonAlt(41.62670, -95.86510, 0.00);     sPoly0.add(p328);
		Position p329  = Position::makeLatLonAlt(41.62670, -95.85260, 0.00);     sPoly0.add(p329);
		Position p330  = Position::makeLatLonAlt(41.63570, -95.85260, 0.00);     sPoly0.add(p330);
		Position p331  = Position::makeLatLonAlt(41.64470, -95.84010, 0.00);     sPoly0.add(p331);
		Position p332  = Position::makeLatLonAlt(41.65370, -95.82760, 0.00);     sPoly0.add(p332);
		Position p333  = Position::makeLatLonAlt(41.66270, -95.81510, 0.00);     sPoly0.add(p333);
		Position p334  = Position::makeLatLonAlt(41.67170, -95.80260, 0.00);     sPoly0.add(p334);
		Position p335  = Position::makeLatLonAlt(41.68970, -95.79010, 0.00);     sPoly0.add(p335);
		Position p336  = Position::makeLatLonAlt(41.69870, -95.79010, 0.00);     sPoly0.add(p336);
		Position p337  = Position::makeLatLonAlt(41.71670, -95.77760, 0.00);     sPoly0.add(p337);
		Position p338  = Position::makeLatLonAlt(41.72570, -95.77760, 0.00);     sPoly0.add(p338);
		Position p339  = Position::makeLatLonAlt(41.73470, -95.77760, 0.00);     sPoly0.add(p339);
		Position p340  = Position::makeLatLonAlt(41.74370, -95.77760, 0.00);     sPoly0.add(p340);
		Position p341  = Position::makeLatLonAlt(41.75270, -95.76500, 0.00);     sPoly0.add(p341);
		Position p342  = Position::makeLatLonAlt(41.81570, -95.84010, 0.00);     sPoly0.add(p342);
		Position p343  = Position::makeLatLonAlt(41.81570, -95.85260, 0.00);     sPoly0.add(p343);
		Position p344  = Position::makeLatLonAlt(41.81570, -95.87770, 0.00);     sPoly0.add(p344);
		Position p345  = Position::makeLatLonAlt(41.81570, -95.89020, 0.00);     sPoly0.add(p345);
		Position p346  = Position::makeLatLonAlt(41.85150, -95.86470, 0.00);     sPoly0.add(p346);
		Position p347  = Position::makeLatLonAlt(41.81570, -95.90270, 0.00);     sPoly0.add(p347);
		Position p348  = Position::makeLatLonAlt(41.85150, -95.87730, 0.00);     sPoly0.add(p348);
		Position p349  = Position::makeLatLonAlt(41.82470, -95.91520, 0.00);     sPoly0.add(p349);
		Position p350  = Position::makeLatLonAlt(41.83370, -95.92770, 0.00);     sPoly0.add(p350);
		Position p351  = Position::makeLatLonAlt(41.84270, -95.94020, 0.00);     sPoly0.add(p351);
		Position p352  = Position::makeLatLonAlt(41.85170, -95.95270, 0.00);     sPoly0.add(p352);
		Position p353  = Position::makeLatLonAlt(41.77970, -96.05290, 0.00);     sPoly0.add(p353);
		Position p354  = Position::makeLatLonAlt(41.77970, -96.06540, 0.00);     sPoly0.add(p354);
		Position p355  = Position::makeLatLonAlt(41.77970, -96.07790, 0.00);     sPoly0.add(p355);
		Position p356  = Position::makeLatLonAlt(41.77970, -96.10290, 0.00);     sPoly0.add(p356);
		Position p357  = Position::makeLatLonAlt(41.78870, -96.11540, 0.00);     sPoly0.add(p357);
		Position p358  = Position::makeLatLonAlt(41.78870, -96.12790, 0.00);     sPoly0.add(p358);
		Position p359  = Position::makeLatLonAlt(41.79770, -96.14050, 0.00);     sPoly0.add(p359);
		Position p360  = Position::makeLatLonAlt(41.80670, -96.15300, 0.00);     sPoly0.add(p360);
		Position p361  = Position::makeLatLonAlt(41.81570, -96.15300, 0.00);     sPoly0.add(p361);
		Position p362  = Position::makeLatLonAlt(41.82470, -96.15300, 0.00);     sPoly0.add(p362);
		Position p363  = Position::makeLatLonAlt(41.83370, -96.15300, 0.00);     sPoly0.add(p363);
		Position p364  = Position::makeLatLonAlt(41.84270, -96.16550, 0.00);     sPoly0.add(p364);
		Position p365  = Position::makeLatLonAlt(41.85170, -96.16550, 0.00);     sPoly0.add(p365);
		Position p366  = Position::makeLatLonAlt(41.86970, -96.16550, 0.00);     sPoly0.add(p366);
		Position p367  = Position::makeLatLonAlt(41.87870, -96.16550, 0.00);     sPoly0.add(p367);
		Position p368  = Position::makeLatLonAlt(41.88770, -96.17800, 0.00);     sPoly0.add(p368);
		Position p369  = Position::makeLatLonAlt(41.89670, -96.17800, 0.00);     sPoly0.add(p369);
		Position p370  = Position::makeLatLonAlt(41.90570, -96.19050, 0.00);     sPoly0.add(p370);
		Position p371  = Position::makeLatLonAlt(41.91470, -96.19050, 0.00);     sPoly0.add(p371);
		Position p372  = Position::makeLatLonAlt(41.92370, -96.19050, 0.00);     sPoly0.add(p372);
		Position p373  = Position::makeLatLonAlt(41.93270, -96.19050, 0.00);     sPoly0.add(p373);
		Position p374  = Position::makeLatLonAlt(41.94170, -96.20300, 0.00);     sPoly0.add(p374);
		Position p375  = Position::makeLatLonAlt(41.95070, -96.20300, 0.00);     sPoly0.add(p375);
		Position p376  = Position::makeLatLonAlt(41.98650, -96.17760, 0.00);     sPoly0.add(p376);
		Position p377  = Position::makeLatLonAlt(41.95070, -96.21550, 0.00);     sPoly0.add(p377);
		Position p378  = Position::makeLatLonAlt(41.95070, -96.22810, 0.00);     sPoly0.add(p378);
		Position p379  = Position::makeLatLonAlt(41.95070, -96.25310, 0.00);     sPoly0.add(p379);
		Position p380  = Position::makeLatLonAlt(41.94170, -96.26560, 0.00);     sPoly0.add(p380);
		Position p381  = Position::makeLatLonAlt(41.94170, -96.27810, 0.00);     sPoly0.add(p381);
		Position p382  = Position::makeLatLonAlt(41.93270, -96.30310, 0.00);     sPoly0.add(p382);
		Position p383  = Position::makeLatLonAlt(41.93270, -96.31570, 0.00);     sPoly0.add(p383);
		Position p384  = Position::makeLatLonAlt(41.94170, -96.31570, 0.00);     sPoly0.add(p384);
		Position p385  = Position::makeLatLonAlt(41.94170, -96.32820, 0.00);     sPoly0.add(p385);
		Position p386  = Position::makeLatLonAlt(41.95070, -96.34070, 0.00);     sPoly0.add(p386);
		Position p387  = Position::makeLatLonAlt(41.95970, -96.34070, 0.00);     sPoly0.add(p387);
		Position p388  = Position::makeLatLonAlt(41.96870, -96.34070, 0.00);     sPoly0.add(p388);
		Position p389  = Position::makeLatLonAlt(41.98670, -96.34070, 0.00);     sPoly0.add(p389);
		Position p390  = Position::makeLatLonAlt(41.99570, -96.35320, 0.00);     sPoly0.add(p390);
		Position p391  = Position::makeLatLonAlt(42.00470, -96.35320, 0.00);     sPoly0.add(p391);
		Position p392  = Position::makeLatLonAlt(42.01370, -96.36570, 0.00);     sPoly0.add(p392);
		Position p393  = Position::makeLatLonAlt(42.02270, -96.36570, 0.00);     sPoly0.add(p393);
		Position p394  = Position::makeLatLonAlt(42.03170, -96.37820, 0.00);     sPoly0.add(p394);
		Position p395  = Position::makeLatLonAlt(42.03170, -96.39070, 0.00);     sPoly0.add(p395);
		Velocity v0  = Velocity::mkTrkGsVs(0.000, 0.000, 0.000);

		//DebugSupport.dumpPoly(sPoly0,"spd_poly");

		Plan lpc("Aircraft_1");
		Position pp0  = Position::makeLatLonAlt(41.4384, -96.6862, 5000.00);
		//Position pp0  = Position::makeLatLonAlt(41.3114, -97.8982, 5000.00000000);
		Position pp1  = Position::makeLatLonAlt(41.5266, -95.6632, 5000.00);
		NavPoint np0(pp0,0.000);    	 lpc.addNavPoint(np0);
		NavPoint np1(pp1,1000.0);    	 lpc.addNavPoint(np1);


		//DebugSupport.dumpPlan(lpc,"spd_lpc");
		std::vector<LatLonAlt> polyList; // = new std::vector<LatLonAlt>();
		for (int j = 0; j < sPoly0.size(); j++) {
			polyList.push_back(sPoly0.getVertex(j).lla());
		}
		EXPECT_EQ((unsigned long)sPoly0.size(),polyList.size());
		//fpln(" $$$ polyList.size = "+polyList.size());
		// WxV = [585.56, 1027.20]
//		int numErrs = 0;
//		for (double t = 0.0; t < 1000.0; t= t+1.0) {
//			Position pos = lpc.position(t);
//			bool sPolyContains = sPoly0.contains(pos);
//			double BUFF = 1E-8;
//			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
//			if (sPolyContains != isInside) {
//		    	//fpln(" $$$ speedComparison: METHODS DIFFER AT t = "+t+" sPolyContains = "+sPolyContains+" isInside = "+isInside);
//		    	numErrs++;
//			}
//			//EXPECT_EQ(sPolyContains,isInside);
//		}
//		//fpln(" $$$ numErrs = "+numErrs);
//		double step = 2.0;
//		//long startTimeMs = System.currentTimeMillis();
//        for (double t = 0.0; t < 1000.0; t = t + step) {
//			Position pos = lpc.position(t);
//			bool sPolyContains = sPoly0.contains(pos);
//			//EXPECT_EQ(sPolyContains,isInside);
//		}
//		//double taskTimeA = (System.currentTimeMillis() - startTimeMs) / 1000.0;
//		//fpln("## speedComparison: SimplePoly Simulation elapsed time " + taskTimeA);
//		//startTimeMs = System.currentTimeMillis();
//		double BUFF = 1E-8;
//        for (double t = 0.0; t < 1000.0; t = t + step) {
//			Position pos = lpc.position(t);
//			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
//			//EXPECT_EQ(sPolyContains,isInside);
//		}
//        //double taskTimeB = (System.currentTimeMillis() - startTimeMs) / 1000.0;
//		//fpln("## speedComparison: PolycarpContainmentSpherical elapsed time " + taskTimeB);
//		//fpln("## percentage diff = "+Fm1(100.0*(taskTimeA-taskTimeB)/taskTimeA)+" %");

	}




	void speedComparison2() {
		fpln(" -------------------------- speedComparison2 -------------------------------");
		SimplePoly sPoly0 = SimplePoly(0.0,15163.0);
		Position p0  = Position::makeLatLonAlt(-6.10131379, -93.85335004, 0.00000000);     sPoly0.add(p0);
		Position p1  = Position::makeLatLonAlt(-6.11031379, -93.86585004, 0.00000000);     sPoly0.add(p1);
		Position p2  = Position::makeLatLonAlt(-6.11031379, -93.87835004, 0.00000000);     sPoly0.add(p2);
		Position p3  = Position::makeLatLonAlt(-6.11931379, -93.90335004, 0.00000000);     sPoly0.add(p3);
		Position p4  = Position::makeLatLonAlt(-6.13731379, -93.91585004, 0.00000000);     sPoly0.add(p4);
		Position p5  = Position::makeLatLonAlt(-6.14631379, -93.92835004, 0.00000000);     sPoly0.add(p5);
		Position p6  = Position::makeLatLonAlt(-6.15531379, -93.94095004, 0.00000000);     sPoly0.add(p6);
		Position p7  = Position::makeLatLonAlt(-6.16431379, -93.94095004, 0.00000000);     sPoly0.add(p7);
		Position p8  = Position::makeLatLonAlt(-6.17331379, -93.94095004, 0.00000000);     sPoly0.add(p8);
		Position p9  = Position::makeLatLonAlt(-6.19131379, -93.92835004, 0.00000000);     sPoly0.add(p9);
		Position p10  = Position::makeLatLonAlt(-6.20031379, -93.91585004, 0.00000000);     sPoly0.add(p10);
		Position p11  = Position::makeLatLonAlt(-6.20931379, -93.90335004, 0.00000000);     sPoly0.add(p11);
		Position p12  = Position::makeLatLonAlt(-6.21831379, -93.89085004, 0.00000000);     sPoly0.add(p12);
		Position p13  = Position::makeLatLonAlt(-6.23631379, -93.87835004, 0.00000000);     sPoly0.add(p13);
		Position p14  = Position::makeLatLonAlt(-6.24531379, -93.87835004, 0.00000000);     sPoly0.add(p14);
		Position p15  = Position::makeLatLonAlt(-6.25431379, -93.87835004, 0.00000000);     sPoly0.add(p15);
		Position p16  = Position::makeLatLonAlt(-6.26331379, -93.87835004, 0.00000000);     sPoly0.add(p16);
		Position p17  = Position::makeLatLonAlt(-6.26331379, -93.89085004, 0.00000000);     sPoly0.add(p17);
		Position p18  = Position::makeLatLonAlt(-6.27231379, -93.90335004, 0.00000000);     sPoly0.add(p18);
		Position p19  = Position::makeLatLonAlt(-6.27231379, -93.91585004, 0.00000000);     sPoly0.add(p19);
		Position p20  = Position::makeLatLonAlt(-6.28131379, -93.92835004, 0.00000000);     sPoly0.add(p20);
		Position p21  = Position::makeLatLonAlt(-6.28131379, -93.94095004, 0.00000000);     sPoly0.add(p21);
		Position p22  = Position::makeLatLonAlt(-6.29031379, -93.95345004, 0.00000000);     sPoly0.add(p22);
		Position p23  = Position::makeLatLonAlt(-6.29931379, -93.95345004, 0.00000000);     sPoly0.add(p23);
		Position p24  = Position::makeLatLonAlt(-6.30831379, -93.95345004, 0.00000000);     sPoly0.add(p24);
		Position p25  = Position::makeLatLonAlt(-6.32631379, -93.94095004, 0.00000000);     sPoly0.add(p25);
		Position p26  = Position::makeLatLonAlt(-6.33531379, -93.91585004, 0.00000000);     sPoly0.add(p26);
		Position p27  = Position::makeLatLonAlt(-6.34431379, -93.90335004, 0.00000000);     sPoly0.add(p27);
		Position p28  = Position::makeLatLonAlt(-6.35331379, -93.87835004, 0.00000000);     sPoly0.add(p28);
		Position p29  = Position::makeLatLonAlt(-6.35331379, -93.86585004, 0.00000000);     sPoly0.add(p29);
		Position p30  = Position::makeLatLonAlt(-6.43431379, -93.89085004, 0.00000000);     sPoly0.add(p30);
		Position p31  = Position::makeLatLonAlt(-6.43431379, -93.86585004, 0.00000000);     sPoly0.add(p31);
		Position p32  = Position::makeLatLonAlt(-6.43431379, -93.85335004, 0.00000000);     sPoly0.add(p32);
		Position p33  = Position::makeLatLonAlt(-6.44331379, -93.85335004, 0.00000000);     sPoly0.add(p33);
		Position p34  = Position::makeLatLonAlt(-6.49731379, -93.85335004, 0.00000000);     sPoly0.add(p34);
		Position p35  = Position::makeLatLonAlt(-6.50631379, -93.85335004, 0.00000000);     sPoly0.add(p35);
		Position p36  = Position::makeLatLonAlt(-6.51531379, -93.86585004, 0.00000000);     sPoly0.add(p36);
		Position p37  = Position::makeLatLonAlt(-6.29101379, -94.28985004, 0.00000000);     sPoly0.add(p37);
		Position p38  = Position::makeLatLonAlt(-6.47181379, -94.49945004, 0.00000000);     sPoly0.add(p38);
		Position p39  = Position::makeLatLonAlt(-6.86631379, -94.24125004, 0.00000000);     sPoly0.add(p39);
		Position p40  = Position::makeLatLonAlt(-6.87531379, -94.25375004, 0.00000000);     sPoly0.add(p40);
		Position p41  = Position::makeLatLonAlt(-6.87531379, -94.27875004, 0.00000000);     sPoly0.add(p41);
		Position p42  = Position::makeLatLonAlt(-6.88431379, -94.29125004, 0.00000000);     sPoly0.add(p42);
		Position p43  = Position::makeLatLonAlt(-6.89331379, -94.31635004, 0.00000000);     sPoly0.add(p43);
		Position p44  = Position::makeLatLonAlt(-6.89331379, -94.32885004, 0.00000000);     sPoly0.add(p44);
		Position p45  = Position::makeLatLonAlt(-6.89331379, -94.34135004, 0.00000000);     sPoly0.add(p45);
		Position p46  = Position::makeLatLonAlt(-6.90231379, -94.35385004, 0.00000000);     sPoly0.add(p46);
		Position p47  = Position::makeLatLonAlt(-6.90231379, -94.36635004, 0.00000000);     sPoly0.add(p47);
		Position p48  = Position::makeLatLonAlt(-6.91131379, -94.37885004, 0.00000000);     sPoly0.add(p48);
		Position p49  = Position::makeLatLonAlt(-6.92031379, -94.37885004, 0.00000000);     sPoly0.add(p49);
		Position p50  = Position::makeLatLonAlt(-6.92931379, -94.39145004, 0.00000000);     sPoly0.add(p50);
		Position p51  = Position::makeLatLonAlt(-6.93831379, -94.40395004, 0.00000000);     sPoly0.add(p51);
		Position p52  = Position::makeLatLonAlt(-6.95631379, -94.40395004, 0.00000000);     sPoly0.add(p52);
		Position p53  = Position::makeLatLonAlt(-6.96531379, -94.41645004, 0.00000000);     sPoly0.add(p53);
		Position p54  = Position::makeLatLonAlt(-6.97431379, -94.41645004, 0.00000000);     sPoly0.add(p54);
		Position p55  = Position::makeLatLonAlt(-6.98331379, -94.41645004, 0.00000000);     sPoly0.add(p55);
		Position p56  = Position::makeLatLonAlt(-6.99231379, -94.42895004, 0.00000000);     sPoly0.add(p56);
		Position p57  = Position::makeLatLonAlt(-7.00131379, -94.42895004, 0.00000000);     sPoly0.add(p57);
		Position p58  = Position::makeLatLonAlt(-7.01931379, -94.41645004, 0.00000000);     sPoly0.add(p58);
		Position p59  = Position::makeLatLonAlt(-7.02831379, -94.41645004, 0.00000000);     sPoly0.add(p59);
		Position p60  = Position::makeLatLonAlt(-7.04631379, -94.41645004, 0.00000000);     sPoly0.add(p60);
		Position p61  = Position::makeLatLonAlt(-7.05531379, -94.40395004, 0.00000000);     sPoly0.add(p61);
		Position p62  = Position::makeLatLonAlt(-7.06431379, -94.39145004, 0.00000000);     sPoly0.add(p62);
		Position p63  = Position::makeLatLonAlt(-7.07331379, -94.37885004, 0.00000000);     sPoly0.add(p63);
		Position p64  = Position::makeLatLonAlt(-7.07331379, -94.36635004, 0.00000000);     sPoly0.add(p64);
		Position p65  = Position::makeLatLonAlt(-7.06431379, -94.34135004, 0.00000000);     sPoly0.add(p65);
		Position p66  = Position::makeLatLonAlt(-7.06431379, -94.32885004, 0.00000000);     sPoly0.add(p66);
		Position p67  = Position::makeLatLonAlt(-7.06431379, -94.31635004, 0.00000000);     sPoly0.add(p67);
		Position p68  = Position::makeLatLonAlt(-7.06431379, -94.30385004, 0.00000000);     sPoly0.add(p68);
		Position p69  = Position::makeLatLonAlt(-7.06431379, -94.29125004, 0.00000000);     sPoly0.add(p69);
		Position p70  = Position::makeLatLonAlt(-7.07331379, -94.29125004, 0.00000000);     sPoly0.add(p70);
		Position p71  = Position::makeLatLonAlt(-7.09131379, -94.29125004, 0.00000000);     sPoly0.add(p71);
		Position p72  = Position::makeLatLonAlt(-7.10031379, -94.29125004, 0.00000000);     sPoly0.add(p72);
		Position p73  = Position::makeLatLonAlt(-7.11831379, -94.27875004, 0.00000000);     sPoly0.add(p73);
		Position p74  = Position::makeLatLonAlt(-7.12731379, -94.27875004, 0.00000000);     sPoly0.add(p74);
		Position p75  = Position::makeLatLonAlt(-7.13631379, -94.27875004, 0.00000000);     sPoly0.add(p75);
		Position p76  = Position::makeLatLonAlt(-7.13631379, -94.26625004, 0.00000000);     sPoly0.add(p76);
		Position p77  = Position::makeLatLonAlt(-7.13631379, -94.25375004, 0.00000000);     sPoly0.add(p77);
		Position p78  = Position::makeLatLonAlt(-7.13631379, -94.24125004, 0.00000000);     sPoly0.add(p78);
		Position p79  = Position::makeLatLonAlt(-7.13631379, -94.21625004, 0.00000000);     sPoly0.add(p79);
		Position p80  = Position::makeLatLonAlt(-7.14531379, -94.20375004, 0.00000000);     sPoly0.add(p80);
		Position p81  = Position::makeLatLonAlt(-7.15431379, -94.20375004, 0.00000000);     sPoly0.add(p81);
		Position p82  = Position::makeLatLonAlt(-7.16331379, -94.20375004, 0.00000000);     sPoly0.add(p82);
		Position p83  = Position::makeLatLonAlt(-7.18131379, -94.20375004, 0.00000000);     sPoly0.add(p83);
		Position p84  = Position::makeLatLonAlt(-7.19031379, -94.20375004, 0.00000000);     sPoly0.add(p84);
		Position p85  = Position::makeLatLonAlt(-7.20831379, -94.21625004, 0.00000000);     sPoly0.add(p85);
		Position p86  = Position::makeLatLonAlt(-7.21731379, -94.20375004, 0.00000000);     sPoly0.add(p86);
		Position p87  = Position::makeLatLonAlt(-7.23531379, -94.20375004, 0.00000000);     sPoly0.add(p87);
		Position p88  = Position::makeLatLonAlt(-7.24431379, -94.19115004, 0.00000000);     sPoly0.add(p88);
		Position p89  = Position::makeLatLonAlt(-7.25331379, -94.19115004, 0.00000000);     sPoly0.add(p89);
		Position p90  = Position::makeLatLonAlt(-7.26231379, -94.17865004, 0.00000000);     sPoly0.add(p90);
		Position p91  = Position::makeLatLonAlt(-7.27131379, -94.16615004, 0.00000000);     sPoly0.add(p91);
		Position p92  = Position::makeLatLonAlt(-7.28031379, -94.15365004, 0.00000000);     sPoly0.add(p92);
		Position p93  = Position::makeLatLonAlt(-7.29831379, -94.14115004, 0.00000000);     sPoly0.add(p93);
		Position p94  = Position::makeLatLonAlt(-7.29831379, -94.11615004, 0.00000000);     sPoly0.add(p94);
		Position p95  = Position::makeLatLonAlt(-7.30731379, -94.10355004, 0.00000000);     sPoly0.add(p95);
		Position p96  = Position::makeLatLonAlt(-7.31631379, -94.09105004, 0.00000000);     sPoly0.add(p96);
		Position p97  = Position::makeLatLonAlt(-7.31631379, -94.07855004, 0.00000000);     sPoly0.add(p97);
		Position p98  = Position::makeLatLonAlt(-7.31631379, -94.05355004, 0.00000000);     sPoly0.add(p98);
		Position p99  = Position::makeLatLonAlt(-7.30731379, -94.02855004, 0.00000000);     sPoly0.add(p99);
		Position p100  = Position::makeLatLonAlt(-7.30731379, -94.01595004, 0.00000000);     sPoly0.add(p100);
		Position p101  = Position::makeLatLonAlt(-7.29831379, -94.00345004, 0.00000000);     sPoly0.add(p101);
		Position p102  = Position::makeLatLonAlt(-7.28931379, -93.97845004, 0.00000000);     sPoly0.add(p102);
		Position p103  = Position::makeLatLonAlt(-7.28031379, -93.97845004, 0.00000000);     sPoly0.add(p103);
		Position p104  = Position::makeLatLonAlt(-7.27131379, -93.96595004, 0.00000000);     sPoly0.add(p104);
		Position p105  = Position::makeLatLonAlt(-7.25331379, -93.95345004, 0.00000000);     sPoly0.add(p105);
		Position p106  = Position::makeLatLonAlt(-7.24431379, -93.95345004, 0.00000000);     sPoly0.add(p106);
		Position p107  = Position::makeLatLonAlt(-7.23531379, -93.94095004, 0.00000000);     sPoly0.add(p107);
		Position p108  = Position::makeLatLonAlt(-7.22631379, -93.92835004, 0.00000000);     sPoly0.add(p108);
		Position p109  = Position::makeLatLonAlt(-7.21731379, -93.91585004, 0.00000000);     sPoly0.add(p109);
		Position p110  = Position::makeLatLonAlt(-7.21731379, -93.90335004, 0.00000000);     sPoly0.add(p110);
		Position p111  = Position::makeLatLonAlt(-7.21731379, -93.89085004, 0.00000000);     sPoly0.add(p111);
		Position p112  = Position::makeLatLonAlt(-7.22631379, -93.87835004, 0.00000000);     sPoly0.add(p112);
		Position p113  = Position::makeLatLonAlt(-7.10031379, -93.65305004, 0.00000000);     sPoly0.add(p113);
		Position p114  = Position::makeLatLonAlt(-7.09131379, -93.64055004, 0.00000000);     sPoly0.add(p114);
		Position p115  = Position::makeLatLonAlt(-7.08231379, -93.64055004, 0.00000000);     sPoly0.add(p115);
		Position p116  = Position::makeLatLonAlt(-7.07331379, -93.64055004, 0.00000000);     sPoly0.add(p116);
		Position p117  = Position::makeLatLonAlt(-7.05531379, -93.64055004, 0.00000000);     sPoly0.add(p117);
		Position p118  = Position::makeLatLonAlt(-7.04631379, -93.64055004, 0.00000000);     sPoly0.add(p118);
		Position p119  = Position::makeLatLonAlt(-7.03731379, -93.64055004, 0.00000000);     sPoly0.add(p119);
		Position p120  = Position::makeLatLonAlt(-7.02831379, -93.62805004, 0.00000000);     sPoly0.add(p120);
		Position p121  = Position::makeLatLonAlt(-7.01931379, -93.62805004, 0.00000000);     sPoly0.add(p121);
		Position p122  = Position::makeLatLonAlt(-7.01031379, -93.61555004, 0.00000000);     sPoly0.add(p122);
		Position p123  = Position::makeLatLonAlt(-7.01031379, -93.60305004, 0.00000000);     sPoly0.add(p123);
		Position p124  = Position::makeLatLonAlt(-7.01031379, -93.59055004, 0.00000000);     sPoly0.add(p124);
		Position p125  = Position::makeLatLonAlt(-7.00131379, -93.57805004, 0.00000000);     sPoly0.add(p125);
		Position p126  = Position::makeLatLonAlt(-7.00131379, -93.56545004, 0.00000000);     sPoly0.add(p126);
		Position p127  = Position::makeLatLonAlt(-7.00131379, -93.55295004, 0.00000000);     sPoly0.add(p127);
		Position p128  = Position::makeLatLonAlt(-7.00131379, -93.52795004, 0.00000000);     sPoly0.add(p128);
		Position p129  = Position::makeLatLonAlt(-7.00131379, -93.51545004, 0.00000000);     sPoly0.add(p129);
		Position p130  = Position::makeLatLonAlt(-7.00131379, -93.50295004, 0.00000000);     sPoly0.add(p130);
		Position p131  = Position::makeLatLonAlt(-7.00131379, -93.47785004, 0.00000000);     sPoly0.add(p131);
		Position p132  = Position::makeLatLonAlt(-7.00131379, -93.46535004, 0.00000000);     sPoly0.add(p132);
		Position p133  = Position::makeLatLonAlt(-7.00131379, -93.45285004, 0.00000000);     sPoly0.add(p133);
		Position p134  = Position::makeLatLonAlt(-6.96551379, -93.42745004, 0.00000000);     sPoly0.add(p134);
		Position p135  = Position::makeLatLonAlt(-6.67731379, -93.52795004, 0.00000000);     sPoly0.add(p135);
		Position p136  = Position::makeLatLonAlt(-6.67731379, -93.51545004, 0.00000000);     sPoly0.add(p136);
		Position p137  = Position::makeLatLonAlt(-6.67731379, -93.50295004, 0.00000000);     sPoly0.add(p137);
		Position p138  = Position::makeLatLonAlt(-6.67731379, -93.49045004, 0.00000000);     sPoly0.add(p138);
		Position p139  = Position::makeLatLonAlt(-6.67731379, -93.47785004, 0.00000000);     sPoly0.add(p139);
		Position p140  = Position::makeLatLonAlt(-6.66831379, -93.46535004, 0.00000000);     sPoly0.add(p140);
		Position p141  = Position::makeLatLonAlt(-6.66831379, -93.45285004, 0.00000000);     sPoly0.add(p141);
		Position p142  = Position::makeLatLonAlt(-6.65931379, -93.42785004, 0.00000000);     sPoly0.add(p142);
		Position p143  = Position::makeLatLonAlt(-6.65931379, -93.41535004, 0.00000000);     sPoly0.add(p143);
		Position p144  = Position::makeLatLonAlt(-6.65931379, -93.40285004, 0.00000000);     sPoly0.add(p144);
		Position p145  = Position::makeLatLonAlt(-6.66831379, -93.39025004, 0.00000000);     sPoly0.add(p145);
		Position p146  = Position::makeLatLonAlt(-6.67731379, -93.37775004, 0.00000000);     sPoly0.add(p146);
		Position p147  = Position::makeLatLonAlt(-6.68631379, -93.36525004, 0.00000000);     sPoly0.add(p147);
		Position p148  = Position::makeLatLonAlt(-6.68631379, -93.35275004, 0.00000000);     sPoly0.add(p148);
		Position p149  = Position::makeLatLonAlt(-6.69531379, -93.34025004, 0.00000000);     sPoly0.add(p149);
		Position p150  = Position::makeLatLonAlt(-6.69531379, -93.32775004, 0.00000000);     sPoly0.add(p150);
		Position p151  = Position::makeLatLonAlt(-6.68631379, -93.31525004, 0.00000000);     sPoly0.add(p151);
		Position p152  = Position::makeLatLonAlt(-6.67731379, -93.29015004, 0.00000000);     sPoly0.add(p152);
		Position p153  = Position::makeLatLonAlt(-6.67731379, -93.27765004, 0.00000000);     sPoly0.add(p153);
		Position p154  = Position::makeLatLonAlt(-6.66831379, -93.25265004, 0.00000000);     sPoly0.add(p154);
		Position p155  = Position::makeLatLonAlt(-6.67731379, -93.24015004, 0.00000000);     sPoly0.add(p155);
		Position p156  = Position::makeLatLonAlt(-6.67731379, -93.22765004, 0.00000000);     sPoly0.add(p156);
		Position p157  = Position::makeLatLonAlt(-6.68631379, -93.21515004, 0.00000000);     sPoly0.add(p157);
		Position p158  = Position::makeLatLonAlt(-6.69531379, -93.20255004, 0.00000000);     sPoly0.add(p158);
		Position p159  = Position::makeLatLonAlt(-6.70431379, -93.20255004, 0.00000000);     sPoly0.add(p159);
		Position p160  = Position::makeLatLonAlt(-6.66851379, -93.17715004, 0.00000000);     sPoly0.add(p160);
		Position p161  = Position::makeLatLonAlt(-6.49641379, -93.10075004, 0.00000000);     sPoly0.add(p161);
		Position p162  = Position::makeLatLonAlt(-6.54231379, -93.42785004, 0.00000000);     sPoly0.add(p162);
		Position p163  = Position::makeLatLonAlt(-6.53331379, -93.44035004, 0.00000000);     sPoly0.add(p163);
		Position p164  = Position::makeLatLonAlt(-6.52431379, -93.45285004, 0.00000000);     sPoly0.add(p164);
		Position p165  = Position::makeLatLonAlt(-6.51531379, -93.46535004, 0.00000000);     sPoly0.add(p165);
		Position p166  = Position::makeLatLonAlt(-6.51531379, -93.49045004, 0.00000000);     sPoly0.add(p166);
		Position p167  = Position::makeLatLonAlt(-6.51531379, -93.50295004, 0.00000000);     sPoly0.add(p167);
		Position p168  = Position::makeLatLonAlt(-6.51531379, -93.51545004, 0.00000000);     sPoly0.add(p168);
		Position p169  = Position::makeLatLonAlt(-6.51531379, -93.54045004, 0.00000000);     sPoly0.add(p169);
		Position p170  = Position::makeLatLonAlt(-6.51531379, -93.55295004, 0.00000000);     sPoly0.add(p170);
		Position p171  = Position::makeLatLonAlt(-6.50631379, -93.57805004, 0.00000000);     sPoly0.add(p171);
		Position p172  = Position::makeLatLonAlt(-6.49731379, -93.59055004, 0.00000000);     sPoly0.add(p172);
		Position p173  = Position::makeLatLonAlt(-6.48831379, -93.60305004, 0.00000000);     sPoly0.add(p173);
		Position p174  = Position::makeLatLonAlt(-6.47931379, -93.61555004, 0.00000000);     sPoly0.add(p174);
		Position p175  = Position::makeLatLonAlt(-6.47031379, -93.61555004, 0.00000000);     sPoly0.add(p175);
		Position p176  = Position::makeLatLonAlt(-6.45231379, -93.62805004, 0.00000000);     sPoly0.add(p176);
		Position p177  = Position::makeLatLonAlt(-6.44331379, -93.61555004, 0.00000000);     sPoly0.add(p177);
		Position p178  = Position::makeLatLonAlt(-6.44331379, -93.60305004, 0.00000000);     sPoly0.add(p178);
		Position p179  = Position::makeLatLonAlt(-6.44331379, -93.59055004, 0.00000000);     sPoly0.add(p179);
		Position p180  = Position::makeLatLonAlt(-6.45231379, -93.56545004, 0.00000000);     sPoly0.add(p180);
		Position p181  = Position::makeLatLonAlt(-6.46131379, -93.55295004, 0.00000000);     sPoly0.add(p181);
		Position p182  = Position::makeLatLonAlt(-6.47031379, -93.52795004, 0.00000000);     sPoly0.add(p182);
		Position p183  = Position::makeLatLonAlt(-6.47031379, -93.51545004, 0.00000000);     sPoly0.add(p183);
		Position p184  = Position::makeLatLonAlt(-6.47931379, -93.50295004, 0.00000000);     sPoly0.add(p184);
		Position p185  = Position::makeLatLonAlt(-6.47931379, -93.49045004, 0.00000000);     sPoly0.add(p185);
		Position p186  = Position::makeLatLonAlt(-6.48831379, -93.46535004, 0.00000000);     sPoly0.add(p186);
		Position p187  = Position::makeLatLonAlt(-6.48831379, -93.45285004, 0.00000000);     sPoly0.add(p187);
		Position p188  = Position::makeLatLonAlt(-6.48831379, -93.44035004, 0.00000000);     sPoly0.add(p188);
		Position p189  = Position::makeLatLonAlt(-6.48831379, -93.42785004, 0.00000000);     sPoly0.add(p189);
		Position p190  = Position::makeLatLonAlt(-6.49731379, -93.40285004, 0.00000000);     sPoly0.add(p190);
		Position p191  = Position::makeLatLonAlt(-6.49731379, -93.39025004, 0.00000000);     sPoly0.add(p191);
		Position p192  = Position::makeLatLonAlt(-6.50631379, -93.36525004, 0.00000000);     sPoly0.add(p192);
		Position p193  = Position::makeLatLonAlt(-6.50631379, -93.35275004, 0.00000000);     sPoly0.add(p193);
		Position p194  = Position::makeLatLonAlt(-6.50631379, -93.34025004, 0.00000000);     sPoly0.add(p194);
		Position p195  = Position::makeLatLonAlt(-6.50631379, -93.32775004, 0.00000000);     sPoly0.add(p195);
		Position p196  = Position::makeLatLonAlt(-6.50631379, -93.30265004, 0.00000000);     sPoly0.add(p196);
		Position p197  = Position::makeLatLonAlt(-6.50631379, -93.29015004, 0.00000000);     sPoly0.add(p197);
		Position p198  = Position::makeLatLonAlt(-6.49731379, -93.29015004, 0.00000000);     sPoly0.add(p198);
		Position p199  = Position::makeLatLonAlt(-6.48831379, -93.27765004, 0.00000000);     sPoly0.add(p199);
		Position p200  = Position::makeLatLonAlt(-6.47931379, -93.26515004, 0.00000000);     sPoly0.add(p200);
		Position p201  = Position::makeLatLonAlt(-6.47031379, -93.25265004, 0.00000000);     sPoly0.add(p201);
		Position p202  = Position::makeLatLonAlt(-6.46131379, -93.24015004, 0.00000000);     sPoly0.add(p202);
		Position p203  = Position::makeLatLonAlt(-6.44331379, -93.22765004, 0.00000000);     sPoly0.add(p203);
		Position p204  = Position::makeLatLonAlt(-6.43431379, -93.22765004, 0.00000000);     sPoly0.add(p204);
		Position p205  = Position::makeLatLonAlt(-6.41631379, -93.21515004, 0.00000000);     sPoly0.add(p205);
		Position p206  = Position::makeLatLonAlt(-6.40731379, -93.21515004, 0.00000000);     sPoly0.add(p206);
		Position p207  = Position::makeLatLonAlt(-6.39831379, -93.21515004, 0.00000000);     sPoly0.add(p207);
		Position p208  = Position::makeLatLonAlt(-6.38931379, -93.21515004, 0.00000000);     sPoly0.add(p208);
		Position p209  = Position::makeLatLonAlt(-6.38031379, -93.20255004, 0.00000000);     sPoly0.add(p209);
		Position p210  = Position::makeLatLonAlt(-6.31731379, -93.27765004, 0.00000000);     sPoly0.add(p210);
		Position p211  = Position::makeLatLonAlt(-6.31731379, -93.29015004, 0.00000000);     sPoly0.add(p211);
		Position p212  = Position::makeLatLonAlt(-6.31731379, -93.31525004, 0.00000000);     sPoly0.add(p212);
		Position p213  = Position::makeLatLonAlt(-6.31731379, -93.32775004, 0.00000000);     sPoly0.add(p213);
		Position p214  = Position::makeLatLonAlt(-6.31731379, -93.34025004, 0.00000000);     sPoly0.add(p214);
		Position p215  = Position::makeLatLonAlt(-6.30831379, -93.35275004, 0.00000000);     sPoly0.add(p215);
		Position p216  = Position::makeLatLonAlt(-6.29931379, -93.36525004, 0.00000000);     sPoly0.add(p216);
		Position p217  = Position::makeLatLonAlt(-6.29031379, -93.37775004, 0.00000000);     sPoly0.add(p217);
		Position p218  = Position::makeLatLonAlt(-6.28131379, -93.39025004, 0.00000000);     sPoly0.add(p218);
		Position p219  = Position::makeLatLonAlt(-6.35331379, -93.49045004, 0.00000000);     sPoly0.add(p219);
		Position p220  = Position::makeLatLonAlt(-6.35331379, -93.50295004, 0.00000000);     sPoly0.add(p220);
		Position p221  = Position::makeLatLonAlt(-6.35331379, -93.51545004, 0.00000000);     sPoly0.add(p221);
		Position p222  = Position::makeLatLonAlt(-6.35331379, -93.54045004, 0.00000000);     sPoly0.add(p222);
		Position p223  = Position::makeLatLonAlt(-6.34431379, -93.55295004, 0.00000000);     sPoly0.add(p223);
		Position p224  = Position::makeLatLonAlt(-6.34431379, -93.56545004, 0.00000000);     sPoly0.add(p224);
		Position p225  = Position::makeLatLonAlt(-6.33531379, -93.57805004, 0.00000000);     sPoly0.add(p225);
		Position p226  = Position::makeLatLonAlt(-6.32631379, -93.59055004, 0.00000000);     sPoly0.add(p226);
		Position p227  = Position::makeLatLonAlt(-6.31731379, -93.59055004, 0.00000000);     sPoly0.add(p227);
		Position p228  = Position::makeLatLonAlt(-6.30831379, -93.59055004, 0.00000000);     sPoly0.add(p228);
		Position p229  = Position::makeLatLonAlt(-6.29931379, -93.59055004, 0.00000000);     sPoly0.add(p229);
		Position p230  = Position::makeLatLonAlt(-6.29031379, -93.60305004, 0.00000000);     sPoly0.add(p230);
		Position p231  = Position::makeLatLonAlt(-6.28131379, -93.60305004, 0.00000000);     sPoly0.add(p231);
		Position p232  = Position::makeLatLonAlt(-6.26331379, -93.60305004, 0.00000000);     sPoly0.add(p232);
		Position p233  = Position::makeLatLonAlt(-6.25431379, -93.60305004, 0.00000000);     sPoly0.add(p233);
		Position p234  = Position::makeLatLonAlt(-6.24531379, -93.61555004, 0.00000000);     sPoly0.add(p234);
		Position p235  = Position::makeLatLonAlt(-6.23631379, -93.61555004, 0.00000000);     sPoly0.add(p235);
		Position p236  = Position::makeLatLonAlt(-6.22731379, -93.62805004, 0.00000000);     sPoly0.add(p236);
		Position p237  = Position::makeLatLonAlt(-6.21831379, -93.62805004, 0.00000000);     sPoly0.add(p237);
		Position p238  = Position::makeLatLonAlt(-6.20931379, -93.62805004, 0.00000000);     sPoly0.add(p238);
		Position p239  = Position::makeLatLonAlt(-6.20031379, -93.62805004, 0.00000000);     sPoly0.add(p239);
		Position p240  = Position::makeLatLonAlt(-6.19131379, -93.64055004, 0.00000000);     sPoly0.add(p240);
		Position p241  = Position::makeLatLonAlt(-6.18231379, -93.64055004, 0.00000000);     sPoly0.add(p241);
		Position p242  = Position::makeLatLonAlt(-6.18231379, -93.65305004, 0.00000000);     sPoly0.add(p242);
		Position p243  = Position::makeLatLonAlt(-6.18231379, -93.66565004, 0.00000000);     sPoly0.add(p243);
		Position p244  = Position::makeLatLonAlt(-6.18231379, -93.69065004, 0.00000000);     sPoly0.add(p244);
		Position p245  = Position::makeLatLonAlt(-6.19131379, -93.70315004, 0.00000000);     sPoly0.add(p245);
		Position p246  = Position::makeLatLonAlt(-6.19131379, -93.71565004, 0.00000000);     sPoly0.add(p246);
		Position p247  = Position::makeLatLonAlt(-6.20031379, -93.74065004, 0.00000000);     sPoly0.add(p247);
		Position p248  = Position::makeLatLonAlt(-6.20031379, -93.75325004, 0.00000000);     sPoly0.add(p248);
		Position p249  = Position::makeLatLonAlt(-6.19131379, -93.75325004, 0.00000000);     sPoly0.add(p249);
		Position p250  = Position::makeLatLonAlt(-6.19131379, -93.76575004, 0.00000000);     sPoly0.add(p250);
		Position p251  = Position::makeLatLonAlt(-6.18231379, -93.77825004, 0.00000000);     sPoly0.add(p251);
		Position p252  = Position::makeLatLonAlt(-6.17331379, -93.77825004, 0.00000000);     sPoly0.add(p252);
		Position p253  = Position::makeLatLonAlt(-6.16431379, -93.77825004, 0.00000000);     sPoly0.add(p253);
		Position p254  = Position::makeLatLonAlt(-6.14631379, -93.77825004, 0.00000000);     sPoly0.add(p254);
		Position p255  = Position::makeLatLonAlt(-6.13731379, -93.79075004, 0.00000000);     sPoly0.add(p255);
		Position p256  = Position::makeLatLonAlt(-6.12831379, -93.79075004, 0.00000000);     sPoly0.add(p256);
		Position p257  = Position::makeLatLonAlt(-6.11931379, -93.80325004, 0.00000000);     sPoly0.add(p257);
		Position p258  = Position::makeLatLonAlt(-6.11031379, -93.80325004, 0.00000000);     sPoly0.add(p258);
		Position p259  = Position::makeLatLonAlt(-6.10131379, -93.81575004, 0.00000000);     sPoly0.add(p259);
		Position p260  = Position::makeLatLonAlt(-6.10131379, -93.82825004, 0.00000000);     sPoly0.add(p260);


		//DebugSupport.dumpPoly(sPoly0,"spd_poly2");

		Plan lpc("Aircraft_1");
		Position pA  = Position::makeLatLonAlt(-6.04598297, -94.32443129, 5000.00000000);
		Position pB  = Position::makeLatLonAlt(-6.80637769, -93.22777312, 5000.00000000);
		NavPoint np0(pA,0.000000);    	 lpc.addNavPoint(np0);
		NavPoint np1(pB,1000.0);    	 lpc.addNavPoint(np1);
		//DebugSupport.dumpPlan(lpc,"spd_lpc2");
		std::vector<LatLonAlt> polyList; //  = new std::vector<LatLonAlt>();
		for (int j = 0; j < sPoly0.size(); j++) {
			polyList.push_back(sPoly0.getVertex(j).lla());
		}
		EXPECT_EQ((unsigned long)sPoly0.size(),polyList.size());
		//fpln(" $$$ polyList.size = "+polyList.size());
		// WxV = [585.56, 1027.20]
		int numErrs = 0;
		double BUFF = 1E-8;
		for (double t = 0.0; t < 1000.0; t= t+0.1) {
			Position pos = lpc.position(t);
			bool sPolyContains = sPoly0.contains(pos);
			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
			if (sPolyContains != isInside) {
				//fpln(" $$$ speedComparison2: METHODS DIFFER AT t = "+t+" sPolyContains = "+sPolyContains+" isInside = "+isInside);
				numErrs++;
			}
			//EXPECT_EQ(sPolyContains,isInside);
		}
//		//fpln(" $$$ numErrs = "+numErrs);
//		double step = 1.0;
//		//long startTimeMs = System.currentTimeMillis();
//        for (double t = 0.0; t < 1000.0; t = t + step) {
//			Position pos = lpc.position(t);
//			bool sPolyContains = sPoly0.contains(pos);
//			//EXPECT_EQ(sPolyContains,isInside);
//		}
//		//double taskTimeA = (System.currentTimeMillis() - startTimeMs) / 1000.0;
//		fpln("## speedComparison2: SimplePoly Simulation elapsed time " + taskTimeA);
//		//startTimeMs = System.currentTimeMillis();
//        for (double t = 0.0; t < 1000.0; t = t + step) {
//			Position pos = lpc.position(t);
//			bool isInside = Poly2DLLCore::spherical_inside(polyList, pos.lla(), BUFF);
//			//EXPECT_EQ(sPolyContains,isInside);
//		}
//        //double taskTimeB = (System.currentTimeMillis() - startTimeMs) / 1000.0;
//		fpln("## speedComparison2: PolycarpContainmentSpherical elapsed time " + taskTimeB);
//		fpln("## percentage diff = "+Fm1(100.0*(taskTimeA-taskTimeB)/taskTimeA)+" %");

}

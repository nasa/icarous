/* Copyright (c) 2015-2019 United States Government as represented by
		* the National Aeronautics and Space Administration.  No copyright
		* is claimed in the United States under Title 17, U.S.Code. All Other
		* Rights Reserved.
		*/

#include "Vect2.h"
#include <cmath>
#include <gtest/gtest.h>
#include "SimplePoly.h"
#include "Position.h"
#include "PolyPath.h"
#include "PolyUtil.h"
#include "Projection.h"
#include "format.h"

		using namespace larcfm;

class SimplePolyTest : public ::testing::Test {

public:

protected:
	virtual void SetUp() {
	}
};



TEST_F(SimplePolyTest, test0) {
	SimplePoly sPoly1 = SimplePoly(Units::from("ft",0),Units::from("ft",10000));
	EXPECT_EQ(0,sPoly1.size());
	Position cent0 = sPoly1.centroid();
	EXPECT_TRUE(cent0.isInvalid());
	EXPECT_TRUE(ISNAN(sPoly1.maxRadius()));
	EXPECT_TRUE(ISNAN(sPoly1.boundingCircleRadius()));
	EXPECT_TRUE(ISNAN(sPoly1.boundingCircleRadius()));
	EXPECT_TRUE(ISNAN(sPoly1.distanceFromEdge(sPoly1.getVertex(0))));
	Position p0(LatLonAlt::make(0.25,  4.7, 5000));
	EXPECT_FALSE(sPoly1.contains(p0));
	Position p1(LatLonAlt::make(0.28,  4.7, 5000));
	Position p2(LatLonAlt::make(0.28,  4.9, 5000));
	Position p3(LatLonAlt::make(0.25,  4.9, 5000));
	sPoly1.add(p0);
	sPoly1.add(p1);
	sPoly1.add(p2);
	sPoly1.add(p3);
	EXPECT_TRUE(sPoly1.isConvex());
	EXPECT_TRUE(sPoly1.isClockwise());
	EXPECT_EQ(4,sPoly1.size());
	Position avgpt = sPoly1.averagePoint();
	EXPECT_TRUE(sPoly1.validate());
	//	fpln(" %% avgpt = "+pos.toString12NP());
	Position center(LatLonAlt::make(0.265,  4.8, 5000));
	EXPECT_NEAR(avgpt.lat(),center.lat(),0.00001);
	EXPECT_NEAR(avgpt.lon(),center.lon(),0.00001);
	EXPECT_NEAR(avgpt.alt(),center.alt(),0.00001);

	Position cent = sPoly1.centroid();
	EXPECT_NEAR(0.00462,cent.lat(),0.001);
	EXPECT_NEAR(0.08377,cent.lon(),0.001);
	EXPECT_NEAR(1524,cent.alt(),0.001);

	EXPECT_NEAR(11236.2036,sPoly1.maxRadius(),0.0001);

	EXPECT_NEAR(11236.2036,sPoly1.boundingCircleRadius(),0.0001);
	EXPECT_NEAR(11236.2036,sPoly1.apBoundingRadius(),0.0001);

	for (int i = 0; i< sPoly1.size(); i++) {
		double dfe_i = sPoly1.distanceFromEdge(sPoly1.getVertex(i));
		EXPECT_NEAR(0.0,dfe_i,0.0000001);
	}
	EXPECT_NEAR(1666.757689,sPoly1.distanceFromEdge(avgpt),0.00001);
	EXPECT_NEAR(1666.757689,sPoly1.distanceFromEdge(cent),0.00001);
	Position p4(LatLonAlt::make(0.24,  4.9, 7000));
	double dfe4 = sPoly1.distanceFromEdge(p4);
	EXPECT_NEAR(1111.2,dfe4,0.0001);

	EXPECT_NEAR(0.0,sPoly1.distanceV(avgpt),0.00001);
	EXPECT_NEAR(0.0,sPoly1.distanceV(cent),0.00001);
	double dfeV = sPoly1.distanceV(p4);
	EXPECT_NEAR(0.0,dfeV,0.0001);

	Position avgP = sPoly1.averagePoint();
	EXPECT_NEAR(0.00462,avgP.lat(),0.001);
	EXPECT_NEAR(0.08377,avgP.lon(),0.001);
	EXPECT_NEAR(1524,avgP.alt(),0.001);

	EXPECT_NEAR(Units::from("ft",10000),sPoly1.getTop(),0.001);
	EXPECT_NEAR(0.0,sPoly1.getBottom(),0.001);

	Plan lpc("lpc");
	NavPoint np1 = NavPoint::makeLatLonAlt(0.258,  4.658, 5000, 100.0);
	lpc.addNavPoint(np1);
	NavPoint np2 = NavPoint::makeLatLonAlt(0.267,  4.749, 5000, 200.0);
	lpc.addNavPoint(np2);
	NavPoint np3 = NavPoint::makeLatLonAlt(0.272,  4.788, 7000, 300.0);
	lpc.addNavPoint(np3);
	NavPoint np4 = NavPoint::makeLatLonAlt(0.282,  4.805, 7000, 400.0);
	lpc.addNavPoint(np4);
	NavPoint np5 = NavPoint::makeLatLonAlt(0.280,  4.899, 7000, 500.0);
	lpc.addNavPoint(np5);
	NavPoint np6 = NavPoint::makeLatLonAlt(0.248,  4.897, 7000, 600.0);
	lpc.addNavPoint(np6);

	//DebugSupport.dumpPlanAndPoly(lpc,sPoly1,"test0");
	EXPECT_FALSE(sPoly1.contains(np1.position()));
	EXPECT_TRUE(sPoly1.contains(np2.position()));
	EXPECT_TRUE(sPoly1.contains(np3.position()));
	EXPECT_FALSE(sPoly1.contains(np4.position()));
	EXPECT_TRUE(sPoly1.contains(np5.position()));
	EXPECT_FALSE(sPoly1.contains(np6.position()));

	EXPECT_TRUE(sPoly1.contains2D(np5.position().mkZ(1E12)));

	BoundingRectangle br = sPoly1.getBoundingRectangle();
	EXPECT_NEAR(0.085521,br.getMaxX(),0.0001);
	EXPECT_NEAR(0.00488,br.getMaxY(),0.0001);
	EXPECT_NEAR(0.08203,br.getMinX(),0.0001);
	EXPECT_NEAR(0.00436,br.getMinY(),0.0001);

	Velocity v = Velocity::makeTrkGsVs(45.0,500.0,1000);
	double t = 404;
	SimplePoly linPoly = sPoly1.linear(v,t);
	//DebugSupport.dumpAsUnitTest(linPoly);
	//fpln(" $$ linPoly = "+linPoly);
	EXPECT_EQ(4,linPoly.size());
	EXPECT_NEAR(0.0159,linPoly.getVertex(0).lat(),0.0001);
	EXPECT_NEAR(0.0936,linPoly.getVertex(0).lon(),0.0001);
	EXPECT_NEAR(0.0164,linPoly.getVertex(1).lat(),0.0001);
	EXPECT_NEAR(0.0936,linPoly.getVertex(1).lon(),0.0001);
	EXPECT_NEAR(0.0164,linPoly.getVertex(2).lat(),0.0001);
	EXPECT_NEAR(0.0971,linPoly.getVertex(2).lon(),0.0001);
	EXPECT_NEAR(0.0159,linPoly.getVertex(3).lat(),0.0001);
	EXPECT_NEAR(0.0971,linPoly.getVertex(3).lon(),0.0001);

	EXPECT_NEAR(4.7125,linPoly.perpSide(0),0.001);
	EXPECT_NEAR(0.0,linPoly.perpSide(1),0.0001);
	EXPECT_NEAR(M_PI/2,linPoly.perpSide(2),0.001);
	EXPECT_NEAR(M_PI,linPoly.perpSide(3),0.0001);
	// ------------------ Translate -------------
	SimplePoly sPolySave = sPoly1.copy();
	Vect3 off(0.0,0.0,0.0);
	//fpln(" $$ test0: sPoly1 = "+sPoly1);
	EXPECT_TRUE(sPoly1.isLatLon());
	sPoly1.translate(off);
	//fpln(" $$ test0: sPoly1 = "+sPoly1);
	//EXPECT_TRUE(sPoly1.isLatLon());             // TODO:  put this back
	//EXPECT_TRUE(sPoly1.equals(sPolySave));      // TODO:  put this back

	off = Vect3(333.33,  7777.77, 99.99);
	sPoly1.translate(off);
	//fpln(" $$ test0: sPoly1 = "+sPoly1);

	//PolyUtil::dumpSimplePolyAsUnitTest(sPoly1,2,true);
	SimplePoly sPoly2 = SimplePoly(99.99,3147.99);
	Position p200  = Position::makeXYZ(-5.81995604, 3.29968405, 328.05118110);  sPoly2.add(p200);
	Position p201  = Position::makeXYZ(-5.81994151, 5.09968403, 328.05118110);  sPoly2.add(p201);
	Position p202  = Position::makeXYZ(6.17990911, 5.09968403, 328.05118110);   sPoly2.add(p202);
	Position p203  = Position::makeXYZ(6.17992364, 3.29968405, 328.05118110);   sPoly2.add(p203);
	EXPECT_EQ(4,sPoly2.size());
	EXPECT_NEAR(sPoly2.getVertex(0).lat(),6111.0149,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(0).lon(),-10778.5586,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(0).alt(),99.9900,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(1).lat(),9444.6148,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(1).lon(),-10778.5317,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(1).alt(),99.9900,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(2).lat(),9444.6148,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(2).lon(),11445.1917,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(2).alt(),99.9900,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(3).lat(),6111.0149,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(3).lon(),11445.2186,0.0001);
	EXPECT_NEAR(sPoly2.getVertex(3).alt(),99.9900,0.0001);
}


TEST_F(SimplePolyTest, test1) {
	SimplePoly sPoly1 = SimplePoly(0.0,10668.0);
	Position p100  = Position::makeLatLonAlt(42.88476049, -100.14131084, 0.00000000); sPoly1.add(p100);
	Position p101  = Position::makeLatLonAlt(42.86676149, -100.10377681, 0.00000000); sPoly1.add(p101);
	Position p109  = Position::makeLatLonAlt(42.87965521, -100.16944539, 0.00000000); sPoly1.add(p109);
	//DebugSupport.dumpPoly(sPoly1,"test1_poly");
	Plan dpc("debug");
	for (int i = 0; i < sPoly1.size(); i++) {
		Position vert_i = sPoly1.getVertex(i);
		double epsilon = 0.0001;
		Position vertex = PolyUtil::pushOut(sPoly1, i, -epsilon);
		//double dist = vert_i.distanceH(vertex);
		dpc.addNavPoint(NavPoint(vertex,i*100.0));
		//fpln(" $$$ i = "+i+" vertex = "+vertex+" dist = "+dist);
		//DebugSupport.dumpPlan(dpc,"test1_dpc");
		EXPECT_TRUE(sPoly1.contains(vertex));
	}
}



TEST_F(SimplePolyTest, test_contains) {
	SimplePoly sPoly1 = SimplePoly(0.0,10668.0);
	Position p100  = Position::makeLatLonAlt(42.88476049, -100.14131084, 0.00000000); sPoly1.add(p100);
	Position p101  = Position::makeLatLonAlt(42.86676149, -100.10377681, 0.00000000); sPoly1.add(p101);
	Position p102  = Position::makeLatLonAlt(42.74076550, -99.99120737, 0.00000000);  sPoly1.add(p102);
	Position p103  = Position::makeLatLonAlt(42.73580900, -100.01845900, 0.00000000); sPoly1.add(p103);
	Position p104  = Position::makeLatLonAlt(42.73566021, -100.01927648, 0.00000000); sPoly1.add(p104);
	Position p105  = Position::makeLatLonAlt(42.75365921, -100.16944372, 0.00000000); sPoly1.add(p105);
	Position p106  = Position::makeLatLonAlt(42.78065821, -100.18195808, 0.00000000); sPoly1.add(p106);
	Position p107  = Position::makeLatLonAlt(42.83465721, -100.20698579, 0.00000000); sPoly1.add(p107);
	Position p108  = Position::makeLatLonAlt(42.86165621, -100.20698615, 0.00000000); sPoly1.add(p108);
	Position p109  = Position::makeLatLonAlt(42.87965521, -100.16944539, 0.00000000); sPoly1.add(p109);
	//DebugSupport.dumpPoly(sPoly1,"test2_poly");
	Plan dpc("debug");
	for (int i = 0; i < sPoly1.size(); i++) {
		//Position vertex = poly1.get(i);
		double epsilon = 0.0001;
		Position vertex = PolyUtil::pushOut(sPoly1, i, -epsilon);
		dpc.addNavPoint(NavPoint(vertex,i*100.0));
		//fpln(" $$$ i = "+i+" vertex = "+vertex);
		//DebugSupport.dumpPlan(dpc,"test2_dpc");
		EXPECT_TRUE(sPoly1.contains(vertex));
	}
}


TEST_F(SimplePolyTest, testSpeed) {
	double step = 200; // 20 or 200
	long startTimeMs = clock();
	SimplePoly poly = SimplePoly(0.0,10668.0);
	Velocity v0  = Velocity::makeTrkGsVs(60.0,40,0.0);
	Position p0  = Position::makeLatLonAlt(42.6576,  -99.9165, 0.0); poly.add(p0);
	Position p1  = Position::makeLatLonAlt(42.8313, -100.0093, 0.0); poly.add(p1);
	Position p2  = Position::makeLatLonAlt(42.8751, -100.3355, 0.0); poly.add(p2);
	Position p3  = Position::makeLatLonAlt(42.6603, -100.5250, 0.0); poly.add(p3);
	Position p4  = Position::makeLatLonAlt(42.5569, -100.6152, 0.0); poly.add(p4);
	Position p5  = Position::makeLatLonAlt(42.4323, -100.7186, 0.0); poly.add(p5);
	Position p6  = Position::makeLatLonAlt(42.3633, -100.5158, 0.0); poly.add(p6);
	Position p7  = Position::makeLatLonAlt(42.4840, -100.3209, 0.0); poly.add(p7);
	Position p8  = Position::makeLatLonAlt(42.5504, -100.1933, 0.0); poly.add(p8);
	Position p9  = Position::makeLatLonAlt(42.5594, -100.1558, 0.0); poly.add(p9);
	Position p10 = Position::makeLatLonAlt(42.5234, -100.1182, 0.0); poly.add(p10);
	Position p11 = Position::makeLatLonAlt(42.4110, -100.1366, 0.0); poly.add(p11);
	Position p12 = Position::makeLatLonAlt(42.3858,  -99.8051, 0.0); poly.add(p12);
	Position p13 = Position::makeLatLonAlt(42.2161,  -99.6818, 0.0); poly.add(p13);
	Position p14 = Position::makeLatLonAlt(42.5861,  -99.5293, 0.0); poly.add(p14);
	Position p15 = Position::makeLatLonAlt(42.5622,  -99.7561, 0.0); poly.add(p15);
	//fpln(pp.toString());
	Plan own("Aircraft_0");
	Position pA  = Position::makeLatLonAlt(42.89261, -101.01528, 5000.00);
	Position pB  = Position::makeLatLonAlt(42.90706,  -98.85868, 5000.00);
	double startTime = 600;
	NavPoint npA(pA,startTime);   own.addNavPoint(npA);
	NavPoint npB(pB,2000);    	 own.addNavPoint(npB);

	//DebugSupport.dumpPlanAndPolyPath(own,pp,"Example1_pp");
	PolyPath pp = PolyPath("Example1");
	pp.addPolygon(poly,v0,600.0);
	pp.setPathMode(PolyPath::USER_VEL);

	double incr = 0.1;
	double tmOfIntersection = PolyUtil::intersectsPolygon2D(own, pp, startTime, own.getLastTime(), incr);
	//fpln("## SimplePoly.testSpeed: cpu time " + taskTime+" [s]");
	EXPECT_NEAR(1101.80,tmOfIntersection,0.001);
	for (double dt = -500; dt < 1000; dt = dt + step) {
		own.setTime(1,2000+dt);
		tmOfIntersection = PolyUtil::intersectsPolygon2D(own, pp, startTime, own.getLastTime(), incr);
		//fpln(" $$ testSpeed: dt = "+dt);
		EXPECT_TRUE(tmOfIntersection > 0.0);
	}
	double totalExecTime = (clock()-startTimeMs)/CLOCKS_PER_SEC;
	fpln(" ## SimplePoly.testSpeed: for step = "+Fm0(step)+" cpu time  "+Fm2(totalExecTime/1000.0)+" ms");
	//fpln("## SimplePoly.testSpeed: cpu time " + taskTime+" [s]");
}





TEST_F(SimplePolyTest, testSpeed2) {
	double step = 200; // 50.0 or 200.0

	long startTimeMs = clock();

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
	Velocity v0  = Velocity::mkTrkGsVs(12.000, 50.000, 10.000);

	//DebugSupport.dumpPoly(sPoly0,"spd_poly");

	PolyPath pp = PolyPath("Example1");
	pp.addPolygon(sPoly0,v0,600.0);
	pp.setPathMode(PolyPath::USER_VEL);

	Plan own("Aircraft_1");
	Position op0  = Position::makeLatLonAlt(41.38937, -97.09074, 5000.0);
	Position op1  = Position::makeLatLonAlt(41.50263, -95.27465, 5000.0);
	NavPoint np0(op0,0.000000);    	own.addNavPoint(np0);
	NavPoint np1(op1,1000.000000);   own.addNavPoint(np1);
	//DebugSupport.dumpPlanAndPolyPath(own,pp,"testSpeed2_pp");

	double incr = 0.1;
	double tmOfIntersection = PolyUtil::intersectsPolygon2D(own, pp, 0.0, own.getLastTime(), incr);
	//double taskTime = (System.currentTimeMillis() - startTimeMs) / 1000.0;
	//fpln("## SimplePoly.testSpeed2: cpu time " + taskTime+" [s]");
	EXPECT_NEAR(631,tmOfIntersection,0.001);
	for (double dt = -500; dt < 1000; dt = dt + step) {
		own.setTime(1,2000+dt);
		tmOfIntersection = PolyUtil::intersectsPolygon2D(own, pp, 0.0, own.getLastTime(), incr);
		//fpln(" $$ testSpeed: dt = "+dt);
		EXPECT_TRUE(tmOfIntersection > 0.0);
	}
	//taskTime = (System.currentTimeMillis() - startTimeMs) / 1000.0;
	//fpln("## SimplePoly.testSpeed2: cpu time " + taskTime+" [s]");

	Plan own2("Aircraft_1");
	op0  = Position::makeLatLonAlt(41.75867768, -96.60001190, 5000.00000000);
	op1  = Position::makeLatLonAlt(41.15002118, -96.09856194, 5000.00000000);
	np0 = NavPoint(op0,0.000000);    	own2.addNavPoint(np0);
	np1 = NavPoint(op1,2000.000000);    own2.addNavPoint(np1);
	//DebugSupport.dumpPlanAndPolyPath(own2,pp,"testSpeed2_pp2");
	for (double dt = -500; dt < 1000; dt = dt + step) {
		own.setTime(1,2000+dt);
		tmOfIntersection = PolyUtil::intersectsPolygon2D(own, pp, 0.0, own.getLastTime(), incr);
		//fpln(" $$ testSpeed: dt = "+dt);
		EXPECT_TRUE(tmOfIntersection > 0.0);
	}
	double totalExecTime = (clock()-startTimeMs)/CLOCKS_PER_SEC;
	fpln("  SimplePoly.testSpeed2: for step = "+Fm0(step)+" cpu time = "+Fm2(totalExecTime/1000.0)+" ms");

	fpln("## SimplePoly.testSpeed2: DONE");
}


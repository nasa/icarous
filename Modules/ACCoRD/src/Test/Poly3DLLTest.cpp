/* Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include <cmath>
#include <gtest/gtest.h>
#include "Poly3DLL.h"
#include "format.h"

using namespace larcfm;

class Poly3DLLTest : public ::testing::Test {

public:
	double lat[4]; // = double[4];
	double lon[4]; // = double[4];
	std::vector<LatLonAlt> vertices; // = new ArrayList<LatLonAlt>(4);


protected:
	virtual void SetUp() {

		lat[0] = 0.0;
		lat[1] = 0.5;
		lat[2] = 1.0;
		lat[3] = 0.5;
		lon[0] = 0;
		lon[1] = 1.0;
		lon[2] = 0;
		lon[3] = -0.5;
		for (int j = 0; j < 4; j++) {
			LatLonAlt v = LatLonAlt::mk(lat[j],lon[j],0.0);
			vertices.push_back(v);
		}
	}
};


TEST_F(Poly3DLLTest, test0) {
	Poly3DLL aPoly; // = new Poly3DLL();
	aPoly.add(LatLonAlt::mk(0.0,0.0,0.0));
	EXPECT_NEAR(0.0,aPoly.centroid().lat(),0.00001);
	EXPECT_NEAR(0.0,aPoly.centroid().lon(),0.00001);
	aPoly.add(LatLonAlt::mk(0.0,0.2,0.0));
	EXPECT_NEAR(0.0,aPoly.centroid().lat(),0.00001);
	EXPECT_NEAR(0.1,aPoly.centroid().lon(),0.00001);
	aPoly.add(LatLonAlt::mk(0.2,0.2,0.0));
	aPoly.add(LatLonAlt::mk(0.2,0.0,0.0));
	//fpln(" $$$ test0: aPoly = "+aPoly);
	//EXPECT_NEAR(-0.4,aPoly.area(),0.00001);
	//EXPECT_NEAR(-0.04,aPoly.signedArea(),0.00001);
	EXPECT_NEAR(0.1,aPoly.centroid().lat(),0.00001);
	EXPECT_NEAR(0.1,aPoly.centroid().lon(),0.00001);

	EXPECT_EQ(4,aPoly.size());
	//fpln(" ## test0: p0 = "+p0);
	EXPECT_NEAR(0.0,aPoly.getTop(),0.001);
	EXPECT_NEAR(0.0,aPoly.getBottom(),0.001);
	aPoly.setTop(100.0);
	aPoly.setBottom(20.0);
	EXPECT_NEAR(100.0,aPoly.getTop(),0.001);
	EXPECT_NEAR(20.0,aPoly.getBottom(),0.001);
	EXPECT_NEAR(0.1,aPoly.centroid().lat(),0.00001);
	EXPECT_NEAR(0.1,aPoly.centroid().lon(),0.00001);
}






TEST_F(Poly3DLLTest, test2) {
	Poly3DLL apoly = Poly3DLL(vertices,0.0,1E20);
	//SimplePoly sPoly = SimplePoly.make(apoly);
	//DebugSupport.dumpPoly(sPoly,"test1");
	for (double dd = -1; dd < 1.8; dd = dd + 0.1) {
		double lon = 0.2;
		LatLonAlt lla = LatLonAlt::mk(dd,lon,0.0);
		//bool c0 = apoly.contains(lla);
		//fpln(" dd = "+dd+" lla = "+lla+" c0 = "+c0);
		if (dd > 0.19 && dd < 0.9)
			EXPECT_TRUE(apoly.contains(lla));
	}
}


TEST_F(Poly3DLLTest, test3) {
	Poly3DLL p0; // = new Poly3DLL();
	// order of insertion is important
	p0.add(0.0,0.0,0.0);
	p0.add(0.4,0.0,0.0);
	p0.add(0.4,0.4,0.0);
	p0.add(0.0,0.4,0.0);
	EXPECT_NEAR(0.0,p0.get(3).lat(),0.001);
	EXPECT_NEAR(0.4,p0.get(3).lon(),0.001);
	EXPECT_TRUE(p0.contains(LatLonAlt::mk(0.2,0.2,0.0)));
	EXPECT_FALSE(p0.contains(LatLonAlt::mk(0.4,0.41,0.0)));
	EXPECT_EQ(4,p0.size());
	//fpln(" ## test2: p0 = "+p0);
	EXPECT_NEAR(6.386E12,p0.area(),1E9);
	EXPECT_NEAR(0.2,p0.centroid().lat(),0.001);
	EXPECT_NEAR(0.2,p0.centroid().lon(),0.001);
	Poly3DLL p1; // = new Poly3DLL();
	//fpln(" ## test2: p1 = "+p1);
	p1.add(0.0,0.0,0.0);
	p1.add(0.0,0.4,0.0);
	EXPECT_NEAR(0.0,p1.area(),0.001);
	p1.add(0.4,0.4,0.0);
	EXPECT_EQ(3,p1.size());
	EXPECT_NEAR(3.329E12,p1.area(),1E9);                // negative area
	EXPECT_NEAR(.1333333,p1.centroid().lat(),0.001);
	EXPECT_NEAR(.2666666,p1.centroid().lon(),0.001);
	EXPECT_TRUE(p1.contains(LatLonAlt::mk(0.2,0.30,0.0)));
	EXPECT_FALSE(p1.contains(LatLonAlt::mk(0.4,0.10,0.0)));
}



TEST_F(Poly3DLLTest, test_contains_CC) {
	Poly3DLL p0; // = new Poly3DLL();
	// order of insertion is important
	//		|------------------------------------------|      0.10
	//		|                                          |
	//		|                                          |
	//		|           |--------------------|         |      0.08
	//		|           |                    |         |
	//		|           |                    |         |
	//		|           |                    |         |
	//		|           |                    |         |
	//	. . |  . . . .  |    . . . . . . .   |  .  .   |  .   0.01
	//		|           |                    |         |
	//		|           |                    |         |
	//		|-----------|                    |---------|	 -0.02
	//		0.03        0.07                 .12       .20
	p0.add(0.03,-0.02,0.0);
	p0.add(0.03,0.10,0.0);
	p0.add(0.20,0.10,0.0);
	p0.add(0.20,-0.02,0.0);
	p0.add(0.12,-0.02,0.0);
	p0.add(0.12,0.08,0.0);
	p0.add(0.07,0.08,0.0);
	p0.add(0.07,-0.02,0.0);
	for (double xval = 0; xval < 0.3; xval = xval + 0.01) {
		LatLonAlt nv = LatLonAlt::mk(xval,0.01,0.0);
		bool cntns = p0.contains(nv);
		//fpln(" test1: xval = "+xval+" cntns = "+cntns);
		if ((0.03 < xval && xval <= 0.07) || (0.12 < xval && xval <= 0.2000001)) {      // < 20 does not work
			EXPECT_TRUE(cntns);
		} else {
			EXPECT_FALSE(cntns);
		}
		double xvalPlus = xval+0.00001;
		LatLonAlt nvPlus = LatLonAlt::mk(xvalPlus,0.01,0.0);
		bool cntnsPlus = p0.contains(nvPlus);
		//fpln(" test1: xvalPlus = "+xvalPlus+" cntnsPlus = "+cntnsPlus);
		if ((0.04 < xvalPlus && xvalPlus <= 0.075) || (0.125 < xvalPlus && xvalPlus <= 0.2001)) {
			EXPECT_TRUE(cntnsPlus);
		} else {
			EXPECT_FALSE(cntnsPlus);
		}
		double xvalMinus = xval-0.00001;
		LatLonAlt nvMinus = LatLonAlt::mk(xvalMinus,0.01,0.0);
		bool cntnsMinus = p0.contains(nvMinus);
		//fpln(" test1: xvalMinus = "+xvalMinus+" cntnsMinus = "+cntnsMinus);
		if ((0.03 < xvalMinus && xvalMinus <= 0.07) || (0.12 < xvalMinus && xvalMinus <= .20)) {
			EXPECT_TRUE(cntnsMinus);
		} else {
			EXPECT_FALSE(cntnsMinus);
		}
	}
}










/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "BoundingRectangle.h"
#include "Plan.h"
#include "GreatCircle.h"
#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class BoundingRectangleTest : public ::testing::Test {

public:

	double x[4];
	double y[4];

	std::vector<Vect2> vertices;

protected:
	virtual void SetUp() {
		x[0] = 0;
		x[1] = 5;
		x[2] = 10;
		x[3] = 5;
		y[0] = 0;
		y[1] = 10;
		y[2] = 0;
		y[3] = -5;
		vertices.resize(4);
		for (int j = 0; j < 4; j++) {
			Vect2 v(x[j],y[j]);
			vertices[j] = v;
		}
	}
};

LatLonAlt mkll(double lat,double lon) {
    return LatLonAlt::mk(lat,lon,-MAXDOUBLE);
}


TEST_F(BoundingRectangleTest, testBoundingRectangleLL) {
	BoundingRectangle bbox(vertices);
	//BoundingRectangle bbox = BoundingRectangle(x,y);
	//fpln(" $$ bbox = "+bbox);
	EXPECT_NEAR(0.0,bbox.getMinX(),0.0001);
	EXPECT_NEAR(10.0,bbox.getMaxX(),0.0001);
	EXPECT_NEAR(-5.0,bbox.getMinY(),0.0001);
	EXPECT_NEAR(10.0,bbox.getMaxY(),0.0001);
	EXPECT_TRUE(bbox.contains(5.0,0.0));
	EXPECT_TRUE(bbox.contains(0.0,0.0));
	EXPECT_FALSE(bbox.contains(11.0,0.0));
	EXPECT_FALSE(bbox.contains(5.0,-6.0));
	EXPECT_TRUE(bbox.contains(4.0,-4.0));
	EXPECT_TRUE(bbox.contains(3.0,-4.0));
	EXPECT_TRUE(bbox.contains(5.0,0.0));
	EXPECT_TRUE(bbox.contains(0.0,0.0));
	EXPECT_TRUE(bbox.contains(0.00001,0.0));
	EXPECT_FALSE(bbox.contains(11.0,0.0));
	EXPECT_FALSE(bbox.contains(5.0,-6.0));
	EXPECT_TRUE(bbox.contains(4.0,-4.0));
	EXPECT_TRUE(bbox.contains(4.0001,-4.0));
	EXPECT_TRUE(bbox.contains(7.5,2.5));
	EXPECT_TRUE(bbox.contains(10.0,-4.0));
	EXPECT_FALSE(bbox.contains(10.000001,-4.0));
}


TEST_F(BoundingRectangleTest, testBoundingRectangle2) {
	BoundingRectangle bbox; // = BoundingRectangle();
	bbox.add(0,-5);
	bbox.add(10,10);

	EXPECT_NEAR(0.0,bbox.getMinX(),0.0001);
	EXPECT_NEAR(10.0,bbox.getMaxX(),0.0001);
	EXPECT_NEAR(-5.0,bbox.getMinY(),0.0001);
	EXPECT_NEAR(10.0,bbox.getMaxY(),0.0001);
	EXPECT_TRUE(bbox.contains(5.0,0.0));
	EXPECT_TRUE(bbox.contains(0.0,0.0));
	EXPECT_FALSE(bbox.contains(11.0,0.0));
	EXPECT_FALSE(bbox.contains(5.0,-6.0));
	EXPECT_TRUE(bbox.contains(4.0,-4.0));
	EXPECT_TRUE(bbox.contains(3.0,-4.0));
	EXPECT_TRUE(bbox.contains(5.0,0.0));
	EXPECT_TRUE(bbox.contains(0.0,0.0));
	EXPECT_TRUE(bbox.contains(0.00001,0.0));
	EXPECT_FALSE(bbox.contains(11.0,0.0));
	EXPECT_FALSE(bbox.contains(5.0,-6.0));
	EXPECT_TRUE(bbox.contains(4.0,-4.0));
	EXPECT_TRUE(bbox.contains(4.0001,-4.0));
	EXPECT_TRUE(bbox.contains(7.5,2.5));
	EXPECT_TRUE(bbox.contains(10.0,-4.0));
	EXPECT_FALSE(bbox.contains(10.000001,-4.0));
}


TEST_F(BoundingRectangleTest, testBoundingRectangleLLA) {

	// case 1: wrap around, with start on negative longitude

	BoundingRectangle bbox; // = BoundingRectangle();
	LatLonAlt p1 = LatLonAlt::make(10, -150, 10000);
	LatLonAlt p2 = LatLonAlt::make( 5,  170, 10000);
	LatLonAlt p3 = LatLonAlt::make( 7,  175, 10000);
	LatLonAlt p4 = LatLonAlt::make( 7, -160, 10000);
	LatLonAlt p5 = LatLonAlt::make( 7, -140, 10000);
	LatLonAlt p6 = LatLonAlt::make( 7, -200, 10000);
	bbox.add(p2);
	bbox.add(p1);

	EXPECT_NEAR(170.0,Units::to("deg",bbox.getMinX()),0.0001);
	EXPECT_NEAR(210.0,Units::to("deg",bbox.getMaxX()),0.0001);
	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinY()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxY()),0.0001);

	EXPECT_TRUE(bbox.contains(p3));
	EXPECT_TRUE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_FALSE(bbox.contains(p6));

	// case 2: wrap around, with start on positive longitude

	bbox = BoundingRectangle();
	bbox.add(p1);
	bbox.add(p2);

	EXPECT_NEAR(-190.0,Units::to("deg",bbox.getMinX()),0.0001);
	EXPECT_NEAR(-150.0,Units::to("deg",bbox.getMaxX()),0.0001);
	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinY()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxY()),0.0001);

	EXPECT_TRUE(bbox.contains(p3));
	EXPECT_TRUE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_FALSE(bbox.contains(p6));

	// case 3: negative latitude

	LatLonAlt p2b = LatLonAlt::make( -5,  170, 10000);
	LatLonAlt p3b = LatLonAlt::make( -2,  175, 10000);
	LatLonAlt p4b = LatLonAlt::make( -2, -160, 10000);
	LatLonAlt p5b = LatLonAlt::make( -2, -140, 10000);
	LatLonAlt p6b = LatLonAlt::make( -2, -200, 10000);
	bbox = BoundingRectangle();
	bbox.add(p1);
	bbox.add(p2b);

	EXPECT_NEAR(-190.0,Units::to("deg",bbox.getMinX()),0.0001);
	EXPECT_NEAR(-150.0,Units::to("deg",bbox.getMaxX()),0.0001);
	EXPECT_NEAR(-5.0,Units::to("deg",bbox.getMinY()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxY()),0.0001);

	EXPECT_TRUE(bbox.contains(p3b));
	EXPECT_TRUE(bbox.contains(p4b));
	EXPECT_FALSE(bbox.contains(p5b));
	EXPECT_FALSE(bbox.contains(p6b));

	// case 4: nominal

	LatLonAlt p2c = LatLonAlt::make( 5, -170, 10000);
	bbox = BoundingRectangle();
	bbox.add(p1);
	bbox.add(p2c);

	EXPECT_NEAR(-170.0,Units::to("deg",bbox.getMinX()),0.0001);
	EXPECT_NEAR(-150.0,Units::to("deg",bbox.getMaxX()),0.0001);
	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinY()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxY()),0.0001);

	EXPECT_FALSE(bbox.contains(p3));
	EXPECT_TRUE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_FALSE(bbox.contains(p6));

	// case 4: wraparound pole

	LatLonAlt p1d = LatLonAlt::make( 95, 30, 10000);  // really (85, -150, 10000)
	LatLonAlt p7 = LatLonAlt::make( 80, -160, 10000);
	bbox = BoundingRectangle();
	bbox.add(p2c);
	bbox.add(p1d);

	EXPECT_NEAR(-170.0,Units::to("deg",bbox.getMinX()),0.0001);
	EXPECT_NEAR(-150.0,Units::to("deg",bbox.getMaxX()),0.0001);
	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinY()),0.0001);
	EXPECT_NEAR(85.0,Units::to("deg",bbox.getMaxY()),0.0001);

	EXPECT_FALSE(bbox.contains(p3));
	EXPECT_TRUE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_FALSE(bbox.contains(p6));
	EXPECT_TRUE(bbox.contains(p7));

}



TEST_F(BoundingRectangleTest, testIntersection) {
	BoundingRectangle bbox1; // = BoundingRectangle();
	bbox1.add(0,-5);
	bbox1.add(10,10);

	// case 1
	BoundingRectangle bbox2; // = BoundingRectangle();
	bbox2.add(11,-5);
	bbox2.add(12,10);

	EXPECT_FALSE(bbox1.intersects(bbox2));
	EXPECT_TRUE(bbox1.intersects(bbox2,2.0));

	// case 2
	BoundingRectangle bbox3; // = BoundingRectangle();
	bbox3.add(-5,-5);
	bbox3.add(-1,10);

	EXPECT_FALSE(bbox1.intersects(bbox3));
	EXPECT_TRUE(bbox1.intersects(bbox3,2.0));

	// case 3
	BoundingRectangle bbox4; // = BoundingRectangle();
	bbox4.add(0,11);
	bbox4.add(10,15);

	EXPECT_FALSE(bbox1.intersects(bbox4));
	EXPECT_TRUE(bbox1.intersects(bbox4,2.0));

	// case 4
	BoundingRectangle bbox5; // = BoundingRectangle();
	bbox5.add(0,-10);
	bbox5.add(10,-6);

	EXPECT_FALSE(bbox1.intersects(bbox5));
	EXPECT_TRUE(bbox1.intersects(bbox5,2.0));

}

TEST_F(BoundingRectangleTest, testCenter) {
	BoundingRectangle bbox1; // = BoundingRectangle();
	bbox1.add(0,-5);
	bbox1.add(10,10);

	EXPECT_NEAR(5, bbox1.centerVect().x, 0.0001);
	EXPECT_NEAR(2.5, bbox1.centerVect().y, 0.0001);

	EXPECT_FALSE(bbox1.centerPos().isLatLon());

	// case 1
	BoundingRectangle bbox2;
	bbox2.add(11,5);
	bbox2.add(12,-10);

	EXPECT_NEAR(11.5, bbox2.centerVect().x, 0.0001);
	EXPECT_NEAR(-2.5, bbox2.centerVect().y, 0.0001);

	EXPECT_FALSE(bbox2.centerPos().isLatLon());

	BoundingRectangle bbox3;
	bbox3.add(LatLonAlt::make(20, -30, 0.0));
	bbox3.add(LatLonAlt::make(30, -35, 0.0));

	Position pos = Position(LatLonAlt::make(25.0,-32.5, 0.0));

	EXPECT_NEAR(Units::from("deg", -32.5), bbox3.centerVect().x, 0.0001);
	EXPECT_NEAR(Units::from("deg", 25.0), bbox3.centerVect().y, 0.0001);

	EXPECT_TRUE(bbox3.centerPos().isLatLon());

	EXPECT_NEAR(pos.lat(), bbox3.centerPos().lat(), 0.0001);
	EXPECT_NEAR(pos.lon(), bbox3.centerPos().lon(), 0.0001);

	BoundingRectangle bbox4;
	bbox4.add(LatLonAlt::make(-20, -170, 0.0));
	bbox4.add(LatLonAlt::make(30, 150, 0.0));

	pos = Position(LatLonAlt::make(5, 170, 0));

	EXPECT_NEAR(Units::from("deg", -190.0), bbox4.centerVect().x, 0.0001); // not normalized
	EXPECT_NEAR(Units::from("deg", 5.0), bbox4.centerVect().y, 0.0001);

	EXPECT_TRUE(bbox4.centerPos().isLatLon());

	EXPECT_NEAR(pos.lat(), bbox4.centerPos().lat(), 0.0001);
	EXPECT_NEAR(pos.lon(), bbox4.centerPos().lon(), 0.0001);

}







TEST_F(BoundingRectangleTest, testBoundingRectangle) {
	double lat[4]; // = double[4];
	double lon[4]; // = double[4];
	std::vector<LatLonAlt> vertices; // = new std::vector<LatLonAlt>(4);
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
	BoundingRectangle bbox = BoundingRectangle::makeUnconnected(vertices);


	//Plan bbPlan = bbox.toPlan();
	//NavPoint npExtra = NavPoint::mkLatLonAlt(0.3,-0.4,0,1000.0); bbPlan.addNavPoint(npExtra);

	//DebugSupport.dumpPlan(bbPlan,"testBoundingRectangle");
	//fpln(" $$$ testBoundingRectangle: bbox = "+bbox.toStringRadians());
	//fpln(" $$$ testBoundingRectangle: bbox = "+bbox.toStringLL("rad"));

	//BoundingRectangle bbox = BoundingRectangle(x,y);
	//fpln(" $$ bbox = "+bbox);

	EXPECT_NEAR(0.0,bbox.getMinLat(),0.0001);  //TODO: PUT BACK
	EXPECT_NEAR(1.0,bbox.getMaxLat(),0.0001);
	EXPECT_NEAR(-0.5,bbox.getMinLon(),0.0001);
	EXPECT_NEAR(1.0,bbox.getMaxLon(),0.0001);
	EXPECT_TRUE(bbox.contains(mkll(0.5,0.0)));
	EXPECT_TRUE(bbox.contains(mkll(0.0,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(1.10,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(0.5,-0.6)));
	EXPECT_TRUE(bbox.contains(mkll(0.4,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.3,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.5,0.0)));
	EXPECT_TRUE(bbox.contains(mkll(0.0,0.0)));
	EXPECT_TRUE(bbox.contains(mkll(0.00001,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(11.0,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(0.5,-0.6)));
	EXPECT_TRUE(bbox.contains(mkll(0.4,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.4001,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.75,0.25)));
	EXPECT_TRUE(bbox.contains(mkll(1.0,-0.4)));
	EXPECT_FALSE(bbox.contains(mkll(1.000001,-0.4)));
}


TEST_F(BoundingRectangleTest, test_hugeLL2) {
	BoundingRectangle bbox; // = new BoundingRectangle();
	//		bbox.add(0,-5);
	//		bbox.add(10,10);

	bbox.add(mkll(0,-.5), mkll(1.0, 1.0));
	//Plan bbPlan = bbox.toPlan();
	//NavPoint npExtra = NavPoint::mkLatLonAlt(0.3,-0.4,0,1000.0); bbPlan.addNavPoint(npExtra);

	//DebugSupport.dumpPlan(bbPlan,"test_hugeLL2");
	//fpln(" $$$ test_hugeLL2: bbox = "+bbox.toStringRadians());
	//fpln(" $$$ test_hugeLL2: bbox = "+bbox.toStringLL("rad"));

	EXPECT_NEAR(0.0,bbox.getMinLat(),0.0001);
	EXPECT_NEAR(1.0,bbox.getMaxLat(),0.0001);
	EXPECT_NEAR(-0.5,bbox.getMinLon(),0.0001);
	EXPECT_NEAR(1.0,bbox.getMaxLon(),0.0001);
	EXPECT_TRUE(bbox.contains(mkll(0.5,0.0)));
	EXPECT_TRUE(bbox.contains(mkll(0.0,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(1.1,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(0.5,-0.50001)));
	EXPECT_TRUE(bbox.contains(mkll(0.4,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.3,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.5,0.0)));
	EXPECT_TRUE(bbox.contains(mkll(0.0,0.0)));
	EXPECT_TRUE(bbox.contains(mkll(0.00001,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(11.0,0.0)));
	EXPECT_FALSE(bbox.contains(mkll(0.5,-0.6)));
	EXPECT_TRUE(bbox.contains(mkll(0.4,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.4001,-0.4)));
	EXPECT_TRUE(bbox.contains(mkll(0.75,0.25)));
	EXPECT_TRUE(bbox.contains(mkll(1.0,-0.4)));
	EXPECT_FALSE(bbox.contains(mkll(1.000001,-0.4)));
}


TEST_F(BoundingRectangleTest, test_LLA) {

	// case 1: wrap around, with start on negative longitude
	BoundingRectangle bbox; // = new BoundingRectangle();
	LatLonAlt p1 = LatLonAlt::make(10, 150, 10000);
	LatLonAlt p2 = LatLonAlt::make( 5,  170, 10000);
	LatLonAlt p3 = LatLonAlt::make( 7,  175, 10000);
	LatLonAlt p4 = LatLonAlt::make( 6, 160, 10000);
	LatLonAlt p5 = LatLonAlt::make( 5.1, 140, 10000);
	LatLonAlt p6 = LatLonAlt::make( 5.0001, 160, 10000);
	LatLonAlt p7 = LatLonAlt::make( 4.9999, 0.0, 10000);
	LatLonAlt p8 = LatLonAlt::make( 5.0001, 169.999, 10000);
	bbox.add(p1,p2);

	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinLat()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxLat()),0.0001);
	EXPECT_NEAR(150,Units::to("deg",bbox.getMinLon()),0.0001);
	EXPECT_NEAR(170,Units::to("deg",bbox.getMaxLon()),0.0001);
	EXPECT_FALSE(bbox.contains(p3));
	EXPECT_TRUE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_TRUE(bbox.contains(p6));
	EXPECT_FALSE(bbox.contains(p7));
	EXPECT_TRUE(bbox.contains(p8));


	// case 2: wrap around, with start on positive longitude

	bbox = BoundingRectangle();
	bbox.add(p2,p3);

	//Plan bbPlan = bbox.toPlan();
	//NavPoint npExtra(Position(p3),0.0); bbPlan.addNavPoint(npExtra);
	//DebugSupport.dumpPlan(bbPlan,"test_LLA");
	//fpln(" $$$ test_LLA: bbox = "+bbox.toStringRadians());
	//fpln(" $$$ test_LLA: bbox = "+bbox.toString());

	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinLat()),0.0001);
	EXPECT_NEAR(7.0,Units::to("deg",bbox.getMaxLat()),0.0001);
	EXPECT_NEAR(170.0,Units::to("deg",bbox.getMinLon()),0.0001);
	EXPECT_NEAR(175.0,Units::to("deg",bbox.getMaxLon()),0.0001);

	EXPECT_TRUE(bbox.contains(p2));
	EXPECT_TRUE(bbox.contains(p3));
	EXPECT_FALSE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_FALSE(bbox.contains(p6));

	// case 3: negative latitude

	LatLonAlt p2b = LatLonAlt::make( -5,  170, 10000);
	LatLonAlt p3b = LatLonAlt::make( -4.99,  165, 10000);
	LatLonAlt p4b = LatLonAlt::make( -3, 130, 10000);
	LatLonAlt p5b = LatLonAlt::make( -2, 140, 10000);
	LatLonAlt p6b = LatLonAlt::make( -2, 200, 10000);
	bbox = BoundingRectangle();
	bbox.add(p1,p2b);
	//fpln(" $$$ test_LLA: bbox = "+bbox.toStringLL("deg"));

	EXPECT_NEAR(-5.0,Units::to("deg",bbox.getMinLat()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxLat()),0.0001);
	EXPECT_NEAR(150.0,Units::to("deg",bbox.getMinLon()),0.0001);
	EXPECT_NEAR(170.0,Units::to("deg",bbox.getMaxLon()),0.0001);

	EXPECT_TRUE(bbox.contains(p3b));
	EXPECT_FALSE(bbox.contains(p4b));
	EXPECT_FALSE(bbox.contains(p5b));
	EXPECT_FALSE(bbox.contains(p6b));

	// case 4: nominal

	LatLonAlt p2c = LatLonAlt::make( 5, -170, 10000);
	bbox = BoundingRectangle();
	bbox.add(p1,p2c);

	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinLat()),0.0001);
	EXPECT_NEAR(10.0,Units::to("deg",bbox.getMaxLat()),0.0001);
	EXPECT_NEAR(150,Units::to("deg",bbox.getMinLon()),0.0001);
	EXPECT_NEAR(190,Units::to("deg",bbox.getMaxLon()),0.0001);
	EXPECT_TRUE(bbox.contains(p3));
	EXPECT_TRUE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_TRUE(bbox.contains(p6));

	// case 4: wraparound pole

	LatLonAlt p1d = LatLonAlt::make( 95, 30, 10000);  // really (85, -150, 10000)
	LatLonAlt p9= LatLonAlt::make( 80, -160, 10000);
	LatLonAlt p10= LatLonAlt::make( 83, -151, 10000);
	//fpln(" p10 = "+p10.normalize());
	LatLonAlt p11= LatLonAlt::make( 96, -160, 10000);

	bbox = BoundingRectangle();
	bbox.add(p2c,p1d);


	//fpln("\n $$$ test_hugeLL2: bbox = "+bbox.toString());

	EXPECT_NEAR(5.0,Units::to("deg",bbox.getMinLat()),0.0001);
	EXPECT_NEAR(85.0,Units::to("deg",bbox.getMaxLat()),0.0001);
	EXPECT_NEAR(-170,Units::to("deg",bbox.getMinLon()),0.0001);
	EXPECT_NEAR(-150,Units::to("deg",bbox.getMaxLon()),0.0001);

	EXPECT_FALSE(bbox.contains(p3));
	EXPECT_FALSE(bbox.contains(p4));
	EXPECT_FALSE(bbox.contains(p5));
	EXPECT_FALSE(bbox.contains(p8));
	EXPECT_TRUE(bbox.contains(p9));
	EXPECT_TRUE(bbox.contains(p10));
	EXPECT_FALSE(bbox.contains(p11));

}



TEST_F(BoundingRectangleTest, testIntersectionLL) {
	BoundingRectangle bbox1 =  BoundingRectangle();
	bbox1.add(mkll(0,-0.5),mkll(1.0,1.0));

	// case 1
	BoundingRectangle bbox2 = BoundingRectangle();
	bbox2.add(mkll(1.1,-0.5), mkll(1.2,1.0));
	EXPECT_FALSE(bbox1.intersects(bbox2));
	EXPECT_TRUE(bbox1.intersects(bbox2,2.0));

	// case 2
	BoundingRectangle bbox3 = BoundingRectangle();
	bbox3.add(mkll(-0.5,-0.5), mkll(-1,1.0));
	EXPECT_FALSE(bbox1.intersects(bbox3));
	EXPECT_TRUE(bbox1.intersects(bbox3,2.0));

	// case 3
	BoundingRectangle bbox4 = BoundingRectangle();
	bbox4.add(mkll(0,1.1), mkll(1.0,1.5));
	EXPECT_FALSE(bbox1.intersects(bbox4));
	EXPECT_TRUE(bbox1.intersects(bbox4,2.0));

	// case 4
	BoundingRectangle bbox5 = BoundingRectangle();
	bbox5.add(mkll(0,-1.0), mkll(1.0,-0.6));
	EXPECT_FALSE(bbox1.intersects(bbox5));
	EXPECT_TRUE(bbox1.intersects(bbox5,2.0));
}


TEST_F(BoundingRectangleTest, testCenterLL) {
	BoundingRectangle bbox1 = BoundingRectangle();
	bbox1.add(mkll(0,-0.5), mkll(1.0,1.0));
	EXPECT_NEAR(.5,bbox1.centerPos().lat(),0.0001);
	EXPECT_NEAR(.25,bbox1.centerPos().lon(),0.0001);
	EXPECT_TRUE(bbox1.centerPos().isLatLon());

	// case 1
	BoundingRectangle bbox2 = BoundingRectangle();
	bbox2.add(mkll(1.1,.5), mkll(1.2,-1.0));
	EXPECT_NEAR(1.1808,bbox2.centerPos().lat(),0.0001);
	EXPECT_NEAR(-.25,bbox2.centerPos().lon(),0.0001);
	EXPECT_TRUE(bbox2.centerPos().isLatLon());

	BoundingRectangle bbox3 = BoundingRectangle();
	bbox3.add(LatLonAlt::make(20, -30, 0.0), LatLonAlt::make(30, -35, 0.0));
	Position pos(LatLonAlt::make(25.0,-32.5, 0.0));
	EXPECT_NEAR(Units::from("deg", -32.5),bbox3.centerPos().lon(),0.0001);
	EXPECT_NEAR(Units::from("deg", 25.0),bbox3.centerPos().lat(),0.0001);
	EXPECT_TRUE(bbox3.centerPos().isLatLon());
	EXPECT_NEAR(pos.lat(),bbox3.centerPos().lat(),0.0001);
	EXPECT_NEAR(pos.lon(),bbox3.centerPos().lon(),0.0001);

	BoundingRectangle bbox4 = BoundingRectangle();
	bbox4.add(LatLonAlt::make(-20, -170, 0.0), LatLonAlt::make(30, 150, 0.0));

	pos = Position(LatLonAlt::make(5, 170, 0));
	EXPECT_NEAR(2.96706,bbox4.centerPos().lon(),0.0001); // not normalized
	EXPECT_NEAR(Units::from("deg", 5.0),bbox4.centerPos().lat(),0.0001);
	EXPECT_TRUE(bbox4.centerPos().isLatLon());
	EXPECT_NEAR(pos.lat(),bbox4.centerPos().lat(),0.0001);
	//EXPECT_NEAR(pos.lon(),bbox4.centerPos().lon(),0.0001);
}


TEST_F(BoundingRectangleTest, test_maxLat) {
	//double lat1 = Units::from("deg",41.000); {
	for (double lat1 = Units::from("deg",-80.0); lat1 <= Units::from("deg",80.0); lat1 = lat1 + Units::from("deg",5.0)) {
		double lat2 = lat1+ 0.01;
		double lon1 = Units::from("deg",120.45);
		double lon2 = Units::from("deg",125.97);
		double maxLati = GreatCircle::max_latitude(lat1,lon1,lat2,lon2);
		double minLati = GreatCircle::min_latitude(lat1,lon1,lat2,lon2);
		//fpln(" $$$ maxLati = "+Units::str("deg",maxLati));
		//fpln(" $$$ minLati = "+Units::str("deg",minLati));
		LatLonAlt lla1 = LatLonAlt::mk(lat1,lon1,0.0);
		LatLonAlt lla2 = LatLonAlt::mk(lat2,lon2,0.0);
		for (double ff = 0.0; ff <= 1.0; ff = ff+0.1) {
			double lla_ff_lat = GreatCircle::interpolate(lla1,lla2,ff).lat();
			if (lla_ff_lat > lat2+1E-14) {
				//f.p(" $$$ for ff = "+Fm1(ff)+" lla_f_lat = "+Units::str("deg",lla_ff_lat,8));
				//f.p(" $$$ maxLati = "+Units::str("deg",maxLati,8));
				//fpln(" $$$ minLati = "+Units::str("deg",minLati,8));
			}
			EXPECT_TRUE(lla_ff_lat <= maxLati+1E-14);
			EXPECT_TRUE(lla_ff_lat >= minLati-1E-14);
		}
	}
}



TEST_F(BoundingRectangleTest, test_maxLat_Err) {
	double lat1 = 0.0;
	double lon1 = 0.0;
	double lat2 = Units::from("deg",28.647890);
	double lon2 = Units::from("deg",57.295780);
	//double minLati = GreatCircle::min_latitude(lat1,lon1,lat2,lon2);
	//fpln(" $$$ minLati = "+Units::str("deg",minLati));
	NavPoint np1 = NavPoint::mkLatLonAlt(lat1,lon1,0.0,100.0);
	NavPoint np2 = NavPoint::mkLatLonAlt(lat2,lon2,0.0,200.0);
	Plan pln("err");
	pln.addNavPoint(np1);
	pln.addNavPoint(np2);
	//DebugSupport.dumpPlan(pln,"test_maxLat_Err");
}





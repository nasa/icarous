/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"

#include <cmath>
#include <gtest/gtest.h>
#include "BoundingRectangle.h"
#include "Poly2D.h"

using namespace larcfm;
class Poly2DTest : public ::testing::Test {

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
	
	// TEST_F(Poly2DTest, testBoundingRectangle) {
	// 	BoundingRectangle bbox = BoundingRectangle(vertices);
	// 	//BoundingRectangle bbox = BoundingRectangle(x,y);
	// 	//fpln(" $$ bbox = "+bbox);
	// 	EXPECT_NEAR(0.0,bbox.getMinX(),0.0001);
	// 	EXPECT_NEAR(10.0,bbox.getMaxX(),0.0001);
	// 	EXPECT_NEAR(-5.0,bbox.getMinY(),0.0001);
	// 	EXPECT_NEAR(10.0,bbox.getMaxY(),0.0001);		
	// 	EXPECT_TRUE(bbox.contains(5.0,0.0));
	// 	EXPECT_TRUE(bbox.contains(0.0,0.0));
	// 	EXPECT_FALSE(bbox.contains(11.0,0.0));
	// 	EXPECT_FALSE(bbox.contains(5.0,-6.0));
	// 	EXPECT_TRUE(bbox.contains(4.0,-4.0));
	// 	EXPECT_TRUE(bbox.contains(3.0,-4.0));	
	// 	//EXPECT_TRUE(bbox.contains(Vect2(2.0,-4.0)));	
	// 	//Corners corners = bbox.computeCorners();
	// 	//fpln(" $$ corners = "+corners);					
	// 	//fpln(" $$ vertices = "+vertices);
	// 	Poly2D apoly = Poly2D(vertices);
	// 	BoundingRectangle bbox2 = BoundingRectangle(apoly.getVertices());
	// 	//BoundingRectangle bbox2 = BoundingRectangle(vertices);
	// 	EXPECT_NEAR(0.0,bbox2.getMinX(),0.0001);
	// 	EXPECT_NEAR(10.0,bbox2.getMaxX(),0.0001);
	// 	EXPECT_NEAR(-5.0,bbox2.getMinY(),0.0001);
	// 	EXPECT_NEAR(10.0,bbox2.getMaxY(),0.0001);		
	// 	EXPECT_TRUE(apoly.contains(5.0,0.0));
	// 	EXPECT_FALSE(apoly.contains(0.00,0.0));
	// 	EXPECT_TRUE(apoly.contains(0.00001,0.0));
	// 	EXPECT_FALSE(apoly.contains(11.0,0.0));
	// 	EXPECT_FALSE(apoly.contains(5.0,-6.0));
	// 	EXPECT_FALSE(apoly.contains(4.0,-4.0));
	// 	EXPECT_TRUE(apoly.contains(4.0001,-4.0));
	// 	EXPECT_TRUE(apoly.contains(7.5,2.5));		
	// 	//EXPECT_TRUE(apoly.contains(Vect2(4.5,2.5)));
	// 	EXPECT_NEAR(-75.0,apoly.signedArea(),0.001);
	// 	EXPECT_NEAR(5.0,apoly.centroid().x,0.001);
	// 	EXPECT_NEAR(1.6666667,apoly.centroid().y,0.001);
	// }
	

TEST_F(Poly2DTest, test0) {
	Poly2D aPoly = Poly2D();
	EXPECT_TRUE(ISNAN(aPoly.centroid().x));
	EXPECT_TRUE(ISNAN(aPoly.centroid().y));
	aPoly.add(0,0);
	EXPECT_NEAR(0.0,aPoly.centroid().x,0.00001);
	EXPECT_NEAR(0.0,aPoly.centroid().y,0.00001);
	aPoly.add(0,2);
	EXPECT_NEAR(0.0,aPoly.centroid().x,0.00001);
	EXPECT_NEAR(1.0,aPoly.centroid().y,0.00001);
	aPoly.add(2,2);
	EXPECT_NEAR(0.66666,aPoly.centroid().x,0.00001);
	EXPECT_NEAR(1.33333,aPoly.centroid().y,0.00001);
	aPoly.add(2,0);
	//EXPECT_NEAR(-4.0,aPoly.area(),0.00001);
	EXPECT_NEAR(-4.0,aPoly.signedArea(),0.00001);
	EXPECT_NEAR(1.0,aPoly.centroid().x,0.00001);
	EXPECT_NEAR(1.0,aPoly.centroid().y,0.00001);
}

	
	TEST_F(Poly2DTest, test1) {
		Poly2D apoly = Poly2D(vertices);
		for (double dd = -1; dd < 18; dd = dd + 0.1) {
			//fpln(" dd = "+dd);
			if (dd > 0 && dd < 9.999)
				EXPECT_TRUE(apoly.contains(dd,0.0));
			if (dd > 0.9999999999999999 && dd < 9.09)
				EXPECT_TRUE(apoly.contains(dd,2.0));
			if (dd > 1.91 && dd < 8.09)
				EXPECT_TRUE(apoly.contains(dd,4.0));
		}		
	}


	TEST_F(Poly2DTest, test2) {
		Poly2D p0 = Poly2D();
		// order of insertion is important
		p0.add(0.0,0.0);
		p0.add(4.0,0.0);
		p0.add(4.0,4.0);
		p0.add(0.0,4.0);
		EXPECT_NEAR(0.0,p0.get(3).x,0.001);
		EXPECT_NEAR(4.0,p0.get(3).y,0.001);
		EXPECT_TRUE(p0.contains(Vect2(2.0,2.0)));
		EXPECT_FALSE(p0.contains(Vect2(4.0,4.1)));
		EXPECT_EQ(4,p0.size());
		//f.pln(" p0 = "+p0);
		//EXPECT_NEAR(16.0,p0.area(),0.001);                  // area not yet implemented
		EXPECT_NEAR(2.0,p0.centroid().x,0.001);
		EXPECT_NEAR(2.0,p0.centroid().y,0.001);
		Poly2D p1 = Poly2D();
		//f.pln(" p1 = "+p1);
		p1.add(0.0,0.0);
		p1.add(0.0,4.0);
		//EXPECT_NEAR(0.0,p1.area(),0.001);                // area not yet implemented
		p1.add(4.0,4.0);
		EXPECT_EQ(3,p1.size());
		//EXPECT_NEAR(-8.0,p1.area(),0.001);                // negative area
		EXPECT_NEAR(1.333333,p1.centroid().x,0.001);
		EXPECT_NEAR(2.666666,p1.centroid().y,0.001);
		EXPECT_TRUE(p1.contains(Vect2(2.0,3.0)));
		EXPECT_FALSE(p1.contains(Vect2(4.0,1.0)));
	}

	TEST_F(Poly2DTest, testBoundingRectangle2) {
		std::vector<Vect2> verts(0);
		Vect2 v0 = Vect2(197193.50258148066, 34260.90753773977);     verts.push_back(v0);
		Vect2 v1 = Vect2(218676.69093240067, 1029.1005684054353);    verts.push_back(v1);
		Vect2 v2 = Vect2(262650.09206012066, 30568.484537899763);    verts.push_back(v2);
		BoundingRectangle br = BoundingRectangle(verts);
		//f.pln(" br = "+br);
		EXPECT_NEAR(197193.5025,br.getMinX(),0.0001);
		EXPECT_NEAR(262650.0920,br.getMaxX(),0.0001);
		EXPECT_NEAR(1029.1005,br.getMinY(),0.0001);
		EXPECT_NEAR(34260.9075,br.getMaxY(),0.0001);
		//EXPECT_NEAR(0.0,br.getZMin(),0.0001);
		//EXPECT_NEAR(0.0,br.getZMax(),0.0001);
        EXPECT_FALSE(br.contains(0,0));
        EXPECT_FALSE(br.contains(0,10000));
        EXPECT_FALSE(br.contains(0,50000));
        EXPECT_FALSE(br.contains(10000,0));
        EXPECT_FALSE(br.contains(10000,10000));
        EXPECT_FALSE(br.contains(50000,10000));
        EXPECT_FALSE(br.contains(50000,50000));
//        br.add(Vect2(50100,50200));
//        EXPECT_TRUE(br.contains(0,50000));
//        EXPECT_TRUE(br.contains(10000,0));
//        EXPECT_TRUE(br.contains(10000,10000));
//        EXPECT_TRUE(br.contains(50000,10000));
//        EXPECT_TRUE(br.contains(50000,50000));
//        EXPECT_FALSE(br.contains(50000,50201));
//        EXPECT_TRUE(br.contains(50111,50000));
	}



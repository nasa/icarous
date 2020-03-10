/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include <cmath>
#include <gtest/gtest.h>
#include "Poly3D.h"


using namespace larcfm;

class Poly3DTest : public ::testing::Test {
	
public:

protected:
    virtual void SetUp() {
    }
};

	TEST_F(Poly3DTest, test0) {
		Poly3D p0 = Poly3D();
		// order of insertion is important
		p0.add(Vect2(0.0,0.0));
		p0.add(Vect2(4.0,0.0));
		p0.add(Vect2(4.0,4.0));
		p0.add(Vect2(0.0,4.0));

		EXPECT_NEAR(0.0,p0.get2D(3).x,0.001);
		EXPECT_NEAR(4.0,p0.get2D(3).y,0.001);

		EXPECT_EQ(4,p0.size());
		//fpln(" p0 = "+p0);
		EXPECT_NEAR(0.0,p0.getTop(),0.001);  
		EXPECT_NEAR(0.0,p0.getBottom(),0.001); 
		p0.setTop(100.0);
		p0.setBottom(20.0);
		EXPECT_NEAR(100.0,p0.getTop(),0.001);  
		EXPECT_NEAR(20.0,p0.getBottom(),0.001); 		
		EXPECT_NEAR(2.0,p0.centroid().x,0.001);
		EXPECT_NEAR(2.0,p0.centroid().y,0.001);
	}










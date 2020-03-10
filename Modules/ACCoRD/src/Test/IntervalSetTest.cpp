/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

// Uses the Google unit test framework

#include "IntervalSet.h"
#include <gtest/gtest.h>


using namespace larcfm;

class IntervalSetTest : public ::testing::Test {

public:
	IntervalSet a;
	IntervalSet rs1;
	IntervalSet rs3;
	Interval r0;
	Interval r1;
	Interval r2;
	Interval r3;
	Interval r5;
	Interval r7;

protected: 
	virtual void SetUp() {
		r0 = Interval(0.0, 1.0);
		r1 = Interval(1.0, 2.0);
		r2 = Interval(2.0, 3.0);
		r3 = Interval(3.0, 4.0);
		r5 = Interval(5.0, 6.0);
		r7 = Interval(7.0, 8.0);

		a = IntervalSet();
		rs1 = IntervalSet();
		rs1.unions(r1);

		rs3 = IntervalSet();
		rs3.unions(r1);
		rs3.unions(r3);
	}

	virtual void TearDown() {
		// Check to make sure rs3 didn't change
		EXPECT_EQ(rs3.size(), 2);
		EXPECT_NEAR(1.0, rs3.getInterval(0).low, 0.0);
		EXPECT_NEAR(2.0, rs3.getInterval(0).up, 0.0);
		EXPECT_NEAR(3.0, rs3.getInterval(1).low, 0.0);
		EXPECT_NEAR(4.0, rs3.getInterval(1).up, 0.0);
	}
};

TEST_F(IntervalSetTest,testIntervalSet) {
	EXPECT_EQ(a.size(), 0);
	EXPECT_EQ(rs1.size(), 1);
}

TEST_F(IntervalSetTest, testIntervalSetIntervalSet) {
	EXPECT_EQ(rs3.size(), 2);
	EXPECT_NEAR(1.0, rs3.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs3.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs3.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs3.getInterval(1).up, 0.0);
}

TEST_F(IntervalSetTest, testClear) {
	a.unions(r0);
	EXPECT_FALSE(a.size() == 0);
	a.clear();
	EXPECT_EQ(a.size(), 0);
}

TEST_F(IntervalSetTest, testSize) {
	EXPECT_EQ(rs3.size(), 2);
}

TEST_F(IntervalSetTest, testGetInterval) {
	EXPECT_NEAR(1.0, rs3.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs3.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs3.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs3.getInterval(1).up, 0.0);
}

TEST_F(IntervalSetTest, testIn) {
	EXPECT_TRUE(rs1.in(1.0));
	EXPECT_TRUE(rs1.in(1.5));
	EXPECT_TRUE(rs1.in(2.0));
	EXPECT_FALSE(rs1.in(0.5));
	EXPECT_FALSE(rs1.in(2.5));
}

TEST_F(IntervalSetTest, testIntervalIn) {
  EXPECT_TRUE(r1.in(1.0));
  EXPECT_TRUE(r1.inCC(1.0));
  EXPECT_TRUE(r1.almost_in(1.0,true,true));
  EXPECT_TRUE(r1.inCO(1.0));
  EXPECT_TRUE(r1.almost_in(1.0,true,false));
  EXPECT_FALSE(r1.inOO(1.0));
  EXPECT_FALSE(r1.almost_in(1.0,false,false));
  EXPECT_TRUE(r1.in(2.0));
  EXPECT_TRUE(r1.inCC(2.0));
  EXPECT_TRUE(r1.almost_in(2.0,true,true));
  EXPECT_TRUE(r1.inOC(2.0));
  EXPECT_TRUE(r1.almost_in(2.0,false,true));
  EXPECT_FALSE(r1.inOO(2.0));
  EXPECT_FALSE(r1.almost_in(2.0,false,false));
  EXPECT_TRUE(r1.in(1.5));
  EXPECT_TRUE(r1.inCC(1.5));
  EXPECT_TRUE(r1.almost_in(1.5,true,true));
  EXPECT_TRUE(r1.inOC(1.5));
  EXPECT_TRUE(r1.almost_in(1.5,false,true));
  EXPECT_TRUE(r1.inOO(1.5));
  EXPECT_TRUE(r1.almost_in(1.5,false,false));
  EXPECT_FALSE(r1.in(0.5));
  EXPECT_FALSE(r1.inCC(0.5));
  EXPECT_FALSE(r1.almost_in(0.5,true,true));
  EXPECT_FALSE(r1.inOC(0.5));
  EXPECT_FALSE(r1.almost_in(0.5,false,true));
  EXPECT_FALSE(r1.inOO(0.5));
  EXPECT_FALSE(r1.almost_in(0.5,false,false));
}

TEST_F(IntervalSetTest, testUnionInterval) {
	rs1 = IntervalSet();
	rs1.unions(r1);
	EXPECT_EQ(rs1.size(), 1);
	EXPECT_NEAR(1.0, rs1.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs1.getInterval(0).up, 0.0);

	rs1 = IntervalSet();
	rs1.unions(r1);
	rs1.unions(r2);
	EXPECT_EQ(rs1.size(), 1);
	EXPECT_NEAR(1.0, rs1.getInterval(0).low, 0.0);
	EXPECT_NEAR(3.0, rs1.getInterval(0).up, 0.0);
}

TEST_F(IntervalSetTest, testUnionInterval2) {
	EXPECT_EQ(rs3.size(), 2);
	EXPECT_NEAR(1.0, rs3.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs3.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs3.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs3.getInterval(1).up, 0.0);

	IntervalSet rs4;
	rs4 = IntervalSet(rs1);
	rs4.unions(r0);
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(0.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(2.5,2.7));
	EXPECT_EQ(rs4.size(), 3);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(2.5, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(2.7, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(0.5,2.5));
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(0.5, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(0.5,3.5));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(0.5, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(0.5,5.0));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(0.5, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(5.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(0.5,3.0));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(0.5, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(1.5,2.5));
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(Interval(1.5,3.5));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.unions(r5);
	rs4.unions(Interval(1.5,7.5));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(7.5, rs4.getInterval(0).up, 0.0);
}

TEST_F(IntervalSetTest, testUnionIntervalSet) {
	IntervalSet rs4;
	rs4 = IntervalSet();

	rs4.unions(r5);
	rs4.unions(r7);
	rs4.unions(rs3);
	EXPECT_EQ(rs4.size(), 4);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(5.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(6.0, rs4.getInterval(2).up, 0.0);
	EXPECT_NEAR(7.0, rs4.getInterval(3).low, 0.0);
	EXPECT_NEAR(8.0, rs4.getInterval(3).up, 0.0);

	rs4 = IntervalSet();
	rs4.unions(r0);
	rs4.unions(r2);
	rs4.unions(rs3);
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(0.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet();
	rs4.unions(Interval(2.2,2.4));
	rs4.unions(Interval(4.2,4.4));
	rs4.unions(rs3);
	EXPECT_EQ(rs4.size(), 4);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(2.2, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(2.4, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);
	EXPECT_NEAR(4.2, rs4.getInterval(3).low, 0.0);
	EXPECT_NEAR(4.4, rs4.getInterval(3).up, 0.0);
}

TEST_F(IntervalSetTest, testDiffInterval) {
	IntervalSet rs4;
	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(0.5, 1.5));
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.5, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(0.5, 0.7));
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.5, 2.5));
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.5, 3.5));
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.5, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(0.5, 2.5));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(3.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.5, 4.5));
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.2, 1.5));
	EXPECT_EQ(rs4.size(), 3);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.2, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(r1);
	EXPECT_EQ(rs4.size(), 3);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.0, 1.5));
	EXPECT_EQ(rs4.size(), 3);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.5, 2.0));
	EXPECT_EQ(rs4.size(), 3);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);
}

TEST_F(IntervalSetTest, testDiffIntervalSet) {
	IntervalSet rs4;

	rs4 = IntervalSet();
	rs4.unions(Interval(2.2,2.4));
	rs4.unions(Interval(4.2,4.4));
	rs4.diff(rs3);
	EXPECT_EQ(2, rs4.size());
	EXPECT_NEAR(2.2, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.4, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(4.2, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.4, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	IntervalSet rs5 = IntervalSet();
	rs5.unions(Interval(1.5,3.5));
	rs5.unions(Interval(3.6,3.7));
	rs4.diff(rs5);
	EXPECT_EQ(3, rs4.size());
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.5, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(3.6, rs4.getInterval(1).up, 0.0);
	EXPECT_NEAR(3.7, rs4.getInterval(2).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(2).up, 0.0);
}

TEST_F(IntervalSetTest, testIntersectIntervalSet0) {
	IntervalSet s1 = IntervalSet();
	s1.unions(Interval(0.5,1.0));
	s1.unions(Interval(1.5,2.5));
	IntervalSet s2 = IntervalSet();
	s1.almost_intersect(s2);
	EXPECT_TRUE(s1.isEmpty());
	s1.clear();
	s2.unions(Interval(0.5,1.0));
	s2.unions(Interval(1.5,2.5));
	s1.almost_intersect(s2);
	EXPECT_TRUE(s1.isEmpty());
}

TEST_F(IntervalSetTest, testIntersectIntervalSet1) {
	IntervalSet s1 = IntervalSet();
	s1.unions(Interval(0.5,1.0));
	s1.unions(Interval(1.5,2.5));
	IntervalSet s2 = IntervalSet();
	s2.unions(Interval(0.0,0.7));
	s2.unions(Interval(0.8,1.7));
	s2.unions(Interval(1.9,2.7));
	s1.almost_intersect(s2);
	EXPECT_EQ(4,s1.size());
	EXPECT_NEAR(0.5,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(0.7,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(0.8,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(1.0,s1.getInterval(1).up,0.1);
	EXPECT_NEAR(1.5,s1.getInterval(2).low,0.1);
	EXPECT_NEAR(1.7,s1.getInterval(2).up,0.1);
	EXPECT_NEAR(1.9,s1.getInterval(3).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(3).up,0.1);
	s1.clear();
	s1.unions(Interval(0.0,0.7));
	s1.unions(Interval(0.8,1.7));
	s1.unions(Interval(1.9,2.7));
	s2.clear();
	s2.unions(Interval(0.5,1.0));
	s2.unions(Interval(1.5,2.5));
	s1.almost_intersect(s2);
	EXPECT_EQ(4,s1.size());
	EXPECT_NEAR(0.5,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(0.7,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(0.8,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(1.0,s1.getInterval(1).up,0.1);
	EXPECT_NEAR(1.5,s1.getInterval(2).low,0.1);
	EXPECT_NEAR(1.7,s1.getInterval(2).up,0.1);
	EXPECT_NEAR(1.9,s1.getInterval(3).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(3).up,0.1);
}

TEST_F(IntervalSetTest, testIntersectIntervalSet2) {
	IntervalSet s1 = IntervalSet();
	s1.unions(Interval(0.5,1.0));
	s1.unions(Interval(1.5,2.5));
	IntervalSet s2 = IntervalSet();
	s2.unions(Interval(1.0,1.5));
	s1.almost_intersect(s2);
	EXPECT_TRUE(s1.isEmpty());
	s1.clear();
	s1.unions(Interval(1.0,1.5));
	s2.clear();
	s2.unions(Interval(0.5,1.0));
	s2.unions(Interval(1.5,2.5));
	s1.almost_intersect(s2);
	EXPECT_TRUE(s1.isEmpty());
}

TEST_F(IntervalSetTest, testIntersectIntervalSet3) {
	IntervalSet s1 = IntervalSet();
	s1.unions(Interval(0.5,1.0));
	s1.unions(Interval(1.5,2.5));
	IntervalSet s2 = IntervalSet();
	s2.unions(Interval(1.2,1.4));
	s2.unions(Interval(2.0,3.0));
	s1.almost_intersect(s2);
	EXPECT_EQ(1,s1.size());
	EXPECT_NEAR(2.0,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(0).up,0.1);
	s1.clear();
	s1.unions(Interval(1.2,1.4));
	s1.unions(Interval(2.0,3.0));
	s2.clear();
	s2.unions(Interval(0.5,1.0));
	s2.unions(Interval(1.5,2.5));
	s1.almost_intersect(s2);
	EXPECT_EQ(1,s1.size());
	EXPECT_NEAR(2.0,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(0).up,0.1);
}

TEST_F(IntervalSetTest, testAddntervalSet) {
	IntervalSet s1 = IntervalSet();
	s1.almost_add(-0.3,1.2);
	EXPECT_EQ(1,s1.size());
	EXPECT_NEAR(-0.3,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(1.2,s1.getInterval(0).up,0.1);
	s1.almost_add(-1.0,0.5);
	EXPECT_EQ(1,s1.size());
	EXPECT_NEAR(-1.0,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(1.2,s1.getInterval(0).up,0.1);
	s1.almost_add(2.0,2.5);
	EXPECT_EQ(2,s1.size());
	EXPECT_NEAR(-1.0,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(1.2,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(2.0,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(1).up,0.1);
	s1.almost_add(1.5,1.8);
	EXPECT_EQ(3,s1.size());
	EXPECT_NEAR(-1.0,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(1.2,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(1.5,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(1.8,s1.getInterval(1).up,0.1);
	EXPECT_NEAR(2.0,s1.getInterval(2).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(2).up,0.1);
	s1.almost_add(-2.5,-1.8);
	EXPECT_EQ(4,s1.size());
	EXPECT_NEAR(-2.5,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(-1.8,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(-1.0,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(1.2,s1.getInterval(1).up,0.1);
	EXPECT_NEAR(1.5,s1.getInterval(2).low,0.1);
	EXPECT_NEAR(1.8,s1.getInterval(2).up,0.1);
	EXPECT_NEAR(2.0,s1.getInterval(3).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(3).up,0.1);
	s1.almost_add(-2.0,-1.9);
	EXPECT_EQ(4,s1.size());
	EXPECT_NEAR(-2.5,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(-1.8,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(-1.0,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(1.2,s1.getInterval(1).up,0.1);
	EXPECT_NEAR(1.5,s1.getInterval(2).low,0.1);
	EXPECT_NEAR(1.8,s1.getInterval(2).up,0.1);
	EXPECT_NEAR(2.0,s1.getInterval(3).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(3).up,0.1);
	s1.almost_add(-1.8,1.3);
	EXPECT_EQ(3,s1.size());
	EXPECT_NEAR(-2.5,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(1.3,s1.getInterval(0).up,0.1);
	EXPECT_NEAR(1.5,s1.getInterval(1).low,0.1);
	EXPECT_NEAR(1.8,s1.getInterval(1).up,0.1);
	EXPECT_NEAR(2.0,s1.getInterval(2).low,0.1);
	EXPECT_NEAR(2.5,s1.getInterval(2).up,0.1);
	s1.almost_add(1.3,3.0);
	EXPECT_EQ(1,s1.size());
	EXPECT_NEAR(-2.5,s1.getInterval(0).low,0.1);
	EXPECT_NEAR(3.0,s1.getInterval(0).up,0.1);
}

TEST_F(IntervalSetTest, testRemoveSingle) {
	IntervalSet rs4;
	rs4 = IntervalSet(rs3);
	rs4.diff(r1);
	rs4.removeSingle(1.0);
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(2.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);
}

TEST_F(IntervalSetTest, testSweepSingle) {
	IntervalSet rs4;
	rs4 = IntervalSet(rs3);
	rs4.diff(r1);
	rs4.sweepSingle();
	EXPECT_EQ(rs4.size(), 1);
	EXPECT_NEAR(3.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(0).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.0, 1.5));
	rs4.sweepSingle();
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.5, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(2.0, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);

	rs4 = IntervalSet(rs3);
	rs4.diff(Interval(1.5, 2.0));
	rs4.sweepSingle();
	EXPECT_EQ(rs4.size(), 2);
	EXPECT_NEAR(1.0, rs4.getInterval(0).low, 0.0);
	EXPECT_NEAR(1.5, rs4.getInterval(0).up, 0.0);
	EXPECT_NEAR(3.0, rs4.getInterval(1).low, 0.0);
	EXPECT_NEAR(4.0, rs4.getInterval(1).up, 0.0);
}


//
//#ifdef _MSC_VER
//int main(int argc, char** argv)
//{
//
//	testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
//
//	system ("pause");
//}
//#endif

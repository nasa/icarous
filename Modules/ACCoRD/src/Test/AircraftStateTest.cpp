/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "Vect2.h"
#include "Units.h"
#include "Position.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "AircraftState.h"
#include "EuclideanProjection.h"
#include "Projection.h"

#include <cmath>
#include <gtest/gtest.h>

using namespace larcfm;

class AircraftStateTest : public ::testing::Test {

protected:
	AircraftState ah;
	Position pos1;
	Velocity vel1;
	Position pos2;
	Velocity vel2;
	Position pos3;
	Velocity vel3;
	Position pos4;
	Velocity vel4;
	Position pos5;
	Velocity vel5;


	virtual void SetUp() {
		ah = AircraftState("TestState", 5);
		pos1 = Position::makeLatLonAlt(10.1,10.0,10000.0);
		vel1 = Velocity::mkVxyz(5.1,5.0,50.0);
		pos2 = Position::makeLatLonAlt(10.2,10.0,10000.0);
		vel2 = Velocity::mkVxyz(5.2,5.0,50.0);
		pos3 = Position::makeLatLonAlt(10.3,10.0,10000.0);
		vel3 = Velocity::mkVxyz(5.3,5.0,50.0);
		pos4 = Position::makeLatLonAlt(10.4,10.0,10000.0);
		vel4 = Velocity::mkVxyz(5.4,5.0,50.0);
		pos5 = Position::makeLatLonAlt(10.5,10.0,10000.0);
		vel5 = Velocity::mkVxyz(5.5,5.0,50.0);
	}
};

TEST_F(AircraftStateTest,  testAircraftStateStringInt) {
	EXPECT_EQ("TestState", ah.name());
	EXPECT_EQ(5, ah.getBufferSize());
	EXPECT_EQ(0, ah.size());
}

TEST_F(AircraftStateTest,  testAircraftState) {
	ah = AircraftState();
	EXPECT_EQ("Aircraft", ah.name());
	EXPECT_EQ(10, ah.getBufferSize());
	EXPECT_EQ(0, ah.size());
}

TEST_F(AircraftStateTest,  testAircraftStateString) {
	ah = AircraftState("TestState2");
	EXPECT_EQ("TestState2", ah.name());
	EXPECT_EQ(10, ah.getBufferSize());
	EXPECT_EQ(0, ah.size());
}

TEST_F(AircraftStateTest,  testCopy) {
	ah.add(pos1,vel1,1.0);

	AircraftState t = ah.copy();

	EXPECT_EQ("TestState", t.name());
	EXPECT_EQ(5, t.getBufferSize());
	EXPECT_EQ(1, t.size());
	EXPECT_NEAR(10.1, t.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, t.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, t.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.1,  t.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  t.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, t.velocity(0).z, 0.0);
	EXPECT_NEAR(1.0,  t.time(0), 0.0);
}

TEST_F(AircraftStateTest,  testClear) {
	ah.add(pos1,vel1,1.0);
	EXPECT_EQ(1, ah.size());

	ah.add(pos2,vel2,2.0);
	EXPECT_EQ(2, ah.size());

	ah.add(pos3,vel3,3.0);
	EXPECT_EQ(3, ah.size());

	ah.clear();
	EXPECT_EQ(0, ah.size());
}

TEST_F(AircraftStateTest,  testRemove) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	ah.add(pos4,vel4,4.0);
	ah.add(pos5,vel5,5.0);
	ah.add(pos1,vel2,6.0);
	ah.add(pos1,vel3,7.0);
	ah.add(pos1,vel4,8.0);
	EXPECT_EQ(5, ah.size());

	ah.remove(3);
	EXPECT_EQ(2, ah.size());

	// Check the values in the array
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);
	EXPECT_NEAR(7.0, ah.time(0), 0.0);

	EXPECT_NEAR(10.1, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);
	EXPECT_NEAR(8.0, ah.time(1), 0.0);

}

TEST_F(AircraftStateTest,  testFind) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	ah.add(pos4,vel4,4.0);
	ah.add(pos5,vel5,5.0);
	ah.add(pos1,vel2,6.0);
	ah.add(pos1,vel3,7.0);
	ah.add(pos1,vel4,8.0);
	EXPECT_EQ(5, ah.size());

	int i;
	i = ah.find(2.0);
	EXPECT_EQ(-1, i);
	i = ah.find(7.0);
	EXPECT_EQ(3, i);
	i = ah.find(6.5);
	EXPECT_EQ(-4, i);
	i = ah.find(8.0);
	EXPECT_EQ(4, i);
	i = ah.find(8.5);
	EXPECT_EQ(-6, i);

}

TEST_F(AircraftStateTest,  testRemoveUpToTime) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	ah.add(pos4,vel4,4.0);
	ah.add(pos5,vel5,5.0);
	ah.add(pos1,vel2,6.0);
	ah.add(pos1,vel3,7.0);
	ah.add(pos1,vel4,8.0);
	EXPECT_EQ(5, ah.size());

	ah.removeUpToTime(6.5);
	EXPECT_EQ(2, ah.size());

	// Check the values in the array
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);
	EXPECT_NEAR(7.0, ah.time(0), 0.0);

	EXPECT_NEAR(10.1, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);
	EXPECT_NEAR(8.0, ah.time(1), 0.0);
}

TEST_F(AircraftStateTest,  testRemoveUpToTime2) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	ah.add(pos4,vel4,4.0);
	ah.add(pos5,vel5,5.0);
	ah.add(pos1,vel2,6.0);
	ah.add(pos1,vel3,7.0);
	ah.add(pos1,vel4,8.0);
	EXPECT_EQ(5, ah.size());

	ah.removeUpToTime(7.0);
	EXPECT_EQ(2, ah.size());

	// Check the values in the array
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);
	EXPECT_NEAR(7.0, ah.time(0), 0.0);

	EXPECT_NEAR(10.1, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);
	EXPECT_NEAR(8.0, ah.time(1), 0.0);
}

TEST_F(AircraftStateTest,  testRemoveLast) {
	ah.clear();
	ah.add(pos1,vel1,1.1);
	ah.add(pos2,vel2,2.2);
	ah.add(pos3,vel3,3.3);
	ah.add(pos4,vel4,4.2);
	ah.add(pos5,vel5,5.3);
	//ah.add(pos3,vel2,6.5);
	EXPECT_EQ(5, ah.size());
	ah.removeLast();
	EXPECT_EQ(4, ah.size());
	EXPECT_NEAR(4.2, ah.timeLast(),0.0);
	// Check the values in the array
	EXPECT_NEAR(1.1, ah.time(0), 0.0);
	EXPECT_NEAR(4.2, ah.time(3), 0.0);
	//
	// Add more elements to exercise wraparound
	//
	ah.add(pos3,vel1,6.5);
	EXPECT_EQ(5, ah.size());
	ah.add(pos3,vel2,7.5);
	EXPECT_EQ(5, ah.size());
	EXPECT_NEAR(7.5, ah.timeLast(),0.0);
	//
	// Leave only one element in list
	//
	ah.removeLast();
	ah.removeLast();
	ah.removeLast();
	ah.removeLast();
	EXPECT_EQ(1, ah.size());
	EXPECT_NEAR(2.2, ah.timeLast(),0.0);
	//
	// Remove the rest of the elements
	//
	ah.removeLast();
	EXPECT_EQ(0, ah.size());
	ah.removeLast();
	EXPECT_EQ(0, ah.size());
	ah.removeLast();
	EXPECT_EQ(0, ah.size());
}



TEST_F(AircraftStateTest,  testAddNominal) {
	ah.add(pos1,vel1,1.0);
	EXPECT_EQ(1, ah.size());
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.1,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);
	EXPECT_NEAR(1.0, ah.time(0), 0.0);

	ah.add(pos2,vel2,2.0);
	EXPECT_EQ(2, ah.size());

	ah.add(pos3,vel3,3.0);
	EXPECT_EQ(3, ah.size());

	ah.add(pos4,vel4,4.0);
	EXPECT_EQ(4, ah.size());

	ah.add(pos5,vel5,5.0);
	EXPECT_EQ(5, ah.size());

	// Check the values in the array
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.1,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);
	EXPECT_NEAR(1.0, ah.time(0), 0.0);

	EXPECT_NEAR(10.2, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);
	EXPECT_NEAR(2.0, ah.time(1), 0.0);

	EXPECT_NEAR(10.3, ah.position(2).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(2).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(2).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(2).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(2).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(2).z, 0.0);
	EXPECT_NEAR(3.0, ah.time(2), 0.0);

	EXPECT_NEAR(10.4, ah.position(3).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(3).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(3).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(3).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(3).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(3).z, 0.0);
	EXPECT_NEAR(4.0, ah.time(3), 0.0);

	EXPECT_NEAR(10.5, ah.position(4).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(4).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(4).altitude(), 0.0);
	EXPECT_NEAR(5.5,  ah.velocity(4).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(4).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(4).z, 0.0);
	EXPECT_NEAR(5.0, ah.time(4), 0.0);

	// Continue adding, testing the wrap around

	ah.add(pos1,vel2,6.0);
	EXPECT_EQ(5, ah.size());

	ah.add(pos1,vel3,7.0);
	EXPECT_EQ(5, ah.size());

	ah.add(pos1,vel4,8.0);
	EXPECT_EQ(5, ah.size());


	// Check the values in the array
	EXPECT_NEAR(10.4, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);
	EXPECT_NEAR(4.0, ah.time(0), 0.0);

	EXPECT_NEAR(10.5, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.5,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);
	EXPECT_NEAR(5.0, ah.time(1), 0.0);

	EXPECT_NEAR(10.1, ah.position(2).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(2).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(2).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(2).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(2).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(2).z, 0.0);
	EXPECT_NEAR(6.0, ah.time(2), 0.0);

	EXPECT_NEAR(10.1, ah.position(3).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(3).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(3).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(3).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(3).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(3).z, 0.0);
	EXPECT_NEAR(7.0, ah.time(3), 0.0);

	EXPECT_NEAR(10.1, ah.position(4).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(4).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(4).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(4).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(4).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(4).z, 0.0);
	EXPECT_NEAR(8.0, ah.time(4), 0.0);

	// Test values past the end
	EXPECT_EQ(Position::ZERO_LL().lat(), ah.position(5).lat());
	EXPECT_EQ(Position::ZERO_LL().lon(), ah.position(5).lon());
	EXPECT_EQ(Position::ZERO_LL().alt(), ah.position(5).alt());
	EXPECT_EQ(Velocity::ZEROV().x, ah.velocity(5).x);
	EXPECT_EQ(Velocity::ZEROV().y, ah.velocity(5).y);
	EXPECT_EQ(Velocity::ZEROV().z, ah.velocity(5).z);
	EXPECT_TRUE (ah.time(5) < 0.0);

	// Test values before the beginning
	EXPECT_EQ(Position::ZERO_LL().lat(), ah.position(-1).lat());
	EXPECT_EQ(Position::ZERO_LL().lon(), ah.position(-1).lon());
	EXPECT_EQ(Position::ZERO_LL().alt(), ah.position(-1).alt());
	EXPECT_EQ(Velocity::ZEROV().x, ah.velocity(-1).x);
	EXPECT_EQ(Velocity::ZEROV().y, ah.velocity(-1).y);
	EXPECT_EQ(Velocity::ZEROV().z, ah.velocity(-1).z);
	EXPECT_TRUE (ah.time(-1) < 0.0);

}

TEST_F(AircraftStateTest,  testAddOutOfOrder_full_array) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	ah.add(pos4,vel4,4.0);
	ah.add(pos5,vel5,5.0);
	EXPECT_EQ(5, ah.size());

	// Continue adding, testing the wrap around

	ah.add(pos1,vel2,0.5);
	EXPECT_EQ(5, ah.size());

	// Check the values in the array
	EXPECT_NEAR(1.0,  ah.time(0), 0.0);
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.1,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);

	ah.add(pos1,vel2,1.1);
	EXPECT_EQ(5, ah.size());

	// Check the values in the array
	EXPECT_NEAR(1.1,  ah.time(0), 0.0);
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);

	ah.add(pos1,vel3,3.5);
	EXPECT_EQ(5, ah.size());

	// Check the values in the array
	EXPECT_NEAR(2.0,  ah.time(0), 0.0);
	EXPECT_NEAR(10.2, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);

	EXPECT_NEAR(3.0,  ah.time(1), 0.0);
	EXPECT_NEAR(10.3, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);

	EXPECT_NEAR(3.5,  ah.time(2), 0.0);
	EXPECT_NEAR(10.1, ah.position(2).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(2).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(2).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(2).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(2).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(2).z, 0.0);

	EXPECT_NEAR(4.0,  ah.time(3), 0.0);
	EXPECT_NEAR(10.4, ah.position(3).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(3).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(3).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(3).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(3).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(3).z, 0.0);

	EXPECT_NEAR(5.0,  ah.time(4), 0.0);
	EXPECT_NEAR(10.5, ah.position(4).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(4).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(4).altitude(), 0.0);
	EXPECT_NEAR(5.5,  ah.velocity(4).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(4).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(4).z, 0.0);

}

TEST_F(AircraftStateTest,  testAddOutOfOrder_not_full_array) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	EXPECT_EQ(3, ah.size());

	// Continue adding, testing the wrap around

	ah.add(pos1,vel2,0.5);
	EXPECT_EQ(4, ah.size());

	// Check the values in the array
	EXPECT_NEAR(0.5,  ah.time(0), 0.0);
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);

	ah.add(pos1,vel3,2.5);
	EXPECT_EQ(5, ah.size());

	// Check the values in the array
	EXPECT_NEAR(0.5,  ah.time(0), 0.0);
	EXPECT_NEAR(1.0,  ah.time(1), 0.0);
	EXPECT_NEAR(2.0,  ah.time(2), 0.0);
	EXPECT_NEAR(2.5,  ah.time(3), 0.0);
	EXPECT_NEAR(3.0,  ah.time(4), 0.0);

	EXPECT_NEAR(10.1, ah.position(3).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(3).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(3).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(3).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(3).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(3).z, 0.0);

}

TEST_F(AircraftStateTest,  testAddOutOfOrder_not_full_array2) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	EXPECT_EQ(3, ah.size());

	// Continue adding, testing the wrap around

	ah.add(pos1,vel2,3.5);
	EXPECT_EQ(4, ah.size());

	// Check the values in the array
	EXPECT_NEAR(1.0,  ah.time(0), 0.0);
	EXPECT_NEAR(2.0,  ah.time(1), 0.0);
	EXPECT_NEAR(3.0,  ah.time(2), 0.0);
	EXPECT_NEAR(3.5,  ah.time(3), 0.0);
	EXPECT_NEAR(10.1, ah.position(3).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(3).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(3).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(3).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(3).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(3).z, 0.0);
}

TEST_F(AircraftStateTest,  testSetPositionVelocityDouble) {
	ah.add(pos1,vel1,1.0);
	ah.add(pos2,vel2,2.0);
	ah.add(pos3,vel3,3.0);
	EXPECT_EQ(3, ah.size());

	// Overwrite previous times

	ah.add(pos1,vel2,1.0);
	ah.add(pos1,vel3,2.0);
	ah.add(pos1,vel4,3.0);
	EXPECT_EQ(3, ah.size());

	// Check the values in the array
	EXPECT_NEAR(1.0,  ah.time(0), 0.0);
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);

	EXPECT_NEAR(2.0,  ah.time(1), 0.0);
	EXPECT_NEAR(10.1, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);

	EXPECT_NEAR(3.0,  ah.time(2), 0.0);
	EXPECT_NEAR(10.1, ah.position(2).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(2).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(2).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(2).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(2).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(2).z, 0.0);

	// add an out of order point
	ah.add(pos1,vel5,2.5);

	// Check the values in the array
	EXPECT_NEAR(1.0,  ah.time(0), 0.0);
	EXPECT_NEAR(10.1, ah.position(0).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(0).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(0).altitude(), 0.0);
	EXPECT_NEAR(5.2,  ah.velocity(0).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(0).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(0).z, 0.0);

	EXPECT_NEAR(2.0,  ah.time(1), 0.0);
	EXPECT_NEAR(10.1, ah.position(1).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(1).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(1).altitude(), 0.0);
	EXPECT_NEAR(5.3,  ah.velocity(1).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(1).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(1).z, 0.0);

	EXPECT_NEAR(2.5,  ah.time(2), 0.0);
	EXPECT_NEAR(10.1, ah.position(2).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(2).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(2).altitude(), 0.0);
	EXPECT_NEAR(5.5,  ah.velocity(2).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(2).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(2).z, 0.0);

	EXPECT_NEAR(3.0,  ah.time(3), 0.0);
	EXPECT_NEAR(10.1, ah.position(3).latitude(), 0.0);
	EXPECT_NEAR(10.0, ah.position(3).longitude(), 0.0);
	EXPECT_NEAR(10000.0, ah.position(3).altitude(), 0.0);
	EXPECT_NEAR(5.4,  ah.velocity(3).x, 0.0);
	EXPECT_NEAR(5.0,  ah.velocity(3).y, 0.0);
	EXPECT_NEAR(50.0, ah.velocity(3).z, 0.0);
}

//TEST_F(AircraftStateTest,  testProjection) {
//	ah.add(pos1,vel1,1.0);
//	ah.add(pos2,vel2,2.0);
//	ah.add(pos3,vel3,3.0);
//	EXPECT_EQ(3, ah.size());
//
//	EuclideanProjection sp = getProjection(from("deg",10.1), from("deg",10.0));
//	ah.setProjection(sp);
//
//	std::pair<Vect3,Velocity> p0 = ah.get(0);
//	EXPECT_NEAR(1.0, ah.time(0), 0.000000001);
//	EXPECT_NEAR(0.0, p0.first.x, 0.000000001);
//	EXPECT_NEAR(0.0, p0.first.y, 0.000000001);
//	EXPECT_NEAR(pos1.alt(), p0.first.z, 0.000000001);
//	EXPECT_NEAR(vel1.x, p0.second.x, 0.00001);
//	EXPECT_NEAR(vel1.y, p0.second.y, 0.00001);
//	EXPECT_NEAR(vel1.z, p0.second.z, 0.000000001);
//
//	std::pair<Vect3,Velocity> p1 = ah.get(1);
//	EXPECT_NEAR(2.0, ah.time(1), 0.000000001);
//	EXPECT_TRUE(p1.first.x > 0.0);
//	EXPECT_TRUE(p1.first.y > 0.0);
//	EXPECT_NEAR(pos2.alt(), p1.first.z, 0.000000001);
//	EXPECT_NEAR(vel2.x, p1.second.x, 0.000000001);
//	EXPECT_NEAR(vel2.y, p1.second.y, 0.000000001);
//	EXPECT_NEAR(vel2.z, p1.second.z, 0.000000001);
//
//	std::pair<Vect3,Velocity> p2 = ah.get(2);
//	EXPECT_NEAR(3.0, ah.time(2), 0.000000001);
//	EXPECT_TRUE(p2.first.x > 0.0);
//	EXPECT_TRUE(p2.first.y > 0.0);
//	EXPECT_NEAR(pos3.alt(), p2.first.z, 0.000000001);
//	EXPECT_NEAR(vel3.x, p2.second.x, 0.000000001);
//	EXPECT_NEAR(vel3.y, p2.second.y, 0.000000001);
//	EXPECT_NEAR(vel3.z, p2.second.z, 0.000000001);
//
//	ah.add(Position::makeLatLonAlt(9.9,9.9,10000.0),vel1,4.0);
//	std::pair<Vect3,Velocity> p3 = ah.get(3);
//	EXPECT_NEAR(4.0, ah.time(3), 0.000000001);
//	EXPECT_TRUE(p3.first.x < 0.0);
//	EXPECT_TRUE(p3.first.y < 0.0);
//	EXPECT_NEAR(pos3.alt(), p3.first.z, 0.000000001);
//	EXPECT_NEAR(vel1.x, p3.second.x, 0.000000001);
//	EXPECT_NEAR(vel1.y, p3.second.y, 0.000000001);
//	EXPECT_NEAR(vel1.z, p3.second.z, 0.000000001);
//
//}

Vect3 actPos(double t) {
	double xvar = 20.5 + t * (3 + 4 * t) * 1.7;
	double yvar = 12.2 + t * (3 + 4 * t) * 2.6;
	double zvar = 3.4 * t * t + 2.5 * t + 5.2;
	Vect3 actPosVect(xvar, yvar, zvar);
	return actPosVect;
}

Velocity actVel(double t) {
	double vx = 2 * 4 * 1.7 * t + 3 * 1.7;
	double vy = 2 * 4 * 2.6 * t + 3 * 2.6;
	double vz = 2 * 3.4 * t + 2.5;
	return Velocity::mkVxyz(vx, vy,vz);
}

TEST_F(AircraftStateTest, testPrediction) {
	AircraftState ah;
	double predictionTime = 7.0;
	for (double i = 1.0; i <= predictionTime; i = i + 1.0) {
		ah.add(Position(actPos(i)), actVel(i), 1.0*i);
	}

	EXPECT_EQ(7, ah.size());
	EXPECT_NEAR(7.0, ah.timeLast(), 0.0);

	StateVector p;
	p = ah.pred(predictionTime);
	EXPECT_NEAR(actPos(predictionTime).x, p.s().x, 0.0);
	EXPECT_NEAR(actPos(predictionTime).y, p.s().y, 0.0);
	EXPECT_NEAR(actPos(predictionTime).z, p.s().z, 0.0);

	predictionTime = 9.0;
	p = ah.pred(predictionTime);
	EXPECT_NEAR(actPos(predictionTime).x, p.s().x, 0.00000000001);
	EXPECT_NEAR(actPos(predictionTime).y, p.s().y, 0.00000000001);
	EXPECT_NEAR(actPos(predictionTime).z, p.s().z, 0.00000000001);

	//EXPECT_NEAR(1.0, p.getSecond().x(), 0.0);
	//EXPECT_NEAR(1.0, p.getSecond().y(), 0.0);
	//EXPECT_NEAR(1.0, p.getSecond().z(), 0.0);
}

TEST_F(AircraftStateTest, testMore) {
	AircraftState aSt = AircraftState("TestState");
	pos1 = Position::makeLatLonAlt(0,-20.0, 10000.0);
	vel1 = Velocity::mkTrkGsVs(Units::from("deg",120),Units::from("kn",400),0.0);
	vel2 = Velocity::mkTrkGsVs(Units::from("deg",121),Units::from("kn",400),0.0);
	vel3 = Velocity::mkTrkGsVs(Units::from("deg",122),Units::from("kn",400),0.0);
	vel4 = Velocity::mkTrkGsVs(Units::from("deg",123),Units::from("kn",400),0.0);
	vel5 = Velocity::mkTrkGsVs(Units::from("deg",124),Units::from("kn",400),0.0);
	Velocity vel6 = Velocity::mkTrkGsVs(Units::from("deg",125),Units::from("kn",400),0.0);
	Velocity vel7 = Velocity::mkTrkGsVs(Units::from("deg",126),Units::from("kn",400),0.0);
	aSt.add(pos1,vel1,1.0);
	aSt.add(pos1,vel1,2.0);
	aSt.add(pos1,vel2,3.0);
	aSt.add(pos1,vel3,4.0);
	aSt.add(pos1,vel4,5.0);
	aSt.add(pos1,vel5,6.0);
	aSt.add(pos1,vel6,7.0);
	aSt.add(pos1,vel7,8.0);
	EXPECT_EQ(8, aSt.size());
	EuclideanProjection sp = Projection::createProjection(pos1);
	aSt.setProjection(sp);
	double trkRate = aSt.avgTrackRate(3);
	EXPECT_NEAR(Units::from("deg/s",1.00),trkRate,0.001);
	trkRate = aSt.avgTrackRate(4);
	EXPECT_NEAR(Units::from("deg/s",1.00),trkRate,0.001);
	trkRate = aSt.avgTrackRate(5);
	EXPECT_NEAR(Units::from("deg/s",1.00),trkRate,0.001);
	trkRate = aSt.avgTrackRate(6);
	EXPECT_NEAR(Units::from("deg/s",1.00),trkRate,0.001);
	Velocity vel8 = Velocity::mkTrkGsVs(Units::from("deg",128),Units::from("kn",400),0.0);
	double tmLastZero = aSt.timeLastZeroTrackRate();
	EXPECT_NEAR(2.0,tmLastZero,0.001);
	tmLastZero = aSt.lastStraightTime();
	EXPECT_NEAR(2.0,tmLastZero,0.001);
	aSt.add(pos1,vel8,9.0);
	trkRate = aSt.avgTrackRate(6);
	EXPECT_NEAR(Units::from("deg/s",1.20),trkRate,0.001);
	trkRate = aSt.avgTrackRate(4);
	EXPECT_NEAR(Units::from("deg/s",1.33),trkRate,0.001);
	trkRate = aSt.avgTrackRate(2);
	EXPECT_NEAR(Units::from("deg/s",2.00),trkRate,0.001);
	EXPECT_NEAR(aSt.trackRate(aSt.size()-1),trkRate,0.001);
	tmLastZero = aSt.timeLastZeroTrackRate();
	EXPECT_NEAR(2.0,tmLastZero,0.001);
	tmLastZero = aSt.lastStraightTime();
	EXPECT_NEAR(2.0,tmLastZero,0.001);
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

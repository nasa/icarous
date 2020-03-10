/*
 * Copyright (c) 2014-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "LatLonAlt.h"
#include "TcpData.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "TrajTemplates.h"
#include "format.h"
#include <gtest/gtest.h>
#include "VectFuns.h"
#include "PlanUtil.h"
#include "TrajGen.h"
#include "GsPlan.h"
#include "TurnGeneration.h"
#include "DebugSupport.h"
#include "PositionUtil.h"
#include "KinematicsPosition.h"
#include "PlanIO.h"

using namespace larcfm;


class TrajGenExamples : public ::testing::Test {

public:
     bool showOutput;

protected:
	virtual void SetUp() {
		showOutput = false;
	}
};


TEST_F(TrajGenExamples, trajGen_ex1) {
	// NOTE: EUTL contains all data internally as SI units.  Most methods operate on SI units assumption

	// This section shows an example of something that a trajectory profile generator would have to produce.
	// TrajGen accepts a linear Plan as an input, with encoded groundspeed and vertical speed profiles,
	// and produces a kinematic Plan for that trajectory profile.  This example illustrates a simple
	// example but beware that a TG profile generator requires special heuristics to ensure that the input
	// to TrajGen is realistic and feasible.

	// The simple trajectory profile we will try to encode will be:
	//      p0 ---------------- p1 ---------------- p2 ---------------- p3
	// Alt: 0 ft			  2000 ft			 2000 ft				0 ft
	// VS:  "     1000 ft/min 	"     1000 ft/min	"	 -1000 ft/min
	// GS:  "       80 knots	"	   130 knots	"		80 knots
	// TRK: "      050 deg		"	   120 deg		"	   020 deg

	std::string flightID = "EX1";						// this plan's ID; typically also the flight ID
	Plan linearProfile(flightID);		// create an empty EUTL Plan to contain the linear trajectory profile
	double startTime = 0.0;							// desired trajectory start time

	//fpln(" HERE I AM  trajGen_ex1 111");

	// define a few parameters first
	double startAltitude = Units::from("ft", 0.0);
	double endAltitude = Units::from("ft", 0.0);
	double climbVS = Units::from("fpm", 1000.0);		// climb vertical speed note the Units class used to convert to internal units
	double climbGS = Units::from("kts", 80.0);		// climb ground speed
	double climbTrk = Units::from("deg", 50.0);		// track angle during the climb
	double cruiseAlt = Units::from("ft", 2000.0);	// desired cruise altitude
	double cruiseTrk = Units::from("deg", 120.0);	// track angle during the cruise
	double cruiseGS = Units::from("kts", 130.0);		// cruise ground speed
	double descentVS = Units::from("fpm", -1000.0);	// descent vertical speed note the Units class used to convert to internal units
	double descentGS = Units::from("kts", 80.0);		// descent ground speed
	double descentTrk = Units::from("deg", 20.0);	// track angle during the descent

	// this is how you define a position of a point
	Position p0  = Position::makeLatLonAlt(32.6, -97.5, startAltitude);	// note the units of makeLatLonAlt vs mkLatLonAlt
	NavPoint np0(p0, startTime,"KABC");	// create a navpoint, which also has a time component and a name field
	linearProfile.addNavPoint(np0);									// add this simple point to the linear profile

	// Let's compute a top-of-climb point to some desired altitude
	double climbDt = (cruiseAlt - startAltitude)/climbVS;			// the linear time required to reach cruise altitude
	double climbRange = climbGS * climbDt;							// the linear ground-based distance required to reach cruise altitude
	LatLonAlt lla1 = GreatCircle::linear_initial(p0.lla(), climbTrk, climbRange);	// use Great Circle to get the location
	lla1 = lla1.mkAlt(cruiseAlt);									// set the altitude at this point (careful with make* versus mk* methods)
	Position p1(lla1);								// create a new position object for point 1
	// add the point p1 to the linear profile, but add label information
	NavPoint np1(p1, startTime + climbDt);			// this NavPoint at the climb time

	TcpData tcp1;
	tcp1.setInformation("<TOC>");			// create TcpData object, which contains a place for information labels
	linearProfile.add(np1, tcp1);									// add the TOC point

	// compute a top-of-descent point at some arbitrary distance
	// Note: typically, TOD point needs to be computed in reverse from the destination point
	double cruiseRange = Units::from("NM", 10.0);					// assume a cruise distance
	double cruiseDt = cruiseRange/cruiseGS;							// the cruise time
	LatLonAlt lla2 = GreatCircle::linear_initial(p1.lla(), cruiseTrk, cruiseRange);	// use Great Circle to get the location
	lla2 = lla2.mkAlt(cruiseAlt);									// set the altitude at this point
	Position p2(lla2);								// create a new position object for point 2
	// add the point p1 to the linear profile, but add label information
	NavPoint np2(p2, startTime + climbDt + cruiseDt);// this NavPoint at the end of cruise
	TcpData tcp2;
	tcp2.setInformation("<TOD>");			// create TcpData object, which contains a place for information labels
	linearProfile.add(np2, tcp2);									// add the TOC point

	// Compute the end point (note: typically this is a constraint; here it is a function of other parameters for the sake of example)
	double descentDt = (cruiseAlt - endAltitude)/std::abs(descentVS);	// the linear time required to descend from cruise altitude
	double descentRange = descentGS * descentDt;					// the linear ground-based distance required to descend from cruise altitude
	LatLonAlt lla3 = GreatCircle::linear_initial(p2.lla(), descentTrk, descentRange);	// use Great Circle to get the location
	lla3 = lla3.mkAlt(endAltitude);									// set the altitude at this point
	Position p3(lla3);								// create a new position object for point 3
	NavPoint np3(p3, startTime + climbDt + cruiseDt + descentDt,"KXYZ");
	linearProfile.addNavPoint(np3);									// add the destination point

	// let's print to the console this linear trajectory profile to ensure it has the characteristics we desire
	if (showOutput) fpln("LINEAR TRAJECTORY PROFILE FOR " + flightID + ": " + linearProfile.toString());


	// NOW: use TrajGen to create a kinematic trajectory for this profile
	double bankAngle = Units::from("deg", 20.0);  // performance parameter: assumed bank angle for turn geometries
	double gsAccel = Units::from("G", 0.1);		  // performance parameter: groundspeed acceleration (specified in "G's" here for example)
	double vsAccel = Units::from("G", 0.1);		  // performance parameter: vertical speed acceleration (specified in "G's" here for example)
	bool repairTurn = false;				      // typically false; tries to repair infeasible turns; see TrajGen information
	bool repairGs = true;						  // typically true; tries to repair infeasible ground speed segments; see TrajGen information
	bool repairVs = false;						  // typically false; tries to repair infeasible vertical speed transitions; see TrajGen information
	Plan kinematicPlan = TrajGen::makeKinematicPlan(linearProfile, bankAngle, gsAccel, vsAccel, repairTurn, repairGs, repairVs);

	// let's print the details of this kinematic plan
	if (showOutput) fpln("KINEMATIC TRAJECTORY PROFILE FOR " + flightID + ": " + kinematicPlan.toStringGs());

	// NOTES: the above kinematic plan start from some non-zero groundspeed and some non-zero vertical rate.
	// There are advanced ways to specify a linear profile that creates trajectories that start and end at
	// zero ground and vertical speeds.


	// HERE is how you use the EUTL library to extract information out of a kinematic plan
	double t_arbitrary = 99.0;
	Position pos = kinematicPlan.position(t_arbitrary);	// the position at this time
	Velocity vel = kinematicPlan.velocity(t_arbitrary);	// the velocity at this time
	// their components
	double lat = pos.lat();					// latitude at this time (radians)
	double latd = pos.latitude();			// latitude at this time (deg)
	double lon = pos.lon();					// longitude at this time (radians)
	double lond = pos.longitude();			// longitude at this time (deg)
	double alt = pos.alt();					// altitude at this time (m)
	double altFt = pos.altitude();			// altitude at this time (ft)
	double gs = vel.gs();					// groundspeed at this time (m/s)
	double gsKts = vel.groundSpeed("kts");	// groundspeed at this time (kts)
	double vs = vel.vs();					// vertical speed at this time (m/s)
	double vsFpm = vel.verticalSpeed("fpm");// vertical speed at this time (ft/min)
	double trk = vel.trk();					// track angle at this time (radians)
	double trkDeg = vel.track("deg");		// track angle at this time (degrees)


	// here is an example sampling of the given plan for an array of times (5 second increments)
	if (showOutput) fpln("\n\n TIME SAMPLING OF KINEMATIC PLAN FOR " + flightID + ": \n");
	if (showOutput) fpln("  time [s]	lat [deg]	lon [deg]	alt [ft]	gs [kts]	vs [ft/min]	trk [deg]");
	if (showOutput) fpln("  ____________________________________________________________________________________________________________");
	for (double t = startTime; t < kinematicPlan.getLastTime(); t += 5.0) {
		pos = kinematicPlan.position(t);	// the position at this time
		vel = kinematicPlan.velocity(t);	// the velocity at this time
		latd = pos.latitude();			// latitude at this time (deg)
		lond = pos.longitude();			// longitude at this time (deg)
		altFt = pos.altitude();			// altitude at this time (ft)
		gsKts = vel.groundSpeed("kts");	// groundspeed at this time (kts)
		vsFpm = vel.verticalSpeed("fpm");// vertical speed at this time (ft/min)
		trkDeg = vel.track("deg");		// track angle at this time (degrees)
		if (showOutput) fpln("  " + Fm1(t) + "\t\t" + Fm6(latd) + "\t" + Fm6(lond) + "\t" + Fm1(altFt)
				             + "\t\t" + Fm1(gsKts) + "\t\t" + Fm1(vsFpm) + "\t\t" + Fm1(trkDeg)  );
	}


	// These are the kinds of checks that should be made on a kinematic plan after the use of TrajGen to verify successful
	// kinematic trajectory generation (Note that I have unit test assertions here as an example)
	EXPECT_FALSE(kinematicPlan.isLinear());				// the resulting trajectory should be kinematic, not linear
	EXPECT_FALSE(kinematicPlan.hasError());				// there were no kinematic generation errors
	EXPECT_TRUE(kinematicPlan.isConsistent(false));		// checks for reasonable accelerations
	EXPECT_TRUE(kinematicPlan.isVelocityContinuous(false));	// check that the velocity profile is continuous throughout the plan

	// THINGS TO NOTE:
	// - Note that, due to the introduction of the kinematic accelerations, the climb and descent vertical speeds are
	//		slightly different that the linear plan vertical speeds
	// - Note that, due to the introduction of the acceleration zones, the times at the major points (e.g., TOC, TOD)
	// 		have been changed
}

TEST_F(TrajGenExamples, basic_plan_methods) {
	// Create a simple straight plan (geodesic)
	Plan lpc("NAME");
	Position p0(LatLonAlt::make(34.320, -117.631, 11000));
	double track = Units::from("deg",33.1);
	Position p1  = p0.linearDist2D(track,4000);
	Position p2  = p1.linearDist2D(track,8000);
	Position p3  = p1.linearDist2D(track,12000);
	NavPoint np0(p0,0,"P0");    	  lpc.addNavPoint(np0);
	NavPoint np1(p1,30);    	      lpc.addNavPoint(np1);
	NavPoint np2(p2,100);          lpc.addNavPoint(np2);
	NavPoint np3(p3,140,"P4");     lpc.addNavPoint(np3);
	// make ground speeds constant
	double gs = Units::from("kn",220);
	lpc.mkGsConstant(1,lpc.size()-1,gs);
	//fpln("$$ basic_plan_methods: lpc = "+lpc.toStringGs());
	// generate a kinematic trajectory from a linear plan
	double bankAngle = Units::from("deg", 20.0);	// performance parameter: assumed bank angle for turn geometries
	double gsAccel   = Units::from("G", 0.1);		// performance parameter: ground speed acceleration (specified in "G's" here for example)
	double vsAccel   = Units::from("G", 0.1);	    // performance parameter: vertical speed acceleration (specified in "G's" here for example)
	bool repairTurn = false;						// typically false; tries to repair infeasible turns; see TrajGen information
	bool repairGs = true;						    // typically true; tries to repair infeasible ground speed segments; see TrajGen information
	bool repairVs = false;						    // typically false; tries to repair infeasible vertical speed transitions; see TrajGen information
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repairTurn, repairGs, repairVs);
	//fpln("$$ basic_plan_methods: kpc = "+kpc.toStringGs());
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isConsistent());
	EXPECT_TRUE(kpc.isVelocityContinuous(false));
	// change altitudes
	double newAlt = Units::from("ft",14000);
	lpc.mkAlt(1,newAlt);
	lpc.mkAlt(2,newAlt);
	lpc.mkAlt(3,newAlt);
	//fpln("$$ basic_plan_methods: lpc = "+lpc.toStringVs());
	// generate another kinematic trajectory from a linear plan
	Plan kpc2 = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel);
	//fpln("$$ basic_plan_methods: kpc2 = "+kpc2.toStringVs());
	EXPECT_FALSE(kpc2.hasError());
	EXPECT_TRUE(kpc2.isConsistent());
	EXPECT_TRUE(kpc2.isVelocityContinuous(false));
	// extra info from a plan
	int ixBGS = kpc2.nextBGS(0);
	NavPoint np_BGS = kpc2.point(ixBGS);
	//fpln("$$ basic_plan_methods: ixBGS = "+Fm0(ixBGS)+" np_BGS = "+np_BGS.toString());
	double gs_BGS = kpc2.gsOut(ixBGS);
	double vs_BGS = kpc2.vsOut(ixBGS);
	double t_BGS  = kpc2.time(ixBGS);
	EXPECT_NEAR(Units::from("kn",259.1793),gs_BGS,0.001);
	EXPECT_NEAR(Units::from("fpm",3000.00),vs_BGS,0.001);
	EXPECT_NEAR(Units::from("s",30.00),t_BGS,0.001);
	int ixEGS = kpc2.nextEGS(ixBGS);
	double alt_EGS = kpc2.alt(ixEGS);
	double distBGS_EGS = kpc2.pathDistance(ixBGS,ixEGS);
	EXPECT_NEAR(4267.2,alt_EGS,0.001);
	EXPECT_NEAR(2533.2648,distBGS_EGS,0.001);
	double currentTime = kpc.getFirstTime();
	double advanceDist = Units::from("NM",1.0);
	// some PlanUtil methods
    Position posFromDist = PlanUtil::positionFromDistance(kpc2, currentTime, advanceDist, false).first;
	//fpln("$$ basic_plan_methods: posFromDist = "+posFromDist.toString());
	NavPoint cp = kpc2.closestPoint(posFromDist);
	//fpln("$$ basic_plan_methods: posFromDist = "+posFromDist.toString()+" cp = "+cp.toString());
	EXPECT_NEAR(0.0,posFromDist.distanceH(cp.position()),0.00001);   // posFromDist is on trajectory
	// dump file
	PlanIO::savePlan(kpc, "test_kpcFile");
	// create linear plan from a kinematic plan
	Plan reverted = PlanUtil::revertAllTCPs(kpc);
	//fpln("$$ basic_plan_methods: lpc = "+lpc.toString());
	PlanUtil::checkReversion(reverted,lpc);
}

TEST_F(TrajGenExamples, basic_plan_methods_eucl) {
	Plan lpc("EUCL");
	Position p0  = Position::makeXYZ(100.0, 100.0, 5000.0);
	Position p1  = Position::makeXYZ(100.0, 300.0, 15000.0);
	Position p2  = Position::makeXYZ(200.0, 500.0, 15000.0);
	Position p3  = Position::makeXYZ(400.0, 600.0, 5000.0);
	lpc.addNavPoint(NavPoint(p0,100.0));
	lpc.addNavPoint(NavPoint(p1,1750.0));
	lpc.addNavPoint(NavPoint(p2,3800.0));
	lpc.addNavPoint(NavPoint(p3,5400.0));
	//fpln("$$ basic_plan_methods_eucl: lpc = "+lpc.toStringGs());
	// make ground speeds constant
	lpc.mkGsConstant(Units::from("kn",180.0));
	//fpln("$$ basic_plan_methods_eucl: lpc = "+lpc.toString());
	// write linear plan to a file
	PlanIO::savePlan(lpc, "test_lpcFile");
	// generate a kinematic trajectory from a linear plan
	double bankAngle = Units::from("deg", 25.0);
	double gsAccel   = Units::from("m/s^2", 2.0);
	double vsAccel   = Units::from("m/s^2", 1.8);
	Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel);
	//fpln("$$ basic_plan_methods_eucl: kpc = "+kpc.toString());
	EXPECT_FALSE(kpc.hasError());
	EXPECT_TRUE(kpc.isConsistent());
	EXPECT_TRUE(kpc.isVelocityContinuous(false));
	// find info about turns
	int ixBOT = kpc.nextBOT(0);
	EXPECT_EQ(1,ixBOT);
	double turnRadius = kpc.signedRadius(ixBOT);      // turn radius is stored in BOT
	EXPECT_NEAR(1875.118,turnRadius,0.001);
	Position turnCenter = kpc.turnCenter(ixBOT);
	//fpln("$$ basic_plan_methods_eucl:  turnCenter = "+turnCenter.toString());
	int ixEOT = kpc.nextEOT(ixBOT);
	double distBOT_EOT = kpc.pathDistance(ixBOT,ixEOT);    // circular distance
	EXPECT_NEAR(869.394,distBOT_EOT,0.001);
	double directDist = kpc.point(ixBOT).distanceH(kpc.point(ixEOT));
	EXPECT_NEAR(861.6279,directDist,0.001);
}




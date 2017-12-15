/*
u * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Plan.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "Velocity.h"
#include "format.h"
#include "Util.h"
#include "Constants.h"
#include "string_util.h"
#include "TrajGen.h"
#include "TrajTemplates.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;

	const double gsAccel = 2;
	const double vsAccel = 1;
	const double bankAngle = Units::from("deg", 10);

	
	Plan TrajTemplates::makeLPC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt) {
		Plan pc;
		double dist = GreatCircle::distance(start,end);
		double flightTime = dist/gs;
		Velocity v = GreatCircle::velocity_initial(start, end, flightTime);
		NavPoint np1(Position(start),0.0);
		NavPoint np2(Position(end), flightTime);
		pc.addNavPoint(np1);
		pc.addNavPoint(np2);
		double climbTime = std::abs((cruiseAlt-start.alt())/vs);
		LatLonAlt lla3 = GreatCircle::linear_gcgs(start, end, v, climbTime).mkAlt(cruiseAlt);
		NavPoint np3(Position(lla3),climbTime);
		pc.addNavPoint(np3);
		double descentTime = std::abs((cruiseAlt-end.alt())/vs);
		LatLonAlt lla4 = GreatCircle::linear_gcgs(start, end, v, flightTime-descentTime).mkAlt(cruiseAlt);
		NavPoint np4(Position(lla4), flightTime-descentTime);
		pc.addNavPoint(np4);
		//fpln(" makeLPC: pc = "+pc.toOutput());
		return pc;
	}
	
	Plan TrajTemplates::makeKPC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt) {
		Plan lpc = makeLPC(start,end,gs,vs,cruiseAlt);
		bool allowSmoothing = true;
		Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, allowSmoothing);
		//fpln(" makeKPC: kpc = \n"+kpc.toOutput());
        return kpc;
	}
	
	Plan TrajTemplates::makeLPC_Turn(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt) {
		Plan pc;
		LatLonAlt mid = GreatCircle::interpolate(start,end,0.5).mkAlt(cruiseAlt);
		double trk_mid = GreatCircle::initial_course(mid,end);
		LatLonAlt midPerp = GreatCircle::linear_initial(mid, trk_mid+M_PI/2,Units::from("nm",20.0));
		double dist = GreatCircle::distance(start,midPerp)+GreatCircle::distance(midPerp,end);
		double flightTime = dist/gs;
		NavPoint npMid(Position(midPerp),flightTime/2.0);
		NavPoint np1(Position(start),0.0);
		NavPoint np2(Position(end), flightTime);
		pc.addNavPoint(np1);
		pc.addNavPoint(np2);
		pc.addNavPoint(npMid);
		double climbTime = std::abs((cruiseAlt-start.alt())/vs);
		Velocity v = GreatCircle::velocity_average(start, midPerp, flightTime/2);
		LatLonAlt lla3 = GreatCircle::linear_gcgs(start, midPerp, v, climbTime).mkAlt(cruiseAlt);
		NavPoint np3(Position(lla3),climbTime);
		pc.addNavPoint(np3);
		double descentTime = std::abs((cruiseAlt-end.alt())/vs);
		//fpln(" flightTime = "+flightTime+" descentTime = "+descentTime);
		v = GreatCircle::velocity_average(midPerp, end, flightTime/2);
		LatLonAlt lla4 = GreatCircle::linear_gcgs(midPerp, end, v, flightTime/2-descentTime).mkAlt(cruiseAlt);
		NavPoint np4(Position(lla4), flightTime-descentTime);
		pc.addNavPoint(np4);
		//fpln(" makeLPC: pc = -------------------\n"+pc.toOutput());
		return pc;
	}

	Plan TrajTemplates::makeKPC_Turn(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt) {
		Plan lpc = makeLPC_Turn(start,end,gs,vs,cruiseAlt);
		bool allowSmoothing = true;
		Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, allowSmoothing);
		//fpln(kpc.toOutput());
        return kpc;
	}
	
	Plan TrajTemplates::makeLPC_FLC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs) {
		Plan pc;
		double dist = GreatCircle::distance(start,end);
		double flightTime = dist/gs;
		Velocity v = GreatCircle::velocity_initial(start, end, flightTime);
		NavPoint np1(Position(start),0.0);
		NavPoint np2(Position(end), flightTime);
		pc.addNavPoint(np1);
		pc.addNavPoint(np2);
		LatLonAlt lla3 = GreatCircle::linear_gcgs(start, end, v, flightTime/2.0).mkAlt(start.alt());
		NavPoint np3(Position(lla3),flightTime/2.0);
		pc.addNavPoint(np3);
		double FLC_Time = std::abs((end.alt()-start.alt())/vs);
		LatLonAlt lla4 = GreatCircle::linear_gcgs(lla3, end, v, FLC_Time).mkAlt(end.alt());
		NavPoint np4(Position(lla4), flightTime/2.0+FLC_Time);
		pc.addNavPoint(np4);
		//fpln(" makeLPC: pc = "+pc.toOutput());
		return pc;
	}

	Plan TrajTemplates::makeKPC_FLC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs) {
		Plan lpc = makeLPC_FLC(start,end,gs,vs);
		fpln(" makeKPC_FLC: lpc = "+lpc.toOutput());
		bool allowSmoothing = true;
		Plan kpc = TrajGen::makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, allowSmoothing);
		//fpln(kpc.toOutput());
        return kpc;
	}

	/* Take a linear PlanCore and assume first and last waypoint are airports.
	 * First make the altitude of those waypoints 0.
	 * Then add a top of climb point and beginning of descent point.
	 * Also add level sections at beginning and end
	 */
	Plan TrajTemplates::addClimbDescent(Plan lpc, double gs, double vs, double cruiseAlt) {
		Plan npc(lpc);
		npc.setAlt(0,0);
		npc.setAlt(npc.size()-1,0);

		NavPoint startNP = npc.point(0);
		NavPoint endNP = npc.point(npc.size()-1);
	
		LatLonAlt start = startNP.lla();		
		LatLonAlt end = endNP.lla();
		double climbTime = std::abs((cruiseAlt-start.alt())/vs);
		
		double np3Time = npc.getFirstTime()+climbTime;
		NavPoint np3(npc.position(np3Time).mkAlt(cruiseAlt),np3Time);
		npc.addNavPoint(np3);
		
		double descentTime = std::abs((cruiseAlt-end.alt())/vs);
		double np4Time = npc.getLastTime()-descentTime;
		NavPoint np4(npc.position(np4Time).mkAlt(cruiseAlt),np4Time);
		npc.addNavPoint(np4);
		if (climbTime > 10.0) {  // add lead in
			double np5Time = npc.getFirstTime()+10.0;
			NavPoint np5(npc.position(np5Time).mkAlt(0.0),np5Time);
			npc.addNavPoint(np5);
		}
		if (descentTime > 10.0) {
			double np6Time = npc.getLastTime()-10.0;
			NavPoint np6(npc.position(np6Time).mkAlt(0.0),np6Time);
			npc.addNavPoint(np6);
		}
		//fpln(" $$$$$ addClimbDescent5: npc = "+npc);
		return npc;
	}
	



}

/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.GreatCircle;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.LatLonAlt;
import gov.nasa.larcfm.Util.NavPoint;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.TrajGen;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

/**
 * Trajectory generation functionality.  This class translates between Linear and Kinematic plans.  Note that the translations are 
 * not currently robust.
 * 
 * Note: there are several global variables that are settable by the user and modify low-level aspects of the transformation:
 * trajKinematicsTrack: allow track transformations (default true, if false, no turn TCPs will be generated)
 * trajKinematicsGS: allow gs transformations (default true, if false, no ground speed TCPs will be generated)
 * trajKinematicsVS: allow vs transformations (default true, if false, no vertical speed TCPs will be generated)
 * trajPreserveGS: prioritize preserving grounds speeds (default FALSE. If true, times may change, if false point times should be preserved)
 * trajAccelerationReductionAllowed: if true, if there are vs end points that nearly overlap with existing points, allow the acceleration to be adjusted so that they actually do overlap or are sufficiently distinct that there will not be problems inferring the velocities between them. (default true)
 * 
 * These values may be set through setter methods.
 * 
 */
public class TrajTemplates {

	static double gsAccel = 2;
	static double vsAccel = 1;		
	static double bankAngle = Units.from("deg", 10);

	
	public static Plan makeLPC(LatLonAlt start, LatLonAlt end, double gs, double vs, double cruiseAlt) {
		Plan pc = new Plan();
		double dist = GreatCircle.distance(start,end);
		double flightTime = dist/gs;
		Velocity v = GreatCircle.velocity_initial(start, end, flightTime);
		NavPoint np1 = new NavPoint(new Position(start),0.0);
		NavPoint np2 = new NavPoint(new Position(end), flightTime);
		pc.addNavPoint(np1);
		pc.addNavPoint(np2);
		double climbTime = Math.abs((cruiseAlt-start.alt())/vs);
		LatLonAlt lla3 = GreatCircle.linear_gcgs(start, end, v, climbTime).mkAlt(cruiseAlt);
		NavPoint np3 = new NavPoint(new Position(lla3),climbTime);
		pc.addNavPoint(np3);
		double descentTime = Math.abs((cruiseAlt-end.alt())/vs);
		LatLonAlt lla4 = GreatCircle.linear_gcgs(start, end, v, flightTime-descentTime).mkAlt(cruiseAlt);
		NavPoint np4 = new NavPoint(new Position(lla4), flightTime-descentTime);
		pc.addNavPoint(np4);
		//f.pln(" makeLPC: pc = "+pc.toOutput());
		return pc;
	}
	
	public static Plan makeKPC(LatLonAlt start, LatLonAlt end, double gs, double vs, double cruiseAlt) {
		Plan lpc = makeLPC(start,end,gs,vs,cruiseAlt);
		boolean allowSmoothing = true;
		Plan kpc = TrajGen.makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, allowSmoothing);
		//f.pln(" makeKPC: kpc = \n"+kpc.toOutput());
        return kpc;
	}
	
	public 	static Plan makeLPC_Turn(LatLonAlt start, LatLonAlt end, double gs, double vs, double cruiseAlt) {
		Plan pc = new Plan();
		LatLonAlt mid = GreatCircle.interpolate(start,end,0.5).mkAlt(cruiseAlt);
    	double trk_mid = GreatCircle.initial_course(mid,end);
    	LatLonAlt midPerp = GreatCircle.linear_initial(mid, trk_mid+Math.PI/2,Units.from("nm",20.0)); 
		double dist = GreatCircle.distance(start,midPerp)+GreatCircle.distance(midPerp,end);
		double flightTime = dist/gs;
		NavPoint npMid = new NavPoint(new Position(midPerp),flightTime/2.0);
		NavPoint np1 = new NavPoint(new Position(start),0.0);
		NavPoint np2 = new NavPoint(new Position(end), flightTime);
		pc.addNavPoint(np1);
		pc.addNavPoint(np2);
		pc.addNavPoint(npMid);
		double climbTime = Math.abs((cruiseAlt-start.alt())/vs);
		Velocity v = GreatCircle.velocity_average(start, midPerp, flightTime/2);
		LatLonAlt lla3 = GreatCircle.linear_gcgs(start, midPerp, v, climbTime).mkAlt(cruiseAlt);
		NavPoint np3 = new NavPoint(new Position(lla3),climbTime);
		pc.addNavPoint(np3);
		double descentTime = Math.abs((cruiseAlt-end.alt())/vs);
		//f.pln(" flightTime = "+flightTime+" descentTime = "+descentTime);
		v = GreatCircle.velocity_average(midPerp, end, flightTime/2);
		LatLonAlt lla4 = GreatCircle.linear_gcgs(midPerp, end, v, flightTime/2-descentTime).mkAlt(cruiseAlt);
		NavPoint np4 = new NavPoint(new Position(lla4), flightTime-descentTime);
		pc.addNavPoint(np4);
		//f.pln(" makeLPC: pc = -------------------\n"+pc.toOutput());
		return pc;
	}

	public static Plan makeKPC_Turn(LatLonAlt start, LatLonAlt end, double gs, double vs, double cruiseAlt) {
		Plan lpc = makeLPC_Turn(start,end,gs,vs,cruiseAlt);
		boolean repair = true;
		Plan kpc = TrajGen.makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, repair);
		//f.pln(kpc.toOutput());
        return kpc;
	}
	
//	public static Plan makeKPC_Turn_Over(LatLonAlt start, LatLonAlt end, double gs, double vs, double cruiseAlt) {
//		Plan lpc = makeLPC_Turn(start,end,gs,vs,cruiseAlt);
//		boolean repair = true;
//		Plan kpc = TrajGen.makeKinPlanFlyOver(lpc, bankAngle, gsAccel, vsAccel, repair);
//		//f.pln(kpc.toOutput());
//        return kpc;
//	}

	
	public static Plan makeLPC_FLC(LatLonAlt start, LatLonAlt end, double gs, double vs) {
		Plan pc = new Plan();
		double dist = GreatCircle.distance(start,end);
		double flightTime = dist/gs;
		Velocity v = GreatCircle.velocity_initial(start, end, flightTime);
		NavPoint np1 = new NavPoint(new Position(start),0.0);
		NavPoint np2 = new NavPoint(new Position(end), flightTime);
		pc.addNavPoint(np1);
		pc.addNavPoint(np2);
		LatLonAlt lla3 = GreatCircle.linear_gcgs(start, end, v, flightTime/2.0).mkAlt(start.alt());
		NavPoint np3 = new NavPoint(new Position(lla3),flightTime/2.0);
		pc.addNavPoint(np3);
		double FLC_Time = Math.abs((end.alt()-start.alt())/vs);
		LatLonAlt lla4 = GreatCircle.linear_gcgs(lla3, end, v, FLC_Time).mkAlt(end.alt());
		NavPoint np4 = new NavPoint(new Position(lla4), flightTime/2.0+FLC_Time);
		pc.addNavPoint(np4);
		//f.pln(" makeLPC: pc = "+pc.toOutput());
		return pc;
	}

	public static Plan makeKPC_FLC(LatLonAlt start, LatLonAlt end, double gs, double vs) {
		Plan lpc = makeLPC_FLC(start,end,gs,vs);
		f.pln(" makeKPC_FLC: lpc = "+lpc.toOutput());
		boolean allowSmoothing = true;
		Plan kpc = TrajGen.makeKinematicPlan(lpc, bankAngle, gsAccel, vsAccel, allowSmoothing);
		//f.pln(kpc.toOutput());
        return kpc;
	}

	/* Take a linear PlanCore and assume first and last waypoint are airports.
	 * First make the altitude of those waypoints 0.
	 * Then add a top of climb point and beginning of descent point.
	 * Also add level sections at beginning and end
	 */
	public static Plan addClimbDescent(Plan lpc, double gs, double vs, double cruiseAlt) {
		Plan npc = new Plan(lpc);
		npc.setAlt(0,0);
		npc.setAlt(npc.size()-1,0);

		NavPoint startNP = npc.point(0);
		NavPoint endNP = npc.point(npc.size()-1);
	
		LatLonAlt start = startNP.lla();		
		LatLonAlt end = endNP.lla();
		double climbTime = Math.abs((cruiseAlt-start.alt())/vs);
		
		double np3Time = npc.getFirstTime()+climbTime;
		NavPoint np3 = new NavPoint(npc.position(np3Time).mkAlt(cruiseAlt),np3Time);
		npc.addNavPoint(np3);
		
		double descentTime = Math.abs((cruiseAlt-end.alt())/vs);
		double np4Time = npc.getLastTime()-descentTime;
		NavPoint np4 = new NavPoint(npc.position(np4Time).mkAlt(cruiseAlt),np4Time);
		npc.addNavPoint(np4);
		if (climbTime > 10.0) {  // add lead in
			double np5Time = npc.getFirstTime()+10.0;
			NavPoint np5 = new NavPoint(npc.position(np5Time).mkAlt(0.0),np5Time);
			npc.addNavPoint(np5);
		}
		if (descentTime > 10.0) {
			double np6Time = npc.getLastTime()-10.0;
			NavPoint np6 = new NavPoint(npc.position(np6Time).mkAlt(0.0),np6Time);
			npc.addNavPoint(np6);
		}
		//f.pln(" $$$$$ addClimbDescent5: npc = "+npc);
		return npc;
	}
	
}

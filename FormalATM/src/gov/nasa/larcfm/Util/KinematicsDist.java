/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.Kinematics;

/**
 * A library of functions to aid the computation of the kinematics of an aircraft.  This
 * library is currently under development and is far from complete.  The majority of the functions
 * handle constant velocity turns and movement with constant ground speed acceleration.<p>
 * 
 * Unless otherwise noted, all kinematics function parameters are in internal units -- angles are in radians,
 * linear speeds are in m/s, distances are in meters, time is in seconds.
 * 
 */
public class KinematicsDist {

	

	/** EXPERIMENTAL -- minimum distance between two aircraft when both turn, compute trajectories up to time stopTime
	 * 
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi   the target velocity of traffic (i.e. after turn maneuver complete)
	 * @param bankAngOwn       the bank angle of the ownship
	 * @param turnRightOwn     the turn direction of ownship
	 * @param bankAngTraf      the bank angle of the traffic
	 * @param turnRightTraf    the turn direction of traffic
	 * @param stopTime         the duration of the turns
	 * @return the minimum distance
	 */
	public static double minDistTrk(Vect3 so, Velocity vo, Velocity nvo, Vect3 si, Velocity vi, Velocity nvi, 
			double bankAngOwn, double bankAngTraf, double stopTime) {
		double minDist = Double.MAX_VALUE;
        double step = 1.0;
		for (double t = 0; t < stopTime; t = t + step) {
			Vect3 soAtTm = Kinematics.turnUntil(so, vo, t, nvo.trk(), bankAngOwn).first;							
			Vect3 siAtTm = Kinematics.turnUntil(si, vi, t, nvi.trk(), bankAngTraf).first;
            double dist = soAtTm.Sub(siAtTm).vect2().norm();
            if (dist < minDist) minDist = dist;
		}
		return minDist;
	}

	
	
	/** Minimum distance between two aircraft when BOTH turn, compute trajectories up to time stopTime
	 * 
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi   the target velocity of traffic (i.e. after turn maneuver complete)
	 * @param bankAngOwn       the bank angle of the ownship
	 * @param turnRightOwn     the turn direction of ownship
	 * @param bankAngTraf      the bank angle of the traffic
	 * @param turnRightTraf    the turn direction of traffic
	 * @return                 minimum distance data packed in a Vect4
	 */
	public static Vect4 minDistBetweenTrk(Vect3 so, Velocity vo, Velocity nvo, Vect3 si, Velocity vi, Velocity nvi, 
			double bankAngOwn, boolean turnRightOwn, double bankAngTraf, boolean turnRightTraf) {
		double minDist = Double.MAX_VALUE;
		double minDistH = Double.MAX_VALUE;
		double minDistV = Double.MAX_VALUE;
        double minT = -1;
		double step = 1.0;
		double stopTime = 1000;   // should be more than enough time to finish turns, safety value in case parallel velocities
		for (double t = 0; t < stopTime; t = t + step) {
			//Vect3 soAtTm = turnUntilPosition(so, vo, nvo.track(), bankAngOwn, t, turnRightOwn);							
			//Vect3 siAtTm = turnUntilPosition(si, vi, nvi.track(), bankAngOwn, t, turnRightTraf);
	        //Velocity vown = turnUntilVelocity(vo, nvo.verticalSpeed(), bankAngOwn, t, turnRightOwn);
	        //Velocity vtraf = turnUntilVelocity(vi, nvi.verticalSpeed(), bankAngTraf, t, turnRightTraf);
			Pair<Vect3,Velocity> psv = Kinematics.turnUntil(so, vo, t, nvo.trk(), bankAngOwn);		
			Vect3 soAtTm = psv.first;
			Velocity vown = psv.second;
			Pair<Vect3,Velocity> psvi = Kinematics.turnUntil(si, vi, t, nvi.trk(), bankAngOwn);
			Vect3 siAtTm = psvi.first;
			Velocity vtraf = psvi.second;
            //f.pln(" $$$$ minDistBetweenTrk: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
			Vect3 s = soAtTm.Sub(siAtTm);
            double dist = s.norm();
            double distH = s.vect2().norm();
            double distV = Math.abs(s.z);
            //f.pln(" $$$$ minDistBetweenTrk: t = "+t+"  dist = "+Units.str("nm",dist));
            if (dist < minDist) {               // compute distances at TCA in 3D
            	minDist = dist;
            	minDistH = distH;
            	minDistV = distV;
            	minT = t;
            }
             boolean divg = s.dot(vown.Sub(vtraf)) > 0;
            if (divg) break;
		}
		return new Vect4(minDistH,minDist,minDistV,minT);
	}
  
	
	
	/** Minimum distance between two aircraft when BOTH aircraft gs accelerate, compute trajectories up to time stopTime
	 * 
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi           target velocity of traffic (i.e. after acceleration maneuver complete)
	 * @param gsAccelOwn    ground speed acceleration of the ownship
	 * @param gsAccelTraf   ground speed acceleration of the traffic
	 * @return                 minimum distance data packed in a Vect4
	 */
	public static Vect4 minDistBetweenGs(Vect3 so, Velocity vo, Velocity nvo, Vect3 si, Velocity vi,  Velocity nvi,
			double gsAccelOwn, double gsAccelTraf) {
		double minDist = Double.MAX_VALUE;
		double minDistH = Double.MAX_VALUE;
		double minDistV = Double.MAX_VALUE;
		//f.pln(" $$$$ minDistBetween: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
        double step = 1.0;
        double minT = -1;
		//f.pln(" $$$$$$$$$$$$$$$$$$$$ minDistBetweenTrk: step = "+step);
		double stopTime = 1000;   // should be more than enough time to finish turns, safety value in case parallel velocities
		for (double t = 0; t < stopTime; t = t + step) {
			Vect3 soAtTm = Kinematics.gsAccelUntil(so, vo, t, nvo.gs(), gsAccelOwn).first;	
			Vect3 siAtTm = Kinematics.gsAccelUntil(si, vi, t, nvi.gs(), gsAccelTraf).first;
			Vect3 s = soAtTm.Sub(siAtTm);
            //f.pln(" $$$$ minDistBetweenTrk: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
            double dist = s.norm();
            double distH = s.vect2().norm();
            double distV = Math.abs(s.z);
            //f.pln(" $$$$ minDistBetweenTrk: dist = "+Units.str("nm",dist));
            if (dist < minDist) {               // compute distances at TCA in 3D
            	minDist = dist;
            	minDistH = distH;
            	minDistV = distV;
            	minT = t;
            }
            Vect3 vown = Kinematics.gsAccelUntil(so, vo, t, nvo.vs(), gsAccelOwn).second;
            Vect3 vtraf = Kinematics.gsAccelUntil(si, vi, t, nvi.vs(), gsAccelTraf).second;
            boolean divg = s.dot(vown.Sub(vtraf)) > 0;
            if (divg) break;
		}
		return new Vect4(minDistH,minDist,minDistV,minT);
	}

	
	/** Minimum distance between two aircraft when BOTH aircraft vs accelerate, compute trajectories up to time stopTime
	 * 
	 * @param so    initial position of ownship
	 * @param vo    initial velocity of ownship
	 * @param nvo   the target velocity of ownship (i.e. after turn maneuver complete)
	 * @param si    initial position of traffic
	 * @param vi    initial velocity of traffic
	 * @param nvi           target velocity of traffic (i.e. after acceleration maneuver complete)
	 * @param vsAccelOwn    vertical speed acceleration of the ownship
	 * @param vsAccelTraf   vertical speed acceleration of the traffic
	 * @return                 minimum distance data packed in a Vect4
	 */
	public static Vect4 minDistBetweenVs(Vect3 so, Velocity vo, Velocity nvo, Vect3 si, Velocity vi,  Velocity nvi,
			double vsAccelOwn, double vsAccelTraf) {
		double minDist = Double.MAX_VALUE;
		double minDistH = Double.MAX_VALUE;
		double minDistV = Double.MAX_VALUE;
        double minT = -1;
		//f.pln(" $$$$ minDistBetweenVs: vo = "+vo+" vi = "+vi+"  nvo = "+nvo+" nvi = "+nvi);
        double step = 1.0;
        double stopTime = 1000;    // should be more than enough time for accelerations to be done
		for (double t = 0; t < stopTime; t = t + step) {
			Vect3 soAtTm = Kinematics.vsAccelUntil(so, vo, t, nvo.vs(), vsAccelOwn).first;	
			Vect3 siAtTm = Kinematics.vsAccelUntil(si, vi, t, nvi.vs(), vsAccelTraf).first;
            //f.pln(" $$$$ minDistBetweenVs: soAtTm = "+f.sStr(soAtTm)+" siAtTm = "+f.sStr(siAtTm));
			Vect3 s = soAtTm.Sub(siAtTm);
            double dist = s.norm();
            double distH = s.vect2().norm();
            double distV = Math.abs(s.z);
            if (dist < minDist) {               // compute distances at TCA in 3D
            	minDist = dist;
            	minDistH = distH;
            	minDistV = distV;
            	minT = t;
            }
            Vect3 vown = Kinematics.vsAccelUntil(so, vo, t, nvo.vs(), vsAccelOwn).second;
            Vect3 vtraf = Kinematics.vsAccelUntil(si,vi, t, nvi.vs(), vsAccelTraf).second;
            boolean divg = s.dot(vown.Sub(vtraf)) > 0;
            if (divg) break;
            //f.pln(" $$$$ minDistBetweenVs: t = "+t+" dist = "+Units.str("nm",dist)+" divg = "+divg);
		}
		return new Vect4(minDistH,minDist,minDistV,minT);
	}

	
}

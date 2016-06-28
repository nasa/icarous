/*
 * PlanUtil - support utility functions for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.NavPoint.Trk_TCPType;

import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeSet;

public class PlanUtil {

	static final double revertGsTurnConnectionTime = 5;  // if a turn segment and GS segment are this close in time, then assume they are related
	
	public static Plan makeSourceNew(Plan lpc) {
		Plan npc = new Plan(lpc.getName(),lpc.getNote());
		for (int i = 0; i < lpc.size(); i++) {
			NavPoint np = lpc.point(i);
			NavPoint npn = np.makeNewPoint();
			npc.add(npn);
		}
		return npc;
	}
	

	/** Write the plan to the given file */
	public static void savePlan(Plan plan, String fileName) {
		if (plan.getName().equals("")) {
			plan.setName("noname");
		}
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		pw.writePlan(plan,true);
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

	/** Write the plans to the given file */
	public static void savePlans(Plan plan1, Plan plan2, String fileName) {
		if (plan1.getName().equals("")) {
			plan1.setName("noname1");
		}
		if (plan2.getName().equals("")) {
			plan2.setName("noname2");
		}
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		pw.writePlan(plan1,true);
		pw.writePlan(plan2,true);
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

	/** Write the plans to the given file */
	public static void savePlans(List<Plan> plans, String fileName) {
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		int count = 0;
		for (Plan plan: plans) {
			if (plan.getName().equals("")) {
				plan.setName("noname"+count);
				count++;
			}
			pw.writePlan(plan,true);
		}
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}
	
	/** Write the plan to the given file */
	public static void savePlanWithHdr(Plan plan, String fileName, String hdr) {
		if (plan.getName().equals("")) {
			plan.setName("noname");
		}
		PlanWriter pw = new PlanWriter();
		pw.setPrecision(12);
		pw.open(fileName);
		pw.writeLn(hdr);
		pw.writePlan(plan,true);
		pw.close();
		if (pw.hasMessage()) {
			f.dln(pw.getMessage());
		}				
	}

	
	private static boolean gsConsistent(int i, NavPoint GSCBegin, NavPoint GSCEnd, double accelEpsilon, double distEpsilon,			
            Velocity vin, Velocity vout, boolean silent) {
		//Velocity vout = p.initialVelocity(p.nextEGS(i));
		boolean rtn = true;
        double dt = GSCEnd.time() - GSCBegin.time();
		double a = GSCBegin.gsAccel();	    
	    double acalc = (vout.gs() - vin.gs())/(dt); 
	    //f.pln("\n\n\n\n ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
		//f.pln("\n $$$ gsConsistent: vin = "+vin+" vout = "+vout+" dt = "+dt+" a = "+a+" acalc = "+acalc);
		if (!Util.within_epsilon(a, acalc, accelEpsilon)) { // 0.00001)) {
			double deltaAccel = Math.abs(a-acalc);
			if (!silent) {
				f.pln(" >>> gsConsistent: GS FAIL! at i = "+i+" GSC section fails ACCEL test! a = "+a+ " acalc = "+acalc);
			}
		    if (deltaAccel > 0.5) f.pln(" gsConsistent: **************************************************** deltaAccel = "+deltaAccel);
			rtn = false;
		}
		double ds = vin.gs()*dt + 0.5*a*dt*dt;
		double distH = 	GSCBegin.position().distanceH(GSCEnd.position());		
		double absDiff = Math.abs(ds-distH);
		//f.pln(" ds = "+Units.str("nm",ds)+ " distH = "+Units.str("nm",distH)+" absDiff = "+Units.str15("m",absDiff));
		if (!Util.within_epsilon(absDiff, distEpsilon)) { // See testGsSimple for worst case 0.07
			if ( ! silent) f.pln(" >>> gsConsistent: GS FAIL! at i = "+i+" GSC section fails Test! absDiff = "+Units.str("m",absDiff));
			if ( ! silent && absDiff > Units.from("m", 10.0)) f.pln(" gsConsistent: **************************************************  distanceH = "+Units.str("m",absDiff,8));
			rtn = false;
		}
		return rtn;
	}

	public static boolean gsConsistent(Plan p, int i, double accelEpsilon, double distEpsilon,	 boolean silent) {
		if ( ! p.point(i).isBGS()) return true;
        //boolean rtn = true;
		NavPoint GSCBegin = p.point(i);
		NavPoint GSCEnd = p.point(p.nextEGS(i));
		Velocity vin = vin = p.point(i).velocityIn();
		//if (i == 0) vin = p.point(i).velocityIn();     // not sure if we should allow TCP as current point ??
		//else vin = p.finalVelocity(i-1);
		Velocity vout = p.initialVelocity(p.nextEGS(i));
		return gsConsistent(i, GSCBegin, GSCEnd, accelEpsilon, distEpsilon, vin, vout, silent);
	}

	
	private static boolean vsConsistent(int i, NavPoint VSCBegin, NavPoint VSCEnd, double accelEpsilon, double distEpsilon,			
            Velocity vin, Velocity vout, boolean silent) {
        boolean rtn = true;
		double dt = VSCEnd.time() - VSCBegin.time();
		double a = VSCBegin.vsAccel();
		// Tests that that stored acceleration matches acceleration calculated from surrounding velocities
		//double acalc = (initialVelocity(i+1).vs()-vo.vs())/t;
		//double acalc = calcVertAccel(i);
	    double acalc = (vout.vs() - vin.vs())/dt; 	
		//f.pln("\n $$$ vsConsistent: vin = "+vin+" vout = "+vout+" dt = "+dt+" a = "+a+" acalc = "+acalc);
		if (!Util.within_epsilon(a, acalc, accelEpsilon)) { // 0.0000001 )) {
			double deltaAccel = Math.abs(a-acalc);
			if (!silent) {
				f.pln(" >>> vsConsistent VS FAIL! at i = "+i+" VSC section fails ACCEL test! a = "+a+ " acalc = "+acalc);
			}
	    	if (deltaAccel > 10.0*accelEpsilon) f.pln(" vsConsistent: ******************************************************* deltaAccel = "+deltaAccel);
			rtn = false;
		}
		//f.pln(" >>> isConsistent i = "+i+" a = "+a+"  vo = "+vo);
		// check that the altitude at VSCEnd is proper
		double ds = vin.vs()*dt + 0.5*a*dt*dt;
		//double distV = vertDistance(i,nextVSCEnd(i));
		double distV = 	VSCEnd.position().signedDistanceV(VSCBegin.position());		
		double absDiff = Math.abs(ds-distV);
		//f.pln(" $$$ vsConsistent i = "+i+" dt = "+dt+" ds = "+Units.str8("ft",ds)+ " distV = "+Units.str8("ft",distV)+" absDiff = "+Units.str8("ft",absDiff));
		if (!Util.within_epsilon(absDiff,distEpsilon)) { // 0.0000001)) {
			if (!silent) {
				f.pln(" >>> vsConsistent VS FAIL! at i = "+i+" VSC Section fails Test! absDiff = "+Units.str("m",absDiff,8));
			    f.pln(" >>> .........  ds = "+Units.str("ft",ds,8)+ " distV = "+Units.str("ft",distV,8));
			}
		    if ( ! silent && absDiff > 10.0*distEpsilon) f.pln(" vsConsistent: ******************************************************* absDiff = "+Units.str("ft",absDiff,8));
			rtn = false;
		}
		return rtn;

	}

	public static boolean vsConsistent(Plan p, int idxBegin,  double accelEpsilon, double distEpsilon, boolean silent) {
		if ( ! p.point(idxBegin).isBVS()) return true;
		int idxEnd = p.nextEVS(idxBegin);
		NavPoint VSCBegin = p.point(idxBegin);
		NavPoint VSCEnd = p.point(idxEnd);
		Velocity vin;
		//if (idxBegin == 0) {
			vin = p.point(idxBegin).velocityIn();     // not sure if we should allow TCP as current point ??
		//} else {
		//	vin = p.finalVelocity(idxBegin-1);
		//}
		Velocity vout = p.initialVelocity(idxEnd);
		return vsConsistent(idxBegin, VSCBegin, VSCEnd, accelEpsilon, distEpsilon, vin, vout, silent);
	}
	
	
	public static boolean turnConsistent(int i, NavPoint BOT, NavPoint EOT, double timeEpsilon, double distH_Epsilon, double distV_Epsilon, 
			Velocity vin, Velocity vout, boolean silent) {
 		Position vertex = BOT.sourcePosition();
 		//f.pln(" $$$$$ turnConsistent: vertex = "+vertex+" BOT = "+BOT);
	    EuclideanProjection proj = Projection.createProjection(vertex.lla().zeroAlt());  
	    return turnConsistent(i, BOT, EOT, timeEpsilon, distH_Epsilon, distV_Epsilon, vin, vout, silent, proj);
	}
	
	public static boolean turnConsistent(int i, NavPoint BOT, NavPoint EOT, double timeEpsilon, double distH_Epsilon, double distV_Epsilon, 
			Velocity vin, Velocity vout, boolean silent, EuclideanProjection proj) {
        boolean rtn = true;
		double finalTrack = vout.trk();
		double dt = EOT.time() - BOT.time();
		double omega = BOT.trkAccel();
		Position so = BOT.position();
		//f.pln(" $$>>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: i = "+i+" vin = "+vin+" dt = "+dt+" omega = "+Units.str4("deg/s",omega));
		//f.pln(" $$>>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: i = "+i+"   vout = "+vout);	
		//f.pln(" $$>>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: i = "+i+" vin.trk() = "+Units.str("deg", vin.compassAngle())+" finalTrack = "+Units.str("deg", vout.compassAngle())+" omega = "+Units.str("deg/s",omega,4));
		Position pos = (ProjectedKinematics.turnOmega(so, vin, dt, omega, proj).first).mkAlt(EOT.alt()); // need to treat altitude separately
		if (!EOT.position().almostEquals(pos,distH_Epsilon,distV_Epsilon)) { //       (p,0.005,1.2)) {  
			double distanceH = EOT.position().distanceH(pos);
			if (!silent) {
				f.pln(" >>> turnConsistent: TURN FAIL! i = "+f.padLeft(i+"", 2)+" calculated pos = "+pos.toString(8)
						+ "\n                                             plan EOT = "+EOT.position().toString(8));
				f.pln("            .... distanceH = "+Units.str("m",distanceH,8));
				double distanceV = EOT.position().distanceV(pos);
				f.pln("            .... distanceV = "+Units.str("m",distanceV,8));
			}
			if ( ! silent && distanceH > 10*distH_Epsilon) f.pln(" turnConsistent: ************************************************* (turn) distanceH = "+Units.str("m",distanceH,8));
			rtn = false;
		}
		//f.pln(" >>> turnConsistent: i = "+i+" POSITION test! calculated EOTpos = "+pos.toString(8)+ " plan EOT = "+EOT.toString(8));
		//f.pln(" $$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>..... EOT.velocity.trk() = "+Units.str("deg",EOT.velocityIn().trk(),4)+" finalTrack = "+Units.str("deg",finalTrack,4));				
		double deltaTrack = Util.turnDelta(vin.trk(),finalTrack);
//		double deltaTrackP = ProjectedKinematics.turnDelta(so,vin.trk(),finalTrack);		
	    double turnTime = Kinematics.turnTime(deltaTrack,omega);    
//	    double turnTimeP = Kinematics.turnTime(deltaTrackP,omega);    
	    //f.pln(" >>>>>>>>>>>>>>>>>>>>>>>>> turnConsistent: deltaTrack = "+Units.str("deg",deltaTrack)+" turnTime = "+turnTime);	
		if (!Util.within_epsilon(dt,turnTime,timeEpsilon)) { // 0.02)) {  // See testVsWithTurnHard3Vert for worst case  ***KCHANGE*** from 0.012 to 0.02
			if (!silent) f.pln(" >>> turnConsistent: TURN FAIL! i = "+i+" Turn section fails TIME test!  dt = "+dt+" != turnTime = "+turnTime);
//f.pln("turnTimeProjected="+turnTimeP);			
			rtn = false;
		}
		//f.pln(" >>> turnConsistent: TURN  i = "+i+" Turn section TIME test!  dt = "+dt+" != turnTime = "+turnTime);
		//f.pln(" $$$ turnConsistent: vin = "+vin+" dt = "+dt+" 	getVelocityIn() = "+BOT.getVelocityIn()+" rtn = "+rtn);
		return rtn;		
	}
	

	
	public static boolean turnConsistent(Plan p, int i, double timeEpsilon, double distH_Epsilon, double distV_Epsilon, boolean silent) {
		if ( ! p.point(i).isBOT()) return true;
		NavPoint BOT = p.point(i);
		int ixEOT = p.nextEOT(i);
		NavPoint EOT = p.point(ixEOT);	
		Velocity vin = p.point(i).velocityIn();
		//if (i == 0) vin = p.point(i).velocityIn();     // not sure if we should allow TCP as current point ??
		//else vin = p.finalVelocity(i-1);		  
		Velocity vout = p.initialVelocity(ixEOT);
		//Velocity vout = p.finalVelocity(ixEOT-1);
	    //f.pln(" $$$$$$$$$$$$$$$$ turnConsistent  ixEOT = "+ixEOT+" EOT = "+EOT+" vout = "+vout);
	    //DebugSupport.dumpPlan(p,"_BUGBUG");
		return turnConsistent(i, BOT, EOT, timeEpsilon, distH_Epsilon, distV_Epsilon, vin, vout, silent);
	}

	public static boolean velocityContinuous(Plan p, int i, double velEpsilon, boolean silent) {
		boolean rtn = true;
		if (!p.finalVelocity(i-1).within_epsilon(p.initialVelocity(i), velEpsilon)) { // 2.6)) { // see testAces3, testRandom for worst cases
			if (!silent) {
				f.pln("\n ----------------------------------------------");
				f.pln(" $$$ isConsistent: FAIL! continuity: finalVelocity("+(i-1)+") = "+p.finalVelocity(i-1).toStringUnits()						
						+"\n                                != initialVelocity("+i+") = "+p.initialVelocity(i).toStringUnits()
						+" point "+(i-1)+" type "+p.point(i-1).tcpTypeString()+" point "+i+" type "+p.point(i).tcpTypeString()); 
				Velocity DeltaV = p.finalVelocity(i-1).Sub(p.initialVelocity(i));
				if (DeltaV.norm() > 10*velEpsilon) {f.p(" turnConsistent: ********************************");
				      f.pln("           ....  DeltaV = "+DeltaV.toStringUnits()+" DeltaV.norm() = "+DeltaV.norm());
				}
				//DebugSupport.halt();
			}
			rtn = false;  
		} 
		return rtn;
	}
	
	public static boolean isGsConsistent(Plan p,  double accelEpsilon, double distEpsilon, boolean silent) {	
		boolean rtn = true;
		for (int i = 0; i < p.size(); i++) {
			if (p.point(i).isBGS()) {
				if ( ! PlanUtil.gsConsistent(p, i, accelEpsilon, distEpsilon, silent)) 
					rtn = false;
			}
		}		
		return rtn;
	}

	
	public static boolean isVsConsistent(Plan p, double accelEpsilon, double distEpsilon, boolean silent) {	
		boolean rtn = true;
		for (int i = 0; i < p.size(); i++) {
			if (p.point(i).isBVS()) {
				if ( ! PlanUtil.vsConsistent(p, i, accelEpsilon, distEpsilon, silent))
					rtn = false;
			}
		}		
		return rtn;
	}

	
	public static boolean isTurnConsistent(Plan p, double timeEpsilon, double distH_Epsilon, double distV_Epsilon, boolean silent) {	
		boolean rtn = true;
		for (int i = 0; i < p.size(); i++) {
			if (p.point(i).isBOT()) {
				if ( ! PlanUtil.turnConsistent(p, i, timeEpsilon, distH_Epsilon, distV_Epsilon,  silent))
					rtn = false;
			}
		}		
		return rtn;
	}

	public static boolean isVelocityConsistent(Plan p, double velEpsilon, boolean silent) {	
		boolean rtn = true;
		for (int i = 0; i < p.size(); i++) {
			if (p.point(i).isTCP()) { 
				if (! PlanUtil.velocityContinuous(p, i, velEpsilon, silent)) rtn = false;
			}
		}		
		return rtn;
	}

	/**
	 * This returns true if the entire plan produces reasonable accelerations. If
	 * the plan has instantaneous "jumps," it is not consistent.
	 */
	public boolean isConsistent(Plan p, boolean silent) {	
		boolean rtn = true;
		if ( ! p.isWellFormed()) {
			if ( ! silent) {
				f.pln("  >>> isConsistent FAIL! not WellFormed!! "+p.strWellFormed());
			}
			return false;
		}
		for (int i = 0; i < p.size(); i++) {
			if (p.point(i).isBGS()) {
				if ( ! PlanUtil.gsConsistent(p, i, 0.00001, 0.07, silent)) 
					rtn = false;
			}
			if (p.point(i).isBVS()) {
				if ( ! PlanUtil.vsConsistent(p, i, 0.00001, 0.00001, silent))
					rtn = false;
			}
			if (p.point(i).isBOT()) {
				if ( ! PlanUtil.turnConsistent(p, i, 0.02, 0.005, 1.2,  silent))
					rtn = false;
			}
			if (i > 0) {  
				if (p.point(i).isTCP()) { 
					if (! PlanUtil.velocityContinuous(p, i, 2.6, silent)) rtn = false;
				}
			}
		}		
		return rtn;
	}


	
//	public static boolean turnConsistent(int i, NavPoint BOT, NavPoint EOT, Velocity vin, double finalTrack, boolean silent, EuclideanProjection proj) {
//		boolean rtn = true;
//		double dt = EOT.time() - BOT.time();
//		double omega = BOT.trkAccel();
//		Position so = BOT.position();
//		//boolean turnRight = Util.clockwise(vo.trk(), vEOT.trk());
//		Position p = BOT.position().linear(vin,dt);                  // for vertical
//		p = (ProjectedKinematics.turnOmega(so, vin, dt, omega, proj).first).mkAlt(EOT.alt()); // need to treat altitude separately
//		//f.pln(" $$$ isConsistent: so = "+so+" vo = "+vo+" p = "+p);
//		if (!EOT.position().almostEquals(p,0.001,1.2)) {
//			if (!silent) {
//				f.pln(" >>> isConsistent: TURN FAIL! i = "+i+" POSITION test! p = "+p.toString4()+ " EOT = "+EOT.toString(8));
//				f.pln("      .... distanceH = "+Units.str8("m",EOT.position().distanceH(p)));
//				//f.pln("      .... distanceV = "+Units.str8("m",EOT.position().distanceV(p)));
//			}
//			rtn = false;
//		}
//		//f.pln(i+" $$$$ isConsistent: nextEOT(i) = "+nextEOT(i)+" finalTrack = "+Units.str8("deg",finalTrack)+" vo.trk() = "+Units.str8("deg",vo.compassAngle()));
//		double deltaTrack = Util.turnDelta(vin.trk(),finalTrack);
//		//f.pln(i+" $$$$ isConsistent: deltaTrack = "+Units.str8("deg",deltaTrack)+" turnRate = "+Units.str8("deg/s",turnRate));
//		double turnTime = Kinematics.turnTime(deltaTrack,omega);
//		if (!Util.within_epsilon(dt,turnTime,0.012)) {  // See testVsWithTurnHard3Vert for worst case
//			if (!silent) f.pln(" >>> isConsistent: TURN FAIL! i = "+i+" Turn section fails TIME test!  dt = "+dt+" != turnTime = "+turnTime);
//			rtn = false;
//		}
//		//f.pln(" $$$ turnConsistent: vin = "+vin+" dt = "+dt+" 	getVelocityIn() = "+BOT.getVelocityIn()+" rtn = "+rtn);
//		return rtn;
//	}


	public static Plan applyWindField(Plan pin, Velocity v) {
		Plan p = pin.copy(); 
		NavPoint np0 = p.point(0);
		Velocity v0 = p.initialVelocity(0);
		for (int j = 1; j < p.size(); j++) {
			Velocity v1 = v0.Sub(v);
			NavPoint np1 = p.point(j);
			double dt = np1.time()-np0.time();
			NavPoint np2 = np1.makePosition(np0.linear(v1, dt).position());
			np0 = np1;
			p.set(j, np2);
		}
		return p;
	}

	public static int insertVirtual(Plan ac, double time) {
		if (time >= ac.getFirstTime() && (time <= ac.getLastTime()) && ac.getIndex(time) < 0) {
			NavPoint src = ac.point(ac.getSegment(time));
//			NavPoint np = src.makePosition(ac.position(time)).makeTime(time); // .makeMutability(false, false, false);
			NavPoint np = src.makeStandardRetainSource().makePosition(ac.position(time)).makeTime(time);
			np = np.makeVirtual();
			return ac.add(np);
		}
		else return -1;
	}

	
	// from Aviation Formulary
	// longitude sign is reversed from the formulary!
	private static double lonCross(Plan ac, int i, double lat3) {
		//f.pln("lonCross 1 "+i);  
		double lat1 = ac.point(i).lat();
		double lon1 = ac.point(i).lon();
		double lat2 = ac.point(i+1).lat();
		double lon2 = ac.point(i+1).lon();
		double tc = ac.initialVelocity(i).compassAngle(); 
		boolean NW = (tc > Math.PI/2 && tc <= Math.PI) || tc >= 3*Math.PI/2;
		double l12;

		if (NW) l12 = lon1-lon2;
		else l12 = lon2-lon1;

		double A = Math.sin(lat1)*Math.cos(lat2)*Math.cos(lat3)*Math.sin(l12);
		double B = Math.sin(lat1)*Math.cos(lat2)*Math.cos(lat3)*Math.cos(l12) - Math.cos(lat1)*Math.sin(lat2)*Math.cos(lat3);
		double lon;

		if (NW) lon = lon1 + Math.atan2(B,A) + Math.PI;
		else lon = lon1 - Math.atan2(B,A) - Math.PI;

		if (lon >= 2*Math.PI) lon = lon-2*Math.PI;

		if (NW) {
			lon = lon-Math.PI;
		} else {
			lon = Math.PI+lon;
		}

		if (lon < -Math.PI) lon = 2*Math.PI+lon;
		if (lon > Math.PI) lon = -2*Math.PI+lon;

		//f.pln("lonCross 2 "+i);  
		return lon;
	}


	// from Aviation Formulary
	private static double latMax(Plan ac, int i) { 
		//f.pln("latMax 1 "+i);  
		double tc = ac.initialVelocity(i).compassAngle(); 
		double lat = ac.point(i).lat();
		double ret = Math.acos(Math.abs(Math.sin(tc)*Math.cos(lat)));
		if (tc > Math.PI/2 && tc < 3*Math.PI/2) ret = -ret;
		//f.pln("latMax 2 "+i);  
		return ret;
	}

	private static int addLocalMaxLat(Plan ac, int i) {
		//f.pln("addLocal 1 "+i);  

		boolean t1 = Math.abs(ac.initialVelocity(i).trk()) > Math.PI/2;
		boolean t2 = Math.abs(ac.finalVelocity(i).trk()) > Math.PI/2;
		if (t1 == t2) {
			//f.pln("addLocal 2 "+i);  
			return i+1;
		} else {
			double lat1 = Math.abs(ac.point(i).lat());
			double lon1 = Math.abs(ac.point(i).lon());
			double lat2 = latMax(ac,i);
			double lon2 = lonCross(ac,i,lat2);
			double dist = GreatCircle.distance(lat1,lon1,lat2,lon2);
			double gs = ac.initialVelocity(i).gs();
			double t = ac.getTime(i) + dist/gs;  
			if (t < ac.getTime(i+1) && t > ac.getTime(i)) {
				//    	  error.addError("addLocalMaxLat time out of bounds");
				insertVirtual(ac,t);
				//f.pln("NEWTIME "+ac+" "+i+" "+t);      
				//ac.setPointLabel(i+1, "maxlat");
				ac.set(i+1, ac.point(i+1).makeLabel("maxlat"));
//				modified = true;
				//f.pln("addLocal 3 "+i);  
				return i+2;
			}
			else return i+1;
		}
	}

	private static void insertLocalMax(Plan ac) {
		int i = 0;
		while (i < ac.size()-1) {
			i = addLocalMaxLat(ac,i);
		}
	}


	private static double getLegDist(Plan ac, int i, double accuracy, double mindist) {
		double lat = Math.max(Math.abs(ac.point(i).lat()),
				Math.abs(ac.point(i+1).lat()));
		double maxdist = Projection.projectionConflictRange(lat, accuracy);
		// necessary due to hard limits on number of wp
		if (maxdist < mindist) maxdist = mindist;
		return maxdist;
	}
	
	private static int interpolateVirtualsAccelHalf(Plan ac, double haccuracy, double vaccuracy, double minDt, double t1, double t2, int depth) {
		if (t2-t1 > minDt) {
			double tmid = (t2+t1)/2.0;
			Position l1 = ac.position(tmid, true);
			Position k1 = ac.position(tmid,false);
			if (l1.distanceH(k1) > haccuracy || l1.distanceV(k1) > vaccuracy) {
				insertVirtual(ac,tmid);
				return interpolateVirtualsAccelHalf(ac, haccuracy, vaccuracy, minDt, t1, tmid, depth+1);
//				interpolateVirtualsAccelHalf(ac, haccuracy, vaccuracy, minDt, tmid, t2);
			}
		}
		return depth;
	}

	
	

	public static void interpolateVirtualsAccel(Plan ac, double haccuracy, double vaccuracy, double minDt, double start, double end, boolean midOnly) {
			for (int i = 0; i < ac.size(); i++) {
				NavPoint np =  ac.point(i);
				double t1 = np.time();
				double t2;
				if (t1 >= start && t1 <= end) {
					if (ac.inAccel(t1)) {
						t2 = ac.getTime(i+1);
						if (midOnly) {
							insertVirtual(ac,(t2+t1)/2.0);
							i++;
						} else {
							i += Math.max(0, interpolateVirtualsAccelHalf(ac,haccuracy, vaccuracy, minDt, t1, t2, 0)-1); // add on the number of additional points, -1, if 2 or more added
						}
					}
//					if (np..isBOT()) {
//						t2 = ac.getTime(ac.nextEOT(i));
//						if (midOnly) {
//							insertVirtual(ac,(t2+t1)/2.0);
//							i++;
//						} else {
//							i += interpolateVirtualsAccelHalf(ac,haccuracy, vaccuracy, minDt, t1, t2, 0);
//						}
//					}
//					if (np.isBGS()) {
//						t2 = ac.getTime(ac.nextEGS(i));
//						if (midOnly) {
//							insertVirtual(ac,(t2+t1)/2.0);
//							i++;
//						} else {
//							i += interpolateVirtualsAccelHalf(ac,haccuracy, vaccuracy, minDt, t1, t2, 0);
//						}
//					}
//					if (np.isBVS()) {
//						t2 = ac.getTime(ac.nextEVS(i));
//						if (midOnly) {
//							insertVirtual(ac,(t2+t1)/2.0);
//							i++;
//						} else {
//							i += interpolateVirtualsAccelHalf(ac,haccuracy, vaccuracy, minDt, t1, t2, 0);
//						}
//					}
					
				}
			}
	}
		
	
	// this adds for the leg starting at or before startTm, and ending before or at endTm
	public static void interpolateVirtuals(Plan ac, double accuracy, double startTm, double endTm) {
		if (ac.isLatLon()) {
			if (startTm < ac.getFirstTime()) startTm = ac.getFirstTime();
			if (endTm > ac.getLastTime()) endTm = ac.getLastTime();
			insertLocalMax(ac);
			int i = ac.getSegment(startTm);
			double mindist = ac.pathDistance() / 900; // lower bound on distance between virtuals
			while (i < ac.size()-1 && ac.getTime(i) <= endTm) {
				double dist = ac.pathDistance(i);
				double legDist = getLegDist(ac,i,accuracy,mindist); // "optimal" distance between virtuals
				//Special case for kinematic plans in acceleration zones -- ensure there are at least 2 mid-points
                // TODO: Possibly get rid of this
//				if (!ac.isLinear()) {
//					Plan kpc = ac;
//					if (kpc.point(i).isTurnBegin() || kpc.point(i).isTurnMid()) {
//						legDist = Math.min(legDist, dist/3.0);
//					}
//				}
				if (dist > legDist) {
					double gs = ac.averageVelocity(i).gs();
					int j = i;
					double tmIncr = legDist/gs;
					//System.out.println("Points "+ac.point(i)+" "+ac.point(i+1));
					if (Math.abs(ac.point(i).lat()) >= Math.abs(ac.point(i+1).lat())) {
						double nextT = ac.getTime(i+1);
						double t = ac.getTime(i);
						while (t + tmIncr + Constants.TIME_LIMIT_EPSILON < nextT) {
							t += tmIncr;
							//System.out.println("1AC "+ac.getName()+" Time "+t+" "+nextT);
							insertVirtual(ac,t);
							legDist = getLegDist(ac,j,accuracy,mindist);
							j++;
							tmIncr = legDist/gs;
						}
					} else {
						double thisT = ac.getTime(i);
						double t = ac.getTime(i+1);
						while (t - tmIncr - Constants.TIME_LIMIT_EPSILON > thisT) {
							t -= tmIncr;
							//System.out.println("2AC "+ac.getName()+" Time "+t+" "+thisT);
							insertVirtual(ac,t);
							legDist = getLegDist(ac,i,accuracy,mindist);
							j++;
							tmIncr = legDist/gs;
						}
					}
					i = Math.max(i,j-1); // possibly take back last increment
				} // dist < legdist    
				i++;
			} //while
//			modified = true;
		} //islatlon
	}
	
	public static void interpolateVirtuals(Plan ac, double accuracy) {
			interpolateVirtuals(ac, accuracy, ac.getFirstTime(), ac.getLastTime());
	}
	
	// this removes all virtuals AFTER time startTm and BEFORE endTm.
	// note this WILL NOT remove Fixed virtuals!
	// if it enounters any fixed virtuals, it will return a value of FALSE
	// if all virtuals were successfully removed, it will return a value of TRUE
	private static boolean removeVirtualsRange(Plan ac, double startTm, double endTm, boolean all) {
		int i = ac.getSegment(startTm)+1;
		boolean rtn = true;
		while (i < ac.size() && ac.getTime(i) < endTm) {
			if (ac.point(i).isVirtual() || all) {
//				if (ac.point(i).isFixed()) {
//					rtn = false;
//					i++;
//				} else {
					ac.remove(i);
//				}
			} else {
				i++;
			}
		}
//		modified = true;
//		noLongLegs[ac] = false;
//		interpolated[ac] = -1.0;
		return rtn;
	}
	
	/**
	 * Return a new plan that has both latlon and acceleration virtual points added (if necessary)
	 * @param acin
	 * @param haccuracy max horizontal deviation
	 * @param vaccuracy max vertical deviation
	 * @param minDt minimal segment time to consider breaking up
	 * @param start start time in plan
	 * @param end end time in plan
	 * @return new plan with added virtuals (if latlon or kinematic)
	 */
	public static Plan interpolateVirtualsAll(Plan acin, double haccuracy, double vaccuracy, double minDt, double start, double end, boolean midOnly) {
		if (!acin.isLatLon() && acin.isLinear()) return acin; // skip copy
		Plan ac = acin.copy();
		interpolateVirtuals(ac,haccuracy, start,end);
		interpolateVirtualsAccel(ac,haccuracy,vaccuracy,minDt, start,end,false);
		return ac;
	}

	

//	// **** EXPERIMENTAL ****
//	public static Plan removeVsTCPs(Plan fp, int start, int upto) {
//		f.pln(" $$$$$$$$ removeVsTCPs: start = "+start+" upto = "+upto);
//		Plan traj = new Plan(fp.getName());
//		for (int i = 0; i < start; i++) traj.add(fp.point(i));
//		for (int i = start; i <= upto; i++) {
//			if (fp.point(i).isBVS()) {
//				f.pln(" $$$$$$$$ removeVsTCPs: i = "+i);
//			    int j = fp.nextEVS(i);
//			    upto = Math.max(upto, j);
//			    double t1 = fp.point(i).time();
//			    double v1 = fp.point(i).velocityIn().z;
//			    double v2 = fp.point(j).velocityIn().z;
//			    double t2 = fp.point(j).time();	 
//			    double z1 = fp.point(i).z();
//			    double tNew = (t1 + t2)/2.0;
//			    double newAlt = z1 + v1*(tNew-t1);
//			    NavPoint np = new NavPoint(fp.position(tNew).mkAlt(newAlt),tNew)	;
//			    traj.add(np);	
//			    f.pln(" $$$$$$$$ removeVsTCPs: ADD np = "+np.toStringFull());
//				for (int k = i+1; k < j; k++) {  // need to change altitude of this points between BVS and EVS
//					NavPoint kp = fp.point(k);
//					double kt = kp.time();
//					double kAlt;
//					if (kt <= tNew) {
//						kAlt = z1 + v1*(kt-t1);
//					} else {
//						kAlt = newAlt +v2*(kt-tNew);
//					}
//				    NavPoint kpNew = kp.makePosition(kp.position().mkAlt(kAlt));
//                    int ix = traj.add(kpNew);
//    			    f.pln(" $$$$$$$$ removeVsTCPs: ADD kpNew = "+kpNew.toStringFull());
//                    if (ix < 0) {
//                    	f.pln(" $$$$$$$$ removeVsTCPs: error: traj.add(kpNew) OVERLAPPED!!!");
//                    }
//				}
//				i = j;
//			} else {
//				traj.add(fp.point(i));
//			}
//			
//		}		
//		for (int i = upto+1; i < fp.size(); i++) traj.add(fp.point(i));	
//		return traj;
//	}
	
	/**
	 * Returns a Plan with TCPs removed between start and upto.  If it was a kinematic plan, this will attempt to regress the TCPs to 
	 * their original source points and times (if the proper metadata is available).  
	 * 
	 * Note.  No check is made to insure that start or upto is not in the middle of a TCP.
	 * Note.  See also Plan.revertGroupOfTCPs which uses sourcePosition but not sourceTimes.  It seeks to retain ground speeds.
	 * 
	 * Warning:  The reversion of points later in a plan without reverting earlier points can lead to strange ground speeds, because
	 *           the source times correspond to longer paths (without turns).
	 * 
	 */
	public static Plan revertTCPs(Plan fp, int start, int upto) {
		Plan lpc = new Plan(fp.getName(),fp.getNote());
		//f.pln(" $$ revertTCPs: START: fp = "+fp.toString()+" "+start+" "+upto);
		for (int i = 0; i < fp.size(); i++) {
			NavPoint p = fp.point(i);
			// case: point it virtual or temp: delete point
			// case: point is not original or first or last: keep point.
			// case: point is part of a turn: keep only the vertex point, delete others, if all three are present, otherwise keep remaining
			// case: point is part of GSC: keep only first point, delete second if both present, otherwise keep remaining
			// case: point is part of VSC: keep only vertext point, delete others if both present, otherwise keep remaining
			// don't deal with deleted points quite yet.  this gets really messy.
			if (i >= start && i <= upto) {
				if ((!p.isVirtual()) ) {
					//f.pln("revertTCPs $$$$$$$$$$$$ "+p.hasSource());
					if (p.hasSource()){   // note for standard (linear) points, sourceTime and sourcePosition equal time() and position()
						lpc.add(new NavPoint(p.sourcePosition(),p.sourceTime()));
						//f.pln(" $$ revertTCPs: add "+" "+f.Fm1(p.sourceTime())+"   "+p.sourcePosition());
					}
				}
			} else {
				lpc.add(p);
			}
		}
		lpc.getMessage();
		return lpc;	
	}

	/**
	 * Returns a Plan with all TCPs removed. Points are returned to their original source points and times 
	 * (if the proper metadata is available).  
	 * 
	 * Note.  If a subset of points need to be reverted, See Plan.revertGroupOfTCPs which uses sourcePosition but not sourceTimes.  
	 * 
	 */
	public static Plan revertTCPs(Plan fp) {
		return revertTCPs(fp,0, fp.size()-1);       
	}

	
	// will not remove first or last point
	public static void removeRedundantPoints(Plan pln, int from, int to) {
		double velEpsilon = 1.0;
		int ixLast = Math.min(pln.size() - 2, to);
		int ixFirst = Math.max(1, from);
		for (int i = ixLast; i >= ixFirst; i--) {
			NavPoint p = pln.point(i);
			Velocity vin = pln.finalVelocity(i-1);
			Velocity vout = pln.initialVelocity(i);
			if (!p.isTCP() && vin.within_epsilon(vout, velEpsilon)) { // 2.6)) { // see testAces3, testRandom for worst cases
                //f.pln(" $$$$$ removeRedundantPoints: REMOVE i = "+i);
				pln.remove(i);
			}
		}
	}
	
	public static void removeRedundantPoints(Plan pln) {
         removeRedundantPoints(pln,0,Integer.MAX_VALUE);
	}

	/** *********** UNDER DEVELOPMENT *************
	 * Revert the TCP pair in a plan structurally.  Properly relocate all points between TCP pair.
	 * 
	 * Note.  No check is made to insure that start or upto is not in the middle of a TCP.
	 */
	public static void structRevertTCP(Plan pln, int ix) {
		double zVertex = -1;
		if (ix < 0 || ix >= pln.size()) {
			pln.addError(" structRevertTCP: index out of range");
		}
		if (pln.point(ix).isBGS()) {
			boolean revertPreviousTurn = true;
			structRevertGsTCP(pln,ix,revertPreviousTurn);
		} else if (pln.point(ix).isBVS()) {
			zVertex = structRevertVsTCP(pln,ix);						
		} if (pln.point(ix).isBOT()) {
			// store distance from BOT to all points between BOT and EOT	
			boolean addBackMidPoints = true;
			boolean killNextGsTCPs = false;
			structRevertTurnTCP(pln,ix,addBackMidPoints,killNextGsTCPs,zVertex);
		} else {
			pln.addError(" structRevertTCP: index must be a begin TCP!!!" );
		}
		// return new Plan();
	}

	public static void structRevertTCPs(Plan pln, boolean removeRedundantPoints) {
		f.pln(" ----------------------- structRevertVsTCP pass ------------------------");
		double zVertex = -1;
		for (int i = pln.size()-2; i > 0; i--) {
			structRevertVsTCP(pln,i);
		}
		f.pln(" ----------------------- structRevertTurnTCP pass ------------------------");
		for (int i = pln.size()-2; i > 0; i--) {
			boolean addBackMidPoints = true;
			boolean killNextGsTCPs = true;
			structRevertTurnTCP(pln,i,addBackMidPoints,killNextGsTCPs, -1.0);
		}
		f.pln(" ----------------------- structRevertGsTCP pass ------------------------");
		for (int i = pln.size()-2; i > 0; i--) {
			boolean revertPreviousTurn = true;
			structRevertGsTCP(pln,i,revertPreviousTurn);
		}
		f.pln(" ----------------------- removeRedundantPoints pass ------------------------");
		if (removeRedundantPoints) removeRedundantPoints(pln,0,pln.size()-1);
	}
	
	/** structurally revert all TCPS back to its original linear point which have the same sourceTime as the point at index i
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain 
	 *  original ground speed into the point after ix. 
	 *  
	 *   **** SOURCE TIME VERSION *****
	 * 
	 * @param ix  The index of one of the TCPs created together that should be reverted
	 * @return index of the reverted point
	 */
	public static int structRevertGroupOfTCPsSource(Plan pln, int ix, boolean killAllOthersInside) {
		if (ix < 0 || ix >= pln.size()) {
			pln.addError(".. structRevertGroupOfTCPs: invalid index "+ix, 0); 
			return -1;
		} 
		NavPoint origDsegPt = pln.point(ix);	
		if (!origDsegPt.isTCP()) {
			//f.pln(" $$ revertGroupOfTCPs: point "+dSeg+" is not a TCP, do nothing!");
			return ix;
		}
		double sourceTm = origDsegPt.sourceTime();
	    //int dSeg = getSegment(sourceTm);
		//f.pln("\n $$$ structRevertGroupOfTCPs: point(dSeg).time = "+pln.point(ix).time() +" sourceTm = "+sourceTm);
	    int firstInGroup = -1;                            // index of first TCP in the group
	    int lastInGroup = pln.size()-1;                   // index of the last TCP in the group
	    //boolean isTurnGsFourGroup = false;    
	    boolean gsTCPInGroup = false;    
	    for (int j = 0; j < pln.size(); j++) {
	    	if (Constants.almost_equals_time(pln.point(j).sourceTime(),sourceTm)) {
	    		if (firstInGroup == -1) firstInGroup = j;
	    		lastInGroup = j;
	    		if (pln.point(j).isGsTCP()) gsTCPInGroup = true;
	    	}
	    }
	    double gsInFirst = pln.finalVelocity(firstInGroup-1).gs();
	    int nextSeg = lastInGroup+1;
	    double tmNextSeg = pln.getTime(nextSeg);
	    double gsInNext = pln.finalVelocity(nextSeg-1).gs();
	    if (killAllOthersInside) {
	    	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
                 if ( ! Constants.almost_equals_time(pln.point(ii).sourceTime(),sourceTm)) {
                	 pln.remove(ii);
                     lastInGroup--;
                 }
	    	}
	    }
	    // revert vertical TCPs first
    	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
            if (pln.point(ii).isVsTCP()) {
            	double zVertex = structRevertVsTCP(pln,ii);
            	if (zVertex >= 0 && ii <= ix) ix--;
            }
    	}
    	// get rid of all ground speed TCPs if this is fundamentally a turn
    	if (pln.point(firstInGroup).isTrkTCP()) {
	    	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
                if (pln.point(ii).isGsTCP()) { 
               	    pln.remove(ii);
                    lastInGroup--;
                }
	    	}
	    	boolean killNextGsTCPs = false;
	    	structRevertTurnTCP(pln,firstInGroup, !killAllOthersInside, killNextGsTCPs,-1.0);
    	}
//    	// restore ground speed into nextSeg
//	    if (tmNextSeg > 0) { // if reverted last point, no need to timeshift points after dSeg
//	    	int newNextSeg = pln.getSegment(tmNextSeg);
//	    	double newNextSegTm = pln.linearCalcTimeGSin(newNextSeg, gsInNext);
//	    	double dt2 = newNextSegTm - tmNextSeg;
//	    	pln.timeshiftPlan(newNextSeg, dt2);   
//	    }
    	NavPoint ixP = pln.point(ix);
	   	if (ixP.isGsTCP()) {  // does not revert previous turn  	
	    	//f.pln(" $$$ structRevertGroupOfTCPs(SOURCE): pln.point("+ix+") = "+pln.point(ix).toStringFull());
	   		boolean revertPreviousTurn = true;
    		structRevertGsTCP(pln,ix,revertPreviousTurn);   	
    	}
        return firstInGroup; // ???
	}

	private static boolean inGsAccel(Plan pln, int firstInGroup, int lastInGroup) {  // See test case T445 to see need for this
		for (int j = firstInGroup; j < lastInGroup; j++ ) {
		    if (pln.inGsChange(pln.point(j).time())) return true;
		}
		 return false;
	}
	
	private static boolean inVsAccel(Plan pln, int firstInGroup, int lastInGroup) {  // See test case T445 to see need for this
		for (int j = firstInGroup; j < lastInGroup; j++ ) {
		    if (pln.inVsChange(pln.point(j).time())) return true;
		}
		 return false;
	}

	private static boolean inTrkAccel(Plan pln, int firstInGroup, int lastInGroup) {  // See test case T445 to see need for this
		for (int j = firstInGroup; j < lastInGroup; j++ ) {
		    if (pln.inTrkChange(pln.point(j).time())) return true;
		}
		 return false;
	}

	
	/** (EXPERIMENTAL) structurally revert all TCPS that create acceleration zones containing ix
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain 
	 *  original ground speed into the point after ix.  
	 *  
	 *  NOTE This method does not depend upon source time!!
	 * 
	 * @param ix  The index of one of the TCPs created together that should be reverted
	 * @return index of the reverted point
	 */
	public static int structRevertGroupOfTCPsTimeWindow(Plan pln, int ix, double timeWindow) {
		if (ix < 0 || ix >= pln.size()) {
			pln.addError(".. structRevertGroupOfTCPs: invalid index "+ix, 0); 
			return -1;
		} 
		NavPoint origPt = pln.point(ix);	
        double lowTm = origPt.time()-timeWindow/2.0;
        double highTm = origPt.time()+timeWindow/2.0;
        int lowIx = pln.getNearestIndex(lowTm);
        int highIx = pln.getNearestIndex(highTm);
        f.pln(" $$$$$ structRevertGroupOfTCPsTimeWindow lowIx = "+lowIx+" highIx = "+highIx);
        for (int i = lowIx; i <= highIx; i++) {
    		NavPoint np = pln.point(i);
    		if (np.time() > highTm) break;
    		double zVertex = -1;
		    if (np.isVsTCP()) {
    			if (np.isEVS()) i = pln.prevBVS(i);
    			f.pln(" $$$ structRevertVsTCP i = "+i);
            	zVertex = structRevertVsTCP(pln,i);
//		    } 
//		    if (zVertex >= 0) {
//		    	highIx--;
//		    	i--;
//		    	np = pln.point(i);
//		    }
		    } else if (np.isTrkTCP()) {
    			if (np.isEOT()) i = pln.prevBOT(i);
    	    	boolean killNextGsTCPs = true;
    	    	boolean addBackMidPoints = false;
    			f.pln(" $$$ structRevertTurnTCP i = "+i);
    	    	structRevertTurnTCP(pln,i, addBackMidPoints, killNextGsTCPs,zVertex);
    		} else if (np.isGsTCP()) {
    			if (np.isEGS()) i = pln.prevBGS(i);
    	    	zVertex = -1;
       			f.pln(" $$$ structRevertGsTCP i = "+i);  
       			boolean revertPreviousTurn = true;
    	   		structRevertGsTCP(pln,i,revertPreviousTurn);  
    		}
        }
		return -1;
	}
	
	
	public static int structRevertGroupOfTCPsTimeWindow(Plan pln, int ix) {
		return structRevertGroupOfTCPsTimeWindow(pln,ix,100.0);
	}

	/** structurally revert all TCPS that create acceleration zones containing ix
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain 
	 *  original ground speed into the point after ix.  
	 *  
	 *  NOTE This method does not depend upon source time!!
	 * 
	 * @param ix  The index of one of the TCPs created together that should be reverted
	 * @return index of the reverted point
	 */
	public static int structRevertGroupOfTCPs(Plan pln, int ix, boolean killAllOthersInside) {
		if (ix < 0 || ix >= pln.size()) {
			pln.addError(".. structRevertGroupOfTCPs: invalid index "+ix, 0); 
			return -1;
		} 
		//f.pln(" $$$$ BEFORE ix = "+ix+" "+pln.point(ix).toStringFull());
//		if (pln.inAccel(pln.point(ix).time()) && !pln.point(ix).isTCP()) { // acceleration mid Point	    	ix = pln.prevTCP(ix);
//			ix = pln.prevTCP(ix);
//	    	f.pln(" structRevertGroupOfTCPs:: MID ix reset to ix = "+ix);
//	    }		
		NavPoint origPt = pln.point(ix);	
		if (! origPt.isTCP()) return ix;  // nothing to do
 		//f.pln("$$$ structRevertGroupOfTCPs:origPt = point("+ix+") = "+pln.point(ix).toStringFull());
	    int firstInGroup = ix;                  // index of first TCP in the group
	    int lastInGroup = ix;   // pln.size()-1;                // index of the last TCP in the group
	    if (firstInGroup == 0) {
	    	f.pln(" !! structRevertGroupOfTCPs: ERROR cannot remove first point 1");
	    	return -1;
	    }
	    if (origPt.isVsTCP()) {
	    	if (origPt.isEVS()) {
	    		int ixBVS = pln.prevBVS(ix);
	    		if (ixBVS < firstInGroup) firstInGroup = ixBVS;
	    		lastInGroup = ix;
	    	} else {
	    		int ixEVS = pln.nextEVS(ix);
	    		if (ixEVS > lastInGroup) lastInGroup = ixEVS;
	    		firstInGroup = ix;
	    	}
	    	//f.pln(" $$$$ structRevertGroupOfTCPs(VS): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
	    } else {
		    if (origPt.isGsTCP()) {
		    	if (origPt.isEGS()) {
		    		int ixBGS = pln.prevBGS(ix);
		    		if (ixBGS >= 0 && ixBGS < firstInGroup) firstInGroup = ixBGS;
		    		lastInGroup = ix;
		    	} else {
		    		int ixEGS = pln.nextEGS(ix);
		    		if (ixEGS >= 0 && ixEGS > lastInGroup) lastInGroup = ixEGS;
		    		firstInGroup = ix;
		    	}
		        //f.pln(" $$$$ structRevertGroupOfTCPs(GS): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);	
		    } else { // track
		    	if (origPt.isEOT()) {
		    		int ixBOT = pln.prevBOT(ix);
		    		if (ixBOT >= 0 && ixBOT < firstInGroup) firstInGroup = ixBOT;
		    		lastInGroup = ix;
		    	} else {
		    		int ixEOT = pln.nextEOT(ix);
		    		if (ixEOT >= 0 && ixEOT > lastInGroup) lastInGroup = ixEOT;
		    		firstInGroup = ix;
		    	}
		    	//f.pln(" $$$$ structRevertGroupOfTCPs(TRK): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
		    }
	    }	    		
        //f.pln(" $$$$ structRevertGroupOfTCPs(FINAL): sz = "+pln.size()+" firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);	    
	    // revert vertical TCPs 
	    double zVertex = -1;
    	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
            if (pln.point(ii).isVsTCP()) {
            	//f.pln(" $$$$ structRevertGroupOfTCPs(VS-2): ii = "+ii);
            	zVertex = structRevertVsTCP(pln,ii);
            	if (zVertex >= 0) {
            		lastInGroup--;
            		//if (ii == firstInGroup) firstInGroup++;
            	}           	
            }
    	}
    	if (pln.point(firstInGroup).isTrkTCP()) {
	    	boolean killNextGsTCPs = true;
	    	boolean addBackMidPoints = false;
	    	structRevertTurnTCP(pln,firstInGroup, addBackMidPoints, killNextGsTCPs,zVertex);
    	}
    	if (pln.point(firstInGroup).isGsTCP()) {  // does not revert previous turn  
    		//f.pln(" $$$$ structRevertGroupOfTCPs: GS section: firstInGroup = "+firstInGroup);
    		boolean revertPreviousTurn = true;
    		firstInGroup = structRevertGsTCP(pln,firstInGroup, revertPreviousTurn);  
    	}
     	//f.pln(" $$$$$$$$$$$$$ firstInGroup = "+firstInGroup);
        return firstInGroup; 
	}

	
	/** Structurally revert TCP at ix: (does not depend upon source time!!)
	 *  This private method assumes ix > 0 AND ix < pln.size().  If ix is not a BOT, then nothing is done
	 * 
	 * @param pln
	 * @param ix
	 * @param addBackMidPoints if addBackMidPoints = true, then if there are extra points between the BOT and EOT, make sure they are moved to
	                            the correct place in the new linear sections.  Do this by distance not time.
	 */
	protected static void structRevertTurnTCP(Plan pln, int ix, boolean addBackMidPoints, boolean killNextGsTCPs, double zVertex) {
		//f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT = "+pln.point(ix).isBOT());
		//f.pln(" $$$$$ structRevertTurnTCP: pln = "+pln);
		if (pln.point(ix).isBOT()) {
			//f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix);
			NavPoint BOT = pln.point(ix);
			String label = BOT.label();
			double tBOT = BOT.time();
			int nextEOTix = pln.nextEOT(ix);
			NavPoint EOT = pln.point(nextEOTix);	
			double tEOT = EOT.time();
			ArrayList<NavPoint> betweenPoints = new ArrayList<NavPoint>(4);
			ArrayList<Double> betweenPointDists = new ArrayList<Double>(4);
			if (addBackMidPoints) {  // add back mid points that are not TCPs
				for (int j = ix+1; j < nextEOTix; j++) {
					NavPoint np = pln.point(j);
					if ( ! np.isTCP()) {
						//f.pln(" >>>>> structRevertTurnTCP: SAVE MID pln.point("+j+") = "+pln.point(j).toStringFull());
						betweenPoints.add(np);
						double distance_j = pln.pathDistance(ix,j);
						betweenPointDists.add(distance_j);
					}
				}	
			}
			Velocity vin;
			if (ix == 0) vin = pln.point(ix).velocityIn();     // not sure if we should allow TCP as current point ??
			else vin = pln.finalVelocity(ix-1);
			double gsin = vin.gs();
			//f.pln(" $$$$ structRevertTurnTCP: gsin = "+Units.str("kn",gsin,8));
			Velocity vout = pln.initialVelocity(nextEOTix);			
			Pair<Position,Double> interSec = Position.intersection(BOT.position(), vin, EOT.position(), vout);
			double distToIntersec = interSec.first.distanceH(BOT.position());
			double tmInterSec = tBOT + distToIntersec/gsin;
			NavPoint vertex;
			if (tmInterSec >= tEOT) { // use BOT position, if vertex angle is too small
				 double tMid = (tBOT+tEOT)/2.0;
				 Position posMid = pln.position(tMid);
				 //f.pln(" $$$$ structRevertTurnTCP: use BOT position, if vertex angle is too small posMid = "+posMid);
			     vertex = new NavPoint(posMid,tMid);
			} else {
			     vertex = new NavPoint(interSec.first,tmInterSec);
			}
			if (zVertex >= 0) {
				vertex = vertex.mkAlt(zVertex);          // a better value for altitude obtained from previous internal BVS-EVS pair
			}
			// ======================== No Changes To Plan Before This Point ================================
			double gsInNext = vout.gs();
			if (killNextGsTCPs & nextEOTix+1 < pln.size()) {
				NavPoint npAfter = pln.point(nextEOTix+1);
				double dt = pln.getTime(nextEOTix+1) - pln.getTime(nextEOTix);
				//f.pln(" $$$$ structRevertTurnTCP: npAfter = "+npAfter.toStringFull()+" dt = "+dt);
				if (npAfter.isBGS() && dt < revertGsTurnConnectionTime) {  // note that makeKinematicPlan always makes it 1 sec after the EOT
					int ixEGS = pln.nextEGS(nextEOTix+1);				
					vout = pln.initialVelocity(ixEGS);
					gsInNext = vout.gs();
					//f.pln(" $$$$$ let's KILL TWO GS points AT "+(nextEOTix+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
					//f.pln(" $$$$$ KILL "+pln.point(ixEGS).toStringFull());
					//f.pln(" $$$$$ KILL "+pln.point(nextEOTix+1).toStringFull());
					pln.remove(ixEGS);
					pln.remove(nextEOTix+1);					
				}
			}
			// Kill all points between ix and nextEOTix
			// f.pln(" $$$$$ structRevertTurnTCP: ix = "+ix+" nextEOTix = "+nextEOTix);
			for (int k = nextEOTix; k >= ix; k--) {
				//f.pln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+pln.point(k).toStringFull());
				pln.remove(k);
			}
			//f.pln(" $$$$ structRevertTurnTCP: ADD vertex = "+vertex);
			int ixAdd = pln.add(vertex.makeLabel(label));
			int ixNextPt = ixAdd+1;
			// add back all removed points with revised position and time
			if (addBackMidPoints) {
				for (int i = 0; i < betweenPointDists.size(); i++) {
					double newTime = BOT.time() + betweenPointDists.get(i)/vin.gs();
					Position newPosition = pln.position(newTime);
					NavPoint savePt = betweenPoints.get(i);
					NavPoint np = savePt.makePosition(newPosition).makeTime(newTime).mkAlt(savePt.alt());
					pln.add(np);
					//f.pln(" $$$$ structRevertTurnTCP: ADD BACK np = "+np);
					ixNextPt++;
				}
			}
			// fix ground speed after
			//f.pln(" $$$$ structRevertTurnTCP: ixNextPt = "+ixNextPt+" gsInNext = "+Units.str("kn", gsInNext));
		    double tmNextSeg = pln.getTime(ixNextPt);
		    if (tmNextSeg > 0) { // if reverted last point, no need to timeshift points after dSeg
		    	int newNextSeg = pln.getSegment(tmNextSeg);
		    	double newNextSegTm = pln.linearCalcTimeGSin(newNextSeg, gsInNext);
		    	double dt2 = newNextSegTm - tmNextSeg;
		    	//f.pln(" $$$$$$$$ structRevertTurnTCP: dt2 = "+dt2);
		    	pln.timeshiftPlan(newNextSeg, dt2);   
		    }
		    //f.pln(" $$$$ structRevertTurnTCP: pln.initialVelocity("+ixNextPt+") = "+pln.initialVelocity(ixNextPt));			
			removeRedundantPoints(pln,pln.getIndex(tBOT),pln.getIndex(tEOT));
		}
	}

	
	
	// assumes ix > 0 AND ix < pln.size() and is a Gs TCP otherwise it does nothing
	// Note. if this GS segment is right after a turn then revert the turn as well
	public static int structRevertGsTCP(Plan pln, int ix, boolean revertPreviousTurn) {
		//f.pln(" $$$$>>>>>>>>> structRevertGsTCP: pln.point("+ix+") = "+pln.point(ix).toStringFull());
		if (pln.point(ix).isBGS()) {		
        	NavPoint ixP = pln.point(ix);
	   		if (ixP.isEGS()) ix = pln.prevBGS(ix);
	   		int ixPrev = pln.prevTCP(ix);
	   		NavPoint npPrev = pln.point(ixPrev);
	   		double dt = ixP.time() - npPrev.time();
	   		//f.pln(" $$$$$$$$$$$ structRevertGsTCP: npPrev = "+npPrev.toStringFull()+" dt = "+dt);
	   		if (npPrev.isEOT() && dt < revertGsTurnConnectionTime && revertPreviousTurn) {
	   			boolean killNextGsTCPs = true;
	   			int ixPrevBOT = pln.prevBOT(ixPrev);
	   			//f.pln(" $$$$$$$$$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
	   			boolean addBackMidPoints = false;
	   			structRevertTurnTCP(pln,ixPrevBOT, addBackMidPoints, killNextGsTCPs,-1.0);
	   			//f.pln(" $$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
	   			return ixPrevBOT;
	   		} else {
	   			//NavPoint BGS = pln.point(ix);
	   			int nextEGSix = pln.nextEGS(ix);
	   			if (nextEGSix == pln.size()-1) {
	   				f.pln(" $$$$ structRevertGsTCP: ERROR EGS cannot be last point! ");
	   			}
	   			NavPoint EGS = pln.point(nextEGSix);	
	   			NavPoint newPoint = EGS.makeNewPoint();
	   			pln.remove(nextEGSix);
	   			pln.remove(ix);
	   			pln.add(newPoint);
	   			return ix;
	   		}
		}
		return ix;
	}

	
	// assumes ix > 0 AND ix < pln.size()
	private static double structRevertVsTCP(Plan pln, int ix) {
		if (pln.point(ix).isBVS()) {
			//f.pln(" $$$$>>>>>>>>> structRevertVsTCP: pln.point("+ix+") = "+pln.point(ix).toStringFull());
			NavPoint BVS = pln.point(ix);
			int nextEVSix = pln.nextEVS(ix);
			if (nextEVSix == pln.size()-1) {
				f.pln(" $$$$ structRevertVerticalTCP: ERROR EVS cannot be last point! ");
			}
			NavPoint EVS = pln.point(nextEVSix);	
            NavPoint pp = pln.point(ix-1); 
			//NavPoint qq = pln.point(nextEVSix+1);
			double vsin = (BVS.z() - pp.z())/(BVS.time() - pp.time());
			double dt = EVS.time() - BVS.time();
			double tVertex = BVS.time() + dt/2.0;
			double zVertex = BVS.z() + vsin*dt/2.0;
			//f.pln(" $$$$ structRevertVsTCP: zVertex = "+Units.str("ft",zVertex));
			Position pVertex = pln.position(tVertex);
			NavPoint vertex = new NavPoint(pVertex.mkAlt(zVertex),tVertex);
			pln.remove(nextEVSix);
			pln.remove(ix);
			pln.add(vertex);
			return zVertex;
		}
		return -1;
	}

	/**
	 * Returns a new Plan that sets all points in a range to have a constant GS.
	 * THIS ASSUMES NO VERTICAL TCPS, but allows turns (turn omega metatdata may be altered -- maximum omega values are not tested for).
	 * The return plan type will be the same as the input plan type.
	 * The new gs is specified by the user, it must be > 0!
	 * This will set an error status in the return plan if there are any vertical TCPs.
	 */
	public static Plan makeGSConstant_No_Verts(Plan p, double newGs) {
		//f.pln(" makeGSConstant: newGs = "+Units.str4("kn",newGs)+"  type = "+p.getType());
		Plan kpc = new Plan(p.name,p.note);
		if (Util.almost_equals(newGs,0.0)) {
			kpc.addError(" 	makeGSConstant_No_Verts: newGs cannot be zero1");
			return kpc;
		}
		//kpc.setPlanType(p.getType());
		//double time0 = point(0).time();
		double lastTime = p.point(0).time();
		kpc.add(p.point(0));
		int j = 0;   // last point included in new file (note GS TCPs are removed)
		for (int i = 1; i < p.size(); i++) {
			NavPoint np = p.point(i);
			//f.pln(" $$$$ makeGSConstant: i = "+i+" np = "+np.toStringFull());
			// skip any GSC points (i.e. remove them)
			if (!np.isGsTCP()) {
				double d = p.pathDistance(j,i);
				double dt = d/newGs;
				double nt = lastTime + dt;
				//f.pln(" $$$$ makeGSConstant_No_Verts: i = "+i+" j ="+j+" d = "+Units.str("nm",d)+" dt = "+dt+" nt = "+nt);
				lastTime = nt;
				if (np.isTCP()) {        // assume only turn TCPs allowed
					if (!np.isTrkTCP()) {
						//f.pln("makeGSConstant(TCP): start point within acceleration zone");
						p.addError("TrajGen.genGSConstant: start point within acceleration zone "+i);
					}
					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" BEFORE np.turnRadius = "+np.turnRadius());
					double oldGs = np.velocityIn().gs();
					double newVsIn = p.vertDistance(j,i)/dt;
					np = np.makeVelocityIn(np.velocityIn().mkGs(newGs).mkVs(newVsIn));  // modify gs,vs in for turns
					//f.pln(" makeGSConstant(TCP): make point i = "+i+" have velocityIn = "+Units.str4("kn",newGs));
					// calculate new acceleration: newAccel = oldAccel * oldTime/newTime
					double k = newGs/oldGs;
					double newAccel;
					if (np.isBOT()) {                                 // ***RWB*** KCHANGE
						newAccel = np.trkAccel() * k;
						//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE Trkaccel from "+np.trkAccel()+" to "+newAccel);
				    	np = np.makeTrkAccel(newAccel); // modify turn omega to match new gs in
					} else if (np.isBGS()) {
						newAccel = np.gsAccel() * k;
						//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE gsAccel from "+np.gsAccel()+" to "+newAccel);
				    	np = np.makeGsAccel(newAccel); // modify gsAccel
					} else if (np.isBVS()) {
						newAccel = np.vsAccel() * k;
						//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" CHANGE vsAccel from "+np.vsAccel()+" to "+newAccel);
				    	np = np.makeVsAccel(newAccel); // modify turn omega to match new gs in
					}
					//f.pln(" $$$ makeGSConstant(TCP) for i = "+i+" AFTER np.turnRadius = "+np.turnRadius());
					// NOTE: we need to recalculate times of MOT and BOT, we are preserving turn radius
				}
				//f.pln(" $$$$$ nt = "+nt+" newGs = "+newGs);
				NavPoint newNp = np.makeTime(nt);
				//f.pln(" $$$ makeGSConstant for i = "+i+" CHANGE time from "+p.point(i).time()+" to "+newNp.time());
				//f.pln(" $$$ makeGSConstant_No_Verts add "+newNp);
				kpc.add(newNp);
				j = i;
			}
		}
		return kpc;
	}



	/**
	 * Returns a new Plan that sets all points in a range to have a constant GS.
	 * THIS ASSUMES NO VERTICAL TCPS, but allows turns (turn omega metatdata may be altered -- maximum omega values are not tested for).
	 * The return plan type will be the same as the input plan type.
	 * The new gs is an average over the whole plan.  End time should be preserved.
	 * This will set an error status in the return plan if there are any vertical TCPs.
	 */
	public static Plan makeGSConstant_No_Verts(Plan p) {
		double dt = p.getLastTime() - p.getFirstTime();
		double newGS = p.pathDistance()/dt;
		return makeGSConstant_No_Verts(p, newGS);
	}


	/**
	 * Make a new plan with constant ground speed from wp1 to wp2. 
	 * Assumes input plan is linear.
	 * 
	 *       200     200     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    linearMakeGSConstant(p, 1, 3, 500)
	 *    
	 *    	 200     500     500     200     200     200
	 *    0 ----- 1 ----- 2 ----- 3 ----- 4 ----- 5
	 * 
	 *    Note that if wp1 == wp2 no change is made.
	 */
	public static Plan linearMakeGSConstant(Plan p, int wp1, int wp2, double gs) {
		//f.pln("%%## linearMakeGSConstant:  wp1 = "+wp1+" wp2 = "+wp2+ " gs = "+Units.str("kn",gs));
		if (gs <= 0.0) {
			p.addError("PlanUtil.linearMakeGSConstant: cannot accept gs <= 0");
			return p;
		}
		//PlanUtil.savePlan(p,"dump_linearMakeGSConstant.txt");
		if (wp1 < 0) return p;
		if (wp2 >= p.size()) wp2 = p.size()-1;
		if (wp1 >= wp2) return p;
		Plan rtnPln = new Plan(p.getName(),p.getNote());
		// -------------- process indices [0,wp1] ----------------------------------
		for (int i = 0; i < wp1+1; i++) {
			//f.pln(" $$$$$BEFORE makeGSConstant  ADD  "+p.point(i));
			rtnPln.add(p.point(i)); 
		}
		//  -------------- process indices [wp1+1,wp2]  ----------------------------------
		double lastTime = p.getTime(wp1); // p.getTime(Math.min(3, p.size()-1));
		for (int i = wp1; i < wp2; i++) {
			double dt = p.pathDistance(i,i+1)/gs;
			//f.pln(" $$$ lastTime = "+lastTime);
			double newTime = lastTime+dt;
			NavPoint np = p.point(i+1).makeTime(newTime); 
			//f.pln(" $$$$$>>> linearMakeGSConstant CHANGE i+1 = "+(i+1)+" from tm = "+p.point(i+1).time()+" to newTime = "+newTime+" dt = "+dt);
			rtnPln.add(np);
			lastTime = newTime;
		}
		//  -------------- process indices [wp2+1,size()-1]  ----------------------------------
		for (int i = wp2+1; i < p.size(); i++) {
			double originalGs = p.initialVelocity(i-1).gs();
			double d = p.point(i-1).distanceH(p.point(i));
			//f.pln(" $$$$ calcTimeGSin: d = "+Units.str("nm",d));
			double newTime = rtnPln.point(i-1).time() + d/originalGs;						
			//f.pln(i+" $$$ originalGs = "+Units.str("kn", originalGs)+" newTime = "+newTime);
			NavPoint np = p.point(i).makeTime(newTime);
			//f.pln(" $$$$$ AFTER linearMakeGSConstant ADD Back  i = "+i+": "+np);			
			rtnPln.add(np); 			
			//rtnPln.add(p.point(i)); 			
		}
		//f.pln(" $$$$ linearMakeGSConstant rtnPln = "+rtnPln);
		if (p.size() != rtnPln.size()) {
			rtnPln.addError("PlanUtil.linearMakeGSConstant new plan size does not match old plan size!");
		}
		for (int i = wp1; i < wp2; i++) {
			if (!Util.almost_equals(rtnPln.initialVelocity(i).gs(), gs)) {
				rtnPln.addWarning("PlanUtil.linearMakeGSConstant i="+i+" gs is "+rtnPln.initialVelocity(i).gs()+", not "+gs+"!");								
			}
		}
		return rtnPln;
	}


	/**
	 * Make ground speed constant gs for entire plan.
	 * Assumes input plan is linear.
	 */
	public static Plan linearMakeGSConstant(Plan p, double gs) {
//		if (gs <= 0.0) {
//			p.addError("PlanUtil.linearMakeGSConstant: cannot accept gs = 0");
//			return p;
//		} else if (p.size() < 2) {
//			p.addError("PlanUtil.linearMakeGSConstant: plan too short!");
//			return p;
//		}
//		Plan pp = new Plan(p.getName());
//		pp.add(p.point(0));
//		for (int i = 1; i < p.size(); i++) {
//			NavPoint np1 = p.point(i-1);
//			NavPoint np2 = p.point(i);
//			double dist = np1.distanceH(np2);
//			//f.pln(" $$$ linearMakeGSConstant: to i = "+i+" linearMakeGSConstant dist = "+Units.str("nm",dist));
//			double dt = dist/gs;
//			if (Util.almost_equals(dist, 0.0)) {
//				dist = np1.distanceV(np2);
//				double vs = np1.initialVelocity(np2).vs();
//				if (!Util.almost_equals(vs, 0)) {
//					dt = dist/Math.abs(vs);
//				}
//			}
//			double t = pp.getTime(i-1)+dt;
//			pp.add(np2.makeTime(t));
//		}
//		return pp;
//f.pln("linearMakeGSConstant p="+p.toOutput()+" wp1=0 wp2="+(p.size()-1)+" gs="+gs);		
//DebugSupport.dumpPlan(p, "tbodynrnpbuggyIN");
		return linearMakeGSConstant(p,0,p.size()-1,gs);
	}

	/**
	 * Make ground speed constant gs between wp1 and wp2.
	 * Assumes input plan is linear.
	 */
	public static Plan linearMakeGSConstant(Plan p) {
		//f.pln(" $$$$$$ PlanUtil.linearMakeGSConstant: p = "+p);
		double dtot = p.pathDistance(0,p.size()-1);
		double ttot = p.getTime(p.size()-1) - p.getTime(0);
		double gs = dtot/ttot;
		return linearMakeGSConstant(p, 0,p.size()-1,gs);
	}


	/**
	 * Make ground speed constant between wp1 and wp2 as an average of the total distance and time travelled.
	 * Assumes input plan is linear.
	 */
	public static Plan linearMakeGSConstant(Plan p, int wp1, int wp2) {
		//f.pln("%%##  linearMakeGSConstant: wp1 = "+wp1+" wp2 = "+wp2);
		if (wp1 < 0) return p;
		if (wp2 >= p.size()) wp2 = p.size()-1;
		if (wp1 >= wp2) return p;
		double dtot = p.pathDistance(wp1,wp2);
		double ttot = p.getTime(wp2) - p.getTime(wp1);
		double gs = dtot/ttot;
		//f.pln(" linearMakeGSConstant: TARGET gs = "+Units.str("kn",gs)+"  from waypoint "+wp1+" to "+wp2);	
		return linearMakeGSConstant(p, wp1,wp2,gs);
	}






	/**
	 * Make vertical speed constant vs between wp1 and wp2.
	 * Assumes input plan is linear.
	 */
	public static void linearMakeVsConstant(Plan p, int start, int end, double vs) {
		//f.pln("%%## makeVsConstant:  wp1 = "+wp1+" wp2 = "+wp2+ " vs = "+Units.str("fpm",vs));
		if (start < 0) return;
		if (end >= p.size()) end = p.size()-1;
		if (start >= end) return;
		for (int i = start; i < end; i++) {
			double dt = p.point(i+1).time() - p.point(i).time();
			double newAlt = p.point(i).alt()+dt*vs;
			NavPoint np = p.point(i+1).mkAlt(newAlt); 
			//f.pln(" $$$$$>> makeVsConstant dt = "+dt+" vs = "+Units.str8("fpm",vs)+" ADD  "+np);
			//f.pln(" $$$$$>> makeVsConstant dt*vs = "+dt*vs+" REMOVE: "+p.point(i+1));
			//f.pln(" $$$$$>> makeVsConstant  p.point(i+1).alt() = "+Units.str("ft",p.point(i+1).alt())+" newAlt = "+Units.str("ft",newAlt));
			p.remove(i+1);
			p.add(np);
		}
	}

	/**
	 * Make vertical speed constant vs between wp1 and wp2, with vs being the average speed.
	 * Assumes input plan is linear.
	 */
	public static void linearMakeVsConstant(Plan p, int start, int end) {
		if (start < 0) return;
		if (end >= p.size()) end = p.size()-1;
		if (start >= end) return;
		double zStart = p.point(start).z();
		double zEnd = p.point(end).z();
		double dtot = zEnd - zStart;
		double ttot = p.getTime(end) - p.getTime(start);
		double vs = dtot/ttot;
		linearMakeVsConstant(p,start,end,vs);
	}


	/**
	 * Make vertical speed constant vs over plan with vs being the average speed.
	 * Assumes input plan is linear.
	 */
	public static void linearMakeVsConstant(Plan p) {
		linearMakeVsConstant(p,0,p.size()-1);
	}

	/**
	 * Make vertical speed constant vs for full plan.
	 * Assumes input plan is linear.
	 */
	public static void linearMakeVsConstant(Plan p, double vs) {
		linearMakeVsConstant(p,0,p.size()-1,vs);
	}



	
	public static boolean removeVirtuals(Plan ac) {
		return removeVirtualsRange(ac, ac.getFirstTime(), ac.getLastTime(), false);
	}

	/* Assumes ownship is plans.get(0);  Check if in LoS at time timeOfCurrentPosition
	 * 
	 */
	public boolean losChecker(ArrayList<Plan> plans, double timeOfCurrentPosition, double D, double H) {
		double currentTime;
		Plan ownship = plans.get(0);
		if (timeOfCurrentPosition >= 0) {
			if (timeOfCurrentPosition < ownship.getFirstTime() || timeOfCurrentPosition > ownship.getLastTime()) {
				f.pln(" Parameter timeOfCurrentPosition "+timeOfCurrentPosition+" occurs outside the ownship plan");
			} 
			currentTime = timeOfCurrentPosition;
			//f.pln(" Stratway:  set currentTime equal to timeOfCurrentPosition = "+currentTime);
		} else {
			currentTime = ownship.getFirstTime();
		}
		Position so = ownship.position(currentTime);
		for (int j = 1; j < plans.size(); j++) {
			if (plans.get(j).getSegment(currentTime) >= 0) { // time is in range
				Position sj = plans.get(j).position(currentTime);
				if (so.LoS(sj,D,H)) {
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * Convert from OLD NavPoint TCP data string to NEW NavPoint metadata string.
	 * Should work in most cases, though the old vin data (which may be incorrect) is preserved
	 */
	public static String convertOldTCPLabel(String oldLabel) {
		String newLabel = "";
		int j = -1;
		int first = Integer.MAX_VALUE;
		int i = -1;
		i = oldLabel.indexOf(":BOT:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "BOT:";
			i = oldLabel.indexOf(":ACC:"); // acceleration
			if (i >= 0) {
				j = oldLabel.indexOf(":",i+5);
				newLabel += "ATRK:"+oldLabel.substring(i+5,j+1);
			}
		}
		i = oldLabel.indexOf(":BGSC:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "BGS:";
			i = oldLabel.indexOf(":ACC:"); // acceleration
			if (i >= 0) {
				j = oldLabel.indexOf(":",i+5);
				newLabel += "AGS:"+oldLabel.substring(i+5,j+1);
			}
		}
		i = oldLabel.indexOf(":BVSC:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "BVS:";
			i = oldLabel.indexOf(":ACC:"); // acceleration
			if (i >= 0) {
				j = oldLabel.indexOf(":",i+5);
				newLabel += "AVS:"+oldLabel.substring(i+5,j+1);
			}
		}
		i = oldLabel.indexOf(":EOT:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "EOT:";
		}
		i = oldLabel.indexOf(":EGSC:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "EGS:";
		}
		i = oldLabel.indexOf(":EVSC:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "EVS:";
		}
		i = oldLabel.indexOf(":TMID:"); // becomes ADDED
		if (i >= 0) {
			first = Math.min(first,i);
			newLabel += "ADDED:";
		} else { // add source, if present
			i = oldLabel.indexOf(":SRC:");
			if (i >= 0) {
				first = Math.min(first,i);
				j = oldLabel.indexOf(":",i+5);
				newLabel += oldLabel.substring(i+1, j+1);
				i = oldLabel.indexOf(":",j+1);
				if (i > j) {
					newLabel += "STM"+oldLabel.substring(j, i+1);
				}
			}			
		}
		i = oldLabel.indexOf(":VEL:"); // velocity in
		if (i >= 0) {
			first = Math.min(first,i);
			j = oldLabel.indexOf(":",i+5);
			newLabel += oldLabel.substring(i+1, j+1);
		}

		
		i = oldLabel.indexOf(":mV:"); // minor -- just copy for now
		if (i >= 0) {
			first = Math.min(first,i);
			j = oldLabel.indexOf(":",i+4);
			newLabel += oldLabel.substring(i,j+1);
		}
		if (first < oldLabel.length() && !newLabel.endsWith(":")) { // ending colon
			newLabel += ":";
		}
		if (first >= oldLabel.length()) {
			newLabel = oldLabel;
		} else {
			newLabel = oldLabel.substring(0, first+1)+newLabel;
		}
f.pln("\n"+oldLabel);
f.pln(newLabel);
		return newLabel;
	}

	public static Plan subPlan(Plan plan, int startIx, int endIx) {
		if (startIx < 0) startIx = 0;
		if (endIx > plan.size()) endIx = plan.size() - 1;
		Plan rtn = new Plan("");
		for (int i = startIx; i <= endIx; i++) {
			f.pln(" $$$ subPlan: ADD plan.point(i) = "+plan.point(i).toStringFull());
			rtn.add(plan.point(i));
		}
		return rtn;
	}
	
	/**  make a new plan that is identical to plan except first part (up to startTime) and last part (after endTime) is thrown away
	 * 
	 * @param plan         source plan
	 * @param startTime    absolute time of start time
	 * @param endTime      absolute time of end time

	 * @return   truncated plan
	 */
	public static Plan cutDown(Plan plan, double startTime, double endTime) {
		Plan rtn = new Plan(plan.getName(),plan.getNote());
		int ix = plan.getIndex(startTime);
		NavPoint start;
		if (ix >= 0) {
            start = plan.point(ix);
		} else {
			Position startPos = plan.position(startTime);
			start = new NavPoint(startPos, startTime);
		}
		rtn.add(start);
		for (int i = 0; i < plan.size(); i++) {
			NavPoint pi = plan.point(i);
			if (startTime < pi.time() && pi.time() < endTime ) {
				rtn.add(pi);
			}
		}
		ix = plan.getIndex(endTime);
		NavPoint end;
		if (ix >= 0) {
			end = plan.point(ix);
		} else {
			Position endPos = plan.position(endTime);
			end = new NavPoint(endPos, endTime);
		}
        rtn.add(end);
		return rtn;
	}
	
	/** This method cuts a Plan so that the acceleration information after intentThreshold is discarded.  The plan
	 *  is continued linearly to time tExtend.  The first time point of the new plan is the 
	 *  NavPoint before timeOfCurrentPosition in the plan.  The  intentThreshold and tExtend times are absolute.
	 * 
	 * @param plan                      Plan file to be cut
	 * @param timeOfCurrentPosition     Current location of aircraft in the plan file
	 * @param intentThreshold           the absolute lookahead time -- all acceleration information after this time is not copied
	 * @param tExtend                   After the intentThreshold, the plan is extended linearly to this time (absolute time)
	 * @return
	 */
	public static Plan cutDownTo(Plan plan, double timeOfCurrentPosition, double intentThreshold, double tExtend) {
		Plan nPlan = new Plan(plan.getName(),plan.getNote());  // empty plan with same name and note
		if (intentThreshold < 0) {
			f.pln(" $$$ ERROR:  negative intentThreshold = "+intentThreshold);
			return nPlan;
		}
		if (plan.size() == 0) return plan;
        //f.pln(" $$$$$$$$$$$$$$$$ plan = "+plan);
		int ixInit = 0; 
		boolean inAccel = false;
		NavPoint firstPoint = plan.point(0);	
		if (timeOfCurrentPosition >= firstPoint.time()) {
			inAccel = plan.inAccel(timeOfCurrentPosition); 
			//if (inAccel) ix = prevNonTCP(ix);
			ixInit = plan.getSegment(timeOfCurrentPosition);
			//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> timeOfCurrentPosition = "+timeOfCurrentPosition+" ixInit = "+ixInit);
			if (ixInit < 0) {
				return new Plan(); // TODO -- what do we really want to do here ??       	
			}
			//f.pln(" $$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> inAccel = "+inAccel);
			if (inAccel) ixInit = plan.prevTCP(ixInit+1);            // NOTE THE ixInit+1
		}       
		//f.pln("\n $$$$ cutDownTo: ixInit = "+ixInit+" timeOfCurrentPosition = "+timeOfCurrentPosition+" intentThreshold = "+intentThreshold);
		// ADD FIRST POINT
		firstPoint = plan.point(ixInit);
		if (firstPoint.isEndTCP()) firstPoint = firstPoint.makeNewPoint();
		nPlan.add(firstPoint); 
		//f.pln(" $$$$0 cutDownTo: ADD INITIAL point("+ixInit+") = "+firstPoint.toStringFull());
		int ix = ixInit + 1;
		while (plan.point(ix).time() < intentThreshold && ix < plan.size()) {
			NavPoint p = plan.point(ix);
			nPlan.add(p);
			//f.pln(" $$$$$$ cutDownTo ADD p = "+p);
			ix++;
		}
		if (ix >= plan.size()) { // plan ran out before intentThreshold
			double dt = intentThreshold - plan.getLastTime();
			Velocity vout = plan.initialVelocity(plan.size()-1);
			NavPoint lastP = plan.point(plan.size()-1);
			NavPoint np = lastP.linear(vout, dt).makeLabel("CutDownTo_0");
			nPlan.add(np);
			//f.pln(" $$$$$$ cutDownTo ADD np = "+np);
		} else {                // intentThreshold is within plan
			NavPoint newLastPt = new NavPoint(plan.position(intentThreshold),intentThreshold).makeLabel("CutDownTo_newLastPt");
			Velocity vout = plan.velocity(intentThreshold);
			if (! plan.inAccel(intentThreshold)) {
				//f.pln(" $$$$$$ cutDownTo ADD newLastPtp = "+newLastPt+" vout = "+vout);
				nPlan.add(newLastPt);
			} else {
				boolean inTurn = plan.inTrkChange(intentThreshold);
				boolean inGsAccel = plan.inGsChange(intentThreshold);
				boolean inVsAccel = plan.inVsChange(intentThreshold);
				NavPoint.Trk_TCPType trkType = NavPoint.Trk_TCPType.NONE;
				NavPoint.Gs_TCPType gsType = NavPoint.Gs_TCPType.NONE;
				NavPoint.Vs_TCPType vsType = NavPoint.Vs_TCPType.NONE;			 
                if (inTurn) trkType = NavPoint.Trk_TCPType.EOT;
                if (inGsAccel) gsType = NavPoint.Gs_TCPType.EGS;
                if (inVsAccel) vsType = NavPoint.Vs_TCPType.EVS;             
                NavPoint lastP = NavPoint.makeFull(newLastPt.position(), newLastPt.time(), NavPoint.WayType.Orig,  "", trkType,  gsType, vsType,
                        0.0, 0.0, 0.0, vout, 0.0, newLastPt.position(), newLastPt.time()).makeLabel("CutDownTo_lastP");
                nPlan.add(lastP);
                //f.pln(" $$$$$$ cutDownTo ADD lastP = "+lastP+" vout = "+vout);
  			}			
		}
		NavPoint lastPt = nPlan.getLastPoint();
		if (tExtend > intentThreshold && tExtend > lastPt.time()) {
			//Velocity vout = nPlan.finalVelocity(nPlan.size()-2);  // final velocity of next to last point
			Velocity vout = nPlan.initialVelocity(nPlan.size()-1);  // final velocity of next to last point
			double dt = tExtend - lastPt.time();
			//if (timeOfCurrentPosition == 3900) 
			//    	f.pln(" $$$$.............. cutDownTo: dt = "+dt+" vout = "+vout);
			NavPoint extendPt = lastPt.linear(vout, dt).makeLabel("CutDownTo_extendPt");
			nPlan.add (extendPt);
		}
		//f.pln(" $$$$ cutDownTo: nPlan = "+nPlan);
		return nPlan;
	}
	
	public static Plan cutDownTo(Plan plan, double timeOfCurrentPosition, double intentThreshold) {
       return cutDownTo(plan, timeOfCurrentPosition, intentThreshold, intentThreshold);
	}
	
	// return true if enough distance for the current speed and specified acceleration (maxAccel)
	// return new time at ix+1 (and hence new ground speed) that is achievable if not enough distance
	public  static Pair<Boolean,Double> enoughDistanceForAccel(Plan p, int ix, double maxAccel) {
		if (ix == 0 || ix == p.size()-1) return new Pair<Boolean,Double>(true,0.0);
		double dt = p.getTime(ix+1) - p.getTime(ix);
		double gsIn = p.initialVelocity(ix-1).gs();
		double gsOut = p.initialVelocity(ix).gs();
		//f.pln(" $$ enoughDistanceForAccel: gsIn = "+Units.str("kn",gsIn)+" gsOut = "+Units.str("kn",gsOut));
		double deltaGs = gsOut - gsIn;
		double a = Util.sign(deltaGs)*maxAccel;
		//f.pln(" $$ enoughDistanceForAccel: ix = "+ix+"   deltaGs = "+Units.str("kn",deltaGs)+" actual accel = "+deltaGs/dt);
		double dtNeeded = deltaGs/a;
		//f.pln(" $$ enoughDistanceForAccel: dtNeeded = "+dtNeeded);
		double distanceNeeded = gsIn*dtNeeded + 0.5*a*dtNeeded*dtNeeded;
		double distanceBetween = p.pathDistance(ix,ix+1);
		//f.pln(" $$ enoughDistanceForAccel: distanceBetween = "+distanceBetween+" distanceNeeded = "+distanceNeeded);		
		boolean rtn = distanceNeeded <= distanceBetween;
		if (rtn) return new Pair<Boolean,Double>(true,0.0);
		else {
			double b = gsIn;
	        double c = -distanceBetween;	        
	        double dtp = (-b+Math.sqrt(b*b - 2*a*c))/(a);
	        double dtn = (-b-Math.sqrt(b*b - 4*a*c))/(2.0*a);	  
	        double tmRtn = dtp;
	        //f.pln(" $$ enoughDistanceForAccel: dt = "+dt+" tmRtn = "+tmRtn+" dtNeeded = "+dtNeeded);
	        //f.pln(" $$ enoughDistanceForAccel: max delta GS = "+Units.str("kn",a*tmRtn));	        	        	        
	        return new Pair<Boolean,Double>(false, tmRtn);
		}
	}
	
	
	// Fix Plan p at ix if there is not enough distance for the current speed and specified acceleration (maxAccel)
	// It makes the new ground speed as close to the original as possible (that is achievable over the distance)
    // return -1 if no change was necessary,
	// otherwise return the new time at ix+1
	public static double fixGsAccelAt(Plan p, int ix, double maxAccel) {
		    Pair<Boolean,Double> pEnoughDist = enoughDistanceForAccel(p, ix, maxAccel);
		    if ( ! pEnoughDist.first) {
		    	double dtNew = pEnoughDist.second;
				double dtCurrent = p.getTime(ix+1) - p.getTime(ix);
				double correction = (dtNew-dtCurrent);
		    	//f.pln(" $$$$>>>>>>>>>>>>>>>>>>>>>.. fixGsAccelAt: ix = "+ix+" dtCurrent = "+dtCurrent+" dtNew = "+dtNew+" correction = "+correction);
                p.timeshiftPlan(ix+1,correction);	  // 2.2 about right
		    }
		    return -1;
	}
	
	
	public static void fixGsAccel(Plan p, int start, int end, double maxAccel) {
         for (int j = start; j <= end; j++) {
        	 fixGsAccelAt(p,j,maxAccel);
         }
	}
	
	public static void fixGsAccelAfter(Plan p, int start, double maxAccel) {
        for (int j = start; j < p.size(); j++) {
       	    fixGsAccelAt(p,j,maxAccel);
        }
	}


	// pretty sure this is incorrect!  GEH
//	public static int nearestSegment(Plan p, Position q) {
//		NavPoint lastPoint = NavPoint.INVALID;
//		double minDist = Double.MAX_VALUE;
//		int minIndex = -1;
//		for (int j = 0; j < p.size(); j++) {
//			NavPoint np = p.point(j);
//			if (j > 0) {
//				double dist = Position.perp_distance(lastPoint.position(), np.position(), q);
//				f.pln(j+" $$ nearestSegment: dist = "+dist);
//				if (dist < minDist) {
//					minIndex = j-1;
//					minDist = dist;
//					f.pln(" $$ nearestSegment: minIndex = "+minIndex+" minDist = "+minDist);
//				}
//			}
//			lastPoint = np;
//		}
//		return minIndex;
//	}
	


//	/**
//	 * Cut a Plan down to contain "numTCPs" future TCPS past the current time (i.e. timeOfCurrentPosition). If tExtend
//	 * is greater than 0, create an extra leg after the last TCP with a duration of "tExtend".  This is intended
//	 * to mimic having only state information after the last TCP.  Note that numTCPs should be interpreted as number of
//	 * acceleration zones.  That is  [BOT,EOT] counts as one TCP.  Similarly [BGS, EGS] is one TCP.
//	 * 
//	 * This method eliminates waypoints earlier than timeOfCurrentPosition as much as possible.  If the aircraft
//	 * is in an acceleration zone at timeOfCurrentPosition, then it retains the plan back to the last begin TCP.
//	 * 
//	 * @param numTCPs  maximum number of TCPs to allow in the future, see note above.
//	 * @param timeOfCurrentPosition  indicates current location of aircraft, if negative, then aircraft is at point 0.
//	 * @param tExtend  amount of additional time to extend the plan after last TCP end point.
//	 * 
//	 * NOTE: THIS CODE WILL NOT WORK WITH OVERLAPPING HORIZONTAL/VERTICAL Accel Zones
//	 * 
//	 * @return
//	 */
	public static Plan cutDownToByCount(Plan plan, int numTCPs, double timeOfCurrentPosition, double tExtend) {
        int ix = plan.getSegment(timeOfCurrentPosition);
        int cnt = 0;
        if (plan.inAccel(timeOfCurrentPosition)) cnt = 1;
      	while (cnt < 2*numTCPs && ix < plan.size()) {
			//f.pln(" $$$$ cutDownTo point ix = "+ix+" point(ix) = "+point(ix).toStringFull());
      		NavPoint p = plan.point(ix);
			if (p.isTCP()) cnt++;	
		    ix++;
      	}
      	double lastTm = plan.point(ix).time() + 5.0;
      	return cutDownTo(plan,timeOfCurrentPosition,lastTm, lastTm+tExtend);
	}
	
	/** Trim down a plan by index and keep the portion from startIndex to endIdex, inclusive
	 * 
	 * @param plan              plan to be cut
	 * @param startIndex        first index  to be retained
	 * @param endIndex          last index to be retained
	 * @return   plan[startIndex,endIndex] or null if error on inputd
	 */
	public static Plan cutDownPlanByIndex(Plan plan, int startIndex, int endIndex) {
		if (startIndex<0 || startIndex>plan.size()-1) {
			//f.pln(" $$$$ TBOUtil.cutDownPlanByIndex: startIndex out of bounds, startIndex="+startIndex+", plan.size()="+plan.size());
			return null;
		}
		if (endIndex<0 || endIndex>plan.size()-1) {
			//f.pln(" $$$$ TBOUtil.cutDownPlanByIndex: endIndex out of bounds, endIndex="+endIndex+", plan.size()="+plan.size());
			return null;
		}
		if (startIndex>endIndex) {
			//f.pln(" $$$$ TBOUtil.cutDownPlanByIndex: startIndex is larger than endIndex, startindex="+startIndex+", endIndex="+endIndex);
			return null;
		}
		Plan rtn = new Plan(plan.getName(),plan.getNote());
		for (int i = startIndex; i <= endIndex; i++) {
            rtn.add(plan.point(i));
		}
		return rtn;
	}

	
	public static int hasPointsTooClose(Plan plan) {
        for (int i = 0; i < plan.size()-1; i++) {
        	NavPoint pi = plan.point(i);
        	NavPoint pn = plan.point(i+1);	
        	if (pi.almostEquals(pn)) {
        	    return i;
        	}        	
        }
        return -1;
	}
	
	
	
	static double diffMetric(Plan lpc, Plan kpc) {
        double sumSqDist = 0.0;
        int nPoints = 0;
		for (int i = 0; i < lpc.size(); i++) {   	   
    	   Position lpcPos = lpc.point(i).position();
    	   Position kpcPos = kpc.point(i).position();
			double distH = lpcPos.distanceH(kpcPos);  
			double distV = lpcPos.distanceV(kpcPos);
			double dist = Math.sqrt(distH*distH+distV*distV);
			sumSqDist = sumSqDist + dist*dist;
			//f.pln(tm+" dist = "+Units.str("nm",dist));
			nPoints++;
		}
		//f.pln(" $$ computeMetric: "+sumSqDist+" "+nPoints);
		return Math.sqrt(sumSqDist)/nPoints;
	}

	
	// will not remove segments that are longer than maxLegSize
    public static Plan unZigZag(Plan pp) {
         return unZigZag(pp, Double.MAX_VALUE);
    }

	// will not remove segments that are longer than maxLegSize
    public static Plan unZigZag(Plan pp, double maxLegSize) {
    	Plan p = pp.copy();
		double lastEvenTrack = 0.0;
		double lastOddTrack = 0.0;	
		Boolean[] prevTheSame = new Boolean[p.size()];	
		for (int i = 0; i < p.size(); i++) {
			Velocity v_i = p.initialVelocity(i);
			//f.pln(" $$ reRouteWx: v_"+i+" = "+v_i);
			double currentTrk = v_i.trk();
			if (i % 2 == 0) {  // even
				prevTheSame[i] = (Util.turnDelta(lastEvenTrack,currentTrk) < Units.from("deg",5));
				lastEvenTrack = currentTrk;
			} else {
				prevTheSame[i] = Util.turnDelta(lastOddTrack,currentTrk) < Units.from("deg",5);
				lastOddTrack = currentTrk;
			}
		}
//		for (int i = 0; i < p.size(); i++) {
//			if (i % 2 != 0) f.p("      ");
////			f.pln(" "+prevTheSame[i]);
//		}		
		int cntZigZag = 0;
		boolean lastTheSame = false;
		for (int j = p.size()-1; j >= 0; j--) {
            if (prevTheSame[j] || lastTheSame) cntZigZag++;
            else cntZigZag = 0;
            lastTheSame = prevTheSame[j];
            if (cntZigZag > 1 && cntZigZag % 2 == 0) {
            	if (j > 1 && p.pathDistance(j) <= maxLegSize && p.pathDistance(j-1) <= maxLegSize) {        // do not remove point 1, it preserves current velocity (lead in)
            		//f.pln(" $$$$$$$$$ unZigZag: REMOVE j = "+j);
            		p.remove(j);
            	}
            }
            if (!lastTheSame) cntZigZag = 0;
 		}
		p = linearMakeGSConstant(p);
		//f.pln(" $$$$ END: unZigZag: p = "+p);
    	return p;
    }
    
    private static boolean aboutTheSameTrk(Velocity v1, Velocity v2, double sameTrackBound) {
    	return Math.abs(v1.trk()- v2.trk()) < sameTrackBound;
    }

    public static Plan removeCollinearTrk(Plan pp, double sameTrackBound){
    	if (pp.size() < 1) return new Plan(pp.getName(),pp.getNote());
    	//f.pln(" $$ removeCollinearTrk: pp = "+pp);
    	Plan p = new Plan(pp.getName(),pp.getNote());
     	Velocity lastVel = pp.initialVelocity(0);
    	p.add(pp.point(0));
    	for (int j = 1; j < pp.size()-1; j++) {
    		Velocity vel = pp.initialVelocity(j);
    		boolean same = aboutTheSameTrk(lastVel,vel,sameTrackBound);
            //f.pln(j+" $$ removeCollinear: lastVel = "+lastVel+" vel = "+vel+" same = "+same);   		
    		if (!same) {
    			p.add(pp.point(j));
    		}
    		lastVel = vel;
    	}
    	p.add(pp.getLastPoint());
    	//f.pln(" $$ removeCollinearTrk: pp.size() = "+pp.size()+" p.size() = "+p.size());
		return p;
	}
    
//	static boolean isAltOnly(Plan traj, int j) {
//		if (j <= 0 || j >= traj.size()-1) return false;
//		if (traj.point(j).isTCP()) return false;           // in case turns have already been kinematically generated 
//		double gsIn = traj.finalVelocity(j-1).gs();
//		double gsOut = traj.initialVelocity(j).gs();
//		double trkIn = traj.finalVelocity(j-1).trk();
//		double trkOut = traj.initialVelocity(j).trk();
//		if (Math.abs(trkOut-trkIn) < Units.from("deg",2.0) 
//				&& Math.abs(gsOut-gsIn) < Units.from("kn",2.0)) return true;
//		return false;
//	}
    
    /**
     * Return true if plan p contains at least one point with the given label
     */
    public static int containsLabel(Plan p, String label) {
    	for (int i = 0; i < p.size(); i++) {
    		if (p.point(i).label().equals(label)) {
    			return i;
    		}		
    	}
    	return -1;
    }
    
    public static double crudeMinDistanceBetween(Plan p, Plan q) {
    	double maxFirstTime = Math.max(p.getFirstTime(), q.getFirstTime());
    	double minLastTime = Math.min(p.getLastTime(), q.getLastTime());
    	if (minLastTime < maxFirstTime) return Double.MAX_VALUE;
    	double minDistBetween = p.position(minLastTime).distanceH(q.position(minLastTime));
    	for (double t = maxFirstTime; t <= minLastTime; t = t + 1800) {
    		double distBetween = p.position(t).distanceH(q.position(t));
    		if (distBetween < minDistBetween) minDistBetween = distBetween;
    	}
    	return minDistBetween;
    }
    
    /**
     * Return the time where plan a intersects horizontally with segment b1-b2, or -1 if there is no intersection.
     * b1 and b2 should be 
     */
    public static double intersectionTimeHorizontal(Plan a, Position b1, Position b2) {
    	for (int i = 1; i < a.size(); i++) {
    		double tprev = a.getTime(i-1);
    		double t = a.getTime(i);
    		if (tprev >= 0) {
    			double dt = t - tprev;
    				if (a.isLatLon()) {
    					LatLonAlt x1 = a.point(i-1).lla();
    					LatLonAlt x2 = a.point(i).lla();
    					LatLonAlt y1 = b1.lla();
    					LatLonAlt y2 = b2.lla();
    					double ti = GreatCircle.intersection(x1, x2, dt, y1, y2).second;
    					if (ti >= 0 && ti < dt) return tprev + ti;
    				} else {
    					Vect3 x1 = a.point(i-1).point();
    					Vect3 x2 = a.point(i).point();
    					Vect3 y1 = b1.point();
    					Vect3 y2 = b2.point();
//f.pln("x1="+x1+" x2="+x2+" y1="+y1+" y2="+y2);
    					double ti = VectFuns.intersection(x1, x2, dt, y1, y2).second;
    					f.pln(" intersectionTimeHorizontal: $$$$$$$$$$$ tprev = "+tprev+" ti = "+ti);
    					if (ti >= 0 && ti < dt) return tprev + ti;    					
    				}
    		}
    	}
    	return -1.0;
    }
    
    public static int positionIndex(Plan a, Position p) {
    	for (int i = 0; i < a.size(); i++) {
    		if (a.point(i).position().equals(p)) {
    			return i;
    		}
    	}
    	return -1;
    }

    /**
     * Return true if Plan a contains at least n consecutive points from plan b
     */
    public static boolean containsSubPlan(Plan a, int n, Plan b) {
    	int sz = 0;
    	int j = -1;
    	for (int i = 0; i < b.size(); i++) {
    		int k = positionIndex(a,b.point(i).position());
    		if (j == -1) {
    			sz = 1;
    			j = k;
    		} else if (j == k-1) {
    			sz++;
    			j = k;
    		} else {
    			sz = 0;
    			j = -1;
    		}
    		if (sz >= n) return true;
    	}
    	return false;
    }
    
    public static Plan timeShiftPlan(Plan p, int start, double dt) {
    	Plan nPlan = p.copy();
    	nPlan.timeshiftPlan(start,dt);
    	return nPlan;
    }
    
	public static boolean checkMySolution(Plan solution, double currentTime, Position currentPos, Velocity currentVel) {
		boolean ok = true;
		if (!solution.position(currentTime).almostEquals(currentPos)) {
			f.pln("\n---------------------------------------------------------------------------------");
			f.pln(" ............... Stratway ERROR: moved location of current position! currentTime = "+currentTime+" .......");
			f.pln(" ............... from "+currentPos+" to "+solution.position(currentTime));
			f.pln("----------------------------------------------------------------------------------\n");
			//PlanCollection pc = new PlanCollection(plans);
			//pc.setPlan(0, saveMyPlan.copy());
			//StratUtil.dumpPlans(pc,"moved",this.getTimeOfCurrentPosition());				
			//DebugSupport.halt();
			ok = false;
		}
		//f.pln(" Stratway: Velocity Test AT END "+mySolution.velocity(currentTime)+" "+currentVel);
		if (!solution.velocity(currentTime).within_epsilon(currentVel,0.10)) {
			f.pln("\n---------------------------------------------------------------------------------");
			f.pln(" ............... Stratway ERROR: changed **velocity** of current position! currentTime = "+currentTime+" ....");
			f.pln(" ............... from "+currentVel+ " to "+solution.velocity(currentTime));
			f.pln("----------------------------------------------------------------------------------\n");
			//PlanCollection pc = new PlanCollection(plans);
			//pc.setPlan(0, saveMyPlan.copy());
			//StratUtil.dumpPlans(pc,"velChanged",this.getTimeOfCurrentPosition());				
			//DebugSupport.halt();
			ok = false;
		}
		if (!solution.isWellFormed()) {
			f.pln("\n---------------------------------------------------------------------------------");
			f.pln(" ............... Stratway ERROR: solution is not wellFormed currentTime = "+currentTime+" ....");
			f.pln(" ..............."+solution.strWellFormed());
			f.pln(" mySolution = "+solution.toString());
			f.pln("----------------------------------------------------------------------------------\n");
			ok = false;
		}
		return ok;
	}

    
}

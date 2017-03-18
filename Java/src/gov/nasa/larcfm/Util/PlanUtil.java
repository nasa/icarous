/*
 * PlanUtil - support utility functions for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

/**
 * Utilities to operate on Plans
 * 
 */
public class PlanUtil {


	public static Plan makeSourceNew(Plan lpc) {
		Plan npc = new Plan(lpc.getName(),lpc.getNote());
		for (int i = 0; i < lpc.size(); i++) {
			NavPoint np = lpc.point(i);
			TcpData tcp = lpc.getTcpData(i);
			tcp.reset();
			npc.add(np, tcp);                  // TODO: is this tight?
		}
		return npc;
	}

	public static boolean gsConsistent(Plan p, int ixBGS,  double distEpsilon,	 boolean silent) {
 		if ( ! p.isBGS(ixBGS)) return true;
		NavPoint BGS = p.point(ixBGS);
		int ixEGS = p.nextEGS(ixBGS);//fixed
		NavPoint EGS = p.point(ixEGS);
		//f.pln("$$$ gsConsistent: at ixBGS = "+ixBGS+" ixEGS = "+ixEGS);
		double gsOutBGS = p.gsOut(ixBGS);
		//f.pln("$$$ gsConsistent:  gsOutEGS = "+Units.str("kn",gsOutEGS));
		boolean rtn = true;
		double dt = EGS.time() - BGS.time();
		//f.pln("$$$ gsConsistent:   dt = "+dt);
		double a_BGS = p.gsAccel(ixBGS);	 	
//		double acalc = (gsOutEGS - gsOutBGS)/(dt); 
//		//f.pln("$$$ gsConsistent: at ixBGS = "+ixBGS+" gsOutBGS = "+Units.str("kn",gsOutBGS)+" gsOutEGS = "+Units.str("kn",gsOutEGS)+" dt = "+dt+" a_BGS = "+a_BGS+" acalc = "+acalc);
//		if (!Util.within_epsilon(a_BGS, acalc, accelEpsilon)) { // 0.00001)) {
//			double deltaAccel = Math.abs(a_BGS-acalc);
//			if (!silent) {
//				f.pln(" >>> gsConsistent: GS FAIL! at i = "+ixBGS+" GSC section fails ACCEL test at t = "+f.Fm2(p.time(ixBGS))+": a_BGS = "+f.Fm4(a_BGS)+ " acalc = "+f.Fm4(acalc));
//			}
//			if (deltaAccel > 0.5) f.pln(" gsConsistent:  deltaAccel = "+f.Fm4(deltaAccel));
//			rtn = false;
//		}
		double ds = gsOutBGS*dt + 0.5*a_BGS*dt*dt;
		//f.pln(" gsConsistent: gsOutBGS = "+gsOutBGS+" "+Units.str("kn", gsOutBGS, 4)+" dt = "+dt+" a_BGS = "+a_BGS);
		double distH = 	p.pathDistance(ixBGS,ixEGS);		
		double absDiff = Math.abs(ds-distH);
        //f.pln(" gsConsistent: ds = "+ds+ " distH = "+distH+" absDiff = "+absDiff);
		if (absDiff > distEpsilon) { // See testGsSimple for worst case 0.07 //TODO FIX		
			if ( ! silent) f.pln(" >>> gsConsistent: GS FAIL! at i = "+ixBGS+" GSC section fails Test! absDiff = "+Units.str("ft",absDiff,6));
			if ( ! silent && absDiff > Units.from("m", 10.0)) f.pln(" gsConsistent:  distanceH = "+Units.str("ft",absDiff,8));
			double t = p.time(ixBGS);
			if ( ! silent) f.pln(" >>> gsConsistent gsaccel="+p.inGsChange(t)+" trkaccel="+p.inTrkChange(t)+" vsaccel="+p.inVsChange(t));
			rtn = false;
		}
		return rtn;				
		//return gsConsistent(i, BGS, EGS, accelEpsilon, distEpsilon, gsOutBGS, gsOutEGS, silent);
	}



	public static boolean vsConsistent(Plan p, int ixBVS,  double distEpsilon, boolean silent) {
		if ( ! p.isBVS(ixBVS)) { // must be a BVS
			return true;
		}		
		int ixEVS = p.nextEVS(ixBVS);//fixed
		NavPoint VSCBegin = p.point(ixBVS);
		NavPoint VSCEnd = p.point(ixEVS);
		double a = p.vsAccel(ixBVS);
		boolean rtn = true;
		double dt = VSCEnd.time() - VSCBegin.time();
		double ds = p.vsOut(ixBVS)*dt + 0.5*a*dt*dt;
		double deltaAlts = VSCEnd.alt() - VSCBegin.alt();		
		double absDiff = Math.abs(ds-deltaAlts);
		//f.pln(" $$$ vsConsistent i = "+i+" dt = "+dt+" ds = "+Units.str8("ft",ds)+ " distV = "+Units.str8("ft",distV)+" absDiff = "+Units.str8("ft",absDiff));
		if (absDiff > distEpsilon) { 
			if (!silent) {
				f.pln(" >>> vsConsistent: VS FAIL! at idxBegin = "+ixBVS+" VSC Section fails Test! absDiff = "+Units.str("m",absDiff,8));
				f.pln(" >>> vsConsistent: ds = "+Units.str("ft",ds,8)+ " distV = "+Units.str("ft",deltaAlts,8));
			}
			if ( ! silent && absDiff > 10.0*distEpsilon) f.pln(" vsConsistent: absDiff = "+Units.str("ft",absDiff,8));
			rtn = false;
		}
		return rtn;

		
		
		
	}

	
	/** Calculate the distance from the Navpoint at "seq" to plan position at time "t"
	 * 
	 * @param p    plan
	 * @param seg  segment number
	 * @param t    time
	 * @param linear true, if linear
	 * @return distance
	 */
	public static double distFromPointToTime(Plan p, int seg, double t, boolean linear) {
		NavPoint np1 = p.point(seg);
		double distFromSo = 0;
		double gs0 = p.gsOut(seg,linear);
		double dt = t - np1.time();		
		if (p.inGsChange(t) && ! linear) { 		//TODO: POSSIBLE BUG What if t is not on segment seg? 
			double gsAccel = p.gsAccel(p.prevBGS(seg+1));
			distFromSo = gs0*dt + 0.5*gsAccel*dt*dt;
			//f.pln(" $$$ positionVelocity(inGsChange A): dt = "+f.Fm2(dt)+" vo.gs() = "+Units.str("kn",gs0)+" distFromSo = "+Units.str("ft",distFromSo));
		} else {
			distFromSo = gs0*dt;
			//f.pln(" $$$ positionVelocity(! inGsChange B): dt = "+f.Fm4(dt)+" gs0 = "+Units.str("kn",gs0)+" distFromSo = "+Units.str("ft",distFromSo));
		}
		return distFromSo;
	}


	public static boolean turnConsistent(Plan p, int i, double distH_Epsilon, boolean silent) {
		boolean rtn = turnCenterConsistent(p,i,distH_Epsilon,silent);		
		if ( ! p.isBOT(i)) return rtn;
		NavPoint BOT = p.point(i);
		int ixEOT = p.nextEOT(i);
		NavPoint EOT = p.point(ixEOT);	
		double pathDist = p.pathDistance(i,ixEOT);
		//f.pln(" $$$$>>>> turnConsistent  ixEOT = "+ixEOT+" EOT = "+EOT+" pathDist = "+Units.str("ft",pathDist));
		double signedRadius = p.signedRadius(i);	
		Position center = p.turnCenter(i);
   	    //f.pln(" $$$ turnConsistent: signedRadius = "+Units.str("ft",signedRadius,4)+" center = "+center);
		int dir = Util.sign(signedRadius);			
	    Position tAtd = KinematicsPosition.turnByDist2D(BOT.position(), center, dir, pathDist);
	    Position EOTcalc = tAtd.mkAlt(EOT.alt());// TODO: should we test altitude?
	    //f.pln(" $$$$$ EOTcalc = "+EOTcalc); // +" EOTcalc2 = "+EOTcalc2);
		if ( ! EOT.position().almostEquals2D(EOTcalc,distH_Epsilon)) { 
			if ( ! silent) {
				double distanceH = EOT.position().distanceH(EOTcalc);
				//double distanceV = EOT.position().distanceV(EOTcalc);
				f.pln(" >>> turnConsistent: TURN FAIL! i = "+f.padLeft(i+"", 2)+" calculated pos = "+EOTcalc.toString(8));
				f.pln("                                             plan EOT = "+EOT.position().toString(8));
				f.pln("            .... distanceH = "+Units.str("m",distanceH,8));
				//f.pln("            .... distanceV = "+Units.str("m",distanceV,8));
			}
			rtn = false;
		}
		return rtn;								
	}

	public static boolean isTrkContinuous(Plan p, int i, double trkEpsilon, boolean silent) {
		boolean rtn = true;
		double trkIn = p.trkIn(i);
		double trkOut = p.trkOut(i);
		double trkDelta = Util.turnDelta(trkIn,trkOut);
		if ( Math.abs(trkDelta) > trkEpsilon) {
			if (!silent) f.pln(" $$ isTrkContinuous: FAIL trkDelta ("+i+") = "+Units.str("deg",trkDelta));
			rtn = false; 
		}					
        return rtn;
	}
	
	public static boolean isGsContinuous(Plan p, int i, double gsEpsilon, boolean silent) {
		boolean rtn = true;
		double gsIn =   p.gsIn(i); 
		double gsOut = p.gsOut(i); 
		double gsDelta = gsOut - gsIn;
		if (Math.abs(gsDelta) > gsEpsilon) {
			if (!silent) {
				f.pln(" $$ isGsContinuous: FAIL gsIn ("+i+") = "+Units.str("kn",gsIn)+"  gsOut ("+i+") = "+Units.str("kn",gsOut)+" gsDelta = "+Units.str("kn",gsDelta));
			}
			rtn = false; 
		}
        return rtn;
	}
	
	public static boolean isVsContinuous(Plan p, int i, double velEpsilon, boolean silent) {
		boolean rtn = true;
		double vsIn  = p.vsIn(i); 
		double vsOut = p.vsOut(i); 
		double vsDelta = vsIn - vsOut;
		if (Math.abs(vsDelta) > velEpsilon) {
			if (!silent) {
				f.pln(" $$ isVsContinuous: FAIL vsIn ("+i+") = "+Units.str("fpm",vsIn)+" vsOut ("+i+") = "+Units.str("fpm",vsOut)+" vsDelta  = "+Units.str("fpm",vsDelta));
			}
			rtn = false; 
		}			
        return rtn;
	}
	
//	public static boolean isVelocityContinuous(Plan p, int i, double velEpsilon, boolean silent) {
//        boolean rtn = isTrkContinuous(p,i,velEpsilon,silent)
//                      && isGsContinuous(p,i,velEpsilon,silent)
//                      && isVsContinuous(p,i,velEpsilon,silent);
//	    return rtn;
//	}

	/** Checks to make sure that turnCenter, radius and location of BOT is consistent
	 * 
	 * @param p                plan
	 * @param i                index point
	 * @param distH_Epsilon    error bound
	 * @param silent
	 * @return
	 */
	static boolean turnCenterConsistent(Plan p, int i,  double distH_Epsilon, boolean silent) {
		boolean rtn = true;
		if (p.inTrkChange(p.point(i).time())) {
			int ixBOT = p.prevBOT(i+1);
			Position center = p.turnCenter(ixBOT);
			double distanceFromCenter = p.point(i).position().distanceH(center);
        	double turnRadius = p.getTcpData(ixBOT).turnRadius();
        	double deltaRadius = distanceFromCenter - turnRadius;
        	if (Math.abs(distanceFromCenter - turnRadius) > distH_Epsilon) {
    			if ( ! silent) {
    				f.pln(" >>> checkWithinTurn: "+p.getName()+" POINT OFF CIRCLE at i = "+i+" deltaRadius = "+Units.str("NM",deltaRadius));
    			}
    			rtn = false;
        	}
		}
		return rtn;
	}



	public static int insertVirtual(Plan ac, double time) {
		if (time >= ac.getFirstTime() && (time <= ac.getLastTime()) && ac.getIndex(time) < 0) {
			//NavPoint src = ac.point(ac.getSegment(time));
			TcpData tcp = ac.getTcpData(ac.getSegment(time));
			NavPoint np = new NavPoint(ac.position(time), time); //src.makeStandardRetainSource().makePosition(ac.position(time)).makeTime(time);
			tcp.reset();
			tcp.setVirtual();
			return ac.add(np, tcp);
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
			double t = ac.time(i) + dist/gs;  
			if (t < ac.time(i+1) && t > ac.time(i)) {
				//    	  error.addError("addLocalMaxLat time out of bounds");
				insertVirtual(ac,t);
				//f.pln("NEWTIME "+ac+" "+i+" "+t);      
				//ac.setPointLabel(i+1, "maxlat");
				ac.setNavPoint(i+1, ac.point(i+1));
				ac.setInfo(i+1,"maxlat");
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
		double lat = Util.max(Math.abs(ac.point(i).lat()),
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
					t2 = ac.time(i+1);
					if (midOnly) {
						insertVirtual(ac,(t2+t1)/2.0);
						i++;
					} else {
						i += Util.max(0, interpolateVirtualsAccelHalf(ac,haccuracy, vaccuracy, minDt, t1, t2, 0)-1); // add on the number of additional points, -1, if 2 or more added
					}
				}
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
			while (i < ac.size()-1 && ac.time(i) <= endTm) {
				double dist = ac.pathDistance(i);
				double legDist = getLegDist(ac,i,accuracy,mindist); // "optimal" distance between virtuals
				//Special case for kinematic plans in acceleration zones -- ensure there are at least 2 mid-points
				// Possibly get rid of this
				//				if (!ac.isLinear()) {
				//					Plan kpc = ac;
				//					if (kpc.point(i).isTurnBegin() || kpc.point(i).isTurnMid()) {
				//						legDist = Util.min(legDist, dist/3.0);
				//					}
				//				}
				if (dist > legDist) {
					double gs = ac.averageVelocity(i).gs();
					int j = i;
					double tmIncr = legDist/gs;
					//System.out.println("Points "+ac.point(i)+" "+ac.point(i+1));
					if (Math.abs(ac.point(i).lat()) >= Math.abs(ac.point(i+1).lat())) {
						double nextT = ac.time(i+1);
						double t = ac.time(i);
						while (t + tmIncr + Constants.TIME_LIMIT_EPSILON < nextT) {
							t += tmIncr;
							//System.out.println("1AC "+ac.getName()+" Time "+t+" "+nextT);
							insertVirtual(ac,t);
							legDist = getLegDist(ac,j,accuracy,mindist);
							j++;
							tmIncr = legDist/gs;
						}
					} else {
						double thisT = ac.time(i);
						double t = ac.time(i+1);
						while (t - tmIncr - Constants.TIME_LIMIT_EPSILON > thisT) {
							t -= tmIncr;
							//System.out.println("2AC "+ac.getName()+" Time "+t+" "+thisT);
							insertVirtual(ac,t);
							legDist = getLegDist(ac,i,accuracy,mindist);
							j++;
							tmIncr = legDist/gs;
						}
					}
					i = Util.max(i,j-1); // possibly take back last increment
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
		while (i < ac.size() && ac.time(i) < endTm) {
			if (ac.isVirtual(i) || all) {
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
	 * @param midOnly
	 * @return new plan with added virtuals (if latlon or kinematic)
	 */
	public static Plan interpolateVirtualsAll(Plan acin, double haccuracy, double vaccuracy, double minDt, double start, double end, boolean midOnly) {
		if (!acin.isLatLon() && acin.isLinear()) return acin; // skip copy
		Plan ac = acin.copy();
		interpolateVirtuals(ac,haccuracy, start,end);
		interpolateVirtualsAccel(ac,haccuracy,vaccuracy,minDt, start,end,false);
		return ac;
	}



	/**
	 * @deprecated
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
	public static Plan revertTCPs(Plan fp) {
		Plan lpc = new Plan(fp.getName(),fp.getNote());
		//f.pln(" $$ revertTCPs: START: fp = "+fp.toString()+" "+start+" "+upto);
		for (int i = 0; i < fp.size(); i++) {
			//NavPoint p = fp.point(i);
			TcpData tcp = fp.getTcpData(i);
			// case: point it virtual or temp: delete point
			// case: point is not original or first or last: keep point.
			// case: point is part of a turn: keep only the vertex point, delete others, if all three are present, otherwise keep remaining
			// case: point is part of GSC: keep only first point, delete second if both present, otherwise keep remaining
			// case: point is part of VSC: keep only vertext point, delete others if both present, otherwise keep remaining
			// don't deal with deleted points quite yet.  this gets really messy.
			if ((!fp.isVirtual(i)) ) {
				//f.pln("revertTCPs $$$$$$$$$$$$ "+fp.hasSource(i));
				if (fp.hasSource(i)){   // note for standard (linear) points, sourceTime and sourcePosition equal time() and position()
					//lpc.add(new NavPoint(tcp.sourcePosition(), tcp.sourceTime()).makeLabel(fp.label(i)), tcp);
					lpc.addNavPoint(new NavPoint(tcp.getSourcePosition(), tcp.getSourceTime()).makeLabel(fp.label(i)));
					//f.pln(" $$ revertTCPs: add "+" "+f.Fm1(tcp.sourceTime())+"   "+tcp.sourcePosition());
				}
			}
		}
		lpc.getMessage();
		return lpc;	
	}



	/** 
	 * Revert the TCP pair in a plan structurally.  Properly relocate all points between TCP pair.
	 *
	 * Note.  No check is made to insure that start or upto is not in the middle of a TCP.
	 * 
	 * @param pln plan
	 * @param ix  index
	 */
	public static void structRevertTCP(Plan pln, int ix) {
		if (ix < 0 || ix >= pln.size()) {
			pln.addError(" structRevertTCP: index out of range");
		}
		if (pln.isBGS(ix)) {
			boolean revertPreviousTurn = true;
			structRevertGsTCP(pln,ix,revertPreviousTurn);
		} else if (pln.isBVS(ix)) {
			pln.structRevertVsTCP(ix);						
		} if (pln.isBOT(ix)) {
			// store distance from BOT to all points between BOT and EOT	
			boolean addBackMidPoints = true;
			boolean killNextGsTCPs = false;
			double zVertex = -1;
	    	pln.structRevertTurnTCP(ix,addBackMidPoints,killNextGsTCPs);
		} else {
			pln.addError(" structRevertTCP: index must be a begin TCP!!!" );
		}
		// return new Plan();
	}

	/**
	 * Revert all TCPs in the plan.
	 * 
	 * @param pln
	 * @param removeRedundantPoints
	 */
	public static void structRevertTCPs(Plan pln, boolean removeRedundantPoints) {
		//f.pln(" ----------------------- structRevertVsTCP pass ------------------------");
		//double zVertex = -1;
		for (int i = pln.size()-2; i > 0; i--) {
			pln.structRevertVsTCP(i);
		}
		//f.pln(" ----------------------- structRevertTurnTCP pass ------------------------");
		for (int i = pln.size()-2; i > 0; i--) {
			boolean addBackMidPoints = true;
			boolean killNextGsTCPs = true;
			pln.structRevertTurnTCP(i,addBackMidPoints,killNextGsTCPs);
		}
		//f.pln(" ----------------------- structRevertGsTCP pass ------------------------");
		//for (int i = pln.size()-2; i > 0; i--) {
		for (int i = 0; i < pln.size(); i++) {
			//f.pln(" $$$$ structRevertTCPs: revert GSTCP at i = "+i);
			boolean revertPreviousTurn = true;
			structRevertGsTCP(pln,i,revertPreviousTurn);
		}
		//f.pln(" ----------------------- removeRedundantPoints pass ------------------------");
		if (removeRedundantPoints) pln.removeRedundantPoints(0,pln.size()-1);
	}
	

//	/** Structurally revert all TCPS that create acceleration zones containing ix
//	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain 
//	 *  original ground speed into the point after ix.  
//	 *  
//	 *  NOTE This method does not depend upon source time!!
//	 * 
//	 * @param ix  The index of one of the TCPs created together that should be reverted
//	 * @return index of the reverted point
//	 */
//	public static int structRevertGroupOfTCPsTimeWindow(Plan pln, int ix, double timeWindow) {
//		if (ix < 0 || ix >= pln.size()) {
//			pln.addError(".. structRevertGroupOfTCPs: invalid index "+ix, 0); 
//			return -1;
//		} 
//		NavPoint origPt = pln.point(ix);	
//		double lowTm = origPt.time()-timeWindow/2.0;
//		double highTm = origPt.time()+timeWindow/2.0;
//		int lowIx = pln.getNearestIndex(lowTm);
//		int highIx = pln.getNearestIndex(highTm);
//		f.pln(" $$$$$ structRevertGroupOfTCPsTimeWindow lowIx = "+lowIx+" highIx = "+highIx);
//		for (int i = lowIx; i <= highIx; i++) {
//			NavPoint np = pln.point(i);
//			if (np.time() > highTm) break;
//			double zVertex = -1;
//			if (np.isVsTCP()) {
//				if (np.isEVS()) i = pln.prevBVS(i);
//				f.pln(" $$$ structRevertVsTCP i = "+i);
//				zVertex = structRevertVsTCP(pln,i);
//			} else if (np.isTrkTCP()) {
//				if (np.isEOT()) i = pln.prevBOT(i);
//				boolean killNextGsTCPs = true;
//				boolean addBackMidPoints = false;
//				f.pln(" $$$ structRevertTurnTCP i = "+i);
//				structRevertTurnTCP(pln,i, addBackMidPoints, killNextGsTCPs,zVertex);
//			} else if (np.isGsTCP()) {
//				if (np.isEGS()) i = pln.prevBGS(i);
//				zVertex = -1;
//				f.pln(" $$$ structRevertGsTCP i = "+i);  
//				boolean revertPreviousTurn = true;
//				structRevertGsTCP(pln,i,revertPreviousTurn);  
//			}
//		}
//		return -1;
//	}


//	public static int structRevertGroupOfTCPsTimeWindow(Plan pln, int ix) {
//		return structRevertGroupOfTCPsTimeWindow(pln,ix,100.0);
//	}

	/** Structurally revert all TCPS that create acceleration zones containing ix
	 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after ix to regain 
	 *  original ground speed into the point after ix.  
	 *  
	 *  NOTE This method does not depend upon source time or source position
	 * 
	 * @param pln plan
	 * @param ix  The index of one of the TCPs created together that should be reverted
	 * @param killAllOthersInside
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
		//NavPoint origPt = pln.point(ix);	
		if (! pln.isTCP(ix)) return ix;  // nothing to do
		//f.pln(" $$$ structRevertGroupOfTCPs:origPt = point("+ix+") = "+pln.point(ix).toStringFull());
		int firstInGroup = ix;                  // index of first TCP in the group
		int lastInGroup = ix;   // pln.size()-1;                // index of the last TCP in the group
		if (firstInGroup == 0) {
			f.pln(" !! structRevertGroupOfTCPs: ERROR cannot remove first point 1");
			return -1;
		}
		if (pln.isVsTCP(ix)) {
			if (pln.isEVS(ix)) {
				int ixBVS = pln.prevBVS(ix);//fixed
				if (ixBVS < firstInGroup) firstInGroup = ixBVS;
				lastInGroup = ix;
			} else {
				int ixEVS = pln.nextEVS(ix);//fixed
				if (ixEVS > lastInGroup) lastInGroup = ixEVS;
				firstInGroup = ix;
			}
			//f.pln(" $$$$ structRevertGroupOfTCPs(VS): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
		} else {
			if (pln.isGsTCP(ix)) {
				if (pln.isEGS(ix)) {
					int ixBGS = pln.prevBGS(ix);//fixed
					if (ixBGS >= 0 && ixBGS < firstInGroup) firstInGroup = ixBGS;
					lastInGroup = ix;
				} else {
					int ixEGS = pln.nextEGS(ix);//fixed
					if (ixEGS >= 0 && ixEGS > lastInGroup) lastInGroup = ixEGS;
					firstInGroup = ix;
				}
				//f.pln(" $$$$ structRevertGroupOfTCPs(GS): firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);	
			} else { // track
				if (pln.isEOT(ix)) {
					int ixBOT = pln.prevBOT(ix);//fixed
					if (ixBOT >= 0 && ixBOT < firstInGroup) firstInGroup = ixBOT;
					lastInGroup = ix;
				} else {
					int ixEOT = pln.nextEOT(ix);//fixed
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
			if (pln.isVsTCP(ii)) {
				//f.pln(" $$$$ structRevertGroupOfTCPs(VS-2): ii = "+ii);
				zVertex = pln.structRevertVsTCP(ii);
				if (zVertex >= 0) {
					lastInGroup--;
					//if (ii == firstInGroup) firstInGroup++;
				}           	
			}
		}
		if (pln.isTrkTCP(firstInGroup)) {
			boolean killNextGsTCPs = true;
			boolean addBackMidPoints = false;
			//f.pln(" structRevertGroupOfTCPs: zVertex = "+zVertex);
			pln.structRevertTurnTCP(firstInGroup, addBackMidPoints, killNextGsTCPs);
		}
		if (pln.isGsTCP(firstInGroup)) {  // does not revert previous turn  
			//f.pln(" $$$$ structRevertGroupOfTCPs: GS section: firstInGroup = "+firstInGroup);
			boolean revertPreviousTurn = true;
			firstInGroup = structRevertGsTCP(pln,firstInGroup, revertPreviousTurn);  
		}
		//f.pln(" $$$$$$$$$$$$$ firstInGroup = "+firstInGroup);
		return firstInGroup; 
	}


	/** if "ix" is a BGS, then it reverts the BGS-EGS pair back to a single linear point
	 *  Note: It (does not depend upon source time or source position!)
	 *  
	 * @param pln                  plan
	 * @param ix                   index of BGS
	 * @param revertPreviousTurn   if true then if this GS segment is right after a turn then revert the turn as well  
	 * @return                     index of reverted point
	 */	
	public static int structRevertGsTCP(Plan pln, int ix, boolean revertPreviousTurn) {
		if (! pln.validIndex(ix)) return -1;
		if (pln.isBGS(ix)) {	
			//NavPoint ixP = pln.point(ix);
			int ixPrev = pln.prevTCP(ix);
			int prevLinIndex = -1;
			if (ixPrev >= 0) prevLinIndex = pln.getTcpData(ixPrev).getLinearIndex();
			//f.pln(" $$$ ixPrev = "+ixPrev+" prevLinIndex = "+prevLinIndex);
			//if (npPrev.isEOT() && dt < Plan.revertGsTurnConnectionTime && revertPreviousTurn) {
			if (revertPreviousTurn && ixPrev >= 0 && pln.isEOT(ixPrev) && pln.getTcpData(ix).getLinearIndex() == prevLinIndex) {; 
				boolean killNextGsTCPs = true;
				int ixPrevBOT = pln.prevBOT(ixPrev);//fixed
				boolean addBackMidPoints = false;
				pln.structRevertTurnTCP(ixPrevBOT, addBackMidPoints, killNextGsTCPs);
				//f.pln(" $$$$ structRevertGsTCP: ixPrevBOT = "+ixPrevBOT);
				return ixPrevBOT;
			} else {
				boolean storeAccel = true;
				pln.structRevertGsTCP(ix, storeAccel);
			}
		}
		return ix;
	}
	

	

//	/** Revert all BGS-EGS pairs in range "start" to "end"
//	 * 
//	 * @param pln      plan file
//	 * @param start    starting index
//	 * @param end      ending index
//	 * @return         reverted plan containing no ground speed TCPS in specified range
//	 */
//	public static void OLDrevertGsTCPs(Plan pln, int start, int end) {
//		//f.pln("  $$$ revertGsTCPs start = "+start+" end = "+end);
//		if (start < 0) start = 0;
//		if (end > pln.size()-1) end = pln.size()-1;
//		for (int j = end; j >= start; j--) {
//			//f.pln("  $$$ REVERT GS AT j = "+j+" np_ix = "+pln.point(j));
//			//boolean revertPreviousTurn = false;
//			structRevertGsTCP(pln,j); // ,revertPreviousTurn);
//		}               
//	}
	
	
	
	/**
	 * change ground speed to newGs, starting at startIx 
	 * @param p
	 * @param newGs
	 * @param startIx
	 * @return a new Plan
	 */
	public static Plan makeGsConstant(Plan p, double newGs, int startIx) {
		//f.pln(" makeGSConstant: newGs = "+Units.str("kn",newGs,4)+"  startIx = "+startIx);
		int ix;
		if (p.isBVS(startIx)) {
			ix = startIx;
		} else {
			ix = p.nextBVS(startIx); // fixed
		}
		double vsAccel = 1.0;
		if (ix >= 0) {
			vsAccel = p.vsAccel(ix);
		}
		p.revertVsTCPs(startIx,p.size()-1);
		boolean storeAccel = true;
		p.revertGsTCPs(startIx,storeAccel);
		Plan kpc = p.copy();
		if (Util.almost_equals(newGs,0.0)) {
			kpc.addError(" 	makeGSConstant_No_Verts: newGs cannot be zero1");
			return kpc;
		}
		for (int i = startIx; i < kpc.size(); i++) {
			//f.pln(" $$$$ makeGSConstant: i = "+i+" kpc.point(i) = "+kpc.point(i).toStringFull());
            kpc.mkGsOut(i,newGs);
		}
		Plan vpc = kpc;
		if (ix >= 0) {
			boolean continueGen = false;
			vpc = TrajGen.generateVsTCPs(kpc,vsAccel,continueGen);
		} 
		return vpc;
	}

	public static Plan makeGsConstant(Plan p, double newGs) {
        return makeGsConstant(p,newGs,0);
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
	 *    
	 * 
	 * @param p
	 * @param wp1
	 * @param wp2
	 * @param gs
	 * @return a new plan
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
			rtnPln.add(p.get(i)); 
		}
		//  -------------- process indices [wp1+1,wp2]  ----------------------------------
		double lastTime = p.time(wp1); // p.getTime(Util.min(3, p.size()-1));
		for (int i = wp1; i < wp2; i++) {
			double dt = p.pathDistance(i,i+1)/gs;
			//f.pln(" $$$ lastTime = "+lastTime);
			double newTime = lastTime+dt;
			NavPoint np = p.point(i+1).makeTime(newTime); 
			//f.pln(" $$$$$>>> linearMakeGSConstant CHANGE i+1 = "+(i+1)+" from tm = "+p.point(i+1).time()+" to newTime = "+newTime+" dt = "+dt);
			rtnPln.add(np, p.getTcpData(i+1));
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
			rtnPln.add(np,p.getTcpData(i)); 			
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
	 * 
	 * @param p plan
	 * @param gs ground speed
	 * @return a new plan
	 */
	public static Plan linearMakeGSConstant(Plan p, double gs) {
		return linearMakeGSConstant(p,0,p.size()-1,gs);
	}

	/**
	 * Make ground speed constant gs between wp1 and wp2.
	 * Assumes input plan is linear.
	 * 
	 * @param p a plan
	 * @return a new plan with constant ground speed
	 */
	public static Plan linearMakeGSConstant(Plan p) {
		//f.pln(" $$$$$$ PlanUtil.linearMakeGSConstant: p = "+p);
		double dtot = p.pathDistance(0,p.size()-1);
		double ttot = p.time(p.size()-1) - p.time(0);
		double gs = dtot/ttot;
		return linearMakeGSConstant(p, 0,p.size()-1,gs);
	}


	/**
	 * Make ground speed constant between wp1 and wp2 as an average of the total distance and time travelled.
	 * Assumes input plan is linear.
	 * 
	 * @param p   
	 * @param wp1
	 * @param wp2
	 * @return
	 */
	public static Plan linearMakeGSConstant(Plan p, int wp1, int wp2) {
		//f.pln("%%##  linearMakeGSConstant: wp1 = "+wp1+" wp2 = "+wp2);
		if (wp1 < 0) return p;
		if (wp2 >= p.size()) wp2 = p.size()-1;
		if (wp1 >= wp2) return p;
		double dtot = p.pathDistance(wp1,wp2);
		double ttot = p.time(wp2) - p.time(wp1);
		double gs = dtot/ttot;
		//f.pln(" linearMakeGSConstant: TARGET gs = "+Units.str("kn",gs)+"  from waypoint "+wp1+" to "+wp2);	
		return linearMakeGSConstant(p, wp1,wp2,gs);
	}


//	/** Convert a plan (kinematic or linear) starting at waypoint wp1 to a route.   The original turn radius is 
//	 *  retained so that the horizontal path distance will be the same if it is converted back to a kinematic plan.
//	 *  Note.  the gsAccel and vsAccel acceleration parameters are not stored.
//	 *  Note.  The sourcePosition fields are used for reversion.  SourceTimes are use to identify all
//	 *         points that were generated from the soursePosition, but its value is not used.
//	 * 
//	 * 
//	 * @param p     plan to be ripped
//	 * @param wp1   starting point of the rip
//	 * @return      route from the plan
//	 */
//	
//	public static Route toRoute(Plan p, int wp1) {
//        Route rt = new Route();
//        ArrayList<Double> sourceTimes = new  ArrayList<Double>();
//		for (int j = wp1; j < p.size(); j++) {
//			NavPoint np_j = p.point(j);
//			//f.pln(" $$$$ toRoute: np_j = "+np_j.toStringFull());
//			double sourceTime = np_j.sourceTime();
//			if (! sourceTimes.contains(sourceTime)) {
//			    Position pos = np_j.sourcePosition();
//			    rt.add(pos, np_j.label(), np_j.turnRadius());
//			    sourceTimes.add(sourceTime);
//			}
//		}		
//		return rt;
//	}
	
//	/** EXPERIMENTAL --> subtle
//	 *  Convert a plan (kinematic or linear) starting at waypoint wp1 to a route.   The original turn radius is 
//	 *  retained so that the horizontal path distance will be the same if it is converted back to a kinematic plan.
//	 *  Note.  the gsAccel and vsAccel acceleration parameters are not stored.
//	 *  Note.  The sourcePosition fields are used for reversion.  SourceTimes are use to identify all
//	 *         points that were generated from the soursePosition, but its value is not used.
//	 * 
//	 * @param p     plan to be ripped
//	 * @param wp1   starting point of the rip
//	 * @return      route from the plan
//	 */	
//	public static GsPlan toGsPlan(Plan p, int wp1) {
//		//f.pln(" $$ ------------------------- toGsPlan: p = "+p.toStringGs());
//        GsPlan gsp = new GsPlan(p.getFirstTime());
//        ArrayList<Double> sourceTimes = new  ArrayList<Double>();
//		for (int j = wp1; j < p.size(); j++) {
//			NavPoint np_j = p.point(j);
//			//f.pln(" $$$$ toGsPlan: np_j = "+np_j.toStringFull());
//			double sourceTime = np_j.sourceTime();
//			if (! sourceTimes.contains(sourceTime)) {
//			    Position pos = np_j.sourcePosition();
//			    double gsInit = p.initialVelocity(j).gs();
//			    if (np_j.isBOT()) {
//			    	//f.p(" **IS BOT** ");
//			    	int jNext = j-1;
//			    	double srcTime = sourceTime;
//			    	// find ground speed at the end of the srcTime group
//			    	while (srcTime == sourceTime && jNext < p.size()-1) {
//			    		jNext++;
//			    		//f.pln(" $$$$>>>>>>>>>>> toGsPlan: p.point("+jNext+")= "+p.point(jNext).toStringFull());
//			    		srcTime = p.point(jNext).sourceTime();
//			    	}
//			    	//f.p(" jNext = "+jNext);
//			    	gsInit = p.initialVelocity(jNext-1).gs();
//			    } else if (np_j.isBGS()) {
//			    	//f.p(" **IS BGS** ");
//			    	int jNext = p.nextEGS(j);
//			    	gsInit = p.initialVelocity(jNext).gs();
//			    } else if (np_j.isBVS()) {
//			    	//f.p(" **IS BVS** ");
//			    	int jNext = p.nextEVS(j);
//			    	double tm = p.getTime(jNext);
//			    	while (p.inGsChange(tm)) {  // if EVS is inside a BGS EGS get speed after EGS
//			    		jNext++;
//			    		 tm = p.getTime(jNext);
//			    		 if (jNext == p.size()-1) break;
//			    	}		    
//			    	gsInit = p.initialVelocity(jNext).gs();
//			    }
//			    gsp.add(pos, np_j.label(), gsInit, np_j.turnRadius());
//			    //f.pln(" $$###>>>>> toGsPlan.add: j = "+j+" "+pos+" "+np_j.label()+" gsin = "+Units.str("kn",gsInit)+" radius = "+Units.str("nm",np_j.turnRadius()));
//			    sourceTimes.add(sourceTime);
//			}
//		}		
//		return gsp;
//	}


	
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
			TcpData  tcp = p.getTcpData(i+1);
			//f.pln(" $$$$$>> makeVsConstant dt = "+dt+" vs = "+Units.str8("fpm",vs)+" ADD  "+np);
			//f.pln(" $$$$$>> makeVsConstant dt*vs = "+dt*vs+" REMOVE: "+p.point(i+1));
			//f.pln(" $$$$$>> makeVsConstant  p.point(i+1).alt() = "+Units.str("ft",p.point(i+1).alt())+" newAlt = "+Units.str("ft",newAlt));
			p.remove(i+1);
			p.add(np,tcp);
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
		double ttot = p.time(end) - p.time(start);
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
	 * Return a string consisting of both the navpoint label and the tcpdata information for a point.  
	 * If either is nonempty they will be separated by a slash (so "KEEHO/", "/$vertpt, and "KEEHO/$vertpt" are all possibilities).
	 * If both are empty, return the empty string.
	 * @param p plan
	 * @param i point index
	 * @return string of label/info or empty string
	 */
	public static String getLabelAndInfoString(Plan p, int i) {
		if (p.point(i).label().length() > 0 || p.getInfo(i).length() > 0) {
			return  p.point(i).label()+"/"+p.getInfo(i);
		}
		return "";
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
			first = Util.min(first,i);
			newLabel += "BOT:";
			i = oldLabel.indexOf(":ACC:"); // acceleration
			if (i >= 0) {
				j = oldLabel.indexOf(":",i+5);
				newLabel += "ATRK:"+oldLabel.substring(i+5,j+1);
			}
		}
		i = oldLabel.indexOf(":BGSC:"); // velocity in
		if (i >= 0) {
			first = Util.min(first,i);
			newLabel += "BGS:";
			i = oldLabel.indexOf(":ACC:"); // acceleration
			if (i >= 0) {
				j = oldLabel.indexOf(":",i+5);
				newLabel += "AGS:"+oldLabel.substring(i+5,j+1);
			}
		}
		i = oldLabel.indexOf(":BVSC:"); // velocity in
		if (i >= 0) {
			first = Util.min(first,i);
			newLabel += "BVS:";
			i = oldLabel.indexOf(":ACC:"); // acceleration
			if (i >= 0) {
				j = oldLabel.indexOf(":",i+5);
				newLabel += "AVS:"+oldLabel.substring(i+5,j+1);
			}
		}
		i = oldLabel.indexOf(":EOT:"); // velocity in
		if (i >= 0) {
			first = Util.min(first,i);
			newLabel += "EOT:";
		}
		i = oldLabel.indexOf(":EGSC:"); // velocity in
		if (i >= 0) {
			first = Util.min(first,i);
			newLabel += "EGS:";
		}
		i = oldLabel.indexOf(":EVSC:"); // velocity in
		if (i >= 0) {
			first = Util.min(first,i);
			newLabel += "EVS:";
		}
		i = oldLabel.indexOf(":TMID:"); // becomes ADDED
		if (i >= 0) {
			first = Util.min(first,i);
			newLabel += "ADDED:";
		} else { // add source, if present
			i = oldLabel.indexOf(":SRC:");
			if (i >= 0) {
				first = Util.min(first,i);
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
			first = Util.min(first,i);
			j = oldLabel.indexOf(":",i+5);
			newLabel += oldLabel.substring(i+1, j+1);
		}


		i = oldLabel.indexOf(":mV:"); // minor -- just copy for now
		if (i >= 0) {
			first = Util.min(first,i);
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
		//f.pln("\n"+oldLabel);
		//f.pln(newLabel);
		return newLabel;
	}

	public static Plan subPlan(Plan plan, int startIx, int endIx) {
		if (startIx < 0) startIx = 0;
		if (endIx > plan.size()) endIx = plan.size() - 1;
		Plan rtn = new Plan("");
		for (int i = startIx; i <= endIx; i++) {
			//f.pln(" $$$ subPlan: ADD plan.point(i) = "+plan.point(i).toStringFull());
			rtn.add(plan.get(i));
		}
		return rtn;
	}

	/**  make a new plan that is identical to plan from startTime to endTime 
	 *   It assumes that  startTime and endTime are in linear segments
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
		TcpData start_tcp = new TcpData();
		if (ix >= 0) {
			start = plan.point(ix);
			start_tcp = plan.getTcpData(ix);
		} else {
			Position startPos = plan.position(startTime);
			start = new NavPoint(startPos, startTime);
		}
		rtn.add(start, start_tcp);
		for (int i = 0; i < plan.size(); i++) {
			 Pair<NavPoint,TcpData> pi = plan.get(i);
			if (startTime < pi.first.time() && pi.first.time() < endTime ) {
				rtn.add(pi);
			}
		}
		ix = plan.getIndex(endTime);
		Pair<NavPoint,TcpData> end;
		if (ix >= 0) {
			end = plan.get(ix);
		} else {
			Position endPos = plan.position(endTime);
			NavPoint endPt = new NavPoint(endPos, endTime);
			end = new Pair<NavPoint,TcpData>(endPt,new TcpData());
		}
		rtn.add(end);
		return rtn;
	}

	/** 
	 * Cut down a plan so that it only contains points between timeOfCurrentPosition and intentThreshold.
	 * This method cuts a Plan so that the acceleration information after intentThreshold is discarded.  The plan
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
		Pair<NavPoint,TcpData> firstPair = plan.get(ixInit);
		firstPoint = firstPair.first;
		TcpData firstPoint_tcp = firstPair.second;
		if (plan.isEndTCP(ixInit)) {
			firstPoint_tcp.reset();
		}
		nPlan.add(firstPoint,firstPoint_tcp); 
		//f.pln(" $$$$0 cutDownTo: ADD INITIAL point("+ixInit+") = "+firstPoint.toString()+" "+firstPoint_tcp);

		int ix = ixInit + 1;
		//f.pln(" ix = "+ix+" sz = "+plan.size());
		while (ix < plan.size() && plan.point(ix).time() < intentThreshold) {
			Pair<NavPoint,TcpData> p = plan.get(ix);
			nPlan.add(p);
			//f.pln(" $$$$$$ cutDownTo ADD p = "+p);
			ix++;
		}
		if (ix >= plan.size()) { // plan ran out before intentThreshold
			double dt = intentThreshold - plan.getLastTime();
			Velocity vout = plan.initialVelocity(plan.size()-1);
			NavPoint lastP = plan.point(plan.size()-1);
			NavPoint np = lastP.linear(vout, dt).makeLabel("CutDownTo_0");
			nPlan.addNavPoint(np);
			//f.pln(" $$$$$$ cutDownTo ADD np = "+np);
		} else {                // intentThreshold is within plan
			NavPoint newLastPt = new NavPoint(plan.position(intentThreshold),intentThreshold);
			TcpData newLastTcpData = new TcpData().setInformation("CutDownTo_newLastPt");
			Velocity vout = plan.velocity(intentThreshold);
			if (! plan.inAccel(intentThreshold)) {
				//f.pln(" $$$$$$ cutDownTo ADD newLastPtp = "+newLastPt+" vout = "+vout);
				nPlan.add(newLastPt,newLastTcpData);
			} else {
				boolean inTurn = plan.inTrkChange(intentThreshold);
				boolean inGsAccel = plan.inGsChange(intentThreshold);
				boolean inVsAccel = plan.inVsChange(intentThreshold);

				String trkType = "NONE";
				String gsType = "NONE";
				String vsType = "NONE";			 
				if (inTurn) trkType = "EOT";
				if (inGsAccel) gsType = "EGS";
				if (inVsAccel) vsType = "EVS";             
				NavPoint lastP = new NavPoint(newLastPt.position(), newLastPt.time(), "CutDownTo_lastP");
				// TODO: THE FOLLOWING IS NOT RIGHT!
				TcpData lastTcp = TcpData.makeFull("Orig",  trkType,  gsType, vsType,
						0.0, Position.ZERO_LL, 0.0, 0.0, vout, newLastPt.position(), newLastPt.time(),-1).setInformation("CutDownTo_lastP");
				nPlan.add(lastP,lastTcp);
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
			NavPoint extendPt = lastPt.linear(vout, dt);
			TcpData extendTcpData = new TcpData().setInformation("CutDownTo_extendPt");
			nPlan.add(extendPt,extendTcpData);
		}
		//f.pln(" $$$$ cutDownTo: nPlan = "+nPlan);
		return nPlan;
	}

	public static Plan cutDownTo(Plan plan, double timeOfCurrentPosition, double intentThreshold) {
		return cutDownTo(plan, timeOfCurrentPosition, intentThreshold, intentThreshold);
	}


	/**
	 * Cut a Plan down to contain "numTCPs" future TCPS past the current time (i.e. timeOfCurrentPosition). If tExtend
	 * is greater than 0, create an extra leg after the last TCP with a duration of "tExtend".  This is intended
	 * to mimic having only state information after the last TCP.  Note that numTCPs should be interpreted as number of
	 * acceleration zones.  That is  [BOT,EOT] counts as one TCP.  Similarly [BGS, EGS] is one TCP.
	 * 
	 * This method eliminates waypoints earlier than timeOfCurrentPosition as much as possible.  If the aircraft
	 * is in an acceleration zone at timeOfCurrentPosition, then it retains the plan back to the last begin TCP.
	 * 
	 * @param numTCPs  maximum number of TCPs to allow in the future, see note above.
	 * @param timeOfCurrentPosition  indicates current location of aircraft, if negative, then aircraft is at point 0.
	 * @param tExtend  amount of additional time to extend the plan after last TCP end point.
	 * 
	 * NOTE: THIS CODE WILL NOT WORK WITH OVERLAPPING HORIZONTAL/VERTICAL Accel Zones
	 * 
	 * @return
	 */
	public static Plan cutDownToByCount(Plan plan, int numTCPs, double timeOfCurrentPosition, double tExtend) {
		int ix = plan.getSegment(timeOfCurrentPosition);
		int cnt = 0;
		if (plan.inAccel(timeOfCurrentPosition)) cnt = 1;
		while (cnt < 2*numTCPs && ix < plan.size()) {
			//f.pln(" $$$$ cutDownTo point ix = "+ix+" point(ix) = "+point(ix).toStringFull());
			//NavPoint p = plan.point(ix);
			if (plan.isTCP(ix)) cnt++;	
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
			rtn.add(plan.get(i));
		}
		return rtn;
	}

	/** Determines if there is enough distance for the current speed and specified acceleration (maxAccel)
	 * 
	 * @param p         plan 
	 * @param ix        index of ground speed change 
	 * @param maxAccel  maximum ground speed acceleration
	 * 
	 * Note: calculates delta time at ix+1 that is achievable if not enough distance
	 * @return  Return the needed correction at ix+1:  achievable delta time - current delta time (see fixGsAccelAt);
	 * 
	 *           M secs                   t                            M secs
	 *      * ------------* ---------------------------------- * -------------------*
	 *     ix            BGS              dist                EGS                 ix + 1
	 *     
	 *     
	 *     dist = v0*M + v0*t +0.5*t*t + (v0+at)*M
	 *     
	 *     solution to t is a quadratic with
	 *        a = a
	 *        b = 2*(v0 + a*M)
	 *        c = 2*v0*M - 2*dist
	 *     
	 *     
	 *     
	 */
	public static Pair<Boolean,Double> enoughDistanceForAccel(Plan p, int ix, double maxAccel, double M) {
		if (ix == 0 || ix == p.size()-1) return new Pair<Boolean,Double>(true,0.0);
		double gsIn = p.finalVelocity(ix-1).gs();
		double gsOut = p.initialVelocity(ix).gs();
		//f.pln(" $$ enoughDistanceForAccel: ix = "+ix+" p = "+p.toStringGs());
		//f.pln(" $$ enoughDistanceForAccel: gsIn = "+Units.str("kn",gsIn)+" gsOut = "+Units.str("kn",gsOut));
		double deltaGs = gsOut - gsIn;
		double a = Util.sign(deltaGs)*maxAccel;
		//f.pln(" $$ enoughDistanceForAccel: deltaGs = "+Units.str("kn",deltaGs)+" actual accel = "+deltaGs/dt);
		double dtNeeded = deltaGs/a + 2*M;  // plus 2*M because BGS starts M second into segment and EGS ends M sec before
		//double dtNow = p.getTime(ix+1) - p.getTime(ix);
		//f.pln(" $$ enoughDistanceForAccel: dtNeeded = "+dtNeeded+" dtNow = "+f.Fm1(dtNow));
		double distanceNeeded = gsIn*dtNeeded + 0.5*a*dtNeeded*dtNeeded;
		double distanceBetween = p.pathDistance(ix,ix+1);
		//f.pln("\n $$ enoughDistanceForAccel: distanceBetween = "+Units.str("ft",distanceBetween)+" distanceNeeded = "+Units.str("ft",distanceNeeded));		
		boolean rtn = distanceNeeded <= distanceBetween;
		if (rtn) { // no repair needed
			return new Pair<Boolean,Double>(true,0.0);
		} else {
			//double b = 2*gsIn;
			//double c = -2*distanceBetween;	        
			double b = 2*(gsIn + a*M);
			double c = 4*gsIn*M - 2.0*distanceBetween;	        	        
			double dtp = (-b+Math.sqrt(b*b - 4*a*c))/(2.0*a);
			//double dtn = (-b-Math.sqrt(b*b - 4*a*c))/(2.0*a);	
			//f.pln(" $$ enoughDistanceForAccel: dtp = "+dtp+" dtn = "+dtn);
			//f.pln(" $$ enoughDistanceForAccel: dt = "+dt+" tmRtn = "+tmRtn+" dtNeeded = "+dtNeeded);
			//f.pln(" $$ enoughDistanceForAccel: max delta GS = "+Units.str("kn",a*tmRtn));	   
			double dtCurrent = p.time(ix+1) - p.time(ix) - 2*M;
			double correction = (dtp-dtCurrent);  
			//f.pln(" $$ enoughDistanceForAccel: correction = "+correction);
			//f.pln(" $$ enoughDistanceForAccel: BEFORE deltaGs = "+Units.str("kn",deltaGs));
			return new Pair<Boolean,Double>(false, correction);
		}
	}

	/** Fix Plan p at ix if there is not enough distance for the current speed and specified acceleration (maxAccel)
	 *  It makes the new ground speed as close to the original as possible (that is achievable over the distance)
	 * 	
	 * @param p         plan 
	 * @param ix        index of ground speed change 
	 * @param maxAccel  maximum ground speed acceleration
	 * @param checkTCP  if true, do not alter time if point is an EOT or EVS
	 * @return -1 if no change was necessary, otherwise return the new time at ix+1
	 */
	public static void fixGsAccelAt(Plan p, int ix, double maxAccel, boolean checkTCP, double M) {
		//f.pln(" $$$$>>>>>>>>>>>>>>>>>>>>>.. fixGsAccelAt: ix = "+ix);
		if (ix < 0 || ix >= p.size() - 1) return;
		double dtNow = p.time(ix+1) - p.time(ix);
		if (2*M >= dtNow) {
			double gsIn = p.finalVelocity(ix-1).gs();
			double calcTimeGSin = p.calcTimeGSin(ix+1,gsIn);
			double correction = calcTimeGSin - p.time(ix+1);
			p.timeShiftPlan(ix+1,correction);
			//double gsOut = p.initialVelocity(ix).gs();
			//double deltaGs = gsOut - gsIn;
			//f.pln(" $$ fixGsAccelAt: AFTER deltaGs = "+Units.str("kn",deltaGs));
		} else {
			Pair<Boolean,Double> pEnoughDist = enoughDistanceForAccel(p, ix, maxAccel, M);
			//NavPoint np_ix = p.point(ix+1);   // we will be altering point ix+1
			if (checkTCP && (p.isEOT(ix+1) || p.isEVS(ix+1))) return;
			if ( ! pEnoughDist.first) {
				double correction = pEnoughDist.second;
				//f.pln(" $$$$>>>>>>>>>>>>>>>>>>>>>.. fixGsAccelAt: ix = "+ix+"  correction = "+correction);
				p.timeShiftPlan(ix+1,correction);	  // 2.2 about right
				//double gsIn = p.finalVelocity(ix-1).gs();
				//double gsOut = p.initialVelocity(ix).gs();
				//f.pln(" $$ enoughDistanceForAccel: ix = "+ix+" gsIn = "+Units.str("kn",gsIn)+" gsOut = "+Units.str("kn",gsOut));
				//double deltaGs = gsOut - gsIn;
				//f.pln(" $$ fixGsAccelAt: AFTER deltaGs = "+Units.str("kn",deltaGs));
			}
		}
	}


	public static void fixGsAccel(Plan p, int start, int end, double maxAccel, double M) {
		for (int j = start; j <= end; j++) {
			fixGsAccelAt(p,j,maxAccel,true,M);
		}
	}

	public static void fixGsAccelAfter(Plan p, int start, double maxAccel, double M) {
		for (int j = start; j < p.size(); j++) {
			fixGsAccelAt(p,j,maxAccel,true,M);
		}
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
		p.add(pp.point(0), pp.getTcpData(0));
		for (int j = 1; j < pp.size()-1; j++) {
			Velocity vel = pp.initialVelocity(j);
			boolean same = aboutTheSameTrk(lastVel,vel,sameTrackBound);
			//f.pln(j+" $$ removeCollinear: lastVel = "+lastVel+" vel = "+vel+" same = "+same);   		
			if (!same) {
				p.add(pp.get(j));
			}
			lastVel = vel;
		}
		p.add(pp.get(pp.size()-1));
		//f.pln(" $$ removeCollinearTrk: pp.size() = "+pp.size()+" p.size() = "+p.size());
		return p;
	}

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

	public static int containsInfo(Plan p, String label) {
		for (int i = 0; i < p.size(); i++) {
			if (p.getInfo(i).equals(label)) {
				return i;
			}		
		}
		return -1;
	}

	public static double crudeMinDistanceBetween(Plan p, Plan q) {
		double maxFirstTime = Util.max(p.getFirstTime(), q.getFirstTime());
		double minLastTime = Util.min(p.getLastTime(), q.getLastTime());
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
			double tprev = a.time(i-1);
			double t = a.time(i);
			if (tprev >= 0) {
				double dt = t - tprev;
				if (a.isLatLon()) {
					LatLonAlt x1 = a.point(i-1).lla();
					LatLonAlt x2 = a.point(i).lla();
					LatLonAlt y1 = b1.lla();
					LatLonAlt y2 = b2.lla();
					double ti = GreatCircle.intersectionAvgAlt(x1, x2, dt, y1, y2).second;
					if (ti >= 0 && ti < dt) return tprev + ti;
				} else {
					Vect3 x1 = a.point(i-1).point();
					Vect3 x2 = a.point(i).point();
					Vect3 y1 = b1.point();
					Vect3 y2 = b2.point();
					//f.pln("x1="+x1+" x2="+x2+" y1="+y1+" y2="+y2);
					double ti = VectFuns.intersectionAvgZ(x1, x2, dt, y1, y2).second;
					//f.pln(" intersectionTimeHorizontal: $$$$$$$$$$$ tprev = "+tprev+" ti = "+ti);
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

//	public static Plan timeShiftPlan(Plan p, int start, double dt) {
//		Plan nPlan = p.copy();
//		nPlan.timeshiftPlan(start,dt);
//		return nPlan;
//	}

	/**  timeShift points in plan by "dt" starting at index "start"
	 *   Note: This will drop any points that become negative or that become out of order using a negative dt
	 *   See also Plan.timeShiftPlan for equivalent method
	 * 
	 * @param p       plan
	 * @param start   starting index
	 * @param dt      delta time
	 * @return        time-shifted plan
	 */
	public static Plan timeShift(Plan p, int start, double dt) {
		Plan np = new Plan(p.getName());
		double lastTime = -1; // ensure overlapping points are not included; also fixes start=0 and dt=0 problem
		for (int i = 0; i < p.size(); i++) {
			if (i < start) {
				np.add(p.get(i));
				lastTime = p.point(i).time();
			} else {
				double t = p.time(i) + dt;
				NavPoint nav = p.point(i).makeTime(t);
				if (t >= 0 && t > lastTime) {
					np.add(nav,p.getTcpData(i));
					lastTime = t;
				}
			}
		}
		return np;
	}
	

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
			p.setNavPoint(j, np2);
		}
		return p;
	}


	private static boolean errorsContains(List<Triple<Double,Velocity,String>> errors, double t) {
		for (Triple<Double,Velocity,String> item : errors) {
			if (t == item.first) return true;
		}
		return false;
	}


	/**
	 * 
	 * @param p initial plan
	 * @param errors full list of error velocities at all intervals and plan time points,
	 *   for each point the Triple is: time, Velocity (2D), and "name" 
	 * 
	 * @return new plan
	 */
	public static Plan applyWindField(Plan p, List<Triple<Double,Velocity,String>> errors) {
		GsPlan gsp = new GsPlan(p.getFirstTime());
		// check errors list has all plan time points
		for (int i = 0; i < p.size(); i++) {
			if (!errorsContains(errors,p.time(i))) {
				f.pln("PlanUtil.applyWindError errorlist does not contain plan time t="+p.time(i));
				return null;
			}
		}
		for (int i = 0; i < errors.size(); i++) {
			double t = errors.get(i).first;
			Velocity verr = errors.get(i).second;
			String name = errors.get(i).third;
			String info = "";
			Position pos = p.position(t);
			Velocity v = p.velocity(t);
			double gserr = v.vect2().Hat().dot(verr.vect2()); //TODO: check with negative error
			double gsbase = v.gs();
			gsp.add(pos, name, info, gsbase+gserr);
		}
		Plan np = gsp.linearPlan();
		np.setName(p.getName());
		return np;
	}

	public static boolean checkMySolution(Plan solution, double currentTime, Position currentPos, Velocity currentVel) {
		boolean ok = true;
		if (!solution.position(currentTime).almostEquals(currentPos)) {
			System.out.println("\n---------------------------------------------------------------------------------");
			System.out.println(" ............... ERROR: moved location of current position! currentTime = "+currentTime+" .......");
			System.out.println(" ............... from "+currentPos+" to "+solution.position(currentTime));
			System.out.println("----------------------------------------------------------------------------------\n");
			//PlanCollection pc = new PlanCollection(plans);
			//pc.setPlan(0, saveMyPlan.copy());
			//StratUtil.dumpPlans(pc,"moved",this.getTimeOfCurrentPosition());				
			//DebugSupport.halt();
			ok = false;
		}
		//f.pln(" Stratway: Velocity Test AT END "+mySolution.velocity(currentTime)+" "+currentVel);
		if (!solution.velocity(currentTime).within_epsilon(currentVel,0.10)) {
			System.out.println("\n---------------------------------------------------------------------------------");
			System.out.println(" ............... ERROR: changed **velocity** of current position! currentTime = "+currentTime+" ....");
			System.out.println(" ............... from "+currentVel+ " to "+solution.velocity(currentTime));
			System.out.println("----------------------------------------------------------------------------------\n");
			//PlanCollection pc = new PlanCollection(plans);
			//pc.setPlan(0, saveMyPlan.copy());
			//StratUtil.dumpPlans(pc,"velChanged",this.getTimeOfCurrentPosition());				
			//DebugSupport.halt();
			ok = false;
		}
		if (!solution.isWellFormed()) {
			System.out.println("\n---------------------------------------------------------------------------------");
			System.out.println(" ............... ERROR: solution is not wellFormed currentTime = "+currentTime+" ....");
			System.out.println(" ..............."+solution.strWellFormed());
			System.out.println(" mySolution = "+solution.toString());
			System.out.println("----------------------------------------------------------------------------------\n");
			ok = false;
		}
		return ok;
	}

	/**  Used in bDistDiff to calculate a distance difference between two plans
	 * 
	 * @param A    plan A
	 * @param B    plan B
	 * @return
	 */
	public static double distanceBetween(Plan A, Plan B) {
		double rtn = 0;
		rtn = Math.abs(A.getFirstTime() - B.getFirstTime());
		rtn = rtn + Math.abs(A.getLastTime() - B.getLastTime());
		double maxStart = Util.max(A.getFirstTime(), B.getFirstTime());
		double minEnd   = Util.min(A.getLastTime(), B.getLastTime());
		double step = (minEnd - maxStart)/20;
		for (double t = maxStart; t <= minEnd; t = t + step) {
			double errH = A.position(t).distanceH(B.position(t));
			double errV = A.position(t).distanceV(B.position(t));
			//f.pln(" $$$$$ distanceBetween t = "+t+" errH = "+f.Fm2(errH)+" errV = "+f.Fm2(errV));
			rtn = rtn + errH + errV;
		}    	
		return rtn;
	}

    /** Advance forward in plan "p"  starting at time "curTm" a distance of "advDistance" within a single segment
     * 
     * Note : assumes the advance by distance will not leave current segment
     * Note : this can be used before there is a ground speed profile -- it does not depend upon correct velocities
     * 
     * @param p            plan of interest
     * @param curTm        currentTime  of so
     * @param advDistance  distance to advance
     * @param linear       if true, treat plan as a linear plan (i.e. path is not curved)
     * @return             Position "advDistance" ahead of "curTm"
     */
    private static Position advanceDistanceInSeg(Plan p, double curTm, double advDistance, boolean linear) {
       	//f.pln(" $$ %%%% advanceDistanceInSeg: ENTER: curTm = "+f.Fm4(curTm)+"  advDistance = "+Units.str("NM",advDistance));        
    	if (curTm < p.getFirstTime() || curTm > p.getLastTime()) {
    		f.pln(" $$@@ advanceDistanceInSeg:  ERROR: currentTime = "+curTm+" outside of plan!");
    	}
    	int seg = p.getSegment(curTm);
    	//f.pln(" $$$ advanceDistanceInSeg: currentTime = "+currentTime+" seg = "+seg);
       	double distLeftInSeg0 = p.partialPathDistance(curTm,linear);
       	if (Util.almost_equals(advDistance, distLeftInSeg0)) {
       		return p.point(seg+1).position();
       	}
        if (advDistance > distLeftInSeg0) {
        	//f.pln(" $$$$ currentTime = "+currentTime+" distFromSo = "+Units.str("NM",advDistance,8)+" distLeftInSeg0 = "+Units.str("NM",distLeftInSeg0,12));
        	//f.pln(" $$$$ ERROR: advanceDistanceInSeg assumes the advance by distance will not leave current segment!");
        	//Debug.halt();
        	return Position.INVALID;
        }
    	Pair<Position, Velocity> positionVelocity = p.positionVelocity(curTm,linear);
    	Position so = positionVelocity.first;
    	Velocity vo = positionVelocity.second;
    	//f.pln(" $$ %%%% advanceDistanceInSeg: currentTime = "+f.Fm4(currentTime)+" vo = "+vo);
    	Position sNew;  	
    	if (p.inTrkChange(curTm) & !linear) {
    		int ixPrevBOT = p.prevBOT(seg+1);//fixed
    		Position center = p.turnCenter(ixPrevBOT);
    		double signedRadius = p.signedRadius(ixPrevBOT);
    		int dir = Util.sign(signedRadius);		
    		double gsAt_d = -1.0;             // THIS IS ONLY USED IN THE VELOCITY CALCULATION WHICH WE ARE NOT USING
    		Pair<Position,Velocity> tAtd = KinematicsPosition.turnByDist2D(so, center, dir, advDistance, gsAt_d);
    		sNew = tAtd.first;
     		//f.pln(" $$ %%%% advanceDistanceInSeg A: sNew("+f.Fm2(currentTime)+") = "+sNew);
    	} else {
    		//f.pln("\n\n $$ %%%% advanceDistanceInSeg B1: currentTime = "+currentTime+" seg = "+seg+"  distFromSo = "+Units.str("NM",distFromSo));
    		double track = vo.trk();  // TODO:  look into getting track another way    		
    		sNew = so.linearDist2D(track, advDistance);  // does not compute altitude !!
    		//f.pln(" $$ %%%% advanceDistanceInSeg B2: seg = "+seg+" sNew("+f.Fm2(currentTime)+") = "+sNew);
    	}
        double segDistance = p.pathDistance(seg);
		double deltaAlt = p.point(seg+1).alt() - p.point(seg).alt();
		double sZ = so.alt() + advDistance/segDistance*deltaAlt;
		//f.pln(" $$ %%%% advanceDistanceInSeg B1: curTm = "+curTm+" sZ = "+Units.str("ft",sZ)+" deltaAlt = "+Units.str("fpm",deltaAlt));
		sNew = sNew.mkAlt(sZ);
    	return sNew;
    }
    
    
    /** Beginning at location determined by current time find new position that is "advanceDist" ahead in plan
     * 
     * @param p            plan of interest
     * @param currentTime  currentTime  of so
     * @param advanceDist  distance to advance
     * @param linear       if true, then use the linear distance between points instead of the curved distance
     * @return             a pair containing the position and the segment where the position is located
     */
    public static Pair<Position,Integer> advanceDistance(Plan p, double currentTime, double advanceDist, boolean linear) {
    	Position sNew;
    	int initSeg = p.getSegment(currentTime);
    	//f.pln(" $$::::::::::: advanceDistance AA: initSeg = "+initSeg+" p.size = "+p.size()+"  advanceDist = "+Units.str("NM",advanceDist));
    	double distLeftInSeg0 = p.partialPathDistance(currentTime,linear);
        //f.pln("distLeftInSeg0="+distLeftInSeg0);    	
    	int finalSeg = initSeg;
    	//f.pln(" $$::::::::::: advanceDistance BB: initSeg = "+initSeg+"  distLeftInSeg0 = "+Units.str("NM",distLeftInSeg0));
    	if (advanceDist < distLeftInSeg0) {  //  new position remains in segment "seg"
    		//f.pln(" $$::::::::::: advanceDistance 00000000: initSeg = "+initSeg+"  advanceDist = "+Units.str("NM",advanceDist)+" distLeftInSeg0 = "+Units.str("NM",distLeftInSeg0));
    		sNew = advanceDistanceInSeg(p, currentTime, advanceDist, linear);
    		//f.pln(" $$ advanceDistance A: sNew = "+sNew);
    	} else { 
    		double distSofar = distLeftInSeg0;
    		for (finalSeg = initSeg+1; finalSeg < p.size(); finalSeg++) {
    			double nextDistSoFar = distSofar + p.pathDistance(finalSeg,finalSeg+1,linear);
                //f.pln("advanceDist="+advanceDist+" nextDistSoFar="+nextDistSoFar+" totalDist="+p.pathDistance());    			
    			if (nextDistSoFar > advanceDist) {
    				break;
    			}
    			distSofar = nextDistSoFar;
    		}
    		double remainingDist = advanceDist - distSofar;
    		//f.pln(" $$ advanceDistance B1: remainingDist = "+Units.str("NM",remainingDist));
    		if (finalSeg >= p.size()-1) {
    			return new Pair<Position,Integer>(p.getLastPoint().position(),p.size()-1);
    		} else {
    			double t0 = p.time(finalSeg);
    			//f.pln(" $$ advanceDistance B2: j = "+j+" t0 = "+t0+" remainingDist = "+Units.str("NM",remainingDist));
    			sNew = advanceDistanceInSeg(p, t0, remainingDist, linear);  
    			//f.pln(j+" $$ advanceDistance B3: sNew = "+sNew);
    		}
    	}
    	return new Pair<Position,Integer>(sNew,finalSeg);
    }
    
 	

	/** time required to cover distance "dist" if initial speed is "gsInit" and acceleration is "gsAccel"
	 * 
	 * @param gsAccel   ground speed acceleration
	 * @param gsInit    initial ground speed
	 * @param dist      distance travelled
	 * @return
	 */
	static double timeFromGs(double gsInit, double gsAccel, double dist) {
		//double vo = initialVelocity(seg).gs();
		//double a = point(prevBGS(seg)).gsAccel();
		double t1 = Util.root(0.5*gsAccel, gsInit, -dist, 1);
		double t2 = Util.root(0.5*gsAccel, gsInit, -dist, -1);
		double dt = Double.isNaN(t1) || t1 < 0 ? t2 : (Double.isNaN(t2) || t2 < 0 ? t1 : Util.min(t1, t2));
		return dt;
	}
	



    /** insert a BGS - EGS region starting at time "t" in plan "p".  The BGS - EGS region accomplishes
     *  a change in ground speed equal to "deltaGS' via a constant acceleration of "gsAccel".  Speed is
     *  constant after that.
     *  
     *  Note: Vertical acceleration zones are rebuilt using the vsAccel of the first vertical TCP
     * 
     * @param p         Plan of interest
     * @param t         time point where BGS is to be inserted
     * @param deltaGs   desired change in ground speed
     * @param gsAccel   ground speed acceleration used 
     */
	public static Plan add_BGS_EGS_pair(Plan p, double t, double deltaGs, double gsAccel) {
		//f.pln(" $$$$ add_BGS_EGS_pair: ENTER p = "+p.toStringGs());
		if (t < p.getFirstTime() || t > p.getLastTime()) {
			p.addError("velocity: time "+f.Fm2(t)+" is not in plan!", p.size()-1);
			return p;
		} else {
			if (p.inGsChange(t)) {
				System.out.println(" $$$ addBGS_EGS_pair: Attempt to overlap BGS-EGS regions at time t = "+t);
				p.addError(" addBGS_EGS_pair: ERROR Attempt to overlap BGS-EGS regions at time t = "+t);
				return p;
			} else {
				Velocity vBGS = p.velocity(t);
				double a = Util.sign(deltaGs)*Math.abs(gsAccel);
				double dt = Math.abs(deltaGs/a);
				//f.pln(" $$$ addBGS_EGS_pair: dt = "+f.Fm4(dt)+"          plan length = "+(p.getLastTime() - p.getFirstTime()));
				double origGsIn = vBGS.gs();
				double distToEGS = origGsIn*dt + 0.5*a*dt*dt;
				//f.pln(" $$$ add_BGS_EGS_pair: distToEGS = "+Units.from("NM",distToEGS));
				Position bgs = p.position(t);
				double tBGS = t;			
				NavPoint tempBGS = new NavPoint(bgs,tBGS);
				int ix_t = p.getIndex(t);
				 Pair<NavPoint,TcpData> npBGS;
				if (ix_t >= 0 && p.isEGS(ix_t)) {					
					npBGS = Plan.makeEGSBGS(tempBGS, bgs,tBGS,a,-1); 
					p.remove(ix_t);
					//f.pln(" $$$ add_BGS_EGS_pair: remove ix_t = "+ix_t);
				} else { 
				    npBGS = Plan.makeBGS(tempBGS,bgs,tBGS,a,-1); 
				}
				//f.pln(" $$$ add_BGS_EGS_pair: npBGS = "+npBGS.toStringFull());				
				int seg = p.getSegment(t);	
				//PlanUtil.revertGsTCPs(p, seg, p.size()-1);
				// retrieve a value for vsAccel
				int ixBVS = p.nextBVS(seg); //fixed
				double vsAccel = 0.0;
				if (ixBVS >= 0) {		
					vsAccel = p.vsAccel(ixBVS);
				    p.revertVsTCPs(seg, p.size()-1);
				}
				int ixBGS = p.add(npBGS);
				//f.pln(" $$ add_BGS_EGS_pair: ixBGS = "+ixBGS);			
				boolean linear = false;            	
				Pair<Position,Integer> pad = advanceDistance(p, t, distToEGS, linear);
            	Position egsPos = pad.first;
            	int egsSeg = pad.second;
            	//f.pln(" %%%% egsSeg = "+egsSeg+"  egsPos = "+egsPos+" lastTIme = "+p.getLastTime());
             	double tEGS;
            	if (egsSeg == p.size()-1) {
            		p.addError(" ERROR: add_BGS_EGS_pair EGS point would occur after end of plan ");
            		tEGS = p.time(egsSeg);
            	} else {
            		tEGS = (p.time(egsSeg)+p.time(egsSeg+1))/2.0;  // temporarily use midpoint in time  
            	}
				NavPoint tempEGS = new NavPoint(egsPos,tEGS);
				Pair<NavPoint,TcpData> npEGS = Plan.makeEGS(tempEGS,egsPos,tEGS,-1);
				int ixEGS = p.add(npEGS);
            	//f.pln(" -------------------- AFTER ADD EGS -------------------------------------");
            	addSpeedProfile(p, ixBGS, origGsIn, ixEGS, deltaGs, a); 
				Plan newPlan = p;
				boolean continueGen = false;
            	if (ixBVS >= 0) newPlan = TrajGen.generateVsTCPs(p, vsAccel,continueGen);
				return newPlan;
			}
		}
	}
	
	/** Accelerate from "ixBGS" to "ixEGS" to achieve deltaGs speed change.  The ground speed into
	 *  "ixBGS" is "origGsOut".  This method critically depends upon the points at "ixBGS" and "ixEGS"
	 *  be TCPs and that the pathDistance between them is 
	 *  
	 *    a = Util.sign(deltaGs)*Math.abs(gsAccel);
	 *	  dt = Math.abs(deltaGs/a);
	 *    origGsIn = vBGS.gs();
	 *	  distToEGS = origGsIn*dt + 0.5*a*dt*dt;
     *
	 * 
	 * @param p           Plan of interest
	 * @param ixBGS       index of BGS NavPoint in plan p
	 * @param origGsIn    ground speed at BGS
	 * @param ixEGS       index of EGS NavPoint in plan p
	 * @param deltaGs     target change in ground speed
	 * @param a           acceleration
	 */
	private static void addSpeedProfile(Plan p, int ixBGS, double origGsIn, int ixEGS, double deltaGs, double a) {
		double gs = origGsIn;
		p.timeShiftPlan(ixBGS+1,1000);
		//DebugSupport.dumpPlan(p,"add_BGS_EGS_pair_0000000");
		//f.pln("\n\n --------------------------------------- $$ add_BGS_EGS_pair: p = "+p);
		double totalDistance = 0.0;
		for (int i = ixBGS; i < p.size()-1; i++) {
			//f.pln(" $$$$ add_BGS_EGS_pair: i = "+i+" p.size() = "+p.size());
			double dist = p.pathDistance(i);
			totalDistance = totalDistance + dist;
			double dt = timeFromGs(gs, a, dist);
			//f.pln(i+" $$$$ gs = "+Units.str("kn",gs)+"      .... dist/dt = "+Units.str("kn",dist/dt)+" dt = "+f.Fm2(dt)+" a = "+a+" dist = "+Units.str("NM",dist));
			NavPoint np_ip1 = p.point(i+1);
			//if (i == ixEGS) a = 0.0;
			if (i >= ixEGS) {
				gs = origGsIn + deltaGs;
				double newTime = p.calcTimeGSin(i+1,gs);
				//f.pln(" $$>>> make GS at i = "+i+" = "+Units.str("kn",gs)+" newTime = "+newTime);					
				p.setNavPoint(i+1,p.point(i+1).makeTime(newTime));
				//f.pln(" $$>>>    AFTER      postVel ("+(i)+") = "+Units.str("kn",p.gsOut(i)));					
			} else  {
				gs = gs + a*dt;
				double newTime = p.point(i).time()+dt;               		
				//f.pln(i+" >>>>> SET "+(i+1)+" newTime = "+newTime+"  np_ip1 = "+np_ip1+" gs = "+Units.str("kn",gs));
				//Velocity vin = p.velocityInit(i+1);
				Velocity vin = p.initialVelocity(i+1);
				if (p.isBeginTCP(i+1)) {
					vin = vin.mkGs(gs);
				}						
				NavPoint npNew = np_ip1.makeTime(newTime);				
		        TcpData tcpNew = p.getTcpData(i+1); //.setVelocityInit(vin);
				p.set(i+1,npNew,tcpNew);
				//f.pln(" $$>>>                                 postVel ("+(i)+") = "+Units.str("kn",p.gsOut(i)));					
			}
		}
	}
	
  
	public static String doubleAsUnitTest(String dName, double dVal, String units) {
		String rtn = " double "+dName+" = Units.from(\""+units+"\","+Units.to(units,dVal)+");";
        return rtn;
	}

	
	/**  Create a line for a unit test, the int j is used to name the point
	 * 
	 * @param p  
	 * @param j
	 * @return
	 */
	public static String positionAsUnitTest(Position p, int j) {
		String rtn = "  Position p"+j+"  = "+posAsUnitTest(p,j)+";";
		return rtn;
	}
	
	public static String posAsUnitTest(Position p, int j) {
		String rtn = "";
		if (p.isLatLon()) {
			rtn = "new Position(LatLonAlt.make("
					+ f.Fm6(Units.to("deg",p.lat()))
					+", "+f.Fm6(Units.to("deg",p.lon()))
					+", "+f.Fm6(Units.to("ft",p.alt()))+"))";
		} else {
			rtn = "new Position.makeXYZ("
					+(f.Fm6(Units.to("NM",p.x()))+", "
							+f.Fm6(Units.to("NM",p.y()))+", "
							+f.Fm6(Units.to("ft",p.z()))+"))");
		}
		return rtn;
	}


	public static String velocityAsUnitTest(Velocity v, int j) {
		String rtn = "";
		rtn = "  Velocity v"+j+"  = Velocity.mkTrkGsVs("  +f.Fm6(v.trk())+", "+f.Fm6(v.gs())+", "+f.Fm6(v.vs())+"); ";
		return rtn;
	}



	public static void dumpSimplePolyAsUnitTest(SimplePoly p, int i) {					
		System.out.println(" SimplePoly sPoly"+i+" = new SimplePoly("+p.getBottom()+","+p.getTop()+");   ");
		for (int j = 0; j < p.size(); j++) {
			Position np = p.getVertex(j);
			int ix = 100*i+j;
			f.p(positionAsUnitTest(np,ix));
			System.out.println("     sPoly"+i+".addVertex(p"+ix+");");
		}
	}

	public static void dumpPolyPathAsUnitTest(PolyPath pp0, String str) {
		System.out.println(" PolyPath pp = new PolyPath(\""+str+"\");");
		for (int j = 0; j < pp0.size(); j++) {
			//DebugSupport.dumpSimplePoly(pp0.getPoly(j),"polypath_"+j);
			dumpSimplePolyAsUnitTest(pp0.getPoly(j),j);
		}
		for (int j = 0; j < pp0.size(); j++) {
			Velocity vj = pp0.initialVelocity(j);
			System.out.println(velocityAsUnitTest(vj,j));
			System.out.println(" pp.addPolygon(sPoly"+j+",v"+j+","+pp0.getTime(j)+"); ");
		}
		System.out.println(" pp.setPathMode(PolyPath.PathMode."+pp0.getPathMode()+");");
	}


	
	public static String pointAsUnitTest(NavPoint p, int j) {
		return positionAsUnitTest(p.position(),j);
	}

	public static void dumpAsUnitTest(Plan p) {
		boolean asserts = false;
		dumpAsUnitTest(p, asserts);
	}
	
	public static void dumpAsUnitTest(Plan plan, boolean asserts) {
		System.out.println("Plan lpc = new Plan(\""+plan.getName()+"\");");
		for (int j = 0; j < plan.size(); j++) {
			System.out.println(pointAsUnitTest(plan.point(j),j));			
		}
		for (int j = 0; j < plan.size(); j++) {
			NavPoint np = plan.point(j);
			//Pair<NavPoint, TcpData> npPair = plan.get(j);
			//f.pln(" $$$$ dumpAsUnitTest: npPair = "+npPair.second.toString(true));
			TcpData tcp = plan.getTcpData(j);
			if (tcp.isTCP()) {
				Position sourcePosition = tcp.getSourcePosition();
				if (sourcePosition.isInvalid()) sourcePosition = Position.ZERO_LL;
				System.out.println("  NavPoint src"+j+" =  NavPoint.makeLatLonAlt("+sourcePosition.toStringNP(8)
				+", "+f.Fm6(tcp.getSourceTime())+").makeLabel(\""+np.label()+"\");");
				int linearIndex = tcp.getLinearIndex();
				if (tcp.isBOT()) {
					f.pln("  Position turnCenter"+j+" = "+posAsUnitTest(plan.turnCenter(j),j)+";");
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeBOT(src"+j+", p"+j+","+f.Fm6(np.time())+", "
				    		+f.Fm6(plan.signedRadius(j))+", turnCenter"+j+", "+linearIndex+");");		    
				} else if (tcp.isEOT()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeEOT(src"+j+", p"+j+","+f.Fm6(np.time())+", "+linearIndex+");");	
				} else if (tcp.isEGSBGS()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeEGSBGS(src"+j+", p"+j+","+f.Fm6(np.time())+", "
				    		+f.Fm6(plan.gsAccel(j))+", "+linearIndex+");");		    
				} else if (tcp.isBGS()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeBGS(src"+j+", p"+j+","+f.Fm6(np.time())+", "
				    		+f.Fm6(plan.gsAccel(j))+", "+linearIndex+");");		    
				} else if (tcp.isEGS()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeEGS(src"+j+", p"+j+","+f.Fm6(np.time())+", "+linearIndex+");");	
				} else if (tcp.isEVSBVS()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeEVSBVS(src"+j+", p"+j+","+f.Fm6(np.time())+", "
				    		+f.Fm6(plan.vsAccel(j))+", "+linearIndex+");");		    
					//f.p("  NavPoint np"+j+" = src"+j+".makeBVS(p"+j+", "+f.Fm6(np.time())+", "+f.Fm6(plan.vsAccel(j))+", vin"+j+","+linearIndex+");");				
				} else if (tcp.isBVS()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeBVS(src"+j+", p"+j+","+f.Fm6(np.time())+", "
				    		+f.Fm6(plan.vsAccel(j))+", "+linearIndex+");");		    
					//f.p("  NavPoint np"+j+" = src"+j+".makeBVS(p"+j+", "+f.Fm6(np.time())+", "+f.Fm6(plan.vsAccel(j))+", vin"+j+","+linearIndex+");");				
				} else if (tcp.isEVS()) {
				    f.pln("  Pair<NavPoint,TcpData> np"+j+" = Plan.makeEVS(src"+j+", p"+j+","+f.Fm6(np.time())+", "+linearIndex+");");	
				} else {
					f.p("  NavPoint np"+j+" = NavPoint.make???()");
				}
				System.out.println("    \t lpc.add(np"+j+");");
			} else {
				String label = np.label();
				if (label.equals(""))
					f.p("  NavPoint np"+j+" = new NavPoint(p"+j+","+f.Fm6(np.time())+");");
				else
					f.p("  NavPoint np"+j+" = new NavPoint(p"+j+","+f.Fm6(np.time())+").makeLabel(\""+label+"\");");
				System.out.println("    \t lpc.addNavPoint(np"+j+");");
			}			
		}
		if (asserts) {
			System.out.println(" assertEquals("+plan.size()+",plan.size());");
			for (int j = 0; j < plan.size(); j++) {
				NavPoint np = plan.point(j);
				System.out.println(" assertEquals(plan.point("+j+").time(), "+f.Fm4(np.time())+", 0.001);");
				System.out.println(" assertEquals(plan.point("+j+").lat(), "+f.Fm4(np.lat())+", 0.001);");
				System.out.println(" assertEquals(plan.point("+j+").lon(), "+f.Fm4(np.lon())+", 0.001);");
				System.out.println(" assertEquals(plan.point("+j+").alt(), "+f.Fm4(np.alt())+", 0.001);");
				if (plan.isBOT(j))	System.out.println(" assertTrue(plan.point("+j+").isBOT());");
				if (plan.isEOT(j))	System.out.println(" assertTrue(plan.point("+j+").isEOT());");
				if (plan.isBGS(j))	System.out.println(" assertTrue(plan.point("+j+").isBGS());");
				if (plan.isEGS(j))	System.out.println(" assertTrue(plan.point("+j+").isEGS());");
				if (plan.isBVS(j))	System.out.println(" assertTrue(plan.point("+j+").isBVS());");
				if (plan.isEVS(j))	System.out.println(" assertTrue(plan.point("+j+").isEVS());");
			}
		}
	}


	/**
	 * Return the time offset so the plan will synchronize at the given position and time.
	 * @param p point to synchronize.  This will match against the plan's closest point.
	 * @param pos position to synchronize
	 * @param t time to synchronize pos against
	 * @return time delta to synchronize the plan with the point
	 */
	public static double synchronize(Plan p, Position pos, double t) {
		NavPoint np = p.closestPoint(pos);
		return np.time()-t;
	}
	
	
    public static double centerAccuracy(Position bot, Position eot, Position center) {
    	double dist1 = bot.distanceH(center);
    	double dist2 = eot.distanceH(center);
    	double rtn = Math.abs(dist1-dist2);
    	//f.pln(" $$$$ centerAccuracy: rtn = "+rtn);
    	return rtn;
    }
	
	public static void turnPrint(Plan p, int i) {
		if ( ! p.isBOT(i)) return;
		NavPoint BOT = p.point(i);
		int ixEOT = p.nextEOT(i);//fixed
		NavPoint EOT = p.point(ixEOT);	
		double pathDist = p.pathDistance(i,ixEOT);
		f.pln(" $$$ turnPrint: pathDist = "+Units.str("NM",pathDist,12));	 
		double signedRadius = p.signedRadius(i);
		//f.pln(" $$$ turnConsistent: signedRadius = "+Units.str("ft",signedRadius,4)+" center = "+center.toStringNP(12));
		Position center = p.turnCenter(i);		
		//PlanUtil.Position center = NavPoint.centerExperimental;  // TODO --- THIS IS EXPERIMENTAL
		double theta = PositionUtil.angle_between(BOT.position(), center, EOT.position());
		//f.pln(" $$ pathDistance: R = "+Units.str("NM",R,12)+ "  theta="+Units.str("deg",theta,12)+" bot="+bot+" center="+center);
        pathDist = theta * p.turnRadius(i);
		//f.pln(" $$$ turnPrint: pathDist (V2) = "+Units.str("NM",pathDist,12));	 
		double radius1 = center.distanceH(BOT.position());
		double radius2 = center.distanceH(EOT.position());
		f.pln(" $$$ radius1 = "+Units.str("NM",radius1,12));
		f.pln(" $$$ radius2 = "+Units.str("NM",radius2,12)+" delta = "+Units.str("NM",radius2-radius1,12));
		double gsAt_d = 100;
		int dir = Util.sign(signedRadius);			
		Pair<Position,Velocity> tAtd = KinematicsPosition.turnByDist2D(BOT.position(), center, dir, pathDist, gsAt_d);
		Position EOTcalc = (tAtd.first).mkAlt(EOT.alt());// TODO: should we test altitude?
		f.pln(" >>> turnPrint i = "+f.padLeft(i+"", 2)+" calculated pos = "+EOTcalc.toString(12)
		+ "\n                            plan EOT = "+EOT.position().toString(12));
		double distanceH = EOT.position().distanceH(EOTcalc);
		double distanceV = EOT.position().distanceV(EOTcalc);
		f.pln("            .... distanceH = "+Units.str("m",distanceH,12));
		f.pln("            .... distanceV = "+Units.str("m",distanceV,12));
	}

	
	
}




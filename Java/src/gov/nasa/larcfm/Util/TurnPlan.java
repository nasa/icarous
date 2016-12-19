/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;

import gov.nasa.larcfm.IO.DebugSupport;

/** TurnPlan  -- an alternate yet convenient way to store a kinematic plan (turns only).  There are no times
 *               stored. Instead the ground speed at each point is specified.
 * 
 *  Since there are no times in a TurnPlan, only the order of the points is maintained.
 *  This often eliminates the need to continually recalculate times.  After an appropriate
 *  TurnPlan is constructed it can be converted to a kinematic plan using the "kinematicPlan" method
 *  
  */

public class TurnPlan { //extends Route {
	public static enum PointType     {LINEAR, BOT, EOT};

	public GsPlan gsp;
	private ArrayList<PointType> ptTypes;
	private ArrayList<Velocity>  vins;                // only needed for BOTs ???
	double gsAccel = 2;
	double vsAccel =  1;
		
	public TurnPlan(double startTime) {
		gsp = new GsPlan(startTime);
		ptTypes = new ArrayList<PointType>();
		vins = new ArrayList<Velocity>();
	}
	
	/** converts a linear plan into a TurnPlan using the points from "start" to "end"
	 * 
	 * @param lpc   linear plan
	 */
	public TurnPlan(Plan kpcOrig,  int start, int end) {
		//f.pln(" $$$$ start = "+start+" end = "+end);
		Plan kpc = kpcOrig.copy();
		// Use labels to find updated indices
		String saveStartLabel = kpc.point(start).label();
		String saveEndLabel = kpc.point(end).label();
		kpc.set(start,kpc.point(start).makeLabel("$startTurnPlan"));
		kpc.set(end,kpc.point(end).makeLabel("$endTurnPlan"));
		int ixBGS = kpc.prevBGS(kpc.size()-1);
		if (ixBGS >= 0) gsAccel = kpc.point(ixBGS).gsAccel();
		int ixBVS = kpc.prevBVS(kpc.size()-1);
		if (ixBVS >= 0) vsAccel = kpc.point(ixBVS).vsAccel();		
		kpc.revertGsTCPs();
		//f.pln(" $$$$ TurnPlan: AFTER revertGsTCPs: kpc = "+kpc.toStringGs());		
		kpc.revertVsTCPs();
		int startIx = kpc.findLabel("$startTurnPlan");
		int endIx = kpc.findLabel("$endTurnPlan");
		kpc.set(startIx,kpc.point(startIx).makeLabel(saveStartLabel));
		kpc.set(endIx,kpc.point(endIx).makeLabel(saveEndLabel));
		gsp = new GsPlan(kpc,startIx,endIx);
		ptTypes = new ArrayList<PointType>();
		vins = new ArrayList<Velocity>();
		//f.pln(" $$$$ startIx = "+startIx+" endIx = "+endIx);
		for (int i = startIx; i <= endIx; i++) {
			NavPoint np = kpc.point(i);
			//f.pln(" $$$$ np = "+np);
			if (np.isBOT()) {
				ptTypes.add(PointType.BOT);
				gsp.setRadius(i, np.signedRadius());
			} else if (np.isEOT()) {
				ptTypes.add(PointType.EOT);
			} else {
				ptTypes.add(PointType.LINEAR);
			}
			vins.add(kpc.initialVelocity(i));    // not needed for linear case
		}
		gsp.id = kpcOrig.getName();
	}	
	
	
	public TurnPlan(Plan kpcOrig) {
        this(kpcOrig,0,kpcOrig.size()-1);
	}
	
	public TurnPlan(TurnPlan tp) {
		gsp = new GsPlan(tp.gsp);
		ptTypes = new ArrayList<PointType>(tp.ptTypes);
		vins = new ArrayList<Velocity>(tp.vins);
	    gsp.id = tp.gsp.id;

	}
	
	public static TurnPlan makeTurnPlanCut(TurnPlan tp, int start, int end) {
		if (start < 0) start = 0;
		if (end >= tp.size()) end = tp.size()-1;
	    TurnPlan rtn = new TurnPlan(tp);
	    for (int j = rtn.size()-1; j > end ; j--) {
	    	rtn.remove(j);
	    } 
	    for (int j = start-1; j >= 0; j--) {
	    	rtn.remove(j);
	    }  
	    //
	    if (rtn.getPtType(rtn.size()-1) == PointType.BOT) {
	    	rtn.setPtType(rtn.size()-1,PointType.LINEAR);
	    }
	    return rtn;
	}

	
	/**
	 * Convert a linear GsPlan to a turnplan with BOT/EOT
	 * @param g base gs plan
	 * @param linear if false, add BOT/EOT points, if false, keep current point set
	 */
	public TurnPlan convert(GsPlan g, double bankAngle) {
		Plan lpc = g.linearPlan();
		Plan kpc = TrajGen.generateTurnTCPs(lpc, bankAngle);
		return new TurnPlan(kpc);
	}
	
//	public static TurnPlan makeTurnPlanConstant(TurnPlan gsp, double gsNew) {
//		TurnPlan gspNew = new TurnPlan(gsp);
//		for (int j = 0; j < gsp.size(); j++) {
//			gspNew.setGs(j,gsNew);
//		}
//        return gspNew;
//	}

	public int size() {
		return gsp.size();
	}
	
	public double gs(int i) {
		return gsp.gs(i);
	}
	
	/** Provide a copy of the GsPlan in this TurnPlan
	 * 
	 * @return
	 */
	public GsPlan getGsPlan() {
		return new GsPlan(gsp);
	}
	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals TurnPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param gsin grounds speed
	 */
	public void add(Position pos, String label, double gsin, double rad, PointType ptType, Velocity vin) {
		//f.pln(" $$###>>>>> TurnPlan.add: "+pos+" "+label+" gsin = "+Units.str("kn",gsin)+" radius = "+Units.str("nm",rad));
		gsp.add(pos, label, gsin, rad);
		//positions.add(pos);
		//names.add(label);
		//radius.add(rad);
		ptTypes.add(ptType);
		vins.add(vin);
	}
	
	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals TurnPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param gsin grounds speed
	 */
	public void add(Position pos, String label, Velocity vin) {
		add(pos,label,vin.gs(),0.0,PointType.LINEAR, vin);	}

	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals TurnPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param gsin grounds speed
	 */
	public void add(Position pos, String label, double gsin) {
		//f.pln(" $$###>>>>> TurnPlan.add: "+pos+" "+label+" gsin = "+Units.str("kn",gsin)+" radius = "+Units.str("nm",rad));
		add(pos,label,gsin,0.0,PointType.LINEAR, Velocity.INVALID);
	}

	/** add point "ix" from GsPlan "p"
	 * 
	 * @param p
	 * @param ix
	 */
	public void add(TurnPlan p, int ix) {
		//f.pln(" $$$ TurnPlan.add: ptTypes = "+ptTypes+" ix = "+ix);
		gsp.add(p.gsp,ix);
		ptTypes.add(p.getPtType(ix));
		vins.add(p.vin(ix));
		//f.pln(" $$ GsPlan add "+p.names.get(ix));
	}


	public Position position(int i) {
		return gsp.position(i);
	}
	
	public Position last() {
		return gsp.last();
	}
	
	public String name(int i) {
		return gsp.name(i);
	}
	
	public double radius(int i) {
		return gsp.radius(i);
	}
	
	public PointType ptType(int i) {
		return ptTypes.get(i);
	}

	public Velocity vin(int i) {
		return vins.get(i);
	}
	public Route getRoute() {
		return gsp.route();
	}
	
	public PointType getPtType(int i) {
		//f.pln(" $$$$$$$$$$ TurnPlan.getPtType i = "+i+" ptTypes = "+ptTypes.size());
		return ptTypes.get(i);
	}

	public void setPtType(int i, PointType ptt) {
		ptTypes.set(i,ptt);
	}


	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param nm      String to match
	 */
	public int findName(String nm) {
		return getRoute().findName(nm);
	}
	
	public String getId() {
		return gsp.id;
	}

	
//	public void add(TurnPlan p, int ix) {
//		gsp.add(p.position(ix),p.name(ix),p.radius(ix));
//		//positions.add(p.positions.get(ix));
//		//names.add(p.names.get(ix));
//		//radius.add(p.radius.get(ix));
//		gsAts.add(p.gsAts.get(ix));
//		//f.pln(" $$ TurnPlan add "+p.names.get(ix));
//	}
//
//	public void addAll(TurnPlan p) {
//		gsp.addAll(p.gsp);
//		//positions.addAll(p.positions);
//		//names.addAll(p.names);
//		//radius.addAll(p.radius);
//		gsAts.addAll(p.gsAts);
//	}
	
//	/** Create a new TurnPlan that is a copy of this one, then add all the elements from p2 to this new TurnPlan 
//	 * 
//	 * @param p2
//	 * @return
//	 */
//	public TurnPlan append(TurnPlan p2) {
//		TurnPlan rtn = new TurnPlan(this);
//		rtn.addAll(p2);
//		//f.pln(" $$ TurnPlan:  append: rtn.size() = "+rtn.size());
//		return rtn;
//	}
//	
	
	public void remove(int i) {
		gsp.remove(i);
		//positions.remove(i);
		//names.remove(i);
		//radius.remove(i);
		ptTypes.remove(i);
		vins.remove(i);
	}
//
//
//	public void setGs(int i, double gsin) {
//	     gsAts.set(i,gsin);
//	}
	
	
	public void addAll(TurnPlan p) {
		gsp.addAll(p.gsp);
		ptTypes.addAll(p.ptTypes);
		vins.addAll(p.vins);
	}

	
	/** Create a new TurnPlan that is a copy of this one, then add all the elements from p2 to this new GsPlan 
	 * 
	 * @param p2
	 * @return
	 */
	public TurnPlan append(TurnPlan p2) {
		TurnPlan rtn = new TurnPlan(this);
		rtn.addAll(p2);
		//f.pln(" $$ GsPlan:  append: rtn.size() = "+rtn.size());
		return rtn;
	}

	
	public double startTime() {
		return gsp.startTime();
	}

	public void setStartTime(double startTime) {
		gsp.setStartTime(startTime);
	}
	
//	public double getDefaultGroundSpeed() {
//		return defaultGroundSpeed;
//	}
//
//	public void setDefaultGroundSpeed(double defaultGroundSpeed) {
//		this.defaultGroundSpeed = defaultGroundSpeed;
//	}

	public int prevBOT(int current) {
		if (current < 0) {
			return -1;
		}
		for (int j = current; j >=0; j--) {
			if (ptTypes.get(j) == PointType.BOT) {
				return j;
			}
		}
		return -1;
	}
	
	public int prevEOT(int current) {
		if (current < 0) {
			return -1;
		}
		for (int j = current; j >=0; j--) {
			if (ptTypes.get(j) == PointType.EOT) {
				return j;
			}
		}
		return -1;
	}

	
	public boolean inTrkChange(int ix) {
		int ixBOT = prevBOT(ix);
		int ixEOT = prevEOT(ix);
		if (ixBOT >= 0 && ixEOT < ixBOT) return true;
		return false;
	}

	/** Provide a copy of the route in this GsPlan
	 * 
	 * @return
	 */
	public Route route() {
		return new Route(gsp.route());
	}

	
	/**
	 * This method returns a center of turn position with the same altitude as the current point.  If the current point is not a turn point, and has a zero stored radius,
	 * this returns an invalid position. 
	 * 
	 * @param ix  index of a preceding BOT type
	 */
	public Position turnCenter(int ix) {
		double R = route().radius(ix);
		if (R != 0) {
			Velocity vHat = vin(ix).mkAddTrk(Util.sign(R)*Math.PI/2).Hat2D();
			Position p = position(ix);
			return p.linear(vHat, Math.abs(R)).mkZ(p.z());			
		}
		return Position.INVALID;
	}

	
	public double pathDistance(int i, boolean linear) {
		if (i < 0 || i + 1 >= size()) {
			return 0.0;
		}
		Position p1 = position(i);
		if ( ! linear && inTrkChange(i)) { 
			// if in a turn, figure the arc distance
			Position p2 = gsp.position(i+1);
			int ixBOT = prevBOT(i);
			Position bot = gsp.position(ixBOT); 
			Position center = turnCenter(ixBOT);
			double R = radius(ixBOT);
			double theta = PositionUtil.angle_between(p1,center,p2);
			//double theta = GreatCircle.side_side_angle(GreatCircle.angular_distance(p1.position().lla(),center.lla()),GreatCircle.angular_distance(p2.position().lla(),center.lla()),Math.PI/2,false).second;
			//double theta = GreatCircle.angle_temp(p1.position().lla(),center.lla(),p2.position().lla());
			//f.pln(" $$ Plan 2036:  R = "+Units.str("ft",R)+ "  theta="+Units.to("deg",theta)+"   bot="+bot+"  same="+p1.equals(bot));
			return Math.abs(theta*R);	    	// TODO is this right for spherical coordinates???
		} else {
			// otherwise just use linear distance
			return position(i).distanceH(position(i+1));
		}
	}

	/**
	 * Find the cumulative horizontal (curved) path distance for whole plan.
	 */
	public double pathDistance() {
		return pathDistance(0, size(), false);
	}

	
	/** 
	 * Find the cumulative horizontal (curved) path distance between points i and j [meters].
	 */
	public double pathDistance(int i, int j) {
		return pathDistance(i, j, false);
	}

	/** 
	 * Find the cumulative horizontal path distance between points i and j [meters].   
	 * 
	 * @param i beginning index
	 * @param j ending index
	 * @param linear if true, then TCP turns are ignored. Otherwise, the length of the circular turns are calculated.
	 * @return cumulative path distance (horizontal only)
	 */
	public double pathDistance(int i, int j, boolean linear) {
		//f.pln(" $$ pathDistance: i = "+i+" j = "+j+" size = "+size());
		if (i < 0) {
			i = 0;
		}
		if (j >= size()) { // >= is correct, pathDistance(jj, linear) measures from jj to jj+1
			j = size()-1; 
		}
		double total = 0.0; 
		for (int jj = i; jj < j; jj++) {
			total = total + pathDistance(jj, linear);
			//f.pln(" $$ pathDistance: i = "+i+" jj = "+jj+" dist = "+Units.str("NM",pathDistance(jj, linear)));
		}
		//f.pln(" $$ pathDistance: total = "+Units.str("ft",total));
		return total;
	}

	
	
	public Plan kinematicPlan(boolean generateGsVsTCPS) {
		Plan kpc = new Plan("");
		if (gsp.size() < 1) return kpc;
		double lastT = gsp.startTime();
		Position lastNp = position(0);
		kpc.add(new NavPoint(lastNp, gsp.startTime()).makeLabel(name(0)));
		//f.pln(" $$$$$ TurnPlan.kinematicPlan: this = "+this.toString());
		for (int i = 1; i < gsp.size(); i++) {
			Position np = position(i);
			//double pathDist = np.distanceH(lastNp);
			boolean linear = false;
			double pathDist = pathDistance(i-1,linear);
			double gs_i = gs(i-1);
			double t = lastT + pathDist/gs_i;
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs_i)+" t = "+t);
			NavPoint nvp = new NavPoint(np,t).makeRadius(radius(i)).makeLabel(name(i));
			if (ptType(i) == PointType.LINEAR) {
				if (name(i).equals(gsp.virtualName)) nvp = nvp.makeVirtual();
				kpc.add(nvp);
			} else if (ptType(i) == PointType.BOT){
				//f.pln(" TurnPlan.kinematicPlan: i = "+" vin = "+vin(i)+" radius = "+radius(i));
				nvp = nvp.makeBOT(np,t,vin(i),radius(i),i);
				kpc.add(nvp);
			} else if (ptType(i) == PointType.EOT){
				nvp = nvp.makeEOT(np,t,vin(i),i);
				kpc.add(nvp);
			}
			//f.pln(" $$$$$ TurnPlan.linearPlan: nvp = "+nvp.toStringFull());
			lastT = t;
			lastNp = np;
		}
		if (generateGsVsTCPS) {
			//DebugSupport.dumpPlan(kpc,"TurnPlan.kinematicPlan");
			boolean repairGs = true;
//			Plan kpc2 = TrajGen.generateGsTCPs(kpc, gsAccel, TrajGen.GsMode.PRESERVE_GS, repairGs);
//			if (kpc2.hasError()) {
//				f.pln(" $$$$$ TurnPlan.kinematicPlan: generateGsTCPs FAILED: kpc2.msg = "+kpc2.getMessageNoClear());
//				return kpc2;
//			} else {
//				Plan kpc3 = TrajGen.generateVsTCPs(kpc2, vsAccel);
//				//DebugSupport.dumpPlan(kpc5, "generateTCPs_VsTCPs");
//				//f.pln(" $$>> makeKinematicPlan: kpc5 = "+kpc5.toStringTrk());	
//				TrajGen.cleanPlan(kpc3);
//				if (kpc3.hasError()) {
//					f.pln(" $$$$$ TurnPlan.kinematicPlan: generateVsTCPs FAILED: kpc3.msg = "+kpc3.getMessageNoClear());
//					f.pln(" $$$$$ TurnPlan.kinematicPlan: generateVsTCPs FAILED: kpc2 = "+kpc2); 
//				}
//				return kpc3;
//			}
			double vsAccel = 1;		
			double bankAngle = Units.from("deg", 25);
	        boolean repair = false;
	        Plan kpc2 = TrajGen.makeKinematicPlan(kpc, bankAngle, gsAccel, vsAccel, repair);      
			return kpc2;
		} else {
		    return kpc;
		}
	}

//	// EXPERIMENTAL
//	public void setGsChange(int bgsIndex, int egsIndex, double accel) {
//		if (bgsIndex >= positions.size()-1) return;
//		if (egsIndex < 0) return;
//		if (egsIndex <= bgsIndex) return;
//		
//		double lastT = startTime;
//		Position lastNp = positions.get(0);
//		
//		for (int i = 1; i < positions.size(); i++) {
//			Position np = positions.get(i);
//			double pathDist;
//			if ( radius.get(i) != 0.0 ) { // in turn  // TODO assumes radius is something from BOT until every point excluding the EOT
//				pathDist = 0.0; // TODO can't get this I need a center
//			} else {
//				pathDist = np.distanceH(lastNp);
//			}
//			double gs_i = gsAts.get(i-1);
//			double dt = pathDist/gs_i;
//			double t = lastT + dt;
//			
//			if ( bgsIndex < i && i <= egsIndex) {
//				gsAts.set(i, gs_i+accel*dt);
//				accelAt.set(i-1, accel);
//			} else {
//				accelAt.set(i-1, 0.0);
//			}
//			
//			lastT = t;
//			lastNp = np;
//		}
//	}

	/** test equality of TurnPlans
	 */
	public boolean equals(TurnPlan fp) {
		if (gsp.startTime() != fp.gsp.startTime()) return false;
		for (int i = 0; i < fp.size(); i++) {                // Unchanged
			if (position(i) != fp.position(i)) return false;
			if (! name(i).equals(fp.name(i))) return false;
			if (gs(i) != fp.gs(i)) return false;
			if (ptTypes.get(i) != fp.ptTypes.get(i)) return false;
		}
		return true;
	}
	
	public boolean almostEquals(TurnPlan p) {
		boolean rtn = true;
		for (int i = 0; i < size(); i++) {                // Unchanged
			if (!position(i).almostEquals(p.position(i))) {
				rtn = false;
				f.pln("almostEquals: point i = "+i+" does not match: "+position(i)+"  !=   "+p.position(i));
			}
			if (! name(i).equals(p.name(i))) {
				f.pln("almostEquals: name i = "+i+" does not match: "+name(i)+"  !=   "+p.name(i));
				rtn = false;
			}
		}
		return rtn;
	}

	
	public String toString() {
		String rtn = "TurnPlan size = "+gsp.size()+"\n";
		for (int i = 0; i < gsp.size(); i++) {
			rtn += " "+i+" "+position(i)+" "+f.padRight(name(i),4);
			rtn += " vin = "+vin(i);
			rtn += "  type = "+f.padRight(""+ptTypes.get(i),6);
			if (radius(i) != 0.0) rtn += " radius = "+Units.str("NM",radius(i));
			rtn += "\n";
		}
		return rtn;
	}

	
}

/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;

/** GsPlan  -- an alternate yet convenient way to store a linear plan.  There are no times
 *             stored. Instead the ground speed at each point is specified.
 * 
 *  Since there are no times in a GsPlan, only the order of the points is maintained.
 *  This often eliminates the need to continually recalculate times.  After an appropriate
 *  GsPlan is constructed it can be converted to a linear plan using the "linearPlan" method
 *  
  */

public class GsPlan extends Route {

	//final double defGroundSpeed = Units.from("kn",400);
	
	ArrayList<Double>  gsAts;                  // initial ground speeds
	double startTime;
	//double defaultGroundSpeed;
		
	public GsPlan(double startTime) {
		positions = new ArrayList<Position>();
		names = new ArrayList<String>();
		gsAts  = new ArrayList<Double>();
		this.startTime = startTime;
		//defaultGroundSpeed = defGroundSpeed;
	}
	
	/** converts a linear plan into a GsPlan
	 * 
	 * @param lpc   linear plan
	 */
	public GsPlan(Plan lpc, int start, int end) {
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		positions = new ArrayList<Position>();
		names = new ArrayList<String>();
		gsAts  = new ArrayList<Double>();
		for (int i = start; i <= end ; i++) {
			NavPoint np = lpc.point(i);
			Velocity vel = lpc.initialVelocity(i);
			add(np.position(),np.label(),vel.gs());
		}
		startTime = lpc.getFirstTime();
		//defaultGroundSpeed = defGroundSpeed;
	}
		
	public GsPlan(Plan lpc) {
		this(lpc,0,lpc.size()-1);
	}
	
	public GsPlan(GsPlan gsp) {
		positions = new ArrayList<Position>(gsp.positions);
		names = new ArrayList<String>(gsp.names);
		gsAts  = new ArrayList<Double>(gsp.gsAts);
		startTime = gsp.startTime;
		//defaultGroundSpeed = gsp.defaultGroundSpeed;
	}

	public int size() {
		return positions.size();
	}
	
	public double gs(int i) {
		return gsAts.get(i);
	}
	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals GsPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param gsin grounds speed
	 */
	public void add(Position pos, String label, double gsin) {
		positions.add(pos);
		names.add(label);
		gsAts.add(gsin);
	}
	
	/** This method is primarily added to prevent accidental use of lower level Route method
	 * 
	 *  It makes the ground speed the same as the last one, if there is a previous point, otherwise -1.0
	 * 
	 */
	public void add(Position pos, String label) {
		positions.add(pos);
		names.add(label);
		if (gsAts.size() > 0) {
		   gsAts.add(gsAts.get(gsAts.size()-1));
		} else {
		   gsAts.add(-1.0);
		}
	}
	
	
//	/** This method is primarily added to prevent accidental use of lower level Route method
//	 * 
//	 *  It makes the ground speed equal to defaultGroundSpeed
//	 * 
//	 */
//	void add(Position pos, String label) {
//		positions.add(pos);
//		names.add(label);
//		gsAts.add(defaultGroundSpeed);
//	}


	
	public void add(GsPlan p, int ix) {
		positions.add(p.positions.get(ix));
		names.add(p.names.get(ix));
		gsAts.add(p.gsAts.get(ix));
		//f.pln(" $$ GsPlan add "+p.names.get(ix));
	}

	public void addAll(GsPlan p) {
		positions.addAll(p.positions);
		names.addAll(p.names);
		gsAts.addAll(p.gsAts);
	}
	
	public GsPlan append(GsPlan p2) {
		GsPlan rtn = new GsPlan(this);
		rtn.addAll(p2);
		//f.pln(" $$ GsPlan:  append: rtn.size() = "+rtn.size());
		return rtn;
	}
	
	public void remove(int i) {
		positions.remove(i);
		names.remove(i);
		gsAts.remove(i);
	}


	public void setGs(int i, double gsin) {
	     gsAts.set(i,gsin);
	}
	
	public double startTime() {
		return startTime;
	}

	public void setStartTime(double startTime) {
		this.startTime = startTime;
	}
	
//	public double getDefaultGroundSpeed() {
//		return defaultGroundSpeed;
//	}
//
//	public void setDefaultGroundSpeed(double defaultGroundSpeed) {
//		this.defaultGroundSpeed = defaultGroundSpeed;
//	}

	
	public Plan linearPlan() {
		Plan lpc = new Plan("");
		if (positions.size() < 1) return lpc;
		double lastT = startTime;
		Position lastNp = positions.get(0);
		lpc.add(new NavPoint(lastNp,startTime).makeLabel(names.get(0)));
		for (int i = 1; i < positions.size(); i++) {
			Position np = positions.get(i);
			double pathDist = np.distanceH(lastNp);
			double gs_i = gsAts.get(i-1);
			double t = lastT + pathDist/gs_i;
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs_i)+" t = "+t);
			NavPoint nvp = new NavPoint(np,t).makeLabel(names.get(i));
			if (names.get(i).equals(virtualName)) nvp = nvp.makeVirtual();
			//f.pln(" $$$$$ GsPlan.linearPlan: nvp = "+nvp.toStringFull());
			lpc.add(nvp);
			lastT = t;
			lastNp = np;
		}
		return lpc;
	}


	/** test equality of GsPlans
	 */
	public boolean equals(GsPlan fp) {
		if (startTime != fp.startTime) return false;
		for (int i = 0; i < fp.size(); i++) {                // Unchanged
			if (position(i) != fp.position(i)) return false;
			if (! name(i).equals(fp.name(i))) return false;
			if (gs(i) != fp.gs(i)) return false;
		}
		return true;
	}
	
	public String toString() {
		String rtn = "GsPlan size = "+positions.size()+"\n";
		for (int i = 0; i < positions.size(); i++) {
			rtn += " "+i+" "+positions.get(i)+" "+names.get(i);
			rtn += " gsIn = "+Units.str("kn",gsAts.get(i));
			rtn += "\n";
		}
		return rtn;
	}

	
}

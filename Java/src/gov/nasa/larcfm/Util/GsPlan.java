/*
 * Copyright (c) 2016-2017 United States Government as represented by
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

public class GsPlan { //extends Route {

	private Route rt;
	private ArrayList<Double>  gsOuts;                   // ground speeds out
	public String id;                                    // name of the GsPlan
	private double startTime;                            // start time 
	public static final String virtualName = "$virtual";
		
	public GsPlan(double startTime) {
		rt = new Route();
		id = "";
		gsOuts  = new ArrayList<Double>();
		this.startTime = startTime;
	}

	public GsPlan(String s) {
		rt = new Route();
		id = s;
		gsOuts  = new ArrayList<Double>();
		this.startTime = 0.0;
	}


	/** Converts a section of a linear plan into a GsPlan.
	 *  The ground speeds correspond to the speeds in the "lcp" linear plan.
	 *  However, no route radii values are set using this constructor
	 * 
	 * @param lpc     linear plan
	 * @param start   starting index of the section to be used 
	 * @param end     ending index of the section to be used 
	 * 
	 */
	public GsPlan(Plan lpc, int start, int end) {
		//f.pln(" $$$$ GsPlan Constructor: start = "+start+" end = "+end+" lpc = "+lpc.toStringGs());
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		rt = new Route();
		id = lpc.getName();
		gsOuts  = new ArrayList<Double>();
		for (int i = start; i <= end ; i++) {
			NavPoint np = lpc.point(i);
			double gsOut_i = lpc.gsOut(i);  
			//f.pln(" $$$ GsPlan: i = "+i+" gsOut_i() = "+Units.str("kn",gsOut_i));
			add(np.position(),np.label(),lpc.getInfo(i),gsOut_i);
		}
		startTime = lpc.getFirstTime();
	}
		
	/** Converts a full linear plan into a GsPlan.
	 *  The ground speeds correspond to the speeds in the "lcp" linear plan.
	 *  However, no route radii values are set using this constructor.
	 *  @param lpc     linear plan
    */
	public GsPlan(Plan lpc) {
		this(lpc,0,lpc.size()-1);
	}
	
	/** Creates a copy of a GsPlan
	 * 
	 * @param gsp
	 */
	public GsPlan(GsPlan gsp) {
		rt = new Route(gsp.rt);
		id = gsp.id;
		gsOuts  = new ArrayList<Double>(gsp.gsOuts);
		startTime = gsp.startTime;
	}
	
	/** Creates a gsPlan from a route
	 * 
	 * @param rt          the source route
	 * @param name        the name to be given to the GsPlan
	 * @param startTime   start time of the GsPlan
	 * @param gsAll       ground speed to be assigned to every segment
	 */
	public GsPlan(Route rt, String name, double startTime, double gsAll) {
		this.rt = rt;
		id = name;
		gsOuts  = new ArrayList<Double>();
		for (int j = 0; j < rt.size(); j++) {		
			gsOuts.add(gsAll);
		}
 		this.startTime = startTime;
	}

	/** Create a new GsPlan from "gsp" with every segment given a ground speed of "gsNew"
	 * 
	 * @param gsp      The source GsPlan
	 * @param gsNew    the ground speed used on every segment
	 * @return a new GsPlan
	 */
	public static GsPlan makeGsPlanConstant(GsPlan gsp, double gsNew) {
		GsPlan gspNew = new GsPlan(gsp);
		for (int j = 0; j < gsp.size(); j++) {
			gspNew.setGs(j,gsNew);
		}
        return gspNew;
	}
	

	
	/** Create a route from a linear plan and calculate radii using "bankAngle"
	 * 
	 * @param lpc         linear plan
	 * @param bankAngle   bank angle used for turn generation
	 * @return a new GsPlan
	 */
	public static GsPlan mkGsPlanBankAngle(Plan lpc, double bankAngle) {
		Route rt = Route.mkRouteBankAngle(lpc,bankAngle);
		GsPlan gsp = new GsPlan(rt, lpc.getName(), lpc.getFirstTime(), 0.0);
		f.pln(lpc.size()+" "+gsp.size());
		for (int j = 0; j < rt.size(); j++) {
			gsp.gsOuts.set(j,lpc.gsOut(j));
		}
        return gsp;
	}

	public int size() {
		return rt.size();
	}
	
	public String getName() {
		return id;
	}
	
	public void setName(String s) {
		id = s;
	}
	
	public double gs(int i) {
		return gsOuts.get(i);
	}
	
	/** Provide a copy of the route in this GsPlan
	 * 
	 * @return route
	 */
	public Route route() {
		return new Route(rt);
	}
	
	/**
	 * Add a position
	 * @param pos position
	 * @param label label for point -- if this equals GsPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param info  information for a point
	 * @param gsOut grounds speed
	 * @param radius radius of turn
	 */
	public void add(Position pos, String label, String info, double gsOut, double radius) {
		//f.pln(" $$###>>>>> GsPlan.add: "+pos+" "+label+" gsin = "+Units.str("kn",gsin)+" radius = "+Units.str("nm",rad));
		rt.add(pos, label, info, radius);
		gsOuts.add(gsOut);
	}
	
	
	/**
	 * Sets a point
	 * @param ix  index of the point
	 * @param pos position
	 * @param label label for point -- if this equals GsPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param info  information for a point
	 * @param gsOut ground speed out of "ix"
	 */
	public void set(int ix, Position pos, String label, String info, double gsOut) {
		//f.pln(" $$###>>>>> GsPlan.set: ix = "+ix+" ps = "+pos+" "+label+" gsin = "+Units.str("kn",gsOut));
        double radius = 0.0;
		rt.set(ix, pos, label, info, radius);
	    gsOuts.set(ix, gsOut);
	}

	
	/**
	 * Add a position 
	 * @param pos position
	 * @param label label for point -- if this equals GsPlan.virtualName, then this will become a virtual point when make into a linear plan
	 * @param info  information for a point
	 * @param gsOut ground speed out 
	 */
	public void add(Position pos, String label, String info, double gsOut) {
        double radius = 0.0;
		rt.add(pos, label, info, radius);
		gsOuts.add(gsOut);
	}

	
	/** This method is primarily added to prevent accidental use of lower level Route method
	 * 
	 *  It makes the ground speed the same as the last one, if there is a previous point, otherwise -1.0
	 * 
	 */
	public void add(Position pos, String label, String info) {
		rt.add(pos, label, info, 0.0);
		if (gsOuts.size() > 0) {
		   gsOuts.add(gsOuts.get(gsOuts.size()-1));
		} else {
		   gsOuts.add(-1.0);
		}
	}
	
	
	/** add point "ix" from GsPlan "p"
	 * 
	 * @param p
	 * @param ix
	 */
	public void add(GsPlan p, int ix) {
		rt.add(p.position(ix),p.name(ix),p.info(ix),p.radius(ix));
		gsOuts.add(p.gsOuts.get(ix));
		//f.pln(" $$ GsPlan add "+p.names.get(ix));
	}

	public void addAll(GsPlan p) {
		rt.addAll(p.rt);
		gsOuts.addAll(p.gsOuts);
	}
	
	/** Create a new GsPlan that is a copy of this one, then add all the elements from p2 to this new GsPlan 
	 * 
	 * @param p2
	 * @return
	 */
	public GsPlan append(GsPlan p2) {
		GsPlan rtn = new GsPlan(this);
		rtn.addAll(p2);
		//f.pln(" $$ GsPlan:  append: rtn.size() = "+rtn.size());
		return rtn;
	}
	
	public void remove(int i) {
		rt.remove(i);
		gsOuts.remove(i);
	}


	public Position position(int i) {
		return rt.position(i);				
	}
	
	public Position last() {
		return rt.position(size()-1);
	}
	
	public String name(int i) {
		return rt.name(i);
	}
	
	public String info(int i) {
		return rt.info(i);
	}
	
	public double radius(int i) {
		return rt.radius(i);
	}
	
	public void setRadius(int i, double rad) {
		if (i < 0 || i >= size()) {
			f.pln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			rt.setRadius(i,rad);
		}
	}

	public ArrayList<Double> getGsInits() {
		return gsOuts;
	}

	public void setGs(int i, double gsin) {
	     gsOuts.set(i,gsin);
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

	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param label      String to match
	 */
	public int findName(String nm) {
		return rt.findName(nm);
	}

	public int findInfo(String nm) {
		return rt.findInfo(nm);
	}

	
	
	public double pathDistance(int i, int j, boolean linear) {
		return rt.pathDistance(i, j, linear);
	}

//	public double pathDistance(boolean linear) {
//		return pathDistance(0,size()-1,linear);
//	}
//	
//	public double pathDistance() {
//		boolean linear = false;
//		return pathDistance(0,size()-1,linear);
//	}


	/**
	 * Return ETA for this gsplan
	 * @param linear false to include kinematic plans
	 * @return estimated final time for this plan
	 */
	public double ETA(boolean linear) {
		double tot = startTime;
		double mygs = 1.0;
		for (int i = 0; i < size()-1; i++) {
			if (gs(i) > 0.0) {
				mygs = gs(i);
			}
			tot += pathDistance(i, i+1, linear) / mygs;
		}
		return tot;
	}

	public Plan linearPlan() {
		Plan lpc = new Plan("");
		if (rt.size() < 1) return lpc;
		double lastT = startTime;
		Position lastNp = position(0);
		//f.pln(" $$$ GsPlan.linearPlan: lastNp = "+lastNp);
		lpc.addNavPoint(new NavPoint(lastNp,startTime).makeLabel(name(0)));
		for (int i = 1; i < rt.size(); i++) {
			Position np = position(i);
			double pathDist = np.distanceH(lastNp);
			double gs_i = gsOuts.get(i-1);
			double t = lastT + pathDist/gs_i;
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs_i)+" t = "+t);
			NavPoint nvp = new NavPoint(np,t).makeLabel(name(i));
			//f.pln(" $$$ GsPlan.linearPlan: i = "+i+" nvp = "+nvp);
			TcpData tcp = TcpData.makeSource(nvp).setRadiusSigned(radius(i));
			//NavPt nvPair= new NavPt(nvp,tcp);
			if (name(i).equals(Route.virtualName)) tcp = tcp.setVirtual();
			//f.pln(" $$$$$ GsPlan.linearPlan: nvp = "+nvp.toStringFull());
			lpc.add(nvp,tcp);
			lastT = t;
			lastNp = np;
		}
		return lpc;
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

	/**
	 * Return the segment containing the point closest to the given position.
	 */
	public int closestSegment(Position pos) {
		Plan pl = linearPlan();
		double t = pl.closestPoint(pos).time();
		return pl.getSegment(t);
	}
	
	
	/** 
	 * Position at distance
	 * @param dist distance to query
	 * @param defaultBank default bank angle for turns (overridden by radius)
	 * @param linear flag to generate turns
	 * @return position at horizontal distance (does not incorporate vertical or ground speed accelerations)
	 */
	public Position positionFromDistance(double dist, double defaultBank, boolean linear) {
		Plan p = linearPlan();
		double startTime = p.getFirstTime();
		if (!linear) {
			p = TrajGen.generateTurnTCPs(p, defaultBank);
		}
		return PlanUtil.advanceDistance(p, startTime, dist, false).first;
	}

	/** 
	 * Velocity at distance
	 * @param dist distance to query
	 * @param defaultBank default bank angle for turns (overridden by radius)
	 * @param linear flse to generate turns
	 * @return velocity at horizontal distance (does not incorporate vertical or ground speed accelerations)
	 */
	public Velocity velocityFromDistance(double dist, double defaultBank, boolean linear) {
		Plan p = linearPlan();
		if (!linear) {
			p = TrajGen.generateTurnTCPs(p, defaultBank);
		}
		return p.velocityByDistance(dist);
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
	
	public boolean almostEquals(GsPlan p) {
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
		String rtn = "GsPlan size = "+rt.size()+"\n";
		for (int i = 0; i < rt.size(); i++) {
			rtn += " "+i+" "+position(i)+" "+name(i);
			if (radius(i) != 0.0) rtn += " radius ="+radius(i);
			rtn += " gsInit = "+Units.str("kn",gsOuts.get(i));
			rtn += "\n";
		}
		return rtn;
	}

	
}

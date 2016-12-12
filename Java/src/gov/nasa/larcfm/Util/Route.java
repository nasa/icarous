/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;


/* Tools for creating a Route, that is a sequence of 3D positions
 * 
 */

public class Route {

	private ArrayList<String>   names;    
	private ArrayList<Position> positions;
	private ArrayList<Double> radius;                               // optional information about turn
	public static final String virtualName = "$virtual";

	public Route() {
		positions = new ArrayList<Position>();
		names = new ArrayList<String>();
		radius = new ArrayList<Double>();
	}
	
	public Route(Route gsp) {
		positions = new ArrayList<Position>(gsp.positions);
		names = new ArrayList<String>(gsp.names);
		radius = new ArrayList<Double>(gsp.radius);
	}

	/** Creates a route from a linear plan, all points have radius 0.0
	 * 
	 * @param lpc     linear plan
	 * @param start   starting index
	 * @param end     ending index
	 */
	public Route(Plan lpc, int start, int end) {
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		positions = new ArrayList<Position>();
		names = new ArrayList<String>();
		radius = new ArrayList<Double>();
		for (int i = start; i <= end; i++) {
			NavPoint np = lpc.point(i);
			add(np.position(),np.label());
		}
	}
	
	/** Create a route from a linear plan, all points have radius 0.0
	 * 
	 * @param lpc      linear plan
	 * */
	public Route(Plan lpc) {
		this(lpc,0,lpc.size()-1);
	}

	/** Create a route from a linear plan and calculate radii using "radius"
	 * 
	 * @param lpc      linear plan
	 * @param radius   this radius value is inserted at all vertex points (used in path distance calculations)
	 * @return         route generated from linear plan
	 */
	public static Route mkRoute(Plan fp, int start, int end, double radius) {
		if (start < 0) start = 0;
		if (end >= fp.size()) end = fp.size()-1;
		Route rt = new Route();
		for (int i = start; i <= end; i++) {
			NavPoint np = fp.point(i);
			rt.add(np.position(),np.label(),radius);
		}
		return rt;
	}

	/** Create a route from a plan fp using only the named points.   All other points are discarded
	 * 
	 * @param fp    source plan
	 * @return      route constructed from named points in plan "fp"
	 */
	public static Route mkRouteNamedOnly(Plan fp) {
		Route rt = new Route();
		for (int i = 0; i < fp.size(); i++) {
			NavPoint np = fp.point(i);
			if (! np.label().equals("")) {
			    rt.add(np.position(),np.label());
			}
		}
		return rt;
	}
	
	/** Create a route from a linear plan and make all radii have the value "radius"
	 * 
	 * @param fp       source plan
	 * @param radius   radius to be used at every vertex (for path distance calculations)
	 * @return
	 */
	public static Route mkRoute(Plan fp, double radius) {
	     return mkRoute(fp,0,fp.size()-1,radius);
	}

	
	/** Create a route from a linear plan and calculate radii using "bankAngle"
	 * 
	 * @param lpc         linear plan
	 * @param bankAngle   bank angle used to calculate radii values (used in path distance calculations)
	 * @return            Route generated from a linear plan fp,
	 *                    
	 */
	public static Route mkRouteBankAngle(Plan fp, double bankAngle) {
		Plan lpc = fp.copyWithIndex();
		//Plan kpc = TrajGen.generateTurnTCPs(lpc, bankAngle);
		boolean continueGen = true;
		boolean strict = false;
		//f.pln(" $$$ mkRouteBankAngle: lpc = "+lpc.toStringGs());
		Plan kpc = TrajGen.generateTurnTCPs(lpc,bankAngle,continueGen,strict);
		//f.pln(" $$$ mkRouteBankAngle: kpc = "+kpc);
		if (kpc.hasError()) {
			f.pln(" WARNING: mkRouteBankAngle: "+kpc.getMessageNoClear());
		}
		Route rt = new Route();
	    double radius = 0.0;
	    String labelBOT = "";
	    int lastLinIndex = -1;
		for (int j = 0; j < kpc.size(); j++) {
		    NavPoint np = kpc.point(j);
		    //f.pln(" $$$>>>>>>>>>>>>>>>>>>>>>>>>>.. makeRoute: np = "+np);
		    if (np.isBOT()) {
		    	radius = np.signedRadius();
		    	labelBOT = np.label();
		    } else if (np.isEOT()) {
		    	radius = 0.0;
		    	labelBOT = "";
		    } else {
		    	String label = np.label();
		    	if (radius != 0) label = labelBOT;
		    	int linIndex = np.linearIndex();
		    	//f.pln(" $$$>>>>>>>>>>>>>>>>>>>>>>>>>.. makeRoute: linIndex = "+linIndex+" lastLinIndex = "+lastLinIndex);
		    	if (linIndex != lastLinIndex) { 
		    		NavPoint np_lpc = lpc.point(linIndex);
			    	//f.pln(" $$$>>>>>>>>>>>>>>>>>>>>>>>>> radius = "+Units.str("NM",radius));
		    	    rt.add(np_lpc.position(),label,radius);
		    	    lastLinIndex = linIndex;
		    	}
		    }
		}
		return rt;
	}

	/** Create a route from a linear plan and make all radii have the value "radius"
	 * 
	 * @param fp       source plan
	 * @param radius   radius to be used at every vertex (for path distance calculations)
	 * @return
	 */
	public static Route mkRouteCut(Route fp, int start, int end) {
		if (start < 0) start = 0;
		if (end >= fp.size()) end = fp.size()-1;
		Route rt = new Route();
		for (int i = start; i <= end; i++) {
			//f.pln(" $$ mkRouteCut: i = "+i+" fp.position(i) = "+fp.position(i)+" fp.name(i) = "+fp.name(i));
			rt.add(fp.position(i),fp.name(i),fp.radius(i));
		}
		return rt;

	}

	
	public int size() {
		return positions.size();
	}
	
	public Position position(int i) {
		if (i < 0 || i >= size()) return Position.INVALID;
		else return positions.get(i);
	}
	
	public Position positionByDistance(double dist, boolean linear) {
        double anyGs = Units.from("kts",300);
		Plan p = linearPlan(0,anyGs);
		double startTime = p.getFirstTime();
		if ( ! linear) {
			p = TrajGen.generateTurnTCPsRadius(p); //, 0.0);
		}
		return PlanUtil.advanceDistance(p, startTime, dist, linear).first;
	}

	public Position positionByDistance(int i, double dist, boolean linear) {
        double anyGs = Units.from("kts",300);
		Plan p = linearPlan(0,anyGs);
		double startTime = p.getTime(i);
		if ( ! linear) {
			p = TrajGen.generateTurnTCPsRadius(p); //, 0.0);
		}
		return PlanUtil.advanceDistance(p, startTime, dist, linear).first;
	}
	
	public String name(int i) {
		if (i < 0 || i >= size()) return "<INVALID>";
		else return names.get(i);
	}
	
	public double radius(int i) {
		if (i < 0 || i >= size()) return 0;
		else return radius.get(i);
	}

	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 */
	public void add(Position pos, String label) {
		positions.add(pos);
		names.add(label);
		radius.add(0.0);
	}
	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 */
	public void add(Position pos,  String label, double rad) {
		//f.pln(" ################### Route.add: "+pos+" "+label+" radius = "+Units.str("nm",rad));
		positions.add(pos);
		names.add(label);
		radius.add(rad);
	}

	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 */
	public void add(int ix, Position pos,  String label, double rad) {
		positions.add(ix,pos);
		names.add(ix,label);
		radius.add(ix,rad);
	}
		
	public void remove(int i) {
		positions.remove(i);
		names.remove(i);
		radius.remove(i);
	}
	
	public void removeFirst() {
		positions.remove(0);
		names.remove(0);
		radius.remove(0);
	}

	
	public void removeLast() {
		remove(positions.size()-1);
	}

	
	public void add(Route p, int ix) {
		positions.add(p.positions.get(ix));
		names.add(p.names.get(ix));
		radius.add(p.radius.get(ix));
	}
	
	public void addAll(Route p) {
		positions.addAll(p.positions);
		names.addAll(p.names);
		radius.addAll(p.radius);
	}
	
	public Route append(Route p2) {
		Route rtn = new Route(this);
		rtn.addAll(p2);
		return rtn;
	}


	public void updateWithDefaultRadius(double default_radius) {
		for (int i = 0; i <= size(); i++) {
			if (radius.get(i) == 0.0) {
				radius.set(i, default_radius);
			}
		}
	}

	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param label      String to match
	 */

	public int findName(String nm, int startIx) {
		for (int i = startIx; i < positions.size(); i++) {
			String name = names.get(i);
			if (name.equals(nm)) return i;
		}
		return -1;
	}
	
	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param label      String to match
	 */

	public int findName(String nm) {
		return findName(nm, 0);
	}

	
	public void setName(int i, String name) {
		if (i < 0 || i >= size()) {
			f.pln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			names.set(i,name);
		}
	}
	
	public void setPosition(int i, Position pos) {
		if (i < 0 || i >= size()) {
			f.pln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			positions.set(i,pos);
		}
	}
	
	public void setRadius(int i, double rad) {
		if (i < 0 || i >= size()) {
			f.pln(" $$$ ERROR: Route.setName: index out of range");
		} else {
			radius.set(i,rad);
		}
	}

	public Route copy() {
		Route rtn = new Route();
		for (int i = 0; i < positions.size(); i++) {
             rtn.add(positions.get(i), names.get(i), radius.get(i));
		}
		return rtn;
	}
	
	/**
	 * Find the path distance between the given starting and ending indexes
	 * @param i    starting index
	 * @param j    ending index
	 * @return
	 */
	public double pathDistance(int i, int j, boolean linear) {
		//f.pln(" $$ pathDistance: ENTER ============================== i = "+i+" j = "+j);
        double anyGs = Units.from("kts",300);
		Plan linPlan = linearPlan(0.0,anyGs);
		Plan kpc = linPlan;
		
		if (i >= size()) {
			i = size()-1;
		}
		if (j >= size()) {
			j = size()-1;
		}
		if (i < 0) {
			i = 0;
		}
		if (j < 0) {
			j = 0;
		}		
		int kix = i;
		int kjx = j;
		if ( ! linear) {
		    kpc = TrajGen.generateTurnTCPsRadius(linPlan); //, 0.0); // , bankAngle); // -- is this right?
		    if (kpc.hasError()) { // No valid radius values are available
		    	linear = true;
		    	//f.pln(" $$ pathDistance: $$$$$$$$$$$$$ HERE I AM $$$$$$$$$$$$$$$$ "+kpc.getMessageNoClear());
		    	return linPlan.pathDistance(i,j,linear);
		    } else {
		    	//f.pln(" $$ Route.pathDistance: ................... kpc = "+kpc.toString());
		    	ArrayList<Integer> iAl = kpc.findLinearIndex(i);
		    	//f.pln(" iAl = "+iAl);
		    	if (iAl.size() == 0) return -1;
		    	if (kpc.point(iAl.get(0)).isBOT()) {
		    		kix = iAl.get(1);
		    	} else {
		    		kix = iAl.get(0);
		    	}			
		    	ArrayList<Integer> jAl = kpc.findLinearIndex(j);
		    	//f.pln(" j="+j+"  jAl = "+jAl);
		    	if (jAl.size() == 0) return -1;
		    	if (kpc.point(jAl.get(0)).isBOT()) {
		    		kjx = jAl.get(1);
		    	} else {
		    		kjx = jAl.get(0);
		    	}
		    }
		}
		double rtn = kpc.pathDistance(kix,kjx,linear);
		//f.pln(" $$ Route.pathDistance: kix = "+kix+"  kjx="+kjx+" rtn = "+Units.str("NM",rtn));	
		//f.pln(" $$ pathDistance: EXIT ============================== i = "+i+" j = "+j+" rtn = "+Units.str("NM",rtn));
		return rtn;
	}
	
	
	public double pathDistance(boolean linear) {
		return pathDistance(0,size()-1,linear);
	}
	
	
	public double pathDistance() {
		boolean linear = false;
		return pathDistance(0,size()-1,linear);
	}

	/**
	 * Position along the route
	 * @param dist distance to query
	 * @param gs ground speed (must be greater than zero, but otherwise only used for turn generation)
	 * @param defaultBank default bank angle for turns (overridden by radius info)
	 * @param linear true to remain linear, false to generate turns.
	 * @return Position at distance  (does not incorporate vertical or ground speed accelerations)
	 */
	public Position positionFromDistance(double dist, double gs, double defaultBank, boolean linear) {
		Plan p = linearPlan(0,gs);
		double startTime = p.getFirstTime();
		if (!linear) {
			p = TrajGen.generateTurnTCPs(p, defaultBank);
		}
		return PlanUtil.advanceDistance(p, startTime, dist, false).first;
	}

	/**
	 * Velocity along the route (primarily track)
	 * @param dist distance to query
	 * @param gs ground speed (must be greater than zero, but otherwise only used for turn generation)
	 * @param defaultBank default bank angle for turns (overridden by radius info)
	 * @param linear true to remain linear, false to generate turns.
	 * @return Velocity at distance (does not incorporate vertical or ground speed accelerations)
	 */
	public Velocity velocityFromDistance(double dist, double gs, double defaultBank, boolean linear) {
		Plan p = linearPlan(0,gs);
		if (!linear) {
			p = TrajGen.generateTurnTCPs(p, defaultBank);
		}
		return p.velocityByDistance(dist);
	}


	public Plan linearPlan(double startTime, double gs) {
		Plan lpc = new Plan("");
		if (positions.size() < 1) return lpc;
		double lastT = startTime;
		Position lastNp = positions.get(0);
		lpc.add(new NavPoint(lastNp,startTime).makeLinearIndex(0).makeLabel(names.get(0)));
		for (int i = 1; i < positions.size(); i++) {
			Position np = positions.get(i);
			double pathDist = np.distanceH(lastNp);
			double t = lastT + pathDist/gs;
			double rad = radius.get(i);
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs)+" t = "+t+" rad = "+Units.str("ft",rad));
			NavPoint nvp = new NavPoint(np,t).makeRadius(rad).makeLinearIndex(i).makeLabel(names.get(i));
			if (names.get(i).equals(virtualName)) nvp = nvp.makeVirtual();
			lpc.add(nvp);
			lastT = t;
			lastNp = np;
		}
		return lpc;
	}
	
	// TODO: NMG method to avoid assuming a groundspeed (assume linear increasing index time)
	public Plan path2D() {
		Plan pathPlan = new Plan("");
		if (positions.size() < 1) return pathPlan;
		double t = 0.0;
		pathPlan.add(new NavPoint(positions.get(0),t).makeLinearIndex(0).makeLabel(names.get(0)));
		for (int i = 1; i < positions.size(); i++) {
			Position np = positions.get(i);
			double rad = radius.get(i);
			// no radius or end of route point
			if (Util.almost_equals(0.0,rad) || i==positions.size()-1) {		
				t+=1.0;
				NavPoint nvp = new NavPoint(np,t).makeRadius(rad).makeLinearIndex(i).makeLabel(names.get(i));
				if (names.get(i).equals(virtualName)) nvp = nvp.makeVirtual();
				pathPlan.add(nvp);
				continue;
			}
			// insert BOT point
			t+=1.0;
			double trkIn = GreatCircle.velocity_initial(np.lla(), positions.get(i-1).lla(), 1.0).trk() + Math.PI;
			double trkOut = GreatCircle.velocity_initial(np.lla(), positions.get(i+1).lla(), 1.0).trk();
			double theta = Util.turnDelta(trkIn, trkOut);
			double distBOT = rad * Math.tan(theta/2);
			LatLonAlt BOTLLA = GreatCircle.linear_initial(np.lla(), trkIn+Math.PI/2, distBOT);
			Position BOTpos = new Position(BOTLLA);
			NavPoint BOT = new NavPoint(BOTpos,t).makeBOT(BOTpos, t, Velocity.makeTrkGsVs(trkIn, 0.0, 0.0), Util.turnDir(trkIn, trkOut)*rad, i).makeLabel(names.get(i));
			if (names.get(i).equals(virtualName)) BOT = BOT.makeVirtual();
			pathPlan.add(BOT);
			// insert EOT point
			t+=1.0;
			double distEOT = distBOT;
			LatLonAlt EOTLLA = GreatCircle.linear_initial(np.lla(), trkOut, distEOT);
			Position EOTpos = new Position(EOTLLA);
			NavPoint EOT = new NavPoint(EOTpos,t).makeBOT(EOTpos, t, Velocity.makeTrkGsVs(trkOut, 0.0, 0.0), Util.turnDir(trkIn, trkOut)*rad, i);
			pathPlan.add(EOT);
		}
		return pathPlan;
	}
	
	/** test equality of GsPlans
	 */
	public boolean equals(Route fp) {
		for (int i = 0; i < fp.size(); i++) {                // Unchanged
			if (position(i) != fp.position(i)) return false;
			if (! name(i).equals(fp.name(i))) return false;
			if (Math.abs(radius.get(i) - fp.radius.get(i)) > 1E-10) return false;
		}
		return true;
	}


	
	public String toString() {
		String rtn = "PrePlan size = "+positions.size()+"\n";
		double dist = 0;
		for (int i = 0; i < positions.size(); i++) {
			rtn += " "+f.padLeft(""+i,2)+" "+positions.get(i)+" "+f.padRight(names.get(i),15);
			if (radius(i) != 0.0) rtn += " radius = "+Units.str("NM",radius(i));
			if (i > 0) dist += pathDistance(i-1,i, false);
			rtn += " dist="+Units.str("NM",dist);
			rtn += "\n";
		}
		return rtn;
	}
	
	public String toString(double startTime, double gs) {
	    return linearPlan(startTime,gs).toStringGs();
	}
	
}

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

	ArrayList<String>   names;    
	ArrayList<Position> positions;
	public static final String virtualName = "$virtual";

	public Route() {
		positions = new ArrayList<Position>();
		names = new ArrayList<String>();
	}
	
	public Route(Route gsp) {
		positions = new ArrayList<Position>(gsp.positions);
		names = new ArrayList<String>(gsp.names);
	}

	public Route(Plan lpc, int start, int end) {
		if (start < 0) start = 0;
		if (end >= lpc.size()) end = lpc.size()-1;
		positions = new ArrayList<Position>();
		names = new ArrayList<String>();
		for (int i = start; i <= end; i++) {
			NavPoint np = lpc.point(i);
			add(np.position(),np.label());
		}
	}
	
	public Route(Plan lpc) {
		this(lpc,0,lpc.size()-1);
	}

	
	public int size() {
		return positions.size();
	}
	
	public Position position(int i) {
		if (i < 0 || i >= size()) return Position.INVALID;
		else return positions.get(i);
	}
	
	public String name(int i) {
		if (i < 0 || i >= size()) return "<INVALID>";
		else return names.get(i);
	}
	
	/**
	 * 
	 * @param pos position
	 * @param label label for point -- if this equals Route.virtualName, then this will become a virtual point when make into a linear plan
	 */
	public void add(Position pos, String label) {
		positions.add(pos);
		names.add(label);
	}
		
	public void remove(int i) {
		positions.remove(i);
		names.remove(i);
	}
	
	public void removeFirst() {
		positions.remove(0);
		names.remove(0);
	}

	
	public void removeLast() {
		remove(positions.size()-1);
	}

	
	public void add(Route p, int ix) {
		positions.add(p.positions.get(ix));
		names.add(p.names.get(ix));
	}
	
	public void addAll(Route p) {
		positions.addAll(p.positions);
		names.addAll(p.names);
	}
	
	public Route append(Route p2) {
		Route rtn = new Route(this);
		rtn.addAll(p2);
		return rtn;
	}


	/**
	 * Return the index of first point that has a label equal to the given string -1 if there are no matches.
	 * 
	 *  @param label      String to match
	 */

	public int findName(String nm) {
		for (int i = 0; i < positions.size(); i++) {
			Position np = positions.get(i);
			String name = names.get(i);
			if (name.equals(nm)) return i;
		}
		return -1;
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

	public Route copy() {
		Route rtn = new Route();
		for (int i = 0; i < positions.size(); i++) {
             rtn.add(positions.get(i), names.get(i));
		}
		return rtn;
	}

	
	public Plan linearPlan(double startTime, double gs) {
		Plan lpc = new Plan("");
		if (positions.size() < 1) return lpc;
		double lastT = startTime;
		Position lastNp = positions.get(0);
		lpc.add(new NavPoint(lastNp,startTime).makeLabel(names.get(0)));
		for (int i = 1; i < positions.size(); i++) {
			Position np = positions.get(i);
			double pathDist = np.distanceH(lastNp);
			double t = lastT + pathDist/gs;
			//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs)+" t = "+t);
			NavPoint nvp = new NavPoint(np,t).makeLabel(names.get(i));
			if (names.get(i).equals(virtualName)) nvp = nvp.makeVirtual();
			lpc.add(nvp);
			lastT = t;
			lastNp = np;
		}
		return lpc;
	}
	
	/** test equality of GsPlans
	 */
	public boolean equals(Route fp) {
		for (int i = 0; i < fp.size(); i++) {                // Unchanged
			if (position(i) != fp.position(i)) return false;
			if (! name(i).equals(fp.name(i))) return false;
		}
		return true;
	}

	
	public String toString() {
		String rtn = "PrePlan size = "+positions.size()+"\n";
		for (int i = 0; i < positions.size(); i++) {
			rtn += " "+i+" "+positions.get(i)+" "+names.get(i);
			rtn += "\n";
		}
		return rtn;
	}
	
	public String toString(double startTime, double gs) {
	    return linearPlan(startTime,gs).toStringGs();
	}
	
}

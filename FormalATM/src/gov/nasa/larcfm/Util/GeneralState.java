/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.PolyPath.PathMode;

import java.util.ArrayList;


/**
 * A "general state" object that holds Euclidean or Lat/Lon information about position and velocity for an object.
 * Currently this can either be a (point-mass) aircraft or a (possible morphing) polygon.
 * This is intended to be traffic information that will be converted to a General3DState 
 * object to be sent to a GeneralDetector object.  
 */
public class GeneralState {
	private final String id;
	private final Position si;
	private final Velocity vi;
	private final SimpleMovingPoly mp;
	private final double t;
	private final boolean containment;

	private General3DState state = General3DState.INVALID;

	public GeneralState() {
		id = "";
		si = null;
		vi = null;
		mp = null;
		containment = false;
		t = 0.0;
	}

	public final static GeneralState INVALID = new GeneralState();

	public GeneralState(String name, Position s, Velocity v, double time) {
		id = name;
		si = s;
		vi = v;
		mp = null;
		t = time;
		containment = false;
	}

	public GeneralState(String name, SimpleMovingPoly p, double time) {
		id = name;
		si = null;
		vi = null;
		mp = p;
		t = time;
		containment = false;
	}

	public GeneralState(String name, SimpleMovingPoly p, double time, boolean cont) {
		id = name;
		si = null;
		vi = null;
		mp = p;
		t = time;
		containment = cont;
	}

	public GeneralState(GeneralState g) {
		id = g.id;
		si = g.si;
		vi = g.vi;
		if (g.mp == null) {
			mp = null;
		} else {
			mp = g.mp.copy();
		}
		t = g.t;
		containment = g.containment;
	}

	/**
	 * Creates a Euclidean GeneralState 
	 * @param g
	 */
	public static GeneralState make(General3DState g, String name, double time) {
		if (g.hasPointMass()) {
			return new GeneralState(name, new Position(g.getVect3()), g.getVelocity(), time);
		} else if (g.hasPolygon()) {
			return new GeneralState(name, SimpleMovingPoly.make(g.getPolygon()), time);
		} else {
			return INVALID;
		}
	}

	/**
	 * Creates a LatLon GeneralState
	 * @param g
	 * @param proj
	 */
	public static GeneralState make(General3DState g, EuclideanProjection proj, String name, double time) {
		if (g.hasPointMass()) {
			Pair<Position,Velocity> pr = proj.inverse(g.getVect3(), g.getVelocity(), true);
			return new GeneralState(name, pr.first, pr.second, time);
		} else if (g.hasPolygon()) {
			return new GeneralState(name, SimpleMovingPoly.make(g.getPolygon(),proj), time);
		} else {
			return INVALID;
		}		
	}

	public double distanceH(Position p) {
		if (hasPointMass()) {
			return si.distanceH(p);
		} else if (hasPolygon()) {
			return mp.position(0).distanceFromEdge(p);
		} else {
			return -1.0;
		}
	}
	
	public double distanceV(Position p) {
		if (hasPointMass()) {
			return si.distanceV(p);
		} else if (hasPolygon()) {
			return mp.position(0).distanceV(p);
		} else {
			return -1.0;
		}		
	}
	
	/**
	 * Return a representative position for this state (averagePoint in the case of polygons)
	 */
	public Position representativePosition() {
		if (hasPointMass()) {
			return si;
		} else if (hasPolygon()) {
			return mp.position(0).averagePoint();
		} else {
			return Position.INVALID;
		}				
	}
	
	public Velocity representativeVelocity() {
		if (hasPointMass()) {
			return vi;
		} else if (hasPolygon()) {
			return mp.averageVelocity();
		} else {
			return Velocity.INVALID;
		}						
	}
	
	public String getName() {
		return id;
	}

	public GeneralState copy() {
		return new GeneralState(this);
	}

	public boolean hasPointMass() {
		return si != null && vi != null;
	}

	public boolean hasPolygon() {
		return mp != null;
	}

	public Position getPosition() {
		return si;
	}

	public Velocity getVelocity() {
		return vi;
	}

	public SimpleMovingPoly getPolygon() {
		return mp;
	}

	public double getTime() {
		return t;
	}

	public boolean isContainment() {
		return hasPolygon() && containment;
	}
	
	public General3DState get3DState(EuclideanProjection proj) {
		if (si != null) {
			if (proj != null && si.isLatLon()) {
				state = new General3DState(proj.project(si), proj.projectVelocity(si, vi));; 
				return state;
			} else {
				state = new General3DState(si.point(), vi);;
				return state;
			}
		} else if (mp != null) {
			state = new General3DState(mp.getMovingPolygon(proj));
			return state;
		} else {
			return General3DState.INVALID;
		}
	}

	/**
	 * Return the last computed 3D state (via get3DState)
	 */
	public General3DState getLast3DState() {
		return state;
	}

	/**
	 * Return most recent 3d projection advanced to new time t (or invalid if get3DState has not been called)
	 */
	public General3DState pred(double t) {
		double dt = t-this.t;
		return state.linear(dt);
	}

	/**
	 * Return a new general state advanced to new (absolute) time t
	 */
	public GeneralState linearPred(double t) {
		double dt = t-this.t;
		if (hasPointMass()) {
			Position s2 = si.linear(vi, dt);
			return new GeneralState(id, s2, si.finalVelocity(s2, dt), t);
		} else if (hasPolygon()) {
			return new GeneralState(id, mp.linear(dt), t, containment);
		} else {
			return GeneralState.INVALID;
		}
	}

	/**
	 * Return a new general state advanced by (relative) time dt
	 */
	public GeneralState linear(double dt) {
		if (hasPointMass()) {
			Position s2 = si.linear(vi, dt);
			return new GeneralState(id, s2, si.finalVelocity(s2, dt), t+dt);
		} else if (hasPolygon()) {
			return new GeneralState(id, mp.linear(dt), t+dt, containment);
		} else {
			return GeneralState.INVALID;
		}
	}


	public boolean checkLatLon(boolean ll) {
		if (si == null || mp == null) {
			return true;
		}
		return ll == (si == null ? mp.isLatLon() : si.isLatLon());		
	}

	public boolean isLatLon() {
		return checkLatLon(true);
	}

	public boolean isInvalid() {
		return (si == null || vi == null) && mp == null; 
	}
	
	public String toString() {
		String s = "State="+id+" t="+t+":";
		if (hasPointMass()) {
			s = s + "s="+si.toString()+" v="+vi.toString();
		} else if (hasPolygon()) {
			s = s + mp.toString();
		} else {
			s = s + "INVALID";
		}
		return s;
	}

	public String toOutput(int precision) {
		if (hasPointMass()) {
			return id+","+si.toStringNP(precision)+","+vi.toStringNP(precision)+","+t+",-";
		} else if (hasPolygon()) {
			SimplePoly poly = mp.poly;
			String s = "";
			for (int j = 0; j < poly.size(); j++) {
				ArrayList<String> ret = new ArrayList<String>(9);
				ret.add(id);  // name is (0)
				ret.addAll(poly.getVertex(j).toStringList(precision)); //vertex 1-3
				ret.addAll(mp.vlist.get(j).toStringList()); // vel 5-7
				ret.add(f.FmPrecision(t,precision)); // time 8
				ret.add(f.FmPrecision(Units.to("ft", poly.getTop()),precision));
				s += f.list2str(ret,", ")+"\n";
			}
			return s;
		} else {
			return "";
		}
	}
}

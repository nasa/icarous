/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

/**
 * Polygons are arranged in "paths" similar to aircraft plans (and, in fact, this object can produce one 
 * -- or more -- faux-aircraft plans that approximate the area in question).  The polygon is allowed to 
 * change shape as it travels along the path, but the number of vertices must remain constant.  The shape
 * at non-specified time points is linearly interpolated from the neighboring points.   
 * 
 * Paths with a single entry (at any time) are considered to be static, immobile polygons that exist at 
 * all times
 *  
 *  Paths can have one of 3 modes:
 *  MORPHING (the default): all polygons must have the same number of vertices.  Velocities are calculated by vertex between consecutive steps.
 *  AVG_VEL: Velocities are calculated between the average points of consecutive steps.  Polygons do not need to have the same number of vertices at each step.
 *  USER_VEL: Velocities are user-specified and applied to all vertices.  Polygons do not need to have the same number of vertices at each step.  The last step will have an infinitely continuing velocity.
 *  USER_VEL_FINITE: As USER_VEL, but the path does not continue past the last step.  (One-step paths are considered the same as USER_VEL.)  The last step is mostly ignored except for time and can be any polygon.
 *  
 * Note: polygon support is experimental and the interface is subject to change!
 */


public class PolyPath implements ErrorReporter {
	public enum PathMode {MORPHING, AVG_VEL, USER_VEL, USER_VEL_FINITE};
	// note: new mode to preserve same pattern of functionality with AVG_VEL, etc, as opposed to additional special cases when dealing with limits on USER_VEL

	private ArrayList<SimplePoly> polyList = new ArrayList<SimplePoly>(2);
	private ArrayList<Double> times = new ArrayList<Double>(2);
	private ArrayList<Velocity> vlist = new ArrayList<Velocity>(2);
	private String name = "";
	private ErrorLog error = new ErrorLog("PolyPath");
	private PathMode mode = PathMode.MORPHING;


	/**
	 * Constructor
	 */
	public PolyPath() {
	}

	/**
	 * Constructor with associated name
	 * @param n name string
	 */
	public PolyPath(String n) {
		name = n;
	}

	/**
	 * Make a PolyPlan for a non-dynamic area p.
	 * This defaults to MORPHING mode.
	 * 
	 * @param id identifier
	 * @param p polygon
	 */
	public PolyPath(String id, SimplePoly p) {
		name = id;
		polyList.add(p.copy());
		times.add(0.0);
		vlist.add(Velocity.INVALID);
		mode = PathMode.MORPHING;
		//	  calcVelocities = true;
		//	  morphingPolys = true;
	}

	/**
	 * Construct a new (dynamic) path including polygon p at time t
	 * This defaults to MORPHING mode.
	 * 
	 * @param id identifier
	 * @param p polygon
	 * @param t time
	 */
	public PolyPath(String id, SimplePoly p, double t) {
		name = id;
		polyList.add(p.copy());
		times.add(t);
		vlist.add(Velocity.INVALID);
		mode = PathMode.MORPHING;
		//	  calcVelocities = true;
		//	  morphingPolys = true;
	}

	/**
	 * This defaults to USER_VEL mode.
	 * 
	 * @param id identifier
	 * @param p polygon
	 * @param v velocity
	 * @param t time
	 */
	public PolyPath(String id, SimplePoly p, Velocity v, double t) {
		name = id;
		polyList.add(p.copy());
		times.add(t);
		vlist.add(v);
		mode = PathMode.USER_VEL;
		//	  calcVelocities = false;
		//	  morphingPolys = false;
	}

	/**
	 * Construct a new path using a set of SimplePolies and associated times.  ps and ts must both have the same length.
	 * This defaults to MORPHING mode.
	 * 
	 * @param id identifier
	 * @param ps array of SimplePolys
	 * @param ts array of times.
	 */
	public PolyPath(String id, SimplePoly[] ps, double[] ts) {
		name = id;
		if (ps.length == ts.length){ 
			for (int i=0; i < ps.length; i++) {
				addPolygon(ps[i],ts[i]);
			}
		}
		mode = PathMode.MORPHING;
		//	  calcVelocities = true;
		//	  morphingPolys = true;
	}

	/**
	 * Construct a deep copy of path p
	 * @param p path to copy
	 */
	public PolyPath(PolyPath p) {
		error = new ErrorLog(p.error);
		name = p.name;
		//	  morphingPolys = p.morphingPolys;
		mode = p.mode;
		for (int i = 0; i < p.size(); i++) {
			addPolygon(p.polyList.get(i).copy(), p.vlist.get(i), p.times.get(i));
		}
		//	  calcVelocities = p.calcVelocities;
		mode = p.mode;
	}

	/**
	 * Return a one-step finite PolyPath consisting of the given SimplePoly moving in the indicated direction
	 * This defaults to AVG_VEL mode.
	 * 
	 * @param name name
	 * @param p initial polygon
	 * @param v velocity of movement (as measured by each vertex)
	 * @param tstart start time of path (absolute)
	 * @param tend end time of path (absolute)
	 * @return path of polygon
	 */
	public static PolyPath pathFromState(String name, SimplePoly p, Velocity v, double tstart, double tend) {
		SimplePoly ep = p.linear(v, tend-tstart);
		PolyPath pp = new PolyPath(name,p,tstart);
		pp.addPolygon(ep, tend);
		pp.setName(name);
		pp.mode = PathMode.AVG_VEL;
		//	  pp.morphingPolys = true;
		//	  pp.calcVelocities = true;
		return pp;
	}

	/**
	 * Return a one-step finite LatLon PolyPath consisting of the given MovingPolygon3D
	 * This defaults to AVG_VEL mode.
	 * 
	 * @param name name
	 * @param p initial polygon
	 * @param proj projection to be used
	 * @param tstart start time of path (absolute)
	 * @param tend end time of path (absolute)
	 * @return path of polygon
	 */
	public static PolyPath pathFromState(String name, MovingPolygon3D p, EuclideanProjection proj, double tstart, double tend) {
		SimplePoly sp = SimplePoly.make(p.position(0), proj);
		SimplePoly ep = SimplePoly.make(p.position(tend-tstart), proj);
		PolyPath pp = new PolyPath(name,sp,tstart);
		pp.addPolygon(ep, tstart+tend);
		pp.setName(name);
		pp.mode = PathMode.AVG_VEL;
		//	  pp.morphingPolys = true;
		//	  pp.calcVelocities = true;
		if (sp.size() < p.size()) {
			pp.error.addWarning("pathFromState: "+pp.getName()+" attempted to add "+(p.size()-sp.size())+" invalid or duplicate vertices, ignored");
		}
		return pp;
	}

	/**
	 * Return a one-step finite Euclidean PolyPath consisting of the given MovingPolygon3D
	 * This defaults to AVG_VEL mode.
	 * 
	 * @param name name
	 * @param p initial polygon
	 * @param tstart start time of path (absolute)
	 * @param tend end time of path (absolute)
	 * @return path of polygon
	 */
	public static PolyPath pathFromState(String name, MovingPolygon3D p, double tstart, double tend) {
		SimplePoly sp = SimplePoly.make(p.position(0));
		SimplePoly ep = SimplePoly.make(p.position(tend-tstart));
		PolyPath pp = new PolyPath(name,sp,tstart);
		pp.addPolygon(ep, tstart+tend);
		pp.setName(name);
		pp.mode = PathMode.AVG_VEL;
		//	  pp.morphingPolys = true;
		//	  pp.calcVelocities = true;
		if (sp.size() < p.size()) {
			pp.error.addWarning("pathFromState: "+pp.getName()+" attempted to add "+(p.size()-sp.size())+" invalid or duplicate vertices, ignored");
		}
		return pp;
	}


	/**
	 * This defaults to MORPHING mode.
	 * 
	 * @param p polygon
	 * @param time time
	 */
	public void addPolygon(SimplePoly p, double time) {
		int i = 0;
		if (times.size() > 0 && p.size() != polyList.get(0).size()) {
			error.addError("Poly size does not match existing list!");
			return;
		}
		while(i < times.size() && times.get(i) < time) {
			i++;
		}
		polyList.add(i,p);
		times.add(i,time);
		vlist.add(i,Velocity.INVALID);
		if (times.size() > 0 && isUserVel()) {
			error.addWarning("addPolygon(p,t): previous polygons had velocity information which will be ignored");
		}
		mode = PathMode.MORPHING;
	}

	/**
	 * Add a polygon with velocity information.
	 * This defaults to USER_VEL mode.
	 * @param p         position
	 * @param v         velocity
	 * @param time      start time for this polygon
	 */
	public void addPolygon(SimplePoly p, Velocity v, double time) {
		int i = 0;
		if (times.size() > 0 && !isUserVel()) {
			error.addWarning("addPolygon(p,v,t): previous polygons did not have velocity information, it will be ignored here");
			if (p.size() != polyList.get(0).size()) {
				error.addError("Poly size does not match existing list!");
				return;
			}
		} else {
			mode = PathMode.USER_VEL; 
			//		  calcVelocities = false;
			//		  morphingPolys = false;
		}
		while(i < times.size() && times.get(i) < time) {
			i++;
		}
		polyList.add(i,p.copy());
		times.add(i,time);
		vlist.add(i,v);
	}

	/**
	 * Return a deep copy of this path.
	 * 
	 * @return path of polygon
	 */
	public PolyPath copy() {
		return new PolyPath(this);
	}

	/**
	 * Returns a Plan based on this PolyPath.  This may change to return a set of plans in the future.
	 * NOTE: This does NOT currently work properly for USER_VEL or USER_VEL_FINITE plans
	 * @return new Plan, radius, height
	 */
	public Triple<Plan,Double,Double> buildPlan() {
		Plan p = new Plan(name);
		if (size() < 1) return Triple.make(p, 0.0, 0.0);

		double maxH = 0;
		double maxD = 0;
		for (int i = 0; i < times.size(); i++) {
			SimplePoly poly = polyList.get(i);
			if (poly.size() < 2) {
				// TODO: needed for vstrat to function properly, check this
				return Triple.make(p, 0.0, 0.0);
			}
			double time = times.get(i);
			NavPoint np = new NavPoint(poly.boundingCircleCenter(),time);
			p.addNavPoint(np);
			if (Math.abs(poly.getTop()-poly.getBottom()) > maxH) { 
				maxH = Math.abs(poly.getTop()-poly.getBottom());
			}
			if (poly.boundingCircleRadius() > maxD) {
				maxD = poly.boundingCircleRadius();
			}
		}

		// static poly or USER_VEL
		if (times.size() == 1) {
			SimplePoly poly = polyList.get(0);
			NavPoint np = new NavPoint(poly.boundingCircleCenter(),Double.MAX_VALUE);			
			if (isUserVel()) {
				if (vlist.get(0).isInvalid()) {
					error.addError("USER_VEL path has invalid velocity");
				} else {
					np = new NavPoint(poly.boundingCircleCenter().linear(vlist.get(0), 36000), times.get(0)+36000);
				}
			}
			p.addNavPoint(np);
		}

		//		p.setProtectionDistance(maxD);
		//		p.setProtectionHeight(maxH/2);
		return Triple.make(p, maxD, maxH/2);
	}


	/**
	 * Return true if this path contains any geodetic points.
	 * @return true, if lat/lon
	 */
	public boolean isLatLon() {
		return polyList.size() > 0 && polyList.get(0).isLatLon();
	}

	/**
	 * Return true if this PolyPath is static (it always exists in a fixed location).
	 * This is the case if there is only one step in the path and the mode is not USER_VEL 
	 * @return true, if static
	 */
	public boolean isStatic() {
		return polyList.size() == 1 && mode != PathMode.USER_VEL;
	}

	/**
	 * Return true if this PolyPath continues to exist past the defined path (it has a velocity, which may be zero, and no definite end point)
	 * This is the case if mode is USER_VEL or if there is only one step in the path. 
	 * @return true, if continuing
	 */
	public boolean isContinuing() {
		return polyList.size() == 1 || mode == PathMode.USER_VEL;
	}

	/**
	 * Get the mode for this path.
	 *  MORPHING: all polygons must have the same number of vertices.  Velocities are calculated by vertex between consecutive steps.
	 *  AVG_VEL: Velocities are calculated between the average points of consecutive steps.  Polygons do not need to have the same 
	 *  number of vertices at each step.
	 *  USER_VEL: Velocities are user-specified and applied to all vertices.  Polygons do not need to have the same number of 
	 *  vertices at each step.  The last step will have an infinitely continuing velocity.
	 *  @return path mode
	 */
	public PathMode getPathMode() {
		return mode;
	}

	/**
	 * Helper function to combine the 2 user vel modes 
	 * @return true, if user velocity
	 */
	private boolean isUserVel() {
		return mode == PathMode.USER_VEL || mode == PathMode.USER_VEL_FINITE;
	}
	
	/**
	 * Set the mode for this path.
	 *  MORPHING: all polygons must have the same number of vertices.  Velocities are calculated by vertex between consecutive steps.
	 *  AVG_VEL: Velocities are calculated between the average points of consecutive steps.  Polygons do not need to have the same number of vertices at each step.
	 *  USER_VEL: Velocities are user-specified and applied to all vertices.  Polygons do not need to have the same number of vertices at each step.  The last step will have an infinitely continuing velocity.
	 *  Changing away from USER_VEL will erase any stored velocity information.  Changing to USER_VEL will populate the path with the same values
	 *  as AVG_VEL (the last step will have a zero velocity). 
	 *  USER_VEL_FINITE: as USER_VEL
	 *  @param m path mode
	 */
	public void setPathMode(PathMode m) {
		if (m == PathMode.MORPHING) {
			for (int i = 1; i < size(); i++) {
				if (polyList.get(i).size() != polyList.get(0).size()) {
					error.addError("setPathMode: polygon sizes are not consistent, cannot switch to MORPHING mode");
					return;
				}
			}
		}

		boolean wasUserVel = isUserVel();
		boolean willBeUserVel = (m == PathMode.USER_VEL || m == PathMode.USER_VEL_FINITE);

		// we need to fix the velocities if it did not used to be a userVel mode, and will be afterwards
		// other cases either keep the same stored velocities or ignore them
		
		if (willBeUserVel && !wasUserVel) {
			for (int i = 0; i < vlist.size(); i++) {
				vlist.set(i, initialVelocity(i));
			}			
		}
		
//		if (isUserVel()) {
//			for (int i = 0; i < vlist.size()-1; i++) {
//				if (vlist.get(i).isInvalid()) {
//					vlist.set(i, initialVelocity(i));
//				}
//			}
//			if (vlist.size() > 0 && vlist.get(vlist.size()-1).isInvalid()) {
//				vlist.set(vlist.size()-1, Velocity.ZERO);
//			}		  
//		} else {
//			for (int i = 0; i < vlist.size(); i++) {
//				vlist.set(i, Velocity.INVALID);
//			}
//		}
		mode = m;
	}



	/**
	 * Returns the name associated with this path
	 * @return name
	 */
	public String getName() {
		return name;
	}

	/**
	 * Sets the name to be associated with this path.
	 * @param n new name
	 */
	public void setName(String n) {
		name = n;
	}

	/**
	 * Returns the number of time points in this path.  Static paths will have a size of 1.
	 * @return size
	 */
	public int size() {
		return polyList.size();
	}



	/**
	 * Remove a poly from this path, by step index.
	 * @param n index
	 */
	public void remove(int n) {
		polyList.remove(n);
		times.remove(n);
		vlist.remove(n);
	}

	/**
	 * Returns the start time of this path.  Static paths have a start time of 0.
	 * @return first time
	 */
	public double getFirstTime() {
		if (times.size() > 1) return times.get(0);
		return 0.0;
	}

	/**
	 * Returns the end time of this path.  Static paths have an end time of 0.  Continuing paths have an end time of POSITIVE_INFINITY
	 * @return last time
	 */
	public double getLastTime() {
		if (isContinuing()) return Double.POSITIVE_INFINITY;
		if (times.size() > 1) return times.get(times.size()-1);
		return 0.0;
	}

	/**
	 * Returns an interpolated poly from an arbitrary time on this path.
	 * 
	 * @param time time
	 * @return polygon
	 */
	public SimplePoly interpolate(double time) {
		//f.pln("A mode="+mode+" size="+size());	  
		if (isStatic()) {
			return polyList.get(0).copy();
		}
		//f.pln("B");		

		if (times.size() == 0 || time < getFirstTime() || time > getLastTime()) {
			error.addError("interpolate: time out of bounds");
			return null;
		}
		//f.pln("C");		

		if (time == getLastTime()) {
			if (mode == PathMode.USER_VEL_FINITE) {
				Velocity v = vlist.get(size()-1);
				double dt = time - times.get(size()-1); 
				return polyList.get(size()-1).linear(v, dt);
			}
			return polyList.get(times.size()-1).copy();
		}

		//f.pln("D");		

		int i = 0;
		double t1 = times.get(0);
		double t2 = t1;
		for (int j = 0; j < times.size()-1; j++) {
			t1 = times.get(j);
			t2 = times.get(j+1);
			if (time == t1) return polyList.get(j).copy();
			if (time == t2) return polyList.get(j+1).copy();
			if (t1 < time && time < t2) {
				i = j;
				break;
			} else if (time > t2) {
				// fill in in case we go past the end
				i = j+1;
				t1 = t2;
			}
		}
		double dt = time-t1;

		SimplePoly poly1 = polyList.get(i);
		SimplePoly newPoly = new SimplePoly();
		int sz = poly1.size();
		//f.pln("poly1="+poly1.toString());

		if (mode == PathMode.MORPHING) {
			SimplePoly poly2 = polyList.get(i+1);
			//f.pln("1");
			for (int j = 0; j < sz; j++) {
				Position n1 = poly1.getVertex(j);
				Position n2 = poly2.getVertex(j);
				Position n1t = poly1.getTopPoint(j);
				Position n2t = poly2.getTopPoint(j);
				Velocity v = n1.initialVelocity(n2, t2-t1);
				Velocity vt = n1t.initialVelocity(n2t, t2-t1);
				Position p = n1.linear(v,dt);
				Position pt = n1t.linear(vt,dt);
				newPoly.addVertex(p);
				newPoly.setBottom(p.z());
				newPoly.setTop(pt.z());
			}
		} else {
			//f.pln("2");		
			Velocity v = vlist.get(getSegment(time)); // USER_VEL or USER_VEL_FINITE
			if (mode == PathMode.AVG_VEL) {
				//f.pln("3");		
				SimplePoly poly2 = polyList.get(i+1);
				v = poly1.averagePoint().initialVelocity(poly2.averagePoint(), t2 - t1);
			}
			if (v.isZero()) {
				newPoly = poly1.copy();
			} else {
				newPoly = poly1.linear(v, dt);
			}
		}
		return newPoly;
	}

	/**
	 * Returns the initial velocity of this path, starting from a given step index.
	 * @param n index
	 * @return velocity
	 */
	public Velocity initialVelocity(int n) {
//f.pln(" $$ initialVelocity "+n+": polyList.size() = "+polyList.size());
		if (times.size() == 1 && !isUserVel()) {
			return Velocity.ZERO;
		}
		if (n < 0  || n >= size()) {
			error.addError("velocity requested for out-of-bounds index!");
			return Velocity.INVALID;
		}
		if (isUserVel()) {
			return vlist.get(n);
		}
		if (n >=0 && n < polyList.size()-1) {
			Position n1 = polyList.get(n).averagePoint();
			Position n2 = polyList.get(n+1).averagePoint();
			return n1.initialVelocity(n2, times.get(n+1)-times.get(n));
		}
		return Velocity.ZERO;
	}

	public Velocity initialVertexVelocity(int vert, int n) {
		if (mode != PathMode.MORPHING) {
			return initialVelocity(n);
		}
		if (times.size() == 1) {
			return Velocity.ZERO;
		}
		if (n < 0  || n >= size()) {
			error.addError("vertex velocity requested for out-of-bounds index!");
			return Velocity.INVALID;
		}
		if (vert < 0  || vert >= polyList.get(0).size()) {
			error.addError("vertex velocity requested for out-of-bounds vertex!");
			return Velocity.INVALID;
		}
		if (n >= 0 && n < polyList.size()-1) {
			Position n1 = polyList.get(n).getVertex(vert);
			Position n2 = polyList.get(n+1).getVertex(vert);
			return n1.initialVelocity(n2, times.get(n+1)-times.get(n));
		}
		return Velocity.ZERO;
	}

	/**
	 * Returns the step index associated with a particular time, if there is a match, otherwise returns a negative value.
	 * -1 indicates a time before the path begins. -x indicates a time after point (x-1), so a time between points 1 and 2 will return index -2. 
	 * A time after the path will return -size-1.
	 * 
	 * @param time time
	 * @return index
	 */
	public int getIndex(double time) {
		if (polyList.size() == 1) return 0;
		if (time > times.get(times.size()-1)) {
			return -times.size()-1;
		}
		for (int i = 0; i < polyList.size(); i++) {
			if (times.get(i) == time) {
				return i;
			} else if (times.get(i) > time) {
				return -(i+1);
			}
		}
		return -1;
	}

	/**
	 * Returns the poly at step index n (reference).
	 * 
	 * @param n index
	 * @return polygon
	 */
	public SimplePoly getPolyRef(int n) {
		if (times.size() == 1) return polyList.get(0);
		if (n >=0 && n < times.size()) {
			return polyList.get(n);
		}
		error.addError("getPolyRef index out of bounds");
		return null;
	}

	/**
	 * Returns a copy of the poly at step index n.
	 * 
	 * @param n index
	 * @return polygon
	 */
	public SimplePoly getPoly(int n) {
		if (times.size() == 1) return polyList.get(0);
		if (n >=0 && n < times.size()) {
			return polyList.get(n).copy();
		}
		error.addError("getPoly index out of bounds");
		return null;
	}

	/**
	 * Returns the time for a given step index.
	 * 
	 * @param n index
	 * @return time
	 */
	public double getTime(int n) {
		if (times.size() == 1) return times.get(0);
		if (n >=0 && n < times.size()) {
			return times.get(n);
		}
		return 0;
	}

	/**
	 * Sets the time t for a given step n.  The order of times must be preserved. 
	 * 
	 * @param n index
	 * @param t time
	 */
	public void setTime(int n, double t) {
		if (t >= 0 && n >= 0 && n < size()) {
			if ((n > 0 && n < size()-1 && t > times.get(n-1) && t < times.get(n+1)) ||
					(n > 0 && t > times.get(n-1)) || (n <size()-1 && t < times.get(n+1)) ||
					size() == 1) {
				times.set(n,t);			  
			}
		}
	}

	/**
	 * Set the polygon at index i to be a copy of p
	 * 
	 * @param i index
	 * @param p polygon
	 */
	public void setPolygon(int i, SimplePoly p) {
		if (i < 0 || i >= times.size()) {
			error.addError("setPolygon: index out of bounds");
		} else {
			polyList.set(i,p.copy());
		}
	}

	/**
	 * Set the user velocity at index i to be v
	 * This only has effect if isCalculatedVelocities() is false.
	 * 
	 * @param i index
	 * @param v velocity
	 */
	public void setVelocity(int i, Velocity v) {
		if (i < 0 || i >= times.size()) {
			error.addError("setVelocity: index out of bounds");
		} else {
			vlist.set(i,v);
		}
	}

	/**
	 * Returns true if the polypath follows expected behaviors (i.e. increasing times and same size for each MORPHING poly).
	 * Otherwise this sets error messages.
	 * 
	 * @return true, if polypath performs expected behaviors
	 */
	public boolean validate() {
//f.pln("validate "+name);		
		double lastTime = -100;
		if (polyList.size() != times.size()) {
			error.addError(name+": poly list "+polyList.size()+" different size from times list "+vlist.size());
			return false;
		}
		if (polyList.size() != vlist.size()) {
			error.addError(name+": poly list "+polyList.size()+" different size from velocity list "+vlist.size());
			return false;
		}
		for (int i = 0 ; i < polyList.size(); i++) {
			if (mode == PathMode.MORPHING && polyList.get(0).size() != polyList.get(i).size()) {
				error.addError(name+": Mode MORPHING step "+i+" size "+polyList.get(i).size()+" != step 0 size "+polyList.get(0).size());
				//f.pln("list = "+polyList.get(i).toString());			  
				return false;
			}
			if (isUserVel() && vlist.get(i).isInvalid()) {
				error.addError(name+": Mode USER_VEL step "+i+" has invalid velocity");
			}
			if (times.get(i) < lastTime) {
				error.addError(name+": Times are not increasing from "+lastTime+" to "+times.get(i)+" "+i);
				return false;
			}
			lastTime = times.get(i);
//f.pln("check poly key "+i+" "+polyList.get(i));			
//			if (!polyList.get(i).validate(error)) {
//				error.addError(name+": Key polygon at time "+lastTime+" is invalid!");
//				return false;
//			}
		}
//f.pln("validate OK "+name);		
		return true;
	}

//	public void cleanup() {
//		for (int i = 0; i < polyList.size(); i++) {
//			polyList.set(i, polyList.get(i).cleanup());
//		}
//	}
	
	
	/**
	 * Return a truncated version of this path that ends at time t.  This will change a USER_VEL path into a USER_VEL_FINITE path.
	 * @param t The time to halt the path.
	 * @return A truncated path.  If t &le; the path's start time, then return an empty path.  If t &ge; the path's end time, then return a copy of the original path.
	 */
	public PolyPath truncate(double t) {
		if (t >= getLastTime()) return copy();
		PolyPath path = new PolyPath(name);
		if (t <= getFirstTime()) return path;
		
		SimplePoly end = interpolate(t);
		for (int i = 0; i < size() && getTime(i) < t; i++) {
			if (isUserVel()) {
				path.addPolygon(getPoly(i), initialVelocity(i), getTime(i));
			} else {
				path.addPolygon(getPoly(i), getTime(i));
			}
		}
		if (isUserVel()) {
			path.addPolygon(end, velocity(t), t);
			path.setPathMode(PathMode.USER_VEL_FINITE);
		} else {
			path.addPolygon(end, t);
			path.setPathMode(mode);
		}
		return path;
	}
	
	/**
	 * A quick and dirty method for building paths one point at a time.
	 * This does not check that there are the same number of points at each step -- call validate() on this object
	 * when done to confirm that the final path is correct.
	 * @param p A new (bottom) point.
	 * @param top p's corresponding top altitude 
	 * @param time the time for the poly p is a vertex of
	 */
	public void addVertex(Position p, double top, double time) {
		//f.pln("polypath "+getName()+" add vertex "+p);

		int index = -1;
		for (int i = 0; i < polyList.size(); i++) {
			if (Util.almost_equals(times.get(i),time)) index = i;
		}

		SimplePoly poly = new SimplePoly();
		if (index < 0) {
			index = 0;
			while(index < times.size() && times.get(index) < time) {
				index++;
			}
			polyList.add(index,poly);
			times.add(index,time);
			vlist.add(index,Velocity.INVALID);
		} else {
			poly = polyList.get(index);
		}
		if (!poly.addVertex(p)) {
			error.addWarning("addVertex: "+getName()+" attempted to add invalid or duplicate vertex, ignored");
		}
		poly.setTop(top);

	}

	/**
	 * This adds a vertex to every polygon on this path.  The vertex will be added relative to the centroid in each case, so other polygons may
	 * need to be modified to match.
	 * 
	 * @param n index
	 * @param p position
	 */
	public void addVertex(int n, Position p) {
		SimplePoly poly = polyList.get(n);
		Position cent = poly.centroid();
		Velocity v = cent.initialVelocity(p, 100);
		for (int i = 0; i < size(); i++) {
			poly = polyList.get(i);
			Position pp = poly.centroid().linear(v, 100);
			if (!poly.addVertex(pp)) {
				error.addWarning("addVertex: attempted to add duplicate vertex, ignored");
			}
		}
	}


	/**
	 * Remove the nth vertex from all polygons on the path.
	 * @param vert index of vertex to be removed.
	 */
	public void removeVertex(int vert) {
		for (int i = 0; i < size(); i++) {
			polyList.get(i).remove(vert);
		}
	}

	/**
	 * Translate the entire path by a Euclidean offset 
	 * @param off Euclidean offset
	 */
	public void translate(Vect3 off) {
		for (int i = 0; i < polyList.size(); i++) {
			polyList.get(i).translate(off);
		}
	}


	/**
	 * Shift all times in the path by the given number of seconds.
	 * @param shift Amount of shift.
	 */
	public void timeshift(double shift) {
		for (int i = 0; i < size(); i++) {
			times.set(i, times.get(i) + shift);
		}
		while (times.get(0) < 0.0) {
			times.remove(0);
			polyList.remove(0);
			vlist.remove(0);
		}

	}

	/**
	 * Return the segment number that contains 'time' in [s].  If the
	 * time is not contained in the flight plan then -1 is returned.  If
	 * the time for point 1 is 10.0 and the time for point 2 is 20.0, then
	 * getSegment(10.0) will produce 0, getSegment(15.0) will produce 0,
	 * and getSegment(20.0) will produce 1.
	 * 
	 * @param tm time
	 * @return segment number
	 */
	public int getSegment(double tm) {
		int i = getIndex(tm);
		if (i == -1) return -1;
		if (i >= 0) return i;
		if (-i-2 == size()) {
			if (mode != PathMode.USER_VEL) {
				return -1;
			} else {
				return size()-1;
			}
		}
		return -i-2;
	}  

	/**
	 * Interpolate the poly at the given time
	 * If time is outside the path's limit, return null.
	 * @param time
	 * @return polygon
	 */
	public SimplePoly position(double time) {
		if (times.size() > 1 && (time < getFirstTime() || time > getLastTime())) {
			error.addError("position requested for out-of-bounds time!");
			return null;
		}

		return interpolate(time);
	}

	/**
	 * Interpolate the averagePoint velocity at the given time
	 * If time is outside the path's limit, return invalid velocity.
	 * @param time
	 * @return velocity
	 */
	public Velocity velocity(double time) {
		if (isStatic()) {
			return Velocity.ZERO;
		}
		if (time < getFirstTime() || time > getLastTime()) {
			error.addError("velocity requested for out-of-bounds time!");
			return Velocity.INVALID;
		}
		if (Util.within_epsilon(time, getLastTime(), Constants.get_time_accuracy())) {
			return finalVelocity(size()-2);
		}
		int i = getSegment(time);
		if (isLatLon()) {
			if (isContinuing()) {
				Velocity v = vlist.get(i);
				double dt = time-getTime(i);
				if (Util.within_epsilon(dt, Constants.get_time_accuracy())) {
					return v;
				}
				Position p1 = polyList.get(i).getVertex(0);
				Position p2 = p1.linear(v, dt);
				return p2.initialVelocity(p1, dt).Neg();
			} else {
				Position p1 = polyList.get(i).averagePoint();
				Position p2 = polyList.get(i+1).averagePoint();
				double t1 = times.get(i);
				double t2 = times.get(i+1);
				Velocity v = p1.initialVelocity(p2, t2-t1);
				Position p3 = p1.linear(v, time-t1);
				return p3.initialVelocity(p2, t2-time);
			}
		} else {
			return initialVelocity(i);
		}
	}

	public Velocity vertexVelocity(int vert, double time) {
		if (mode != PathMode.MORPHING) {
			return velocity(time);
		}
		if (times.size() == 1) {
			return Velocity.ZERO;
		}
		if (time < getFirstTime() || time > getLastTime()) {
			error.addError("vertex velocity requested for out-of-bounds time!");
			return Velocity.INVALID;
		}
		if (vert < 0 || vert >= polyList.get(0).size()) {
			error.addError("vertex velocity requested for out-of-bounds index!");
			return Velocity.INVALID;
		}
		if (Util.within_epsilon(time, getLastTime(), Constants.get_time_accuracy())) {
			return finalVertexVelocity(vert, size()-2);
		}
		int i = getSegment(time);
		if (isLatLon()) {
			//		  if (isContinuing()) {
			//			  Velocity v = vlist.get(i);
			//			  double dt = time-getTime(i);
			//			  if (Util.within_epsilon(dt, Constants.get_time_accuracy())) {
			//				  return v;
			//			  }			  
			//			  Position p1 = polyList.get(i).getVertex(vert);
			//			  Position p2 = p1.linear(v, dt);
			//			  return p2.initialVelocity(p1, dt).Neg();
			//		  } else {
			Position p1 = polyList.get(i).getVertex(vert);
			Position p2 = polyList.get(i+1).getVertex(vert);
			double t1 = times.get(i);
			double t2 = times.get(i+1);
			Velocity v = p1.initialVelocity(p2, t2-t1);
			Position p3 = p1.linear(v, time-t1);
			return p3.initialVelocity(p2, t2-time);
			//		  }
		} else {
			return initialVertexVelocity(vert,i);
		}
	}

	public Velocity finalVelocity(int i) {
		if (i < 0 || i >= size()-1) {
			error.addError("final velocity index out of bounds!");
			return Velocity.INVALID;
		}
		Position p1 = polyList.get(i).averagePoint();
		Position p2 = polyList.get(i+1).averagePoint();
		return p2.initialVelocity(p1, times.get(i+1)-times.get(i)).Neg();
	}

	public Velocity finalVertexVelocity(int vert, int i) {
		if (mode != PathMode.MORPHING) {
			return finalVelocity(i);
		}
		if (i < 0 || i >= size()-1) {
			error.addError("final vertex velocity index out of bounds!");
			return Velocity.INVALID;
		}
		if (vert < 0 || vert >= polyList.get(0).size()) {
			error.addError("vertex velocity requested for out-of-bounds index!");
			return Velocity.INVALID;
		}
		Position p1 = polyList.get(i).getVertex(vert);
		Position p2 = polyList.get(i+1).getVertex(vert);
		return p2.initialVelocity(p1, times.get(i+1)-times.get(i)).Neg();
	}

	/**
	 * This will return a moving polygon that starts at point i and ends at point i+1
	 * @param i    index
	 * @param proj projection
	 * @return polygon
	 */
	public MovingPolygon3D getInitialMovingPolygon(int i, EuclideanProjection proj) {
		if (i < 0 || i >= size() || times.get(i) >= getLastTime()) {
			error.addError("getMovingPolygon index out of bounds");
			return new MovingPolygon3D();
		}	  
		Poly3D p3d = polyList.get(i).poly3D(proj);
		double dt = Double.MAX_VALUE;
		if (i < size()-1) {
			dt = times.get(i+1) - times.get(i);
		}
		if (mode == PathMode.MORPHING) {
			ArrayList<Velocity> vvlist = new ArrayList<Velocity>();
			for (int j = 0; j < p3d.size(); j++) {
				vvlist.add(initialVertexVelocity(j,i));
			}
			return new MovingPolygon3D(p3d,vvlist,dt);
		} else {
			Velocity v = initialVelocity(i);
			return new MovingPolygon3D(p3d,v,dt);
		}
	}

	/**
	 * This will return a moving polygon that STARTS at time t (relative time 0) and ends at its segment end time.
	 * 
	 * @param time
	 * @param proj
	 * @return polygon
	 */
	public MovingPolygon3D getMovingPolygon(double time, EuclideanProjection proj) {
		//f.pln("PolyPath.getMovingPolygon "+toString());	  
		if (time < getFirstTime() || time >= getLastTime()) {
			error.addError("getMovingPolygon time out of bounds");
			return new MovingPolygon3D();
		}
		Poly3D p3d = position(time).poly3D(proj);
		int seg = getSegment(time);
		double dt = Double.MAX_VALUE;
		if (seg < size()-1) {
			dt = getTime(seg+1) - time;
		}
		//f.pln("PolyPath.getMovingPolygon "+f.Fm4(time)+" seg="+f.Fm0(seg)+" dt="+f.Fm4(dt)+" p3d="+p3d.toString());
		if (mode == PathMode.MORPHING) {
			ArrayList<Velocity> vvlist = new ArrayList<Velocity>();
			for (int i = 0; i < p3d.size(); i++) {
				vvlist.add(vertexVelocity(i,time));
			}
			return new MovingPolygon3D(p3d,vvlist,dt);
		} else {
			Velocity v = velocity(time);
			return new MovingPolygon3D(p3d,v,dt);
		}
	}

	public SimpleMovingPoly getSimpleMovingPolygon(double time) {
		SimplePoly p = position(time);
		ArrayList<Velocity> vvlist = new ArrayList<Velocity>();
		for (int i = 0; i < p.size(); i++) {
			vvlist.add(vertexVelocity(i,time));
		}
		return new SimpleMovingPoly(p,vvlist);
	}

	/**
	 * Return s (hopefully) simpler (fewer vertices) version of the path that contains all points in the old path if the mode is AVG_VEL or USER_VEL.
	 * If the current mode if MORPHING, it will return a copy of this path.
	 * @param buffer
	 * @return poly path
	 */
	public PolyPath simplify(double buffer) {
		if (mode != PathMode.MORPHING) {
			PolyPath p2 = new PolyPath(name);
			p2.setPathMode(mode);
			for (int i = 0; i < size(); i++) {
//				p2.addPolygon(PolyUtil.simplify(polyList.get(i), buffer), vlist.get(i), times.get(i));
//				p2.addPolygon(PolyUtil.simplify(polyList.get(i)), vlist.get(i), times.get(i));
//				p2.addPolygon(PolyUtil.convexHull(polyList.get(i)), vlist.get(i), times.get(i));
				p2.addPolygon(PolyUtil.simplify(PolyUtil.convexHull(polyList.get(i)), buffer), vlist.get(i), times.get(i));
			}
			p2.setPathMode(mode);
			return p2;
		} else {
			error.addWarning("Cannot simplify() for MORPHING polypaths");
			return new PolyPath(this);
		}
	}

	public PolyPath simplifyToSize(int num) {
		if (mode != PathMode.MORPHING) {
			PolyPath p2 = new PolyPath(name);
			p2.setPathMode(mode);
			for (int i = 0; i < size(); i++) {
				p2.addPolygon(PolyUtil.simplify(polyList.get(i), num), vlist.get(i), times.get(i));
			}
			p2.setPathMode(mode);
			return p2;
		} else {
			error.addWarning("Cannot simplifyToSize() for MORPHING polypaths");
			return new PolyPath(this);
		}
		
	}
	
	private static final String nl = System.getProperty("line.separator");

	/**
	 * Returns a string representation of this path.
	 */
	public String toString() {
		String s = "POLY "+name+" mode="+mode+nl;
		for (int i = 0; i < size(); i++) {
			s = s + f.Fm2(times.get(i))+"s : "+polyList.get(i)+" v="+initialVelocity(i).toString()+nl;
		}
		return s;
	}


	/**
	 * Returns a string representation of this path that is compatible with the PolyReader input format.
	 * @return string representation
	 */
	public String toOutput() {
		return toOutput(Constants.get_output_precision(), false);
	}


	public String toOutput(int precision, boolean tcpColumns) {
		StringBuffer sb = new StringBuffer(100);
		for (int i = 0; i < times.size(); i++) {
			for (int j = 0; j < polyList.get(i).size(); j++) {
				List<String> ret = toStringList(i,j,precision,tcpColumns);
				sb.append(f.list2str(ret,", ")+nl);
			}
		}
		return sb.toString();
	}

	/**
	 * Represent this polypath as a list of strings
	 * @param i step number
	 * @param j vertex number
	 * @param precision
	 * @param tcpColumns
	 * @return list of strings
	 */
	public List<String> toStringList(int i, int j, int precision, boolean tcpColumns) {
		SimplePoly poly = polyList.get(i);
		ArrayList<String> ret = new ArrayList<String>(TcpData.TCP_OUTPUT_COLUMNS+2);
		ret.add(name);  // name is (0)
		ret.addAll(poly.getVertex(j).toStringList(precision)); //vertex 1-3
		ret.add(f.FmPrecision(times.get(i),precision)); // time 4
		if (tcpColumns) {
			int start = 4; 
			for (int k = start; k < TcpData.TCP_OUTPUT_COLUMNS; k++) {
				ret.add("-");
			}
		} else {
			ret.add("-"); // label
		}
		ret.add(f.FmPrecision(Units.to("ft", poly.getTop()),precision));
		if (isUserVel()) {
			ret.addAll(initialVelocity(j).toStringList()); // vel columns
		}
		return ret; 
	}
	
	public SimpleMovingPoly getSimpleMovingPoly(int i) {
		SimplePoly poly = getPoly(i);
		if (mode == PathMode.MORPHING) {
			ArrayList<Velocity> vvlist = new ArrayList<Velocity>();
			for (int j = 0; j < poly.size(); j++) {
				vvlist.add(initialVertexVelocity(j,i));
			}
			return new SimpleMovingPoly(poly,vvlist);
		} else {
			return new SimpleMovingPoly(poly,initialVelocity(i));
		}
	}

	public SimpleMovingPoly getSimpleMovingPoly(double t) {
		SimplePoly poly = interpolate(t);
		if (mode == PathMode.MORPHING) {
			ArrayList<Velocity> vvlist = new ArrayList<Velocity>();
			for (int j = 0; j < poly.size(); j++) {
				vvlist.add(vertexVelocity(j,t));
			}
			return new SimpleMovingPoly(poly,vvlist);
		} else {
			return new SimpleMovingPoly(poly,velocity(t));
		}
	}


	/**
	 * Return true if position p is within the polygon at time t
	 * 
	 * @param p position
	 * @param t time
	 * @return true, if within polygon
	 */
	public boolean contains(Position p, double t) {
		return t >= getFirstTime() && t <= getLastTime() && position(t).contains(p);
	}

	public boolean contains2D (Position p, double t) {
//f.pln("polypath contains2d:"+position(t)+"\ncontains:"+p+" "+(position(t).contains2D(p)));		
		return t >= getFirstTime() && t <= getLastTime() && position(t).contains2D(p);
	}

	public int totalNumberVertices() {
		int count = 0;
		for (int i = 0; i < polyList.size(); i++) {
			count += polyList.get(i).size();
		}
		return count;
	}
	
	public BoundingRectangle getBoundingRectangle() {
		BoundingRectangle br = new BoundingRectangle();
		for (int i = 0; i < polyList.size()-1; i++) {
			br.add(polyList.get(i).getBoundingRectangle());
		}
		if (mode == PathMode.USER_VEL && !vlist.get(size()-1).isZero()) {
			// add last poly
			br.add(polyList.get(size()-1).getBoundingRectangle());
			// add poly 20 hours from now
			br.add(position(times.get(size()-1)+72000).getBoundingRectangle());
		} else if (mode == PathMode.USER_VEL_FINITE) {
			// do not add last poly
			br.add(position(getLastTime()).getBoundingRectangle());
		} else {
			// add last poly
			br.add(polyList.get(size()-1).getBoundingRectangle());
		}
		
		return br;
	}
	
	// ErrorReporter Interface Methods

	public boolean hasError() {
		return error.hasError();
	}
	public boolean hasMessage() {
		return error.hasMessage();
	}
	public String getMessage() {
		return error.getMessage();
	}
	public String getMessageNoClear() {
		return error.getMessageNoClear();
	}


}

/* 
 * StateVector
 *
 * Contact: Rick Butler
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

/**
 * Captures the position and velocity at a certain point in time.  This class requires 
 * the position and velocity are already in a Cartesian frame.
 *
 */
public final class StateVector implements OutputList {

	public Vect3    s;
	public Velocity v;
	public double   t;

	/**
	 * Creates a state vector including the current time,
	 * position and velocity
	 * 
	 * @param s position vector (Vect3)
	 * @param v Velocity vector (Velocity)
	 * @param t time in seconds (double)
	 * 
	 */
	public StateVector(Vect3 s, Velocity v, double t) {
		this.s = s;
		this.v = v;
		this.t = t;
	}

	/**
	 * Creates an all-zeros state vector including the current time,
	 * position and velocity
	 * 
	 */
	public StateVector() {
		this.t = 0.0;
		this.s = Vect3.ZERO;
		this.v = Velocity.ZERO;
	}
	
	public static StateVector makeXYZ(double x, double y, double z, double trk, double gs, double vs, double t){
		Vect3 s = Vect3.makeXYZ(x,"nmi",y,"nmi",z,"ft");
		Velocity v = Velocity.makeTrkGsVs(trk,gs,vs);
		return new StateVector(s,v,t);
	}
	
	public StateVector(Pair<Vect3,Velocity> sv, double t ) {
		this.s = sv.first;
		this.v = sv.second;
		this.t = t;
  }


	
	public Pair<Vect3,Velocity> pair() {
		return new Pair<Vect3,Velocity>(s,v);
	}
	
	public Vect3 s() {
		return s;
	}

	public Point point() {
		return Point.mk(s);
	}

	public Velocity v() {
		return v;
	}

	public double t() {
		return t;
	}

	public String toString() {
		return f.sStr(s)+" "+v.toString()+" "+f.Fm2(t);
	}
	
	public String toString8() {
		return f.sStr8(s)+" "+v.toString(8)+" "+f.Fm8(t);
	}

	public List<String> toStringList() {
		ArrayList<String> ret = new ArrayList<String>(7);
		ret.addAll(Point.mk(s).toStringList());
		ret.addAll(v.toStringList());
		ret.add(Double.toString(Units.to("s", t)));
		return ret;
	}
	
	public List<String> toStringList(int precision) {
		ArrayList<String> ret = new ArrayList<String>(7);
		ret.addAll(Point.mk(s).toStringList(precision));
		ret.addAll(v.toStringList(precision));
		ret.add(f.FmPrecision(Units.to("s", t),precision));
		return ret;
	}
}

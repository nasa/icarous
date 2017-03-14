/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;

/**
 * Polygon detection.
 * Based on UNVERIFIED PVS code.
 */
public class MovingPolygon2D {
  public ArrayList<Vect2> polystart = new ArrayList<Vect2>();
  public ArrayList<Vect2> polyvel = new ArrayList<Vect2>();
  public double tend;                                                 // duration of this part of a polypath
  
  boolean knownStable = false;
  boolean knownUnstable = false;
  
  public static class BadPolygonException extends Exception {
    private static final long serialVersionUID = 1L;
    BadPolygonException() {
      super();
    }
    public BadPolygonException(String s) {
      super(s);
    }
  }
  
  public MovingPolygon2D() {
    
  }

  public MovingPolygon2D(ArrayList<Vect2> polystart, ArrayList<Vect2> polyvel, double tend) {
    super();
    this.polystart = polystart;
    this.polyvel = polyvel;
    this.tend = tend;
  }
  
  
  public MovingPolygon2D(Poly2D polygon, Vect2 polyvel, double tend) {
      super();
      for (Vect2 v: polygon.getVertices()) {
          this.polystart.add(v);
          this.polyvel.add(polyvel);              // same velocity vector for all vertices
      } 
      this.tend = tend;
 }

  public MovingPolygon2D(Poly2D polygon, ArrayList<Vect2> polyvel, double tend) {
      super();
      for (Vect2 v: polygon.getVertices()) {
          this.polystart.add(v);
      } 
      this.polyvel = polyvel;
      this.tend = tend;
 }
  
  public MovingPolygon2D copy() {
	  ArrayList<Vect2> ps = new ArrayList<Vect2>(polystart.size());
	  ArrayList<Vect2> pv = new ArrayList<Vect2>(polyvel.size());
	  for (int i = 0; i < polystart.size(); i++) {
		  ps.add(polystart.get(i));
		  pv.add(polyvel.get(i));
	  }
	  return new MovingPolygon2D(ps,pv,tend);
  }

  public Poly2D position(double t) {
	  if (t <= 0.0) return new Poly2D(polystart);
	  ArrayList<Vect2> vs = new ArrayList<Vect2>();
	  for (int i = 0; i < polystart.size(); i++) {
		  vs.add(polystart.get(i).AddScal(t, polyvel.get(i)));
	  }
	  return new Poly2D(vs);
  }

  /** return true if this polygon is "stable", i.e., all vector velocities are the same 
   * @return true, if stable
   * */
  public boolean isStable() {
	  if (polyvel.size() < 2) return true;
	  if (knownStable) return true;
	  if (knownUnstable) return false;
	  Vect2 base = polyvel.get(0);
	  for (int i = 1; i < polyvel.size(); ++i) {
		  if (!polyvel.get(i).almostEquals(base)) {
			  knownUnstable = true;
			  return false;
		  }
	  }
	  knownStable = true;
	  return true;
  }
  
  public int size() {
    return polystart.size();
  }
  
  /**
   * Create a MovingPolygon2D that is rotated and translated such that the ownship so is at the origin and moving directly north.
   * @param so ownship position
   * @param vo ownship velocity
   * @return Translated/rotated version of this polygon 
   */
  public MovingPolygon2D relative(Vect2 so, Vect2 vo) {
	  ArrayList<Vect2> p = new ArrayList<Vect2>();
	  ArrayList<Vect2> v = new ArrayList<Vect2>();
	  Vect2 vx = vo.PerpR().Hat();
	  Vect2 vy = vo.Hat();
	  for (int i = 0; i < polystart.size(); i++) {
		  Vect2 pi = new Vect2(vx.dot(polystart.get(i).Sub(so)), vy.dot(polystart.get(i).Sub(so)));
		  Vect2 vi = new Vect2(vx.dot(polyvel.get(i)), vy.dot(polyvel.get(i)));
		  p.add(pi);
		  v.add(vi);
	  }
	  return new MovingPolygon2D(p,v,tend);
  }
  
	/**
	 * Reverse order of vertices
	 * @return new polygon
	 */
	public MovingPolygon2D reverseOrder() {
		  ArrayList<Vect2> p = new ArrayList<Vect2>();
		  ArrayList<Vect2> v = new ArrayList<Vect2>();
		for (int i = size()-1; i >= 0; i--) {
			p.add(polystart.get(i));
			v.add(polyvel.get(i));
		}
		return new MovingPolygon2D(p,v,tend);
	}

  
  public String toStringOLD() {
	  return "vertices="+f.Fobj(polystart)+" vels="+f.Fobj(polyvel)+" tend="+tend;
  } 

  public String toString() {
	  String rtn = "vertices = [";
	  for (Vect2 s: polystart) {
	       rtn = rtn + s.toString();
	  }
	  rtn = rtn +"]\n                     vels= ";
	  for (Vect2 v: polyvel) {
	       rtn = rtn + v.toString();
	  }
	  rtn = rtn + " tend = "+tend;
	  return rtn;
  }

}

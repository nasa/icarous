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
public class MovingPolygon3D {
    public MovingPolygon2D horizpoly = new MovingPolygon2D();
    public double vspeed;
    public double minalt;
    public double maxalt;
//    private boolean morphing;

    public MovingPolygon3D() {
    	vspeed = 0;
    	minalt = 10000;
    	maxalt = 10000;
    }

    public MovingPolygon3D(MovingPolygon2D horizpoly, double vspeed, double minalt, double maxalt) {
      super();
      this.horizpoly = horizpoly;
      this.vspeed = vspeed;
      this.minalt = minalt;
      this.maxalt = maxalt;
//      morphing = false;
    }
    
    public MovingPolygon3D(Poly3D p, Velocity v, double end) {
      vspeed = v.vs();
      minalt = p.getBottom();
      maxalt = p.getTop();
      ArrayList<Vect2> ps = new ArrayList<Vect2>(p.size());
      ArrayList<Vect2> vs = new ArrayList<Vect2>(p.size());
      for (int i = 0; i < p.size(); i++) {
        ps.add(p.getVertex(i));
        vs.add(v.vect2());
//f.pln("MovingPolygon3D "+i+" "+ps.get(i).toString()+" "+vs.get(i).toString());
      }
      horizpoly = new MovingPolygon2D(ps,vs,end);
//      morphing = false;
    }

    public MovingPolygon3D(Poly3D p, ArrayList<Velocity> v, double end) {
        vspeed = v.get(0).vs();
        minalt = p.getBottom();
        maxalt = p.getTop();
        ArrayList<Vect2> ps = new ArrayList<Vect2>(p.size());
        ArrayList<Vect2> vs = new ArrayList<Vect2>(p.size());
        for (int i = 0; i < p.size(); i++) {
          ps.add(p.getVertex(i));
          vs.add(v.get(i).vect2());
  //f.pln("MovingPolygon3D "+i+" "+ps.get(i).toString()+" "+vs.get(i).toString());
        }
        horizpoly = new MovingPolygon2D(ps,vs,end);
//        morphing = true;
      }
    
    public MovingPolygon3D copy() {
    	return new MovingPolygon3D(horizpoly.copy(), vspeed, minalt, maxalt);
    }
    
    public Poly3D position(double t) {
    	Poly2D p2d = horizpoly.position(t);
    	return new Poly3D(p2d, minalt+vspeed*t, maxalt+vspeed*t);
    }
    
	/**
	 * Return the average Velocity (at time 0).
	 * 
	 * @return velocity
	 */
	public Velocity averageVelocity() {
		Vect2 v = Vect2.ZERO;
		for (int i = 0; i < size(); i++) {
			v = v.Add(horizpoly.polyvel.get(i));
		}
		return Velocity.make(new Vect3(v.Scal(1.0/size()),vspeed));
	}

    
    /**
     * Initial velocity for vertex i
     * @param i index
     * @return velocity
     */
    public Velocity velocity(int i) {
    	return Velocity.make(horizpoly.polyvel.get(i)).mkVs(vspeed);
    }
    
    public MovingPolygon3D linear(double t) {
    	ArrayList<Velocity> vlist = new ArrayList<Velocity>(size());
    	for (int i = 0; i < size(); i++) {
    		vlist.add(velocity(i));
    	}
    	return new MovingPolygon3D(position(t), vlist, horizpoly.tend+t);
    }
    
//    public boolean isMorphing() {
//    	return morphing;
//    }
    
    public MovingPolygon2D movingPolygon2D(double t) {
    	return new MovingPolygon2D(horizpoly.position(t),horizpoly.polyvel, horizpoly.tend);
    }
    
	public MovingPolygon3D reverseOrder() {
		return new MovingPolygon3D(horizpoly.reverseOrder(), vspeed, minalt, maxalt);
	}
    
    public double getTimeHorizon() {
    	return horizpoly.tend;
    }

    public void setTimeHorizon(double val) {
    	horizpoly.tend = val;
    }
    
    public boolean isStable() {
    	return horizpoly.isStable();
    }
    
    public int size() {
    	return horizpoly.size();
    }
    
     public String toString() {
//       	return horizpoly.toString()+" vspd="+Units.str("fpm",vspeed)+" minalt="+Units.str("ft",minalt)+" maxalt="+Units.str("ft",maxalt);
       	return horizpoly.toString()+" vspd="+vspeed+" minalt="+minalt+" maxalt="+maxalt;
    }
    
    public String toOutput(String id) {
    	
    	Poly3D p1 = position(0.0);
    	Poly3D p2 = position(horizpoly.tend);
    	String s1 = "";
    	String s2 = "";
    	for (int i = 0; i < horizpoly.size(); i++) {
    		s1 += id+","+p1.getVertex(i).toStringNP("NM", "NM", 8)+","+Units.to("ft", minalt)+","+Units.to("ft",maxalt)+",0.0\n";
    		s2 += id+","+p2.getVertex(i).toStringNP("NM", "NM", 8)+","+Units.to("ft", minalt)+","+Units.to("ft",maxalt)+","+horizpoly.tend+"\n";
    	}
    	return s1+s2;
    }
}

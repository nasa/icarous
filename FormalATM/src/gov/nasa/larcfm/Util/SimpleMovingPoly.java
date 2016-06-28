/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;


/**
 * A "stateful" version of a SimplePoly that includes velocity.
 */
public class SimpleMovingPoly {
	SimplePoly poly;
	List<Velocity> vlist;
	boolean morphingPoly = true;
	
	public SimpleMovingPoly(SimplePoly start, List<Velocity> vs) {
		poly = start;
		vlist = vs;
		while (vlist.size() < start.size()) {
			vlist.add(Velocity.ZERO);
		}
	}

	public SimpleMovingPoly(SimplePoly start, Velocity v) {
		poly = start;
		vlist = new ArrayList<Velocity>();
		for (int i = 0; i < start.size(); i++) {
			vlist.add(v);
		}
		morphingPoly = false;
	}

	public SimpleMovingPoly() {
		poly = new SimplePoly();
		vlist = new ArrayList<Velocity>();
	}

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top Altitude
	 */
	public SimpleMovingPoly(double b, double t) {
		poly = new SimplePoly(b,t);
		vlist = new ArrayList<Velocity>();
	}
	
	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top Altitude
	 */
	public SimpleMovingPoly(double b, double t, String units) {
		poly = new SimplePoly(b,t,units);
		vlist = new ArrayList<Velocity>();
	}


	/**
	 * Create a deep copy of a SimplePoly
	 * 
	 * @param p Source poly.
	 */
	public SimpleMovingPoly(SimpleMovingPoly p) {
		poly = new SimplePoly(p.poly);
		vlist = new ArrayList<Velocity>();
		for(int i = 0; i < p.size(); i++) {
			vlist.add(p.vlist.get(i));
		}
	}
	
	public SimpleMovingPoly copy() {
		return new SimpleMovingPoly(this);
	}
	
	public boolean addVertex(Position p, Velocity v) {
		boolean ret = poly.addVertex(p);
		if (ret) {
			vlist.add(v);
		}
		return ret;
	}

	public void removeVertex(int i) {
		if (i >= 0 && i < poly.size()) {
			poly.remove(i);
			vlist.remove(i);
		}
	}

	public void setTop(double top) {
		poly.setTop(top);
	}

	public void setBottom(double bot) {
		poly.setBottom(bot);
	}

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates.
	 */
	public static SimpleMovingPoly make(MovingPolygon3D p3, EuclideanProjection proj) {
		Poly3D base = p3.position(0);
		if (p3.isMorphing()) {
			ArrayList<Velocity> vs = new ArrayList<Velocity>();
			for(int i = 0; i < p3.size(); i++) {
				vs.add(proj.inverseVelocity(new Vect3(base.getVertex(i),0), Velocity.make(new Vect3(p3.horizpoly.polyvel.get(i),p3.vspeed)), true));
			}
			return new SimpleMovingPoly(SimplePoly.make(base,proj), vs);
		} else {
			return new SimpleMovingPoly(SimplePoly.make(base,proj), proj.inverseVelocity(base.averagePoint(), Velocity.make(new Vect3(p3.horizpoly.polyvel.get(0),p3.vspeed)), true));
		}
	}
	
	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
	 */
	public static SimpleMovingPoly make(MovingPolygon3D p3) {
		SimpleMovingPoly sp = new SimpleMovingPoly();
		sp.poly = SimplePoly.make(p3.position(0));
		for(int i = 0; i < p3.size(); i++) {
			Velocity v = Velocity.make(new Vect3(p3.horizpoly.polyvel.get(i),p3.vspeed));
			sp.vlist.add(v);
		}
		return sp;
	}

	public boolean isLatLon() {
		return poly.isLatLon();
	}
	
	/**
	 * Return the polygon projected to be at time dt (dt = 0 returns a copy of the base polygon)
	 */
	public SimplePoly position(double dt) {
		if (dt == 0.0) {
			return poly.copy();
		}
		  SimplePoly newPoly = new SimplePoly();
		  int sz = poly.size();
		  if (morphingPoly) {
			  for (int j = 0; j < sz; j++) {
				  Position n1 = poly.getVertex(j);
				  Position n1t = poly.getTopPoint(j);
				  Position p = n1.linear(vlist.get(j),dt);
				  Position pt = n1t.linear(vlist.get(j),dt);
				  newPoly.addVertex(p);
				  newPoly.setBottom(p.z());
				  newPoly.setTop(pt.z());
		  		}
		  } else {
			  Velocity v = vlist.get(0);
			  for (int j = 0; j < sz; j++) {
				  Position n1 = poly.getVertex(j);
				  Position n1t = poly.getTopPoint(j);
				  Position p = n1.linear(v,dt);
				  Position pt = n1t.linear(v,dt);
				  newPoly.addVertex(p);
				  newPoly.setBottom(p.z());
				  newPoly.setTop(pt.z());
		  		}
		  }
		  return newPoly;
	}
	
	/**
	 * Return the average Velocity (at time 0).
	 */
	public Velocity averageVelocity() {
		if (morphingPoly) {
			Velocity v = Velocity.ZERO;
			for (int i = 0; i < vlist.size(); i++) {
				v = v.Add(vlist.get(i));
			}
			return Velocity.make(v.Scal(1.0/vlist.size()));
		} else {
			return vlist.get(0);
		}
	}

	public SimpleMovingPoly linear(double dt) {
		  SimplePoly newPoly = new SimplePoly();
		  int sz = poly.size();
		  if (morphingPoly) {
			  for (int j = 0; j < sz; j++) {
				  Position n1 = poly.getVertex(j);
				  Position n1t = poly.getTopPoint(j);
				  Position p = n1.linear(vlist.get(j),dt);
				  Position pt = n1t.linear(vlist.get(j),dt);
				  newPoly.addVertex(p);
				  newPoly.setBottom(p.z());
				  newPoly.setTop(pt.z());
		  		}
		  } else {
			  Velocity v = vlist.get(0);
			  for (int j = 0; j < sz; j++) {
				  Position n1 = poly.getVertex(j);
				  Position n1t = poly.getTopPoint(j);
				  Position p = n1.linear(v,dt);
				  Position pt = n1t.linear(v,dt);
				  newPoly.addVertex(p);
				  newPoly.setBottom(p.z());
				  newPoly.setTop(pt.z());
		  		}
		  }
		  return new SimpleMovingPoly(newPoly,vlist);
	}


	  /**
	   * This will return a moving polygon that starts at point i and ends at point i+1
	   * @param i
	   * @param proj
	   * @return
	   */
	  public MovingPolygon3D getMovingPolygon(double time, EuclideanProjection proj) {
		  Poly3D p3d = position(time).poly3D(proj);
		  if (morphingPoly) {
			  ArrayList<Velocity> vs = new ArrayList<Velocity>();
			  for (int i = 0; i < p3d.size(); i++) {
				  if (isLatLon()) {
					  vs.add(proj.projectVelocity(poly.getVertex(i), vlist.get(i)));
				  } else {
					  vs.add(vlist.get(i));
				  }
			  }
			  return new MovingPolygon3D(p3d,vs,1000000.0);
		  } else {
			  if (isLatLon()) {
				  return new MovingPolygon3D(p3d,proj.projectVelocity(poly.averagePoint(), vlist.get(0)),1000000.0);
			  } else {
				  return new MovingPolygon3D(p3d,vlist.get(0),1000000.0);
			  }
		  }
	  }

	  public MovingPolygon3D getMovingPolygon(EuclideanProjection proj) {
		  return getMovingPolygon(0.0,proj);
	  }

	  
	int size() {
		return poly.size();
	}
	
	/**
	 * Return true if point p is within the polygon at time dt from now.
	 */
	boolean contains(Position p, double dt) {
		return dt >= 0 && position(dt).contains(p);
	}
	
	/**
	 * Create a list of strings describing one vertex: lat, lon, alt, alt2, trk, gs, vs (or sx, sy, sz)
	 * @param vertex vertex number
	 * @param trkgsvs true use track/Gs/Vs, false use Euclidean velocity vector (sx, sy, sz)
	 * @param precision number of fractional digits in fields
	 * @return
	 */
	  public List<String> toStringList(int vertex, boolean trkgsvs, int precision) {
		    ArrayList<String> ret = new ArrayList<String>(7);
		    Velocity v = vlist.get(vertex);
		    ret.addAll(poly.toStringList(vertex,  precision));
		    if (trkgsvs) {
		    	ret.addAll(v.toStringList(precision));
		    } else {
		    	ret.addAll(v.toStringXYZList(precision));
		    }
		    return ret;
		  }


}

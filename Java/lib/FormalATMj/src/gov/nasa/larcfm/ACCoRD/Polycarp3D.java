/* 
 * Polycarp3D - containment and conflict detection for 3D polygons
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 * 
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.Collections;

import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.IntervalSet;
import gov.nasa.larcfm.Util.MovingPolygon3D;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.Poly3D;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

public class Polycarp3D {

	/**
	 * Set to true if a polygon niceness check is needed.  If true, this also allows for both clockwise and counterclockwise polygons. 
	 */
	public static boolean checkNice = true;

	public static boolean nearEdge(Vect3 so, Poly3D p, double h, double v) {
		// well above or below polygon
		if (so.z() < p.getBottom()-v || so.z() > p.getTop()+v) {
			return false;
		}
		ArrayList<Vect2> ps = new ArrayList<Vect2>(p.size());
		for (int i = 0; i < p.size(); i++) {
			ps.add(p.poly2D().getVertex(i));
		}
		if (checkNice && !PolycarpContain.nice_polygon_2D(ps, h)) {			
			Collections.reverse(ps);
			if (!PolycarpContain.nice_polygon_2D(ps, h)) {
				f.pln("WARNING: Polycarp3D.violation: NOT A NICE POLYGON!");
				return false;				
			}
		}
		// near a 2D edge
		if (PolycarpContain.near_any_edge(ps,so.vect2(),h)) {
			return true;
		}
		// within polygon and near top or bottom
		if (PolycarpContain.definitely_inside(ps, so.vect2(), h) && (so.z() < p.getBottom()+v || so.z() > p.getTop()-v)) {
			return true;
		}
		// not near any edge
		return false;
	}

	public static boolean definitely_inside(Vect3 so, Poly3D p, double buff) {
		if (so.z() < p.getBottom()+buff || so.z() > p.getTop()-buff) {
			return false;
		}
		ArrayList<Vect2> ps = new ArrayList<Vect2>(p.size());
		for (int i = 0; i < p.size(); i++) {
			ps.add(p.poly2D().getVertex(i));
		}
		if (checkNice && !PolycarpContain.nice_polygon_2D(ps, buff)) {			
			Collections.reverse(ps);
			if (!PolycarpContain.nice_polygon_2D(ps, buff)) {
				f.pln("WARNING: Polycarp3D.violation: NOT A NICE POLYGON!");
				return false;				
			}
		}
		return PolycarpContain.definitely_inside(ps, so.vect2(), buff);
	}
	
	public static boolean violation(Vect3 so, Poly3D p, double buff) {
		if (so.z() < p.getBottom() || so.z() > p.getTop()) {
			return false;
		}
		ArrayList<Vect2> ps = new ArrayList<Vect2>(p.size());
		for (int i = 0; i < p.size(); i++) {
			ps.add(p.poly2D().getVertex(i));
		}
		if (checkNice && !PolycarpContain.nice_polygon_2D(ps, buff)) {			
			Collections.reverse(ps);
			if (!PolycarpContain.nice_polygon_2D(ps, buff)) {
				f.pln("WARNING: Polycarp3D.violation: NOT A NICE POLYGON!");
				return false;				
			}
		}
		return PolycarpContain.definitely_inside(ps, so.vect2(), buff) || !PolycarpContain.definitely_outside(ps, so.vect2(), buff);
	}


	public static boolean detection(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, double buff, double fac) {
		double sz = so.z() - (mp.maxalt+mp.minalt)/2.0;
		double vz = vo.z() - mp.vspeed;
		double b = B;
		double t = T;
		if (vz != 0) {
			double h = (mp.maxalt-mp.minalt)/2.0;
			double vb = Vertical.Theta_H(sz, vz, -1, h);
			double vt = Vertical.Theta_H(sz, vz, +1, h);
			b = Math.max(B, vb);
			t = Math.min(T, vt);
		} else if (so.z() < mp.minalt || so.z() > mp.maxalt) {
			return false; 
		}
		if (t < b) {
			return false; // no vertical conflict possible
		}
		MovingPolygon3D mp2 = mp;
		if (checkNice && !PolycarpContain.nice_polygon_2D(mp.horizpoly.polystart, buff)) {
			mp2 = mp.reverseOrder();
			if (!PolycarpContain.nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
				f.pln("WARNING: Polycarp3D.detection: NOT A NICE POLYGON!");
				return false;
			}
		}

		return PolycarpDetection.Collision_Detector(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac);
	}

	// NEW STUFF

	/**
	 * Return conflict detection times in and out 
	 * @param so
	 * @param vo
	 * @param mp
	 * @param B
	 * @param T
	 * @param buff
	 * @param fac
	 * @return IntervalSet of times in violation   
	 */
	public static IntervalSet conflictTimes(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, double buff, double fac) {
		double sz = so.z() - (mp.maxalt+mp.minalt)/2.0;
		double vz = vo.z() - mp.vspeed;
		double b = B;
		double t = T;
		IntervalSet ret = new IntervalSet();
		if (vz != 0) {
			double h = (mp.maxalt-mp.minalt)/2.0;
			double vb = Vertical.Theta_H(sz, vz, -1, h);
			double vt = Vertical.Theta_H(sz, vz, +1, h);
			b = Math.max(B, vb);
			t = Math.min(T, vt);
		} else if (so.z() < mp.minalt || so.z() > mp.maxalt) {
			return ret; 
		}
		if (t < b) {
			return ret; // no vertical conflict possible
		}
		MovingPolygon3D mp2 = mp;
		if (checkNice && !PolycarpContain.nice_polygon_2D(mp.horizpoly.polystart, buff)) {
			mp2 = mp.reverseOrder();
			if (!PolycarpContain.nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
				f.pln("WARNING: Polycarp3D.conflictTimes: NOT A NICE POLYGON!");
				return ret;
			}
		}

		ArrayList<Double> times = PolycarpDetection.collisionTimesInOut(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac);
		for (int i = 0; i < times.size()-1; i++) {
			double t1 = times.get(i);
			double t2 = times.get(i+1);
			double mid = (t1+t2)/2;
			boolean inside = violation(so.AddScal(mid, vo), mp2.position(mid), buff);
			if (inside) {
				ret.union(new Interval(t1, t2));
			}
		}
		// note: because B,T is returned as a conflict time, we don't need to explicitly check for always inside
		return ret;
	}
}

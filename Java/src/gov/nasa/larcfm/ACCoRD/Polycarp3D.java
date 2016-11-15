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
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

public class Polycarp3D {

	public static boolean nearEdge(Vect3 so, Poly3D p, double h, double v, boolean checkNice) {
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

	public static boolean definitely_inside(Vect3 so, Poly3D p, double buff, boolean checkNice) {
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
				f.pln("WARNING: Polycarp3D.definitely_inside: NOT A NICE POLYGON!");
				return false;				
			}
		}
		return PolycarpContain.definitely_inside(ps, so.vect2(), buff);
	}

	public static boolean definitely_outside(Vect3 so, Poly3D p, double buff, boolean checkNice) {
		if (so.z() < p.getBottom()-buff || so.z() > p.getTop()+buff) {
			return true;
		}
		ArrayList<Vect2> ps = new ArrayList<Vect2>(p.size());
		for (int i = 0; i < p.size(); i++) {
			ps.add(p.poly2D().getVertex(i));
		}
		if (checkNice && !PolycarpContain.nice_polygon_2D(ps, buff)) {			
			Collections.reverse(ps);
			if (!PolycarpContain.nice_polygon_2D(ps, buff)) {
				f.pln("WARNING: Polycarp3D.definitely_outside: NOT A NICE POLYGON!");
				return false;				
			}
		}
		return PolycarpContain.definitely_outside(ps, so.vect2(), buff);
	}


	// return true if starting condition is bad or if we cross a boundary
	public static boolean entranceDetection(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, double buff, double fac, boolean checkNice) {
		double sz = so.z() - (mp.maxalt+mp.minalt)/2.0;
		double vz = vo.z() - mp.vspeed;
		// times cannot be too big!  limit to 100 days for now
		double b = Math.max(0.0, Math.min(B, Units.from("day",100.0)));
		double t = Math.max(0.0, Math.min(T, Units.from("day",100.0)));
		if (vz != 0) {
			double h = (mp.maxalt-mp.minalt)/2.0;
			double vb = Vertical.Theta_H(sz, vz, -1, h);
			double vt = Vertical.Theta_H(sz, vz, +1, h);
			b = Math.max(B, vb);
			t = Math.min(T, vt);
		} else if (so.z() < mp.minalt || so.z() > mp.maxalt) {
			//f.pln("entranceDetection fail z1");
			return false; //return false; // so never enters the polygon's vertical slice
		}
		if (t < b) {
			//f.pln("entranceDetection fail z2");
			return false; //return false; // so never enters the polygon's vertical slice
		}

		MovingPolygon3D mp2 = mp;
		if (checkNice && !PolycarpContain.nice_polygon_2D(mp.horizpoly.polystart, buff)) {
			mp2 = mp.reverseOrder();
			if (!PolycarpContain.nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
				f.pln("WARNING: Polycarp3D.detection: NOT A NICE POLYGON!");
				return false;
			}
			if (!mp2.isStable() && !PolycarpDetection.nice_moving_polygon_2D(b, t, mp2.horizpoly, buff, fac)) {
				f.pln("WARNING: Polycarp3D.detection: NOT A NICE MOVING POLYGON!");
				return false;
			}
		}

		if (mp2.isStable()) {
			Vect2 pv =  mp2.velocity(0).vect2();
			return PolycarpDetection.Static_Collision_Detector(b, t, mp2.horizpoly.polystart, pv, so.vect2(), vo.vect2(), buff, true);
		} else {
			return PolycarpDetection.Collision_Detector(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac, true);
		}
	}

	public static boolean exitDetection(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, double buff, double fac, boolean checkNice) {
		double sz = so.z() - (mp.maxalt+mp.minalt)/2.0;
		double vz = vo.z() - mp.vspeed;
		// times cannot be too big!  limit to 100 days for now
		double b = Math.max(0.0, Math.min(B, Units.from("day",100.0)));
		double t = Math.max(0.0, Math.min(T, Units.from("day",100.0)));
		if (vz != 0) {
			double h = (mp.maxalt-mp.minalt)/2.0;
			double vb = Vertical.Theta_H(sz, vz, -1, h);
			double vt = Vertical.Theta_H(sz, vz, +1, h);
			b = Math.max(B, vb);
			t = Math.min(T, vt);
		} else if (so.z() < mp.minalt || so.z() > mp.maxalt) {
			return true; //return false; // so never enters the polygon's vertical slice
		}
		if (t < b) {
			return true; //return false; // so never enters the polygon's vertical slice
		}

		MovingPolygon3D mp2 = mp;
		if (checkNice && !PolycarpContain.nice_polygon_2D(mp.horizpoly.polystart, buff)) {
			mp2 = mp.reverseOrder();
			if (!PolycarpContain.nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
				f.pln("WARNING: Polycarp3D.detection: NOT A NICE POLYGON!");
				return false;
			}
			if (!mp2.isStable() && !PolycarpDetection.nice_moving_polygon_2D(b, t, mp2.horizpoly, buff, fac)) {
				f.pln("WARNING: Polycarp3D.detection: NOT A NICE MOVING POLYGON!");
				return false;
			}
		}

		if (mp2.isStable()) {
			Vect2 pv =  mp2.velocity(0).vect2();
			return PolycarpDetection.Static_Collision_Detector(b, t, mp2.horizpoly.polystart, pv, so.vect2(), vo.vect2(), buff, false);
		} else {
			return PolycarpDetection.Collision_Detector(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac, false);
		}
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
	 * @param insideBad true if trying to avoid polygon, false if trying to be contained by polygon
	 * @param checkNice true to explicitly check for niceness of polygons
	 * @return IntervalSet of times in violation   
	 */
	public static IntervalSet conflictTimes(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, double buff, double fac, boolean insideBad, boolean checkNice) {
		double sz = so.z() - (mp.maxalt+mp.minalt)/2.0;
		double vz = vo.z() - mp.vspeed;
		// times cannot be too big!  limit to 100 days for now
		double b = Math.max(0.0, Math.min(B, Units.from("day",100.0)));
		double t = Math.max(0.0, Math.min(T, Units.from("day",100.0)));
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
			if (!mp2.isStable() && !PolycarpDetection.nice_moving_polygon_2D(b, t, mp2.horizpoly, buff, fac)) {
				f.pln("WARNING: Polycarp3D.conflictTimes: NOT A NICE MOVING POLYGON!");
				return ret;
			}
		}

		ArrayList<Double> times = PolycarpDetection.collisionTimesInOut(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac);
		for (int i = 0; i < times.size()-1; i++) {
			double t1 = times.get(i);
			double t2 = times.get(i+1);
			double mid = (t1+t2)/2;
			boolean addInterval;
			if (insideBad) {
				addInterval = !definitely_outside(so.AddScal(mid, vo), mp2.position(mid), buff, false); // was already checked nice above
			} else {
				addInterval = !definitely_inside(so.AddScal(mid, vo), mp2.position(mid), buff, false); // was already checked nice above
			}
			if (addInterval) {
				ret.union(new Interval(t1, t2));
			}
		}
		// note: because B,T is returned as a conflict time, we don't need to explicitly check for always inside
		return ret;
	}
}

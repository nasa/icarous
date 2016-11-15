/* 
 * PolycarpResolution - closest point inside-outside a polygon to a given point - with a buffer
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

import gov.nasa.larcfm.Util.Vect2;

public class PolycarpResolution {

	public static Vect2 proj_vect(Vect2 u, Vect2 v, Vect2 w, double BUFF) {
		if (v.Sub(u).norm() <= BUFF || w.Sub(v).norm() <= BUFF) return new Vect2(1.0,1.0);
		if (v.Sub(u).dot(w.Sub(v)) >= 0) return v.Sub(u).PerpR().Hat().Add(w.Sub(v).PerpR().Hat());
		if (v.Sub(u).det(v.Sub(u)) <= 0) return v.Sub(u).Hat().Add(v.Sub(w).Hat());
		return u.Sub(v).Hat().Add(w.Sub(v).Hat());
	}

	public static ArrayList<Vect2> expand_polygon_2D(double BUFF, double ResolBUFF, ArrayList<Vect2> p) {
		ArrayList<Vect2> p2 = new ArrayList<Vect2>();
		for (int i = 0; i < p.size(); i++) {
			Vect2 prev = i > 0 ? p.get(i-1) : p.get(p.size()-1);
			Vect2 next = i < p.size()-1 ? p.get(i+1) : p.get(0);
			Vect2 pv = proj_vect(prev, p.get(i), next, BUFF);
			Vect2 R = p.get(i).Sub(prev).PerpR().Hat();
			Vect2 q = p.get(i).Add(R.Scal(ResolBUFF));
			double tt = (Math.abs(pv.dot(R)) <= BUFF/100) ? 0 : q.Sub(p.get(i)).dot(R)/pv.dot(R);
			p2.add(p.get(i).Add(pv.Scal(tt)));
		}
		return p2;
	}

	public static ArrayList<Vect2> contract_polygon_2D(double BUFF, double ResolBUFF, ArrayList<Vect2> p) {
		ArrayList<Vect2> p2 = new ArrayList<Vect2>();
		for (int i = 0; i < p.size(); i++) {
			Vect2 prev = i > 0 ? p.get(i-1) : p.get(p.size()-1);
			Vect2 next = i < p.size()-1 ? p.get(i+1) : p.get(0);
			Vect2 pv = proj_vect(prev, p.get(i), next, BUFF);
			Vect2 R = p.get(i).Sub(prev).PerpR().Hat();
			Vect2 q = p.get(i).Sub(R.Scal(ResolBUFF));
			double tt = (Math.abs(pv.dot(R)) <= BUFF/100) ? 0 : q.Sub(p.get(i)).dot(R)/pv.dot(R);
			p2.add(p.get(i).Add(pv.Scal(tt)));
		}
		return p2;
	}
	
	public static int closest_edge(ArrayList<Vect2> p, double BUFF, Vect2 s) {
		int ce = 0;
		for (int i = 0; i < p.size(); i++) {
			int next = i < p.size()-1 ? i+1 : 0;
			Vect2 closp = PolycarpEdgeProximity.closest_point(p.get(i), p.get(next), s, BUFF);
			double thisdist = s.Sub(closp).norm();
			int nextce = ce < p.size()-1 ? ce+1 : 0;
			Vect2 prevclosp = PolycarpEdgeProximity.closest_point(p.get(ce),  p.get(nextce),  s,  BUFF);
			double prevdist = s.Sub(prevclosp).norm();
			if (thisdist < prevdist) {
				ce = i;
			}
		}
		return ce;
	}
	
	public static Vect2 recovery_test_point(double BUFF, double ResolBUFF, ArrayList<Vect2>p, Vect2 s, int eps) {
		int i = closest_edge(p,BUFF,s);
		int nexti = i < p.size()-1 ? i+1 : 0;
		Vect2 ip = PolycarpEdgeProximity.closest_point(p.get(i), p.get(nexti), s, BUFF);
		Vect2 dirvect = p.get(nexti).Sub(p.get(i)).PerpR();
		Vect2 testdir = dirvect.Hat();
		Vect2 testvect = ip.Add(testdir.Scal(eps*(ResolBUFF+BUFF/2)));
		return testvect;
	}

	public static Vect2 recovery_point(double BUFF, double ResolBUFF, ArrayList<Vect2> p, Vect2 s, int eps) {
		Vect2 tv = recovery_test_point(BUFF,ResolBUFF,p,s,eps);
		if (eps == 1 && PolycarpContain.definitely_outside(p,s,BUFF) &&
				!(PolycarpContain.near_any_edge(p,s, ResolBUFF))) {
			//System.out.println("hit 1");
			return s;
		}
		if (eps == -1 && PolycarpContain.definitely_inside(p,s, BUFF) &&
				!(PolycarpContain.near_any_edge(p,s, ResolBUFF))) {
			//System.out.println("hit 2");
			return s;
		}
		if (eps == 1 && PolycarpContain.definitely_outside(p,tv,BUFF) &&
				!(PolycarpContain.near_any_edge(p, tv, ResolBUFF))) {
			//System.out.println("hit 3");
			return tv;
		}
		if (eps == -1 && PolycarpContain.definitely_inside(p, tv, BUFF) &&
				!(PolycarpContain.near_any_edge(p, tv, ResolBUFF))) {
			//System.out.println("hit 4");
			return tv;
		}
		int i = closest_edge(p,BUFF,s);
		int nexti = i < p.size()-1 ? i+1 : 0;
		int neari = s.Sub(p.get(i)).sqv() <= s.Sub(p.get(nexti)).sqv() ? i : nexti;
		int nearnexti = neari < p.size()-1 ? neari+1 : 0;
		int nearprevi = neari > 0 ? neari-1 : p.size()-1;
		Vect2 V1 = p.get(neari).Sub(p.get(nearprevi));
		Vect2 V2 = p.get(nearnexti).Sub(p.get(neari));
		boolean leftturn = (V2.det(V1) <= 0);
		Vect2 pv = proj_vect(p.get(nearprevi), p.get(neari), p.get(nearnexti),BUFF);
		Vect2 pvnormed = pv.Hat();
		Vect2 R = p.get(neari).Sub(p.get(nearprevi)).PerpR().Hat();
		Vect2 q = p.get(neari).Add(R.Scal(eps*ResolBUFF));
		double tt = Math.abs(pv.dot(R)) <= BUFF/100 ? 0 : q.Sub(p.get(i)).dot(R)/pv.dot(R);
		Vect2 ans = p.get(neari).Add(pv.Scal(tt));
		if ((eps == 1 && leftturn) || eps == -1 && !leftturn) {
			ans = p.get(neari).Add(pvnormed.Scal((eps*ResolBUFF)));
		}
		if (eps == 1 && PolycarpContain.definitely_outside(p,ans,BUFF)) return ans;
		if (eps == -1 && PolycarpContain.definitely_inside(p,ans,BUFF)) return ans;
		return s;
	}
	
	public static Vect2 outside_recovery_point(double BUFF, double ResolBUFF, ArrayList<Vect2> p, Vect2 s) {
		return recovery_point(BUFF,ResolBUFF,p,s,1);
	}
	
	public static Vect2 inside_recovery_point(double BUFF, double ResolBUFF, ArrayList<Vect2> p, Vect2 s) {
		return recovery_point(BUFF,ResolBUFF,p,s,-1);
	}
		

}

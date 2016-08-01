/* 
 * PolycarpDetection - collision detection between and point and a 2D polygon
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
import gov.nasa.larcfm.Util.MovingPolygon2D;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.f;
import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.ae;
//import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.discr;
//import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.root;
import static gov.nasa.larcfm.ACCoRD.PolycarpContain.definitely_outside;

public class PolycarpDetection {

	public static Interval dot_nneg_linear_2D_alg(double T, Vect2 w, Vect2 v, Vect2 a, Vect2 b, int eps) {
		double aa = v.dot(b);
		double bb = w.dot(b) + a.dot(v);
		double cc = w.dot(a);
		Interval in;
		if (!ae(aa,0)) {
			if (Util.discr(aa,bb,cc)>=0) {
				if (aa<0) {
					in = new Interval (Util.root(aa,bb,cc,1),Util.root(aa,bb,cc,-1));
				} else if (eps==-1) {
					in = new Interval(0,Util.root(aa,bb,cc,-1));
				} else {
					in = new Interval(Util.root(aa,bb,cc,1),T);
				}
			} else if (aa>0) {
				in = new Interval(0,T);
			} else {
				in = new Interval(T,0);
			}
		} else if (ae(bb,0) && cc>=0) {
			in = new Interval(0,T);
		} else if (ae(bb,0)) {
			in = new Interval(T,0);
		} else if (bb>0) {
			in = new Interval(-cc/bb,T);
		} else {
			in = new Interval(0,-cc/bb);
		}
		if (in.up<in.low || in.up<0 || in.low>T) {
			return new Interval(T,0);
		} else {
			return new Interval(Math.max(in.low,0),Math.min(in.up,T));
		}
	}

	public static Interval dot_nneg_spec(double T, Vect2 w, Vect2 v, Vect2 a, Vect2 b, int eps, int eps2, double Fac) {
		Vect2 ww = a.Scal(Fac).Add(w.Scal(eps2));
		Vect2 vv = b.Scal(Fac).Add(v.Scal(eps2));
		return dot_nneg_linear_2D_alg(T,ww,vv,a,b,eps);
	}




	public static boolean edge_detect_simple(double T, Vect2 w, Vect2 v, Vect2 a, Vect2 b, double Fac) { //Fac close to 0
		Interval Vn = dot_nneg_spec(T,w,v,a,b,-1,-1,1+Fac); //[Vn.low,Vn.up]
				Interval V = dot_nneg_spec(T,w,v,a,b,-1,1,1+Fac); //[V.low,V.up] 
				Interval Vnx = dot_nneg_spec(T,w,v,a,b,1,-1,1+Fac); //[Vnx.low,Vnx.up]
				Interval Vx = dot_nneg_spec(T,w,v,a,b,1,1,1+Fac); //[Vx.low,Vx.up]
				Interval Pn = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),-1,-1,Fac); //[Pn.low,Pn.up]
				Interval P = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),-1,1,Fac); //[P.low,P.up]
				Interval Pnx = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),1,-1,Fac); //[Pnx.low,Pnx.up]
				Interval Px = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),1,1,Fac); //[Px.low,Px.up]
				double Vlb1 = Math.max(V.low,Vn.low);
				double Vub1 = Math.min(V.up,Vn.up);
				double Vlb2 = Math.max(V.low,Vnx.low);
				double Vub2 = Math.min(V.up,Vnx.up);
				double Vlb3 = Math.max(Vx.low,Vn.low);
				double Vub3 = Math.min(Vx.up,Vn.up);
				double Vlb4 = Math.max(Vx.low,Vnx.low);
				double Vub4 = Math.min(Vx.up,Vnx.up);
				double Plb1 = Math.max(P.low,Pn.low);
				double Pub1 = Math.min(P.up,Pn.up);
				double Plb2 = Math.max(P.low,Pnx.low);
				double Pub2 = Math.min(P.up,Pnx.up);
				double Plb3 = Math.max(Px.low,Pn.low);
				double Pub3 = Math.min(Px.up,Pn.up);
				double Plb4 = Math.max(Px.low,Pnx.low);
				double Pub4 = Math.min(Px.up,Pnx.up);

				if (Math.max(Vlb1,Plb1)<=Math.min(Vub1,Pub1)) return true;
				if (Math.max(Vlb1,Plb2)<=Math.min(Vub1,Pub2)) return true;
				if (Math.max(Vlb1,Plb3)<=Math.min(Vub1,Pub3)) return true;
				if (Math.max(Vlb1,Plb4)<=Math.min(Vub1,Pub4)) return true;
				if (Math.max(Vlb2,Plb1)<=Math.min(Vub2,Pub1)) return true;
				if (Math.max(Vlb2,Plb2)<=Math.min(Vub2,Pub2)) return true;
				if (Math.max(Vlb2,Plb3)<=Math.min(Vub2,Pub3)) return true;
				if (Math.max(Vlb2,Plb4)<=Math.min(Vub2,Pub4)) return true;
				if (Math.max(Vlb3,Plb1)<=Math.min(Vub3,Pub1)) return true;
				if (Math.max(Vlb3,Plb2)<=Math.min(Vub3,Pub2)) return true;
				if (Math.max(Vlb3,Plb3)<=Math.min(Vub3,Pub3)) return true;
				if (Math.max(Vlb3,Plb4)<=Math.min(Vub3,Pub4)) return true;
				if (Math.max(Vlb4,Plb1)<=Math.min(Vub4,Pub1)) return true;
				if (Math.max(Vlb4,Plb2)<=Math.min(Vub4,Pub2)) return true;
				if (Math.max(Vlb4,Plb3)<=Math.min(Vub4,Pub3)) return true;
				if (Math.max(Vlb4,Plb4)<=Math.min(Vub4,Pub4)) return true;
				return false;
	}

	public static boolean edge_detect(double T, Vect2 s, Vect2 v, Vect2 segstart, Vect2 segend, Vect2 startvel, Vect2 endvel, double Fac) {
		Vect2 midpt = (segend.Add(segstart)).Scal(0.5);
		Vect2 news = s.Sub(midpt);
		Vect2 midv = (endvel.Add(startvel)).Scal(0.5);
		return edge_detect_simple(T,s.Sub(midpt),v.Sub(midv),segend.Scal(0.5).Sub(segstart.Scal(0.5)),endvel.Scal(0.5).Sub(startvel.Scal(0.5)),Fac);
	}



	public static boolean Collision_Detector(double B, double T, MovingPolygon2D mp, Vect2 s, Vect2 v, double BUFF, double Fac) {
		if (B > 0.0) {
			mp = new MovingPolygon2D(mp.position(B), mp.polyvel, mp.tend-B);
			s = s.AddScal(B, v);
			T = T-B;
		}
		if (!definitely_outside(mp.polystart,s,BUFF)) {
			return true;
		}
		for (int i = 0; i < mp.size(); i++) {
			int nexti = i< mp.size()-1 ? i+1 : 0;
			if (edge_detect(T,s,v,mp.polystart.get(i),mp.polystart.get(nexti), mp.polyvel.get(i),mp.polyvel.get(nexti),Fac)) {
				return true;
			}
		}
		return false;
	}


	// NEW STUFF

	private static double dot_zero_linear_2D_alg(double B, double T, Vect2 w, Vect2 v, Vect2 a, Vect2 b, int eps) {
		double aa = v.dot(b);
		double bb = w.dot(b)+a.dot(v);
		double cc = w.dot(a);
		if (!Util.almost_equals(aa, 0.0) && Util.discr(aa, bb, cc) >= 0.0) {
			return Util.root(aa, bb, cc, eps);
		} else if (!Util.almost_equals(bb, 0.0)) {
			return -cc/bb;
		} else if (eps == -1) {
			return B;
		} else {
			return T;
		}
	}

	private static double lookahead_proj(double B, double T, double t) {
		if (t > T) return T;
		if (t < B) return B;
		return t;
	}

	private static ArrayList<Double> swap_times(double B, double T, Vect2 s, Vect2 v, Vect2 segstart, Vect2 segend, Vect2 startvel, Vect2 endvel) {
		ArrayList<Double> ret = new ArrayList<Double>(6);
		double t0 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart), v.Sub(startvel), segend.Sub(segstart).PerpR(), endvel.Sub(startvel).PerpR(), -1);
		double t1 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart), v.Sub(startvel), segend.Sub(segstart).PerpR(), endvel.Sub(startvel).PerpR(), +1);
		double t2 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart),v.Sub(startvel),segend.Sub(segstart),endvel.Sub(startvel),+1);
		double t3 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart),v.Sub(startvel),segend.Sub(segstart),endvel.Sub(startvel),-1);
		double t4 = dot_zero_linear_2D_alg(B,T,s.Sub(segend),v.Sub(endvel),segstart.Sub(segend),startvel.Sub(endvel),-1);
		double t5 = dot_zero_linear_2D_alg(B,T,s.Sub(segend),v.Sub(endvel),segstart.Sub(segend),startvel.Sub(endvel),+1);
		// only include times that are within bounds
		if (t0 > B && t0 < T) ret.add(t0);
		if (t1 > B && t1 < T) ret.add(t1);
		if (t2 > B && t2 < T) ret.add(t2);
		if (t3 > B && t3 < T) ret.add(t3);
		if (t4 > B && t4 < T) ret.add(t4);
		if (t5 > B && t5 < T) ret.add(t5);
		return ret;
	}

	
	/**
	 * Return a list of all times an edge is intersected.  Returns empty list if not a collision.  These times will be in increasing order, 
	 * @param B
	 * @param T
	 * @param mp
	 * @param s
	 * @param v
	 * @param BUFF
	 * @param Fac
	 * @return
	 */
	public static ArrayList<Double> collisionTimesInOut(double B, double T, MovingPolygon2D mp, Vect2 s, Vect2 v, double BUFF, double Fac) {
		ArrayList<Double> ret = new ArrayList<Double>(6);
		MovingPolygon2D mp2 = mp;
		Vect2 s2 = s;
		double T2 = T;
		if (B > 0.0) {
			mp2 = new MovingPolygon2D(mp.position(B), mp.polyvel, mp.tend-B);
			s2 = s.AddScal(B, v);
			T2 = T-B;
		}
		for (int i = 0; i < mp.size(); i++) {
			int nexti = i< mp.size()-1 ? i+1 : 0;
			if (edge_detect(T2,s2,v,mp2.polystart.get(i),mp2.polystart.get(nexti), mp2.polyvel.get(i),mp2.polyvel.get(nexti),Fac)) {
				ret.addAll(swap_times(B,T,s,v,mp.polystart.get(i),mp.polystart.get(nexti), mp.polyvel.get(i),mp.polyvel.get(nexti)));
			}
		}
		ret.add(B);
		ret.add(T);
		Collections.sort(ret);
		return ret;
	}
}

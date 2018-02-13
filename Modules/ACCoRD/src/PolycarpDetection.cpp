/*
 * PolycarpDetection - collision detection between and point and a 2D polygon
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
//import gov.nasa.larcfm.Util::Interval;
//import gov.nasa.larcfm.Util::MovingPolygon2D;
//import gov.nasa.larcfm.Util::Util;
//import gov.nasa.larcfm.Util::Vect2;
//import static gov.nasa.larcfm.ACCoRD.PolycarpDoubleQuadratic.ae;
//import static gov.nasa.larcfm.ACCoRD.PolycarpContain.definitely_outside;

#include "PolycarpDetection.h"
#include "Interval.h"
#include "Vect2.h"
#include "MovingPolygon2D.h"
#include "PolycarpDoubleQuadratic.h"
#include "PolycarpEdgeProximity.h"
#include "PolycarpContain.h"
#include "Util.h"
#include "format.h"
#include <vector>
#include <algorithm>

namespace larcfm {

std::vector<Vect2> PolycarpDetection::polygon_2D_at(const std::vector<Vect2>& p, const Vect2& pv, double t) {
  std::vector<Vect2> ans;
  for (int i = 0; i < (int) p.size(); i++) {
    ans.push_back(p[i].AddScal(t, pv));
  }
  return ans;
}



Interval PolycarpDetection::dot_nneg_linear_2D_alg(double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, int eps) {
  double aa = v.dot(b);
  double bb = w.dot(b) + a.dot(v);
  double cc = w.dot(a);
  Interval in;
  if (!PolycarpDoubleQuadratic::ae(aa,0)) {
    if (Util::discr(aa,bb,cc)>=0) {
      if (aa<0) {
        in = Interval (Util::root(aa,bb,cc,1),Util::root(aa,bb,cc,-1));
      } else if (eps==-1) {
        in = Interval(0,Util::root(aa,bb,cc,-1));
      } else {
        in = Interval(Util::root(aa,bb,cc,1),T);
      }
    } else if (aa>0) {
      in = Interval(0,T);
    } else {
      in = Interval(T,0);
    }
  } else if (PolycarpDoubleQuadratic::ae(bb,0) && cc>=0) {
    in = Interval(0,T);
  } else if (PolycarpDoubleQuadratic::ae(bb,0)) {
    in = Interval(T,0);
  } else if (bb>0) {
    in = Interval(-cc/bb,T);
  } else {
    in = Interval(0,-cc/bb);
  }
  if (in.up<in.low || in.up<0 || in.low>T) {
    return Interval(T,0);
  } else {
    return Interval(Util::max(in.low,0.0),Util::min(in.up,T));
  }
}

Interval PolycarpDetection::dot_nneg_spec(double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, int eps, int eps2, double Fac) {
  Vect2 ww = a.Scal(Fac).Add(w.Scal(eps2));
  Vect2 vv = b.Scal(Fac).Add(v.Scal(eps2));
  //fpln("PolycarpDetection::dot_nneg_spec T="+Fm4(T)+" ww="+ww.toString()+" vv="+vv.toString());
  return dot_nneg_linear_2D_alg(T,ww,vv,a,b,eps);
}




bool PolycarpDetection::edge_detect_simple(double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, double Fac) { //Fac close to 0
  Interval Vn = dot_nneg_spec(T,w,v,a,b,-1,-1,1+Fac); //[Vn.low,Vn.up]
  Interval V = dot_nneg_spec(T,w,v,a,b,-1,1,1+Fac); //[V.low,V.up]
  Interval Vnx = dot_nneg_spec(T,w,v,a,b,1,-1,1+Fac); //[Vnx.low,Vnx.up]
  Interval Vx = dot_nneg_spec(T,w,v,a,b,1,1,1+Fac); //[Vx.low,Vx.up]
  Interval Pn = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),-1,-1,Fac); //[Pn.low,Pn.up]
  Interval P = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),-1,1,Fac); //[P.low,P.up]
  Interval Pnx = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),1,-1,Fac); //[Pnx.low,Pnx.up]
  Interval Px = dot_nneg_spec(T,w,v,a.PerpR(),b.PerpR(),1,1,Fac); //[Px.low,Px.up]
  double Vlb1 = Util::max(V.low,Vn.low);
  double Vub1 = Util::min(V.up,Vn.up);
  double Vlb2 = Util::max(V.low,Vnx.low);
  double Vub2 = Util::min(V.up,Vnx.up);
  double Vlb3 = Util::max(Vx.low,Vn.low);
  double Vub3 = Util::min(Vx.up,Vn.up);
  double Vlb4 = Util::max(Vx.low,Vnx.low);
  double Vub4 = Util::min(Vx.up,Vnx.up);
  double Plb1 = Util::max(P.low,Pn.low);
  double Pub1 = Util::min(P.up,Pn.up);
  double Plb2 = Util::max(P.low,Pnx.low);
  double Pub2 = Util::min(P.up,Pnx.up);
  double Plb3 = Util::max(Px.low,Pn.low);
  double Pub3 = Util::min(Px.up,Pn.up);
  double Plb4 = Util::max(Px.low,Pnx.low);
  double Pub4 = Util::min(Px.up,Pnx.up);

  //  fpln("edge_detect_simple "+Fm4(V.low)+" "+Fm4(Vn.low)+" "+Fm4(P.low)+" "+Fm4(Pn.low)+" "+Fm4(V.up)+" "+Fm4(Vn.up)+" "+Fm4(P.up)+" "+Fm4(Pn.up));

  if (Util::max(Vlb1,Plb1)<=Util::min(Vub1,Pub1)) return true;
  if (Util::max(Vlb1,Plb2)<=Util::min(Vub1,Pub2)) return true;
  if (Util::max(Vlb1,Plb3)<=Util::min(Vub1,Pub3)) return true;
  if (Util::max(Vlb1,Plb4)<=Util::min(Vub1,Pub4)) return true;
  if (Util::max(Vlb2,Plb1)<=Util::min(Vub2,Pub1)) return true;
  if (Util::max(Vlb2,Plb2)<=Util::min(Vub2,Pub2)) return true;
  if (Util::max(Vlb2,Plb3)<=Util::min(Vub2,Pub3)) return true;
  if (Util::max(Vlb2,Plb4)<=Util::min(Vub2,Pub4)) return true;
  if (Util::max(Vlb3,Plb1)<=Util::min(Vub3,Pub1)) return true;
  if (Util::max(Vlb3,Plb2)<=Util::min(Vub3,Pub2)) return true;
  if (Util::max(Vlb3,Plb3)<=Util::min(Vub3,Pub3)) return true;
  if (Util::max(Vlb3,Plb4)<=Util::min(Vub3,Pub4)) return true;
  if (Util::max(Vlb4,Plb1)<=Util::min(Vub4,Pub1)) return true;
  if (Util::max(Vlb4,Plb2)<=Util::min(Vub4,Pub2)) return true;
  if (Util::max(Vlb4,Plb3)<=Util::min(Vub4,Pub3)) return true;
  if (Util::max(Vlb4,Plb4)<=Util::min(Vub4,Pub4)) return true;
  return false;
}

bool PolycarpDetection::edge_detect(double T, const Vect2& s, const Vect2& v, const Vect2& segstart, const Vect2& segend, const Vect2& startvel, const Vect2& endvel, double Fac) {
  Vect2 midpt = (segend.Add(segstart)).Scal(0.5);
  Vect2 news = s.Sub(midpt);
  Vect2 midv = (endvel.Add(startvel)).Scal(0.5);
  //fpln("edge_detect s="+s.toString()+" v="+v.toString()+" segstart="+segstart.toString()+" segend="+segend.toString()+" startvel="+startvel.toString()+" endvel="+endvel.toString()+" Fac="+Fm6(Fac));
  return edge_detect_simple(T,s.Sub(midpt),v.Sub(midv),segend.Scal(0.5).Sub(segstart.Scal(0.5)),endvel.Scal(0.5).Sub(startvel.Scal(0.5)),Fac);
}

bool PolycarpDetection::Collision_Detector(double B, double T, const MovingPolygon2D& mp, const Vect2& s, const Vect2& v, double BUFF, double Fac, bool insideBad) {
  MovingPolygon2D mp2 = mp;
  double T2 = T;
  Vect2 s2 = s;
  if (B > 0.0) {
    mp2 = MovingPolygon2D(mp.position(B), mp.polyvel, mp.tend-B);
    s2 = s.AddScal(B,v);
    T2 = T-B;
  }
  if (insideBad) {
    if (!PolycarpContain::definitely_outside(mp2.polystart,s2,BUFF)) {
      return true;
    }
  }else {
    if (!PolycarpContain::definitely_inside(mp2.polystart,s2,BUFF)) {
      return true;
    }
  }
  if (T2 <= 0) {
    return false;
  }
  for (int i = 0; i < mp2.size(); i++) {
    int nexti = i< mp2.size()-1 ? i+1 : 0;
    if (edge_detect(T2,s2,v,mp2.polystart[i],mp2.polystart[nexti], mp2.polyvel[i],mp2.polyvel[nexti],Fac)) {
      return true;
    }
  }
  return false;
}


bool PolycarpDetection::Static_Collision_Detector(double B, double T, const std::vector<Vect2>& p, const Vect2& pv, const Vect2& s, const Vect2& v, double BUFF, bool insideBad) {
  if (insideBad) {
    if (!PolycarpContain::definitely_outside(polygon_2D_at(p,pv,B),s.AddScal(B, v),BUFF)) {
      return true;
    } else if (!PolycarpContain::definitely_outside(polygon_2D_at(p,pv,T),s.AddScal(T, v),BUFF)) {
      return true;
    }
  } else {
    if (!PolycarpContain::definitely_inside(polygon_2D_at(p,pv,B),s.AddScal(B, v),BUFF)) {
      return true;
    } else if (!PolycarpContain::definitely_inside(polygon_2D_at(p,pv,T),s.AddScal(T, v),BUFF)) {
      return true;
    }
  }
  if (T <= B) {
    return false;
  }
  for (int i = 0; i < (int) p.size(); i++) {
    int nexti = i+1;
    if (i == (int) p.size()-1) {
      nexti = 0;
    }
    if (PolycarpEdgeProximity::segments_2D_close(p[i], p[nexti], s.AddScal(B, v.Sub(pv)), s.AddScal(T, v.Sub(pv)), BUFF)) {
      return true;
    }
  }
  return false;
}

double PolycarpDetection::dot_zero_linear_2D_alg(double B, double T, const Vect2& w, const Vect2& v, const Vect2& a, const Vect2& b, int eps) {
  double aa = v.dot(b);
  double bb = w.dot(b)+a.dot(v);
  double cc = w.dot(a);
  if (!Util::almost_equals(aa, 0.0) && Util::discr(aa, bb, cc) >= 0.0) {
    return Util::root(aa, bb, cc, eps);
  } else if (!Util::almost_equals(bb, 0.0)) {
    return -cc/bb;
  } else if (eps == -1) {
    return B;
  } else {
    return T;
  }
}

double PolycarpDetection::lookahead_proj(double B, double T, double t) {
  if (t > T) return T;
  if (t < B) return B;
  return t;
}


std::vector<double> PolycarpDetection::swap_times(double B, double T, const Vect2& s, const Vect2& v, const Vect2& segstart, const Vect2& segend, const Vect2& startvel, const Vect2& endvel) {
  std::vector<double> ret;
  double t0 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart), v.Sub(startvel), segend.Sub(segstart).PerpR(), endvel.Sub(startvel).PerpR(), -1);
  double t1 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart), v.Sub(startvel), segend.Sub(segstart).PerpR(), endvel.Sub(startvel).PerpR(), +1);
  double t2 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart),v.Sub(startvel),segend.Sub(segstart),endvel.Sub(startvel),+1);
  double t3 = dot_zero_linear_2D_alg(B,T,s.Sub(segstart),v.Sub(startvel),segend.Sub(segstart),endvel.Sub(startvel),-1);
  double t4 = dot_zero_linear_2D_alg(B,T,s.Sub(segend),v.Sub(endvel),segstart.Sub(segend),startvel.Sub(endvel),-1);
  double t5 = dot_zero_linear_2D_alg(B,T,s.Sub(segend),v.Sub(endvel),segstart.Sub(segend),startvel.Sub(endvel),+1);
  // only include times that are within bounds
  if (t0 > B && t0 < T) ret.push_back(t0);
  if (t1 > B && t1 < T) ret.push_back(t1);
  if (t2 > B && t2 < T) ret.push_back(t2);
  if (t3 > B && t3 < T) ret.push_back(t3);
  if (t4 > B && t4 < T) ret.push_back(t4);
  if (t5 > B && t5 < T) ret.push_back(t5);
  //f.pln(f.Fobj(ret));
  return ret;
}


std::vector<double> PolycarpDetection::collisionTimesInOut(double B, double T, const MovingPolygon2D& mp, const Vect2& s, const Vect2& v, double BUFF, double Fac) {
  std::vector<double> ret;
  MovingPolygon2D mp2 = mp;
  Vect2 s2 = s;
  double T2 = T;
  if (B > 0.0) {
    mp2 = MovingPolygon2D(mp.position(B), mp.polyvel, mp.tend-B);
    s2 = s.AddScal(B, v);
    T2 = T-B;
  }
  if (T2 > 0) {
    for (int i = 0; i < mp.size(); i++) {
      int nexti = i< mp.size()-1 ? i+1 : 0;
      if (edge_detect(T2,s2,v,mp2.polystart[i],mp2.polystart[nexti], mp2.polyvel[i],mp2.polyvel[nexti],Fac)) {
        std::vector<double> st = swap_times(B,T,s,v,mp.polystart[i],mp.polystart[nexti], mp.polyvel[i],mp.polyvel[nexti]);
        ret.insert(ret.end(), st.begin(), st.end() );
        //f.pln("collisionTimesInOut adding "+ret);
      }
    }
  }
  ret.push_back(B);
  ret.push_back(T);
  std::sort(ret.begin(), ret.end());
  //f.pln(f.Fobj(ret));
  return ret;
}

bool PolycarpDetection::nice_moving_polygon_2D(double B, double T, const MovingPolygon2D& mp, double BUFF, double Fac) {
  MovingPolygon2D mp2 = mp;
  if (B > 0.0) {
    mp2 = MovingPolygon2D(mp.position(B), mp.polyvel, mp.tend-B);
    T = T-B;
  }
  if (!PolycarpContain::nice_polygon_2D(mp2.polystart, BUFF)) {
    return false;
  }
  if (mp2.isStable()) {
    return true;
  }
  int sz = mp2.size();
  for (int i = 0; i < sz; i++) {
    Vect2 s = mp2.polystart[i];
    Vect2 v = mp2.polyvel[i];
    for (int e = 0; e < sz; e++) {
      if (e == i-1 || e == i) continue; // skip edges that share vertex i
      if (i == 0 && e == sz-1) continue; // wraparound case to skip an edge
      int nexte = (e == sz-1) ? 0 : e+1;
      if (edge_detect(T,s,v,mp2.polystart[e],mp2.polystart[nexte], mp2.polyvel[e],mp2.polyvel[nexte],Fac)) {
        return false;
      }
    }
  }
  return true;
}


}

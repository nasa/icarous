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
/*
 * PolycarpResolution.cpp
 *
 *  Created on: Dec 18, 2015
 *      Author: ghagen
 */

#include "PolycarpResolution.h"
#include "Vect2.h"
#include "PolycarpEdgeProximity.h"
#include "PolycarpContain.h"
#include <vector>

namespace larcfm {

 Vect2 PolycarpResolution::proj_vect(const Vect2& u, const Vect2& v, const Vect2& w, double BUFF) {
    if (v.Sub(u).norm() <= BUFF || w.Sub(v).norm() <= BUFF) return Vect2(1.0,1.0);
    if (v.Sub(u).dot(w.Sub(v)) >= 0) return v.Sub(u).PerpR().Hat().Add(w.Sub(v).PerpR().Hat());
    if (v.Sub(u).det(v.Sub(u)) <= 0) return v.Sub(u).Hat().Add(v.Sub(w).Hat());
    return u.Sub(v).Hat().Add(w.Sub(v).Hat());
}

 std::vector<Vect2> PolycarpResolution::expand_polygon_2D(double BUFF, double ResolBUFF, const std::vector<Vect2>& p) {
    std::vector<Vect2> p2 = std::vector<Vect2>();
    for (int i = 0; i < (int) p.size(); i++) {
        Vect2 prev = i > 0 ? p[i-1] : p[p.size()-1];
        Vect2 next = i < (int) p.size()-1 ? p[i+1] : p[0];
        Vect2 pv = proj_vect(prev, p[i], next, BUFF);
        Vect2 R = p[i].Sub(prev).PerpR().Hat();
        Vect2 q = p[i].Add(R.Scal(ResolBUFF));
        double tt = (std::abs(pv.dot(R)) <= BUFF/100) ? 0 : q.Sub(p[i]).dot(R)/pv.dot(R);
        p2.push_back(p[i].Add(pv.Scal(tt)));
    }
    return p2;
}

 std::vector<Vect2> PolycarpResolution::contract_polygon_2D(double BUFF, double ResolBUFF, const std::vector<Vect2>& p) {
    std::vector<Vect2> p2 = std::vector<Vect2>();
    for (int i = 0; i < (int) p.size(); i++) {
        Vect2 prev = i > 0 ? p[i-1] : p[p.size()-1];
        Vect2 next = i < (int) p.size()-1 ? p[i+1] : p[0];
        Vect2 pv = proj_vect(prev, p[i], next, BUFF);
        Vect2 R = p[i].Sub(prev).PerpR().Hat();
        Vect2 q = p[i].Sub(R.Scal(ResolBUFF));
        double tt = (std::abs(pv.dot(R)) <= BUFF/100) ? 0 : q.Sub(p[i]).dot(R)/pv.dot(R);
        p2.push_back(p[i].Add(pv.Scal(tt)));
    }
    return p2;
}

 int PolycarpResolution::closest_edge(const std::vector<Vect2>& p, double BUFF, const Vect2& s) {
    int ce = 0;
    for (int i = 0; i < (int) p.size(); i++) {
        int next = i < (int) p.size()-1 ? i+1 : 0;
        Vect2 closp = PolycarpEdgeProximity::closest_point(p[i], p[next], s, BUFF);
        double thisdist = s.Sub(closp).norm();
        int nextce = ce < (int) p.size()-1 ? ce+1 : 0;
        Vect2 prevclosp = PolycarpEdgeProximity::closest_point(p[ce],  p[nextce],  s,  BUFF);
        double prevdist = s.Sub(prevclosp).norm();
        if (thisdist < prevdist) {
            ce = i;
        }
    }
    return ce;
}

 Vect2 PolycarpResolution::recovery_test_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s, int eps) {
    int i = closest_edge(p,BUFF,s);
    int nexti = i < (int) p.size()-1 ? i+1 : 0;
    Vect2 ip = PolycarpEdgeProximity::closest_point(p[i], p[nexti], s, BUFF);
    Vect2 dirvect = p[nexti].Sub(p[i]).PerpR();
    Vect2 testdir = dirvect.Hat();
    Vect2 testvect = ip.Add(testdir.Scal(eps*(ResolBUFF+BUFF/2)));
    return testvect;
}

 Vect2 PolycarpResolution::recovery_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s, int eps) {
    Vect2 tv = recovery_test_point(BUFF,ResolBUFF,p,s,eps);
    if (eps == 1 && PolycarpContain::definitely_outside(p,s,BUFF) &&
        !PolycarpContain::near_any_edge(p,s,ResolBUFF)) return s;
    if (eps == -1 && PolycarpContain::definitely_inside(p,s, BUFF) &&
        !PolycarpContain::near_any_edge(p,s,ResolBUFF)) return s;
    if (eps == 1 && PolycarpContain::definitely_outside(p,tv,BUFF) &&
        !PolycarpContain::near_any_edge(p,tv,ResolBUFF)) return tv;
    if (eps == -1 && PolycarpContain::definitely_inside(p,tv, BUFF) &&
        !PolycarpContain::near_any_edge(p,tv,ResolBUFF)) return tv;
    int i = closest_edge(p,BUFF,s);
    int nexti = i < (int) p.size()-1 ? i+1 : 0;
    int neari = s.Sub(p[i]).sqv() <= s.Sub(p[nexti]).sqv() ? i : nexti;
    int nearnexti = neari < (int) p.size()-1 ? neari+1 : 0;
    int nearprevi = neari > 0 ? neari-1 : p.size()-1;
    Vect2 V1 = p[neari].Sub(p[nearprevi]);
    Vect2 V2 = p[nearnexti].Sub(p[neari]);
    bool leftturn = (V2.det(V1) <= 0);
    Vect2 pv = proj_vect(p[nearprevi], p[neari], p[nearnexti],BUFF);
    Vect2 pvnormed = pv.Hat();
    Vect2 R = p[neari].Sub(p[nearprevi]).PerpR().Hat();
    Vect2 q = p[neari].Add(R.Scal(eps*ResolBUFF));
    double tt = std::abs(pv.dot(R)) <= BUFF/100 ? 0 : q.Sub(p[i]).dot(R)/pv.dot(R);
    Vect2 ans = p[neari].Add(pv.Scal(tt));
    if ((eps == 1 && leftturn) || (eps == -1 && !leftturn)) {
        ans = p[neari].Add(pvnormed.Scal((eps*ResolBUFF)));
    }
    if (eps == 1 && PolycarpContain::definitely_outside(p,ans,BUFF)) {
      return ans;
    }
    if (eps == -1 && PolycarpContain::definitely_inside(p,ans,BUFF)) {
      return ans;
    }
    return ans;
}

 Vect2 PolycarpResolution::outside_recovery_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s) {
    return recovery_point(BUFF,ResolBUFF,p,s,1);
}

 Vect2 PolycarpResolution::inside_recovery_point(double BUFF, double ResolBUFF, const std::vector<Vect2>& p, const Vect2& s) {
    return recovery_point(BUFF,ResolBUFF,p,s,-1);
}


} /* namespace larcfm */

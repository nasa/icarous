/*
 * Copyright (c) 2012-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include "TCAS2D.h"
#include "Util.h"
#include "Horizontal.h"

namespace larcfm {

// Compute modified tau
double TCAS2D::tau_mod(double DMOD, const Vect2& s, const Vect2& v) {
  double sdotv = s.dot(v);

  if (Util::almost_equals(sdotv,0)) // [CAM] Changed from == to almost_equals to mitigate numerical problems
    return 0;
  return (Util::sq(DMOD)-s.sqv())/sdotv;
}

bool TCAS2D::horizontal_RA(double DMOD, double Tau, const Vect2& s, const Vect2& v) {
  if (s.dot(v) >= 0) return s.norm() <= DMOD;
  else return s.norm() <= DMOD || tau_mod(DMOD,s,v) <= Tau;
}

bool TCAS2D::horizontal_RA_at(double DMOD, double Tau, const Vect2& s, const Vect2& v, double t) {
  const Vect2& sat = v.ScalAdd(t,s);
  return horizontal_RA(DMOD,Tau,sat,v);
}

double TCAS2D::nominal_tau(double B, double T, const Vect2& s, const Vect2& v, double rr) {
  if (v.isZero())
    return B;
  return Util::max(B,Util::min(T,-s.dot(v) / v.sqv()-rr/2));
}

double TCAS2D::time_of_min_tau(double DMOD, double B, double T, const Vect2& s, const Vect2& v) {
  if (v.ScalAdd(B,s).dot(v) >= 0)
    return B;
  double d = Horizontal::Delta(s,v,DMOD);
  double rr = 0;
  if (d < 0)
    rr = 2*std::sqrt(-d) / v.sqv();
  if (v.ScalAdd(T,s).dot(v) < 0)
    return T;
  return nominal_tau(B,T,s,v,rr);
}

bool TCAS2D::min_tau_undef(double DMOD, double B, double T, Vect2 s, Vect2 v) {
  return Horizontal::Delta(s,v,DMOD) >= 0 &&
      v.ScalAdd(B,s).dot(v) < 0 && v.ScalAdd(T,s).dot(v) >= 0;
}


bool TCAS2D::RA2D(double DMOD, double Tau, double B, double T, const Vect2& s, const Vect2& v) {
  if (min_tau_undef(DMOD,B,T,s,v))
    return true;
  double mt = time_of_min_tau(DMOD,B,T,s,v);
  return horizontal_RA_at(DMOD,Tau,s,v,mt);
}

void TCAS2D::RA2D_interval(double DMOD, double Tau, double B, double T, Vect2 s, Vect2 vo, Vect2 vi) {
  t_in = B;
  t_out = T;
  Vect2 v = vo.Sub(vi);
  double sqs = s.sqv();
  double sdotv = s.dot(v);
  double sqD = Util::sq(DMOD);
  if (vo.almostEquals(vi) && sqs <= sqD)
    return;
  double sqv = v.sqv();
  if (sqs <= sqD) {
    t_out = Util::root2b(sqv,sdotv,sqs-sqD,1);
    return;
  }
  double b = 2*sdotv+Tau*sqv;
  double c = sqs+Tau*sdotv-sqD;
  if (sdotv >= 0 || Util::discr(sqv,b,c) < 0) {
    t_in = T+1;
    t_out = 0;
    return;
  }
  t_in = Util::root(sqv,b,c,-1);
  if (Horizontal::Delta(s,v,DMOD) >= 0)
    t_out = Horizontal::Theta_D(s,v,1,DMOD);
  else
    t_out = Util::root(sqv,b,c,1);
}

}

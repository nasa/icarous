/*
 * CD2D.cpp 
 * Release: ACCoRD++-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CD2D.h"
#include "Util.h"

namespace larcfm {

// tau_vv = tau*v^2 [defined such that it's continuous everywhere]
double tau_vv(const Vect2& s, const Vect2& v,
    const double B, const double T) {
  return Util::min(Util::max(B*v.sqv(),-(s*v)),T*v.sqv());
}

// omega_vv = omega*v^2 - D^2*v^2 [defined such that it's continuous everywhere]
double omega_vv(const Vect2& s, const Vect2& v,
    const double D, const double B, const double T) {
  if (Util::almost_equals(s.sqv(),sq(D)) && Util::almost_equals(B,0)) {
    return s.dot(v);
  } else {
    double tau = tau_vv(s,v,B,T);
    return v.sqv()*s.sqv() + (2*tau)*(s*v) + sq(tau) - sq(D)*v.sqv();
  }
}

/**
 * violation(s,D) IFF there is an horizontal loss of separation at current time
 */
bool CD2D::violation(const Vect2& s, const double D) {
  return s.sqv() < sq(D);
}

/**
 * detection(s,vo,vi,D,B,T) computes the horizontal conflict time interval
 * (t_in,t_out) in [B,T], where 0 <= B < T
 */
LossData CD2D::detection(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double D, const double B, const double T) {
  double t_in  = T+1;
  double t_out = B;
  if (B >= 0 && B < T) {
    if (vo.almostEquals(vi) && Horizontal::almost_horizontal_los(s,D)) {
      t_in = B;
      t_out = T;
    } else {
      Vect2 v = vo - vi;
      if (Horizontal::Delta(s,v,D) > 0) {
        double tin  = Horizontal::Theta_D(s,v,larcfm::Entry,D);
        double tout = Horizontal::Theta_D(s,v,larcfm::Exit,D);
        t_in  = Util::min(Util::max(tin,B),T);
        t_out = Util::max(Util::min(tout,T),B);
      } 
    }
  }
  return LossData(t_in,t_out);
}

/**
 * detection(s,vo,vi,D,T) computes the horizontal conflict time interval
 * (t_in,t_out) in [0,T], where 0 <= B < T.
 */
LossData CD2D::detection(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double D, const double T) {
  return detection(s,vo,vi,D,0,T);
}

/*
 * detection(s,vo,vi,D) computes the horizontal conflict time interval
 * (t_in,t_out) in [0,...).
 */
LossData CD2D::detection(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double D) {
  return detection(s,vo,vi,D,0,PINFINITY);
}

/**
 * cd2d(s,vo,vi,D,B,T) IFF there is an horizontal conflict in the time
 * interval [B,T], where 0 <= B < T.
 */
bool CD2D::cd2d(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double D, const double B, const double T) {
  if (ISINF(T)) return cd2d(s,vo,vi,D);
  if (B < 0 || B >= T) return false;
  Vect2 v = vo - vi;
  return Horizontal::almost_horizontal_los(v.ScalAdd(B,s),D) ||
      omega_vv(s,v,D,B,T) < 0;
}

/*
 * cd2d(s,vo,vi,D,T) IFF there is an horizontal conflict in the time
 * interval [0,T], where T > 0.
 */
bool CD2D::cd2d(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double D, const double T) {
  return cd2d(s,vo,vi,D,0,T);
}

/*
 * cd2d(s,vo,vi,D) IFF there is an horizontal conflict in the time
 * interval [0,...).
 */
bool CD2D::cd2d(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double D) {
  Vect2 v = vo-vi;
  return Horizontal::almost_horizontal_los(s,D) || (Horizontal::Delta(s,v,D) > 0 && s*v < 0);
}

/**
 * Returns the time to horizontal closest point of approach for s,vo,vi during the
 * interval [B,T], where 0 <= B < T.
 */
double CD2D::tcpa(const Vect2& s, const Vect2& vo, const Vect2& vi,  const double B, const double T) {
  if (vo.almostEquals(vi))
    return B;
  else {
    Vect2 v = vo-vi;
    double tau = -(s.dot(v))/v.sqv();
    return Util::min(T,Util::max(B,tau));
  }
}

/**
 * Returns the time to horizontal closest point of approach for s,vo,vi, for
 * interval [0,T], where T > 0.
 */
double CD2D::tcpa(const Vect2& s, const Vect2& vo, const Vect2& vi, double T) {
  return tcpa(s,vo,vi,0,T);
}

/**
 * Returns the time to horizontal closest point of approach for s,vo,vi, for
 * interval [0,...).
 */
double CD2D::tcpa(const Vect2& s, const Vect2& vo, const Vect2& vi) {
  return tcpa(s,vo,vi,0,PINFINITY);
}

}

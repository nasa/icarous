/*
 * CD2D.java 
 * Release: ACCoRDj-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * CD2D is an algorithm for 2-D conflict *detection*.
 *
 * Unit Convention
 * ---------------
 *   All units in this file are *internal*:
 * - Units of distance are denoted [d]
 * - Units of time are denoted     [t]
 * - Units of speed are denoted    [d/t]
 *
 * REMARK: X points to East, Y points to North. 
 *
 * Naming Convention
 * -----------------
 *   The intruder is fixed at the origin of the coordinate system.
 * 
 *   D  : Diameter of the protected zone [d]
 *   B  : Lower bound of lookahed time interval [t] (B >= 0)
 *   T  : Upper bound of lookahead time interval [t] (B < T)
 *   s  : Relative 2-D position of the ownship [d,d]
 *   vo : Ownship velocity vector [d/t,d/t]
 *   vi : Traffic velocity vector [d/t,d/t]
 * 
 * Functions
 * ---------
 * violation : Check for 2-D loss of separation
 * detection : 2-D conflict detection with calculation of conflict interval 
 * cd2d      : Check for predicted conflict
 * 
 * Global variables (modified by detection)
 * ----------------
 * t_in  : Time to loss of separation
 * t_out : Time to recovery from loss of separation
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 */

package gov.nasa.larcfm.ACCoRD;

import static gov.nasa.larcfm.ACCoRD.Consts.*;
import gov.nasa.larcfm.Util.*;

/**
 * CD2D is an algorithm for 2-D conflict *detection*.<p>
 *
 * All units in this file are *internal*:
 * <ul>
 * <li> Units of distance are denoted [d]
 * <li> Units of time are denoted     [t]
 * <li> Units of speed are denoted    [d/t]
 * </ul>
 *
 * REMARK: X points to East, Y points to North. 
 *
 * Naming Convention<br>
 *   The intruder is fixed at the origin of the coordinate system.
 *<ul>
 *<li>   D  : Diameter of the protected zone [d]
 *<li>   B  : Lower bound of lookahed time interval [t] (B >= 0)
 *<li>   T  : Upper bound of lookahead time interval [t] (B < T)
 *<li>   s  : Relative 2-D position of the ownship [d,d]
 *<li>   vo : Ownship velocity vector [d/t,d/t]
 *<li>   vi : Traffic velocity vector [d/t,d/t]
 *</ul>
 * 
 * Functions <br>
 *<ul>
 *<li> violation : Check for 2-D loss of separation
 *<li> detection : 2-D conflict detection with calculation of conflict interval
 *<li> cd2d      : Check for predicted conflict
 *</ul>
 * 
 */
public class CD2D {

  // tau_vv = tau*v^2 [defined such that it's continuous everywhere]
  private static double tau_vv(Vect2 s, Vect2 v, double B, double T) {
    return Util.min(Util.max(B*v.sqv(),-(s.dot(v))),T*v.sqv());
  }

  // omega_vv = omega*v^2 - D^2*v^2 [defined such that it's continuous everywhere]
  private static double omega_vv(Vect2 s,Vect2 v, double D, double B, double T) {
    if (Util.almost_equals(s.sqv(),Util.sq(D)) && Util.almost_equals(B,0)) {
      return s.dot(v);
    } else {
      double tau = tau_vv(s,v,B,T); 
      return v.sqv()*s.sqv() + (2*tau)*(s.dot(v)) 
          + Util.sq(tau) - Util.sq(D)*v.sqv();
    }
  }

  /** 
   * violation(s,D) IFF there is an horizontal loss of separation at current time
   */
  public static boolean violation(Vect2 s, double D) {
    return s.sqv() < Util.sq(D);
  }

  /** 
   * detection(s,vo,vi,D,B,T) computes the horizontal conflict time interval 
   * (t_in,t_out) in [B,T], where 0 <= B < T
   */
  public static LossData detection(Vect2 s,Vect2 vo, Vect2 vi, double D, double B, double T) {
    double t_in  = T+1;
    double t_out = B;
    if (B >= 0 && B < T) {
      if (vo.almostEquals(vi) && Horizontal.almost_horizontal_los(s,D)) {
        t_in  = B;
        t_out = T;
      } else {
        Vect2 v = vo.Sub(vi);
        if (Horizontal.Delta(s,v,D) > 0) {
          double tin  = Horizontal.Theta_D(s,v,Entry,D);
          double tout = Horizontal.Theta_D(s,v,Exit,D);
          t_in  = Util.min(Util.max(tin,B),T);
          t_out = Util.max(Util.min(tout,T),B);
        } 
      }
    } 
    return new LossData(t_in,t_out);
  }

  /** 
   * detection(s,vo,vi,D,T) computes the horizontal conflict time interval 
   * (t_in,t_out) in [0,T], where 0 <= B < T.
   */
  public static LossData detection(Vect2 s,Vect2 vo, Vect2 vi, double D, double T) { 
    return detection(s,vo,vi,D,0,T);
  }

  /** 
   * detection(s,vo,vi,D) computes the horizontal conflict time interval 
   * (t_in,t_out) in [0,...). 
   */
  public static LossData detection(Vect2 s,Vect2 vo, Vect2 vi, double D) {
    return detection(s,vo,vi,D,0,Double.POSITIVE_INFINITY);
  }

  /**
   * cd2d(s,vo,vi,D,B,T) IFF there is an horizontal conflict in the time 
   * interval [B,T], where 0 <= B < T.
   */
  public static boolean cd2d(Vect2 s, Vect2 vo, Vect2 vi, double D, double B, double T) { 
    if (Double.isInfinite(T)) return cd2d(s,vo,vi,D);
    if (B < 0 || B >= T) return false;
    Vect2 v = vo.Sub(vi);
    return Horizontal.almost_horizontal_los(v.ScalAdd(B,s),D) ||
        omega_vv(s,v,D,B,T) < 0;
  }

  /**
   * cd2d(s,vo,vi,D,T) IFF there is an horizontal conflict in the time 
   * interval [0,T], where T > 0.
   */
  public static boolean cd2d(Vect2 s, Vect2 vo, Vect2 vi, double D, double T) {
    return cd2d(s,vo,vi,D,0,T);
  }

  /**
   * cd2d(s,vo,vi,D) IFF there is an horizontal conflict in the time 
   * interval [0,...).
   */
  public static boolean cd2d(Vect2 s, Vect2 vo, Vect2 vi, double D) {
    Vect2 v = vo.Sub(vi);
    return Horizontal.almost_horizontal_los(s,D) || 
        Horizontal.Delta(s,v,D) > 0 && s.dot(v) < 0;
  } 

  /** 
   * Returns the time to horizontal closest point of approach for s,vo,vi during the
   * interval [B,T], where 0 <= B < T. 
   */
  public static double tcpa(Vect2 s, Vect2 vo, Vect2 vi, double B, double T) {
    if (vo.almostEquals(vi)) 
      return B;
    else {
      Vect2 v = vo.Sub(vi);
      double tau = -(s.dot(v))/v.sqv();
      return Util.min(T,Util.max(B,tau)); 
    }
  }

  /** 
   * Returns the time to horizontal closest point of approach for s,vo,vi, for
   * interval [0,T], where T > 0.
   */
  public static double tcpa(Vect2 s, Vect2 vo, Vect2 vi, double T) {
    return tcpa(s,vo,vi,0,T);
  }

  /** 
   * Returns the time to horizontal closest point of approach for s,vo,vi, for
   * interval [0,...). 
   */
  public static double tcpa(Vect2 s, Vect2 vo, Vect2 vi) {
    return tcpa(s,vo,vi,0,Double.POSITIVE_INFINITY);
  }

}

/*
> * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;
import gov.nasa.larcfm.Util.*;

public class TCAS2D {
  
  double time_in;
  double time_out; 
  
  // Compute modified tau
  public static double tau_mod(double DMOD, Vect2 s, Vect2 v) {
    double sdotv = s.dot(v);
    
    if (Util.almost_equals(sdotv,0)) // [CAM] Changed from == to almost_equals to mitigate numerical problems 
      return 0;
    return (Util.sq(DMOD)-s.sqv())/sdotv;    
  }

  public static boolean horizontal_RA(double DMOD, double Tau, Vect2 s, Vect2 v) {
    if (s.dot(v) >= 0) return s.norm() <= DMOD;
    else return s.norm() <= DMOD || tau_mod(DMOD,s,v) <= Tau;
  }

  public static boolean horizontal_RA_at(double DMOD, double Tau, Vect2 s, Vect2 v, double t) {
    Vect2 sat = v.ScalAdd(t,s);
    return horizontal_RA(DMOD,Tau,sat,v);
  }

  static double nominal_tau(double B, double T, Vect2 s, Vect2 v, double rr) {
    if (v.isZero())
      return B;
    return Util.max(B,Util.min(T,-s.dot(v) / v.sqv()-rr/2));
  }

  public static double time_of_min_tau(double DMOD, double B, double T, Vect2 s, Vect2 v) {
    if (v.ScalAdd(B,s).dot(v) >= 0)
      return B;
    double d = Horizontal.Delta(s,v,DMOD);
    double rr = 0;
    if (d < 0) 
      rr = 2*Math.sqrt(-d) / v.sqv();
    else if (v.ScalAdd(T,s).dot(v) < 0)
      return T;
    return nominal_tau(B,T,s,v,rr);
  }
  
  static boolean min_tau_undef(double DMOD, double B, double T, Vect2 s, Vect2 v) {
    return Horizontal.Delta(s,v,DMOD) >= 0 &&
        v.ScalAdd(B,s).dot(v) < 0 && v.ScalAdd(T,s).dot(v) >= 0;
  }
  
  public static boolean RA2D(double DMOD, double Tau, double B, double T, Vect2 s, Vect2 v) {
    if (min_tau_undef(DMOD,B,T,s,v)) 
        return true;
    double mt = time_of_min_tau(DMOD,B,T,s,v);
    return horizontal_RA_at(DMOD,Tau,s,v,mt);
  }
  
  public void RA2D_interval(double DMOD, double Tau, double B, double T, Vect2 s, Vect2 vo, Vect2 vi) {
    time_in = B;
    time_out = T;
    Vect2 v = vo.Sub(vi);
    double sqs = s.sqv();
    double sdotv = s.dot(v);
    double sqD = Util.sq(DMOD);
    if (vo.almostEquals(vi) && sqs <= sqD) 
      return;
    double sqv = v.sqv();
    if (sqs <= sqD) { 
      time_out = Util.root2b(sqv,sdotv,sqs-sqD,1);  
      return;
    }
    double b = 2*sdotv+Tau*sqv;
    double c = sqs+Tau*sdotv-sqD;
    if (sdotv >= 0 || Util.discr(sqv,b,c) < 0) {
      time_in = T+1;
      time_out = 0;
      return;
    } 
    time_in = Util.root(sqv,b,c,-1);
    if (Horizontal.Delta(s,v,DMOD) >= 0) 
      time_out = Horizontal.Theta_D(s,v,1,DMOD);
    else
      time_out = Util.root(sqv,b,c,1);
  }

}

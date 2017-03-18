/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;
import static gov.nasa.larcfm.ACCoRD.Consts.*;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.LossData;

/*
 *  This static class is constructed to be as close as possible to the PVS model CD3D.
 * 
 */

public class CD3D {

  /**
   * Returns true is aircraft are in loss of separation (LoS) at time 0.
   * 
   * @param s the relative position of the aircraft
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if aircraft are in loss of separation
   */
  static public boolean LoS(Vect3 s, double D, double H) {
    return s.vect2().sqv() < Util.sq(D) && Math.abs(s.z) < H;
  }

  /**
   * Returns true is aircraft are in loss of separation at time 0.
   * 
   * @param so the relative position of the ownship aircraft
   * @param si the relative position of the traffic aircraft
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if aircraft are in loss of separation
   */
  static public boolean lossOfSep(Vect3 so, Vect3 si, double D, double H) {
    return LoS(so.Sub(si),D,H);
  }

  /**
   * Computes the conflict time interval in [B,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time (B < T)
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [B,T].
   * The returned t_in and t_out values are truncated to be within [B,T]
   */
  public static LossData detection(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, double B, double T) { 
    double t_in  = T+1;
    double t_out = B;
    if (B >= 0 && B < T) {
      Vect2 s2  = s.vect2();
      Vect2 vo2 = vo.vect2();
      Vect2 vi2 = vi.vect2();
      double vz = vo.z-vi.z;
      if (vo2.almostEquals(vi2) && Horizontal.almost_horizontal_los(s2,D)) {
        if (!Util.almost_equals(vo.z,vi.z)) {
          t_in  = Util.min(Util.max(Vertical.Theta_H(s.z,vz,Entry,H),B),T);
          t_out = Util.max(Util.min(Vertical.Theta_H(s.z,vz,Exit,H),T),B);
        } else if (Vertical.almost_vertical_los(s.z,H)) {
          t_in  = B;
          t_out = T;
        }     
      } else {
        Vect2 v2 = vo2.Sub(vi2);
        if (Horizontal.Delta(s2,v2,D) > 0) {
          double td1 = Horizontal.Theta_D(s2,v2,Entry,D);
          double td2 = Horizontal.Theta_D(s2,v2,Exit,D);
          if (!Util.almost_equals(vo.z,vi.z)) {
            double tin  = Util.max(td1,Vertical.Theta_H(s.z,vz,Entry,H));
            double tout = Util.min(td2,Vertical.Theta_H(s.z,vz,Exit,H));
            t_in  = Util.min(Util.max(tin,B),T);
            t_out = Util.max(Util.min(tout,T),B);
          } else if (Vertical.almost_vertical_los(s.z,H) ) {
            t_in  = Util.min(Util.max(td1,B),T);
            t_out = Util.max(Util.min(td2,T),B);
          }
        } 
      }
    }
    return new LossData(t_in, t_out);
  }

  /**
   * Computes the actual conflict times 
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if there is a conflict time interval (t_in,t_out)
   */
  public static LossData detectionActual(Vect3 s, Vect3 vo, Vect3 vi, double D, double H) { 
    double t_in  = Double.POSITIVE_INFINITY;
    double t_out = Double.NEGATIVE_INFINITY;
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    double vz = vo.z-vi.z;
    if (vo2.almostEquals(vi2) && Horizontal.almost_horizontal_los(s2,D)) {
      if (!Util.almost_equals(vo.z,vi.z)) {
        t_in  = Vertical.Theta_H(s.z,vz,Entry,H);
        t_out = Vertical.Theta_H(s.z,vz,Exit,H);
      } else if (Vertical.almost_vertical_los(s.z,H)) {  // parallel case
        t_in  = Double.NEGATIVE_INFINITY;
        t_out = Double.POSITIVE_INFINITY;
      }     
    } else {
      Vect2 v2 = vo2.Sub(vi2);
      if (Horizontal.Delta(s2,v2,D) > 0) {
        double td1 = Horizontal.Theta_D(s2,v2,Entry,D);
        double td2 = Horizontal.Theta_D(s2,v2,Exit,D);
        if (!Util.almost_equals(vo.z,vi.z)) {
          t_in  = Util.max(td1,Vertical.Theta_H(s.z,vz,Entry,H));
          t_out = Util.min(td2,Vertical.Theta_H(s.z,vz,Exit,H));
        } else if (Vertical.almost_vertical_los(s.z,H) ) {
          t_in  = td1; 
          t_out = td2; 
        }
      } 
    }
    return new LossData(t_in, t_out);
  }

  /**
   * Determines if there is a conflict in the time interval [B,T]
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time  (B < T)
   * 
   * @return true, if there is a conflict in the time interval [B,T].
   */
  static public boolean cd3d(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, double B, double T) {
    if (B < 0 || B >= T) return false;
    Vect2 s2  = s.vect2(); 
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    if (Util.almost_equals(vo.z,vi.z) && Math.abs(s.z) < H) {
      return CD2D.cd2d(s.vect2(),vo2,vi2,D,B,T);
    }
    double vz = vo.z - vi.z;
    double m1 = Util.max(-H-Util.sign(vz)*s.z,B*Math.abs(vz));
    double m2 = Util.min(H-Util.sign(vz)*s.z,T*Math.abs(vz));
    if (!Util.almost_equals(vo.z,vi.z) && m1 < m2) {
      return CD2D.cd2d(s2.Scal(Math.abs(vz)),
          vo2,vi2,D*Math.abs(vz),m1,m2);
    } else {
      return false;
    }
  }

  /**
   * Determines if there is a conflict in the time interval [0,T]
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param T the upper bound of the lookahead time (T > 0)
   * 
   * @return true, if there is a conflict in the time interval [0,T].
   */  
  static public boolean cd3d(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, double T) {
    return cd3d(s,vo,vi,D,H,0,T);
  }

  /**
   * Determines if there is a conflict in the time interval [0,...)
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if there is a conflict in the time interval [0,...)
   */  
  static public boolean cd3d(Vect3 s, Vect3 vo, Vect3 vi, double D, double H) {
    return cd3d(s,vo,vi,D,H,0,Double.POSITIVE_INFINITY);
  }

  /**
   * Computes the time to cylindrical closest point of approach for the interval [0,...).
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return the time to cylindrical closest point of approach for the interval [0,...).
   */
  public static double tccpa(Vect3 s, Vect3 vo, Vect3 vi, double D, double H) {
    Vect3 v   = vo.Sub(vi);
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    Vect2 v2  = v.vect2();
    double mint = 0;
    double mind = s.cyl_norm(D,H);
    if (!vo2.almostEquals(vi2)) {
      double t = -s2.dot(v2)/v2.sqv();
      if (t > 0) {
        double d = v.ScalAdd(t,s).cyl_norm(D,H);
        if (d < mind) {
          mint = t;
          mind = d;
        }
      }
    } 
    if (!Util.almost_equals(vo.z, vi.z)) {
      double t = -s.z/v.z;
      if (t > 0) {
        double d = v.ScalAdd(t,s).cyl_norm(D,H);
        if (d < mind) {
          mint = t;
          mind = d;
        } 
      }
    }
    double a = v2.sqv()/Util.sq(D) - Util.sq(v.z/H);
    double b = s2.dot(v2)/Util.sq(D) - (s.z*v.z)/Util.sq(H);
    double c = s2.sqv()/Util.sq(D) - Util.sq(s.z/H);
    for (int eps = -1; eps <= 1; eps += 2) {
      double t = Util.root2b(a,b,c,eps);
      if (!Double.isNaN(t) && t > 0) {
        double d = v.ScalAdd(t,s).cyl_norm(D,H);
        if (d < mind) {
          mint = t;
          mind = d;
        }       
      }
    }
    return mint;
  }

  /**
   * Computes the time to cylindrical closest point of approach for the interval [B,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time  (B < T)
   * 
   * @return the time to cylindrical closest point of approach for the interval [B,T].
   */
  public static double tccpa(Vect3 s, Vect3 vo, Vect3 vi, double D, double H,  double B, double T) {
    double tau = tccpa(s,vo,vi,D,H);
    return Util.min(Util.max(B,tau),T);
  }

  /**
   * Computes the time to cylindrical closest point of approach for the interval [0,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param T the upper bound of the lookahead time  (T > 0)
   * 
   * @return the time to cylindrical closest point of approach for the interval [0,T].
   */
  public static double tccpa(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, double T) {
    return tccpa(s,vo,vi,D,H,0,T);
  }

}

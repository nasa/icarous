/*
> * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;
import gov.nasa.larcfm.Util.*;

/* Horizontal Well Clear Volume concept based on Modified TAU
 * DTHR and TAUMOD are distance and time thresholds, respectively 
 */

public class WCV_TAUMOD extends WCV_tvar {
  
  /** Constructor that a default instance of the WCV tables. */
  public WCV_TAUMOD() {
    table = new WCVTable();
    wcv_vertical = new WCV_TCOA();
  }

  /** Constructor that specifies a particular instance of the WCV tables. */
  public WCV_TAUMOD(WCVTable tab) {
  	table = tab.copy();
  	wcv_vertical = new WCV_TCOA();
  }

  public double horizontal_tvar(Vect2 s, Vect2 v) {
    // Time variable is Modified Tau
    double taumod = -1;
    double sdotv = s.dot(v);
    if (sdotv < 0)
      return (Util.sq(table.DTHR)-s.sqv())/sdotv;
    return taumod;
  }
  
  public LossData horizontal_WCV_interval(double T, Vect2 s, Vect2 v) {
    double time_in = T;
    double time_out = 0;
    double sqs = s.sqv();
    double sdotv = s.dot(v);
    double sqD = Util.sq(table.DTHR);
    double a = v.sqv();
    double b = 2*sdotv+table.TTHR*v.sqv();
    double c = sqs+table.TTHR*sdotv-sqD;
    if (Util.almost_equals(a,0) && sqs <= sqD) { // [CAM] Changed from == to almost_equals to mitigate numerical problems 
      time_in = 0;
      time_out = T;
      return new LossData(time_in, time_out);
    }
    if (sqs <= sqD) {
      time_in = 0;
      time_out = Util.min(T,Horizontal.Theta_D(s,v,1,table.DTHR));
      return new LossData(time_in, time_out);
    }
    double discr = Util.sq(b)-4*a*c;
    if (sdotv >= 0 || discr < 0) 
      return new LossData(time_in, time_out);
    double t = (-b - Math.sqrt(discr))/(2*a);
    if (Horizontal.Delta(s, v,table.DTHR) >= 0 && t <= T) {
      time_in = Util.max(0,t);
      time_out = Util.min(T, Horizontal.Theta_D(s,v,1,table.DTHR));
    }
    return new LossData(time_in, time_out);
  } 

  public WCV_TAUMOD make() {
    return new WCV_TAUMOD();
  }

  /**
   * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.  
   */
  public WCV_TAUMOD copy() {
    WCV_TAUMOD ret = new WCV_TAUMOD(table);
    ret.id = id;
    return ret;
  }
  
  public boolean contains(Detection3D cd) {
    if (cd instanceof WCV_TAUMOD || cd instanceof WCV_TCPA) {
      return containsTable((WCV_tvar)cd);
    }
    return false;
  }

}

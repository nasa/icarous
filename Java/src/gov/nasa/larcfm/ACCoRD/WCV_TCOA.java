/*
> * Copyright (c) 2012-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.Util;

/* Vertical Well Clear Volume concept */

public class WCV_TCOA implements WCV_Vertical {
  
  // Vertical Not Well Clear Violation
  // ZTHR and TCOA are altitude and time thresholds 
  public boolean vertical_WCV(double ZTHR, double TCOA, double sz, double vz) {
    return Math.abs(sz) <= ZTHR ||
        (!Util.almost_equals(vz,0) && sz*vz <= 0 && 
        Vertical.time_coalt(sz,vz) <= TCOA); // [CAM] Changed from != to !almost_equals to mitigate numerical problems 
  }
  
  // ZTHR and TCOA are altitude and time thresholds 
  public Interval vertical_WCV_interval(double ZTHR, double TCOA, double B, double T, double sz, double vz) {
    double time_in = B;
    double time_out = T;
    if (Util.almost_equals(vz,0) && Math.abs(sz) <= ZTHR) // [CAM] Changed from == to almost_equals to mitigate numerical problems
      return new Interval(time_in,time_out);
    if (Util.almost_equals(vz,0)) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
      time_in = T;   
      time_out = B;   
      return new Interval(time_in,time_out);
    }   
    double act_H = Util.max(ZTHR,Math.abs(vz)*TCOA);
    double tentry = Vertical.Theta_H(sz,vz,-1,act_H);
    double texit = Vertical.Theta_H(sz,vz,1,ZTHR);
    if (T < tentry || texit < B) {
      time_in = T;
      time_out = B;
      return new Interval(time_in,time_out);
    }
    time_in = Util.max(B,tentry);
    time_out = Util.min(T,texit);
    return new Interval(time_in,time_out);
  }

}

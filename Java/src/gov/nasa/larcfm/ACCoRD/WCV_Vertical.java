/*
> * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Util;

/* Vertical Well Clear Volume concept */

public class WCV_Vertical {
  
  double time_in;
  double time_out; 
  
  // Vertical Not Well Clear Violation
  // ZTHR and TTHR are altitude and time thresholds 
  public static boolean vertical_WCV(double ZTHR, double TCOA, double sz, double vz) {
    return Math.abs(sz) <= ZTHR ||
        (!Util.almost_equals(vz,0) && sz*vz <= 0 && 
        Vertical.time_coalt(sz,vz) <= TCOA); // [CAM] Changed from != to !almost_equals to mitigate numerical problems 
  }

  
  public void vertical_WCV_interval(double ZTHR, double TCOA, double B, double T, double sz, double vz) {
    time_in = B;
    time_out = T;
    if (Util.almost_equals(vz,0) && Math.abs(sz) <= ZTHR) // [CAM] Changed from == to almost_equals to mitigate numerical problems
      return;
    if (Util.almost_equals(vz,0)) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
      time_in = T;   
      time_out = B;   
      return;
    }   
    double act_H = Math.max(ZTHR,Math.abs(vz)*TCOA);
    double tentry = Vertical.Theta_H(sz,vz,-1,act_H);
    double texit = Vertical.Theta_H(sz,vz,1,ZTHR);
    if (T < tentry || texit < B) {
      time_in = T;
      time_out = B;
      return;
    }
    time_in = Math.max(B,tentry);
    time_out = Math.min(T,texit);
  }

}

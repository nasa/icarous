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

public class WCV_TEP extends WCV_tvar {

  /** Constructor that a default instance of the WCV tables. */
  public WCV_TEP() {
    table = new WCVTable();
    wcv_vertical = new WCV_TCOA();
  }

  /** Constructor that specifies a particular instance of the WCV tables. */
  public WCV_TEP(WCVTable tab) {
  	table = tab.copy();
  	wcv_vertical = new WCV_TCOA();
  }

  public double horizontal_tvar(Vect2 s, Vect2 v) {
    // Time variable is Time to Entry Vect3
    double tep = -1;
    double sdotv = s.dot(v);
    if (sdotv < 0 && Horizontal.Delta(s,v,table.DTHR) >= 0)
      return Horizontal.Theta_D(s,v,-1,table.DTHR);
    return tep;
  }

  public LossData horizontal_WCV_interval(double T, Vect2 s, Vect2 v) {
    double time_in = T;
    double time_out = 0;
    double sqs = s.sqv();
    double sqv = v.sqv();
    double sdotv = s.dot(v);
    double sqD = Util.sq(table.DTHR);
    if (Util.almost_equals(sqv,0) && sqs <= sqD) { // [CAM] Changed from == to almost_equals to mitigate numerical problems 
      time_in = 0;
      time_out = T;
      return new LossData(time_in,time_out);
    }
    if (Util.almost_equals(sqv,0)) // [CAM] Changed from == to almost_equals to mitigate numerical problems
      return new LossData(time_in,time_out);
    if (sqs <= sqD) {
      time_in = 0;
      time_out = Math.min(T,Horizontal.Theta_D(s,v,1,table.DTHR));
      return new LossData(time_in,time_out);
    }
    if (sdotv > 0 || Horizontal.Delta(s,v,table.DTHR) < 0) 
      return new LossData(time_in,time_out);
    double tep = Horizontal.Theta_D(s,v,-1,table.DTHR);
    if (tep-table.TTHR > T) 
      return new LossData(time_in,time_out);
    time_in = Math.max(0,tep-table.TTHR);
    time_out = Math.min(T,Horizontal.Theta_D(s,v,1,table.DTHR));
    return new LossData(time_in,time_out);
  }

  public WCV_TEP make() {
    return new WCV_TEP();
  }

  /**
   * Returns a deep copy of this WCV_TEP object, including any results that have been calculated.  
   */
  public WCV_TEP copy() {
    WCV_TEP ret = new WCV_TEP(table);
    ret.id = id;
    return ret;
  }

  public boolean contains(Detection3D cd) {
    if (cd instanceof WCV_TEP || cd instanceof WCV_TAUMOD || cd instanceof WCV_TCPA) {
    	return containsTable((WCV_tvar)cd);
    }
    return false;
  }

}

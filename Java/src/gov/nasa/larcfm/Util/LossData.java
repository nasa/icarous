/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

public class LossData {

  /* 
   * [CAM] (time_in,time_out) is the time interval of loss of separation. Every point in the open interval represents
   * a time where the aircraft are in violation. Whether or not the bounds of the interval are violation points or not
   * depends on the detector, e.g., for CD3D the points time_in and time_out are not necessarily violations points, 
   * for WCV_tvar the points time_in and time_out violation times. Furthermore, time_in and time_out are always 
   * between the lookahead time interval [B,T], where 0 <= B < T. It is always the case that if time_in < time_out 
   * is true, then there is a conflict. 
   */ 
  final public double  time_in;                   // relative time to loss of separation
  final public double  time_out;                  // relative time to the exit from loss of separation	

  public LossData(double tin, double tout) {
    time_in = tin;
    time_out = tout;
  }

  public LossData() {
    time_in = Double.POSITIVE_INFINITY;
    time_out = Double.NEGATIVE_INFINITY;
  }

  /**
   * Returns true if loss
   */
  public boolean conflict() {
    return time_in < time_out && !Util.almost_equals(time_in,time_out); //[CAM] Added to avoid numerical instability  
  }

  /**
   * Returns true if loss last more than thr in seconds
   */
  public boolean conflict(double thr) {
    return conflict() && (time_out - time_in >= thr);
  }

  /**
   * Returns time to first loss in seconds.
   */
  public double getTimeIn() {
    return time_in;
  }

  /**
   * Returns time to last loss in seconds.
   */
  public double getTimeOut() {
    return time_out;
  }

  public String toString() {
    String str = "[time_in: " + f.Fm2(time_in) + ", time_out: " + f.Fm2(time_out)+"]";
    return str;
  }

}

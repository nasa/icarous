/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.LossData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class ConflictData extends LossData {

  final protected double time_crit; // relative time to critical point
  final protected double dist_crit; // distance or severity at critical point (0 is most critical, +inf is least severe)
  final private Vect3  s_; // Relative position
  final private Velocity  v_; // Relative velocity

  public ConflictData(double t_in, double t_out, double t_crit, double d_crit, Vect3 s, Velocity v) {
    super(t_in, t_out);
    time_crit = t_crit;
    dist_crit = d_crit;
    s_ = s;
    v_ = v;
  }
  
  public ConflictData() {
    super();
    time_crit = Double.POSITIVE_INFINITY;
    dist_crit = Double.POSITIVE_INFINITY;
    s_ = Vect3.INVALID;
    v_ = Velocity.INVALID;
  }

  public ConflictData(LossData ld, double t_crit, double d_crit, Vect3 s, Velocity v) {
    super(ld.getTimeIn(), ld.getTimeOut());
    time_crit = t_crit;
    dist_crit = d_crit;
    s_ = s;
    v_ = v;
  }

  /**
   * Returns internal vector representation of relative aircraft position.
   */
  public Vect3 get_s() {
    return s_;
  }

  /**
   * Returns internal vector representation of relative aircraft velocity.
   */
  public Vect3 get_v() {
    return v_;
  }

  /** 
   * Returns HMD, in internal units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  public double HMD(double T) {
    return Horizontal.hmd(s_.vect2(),v_.vect2(),T);
  }

  /** 
   * Returns HMD, in specified units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  public double HMD(String u, double T) {
    return Units.to(u,HMD(T));
  }

  /** 
   * Returns VMD, in internal units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  public double VMD(double T) {
    return Vertical.vmd(s_.z,v_.z,T);
  }

  /** 
   * Returns VMD, in specified units, within lookahead time t, in seconds, assuming straight line trajectory.
   */
  public double VMD(String u, double T) {
    return Units.to(u,VMD(T));
  }

  public double getCriticalTimeOfConflict() {
    return time_crit;
  }

  public double getDistanceAtCriticalTime() {
    return dist_crit;
  }

  public String toString() {
    String str = super.toString()+" [time_crit: "+time_crit+", dist_crit: "+dist_crit+"]";
    return str;
  }

}

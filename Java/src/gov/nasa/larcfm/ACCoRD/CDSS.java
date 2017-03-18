/*
 * CDSS.java
 * Release: ACCoRDj-2.b (08/22/10) 
 *  
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict detection between an ownship and traffic aircraft
 * using state information (specifically for cylindrical separation volumes).
 *  
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * Usage:  CDSS cdss = new CDSS();
 *         cdss.setDistance(5,"NM")
 *         cdss.setHeight(950,"ft")
 *         boolean det = cdss.detection(so,vo,si,vi,T);
 */

package gov.nasa.larcfm.ACCoRD;

import static gov.nasa.larcfm.ACCoRD.Consts.*;
import gov.nasa.larcfm.Util.*;


// TODO: Auto-generated Javadoc
/**
 * The Class CDSS.  This is a dedicated class that only uses the CD3D (CDCylinder) detection algorithm.
 * The more general form is CDSSCore, which may utilize arbitrary Detection3D detection algorithms.
 */
public class CDSS {  

  /** The minimum horizontal distance. */
  private double D;

  /** The minimum vertical distance. */
  private double H;

  /** Time horizon. This parameter affects timeOut()!
   *  Note>  This is NOT the lookahead time!  The lookahead time is provided as a method parameter.
   */
  private double time_horizon;

  /** The conflict filter time. It must be a non-negative number */
  private double filter;

  /** Time of (cylindrical) closest approach *. */
  private double tca;

  /** Aircraft relative position at time of closest approach. */
  private Vect3 stca;

  /** CD3D object */
  private CDCylinder cd;

  private double t_in;
  private double t_out;

  /**
   * Create a new state-based conflict detection object using internal units.
   * 
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   * @param cdfilter the cdfilter
   */
  public CDSS(double distance, double height, double cdfilter) {
    D = distance;
    H = height;
    filter = cdfilter;
    time_horizon = Double.POSITIVE_INFINITY;
    tca  = Double.POSITIVE_INFINITY;
    stca = Vect3.ZERO; //new Vect3();
    t_in = Double.POSITIVE_INFINITY;
    t_out = Double.NEGATIVE_INFINITY;
    cd = new CDCylinder();
  }

  /**
   * Create a new state-based conflict detection object using specified units.
   * 
   * @param distance the minimum horizontal separation distance [dStr]
   * @param height the minimum vertical separation height [hStr].
   */
  public static CDSS make(double distance, String dStr, double height, String hStr) {
    return new CDSS(Units.from(dStr, distance), Units.from(hStr,height), 1.0);  
  }

  /**
   * Create a new state-based conflict detection object using internal units.
   * 
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  public static CDSS mk(double distance, double height) {
    return new CDSS(distance, height, 1.0);  
  }


  public CDSS() {
    this(Units.from("nm",5.0),Units.from("ft",1000),1.0);
  }  
  /**
   * Create a new state-based conflict detection object using specified units.
   * 
   * @param distance the minimum horizontal separation distance [ud]
   * @param ud the specified units of distance
   * @param height the minimum vertical separation height [uh]
   * @param uh the specified units of height
   * @param cdfilter the cdfilter
   */
  public CDSS(double distance, String ud, double height, String uh, double cdfilter) {
    this(Units.from(ud,distance),Units.from(uh,height),
        cdfilter);
  }

  /** Copy constructor */
  public CDSS(CDSS cdss) {
    D = cdss.D;
    H = cdss.H;
    time_horizon = cdss.time_horizon;
    filter = cdss.filter;
    tca = cdss.tca;
    stca = cdss.stca;
    t_in = cdss.t_in;
    t_out = cdss.t_out;
    cd = cdss.cd.copy();
  }


  /**
   * Returns the minimum horizontal separation distance in internal units [m].
   * 
   * @return the distance
   */
  public double getDistance() {
    return D;
  }

  /**
   * Returns the minimum horizontal separation distance in specified units.
   * 
   * @param ud the specified units of distance
   * 
   * @return the distance in specified units [ud]
   */
  public double getDistance(String ud) {
    return Units.to(ud,D);
  }

  /**
   * Returns the minimum vertical separation distance in internal units.
   * 
   * @return the height in internal units [m]
   */
  public double getHeight() {
    return H;
  }

  /**
   * Returns the minimum vertical separation distance in specified units.
   * 
   * @param uh the specified units of height
   * 
   * @return the height in specified units [uh]
   */
  public double getHeight(String uh) {
    return Units.to(uh,H);
  }

  /**
   * Returns the time horizon in internal units.
   *  
   * @return the time horizon [s]
   */
  public double getTimeHorizon() {
    return time_horizon;
  }

  /**
   * Returns the time horizon in specified units.
   * 
   * @param ut the specified units of time 
   * 
   * @return the time horizon in specified units [ut]
   */
  public double getTimeHorizon(String ut) {
    return Units.to(ut,time_horizon);
  }

  /**
   * Returns the conflict detection filter time.
   * 
   * @return the conflict detection filter time seconds
   */
  public double getFilterTime() {
    return filter;
  }

  /**
   * Sets the minimum horizontal separation distance in internal units.
   * 
   * @param distance the distance in internal units [m]
   */
  public void setDistance(double distance) {
    D = distance;
  }

  /**
   * Sets the minimum horizontal separation distance in specified units.
   * 
   * @param distance the distance in specified units [ud]
   * @param ud the specified units of distance
   */
  public void setDistance(double distance, String ud) {
    D = Units.from(ud,distance);
  }

  /**
   * Sets the minimum vertical separation distance in internal units.
   * 
   * @param height the height in internal units [m]
   */
  public void setHeight(double height) {
    H = height;
  }

  /**
   * Sets the minimum vertical separation distance in specified units.
   * 
   * @param height the height in specified units [uh]
   * @param uh the specified units of time of height
   */
  public void setHeight(double height, String uh) {
    H = Units.from(uh,height);
  }

  /**
   * Sets the time horizon in internal units.
   * 
   * @param time the time horizon in internal units [s].
   */
  public void setTimeHorizon(double time) {
    time_horizon = time;
  }

  /**
   * Sets the time horizon in specified units.
   * 
   * @param time the time horizon in specified units [ut].
   * @param ut the specified units of time
   */
  public void setTimeHorizon(double time, String ut) {
    time_horizon = Units.from(ut,time);
  }

  /**
   * Sets the conflict detection filter time.
   * 
   * @param cdfilter the conflict detection filter time in seconds.
   */
  public void setFilterTime(double cdfilter) {
    filter = cdfilter;
  }  

  public double getFilter() {
    return filter;
  }


  /**
   * Determines if two aircraft are in loss of separation.
   * This function DOES NOT compute time interval of conflict.
   * This function uses the CD3D definition of loss of separation -- for a more general solution, use lossOfSeparation(Vect3,Velocity,Velocity)
   * 
   * @param s the relative position of the aircraft
   * 
   * @return true, if the aircraft are in loss of separation.
   */
  public boolean lossOfSeparation(Vect3 s) {
    boolean rtn = CD3D.LoS(s,D,H);
    return rtn;
  }

  /**
   * Determines if two aircraft are in loss of separation.
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo velocity of ownship
   * @param si the position of the intruder
   * @param vi velocity of intruder
   * 
   * @return true, if the aircraft are in loss of separation.
   */
  public boolean lossOfSeparation(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    boolean rtn = cd.violation(so, vo, si, vi, D, H);
    return rtn;
  }

  /**
   * Determines if two aircraft are in loss of separation.
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo velocity of ownship
   * @param si the position of the intruder
   * @param vi velocity of intruder
   * 
   * @return true, if the aircraft are in loss of separation.
   */
  public boolean lossOfSeparation(Vect3 so, Velocity vo, Vect3 si, Velocity vi,double d, double h) {
    boolean rtn = cd.violation(so, vo, si, vi, d, h);
    return rtn;
  }


  /**
   * Determines if two aircraft are in conflict ever within time horizon.
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * 
   * @return true, if the aircraft are in conflict in the interval [0,time_horizon].
   */
  public boolean conflictEver(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    return cd.conflict(so, vo, si, vi, D, H, 0, time_horizon);
  }

  // RWB: These static versions ignore time_horizon

  /**
   * Determines if two aircraft are in conflict in a given lookahead time interval (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * This function uses the CD3D definition of "conflict" -- for a more general solution, use the one of the other conflict() or detection() methods.
   *
   * @param s the relative position of the aircraft
   * @param vo the velocity of the ownship
   * @param vi the velocity of the intruder
   * @param D  The horizontal size of the protection zone
   * @param H  The vertical size of the protection zone
   * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
   *
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  public static boolean conflict(Vect3 s, Velocity vo, Velocity vi, double D, double H, double T) {
    return CD3D.cd3d(s,vo,vi,D,H,T);
  }


  /**
   * Determines if two aircraft are in conflict in a given lookahead time interval (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   *
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder 
   * @param vi the velocity of the intruder
   * @param D  The horizontal size of the protection zone
   * @param H  The vertical size of the protection zone
   * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
   *
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double T) {
    return cd.conflict(so,vo,si,vi,D,H,0.0,T);
  }


  /**
   * Determines if two aircraft are in conflict in a given lookahed time interval (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder 
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in internal units [s] (B >= 0)
   * @param T  upper bound of lookahead time interval in internal units [s] (T > B)
   * 
   * @return true, if the aircraft are in conflict in the interval [B,T].
   */
  public boolean conflictBetween(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return cd.conflict(so,vo,si,vi,D,H,B,Util.min(T,time_horizon));
  }


  /**
   * Determines if two aircraft are in conflict in a given lookahead time internal (specified
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder 
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in specified units [ut] (B >= 0)
   * @param T  upper bound of lookahead time interval in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if the aircraft are in conflict in the interval [B,T].
   */
  public boolean conflictBetween(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T, String ut) {
    return conflictBetween(so,vo,si,vi,Units.from(ut,B),Units.from(ut,T));
  }



  /**
   * Determines if two aircraft are in conflict within a given lookahed time (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  lookahead time in internal units [s] (T > 0)
   * 
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double T) {
    return conflictBetween(so,vo,si,vi,0,T);
  }

  /**
   * Determines if two aircraft are in conflict within a given lookahead time (specified
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  upper bound of lookahead time interval in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double T, String ut) {
    return conflictBetween(so,vo,si,vi,0,Units.from(ut,T));
  }

  /**
   * Detects a conflict that lasts more than filter time within time horizon 
   * and computes the time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,time_horizon].
   */
  public boolean detectionEver(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    ConflictData det = cd.conflictDetection(so, vo, si, vi, D, H, 0.0, time_horizon);
    tca = det.getCriticalTimeOfConflict();
    Vect3 v = vo.Sub(vi);
    stca = v.ScalAdd(tca,so.Sub(si));
    t_in = det.getTimeIn();
    t_out = det.getTimeOut();
    return det.conflict(filter);
  }

  /**
   * Detects a conflict that lasts more than filter time in a given lookahead time 
   * interval and computes the time interval of conflict (internal units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in internal units [s] (B >= 0)
   * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [B,T].
   */
  public boolean detectionBetween(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    boolean conflict = detectionEver(so,vo,si,vi);
    tca =  Util.min(Util.max(B,tca),time_horizon);
    Vect3 v = vo.Sub(vi);
    stca = v.ScalAdd(tca,so.Sub(si));
    return conflict && t_in < T && t_out >= B;    
  }

  /**
   * Detects a conflict that lasts more than filter time in a given lookahead time
   * interval and computes the time interval of conflict (specified units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in specified units [ut] (B >= 0)
   * @param T  upper bound of lookahead time interval in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [B,T].
   */
  public boolean detectionBetween(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T, String ut) {
    return detectionBetween(so,vo,si,vi,Units.from(ut,B),Units.from(ut,T));
  }

  /**
   * Detects a conflict that lasts more than filter time within a given lookahed time 
   * and computes the time interval of conflict (internal units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  the lookahead time in internal units [s] (T > 0)
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,T].
   */
  public boolean detection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double T) {
    return detectionBetween(so,vo,si,vi,0,T);
  }

  /**
   * Detects a conflict that lasts more than filter time within a given lookahead time
   * and computes the time interval of conflict (specified units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  the lookahead time in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,T].
   */

  public boolean detection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double T, String ut) {
    return detectionBetween(so,vo,si,vi,0,Units.from(ut,T));
  }

  /**
   * Detects a conflict that lasts more than filter time within a given lookahed time 
   * and computes the time interval of conflict (internal units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  the lookahead time in internal units [s] (T > 0)
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,T].
   */
  public boolean detection(Position sop, Velocity vop, Position sip, Velocity vip, double T, EuclideanProjection proj) {
    if (sop.isLatLon()) {
      Vect3 so = proj.projectPoint(sop);
      Velocity vo = proj.projectVelocity(sop, vop);
      Vect3 si = proj.projectPoint(sip);
      Velocity vi = proj.projectVelocity(sip, vip);
      return detection(so, vo, si, vi, T);
    } else {
      return detection(sop.point(), vop, sip.point(), vip, T);
    }
  }

  public boolean detection(Position sop, Velocity vop, Position sip, Velocity vip, double T) {
    EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
    return detection(sop,vop,sip,vip,T,proj);
  }

  /**
   * Duration of conflict in internal units. 
   * 
   * @return the duration of conflict. Zero means that there is not conflict.
   */
  public double conflictDuration() {
    if (!conflict()) 
      return 0;
    return t_out - t_in;
  }

  /**
   * Duration of conflict in specified units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the duration of conflict in explicit units [ut]. Zero means that there is not conflict.
   */
  public double conflictDuration(String ut) {
    return Units.to(ut,conflictDuration());
  }

  /** EXPERIMENTAL STATIC TIME INTO LOS (assumes infinite lookahead time), undefined if not conflict
   *  
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return time to enter loss of separation 
   */
  public static double timeIntoLoS(Vect3 s, Vect3 vo, Vect3 vi, double D, double H) { 
    double t_in  = Double.POSITIVE_INFINITY;  // no conflict
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    double vz = vo.z-vi.z;
    if (vo2.almostEquals(vi2) && Horizontal.almost_horizontal_los(s2,D)) {
      if (!Util.almost_equals(vo.z,vi.z)) {
        t_in  = Util.max(Vertical.Theta_H(s.z,vz,Entry,H),0);
      } else if (Vertical.almost_vertical_los(s.z,H)) {
        t_in  = 0;
      }     
    } else {
      Vect2 v2 = vo2.Sub(vi2);
      if (Horizontal.Delta(s2,v2,D) > 0) {
        double td1 = Horizontal.Theta_D(s2,v2,Entry,D);
        if (!Util.almost_equals(vo.z,vi.z)) {
          double tin  = Util.max(td1,Vertical.Theta_H(s.z,vz,Entry,H));
          t_in  = Util.max(tin,0); 
        } else if (Vertical.almost_vertical_los(s.z,H) ) {
          t_in  = Util.max(td1,0);
        }
      } 
    }
    return t_in;
  }


  /**
   * Was there a detected conflict?
   */
  public boolean conflict() {
    return t_in < t_out;
  }

  /**
   * Time to loss of separation in internal units.
   * 
   * @return the time to loss of separation. Note that this is a relative time.
   */
  public double getTimeIn() {
    if (conflict())
      return t_in;
    return Double.POSITIVE_INFINITY;
  }

  /**
   * Time to loss of separation in explicit units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the time to loss of separation in explicit units [ut]. 
   * If time is negative then there is no conflict.
   */
  public double getTimeIn(String ut) {
    return Units.to(ut,getTimeIn());
  }

  /**
   * Time to recovery of loss of separation in internal units.
   * 
   * @return the time to recovery of loss of separation. Note that this is a relative time.
   */
  public double getTimeOut() {
    if (conflict())
      return t_out;
    return Double.NEGATIVE_INFINITY;
  }

  /**
   * Time to recovery of loss of separation in explicit units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the time to recovery of loss of separation in explicit units [ut].
    */
  public double getTimeOut(String ut) {
    return Units.to(ut,getTimeOut());
  }

  /**
   * Time of closest approach in internal units.
   * 
   * @return the cylindrical time of closest approach [s].
   */
  public double timeOfClosestApproach() {
    return tca;
  }

  /**
   * Time of (cylindrical) closest approach in explicit units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the time of (cylindrical) closest approach in explicit units [ut]
   */
  public double timeOfClosestApproach(String ut) {
    return Units.to(ut,tca);
  }

  /**
   * Cylindrical distance at time of closest approach.
   * 
   * @return the cylindrical distance at time of closest approach. This distance normalizes
   * horizontal and vertical distances. Therefore, it is unitless. It has the property that
   * the value is less than 1 if and only if the aircraft are in loss of separation. The value is 1
   * if the ownship is at the boundary of the intruder's protected zone.
   */
  public double cylindricalDistanceAtTCA() { 
    return stca.cyl_norm(D,H);
  }

  /**
   * Relative position at time of closest approach (internal units).
   * 
   * @return the relative position of the ownship with respect to the intruder
   * at time of closest approach. 
   * 
   */
  public Vect3 relativePositionAtTCA() { 
    return stca;
  }

  /**
   * Vertical distance at time of closest approach (internal units).
   * 
   * @return the vertical distance at time of closest approach. 
   * 
   */
  public double verticalDistanceAtTCA() { 
    return Math.abs(stca.z);
  }

  /**
   * Vertical distance at time of closest approach (explicit units).
   * 
   * @param ud the explicit units of distance
   * 
   * @return the vertical distance at time of closest approach. 
   * 
   */
  public double verticalDistanceAtTCA(String ud) { 
    return Units.to(ud,verticalDistanceAtTCA());
  }

  /**
   * Horizontal distance at time of closest approach (internal units).
   * 
   * @return the horizontal distance at time of closest approach. 
   */
  public double horizontalDistanceAtTCA() { 
    return stca.vect2().norm();
  }

  /**
   * Horizontal distance at time of closest approach (explicit units).
   * 
   * @param ud the explicit units of distance
   * 
   * @return the horizontal distance at time of closest approach. 
   */
  public double horizontalDistanceAtTCA(String ud) { 
    return Units.to(ud,horizontalDistanceAtTCA());
  }

  public CDCylinder getCoreDetection() {
    return cd;
  }

  public String toString() {
    String rtn = "CDSS: D = "+Units.str("nm",D)+" H = "+Units.str("ft",H)+" Time Horizon = "+time_horizon+
        " t_in = "+t_in+" t_out =  "+t_out+" tca = "+tca+"\n";
    if (t_in == t_out) rtn = rtn + "--------- No conflict ---------";
    rtn = rtn+"\nUsing "+cd.getClass().getName();
    return rtn;
  }



}


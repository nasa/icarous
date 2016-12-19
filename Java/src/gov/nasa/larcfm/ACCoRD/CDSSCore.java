/*
 * CDSSCore.java
 *  
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict detection between an ownship and traffic aircraft
 * using state information for generic detectors.
 *  
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.*;

// TODO: Auto-generated Javadoc
/**
 * The Class CDSSCore.
 */
public class CDSSCore implements Detection3DAcceptor {  

  /** The conflict filter time. It must be a non-negative number */
  private double filter;

  /** Time of (cylindrical) closest approach *. */
  private double tca;

  /** Aircraft relative position at time of closest approach. */
  private Vect3 stca;

  private double dtca;

  /** CD3D object */
  private Detection3D cd;

  private double t_in;

  private double t_out;

  public CDSSCore(Detection3D cd,double filter) {
    this.filter = filter;
    this.cd = cd.copy();
    this.t_in = Double.POSITIVE_INFINITY;;
    this.t_out = Double.NEGATIVE_INFINITY;
    this.tca = Double.POSITIVE_INFINITY;
    this.dtca = Double.POSITIVE_INFINITY;
  }

  public CDSSCore() {
    this(new CDCylinder(), 1.0);
  }  

  /** Copy constructor */
  public CDSSCore(CDSSCore cdss) {
    filter = cdss.filter;
    tca = cdss.tca;
    stca = cdss.stca;
    dtca = cdss.dtca;
    t_in = cdss.t_in;
    t_out = cdss.t_out;
    cd = cdss.cd.copy();
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
   * Sets the conflict detection filter time.
   * 
   * @param cdfilter the conflict detection filter time in seconds.
   */
  public void setFilterTime(double cdfilter) {
    filter = cdfilter;
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
  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    boolean rtn = cd.violation(so, vo, si, vi);
    return rtn;
  }

  public boolean violation(Position sop, Velocity vop, Position sip, Velocity vip, EuclideanProjection proj) {
    if (sop.isLatLon()) {
      Vect3 so = proj.projectPoint(sop);
      Velocity vo = proj.projectVelocity(sop, vop);
      Vect3 si = proj.projectPoint(sip);
      Velocity vi = proj.projectVelocity(sip, vip);
      return violation(so, vo, si, vi);
    } else {
      return violation(sop.point(), vop, sip.point(), vip);
    }
  }

  public boolean violation(Position sop, Velocity vop, Position sip, Velocity vip) {
    EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
    return violation(sop,vop,sip,vip,proj);
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
  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double T) {
	    return cd.conflict(so,vo,si,vi,0.0,T);
	  }

  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
	    return cd.conflict(so,vo,si,vi,B,T);
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
   * @return true, if there is a conflict that last more than the filter time and is in the interval [0,infinity].
   */
  public boolean detectionEver(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    ConflictData det = cd.conflictDetection(so, vo, si, vi, 0.0, Double.POSITIVE_INFINITY);
    tca = det.getCriticalTimeOfConflict();
    Vect3 v = vo.Sub(vi);
    stca = v.ScalAdd(tca,so.Sub(si));
    dtca = det.getDistanceAtCriticalTime();
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
   * @return true, if there is a conflict that last more than the filter time in the interval [B,T].
   * NOTE: The timeIn and timeOut values are NOT truncated by the B and T values.
   * NOTE: the timeOut value is truncated by TimeHorizon
   */
  public boolean detectionBetween(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    boolean conflict = detectionEver(so,vo,si,vi);
    tca = Util.max(B,tca);
    Vect3 v = vo.Sub(vi);
    stca = v.ScalAdd(tca,so.Sub(si));
    return conflict && t_in < T && t_out >= B;    
  }

  /** Time horizon. 
   *  Note>  This is NOT the lookahead time!  This parameter affects timeOut()!
   *  The lookahead time is provided as a method parameter.
   *  Instead, this is intended to be a hard limit on the duration that state information is valid, for 
   *  example if there is a TCP in the future.
   *  Effectively, a non-negative TimeHorizon will cut short detection at that point, even if the 
   *  lookahead time is longer.
   *  Note: used by CDSICore.
   */
  public boolean detectionBetween(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T, double time_horizon) {
    //f.pln("CDSSCore.detectionBetween: so = "+so+" vo = "+vo+" si = "+si+" vi = "+vi+" B = "+B+" T = "+T+" timeHorizon = "+timeHorizon);
    ConflictData det = cd.conflictDetection(so, vo, si, vi, 0.0, time_horizon);
    tca = det.getCriticalTimeOfConflict();
    t_in = det.getTimeIn();
    t_out = det.getTimeOut();
    boolean conflict = det.conflict(filter);
    tca = Util.min(Util.max(B,tca),time_horizon);
    Vect3 v = vo.Sub(vi);
    stca = v.ScalAdd(tca,so.Sub(si));
    dtca = det.getDistanceAtCriticalTime();
    //f.pln("CDSSCore.detectionBetween: t_in = "+t_in+" t_out = "+t_out);
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
   * and computes the time interval of conflict (internal units). If timeIn() == 0, after this function is called then aircraft 
   * is in loss of separation.
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
   * and computes the time interval of conflict (specified units). If timeIn() == 0, after this function is called then aircraft 
   * is in loss of separation.
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
   * and computes the time interval of conflict (internal units).  If timeIn() == 0, after this function is called then aircraft 
   * is in loss of separation.
   * 
   * @param so   the position of the ownship
   * @param vo   the velocity of the ownship
   * @param si   the position of the intruder
   * @param vi   the velocity of the intruder
   * @param T    the lookahead time in internal units [s] (T > 0)
   * @param proj projection function from lat/lon -> euclidean coordinates

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
    if (!conflict()) return 0;
    return t_out - t_in;
  }

  /**
   * Duration of conflict in specified units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the duration of conflict in explicit units [ut]. Zero  means that 
   * there is no conflict. 
   */
  public double conflictDuration(String ut) {
    return Units.to(ut,conflictDuration());
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
   * Time to exit from loss of separation in internal units.
   * 
   * @return the time to exit out of loss of separation. 
   *  Note that this is a relative time.
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
  public double distanceAtCriticalTime() {
    return dtca;
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

  public String toString() {
    String rtn = "CDSSCore: "+ cd.toString()+"\n";
    if (cd instanceof CDCylinder) {
      double d = ((CDCylinder)cd).getHorizontalSeparation("nmi");
      double h = ((CDCylinder)cd).getVerticalSeparation("ft");
      rtn+= " D="+d+" H="+h;
    }
    rtn = rtn + " t_in = "+t_in+" t_out =  "+t_out+" tca = "+tca+"\n";
    if (t_in == t_out) rtn = rtn + "--------- No conflict ---------";
    rtn = rtn+"\nUsing "+cd.getSimpleClassName();
    return rtn;
  }

  public void setCoreDetection(Detection3D c) {
    cd = c.copy();
    tca = 0;
    stca = Vect3.ZERO;
  }

  public Detection3D getCoreDetection() {
    return cd;
  }

}


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

import java.util.ArrayList;
import java.util.List;

import gov.nasa.larcfm.Util.*;


// TODO: Auto-generated Javadoc
/**
 * The Class CDSS.
 */
public class CDSSPolygon implements DetectionPolygonAcceptor {  

  /** The conflict filter time. It must be a non-negative number */
  private double filter;

  /** CD3D object */
  private DetectionPolygon cd;

  private List<Double> t_in;
  private List<Double> t_out;
  private List<Double> tca;
  private List<Double> dtca;

  public CDSSPolygon(DetectionPolygon cdp, double filt) {
    filter = filt;
    cd = cdp.copy();
    t_in = new ArrayList<Double>();
    t_out = new ArrayList<Double>();
    tca = new ArrayList<Double>();
  }

  public CDSSPolygon() {
    this(new CDPolyIter(), 1.0);
  }  

  /** Copy constructor */
  @SuppressWarnings("unchecked")
  public CDSSPolygon(CDSSPolygon cdss) {
    filter = cdss.filter;
    tca = cdss.tca;
    dtca = cdss.dtca;
    t_in = new ArrayList<Double>(cdss.t_in);
    t_out = new ArrayList<Double>(cdss.t_out);
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
  public boolean violation(Vect3 so, Velocity vo, Poly3D mp) {
    boolean rtn = cd.violation(so, vo, mp);
    return rtn;
  }


  public boolean violation(Position sop, Velocity vop, SimplePoly sp, EuclideanProjection proj) {
    if (sop.isLatLon()) {
      Vect3 so = proj.projectPoint(sop);
      Velocity vo = proj.projectVelocity(sop, vop);
      return violation(so, vo, sp.poly3D(proj));
    } else {
      return violation(sop.point(), vop, sp.poly3D(proj));
    }
  }

  public boolean violation(Position sop, Velocity vop, SimplePoly sp) {
    EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
    return violation(sop,vop,sp,proj);
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
  public boolean conflict(Vect3 so, Velocity vo, MovingPolygon3D mp, double T) {
    return cd.conflict(so,vo,mp,0.0,T);
  }

  public boolean conflict(Position sop, Velocity vop, SimpleMovingPoly mp, double T, EuclideanProjection proj) {
    if (sop.isLatLon()) {
      Vect3 so = proj.projectPoint(sop);
      Velocity vo = proj.projectVelocity(sop, vop);
      return conflict(so, vo, mp.getMovingPolygon(proj), T);		  
    } else {
      return conflict(sop.point(),vop,mp.getMovingPolygon(proj),T);
    }
  }

  public boolean conflict(Position sop, Velocity vop, SimpleMovingPoly mp, double T) {
    EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
    return conflict(sop,vop,mp,T,proj);
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
  public boolean detectionEver(Vect3 so, Velocity vo, MovingPolygon3D mp) {
    t_in.clear();
    t_out.clear();
    tca.clear();
    dtca.clear();
    boolean det = cd.conflictDetection(so, vo, mp, 0.0, -1);
    tca = cd.getCriticalTimesOfConflict();
    dtca = cd.getDistancesAtCriticalTimes();
    t_in = cd.getTimesIn();
    t_out = cd.getTimesOut();
    for (int i = 0; i < t_in.size(); i++) {
      if (t_out.get(i) - t_in.get(i) > filter) {
        tca.remove(i);
        t_in.remove(i);
        t_out.remove(i);
        dtca.remove(i);
        i--; // fix index
      }
    }
    return det && t_in.size() > 0;
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
  public boolean detectionBetween(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T) {
    t_in.clear();
    t_out.clear();
    tca.clear();
    dtca.clear();
    boolean det = cd.conflictDetection(so, vo, mp, B, T);
    tca = cd.getCriticalTimesOfConflict();
    dtca = cd.getDistancesAtCriticalTimes();
    t_in = cd.getTimesIn();
    t_out = cd.getTimesOut();
    for (int i = 0; i < t_in.size(); i++) {
      if (t_out.get(i) - t_in.get(i) > filter) {
        tca.remove(i);
        t_in.remove(i);
        t_out.remove(i);
        dtca.remove(i);
        i--; // fix index
      }
    }
    return det && t_in.size() > 0;  }

  /** Time horizon. This parameter affects timeOut()!
   *  Note>  This is NOT the lookahead time!  The lookahead time is provided as a method parameter.
   *  Instead, this is intended to be a hard limit on the duration that state information is valid, for 
   *  example if there is a TCP in the future.
   *  Effectively, a non-negative TimeHorizon will cut short detection at that point, even if the 
   *  lookahead time is longer.
   *  Note: used by CDSICore.
   */
  public boolean detectionBetween(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, double time_horizon) {
    t_in.clear();
    t_out.clear();
    tca.clear();
    dtca.clear();
    double endT = Math.min(T,time_horizon);
    boolean det = cd.conflictDetection(so, vo, mp, B, endT);
    tca = cd.getCriticalTimesOfConflict();
    t_in = cd.getTimesIn();
    t_out = cd.getTimesOut();
    dtca = cd.getDistancesAtCriticalTimes();
    for (int i = 0; i < t_in.size(); i++) {
      if (t_out.get(i) - t_in.get(i) > filter) {
        tca.remove(i);
        t_in.remove(i);
        t_out.remove(i);
        dtca.remove(i);
        i--; // fix index
      }
    }    
    boolean conflict = det && t_out.size() > 0;
    return conflict;    
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
  public boolean detectionBetween(Vect3 so, Velocity vo, MovingPolygon3D mp, double B, double T, String ut) {
    return detectionBetween(so,vo,mp,Units.from(ut,B),Units.from(ut,T));
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
  public boolean detection(Vect3 so, Velocity vo, MovingPolygon3D mp, double T) {
    return detectionBetween(so,vo,mp,0,T);
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

  public boolean detection(Vect3 so, Velocity vo, MovingPolygon3D mp, double T, String ut) {
    return detectionBetween(so,vo,mp,0,Units.from(ut,T));
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
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,T].
   */
  public boolean detection(Position sop, Velocity vop, SimpleMovingPoly mp, double T, EuclideanProjection proj) {
    if (sop.isLatLon()) {
      Vect3 so = proj.projectPoint(sop);
      Velocity vo = proj.projectVelocity(sop, vop);
      return detection(so, vo, mp.getMovingPolygon(proj), T);
    } else {
      return detection(sop.point(), vop, mp.getMovingPolygon(proj), T);
    }
  }

  public boolean detection(Position sop, Velocity vop, SimpleMovingPoly mp, double T) {
    EuclideanProjection proj = Projection.createProjection(sop.lla().zeroAlt());
    return detection(sop,vop,mp,T,proj);
  }

  /**
   * Duration of conflict in internal units. 
   * 
   * @return the duration of conflict. Zero means that there is not conflict.
   */
  public double conflictDuration(int i) {
    return Math.max(0,t_out.get(i) - t_in.get(i));
  }

  /**
   * Duration of conflict in specified units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the duration of conflict in explicit units [ut]. Zero  means that 
   * there is no conflict. 
   */
  public double conflictDuration(String ut, int i) {
    return Units.to(ut,conflictDuration(i));
  }

  public int conflictSize() {
    return t_in.size();
  }

  /**
   * Time to loss of separation in internal units.
   * 
   * @return the time to loss of separation. If time is negative then there 
   * is no conflict.   Note that this is a relative time.
   */
  public List<Double> getTimesIn() {
    return t_in;
  }

  /**
   * Time to exit from loss of separation in internal units.
   * 
   * @return the time to exit out of loss of separation. If getTimeOut is zero then 
   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
   *  Note that this is a relative time.
   */
  public List<Double> getTimesOut() {
    return t_out;
  }

  /**
   * Time of closest approach in internal units.
   * 
   * @return the cylindrical time of closest approach [s].
   */
  public List<Double> timesOfClosestApproach() {
    return tca;
  }

  public List<Double> distancesAtCriticalTimes() {
    return dtca;
  }

  public String toString() {
    String rtn = "CDSSPolygon: "+ cd.toString()+"\n";
    rtn = rtn + " t_in = "+t_in+" t_out =  "+t_out+" tca = "+tca+"\n";
    if (t_in == t_out) rtn = rtn + "--------- No conflict ---------";
    rtn = rtn+"\nUsing "+cd.getClassName();
    return rtn;
  }

  public void setCorePolygonDetection(DetectionPolygon c) {
    cd = c.copy();
    t_in.clear();
    t_out.clear();
    tca.clear();
    dtca.clear();
  }

  public DetectionPolygon getCorePolygonDetection() {
    return cd;
  }

}


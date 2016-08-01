/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

import java.util.ArrayList;
import java.util.List;

public class KinematicBandsCore implements Detection3DAcceptor {

  public static TCASTable RA = new TCASTable();

  /* Absolute ownship state */
  public TrafficState ownship;
  /* Absolute list of traffic states */
  public List<TrafficState> traffic;
  /* Conflict detector */
  public Detection3D detector;
  /* Lookahead time. This is the time horizon used in the computation of bands (T > 0) */
  public double lookahead_time; 
  /* Alerting time. This is the first time prior to a violation when bands are
   * computed (when this value is 0, lookahead time is used instead) */
  public double alerting_time; 
  /* Maximum time for recovery bands. After this time, bands algorithm gives up 
   * (when this value is 0, lookahead time is used instead) */
  public double max_recovery_time; 
  /* Stability time for the computation of recovery bands. Recovery bands are computed at time 
   * of first green plus this time. */
  public double recovery_stability_time; 
  public TrafficState most_urgent_ac; /* Most urgent aircraft */
  public boolean conflict_crit; /* Use criteria for conflict bands */
  public boolean recovery_crit; /* Use criteria for recovery bands */
  /* Minimum horizontal separation for recovery (when this value is 0, TCAS RA HMD value
   * is used instead) */
  public double min_horizontal_recovery; 
  /* Minimum vertical separation for recovery (when this value is 0, TCAS RA ZTHR value
   * is used instead) */
  public double min_vertical_recovery;   
  /* Compute collision avoidance bands */
  public boolean ca_bands; 

  private boolean outdated_; // Boolean to control re-computation of cached values
  private List<TrafficState> corrective_; // Cached list of corrective aircraft
  private Interval tiov_; // Cached time interval of violations
  private int epsh_; // Cached horizontal epsilon for implicit coordination
  private int epsv_; // Cached vertical epsilon for implicit coordination

  public KinematicBandsCore(Detection3D det) {
    ownship = TrafficState.INVALID;
    traffic = new ArrayList<TrafficState>(); 
    lookahead_time = DefaultDaidalusParameters.getLookaheadTime();
    alerting_time = DefaultDaidalusParameters.getAlertingTime();
    max_recovery_time = DefaultDaidalusParameters.getMaxRecoveryTime();
    recovery_stability_time = DefaultDaidalusParameters.getRecoveryStabilityTime();
    most_urgent_ac = TrafficState.INVALID;
    conflict_crit = DefaultDaidalusParameters.isEnabledConflictCriteria();
    recovery_crit = DefaultDaidalusParameters.isEnabledRecoveryCriteria();
    min_horizontal_recovery = DefaultDaidalusParameters.getMinHorizontalRecovery();
    min_vertical_recovery = DefaultDaidalusParameters.getMinVerticalRecovery();
    ca_bands = DefaultDaidalusParameters.isEnabledCollisionAvoidanceBands();
    detector = det.copy();
    outdated_ = true;
    corrective_ = new ArrayList<TrafficState>();
    tiov_ = Interval.EMPTY;
    epsh_ = 0;
    epsv_ = 0;
  }

  public KinematicBandsCore(KinematicBandsCore core) {
    ownship = core.ownship;
    traffic = new ArrayList<TrafficState>(); 
    traffic.addAll(core.traffic);
    lookahead_time = core.lookahead_time;
    alerting_time = core.alerting_time;
    max_recovery_time = core.max_recovery_time;
    recovery_stability_time = core.recovery_stability_time;
    most_urgent_ac = core.most_urgent_ac;
    conflict_crit = core.conflict_crit;
    recovery_crit = core.recovery_crit;
    min_horizontal_recovery = core.min_horizontal_recovery;
    min_vertical_recovery = core.min_vertical_recovery;
    ca_bands = core.ca_bands;
    detector = core.detector.copy();
    outdated_ = true;
    corrective_ = new ArrayList<TrafficState>();
    tiov_ = Interval.EMPTY;
    epsh_ = 0;
    epsv_ = 0;
  }

  /**
   *  Clear ownship and traffic data from this object.   
   */
  public void clear() {
    ownship = TrafficState.INVALID;
    traffic.clear();
    reset();
  }

  /**
   *  Reset cached values 
   */
  public void reset() {
    outdated_ = true;
    corrective_.clear();
    tiov_ = Interval.EMPTY;
    epsh_ = 0;
    epsv_ = 0;
  }

  /**
   *  Update cached values 
   */
  private void update() {
    if (outdated_) {
      tiov_ = conflictAircraft(corrective_,alertingTime());
      epsh_ = epsilonH(ownship,most_urgent_ac);
      epsv_ = epsilonV(ownship,most_urgent_ac);
      outdated_ = false;
    } 
  }

  /**
   *  Return list of corrective aircraft 
   */
  public List<TrafficState> correctiveAircraft() {
    update();
    return corrective_;
  }

  /**
   *  Return time interval of corrective bands
   */
  public Interval timeIntervalOfCorrectiveBands() {
    update();
    return tiov_;
  }

  /**
   *  Returns horizontal epsilon for implicit coordination with respect to criteria ac
   */
  public int epsilonH() {
    update();
    return epsh_;
  }

  /**
   *  Returns vertical epsilon for implicit coordination with respect to criteria ac
   */
  public int epsilonV() {
    update();
    return epsv_;
  }

  /**
   * Return actual alerting time in seconds. 
   */
  public double alertingTime() {
    return alerting_time > 0 ? alerting_time : lookahead_time;
  }

  /**
   *  Returns actual maximum recovery time in seconds. 
   */ 
  public double maxRecoveryTime() {
    return max_recovery_time > 0 ? max_recovery_time : lookahead_time;
  }

  /**
   * Returns actual minimum horizontal separation for recovery bands in internal units. 
   */
  public double minHorizontalRecovery() {
    if (min_horizontal_recovery > 0) 
      return min_horizontal_recovery;
    int sl = !hasOwnship() ? 3 : Math.max(3,TCASTable.getSensitivityLevel(ownship.getPosition().alt()));
    return RA.getHMD(sl);
  }

  /** 
   * Returns actual minimum vertical separation for recovery bands in internal units. 
   */
  public double minVerticalRecovery() {
    if (min_vertical_recovery > 0) 
      return min_vertical_recovery;
    int sl = !hasOwnship() ? 3 : Math.max(3,TCASTable.getSensitivityLevel(ownship.getPosition().alt()));
    return RA.getZTHR(sl);
  }

  public boolean hasOwnship() {
    return ownship.isValid();
  }

  public TrafficState intruder(String id) {
    return TrafficState.findAircraft(traffic,id);
  }

  public boolean hasTraffic() {
    return traffic.size() > 0;
  }

  public Position trafficPosition(int i) {
    return traffic.get(i).getPosition();
  }

  public Velocity trafficVelocity(int i) {
    return traffic.get(i).getVelocity();
  }

  public Vect3 own_s() {
    return ownship.get_s();
  }

  public Velocity own_v() {
    return ownship.get_v();
  }

  public Vect3 traffic_s(int i) {
    return traffic.get(i).get_s();
  }

  public Velocity traffic_v(int i) {
    return traffic.get(i).get_v();
  }

  /**
   * Returns true if the aircraft are in violation at current time
   */
  public boolean checkViolation(TrafficState ac) {
    return detector.violation(own_s(),own_v(),ac.get_s(),ac.get_v());
  }

  /**
   * Returns true if the aircraft will be in Violation within time [B,T]
   */
  public ConflictData checkConflict(TrafficState ac, double B, double T) {
    return detector.conflictDetection(own_s(),own_v(),ac.get_s(),ac.get_v(),B,T);
  }

  /**
   * Put in conflict the list of aircraft predicted to be in conflict before T [s], 
   * where T <= lookahead_time. Returns the time interval of conflict for all 
   * conflicting aircraft. The time interval is empty if there are no conflicting aircraft.
   */
  public Interval conflictAircraft(List<TrafficState> conflict, double T) {
    conflict.clear();
    double tin = Double.POSITIVE_INFINITY;
    double tout = Double.NEGATIVE_INFINITY;
    for (int i = 0; i < traffic.size(); ++i) {
      TrafficState ac = traffic.get(i);
      ConflictData det = checkConflict(ac,0,lookahead_time);
      if (det.conflict() && det.getTimeIn() <= T) {
        if (det.getTimeIn() < tin) {
          tin = det.getTimeIn();
        }
        if (det.getTimeOut() > tout) {
          tout = det.getTimeOut();
        }
        conflict.add(ac);
      } 
    }
    return new Interval(tin,tout);
  }

  // In contrast to the function provided in CriteriaCore, this function returns 0 
  // when determinant is almost equal to 0 (meaning, no criteria is applied to filter 
  // the resolutions.
  public static int epsilonH(TrafficState ownship, TrafficState ac) {
    if (ownship.isValid() && ac.isValid()) {
      Vect2 s = ownship.get_s().Sub(ac.get_s()).vect2();
      Vect2 v = ownship.get_v().Sub(ac.get_v()).vect2();   
      double d = v.det(s);
      if (Util.almost_equals(d,0,Util.PRECISION5)) {
        return 0;
      } else if (d > 0) {
        return  1;
      } else {
        return -1;
      }
    } else {
      return 0;
    } 
  }

  public static int epsilonV(TrafficState ownship, TrafficState ac) {
    if (ownship.isValid() && ac.isValid()) {
      Vect3 s = ownship.get_s().Sub(ac.get_s());
      return CriteriaCore.verticalCoordinationLoS(s,ownship.get_v(),ac.get_v(),
          ownship.getId(), ac.getId());
    } else {
      return 0;
    }
  }
  
  public TrafficState conflict_ac() {
    return conflict_crit ? most_urgent_ac : TrafficState.INVALID;
  }

  public TrafficState recovery_ac() {
    return recovery_crit ? most_urgent_ac : TrafficState.INVALID;
  }

  public Detection3D getCoreDetection() {
    return getConflictDetector();
  }

  public void setCoreDetection(Detection3D det) {
    setConflictDetector(det);
  }

  public Detection3D getConflictDetector() {
    return detector;
  }

  public void setConflictDetector(Detection3D det) {
    detector = det.copy();
  }

}

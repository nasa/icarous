/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Util;
import gov.nasa.larcfm.Util.Vect2;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

import java.util.ArrayList;
import java.util.List;

public class KinematicBandsCore {

  public static TCASTable RA = new TCASTable();

  /* Absolute ownship state */
  public TrafficState ownship;
  /* Absolute list of traffic states */
  public List<TrafficState> traffic;
  /* Alertor */
  public AlertLevels alertor; 
  /* Stability time for the computation of recovery bands. Recovery bands are computed at time 
   * of first green plus this time. */
  public double lookahead_time; 
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
  public double ca_factor;

  private boolean outdated_; // Boolean to control re-computation of cached values
  private List<List<TrafficState>> conflict_acs_; // Cached list of conflict aircraft per alert level
  /* The length of conflict_acs_ is greater than or equal to the length of the alertor. */
  private int epsh_; // Cached horizontal epsilon for implicit coordination
  private int epsv_; // Cached vertical epsilon for implicit coordination

  public KinematicBandsCore() {
    ownship = TrafficState.INVALID;
    traffic = new ArrayList<TrafficState>(); 
    alertor = new AlertLevels();
    lookahead_time = DaidalusParameters.DefaultValues.getLookaheadTime();
    recovery_stability_time = DaidalusParameters.DefaultValues.getRecoveryStabilityTime();
    most_urgent_ac = TrafficState.INVALID;
    conflict_crit = DaidalusParameters.DefaultValues.isEnabledConflictCriteria();
    recovery_crit = DaidalusParameters.DefaultValues.isEnabledRecoveryCriteria();
    min_horizontal_recovery = DaidalusParameters.DefaultValues.getMinHorizontalRecovery();
    min_vertical_recovery = DaidalusParameters.DefaultValues.getMinVerticalRecovery();
    ca_bands = DaidalusParameters.DefaultValues.isEnabledCollisionAvoidanceBands();
    ca_factor = DaidalusParameters.DefaultValues.getCollisionAvoidanceBandsFactor();
    conflict_acs_ = new ArrayList<List<TrafficState>>();
    reset();
  }

  public KinematicBandsCore(KinematicBandsCore core) {
    ownship = core.ownship;
    traffic = new ArrayList<TrafficState>(); 
    traffic.addAll(core.traffic);
    alertor = new AlertLevels(core.alertor);
    lookahead_time = core.lookahead_time;
    recovery_stability_time = core.recovery_stability_time;
    most_urgent_ac = core.most_urgent_ac;
    conflict_crit = core.conflict_crit;
    recovery_crit = core.recovery_crit;
    min_horizontal_recovery = core.min_horizontal_recovery;
    min_vertical_recovery = core.min_vertical_recovery;
    ca_bands = core.ca_bands;
    ca_factor = core.ca_factor;
    conflict_acs_ = new ArrayList<List<TrafficState>>();
    reset();
  }

  public void setParameters(DaidalusParameters parameters) {
  	alertor = new AlertLevels(parameters.alertor);
  	lookahead_time = parameters.getLookaheadTime();
    recovery_stability_time = parameters.getRecoveryStabilityTime(); 
    conflict_crit = parameters.isEnabledConflictCriteria();
    recovery_crit = parameters.isEnabledRecoveryCriteria();
    min_horizontal_recovery = parameters.getMinHorizontalRecovery();
    min_vertical_recovery = parameters.getMinVerticalRecovery();
    ca_bands = parameters.isEnabledCollisionAvoidanceBands();
    ca_factor = parameters.getCollisionAvoidanceBandsFactor();
    reset();
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
    epsh_ = 0;
    epsv_ = 0;
  }

  /**
   *  Update cached values 
   */
  private void update() {
    if (outdated_) {
      for (int alert_level=1; alert_level <= alertor.mostSevereAlertLevel(); ++alert_level) {
        if (alert_level-1 >= conflict_acs_.size()) {
          conflict_acs_.add(new ArrayList<TrafficState>());
        } else {
          conflict_acs_.get(alert_level-1).clear();
        }
        if (alertor.get(alert_level).getRegion().isConflictBand()) {
          conflict_aircraft(alert_level);
        }
      }
      epsh_ = epsilonH(ownship,most_urgent_ac);
      epsv_ = epsilonV(ownship,most_urgent_ac);
      outdated_ = false;
    } 
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
   * Put in conflict_acs_ the list of aircraft predicted to be in conflict for the given alert level.
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  private void conflict_aircraft(int alert_level) {
    Detection3D detector = alertor.get(alert_level).getDetector();
    double T = alertor.get(alert_level).getAlertingTime();
    for (int i = 0; i < traffic.size(); ++i) {
      TrafficState ac = traffic.get(i);
      ConflictData det = detector.conflictDetection(own_s(),own_v(),ac.get_s(),ac.get_v(),0,T);
      if (det.conflict()) {
        conflict_acs_.get(alert_level-1).add(ac);
      } 
    }
  }

  /**
   * Return list of conflict aircraft for a given alert level.
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  public List<TrafficState> conflictAircraft(int alert_level) {
    update();
    if (alert_level >= 1 && alert_level <= alertor.mostSevereAlertLevel()) {
      return conflict_acs_.get(alert_level-1);
    }
    return TrafficState.INVALIDL;
  }

  // In contrast to the function provided in CriteriaCore, this function returns 0 
  // when determinant is almost equal to 0 (meaning, no criteria is applied to filter 
  // the resolutions.
  public static int epsilonH(TrafficState ownship, TrafficState ac) {
    if (ownship.isValid() && ac.isValid()) {
      Vect2 s = ownship.get_s().Sub(ac.get_s()).vect2();
      Vect2 v = ownship.get_v().Sub(ac.get_v()).vect2();   
      double d = v.det(s);
      if (Util.almost_equals(d,0)) {
        return 0;
      } else if (d > 0) {
        return  1;
      } else if (d < 0) {
        return -1;
      } else {
        return 0;
      } 
    }
    return 0;
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

  public TrafficState criteria_ac() {
    return conflict_crit ? most_urgent_ac : TrafficState.INVALID;
  }

  public TrafficState recovery_ac() {
    return recovery_crit ? most_urgent_ac : TrafficState.INVALID;
  }

}

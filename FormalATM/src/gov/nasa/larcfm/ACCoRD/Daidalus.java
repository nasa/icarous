/*
 * ================================================================================
 * Daidalus : This class implements the interface to DAIDALUS (Detect and Avoid 
 * Alerting Logic for Unmanned Systems). 
 * 
 * Contact:  Cesar Munoz               NASA Langley Research Center
 *           George Hagen              NASA Langley Research Center
 *           
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * ==================================================================================
 */

package gov.nasa.larcfm.ACCoRD;

import java.util.ArrayList;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.List;

import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.LossData;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Position;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class Daidalus implements ErrorReporter {

  /**
   * String indicating the DAIDALUS version
   */
  public static final String VERSION = "0.99c";

  private TrafficState ownship_; // Ownship aircraft. Velocity vector is wind-based.
  private List<TrafficState> traffic_; // Traffic aircraft states. Positions are synchronized in time with ownship. Velocity vector is wind-based.  
  private double current_time_; // Current time
  private Velocity wind_vector_; // Wind information
  private Detection3D detector_; // State-based detector
  private List<AlertThresholds> alertor_; // Alertor
  private UrgencyStrategy urgency_strat_; // Strategy for most urgent aircraft
  private DaidalusParameters parameters_; // Parameters

  private ErrorLog error = new ErrorLog("Daidalus");

  /**
   * Create a new Daidalus object. This object will default to using WCV_TAUMOD as state detector.  
   */
  public Daidalus() {
    parameters_ = new DaidalusParameters(DefaultDaidalusParameters.getParameters());
    detector_ = new WCV_TAUMOD();
    init();
  }

  /**
   * Create a new Daidalus object with the specified state detector. 
   */
  public Daidalus(Detection3D d) {
    parameters_ = new DaidalusParameters(DefaultDaidalusParameters.getParameters());
    detector_ = d.copy();
    init();
  }

  /**
   * Create a new Daidalus object and copy all configuration parameters and traffic information 
   * from another Daidalus object.
   */
  public Daidalus(Daidalus daa) {
    parameters_ = new DaidalusParameters(daa.parameters_);
    detector_ = daa.detector_.copy();
    urgency_strat_ = daa.urgency_strat_.copy();
    wind_vector_ = daa.wind_vector_;
    current_time_ = daa.current_time_;
    ownship_ = daa.ownship_;
    traffic_ = new ArrayList<TrafficState>();
    traffic_.addAll(daa.traffic_);
    // Alertor
    alertor_ = new ArrayList<AlertThresholds>();
    for (int i=0; i < daa.alertor_.size(); ++i) {
      alertor_.add(new AlertThresholds(daa.alertor_.get(i)));
    }
  }

  private void init() {
    urgency_strat_ = new NoneUrgencyStrategy(); 
    wind_vector_ = Velocity.ZERO;
    current_time_ = 0;
    ownship_ = TrafficState.INVALID;
    traffic_ = new ArrayList<TrafficState>();
    alertor_ = new ArrayList<AlertThresholds>();
    setWCVThresholds();    
  }

  public void setWCVThresholds() {

    WCVTable wcv = new WCVTable();
    wcv.setDTHR(0.66,"nmi");
    wcv.setZTHR(450,"ft");
    wcv.setTTHR(35);
    wcv.setTCOA(0);
    setDetector(new WCV_TAUMOD(wcv));
    setAlertingTime(50);
    setLookaheadTime(75);

    WCVTable preventive = new WCVTable();
    preventive.setDTHR(0.66,"nmi");
    preventive.setZTHR(700,"ft");
    preventive.setTTHR(35);
    preventive.setTCOA(0);

    WCVTable corrective = new WCVTable();
    corrective.setDTHR(0.66,"nmi");
    corrective.setZTHR(450,"ft");
    corrective.setTTHR(35);
    corrective.setTCOA(0);

    WCVTable warning = new WCVTable();
    warning.setDTHR(0.66,"nmi");
    warning.setZTHR(450,"ft");
    warning.setTTHR(35);
    warning.setTCOA(0);

    clearAlertLevels();
    addAlertLevel(new AlertThresholds(new WCV_TAUMOD(preventive),50)); 
    addAlertLevel(new AlertThresholds(new WCV_TAUMOD(corrective),50)); 
    addAlertLevel(new AlertThresholds(new WCV_TAUMOD(warning),30));

  }

  public void setMOPSThresholds() {

    WCVTable wcv = new WCVTable();
    wcv.setDTHR(0.9,"nmi");
    wcv.setZTHR(450,"ft");
    wcv.setTTHR(45);
    wcv.setTCOA(0);

    setDetector(new WCV_TAUMOD(wcv));
    setAlertingTime(50);
    setLookaheadTime(75);

    WCVTable preventive = new WCVTable();
    preventive.setDTHR(0.9,"nmi");
    preventive.setZTHR(750,"ft");
    preventive.setTTHR(45);
    preventive.setTCOA(0);

    WCVTable corrective = new WCVTable();
    corrective.setDTHR(0.9,"nmi");
    corrective.setZTHR(450,"ft");
    corrective.setTTHR(45);
    corrective.setTCOA(0);

    WCVTable warning = new WCVTable();
    warning.setDTHR(0.9,"nmi");
    warning.setZTHR(450,"ft");
    warning.setTTHR(45);
    warning.setTCOA(0);

    clearAlertLevels();
    addAlertLevel(new AlertThresholds(new WCV_TAUMOD(preventive),50));
    addAlertLevel(new AlertThresholds(new WCV_TAUMOD(corrective),50));
    addAlertLevel(new AlertThresholds(new WCV_TAUMOD(warning),30));

  }


  /**
   * Clear aircraft list, current time, and wind vector.
   */
  public void reset() {
    ownship_ = TrafficState.INVALID;
    traffic_.clear();
    wind_vector_ = Velocity.ZERO;
    current_time_ = 0;
  }

  /**
   * @return a reference to this object's Detectoin3D instance.
   */
  public Detection3D getDetector() {
    return detector_;
  }

  /**
   * Set this object's Detection3D method to be a copy of the given method.
   */
  public void setDetector(Detection3D d) {
    detector_ = d.copy();
  }

  /**
   * @return number of aircraft, including ownship.
   */
  public int numberOfAircraft() {
    if (!ownship_.isValid()) {
      return 0;
    } else {
      return traffic_.size()+1;
    }
  }

  /**
   * @return last traffic index. Every traffic aircraft has an index between 1 and lastTrafficIndex. 
   * The index 0 is reserved for the ownship. When lastTrafficIndex is 0, the ownship is set but no
   * traffic aircraft has been set. When lastTrafficIndex is negative, ownship has not been set.
   */
  public int lastTrafficIndex() {
    if (!ownship_.isValid()) {
      return -1; 
    } else {
      return traffic_.size();
    }
  }

  /**
   * Get wind vector
   */
  public Velocity getWindField() {
    return wind_vector_;
  }

  /**
   * Set wind vector (common to all aircraft)
   */
  public void setWindField(Velocity wind) {
    if (lastTrafficIndex() >= 0) {
      Velocity delta_wind = wind_vector_.Sub(wind);
      ownship_ = TrafficState.makeOwnship(ownship_.getId(),ownship_.getPosition(),
          ownship_.getVelocity().Add(delta_wind));
      for (int i=0; i < traffic_.size(); ++i) {
        TrafficState ac = traffic_.get(i);
        traffic_.set(i,ownship_.makeIntruder(ac.getId(),ac.getPosition(),
            ac.getVelocity().Add(delta_wind)));
      }
    }
    wind_vector_ = wind;
  }

  /**
   * Clear all aircraft and set ownship state and current time. Velocity vector is ground velocity.
   */
  public void setOwnshipState(String id, Position pos, Velocity vel, double time) {
    traffic_.clear();
    ownship_ = TrafficState.makeOwnship(id,pos,vel.Sub(wind_vector_));
    current_time_ = time;
  }

  /**
   * Add traffic state at given time. Velocity vector is ground velocity.
   * If time is different from current time, traffic state is projected, past or future, 
   * into current time assuming wind information. If it's the first aircraft, this aircraft is 
   * set as the ownship. Return aircraft index.
   */
  public int addTrafficState(String id, Position pos, Velocity vel, double time) {
    if (lastTrafficIndex() < 0) {
      setOwnshipState(id,pos,vel,time);
      return 0;
    } else {
      double dt = current_time_-time;
      Position pt = dt == 0 ? pos : pos.linear(vel,dt);    
      TrafficState ac = ownship_.makeIntruder(id,pt,vel.Sub(wind_vector_));
      if (ac.isValid()) {
        traffic_.add(ac);
        return traffic_.size();
      } else {
        return -1;
      }
    }
  }

  /**
   * Add traffic state at current time. Velocity vector is ground velocity. 
   * If it's the first aircraft, this aircraft is set as the ownship. Return aircraft index.
   */
  public int addTrafficState(String id, Position pos, Velocity vel) {
    return addTrafficState(id,pos,vel,current_time_);
  }

  /**
   * Exchange ownship aircraft with aircraft at index ac_idx.
   */
  public void resetOwnship(int ac_idx) {
    if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
      int ac = ac_idx-1;
      TrafficState new_own = TrafficState.makeOwnship(traffic_.get(ac).getId(),traffic_.get(ac).getPosition(),traffic_.get(ac).getVelocity());
      TrafficState old_own = new_own.makeIntruder(ownship_.getId(),ownship_.getPosition(),ownship_.getVelocity());
      ownship_ = new_own;
      for (int i = 0; i < traffic_.size(); ++i) {
        if (i == ac) {
          traffic_.set(i,old_own);
        } else {
          traffic_.set(i,ownship_.makeIntruder(traffic_.get(i).getId(),traffic_.get(i).getPosition(),traffic_.get(i).getVelocity()));
        }
      }
    } else {
      error.addError("resetOwnship: aircraft index "+ac_idx+" is out of bounds");
    }
  }

  /**
   * Exchange ownship aircraft with aircraft named id.
   */
  public void resetOwnship(String id) {
    resetOwnship(aircraftIndex(id));
  }

  /** 
   * Get index of aircraft with given name. Return -1 if no such index exists
   */
  public int aircraftIndex(String name) {
    if (lastTrafficIndex() >= 0) {
      if (ownship_.getId().equals(name)) {
        return 0;
      }
      for (int i = 0; i < traffic_.size(); ++i) {
        if (traffic_.get(i).getId().equals(name))
          return i+1;
      }
    }
    return -1;
  }

  /** 
   * Clears alert levels
   **/
  public void clearAlertLevels() {
    alertor_.clear();
  }

  /**
   * @return numerical type of most severe alert level
   */
  public int mostSevereAlertLevel() {
    return alertor_.size();
  }

  /**
   * Resets the threshold values of a given alert level. 
   */
  public void resetAlertLevel(int level, AlertThresholds thresholds) {
    if (0 < level && level <= alertor_.size()) {
      alertor_.set(level-1,new AlertThresholds(thresholds));
    } else {
      error.addError("resetAlertLevel: invalid alert level "+level);
    }
  }

  /**
   * Adds the threshold values of a new alert level and returns its numerical type, which is a positive
   * number.
   */
  public int addAlertLevel(AlertThresholds thresholds) {
    alertor_.add(new AlertThresholds(thresholds));
    return alertor_.size();
  }

  /** 
   * @return threshold values of a given alert level
   */
  public AlertThresholds getAlertLevel(int level) {
    if (0 < level && level <= alertor_.size()) {
      return alertor_.get(level-1);
    } else {
      error.addError("getAlertLevel: invalid alert level "+level);
      return AlertThresholds.INVALID;
    }
  }

  /**
   * Set alert thresholds for alerting logic.
   */
  public void setAlertor(List<AlertThresholds> al) {
    alertor_.clear();
    for (int i=0; i < al.size(); ++i) {
      alertor_.add(new AlertThresholds(al.get(i)));
    }
  }

  /**
   * Get alert thresholds for alerting logic.
   */
  public List<AlertThresholds> getAlertor() {
    return alertor_;
  }

  /**
   * @return get current time, i.e., time of ownship aircraft.
   */
  public double getCurrentTime() {
    return current_time_;
  }

  /**
   * Linearly projects all aircraft states to time t [s] and set current time to t.
   */
  public void setCurrentTime(double time) {
    if (time != current_time_ && lastTrafficIndex() >= 0) {
      double dt = time-current_time_;
      Velocity vo = ownship_.getVelocity().Add(wind_vector_); // Original ground velocity
      Position po = ownship_.getPosition().linear(vo,dt);   
      ownship_ = TrafficState.makeOwnship(ownship_.getId(),po,ownship_.getVelocity());
      for (int i=0; i < traffic_.size(); ++i) {
        TrafficState ac = traffic_.get(i);
        Velocity vi = ac.getVelocity().Add(wind_vector_); // Original ground velocity
        Position pi = ac.getPosition().linear(vi,dt); 
        traffic_.set(i,ownship_.makeIntruder(ac.getId(),pi,ac.getVelocity()));
      }
      current_time_ = time;
    }
  }

  /** 
   * Computes alerting type of ownship and aircraft at index ac_idx for current 
   * aircraft states.  The number 0 means no alert. A negative number means
   * that aircraft index is not valid.
   * When the alertor object has been configured to consider ownship maneuvers, i.e.,
   * using spread values, the alerting logic could also use information about the ownship
   * turning, accelerating, and climbing status as follows:
   * - turning < 0: ownship is turning left, turning > 0: ownship is turning right, turning = 0: 
   * do not make any turning assumption about the ownship.
   * - accelerating < 0: ownship is decelerating, accelerating > 0: ownship is accelerating, 
   * accelerating = 0: do not make any accelerating assumption about the ownship.
   * - climbing < 0: ownship is descending, climbing > 0: ownship is climbing, climbing = 0:
   * do not make any climbing assumption about the ownship.
   */
  public int alerting(int ac_idx, int turning, int accelerating, int climbing) {
    if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
      TrafficState ac = traffic_.get(ac_idx-1);
      TrafficState mu_ac = mostUrgentAircraft();
      for (int i=alertor_.size(); i > 0; --i) {
        if (alertor_.get(i-1).alerting(parameters_,ownship_,ac,mu_ac,turning,accelerating,climbing)) {
          return i;
        }
      }
      return 0;
    } else {
      error.addError("alerting: aircraft index "+ac_idx+" is out of bounds");
      return -1;
    } 
  }

  /** 
   * Computes alerting type of ownship and aircraft at index ac_idx for current 
   * aircraft states.  The number 0 means no alert. A negative number means
   * that aircraft index is not valid.
   */
  public int alerting(int ac_idx) {
    return alerting(ac_idx,0,0,0);
  }

  /**
   * Detects conflict with aircraft at index ac_idx. Conflict data provides time 
   * to first violation and time to last violation. 
   */
  public ConflictData detection(int ac_idx) {
    if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
      TrafficState ac = traffic_.get(ac_idx-1);
      return detector_.conflictDetection(ownship_.get_s(),ownship_.get_v(),ac.get_s(),ac.get_v(),
          0,getLookaheadTime());
    } else {
      error.addError("detection: aircraft index "+ac_idx+" is out of bounds");
      return new ConflictData();
    }
  }

  /**
   * @return time to violation, in seconds, between ownship and aircraft at index ac_idx, for the 
   * lookahead time. The returned time is relative to current time. NaN means no 
   * conflict within lookahead time or aircraft index is out of range. 
   */
  public double timeToViolation(int ac_idx) {
    if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
      ConflictData det = detection(ac_idx);
      if (det.conflict()) {
        return det.getTimeIn();
      }
    } else {
      error.addError("timeToViolation: aircraft index "+ac_idx+" is out of bounds");
    }
    return Double.NaN;
  }

  /**
   * @return kinematic bands. Computation of bands is lazy, they are only compute when needed.
   */
  public KinematicBands getKinematicBands() {   
    if (lastTrafficIndex() < 0) {
      error.addError("getKinematicBands: ownship has not been set");
      return new KinematicBands();
    } else {
      KinematicBands bands = new KinematicBands(detector_); 
      bands.setOwnship(ownship_);
      bands.setTraffic(traffic_);
      bands.setParameters(parameters_);
      bands.setMostUrgentAircraft(mostUrgentAircraft());
      return bands;
    }
  }

  /**
   * Returns state of ownship.
   */
  public TrafficState getOwnshipState() {
    return ownship_;
  }

  /**
   * Returns state of aircraft at index ac_idx 
   */
  public TrafficState getAircraftState(int ac_idx) {
    if (0 <= ac_idx && ac_idx <= lastTrafficIndex()) {
      if (ac_idx == 0) {
        return ownship_;
      } else {
        return traffic_.get(ac_idx-1);
      }
    } else {
      error.addError("getAircraftState: aircraft index "+ac_idx+" is out of bounds");
      return TrafficState.INVALID;
    }
  }

  public boolean hasError() {
    return error.hasError();
  }

  public boolean hasMessage() {
    return error.hasMessage();
  }

  public String getMessage() {
    return error.getMessage();
  }

  public String getMessageNoClear() {
    return error.getMessageNoClear();
  }

  public DaidalusParameters getParameters() {
    return parameters_;
  }

  /** 
   * @return DTHR threshold in internal units [m]. Return NaN if detector is not an instance of a WCV detector.
   */
  public double getDTHR() {
    if (!(detector_ instanceof WCV_tvar)) {
      error.addWarning("[getDTHR] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      return Double.NaN;
    } else {
      return ((WCV_tvar)detector_).getDTHR();
    }
  }

  /** 
   * @return ZTHR threshold in internal units [m]. Return NaN if detector is not an instance of a WCV detector.
   */
  public double getZTHR() {
    if (!(detector_ instanceof WCV_tvar)) {
      error.addWarning("[getZTHR] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      return Double.NaN;
    } else {
      return ((WCV_tvar)detector_).getZTHR();
    }
  }

  /** 
   * @return TTHR threshold in seconds. Return NaN if detector is not an instance of a WCV detector.
   */
  public double getTTHR() {
    if (!(detector_ instanceof WCV_tvar)) {
      error.addWarning("[getTTHR] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      return Double.NaN;
    } else {
      return ((WCV_tvar)detector_).getTTHR();
    }
  }

  /** 
   * @return TCOA threshold in seconds. Return NaN if detector is not an instance of a WCV detector.
   */
  public double getTCOA() {
    if (!(detector_ instanceof WCV_tvar)) {
      error.addWarning("[getTCOA] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      return Double.NaN;
    } else {
      return ((WCV_tvar)detector_).getTCOA();
    }
  }

  /** 
   * Sets DTHR threshold in internal units [m] when detector is an instance of a WCV detector.
   */
  public void setDTHR(double val) {
    if (error.isPositive("setDTHR",val)) {
      if (!(detector_ instanceof WCV_tvar)) {
        error.addWarning("[setDTHR] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      } else {
        ((WCV_tvar)detector_).setDTHR(val);
      }
    }
  }

  /** 
   * Sets ZTHR threshold in internal units [m] when detector is an instance of a WCV detector.
   */
  public void setZTHR(double val) {
    if (error.isPositive("setZTHR",val)) {
      if (!(detector_ instanceof WCV_tvar)) {
        error.addWarning("[setZTHR] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      } else {
        ((WCV_tvar)detector_).setZTHR(val);
      }
    } 
  }

  /** 
   * Sets TTHR threshold in seconds when detector is an instance of a WCV detector.
   */
  public void setTTHR(double val) {
    if (error.isNonNegative("setTTHR",val)) {
      if (!(detector_ instanceof WCV_tvar)) {
        error.addWarning("[setTTHR] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      } else {
        ((WCV_tvar)detector_).setTTHR(val);
      }
    }
  }

  /** 
   * Sets TCOA threshold in seconds when detector is an instance of a WCV detector.
   */
  public void setTCOA(double val) {
    if (error.isNonNegative("setTCOA",val)) {
      if (!(detector_ instanceof WCV_tvar)) {
        error.addWarning("[setTCOA] Detector "+detector_.getClass().getCanonicalName()+" is not an instance of WCV detector");
      } else {
        ((WCV_tvar)detector_).setTCOA(val);
      }
    } 
  }

  /** 
   * @return DTHR threshold in specified units [u]. Return NaN if detector is not an instance of a WCV detector.
   */
  public double getDTHR(String u) {
    return Units.to(u,getDTHR());
  }

  /** 
   * @return DTHR threshold in specified units [u]. Return NaN if detector is not an instance of a WCV detector.
   */
  public double getZTHR(String u) {
    return Units.to(u,getZTHR());
  }

  /** 
   * Sets DTHR threshold in specified units [u] when detector is an instance of a WCV detector.
   */
  public void setDTHR(double val, String u) {
    setDTHR(Units.from(u,val));
  }

  /** 
   * Sets ZTHR threshold in specified units [u] when detector is an instance of a WCV detector.
   */
  public void setZTHR(double val, String u) {
    setZTHR(Units.from(u,val));
  }

  /** 
   * @return alerting time. If set to 0, returns lookahead time instead.
   */
  public double alertingTime() {
    double alerting_time = parameters_.getAlertingTime();
    return alerting_time > 0 ? alerting_time : parameters_.getLookaheadTime();
  }

  /** 
   * Get alerting time in seconds
   */
  public double getAlertingTime() {
    return parameters_.getAlertingTime();
  }

  /** 
   * Get alerting time in specified units [u]
   */
  public double getAlertingTime(String u) {
    return parameters_.getAlertingTime(u);
  }

  /** 
   * Get lookahead time in seconds
   */
  public double getLookaheadTime() {
    return parameters_.getLookaheadTime();
  }

  /** 
   * Get lookahead time in specified units [u]
   */
  public double getLookaheadTime(String u) {
    return parameters_.getLookaheadTime(u);
  }

  /** 
   * @return left track in radians [-pi - 0] [rad] from current ownship's track
   */
  public double getLeftTrack() {
    return parameters_.getLeftTrack();
  }

  /** 
   * @return left track in specified units [-pi - 0] [u] from current ownship's track
   */
  public double getLeftTrack(String u) {
    return parameters_.getLeftTrack(u);
  }

  /** 
   * @return right track in radians [0 - pi] [rad] from current ownship's track
   */
  public double getRightTrack() {
    return parameters_.getRightTrack();
  }

  /** 
   * @return right track in specified units [0 - pi] [u] from current ownship's track
   */
  public double getRightTrack(String u) {
    return parameters_.getRightTrack(u);
  }

  /** 
   * Get minimum ground speed in internal units [m/s]
   */
  public double getMinGroundSpeed() {
    return parameters_.getMinGroundSpeed();
  }

  /** 
   * Get minimum ground speed in specified units [u]
   */
  public double getMinGroundSpeed(String u) {
    return parameters_.getMinGroundSpeed(u);
  }

  /** 
   * Get maximum ground speed in internal units [m/s]
   */
  public double getMaxGroundSpeed() {
    return parameters_.getMaxGroundSpeed();
  }

  /** 
   * Get maximum ground speed in specified units [u]
   */
  public double getMaxGroundSpeed(String u) {
    return parameters_.getMaxGroundSpeed(u);
  }

  /** 
   * Get minimum vertical speed in internal units [m/s]
   */
  public double getMinVerticalSpeed() {
    return parameters_.getMinVerticalSpeed();
  }

  /** 
   * Get minimum vertical speed in specified units [u]
   */
  public double getMinVerticalSpeed(String u) {
    return parameters_.getMinVerticalSpeed(u);
  }

  /** 
   * Get maximum vertical speed in internal units [m/s]
   */
  public double getMaxVerticalSpeed() {
    return parameters_.getMaxVerticalSpeed();
  }

  /** 
   * Get maximum vertical speed in specified units [u]
   */
  public double getMaxVerticalSpeed(String u) {
    return parameters_.getMaxVerticalSpeed(u);
  }

  /** 
   * Get minimum altitude in internal units [m]
   */
  public double getMinAltitude() {
    return parameters_.getMinAltitude();
  }

  /** 
   * Get minimum altitude in specified units [u]
   */
  public double getMinAltitude(String u) {
    return parameters_.getMinAltitude(u);
  }

  /** 
   * Get maximum altitude in internal units [m]
   */
  public double getMaxAltitude() {
    return parameters_.getMaxAltitude();
  }

  /** 
   * Get maximum altitude in specified units [u]
   */
  public double getMaxAltitude(String u) {
    return parameters_.getMaxAltitude(u);
  }

  /** 
   * Get track step in internal units [rad]
   */
  public double getTrackStep() {
    return parameters_.getTrackStep();
  }

  /** 
   * Get track step in specified units [u]
   */
  public double getTrackStep(String u) {
    return parameters_.getTrackStep(u);
  }

  /** 
   * Get ground speed step in internal units [m/s]
   */
  public double getGroundSpeedStep() {
    return parameters_.getGroundSpeedStep();
  }

  /** 
   * Get ground speed step in specified units [u]
   */
  public double getGroundSpeedStep(String u) {
    return parameters_.getGroundSpeedStep(u);
  }

  /** 
   * Get vertical speed step in internal units [m/s]
   */
  public double getVerticalSpeedStep() {
    return parameters_.getVerticalSpeedStep();
  }

  /** 
   * Get vertical speed step in specified units [u]
   */
  public double getVerticalSpeedStep(String u) {
    return parameters_.getVerticalSpeedStep(u);
  }

  /** 
   * Get altitude step in internal units [m]
   */
  public double getAltitudeStep() {
    return parameters_.getAltitudeStep();
  }

  /** 
   * Get altitude step in specified units [u]
   */
  public double getAltitudeStep(String u) {
    return parameters_.getAltitudeStep(u);
  }

  /** 
   * Get horizontal acceleration in internal units [m/s^2]
   */
  public double getHorizontalAcceleration() {
    return parameters_.getHorizontalAcceleration();
  }

  /** 
   * Get horizontal acceleration in specified units [u]
   */
  public double getHorizontalAcceleration(String u) {
    return parameters_.getHorizontalAcceleration(u);
  }

  /** 
   * Get vertical acceleration in internal units [m/s^2]
   */
  public double getVerticalAcceleration() {
    return parameters_.getVerticalAcceleration();
  }

  /** 
   * Get vertical acceleration in specified units [u]
   */
  public double getVerticalAcceleration(String u) {
    return parameters_.getVerticalAcceleration(u);
  }

  /** 
   * Get turn rate in internal units [rad/s]
   */
  public double getTurnRate() {
    return parameters_.getTurnRate();
  }

  /** 
   * Get turn rate in specified units [u]
   */
  public double getTurnRate(String u) {
    return parameters_.getTurnRate(u);
  }

  /** 
   * Get bank angle in internal units [rad]
   */
  public double getBankAngle() {
    return parameters_.getBankAngle();
  }

  /** 
   * Get bank angle in specified units [u]
   */
  public double getBankAngle(String u) {
    return parameters_.getBankAngle(u);
  }

  /** 
   * Get vertical rate in internal units [m/s]
   */
  public double getVerticalRate() {
    return parameters_.getVerticalRate();
  }

  /** 
   * Get vertical rate in specified units [u]
   */
  public double getVerticalRate(String u) {
    return parameters_.getVerticalRate(u);
  }

  /** 
   * Get recovery stability time in seconds
   */
  public double getRecoveryStabilityTime() {
    return parameters_.getRecoveryStabilityTime();
  }

  /** 
   * Get recovery stability time in specified units [u]
   */
  public double getRecoveryStabilityTime(String u) {
    return parameters_.getRecoveryStabilityTime(u);
  }

  /** 
   * Get maximum recovery time in seconds
   */
  public double getMaxRecoveryTime() {
    return parameters_.getMaxRecoveryTime();
  }
  /** 
   * Get maximum recovery time in specified units [u]
   */
  public double getMaxRecoveryTime(String u) {
    return parameters_.getMaxRecoveryTime(u);
  }

  /** 
   * Get minimum horizontal recovery distance in internal units [m]
   */
  public double getMinHorizontalRecovery() {
    return parameters_.getMinHorizontalRecovery();
  }

  /** 
   * Get minimum horizontal recovery distance in specified units [u]
   */
  public double getMinHorizontalRecovery(String u) {
    return parameters_.getMinHorizontalRecovery(u);
  }

  /** 
   * Get minimum vertical recovery distance in internal units [m]
   */
  public double getMinVerticalRecovery() {
    return parameters_.getMinVerticalRecovery();
  }

  /** 
   * Get minimum vertical recovery distance in specified units [u]
   */
  public double getMinVerticalRecovery(String u) {
    return parameters_.getMinVerticalRecovery(u);
  }

  /** 
   * Set alerting time to specified value in seconds
   */
  public void setAlertingTime(double val) {
    parameters_.setAlertingTime(val);
  }

  /** 
   * Set alerting time to specified value in specified units [u]
   */
  public void setAlertingTime(double val, String u) {
    parameters_.setAlertingTime(val,u);
  }

  /** 
   * Set lookahead time to specified value in seconds
   */
  public void setLookaheadTime(double val) {
    parameters_.setLookaheadTime(val);
  }

  /** 
   * Set lookahead time to specified value in specified units [u]
   */
  public void setLookaheadTime(double val, String u) {
    parameters_.setLookaheadTime(val,u);
  }

  /** 
   * Set left track to value in internal units [rad]. Value is expected to be in [-pi - 0]
   */
  public void setLeftTrack(double val) {
    parameters_.setLeftTrack(val);
  }

  /** 
   * Set left track to value in specified units [u]. Value is expected to be in [-pi - 0]
   */
  public void setLeftTrack(double val, String u) {
    parameters_.setLeftTrack(val,u);
  }

  /** 
   * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  public void setRightTrack(double val) {
    parameters_.setRightTrack(val);
  }

  /** 
   * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  public void setRightTrack(double val, String u) {
    parameters_.setRightTrack(val,u);
  }

  /** 
   * Set minimum ground speed to specified value in internal units [m/s]
   */
  public void setMinGroundSpeed(double val) {
    parameters_.setMinGroundSpeed(val);
  }

  /** 
   * Set minimum ground speed to specified value in specified units [u]
   */
  public void setMinGroundSpeed(double val, String u) {
    parameters_.setMinGroundSpeed(val,u);
  }

  /** 
   * Set maximum ground speed to specified value in internal units [m/s]
   */
  public void setMaxGroundSpeed(double val) {
    parameters_.setMaxGroundSpeed(val);
  }

  /** 
   * Set maximum ground speed to specified value in specified units [u]
   */
  public void setMaxGroundSpeed(double val, String u) {
    parameters_.setMaxGroundSpeed(val,u);
  }

  /** 
   * Set minimum vertical speed to specified value in internal units [m/s]
   */
  public void setMinVerticalSpeed(double val) {
    parameters_.setMinVerticalSpeed(val);
  }

  /** 
   * Set minimum vertical speed to specified value in specified units [u]
   */
  public void setMinVerticalSpeed(double val, String u) {
    parameters_.setMinVerticalSpeed(val,u);
  }

  /** 
   * Set maximum vertical speed to specified value in internal units [m/s]
   */
  public void setMaxVerticalSpeed(double val) {
    parameters_.setMaxVerticalSpeed(val);
  }

  /** 
   * Set maximum vertical speed to specified value in specified units [u]
   */
  public void setMaxVerticalSpeed(double val, String u) {
    parameters_.setMaxVerticalSpeed(val,u);
  }

  /** 
   * Set minimum altitude to specified value in internal units [m]
   */
  public void setMinAltitude(double val) {
    parameters_.setMinAltitude(val);
  }

  /** 
   * Set minimum altitude to specified value in specified units [u]
   */
  public void setMinAltitude(double val, String u) {
    parameters_.setMinAltitude(val,u);
  }

  /** 
   * Set maximum altitude to specified value in internal units [m]
   */
  public void setMaxAltitude(double val) {
    parameters_.setMaxAltitude(val);
  }

  /** 
   * Set maximum altitude to specified value in specified units [u]
   */
  public void setMaxAltitude(double val, String u) {
    parameters_.setMaxAltitude(val,u);
  }

  /** 
   * Set track step to specified value in internal units [rad]
   */
  public void setTrackStep(double val) {
    parameters_.setTrackStep(val);
  }

  /** 
   * Set track step to specified value in specified units [u]
   */
  public void setTrackStep(double val, String u) {
    parameters_.setTrackStep(val,u);
  }

  /** 
   * Set ground speed step to specified value in internal units [m/s]
   */
  public void setGroundSpeedStep(double val) {
    parameters_.setGroundSpeedStep(val);
  }

  /** 
   * Set ground speed step to specified value in specified units [u]
   */
  public void setGroundSpeedStep(double val, String u) {
    parameters_.setGroundSpeedStep(val,u);
  }

  /** 
   * Set vertical speed step to specified value in internal units [m/s]
   */
  public void setVerticalSpeedStep(double val) {
    parameters_.setVerticalSpeedStep(val);
  }

  /** 
   * Set vertical speed step to specified value in specified units [u]
   */
  public void setVerticalSpeedStep(double val, String u) {
    parameters_.setVerticalSpeedStep(val,u);
  }

  /** 
   * Set altitude step to specified value in internal units [m]
   */
  public void setAltitudeStep(double val) {
    parameters_.setAltitudeStep(val);
  }

  /** 
   * Set altitude step to specified value in specified units [u]
   */
  public void setAltitudeStep(double val, String u) {
    parameters_.setAltitudeStep(val,u);
  }

  /** 
   * Set horizontal acceleration to specified value in internal units [m/s^2]
   */
  public void setHorizontalAcceleration(double val) {
    parameters_.setHorizontalAcceleration(val);
  }

  /** 
   * Set horizontal acceleration to specified value in specified units [u]
   */
  public void setHorizontalAcceleration(double val, String u) {
    parameters_.setHorizontalAcceleration(val,u);
  }

  /** 
   * Set vertical acceleration to specified value in internal units [m/s^2]
   */
  public void setVerticalAcceleration(double val) {
    parameters_.setVerticalAcceleration(val);
  }

  /** 
   * Set vertical acceleration to specified value in specified units [u]
   */
  public void setVerticalAcceleration(double val, String u) {
    parameters_.setVerticalAcceleration(val,u);
  }

  /** 
   * Set turn rate to specified value in internal units [rad/s]. As a side effect, this method
   * resets the bank angle.
   */
  public void setTurnRate(double val) {
    parameters_.setTurnRate(val);
  }

  /** 
   * Set turn rate to specified value in specified units [u]. As a side effect, this method
   * resets the bank angle.
   */
  public void setTurnRate(double val, String u) {
    parameters_.setTurnRate(val,u);
  }

  /** 
   * Set bank angle to specified value in internal units [rad]. As a side effect, this method
   * resets the turn rate.
   */
  public void setBankAngle(double val) {
    parameters_.setBankAngle(val);
  }

  /** 
   * Set bank angle to specified value in specified units [u]. As a side effect, this method
   * resets the turn rate.
   */
  public void setBankAngle(double val, String u) {
    parameters_.setBankAngle(val,u);
  }

  /** 
   * Set vertical rate to specified value in internal units [m/s]
   */
  public void setVerticalRate(double val) {
    parameters_.setVerticalRate(val);
  }

  /** 
   * Set vertical rate to specified value in specified units [u]
   */
  public void setVerticalRate(double val, String u) {
    parameters_.setVerticalRate(val,u);
  }

  /** 
   * Set recovery stability time to specified value in seconds
   */
  public void setRecoveryStabilityTime(double val) {
    parameters_.setRecoveryStabilityTime(val);
  }

  /** 
   * Set recovery stability time to specified value in specified units [u]
   */
  public void setRecoveryStabilityTime(double val, String u) {
    parameters_.setRecoveryStabilityTime(val,u);
  }

  /** 
   * Set maximum recovery time to specified value in seconds
   */
  public void setMaxRecoveryTime(double val) {
    parameters_.setMaxRecoveryTime(val);
  }

  /** 
   * Set maximum recovery time to specified value in specified units [u]
   */
  public void setMaxRecoveryTime(double val, String u) {
    parameters_.setMaxRecoveryTime(val,u);
  }

  /** 
   * Set minimum recovery horizontal distance to specified value in internal units [m]
   */
  public void setMinHorizontalRecovery(double val) {
    parameters_.setMinHorizontalRecovery(val);
  }

  /** 
   * Set minimum recovery horizontal distance to specified value in specified units [u]
   */
  public void setMinHorizontalRecovery(double val, String u) {
    parameters_.setMinHorizontalRecovery(val,u);
  }

  /** 
   * Set minimum recovery vertical distance to specified value in internal units [m]
   */
  public void setMinVerticalRecovery(double val) {
    parameters_.setMinVerticalRecovery(val);
  }

  /** 
   * Set minimum recovery vertical distance to specified value in specified units [u]
   */
  public void setMinVerticalRecovery(double val, String u) {
    parameters_.setMinVerticalRecovery(val,u);
  }

  /**
   * @return strategy for computing most urgent aircraft. 
   */
  public UrgencyStrategy getUrgencyStrategy() { 
    return urgency_strat_;
  }

  /**
   * Set strategy for computing most urgent aircraft.
   */
  public void setUrgencyStrategy(UrgencyStrategy strat) { 
    urgency_strat_ = strat;
  }

  /**
   * Returns most urgent aircraft at current time according to urgency strategy.
   */
  public TrafficState mostUrgentAircraft() {
    if (lastTrafficIndex() > 0) {
      return urgency_strat_.mostUrgentAircraft(detector_,ownship_,traffic_,alertingTime());
    } else {
      return TrafficState.INVALID;
    }
  }

  private static void add_blob(List<List<Position>> blobs, Deque<Position> vin, Deque<Position> vout) {
    if (vin.isEmpty() && vout.isEmpty()) {
      return;
    }
    // Add conflict contour
    List<Position> blob = new ArrayList<Position>(vin);
    blob.addAll(vout);
    blobs.add(blob);
    vin.clear();
    vout.clear();
  }

  /**
   * Computes horizontal contours contributed by aircraft at index ac_idx that are within 
   * the horizontal contour threshold of the ownship current direction. A contour is a non-empty 
   * list of points in counter-clockwise direction representing a polygon.   
   * @param blobs list of track contours returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   */
  public void horizontalContours(List<List<Position>>blobs, int ac_idx) {
    blobs.clear();
    if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
      Deque<Position> vin = new ArrayDeque<Position>();
      Position po = ownship_.getPosition();
      Velocity vo = ownship_.getVelocity();
      TrafficState intruder = traffic_.get(ac_idx-1);
      Vect3 si = intruder.get_s();
      Velocity vi = intruder.get_v();
      double current_trk = vo.trk();
      Deque<Position> vout = new ArrayDeque<Position>();
      /* First step: Computes conflict contour (contour in the current path of the aircraft).
       * Get contour portion to the right.  If los.getTimeIn() == 0, a 360 degree
       * contour will be computed. Otherwise, stops at the first non-conflict degree.
       */
      double right = 0; // Contour conflict limit to the right relative to current track  [0-2pi rad]
      double two_pi = 2*Math.PI;
      for (; right < two_pi; right += parameters_.getTrackStep()) {
        Velocity vop = vo.mkTrk(current_trk+right);
        LossData los = detector_.conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
            0, getLookaheadTime());
        if ( !los.conflict() ) {
          break;
        }
        if (los.getTimeIn() != 0 ) {
          // if not in los, add position at time in (counter clock-wise)
          vin.addLast(po.linear(vop,los.getTimeIn()));
        }
        // in any case, add position ad time out (counter clock-wise)
        vout.addFirst(po.linear(vop,los.getTimeOut()));
      }
      /* Second step: Compute conflict contour to the left */
      double left = 0;  // Contour conflict limit to the left relative to current track [0-2pi rad]
      if (0 < right && right < two_pi) {
        /* There is a conflict contour, but not a violation */
        for (left = parameters_.getTrackStep(); left < two_pi; left += parameters_.getTrackStep()) {
          Velocity vop = vo.mkTrk(current_trk-left);
          LossData los = detector_.conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
              0, getLookaheadTime());
          if ( !los.conflict() ) {
            break;
          }
          vin.addFirst(po.linear(vop,los.getTimeIn()));
          vout.addLast(po.linear(vop,los.getTimeOut()));
        }
      }
      add_blob(blobs,vin,vout);
      // Third Step: Look for other blobs to the right within track threshold
      if (right < parameters_.getHorizontalContourThreshold()) {
        for (; right < two_pi-left; right += parameters_.getTrackStep()) {
          Velocity vop = vo.mkTrk(current_trk+right);
          LossData los = detector_.conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
              0, getLookaheadTime());
          if (los.conflict()) {
            vin.addLast(po.linear(vop,los.getTimeIn()));
            vout.addFirst(po.linear(vop,los.getTimeOut()));
          } else {
            add_blob(blobs,vin,vout);
            if (right >= parameters_.getHorizontalContourThreshold()) {
              break;
            }
          }
        }
        add_blob(blobs,vin,vout);
      }
      // Fourth Step: Look for other blobs to the left within track threshold
      if (left < parameters_.getHorizontalContourThreshold()) {
        for (; left < two_pi-right; left += parameters_.getTrackStep()) {
          Velocity vop = vo.mkTrk(current_trk-left);
          LossData los = detector_.conflictDetection(ownship_.get_s(),ownship_.vel_to_v(po,vop),si,vi,
              0, getLookaheadTime());
          if (los.conflict()) {
            vin.addFirst(po.linear(vop,los.getTimeIn()));
            vout.addLast(po.linear(vop,los.getTimeOut()));
          } else {
            add_blob(blobs,vin,vout);
            if (left >= parameters_.getHorizontalContourThreshold()) {
              break;
            }
          }
        }
        add_blob(blobs,vin,vout);
      }
    } else {
      error.addError("trackContour: aircraft index "+ac_idx+" is out of bounds");
    }
  }

  /** 
   * @return true if repulsive criteria is enabled for conflict bands.
   */
  public boolean isEnabledConflictCriteria() {
    return parameters_.isEnabledConflictCriteria();
  }

  /** 
   * Enable/disable repulsive criteria for conflict bands.
   */
  public void setConflictCriteria(boolean flag) {
    parameters_.setConflictCriteria(flag);
  }

  /** 
   * Enable repulsive criteria for conflict bands.
   */
  public void enableConflictCriteria() {
    parameters_.enableConflictCriteria();
  }

  /** 
   * Disable repulsive criteria for conflict bands.
   */
  public void disableConflictCriteria() {
    parameters_.disableConflictCriteria();;
  }

  /** 
   * @return true if repulsive criteria is enabled for recovery bands.
   */
  public boolean isEnabledRecoveryCriteria() {
    return parameters_.isEnabledRecoveryCriteria();
  }

  /** 
   * Enable/disable repulsive criteria for recovery bands.
   */
  public void setRecoveryCriteria(boolean flag) {
    parameters_.setRecoveryCriteria(flag);
  }

  /** 
   * Enable repulsive criteria for recovery bands.
   */
  public void enableRecoveryCriteria() {
    parameters_.enableRecoveryCriteria();
  }

  /** 
   * Disable repulsive criteria for recovery bands.
   */
  public void disableRecoveryCriteria() {
    parameters_.disableRecoveryCriteria();
  }

  /** 
   * Enable/disable repulsive criteria for conflict and recovery bands.
   */
  public void setRepulsiveCriteria(boolean flag) {
    parameters_.setRepulsiveCriteria(flag);
  }

  /** 
   * Enable repulsive criteria for conflict and recovery bands.
   */
  public void enableRepulsiveCriteria() {
    parameters_.enableRepulsiveCriteria();
  }

  /** 
   * Disable repulsive criteria for conflict and recovery bands.
   */
  public void disableRepulsiveCriteria() {
    parameters_.disableRepulsiveCriteria();
  }

  /**
   * @return true if recovery track bands are enabled.
   */
  public boolean isEnabledRecoveryTrackBands() {
    return parameters_.isEnabledRecoveryTrackBands();
  }

  /**
   * @return true if recovery ground speed bands are enabled.
   */
  public boolean isEnabledRecoveryGroundSpeedBands() {
    return parameters_.isEnabledRecoveryGroundSpeedBands();
  }

  /**
   *  @return true if recovery vertical speed bands are enabled.
   */
  public boolean isEnabledRecoveryVerticalSpeedBands() {
    return parameters_.isEnabledRecoveryVerticalSpeedBands();
  }

  /**
   *  @return true if recovery altitude bands are enabled.
   */
  public boolean isEnabledRecoveryAltitudeBands() {
    return parameters_.isEnabledRecoveryAltitudeBands();
  }

  /** 
   * Sets recovery bands flag for track bands to specified value.
   */ 
  public void setRecoveryTrackBands(boolean flag) {
    parameters_.setRecoveryTrackBands(flag);
  }

  /** 
   * Sets recovery bands flag for ground speed bands to specified value.
   */ 
  public void setRecoveryGroundSpeedBands(boolean flag) {
    parameters_.setRecoveryGroundSpeedBands(flag);
  }

  /** 
   * Sets recovery bands flag for vertical speed bands to specified value.
   */ 
  public void setRecoveryVerticalSpeedBands(boolean flag) {
    parameters_.setRecoveryVerticalSpeedBands(flag);
  }

  /** 
   * Sets recovery bands flag for altitude bands to specified value.
   */ 
  public void setRecoveryAltitudeBands(boolean flag) {
    parameters_.setRecoveryAltitudeBands(flag);
  }

  /** 
   * Enable/disable recovery bands for track, ground speed, and vertical speed.
   */ 
  public void setRecoveryBands(boolean flag) {
    parameters_.setRecoveryBands(flag);
  }

  /** 
   * Enable recovery bands for track, ground speed, and vertical speed.
   */ 
  public void enableRecoveryBands() {
    parameters_.enableRecoveryBands();
  }

  /** 
   * Disable recovery bands for track, ground speed, and vertical speed.
   */ 
  public void disableRecoveryBands() {
    parameters_.disableRecoveryBands();
  }

  /** 
   * @return true if collision avoidance bands are enabled.
   */
  public boolean isEnabledCollisionAvoidanceBands() {
    return parameters_.isEnabledCollisionAvoidanceBands();
  }

  /** 
   * Enable/disable collision avoidance bands.
   */ 
  public void setCollisionAvoidanceBands(boolean flag) {
    parameters_.setCollisionAvoidanceBands(flag);
  }

  /** 
   * Enable collision avoidance bands.
   */ 
  public void enableCollisionAvoidanceBands() {
    parameters_.enableCollisionAvoidanceBands();
  }

  /** 
   * Disable collision avoidance bands.
   */ 
  public void disableCollisionAvoidanceBands() {
    parameters_.disableCollisionAvoidanceBands();
  }

  /** 
   * Get horizontal contour threshold, specified in internal units [rad] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi means all contours.
   */
  public double getHorizontalContourThreshold() {
    return parameters_.getHorizontalContourThreshold();
  }

  /** 
   * Get horizontal contour threshold, specified in given units [u] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi  means all contours.
   */
  public double getHorizontalContourThreshold(String u) {
    return parameters_.getHorizontalContourThreshold(u);
  }

  /** 
   * Set horizontal contour threshold, specified in internal units [rad] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi means all contours.
   */
  public void setHorizontalContourThreshold(double val) {
    parameters_.setHorizontalContourThreshold(val);
  }

  /** 
   * Set horizontal contour threshold, specified in given units [u] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi means all contours.
   */
  public void setHorizontalContourThreshold(double val, String u) {
    parameters_.setHorizontalContourThreshold(val,u);
  }

  /**
   *  Load parameters from file.
   */
  public boolean loadParametersFromFile(String file) {
    boolean b = parameters_.loadFromFile(file);
    return b;
  }

  /**
   *  Write parameters to file.
   */
  public boolean saveParametersToFile(String file) {
    return parameters_.saveToFile(file);
  }

  public void updateParameterData(ParameterData p) {
    parameters_.updateParameterData(p);
  }

  public void setParameters(ParameterData p) {
    parameters_.setParameters(p);
  }

  public String aircraftListToPVS(int prec) {
    return ownship_.listToPVS(traffic_,prec);
  }

  public String toString() {
    String s = "Daidalus Object\n"+"Detector:\n"+detector_.toString()+
        "\nAlertor:\n"+AlertThresholds.listToString(alertor_)+"Parameters:\n###\n"+parameters_.toString();
    if (traffic_.size() > 0) {
      s += "###\nAircraft:\n"+ownship_.formattedTraffic(traffic_,current_time_);
    }
    return s;
  }

}

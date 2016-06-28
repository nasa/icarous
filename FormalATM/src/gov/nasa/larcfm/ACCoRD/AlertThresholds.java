/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.List;

import gov.nasa.larcfm.Util.Interval;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.f;

public class AlertThresholds {

  private Detection3D detector_; // State-based detector  
  private double alerting_time_; // Alerting_time
  // If alerting_time > 0, alert is based on detection
  // If alerting_time = 0, alert is based on violation
  private Interval spread_trk_; // Alert when track band within spread interval. 
  private Interval spread_gs_;  // Alert when ground speed band within spread interval.
  private Interval spread_vs_;  // Alert when vertical speed band within speed interval.
  private Interval spread_alt_; // Alert when altitude  band within spread interval.
  // Spread intervals must include 0 and the low and up values are relative to ownship's state. 


  /** 
   * Creates an alert threholds object. Parameter det is an arbitrary detector,
   * tthr is a non-negative alerting time (possibly positive infinity). 
   */
  public AlertThresholds(Detection3D det, double tthr) {
    detector_ = det.copy();
    alerting_time_ = Math.max(0,tthr);
    spread_trk_ = new Interval(0,0);
    spread_gs_ = new Interval(0,0);
    spread_vs_ = new Interval(0,0);
    spread_alt_ = new Interval(0,0);
  }

  public AlertThresholds(AlertThresholds athr) {
    detector_ = athr.detector_.copy();
    alerting_time_ = athr.alerting_time_;
    spread_trk_ = athr.spread_trk_;
    spread_gs_ = athr.spread_gs_;
    spread_vs_ = athr.spread_vs_;
    spread_alt_ = athr.spread_alt_;    
  }

  private AlertThresholds() {
    detector_ = null;
    alerting_time_ = Double.NaN;
    spread_trk_ = Interval.EMPTY;
    spread_gs_ = Interval.EMPTY;
    spread_vs_ = Interval.EMPTY;
    spread_alt_ = Interval.EMPTY;
  }

  public static final AlertThresholds INVALID = new AlertThresholds();

  public boolean isValid() {
    return detector_ != null && !Double.isNaN(alerting_time_) &&
        !spread_trk_.isEmpty() && !spread_gs_.isEmpty() &&
        !spread_vs_.isEmpty() && !spread_alt_.isEmpty();
  }

  /**
   * Return detector.
   */
  public Detection3D getDetector() {
    return detector_;
  }

  /**
   * Set detector.
   */
  public void setDetector(Detection3D det) {
    this.detector_ = det.copy();
  }

  /**
   * Return alerting time in seconds.
   */
  public double getAlertingTime() {
    return alerting_time_;
  }

  /**
   * Set alerting time. Parameter val is a non-negative value, possibly positive infinity, given in seconds.
   */
  public void setAlertingTime(double val) {
    alerting_time_ = Math.abs(val);
  }

  /**
   * Get left track, in internal units [rad], relative to ownship's track, for alerting.
   */
  public double getSpreadLeftTrack() {
    return spread_trk_.low;
  }

  /**
   * Get left track, in given units [u], relative to ownship's track, for alerting.
   */
  public double getSpreadLeftTrack(String u) {
    return Units.to(u,getSpreadLeftTrack());
  }  

  /**
   * Get right track, in internal units [rad], relative to ownship's track, for alerting.
   */
  public double getSpreadRightTrack() {
    return spread_trk_.up;
  }

  /**
   * Get right track, in given units [u], relative to ownship's track, for alerting.
   */
  public double getSpreadRightTrack(String u) {
    return Units.to(u,getSpreadRightTrack());
  }  

  /** 
   * Set left/right tracks, relative to ownship's track, for alerting. 
   * Tracks are specified in internal units [rad]. Left value is expected to be in [-pi,0].
   * Right value is expected to be in [0,pi] 
   */
  public void setSpreadTrack(double left, double right) {
    left = -Math.abs(left);
    right = Math.abs(right);
    if (left >= -Math.PI && right <= Math.PI) {
      spread_trk_ = new Interval(left,right);
    }     
  }

  /** 
   * Set left/right tracks, relative to ownship's track, for alerting. 
   * Tracks are specified in given units [u]. Left value is expected to be in [-pi,0] [u].
   * Right value is expected to be in [0,pi] [u]. 
   */
  public void setSpreadTrack(double left, double right, String u) {
    setSpreadTrack(Units.from(u,left),Units.from(u,right));
  }

  /**
   * Get below ground speed, in internal units [m/s], relative to ownship's ground speed, for alerting.
   */
  public double getSpreadBelowGroundSpeed() {
    return spread_gs_.low;
  }

  /**
   * Get below ground speed, in given units [u], relative to ownship's ground speed, for alerting.
   */
  public double getSpreadBelowGroundSpeed(String u) {
    return Units.to(u,getSpreadBelowGroundSpeed());
  }  

  /**
   * Get above ground speed, in internal units [m/s], relative to ownship's ground speed, for alerting.
   */
  public double getSpreadAboveGroundSpeed() {
    return spread_gs_.up;
  }

  /**
   * Get above ground speed, in given units [u], relative to ownship's ground speed, for alerting.
   */
  public double getSpreadAboveGroundSpeed(String u) {
    return Units.to(u,getSpreadAboveGroundSpeed());
  }  

  /** 
   * Set below/above ground speeds, relative to ownship's ground speed, for alerting. 
   * Ground speeds are specified in internal units [m/s]. Below value is expected to be non-positive.
   * Above value is expected to be non-negative.
   */
  public void setSpreadGroundSpeed(double below, double above) {
    below = -Math.abs(below);
    above = Math.abs(above);
    spread_gs_ = new Interval(below,above);
  }

  /** 
   * Set below/above ground speeds, relative to ownship's ground speed, for alerting. 
   * Ground speeds are specified in given units [u]. Below value is expected to be non-positive.
   * Above value is expected to be non-negative.
   */
  public void setSpreadGroundSpeed(double below, double above, String u) {
    setSpreadGroundSpeed(Units.from(u,below),Units.from(u,above));
  }

  /**
   * Get below vertical speed, in internal units [m/s], relative to ownship's vertical speed, for alerting.
   */
  public double getSpreadBelowVerticalSpeed() {
    return spread_vs_.low;
  }

  /**
   * Get below vertical speed, in given units [u], relative to ownship's vertical speed, for alerting.
   */
  public double getSpreadBelowVerticalSpeed(String u) {
    return Units.to(u,getSpreadBelowVerticalSpeed());
  }  

  /**
   * Get above vertical speed, in internal units [m/s], relative to ownship's vertical speed, for alerting.
   */
  public double getSpreadAboveVerticalSpeed() {
    return spread_vs_.up;
  }

  /**
   * Get above vertical speed, in given units [u], relative to ownship's vertical speed, for alerting.
   */
  public double getSpreadAboveVerticalSpeed(String u) {
    return Units.to(u,getSpreadAboveVerticalSpeed());
  }  

  /** 
   * Set below/above vertical speeds, relative to ownship's vertical speed, for alerting. 
   * Vertical speeds are specified in internal units [m/s]. Below value is expected to be non-positive.
   * Above value is expected to be non-negative.
   */
  public void setSpreadVerticaSpeed(double below, double above) {
    below = -Math.abs(below);
    above = Math.abs(above);
    spread_vs_ = new Interval(below,above);
  }

  /** 
   * Set below/above vertical speeds, relative to ownship's vertical speed, for alerting. 
   * Vertical speeds are specified in given units [u]. Below value is expected to be non-positive.
   * Above value is expected to be non-negative.
   */
  public void setSpreadVerticalSpeed(double below, double above, String u) {
    setSpreadVerticaSpeed(Units.from(u,below),Units.from(u,above));
  }

  /**
   * Get below altitude, in internal units [m], relative to ownship's altitude, for alerting.
   */
  public double getSpreadBelowAltitude() {
    return spread_alt_.low;
  }

  /**
   * Get below altitude, in given units [u], relative to ownship's altitude, for alerting.
   */
  public double getSpreadBelowAltitude(String u) {
    return Units.to(u,getSpreadBelowAltitude());
  }  

  /**
   * Get above altitude, in internal units [m], relative to ownship's altitude, for alerting.
   */
  public double getSpreadAboveAltitude() {
    return spread_alt_.up;
  }

  /**
   * Get above altitude, in given units [u], relative to ownship's altitude, for alerting.
   */
  public double getSpreadAboveAltitude(String u) {
    return Units.to(u,getSpreadAboveAltitude());
  }  

  /** 
   * Set below/above altitudes, relative to ownship's altitude, for alerting. 
   * Altitudes are specified in internal units [m]. Below value is expected to be non-positive.
   * Above value is expected to be non-negative.
   */
  public void setSpreadAltitude(double below, double above) {
    below = -Math.abs(below);
    above = Math.abs(above);
    spread_alt_ = new Interval(below,above);
  }

  /** 
   * Set below/above altitudes, relative to ownship's altitude, for alerting. 
   * Altitudes are specified in given units [u]. Below value is expected to be non-positive.
   * Above value is expected to be non-negative.
   */
  public void setSpreadAltitude(double below, double above, String u) {
    setSpreadAltitude(Units.from(u,below),Units.from(u,above));
  }

  /** 
   * Return true if and only if threshold values, defining an alerting level, are violated.
   */ 
  public boolean alerting(DaidalusParameters parameters, TrafficState own, TrafficState ac, 
      TrafficState mu_ac, int turning, int accelerating, int climbing) {
    if (isValid()) {
      Vect3 so = own.get_s();
      Velocity vo = own.get_v();
      Vect3 si = ac.get_s();
      Velocity vi = ac.get_v();
      if (detector_.violation(so,vo,si,vi)) {
        return true;
      }
      ConflictData det = detector_.conflictDetection(so,vo,si,vi,0,alerting_time_);
      if (det.conflict()) {
        return true;
      }
      if (!spread_trk_.isSingle() || !spread_gs_.isSingle() ||
          !spread_vs_.isSingle() || !spread_alt_.isSingle()) {
        KinematicBands bands = new KinematicBands(detector_); 
        bands.setOwnship(own);
        bands.setParameters(parameters);
        bands.setMostUrgentAircraft(mu_ac);
        bands.disableRecoveryBands();
        bands.setAlertingTime(0);
        bands.setLookaheadTime(alerting_time_);
        if (!spread_trk_.isSingle()) {
          bands.setLeftTrack(turning <= 0 ? spread_trk_.low : 0);
          bands.setRightTrack(turning >= 0 ? spread_trk_.up : 0);       
          if (bands.trk_band().kinematic_conflict(bands.core(),ac)) {
            return true;
          }
        }
        if (!spread_gs_.isSingle()) {
          bands.setBelowAboveGroundSpeed(accelerating <= 0 ? spread_gs_.low : 0, 
              accelerating >= 0 ? spread_gs_.up : 0);
          if (bands.gs_band().kinematic_conflict(bands.core(),ac)) {
            return true;
          }
        }
        if (!spread_vs_.isSingle()) {
          bands.setBelowAboveVerticalSpeed(climbing <= 0 ? spread_vs_.low : 0, 
              climbing >= 0 ? spread_vs_.up : 0);
          if (bands.vs_band().kinematic_conflict(bands.core(),ac)) {
            return true;
          }
        }
        if (!spread_alt_.isSingle()) {
          bands.setBelowAboveAltitude(climbing <= 0 ? spread_alt_.low : 0, 
              climbing >= 0 ? spread_alt_.up : 0);
          if (bands.alt_band().kinematic_conflict(bands.core(),ac)) {
            return true;
          }
        }
      }
    }
    return false;
  }

  public String toString() {
    return detector_.toString()+", Alerting Time: "+f.Fm1(alerting_time_)+
        " [s], Spread Track: "+spread_trk_.toStringUnits("deg")+
        ", Spread Ground Speed: "+spread_gs_.toStringUnits("knot")+
        ", Spread Vertical Speed: "+spread_vs_.toStringUnits("fpm")+
        ", Spread Altitude: "+spread_alt_.toStringUnits("ft");
  }

  public static String listToString(List<AlertThresholds> l) {
    String s = "";
    for (int i=0; i < l.size(); ++i) {
      s += "Level "+(i+1)+": "+l.get(i).toString()+"\n";
    }
    return s;
  }

  public String toPVS(int prec) {
    return "(# wcv:= "+detector_.toPVS(prec)+
        ", lookahead:= "+f.Fm1(alerting_time_)+
        ", spread_trk:= "+spread_trk_.toPVS(prec)+
        ", spread_gs:= "+spread_gs_.toPVS(prec)+
        ", spread_vs:= "+spread_vs_.toPVS(prec)+
        ", spread_alt:= "+spread_alt_.toPVS(prec)+
        " #)"; 
  }

  public static String listToPVS(List<AlertThresholds> l, int prec) {
    String s = "(: ";
    boolean first = true;
    for (int i=0; i < l.size(); ++i) {
      if (first) {
        first = false;
      } else {
        s += ",";
      }
      s += l.get(i).toPVS(prec);
    }
    return s+" :)";
  }
}

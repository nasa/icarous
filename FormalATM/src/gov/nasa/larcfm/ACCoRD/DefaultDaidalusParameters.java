/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ParameterData;

public class DefaultDaidalusParameters {

  static private DaidalusParameters parameters_ = new DaidalusParameters();

  public static DaidalusParameters getParameters() {
    return parameters_;
  }

  /** 
   * Get default alerting time in seconds
   */
  public static double getAlertingTime() {
    return parameters_.getAlertingTime();
  }

  /** 
   * Get default alerting time in specified units [u]
   */
  public static double getAlertingTime(String u) {
    return parameters_.getAlertingTime(u);
  }

  /** 
   * Get default lookahead time in seconds
   */
  public static double getLookaheadTime() {
    return parameters_.getLookaheadTime();
  }

  /** 
   * Get default lookahead time in specified units [u]
   */
  public static double getLookaheadTime(String u) {
    return parameters_.getLookaheadTime(u);
  }

  /** 
   * @return left track in radians [-pi - 0] [rad] from current ownship's track
   */
  public static double getLeftTrack() {
    return parameters_.getLeftTrack();
  }

  /** 
   * @return left track in specified units [-pi - 0] [u] from current ownship's track
   */
  public static double getLeftTrack(String u) {
    return parameters_.getLeftTrack(u);
  }

  /** 
   * @return right track in radians [0 - pi] [rad] from current ownship's track
   */
  public static double getRightTrack() {
    return parameters_.getRightTrack();
  }

  /** 
   * @return right track in specified units [0 - pi] [u] from current ownship's track
   */
  public static double getRightTrack(String u) {
    return parameters_.getRightTrack(u);
  }

  /** 
   * Get default minimum ground speed in internal units [m/s]
   */
  public static double getMinGroundSpeed() {
    return parameters_.getMinGroundSpeed();
  }

  /** 
   * Get default minimum ground speed in specified units [u]
   */
  public static double getMinGroundSpeed(String u) {
    return parameters_.getMinGroundSpeed(u);
  }

  /** 
   * Get default maximum ground speed in internal units [m/s]
   */
  public static double getMaxGroundSpeed() {
    return parameters_.getMaxGroundSpeed();
  }

  /** 
   * Get default maximum ground speed in specified units [u]
   */
  public static double getMaxGroundSpeed(String u) {
    return parameters_.getMaxGroundSpeed(u);
  }

  /** 
   * Get default minimum vertical speed in internal units [m/s]
   */
  public static double getMinVerticalSpeed() {
    return parameters_.getMinVerticalSpeed();
  }

  /** 
   * Get default minimum vertical speed in specified units [u]
   */
  public static double getMinVerticalSpeed(String u) {
    return parameters_.getMinVerticalSpeed(u);
  }

  /** 
   * Get default maximum vertical speed in internal units [m/s]
   */
  public static double getMaxVerticalSpeed() {
    return parameters_.getMaxVerticalSpeed();
  }

  /** 
   * Get default maximum vertical speed in specified units [u]
   */
  public static double getMaxVerticalSpeed(String u) {
    return parameters_.getMaxVerticalSpeed(u);
  }

  /** 
   * Get default minimum altitude in internal units [m]
   */
  public static double getMinAltitude() {
    return parameters_.getMinAltitude();
  }

  /** 
   * Get default minimum altitude in specified units [u]
   */
  public static double getMinAltitude(String u) {
    return parameters_.getMinAltitude(u);
  }

  /** 
   * Get maximum altitude in internal units [m]
   */
  public static double getMaxAltitude() {
    return parameters_.getMaxAltitude();
  }

  /** 
   * Get maximum altitude in specified units [u]
   */
  public static double getMaxAltitude(String u) {
    return parameters_.getMaxAltitude(u);
  }

  /** 
   * Get default track step in internal units [rad]
   */
  public static double getTrackStep() {
    return parameters_.getTrackStep();
  }

  /** 
   * Get default track step in specified units [u]
   */
  public static double getTrackStep(String u) {
    return parameters_.getTrackStep(u);
  }

  /** 
   * Get default ground speed step in internal units [m/s]
   */
  public static double getGroundSpeedStep() {
    return parameters_.getGroundSpeedStep();
  }

  /** 
   * Get default ground speed step in specified units [u]
   */
  public static double getGroundSpeedStep(String u) {
    return parameters_.getGroundSpeedStep(u);
  }

  /** 
   * Get vertical speed step in internal units [m/s]
   */
  public static double getVerticalSpeedStep() {
    return parameters_.getVerticalSpeedStep();
  }

  /** 
   * Get vertical speed step in specified units [u]
   */
  public static double getVerticalSpeedStep(String u) {
    return parameters_.getVerticalSpeedStep(u);
  }

  /** 
   * Get default altitude step in internal units [m]
   */
  public static double getAltitudeStep() {
    return parameters_.getAltitudeStep();
  }

  /** 
   * Get default altitude step in specified units [u]
   */
  public static double getAltitudeStep(String u) {
    return parameters_.getAltitudeStep(u);
  }

  /** 
   * Get default horizontal acceleration in internal units [m/s^2]
   */
  public static double getHorizontalAcceleration() {
    return parameters_.getHorizontalAcceleration();
  }

  /** 
   * Get default horizontal acceleration in specified units [u]
   */
  public static double getHorizontalAcceleration(String u) {
    return parameters_.getHorizontalAcceleration(u);
  }

  /** 
   * Get default vertical acceleration in internal units [m/s^2]
   */
  public static double getVerticalAcceleration() {
    return parameters_.getVerticalAcceleration();
  }

  /** 
   * Get default vertical acceleration in specified units [u]
   */
  public static double getVerticalAcceleration(String u) {
    return parameters_.getVerticalAcceleration(u);
  }

  /** 
   * Get default turn rate in internal units [rad/s]
   */
  public static double getTurnRate() {
    return parameters_.getTurnRate();
  }

  /** 
   * Get default turn rate in specified units [u]
   */
  public static double getTurnRate(String u) {
    return parameters_.getTurnRate(u);
  }

  /** 
   * Get default bank angle in internal units [rad]
   */
  public static double getBankAngle() {
    return parameters_.getBankAngle();
  }

  /** 
   * Get default bank angle in specified units [u]
   */
  public static double getBankAngle(String u) {
    return parameters_.getBankAngle(u);
  }

  /** 
   * Get default vertical rate in internal units [m/s]
   */
  public static double getVerticalRate() {
    return parameters_.getVerticalRate();
  }

  /** 
   * Get default vertical rate in specified units [u]
   */
  public static double getVerticalRate(String u) {
    return parameters_.getVerticalRate(u);
  }

  /** 
   * Get default recovery stability time in seconds
   */
  public static double getRecoveryStabilityTime() {
    return parameters_.getRecoveryStabilityTime();
  }

  /** 
   * Get default recovery stability time in specified units [u]
   */
  public static double getRecoveryStabilityTime(String u) {
    return parameters_.getRecoveryStabilityTime(u);
  }

  /** 
   * Get default maximum recovery time in seconds
   */
  public static double getMaxRecoveryTime() {
    return parameters_.getMaxRecoveryTime();
  }

  /** 
   * Get default maximum recovery time in specified units [u]
   */
  public static double getMaxRecoveryTime(String u) {
    return parameters_.getMaxRecoveryTime(u);
  }

  /** 
   * Get default minimum horizontal recovery distance in internal units [m]
   */
  public static double getMinHorizontalRecovery() {
    return parameters_.getMinHorizontalRecovery();
  }

  /** 
   * Get default minimum horizontal recovery distance in specified units [u]
   */
  public static double getMinHorizontalRecovery(String u) {
    return parameters_.getMinHorizontalRecovery(u);
  }

  /** 
   * Get default minimum vertical recovery distance in internal units [m]
   */
  public static double getMinVerticalRecovery() {
    return parameters_.getMinVerticalRecovery();
  }

  /** 
   * Get default minimum vertical recovery distance in specified units [u]
   */
  public static double getMinVerticalRecovery(String u) {
    return parameters_.getMinVerticalRecovery(u);
  }

  /** 
   * Set default alerting time to specified value in seconds
   */
  public static void setAlertingTime(double val) {
    parameters_.setAlertingTime(val);
  }

  /** 
   * Set default alerting time to specified value in specified units [u]
   */
  public static void setAlertingTime(double val, String u) {
    parameters_.setAlertingTime(val,u);
  }

  /** 
   * Set default lookahead time to specified value in seconds
   */
  public static void setLookaheadTime(double val) {
    parameters_.setLookaheadTime(val);
  }

  /** 
   * Set default lookahead time to specified value in specified units [u]
   */
  public static void setLookaheadTime(double val, String u) {
    parameters_.setLookaheadTime(val,u);
  }

  /** 
   * Set left track to value in internal units [rad]. Value is expected to be in [-pi - 0]
   */
  public static void setLeftTrack(double val) {
    parameters_.setLeftTrack(val);
  }

  /** 
   * Set left track to value in specified units [u]. Value is expected to be in [-pi - 0]
   */
  public static void setLeftTrack(double val, String u) {
    parameters_.setLeftTrack(val,u);
  }

  /** 
   * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  public static void setRightTrack(double val) {
    parameters_.setRightTrack(val);
  }

  /** 
   * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  public static void setRightTrack(double val, String u) {
    parameters_.setRightTrack(val,u);
  }

  /** 
   * Set default minimum ground speed to specified value in internal units [m/s]
   */
  public static void setMinGroundSpeed(double val) {
    parameters_.setMinGroundSpeed(val);
  }

  /** 
   * Set default minimum ground speed to specified value in specified units [u]
   */
  public static void setMinGroundSpeed(double val, String u) {
    parameters_.setMinGroundSpeed(val,u);
  }

  /** 
   * Set default maximum ground speed to specified value in internal units [m/s]
   */
  public static void setMaxGroundSpeed(double val) {
    parameters_.setMaxGroundSpeed(val);
  }

  /** 
   * Set default maximum ground speed to specified value in specified units [u]
   */
  public static void setMaxGroundSpeed(double val, String u) {
    parameters_.setMaxGroundSpeed(val,u);
  }

  /** 
   * Set default minimum vertical speed to specified value in internal units [m/s]
   */
  public static void setMinVerticalSpeed(double val) {
    parameters_.setMinVerticalSpeed(val);
  }

  /** 
   * Set default minimum vertical speed to specified value in specified units [u]
   */
  public static void setMinVerticalSpeed(double val, String u) {
    parameters_.setMinVerticalSpeed(val,u);
  }

  /** 
   * Set default maximum vertical speed to specified value in internal units [m/s]
   */
  public static void setMaxVerticalSpeed(double val) {
    parameters_.setMaxVerticalSpeed(val);
  }

  /** 
   * Set default maximum vertical speed to specified value in specified units [u]
   */
  public static void setMaxVerticalSpeed(double val, String u) {
    parameters_.setMaxVerticalSpeed(val,u);
  }

  /** 
   * Set default minimum altitude to specified value in internal units [m]
   */
  public static void setMinAltitude(double val) {
    parameters_.setMinAltitude(val);
  }

  /** 
   * Set default minimum altitude to specified value in specified units [u]
   */
  public static void setMinAltitude(double val, String u) {
    parameters_.setMinAltitude(val,u);
  }

  /** 
   * Set default maximum altitude to specified value in internal units [m]
   */
  public static void setMaxAltitude(double val) {
    parameters_.setMaxAltitude(val);
  }

  /** 
   * Set default maximum altitude to specified value in specified units [u]
   */
  public static void setMaxAltitude(double val, String u) {
    parameters_.setMaxAltitude(val,u);
  }

  /** 
   * Set default track step to specified value in internal units [rad]
   */
  public static void setTrackStep(double val) {
    parameters_.setTrackStep(val);
  }

  /** 
   * Set default track step to specified value in specified units [u]
   */
  public static void setTrackStep(double val, String u) {
    parameters_.setTrackStep(val,u);
  }

  /** 
   * Set default ground speed step to specified value in internal units [m/s]
   */
  public static void setGroundSpeedStep(double val) {
    parameters_.setGroundSpeedStep(val);
  }

  /** 
   * Set default ground speed step to specified value in specified units [u]
   */
  public static void setGroundSpeedStep(double val, String u) {
    parameters_.setGroundSpeedStep(val,u);
  }

  /** 
   * Set default vertical speed step to specified value in internal units [m/s]
   */
  public static void setVerticalSpeedStep(double val) {
    parameters_.setVerticalSpeedStep(val);
  }

  /** 
   * Set default vertical speed step to specified value in specified units [u]
   */
  public static void setVerticalSpeedStep(double val, String u) {
    parameters_.setVerticalSpeedStep(val,u);
  }

  /** 
   * Set default altitude step to specified value in internal units [m]
   */
  public static void setAltitudeStep(double val) {
    parameters_.setAltitudeStep(val);
  }

  /** 
   * Set default altitude step to specified value in specified units [u]
   */
  public static void setAltitudeStep(double val, String u) {
    parameters_.setAltitudeStep(val,u);
  }

  /** 
   * Set default horizontal acceleration to specified value in internal units [m/s^2]
   */
  public static void setHorizontalAcceleration(double val) {
    parameters_.setHorizontalAcceleration(val);
  }

  /** 
   * Set default horizontal acceleration to specified value in specified units [u]
   */
  public static void setHorizontalAcceleration(double val, String u) {
    parameters_.setHorizontalAcceleration(val,u);
  }

  /** 
   * Set default vertical acceleration to specified value in internal units [m/s^2]
   */
  public static void setVerticalAcceleration(double val) {
    parameters_.setVerticalAcceleration(val);
  }

  /** 
   * Set default vertical acceleration to specified value in specified units [u]
   */
  public static void setVerticalAcceleration(double val, String u) {
    parameters_.setVerticalAcceleration(val,u);
  }

  /** 
   * Set default turn rate to specified value in internal units [rad/s]. As a side effect, this method
   * resets the bank angle.
   */
  public static void setTurnRate(double val) {
    parameters_.setTurnRate(val);
  }

  /** 
   * Set default turn rate to specified value in specified units [u]. As a side effect, this method
   * resets the bank angle.
   */
  public static void setTurnRate(double val, String u) {
    parameters_.setTurnRate(val,u);
  }

  /** 
   * Set default bank angle to specified value in internal units [rad]. As a side effect, this method
   * resets the turn rate.
   */
  public static void setBankAngle(double val) {
    parameters_.setBankAngle(val);
  }

  /** 
   * Set default bank angle to specified value in specified units [u]. As a side effect, this method
   * resets the turn rate.
   */
  public static void setBankAngle(double val, String u) {
    parameters_.setBankAngle(val,u);
  }

  /** 
   * Set default vertical rate to specified value in internal units [m/s]
   */
  public static void setVerticalRate(double val) {
    parameters_.setVerticalRate(val);
  }

  /** 
   * Set default vertical rate to specified value in specified units [u]
   */
  public static void setVerticalRate(double val, String u) {
    parameters_.setVerticalRate(val,u);
  }

  /** 
   * Set default recovery stability time to specified value in seconds
   */
  public static void setRecoveryStabilityTime(double val) {
    parameters_.setRecoveryStabilityTime(val);
  }

  /** 
   * Set default recovery stability time to specified value in specified units [u]
   */
  public static void setRecoveryStabilityTime(double val, String u) {
    parameters_.setRecoveryStabilityTime(val,u);
  }

  /** 
   * Set default maximum recovery time to specified value in seconds
   */
  public static void setMaxRecoveryTime(double val) {
    parameters_.setMaxRecoveryTime(val);
  }

  /** 
   * Set default maximum recovery time to specified value in specified units [u]
   */
  public static void setMaxRecoveryTime(double val, String u) {
    parameters_.setMaxRecoveryTime(val,u);
  }

  /** 
   * Set default minimum recovery horizontal distance to specified value in internal units [m]
   */
  public static void setMinHorizontalRecovery(double val) {
    parameters_.setMinHorizontalRecovery(val);
  }

  /** 
   * Set default minimum recovery horizontal distance to specified value in specified units [u]
   */
  public static void setMinHorizontalRecovery(double val, String u) {
    parameters_.setMinHorizontalRecovery(val,u);
  }

  /** 
   * Set default minimum recovery vertical distance to specified value in internal units [m]
   */
  public static void setMinVerticalRecovery(double val) {
    parameters_.setMinVerticalRecovery(val);
  }

  /** 
   * Set default minimum recovery vertical distance to specified value in specified units [u]
   */
  public static void setMinVerticalRecovery(double val, String u) {
    parameters_.setMinVerticalRecovery(val,u);
  }

  /** 
   * @return true if repulsive criteria is enabled for conflict bands.
   */
  public static boolean isEnabledConflictCriteria() {
    return parameters_.isEnabledConflictCriteria();
  }

  /** 
   * Enable/disable repulsive criteria for conflict bands.
   */
  public static void setConflictCriteria(boolean flag) {
    parameters_.setConflictCriteria(flag);
  }

  /** 
   * Enable repulsive criteria for conflict bands.
   */
  public static void enableConflictCriteria() {
    parameters_.enableConflictCriteria();
  }

  /** 
   * Disable repulsive criteria for conflict bands.
   */
  public static void disableConflictCriteria() {
    parameters_.disableConflictCriteria();
  }

  /** 
   * @return true if repulsive criteria is enabled for recovery bands.
   */
  public static boolean isEnabledRecoveryCriteria() {
    return parameters_.isEnabledRecoveryCriteria();
  }

  /** 
   * Enable/disable repulsive criteria for recovery bands.
   */
  public static void setRecoveryCriteria(boolean flag) {
    parameters_.setRecoveryCriteria(flag);
  }

  /** 
   * Enable repulsive criteria for recovery bands.
   */
  public static void enableRecoveryCriteria() {
    parameters_.enableRecoveryCriteria();
  }

  /** 
   * Disable repulsive criteria for recovery bands.
   */
  public static void disableRecoveryCriteria() {
    parameters_.disableRecoveryCriteria();
  }

  /** 
   * Enable/disable repulsive criteria for conflict and recovery bands.
   */
  public static void setRepulsiveCriteria(boolean flag) {
    parameters_.setRepulsiveCriteria(flag);
  }

  /** 
   * Enable repulsive criteria for conflict and recovery bands.
   */
  public static void enableRepulsiveCriteria() {
    parameters_.enableRepulsiveCriteria();
  }

  /** 
   * Disable repulsive criteria for conflict and recovery bands.
   */
  public static void disableRepulsiveCriteria() {
    parameters_.disableRepulsiveCriteria();
  }

  /**
   * @return true if recovery track bands are enabled.
   */
  public static boolean isEnabledRecoveryTrackBands() {
    return parameters_.isEnabledRecoveryTrackBands();
  }

  /**
   * @return true if recovery ground speed bands are enabled.
   */
  public static boolean isEnabledRecoveryGroundSpeedBands() {
    return parameters_.isEnabledRecoveryGroundSpeedBands();
  }

  /**
   *  @return true if recovery vertical speed bands are enabled.
   */
  public static boolean isEnabledRecoveryVerticalSpeedBands() {
    return parameters_.isEnabledRecoveryVerticalSpeedBands();
  }

  /**
   *  @return true if recovery altitude bands are enabled.
   */
  public static boolean isEnabledRecoveryAltitudeBands() {
    return parameters_.isEnabledRecoveryAltitudeBands();
  }

  /** 
   * Enable/disable recovery bands for track, ground speed, and vertical speed.
   */ 
  public static void setRecoveryBands(boolean flag) {
    parameters_.setRecoveryBands(flag);
  }

  /** 
   * Enable all recovery bands for track, ground speed, and vertical speed.
   */ 
  public static void enableRecoveryBands() {
    parameters_.enableRecoveryBands();
  }

  /** 
   * Disable all recovery bands for track, ground speed, and vertical speed.
   */ 
  public static void disableRecoveryBands() {
    parameters_.disableRecoveryBands();
  }

  /** 
   * Sets recovery bands flag for track bands to specified value.
   */ 
  public static void setRecoveryTrackBands(boolean flag) {
    parameters_.setRecoveryTrackBands(flag);
  }

  /** 
   * Sets recovery bands flag for ground speed bands to specified value.
   */ 
  public static void setRecoveryGroundSpeedBands(boolean flag) {
    parameters_.setRecoveryGroundSpeedBands(flag);
  }

  /** 
   * Sets recovery bands flag for vertical speed bands to specified value.
   */ 
  public static void setRecoveryVerticalSpeedBands(boolean flag) {
    parameters_.setRecoveryVerticalSpeedBands(flag);
  }

  /** 
   * Sets recovery bands flag for altitude bands to specified value.
   */ 
  public static void setRecoveryAltitudeBands(boolean flag) {
    parameters_.setRecoveryAltitudeBands(flag);
  }

  /** 
   * @return true if collision avoidance bands are enabled.
   */
  public static boolean isEnabledCollisionAvoidanceBands() {
    return parameters_.isEnabledCollisionAvoidanceBands();
  }

  /** 
   * Enable/disable collision avoidance bands.
   */ 
  public static void setCollisionAvoidanceBands(boolean flag) {
    parameters_.setCollisionAvoidanceBands(flag);
  }

  /** 
   * Enable collision avoidance bands.
   */ 
  public static void enableCollisionAvoidanceBands() {
    parameters_.enableCollisionAvoidanceBands();
  }

  /** 
   * Disable collision avoidance bands.
   */ 
  public static void disableCollisionAvoidanceBands() {
    parameters_.disableCollisionAvoidanceBands();
  }

  /** 
   * Get horizontal contour threshold, specified in internal units [rad] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi means all contours.
   */
  public static double getHorizontalContourThreshold() {
    return parameters_.getHorizontalContourThreshold();
  }

  /** 
   * Get horizontal contour threshold, specified in given units [u] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi  means all contours.
   */
  public static double getHorizontalContourThreshold(String u) {
    return parameters_.getHorizontalContourThreshold(u);
  }

  /** 
   * Set horizontal contour threshold, specified in internal units [rad] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi means all contours.
   */
  public static void setHorizontalContourThreshold(double val) {
    parameters_.setHorizontalContourThreshold(val);
  }

  /** 
   * Set horizontal contour threshold, specified in given units [u] as an angle to 
   * the left/right of current aircraft direction. A value of 0 means only conflict contours. 
   * A value of pi means all contours.
   */
  public static void setHorizontalContourThreshold(double val, String u) {
    parameters_.setHorizontalContourThreshold(val,u);
  }

  /**
   *  Load default parameters from file.
   */
  public static boolean loadFromFile(String file) {
    return parameters_.loadFromFile(file);
  }

  /**
   *  Write default parameters to file.
   */
  public static boolean saveToFile(String file) {
    return parameters_.saveToFile(file);
  }

  public static void setParmeters(ParameterData p) {
    parameters_.setParameters(p);
  }

  public static void updateParameterData(ParameterData p) {
    parameters_.updateParameterData(p);
  }

}

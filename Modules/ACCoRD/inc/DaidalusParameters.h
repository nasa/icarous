/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSPARAMETERS_H_
#define DAIDALUSPARAMETERS_H_

#include "ParameterData.h"
#include "ParameterAcceptor.h"
#include "ErrorReporter.h"
#include "Alerter.h"
#include "Function.h"

#include <map>
#include <vector>

namespace larcfm {

class DaidalusParameters : public ErrorReporter, public ParameterAcceptor {

private:

  std::vector<Alerter> alerters_;

  ErrorLog error;

  // Bands
  double lookahead_time_;
  double left_hdir_;
  double right_hdir_;
  double min_hs_;
  double max_hs_;
  double min_vs_;
  double max_vs_;
  double min_alt_;
  double max_alt_;

  // Relative bands
  // The following values specify above and below values for the computation of bands
  // relative to the current value. In general, if above >= 0 and below >= 0 the bands range is
  // [val-below,val+above], where val is current value. The following are special cases,
  // if below < 0 && above >= 0 then [min,val+above]
  // if below >=0 && above < = then [val-below,max]
  // if below == 0 && above == 0] then [min,max]
  double below_relative_hs_;
  double above_relative_hs_;
  double below_relative_vs_;
  double above_relative_vs_;
  double below_relative_alt_;
  double above_relative_alt_;

  // Kinematic bands
  double step_hdir_; // Direction step
  double step_hs_;  // Horizontal speed step
  double step_vs_;  // Vertical speed step
  double step_alt_; // Altitude step
  double horizontal_accel_; // Horizontal acceleration
  double vertical_accel_; // Vertical acceleration
  double turn_rate_; // Turn rate
  double bank_angle_; // Bank angles (only used when turn_rate is 0)
  double vertical_rate_; // Vertical rate

  // Recovery bands
  double min_horizontal_recovery_; // Horizontal distance protected during recovery. TCAS RA DMOD is used this value is 0
  double min_vertical_recovery_; // Vertical distance protected during recovery. TCAS RA ZTHR is used when this value is 0
  bool recovery_hdir_;
  bool recovery_hs_;
  bool recovery_vs_;
  bool recovery_alt_;

  // Collision avoidance bands 
  bool   ca_bands_;  // When true, compute recovery bands until NMAC
  double ca_factor_; // Reduction factor when computing CA bands. It;s a value in [0,1]
  double horizontal_nmac_; // Horizontal Near Mid-Air Collision
  double vertical_nmac_; // Vertical Near Mid-Air Collision

  // Hysteresis and persistence
  double recovery_stability_time_;    // Recovery stability time
  double hysteresis_time_;            // Resolution hysteresis time
  double persistence_time_;           // Persistence time
  bool   bands_persistence_;          // Bands persistence enabled/disabled
  double persistence_preferred_hdir_; // Persistence for preferred horizontal direction resolution
  double persistence_preferred_hs_;   // Persistence for preferred horizontal speed resolution
  double persistence_preferred_vs_;   // Persistence for preferred Max delta for vertical speed resolution
  double persistence_preferred_alt_;  // Persistence for preferred altitude resolution resolution
  int    alerting_m_; // Alerting M parameter of M of N
  int    alerting_n_; // Alerting N parameter of M of N

  // Implicit coordination
  bool conflict_crit_; /* Use criteria for conflict bands */
  bool recovery_crit_; /* Use criteria for recovery bands */

  // Sensor Uncertainty Mitigation
  double h_pos_z_score_; // Number of horizontal position standard deviations
  double h_vel_z_score_min_; // Minimum number of horizontal velocity standard deviations
  double h_vel_z_score_max_; // Maximum number of horizontal velocity standard deviations
  double h_vel_z_distance_; // Distance at which h_vel_z_score scales from min to max as range decreases
  double v_pos_z_score_; // Number of vertical position standard deviations
  double v_vel_z_score_; // Number of vertical velocity standard deviations

  // Contours
  double contour_thr_; // Horizontal threshold, specified as an angle to the left/right of current aircraft direction,
  // for computing horizontal contours. A value of 0 means only conflict contours. A value of pi means all contours.

  // Alerting logic
  bool ownship_centric_alerting_; // When true, alerting logic is ownship-centric.
  // Otherwise, it is intruder-centric.
  BandsRegion::Region corrective_region_; // Corrective region for all alerters.
  // (IMPORTANT: all alerters should declare an alert level with a corrective region!)

  std::map<std::string,std::string> units_;

  static bool initialized;
  static void init();

  /** This method is needed because WCV_TAUMOD_SUM doesn't require the
   *  user to initialize SUM parameters, which may be specified globally.
   */
  void set_alerter_with_SUM_parameters(Alerter& alerter) ;

  /** This method is needed because WCV_TAUMOD_SUM doesn't require the
   *  user to initialize SUM parameters, which may be specified globally.
   */
  void set_alerters_with_SUM_parameters();

public:

  /**
   * DAIDALUS version
   */
  static const std::string VERSION;
  static const INT64FM ALMOST_;

  /**
   * The following method set default output precision and enable/disable trailing zeros.
   * It doesn't affect computations
   */
  static void setDefaultOutputPrecision(int precision, bool trailing_zeros = false);

  DaidalusParameters();

  ~DaidalusParameters();

  typedef std::map< std::string,std::vector<std::string> > aliasestype;
  static const aliasestype& getAliases(); // List of key aliases

  int numberOfAlerters() const;

  const Alerter& getAlerterAt(int i) const;

  int getAlerterIndex(const std::string& id) const;

  void clearAlerters();

  /**
   * Add alerter (if id of alerter already exists, replaces alerter with new one).
   * Return index of added alerter
   */
  int addAlerter(const Alerter& alerter);

  /** 
   * @return lookahead time in seconds.
   */
  double getLookaheadTime() const;

  /** 
   * @return lookahead time in specified units [u].
   */
  double getLookaheadTime(const std::string& u) const;

  double getLeftHorizontalDirection() const;

  double getLeftHorizontalDirection(const std::string& u) const;

  double getRightHorizontalDirection() const;

  double getRightHorizontalDirection(const std::string& u) const;

  double getMinHorizontalSpeed() const;

  double getMinHorizontalSpeed(const std::string& u) const;

  double getMaxHorizontalSpeed() const;

  double getMaxHorizontalSpeed(const std::string& u) const;

  double getMinVerticalSpeed() const;

  double getMinVerticalSpeed(const std::string& u) const;

  double getMaxVerticalSpeed() const;

  double getMaxVerticalSpeed(const std::string& u) const;

  double getMinAltitude() const;

  double getMinAltitude(const std::string& u) const;

  double getMaxAltitude() const;

  double getMaxAltitude(const std::string& u) const;

  double getBelowRelativeHorizontalSpeed() const;

  double getBelowRelativeHorizontalSpeed(const std::string& u) const;

  double getAboveRelativeHorizontalSpeed() const;

  double getAboveRelativeHorizontalSpeed(const std::string& u) const;

  double getBelowRelativeVerticalSpeed() const;

  double getBelowRelativeVerticalSpeed(const std::string& u) const;

  double getAboveRelativeVerticalSpeed() const;

  double getAboveRelativeVerticalSpeed(const std::string& u) const;

  double getBelowRelativeAltitude() const;

  double getBelowRelativeAltitude(const std::string& u) const;

  double getAboveRelativeAltitude() const;

  double getAboveRelativeAltitude(const std::string& u) const;

  double getHorizontalDirectionStep() const;

  double getHorizontalDirectionStep(const std::string& u) const;

  double getHorizontalSpeedStep() const;

  double getHorizontalSpeedStep(const std::string& u) const;

  double getVerticalSpeedStep() const;

  double getVerticalSpeedStep(const std::string& u) const;

  double getAltitudeStep() const;

  double getAltitudeStep(const std::string& u) const;

  double getHorizontalAcceleration() const;

  double getHorizontalAcceleration(const std::string& u) const;

  double getVerticalAcceleration() const;

  double getVerticalAcceleration(const std::string& u) const;

  double getTurnRate() const;

  double getTurnRate(const std::string& u) const;

  double getBankAngle() const;

  double getBankAngle(const std::string& u) const;

  double getVerticalRate() const;

  double getVerticalRate(const std::string& u) const;

  double getHorizontalNMAC() const;

  double getHorizontalNMAC(const std::string& u) const;

  double getVerticalNMAC() const;

  double getVerticalNMAC(const std::string& u) const;

  double getRecoveryStabilityTime() const;

  double getRecoveryStabilityTime(const std::string& u) const;

  /**
   * @return hysteresis time in seconds.
   */
  double getHysteresisTime() const;

  /**
   * @return hysteresis time in specified units [u]
   */
  double getHysteresisTime(const std::string& u) const;

  double getPersistenceTime() const;

  double getPersistenceTime(const std::string& u) const;

  bool   isEnabledBandsPersistence() const;

  void   setBandsPersistence(bool flag);

  void   enableBandsPersistence();

  void   disableBandsPersistence();

  double getPersistencePreferredHorizontalDirectionResolution() const;

  double getPersistencePreferredHorizontalDirectionResolution(const std::string& u) const;

  double getPersistencePreferredHorizontalSpeedResolution() const;

  double getPersistencePreferredHorizontalSpeedResolution(const std::string& u) const;

  double getPersistencePreferredVerticalSpeedResolution() const;

  double getPersistencePreferredVerticalSpeedResolution(const std::string& u) const;

  double getPersistencePreferredAltitudeResolution() const;

  double getPersistencePreferredAltitudeResolution(const std::string& u) const;

  /**
   * @return Alerting parameter m of "M of N" strategy
   */
  int getAlertingParameterM() const;

  /**
   * @return Alerting parameter m of "M of N" strategy
   */
  int getAlertingParameterN() const;

  double getMinHorizontalRecovery() const;

  double getMinHorizontalRecovery(const std::string& u) const;

  double getMinVerticalRecovery() const;

  double getMinVerticalRecovery(const std::string& u) const;

  bool setLookaheadTime(double val);

  bool setLookaheadTime(double val, const std::string& u);

  bool setLeftHorizontalDirection(double val);

  bool setLeftHorizontalDirection(double val, const std::string& u);

  bool setRightHorizontalDirection(double val);

  bool setRightHorizontalDirection(double val, const std::string& u);

  bool setMinHorizontalSpeed(double val);

  bool setMinHorizontalSpeed(double val, const std::string& u);

  bool setMaxHorizontalSpeed(double val);

  bool setMaxHorizontalSpeed(double val, const std::string& u);

  bool setMinVerticalSpeed(double val);

  bool setMinVerticalSpeed(double val, const std::string& u);

  bool setMaxVerticalSpeed(double val);

  bool setMaxVerticalSpeed(double val, const std::string& u);

  bool setMinAltitude(double val);

  bool setMinAltitude(double val, const std::string& u);

  bool setMaxAltitude(double val);

  bool setMaxAltitude(double val, const std::string& u);

  void setBelowRelativeHorizontalSpeed(double val);

  void setBelowRelativeHorizontalSpeed(double val, const std::string& u);

  void setAboveRelativeHorizontalSpeed(double val);

  void setAboveRelativeHorizontalSpeed(double val, const std::string& u);

  void setBelowRelativeVerticalSpeed(double val);

  void setBelowRelativeVerticalSpeed(double val, const std::string& u);

  void setAboveRelativeVerticalSpeed(double val);

  void setAboveRelativeVerticalSpeed(double val, const std::string& u);

  void setBelowRelativeAltitude(double val);

  void setBelowRelativeAltitude(double val, const std::string& u);

  void setAboveRelativeAltitude(double val);

  void setAboveRelativeAltitude(double val, const std::string& u);

  void setBelowToMinRelativeHorizontalSpeed();

  void setAboveToMaxRelativeHorizontalSpeed();

  void setBelowToMinRelativeVerticalSpeed();

  void setAboveToMaxRelativeVerticalSpeed();

  void setBelowToMinRelativeAltitude();

  void setAboveToMaxRelativeAltitude();

  void disableRelativeHorizontalSpeedBands();

  void disableRelativeVerticalSpeedBands();

  void disableRelativeAltitude();

  bool setHorizontalDirectionStep(double val);

  bool setHorizontalDirectionStep(double val, const std::string& u);

  bool setHorizontalSpeedStep(double val);

  bool setHorizontalSpeedStep(double val, const std::string& u);

  bool setVerticalSpeedStep(double val);

  bool setVerticalSpeedStep(double val, const std::string& u);

  bool setAltitudeStep(double val);

  bool setAltitudeStep(double val, const std::string& u);

  bool setHorizontalAcceleration(double val);

  bool setHorizontalAcceleration(double val, const std::string& u);

  bool setVerticalAcceleration(double val);

  bool setVerticalAcceleration(double val, const std::string& u);

private:
  bool set_turn_rate(double val);

  bool set_bank_angle(double val);

  bool set_alerting_parameterM(int m);

  bool set_alerting_parameterN(int n);

public:
  bool setTurnRate(double val);

  bool setTurnRate(double val, const std::string& u);

  bool setBankAngle(double val);

  bool setBankAngle(double val, const std::string& u);

  bool setVerticalRate(double val);

  bool setVerticalRate(double val, const std::string& u);

  bool setHorizontalNMAC(double val);

  bool setHorizontalNMAC(double val, const std::string& u);

  bool setVerticalNMAC(double val);

  bool setVerticalNMAC(double val, const std::string& u);

  bool setRecoveryStabilityTime(double val);

  bool setRecoveryStabilityTime(double val, const std::string& u);

  bool setHysteresisTime(double val);

  bool setHysteresisTime(double val, const std::string& u);

  bool setPersistenceTime(double val);

  bool setPersistenceTime(double val, const std::string& u);

  bool setPersistencePreferredHorizontalDirectionResolution(double val);

  bool setPersistencePreferredHorizontalDirectionResolution(double val, const std::string& u);

  bool setPersistencePreferredHorizontalSpeedResolution(double val);

  bool setPersistencePreferredHorizontalSpeedResolution(double val, const std::string& u);

  bool setPersistencePreferredVerticalSpeedResolution(double val);

  bool setPersistencePreferredVerticalSpeedResolution(double val, const std::string& u);

  bool setPersistencePreferredAltitudeResolution(double val);

  bool setPersistencePreferredAltitudeResolution(double val, const std::string& u);

  /**
   * Set alerting parameters of M of N strategy
   */
  bool setAlertingMofN(int m, int n);

  bool setMinHorizontalRecovery(double val);

  bool setMinHorizontalRecovery(double val, const std::string& u);

  bool setMinVerticalRecovery(double val);

  bool setMinVerticalRecovery(double val, const std::string& u);

  /**
   * @return true if repulsive criteria is enabled for conflict bands.
   */
  bool isEnabledConflictCriteria() const;

  /**
   * Enable/disable repulsive criteria for conflict bands.
   */
  void setConflictCriteria(bool flag);

  /**
   * Enable repulsive criteria for conflict bands.
   */
  void enableConflictCriteria();

  /**
   * Disable repulsive criteria for conflict bands.
   */
  void disableConflictCriteria();

  /**
   * @return true if repulsive criteria is enabled for recovery bands.
   */
  bool isEnabledRecoveryCriteria() const;

  /**
   * Enable/disable repulsive criteria for recovery bands.
   */
  void setRecoveryCriteria(bool flag);

  /**
   * Enable repulsive criteria for recovery bands.
   */
  void enableRecoveryCriteria();
  /**
   * Disable repulsive criteria for recovery bands.
   */
  void disableRecoveryCriteria();

  /**
   * Enable/disable repulsive criteria for conflict and recovery bands.
   */
  void setRepulsiveCriteria(bool flag);

  /**
   * Enable repulsive criteria for conflict and recovery bands.
   */
  void enableRepulsiveCriteria();

  /**
   * Disable repulsive criteria for conflict and recovery bands.
   */
  void disableRepulsiveCriteria();

  /**
   * @return recovery bands flag for track bands.
   */
  bool isEnabledRecoveryHorizontalDirectionBands() const;

  /**
   * @return recovery bands flag for ground speed bands.
   */
  bool isEnabledRecoveryHorizontalSpeedBands() const;

  /**
   * @return true if recovery bands for vertical speed bands is enabled.
   */
  bool isEnabledRecoveryVerticalSpeedBands() const;

  /**
   * @return true if recovery bands for altitude bands is enabled.
   */
  bool isEnabledRecoveryAltitudeBands() const;

  /**
   * Enable/disable recovery bands for track, ground speed, vertical speed, and altitude.
   */
  void setRecoveryBands(bool flag);

  /**
   * Enable all recovery bands for track, ground speed, vertical speed, and altitude.
   */ 
  void enableRecoveryBands();

  /** 
   * Disable all recovery bands for track, ground speed, vertical speed, and altitude.
   */
  void disableRecoveryBands();

  /**
   * Sets recovery bands flag for track bands to value.
   */ 
  void setRecoveryHorizontalDirectionBands(bool flag);

  /** 
   * Sets recovery bands flag for ground speed bands to value.
   */ 
  void setRecoveryHorizontalSpeedBands(bool flag);

  /** 
   * Sets recovery bands flag for vertical speed bands to value.
   */ 
  void setRecoveryVerticalSpeedBands(bool flag);

  /**
   * Sets recovery bands flag for altitude bands to value.
   */
  void setRecoveryAltitudeBands(bool flag);

  /**
   * @return true if collision avoidance bands are enabled.
   */
  bool isEnabledCollisionAvoidanceBands() const;

  /**
   * Enable/disable collision avoidance bands.
   */
  void setCollisionAvoidanceBands(bool flag);

  /**
   * Enable collision avoidance bands.
   */
  void enableCollisionAvoidanceBands();

  /**
   * Disable collision avoidance bands.
   */
  void disableCollisionAvoidanceBands();

  /**
   * @return get factor for computing collision avoidance bands. Factor value is in (0,1]
   */
  double getCollisionAvoidanceBandsFactor() const;

  /**
   * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
   */
  bool setCollisionAvoidanceBandsFactor(double val);

  double getHorizontalPositionZScore() const;

  bool setHorizontalPositionZScore(double val);

  double getHorizontalVelocityZScoreMin() const;

  bool setHorizontalVelocityZScoreMin(double val);

  double getHorizontalVelocityZScoreMax() const;

  bool setHorizontalVelocityZScoreMax(double val);

  double getHorizontalVelocityZDistance() const;

  bool setHorizontalVelocityZDistance(double val);

  double getHorizontalVelocityZDistance(const std::string& u) const;

  bool setHorizontalVelocityZDistance(double val, const std::string& u);

  double getVerticalPositionZScore() const;

  bool setVerticalPositionZScore(double val);

  double getVerticalSpeedZScore() const;

  bool setVerticalSpeedZScore(double val);

  /**
   * Get horizontal contour threshold, specified in internal units [rad] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  double getHorizontalContourThreshold() const;

  /**
   * Get horizontal contour threshold, specified in given units [u] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  double getHorizontalContourThreshold(const std::string& u) const;

  /**
   * Set horizontal contour threshold, specified in internal units [rad] [0 - pi] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  bool setHorizontalContourThreshold(double val);

  /**
   * Set horizontal contour threshold, specified in given units [u] [0 - pi] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  bool setHorizontalContourThreshold(double val, const std::string& u);

  void setAlertingLogic(bool ownship_centric);

  void setOwnshipCentricAlertingLogic();

  void setIntruderCentricAlertingLogic();

  bool isAlertingLogicOwnshipCentric() const;

  BandsRegion::Region getCorrectiveRegion() const;

  bool setCorrectiveRegion(BandsRegion::Region val);

  int correctiveAlertLevel(int alerter_idx);

  int maxAlertLevel() const;

  /**
   * Set instantaneous bands.
   */
  void setInstantaneousBands() ;

  /**
   * Set kinematic bands.
   * Set turn rate to 3 deg/s, when type is true; set turn rate to  1.5 deg/s
   * when type is false;
   */
  void setKinematicBands(bool type);

  void disableHysteresis();

  /**
   *  Load parameters from file.
   */
  bool loadFromFile(const std::string& file);

  /**
   *  Write parameters to file.
   */
  bool saveToFile(const std::string& file);

  std::string toString() const;

  std::string toPVS() const;

  ParameterData getParameters() const;

  void updateParameterData(ParameterData& parameters) const;

private:

  /**
   * Return a ParameterData suitable to be read by readAlerterList() based on the supplied Alerter instances.
   * @param alerters list of Alerter instances
   * @param ordered true to modify identifiers to ensure they retain the input list's ordering when decoded, false will only modify identifiers if they are not unique
   * This is a cosmetic alteration to allow for the string representation to have parameters grouped together.
   */
  void writeAlerterList(ParameterData& p) const;

  /**
   * This parses parameters involved with a list of Alerter objects, if properly specified.  The list will be sorted by the instance identifiers.
   *
   * Specifically this looks for parameters:
   * alerterNameList : create Alerter objects for each item in the list
   * XXX_* : parameters associated with Alerter with id "XXX"
   *
   * The user still needs to assign these to the appropriate object(s).
   * If no alternates are loaded, return the empty list.
   *
   * If verbose is false (default true), suppress all status messages except exceptions
   */
  void readAlerterList(const std::vector<std::string>& alerter_list, const ParameterData& params);

  static bool contains(const ParameterData& p,const std::string& key);

  static std::string getUnit(const ParameterData& p,const std::string& key);

  static double getValue(const ParameterData& p,const std::string& key);

  static bool getBool(const ParameterData& p,const std::string& key);

  static int getInt(const ParameterData& p,const std::string& key);

  static std::string getString(const ParameterData& p,const std::string& key);

  static std::vector<std::string> getListString(const ParameterData& p,const std::string& key);

public:
  bool setParameterData(const ParameterData& p);

  void setParameters(const ParameterData& p);

  std::string getUnitsOf(const std::string& key) const;

  bool hasError() const;

  bool hasMessage() const;

  std::string getMessage();

  std::string getMessageNoClear() const;

};

template <typename T>T parameter_data(const ParameterData& p, const std::string& key, Function<const std::string&,T> &f){
  T default_val = f.apply(key);
  if (p.contains(key)) {
    return default_val;
  }
  DaidalusParameters::aliasestype::const_iterator find_ptr = DaidalusParameters::getAliases().find(key);
  if (find_ptr == DaidalusParameters::getAliases().end()) {
    return default_val;
  }
  std::vector<std::string>::const_iterator alias_ptr;
  for (alias_ptr = find_ptr->second.begin(); alias_ptr != find_ptr->second.end(); ++alias_ptr) {
    if (p.contains(*alias_ptr)) {
      return f.apply(*alias_ptr);
    }
  }
  return default_val;
}

}
#endif

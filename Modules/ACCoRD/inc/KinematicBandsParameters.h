/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICBANDSPARAMETERS_H_
#define KINEMATICBANDSPARAMETERS_H_

#include "ParameterData.h"
#include "ParameterAcceptor.h"
#include "ErrorReporter.h"
#include "AlertLevels.h"
#include <map>

namespace larcfm {

class KinematicBandsParameters : public ErrorReporter, public ParameterAcceptor {

private:

  ErrorLog error;

  // Bands
  double lookahead_time_;
  double left_trk_;
  double right_trk_;
  double min_gs_;
  double max_gs_;
  double min_vs_;
  double max_vs_;
  double min_alt_;
  double max_alt_;

  // Kinematic bands
  double trk_step_;
  double gs_step_;
  double vs_step_;
  double alt_step_;
  double horizontal_accel_;
  double vertical_accel_;
  double turn_rate_;
  double bank_angle_;
  double vertical_rate_;

  // Recovery bands
  double horizontal_nmac_; // Horizontal Near Mid-Air Collision
  double vertical_nmac_; // Vertical Near Mid-Air Collision
  double recovery_stability_time_;
  double min_horizontal_recovery_;
  double min_vertical_recovery_;
  bool conflict_crit_; /* Use criteria for conflict bands */
  bool recovery_crit_; /* Use criteria for recovery bands */
  /* Compute recovery bands */
  bool recovery_trk_;
  bool recovery_gs_;
  bool recovery_vs_;
  bool recovery_alt_;
  /* Compute collision avoidance bands */
  bool ca_bands_;  // When true, compute recovery bands until NMAC
  double ca_factor_; // Reduction factor when computing CA bands. It;s a value in [0,1]

  // Contours
  double contour_thr_; // Horizontal threshold, specified as an angle to the left/right of current aircraft direction,
  // for computing horizontal contours. A value of 0 means only conflict contours. A value of pi means all contours.

  std::map<std::string,std::string> units_;
  std::vector<std::string> keys_; // List of key values
  bool set_turn_rate(double val);
  bool set_bank_angle(double val);
  void addKeyDouble(const std::string& key, double val, const std::string& units, const std::string& msg="");
  void addKeyBoolean(const std::string& key, bool val, const std::string& msg="");


public:

  /**
   * DAIDALUS version
   */
  static const std::string VERSION;

  /**
   * Alertor
   */
  AlertLevels alertor;

  KinematicBandsParameters();

  KinematicBandsParameters(const KinematicBandsParameters& parameters);

  ~KinematicBandsParameters();

  KinematicBandsParameters& operator=(const KinematicBandsParameters& parameters);

  /**
   * Set kinematic bands parameters
   */
  void setKinematicBandsParameters(const KinematicBandsParameters& parameters);

  /** 
   * @return lookahead time in seconds.
   */
  double getLookaheadTime() const;

  /** 
   * @return lookahead time in specified units [u].
   */
  double getLookaheadTime(const std::string& u) const;

  /** 
   * @return left track in radians [0 - pi] [rad] from current ownship's track
   */
  double getLeftTrack() const;

  /**
   * @return left track in specified units [0 - pi] [u] from current ownship's track
   */
  double getLeftTrack(const std::string& u) const;

  /**
   * @return right track in radians [0 - pi] [rad] from current ownship's track
   */
  double getRightTrack() const;

  /**
   * @return right track in specified units [0 - pi] [u] from current ownship's track
   */
  double getRightTrack(const std::string& u) const;

  /**
   * @return minimum ground speed in internal units [m/s].
   */
  double getMinGroundSpeed() const;

  /** 
   * @return minimum ground speed in specified units [u].
   */
  double getMinGroundSpeed(const std::string& u) const;

  /** 
   * @return maximum ground speed in internal units [m/s].
   */
  double getMaxGroundSpeed() const;

  /** 
   * @return maximum ground speed in specified units [u].
   */
  double getMaxGroundSpeed(const std::string& u) const;

  /** 
   * @return minimum vertical speed in internal units [m/s].
   */
  double getMinVerticalSpeed() const;

  /** 
   * @return minimum vertical speed in specified units [u].
   */
  double getMinVerticalSpeed(const std::string& u) const;

  /** 
   * @return maximum vertical speed in internal units [m/s].
   */
  double getMaxVerticalSpeed() const;

  /** 
   * @return maximum vertical speed in specified units [u].
   */
  double getMaxVerticalSpeed(const std::string& u) const;

  /** 
   * @return minimum altitude in internal units [m].
   */
  double getMinAltitude() const;

  /** 
   * @return minimum altitude in specified units [u].
   */
  double getMinAltitude(const std::string& u) const;

  /** 
   * @return maximum altitude in internal units [m].
   */
  double getMaxAltitude() const;

  /** 
   * @return maximum altitude in specified units [u].
   */
  double getMaxAltitude(const std::string& u) const;

  /** 
   * @return track step in internal units [rad].
   */
  double getTrackStep() const;

  /** 
   * @return track step in specified units [u].
   */
  double getTrackStep(const std::string& u) const;

  /** 
   * @return ground speed step in internal units [m/s].
   */
  double getGroundSpeedStep() const;

  /** 
   * @return ground speed step in specified units [u].
   */
  double getGroundSpeedStep(const std::string& u) const;

  /** 
   * @return vertical speed step in internal units [m/s].
   */
  double getVerticalSpeedStep() const;

  /** 
   * @return vertical speed step in specified units [u].
   */
  double getVerticalSpeedStep(const std::string& u) const;

  /** 
   * @return altitude step in internal units [m].
   */
  double getAltitudeStep() const;

  /** 
   * @return altitude step in specified units [u].
   */
  double getAltitudeStep(const std::string& u) const;

  /** 
   * @return horizontal acceleration in internal units [m/s^2].
   */
  double getHorizontalAcceleration() const;

  /** 
   * @return horizontal acceleration in specified units [u].
   */
  double getHorizontalAcceleration(const std::string& u) const;

  /** 
   * @return vertical acceleration in internal units [m/s^2].
   */
  double getVerticalAcceleration() const;

  /** 
   * @return vertical acceleration in specified units [u].
   */
  double getVerticalAcceleration(const std::string& u) const;

  /** 
   * @return turn rate in internal units [rad/s].
   */
  double getTurnRate() const;

  /** 
   * @return turn rate in specified units [u].
   */
  double getTurnRate(const std::string& u) const;

  /** 
   * @return bank angle in internal units [rad].
   */
  double getBankAngle() const;

  /** 
   * @return bank angle in specified units [u].
   */
  double getBankAngle(const std::string& u) const;

  /** 
   * @return vertical rate in internal units [m/s].
   */
  double getVerticalRate() const;

  /** 
   * @return vertical rate in specified units [u].
   */
  double getVerticalRate(const std::string& u) const;

  /** 
   * @return horizontal NMAC distance in internal units [m].
   */
  double getHorizontalNMAC() const;

  /**
   * @return horizontal NMAC distance in specified units [u].
   */
  double getHorizontalNMAC(const std::string& u) const;

  /**
   * @return vertical NMAC distance in internal units [m].
   */
  double getVerticalNMAC() const;

  /**
   * @return vertical NMAC distance in specified units [u].
   */
  double getVerticalNMAC(const std::string& u) const;

  /**
   * @return recovery stability time in seconds.
   */
  double getRecoveryStabilityTime() const;

  /** 
   * @return recovery stability time in specified units [u].
   */
  double getRecoveryStabilityTime(const std::string& u) const;

  /** 
   * @return minimum horizontal recovery distance in internal units [m].
   */
  double getMinHorizontalRecovery() const;

  /** 
   * @return minimum horizontal recovery distance in specified units [u].
   */
  double getMinHorizontalRecovery(const std::string& u) const;

  /** 
   * @return minimum vertical recovery distance in internal units [m].
   */
  double getMinVerticalRecovery() const;

  /** 
   * @return minimum vertical recovery distance in specified units [u].
   */
  double getMinVerticalRecovery(const std::string& u) const;

  /** 
   * Set lookahead time to value in seconds.
   */
  bool setLookaheadTime(double val);

  /** 
   * Set lookahead time to value in specified units [u].
   */
  bool setLookaheadTime(double val, const std::string& u);

  /** 
   * Set left track to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  bool setLeftTrack(double val);

  /**
   * Set left track to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  bool setLeftTrack(double val, const std::string& u);

  /**
   * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  bool setRightTrack(double val);

  /**
   * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  bool setRightTrack(double val, const std::string& u);

  /**
   * Set minimum ground speed to value in internal units [m/s].
   * Minimum ground speed must be greater than ground speed step.
   */
  bool setMinGroundSpeed(double val);

  /** 
   * Set minimum ground speed to value in specified units [u].
   * Minimum ground speed must be greater than ground speed step.
   */
  bool setMinGroundSpeed(double val, const std::string& u);

  /** 
   * Set maximum ground speed to value in internal units [m/s].
   */
  bool setMaxGroundSpeed(double val);

  /** 
   * Set maximum ground speed to value in specified units [u].
   */
  bool setMaxGroundSpeed(double val, const std::string& u);

  /** 
   * Set minimum vertical speed to value in internal units [m/s].
   */
  bool setMinVerticalSpeed(double val);

  /** 
   * Set minimum vertical speed to value in specified units [u].
   */
  bool setMinVerticalSpeed(double val, const std::string& u);

  /** 
   * Set maximum vertical speed to value in internal units [m/s].
   */
  bool setMaxVerticalSpeed(double val);

  /** 
   * Set maximum vertical speed to value in specified units [u].
   */
  bool setMaxVerticalSpeed(double val, const std::string& u);

  /** 
   * Set minimum altitude to value in internal units [m].
   */
  bool setMinAltitude(double val);

  /** 
   * Set minimum altitude to value in specified units [u].
   */
  bool setMinAltitude(double val, const std::string& u);

  /** 
   * Set maximum altitude to value in internal units [m].
   */
  bool setMaxAltitude(double val);

  /** 
   * Set maximum altitude to value in specified units [u].
   */
  bool setMaxAltitude(double val, const std::string& u);

  /** 
   * Set track step to value in internal units [rad].
   */
  bool setTrackStep(double val);

  /** 
   * Set track step to value in specified units [u].
   */
  bool setTrackStep(double val, const std::string& u);

  /** 
   * Set ground speed step to value in internal units [m/s].
   */
  bool setGroundSpeedStep(double val);

  /** 
   * Set ground speed step to value in specified units [u].
   */
  bool setGroundSpeedStep(double val, const std::string& u);

  /** 
   * Set vertical speed step to value in internal units [m/s].
   */
  bool setVerticalSpeedStep(double val);

  /** 
   * Set vertical speed step to value in specified units [u].
   */
  bool setVerticalSpeedStep(double val, const std::string& u);

  /** 
   * Set altitude step to value in internal units [m].
   */
  bool setAltitudeStep(double val);

  /** 
   * Set altitude step to value in specified units [u].
   */
  bool setAltitudeStep(double val, const std::string& u);

  /** 
   * Set horizontal acceleration to value in internal units [m/s^2].
   */
  bool setHorizontalAcceleration(double val);

  /** 
   * Set horizontal acceleration to value in specified units [u].
   */
  bool setHorizontalAcceleration(double val, const std::string& u);

  /** 
   * Set vertical acceleration to value in internal units [m/s^2].
   */
  bool setVerticalAcceleration(double val);

  /** 
   * Set vertical acceleration to value in specified units [u].
   */
  bool setVerticalAcceleration(double val, const std::string& u);

  /** 
   * Set turn rate to value in internal units [rad/s]. As a side effect, this method
   * resets the bank angle.
   */
  bool setTurnRate(double val);

  /** 
   * Set turn rate to value in specified units [u]. As a side effect, this method
   * resets the bank angle.
   */
  bool setTurnRate(double val, const std::string& u);

  /** 
   * Set bank angle to value in internal units [rad]. As a side effect, this method
   * resets the turn rate.
   */
  bool setBankAngle(double val);

  /** 
   * Set bank angle to value in specified units [u]. As a side effect, this method
   * resets the turn rate.
   */
  bool setBankAngle(double val, const std::string& u);

  /** 
   * Set vertical rate to value in internal units [m/s].
   */
  bool setVerticalRate(double val);

  /** 
   * Set vertical rate to value in specified units [u].
   */
  bool setVerticalRate(double val, const std::string& u);

  /**
   * Set horizontal NMAC distance to value in internal units [m].
   */
  bool setHorizontalNMAC(double val);

  /**
   * Set horizontal NMAC distance to value in specified units [u].
   */
  bool setHorizontalNMAC(double val, const std::string& u);

  /**
   * Set vertical NMAC distance to value in internal units [m].
   */
  bool setVerticalNMAC(double val);

  /**
   * Set vertical NMAC distance to value in specified units [u].
   */
  bool setVerticalNMAC(double val, const std::string& u);

  /**
   * Set recovery stability time to value in seconds.
   */
  bool setRecoveryStabilityTime(double val);

  /** 
   * Set recovery stability time to value in specified units [u].
   */
  bool setRecoveryStabilityTime(double val, const std::string& u);

  /** 
   * Set minimum recovery horizontal distance to value in internal units [m].
   */
  bool setMinHorizontalRecovery(double val);

  /** 
   * Set minimum recovery horizontal distance to value in specified units [u].
   */
  bool setMinHorizontalRecovery(double val, const std::string& u);

  /** 
   * Set minimum recovery vertical distance to value in internal units [m].
   */
  bool setMinVerticalRecovery(double val);

  /** 
   * Set minimum recovery vertical distance to value in specified units [u].
   */
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
  bool isEnabledRecoveryTrackBands() const;

  /**
   * @return recovery bands flag for ground speed bands.
   */
  bool isEnabledRecoveryGroundSpeedBands() const;

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
  void setRecoveryTrackBands(bool flag);

  /** 
   * Sets recovery bands flag for ground speed bands to value.
   */ 
  void setRecoveryGroundSpeedBands(bool flag);

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

  /**
   *  Load parameters from file.
   */
  bool loadFromFile(const std::string& file);

  /**
   *  Write parameters to file.
   */
  bool saveToFile(const std::string& file) const;

  std::string toString() const;

  std::string toPVS(int prec) const;

  ParameterData getParameters() const;

  void updateParameterData(ParameterData& parameters) const;

  void setParameters(const ParameterData& parameters);

  std::string getUnits(const std::string& key) const;

  bool hasError() const;

  bool hasMessage() const;

  std::string getMessage();

  std::string getMessageNoClear() const;

};

}
#endif

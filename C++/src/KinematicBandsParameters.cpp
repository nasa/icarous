/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicBandsParameters.h"
#include "ParameterData.h"
#include "StateReader.h"
#include "Units.h"
#include "format.h"
#include "AlertLevels.h"
#include "ACCoRDConfig.h"

namespace larcfm {

/**
 * DAIDALUS version
 */
const std::string KinematicBandsParameters::VERSION = "1.a2";

/* NOTE: By default, no alert levels are configured */
KinematicBandsParameters::KinematicBandsParameters() : error("DaidalusParameters") {
  // Bands
  lookahead_time_ = 180;              // [s] Lookahead time
  left_trk_ =  Pi;                    // Left track [0 - pi]
  right_trk_ = Pi;                    // Right track [0 - pi]
  min_gs_ = Units::from("knot",10);   // Minimum ground speed
  max_gs_ = Units::from("knot",700);  // Maximum ground speed
  min_vs_ = Units::from("fpm",-5000); // Minimum vertical speed
  max_vs_ = Units::from("fpm",5000);  // Maximum vertical speed
  min_alt_ = Units::from("ft",500);   // Minimum altitude
  max_alt_ = Units::from("ft",50000); // Maximum altitude

  // Kinematic bands
  trk_step_         = Units::from("deg",  1.0); // Track step
  gs_step_          = Units::from("knot", 1.0); // Ground speed step
  vs_step_          = Units::from("fpm", 10.0); // Vertical speed step
  alt_step_         = Units::from("ft", 100.0); // Altitude step
  horizontal_accel_ = Units::from("m/s^2",2.0); // Horizontal acceleration
  vertical_accel_   = Units::from("G",0.25);    // Section 1.2.3, DAA MOPS V3.6
  turn_rate_        = Units::from("deg/s",3.0); // Section 1.2.3, DAA MOPS V3.6
  bank_angle_       = 0.0;
  vertical_rate_    = Units::from("fpm",500);   // Section 1.2.3, DAA MOPS V3.6

  // Recovery bands
  horizontal_nmac_ = ACCoRDConfig::NMAC_D;
  vertical_nmac_   = ACCoRDConfig::NMAC_H;
  recovery_stability_time_ = 2; // Recovery stability time
  min_horizontal_recovery_ = 0; // Horizontal distance protected during recovery. TCAS RA DMOD is used this value is 0
  min_vertical_recovery_ = 0; // Vertical distance protected during recovery. TCAS RA ZTHR is used when this value is 0
  conflict_crit_ = false;
  recovery_crit_ = false;
  /* Compute recovery bands */
  recovery_trk_ = true;
  recovery_gs_  = true;
  recovery_vs_  = true;
  recovery_alt_ = true;
  /* Compute collision avoidance bands */
  ca_bands_     = false;
  ca_factor_    = 0.2;

  // Contours
  contour_thr_ = Pi;

  // Alert levels
  alertor = AlertLevels();
}

KinematicBandsParameters::KinematicBandsParameters(const KinematicBandsParameters& parameters) : error("DaidalusParameters") {
  setKinematicBandsParameters(parameters);
}

KinematicBandsParameters::~KinematicBandsParameters() {}

KinematicBandsParameters& KinematicBandsParameters::operator=(const KinematicBandsParameters& parameters) {
  setKinematicBandsParameters(parameters);
  return *this;
}

/**
 * Set kinematic bands parameters
 */
void KinematicBandsParameters::setKinematicBandsParameters(const KinematicBandsParameters& parameters) {
  // Bands
  lookahead_time_ = parameters.lookahead_time_;
  left_trk_ = parameters.left_trk_;
  right_trk_ = parameters.right_trk_;
  min_gs_ = parameters.min_gs_;
  max_gs_ = parameters.max_gs_;
  min_vs_ = parameters.min_vs_;
  max_vs_ = parameters.max_vs_;
  min_alt_ = parameters.min_alt_;
  max_alt_ = parameters.max_alt_;

  // Kinematic bands
  trk_step_         = parameters.trk_step_;
  gs_step_          = parameters.gs_step_;
  vs_step_          = parameters.vs_step_;
  alt_step_         = parameters.alt_step_;
  horizontal_accel_ = parameters.horizontal_accel_;
  vertical_accel_   = parameters.vertical_accel_;
  turn_rate_        = parameters.turn_rate_;
  bank_angle_       = parameters.bank_angle_;
  vertical_rate_    = parameters.vertical_rate_;

  // Recovery bands
  horizontal_nmac_         = parameters.horizontal_nmac_;
  vertical_nmac_           = parameters.vertical_nmac_;
  recovery_stability_time_ = parameters.recovery_stability_time_;
  min_horizontal_recovery_ = parameters.min_horizontal_recovery_;
  min_vertical_recovery_ = parameters.min_vertical_recovery_;
  conflict_crit_ = parameters.conflict_crit_;
  recovery_crit_ = parameters.recovery_crit_;
  recovery_trk_ = parameters.recovery_trk_;
  recovery_gs_ = parameters.recovery_gs_;
  recovery_vs_ = parameters.recovery_vs_;
  recovery_alt_ = parameters.recovery_alt_;
  ca_bands_ = parameters.ca_bands_;
  ca_factor_ = parameters.ca_factor_;

  // Contours
  contour_thr_ = parameters.contour_thr_;

  // Alert levels
  alertor = AlertLevels(parameters.alertor);
}

/**
 * Returns lookahead time in seconds.
 */
double KinematicBandsParameters::getLookaheadTime()  const {
  return lookahead_time_;
}

/**
 * Returns lookahead time in specified units [u]
 */
double KinematicBandsParameters::getLookaheadTime(const std::string& u)  const {
  return Units::to(u,getLookaheadTime());
}

/**
 * @return left track in radians [0 - pi] [rad] from current ownship's track
 */
double KinematicBandsParameters::getLeftTrack() const {
  return left_trk_;
}

/**
 * @return left track in specified units [0 - pi] [u] from current ownship's track
 */
double KinematicBandsParameters::getLeftTrack(const std::string& u) const {
  return Units::to(u,getLeftTrack());
}

/**
 * @return right track in radians [0 - pi] [rad] from current ownship's track
 */
double KinematicBandsParameters::getRightTrack() const {
  return right_trk_;
}

/**
 * @return right track in specified units [0 - pi] [u] from current ownship's track
 */
double KinematicBandsParameters::getRightTrack(const std::string& u) const {
  return Units::to(u,getRightTrack());
}

/**
 * Returns minimum ground speed in internal units [m/s]
 */
double KinematicBandsParameters::getMinGroundSpeed()  const {
  return min_gs_;
}

/**
 * Returns minimum ground speed in specified units [u]
 */
double KinematicBandsParameters::getMinGroundSpeed(const std::string& u)  const {
  return Units::to(u,getMinGroundSpeed());
}

/**
 * Returns maximum ground speed in internal units [m/s]
 */
double KinematicBandsParameters::getMaxGroundSpeed()  const {
  return max_gs_;
}

/**
 * Returns maximum ground speed in specified units [u]
 */
double KinematicBandsParameters::getMaxGroundSpeed(const std::string& u)  const {
  return Units::to(u,getMaxGroundSpeed());
}

/**
 * Returns minimum vertical speed in internal units [m/s]
 */
double KinematicBandsParameters::getMinVerticalSpeed()  const {
  return min_vs_;
}

/**
 * Returns minimum vertical speed in specified units [u]
 */
double KinematicBandsParameters::getMinVerticalSpeed(const std::string& u)  const {
  return Units::to(u,getMinVerticalSpeed());
}

/**
 * Returns maximum vertical speed in internal units [m/s]
 */
double KinematicBandsParameters::getMaxVerticalSpeed()  const {
  return max_vs_;
}

/**
 * Returns maximum vertical speed in specified units [u]
 */
double KinematicBandsParameters::getMaxVerticalSpeed(const std::string& u)  const {
  return Units::to(u,getMaxVerticalSpeed());
}

/**
 * Returns minimum altitude in internal units [m]
 */
double KinematicBandsParameters::getMinAltitude()  const {
  return min_alt_;
}

/**
 * Returns minimum altitude in specified units [u]
 */
double KinematicBandsParameters::getMinAltitude(const std::string& u)  const {
  return Units::to(u,getMinAltitude());
}

/**
 * Returns maximum altitude in internal units [m]
 */
double KinematicBandsParameters::getMaxAltitude()  const {
  return max_alt_;
}

/**
 * Returns maximum altitude in specified units [u]
 */
double KinematicBandsParameters::getMaxAltitude(const std::string& u)  const {
  return Units::to(u,getMaxAltitude());
}

/**
 * Returns track step in internal units [rad]
 */
double KinematicBandsParameters::getTrackStep()  const {
  return trk_step_;
}

/**
 * Returns track step in specified units [u]
 */
double KinematicBandsParameters::getTrackStep(const std::string& u)  const {
  return Units::to(u,getTrackStep());
}

/**
 * Returns ground speed step in internal units [m/s]
 */
double KinematicBandsParameters::getGroundSpeedStep()  const {
  return gs_step_;
}

/**
 * Returns ground speed step in specified units [u]
 */
double KinematicBandsParameters::getGroundSpeedStep(const std::string& u)  const {
  return Units::to(u,getGroundSpeedStep());
}

/**
 * Returns vertical speed step in internal units [m/s]
 */
double KinematicBandsParameters::getVerticalSpeedStep()  const {
  return vs_step_;
}

/**
 * Returns vertical speed step in specified units [u]
 */
double KinematicBandsParameters::getVerticalSpeedStep(const std::string& u)  const {
  return Units::to(u,getVerticalSpeedStep());
}

/**
 * Returns altitude step in internal units [m]
 */
double KinematicBandsParameters::getAltitudeStep()  const {
  return alt_step_;
}

/**
 * Returns altitude step in specified units [u]
 */
double KinematicBandsParameters::getAltitudeStep(const std::string& u)  const {
  return Units::to(u,getAltitudeStep());
}

/**
 * Returns horizontal acceleration in internal units [m/s^2]
 */
double KinematicBandsParameters::getHorizontalAcceleration()  const {
  return horizontal_accel_;
}

/**
 * Returns horizontal acceleration in specified units [u]
 */
double KinematicBandsParameters::getHorizontalAcceleration(const std::string& u)  const {
  return Units::to(u,getHorizontalAcceleration());
}

/**
 * Returns vertical acceleration in internal units [m/s^2]
 */
double KinematicBandsParameters::getVerticalAcceleration()  const {
  return vertical_accel_;
}

/**
 * Returns vertical acceleration in specified units [u]
 */
double KinematicBandsParameters::getVerticalAcceleration(const std::string& u)  const {
  return Units::to(u,getVerticalAcceleration());
}

/**
 * Returns turn rate in internal units [rad/s]
 */
double KinematicBandsParameters::getTurnRate()  const {
  return turn_rate_;
}

/**
 * Returns turn rate in specified units [u]
 */
double KinematicBandsParameters::getTurnRate(const std::string& u)  const {
  return Units::to(u,getTurnRate());
}

/**
 * Returns bank angle in internal units [rad]
 */
double KinematicBandsParameters::getBankAngle()  const {
  return bank_angle_;
}

/**
 * Returns bank angle in specified units [u]
 */
double KinematicBandsParameters::getBankAngle(const std::string& u)  const {
  return Units::to(u,getBankAngle());
}

/**
 * Returns vertical rate in internal units [m/s]
 */
double KinematicBandsParameters::getVerticalRate()  const {
  return vertical_rate_;
}

/**
 * Returns vertical rate in specified units [u]
 */
double KinematicBandsParameters::getVerticalRate(const std::string& u)  const {
  return Units::to(u,getVerticalRate());
}

/**
 * @return horizontal NMAC distance in internal units [m].
 */
double KinematicBandsParameters::getHorizontalNMAC() const {
  return horizontal_nmac_;
}

/**
 * @return horizontal NMAC distance in specified units [u].
 */
double KinematicBandsParameters::getHorizontalNMAC(const std::string& u) const {
  return Units::to(u,getHorizontalNMAC());
}

/**
 * @return vertical NMAC distance in internal units [m].
 */
double KinematicBandsParameters::getVerticalNMAC() const {
  return vertical_nmac_;
}

/**
 * @return vertical NMAC distance in specified units [u].
 */
double KinematicBandsParameters::getVerticalNMAC(const std::string& u) const {
  return Units::to(u,getVerticalNMAC());
}

/**
 * Returns recovery stability time in seconds.
 */
double KinematicBandsParameters::getRecoveryStabilityTime()  const {
  return recovery_stability_time_;
}

/**
 * Returns recovery stability time in specified units [u]
 */
double KinematicBandsParameters::getRecoveryStabilityTime(const std::string& u)  const {
  return Units::to(u,getRecoveryStabilityTime());
}

/**
 * Returns minimum horizontal recovery distance in internal units [m]
 */
double KinematicBandsParameters::getMinHorizontalRecovery()  const {
  return min_horizontal_recovery_;
}

/**
 * Returns minimum horizontal recovery distance in specified units [u]
 */
double KinematicBandsParameters::getMinHorizontalRecovery(const std::string& u)  const {
  return Units::to(u,getMinHorizontalRecovery());
}

/**
 * Returns minimum vertical recovery distance in internal units [m]
 */
double KinematicBandsParameters::getMinVerticalRecovery()  const {
  return min_vertical_recovery_;
}

/**
 * Returns minimum vertical recovery distance in specified units [u]
 */
double KinematicBandsParameters::getMinVerticalRecovery(const std::string& u)  const {
  return Units::to(u,getMinVerticalRecovery());
}

/**
 * Set lookahead time to value in seconds.
 */
bool KinematicBandsParameters::setLookaheadTime(double val)  {
  if (error.isPositive("setLookaheadTime",val))  {
    lookahead_time_ = val;
    return true;
  }
  return false;
}

/**
 * Set lookahead time to value in specified units [u]
 */
bool KinematicBandsParameters::setLookaheadTime(double val, const std::string& u)  {
  return setLookaheadTime(Units::from(u,val));
}

/**
 * Set left track to value in internal units [rad]. Value is expected to be in [0 - pi]
 */
bool KinematicBandsParameters::setLeftTrack(double val) {
  val = std::abs(val);
  if (error.isBetween("setLeftTrack",val,0,Pi)) {
    left_trk_ = val;
    return true;
  }
  return false;
}

/**
 * Set left track to value in specified units [u]. Value is expected to be in [0 - pi]
 */
bool KinematicBandsParameters::setLeftTrack(double val, const std::string& u) {
  return setLeftTrack(Units::from(u,val));
}

/**
 * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
 */
bool KinematicBandsParameters::setRightTrack(double val) {
  val = std::abs(val);
  if (error.isBetween("setRightTrack",val,0,Pi)) {
    right_trk_ = val;
    return true;
  }
  return false;
}

/**
 * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
 */
bool KinematicBandsParameters::setRightTrack(double val, const std::string& u) {
  return setRightTrack(Units::from(u,val));
}

/**
 * Set minimum ground speed to value in internal units [m/s]
 * Minimum ground speed must be greater than ground speed step.
 */
bool KinematicBandsParameters::setMinGroundSpeed(double val)  {
  if (error.isPositive("setMinGroundSpeed",val))  {
    min_gs_ = val;
    return true;
  }
  return false;
}

/**
 * Set minimum ground speed to value in specified units [u]
 * Minimum ground speed must be greater than ground speed step.
 */
bool KinematicBandsParameters::setMinGroundSpeed(double val, const std::string& u)  {
  return setMinGroundSpeed(Units::from(u,val));
}

/**
 * Set maximum ground speed to value in internal units [m/s]
 */
bool KinematicBandsParameters::setMaxGroundSpeed(double val)  {
  if (error.isPositive("setMaxGroundSpeed",val))  {
    max_gs_ = val;
    return true;
  }
  return false;
}

/**
 * Set maximum ground speed to value in specified units [u]
 */
bool KinematicBandsParameters::setMaxGroundSpeed(double val, const std::string& u)  {
  return setMaxGroundSpeed(Units::from(u,val));
}

/**
 * Set minimum vertical speed to value in internal units [m/s]
 */
bool KinematicBandsParameters::setMinVerticalSpeed(double val)  {
  min_vs_ = val;
  return true;
}

/**
 * Set minimum vertical speed to value in specified units [u]
 */
bool KinematicBandsParameters::setMinVerticalSpeed(double val, const std::string& u)  {
  return setMinVerticalSpeed(Units::from(u,val));
}

/**
 * Set maximum vertical speed to value in internal units [m/s]
 */
bool KinematicBandsParameters::setMaxVerticalSpeed(double val)  {
  max_vs_ = val;
  return true;
}

/**
 * Set maximum vertical speed to value in specified units [u]
 */
bool KinematicBandsParameters::setMaxVerticalSpeed(double val, const std::string& u)  {
  return setMaxVerticalSpeed(Units::from(u,val));
}

/**
 * Set minimum altitude to value in internal units [m]
 */
bool KinematicBandsParameters::setMinAltitude(double val)  {
  if (error.isNonNegative("setMinAltitude",val))  {
    min_alt_ = val;
    return true;
  }
  return false;
}

/**
 * Set minimum altitude to value in specified units [u]
 */
bool KinematicBandsParameters::setMinAltitude(double val, const std::string& u)  {
  return setMinAltitude(Units::from(u,val));
}

/**
 * Set maximum altitude to value in internal units [m]
 */
bool KinematicBandsParameters::setMaxAltitude(double val)  {
  if (error.isPositive("setMaxAltitude",val))  {
    max_alt_ = val;
    return true;
  }
  return false;
}

/**
 * Set maximum altitude to value in specified units [u]
 */
bool KinematicBandsParameters::setMaxAltitude(double val, const std::string& u)  {
  return setMaxAltitude(Units::from(u,val));
}

/**
 * Set track step to value in internal units [rad]
 */
bool KinematicBandsParameters::setTrackStep(double val)  {
  if (error.isPositive("setTrackStep",val) &&
      error.isLessThan("setTrackStep",val,Pi))  {
    trk_step_ = val;
    return true;
  }
  return false;
}

/**
 * Set track step to value in specified units [u]
 */
bool KinematicBandsParameters::setTrackStep(double val, const std::string& u)  {
  return setTrackStep(Units::from(u,val));
}

/**
 * Set ground speed step to value in internal units [m/s]
 */
bool KinematicBandsParameters::setGroundSpeedStep(double val)  {
  if (error.isPositive("setGroundSpeedStep",val))  {
    gs_step_ = val;
    return true;
  }
  return false;
}

/**
 * Set ground speed step to value in specified units [u]
 */
bool KinematicBandsParameters::setGroundSpeedStep(double val, const std::string& u)  {
  return setGroundSpeedStep(Units::from(u,val));
}

/**
 * Set vertical speed step to value in internal units [m/s]
 */
bool KinematicBandsParameters::setVerticalSpeedStep(double val)  {
  if (error.isPositive("setVerticalSpeedStep",val))  {
    vs_step_ = val;
    return true;
  }
  return false;
}

/**
 * Set vertical speed step to value in specified units [u]
 */
bool KinematicBandsParameters::setVerticalSpeedStep(double val, const std::string& u)  {
  return setVerticalSpeedStep(Units::from(u,val));
}

/**
 * Set altitude step to value in internal units [m]
 */
bool KinematicBandsParameters::setAltitudeStep(double val)  {
  if (error.isPositive("setAltitudeStep",val))  {
    alt_step_ = val;
    return true;
  }
  return false;
}

/**
 * Set altitude step to value in specified units [u]
 */
bool KinematicBandsParameters::setAltitudeStep(double val, const std::string& u)  {
  return setAltitudeStep(Units::from(u,val));
}

/**
 * Set horizontal acceleration to value in internal units [m/s^2]
 */
bool KinematicBandsParameters::setHorizontalAcceleration(double val)  {
  if (error.isNonNegative("setHorizontalAcceleration",val))  {
    horizontal_accel_ = val;
    return true;
  }
  return false;
}

/**
 * Set horizontal acceleration to value in specified units [u]
 */
bool KinematicBandsParameters::setHorizontalAcceleration(double val, const std::string& u)  {
  return setHorizontalAcceleration(Units::from(u,val));
}

/**
 * Set vertical acceleration to value in internal units [m/s^2]
 */
bool KinematicBandsParameters::setVerticalAcceleration(double val)  {
  if (error.isNonNegative("setVerticalAcceleration",val))  {
    vertical_accel_ = val;
    return true;
  }
  return false;
}

/**
 * Set vertical acceleration to value in specified units [u]
 */
bool KinematicBandsParameters::setVerticalAcceleration(double val, const std::string& u)  {
  return setVerticalAcceleration(Units::from(u,val));
}

bool KinematicBandsParameters::set_turn_rate(double val) {
  if (error.isNonNegative("setTurnRate",val)) {
    turn_rate_ = val;
    return true;
  }
  return false;
}

/**
 * Set turn rate to value in internal units [rad/s]. As a side effect, this method
 * resets the bank angle.
 */
bool KinematicBandsParameters::setTurnRate(double val)  {
  if (set_turn_rate(val)) {
    bank_angle_ = 0.0;
    return true;
  }
  return false;
}

/**
 * Set turn rate to value in specified units [u]. As a side effect, this method
 * resets the bank angle.
 */
bool KinematicBandsParameters::setTurnRate(double val, const std::string& u)  {
  return setTurnRate(Units::from(u,val));
}

bool KinematicBandsParameters::set_bank_angle(double val) {
  if (error.isNonNegative("setBankAngle",val)) {
    bank_angle_ = val;
    return true;
  }
  return false;
}

/**
 * Set bank angle to value in internal units [rad] As a side effect, this method
 * resets the turn rate.
 */
bool KinematicBandsParameters::setBankAngle(double val)  {
  if (set_bank_angle(val)) {
    turn_rate_ = 0.0;
    return true;
  }
  return false;
}

/**
 * Set bank angle to value in specified units [u] As a side effect, this method
 * resets the turn rate.
 */
bool KinematicBandsParameters::setBankAngle(double val, const std::string& u)  {
  return setBankAngle(Units::from(u,val));
}

/**
 * Set vertical rate to value in internal units [m/s]
 */
bool KinematicBandsParameters::setVerticalRate(double val)  {
  if (error.isNonNegative("setVerticalRate",val))  {
    vertical_rate_ = val;
    return true;
  }
  return false;
}

/**
 * Set vertical rate to value in specified units [u]
 */
bool KinematicBandsParameters::setVerticalRate(double val, const std::string& u)  {
  return setVerticalRate(Units::from(u,val));
}

/**
 * Set horizontal NMAC distance to value in internal units [m].
 */
bool KinematicBandsParameters::setHorizontalNMAC(double val) {
  if (error.isNonNegative("setHorizontalNMAC",val)) {
    horizontal_nmac_ = val;
    return true;
  }
  return false;
}

/**
 * Set horizontal NMAC distance to value in specified units [u].
 */
bool KinematicBandsParameters::setHorizontalNMAC(double val, const std::string& u){
  return setHorizontalNMAC(Units::from(u,val));
}

/**
 * Set vertical NMAC distance to value in internal units [m].
 */
bool KinematicBandsParameters::setVerticalNMAC(double val) {
  if (error.isNonNegative("setVerticalNMAC",val)) {
    vertical_nmac_ = val;
    return true;
  }
  return false;
}

/**
 * Set vertical NMAC distance to value in specified units [u].
 */
bool KinematicBandsParameters::setVerticalNMAC(double val, const std::string& u) {
  return setVerticalNMAC(Units::from(u,val));
}

/**
 * Set recovery stability time to value in seconds.
 */
bool KinematicBandsParameters::setRecoveryStabilityTime(double val)  {
  if (error.isNonNegative("setRecoveryStabilityTime",val))  {
    recovery_stability_time_ = val;
    return true;
  }
  return false;
}

/**
 * Set recovery stability time to value in specified units [u]
 */
bool KinematicBandsParameters::setRecoveryStabilityTime(double val, const std::string& u)  {
  return setRecoveryStabilityTime(Units::from(u,val));
}

/**
 * Set minimum recovery horizontal distance to value in internal units [m]
 */
bool KinematicBandsParameters::setMinHorizontalRecovery(double val)  {
  if (error.isNonNegative("setMinHorizontalRecovery",val))  {
    min_horizontal_recovery_ = val;
    return true;
  }
  return false;
}

/**
 * Set minimum recovery horizontal distance to value in specified units [u]
 */
bool KinematicBandsParameters::setMinHorizontalRecovery(double val, const std::string& u)  {
  return setMinHorizontalRecovery(Units::from(u,val));
}

/**
 * Set minimum recovery vertical distance to value in internal units [m]
 */
bool KinematicBandsParameters::setMinVerticalRecovery(double val)  {
  if (error.isNonNegative("setMinVerticalRecovery",val)) {
    min_vertical_recovery_ = val;
    return true;
  }
  return false;
}

/**
 * Set minimum recovery vertical distance to value in specified units [u]
 */
bool KinematicBandsParameters::setMinVerticalRecovery(double val, const std::string& u)  {
  return setMinVerticalRecovery(Units::from(u,val));
}

/**
 * @return true if repulsive criteria is enabled for conflict bands.
 */
bool KinematicBandsParameters::isEnabledConflictCriteria() const {
  return conflict_crit_;
}

/**
 * Enable/disable repulsive criteria for conflict bands.
 */
void KinematicBandsParameters::setConflictCriteria(bool flag) {
  conflict_crit_ = flag;
}

/**
 * Enable repulsive criteria for conflict bands.
 */
void KinematicBandsParameters::enableConflictCriteria() {
  setConflictCriteria(true);
}

/**
 * Disable repulsive criteria for conflict bands.
 */
void KinematicBandsParameters::disableConflictCriteria() {
  setConflictCriteria(false);
}

/**
 * @return true if repulsive criteria is enabled for recovery bands.
 */
bool KinematicBandsParameters::isEnabledRecoveryCriteria() const {
  return recovery_crit_;
}

/**
 * Enable/disable repulsive criteria for recovery bands.
 */
void KinematicBandsParameters::setRecoveryCriteria(bool flag) {
  recovery_crit_ = flag;
}

/**
 * Enable repulsive criteria for recovery bands.
 */
void KinematicBandsParameters::enableRecoveryCriteria() {
  setRecoveryCriteria(true);
}

/**
 * Disable repulsive criteria for recovery bands.
 */
void KinematicBandsParameters::disableRecoveryCriteria() {
  setRecoveryCriteria(false);
}

/**
 * Enable/disable repulsive criteria for conflict and recovery bands.
 */
void KinematicBandsParameters::setRepulsiveCriteria(bool flag) {
  setConflictCriteria(flag);
  setRecoveryCriteria(flag);
}

/**
 * Enable repulsive criteria for conflict and recovery bands.
 */
void KinematicBandsParameters::enableRepulsiveCriteria() {
  setRepulsiveCriteria(true);
}

/**
 * Disable repulsive criteria for conflict and recovery bands.
 */
void KinematicBandsParameters::disableRepulsiveCriteria() {
  setRepulsiveCriteria(false);
}

/**
 * Return recovery bands flag for track bands.
 */
bool KinematicBandsParameters::isEnabledRecoveryTrackBands()  const {
  return recovery_trk_;
}

/**
 * Return recovery bands flag for ground speed bands.
 */
bool KinematicBandsParameters::isEnabledRecoveryGroundSpeedBands()  const {
  return recovery_gs_;
}

/**
 * Return recovery bands flag for vertical speed bands.
 */
bool KinematicBandsParameters::isEnabledRecoveryVerticalSpeedBands()  const {
  return recovery_vs_;
}

/**
 * Return recovery bands flag for altitude bands.
 */
bool KinematicBandsParameters::isEnabledRecoveryAltitudeBands()  const {
  return recovery_alt_;
}

/**
 * Enable/disable recovery bands for track, ground speed, vertical speed, and altitude.
 */
void KinematicBandsParameters::setRecoveryBands(bool flag) {
  recovery_trk_ = flag;
  recovery_gs_ = flag;
  recovery_vs_ = flag;
  recovery_alt_ = flag;
}

/**
 * Enable all recovery bands for track, ground speed, vertical speed, and altitude.
 */
void KinematicBandsParameters::enableRecoveryBands() {
  setRecoveryBands(true);
}

/**
 * Disable all recovery bands for track, ground speed, vertical speed, and altitude.
 */
void KinematicBandsParameters::disableRecoveryBands() {
  setRecoveryBands(false);
}

/**
 * Enable/disable recovery bands for track, ground speed, and vertical speed.
 */
void KinematicBandsParameters::setRecoveryTrackBands(bool flag)  {
  recovery_trk_ = flag;
}

/**
 * Sets recovery bands flag for ground speed bands to value.
 */
void KinematicBandsParameters::setRecoveryGroundSpeedBands(bool flag)  {
  recovery_gs_ = flag;
}

/**
 * Sets recovery bands flag for vertical speed bands to value.
 */
void KinematicBandsParameters::setRecoveryVerticalSpeedBands(bool flag)  {
  recovery_vs_ = flag;
}

/**
 * Sets recovery bands flag for altitude bands to value.
 */
void KinematicBandsParameters::setRecoveryAltitudeBands(bool flag)  {
  recovery_alt_ = flag;
}

/**
 * Returns true if collision avoidance bands are enabled.
 */
bool KinematicBandsParameters::isEnabledCollisionAvoidanceBands()  const {
  return ca_bands_;
}

/**
 * Enable/disable collision avoidance bands.
 */
void KinematicBandsParameters::setCollisionAvoidanceBands(bool flag)  {
  ca_bands_ = flag;
}

/**
 * Enable collision avoidance bands.
 */
void KinematicBandsParameters::enableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(true);
}

/**
 * Disable collision avoidance bands.
 */
void KinematicBandsParameters::disableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(false);
}

/**
 * @return get factor for computing collision avoidance bands. Factor value is in (0,1]
 */
double KinematicBandsParameters::getCollisionAvoidanceBandsFactor() const {
  return ca_factor_;
}

/**
 * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
 */
bool KinematicBandsParameters::setCollisionAvoidanceBandsFactor(double val) {
  if (error.isPositive("setCollisionAvoidanceBandsFactor",val) &&
      error.isLessThan("setCollisionAvoidanceBandsFactor", val,1)) {
    ca_factor_ = val;
    return true;
  }
  return false;
}

/**
 * Get horizontal contour threshold, specified in internal units [rad] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
double KinematicBandsParameters::getHorizontalContourThreshold() const {
  return contour_thr_;
}

/**
 * Get horizontal contour threshold, specified in given units [u] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
double KinematicBandsParameters::getHorizontalContourThreshold(const std::string& u) const {
  return Units::to(u,getHorizontalContourThreshold());
}

/**
 * Set horizontal contour threshold, specified in internal units [rad] [0 - pi] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
bool KinematicBandsParameters::setHorizontalContourThreshold(double val) {
  val = std::abs(val);
  if (error.isBetween("setHorizontalContourThreshold",val,0,Pi)) {
    contour_thr_ = val;
    return true;
  }
  return false;
}

/**
 * Set horizontal contour threshold, specified in given units [u] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
bool KinematicBandsParameters::setHorizontalContourThreshold(double val, const std::string& u) {
  return setHorizontalContourThreshold(Units::from(u,val));
}

/**
 * Set instantaneous bands.
 */
void KinematicBandsParameters::setInstantaneousBands() {
  turn_rate_ = 0;
  bank_angle_ = 0;
  horizontal_accel_ = 0;
  vertical_accel_ = 0;
  vertical_rate_ = 0;
}

/**
 * Set kinematic bands.
 * Set turn rate to 3 deg/s, when type is true; set turn rate to  1.5 deg/s
 * when type is false;
 */
void KinematicBandsParameters::setKinematicBands(bool type) {
  // Section 1.2.3, DAA MOPS SC-228 V3.6
  turn_rate_ = Units::from("deg/s",type ? 3.0 : 1.5);
  bank_angle_ = 0;
  horizontal_accel_ = Units::from("m/s^2",2.0);
  vertical_accel_ = Units::from("G",0.25);
  vertical_rate_ = Units::from("fpm",500);
}

/**
 *  Load parameters for kinematic bands from file.
 */
void KinematicBandsParameters::loadFromParameterData(const ParameterData& parameters) {
  setParameters(parameters);
}

/**
 *  Load parameters for kinematic bands from file.
 */
bool KinematicBandsParameters::loadFromFile(const std::string& file) {
  StateReader reader;
  reader.open(file);
  ParameterData parameters = reader.getParametersRef();
  setParameters(parameters);
  return !reader.hasError();
}

/**
 *  Write parameters for kinematic bands to file.
 */
bool KinematicBandsParameters::saveToFile(const std::string& file) const {
  std::ofstream out;
  out.open(file.c_str());
  if ( out.fail() ) {
    fpln("File "+file+" write protected");
    return false;
  }
  fp(&out,toString());
  out.close();
  return true;
}

std::string KinematicBandsParameters::val_unit(double val, const std::string& u) {
  return FmPrecision(Units::to(u,val))+" ["+u+"]";
}

std::string KinematicBandsParameters::toString() const {
  std::string s = "# V-"+VERSION+"\n";
  s+="# Conflict Bands Parameters\n";
  s+="lookahead_time = "+val_unit(lookahead_time_,"s")+"\n";
  s+="left_trk = "+val_unit(left_trk_,"deg")+"\n";
  s+="right_trk = "+val_unit(right_trk_,"deg")+"\n";
  s+="min_gs = "+val_unit(min_gs_,"knot")+"\n";
  s+="max_gs = "+val_unit(max_gs_,"knot")+"\n";
  s+="min_vs = "+val_unit(min_vs_,"fpm")+"\n";
  s+="max_vs = "+val_unit(max_vs_,"fpm")+"\n";
  s+="min_alt = "+val_unit(min_alt_,"ft")+"\n";
  s+="max_alt = "+val_unit(max_alt_,"ft")+"\n";
  s+="# Kinematic Bands Parameters\n";
  s+="trk_step = "+val_unit(trk_step_,"deg")+"\n";
  s+="gs_step = "+val_unit(gs_step_,"knot")+"\n";
  s+="vs_step = "+val_unit(vs_step_,"fpm")+"\n";
  s+="alt_step = "+val_unit(alt_step_,"ft")+"\n";
  s+="horizontal_accel = "+val_unit(horizontal_accel_,"m/s^2")+"\n";
  s+="vertical_accel = "+val_unit(vertical_accel_,"m/s^2")+"\n";
  s+="turn_rate = "+val_unit(turn_rate_,"deg/s")+"\n";
  s+="bank_angle = "+val_unit(bank_angle_,"deg")+"\n";
  s+="vertical_rate = "+val_unit(vertical_rate_,"fpm")+"\n";
  s+="horizontal_nmac = "+val_unit(horizontal_nmac_,"ft")+"\n";
  s+="vertical_nmac = "+val_unit(vertical_nmac_,"ft")+"\n";
  s+="# Recovery Bands Parameters\n";
  s+="recovery_stability_time = "+val_unit(recovery_stability_time_,"s")+"\n";
  s+="# If min_horizontal_recovery is set to 0, TCAS RA HMD is used instead\n";
  s+="min_horizontal_recovery = "+val_unit(min_horizontal_recovery_,"nmi")+"\n";
  s+="# If min_vertical_recovery is set to 0, TCAS RA ZTHR is used instead\n";
  s+="min_vertical_recovery = "+val_unit(min_vertical_recovery_,"ft")+"\n";
  s+="conflict_crit = "+Fmb(conflict_crit_)+"\n";
  s+="recovery_crit = "+Fmb(recovery_crit_)+"\n";
  s+="recovery_trk = "+Fmb(recovery_trk_)+"\n";
  s+="recovery_gs = "+Fmb(recovery_gs_)+"\n";
  s+="recovery_vs = "+Fmb(recovery_vs_)+"\n";
  s+="recovery_alt = "+Fmb(recovery_alt_)+"\n";
  s+="# if ca_bands is true, keep computing recovery bands by reducing min horizontal/vertical recovery until NMAC\n";
  s+="ca_bands = "+Fmb(ca_bands_)+"\n";
  s+="# ca_factor is the reduction factor, when computing CA bands\n";
  s+="ca_factor = "+Fm4(ca_factor_)+"\n";
  s+="# Contours Parameters\n";
  s+="# If contour_thr is set to 0, only conflict contours are computed. Max value is 180 [deg]\n";
  s+="contour_thr = "+val_unit(contour_thr_,"deg")+"\n";
  s+="# Alert Levels\n";
  ParameterData p = alertor.getParameters();
  s+=p.toString();
  return s;
}

std::string KinematicBandsParameters::toPVS(int prec) const {
  std::string s = "";
  s+="(# ";
  s+="lookahead_time := "+FmPrecision(lookahead_time_,prec)+", ";
  s+="left_trk := "+FmPrecision(left_trk_,prec)+", ";
  s+="right_trk := "+FmPrecision(right_trk_,prec)+", ";
  s+="min_gs := "+FmPrecision(min_gs_,prec)+", ";
  s+="max_gs := "+FmPrecision(max_gs_,prec)+", ";
  s+="min_vs := "+FmPrecision(min_vs_,prec)+", ";
  s+="max_vs := "+FmPrecision(max_vs_,prec)+", ";
  s+="min_alt := "+FmPrecision(min_alt_,prec)+", ";
  s+="max_alt := "+FmPrecision(max_alt_,prec)+", ";
  s+="trk_step := "+FmPrecision(trk_step_,prec)+", ";
  s+="gs_step := "+FmPrecision(gs_step_,prec)+", ";
  s+="vs_step := "+FmPrecision(vs_step_,prec)+", ";
  s+="alt_step := "+FmPrecision(alt_step_,prec)+", ";
  s+="horizontal_accel := "+FmPrecision(horizontal_accel_,prec)+", ";
  s+="vertical_accel := "+FmPrecision(vertical_accel_,prec)+", ";
  s+="turn_rate := "+FmPrecision(turn_rate_,prec)+", ";
  s+="bank_angle := "+FmPrecision(bank_angle_,prec)+", ";
  s+="vertical_rate := "+FmPrecision(vertical_rate_,prec)+", ";
  s+="horizontal_nmac :="+FmPrecision(horizontal_nmac_,prec)+", ";
  s+="vertical_nmac :="+FmPrecision(vertical_nmac_,prec)+", ";
  s+="recovery_stability_time := "+FmPrecision(recovery_stability_time_,prec)+", ";
  s+="min_horizontal_recovery := "+FmPrecision(min_horizontal_recovery_,prec)+", ";
  s+="min_vertical_recovery := "+FmPrecision(min_vertical_recovery_,prec)+", ";
  s+="conflict_crit := "+Fmb(conflict_crit_)+", ";
  s+="recovery_crit := "+Fmb(recovery_crit_)+", ";
  s+="recovery_trk := "+Fmb(recovery_trk_)+", ";
  s+="recovery_gs := "+Fmb(recovery_gs_)+", ";
  s+="recovery_vs := "+Fmb(recovery_vs_)+", ";
  s+="recovery_alt := "+Fmb(recovery_alt_)+", ";
  s+="ca_bands := "+Fmb(ca_bands_)+", ";
  s+="ca_factor := "+FmPrecision(ca_factor_,prec)+", ";
  s+="contour_thr := "+FmPrecision(contour_thr_,prec)+", ";
  s+="alertor := "+alertor.toPVS(prec);
  s+="#)";
  return s;
}

ParameterData KinematicBandsParameters::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void KinematicBandsParameters::updateParameterData(ParameterData& p) const {
  // Bands
  p.setInternal("lookahead_time", lookahead_time_, "s");
  p.setInternal("left_trk", left_trk_, "deg");
  p.setInternal("right_trk", right_trk_, "deg");
  p.setInternal("min_gs", min_gs_, "kts");
  p.setInternal("max_gs", max_gs_, "kts");
  p.setInternal("min_vs", min_vs_, "fpm");
  p.setInternal("max_vs", max_vs_, "fpm");
  p.setInternal("min_alt", min_alt_, "ft");
  p.setInternal("max_alt", max_alt_, "ft");

  // Kinematic bands
  p.setInternal("trk_step", trk_step_, "deg");
  p.setInternal("gs_step", gs_step_, "kts");
  p.setInternal("vs_step", vs_step_, "fpm");
  p.setInternal("alt_step", alt_step_, "ft");
  p.setInternal("horizontal_accel", horizontal_accel_, "m/s^2");
  p.setInternal("vertical_accel", vertical_accel_, "m/s^2");
  p.setInternal("turn_rate", turn_rate_, "deg/s");
  p.setInternal("bank_angle", bank_angle_, "deg");
  p.setInternal("vertical_rate", vertical_rate_, "fpm");
  p.setInternal("horizontal_nmac",horizontal_nmac_,"ft");
  p.setInternal("vertical_nmac",vertical_nmac_,"ft");

  // Recovery bands
  p.setInternal("recovery_stability_time", recovery_stability_time_, "s");
  p.setInternal("min_horizontal_recovery", min_horizontal_recovery_, "nmi");
  p.setInternal("min_vertical_recovery", min_vertical_recovery_, "ft");
  p.setBool("conflict_crit", conflict_crit_);
  p.setBool("recovery_crit", recovery_crit_);
  p.setBool("recovery_trk", recovery_trk_);
  p.setBool("recovery_gs", recovery_gs_);
  p.setBool("recovery_vs", recovery_vs_);
  p.setBool("recovery_alt", recovery_alt_);
  p.setBool("ca_bands", ca_bands_);
  p.setInternal("ca_factor", ca_factor_, "unitless");

  // Contours
  p.setInternal("contour_thr", contour_thr_, "deg");

  // Alertor
  alertor.updateParameterData(p);
}

void KinematicBandsParameters::setParameters(const ParameterData& p) {
  // Bands
  if (p.contains("lookahead_time")) {
    setLookaheadTime(p.getValue("lookahead_time"));
  }
  if (p.contains("left_trk")) {
    setLeftTrack(p.getValue("left_trk"));
  }
  if (p.contains("right_trk")) {
    setRightTrack(p.getValue("right_trk"));
  }
  if (p.contains("min_gs")) {
    setMinGroundSpeed(p.getValue("min_gs"));
  }
  if (p.contains("max_gs")) {
    setMaxGroundSpeed(p.getValue("max_gs"));
  }
  if (p.contains("min_vs")) {
    setMinVerticalSpeed(p.getValue("min_vs"));
  }
  if (p.contains("max_vs")) {
    setMaxVerticalSpeed(p.getValue("max_vs"));
  }
  if (p.contains("min_alt")) {
    setMinAltitude(p.getValue("min_alt"));
  }
  if (p.contains("max_alt")) {
    setMaxAltitude(p.getValue("max_alt"));
  }
  // Kinematic bands
  if (p.contains("trk_step")) {
    setTrackStep(p.getValue("trk_step"));
  }
  if (p.contains("gs_step")) {
    setGroundSpeedStep(p.getValue("gs_step"));
  }
  if (p.contains("vs_step")) {
    setVerticalSpeedStep(p.getValue("vs_step"));
  }
  if (p.contains("alt_step")) {
    setAltitudeStep(p.getValue("alt_step"));
  }
  if (p.contains("horizontal_accel")) {
    setHorizontalAcceleration(p.getValue("horizontal_accel"));
  }
  if (p.contains("vertical_accel")) {
    setVerticalAcceleration(p.getValue("vertical_accel"));
  }
  if (p.contains("turn_rate")) {
    set_turn_rate(p.getValue("turn_rate"));
  }
  if (p.contains("bank_angle")) {
    set_bank_angle(p.getValue("bank_angle"));
  }
  if (p.contains("vertical_rate")) {
    setVerticalRate(p.getValue("vertical_rate"));
  }
  if (p.contains("horizontal_nmac")) {
    setHorizontalNMAC(p.getValue("horizontal_nmac"));
  }
  if (p.contains("vertical_nmac")) {
    setVerticalNMAC(p.getValue("vertical_nmac"));
  }
  // Recovery bands
  if (p.contains("recovery_stability_time")) {
    setRecoveryStabilityTime(p.getValue("recovery_stability_time"));
  }
  if (p.contains("min_horizontal_recovery")) {
    setMinHorizontalRecovery(p.getValue("min_horizontal_recovery"));
  }
  if (p.contains("min_vertical_recovery")) {
    setMinVerticalRecovery(p.getValue("min_vertical_recovery"));
  }
  // Criteria parameters
  if (p.contains("conflict_crit")) {
    conflict_crit_ = p.getBool("conflict_crit");
  }
  if (p.contains("recovery_crit")) {
    recovery_crit_ = p.getBool("recovery_crit");
  }
  // Recovery parameters
  if (p.contains("recovery_trk")) {
    recovery_trk_ = p.getBool("recovery_trk");
  }
  if (p.contains("recovery_gs")) {
    recovery_gs_ = p.getBool("recovery_gs");
  }
  if (p.contains("recovery_vs")) {
    recovery_vs_ = p.getBool("recovery_vs");
  }
  if (p.contains("recovery_alt")) {
    recovery_alt_ = p.getBool("recovery_alt");
  }
  if (p.contains("ca_bands")) {
    ca_bands_ = p.getBool("ca_bands");
  }
  if (p.contains("ca_factor")) {
    setCollisionAvoidanceBandsFactor(p.getValue("ca_factor"));
  }
  // Contours
  if (p.contains("contour_thr")) {
    setHorizontalContourThreshold(p.getValue("contour_thr"));
  }
  // Alertor
  alertor.setParameters(p);
}

bool KinematicBandsParameters::hasError() const {
  return error.hasError();
}

bool KinematicBandsParameters::hasMessage() const {
  return error.hasMessage();
}

std::string KinematicBandsParameters::getMessage() {
  return error.getMessage();
}

std::string KinematicBandsParameters::getMessageNoClear()  const {
  return error.getMessageNoClear();
}

}

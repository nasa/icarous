/*
 * Kinematic Multi Bands
 *
 * Contact: Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicMultiBands.h"
#include "TCASTable.h"
#include "string_util.h"
#include "format.h"
#include "CriteriaCore.h"
#include "Constants.h"

namespace larcfm {

/**
 * Construct a KinematicMultiBands object with initial parameters and an empty list of detectors.
 */
KinematicMultiBands::KinematicMultiBands(const KinematicBandsParameters& parameters) : error("KinematicMultiBands"),
    core_(KinematicBandsCore(parameters)),
    trk_band_(KinematicTrkBands(parameters)),
    gs_band_(KinematicGsBands(parameters)),
    vs_band_(KinematicVsBands(parameters)),
    alt_band_(KinematicAltBands(parameters)) {}

/**
 * Construct a KinematicMultiBands object with the default parameters and an empty list of detectors.
 */
KinematicMultiBands::KinematicMultiBands() : error("KinematicMultiBands"),
    core_(KinematicBandsCore(KinematicBandsParameters())),
    trk_band_(KinematicTrkBands(core_.parameters)),
    gs_band_(KinematicGsBands(core_.parameters)),
    vs_band_(KinematicVsBands(core_.parameters)),
    alt_band_(KinematicAltBands(core_.parameters)) {}

/**
 * Construct a KinematicMultiBands object from an existing KinematicMultiBands object.
 * This copies all traffic data.
 */
KinematicMultiBands::KinematicMultiBands(const KinematicMultiBands& b) : error("KinematicMultiBands"),
    core_(KinematicBandsCore(b.core_.parameters)),
    trk_band_(KinematicTrkBands(b.core_.parameters)),
    gs_band_(KinematicGsBands(b.core_.parameters)),
    vs_band_(KinematicVsBands(b.core_.parameters)),
    alt_band_(KinematicAltBands(b.core_.parameters)) {}

KinematicMultiBands::~KinematicMultiBands() {}

KinematicMultiBands& KinematicMultiBands::operator=(const KinematicMultiBands& b) {
  core_ = KinematicBandsCore(b.core_.parameters);
  trk_band_ = KinematicTrkBands(b.core_.parameters);
  gs_band_ = KinematicGsBands(b.core_.parameters);
  vs_band_ = KinematicVsBands(b.core_.parameters);
  alt_band_ = KinematicAltBands(b.core_.parameters);
  reset();
  return *this;
}

/** Ownship and Traffic **/

TrafficState const & KinematicMultiBands::getOwnship() const {
  return core_.ownship;
}

// This function clears the traffic
void KinematicMultiBands::setOwnship(const TrafficState& own) {
  clear();
  core_.ownship = own;
}

// This function clears the traffic
void KinematicMultiBands::setOwnship(const std::string& id, const Position& p, const Velocity& v) {
  setOwnship(id,p,v,0.0);
}

// This function clears the traffic
void KinematicMultiBands::setOwnship(const std::string& id, const Position& p, const Velocity& v,
    double time) {
  setOwnship(TrafficState::makeOwnship(id,p,v,time));
}

// This function clears the traffic
void KinematicMultiBands::setOwnship(const Position& p, const Velocity& v) {
  setOwnship("Ownship",p,v);
}

void KinematicMultiBands::setTraffic(const std::vector<TrafficState>& traffic) {
  core_.traffic.clear();
  core_.traffic.insert(core_.traffic.end(),traffic.begin(),traffic.end());
  reset();
}

std::vector<TrafficState> const & KinematicMultiBands::getTraffic()  const {
  return core_.traffic;
}

bool KinematicMultiBands::hasOwnship() const {
  return core_.hasOwnship();
}

bool KinematicMultiBands::hasTraffic() const {
  return core_.hasTraffic();
}

void KinematicMultiBands::addTraffic(const TrafficState& ac) {
  if (!ac.isValid()) {
    error.addError("addTraffic: invalid aircraft.");
    return;
  } else if (!hasOwnship()) {
    error.addError("addTraffic: setOwnship must be called before addTraffic.");
  } else if (ac.isLatLon() != isLatLon()) {
    error.addError("addTraffic: inconsistent use of lat/lon and Euclidean data.");
  } else {
    core_.traffic.push_back(ac);
    reset();
  }
}

void KinematicMultiBands::addTraffic(const std::string& id, const Position& pi, const Velocity& vi) {
  if (!hasOwnship()) {
    error.addError("addTraffic: setOwnship must be called before addTraffic.");
    return;
  }
  addTraffic(core_.ownship.makeIntruder(id,pi,vi));
}

void KinematicMultiBands::addTraffic(const Position& pi, const Velocity& vi) {
  addTraffic("AC_"+Fmi(core_.traffic.size()+1),pi,vi);
}

/** General Settings **/

/**
 * Set alert thresholds
 */
void KinematicMultiBands::setAlertor(const AlertLevels& alertor) {
  core_.parameters.alertor.copy(alertor);
  reset();
}

/**
 * Set bands parameters
 */
void KinematicMultiBands::setKinematicBandsParameters(const KinematicBandsParameters& parameters) {
  core_.parameters.setKinematicBandsParameters(parameters);

  // Set Track Bands
  trk_band_.set_step(parameters.getTrackStep());
  trk_band_.set_turn_rate(parameters.getTurnRate());
  trk_band_.set_bank_angle(parameters.getBankAngle());
  trk_band_.set_recovery(parameters.isEnabledRecoveryTrackBands());
  trk_band_.set_step(parameters.getTrackStep());
  setLeftTrack(parameters.getLeftTrack());
  setRightTrack(parameters.getRightTrack());

  // Set Ground Speed Bands
  gs_band_.set_step(parameters.getGroundSpeedStep());
  gs_band_.set_horizontal_accel(parameters.getHorizontalAcceleration());
  gs_band_.set_recovery(parameters.isEnabledRecoveryGroundSpeedBands());
  gs_band_.set_step(parameters.getGroundSpeedStep());
  setMinGroundSpeed(parameters.getMinGroundSpeed());
  setMaxGroundSpeed(parameters.getMaxGroundSpeed());

  // Set Vertical Speed Bands
  vs_band_.set_step(parameters.getVerticalSpeedStep());
  vs_band_.set_vertical_accel(parameters.getVerticalAcceleration());
  vs_band_.set_recovery(parameters.isEnabledRecoveryVerticalSpeedBands());
  vs_band_.set_step(parameters.getVerticalSpeedStep());
  setMinVerticalSpeed(parameters.getMinVerticalSpeed());
  setMaxVerticalSpeed(parameters.getMaxVerticalSpeed());

  // Set Altitude Bands
  alt_band_.set_step(parameters.getAltitudeStep());
  alt_band_.set_vertical_rate(parameters.getVerticalRate());
  alt_band_.set_vertical_accel(parameters.getVerticalAcceleration());
  alt_band_.set_recovery(parameters.isEnabledRecoveryAltitudeBands());
  alt_band_.set_step(parameters.getAltitudeStep());
  setMinAltitude(parameters.getMinAltitude());
  setMaxAltitude(parameters.getMaxAltitude());

  reset();
}

/**
 * @return recovery stability time in seconds. Recovery bands are computed at time of first green plus
 * this time.
 */
double KinematicMultiBands::getRecoveryStabilityTime()  const {
  return core_.parameters.getRecoveryStabilityTime();
}

/**
 * @return recovery stability time in specified units. Recovery bands are computed at time of first green plus
 * this time.
 */
double KinematicMultiBands::getRecoveryStabilityTime(const std::string& u) const {
  return core_.parameters.getRecoveryStabilityTime(u);
}

/**
 * Sets recovery stability time in seconds. Recovery bands are computed at time of first green plus
 * this time.
 */
void KinematicMultiBands::setRecoveryStabilityTime(double t) {
  core_.parameters.setRecoveryStabilityTime(t);
  reset();
}

/**
 * Sets recovery stability time in seconds. Recovery bands are computed at time of first green plus
 * this time.
 */
void KinematicMultiBands::setRecoveryStabilityTime(double t, const std::string& u) {
  core_.parameters.setRecoveryStabilityTime(t,u);
  reset();
}

/**
 * @return minimum horizontal separation for recovery bands in internal units [m]
 */
double KinematicMultiBands::getMinHorizontalRecovery()  const {
  return core_.parameters.getMinHorizontalRecovery();
}

/**
 * Return minimum horizontal separation for recovery bands in specified units [u]
 */
double KinematicMultiBands::getMinHorizontalRecovery(const std::string& u)  const {
  return core_.parameters.getMinHorizontalRecovery(u);
}

/**
 * Sets minimum horizontal separation for recovery bands in internal units [m]
 */
void KinematicMultiBands::setMinHorizontalRecovery(double val) {
  core_.parameters.setMinHorizontalRecovery(val);
  reset();
}

/**
 * Set minimum horizontal separation for recovery bands in specified units [u].
 */
void KinematicMultiBands::setMinHorizontalRecovery(double val, const std::string& u) {
  core_.parameters.setMinHorizontalRecovery(val,u);
  reset();
}

/**
 * @return minimum vertical separation for recovery bands in internal units [m]
 */
double KinematicMultiBands::getMinVerticalRecovery()  const {
  return core_.parameters.getMinVerticalRecovery();
}

/**
 * Return minimum vertical separation for recovery bands in specified units [u]
 */
double KinematicMultiBands::getMinVerticalRecovery(const std::string& u)  const {
  return core_.parameters.getMinVerticalRecovery(u);
}

/**
 * Sets minimum vertical separation for recovery bands in internal units [m]
 */
void KinematicMultiBands::setMinVerticalRecovery(double val) {
  core_.parameters.setMinVerticalRecovery(val);
  reset();
}

/**
 * Set minimum vertical separation for recovery bands in units
 */
void KinematicMultiBands::setMinVerticalRecovery(double val, const std::string& u) {
  core_.parameters.setMinVerticalRecovery(val,u);
  reset();
}

bool KinematicMultiBands::isEnabledCollisionAvoidanceBands() const{
  return core_.parameters.isEnabledCollisionAvoidanceBands();
}

void KinematicMultiBands::setCollisionAvoidanceBands(bool flag) {
  core_.parameters.setCollisionAvoidanceBands(flag);
  reset();
}

void KinematicMultiBands::enableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(true);
}

void KinematicMultiBands::disableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(false);
}

/**
 * @return get factor for computing collision avoidance bands. Factor value is in (0,1]
 */
double KinematicMultiBands::getCollisionAvoidanceBandsFactor(){
  return core_.parameters.getCollisionAvoidanceBandsFactor();
}

/**
 * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
 */
void KinematicMultiBands::setCollisionAvoidanceBandsFactor(double val){
  core_.parameters.setCollisionAvoidanceBandsFactor(val);
  reset();
}

/**
 * @return most urgent aircraft.
 */
const TrafficState& KinematicMultiBands::getMostUrgentAircraft() const {
  return core_.most_urgent_ac;
}

/**
 * Set most urgent aircraft.
 */
void KinematicMultiBands::setMostUrgentAircraft(const TrafficState& ac) {
  core_.most_urgent_ac = ac;
  reset();
}

/**
 * Set most urgent aircraft by identifier.
 */
void KinematicMultiBands::setMostUrgentAircraft(const std::string& id) {
  core_.most_urgent_ac = core_.intruder(id);
  reset();
}

/**
 * @return true if repulsive criteria is enabled for conflict bands.
 */
bool KinematicMultiBands::isEnabledConflictCriteria() const {
  return core_.parameters.isEnabledConflictCriteria();
}

/**
 * Enable/disable repulsive criteria for conflict bands.
 */
void KinematicMultiBands::setConflictCriteria(bool flag) {
  core_.parameters.setConflictCriteria(flag);
  reset();
}

/**
 * Enable repulsive criteria for conflict bands.
 */
void KinematicMultiBands::enableConflictCriteria() {
  setConflictCriteria(true);
}

/**
 * Disable repulsive criteria for conflict bands.
 */
void KinematicMultiBands::disableConflictCriteria() {
  setConflictCriteria(false);
}

/**
 * @return true if repulsive criteria is enabled for recovery bands.
 */
bool KinematicMultiBands::isEnabledRecoveryCriteria() const {
  return core_.parameters.isEnabledRecoveryCriteria();
}

/**
 * Enable/disable repulsive criteria for recovery bands.
 */
void KinematicMultiBands::setRecoveryCriteria(bool flag) {
  core_.parameters.setRecoveryCriteria(flag);
  reset();
}

/**
 * Enable repulsive criteria for recovery bands.
 */
void KinematicMultiBands::enableRecoveryCriteria() {
  setRecoveryCriteria(true);
}

/**
 * Disable repulsive criteria for recovery bands.
 */
void KinematicMultiBands::disableRecoveryCriteria() {
  setRecoveryCriteria(false);
}

/**
 * Enable/disable repulsive criteria for conflict and recovery bands.
 */
void KinematicMultiBands::setRepulsiveCriteria(bool flag) {
  setConflictCriteria(flag);
  setRecoveryCriteria(flag);
}

/**
 * Enable repulsive criteria for conflict and recovery bands.
 */
void KinematicMultiBands::enableRepulsiveCriteria() {
  setRepulsiveCriteria(true);
}

/**
 * Disable repulsive criteria for conflict and recovery bands.
 */
void KinematicMultiBands::disableRepulsiveCriteria() {
  setRepulsiveCriteria(false);
}

/**
 * Sets recovery bands flag for track, ground speed, and vertical speed bands to specified value.
 */
void KinematicMultiBands::setRecoveryBands(bool flag) {
  trk_band_.set_recovery(flag);
  gs_band_.set_recovery(flag);
  vs_band_.set_recovery(flag);
  alt_band_.set_recovery(flag);
  core_.parameters.setRecoveryTrackBands(flag);
  core_.parameters.setRecoveryGroundSpeedBands(flag);
  core_.parameters.setRecoveryVerticalSpeedBands(flag);
  core_.parameters.setRecoveryAltitudeBands(flag);
  reset();
}

/**
 * Enables recovery bands for track, ground speed, and vertical speed.
 */
void KinematicMultiBands::enableRecoveryBands() {
  setRecoveryBands(true);
}

/**
 * Disable recovery bands for track, ground speed, and vertical speed.
 */
void KinematicMultiBands::disableRecoveryBands() {
  setRecoveryBands(false);
}

/** Track Bands Settings **/

/**
 * @return left track in radians [0 - pi] [rad] from current ownship's track
 */
double KinematicMultiBands::getLeftTrack() {
  if (trk_band_.get_rel()) {
    return trk_band_.get_min();
  } else if (trk_band_.check_input(core_)) {
    return -trk_band_.min_rel(core_.ownship);
  } else {
    return NaN;
  }
}

/**
 * @return left track in specified units [0 - pi] [u] from current ownship's track
 */
double KinematicMultiBands::getLeftTrack(const std::string& u) {
  return Units::to(u,getLeftTrack());
}

/**
 * @return right track in radians [0 - pi] [rad] from current ownship's track
 */
double KinematicMultiBands::getRightTrack() {
  if (trk_band_.get_rel()) {
    return trk_band_.get_max();
  } else if (trk_band_.check_input(core_)) {
    return trk_band_.max_rel(core_.ownship);
  } else {
    return NaN;
  }
}

/**
 * @return right track in specified units [0 - pi] [u] from current ownship's track
 */
double KinematicMultiBands::getRightTrack(const std::string& u) {
  return Units::to(u,getRightTrack());
}

/**
 * Set left track to value in internal units [rad]. Value is expected to be in [0 - pi]
 */
void KinematicMultiBands::setLeftTrack(double val) {
  val = -std::abs(Util::to_pi(val));
  if (!trk_band_.get_rel()) {
    trk_band_.set_rel(true);
  }
  trk_band_.set_min(val);
  reset();
}

/**
 * Set left track to value in specified units [u]. Value is expected to be in [0 - pi]
 */
void KinematicMultiBands::setLeftTrack(double val, const std::string& u) {
  setLeftTrack(Units::from(u,val));
}

/**
 * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
 */
void KinematicMultiBands::setRightTrack(double val) {
  val = std::abs(Util::to_pi(val));
  if (!trk_band_.get_rel()) {
    trk_band_.set_rel(true);
  }
  trk_band_.set_max(val);
  reset();
}

/**
 * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
 */
void KinematicMultiBands::setRightTrack(double val, const std::string& u) {
  setRightTrack(Units::from(u,val));
}

/**
 * Set absolute min/max tracks for bands computations. Tracks are specified in internal units [rad].
 * Values are expected to be in [0 - 2pi]
 */
void KinematicMultiBands::setMinMaxTrack(double min, double max) {
  min = Util::to_2pi(min);
  max = Util::to_2pi(max);
  if (max == 0) {
    max = 2*Pi;
  }
  if (trk_band_.get_rel()) {
    trk_band_.set_rel(false);
  }
  trk_band_.set_min(min);
  trk_band_.set_max(max);
  reset();
}

/**
 * Set absolute min/max tracks for bands computations. Tracks are specified in given units [u].
 * Values are expected to be in [0 - 2pi] [u]
 */
void KinematicMultiBands::setMinMaxTrack(double min, double max, const std::string& u) {
  setMinMaxTrack(Units::from(u,min),Units::from(u,max));
}

/**
 * @return step size for track bands in internal units [rad]
 */
double KinematicMultiBands::getTrackStep()  const {
  return core_.parameters.getTrackStep();
}

/**
 * @return step size for track bands in specified units [u].
 */
double KinematicMultiBands::getTrackStep(const std::string& u)  const {
  return core_.parameters.getTrackStep(u);
}

/**
 * Sets step size for track bands in internal units [rad]
 */
void KinematicMultiBands::setTrackStep(double val) {
  core_.parameters.setTrackStep(val);
  trk_band_.set_step(core_.parameters.getTrackStep());
  reset();
}

/**
 * Sets step size for track bands in specified units [u].
 */
void KinematicMultiBands::setTrackStep(double val, const std::string& u) {
  core_.parameters.setTrackStep(val,u);
  trk_band_.set_step(core_.parameters.getTrackStep());
  reset();
}

/**
 * @return bank angle in internal units [rad]
 */
double KinematicMultiBands::getBankAngle()  const {
  return core_.parameters.getBankAngle();
}

/**
 * @return bank angle in specified units [u].
 */
double KinematicMultiBands::getBankAngle(const std::string& u)  const {
  return core_.parameters.getBankAngle(u);
}

/**
 * Sets bank angle for track bands to value in internal units [rad]. As a side effect, this method
 * resets the turn rate.
 */
void KinematicMultiBands::setBankAngle(double val) {
  core_.parameters.setBankAngle(val);
  trk_band_.set_bank_angle(core_.parameters.getBankAngle());
  trk_band_.set_turn_rate(0);
  reset();
}

/**
 * Sets bank angle for track bands to value in specified units [u]. As a side effect, this method
 * resets the turn rate.
 */
void KinematicMultiBands::setBankAngle(double val, const std::string& u) {
  core_.parameters.setBankAngle(val,u);
  trk_band_.set_bank_angle(core_.parameters.getBankAngle());
  trk_band_.set_turn_rate(0);
  reset();
}

/**
 * @return turn rate in internal units [rad/s]
 */
double KinematicMultiBands::getTurnRate()  const {
  return core_.parameters.getTurnRate();
}

/**
 * @return turn rate in specified units [u].
 */
double KinematicMultiBands::getTurnRate(const std::string& u)  const {
  return core_.parameters.getTurnRate(u);
}

/**
 * Sets turn rate for track bands to value in internal units [rad/s]. As a side effect, this method
 * resets the bank angle.
 */
void KinematicMultiBands::setTurnRate(double val) {
  core_.parameters.setTurnRate(val);
  trk_band_.set_turn_rate(core_.parameters.getTurnRate());
  trk_band_.set_bank_angle(0);
  reset();
}

/**
 * Sets turn rate for track bands to value in specified units [u]. As a side effect, this method
 * resets the bank angle.
 */
void KinematicMultiBands::setTurnRate(double val, const std::string& u) {
  core_.parameters.setTurnRate(val,u);
  trk_band_.set_turn_rate(core_.parameters.getTurnRate());
  trk_band_.set_bank_angle(0);
  reset();
}

/**
 * @return true if recovery track bands are enabled.
 */
bool KinematicMultiBands::isEnabledRecoveryTrackBands() const {
  return core_.parameters.isEnabledRecoveryTrackBands();
}

/**
 * Sets recovery bands flag for track bands to specified value.
 */
void KinematicMultiBands::setRecoveryTrackBands(bool flag) {
  trk_band_.set_recovery(flag);
  core_.parameters.setRecoveryTrackBands(flag);
  reset();
}

/** Ground Speed Bands Settings **/

/**
 * @return minimum ground speed for ground speed bands in internal units [m/s]
 */
double KinematicMultiBands::getMinGroundSpeed() {
  if (gs_band_.get_rel()) {
    if (gs_band_.check_input(core_)) {
      return gs_band_.min_val(core_.ownship);
    } else {
      return NaN;
    }
  } else {
    return gs_band_.get_min();
  }
}

/**
 * @return minimum ground speed for ground speed bands in specified units [u].
 */
double KinematicMultiBands::getMinGroundSpeed(const std::string& u) {
  return Units::to(u,getMinGroundSpeed());
}

/**
 * Sets minimum ground speed for ground speed bands to value in internal units [m/s]
 */
void KinematicMultiBands::setMinGroundSpeed(double val) {
  if (error.isNonNegative("setMinGroundSpeed",val)) {
    if (gs_band_.get_rel()) {
      gs_band_.set_rel(false);
    }
    gs_band_.set_min(val);
    reset();
  }
}

/**
 * Sets minimum ground speed for ground speed bands to value in specified units [u].
 */
void KinematicMultiBands::setMinGroundSpeed(double val, const std::string& u) {
  setMinGroundSpeed(Units::from(u, val));
}

/**
 * @return maximum ground speed for ground speed bands in internal units [m/s]
 */
double KinematicMultiBands::getMaxGroundSpeed() {
  if (gs_band_.get_rel()) {
    if (gs_band_.check_input(core_)) {
      return gs_band_.max_val(core_.ownship);
    } else {
      return NaN;
    }
  } else {
    return gs_band_.get_max();
  }
}

/**
 * @return maximum ground speed for ground speed bands in specified units [u].
 */
double KinematicMultiBands::getMaxGroundSpeed(const std::string& u) {
  return Units::to(u,getMaxGroundSpeed());
}

/**
 * Sets maximum ground speed for ground speed bands to value in internal units [m/s]
 */
void KinematicMultiBands::setMaxGroundSpeed(double val) {
  if (error.isPositive("setMaxGroundSpeed",val)) {
    if (gs_band_.get_rel()) {
      gs_band_.set_rel(false);
    }
    gs_band_.set_max(val);
    reset();
  }
}

/**
 * Sets maximum ground speed for ground speed bands to value in specified units [u].
 */
void KinematicMultiBands::setMaxGroundSpeed(double val, const std::string& u) {
  setMaxGroundSpeed(Units::from(u, val));
}

/**
 * Set below/above ground speed, relative to ownship's ground speed, for bands computations.
 * Ground speeds are specified in internal units [m/s]. Below and above are expected to be
 * non-negative values.
 */
void KinematicMultiBands::setBelowAboveGroundSpeed(double below, double above) {
  below = -std::abs(below);
  above = std::abs(above);
  if (!gs_band_.get_rel()) {
    gs_band_.set_rel(true);
  }
  gs_band_.set_min(below);
  gs_band_.set_max(above);
  reset();
}

/**
 * Set below/above ground speed, relative to ownship's ground speed, for bands computations.
 * Ground speeds are specified in given units [u]. Below and above are expected to be
 * non-negative values.
 */
void KinematicMultiBands::setBelowAboveGroundSpeed(double below, double above, const std::string& u) {
  setBelowAboveGroundSpeed(Units::from(u,below),Units::from(u,above));
}

/**
 * @return step size for ground speed bands in internal units [m/s]
 */
double KinematicMultiBands::getGroundSpeedStep()  const {
  return core_.parameters.getGroundSpeedStep();
}

/**
 * @return step size for ground speed bands in specified units [u].
 */
double KinematicMultiBands::getGroundSpeedStep(const std::string& u)  const {
  return core_.parameters.getGroundSpeedStep(u);
}

/**
 * Sets step size for ground speed bands to value in internal units [m/s]
 */
void KinematicMultiBands::setGroundSpeedStep(double val) {
  core_.parameters.setGroundSpeedStep(val);
  gs_band_.set_step(core_.parameters.getGroundSpeedStep());
  reset();
}

/**
 * Sets step size for ground speed bands to value in specified units [u].
 */
void KinematicMultiBands::setGroundSpeedStep(double val, const std::string& u) {
  core_.parameters.setGroundSpeedStep(val,u);
  gs_band_.set_step(core_.parameters.getGroundSpeedStep());
  reset();
}

/**
 * @return horizontal acceleration for ground speed bands to value in internal units [m/s^2]
 */
double KinematicMultiBands::getHorizontalAcceleration()  const {
  return core_.parameters.getHorizontalAcceleration();
}

/**
 * @return horizontal acceleration for ground speed bands to value in specified units [u].
 */
double KinematicMultiBands::getHorizontalAcceleration(const std::string& u)  const {
  return core_.parameters.getHorizontalAcceleration(u);
}

/**
 * Sets horizontal acceleration for ground speed bands to value in internal units [m/s^2]
 */
void KinematicMultiBands::setHorizontalAcceleration(double val) {
  core_.parameters.setHorizontalAcceleration(val);
  gs_band_.set_horizontal_accel(core_.parameters.getHorizontalAcceleration());
  reset();
}

/**
 * Sets horizontal acceleration for ground speed bands to value in specified units [u].
 */
void KinematicMultiBands::setHorizontalAcceleration(double val, const std::string& u) {
  core_.parameters.setHorizontalAcceleration(val,u);
  gs_band_.set_horizontal_accel(core_.parameters.getHorizontalAcceleration());
  reset();
}

/**
 * @return true if recovery ground speed bands are enabled.
 */
bool KinematicMultiBands::isEnabledRecoveryGroundSpeedBands() {
  return core_.parameters.isEnabledRecoveryGroundSpeedBands();
}

/**
 * Sets recovery bands flag for ground speed bands to specified value.
 */
void KinematicMultiBands::setRecoveryGroundSpeedBands(bool flag) {
  gs_band_.set_recovery(flag);
  core_.parameters.setRecoveryGroundSpeedBands(flag);
  reset();
}

/** Vertical Speed Bands Settings **/

/**
 * @return minimum vertical speed for vertical speed bands in internal units [m/s]
 */
double KinematicMultiBands::getMinVerticalSpeed() {
  if (vs_band_.get_rel()) {
    if (vs_band_.check_input(core_)) {
      return vs_band_.min_val(core_.ownship);
    } else {
      return NaN;
    }
  } else {
    return vs_band_.get_min();
  }
}

/**
 * @return minimum vertical speed for vertical speed bands in specified units [u].
 */
double KinematicMultiBands::getMinVerticalSpeed(const std::string& u) {
  return Units::to(u,getMinVerticalSpeed());
}

/**
 * Sets minimum vertical speed for vertical speed bands to value in internal units [m/s]
 */
void KinematicMultiBands::setMinVerticalSpeed(double val) {
  if (vs_band_.get_rel()) {
    vs_band_.set_rel(false);
  }
  vs_band_.set_min(val);
  reset();
}

/**
 * Sets minimum vertical speed for vertical speed bands to value in specified units [u].
 */
void KinematicMultiBands::setMinVerticalSpeed(double val, const std::string& u) {
  setMinVerticalSpeed(Units::from(u, val));
}

/**
 * @return maximum vertical speed for vertical speed bands in internal units [m/s]
 */
double KinematicMultiBands::getMaxVerticalSpeed() {
  if (vs_band_.get_rel()) {
    if (vs_band_.check_input(core_)) {
      return vs_band_.max_val(core_.ownship);
    } else {
      return NaN;
    }
  } else {
    return vs_band_.get_max();
  }
}

/**
 * @return maximum vertical speed for vertical speed bands in specified units [u].
 */
double KinematicMultiBands::getMaxVerticalSpeed(const std::string& u) {
  return Units::to(u,getMaxVerticalSpeed());
}

/**
 * Sets maximum vertical speed for vertical speed bands to value in internal units [m/s]
 */
void KinematicMultiBands::setMaxVerticalSpeed(double val) {
  if (vs_band_.get_rel()) {
    vs_band_.set_rel(false);
  }
  vs_band_.set_max(val);
  reset();
}

/**
 * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations.
 * Vertical speeds are specified in internal units [m/s]. Below and above are expected to be
 * non-negative values.
 */
void KinematicMultiBands::setBelowAboveVerticalSpeed(double below, double above) {
  below = -std::abs(below);
  above = std::abs(above);
  if (!vs_band_.get_rel()) {
    vs_band_.set_rel(true);
  }
  vs_band_.set_min(below);
  vs_band_.set_max(above);
  reset();
}

/**
 * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations.
 * Vertical speeds are specified in given units [u]. Below and above are expected to be
 * non-negative values.
 */
void KinematicMultiBands::setBelowAboveVerticalSpeed(double below, double above, const std::string& u) {
  setBelowAboveVerticalSpeed(Units::from(u,below),Units::from(u,above));
}

/**
 * Sets maximum vertical speed for vertical speed bands to value in specified units [u].
 */
void KinematicMultiBands::setMaxVerticalSpeed(double val, const std::string& u) {
  setMaxVerticalSpeed(Units::from(u, val));
}

/**
 * @return step size for vertical speed bands in internal units [m/s]
 */
double KinematicMultiBands::getVerticalSpeedStep()  const {
  return core_.parameters.getVerticalSpeedStep();
}

/**
 * @return step size for vertical speed bands in specified units [u].
 */
double KinematicMultiBands::getVerticalSpeedStep(const std::string& u)  const {
  return core_.parameters.getVerticalSpeedStep(u);
}

/**
 * Sets step size for vertical speed bands to value in internal units [m/s]
 */
void KinematicMultiBands::setVerticalSpeedStep(double val) {
  core_.parameters.setVerticalSpeedStep(val);
  vs_band_.set_step(core_.parameters.getVerticalSpeedStep());
  reset();
}

/**
 * Sets step size for vertical speed bands to value in specified units [u].
 */
void KinematicMultiBands::setVerticalSpeedStep(double val, const std::string& u) {
  core_.parameters.setVerticalSpeedStep(val,u);
  vs_band_.set_step(core_.parameters.getVerticalSpeedStep());
  reset();
}

/**
 * @return constant vertical acceleration for vertical speed and altitude bands in internal
 * units [m/s^2]
 */
double KinematicMultiBands::getVerticalAcceleration()  const {
  return core_.parameters.getVerticalAcceleration();
}

/**
 * @return constant vertical acceleration for vertical speed and altitude bands in specified
 * units
 */
double KinematicMultiBands::getVerticalAcceleration(const std::string& u)  const {
  return core_.parameters.getVerticalAcceleration(u);
}

/**
 * Sets the constant vertical acceleration for vertical speed and altitude bands
 * to value in internal units [m/s^2]
 */
void KinematicMultiBands::setVerticalAcceleration(double val) {
  core_.parameters.setVerticalAcceleration(val);
  vs_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
  alt_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
  reset();
}

/**
 * Sets the constant vertical acceleration for vertical speed and altitude bands
 * to value in specified units [u].
 */
void KinematicMultiBands::setVerticalAcceleration(double val, const std::string& u) {
  core_.parameters.setVerticalAcceleration(val,u);
  vs_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
  alt_band_.set_vertical_accel(core_.parameters.getVerticalAcceleration());
  reset();
}

/**
 * @return true if recovery vertical speed bands are enabled.
 */
bool KinematicMultiBands::isEnabledRecoveryVerticalSpeedBands() {
  return core_.parameters.isEnabledRecoveryVerticalSpeedBands();
}

/**
 * Sets recovery bands flag for vertical speed bands to specified value.
 */
void KinematicMultiBands::setRecoveryVerticalSpeedBands(bool flag) {
  vs_band_.set_recovery(flag);
  core_.parameters.setRecoveryVerticalSpeedBands(flag);
  reset();
}

/** Altitude Bands Settings **/

/**
 * @return minimum altitude for altitude bands in internal units [m]
 */
double KinematicMultiBands::getMinAltitude() {
  if (alt_band_.get_rel()) {
    if (alt_band_.check_input(core_)) {
      return alt_band_.min_val(core_.ownship);
    } else {
      return NaN;
    }
  } else {
    return alt_band_.get_min();
  }
}

/**
 * @return minimum altitude for altitude bands in specified units [u].
 */
double KinematicMultiBands::getMinAltitude(const std::string& u) {
  return Units::to(u,getMinAltitude());
}

/**
 * Sets minimum altitude for altitude bands to value in internal units [m]
 */
void KinematicMultiBands::setMinAltitude(double val) {
  if (error.isNonNegative("setMinAltitude",val)) {
    if (alt_band_.get_rel()) {
      alt_band_.set_rel(false);
    }
    alt_band_.set_min(val);
    reset();
  }
}

/**
 * Sets minimum altitude for altitude bands to value in specified units [u].
 */
void KinematicMultiBands::setMinAltitude(double val, const std::string& u) {
  setMinAltitude(Units::from(u, val));
}

/**
 * @return maximum altitude for altitude bands in internal units [m]
 */
double KinematicMultiBands::getMaxAltitude() {
  if (alt_band_.get_rel()) {
    if (alt_band_.check_input(core_)) {
      return alt_band_.max_val(core_.ownship);
    } else {
      return NaN;
    }
  } else {
    return alt_band_.get_max();
  }
}

/**
 * @return maximum altitude for altitude bands in specified units [u].
 */
double KinematicMultiBands::getMaxAltitude(const std::string& u) {
  return Units::to(u,getMaxAltitude());
}

/**
 * Sets maximum altitude for altitude bands to value in internal units [m]
 */
void KinematicMultiBands::setMaxAltitude(double val) {
  if (error.isPositive("setMaxAltitude",val)) {
    if (alt_band_.get_rel()) {
      alt_band_.set_rel(false);
    }
    alt_band_.set_max(val);
    reset();
  }
}

/**
 * Sets maximum altitude for altitude bands to value in specified units [u].
 */
void KinematicMultiBands::setMaxAltitude(double val, const std::string& u) {
  setMaxAltitude(Units::from(u, val));
}

/**
 * Set below/above altitude, relative to ownship's altitude, for bands computations.
 * Altitude are specified in internal units [m]. Below and above are expected to be
 * non-negative values.
 */
void KinematicMultiBands::setBelowAboveAltitude(double below, double above) {
  below = -std::abs(below);
  above = std::abs(above);
  if (!alt_band_.get_rel()) {
    alt_band_.set_rel(true);
  }
  alt_band_.set_min(below);
  alt_band_.set_max(above);
  reset();
}

/**
 * Set below/above altitude, relative to ownship's altitude, for bands computations.
 * Altitudes are specified in given units [u]. Below and above are expected to be
 * non-negative values.
 */
void KinematicMultiBands::setBelowAboveAltitude(double below, double above, const std::string& u) {
  setBelowAboveAltitude(Units::from(u,below),Units::from(u,above));
}

/**
 * @return step size for altitude bands in internal units [m]
 */
double KinematicMultiBands::getAltitudeStep()  const {
  return core_.parameters.getAltitudeStep();
}

/**
 * @return step size for altitude bands in specified units [u].
 */
double KinematicMultiBands::getAltitudeStep(const std::string& u)  const {
  return core_.parameters.getAltitudeStep(u);
}

/**
 * Sets step size for altitude bands to value in internal units [m]
 */
void KinematicMultiBands::setAltitudeStep(double val) {
  core_.parameters.setAltitudeStep(val);
  alt_band_.set_step(core_.parameters.getAltitudeStep());
  reset();
}

/**
 * Sets step size for altitude bands to value in specified units [u].
 */
void KinematicMultiBands::setAltitudeStep(double val, const std::string& u) {
  core_.parameters.setAltitudeStep(val,u);
  alt_band_.set_step(core_.parameters.getAltitudeStep());
  reset();
}

/**
 * @return the vertical climb/descend rate for altitude bands in internal units [m/s]
 */
double KinematicMultiBands::getVerticalRate()  const {
  return core_.parameters.getVerticalRate();
}

/**
 * @return the vertical climb/descend rate for altitude bands in specified units [u].
 */
double KinematicMultiBands::getVerticalRate(const std::string& u)  const {
  return core_.parameters.getVerticalRate(u);
}

/**
 * Sets vertical rate for altitude bands to value in internal units [m/s]
 */
void KinematicMultiBands::setVerticalRate(double val) {
  core_.parameters.setVerticalRate(val);
  alt_band_.set_vertical_rate(core_.parameters.getVerticalRate());
  reset();
}

/**
 * Sets vertical rate for altitude bands to value in specified units [u].
 */
void KinematicMultiBands::setVerticalRate(double val, const std::string& u) {
  core_.parameters.setVerticalRate(val,u);
  alt_band_.set_vertical_rate(core_.parameters.getVerticalRate());
  reset();
}

/**
 * @return horizontal NMAC distance in internal units [m].
 */
double KinematicMultiBands::getHorizontalNMAC() const {
  return core_.parameters.getHorizontalNMAC();
}

/**
 * @return horizontal NMAC distance in specified units [u].
 */
double KinematicMultiBands::getHorizontalNMAC(const std::string& u) const {
  return core_.parameters.getHorizontalNMAC(u);
}

/**
 * @return vertical NMAC distance in internal units [m].
 */
double KinematicMultiBands::getVerticalNMAC() const {
  return core_.parameters.getVerticalNMAC();
}

/**
 * @return vertical NMAC distance in specified units [u].
 */
double KinematicMultiBands::getVerticalNMAC(const std::string& u) const {
  return core_.parameters.getVerticalNMAC(u);
}

/**
 * Set horizontal NMAC distance to value in internal units [m].
 */
void KinematicMultiBands::setHorizontalNMAC(double val) {
  core_.parameters.setHorizontalNMAC(val);
  reset();
}

/**
 * Set horizontal NMAC distance to value in specified units [u].
 */
void KinematicMultiBands::setHorizontalNMAC(double val, const std::string& u) {
  core_.parameters.setHorizontalNMAC(val,u);
  reset();
}

/**
 * Set vertical NMAC distance to value in internal units [m].
 */
void KinematicMultiBands::setVerticalNMAC(double val) {
  core_.parameters.setVerticalNMAC(val);
  reset();
}

/**
 * Set vertical NMAC distance to value in specified units [u].
 */
void KinematicMultiBands::setVerticalNMAC(double val, const std::string& u) {
  core_.parameters.setVerticalNMAC(val,u);
  reset();
}

/**
 * @return true if recovery altitude bands are enabled.
 */
bool KinematicMultiBands::isEnabledRecoveryAltitudeBands() {
  return core_.parameters.isEnabledRecoveryAltitudeBands();
}

/**
 * Sets recovery bands flag for altitude bands to specified value.
 */
void KinematicMultiBands::setRecoveryAltitudeBands(bool flag) {
  alt_band_.set_recovery(flag);
  core_.parameters.setRecoveryAltitudeBands(flag);
  reset();
}

/** Utility methods **/

/**
 *  Clear ownship and traffic data from this object.
 */
void KinematicMultiBands::clear() {
  core_.clear();
  reset();
}

void KinematicMultiBands::reset() {
  core_.reset();
  trk_band_.reset();
  gs_band_.reset();
  vs_band_.reset();
  alt_band_.reset();
}

/** Main interface methods **/

/**
 *  Return list of conflict aircraft for a given alert level.
 *  Note: alert level are 1-indexed.
 */
std::vector<TrafficState> const & KinematicMultiBands::conflictAircraft(int alert_level) {
  return core_.conflictAircraft(alert_level);
}

/**
 * Return time interval of violation for given alert level
 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
 */
Interval const & KinematicMultiBands::timeIntervalOfViolation(int alert_level) {
  return core_.timeIntervalOfViolation(alert_level);
}

/**
 * @return the number of track band intervals, negative if the ownship has not been KinematicMultiBands::set
 */
int KinematicMultiBands::trackLength() {
  return trk_band_.length(core_);
}

/**
 * Force computation of track bands. Usually, bands are only computed when needed. This method
 * forces the computation of track bands (this method is included mainly for debugging purposes).
 */
void KinematicMultiBands::forceTrackBandsComputation() {
  trk_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [rad], of the computed track bands.
 * @param i index
 */
Interval KinematicMultiBands::track(int i) {
  return trk_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed track bands.
 * @param i index
 * @param u units
 */
Interval KinematicMultiBands::track(int i, const std::string& u) {
  Interval ia = trk_band_.interval(core_,i);
  if (ia.isEmpty()) {
    return ia;
  }
  return Interval(Units::to(u, ia.low), Units::to(u, ia.up));
}

/**
 * @return the i-th region of the computed track bands.
 * @param i index
 */
BandsRegion::Region KinematicMultiBands::trackRegion(int i) {
  return trk_band_.region(core_,i);
}

/**
 * @return the range index of a given track specified in internal units [rad]
 * @param trk [rad]
 */
int KinematicMultiBands::trackRangeOf(double trk) {
  return trk_band_.rangeOf(core_,trk);
}

/**
 * @return the range index of a given track specified in given units [u]
 * @param trk [u]
 * @param u Units
 */
int KinematicMultiBands::trackRangeOf(double trk, const std::string& u) {
  return trk_band_.rangeOf(core_,Units::from(u, trk));
}

/**
 * @return the region of a given track specified in internal units [rad]
 * @param trk [rad]
 */
BandsRegion::Region KinematicMultiBands::regionOfTrack(double trk) {
  return trackRegion(trackRangeOf(trk));
}

/**
 * @return the region of a given track specified in given units [u]
 * @param trk [u]
 * @param u Units
 */
BandsRegion::Region KinematicMultiBands::regionOfTrack(double trk, const std::string& u) {
  return trackRegion(trackRangeOf(trk,u));
}

/**
 * Return last time to track maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double KinematicMultiBands::lastTimeToTrackManeuver(const TrafficState& ac) {
  return trk_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return time to recovery using track bands. Return NaN when bands are not saturated or when
 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
 */
double KinematicMultiBands::timeToTrackRecovery() {
  return trk_band_.timeToRecovery(core_);
}

/**
 * @return list of aircraft responsible for peripheral track bands for a given alert level.
 * Note: alert level are 1-indexed.
 */
std::vector<TrafficState> const & KinematicMultiBands::peripheralTrackAircraft(int alert_level) {
  return trk_band_.peripheralAircraft(core_,alert_level);
}

/**
 * Compute track resolution maneuver for given alert level.
 * @parameter dir is right (true)/left (false) of ownship current track
 * @return track resolution in internal units [rad] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no resolution to the right, and negative infinity if there
 * is no resolution to the left.
 */
double KinematicMultiBands::trackResolution(bool dir, int alert_level) {
  return trk_band_.compute_resolution(core_,alert_level,dir);
}

/**
 * Compute track resolution maneuver for given alert level.
 * @parameter dir is right (true)/left (false) of ownship current track
 * @parameter u units
 * @return track resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no resolution to the right, and negative infinity if there
 * is no resolution to the left.
 */
double KinematicMultiBands::trackResolution(bool dir, int alert_level, const std::string& u) {
  return Units::to(u,trackResolution(dir,alert_level));
}

/**
 * Compute track resolution maneuver for conflict alert level.
 * @parameter dir is right (true)/left (false) of ownship current track
 * @return track resolution in internal units [rad] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no resolution to the right, and negative infinity if there
 * is no resolution to the left.
 */
double KinematicMultiBands::trackResolution(bool dir) {
  return trackResolution(dir,0);
}

/**
 * Compute track resolution maneuver for conflict alert level.
 * @parameter dir is right (true)/left (false) of ownship current track
 * @parameter u units
 * @return track resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no resolution to the right, and negative infinity if there
 * is no resolution to the left.
 */
double KinematicMultiBands::trackResolution(bool dir, const std::string& u) {
  return Units::to(u,trackResolution(dir));
}

/**
 * Compute preferred track direction, for given alert level,
 * based on resolution that is closer to current track.
 * True: Right. False: Left.
 */
bool KinematicMultiBands::preferredTrackDirection(int alert_level) {
  return trk_band_.preferred_direction(core_, alert_level);
}

/**
 * Compute preferred track direction, for conflict alert level,
 * based on resolution that is closer to current track.
 * True: Right. False: Left.
 */
bool KinematicMultiBands::preferredTrackDirection() {
  return preferredTrackDirection(0);
}

/**
 * @return the number of ground speed band intervals, negative if the ownship has not been KinematicMultiBands::set
 */
int KinematicMultiBands::groundSpeedLength() {
  return gs_band_.length(core_);
}

/**
 * Force computation of ground speed bands. Usually, bands are only computed when needed. This method
 * forces the computation of ground speed bands (this method is included mainly for debugging purposes).
 */
void KinematicMultiBands::forceGroundSpeedBandsComputation() {
  gs_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [m/s], of the computed ground speed bands.
 * @param i index
 */
Interval KinematicMultiBands::groundSpeed(int i) {
  return gs_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed ground speed bands.
 * @param i index
 * @param u units
 */
Interval KinematicMultiBands::groundSpeed(int i, const std::string& u) {
  Interval ia = gs_band_.interval(core_,i);
  if (ia.isEmpty()) {
    return ia;
  }
  return Interval(Units::to(u, ia.low), Units::to(u, ia.up));
}

/**
 * @return the i-th region of the computed ground speed bands.
 * @param i index
 */
BandsRegion::Region KinematicMultiBands::groundSpeedRegion(int i) {
  return gs_band_.region(core_,i);
}

/**
 * @return the range index of a given ground speed specified in internal units [m/s]
 * @param gs [m/s]
 */
int KinematicMultiBands::groundSpeedRangeOf(double gs) {
  return gs_band_.rangeOf(core_,gs);
}

/**
 * @return the range index of a given ground speed specified in given units [u]
 * @param gs [u]
 * @param u Units
 */
int KinematicMultiBands::groundSpeedRangeOf(double gs, const std::string& u) {
  return groundSpeedRangeOf(Units::from(u,gs));
}

/**
 * @return the region of a given ground speed specified in internal units [m/s]
 * @param gs [m/s]
 */
BandsRegion::Region KinematicMultiBands::regionOfGroundSpeed(double gs) {
  return groundSpeedRegion(groundSpeedRangeOf(gs));
}

/**
 * @return the region of a given ground speed specified in given units [u]
 * @param gs [u]
 * @param u Units
 */
BandsRegion::Region KinematicMultiBands::regionOfGroundSpeed(double gs, const std::string& u) {
  return groundSpeedRegion(groundSpeedRangeOf(gs,u));
}

/**
 * Return last time to ground speed maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double KinematicMultiBands::lastTimeToGroundSpeedManeuver(const TrafficState& ac) {
  return gs_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return time to recovery using ground speed bands. Return NaN when bands are not saturated or when
 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
 */
double KinematicMultiBands::timeToGroundSpeedRecovery() {
  return gs_band_.timeToRecovery(core_);
}

/**
 * @return list of aircraft responsible for peripheral ground speed bands for a given alert level.
 * Note: alert level are 1-indexed.
 */
std::vector<TrafficState> const & KinematicMultiBands::peripheralGroundSpeedAircraft(int alert_level) {
  return gs_band_.peripheralAircraft(core_,alert_level);
}

/**
 * Compute ground speed resolution maneuver for given alert level.
 * @parameter dir is up (true)/down (false) of ownship current ground speed
 * @return ground speed resolution in internal units [m/s] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::groundSpeedResolution(bool dir, int alert_level) {
  return gs_band_.compute_resolution(core_,alert_level,dir);
}

/**
 * Compute ground speed resolution maneuver for given alert level.
 * @parameter dir is up (true)/down (false) of ownship current ground speed
 * @parameter u units
 * @return ground speed resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::groundSpeedResolution(bool dir, int alert_level, const std::string& u) {
  return Units::to(u,groundSpeedResolution(dir,alert_level));
}

/**
 * Compute ground speed resolution maneuver for conflict alert level.
 * @parameter dir is up (true)/down (false) of ownship current ground speed
 * @return ground speed resolution in internal units [m/s] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::groundSpeedResolution(bool dir) {
  return groundSpeedResolution(dir,0);
}

/**
 * Compute ground speed resolution maneuver for conflict alert level.
 * @parameter dir is up (true)/down (false) of ownship current ground speed
 * @parameter u units
 * @return ground speed resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::groundSpeedResolution(bool dir, const std::string& u) {
  return Units::to(u,groundSpeedResolution(dir));
}

/**
 * Compute preferred ground speed direction, for given alert level,
 * based on resolution that is closer to current ground speed.
 * True: Increase speed, False: Decrease speed.
 */
bool KinematicMultiBands::preferredGroundSpeedDirection(int alert_level) {
  return gs_band_.preferred_direction(core_, alert_level);
}

/**
 * Compute preferred  ground speed direction, for conflict alert level,
 * based on resolution that is closer to current ground speed.
 * True: Increase speed, False: Decrease speed.
 */
bool KinematicMultiBands::preferredGroundSpeedDirection() {
  return preferredGroundSpeedDirection(0);
}

/**
 * @return the number of vertical speed band intervals, negative if the ownship has not been KinematicMultiBands::set
 */
int KinematicMultiBands::verticalSpeedLength() {
  return vs_band_.length(core_);
}

/**
 * Force computation of vertical speed bands. Usually, bands are only computed when needed. This method
 * forces the computation of vertical speed bands (this method is included mainly for debugging purposes).
 */
void KinematicMultiBands::forceVerticalSpeedBandsComputation() {
  vs_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [m/s], of the computed vertical speed bands.
 * @param i index
 */
Interval KinematicMultiBands::verticalSpeed(int i) {
  return vs_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed vertical speed bands.
 * @param i index
 * @param u units
 */
Interval KinematicMultiBands::verticalSpeed(int i, const std::string& u) {
  Interval ia = vs_band_.interval(core_,i);
  if (ia.isEmpty()) {
    return ia;
  }
  return Interval(Units::to(u, ia.low), Units::to(u, ia.up));
}

/**
 * @return the i-th region of the computed vertical speed bands.
 * @param i index
 */
BandsRegion::Region KinematicMultiBands::verticalSpeedRegion(int i) {
  return vs_band_.region(core_,i);
}

/**
 * @return the region of a given vertical speed specified in internal units [m/s]
 * @param vs [m/s]
 */
int KinematicMultiBands::verticalSpeedRangeOf(double vs) {
  return vs_band_.rangeOf(core_,vs);
}

/**
 * @return the region of a given vertical speed specified in given units [u]
 * @param vs [u]
 * @param u Units
 */
int KinematicMultiBands::verticalSpeedRangeOf(double vs, const std::string& u) {
  return verticalSpeedRangeOf(Units::from(u, vs));
}

/**
 * @return the region of a given vertical speed specified in internal units [m/s]
 * @param vs [m/s]
 */
BandsRegion::Region KinematicMultiBands::regionOfVerticalSpeed(double vs) {
  return verticalSpeedRegion(verticalSpeedRangeOf(vs));
}

/**
 * @return the region of a given vertical speed specified in given units [u]
 * @param vs [u]
 * @param u Units
 */
BandsRegion::Region KinematicMultiBands::regionOfVerticalSpeed(double vs, const std::string& u) {
  return verticalSpeedRegion(verticalSpeedRangeOf(vs,u));
}

/**
 * Return last time to vertical speed maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double KinematicMultiBands::lastTimeToVerticalSpeedManeuver(const TrafficState& ac) {
  return vs_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return time to recovery using vertical speed bands. Return NaN when bands are not saturated or when
 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
 */
double KinematicMultiBands::timeToVerticalSpeedRecovery() {
  return vs_band_.timeToRecovery(core_);
}

/**
 * @return list of aircraft responsible for peripheral vertical speed bands for a given alert level.
 * Note: alert level are 1-indexed.
 */
std::vector<TrafficState> const & KinematicMultiBands::peripheralVerticalSpeedAircraft(int alert_level) {
  return vs_band_.peripheralAircraft(core_,alert_level);
}

/**
 * Compute vertical speed resolution maneuver for given alert level.
 * @parameter dir is up (true)/down (false) of ownship current vertical speed
 * @return vertical speed resolution in internal units [m/s] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::verticalSpeedResolution(bool dir, int alert_level) {
  return vs_band_.compute_resolution(core_,alert_level,dir);
}

/**
 * Compute vertical speed resolution maneuver for given alert level.
 * @parameter dir is up (true)/down (false) of ownship current vertical speed
 * @parameter u units
 * @return vertical speed resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::verticalSpeedResolution(bool dir, int alert_level, const std::string& u) {
  return Units::to(u,verticalSpeedResolution(dir,alert_level));
}

/**
 * Compute vertical speed resolution maneuver for conflict alert level.
 * @parameter dir is up (true)/down (false) of ownship current vertical speed
 * @return vertical speed resolution in internal units [m/s] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::verticalSpeedResolution(bool dir) {
  return verticalSpeedResolution(dir,0);
}

/**
 * Compute vertical speed resolution maneuver for conflict alert level.
 * @parameter dir is up (true)/down (false) of ownship current vertical speed
 * @parameter u units
 * @return vertical speed resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::verticalSpeedResolution(bool dir, const std::string& u) {
  return Units::to(u,verticalSpeedResolution(dir));
}

/**
 * Compute preferred vertical speed direction, for given alert level,
 * based on resolution that is closer to current vertical speed.
 * True: Increase speed, False: Decrease speed.
 */
bool KinematicMultiBands::preferredVerticalSpeedDirection(int alert_level) {
  return vs_band_.preferred_direction(core_, alert_level);
}

/**
 * Compute preferred  vertical speed direction, for conflict alert level,
 * based on resolution that is closer to current vertical speed.
 * True: Increase speed, False: Decrease speed.
 */
bool KinematicMultiBands::preferredVerticalSpeedDirection() {
  return preferredVerticalSpeedDirection(0);
}

/**
 * @return the number of altitude band intervals, negative if the ownship has not been KinematicMultiBands::set
 */
int KinematicMultiBands::altitudeLength() {
  return alt_band_.length(core_);
}

/**
 * Force computation of altitude bands. Usually, bands are only computed when needed. This method
 * forces the computation of altitude bands (this method is included mainly for debugging purposes).
 */
void KinematicMultiBands::forceAltitudeBandsComputation() {
  alt_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [m], of the computed altitude bands.
 * @param i index
 */
Interval KinematicMultiBands::altitude(int i) {
  return alt_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed altitude bands.
 * @param i index
 * @param u units
 */
Interval KinematicMultiBands::altitude(int i, const std::string& u) {
  Interval ia = alt_band_.interval(core_,i);
  if (ia.isEmpty()) {
    return ia;
  }
  return Interval(Units::to(u, ia.low), Units::to(u, ia.up));
}

/**
 * @return the i-th region of the computed altitude bands.
 * @param i index
 */
BandsRegion::Region KinematicMultiBands::altitudeRegion(int i) {
  return alt_band_.region(core_,i);
}

/**
 * @return the range index of a given altitude specified in internal units [m]
 * @param alt [m]
 */
int KinematicMultiBands::altitudeRangeOf(double alt) {
  return alt_band_.rangeOf(core_,alt);
}

/**
 * @return the range index of a given altitude specified in given units [u]
 * @param alt [u]
 * @param u Units
 */
int KinematicMultiBands::altitudeRangeOf(double alt, const std::string& u) {
  return altitudeRangeOf(Units::from(u,alt));
}

/**
 * @return the region of a given altitude specified in internal units [m]
 * @param alt [m]
 */
BandsRegion::Region KinematicMultiBands::regionOfAltitude(double alt) {
  return altitudeRegion(altitudeRangeOf(alt));
}

/**
 * @return the region of a given altitude specified in given units [u]
 * @param alt [u]
 * @param u Units
 */
BandsRegion::Region KinematicMultiBands::regionOfAltitude(double alt, const std::string& u) {
  return altitudeRegion(altitudeRangeOf(alt,u));
}

/**
 * Return last time to altitude maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double KinematicMultiBands::lastTimeToAltitudeManeuver(const TrafficState& ac) {
  return alt_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return time to recovery using altitude bands. Return NaN when bands are not saturated or when
 * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
 */
double KinematicMultiBands::timeToAltitudeRecovery() {
  return alt_band_.timeToRecovery(core_);
}

/**
 * @return list of aircraft responsible for peripheral altitude bands for a given alert level.
 * Note: alert level are 1-indexed.
 */
std::vector<TrafficState> const & KinematicMultiBands::peripheralAltitudeAircraft(int alert_level) {
  return alt_band_.peripheralAircraft(core_,alert_level);
}

/**
 * Compute altitude resolution maneuver for given alert level.
 * @parameter dir is up (true)/down (false) of ownship current altitude
 * @return altitude resolution in internal units [m] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::altitudeResolution(bool dir, int alert_level) {
  return alt_band_.compute_resolution(core_,alert_level,dir);
}

/**
 * Compute altitude resolution maneuver for given alert level.
 * @parameter dir is up (true)/down (false) of ownship current altitude
 * @parameter u units
 * @return altitude resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::altitudeResolution(bool dir, int alert_level, const std::string& u) {
  return Units::to(u,altitudeResolution(dir,alert_level));
}

/**
 * Compute altitude resolution maneuver for conflict alert level.
 * @parameter dir is up (true)/down (false) of ownship current altitude
 * @return altitude resolution in internal units [m] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::altitudeResolution(bool dir) {
  return altitudeResolution(dir,0);
}

/**
 * Compute altitude resolution maneuver for conflict alert level.
 * @parameter dir is up (true)/down (false) of ownship current altitude
 * @parameter u units
 * @return altitude resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double KinematicMultiBands::altitudeResolution(bool dir, const std::string& u) {
  return Units::to(u,altitudeResolution(dir));
}

/**
 * Compute preferred altitude direction, for given alert level,
 * based on resolution that is closer to current altitude.
 * True: Climb, False: Descend.
 */
bool KinematicMultiBands::preferredAltitudeDirection(int alert_level) {
  return alt_band_.preferred_direction(core_, alert_level);
}

/**
 * Compute preferred  altitude direction, for conflict alert level,
 * based on resolution that is closer to current altitude.
 * True: Climb, False: Descend.
 */
bool KinematicMultiBands::preferredAltitudeDirection() {
  return preferredAltitudeDirection(0);
}

/**
 * Return true if and only if threshold values, defining an alerting level, are violated.
 */
bool KinematicMultiBands::check_thresholds(const AlertThresholds& athr,
    const TrafficState& ac, int turning, int accelerating, int climbing) {
  if (athr.isValid()) {
    Vect3 so = core_.ownship.get_s();
    Velocity vo = core_.ownship.get_v();
    Vect3 si = ac.get_s();
    Velocity vi = ac.get_v();
    Detection3D* detector = athr.getDetectorRef();
    double alerting_time = Util::min(core_.parameters.getLookaheadTime(),athr.getAlertingTime());

    if (detector->violation(so,vo,si,vi)) {
      return true;
    }
    ConflictData det = detector->conflictDetection(so,vo,si,vi,0,alerting_time);
    if (det.conflict()) {
      return true;
    }
    if (athr.getTrackSpread() > 0 || athr.getGroundSpeedSpread() > 0 ||
        athr.getVerticalSpeedSpread() > 0 || athr.getAltitudeSpread() > 0) {
      if (athr.getTrackSpread() > 0) {
        KinematicTrkBands trk_band = KinematicTrkBands(core_.parameters);
        trk_band.set_rel(true);
        trk_band.set_min(turning <= 0 ? -athr.getTrackSpread() : 0);
        trk_band.set_max(turning >= 0 ? athr.getTrackSpread() : 0);
        if (trk_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
          return true;
        }
      }
      if (athr.getGroundSpeedSpread() > 0) {
        KinematicGsBands gs_band = KinematicGsBands(core_.parameters);
        gs_band.set_rel(true);
        gs_band.set_min(accelerating <= 0 ? -athr.getGroundSpeedSpread() : 0);
        gs_band.set_max(accelerating >= 0 ? athr.getGroundSpeedSpread() : 0);
        if (gs_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
          return true;
        }
      }
      if (athr.getVerticalSpeedSpread() > 0) {
        KinematicVsBands vs_band = KinematicVsBands(core_.parameters);
        vs_band.set_rel(true);
        vs_band.set_min(climbing <= 0 ? -athr.getVerticalSpeedSpread() : 0);
        vs_band.set_max(climbing >= 0 ? athr.getVerticalSpeedSpread() : 0);
        if (vs_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
          return true;
        }
      }
      if (athr.getAltitudeSpread() > 0) {
        KinematicAltBands alt_band = KinematicAltBands(core_.parameters);
        alt_band.set_rel(true);
        alt_band.set_min(climbing <= 0 ? -athr.getAltitudeSpread() : 0);
        alt_band.set_max(climbing >= 0 ? athr.getAltitudeSpread() : 0);
        if (alt_band.kinematic_conflict(core_,ac,detector,alerting_time)) {
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * Computes alerting level beteween ownship and aircraft.
 * The number 0 means no alert. A negative number means that aircraft index is not valid.
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
int KinematicMultiBands::alerting(const TrafficState& ac, int turning, int accelerating, int climbing) {
  for (int alert_level=core_.parameters.alertor.mostSevereAlertLevel(); alert_level > 0; --alert_level) {
    AlertThresholds athr = core_.parameters.alertor.getLevel(alert_level);
    if (check_thresholds(athr,ac,turning,accelerating,climbing)) {
      return alert_level;
    }
  }
  return 0;
}

std::string KinematicMultiBands::toString() const {
  std::string s = "";
  s+="## KinematicMultiBands\n";
  s+=core_.toString();
  s+="## Track Bands Internals\n";
  s+=trk_band_.toString()+"\n";
  s+="## Ground Speed Bands Internals\n";
  s+=gs_band_.toString()+"\n";
  s+="## Vertical Speed Bands Internals\n";
  s+=vs_band_.toString()+"\n";
  s+="## Altitude Bands Internals\n";
  s+=alt_band_.toString()+"\n";
  return s;
}

std::string KinematicMultiBands::outputStringInfo() {
  std::string s="";
  std::string ualt = core_.parameters.getUnits("alt_step");
  std::string ugs = core_.parameters.getUnits("gs_step");
  std::string uvs = core_.parameters.getUnits("vs_step");
  std::string uxy = "m";
  if (Units::isCompatible(ugs,"knot")) {
    uxy = "nmi";
  } else if (Units::isCompatible(ugs,"fpm")) {
    uxy = "ft";
  } else if (Units::isCompatible(ugs,"kph")) {
    uxy = "km";
  }
  s+="Time: "+Units::str("s",core_.ownship.getTime())+"\n";
  s+= core_.ownship.formattedTraffic(core_.traffic,uxy, ualt, ugs, uvs);
  s+="Conflict Criteria: ";
  if (core_.parameters.isEnabledConflictCriteria()) {
    s+="Enabled";
  } else {
    s+="Disabled";
  }
  s+="\n";
  s+="Recovery Criteria: ";
  if (core_.parameters.isEnabledRecoveryCriteria()) {
    s+="Enabled";
  } else {
    s+="Disabled";
  }
  s+="\n";
  s+="Most Urgent Aircraft: "+core_.most_urgent_ac.getId()+"\n";
  s+="Horizontal Epsilon: "+Fmi(core_.epsilonH())+"\n";
  s+="Vertical Epsilon: "+Fmi(core_.epsilonV())+"\n";
  return s;
}

std::string KinematicMultiBands::outputStringAlerting() {
  std::string s="";
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    s+="Conflict Aircraft (alert level "+Fmi(alert_level)+"): "+
        TrafficState::listToString(conflictAircraft(alert_level))+"\n";
  }
  return s;
}

std::string KinematicMultiBands::outputStringTrackBands() {
  std::string s="";
  std::string u = core_.parameters.getUnits("trk_step");
  double val = core_.ownship.track();
  s+="Ownship Track: "+Units::str(u,val)+"\n";
  s+="Region of Current Track: "+BandsRegion::to_string(regionOfTrack(val))+"\n";
  s+="Track Bands ["+u+","+u+"]:\n";
  for (int i=0; i < trackLength(); ++i) {
    s+="  "+track(i,u).toString()+" "+BandsRegion::to_string(trackRegion(i))+"\n";
  }
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    s+="Peripheral Track Aircraft (alert level "+Fmi(alert_level)+"): "+
        TrafficState::listToString(peripheralTrackAircraft(alert_level))+"\n";
  }
  s+="Track Resolution (right): "+Units::str(u,trackResolution(true))+"\n";
  s+="Track Resolution (left): "+Units::str(u,trackResolution(false))+"\n";
  s+="Preferred Track Direction: ";
  if (preferredTrackDirection()) {
    s+="right\n";
  } else {
    s+="left\n";
  }
  s+="Time to Track Recovery: "+Units::str("s",timeToTrackRecovery())+"\n";
  return s;
}

std::string KinematicMultiBands::outputStringGroundSpeedBands() {
  std::string s="";
  std::string u = core_.parameters.getUnits("gs_step");
  double val = core_.ownship.groundSpeed();
  s+="Ownship Ground Speed: "+Units::str(u,val)+"\n";
  s+="Region of Current Ground Speed: "+BandsRegion::to_string(regionOfGroundSpeed(val))+"\n";
  s+="Ground Speed Bands ["+u+","+u+"]:\n";
  for (int i=0; i < groundSpeedLength(); ++i) {
      s+="  "+groundSpeed(i,u).toString()+" "+BandsRegion::to_string(groundSpeedRegion(i))+"\n";
  }
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
      s+="Peripheral Ground Speed Aircraft (alert level "+Fmi(alert_level)+"): "+
              TrafficState::listToString(peripheralGroundSpeedAircraft(alert_level))+"\n";
  }
  s+="Ground Speed Resolution (up): "+Units::str(u,groundSpeedResolution(true))+"\n";
  s+="Ground Speed Resolution (down): "+Units::str(u,groundSpeedResolution(false))+"\n";
  s+="Preferred Ground Speed Direction: ";
  if (preferredGroundSpeedDirection()) {
      s+="up\n";
  } else {
      s+="down\n";
  }
  s+="Time to Ground Speed Recovery: "+Units::str("s",timeToGroundSpeedRecovery())+"\n";
  return s;
}

std::string KinematicMultiBands::outputStringVerticalSpeedBands() {
  std::string s="";
  std::string u = core_.parameters.getUnits("vs_step");
  double val = core_.ownship.verticalSpeed();
  s+="Ownship Vertical Speed: "+Units::str(u,val)+"\n";
  s+="Region of Current Vertical Speed: "+BandsRegion::to_string(regionOfVerticalSpeed(val))+"\n";
  s+="Vertical Speed Bands ["+u+","+u+"]:\n";
  for (int i=0; i < verticalSpeedLength(); ++i) {
      s+="  "+verticalSpeed(i,u).toString()+" "+ BandsRegion::to_string(verticalSpeedRegion(i))+"\n";
  }
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
      s+="Peripheral Vertical Speed Aircraft (alert level "+Fmi(alert_level)+"): "+
              TrafficState::listToString(peripheralVerticalSpeedAircraft(alert_level))+"\n";
  }
  s+="Vertical Speed Resolution (up): "+Units::str(u,verticalSpeedResolution(true))+"\n";
  s+="Vertical Speed Resolution (down): "+Units::str(u,verticalSpeedResolution(false))+"\n";
  s+="Preferred Vertical Speed Direction: ";
  if (preferredVerticalSpeedDirection()) {
      s+="up\n";
  } else {
      s+="down\n";
  }
  s+="Time to Vertical Speed Recovery: "+Units::str("s",timeToVerticalSpeedRecovery())+"\n";
  return s;
}

std::string KinematicMultiBands::outputStringAltitudeBands() {
  std::string s="";
  std::string u = core_.parameters.getUnits("alt_step");
  double val = core_.ownship.altitude();
  s+="Ownship Altitude: "+Units::str(u,val)+"\n";
  s+="Region of Current Altitude: "+BandsRegion::to_string(regionOfAltitude(val))+"\n";
  s+="Altitude Bands ["+u+","+u+"]:\n";
  for (int i=0; i < altitudeLength(); ++i) {
      s+="  "+altitude(i,u).toString()+" "+ BandsRegion::to_string(altitudeRegion(i))+"\n";
  }
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
      s+="Peripheral Altitude Aircraft (alert level "+Fmi(alert_level)+"): "+
              TrafficState::listToString(peripheralAltitudeAircraft(alert_level))+"\n";
  }
  s+="Altitude Resolution (up): "+Units::str(u,altitudeResolution(true))+"\n";
  s+="Altitude Resolution (down): "+Units::str(u,altitudeResolution(false))+"\n";
  s+="Preferred Altitude Direction: ";
  if (preferredAltitudeDirection()) {
      s+="up\n";
  } else {
      s+="down\n";
  }
  s+="Time to Altitude Recovery: "+Units::str("s",timeToAltitudeRecovery())+"\n";
  return s;
}

std::string KinematicMultiBands::outputStringLastTimeToManeuver() {
  std::string s="";
  for (TrafficState::nat i=0; i < core_.traffic.size(); ++i) {
    TrafficState ac = core_.traffic[i];
    s+="Last Times to Maneuver with Respect to "+ac.getId()+"\n";
    s+="  Last Time to Track Maneuver: "+Units::str("s",lastTimeToTrackManeuver(ac))+"\n";
    s+="  Last Time to Ground Speed Maneuver: "+Units::str("s",lastTimeToGroundSpeedManeuver(ac))+"\n";
    s+="  Last Time to Vertical Speed Maneuver: "+Units::str("s",lastTimeToVerticalSpeedManeuver(ac))+"\n";
    s+="  Last Time to Altitude Maneuver: "+Units::str("s",lastTimeToAltitudeManeuver(ac))+"\n";
  }
  return s;
}

std::string KinematicMultiBands::outputString() {
  std::string s="";
  s+=outputStringInfo();
  s+=outputStringAlerting();
  s+=outputStringTrackBands();
  s+=outputStringGroundSpeedBands();
  s+=outputStringVerticalSpeedBands();
  s+=outputStringAltitudeBands();
  s+=outputStringLastTimeToManeuver();
  return s;
}

std::string KinematicMultiBands::toPVS(int precision) {
  std::string s="";
  s += "%%% Conflict Aircraft:\n";
  s += "(: ";
  bool comma = false;
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += "("+Fmi(alert_level)+","+TrafficState::listToPVSStringList(conflictAircraft(alert_level),precision)+")";
  }
  s += " :)::list[[nat,list[string]]]\n";
  s += "%%% Region of Current Track:\n"+
      BandsRegion::to_string(trackRegion(trackRangeOf(getOwnship().track())))+"\n";
  s += "%%% Track Bands: "+Fmi(trackLength())+"\n";
  s += trk_band_.toPVS(precision)+"\n";
  s += "%%% Peripheral Track Aircraft:\n";
  s += "(: ";
  comma = false;
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += "("+Fmi(alert_level)+","+TrafficState::listToPVSStringList(peripheralTrackAircraft(alert_level),precision)+")";
  }
  s += " :)::list[[nat,list[string]]]\n";
  s += "%%% Track Resolution:\n";
  s += "("+double2PVS(trackResolution(false),precision)+
      ","+double2PVS(trackResolution(true),precision)+
      ","+Fmb(preferredTrackDirection())+")\n";
  s += "%%% Time to Track Recovery:\n"+double2PVS(timeToTrackRecovery(),2)+"\n";
  s += "%%% Last Times to Track Maneuver:\n(:";
  comma = false;
  for (TrafficState::nat i = 0; i < core_.traffic.size(); ++i) {
    TrafficState ac = core_.traffic[i];
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToTrackManeuver(ac),2);
  }
  s += " :)\n";

  s += "%%% Region of Current Ground Speed:\n"+
      BandsRegion::to_string(groundSpeedRegion(groundSpeedRangeOf(getOwnship().groundSpeed())))+"\n";
  s += "%%% Ground Speed Bands: "+Fmi(groundSpeedLength())+"\n";
  s += gs_band_.toPVS(precision)+"\n";
  s += "%%% Peripheral Ground Speed Aircraft:\n";
  s += "(: ";
  comma = false;
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += "("+Fmi(alert_level)+","+TrafficState::listToPVSStringList(peripheralGroundSpeedAircraft(alert_level),precision)+")";
  }
  s += " :)::list[[nat,list[string]]]\n";
  s += "%%% Ground Speed Resolution:\n";
  s += "("+double2PVS(groundSpeedResolution(false),precision)+
      ","+double2PVS(groundSpeedResolution(true),precision)+
      ","+Fmb(preferredGroundSpeedDirection())+")\n";
  s += "%%% Time to Ground Speed Recovery:\n"+double2PVS(timeToGroundSpeedRecovery(),2)+"\n";
  s += "%%% Last Times to Ground Speed Maneuver:\n(:";
  comma = false;
  for (TrafficState::nat i = 0; i < core_.traffic.size(); ++i) {
    TrafficState ac = core_.traffic[i];
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToGroundSpeedManeuver(ac),2);
  }
  s += " :)\n";

  s += "%%% Region of Current Vertical Speed:\n"+
      BandsRegion::to_string(verticalSpeedRegion(verticalSpeedRangeOf(getOwnship().verticalSpeed())))+"\n";
  s += "%%% Vertical Speed Bands: "+Fmi(verticalSpeedLength())+"\n";
  s += vs_band_.toPVS(precision)+"\n";
  s += "%%% Peripheral Vertical Speed Aircraft:\n";
  s += "(: ";
  comma = false;
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += "("+Fmi(alert_level)+","+TrafficState::listToPVSStringList(peripheralVerticalSpeedAircraft(alert_level),precision)+")";
  }
  s += " :)::list[[nat,list[string]]]\n";
  s += "%%% Vertical Speed Resolution:\n";
  s += "("+double2PVS(verticalSpeedResolution(false),precision)+
      ","+double2PVS(verticalSpeedResolution(true),precision)+
      ","+Fmb(preferredVerticalSpeedDirection())+")\n";
  s += "%%% Time to Vertical Speed Recovery:\n"+double2PVS(timeToVerticalSpeedRecovery(),2)+"\n";
  s += "%%% Last Times to Vertical Speed Maneuver:\n(:";
  comma = false;
  for (TrafficState::nat i = 0; i < core_.traffic.size(); ++i) {
    TrafficState ac = core_.traffic[i];
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToVerticalSpeedManeuver(ac),2);
  }
  s += " :)\n";

  s += "%%% Region of Current Altitude:\n"+
      BandsRegion::to_string(altitudeRegion(altitudeRangeOf(getOwnship().altitude())))+"\n";
  s += "%%% Altitude Bands: "+Fmi(altitudeLength())+"\n";
  s += alt_band_.toPVS(precision)+"\n";
  s += "%%% Peripheral Altitude Aircraft:\n";
  s += "(: ";
  comma = false;
  for (int alert_level=1; alert_level <= core_.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += "("+Fmi(alert_level)+","+TrafficState::listToPVSStringList(peripheralAltitudeAircraft(alert_level),precision)+")";
  }
  s += " :)::list[[nat,list[string]]]\n";
  s += "%%% Altitude Resolution:\n";
  s += "("+double2PVS(altitudeResolution(false),precision)+
      ","+double2PVS(altitudeResolution(true),precision)+
      ","+Fmb(preferredAltitudeDirection())+")\n";
  s += "%%% Time to Altitude Recovery:\n"+double2PVS(timeToAltitudeRecovery(),2)+"\n";
  s += "%%% Last Times to Altitude Maneuver:\n(:";
  comma = false;
  for (TrafficState::nat i = 0; i < core_.traffic.size(); ++i) {
    TrafficState ac = core_.traffic[i];
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToAltitudeManeuver(ac),2);
  }
  s += " :)\n";

  return s;
}

void KinematicMultiBands::linearProjection(double offset) {
  if (offset != 0) {
    core_.ownship = core_.ownship.linearProjection(offset);
    for (TrafficState::nat i = 0; i < core_.traffic.size(); ++i) {
      core_.traffic[i] = core_.traffic[i].linearProjection(offset);
    }
    reset();
  }
}

/**
 * @return lookahead time in seconds.
 */
double KinematicMultiBands::getLookaheadTime() const {
  return core_.parameters.getLookaheadTime();
}

/**
 * @return lookahead time in specified units [u].
 */
double KinematicMultiBands::getLookaheadTime(const std::string& u) const {
  return Units::to(u,getLookaheadTime());
}

/**
 * Sets lookahead time in seconds.
 */
void KinematicMultiBands::setLookaheadTime(double t) {
  core_.parameters.setLookaheadTime(t);
  reset();
}

/**
 * Sets lookahead time in specified units [u].
 */
void KinematicMultiBands::setLookaheadTime(double t, const std::string& u) {
  setLookaheadTime(Units::from(u,t));
}

bool KinematicMultiBands::isLatLon() const {
  return hasOwnship() && core_.ownship.isLatLon();
}

}

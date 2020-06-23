/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

/**
 * Objects of class "Daidalus" compute the conflict bands using
 * kinematic single-maneuver projections of the ownship and linear preditions
 * of (multiple) traffic aircraft positions. The bands consist of ranges of
 * guidance maneuvers: direction angles, horizontal speeds, vertical
 * speeds, and altitude.<p>
 *
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver. If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR/MID/FAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will corresponding alert within the corresponding alerting level thresholds.<p>
 *
 * If recovery bands are set and the ownship is in
 * a violation path, loss of separation recovery bands and recovery times are
 * computed for each type of maneuver. If the ownship immediately executes a
 * RECOVERY guidance maneuver, then the new path is conflict-free after the
 * recovery time. Furthermore, the recovery time is the minimum time for which
 * there exists a kinematic conflict-free maneuver in the future. <p>
 *
 * Note that in the case of geodetic coordinates this version of bands
 * performs an internal projection of the coordinates and velocities
 * into the Euclidean frame (see Util/Projection).  Accuracy may be
 * reduced if the traffic plans involve any segments longer than
 * Util.Projection.projectionConflictRange(lat,acc), and an error will
 * be logged if the distance between traffic and ownship exceeds
 * Util.Projection.projectionMaxRange() at any point in the lookahead
 * range.<p>
 *
 * Disclaimers: The formal proofs of the core algorithms use real numbers,
 * however these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodetic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 * The basic usage is
 * <pre>
 * Daidalus daa = new Daidalus();
 * daa.loadFromFile(<configurationfile>);
 *
 * ...
 * daa.setOwnshipState(position of ownship, velocity of ownship);
 * daa.addTrafficState(position of (one) traffic aircraft, velocity of traffic);
 * daa.addTrafficState(position of (another) traffic aircraft, velocity of traffic);
 * ...add other traffic aircraft...
 *
 * for (int i = 0; i < daa.horizontalDirectionBandsLength(); i++ ) {
 *    interval = daa.horizontalDirectionIntervalAt(i);
 *    lower_ang = intrval.low;
 *    upper_ang = intrval.up;
 *    regionType = daa.horizontalDirectionRegionAt(i);
 *    ..do something with this information..
 * }
 *
 * ...similar for horizontal speed and vertical speed...
 * </pre>
 *
 */

#include "Daidalus.h"
#include "CriteriaCore.h"
#include "UrgencyStrategy.h"
#include "IndexLevelT.h"
#include "Interval.h"
#include "TCASTable.h"
#include "Alerter.h"
#include "Constants.h"
#include "NoneUrgencyStrategy.h"
#include "DaidalusAltBands.h"
#include "DaidalusDirBands.h"
#include "DaidalusHsBands.h"
#include "DaidalusVsBands.h"
#include <vector>
#include <cmath>
#include "TrafficState.h"

namespace larcfm {

Daidalus::Daidalus() : error("Daidalus") {}

/**
 * Construct a Daidalus object with initial alerter.
 */
Daidalus::Daidalus(const Alerter& alerter) : error("Daidalus"), core_(alerter) {}

/**
 * Construct a Daidalus object with the default parameters and one alerter with the
 * given detector and T (in seconds) as the alerting time, early alerting time, and lookahead time.
 */
Daidalus::Daidalus(const Detection3D* det, double T) : error("Daidalus"), core_(det,T) {}

/* Setting for WC Definitions RTCA DO-365 */

/*
 * Set Daidalus object such that
 * - Alerting thresholds are unbuffered as defined in RTCA DO-365.
 * - Maneuver guidance logic assumes instantaneous maneuvers
 * - Bands saturate at DMOD/ZTHR
 */
void Daidalus::set_WC_DO_365() {
  clearAlerters();
  addAlerter(Alerter::DWC_Phase_I());
  setCorrectiveRegion(BandsRegion::MID);
  setInstantaneousBands();
  disableHysteresis();
  setCollisionAvoidanceBands(false);
  setCollisionAvoidanceBandsFactor(0.0);
  setMinHorizontalRecovery(0.66,"nmi");
  setMinVerticalRecovery(450,"ft");
}

/*
 * Set DAIDALUS object such that
 * - Alerting thresholds are buffered
 * - Maneuver guidance logic assumes kinematic maneuvers
 * - Turn rate is set to 3 deg/s, when type is true, and to  1.5 deg/s
 *   when type is false.
 * - Bands don't saturate until NMAC
 */
void Daidalus::set_Buffered_WC_DO_365(bool type) {
  clearAlerters();
  addAlerter(Alerter::Buffered_DWC_Phase_I());
  setCorrectiveRegion(BandsRegion::MID);
  setKinematicBands(type);
  disableHysteresis();
  setCollisionAvoidanceBands(true);
  setCollisionAvoidanceBandsFactor(0.1);
  setMinHorizontalRecovery(1.0,"nmi");
  setMinVerticalRecovery(450,"ft");
}

/* Set DAIDALUS object such that alerting logic and maneuver guidance corresponds to
 * ACCoRD's CD3D, i.e.,
 * - Separation is given by a cylinder of of diameter 5nm and height 1000ft
 * - Lookahead time and alerting time is 180s
 * - Only 1 alert level
 * - Instantaneous maneuvers */
void Daidalus::set_CD3D() {
  clearAlerters();
  addAlerter(Alerter::CD3D_SingleBands());
  setCorrectiveRegion(BandsRegion::NEAR);
  setInstantaneousBands();
  setCollisionAvoidanceBands(true);
  setCollisionAvoidanceBandsFactor(0.1);
}

/**
 * Return release version string
 */
std::string Daidalus::release() {
  return "DAIDALUS++ V-"+DaidalusParameters::VERSION+
      "-FormalATM-"+Constants::version;
}

/* Ownship and Traffic Setting */

/**
 * Returns state of ownship.
 */
const TrafficState& Daidalus::getOwnshipState() const {
  return core_.ownship;
}

/**
 * Returns state of aircraft at index idx
 */
const TrafficState& Daidalus::getAircraftStateAt(int idx) const {
  if (0 <= idx && idx <= lastTrafficIndex()) {
    if (idx == 0) {
      return core_.ownship;
    } else {
      return core_.traffic[idx-1];
    }
  } else {
    error.addError("getAircraftState: aircraft index "+Fmi(idx)+" is out of bounds");
    return TrafficState::INVALID();
  }
}

/**
 * Set ownship state and current time. Clear all traffic.
 * @param id Ownship's identifier
 * @param pos Ownship's position
 * @param vel Ownship's ground velocity
 * @param time Time stamp of ownship's state
 */
void Daidalus::setOwnshipState(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  if (!hasOwnship() || !equals(core_.ownship.getId(),id) ||
      time < getCurrentTime() ||
      time-getCurrentTime() > getHysteresisTime()) {
    // Full reset (including hysteresis) if adding a different ownship or time is
    // in the past. Note that wind is not clear.
    clearHysteresis();
    core_.set_ownship_state(id,pos,vel,time);
  } else {
    // Otherwise, reset cache values but keeps hysteresis.
    core_.set_ownship_state(id,pos,vel,time);
    stale_bands();
  }
}

/**
 * Set ownship state at time 0.0. Clear all traffic.
 * @param id Ownship's identifier
 * @param pos Ownship's position
 * @param vel Ownship's ground velocity
 */
void Daidalus::setOwnshipState(const std::string& id, const Position& pos, const Velocity& vel) {
  setOwnshipState(id,pos,vel,0.0);
}

/**
 * Add traffic state at given time.
 * If time is different from current time, traffic state is projected, past or future,
 * into current time. If it's the first aircraft, this aircraft is
 * set as the ownship.
 * @param id Aircraft's identifier
 * @param pos Aircraft's position
 * @param vel Aircraft's ground velocity
 * @param time Time stamp of aircraft's state
 * @return Aircraft's index
 */
int Daidalus::addTrafficState(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  if (lastTrafficIndex() < 0) {
    setOwnshipState(id,pos,vel,time);
    return 0;
  } else {
    int idx = core_.set_traffic_state(id,pos,vel,time);
    if (idx >= 0) {
      ++idx;
      stale_bands();
    }
    return idx;
  }
}

/**
 * Add traffic state at current time. If it's the first aircraft, this aircraft is
 * set as the ownship.
 * @param id Aircraft's identifier
 * @param pos Aircraft's position
 * @param vel Aircraft's ground velocity
 * @return Aircraft's index
 */
int Daidalus::addTrafficState(const std::string& id, const Position& pos, const Velocity& vel) {
  return addTrafficState(id,pos,vel,core_.current_time);
}


/**
 * Get index of aircraft with given name. Return -1 if no such index exists
 */
int Daidalus::aircraftIndex(const std::string& name) const {
  int idx = -1;
  if (lastTrafficIndex() >= 0) {
    if (equals(core_.ownship.getId(),name)) {
      return 0;
    }
    idx = core_.find_traffic_state(name);
    if (idx >= 0) {
      ++idx;
    }
  }
  return idx;
}

/**
 * Exchange ownship aircraft with aircraft named id.
 * EXPERT USE ONLY !!!
 */
void Daidalus::resetOwnship(const std::string& id) {
  int ac_idx = aircraftIndex(id);
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    clearHysteresis();
    core_.reset_ownship(ac_idx-1);
  } else {
    error.addError("resetOwnship: aircraft index "+Fmi(ac_idx)+" is out of bounds");
  }
}

/**
 * Remove traffic from the list of aircraft. Returns false if no aircraft was removed.
 * Ownship cannot be removed.
 * If traffic is at index i, the indices of aircraft at k > i, are shifted to k-1.
 * EXPERT USE ONLY !!!
 */
bool Daidalus::removeTrafficAircraft(const std::string& name) {
  int ac_idx = aircraftIndex(name);
  if (core_.remove_traffic(ac_idx-1)) {
    stale_bands();
    return true;
  }
  return false;
}

/**
 * Project ownship and traffic aircraft offset seconds in the future (if positive) or in the past (if negative)
 * XPERT USE ONLY !!!
 */
void Daidalus::linearProjection(double offset) {
  if (core_.linear_projection(offset)) {
    stale_bands();
  }
}

/**
 * @return true if ownship has been set
 */
bool Daidalus::hasOwnship() const {
  return core_.has_ownship();
}

/**
 * @return true if at least one traffic has been set
 */
bool Daidalus::hasTraffic() const {
  return core_.has_traffic();
}

/**
 * @return number of aircraft, including ownship.
 */
int Daidalus::numberOfAircraft() const {
  if (!hasOwnship()) {
    return 0;
  } else {
    return core_.traffic.size()+1;
  }
}

/**
 * @return last traffic index. Every traffic aircraft has an index between 1 and lastTrafficIndex.
 * The index 0 is reserved for the ownship. When lastTrafficIndex is 0, the ownship is set but no
 * traffic aircraft has been set. When lastTrafficIndex is negative, ownship has not been set.
 */
int Daidalus::lastTrafficIndex() const {
  return numberOfAircraft()-1;
}

bool Daidalus::isLatLon() const {
  return hasOwnship() && core_.ownship.isLatLon();
}

/* Current Time */

/**
 * Return currrent time in seconds. Current time is the time of the ownship.
 */
double Daidalus::getCurrentTime() const {
  return core_.current_time;
}

/**
 * Return currrent time in specified units. Current time is the time of the ownship.
 */
double Daidalus::getCurrentTime(const std::string& u) const {
  return Units::to(u,getCurrentTime());
}

/* Wind Setting */

/**
 * Get wind velocity specified in the TO direction
 */
const Velocity& Daidalus::getWindVelocityTo() const {
  return core_.wind_vector;
}

/**
 * Get wind velocity specified in the From direction
 */
Velocity Daidalus::getWindVelocityFrom() const {
  return core_.wind_vector.NegV();
}

/**
 * Set wind velocity specified in the TO direction
 * @param wind_velocity: Wind velocity specified in TO direction
 */
void Daidalus::setWindVelocityTo(const Velocity& wind_vector) {
  core_.set_wind_velocity(wind_vector);
  stale_bands();
}

/**
 * Set wind velocity specified in the From direction
 * @param nwind_velocity: Wind velocity specified in From direction
 */
void Daidalus::setWindVelocityFrom(const Velocity& nwind_vector) {
  setWindVelocityTo(nwind_vector.NegV());
}

/**
 * Set no wind velocity
 */
void Daidalus::setNoWind() {
  core_.clear_wind();
  stale_bands();
}

/* Alerter Setting */

/**
 * Set alerter of the aircraft at ac_idx to alerter_idx
 * @param ac_idx: Aircraft index between 0 (ownship) and lastTrafficIndex(), inclusive
 * @param alerter_idx: Alerter index starting from 1. The value 0 means none.
 */
void Daidalus::setAlerterIndex(int ac_idx, int alerter_idx) {
  if (0 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    if (getAircraftStateAt(ac_idx).getAlerterIndex() != alerter_idx) {
      if (ac_idx == 0 && core_.set_alerter_ownship(alerter_idx)) {
        stale_bands();
      } else if (ac_idx > 0 && core_.set_alerter_traffic(ac_idx-1,alerter_idx)) {
        stale_bands();
      }
    }
  } else {
    error.addError("setAlerterIndex: aircraft index "+Fmi(ac_idx)+" is out of bounds");
  }
  if (alerter_idx > core_.parameters.numberOfAlerters()) {
    error.addWarning("setAlerterIndex: alerter index "+Fmi(alerter_idx)+" is out of bounds");
  }
}

/**
 * Set alerter of the aircraft at ac_idx to alerter
 * @param ac_idx: Aircraft index between 0 (ownship) and lastTrafficIndex(), inclusive
 * @param alerter: Alerter identifier
 */
void Daidalus::setAlerter(int ac_idx, const std::string& alerter) {
  setAlerterIndex(ac_idx,core_.parameters.getAlerterIndex(alerter));
}

/**
 * Return alert index used for the traffic aircraft at index ac_idx.
 * The alert index depends on alerting logic. If ownship centric, it returns the
 * alert index of ownship. Otherwise, it returns the alert index of the traffic aircraft
 * at ac_idx.
 */
int Daidalus::alerterIndexBasedOnAlertingLogic(int ac_idx) {
  if (0 <= ac_idx && ac_idx <= numberOfAircraft()) {
    return core_.alerter_index_of(getAircraftStateAt(ac_idx));
  }
  return 0;
}

/**
 * Returns most severe alert level for a given aircraft. Returns 0 if either the aircraft or the alerter is undefined.
 */
int Daidalus::mostSevereAlertLevel(int ac_idx) {
  int alerter_idx = alerterIndexBasedOnAlertingLogic(ac_idx);
  if (alerter_idx > 0) {
    const Alerter& alerter = core_.parameters.getAlerterAt(alerter_idx);
    if (alerter.isValid()) {
      return alerter.mostSevereAlertLevel();
    }
  }
  return 0;
}

/* SUM Setting */

/**
 * Set horizontal position uncertainty of aircraft at index ac_idx
 * s_EW_std: East/West position standard deviation in internal units
 * s_NS_std: North/South position standard deviation in internal units
 * s_EN_std: East/North position standard deviation in internal units
 */
void Daidalus::setHorizontalPositionUncertainty(int ac_idx, double s_EW_std, double s_NS_std, double s_EN_std) {
  if (0 <= ac_idx && ac_idx <= numberOfAircraft()) {
    if (ac_idx == 0) {
      core_.ownship.setHorizontalPositionUncertainty(s_EW_std,s_NS_std,s_EN_std);
    } else {
      core_.traffic[ac_idx-1].setHorizontalPositionUncertainty(s_EW_std,s_NS_std,s_EN_std);
    }
    reset();
  }
}

/**
 * Set horizontal position uncertainty of aircraft at index ac_idx
 * s_EW_std: East/West position standard deviation in given units
 * s_NS_std: North/South position standard deviation in given units
 * s_EN_std: East/North position standard deviation in given units
 */
void Daidalus::setHorizontalPositionUncertainty(int ac_idx, double s_EW_std, double s_NS_std, double s_EN_std, const std::string& u) {
  setHorizontalPositionUncertainty(ac_idx,Units::from(u,s_EW_std),Units::from(u,s_NS_std),Units::from(u,s_EN_std));
}

/**
 * Set vertical position uncertainty of aircraft at index ac_idx
 * sz_std : Vertical position standard deviation in internal units
 */
void Daidalus::setVerticalPositionUncertainty(int ac_idx, double sz_std) {
  if (0 <= ac_idx && ac_idx <= numberOfAircraft()) {
    if (ac_idx == 0) {
      core_.ownship.setVerticalPositionUncertainty(sz_std);
    } else {
      core_.traffic[ac_idx-1].setVerticalPositionUncertainty(sz_std);
    }
    reset();
  }
}

/**
 * Set vertical position uncertainty of aircraft at index ac_idx
 * sz_std : Vertical position standard deviation in given units
 */
void Daidalus::setVerticalPositionUncertainty(int ac_idx, double sz_std, const std::string& u) {
  setVerticalPositionUncertainty(ac_idx,Units::from(u,sz_std));
}

/**
 * Set horizontal speed uncertainty of aircraft at index ac_idx
 * v_EW_std: East/West position standard deviation in internal units
 * v_NS_std: North/South position standard deviation in internal units
 * v_EN_std: East/North position standard deviation in internal units
 */
void Daidalus::setHorizontalVelocityUncertainty(int ac_idx, double v_EW_std, double v_NS_std,  double v_EN_std) {
  if (0 <= ac_idx && ac_idx <= numberOfAircraft()) {
    if (ac_idx == 0) {
      core_.ownship.setHorizontalVelocityUncertainty(v_EW_std,v_NS_std,v_EN_std);
    } else {
      core_.traffic[ac_idx-1].setHorizontalVelocityUncertainty(v_EW_std,v_NS_std,v_EN_std);
    }
    reset();
  }
}

/**
 * Set horizontal speed uncertainty of aircraft at index ac_idx
 * v_EW_std: East/West position standard deviation in given units
 * v_NS_std: North/South position standard deviation in given units
 * v_EN_std: East/North position standard deviation in given units
 */
void Daidalus::setHorizontalVelocityUncertainty(int ac_idx, double v_EW_std, double v_NS_std,  double v_EN_std, const std::string& u) {
  setHorizontalVelocityUncertainty(ac_idx,Units::from(u,v_EW_std),Units::from(u,v_NS_std),Units::from(u,v_EN_std));
}

/**
 * Set vertical speed uncertainty of aircraft at index ac_idx
 * vz_std : Vertical speed standard deviation in internal units
 */
void Daidalus::setVerticalSpeedUncertainty(int ac_idx, double vz_std) {
  if (0 <= ac_idx && ac_idx <= numberOfAircraft()) {
    if (ac_idx == 0) {
      core_.ownship.setVerticalSpeedUncertainty(vz_std);
    } else {
      core_.traffic[ac_idx-1].setVerticalSpeedUncertainty(vz_std);
    }
    reset();
  }
}

/**
 * Set vertical speed uncertainty of aircraft at index ac_idx
 * vz_std : Vertical speed standard deviation in given units
 */
void Daidalus::setVerticalSpeedUncertainty(int ac_idx, double vz_std, const std::string& u) {
  setVerticalSpeedUncertainty(ac_idx,Units::from(u,vz_std));
}

/**
 * Reset all uncertainties of aircraft at index ac_idx
 */
void Daidalus::resetUncertainty(int ac_idx) {
  if (0 <= ac_idx && ac_idx <= numberOfAircraft()) {
    if (ac_idx == 0) {
      core_.ownship.resetUncertainty();
    } else {
      core_.traffic[ac_idx-1].resetUncertainty();
    }
    reset();
  }
}

/* Urgency strategy for implicitly coordinate bands (experimental) */

/**
 * @return strategy for computing most urgent aircraft.
 */
const UrgencyStrategy* Daidalus::getUrgencyStrategy() const {
  return core_.urgency_strategy;
}

/**
 * Set strategy for computing most urgent aircraft.
 */
void Daidalus::setUrgencyStrategy(const UrgencyStrategy* strat) {
  if (core_.urgency_strategy != NULL) {
    delete core_.urgency_strategy;
  }
  core_.urgency_strategy = strat != NULL ? strat->copy() : NULL;
  reset();
}

/**
 * @return most urgent aircraft.
 */
TrafficState Daidalus::mostUrgentAircraft() {
  return core_.mostUrgentAircraft();
}

/* Computation of contours, a.k.a. blobs */

/**
 * Computes horizontal contours contributed by aircraft at index idx, for
 * given alert level. A contour is a non-empty list of points in counter-clockwise
 * direction representing a polygon.
 * @param blobs list of direction contours returned by reference.
 * @param ac_idx is the index of the aircraft used to compute the contours.
 */
void Daidalus::horizontalContours(std::vector<std::vector<Position> >& blobs, int ac_idx, int alert_level) {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    int code = core_.horizontal_contours(blobs,ac_idx-1,alert_level);
    switch (code) {
    case 1:
      error.addError("horizontalContours: detector of traffic aircraft "+Fmi(ac_idx)+" is not set");
      break;
    case 2:
      error.addError("horizontalContours: no corrective alerter level for alerter of "+Fmi(ac_idx));
      break;
    case 3:
      error.addError("horizontalContours: alerter of traffic aircraft "+Fmi(ac_idx)+" is out of bounds");
      break;
    }
  } else {
    error.addError("horizontalContours: aircraft index "+Fmi(ac_idx)+" is out of bounds");
  }
}

/**
 * Computes horizontal contours contributed by aircraft at index idx, the alert level
 * corresponding to the corrective region. A contour is a non-empty list of points in
 * counter-clockwise direction representing a polygon.
 * @param blobs list of direction contours returned by reference.
 * @param idx is the index of the aircraft used to compute the contours.
 */
void Daidalus::horizontalContours(std::vector<std::vector<Position> >& blobs, int idx) {
  horizontalContours(blobs,idx,0);
}

/* Setting and getting DaidalusParameters */

/**
 * Return number of alerters.
 */
int Daidalus::numberOfAlerters() const {
  return core_.parameters.numberOfAlerters();
}

/**
 * Return alerter at index i (starting from 1).
 */
const Alerter& Daidalus::getAlerterAt(int i) const {
  return core_.parameters.getAlerterAt(i);
}

/**
 * Return index of alerter with a given name. Return 0 if it doesn't exist
 */
int Daidalus::getAlerterIndex(std::string id) const {
  return core_.parameters.getAlerterIndex(id);
}

/**
 * Clear all alert thresholds
 */
void Daidalus::clearAlerters() {
  core_.parameters.clearAlerters();
  reset();
}

/**
 * Add alert thresholds
 */
int Daidalus::addAlerter(const Alerter& alerter) {
  int alert_idx = core_.parameters.addAlerter(alerter);
  reset();
  return alert_idx;
}

/**
 * @return lookahead time in seconds.
 */
double Daidalus::getLookaheadTime() const {
  return core_.parameters.getLookaheadTime();
}

/**
 * @return lookahead time in specified units [u].
 */
double Daidalus::getLookaheadTime(const std::string& u) const {
  return core_.parameters.getLookaheadTime(u);
}

/**
 * @return left direction in radians [0 - pi] [rad] from current ownship's direction
 */
double Daidalus::getLeftHorizontalDirection() const {
  return core_.parameters.getLeftHorizontalDirection();
}

/**
 * @return left direction in specified units [0 - pi] [u] from current ownship's direction
 */
double Daidalus::getLeftHorizontalDirection(const std::string& u) const {
  return Units::to(u,getLeftHorizontalDirection());
}

/**
 * @return right direction in radians [0 - pi] [rad] from current ownship's direction
 */
double Daidalus::getRightHorizontalDirection() const {
  return core_.parameters.getRightHorizontalDirection();
}

/**
 * @return right direction in specified units [0 - pi] [u] from current ownship's direction
 */
double Daidalus::getRightHorizontalDirection(const std::string& u) const {
  return Units::to(u,getRightHorizontalDirection());
}

/**
 * @return minimum horizontal speed for horizontal speed bands in internal units [m/s].
 */
double Daidalus::getMinHorizontalSpeed() const {
  return core_.parameters.getMinHorizontalSpeed();
}

/**
 * @return minimum horizontal speed for horizontal speed bands in specified units [u].
 */
double Daidalus::getMinHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getMinHorizontalSpeed());
}

/**
 * @return maximum horizontal speed for horizontal speed bands in internal units [m/s].
 */
double Daidalus::getMaxHorizontalSpeed() const {
  return core_.parameters.getMaxHorizontalSpeed();
}

/**
 * @return maximum horizontal speed for horizontal speed bands in specified units [u].
 */
double Daidalus::getMaxHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getMaxHorizontalSpeed());
}

/**
 * @return minimum vertical speed for vertical speed bands in internal units [m/s].
 */
double Daidalus::getMinVerticalSpeed() const {
  return core_.parameters.getMinVerticalSpeed();
}

/**
 * @return minimum vertical speed for vertical speed bands in specified units [u].
 */
double Daidalus::getMinVerticalSpeed(const std::string& u) const {
  return Units::to(u,getMinVerticalSpeed());
}

/**
 * @return maximum vertical speed for vertical speed bands in internal units [m/s].
 */
double Daidalus::getMaxVerticalSpeed() const {
  return core_.parameters.getMaxVerticalSpeed();
}

/**
 * @return maximum vertical speed for vertical speed bands in specified units [u].
 */
double Daidalus::getMaxVerticalSpeed(const std::string& u) const {
  return Units::to(u,getMaxVerticalSpeed());
}

/**
 * @return minimum altitude for altitude bands in internal units [m]
 */
double Daidalus::getMinAltitude() const {
  return core_.parameters.getMinAltitude();
}

/**
 * @return minimum altitude for altitude bands in specified units [u].
 */
double Daidalus::getMinAltitude(const std::string& u) const {
  return Units::to(u,getMinAltitude());
}

/**
 * @return maximum altitude for altitude bands in internal units [m]
 */
double Daidalus::getMaxAltitude() const {
  return core_.parameters.getMaxAltitude();
}

/**
 * @return maximum altitude for altitude bands in specified units [u].
 */
double Daidalus::getMaxAltitude(const std::string& u) const {
  return Units::to(u,getMaxAltitude());
}

/**
 * @return Horizontal speed in internal units (below current value) for the
 * computation of relative bands
 */
double Daidalus::getBelowRelativeHorizontalSpeed() const {
  return core_.parameters.getBelowRelativeHorizontalSpeed();
}

/**
 * @return Horizontal speed in given units (below current value) for the
 * computation of relative bands
 */
double Daidalus::getBelowRelativeHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getBelowRelativeHorizontalSpeed());
}

/**
 * @return Horizontal speed in internal units (above current value) for the
 * computation of relative bands
 */
double Daidalus::getAboveRelativeHorizontalSpeed() const {
  return core_.parameters.getAboveRelativeHorizontalSpeed();
}

/**
 * @return Horizontal speed in given units (above current value) for the
 * computation of relative bands
 */
double Daidalus::getAboveRelativeHorizontalSpeed(const std::string& u) const {
  return Units::to(u,getAboveRelativeHorizontalSpeed());
}

/**
 * @return Vertical speed in internal units (below current value) for the
 * computation of relative bands
 */
double Daidalus::getBelowRelativeVerticalSpeed() const {
  return core_.parameters.getBelowRelativeVerticalSpeed();
}

/**
 * @return Vertical speed in given units (below current value) for the
 * computation of relative bands
 */
double Daidalus::getBelowRelativeVerticalSpeed(const std::string& u) const {
  return Units::to(u,getBelowRelativeVerticalSpeed());
}

/**
 * @return Vertical speed in internal units (above current value) for the
 * computation of relative bands
 */
double Daidalus::getAboveRelativeVerticalSpeed() const {
  return core_.parameters.getAboveRelativeVerticalSpeed();
}

/**
 * @return Vertical speed in given units (above current value) for the
 * computation of relative bands
 */
double Daidalus::getAboveRelativeVerticalSpeed(const std::string& u) const {
  return Units::to(u,getAboveRelativeVerticalSpeed());
}

/**
 * @return Altitude in internal units (below current value) for the
 * computation of relative bands
 */
double Daidalus::getBelowRelativeAltitude() const {
  return core_.parameters.getBelowRelativeAltitude();
}

/**
 * @return Altitude in given units (below current value) for the
 * computation of relative bands
 */
double Daidalus::getBelowRelativeAltitude(const std::string& u) const {
  return Units::to(u,getBelowRelativeAltitude());
}

/**
 * @return Altitude in internal units (above current value) for the
 * computation of relative bands
 */
double Daidalus::getAboveRelativeAltitude() const {
  return core_.parameters.getAboveRelativeAltitude();
}

/**
 * @return Altitude in given units (above current value) for the
 * computation of relative bands
 */
double Daidalus::getAboveRelativeAltitude(const std::string& u) const {
  return Units::to(u,getAboveRelativeAltitude());
}

/**
 * @return step size for direction bands in internal units [rad].
 */
double Daidalus::getHorizontalDirectionStep() const {
  return core_.parameters.getHorizontalDirectionStep();
}

/**
 * @return step size for direction bands in specified units [u].
 */
double Daidalus::getHorizontalDirectionStep(const std::string& u) const {
  return core_.parameters.getHorizontalDirectionStep(u);
}

/**
 * @return step size for horizontal speed bands in internal units [m/s].
 */
double Daidalus::getHorizontalSpeedStep() const {
  return core_.parameters.getHorizontalSpeedStep();
}

/**
 * @return step size for horizontal speed bands in specified units [u].
 */
double Daidalus::getHorizontalSpeedStep(const std::string& u) const {
  return core_.parameters.getHorizontalSpeedStep(u);
}

/**
 * @return step size for vertical speed bands in internal units [m/s].
 */
double Daidalus::getVerticalSpeedStep() const {
  return core_.parameters.getVerticalSpeedStep();
}

/**
 * @return step size for vertical speed bands in specified units [u].
 */
double Daidalus::getVerticalSpeedStep(const std::string& u) const {
  return core_.parameters.getVerticalSpeedStep(u);
}

/**
 * @return step size for altitude bands in internal units [m]
 */
double Daidalus::getAltitudeStep() const {
  return core_.parameters.getAltitudeStep();
}

/**
 * @return step size for altitude bands in specified units [u].
 */
double Daidalus::getAltitudeStep(const std::string& u) const {
  return core_.parameters.getAltitudeStep(u);
}

/**
 * @return horizontal acceleration for horizontal speed bands to value in internal units [m/s^2].
 */
double Daidalus::getHorizontalAcceleration() const {
  return core_.parameters.getHorizontalAcceleration();
}

/**
 * @return horizontal acceleration for horizontal speed bands to value in specified units [u].
 */
double Daidalus::getHorizontalAcceleration(const std::string& u) const {
  return core_.parameters.getHorizontalAcceleration(u);
}

/**
 * @return constant vertical acceleration for vertical speed and altitude bands in internal [m/s^2]
 * units
 */
double Daidalus::getVerticalAcceleration() const {
  return core_.parameters.getVerticalAcceleration();
}

/**
 * @return constant vertical acceleration for vertical speed and altitude bands in specified
 * units
 */
double Daidalus::getVerticalAcceleration(const std::string& u) const {
  return core_.parameters.getVerticalAcceleration(u);
}

/**
 * @return turn rate in internal units [rad/s].
 */
double Daidalus::getTurnRate() const {
  return core_.parameters.getTurnRate();
}

/**
 * @return turn rate in specified units [u].
 */
double Daidalus::getTurnRate(const std::string& u) const {
  return core_.parameters.getTurnRate(u);
}

/**
 * @return bank angle in internal units [rad].
 */
double Daidalus::getBankAngle() const {
  return core_.parameters.getBankAngle();
}

/**
 * @return bank angle in specified units [u].
 */
double Daidalus::getBankAngle(const std::string& u) const {
  return core_.parameters.getBankAngle(u);
}

/**
 * @return the vertical climb/descend rate for altitude bands in internal units [m/s]
 */
double Daidalus::getVerticalRate() const {
  return core_.parameters.getVerticalRate();
}

/**
 * @return the vertical climb/descend rate for altitude bands in specified units [u].
 */
double Daidalus::getVerticalRate(std::string u) const {
  return core_.parameters.getVerticalRate(u);
}

/**
 * @return horizontal NMAC distance in internal units [m].
 */
double Daidalus::getHorizontalNMAC() const {
  return core_.parameters.getHorizontalNMAC();
}

/**
 * @return horizontal NMAC distance in specified units [u].
 */
double Daidalus::getHorizontalNMAC(const std::string& u) const {
  return core_.parameters.getHorizontalNMAC(u);
}

/**
 * @return vertical NMAC distance in internal units [m].
 */
double Daidalus::getVerticalNMAC() const {
  return core_.parameters.getVerticalNMAC();
}

/**
 * @return vertical NMAC distance in specified units [u].
 */
double Daidalus::getVerticalNMAC(const std::string& u) const {
  return core_.parameters.getVerticalNMAC(u);
}

/**
 * @return recovery stability time in seconds. Recovery bands are computed at time of
 * first conflict-free region plus this time.
 */
double Daidalus::getRecoveryStabilityTime() const {
  return core_.parameters.getRecoveryStabilityTime();
}

/**
 * @return recovery stability time in specified Units:: Recovery bands are computed at time of
 * first conflict-free region plus this time.
 */
double Daidalus::getRecoveryStabilityTime(const std::string& u) const {
  return core_.parameters.getRecoveryStabilityTime(u);
}

/**
 * @return hysteresis time in seconds.
 */
double Daidalus::getHysteresisTime() const {
  return core_.parameters.getHysteresisTime();
}

/**
 * @return hysteresis time in specified units [u].
 */
double Daidalus::getHysteresisTime(const std::string& u) const {
  return core_.parameters.getHysteresisTime(u);
}

/**
 * @return alerting persistence time in seconds.
 */
double Daidalus::getPersistenceTime() const {
  return core_.parameters.getPersistenceTime();
}

/**
 * @return alerting persistence time in specified units [u].
 */
double Daidalus::getPersistenceTime(const std::string& u) const {
  return core_.parameters.getPersistenceTime(u);
}

/**
 * @return true if bands persistence is enabled
 */
bool Daidalus::isEnabledBandsPersistence() const {
  return core_.parameters.isEnabledBandsPersistence();
}

/**
 * Enable/disable bands persistence
 */
void Daidalus::setBandsPersistence(bool flag) {
  core_.parameters.setBandsPersistence(flag);
  reset();
}

/**
 * Enable bands persistence
 */
void Daidalus::enableBandsPersistence() {
  setBandsPersistence(true);
}

/**
 * Disable bands persistence
 */
void Daidalus::disableBandsPersistence() {
  setBandsPersistence(false);
}

/**
 * @return persistence for preferred horizontal direction resolution in internal units
 */
double Daidalus::getPersistencePreferredHorizontalDirectionResolution() const {
  return core_.parameters.getPersistencePreferredHorizontalDirectionResolution();
}

/**
 * @return persistence for preferred horizontal direction resolution in given units
 */
double Daidalus::getPersistencePreferredHorizontalDirectionResolution(const std::string& u) const {
  return core_.parameters.getPersistencePreferredHorizontalDirectionResolution(u);
}

/**
 * @return persistence for preferred horizontal speed resolution in internal units
 */
double Daidalus::getPersistencePreferredHorizontalSpeedResolution() const {
  return core_.parameters.getPersistencePreferredHorizontalSpeedResolution();
}

/**
 * @return persistence for preferred horizontal speed resolution in given units
 */
double Daidalus::getPersistencePreferredHorizontalSpeedResolution(const std::string& u) const {
  return core_.parameters.getPersistencePreferredHorizontalSpeedResolution(u);
}

/**
 * @return persistence for preferred vertical speed resolution in internal units
 */
double Daidalus::getPersistencePreferredVerticalSpeedResolution() const {
  return core_.parameters.getPersistencePreferredVerticalSpeedResolution();
}

/**
 * @return persistence for preferred vertical speed resolution in given units
 */
double Daidalus::getPersistencePreferredVerticalSpeedResolution(const std::string& u) const {
  return core_.parameters.getPersistencePreferredVerticalSpeedResolution(u);
}

/**
 * @return persistence for preferred altitude resolution in internal units
 */
double Daidalus::getPersistencePreferredAltitudeResolution() const {
  return core_.parameters.getPersistencePreferredAltitudeResolution();
}

/**
 * @return persistence for preferred altitude resolution in given units
 */
double Daidalus::getPersistencePreferredAltitudeResolution(const std::string& u) const {
  return core_.parameters.getPersistencePreferredAltitudeResolution(u);
}

/**
 * @return Alerting parameter m of "M of N" strategy
 */
int Daidalus::getAlertingParameterM() const {
  return core_.parameters.getAlertingParameterM();
}

/**
 * @return Alerting parameter n of "M of N" strategy
 */
int Daidalus::getAlertingParameterN() const {
  return core_.parameters.getAlertingParameterM();
}

/**
 * @return minimum horizontal separation for recovery bands in internal units [m].
 */
double Daidalus::getMinHorizontalRecovery() const {
  return core_.parameters.getMinHorizontalRecovery();
}

/**
 * Return minimum horizontal separation for recovery bands in specified units [u]
 */
double Daidalus::getMinHorizontalRecovery(const std::string& u) const {
  return core_.parameters.getMinHorizontalRecovery(u);
}

/**
 * @return minimum vertical separation for recovery bands in internal units [m].
 */
double Daidalus::getMinVerticalRecovery() const {
  return core_.parameters.getMinVerticalRecovery();
}

/**
 * Return minimum vertical separation for recovery bands in specified units [u].
 */
double Daidalus::getMinVerticalRecovery(const std::string& u) const {
  return core_.parameters.getMinVerticalRecovery(u);
}

/**
 * Sets lookahead time in seconds.
 */
void Daidalus::setLookaheadTime(double t) {
  core_.parameters.setLookaheadTime(t);
  reset();
}

/**
 * Set lookahead time to value in specified units [u].
 */
void Daidalus::setLookaheadTime(double t, const std::string& u) {
  core_.parameters.setLookaheadTime(t,u);
  reset();
}

/**
 * Set left direction to value in internal units [rad]. Value is expected to be in [0 - pi]
 */
void Daidalus::setLeftHorizontalDirection(double val) {
  core_.parameters.setLeftHorizontalDirection(val);
  reset();
}

/**
 * Set left direction to value in specified units [u]. Value is expected to be in [0 - pi]
 */
void Daidalus::setLeftHorizontalDirection(double val, const std::string& u) {
  core_.parameters.setLeftHorizontalDirection(val,u);
  reset();
}

/**
 * Set right direction to value in internal units [rad]. Value is expected to be in [0 - pi]
 */
void Daidalus::setRightHorizontalDirection(double val) {
  core_.parameters.setRightHorizontalDirection(val);
  reset();
}

/**
 * Set right direction to value in specified units [u]. Value is expected to be in [0 - pi]
 */
void Daidalus::setRightHorizontalDirection(double val, const std::string& u) {
  core_.parameters.setRightHorizontalDirection(val,u);
  reset();
}

/**
 * Sets minimum horizontal speed for horizontal speed bands to value in internal units [m/s].
 */
void Daidalus::setMinHorizontalSpeed(double val) {
  core_.parameters.setMinHorizontalSpeed(val);
  reset();
}

/**
 * Sets minimum horizontal speed for horizontal speed bands to value in specified units [u].
 */
void Daidalus::setMinHorizontalSpeed(double val, const std::string& u) {
  core_.parameters.setMinHorizontalSpeed(val,u);
  reset();
}

/**
 * Sets maximum horizontal speed for horizontal speed bands to value in internal units [m/s].
 */
void Daidalus::setMaxHorizontalSpeed(double val) {
  core_.parameters.setMaxHorizontalSpeed(val);
  reset();
}

/**
 * Sets maximum horizontal speed for horizontal speed bands to value in specified units [u].
 */
void Daidalus::setMaxHorizontalSpeed(double val, const std::string& u) {
  core_.parameters.setMaxHorizontalSpeed(val,u);
  reset();
}

/**
 * Sets minimum vertical speed for vertical speed bands to value in internal units [m/s].
 */
void Daidalus::setMinVerticalSpeed(double val) {
  core_.parameters.setMinVerticalSpeed(val);
  reset();
}

/**
 * Sets minimum vertical speed for vertical speed bands to value in specified units [u].
 */
void Daidalus::setMinVerticalSpeed(double val, const std::string& u) {
  core_.parameters.setMinVerticalSpeed(val,u);
  reset();
}

/**
 * Sets maximum vertical speed for vertical speed bands to value in internal units [m/s].
 */
void Daidalus::setMaxVerticalSpeed(double val) {
  core_.parameters.setMaxVerticalSpeed(val);
  reset();
}

/**
 * Sets maximum vertical speed for vertical speed bands to value in specified units [u].
 */
void Daidalus::setMaxVerticalSpeed(double val, const std::string& u) {
  core_.parameters.setMaxVerticalSpeed(val,u);
  reset();
}

/**
 * Sets minimum altitude for altitude bands to value in internal units [m]
 */
void Daidalus::setMinAltitude(double val) {
  core_.parameters.setMinAltitude(val);
  reset();
}

/**
 * Sets minimum altitude for altitude bands to value in specified units [u].
 */
void Daidalus::setMinAltitude(double val, const std::string& u) {
  core_.parameters.setMinAltitude(val,u);
  reset();
}

/**
 * Sets maximum altitude for altitude bands to value in internal units [m]
 */
void Daidalus::setMaxAltitude(double val) {
  core_.parameters.setMaxAltitude(val);
  reset();
}

/**
 * Sets maximum altitude for altitude bands to value in specified units [u].
 */
void Daidalus::setMaxAltitude(double val, const std::string& u) {
  core_.parameters.setMaxAltitude(val,u);
  reset();
}

/**
 * Set horizontal speed in internal units (below current value) for the
 * computation of relative bands
 */
void Daidalus::setBelowRelativeHorizontalSpeed(double val) {
  core_.parameters.setBelowRelativeHorizontalSpeed(val);
  reset();
}

/**
 * Set horizontal speed in given units (below current value) for the
 * computation of relative bands
 */
void Daidalus::setBelowRelativeHorizontalSpeed(double val,std::string u) {
  core_.parameters.setBelowRelativeHorizontalSpeed(val,u);
  reset();
}

/**
 * Set horizontal speed in internal units (above current value) for the
 * computation of relative bands
 */
void Daidalus::setAboveRelativeHorizontalSpeed(double val) {
  core_.parameters.setAboveRelativeHorizontalSpeed(val);
  reset();
}

/**
 * Set horizontal speed in given units (above current value) for the
 * computation of relative bands
 */
void Daidalus::setAboveRelativeHorizontalSpeed(double val, const std::string& u) {
  core_.parameters.setAboveRelativeHorizontalSpeed(val,u);
  reset();
}

/**
 * Set vertical speed in internal units (below current value) for the
 * computation of relative bands
 */
void Daidalus::setBelowRelativeVerticalSpeed(double val) {
  core_.parameters.setBelowRelativeHorizontalSpeed(val);
  reset();
}

/**
 * Set vertical speed in given units (below current value) for the
 * computation of relative bands
 */
void Daidalus::setBelowRelativeVerticalSpeed(double val, const std::string& u) {
  core_.parameters.setBelowRelativeVerticalSpeed(val,u);
  reset();
}

/**
 * Set vertical speed in internal units (above current value) for the
 * computation of relative bands
 */
void Daidalus::setAboveRelativeVerticalSpeed(double val) {
  core_.parameters.setAboveRelativeVerticalSpeed(val);
  reset();
}

/**
 * Set vertical speed in given units (above current value) for the
 * computation of relative bands
 */
void Daidalus::setAboveRelativeVerticalSpeed(double val, const std::string& u) {
  core_.parameters.setAboveRelativeVerticalSpeed(val,u);
  reset();
}

/**
 * Set altitude in internal units (below current value) for the
 * computation of relative bands
 */
void Daidalus::setBelowRelativeAltitude(double val) {
  core_.parameters.setBelowRelativeAltitude(val);
  reset();
}

/**
 * Set altitude in given units (below current value) for the
 * computation of relative bands
 */
void Daidalus::setBelowRelativeAltitude(double val, const std::string& u) {
  core_.parameters.setBelowRelativeAltitude(val,u);
  reset();
}

/**
 * Set altitude in internal units (above current value) for the
 * computation of relative bands
 */
void Daidalus::setAboveRelativeAltitude(double val) {
  core_.parameters.setAboveRelativeAltitude(val);
  reset();
}

/**
 * Set altitude in given units (above current value) for the
 * computation of relative bands
 */
void Daidalus::setAboveRelativeAltitude(double val, const std::string& u) {
  core_.parameters.setAboveRelativeAltitude(val,u);
  reset();
}

/**
 * Set below value to min when computing horizontal speed bands
 */
void Daidalus::setBelowToMinRelativeHorizontalSpeed() {
  setBelowRelativeHorizontalSpeed(-1);
}

/**
 * Set above value to max when computing horizontal speed bands
 */
void Daidalus::setAboveToMaxRelativeHorizontalSpeed() {
  setAboveRelativeHorizontalSpeed(-1);
}

/**
 * Set below value to min when computing vertical speed bands
 */
void Daidalus::setBelowToMinRelativeVerticalSpeed() {
  setBelowRelativeVerticalSpeed(-1);
}

/**
 * Set above value to max when computing vertical speed bands
 */
void Daidalus::setAboveToMaxRelativeVerticalSpeed() {
  setAboveRelativeVerticalSpeed(-1);
}

/**
 * Set below value to min when computing altitude bands
 */
void Daidalus::setBelowToMinRelativeAltitude() {
  setBelowRelativeAltitude(-1);
}

/**
 * Set above value to max when computing altitude bands
 */
void Daidalus::setAboveToMaxRelativeAltitude() {
  setAboveRelativeAltitude(-1);
}

/**
 * Disable relative horizontal speed bands
 */
void Daidalus::disableRelativeHorizontalSpeedBands() {
  setBelowRelativeHorizontalSpeed(0);
  setAboveRelativeHorizontalSpeed(0);
}

/**
 * Disable relative vertical speed bands
 */
void Daidalus::disableRelativeVerticalSpeedBands() {
  setBelowRelativeVerticalSpeed(0);
  setAboveRelativeVerticalSpeed(0);
}

/**
 * Disable relative altitude bands
 */
void Daidalus::disableRelativeAltitude() {
  setBelowRelativeAltitude(0);
  setAboveRelativeAltitude(0);
}

/**
 * Sets step size for direction bands in internal units [rad].
 */
void Daidalus::setHorizontalDirectionStep(double val) {
  core_.parameters.setHorizontalDirectionStep(val);
  reset();
}

/**
 * Sets step size for direction bands in specified units [u].
 */
void Daidalus::setHorizontalDirectionStep(double val, const std::string& u) {
  core_.parameters.setHorizontalDirectionStep(val,u);
  reset();
}

/**
 * Sets step size for horizontal speed bands to value in internal units [m/s].
 */
void Daidalus::setHorizontalSpeedStep(double val) {
  core_.parameters.setHorizontalSpeedStep(val);
  reset();
}

/**
 * Sets step size for horizontal speed bands to value in specified units [u].
 */
void Daidalus::setHorizontalSpeedStep(double val, const std::string& u) {
  core_.parameters.setHorizontalSpeedStep(val,u);
  reset();
}

/**
 * Sets step size for vertical speed bands to value in internal units [m/s].
 */
void Daidalus::setVerticalSpeedStep(double val) {
  core_.parameters.setVerticalSpeedStep(val);
  reset();
}

/**
 * Sets step size for vertical speed bands to value in specified units [u].
 */
void Daidalus::setVerticalSpeedStep(double val, const std::string& u) {
  core_.parameters.setVerticalSpeedStep(val,u);
  reset();
}

/**
 * Sets step size for altitude bands to value in internal units [m]
 */
void Daidalus::setAltitudeStep(double val) {
  core_.parameters.setAltitudeStep(val);
  reset();
}

/**
 * Sets step size for altitude bands to value in specified units [u].
 */
void Daidalus::setAltitudeStep(double val, const std::string& u) {
  core_.parameters.setAltitudeStep(val,u);
  reset();
}

/**
 * Sets horizontal acceleration for horizontal speed bands to value in internal units [m/s^2].
 */
void Daidalus::setHorizontalAcceleration(double val) {
  core_.parameters.setHorizontalAcceleration(val);
  reset();
}

/**
 * Sets horizontal acceleration for horizontal speed bands to value in specified units [u].
 */
void Daidalus::setHorizontalAcceleration(double val, const std::string& u) {
  core_.parameters.setHorizontalAcceleration(val,u);
  reset();
}

/**
 * Sets the constant vertical acceleration for vertical speed and altitude bands
 * to value in internal units [m/s^2]
 */
void Daidalus::setVerticalAcceleration(double val) {
  core_.parameters.setVerticalAcceleration(val);
  reset();
}

/**
 * Sets the constant vertical acceleration for vertical speed and altitude bands
 * to value in specified units [u].
 */
void Daidalus::setVerticalAcceleration(double val, const std::string& u) {
  core_.parameters.setVerticalAcceleration(val,u);
  reset();
}

/**
 * Sets turn rate for direction bands to value in internal units [rad/s]. As a side effect, this method
 * resets the bank angle.
 */
void Daidalus::setTurnRate(double val) {
  core_.parameters.setTurnRate(val);
  reset();
}

/**
 * Sets turn rate for direction bands to value in specified units [u]. As a side effect, this method
 * resets the bank angle.
 */
void Daidalus::setTurnRate(double val, const std::string& u) {
  core_.parameters.setTurnRate(val,u);
  reset();
}

/**
 * Sets bank angle for direction bands to value in internal units [rad]. As a side effect, this method
 * resets the turn rate.
 */
void Daidalus::setBankAngle(double val) {
  core_.parameters.setBankAngle(val);
  reset();
}

/**
 * Sets bank angle for direction bands to value in specified units [u]. As a side effect, this method
 * resets the turn rate.
 */
void Daidalus::setBankAngle(double val, const std::string& u) {
  core_.parameters.setBankAngle(val,u);
  reset();
}

/**
 * Sets vertical rate for altitude bands to value in internal units [m/s]
 */
void Daidalus::setVerticalRate(double val) {
  core_.parameters.setVerticalRate(val);
  reset();
}

/**
 * Sets vertical rate for altitude bands to value in specified units [u].
 */
void Daidalus::setVerticalRate(double val, const std::string& u) {
  core_.parameters.setVerticalRate(val,u);
  reset();
}

/**
 * Set horizontal NMAC distance to value in internal units [m].
 */
void Daidalus::setHorizontalNMAC(double val) {
  core_.parameters.setHorizontalNMAC(val);
  reset();
}

/**
 * Set horizontal NMAC distance to value in specified units [u].
 */
void Daidalus::setHorizontalNMAC(double val, const std::string& u) {
  core_.parameters.setHorizontalNMAC(val,u);
  reset();
}

/**
 * Set vertical NMAC distance to value in internal units [m].
 */
void Daidalus::setVerticalNMAC(double val) {
  core_.parameters.setVerticalNMAC(val);
  reset();
}

/**
 * Set vertical NMAC distance to value in specified units [u].
 */
void Daidalus::setVerticalNMAC(double val, const std::string& u) {
  core_.parameters.setVerticalNMAC(val,u);
  reset();
}

/**
 * Sets recovery stability time in seconds. Recovery bands are computed at time of
 * first conflict-free region plus this time.
 */
void Daidalus::setRecoveryStabilityTime(double t) {
  core_.parameters.setRecoveryStabilityTime(t);
  reset();
}

/**
 * Sets recovery stability time in specified units. Recovery bands are computed at time of
 * first conflict-free region plus this time.
 */
void Daidalus::setRecoveryStabilityTime(double t, const std::string& u) {
  core_.parameters.setRecoveryStabilityTime(t,u);
  reset();
}

/**
 * Set hysteresis time to value in seconds.
 */
void Daidalus::setHysteresisTime(double val) {
  core_.parameters.setHysteresisTime(val);
  clearHysteresis();
}

/**
 * Set hysteresis time to value in specified units [u].
 */
void Daidalus::setHysteresisTime(double val, const std::string& u) {
  core_.parameters.setHysteresisTime(val,u);
  clearHysteresis();
}

/**
 * Set alerting persistence time to value in seconds.
 */
void Daidalus::setPersistenceTime(double val) {
  core_.parameters.setPersistenceTime(val);
  clearHysteresis();
}

/**
 * Set alerting persistence time to value in specified units [u].
 */
void Daidalus::setPersistenceTime(double val, const std::string& u) {
  core_.parameters.setPersistenceTime(val,u);
  clearHysteresis();
}

/**
 * Set persistence for preferred horizontal direction resolution in internal units
 */
void Daidalus::setPersistencePreferredHorizontalDirectionResolution(double val) {
  core_.parameters.setPersistencePreferredHorizontalDirectionResolution(val);
  reset();
}

/**
 * Set persistence for preferred horizontal direction resolution in given units
 */
void Daidalus::setPersistencePreferredHorizontalDirectionResolution(double val, const std::string& u) {
  core_.parameters.setPersistencePreferredHorizontalDirectionResolution(val,u);
  reset();
}

/**
 * Set persistence for preferred horizontal speed resolution in internal units
 */
void Daidalus::setPersistencePreferredHorizontalSpeedResolution(double val) {
  core_.parameters.setPersistencePreferredHorizontalSpeedResolution(val);
  reset();
}

/**
 * Set persistence for preferred horizontal speed resolution in given units
 */
void Daidalus::setPersistencePreferredHorizontalSpeedResolution(double val, const std::string& u) {
  core_.parameters.setPersistencePreferredHorizontalSpeedResolution(val,u);
  reset();
}

/**
 * Set persistence for preferred vertical speed resolution in internal units
 */
void Daidalus::setPersistencePreferredVerticalSpeedResolution(double val) {
  core_.parameters.setPersistencePreferredVerticalSpeedResolution(val);
  reset();
}

/**
 * Set persistence for preferred vertical speed resolution in given units
 */
void Daidalus::setPersistencePreferredVerticalSpeedResolution(double val, const std::string& u) {
  core_.parameters.setPersistencePreferredVerticalSpeedResolution(val,u);
  reset();
}

/**
 * Set persistence for preferred altitude resolution in internal units
 */
void Daidalus::setPersistencePreferredAltitudeResolution(double val) {
  core_.parameters.setPersistencePreferredAltitudeResolution(val);
  reset();
}

/**
 * Set persistence for preferred altitude resolution in given units
 */
void Daidalus::setPersistencePreferredAltitudeResolution(double val, const std::string& u) {
  core_.parameters.setPersistencePreferredAltitudeResolution(val,u);
  reset();
}

/**
 * Set alerting parameters of M of N strategy
 */
void Daidalus::setAlertingMofN(int m, int n) {
  core_.parameters.setAlertingMofN(m,n);
  clearHysteresis();
}

/**
 * Sets minimum horizontal separation for recovery bands in internal units [m].
 */
void Daidalus::setMinHorizontalRecovery(double val) {
  core_.parameters.setMinHorizontalRecovery(val);
  reset();
}

/**
 * Set minimum horizontal separation for recovery bands in specified units [u].
 */
void Daidalus::setMinHorizontalRecovery(double val, const std::string& u) {
  core_.parameters.setMinHorizontalRecovery(val,u);
  reset();
}

/**
 * Sets minimum vertical separation for recovery bands in internal units [m].
 */
void Daidalus::setMinVerticalRecovery(double val) {
  core_.parameters.setMinVerticalRecovery(val);
  reset();
}

/**
 * Set minimum vertical separation for recovery bands in units
 */
void Daidalus::setMinVerticalRecovery(double val, const std::string& u) {
  core_.parameters.setMinVerticalRecovery(val,u);
  reset();
}

/**
 * @return true if repulsive criteria is enabled for conflict bands.
 */
bool Daidalus::isEnabledConflictCriteria() {
  return core_.parameters.isEnabledConflictCriteria();
}

/**
 * Enable/disable repulsive criteria for conflict bands.
 */
void Daidalus::setConflictCriteria(bool flag) {
  core_.parameters.setConflictCriteria(flag);
  reset();
}

/**
 * Enable repulsive criteria for conflict bands.
 */
void Daidalus::enableConflictCriteria() {
  setConflictCriteria(true);
}

/**
 * Disable repulsive criteria for conflict bands.
 */
void Daidalus::disableConflictCriteria() {
  setConflictCriteria(false);
}

/**
 * @return true if repulsive criteria is enabled for recovery bands.
 */
bool Daidalus::isEnabledRecoveryCriteria() {
  return core_.parameters.isEnabledRecoveryCriteria();
}

/**
 * Enable/disable repulsive criteria for recovery bands.
 */
void Daidalus::setRecoveryCriteria(bool flag) {
  core_.parameters.setRecoveryCriteria(flag);
  reset();
}

/**
 * Enable repulsive criteria for recovery bands.
 */
void Daidalus::enableRecoveryCriteria() {
  setRecoveryCriteria(true);
}

/**
 * Disable repulsive criteria for recovery bands.
 */
void Daidalus::disableRecoveryCriteria() {
  setRecoveryCriteria(false);
}

/**
 * Enable/disable repulsive criteria for conflict and recovery bands.
 */
void Daidalus::setRepulsiveCriteria(bool flag) {
  setConflictCriteria(flag);
  setRecoveryCriteria(flag);
}

/**
 * Enable repulsive criteria for conflict and recovery bands.
 */
void Daidalus::enableRepulsiveCriteria() {
  setRepulsiveCriteria(true);
}

/**
 * Disable repulsive criteria for conflict and recovery bands.
 */
void Daidalus::disableRepulsiveCriteria() {
  setRepulsiveCriteria(false);
}

/**
 * @return true if recovery direction bands are enabled.
 */
bool Daidalus::isEnabledRecoveryHorizontalDirectionBands() {
  return core_.parameters.isEnabledRecoveryHorizontalDirectionBands();
}

/**
 * @return true if recovery horizontal speed bands are enabled.
 */
bool Daidalus::isEnabledRecoveryHorizontalSpeedBands() {
  return core_.parameters.isEnabledRecoveryHorizontalSpeedBands();
}

/**
 * @return true if recovery vertical speed bands are enabled.
 */
bool Daidalus::isEnabledRecoveryVerticalSpeedBands() {
  return core_.parameters.isEnabledRecoveryVerticalSpeedBands();
}

/**
 * @return true if recovery altitude bands are enabled.
 */
bool Daidalus::isEnabledRecoveryAltitudeBands() {
  return core_.parameters.isEnabledRecoveryAltitudeBands();
}

/**
 * Sets recovery bands flag for direction, horizontal speed, and vertical speed bands to specified value.
 */
void Daidalus::setRecoveryBands(bool flag) {
  setRecoveryHorizontalDirectionBands(flag);
  setRecoveryHorizontalSpeedBands(flag);
  setRecoveryVerticalSpeedBands(flag);
  setRecoveryAltitudeBands(flag);
}

/**
 * Enable all recovery bands for direction, horizontal speed, vertical speed, and altitude.
 */
void Daidalus::enableRecoveryBands() {
  setRecoveryBands(true);
}

/**
 * Disable all recovery bands for direction, horizontal speed, vertical speed, and altitude.
 */
void Daidalus::disableRecoveryBands() {
  setRecoveryBands(false);
}

/**
 * Sets recovery bands flag for direction bands to specified value.
 */
void Daidalus::setRecoveryHorizontalDirectionBands(bool flag) {
  core_.parameters.setRecoveryHorizontalDirectionBands(flag);
  reset();
}

/**
 * Sets recovery bands flag for horizontal speed bands to specified value.
 */
void Daidalus::setRecoveryHorizontalSpeedBands(bool flag) {
  core_.parameters.setRecoveryHorizontalSpeedBands(flag);
  reset();
}

/**
 * Sets recovery bands flag for vertical speed bands to specified value.
 */
void Daidalus::setRecoveryVerticalSpeedBands(bool flag) {
  core_.parameters.setRecoveryVerticalSpeedBands(flag);
  reset();
}

/**
 * Sets recovery bands flag for altitude bands to specified value.
 */
void Daidalus::setRecoveryAltitudeBands(bool flag) {
  core_.parameters.setRecoveryAltitudeBands(flag);
  reset();
}

/**
 * @return true if collision avoidance bands are enabled.
 */
bool Daidalus::isEnabledCollisionAvoidanceBands() {
  return core_.parameters.isEnabledCollisionAvoidanceBands();
}

/**
 * Enable/disable collision avoidance bands.
 */
void Daidalus::setCollisionAvoidanceBands(bool flag) {
  core_.parameters.setCollisionAvoidanceBands(flag);
  reset();
}

/**
 * Enable collision avoidance bands.
 */
void Daidalus::enableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(true);
}

/**
 * Disable collision avoidance bands.
 */
void Daidalus::disableCollisionAvoidanceBands() {
  setCollisionAvoidanceBands(false);
}

/**
 * @return get factor for computing collision avoidance bands. Factor value is in (0,1]
 */
double Daidalus::getCollisionAvoidanceBandsFactor() const {
  return core_.parameters.getCollisionAvoidanceBandsFactor();
}

/**
 * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
 */
void Daidalus::setCollisionAvoidanceBandsFactor(double val) {
  core_.parameters.setCollisionAvoidanceBandsFactor(val);
  reset();
}

/**
 * @return get z-score (number of standard deviations) for horizontal position
 */
double Daidalus::getHorizontalPositionZScore() const {
  return core_.parameters.getHorizontalPositionZScore();
}

/**
 * @return set z-score (number of standard deviations) for horizontal position (non-negative value)
 */
void Daidalus::setHorizontalPositionZScore(double val) {
  core_.parameters.setHorizontalPositionZScore(val);
  reset();
}

/**
 * @return get min z-score (number of standard deviations) for horizontal velocity
 */
double Daidalus::getHorizontalVelocityZScoreMin() const {
  return core_.parameters.getHorizontalVelocityZScoreMin();
}

/**
 * @return set min z-score (number of standard deviations) for horizontal velocity (non-negative value)
 */
void Daidalus::setHorizontalVelocityZScoreMin(double val) {
  core_.parameters.setHorizontalVelocityZScoreMin(val);
  reset();
}

/**
 * @return get max z-score (number of standard deviations) for horizontal velocity
 */
double Daidalus::getHorizontalVelocityZScoreMax() const {
  return core_.parameters.getHorizontalVelocityZScoreMax();
}

/**
 * @return set max z-score (number of standard deviations) for horizontal velocity (non-negative value)
 */
void Daidalus::setHorizontalVelocityZScoreMax(double val) {
  core_.parameters.setHorizontalVelocityZScoreMax(val);
  reset();
}

/**
 * @return Distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
 */
double Daidalus::getHorizontalVelocityZDistance() const {
  return core_.parameters.getHorizontalVelocityZDistance();
}

/**
 * @return Distance (in given units) at which h_vel_z_score scales from min to max as range decreases
 */
double Daidalus::getHorizontalVelocityZDistance(std::string u) const {
  return core_.parameters.getHorizontalVelocityZDistance(u);
}

/**
 * @return Set distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
 */
void Daidalus::setHorizontalVelocityZDistance(double val) {
  core_.parameters.setHorizontalVelocityZDistance(val);
  reset();
}

/**
 * @return Set distance (in given units) at which h_vel_z_score scales from min to max as range decreases
 */
void Daidalus::setHorizontalVelocityZDistance(double val, const std::string& u) {
  core_.parameters.setHorizontalVelocityZDistance(val,u);
  reset();
}

/**
 * @return get z-score (number of standard deviations) for vertical position
 */
double Daidalus::getVerticalPositionZScore() const {
  return core_.parameters.getVerticalPositionZScore();
}

/**
 * @return set z-score (number of standard deviations) for vertical position (non-negative value)
 */
void Daidalus::setVerticalPositionZScore(double val) {
  core_.parameters.setVerticalPositionZScore(val);
  reset();
}

/**
 * @return get z-score (number of standard deviations) for vertical velocity
 */
double Daidalus::getVerticalSpeedZScore() const {
  return core_.parameters.getVerticalSpeedZScore();
}

/**
 * @return set z-score (number of standard deviations) for vertical velocity (non-negative value)
 */
void Daidalus::setVerticalSpeedZScore(double val) {
  core_.parameters.setVerticalSpeedZScore(val);
  reset();
}

/**
 * Get horizontal contour threshold, specified in internal units [rad] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
double Daidalus::getHorizontalContourThreshold() const {
  return core_.parameters.getHorizontalContourThreshold();
}

/**
 * Get horizontal contour threshold, specified in given units [u] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
double Daidalus::getHorizontalContourThreshold(std::string u) const {
  return core_.parameters.getHorizontalContourThreshold(u);
}

/**
 * Set horizontal contour threshold, specified in internal units [rad] [0 - pi] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
void Daidalus::setHorizontalContourThreshold(double val) {
  core_.parameters.setHorizontalContourThreshold(val);
}

/**
 * Set horizontal contour threshold, specified in given units [u] [0 - pi] as an angle to
 * the left/right of current aircraft direction. A value of 0 means only conflict contours.
 * A value of pi means all contours.
 */
void Daidalus::setHorizontalContourThreshold(double val, const std::string& u) {
  core_.parameters.setHorizontalContourThreshold(val,u);
}

/**
 * Return true if DTA logic is active at current time
 */
bool Daidalus::isActiveDTALogic() {
  return core_.DTAStatus() != 0;
}

/**
 * Return true if DTA special maneuver guidance is active at current time
 */
bool Daidalus::isActiveDTASpecialManeuverGuidance() {
  return core_.DTAStatus() > 0;
}

/**
 * Return true if DAA Terminal Area (DTA) logic is disabled.
 */
bool Daidalus::isDisabledDTALogic() const {
  return core_.parameters.getDTALogic() == 0;
}

/**
 * Return true if DAA Terminal Area (DTA) logic is enabled with horizontal
 * direction recovery guidance. If true, horizontal direction recovery is fully enabled,
 * but vertical recovery blocks down resolutions when alert is higher than corrective.
 * NOTE:
 * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
 * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
 * intruder-centric logic).
 */
bool Daidalus::isEnabledDTALogicWithHorizontalDirRecovery() const {
  return core_.parameters.getDTALogic() > 0;
}

/**
 * Return true if DAA Terminal Area (DTA) logic is enabled without horizontal
 * direction recovery guidance. If true, horizontal direction recovery is disabled and
 * vertical recovery blocks down resolutions when alert is higher than corrective.
 * NOTE:
 * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
 * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
 * intruder-centric logic).
 */
bool Daidalus::isEnabledDTALogicWithoutHorizontalDirRecovery() const {
  return core_.parameters.getDTALogic() < 0;
}

/**
 * Disable DAA Terminal Area (DTA) logic
 */
void Daidalus::disableDTALogic() {
  core_.parameters.setDTALogic(0);
  reset();
}

/**
 * Enable DAA Terminal Area (DTA) logic with horizontal direction recovery guidance, i.e.,
 * horizontal direction recovery is fully enabled, but vertical recovery blocks down
 * resolutions when alert is higher than corrective.
 * NOTE:
 * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
 * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
 * intruder-centric logic).
 */
void Daidalus::enableDTALogicWithHorizontalDirRecovery() {
  core_.parameters.setDTALogic(1);
  reset();
}

/**
 * Enable DAA Terminal Area (DTA) logic withou horizontal direction recovery guidance, i.e.,
 * horizontal direction recovery is disabled and vertical recovery blocks down
 * resolutions when alert is higher than corrective.
 * NOTE:
 * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
 * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
 * intruder-centric logic).
 */
void Daidalus::enableDTALogicWithoutHorizontalDirRecovery() {
  core_.parameters.setDTALogic(-1);
  reset();
}

/**
 * Get DAA Terminal Area (DTA) position (lat/lon)
 */
const Position& Daidalus::getDTAPosition() const {
  return core_.parameters.getDTAPosition();
}

/**
 * Set DAA Terminal Area (DTA) latitude (internal units)
 */
void Daidalus::setDTALatitude(double lat) {
  core_.parameters.setDTALatitude(lat);
  reset();
}

/**
 * Set DAA Terminal Area (DTA) latitude in given units
 */
void Daidalus::setDTALatitude(double lat, const std::string& ulat) {
  core_.parameters.setDTALatitude(lat,ulat);
  reset();
}

/**
 * Set DAA Terminal Area (DTA) longitude (internal units)
 */
void Daidalus::setDTALongitude(double lon) {
  core_.parameters.setDTALongitude(lon);
  reset();
}

/**
 * Set DAA Terminal Area (DTA) longitude in given units
 */
void Daidalus::setDTALongitude(double lon, const std::string& ulon) {
  core_.parameters.setDTALongitude(lon,ulon);
  reset();
}

/**
 * Get DAA Terminal Area (DTA) radius (internal units)
 */
double Daidalus::getDTARadius() const {
  return core_.parameters.getDTARadius();}

/**
 * Get DAA Terminal Area (DTA) radius in given units
 */
double Daidalus::getDTARadius(const std::string& u) const {
  return core_.parameters.getDTARadius(u);
}

/**
 * Set DAA Terminal Area (DTA) radius (internal units)
 */
void Daidalus::setDTARadius(double val) {
  core_.parameters.setDTARadius(val);
  reset();
}

/**
 * Set DAA Terminal Area (DTA) radius in given units
 */
void Daidalus::setDTARadius(double val, const std::string& u) {
  core_.parameters.setDTARadius(val,u);
  reset();
}

/**
 * Get DAA Terminal Area (DTA) height (internal units)
 */
double Daidalus::getDTAHeight() const {
  return core_.parameters.getDTAHeight();
}

/**
 * Get DAA Terminal Area (DTA) height in given units
 */
double Daidalus::getDTAHeight(const std::string& u) const {
  return core_.parameters.getDTAHeight(u);
}

/**
 * Set DAA Terminal Area (DTA) height (internal units)
 */
void Daidalus::setDTAHeight(double val) {
  core_.parameters.setDTAHeight(val);
  reset();
}

/**
 * Set DAA Terminal Area (DTA) height in given units
 */
void Daidalus::setDTAHeight(double val, const std::string& u) {
  core_.parameters.setDTAHeight(val,u);
  reset();
}

/**
 * Get DAA Terminal Area (DTA) alerter
 */
int Daidalus::getDTAAlerter() const {
  return core_.parameters.getDTAAlerter();
}

/**
 * Set DAA Terminal Area (DTA) alerter
 */
void Daidalus::setDTAAlerter(int alerter) {
  core_.parameters.setDTAAlerter(alerter);
  reset();
}

/**
 * Set alerting logic to the value indicated by ownship_centric.
 * If ownship_centric is true, alerting and guidance logic will use the alerter in ownship. Alerter
 * in every intruder will be disregarded.
 * If ownship_centric is false, alerting and guidance logic will use the alerter in every intruder. Alerter
 * in ownship will be disregarded.
 */
void Daidalus::setAlertingLogic(bool ownship_centric) {
  core_.parameters.setAlertingLogic(ownship_centric);
  reset();
}

/**
 * Set alerting and guidance logic to ownship-centric. Alerting and guidance logic will use the alerter in ownship.
 * Alerter in every intruder will be disregarded.
 */
void Daidalus::setOwnshipCentricAlertingLogic() {
  setAlertingLogic(true);
}

/**
 * Set alerting and guidance logic to intruder-centric. Alerting and guidance logic will use the alerter in every intruder.
 * Alerter in ownship will be disregarded.
 */
void Daidalus::setIntruderCentricAlertingLogic() {
  setAlertingLogic(false);
}

/**
 * @return true if alerting/guidance logic is ownship centric.
 */
bool Daidalus::isAlertingLogicOwnshipCentric() const {
  return core_.parameters.isAlertingLogicOwnshipCentric();
}

/**
 * Get corrective region for calculation of resolution maneuvers and bands saturation.
 */
BandsRegion::Region Daidalus::getCorrectiveRegion() const {
  return core_.parameters.getCorrectiveRegion();
}

/**
 * Set corrective region for calculation of resolution maneuvers and bands saturation.
 */
void Daidalus::setCorrectiveRegion(BandsRegion::Region val) {
  core_.parameters.setCorrectiveRegion(val);
  reset();
}

/**
 * @param alerter_idx Indice of an alerter (starting from 1)
 * @return corrective level of alerter at alerter_idx. The corrective level
 * is the first alert level that has a region equal to or more severe than corrective_region.
 * Return -1 if alerter_idx is out of range. Return 0 is there is no corrective alert level
 * for this alerter.
 */
int Daidalus::correctiveAlertLevel(int alerter_idx) {
  return core_.parameters.correctiveAlertLevel(alerter_idx);
}

/**
 * @return maximum alert level for all alerters. Returns 0 if alerter list is empty.
 */
int Daidalus::maxAlertLevel() const {
  return core_.parameters.maxAlertLevel();
}

/**
 * Set instantaneous bands.
 */
void Daidalus::setInstantaneousBands() {
  core_.parameters.setInstantaneousBands();
  reset();
}

/**
 * Set kinematic bands.
 * Set turn rate to 3 deg/s, when type is true; set turn rate to  1.5 deg/s
 * when type is false;
 */
void Daidalus::setKinematicBands(bool type) {
  core_.parameters.setKinematicBands(type);
  reset();
}

/**
 * Disable hysteresis parameters
 */
void Daidalus::disableHysteresis() {
  core_.parameters.disableHysteresis();
  clearHysteresis();
}

/**
 *  Load parameters from file.
 */
bool Daidalus::loadFromFile(const std::string& file) {
  bool flag = core_.parameters.loadFromFile(file);
  clearHysteresis();
  return flag;
}

/**
 *  Write parameters to file.
 */
bool Daidalus::saveToFile(const std::string& file) {
  return core_.parameters.saveToFile(file);
}

/**
 * Set bands parameters
 */
void Daidalus::setDaidalusParameters(const DaidalusParameters& parameters) {
  core_.parameters = parameters;
  clearHysteresis();
}

void Daidalus::setParameterData(const ParameterData& p) {
  if (core_.parameters.setParameterData(p)) {
    clearHysteresis();
  }
}

const ParameterData Daidalus::getParameterData() {
  return core_.parameters.getParameters();
}

/**
 * Get std::string units of parameters key
 */
std::string Daidalus::getUnitsOf(const std::string& key) const {
  return core_.parameters.getUnitsOf(key);
}

/* Utility methods */

/**
 * Return core object of bands. For expert users only
 * DO NOT USE IT, UNLESS YOU KNOW WHAT YOU ARE DOING. EXPERT USE ONLY !!!
 */
const DaidalusCore& Daidalus::getCore() {
  return core_;
}

/**
 *  Clear hysteresis data
 */
void Daidalus::clearHysteresis() {
  core_.clear_hysteresis();
  hdir_band_.clear_hysteresis();
  hs_band_.clear_hysteresis();
  vs_band_.clear_hysteresis();
  alt_band_.clear_hysteresis();
}

/**
 *  Clear ownship and traffic state data from this object.
 *  IMPORTANT: This method reset cache and hysteresis parameters.
 */
void Daidalus::clear() {
  core_.clear();
  hdir_band_.clear_hysteresis();
  hs_band_.clear_hysteresis();
  vs_band_.clear_hysteresis();
  alt_band_.clear_hysteresis();
}

void Daidalus::stale_bands() {
  hdir_band_.stale();
  hs_band_.stale();
  vs_band_.stale();
  alt_band_.stale();
}

/**
 * Set cached values to stale conditions and clear hysteresis variables.
 */
void Daidalus::reset() {
  core_.stale();
  stale_bands();
}

/* Main interface methods */

/**
 * Compute in acs list of aircraft identifiers contributing to conflict bands for given region.
 * 1 = FAR, 2 = MID, 3 = NEAR
 */
void Daidalus::conflictBandsAircraft(std::vector<std::string>& acs, int region) {
  if (0 < region && region <= BandsRegion::NUMBER_OF_CONFLICT_BANDS) {
    IndexLevelT::toStringList(acs,
        core_.acs_conflict_bands(BandsRegion::NUMBER_OF_CONFLICT_BANDS-region),core_.traffic);
  } else {
    acs.clear();
  }
}

/**
 * Compute in acs list of aircraft identifiers contributing to conflict bands for given region.
 */
void Daidalus::conflictBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region) {
  conflictBandsAircraft(acs,BandsRegion::orderOfRegion(region));
}

/**
 * Return time interval of violation for given bands region
 * 1 = FAR, 2 = MID, 3 = NEAR
 */
Interval Daidalus::timeIntervalOfConflict(int region) {
  if (0 < region && region <= BandsRegion::NUMBER_OF_CONFLICT_BANDS) {
    return core_.tiov(BandsRegion::NUMBER_OF_CONFLICT_BANDS-region);
  }
  return Interval::EMPTY;
}

/**
 * Return time interval of violation for given bands region
 */
Interval Daidalus::timeIntervalOfConflict(BandsRegion::Region region) {
  return timeIntervalOfConflict(BandsRegion::orderOfRegion(region));
}

/**
 * @return the number of horizontal direction bands negative if the ownship has not been set
 */
int Daidalus::horizontalDirectionBandsLength() {
  return hdir_band_.length(core_);
}

/**
 * Force computation of direction bands. Usually, bands are only computed when needed. This method
 * forces the computation of direction bands (this method is included mainly for debugging purposes).
 */
void Daidalus::forceHorizontalDirectionBandsComputation() {
  hdir_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [rad], of the computed direction bands.
 * @param i index
 */
Interval Daidalus::horizontalDirectionIntervalAt(int i) {
  return hdir_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed direction bands.
 * @param i index
 * @param u units
 */
Interval Daidalus::horizontalDirectionIntervalAt(int i, const std::string& u) {
  Interval ia = hdir_band_.interval(core_,i);
  if (ia.isEmpty()) {
    return ia;
  }
  return Interval(Units::to(u, ia.low), Units::to(u, ia.up));
}

/**
 * @return the i-th region of the computed direction bands.
 * @param i index
 */
BandsRegion::Region Daidalus::horizontalDirectionRegionAt(int i) {
  return hdir_band_.region(core_,i);
}

/**
 * @return the index of a given direction specified in internal units [rad]
 * @param dir [rad]
 */
int Daidalus::indexOfHorizontalDirection(double dir) {
  return hdir_band_.indexOf(core_,dir);
}

/**
 * @return the index of a given direction specified in given units [u]
 * @param dir [u]
 * @param u Units
 */
int Daidalus::indexOfHorizontalDirection(double dir, const std::string& u) {
  return indexOfHorizontalDirection(Units::from(u, dir));
}

/**
 * @return the region of a given direction specified in internal units [rad].
 * @param dir [rad]
 */
BandsRegion::Region Daidalus::regionOfHorizontalDirection(double dir) {
  return horizontalDirectionRegionAt(indexOfHorizontalDirection(dir));
}

/**
 * @return the region of a given direction specified in given units [u]
 * @param dir [u]
 * @param u Units
 */
BandsRegion::Region Daidalus::regionOfHorizontalDirection(double dir, const std::string& u) {
  return horizontalDirectionRegionAt(indexOfHorizontalDirection(dir,u));
}

/**
 * Return last time to direction maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double Daidalus::lastTimeToHorizontalDirectionManeuver(const TrafficState& ac) {
  return hdir_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return recovery information for horizontal direction bands.
 */
RecoveryInformation Daidalus::horizontalDirectionRecoveryInformation() {
  return hdir_band_.recoveryInformation(core_);
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral horizontal direction bands
 * for given region.
 * 1 = FAR, 2 = MID, 3 = NEAR
 */
void Daidalus::peripheralHorizontalDirectionBandsAircraft(std::vector<std::string>& acs, int region) {
  if (0 < region && region <= BandsRegion::NUMBER_OF_CONFLICT_BANDS) {
    IndexLevelT::toStringList(acs,hdir_band_.acs_peripheral_bands(core_,BandsRegion::NUMBER_OF_CONFLICT_BANDS-region),core_.traffic);
  } else {
    acs.clear();
  }
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral horizontal direction bands
 * for given region.
 */
void Daidalus::peripheralHorizontalDirectionBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region) {
  peripheralHorizontalDirectionBandsAircraft(acs,BandsRegion::orderOfRegion(region));
}

/**
 * Compute horizontal direction resolution maneuver for a given direction.
 * @parameter dir is right (true)/left (false) of ownship current direction
 * @return direction resolution in internal units [rad] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no resolution to the right, and negative infinity if there
 * is no resolution to the left.
 */
double Daidalus::horizontalDirectionResolution(bool dir) {
  return hdir_band_.resolution(core_,dir);
}

/**
 * Compute horizontal direction resolution maneuver for a given direction.
 * @parameter dir is right (true)/left (false) of ownship current direction
 * @parameter u units
 * @return direction resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no resolution to the right, and negative infinity if there
 * is no resolution to the left.
 */
double Daidalus::horizontalDirectionResolution(bool dir, const std::string& u) {
  return Units::to(u,horizontalDirectionResolution(dir));
}

/**
 * Compute preferred horizontal direction based on resolution that is closer to current direction.
 * @return True: Right. False: Left.
 */
bool Daidalus::preferredHorizontalDirectionRightOrLeft() {
  return hdir_band_.preferred_direction(core_);
}

/**
 * @return the number of horizontal speed band intervals, negative if the ownship has not been set
 */
int Daidalus::horizontalSpeedBandsLength() {
  return hs_band_.length(core_);
}

/**
 * Force computation of horizontal speed bands. Usually, bands are only computed when needed. This method
 * forces the computation of horizontal speed bands (this method is included mainly for debugging purposes).
 */
void Daidalus::forceHorizontalSpeedBandsComputation() {
  hs_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [m/s], of the computed horizontal speed bands.
 * @param i index
 */
Interval Daidalus::horizontalSpeedIntervalAt(int i) {
  return hs_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed horizontal speed bands.
 * @param i index
 * @param u units
 */
Interval Daidalus::horizontalSpeedIntervalAt(int i, const std::string& u) {
  Interval ia = hs_band_.interval(core_,i);
  if (ia.isEmpty()) {
    return ia;
  }
  return Interval(Units::to(u, ia.low), Units::to(u, ia.up));
}

/**
 * @return the i-th region of the computed horizontal speed bands.
 * @param i index
 */
BandsRegion::Region Daidalus::horizontalSpeedRegionAt(int i) {
  return hs_band_.region(core_,i);
}

/**
 * @return the range index of a given horizontal speed specified in internal units [m/s]
 * @param gs [m/s]
 */
int Daidalus::indexOfHorizontalSpeed(double gs) {
  return hs_band_.indexOf(core_,gs);
}

/**
 * @return the range index of a given horizontal speed specified in given units [u]
 * @param gs [u]
 * @param u Units
 */
int Daidalus::indexOfHorizontalSpeed(double gs, const std::string& u) {
  return indexOfHorizontalSpeed(Units::from(u,gs));
}

/**
 * @return the region of a given horizontal speed specified in internal units [m/s]
 * @param gs [m/s]
 */
BandsRegion::Region Daidalus::regionOfHorizontalSpeed(double gs) {
  return horizontalSpeedRegionAt(indexOfHorizontalSpeed(gs));
}

/**
 * @return the region of a given horizontal speed specified in given units [u]
 * @param gs [u]
 * @param u Units
 */
BandsRegion::Region Daidalus::regionOfHorizontalSpeed(double gs, const std::string& u) {
  return horizontalSpeedRegionAt(indexOfHorizontalSpeed(gs,u));
}

/**
 * Return last time to horizontal speed maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double Daidalus::lastTimeToHorizontalSpeedManeuver(const TrafficState& ac) {
  return hs_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return recovery information for horizontal speed bands.
 */
RecoveryInformation Daidalus::horizontalSpeedRecoveryInformation() {
  return hs_band_.recoveryInformation(core_);
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral horizontal speed bands
 * for given region.
 * 1 = FAR, 2 = MID, 3 = NEAR
 */
void Daidalus::peripheralHorizontalSpeedBandsAircraft(std::vector<std::string>& acs, int region) {
  if (0 < region && region <= BandsRegion::NUMBER_OF_CONFLICT_BANDS) {
    IndexLevelT::toStringList(acs,hs_band_.acs_peripheral_bands(core_,BandsRegion::NUMBER_OF_CONFLICT_BANDS-region),core_.traffic);
  } else {
    acs.clear();
  }
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral horizontal speed bands
 * for given region.
 */
void Daidalus::peripheralHorizontalSpeedBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region) {
  peripheralHorizontalSpeedBandsAircraft(acs,BandsRegion::orderOfRegion(region));
}

/**
 * Compute horizontal speed resolution maneuver.
 * @parameter dir is up (true)/down (false) of ownship current horizontal speed
 * @return horizontal speed resolution in internal units [m/s] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double Daidalus::horizontalSpeedResolution(bool dir) {
  return hs_band_.resolution(core_,dir);
}

/**
 * Compute horizontal speed resolution maneuver for corrective region.
 * @parameter dir is up (true)/down (false) of ownship current horizontal speed
 * @parameter u units
 * @return horizontal speed resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double Daidalus::horizontalSpeedResolution(bool dir, const std::string& u) {
  return Units::to(u,horizontalSpeedResolution(dir));
}

/**
 * Compute preferred horizontal speed direction on resolution that is closer to current horizontal speed.
 * True: Increase speed, False: Decrease speed.
 */
bool Daidalus::preferredHorizontalSpeedUpOrDown() {
  return hs_band_.preferred_direction(core_);
}

/**
 * @return the number of vertical speed band intervals, negative if the ownship has not been set
 */
int Daidalus::verticalSpeedBandsLength() {
  return vs_band_.length(core_);
}

/**
 * Force computation of vertical speed bands. Usually, bands are only computed when needed. This method
 * forces the computation of vertical speed bands (this method is included mainly for debugging purposes).
 */
void Daidalus::forceVerticalSpeedBandsComputation() {
  vs_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [m/s], of the computed vertical speed bands.
 * @param i index
 */
Interval Daidalus::verticalSpeedIntervalAt(int i) {
  return vs_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed vertical speed bands.
 * @param i index
 * @param u units
 */
Interval Daidalus::verticalSpeedIntervalAt(int i, const std::string& u) {
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
BandsRegion::Region Daidalus::verticalSpeedRegionAt(int i) {
  return vs_band_.region(core_,i);
}

/**
 * @return the region of a given vertical speed specified in internal units [m/s]
 * @param vs [m/s]
 */
int Daidalus::indexOfVerticalSpeed(double vs) {
  return vs_band_.indexOf(core_,vs);
}

/**
 * @return the region of a given vertical speed specified in given units [u]
 * @param vs [u]
 * @param u Units
 */
int Daidalus::indexOfVerticalSpeed(double vs, const std::string& u) {
  return indexOfVerticalSpeed(Units::from(u, vs));
}

/**
 * @return the region of a given vertical speed specified in internal units [m/s]
 * @param vs [m/s]
 */
BandsRegion::Region Daidalus::regionOfVerticalSpeed(double vs) { //TODO shadows! verticalSpeedRegion **********************************************************************
  return verticalSpeedRegionAt(indexOfVerticalSpeed(vs));
}

/**
 * @return the region of a given vertical speed specified in given units [u]
 * @param vs [u]
 * @param u Units
 */
BandsRegion::Region Daidalus::regionOfVerticalSpeed(double vs, const std::string& u) {
  return verticalSpeedRegionAt(indexOfVerticalSpeed(vs,u));
}

/**
 * Return last time to vertical speed maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double Daidalus::lastTimeToVerticalSpeedManeuver(const TrafficState& ac) {
  return vs_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return recovery information for vertical speed bands.
 */
RecoveryInformation Daidalus::verticalSpeedRecoveryInformation() {
  return vs_band_.recoveryInformation(core_);
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral vertical speed bands
 * for given region.
 * 1 = FAR, 2 = MID, 3 = NEAR
 */
void Daidalus::peripheralVerticalSpeedBandsAircraft(std::vector<std::string>& acs, int region) {
  if (0 < region && region <= BandsRegion::NUMBER_OF_CONFLICT_BANDS) {
    IndexLevelT::toStringList(acs,vs_band_.acs_peripheral_bands(core_,BandsRegion::NUMBER_OF_CONFLICT_BANDS-region),core_.traffic);
  } else {
    acs.clear();
  }
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral vertical speed bands
 * for given region.
 */
void Daidalus::peripheralVerticalSpeedBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region) {
  peripheralVerticalSpeedBandsAircraft(acs,BandsRegion::orderOfRegion(region));
}

/**
 * Compute vertical speed resolution maneuver for given direction.
 * @parameter dir is up (true)/down (false) of ownship current vertical speed
 * @return vertical speed resolution in internal units [m/s] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double Daidalus::verticalSpeedResolution(bool dir) {
  return vs_band_.resolution(core_,dir);
}

/**
 * Compute vertical speed resolution maneuver for given direction.
 * @parameter dir is up (true)/down (false) of ownship current vertical speed
 * @parameter u units
 * @return vertical speed resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double Daidalus::verticalSpeedResolution(bool dir, const std::string& u) {
  return Units::to(u,verticalSpeedResolution(dir));
}

/**
 * Compute preferred  vertical speed direction based on resolution that is closer to current vertical speed.
 * True: Increase speed, False: Decrease speed.
 */
bool Daidalus::preferredVerticalSpeedUpOrDown() {
  return vs_band_.preferred_direction(core_);
}

/**
 * @return the number of altitude band intervals, negative if the ownship has not been set.
 */
int Daidalus::altitudeBandsLength() {
  return alt_band_.length(core_);
}

/**
 * Force computation of altitude bands. Usually, bands are only computed when needed. This method
 * forces the computation of altitude bands (this method is included mainly for debugging purposes).
 */
void Daidalus::forceAltitudeBandsComputation() {
  alt_band_.force_compute(core_);
}

/**
 * @return the i-th interval, in internal units [m], of the computed altitude bands.
 * @param i index
 */
Interval Daidalus::altitudeIntervalAt(int i) {
  return alt_band_.interval(core_,i);
}

/**
 * @return the i-th interval, in specified units [u], of the computed altitude bands.
 * @param i index
 * @param u units
 */
Interval Daidalus::altitudeIntervalAt(int i, const std::string& u) {
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
BandsRegion::Region Daidalus::altitudeRegionAt(int i) {
  return alt_band_.region(core_,i);
}

/**
 * @return the range index of a given altitude specified internal units [m]
 * @param alt [m]
 */
int Daidalus::indexOfAltitude(double alt) {
  return alt_band_.indexOf(core_,alt);
}

/**
 * @return the range index of a given altitude specified in given units [u]
 * @param alt [u]
 * @param u Units
 */
int Daidalus::indexOfAltitude(double alt, const std::string& u) {
  return indexOfAltitude(Units::from(u,alt));
}

/**
 * @return the region of a given altitude specified in internal units [m]
 * @param alt [m]
 */
BandsRegion::Region Daidalus::regionOfAltitude(double alt) {
  return altitudeRegionAt(indexOfAltitude(alt));
}

/**
 * @return the region of a given altitude specified in given units [u]
 * @param alt [u]
 * @param u Units
 */
BandsRegion::Region Daidalus::regionOfAltitude(double alt, const std::string& u) {
  return altitudeRegionAt(indexOfAltitude(alt,u));
}

/**
 * Return last time to altitude maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
 * lookahead time. Return negative infinity if there is no time to maneuver.
 */
double Daidalus::lastTimeToAltitudeManeuver(const TrafficState& ac) {
  return alt_band_.last_time_to_maneuver(core_,ac);
}

/**
 * @return recovery information for altitude speed bands.
 */
RecoveryInformation Daidalus::altitudeRecoveryInformation() {
  return alt_band_.recoveryInformation(core_);
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral altitude bands
 * for given region.
 * 1 = FAR, 2 = MID, 3 = NEAR
 */
void Daidalus::peripheralAltitudeBandsAircraft(std::vector<std::string>& acs, int region) {
  if (0 < region && region <= BandsRegion::NUMBER_OF_CONFLICT_BANDS) {
    IndexLevelT::toStringList(acs,alt_band_.acs_peripheral_bands(core_,BandsRegion::NUMBER_OF_CONFLICT_BANDS-region),core_.traffic);
  } else {
    acs.clear();
  }
}

/**
 * Compute in acs list of aircraft identifiers contributing to peripheral altitude bands
 * for given region.
 */
void Daidalus::peripheralAltitudeBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region) {
  peripheralAltitudeBandsAircraft(acs,BandsRegion::orderOfRegion(region));
}

/**
 * Compute altitude resolution maneuver for given direction.
 * @parameter dir is up (true)/down (false) of ownship current altitude
 * @return altitude resolution in internal units [m] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double Daidalus::altitudeResolution(bool dir) {
  return alt_band_.DaidalusRealBands::resolution(core_, dir);
}

/**
 * Compute altitude resolution maneuver for given direction.
 * @parameter dir is up (true)/down (false) of ownship current altitude
 * @parameter u units
 * @return altitude resolution in specified units [u] in specified direction.
 * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
 * positive infinity if there is no up resolution, and negative infinity if there
 * is no down resolution.
 */
double Daidalus::altitudeResolution(bool dir, const std::string& u) {
  return Units::to(u,altitudeResolution(dir));
}

/**
 * Compute preferred  altitude direction on resolution that is closer to current altitude.
 * True: Climb, False: Descend.
 */
bool Daidalus::preferredAltitudeUpOrDown() {
  return alt_band_.preferred_direction(core_);
}

/*
 * Alerting logic
 */

/**
 * Computes alerting type of ownship and aircraft at index ac_idx for current
 * aircraft states.  The number 0 means no alert. A negative number means
 * that aircraft index is not valid.
 * When the alerter object has been configured to consider ownship maneuvers, i.e.,
 * using spread values, the alerting logic could also use information about the ownship
 * turning, accelerating, and climbing status as follows:
 * - turning < 0: ownship is turning left, turning > 0: ownship is turning right, turning = 0:
 * do not make any turning assumption about the ownship.
 * - accelerating < 0: ownship is decelerating, accelerating > 0: ownship is accelerating,
 * accelerating = 0: do not make any accelerating assumption about the ownship.
 * - climbing < 0: ownship is descending, climbing > 0: ownship is climbing, climbing = 0:
 * do not make any climbing assumption about the ownship.
 */
int Daidalus::alertLevel(int ac_idx, int turning, int accelerating, int climbing) {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    return core_.alert_level(ac_idx-1,turning,accelerating,climbing);
  } else {
    error.addError("alertLevel: aircraft index "+Fmi(ac_idx)+" is out of bounds");
    return -1;
  }
}

/**
 * Computes alert level of ownship and aircraft at index idx.
 * The number 0 means no alert. A negative number means
 * that aircraft index is not valid.
 */
int Daidalus::alertLevel(int ac_idx) {
  return alertLevel(ac_idx,0,0,0);
}

/**
 * Detects violation of alert thresholds for a given alert level with an
 * aircraft at index ac_idx.
 * Conflict data provides time to violation and time to end of violation
 * of alert thresholds of given alert level.
 * @param ac_idx is the index of the traffic aircraft
 * @param alert_level alert level used to compute detection. The value 0
 * indicate the alert volume of the corrective region.
 */
ConflictData Daidalus::violationOfAlertThresholds(int ac_idx, int alert_level) {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    const TrafficState& intruder = core_.traffic[ac_idx-1];
    int alerter_idx = core_.alerter_index_of(intruder);
    if (1 <= alerter_idx && alerter_idx <= core_.parameters.numberOfAlerters()) {
      const Alerter& alerter = core_.parameters.getAlerterAt(alerter_idx);
      if (alert_level == 0) {
        alert_level = core_.parameters.correctiveAlertLevel(alerter_idx);
      }
      if (alert_level > 0) {
        Detection3D* detector = alerter.getDetectorPtr(alert_level);
        if (detector != NULL) {
          return detector->conflictDetectionWithTrafficState(core_.ownship,intruder,0.0,core_.parameters.getLookaheadTime());
        } else {
          error.addError("violationOfAlertThresholds: detector of traffic aircraft "+Fmi(ac_idx)+" is not set");
        }
      } else {
        error.addError("violationOfAlertThresholds: no corrective alerter level for alerter of "+Fmi(ac_idx));
      }
    } else {
      error.addError("violationOfAlertThresholds: alerter of traffic aircraft "+Fmi(ac_idx)+" is out of bounds");
    }
  } else {
    error.addError("violationOfAlertThresholds: aircraft index "+Fmi(ac_idx)+" is out of bounds");
  }
  return ConflictData::EMPTY();
}

/**
 * Detects violation of corrective thresholds with an aircraft at index ac_idx.
 * Conflict data provides time to violation and time to end of violation
 * @param ac_idx is the index of the traffic aircraft
 */
ConflictData Daidalus::violationOfCorrectiveThresholds(int ac_idx) {
  return violationOfAlertThresholds(ac_idx,0);
}

/**
 * @return time to corrective volume, in seconds, between ownship and aircraft at index idx, for the
 * corrective volume. The returned time is relative to current time. POSITIVE_INFINITY means no
 * conflict within lookahead time. NaN means aircraft index is out of range.
 * @param ac_idx is the index of the traffic aircraft
 */
double Daidalus::timeToCorrectiveVolume(int ac_idx) {
  if (1 <= ac_idx && ac_idx <= lastTrafficIndex()) {
    ConflictData det = violationOfCorrectiveThresholds(ac_idx);
    if (det.conflict()) {
      return det.getTimeIn();
    }
    return PINFINITY;
  } else {
    error.addError("timeToCorrectiveVolume: aircraft index "+Fmi(ac_idx)+" is out of bounds");
    return NaN;
  }
}

/* Getting and Setting DaidalusParameters (note that setters stale the Daidalus object) */

/* Input/Output methods */

std::string Daidalus::outputStringAircraftStates() const {
  return core_.outputStringAircraftStates(false);
}

std::string Daidalus::rawString() const {
  std::string s = "";
  s+=core_.rawString();
  s+="## Direction Bands Internals\n";
  s+=hdir_band_.rawString()+"\n";
  s+="## Horizontal Speed Bands Internals\n";
  s+=hs_band_.rawString()+"\n";
  s+="## Vertical Speed Bands Internals\n";
  s+=vs_band_.rawString()+"\n";
  s+="## Altitude Bands Internals\n";
  s+=alt_band_.rawString()+"\n";
  return s;
}

std::string Daidalus::toString() const {
  std::string s = "";
  s = "# Daidalus Object\n";
  s += core_.parameters.toString();
  if (core_.ownship.isValid()) {
    s += "###\n"+outputStringAircraftStates();
    if (core_.isFresh()) {
      s+=core_.toString();
      if (hdir_band_.isFresh()) {
        s+="## Direction Bands\n";
        s+=hdir_band_.toString();
      }
      if (hs_band_.isFresh()) {
        s+="## Horizontal Speed Bands\n";
        s+=hs_band_.toString();
      }
      if (vs_band_.isFresh()) {
        s+="## Vertical Speed Bands\n";
        s+=vs_band_.toString();
      }
      if (alt_band_.isFresh()) {
        s+="## Altitude Bands\n";
        s+=alt_band_.toString();
      }
    }
  }
  if (hasMessage()) {
    s += "###\n";
    s+= getMessageNoClear();
  }
  return s;
}

std::string Daidalus::outputStringInfo() {
  std::string s="";
  s+="Time: "+Units::str("s",core_.current_time)+"\n";
  s+= outputStringAircraftStates();
  std::string enabled = core_.parameters.isEnabledConflictCriteria()?"Enabled":"Disabled";
  s+="Conflict Criteria: "+enabled+"\n";
  enabled = core_.parameters.isEnabledRecoveryCriteria()?"Enabled":"Disabled";
  s+="Recovery Criteria: "+enabled+"\n";
  s+="Most Urgent Aircraft: "+core_.mostUrgentAircraft().getId()+"\n";
  s+="Horizontal Epsilon: "+Fmi(core_.epsilonH())+"\n";
  s+="Vertical Epsilon: "+Fmi(core_.epsilonV())+"\n";
  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    conflictBandsAircraft(acs,region);
    s+="Conflict Bands Aircraft ("+BandsRegion::to_string(region)+"): "+
        TrafficState::listToString(acs)+"\n";
  }
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    s+="Time Interval of Conflict ("+BandsRegion::to_string(region)+"): "+
        timeIntervalOfConflict(region).toStringUnits("s")+"\n";
  }
  return s;
}

std::string Daidalus::outputStringAlerting() {
  std::string s = "";
  for (int ac=1; ac <= lastTrafficIndex(); ++ac) {
    ConflictData conf = violationOfCorrectiveThresholds(ac);
    if (conf.conflict()) {
      s += "Time to Corrective Volume with "+getAircraftStateAt(ac).getId()+
          ": "+conf.getTimeInterval().toStringUnits("s")+"\n";
    }
  }
  for (int ac=1; ac <= lastTrafficIndex(); ++ac) {
    int alert_ac = alertLevel(ac);
    s += "Alert Level "+Fmi(alert_ac)+" with "+getAircraftStateAt(ac).getId()+"\n";
  }
  return s;
}


std::string Daidalus::outputStringDirectionBands() {
  std::string s="";
  std::string u  = core_.parameters.getUnitsOf("step_hdir");
  std::string uh = core_.parameters.getUnitsOf("min_horizontal_recovery");
  std::string uv = core_.parameters.getUnitsOf("min_vertical_recovery");
  double val = core_.ownship.horizontalDirection();
  s+="Ownship Horizontal Direction: "+Units::str(u,val)+"\n";
  s+="Region of Current Horizontal Direction: "+BandsRegion::to_string(regionOfHorizontalDirection(val))+"\n";
  s+="Horizontal Direction Bands:\n";
  for (int i=0; i < horizontalDirectionBandsLength(); ++i) {
    s+="  "+horizontalDirectionIntervalAt(i).toStringUnits(u)+" "+BandsRegion::to_string(horizontalDirectionRegionAt(i))+"\n";
  }
  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    peripheralHorizontalDirectionBandsAircraft(acs,region);
    s+="Peripheral Horizontal Direction Bands Aircraft ("+BandsRegion::to_string(region)+"): "+
        TrafficState::listToString(acs)+"\n";
  }
  s+="Horizontal Direction Resolution (right): "+Units::str(u,horizontalDirectionResolution(true))+"\n";
  s+="Horizontal Direction Resolution (left): "+Units::str(u,horizontalDirectionResolution(false))+"\n";
  s+="Preferred Horizontal Direction: ";
  if (preferredHorizontalDirectionRightOrLeft()) {
    s+="right\n";
  } else {
    s+="left\n";
  }
  s+="Recovery Information for Horizontal Direction Bands:\n";
  RecoveryInformation recovery = horizontalDirectionRecoveryInformation();
  s+="  Time to Recovery: "+
      Units::str("s",recovery.timeToRecovery())+"\n";
  s+="  Recovery Horizontal Distance: "+
      Units::str(uh,recovery.recoveryHorizontalDistance())+"\n";
  s+="  Recovery Vertical Distance: "+
      Units::str(uv,recovery.recoveryVerticalDistance())+"\n";
  s+="  Recovery N Factor: "+ Fmi(recovery.nFactor())+"\n";
  return s;
}

std::string Daidalus::outputStringHorizontalSpeedBands() {
  std::string s="";
  std::string u = core_.parameters.getUnitsOf("step_hs");
  std::string uh = core_.parameters.getUnitsOf("min_horizontal_recovery");
  std::string uv = core_.parameters.getUnitsOf("min_vertical_recovery");
  double val = core_.ownship.horizontalSpeed();
  s+="Ownship Horizontal Speed: "+Units::str(u,val)+"\n";
  s+="Region of Current Horizontal Speed: "+BandsRegion::to_string(regionOfHorizontalSpeed(val))+"\n";
  s+="Horizontal Speed Bands:\n";
  for (int i=0; i < horizontalSpeedBandsLength(); ++i) {
    s+="  "+horizontalSpeedIntervalAt(i).toStringUnits(u)+" "+BandsRegion::to_string(horizontalSpeedRegionAt(i))+"\n";
  }
  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    peripheralHorizontalSpeedBandsAircraft(acs,region);
    s+="Peripheral Horizontal Speed Bands Aircraft ("+BandsRegion::to_string(region)+"): "+
        TrafficState::listToString(acs)+"\n";
  }
  s+="Horizontal Speed Resolution (up): "+Units::str(u,horizontalSpeedResolution(true))+"\n";
  s+="Horizontal Speed Resolution (down): "+Units::str(u,horizontalSpeedResolution(false))+"\n";
  s+="Preferred Horizontal Speed: ";
  if (preferredHorizontalSpeedUpOrDown()) {
    s+="up\n";
  } else {
    s+="down\n";
  }
  s+="Recovery Information for Horizontal Speed Bands:\n";
  RecoveryInformation recovery = horizontalSpeedRecoveryInformation();
  s+="  Time to Recovery: "+
      Units::str("s",recovery.timeToRecovery())+"\n";
  s+="  Recovery Horizontal Distance: "+
      Units::str(uh,recovery.recoveryHorizontalDistance())+"\n";
  s+="  Recovery Vertical Distance: "+
      Units::str(uv,recovery.recoveryVerticalDistance())+"\n";
  s+="  Recovery N Factor: "+Fmi(recovery.nFactor())+"\n";
  return s;
}

std::string Daidalus::outputStringVerticalSpeedBands() {
  std::string s="";
  std::string u = core_.parameters.getUnitsOf("step_vs");
  std::string uh = core_.parameters.getUnitsOf("min_horizontal_recovery");
  std::string uv = core_.parameters.getUnitsOf("min_vertical_recovery");
  double val = core_.ownship.verticalSpeed();
  s+="Ownship Vertical Speed: "+Units::str(u,val)+"\n";
  s+="Region of Current Vertical Speed: "+BandsRegion::to_string(regionOfVerticalSpeed(val))+"\n";
  s+="Vertical Speed Bands:\n";
  for (int i=0; i < verticalSpeedBandsLength(); ++i) {
    s+="  "+verticalSpeedIntervalAt(i).toStringUnits(u)+" "+ BandsRegion::to_string(verticalSpeedRegionAt(i))+"\n";
  }
  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    peripheralVerticalSpeedBandsAircraft(acs,region);
    s+="Peripheral Vertical Speed Bands Aircraft ("+BandsRegion::to_string(region)+"): "+
        TrafficState::listToString(acs)+"\n";
  }
  s+="Vertical Speed Resolution (up): "+Units::str(u,verticalSpeedResolution(true))+"\n";
  s+="Vertical Speed Resolution (down): "+Units::str(u,verticalSpeedResolution(false))+"\n";
  s+="Preferred Vertical Speed: ";
  if (preferredVerticalSpeedUpOrDown()) {
    s+="up\n";
  } else {
    s+="down\n";
  }
  s+="Recovery Information for Vertical Speed Bands:\n";
  RecoveryInformation recovery = verticalSpeedRecoveryInformation();
  s+="  Time to Recovery: "+
      Units::str("s",recovery.timeToRecovery())+"\n";
  s+="  Recovery Horizontal Distance: "+
      Units::str(uh,recovery.recoveryHorizontalDistance())+"\n";
  s+="  Recovery Vertical Distance: "+
      Units::str(uv,recovery.recoveryVerticalDistance())+"\n";
  s+="  Recovery N Factor: "+Fmi(recovery.nFactor())+"\n";
  return s;
}

std::string Daidalus::outputStringAltitudeBands() {
  std::string s="";
  std::string u = core_.parameters.getUnitsOf("step_alt");
  std::string uh = core_.parameters.getUnitsOf("min_horizontal_recovery");
  std::string uv = core_.parameters.getUnitsOf("min_vertical_recovery");
  double val = core_.ownship.altitude();
  s+="Ownship Altitude: "+Units::str(u,val)+"\n";
  s+="Region of Current Altitude: "+BandsRegion::to_string(regionOfAltitude(val))+"\n";
  s+="Altitude Bands:\n";
  for (int i=0; i < altitudeBandsLength(); ++i) {
    s+="  "+altitudeIntervalAt(i).toStringUnits(u)+" "+ BandsRegion::to_string(altitudeRegionAt(i))+"\n";
  }
  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    peripheralAltitudeBandsAircraft(acs,region);
    s+="Peripheral Altitude Bands Aircraft ("+BandsRegion::to_string(region)+"): "+
        TrafficState::listToString(acs)+"\n";
  }
  s+="Altitude Resolution (up): "+Units::str(u,altitudeResolution(true))+"\n";
  s+="Altitude Resolution (down): "+Units::str(u,altitudeResolution(false))+"\n";
  s+="Preferred Altitude: ";
  if (preferredAltitudeUpOrDown()) {
    s+="up\n";
  } else {
    s+="down\n";
  }
  s+="Recovery Information for Altitude Bands:\n";
  RecoveryInformation recovery = altitudeRecoveryInformation();
  s+="  Time to Recovery: "+
      Units::str("s",recovery.timeToRecovery())+"\n";
  s+="  Recovery Horizontal Distance: "+
      Units::str(uh,recovery.recoveryHorizontalDistance())+"\n";
  s+="  Recovery Vertical Distance: "+
      Units::str(uv,recovery.recoveryVerticalDistance())+"\n";
  s+="  Recovery N Factor: "+Fmi(recovery.nFactor())+"\n";
  return s;
}

std::string Daidalus::outputStringLastTimeToManeuver() {
  std::string s="";
  for (int i = 0; i < static_cast<int>(core_.traffic.size()); ++i) {
    const TrafficState& ac = core_.traffic[i];
    s+="Last Times to Maneuver with Respect to "+ac.getId()+"\n";
    s+="  Last Time to Horizontal Direction Maneuver: "+Units::str("s",lastTimeToHorizontalDirectionManeuver(ac))+"\n";
    s+="  Last Time to Horizontal Speed Maneuver: "+Units::str("s",lastTimeToHorizontalSpeedManeuver(ac))+"\n";
    s+="  Last Time to Vertical Speed Maneuver: "+Units::str("s",lastTimeToVerticalSpeedManeuver(ac))+"\n";
    s+="  Last Time to Altitude Maneuver: "+Units::str("s",lastTimeToAltitudeManeuver(ac))+"\n";
  }
  return s;
}

std::string Daidalus::outputString() {
  std::string s="";
  s+=outputStringInfo();
  s+=outputStringAlerting();
  s+=outputStringDirectionBands();
  s+=outputStringHorizontalSpeedBands();
  s+=outputStringVerticalSpeedBands();
  s+=outputStringAltitudeBands();
  s+=outputStringLastTimeToManeuver();
  return s;
}

std::string Daidalus::toPVS() {
  return toPVS(true);
}

std::string Daidalus::toPVS(bool parameters) {
  bool comma;
  std::string s="%%% INPUTS %%%\n";
  if (parameters) {
    s += "%%% Parameters:\n"+core_.parameters.toPVS()+"\n";
  }
  s += "%%% Time:\n"+FmPrecision(getCurrentTime())+"\n";
  s += "%%% Aircraft List:\n"+core_.ownship.listToPVSAircraftList(core_.traffic)+"\n";
  s += "%%% Most Urgent Aircraft:\n\""+core_.mostUrgentAircraft().getId()+"\"\n";
  s += "%%% Horizontal Epsilon:\n"+Fmi(core_.epsilonH())+"\n";
  s += "%%% Vertical Epsilon:\n"+Fmi(core_.epsilonV())+"\n";
  s += "%%% Bands for Regions:\n";
  s += "(# ";
  comma = false;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += BandsRegion::to_string(region)+"_:= "+Fmb(core_.bands_for(BandsRegion::NUMBER_OF_CONFLICT_BANDS-regidx));
  }
  s += " #)\n";
  s += "%%% OUTPUTS %%%\n";
  s += "%%% Conflict Bands Aircraft (FAR,MID,NEAR):\n";
  s += "( ";
  comma = false;
  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    conflictBandsAircraft(acs,region);
    s += TrafficState::listToPVSStringList(acs);
  }
  s += " )::[list[string],list[string],list[string]]\n";
  s += "%%% Region of Current Horizontal Direction:\n"+
      BandsRegion::to_string(horizontalDirectionRegionAt(indexOfHorizontalDirection(getOwnshipState().horizontalDirection())))+"\n";
  s += "%%% Horizontal Direction Bands: "+Fmi(horizontalDirectionBandsLength())+"\n";
  s += hdir_band_.toPVS()+"\n";
  s += "%%% Peripheral Horizontal Direction Bands Aircraft (FAR,MID,NEAR):\n";
  s += "( ";
  comma = false;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    peripheralHorizontalDirectionBandsAircraft(acs,region);
    s += TrafficState::listToPVSStringList(acs);
  }
  s += " )::[list[string],list[string],list[string]]\n";
  s += "%%% Horizontal Direction Resolution:\n";
  s += "("+double2PVS(horizontalDirectionResolution(false))+
      ","+double2PVS(horizontalDirectionResolution(true))+
      ","+Fmb(preferredHorizontalDirectionRightOrLeft())+")\n";
  RecoveryInformation recovery = horizontalDirectionRecoveryInformation();
  s += "%%% Horizontal Recovery Information:\n"+recovery.toPVS()+"\n";
  s += "%%% Last Times to Direction Maneuver wrt Traffic Aircraft:\n(:";
  comma = false;
  for (int ac_idx = 0; ac_idx < static_cast<int>(core_.traffic.size()); ++ac_idx) {
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToHorizontalDirectionManeuver(core_.traffic[ac_idx]));
  }
  s += " :)\n";

  s += "%%% Region of Current Horizontal Speed:\n"+
      BandsRegion::to_string(horizontalSpeedRegionAt(indexOfHorizontalSpeed(getOwnshipState().horizontalSpeed())))+"\n";
  s += "%%% Horizontal Speed Bands: "+Fmi(horizontalSpeedBandsLength())+"\n";
  s += hs_band_.toPVS()+"\n";
  s += "%%% Peripheral Horizontal Speed Bands Aircraft (FAR,MID,NEAR):\n";
  s += "( ";
  comma = false;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    peripheralHorizontalSpeedBandsAircraft(acs,region);
    s += TrafficState::listToPVSStringList(acs);
  }
  s += " )::[list[string],list[string],list[string]]\n";
  s += "%%% Horizontal Speed Resolution:\n";
  s += "("+double2PVS(horizontalSpeedResolution(false))+
      ","+double2PVS(horizontalSpeedResolution(true))+
      ","+Fmb(preferredHorizontalSpeedUpOrDown())+")\n";
  recovery = horizontalSpeedRecoveryInformation();
  s += "%%% Horizontal Speed Information:\n"+recovery.toPVS()+"\n";
  s += "%%% Last Times to Horizontal Speed Maneuver wrt Traffic Aircraft:\n(:";
  comma = false;
  for (int ac_idx = 0; ac_idx < static_cast<int>(core_.traffic.size()); ++ac_idx) {
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToHorizontalSpeedManeuver(core_.traffic[ac_idx]));
  }
  s += " :)\n";

  s += "%%% Region of Current Vertical Speed:\n"+
      BandsRegion::to_string(verticalSpeedRegionAt(indexOfVerticalSpeed(getOwnshipState().verticalSpeed())))+"\n";
  s += "%%% Vertical Speed Bands: "+Fmi(verticalSpeedBandsLength())+"\n";
  s += vs_band_.toPVS()+"\n";
  s += "%%% Peripheral Vertical Speed Bands Aircraft (FAR,MID,NEAR):\n";
  s += "( ";
  comma = false;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    peripheralVerticalSpeedBandsAircraft(acs,region);
    s += TrafficState::listToPVSStringList(acs);
  }
  s += " )::[list[string],list[string],list[string]]\n";
  s += "%%% Vertical Speed Resolution:\n";
  s += "("+double2PVS(verticalSpeedResolution(false))+
      ","+double2PVS(verticalSpeedResolution(true))+
      ","+Fmb(preferredVerticalSpeedUpOrDown())+")\n";
  recovery = verticalSpeedRecoveryInformation();
  s += "%%% Vertical Speed Information:\n"+recovery.toPVS()+"\n";
  s += "%%% Last Times to Vertical Speed Maneuver wrt Traffic Aircraft:\n(:";
  comma = false;
  for (int ac_idx = 0; ac_idx < static_cast<int>(core_.traffic.size()); ++ac_idx) {
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToVerticalSpeedManeuver(core_.traffic[ac_idx]));
  }
  s += " :)\n";

  s += "%%% Region of Current Altitude:\n"+
      BandsRegion::to_string(altitudeRegionAt(indexOfAltitude(getOwnshipState().altitude())))+"\n";
  s += "%%% Altitude Bands: "+Fmi(altitudeBandsLength())+"\n";
  s += alt_band_.toPVS()+"\n";
  s += "%%% Peripheral Altitude Bands Aircraft (FAR,MID,NEAR):\n";
  s += "( ";
  comma = false;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    peripheralAltitudeBandsAircraft(acs,region);
    s += TrafficState::listToPVSStringList(acs);
  }
  s += " )::[list[string],list[string],list[string]]\n";
  s += "%%% Altitude Resolution:\n";
  s += "("+double2PVS(altitudeResolution(false))+
      ","+double2PVS(altitudeResolution(true))+
      ","+Fmb(preferredAltitudeUpOrDown())+")\n";
  recovery = altitudeRecoveryInformation();
  s += "%%% Altitude Information:\n"+recovery.toPVS()+"\n";
  s += "%%% Last Times to Altitude Maneuver wrt Traffic Aircraft:\n(:";
  comma = false;
  for (int ac_idx = 0; ac_idx < static_cast<int>(core_.traffic.size()); ++ac_idx) {
    if (comma) {
      s += ",";
    } else {
      comma = true;
    }
    s += " "+double2PVS(lastTimeToAltitudeManeuver(core_.traffic[ac_idx]));
  }
  s += " :)\n";

  s += "%%% Time to Corrective Volume:\n";
  s += "(: ";
  comma = false;
  for (int ac=1; ac <= lastTrafficIndex(); ++ac) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    ConflictData conf = violationOfCorrectiveThresholds(ac);
    s += "("+FmPrecision(conf.getTimeIn())+","+FmPrecision(conf.getTimeOut())+")";
  }
  s += " :)\n";

  s += "%%% Alerting:\n";
  s += "(: ";
  comma = false;
  for (int ac=1; ac <= lastTrafficIndex(); ++ac) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += "(\""+core_.traffic[ac-1].getId()+"\","+Fmi(alertLevel(ac))+")";
  }
  s += " :)\n";
  return s;
}

// ErrorReporter Interface Methods

bool Daidalus::hasError() const {
  return error.hasError() || core_.parameters.hasError();
}

bool Daidalus::hasMessage() const {
  return error.hasMessage() || core_.parameters.hasMessage();
}

std::string Daidalus::getMessage() {
  return error.getMessage()+"; "+core_.parameters.getMessage();
}

std::string Daidalus::getMessageNoClear() const {
  return error.getMessageNoClear()+"; "+core_.parameters.getMessageNoClear();
}

}

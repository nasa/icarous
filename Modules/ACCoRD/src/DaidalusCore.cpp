/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusCore.h"
#include "DaidalusDirBands.h"
#include "DaidalusHsBands.h"
#include "DaidalusVsBands.h"
#include "DaidalusAltBands.h"
#include "CriteriaCore.h"
#include "UrgencyStrategy.h"
#include "IndexLevelT.h"
#include "Interval.h"
#include "TCASTable.h"
#include "Alerter.h"
#include "NoneUrgencyStrategy.h"
#include <vector>
#include <string>
#include <cmath>
#include "TrafficState.h"

namespace larcfm {

void DaidalusCore::init() {
  // Public variables are initialized
  // ownship = TrafficState::INVALID();      // Default initialization
  current_time = 0;
  // wind_vector = Velocity.ZERO;  // Default initialization
  urgency_strategy = new NoneUrgencyStrategy();

  // Cached arrays_ are initialized
  acs_conflict_bands_ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);

  // Cached_ variables are cleared
  cache_ = 0;
  stale(true);
}

DaidalusCore::DaidalusCore() {
  init();
}

DaidalusCore::DaidalusCore(const Alerter& alerter) {
  init();
  parameters.addAlerter(alerter);
}

DaidalusCore::DaidalusCore(const Detection3D* det, double T) {
  init();
  parameters.addAlerter(Alerter::SingleBands(det,T,T));
  parameters.setLookaheadTime(T);
}

DaidalusCore::DaidalusCore(const DaidalusCore& core) {
  // Cached arrays_ are initialized
  acs_conflict_bands_ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);
  copyFrom(core);
}

void DaidalusCore::copyFrom(const DaidalusCore& core) {
  if (&core != this) {
    ownship = core.ownship;
    traffic = core.traffic;
    current_time = core.current_time;
    wind_vector = core.wind_vector;
    parameters = core.parameters;
    if (urgency_strategy != NULL) {
      delete urgency_strategy;
    }
    urgency_strategy = core.urgency_strategy != NULL ? core.urgency_strategy->copy() : NULL;
    // Cached_ variables are cleared
    cache_ = 0;
    stale(true);
  }
}

DaidalusCore& DaidalusCore::operator=(const DaidalusCore& core) {
  copyFrom(core);
  return *this;
}

/**
 *  Clear ownship and traffic data from this object.
 */
void DaidalusCore::clear() {
  ownship = TrafficState::INVALID();
  traffic.clear();
  current_time = 0;
  wind_vector = Velocity::ZEROV();
  stale(false);
}

/**
 * Set cached values to stale conditions as they are no longer fresh.
 * If hysteresis is true, it also clears hysteresis variables
 */
void DaidalusCore::stale(bool hysteresis) {
  if (cache_ >= 0) {
    cache_ = -1;
    most_urgent_ac_ = TrafficState::INVALID();
    epsh_ = 0;
    epsv_ = 0;
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      acs_conflict_bands_[conflict_region].clear();
      tiov_[conflict_region] = Interval::EMPTY;
      bands4region_[conflict_region] = false;
    }
  }
  if (hysteresis) {
    alerting_hysteresis_acs_.clear();
  }
}

/**
 * stale hysteresis of given aircraft index (1-based index, where 0 is ownship)
 */
void DaidalusCore::reset_hysteresis(int ac_idx) {
  if (ac_idx == 0) {
    std::map<std::string,AlertingHysteresis>::iterator alerting_hysteresis_ptr;
    for (alerting_hysteresis_ptr = alerting_hysteresis_acs_.begin();alerting_hysteresis_ptr != alerting_hysteresis_acs_.end();++alerting_hysteresis_ptr) {
      alerting_hysteresis_ptr->second.resetIfCurrentTime(current_time);
    }
  } else if (1 <= ac_idx && ac_idx <= static_cast<int>(traffic.size())) {
    const TrafficState& ac = traffic[ac_idx-1];
    if (ac.isValid()) {
      std::map<std::string,AlertingHysteresis>::iterator alerting_hysteresis_ptr = alerting_hysteresis_acs_.find(ac.getId());
      if (alerting_hysteresis_ptr != alerting_hysteresis_acs_.end()) {
        alerting_hysteresis_ptr->second.resetIfCurrentTime(current_time);
      }
    }
  }
}

/**
 * Returns true is object is fresh
 */
bool DaidalusCore::isFresh() const {
  return cache_ > 0;
}

/**
 *  Refresh cached values
 */
void DaidalusCore::refresh() {
  if (cache_ <= 0) {
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      conflict_aircraft(conflict_region);
      BandsRegion::Region region = BandsRegion::regionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
      for (int alerter_idx=1;  alerter_idx <= parameters.numberOfAlerters(); ++alerter_idx) {
        const Alerter& alerter = parameters.getAlerterAt(parameters.isAlertingLogicOwnshipCentric() ?
            ownship.getAlerterIndex() :alerter_idx);
        int alert_level = alerter.alertLevelForRegion(region);
        if (alert_level > 0) {
          bands4region_[conflict_region] = true;
        }
      }
    }
    refresh_mua_eps();
    cache_ = 1;
  }
}

void DaidalusCore::refresh_mua_eps() {
  if (cache_ < 0) {
    int muac = -1;
    if (!traffic.empty()) {
      muac = urgency_strategy->mostUrgentAircraft(ownship, traffic, parameters.getLookaheadTime());
    }
    if (muac >= 0) {
      most_urgent_ac_ = traffic[muac];
    } else {
      most_urgent_ac_ = TrafficState::INVALID();
    }
    epsh_ = epsilonH(ownship,most_urgent_ac_);
    epsv_ = epsilonV(ownship,most_urgent_ac_);
    cache_ = 0;
  }
}

/**
 * @return most urgent aircraft for implicit coordination
 */
const TrafficState& DaidalusCore::mostUrgentAircraft() {
  refresh_mua_eps();
  return most_urgent_ac_;
}

/**
 * Returns horizontal epsilon for implicit coordination with respect to criteria ac.
 *
 */
int DaidalusCore::epsilonH() {
  refresh_mua_eps();
  return epsh_;
}

/**
 * Returns vertical epsilon for implicit coordination with respect to criteria ac.
 */
int DaidalusCore::epsilonV() {
  refresh_mua_eps();
  return epsv_;
}

/**
 * Returns horizontal epsilon for implicit coordination with respect to criteria ac.
 *
 */
int DaidalusCore::epsilonH(bool recovery_case, const TrafficState& traffic) {
  refresh_mua_eps();
  if ((recovery_case? parameters.isEnabledRecoveryCriteria() : parameters.isEnabledConflictCriteria()) &&
      traffic.sameId(most_urgent_ac_)) {
    return epsh_;
  }
  return 0;
}

/**
 * Returns vertical epsilon for implicit coordination with respect to criteria ac.
 */
int DaidalusCore::epsilonV(bool recovery_case, const TrafficState& traffic) {
  refresh_mua_eps();
  if ((recovery_case? parameters.isEnabledRecoveryCriteria() : parameters.isEnabledConflictCriteria()) &&
      traffic.sameId(most_urgent_ac_)) {
    return epsv_;
  }
  return 0;
}

/**
 * Return true if bands are computed for this particular region (0:NEAR, 1:MID, 2: FAR)
 */
bool DaidalusCore::bands_for(int region) {
  refresh();
  return bands4region_[region];
}

/**
 * Returns actual minimum horizontal separation for recovery bands in internal units.
 */
double DaidalusCore::minHorizontalRecovery() const {
  double min_horizontal_recovery = parameters.getMinHorizontalRecovery();
  if (min_horizontal_recovery > 0)
    return min_horizontal_recovery;
  int sl = !has_ownship() ? 3 : Util::max(3,TCASTable::TCASII_RA().getSensitivityLevel(ownship.altitude()));
  return TCASTable::TCASII_RA().getHMD(sl);
}

/**
 * Returns actual minimum vertical separation for recovery bands in internal units.
 */
double DaidalusCore::minVerticalRecovery() const {
  double min_vertical_recovery = parameters.getMinVerticalRecovery();
  if (min_vertical_recovery > 0)
    return min_vertical_recovery;
  int sl = !has_ownship() ? 3 : Util::max(3,TCASTable::TCASII_RA().getSensitivityLevel(ownship.altitude()));
  return TCASTable::TCASII_RA().getZTHR(sl);
}

void DaidalusCore::set_ownship_state(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  ownship = TrafficState::makeOwnship(id,pos,vel);
  ownship.applyWindVector(wind_vector);
  current_time = time;
}

// Return 0-based index in traffic list (-1 if aircraft doesn't exist)
int DaidalusCore::find_traffic_state(const std::string& id) const {
  for (int i = 0; i < static_cast<int>(traffic.size()); ++i) {
    if (equals(traffic[i].getId(),id)) {
      return i;
    }
  }
  return -1;
}

// Return 0-based index in traffic list where aircraft was added. Return -1 if
// nothing is done (e.g., id is the same as ownship's)
int DaidalusCore::set_traffic_state(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  if (ownship.isValid() && equals(ownship.getId(),id)) {
    return -1;
  }
  double dt = current_time-time;
  Position pt = dt == 0 ? pos : pos.linear(vel,dt);
  TrafficState ac = ownship.makeIntruder(id,pt,vel);
  if (ac.isValid()) {
    ac.applyWindVector(wind_vector);
    int idx = find_traffic_state(id);
    if (idx >= 0) {
      traffic[idx]=ac;
    } else {
      idx = traffic.size();
      traffic.push_back(ac);
    }
    return idx;
  } else {
    return -1;
  }
}

// idx is 0-based index in traffic list
void DaidalusCore::reset_ownship(int idx) {
  TrafficState old_own = ownship;
  ownship = traffic[idx];
  ownship.setAsOwnship();
  old_own.setAsIntruderOf(ownship);
  for (int i = 0; i < (int)traffic.size(); ++i) {
    if (i == idx) {
      traffic[i] = old_own;
    } else {
      traffic[i].setAsIntruderOf(ownship);
    }
  }
}

void DaidalusCore::set_wind_velocity(const Velocity& wind) {
  if (has_ownship()) {
    ownship.applyWindVector(wind);
    std::vector<TrafficState>::iterator ac_ptr;
    for (ac_ptr=traffic.begin();ac_ptr != traffic.end(); ++ac_ptr) {
      ac_ptr->applyWindVector(wind);
    }
  }
  wind_vector = wind;
}

bool DaidalusCore::has_ownship() const {
  return ownship.isValid();
}

bool DaidalusCore::has_traffic() const {
  return traffic.size() > 0;
}

void DaidalusCore::add_blob(std::vector<std::vector<Position> >& blobs, std::vector<Position>& vin, std::vector<Position>& vout) {
  if (vin.empty() && vout.empty()) {
    return;
  }
  // Add conflict contour
  std::vector<Position> blob = vin;
  blob.insert(blob.end(), vout.begin(), vout.end());
  blobs.push_back(blob);
  vin.clear();
  vout.clear();
}

/* idx is a 0-based index in the list of traffic aircraft
 * returns 1 if detector of traffic aircraft
 * returns 2 if corrective alerter level is not set
 * returns 3 if alerter of traffic aircraft is out of bands
 * otherwise, if there are no errors, returns 0 and the answer is in blobs
 */
int DaidalusCore::horizontal_contours(std::vector<std::vector<Position> >& blobs, int idx, int alert_level) {
  const TrafficState& intruder = traffic[idx];
  int alerter_idx = alerter_index_of(idx);
  if (1 <= alerter_idx && alerter_idx <= parameters.numberOfAlerters()) {
    const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
    if (alert_level == 0) {
      alert_level = parameters.correctiveAlertLevel(alerter_idx);
    }
    if (alert_level > 0) {
      Detection3D* detector = alerter.getDetectorPtr(alert_level);
      if (detector != NULL) {
        blobs.clear();
        std::vector<Position> vin;
        const Position& po = ownship.getPosition();
        const Velocity& vo = ownship.getVelocity();
        double current_trk = vo.trk();
        std::vector<Position> vout;
        /* First step: Computes conflict contour (contour in the current path of the aircraft).
         * Get contour portion to the right.  If los.getTimeIn() == 0, a 360 degree
         * contour will be computed. Otherwise, stops at the first non-conflict degree.
         */
        double right = 0; // Contour conflict limit to the right relative to current direction  [0-2pi rad]
        double two_pi = 2*Pi;
        TrafficState own = ownship;
        for (; right < two_pi; right += parameters.getHorizontalDirectionStep()) {
          Velocity vop = vo.mkTrk(current_trk+right);
          own.setAirVelocity(vop);
          LossData los = detector->conflictDetectionWithTrafficState(own,intruder,0.0,parameters.getLookaheadTime());
          if ( !los.conflict() ) {
            break;
          }
          if (los.getTimeIn() != 0 ) {
            // if not in los, add position at time in (counter clock-wise)
            vin.push_back(po.linear(vop,los.getTimeIn()));
          }
          // in any case, add position ad time out (counter clock-wise)
          vout.insert(vout.begin(), po.linear(vop,los.getTimeOut()));
        }
        /* Second step: Compute conflict contour to the left */
        double left = 0;  // Contour conflict limit to the left relative to current direction [0-2pi rad]
        if (0 < right && right < two_pi) {
          /* There is a conflict contour, but not a violation */
          for (left = parameters.getHorizontalDirectionStep(); left < two_pi; left += parameters.getHorizontalDirectionStep()) {
            Velocity vop = vo.mkTrk(current_trk-left);
            own.setAirVelocity(vop);
            LossData los = detector->conflictDetectionWithTrafficState(own,intruder,0.0,parameters.getLookaheadTime());
            if ( !los.conflict() ) {
              break;
            }
            vin.insert(vin.begin(), po.linear(vop,los.getTimeIn()));
            vout.push_back(po.linear(vop,los.getTimeOut()));
          }
        }
        add_blob(blobs,vin,vout);
        // Third Step: Look for other blobs to the right within direction threshold
        if (right < parameters.getHorizontalContourThreshold()) {
          for (; right < two_pi-left; right += parameters.getHorizontalDirectionStep()) {
            Velocity vop = vo.mkTrk(current_trk+right);
            own.setAirVelocity(vop);
            LossData los = detector->conflictDetectionWithTrafficState(own,intruder,0.0,parameters.getLookaheadTime());
            if (los.conflict()) {
              vin.push_back(po.linear(vop,los.getTimeIn()));
              vout.insert(vout.begin(), po.linear(vop,los.getTimeOut()));
            } else {
              add_blob(blobs,vin,vout);
              if (right >= parameters.getHorizontalContourThreshold()) {
                break;
              }
            }
          }
          add_blob(blobs,vin,vout);
        }
        // Fourth Step: Look for other blobs to the left within direction threshold
        if (left < parameters.getHorizontalContourThreshold()) {
          for (; left < two_pi-right; left += parameters.getHorizontalDirectionStep()) {
            Velocity vop = vo.mkTrk(current_trk-left);
            own.setAirVelocity(vop);
            LossData los = detector->conflictDetectionWithTrafficState(own,intruder,0.0,parameters.getLookaheadTime());
            if (los.conflict()) {
              vin.insert(vin.begin(), po.linear(vop,los.getTimeIn()));
              vout.push_back(po.linear(vop,los.getTimeOut()));
            } else {
              add_blob(blobs,vin,vout);
              if (left >= parameters.getHorizontalContourThreshold()) {
                break;
              }
            }
          }
          add_blob(blobs,vin,vout);
        }
      } else {
        return 1;
      }
    } else {
      return 2;
    }
  } else {
    return 3;
  }
  return 0;
}

/**
 * Requires 0 <= conflict_region < CONFICT_BANDS
 * Put in acs_conflict_bands_ the list of aircraft predicted to be in conflict for the given region.
 * Put compute_bands_ a flag indicating if bands for given region are computed for some aircraft
 * Put in tiov_ the time interval of violation for given region
 */
void DaidalusCore::conflict_aircraft(int conflict_region) {
  double tin  = PINFINITY;
  double tout = NINFINITY;
  // Iterate on all traffic aircraft
  for (int ac = 0; ac < (int) traffic.size(); ++ac) {
    const TrafficState& intruder = traffic[ac];
    const Alerter& alerter = alerter_of(ac);
    // Assumes that thresholds of severe alerts are included in the volume of less severe alerts
    BandsRegion::Region region = BandsRegion::regionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
    int alert_level = alerter.alertLevelForRegion(region);
    if (alert_level > 0) {
      Detection3D* detector =  alerter.getLevel(alert_level).getCoreDetectionPtr();
      if (detector != NULL) {
        double alerting_time = Util::min(parameters.getLookaheadTime(),
            alerter.getLevel(alert_level).getAlertingTime());
        double early_alerting_time = Util::min(parameters.getLookaheadTime(),
            alerter.getLevel(alert_level).getEarlyAlertingTime());
        ConflictData det = detector->conflictDetectionWithTrafficState(ownship,intruder,0.0,parameters.getLookaheadTime());
        if (det.conflict()) {
          if (det.getTimeIn() == 0 || det.getTimeIn() < alerting_time) {
            acs_conflict_bands_[conflict_region].push_back(IndexLevelT(ac,alert_level,early_alerting_time));
          }
          tin = Util::min(tin,det.getTimeIn());
          tout = Util::max(tout,det.getTimeOut());
        }
      }
    }
  }
  tiov_[conflict_region]= Interval(tin,tout);
}


/**
 * Requires 0 <= conflict_region < CONFICT_BANDS
 * @return sorted list of aircraft indices and alert_levels contributing to conflict (non-peripheral)
 * bands for given conflict region.
 * INTERNAL USE ONLY
 */
const std::vector<IndexLevelT>& DaidalusCore::acs_conflict_bands(int conflict_region) {
  refresh();
  return acs_conflict_bands_[conflict_region];
}

/**
 * Requires 0 <= conflict_region < CONFICT_BANDS
 * @return Return time interval of conflict for given conflict region
 * INTERNAL USE ONLY
 */
const Interval& DaidalusCore::tiov(int conflict_region) {
  refresh();
  return tiov_[conflict_region];
}

/**
 * Return alert index used for the traffic aircraft at 0 <= idx < traffic.size().
 * The alert index depends on alerting logic. If ownship centric, it returns the
 * alert index of ownship. Otherwise, returns the alert index of the traffic aircraft
 * at idx. Return 0 if idx is out of range
 */
int DaidalusCore::alerter_index_of(int idx) const {
  if (0 <= idx && idx < (int) traffic.size()) {
    if (parameters.isAlertingLogicOwnshipCentric()) {
      return ownship.getAlerterIndex();
    } else {
      return traffic[idx].getAlerterIndex();
    }
  }
  return 0;
}

/**
 * Return alerter used for the traffic aircraft at 0 <= idx < traffic.size().
 * The alert index depends on alerting logic. If ownship centric, it returns the
 * alerter of the ownship. Otherwise, returns the alerter of the traffic aircraft
 * at idx.
 */
const Alerter& DaidalusCore::alerter_of(int idx) const {
  int alerter_idx = alerter_index_of(idx);
  return parameters.getAlerterAt(alerter_idx);
}

int DaidalusCore::epsilonH(const TrafficState& ownship, const TrafficState& ac) {
  if (ownship.isValid() && ac.isValid()) {
    Vect2 s = ownship.get_s().Sub(ac.get_s()).vect2();
    Vect2 v = ownship.get_v().Sub(ac.get_v()).vect2();
    return CriteriaCore::horizontalCoordination(s,v);
  } else {
    return 0;
  }
}

int DaidalusCore::epsilonV(const TrafficState& ownship, const TrafficState& ac) {
  if (ownship.isValid() && ac.isValid()) {
    Vect3 s = ownship.get_s().Sub(ac.get_s());
    return CriteriaCore::verticalCoordinationLoS(s,ownship.get_v(),ac.get_v(),
        ownship.getId(), ac.getId());
  } else {
    return 0;
  }
}

TrafficState DaidalusCore::criteria_ac() {
  return parameters.isEnabledConflictCriteria() ? mostUrgentAircraft() : TrafficState::INVALID();
}

TrafficState DaidalusCore::recovery_ac() {
  return parameters.isEnabledRecoveryCriteria() ? mostUrgentAircraft() : TrafficState::INVALID();
}

/**
 * Return true if and only if threshold values, defining an alerting level, are violated.
 */
bool DaidalusCore::check_alerting_thresholds(const AlertThresholds& athr, const TrafficState& intruder, int turning, int accelerating, int climbing) {
  if (athr.isValid()) {
    Detection3D* detector = athr.getCoreDetectionPtr();
    double alerting_time = Util::min(parameters.getLookaheadTime(),athr.getAlertingTime());
    int epsh = epsilonH(false,intruder);
    int epsv = epsilonV(false,intruder);
    ConflictData det = detector->conflictDetectionWithTrafficState(ownship,intruder,0.0,alerting_time);
    if (det.conflict()) {
      return true;
    }
    if (athr.getHorizontalDirectionSpread() > 0 || athr.getHorizontalSpeedSpread() > 0 ||
        athr.getVerticalSpeedSpread() > 0 || athr.getAltitudeSpread() > 0) {
      if (athr.getHorizontalDirectionSpread() > 0) {
        DaidalusDirBands dir_band(parameters);
        dir_band.set_min_rel(turning <= 0 ? athr.getHorizontalDirectionSpread() : 0);
        dir_band.setmax_rel(turning >= 0 ? athr.getHorizontalDirectionSpread() : 0);
        dir_band.set_step(parameters.getHorizontalDirectionStep());
        dir_band.set_turn_rate(parameters.getTurnRate());
        dir_band.set_bank_angle(parameters.getBankAngle());
        dir_band.set_recovery(parameters.isEnabledRecoveryHorizontalDirectionBands());
        if (dir_band.kinematic_conflict(ownship,intruder,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
      if (athr.getHorizontalSpeedSpread() > 0) {
        DaidalusHsBands hs_band = DaidalusHsBands(parameters);
        hs_band.set_min_rel(accelerating <= 0 ? athr.getHorizontalSpeedSpread() : 0);
        hs_band.setmax_rel(accelerating >= 0 ? athr.getHorizontalSpeedSpread() : 0);
        hs_band.set_step(parameters.getHorizontalSpeedStep());
        hs_band.set_horizontal_accel(parameters.getHorizontalAcceleration());
        hs_band.set_recovery(parameters.isEnabledRecoveryHorizontalSpeedBands());
        if (hs_band.kinematic_conflict(ownship,intruder,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
      if (athr.getVerticalSpeedSpread() > 0) {
        DaidalusVsBands vs_band = DaidalusVsBands(parameters);
        vs_band.set_min_rel(climbing <= 0 ? athr.getVerticalSpeedSpread() : 0);
        vs_band.setmax_rel(climbing >= 0 ? athr.getVerticalSpeedSpread() : 0);
        vs_band.set_step(parameters.getVerticalSpeedStep());
        vs_band.set_vertical_accel(parameters.getVerticalAcceleration());
        vs_band.set_recovery(parameters.isEnabledRecoveryVerticalSpeedBands());
        if (vs_band.kinematic_conflict(ownship,intruder,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
      if (athr.getAltitudeSpread() > 0) {
        DaidalusAltBands alt_band = DaidalusAltBands(parameters);
        alt_band.set_min_rel(climbing <= 0 ? athr.getAltitudeSpread() : 0);
        alt_band.setmax_rel(climbing >= 0 ? athr.getAltitudeSpread() : 0);
        alt_band.set_step(parameters.getAltitudeStep());
        alt_band.set_vertical_rate(parameters.getVerticalRate());
        alt_band.set_vertical_accel(parameters.getVerticalAcceleration());
        alt_band.set_recovery(parameters.isEnabledRecoveryAltitudeBands());
        if (alt_band.kinematic_conflict(ownship,intruder,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
    }
  }
  return false;
}

/**
 * Computes alerting type of ownship and an the idx-th aircraft in the traffic list
 * The number 0 means no alert. A negative number means
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
 * NOTES:
 * 1. This method uses a 0-based traffic index.
 * 2. This methods applies MofN alerting strategy
 */
int DaidalusCore::alert_level(int idx, int turning, int accelerating, int climbing) {
  if (0 <= idx && idx < static_cast<int>(traffic.size())) {
    const TrafficState& intruder = traffic[idx];
    int alerter_idx = alerter_index_of(idx);
    if (alerter_idx > 0) {
      const std::string& id = intruder.getId();
      std::map<std::string,AlertingHysteresis>::iterator alerting_hysteresis_ptr = alerting_hysteresis_acs_.find(id);
      if (alerting_hysteresis_ptr != alerting_hysteresis_acs_.end() &&
          !ISNAN(alerting_hysteresis_ptr->second.getLastTime()) &&
          alerting_hysteresis_ptr->second.getLastTime() == current_time) {
        return alerting_hysteresis_ptr->second.getLastAlert();
      }
      const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
      int current_alert_level = alert_level(alerter,intruder,turning,accelerating,climbing);
      if (alerting_hysteresis_ptr == alerting_hysteresis_acs_.end()) {
        AlertingHysteresis mofn = AlertingHysteresis(
            parameters.getHysteresisTime(),
            parameters.getPersistenceTime(),
            parameters.getAlertingParameterM(),
            parameters.getAlertingParameterN());
        int alert = mofn.alertingHysteresis(current_alert_level,current_time);
        alerting_hysteresis_acs_[id]=mofn;
        return alert;
      } else {
        return alerting_hysteresis_ptr->second.alertingHysteresis(current_alert_level,current_time);
      }
    }
  }
  return -1;
}

int DaidalusCore::alert_level(const Alerter& alerter, const TrafficState& intruder, int turning, int accelerating, int climbing) {
  for (int alert_level=alerter.mostSevereAlertLevel(); alert_level > 0; --alert_level) {
    const AlertThresholds& athr = alerter.getLevel(alert_level);
    if (check_alerting_thresholds(athr,intruder,turning,accelerating,climbing)) {
      return alert_level;
    }
  }
  return 0;
}

void DaidalusCore::setParameterData(const ParameterData& p) {
  if (parameters.setParameterData(p)) {
    stale(true);
  }
}

std::string DaidalusCore::outputStringAircraftStates(bool internal) const {
  std::string ualt = internal ? "m" : parameters.getUnitsOf("step_alt");
  std::string uhs = internal ? "m/s" : parameters.getUnitsOf("step_hs");
  std::string uvs = internal ? "m/s" : parameters.getUnitsOf("step_vs");
  std::string uxy = "m";
  std::string utrk = "deg";
  if (!internal) {
    if (Units::isCompatible(uhs,"knot")) {
      uxy = "nmi";
    } else if (Units::isCompatible(uhs,"fpm")) {
      uxy = "ft";
    } else if (Units::isCompatible(uhs,"kph")) {
      uxy = "km";
    }
  } else {
    utrk="rad";
  }
  return ownship.formattedTraffic(traffic,utrk,uxy,ualt,uhs,uvs,current_time);
}

std::string DaidalusCore::rawString() const {
  std::string s="## Daidalus Core\n";
  s+="current_time = "+FmPrecision(current_time)+"\n";
  s+="## Daidalus Parameters\n";
  s+=parameters.toString();
  s+="## Cached variables\n";
  s+="cache_ = "+Fmi(cache_)+"\n";
  s+="most_urgent_ac_ = "+most_urgent_ac_.getId()+"\n";
  s+="epsh_ = "+Fmi(epsh_)+"\n";
  s+="epsv_ = "+Fmi(epsv_)+"\n";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    s+="acs_conflict_bands_["+Fmi(conflict_region)+"] = "+
        IndexLevelT::toString(acs_conflict_bands_[conflict_region])+"\n";
  }
  bool comma=false;
  s+="tiov_ = {";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += tiov_[conflict_region].toString();
  }
  s += "}\n";
  comma=false;
  s+="bands4region_ = {";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += Fmb(bands4region_[conflict_region]);
  }
  s += "}\n";
  std::map<std::string,AlertingHysteresis>::const_iterator entry_ptr = alerting_hysteresis_acs_.begin();
  while (entry_ptr != alerting_hysteresis_acs_.end()) {
    s+="alerting_hysteresis_acs_["+entry_ptr->first+"] = "+
        entry_ptr->second.toString();
    ++entry_ptr;
  }
  if (!alerting_hysteresis_acs_.empty()) {
    s+="\n";
  }
  s+="wind_vector = "+wind_vector.toString()+"\n";
  s+="## Ownship and Traffic Relative to Wind\n";
  s+=outputStringAircraftStates(true);
  s+="##\n";
  return s;
}

std::string DaidalusCore::toString() const {
  std::string s="##\n";
  s+="current_time = "+FmPrecision(current_time)+"\n";
  s+="wind_vector = "+wind_vector.toString()+"\n";
  s+="##\n";
  return s;
}

}

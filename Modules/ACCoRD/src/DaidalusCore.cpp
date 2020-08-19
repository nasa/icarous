/*
 * Copyright (c) 2015-2020 United States Government as represented by
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
  stale();
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
    stale();
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
  clear_hysteresis();
}

/**
 *  Clear wind vector from this object.
 */
void DaidalusCore::clear_wind() {
  set_wind_velocity(Velocity::ZEROV());
}

bool DaidalusCore::set_alerter_ownship(int alerter_idx) {
  if (ownship.isValid()) {
    ownship.setAlerterIndex(alerter_idx);
    stale();
    return true;
  }
  return false;
}

// idx is zero-based
bool DaidalusCore::set_alerter_traffic(int idx, int alerter_idx) {
  if (0 <= idx && idx < static_cast<int>(traffic.size())) {
    traffic[idx].setAlerterIndex(alerter_idx);
    stale();
    return true;
  }
  return false;
}

/**
 *  Clear alerting hysteresis information from this object.
 */
void DaidalusCore::clear_hysteresis() {
  alerting_hysteresis_acs_.clear();
  dta_hysteresis_acs_.clear();
  stale();
}

/**
 * Set cached values to stale conditions as they are no longer fresh.
 * If hysteresis is true, it also clears hysteresis variables
 */
void DaidalusCore::stale() {
  if (cache_ >= 0) {
    cache_ = -1;
    most_urgent_ac_ = TrafficState::INVALID();
    epsh_ = 0;
    epsv_ = 0;
    dta_status_ = 0;
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      acs_conflict_bands_[conflict_region].clear();
      tiov_[conflict_region] = Interval::EMPTY;
      bands4region_[conflict_region] = false;
    }
    std::map<std::string,HysteresisData>::iterator hysteresis_ptr;
    for (hysteresis_ptr = alerting_hysteresis_acs_.begin();hysteresis_ptr != alerting_hysteresis_acs_.end();++hysteresis_ptr) {
      hysteresis_ptr->second.outdateIfCurrentTime(current_time);
    }
    for (hysteresis_ptr = dta_hysteresis_acs_.begin();hysteresis_ptr != dta_hysteresis_acs_.end();++hysteresis_ptr) {
      hysteresis_ptr->second.outdateIfCurrentTime(current_time);
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
    for (int ac=0; ac < static_cast<int>(traffic.size()); ++ac) {
      alert_level(ac,0,0,0);
    }
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      conflict_aircraft(conflict_region);
      BandsRegion::Region region = BandsRegion::regionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
      for (int alerter_idx=1;  alerter_idx <= parameters.numberOfAlerters(); ++alerter_idx) {
        const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
        int alert_level = alerter.alertLevelForRegion(region);
        if (alert_level > 0) {
          bands4region_[conflict_region] = true;
        }
      }
    }
    dta_status_ = 0; // Not active
    if (parameters.getDTALogic() != 0 && parameters.getDTAAlerter() != 0) {
      if (parameters.isAlertingLogicOwnshipCentric()) {
        if (alerter_index_of(ownship) == parameters.getDTAAlerter()) {
          dta_status_ = -1; // Inside DTA
        }
      } else {
        for (int ac=0; ac < static_cast<int>(traffic.size()) && dta_status_ == 0; ++ac) {
          if (alerter_index_of(traffic[ac]) == parameters.getDTAAlerter()) {
            dta_status_ = -1; // Inside DTA
          }
        }
      }
      if (dta_status_  < 0 && greater_than_corrective()) {
        dta_status_ = 1; //Inside DTA and special bands enabled
      }
    }
    refresh_mua_eps();
    cache_ = 1;
  }
}

bool DaidalusCore::greater_than_corrective() const {
  int corrective_idx = BandsRegion::orderOfConflictRegion(parameters.getCorrectiveRegion());
  if (corrective_idx > 0){
    for (int region_idx = 0; region_idx < BandsRegion::NUMBER_OF_CONFLICT_BANDS-corrective_idx; ++region_idx) {
      if (!acs_conflict_bands_[region_idx].empty()) {
        return true;
      }
    }
  }
  return false;
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
 * Returns DTA status:
 *  0 : DTA is not active
 * -1 : DTA is active, but special bands are not enabled yet
 *  1 : DTA is active and special bands are enabled
 */
int DaidalusCore::DTAStatus() {
  refresh();
  return dta_status_;
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
  traffic.clear();
  ownship = TrafficState::makeOwnship(id,pos,vel);
  ownship.applyWindVector(wind_vector);
  current_time = time;
  stale();
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
    stale();
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
  for (int i = 0; i < static_cast<int>(traffic.size()); ++i) {
    if (i == idx) {
      traffic[i] = old_own;
    } else {
      traffic[i].setAsIntruderOf(ownship);
    }
  }
  stale();
}

// idx is 0-based index in traffic list
bool DaidalusCore::remove_traffic(int idx) {
  if (0 < idx && idx < static_cast<int>(traffic.size())) {
    dta_hysteresis_acs_.erase(traffic[idx].getId());
    alerting_hysteresis_acs_.erase(traffic[idx].getId());
    traffic.erase(traffic.begin()+idx);
    stale();
    return true;
  }
  return false;
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
  stale();
}

bool DaidalusCore::linear_projection(double offset) {
  if (offset != 0 && has_ownship()) {
    ownship = ownship.linearProjection(offset);
    for (int i = 0; i < static_cast<int>(traffic.size()); ++i) {
      traffic[i] = traffic[i].linearProjection(offset);
    }
    current_time += offset;
    stale();
    return true;
  }
  return false;
}


bool DaidalusCore::has_ownship() const {
  return ownship.isValid();
}

bool DaidalusCore::has_traffic() const {
  return traffic.size() > 0;
}

/* idx is a 0-based index in the list of traffic aircraft
 * returns 1 if detector of traffic aircraft
 * returns 2 if corrective alerter level is not set
 * returns 3 if alerter of traffic aircraft is out of bands
 * otherwise, if there are no errors, returns 0 and the answer is in blobs
 */
int DaidalusCore::horizontal_contours(std::vector<std::vector<Position> >& blobs, int idx, int alert_level) {
  const TrafficState& intruder = traffic[idx];
  int alerter_idx = alerter_index_of(intruder);
  if (1 <= alerter_idx && alerter_idx <= parameters.numberOfAlerters()) {
    const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
    if (alert_level == 0) {
      alert_level = parameters.correctiveAlertLevel(alerter_idx);
    }
    if (alert_level > 0) {
      Detection3D* detector = alerter.getDetectorPtr(alert_level);
      if (detector != NULL) {
        detector->horizontalContours(blobs,ownship,intruder,
            parameters.getHorizontalContourThreshold(),
            parameters.getLookaheadTime());
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

/* idx is a 0-based index in the list of traffic aircraft
 * returns 1 if detector of traffic aircraft
 * returns 2 if corrective alerter level is not set
 * returns 3 if alerter of traffic aircraft is out of bands
 * otherwise, if there are no errors, returns 0 and the answer is in blobs
 */
int DaidalusCore::horizontal_hazard_zone(std::vector<Position>& haz, int idx, int alert_level,
    bool loss, bool from_ownship) {
  const TrafficState& intruder = traffic[idx];
  int alerter_idx = alerter_index_of(intruder);
  if (1 <= alerter_idx && alerter_idx <= parameters.numberOfAlerters()) {
    const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
    if (alert_level == 0) {
      alert_level = parameters.correctiveAlertLevel(alerter_idx);
    }
    if (alert_level > 0) {
      Detection3D* detector = alerter.getDetectorPtr(alert_level);
      if (detector != NULL) {
        detector->horizontalHazardZone(haz,
            (from_ownship ? ownship : intruder),
            (from_ownship ? intruder : ownship),
            (loss ? 0 : alerter.getLevel(alert_level).getAlertingTime()));
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
  for (int ac = 0; ac < static_cast<int>(traffic.size()); ++ac) {
    const TrafficState& intruder = traffic[ac];
    int alerter_idx = alerter_index_of(intruder);
    if (1 <= alerter_idx && alerter_idx <= parameters.numberOfAlerters()) {
      const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
      // Assumes that thresholds of severe alerts are included in the volume of less severe alerts
      BandsRegion::Region region = BandsRegion::regionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
      int alert_level = alerter.alertLevelForRegion(region);
      if (alert_level > 0) {
        Detection3D* detector =  alerter.getLevel(alert_level).getCoreDetectionPtr();
        if (detector != NULL) {
          std::map<std::string,HysteresisData>::iterator alerting_hysteresis_ptr = alerting_hysteresis_acs_.find(intruder.getId());
          double alerting_time = alerter.getLevel(alert_level).getAlertingTime();
          if (alerting_hysteresis_ptr != alerting_hysteresis_acs_.end() &&
              !ISNAN(alerting_hysteresis_ptr->second.getInitTime()) &&
              alerting_hysteresis_ptr->second.getInitTime() < current_time &&
              alerting_hysteresis_ptr->second.getLastValue() == alert_level) {
            alerting_time = alerter.getLevel(alert_level).getEarlyAlertingTime();
          }
          ConflictData det = detector->conflictDetectionWithTrafficState(ownship,intruder,0.0,parameters.getLookaheadTime());
          if (det.conflict()) {
            if (det.conflictBefore(alerting_time)) {
              acs_conflict_bands_[conflict_region].push_back(IndexLevelT(ac,alert_level,parameters.getLookaheadTime()));
            }
            tin = Util::min(tin,det.getTimeIn());
            tout = Util::max(tout,det.getTimeOut());
          }
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

int DaidalusCore::dta_hysteresis_current_value(const TrafficState& ac) {
  if (parameters.getDTALogic() != 0 && parameters.getDTAAlerter() != 0 &&
      parameters.getDTARadius() > 0 && parameters.getDTAHeight() > 0) {
    std::map<std::string,HysteresisData>::iterator dta_hysteresis_ptr = dta_hysteresis_acs_.find(ac.getId());
    if (dta_hysteresis_ptr ==  dta_hysteresis_acs_.end()) {
      HysteresisData dta_hysteresis = HysteresisData(
          parameters.getHysteresisTime(),
          parameters.getPersistenceTime(),
          parameters.getAlertingParameterM(),
          parameters.getAlertingParameterN());
      int raw_dta = Util::almost_leq(ac.getPosition().distanceH(parameters.getDTAPosition()),parameters.getDTARadius()) &&
          Util::almost_leq(ac.getPosition().alt(),parameters.getDTAHeight()) ? 1 : 0;
      int actual_dta = dta_hysteresis.applyHysteresisLogic(raw_dta,current_time);
      dta_hysteresis_acs_[ac.getId()] = dta_hysteresis;
      return actual_dta;
    } else if (dta_hysteresis_ptr->second.isUpdatedAtCurrentTime(current_time)) {
      return dta_hysteresis_ptr->second.getLastValue();
    } else {
      int raw_dta = Util::almost_leq(ac.getPosition().distanceH(parameters.getDTAPosition()),parameters.getDTARadius()) &&
          Util::almost_leq(ac.getPosition().alt(),parameters.getDTAHeight()) ? 1 : 0;
      return dta_hysteresis_ptr->second.applyHysteresisLogic(raw_dta,current_time);
    }
  } else {
    return 0;
  }
}

/**
 * Return alert index used for intruder aircraft.
 * The alert index depends on alerting logic and DTA logic.
 * If ownship centric, it returns the alert index of ownship.
 * Otherwise, returns the alert index of the intruder.
 * If the DTA logic is enabled, the alerter of an aircraft is determined by
 * its dta status.
 */
int DaidalusCore::alerter_index_of(const TrafficState& intruder) {
  if (parameters.isAlertingLogicOwnshipCentric()) {
    if (dta_hysteresis_current_value(ownship) == 1) {
      return parameters.getDTAAlerter();
    } else {
      return ownship.getAlerterIndex();
    }
  } else {
    if (dta_hysteresis_current_value(intruder) == 1) {
      return parameters.getDTAAlerter();
    } else {
      return intruder.getAlerterIndex();
    }
  }
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
bool DaidalusCore::check_alerting_thresholds(const Alerter& alerter, int alert_level, const TrafficState& intruder, int turning, int accelerating, int climbing) {
  const AlertThresholds& athr = alerter.getLevel(alert_level);
  if (athr.isValid()) {
    Detection3D* detector = athr.getCoreDetectionPtr();
    std::map<std::string,HysteresisData>::iterator alerting_hysteresis_ptr = alerting_hysteresis_acs_.find(intruder.getId());
    double alerting_time = alerter.getLevel(alert_level).getAlertingTime();
    if (alerting_hysteresis_ptr != alerting_hysteresis_acs_.end() &&
        !ISNAN(alerting_hysteresis_ptr->second.getLastTime()) &&
        alerting_hysteresis_ptr->second.getLastTime() < current_time &&
        alerting_hysteresis_ptr->second.getLastValue() == alert_level) {
      alerting_time = alerter.getLevel(alert_level).getEarlyAlertingTime();
    }
    int epsh = epsilonH(false,intruder);
    int epsv = epsilonV(false,intruder);
    ConflictData det = detector->conflictDetectionWithTrafficState(ownship,intruder,0.0,parameters.getLookaheadTime());
    if (det.conflictBefore(alerting_time)) {
      return true;
    }
    if (athr.getHorizontalDirectionSpread() > 0 || athr.getHorizontalSpeedSpread() > 0 ||
        athr.getVerticalSpeedSpread() > 0 || athr.getAltitudeSpread() > 0) {
      if (athr.getHorizontalDirectionSpread() > 0) {
        DaidalusDirBands dir_band;
        dir_band.set_min_max_rel(turning <= 0 ? athr.getHorizontalDirectionSpread() : 0,
            turning >= 0 ? athr.getHorizontalDirectionSpread() : 0);
        if (dir_band.kinematic_conflict(parameters,ownship,intruder,detector,epsh,epsv,alerting_time,DTAStatus())) {
          return true;
        }
      }
      if (athr.getHorizontalSpeedSpread() > 0) {
        DaidalusHsBands hs_band;
        hs_band.set_min_max_rel(accelerating <= 0 ? athr.getHorizontalSpeedSpread() : 0,
            accelerating >= 0 ? athr.getHorizontalSpeedSpread() : 0);
        if (hs_band.kinematic_conflict(parameters,ownship,intruder,detector,epsh,epsv,alerting_time,DTAStatus())) {
          return true;
        }
      }
      if (athr.getVerticalSpeedSpread() > 0) {
        DaidalusVsBands vs_band;
        vs_band.set_min_max_rel(climbing <= 0 ? athr.getVerticalSpeedSpread() : 0,
            climbing >= 0 ? athr.getVerticalSpeedSpread() : 0);
        if (vs_band.kinematic_conflict(parameters,ownship,intruder,detector,epsh,epsv,alerting_time,DTAStatus())) {
          return true;
        }
      }
      if (athr.getAltitudeSpread() > 0) {
        DaidalusAltBands alt_band;
        alt_band.set_min_max_rel(climbing <= 0 ? athr.getAltitudeSpread() : 0,
            climbing >= 0 ? athr.getAltitudeSpread() : 0);
        if (alt_band.kinematic_conflict(parameters,ownship,intruder,detector,epsh,epsv,alerting_time,DTAStatus())) {
          return true;
        }
      }
    }
  }
  return false;
}

int DaidalusCore::alerting_hysteresis_current_value(const TrafficState& intruder, int turning, int accelerating, int climbing) {
  int alerter_idx = alerter_index_of(intruder);
  if (1 <= alerter_idx && alerter_idx <= parameters.numberOfAlerters()) {
    std::map<std::string,HysteresisData>::iterator alerting_hysteresis_ptr = alerting_hysteresis_acs_.find(intruder.getId());
    if (alerting_hysteresis_ptr == alerting_hysteresis_acs_.end()) {
      HysteresisData alerting_hysteresis = HysteresisData(
          parameters.getHysteresisTime(),
          parameters.getPersistenceTime(),
          parameters.getAlertingParameterM(),
          parameters.getAlertingParameterN());
      const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
      int raw_alert = raw_alert_level(alerter,intruder,turning,accelerating,climbing);
      int actual_alert = alerting_hysteresis.applyHysteresisLogic(raw_alert,current_time);
      alerting_hysteresis_acs_[intruder.getId()] = alerting_hysteresis;
      return actual_alert;
    } else if (alerting_hysteresis_ptr->second.isUpdatedAtCurrentTime(current_time)) {
      return alerting_hysteresis_ptr->second.getLastValue();
    } else {
      const Alerter& alerter = parameters.getAlerterAt(alerter_idx);
      int raw_alert = raw_alert_level(alerter,intruder,turning,accelerating,climbing);
      return alerting_hysteresis_ptr->second.applyHysteresisLogic(raw_alert,current_time);
    }
  } else {
    return -1;
  }
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
    return alerting_hysteresis_current_value(traffic[idx],turning,accelerating,climbing);
  } else {
    return -1;
  }
}

int DaidalusCore::raw_alert_level(const Alerter& alerter, const TrafficState& intruder, int turning, int accelerating, int climbing) {
  for (int alert_level=alerter.mostSevereAlertLevel(); alert_level > 0; --alert_level) {
    if (check_alerting_thresholds(alerter,alert_level,intruder,turning,accelerating,climbing)) {
      return alert_level;
    }
  }
  return 0;
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
  s+="dta_status_ = "+Fmi(dta_status_)+"\n";
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
  std::map<std::string,HysteresisData>::const_iterator entry_ptr = alerting_hysteresis_acs_.begin();
  while (entry_ptr != alerting_hysteresis_acs_.end()) {
    s+="alerting_hysteresis_acs_["+entry_ptr->first+"] = "+
        entry_ptr->second.toString();
    ++entry_ptr;
  }
  if (!alerting_hysteresis_acs_.empty()) {
    s+="\n";
  }
  entry_ptr = dta_hysteresis_acs_.begin();
  while (entry_ptr != dta_hysteresis_acs_.end()) {
    s+="dta_hysteresis_acs_["+entry_ptr->first+"] = "+
        entry_ptr->second.toString();
    ++entry_ptr;
  }
  if (!dta_hysteresis_acs_.empty()) {
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

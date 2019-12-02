/*
 * Copyright (c) 2015-2018 United States Government as represented by
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
  // Public variables_ are initialized
  // ownship = TrafficState::INVALID();      // Default initialization
  current_time = 0;
  // wind_vector = Velocity.ZERO;  // Default initialization
  urgency_strategy = new NoneUrgencyStrategy();

  // Cached arrays__ are initialized
  acs_conflict_bands__ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);

  // Cached__ variables are cleared
  outdated__ = false; // Force stale
  stale(true);
}

DaidalusCore::DaidalusCore() {
  init();
}

DaidalusCore::DaidalusCore(const Alerter& alerter) {
  init();
  parameters.addAlerter(alerter);
}

DaidalusCore::DaidalusCore(const Detection3D& det, double T) {
  init();
  parameters.addAlerter(Alerter::SingleBands(det,T,T));
  parameters.setLookaheadTime(T);
}

DaidalusCore::DaidalusCore(const DaidalusCore& core) {
  // Cached arrays__ are initialized
  acs_conflict_bands__ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);
  copyFrom(core);
}

void DaidalusCore::copyFrom(const DaidalusCore& core) {
  if (&core != this) {
    ownship = core.ownship;
    traffic = core.traffic;
    current_time = core.current_time;
    wind_vector = core.wind_vector;
    parameters = core.parameters;
    urgency_strategy = core.urgency_strategy->copy();
    // Cached__ variables are cleared
    outdated__ = false; // Force stale
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
 * Set cached values to stale conditions as they are no longer fresh
 */
void DaidalusCore::stale(bool hysteresis) {
  if (!outdated__) {
    outdated__ = true;
    most_urgent_ac__ = TrafficState::INVALID();
    epsh__ = 0;
    epsv__ = 0;
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      acs_conflict_bands__[conflict_region].clear();
      tiov__[conflict_region] = Interval::EMPTY;
      bands4region__[conflict_region] = false;
    }
  }
  if (hysteresis) {
    _alerting_mofns_.clear();
  }
}

/**
 * Returns true is object is fresh
 */
bool DaidalusCore::isFresh() const {
  return !outdated__;
}

/**
 *  Refresh cached values
 */
void DaidalusCore::refresh() {
  if (outdated__) {
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      conflict_aircraft(conflict_region);
      BandsRegion::Region region = BandsRegion::conflictRegionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
      for (int alerter_idx=1;  alerter_idx <= parameters.numberOfAlerters(); ++alerter_idx) {
        Alerter alerter = parameters.getAlerterAt(parameters.isAlertingLogicOwnshipCentric() ?
            ownship.getAlerterIndex() :alerter_idx);
        int alert_level = alerter.alertLevelForRegion(region);
        if (alert_level > 0) {
          bands4region__[conflict_region] = true;
        }
      }
    }
    int muac = -1;
    if (!traffic.empty()) {
      muac = urgency_strategy->mostUrgentAircraft(ownship, traffic, parameters.getLookaheadTime());
    }
    if (muac >= 0) {
      most_urgent_ac__ = traffic[muac];
    } else {
      most_urgent_ac__ = TrafficState::INVALID();
    }
    epsh__ = epsilonH(ownship,most_urgent_ac__);
    epsv__ = epsilonV(ownship,most_urgent_ac__);
    outdated__ = false;
  }
}

/**
 * @return most urgent aircraft for implicit coordination
 */
TrafficState DaidalusCore::mostUrgentAircraft() {
  refresh();
  return most_urgent_ac__;
}

/**
 * Returns horizontal epsilon for implicit coordination with respect to criteria ac.
 *
 */
int DaidalusCore::epsilonH() {
  refresh();
  return epsh__;
}

/**
 * Returns vertical epsilon for implicit coordination with respect to criteria ac.
 */
int DaidalusCore::epsilonV() {
  refresh();
  return epsv__;
}

/**
 * Returns horizontal epsilon for implicit coordination with respect to criteria ac.
 *
 */
int DaidalusCore::epsilonH(bool recovery_case, const TrafficState& traffic) {
  refresh();
  if ((recovery_case? parameters.isEnabledRecoveryCriteria() : parameters.isEnabledConflictCriteria()) &&
      traffic.sameId(most_urgent_ac__)) {
    return epsh__;
  }
  return 0;
}

/**
 * Returns vertical epsilon for implicit coordination with respect to criteria ac.
 */
int DaidalusCore::epsilonV(bool recovery_case, const TrafficState& traffic) {
  refresh();
  if ((recovery_case? parameters.isEnabledRecoveryCriteria() : parameters.isEnabledConflictCriteria()) &&
      traffic.sameId(most_urgent_ac__)) {
    return epsv__;
  }
  return 0;
}

/**
 * Return true if bands are computed for this particular region (0:NEAR, 1:MID, 2: FAR)
 */
bool DaidalusCore::bands_for(int region) {
  refresh();
  return bands4region__[region];
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

int DaidalusCore::add_traffic_state(const std::string& id, const Position& pos, const Velocity& vel, double time) {
  double dt = current_time-time;
  Position pt = dt == 0 ? pos : pos.linear(vel,dt);
  TrafficState ac = ownship.makeIntruder(id,pt,vel);
  if (ac.isValid()) {
    ac.applyWindVector(wind_vector);
    traffic.push_back(ac);
    return traffic.size();
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
  TrafficState intruder = traffic[idx];
  int alerter_idx = alerter_index_of(idx);
  if (1 <= alerter_idx && alerter_idx <= parameters.numberOfAlerters()) {
    Alerter alerter = parameters.getAlerterAt(alerter_idx);
    if (alert_level == 0) {
      alert_level = parameters.correctiveAlertLevel(alerter_idx);
    }
    if (alert_level > 0) {
      Detection3D* detector = alerter.getDetectorPtr(alert_level);
      if (detector != NULL) {
        blobs.clear();
        std::vector<Position> vin;
        Position po = ownship.getPosition();
        Velocity vo = ownship.getVelocity();
        Vect3 si = intruder.get_s();
        Velocity vi = intruder.get_v();
        double current_trk = vo.trk();
        std::vector<Position> vout;
        /* First step: Computes conflict contour (contour in the current path of the aircraft).
         * Get contour portion to the right.  If los.getTimeIn() == 0, a 360 degree
         * contour will be computed. Otherwise, stops at the first non-conflict degree.
         */
        double right = 0; // Contour conflict limit to the right relative to current direction  [0-2pi rad]
        double two_pi = 2*Pi;
        double s_err = intruder.relativeHorizontalPositionError(ownship,parameters);
        double sz_err = intruder.relativeVerticalPositionError(ownship,parameters);
        double v_err = intruder.relativeHorizontalSpeedError(ownship,s_err,parameters);
        double vz_err = intruder.relativeVerticalSpeedError(ownship,parameters);
        for (; right < two_pi; right += parameters.getHorizontalDirectionStep()) {
          Velocity vop = vo.mkTrk(current_trk+right);
          LossData los = detector->conflictDetectionSUM(ownship.get_s(),ownship.vel_to_v(po,vop),si,vi,
              0,parameters.getLookaheadTime(),
              s_err,sz_err,v_err,vz_err);
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
            LossData los = detector->conflictDetectionSUM(ownship.get_s(),ownship.vel_to_v(po,vop),si,vi,
                0,parameters.getLookaheadTime(),
                s_err,sz_err,v_err,vz_err);
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
            LossData los = detector->conflictDetectionSUM(ownship.get_s(),ownship.vel_to_v(po,vop),si,vi,
                0,parameters.getLookaheadTime(),
                s_err,sz_err,v_err,vz_err);
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
            LossData los = detector->conflictDetectionSUM(ownship.get_s(),ownship.vel_to_v(po,vop),si,vi,
                0,parameters.getLookaheadTime(),
                s_err,sz_err,v_err,vz_err);
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
 * Put in acs_conflict_bands__ the list of aircraft predicted to be in conflict for the given region.
 * Put compute_bands__ a flag indicating if bands for given region are computed for some aircraft
 * Put in tiov__ the time interval of violation for given region
 */
void DaidalusCore::conflict_aircraft(int conflict_region) {
  double tin  = PINFINITY;
  double tout = NINFINITY;
  // Iterate on all traffic aircraft
  for (int ac = 0; ac < (int) traffic.size(); ++ac) {
    TrafficState intruder = traffic[ac];
    Alerter alerter = alerter_of(ac);
    // Assumes that thresholds of severe alerts are included in the volume of less severe alerts
    BandsRegion::Region region = BandsRegion::conflictRegionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
    int alert_level = alerter.alertLevelForRegion(region);
    if (alert_level > 0) {
      Detection3D* detector =  alerter.getLevel(alert_level).getCoreDetectionPtr();
      double alerting_time = Util::min(parameters.getLookaheadTime(),
          alerter.getLevel(alert_level).getAlertingTime());
      double early_alerting_time = Util::min(parameters.getLookaheadTime(),
          alerter.getLevel(alert_level).getEarlyAlertingTime());
      double s_err = intruder.relativeHorizontalPositionError(ownship,parameters);
      double sz_err = intruder.relativeVerticalPositionError(ownship,parameters);
      double v_err = intruder.relativeHorizontalSpeedError(ownship,s_err,parameters);
      double vz_err = intruder.relativeVerticalSpeedError(ownship,parameters);
      ConflictData det = detector->conflictDetectionSUM(ownship.get_s(),ownship.get_v(),
          intruder.get_s(),intruder.get_v(),0,parameters.getLookaheadTime(),
          s_err,sz_err,v_err,vz_err);
      bool lowc = detector->violationSUMAt(ownship.get_s(),ownship.get_v(),intruder.get_s(),intruder.get_v(),
          s_err,sz_err,v_err,vz_err,0.0);
      if (lowc || det.conflict()) {
        if (lowc || det.getTimeIn() < alerting_time) {
          acs_conflict_bands__[conflict_region].push_back(IndexLevelT(ac,alert_level,early_alerting_time,true));
        }
        tin = Util::min(tin,det.getTimeIn());
        tout = Util::max(tout,det.getTimeOut());
      }
    }
  }
  tiov__[conflict_region]= Interval(tin,tout);
}


/**
 * Requires 0 <= conflict_region < CONFICT_BANDS
 * @return sorted list of aircraft indices and alert_levels contributing to conflict (non-peripheral)
 * bands for given conflict region.
 * INTERNAL USE ONLY
 */
const std::vector<IndexLevelT>& DaidalusCore::acs_conflict_bands(int conflict_region) {
  refresh();
  return acs_conflict_bands__[conflict_region];
}

/**
 * Requires 0 <= conflict_region < CONFICT_BANDS
 * @return Return time interval of conflict for given conflict region
 * INTERNAL USE ONLY
 */
const Interval& DaidalusCore::tiov(int conflict_region) {
  refresh();
  return tiov__[conflict_region];
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
    Vect3 so = ownship.get_s();
    Velocity vo = ownship.get_v();
    Vect3 si = intruder.get_s();
    Velocity vi = intruder.get_v();
    Detection3D* detector = athr.getCoreDetectionPtr();
    double alerting_time = Util::min(parameters.getLookaheadTime(),athr.getAlertingTime());
    int epsh = epsilonH(false,intruder);
    int epsv = epsilonV(false,intruder);
    double s_err = intruder.relativeHorizontalPositionError(ownship,parameters);
    double sz_err = intruder.relativeVerticalPositionError(ownship,parameters);
    double v_err = intruder.relativeHorizontalSpeedError(ownship,s_err,parameters);
    double vz_err = intruder.relativeVerticalSpeedError(ownship,parameters);
    if (detector->violationSUMAt(so,vo,si,vi,
        s_err,sz_err,v_err,vz_err,0.0)) {
      return true;
    }
    ConflictData det = detector->conflictDetectionSUM(so,vo,si,vi,0,alerting_time,
        s_err,sz_err,v_err,vz_err);
    if (det.conflict()) {
      return true;
    }
    if (athr.getHorizontalDirectionSpread() > 0 || athr.getHorizontalSpeedSpread() > 0 ||
        athr.getVerticalSpeedSpread() > 0 || athr.getAltitudeSpread() > 0) {
      if (athr.getHorizontalDirectionSpread() > 0) {
        DaidalusDirBands dir_band(parameters);
        dir_band.set_min_rel(turning <= 0 ? athr.getHorizontalDirectionSpread() : 0);
        dir_band.set_max_rel(turning >= 0 ? athr.getHorizontalDirectionSpread() : 0);
        dir_band.set_step(parameters.getHorizontalDirectionStep());
        dir_band.set_turn_rate(parameters.getTurnRate());
        dir_band.set_bank_angle(parameters.getBankAngle());
        dir_band.set_recovery(parameters.isEnabledRecoveryHorizontalDirectionBands());
        if (dir_band.kinematic_conflict(ownship,intruder,parameters,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
      if (athr.getHorizontalSpeedSpread() > 0) {
        DaidalusHsBands hs_band = DaidalusHsBands(parameters);
        hs_band.set_min_rel(accelerating <= 0 ? athr.getHorizontalSpeedSpread() : 0);
        hs_band.set_max_rel(accelerating >= 0 ? athr.getHorizontalSpeedSpread() : 0);
        hs_band.set_step(parameters.getHorizontalSpeedStep());
        hs_band.set_horizontal_accel(parameters.getHorizontalAcceleration());
        hs_band.set_recovery(parameters.isEnabledRecoveryHorizontalSpeedBands());
        if (hs_band.kinematic_conflict(ownship,intruder,parameters,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
      if (athr.getVerticalSpeedSpread() > 0) {
        DaidalusVsBands vs_band = DaidalusVsBands(parameters);
        vs_band.set_min_rel(climbing <= 0 ? athr.getVerticalSpeedSpread() : 0);
        vs_band.set_max_rel(climbing >= 0 ? athr.getVerticalSpeedSpread() : 0);
        vs_band.set_step(parameters.getVerticalSpeedStep());
        vs_band.set_vertical_accel(parameters.getVerticalAcceleration());
        vs_band.set_recovery(parameters.isEnabledRecoveryVerticalSpeedBands());
        if (vs_band.kinematic_conflict(ownship,intruder,parameters,detector,epsh,epsv,alerting_time)) {
          return true;
        }
      }
      if (athr.getAltitudeSpread() > 0) {
        DaidalusAltBands alt_band = DaidalusAltBands(parameters);
        alt_band.set_min_rel(climbing <= 0 ? athr.getAltitudeSpread() : 0);
        alt_band.set_max_rel(climbing >= 0 ? athr.getAltitudeSpread() : 0);
        alt_band.set_step(parameters.getAltitudeStep());
        alt_band.set_vertical_rate(parameters.getVerticalRate());
        alt_band.set_vertical_accel(parameters.getVerticalAcceleration());
        alt_band.set_recovery(parameters.isEnabledRecoveryAltitudeBands());
        if (alt_band.kinematic_conflict(ownship,intruder,parameters,detector,epsh,epsv,alerting_time)) {
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
  int alert = -1;
  if (0 <= idx && idx < (int)traffic.size()) {
    int alerter_idx = alerter_index_of(idx);
    if (alerter_idx > 0) {
      alert = 0;
      Alerter alerter = parameters.getAlerterAt(alerter_idx);
      for (int alert_level=alerter.mostSevereAlertLevel(); alert_level > 0; --alert_level) {
        if (check_alerting_thresholds(alerter.getLevel(alert_level),traffic[idx],turning,accelerating,climbing)) {
          alert = alert_level;
          break;
        }
      }
      std::string id = traffic[idx].getId();
      std::map<std::string,AlertingMofN>::iterator find_ptr = _alerting_mofns_.find(id);
      if (find_ptr == _alerting_mofns_.end()) {
        AlertingMofN mofn = AlertingMofN(parameters.getAlertingParameterM(),
            parameters.getAlertingParameterN(),
            parameters.getHysteresisTime(),
            parameters.getPersistenceTime());
        alert = mofn.m_of_n(alert,current_time);
        _alerting_mofns_[id]=mofn;
      } else {
        alert = find_ptr->second.m_of_n(alert,current_time);
      }
    }
  }
  return alert;
}

void DaidalusCore::setParameterData(const ParameterData& p) {
  if (parameters.setParameterData(p)) {
    stale(true);
  }
}

std::string DaidalusCore::rawString() const {
  std::string s="";
  s+="## DaidalusCore Parameters\n";
  s+=parameters.toString();
  s+="## Cached variables__\n";
  s+="outdated__ = "+Fmb(outdated__)+"\n";
  s+="most_urgent_ac__ = "+most_urgent_ac__.getId()+"\n";
  s+="epsh__ = "+Fmi(epsh__)+"\n";
  s+="epsv__ = "+Fmi(epsv__)+"\n";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    s+="acs_conflict_bands__["+Fmi(conflict_region)+"] = "+
        IndexLevelT::toString(acs_conflict_bands__[conflict_region])+"\n";
  }
  bool comma=false;
  s+="tiov__ = {";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += tiov__[conflict_region].toString();
  }
  s += "}\n";
  comma=false;
  s+="bands4region__ = {";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += Fmb(bands4region__[conflict_region]);
  }
  s += "}\n";
  s+="wind_vector = "+wind_vector.toString()+"\n";
  s+="## Ownship and Traffic Relative to Wind\n";
  ownship.formattedTraffic(traffic, "m","m","m/s","m/s",current_time);
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

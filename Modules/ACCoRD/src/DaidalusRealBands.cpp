/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusRealBands.h"
#include "Velocity.h"
#include "Position.h"
#include "Detection3D.h"
#include "Integerval.h"
#include "IntervalSet.h"
#include "BandsRange.h"
#include "BandsRegion.h"
#include "DaidalusIntegerBands.h"
#include "IndexLevelT.h"
#include "Util.h"
#include "DaidalusCore.h"
#include "DaidalusParameters.h"
#include "RecoveryInformation.h"

#include <cmath>
#include <vector>
#include <string>

#include "ColorValue.h"
#include "TrafficState.h"

namespace larcfm {

DaidalusRealBands::DaidalusRealBands(double mod) {
  // Private variables are initialized
  mod_ = std::abs(mod);
  min_rel_ = 0;
  max_rel_ = 0;

  bands_hysteresis_.setMod(mod_);

  // Cached arrays_ are initialized
  acs_peripheral_bands_ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);
  acs_bands_ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);

  // Cached_ variables are cleared
  outdated_ = false; // Force stale
  stale();
}

DaidalusRealBands::DaidalusRealBands(const DaidalusRealBands& b) {
  // Private variables are copied
  mod_ = b.mod_;
  min_rel_ = b.min_rel_;
  max_rel_ = b.max_rel_;

  bands_hysteresis_.setMod(mod_);

  // Cached arrays_ are initialized
  acs_peripheral_bands_ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);
  acs_bands_ = std::vector<std::vector<IndexLevelT> >(BandsRegion::NUMBER_OF_CONFLICT_BANDS);

  // Cached_ variables are cleared
  outdated_ = false; // Force stale
  stale();
}

double DaidalusRealBands::min_rel(const DaidalusParameters& parameters) const {
  if (min_rel_ == 0 && max_rel_ == 0) {
    return get_min_rel(parameters);
  } else {
    return min_rel_;
  }
}

double DaidalusRealBands::max_rel(const DaidalusParameters& parameters) const {
  if (min_rel_ == 0 && max_rel_ == 0) {
    return get_max_rel(parameters);
  } else {
    return max_rel_;
  }
}

bool DaidalusRealBands::saturate_corrective_bands(const DaidalusParameters& parameters, int dta_status) const {
  return false;
}

double DaidalusRealBands::get_min_val_() const {
  return min_val_;
}

double DaidalusRealBands::get_max_val_() const {
  return max_val_;
}

double DaidalusRealBands::get_mod() const {
  return mod_;
}

/**
 * Overwrite relative values from those in the parameters. When mod_ > 0, requires min_rel and max_rel
 * to be in [0,mod/2]. When mod_ == 0, a negative min_rel value represents val-min and a negative value
 * max_rel value represents max-val.
 */
void DaidalusRealBands::set_min_max_rel(double min_rel, double max_rel) {
  min_rel_ = min_rel;
  max_rel_ = max_rel;
  // This method doesn't stale data. Use with care.
}

bool DaidalusRealBands::set_input(const DaidalusParameters& parameters, const TrafficState& ownship, int dta_status) {
  if (checked_ < 0) {
    checked_ = 0;
    set_special_configuration(parameters,dta_status);
    if (ownship.isValid() && get_step(parameters) > 0) {
      double val = own_val(ownship);
      // When mod_ == 0, min_val <= max_val. When mod_ > 0, min_val, max_val in [0,mod_].
      // In the later case, min_val may be greater than max_val. Furthermore, min_val = max_val means
      // a range of values from 0 to mod, i.e., a circular band.
      if (min_rel(parameters) == 0 && max_rel(parameters) == 0) {
        min_val_ = get_min(parameters);
        max_val_ = get_max(parameters);
      } else {
        if (min_rel(parameters) >= 0) {
          min_val_ = Util::safe_modulo(val-min_rel(parameters),mod_);
        } else {
          min_val_ = get_min(parameters);
        }
        if (max_rel(parameters) >= 0) {
          max_val_ = Util::safe_modulo(val+max_rel(parameters),mod_);
        } else {
          max_val_ = get_max(parameters);
        }
        if (mod_ == 0) {
          min_val_ = Util::max(min_val_,get_min(parameters));
          max_val_ = Util::min(max_val_,get_max(parameters));
        }
      }
      circular_ = mod_ > 0 && Util::almost_equals(min_val_,max_val_,DaidalusParameters::ALMOST_);
      if (circular_) {
        min_relative_ = mod_/2.0;
        max_relative_ = mod_/2.0;
      } else {
        if (min_rel(parameters) > 0) {
          min_relative_ = min_rel(parameters);
        } else {
          min_relative_ = Util::safe_modulo(val-min_val_,mod_);
        }
        if (max_rel(parameters) > 0) {
          max_relative_ = max_rel(parameters);
        } else {
          max_relative_ = Util::safe_modulo(max_val_-val,mod_);
        }
      }
      if ((min_val_ <= val && val <= max_val_ && min_val_ != max_val_) ||
          (mod_ > 0 && (circular_ ||
              (0 <= val && val <= max_val_) || (min_val_ <= val && val <= mod_)))) {
        checked_     = 1;
      }
    }
  }
  return checked_ > 0;
}

bool DaidalusRealBands::kinematic_conflict(const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
    Detection3D* detector, int epsh, int epsv, double alerting_time, int dta_status) {
  return set_input(parameters,ownship,dta_status) &&
      any_red(detector,NULL,epsh,epsv,0.0,alerting_time,parameters,ownship,traffic);
}

int DaidalusRealBands::length(DaidalusCore& core) {
  refresh(core);
  return static_cast<int>(ranges_.size());
}

const Interval& DaidalusRealBands::interval(DaidalusCore& core, int i) {
  if (i < 0 || i >= length(core)) {
    return Interval::EMPTY;
  }
  return ranges_[i].interval;
}

BandsRegion::Region DaidalusRealBands::region(DaidalusCore& core, int i) {
  if (i < 0 || i >= length(core)) {
    return BandsRegion::UNKNOWN;
  } else {
    return ranges_[i].region;
  }
}

/**
 * Return index in ranges_ where val is found, -1 if invalid input, >= length if not found
 */
int DaidalusRealBands::indexOf(DaidalusCore& core, double val) {
  if (set_input(core.parameters,core.ownship,core.DTAStatus())) {
    refresh(core);
    return BandsRange::index_of(ranges_,val,mod_);
  } else {
    return -1;
  }
}

/**
 * Set cached values to stale conditions as they are no longer fresh
 */
void DaidalusRealBands::stale() {
  if (!outdated_) {
    outdated_ = true;
    checked_ = -1;
    for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
      acs_peripheral_bands_[conflict_region].clear();
      acs_bands_[conflict_region].clear();
    }
    ranges_.clear();
    recovery_time_ = NaN;
    recovery_nfactor_ = -1;
    recovery_horizontal_distance_ = NaN;
    recovery_vertical_distance_ = NaN;
    min_rel_ = 0;
    max_rel_ = 0;
    min_val_ = 0;
    max_val_ = 0;
    min_relative_ = 0;
    max_relative_ = 0;
    circular_ = false;
  }
}

/**
 * clear hysteresis
 */
void DaidalusRealBands::clear_hysteresis() {
  bands_hysteresis_.reset();
  stale();
}

/**
 * Returns true is object is fresh
 */
bool DaidalusRealBands::isFresh() const {
  return !outdated_;
}

/**
 * Refresh cached values
 */
void DaidalusRealBands::refresh(DaidalusCore& core) {
  if (outdated_) {
    if (set_input(core.parameters,core.ownship,core.DTAStatus())) {
      for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
        acs_bands_[conflict_region] = core.acs_conflict_bands(conflict_region);
        if (core.bands_for(conflict_region)) {
          peripheral_aircraft(core,conflict_region);
          acs_bands_[conflict_region].insert(acs_bands_[conflict_region].end(), acs_peripheral_bands_[conflict_region].begin(), acs_peripheral_bands_[conflict_region].end());
        }
      }
      compute(core);
    }
    outdated_ = false;
  }
}

/**
 *  Force computation of kinematic bands
 */
void DaidalusRealBands::force_compute(DaidalusCore& core) {
  stale();
  refresh(core);
}

/**
 * Requires 0 <= conflict_region < CONFICT_BANDS and acs_peripheral_bands_ is empty
 * Put in acs_peripheral_bands_ the list of aircraft predicted to have a peripheral band for the given region.
 */
void DaidalusRealBands::peripheral_aircraft(DaidalusCore& core, int conflict_region) {
  // Iterate on all traffic aircraft
  for (int ac = 0; ac < static_cast<int>(core.traffic.size()); ++ac) {
    const TrafficState& intruder = core.traffic[ac];
    int alerter_idx = core.alerter_index_of(intruder);
    if (1 <= alerter_idx && alerter_idx <= core.parameters.numberOfAlerters()) {
      const Alerter& alerter = core.parameters.getAlerterAt(alerter_idx);
      // Assumes that thresholds of severe alerts are included in the volume of less severe alerts
      BandsRegion::Region region = BandsRegion::regionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region);
      int alert_level = alerter.alertLevelForRegion(region);
      if (alert_level > 0) {
        Detection3D* detector = alerter.getLevel(alert_level).getCoreDetectionPtr();
        double alerting_time = Util::min(core.parameters.getLookaheadTime(),
            alerter.getLevel(alert_level).getAlertingTime());
        ConflictData det = detector->conflictDetectionWithTrafficState(core.ownship,intruder,0.0,alerting_time);
        if (!det.conflict() && kinematic_conflict(core.parameters,core.ownship,intruder,detector,
            core.epsilonH(false,intruder),core.epsilonV(false,intruder),alerting_time,
            core.DTAStatus())) {
          acs_peripheral_bands_[conflict_region].push_back(IndexLevelT(ac,alert_level,alerting_time));
        }
      }
    }
  }
}

/**
 * Requires 0 <= conflict_region < CONFICT_BANDS
 * @return sorted list of aircraft indices and alert_levels contributing to peripheral bands
 * for given conflict region.
 * INTERNAL USE ONLY
 */
const std::vector<IndexLevelT>& DaidalusRealBands::acs_peripheral_bands(DaidalusCore& core, int conflict_region) {
  refresh(core);
  return acs_peripheral_bands_[conflict_region];
}

/**
 * Return recovery information.
 */
RecoveryInformation DaidalusRealBands::recoveryInformation(DaidalusCore& core) {
  refresh(core);
  return RecoveryInformation(recovery_time_,recovery_nfactor_, recovery_horizontal_distance_,recovery_vertical_distance_);
}

/**
 * Return list of bands ranges
 */
const std::vector<BandsRange>& DaidalusRealBands::ranges(DaidalusCore& core) {
  refresh(core);
  return ranges_;
}

/**
 * Compute list of color values in lcvs from sets of none bands
 * Ensure that the intervals are "complete", filling in missing intervals and ensuring the
 * bands end at the proper bounds.
 */
void DaidalusRealBands::color_values(std::vector<ColorValue>& lcvs, const std::vector<IntervalSet>& none_sets, DaidalusCore& core, bool recovery,
    int last_region) {
  if (mod_ == 0) {
    ColorValue::init(lcvs,get_min(core.parameters),get_max(core.parameters),min_val_,max_val_,BandsRegion::NONE);
  } else {
    ColorValue::init_with_mod(lcvs,min_val_,max_val_,mod_,BandsRegion::NONE);
  }
  for (int conflict_region = 0; conflict_region <= last_region; ++conflict_region) {
    if (core.bands_for(conflict_region)) {
      ColorValue::insertNoneSetToColorValues(lcvs, none_sets[conflict_region],
          BandsRegion::regionFromOrder(BandsRegion::NUMBER_OF_CONFLICT_BANDS-conflict_region));
      if (none_sets[conflict_region].isEmpty()) {
        break;
      }
    }
  }
  if (recovery) {
    ColorValue::insertRecoverySetToColorValues(lcvs,none_sets[last_region]);
  }
}

/**
 * Create an IntervalSet that represents a satured NONE band
 */
void DaidalusRealBands::saturateNoneIntervalSet(IntervalSet& noneset) const {
  noneset.clear();
  if (mod_ == 0) {
    noneset.almost_add(min_val_,max_val_,DaidalusParameters::ALMOST_);
  } else {
    if (circular_) {
      noneset.almost_add(0,mod_,DaidalusParameters::ALMOST_);
    } else if (min_val_ < max_val_) {
      noneset.almost_add(min_val_,max_val_,DaidalusParameters::ALMOST_);
    } else {
      noneset.almost_add(min_val_,mod_,DaidalusParameters::ALMOST_);
      noneset.almost_add(0,max_val_,DaidalusParameters::ALMOST_);
    }
  }
}

/**
 * Compute none bands for a const std::vector<IndexLevelT>& ilts of IndexLevelT in none_set_region.
 * The none_set_region is initiated as a saturated green band.
 * Uses aircraft detector if parameter detector is none.
 * The epsilon parameters for coordinations are handled according to the recovery_case flag.
 */
void DaidalusRealBands::compute_none_bands(IntervalSet& none_set_region, const std::vector<IndexLevelT>& ilts,
    Detection3D* det, Detection3D* recovery,
    bool recovery_case, double B, DaidalusCore& core) {
  saturateNoneIntervalSet(none_set_region);
  // Compute bands for given region
  std::vector<IndexLevelT>::const_iterator ilt_ptr;
  for (ilt_ptr = ilts.begin(); ilt_ptr != ilts.end(); ++ilt_ptr) {
    const TrafficState& intruder = core.traffic[ilt_ptr->index];
    int alerter_idx = core.alerter_index_of(intruder);
    if (1 <= alerter_idx && alerter_idx <= core.parameters.numberOfAlerters()) {
      const Alerter& alerter = core.parameters.getAlerterAt(alerter_idx);
      Detection3D* detector = (det == NULL ? alerter.getLevel(ilt_ptr->level).getCoreDetectionPtr() : det);
      IntervalSet noneset2 = IntervalSet();
      double T = ilt_ptr->time_horizon;
      if (B > T) {
        // This case corresponds to recovery bands, where B is a recovery time.
        // If recovery time is greater than lookahead time for aircraft, then only
        // the internal cylinder is checked until this time.
        if (recovery != NULL) {
          none_bands(noneset2,recovery,NULL,
              core.epsilonH(recovery_case,intruder),core.epsilonV(recovery_case,intruder),0,T,
              core.parameters,core.ownship,intruder);
        } else {
          saturateNoneIntervalSet(noneset2);
        }
      } else if (B <= T) {
        none_bands(noneset2,detector,recovery,
            core.epsilonH(recovery_case,intruder),core.epsilonV(recovery_case,intruder),B,T,
            core.parameters,core.ownship,intruder);
      }
      none_set_region.almost_intersect(noneset2,DaidalusParameters::ALMOST_);
      if (none_set_region.isEmpty()) {
        break; // No need to compute more bands. This region is currently saturated.
      }
    }
  }
}

/**
 * Compute recovery bands. Class variables recovery_time_, recovery_horizontal_distance_,
 * and recovery_vertical_distance_ are set.
 * Return true if non-saturated recovery bands where computed
 */
bool DaidalusRealBands::compute_recovery_bands(IntervalSet& none_set_region, const std::vector<IndexLevelT>& ilts,
    DaidalusCore& core) {
  recovery_time_ = NINFINITY;
  recovery_nfactor_ = 0;
  recovery_horizontal_distance_ = NINFINITY;
  recovery_vertical_distance_ = NINFINITY;
  double T = core.parameters.getLookaheadTime();
  CDCylinder cd3d = CDCylinder::mk(core.parameters.getHorizontalNMAC(),core.parameters.getVerticalNMAC());
  Detection3D* ocd3d = &cd3d;
  compute_none_bands(none_set_region,ilts,ocd3d,NULL,true,0.0,core);
  if (none_set_region.isEmpty()) {
    // If solid red, nothing to do. No way to kinematically escape using vertical speed without intersecting the
    // NMAC cylinder
    return false;
  } else {
    cd3d = CDCylinder::mk(core.minHorizontalRecovery(),core.minVerticalRecovery());
    ocd3d = &cd3d;
    double factor = 1-core.parameters.getCollisionAvoidanceBandsFactor();
    while (cd3d.getHorizontalSeparation()  > core.parameters.getHorizontalNMAC() ||
        cd3d.getVerticalSeparation() > core.parameters.getVerticalNMAC()) {
      compute_none_bands(none_set_region,ilts,ocd3d,NULL,true,0.0,core);
      bool solidred = none_set_region.isEmpty();
      if (solidred && !core.parameters.isEnabledCollisionAvoidanceBands()) {
        // Saturated band and collision avoidance is not enabled. Nothing to do here.
        return false;
      } else if (!solidred) {
        // Find first green band
        double pivot_red = 0;
        double pivot_green = T+1;
        double pivot = pivot_green-1;
        while ((pivot_green-pivot_red) > 0.5) {
          compute_none_bands(none_set_region,ilts,NULL,ocd3d,true,pivot,core);
          solidred = none_set_region.isEmpty();
          if (solidred) {
            pivot_red = pivot;
          } else {
            pivot_green = pivot;
          }
          pivot = (pivot_red+pivot_green)/2.0;
        }
        double recovery_time;
        if (pivot_green <= T) {
          recovery_time = Util::min(T,
              pivot_green+core.parameters.getRecoveryStabilityTime());
        } else {
          recovery_time = pivot_red;
        }
        compute_none_bands(none_set_region,ilts,NULL,ocd3d,true,
            recovery_time,core);
        solidred = none_set_region.isEmpty();
        if (!solidred) {
          recovery_time_ = recovery_time;
          recovery_horizontal_distance_ = cd3d.getHorizontalSeparation();
          recovery_vertical_distance_ = cd3d.getVerticalSeparation();
          return true;
        } else if (!core.parameters.isEnabledCollisionAvoidanceBands()) {
          // Nothing else to do. Collision avoidance bands are not enabled.
          return false;
        }
      }
      ++recovery_nfactor_;
      cd3d.setHorizontalSeparation(std::max(core.parameters.getHorizontalNMAC(),cd3d.getHorizontalSeparation()*factor));
      cd3d.setVerticalSeparation(std::max(core.parameters.getVerticalNMAC(),cd3d.getVerticalSeparation()*factor));
    }
  }
  return false;
}

/**
 * Requires: compute_bands(conflict_region) = true && 0 <= conflict_region < CONFLICT_BANDS
 * Compute bands for one region. Return true iff recovery bands were computed.
 */
bool DaidalusRealBands::compute_region(std::vector<IntervalSet>& none_sets, int conflict_region, int corrective_region, DaidalusCore& core) {
  if (saturate_corrective_bands(core.parameters,core.DTAStatus()) && conflict_region <= corrective_region) {
    none_sets[conflict_region].clear();
    return false;
  }
  compute_none_bands(none_sets[conflict_region], acs_bands_[conflict_region],
      NULL,NULL,false,0.0,core);
  if (get_recovery(core.parameters)) {
    if  (none_sets[conflict_region].isEmpty() && conflict_region <= corrective_region) {
      // Compute recovery bands
      compute_recovery_bands(none_sets[corrective_region],acs_bands_[corrective_region],core);
      return true;
    } else if (instantaneous_bands(core.parameters) && conflict_region == corrective_region &&
        core.tiov(conflict_region).low == 0) {
      // Recovery bands for instantaneous bands saturate when internal volume is violated
      recovery_time_ = 0;
      recovery_nfactor_ = 0;
      recovery_horizontal_distance_ = core.parameters.getMinHorizontalRecovery();
      recovery_vertical_distance_ = core.parameters.getMinVerticalRecovery();
      return true;
    }
  }
  // Normal conflict bands (recovery bands are not computed)
  return false;
}

/**
 * Compute all bands.
 */
void DaidalusRealBands::compute(DaidalusCore& core) {
  recovery_time_ = NaN;
  recovery_horizontal_distance_ = NaN;
  recovery_vertical_distance_ = NaN;
  std::vector<IntervalSet> none_sets;
  none_sets.resize(BandsRegion::NUMBER_OF_CONFLICT_BANDS); // use resize since we are then using subscript assignment
  for (int conflict_region=0;conflict_region<BandsRegion::NUMBER_OF_CONFLICT_BANDS;++conflict_region) {
    none_sets[conflict_region] = IntervalSet();
  }
  bool recovery = false;
  bool saturated = false;
  int conflict_region = 0;
  double val = own_val(core.ownship);
  int corrective_region = BandsRegion::NUMBER_OF_CONFLICT_BANDS-BandsRegion::orderOfRegion(core.parameters.getCorrectiveRegion());
  // From most severe to least severe
  while (conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS && !saturated)  {
    if (core.bands_for(conflict_region)) {
      recovery = compute_region(none_sets,conflict_region,corrective_region,core);
      saturated = recovery || none_sets[conflict_region].isEmpty();
    }
    ++conflict_region;
  }
  if (recovery) {
    conflict_region = corrective_region;
  } else {
    --conflict_region;
  }
  // At this point conflict_region has the last region for which bands are computed
  // Compute list of color values (primitive representation of bands)
  std::vector<ColorValue> lcvs;
  color_values(lcvs,none_sets,core,recovery,conflict_region);

  // From this point of hysteresis logic, including M of N and persistence, is applied.
  if (ISNAN(bands_hysteresis_.getLastTime())) {
    bands_hysteresis_.initialize(core.parameters.getHysteresisTime(),
        core.parameters.getPersistenceTime(),
        core.parameters.isEnabledBandsPersistence(),
        core.parameters.getAlertingParameterM(),
        core.parameters.getAlertingParameterN());
  }
  bands_hysteresis_.resetIfCurrentTime(core.current_time);
  bands_hysteresis_.m_of_n(lcvs);
  int idx = bands_hysteresis_.bandsPersistence(ranges_,lcvs,recovery,val);
  // Compute resolutions and preferred direction using persistence logic
  bands_hysteresis_.bandsHysteresis(ranges_,core.parameters.getCorrectiveRegion(),
      max_delta_resolution(core.parameters),recovery_nfactor_,val,idx);
}

/**
 * Returns resolution maneuver.
 * Return NaN if there is no conflict or if input is invalid.
 * Return positive/negative infinity if there is no resolution to the
 * right/up and negative infinity if there is no resolution to the left/down.
 */
double DaidalusRealBands::resolution(DaidalusCore& core, bool dir) {
  refresh(core);
  if (dir) {
    return bands_hysteresis_.getLastResolutionUp();
  } else {
    return bands_hysteresis_.getLastResolutionLow();
  }
}

/**
 * Compute preferred direction based on resolution that is closer
 * to current value.
 */
bool DaidalusRealBands::preferred_direction(DaidalusCore& core) {
  refresh(core);
  return bands_hysteresis_.getLastPreferredDirection();
}

/**
 * Return last time to maneuver, in seconds, for ownship with respect to traffic
 * aircraft at ac_idx for conflict alert level. Return NaN if the ownship is not
 * in conflict with aircraft within lookahead time. Return negative infinity if
 * there is no time to maneuver.
 * Note: 1 <= alert_level <= alerter.size()
 */
double DaidalusRealBands::last_time_to_maneuver(DaidalusCore& core, const TrafficState& intruder) {
  int alert_idx = core.parameters.isAlertingLogicOwnshipCentric() ? core.ownship.getAlerterIndex() : intruder.getAlerterIndex();
  int alert_level = core.parameters.correctiveAlertLevel(alert_idx);
  if (set_input(core.parameters,core.ownship,core.DTAStatus()) && alert_level > 0) {
    const AlertThresholds& alertthr = core.parameters.getAlerterAt(alert_idx).getLevel(alert_level);
    Detection3D* detector = alertthr.getCoreDetectionPtr();
    double T = Util::min(core.parameters.getLookaheadTime(),alertthr.getEarlyAlertingTime());
    ConflictData det = detector->conflictDetectionWithTrafficState(core.ownship,intruder,0.0,T);
    if (det.conflict()) {
      double pivot_red = det.getTimeIn();
      if (pivot_red == 0) {
        return NINFINITY;
      }
      double pivot_green = 0;
      double pivot = pivot_green;
      while ((pivot_red-pivot_green) > 0.5) {
        TrafficState ownship_at_pivot  = core.ownship.linearProjection(pivot);
        TrafficState intruder_at_pivot = intruder.linearProjection(pivot);
        if (detector->violationAtWithTrafficState(ownship_at_pivot,intruder_at_pivot,0.0) ||
            all_red(detector,NULL,0,0,0.0,T,core.parameters,ownship_at_pivot,intruder_at_pivot)) {
          pivot_red = pivot;
        } else {
          pivot_green = pivot;
        }
        pivot = (pivot_red+pivot_green)/2.0;
      }
      if (pivot_green == 0) {
        return NINFINITY;
      } else {
        return pivot_green;
      }
    }
  }
  return NaN;
}

int DaidalusRealBands::maxdown(const DaidalusParameters& parameters, const TrafficState& ownship) const {
  int down = static_cast<int>(std::ceil(min_relative_/get_step(parameters)));
  if (mod_ > 0 && Util::almost_greater(down*get_step(parameters),mod_/2.0,DaidalusParameters::ALMOST_)) {
    --down;
  }
  return down;
}

int DaidalusRealBands::maxup(const DaidalusParameters& parameters, const TrafficState& ownship) const {
  int up = static_cast<int>(std::ceil(max_relative_/get_step(parameters)));
  if (mod_ > 0 && Util::almost_greater(up*get_step(parameters),mod_/2.0,DaidalusParameters::ALMOST_)) {
    --up;
  }
  return up;
}

/** Add (lb,ub) to noneset. In the case of mod_ > 0, lb can be greater than ub. This function takes
 * care of the mod logic. This function doesn't do anything when lb and ub are almost equals.
 * @param noneset: Interval set where (lb,ub) will be added
 * @param lb: lower bound
 * @param ub: upper bound
 * When mod_ = 0, lb <= ub.
 */
void DaidalusRealBands::add_noneset(IntervalSet& noneset, double lb, double ub) const {
  if (Util::almost_equals(lb,ub,DaidalusParameters::ALMOST_)) {
    return;
  }
  if (mod_ == 0)  {
    lb = Util::max(min_val_,lb);
    ub = Util::min(max_val_,ub);
    if (Util::almost_less(lb,ub,DaidalusParameters::ALMOST_)) {
      noneset.almost_add(lb,ub,DaidalusParameters::ALMOST_);
    }
    return;
  }
  lb = Util::safe_modulo(lb,mod_);
  ub = Util::safe_modulo(ub,mod_);
  IntervalSet minmax_noneset = IntervalSet();
  if (circular_) {
  } else if (min_val_ < max_val_) {
    minmax_noneset.almost_add(min_val_,max_val_,DaidalusParameters::ALMOST_);
  } else {
    minmax_noneset.almost_add(0,max_val_,DaidalusParameters::ALMOST_);
    minmax_noneset.almost_add(min_val_,mod_,DaidalusParameters::ALMOST_);
  }
  IntervalSet lbub_noneset = IntervalSet();
  if (Util::almost_equals(lb,ub,DaidalusParameters::ALMOST_)) {
  } else if (lb < ub) {
    lbub_noneset.almost_add(lb,ub,DaidalusParameters::ALMOST_);
  } else {
    lbub_noneset.almost_add(0,ub,DaidalusParameters::ALMOST_);
    lbub_noneset.almost_add(lb,mod_,DaidalusParameters::ALMOST_);
  }
  if (minmax_noneset.isEmpty() && lbub_noneset.isEmpty()) {
    // In this case, the band if all green from 0 to mod
    noneset.almost_add(0,mod_,DaidalusParameters::ALMOST_);
  } else if (lbub_noneset.isEmpty()) {
    // In this case return minmax_noneset
    noneset.almost_unions(minmax_noneset,DaidalusParameters::ALMOST_);
  } else {
    if (!minmax_noneset.isEmpty()) {
      lbub_noneset.almost_intersect(minmax_noneset,DaidalusParameters::ALMOST_);
    }
    noneset.almost_unions(lbub_noneset,DaidalusParameters::ALMOST_);
  }
}

/**
 *  This function scales the interval, add a constant, and constraint the intervals to min and max.
 *  The function takes care of modulo logic, in the case of circular bands.
 */
void DaidalusRealBands::toIntervalSet(IntervalSet& noneset, const std::vector<Integerval>& l, double scal, double add) const {
  noneset.clear();
  for (int i=0; i < static_cast<int>(l.size()); ++i) {
    const Integerval& ii = l[i];
    if (ii.lb == ii.ub) {
      continue;
    }
    double lb = scal*ii.lb+add;
    double ub = scal*ii.ub+add;
    add_noneset(noneset,lb,ub);
  }
}

/**
 * The output parameter noneset has a list of non-conflict ranges orderd within [min,max]
 * values (or [0,mod] in the case of circular bands, i.e., when mod == 0).
 */
void DaidalusRealBands::none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  std::vector<Integerval> bands_int;
  int mino = maxdown(parameters,ownship);
  int maxo = maxup(parameters,ownship);
  if (instantaneous_bands(parameters)) {
    instantaneous_bands_combine(bands_int,conflict_det,recovery_det,B,T,0.0,B,
        mino,maxo,parameters,ownship,traffic,epsh,epsv);
  } else {
    kinematic_bands_combine(bands_int,conflict_det,recovery_det,time_step(parameters,ownship),B,T,0.0,B,
        mino,maxo,parameters,ownship,traffic,epsh,epsv);
  }
  toIntervalSet(noneset,bands_int,get_step(parameters),own_val(ownship));
}

bool DaidalusRealBands::any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  return instantaneous_bands(parameters) ?
      any_instantaneous_red(conflict_det,recovery_det,B,T,0.0,B,
          maxdown(parameters,ownship),maxup(parameters,ownship),parameters,ownship,traffic,epsh,epsv,0):
          any_int_red(conflict_det,recovery_det,time_step(parameters,ownship),B,T,0.0,B,
              maxdown(parameters,ownship),maxup(parameters,ownship),parameters,ownship,traffic,epsh,epsv,0);
}

bool DaidalusRealBands::all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  return instantaneous_bands(parameters) ?
      all_instantaneous_red(conflict_det,recovery_det,B,T,0,B,
          maxdown(parameters,ownship),maxup(parameters,ownship),parameters,ownship,traffic,epsh,epsv,0):
          all_int_red(conflict_det,recovery_det,time_step(parameters,ownship),B,T,0.0,B,
              maxdown(parameters,ownship),maxup(parameters,ownship),parameters,ownship,traffic,epsh,epsv,0);
}

bool DaidalusRealBands::all_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  return !any_red(conflict_det,recovery_det,epsh,epsv,B,T,parameters,ownship,traffic);
}

bool DaidalusRealBands::any_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic) {
  return !all_red(conflict_det,recovery_det,epsh,epsv,B,T,parameters,ownship,traffic);
}

/**
 * This function returns a resolution maneuver that is valid from B to T.
 * It returns NaN if there is no conflict and +/- infinity, depending on dir, if there
 * are no resolutions.
 * The value dir=false is down and dir=true is up.
 */
double DaidalusRealBands::resolution(const Detection3D* conflict_det, const Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const DaidalusParameters& parameters, const TrafficState& ownship, const TrafficState& traffic,
    bool dir) {
  int maxn;
  int sign;
  if (dir) {
    maxn = maxup(parameters,ownship);
    sign = 1;
  } else {
    maxn = maxdown(parameters,ownship);
    sign = -1;
  }
  int ires = first_green(conflict_det,recovery_det,time_step(parameters,ownship),B,T,0.0,B,
      dir,maxn,parameters,ownship,traffic,epsh,epsv);
  if (ires == 0) {
    return NaN;
  } else if (ires < 0) {
    return sign*PINFINITY;
  } else {
    return Util::safe_modulo(own_val(ownship)+sign*ires*get_step(parameters),mod_);
  }
}

std::string DaidalusRealBands::rawString() const {
  std::string s = "";
  s+="# Private variables\n";
  s+="mod_ = "+FmPrecision(mod_)+"\n";
  s+="min_rel_ = "+FmPrecision(min_rel_)+"\n";
  s+="max_rel_ = "+FmPrecision(max_rel_)+"\n";
  s+="# Cached variables\n";
  s+="outdated_ = "+Fmb(outdated_)+"\n";
  s+="checked_ = "+Fmi(checked_)+"\n";
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    s+="acs_peripheral_bands_["+Fmi(conflict_region)+"] = "+
        IndexLevelT::toString(acs_peripheral_bands_[conflict_region])+"\n";
  }
  for (int conflict_region=0; conflict_region < BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++conflict_region) {
    s+="acs_bands_["+Fmi(conflict_region)+"] = "+
        IndexLevelT::toString(acs_bands_[conflict_region])+"\n";
  }
  for (int i = 0; i < static_cast<int>(ranges_.size()); ++i) {
    s+="ranges_["+Fmi(i)+"] = ";
    s+=ranges_[i].toString()+"\n";
  }
  s+="recovery_time_ = "+FmPrecision(recovery_time_)+"\n";
  s+="recovery_nfactor_ = "+Fmi(recovery_nfactor_)+"\n";
  s+="recovery_horizontal_distance_ = "+FmPrecision(recovery_horizontal_distance_)+"\n";
  s+="recovery_vertical_distance_ = "+FmPrecision(recovery_vertical_distance_)+"\n";
  s+="min_val_ = "+FmPrecision(min_val_)+"\n";
  s+="max_val_ = "+FmPrecision(max_val_)+"\n";
  s+="min_relative_ = "+FmPrecision(min_relative_)+"\n";
  s+="max_relative_ = "+FmPrecision(max_relative_)+"\n";
  s+="circular_ = "+Fmb(circular_)+"\n";
  s+=bands_hysteresis_.toString();
  return s;
}

std::string DaidalusRealBands::toString() const {
  std::string s = "";
  for (int i = 0; i < static_cast<int>(ranges_.size()); ++i) {
    s+="ranges["+Fmi(i)+"] = ";
    s+=ranges_[i].toString()+"\n";
  }
  s+="recovery_time = "+FmPrecision(recovery_time_)+" [s]\n";
  s+="recovery_nfactor = "+Fmi(recovery_nfactor_)+"\n";
  s+="recovery_horizontal_distance = "+FmPrecision(recovery_horizontal_distance_)+ " [m]\n";
  s+="recovery_vertical_distance = "+FmPrecision(recovery_vertical_distance_)+ " [m]\n";
  s+="preferred_dir = "+Fmb(bands_hysteresis_.getLastPreferredDirection())+"\n";
  s+="resolution_low = "+FmPrecision(bands_hysteresis_.getLastResolutionLow())+"\n";
  s+="resolution_up = "+FmPrecision(bands_hysteresis_.getLastResolutionUp())+"\n";
  return s;
}

std::string DaidalusRealBands::toPVS() const {
  std::string s = "(:";
  for (int i = 0; i < static_cast<int>(ranges_.size()); ++i) {
    if (i > 0) {
      s+=", ";
    } else {
      s+=" ";
    }
    s += ranges_[i].toPVS();
  }
  s+=" :)";
  return s;
}

}

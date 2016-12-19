/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "KinematicRealBands.h"
#include "Interval.h"
#include "IntervalSet.h"
#include "Util.h"
#include "CDCylinder.h"
#include "format.h"
#include "ColoredValue.h"

namespace larcfm {

KinematicRealBands::KinematicRealBands() {
  outdated_ = true;
  checked_ = -1;
  peripheral_acs_ = std::vector< std::vector<TrafficState> >();
  ranges_ = std::vector<BandsRange>();
  recovery_time_ = NaN;
  resolutions_ = std::vector<Interval>();
  min_ = 0;
  max_ = 0;
  rel_ = false;
  mod_ = 0;
  circular_ = false;
  step_ = 0;
  recovery_ = false;
}

KinematicRealBands::KinematicRealBands(double min, double max, bool rel, double mod, double step, bool recovery) {
  outdated_ = true;
  checked_ = -1;
  peripheral_acs_ = std::vector< std::vector<TrafficState> >();
  ranges_ = std::vector<BandsRange>();
  recovery_time_ = NaN;
  resolutions_ = std::vector<Interval>();
  min_ = min;
  max_ = max;
  rel_ = rel;
  mod_ = mod;
  circular_ = false;
  step_ = step;
  recovery_ = recovery;
}

KinematicRealBands::KinematicRealBands(double min, double max, double step, bool recovery) {
  outdated_ = true;
  checked_ = -1;
  peripheral_acs_ = std::vector< std::vector<TrafficState> >();
  ranges_ = std::vector<BandsRange>();
  recovery_time_ = NaN;
  resolutions_ = std::vector<Interval>();
  min_ = min;
  max_ = max;
  rel_ = false;
  mod_ = 0;
  circular_ = false;
  step_ = step;
  recovery_ = recovery;
}

KinematicRealBands::KinematicRealBands(const KinematicRealBands& b) {
  outdated_ = true;
  checked_ = -1;
  peripheral_acs_ = std::vector< std::vector<TrafficState> >();
  ranges_ = std::vector<BandsRange>();
  recovery_time_ = NaN;
  resolutions_ = std::vector<Interval>();
  min_ = b.min_;
  max_ = b.max_;
  rel_ = b.rel_;
  mod_ = b.mod_;
  circular_ = false;
  step_ = b.step_;
  recovery_ = b.recovery_;
}

double KinematicRealBands::get_min() const {
  return min_;
}

double KinematicRealBands::get_max() const {
  return max_;
}

bool KinematicRealBands::get_rel() const {
  return rel_;
}

double KinematicRealBands::get_mod() const {
  return mod_;
}

double KinematicRealBands::get_step() const {
  return step_;
}

bool KinematicRealBands::get_recovery() const {
  return recovery_;
}

void KinematicRealBands::set_min(double val) {
  if (val != min_) {
    min_ = val;
    reset();
  }
}

void KinematicRealBands::set_max(double val) {
  if (val != max_) {
    max_ = val;
    reset();
  }
}

// As a side effect this method resets the min_/max_ values.
void KinematicRealBands::set_rel(bool val) {
  if (val != rel_) {
    rel_ = val;
    min_ = NaN;
    max_ = NaN;
    reset();
  }
}

void KinematicRealBands::set_mod(double val) {
  if (val >= 0 && val != mod_) {
    mod_ = val;
    reset();
  }
}

void KinematicRealBands::set_step(double val) {
  if (val > 0 && val != step_) {
    step_ = val;
    reset();
  }
}

void KinematicRealBands::set_recovery(bool flag) {
  if (flag != recovery_) {
    recovery_ = flag;
    reset();
  }
}

/**
 * Return val modulo mod_, when mod_ > 0. Otherwise, returns val.
 */
double KinematicRealBands::mod_val(double val) const {
  return mod_ > 0 ? Util::modulo(val,mod_) : val;
}

/**
 * Returned value is in [0,mod_]. When mod_ == 0, min_val() <= max_val().
 * When mod_ > 0, it is not always true that min_val() <= max_val()
 */
double KinematicRealBands::min_val(const TrafficState& ownship) const {
  if (circular_) {
    return 0;
  }
  return rel_ ? mod_val(own_val(ownship)+min_) : min_;
}

/*
 * Return a positive number in [0,mod_/2]
 */
double KinematicRealBands::min_rel(const TrafficState& ownship) const {
  if (circular_) {
    return mod_/2.0;
  }
  return rel_ ? -min_ : mod_val(own_val(ownship)-min_);
}

/*
 * Returned value is in [0,mod_]. When mod_ == 0, min_val() <= max_val().
 * When mod_ > 0, it is not always true that min_val() <= max_val()
 */
double KinematicRealBands::max_val(const TrafficState& ownship) const {
  if (circular_) {
    return mod_;
  }
  return rel_ ? mod_val(own_val(ownship)+max_) : max_;
}

/*
 * Return a positive number in [0,mod_/2]
 */
double KinematicRealBands::max_rel(const TrafficState& ownship) const {
  if (circular_) {
    return mod_/2.0;
  }
  return rel_ ? max_ : mod_val(max_ - own_val(ownship));
}

bool KinematicRealBands::check_input(const KinematicBandsCore& core) {
  if (checked_ < 0) {
    checked_ = 0;
    int level = core.parameters.alertor.conflictAlertLevel();
    if (core.ownship.isValid() && step_ > 0 && ISFINITE(min_) && ISFINITE(max_)&&
        BandsRegion::isConflictBand(core.parameters.alertor.getLevel(level).getRegion())) {
      double val = own_val(core.ownship);
      if (rel_ ? min_ <= 0.0 && max_ >= 0.0 :
          min_ <= val && val <= max_) {
        if (mod_ >= 0.0 && (mod_ == 0.0 ||
            (Util::almost_leq(max_-min_,mod_) &&
                (rel_ ? Util::almost_leq(max_,mod_/2.0): Util::almost_leq(max_,mod_))))) {
          checked_ = 1;
          circular_ = mod_ > 0 && Util::almost_equals(max_-min_,mod_);
        }
      }
    }
  }
  return checked_ > 0;
}

bool KinematicRealBands::kinematic_conflict(KinematicBandsCore& core, const TrafficState& ac,
    Detection3D* detector, double alerting_time) {
  std::vector<TrafficState> alerting_set = std::vector<TrafficState>();
  alerting_set.push_back(ac);
  return check_input(core) &&
      any_red(detector,NULL,core.criteria_ac(),core.epsilonH(),core.epsilonV(),
          0,alerting_time,core.ownship,alerting_set);
}

int KinematicRealBands::length(KinematicBandsCore& core) {
  update(core);
  return ranges_.size();
}

Interval KinematicRealBands::interval(KinematicBandsCore& core, int i) {
  if (i < 0 || i >= length(core)) {
    return Interval::EMPTY;
  }
  return ranges_[i].interval;
}

BandsRegion::Region KinematicRealBands::region(KinematicBandsCore& core, int i) {
  if (i < 0 || i >= length(core)) {
    return BandsRegion::UNKNOWN;
  } else {
    return ranges_[i].region;
  }
}

/*
 *  Returns true if the first interval extends to the last interval. This happens
 *  when mod_ > 0, the low value is 0, and the max value is mod_.
 */
bool KinematicRealBands::rollover() {
  return mod_ > 0 &&
      Util::almost_equals(ranges_[0].interval.low,0) &&
      Util::almost_equals(ranges_[ranges_.size()-1].interval.up,mod_);
}

/**
 * Return index where val is found, -1 if invalid input, >= length if not found
 */
int KinematicRealBands::rangeOf(KinematicBandsCore& core, double val) {
  if (check_input(core)) {
    val = mod_val(val);
    int last_index = length(core)-1;
    bool rov = rollover();
    for (int i=0; i <= last_index; ++i) {
      bool none = BandsRegion::isResolutionBand(ranges_[i].region);
      int order_i = BandsRegion::order(ranges_[i].region);
      bool lb_close = none ||
          (i > 0 && order_i <= BandsRegion::order(ranges_[i-1].region)) ||
          (i == 0 && rov && order_i <= BandsRegion::order(ranges_[last_index].region));
      bool ub_close = none ||
          (i < last_index && order_i <= BandsRegion::order(ranges_[i+1].region)) ||
          (i == last_index && rov && order_i <= BandsRegion::order(ranges_[0].region));
      if (ranges_[i].interval.almost_in(val,lb_close,ub_close)) {
        return i;
      }
    }
    if (rov) {
      if (Util::almost_equals(val,0)) {
        return 0;
      }
    } else {
      if (Util::almost_equals(val,min_val(core.ownship))) {
        return 0;
      }
      if (Util::almost_equals(val,max_val(core.ownship))) {
        return last_index;
      }
    }
  }
  return -1;
}

/**
 *  Reset cached values
 */
void KinematicRealBands::reset() {
  outdated_ = true;
  checked_ = -1;
  ranges_.clear();
  recovery_time_ = NaN;
  resolutions_.clear();
}

/**
 *  Update cached values
 */
void KinematicRealBands::update(KinematicBandsCore& core) {
  if (outdated_) {
    for (int alert_level=1; alert_level <= core.parameters.alertor.mostSevereAlertLevel(); ++alert_level) {
      if (alert_level-1 >= (int) peripheral_acs_.size()) {
        peripheral_acs_.push_back(std::vector<TrafficState>());
      } else {
        peripheral_acs_[alert_level-1].clear();
      }
      if (BandsRegion::isConflictBand(core.parameters.alertor.getLevel(alert_level).getRegion())) {
        peripheral_aircraft(core,alert_level);
      }
    }
    if (check_input(core)) {
      compute(core);
    }
    outdated_ = false;
  }
}

/**
 *  Force computation of kinematic bands
 */
void KinematicRealBands::force_compute(KinematicBandsCore& core) {
  reset();
  update(core);
}

/**
 * Put in peripheral_acs_ the list of aircraft predicted to be in conflict for the given alert level
 * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
 */
void KinematicRealBands::peripheral_aircraft(KinematicBandsCore& core, int alert_level) {
  Detection3D* detector = core.parameters.alertor.getLevel(alert_level).getDetectorRef();
  double T = core.parameters.alertor.getLevel(alert_level).getAlertingTime();
  for (int i = 0; i < (int) core.traffic.size(); ++i) {
    TrafficState ac = core.traffic[i];
    ConflictData det = detector->conflictDetection(core.own_s(),core.own_v(),ac.get_s(),ac.get_v(),0,T);
    if (!det.conflict() && kinematic_conflict(core,ac,detector,T)) {
      peripheral_acs_[alert_level-1].push_back(ac);
    }
  }
}

/**
 * Return list of peripheral aircraft for a given alert level.
 * Requires: 0 <= alert_level <= alertor.size(). If alert_level is 0,
 * conflict_level is used.
 */
std::vector<TrafficState> const & KinematicRealBands::peripheralAircraft(KinematicBandsCore& core, int alert_level) {
  update(core);
  if (alert_level == 0) {
    alert_level = core.parameters.alertor.conflictAlertLevel();
  }
  if (alert_level >= 1 && alert_level <= core.parameters.alertor.mostSevereAlertLevel()) {
    return peripheral_acs_[alert_level-1];
  }
  return TrafficState::INVALIDL;
}

/**
 * Return time to recovery. Return NaN if bands are not saturated and negative infinity
 * when bands are saturated but no recovery within max_recovery_time.
 */
double KinematicRealBands::timeToRecovery(KinematicBandsCore& core) {
  update(core);
  return recovery_time_;
}

/**
 * Return list of bands ranges
 */
std::vector<BandsRange> const & KinematicRealBands::ranges(KinematicBandsCore& core) {
  update(core);
  return ranges_;
}

/**
 * Ensure that the intervals are "complete", filling in missing intervals and ensuring the
 * bands end at the  proper bounds.
 * Requires none_sets to be a non-empty list and size(none_sets) == size(regions)
 */
void KinematicRealBands::color_bands(const std::vector<IntervalSet>& none_sets, const std::vector<BandsRegion::Region>& regions,
    KinematicBandsCore& core, bool recovery) {

  double min = min_val(core.ownship);
  double max = max_val(core.ownship);

  // Lists colored bounds
  std::vector<ColoredValue> l1 = std::vector<ColoredValue>();
  std::vector<ColoredValue> l2 = std::vector<ColoredValue>();

  BandsRegion::Region green = recovery? BandsRegion::RECOVERY : BandsRegion::NONE;

  if (mod_ == 0 || min <= max) {
    l1.push_back(ColoredValue(min,BandsRegion::UNKNOWN));
    l1.push_back(ColoredValue(max,regions[regions.size()-1]));
  } else {
    // When mod !=0 && min > max, there are two lists of colored bounds
    l1.push_back(ColoredValue(0,BandsRegion::UNKNOWN));
    l1.push_back(ColoredValue(max,regions[regions.size()-1]));
    l2.push_back(ColoredValue(min,BandsRegion::UNKNOWN));
    l2.push_back(ColoredValue(mod_,regions[regions.size()-1]));
  }

  int last_level = recovery ? none_sets.size()-1 : 0;

  // Color levels from most severe to less severe
  for (int level = none_sets.size()-1; level >= last_level; --level) {
    BandsRegion::Region lb_color = regions[level];
    BandsRegion::Region ub_color = level == last_level ? green : regions[level-1];
    for (int i=0; i < none_sets[level].size(); ++i) {
      Interval ii = none_sets[level].getInterval(i);
      if (ii.up <= max) {
        ColoredValue::insert(l1,ii,lb_color,ub_color);
      } else {
        ColoredValue::insert(l2,ii,lb_color,ub_color);
      }
    }
  }

  ranges_.clear();
  ColoredValue::toBands(ranges_, l1);
  if (mod_ != 0 && min > max) {
    ColoredValue::toBands(ranges_, l2);
  }
}

/**
 * Compute recovery bands. Returns recovery time.
 */
double KinematicRealBands::compute_recovery_bands(IntervalSet& noneset, KinematicBandsCore& core,
    const std::vector<TrafficState>& alerting_set) {
  double recovery_time = NINFINITY;
  int recovery_level = core.parameters.alertor.conflictAlertLevel();
  Detection3D* detector = core.parameters.alertor.getLevel(recovery_level).getDetectorRef();
  double T = core.parameters.alertor.getLevel(recovery_level).getEarlyAlertingTime();
  TrafficState repac = core.recovery_ac();
  CDCylinder cd3d = CDCylinder::mk(core.parameters.getHorizontalNMAC(),core.parameters.getVerticalNMAC());
  none_bands(noneset,&cd3d,NULL,repac,core.epsilonH(),core.epsilonV(),0,T,core.ownship,alerting_set);
  if (!noneset.isEmpty()) {
    // If solid red, nothing to do. No way to kinematically escape using vertical speed without intersecting the
    // NMAC cylinder
    cd3d = CDCylinder::mk(core.minHorizontalRecovery(),core.minVerticalRecovery());
    double factor = 1-core.parameters.getCollisionAvoidanceBandsFactor();
    while (cd3d.getHorizontalSeparation() > core.parameters.getHorizontalNMAC() || cd3d.getVerticalSeparation() > core.parameters.getVerticalNMAC()) {
      none_bands(noneset,&cd3d,NULL,repac,core.epsilonH(),core.epsilonV(),0,T,core.ownship,alerting_set);
      bool solidred = noneset.isEmpty();
      if (solidred && !core.parameters.isEnabledCollisionAvoidanceBands()) {
        return recovery_time;
      } else if (!solidred) {
        // Find first green band
        double pivot_red = 0;
        double pivot_green = T+1;
        double pivot = pivot_green-1;
        while ((pivot_green-pivot_red) > 0.5) {
          none_bands(noneset,detector,&cd3d,repac,core.epsilonH(),core.epsilonV(),pivot,T,core.ownship,alerting_set);
          solidred = noneset.isEmpty();
          if (solidred) {
            pivot_red = pivot;
          } else {
            pivot_green = pivot;
          }
          pivot = (pivot_red+pivot_green)/2.0;
        }
        if (pivot_green <= T) {
          recovery_time = Util::min(T,pivot_green+core.parameters.getRecoveryStabilityTime());
        } else {
          recovery_time = pivot_red;
        }
        none_bands(noneset,detector,&cd3d,repac,core.epsilonH(),core.epsilonV(),recovery_time,T,core.ownship,alerting_set);
        solidred = noneset.isEmpty();
        if (solidred) {
          recovery_time = NINFINITY;
        }
        if (!solidred || !core.parameters.isEnabledCollisionAvoidanceBands()) {
          return recovery_time;
        }
      }
      cd3d.setHorizontalSeparation(cd3d.getHorizontalSeparation()*factor);
      cd3d.setVerticalSeparation(cd3d.getVerticalSeparation()*factor);
    }
  }
  return recovery_time;
}

/**
 * Compute bands for one level. Return recovery time (NaN if recover bands are not computed)
 */
double KinematicRealBands::compute_level(IntervalSet& noneset, KinematicBandsCore& core, int alert_level) {
  double min = min_val(core.ownship);
  double max = max_val(core.ownship);
  std::vector<TrafficState> alerting_set = std::vector<TrafficState>();
  alerting_set.insert(alerting_set.end(),
      peripheral_acs_[alert_level-1].begin(),peripheral_acs_[alert_level-1].end());
  alerting_set.insert(alerting_set.end(),
      core.conflictAircraft(alert_level).begin(),core.conflictAircraft(alert_level).end());
  if (alerting_set.empty()) {
    if (mod_ == 0 || min <= max) {
      noneset.almost_add(min,max);
    } else {
      noneset.almost_add(min, mod_);
      noneset.almost_add(0,max);
    }
  } else {
    compute_none_bands(noneset,core,alert_level,core.criteria_ac());
    if (recovery_ && alert_level == core.parameters.alertor.conflictAlertLevel()) {
      // Compute recovery bands
      if (noneset.isEmpty()) {
        return compute_recovery_bands(noneset,core,alerting_set);
      } else if (instantaneous_bands() && core.timeIntervalOfViolation(alert_level).low == 0) {
        return 0;
      }
    }
  }
  return NaN;
}

/**
 * Compute all bands.
 */
void KinematicRealBands::compute(KinematicBandsCore& core) {
  recovery_time_ = NaN;
  resolutions_.clear();
  std::vector<IntervalSet> none_sets = std::vector<IntervalSet>();
  std::vector<BandsRegion::Region> regions = std::vector<BandsRegion::Region>();
  bool recovery = false;
  int alert_level;
  for (alert_level=1; alert_level <= core.parameters.alertor.mostSevereAlertLevel() && !recovery; ++alert_level) {
    BandsRegion::Region region = core.parameters.alertor.getLevel(alert_level).getRegion();
    if (BandsRegion::isConflictBand(region)) {
      IntervalSet noneset = IntervalSet();
      double recovery_time = compute_level(noneset,core,alert_level);
      if (!ISNAN(recovery_time)) {
        recovery = true;
        recovery_time_ = recovery_time;
        int cal = core.currentAlertLevel();
        if (cal > alert_level) {
            region = core.parameters.alertor.getLevel(cal).getRegion();
        }
      }
      none_sets.push_back(noneset);
      regions.push_back(region);
      resolutions_.push_back(find_resolution(core,noneset));
    } else {
      resolutions_.push_back(Interval(NaN,NaN));
    }
  }
  for (;alert_level <= core.parameters.alertor.mostSevereAlertLevel();++alert_level) {
    // Add [-oo,+oo] resolutions for level > conflict_level in case of recovery bands
    resolutions_.push_back(Interval(NINFINITY,PINFINITY));
  }
  color_bands(none_sets,regions,core,recovery);
}

Interval KinematicRealBands::find_resolution(KinematicBandsCore& core, const IntervalSet& noneset) {
  double l = NINFINITY;
  double u = PINFINITY;
  if (!noneset.isEmpty()) {
    // There is a resolution
    double val = own_val(core.ownship);
    for (int i=0; i < noneset.size(); ++i) {
      if (noneset.getInterval(i).almost_in(val,true,true)) {
        // There is no conflict
        l = NaN;
        u = NaN;
        break;
      } else if (noneset.getInterval(i).up < val) {
        if (i+1==noneset.size()) {
          l = noneset.getInterval(i).up;
          if (mod_ > 0) {
            u = noneset.getInterval(0).low;
            if (Util::almost_geq(mod_val(u-val),mod_/2.0)) {
              u = PINFINITY;
            }
          }
          break;
        } else if (val < noneset.getInterval(i+1).low) {
          l = noneset.getInterval(i).up;
          u = noneset.getInterval(i+1).low;
          break;
        }
      } else if (val < noneset.getInterval(i).low) {
        if (i==0) {
          if (mod_ > 0) {
            l = noneset.getInterval(noneset.size()-1).up;
            if (Util::almost_geq(mod_val(val-l),mod_/2.0)) {
              l = NINFINITY;
            }
          }
          u = noneset.getInterval(i).low;
          break;
        }
      }
    }
  }
  return Interval(l,u);
}

/**
 * Returns resolution maneuver for given alert level and direction.
 * Return NaN if there is no conflict or if input is invalid.
 * Return positive/negative infinity if there is no resolution to the
 * right/up and negative infinity if there is no resolution to the left/down.
 * Requires: 0 <= alert_level <= alertor.size(). If alert_level is 0,
 * conflict_level is used.
 * Region of alert_level should be a conflict band type, e.g., NEAR, MID, or FAR.
 */
double KinematicRealBands::compute_resolution(KinematicBandsCore& core, int alert_level, bool dir) {
  if (alert_level == 0) {
    alert_level = core.parameters.alertor.conflictAlertLevel();
  }
  if (length(core) > 0 && 1 <= alert_level &&
      alert_level <= core.parameters.alertor.mostSevereAlertLevel() &&
      BandsRegion::isConflictBand(core.parameters.alertor.getLevel(alert_level).getRegion())) {
    Interval resolution = resolutions_[alert_level-1];
    if (dir) {
      return resolution.up;
    } else {
      return resolution.low;
    }
  }
  return NaN;
}


/**
 * Compute preferred direction base on resolution that is closer
 * to current value.
 */
bool KinematicRealBands::preferred_direction(KinematicBandsCore& core, int alert_level) {
  double up = compute_resolution(core,alert_level,true);
  double down = compute_resolution(core,alert_level,false);
  double val = own_val(core.ownship);
  if (!ISFINITE(up)) {
    return false;
  } else if (!ISFINITE(down)) {
    return true;
  }
  return Util::almost_leq(mod_val(up-val),mod_val(val-down));
}

/**
 * Return last time to maneuver, in seconds, for ownship with respect to traffic
 * aircraft ac for conflict alert level. Return NaN if the ownship is not in conflict with aircraft ac within
 * early alerting time. Return negative infinity if there is no time to maneuver.
 * Note: 1 <= alert_level <= alertor.size()
 */
double KinematicRealBands::last_time_to_maneuver(KinematicBandsCore& core, const TrafficState& ac) {
  if (check_input(core)) {
    int conflict_level = core.parameters.alertor.conflictAlertLevel();
    Detection3D* detector = core.parameters.alertor.getLevel(conflict_level).getDetectorRef();
    double T = core.parameters.alertor.getLevel(conflict_level).getEarlyAlertingTime();
    ConflictData det = detector->conflictDetection(core.own_s(),core.own_v(),ac.get_s(),ac.get_v(),0,T);
    if (det.conflict()) {
      double pivot_red = det.getTimeIn();
      if (pivot_red == 0) {
        return NINFINITY;
      }
      TrafficState own = core.ownship;
      std::vector<TrafficState> traffic = std::vector<TrafficState> ();
      double pivot_green = 0;
      double pivot = pivot_green;
      while ((pivot_red-pivot_green) > 0.5) {
        TrafficState ownship  = own.linearProjection(pivot);
        TrafficState intruder = ac.linearProjection(pivot);
        traffic.clear();
        traffic.push_back(intruder);
        if (detector->violation(ownship.get_s(),ownship.get_v(),intruder.get_s(),intruder.get_v()) ||
            all_red(detector,NULL,core.criteria_ac(),0,0,0,T,ownship,traffic)) {
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

int KinematicRealBands::maxdown(const TrafficState& ownship) const {
  int down = (int)std::ceil(min_rel(ownship)/get_step());
  if (mod_ > 0 && Util::almost_greater(down*get_step(),mod_/2.0)) {
    --down;
  }
  return down;
}

int KinematicRealBands::maxup(const TrafficState& ownship) const {
  int up = (int)std::ceil(max_rel(ownship)/get_step());
  if (mod_ > 0 && Util::almost_greater(up*get_step(),mod_/2.0)) {
    --up;
  }
  return up;
}

/**
 *  This function scales the interval, add a constant, and constraint the intervals to min and max.
 *  The function takes care of modulo logic, in the case of circular bands.
 */
void KinematicRealBands::toIntervalSet(IntervalSet& noneset, const std::vector<Integerval>& l,
    double scal, double add, double min, double max) {
  noneset.clear();
  for (int i=0; i < (int) l.size(); ++i) {
    Integerval ii = l[i];
    double lb = scal*ii.lb+add;
    double ub = scal*ii.ub+add;
    if (mod_ == 0)  {
      lb = Util::max(min,lb);
      ub = Util::min(max,ub);
      noneset.almost_add(lb,ub);
    } else {
      lb = mod_val(lb);
      ub = mod_val(ub);
      if (Util::almost_equals(lb,ub)) {
        // In this case the range is the whole interval
        if (min <= max) {
          noneset.almost_add(min,max);
        } else {
          noneset.almost_add(min,mod_);
          noneset.almost_add(0,max);
        }
      } else if (min <= max && lb <= ub) {
        noneset.almost_add(Util::max(min,lb),Util::min(max,ub));
      } else if (min <= max) {
        Interval mm(min,max);
        Interval lbmax = Interval(lb,mod_).intersect(mm);
        Interval minub = Interval(0,ub).intersect(mm);
        noneset.almost_add(lbmax.low,lbmax.up);
        noneset.almost_add(minub.low,minub.up);
      } else if (lb <= ub) {
        Interval lbub(lb,ub);
        Interval lbmax = Interval(0,max).intersect(lbub);
        Interval minub = Interval(min,mod_).intersect(lbub);
        noneset.almost_add(lbmax.low,lbmax.up);
        noneset.almost_add(minub.low,minub.up);
      } else {
        noneset.almost_add(Util::max(min,lb),mod_);
        noneset.almost_add(0,Util::min(max,ub));
      }
    }
  }
}

void KinematicRealBands::none_bands(IntervalSet& noneset, Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  std::vector<Integerval> bands_int = std::vector<Integerval>();
  if (instantaneous_bands()) {
    instantaneous_bands_combine(bands_int,conflict_det,recovery_det,B,T,0,B,
        maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv);
  } else {
    kinematic_bands_combine(bands_int,conflict_det,recovery_det,time_step(ownship),B,T,0,B,
        maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv);
  }
  toIntervalSet(noneset,bands_int,get_step(),own_val(ownship),min_val(ownship),max_val(ownship));
}

bool KinematicRealBands::any_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  return instantaneous_bands() ?
      any_instantaneous_red(conflict_det,recovery_det,B,T,0,B,
          maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0):
          any_int_red(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
              maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0);
}

bool KinematicRealBands::all_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  return instantaneous_bands() ?
      all_instantaneous_red(conflict_det,recovery_det,B,T,0,B,
          maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0):
          all_int_red(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
              maxdown(ownship),maxup(ownship),ownship,traffic,repac,epsh,epsv,0);
}

bool KinematicRealBands::all_green(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  return !any_red(conflict_det,recovery_det,repac,epsh,epsv,B,T,ownship,traffic);
}

bool KinematicRealBands::any_green(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic) {
  return !all_red(conflict_det,recovery_det,repac,epsh,epsv,B,T,ownship,traffic);
}

/**
 * This function returns a resolution maneuver that is valid from B to T.
 * It returns NaN if there is no conflict and +/- infinity, depending on dir, if there
 * are no resolutions.
 * The value dir=false is down and dir=true is up.
 */
double KinematicRealBands::resolution(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
    int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic, bool dir) {
  int maxn;
  int sign;
  if (dir) {
    maxn = maxup(ownship);
    sign = 1;
  } else {
    maxn = maxdown(ownship);
    sign = -1;
  }
  int ires = first_green(conflict_det,recovery_det,time_step(ownship),B,T,0,B,
      dir,maxn,ownship,traffic,repac,epsh,epsv);
  if (ires == 0) {
    return NaN;
  } else if (ires < 0) {
    return sign*PINFINITY;
  } else {
    return mod_val(own_val(ownship)+sign*ires*get_step());
  }
}

void KinematicRealBands::compute_none_bands(IntervalSet& noneset, KinematicBandsCore& core, int alert_level, const TrafficState& repac) {
  Detection3D* detector = core.parameters.alertor.getLevel(alert_level).getDetectorRef();
  none_bands(noneset,detector,NULL,repac,
      core.epsilonH(),core.epsilonV(),0,core.parameters.alertor.getLevel(alert_level).getAlertingTime(),
      core.ownship,peripheral_acs_[alert_level-1]);
  IntervalSet noneset2 = IntervalSet();
  none_bands(noneset2,detector,NULL,repac,
      core.epsilonH(),core.epsilonV(),0,core.parameters.alertor.getLevel(alert_level).getEarlyAlertingTime(),
      core.ownship,core.conflictAircraft(alert_level));
  noneset.almost_intersect(noneset2);
}

std::string KinematicRealBands::toString() const {
  std::string s = "";
  s+="outdated_ = "+Fmb(outdated_)+"\n";
  s+="checked_ = "+Fmi(checked_)+"\n";
  for (TrafficState::nat i=0; i < peripheral_acs_.size(); ++i) {
    s+="peripheral_acs_["+Fmi(i)+"]: "+
        TrafficState::listToString(peripheral_acs_[i])+"\n";
  }
  s+="step_ = "+FmPrecision(step_)+"\n";
  s+="min_ = "+FmPrecision(min_)+"\n";
  s+="max_ = "+FmPrecision(max_)+"\n";
  s+="mod_ = "+FmPrecision(mod_)+"\n";
  s+="rel_ = "+Fmb(rel_)+"\n";
  s+="circular_ = "+Fmb(circular_)+"\n";
  for (BandsRange::nat i = 0; i < ranges_.size(); ++i) {
    s+="ranges_["+Fmi(i)+"] = ";
    s+=ranges_[i].toString()+"\n";
  }
  for (Interval::nat i=0; i < resolutions_.size(); ++i) {
    s+="resolutions_["+Fmi(i)+"] = "+resolutions_[i].toString()+"\n";
  }
  s+="recovery_time_: "+FmPrecision(recovery_time_)+ " [s]";
  return s;
}

std::string KinematicRealBands::toPVS(int prec) const {
  std::string s = "((:";
  for (int i = 0; i < (int) ranges_.size(); ++i) {
    if (i > 0) {
      s+=", ";
    } else {
      s+=" ";
    }
    s+=ranges_[i].interval.toPVS(prec);
  }
  s+=" :), (:";
  for (int i = 0; i < (int) ranges_.size(); ++i) {
    if (i > 0) {
      s+=", ";
    } else {
      s+=" ";
    }
    s+= BandsRegion::to_string(ranges_[i].region);
  }
  s+=" :), "+FmPrecision(recovery_time_,prec)+"::ereal)";
  return s;
}

}

/*
 * Copyright (c) 2018-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSREALBANDS_H_
#define DAIDALUSREALBANDS_H_

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
#include "BandsMofN.h"
#include "BandsHysteresis.h"

#include <vector>
#include <string>

#include "ColorValue.h"
#include "TrafficState.h"
//#include "DaidalusCore.h"

namespace larcfm {

class DaidalusRealBands : public DaidalusIntegerBands {

private:

  /*** PRIVATE VARIABLES */

  double mod_;  // If mod_ > 0, bands are circular modulo this value
  double step_; // Value step
  double min_param_; // Min value in parameters
  double max_param_; // Max value in parameters
  // min_rel (max_rel) is the positive distance from current value to minimum (maximum) value.
  // When mod_ > 0, min_rel, max_rel in [0,mod_/2]
  double min_rel_param_; // Relative min value in parameters. A negative value represents val-min
  double max_rel_param_; // Relative max value in parameters. A negative value represents max-val

  /* Parameters for recovery bands */
  bool recovery_;

  /**** CACHED VARIABLES ****/

  bool outdated_; // bool to control re-computation of cached values
  int checked_;  // Cached status of input values. Negative unchecked, 0 invalid, 1 valid

  /* Cached lists of aircraft indices, alert_levels, and lookahead times, sorted by indices, contributing to peripheral
   * bands listed per conflict bands, where 0th:NEAR, 1th:MID, 2th:FAR */
  std::vector<std::vector<IndexLevelT> > acs_peripheral_bands_;

  /* Cached lists of aircraft indices, alert_levels, and lookahead times, sorted by indices, contributing to any type
   * of bands listed per conflict bands, where 0th:NEAR, 1th:MID, 2th:FAR.
   * These lists are computed as the concatenation of acs_conflict_bands and acs_peripheral_bands. */
  std::vector<std::vector<IndexLevelT> > acs_bands_;

  std::vector<BandsRange> ranges_;     // Cached list of bands ranges

  /*
   * recovery_time_ is the time to recovery from violation.
   * Negative infinity means no possible recovery.
   * NaN means no recovery bands are computed (either because there is no conflict or
   * because they are disabled)
   */
  double recovery_time_;   // Cached recovery time
  int recovery_nfactor_; // Number of times the recovery volume was reduced
  /*
   * recovery_horizontal_distance and recovery_vertical_distance is the
   * distance guaranteed by the recovery bands. Negative infinity means no possible recovery.
   * NaN means no recovery bands are computed (either because there is no conflict of
   * because they are disabled)
   */
  double recovery_horizontal_distance_; // Cached recovery horizontal_separaton
  double recovery_vertical_distance_; // Cached recovery horizontal_separaton

  double min_val_; // Absolute min value (min_val= min when mod == 0 && !rel)
  double max_val_; // Absolute max value (max_val = max when mod == 0 && !rel)
  double min_relative_; // Computed relative min value
  double max_relative_; // Computed relative max value
  bool   circular_; // True if bands is fully circular

  /**** HYSTERESIS VARIABLES ****/

  BandsHysteresis bands_hysteresis_;

protected:
  double get_min_val_() const;
  double getmax_val_() const;

public:
  DaidalusRealBands(double mod=0);

  DaidalusRealBands(const DaidalusRealBands& b);

  virtual void setDaidalusParameters(const DaidalusParameters& parameters) = 0;

  virtual double own_val(const TrafficState& ownship) const = 0;

  virtual double time_step(const TrafficState& ownship) const = 0;

  virtual bool instantaneous_bands() const = 0;

  virtual double max_delta_resolution(const DaidalusParameters& parameters) const = 0;

  double get_mod() const;

  double get_step() const;

  bool get_recovery() const;

  // Set min and max when mod_ > 0. Requires min_val and max_val to be in range [0,mod)
  void set_minmax_mod(double min, double max);

  // Set min when mod == 0.
  void set_min_nomod(double min);

  // Set max when mod == 0.
  void setmax_nomod(double max);

  /**
   * Set min_rel. When mod_ > 0, requires min_rel to be in [0,mod/2]. Otherwise, a
   * negative value represents min.
   */
  void set_min_rel(double min_rel);

  /**
   * Set min_rel. When mod_ > 0, requires max_rel to be in [0,mod/2]. Otherwise, a
   * negative value represents max.
   */
  void setmax_rel(double max_rel);

  void set_step(double val);

  void set_recovery(bool flag);

private:
  bool check_input(const TrafficState& ownship);

public:
  bool kinematic_conflict(const TrafficState& ownship, const TrafficState& traffic,
      Detection3D* detector, int epsh, int epsv, double alerting_time);

  int length(DaidalusCore& core);

  const Interval& interval(DaidalusCore& core, int i);

  BandsRegion::Region region(DaidalusCore& core, int i);

  /**
   * Return index in ranges_ where val is found, -1 if invalid input or not found
   */
  int indexOf(DaidalusCore& core, double val);

  /**
   * Set cached values to stale conditions as they are no longer fresh
   */
  void stale(bool hysteresis);

  /**
   * Returns true is object is fresh
   */
  bool isFresh() const;

  /**
   * Refresh cached values
   */
  void refresh(DaidalusCore& core);

  /**
   *  Force computation of kinematic bands
   */
  void force_compute(DaidalusCore& core);

private:

  /**
   * Requires 0 <= conflict_region < CONFICT_BANDS and acs_peripheral_bands_ is empty
   * Put in acs_peripheral_bands_ the list of aircraft predicted to have a peripheral band for the given region.
   */
  void peripheral_aircraft(DaidalusCore& core, int conflict_region);

public:
  /**
   * Requires 0 <= conflict_region < CONFICT_BANDS
   * @return sorted list of aircraft indices and alert_levels contributing to peripheral bands
   * for given conflict region.
   * INTERNAL USE ONLY
   */
  const std::vector<IndexLevelT>& acs_peripheral_bands(DaidalusCore& core, int conflict_region);

  /**
   * Return recovery information.
   */
  RecoveryInformation recoveryInformation(DaidalusCore& core);

  /**
   * Return list of bands ranges
   */
  const std::vector<BandsRange>& ranges(DaidalusCore& core);

private:
  /**
   * Compute list of color values in lcvs from sets of none bands
   * Ensure that the intervals are "complete", filling in missing intervals and ensuring the
   * bands end at the proper bounds.
   */
  void color_values(std::vector<ColorValue>& lcvs, const std::vector<IntervalSet>& none_sets, DaidalusCore& core, bool recovery, int last_region);

  /**
   * Create an IntervalSet that represents a satured NONE band
   */
  void saturateNoneIntervalSet(IntervalSet& noneset) const;

  /**
   * Compute none bands for a const std::vector<IndexLevelT>& ilts of IndexLevelT in none_set_region.
   * The none_set_region is initiated as a saturated green band.
   * Uses aircraft detector if parameter detector is none.
   * The epsilon parameters for coordinations are handled according to the recovery_case flag.
   */
  void compute_none_bands(IntervalSet& none_set_region, const std::vector<IndexLevelT>& ilts,
      Detection3D* det, Detection3D* recovery,
      bool recovery_case, double B, DaidalusCore& core);

  /**
   * Compute recovery bands. Class variables recovery_time_, recovery_horizontal_distance_,
   * and recovery_vertical_distance_ are set.
   * Return true if non-saturated recovery bands where computed
   */
  bool compute_recovery_bands(IntervalSet& none_set_region, const std::vector<IndexLevelT>& ilts,
      DaidalusCore& core);

  // Return index of corrective region: 0: NEAR, 1: MID, 2: FAR
  static int corrective_region_index(const DaidalusCore& core);

  /**
   * Requires: compute_bands(conflict_region) = true && 0 <= conflict_region < CONFLICT_BANDS
   * Compute bands for one region. Return true iff recovery bands were computed.
   */
  bool compute_region(std::vector<IntervalSet>& none_sets, int conflict_region, DaidalusCore& core);

  /**
   * Compute all bands.
   */
  void compute(DaidalusCore& core);

public:
  /**
   * stale hysteresis depending on current_time and hysteresis_time
   */
  void reset_hysteresis(double current_time, double hysteresis_time);

  /**
   * Returns resolution maneuver.
   * Return NaN if there is no conflict or if input is invalid.
   * Return positive/negative infinity if there is no resolution to the
   * right/up and negative infinity if there is no resolution to the left/down.
   */
  double resolution(DaidalusCore& core, bool dir);

  /**
   * Compute preferred direction based on resolution that is closer
   * to current value.
   */
  bool preferred_direction(DaidalusCore& core);

  /**
   * Return last time to maneuver, in seconds, for ownship with respect to traffic
   * aircraft at ac_idx for conflict alert level. Return NaN if the ownship is not in conflict with aircraft within
   * early alerting time. Return negative infinity if there is no time to maneuver.
   * Note: 1 <= alert_level <= alerter.size()
   */
  double last_time_to_maneuver(DaidalusCore& core, const TrafficState& intruder);

private:
  int maxdown(const TrafficState& ownship) const;

  int maxup(const TrafficState& ownship) const;

  /** Add (lb,ub) to noneset. In the case of mod_ > 0, lb can be greater than ub. This function takes
   * care of the mod logic. This function doesn't do anything when lb and ub are almost equals.
   * @param noneset: Interval set where (lb,ub) will be added
   * @param lb: lower bound
   * @param ub: upper bound
   * When mod_ = 0, lb <= ub.
   */
  void add_noneset(IntervalSet& noneset, double lb, double ub) const;

public:
  /**
   *  This function scales the interval, add a constant, and constraint the intervals to min and max.
   *  The function takes care of modulo logic, in the case of circular bands.
   */
  void toIntervalSet(IntervalSet& noneset, const std::vector<Integerval>& l, double scal, double add) const;

  /**
   * The output parameter noneset has a list of non-conflict ranges orderd within [min,max]
   * values (or [0,mod] in the case of circular bands, i.e., when mod == 0).
   */
  virtual void none_bands(IntervalSet& noneset, const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic);

  virtual bool any_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic);

  virtual bool all_red(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic);

  bool all_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic);

  bool any_green(const Detection3D* conflict_det, const Detection3D* recovery_det,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic);

  /**
   * This function returns a resolution maneuver that is valid from B to T.
   * It returns NaN if there is no conflict and +/- infinity, depending on dir, if there
   * are no resolutions.
   * The value dir=false is down and dir=true is up.
   */
  virtual double resolution(const Detection3D* conflict_det, const Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const TrafficState& traffic,
      bool dir);

  std::string rawString() const;

  std::string toString() const;

  std::string toPVS() const;

};

}

#endif

/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICREALBANDS_H_
#define KINEMATICREALBANDS_H_

#include "Velocity.h"
#include "Position.h"
#include "Detection3D.h"
#include "TrafficState.h"
#include "Integerval.h"
#include "IntervalSet.h"
#include "BandsRange.h"
#include "KinematicIntegerBands.h"

#include <vector>
#include <string>
#include "KinematicBandsCore.h"

namespace larcfm {

class KinematicRealBands : public KinematicIntegerBands {

private:
  static const INT64FM ALMOST_;

  bool outdated_; // Boolean to control re-computation of cached values
  int checked_;  // Cached status of input values. Negative unchecked, 0 unvalid, 1 valid
  std::vector< std::vector<TrafficState> > peripheral_acs_; //  Cached list of peripheral aircraft per alert level
  std::vector<BandsRange> ranges_;     // Cached list of bands ranges
  /*
   * recovery_time_ is the time to recovery from violation.
   * Negative infinity means no possible recovery.
   * NaN means no recovery bands are computed (either because there is no conflict or
   * because they are disabled)
   */
  double recovery_time_; // Cached recovery time
  /*
   * resolutions_ is a list of resolution intervals per alert level
   * Negative/positive infinity means no possible resolutions
   * NaN means no resolutions are computed (either because there is no conflict or
   * because of invalid inputs)
   */
  std::vector<Interval> resolutions_; // Chached resolutions per alert level

  /* Parameters for conflict bands */
  double  min_;  // Minimum/donw value
  double  max_;  // Maximum/up value
  bool rel_;  // Determines if (min_,max_) are either relative, when rel_ is true,
  // or absolute values, when rel is false, with respect to current value. In the former case,
  // it is expected that min <= 0, and max >= 0. Otherwise, it is expected that
  // min <= current value <= max.
  double  mod_;  // If mod_ > 0, bands are circular modulo this value
  bool circular_; // True if bands is fully circular
  double step_; // Value step

  /* Parameters for recovery bands */
  bool recovery_; // Do compute recovery bands

public:

  KinematicRealBands();

  KinematicRealBands(double min, double max, bool rel, double mod, double step, bool recovery);

  KinematicRealBands(double min, double max, double step, bool recovery);

  KinematicRealBands(const KinematicRealBands&  b);

  virtual ~KinematicRealBands() { }

  virtual bool instantaneous_bands() const = 0;

  virtual double own_val(const TrafficState& ownship) const = 0;

  virtual double time_step(const TrafficState& ownship) const = 0;

  double get_min() const;

  double get_max() const;

  bool get_rel() const;

  double get_mod() const;

  double get_step() const;

  bool get_recovery() const;

  void set_min(double val);

  void set_max(double val);

  // As a side effect this method resets the min_/max_ values.
  void set_rel(bool val);

  void set_mod(double val);

  void set_step(double val);

  void set_recovery(bool flag);

	/** 
	 * When mod_ == 0, min_val <= max_val. When mod_ > 0, min_val is a value is in [0,mod_]. 
	 * In this case, it is not always true that min_val <= max_val
	 */
  double min_val(const TrafficState& ownship) const;

  /*
   * Return a positive number in [0,mod_/2]
   */
	/** 
	 * Positive distance from current value to minimum value. When mod_ > 0, min_rel is a value in [0,mod_/2]
	 */
  double min_rel(const TrafficState& ownship) const;

  /*
   * Returned value is in [0,mod_]. When mod_ == 0, min_val() <= max_val().
   * When mod_ > 0, it is not always true that min_val() <= max_val()
   */
	/** 
	 * When mod_ == 0, min_val <= max_val. When mod_ > 0, max_val is a value in [0,mod_]. 
	 * In this case, it is not always true that min_val <= max_val
	 */
  double max_val(const TrafficState& ownship) const;

	/**
	 * Positive distance from current value to maximum value. When mod_ > 0, max_rel is a value in [0,mod_/2]
	 */
  double max_rel(const TrafficState& ownship) const;

  bool check_input(const KinematicBandsCore& core);

  bool kinematic_conflict(KinematicBandsCore& core, const TrafficState& ac,
      Detection3D* detector, double alerting_time);

  int length(KinematicBandsCore& core);

  Interval interval( KinematicBandsCore& core, int i);

  BandsRegion::Region region(KinematicBandsCore& core, int i);

  /**
   * Return index where val is found, -1 if invalid input, >= length if not found
   */
  int rangeOf(KinematicBandsCore& core, double val);

  /**
   *  Reset cached values
   */
  void reset();

  /**
   *  Force computation of kinematic bands
   */
  void force_compute(KinematicBandsCore& core);

  /**
   * Return list of peripheral aircraft for a given alert level.
   * Requires: 0 <= alert_level <= alertor.size(). If alert_level is 0,
   * conflict_level is used.
   */
  std::vector<TrafficState> const & peripheralAircraft(KinematicBandsCore& core, int alert_level);

	/**
	 * Return time to recovery. Return NaN if bands are not saturated and negative infinity 
	 * when bands are saturated but no recovery within early alerting time.
	 */
  double timeToRecovery(KinematicBandsCore& core);

  /**
   * Return list of bands ranges
   */
  std::vector<BandsRange> const & ranges(KinematicBandsCore& core);

  /**
   * Returns resolution maneuver for given alert level and direction.
   * Return NaN if there is no conflict or if input is invalid.
   * Return positive/negative infinity if there is no resolution to the
   * right/up and negative infinity if there is no resolution to the left/down.
   * Requires: 0 <= alert_level <= alertor.size(). If alert_level is 0,
   * conflict_level is used.
   * Region of alert_level should be a conflict band type, e.g., NEAR, MID, or FAR.
   */
  double compute_resolution(KinematicBandsCore& core, int alert_level, bool dir);

	/**
	 * Compute preferred direction, for given alert level, based on resolution that is closer
	 * to current value.
	 */
  bool preferred_direction(KinematicBandsCore& core, int alert_level);

	/**
	 * Return last time to maneuver, in seconds, for ownship with respect to traffic
	 * aircraft ac for conflict alert level. Return NaN if the ownship is not in conflict with aircraft ac within 
	 * early alerting time. Return negative infinity if there is no time to maneuver.
	 * Note: 1 <= alert_level <= alertor.size()
	 */
  double last_time_to_maneuver(KinematicBandsCore& core, const TrafficState& ac);

  /**
   *  This function scales the interval, add a constant, and constraint the intervals to min and max.
   *  The function takes care of modulo logic, in the case of circular bands.
   */
  void toIntervalSet(IntervalSet& noneset, const std::vector<Integerval>& l,
      double scal, double add, double min, double max);

  virtual void none_bands(IntervalSet& noneset, Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  virtual bool any_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  virtual bool all_red(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  bool all_green(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  bool any_green(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic);

  /**
   * This function returns a resolution maneuver that is valid from B to T.
   * It returns NaN if there is no conflict and +/- infinity, depending on dir, if there
   * are no resolutions.
   * The value dir=false is down and dir=true is up.
   */
  virtual double resolution(Detection3D* conflict_det, Detection3D* recovery_det, const TrafficState& repac,
      int epsh, int epsv, double B, double T, const TrafficState& ownship, const std::vector<TrafficState>& traffic, bool dir);

  std::string toString() const;

  std::string toPVS(int prec) const;

private:

  /*
   *  Returns true if the first interval extends to the last interval. This happens
   *  when mod_ > 0, the low value is 0, and the max value is mod_.
   */
  bool rollover();

  /**
   * Return val modulo mod_, when mod_ > 0. Otherwise, returns val.
   */
  double mod_val(double val) const;

  /**
   *  Update cached values
   */
  void update(KinematicBandsCore& core);

  /**
   * Put in peripheral_acs_ the list of aircraft predicted to be in conflict for the given alert level
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  void peripheral_aircraft(KinematicBandsCore& core, int alert_level);

	/** 
	 * Ensure that the intervals are "complete", filling in missing intervals and ensuring the 
	 * bands end at the  proper bounds. 
	 * Requires none_sets to be a non-empty list and size(none_sets) == size(regions)
	 */
  void color_bands(const std::vector<IntervalSet> & none_sets, const std::vector<BandsRegion::Region>& regions,
      KinematicBandsCore & core, bool recovery);

  /**
   * Compute recovery bands. Returns recovery time.
   */
  double compute_recovery_bands(IntervalSet& noneset, KinematicBandsCore& core, const std::vector<TrafficState>& alerting_set);

  /**
   * Compute bands for one level. Return recovery time (NaN if recover bands are not computed)
   */
  double compute_level(IntervalSet& noneset, KinematicBandsCore& core, int alert_level);

  /**
   * Compute all bands.
   */
  void compute(KinematicBandsCore& core);

  Interval find_resolution(KinematicBandsCore& core, const IntervalSet& noneset);

  int maxdown(const TrafficState& ownship) const;

  int maxup(const TrafficState& ownship) const;

  void compute_none_bands(IntervalSet& noneset, KinematicBandsCore& core, int alert_level, const TrafficState& repac);

};

}

#endif

/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef DAIDALUSBANDSCORE_H_
#define DAIDALUSBANDSCORE_H_

#include "CriteriaCore.h"
#include "UrgencyStrategy.h"
#include "IndexLevelT.h"
#include "Interval.h"
#include "TCASTable.h"
#include "Alerter.h"
#include "Constants.h"
#include "NoneUrgencyStrategy.h"
#include "TrafficState.h"
#include "DaidalusParameters.h"
#include <map>
#include <vector>
#include <string>
#include <cmath>

#include "HysteresisData.h"

namespace larcfm {

class DaidalusCore {

public:
  /* Absolute ownship state */
  TrafficState ownship;
  /* Absolute list of traffic states */
  std::vector<TrafficState> traffic;
  /* Kinematic bands parameters */
  /* Current time */
  double current_time;
  /* Wind vector in TO direction */
  Velocity wind_vector;
  DaidalusParameters parameters;

  /* Strategy for most urgent aircraft */
  const UrgencyStrategy* get_urgency_strategy() const;
  bool set_urgency_strategy(const UrgencyStrategy* strat);

private:

  /* Strategy for most urgent aircraft */
  const UrgencyStrategy* urgency_strategy_;

  /**** CACHED VARIABLES ****/

  /* Variable to control re-computation of cached values */
  int cache_; // -1: outdated, 1:updated, 0: updated only most_urgent_ac and eps values
  /* Most urgent aircraft */
  TrafficState most_urgent_ac_;
  /* Cached horizontal epsilon for implicit coordination */
  int epsh_;
  /* Cached vertical epsilon for implicit coordination */
  int epsv_;
  /* Cached value of DTA status given current aircraft states.
   *  0 : Not in DTA
   * -1 : In DTA, but special bands are not enabled yet
   *  1 : In DTA and special bands are enabled
   */
  int dta_status_;
  /* Cached lists of aircraft indices, alert_levels, and lookahead times sorted by indices, contributing to conflict (non-peripheral)
   * band listed per conflict bands, where 0th:NEAR, 1th:MID, 2th:FAR */
  std::vector<std::vector<IndexLevelT> > acs_conflict_bands_;
  /* Cached list of time to violation per conflict bands, where 0th:NEAR, 1th:MID, 2th:FAR */
  Interval tiov_[BandsRegion::NUMBER_OF_CONFLICT_BANDS];
  /* Cached list of bool alues indicating which bands should be computed, where 0th:NEAR, 1th:MID, 2th:FAR.
   * NaN means that bands are not computed for that region*/
  bool bands4region_[BandsRegion::NUMBER_OF_CONFLICT_BANDS];

  /**** HYSTERESIS VARIABLES ****/

  // Alerting and DTA hysteresis per aircraft's ids
  std::map<std::string,HysteresisData> alerting_hysteresis_acs_;
  std::map<std::string,HysteresisData> dta_hysteresis_acs_;

  void copyFrom(const DaidalusCore& core);
  void refresh_mua_eps();

public:
  DaidalusCore();
  virtual ~DaidalusCore() {};
  DaidalusCore& operator=(const DaidalusCore& core);

  DaidalusCore(const DaidalusCore& core);

  DaidalusCore(const Alerter& alerter);

  DaidalusCore(const Detection3D* det, double T);

  /**
   *  Clear ownship and traffic data from this object.
   */
  void clear();

  /**
   *  Clear wind vector from this object.
   */
  void clear_wind();

  bool set_alerter_ownship(int alerter_idx);

  bool set_alerter_traffic(int idx, int alerter_idx);

  /**
   *  Clear hysteresis information from this object.
   */
  void clear_hysteresis();

  /**
   * Set cached values to stale conditions as they are no longer fresh.
   */
  void stale();

  /**
   * Returns true is object is fresh
   */
  bool isFresh() const;

  /**
   *  Refresh cached values
   */
  void refresh();

  /**
   * Returns DTA status:
   *  0 : DTA is not active
   * -1 : DTA is active, but special bands are not enabled yet
   *  1 : DTA is active and special bands are enabled
   */
  int DTAStatus();

  /**
   * @return most urgent aircraft for implicit coordination
   */
  const TrafficState& mostUrgentAircraft();

  /**
   * Returns horizontal epsilon for implicit coordination with respect to criteria ac.
   *
   */
  int epsilonH();

  /**
   * Returns vertical epsilon for implicit coordination with respect to criteria ac.
   */
  int epsilonV() ;

  /**
   * Returns horizontal epsilon for implicit coordination with respect to criteria ac.
   *
   */
  int epsilonH(bool recovery_case, const TrafficState& traffic);

  /**
   * Returns vertical epsilon for implicit coordination with respect to criteria ac.
   */
  int epsilonV(bool recovery_case, const TrafficState& traffic);

  /**
   * Return true if bands are computed for this particular region (0:NEAR, 1:MID, 2: FAR)
   */
  bool bands_for(int region);

  /**
   * Returns actual minimum horizontal separation for recovery bands in internal units.
   */
  double minHorizontalRecovery() const;

  /**
   * Returns actual minimum vertical separation for recovery bands in internal units.
   */
  double minVerticalRecovery() const;

  void set_ownship_state(const std::string& id, const Position& pos, const Velocity& vel, double time);

  // Return 0-based index in traffic list (-1 if aircraft doesn't exist)
  int find_traffic_state(const std::string& id) const;

  // Return 0-based index in traffic list where aircraft was added. Return -1 if
  // nothing is done (e.g., id is the same as ownship's)
  int set_traffic_state(const std::string& id, const Position& pos, const Velocity& vel, double time);

  void reset_ownship(int idx);

  // idx is 0-based index in traffic list
  bool remove_traffic(int idx);

  void set_wind_velocity(const Velocity& wind);

  bool linear_projection(double offset);

  bool has_ownship() const;

  bool has_traffic() const;

  /* idx is a 0-based index in the list of traffic aircraft
   * returns 1 if detector of traffic aircraft
   * returns 2 if corrective alerter level is not set
   * returns 3 if alerter of traffic aircraft is out of bands
   * otherwise, if there are no errors, returns 0 and the answer is in blobs
   */
  int horizontal_contours(std::vector<std::vector<Position> >& blobs, int idx, int alert_level);

  /* idx is a 0-based index in the list of traffic aircraft
   * returns 1 if detector of traffic aircraft
   * returns 2 if corrective alerter level is not set
   * returns 3 if alerter of traffic aircraft is out of bands
   * otherwise, if there are no errors, returns 0 and the answer is in blobs
   */
  int horizontal_hazard_zone(std::vector<Position>& haz, int idx, int alert_level,
      bool loss, bool from_ownship);

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
  int alert_level(int idx, int turning, int accelerating, int climbing);

private:

  int dta_hysteresis_current_value(const TrafficState& ac);

  int alerting_hysteresis_current_value(const TrafficState& intruder, int turning, int accelerating, int climbing);

  bool greater_than_corrective() const;

  int raw_alert_level(const Alerter& alerter, const TrafficState& intruder, int turning, int accelerating, int climbing);

  /**
   * Return true if and only if threshold values, defining an alerting level, are violated.
   */
  bool check_alerting_thresholds(const Alerter& alerter, int alert_level, const TrafficState& intruder, int turning, int accelerating, int climbing);

  /**
   * Requires 0 <= conflict_region < CONFICT_BANDS
   * Put in acs_conflict_bands_ the list of aircraft predicted to be in conflict for the given region.
   * Put compute_bands_ a flag indicating if bands for given region are computed for some aircraft
   * Put in tiov_ the time interval of violation for given region
   */
  void conflict_aircraft(int conflict_region);

public:
  /**
   * Requires 0 <= conflict_region < CONFICT_BANDS
   * @return sorted list of aircraft indices and alert_levels contributing to conflict (non-peripheral)
   * bands for given conflict region.
   * INTERNAL USE ONLY
   */
  const std::vector<IndexLevelT>& acs_conflict_bands(int conflict_region);

  /**
   * Requires 0 <= conflict_region < CONFICT_BANDS
   * @return Return time interval of conflict for given conflict region
   * INTERNAL USE ONLY
   */
  const Interval& tiov(int conflict_region);

  /**
   * Return alert index used for intruder aircraft.
   * The alert index depends on alerting logic and DTA logic.
   * If ownship centric, it returns the alert index of ownship.
   * Otherwise, returns the alert index of the intruder.
   * If the DTA logic is enabled, the alerter of an aircraft is determined by
   * its dta status.
   */
  int alerter_index_of(const TrafficState& intruder);

  static int epsilonH(const TrafficState& ownship, const TrafficState& ac);

  static int epsilonV(const TrafficState& ownship, const TrafficState& ac);

  TrafficState criteria_ac();

  TrafficState recovery_ac();

  std::string outputStringAircraftStates(bool internal) const;

  std::string rawString() const;

  std::string toString() const;
};

}

#endif

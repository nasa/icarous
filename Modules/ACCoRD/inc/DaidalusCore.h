/*
 * Copyright (c) 2015-2018 United States Government as represented by
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
#include "AlertingMofN.h"
#include "ParameterData.h"
#include "TrafficState.h"
#include <map>
#include <vector>
#include <string>
#include <cmath>

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
  const UrgencyStrategy* urgency_strategy; // Strategy for most urgent aircraft

  /* Cached values */
private:
  /**** CACHED VARIABLES__ ****/

  /* Variable to control re-computation of cached values */
  int cache__; // -1: outdated, 1:updated, 0: updated only most_urgent_ac and eps
  /* Most urgent aircraft */
  TrafficState most_urgent_ac__;
  /* Cached horizontal epsilon for implicit coordination */
  int epsh__;
  /* Cached vertical epsilon for implicit coordination */
  int epsv__;
  /* Cached lists of aircraft indices, alert_levels, and lookahead times sorted by indices, contributing to conflict (non-peripheral)
   * band listed per conflict bands, where 0th:NEAR, 1th:MID, 2th:FAR */
  std::vector<std::vector<IndexLevelT> > acs_conflict_bands__;
  /* Cached list of time to violation per conflict bands, where 0th:NEAR, 1th:MID, 2th:FAR */
  Interval tiov__[BandsRegion::NUMBER_OF_CONFLICT_BANDS];
  /* Cached list of bool alues indicating which bands should be computed, where 0th:NEAR, 1th:MID, 2th:FAR.
   * NaN means that bands are not computed for that region*/
  bool bands4region__[BandsRegion::NUMBER_OF_CONFLICT_BANDS];

  /**** HYSTERESIS _VARIABLES_ ****/
  //Alerting MofNs and times per aircraft's ids
  std::map<std::string,AlertingMofN> _alerting_mofns_;

  void init();
  void copyFrom(const DaidalusCore& core);
  void stale(bool hysteresis, bool forced);
  void refresh_mua_eps();

public:
  DaidalusCore();
  virtual ~DaidalusCore() {};
  DaidalusCore& operator=(const DaidalusCore& core);

  DaidalusCore(const DaidalusCore& core);

  DaidalusCore(const Alerter& alerter);

  DaidalusCore(const Detection3D& det, double T);

  /**
   *  Clear ownship and traffic data from this object.
   */
  void clear();

  /**
   * Set cached values to stale conditions as they are no longer fresh
   */
  void stale(bool hysteresis);

  /**
   * Returns true is object is fresh
   */
  bool isFresh() const;

  /**
   *  Refresh cached values
   */
  void refresh();

  /**
   * @return most urgent aircraft for implicit coordination
   */
  TrafficState mostUrgentAircraft();

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

  int add_traffic_state(const std::string& id, const Position& pos, const Velocity& vel, double time);

  void reset_ownship(int idx);

  void set_wind_velocity(const Velocity& wind);

  bool has_ownship() const;

  bool has_traffic() const;

  /* idx is a 0-based index in the list of traffic aircraft
   * returns 1 if detector of traffic aircraft
   * returns 2 if corrective alerter level is not set
   * returns 3 if alerter of traffic aircraft is out of bands
   * otherwise, if there are no errors, returns 0 and the answer is in blobs
   */
  int horizontal_contours(std::vector<std::vector<Position> >& blobs, int idx, int alert_level);

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

  int alert_level(const Alerter& alerter, const TrafficState& intruder, int turning, int accelerating, int climbing);

  static void add_blob(std::vector<std::vector<Position> >& blobs, std::vector<Position>& vin, std::vector<Position>& vout);

  /**
   * Return true if and only if threshold values, defining an alerting level, are violated.
   */
  bool check_alerting_thresholds(const AlertThresholds& athr, const TrafficState& intruder, int turning, int accelerating, int climbing);

  /**
   * Requires 0 <= conflict_region < CONFICT_BANDS
   * Put in acs_conflict_bands_ the list of aircraft predicted to be in conflict for the given region.
   * Put compute_bands_ a flag indicating if bands for given region are computed for some aircraft
   * Put in tiov__ the time interval of violation for given region
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
   * Return alert index used for the traffic aircraft at 0 <= idx < traffic.size().
   * The alert index depends on alerting logic. If ownship centric, it returns the
   * alert index of ownship. Otherwise, returns the alert index of the traffic aircraft
   * at idx. Return 0 if idx is out of range
   */
  int alerter_index_of(int idx) const;

  /**
   * Return alerter used for the traffic aircraft at 0 <= idx < traffic.size().
   * The alert index depends on alerting logic. If ownship centric, it returns the
   * alerter of the ownship. Otherwise, returns the alerter of the traffic aircraft
   * at idx.
   */
  const Alerter& alerter_of(int ac) const;

  static int epsilonH(const TrafficState& ownship, const TrafficState& ac);

  static int epsilonV(const TrafficState& ownship, const TrafficState& ac);

  TrafficState criteria_ac();

  TrafficState recovery_ac();

  void setParameterData(const ParameterData& p);

  std::string rawString() const;

  std::string toString() const;
};

}

#endif

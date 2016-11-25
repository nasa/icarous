/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICBANDSCORE_H_
#define KINEMATICBANDSCORE_H_

#include "AlertLevels.h"
#include "Velocity.h"
#include "Position.h"
#include "TrafficState.h"
#include "Detection3D.h"
#include "Detection3DAcceptor.h"
#include "TCASTable.h"
#include "KinematicBandsParameters.h"
#include "Interval.h"
#include <vector>
#include <string>

namespace larcfm {

class KinematicBandsCore {

public:

  static TCASTable RA;

  /* Absolute ownship state */
  TrafficState ownship;
  /* Absolute list of traffic states */
  std::vector<TrafficState> traffic;
  /* Kinematic bands parameters */
  KinematicBandsParameters parameters;
  /* Most urgent aircraft */
  TrafficState most_urgent_ac;

private:

  /* Boolean to control re-computation of cached values */
  bool outdated_;
  /* Cached horizontal epsilon for implicit coordination */
  int epsh_;
  /* Cached vertical epsilon for implicit coordination */
  int epsv_;
  /* The length of conflict_acs_ is greater than or equal to the length of the alert levels. */
  /* Cached list of conflict aircraft per alert level */
  std::vector< std::vector<TrafficState> > conflict_acs_;
  /* Cached list of time intervals of violation per alert level */
  std::vector<Interval> tiov_; //
  /* Last conflict level */
  int last_conflict_level_;

  /**
   *  Update cached values
   */
  void update();

  /**
   * Put in conflict_acs_ the list of aircraft predicted to be in conflict for the given alert level.
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  void conflict_aircraft(int alert_level);

public:

  KinematicBandsCore(const KinematicBandsParameters& params);

  KinematicBandsCore(const KinematicBandsCore& core);

  ~KinematicBandsCore();

  // needed because of pointer
  KinematicBandsCore& operator=(const KinematicBandsCore& core);

  /**
   * Set kinematic bands core
   */
  void setKinematicBandsCore(const KinematicBandsCore core);

  /**
   *  Clear ownship and traffic data from this object.
   */
  void clear();

  /**
   *  Reset cached values
   */
  void reset();

  /**
   * Returns most severe alert level where there is a conflict aircraft
   */
  int lastConflictAlertLevel();

  /**
   *  Returns horizontal epsilon for implicit coordination with respect to criteria ac
   */
  int epsilonH();

  /**
   *  Returns vertical epsilon for implicit coordination with respect to criteria ac
   */
  int epsilonV();

  /**
   *  Return list of corrective aircraft
   */
  std::vector<TrafficState> const & correctiveAircraft();

  /**
   * Returns actual minimum horizontal separation for recovery bands in internal units. 
   */
  double minHorizontalRecovery() const;

  /** 
   * Returns actual minimum vertical separation for recovery bands in internal units. 
   */
  double minVerticalRecovery() const;

  bool hasOwnship() const;

  TrafficState intruder(const std::string& id) const;

  bool hasTraffic() const;

  Position const & trafficPosition(int i) const;

  Velocity const & trafficVelocity(int i) const;

  Vect3 const & own_s() const;

  Velocity const & own_v() const;

  Vect3 const & traffic_s(int i) const;

  Velocity const & traffic_v(int i) const;

  /**
   * Return list of conflict aircraft for a given alert level.
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  std::vector<TrafficState> const & conflictAircraft(int alert_level);

  /**
   * Return time interval of violation for given alert level
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  Interval const & timeIntervalOfViolation(int alert_level);

  static int epsilonH(const TrafficState& ownship, const TrafficState& ac);

  static int epsilonV(const TrafficState& ownship, const TrafficState& ac);

  TrafficState const & criteria_ac() const;

  TrafficState const & recovery_ac() const;

  std::string toString() const;

};

}

#endif

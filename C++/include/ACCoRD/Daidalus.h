/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DAIDALUS_H_
#define DAIDALUS_H_

#include "Detection3D.h"
#include "Vect3.h"
#include "Velocity.h"
#include "AlertThresholds.h"
#include "KinematicMultiBands.h"
#include "UrgencyStrategy.h"
#include "WCVTable.h"
#include "WCV_TAUMOD.h"
#include "KinematicBandsParameters.h"
#include <deque>

namespace larcfm {

class Daidalus : public ErrorReporter {

private:

  TrafficState ownship_; // Ownship aircraft. Velocity vector is wind-based.
  std::vector<TrafficState> traffic_; // Traffic aircraft states. Positions are synchronized in time with ownship. Velocity vector is wind-based.
  double current_time_; // Current time
  Velocity wind_vector_; // Wind information
  UrgencyStrategy* urgency_strat_; // Strategy for most urgent aircraft

  static void add_blob(std::vector< std::vector<Position> >& blobs,
      std::deque<Position>& vin, std::deque<Position>& vout);

  KinematicMultiBands kb_; // For internal computations of alerts

  /**
   * @return alerting time. If set to 0, returns lookahead time instead.
   */
  mutable ErrorLog error;

public:

  /**
   * Parameter values for Daidalus object
   */
  KinematicBandsParameters parameters;

  /**
   * Create a new Daidalus object such that
   * - Alerting thresholds are unbuffered as defined by SC-228 MOPS.
   * - Maneuver guidance logic assumes instantaneous maneuvers
   * - Bands saturate at DMOD/ZTHR
   */
  Daidalus();

  /**
   * Create a new Daidalus object and copy all configuration parameters and traffic information
   * from another Daidalus object.
   */
  Daidalus(const Daidalus& daa);

  ~Daidalus();

  // needed because of pointer
  Daidalus& operator=(const Daidalus& daa);

  /*
   * Set Daidalus object such that
   * - Alerting thresholds are unbuffered as defined by SC-228 MOPS.
   * - Maneuver guidance logic assumes instantaneous maneuvers
   * - Bands saturate at DMOD/ZTHR
   */
  void set_WC_SC_228_MOPS();

  /*
   * Set Daidalus object such that
   * - Alerting thresholds are buffered
   * - Maneuver guidance logic assumes kinematic maneuvers
   * - Turn rate is set to 3 deg/s, when type is true, and to  1.5 deg/s
   *   when type is false.
   * - Bands don't saturate until NMAC
   */
  void set_Buffered_WC_SC_228_MOPS(bool type);

  /**
   * Clear aircraft list, current time, and wind vector.
   */
  void reset();

  /**
   * @return number of aircraft, including ownship.
   */
  int numberOfAircraft() const;

  /**
   * @return last traffic index. Every traffic aircraft has an index between 1 and lastTrafficIndex.
   * The index 0 is reserved for the ownship. When lastTrafficIndex is 0, the ownship is set but no
   * traffic aircraft has been set. When lastTrafficIndex is negative, ownship has not been set.
   */
  int lastTrafficIndex() const;

  /**
   * Get wind vector
   */
  Velocity const & getWindField() const;

  /**
   * Set wind vector (common to all aircraft)
   */
  void setWindField(const Velocity& wind);

  /**
   * Clear all aircraft and set ownship state and current time.
   * Velocity vector is ground velocity.
   */
  void setOwnshipState(const std::string& id, const Position& pos, const Velocity& vel, double time);

  /**
   * Clear all aircraft and set ownship state and current time.
   */
  void setOwnshipState(const TrafficState& ownship, double time);

  /**
   * Add traffic state at given time. Velocity vector is ground velocity.
   * If time is different from current time, traffic state is projected, past or future,
   * into current time assuming wind information. If it's the first aircraft, this aircraft is
   * set as the ownship.
   * Return aircraft index.
   */
  int addTrafficState(const std::string& id, const Position& pos, const Velocity& vel, double time);

  /**
   * Add traffic state at current time. Velocity vector is ground velocity.
   * If it's the first aircraft, this aircraft is set as the ownship.
   * Return aircraft index.
   */
  int addTrafficState(const std::string& id, const Position& pos, const Velocity& vel);

  /**
   * Add traffic state at given time.
   * If time is different from current time, traffic state is projected, past or future,
   * into current time assuming wind information. If it's the first aircraft, this aircraft is
   * set as the ownship. Return aircraft index.
   */
  int addTrafficState(const TrafficState& ac, double time);

  /**
   * Add traffic state at current time.
   * If it's the first aircraft, this aircraft is set as the ownship.
   * Return aircraft index.
   */
  int addTrafficState(const TrafficState& ac);

  /**
   * Exchange ownship aircraft with aircraft at index ac_idx.
   */
  void resetOwnship(int ac_idx);

  /**
   * Exchange ownship aircraft with aircraft named id.
   */
  void resetOwnship(const std::string& id);

  /** 
   * Get index of aircraft with given name. Return -1 if no such index exists
   */
  int aircraftIndex(const std::string& name) const;

  /**
   * @return get current time, i.e., time of ownship aircraft.
   */
  double getCurrentTime() const;

  /**
   * Linearly projects all aircraft states to time t [s] and set current time to t.
   */
  void setCurrentTime(double time);

  /** 
   * Computes alerting type of ownship and aircraft at index ac_idx for current 
   * aircraft states.  The number 0 means no alert. A negative number means
   * that aircraft index is not valid.
   * When the alertor object has been configured to consider ownship maneuvers, i.e.,
   * using spread values, the alerting logic could also use information about the ownship
   * turning, accelerating, and climbing status as follows:
   * - turning < 0: ownship is turning left, turning > 0: ownship is turning right, turning = 0:
   * do not make any turning assumption about the ownship.
   * - accelerating < 0: ownship is decelerating, accelerating > 0: ownship is accelerating,
   * accelerating = 0: do not make any accelerating assumption about the ownship.
   * - climbing < 0: ownship is descending, climbing > 0: ownship is climbing, climbing = 0:
   * do not make any climbing assumption about the ownship.
   */
  int alerting(int ac_idx, int turning, int accelerating, int climbing);

  /**
   * Computes alerting type of ownship and aircraft at index ac_idx for current
   * aircraft states.  The number 0 means no alert. A negative number means
   * that aircraft index is not valid.
   */
  int alerting(int ac_idx);

  /**
   * Detects conflict with aircraft at index ac_idx for given alert level.
   * Conflict data provides time to violation and time to end of violation
   * within lookahead time.
   */
  ConflictData detection(int ac_idx, int alert_level) const;

  /**
   * Detects conflict with aircraft at index ac_idx for conflict alert level.
   * Conflict data provides time to violation and time to end of violation
   * within lookahead time.
   */
  ConflictData detection(int ac_idx) const;

  /**
   * @return time to violation, in seconds, between ownship and aircraft at index ac_idx, for the
   * lookahead time. The returned time is relative to current time. PINFINITY means no
   * conflict within lookahead time. NaN means that aircraft index is out of range.
   */
  double timeToViolation(int ac_idx) const;

  /**
   * Returns state of ownship.
   */
  TrafficState const & getOwnshipState() const;

  /**
   * Returns state of aircraft at index ac_idx
   */
  TrafficState const & getAircraftState(int ac_idx) const;

  /**
   * Compute in bands the kinematic multi bands at current time. Computation of bands is lazy,
   * they are only computed when needed.
   */
  void kinematicMultiBands(KinematicMultiBands& bands) const;

  /**
   * @return reference to strategy for computing most urgent aircraft.
   */
  UrgencyStrategy* getUrgencyStrategyRef() const;

  /**
   * Set strategy for computing most urgent aircraft.
   */
  void setUrgencyStrategy(const UrgencyStrategy* strat);

  /**
   * Returns most urgent aircraft for given alert level according to urgency strategy.
   */
  TrafficState mostUrgentAircraft(int alert_level) const;

  /**
   * Returns most urgent aircraft for conflict alert level according to urgency strategy.
   */
  TrafficState mostUrgentAircraft() const;

  /**
   * Computes horizontal contours contributed by aircraft at index ac_idx, for
   * given alert level. A contour is a non-empty list of points in counter-clockwise
   * direction representing a polygon.
   * @param blobs list of track contours returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   */
  void horizontalContours(std::vector< std::vector<Position> >& blobs, int ac_idx, int alert_level);

  /**
   * Computes horizontal contours contributed by aircraft at index ac_idx, for
   * conflict alert level. A contour is a non-empty list of points in counter-clockwise
   * direction representing a polygon.
   * @param blobs list of track contours returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   */
  void horizontalContours(std::vector< std::vector<Position> >& blobs, int ac_idx);

  std::string aircraftListToPVS(int prec) const;

  std::string outputStringAircraftStates() const;

  std::string toString() const;

  static std::string release();

  bool hasError() const {
    return error.hasError();
  }

  bool hasMessage() const {
    return error.hasMessage();
  }

  std::string getMessage() {
    return error.getMessage();
  }

  std::string getMessageNoClear() const {
    return error.getMessageNoClear();
  }

};
}
#endif

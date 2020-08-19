/*
 * Copyright (c) 2015-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

/**
 * Objects of class "Daidalus" compute the conflict bands using
 * kinematic single-maneuver projections of the ownship and linear preditions
 * of (multiple) traffic aircraft positions. The bands consist of ranges of
 * guidance maneuvers: direction angles, horizontal speeds, vertical
 * speeds, and altitude.<p>
 *
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver. If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR/MID/FAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will corresponding alert within the corresponding alerting level thresholds.<p>
 *
 * If recovery bands are set and the ownship is in
 * a violation path, loss of separation recovery bands and recovery times are
 * computed for each type of maneuver. If the ownship immediately executes a
 * RECOVERY guidance maneuver, then the new path is conflict-free after the
 * recovery time. Furthermore, the recovery time is the minimum time for which
 * there exists a kinematic conflict-free maneuver in the future. <p>
 *
 * Note that in the case of geodetic coordinates this version of bands
 * performs an internal projection of the coordinates and velocities
 * into the Euclidean frame (see Util/Projection).  Accuracy may be
 * reduced if the traffic plans involve any segments longer than
 * Util.Projection.projectionConflictRange(lat,acc), and an error will
 * be logged if the distance between traffic and ownship exceeds
 * Util.Projection.projectionMaxRange() at any point in the lookahead
 * range.<p>
 *
 * Disclaimers: The formal proofs of the core algorithms use real numbers,
 * however these implementations use floating point
 * numbers, so numerical differences could result. In addition, the
 * geodetic computations include certain inaccuracies, especially near
 * the poles.<p>
 *
 * The basic usage is
 * <pre>
 * Daidalus daa = new Daidalus();
 * daa.loadFromFile(<configurationfile>);
 *
 * ...
 * daa.setOwnshipState(position of ownship, velocity of ownship);
 * daa.addTrafficState(position of (one) traffic aircraft, velocity of traffic);
 * daa.addTrafficState(position of (another) traffic aircraft, velocity of traffic);
 * ...add other traffic aircraft...
 *
 * for (int i = 0; i < daa.horizontalDirectionBandsLength(); i++ ) {
 *    interval = daa.horizontalDirectionIntervalAt(i);
 *    lower_ang = intrval.low;
 *    upper_ang = intrval.up;
 *    regionType = daa.horizontalDirectionRegionAt(i);
 *    ..do something with this information..
 * }
 *
 * ...similar for horizontal speed and vertical speed...
 * </pre>
 *
 */

#ifndef DAIDALUSBANDS_H_
#define DAIDALUSBANDS_H_

#include "GenericStateBands.h"
#include "DaidalusCore.h"
#include "DaidalusAltBands.h"
#include "DaidalusDirBands.h"
#include "DaidalusHsBands.h"
#include "DaidalusVsBands.h"
#include "UrgencyStrategy.h"
#include "Velocity.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "TrafficState.h"
#include "BandsRegion.h"
#include "Alerter.h"
#include "Detection3D.h"
#include "IndexLevelT.h"
#include "string_util.h"
#include "format.h"
#include <vector>
#include <string>
#include <cmath>

namespace larcfm {

class Daidalus : public GenericStateBands, public ErrorReporter {

private:
  mutable ErrorLog  error;
  DaidalusCore      core_;
  DaidalusDirBands  hdir_band_;
  DaidalusHsBands   hs_band_;
  DaidalusVsBands   vs_band_;
  DaidalusAltBands  alt_band_;

  void stale_bands();

public:
  /* Constructors */

  /**
   * Construct an empty Daidalus object.
   * NOTE: This object doesn't have any alert configured. Alerters can be
   * configured either programmatically, set_DO_365A(true,true) or
   * via a configuration file with the method loadFromFile(configurationfile)
   **/
  Daidalus();

  /**
   * Construct a Daidalus object with initial alerter.
   */
  Daidalus(const Alerter& alerter);

  /**
   * Construct a Daidalus object with the default parameters and one alerter with the
   * given detector and T (in seconds) as the alerting time, early alerting time, and lookahead time.
   */
  Daidalus(const Detection3D* det, double T);

  /* Destructor */
  virtual ~Daidalus() {}

  /* Setting for WC Definitions RTCA DO-365 */

  /*
   * Set Daidalus object such that
   * - Configure two alerters (Phase I and Phase II) as defined as in RTCA DO-365A
   * - Maneuver guidance logic assumes kinematic maneuvers
   * - Turn rate is set to 3 deg/s, when type is true, and to  1.5 deg/s
   *   when type is false.
   * - Configure Sensor Uncertainty Migitation (SUM) when sum is true
   * - Bands don't saturate until NMAC
   */
  void set_DO_365A(bool type=true, bool sum=true);

  /*
   * Set DAIDALUS object such that
   * - Alerting thresholds are buffered
   * - Maneuver guidance logic assumes kinematic maneuvers
   * - Turn rate is set to 3 deg/s, when type is true, and to  1.5 deg/s
   *   when type is false.
   * - Bands don't saturate until NMAC
   */
  void set_Buffered_WC_DO_365(bool type);

  /* Set DAIDALUS object such that alerting logic and maneuver guidance corresponds to
   * ACCoRD's CD3D, i.e.,
   * - Separation is given by a cylinder of of diameter 5nm and height 1000ft
   * - Lookahead time and alerting time is 180s
   * - Only 1 alert level
   * - Instantaneous maneuvers */
  void set_CD3D();

  /* Set DAIDALUS object such that alerting logic and maneuver guidance corresponds to
   * ideal TCASII */
  void set_TCASII();

  /**
   * Return release version string
   */
  static std::string release();

  /* Ownship and Traffic Setting */

  /**
   * Returns state of ownship.
   */
  const TrafficState& getOwnshipState() const;

  /**
   * Returns state of aircraft at index idx
   */
  const TrafficState& getAircraftStateAt(int idx) const;

  /**
   * Set ownship state and current time. Clear all traffic.
   * @param id Ownship's identifier
   * @param pos Ownship's position
   * @param vel Ownship's ground velocity
   * @param time Time stamp of ownship's state
   */
  void setOwnshipState(const std::string& id, const Position& pos, const Velocity& vel, double time);

  /**
   * Set ownship state at time 0.0. Clear all traffic.
   * @param id Ownship's identifier
   * @param pos Ownship's position
   * @param vel Ownship's ground velocity
   */
  void setOwnshipState(const std::string& id, const Position& pos, const Velocity& vel);

  /**
   * Add traffic state at given time.
   * If time is different from current time, traffic state is projected, past or future,
   * into current time. If it's the first aircraft, this aircraft is
   * set as the ownship.
   * @param id Aircraft's identifier
   * @param pos Aircraft's position
   * @param vel Aircraft's ground velocity
   * @param time Time stamp of aircraft's state
   * @return Aircraft's index
   */
  int addTrafficState(const std::string& id, const Position& pos, const Velocity& vel, double time);

  /**
   * Add traffic state at current time. If it's the first aircraft, this aircraft is
   * set as the ownship.
   * @param id Aircraft's identifier
   * @param pos Aircraft's position
   * @param vel Aircraft's ground velocity
   * @return Aircraft's index
   */
  int addTrafficState(const std::string& id, const Position& pos, const Velocity& vel);

  /**
   * Get index of aircraft with given name. Return -1 if no such index exists
   */
  int aircraftIndex(const std::string& name) const;

  /**
   * Exchange ownship aircraft with aircraft named id.
   * EXPERT USE ONLY !!!
   */
  void resetOwnship(const std::string& id);

  /**
   * Remove traffic from the list of aircraft. Returns false if no aircraft was removed.
   * Ownship cannot be removed.
   * If traffic is at index i, the indices of aircraft at k > i, are shifted to k-1.
   * EXPERT USE ONLY !!!
   */
  bool removeTrafficAircraft(const std::string& name);

  /**
   * Project ownship and traffic aircraft offset seconds in the future (if positive) or in the past (if negative)
   * EXPERT USE ONLY !!!
   */
  void linearProjection(double offset);

  /**
   * @return true if ownship has been set
   */
  bool hasOwnship() const;

  /**
   * @return true if at least one traffic has been set
   */
  bool hasTraffic() const;

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

  bool isLatLon() const;

  /* Current Time */

  /**
   * Return currrent time in seconds. Current time is the time of the ownship.
   */
  double getCurrentTime() const;

  /**
   * Return currrent time in specified units. Current time is the time of the ownship.
   */
  double getCurrentTime(const std::string& u) const;

  /* Wind Setting */

  /**
   * Get wind velocity specified in the TO direction
   */
  const Velocity& getWindVelocityTo() const;

  /**
   * Get wind velocity specified in the From direction
   */
  Velocity getWindVelocityFrom() const;

  /**
   * Set wind velocity specified in the TO direction
   * @param wind_velocity: Wind velocity specified in TO direction
   */
  void setWindVelocityTo(const Velocity& wind_vector);

  /**
   * Set wind velocity specified in the From direction
   * @param nwind_velocity: Wind velocity specified in From direction
   */
  void setWindVelocityFrom(const Velocity& nwind_vector);

  /**
   * Set no wind velocity
   */
  void setNoWind();

  /* Alerter Setting */

  /**
   * Set alerter of the aircraft at ac_idx to alerter_idx
   * @param ac_idx: Aircraft index between 0 (ownship) and lastTrafficIndex(), inclusive
   * @param alerter_idx: Alerter index starting from 1. The value 0 means none.
   */
  void setAlerterIndex(int ac_idx, int alerter_idx);

  /**
   * Set alerter of the aircraft at ac_idx to alerter
   * @param ac_idx: Aircraft index between 0 (ownship) and lastTrafficIndex(), inclusive
   * @param alerter: Alerter identifier
   */
  void setAlerter(int ac_idx, const std::string& alerter);

  /**
   * Return alert index used for the traffic aircraft at index ac_idx.
   * The alert index depends on alerting logic. If ownship centric, it returns the
   * alert index of ownship. Otherwise, it returns the alert index of the traffic aircraft
   * at ac_idx.
   */
  int alerterIndexBasedOnAlertingLogic(int ac_idx);

  /**
   * Returns most severe alert level for a given aircraft. Returns 0 if either the aircraft or the alerter is undefined.
   */
  int mostSevereAlertLevel(int ac_idx);

  /* SUM Setting */

  /**
   * Set horizontal position uncertainty of aircraft at index ac_idx
   * s_EW_std: East/West position standard deviation in internal units
   * s_NS_std: North/South position standard deviation in internal units
   * s_EN_std: East/North position standard deviation in internal units
   */
  void setHorizontalPositionUncertainty(int ac_idx, double s_EW_std, double s_NS_std, double s_EN_std);

  /**
   * Set horizontal position uncertainty of aircraft at index ac_idx
   * s_EW_std: East/West position standard deviation in given units
   * s_NS_std: North/South position standard deviation in given units
   * s_EN_std: East/North position standard deviation in given units
   */
  void setHorizontalPositionUncertainty(int ac_idx, double s_EW_std, double s_NS_std, double s_EN_std, const std::string& u);

  /**
   * Set vertical position uncertainty of aircraft at index ac_idx
   * sz_std : Vertical position standard deviation in internal units
   */
  void setVerticalPositionUncertainty(int ac_idx, double sz_std);

  /**
   * Set vertical position uncertainty of aircraft at index ac_idx
   * sz_std : Vertical position standard deviation in given units
   */
  void setVerticalPositionUncertainty(int ac_idx, double sz_std, const std::string& u);

  /**
   * Set horizontal speed uncertainty of aircraft at index ac_idx
   * v_EW_std: East/West position standard deviation in internal units
   * v_NS_std: North/South position standard deviation in internal units
   * v_EN_std: East/North position standard deviation in internal units
   */
  void setHorizontalVelocityUncertainty(int ac_idx, double v_EW_std, double v_NS_std,  double v_EN_std);

  /**
   * Set horizontal speed uncertainty of aircraft at index ac_idx
   * v_EW_std: East/West position standard deviation in given units
   * v_NS_std: North/South position standard deviation in given units
   * v_EN_std: East/North position standard deviation in given units
   */
  void setHorizontalVelocityUncertainty(int ac_idx, double v_EW_std, double v_NS_std,  double v_EN_std, const std::string& u);

  /**
   * Set vertical speed uncertainty of aircraft at index ac_idx
   * vz_std : Vertical speed standard deviation in internal units
   */
  void setVerticalSpeedUncertainty(int ac_idx, double vz_std);

  /**
   * Set vertical speed uncertainty of aircraft at index ac_idx
   * vz_std : Vertical speed standard deviation in given units
   */
  void setVerticalSpeedUncertainty(int ac_idx, double vz_std, const std::string& u);

  /**
   * Reset all uncertainties of aircraft at index ac_idx
   */
  void resetUncertainty(int ac_idx);

  /* Urgency strategy for implicitly coordinate bands (experimental) */

  /**
   * @return strategy for computing most urgent aircraft.
   */
  const UrgencyStrategy* getUrgencyStrategy() const;

  /**
   * Set strategy for computing most urgent aircraft.
   */
  void setUrgencyStrategy(const UrgencyStrategy* strat);

  /**
   * @return most urgent aircraft.
   */
  TrafficState mostUrgentAircraft();

  /* Computation of contours, a.k.a. blobs, and hazard zones */

  /**
   * Computes horizontal contours contributed by aircraft at index idx, for
   * given alert level. A contour is a list of points in counter-clockwise
   * direction representing a polygon. Last point should be connected to first one.
   * The computed polygon should only be used for display purposes since it's merely an
   * approximation of the actual contours defined by the violation and detection methods.
   * @param blobs list of horizontal contours returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   * @param alert_level is the alert level used to compute detection. The value 0
   * indicate the alert level of the corrective region.
   */
  void horizontalContours(std::vector<std::vector<Position> >& blobs, int ac_idx, int alert_level=0);

  /**
   * Computes horizontal contours contributed by aircraft at index idx, for
   * given region. A contour is a list of points in counter-clockwise
   * direction representing a polygon. Last point should be connected to first one.
   * The computed polygon should only be used for display purposes since it's merely an
   * approximation of the actual contours defined by the violation and detection methods.
   * @param blobs list of horizontal contours returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   * @param region is the region used to compute detection.
   */
  void horizontalContours(std::vector<std::vector<Position> >& blobs, int ac_idx,
      BandsRegion::Region region);

  /**
   * Computes horizontal hazard zone around aircraft at index ac_idx, for given alert level.
   * A hazard zone is a list of points in counter-clockwise
   * direction representing a polygon. Last point should be connected to first one.
   * @param haz hazard zone returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   * @param loss true means that the polygon represents the hazard zone. Otherwise,
   * the polygon represents the hazard zone with an alerting time.
   * @param from_ownship true means ownship point of view. Otherwise, the hazard zone is computed
   * from the intruder's point of view.
   * @param alert_level is the alert level used to compute detection. The value 0
   * indicate the alert level of the corrective region.
   * NOTE: The computed polygon should only be used for display purposes since it's merely an
   * approximation of the actual hazard zone defined by the violation and detection methods.
   */
  void horizontalHazardZone(std::vector<Position>& haz, int ac_idx, bool loss, bool from_ownship,
      int alert_level=0);

  /**
   * Computes horizontal hazard zone around aircraft at index ac_idx, for given region.
   * A hazard zone is a list of points in counter-clockwise
   * direction representing a polygon. Last point should be connected to first one.
   * @param haz hazard zone returned by reference.
   * @param ac_idx is the index of the aircraft used to compute the contours.
   * @param loss true means that the polygon represents the hazard zone. Otherwise,
   * the polygon represents the hazard zone with an alerting time.
   * @param from_ownship true means ownship point of view. Otherwise, the hazard zone is computed
   * from the intruder's point of view.
   * @param region is the region used to compute detection.
   * NOTE: The computed polygon should only be used for display purposes since it's merely an
   * approximation of the actual hazard zone defined by the violation and detection methods.
   */
  void horizontalHazardZone(std::vector<Position>& haz, int ac_idx, bool loss, bool from_ownship,
      BandsRegion::Region region);

  /* Setting and getting DaidalusParameters */

  /**
   * Return number of alerters.
   */
  int numberOfAlerters() const;

  /**
   * Return alerter at index i (starting from 1).
   */
  const Alerter& getAlerterAt(int i) const;

  /**
   * Return index of alerter with a given name. Return 0 if it doesn't exist
   */
  int getAlerterIndex(std::string id) const;

  /**
   * Clear all alert thresholds
   */
  void clearAlerters();

  /**
   * Add alerter (if id of alerter already exists, replaces alerter with new one).
   * Return index of added alerter
   */
  int addAlerter(const Alerter& alerter);

  /**
   * @return lookahead time in seconds.
   */
  double getLookaheadTime() const;

  /**
   * @return lookahead time in specified units [u].
   */
  double getLookaheadTime(const std::string& u) const;

  /**
   * @return left direction in radians [0 - pi] [rad] from current ownship's direction
   */
  double getLeftHorizontalDirection() const;

  /**
   * @return left direction in specified units [0 - pi] [u] from current ownship's direction
   */
  double getLeftHorizontalDirection(const std::string& u) const;

  /**
   * @return right direction in radians [0 - pi] [rad] from current ownship's direction
   */
  double getRightHorizontalDirection() const;

  /**
   * @return right direction in specified units [0 - pi] [u] from current ownship's direction
   */
  double getRightHorizontalDirection(const std::string& u) const;

  /**
   * @return minimum horizontal speed for horizontal speed bands in internal units [m/s].
   */
  double getMinHorizontalSpeed() const;

  /**
   * @return minimum horizontal speed for horizontal speed bands in specified units [u].
   */
  double getMinHorizontalSpeed(const std::string& u) const;

  /**
   * @return maximum horizontal speed for horizontal speed bands in internal units [m/s].
   */
  double getMaxHorizontalSpeed() const;

  /**
   * @return maximum horizontal speed for horizontal speed bands in specified units [u].
   */
  double getMaxHorizontalSpeed(const std::string& u) const;

  /**
   * @return minimum vertical speed for vertical speed bands in internal units [m/s].
   */
  double getMinVerticalSpeed() const;

  /**
   * @return minimum vertical speed for vertical speed bands in specified units [u].
   */
  double getMinVerticalSpeed(const std::string& u) const;

  /**
   * @return maximum vertical speed for vertical speed bands in internal units [m/s].
   */
  double getMaxVerticalSpeed() const;

  /**
   * @return maximum vertical speed for vertical speed bands in specified units [u].
   */
  double getMaxVerticalSpeed(const std::string& u) const;

  /**
   * @return minimum altitude for altitude bands in internal units [m]
   */
  double getMinAltitude() const;

  /**
   * @return minimum altitude for altitude bands in specified units [u].
   */
  double getMinAltitude(const std::string& u) const;

  /**
   * @return maximum altitude for altitude bands in internal units [m]
   */
  double getMaxAltitude() const;

  /**
   * @return maximum altitude for altitude bands in specified units [u].
   */
  double getMaxAltitude(const std::string& u) const;

  /**
   * @return Horizontal speed in internal units (below current value) for the
   * computation of relative bands
   */
  double getBelowRelativeHorizontalSpeed() const;

  /**
   * @return Horizontal speed in given units (below current value) for the
   * computation of relative bands
   */
  double getBelowRelativeHorizontalSpeed(const std::string& u) const;

  /**
   * @return Horizontal speed in internal units (above current value) for the
   * computation of relative bands
   */
  double getAboveRelativeHorizontalSpeed() const;

  /**
   * @return Horizontal speed in given units (above current value) for the
   * computation of relative bands
   */
  double getAboveRelativeHorizontalSpeed(const std::string& u) const;

  /**
   * @return Vertical speed in internal units (below current value) for the
   * computation of relative bands
   */
  double getBelowRelativeVerticalSpeed() const;

  /**
   * @return Vertical speed in given units (below current value) for the
   * computation of relative bands
   */
  double getBelowRelativeVerticalSpeed(const std::string& u) const;

  /**
   * @return Vertical speed in internal units (above current value) for the
   * computation of relative bands
   */
  double getAboveRelativeVerticalSpeed() const;

  /**
   * @return Vertical speed in given units (above current value) for the
   * computation of relative bands
   */
  double getAboveRelativeVerticalSpeed(const std::string& u) const;

  /**
   * @return Altitude in internal units (below current value) for the
   * computation of relative bands
   */
  double getBelowRelativeAltitude() const;

  /**
   * @return Altitude in given units (below current value) for the
   * computation of relative bands
   */
  double getBelowRelativeAltitude(const std::string& u) const;

  /**
   * @return Altitude in internal units (above current value) for the
   * computation of relative bands
   */
  double getAboveRelativeAltitude() const;

  /**
   * @return Altitude in given units (above current value) for the
   * computation of relative bands
   */
  double getAboveRelativeAltitude(const std::string& u) const;

  /**
   * @return step size for direction bands in internal units [rad].
   */
  double getHorizontalDirectionStep() const;

  /**
   * @return step size for direction bands in specified units [u].
   */
  double getHorizontalDirectionStep(const std::string& u) const;

  /**
   * @return step size for horizontal speed bands in internal units [m/s].
   */
  double getHorizontalSpeedStep() const;

  /**
   * @return step size for horizontal speed bands in specified units [u].
   */
  double getHorizontalSpeedStep(const std::string& u) const;

  /**
   * @return step size for vertical speed bands in internal units [m/s].
   */
  double getVerticalSpeedStep() const;

  /**
   * @return step size for vertical speed bands in specified units [u].
   */
  double getVerticalSpeedStep(const std::string& u) const;

  /**
   * @return step size for altitude bands in internal units [m]
   */
  double getAltitudeStep() const;

  /**
   * @return step size for altitude bands in specified units [u].
   */
  double getAltitudeStep(const std::string& u) const;

  /**
   * @return horizontal acceleration for horizontal speed bands to value in internal units [m/s^2].
   */
  double getHorizontalAcceleration() const;

  /**
   * @return horizontal acceleration for horizontal speed bands to value in specified units [u].
   */
  double getHorizontalAcceleration(const std::string& u) const;

  /**
   * @return constant vertical acceleration for vertical speed and altitude bands in internal [m/s^2]
   * units
   */
  double getVerticalAcceleration() const;

  /**
   * @return constant vertical acceleration for vertical speed and altitude bands in specified
   * units
   */
  double getVerticalAcceleration(const std::string& u) const;

  /**
   * @return turn rate in internal units [rad/s].
   */
  double getTurnRate() const;

  /**
   * @return turn rate in specified units [u].
   */
  double getTurnRate(const std::string& u) const;

  /**
   * @return bank angle in internal units [rad].
   */
  double getBankAngle() const;

  /**
   * @return bank angle in specified units [u].
   */
  double getBankAngle(const std::string& u) const;

  /**
   * @return the vertical climb/descend rate for altitude bands in internal units [m/s]
   */
  double getVerticalRate() const;

  /**
   * @return the vertical climb/descend rate for altitude bands in specified units [u].
   */
  double getVerticalRate(std::string u) const;

  /**
   * @return horizontal NMAC distance in internal units [m].
   */
  double getHorizontalNMAC() const;

  /**
   * @return horizontal NMAC distance in specified units [u].
   */
  double getHorizontalNMAC(const std::string& u) const;

  /**
   * @return vertical NMAC distance in internal units [m].
   */
  double getVerticalNMAC() const;

  /**
   * @return vertical NMAC distance in specified units [u].
   */
  double getVerticalNMAC(const std::string& u) const;

  /**
   * @return recovery stability time in seconds. Recovery bands are computed at time of
   * first conflict-free region plus this time.
   */
  double getRecoveryStabilityTime() const;

  /**
   * @return recovery stability time in specified Units:: Recovery bands are computed at time of
   * first conflict-free region plus this time.
   */
  double getRecoveryStabilityTime(const std::string& u) const;

  /**
   * @return hysteresis time in seconds.
   */
  double getHysteresisTime() const;

  /**
   * @return hysteresis time in specified units [u].
   */
  double getHysteresisTime(const std::string& u) const;

  /**
   * @return alerting persistence time in seconds.
   */
  double getPersistenceTime() const;

  /**
   * @return alerting persistence time in specified units [u].
   */
  double getPersistenceTime(const std::string& u) const;

  /**
   * @return true if guidance persistence is enabled
   */
  bool isEnabledBandsPersistence() const;

  /**
   * Enable/disable bands persistence
   */
  void setBandsPersistence(bool flag);

  /**
   * Enable bands persistence
   */
  void enableBandsPersistence();

  /**
   * Disable bands persistence
   */
  void disableBandsPersistence();

  /**
   * @return persistence for preferred horizontal direction resolution in internal units
   */
  double getPersistencePreferredHorizontalDirectionResolution() const;

  /**
   * @return persistence for preferred horizontal direction resolution in given units
   */
  double getPersistencePreferredHorizontalDirectionResolution(const std::string& u) const;

  /**
   * @return persistence for preferred horizontal speed resolution in internal units
   */
  double getPersistencePreferredHorizontalSpeedResolution() const;

  /**
   * @return persistence for preferred horizontal speed resolution in given units
   */
  double getPersistencePreferredHorizontalSpeedResolution(const std::string& u) const;

  /**
   * @return persistence for preferred vertical speed resolution in internal units
   */
  double getPersistencePreferredVerticalSpeedResolution() const;

  /**
   * @return persistence for preferred vertical speed resolution in given units
   */
  double getPersistencePreferredVerticalSpeedResolution(const std::string& u) const;

  /**
   * @return persistence for preferred altitude resolution in internal units
   */
  double getPersistencePreferredAltitudeResolution() const;

  /**
   * @return persistence for preferred altitude resolution in given units
   */
  double getPersistencePreferredAltitudeResolution(const std::string& u) const;

  /**
   * @return Alerting parameter m of "M of N" strategy
   */
  int getAlertingParameterM() const;

  /**
   * @return Alerting parameter m of "M of N" strategy
   */
  int getAlertingParameterN() const;

  /**
   * @return minimum horizontal separation for recovery bands in internal units [m].
   */
  double getMinHorizontalRecovery() const;

  /**
   * Return minimum horizontal separation for recovery bands in specified units [u]
   */
  double getMinHorizontalRecovery(const std::string& u) const;

  /**
   * @return minimum vertical separation for recovery bands in internal units [m].
   */
  double getMinVerticalRecovery() const;

  /**
   * Return minimum vertical separation for recovery bands in specified units [u].
   */
  double getMinVerticalRecovery(const std::string& u) const;

  /**
   * Sets lookahead time in seconds.
   */
  void setLookaheadTime(double t);

  /**
   * Set lookahead time to value in specified units [u].
   */
  void setLookaheadTime(double t, const std::string& u);

  /**
   * Set left direction to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  void setLeftHorizontalDirection(double val);

  /**
   * Set left direction to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  void setLeftHorizontalDirection(double val, const std::string& u);

  /**
   * Set right direction to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  void setRightHorizontalDirection(double val);

  /**
   * Set right direction to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  void setRightHorizontalDirection(double val, const std::string& u);

  /**
   * Sets minimum horizontal speed for horizontal speed bands to value in internal units [m/s].
   */
  void setMinHorizontalSpeed(double val);

  /**
   * Sets minimum horizontal speed for horizontal speed bands to value in specified units [u].
   */
  void setMinHorizontalSpeed(double val, const std::string& u);

  /**
   * Sets maximum horizontal speed for horizontal speed bands to value in internal units [m/s].
   */
  void setMaxHorizontalSpeed(double val);

  /**
   * Sets maximum horizontal speed for horizontal speed bands to value in specified units [u].
   */
  void setMaxHorizontalSpeed(double val, const std::string& u);

  /**
   * Sets minimum vertical speed for vertical speed bands to value in internal units [m/s].
   */
  void setMinVerticalSpeed(double val);

  /**
   * Sets minimum vertical speed for vertical speed bands to value in specified units [u].
   */
  void setMinVerticalSpeed(double val, const std::string& u);

  /**
   * Sets maximum vertical speed for vertical speed bands to value in internal units [m/s].
   */
  void setMaxVerticalSpeed(double val);

  /**
   * Sets maximum vertical speed for vertical speed bands to value in specified units [u].
   */
  void setMaxVerticalSpeed(double val, const std::string& u);

  /**
   * Sets minimum altitude for altitude bands to value in internal units [m]
   */
  void setMinAltitude(double val);

  /**
   * Sets minimum altitude for altitude bands to value in specified units [u].
   */
  void setMinAltitude(double val, const std::string& u);

  /**
   * Sets maximum altitude for altitude bands to value in internal units [m]
   */
  void setMaxAltitude(double val);

  /**
   * Sets maximum altitude for altitude bands to value in specified units [u].
   */
  void setMaxAltitude(double val, const std::string& u);

  /**
   * Set horizontal speed in internal units (below current value) for the
   * computation of relative bands
   */
  void setBelowRelativeHorizontalSpeed(double val);

  /**
   * Set horizontal speed in given units (below current value) for the
   * computation of relative bands
   */
  void setBelowRelativeHorizontalSpeed(double val,std::string u);

  /**
   * Set horizontal speed in internal units (above current value) for the
   * computation of relative bands
   */
  void setAboveRelativeHorizontalSpeed(double val);

  /**
   * Set horizontal speed in given units (above current value) for the
   * computation of relative bands
   */
  void setAboveRelativeHorizontalSpeed(double val, const std::string& u);

  /**
   * Set vertical speed in internal units (below current value) for the
   * computation of relative bands
   */
  void setBelowRelativeVerticalSpeed(double val);

  /**
   * Set vertical speed in given units (below current value) for the
   * computation of relative bands
   */
  void setBelowRelativeVerticalSpeed(double val, const std::string& u);

  /**
   * Set vertical speed in internal units (above current value) for the
   * computation of relative bands
   */
  void setAboveRelativeVerticalSpeed(double val);

  /**
   * Set vertical speed in given units (above current value) for the
   * computation of relative bands
   */
  void setAboveRelativeVerticalSpeed(double val, const std::string& u);

  /**
   * Set altitude in internal units (below current value) for the
   * computation of relative bands
   */
  void setBelowRelativeAltitude(double val);

  /**
   * Set altitude in given units (below current value) for the
   * computation of relative bands
   */
  void setBelowRelativeAltitude(double val, const std::string& u);

  /**
   * Set altitude in internal units (above current value) for the
   * computation of relative bands
   */
  void setAboveRelativeAltitude(double val);

  /**
   * Set altitude in given units (above current value) for the
   * computation of relative bands
   */
  void setAboveRelativeAltitude(double val, const std::string& u);

  /**
   * Set below value to min when computing horizontal speed bands
   */
  void setBelowToMinRelativeHorizontalSpeed();

  /**
   * Set above value to max when computing horizontal speed bands
   */
  void setAboveToMaxRelativeHorizontalSpeed();

  /**
   * Set below value to min when computing vertical speed bands
   */
  void setBelowToMinRelativeVerticalSpeed();

  /**
   * Set above value to max when computing vertical speed bands
   */
  void setAboveToMaxRelativeVerticalSpeed();

  /**
   * Set below value to min when computing altitude bands
   */
  void setBelowToMinRelativeAltitude();

  /**
   * Set above value to max when computing altitude bands
   */
  void setAboveToMaxRelativeAltitude();

  /**
   * Disable relative horizontal speed bands
   */
  void disableRelativeHorizontalSpeedBands();

  /**
   * Disable relative vertical speed bands
   */
  void disableRelativeVerticalSpeedBands();

  /**
   * Disable relative altitude bands
   */
  void disableRelativeAltitude();

  /**
   * Sets step size for direction bands in internal units [rad].
   */
  void setHorizontalDirectionStep(double val);

  /**
   * Sets step size for direction bands in specified units [u].
   */
  void setHorizontalDirectionStep(double val, const std::string& u);

  /**
   * Sets step size for horizontal speed bands to value in internal units [m/s].
   */
  void setHorizontalSpeedStep(double val);

  /**
   * Sets step size for horizontal speed bands to value in specified units [u].
   */
  void setHorizontalSpeedStep(double val, const std::string& u);

  /**
   * Sets step size for vertical speed bands to value in internal units [m/s].
   */
  void setVerticalSpeedStep(double val);

  /**
   * Sets step size for vertical speed bands to value in specified units [u].
   */
  void setVerticalSpeedStep(double val, const std::string& u);

  /**
   * Sets step size for altitude bands to value in internal units [m]
   */
  void setAltitudeStep(double val);

  /**
   * Sets step size for altitude bands to value in specified units [u].
   */
  void setAltitudeStep(double val, const std::string& u);

  /**
   * Sets horizontal acceleration for horizontal speed bands to value in internal units [m/s^2].
   */
  void setHorizontalAcceleration(double val);

  /**
   * Sets horizontal acceleration for horizontal speed bands to value in specified units [u].
   */
  void setHorizontalAcceleration(double val, const std::string& u);

  /**
   * Sets the constant vertical acceleration for vertical speed and altitude bands
   * to value in internal units [m/s^2]
   */
  void setVerticalAcceleration(double val);

  /**
   * Sets the constant vertical acceleration for vertical speed and altitude bands
   * to value in specified units [u].
   */
  void setVerticalAcceleration(double val, const std::string& u);

  /**
   * Sets turn rate for direction bands to value in internal units [rad/s]. As a side effect, this method
   * resets the bank angle.
   */
  void setTurnRate(double val);

  /**
   * Sets turn rate for direction bands to value in specified units [u]. As a side effect, this method
   * resets the bank angle.
   */
  void setTurnRate(double val, const std::string& u);

  /**
   * Sets bank angle for direction bands to value in internal units [rad]. As a side effect, this method
   * resets the turn rate.
   */
  void setBankAngle(double val);

  /**
   * Sets bank angle for direction bands to value in specified units [u]. As a side effect, this method
   * resets the turn rate.
   */
  void setBankAngle(double val, const std::string& u);

  /**
   * Sets vertical rate for altitude bands to value in internal units [m/s]
   */
  void setVerticalRate(double val);

  /**
   * Sets vertical rate for altitude bands to value in specified units [u].
   */
  void setVerticalRate(double val, const std::string& u);

  /**
   * Set horizontal NMAC distance to value in internal units [m].
   */
  void setHorizontalNMAC(double val);

  /**
   * Set horizontal NMAC distance to value in specified units [u].
   */
  void setHorizontalNMAC(double val, const std::string& u);

  /**
   * Set vertical NMAC distance to value in internal units [m].
   */
  void setVerticalNMAC(double val);

  /**
   * Set vertical NMAC distance to value in specified units [u].
   */
  void setVerticalNMAC(double val, const std::string& u);

  /**
   * Sets recovery stability time in seconds. Recovery bands are computed at time of
   * first conflict-free region plus this time.
   */
  void setRecoveryStabilityTime(double t);

  /**
   * Sets recovery stability time in specified units. Recovery bands are computed at time of
   * first conflict-free region plus this time.
   */
  void setRecoveryStabilityTime(double t, const std::string& u);

  /**
   * Set hysteresis time to value in seconds.
   */
  void setHysteresisTime(double val);

  /**
   * Set alerting persistence time to value in seconds.
   */
  void setPersistenceTime(double val);

  /**
   * Set alerting persistence time to value in specified units [u].
   */
  void setPersistenceTime(double val, const std::string& u);

  /**
   * Set hysteresis time to value in specified units [u].
   */
  void setHysteresisTime(double val, const std::string& u);

  /**
   * Set persistence for preferred horizontal direction resolution in internal units
   */
  void setPersistencePreferredHorizontalDirectionResolution(double val);

  /**
   * Set persistence for preferred horizontal direction resolution in given units
   */
  void setPersistencePreferredHorizontalDirectionResolution(double val, const std::string& u);

  /**
   * Set persistence for preferred horizontal speed resolution in internal units
   */
  void setPersistencePreferredHorizontalSpeedResolution(double val);

  /**
   * Set persistence for preferred horizontal speed resolution in given units
   */
  void setPersistencePreferredHorizontalSpeedResolution(double val, const std::string& u);

  /**
   * Set persistence for preferred vertical speed resolution in internal units
   */
  void setPersistencePreferredVerticalSpeedResolution(double val);

  /**
   * Set persistence for preferred vertical speed resolution in given units
   */
  void setPersistencePreferredVerticalSpeedResolution(double val, const std::string& u);

  /**
   * Set persistence for preferred altitude resolution in internal units
   */
  void setPersistencePreferredAltitudeResolution(double val);

  /**
   * Set persistence for preferred altitude resolution in given units
   */
  void setPersistencePreferredAltitudeResolution(double val, const std::string& u);

  /**
   * @return Alerting parameter m of "M of N" strategy
   */
  void setAlertingMofN(int m, int n);

  /**
   * Sets minimum horizontal separation for recovery bands in internal units [m].
   */
  void setMinHorizontalRecovery(double val);

  /**
   * Set minimum horizontal separation for recovery bands in specified units [u].
   */
  void setMinHorizontalRecovery(double val, const std::string& u);

  /**
   * Sets minimum vertical separation for recovery bands in internal units [m].
   */
  void setMinVerticalRecovery(double val);

  /**
   * Set minimum vertical separation for recovery bands in units
   */
  void setMinVerticalRecovery(double val, const std::string& u);

  /**
   * @return true if repulsive criteria is enabled for conflict bands.
   */
  bool isEnabledConflictCriteria();

  /**
   * Enable/disable repulsive criteria for conflict bands.
   */
  void setConflictCriteria(bool flag);

  /**
   * Enable repulsive criteria for conflict bands.
   */
  void enableConflictCriteria();

  /**
   * Disable repulsive criteria for conflict bands.
   */
  void disableConflictCriteria();

  /**
   * @return true if repulsive criteria is enabled for recovery bands.
   */
  bool isEnabledRecoveryCriteria();

  /**
   * Enable/disable repulsive criteria for recovery bands.
   */
  void setRecoveryCriteria(bool flag);

  /**
   * Enable repulsive criteria for recovery bands.
   */
  void enableRecoveryCriteria();

  /**
   * Disable repulsive criteria for recovery bands.
   */
  void disableRecoveryCriteria();

  /**
   * Enable/disable repulsive criteria for conflict and recovery bands.
   */
  void setRepulsiveCriteria(bool flag);

  /**
   * Enable repulsive criteria for conflict and recovery bands.
   */
  void enableRepulsiveCriteria();

  /**
   * Disable repulsive criteria for conflict and recovery bands.
   */
  void disableRepulsiveCriteria();

  /**
   * @return true if recovery direction bands are enabled.
   */
  bool isEnabledRecoveryHorizontalDirectionBands();

  /**
   * @return true if recovery horizontal speed bands are enabled.
   */
  bool isEnabledRecoveryHorizontalSpeedBands();

  /**
   * @return true if recovery vertical speed bands are enabled.
   */
  bool isEnabledRecoveryVerticalSpeedBands();

  /**
   * @return true if recovery altitude bands are enabled.
   */
  bool isEnabledRecoveryAltitudeBands();

  /**
   * Sets recovery bands flag for direction, horizontal speed, and vertical speed bands to specified value.
   */
  void setRecoveryBands(bool flag);

  /**
   * Enable all recovery bands for direction, horizontal speed, vertical speed, and altitude.
   */
  void enableRecoveryBands();

  /**
   * Disable all recovery bands for direction, horizontal speed, vertical speed, and altitude.
   */
  void disableRecoveryBands();

  /**
   * Sets recovery bands flag for direction bands to specified value.
   */
  void setRecoveryHorizontalDirectionBands(bool flag);

  /**
   * Sets recovery bands flag for horizontal speed bands to specified value.
   */
  void setRecoveryHorizontalSpeedBands(bool flag);

  /**
   * Sets recovery bands flag for vertical speed bands to specified value.
   */
  void setRecoveryVerticalSpeedBands(bool flag);

  /**
   * Sets recovery bands flag for altitude bands to specified value.
   */
  void setRecoveryAltitudeBands(bool flag);

  /**
   * @return true if collision avoidance bands are enabled.
   */
  bool isEnabledCollisionAvoidanceBands();

  /**
   * Enable/disable collision avoidance bands.
   */
  void setCollisionAvoidanceBands(bool flag);

  /**
   * Enable collision avoidance bands.
   */
  void enableCollisionAvoidanceBands();

  /**
   * Disable collision avoidance bands.
   */
  void disableCollisionAvoidanceBands();

  /**
   * @return get factor for computing collision avoidance bands. Factor value is in (0,1]
   */
  double getCollisionAvoidanceBandsFactor() const;

  /**
   * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
   */
  void setCollisionAvoidanceBandsFactor(double val);

  /**
   * @return get z-score (number of standard deviations) for horizontal position
   */
  double getHorizontalPositionZScore() const;

  /**
   * @return set z-score (number of standard deviations) for horizontal position (non-negative value)
   */
  void setHorizontalPositionZScore(double val);

  /**
   * @return get min z-score (number of standard deviations) for horizontal velocity
   */
  double getHorizontalVelocityZScoreMin() const;

  /**
   * @return set min z-score (number of standard deviations) for horizontal velocity (non-negative value)
   */
  void setHorizontalVelocityZScoreMin(double val);

  /**
   * @return get max z-score (number of standard deviations) for horizontal velocity
   */
  double getHorizontalVelocityZScoreMax() const;

  /**
   * @return set max z-score (number of standard deviations) for horizontal velocity (non-negative value)
   */
  void setHorizontalVelocityZScoreMax(double val);

  /**
   * @return Distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
   */
  double getHorizontalVelocityZDistance() const;

  /**
   * @return Distance (in given units) at which h_vel_z_score scales from min to max as range decreases
   */
  double getHorizontalVelocityZDistance(std::string u) const;

  /**
   * @return Set distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
   */
  void setHorizontalVelocityZDistance(double val);

  /**
   * @return Set distance (in given units) at which h_vel_z_score scales from min to max as range decreases
   */
  void setHorizontalVelocityZDistance(double val, const std::string& u);

  /**
   * @return get z-score (number of standard deviations) for vertical position
   */
  double getVerticalPositionZScore() const;

  /**
   * @return set z-score (number of standard deviations) for vertical position (non-negative value)
   */
  void setVerticalPositionZScore(double val);

  /**
   * @return get z-score (number of standard deviations) for vertical velocity
   */
  double getVerticalSpeedZScore() const;

  /**
   * @return set z-score (number of standard deviations) for vertical velocity (non-negative value)
   */
  void setVerticalSpeedZScore(double val);

  /**
   * Get horizontal contour threshold, specified in internal units [rad] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  double getHorizontalContourThreshold() const;

  /**
   * Get horizontal contour threshold, specified in given units [u] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  double getHorizontalContourThreshold(std::string u) const;

  /**
   * Set horizontal contour threshold, specified in internal units [rad] [0 - pi] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  void setHorizontalContourThreshold(double val);

  /**
   * Set horizontal contour threshold, specified in given units [u] [0 - pi] as an angle to
   * the left/right of current aircraft direction. A value of 0 means only conflict contours.
   * A value of pi means all contours.
   */
  void setHorizontalContourThreshold(double val, const std::string& u);

  /**
   * Return true if DTA logic is active at current time
   */
  bool isActiveDTALogic();

  /**
   * Return true if DTA special maneuver guidance is active at current time
   */
  bool isActiveDTASpecialManeuverGuidance();

  /**
   * Return true if DAA Terminal Area (DTA) logic is disabled.
   */
  bool isDisabledDTALogic() const;

  /**
   * Return true if DAA Terminal Area (DTA) logic is enabled with horizontal
   * direction recovery guidance. If true, horizontal direction recovery is fully enabled,
   * but vertical recovery blocks down resolutions when alert is higher than corrective.
   * NOTE:
   * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
   * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
   * intruder-centric logic).
   */
  bool isEnabledDTALogicWithHorizontalDirRecovery() const;

  /**
   * Return true if DAA Terminal Area (DTA) logic is enabled without horizontal
   * direction recovery guidance. If true, horizontal direction recovery is disabled and
   * vertical recovery blocks down resolutions when alert is higher than corrective.
   * NOTE:
   * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
   * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
   * intruder-centric logic).
   */
  bool isEnabledDTALogicWithoutHorizontalDirRecovery() const;

  /**
   * Disable DAA Terminal Area (DTA) logic
   */
  void disableDTALogic();

  /**
   * Enable DAA Terminal Area (DTA) logic with horizontal direction recovery guidance, i.e.,
   * horizontal direction recovery is fully enabled, but vertical recovery blocks down
   * resolutions when alert is higher than corrective.
   * NOTE:
   * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
   * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
   * intruder-centric logic).
   */
  void enableDTALogicWithHorizontalDirRecovery();

  /**
   * Enable DAA Terminal Area (DTA) logic withou horizontal direction recovery guidance, i.e.,
   * horizontal direction recovery is disabled and vertical recovery blocks down
   * resolutions when alert is higher than corrective.
   * NOTE:
   * When DTA logic is enabled, DAIDALUS automatically switches to DTA alerter and to
   * special maneuver guidance, when aircraft enters DTA volume (depending on ownship- vs
   * intruder-centric logic).
   */
  void enableDTALogicWithoutHorizontalDirRecovery();

  /**
   * Get DAA Terminal Area (DTA) position (lat/lon)
   */
  const Position& getDTAPosition() const;

  /**
   * Set DAA Terminal Area (DTA) latitude (internal units)
   */
  void setDTALatitude(double lat);

  /**
   * Set DAA Terminal Area (DTA) latitude in given units
   */
  void setDTALatitude(double lat, const std::string& ulat);

  /**
   * Set DAA Terminal Area (DTA) longitude (internal units)
   */
  void setDTALongitude(double lon);

  /**
   * Set DAA Terminal Area (DTA) longitude in given units
   */
  void setDTALongitude(double lon, const std::string& ulon);

  /**
   * Get DAA Terminal Area (DTA) radius (internal units)
   */
  double getDTARadius() const;

  /**
   * Get DAA Terminal Area (DTA) radius in given units
   */
  double getDTARadius(const std::string& u) const;

  /**
   * Set DAA Terminal Area (DTA) radius (internal units)
   */
  void setDTARadius(double val);

  /**
   * Set DAA Terminal Area (DTA) radius in given units
   */
  void setDTARadius(double val, const std::string& u);

  /**
   * Get DAA Terminal Area (DTA) height (internal units)
   */
  double getDTAHeight() const;

  /**
   * Get DAA Terminal Area (DTA) height in given units
   */
  double getDTAHeight(const std::string& u) const;

  /**
   * Set DAA Terminal Area (DTA) height (internal units)
   */
  void setDTAHeight(double val);

  /**
   * Set DAA Terminal Area (DTA) height in given units
   */
  void setDTAHeight(double val, const std::string& u);

  /**
   * Get DAA Terminal Area (DTA) alerter
   */
  int getDTAAlerter() const;

  /**
   * Set DAA Terminal Area (DTA) alerter
   */
  void setDTAAlerter(int alerter);

  /**
   * Set alerting logic to the value indicated by ownship_centric.
   * If ownship_centric is true, alerting and guidance logic will use the alerter in ownship. Alerter
   * in every intruder will be disregarded.
   * If ownship_centric is false, alerting and guidance logic will use the alerter in every intruder. Alerter
   * in ownship will be disregarded.
   */
  void setAlertingLogic(bool ownship_centric);

  /**
   * Set alerting and guidance logic to ownship-centric. Alerting and guidance logic will use the alerter in ownship.
   * Alerter in every intruder will be disregarded.
   */
  void setOwnshipCentricAlertingLogic();

  /**
   * Set alerting and guidance logic to intruder-centric. Alerting and guidance logic will use the alerter in every intruder.
   * Alerter in ownship will be disregarded.
   */
  void setIntruderCentricAlertingLogic();

  /**
   * @return true if alerting/guidance logic is ownship centric.
   */
  bool isAlertingLogicOwnshipCentric() const;

  /**
   * Get corrective region for calculation of resolution maneuvers and bands saturation.
   */
  BandsRegion::Region getCorrectiveRegion() const;

  /**
   * Set corrective region for calculation of resolution maneuvers and bands saturation.
   */
  void setCorrectiveRegion(BandsRegion::Region val);

  /**
   * @param alerter_idx Indice of an alerter (starting from 1)
   * @return corrective level of alerter at alerter_idx. The corrective level
   * is the first alert level that has a region equal to or more severe than corrective_region.
   * Return -1 if alerter_idx is out of range of if there is no corrective alert level
   * for this alerter.
   */
  int correctiveAlertLevel(int alerter_idx);

  /**
   * @return maximum alert level for all alerters. Returns 0 if alerter list is empty.
   */
  int maxAlertLevel() const;

  /**
   * Set instantaneous bands.
   */
  void setInstantaneousBands();

  /**
   * Set kinematic bands.
   * Set turn rate to 3 deg/s, when type is true; set turn rate to  1.5 deg/s
   * when type is false;
   */
  void setKinematicBands(bool type);

  /**
   * Disable hysteresis parameters
   */
  void disableHysteresis();

  /**
   *  Load parameters from file.
   */
  bool loadFromFile(const std::string& file);

  /**
   *  Write parameters to file.
   */
  bool saveToFile(const std::string& file);

  /**
   * Set bands parameters
   */
  void setDaidalusParameters(const DaidalusParameters& parameters);

  void setParameterData(const ParameterData& p);

  const ParameterData getParameterData();

  /**
   * Get std::string units of parameters key
   */
  std::string getUnitsOf(const std::string& key) const;

  /* Direction Bands Settings */

  /**
   * Set absolute min/max directions for bands computations. Directions are specified in internal units [rad].
   * Values are expected to be in [0 - 2pi)
   */
  void setAbsoluteHorizontalDirectionBands(double min, double max);

  /**
   * Set absolute min/max directions for bands computations. Directions are specified in given units [u].
   * Values are expected to be in [0 - 2pi) [u]
   */
  void setAbsoluteHorizontalDirectionBands(double min, double max, const std::string& u);

  /* Utility methods */

  /**
   * Return core object of bands. For expert users only
   * DO NOT USE IT, UNLESS YOU KNOW WHAT YOU ARE DOING. EXPERT USE ONLY !!!
   */
  const DaidalusCore& getCore();

  /**
   *  Clear hysteresis data
   */
  void clearHysteresis();

  /**
   *  Clear ownship and traffic state data from this object.
   *  IMPORTANT: This method reset cache and hysteresis parameters.
   */
  void clear();

  /**
   * Set cached values to stale conditions and clear hysteresis variables.
   */
  void reset();

  /* Main interface methods */

  /**
   * Compute in acs list of aircraft identifiers contributing to conflict bands for given
   * conflict bands region.
   * 1 = FAR, 2 = MID, 3 = NEAR.
   */
  void conflictBandsAircraft(std::vector<std::string>& acs, int region);

  /**
   * Compute in acs list of aircraft identifiers contributing to conflict bands for given
   * conflict bands region.
   */
  void conflictBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region);

  /**
   * Return time interval of violation for given conflict bands region
   * 1 = FAR, 2 = MID, 3 = NEAR
   */
  Interval timeIntervalOfConflict(int region);

  /**
   * Return time interval of violation for given conflict bands region
   */
  Interval timeIntervalOfConflict(BandsRegion::Region region);

  /**
   * @return the number of horizontal direction bands negative if the ownship has not been set
   */
  int horizontalDirectionBandsLength();

  /**
   * Force computation of direction bands. Usually, bands are only computed when needed. This method
   * forces the computation of direction bands (this method is included mainly for debugging purposes).
   */
  void forceHorizontalDirectionBandsComputation();

  /**
   * @return the i-th interval, in internal units [rad], of the computed direction bands.
   * @param i index
   */
  Interval horizontalDirectionIntervalAt(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed direction bands.
   * @param i index
   * @param u units
   */
  Interval horizontalDirectionIntervalAt(int i, const std::string& u);

  /**
   * @return the i-th region of the computed direction bands.
   * @param i index
   */
  BandsRegion::Region horizontalDirectionRegionAt(int i);

  /**
   * @return the index of a given direction specified in internal units [rad]
   * @param dir [rad]
   */
  int indexOfHorizontalDirection(double dir);

  /**
   * @return the index of a given direction specified in given units [u]
   * @param dir [u]
   * @param u Units
   */
  int indexOfHorizontalDirection(double dir, const std::string& u);

  /**
   * @return the region of a given direction specified in internal units [rad].
   * @param dir [rad]
   */
  BandsRegion::Region regionOfHorizontalDirection(double dir);

  /**
   * @return the region of a given direction specified in given units [u]
   * @param dir [u]
   * @param u Units
   */
  BandsRegion::Region regionOfHorizontalDirection(double dir, const std::string& u);

  /**
   * Return last time to direction maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  double lastTimeToHorizontalDirectionManeuver(const TrafficState& ac);

  /**
   * @return recovery information for horizontal direction bands.
   */
  RecoveryInformation horizontalDirectionRecoveryInformation();

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral horizontal direction bands
   * for given conflict bands region.
   * 1 = FAR, 2 = MID, 3 = NEAR
   */
  void peripheralHorizontalDirectionBandsAircraft(std::vector<std::string>& acs, int region);

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral horizontal direction bands
   * for given conflict bands region.
   */
  void peripheralHorizontalDirectionBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region);

  /**
   * Compute horizontal direction resolution maneuver for a given direction.
   * @parameter dir is right (true)/left (false) of ownship current direction
   * @return direction resolution in internal units [rad] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no resolution to the right, and negative infinity if there
   * is no resolution to the left.
   */
  double horizontalDirectionResolution(bool dir);

  /**
   * Compute horizontal direction resolution maneuver for a given direction.
   * @parameter dir is right (true)/left (false) of ownship current direction
   * @parameter u units
   * @return direction resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no resolution to the right, and negative infinity if there
   * is no resolution to the left.
   */
  double horizontalDirectionResolution(bool dir, const std::string& u);

  /**
   * Compute preferred horizontal direction based on resolution that is closer to current direction.
   * @return True: Right. False: Left.
   */
  bool preferredHorizontalDirectionRightOrLeft();

  /**
   * @return the number of horizontal speed band intervals, negative if the ownship has not been set
   */
  int horizontalSpeedBandsLength();

  /**
   * Force computation of horizontal speed bands. Usually, bands are only computed when needed. This method
   * forces the computation of horizontal speed bands (this method is included mainly for debugging purposes).
   */
  void forceHorizontalSpeedBandsComputation();

  /**
   * @return the i-th interval, in internal units [m/s], of the computed horizontal speed bands.
   * @param i index
   */
  Interval horizontalSpeedIntervalAt(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed horizontal speed bands.
   * @param i index
   * @param u units
   */
  Interval horizontalSpeedIntervalAt(int i, const std::string& u);

  /**
   * @return the i-th region of the computed horizontal speed bands.
   * @param i index
   */
  BandsRegion::Region horizontalSpeedRegionAt(int i);

  /**
   * @return the range index of a given horizontal speed specified in internal units [m/s]
   * @param gs [m/s]
   */
  int indexOfHorizontalSpeed(double gs);

  /**
   * @return the range index of a given horizontal speed specified in given units [u]
   * @param gs [u]
   * @param u Units
   */
  int indexOfHorizontalSpeed(double gs, const std::string& u);

  /**
   * @return the region of a given horizontal speed specified in internal units [m/s]
   * @param gs [m/s]
   */
  BandsRegion::Region regionOfHorizontalSpeed(double gs);

  /**
   * @return the region of a given horizontal speed specified in given units [u]
   * @param gs [u]
   * @param u Units
   */
  BandsRegion::Region regionOfHorizontalSpeed(double gs, const std::string& u);

  /**
   * Return last time to horizontal speed maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  double lastTimeToHorizontalSpeedManeuver(const TrafficState& ac);

  /**
   * @return recovery information for horizontal speed bands.
   */
  RecoveryInformation horizontalSpeedRecoveryInformation();

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral horizontal speed bands
   * for given conflict bands region.
   * 1 = FAR, 2 = MID, 3 = NEAR
   */
  void peripheralHorizontalSpeedBandsAircraft(std::vector<std::string>& acs, int region);

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral horizontal speed bands
   * for given conflict bands region.
   */
  void peripheralHorizontalSpeedBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region);

  /**
   * Compute horizontal speed resolution maneuver.
   * @parameter dir is up (true)/down (false) of ownship current horizontal speed
   * @return horizontal speed resolution in internal units [m/s] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double horizontalSpeedResolution(bool dir);

  /**
   * Compute horizontal speed resolution maneuver for corrective region.
   * @parameter dir is up (true)/down (false) of ownship current horizontal speed
   * @parameter u units
   * @return horizontal speed resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double horizontalSpeedResolution(bool dir, const std::string& u);

  /**
   * Compute preferred horizontal speed direction on resolution that is closer to current horizontal speed.
   * True: Increase speed, False: Decrease speed.
   */
  bool preferredHorizontalSpeedUpOrDown();

  /**
   * @return the number of vertical speed band intervals, negative if the ownship has not been set
   */
  int verticalSpeedBandsLength();

  /**
   * Force computation of vertical speed bands. Usually, bands are only computed when needed. This method
   * forces the computation of vertical speed bands (this method is included mainly for debugging purposes).
   */
  void forceVerticalSpeedBandsComputation();

  /**
   * @return the i-th interval, in internal units [m/s], of the computed vertical speed bands.
   * @param i index
   */
  Interval verticalSpeedIntervalAt(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed vertical speed bands.
   * @param i index
   * @param u units
   */
  Interval verticalSpeedIntervalAt(int i, const std::string& u);

  /**
   * @return the i-th region of the computed vertical speed bands.
   * @param i index
   */
  BandsRegion::Region verticalSpeedRegionAt(int i);

  /**
   * @return the region of a given vertical speed specified in internal units [m/s]
   * @param vs [m/s]
   */
  int indexOfVerticalSpeed(double vs);

  /**
   * @return the region of a given vertical speed specified in given units [u]
   * @param vs [u]
   * @param u Units
   */
  int indexOfVerticalSpeed(double vs, const std::string& u);

  /**
   * @return the region of a given vertical speed specified in internal units [m/s]
   * @param vs [m/s]
   */
  BandsRegion::Region regionOfVerticalSpeed(double vs);

  /**
   * @return the region of a given vertical speed specified in given units [u]
   * @param vs [u]
   * @param u Units
   */
  BandsRegion::Region regionOfVerticalSpeed(double vs, const std::string& u);

  /**
   * Return last time to vertical speed maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  double lastTimeToVerticalSpeedManeuver(const TrafficState& ac);

  /**
   * @return recovery information for vertical speed bands.
   */
  RecoveryInformation verticalSpeedRecoveryInformation();

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral vertical speed bands
   * for conflict bands region.
   * 1 = FAR, 2 = MID, 3 = NEAR
   */
  void peripheralVerticalSpeedBandsAircraft(std::vector<std::string>& acs, int region);

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral vertical speed bands
   * for conflict bands region.
   */
  void peripheralVerticalSpeedBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region);

  /**
   * Compute vertical speed resolution maneuver for given direction.
   * @parameter dir is up (true)/down (false) of ownship current vertical speed
   * @return vertical speed resolution in internal units [m/s] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double verticalSpeedResolution(bool dir);

  /**
   * Compute vertical speed resolution maneuver for given direction.
   * @parameter dir is up (true)/down (false) of ownship current vertical speed
   * @parameter u units
   * @return vertical speed resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double verticalSpeedResolution(bool dir, const std::string& u);

  /**
   * Compute preferred  vertical speed direction based on resolution that is closer to current vertical speed.
   * True: Increase speed, False: Decrease speed.
   */
  bool preferredVerticalSpeedUpOrDown();

  /**
   * @return the number of altitude band intervals, negative if the ownship has not been set.
   */
  int altitudeBandsLength();

  /**
   * Force computation of altitude bands. Usually, bands are only computed when needed. This method
   * forces the computation of altitude bands (this method is included mainly for debugging purposes).
   */
  void forceAltitudeBandsComputation();

  /**
   * @return the i-th interval, in internal units [m], of the computed altitude bands.
   * @param i index
   */
  Interval altitudeIntervalAt(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed altitude bands.
   * @param i index
   * @param u units
   */
  Interval altitudeIntervalAt(int i, const std::string& u);


  /**
   * @return the i-th region of the computed altitude bands.
   * @param i index
   */
  BandsRegion::Region altitudeRegionAt(int i);

  /**
   * @return the range index of a given altitude specified internal units [m]
   * @param alt [m]
   */
  int indexOfAltitude(double alt);

  /**
   * @return the range index of a given altitude specified in given units [u]
   * @param alt [u]
   * @param u Units
   */
  int indexOfAltitude(double alt, const std::string& u);

  /**
   * @return the region of a given altitude specified in internal units [m]
   * @param alt [m]
   */
  BandsRegion::Region regionOfAltitude(double alt);

  /**
   * @return the region of a given altitude specified in given units [u]
   * @param alt [u]
   * @param u Units
   */
  BandsRegion::Region regionOfAltitude(double alt, const std::string& u);

  /**
   * Return last time to altitude maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  double lastTimeToAltitudeManeuver(const TrafficState& ac);

  /**
   * @return recovery information for altitude speed bands.
   */
  RecoveryInformation altitudeRecoveryInformation();

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral altitude bands
   * for conflict bands region.
   * 1 = FAR, 2 = MID, 3 = NEAR
   */
  void peripheralAltitudeBandsAircraft(std::vector<std::string>& acs, int region);

  /**
   * Compute in acs list of aircraft identifiers contributing to peripheral altitude bands
   * for conflict bands region.
   */
  void peripheralAltitudeBandsAircraft(std::vector<std::string>& acs, BandsRegion::Region region);

  /**
   * Compute altitude resolution maneuver for given direction.
   * @parameter dir is up (true)/down (false) of ownship current altitude
   * @return altitude resolution in internal units [m] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double altitudeResolution(bool dir);

  /**
   * Compute altitude resolution maneuver for given direction.
   * @parameter dir is up (true)/down (false) of ownship current altitude
   * @parameter u units
   * @return altitude resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for early alerting time seconds. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double altitudeResolution(bool dir, const std::string& u);

  /**
   * Compute preferred  altitude direction on resolution that is closer to current altitude.
   * True: Climb, False: Descend.
   */
  bool preferredAltitudeUpOrDown();

  /*
   * Alerting logic
   */

  /**
   * Computes alerting type of ownship and aircraft at index ac_idx for current
   * aircraft states.  The number 0 means no alert. A negative number means
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
   */
  int alertLevel(int ac_idx, int turning, int accelerating, int climbing);

  /**
   * Computes alert level of ownship and aircraft at index idx.
   * The number 0 means no alert. A negative number means
   * that aircraft index is not valid.
   */
  int alertLevel(int ac_idx);

  /**
   * Detects violation of alert thresholds for a given alert level with an
   * aircraft at index ac_idx.
   * Conflict data provides time to violation and time to end of violation
   * of alert thresholds of given alert level.
   * @param ac_idx is the index of the traffic aircraft
   * @param alert_level alert level used to compute detection. The value 0
   * indicate the alert volume of the corrective region.
   */
  ConflictData violationOfAlertThresholds(int ac_idx, int alert_level);

  /**
   * Detects violation of alert thresholds for a given region with an
   * aircraft at index ac_idx.
   * Conflict data provides time to violation and time to end of violation
   * of alert thresholds of given alert level.
   * @param ac_idx is the index of the traffic aircraft
   * @param region region used to compute detection.
   */
  ConflictData violationOfAlertThresholds(int ac_idx, BandsRegion::Region region);

  /**
   * Detects violation of corrective thresholds with an aircraft at index ac_idx.
   * Conflict data provides time to violation and time to end of violation
   * @param ac_idx is the index of the traffic aircraft
   */
  ConflictData violationOfCorrectiveThresholds(int ac_idx);

  /**
   * @return time to corrective volume, in seconds, between ownship and aircraft at index idx, for the
   * corrective volume. The returned time is relative to current time. POSITIVE_INFINITY means no
   * conflict within lookahead time. NaN means aircraft index is out of range.
   * @param ac_idx is the index of the traffic aircraft
   */
  double timeToCorrectiveVolume(int ac_idx);

  /**
   * @return region corresponding to a given alert level for a particular aircraft.
   * This function first finds the alerter for this aircraft, based on ownship/intruder-centric
   * logic, then returns the configured region for the alerter level. It returns
   * UNKNOWN if the aircraft or the alert level are invalid.
   */
  BandsRegion::Region regionOfAlertLevel(int ac_idx, int alert_level);

  /**
   * @return alert_level corresponding to a given region for a particular aircraft.
   * This function first finds the alerter for this aircraft, based on ownship/intruder-centric
   * logic, then returns the configured region for the region. It returns -1
   * if the aircraft or the alert level are invalid.
   * 0 = NONE, 1 = FAR, 2 = MID, 3 = NEAR.
   */
  int alertLevelOfRegion(int ac_idx, int region);

  /**
   * @return alert_level corresponding to a given region for a particular aircraft.
   * This function first finds the alerter for this aircraft, based on ownship/intruder-centric
   * logic, then returns the configured region for the region. It returns -1
   * if the aircraft or its alerter are invalid.
   */
  int alertLevelOfRegion(int ac_idx, BandsRegion::Region region);

  /* Getting and Setting DaidalusParameters (note that setters stale the Daidalus object) */

  /* Input/Output methods */

  std::string outputStringAircraftStates() const;

  std::string rawString() const;

  std::string toString() const;

  std::string outputStringInfo();

  std::string outputStringAlerting();

  std::string outputStringDirectionBands();

  std::string outputStringHorizontalSpeedBands();

  std::string outputStringVerticalSpeedBands();

  std::string outputStringAltitudeBands();

  std::string outputStringLastTimeToManeuver();

  std::string outputString();

  std::string toPVS(bool parameters);

  std::string toPVS();

  // ErrorReporter Interface Methods

  bool hasError() const;

  bool hasMessage() const;

  std::string getMessage();

  std::string getMessageNoClear() const;

  // Deprecate interface methods

  /**
  @Deprecated
   * Use setOwnshipState instead.
   * Set ownship state at time 0.0. Clear all traffic.
   * @param id Ownship's identified
   * @param pos Ownship's position
   * @param vel Ownship's ground velocity
   */
  virtual void setOwnship(const std::string& id, const Position& pos, const Velocity& vel) {
    setOwnshipState(id,pos,vel);
  }

  /**
  @Deprecated
   * Use setOwnshipState instead.
   * Set ownship state at time 0.0. Clear all traffic. Name of ownship will be "Ownship"
   * @param pos Ownship's position
   * @param vel Ownship's ground velocity
   */
  virtual void setOwnship(const Position& pos, const Velocity& vel) {
    setOwnship("Ownship",pos,vel);
  }

  /**
  @Deprecated
   * Add traffic state at current time. If it's the first aircraft, this aircraft is
   * set as the ownship.
   * @param id Aircraft's identifier
   * @param pos Aircraft's position
   * @param vel Aircraft's ground velocity
   * Same function as addTrafficState, but it doesn't return index of added traffic. This is neeeded
   * for compatibility with GenericBands
   */
  void addTraffic(const std::string& id, const Position& pos, const Velocity& vel) {
    addTrafficState(id,pos,vel);
  }

  /**
  @Deprecated
   * Use addTrafficState instead
   * Add traffic state at current time. If it's the first aircraft, this aircraft is
   * set as the ownship. Name of aircraft is AC_n, where n is the index of the aicraft
   * @param pos Aircraft's position
   * @param vel Aircraft's ground velocity
   * Same function as addTrafficState, but it doesn't return index of added traffic.
   */
  void addTraffic(const Position& pos, const Velocity& vel) {
    if (!hasOwnship()) {
      setOwnship(pos,vel);
    } else {
      addTrafficState("AC_"+Fmi(core_.traffic.size()+1),pos,vel);
    }
  }

  /**
    @Deprecated
    Use alertLevel instead
   */
  int alerting(int ac_idx) {
    return alertLevel(ac_idx);
  }

  /**
  @Deprecated
   * Use setMaxHorizontalSpeed instead
   */
  virtual void setMaxGroundSpeed(double gs, const std::string& unit) {
    setMaxHorizontalSpeed(gs,unit);
  }

  /**
  @Deprecated
   * Use getMaxHorizontalSpeed instead
   */
  virtual double getMaxGroundSpeed(const std::string& unit) const {
    return getMaxHorizontalSpeed(unit);
  }

  /**
  @Deprecated
   * Use horizontalDirectionBandsLength instead
   */
  virtual int trackLength() {
    return horizontalDirectionBandsLength();
  }

  /**
    @Deprecated
   * Use horizontalDirectionIntervalAt instead
   */
  virtual Interval track(int i, const std::string& unit) {
    return horizontalDirectionIntervalAt(i,unit);
  }

  /**
    @Deprecated
   * Use horizontalDirectionRegionAt instead
   */
  virtual BandsRegion::Region trackRegion(int i) {
    return horizontalDirectionRegionAt(i);
  }

  /**
    @Deprecated
   * Use regionOfHorizontalDirection instead
   */
  virtual BandsRegion::Region regionOfTrack(double trk, const std::string& unit) {
    return regionOfHorizontalDirection(trk,unit);
  }

  /**
    @Deprecated
   * Use horizontalSpeedBandsLength instead
   */
  virtual int groundSpeedLength() {
    return horizontalSpeedBandsLength();
  }

  /**
    @Deprecated
   * Use horizontalSpeedIntervalAt instead
   */
  virtual Interval groundSpeed(int i, const std::string& unit) {
    return horizontalSpeedIntervalAt(i,unit);
  }

  /**
    @Deprecated
   * Use horizontalSpeedRegionAt instead
   */
  virtual BandsRegion::Region groundSpeedRegion(int i) {
    return horizontalSpeedRegionAt(i);
  }

  /**
    @Deprecated
   * Use regionOfHorizontalSpeed instead
   */
  virtual BandsRegion::Region regionOfGroundSpeed(double gs, const std::string& unit) {
    return regionOfHorizontalSpeed(gs,unit);
  }

  /**
    @Deprecated
   * Use verticalSpeedBandsLength instead
   */
  virtual int verticalSpeedLength() {
    return verticalSpeedBandsLength();
  }

  /**
    @Deprecated
   * Use verticalSpeedIntervalAt instead
   */
  virtual Interval verticalSpeed(int i, const std::string& unit) {
    return verticalSpeedIntervalAt(i,unit);
  }

  /**
    @Deprecated
   * Use verticalSpeedRegionAt instead
   */
  virtual BandsRegion::Region verticalSpeedRegion(int i) {
    return verticalSpeedRegionAt(i);
  }

};

}

#endif

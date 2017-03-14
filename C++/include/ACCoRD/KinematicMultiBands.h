/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef KINEMATICMULTIBANDS_H_
#define KINEMATICMULTIBANDS_H_

#include "Velocity.h"
#include "Position.h"
#include "Detection3D.h"
#include "Detection3DAcceptor.h"
#include "GenericStateBands.h"
#include "TrafficState.h"
#include "KinematicTrkBands.h"
#include "KinematicGsBands.h"
#include "KinematicVsBands.h"
#include "KinematicAltBands.h"
#include "ErrorLog.h"
#include "KinematicBandsCore.h"
#include "KinematicBandsParameters.h"

namespace larcfm {


/**
 * Objects of class "KinematicMultiBands" compute the conflict bands using 
 * kinematic single-maneuver projections of the ownship and linear preditions 
 * of (multiple) traffic aircraft positions. The bands consist of ranges of 
 * guidance maneuvers: track angles, ground speeds, vertical
 * speeds, and altitude.<p> 
 * 
 * An assumption of the bands information is that the traffic aircraft
 * do not maneuver. If the ownship immediately executes a NONE
 * guidance maneuver, then the new path is conflict free (within a
 * lookahead time of the parameter).  If the ownship immediately executes a
 * NEAR/MID/FAR guidance maneuver and no traffic aircraft maneuvers, then
 * there will corresponding alert within the corresponding alerting level thresholds.<p>
 *
 * If recovery bands are set via setRecoveryBands() and the ownship is in
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
 * KinematicMultiBands b = new KinematicMultiBands();
 * ...
 * b.clear();
 * b.setOwnship(position of ownship, velocity of ownship);
 * b.addTraffic(position of (one) traffic aircraft, velocity of traffic);
 * b.addTraffic(position of (another) traffic aircraft, velocity of traffic);
 * ...add other traffic aircraft...
 *
 * for (int i = 0; i < b.trackLength(); i++ ) {  
 *    intrval = b.track(i);
 *    lower_ang = intrval.low;
 *    upper_ang = intrval.up;
 *    regionType = b.trackRegion(i);
 *    ..do something with this information..
 * } 
 *
 * ...similar for ground speed and vertical speed...
 * </pre>
 *
 * When any "parameter" to this class is set (separation distance,
 * lookahead time, positions in latitude/longitude, etc.), all
 * previous bands information is cleared, though unlike 
 * instantaneous-maneuver bands, ownship and traffic state data is 
 * preserved in KineamaticBands unless explicitly cleared.  
 * For more complete example usage see the file <tt>Batch.java</tt>.  <p>
 *
 * Kinematic bands also have a set of "step size" parameters that determine 
 * the granularity of the search.  TrackStep indicates the maximum track 
 * resolution, GroundSpeedStep for ground speed, and VerticalSpeedStep for 
 * vertical speed.  These determine the output granularity, for 
 * example, if TrackStep is set to 1 degree, bands will be in 1 degree 
 * increments.<p>
 *
 * If the detection calculations discover a maneuver will cause the ownship to
 * enter an intruder's protected zone (e.g. a loss of separation, RA), then
 * all further maneuvers in that direction will be marked with a "near" band.
 * An example of this is if the ownship is moving with a track of 20 deg, and
 * a turn to the right would cause a loss of separation at the 50 deg point, 
 * then the "near" bands for track will at least contain the range of approximately 
 * 50-200 degrees (current track +180 deg, +/- the trackStep).<p>
 * 
 * Note that Bands outputs hold within a given (constant) frame of reference, with the default being
 * against a stationary Earth (i.e. GPS coordinates and velocities).  This means that
 * if bands are instead given wind-oriented input data (airspeed and  yaw-heading) for 
 * all aircraft, then the output maneuvers will be in the same frame of reference:
 * "groundSpeed" bands should then be read as "airSpeed" bands, and "track" should be
 * read as "heading".<p>
 *
 * Altitude bands assume assume an immediate maneuver to a given vertical speed, 
 * with a level off maneuver to various altitudes (based on altitudeStep, for example, 
 * every 500 ft). "NONE" bands here indicate no conflict during or immediately following 
 * such a maneuver, assuming all traffic aircraft continue at a constant velocity.  "NEAR"
 * bands indicate either a loss of separation during the climb/descent or a conflict after leveling 
 * off to that altitude.
 *
 */
class KinematicMultiBands : public ErrorReporter, public GenericStateBands {

protected:
  ErrorLog error;

public:

  KinematicBandsCore core_;
  KinematicTrkBands  trk_band_;
  KinematicGsBands   gs_band_;
  KinematicVsBands   vs_band_;
  KinematicAltBands  alt_band_;

  /**
   * Construct a KinematicMultiBands object with initial parameters and an empty list of detectors.
   */
  KinematicMultiBands(const KinematicBandsParameters& parameters);

  /**
   * Construct a KinematicMultiBands object with the default parameters and an empty list of detectors.
   */
  KinematicMultiBands();

  /**
   * Construct a KinematicMultiBands object from an existing object. This copies all traffic data.
   */
  KinematicMultiBands(const KinematicMultiBands& b);

  ~KinematicMultiBands();

  KinematicMultiBands& operator=(const KinematicMultiBands& b);

  /** Ownship and Traffic **/

  TrafficState const & getOwnship() const;

  // This function clears the traffic
  void setOwnship(const TrafficState& own);

  // This function clears the traffic
  void setOwnship(const std::string& id, const Position& p, const Velocity& v);

  // This function clears the traffic
  void setOwnship(const std::string& id, const Position& p, const Velocity& v, double time);

  // This function clears the traffic
  void setOwnship(const Position& p, const Velocity& v);

  void setTraffic(const std::vector<TrafficState>& traffic);

  std::vector<TrafficState> const & getTraffic() const;

  bool hasOwnship() const;

  bool hasTraffic() const;

  void addTraffic(const TrafficState& ac);

  void addTraffic(const std::string& id, const Position& pi, const Velocity& vi);

  void addTraffic(const Position& pi, const Velocity& vi);

  /* General Settings */

  /**
   * Set alert thresholds
   */
  void setAlertor(const AlertLevels& alertor);

  /**
   * Set bands parameters
   */
  void setKinematicBandsParameters(const KinematicBandsParameters& parameters);

  /**
   * @return recovery stability time in seconds. Recovery bands are computed at time of first green plus
   * this time.
   */
  double getRecoveryStabilityTime() const;

  /**
   * @return recovery stability time in specified units. Recovery bands are computed at time of first green plus
   * this time.
   */
  double getRecoveryStabilityTime(const std::string& u) const;

  /**
   * Sets recovery stability time in seconds. Recovery bands are computed at time of first green plus
   * this time.
   */
  void setRecoveryStabilityTime(double t);

  /**
   * Sets recovery stability time in specified units. Recovery bands are computed at time of first green plus
   * this time.
   */
  void setRecoveryStabilityTime(double t, const std::string& u);

  /**
   * @return minimum horizontal separation for recovery bands in internal units [m].
   */
  double getMinHorizontalRecovery() const;

  /**
   * Return minimum horizontal separation for recovery bands in specified units [u]
   */
  double getMinHorizontalRecovery(const std::string& u) const;

  /**
   * Sets minimum horizontal separation for recovery bands in internal units [m].
   */
  void setMinHorizontalRecovery(double val);

  /**
   * Set minimum horizontal separation for recovery bands in specified units [u].
   */
  void setMinHorizontalRecovery(double val, const std::string& u);

  /**
   * @return minimum vertical separation for recovery bands in internal units [m].
   */
  double getMinVerticalRecovery() const;

  /**
   * Return minimum vertical separation for recovery bands in specified units [u].
   */
  double getMinVerticalRecovery(const std::string& u) const;

  /**
   * Sets minimum vertical separation for recovery bands in internal units [m].
   */
  void setMinVerticalRecovery(double val);

  /**
   * Set minimum vertical separation for recovery bands in units
   */
  void setMinVerticalRecovery(double val, const std::string& u);

  /**
   * @return true if collision avoidance bands are enabled.
   */
  bool isEnabledCollisionAvoidanceBands() const;

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
  double getCollisionAvoidanceBandsFactor();

  /**
   * @return set factor for computing collision avoidance bands. Factor value is in (0,1]
   */
  void setCollisionAvoidanceBandsFactor(double val);

  /**
   * @return most urgent aircraft.
   */
  const TrafficState& getMostUrgentAircraft() const;

  /**
   * Set most urgent aircraft.
   */
  void setMostUrgentAircraft(const TrafficState& ac);

  /**
   * Set most urgent aircraft by identifier.
   */
  void setMostUrgentAircraft(const std::string& id);

  /**
   * @return true if repulsive criteria is enabled for conflict bands.
   */
  bool isEnabledConflictCriteria() const;

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
  bool isEnabledRecoveryCriteria() const;

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
   * Sets recovery bands flag for track, ground speed, and vertical speed bands to specified value.
   */
  void setRecoveryBands(bool flag);

  /**
   * Enables recovery bands for track, ground speed, and vertical speed.
   */
  void enableRecoveryBands();

  /**
   * Disable recovery bands for track, ground speed, and vertical speed.
   */
  void disableRecoveryBands();

  /* Track Bands Settings */

  /**
   * @return left track in radians [0 - pi] [rad] from current ownship's track
   */
  double getLeftTrack();

  /**
   * @return left track in specified units [0 - pi] [u] from current ownship's track
   */
  double getLeftTrack(const std::string& u);

  /**
   * @return right track in radians [0 - pi] [rad] from current ownship's track
   */
  double getRightTrack();

  /**
   * @return right track in specified units [0 - pi] [u] from current ownship's track
   */
  double getRightTrack(const std::string& u);

  /**
   * Set left track to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  void setLeftTrack(double val);

  /**
   * Set left track to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  void setLeftTrack(double val, const std::string& u);

  /**
   * Set right track to value in internal units [rad]. Value is expected to be in [0 - pi]
   */
  void setRightTrack(double val);

  /**
   * Set right track to value in specified units [u]. Value is expected to be in [0 - pi]
   */
  void setRightTrack(double val, const std::string& u);

  /**
   * Set absolute min/max tracks for bands computations. Tracks are specified in internal units [rad].
   * Values are expected to be in [0 - 2pi]
   */
  void setMinMaxTrack(double min, double max);

  /**
   * Set absolute min/max tracks for bands computations. Tracks are specified in given units [u].
   * Values are expected to be in [0 - 2pi] [u]
   */
  void setMinMaxTrack(double min, double max, const std::string& u);

  /**
   * @return step size for track bands in internal units [rad].
   */
  double getTrackStep() const;

  /**
   * @return step size for track bands in specified units [u].
   */
  double getTrackStep(const std::string& u) const;

  /**
   * Sets step size for track bands in internal units [rad].
   */
  void setTrackStep(double val);

  /**
   * Sets step size for track bands in specified units [u].
   */
  void setTrackStep(double val, const std::string& u);

  /**
   * @return bank angle in internal units [rad].
   */
  double getBankAngle() const;

  /**
   * @return bank angle in specified units [u].
   */
  double getBankAngle(const std::string& u) const;

  /**
   * Sets bank angle for track bands to value in internal units [rad]. As a side effect, this method
   * resets the turn rate.
   */
  void setBankAngle(double val);

  /**
   * Sets bank angle for track bands to value in specified units [u]. As a side effect, this method
   * resets the turn rate.
   */
  void setBankAngle(double val, const std::string& u);

  /**
   * @return turn rate in internal units [rad/s].
   */
  double getTurnRate() const;

  /**
   * @return turn rate in specified units [u].
   */
  double getTurnRate(const std::string& u) const;

  /**
   * Sets turn rate for track bands to value in internal units [rad/s]. As a side effect, this method
   * resets the bank angle.
   */
  void setTurnRate(double val);

  /**
   * Sets turn rate for track bands to value in specified units [u]. As a side effect, this method
   * resets the bank angle.
   */
  void setTurnRate(double val, const std::string& u);

  /**
   * @return true if recovery track bands are enabled.
   */
  bool isEnabledRecoveryTrackBands() const;

  /**
   * Sets recovery bands flag for track bands to specified value.
   */
  void setRecoveryTrackBands(bool flag);

  /* Ground Speed Bands Settings */

  /**
   * @return minimum ground speed for ground speed bands in internal units [m/s].
   */
  double getMinGroundSpeed();

  /**
   * @return minimum ground speed for ground speed bands in specified units [u].
   */
  double getMinGroundSpeed(const std::string& u);

  /**
   * Sets minimum ground speed for ground speed bands to value in internal units [m/s].
   */
  void setMinGroundSpeed(double val);

  /**
   * Sets minimum ground speed for ground speed bands to value in specified units [u].
   */
  void setMinGroundSpeed(double val, const std::string& u);

  /**
   * @return maximum ground speed for ground speed bands in internal units [m/s].
   */
  double getMaxGroundSpeed();

  /**
   * @return maximum ground speed for ground speed bands in specified units [u].
   */
  double getMaxGroundSpeed(const std::string& u);

  /**
   * Sets maximum ground speed for ground speed bands to value in internal units [m/s].
   */
  void setMaxGroundSpeed(double val);

  /**
   * Sets maximum ground speed for ground speed bands to value in specified units [u].
   */
  void setMaxGroundSpeed(double val, const std::string& u);

  /**
   * Set below/above ground speed, relative to ownship's ground speed, for bands computations.
   * Ground speeds are specified in internal units [m/s]. Below and above are expected to be
   * non-negative values.
   */
  void setBelowAboveGroundSpeed(double below, double above);

  /**
   * Set below/above ground speed, relative to ownship's ground speed, for bands computations.
   * Ground speeds are specified in given units [u]. Below and above are expected to be
   * non-negative values.
   */
  void setBelowAboveGroundSpeed(double below, double above, const std::string& u);

  /**
   * @return step size for ground speed bands in internal units [m/s].
   */
  double getGroundSpeedStep() const;

  /**
   * @return step size for ground speed bands in specified units [u].
   */
  double getGroundSpeedStep(const std::string& u) const;

  /**
   * Sets step size for ground speed bands to value in internal units [m/s].
   */
  void setGroundSpeedStep(double val);

  /**
   * Sets step size for ground speed bands to value in specified units [u].
   */
  void setGroundSpeedStep(double val, const std::string& u);

  /**
   * @return horizontal acceleration for ground speed bands to value in internal units [m/s^2].
   */
  double getHorizontalAcceleration() const;

  /**
   * @return horizontal acceleration for ground speed bands to value in specified units [u].
   */
  double getHorizontalAcceleration(const std::string& u) const;

  /**
   * Sets horizontal acceleration for ground speed bands to value in internal units [m/s^2].
   */
  void setHorizontalAcceleration(double val);

  /**
   * Sets horizontal acceleration for ground speed bands to value in specified units [u].
   */
  void setHorizontalAcceleration(double val, const std::string& u);

  /**
   * @return true if recovery ground speed bands are enabled.
   */
  bool isEnabledRecoveryGroundSpeedBands();

  /**
   * Sets recovery bands flag for ground speed bands to specified value.
   */
  void setRecoveryGroundSpeedBands(bool flag);

  /* Vertical Speed Bands Settings */

  /**
   * @return minimum vertical speed for vertical speed bands in internal units [m/s].
   */
  double getMinVerticalSpeed();

  /**
   * @return minimum vertical speed for vertical speed bands in specified units [u].
   */
  double getMinVerticalSpeed(const std::string& u);

  /**
   * Sets minimum vertical speed for vertical speed bands to value in internal units [m/s].
   */
  void setMinVerticalSpeed(double val);

  /**
   * Sets minimum vertical speed for vertical speed bands to value in specified units [u].
   */
  void setMinVerticalSpeed(double val, const std::string& u);

  /**
   * @return maximum vertical speed for vertical speed bands in internal units [m/s].
   */
  double getMaxVerticalSpeed();

  /**
   * @return maximum vertical speed for vertical speed bands in specified units [u].
   */
  double getMaxVerticalSpeed(const std::string& u);

  /**
   * Sets maximum vertical speed for vertical speed bands to value in internal units [m/s].
   */
  void setMaxVerticalSpeed(double val);

  /**
   * Sets maximum vertical speed for vertical speed bands to value in specified units [u].
   */
  void setMaxVerticalSpeed(double val, const std::string& u);

  /**
   * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations.
   * Vertical speeds are specified in internal units [m/s]. Below and above are expected to be
   * non-negative values.
   */
  void setBelowAboveVerticalSpeed(double below, double above);

  /**
   * Set below/above vertical speed, relative to ownship's vertical speed, for bands computations.
   * Vertical speeds are specified in given units [u]. Below and above are expected to be
   * non-negative values.
   */
  void setBelowAboveVerticalSpeed(double below, double above, const std::string& u);

  /**
   * @return step size for vertical speed bands in internal units [m/s].
   */
  double getVerticalSpeedStep() const;

  /**
   * @return step size for vertical speed bands in specified units [u].
   */
  double getVerticalSpeedStep(const std::string& u) const;

  /**
   * Sets step size for vertical speed bands to value in internal units [m/s].
   */
  void setVerticalSpeedStep(double val);

  /**
   * Sets step size for vertical speed bands to value in specified units [u].
   */
  void setVerticalSpeedStep(double val, const std::string& u);

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
   * @return true if recovery vertical speed bands are enabled.
   */
  bool isEnabledRecoveryVerticalSpeedBands();

  /**
   * Sets recovery bands flag for vertical speed bands to specified value.
   */
  void setRecoveryVerticalSpeedBands(bool flag);

  /* Altitude Bands Settings */

  /**
   * @return minimum altitude for altitude bands in internal units [m]
   */
  double getMinAltitude();

  /**
   * @return minimum altitude for altitude bands in specified units [u].
   */
  double getMinAltitude(const std::string& u);

  /**
   * Sets minimum altitude for altitude bands to value in internal units [m]
   */
  void setMinAltitude(double val);

  /**
   * Sets minimum altitude for altitude bands to value in specified units [u].
   */
  void setMinAltitude(double val, const std::string& u);

  /**
   * @return maximum altitude for altitude bands in internal units [m]
   */
  double getMaxAltitude();

  /**
   * @return maximum altitude for altitude bands in specified units [u].
   */
  double getMaxAltitude(const std::string& u);

  /**
   * Sets maximum altitude for altitude bands to value in internal units [m]
   */
  void setMaxAltitude(double val);

  /**
   * Sets maximum altitude for altitude bands to value in specified units [u].
   */
  void setMaxAltitude(double val, const std::string& u);

  /**
   * Set below/above altitude, relative to ownship's altitude, for bands computations.
   * Altitude are specified in internal units [m]. Below and above are expected to be
   * non-negative values.
   */
  void setBelowAboveAltitude(double below, double above);

  /**
   * Set below/above altitude, relative to ownship's altitude, for bands computations.
   * Altitudes are specified in given units [u]. Below and above are expected to be
   * non-negative values.
   */
  void setBelowAboveAltitude(double below, double above, const std::string& u);

  /**
   * @return step size for altitude bands in internal units [m]
   */
  double getAltitudeStep() const;

  /**
   * @return step size for altitude bands in specified units [u].
   */
  double getAltitudeStep(const std::string& u) const;

  /**
   * Sets step size for altitude bands to value in internal units [m]
   */
  void setAltitudeStep(double val);

  /**
   * Sets step size for altitude bands to value in specified units [u].
   */
  void setAltitudeStep(double val, const std::string& u);

  /**
   * @return the vertical climb/descend rate for altitude bands in internal units [m/s]
   */
  double getVerticalRate() const;

  /**
   * @return the vertical climb/descend rate for altitude bands in specified units [u].
   */
  double getVerticalRate(const std::string& u) const;

  /**
   * Sets vertical rate for altitude bands to value in internal units [m/s]
   */
  void setVerticalRate(double val);

  /**
   * Sets vertical rate for altitude bands to value in specified units [u].
   */
  void setVerticalRate(double val, const std::string& u);

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
   * @return true if recovery altitude bands are enabled.
   */
  bool isEnabledRecoveryAltitudeBands();

  /**
   * Sets recovery bands flag for altitude bands to specified value.
   */
  void setRecoveryAltitudeBands(bool flag);

  /* Utility methods */

  /**
   *  Clear ownship and traffic data from this object.
   */
  void clear();

protected:
  void reset();

private:

  /**
   * Return true if and only if threshold values, defining an alerting level, are violated.
   */
  bool check_thresholds(const AlertThresholds& athr, const TrafficState& ac, int turning, int accelerating, int climbing);

public:

  /* Main interface methods */

  /**
   *  Return list of conflict aircraft for a given alert level.
   *  Note: alert level are 1-indexed.
   */
  std::vector<TrafficState> const & conflictAircraft(int alert_level);

  /**
   * Return time interval of violation for given alert level
   * Requires: 1 <= alert_level <= alertor.mostSevereAlertLevel()
   */
  Interval const & timeIntervalOfViolation(int alert_level);

  /**
   * @return the number of track band intervals, negative if the ownship has not been set
   */
  int trackLength();

  /**
   * Force computation of track bands. Usually, bands are only computed when needed. This method
   * forces the computation of track bands (this method is included mainly for debugging purposes).
   */
  void forceTrackBandsComputation();

  /**
   * @return the i-th interval, in internal units [rad], of the computed track bands.
   * @param i index
   */
  Interval track(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed track bands.
   * @param i index
   * @param u units
   */
  Interval track(int i, const std::string& u);

  /**
   * @return the i-th region of the computed track bands.
   * @param i index
   */
  BandsRegion::Region trackRegion(int i);

  /**
   * @return the range index of a given track specified in internal units [rad]
   * @param trk [rad]
   */
  int trackRangeOf(double trk);

  /**
   * @return the range index of a given track specified in given units [u]
   * @param trk [u]
   * @param u Units
   */
  int trackRangeOf(double trk, const std::string& u);

  /**
   * @return the region of a given track specified in internal units [rad].
   * @param trk [rad]
   */
  BandsRegion::Region regionOfTrack(double trk);

  /**
   * @return the region of a given track specified in given units [u]
   * @param trk [u]
   * @param u Units
   */
  BandsRegion::Region regionOfTrack(double trk, const std::string& u);

  /**
   * Return last time to track maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  double lastTimeToTrackManeuver(const TrafficState& ac);

  /**
   * @return time to recovery using track bands. Return NaN when bands are not saturated or when
   * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
   */
  double timeToTrackRecovery();

  /**
   * @return list of aircraft responsible for peripheral track bands for a given alert level.
   * Note: alert level are 1-indexed.
   */
  std::vector<TrafficState> const &  peripheralTrackAircraft(int alert_level);

  /**
   * Compute track resolution maneuver for given alert level.
   * @parameter dir is right (true)/left (false) of ownship current track
   * @return track resolution in internal units [rad] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no resolution to the right, and negative infinity if there
   * is no resolution to the left.
   */
  double trackResolution(bool dir, int alert_level);

  /**
   * Compute track resolution maneuver for conflict alert level.
   * @parameter dir is right (true)/left (false) of ownship current track
   * @parameter u units
   * @return track resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no resolution to the right, and negative infinity if there
   * is no resolution to the left.
   */
  double trackResolution(bool dir, int alert_level, const std::string& u);

  /**
   * Compute track resolution maneuver for conflict alert level.
   * @parameter dir is right (true)/left (false) of ownship current track
   * @return track resolution in internal units [rad] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no resolution to the right, and negative infinity if there
   * is no resolution to the left.
   */
  double trackResolution(bool dir);

  /**
   * Compute track resolution maneuver for conflict alert level.
   * @parameter dir is right (true)/left (false) of ownship current track
   * @parameter u units
   * @return track resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no resolution to the right, and negative infinity if there
   * is no resolution to the left.
   */
  double trackResolution(bool dir, const std::string& u);

  /**
   * Compute preferred track direction, for given alert level,
   * based on resolution that is closer to current track.
   * True: Right. False: Left.
   */
  bool preferredTrackDirection(int alert_level);

  /**
   * Compute preferred track direction, for conflict alert level,
   * based on resolution that is closer to current track.
   * True: Right. False: Left.
   */
  bool preferredTrackDirection();

  /**
   * @return the number of ground speed band intervals, negative if the ownship has not been set
   */
  int groundSpeedLength();

  /**
   * Force computation of ground speed bands. Usually, bands are only computed when needed. This method
   * forces the computation of ground speed bands (this method is included mainly for debugging purposes).
   */
  void forceGroundSpeedBandsComputation();

  /**
   * @return the i-th interval, in internal units [m/s], of the computed ground speed bands.
   * @param i index
   */
  Interval groundSpeed(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed ground speed bands.
   * @param i index
   * @param u units
   */
  Interval groundSpeed(int i, const std::string& u);

  /**
   * @return the i-th region of the computed ground speed bands.
   * @param i index
   */
  BandsRegion::Region groundSpeedRegion(int i);

  /**
   * @return the range index of a given ground speed specified in internal units [m/s]
   * @param gs [m/s]
   */
  int groundSpeedRangeOf(double gs);

  /**
   * @return the range index of a given ground speed specified in given units [u]
   * @param gs [u]
   * @param u Units
   */
  int groundSpeedRangeOf(double gs, const std::string& u);

  /**
   * @return the region of a given ground speed specified in internal units [m/s]
   * @param gs [m/s]
   */
  BandsRegion::Region regionOfGroundSpeed(double gs);

  /**
   * @return the region of a given ground speed specified in given units [u]
   * @param gs [u]
   * @param u Units
   */
  BandsRegion::Region regionOfGroundSpeed(double gs, const std::string& u);

  /**
   * Return last time to ground speed maneuver, in seconds, for ownship with respect to traffic
   * aircraft ac. Return NaN if the ownship is not in conflict with aircraft ac within
   * lookahead time. Return negative infinity if there is no time to maneuver.
   */
  double lastTimeToGroundSpeedManeuver(const TrafficState& ac);

  /**
   * @return time to recovery using ground speed bands. Return NaN when bands are not saturated or when
   * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
   */
  double timeToGroundSpeedRecovery();

  /**
   * @return list of aircraft responsible for peripheral ground speed bands for a given alert level.
   * Note: alert level are 1-indexed.
   */
  std::vector<TrafficState> const & peripheralGroundSpeedAircraft(int alert_level);

  /**
   * Compute ground speed resolution maneuver for given alert level.
   * @parameter dir is up (true)/down (false) of ownship current ground speed
   * @return ground speed resolution in internal units [m/s] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double groundSpeedResolution(bool dir, int alert_level);

  /**
   * Compute ground speed resolution maneuver for given alert level.
   * @parameter dir is up (true)/down (false) of ownship current ground speed
   * @parameter u units
   * @return ground speed resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double groundSpeedResolution(bool dir, int alert_level, const std::string& u);

  /**
   * Compute ground speed resolution maneuver for conflict alert level.
   * @parameter dir is up (true)/down (false) of ownship current ground speed
   * @return ground speed resolution in internal units [m/s] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double groundSpeedResolution(bool dir);

  /**
   * Compute ground speed resolution maneuver for conflict alert level.
   * @parameter dir is up (true)/down (false) of ownship current ground speed
   * @parameter u units
   * @return ground speed resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double groundSpeedResolution(bool dir, const std::string& u);

  /**
   * Compute preferred ground speed direction, for given alert level,
   * based on resolution that is closer to current ground speed.
   * True: Increase speed, False: Decrease speed.
   */
  bool preferredGroundSpeedDirection(int alert_level);

  /**
   * Compute preferred  ground speed direction, for conflict alert level,
   * based on resolution that is closer to current ground speed.
   * True: Increase speed, False: Decrease speed.
   */
  bool preferredGroundSpeedDirection();

  /**
   * @return the number of vertical speed band intervals, negative if the ownship has not been set
   */
  int verticalSpeedLength();

  /**
   * Force computation of vertical speed bands. Usually, bands are only computed when needed. This method
   * forces the computation of vertical speed bands (this method is included mainly for debugging purposes).
   */
  void forceVerticalSpeedBandsComputation();

  /**
   * @return the i-th interval, in internal units [m/s], of the computed vertical speed bands.
   * @param i index
   */
  Interval verticalSpeed(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed vertical speed bands.
   * @param i index
   * @param u units
   */
  Interval verticalSpeed(int i, const std::string& u);

  /**
   * @return the i-th region of the computed vertical speed bands.
   * @param i index
   */
  BandsRegion::Region verticalSpeedRegion(int i);

  /**
   * @return the region of a given vertical speed specified in internal units [m/s]
   * @param vs [m/s]
   */
  int verticalSpeedRangeOf(double vs);

  /**
   * @return the region of a given vertical speed specified in given units [u]
   * @param vs [u]
   * @param u Units
   */
  int verticalSpeedRangeOf(double vs, const std::string& u);

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
   * @return time to recovery using vertical speed bands. Return NaN when bands are not saturated or when
   * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
   */
  double timeToVerticalSpeedRecovery();

  /**
   * @return list of aircraft responsible for peripheral vertical speed bands for a given alert level.
   * Note: alert level are 1-indexed.
   */
  std::vector<TrafficState> const & peripheralVerticalSpeedAircraft(int alert_level);

  /**
   * Compute vertical speed resolution maneuver for given alert level.
   * @parameter dir is up (true)/down (false) of ownship current vertical speed
   * @return vertical speed resolution in internal units [m/s] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double verticalSpeedResolution(bool dir, int alert_level);

  /**
   * Compute vertical speed resolution maneuver for given alert level.
   * @parameter dir is up (true)/down (false) of ownship current vertical speed
   * @parameter u units
   * @return vertical speed resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double verticalSpeedResolution(bool dir, int alert_level, const std::string& u);

  /**
   * Compute vertical speed resolution maneuver for conflict alert level.
   * @parameter dir is up (true)/down (false) of ownship current vertical speed
   * @return vertical speed resolution in internal units [m/s] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double verticalSpeedResolution(bool dir);

  /**
   * Compute vertical speed resolution maneuver for conflict alert level.
   * @parameter dir is up (true)/down (false) of ownship current vertical speed
   * @parameter u units
   * @return vertical speed resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double verticalSpeedResolution(bool dir, const std::string& u);

  /**
   * Compute preferred vertical speed direction, for given alert level,
   * based on resolution that is closer to current vertical speed.
   * True: Increase speed, False: Decrease speed.
   */
  bool preferredVerticalSpeedDirection(int alert_level);

  /**
   * Compute preferred  vertical speed direction, for conflict alert level,
   * based on resolution that is closer to current vertical speed.
   * True: Increase speed, False: Decrease speed.
   */
  bool preferredVerticalSpeedDirection();

  /**
   * @return the number of altitude band intervals, negative if the ownship has not been set.
   */
  int altitudeLength();

  /**
   * Force computation of altitude bands. Usually, bands are only computed when needed. This method
   * forces the computation of altitude bands (this method is included mainly for debugging purposes).
   */
  void forceAltitudeBandsComputation();

  /**
   * @return the i-th interval, in internal units [m], of the computed altitude bands.
   * @param i index
   */
  Interval altitude(int i);

  /**
   * @return the i-th interval, in specified units [u], of the computed altitude bands.
   * @param i index
   * @param u units
   */
  Interval altitude(int i, const std::string& u);

  /**
   * @return the i-th region of the computed altitude bands.
   * @param i index
   */
  BandsRegion::Region altitudeRegion(int i);

  /**
   * @return the range index of a given altitude specified internal units [m]
   * @param alt [m]
   */
  int altitudeRangeOf(double alt);

  /**
   * @return the range index of a given altitude specified in given units [u]
   * @param alt [u]
   * @param u Units
   */
  int altitudeRangeOf(double alt, const std::string& u);

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
   * @return time to recovery using altitude bands. Return NaN when bands are not saturated or when
   * recovery bands are not enabled. Return negative infinity when no recovery within max_recovery_time.
   */
  double timeToAltitudeRecovery();

  /**
   * @return list of aircraft responsible for peripheral altitude bands for a given alert level.
   * Note: alert level are 1-indexed.
   */
  std::vector<TrafficState> const & peripheralAltitudeAircraft(int alert_level);

  /**
   * Compute altitude resolution maneuver for given alert level.
   * @parameter dir is up (true)/down (false) of ownship current altitude
   * @return altitude resolution in internal units [m] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double altitudeResolution(bool dir, int alert_level);

  /**
   * Compute altitude resolution maneuver for given alert level.
   * @parameter dir is up (true)/down (false) of ownship current altitude
   * @parameter u units
   * @return altitude resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double altitudeResolution(bool dir, int alert_level, const std::string& u);

  /**
   * Compute altitude resolution maneuver for conflict alert level.
   * @parameter dir is up (true)/down (false) of ownship current altitude
   * @return altitude resolution in internal units [m] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double altitudeResolution(bool dir);

  /**
   * Compute altitude resolution maneuver for conflict alert level.
   * @parameter dir is up (true)/down (false) of ownship current altitude
   * @parameter u units
   * @return altitude resolution in specified units [u] in specified direction.
   * Resolution maneuver is valid for lookahead time. Return NaN if there is no conflict,
   * positive infinity if there is no up resolution, and negative infinity if there
   * is no down resolution.
   */
  double altitudeResolution(bool dir, const std::string& u);

  /**
   * Compute preferred altitude direction, for given alert level,
   * based on resolution that is closer to current altitude.
   * True: Climb, False: Descend.
   */
  bool preferredAltitudeDirection(int alert_level);

  /**
   * Compute preferred  altitude direction, for conflict alert level,
   * based on resolution that is closer to current altitude.
   * True: Climb, False: Descend.
   */
  bool preferredAltitudeDirection();

  /**
   * Computes alerting level beteween ownship and aircraft.
   * The number 0 means no alert. A negative number means that aircraft index is not valid.
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
  int alerting(const TrafficState& ac, int turning, int accelerating, int climbing);

  std::string toString() const;

  std::string outputStringInfo();

  std::string outputStringAlerting();

  std::string outputStringTrackBands();

  std::string outputStringGroundSpeedBands();

  std::string outputStringVerticalSpeedBands();

  std::string outputStringAltitudeBands();

  std::string outputStringLastTimeToManeuver();

  std::string outputString();

  std::string toPVS(int precision);

  void linearProjection(double offset);

  // ErrorReporter Interface Methods

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

  /**
   * @return lookahead time in seconds.
   */
  double getLookaheadTime() const;

  /**
   * @return lookahead time in specified units [u].
   */
  double getLookaheadTime(const std::string& u) const;

  /**
   * Sets lookahead time in seconds.
   */
  virtual void setLookaheadTime(double t);

  /**
   * Sets lookahead time in specified units [u].
   */
  void setLookaheadTime(double t, const std::string& u);

  bool isLatLon() const;

};
}
#endif

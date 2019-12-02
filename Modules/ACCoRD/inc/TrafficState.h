/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef TRAFFICSTATE_H_
#define TRAFFICSTATE_H_

#include "Position.h"
#include "Velocity.h"
#include "EuclideanProjection.h"
#include "SUMData.h"
#include "DaidalusParameters.h"
#include <string>

namespace larcfm {

/** Horizontal solution */
class TrafficState {

public:
  // This numeric type is used for index variables over vectors of TrafficState
  typedef std::vector<TrafficState>::size_type nat;

private:
  std::string id_;
  Position pos_;
  Velocity gvel_; // Ground velocity
  Velocity avel_; // Air velocity
  EuclideanProjection eprj_; // Projection
  int alerter_; // Index to alert levels used by this aircraft
  SUMData sum_; // SUM data
  Position posxyz_;  // Projected position
  Vect3    sxyz_; // 3-D Cartesion  position
  Velocity velxyz_; // Projected air velocity

public:

  /**
   * Create an non-valid aircraft state
   */
  TrafficState();

  static const TrafficState& INVALID();

private:
  /**
   * Create a traffic state that is not lat/lon
   * @param id Aircraft's identifier
   * @param pos Aircraft's position
   * @param vel Aircraft's ground velocity
   */
  TrafficState(const std::string& id, const Position& pos, const Velocity& vel);

  /**
   * Create a traffic state
   * @param id Aircraft's identifier
   * @param pos Aircraft's position
   * @param vel Aircraft's ground velocity
   * @param eprj Euclidean projection
   */
  TrafficState(const std::string& id, const Position& pos, const Velocity& vel,
      EuclideanProjection eprj,int alerter);

  /**
   * Apply Euclidean projection. Requires aircraft's position in lat/lon
   * @param eprj
   */
  void applyEuclideanProjection();

public:
  /**
   * Set aircraft as ownship
   */
  void setAsOwnship();

  /**
   * Make an ownship's aircraft
   * @param id Ownship's identifier
   * @param pos Ownship's position
   * @param vel Ownship's ground velocity
   */

  static TrafficState makeOwnship(const std::string& id, const Position& pos, const Velocity& vel);

  /**
   * Set aircraft as intruder of ownship
   */
  void setAsIntruderOf(const TrafficState& ownship);

  /**
   * Make intruder aircraft
   * @param id Intruder's identifier
   * @param pos Intruder's position
   * @param vel Intruder's ground velocity
   * @return
   */
  TrafficState makeIntruder(const std::string& id, const Position& pos, const Velocity& vel) const;

  /**
   * Set alerter index for this aircraft
   * @param alerter
   */
  void setAlerterIndex(int alerter);

  /**
   * @return aircraft index for this aircraft. This index is 1-based.
   */
  int getAlerterIndex() const;

  /**
   * Set wind velocity
   * @param wind_vector Wind velocity specified in the TO direction
   */
  void applyWindVector(const Velocity& wind_vector);

  /**
   * Return wind velocity in the to direction
   * @return
   */
  Velocity windVector() const;

  /**
   * Return Euclidean projection
   */
  const EuclideanProjection& getEuclideanProjection() const;

  const Vect3& get_s() const;

  const Velocity& get_v() const;

  Vect3 pos_to_s(const Position& p) const;

  Velocity vel_to_v(const Position& p,const Velocity& v) const;

  Velocity inverseVelocity(const Velocity& v) const;

  /**
   * Project aircraft state offset time, which can be positive or negative.
   * @param offset Offset time.
   * @return Projected aircraft, including wind velocities
   */
  TrafficState linearProjection(double offset) const;

  /**
   * Index of aircraft id in traffic list. If aircraft is not in the list, returns -1
   * @param traffic
   * @param id
   */
  static int findAircraftIndex(const std::vector<TrafficState>& traffic, const std::string& id);

  static std::string listToString(const std::vector<std::string>& traffic);

  std::string formattedHeader(const std::string& uxy, const std::string& ualt, const std::string& ugs, const std::string& uvs) const;

  std::string formattedTrafficState(const std::string& uxy, const std::string& ualt, const std::string& ugs, const std::string& uvs, double time) const;

  static std::string formattedTrafficList(const std::vector<TrafficState>& traffic,
      const std::string& uxy, const std::string& ualt, const std::string& ugs, const std::string& uvs, double time);

  std::string formattedTraffic(const std::vector<TrafficState>& traffic,
      const std::string& uxy, const std::string& ualt, const std::string& ugs, const std::string& uvs, double time) const;

  std::string toPVS() const;

  std::string listToPVSAircraftList(const std::vector<TrafficState>& traffic) const;

  static std::string listToPVSStringList(const std::vector<std::string>& traffic);

  /**
   * @return true if this is a valid aircraft state
   */
  bool isValid() const;

  /**
   * @return true if aircraft position is specified in lat/lon instead of x,y
   */
  bool isLatLon() const;

  /**
   * @return Aircraft's identifier
   */
  const std::string& getId() const;

  /**
   * @return Aircraft's position
   */
  const Position& getPosition() const;

  /**
   * @return Aircraft's air velocity
   */
  const Velocity& getAirVelocity() const;

  /**
   * @return Aircraft's velocity (can be ground or air depending on whether a wind vector was applied or not)
   */
  const Velocity& getGroundVelocity() const;

  /**
   * @return Aircraft's air velocity
   */
  const Velocity& getVelocity() const;

  const Position& positionXYZ() const;

  const Velocity& velocityXYZ() const;

  /**
   * Returns current horizontal direction in internal units [0 - 2pi] [rad] (clock-wise with respect to North)
   * Direction may be heading or track, depending on whether a wind vector was provided or not.
   */
  double horizontalDirection() const;

  /**
   * Returns current direction in given units [0 - 2pi] [u] (clock wise with respect to North)
   * Direction may be heading or track, depending on whether a wind vector was provided or not.
   */
  double horizontalDirection(const std::string& utrk) const;

  /**
   * Returns current horizontal speed in internal units.
   * Horizontal speed may be air speed or group speed, depending on whether a wind vector
   * was provided or not.
   */
  double horizontalSpeed() const;

  /**
   * Returns current horizontal speed in given units.
   * Horizontal speed may be air speed or group speed, depending on whether a wind vector
   * was provided or not.
   */
  double horizontalSpeed(const std::string& ugs) const;

  /**
   * Returns current vertical speed in internal units
   */
  double verticalSpeed() const;

  /**
   * Returns current vertical speed in given units
   */
  double verticalSpeed(const std::string& uvs) const;

  /**
   * Returns current altitude in internal units
   */
  double altitude() const;

  /**
   * Returns current altitude in given units
   */
  double altitude(const std::string& ualt) const;

  /**
   * @return SUM (Sensor Uncertainty Mitigation) data
   */
  const SUMData& sum() const;

  /**
   * s_EW_std: East/West position standard deviation in internal units
   * s_NS_std: North/South position standard deviation in internal units
   * s_EN_std: East/North position standard deviation in internal units
   */
  void setHorizontalPositionUncertainty(double s_EW_std, double s_NS_std, double s_EN_std);

  /**
   * sz_std : Vertical position standard deviation in internal units
   */
  void setVerticalPositionUncertainty(double sz_std);

  /**
   * v_EW_std: East/West velocity standard deviation in internal units
   * v_NS_std: North/South velocity standard deviation in internal units
   * v_EN_std: East/North velocity standard deviation in internal units
   */
  void setHorizontalVelocityUncertainty(double v_EW_std, double v_NS_std,  double v_EN_std);

  /**
   * vz_std : Vertical velocity standard deviation in internal units
   */
  void setVerticalSpeedUncertainty(double vz_std);

  /**
   * Set all uncertainties to 0
   */
  void resetUncertainty();

  double relativeHorizontalPositionError(const TrafficState& ac, const DaidalusParameters& parameters) const;

  double relativeVerticalPositionError(const TrafficState& ac, const DaidalusParameters& parameters) const;

private:
  static double weighted_z_score(double range, const DaidalusParameters& parameters);

public:
  double relativeHorizontalSpeedError(const TrafficState& ac, double s_err, const DaidalusParameters& parameters) const;

  double relativeVerticalSpeedError(const TrafficState& ac, const DaidalusParameters& parameters) const;

  bool sameId(const TrafficState& ac) const;

  std::string toString() const;

};
}

#endif

/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef TRAFFICSTATE_H_
#define TRAFFICSTATE_H_

#include "Position.h"
#include "Velocity.h"
#include "EuclideanProjection.h"

namespace larcfm {

/** Horizontal solution */
class TrafficState {

private:

  EuclideanProjection eprj_;
  std::string id_;
  Position pos_;
  Velocity vel_;
  Position posxyz_;
  Velocity velxyz_;
  double time_;

  TrafficState(const std::string& id, const Position& pos, const Velocity& vel, double time,
      const EuclideanProjection& eprj);

public:

  TrafficState();

  // This numeric type is used for index variables over vectors of TrafficState
  typedef std::vector<TrafficState>::size_type nat;

  double getTime() const;

  static const TrafficState INVALID;

  static const std::vector<TrafficState> INVALIDL;

  static TrafficState makeOwnship(const TrafficState& ac);

  static TrafficState makeOwnship(const std::string& id, const Position& pos, const Velocity& vel,
      double time=0.0);

  TrafficState makeIntruder(const TrafficState& ac);

  TrafficState makeIntruder(const std::string& id, const Position& pos, const Velocity& vel) const;

  Vect3 const & get_s() const;

  Velocity const & get_v() const;

  Vect3 pos_to_s(const Position& p) const;

  Velocity vel_to_v(const Position& p, const Velocity& v) const;

  Velocity inverseVelocity(const Velocity& v) const;

  TrafficState linearProjection(double offset) const;

  static TrafficState findAircraft(const std::vector<TrafficState>& traffic, const std::string& id);

  static std::string listToString(const std::vector<TrafficState>& traffic);

  std::string formattedHeader(const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const;

  std::string formattedTrafficState(const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const;

  static std::string formattedTrafficList(const std::vector<TrafficState>& traffic,
      const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs);

  std::string formattedTraffic(const std::vector<TrafficState>& traffic,
      const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const;

  std::string toPVS(int prec) const;

  std::string listToPVSAircraftList(const std::vector<TrafficState>& traffic, int prec) const;

  static std::string listToPVSStringList(const std::vector<TrafficState>& traffic, int prec);

  bool isValid() const;

  bool isLatLon() const;

  std::string getId() const;

  Position const & getPosition() const;

  Velocity const & getVelocity() const;

  Position const & getPositionXYZ() const;

  Velocity const & getVelocityXYZ() const;

  /**
   *  Returns current track in internal units [0 - 2pi] [rad] (clock wise with respect to North)
   */
  double track() const;

  /**
   *  Returns current track in given units [0 - 2pi] [u] (clock wise with respect to North)
   */
  double track(const std::string& utrk) const;

  /**
   * Returns current ground speed in internal units
   */
  double groundSpeed() const;

  /**
   * Returns current ground speed in given units
   */
  double groundSpeed(const std::string& ugs) const;

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

  bool sameId(const TrafficState& ac) const;

  std::string toString() const;

};
}

#endif

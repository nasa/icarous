/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef TRAFFICSTATE_H_
#define TRAFFICSTATE_H_

#include "Position.h"
#include "Velocity.h"
#include "TrafficCoreState.h"
#include "EuclideanProjection.h"

namespace larcfm {

/** Horizontal solution */
class TrafficState : public TrafficCoreState {

private:

  Vect3    s_; // Projected position
  Velocity v_; // Projected velocity
  EuclideanProjection eprj_;

  TrafficState(const std::string& id, const Position& pos, const Velocity& vel, const EuclideanProjection& eprj);

public:

  TrafficState();

  // This numeric type is used for index variables over vectors of TrafficState
  typedef std::vector<TrafficState>::size_type nat;

  static const TrafficState INVALID;

  static const std::vector<TrafficState> INVALIDL;

  static TrafficState makeOwnship(const std::string& id, const Position& pos, const Velocity& vel);

  TrafficState makeIntruder(const std::string& id, const Position& pos, const Velocity& vel) const;

  Vect3 const & get_s() const;

  Velocity const & get_v() const;

  EuclideanProjection const & get_eprj() const;

  Vect3 pos_to_s(const Position& p) const;

  Velocity vel_to_v(const Position& p, const Velocity& v) const;

  Velocity inverseVelocity(const Velocity& v) const;

  TrafficState linearProjection(double offset) const;

  static TrafficState findAircraft(const std::vector<TrafficState>& traffic, const std::string& id);

  static std::string listToString(const std::vector<TrafficState>& traffic);

  std::string formattedTraffic(const std::vector<TrafficState>& traffic, double time) const;

  std::string toPVS(int prec) const;

  std::string listToPVSAircraftList(const std::vector<TrafficState>& traffic, int prec) const;

  static std::string listToPVSStringList(const std::vector<TrafficState>& traffic, int prec);

};
}

#endif

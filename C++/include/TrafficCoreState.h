/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef TRAFFICCORESTATE_H_
#define TRAFFICCORESTATE_H_

#include <string>
#include <vector>
#include "Position.h"
#include "Velocity.h"

namespace larcfm {

/** Horizontal solution */
class TrafficCoreState {

private:

  std::string id_;
  Position pos_;
  Velocity vel_;

public:

  TrafficCoreState();
  TrafficCoreState(const std::string& id, const Position& pos, const Velocity& vel);
  TrafficCoreState(const TrafficCoreState& ac);

  static const TrafficCoreState INVALIDC;

  bool isValid() const;

  std::string getId() const;
  bool isLatLon() const;
  Position const & getPosition() const;
  Velocity const & getVelocity() const;

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

  bool sameId(const TrafficCoreState& ac) const;

  std::string toString() const;

};

bool operator<( const TrafficCoreState& lhs , const TrafficCoreState& rhs );

}
#endif

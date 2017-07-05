/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "TrafficCoreState.h"
#include <string>
#include "Position.h"
#include "Velocity.h"
#include "string_util.h"
#include "format.h"

namespace larcfm {

TrafficCoreState::TrafficCoreState() {
  id_ = "_NoAc_";
  pos_ = Position::INVALID();
  vel_ = Velocity::INVALIDV();
}

TrafficCoreState::TrafficCoreState(const std::string& i, const Position& p, const Velocity& v) {
  id_ = i;
  pos_ = p;
  vel_ = v;
}

TrafficCoreState::TrafficCoreState(const TrafficCoreState& ac) {
  id_ = ac.id_;
  pos_ = ac.pos_;
  vel_ = ac.vel_;
}

const TrafficCoreState TrafficCoreState::INVALIDC = TrafficCoreState();

bool TrafficCoreState::isValid() const {
  return !pos_.isInvalid() && !vel_.isInvalid();
}

std::string TrafficCoreState::getId() const {
  return id_;
}

bool TrafficCoreState::isLatLon() const {
  return pos_.isLatLon();
}

Position const & TrafficCoreState::getPosition() const {
  return pos_;
}

Velocity const & TrafficCoreState::getVelocity() const {
  return vel_;
}

/**
 *  Returns current track in internal units [0 - 2pi] [rad] (clock wise with respect to North)
 */
double TrafficCoreState::track() const {
  return vel_.compassAngle();
}

/**
 *  Returns current track in given units [0 - 2pi] [u] (clock wise with respect to North)
 */
double TrafficCoreState::track(const std::string& utrk) const {
  return vel_.compassAngle(utrk);
}

/**
 * Returns current ground speed in internal units
 */
double TrafficCoreState::groundSpeed() const {
  return vel_.gs();
}

/**
 * Returns current ground speed in given units
 */
double TrafficCoreState::groundSpeed(const std::string& ugs) const {
  return vel_.groundSpeed(ugs);
}

/**
 * Returns current vertical speed in internal units
 */
double TrafficCoreState::verticalSpeed() const {
  return vel_.vs();
}

/**
 * Returns current vertical speed in given units
 */
double TrafficCoreState::verticalSpeed(const std::string& uvs) const {
  return vel_.verticalSpeed(uvs);
}

/**
 * Returns current altitude in internal units
 */
double TrafficCoreState::altitude() const{
  return pos_.alt();
}

/**
 * Returns current altitude in given units
 */
double TrafficCoreState::altitude(const std::string& ualt) const {
  return Units::to(ualt,pos_.alt());
}

bool TrafficCoreState::sameId(const TrafficCoreState& ac) const {
  return isValid() && ac.isValid() && id_ == ac.id_;
}

std::string TrafficCoreState::toString() const {
  return "("+id_+", "+pos_.toString()+", "+vel_.toString()+")";
}

// global scope:

bool operator<( const TrafficCoreState& lhs , const TrafficCoreState& rhs )
{
  return lhs.getId() < rhs.getId();
}

}

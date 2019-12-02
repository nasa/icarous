/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * FixedAircraftUrgencyStrategy.cpp
 *
 * Most urgent aircraft strategy where the aircraft is fixed
 *
 */

#include "FixedAircraftUrgencyStrategy.h"

namespace larcfm {

FixedAircraftUrgencyStrategy::FixedAircraftUrgencyStrategy() {
  ac_ = TrafficState::INVALID().getId();
}

FixedAircraftUrgencyStrategy::FixedAircraftUrgencyStrategy(const std::string& id) {
  ac_ = id;
}

std::string FixedAircraftUrgencyStrategy::getFixedAircraftId() const {
  return ac_;
}

void FixedAircraftUrgencyStrategy::setFixedAircraftId(const std::string& id) {
  ac_ = id;
}

/**
 * @return index of aircraft id
 */
int FixedAircraftUrgencyStrategy::mostUrgentAircraft( const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) const {
  return TrafficState::findAircraftIndex(traffic,ac_);
}

UrgencyStrategy* FixedAircraftUrgencyStrategy::copy() const {
  return new FixedAircraftUrgencyStrategy(ac_);
}

}

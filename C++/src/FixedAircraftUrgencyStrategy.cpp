/*
 * Copyright (c) 2015-2016 United States Government as represented by
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
  ac_ = TrafficState::INVALID.getId();
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

TrafficState FixedAircraftUrgencyStrategy::mostUrgentAircraft(Detection3D* detector, const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) {
  return TrafficState::findAircraft(traffic,ac_);
}

UrgencyStrategy* FixedAircraftUrgencyStrategy::copy() const {
  return new FixedAircraftUrgencyStrategy(ac_);
}

}

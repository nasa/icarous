/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * UrgencyStrategy.cpp
 *
 * This strategy always returns an INVALID aircraft.
 *
 */

#include "NoneUrgencyStrategy.h"

namespace larcfm {

TrafficState NoneUrgencyStrategy::mostUrgentAircraft(Detection3D* detector, const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) {
  return TrafficState::INVALID;
}

UrgencyStrategy* NoneUrgencyStrategy::copy() const {
  return new NoneUrgencyStrategy();
}

}

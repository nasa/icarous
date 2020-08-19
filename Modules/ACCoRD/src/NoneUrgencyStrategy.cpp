/*
 * Copyright (c) 2015-2020 United States Government as represented by
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

/**
 * @return -1, which is not a valid aircraft index
 */
int NoneUrgencyStrategy::mostUrgentAircraft(const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) const {
  (void)ownship; //bypass unused parameter warning (needed for interface)
  (void)traffic; //bypass unused parameter warning (needed for interface)
  (void)T; //bypass unused parameter warning (needed for interface)
  return  -1;
}

UrgencyStrategy* NoneUrgencyStrategy::copy() const {
  return new NoneUrgencyStrategy();
}

}

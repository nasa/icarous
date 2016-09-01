/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * UrgencyStrategy.h
 *
 * Generic class for most urgent aircraft strategy.
 *
 */

#ifndef URGENCYSTRATEGY_H_
#define URGENCYSTRATEGY_H_

#include "Detection3D.h"
#include "TrafficState.h"

namespace larcfm {

class UrgencyStrategy {

public:
  UrgencyStrategy() {}
  virtual ~UrgencyStrategy() {}
  virtual TrafficState mostUrgentAircraft(Detection3D* detector, const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) = 0;
  virtual UrgencyStrategy* copy() const = 0;
};

}

#endif /* URGENCYSTRATEGY_H_ */

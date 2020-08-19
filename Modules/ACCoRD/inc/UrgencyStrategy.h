/*
 * Copyright (c) 2015-2020 United States Government as represented by
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


  /**
   * @return index of most urgent traffic aircraft for given ownship, traffic, and lookahead time T.
   * If index <= -1, then no aircraft is the most urgent
   */

  virtual int mostUrgentAircraft(const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) const = 0;
  virtual UrgencyStrategy* copy() const = 0;
};

}

#endif /* URGENCYSTRATEGY_H_ */

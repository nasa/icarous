/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * DCPAUrgencyStrategy.h
 *
 * Most urgent strategy based on distance at closest point of approach. When this distance is less than the minimum
 * recovery separation given by D and H, time to closest point of approach is used.
 *
 */

#ifndef DCPAURGENCYSTRATEGY_H_
#define DCPAURGENCYSTRATEGY_H_

#include "UrgencyStrategy.h"

namespace larcfm {

class DCPAUrgencyStrategy : public UrgencyStrategy {
public:

  /**
   * @return most urgent traffic aircraft for given ownship, traffic and lookahead time T.
   * Return -1 if no aircraft is most urgent.
   */
  int mostUrgentAircraft(const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) const;
  UrgencyStrategy* copy() const;
};

}

#endif /* DCPAURGENCYSTRATEGY_H_ */

/*
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * NoneUrgencyStrategy.h
 *
 * This strategy always returns an INVALID aircraft.
 *
 */

#ifndef NONEURGENCYSTRATEGY_H_
#define NONEURGENCYSTRATEGY_H_

#include "UrgencyStrategy.h"

namespace larcfm {

class NoneUrgencyStrategy : public UrgencyStrategy {

public:

  /**
   * @return -1, which is not a valid aircraft index
   */
  int mostUrgentAircraft(const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T) const;
  UrgencyStrategy* copy() const;
};

}

#endif /* NONEURGENCYSTRATEGY_H_ */

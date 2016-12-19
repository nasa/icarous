/*
 * Copyright (c) 2015-2016 United States Government as represented by
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
   * @return INVALID aircraft
   */
  TrafficState mostUrgentAircraft(Detection3D* detector, const TrafficState& ownship, const std::vector<TrafficState>& traffic, double T);
  UrgencyStrategy* copy() const;
};

}

#endif /* NONEURGENCYSTRATEGY_H_ */

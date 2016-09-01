/* 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef BANDSRANGE_H_
#define BANDSRANGE_H_

#include "Interval.h"
#include "BandsRegion.h"

namespace larcfm {

class BandsRange {
public:
  Interval    interval;
  BandsRegion::Region region;
  BandsRange(const Interval& i, BandsRegion::Region r);
  std::string toString() const;
};

}
#endif

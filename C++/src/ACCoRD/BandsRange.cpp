/* 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BandsRange.h"
#include "format.h"
#include "Constants.h"

namespace larcfm {

BandsRange::BandsRange(const Interval& i, BandsRegion::Region r) {
  interval = i;
  region = r;
}
std::string BandsRange::toString() const {
  int precision = Constants::get_output_precision();
  std::string s = "";
  s+=interval.toString(precision)+" "+BandsRegion::to_string(region);
  return s;

}

}

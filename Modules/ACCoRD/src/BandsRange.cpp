/* 
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BandsRange.h"
#include "format.h"

namespace larcfm {

BandsRange::BandsRange(const Interval& i, BandsRegion::Region r) {
  interval = i;
  region = r;
}

std::string BandsRange::toString() const {
  std::string s = "";
  s+=interval.toString()+" "+BandsRegion::to_string(region);
  return s;
}

std::string BandsRange::toPVS() const {
  std::string s = "";
  s += "(# lb:= "+FmPrecision(interval.low)+
          ", ub:= "+FmPrecision(interval.up)+
          ", region:= "+BandsRegion::to_string(region)+" #)";
  return s;
}

}

/* 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "DaidalusParameters.h"
#include "BandsRange.h"
#include "format.h"

namespace larcfm {

BandsRange::BandsRange(const Interval& i, BandsRegion::Region r) {
  interval = i;
  region = r;
}

// NONE's when in recovery, become RECOVERY
BandsRegion::Region BandsRange::resolution_region(BandsRegion::Region region, bool recovery) {
  if (BandsRegion::isResolutionBand(region)) {
    return recovery ? BandsRegion::RECOVERY : BandsRegion::NONE;
  }
  return region;
}

/*
 * Transforms a list of color values into a list of BandRanges.
 * This function avoids adding color points where the left and right colors are the same.
 */
void BandsRange::makeRangesFromColorValues(std::vector<BandsRange>& ranges, const std::vector<ColorValue>& l, bool recovery) {
  ranges.clear();
  int next=0;
  for (int i = 1; i < static_cast<int>(l.size()); ++i) {
    BandsRegion::Region color_left = resolution_region(l.at(i).color_left,recovery);
    BandsRegion::Region color_right = resolution_region(l.at(i).color_right,recovery);
    if (color_left != color_right || i == static_cast<int>(l.size()-1)) {
      ranges.push_back(BandsRange(Interval(l.at(next).val,l.at(i).val),color_left));
      next = i;
    }
  }
}

/**
 * Return index in ranges_ where val is found, -1 if invalid input or not found. Notice that values at
 * the limits may be included or not depending on the regions.
 */
int BandsRange::index_of(const std::vector<BandsRange>& ranges, double val, double mod) {
  val = Util::safe_modulo(val,mod);
  int last_index = ranges.size()-1;
  // In the following loop is important that RECOVERY and NONE both have the order 0,
  // since they represent close intervals.
  // For that reason, orderOfConflictRegion is used instead of orderOfRegion
  for (int i=0; i <= last_index; ++i) {
    if (ranges[i].interval.almost_in(val,true,true,DaidalusParameters::ALMOST_) ||
        (i==last_index &&
            Util::almost_equals(Util::safe_modulo(ranges[i].interval.up-val,mod),0,
                DaidalusParameters::ALMOST_))) {
      if (BandsRegion::isResolutionBand(ranges[i].region)) {
        return i;
      } else if (Util::almost_equals(val,ranges[i].interval.low,DaidalusParameters::ALMOST_)) {
        int prev_index = -1;
        if (i > 0) {
          prev_index = i-1;
        } else if (mod > 0) {
          prev_index = last_index;
        }
        if (prev_index > 0 && BandsRegion::isValidBand(ranges[i].region)) {
          return !BandsRegion::isValidBand(ranges[prev_index].region) ||
              BandsRegion::orderOfConflictRegion(ranges[i].region) <=
              BandsRegion::orderOfConflictRegion(ranges[prev_index].region) ? i : prev_index;
        }
        return prev_index;
      } else if (Util::almost_less(val,ranges[i].interval.up,DaidalusParameters::ALMOST_)) {
        return i;
      } else {
        // val is equal to upper bound
        int next_index = -1;
        if (i < last_index) {
          next_index = i+1;
        } else if (mod > 0) {
          next_index = 0;
        }
        if (next_index > 0 && BandsRegion::isValidBand(ranges[i].region)) {
          return
              !BandsRegion::isValidBand(ranges[next_index].region) ||
              BandsRegion::orderOfConflictRegion(ranges[i].region) <=
              BandsRegion::orderOfConflictRegion(ranges[next_index].region) ? i : next_index;
        }
        return next_index;
      }
    }
  }
  return -1;
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
